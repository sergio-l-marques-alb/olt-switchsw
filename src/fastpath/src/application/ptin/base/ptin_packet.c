/**
 * ptin_packet.c
 *  
 * Implements the Packet capture and process routines
 *
 * Created on: 2013/05/22 
 * Author: Milton Ruas (milton-r-silva@ext.ptinovacao.pt)
 * Notes: 
 */

#include "ptin_packet.h"
#include "ptin_evc.h"
#include "sysnet_api_ipv4.h"

#include "usmdb_util_api.h"
#include "fdb_api.h"

#include <unistd.h>


/* Maximum number of flooding vlans per client */
#define MAX_FLOOD_VLANS   8

/* Maximum number of messages in queue */
#define PTIN_PACKET_MAX_MESSAGES  2048

/* Message id used in queue */
#define PTIN_PACKET_RECEIVE     1
#define PTIN_PACKET_TRANSMIT    2

/* PDU Message format */
typedef struct ptin_PDU_Msg_s
{
  L7_uint32        msgId;         /* Of type snoopMgmtMessages_t */
  L7_uint32        intIfNum;      /*Interface on which PDU was received */
  L7_uint32        vlanId;        /*VLAN on which PDU was received */
  L7_uint32        innerVlanId;   /*Inner VLAN if present */
  L7_uchar8       *payload;       /* Pointer to the received PDU */
  L7_uint32        payloadLen;    /* Length of received PDU */
  L7_netBufHandle  bufHandle;     /* Buffer handle */
} ptin_PDU_Msg_t;
#define PTIN_PDU_MSG_SIZE   sizeof(ptin_PDU_Msg_t)

/* Broadcast MAC address */
const L7_uchar8 BroadcastMacAddr[L7_MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};


/*************** 
 * PROTOTYPES
 ***************/

/* Queue and Task id */
L7_uint32 ptin_packet_TaskId = L7_ERROR;
void     *ptin_packet_queue  = L7_NULLPTR;


/* Task to process messages */
void ptin_packet_task(void);

/* Process broadcast packet */
L7_RC_t ptinMacBcastProcess( ptin_PDU_Msg_t *pktMsg );


/*****************
 * DEBUG ROUTINES
 *****************/

L7_BOOL ptin_packet_debug_enable = L7_FALSE;

void ptin_packet_debug( L7_BOOL enable)
{
  ptin_packet_debug_enable = enable & 1;
}


/**********************
 * EXSTERNAL ROUTINES
 **********************/

/**
 * Initialize ptin_packet module
 * 
 * @author mruas (5/23/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_packet_init(void)
{
  sysnetNotifyEntry_t snEntry;

  /* Queue that will process timer events */
  ptin_packet_queue = (void *) osapiMsgQueueCreate("PTin_Packet_Queue",
                               PTIN_PACKET_MAX_MESSAGES, PTIN_PDU_MSG_SIZE);
  if (ptin_packet_queue == L7_NULLPTR)
  {
    PT_LOG_FATAL(LOG_CTX_PACKET,"PTIN packet queue creation error.");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_PACKET,"PTIN packet queue created.");

  /* Create task for packets management */
  ptin_packet_TaskId = osapiTaskCreate("ptin_packet_task", ptin_packet_task, 0, 0,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                        L7_DEFAULT_TASK_SLICE);

  if (ptin_packet_TaskId == L7_ERROR)
  {
    PT_LOG_FATAL(LOG_CTX_PACKET, "Could not create task ptin_packet_task");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_PACKET,"Task ptin_packet_task created");

  if (osapiWaitForTaskInit (L7_PTIN_PACKET_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_PACKET,"Unable to initialize ptin_packet_task()\n");
    return(L7_FAILURE);
  }
  PT_LOG_TRACE(LOG_CTX_PACKET,"Task ptin_packet_task initialized");

  PT_LOG_INFO(LOG_CTX_STARTUP,"Going to register ptinMacBcastRecv related to type=%u: 0x%08x",
           SYSNET_MAC_ENTRY, (L7_uint32) ptinMacBcastRecv);

  /* Register broadcast packets */
  strcpy(snEntry.funcName, "ptinBcastPduReceive");
  snEntry.notify_pdu_receive = ptinMacBcastRecv;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, BroadcastMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PACKET, "Cannot register ptinBcastPduReceive callback!");
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_PACKET, "ptinBcastPduReceive registered!");

  return L7_SUCCESS;
}

/**
 * DEInitialize ptin_packet module
 * 
 * @author mruas (5/23/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_packet_deinit(void)
{
  sysnetNotifyEntry_t snEntry;

  /* Deregister broadcast packets capture */
  strcpy(snEntry.funcName, "ptinBcastPduReceive");
  snEntry.notify_pdu_receive = ptinMacBcastRecv;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, BroadcastMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetDeregisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_PACKET, "Cannot unregister ptinBcastPduReceive callback!");
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_PACKET, "ptinBcastPduReceive unregistered!");

  /* Delete task */
  if ( ptin_packet_TaskId != L7_ERROR )
  {
    osapiTaskDelete(ptin_packet_TaskId);
    ptin_packet_TaskId = L7_ERROR;
  }

  /* Queue that will process timer events */
  if (ptin_packet_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(ptin_packet_queue);
    ptin_packet_queue = L7_NULLPTR;
  }

  PT_LOG_INFO(LOG_CTX_PACKET, "PTin packet deinit OK");

  return L7_SUCCESS;
}

/**
 * Callback to be called for bradcast packets. Packets will be 
 * flood for a specific list of vlans.
 * 
 * @param bufHandle 
 * @param pduInfo 
 * 
 * @return L7_RC_t : L7_FAILURE (always)
 */
L7_RC_t ptinMacBcastRecv(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8 *payload;
  L7_uint32 payloadLen;
  ptin_PDU_Msg_t msg;

  L7_uint32 intIfNum    = pduInfo->intIfNum;     /* Source port */
  L7_uint16 vlanId      = pduInfo->vlanId;       /* Vlan */
  L7_uint16 innerVlanId = pduInfo->innerVlanId;  /* Inner vlan */

  L7_RC_t rc = L7_SUCCESS;


  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX )
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"Invalid vlanId %u",vlanId);
    return L7_FAILURE;
  }
  /* Validate inner vlan id */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"Invalid innerVlanId %u",innerVlanId);
    return L7_FAILURE;
  }

  /* Packet should be broadcast type */
  if ( memcmp(&payload[0], BroadcastMacAddr, L7_MAC_ADDR_LEN)!=0 )
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"Packet is not broadcast");
    return L7_FAILURE;
  }

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"intIfNum %u and vlan %u does not belong to any valid EVC/interface");
    return L7_FAILURE;
  }

  /* Send packet to queue */
  memset(&msg, 0x00, sizeof(msg));
  msg.msgId       = PTIN_PACKET_RECEIVE;
  msg.intIfNum    = pduInfo->intIfNum;
  msg.vlanId      = pduInfo->vlanId;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.payload     = payload;
  msg.payloadLen  = payloadLen;
  msg.bufHandle   = bufHandle;

  rc = osapiMessageSend(ptin_packet_queue, &msg, PTIN_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"Failed message sending to ptin_packet queue");
    return L7_FAILURE;
  }

  /* Return failure to guarantee these packets are consumed by other entities */
  return L7_FAILURE;
}


/**
 * Task that makes the timer processing for the clients manageme
 */
void ptin_packet_task(void)
{
  L7_uint32 status;
  ptin_PDU_Msg_t msg;

  PT_LOG_INFO(LOG_CTX_PACKET,"PTin packet process task started");

  if (osapiTaskInitDone(L7_PTIN_PACKET_TASK_SYNC)!=L7_SUCCESS)
  {
    PT_LOG_FATAL(LOG_CTX_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  PT_LOG_INFO(LOG_CTX_PACKET,"PTin packet task ready to process events");

  /* Loop */
  while (1)
  {
    status = (L7_uint32) osapiMessageReceive(ptin_packet_queue,
                                             (void*) &msg,
                                             PTIN_PDU_MSG_SIZE,
                                             L7_WAIT_FOREVER);

    /* TODO: Process message */
    if (status == L7_SUCCESS)
    {
      if ( msg.msgId == PTIN_PACKET_RECEIVE )
      {
        if (ptinMacBcastProcess(&msg)!=L7_SUCCESS)
        {
          if (ptin_packet_debug_enable)
            PT_LOG_ERR(LOG_CTX_PACKET,"Error processing message");
        }
      }
      else if ( msg.msgId == PTIN_PACKET_TRANSMIT )
      {
        if (ptin_packet_frame_l2forward(msg.intIfNum, msg.vlanId, msg.innerVlanId, msg.payload, msg.payloadLen) != L7_SUCCESS)
        {
          if (ptin_packet_debug_enable)
            PT_LOG_ERR(LOG_CTX_PACKET,"Error processing message");
        }
      }
      else
      {
        if (ptin_packet_debug_enable)
          PT_LOG_ERR(LOG_CTX_PACKET,"Message id is unknown (%u)",msg.msgId);
      }
    }
    else
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET,"Failed packet reception from ptin_packet queue (status = %d)",status);
    }
  }
}


/**
 * Process broadcast packet
 * 
 * @param pktMsg : Packet information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptinMacBcastProcess( ptin_PDU_Msg_t *pktMsg )
{
  L7_uint         i;
  L7_uint32       intf;
  NIM_INTF_MASK_t intfList;

  L7_uint16   number_of_vlans;
  L7_uint16   flood_vlan[MAX_FLOOD_VLANS];

  L7_uint16 vlanId      = pktMsg->vlanId;       /* Vlan */
  L7_uint16 innerVlanId = pktMsg->innerVlanId;  /* Inner vlan */

  /* Get list of leaf interfaces */
  if (ptin_evc_intfType_getList(vlanId, PTIN_EVC_INTF_LEAF, &intfList)!=L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"Error getting list of leaf interfaces for vlanId=%u", vlanId);
    return L7_FAILURE;
  }

  /* Forward frame to all interfaces in this VLAN with multicast routers attached */
  for (intf = 1; intf <= L7_MAX_INTERFACE_COUNT; intf++)
  {
    /* Skip not active ports */
    if ( !(L7_INTF_ISMASKBITSET(intfList, intf)) )
    {
      continue;
    }

    /* Getting list of vlans */
    number_of_vlans = MAX_FLOOD_VLANS;
    if ( ptin_evc_flood_vlan_get( intf, vlanId, innerVlanId,
                                  flood_vlan, L7_NULLPTR, &number_of_vlans) != L7_SUCCESS )
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET,"Error getting list of vlans for vlanId=%u, innervlan=%u, intIfNum=%u", vlanId, innerVlanId, intf);
      continue;
    }

    if (ptin_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_PACKET,"%u vlans obtained for intIfNum %u, vlanId=%u, cvlan=%u", number_of_vlans, intf, vlanId, innerVlanId);

    /* Transmit packet to all vlans of this interface */
    for (i=0; i<number_of_vlans; i++)
    {
      if (ptin_packet_debug_enable)
        PT_LOG_TRACE(LOG_CTX_PACKET,"Sending packet to intIfNum %u, with oVlan=%u", intf, flood_vlan[i]);

      ptin_packet_send(intf, vlanId, flood_vlan[i], pktMsg->payload, pktMsg->payloadLen);
    }
  }

  return L7_SUCCESS;
}



/**********************
 * INTERNAL ROUTINES
 **********************/

/*********************************************************************
* @purpose  Send a multicast packet on a specified interface and vlan
*
* @param    intIfNum   @b{(input)} Outgoing internal interface number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ptin_packet_send(L7_uint32 intIfNum,
                      L7_uint32 vlanId,
                      L7_uint32 new_vlanId,
                      L7_uchar8 *payload,
                      L7_uint32 payloadLen)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  DTL_CMD_TX_INFO_t dtlCmd;


  /* If outgoing interface is CPU interface, don't send it */
  if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) )
  {
    return;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, payload, payloadLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, payloadLen);

  /* Change outer vlan */
  dataStart[14] &= 0xf0;
  dataStart[14] |= (new_vlanId >> 8) & 0x0f;
  dataStart[15]  = new_vlanId & 0xff;

  memset((L7_uchar8 *)&dtlCmd, 0, sizeof(DTL_CMD_TX_INFO_t));

  dtlCmd.intfNum             = intIfNum;
  dtlCmd.priority            = 1;
  dtlCmd.typeToSend          = DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;
  dtlCmd.cmdType.L2.vlanId   = vlanId;
  dtlCmd.cmdType.L2.flags    = 0;

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET,"Packet transmited to intIfNum=%u", intIfNum);

  return;
}




static L7_RC_t ptin_packet_frame_unicast(L7_uint32 outgoingIf,
                                         L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 vport_id,
                                         L7_uchar8 *frame, L7_ushort16 frameLen);

static L7_RC_t ptin_packet_frame_flood(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                                       L7_uchar8 *frame, L7_ushort16 frameLen);

static L7_RC_t ptin_packet_frame_send(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 vport_id,
                                      L7_uchar8 *frame, L7_ushort16 frameLen);

/*********************************************************************
* @purpose  Forward a packet (L2 switching).
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ptin_packet_frame_l2forward(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                                    L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_enetHeader_t *eth_header = (L7_enetHeader_t *)frame;
  L7_uchar8 vidMac[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t fdbEntry;

  /* combine mac and vlanid to get an 8-byte vidMac address */
  memset(vidMac, 0, L7_FDB_KEY_SIZE);
  (void)usmDbEntryVidMacCombine(vlanId, eth_header->dest.addr, vidMac);

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET,"VidMAC=%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x",
              vidMac[0], vidMac[1], vidMac[2], vidMac[3], vidMac[4], vidMac[5], vidMac[6], vidMac[7]);

  memset(&fdbEntry, 0, sizeof(fdbEntry));
  if(L7_SUCCESS == fdbFind(vidMac, L7_MATCH_EXACT, &fdbEntry))
  {
    return ptin_packet_frame_unicast(fdbEntry.dot1dTpFdbPort, vlanId, innerVlanId, fdbEntry.dot1dTpFdbVirtualPort, frame, frameLen);
  }

  /* If Destination mac is not found in FDB table, flood the ARP Request/Reply
   * packet to other ports in the VLAN as is done in the h/w */

  return ptin_packet_frame_flood(intIfNum, vlanId, innerVlanId, frame, frameLen);
}

L7_RC_t ptin_packet_frame_l2forward_nonblocking(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                                                L7_uchar8 *frame, L7_ushort16 frameLen)
{
  ptin_PDU_Msg_t msg;
  L7_RC_t rc;

  /* Check if initializations were properly done! */
  if (ptin_packet_TaskId == L7_ERROR || ptin_packet_queue == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_PACKET,"Task or Queue not initialized!");
    return L7_FAILURE;
  }

  memset(&msg, 0x00, sizeof(ptin_PDU_Msg_t));
  msg.msgId       = PTIN_PACKET_TRANSMIT;
  msg.intIfNum    = intIfNum;
  msg.vlanId      = vlanId;
  msg.innerVlanId = innerVlanId;
  msg.payload     = frame;
  msg.payloadLen  = frameLen;

  rc = osapiMessageSend(ptin_packet_queue, &msg, sizeof(ptin_PDU_Msg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_PACKET,"Error scheduling a transmission based on src intIfNum %u, vlanId %u, innerVlanId %u",
              intIfNum, vlanId, innerVlanId);
  }

  return rc;
}

/***********************************************************************
* @purpose Unicast transmission
*
* @param    outgoingIf   @b{(input)} outgoing interface number
* @param    vlanId       @b{(input)} VLAN ID
* @param    innerVlanId  @b{(input)} Inner VLAN ID
* @param    vport_id     @b{(input)} Virtual Port id
* @param    frame        @b{(input)} ethernet frame
* @param    frameLen     @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes
*
* @end
*
***********************************************************************/
static L7_RC_t ptin_packet_frame_unicast(L7_uint32 outgoingIf,
                                         L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 vport_id,
                                         L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_RC_t rc = L7_FAILURE;
  NIM_INTF_MASK_t portMask;
  L7_INTF_TYPES_t sysIntfType = 0;

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET, "intIfNum=%u, vlanId=%u, innerVlanId=%u, vport_id=%u", outgoingIf, vlanId, innerVlanId, vport_id);

  /* Get interface type */
  nimGetIntfType(outgoingIf, &sysIntfType);

  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    /* Do not evaluate port, if it is a virtual port */
    if (sysIntfType == L7_VLAN_PORT_INTF || L7_INTF_ISMASKBITSET(portMask, outgoingIf))
    {
      if (ptin_packet_frame_send(outgoingIf, vlanId, innerVlanId, vport_id, frame, frameLen) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET, "Failure to transmit packet on intIfNum %u in VLAN %d", outgoingIf, vlanId);
    }
    else
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET, "Outgoing intIfNum %u is not member of VLAN %d", outgoingIf, vlanId);
    }
  }

  return rc;
}

/***********************************************************************
* @purpose Flood packet to all ports on this VLAN
*
* @param    intIfNum    @b{(input)} receive interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on ports(trusted and untrusted) in VLAN
*
* @end
*
***********************************************************************/
static L7_RC_t ptin_packet_frame_flood(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId,
                                       L7_uchar8 *frame, L7_ushort16 frameLen)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i, activeState = L7_INACTIVE;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET, "intIfNum=%u, vlanId=%u, innerVlanId=%u", intIfNum, vlanId, innerVlanId);

  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
    {
      if (L7_INTF_ISMASKBITSET(portMask, i))
      {
        /* Don't flood back on the incoming interface */
        if(i != intIfNum)
        {
          if((nimGetIntfActiveState(i, &activeState) == L7_SUCCESS) &&
             (activeState == L7_ACTIVE))
          {
            /* Send on an interface that is link up and in forwarding state */
            if (ptin_packet_frame_send(i, vlanId, innerVlanId, 0, frame, frameLen) != L7_SUCCESS)
            {
              if (ptin_packet_debug_enable)
                PT_LOG_ERR(LOG_CTX_PACKET, "Error transmitting packet to intIfNum %u (vlanId=%u, innerVlanId=%u)",
                        i, vlanId, innerVlanId);
              rc = L7_FAILURE;
            }
          }
        }
      }
    }
  }

  return rc;
}

/***********************************************************************
* @purpose Send a packet to a given interface
*
* @param    intIfNum    @b{(input)} outgoing interface
* @param    vlanId      @b{(input)} VLAN ID
* @param    innerVlanId @b{(input)} Inner VLAN ID
* @param    vport_id    @b{(input)} Virtual Port id
* @param    frame       @b{(input)} ethernet frame
* @param    frameLen    @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on all trusted ports in VLAN
*
* @end
*
***********************************************************************/
static L7_RC_t ptin_packet_frame_send(L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 vport_id,
                                      L7_uchar8 *frame, L7_ushort16 frameLen)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  /* PTin added: DAI */
  #if 1
  L7_uint16           vlanId_list[16][2], number_of_vlans=0, i;
  L7_uint16           extOVlan = vlanId;
  L7_uint16           extIVlan = 0;
  #endif
  L7_RC_t rc = L7_SUCCESS;

  /* If outgoing interface is CPU interface, don't send it */
  if ((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (sysIntfType == L7_CPU_INTF))
  {
    return L7_SUCCESS;
  }

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET, "intIfNum=%u, vlanId=%u, innerVlanId=%u, vport_id=%u", intIfNum, vlanId, innerVlanId, vport_id);

  /* QUATTRO service? */
#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  if (vport_id != 0)
  {
    if (ptin_evc_extVlans_get_fromIntVlanVPort(vlanId, vport_id, &intIfNum, &vlanId_list[0][0], &vlanId_list[0][1]) != L7_SUCCESS)
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET, "Error obtaining Ext. VLANs for VLANs %u and VPort %u", vlanId, vport_id);
      return L7_FAILURE;
    }
    number_of_vlans = 1;
  }
  /* Quattro VLAN, but no vport? (flooding) */
  else if (ptin_evc_is_quattro_fromIntVlan(vlanId) && !ptin_evc_intf_isRoot(vlanId, intIfNum))
  {
    ptin_HwEthEvcFlow_t vport_flow;

    /* Get list of vlans (outer+inner) to be flooded */
    for (memset(&vport_flow, 0x00, sizeof(vport_flow));
         ptin_evc_vlan_client_next(vlanId, intIfNum, &vport_flow, &vport_flow) == L7_SUCCESS && number_of_vlans < 16;
         number_of_vlans++)
    {
      vlanId_list[number_of_vlans][0] = vport_flow.uni_ovid;
      vlanId_list[number_of_vlans][1] = vport_flow.uni_ivid;
    }
  }
  /* Regular service */
  else
#endif
  {
    L7_BOOL   is_stacked;
    L7_uint8  port_type;

    if (ptin_evc_extVlans_get_fromIntVlan(intIfNum, vlanId, innerVlanId, &vlanId_list[0][0], &vlanId_list[0][1]) != L7_SUCCESS ||
        ptin_evc_check_is_stacked_fromIntVlan(vlanId, &is_stacked) != L7_SUCCESS ||
        ptin_evc_intf_type_get(vlanId, intIfNum, &port_type) != L7_SUCCESS)
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET, "Error obtaining UNI VLANs from IntIfNum %u, VLANs %u+%u", intIfNum, vlanId, innerVlanId);
      return L7_FAILURE;
    }
    /* No inner VLAN for root interfaces of unstacked services */
    if (!is_stacked && port_type == PTIN_EVC_INTF_ROOT)
    {
      vlanId_list[0][1] = 0;
    }
    /* Only one VLAN */
    number_of_vlans = 1; 
  }

  if (ptin_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_PACKET, "number_of_vlans=%u", number_of_vlans);

  /* Transmit for all VLANs */
  for (i = 0; i < number_of_vlans; i++)
  {
    extOVlan = vlanId_list[i][0];
    extIVlan = vlanId_list[i][1];

    if (ptin_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_PACKET, "Going to transmit to intIfNum %u, with VLANs %u+%u", intIfNum, extOVlan, extIVlan);

    SYSAPI_NET_MBUF_GET(bufHandle);
    if (bufHandle == L7_NULL)
    {
      /* Don't bother logging this. mbuf alloc failures happen occasionally. */
      return L7_FAILURE;
    }

    if (osapiNtohs(*((L7_uint16 *) &frame[12])) != 0x8100 &&
        osapiNtohs(*((L7_uint16 *) &frame[12])) != 0x88A8 &&
        osapiNtohs(*((L7_uint16 *) &frame[12])) != 0x9100)
    {
      memmove(&frame[16], &frame[12], frameLen);
      frame[12] = 0x81;
      frame[13] = 0x00;
      frame[14] = (vlanId>>8) & 0xff;
      frame[15] = vlanId & 0xff;

      frameLen += 4;

      if (ptin_packet_debug_enable)
        PT_LOG_TRACE(LOG_CTX_PACKET, "Added outer VLAN (%u)", vlanId);
    }

    /* Modify outer vlan */
    if (vlanId!=extOVlan)
    {
      frame[14] &= 0xf0;
      frame[14] |= ((extOVlan>>8) & 0x0f);
      frame[15]  = extOVlan & 0xff;
      //vlanId = extOVlan;
      if (ptin_packet_debug_enable)
        PT_LOG_TRACE(LOG_CTX_PACKET, "Replaced outer VLAN (%u)", extOVlan);
    }
    /* Add inner vlan when there exists, and if vlan belongs to a stacked EVC */
    if (extIVlan!=0)
    {
      //for (i=frameLen-1; i>=16; i--)  frame[i+4] = frame[i];
            /* No inner tag? */
      if (osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x8100 &&
          osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x88A8 &&
          osapiNtohs(*((L7_uint16 *) &frame[16])) != 0x9100)
      {
        memmove(&frame[20],&frame[16],frameLen);
        frame[16] = 0x81;
        frame[17] = 0x00;
        frameLen += 4;
      }
      frame[18] = (frame[14] & 0xe0) | ((extIVlan>>8) & 0x0f);
      frame[19] = extIVlan & 0xff;
      //innerVlanId = extIVlan;
      if (ptin_packet_debug_enable)
        PT_LOG_TRACE(LOG_CTX_PACKET, "Added inner VLAN (%u)", extIVlan);
    }

    if (ptin_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_PACKET, "Going to transmit packet to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, extOVlan, extIVlan);

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    memcpy(dataStart, frame, frameLen);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frameLen);

    rc = dtlIpBufSend(intIfNum, vlanId, bufHandle);

    if (rc != L7_SUCCESS)
    {
      if (ptin_packet_debug_enable)
        PT_LOG_ERR(LOG_CTX_PACKET, "Error transmitting packet to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, extOVlan, extIVlan);
      break;
    }

    if (ptin_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_PACKET, "Packet transmitted to intIfNum %u, vlanId=%u, innerVlanId=%u", intIfNum, extOVlan, extIVlan);
  }

  return rc;
}


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

#include <unistd.h>


/* Maximum number of flooding vlans per client */
#define MAX_FLOOD_VLANS   8

/* Maximum number of messages in queue */
#define PTIN_PACKET_MAX_MESSAGES  2048

/* Message id used in queue */
#define PTIN_PACKET_MESSAGE_ID  1


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
    LOG_FATAL(LOG_CTX_PTIN_PACKET,"PTIN packet queue creation error.");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_PACKET,"PTIN packet queue created.");

  /* Create task for packets management */
  ptin_packet_TaskId = osapiTaskCreate("ptin_packet_task", ptin_packet_task, 0, 0,
                                        L7_DEFAULT_STACK_SIZE,
                                        L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                        L7_DEFAULT_TASK_SLICE);

  if (ptin_packet_TaskId == L7_ERROR)
  {
    LOG_FATAL(LOG_CTX_PTIN_PACKET, "Could not create task ptin_packet_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_PACKET,"Task ptin_packet_task created");

  if (osapiWaitForTaskInit (L7_PTIN_PACKET_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_PACKET,"Unable to initialize ptin_packet_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_PTIN_PACKET,"Task ptin_packet_task initialized");

  /* Register broadcast packets */
  strcpy(snEntry.funcName, "ptinBcastPduReceive");
  snEntry.notify_pdu_receive = ptinMacBcastRecv;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, BroadcastMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_PACKET, "Cannot register ptinBcastPduReceive callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_PTIN_PACKET, "ptinBcastPduReceive registered!");

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
    LOG_ERR(LOG_CTX_PTIN_PACKET, "Cannot unregister ptinBcastPduReceive callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_PTIN_PACKET, "ptinBcastPduReceive unregistered!");

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

  LOG_INFO(LOG_CTX_PTIN_PACKET, "PTin packet deinit OK");

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
    LOG_TRACE(LOG_CTX_PTIN_PACKET,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_PACKET,"Invalid vlanId %u",vlanId);
    return L7_FAILURE;
  }
  /* Validate inner vlan id */
  if ( innerVlanId == 0 || innerVlanId > 4095 )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_PACKET,"Invalid innerVlanId %u",innerVlanId);
    return L7_FAILURE;
  }

  /* Packet should be broadcast type */
  if ( memcmp(&payload[0], BroadcastMacAddr, L7_MAC_ADDR_LEN)!=0 )
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_PACKET,"Packet is not broadcast");
    return L7_FAILURE;
  }

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS)
  {
    if (ptin_packet_debug_enable)
      LOG_ERR(LOG_CTX_PTIN_PACKET,"intIfNum %u and vlan %u does not belong to any valid EVC/interface");
    return L7_FAILURE;
  }

  /* Send packet to queue */
  memset(&msg, 0x00, sizeof(msg));
  msg.msgId       = PTIN_PACKET_MESSAGE_ID;
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
      LOG_ERR(LOG_CTX_PTIN_PACKET,"Failed message sending to ptin_packet queue");
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

  LOG_INFO(LOG_CTX_PTIN_PACKET,"PTin packet process task started");

  if (osapiTaskInitDone(L7_PTIN_PACKET_TASK_SYNC)!=L7_SUCCESS)
  {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_INFO(LOG_CTX_PTIN_PACKET,"PTin packet task ready to process events");

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
      if ( msg.msgId == PTIN_PACKET_MESSAGE_ID )
      {
        if (ptinMacBcastProcess(&msg)!=L7_SUCCESS)
        {
          if (ptin_packet_debug_enable)
            LOG_ERR(LOG_CTX_PTIN_PACKET,"Error processing message");
        }
      }
      else
      {
        if (ptin_packet_debug_enable)
          LOG_ERR(LOG_CTX_PTIN_PACKET,"Message id is unknown (%u)",msg.msgId);
      }
    }
    else
    {
      if (ptin_packet_debug_enable)
        LOG_ERR(LOG_CTX_PTIN_PACKET,"Failed packet reception from ptin_packet queue (status = %d)",status);
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
      LOG_ERR(LOG_CTX_PTIN_PACKET,"Error getting list of leaf interfaces for vlanId=%u", vlanId);
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
        LOG_ERR(LOG_CTX_PTIN_PACKET,"Error getting list of vlans for vlanId=%u, innervlan=%u, intIfNum=%u", vlanId, innerVlanId, intf);
      continue;
    }

    if (ptin_packet_debug_enable)
      LOG_TRACE(LOG_CTX_PTIN_PACKET,"%u vlans obtained for intIfNum %u, vlanId=%u, cvlan=%u", number_of_vlans, intf, vlanId, innerVlanId);

    /* Transmit packet to all vlans of this interface */
    for (i=0; i<number_of_vlans; i++)
    {
      if (ptin_packet_debug_enable)
        LOG_TRACE(LOG_CTX_PTIN_PACKET,"Sending packet to intIfNum %u, with oVlan=%u", intf, flood_vlan[i]);

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

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);

  if (ptin_packet_debug_enable)
    LOG_TRACE(LOG_CTX_PTIN_PACKET,"Packet transmited to intIfNum=%u", intIfNum);

  return;
}


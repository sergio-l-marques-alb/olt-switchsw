/**
 * ptin_ipdtl0_packet.c
 *  
 * Implements routines to trap and to process packets on dtl0 IP 
 * Virtual Inteface 
 *
 * @author joaom (9/26/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */

#include "ptin_globaldefs.h"
#ifdef PTIN_ENABLE_DTL0TRAP

#include <unistd.h>

#include "ptin_ipdtl0_packet.h"

#include "ptin_intf.h"
#include "dtl_ptin.h"
#include "logger.h"
#include "ptin_evc.h"
#include "ptin_xlate_api.h"

#include "sysnet_api_ipv4.h"

//#define _PAYLOAD_DEBUG_

/***************************************
 * GLOBAL VARIABLES
 ***************************************/

/* Queue ID is used to keep packets */
void *ptin_ipdtl0_packetRx_queue = {L7_NULLPTR};

/* Task used to proc packets and send them to dtl0 */
L7_uint32 ptin_ipdtl0_TaskId = L7_ERROR;

typedef struct
{
    L7_uint16 dtl0Vid;
} ptin_ipdtl0_intVidInfo_t;

typedef struct
{
    L7_uint16 intVid;
    L7_uint16 outerVid;
    ptin_ipdtl0_type_t type;
} ptin_ipdtl0_dtl0Info_t;

/* Reference of used internal VLAN IDs */
ptin_ipdtl0_intVidInfo_t ptin_ipdtl0_intVid_info[4096];

/* Reference of used dtl0 VLAN IDs */
ptin_ipdtl0_dtl0Info_t ptin_ipdtl0_dtl0Vid_info[4096];

/***************************************
 * DEBUG ROUTINES
 ***************************************/

L7_BOOL ptin_ipdtl0_debug_enable = L7_FALSE;

/**
 * Enable more detailed debug
 * 
 * @author joaom (9/27/2013)
 * 
 * @param enable    value '0' or '1'
 */
void ptin_ipdtl0_debug(L7_BOOL enable)
{
    ptin_ipdtl0_debug_enable = enable & 1;
}


/***************************************
 * INTERNAL ROUTINES
 ***************************************/

/**
 * Task with infinite loop. 
 * Collects packets from queue and send them to dtl0 
 * 
 * @author joaom (9/27/2013)
 */
static void ptin_ipdtl0_task(void)
{
    ptin_IPDTL0_PDU_Msg_t   msg;
    L7_RC_t                 rc;

    PT_LOG_INFO(LOG_CTX_API,"IP dtl0 Task started");

    if (osapiTaskInitDone(L7_PTIN_IPDTL0_TASK_SYNC) != L7_SUCCESS)
    {
        PT_LOG_FATAL(LOG_CTX_API, "Error syncing task");
        PTIN_CRASH();
    }

    /* Infinite Loop */
    while (1)
    {
        rc = (L7_uint32) osapiMessageReceive(ptin_ipdtl0_packetRx_queue,
                                             (void*) &msg,
                                             PTIN_IPDTL0_PDU_MSG_SIZE,
                                             L7_WAIT_FOREVER);

        if (rc == L7_SUCCESS)
        {
            #ifdef _PAYLOAD_DEBUG_
            int i;
            for (i=0; i<msg.payloadLen; i++)
            {
                if (i%16==0)
                {
                    if (i!=0)
                        printf("\r\n");
                    printf(" 0x%04x:",i);
                }
                printf(" %02x", msg.payload[i]);
            }
            printf("\r\n");
            #endif

            if (msg.msgId == PTIN_IPDTL0_PACKET_MESSAGE_ID)
            {
                if (ptin_ipdtl0_debug_enable)
                {
                    PT_LOG_TRACE(LOG_CTX_API, "Packet received: intIfNum %d, vlanId %d, innerVlanId %d, payloadLen %d, Rx TS %lu\n", 
                           msg.intIfNum, msg.vlanId, msg.innerVlanId, msg.payloadLen, msg.timestamp);
                
                    #ifdef _PAYLOAD_DEBUG_
                    int i;
                    printf("Packet received: intIfNum %d, vlanId %d, innerVlanId %d, payloadLen %d, Rx TS %lu\n", 
                           msg.intIfNum, msg.vlanId, msg.innerVlanId, msg.payloadLen, msg.timestamp);
                    for (i=0; i<msg.payloadLen; i++)
                    {
                        if (i%16==0)
                        {
                            if (i!=0)
                                printf("\r\n");
                            printf(" 0x%02X:",i);
                        }
                        printf(" %02X", msg.payload[i]);
                    }
                    printf("\n\n\r");
                    #endif
                }

                sysnet_pdu_info_t       pduInfo;

                bzero((char *)&pduInfo, sizeof(sysnet_pdu_info_t));
                pduInfo.intIfNum = msg.intIfNum;
                pduInfo.rxPort = msg.intIfNum;
                pduInfo.vlanId = msg.vlanId;
                pduInfo.timestamp = msg.timestamp;

                /* Convert Internal VLAN ID to dtl0 VLAN ID */
                msg.payload[14] = (ptin_ipdtl0_intVid_info[msg.vlanId].dtl0Vid >> 8) & 0x0F;
                msg.payload[15] = (ptin_ipdtl0_intVid_info[msg.vlanId].dtl0Vid)      & 0xFF;

                if (ptin_ipdtl0_debug_enable)
                {
                    PT_LOG_TRACE(LOG_CTX_API, "Converting Internal VLAN ID (%d) to dtl0 VLAN ID (%d)\n\r", msg.vlanId, ptin_ipdtl0_intVid_info[msg.vlanId].dtl0Vid);
                }

                dtlIPProtoRecvAny(msg.bufHandle, msg.payload, msg.payloadLen, &pduInfo, L7_TRUE);
            }
            else if (msg.msgId == PTIN_IPDTL0_MIRRORPKT_MESSAGE_ID)
            {
                sysnet_pdu_info_t       pduInfo;

                bzero((char *)&pduInfo, sizeof(sysnet_pdu_info_t));
                pduInfo.intIfNum = msg.intIfNum;
                pduInfo.rxPort = msg.intIfNum;
                pduInfo.vlanId = msg.vlanId;


                // TODO - It would be interesting if instead of replacing the internal VLAN ID with PTIN_VLAN_PCAP_EXT (2048),
                // we add/push to the packet the external VLAN(s) according to the info on IXLATE for the ingress port

                if (msg.vlanId != 2047)
                {
                  /* Convert Internal VLAN ID to dtl0 VLAN ID */
                  msg.payload[14] = (PTIN_VLAN_PCAP_EXT >> 8) & 0x0F;
                  msg.payload[15] = (PTIN_VLAN_PCAP_EXT)      & 0xFF;
                  dtlIPProtoRecvAny(msg.bufHandle, msg.payload, msg.payloadLen, &pduInfo, L7_FALSE);
                }
                else
                {
                  /* Convert Internal VLAN ID to dtl0 VLAN ID */
                  msg.payload[14] = (ptin_ipdtl0_intVid_info[msg.vlanId].dtl0Vid >> 8) & 0x0F;
                  msg.payload[15] = (ptin_ipdtl0_intVid_info[msg.vlanId].dtl0Vid)      & 0xFF; 
                  dtlIPProtoRecvAny(msg.bufHandle, msg.payload, msg.payloadLen, &pduInfo, L7_TRUE);
                }

            else 
            {
                PT_LOG_TRACE(LOG_CTX_API, "Packet received with Unknown ID");
            }
        }

        usleep(10);
    }
}

/**
 * Handles packets coming from callback
 * 
 * @author joaom (9/27/2013)
 * 
 * @param netBufHandle 
 * @param pduInfo 
 * @param family 
 * 
 * @return L7_RC_t 
 */
static  L7_RC_t ptin_ipdtl0_packetHandle(L7_netBufHandle netBufHandle, sysnet_pdu_info_t *pduInfo, L7_uchar8 family)
{
    L7_uchar8                 *data;
    L7_uint32                 dataLength;
    ptin_IPDTL0_PDU_Msg_t     msg;
    L7_RC_t                   rc = L7_SUCCESS;
    L7_ushort16               etype = 0;

    /* Get start and length of incoming frame */
    SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
    SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);

    if (ptin_ipdtl0_debug_enable)
    {
        PT_LOG_TRACE(LOG_CTX_API,
                  "Packet intercepted vlan %d, innerVlan=%u, intIfNum %d, rx_port=%d, dataLength=%d",
                  pduInfo->vlanId, pduInfo->innerVlanId, pduInfo->intIfNum, pduInfo->rxPort, dataLength);

        PT_LOG_TRACE(LOG_CTX_API,
                  "ptin_ipdtl0_intVid2dtl0Vid[pduInfo->vlanId] %d", ptin_ipdtl0_intVid_info[pduInfo->vlanId].dtl0Vid);
    }

    /* Check if this is for me */
    if (ptin_ipdtl0_intVid_info[pduInfo->vlanId].dtl0Vid == PTIN_IPDTL0_UNUSED_VLAN_ENTRY)
    {
        if (ptin_ipdtl0_debug_enable)
        {
            PT_LOG_TRACE(LOG_CTX_API, "Trapping is not enabled on this VLAN");
        }
        rc = L7_FAILURE;
        return rc;
    }

    /* At this point, Packet is always tagged */
    memcpy(&etype, &data[16], sizeof(etype));
    etype = osapiNtohs(etype);

    if (ptin_ipdtl0_debug_enable)
    {
        PT_LOG_TRACE(LOG_CTX_API, "DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11]);
    }

    memset(&msg, 0x00, sizeof(msg));
    msg.msgId       = PTIN_IPDTL0_PACKET_MESSAGE_ID;
    msg.intIfNum    = pduInfo->intIfNum;
    msg.vlanId      = pduInfo->vlanId;
    msg.innerVlanId = pduInfo->innerVlanId;
    msg.timestamp   = pduInfo->timestamp;
    msg.bufHandle   = netBufHandle;
    msg.payloadLen  = dataLength;
    msg.payload     = data;
    
    /* Send packet to queue */
    rc = osapiMessageSend(ptin_ipdtl0_packetRx_queue, &msg, PTIN_IPDTL0_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

    /* If any error, packet will be dropped */
    if (rc!=L7_SUCCESS)

        PT_LOG_TRACE(LOG_CTX_PTIN_API, "If any error, packet will be dropped");
        return SYSNET_PDU_RC_IGNORED;
        PT_LOG_TRACE(LOG_CTX_PTIN_API, "If any error, packet will be dropped");
    }
        PT_LOG_TRACE(LOG_CTX_PTIN_API, "Error (%d) sending packet to queue, packet will be dropped", rc);
        /* In case of failure, Release buffer. Otherwise, buffer will be realeased later */
        SYSAPI_NET_MBUF_FREE(netBufHandle);
    }    

    /* At this point, packet is always consumed */
    return L7_SUCCESS;
}

/**
 * sysnetPduIntercept.interceptFunc
 * 
 * @author joaom (9/27/2013)
 * 
 * @param hookId 
 * @param bufHandle 
 * @param pduInfo 
 * @param continueFunc 
 * 
 * @return SYSNET_PDU_RC_t 
 */
static SYSNET_PDU_RC_t ptin_ipdtl0_packetProc(L7_uint32 hookId,
                                       L7_netBufHandle bufHandle,
                                       sysnet_pdu_info_t *pduInfo,
                                       L7_FUNCPTR_t continueFunc)
{
    if (ptin_ipdtl0_packetHandle(bufHandle, pduInfo, L7_AF_INET) == L7_SUCCESS)
    {
        /* since this matched... consume the packet */
        return SYSNET_PDU_RC_CONSUMED;
    }

    return SYSNET_PDU_RC_IGNORED;
}

/**
 * Creates HW Trap rule and SW Callback
 * 
 * @author joaom (9/27/2013)
 * 
 * @param vlanId 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_ipdtl0_trapRuleCreate(L7_uint16 vlanId, ptin_ipdtl0_type_t type, L7_BOOL enable)
{
    DAPI_SYSTEM_CMD_t dapiCmd;
    L7_uchar8 mac[L7_MAC_ADDR_LEN];
    L7_RC_t rc;

    /* Get base MAC address */
    if (bspapiMacAddrGet(mac) != L7_SUCCESS)
    {
        PTIN_CRASH();
    }    

    /* HW Rule Creation: All packet for this (reserved) VLAN will be trapped */
    if ((type == PTIN_IPDTL0_ETH) || (type == PTIN_IPDTL0_ETH_IPv4_UDP_PTP))
    {
        dapiCmd.cmdData.snoopConfig.getOrSet              = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
        dapiCmd.cmdData.snoopConfig.family                = L7_AF_INET;
        dapiCmd.cmdData.snoopConfig.vlanId                = vlanId;
        dapiCmd.cmdData.snoopConfig.enable                = enable & 1;
        memcpy(dapiCmd.cmdData.snoopConfig.macAddr.addr,  mac, L7_MAC_ADDR_LEN);
        dapiCmd.cmdData.snoopConfig.packet_type           = PTIN_PACKET_IPDTL0;

        rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
        if (rc!=L7_SUCCESS)
        {
            PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
            return rc;
        }

        /* Tell driver to enable IGMP Snooping */
        dapiCmd.cmdData.snoopConfig.vlanId = L7_NULL;
        rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
        if (rc!=L7_SUCCESS)
        {
            PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
            return rc;
        }

        PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);
    }

    /* HW Rule Creation replaced by L3 Table entry with IP/MAC/Port */
    else if (type == PTIN_IPDTL0_ETH_IPv4)
    {
        #if 0
        dapiCmd.cmdData.ipDtl0Config.getOrSet              = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
        dapiCmd.cmdData.ipDtl0Config.family                = L7_AF_INET;
        dapiCmd.cmdData.ipDtl0Config.vlanId                = vlanId;
        memcpy(dapiCmd.cmdData.ipDtl0Config.macAddr.addr,  mac, L7_MAC_ADDR_LEN);
        dapiCmd.cmdData.ipDtl0Config.packet_type           = PTIN_PACKET_IPDTL0;

        rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
        if (rc!=L7_SUCCESS)
        {
            PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
            return rc;
        }
        PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);
        #endif
    }


    /* Register IP dtl0 packets */
    if ((type == PTIN_IPDTL0_ETH) || (type == PTIN_IPDTL0_ETH_IPv4) || (type == PTIN_IPDTL0_ETH_IPv4_UDP_PTP))
    {
        sysnetPduIntercept_t sysnetPduIntercept;

        sysnetPduIntercept.addressFamily   = L7_AF_INET;
        sysnetPduIntercept.hookId          = SYSNET_INET_RECV_IN;
        sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_IPDTL0_PRECEDENCE;
        sysnetPduIntercept.interceptFunc   = ptin_ipdtl0_packetProc;
        strcpy(sysnetPduIntercept.interceptFuncName, "ptin_ipdtl0_packetProc");

        if (enable)
        {
            /* Register with sysnet */
            sysNetPduInterceptRegister(&sysnetPduIntercept);
            PT_LOG_TRACE(LOG_CTX_API,"sysNetPduInterceptRegister executed");    
        }
        else
        {
            /* Deregister with sysnet */
            sysNetPduInterceptDeregister(&sysnetPduIntercept);
            PT_LOG_TRACE(LOG_CTX_API,"sysNetPduInterceptDeregister executed");
        }
    }

    /* Register ARP dtl0 packets */
    if ((type == PTIN_IPDTL0_ETH) || (type == PTIN_IPDTL0_ETH_IPv4_UDP_PTP))
    {
        sysnetPduIntercept_t sysnetPduIntercept;

        sysnetPduIntercept.addressFamily   = L7_AF_INET;
        sysnetPduIntercept.hookId          = SYSNET_INET_RECV_ARP_IN;
        sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_IPDTL0_ARP_PRECEDENCE;
        sysnetPduIntercept.interceptFunc   = ptin_ipdtl0_packetProc;
        strcpy(sysnetPduIntercept.interceptFuncName, "ptin_ipdtl0_packetProc");

        if (enable)
        {
            /* Register with sysnet */
            sysNetPduInterceptRegister(&sysnetPduIntercept);
            PT_LOG_TRACE(LOG_CTX_API,"sysNetPduInterceptRegister executed");    
        }
        else
        {
            /* Deregister with sysnet */
            sysNetPduInterceptDeregister(&sysnetPduIntercept);
            PT_LOG_TRACE(LOG_CTX_API,"sysNetPduInterceptDeregister executed");
        }
    }

    return L7_SUCCESS;
}


/***************************************
 * EXTERNAL ROUTINES
 ***************************************/

/**
 * IP dtl0 module initialization. 
 * Creates IP/ARP Packets queue and task to dispatch.
 * 
 * @author joaom (9/27/2013)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_init(void)
{
    L7_uint8 queue_str[24];

    /* Init Global variables */
    memset(ptin_ipdtl0_intVid_info, PTIN_IPDTL0_UNUSED_VLAN_ENTRY, sizeof(ptin_ipdtl0_intVid_info));
    memset(ptin_ipdtl0_dtl0Vid_info, PTIN_IPDTL0_UNUSED_VLAN_ENTRY, sizeof(ptin_ipdtl0_dtl0Vid_info));

    /* Queue that will process packets */
    sprintf(queue_str, "PTin_IPDTL0_PacketRx_Queue");
    ptin_ipdtl0_packetRx_queue = (void *) osapiMsgQueueCreate(queue_str, PTIN_IPDTL0_MAX_MESSAGES, PTIN_IPDTL0_PDU_MSG_SIZE);
    if (ptin_ipdtl0_packetRx_queue == L7_NULLPTR)
    {
        PT_LOG_FATAL(LOG_CTX_API,"IP dtl0 Packet Queue creation error.");
        return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_API,"IP dtl0 Packet Queue created.");


    /* Task that will process packets */
    ptin_ipdtl0_TaskId = osapiTaskCreate("ptin_ipdtl0_task", ptin_ipdtl0_task, 0, 0,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                         L7_DEFAULT_TASK_SLICE);

    if (ptin_ipdtl0_TaskId == L7_ERROR)
    {
        PT_LOG_FATAL(LOG_CTX_API, "Could not create IP dtl0 Packet Task");
        return L7_FAILURE;
    }
    PT_LOG_TRACE(LOG_CTX_API,"IP dtl0 Packet Task created");

    if (osapiWaitForTaskInit (L7_PTIN_IPDTL0_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
        PT_LOG_FATAL(LOG_CTX_API,"Unable to initialize IP dtl0 Packet Task\n");
        return(L7_FAILURE);
    }
    PT_LOG_TRACE(LOG_CTX_API,"IP dtl0 Packet Task initialized");

    return L7_SUCCESS;
}

/**
 * IP dtl0 module deinitialization. 
 * Destroys IP/ARP Packets queue and task.
 * 
 * @author joaom (9/27/2013)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_deinit(void)
{
    /* Queue that will process packets */
    osapiMsgQueueDelete(ptin_ipdtl0_packetRx_queue);
    ptin_ipdtl0_packetRx_queue = L7_NULLPTR;

    PT_LOG_TRACE(LOG_CTX_API,"IP dtl0 Packet Queue deleted.");


    /* Task that will process packets */
    osapiTaskDelete(ptin_ipdtl0_TaskId);
    ptin_ipdtl0_TaskId = L7_ERROR;

    PT_LOG_TRACE(LOG_CTX_API,"IP dtl0 Packet Task destroyed.");

    return L7_SUCCESS;
}

/**
 * Enables/Disables IP/ARP packets through dtl0
 * 
 * @author joaom (10/01/2013)
 * 
 * @param dtl0Vid 
 * @param outerVid 
 * @param internalVid 
 * @param intfNum 
 * @param type 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_control(L7_uint16 dtl0Vid, L7_uint16 outerVid, L7_uint16 internalVid, L7_uint32 intfNum, ptin_ipdtl0_type_t type, L7_BOOL enable)
{
    L7_RC_t rc = L7_SUCCESS;

    /* VLAN ID Validation */
    if (dtl0Vid<PTIN_VLAN_MIN || dtl0Vid>PTIN_VLAN_MAX)
    {
        PT_LOG_ERR(LOG_CTX_API,"dtl0 Vid Invalid argument");
        return L7_FAILURE;
    }

    /* VLAN ID Validation */
    if (outerVid<PTIN_VLAN_MIN || outerVid>PTIN_VLAN_MAX)
    {
        PT_LOG_ERR(LOG_CTX_API,"dtl0 Vid Invalid argument");
        return L7_FAILURE;
    }

    if (type <= PTIN_IPDTL0_NONE || type >= PTIN_IPDTL0_LAST)
    {
        PT_LOG_ERR(LOG_CTX_API,"dtl0 Invalid type");
        return L7_FAILURE;
    }

    /* Convert to internal VLAN ID(if not previously provided) */
    if(enable && (internalVid == (L7_uint16)-1))
    {
      rc = ptin_xlate_ingress_get(intfNum, outerVid, PTIN_XLATE_NOT_DEFINED, &internalVid, L7_NULLPTR);
      if ((rc != L7_SUCCESS) || (internalVid == 0))
      {
          PT_LOG_ERR(LOG_CTX_API,"Error Enabling IP dtl0");
          return rc;
      }
    }

    /* Disable */
    if (!enable)
    {
        internalVid = ptin_ipdtl0_dtl0Vid_info[dtl0Vid].intVid;
        ptin_ipdtl0_intVid_info[internalVid].dtl0Vid = PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
        ptin_ipdtl0_dtl0Vid_info[dtl0Vid].intVid = PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
        ptin_ipdtl0_dtl0Vid_info[dtl0Vid].outerVid = PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
        ptin_ipdtl0_dtl0Vid_info[dtl0Vid].type = PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
    }

    /* HW & FW Rule Creation */
    rc = ptin_ipdtl0_trapRuleCreate(internalVid, type, enable);
    if (rc != L7_SUCCESS)
    {
        PT_LOG_ERR(LOG_CTX_API,"Error Enabling IP dtl0");
        return rc;
    }

    /* Enable */
    if (enable)
    {
        ptin_ipdtl0_intVid_info[internalVid].dtl0Vid = dtl0Vid;
        ptin_ipdtl0_dtl0Vid_info[dtl0Vid].intVid = internalVid;
        ptin_ipdtl0_dtl0Vid_info[dtl0Vid].outerVid = outerVid;
        ptin_ipdtl0_dtl0Vid_info[dtl0Vid].type = type;

        PT_LOG_TRACE(LOG_CTX_API,"(dtl0Vid=%d, outerVid=%d, intfNum=%d, type=%d, enable=%d) internalVid %d\n", dtl0Vid, outerVid, intfNum, type, enable, internalVid);
    }

    return rc;
}

/*********************************************************************
* @purpose  Receives sampled packet
*
* @param    netBufHandle    @b{(input)} Handle to buffer
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*                                        which stores intIfNum and vlanId
*
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
* @end
*********************************************************************/
L7_RC_t ptin_ipdtl0_mirrorPacketCapture(L7_netBufHandle netBufHandle,
                                        sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8 *data;
  L7_uint32 dataLength;
  L7_RC_t   rc = L7_SUCCESS;
  ptin_IPDTL0_PDU_Msg_t     msg;

  if (ptin_ipdtl0_debug_enable)
  {
      PT_LOG_TRACE(LOG_CTX_API,
                "Packet intercepted vlan %d, innerVlan=%u, intIfNum %d, rx_port=%d",
                pduInfo->vlanId, pduInfo->innerVlanId, pduInfo->intIfNum, pduInfo->rxPort);
  }

  /* Get start and length of incoming frame */
  SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);

  memset(&msg, 0x00, sizeof(msg));
  msg.msgId       = PTIN_IPDTL0_MIRRORPKT_MESSAGE_ID;
  msg.intIfNum    = pduInfo->intIfNum;
  msg.vlanId      = pduInfo->vlanId;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.bufHandle   = netBufHandle;
  msg.payloadLen  = dataLength;
  msg.payload     = data;

  /* Send packet to queue */
  rc = osapiMessageSend(ptin_ipdtl0_packetRx_queue, &msg, PTIN_IPDTL0_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  /* If any error, packet will be dropped */
  if (rc!=L7_SUCCESS)
  {
      PT_LOG_TRACE(LOG_CTX_API, "Error (%d) sending packet to queue, packet will be dropped", rc);
  
      /* In case of failure, Release buffer. Otherwise, buffer will be realeased later */
      SYSAPI_NET_MBUF_FREE(netBufHandle);
  }    
  
  /* At this point, packet is always consumed */
  return L7_SUCCESS;
}


/**
 * Enables/Disables IP/ARP packets through dtl0
 * 
 * @author joaom (10/01/2013)
 * 
 * @param dtl0Vid 
 * @param outerVid 
 * @param lag_idx 
 * @param type 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_control_b(L7_uint16 dtl0Vid, L7_uint16 outerVid, L7_uint32 lag_idx, ptin_ipdtl0_type_t type, L7_BOOL enable)
{
    L7_uint32   intfNum;
    L7_RC_t     rc = L7_SUCCESS;

    rc = ptin_intf_lag2intIfNum(lag_idx, &intfNum);
    if (rc != L7_SUCCESS)
    {
        PT_LOG_ERR(LOG_CTX_API,"Error Enabling IP dtl0");
        return rc;
    }

    ptin_ipdtl0_control(dtl0Vid, outerVid, (L7_uint16)-1, intfNum, type, enable);

    return rc;
}

/**
 * Get dtl0 VLAN ID from internal VLAN ID
 * 
 * @author daniel (15/4/2013)
 * 
 * @param intVid 
 * 
 * @return L7_uint16 dtl0Vid
 */
L7_uint16 ptin_ipdtl0_dtl0Vid_get(L7_uint16 intVid)
{
    return (ptin_ipdtl0_intVid_info[intVid].dtl0Vid);
}

/**
 * Get Internal VLAN ID from dtl0 VLAN ID
 * 
 * @author joaom (10/3/2013)
 * 
 * @param dtl0Vid 
 * 
 * @return L7_uint16 internalVid
 */
L7_uint16 ptin_ipdtl0_internalVid_get(L7_uint16 dtl0Vid)
{
    /* Reference of used internal VLAN IDs */
    return (ptin_ipdtl0_dtl0Vid_info[dtl0Vid].intVid);
}

/**
 * Get outer VLAN ID from dtl0 VLAN ID
 * 
 * @author joaom (10/3/2013)
 * 
 * @param dtl0Vid 
 * 
 * @return L7_uint16 internalVid
 */
L7_uint16 ptin_ipdtl0_outerVid_get(L7_uint16 dtl0Vid)
{
    /* Reference of used outer VLAN IDs */
    return (ptin_ipdtl0_dtl0Vid_info[dtl0Vid].outerVid);
}

/**
 * Get dtl0 type from dtl0 VLAN ID
 * 
 * @author joaom (1/10/2015)
 * 
 * @param vlanId 
 * 
 * @return L7_RC_t 
 */
L7_uint16 ptin_ipdtl0_dtl0Type_get(L7_uint16 dtl0Vid)
{
    return (ptin_ipdtl0_dtl0Vid_info[dtl0Vid].type);
}

#else

/***************************************
 * DUMMY ROUTINES
 ***************************************/

L7_uint16 ptin_ipdtl0_dtl0Vid_get(L7_uint16 intVid)
{
    return 0 /* PTIN_IPDTL0_UNUSED_VLAN_ENTRY*/ ;
}

L7_uint16 ptin_ipdtl0_internalVid_get(L7_uint16 dtl0Vid)
{
    return 0 /* PTIN_IPDTL0_UNUSED_VLAN_ENTRY*/ ;
}

L7_uint16 ptin_ipdtl0_outerVid_get(L7_uint16 dtl0Vid)
{
    return 0 /* PTIN_IPDTL0_UNUSED_VLAN_ENTRY*/ ;
}

L7_uint16 ptin_ipdtl0_dtl0Type_get(L7_uint16 dtl0Vid)
{
    return 0 /* PTIN_IPDTL0_UNUSED_VLAN_ENTRY*/ ;
}

#endif /* PTIN_ENABLE_DTL0TRAP */


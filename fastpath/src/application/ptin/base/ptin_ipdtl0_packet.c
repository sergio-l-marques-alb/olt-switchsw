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


/***************************************
 * GLOBAL VARIABLES
 ***************************************/

/* Queue ID is used to keep packets */
void *ptin_ipdtl0_packetRx_queue = {L7_NULLPTR};

/* Task used to proc packets and send them to dtl0 */
L7_uint32 ptin_ipdtl0_TaskId = L7_ERROR;


/* Reference of used internal VLAN IDs */
L7_ushort16 ptin_ipdtl0_intVid2dtl0Vid[4096] = {PTIN_IPDTL0_UNUSED_VLAN_ENTRY};

/* Reference of used dtl0 VLAN IDs */
L7_ushort16 ptin_ipdtl0_dtl0Vid2intVid[4096] = {PTIN_IPDTL0_UNUSED_VLAN_ENTRY};

/* Reference of used outer VLAN IDs */
L7_ushort16 ptin_ipdtl0_dtl0Vid2outerVid[4096] = {PTIN_IPDTL0_UNUSED_VLAN_ENTRY};


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

    LOG_INFO(LOG_CTX_PTIN_API,"IP dtl0 Task started");

    if (osapiTaskInitDone(L7_PTIN_IPDTL0_TASK_SYNC) != L7_SUCCESS)
    {
        LOG_FATAL(LOG_CTX_PTIN_API, "Error syncing task");
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
            if (msg.msgId == PTIN_IPDTL0_PACKET_MESSAGE_ID)
            {
                if (ptin_ipdtl0_debug_enable)
                {
                    LOG_TRACE(LOG_CTX_PTIN_API, "Packet received: intIfNum %d, vlanId %d, innerVlanId %d, payloadLen %d\n", 
                           msg.intIfNum, msg.vlanId, msg.innerVlanId, msg.payloadLen);
                
                    #ifdef _PAYLOAD_DEBUG_
                    int i;
                    for (i=0; i<msg.payloadLen; i++)
                    {
                        if (i%16==0)
                        {
                            if (i!=0)
                                printf("\r\n");
                            printf(" 0x%02x:",i);
                        }
                        printf(" %02x", msg.payload[i]);
                    }
                    printf("\r\n");
                    #endif
                }

                sysnet_pdu_info_t       pduInfo;

                bzero((char *)&pduInfo, sizeof(sysnet_pdu_info_t));
                pduInfo.intIfNum = msg.intIfNum;
                pduInfo.rxPort = msg.intIfNum;
                pduInfo.vlanId = msg.vlanId;

                /* Convert Internal VLAN ID to dtl0 VLAN ID */
                msg.payload[14] = (ptin_ipdtl0_intVid2dtl0Vid[msg.vlanId] >> 8) & 0xFF;
                msg.payload[15] = (ptin_ipdtl0_intVid2dtl0Vid[msg.vlanId])      & 0xFF;

                dtlIPProtoRecvAny(msg.bufHandle, msg.payload, msg.payloadLen, &pduInfo);
            }
            else
            {
                LOG_TRACE(LOG_CTX_PTIN_API, "Packet received with Unknown ID");
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


    if (ptin_ipdtl0_debug_enable)
    {
        LOG_TRACE(LOG_CTX_PTIN_API,
                  "Packet intercepted vlan %d, innerVlan=%u, intIfNum %d, rx_port=%d",
                  pduInfo->vlanId, pduInfo->innerVlanId, pduInfo->intIfNum, pduInfo->rxPort);

        LOG_TRACE(LOG_CTX_PTIN_API,
                  "ptin_ipdtl0_intVid2dtl0Vid[pduInfo->vlanId] %d", ptin_ipdtl0_intVid2dtl0Vid[pduInfo->vlanId]);
    }

    /* Check if this is for me */
    if (ptin_ipdtl0_intVid2dtl0Vid[pduInfo->vlanId] == PTIN_IPDTL0_UNUSED_VLAN_ENTRY)
    {
        rc = L7_FAILURE;
        return rc;
    }

    /* Get start and length of incoming frame */
    SYSAPI_NET_MBUF_GET_DATASTART(netBufHandle, data);
    SYSAPI_NET_MBUF_GET_DATALENGTH(netBufHandle, dataLength);

    if (ptin_ipdtl0_debug_enable)
    {
        LOG_TRACE(LOG_CTX_PTIN_API, "DMAC=%02x:%02x:%02x:%02x:%02x:%02x SMAC=%02x:%02x:%02x:%02x:%02x:%02x",
                  data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11]);
    }

    memset(&msg, 0x00, sizeof(msg));
    msg.msgId       = PTIN_IPDTL0_PACKET_MESSAGE_ID;
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
        LOG_TRACE(LOG_CTX_PTIN_API, "If any error, packet will be dropped");
    }

    return rc;
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
static L7_RC_t ptin_ipdtl0_trapRuleCreate(L7_uint16 vlanId, L7_BOOL enable)
{

    DAPI_SYSTEM_CMD_t dapiCmd;
    L7_uchar8 mac[L7_MAC_ADDR_LEN];
    L7_RC_t rc;   

    /* Get base MAC address */
    if (bspapiMacAddrGet(mac) != L7_SUCCESS)
    {
        PTIN_CRASH();
    }    

    /* HW Rule Creation */
    {
        dapiCmd.cmdData.ipDtl0Config.getOrSet              = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
        dapiCmd.cmdData.ipDtl0Config.family                = L7_AF_INET;
        dapiCmd.cmdData.ipDtl0Config.vlanId                = vlanId;
        memcpy(dapiCmd.cmdData.ipDtl0Config.macAddr.addr,  mac, L7_MAC_ADDR_LEN);
        dapiCmd.cmdData.ipDtl0Config.packet_type           = PTIN_PACKET_IPDTL0;

        rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
        if (rc!=L7_SUCCESS)
        {
            LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
            return rc;
        }
        LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);
    }

    /* Register IP dtl0 packets */
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
            LOG_TRACE(LOG_CTX_PTIN_API,"sysNetPduInterceptRegister executed");    
        }
        else
        {
            /* Deregister with sysnet */
            sysNetPduInterceptDeregister(&sysnetPduIntercept);
            LOG_TRACE(LOG_CTX_PTIN_API,"sysNetPduInterceptDeregister executed");
        }
    }

    /* Register ARP dtl0 packets */
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
            LOG_TRACE(LOG_CTX_PTIN_API,"sysNetPduInterceptRegister executed");    
        }
        else
        {
            /* Deregister with sysnet */
            sysNetPduInterceptDeregister(&sysnetPduIntercept);
            LOG_TRACE(LOG_CTX_PTIN_API,"sysNetPduInterceptDeregister executed");
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

    /* Queue that will process packets */
    sprintf(queue_str, "PTin_IPDTL0_PacketRx_Queue");
    ptin_ipdtl0_packetRx_queue = (void *) osapiMsgQueueCreate(queue_str, PTIN_IPDTL0_MAX_MESSAGES, PTIN_IPDTL0_PDU_MSG_SIZE);
    if (ptin_ipdtl0_packetRx_queue == L7_NULLPTR)
    {
        LOG_FATAL(LOG_CTX_PTIN_API,"IP dtl0 Packet Queue creation error.");
        return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_API,"IP dtl0 Packet Queue created.");


    /* Task that will process packets */
    ptin_ipdtl0_TaskId = osapiTaskCreate("ptin_ipdtl0_task", ptin_ipdtl0_task, 0, 0,
                                         L7_DEFAULT_STACK_SIZE,
                                         L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                         L7_DEFAULT_TASK_SLICE);

    if (ptin_ipdtl0_TaskId == L7_ERROR)
    {
        LOG_FATAL(LOG_CTX_PTIN_API, "Could not create IP dtl0 Packet Task");
        return L7_FAILURE;
    }
    LOG_TRACE(LOG_CTX_PTIN_API,"IP dtl0 Packet Task created");

    if (osapiWaitForTaskInit (L7_PTIN_IPDTL0_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
        LOG_FATAL(LOG_CTX_PTIN_API,"Unable to initialize IP dtl0 Packet Task\n");
        return(L7_FAILURE);
    }
    LOG_TRACE(LOG_CTX_PTIN_API,"IP dtl0 Packet Task initialized");

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

    LOG_TRACE(LOG_CTX_PTIN_API,"IP dtl0 Packet Queue deleted.");


    /* Task that will process packets */
    osapiTaskDelete(ptin_ipdtl0_TaskId);
    ptin_ipdtl0_TaskId = L7_ERROR;

    LOG_TRACE(LOG_CTX_PTIN_API,"IP dtl0 Packet Task destroyed.");

    return L7_SUCCESS;
}


/**
 * Enables/Disables IP/ARP packets through dtl0
 * 
 * @author joaom (10/01/2013)
 * 
 * @param dtl0Vid 
 * @param outerVid 
 * @param intfNum 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ipdtl0_control(L7_uint16 dtl0Vid, L7_uint16 outerVid, L7_uint32 intfNum, L7_BOOL enable)
{
    L7_RC_t     rc = L7_SUCCESS;
    L7_uint16   internalVid;

    /* VLAN ID Validation */
    if (dtl0Vid<PTIN_VLAN_MIN || dtl0Vid>PTIN_VLAN_MAX)
    {
        LOG_ERR(LOG_CTX_PTIN_API,"dtl0 Vid Invalid argument");
        return L7_FAILURE;
    }

    /* VLAN ID Validation */
    if (outerVid<PTIN_VLAN_MIN || outerVid>PTIN_VLAN_MAX)
    {
        LOG_ERR(LOG_CTX_PTIN_API,"dtl0 Vid Invalid argument");
        return L7_FAILURE;
    }

    /* Convert to internal VLAN ID */
    rc = ptin_xlate_ingress_get(intfNum, outerVid, PTIN_XLATE_NOT_DEFINED, &internalVid);
    if ((rc != L7_SUCCESS) || (internalVid == 0))
    {
        LOG_ERR(LOG_CTX_PTIN_API,"Error Enabling IP dtl0");
        return rc;
    }

    /* Disable */
    if (!enable)
    {
        ptin_ipdtl0_intVid2dtl0Vid[internalVid] =   PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
        ptin_ipdtl0_dtl0Vid2intVid[dtl0Vid] =       PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
        ptin_ipdtl0_dtl0Vid2outerVid[dtl0Vid] =     PTIN_IPDTL0_UNUSED_VLAN_ENTRY;
    }

    /* HW & FW Rule Creation */
    rc = ptin_ipdtl0_trapRuleCreate(internalVid, enable);
    if (rc != L7_SUCCESS)
    {
        LOG_ERR(LOG_CTX_PTIN_API,"Error Enabling IP dtl0");
        return rc;
    }

    /* Enable */
    if (enable)
    {
        ptin_ipdtl0_intVid2dtl0Vid[internalVid] =   dtl0Vid;
        ptin_ipdtl0_dtl0Vid2intVid[dtl0Vid] =       internalVid;
        ptin_ipdtl0_dtl0Vid2outerVid[dtl0Vid] =     outerVid;

        LOG_TRACE(LOG_CTX_PTIN_API,"(dtl0Vid=%d, outerVid=%d, intfNum%d, enable=%d) internalVid %d\n", dtl0Vid, outerVid, intfNum, enable, internalVid);
    }

    return rc;
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
L7_uint16 ptin_ipdtl0_getInternalVid(L7_uint16 dtl0Vid)
{
    /* Reference of used outer VLAN IDs */
    return (ptin_ipdtl0_dtl0Vid2intVid[dtl0Vid]);
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
L7_uint16 ptin_ipdtl0_getOuterVid(L7_uint16 dtl0Vid)
{
    /* Reference of used outer VLAN IDs */
    return (ptin_ipdtl0_dtl0Vid2outerVid[dtl0Vid]);
}

#else

/***************************************
 * DUMMY ROUTINES
 ***************************************/

L7_uint16 ptin_ipdtl0_getInternalVid(L7_uint16 dtl0Vid)
{
    return 0 /* PTIN_IPDTL0_UNUSED_VLAN_ENTRY*/ ;
}

L7_uint16 ptin_ipdtl0_getOuterVid(L7_uint16 dtl0Vid)
{
    return 0 /* PTIN_IPDTL0_UNUSED_VLAN_ENTRY*/ ;
}

#endif /* PTIN_ENABLE_DTL0TRAP */


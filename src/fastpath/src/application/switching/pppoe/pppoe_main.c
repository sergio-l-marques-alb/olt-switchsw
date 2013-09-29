/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  pppoe_main.c
*
* @purpose   PPPoE Intermediate Agent
*
* @component PPPoE
*
* @comments none
*
* @create 14/05/2013
*
* @author Daniel Figueira
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "log.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "trapapi.h"

#include "ptin_evc.h"
#include "ptin_pppoe.h"

#include "dot1q_api.h"
#include "dot3ad_api.h"

#include "pppoe_cnfgr.h"
#include "pppoe_util.h"

extern pppoeCnfgrState_t pppoeCnfgrState;
extern osapiRWLock_t pppoeCfgRWLock;

extern void *pppoeMsgQSema;
extern void *pppoe_Event_Queue;
extern void *pppoe_Packet_Queue;
extern ptinPppoeBindingAvlTree_t pppoeBindingTable;


/********************************************************************* 
*  Static Methods 
*********************************************************************/
static void    pppoeProcessServerFrame(L7_uchar8* frame, L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx);
static void    pppoeProcessClientFrame(L7_uchar8* frame, L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx);
static L7_RC_t pppoeServerFrameSend(L7_uchar8* frame, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx);
static L7_RC_t pppoeClientFrameSend(L7_uint32 intIfNum, L7_uchar8* frame, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx);
static L7_RC_t pppoeServerInterfaceGet(L7_uint32 *intIfNum, L7_ushort16 vlanId);
static L7_RC_t pppoeCommonErrorFrameCreate(L7_uchar8 *originalFramePtr, L7_uchar8 *newFramePtr, L7_uint32 *newFrameLen);
static L7_RC_t pppoeCopyTlv(L7_uchar8 *originalFramePtr, L7_uchar8 *newFramePtr);
static L7_RC_t pppoeAddVendorIdTlv(L7_uchar8 *framePtr, L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId);



/*********************************************************************
* @purpose  Main function for the DHCP snooping thread. Read incoming
*           messages (events and DHCP packets) and process accordingly.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void pppoeTask(void)
{
   pppoeEventMsg_t eventMsg;
   pppoeFrameMsg_t frameMsg;

   /* Loop forever, processing incoming messages */
   while (L7_TRUE)
   {
     /* Since we are reading from multiple queues, we cannot wait forever
      * on the message receive from each queue. Use a semaphore to indicate
      * whether any queue has data. Wait until data is available.
      */
     if (osapiSemaTake(pppoeMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
     {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, PTIN_PPPOE_COMPONENT_ID,
              "Failure taking PPPoE message queue semaphore.");
       continue;
     }
     memset(&eventMsg, 0, sizeof(pppoeEventMsg_t));
     memset(&frameMsg, 0, sizeof(pppoeFrameMsg_t));

     /* Always try to read from the event queue first. No sense processing
      * DHCP packets with stale state. */
     if (osapiMessageReceive(pppoe_Event_Queue, &eventMsg,
                             sizeof(pppoeEventMsg_t), L7_NO_WAIT) == L7_SUCCESS)
     {
       /* Configuration actions are all done on the UI thread.
        * Synchronize that processing with event processing by
        * taking the same semaphore here. */
       switch (eventMsg.msgType)
       {
         case PPPOE_CNFGR_INIT:
           if (osapiWriteLockTake(pppoeCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
           {
             pppoeCnfgrHandle(&eventMsg.pppoeMsgData.cmdData);
             osapiWriteLockGive(pppoeCfgRWLock);
           }
           break;
         default:
           L7_LOGF(L7_LOG_SEVERITY_ERROR, PTIN_PPPOE_COMPONENT_ID,
                  "Unknown event message type received in the DHCP snooping thread.");
           break;
       }
     }

     /* If the event queue was empty, see if a DHCP message has arrived. */
     else if (osapiMessageReceive(pppoe_Packet_Queue, &frameMsg,
                                  sizeof(pppoeFrameMsg_t), L7_NO_WAIT) == L7_SUCCESS)
     {
       if (osapiWriteLockTake(pppoeCfgRWLock, L7_WAIT_FOREVER) == L7_SUCCESS)
       {
         pppoeFrameProcess(frameMsg.rxIntf, frameMsg.vlanId, frameMsg.frameBuf,
                           frameMsg.frameLen, frameMsg.innerVlanId, frameMsg.client_idx);     /* PTin modified: DHCP snooping */
         osapiWriteLockGive(pppoeCfgRWLock);
       }
     }
   } /* end while true */

  return;
}

/*********************************************************************
* @purpose This routine receives any PPPoE and sends it to the
*          pppoe message queue and pppoeTask
*
* @param   bufHandle    @b{(input)} handle to the network buffer the PAE PDU is stored in
* @param   bpduInfo     @b{(input)} pointer to sysnet structure which contains the internal
*                                     interface number that the PDU was received on
*
* @returns L7_SUCCESS on a successful operation
* @returns L7_FAILURE for any error
*
* @comments This routine is registered with sysnet ethernet type 0x8863 at
*           pppoe init time.
*
* @end
*********************************************************************/
L7_RC_t pppoePduReceive(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uint32 rc;
  L7_uint32 len, ethHeaderSize;
  L7_uchar8 *data;
  L7_uint16 vlanId, innerVlanId = 0;

  L7_uint client_idx = (L7_uint)-1;   /* PTin added: DHCP snooping */

  LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "Packet intercepted: intIfNum=%u, vlanId=%u, innerVlanId=%u",
            pduInfo->intIfNum, pduInfo->vlanId, pduInfo->innerVlanId);

  /* Shouldn't get packets if PPPoE disabled, but if we do, ignore them. */
//if (dsCfgData->dsGlobalAdminMode != L7_ENABLE )
//{
//  #ifdef L7_DHCP_L2_RELAY_PACKAGE
//  if (dsCfgData->dsL2RelayAdminMode != L7_ENABLE)
//  {
//    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS)
//    {
//      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                  "(%s)Packet rx'ed is ignored as neither GBL Snooping nor L2 Relay is enabled at DHCP intercept.",
//                    __FUNCTION__);
//      dsTraceWrite(traceMsg);
//    }
//    return SYSNET_PDU_RC_IGNORED;
//  }
//  #else
//  return SYSNET_PDU_RC_IGNORED;
//  #endif
//}

  /* If either DHCP snooping or the L2 Relay is not enabled on
     rx interface, ignore packet. */
//if (dsVlanIntfIsSnooping(pduInfo->vlanId,pduInfo->intIfNum) /*dsIntfIsSnooping(pduInfo->intIfNum)*/ == L7_FALSE )   /* PTin modified: DHCP snooping */
//{
//  #ifdef L7_DHCP_L2_RELAY_PACKAGE
//  if ( _dsVlanIntfL2RelayGet(pduInfo->vlanId,pduInfo->intIfNum) /*_dsIntfL2RelayGet(pduInfo->intIfNum)*/ == L7_FALSE) /* PTin modified: DHCP snooping */
//  {
//    if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS)
//    {
//      L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//      osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                  "(%s)Packet rx'ed is ignored as neither INTF Snooping nor L2 Relay is enabled at DHCP intercept.",
//                    __FUNCTION__);
//      dsTraceWrite(traceMsg);
//    }
//    return SYSNET_PDU_RC_IGNORED;
//  }
//  #else
//  return SYSNET_PDU_RC_IGNORED;
//  #endif
//}

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);
  ethHeaderSize = sysNetDataOffsetGet(data);

  /* Convert vlan to root vlan */
  if ( ptin_pppoe_rootVlan_get( pduInfo->vlanId, &vlanId ) != L7_SUCCESS )
  {
     if (ptin_debug_pppoe_snooping)
       LOG_ERR(LOG_CTX_PTIN_DHCP,"Root vlan not found! (intIfNum=%u, vlanId=%u)",
               pduInfo->intIfNum, pduInfo->vlanId);
  }
  else
  {
     pduInfo->vlanId = vlanId;
  }

  /* This is used only when the packet comes double tagged.*/
  vlanId = pduInfo->vlanId;
  innerVlanId = pduInfo->innerVlanId;

  /* Only search and validate client for non Matrix (CXP360G, etc) and untrusted interfaces */
#if ( ! PTIN_BOARD_IS_MATRIX )

  ptin_client_id_t client;

//  if (!_pppoeVlanIntfTrustGet(pduInfo->vlanId,pduInfo->intIfNum))
  {
    #if 0
    /* Validate inner vlan */
    if (innerVlanId==0 || innerVlanId>=4095)
    {
      if (ptin_debug_dhcp_snooping)
        LOG_ERR(LOG_CTX_PTIN_DHCP,"Client not referenced! (intIfNum=%u, innerVlanId=%u, intVlanId=%u)",
              pduInfo->intIfNum, innerVlanId, vlanId);
      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);
      return SYSNET_PDU_RC_IGNORED;
    }
    #endif

    /* Client information */
    client.ptin_intf.intf_type = client.ptin_intf.intf_id = 0;
    client.innerVlan = innerVlanId;
    client.mask = PTIN_CLIENT_MASK_FIELD_INTF | PTIN_CLIENT_MASK_FIELD_INNERVLAN;

    /* Only search for a client, if inner vlan is valid */
    /* Otherwise, use dynamic DHCP */
    if (innerVlanId>0 && innerVlanId<4096)
    {
      /* Find client index, and validate it */
      if (ptin_pppoe_clientIndex_get(pduInfo->intIfNum, vlanId, &client, &client_idx)!=L7_SUCCESS ||
          client_idx>=PTIN_SYSTEM_MAXCLIENTS_PER_DHCP_INSTANCE)
      {
        LOG_NOTICE(LOG_CTX_PTIN_PPPOE,"Client not found! (intIfNum=%u, ptin_intf=%u/%u, innerVlanId=%u, intVlanId=%u)",
                   pduInfo->intIfNum, client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, vlanId);
//      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_INTERCEPTED);
//      ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, (L7_uint32)-1, DHCP_STAT_FIELD_RX_FILTERED);

//      SYSAPI_NET_MBUF_FREE(bufHandle);
//      return SYSNET_PDU_RC_IGNORED;
        client_idx = 0;
      }
    }
    else
    {
      client_idx = (L7_uint) -1;
    }
  }
#endif

  rc = pppoePacketQueue(data, len, vlanId, pduInfo->intIfNum, innerVlanId, &client_idx);    /* PTin modified: DHCP snooping */
  SYSAPI_NET_MBUF_FREE(bufHandle);

  /* Packet intercepted */
//  ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_INTERCEPTED);

  if (rc == L7_REQUEST_DENIED) /* DHCP Message got filtered, So Ignore
                                  For further processing */
  {
//  dsInfo->debugStats.msgsIntercepted++;
//  dsInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;
//  if (ptin_debug_dhcp_snooping)
//    LOG_TRACE(LOG_CTX_PTIN_DHCP,"Incremented DHCP_STAT_FIELD_RX_FILTERED");
//  ptin_dhcp_stat_increment_field(pduInfo->intIfNum, pduInfo->vlanId, client_idx, DHCP_STAT_FIELD_RX_FILTERED);
//  SYSAPI_NET_MBUF_FREE(bufHandle);
//  if (dsCfgData->dsTraceFlags & DS_TRACE_FRAME_RX)
//  {
//    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
//    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
//                  "(%s) Packet denied to be stored in packet queue",
//                  __FUNCTION__);
//    dsTraceWrite(traceMsg);
//  }
//  return SYSNET_PDU_RC_CONSUMED;
  }
  if (rc == L7_SUCCESS)
  {
//  dsInfo->debugStats.msgsIntercepted++;
//  dsInfo->debugStats.msgsInterceptedIntf[pduInfo->intIfNum]++;

  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Queue a PPPoE packet for processing on our own thread
*
*
* @param    ethHeader    @b{(input)} ethernet frame
* @param    dataLen      @b{(input)} length of ethernet frame
* @param    vlanId       @b{(input)} Vlan ID
* @param    intIfNum     @b{(input)} port where packet arrived
* @param    innerVlanId  @b{(input)} port where packet arrived
*
* @returns  L7_SUCCESS
*
* @notes    vlanId represents the existing VLANID if single tagged,
*                  represents the outer tag if double tagged.
*           innerVlanId represents the inner tag if double tagged.
*
* @end
*********************************************************************/
L7_RC_t pppoePacketQueue(L7_uchar8 *frame, L7_uint32 dataLen,
                         L7_ushort16 vlanId, L7_uint32 intIfNum,
                         L7_ushort16 innerVlanId, L7_uint *client_idx)
{
  pppoeFrameMsg_t pppoeFrameMsg;

  L7_pppoe_header_t *pppoeHeader;
  L7_ushort16 ethHdrLen;
  L7_ethHeader_t *ethHeader;

  ethHdrLen = sysNetDataOffsetGet(frame);
  ethHeader = (L7_ethHeader_t *) frame;
  pppoeHeader = (L7_pppoe_header_t *) (frame + ethHdrLen);

  /* Validate PPPoE frame length:
   * NOTE: the frame length includes padding for packets <64 bytes
   * Moreover, when inner VLAN is removed automatically on lower level APIs, the
   * frame size is reduced to 60 bytes only (including zero paddind) */ 
  if ((dataLen > 64) && (dataLen != (ethHdrLen + sizeof(L7_pppoe_header_t) + pppoeHeader->length)))
  {
    LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE malformed packet: invalid frame length (pktLen=%u ethHdrLen=%u pppoeHdrLen=%u pppoeDataLen=%u",
               dataLen, ethHdrLen, sizeof(L7_pppoe_header_t), pppoeHeader->length);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "Packet PPPoE received at intIfNum=%u, oVlan=%u, iVlan=%u, MAC=%02x:%02x:%02x:%02x:%02x:%02x",
            intIfNum, vlanId, innerVlanId,
            ethHeader->src.addr[0], ethHeader->src.addr[1], ethHeader->src.addr[2],
            ethHeader->src.addr[3], ethHeader->src.addr[4], ethHeader->src.addr[5]);

  /* Filter PPPoE packet based on security rules */
//if (pppoeFrameFilter(intIfNum, vlanId, frame, innerVlanId, client_idx))
//{
//   dsInfo->debugStats.msgsFiltered++;
//
//   if (ptin_debug_dhcp_snooping)
//      LOG_TRACE(LOG_CTX_PTIN_DHCP, "Incremented DHCP_STAT_FIELD_RX_FILTERED");
//   //ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX_FILTERED);
//   return L7_REQUEST_DENIED;
//}

//  dsInfo->debugStats.msgsReceived++;
//  ptin_dhcp_stat_increment_field(intIfNum, vlanId, *client_idx, DHCP_STAT_FIELD_RX);

  memcpy(&pppoeFrameMsg.frameBuf, frame, dataLen);
  pppoeFrameMsg.rxIntf = intIfNum;
  pppoeFrameMsg.vlanId = vlanId;
  pppoeFrameMsg.frameLen = dataLen;
  pppoeFrameMsg.innerVlanId = innerVlanId;
  pppoeFrameMsg.client_idx  = *client_idx;

  if (osapiMessageSend(pppoe_Packet_Queue, &pppoeFrameMsg, sizeof(pppoeFrameMsg_t), L7_NO_WAIT,
                       L7_MSG_PRIORITY_NORM) == L7_SUCCESS)
  {
    osapiSemaGive(pppoeMsgQSema);
  }
  else
  {
   /* This may be fairly normal, so don't log. DHCP should recover. */
//     dsInfo->debugStats.frameMsgTxError++;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Process a DHCP packet on DHCP snooping thread
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pppoeFrameProcess(L7_uint32 intIfNum, L7_ushort16 vlanId,
                          L7_uchar8 *frame, L7_uint32 frameLen,
                          L7_ushort16 innerVlanId, L7_uint client_idx)
{
   LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "PPPoE: Received new message");

   if(ptin_pppoe_is_intfTrusted(intIfNum, vlanId) == L7_TRUE)
   {
      pppoeProcessServerFrame(frame, intIfNum, vlanId, innerVlanId, client_idx);
   }
   else
   {
      pppoeProcessClientFrame(frame, intIfNum, vlanId, innerVlanId, client_idx);
   }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply security filtering rules to received DHCP packet.
*
* @param    intIfNum @b{(input)} receive interface
* @param    vlanId   @b{(input)} VLAN ID
* @param    frame    @b{(input)} ethernet frame
* @param    ipHeader @b{(input)} IP header inside frame
*
* @returns  L7_TRUE if frame is filtered
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL pppoeFrameFilter(L7_uint32 intIfNum, L7_ushort16 vlanId,
                      L7_uchar8 *frame, L7_ipHeader_t *ipHeader,
                      L7_ushort16 innerVlanId, L7_uint *client_idx)      /* PTin modified: DHCP snooping */
{
///* Discard server packets received on untrusted ports */
//if (dsFilterServerMessage(intIfNum, vlanId, frame, ipHeader, innerVlanId, client_idx))    /* PTin modified: DHCP snooping */
//{
//  if (ptin_debug_dhcp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: server filter");
//  return L7_TRUE;
//}
//
///* Discard certain client messages based on rx interface */
//if (dsFilterClientMessage(intIfNum, vlanId, frame, ipHeader, innerVlanId, client_idx))    /* PTin modified: DHCP snooping */
//{
//  if (ptin_debug_dhcp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: client filter");
//   return L7_TRUE;
//}
//
///* Verify that the source MAC matches the client hw address */
//if (dsFilterVerifyMac(intIfNum, vlanId, frame, ipHeader))
//{
//  if (ptin_debug_dhcp_snooping)
//    LOG_ERR(LOG_CTX_PTIN_DHCP,"Packet dropped here: verify MAC");
//  return L7_TRUE;
//}

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Add a new VendorID TLV to an existing PPPoE frame
* 
* @param    framePtr       Pointer to the new PPPoE frame
* @param    intIfNum       Interface in which the frame was received
* @param    vlanId         Vlan ID
* @param    innerVlanId    Inner Vlan ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    framePtr should point to the end of the previous TLV header.
*
* @end
*********************************************************************/
L7_RC_t pppoeAddVendorIdTlv(L7_uchar8 *framePtr, L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId)
{
   L7_uint8  circuitid_code, remoteid_code, circuitid_length, remoteid_length;
   L7_uint16 tlv_tag_type, tlv_length;
   L7_uint32 vendor_id;
   L7_char8  circuit_id[FD_DS_MAX_REMOTE_ID_STRING], remote_id[FD_DS_MAX_REMOTE_ID_STRING];

   if (ptin_pppoe_stringIds_get(intIfNum, vlanId, innerVlanId , L7_NULLPTR, circuit_id, remote_id) != L7_SUCCESS)
   {
      return L7_FAILURE;
   }

   /* Variable initialization */
   tlv_tag_type     = 0x0105;
   vendor_id        = 0x00000DE9;
   circuitid_code   = 0x01;
   circuitid_length = strlen(circuit_id);
   remoteid_code    = 0x02;
   remoteid_length  = strlen(remote_id);
   tlv_length       = sizeof(L7_uint32) + 2*(sizeof(L7_uint8)+sizeof(L7_uint8)) + circuitid_length + remoteid_length;

   /* TLV Tag Type */
   *((L7_uint16 *)framePtr) = tlv_tag_type;
   framePtr += sizeof(L7_uint16);

   /* TLV Length */
   *((L7_uint16 *)framePtr) = tlv_length;
   framePtr += sizeof(L7_uint16);

   /* Vendor ID */
   *((L7_uint32 *)framePtr) = vendor_id;
   framePtr += sizeof(L7_uint32);

   /* Circuit ID Code */
   *((L7_uint8 *)framePtr) = circuitid_code;
   framePtr += sizeof(L7_uint8);

   /* Circuit ID Length */
   *((L7_uint8 *)framePtr) = circuitid_length;
   framePtr += sizeof(L7_uint8);

   /* Circuit ID Data */
   memcpy(framePtr, circuit_id, circuitid_length);
   framePtr += circuitid_length;

   /* Remote ID Code */
   *((L7_uint8 *)framePtr) = remoteid_code;
   framePtr += sizeof(L7_uint8);

   /* Remote ID Length */
   *((L7_uint8 *)framePtr) = remoteid_length;
   framePtr += sizeof(L7_uint8);

   /* Remote ID Data */
   memcpy(framePtr, remote_id, remoteid_length);
   framePtr += remoteid_length;

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copy an existing TLV header to a new PPPoE frame
*
* @param    originalFramePtr  Pointer to the original TLV header
* @param    newFramePtr       Pointer to the new PPPoE frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pppoeCopyTlv(L7_uchar8 *originalFramePtr, L7_uchar8 *newFramePtr)
{
   L7_tlv_header_t *tlv_header;

   tlv_header = (L7_tlv_header_t*) originalFramePtr;
   memcpy(newFramePtr, originalFramePtr, sizeof(L7_tlv_header_t) + tlv_header->length);

   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a new CommonError PPPoE frame, based off on an
*           existing PPPoE frame
*
* @param    originalFramePtr  Pointer to the original PPPoE frame
* @param    newFramePtr       Pointer to the new PPPoE frame
* @param    newFrameLen       Length of the new frame
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pppoeCommonErrorFrameCreate(L7_uchar8 *originalFramePtr, L7_uchar8 *newFramePtr, L7_uint32 *newFrameLen)
{
   L7_uchar8         *eth_header_new_ptr, *pppoe_header_new_ptr, *tlv_header_new_ptr;
   L7_pppoe_header_t *pppoe_header_new;
   L7_ethHeader_t    *eth_header_new;
   L7_tlv_header_t   tlv_header;
   L7_uchar8         system_mac_addr;

   /* Copy the original frame up to the end of the PPPoE header */
   memset(newFramePtr, 0x00, PPPOE_PACKET_SIZE_MAX);
   memcpy(newFramePtr, originalFramePtr, sysNetDataOffsetGet(originalFramePtr) + sizeof(L7_pppoe_header_t));
   eth_header_new_ptr   = newFramePtr;
   eth_header_new       = (L7_ethHeader_t*) eth_header_new_ptr;
   pppoe_header_new_ptr = eth_header_new_ptr + sysNetDataOffsetGet(eth_header_new_ptr);
   pppoe_header_new     = (L7_pppoe_header_t*) pppoe_header_new_ptr;
   tlv_header_new_ptr   = pppoe_header_new_ptr + sizeof(L7_pppoe_header_t);

   /* Create a new TLV header and add it to the new PPPoE frame */
   memset(&tlv_header, 0x00, sizeof(L7_tlv_header_t));
   tlv_header.type   = L7_TLV_TAGTYPE_COMMON_ERR;
   tlv_header.length = 0;
   memcpy(tlv_header_new_ptr, &tlv_header, sizeof(L7_tlv_header_t));
   *newFrameLen      = sysNetDataOffsetGet(originalFramePtr) + sizeof(L7_pppoe_header_t) + sizeof(L7_tlv_header_t);

   /* If the PPPoE frame was PADI, then create a PADO. Otherwise, if it was a PADR, create a PADS */
   if(pppoe_header_new->code == L7_PPPOE_PADI)
   {
      pppoe_header_new->code = L7_PPPOE_PADO;
   }
   else if(pppoe_header_new->code == L7_PPPOE_PADR)
   {
      pppoe_header_new->code = L7_PPPOE_PADS;
   }

   /* Set the new PPPoE header length */
   pppoe_header_new->length = sizeof(L7_tlv_header_t);

   /* Set Ethernet header DST as the previous SRC, and the new SRC as the system MAC address */
   memcpy(&eth_header_new->dest.addr, &eth_header_new->src.addr, sizeof(L7_enetMacAddr_t));
   if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
   {
    simGetSystemIPBurnedInMac(&system_mac_addr);
   }
   else
   {
    simGetSystemIPLocalAdminMac(&system_mac_addr);
   }
   memcpy(&eth_header_new->src.addr, &system_mac_addr, sizeof(L7_enetMacAddr_t));

   return L7_SUCCESS;
}

/***********************************************************************
* @purpose Get the uplink interface for this PPPoE service
*
* @param    intIfNum   @b{(input)} outgoing interface
* @param    vlanId     @b{(input)} VLAN ID
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on all trusted ports in VLAN
*
* @end
*
***********************************************************************/
L7_RC_t pppoeServerInterfaceGet(L7_uint32 *intIfNum, L7_ushort16 vlanId)
{
  NIM_INTF_MASK_t portMask;

  if(intIfNum == L7_NULLPTR)
  {
     return L7_FAILURE;
  }

  if (ptin_evc_intfType_getList(vlanId,PTIN_EVC_INTF_ROOT,&portMask) == L7_SUCCESS)
  {
    for (*intIfNum = 1; *intIfNum < L7_MAX_INTERFACE_COUNT; ++(*intIfNum))
    {
      /* The API dot1qVlanEgressPortsGet returns both LAG interface as
         well as its particpating interfaces. So exclude the participating
         interfaces so that the underlying layer takes care of forwarding
         to participating interfaces. */
      if (dot3adIsLagActiveMember(*intIfNum) == L7_TRUE)
      {
        continue;
      }

      if (L7_INTF_ISMASKBITSET(portMask, *intIfNum))
      {
         return L7_SUCCESS;
      }
    }
  }

  return L7_FAILURE;
}

/***********************************************************************
* @purpose Send a PPPoE frame on a given interface
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    frame      @b{(input)} ethernet frame
* @param    frameLen   @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on all trusted ports in VLAN
*
* @end
*
***********************************************************************/
L7_RC_t pppoeClientFrameSend(L7_uint32 intIfNum, L7_uchar8* frame, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx)
{
  L7_uint16         extOVlan = vlanId, extIVlan = 0, frame_len;
  L7_uchar8         *dataStart, *pppoe_header_ptr;
  L7_netBufHandle   bufHandle;
  L7_BOOL           is_vlan_stacked;
  L7_pppoe_header_t *pppoe_header;
  L7_INTF_TYPES_t   sysIntfType;

  /* If outgoing interface is CPU interface, don't send it */
  if ((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (sysIntfType == L7_CPU_INTF))
  {
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    /* Don't bother logging this. mbuf alloc failures happen occasionally. DHCP
     * should recover. */
    return L7_FAILURE;
  }

  /* Determine frame length */
  pppoe_header_ptr     = frame + sysNetDataOffsetGet(frame);
  pppoe_header         = (L7_pppoe_header_t*) pppoe_header_ptr;
  frame_len            = sysNetDataOffsetGet(frame) + sizeof(L7_pppoe_header_t) + pppoe_header->length;

  /* Extract external outer and inner vlan for this tx interface */
  if (ptin_evc_extVlans_get_fromIntVlan(intIfNum,vlanId,innerVlanId,&extOVlan,&extIVlan)==L7_SUCCESS)
  {
    /* Check if vlan belongs to a stacked EVC */
    if (ptin_evc_check_is_stacked_fromIntVlan(vlanId,&is_vlan_stacked)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error checking if vlan %u belongs to a stacked EVC",vlanId);
      is_vlan_stacked = L7_TRUE;
    }

    /* Add inner vlan when there exists, and if vlan belongs to a stacked EVC */
    if (is_vlan_stacked && extIVlan!=0)
    {
      memmove(&frame[20],&frame[16],frame_len);
      frame[16] = 0x81;
      frame[17] = 0x00;
      frame[18] = extIVlan>>8;
      frame[19] = extIVlan & 0xff;
      frame_len += 4;
    }
    /* Modify outer vlan */
    if (vlanId!=extOVlan)
    {
      frame[14] &= 0xf0;
      frame[14] |= ((extOVlan>>8) & 0x0f);
      frame[15]  = extOVlan & 0xff;
    }
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, frame, frame_len);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frame_len);

  if (dtlIpBufSend(intIfNum, vlanId, bufHandle) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    LOG_TRACE(LOG_CTX_PTIN_PPPOE, " Failed to send PPPoE pkt on intf (%s) and vlanID (%d)", ifName, vlanId);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Packet transmited to intIfNum=%u, with oVlan=%u (intVlan=%u), iVlan=%u", intIfNum, extOVlan, vlanId, extIVlan);
  return L7_SUCCESS;
}

/***********************************************************************
* @purpose Send a PPPoE frame on a given interface
*
* @param    vlanId     @b{(input)} VLAN ID
* @param    frame      @b{(input)} ethernet frame
* @param    frameLen   @b{(input)} ethernet frame length, incl eth header (bytes)
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   Send on all trusted ports in VLAN
*
* @end
*
***********************************************************************/
L7_RC_t pppoeServerFrameSend(L7_uchar8* frame, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx)
{
  L7_uint16         extOVlan = vlanId, extIVlan = 0, frame_len;
  L7_uchar8         *dataStart, *pppoe_header_ptr;
  L7_netBufHandle   bufHandle;
  //L7_BOOL           is_vlan_stacked;
  L7_pppoe_header_t *pppoe_header;
  L7_INTF_TYPES_t   sysIntfType;
  L7_uint32         intIfNum;
  
  /* Get uplink interface */
  if(L7_SUCCESS != pppoeServerInterfaceGet(&intIfNum, vlanId))
  {
     return L7_FAILURE;
  }

  /* If outgoing interface is CPU interface, don't send it */
  if ((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (sysIntfType == L7_CPU_INTF))
  {
    return L7_SUCCESS;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL)
  {
    /* Don't bother logging this. mbuf alloc failures happen occasionally. DHCP
     * should recover. */
    return L7_FAILURE;
  }

  /* Determine frame length */
  pppoe_header_ptr     = frame + sysNetDataOffsetGet(frame);
  pppoe_header         = (L7_pppoe_header_t*) pppoe_header_ptr;
  frame_len            = sysNetDataOffsetGet(frame) + sizeof(L7_pppoe_header_t) + pppoe_header->length;

  /* Extract external outer and inner vlan for this tx interface */
  if (ptin_pppoe_extVlans_get(intIfNum, vlanId, innerVlanId, client_idx, &extOVlan, &extIVlan) == L7_SUCCESS)
  {
    #if 0
    /* Check if vlan belongs to a stacked EVC */
    if (ptin_evc_check_is_stacked_fromIntVlan(vlanId,&is_vlan_stacked)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_DHCP,"Error checking if vlan %u belongs to a stacked EVC",vlanId);
      is_vlan_stacked = L7_TRUE;
    }
    #endif
    /* Modify outer vlan */
    if (vlanId!=extOVlan)
    {
      frame[14] &= 0xf0;
      frame[14] |= ((extOVlan>>8) & 0x0f);
      frame[15]  = extOVlan & 0xff;
    }
    /* Add inner vlan when there exists, and if vlan belongs to a stacked EVC */
    if (/*is_vlan_stacked &&*/ extIVlan!=0)
    {
      /* No inner tag? */
      if (*((L7_uint16 *) &frame[16]) != 0x8100 &&
          *((L7_uint16 *) &frame[16]) != 0x88A8 &&
          *((L7_uint16 *) &frame[16]) != 0x9100)
      {
        memmove(&frame[20],&frame[16],frame_len);
        frame[16] = 0x81;
        frame[17] = 0x00;
        frame_len += 4;
      }
      frame[18] = (frame[14] & 0xe0) | ((extIVlan>>8) & 0x0f);
      frame[19] = extIVlan & 0xff;
    }
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, frame, frame_len);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, frame_len);

  if (dtlIpBufSend(intIfNum, vlanId, bufHandle) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    LOG_TRACE(LOG_CTX_PTIN_PPPOE, " Failed to send PPPoE pkt on intf (%s) and vlanID (%d)", ifName, vlanId);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_PPPOE,"Packet transmited to intIfNum=%u, with oVlan=%u (intVlan=%u), iVlan=%u", intIfNum, extOVlan, vlanId, extIVlan);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a client PPPoE frame
*
* @param    intIfNum       receive interface
* @param    vlanId         Vlan ID
* @param    innerVlanId    Inner Vlan ID
* @param    frame          ethernet frame
* @param    client_idx     Client index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void pppoeProcessClientFrame(L7_uchar8* frame, L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx)
{
   L7_uchar8                  *eth_header_ptr, *pppoe_header_ptr, *tlv_header_ptr;
   L7_uchar8                  *eth_header_copy_ptr, *pppoe_header_copy_ptr, *tlv_header_copy_ptr;
   L7_ethHeader_t             *eth_header;
   L7_pppoe_header_t          *pppoe_header, *pppoe_header_copy;
   L7_int32                   frame_len;
   L7_uchar8                  frame_copy[PPPOE_PACKET_SIZE_MAX];
   L7_BOOL                    tlv_vendor_id_found = L7_FALSE;
   ptinPppoeClientDataKey_t   binding_table_key, *result;

   LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "PPPoE: Received new client message");

   eth_header_ptr       = frame;
   eth_header           = (L7_ethHeader_t*) eth_header_ptr;
   pppoe_header_ptr     = eth_header_ptr + sysNetDataOffsetGet(eth_header_ptr);
   pppoe_header         = (L7_pppoe_header_t*) pppoe_header_ptr;

   /* Copy received frame up to the end of the PPPoE header */
   memset(frame_copy, 0x00, PPPOE_PACKET_SIZE_MAX);
   memcpy(frame_copy, frame, sysNetDataOffsetGet(eth_header_ptr) + sizeof(L7_pppoe_header_t) + pppoe_header->length);
   eth_header_copy_ptr   = frame_copy;
   pppoe_header_copy_ptr = eth_header_copy_ptr + sysNetDataOffsetGet(eth_header_copy_ptr);
   pppoe_header_copy     = (L7_pppoe_header_t*) pppoe_header_copy_ptr;

   /* If we received a PPPoE frame other than PADI/PADR/PADT on a client port, drop it */
   if(pppoe_header->code!=L7_PPPOE_PADI && pppoe_header->code!=L7_PPPOE_PADR && pppoe_header->code!=L7_PPPOE_PADT)
   {
      LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE: Received invalid message on an untrusted port. Dropped.");
      return;
   }

   /* Add this client to the PPPoE Binding Table */
   /* This entry will exist in this table until the server responds. At that moment, this entry is removed */
   memset(&binding_table_key,           0x00,                 sizeof(ptinPppoeClientDataKey_t));
   memcpy(binding_table_key.macAddr,    eth_header->src.addr, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
   binding_table_key.rootVlan  = vlanId;

   result = (ptinPppoeClientDataKey_t *) avlInsertEntry(&pppoeBindingTable.avlTree, (void *)&binding_table_key);
   if ( result == &binding_table_key )
   {
      LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE: Unable to register client (macAddr: %02X:%02X:%02X:%02X:%02X:%02X, rootVlan:%u).",
                 binding_table_key.macAddr[0],binding_table_key.macAddr[1],binding_table_key.macAddr[2],binding_table_key.macAddr[3],binding_table_key.macAddr[4],binding_table_key.macAddr[5],
                 binding_table_key.rootVlan);
   }
   else if (result == L7_NULLPTR )
   {
      ptinPppoeBindingInfoData_t *binding_table_data;

      if ((binding_table_data=avlSearchLVL7(&pppoeBindingTable.avlTree, (void *)&binding_table_key, AVL_EXACT)) != L7_NULL)
      {
         binding_table_data->interface  = intIfNum;
         binding_table_data->inner_vlan = innerVlanId;
      }
      else
      {
         LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE: Unable to find inserted client (macAddr: %02X:%02X:%02X:%02X:%02X:%02X, rootVlan:%u).",
                    binding_table_key.macAddr[0],binding_table_key.macAddr[1],binding_table_key.macAddr[2],binding_table_key.macAddr[3],binding_table_key.macAddr[4],binding_table_key.macAddr[5],
                    binding_table_key.rootVlan);
         return;
      }
   }

   /* Get pointer to the TLV headers */
   tlv_header_ptr      = pppoe_header_ptr + sizeof(L7_pppoe_header_t);
   tlv_header_copy_ptr = pppoe_header_copy_ptr + sizeof(L7_pppoe_header_t);
   frame_len           = pppoe_header->length;

   /* If the received PPPoE frame is PADT, immediatly send it */
   if(pppoe_header->code == L7_PPPOE_PADT)
   {
      pppoeServerFrameSend(frame, vlanId, innerVlanId, client_idx);
      return;        
   }  

   /* Parse every TLV header */
   while(frame_len > 0)
   {
      L7_tlv_header_t *tlv_header;

      tlv_header = (L7_tlv_header_t*) tlv_header_ptr;

      switch(tlv_header->type)
      {
         case L7_TLV_TAGTYPE_VENDOR_ID:
         {
            L7_tlv_header_t *tlv_header_new;
            L7_uint32       frame_copy_len;

            LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "Found a Vendor ID Tag Type. Replacing.");
            pppoeAddVendorIdTlv(tlv_header_copy_ptr, intIfNum, vlanId, innerVlanId);
            tlv_vendor_id_found       = L7_TRUE;
            tlv_header_new            = (L7_tlv_header_t*) tlv_header_copy_ptr;
            tlv_header_copy_ptr       += sizeof(L7_tlv_header_t) + tlv_header_new->length;
            pppoe_header_copy->length += sizeof(L7_tlv_header_t) + tlv_header_new->length;

            /* If the final frame len exceeds PPPOE_PACKET_SIZE_MAX, drop the packet and return a PADO/PADS with L7_TLV_TAGTYPE_COMMON_ERR */
            if((sysNetDataOffsetGet(eth_header_ptr) + sizeof(L7_pppoe_header_t) + pppoe_header->length + sizeof(L7_tlv_header_t) + tlv_header_new->length) > PPPOE_PACKET_SIZE_MAX)
            {
               LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE: New frame exceeds MTU. Dropping and returning a PADO/PADS with code 0x0203.");

               pppoeCommonErrorFrameCreate(frame, frame_copy, &frame_copy_len);
               pppoeClientFrameSend(intIfNum, frame_copy, vlanId, innerVlanId, client_idx);
               return;
            }
            break;
         }
         default:
         {
            /* Copy existing TLV header to our new frame */
            pppoeCopyTlv(tlv_header_ptr, tlv_header_copy_ptr);
            tlv_header_copy_ptr += sizeof(L7_tlv_header_t) + tlv_header->length;
         }
      }

      /* We finished parsing this TLV. Advance to the next one */
      tlv_header_ptr += sizeof(L7_tlv_header_t) + tlv_header->length;
      frame_len      -= sizeof(L7_tlv_header_t) + tlv_header->length;
   }

   /* If the VENDOR ID TLV header is not present in the frame, add it */
   if(L7_TRUE != tlv_vendor_id_found)
   {
      L7_tlv_header_t *tlv_header_new;

      LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "Adding vendor ID TLV.");
      pppoeAddVendorIdTlv(tlv_header_copy_ptr, intIfNum, vlanId, innerVlanId);

      tlv_header_new            = (L7_tlv_header_t*) tlv_header_copy_ptr;
      pppoe_header_copy->length += sizeof(L7_tlv_header_t) + tlv_header_new->length;
   }

   pppoeServerFrameSend(frame_copy, vlanId, innerVlanId, client_idx);
}

/*********************************************************************
* @purpose  Process a server PPPoE frame
*
* @param    intIfNum       receive interface
* @param    vlanId         Vlan ID
* @param    innerVlanId    Inner Vlan ID
* @param    frame          ethernet frame
* @param    client_idx     Client index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void pppoeProcessServerFrame(L7_uchar8* frame, L7_uint32 intIfNum, L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint client_idx)
{
   L7_uchar8                  *eth_header_ptr, *pppoe_header_ptr, *tlv_header_ptr;
   L7_uchar8                  *eth_header_copy_ptr, *pppoe_header_copy_ptr, *tlv_header_copy_ptr;
   L7_ethHeader_t             *eth_header;
   L7_pppoe_header_t          *pppoe_header, *pppoe_header_copy;
   L7_uint32                  frame_len;
   L7_uchar8                  frame_copy[PPPOE_PACKET_SIZE_MAX];
   ptinPppoeClientDataKey_t   binding_table_key;
   ptinPppoeBindingInfoData_t *binding_table_data;

   LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "PPPoE: Received new server message");

   eth_header_ptr       = frame;
   eth_header           = (L7_ethHeader_t*) eth_header_ptr;
   pppoe_header_ptr     = eth_header_ptr + sysNetDataOffsetGet(eth_header_ptr);
   pppoe_header         = (L7_pppoe_header_t*) pppoe_header_ptr;

   /* Copy received frame up to the end of the PPPoE header */
   memset(frame_copy, 0x00, PPPOE_PACKET_SIZE_MAX);
   memcpy(frame_copy, frame, sysNetDataOffsetGet(eth_header_ptr) + sizeof(L7_pppoe_header_t) + pppoe_header->length);
   eth_header_copy_ptr   = frame_copy;
   pppoe_header_copy_ptr = eth_header_copy_ptr + sysNetDataOffsetGet(eth_header_copy_ptr);
   pppoe_header_copy     = (L7_pppoe_header_t*) pppoe_header_copy_ptr;

   /* If we received a PPPoE frame other than PADO/PADS/PADT on a server port, drop it */
   if(pppoe_header->code!=L7_PPPOE_PADO && pppoe_header->code!=L7_PPPOE_PADS && pppoe_header->code!=L7_PPPOE_PADT)
   {
      LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE: Received invalid message on a trusted port. Dropped.");
      return;
   }

   /* Search for this client in the PPPoE Binding Table */
   memset(&binding_table_key,           0x00,                  sizeof(ptinPppoeClientDataKey_t));
   memcpy(binding_table_key.macAddr,    eth_header->dest.addr, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);
   binding_table_key.rootVlan     = vlanId;
   if ((binding_table_data=avlSearchLVL7(&pppoeBindingTable.avlTree, (void *)&binding_table_key, AVL_EXACT)) != L7_NULL)
   {
      intIfNum    = binding_table_data->interface;
      innerVlanId = binding_table_data->inner_vlan;
      avlDeleteEntry(&pppoeBindingTable.avlTree, (void *)&binding_table_key);
   }
   else
   {
      LOG_NOTICE(LOG_CTX_PTIN_PPPOE, "PPPoE: Message received for an unknown client (macAddr: %02X:%02X:%02X:%02X:%02X:%02X, rootVlan:%u).",
                 binding_table_key.macAddr[0],binding_table_key.macAddr[1],binding_table_key.macAddr[2],binding_table_key.macAddr[3],binding_table_key.macAddr[4],binding_table_key.macAddr[5],
                 binding_table_key.rootVlan);
      return;
   }

   /* Get pointer to the TLV headers */
   tlv_header_ptr      = pppoe_header_ptr + sizeof(L7_pppoe_header_t);
   tlv_header_copy_ptr = pppoe_header_copy_ptr + sizeof(L7_pppoe_header_t);
   frame_len           = pppoe_header->length;

   /* If the received PPPoE frame is PADT, immediatly send it */
   if(pppoe_header->code == L7_PPPOE_PADT)
   {
      pppoeClientFrameSend(intIfNum, frame, vlanId, innerVlanId, client_idx);
      return;        
   }  

   /* Parse every TLV header */
   while(frame_len > 0)
   {
      L7_tlv_header_t *tlv_header;

      tlv_header = (L7_tlv_header_t*) tlv_header_ptr;

      switch(tlv_header->type)
      {
         case L7_TLV_TAGTYPE_VENDOR_ID:
         {
            L7_tlv_header_t *tlv_header_new;

            LOG_DEBUG(LOG_CTX_PTIN_PPPOE, "Found a Vendor ID Tag Type. Removing it.");

            tlv_header_new            = (L7_tlv_header_t*) tlv_header_copy_ptr;
            pppoe_header_copy->length -= sizeof(L7_tlv_header_t) + tlv_header_new->length;

            break;
         }
         default:
         {
            /* Copy existing TLV header to our new frame */
            pppoeCopyTlv(tlv_header_ptr, tlv_header_copy_ptr);
            tlv_header_copy_ptr += sizeof(L7_tlv_header_t) + tlv_header->length;
         }
      }

      /* We finished parsing this TLV. Advance to the next one */
      tlv_header_ptr += sizeof(L7_tlv_header_t) + tlv_header->length;
      frame_len      -= sizeof(L7_tlv_header_t) + tlv_header->length;
   }

   pppoeClientFrameSend(intIfNum, frame_copy, vlanId, innerVlanId, client_idx);
}

#if 0
void ptin_pppoe_bindtable_dump(void)
{
  L7_uint i, i_client;
  pppoeBindingTable avl_key;
  ptinDhcpClientInfoData_t *avl_info;

  for (i = 0; i < PTIN_SYSTEM_N_DHCP_INSTANCES; i++)
  {
    if (!dhcpInstances[i].inUse) {
      printf("*** Dhcp instance %02u not in use\r\n", i);
      continue;
    }

    printf("DHCP instance %02u: EVC_idx=%-5u NNI_VLAN=%-4u #evcs=%-5u options=0x%04x [CircuitId Template: %s]  ", i,
           dhcpInstances[i].UcastEvcId, dhcpInstances[i].nni_ovid, dhcpInstances[i].n_evcs,
           dhcpInstances[i].evcDhcpOptions, dhcpInstances[i].circuitid.template_str);
    printf("\r\n");
    i_client = 0;

    /* Run all cells in AVL tree */
    memset(&avl_key,0x00,sizeof(ptinDhcpClientDataKey_t));
    while ( ( avl_info = (ptinDhcpClientInfoData_t *)
                          avlSearchLVL7(&dhcpInstances[i].dhcpClients.avlTree.dhcpClientsAvlTree, (void *)&avl_key, AVL_NEXT)
            ) != L7_NULLPTR )
    {
      /* Prepare next key */
      memcpy(&avl_key, &avl_info->dhcpClientDataKey, sizeof(ptinDhcpClientDataKey_t));

      printf("   Client#%u: "
             #if (DHCP_CLIENT_INTERF_SUPPORTED)
             "ptin_port=%-2u "
             #endif
             #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
             "svlan=%-4u "
             #endif
             #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
             "cvlan=%-4u "
             #endif
             #if (DHCP_CLIENT_IPADDR_SUPPORTED)
             "IP=%03u.%03u.%03u.%03u "
             #endif
             #if (DHCP_CLIENT_MACADDR_SUPPORTED)
             "MAC=%02x:%02x:%02x:%02x:%02x:%02x "
             #endif
             ": index=%-4u  [uni_vlans=%4u+%-4u] options=0x%04x circuitId=\"%s\" remoteId=\"%s\"\r\n",
             i_client,
             #if (DHCP_CLIENT_INTERF_SUPPORTED)
             avl_info->dhcpClientDataKey.ptin_port,
             #endif
             #if (DHCP_CLIENT_OUTERVLAN_SUPPORTED)
             avl_info->dhcpClientDataKey.outerVlan,
             #endif
             #if (DHCP_CLIENT_INNERVLAN_SUPPORTED)
             avl_info->dhcpClientDataKey.innerVlan,
             #endif
             #if (DHCP_CLIENT_IPADDR_SUPPORTED)
             (avl_info->dhcpClientDataKey.ipv4_addr>>24) & 0xff,
              (avl_info->dhcpClientDataKey.ipv4_addr>>16) & 0xff,
               (avl_info->dhcpClientDataKey.ipv4_addr>>8) & 0xff,
                avl_info->dhcpClientDataKey.ipv4_addr & 0xff,
             #endif
             #if (DHCP_CLIENT_MACADDR_SUPPORTED)
             avl_info->dhcpClientDataKey.macAddr[0],
              avl_info->dhcpClientDataKey.macAddr[1],
               avl_info->dhcpClientDataKey.macAddr[2],
                avl_info->dhcpClientDataKey.macAddr[3],
                 avl_info->dhcpClientDataKey.macAddr[4],
                  avl_info->dhcpClientDataKey.macAddr[5],
             #endif
             avl_info->client_index,
             avl_info->uni_ovid, avl_info->uni_ivid,
             avl_info->client_data.dhcp_options,
             avl_info->client_data.circuitId_str,
             avl_info->client_data.remoteId_str);

      i_client++;
    }
  }
  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
  printf("Total number of QUATTRO-P2P evcs: %u\r\n", dhcp_quattro_p2p_evcs);
  #endif
}
#endif

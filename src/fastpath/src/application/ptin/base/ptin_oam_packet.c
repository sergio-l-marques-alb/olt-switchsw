/**
 * ptin_oam_packet.c
 *  
 * Implements OAM Packet (R-APS and CCM) capture and Process 
 * routines 
 *
 * @author joaom (6/11/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */

#include "ptin_oam_packet.h"
#include "ptin_hal_erps.h"
#include "ptin_evc.h"
#include "ptin_intf.h"
#include "dtl_ptin.h"

/* R-APS MAC address */
#define PTIN_APS_MACADDR  {0x01,0x19,0xA7,0x00,0x00,0x00}   // Correct Ring ID (last Byte) will be seted on each rule/trap creation.

/* CCM MAC address */
L7_uchar8 ccmMacAddr[L7_MAC_ADDR_LEN] = {0x01,0x80,0xC2,0x00,0x00,0x37};  // Last Nibble is the MEG Level -> Fixed = 7

#include <unistd.h>

/* Maximum number of messages APS in queue */
#define PTIN_APS_PACKET_MAX_MESSAGES  128

/* Maximum number of messages APS in queue */
#define PTIN_CCM_PACKET_MAX_MESSAGES  256

/*************** 
 * PROTOTYPES
 ***************/

#ifdef PTIN_ENABLE_ERPS

/* Queue id */
void *ptin_aps_packetRx_queue[MAX_PROT_PROT_ERPS] = {L7_NULLPTR};

/* Callback to be called for APS packets. */
L7_RC_t ptin_aps_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/* Forward an APS packet on a specified interface and vlan */
L7_RC_t ptin_aps_packet_forward(L7_uint8 erps_idx, ptin_APS_PDU_Msg_t *pktMsg);

#endif  // PTIN_ENABLE_ERPS


/* Queue id */
void *ptin_ccm_packetRx_queue                     = L7_NULLPTR;

/* Callback to be called for CCM packets. */
L7_RC_t ptin_ccm_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);



/*****************
 * DEBUG ROUTINES
 *****************/

L7_BOOL ptin_oam_packet_debug_enable = L7_FALSE;

void ptin_oam_packet_debug( L7_BOOL enable)
{
  ptin_oam_packet_debug_enable = enable & 1;
}


/**********************
 * EXTERNAL ROUTINES
 **********************/

/**
 * Configure APS packet trapping in HW
 * 
 * @author joaom (6/13/2013)
 * 
 * @param vlanId HW rule will not use this, only MAC is used to 
 *               create the rule
 * @param ringId 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
#ifdef PTIN_ENABLE_ERPS
L7_RC_t ptin_aps_packet_vlan_trap(L7_uint16 vlanId, L7_uint8 ringId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.oamConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.oamConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.oamConfig.vlanId      = vlanId;
  dapiCmd.cmdData.oamConfig.level       = ringId;
  dapiCmd.cmdData.oamConfig.packet_type = PTIN_PACKET_APS;  

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}

/**
 * Set global enable for APS packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_aps_packet_global_trap(L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  dapiCmd.cmdData.oamConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.oamConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.oamConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.oamConfig.level       = 0;
  dapiCmd.cmdData.oamConfig.packet_type = PTIN_PACKET_APS;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting global enable to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

#endif  // PTIN_ENABLE_ERPS


/**
 * Configure CCM packet trapping in HW
 * 
 * @author joaom (6/13/2013)
 * 
 * @param vlanId HW rule will not use this, only MAC is used to 
 *               create the rule
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_ccm_packet_vlan_trap(L7_uint16 vlanId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.oamConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.oamConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.oamConfig.vlanId      = vlanId;
  dapiCmd.cmdData.oamConfig.level       = 0;
  dapiCmd.cmdData.oamConfig.packet_type = PTIN_PACKET_CCM;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}


/**
 * Initialize APS module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
#ifdef PTIN_ENABLE_ERPS
L7_RC_t ptin_aps_packet_init(L7_uint8 erps_idx)
{
  sysnetNotifyEntry_t snEntry;
  L7_uint8            queue_str[24];
  L7_uchar8           apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID
  L7_uint8            i;

  sprintf(queue_str, "PTin_APS_PacketRx_Queue%d", erps_idx);

  /* Queue that will process timer events */
  ptin_aps_packetRx_queue[erps_idx] = (void *) osapiMsgQueueCreate(queue_str,
                                                       PTIN_APS_PACKET_MAX_MESSAGES, PTIN_APS_PDU_MSG_SIZE);
  if (ptin_aps_packetRx_queue[erps_idx] == L7_NULLPTR) {
    LOG_FATAL(LOG_CTX_ERPS,"APS packet queue %d creation error.", erps_idx);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_ERPS,"APS packet queue %d created.", erps_idx);


  /* Ring ID needs to be seted on rule/trap creation. */
  apsMacAddr[5] = tbl_erps[erps_idx].protParam.ringId;

  /* Rx_callback is always the same; Do not try to register a snEntry if the MAC is already registered (same Ring Id) */
  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
    if (i == erps_idx) continue;
    if ( (tbl_erps[i].admin == PROT_ERPS_ENTRY_BUSY) && (tbl_erps[i].protParam.ringId == tbl_erps[erps_idx].protParam.ringId) ) return L7_SUCCESS;
  }

  /* Register APS packets */
  strcpy(snEntry.funcName, "ptin_aps_packetRx_callback");
  snEntry.notify_pdu_receive = ptin_aps_packetRx_callback;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, apsMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_ERPS, "Cannot register ptin_aps_packetRx_callback callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_ERPS, "ptin_aps_packetRx_callback registered!");

  return L7_SUCCESS;
}
#endif  // PTIN_ENABLE_ERPS


/**
 * Initialize CCM module
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_ccm_packet_init(void)
{
  sysnetNotifyEntry_t snEntry;  

  /* Queue that will process timer events */
  ptin_ccm_packetRx_queue = (void *) osapiMsgQueueCreate("PTin_CCM_PacketRx_Queue",
                                                       PTIN_CCM_PACKET_MAX_MESSAGES, PTIN_CCM_PDU_MSG_SIZE);
  if (ptin_ccm_packetRx_queue == L7_NULLPTR) {
    LOG_FATAL(LOG_CTX_OAM,"CCM packet queue creation error.");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_OAM,"CCM packet queue created.");

  /* Register CCM packets */
  strcpy(snEntry.funcName, "ptin_ccm_packetRx_callback");
  snEntry.notify_pdu_receive = ptin_ccm_packetRx_callback;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, ccmMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_OAM, "Cannot register ptin_ccm_packetRx_callback callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_OAM, "ptin_ccm_packetRx_callback registered!");

  return L7_SUCCESS;
}


/**
 * DEInitialize ptin_aps_packet module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
#ifdef PTIN_ENABLE_ERPS
L7_RC_t ptin_aps_packet_deinit(L7_uint8 erps_idx)
{
  sysnetNotifyEntry_t snEntry;
  L7_uchar8           apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID
  L7_uchar8           i;
  L7_BOOL             inused = L7_FALSE;

  /* Ring ID needs to be seted on rule/trap removal. */
  apsMacAddr[5] = tbl_erps[erps_idx].protParam.ringId;

  /* Rx_callback is always the same; Do not unregister a snEntry if the MAC is in used by others (same Ring Id) */
  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
    if (i == erps_idx) continue;
    if ( (tbl_erps[i].admin == PROT_ERPS_ENTRY_BUSY) && (tbl_erps[i].protParam.ringId == tbl_erps[erps_idx].protParam.ringId) ) inused = L7_TRUE;
  }

  if (!inused) {
    /* Deregister broadcast packets capture */
    strcpy(snEntry.funcName, "ptin_aps_packetRx_callback");
    snEntry.notify_pdu_receive = ptin_aps_packetRx_callback;
    snEntry.type = SYSNET_MAC_ENTRY;
    memcpy(snEntry.u.macAddr, apsMacAddr, L7_MAC_ADDR_LEN);
    if (sysNetDeregisterPduReceive(&snEntry) != L7_SUCCESS) {
      LOG_ERR(LOG_CTX_ERPS, "Cannot unregister ptin_aps_packetRx_callback callback!");
      return L7_FAILURE;
    }
    LOG_INFO(LOG_CTX_ERPS, "ptin_aps_packetRx_callback unregistered!");
  }

  /* Queue that will process timer events */
  if (ptin_aps_packetRx_queue[erps_idx] != L7_NULLPTR) {
    osapiMsgQueueDelete(ptin_aps_packetRx_queue[erps_idx]);
    ptin_aps_packetRx_queue[erps_idx] = L7_NULLPTR;
  }

  LOG_INFO(LOG_CTX_ERPS, "PTin APS packet deinit OK");

  return L7_SUCCESS;
}
#endif  // PTIN_ENABLE_ERPS


/**
 * DEInitialize ptin_ccm_packet module
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_ccm_packet_deinit(void)
{
  sysnetNotifyEntry_t snEntry;

  /* Deregister broadcast packets capture */
  strcpy(snEntry.funcName, "ptin_ccm_packetRx_callback");
  snEntry.notify_pdu_receive = ptin_ccm_packetRx_callback;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, ccmMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetDeregisterPduReceive(&snEntry) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_OAM, "Cannot unregister ptin_ccm_packetRx_callback callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_OAM, "ptin_ccm_packetRx_callback unregistered!");

  /* Queue that will process timer events */
  if (ptin_ccm_packetRx_queue != L7_NULLPTR) {
    osapiMsgQueueDelete(ptin_ccm_packetRx_queue);
    ptin_ccm_packetRx_queue = L7_NULLPTR;
  }

  LOG_INFO(LOG_CTX_OAM, "PTin CCM packet deinit OK");

  return L7_SUCCESS;
}


/**
 * Callback to be called for APS packets.
 * 
 * @param bufHandle 
 * @param pduInfo 
 * 
 * @return L7_RC_t : L7_FAILURE (always)
 */
#ifdef PTIN_ENABLE_ERPS
L7_RC_t ptin_aps_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8           *payload;
  L7_uint32           payloadLen;
  ptin_APS_PDU_Msg_t  msg;
  L7_uchar8           apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID

  L7_uint32 intIfNum    = pduInfo->intIfNum;     /* Source port */
  L7_uint16 vlanId      = pduInfo->vlanId;       /* Vlan */
  L7_uint16 innerVlanId = pduInfo->innerVlanId;  /* Inner vlan */

  L7_RC_t rc = L7_SUCCESS;


  if (ptin_oam_packet_debug_enable)
    LOG_TRACE(LOG_CTX_ERPS,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX ) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Invalid vlanId %u",vlanId);
    return L7_FAILURE;
  }

  /* Packet should be APS Mac Addr (last bytes is ignored) */
  if ( memcmp(&payload[0], apsMacAddr, (L7_MAC_ADDR_LEN-1))!=0 ) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Not an APS Packet");
    return L7_FAILURE;
  }

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"intIfNum %u and vlan %u does not belong to any valid EVC/interface", intIfNum, vlanId);
    return L7_FAILURE;
  }

  /* Send packet to queue */
  memset(&msg, 0x00, sizeof(msg));
  msg.msgId       = PTIN_APS_PACKET_MESSAGE_ID;
  msg.intIfNum    = pduInfo->intIfNum;
  msg.vlanId      = pduInfo->vlanId;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.payload     = payload;
  msg.payloadLen  = payloadLen;
  msg.bufHandle   = bufHandle;  

  if (erpsIdx_from_internalVlan[vlanId] != PROT_ERPS_UNUSEDIDX) {
    ptin_aps_packet_forward(erpsIdx_from_internalVlan[vlanId], &msg);
    rc = osapiMessageSend(ptin_aps_packetRx_queue[erpsIdx_from_internalVlan[vlanId]], &msg, PTIN_APS_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  }

  if (ptin_oam_packet_debug_enable)
    LOG_TRACE(LOG_CTX_ERPS,"Packet sent to queue %d",erpsIdx_from_internalVlan[vlanId]);

  if (rc != L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Failed message sending to ptin_aps_packet queue");
    return L7_FAILURE;
  }

  /* Return failure to guarantee these packets are consumed by other entities */
  return L7_FAILURE;
}
#endif  // PTIN_ENABLE_ERPS


/**
 * Callback to be called for CCM packets.
 * 
 * @param bufHandle 
 * @param pduInfo 
 * 
 * @return L7_RC_t : L7_FAILURE (always)
 */
L7_RC_t ptin_ccm_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8 *payload;
  L7_uint32 payloadLen;
  ptin_CCM_PDU_Msg_t msg;

  L7_uint32 intIfNum    = pduInfo->intIfNum;     /* Source port */
  L7_uint16 vlanId      = pduInfo->vlanId;       /* Vlan */
  L7_uint16 innerVlanId = pduInfo->innerVlanId;  /* Inner vlan */

  L7_RC_t rc = L7_SUCCESS;

  if (ptin_oam_packet_debug_enable)
    LOG_TRACE(LOG_CTX_OAM,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  /*if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX ) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_OAM,"Invalid vlanId %u",vlanId);
    return L7_FAILURE;
  }*/

  /* Packet should be CCM type */
  if ( memcmp(&payload[0], ccmMacAddr, L7_MAC_ADDR_LEN)!=0 ) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_OAM,"Not a CCM Packet");
    return L7_FAILURE;
  }

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  /*if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_OAM,"intIfNum %u and vlan %u does not belong to any valid EVC/interface");
    return L7_FAILURE;
  }*/

  /* Send packet to queue */
  memset(&msg, 0x00, sizeof(msg));
  msg.msgId       = PTIN_CCM_PACKET_MESSAGE_ID;
  msg.intIfNum    = pduInfo->intIfNum;
  msg.vlanId      = pduInfo->vlanId;
  msg.innerVlanId = pduInfo->innerVlanId;
  msg.payload     = payload;
  msg.payloadLen  = payloadLen;
  msg.bufHandle   = bufHandle;

  rc = osapiMessageSend(ptin_ccm_packetRx_queue, &msg, PTIN_CCM_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_OAM,"Failed message sending to ptin_ccm_packet queue");
    return L7_FAILURE;
  }

  //LOG_WARNING(LOG_CTX_OAM,"Decide what to do with CCM packet! Packets are in proper queue waiting for someone to process. NOT YET DONE!!!");

  /* Return failure to guarantee these packets are consumed by other entities */
  return L7_FAILURE;
}


/**
 * Process ASP packet
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
#ifdef PTIN_ENABLE_ERPS
L7_RC_t ptin_aps_packetRx_process(L7_uint32 queueidx, L7_uint8 *aps_reqstate, L7_uint8 *aps_status, L7_uint8 *aps_nodeid, L7_uint32 *aps_rxport)
{
  L7_uint32           status;
  ptin_APS_PDU_Msg_t  msg;
  int i;
  aps_frame_t         *aps_frame;

  //LOG_TRACE(LOG_CTX_ERPS,"Any APS packet received?");
  if (queueidx > MAX_PROT_PROT_ERPS) {
    LOG_ERR(LOG_CTX_OAM, "queueidx (%d) out of range", queueidx);
    return L7_FAILURE;
  }

  status = (L7_uint32) osapiMessageReceive(ptin_aps_packetRx_queue[queueidx],
                                           (void*) &msg,
                                           PTIN_APS_PDU_MSG_SIZE,
                                           L7_NO_WAIT);

  if (status == L7_SUCCESS) {
    if ( msg.msgId == PTIN_APS_PACKET_MESSAGE_ID ) {
      if (ptin_oam_packet_debug_enable) {
        LOG_TRACE(LOG_CTX_ERPS,"APS packet received: intIfNum %d, vlanId %d, innerVlanId %d, payloadLen %d", msg.intIfNum, msg.vlanId, msg.innerVlanId, msg.payloadLen);
        for (i=0; i<msg.payloadLen; i++) {
          printf("%.2x ", msg.payload[i]);
        }
        printf("\n\r");
      }

      aps_frame = (aps_frame_t*) msg.payload;

      *aps_reqstate       = aps_frame->aspmsg.req_state_subcode;
      *aps_status         = aps_frame->aspmsg.status;
      memcpy(aps_nodeid,  aps_frame->aspmsg.nodeid, L7_ENET_MAC_ADDR_LEN);
      *aps_rxport         = msg.intIfNum;

      if (ptin_oam_packet_debug_enable)
        LOG_TRACE(LOG_CTX_ERPS,"aps_rxport %d", *aps_rxport);

      return L7_SUCCESS;
    } else {
      if (ptin_oam_packet_debug_enable)
        LOG_TRACE(LOG_CTX_ERPS,"APS packet received with Unknown ID");
      return L7_FAILURE;
    }
  }
  //else {
  //  if (ptin_oam_packet_debug_enable)
  //    LOG_TRACE(LOG_CTX_ERPS,"No packet from ptin_aps_packet queue#%d (status = %d)", queueidx, status);
  //}

  return L7_FAILURE;
}
#endif  // PTIN_ENABLE_ERPS


/**
* Send a packet on a specified interface and vlan
*
* @param    intIfNum   @b{(input)} Outgoing internal interface number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @return  void
*/
void ptin_oam_packet_send(L7_uint32 intfNum,
                          L7_uint32 vlanId,
                          L7_uchar8 *payload,
                          L7_uint32 payloadLen)
{
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  DTL_CMD_TX_INFO_t dtlCmd;


  /* If outgoing interface is CPU interface, don't send it */
  if ( (nimGetIntfType(intfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_CPU_INTF) ) {
    return;
  }

  SYSAPI_NET_MBUF_GET(bufHandle);
  if (bufHandle == L7_NULL) {
    return;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
  memcpy(dataStart, payload, payloadLen);
  SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, payloadLen);

  memset((L7_uchar8 *)&dtlCmd, 0, sizeof(DTL_CMD_TX_INFO_t));

  dtlCmd.intfNum             = intfNum;
  dtlCmd.priority            = 1;
  dtlCmd.typeToSend          = (intfNum == L7_ALL_INTERFACES)? DTL_VLAN_MULTICAST : DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;
  dtlCmd.cmdType.L2.vlanId   = vlanId;

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);

  if (ptin_oam_packet_debug_enable)
    LOG_TRACE(LOG_CTX_ERPS,"APS Packet transmited to intIfNum=%u", intfNum);

  return;
}


/**
 * Send an APS packet on a specified interface and vlan
 * 
 * @author joaom (6/17/2013)
 * 
 * @param erps_idx 
 * @param reqstate_subcode 
 * @param status 
 */
#ifdef PTIN_ENABLE_ERPS
void ptin_aps_packet_send(L7_uint8 erps_idx, L7_uint8 reqstate_subcode, L7_uint8 status)
{
  aps_frame_t aps_frame;
  L7_uchar8   apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID

  // Last Byte is the Ring ID
  apsMacAddr[5] = tbl_erps[erps_idx].protParam.ringId;

  memcpy(aps_frame.dmac,              apsMacAddr, L7_ENET_MAC_ADDR_LEN);
  memcpy(aps_frame.smac,              srcMacAddr, L7_ENET_MAC_ADDR_LEN);

  aps_frame.vlan_tag[0]               = 0x81;
  aps_frame.vlan_tag[1]               = 0x00;
  aps_frame.vlan_tag[2]               = 0xE0 | ((tbl_erps[erps_idx].protParam.controlVid>>8) & 0xF);
  aps_frame.vlan_tag[3]               = tbl_erps[erps_idx].protParam.controlVid & 0x0FF;
  aps_frame.etherType                 = L7_ETYPE_CFM;

  aps_frame.aspmsg.mel_version        = 0x21;  // MEG Level 1; Version 1
  aps_frame.aspmsg.opCode             = 40;
  aps_frame.aspmsg.flags              = 0x00;
  aps_frame.aspmsg.tlvOffset          = 0x20;
  aps_frame.aspmsg.req_state_subcode  = reqstate_subcode;
  aps_frame.aspmsg.status             = status;
  memcpy(aps_frame.aspmsg.nodeid,     srcMacAddr, L7_ENET_MAC_ADDR_LEN);
  memset(aps_frame.aspmsg.reseved2,   0, 24);
  aps_frame.aspmsg.endTlv             = 0x00;

  ptin_oam_packet_send(L7_ALL_INTERFACES,
                       tbl_halErps[erps_idx].controlVidInternal,
                       (L7_uchar8 *) &aps_frame,
                       sizeof(aps_frame_t));

  return;
}


/**
 * Forward an APS packet on a specified interface and vlan
 * 
 * @author joaom (6/20/2013)
 * 
 * @param pktMsg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aps_packet_forward(L7_uint8 erps_idx, ptin_APS_PDU_Msg_t *pktMsg)
{
  L7_uint32   txintport;
  aps_frame_t *aps_frame;

  aps_frame = (aps_frame_t*) pktMsg->payload;

  // packet arrives with internal VLAN ID. write back the control VID
  aps_frame->vlan_tag[2] = 0xE0 | ((tbl_erps[erps_idx].protParam.controlVid>>8) & 0xF);
  aps_frame->vlan_tag[3] = tbl_erps[erps_idx].protParam.controlVid & 0x0FF;

  if ( memcmp(aps_frame->aspmsg.nodeid, srcMacAddr, L7_MAC_ADDR_LEN) == 0 ) {
    // Own packet! Do not forward it.

    if (ptin_oam_packet_debug_enable)
      LOG_TRACE(LOG_CTX_ERPS,"Own APS Packet! Do not forward it.");

    return L7_SUCCESS;
  }

  if ( pktMsg->intIfNum == tbl_halErps[erps_idx].port0intfNum ) {
    txintport = tbl_halErps[erps_idx].port1intfNum;
  } else {
    txintport = tbl_halErps[erps_idx].port0intfNum;
  }

  if (ptin_oam_packet_debug_enable)
    LOG_TRACE(LOG_CTX_ERPS,"Forwarding APS Packet transmited to intIfNum=%u", txintport);

  ptin_oam_packet_send(txintport,
                       tbl_halErps[erps_idx].controlVidInternal,
                       (L7_uchar8 *) aps_frame,
                       sizeof(aps_frame_t));

  return L7_SUCCESS;
}
#endif  // PTIN_ENABLE_ERPS


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
const L7_uchar8 apsMacAddr[L7_MAC_ADDR_LEN] = {0x01,0x19,0xA7,0x00,0x00,0x01};  // Last Byte is the Ring ID

/* CCM MAC address */
const L7_uchar8 ccmMacAddr[L7_MAC_ADDR_LEN] = {0x01,0x80,0xC2,0x00,0x00,0x37};  // Last Nibble is the MEG Level -> Fixed = 7

#include <unistd.h>

/* Maximum number of messages APS in queue */
#define PTIN_APS_PACKET_MAX_MESSAGES  128

/* Maximum number of messages APS in queue */
#define PTIN_CCM_PACKET_MAX_MESSAGES  256

/* Message id used in APS queue */
#define PTIN_APS_PACKET_MESSAGE_ID  1

/* Message id used in CCM queue */
#define PTIN_CCM_PACKET_MESSAGE_ID  1


/* APS PDU Message format */
typedef struct ptin_APS_PDU_Msg_s {
  L7_uint32        msgId;         /* Of type snoopMgmtMessages_t          */
  L7_uint32        intIfNum;      /* Interface on which PDU was received  */
  L7_uint32        vlanId;        /* VLAN on which PDU was received       */
  L7_uint32        innerVlanId;   /* Inner VLAN if present                */
  L7_uchar8        *payload;      /* Pointer to the received PDU          */
  L7_uint32        payloadLen;    /* Length of received PDU               */
  L7_netBufHandle  bufHandle;     /* Buffer handle                        */
} ptin_APS_PDU_Msg_t;

#define PTIN_APS_PDU_MSG_SIZE   sizeof(ptin_APS_PDU_Msg_t)


/* CCM PDU Message format */
typedef struct ptin_CCM_PDU_Msg_s {
  L7_uint32        msgId;         /* Of type snoopMgmtMessages_t          */
  L7_uint32        intIfNum;      /* Interface on which PDU was received  */
  L7_uint32        vlanId;        /* VLAN on which PDU was received       */
  L7_uint32        innerVlanId;   /* Inner VLAN if present                */
  L7_uchar8        *payload;      /* Pointer to the received PDU          */
  L7_uint32        payloadLen;    /* Length of received PDU               */
  L7_netBufHandle  bufHandle;     /* Buffer handle                        */
} ptin_CCM_PDU_Msg_t;

#define PTIN_CCM_PDU_MSG_SIZE   sizeof(ptin_CCM_PDU_Msg_t)

/*************** 
 * PROTOTYPES
 ***************/

/* Queue id */
void *ptin_aps_packetRx_queue  = L7_NULLPTR;
void *ptin_ccm_packetRx_queue  = L7_NULLPTR;


/* Task id */
L7_uint32 ptin_aps_packetTx_TaskId = L7_ERROR;


/* Task to process Tx messages */
void ptin_aps_packetTx_task(void);


/* Callback to be called for APS packets. */
L7_RC_t ptin_aps_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/* Callback to be called for CCM packets. */
L7_RC_t ptin_ccm_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);


/*****************
 * DEBUG ROUTINES
 *****************/

L7_BOOL ptin_oam_packet_debug_enable = L7_TRUE; //L7_FALSE;

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
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aps_packet_vlan_trap(L7_uint16 vlanId, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid argument");
    return L7_FAILURE;
  }

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_APS;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    LOG_ERR(LOG_CTX_PTIN_API,"Error setting rule to %u",enable);
    return rc;
  }

  LOG_TRACE(LOG_CTX_PTIN_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}


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

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_CCM;

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
L7_RC_t ptin_aps_packet_init(void)
{
  sysnetNotifyEntry_t snEntry;

  /* Queue that will process timer events */
  ptin_aps_packetRx_queue = (void *) osapiMsgQueueCreate("PTin_APS_PacketRx_Queue",
                                                       PTIN_APS_PACKET_MAX_MESSAGES, PTIN_APS_PDU_MSG_SIZE);
  if (ptin_aps_packetRx_queue == L7_NULLPTR) {
    LOG_FATAL(LOG_CTX_ERPS,"APS packet queue creation error.");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_ERPS,"APS packet queue created.");

  /* Create task for packets management */
  ptin_aps_packetTx_TaskId = osapiTaskCreate("ptin_aps_packetTx_task", ptin_aps_packetTx_task, 0, 0,
                                           L7_DEFAULT_STACK_SIZE,
                                           L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                           L7_DEFAULT_TASK_SLICE);

  if (ptin_aps_packetTx_TaskId == L7_ERROR) {
    LOG_FATAL(LOG_CTX_ERPS, "Could not create task ptin_aps_packetTx_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_ERPS,"Task ptin_aps_packetTx_task created");

  if (osapiWaitForTaskInit (L7_PTIN_APS_PACKET_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_ERPS,"Unable to initialize ptin_aps_packetTx_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_ERPS,"Task ptin_aps_packetTx_task initialized");

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
    LOG_FATAL(LOG_CTX_ERPS,"CCM packet queue creation error.");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_ERPS,"CCM packet queue created.");

  /* Register CCM packets */
  strcpy(snEntry.funcName, "ptin_ccm_packetRx_callback");
  snEntry.notify_pdu_receive = ptin_ccm_packetRx_callback;
  snEntry.type = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, ccmMacAddr, L7_MAC_ADDR_LEN);
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS) {
    LOG_ERR(LOG_CTX_ERPS, "Cannot register ptin_ccm_packetRx_callback callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_ERPS, "ptin_ccm_packetRx_callback registered!");

  return L7_SUCCESS;
}


/**
 * DEInitialize ptin_aps_packet module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_aps_packet_deinit(void)
{
  sysnetNotifyEntry_t snEntry;

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

  /* Delete task */
  if ( ptin_aps_packetTx_TaskId != L7_ERROR ) {
    osapiTaskDelete(ptin_aps_packetTx_TaskId);
    ptin_aps_packetTx_TaskId = L7_ERROR;
  }

  /* Queue that will process timer events */
  if (ptin_aps_packetRx_queue != L7_NULLPTR) {
    osapiMsgQueueDelete(ptin_aps_packetRx_queue);
    ptin_aps_packetRx_queue = L7_NULLPTR;
  }

  LOG_INFO(LOG_CTX_ERPS, "PTin APS packet deinit OK");

  return L7_SUCCESS;
}


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
    LOG_ERR(LOG_CTX_ERPS, "Cannot unregister ptin_ccm_packetRx_callback callback!");
    return L7_FAILURE;
  }
  LOG_INFO(LOG_CTX_ERPS, "ptin_ccm_packetRx_callback unregistered!");

  /* Queue that will process timer events */
  if (ptin_ccm_packetRx_queue != L7_NULLPTR) {
    osapiMsgQueueDelete(ptin_ccm_packetRx_queue);
    ptin_ccm_packetRx_queue = L7_NULLPTR;
  }

  LOG_INFO(LOG_CTX_ERPS, "PTin CCM packet deinit OK");

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
L7_RC_t ptin_aps_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8 *payload;
  L7_uint32 payloadLen;
  ptin_APS_PDU_Msg_t msg;

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

  /* Packet should be APS type */
  if ( memcmp(&payload[0], apsMacAddr, L7_MAC_ADDR_LEN)!=0 ) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Not an APS Packet");
    return L7_FAILURE;
  }

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"intIfNum %u and vlan %u does not belong to any valid EVC/interface");
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

  rc = osapiMessageSend(ptin_aps_packetRx_queue, &msg, PTIN_APS_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  if (rc != L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Failed message sending to ptin_aps_packet queue");
    return L7_FAILURE;
  }

  /* Return failure to guarantee these packets are consumed by other entities */
  return L7_FAILURE;
}


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

  /* Packet should be CCM type */
  if ( memcmp(&payload[0], ccmMacAddr, L7_MAC_ADDR_LEN)!=0 ) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Not an APS Packet");
    return L7_FAILURE;
  }

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"intIfNum %u and vlan %u does not belong to any valid EVC/interface");
    return L7_FAILURE;
  }

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
      LOG_ERR(LOG_CTX_ERPS,"Failed message sending to ptin_ccm_packet queue");
    return L7_FAILURE;
  }

  LOG_WARNING(LOG_CTX_ERPS,"Decide what to do with CCM packet! Packets are in proper queue waiting for someone to process. NOT YET DONE!!!");

  /* Return failure to guarantee these packets are consumed by other entities */
  return L7_FAILURE;
}


/**
 * Task that makes the timer processing for the clients manageme
 */
void ptin_aps_packetTx_task(void)
{
//L7_uint32 status;
//ptin_APS_PDU_Msg_t msg;

  LOG_INFO(LOG_CTX_ERPS,"PTin APS packet process task started");

  if (osapiTaskInitDone(L7_PTIN_APS_PACKET_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_SSM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_INFO(LOG_CTX_ERPS,"PTin APS packet task ready to process events");

  /* Loop */
  while (1) {
    sleep(10);
    ptin_hal_erps_sendaps(1, 0, 0);

#if 0
    status = (L7_uint32) osapiMessageReceive(ptin_aps_packetRx_queue,
                                             (void*) &msg,
                                             PTIN_APS_PDU_MSG_SIZE,
                                             L7_WAIT_FOREVER);

    if (status == L7_SUCCESS) {
      if ( msg.msgId == PTIN_APS_PACKET_MESSAGE_ID ) {
        if (ptin_aps_packetRx_process(&msg)!=L7_SUCCESS) {
          if (ptin_oam_packet_debug_enable)
            LOG_ERR(LOG_CTX_ERPS,"Error processing message");
        }
      } else {
        if (ptin_oam_packet_debug_enable)
          LOG_ERR(LOG_CTX_ERPS,"Message id is unknown (%u)",msg.msgId);
      }
    } else {
      if (ptin_oam_packet_debug_enable)
        LOG_ERR(LOG_CTX_ERPS,"Failed packet reception from ptin_aps_packet queue (status = %d)",status);
    }
#endif
  }
}


/**
 * Process broadcast packet
 * 
 * @param pktMsg : Packet information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_aps_packetRx_process( void /*ptin_APS_PDU_Msg_t *pktMsg*/ )
{
  L7_uint32 status;
  ptin_APS_PDU_Msg_t msg;

  LOG_INFO(LOG_CTX_ERPS,"APS packet received");


  status = (L7_uint32) osapiMessageReceive(ptin_aps_packetRx_queue,
                                           (void*) &msg,
                                           PTIN_APS_PDU_MSG_SIZE,
                                           L7_NO_WAIT);

  if (status == L7_SUCCESS) {
    if ( msg.msgId == PTIN_APS_PACKET_MESSAGE_ID ) {
      LOG_INFO(LOG_CTX_ERPS,"APS packet received OK");
    } else {
      LOG_INFO(LOG_CTX_ERPS,"APS packet received NOK");
    }
  } else {
    if (ptin_oam_packet_debug_enable)
      LOG_ERR(LOG_CTX_ERPS,"Failed packet reception from ptin_aps_packet queue (status = %d)",status);
  }

  return L7_SUCCESS;
}



/**
* Send a APS packet on a specified interface and vlan
*
* @param    intIfNum   @b{(input)} Outgoing internal interface number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @return  void
*/
void ptin_aps_packet_send(L7_uint32 intfNum,
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


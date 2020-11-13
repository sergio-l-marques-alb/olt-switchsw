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
#include "ptin_debug.h"

/* R-APS MAC address */
#define PTIN_APS_MACADDR  {0x01,0x19,0xA7,0x00,0x00,0x00}   // Correct Ring ID (last Byte) will be set on each rule/trap creation.
L7_uchar8           apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID

/* CCM MAC address */
L7_uchar8 ccmMacAddr[L7_MAC_ADDR_LEN] = {0x01,0x80,0xC2,0x00,0x00,0x37};  // Last Nibble is the MEG Level -> Fixed = 7

#include <unistd.h>
#include <arpa/inet.h>
#include <dot3ad_api.h>

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

#include <ethsrv_oam.h>
L7_long32 nr_using_ETH_oam_lvl[N_OAM_TMR_VALUES];

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




/*
void time_rx_aps(unsigned char init0_sample1_printf2) {
extern ptin_debug_pktTimer_t debug_pktTimer;
static unsigned long m=-1, M=0;
static unsigned long c;

 switch (init0_sample1_printf2) {
 case 1:
     c=osapiTimeMillisecondsDiff(osapiTimeMillisecondsGet(), debug_pktTimer.time_start);
     if (m>c) m=c;
     if (M<c) M=c;
     break;
 default:
 case 2:
     printf("\tmin=%lu\tlast=%lu\tMax=%lu\n\r", m, c, M);
     break;
 }//switch
}
void time_rx_ccm(unsigned char init0_sample1_printf2) {
extern ptin_debug_pktTimer_t debug_pktTimer;
static unsigned long m=-1, M=0;
static unsigned long c;

 switch (init0_sample1_printf2) {
 case 1:
     c=osapiTimeMillisecondsDiff(osapiTimeMillisecondsGet(), debug_pktTimer.time_start);
     if (m>c) m=c;
     if (M<c) M=c;
     break;
 default:
 case 2:
     printf("\tmin=%lu\tlast=%lu\tMax=%lu\n\r", m, c, M);
     break;
 }//switch
}
*/



#define MEP_TRAP_MAX    N_MEPs
#define MIP_TRAP_MAX    MEP_TRAP_MAX
L7_RC_t ptin_MxP_packet_vlan_trap(L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL mep0_mip1, L7_BOOL enable) {
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  static L7_BOOL          first_time = L7_TRUE;
  struct vl {
      L7_ulong32    n_using;
      L7_ushort16   vid;
      L7_uchar8     lvl;
  } *vid_lvl;
  static struct vl mep_vid_lvl[MEP_TRAP_MAX], mip_vid_lvl[MIP_TRAP_MAX];
  L7_ulong32 index, mep_index, mep_index_free;


  // Initialization
  if (first_time) {
    PT_LOG_TRACE(LOG_CTX_HAPI, "First time processing... make some initializations");
    memset(mep_vid_lvl, 0, sizeof(mep_vid_lvl));
    memset(mip_vid_lvl, 0, sizeof(mip_vid_lvl));
    first_time   = L7_FALSE;
  }





  if (!(vlanId >= PTIN_VLAN_MIN && vlanId <= PTIN_VLAN_MAX)) return L7_FAILURE;
  if (oam_level>=8) return L7_FAILURE;

  vid_lvl=  mep0_mip1?  mip_vid_lvl:    mep_vid_lvl;

  for (index=0, mep_index=mep_index_free=-1; index<MEP_TRAP_MAX; index++) {
      if (vid_lvl[index].n_using) {
          if (vid_lvl[index].vid!=vlanId    ||  vid_lvl[index].lvl!=oam_level) continue;

          if (enable) {
              vid_lvl[index].n_using++;
              return L7_SUCCESS;
          }
          else
          if (vid_lvl[index].n_using>1) {
              vid_lvl[index].n_using--;
              return L7_SUCCESS;
          }

          mep_index=index;
      }
      else
      if (mep_index_free>=MEP_TRAP_MAX) mep_index_free=index;
  }//for

  //If still here, we whether are...
  if (mep_index>=MEP_TRAP_MAX) {
      if (!enable) return L7_FAILURE;   //...deleting an inexistent entry
  }


  if (enable) { //...adding a new entry
      if (mep_index_free>=MEP_TRAP_MAX) return L7_TABLE_IS_FULL;
      index=mep_index_free;
      vid_lvl[index].n_using=1;
      vid_lvl[index].vid=vlanId;
      vid_lvl[index].lvl=oam_level;
  }
  else {    // or deleting an entry with n==1
      index=mep_index;
      vid_lvl[index].n_using=0;
      vid_lvl[index].vid=0;
      vid_lvl[index].lvl=0;
  }
















  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.level       = oam_level;
  dapiCmd.cmdData.snoopConfig.packet_type = mep0_mip1?  PTIN_PACKET_MIP_TRAPPED:    PTIN_PACKET_MEP_TRAPPED;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}//ptin_MxP_packet_vlan_trap



/**********************
 * EXTERNAL ROUTINES
 **********************/

#ifdef PTIN_ENABLE_ERPS
#if defined(__MxP_FILTER__INSTEAD_OF__APS_AND_CCM__)
L7_RC_t ptin_aps_packet_vlan_trap(L7_uint16 vlanId, L7_uint8 ringId_oam_level, L7_BOOL enable) {
 return ptin_MxP_packet_vlan_trap(vlanId, ringId_oam_level, 0, enable);
}
#else
/**
 * Configure APS packet trapping in HW
 * 
 * @author joaom (6/13/2013)
 * 
 * @param vlanId
 * @param ringId 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aps_packet_vlan_trap(L7_uint16 vlanId, L7_uint8 ringId_oam_level, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.level       = ringId_oam_level;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_APS;  

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES, &dapiCmd);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}
#endif

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

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = L7_NULL;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.level       = 0;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_APS;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES, &dapiCmd);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_API,"Error setting global enable to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying global enable to %u",enable);

  return L7_SUCCESS;
}

#endif  // PTIN_ENABLE_ERPS


#if defined(__MxP_FILTER__INSTEAD_OF__APS_AND_CCM__)
L7_RC_t ptin_ccm_packet_vlan_trap(L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL enable) {
 return ptin_MxP_packet_vlan_trap(vlanId, oam_level, 0, enable);
}
#else
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
L7_RC_t ptin_ccm_packet_vlan_trap(L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL enable)
{
  DAPI_SYSTEM_CMD_t dapiCmd;
  L7_RC_t rc;

  /* Policer must be a valid pointer */
  if (vlanId<PTIN_VLAN_MIN || vlanId>PTIN_VLAN_MAX)
  {
    PT_LOG_ERR(LOG_CTX_API,"Invalid argument");
    return L7_FAILURE;
  }

  memset(&dapiCmd.cmdData.snoopConfig, 0x00, sizeof(dapiCmd.cmdData.snoopConfig));

  dapiCmd.cmdData.snoopConfig.getOrSet    = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;
  dapiCmd.cmdData.snoopConfig.family      = L7_AF_INET;
  dapiCmd.cmdData.snoopConfig.enable      = enable & 1;
  dapiCmd.cmdData.snoopConfig.vlanId      = vlanId;
  dapiCmd.cmdData.snoopConfig.CoS         = (L7_uint8) -1;
  dapiCmd.cmdData.snoopConfig.level       = oam_level;
  dapiCmd.cmdData.snoopConfig.packet_type = PTIN_PACKET_CCM;

  rc=dtlPtinPacketsTrap(L7_ALL_INTERFACES,&dapiCmd);
  if (rc!=L7_SUCCESS)  {
    PT_LOG_ERR(LOG_CTX_API,"Error setting rule to %u",enable);
    return rc;
  }

  PT_LOG_TRACE(LOG_CTX_API,"Success applying rule to %u",enable);

  return L7_SUCCESS;
}
#endif


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
  L7_uint8            queue_str[24];
  L7_uchar8           apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID
  L7_uint8            i;

  sprintf(queue_str, "PTin_APS_PacketRx_Queue%d", erps_idx);

  /* Queue that will process packets */
  ptin_aps_packetRx_queue[erps_idx] = (void *) osapiMsgQueueCreate(queue_str,
                                                       PTIN_APS_PACKET_MAX_MESSAGES, PTIN_APS_PDU_MSG_SIZE);
  if (ptin_aps_packetRx_queue[erps_idx] == L7_NULLPTR) {
    PT_LOG_FATAL(LOG_CTX_ERPS,"APS packet queue %d creation error.", erps_idx);
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_ERPS,"APS packet queue %d created.", erps_idx);


  /* Ring ID needs to be set on rule/trap creation. */
  apsMacAddr[5] = tbl_erps[erps_idx].protParam.ringId;

  /* Rx_callback is always the same; Do not try to register a snEntry if the MAC is already registered (same Ring ID) */
  for (i=0; i<MAX_PROT_PROT_ERPS; i++) {
    if (i == erps_idx) continue;
    if ( (tbl_erps[i].admin == PROT_ERPS_ENTRY_BUSY) && (tbl_erps[i].protParam.ringId == tbl_erps[erps_idx].protParam.ringId) ) return L7_SUCCESS;
  }

  //return common_aps_ccm_packetRx_callback_register(); must register callback elsewhere, being sure both OAM ETH and ERP are up

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
L7_RC_t ptin_ccm_packet_init(L7_long32 oam_level)
{
  L7_long32 i;

  if (oam_level>=N_OAM_TMR_VALUES) {
      /* Queue that will process timer events */
      if (L7_NULLPTR==ptin_ccm_packetRx_queue) {
        ptin_ccm_packetRx_queue = (void *) osapiMsgQueueCreate("PTin_CCM_PacketRx_Queue",
                                                               PTIN_CCM_PACKET_MAX_MESSAGES, PTIN_CCM_PDU_MSG_SIZE);
        if (L7_NULLPTR==ptin_ccm_packetRx_queue) {
          PT_LOG_FATAL(LOG_CTX_OAM,"CCM packet queue creation error.");
          return L7_FAILURE;
        }
        PT_LOG_INFO(LOG_CTX_OAM,"CCM packet queue created.");
    
        for (i=0; i<N_OAM_TMR_VALUES; i++) nr_using_ETH_oam_lvl[i]=0;
      }

      return L7_SUCCESS;       //Just creation of the message queue
  }

  if (nr_using_ETH_oam_lvl[oam_level]++) return L7_SUCCESS; //No need to register if already done. (Only need that on "nr_us..."'s transition 0->1.)

  //return common_aps_ccm_packetRx_callback_register(); must register callback elsewhere, being sure both OAM ETH and ERP are up

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

  PT_LOG_INFO(LOG_CTX_ERPS, "PTin APS packet deinit OK");

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
L7_RC_t ptin_ccm_packet_deinit(L7_long32 oam_level)
{
  L7_long32 a,b,i;

  if (oam_level<N_OAM_TMR_VALUES) {
    if (0==nr_using_ETH_oam_lvl[oam_level]) return L7_FAILURE;
    if (1==nr_using_ETH_oam_lvl[oam_level]--) a=b=oam_level;
    else {
      //a=N_OAM_TMR_VALUES;
      //b=0;
      return L7_SUCCESS;
    }
  }
  else {//oam_level>=N_OAM_TMR_VALUES   reset, deregister and delete message queue
    a=0;
    b=N_OAM_TMR_VALUES-1;
    for (i=0; i<N_OAM_TMR_VALUES; i++) nr_using_ETH_oam_lvl[i]=0;
  }

  if (oam_level<N_OAM_TMR_VALUES) return L7_SUCCESS;

  /* Queue that will process timer events */
  if (ptin_ccm_packetRx_queue != L7_NULLPTR) {
    osapiMsgQueueDelete(ptin_ccm_packetRx_queue);
    ptin_ccm_packetRx_queue = L7_NULLPTR;
  }

  PT_LOG_INFO(LOG_CTX_OAM, "PTin CCM packet deinit OK");

  return L7_SUCCESS;
}


#ifdef PTIN_ENABLE_ERPS

/**
 * Check Node ID 
 * Refresh Rx counters 
 * 
 * @author joaom (7/10/2013)
 * 
 * @param erps_idx 
 * @param pktMsg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aps_checkOwnNodeId(L7_uint8 erps_idx, ptin_APS_PDU_Msg_t *pktMsg)
{
  L7_uint32   rxport;
  aps_frame_t *aps_frame;

  aps_frame = (aps_frame_t*) pktMsg->payload;

  if (ptin_oam_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_ERPS,"erps_idx %d, intIfNum %d, P0intIfNum %d, P1intIfNum %d",
                 erps_idx, pktMsg->intIfNum, tbl_halErps[erps_idx].port0intfNum, tbl_halErps[erps_idx].port1intfNum);

  // ERPS Rx Ring port
  if ( pktMsg->intIfNum == tbl_halErps[erps_idx].port0intfNum ) {
    rxport = 0;
  } else if ( pktMsg->intIfNum == tbl_halErps[erps_idx].port1intfNum ) {
    rxport = 1;
  } else {
    return L7_FAILURE;
  }

  // Check Node ID
  if ( memcmp(aps_frame->aspmsg.nodeid, srcMacAddr, L7_MAC_ADDR_LEN) == 0 ) {   
    if (ptin_oam_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_ERPS,"Own APS Packet! Dropped.");

    // Own packet! Drop it.
    ptin_hal_erps_counters_rxdrop(erps_idx, rxport);

    return L7_FAILURE;
  }
 
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
  L7_uchar8           *payload;
  L7_uint32           payloadLen;
  ptin_APS_PDU_Msg_t  msg;
  L7_uchar8           apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID

  L7_uint32 ptin_port;
  L7_uint32 intIfNum    = pduInfo->intIfNum;     /* Source port */
  L7_uint16 vlanId      = pduInfo->vlanId;       /* Vlan */
  L7_uint16 innerVlanId = pduInfo->innerVlanId;  /* Inner vlan */

  L7_RC_t rc = L7_SUCCESS;


  if (debug_APS_CCM_pktTimer) {
      proc_runtime_stop(PTIN_PROC_RAPS_RX_INSTANCE);
      proc_runtime_start(PTIN_PROC_RAPS_RX_INSTANCE2);
  }
  if (ptin_oam_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_ERPS,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX ) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_ERPS,"Invalid vlanId %u",vlanId);
    return L7_FAILURE;
  }

  /* Packet should be APS MAC Addr (last byte is ignored) */
  if ( memcmp(&payload[0], apsMacAddr, (L7_MAC_ADDR_LEN-1))!=0 ) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_ERPS,"Not an APS Packet");
    return L7_FAILURE;
  }

  /* Get ptin_port */
  if (ptin_intf_intIfNum2port(intIfNum, 0/*Vlan*/, &ptin_port) != L7_SUCCESS) /* FIXME TC16SXG */
  {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_ERPS,"Error converting intIfNum %u to ptin_port", intIfNum);
    return L7_FAILURE;
  }
  
  /* Validate interface and VLAN, as belonging to a valid interface in a valid EVC */
  if (ptin_evc_intfVlan_validate(ptin_port, vlanId) != L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_ERPS,"ptin_port %u and vlan %u does not belong to any valid EVC/interface",
                 ptin_port, vlanId);
    return L7_FAILURE;
  }

  /* Proceed if the APS VLAN ID is associated to an ERPS instance */
  if (erpsIdx_from_controlVidInternal[vlanId] != PROT_ERPS_UNUSEDIDX) {

    memset(&msg, 0x00, sizeof(msg));
    msg.msgId       = PTIN_APS_PACKET_MESSAGE_ID;
    msg.intIfNum    = pduInfo->intIfNum;
    msg.vlanId      = pduInfo->vlanId;
    msg.innerVlanId = pduInfo->innerVlanId;
    msg.payload     = payload;
    msg.payloadLen  = payloadLen;
    msg.bufHandle   = bufHandle;

    /* Validate Node ID before processing and forwarding packet*/
    if (ptin_aps_checkOwnNodeId(erpsIdx_from_controlVidInternal[vlanId], &msg) != L7_SUCCESS) {      
      return L7_FAILURE;
    }      

    ptin_aps_packet_forward(erpsIdx_from_controlVidInternal[vlanId], &msg);

    if (debug_APS_CCM_pktTimer) {
        proc_runtime_stop(PTIN_PROC_RAPS_RX_INSTANCE2);
        proc_runtime_start(PTIN_PROC_RAPS_RX_INSTANCE3);
    }

    /* Send packet to queue */
    rc = osapiMessageSend(ptin_aps_packetRx_queue[erpsIdx_from_controlVidInternal[vlanId]], &msg, PTIN_APS_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  }

  if (ptin_oam_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_ERPS,"Packet sent to queue %d",erpsIdx_from_controlVidInternal[vlanId]);

  if (rc != L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_ERPS,"Failed message sending to ptin_aps_packet queue");
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

  if (debug_APS_CCM_pktTimer) {
      proc_runtime_stop(PTIN_PROC_CCM_RX_INSTANCE);
      proc_runtime_start(PTIN_PROC_CCM_RX_INSTANCE2);
  }

  if (ptin_oam_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_OAM,"Packet received at intIfNum=%u with vlanId=%u and innerVlanId=%u",
              intIfNum, vlanId, innerVlanId);

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

  /* Validate vlan id */
  /*if ( vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX ) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_OAM,"Invalid vlanId %u",vlanId);
    return L7_FAILURE;
  }*/

  /* Packet should be CCM type */
  //if ( memcmp(&payload[0], ccmMacAddr, L7_MAC_ADDR_LEN)!=0 ) {
  //  if (ptin_oam_packet_debug_enable)
  //    PT_LOG_ERR(LOG_CTX_OAM,"Not a CCM Packet");
  //  return L7_FAILURE;
  //}

  /* Validate interface and vlan, as belonging to a valid interface in a valid EVC */
  /*if (ptin_evc_intfVlan_validate(intIfNum, vlanId)!=L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_OAM,"intIfNum %u and vlan %u does not belong to any valid EVC/interface");
    return L7_FAILURE;
  }*/

  dot3adWhoisOwnerLag(intIfNum, &intIfNum);
  //From dot1sPortIndexFromIntfNumGet
  //do {
  //  //L7_uint32 portIndex = L7_NULL;
  //  L7_uint32 minLag, maxLag;
  //
  //  if (nimIntIfNumRangeGet(L7_LAG_INTF,&minLag, &maxLag) != L7_SUCCESS) break;
  //
  //  if (intIfNum <= L7_MAX_PORT_COUNT) break; //portIndex = intIfNum;
  //  else
  //  if ((intIfNum >= minLag) && (intIfNum <= maxLag)) intIfNum = L7_MAX_PORT_COUNT + (intIfNum - minLag + 1);
  //                                                 //portIndex = L7_MAX_PORT_COUNT + (intIfNum - minLag + 1);
  //}while (0);

  /* Send packet to queue */
  memset(&msg, 0x00, sizeof(msg));
  msg.msgId       = PTIN_CCM_PACKET_MESSAGE_ID;
  msg.intIfNum    = intIfNum;
  msg.vlanId      = vlanId;
  msg.innerVlanId = innerVlanId;
  msg.payload     = payload;
  msg.payloadLen  = payloadLen;
  msg.bufHandle   = bufHandle;
  msg.timestamp   = pduInfo->timestamp;

  rc = osapiMessageSend(ptin_ccm_packetRx_queue, &msg, PTIN_CCM_PDU_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_HI);

  if (rc != L7_SUCCESS) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_OAM,"Failed message sending to ptin_ccm_packet queue");
    return L7_FAILURE;
  }

  //PT_LOG_WARN(LOG_CTX_OAM,"Decide what to do with CCM packet! Packets are in proper queue waiting for someone to process. NOT YET DONE!!!");

  /* Return failure to guarantee these packets are consumed by other entities */
  return L7_FAILURE;
}





L7_RC_t common_aps_ccm_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo) {
    L7_uchar8 *payload;
    //L7_uint32 payloadLen;

    if (ptin_oam_packet_debug_enable)   PT_LOG_TRACE(LOG_CTX_OAM,"APS or CCM packet received");

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, payload);
    //SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, payloadLen);

#ifdef PTIN_ENABLE_ERPS
    if (0==memcmp(&apsMacAddr[1], &payload[1], 4))  return ptin_aps_packetRx_callback(bufHandle, pduInfo);
    else                                            return ptin_ccm_packetRx_callback(bufHandle, pduInfo);
    //could instead check the OAM ETH opcode (calling aps for APS opcodes, ccm for every other), but this option is closer to the pdu beginning
#else
    return ptin_ccm_packetRx_callback(bufHandle, pduInfo);
#endif
}



L7_RC_t common_aps_ccm_packetRx_callback_register(void) {
  sysnetNotifyEntry_t snEntry;
  static int _1st_time=1;

  if (!_1st_time) return L7_SUCCESS;

  _1st_time=0;

  PT_LOG_INFO(LOG_CTX_STARTUP,"Going to register common_aps_ccm_packetRx_callback related to type=%u, protocol_type=%u: %p",
              SYSNET_ETHERTYPE_ENTRY, L7_ETYPE_CFM, common_aps_ccm_packetRx_callback);

  memset(&snEntry, 0x00, sizeof(snEntry));
  strcpy(snEntry.funcName, "common_aps_ccm_packetRx_callback");
  snEntry.notify_pdu_receive = common_aps_ccm_packetRx_callback;
  snEntry.type = SYSNET_ETHERTYPE_ENTRY;
  snEntry.u.protocol_type = L7_ETYPE_CFM;
  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_OAM, "Cannot register common_aps_ccm_packetRx_callback !");
    return L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_OAM, "common_aps_ccm_packetRx_callback registered!");
  return L7_SUCCESS;
}


//L7_RC_t common_aps_ccm_packetRx_callback_deregister(void) {return L7_SUCCESS;}


/**
 * Process ASP packet
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
#ifdef PTIN_ENABLE_ERPS
L7_RC_t ptin_aps_packetRx_process(L7_uint32 queueidx, L7_uint8 *aps_req, L7_uint8 *aps_status, L7_uint8 *aps_nodeid, L7_uint32 *aps_rxport)
{
  L7_uint32           status;
  ptin_APS_PDU_Msg_t  msg;
  int i;
  aps_frame_t         *aps_frame;

  //PT_LOG_TRACE(LOG_CTX_ERPS,"Any APS packet received?");
  if (queueidx > MAX_PROT_PROT_ERPS) {
    PT_LOG_ERR(LOG_CTX_OAM, "queueidx (%d) out of range", queueidx);
    return L7_FAILURE;
  }

  if (ptin_aps_packetRx_queue[queueidx] == L7_NULLPTR) {
    PT_LOG_ERR(LOG_CTX_OAM, "queueidx (%d) is a NULL Pointer", queueidx);
    return L7_FAILURE;
  }

  status = (L7_uint32) osapiMessageReceive(ptin_aps_packetRx_queue[queueidx],
                                           (void*) &msg,
                                           PTIN_APS_PDU_MSG_SIZE,
                                           L7_NO_WAIT);

  if (status == L7_SUCCESS) {
    if ( msg.msgId == PTIN_APS_PACKET_MESSAGE_ID ) {
      if ( ptin_oam_packet_debug_enable ) {
        PT_LOG_TRACE(LOG_CTX_ERPS,"APS packet received: intIfNum %d, vlanId %d, innerVlanId %d, payloadLen %d", msg.intIfNum, msg.vlanId, msg.innerVlanId, msg.payloadLen);

        if ( msg.payloadLen < 128 ) {
          L7_uint8 buf[512], buf2[4];

          for (i=0, buf[0]=0; i<msg.payloadLen; i++) {
            sprintf(buf2, "%.2x ", msg.payload[i]);     buf2[3]=0;
            strcat(buf, buf2);
          }
          buf[511]=0;
          PT_LOG_TRACE(LOG_CTX_ERPS,"Payload: %s", buf);
        }
      }

      aps_frame = (aps_frame_t*) msg.payload;

      *aps_req            = (aps_frame->aspmsg.req_subcode >> 4) & 0x0F; /* Ignoring subcode */
      *aps_status         = aps_frame->aspmsg.status;
      memcpy(aps_nodeid,  aps_frame->aspmsg.nodeid, L7_ENET_MAC_ADDR_LEN);
      *aps_rxport         = msg.intIfNum;

      //time_rx_aps(1);
      if (debug_APS_CCM_pktTimer) proc_runtime_stop(PTIN_PROC_RAPS_RX_INSTANCE3);

      return L7_SUCCESS;

    } else {
      if (ptin_oam_packet_debug_enable)
        PT_LOG_TRACE(LOG_CTX_ERPS,"APS packet received with Unknown ID");
      return L7_FAILURE;
    }
  }

  return L7_FAILURE;
}
#endif  // PTIN_ENABLE_ERPS


/**
* Send a packet on a specified interface and VLAN
*
* @param    ptin_port  @b{(input)} Outgoing internal interface
*                      number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @return  void
*/
void ptin_oam_packet_send(L7_uint32 ptin_port,
                          L7_uint32 vlanId,
                          L7_uchar8 *payload,
                          L7_uint32 payloadLen)
{
  L7_uint32         intIfNum;
  L7_netBufHandle   bufHandle;
  L7_uchar8        *dataStart;
  L7_INTF_TYPES_t   sysIntfType;
  DTL_CMD_TX_INFO_t dtlCmd;

  /* Convert to intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_ERR(LOG_CTX_API, "Error converting ptin_port %u to intIfNum", ptin_port);
    return;
  }

  /* If outgoing interface is CPU interface, don't send it */
  if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
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

  dtlCmd.intfNum             = intIfNum;
  dtlCmd.priority            = 1;
  dtlCmd.typeToSend          = (intIfNum == L7_ALL_INTERFACES)? DTL_VLAN_MULTICAST : DTL_NORMAL_UNICAST;
  dtlCmd.cmdType.L2.domainId = vlanId;
  dtlCmd.cmdType.L2.vlanId   = vlanId;
  dtlCmd.cmdType.L2.flags    = 0;

  dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);

  if (ptin_oam_packet_debug_enable) {
    PT_LOG_TRACE(LOG_CTX_ERPS,"OAM Packet transmited to ptin_port=%u, vlanId=%u", ptin_port, vlanId);
  }

  return;
}


/**
 * Send an APS packet on a specified interface and VLAN
 * 
 * @author joaom (6/17/2013)
 * 
 * @param erps_idx 
 * @param reqstate_subcode 
 * @param status 
 */
#ifdef PTIN_ENABLE_ERPS
void ptin_aps_packet_send(L7_uint8 erps_idx, L7_uint16 vid, L7_uint8 megLevel, L7_uint8 req_subcode, L7_uint8 status)
{
  aps_frame_t aps_frame;
  L7_uchar8   apsMacAddr[L7_MAC_ADDR_LEN] = PTIN_APS_MACADDR;   // Last Byte is the Ring ID

  // Last Byte is the Ring ID
  apsMacAddr[5] = tbl_erps[erps_idx].protParam.ringId;

  memcpy(aps_frame.dmac,              apsMacAddr, L7_ENET_MAC_ADDR_LEN);

  aps_frame.vlan_tag[0]               = 0x81;
  aps_frame.vlan_tag[1]               = 0x00;
  aps_frame.vlan_tag[2]               = 0xE0 | ((vid>>8) & 0xF);
  aps_frame.vlan_tag[3]               = vid & 0x0FF;
  aps_frame.etherType                 = htons(L7_ETYPE_CFM);

  aps_frame.aspmsg.mel_version        = 1 | megLevel<<5;//0x21;  // MEG Level 1; Version 1
  aps_frame.aspmsg.opCode             = 40;
  aps_frame.aspmsg.flags              = 0x00;
  aps_frame.aspmsg.tlvOffset          = 0x20;
  aps_frame.aspmsg.req_subcode        = req_subcode;
  aps_frame.aspmsg.status             = status;
  memcpy(aps_frame.aspmsg.nodeid,     srcMacAddr, L7_ENET_MAC_ADDR_LEN);
  memset(aps_frame.aspmsg.reseved2,   0, 24);
  aps_frame.aspmsg.endTlv             = 0x00;

  nimGetIntfAddress(tbl_halErps[erps_idx].port0intfNum, L7_SYSMAC_BIA, aps_frame.smac);
  ptin_oam_packet_send(tbl_halErps[erps_idx].port0intfNum,
                       tbl_halErps[erps_idx].controlVidInternal,
                       (L7_uchar8 *) &aps_frame,
                       sizeof(aps_frame_t));

  nimGetIntfAddress(tbl_halErps[erps_idx].port1intfNum, L7_SYSMAC_BIA, aps_frame.smac);
  ptin_oam_packet_send(tbl_halErps[erps_idx].port1intfNum,
                       tbl_halErps[erps_idx].controlVidInternal,
                       (L7_uchar8 *) &aps_frame,
                       sizeof(aps_frame_t));

  return;
}


/**
 * Forward an APS packet on a specified interface and VLAN
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
  L7_uint8    txport, req=0;
  aps_frame_t *aps_frame;

  aps_frame = (aps_frame_t*) pktMsg->payload;

  // Check if this is an open ring configuration.
  // On closed ring, packet should not be forwarded to a blocked port
  // On closed ring, packet received at a blocked ring port should not be forwarded to the other ring port;
  if ( (tbl_erps[erps_idx].protParam.isOpenRing == L7_FALSE) && 
       ( (tbl_erps[erps_idx].portState[PROT_ERPS_PORT0] == ERPS_PORT_BLOCKING) || (tbl_erps[erps_idx].portState[PROT_ERPS_PORT1] == ERPS_PORT_BLOCKING) ) ) {

    // Port blocked. Do not forward packet
    if (ptin_oam_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_ERPS,"Port blocked! Do not forward APS packet.");

    return L7_SUCCESS;
  }

  // If local State machine has a TxReq then FW is not done
  req = (tbl_halErps[erps_idx].apsReqStatusTx >> 12) & 0xF;
  if (req != RReq_NONE) {
    if (ptin_oam_packet_debug_enable)
      PT_LOG_TRACE(LOG_CTX_ERPS,"Local Req! Do not forward APS packet.");

    return L7_SUCCESS;
  }

  // Tx packet to the other Ring port
  if ( pktMsg->intIfNum == tbl_halErps[erps_idx].port0intfNum ) {
    txintport = tbl_halErps[erps_idx].port1intfNum;
    txport    = 1;
  } else {
    txintport = tbl_halErps[erps_idx].port0intfNum;
    txport    = 0;
  }

  // Refresh Tx counter
  ptin_hal_erps_counters_fw(erps_idx, txport, req);

  // Packet arrives with internal VLAN ID. write back the control VID.
  aps_frame->vlan_tag[2] = 0xE0 | ((tbl_erps[erps_idx].protParam.controlVid>>8) & 0xF);
  aps_frame->vlan_tag[3] = tbl_erps[erps_idx].protParam.controlVid & 0x0FF;

  if (ptin_oam_packet_debug_enable)
    PT_LOG_TRACE(LOG_CTX_ERPS,"Forwarding APS Packet transmited to intIfNum=%u", txintport);

  ptin_oam_packet_send(txintport,
                       tbl_halErps[erps_idx].controlVidInternal,
                       (L7_uchar8 *) aps_frame,
                       sizeof(aps_frame_t));

  return L7_SUCCESS;
}













#include <ptin_xlate_api.h>
static unsigned char ERP_bmp[MAX_PROT_PROT_ERPS];
static unsigned char _1st_time_notification=1;

int ptin_erps_FSM_transition_notification(unsigned char erps_idx, unsigned char old_state, unsigned char new_state) {
 if (_1st_time_notification) {
     _1st_time_notification=0;
     memset(ERP_bmp, 0, sizeof(ERP_bmp));
 }

 if (erps_idx>=MAX_PROT_PROT_ERPS) return 1;

 ERP_bmp[erps_idx]=1;

 PT_LOG_TRACE(LOG_CTX_ERPS, "erps_idx=%u", erps_idx);
 return 0;
}//ptin_erps_FSM_transition_notification




int ptin_erps_igmp_notify(unsigned short MC_vid) {
int erps_idx;
//unsigned short MC_vid;

 for (erps_idx=0; erps_idx<MAX_PROT_PROT_ERPS; erps_idx++) {
    if (!ERP_bmp[erps_idx]) continue;

    ERP_bmp[erps_idx]=0;

    //if (L7_SUCCESS==ptin_xlate_egress_get(tbl_halErps[erps_idx].port0intfNum, internal_MC_vid, PTIN_XLATE_NOT_DEFINED, &MC_vid, L7_NULLPTR)) {
        if (tbl_erps[erps_idx].protParam.vid_bmp[MC_vid/8] & 1<<(MC_vid%8)) {
            PT_LOG_TRACE(LOG_CTX_ERPS, "VLAn %d in erp %u", MC_vid, erps_idx);//MC VID in the protected list
            return 0;
      //  }
    }

   // if (L7_SUCCESS==ptin_xlate_egress_get(tbl_halErps[erps_idx].port1intfNum, internal_MC_vid, PTIN_XLATE_NOT_DEFINED, &MC_vid, L7_NULLPTR)) {
        if (tbl_erps[erps_idx].protParam.vid_bmp[MC_vid/8] & 1<<(MC_vid%8)) {
            //MC VID in the protected list
          PT_LOG_TRACE(LOG_CTX_ERPS, "VLAn %d in erp %u", MC_vid, erps_idx);
            return 0;
        }
   // }
 }

 return 1;
}
#endif  // PTIN_ENABLE_ERPS


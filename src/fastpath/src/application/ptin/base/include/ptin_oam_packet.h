/**
 * ptin_oam_packet.h
 *  
 * Implements OAM Packet (R-APS and CCM) capture and Process 
 * routines 
 *
 * @author joaom (6/11/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */


#ifndef _PTIN_OAM_PACKET_H
#define _PTIN_OAM_PACKET_H

#include "ptin_include.h"
#include "sysnet_api.h"


typedef struct aps_pdu_s {

  L7_uchar8         mel_version;
  L7_uchar8         opCode;
  L7_uchar8         flags;
  L7_uchar8         tlvOffset;
  L7_uchar8         req_subcode;
  L7_uchar8         status;
  L7_uchar8         nodeid[6];
  L7_uchar8         reseved2[24];
  L7_uchar8         endTlv;

} __attribute__((packed)) aps_pdu_t;

/**
* @purpose  structure which will be used as an overlay on the
*           received APS messages
*/
typedef struct aps_frame_s {
  L7_uchar8         dmac[L7_ENET_MAC_ADDR_LEN]; /* 6 bytes */
  L7_uchar8         smac[L7_ENET_MAC_ADDR_LEN]; /* 6 bytes */

  L7_uchar8         vlan_tag[4];             /* 4 bytes: | TPID (2Bytes) | PCP(3bits)/CFI(1bit)/VID(12bits) (2Bytes) | */

  L7_uint16         etherType;              /* 2 bytes */

  aps_pdu_t         aspmsg;

#if 0
  L7_uchar8         fcs[4];                 /* 4 bytes */
#endif

} __attribute__((packed)) aps_frame_t;




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
  L7_ulong32       timestamp      /* timestamp                            */
} ptin_CCM_PDU_Msg_t;

#define PTIN_CCM_PDU_MSG_SIZE   sizeof(ptin_CCM_PDU_Msg_t)




extern void *ptin_ccm_packetRx_queue;
/******************************************************************
 *                      EXTERNAL ROUTINES
 ******************************************************************/


/**
 * Configure APS packet trapping in HW
 * 
 * @author joaom (6/13/2013)
 * 
 * @param vlanId 
 * @param enable 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_aps_packet_vlan_trap(L7_uint16 vlanId, L7_uint8 ringId_oam_level, L7_BOOL enable);

/**
 * Set global enable for APS packets to go to the CPU
 * 
 * @param enable : L7_TRUE/L7_FALSE
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_aps_packet_global_trap(L7_BOOL enable);

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
extern L7_RC_t ptin_ccm_packet_vlan_trap(L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL enable);

/**
 * Initialize ptin_aps_packet module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_aps_packet_init(L7_uint8 erps_idx);

/**
 * Initialize CCM module
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_ccm_packet_init(L7_long32 oam_level);

/**
 * DEInitialize ptin_aps_packet module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_aps_packet_deinit(L7_uint8 erps_idx);

/**
 * DEInitialize ptin_ccm_packet module
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_ccm_packet_deinit(L7_long32 oam_level);

/** Send a packet on a specified interface and VLAN
*
* @param    intIfNum   @b{(input)} Outgoing internal interface number
* @param    vlanId     @b{(input)} VLAN ID
* @param    payload    @b{(input)} Message to be forwarded
* @param    payloadLen @b{(input)} Length of message
*
* @return  void
*/
extern void ptin_oam_packet_send(L7_uint32 intfNum, L7_uint32 vlanId, L7_uchar8 *payload, L7_uint32 payloadLen);

/**
 * Send a APS packet on a specified interface and VLAN
 * 
 * @author joaom (6/17/2013)
 * 
 * @param erps_idx 
 * @param vid 
 * @param megLevel 
 * @param req_state 
 * @param status 
 */
extern void ptin_aps_packet_send(L7_uint8 erps_idx, L7_uint16 vid, L7_uint8 megLevel, L7_uint8 req_subcode, L7_uint8 status);

/**
 * Process received APS packet on specified interface and VLAN
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_aps_packetRx_process(L7_uint32 queueidx, L7_uint8 *aps_req, L7_uint8 *aps_status, L7_uint8 *aps_nodeid, L7_uint32 *aps_rxport);




#ifdef COMMON_APS_CCM_CALLBACKS__ETYPE_REG
L7_RC_t common_aps_ccm_packetRx_callback(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);
L7_RC_t common_aps_ccm_packetRx_callback_register(void);
//L7_RC_t common_aps_ccm_packetRx_callback_deregister(void);  //dummy; does nothing
#endif  //COMMON_APS_CCM_CALLBACKS__ETYPE_REG

#endif  /* _PTIN_OAM_PACKET_H */


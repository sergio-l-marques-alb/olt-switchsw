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

/* R-APS MAC address */
extern const L7_uchar8 apsMacAddr[L7_MAC_ADDR_LEN];  // Last Byte is the Node ID

typedef struct aps_pdu_s {

  L7_uchar8         mel_version;
  L7_uchar8         opCode;
  L7_uchar8         flags;
  L7_uchar8         tlvOffset;
  L7_uchar8         req_state_subcode;
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



/******************************************************************
 *                      EXTERNAL ROUTINES
 ******************************************************************/


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
extern L7_RC_t ptin_aps_packet_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

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
extern L7_RC_t ptin_ccm_packet_vlan_trap(L7_uint16 vlanId, L7_BOOL enable);

/**
 * Initialize ptin_aps_packet module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_aps_packet_init(void);

/**
 * Initialize CCM module
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_ccm_packet_init(void);

/**
 * DEInitialize ptin_aps_packet module
 * 
 * @author joaom (6/11/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_aps_packet_deinit(void);

/**
 * DEInitialize ptin_ccm_packet module
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t :  L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_ccm_packet_deinit(void);

/**
 * Send a APS packet on a specified interface and vlan
 * 
 * @author joaom (6/17/2013)
 * 
 * @param erps_idx 
 * @param req_state 
 * @param status 
 */
extern void ptin_aps_packet_send(L7_uint32 erps_idx, L7_uint8 reqstate_subcode, L7_uint8 status);

/**
 * Process received APS packet on specified interface and vlan
 * 
 * @author joaom (6/15/2013)
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_aps_packetRx_process(L7_uint32 queueidx, L7_uint8 *aps_reqstate, L7_uint8 *aps_status, L7_uint8 *aps_nodeid, L7_uint32 *aps_rxport);


#endif  /* _PTIN_OAM_PACKET_H */


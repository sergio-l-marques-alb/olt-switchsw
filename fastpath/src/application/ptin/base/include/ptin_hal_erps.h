/**
 * ptin_hal_erps.h
 *  
 * Implements ITU-T G.8032/Y.1344 Ethernet Ring Protection 
 * Switching HAL
 *
 * @author joaom (6/12/2013)
 * (joao-v-mateiro@ptinovacao.pt) 
 *  
 */

#ifndef __HAL_ERPS_H__
#define __HAL_ERPS_H__


#include "ptin_include.h"
#include "ptin_prot_erps.h"


/// Hardware Abstraction Layer
typedef struct _ptinHalErps_t {

  L7_BOOL   used;

  L7_uint16 controlVidInternal;

  L7_uint32 port0intfNum;
  L7_uint32 port1intfNum;

  // APS    
  L7_uint16 apsReqStatusTx;
  L7_uint16 apsReqStatusRx;
  L7_uint8  apsNodeIdRx[PROT_ERPS_MAC_SIZE];

} ptinHalErps_t;

/// SW Data Base containing ERPS HAL information
extern ptinHalErps_t tbl_halErps[MAX_PROT_PROT_ERPS];

/// Reference of erps_idx using internal vlan as reference
extern L7_uint8 erpsIdx_from_internalVlan[4096];


/**
 * Initialize ERPS hw abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_init(void);


/**
 * Initialize ERPS# hw abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_entry_init(L7_uint32 erps_idx);


/**
 * Send a APS packet on a specified interface and vlan
 * 
 * @author joaom (6/11/2013)
 * 
 * @param slot 
 * @param index 
 * @param apsvid 
 * @param req_state 
 * @param status 
 */
extern L7_RC_t ptin_hal_erps_sendaps(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status);


/**
 * Send 3 consecutives APS packets on ring interfaces
 * 
 * @author joaom (6/11/2013)
 * 
 * @param slot 
 * @param index 
 * @param apsvid 
 * @param req_state 
 * @param status 
 */
extern L7_RC_t ptin_hal_erps_sendapsX3(L7_uint32 erps_idx, L7_uint8 req_state, L7_uint8 status);

/**
 * Receives an APS packet on a specified interface and vlan 
 * 
 * @author joaom (6/14/2013)
 * 
 * @param erps_idx 
 * @param req_status 
 * @param nodeid 
 * @param rxport 
 */
extern L7_RC_t ptin_hal_erps_rcvaps(L7_uint32 erps_idx, L7_uint8 *req_state, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport);


#endif //__HAL_ERPS_H__


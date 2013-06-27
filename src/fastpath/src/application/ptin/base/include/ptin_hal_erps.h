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


#include "ptin_globaldefs.h"
#ifdef PTIN_ENABLE_ERPS

#ifndef __HAL_ERPS_H__
#define __HAL_ERPS_H__


#include "ptin_include.h"
#include "ptin_prot_erps.h"

/// Mac Addr used as APS Src Mac and as ERP Node ID
extern L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN];


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
extern L7_RC_t ptin_hal_erps_entry_init(L7_uint8 erps_idx);


/**
 * DEInitialize ERPS# hw abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_entry_deinit(L7_uint8 erps_idx);


/**
 * DEInitialize ERPS hw abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 */
extern L7_RC_t ptin_hal_erps_deinit(void);


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
extern L7_RC_t ptin_hal_erps_sendaps(L7_uint8 erps_idx, L7_uint8 req_state, L7_uint8 status);


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
extern L7_RC_t ptin_hal_erps_sendapsX3(L7_uint8 erps_idx, L7_uint8 req_state, L7_uint8 status);

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
extern L7_RC_t ptin_hal_erps_rcvaps(L7_uint8 erps_idx, L7_uint8 *req_state, L7_uint8 *status, L7_uint8 *nodeid, L7_uint32 *rxport);

/**
 * Block or unblock ERP Port
 * 
 * @author joaom (6/25/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
extern int ptin_hal_erps_reconfigEvc(L7_uint8 erps_idx);


#endif //__HAL_ERPS_H__

#endif  // PTIN_ENABLE_ERPS


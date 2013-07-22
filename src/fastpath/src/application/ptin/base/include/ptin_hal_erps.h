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

/// MAC Addr used as APS Src MAC and as ERP Node ID
extern L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN];


/// Hardware Abstraction Layer
typedef struct _ptinHalErps_t {

  L7_BOOL   used;                   ///< ENTRY_FREE/ENTRY_BUSY

  L7_uint16 controlVidInternal;

  L7_uint32 port0intfNum;
  L7_uint32 port1intfNum;

  // APS
  L7_uint16 apsReqStatusTx;
  L7_uint16 apsReqStatusRx;
  L7_uint8  apsNodeIdRx[PROT_ERPS_MAC_SIZE];

  // Packets
  L7_uint32 apsPacketsTx[2];
  L7_uint32 apsPacketsRxGood[2];
  L7_uint32 apsPacketsRxDropped[2];

  L7_uint8 hwSync;                  ///< configures VLAN on switch
  L7_uint8 hwFdbFlush;              ///< FDB Flush by VLAN

} ptinHalErps_t;

/// SW Data Base containing ERPS HAL information
extern ptinHalErps_t tbl_halErps[MAX_PROT_PROT_ERPS];

/// Reference of erps_idx using internal VLAN ID as reference
extern L7_uint8 erpsIdx_from_controlVidInternal[4096];


/**
 * Initialize ERPS HW abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_init(void);


/**
 * Print APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_counters(L7_uint8 erps_idx);

/**
 * Clear APS Counters
 * 
 * @author joaom (7/09/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_countersClear(L7_uint8 erps_idx);

/**
 * Initialize ERPS# HW abstraction layer
 * 
 * @author joaom (7/05/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_convert_vid_init(L7_uint8 erps_idx);


/**
 * Initialize ERPS# HW abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_entry_init(L7_uint8 erps_idx);


/**
 * DEInitialize ERPS# HW abstraction layer
 * 
 * @author joaom (6/12/2013)
 * 
 * @param erps_idx 
 */
extern L7_RC_t ptin_hal_erps_entry_deinit(L7_uint8 erps_idx);


/**
 * Clear ERPS hw abstraction layer
 * 
 * @author joaom (7/22/2013)
 * 
 */
extern L7_RC_t ptin_hal_erps_clear(void);


/**
 * DEInitialize ERPS HW abstraction layer
 * 
 * @author joaom (6/17/2013)
 * 
 */
extern L7_RC_t ptin_hal_erps_deinit(void);


/**
 * Send a APS packet on a specified interface and VLAN
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
 * Receives an APS packet on a specified interface and VLAN 
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
 * Block or unblock ERP Port and/or Flush FDB
 * 
 * @author joaom (6/25/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
extern int ptin_hal_erps_hwreconfig(L7_uint8 erps_idx);

/**
 * Block or unblock ERP Port and/or Flush FDB
 * 
 * @author joaom (6/25/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
extern int ptin_hal_erps_forceHwreconfig(L7_uint8 erps_idx);

/**
 * If the VLAN is protected force HW reconfiguration
 * 
 * @author joaom (07/04/2013)
 * 
 * @param erps_idx
 * 
 * @return int 
 */
extern int ptin_hal_erps_evcIsProtected(L7_uint root_intf, L7_uint16 int_vlan);

/**
 * Get MEP alarm
 * 
 * @author joaom (6/28/2013)
 * 
 * @param slot    Unused
 * @param index   MEP Idx
 * 
 * @return int    SF or No SF
 */
extern int ptin_hal_erps_rd_alarms(L7_uint8 slot, L7_uint32 index);



#endif //__HAL_ERPS_H__

#endif  // PTIN_ENABLE_ERPS


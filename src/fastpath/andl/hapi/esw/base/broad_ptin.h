#ifndef _BROAD_PTIN_H
#define _BROAD_PTIN_H

#include "dapi.h"
#include "dapi_struct.h"


/**
 * Initialize HAPI PTin data structures
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinDataInit(void);

/**
 * Initialize HAPI PTin module
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinInit(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Apply hardware procedure
 * 
 * @param usp 
 * @param cmd 
 * @param data 
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadHwApply(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Get slot mode list
 * 
 * @param usp 
 * @param cmd 
 * @param data NULL
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinSlotMode(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Set MEF Extension parameters
 * 
 * @param usp : interface
 * @param cmd : no meaning
 * @param data : priority (int)
 * @param dapi_g : port definitions
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinPortExt(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Read counters from a physical interface
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEthRFC2819_PortStatistics_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t hapiBroadPtinCountersRead(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Clear counters
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEthRFC2819_PortStatistics_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t hapiBroadPtinCountersClear(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Get counters activity summary
 * 
 * @param usp 
 * @param cmd 
 * @param data ptin_HWEth_PortsActivity_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinCountersActivityGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Uses vlan translation functionalities
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_vlanXlate_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinVlanTranslate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Define port groups for egress translation
 * 
 * @param usp 
 * @param cmd 
 * @param data: structure ptin_vlanXlate_classId_t
 * @param dapi_g 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinVlanTranslatePortGroups(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Define general vlan settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinVlanModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Define vlan mode settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinBridgeVlanModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Define vlan port settings
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_mode_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinBridgeVlanPortControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Define vlan mode settings related to Multicast groups
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinBridgeVlanMulticastSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Configure Multicast egress ports
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinMulticastEgressPortSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Configure Virtual ports
 * 
 * @param usp 
 * @param cmd 
 * @param data : ptin_bridge_vlan_multicast_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinVirtualPortSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Set crossconnections
 * 
 * @param usp 
 * @param cmd 
 * @param data: ptin_bridge_crossconnect_t structure
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t hapiBroadPtinBridgeCrossconnect(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * BW Policers implementation using Field Processor
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_fieldproc)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t hapiBroadPtinBwPolicer(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Counters implementation using Field Processor
 * 
 * @param usp 
 * @param cmd 
 * @param data : (struct_bwPolicer)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t hapiBroadPtinFpCounters(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Traffic Rate limiters
 * 
 * @param usp 
 * @param cmd 
 * @param data : (ptin_stormControl_t)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadStormControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Enable PRBS generator/checker
 * 
 * @param usp : portInfo
 * @param enable : enable
 * @param dapi_g : port driver
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPTinPrbsEnable(DAPI_USP_t *usp, L7_BOOL enable, DAPI_t *dapi_g);

/**
 * Read number of PRBS errors
 * 
 * @param usp : portInfo
 * @param rxErrors : number of errors (-1 if no lock)
 * @param dapi_g   : port driver
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPTinPrbsRxStatus(DAPI_USP_t *usp, L7_uint32 *rxErrors, DAPI_t *dapi_g);

/**
 * Get system resources
 * 
 * @param usp 
 * @param cmd 
 * @param data : (st_ptin_policy_resources)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinResourcesGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/**
 * Add L3 Host IP
 * 
 * @param usp 
 * @param cmd 
 * @param data :
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadPtinL3Manage(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#endif  /* _BROAD_PTIN_H */


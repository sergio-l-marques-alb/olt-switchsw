#ifndef _DTL_PTIN_H
#define _DTL_PTIN_H

#include "l7_common.h"
#include "ptin_globaldefs.h"
#include "ptin_structs.h"


/**
 * Initialize DTL PTin module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinInit(void);

/**
 * Set Port Ext definitions
 * 
 * @param intIfNum : interface
 * @param mefExt : MEF Ext parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinL2PortExtSet(L7_uint32 intIfNum, ptin_HWPortExt_t *mefExt);

/**
 * Get MEF Ext definitions
 * 
 * @param intIfNum : interface
 * @param mefExt : MEF Ext parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinL2PortExtGet(L7_uint32 intIfNum, ptin_HWPortExt_t *mefExt);

/**
 * Read counters (physical interfaces)
 *  
 * Note: currently masks are ignored, meaning that all values are read
 * 
 * @param data Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinCountersRead(ptin_HWEthRFC2819_PortStatistics_t *data);

/**
 * Clear counters (physical interfaces)
 *  
 * Note: currently masks are ignored, but the use of such structure as the 
 * input parameter is to allow selective counters clear 
 * 
 * @param data Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinCountersClear(ptin_HWEthRFC2819_PortStatistics_t *data);

/**
 * Get counters activity (physical interfaces)
 *  
 * Note: currently masks are ignored, but the use of such structure as the 
 * input parameter is to allow selective fields read
 * 
 * @param data Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinCountersActivityGet(ptin_HWEth_PortsActivity_t *data);

/**
 * Make use of ingress/egress vlan translations
 *  
 * @param intIfNum : Interface to apply vlan translation  
 * @param xlate_ptr : descriptor for translation data 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinVlanTranslate( L7_uint32 intIfNum, ptin_vlanXlate_t *xlate_ptr );

/**
 * Define groups of ports for vlan egress translations 
 *  
 * @param intIfNum : Interface to be applied to a portGroup 
 * @param portsGroup : descriptor with ports group data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinVlanTranslateEgressPortsGroup( L7_uint32 intIfNum, ptin_vlanXlate_classId_t *portsGroup );

/**
 * Vlan specific configurations
 * 
 * @param vlan_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinVlanSettings( ptin_vlan_mode_t *vlan_mode );

/**
 * Vlan specific configurations
 * 
 * @param vlan_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinVlanDefinitions( ptin_bridge_vlan_mode_t *vlan_mode );

/**
 * Cross-connections 
 *  
 * @param intIfNum : First interface for the cross-connection 
 * @param intIfNum2 : Second interface for the cross-connection
 * @param cross_connect : descriptor with cross-connect data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinBridgeCrossconnect( L7_uint32 intIfNum, L7_uint32 intIfNum2, ptin_bridge_crossconnect_t *cross_connect );

/**
 * Apply/Get bandwidth policer
 *  
 * @param bw_policer : Bandwidth policer 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinBWPolicer( ptin_bwPolicer_t *bw_policer );

/**
 * Apply/Get EVC Statistics
 *  
 * @param evcStats : EVC Stats structure
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinEvcStats( ptin_evcStats_t *evcStats );

/**
 * Add/Remove rule to trap DHCP packets
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinPacketsTrap( L7_uint32 intIfNum, DAPI_SYSTEM_CMD_t *dapiCmd );

/**
 * Storm Control configuration
 *  
 * @param intIfNum : Interface 
 * @param rateLimit: Rate Limit profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinStormControl( L7_uint32 intIfNum, L7_BOOL enable, ptin_stormControl_t *stormControl );

/**
 * PRBS tx/rx
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinPcsPrbs( L7_uint32 intIfNum, DAPI_SYSTEM_CMD_t *dapiCmd );

/**
 * Slot mode
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinSlotMode( ptin_slotmode_t *dapiCmd );

/**
 * Consult available hardware resources
 *  
 * @param resources: Available resources
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinHwResources( st_ptin_policy_resources *resources );

/**
 * Vlan Multicast configurations
 * 
 * @param mc_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinVlanBridgeMulticast( ptin_bridge_vlan_multicast_t *mc_mode );

/**
 * Multicast egress port configurations
 * 
 * @param mc_mode : descriptor with port configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinMulticastEgressPort(L7_uint32 intIfNum, ptin_bridge_vlan_multicast_t *mc_mode );

/**
 * Virtual port configurations
 * 
 * @param vport : descriptor with virtual port configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t dtlPtinVirtualPort(L7_uint32 intIfNum, ptin_vport_t *vport );

#endif

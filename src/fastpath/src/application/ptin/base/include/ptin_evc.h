/**
 * ptin_evc.h 
 *  
 * Implements the EVC configuration and management 
 *
 * Created on: 2011/07/15
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_EVC_H
#define _PTIN_EVC_H

#include "ptin_include.h"
#include "ptin_intf.h"

/**********************************************************
 * AVL TREE with Ext "offline" EvcId from NGPON2 groups
 **********************************************************/
/**********************************************************
 * Internal functions
 **********************************************************/

#ifdef NGPON2_SUPPORTED

#define MAX_NETWORK_SERVICES 256
/* Extended EvcId key */
typedef struct
{
  L7_uint32 ext_evcId;
} ptinExtNGEvcIdDataKey_t;

/* Extended EvcId Tree node */
typedef struct ptinExtNGEvcIdInfoData_s
{
  ptinExtNGEvcIdDataKey_t extNGEvcIdDataKey;
  ptin_HwEthMef10Evc_t    evcNgpon2;
  void *next;
} ptinExtNGEvcIdInfoData_t;

typedef struct ptinExtNGEvcIdAvlTree_s
{
  avlTree_t                 extNGEvcIdAvlTree;
  avlTreeTables_t           *extNGEvcIdTreeHeap;
  ptinExtNGEvcIdInfoData_t  *extNGEvcIdDataHeap;
} ptinExtNGEvcIdAvlTree_t;

extern ptinExtNGEvcIdAvlTree_t extNGEvcId_avlTree;

#endif
/**
 * Initializes EVCs data structures
 * 
 * @return L7_RC_t L7_SUCCESS
 */
extern L7_RC_t ptin_evc_init(void);


/**
 * Initializes EVCs for each platform
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_startup(void);


/******************************************************** 
 * FOR MESSAGES PROCESSING
 ********************************************************/

/**
 * Gets an EVC configuration
 * 
 * @param evcConf Pointer to the output struct (index field is used as input param)
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get(ptin_HwEthMef10Evc_t *evcConf);

/**
 * Creates or reconfigures an EVC
 * 
 * @param evcConf Pointer to the input struct
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_create(ptin_HwEthMef10Evc_t *evcConf);

/**
 * Add port to an EVC
 *  
 * @param evc_ext_id : EVC index
 * @param evc_intf : Port information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_port_add(L7_uint32 evc_ext_id, ptin_HwEthMef10Intf_t *evc_intf);


/**
 * Remove port from an EVC
 *  
 * @param evc_ext_id : EVC index
 * @param evc_intf : Port information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_port_remove(L7_uint32 evc_ext_id, ptin_HwEthMef10Intf_t *evc_intf);
/**
 * Allocates an EVC entry from the pool
 * 
 * @author alex (9/18/2013)
 * 
 * @param evc_ext_id EVC extended index (input)
 * @param evc_id     Allocated index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_offline_entry_add(ptin_HwEthMef10Evc_t *EvcConf);
/**
 * EVC options reconfiguration
 * 
 * @param evc_ext_id : EVC extended id
 * @param evcOptions : EVC options 
 * @param queue_type : queue type 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_config(L7_uint32 evc_ext_id, ptin_HwEthMef10EvcOptions_t *evcOptions, L7_int queue_type);

/**
 * Deletes an EVC
 * 
 * @param evc_id
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_delete(L7_uint32 evc_ext_id);


/**
 * Allocates an EVC entry from the pool
 * 
 * @author Rui Fernandes: rui-f-fernandes@telecom.pt  
 * 
 * @param evc_ext_id EVC extended index (input)
 * @param evc_id     Allocated index (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_offline_entry_remove(L7_uint32 evc_ext_id);

/**
 * Destroys all EVCs (except INBAND!)
 * 
 * @return L7_RC_t L7_SUCCESS
 */
extern L7_RC_t ptin_evc_destroy(L7_uint32 evc_ext_id);

/**
 * Destroys all EVCs (except INBAND!)
 * 
 * @return L7_RC_t L7_SUCCESS
 */
extern L7_RC_t ptin_evc_destroy_all(void);

/**
 * Adds a bridge to a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_p2p_bridge_add(ptin_HwEthEvcBridge_t *evcBridge);

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface
 * 
 * @param evcBridge Bridge info
 * @note The EVC id is the extended one. 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_p2p_bridge_remove(ptin_HwEthEvcBridge_t *evcBridge);

/**
 * Adds a flooding vlan
 * 
 * @param evc_ext_id  : EVC extended id
 * @param ptin_intf   : port of which client_vlan belongs
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : outer vlan of transmitted  packets
 * @param inner_vlan  : inner vlan of transmitted  packets
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_flood_vlan_add( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 client_vlan,
                                        L7_uint16 outer_vlan, L7_uint16 inner_vlan );

/**
 * Removes a flooding vlan
 * 
 * @param evc_ext_id  : EVC extended id
 * @param ptin_intf   : port of which client_vlan belongs
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : outer vlan of transmitted packets
 * @param inner_vlan  : inner vlan of transmitted packets
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_flood_vlan_remove( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 client_vlan,
                                           L7_uint16 outer_vlan, L7_uint16 inner_vlan );

/**
 * Storm control configurations
 */

/**
 * Initializes Storm Control configurations for all vlans
 * 
 * @return L7_RC_t: L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_stormControl_init(void);

/**
 * Get storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_stormControl_get(ptin_stormControl_t *stormControl);

/**
 * Set storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_stormControl_set(L7_BOOL enable, ptin_stormControl_t *stormControl);

/**
 * Reset storm control configurations
 * 
 * @param stormControl 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_evc_stormControl_reset(ptin_stormControl_t *stormControl);

/**
 * Bandwidth Policers management functions
 */

/**
 * Read data of a bandwidth profile
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile 
 * @param meter      : Policer meter (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_get(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile, ptin_bw_meter_t *meter);

/**
 * Apply a bandwidth profile to an EVC and (optionally) to a 
 * specific client 
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile 
 * @param meter      : Policer meter 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_set(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile, ptin_bw_meter_t *meter);

/**
 * Remove a bandwidth profile to an EVC 
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : bw profile 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_bwProfile_delete(L7_uint32 evc_ext_id, ptin_bw_profile_t *profile);

/**
 * EVC Counters management functions
 */

/**
 * Read RX statistics of a particular EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile 
 * @param stats      : Statistics data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_get(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile, ptin_evcStats_counters_t *stats);

/**
 * Add a rule to make packets counting of a specific EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_set(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile);

/**
 * Delete a rule to make packets counting of a specific EVC
 * 
 * @param evc_ext_id : EVC extended index
 * @param profile    : evcStats profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FALSE
 */
extern L7_RC_t ptin_evc_evcStats_delete(L7_uint32 evc_ext_id, ptin_evcStats_profile_t *profile);


/******************************************************** 
 * FOR PTIN INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Determines if a particular EVC is in use
 * 
 * @param evc_ext_id : EVC extended id
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_in_use(L7_uint32 evc_ext_id);

/**
 * Determines if a particular Port/LAG is being used on any EVC
 * 
 * @param intf_idx PTin interface#
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_intf_in_use(L7_uint intf_idx);

/**
 * Determines if a particular Port/LAG is being used on EVC Id
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_intf_in_use_on_evc(L7_uint32 evc_ext_id, L7_uint ptin_port);

/**
 * Determines if a particular Port/LAG is Leaf on EVC Id
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * 
 * @return L7_BOOL L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_is_intf_leaf(L7_uint32 evc_ext_id, L7_uint ptin_port);

/**
 * Get port type on EVC Id
 *  
 * @param evc_ext_id  
 * @param ptin_port 
 * @param portType  
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_port_type_get(L7_uint32 evc_ext_id, L7_uint ptin_port, L7_uint8 *portType);

/**
 * Get port type on Intenal VLAN Id
 *  
 * @param internalVlan  
 * @param ptin_port 
 * @param portType  
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_BOOL ptin_evc_internal_vlan_port_type_get(L7_uint32 internalVlan, L7_uint32 ptin_port, L7_uint8 *portType);

/**
 * Get interface configuration within an EVC
 *  
 * @param evc_ext_id : EVC extended id
 * @param ptin_intf  : PTin interface 
 * @param cfg        : interface configuration 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intfCfg_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_evc_intfCfg_t *cfg);

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evcConf      : Pointer to the evc configuration struct 
 *  
 * @note The returned EVC id will be the extended one 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_fromIntVlan(L7_uint16 internalVlan, ptin_HwEthMef10Evc_t *evcConf);

/**
 * Gets the internal vlan for a particular evc and interface
 * 
 * @param evc_ext_id : EVC extended id 
 * @param ptin_intf  : interface
 * @param intVlan    : Internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intVlan_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, L7_uint16 *intVlan);

/**
 * Gets the root vlan (internal) for a particular evc
 * 
 * @param evc_ext_id  : EVC extended id 
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intRootVlan_get(L7_uint32 evc_ext_id, L7_uint16 *intRootVlan);


/**
 * Gets flag options for a particular evc
 * 
 * @param evc_ext_id: EVC extended id 
 * @param flags     : Flag options 
 * @param mc_flood  : Multicast flood
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_flags_get(L7_uint32 evc_ext_id, L7_uint32 *flags, L7_uint32 *mc_flood);

/**
 * Gets flag options for a particular (internal) OVlan
 * 
 * @param intVlan   : Internal OVlan
 * @param flags     : Flag options 
 * @param mc_flood  : Multicast flood
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_flags_get_fromIntVlan(L7_uint16 intOVlan, L7_uint32 *flags, L7_uint32 *mc_flood);

/**
 * Get the outer+inner external vlan for a specific 
 * interface+evc_id+innerVlan. 
 *  
 * @param ptin_port  : ptin_port
 * @param evc_ext_id : EVC extended index 
 * @param evc_ext_id : EVC internal index 
 * @param innerVlan  : Inner vlan
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                      is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get(L7_uint32 ptin_port, L7_uint32 evc_ext_id, L7_uint32 evc_int_id, L7_uint16 innerVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan);

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Get the outer+inner external vlan for a specific 
 * evc_id+l2intf (only applicable to QUATTRO services). 
 * 
 * @param evc_ext_id      : EVC extended index 
 * @param evc_int_id      : EVC internal index  
 * @param l2intf_id       : l2intf_id 
 * @param port            : Physical port for transmission (out)
 * @param extOVlan        : External outer-vlan 
 * @param extIVlan        : External inner-vlan (01 means that there 
 *                      is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get_from_l2intf(L7_uint32 evc_ext_id, L7_uint32 evc_int_id, L7_uint32 l2intf_id,
                                               L7_uint32 *port, L7_uint16 *extOVlan, L7_uint16 *extIVlan);
#endif

/**
 * Get internal vlans, from external vlans and the interface
 * 
 * @author mruas (8/6/2013)
 * 
 * @param ptin_intf: Interface
 * @param extOVlan : external outer vlan
 * @param extIVlan : external inner vlan
 * @param intOVlan : internal outer vlan
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_evc_intVlan_get_fromOVlan(ptin_intf_t *ptin_intf, L7_uint16 extOVlan, L7_uint16 extIVlan,
                                              L7_uint16 *intOVlan);

/**
 * Get interface type for a given internal vlan
 * 
 * @param intVlan  : Internal vlan 
 * @param ptin_port: Interface
 * @param type     : Interface type (output)
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1,
 *                    PTIN_EVC_INTF_NOTUSED=255
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intf_type_get(L7_uint16 intVlan, L7_uint32 ptin_port, L7_uint8 *type);

/**
 * Check if the given interface is of type ROOT. 
 * 
 * @param intVlan   : Internal vlan 
 * @param ptin_port : Interface
 * 
 * @return L7_BOOL : L7_TRUE/L7_FALSE
 */
extern L7_BOOL ptin_evc_intf_isRoot(L7_uint16 intVlan, L7_uint32 ptin_port);

/**
 * Get the list of interfaces associated to a internal vlan
 * 
 * @param intVlan  : Internal vlan 
 * @param type     : Interface type 
 *                    PTIN_EVC_INTF_ROOT=0,
 *                    PTIN_EVC_INTF_LEAF=1
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intfType_getList(L7_uint16 intVlan, L7_uint8 type, ptin_port_bmp_t *intfList);

/**
 * Get next client, belonging to an EVC
 * 
 * @param evc_id      : evc index
 * @param ptin_intf   : interface
 * @param clientFlow  : Current client data
 * @param clientFlow_next : Next client data
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients (ivid_next==0)
 *  L7_NOT_EXIST tells the reference vlan was not found
 *  L7_NOT_SUPPORTED tells this evc does not support clients
 *  L7_FAILURE in case of error
 */
extern
L7_RC_t ptin_evc_client_next( L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next);

/**
 * Configures a root port (unstacked EVCs) 
 *  1. Associates the internal VLAN to the root intf
 * 
 * @param root_intf     Root interface (ptin_intf)
 * @param int_vlan      Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_root_unblock(L7_uint root_intf, L7_uint16 int_vlan);

/**
 * Removes a root port (unstacked EVCs) 
 * 
 * @param root_intf Root interface (ptin_intf)
 * @param int_vlan  Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_root_block(L7_uint root_intf, L7_uint16 int_vlan);

/**
 * Flushes FDB for all int.VLAN associated to this evc_id
 * 
 * @param int_vlan  Root Inner VLAN
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_fdbFlushByVlan(L7_uint16 int_vlan);

/**
 * Returns internal VLAN and the correspondig EVC ID
 * 
 * @return L7_int evc_id on match condition
 */
extern L7_int switching_erps_internalVlan_get(L7_int initial_evc_id, L7_uint8 erps_ptin_port0, L7_uint8 erps_ptin_port1, L7_uint8 *vid_bmp, L7_uint16 *internalVlan);

/**
 * Flushes all VLANs' FDB associated to this ptin_port
 * 
 * @param ptin_port  Port to Flush
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t switching_fdbFlushVlanByPort(L7_uint8 ptin_port);

#if PTIN_IGMP_STATS_IN_EVCS
/**
 * Get a pointer to IGMP stats
 * 
 * @param intVlan      : internal vlan
 * @param ptin_port    : interface
 * @param stats_intf   : pointer to stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_get_fromIntVlan(L7_uint16 intVlan, L7_uint32 ptin_port, ptin_IGMP_Statistics_t **stats_intf);

/**
 * Get a pointer to IGMP stats
 * 
 * @param evc_ext_id  : EVC extended index 
 * @param ptin_intf   : interface
 * @param stats_intf  : Stats
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_get(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stats_intf);

/**
 * Clear IGMP stats of one interface
 * 
 * @param evc_ext_id  : EVC extended index 
 * @param ptin_intf   : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_clear(L7_uint32 evc_ext_id, ptin_intf_t *ptin_intf);

/**
 * Clear IGMP stats of all interfaces
 * 
 * @param evc_ext_id  : EVC extended index 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
extern L7_RC_t ptin_evc_igmp_stats_clear_all(L7_uint32 evc_ext_id);
#endif

/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Gets an EVC configuration from an internal vlan as input 
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evc_ext_id   : EVC extended id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint32 *evc_ext_id);

/**
 * Gets an EVC internal id from an internal vlan as input
 * parameter 
 *  
 * @param internalVlan : Internal vlan
 * @param evc_ext_id   : EVC internal Id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_internal_evcIdfromIntVlan(L7_uint16 internalVlan, L7_uint32 *evc_id);

/**
 * Get EVC ext id, from NNI vlan
 * 
 * @param nni_ovlan  : NNI OVLAN
 * @param evc_ext_id : EVC extended id list 
 * @param number_of_evcs : Maximum (in) and returned (out) 
 *                         number of EVCs
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_evcId_fromNNIvlan(L7_uint16 nni_ovid, L7_uint32 *evc_ext_id, L7_uint *number_of_evcs);

/**
 * Get (internal) VLAN list, from NNI vlan
 * 
 * @param nni_ovlan  : NNI OVLAN
 * @param intVid     : VLAN id list 
 * @param number_of_evcs : Maximum (in) and returned (out) 
 *                         number of VLANs
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_intVlan_fromNNIvlan(L7_uint16 nni_ovid, L7_uint16 *intVid, L7_uint *number_of_vlans);

/**
 * Get NNI VLAN from EVC ext id
 * 
 * @param evc_ext_id : EVC extended id 
 * @param nni_ovid   : NNI OVLAN (output)
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_get_NNIvlan_fromEvcId(L7_uint32 evc_ext_id, L7_uint16 *nni_ovid);

/**
 * Get the outer+inner external vlan for a specific 
 * interface+outer+inner internal vlan. 
 *  
 * @param ptin_port  : FP interface#
 * @param intOVlan   : Internal outer-vlan 
 * @param intIVlan   : Internal inner-vlan (0 means that there 
 *                     is no inner vlan)
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get_fromIntVlan(L7_uint32 ptin_port, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extOVlan, L7_uint16 *extIVlan);

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
/**
 * Get the outer+inner external vlan for a specific oVLAN+l2intf_id
 * (only applicable to QUATTRO services). 
 *  
 * @param intOVlan   : Internal outer-vlan 
 * @param l2intf_id  : l2intf id 
 * @param ptin_port  : Physical port for transmission (out) 
 * @param extOVlan   : External outer-vlan 
 * @param extIVlan   : External inner-vlan (01 means that there 
 *                     is no inner vlan)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_extVlans_get_from_IntVlan_l2intf(L7_uint16 intOVlan, L7_uint32 l2intf_id,
                                                      L7_uint32 *ptin_port, L7_uint16 *extOVlan, L7_uint16 *extIVlan);
#endif

/**
 * Gets the root vlan (internal) from the internal vlan
 * 
 * @param intVlan     : Internal vlan
 * @param intRootVlan : Internal root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_intRootVlan_get_fromIntVlan(L7_uint16 intVlan, L7_uint16 *intRootVlan);

/**
 * Return EVC type. 
 * 
 * @param evc_id_ext : extended evc id
 * @param evc_type   : EVC type (output)
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_check_evctype(L7_uint32 evc_id_ext, L7_uint8 *evc_type);

/**
 * Return EVC type from internal vlan. 
 *  
 * @param intVlan    : Internal outer-vlan 
 * @param evc_type   : evc type (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_check_evctype_fromIntVlan(L7_uint16 intVlan, L7_uint8 *evc_type);

/**
 * Check if a specific internal VLAN is a QUATTRO service.
 *  
 * @param intVlan    : Internal outer-vlan 
 * 
 * @return L7_BOOL: L7_TRUE or L7_FALSE
 */
extern L7_BOOL ptin_evc_is_quattro_fromIntVlan(L7_uint16 intVlan);

/**
 * Check if the EVC related to an internal vlan is stacked. 
 *  
 * @param intVlan    : Internal outer-vlan 
 * @param is_stacked : Is EVC stacked? (output)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_check_is_stacked_fromIntVlan(L7_uint16 intVlan, L7_BOOL *is_stacked);

/**
 * Validate interface and vlan belonging to a valid interface 
 * inside a valid EVC 
 * 
 * @param ptin_port: source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
extern L7_RC_t ptin_evc_intfVlan_validate(L7_uint32 ptin_port, L7_uint16 intVlan);

/**
 * Get next client, belonging to a vlan
 * 
 * @param intVid    : internal vlan
 * @param ptin_port : port
 * @param clientFlow  : Current client data
 * @param clientFlow_next : Next client data
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients
 *  L7_NOT_EXIST tells the reference vlan was not found
 *  L7_NOT_SUPPORTED tells this evc does not support clients
 *  L7_FAILURE in case of error
 */
extern
L7_RC_t ptin_evc_vlan_client_next( L7_uint intVid, L7_uint32 ptin_port, ptin_HwEthEvcFlow_t *clientFlow, ptin_HwEthEvcFlow_t *clientFlow_next);

#if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
typedef struct
{
  L7_BOOL         in_use;
  ptin_bw_meter_t meter;
  L7_int          policer_id;
} l2intf_entry_policer_t;

typedef struct {
    unsigned long   l2intf_id;
    ptin_intf_t     pon;
    unsigned short  gem_id;
    unsigned int    onu;
    /* Policer */
    l2intf_entry_policer_t policer;
} l2intf_entry_t;

/**
 * Initialize L2intf Database
 * 
 * @author mruas (30/12/20)
 */
extern void l2intf_db_init(void);

/**
 * Determine l2intf_id from pon port and gem id
 * 
 * @param pon_port
 * @param gem_id 
 * 
 * @return l2intf_id (output) 
 */
extern L7_uint32 l2intf_db_search(L7_uint16 pon_port, L7_uint16 gem_id);

/**
 * Search for a specific entry in L2intf Database
 * 
 * @author mruas (30/12/20)
 *  
 * @param l2intf_id (in) 
 * @param entry (out)
 * 
 * @return L7_RC_t : L7_SUCCESS, L7_NOT_EXIST
 */
extern L7_RC_t l2intf_db_data_get(L7_uint32 l2intf_id, l2intf_entry_t *entry);

/**
 * Insert a new entry in L2intf Database
 * 
 * @author mruas (30/12/20)
 * 
 * @param entry 
 * 
 * @return L7_RC_t : L7_SUCCESS, L7_TABLE_IS_FULL
 */
extern L7_RC_t l2intf_db_alloc(l2intf_entry_t *entry);

/**
 * Remove an entry from the L2intf Database
 * 
 * @author mruas (30/12/20)
 * 
 * @param l2intf_id (in)
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
extern L7_RC_t l2intf_db_free(L7_uint32 l2intf_id);
#if 0
extern int intf_vp_DB(int _0init_1insert_2remove_3find, intf_vp_entry_t *entry);
#endif
#endif

/**
 * Adds a flow to the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern
L7_RC_t ptin_evc_flow_add(ptin_HwEthEvcFlow_t *evcFlow);

/**
 * Removes a flow from the EVC
 * 
 * @param evcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern
L7_RC_t ptin_evc_flow_remove(ptin_HwEthEvcFlow_t *evcFlow);

/**
 * Macbridge Flow Client Packages Add
 * 
 * @param ecvFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_macbridge_client_packages_add(ptin_evc_macbridge_client_packages_t *ecvFlow);

/**
 * Macbridge Flow Client Packages Remove
 * 
 * @param ecvFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_evc_macbridge_client_packages_remove(ptin_evc_macbridge_client_packages_t *ecvFlow);

/**
 * Gets the flooding vlans list
 * 
 * @param ptin_port   : leaf interface
 * @param intVlan     : internal Vlan
 * @param client_vlan : client vlan to apply this flooding vlan
 * @param outer_vlan  : list of outer vlans
 * @param inner_vlan  : list of inner vlans 
 * @param number_of_vlans : Size of returned lists
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_flood_vlan_get( L7_uint32 ptin_port, L7_uint intVlan, L7_uint client_vlan,
                                 L7_uint16 *outer_vlan, L7_uint16 *inner_vlan, L7_uint16 *number_of_vlans );


/*********************************************************** 
 * IGMP/DHCP/PPPoE instances management
 ***********************************************************/

/**
 * Set igmp instance for a particular evc
 * 
 * @param evc_ext_id 
 * @param igmp_inst 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_igmpInst_set(L7_uint32 evc_ext_id, L7_uint igmp_inst);

/**
 * Get igmp instance from a particular evc
 * 
 * @param evc_ext_id 
 * @param igmp_inst (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_igmpInst_get(L7_uint32 evc_ext_id, L7_uint *igmp_inst);

/**
 * Set dhcp instance for a particular evc
 * 
 * @param evc_ext_id 
 * @param dhcp_inst 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_dhcpInst_set(L7_uint32 evc_ext_id, L7_uint dhcp_inst);

/**
 * Get dhcp instance from a particular evc
 * 
 * @param evc_ext_id 
 * @param dhcp_inst (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_dhcpInst_get(L7_uint32 evc_ext_id, L7_uint *dhcp_inst);

/**
 * Set pppoe instance for a particular evc
 * 
 * @param evc_ext_id 
 * @param pppoe_inst 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_pppoeInst_set(L7_uint32 evc_ext_id, L7_uint pppoe_inst);

/**
 * Get pppoe instance from a particular evc
 * 
 * @param evc_ext_id 
 * @param pppoe_inst (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_pppoeInst_get(L7_uint32 evc_ext_id, L7_uint *pppoe_inst);

/**
 * Verify if a particular evc is MAC Bridge
 * 
 * @param evc_ext_id 
 * @param is_mac_bridge (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern
L7_RC_t ptin_evc_mac_bridge_check(L7_uint32 evc_ext_id, L7_BOOL *is_mac_bridge);

/**
 * Convert EVC extended id into the internal id
 * 
 * @author alex (9/19/2013)
 * 
 * @param evc_ext_id 
 * @param evc_id 
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_evc_ext2int(L7_uint32 evc_ext_id, L7_uint32 *evc_id);

/**
 * Get L3 Intf Id of EVC Port
 * 
 * @author melo (01/06/2015) 
 *  
 * @param evc_ext_id 
 * @param ptin_port 
 * @param l3_intf_id 
 * 
 * @return L7_RC_t 
 *  
 * @notes Disabled the creation of L3 egress ports on  Multicast
 *        Services!!!
 */
extern L7_RC_t ptin_evc_l3_intf_get(L7_uint32 evc_ext_id, L7_uint32 ptin_port, L7_int *l3_intf_id);

/**
 * Get Multicast Replication Table of an EVC 
 * 
 * @author melo (01/06/2015) 
 *  
 * @param evc_ext_id 
 * @param multicast_group 
 * 
 * @return L7_RC_t 
 *  
 * @notes Disabled the creation of Multicast Replication tables 
 *        on Multicast Services!!!
 */
extern L7_RC_t ptin_evc_l3_multicast_group_get(L7_uint32 evc_ext_id, L7_int *multicast_group);



/**
 * Gets the list of root/leaf interfaces of an EVC
 * 
 * @param evc_id 
 * @param mef_type  PTIN_EVC_INTF_ROOT / PTIN_EVC_INTF_LEAF 
 * @param intf_list (output) Array with the root/leaf list
 * @param n_elems   (output) Nr of elems of list array
 */
extern void    ptin_evc_intf_list_get(L7_uint evc_id, L7_uint8 mef_type, L7_uint intf_list[], L7_uint *n_elems);

/**
 * Get L3 Intf Sem
 * 
 *  
 * 
 * @return void* 
 */
extern void *ptin_evc_l3_intf_sem_get(void);

/**
 * Give L3 Intf Sem
 * 
 * @param vlanId 
 * @param ptin_port 
 * 
 * @return  
 */
extern void ptin_evc_l3_intf_sem_give(L7_uint32 vlanId, L7_uint32 ptin_port);

/**
 * Test EVC functions
 *
 */

extern L7_RC_t ptin_evc_flow_replicate(L7_uint32 ptin_port, L7_uint32 evc_ext_id, L7_uint32 leaf_port);
extern L7_RC_t ptin_evc_p2p_bridge_replicate(L7_uint32 evc_ext_id, L7_uint32 ptin_port, L7_uint32 ptin_port_ngpon2, ptin_HwEthMef10Intf_t *intf);
extern L7_RC_t ptin_evc_flow_remove_port(L7_uint32 ptin_port, L7_uint32 evc_ext_id);
extern L7_RC_t ptin_evc_bwProfile_verify(L7_uint evc_id, ptin_bw_profile_t *profile);

#endif /* _PTIN_EVC_H */

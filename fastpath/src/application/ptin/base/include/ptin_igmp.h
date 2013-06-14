/**
 * ptin_igmp.h 
 *  
 * Implements the IGMP Proxy interface module
 *
 * Created on: 2011/09/19
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#ifndef _PTIN_IGMP_H
#define _PTIN_IGMP_H

#include "ptin_include.h"

/* Macros to get RFC3376 timer values */
#define PTIN_IGMP_AUTO_GMI(rv, qi, qri)                 (((rv) * (qi)) + ((qri)/10))
#define PTIN_IGMP_AUTO_OQPI(rv, qi, qri)                (((rv) * (qi)) + ((qri)/10/2))
#define PTIN_IGMP_AUTO_SQI(qi)                          ((qi)/4)
#define PTIN_IGMP_AUTO_SQC(rv)                          (rv)
#define PTIN_IGMP_AUTO_LMQC(rv)                         (rv)
#define PTIN_IGMP_AUTO_OQPT(rv, qi, qri)                (((rv) * (qi)) + ((qri)/10))
#define PTIN_IGMP_AUTO_OHPT(rv, qi, qri)                (((rv) * (qi)) + ((qri)/10))


/* Default config values (PTin custom parameters) */
#define PTIN_IGMP_DEFAULT_COS                           SNOOP_IGMP_DEFAULT_PRIO
#define PTIN_IGMP_DEFAULT_VERSION                       3
#define PTIN_IGMP_DEFAULT_IPV4                          0xA00000A /* 10.0.0.10 */
#define PTIN_IGMP_DEFAULT_FASTLEAVEMODE                 1

/* Default config values (based on RFC3376) */
#define PTIN_IGMP_DEFAULT_ROBUSTNESS                    2

#define PTIN_IGMP_DEFAULT_QUERYINTERVAL                 125 /* (s) */

#define PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL         100 /* (1/10s - 10s) */

#define PTIN_IGMP_DEFAULT_GROUPMEMBERSHIPINTERVAL       PTIN_IGMP_AUTO_GMI(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                           PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                           PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */

#define PTIN_IGMP_DEFAULT_OTHERQUERIERPRESENTINTERVAL   PTIN_IGMP_AUTO_OQPI(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (255 s) */

#define PTIN_IGMP_DEFAULT_STARTUPQUERYINTERVAL          PTIN_IGMP_AUTO_SQI(PTIN_IGMP_DEFAULT_QUERYINTERVAL) /* (32s) */

#define PTIN_IGMP_DEFAULT_STARTUPQUERYCOUNT             PTIN_IGMP_AUTO_SQC(PTIN_IGMP_DEFAULT_ROBUSTNESS)

#define PTIN_IGMP_DEFAULT_LASTMEMBERQUERYINTERVAL       10  /* (1/10s - 1s) */

#define PTIN_IGMP_DEFAULT_LASTMEMBERQUERYCOUNT          PTIN_IGMP_AUTO_LMQC(PTIN_IGMP_DEFAULT_ROBUSTNESS)

#define PTIN_IGMP_DEFAULT_UNSOLICITEDREPORTINTERVAL     1   /* (1s for IGMPv3, 10s for IGMPv2) */

#define PTIN_IGMP_DEFAULT_OLDERQUERIERPRESENTTIMEOUT    PTIN_IGMP_AUTO_OQPT(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */
#define PTIN_IGMP_DEFAULT_OLDERHOSTPRESENTTIMEOUT       PTIN_IGMP_AUTO_OHPT(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */


/* FOR STATISTICS */
// The values below must be in the same order as in L7_IGMP_Statistics_t structure
typedef enum  {
  SNOOP_STAT_FIELD_ACTIVE_GROUPS=0,
  SNOOP_STAT_FIELD_ACTIVE_CLIENTS,
  SNOOP_STAT_FIELD_IGMP_SENT,
  SNOOP_STAT_FIELD_IGMP_TX_FAILED,
  SNOOP_STAT_FIELD_IGMP_INTERCEPTED,
  SNOOP_STAT_FIELD_IGMP_DROPPED,
  SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID,
  SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID,
  SNOOP_STAT_FIELD_JOINS_SENT,
  SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS,
  SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED,
  SNOOP_STAT_FIELD_LEAVES_SENT,
  SNOOP_STAT_FIELD_LEAVES_RECEIVED,
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_V3,
  SNOOP_STAT_FIELD_GENERAL_QUERIES_SENT,
  SNOOP_STAT_FIELD_GENERAL_QUERIES_RECEIVED,
  SNOOP_STAT_FIELD_SPECIFIC_QUERIES_SENT,
  SNOOP_STAT_FIELD_SPECIFIC_QUERIES_RECEIVED,
  SNOOP_STAT_FIELD_SPECIFIC_GROUP_QUERIES_SENT,
  SNOOP_STAT_FIELD_SPECIFIC_GROUP_QUERIES_RECEIVED,
  SNOOP_STAT_FIELD_SPECIFIC_SOURCE_QUERIES_SENT,
  SNOOP_STAT_FIELD_SPECIFIC_SOURCE_QUERIES_RECEIVED,
  SNOOP_STAT_FIELD_ALL                        // This must be the last element
} ptin_snoop_stat_enum_t;

typedef struct
{
  L7_uint32 active_groups;
  L7_uint32 active_clients;
  L7_uint32 igmp_sent;
  L7_uint32 igmp_tx_failed;
  L7_uint32 igmp_intercepted;
  L7_uint32 igmp_dropped;
  L7_uint32 igmp_received_valid;
  L7_uint32 igmp_received_invalid;
  L7_uint32 joins_sent;
  L7_uint32 joins_received_success;
  L7_uint32 joins_received_failed;
  L7_uint32 leaves_sent;
  L7_uint32 leaves_received;
  L7_uint32 membership_report_v3;
  L7_uint32 general_queries_sent;
  L7_uint32 general_queries_received;
  L7_uint32 specific_queries_sent;
  L7_uint32 specific_queries_received;
} ptin_IGMP_Statistics_t;

/* More debug for IGMP */
extern L7_BOOL ptin_debug_igmp_snooping;

/**
 * Initializes IGMP module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_init(void);

/**
 * Deinitializes IGMP module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_deinit(void);

/**
 * Load IGMP proxy default configuraion parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_defaultcfg_load(void);

/**
 * Applies IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_config_set(ptin_IgmpProxyCfg_t *igmpProxy);

/**
 * Gets IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_config_get(ptin_IgmpProxyCfg_t *igmpProxy);

/**
 * Update snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evcId     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_BOOL : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_snooping_trap_interface_update(L7_uint16 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable);

/**
 * Check if a EVC is being used in an IGMP instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
L7_RC_t ptin_igmp_is_evc_used(L7_uint16 evcId);

/**
 * Creates an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instance_add(L7_uint16 McastEvcId, L7_uint16 UcastEvcId);

/**
 * Removes an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instance_remove(L7_uint16 McastEvcId, L7_uint16 UcastEvcId);

/**
 * Reactivate all IGMP instances
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instances_reactivate(void);

/**
 * Update IGMP entries, when EVCs are deleted
 * 
 * @param evcId : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instance_destroy(L7_uint16 evcId);

/**
 * Add a new Multicast client
 * 
 * @param McastEvcId  : Multicast evc id
 * @param client      : client identification parameters 
 * @param isDynamic   : client type 
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_client_add(L7_uint16 McastEvcId, ptin_client_id_t *client);

/**
 * Remove a Multicast client
 * 
 * @param McastEvcId  : Multicast evc id
 * @param client      : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_client_delete(L7_uint16 McastEvcId, ptin_client_id_t *client);

/**
 * Remove all Multicast clients 
 * 
 * @param McastEvcId  : Multicast evc id
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_all_clients_flush(L7_uint16 McastEvcId);

/**
 * Get list of channels, starting from a specific channel index
 * 
 * @param McastEvcId         : (in) Multicast EVC id
 * @param client             : (in) Client information
 * @param channel_index      : (in) First channel index
 * @param number_of_channels : (in) Maximum number of channels 
 *                             (out) Number of channels 
 * @param channel_list       : (out) Channels array 
 * @param total_channels     : (out) Total number of channels
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_channelList_get(L7_uint16 McastEvcId, ptin_client_id_t *client,
                                         L7_uint16 channel_index, L7_uint16 *number_of_channels, L7_in_addr_t *channel_list,
                                         L7_uint16 *total_channels);

/**
 * Get list of clients, watching a specific channel 
 * 
 * @param McastEvcId         : (in) Multicast EVC id
 * @param channel            : (in) IP Channel
 * @param client_index       : (in) First client index
 * @param number_of_clients  : (in) Maximum number of clients 
 *                             (out) Number of clients 
 * @param client_list        : (out) Clients array 
 * @param total_clients      : (out) Total number of clients
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientList_get(L7_uint16 McastEvcId, L7_in_addr_t *ipv4_channel,
                                        L7_uint16 client_index, L7_uint16 *number_of_clients, ptin_client_id_t *client_list,
                                        L7_uint16 *total_clients);

/**
 * Add a new static channel
 * 
 * @param McastEvcId   : Multicast EVC id
 * @param ipv4_channel : Channel IP
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_static_channel_add(L7_uint16 McastEvcId, L7_in_addr_t *ipv4_channel);

/**
 * Remove an existent channel
 * 
 * @param McastEvcId   : Multicast EVC id
 * @param ipv4_channel : Channel IP
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_channel_remove(L7_uint16 McastEvcId, L7_in_addr_t *ipv4_channel);


/**
 * Get global IGMP statistics
 * 
 * @param intIfNum    : interface
 * @param stat_port_g : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_intf_get(ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stat_port_g);

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * interface 
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param intIfNum    : interface
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instanceIntf_get(L7_uint16 McastEvcId, ptin_intf_t *ptin_intf, ptin_IGMP_Statistics_t *stat_port);

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param client      : client reference
 * @param stat_port   : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_client_get(L7_uint16 McastEvcId, ptin_client_id_t *client, ptin_IGMP_Statistics_t *stat_client);

/**
 * Clear all IGMP statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_clearAll(void);

/**
 * Clear all statistics of one IGMP instance
 * 
 * @param McastEvcId : Multicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_instance_clear(L7_uint16 McastEvcId);

/**
 * Clear interface IGMP statistics
 * 
 * @param intIfNum    : interface 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_intf_clear(ptin_intf_t *ptin_intf);

/**
 * Clear statistics of a particular IGMP instance and interface
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_instanceIntf_clear(L7_uint16 McastEvcId, ptin_intf_t *ptin_intf);

/**
 * Clear IGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param McastEvcId  : Multicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_client_clear(L7_uint16 McastEvcId, ptin_client_id_t *client);

/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Get the client index associated to a Multicast client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param client        : Client information parameters
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientIndex_get(L7_uint32 intIfNum, L7_uint16 intVlan,
                                         ptin_client_id_t *client,
                                         L7_uint *client_index);

/**
 * Get client type from its index. 
 * 
 * @param intIfNum      : interface number 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * @param isDynamic     : client type (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_client_type(L7_uint16 intVlan,
                                     L7_uint client_idx,
                                     L7_BOOL *isDynamic);

/**
 * (Re)start the timer for this client
 *  
 * @param intVlan     : Internal vlan
 * @param client_idx  : client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_timer_start(L7_uint16 intVlan,
                                     L7_uint32 client_idx);

/**
 * Add a dynamic client
 * 
 * @param intVlan     : Internal vlan
 * @param client      : client identification parameters 
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_dynamic_client_add(L7_uint32 intIfNum,
                                            L7_uint16 intVlan,
                                            ptin_client_id_t *client,
                                            L7_uint *client_idx_ret);

/**
 * Remove a particular client
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_dynamic_client_flush(L7_uint16 intVlan,
                                              L7_uint client_idx);

/**
 * Remove all dynamic client
 * 
 * @param intVlan       : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_dynamic_all_clients_flush(L7_uint16 intVlan);

/**
 * Get client information from its index. 
 * 
 * @param intVlan       : internal vlan
 * @param client_index  : Client index
 * @param client        : Client information (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientData_get(L7_uint16 intVlan,
                                        L7_uint client_idx,
                                        ptin_client_id_t *client);

/**
 * Validate igmp packet checking if the input intIfNum and 
 * internal Vlan are valid
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
extern L7_RC_t ptin_igmp_intfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan);

/**
 * Validate igmp packet checking if the input internal Vlan are 
 * valid 
 * 
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
extern L7_RC_t ptin_igmp_vlan_validate(L7_uint16 intVlan);

/**
 * Check if an internal vlan is associated to an unstacked 
 * Unicast EVC 
 * 
 * @param intVlan : internal vlan
 * @param is_unstacked : is unstacked? (output)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_igmp_vlan_UC_is_unstacked(L7_uint16 intVlan, L7_BOOL *is_unstacked);

/**
 * Validate igmp packet checking if the input intIfNum is a root
 * interface and internal Vlan is valid 
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
extern L7_RC_t ptin_igmp_rootIntfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan);

/**
 * Validate igmp packet checking if the input intIfNum is a 
 * client (leaf) interface and internal Vlan is valid 
 * 
 * @param intIfNum : source interface number
 * @param intVlan  : internal vlan
 * 
 * @return L7_RC_t : L7_SUCCESS: Parameters are valid
 *                   L7_FAILURE: Not valid
 */
extern L7_RC_t ptin_igmp_clientIntfVlan_validate(L7_uint32 intIfNum, L7_uint16 intVlan);

/**
 * Get the MC root vlan associated to the internal vlan
 * 
 * @param intVlan       : internal vlan
 * @param McastRootVlan : multicast root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_McastRootVlan_get(L7_uint16 intVlan, L7_uint16 *McastRootVlan);

/**
 * Get the list of root interfaces associated to a internal vlan
 * 
 * @param intVlan  : Internal vlan
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_rootIntfs_getList(L7_uint16 intVlan, L7_INTF_MASK_t *intfList);

/**
 * Get the list of client (leaf) interfaces associated to a 
 * internal vlan 
 * 
 * @param intVlan  : Internal vlan
 * @param intfList : List of interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientIntfs_getList(L7_uint16 intVlan, L7_INTF_MASK_t *intfList);

/**
 * Get the external outer+inner vlan asociated to the MC EVC
 * 
 * @param intIfNum     : interface number
 * @param intOVlan     : Internal outer vlan 
 * @param intIVlan     : Internal inner vlan 
 * @param extMcastVlan : external vlan associated to MC EVC
 * @param extIVlan     : ext. innerVlan associated to MC EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_extMcastVlan_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extMcastVlan, L7_uint16 *extIVlan);

/**
 * Get the external outer+inner vlan asociated to the UC EVC
 * 
 * @param intIfNum     : interface number
 * @param intOVlan     : Internal outer vlan 
 * @param intIVlan     : Internal inner vlan 
 * @param extMcastVlan : external vlan associated to MC EVC
 * @param extIVlan     : ext. innerVlan associated to MC EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_extUcastVlan_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan, L7_uint16 *extUcastVlan, L7_uint16 *extIVlan);

/**
 * Increment IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_increment_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field);

/**
 * Decrement IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to increment
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_decrement_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field);

#endif


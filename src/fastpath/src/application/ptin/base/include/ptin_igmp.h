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
#include "l3_addrdefs.h"

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
#define PTIN_IGMP_DEFAULT_VERSION                       2
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

#define PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL         0.5

#define PTIN_IGMP_MAX_UNSOLICITEDREPORTINTERVAL         5



#define PTIN_IGMP_DEFAULT_OLDERQUERIERPRESENTTIMEOUT    PTIN_IGMP_AUTO_OQPT(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */
#define PTIN_IGMP_DEFAULT_OLDERHOSTPRESENTTIMEOUT       PTIN_IGMP_AUTO_OHPT(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */

#define PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD        64


/* FOR STATISTICS */
// The values below must be in the same order as in L7_IGMP_Statistics_t structure
typedef enum  {
/*Global Counters*/
  SNOOP_STAT_FIELD_ACTIVE_GROUPS=0,
  SNOOP_STAT_FIELD_ACTIVE_CLIENTS,
  SNOOP_STAT_FIELD_IGMP_SENT,
  SNOOP_STAT_FIELD_IGMP_TX_FAILED,
  SNOOP_STAT_FIELD_IGMP_INTERCEPTED,
  SNOOP_STAT_FIELD_IGMP_DROPPED,
  SNOOP_STAT_FIELD_IGMP_RECEIVED_VALID,
  SNOOP_STAT_FIELD_IGMP_RECEIVED_INVALID,

  /*IGMP & MLD Queries */
  SNOOP_STAT_FIELD_GENERIC_QUERY_TX,  
  SNOOP_STAT_FIELD_GENERIC_QUERY_TOTAL_RX,
  SNOOP_STAT_FIELD_GENERIC_QUERY_VALID_RX,
  SNOOP_STAT_FIELD_GENERIC_QUERY_INVALID_RX,
  SNOOP_STAT_FIELD_GENERIC_QUERY_DROPPED_RX,

  SNOOP_STAT_FIELD_GENERAL_QUERY_TX,  
  SNOOP_STAT_FIELD_GENERAL_QUERY_TOTAL_RX,
  SNOOP_STAT_FIELD_GENERAL_QUERY_VALID_RX,
  SNOOP_STAT_FIELD_GENERAL_QUERY_DROPPED_RX,

  SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TX,
  SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_TOTAL_RX,    
  SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_VALID_RX,    
  SNOOP_STAT_FIELD_GROUP_SPECIFIC_QUERY_DROPPED_RX,    

  SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TX,
  SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_VALID_RX,    
  SNOOP_STAT_FIELD_GROUP_AND_SOURCE_SPECIFIC_QUERY_DROPPED_RX,

/*IGMPv2 & MLDv1 only*/
  SNOOP_STAT_FIELD_JOINS_SENT,
  SNOOP_STAT_FIELD_JOINS_RECEIVED_SUCCESS,
  SNOOP_STAT_FIELD_JOINS_RECEIVED_FAILED,
  SNOOP_STAT_FIELD_LEAVES_SENT,
  SNOOP_STAT_FIELD_LEAVES_RECEIVED,
/*IGMPv3 & MLDv2 only*/
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TX,
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_TOTAL_RX,  
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_VALID_RX,  
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_INVALID_RX, 
  SNOOP_STAT_FIELD_MEMBERSHIP_REPORT_DROPPED_RX,    
  
/*Group Records*/
  SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TX,
  SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_VALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_INVALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_ALLOW_NEW_SOURCES_DROPPED_RX,

  SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TX,
  SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_VALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_INVALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_BLOCK_OLD_SOURCES_DROPPED_RX,

  SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_VALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_INVALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_INCLUDE_DROPPED_RX,

  SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_VALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_INVALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_IS_EXCLUDE_DROPPED_RX,

  SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_VALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_INVALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_INCLUDE_DROPPED_RX,

  SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_TOTAL_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_VALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_INVALID_RX,
  SNOOP_STAT_FIELD_GROUP_RECORD_TO_EXCLUDE_DROPPED_RX,
/*This must be the last element*/
  SNOOP_STAT_FIELD_ALL                        
} ptin_snoop_stat_enum_t;



/*Definitions of the IGMP & MLD statictics 
rx - received packet 
tx - transmited packet 
valid - no problems ocurred during the procssing of the packet 
invalid - the packet received is invalid (e.g. invalid message type) 
dropped - the packet is valid, altought it was dropped due to internal error processing (e.g. maximum number of multicast groups exceed)
total - is equal to the sum of valid+invalid+dropped
*/

typedef struct
{
//Since we have three different types of queries using the same message type=0x11,
// we need to have an extra field if the packets are invalid i.e. query.  

//Generic Query
  L7_uint32 generic_query_tx; //query_tx=general_query_tx+group_query_tx+source_query_tx
  L7_uint32 generic_query_total_rx; //query_total_rx=query_valid_rx+query_invalid_rx+query_dropped_rx
  L7_uint32 generic_query_valid_rx; //general_query_valid_rx=general_query_valid_rx+group_query_valid_rx+source_query_valid_rx
  L7_uint32 generic_query_invalid_rx; 
  L7_uint32 generic_query_dropped_rx; //query_dropped_rx=general_query_dropped_rx+group_query_dropped_rx+source_query_dropped_rx

//General Query
  L7_uint32 general_query_tx;
  L7_uint32 general_query_total_rx; //total_rx=valid_rx+dropped_rx
  L7_uint32 general_query_valid_rx;
  L7_uint32 general_query_dropped_rx;
//Group Specific Query
  L7_uint32 group_query_tx;  
  L7_uint32 group_query_total_rx;//total_rx=valid_rx+dropped_rx
  L7_uint32 group_query_valid_rx;
  L7_uint32 group_query_dropped_rx;
//Group and Source Specific Query
  L7_uint32 source_query_tx;
  L7_uint32 source_query_total_rx;//total_rx=valid_rx+dropped_rx
  L7_uint32 source_query_valid_rx;
  L7_uint32 source_query_dropped_rx;

} ptin_Query_Statistics_t;

typedef struct
{  
/*New fields*/
  L7_uint32 join_tx;
  L7_uint32 join_total_rx;//total=valid+invalid+dropped
  L7_uint32 join_valid_rx;
  L7_uint32 join_invalid_rx;
  L7_uint32 join_dropped_rx;

  L7_uint32 leave_tx;
  L7_uint32 leave_total_rx;
  L7_uint32 leave_valid_rx;
  L7_uint32 leave_invalid_rx;
  L7_uint32 leave_dropped_rx;

  /*Here for historical reasons*/
//L7_uint32 joins_sent;
//L7_uint32 joins_received_success;
//L7_uint32 joins_received_failed;
//L7_uint32 leaves_sent;
//L7_uint32 leaves_received;
} ptin_IGMPv2_Statistics_t;

typedef struct
{
//IGMPv3
//Group Record
  L7_uint32 allow_tx;
  L7_uint32 allow_total_rx;
  L7_uint32 allow_valid_rx;
  L7_uint32 allow_invalid_rx;
  L7_uint32 allow_dropped_rx;

  L7_uint32 block_tx;
  L7_uint32 block_total_rx;
  L7_uint32 block_valid_rx;
  L7_uint32 block_invalid_rx;
  L7_uint32 block_dropped_rx;

  L7_uint32 is_include_tx;
  L7_uint32 is_include_total_rx;
  L7_uint32 is_include_valid_rx;  
  L7_uint32 is_include_invalid_rx;  
  L7_uint32 is_include_dropped_rx;  

  L7_uint32 is_exclude_tx;
  L7_uint32 is_exclude_total_rx;  
  L7_uint32 is_exclude_valid_rx;
  L7_uint32 is_exclude_invalid_rx;
  L7_uint32 is_exclude_dropped_rx;

  L7_uint32 to_include_tx;
  L7_uint32 to_include_total_rx;
  L7_uint32 to_include_valid_rx;
  L7_uint32 to_include_invalid_rx;
  L7_uint32 to_include_dropped_rx;

  L7_uint32 to_exclude_tx;
  L7_uint32 to_exclude_total_rx;
  L7_uint32 to_exclude_valid_rx;
  L7_uint32 to_exclude_invalid_rx;
  L7_uint32 to_exclude_dropped_rx;

} ptin_Group_Record_Statistics_t;

typedef struct
{
  L7_uint32                       membership_report_tx;
  L7_uint32                       membership_report_total_rx; 
  L7_uint32                       membership_report_valid_rx;  
  L7_uint32                       membership_report_invalid_rx; 
  L7_uint32                       membership_report_dropped_rx;

  ptin_Group_Record_Statistics_t  group_record;
} ptin_IGMPv3_Statistics_t;

typedef struct
{
  /*Here for historical reasons*/
  L7_uint32 active_groups;
  L7_uint32 active_clients;
 
  /*Global Counters*/
  L7_uint32 igmp_total_tx;  /*This counter is equal to the sum of all IGMP packets received (valid+invalid+dropped*/  
  L7_uint32 igmp_total_rx;  /*This counter is equal to the sum of all IGMP packets received (valid+invalid+dropped*/  
  L7_uint32 igmp_valid_rx;/*This counter is equal to the sum of all valid IGMP packets received*/
  L7_uint32 igmp_invalid_rx;/*This counter is equal to the sum of all invalid IGMP packets received, e.g. invalid message type*/
  L7_uint32 igmp_dropped_rx; /*This counter is equal to the sum of all IGMP packets dropped, due to an internal error processing*/
  
  ptin_IGMPv2_Statistics_t  igmpv2;/*Variable respecting IGMPv2*/
  ptin_IGMPv3_Statistics_t  igmpv3;/*Variable respecting IGMPv3*/  

  ptin_Query_Statistics_t   igmpquery;/*Variable respecting Query*/

////MLD Suppport
//L7_uint32 mld_total_rx;  /*This counter is equal to the sum of all IGMP packets received (valid+invalid+dropped*/
//L7_uint32 mld_rx_valid;/*This counter is equal to the sum of all valid IGMP packets received*/
//L7_uint32 mld_rx_invalid;/*This counter is equal to the sum of all invalid IGMP packets received, e.g. invalid message type*/
//L7_uint32 mld_rx_dropped; /*This counter is equal to the sum of all IGMP packets dropped, due to an internal error processing*/
//ptin_MLDv1_Statistics_t   mldv1;
//ptin_MLDv2_Statistics_t   mldv2;
//ptin_Query_Statistics_t   mldquery;/*Variable respecting Query*/  

} ptin_MGMD_Statistics_t;


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
/*IGMPv2 only*/
  L7_uint32 joins_sent;
  L7_uint32 joins_received_success;
  L7_uint32 joins_received_failed;
  L7_uint32 leaves_sent;
  L7_uint32 leaves_received;
/*IGMPv3 only*/
  L7_uint32 membership_report_v3;

/*New Fields*/
  ptin_IGMPv3_Statistics_t  igmpv3;/*Variable respecting IGMPv3*/  
  ptin_Query_Statistics_t   igmpquery;/*Variable respecting Query*/
/*End New Fields*/

/*IGMPv2 Queries */
  L7_uint32 general_queries_sent;
  L7_uint32 general_queries_received;
  L7_uint32 specific_queries_sent;
  L7_uint32 specific_queries_received;
} ptin_IGMP_Statistics_t;/*To be replaced with ptin_MGMD_Statistics_t in a short future

/* More debug for IGMP */
extern L7_BOOL ptin_debug_igmp_snooping;

typedef enum
{
  PTIN_DIR_NONE=0,
  PTIN_DIR_UPLINK,
  PTIN_DIR_DOWNLINK,
  PTIN_DIR_BOTH,
} ptin_dir_t;

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
 * Configure an IGMP evc with the necessary procedures 
 * 
 * @param evc_idx   : evc index
 * @param enable    : enable flag 
 * @param direction : Ports to be considered (PTIN_DIR_UPLINK, 
 *                    PTIN_DIR_DOWNLINK, PTIN_DIR_BOTH).
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_igmp_evc_configure(L7_uint16 evc_idx, L7_BOOL enable, ptin_dir_t direction);

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
extern L7_RC_t ptin_igmp_snooping_trap_interface_update(L7_uint16 evcId, ptin_intf_t *ptin_intf, L7_BOOL enable);

/**
 * Check if a EVC is being used in an IGMP instance
 * 
 * @param evcId : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
extern L7_RC_t ptin_igmp_is_evc_used(L7_uint16 evcId);

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
                                         L7_uint16 channel_index, L7_uint16 *number_of_channels, ptin_igmpClientInfo_t *channel_list,
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


#define IGMPASSOC_CHANNELS_MAX  L7_MAX_GROUP_REGISTRATION_ENTRIES

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

typedef struct
{
  L7_uint16 evc_uc;
  L7_uint16 evc_mc;
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  L7_BOOL   is_static;
} igmpAssoc_entry_t;

/**
 * Clear all IGMP associations
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_init( void );

/**
 * Get the association of a particular dst/src channel using 
 * vlans. 
 * 
 * @param vlan_uc : UC vlan
 * @param channel_group   : Group address
 * @param channel_source  : Source address
 * @param vlan_mc : MC vlan pair (out)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_vlanPair_get( L7_uint16 vlan_uc,
                                        L7_inet_addr_t *channel_group,
                                        L7_inet_addr_t *channel_source,
                                        L7_uint16 *vlan_mc );

/**
 * Get the the list of channels of a UC+MC association
 * 
 * @param evc_uc : UC EVC index (0 to list all)
 * @param evc_mc : MC EVC pair  (0 to list all)
 * @param channel_group   : Array of group channels - max of 
 *                          *channels_number (output)
 * @param channel_source  : Array of source channels - max of
 *                          *channels_number (output)
 * @param channels_number : In - Max #channels to get 
 *                          Out - Effective #channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_channelList_get( L7_uint16 evc_uc, L7_uint16 evc_mc,
                                           igmpAssoc_entry_t *channel_list,
                                           L7_uint16 *channels_number );

/**
 * Add a new association to a MC service, applied only to a 
 * specific dst/src channel.
 * 
 * @param evc_uc : UC EVC index
 * @param evc_mc : MC EVC index
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * @param channel_source  : Source channel
 * @param channel_srcMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_channel_add( L7_uint16 evc_uc, L7_uint16 evc_mc,
                                       L7_inet_addr_t *channel_group, L7_uint16 channel_grpMask,
                                       L7_inet_addr_t *channel_source, L7_uint16 channel_srcMask,
                                       L7_BOOL is_static );

/**
 * Remove an association to a MC service, applied only to a 
 * specific dst/src channel. 
 * 
 * @param evc_uc : UC EVC index
 * @param channel_group   : Group channel
 * @param channel_grpMask : Number of masked bits
 * @param channel_source  : Source channel
 * @param channel_srcMask : Number of masked bits
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_channel_remove( L7_uint16 evc_uc,
                                          L7_inet_addr_t *channel_group, L7_uint16 channel_grpMask,
                                          L7_inet_addr_t *channel_source, L7_uint16 channel_srcMask);

/**
 * Remove all associations of a MC instance
 * 
 * @param evc_uc : UC EVC index 
 * @param evc_uc : MC EVC index 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_channel_clear( L7_uint16 evc_uc, L7_uint16 evc_mc );
#endif

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

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
/**
 * Get the MC root vlan associated to the internal vlan
 *  
 * @param groupChannel  : Channel Group address 
 * @param sourceChannel : Channel Source address 
 * @param intVlan       : internal vlan
 * @param McastRootVlan : multicast root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_McastRootVlan_get(L7_inet_addr_t *groupChannel, L7_inet_addr_t *sourceChannel,
                                           L7_uint16 intVlan, L7_uint16 *McastRootVlan);
#else
/**
 * Get the MC root vlan associated to the internal vlan
 * 
 * @param intVlan       : internal vlan
 * @param McastRootVlan : multicast root vlan
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_McastRootVlan_get(L7_uint16 intVlan, L7_uint16 *McastRootVlan);
#endif

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

#if (!defined IGMPASSOC_MULTI_MC_SUPPORTED)
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
#endif

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


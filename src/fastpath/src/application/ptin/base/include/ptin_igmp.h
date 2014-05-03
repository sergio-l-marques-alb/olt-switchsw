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
#include "ptin_mgmd_ctrl.h"

#define PTIN_IGMP_STATS_IN_EVCS 0

#define PTIN_IGMP_VERSION_1 1
#define PTIN_IGMP_VERSION_2 2
#define PTIN_IGMP_VERSION_3 3

#define PTIN_IGMP_MAX_ROOT_PORTS 16

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
#define PTIN_MAX_ROBUSTNESS_VARIABLE 9 /*This value must serve two purposes: RV configured on the Management and the RV that came from the network*/
#define PTIN_MIN_ROBUSTNESS_VARIABLE 1 /*This value must serve two purposes: RV configured on the Management and the RV that came from the network*/
#define PTIN_IGMP_DEFAULT_ROBUSTNESS 2


#define PTIN_IGMP_DEFAULT_QUERYINTERVAL                 125 /* (s) */

#define PTIN_IGMP_MIN_QUERYINTERVAL                     10 /* (s) */
#define PTIN_IGMPv2_MAX_QUERYINTERVAL                   255 /* (s) */
#define PTIN_IGMPv3_MAX_QUERYINTERVAL                   31744 /* (s) */

#define PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL         100 /* (1/10s - 10s) (ds)*/

#define PTIN_IGMP_MIN_QUERYRESPONSEINTERVAL             10 /* (ds) */
#define PTIN_IGMPv2_MAX_QUERYRESPONSEINTERVAL           255 /* (ds) */
#define PTIN_IGMPv3_MAX_QUERYRESPONSEINTERVAL           31744 /* (ds) */


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

#define PTIN_IGMP_MIN_UNSOLICITEDREPORTINTERVAL         0.5 /*(s)*/

#define PTIN_IGMP_MAX_UNSOLICITEDREPORTINTERVAL         5 /*(s)*/



#define PTIN_IGMP_DEFAULT_OLDERQUERIERPRESENTTIMEOUT    PTIN_IGMP_AUTO_OQPT(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */
#define PTIN_IGMP_DEFAULT_OLDERHOSTPRESENTTIMEOUT       PTIN_IGMP_AUTO_OHPT(PTIN_IGMP_DEFAULT_ROBUSTNESS,\
                                                                            PTIN_IGMP_DEFAULT_QUERYINTERVAL,\
                                                                            PTIN_IGMP_DEFAULT_QUERYRESPONSEINTERVAL) /* (260 s) */

#define PTIN_IGMP_DEFAULT_MAX_SOURCES_PER_GROUP_RECORD        PTIN_SYSTEM_IGMP_MAXSOURCES_PER_GROUP //64

#define PTIN_IGMP_DEFAULT_MAX_RECORDS_PER_REPORT              64

#define PTIN_IGMP_MAX_RECORDS_PER_REPORT                      64

#define PTIN_IGMP_MIN_RECORDS_PER_REPORT                      1

#define PTIN_IGMP_MIN_VLAN_ID                                 1
#define PTIN_IGMP_MAX_VLAN_ID                                 4095



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

typedef enum
{
  SNOOP_STAT_FIELD_TX=0,
  SNOOP_STAT_FIELD_TOTAL_RX,
  SNOOP_STAT_FIELD_VALID_RX,
  SNOOP_STAT_FIELD_INVALID_RX,
  SNOOP_STAT_FIELD_DROPPED_RX,
  SNOOP_STAT_FIELD_TYPE_ALL
} ptin_snoop_statistics_t;

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
  L7_uint32 igmp_tx;  /*This counter is equal to the sum of all IGMP packets received (valid+invalid+dropped*/  
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
//  ptin_IGMPv2_Statistics_t  igmpv2;/*Variable respecting IGMPv2*/  
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

/* Packet trace for IGMP */
extern L7_BOOL ptin_debug_igmp_packet_trace;

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
 * Applies IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_config_set__snooping_old(ptin_IgmpProxyCfg_t *igmpProxy);
extern L7_RC_t ptin_igmp_proxy_config_set(PTIN_MGMD_CTRL_MGMD_CONFIG_t *igmpProxy);

/**
 * Gets IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_config_get__snooping_old(ptin_IgmpProxyCfg_t *igmpProxy);
extern L7_RC_t ptin_igmp_proxy_config_get(PTIN_MGMD_CTRL_MGMD_CONFIG_t *igmpProxy);

/**
 * Configure an IGMP evc with the necessary procedures 
 * 
 * @param evc_idx   : evc index
 * @param enable    : enable flag 
 * @param set_trap  : configure trap rule?  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_igmp_evc_configure(L7_uint32 evc_idx, L7_BOOL enable, L7_BOOL set_trap);

/**
 * Update snooping configuration, when interfaces are 
 * added/removed 
 * 
 * @param evc_idx     : EVC id 
 * @param ptin_intf : interface 
 * @param enable    : L7_TRUE when interface is added 
 *                    L7_FALSE when interface is removed
 *  
 * @return L7_BOOL : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_snooping_trap_interface_update(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, L7_BOOL enable);

/**
 * Reset Proxy machine
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t ptin_igmp_proxy_reset(void);

/**
 * Check if a EVC is being used in an IGMP instance
 * 
 * @param evc_idx : evc id
 * 
 * @return L7_RC_t : L7_TRUE or L7_FALSE
 */
extern L7_RC_t ptin_igmp_is_evc_used(L7_uint32 evc_idx);

/**
 * Creates an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instance_add(L7_uint32 McastEvcId, L7_uint32 UcastEvcId);

/**
 * Removes an IGMP instance
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instance_remove(L7_uint32 McastEvcId, L7_uint32 UcastEvcId);

/**
 * Removes all IGMP instances
 * 
 * @param McastEvcId : Multicast evc id 
 * @param UcastEvcId : Unicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clean_all(void);

/**
 * Reset MGMD
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_mgmd_resetdefaults(void);

/**
 * Reactivate all IGMP instances
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instances_reactivate(void);

/**
 * Update IGMP entries, when EVCs are deleted
 * 
 * @param evc_idx : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_instance_destroy(L7_uint32 evc_idx);

/**
 * Associate an EVC to an IGMP instance
 * 
 * @param evc_idx : Multicast evc id 
 * @param nni_ovlan  : Network outer vlan (used to aggregate 
 *                   evcs in one instance: 0 to not be used)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_evc_add(L7_uint32 evc_idx, L7_uint16 nni_ovlan);

/**
 * Deassociate an EVC from an IGMP instance
 * 
 * @param evc_idx : Multicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_evc_remove(L7_uint32 evc_idx);

/**
 * Reactivate all IGMP instances
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_evcs_reactivate(void);

/**
 * Remove an EVC from a IGMP instance
 * 
 * @param evc_idx : evc index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_evc_destroy(L7_uint32 evc_idx);

/**
 * Add a new Multicast client
 * 
 * @param evc_idx     : evc id
 * @param client      : client identification parameters 
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_client_add(L7_uint32 evc_idx, ptin_client_id_t *client, L7_uint16 uni_ovid, L7_uint16 uni_ivid);

/**
 * Remove a Multicast client
 * 
 * @param evc_idx : evc id
 * @param client  : client identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_client_delete(L7_uint32 evc_idx, ptin_client_id_t *client);

/**
 * Remove all Multicast clients 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_all_clients_flush(void);

/**
 * Reset all MGMD General Queriers 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_generalquerier_reset(void);

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
extern L7_RC_t ptin_igmp_channelList_get(L7_uint32 McastEvcId, ptin_client_id_t *client,
                                         L7_uint16 channel_index, L7_uint16 *number_of_channels, ptin_igmpChannelInfo_t *channel_list,
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
 * @param extendedEvcId      : (out) Extended EVC Id
 * @param total_clients      : (out) Total number of clients
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientList_get(L7_uint32 McastEvcId, L7_in_addr_t *ipv4_channel,
                                 L7_uint16 client_index, L7_uint16 *number_of_clients, ptin_client_id_t *client_list,L7_uint32 *extendedEvcId, 
                                 L7_uint16 *total_clients);

/**
 * Add a new static channel
 * 
 * @param channel : Static Channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_static_channel_add(PTIN_MGMD_CTRL_STATICGROUP_t* channel);

/**
 * Remove an existent channel
 * 
 * @param channel : Static Channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_channel_remove(PTIN_MGMD_CTRL_STATICGROUP_t* channel);


#define IGMPASSOC_CHANNELS_MAX    4096

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

typedef struct
{
  L7_uint32 evc_uc;
  L7_uint32 evc_mc;
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
extern L7_RC_t igmp_assoc_channelList_get( L7_uint32 evc_uc, L7_uint32 evc_mc,
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
extern L7_RC_t igmp_assoc_channel_add( L7_uint32 evc_uc, L7_uint32 evc_mc,
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
extern L7_RC_t igmp_assoc_channel_remove( L7_uint32 evc_uc,
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
extern L7_RC_t igmp_assoc_channel_clear( L7_uint32 evc_uc, L7_uint32 evc_mc );

/**
 * Remove all associations
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern L7_RC_t igmp_assoc_clean_all(void);
#endif

/******************************************************** 
 * FOR FASTPATH INTERNAL MODULES USAGE
 ********************************************************/

/**
 * Get external vlans
 * 
 * @param intIfNum 
 * @param intOVlan 
 * @param intIVlan 
 * @param client_idx 
 * @param uni_ovid : External Outer Vlan
 * @param uni_ivid : External Inner Vlan
 * 
 * @return L7_RC_t 
 */
extern L7_RC_t ptin_igmp_extVlans_get(L7_uint32 intIfNum, L7_uint16 intOVlan, L7_uint16 intIVlan,
                                      L7_int client_idx, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid);

#if 0
/**
 * Get next IGMP client
 * 
 * @param intIfNum : interface
 * @param intVlan  : internal vlan
 * @param inner_vlan : inner vlan used as base reference 
 *                   (processed and returned next inner vlan)
 * @param uni_ovid : external outer vid
 * @param uni_ivid : external inner vid
 * 
 * @return L7_RC_t : 
 *  L7_SUCCESS tells a next client was returned
 *  L7_NO_VALUE tells there is no more clients
 *  L7_FAILURE in case of error
 */
extern L7_RC_t ptin_igmp_client_next(L7_uint32 intIfNum, L7_uint16 intVlan, L7_uint16 inner_vlan,
                                     L7_uint16 *inner_vlan_next, L7_uint16 *uni_ovid, L7_uint16 *uni_ivid);
#endif
/**
 * Get the client index associated to a Multicast client 
 * 
 * @param intIfNum      : interface number
 * @param intVlan       : internal vlan
 * @param innerVlan     : inner vlan
 * @param client_index  : Client index to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientIndex_get(L7_uint32 intIfNum,
                                         L7_uint16 intVlan, L7_uint16 innerVlan,
                                         L7_uchar8 *smac,
                                         L7_uint *client_index);

/**
 * Get client type from its index. 
 * 
 * @param intIfNum      : interface number 
 * @param client_index  : Client index
 * @param isDynamic     : client type (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_client_type(L7_uint32 intIfNum,
                                     L7_uint client_idx,
                                     L7_BOOL *isDynamic);

/**
 * (Re)start the timer for this client
 *  
 * @param intIfNum    : Interface Number
 * @param client_idx  : client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_client_timer_start(L7_uint32 intIfNum,
                                     L7_uint32 client_idx);

/**
 * Add a new Multicast client group
 * 
 * @param client      : client group identification parameters 
 * @param intVid      : Internal vlan
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientGroup_add(ptin_client_id_t *client, L7_uint16 uni_ovid, L7_uint16 uni_ivid);

/**
 * Add a new Multicast client group
 * 
 * @param client      : client group identification parameters 
 * @param intVid      : Internal vlan
 * @param uni_ovid    : External Outer vlan 
 * @param uni_ivid    : External Inner vlan 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientGroupSnapshot_add(ptin_client_id_t *client);

/**
 * Remove a Multicast client group
 * 
 * @param client      : client group identification parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientGroup_remove(ptin_client_id_t *client);

/**
 * Remove all Multicast client groups
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientGroup_clean(void);

/**
 * Remove all Multicast client groups
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientGroupSnapshot_clean(void);

/**
 * Add a dynamic client
 *  
 * @param intIfNum    : interface number  
 * @param intVlan     : Internal vlan
 * @param innerVlan   : Inner vlan
 * @param client_idx_ret : client index (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_dynamic_client_add(L7_uint32 intIfNum,
                                            L7_uint16 intVlan, L7_uint16 innerVlan,
                                            L7_uchar8 *smac,
                                            L7_uint *client_idx_ret);

/**
 * Remove a particular client
 * 
 * @param intIfNum      : Interface Number
 * @param client_index  : Client index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_dynamic_client_flush(L7_uint32 intIfNum,
                                              L7_uint client_idx);

/**
 * Remove all dynamic client
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_dynamic_all_clients_flush(void);

/**
 * Get client information from its index. 
 * 
 * @param intIfNum      : Interface Number
 * @param client_index  : Client index
 * @param client        : Client information (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_clientData_get(L7_uint32 intIfNum,
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
 * Removes all groups related to this Service ID
 * 
 * @param EvcId : Multicast evc id 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_mgmd_service_remove(L7_uint32 evc_idx);


/**************************** 
 * IGMP statistics
 ****************************/

/**
 * Get global IGMP statistics
 * 
 * @param intIfNum    : interface
 * @param statistics  : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_intf_get(ptin_intf_t *ptin_intf, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics);

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * interface 
 * 
 * @param evc_idx  : Multicast EVC id
 * @param intIfNum    : interface
 * @param statistics  : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_instanceIntf_get(L7_uint32 evc_idx, ptin_intf_t *ptin_intf, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics);

/**
 * GetIGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param evc_idx  : Multicast EVC id
 * @param client      : client reference
 * @param statistics  : statistics (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_client_get(L7_uint32 evc_idx, ptin_client_id_t *client, PTIN_MGMD_CTRL_STATS_RESPONSE_t *statistics);

/**
 * Clear all IGMP statistics
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_clearAll(void);

/**
 * Clear all statistics of one IGMP instance
 * 
 * @param evc_idx : Multicast EVC id
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_instance_clear(L7_uint32 evc_idx);

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
 * @param evc_idx  : Multicast EVC id
 * @param intIfNum    : interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_instanceIntf_clear(L7_uint32 evc_idx, ptin_intf_t *ptin_intf);

/**
 * Clear IGMP statistics of a particular IGMP instance and 
 * client
 * 
 * @param evc_idx  : Multicast EVC id
 * @param client      : client reference
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_igmp_stat_client_clear(L7_uint32 evc_idx, ptin_client_id_t *client);

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


/**
 * Get IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to get
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_stat_get_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field);


/**
 * Reset IGMP statistics
 * 
 * @param intIfNum   : interface where the packet entered
 * @param vlan       : packet's interval vlan
 * @param client_idx : client index
 * @param field      : field to get
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */

L7_RC_t ptin_igmp_stat_reset_field(L7_uint32 intIfNum, L7_uint16 vlan, L7_uint32 client_idx, ptin_snoop_stat_enum_t field);

/**
 * Get MGMD Query Instances 
 *  
 * @return mgmdQueryInstances_t : 
 */
#if (!PTIN_BOARD_IS_MATRIX && (defined (IGMP_QUERIER_IN_UC_EVC)))
extern mgmdQueryInstances_t* ptin_mgmd_query_instances_get(L7_uint32 *mgmdNumberOfQueryInstancesPtr);
#endif

/**
 * Get IGMP Client Bitmap
 *  
 * @param extendedEvcId       : Extended EVC Id
 * @param intIfNum            : intIfNum
 * @param clientBmpPtr        : Client Bitmap Pointer
 * 
 * @return  L7_RC_t           : L7_SUCCESS/L7_FAILURE 
 */
extern L7_RC_t ptin_igmp_clients_bmp_get(L7_uint32 extendedEvcId, L7_uint32 intIfNum,L7_uchar8 *clientBmpPtr, L7_uint32 *noOfClients);

extern L7_RC_t ptin_igmp_groupclients_bmp_get(L7_uint32 intIfNum, L7_uchar8 *clientBmpPtr, L7_uint32 *noOfClients);

/**
 * Open/close ports on the switch for the requested channel 
 *  
 * @param admin      : Admin (L7_ENABLE; L7_DISABLE)
 * @param serviceId  : Service ID
 * @param portId     : Port ID (intfNum)
 * @param groupAddr  : Group IP Address
 * @param sourceAddr : Source IP Address
 * @param groupType  : Dynamic or static port (0-dynamic; 1-static)
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_igmp_mgmd_port_sync(L7_uint8 admin, L7_uint32 serviceId, L7_uint32 portId, L7_uint32 groupAddr, L7_uint32 sourceAddr, L7_uint8 groupType);

#if PTIN_BOARD_IS_MATRIX
extern L7_RC_t ptin_snoop_sync_mx_process_request(L7_uint16 vlanId, L7_uint32 groupAddr);
#else
extern L7_RC_t ptin_snoop_sync_port_process_request(L7_uint16 vlanId, L7_uint32 groupAddr, L7_uint32 portId);
#endif


#endif//_PTIN_IGMP_H


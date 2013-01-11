/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename l3_mcast_defaultconfig.h
*
* @purpose All Factory MCAST default settings are in this file
*
* @component none
*
* @comments Only Factory config files should include this header
*           (ie. nim_config.c, sim_config.c)
*
* @create 03/14/2002
*
* @author vidhumouli H / M Pavan K Chakravarthi
* @end
*
**********************************************************************/

#ifndef INCLUDE_L3_MCAST_CONFIG_DEFAULTS
#define INCLUDE_L3_MCAST_CONFIG_DEFAULTS

/* Start of PIMDM Map Component's Factory Defaults */
/* Start of PIMDM Map Component's Factory Defaults */
/* Start of PIMDM Map Component's Factory Defaults */
/* Start of PIMDM Map Component's Factory Defaults */
/* Start of PIMDM Map Component's Factory Defaults */

/* pimdmCfgData_t default values */
#define FD_PIMDM_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_PIMDM_DEFAULT_TRACE_MODE                     L7_DISABLE

#define FD_PIMDM6_DEFAULT_ADMIN_MODE                    L7_DISABLE
/* pimdmCfgCkt_t default values */
#define FD_PIMDM_INTF_DEFAULT_MODE                      L7_DISABLE
#define FD_PIMDM_INTF_DEFAULT_HELLO_INTERVAL            L7_PIMDM_HELLO_INTERVAL
#define FD_PIMDM_DEFAULT_OVERRIDEINTERVAL    L7_PIMDM_DEFAULT_OVERRIDEINTERVAL
#define FD_PIMDM_DEFAULT_LANPRUNEDELAY       L7_PIMDM_DEFAULT_LANPRUNEDELAY
#define FD_PIMDM_DEFAULT_STATEREFRESHINTERVAL L7_PIMDM_DEFAULT_STATEREFRESHINTERVAL
#define FD_PIMDM_DEFAULT_TRIGGERED_DELAY                5 /*seconds */
#define FD_PIMDM6_INTF_DEFAULT_MODE                     L7_DISABLE
#define FD_PIMDM6_INTF_DEFAULT_HELLO_INTERVAL           L7_PIMDM_HELLO_INTERVAL
/* End of PIMDM Component's Factory Defaults */
/* Start of IGMP Component's Factory Defaults */
/* Start of IGMP Component's Factory Defaults */
/* Start of IGMP Component's Factory Defaults */
/* Start of IGMP Component's Factory Defaults */
/* Start of IGMP Component's Factory Defaults */


#define FD_IGMP_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_IGMP_DEFAULT_ROUTER_ALERT_CHECK             L7_FALSE



/*  IMGP Component Defines */
/* DEVELOPER NOTE:  These need to be less than or equal to values in default_config.h */

/* IGMP PROXY DEFAULT PARAMETERS */
#define FD_IGMP_PROXY_INTF_DEFAULT_ADMIN_MODE          L7_DISABLE
#define FD_IGMP_DEFAULT_UNSOLICITED_REPORT_INTERVAL   1 

/* End of IGMP Component's Factory Defaults */
#define FD_IGMP_INTF_DEFAULT_ADMIN_MODE                L7_DISABLE
#define FD_IGMP_DEFAULT_TRACE_MODE                     L7_DISABLE

#define FD_IGMP_ROBUST_VARIABLE              2
#define FD_IGMP_QUERY_INTERVAL               125
#define FD_IGMP_DEFAULT_VER                 3
#define FD_IGMP_QUERY_RESPONSE_INTERVAL         100
#define FD_IGMP_GROUP_MEMBERSHIP_INTERVAL    260
#define FD_IGMP_OTHER_QUERIER_PRESENT_INTERVAL  255
#define FD_IGMP_STARTUP_QUERY_INTERVAL       31
#define FD_IGMP_STARTUP_QUERY_COUNT          2
#define FD_IGMP_LAST_MEMBER_QUERY_INTERVAL      10
#define FD_IGMP_LAST_MEMBER_QUERY_COUNT         2
#define FD_IGMP_VERSION1_ROUTER_PRESENT_TIMEOUT 400
#define FD_IGMP_ENTRY_STATUS                    L7_ENTRY_FREE

/* MLD Default Values */

#define FD_MLD_DEFAULT_ADMIN_MODE                      L7_DISABLE
#define FD_MLD_DEFAULT_ROUTER_ALERT_CHECK              L7_TRUE
#define FD_MLD_INTF_DEFAULT_ADMIN_MODE                L7_DISABLE
#define FD_MLD_DEFAULT_VERSION              2
#define FD_MLD_QUERY_INTERVAL               125
#define FD_MLD_QUERY_RESPONSE_INTERVAL      10000  /* milli-seconds */
#define FD_MLD_ROBUST_VARIABLE              2
#define FD_MLD_LAST_MEMBER_QUERY_INTERVAL   1000   /* milli-seconds */
#define FD_MLD_LAST_MEMBER_QUERY_COUNT      2

/* MLD PROXY  defines*/
#define FD_MLD_PROXY_DEFAULT_INTF_ADMIN_MODE                     L7_DISABLE
#define FD_MLD_PROXY_DEFAULT_UNSOLICITED_REPORT_INTERVAL   1

/* Start of DVMRP Map Component's Factory Defaults */
/* Start of DVMRP Map Component's Factory Defaults */
/* Start of DVMRP Map Component's Factory Defaults */
/* Start of DVMRP Map Component's Factory Defaults */
/* Start of DVMRP Map Component's Factory Defaults */

/* dvmrpCfgData_t default values */
#define FD_DVMRP_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_DVMRP_DEFAULT_TRACE_MODE                     L7_DISABLE

/* dvmrpCfgCkt_t default values */
#define FD_DVMRP_INTF_DEFAULT_ADMIN_MODE                L7_DISABLE
#define FD_DVMRP_INTF_DEFAULT_METRIC                    1

/* End of DVMRP Component's Factory Defaults */

/* Start of MCAST Component's Factory Defaults */
/* Start of MCAST Component's Factory Defaults */
/* Start of MCAST Component's Factory Defaults */
/* Start of MCAST Component's Factory Defaults */

/* mcastMapCfgData_t default values */
#define FD_MCAST_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_MCAST_DEFAULT_GROUP_IP_ADDRESS               "224.2.0.1"
#define FD_MCAST_DEFAULT_ROUTING_PROTOCOL               L7_MCAST_IANA_MROUTE_UNASSIGNED
#define FD_MCAST_DEFAULT_STATIC_SRC                     0
#define FD_MCAST_DEFAULT_STATIC_SRC_MASK                0
#define FD_MCAST_DEFAULT_STATIC_INTF                    0
#define FD_MCAST_DEFAULT_STATIC_RPFADDR                 0
#define FD_MCAST_DEFAULT_STATIC_METRIC                  0
#define FD_MCAST_DEFAULT_NUM_STATIC_ENTRIES             0
#define FD_MCAST_DEFAULT_NUM_ADMIN_SCOPE_ENTRIES        0
#define FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD             1

#define FD_MCAST_DEFAULT_ADMINSCOPE_INUSE               L7_FALSE
#define FD_MCAST_DEFAULT_ADMINSCOPE_IPADDR              0
#define FD_MCAST_DEFAULT_ADMINSCOPE_MASK                0

#define FD_MCAST_DEFAULT_SOURCE_ADR                     0
#define FD_MCAST_DEFAULT_RECEIVER_ADR                   0
#define FD_MCAST_DEFAULT_GROUP_ADR                      0


/* in seconds - Default multicast Cache Entry life time */ 
#define L7_MCAST_FORWARDING_ENTRY_LIFETIME      L7_MULTICAST_FIB_ENTRY_LIFETIME
/* in seconds - Default time Entry is present in MFC if upcall is not responded */
#define L7_MCAST_FORWARDING_UPCALL_LIFETIME      L7_MULTICAST_FIB_UPCALL_LIFETIME

/* End of MCAST Component's Factory Defaults */

/* Start of PIMSM Map Component's Factory Defaults */
/* Start of PIMSM Map Component's Factory Defaults */
/* Start of PIMSM Map Component's Factory Defaults */
/* Start of PIMSM Map Component's Factory Defaults */
/* Start of PIMSM Map Component's Factory Defaults */

/* pimsmCfgData_t default values */
#define FD_PIMSM_DEFAULT_ADMIN_MODE                     L7_DISABLE
#define FD_PIMSM_DEFAULT_BSR_BORDER                     L7_DISABLE
#define FD_PIMSM_DEFAULT_TRACE_MODE                     L7_DISABLE

#define FD_PIMSM_DEFAULT_JOIN_PRUNE_INTERVAL            L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL
#define FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE            L7_PIMSM_DATA_THRESHOLD_RATE
#define FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE             L7_PIMSM_REG_THRESHOLD_RATE
#define FD_PIMSM_DEFAULT_STATIC_RP_IP_ADDR              0
#define FD_PIMSM_DEFAULT_STATIC_RP_GRP_ADDR             0
#define FD_PIMSM_DEFAULT_STATIC_RP_GRP_MASK             0
#define FD_PIMSM_CBSR_HASH_MASK_LENGTH                  L7_PIMSM_CBSR_HASH_MASK_LENGTH
#define FD_PIMSM_CBSR_PRIORITY                          0
#define FD_PIMSM_SSM_RANGE_GROUP_ADDRESS                      "232.0.0.0"
#define FD_PIMSM_SSM_RANGE_GROUP_MASK                         "255.0.0.0"
#define FD_PIMSM_SSM_MODE                                     L7_DISABLE
#define FD_PIMSM_DEFAULT_INTERFACE_CBSR_SCOPE           5 
#define FD_PIMSM_DEFAULT_BSR_MODE                      L7_DISABLE

/* pimsm6CfgData_t default values */
 
#define FD_PIMSM6_CBSR_HASH_MASK_LENGTH                  L7_PIMSM6_CBSR_HASH_MASK_LENGTH
#define FD_PIMSM6_DEFAULT_INTERFACE_CBSR_HASH_MASK_LENGTH  126
#define FD_PIMSM6_SSM_RANGE_GROUP_ADDRESS                     "FF30::"
#define FD_PIMSM6_SSM_RANGE_PREFIX_LEN                        32
#define FD_PIMSM6_DEFAULT_INTERFACE_CBSR_SCOPE               0

/* pimsmCfgCkt_t default values */
#define FD_PIMSM_DEFAULT_INTERFACE_MODE                   L7_DISABLE
#define FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL         L7_PIMSM_INTERFACE_HELLO_INTERVAL
#define FD_PIMSM_DEFAULT_INTERFACE_JOIN_PRUNE_INTERVAL    L7_PIMSM_INTERFACE_JOIN_PRUNE_INTERVAL
#define FD_PIMSM_DEFAULT_INTERFACE_DR_PRIORITY            L7_PIMSM_INTERFACE_DR_PRIORITY
 
/* End of PIMSM Component's Factory Defaults */

#endif /* INCLUDE_L3_MCAST_CONFIG_DEFAULTS */



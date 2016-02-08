/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename defaultconfig.h
*
* @purpose All Factory default settings are in this file
*
* @component none
*
* @comments Only Factory config files should include this header
*           (ie. nim_config.c, sim_config.c)
*
* @create 08/30/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_L3_CONFIG_DEFAULTS
#define INCLUDE_L3_CONFIG_DEFAULTS

#include <string.h>                /* for memcpy() etc... */
#include <datatypes.h>
#include "l7_product.h"
#include <registry.h>
#include <commdefs.h>
#include <l7_ip_api.h>
#include <l3_commdefs.h>
#include <ipv6_default_cnfgr.h>



/* Start of IP Component's Factory Defaults */
/* Start of IP Component's Factory Defaults */
/* Start of IP Component's Factory Defaults */
/* Start of IP Component's Factory Defaults */
/* Start of IP Component's Factory Defaults */

#define FD_IP_DEFAULT_IP_SPOOFING           L7_DISABLE
#define FD_IP_DEFAULT_IP_SRC_ADDR_CHECKING  L7_DISABLE

/* At this point, this value is informational only. It gets displayed
 * in "show ip brief." Setting the value to match the default used by
 * the IP stacks (both Linux and VxWorks/Interpeak). Our UI doesn't 
 * allow the user to set a different value. */ 
#define FD_IP_DEFAULT_TTL                   64         /* hops    */
#define FD_IP_FRAG_TTL_DFLT                 60

/* 0 means "not configured" */
#define FD_IP_DEFAULT_IP_MTU                0
#define FD_IP_DEFAULT_BW                       0

/* ARP default values */
#define FD_IP_DEFAULT_ARP_AGE_TIME          L7_IP_ARP_AGE_TIME_DEF
#define FD_IP_DEFAULT_ARP_RESP_TIME         L7_IP_ARP_RESP_TIME_DEF
#define FD_IP_DEFAULT_ARP_RETRIES           L7_IP_ARP_RETRIES_DEF
#define FD_IP_DEFAULT_ARP_CACHE_SIZE        L7_IP_ARP_CACHE_SIZE_DEF
#define FD_IP_DEFAULT_ARP_DYNAMIC_RENEW     L7_IP_ARP_DYNAMIC_RENEW_DEF

#define FD_RTR_DEFAULT_MTU                   1500
#define FD_RTR_DEFAULT_ADMIN_MODE            L7_DISABLE
#define FD_RTR_DEFAULT_FORWARDING            L7_ENABLE
#define FD_RTR_DEFAULT_TOS_FORWARDING        L7_DISABLE
#define FD_RTR_DEFAULT_ECMP_MODE             L7_DISABLE
#define FD_RTR_DEFAULT_ICMP_REDIRECT_MODE    L7_ENABLE
#define FD_RTR_DEFAULT_ICMP_UNREACHABLES_MODE    L7_ENABLE
#define FD_RTR_DEFAULT_ICMP_RATE_LIMIT_SIZE 100   /* tokens */
#define FD_RTR_DEFAULT_ICMP_RATE_LIMIT_INTERVAL 1000 /*msec */
#define FD_RTR_DEFAULT_ICMP_ECHO_REPLY_MODE L7_ENABLE

#define FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MAXHOPCOUNT     4
#define FD_RTR_DEFAULT_UDP_RELAY_ADMIN_MODE             L7_DISABLE
#define FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_MINWAITTIME     0
#define FD_RTR_DEFAULT_BOOTP_DHCP_RELAY_CIRCUITIDOPTION L7_DISABLE

#define FD_RTR_DEFAULT_GRAT_ARP              L7_ENABLE


/* IP interfaces are numbered by default */
#define FD_RTR_INTF_UNNUMBERED L7_FALSE

/* Whether proxy ARP is enabled by default on routing interfaces */
#define FD_PROXY_ARP_ADMIN_MODE              L7_ENABLE
#define FD_LOCAL_PROXY_ARP_ADMIN_MODE        L7_DISABLE

#define FD_RTR_DEFAULT_INTF_ADMIN_MODE       L7_DISABLE
#define FD_RTR_DEFAULT_FWD_MULTICASTS        L7_DISABLE
#define FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST  L7_DISABLE

#define FD_RTR_DEFAULT_TRACE_MODE            L7_DISABLE
#define FD_RTR_DEFAULT_INET_IP_ADR           L7_NULL_IP_ADDR
#define FD_RTR_DEFAULT_ADVERTISEMENT_MODE    ADVERTISEMENT
#define FD_RTR_DEFAULT_ADV_LIFETIME          ADV_LIFETIME
#define FD_RTR_DEFAULT_MAX_ADV_INTERVAL      MAX_ADV_INTERVAL
#define FD_RTR_DEFAULT_MIN_ADV_INTERVAL      MIN_ADV_INTERVAL
#define FD_RTR_DEFAULT_PREFERENCE_LEVEL      PREFERENCE_LEVEL
#define FD_RTR_DEFAULT_ADV_ADDR              L7_DEFAULT_ADV_ADDR
/* Whether proxy ARP is enabled by default on routing interfaces */
#define FD_PROXY_ARP_ADMIN_MODE              L7_ENABLE



/* The maximum static ARPs porting constant is defined here, and is derived
 * from the maximum number of ARPs defined in l3_platform.h.  For simplicity,
 * the user configurable number of static ARP entries is quantized into 
 * one of only a few possible values, determined as follows:
 *
 *    platform max ARPs   max static ARPs
 *    -----------------   ---------------
 *      4096 and up             128
 *      1024 to 4095             64
 *         1 to 1023             16
 *
 * This is considered more user-friendly than using a straight percentage.
 *
 * If a particular platform must define a value different than that specified 
 * here, add a #define L7_L3_ARP_CACHE_STATIC_MAX to l3_platform.h to override
 * these pre-defined settings.
 *
 * CAUTION!!  Do not let L7_IP_ARP_CACHE_STATIC_MAX exceed 
 *            FD_RTR_MAX_STATIC_ARP_ENTRIES!
 */
#ifdef L7_L3_ARP_CACHE_STATIC_MAX
  #define L7_IP_ARP_CACHE_STATIC_MAX        L7_L3_ARP_CACHE_STATIC_MAX
#else
  #define L7_IP_ARP_CACHE_STATIC_MAX \
            ((L7_L3_ARP_CACHE_SIZE >= 4096) ? 128 : \
              ((L7_L3_ARP_CACHE_SIZE >= 1024) ? 64 : 16))
#endif


#define FD_RTR_ROUTE_DEFAULT_COST            1  /*  Default cost of a route - xif */
#define FD_RTR_STATIC_ROUTE_COST             0  /* Cost of static routes */
#define FD_RTR_ROUTE_LOCAL_COST              0  /* Cost of local routes */


#define FD_RTR_MAX_NUM_BUFFERS               3 * L7_RTR_MAX_RTR_INTERFACES              


   
/*  Routing Protocol Default Preferences             */

#define    FD_RTR_RTO_PREF_UNSUPPORTED     255
#define    FD_RTR_RTO_PREF_LOCAL           0
#define    FD_RTR_RTO_PREF_STATIC          1
#define    FD_RTR_RTO_PREF_MPLS            FD_RTR_RTO_PREF_UNSUPPORTED
#define    FD_RTR_RTO_PREF_OSPF_INTRA      110
#define    FD_RTR_RTO_PREF_OSPF_INTER      110
#define    FD_RTR_RTO_PREF_OSPF_EXTERNAL   110
#define    FD_RTR_RTO_PREF_RIP             120
#define    FD_RTR_RTO_PREF_IBGP            170 
#define    FD_RTR_RTO_PREF_EBGP            170 
   
        
#ifdef  L7_MAP_ROUTE_PREFERENCES 
static L7_uint32 FD_rtr_rto_preferences[L7_ROUTE_MAX_PREFERENCES + 1] = {
    0,
    FD_RTR_RTO_PREF_LOCAL,
    FD_RTR_RTO_PREF_STATIC,
    FD_RTR_RTO_PREF_MPLS,
    FD_RTR_RTO_PREF_OSPF_INTRA,
    FD_RTR_RTO_PREF_OSPF_INTER,
    FD_RTR_RTO_PREF_OSPF_EXTERNAL,
    FD_RTR_RTO_PREF_RIP,
    FD_RTR_RTO_PREF_IBGP,
    FD_RTR_RTO_PREF_EBGP,
    FD_RTR_RTO_PREF_UNSUPPORTED        /* Always keep this last */
};
#endif
   
   
/* End of IP Component's Factory Defaults */



/* Start of OSPF Map Component's Factory Defaults */
/* Start of OSPF Map Component's Factory Defaults */
/* Start of OSPF Map Component's Factory Defaults */
/* Start of OSPF Map Component's Factory Defaults */
/* Start of OSPF Map Component's Factory Defaults */



/*  OSPF Component Defines */
/* DEVELOPER NOTE:  These need to be less than or equal to values in default_config.h */
#if (30 > L7_MAX_INTERFACE_COUNT)
#define L7_OSPF_MAX_AREAS                    (L7_MAX_INTERFACE_COUNT)   /* Needs to be less than or equal
                                                                           to L7_MAX_INTERFACE_COUNT */
#else
#define L7_OSPF_MAX_AREAS                    (30)   /* Needs to be less than or equal
                                                       to L7_MAX_INTERFACE_COUNT */
#endif
#define L7_OSPF_MAX_AREA_RANGES              (2*L7_OSPF_MAX_AREAS)
#define L7_OSPF_MAX_VIRT_NBRS                10

/* ospfCfgData_t default values */
#define FD_OSPF_DEFAULT_ROUTER_ID            0          /* If 0 the System IP Address 
                                                          is used */
#define FD_OSPF_DEFAULT_ADMIN_MODE                    L7_ENABLE
#define FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY        L7_ENABLE
#define FD_OSPF_DEFAULT_EXT_LSDB_LIMIT                -1
#define FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL        0          
#define FD_OSPF_DEFAULT_SPF_DELAY                     5          
#define FD_OSPF_DEFAULT_SPF_HOLDTIME                  10          
#define FD_OSPF_DEFAULT_OPAQUE_LSA_MODE               L7_ENABLE
#define FD_OSPF_DEFAULT_TRACE_MODE                    L7_DISABLE 
#define FD_OSPF_MAXIMUM_PATHS                         (L7_RT_MAX_EQUAL_COST_ROUTES < 4)?L7_RT_MAX_EQUAL_COST_ROUTES:4
#define FD_OSPF_DEFAULT_MTU_IGNORE                    L7_FALSE
#define FD_OSPF_DEFAULT_REFER_BANDWIDTH                100
#define FD_OSPF_DEFAULT_PASSIVE_MODE                  L7_FALSE


/* ospfCfgCkt_t default values */
#define FD_OSPF_INTF_DEFAULT_ADMIN_MODE               L7_DISABLE
#define FD_OSPF_INTF_DEFAULT_AREA                     0
#define FD_OSPF_INTF_DEFAULT_SECONDARIES_FLAG         0xffffffff
#define FD_OSPF_INTF_DEFAULT_PRIORITY                 1       
#define FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL           10
#define FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL            40
#define FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL            5
#define FD_OSPF_INTF_DEFAULT_NBMA_POLL_INTERVAL       120
#define FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY         1
#define FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL         (FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL / 4)  /* same as demo code */
/* Should be at least 1 */  
#define FD_OSPF_INTF_DEFAULT_AUTH_TYPE                L7_AUTH_TYPE_NONE
#define FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR            0x00
#define FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN             0
#define FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID              0
#define FD_OSPF_INTF_DEFAULT_IF_DEMAND_MODE           L7_FALSE    
#define FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID     0
#define FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR       0
#define FD_OSPF_INTF_DEFAULT_VIRT_INTIFNUM            0
#define FD_OSPF_INTF_DEFAULT_TYPE                     L7_OSPF_INTF_BROADCAST
#define FD_OSPF_INTF_DEFAULT_METRIC                   10
#define FD_OSPF_INTF_DEFAULT_PASSIVE_MODE             FD_OSPF_DEFAULT_PASSIVE_MODE

/* ospfAreaCfg_t default values */
#define FD_OSPF_AREA_DEFAULT_AREA                     0
#define FD_OSPF_AREA_DEFAULT_STUB_AREA                L7_FALSE
#define FD_OSPF_AREA_DEFAULT_AGING_INTERVAL           10     /* LVL7 - Do not change - 
                                                                not currently configurable*/
#define FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY   L7_OSPF_AREA_IMPORT_EXT
#define FD_OSPF_AREA_DEFAULT_SUMMARY                  L7_OSPF_AREA_SEND_SUMMARY
#define FD_OSPF_AREA_DEFAULT_STUB_SUMMARY_LSA         L7_ENABLE

/* ospfAreaStubCfg_t default values */
#define FD_OSPF_AREA_DEFAULT_STUB_METRIC_TYPE         L7_OSPF_AREA_STUB_METRIC
#define FD_OSPF_AREA_DEFAULT_STUB_METRIC              1

/* ospfAreaNSSACfg_t default values */
#define FD_OSPF_NSSA_DEFAULT_TRANSLATOR_ROLE                L7_OSPF_NSSA_TRANSLATOR_CANDIDATE
#define FD_OSPF_NSSA_DEFAULT_TRANSLATOR_STABILITY_INTERVAL  L7_OSPF_NSSA_TRANS_STABILITY_INT_DEF
#define FD_OSPF_NSSA_DEFAULT_IMPORT_SUMMARIES               L7_ENABLE
#define FD_OSPF_NSSA_DEFAULT_REDISTRIBUTE                   L7_ENABLE
#define FD_OSPF_NSSA_DEFAULT_DEFAULT_INFO_ORIGINATE         L7_FALSE
#define FD_OSPF_NSSA_DEFAULT_METRIC                         10
#define FD_OSPF_NSSA_DEFAULT_METRIC_TYPE                    L7_OSPF_AREA_STUB_NON_COMPARABLE_COST

/* ospfAreaRangeCfg_t default values */
#define FD_OSPF_AREA_DEFAULT_ADVERTISE_AREA           L7_TRUE
#define FD_OSPF_AREA_DEFAULT_AREA_RANGE_LSDBTYPE      L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK


/* ospfAsbrExtRoute_t default values */
/* Default values related to Route Redistribution. */
#define FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_METRIC_TYPE    L7_OSPF_METRIC_EXT_TYPE2

/* Default value for tag field in external routes. */
#define FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_ROUTE_TAG      0

/* By default, OSPF does not redistribute any routes. */
#define FD_OSPF_REDISTRIBUTE                          L7_FALSE

/* By default, no distribute list is configured to filter
    redistributed routes. */
#define FD_OSPF_DIST_LIST_OUT                         0

/* No redistribution metric is defined by default. -1 is a magic
    value to indicate "not configured." */
#define FD_OSPF_REDIST_METRIC                         -1

/* By default, OSPF does not redistribute subnetted routes. */
#define FD_OSPF_REDIST_SUBNETS                        L7_FALSE

/* Default value for the default metric for OSPF. */
#define FD_OSPF_DEFAULT_METRIC                        0

/* OSPF does not originate a default route unless configured 
    to do so.  */
#define FD_OSPF_ORIG_DEFAULT_ROUTE                    L7_FALSE

/* By default, OSPF only originates a default route if a default route
    exists in the forwarding table. */
#define FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS             L7_FALSE

/* Default value for metric advertised with default route.
    -1 indicates no value is configured. */
#define FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC             -1

/* The default route is by default advertised with a type 2 external 
    metric. */
#define FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE               L7_OSPF_METRIC_EXT_TYPE2

/* End of OSPF Component's Factory Defaults */



/* Start of RIP Map Component's Factory Defaults */
/* Start of RIP Map Component's Factory Defaults */
/* Start of RIP Map Component's Factory Defaults */
/* Start of RIP Map Component's Factory Defaults */
/* Start of RIP Map Component's Factory Defaults */

/* ripCfgData_t default values */
#define FD_RIP_MAP_DEFAULT_ADMIN_MODE                     L7_ENABLE        
#define FD_RIP_MAP_DEFAULT_SPLITHORIZON_MODE          L7_RIP_SPLITHORIZON_SIMPLE
#define FD_RIP_MAP_DEFAULT_AUTOSUMMARY_MODE           L7_DISABLE
#define FD_RIP_MAP_DEFAULT_HOSTROUTESACCEPT_MODE      L7_ENABLE

                 /* 0xffffffff is considered as a invalid value marker, which
                  * is defined in rip_util.h file. The metric value should be a 
                  * valid value i.e. 1 <= metric <= 15, or 0xffffffff
                  */
#define FD_RIP_DEFAULT_DEFAULT_METRIC                 0xffffffff
#define FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE         L7_DISABLE
                 /* if the filter mode is set to disable, then the filter value
                  * should be set to 0xffffffff, which is considered as a invalid
                  * value marker, which is defined in rip_util.h file.
                  */
#define FD_RIP_DEFAULT_REDIST_ROUTE_FILTER_MODE       L7_DISABLE
#define FD_RIP_DEFAULT_REDIST_ROUTE_FILTER            0xffffffff
#define FD_RIP_DEFAULT_REDIST_ROUTE_MODE              L7_DISABLE
                 /* The comment  for FD_RIP_DEFAULT_DEFAULT_METRIC applies here also. */
#define FD_RIP_DEFAULT_REDIST_ROUTE_METRIC            0xffffffff
#define FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE         L7_OSPF_METRIC_TYPE_INTERNAL

/* ripCfgCkt_t default values */
#define FD_RIP_INTF_DEFAULT_ADMIN_MODE                L7_DISABLE
#define FD_RIP_INTF_DEFAULT_AUTH_TYPE                 L7_AUTH_TYPE_NONE
#define FD_RIP_INTF_DEFAULT_AUTH_KEY_CHAR             0x00
#define FD_RIP_INTF_DEFAULT_AUTH_KEY_LEN              0
#define FD_RIP_INTF_DEFAULT_AUTH_KEY_ID               0
#define FD_RIP_INTF_DEFAULT_VER_SEND                  L7_RIP_CONF_SEND_RIPV2
#define FD_RIP_INTF_DEFAULT_VER_RECV                  L7_RIP_CONF_RECV_RIPV2
#define FD_RIP_INTF_DEFAULT_INTF_COST                 1

/* End of RIP Component's Factory Defaults */


/* Start of VRRP Map Component's Factory Defaults */
/* Start of VRRP Map Component's Factory Defaults */
/* Start of VRRP Map Component's Factory Defaults */
/* Start of VRRP Map Component's Factory Defaults */
/* Start of VRRP Map Component's Factory Defaults */

/* vrrpCfgData_t default values */
#define FD_VRRP_DEFAULT_ADMIN_MODE                     L7_DISABLE

/* vrrpCfgCkt_t default values */


#define FD_VRRP_DEFAULT_VRID                           0
#define FD_VRRP_DEFAULT_PRIORITY                       100
#define FD_VRRP_DEFAULT_PREEMPT                        L7_TRUE
#define FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL         1
#define FD_VRRP_DEFAULT_AUTH_TYPE                      L7_AUTH_TYPE_NONE
#define FD_VRRP_DEFAULT_START_STATE                    L7_VRRP_DOWN
#define FD_VRRP_DEFAULT_AUTH_DATA_KEY_CHAR             0x00
#define FD_VRRP_DEFAULT_IP_ADDRESS                     0
#define FD_VRRP_DEFAULT_TRACK_PRIORITY_DECREMENT       10

/* End of VRRP Component's Factory Defaults */

#ifdef INCLUDE_L3_DEFAULTCONFIG_OVERRIDES
#include "l3_defaultconfig_overrides.h"
#endif

#endif /* include_config_defaults */


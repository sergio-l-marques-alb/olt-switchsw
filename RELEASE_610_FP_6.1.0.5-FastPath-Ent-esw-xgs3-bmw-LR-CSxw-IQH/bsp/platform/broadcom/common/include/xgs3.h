/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xgs3.h
*
* @purpose  Defines based on the switch capabilities
*
* @component hardware
*
*
* @comments Parameters which are dependent on the switch chip capabilities should
*           be defined here. These constants will be used accross several platforms
*           so defines that are not directly dependant on the switch chip should 
*           not be added to this file.
*
*           Constants defined as L7_* are the absolute values used by FastPath.
*           Constants defined as PLAT_* are chip limitations that are used in 
*           the calculation with product limitations to derive absolute values
*           used by FastPath.
*
* @created 03/15/2007
*
* @author nshrivastav 

* @end
*
**********************************************************************/

#ifndef  __INC_XGS3_H
#define  __INC_XGS3_H

#include "flex.h"
#include "datatypes.h"

/*********************************************************************
**  Start of  CHIP Defines 
**********************************************************************/
#ifndef PLAT_BCM_CHIP
#error PLAT_BCM_CHIP must be defined.
#endif
/*********************************************************************
**  End of  CHIP Defines 
**********************************************************************/



/*********************************************************************
**  Start of L2 parameters
**********************************************************************/
#define PLAT_MAX_NUM_LAG_INTF                     64
#define PLAT_MAX_MEMBERS_PER_LAG                  8
#define PLAT_MAX_FRAME_SIZE                       9216
#define PLAT_MAX_VLANS                            4093

#define PLAT_DOT1S_ASYNC_STATE_SET                L7_FALSE 
#define PLAT_DHCP_SNOOPING_MAX_STATIC_ENTRIES     1024 


#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define PLAT_MAX_FDB_MAC_ENTRIES                  8192
#elif PLAT_BCM_CHIP == L7_BCM_FIREBOLT2
/* Max the FDB size for STDL packages only. Performance issues with Stacking */
#ifdef L7_STACKING_PACKAGE
#define PLAT_MAX_FDB_MAC_ENTRIES                  16384
#else
#define PLAT_MAX_FDB_MAC_ENTRIES                  32768
#endif
#else
#define PLAT_MAX_FDB_MAC_ENTRIES                  16384
#endif

#define PLAT_MAX_MFDB_MAC_ENTRIES                 1024  // PTin modified  256
#define PLAT_IPMC_ENTRIES_USED_FOR_L2             0

#ifdef L7_STACKING_PACKAGE
#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            7
#else
#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            8
#endif

/* Maximum Number of DVLAN Ethertypes supported */
#define PLAT_DVLANTAG_MAX_TPIDS                   1

/* Dot1x Maximum users supported depending on platform*/
#if PLAT_BCM_CHIP == L7_BCM_FIREBOLT2
#define PLAT_MAX_DOT1X_USERS                     4094
#else
#define PLAT_MAX_DOT1X_USERS                     1024
#endif

/* Start of Port Mirroring Defines */
#define L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION          L7_MAX_PORT_COUNT
#define L7_MIRRORING_MAX_SESSIONS                       1
/* End of Port Mirroring Defines */


/* Support Denial of Service feature */ 
#define L7_DOSCONTROL_MINTCPHDR_SIZE           0
#define L7_DOSCONTROL_MAXTCPHDR_SIZE           255
#define L7_DOSCONTROL_MINICMP_SIZE             0
#define L7_DOSCONTROL_MAXICMP_SIZE             1023
#define L7_DOSCONTROL_MAXICMPv_SIZE            16384

/* Support Storm Control feature */
#define L7_STORMCONTROL_AVG_PKT_SIZE                     512     /* used to calculate pps */
#define L7_STORMCONTROL_LEVEL_MIN                        0
#define L7_STORMCONTROL_LEVEL_MAX                        100
#define L7_STORMCONTROL_RATE_MIN                         0
#define L7_STORMCONTROL_RATE_MAX                         14880000

/*********************************************************************
**  End of L2 parameters
**********************************************************************/


/*********************************************************************
**  Start of  QOS parameters
**********************************************************************/
#define PLAT_ACL_MAX_LISTS                        100 

#if PLAT_BCM_CHIP == L7_BCM_FIREBOLT2
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          28
#elif PLAT_BCM_CHIP == L7_BCM_HELIX
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          12 
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          12 
#else
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          18
#endif

#define PLAT_ACL_MAX_RULES_PER_LIST               PLAT_ACL_MAX_RULES_PER_MULTILIST 
#define PLAT_ACL_MAX_RULES                        PLAT_ACL_MAX_LISTS*(PLAT_ACL_MAX_RULES_PER_LIST+1)  /* account for implicit deny-all rule */
#define PLAT_ACL_VLAN_MAX_COUNT                   24

#define PLAT_ACL_LOG_RULE_LIMIT                   128
#define PLAT_ACL_LOG_RULE_PER_INTF_DIR_LIMIT      min(PLAT_ACL_LOG_RULE_LIMIT, PLAT_ACL_MAX_RULES_PER_MULTILIST)

#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          6 
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          6
#else
#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          13        
#endif

#define PLAT_DIFFSERV_INST_PER_POLICY_LIM         PLAT_ACL_MAX_RULES_PER_MULTILIST
#define PLAT_DIFFSERV_ATTR_PER_INST_LIM           3 
#define PLAT_DIFFSERV_CLASS_LIM                   32
#define PLAT_DIFFSERV_POLICY_LIM                  64

#ifdef L7_STACKING_PACKAGE
#define PLAT_MAX_CFG_QUEUES_PER_PORT              7
#else
#define PLAT_MAX_CFG_QUEUES_PER_PORT              8
#endif

#define PLAT_MAX_CFG_DROP_PREC_LEVELS             3
/*********************************************************************
**  End of QOS parameters
**********************************************************************/


/*********************************************************************
**  Start of L3 parameters
**********************************************************************/
#ifdef L7_IPV6_PACKAGE
/* Constants when IPv6 package is included */
#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define     PLAT_L3_ARP_CACHE_SIZE                1024 
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
#define     PLAT_L3_ARP_CACHE_SIZE                2048
#else
#define     PLAT_L3_ARP_CACHE_SIZE                4096 
#endif 
#else
/* Constants when only IPv4 package is included */
#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define     PLAT_L3_ARP_CACHE_SIZE                2048
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
/* Even though Bradley has 4K L3 table, it has only 2K nexthop table */
#define     PLAT_L3_ARP_CACHE_SIZE                2048
#else
#define     PLAT_L3_ARP_CACHE_SIZE                4096
#endif
#endif

/* Note: The ARP constants above and NDP constants below use the same
 * Hardware L3 table.
 */
#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        128
#define     PLAT_IPV6_NDP_CACHE_SIZE              512
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        256
#define     PLAT_IPV6_NDP_CACHE_SIZE              1024
#else
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        3072
#define     PLAT_IPV6_NDP_CACHE_SIZE              2048
#endif

#ifdef L7_IPV6_PACKAGE
#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define     PLAT_IPV4_ECMP_ROUTES_MAX             224
#define     PLAT_IPV6_ECMP_ROUTES_MAX             128
#define     PLAT_L3_ROUTE_TBL_SIZE_TOTAL          224 
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
#define     PLAT_IPV4_ECMP_ROUTES_MAX             480  
#define     PLAT_IPV6_ECMP_ROUTES_MAX             256
#define     PLAT_L3_ROUTE_TBL_SIZE_TOTAL          480 
#else
#define     PLAT_IPV4_ECMP_ROUTES_MAX             6112
#define     PLAT_IPV6_ECMP_ROUTES_MAX             3072
#define     PLAT_L3_ROUTE_TBL_SIZE_TOTAL          6112
#endif /* L7_BCM_CHIP */

#else 
#if PLAT_BCM_CHIP == L7_BCM_HELIX
#define     PLAT_IPV4_ECMP_ROUTES_MAX             480  
#define     PLAT_L3_ROUTE_TBL_SIZE_TOTAL          480 
#elif PLAT_BCM_CHIP == L7_BCM_BRADLEY
#define     PLAT_IPV4_ECMP_ROUTES_MAX             992
#define     PLAT_L3_ROUTE_TBL_SIZE_TOTAL          992
#else
#define     PLAT_IPV4_ECMP_ROUTES_MAX             8000
#define     PLAT_L3_ROUTE_TBL_SIZE_TOTAL          8000 
#endif /* PLAT_BCM_CHIP */

#endif /* L7_IPV6_PACKAGE */


#define     PLAT_MAX_NUM_LOOPBACK_INTF            8
#define     PLAT_MAX_NUM_TUNNEL_INTF              8

#define     PLAT_MAX_NUM_VLAN_INTF                128
#define     PLAT_MAX_NUM_ROUTER_INTF              128
#define     PLAT_MAX_NUM_WIRELESS_INTF            64
#define     PLAT_MAX_NUM_L2TUNNEL_VLANS           0

#define     PLAT_RT_MAX_EQUAL_COST_ROUTES         4

/*********************************************************************
**  End of L3 parameters
**********************************************************************/


/*********************************************************************
**  Start of IP multicast parameters
**********************************************************************/
/* IP Multicast Heap Size */
#define PLAT_MULTICAST_V4_HEAP_SIZE                       ((3.2)*(1024)*(1024)) /* 3.2M Heap for IPv4 MRP data */
#define PLAT_MULTICAST_V6_HEAP_SIZE                       ((3.2)*(1024)*(1024)) /* 3.2M Heap for IPv6 MRP data */
/* MFC */
#define PLAT_MULTICAST_FIB_MAX_ENTRIES                    256
/* PIMSM */
#define PLAT_PIMSM_MAX_S_G_RPT_IPV4_TABLE_SIZE            256
#define PLAT_PIMSM_MAX_S_G_RPT_IPV6_TABLE_SIZE            256
#define PLAT_PIMSM_MAX_S_G_IPV4_TABLE_SIZE                256
#define PLAT_PIMSM_MAX_S_G_IPV6_TABLE_SIZE                256
#define PLAT_PIMSM_MAX_STAR_G_IPV4_TABLE_SIZE             256
#define PLAT_PIMSM_MAX_STAR_G_IPV6_TABLE_SIZE             256
/* PIMDM */
#define PLAT_PIMDM_MAX_MRT_IPV4_TABLE_SIZE                256
#define PLAT_PIMDM_MAX_MRT_IPV6_TABLE_SIZE                256
/* DVMRP */
#define PLAT_DVMRP_MAX_MRT_IPV4_TABLE_SIZE                256
/* MGMD */
#define PLAT_MGMD_V4_HEAP_SIZE                            ((2)*(1024)*(1024))   /* 2M Heap for IPv4 MGMD data */
#define PLAT_MGMD_V6_HEAP_SIZE                            ((2.1)*(1024)*(1024)) /* 2.1M Heap for IPv6 MGMD data */
#define PLAT_MGMD_PROXY_MAX_MRT_IPV4_TABLE_SIZE           256
#define PLAT_MGMD_PROXY_MAX_MRT_IPV6_TABLE_SIZE           256
#define PLAT_MGMD_GROUPS_MAX_ENTRIES                      1024
/* PIM */
#define PLAT_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES          12

/*********************************************************************
**  End of IP multicast parameters
**********************************************************************/

/*********************************************************************
**  Start of Stacking paramters 
**********************************************************************/

#define L7_FRONT_PANEL_STACKING               1

/*********************************************************************
**  End of Stacking paramters 
**********************************************************************/

#endif /* __INC_XGS3_H */

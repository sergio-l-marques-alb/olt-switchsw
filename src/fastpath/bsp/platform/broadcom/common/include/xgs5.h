/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xgs5.h
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

#ifndef  __INC_XGS5_H
#define  __INC_XGS5_H

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
#define PLAT_DOT1S_MODE                           L7_ENABLE

#define PLAT_DHCP_SNOOPING_MAX_STATIC_ENTRIES     1024 


/* Max the FDB size for STDL packages only. Performance issues with Stacking */
#if ( PTIN_BOARD == PTIN_BOARD_CXO640G )          /* PTin added */
 #define PLAT_MAX_FDB_MAC_ENTRIES                  131072
#else
#ifdef L7_STACKING_PACKAGE
 #define PLAT_MAX_FDB_MAC_ENTRIES                  32768
#else
 #define PLAT_MAX_FDB_MAC_ENTRIES                  32768
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
#define PLAT_MAX_MFDB_MAC_ENTRIES                256 /* wlan ports use ipmc resources */
#define PLAT_IPMC_ENTRIES_USED_FOR_L2            PLAT_MAX_MFDB_MAC_ENTRIES
#else
/* Note that the value below of 2K is not appropriate for Triumph, Triumph2, etc.
   However, Scorpion can support 2K (HW actually supports 8K), so we'll use the 
   higher value here to enable the testing of Fastpath w/ higher limits. */
#if ( PTIN_BOARD_IS_MATRIX )
 #define PLAT_MAX_MFDB_MAC_ENTRIES                1024    /* Only for matrix boards with Trident switch */
#else
 #define PLAT_MAX_MFDB_MAC_ENTRIES                1024    /* Safe limit for others */
#endif
#define PLAT_IPMC_ENTRIES_USED_FOR_L2            0
#endif

#ifdef L7_STACKING_PACKAGE
#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            7
#else
#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            8
#endif

/* Maximum Number of DVLAN Ethertypes supported */
#define PLAT_DVLANTAG_MAX_TPIDS                   4

/* Dot1x Maximum users supported depending on platform*/
#define PLAT_MAX_DOT1X_USERS                     4094

/* Start of Port Mirroring Defines */
#define L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION          L7_MAX_PORT_COUNT
#define L7_MIRRORING_MAX_SESSIONS                       1
/* End of Port Mirroring Defines */


/* Support Denial of Service feature */ 
#define L7_DOSCONTROL_MINTCPHDR_SIZE           0
#define L7_DOSCONTROL_MAXTCPHDR_SIZE           255
#define L7_DOSCONTROL_MINICMP_SIZE             0

/* The maximum value that can be programmed into the HW is 16384. However, this includes
   the 8 byte ICMP header as well as the ICMP payload. Therefore, the value below, 
   presented to the user, is the maximum ICMP payload size. */
#define L7_DOSCONTROL_MAXICMP_SIZE             16376

/* Support Storm Control feature */
#define L7_STORMCONTROL_AVG_PKT_SIZE                     512     /* used to calculate pps */
#define L7_STORMCONTROL_LEVEL_MIN                        0
#define L7_STORMCONTROL_LEVEL_MAX                        100
#define L7_STORMCONTROL_RATE_MIN                         0
#define L7_STORMCONTROL_RATE_MAX                         14880000

/* PFC pararameters */
#ifdef L7_PFC_PACKAGE
#define PLAT_PFC_MAX_PG_PER_INTF              2
#else
#define PLAT_PFC_MAX_PG_PER_INTF              0
#endif

/*********************************************************************
**  End of L2 parameters
**********************************************************************/


/*********************************************************************
**  Start of  QOS parameters
**********************************************************************/
#define PLAT_ACL_MAX_LISTS                        100 
/* These numbers are somewhat arbitrary. With the introduction
   of virtual slice grouping the actual number of rule per
   multilist can be quite large since ACLs can now span
   slices. These numbers are chosen to put some reasonable
   bounds on the user configuration, even if the HW
   could potentially support higher numbers. */
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          1023
#define PLAT_ACL_MAX_RULES_PER_LIST               PLAT_ACL_MAX_RULES_PER_MULTILIST 
#define PLAT_ACL_MAX_RULES                        (16 * 1024)
#define PLAT_ACL_VLAN_MAX_COUNT                   24

#define PLAT_ACL_LOG_RULE_LIMIT                   128
#define PLAT_ACL_LOG_RULE_PER_INTF_DIR_LIMIT      min(PLAT_ACL_LOG_RULE_LIMIT, PLAT_ACL_MAX_RULES_PER_MULTILIST)

#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          13        

#define PLAT_DIFFSERV_INST_PER_POLICY_LIM         28
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

/* ARP cache size */
#define     PLAT_L3_ARP_CACHE_SIZE_DUAL           6144
#define     PLAT_L3_ARP_CACHE_SIZE_V4ONLY         6144


/* Note: The ARP constants above and NDP constants below use the same
 * Hardware L3 table.
 */
#if ((PLAT_BCM_CHIP == L7_BCM_SCORPION) || (PLAT_BCM_CHIP == L7_BCM_ENDURO))
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        3072
/* PTin added: new switch BCM56843 */
#elif (PLAT_BCM_CHIP == L7_BCM_TRIDENT)
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        4096
/* PTin end */
#else
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        4096
#endif
#define     PLAT_IPV6_NDP_CACHE_SIZE              2560

/* IPv4 routing table sizes */
#if ((PLAT_BCM_CHIP == L7_BCM_SCORPION) || (PLAT_BCM_CHIP == L7_BCM_ENDURO))
#define     PLAT_IPV4_ROUTE_TBL_SIZE_DUAL         6112
#define     PLAT_IPV4_ROUTE_TBL_SIZE_V4ONLY       12256
/* PTin added: new switch BCM56843 */
#elif (PLAT_BCM_CHIP == L7_BCM_TRIDENT)
#define     PLAT_IPV4_ROUTE_TBL_SIZE_DUAL         8160
#define     PLAT_IPV4_ROUTE_TBL_SIZE_V4ONLY       12256
/* PTin end */
#else
#define     PLAT_IPV4_ROUTE_TBL_SIZE_DUAL         8160
#define     PLAT_IPV4_ROUTE_TBL_SIZE_V4ONLY       12256
#endif




#define     PLAT_MAX_NUM_LOOPBACK_INTF            8
#define     PLAT_MAX_NUM_TUNNEL_INTF              8

#define     PLAT_MAX_NUM_VLAN_INTF                128
#define     PLAT_MAX_NUM_ROUTER_INTF              128
#define     PLAT_MAX_NUM_WIRELESS_INTF            64
#ifdef L7_WIRELESS_PACKAGE
#define     PLAT_MAX_NUM_L2TUNNEL_VLANS           64
#else
#define     PLAT_MAX_NUM_L2TUNNEL_VLANS           0
#endif

#define     PLAT_MAX_NUM_VLAN_PORT_INTF           8192   /* PTin added: virtual ports */

#define     PLAT_RT_MAX_EQUAL_COST_ROUTES         16

/*********************************************************************
**  End of L3 parameters
**********************************************************************/


/*********************************************************************
**  Start of IP multicast parameters
**********************************************************************/
/* IP Multicast Heap Size */
#ifdef L7_IPV6_PACKAGE
#define PLAT_MULTICAST_V4_HEAP_SIZE                       ((44.6)*(1024)*(1024))   /* 44.6M Heap for IPv4 MRP data */
#define PLAT_MULTICAST_V6_HEAP_SIZE                       ((15.5)*(1024)*(1024))   /* 15.4M Heap for IPv6 MRP data */
#else
#define PLAT_MULTICAST_V4_HEAP_SIZE                       ((60)*(1024)*(1024))   /* 60M Heap for IPv4 MRP data */
#define PLAT_MULTICAST_V6_HEAP_SIZE                       (0)                    /* 0M Heap for IPv6 MRP data */
#endif /* L7_IPV6_PACKAGE */
/* MFC */
#define PLAT_MULTICAST_FIB_MAX_ENTRIES                    2048 - PLAT_MAX_NUM_L2TUNNEL_VLANS - PLAT_IPMC_ENTRIES_USED_FOR_L2

#define PLAT_MULTICAST_IPV4_ROUTES_V4ONLY                 PLAT_MULTICAST_FIB_MAX_ENTRIES
#define PLAT_MULTICAST_IPV4_ROUTES_DUAL                   1536
#define PLAT_MULTICAST_IPV6_ROUTES                        512

/* DVMRP */
#define PLAT_DVMRP_MAX_MRT_IPV4_TABLE_SIZE                256

/* MGMD */
#define PLAT_MGMD_V4_HEAP_SIZE                            ((4)*(1024)*(1024)) /* 4M Heap for IPv4 MGMD data */
#define PLAT_MGMD_V6_HEAP_SIZE                            ((4)*(1024)*(1024)) /* 4M Heap for IPv6 MGMD data */

#define PLAT_MGMD_GROUPS_MAX_ENTRIES                      2048
/* PIM */
#define PLAT_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES          48

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

#endif /* __INC_XGS5_H */

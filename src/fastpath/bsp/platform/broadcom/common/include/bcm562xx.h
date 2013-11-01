/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename bcm562xx.h
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
*
* @created 03/15/2007
*
* @author nshrivastav 

* @end
*
**********************************************************************/

#ifndef  __INC_BCM562XX_H
#define  __INC_BCM562XX_H

#include "flex.h"
#include "datatypes.h"

/*********************************************************************
**  Start of  CHIP Defines
**********************************************************************/

/* The bcm562xx family consists of Raptor, Raven and Tropicana platforms */
#define L7_BCM_RAPTOR   1
#define L7_BCM_RAVEN    2

#ifndef PLAT_BCM_CHIP
/* For SMB builds, use RAPTOR like constants. For Enterprise builds, use RAVEN
 * constants
 */
#if (defined(L7_PRODUCT_SMB) || defined(L7_PRODUCT_SMARTPATH))
#define PLAT_BCM_CHIP   L7_BCM_RAPTOR
#else
/* Note: Many of the platform constants for RAVEN are scaled down to fit as
 * many packages in the 128MB RAM on reference platforms.
 */
#define PLAT_BCM_CHIP   L7_BCM_RAVEN
#endif

#endif
/*********************************************************************
**  End of  CHIP Defines
**********************************************************************/


/*********************************************************************
**  Start of L2 parameters
**********************************************************************/
#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define PLAT_MAX_NUM_LAG_INTF                     8    /* 32 as per actual tbl size */ 
#else   
#define PLAT_MAX_NUM_LAG_INTF                     64
#endif

#define PLAT_MAX_MEMBERS_PER_LAG                  8
#define PLAT_MAX_FRAME_SIZE                       9216
#if PLAT_BCM_CHIP == L7_BCM_RAVEN
/* 4094 as per actual table limit. Use 256 instead to fit in 128MB RAM */
#define PLAT_MAX_VLANS                            (256 - (1))
#else
#define PLAT_MAX_VLANS                            (4094 - (1))
#endif

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define PLAT_MAX_FDB_MAC_ENTRIES                  8192  /* 16384 as per actual tbl size */
#else
#define PLAT_MAX_FDB_MAC_ENTRIES                  8192
#endif
#define PLAT_MAX_MFDB_MAC_ENTRIES		 256
#define PLAT_IPMC_ENTRIES_USED_FOR_L2    0


#define PLAT_DOT1S_ASYNC_STATE_SET                L7_TRUE
#define PLAT_DOT1S_MODE                           L7_ENABLE 

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define PLAT_DHCP_SNOOPING_MAX_STATIC_ENTRIES     32
#else
#define PLAT_DHCP_SNOOPING_MAX_STATIC_ENTRIES     512
#endif

#ifdef L7_STACKING_PACKAGE
#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            7
#else
#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            8
#endif

/* Maximum Number of DVLAN Ethertypes supported */
#define PLAT_DVLANTAG_MAX_TPIDS                   1

/* Dot1x Maximum users supported depending on platform*/
#define PLAT_MAX_DOT1X_USERS                     1024

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

/*********************************************************************
**  End of L2 parameters
**********************************************************************/


/*********************************************************************
**  Start of  QOS parameters
**********************************************************************/
#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define PLAT_ACL_MAX_LISTS                        50
#else
#define PLAT_ACL_MAX_LISTS                        100
#endif

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
/* These numbers are somewhat arbitrary. With the introduction
   of virtual slice grouping the actual number of rule per
   multilist can be quite large since ACLs can now span
   slices. These numbers are chosen to put some reasonable
   bounds on the user configuration, even if the HW
   could potentially support higher numbers. */
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          1023
#define PLAT_ACL_MAX_RULES                        4096
#else
#define PLAT_ACL_MAX_RULES_PER_MULTILIST          127
#define PLAT_ACL_MAX_RULES                        1024
#endif

#define PLAT_ACL_MAX_RULES_PER_LIST               PLAT_ACL_MAX_RULES_PER_MULTILIST
#define PLAT_ACL_VLAN_MAX_COUNT                   24

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define PLAT_ACL_LOG_RULE_LIMIT                   32
#else
#define PLAT_ACL_LOG_RULE_LIMIT                   128
#endif

#define PLAT_ACL_LOG_RULE_PER_INTF_DIR_LIMIT      min(PLAT_ACL_LOG_RULE_LIMIT, PLAT_ACL_MAX_RULES_PER_MULTILIST)

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          6  /* 13 is platform limit */
#else
#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          6 
#endif

#define PLAT_DIFFSERV_INST_PER_POLICY_LIM         12
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

/* Please refer to chip documentation for more details on table sizes */
/* IPv4 unicast routing sizes */

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define     PLAT_L3_ARP_CACHE_SIZE_DUAL           512   /* 2048 as per actual tbl size */
#define     PLAT_L3_ARP_CACHE_SIZE_V4ONLY         512   /* 4096 as per actual tbl size */
#define     PLAT_IPV4_ROUTE_TBL_SIZE_DUAL         512   /* 4000 as per actual tbl size */
#define     PLAT_IPV4_ROUTE_TBL_SIZE_V4ONLY       512   /* 8000 as per actual tbl size */
#else
#define     PLAT_L3_ARP_CACHE_SIZE_DUAL           63   
#define     PLAT_L3_ARP_CACHE_SIZE_V4ONLY         254   
#define     PLAT_IPV4_ROUTE_TBL_SIZE_DUAL         64   
#define     PLAT_IPV4_ROUTE_TBL_SIZE_V4ONLY       254   
#endif


/* IPv6 unicast routing sizes */
#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        256  /* 2000 as per actual tbl size */
#define     PLAT_IPV6_NDP_CACHE_SIZE              256  /* 1024 as per actual tbl size */
#else
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        127
#define     PLAT_IPV6_NDP_CACHE_SIZE              512
#endif

#define     PLAT_MAX_NUM_LOOPBACK_INTF            8
#define     PLAT_MAX_NUM_TUNNEL_INTF              8

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define     PLAT_MAX_NUM_VLAN_INTF                32   /* max 128 can be supported */
#define     PLAT_MAX_NUM_ROUTER_INTF              32   /* max 128 can be supported */
#else
#define     PLAT_MAX_NUM_VLAN_INTF                32
#define     PLAT_MAX_NUM_ROUTER_INTF              32
#endif

#if PLAT_BCM_CHIP == L7_BCM_RAVEN
#define     PLAT_RT_MAX_EQUAL_COST_ROUTES         4
#else
#define     PLAT_RT_MAX_EQUAL_COST_ROUTES         1
#endif

#define     PLAT_MAX_NUM_WIRELESS_INTF            64
#define     PLAT_MAX_NUM_L2TUNNEL_VLANS           0

#define     PLAT_MAX_NUM_VLAN_PORT_INTF           1   /* PTin added: virtual ports */

/*********************************************************************
**  End of L3 parameters
**********************************************************************/


/*********************************************************************
**  Start of IP multicast parameters
**********************************************************************/
#if PLAT_BCM_CHIP == L7_BCM_RAVEN
/* Reduce IP multicast constants to fit in 128MB RAM */
/* IP Multicast Heap Size */
#define PLAT_MULTICAST_V4_HEAP_SIZE                       ((3.2)*(1024)*(1024)) /* 3.2M Heap for IPv4 MRP data */
#define PLAT_MULTICAST_V6_HEAP_SIZE                       ((3.2)*(1024)*(1024)) /* 3.2M Heap for IPv6 MRP data */
/* MFC */
#define PLAT_MULTICAST_FIB_MAX_ENTRIES                    64

#define PLAT_MULTICAST_IPV4_ROUTES_V4ONLY                 PLAT_MULTICAST_FIB_MAX_ENTRIES
#define PLAT_MULTICAST_IPV4_ROUTES_DUAL                   64
#define PLAT_MULTICAST_IPV6_ROUTES                        64

/* DVMRP */
#define PLAT_DVMRP_MAX_MRT_IPV4_TABLE_SIZE                64
/* MGMD */
#define PLAT_MGMD_V4_HEAP_SIZE                            ((2)*(1024)*(1024))   /* 2M Heap for IPv4 MGMD data */
#define PLAT_MGMD_V6_HEAP_SIZE                            ((2.1)*(1024)*(1024)) /* 2.1M Heap for IPv6 MGMD data */
#define PLAT_MGMD_GROUPS_MAX_ENTRIES                      96
/* PIM */
#define PLAT_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES          12
#else
/* IP Multicast Heap Size */
#define PLAT_MULTICAST_V4_HEAP_SIZE                       ((3.2)*(1024)*(1024)) /* 3.2M Heap for IPv4 MRP data */
#define PLAT_MULTICAST_V6_HEAP_SIZE                       ((3.2)*(1024)*(1024)) /* 3.2M Heap for IPv6 MRP data */
/* MFC */
#define PLAT_MULTICAST_FIB_MAX_ENTRIES                    256

#define PLAT_MULTICAST_IPV4_ROUTES_V4ONLY                 PLAT_MULTICAST_FIB_MAX_ENTRIES
#define PLAT_MULTICAST_IPV4_ROUTES_DUAL                   256
#define PLAT_MULTICAST_IPV6_ROUTES                        256

/* DVMRP */
#define PLAT_DVMRP_MAX_MRT_IPV4_TABLE_SIZE                256

/* MGMD */
#define PLAT_MGMD_V4_HEAP_SIZE                            ((2)*(1024)*(1024))   /* 2M Heap for IPv4 MGMD data */
#define PLAT_MGMD_V6_HEAP_SIZE                            ((2.1)*(1024)*(1024)) /* 2.1M Heap for IPv6 MGMD data */
#define PLAT_MGMD_GROUPS_MAX_ENTRIES                      1024
/* PIM */
#define PLAT_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES          12
#endif

/*********************************************************************
**  End of IP multicast parameters
**********************************************************************/

/*********************************************************************
**  Start of Stacking paramters 
**********************************************************************/

/* These constants are derived based on the packet buffer memory available
 * on 26porter and 50porter Raptor platforms. With 8 COSQs, the 50porter
 * can only receive packets upto 1536 + 128 bytes (not enough cells). Whereas
 * the 24porter can handle upto 6K bytes. 
 * Note, ATP header adds to the HPC message len. 
 */
#define HPC_TRANSPORT_MSG_LENGTH 1500
#define FFTP_MAX_SEGMENT_SIZE    1400

#define L7_FRONT_PANEL_STACKING               1

/*********************************************************************
**  End of Stacking paramters 
**********************************************************************/

#endif /* __INC_BCM562XX_H */

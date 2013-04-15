/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename robo.h
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

#ifndef  __INC_ROBO_H
#define  __INC_ROBO_H

#include "flex.h"
#include "datatypes.h"

/*********************************************************************
**  Start of L2 parameters
**********************************************************************/
#define PLAT_MAX_NUM_LAG_INTF                     2
#define PLAT_MAX_MEMBERS_PER_LAG                  4
#define PLAT_MAX_FRAME_SIZE                       9720
#define PLAT_MAX_VLANS                            4093

#define PLAT_DOT1S_ASYNC_STATE_SET                L7_FALSE 
#define PLAT_DOT1S_MODE                           L7_ENABLE

#define PLAT_DHCP_SNOOPING_MAX_STATIC_ENTRIES     1024 


#define PLAT_MAX_FDB_MAC_ENTRIES                  4096
#define PLAT_MAX_MFDB_MAC_ENTRIES		 256
#define PLAT_IPMC_ENTRIES_USED_FOR_L2    0


#define PLAT_DOT1P_NUM_TRAFFIC_CLASSES            4

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
#define L7_STORMCONTROL_RATE_MAX                         33554431

/*********************************************************************
**  End of L2 parameters
**********************************************************************/


/*********************************************************************
**  Start of  QOS parameters
**********************************************************************/
#define PLAT_ACL_MAX_LISTS                        100 

#define PLAT_ACL_MAX_RULES_PER_MULTILIST          18

#define PLAT_ACL_MAX_RULES_PER_LIST               PLAT_ACL_MAX_RULES_PER_MULTILIST 
#define PLAT_ACL_MAX_RULES                        PLAT_ACL_MAX_LISTS*(PLAT_ACL_MAX_RULES_PER_LIST+1)  /* account for implicit deny-all rule */
#define PLAT_ACL_VLAN_MAX_COUNT                   24

#define PLAT_ACL_LOG_RULE_LIMIT                   128
#define PLAT_ACL_LOG_RULE_PER_INTF_DIR_LIMIT      min(PLAT_ACL_LOG_RULE_LIMIT, PLAT_ACL_MAX_RULES_PER_MULTILIST)

#define PLAT_DIFFSERV_RULE_PER_CLASS_LIM          13        

#define PLAT_DIFFSERV_INST_PER_POLICY_LIM         PLAT_ACL_MAX_RULES_PER_MULTILIST
#define PLAT_DIFFSERV_ATTR_PER_INST_LIM           3 
#define PLAT_DIFFSERV_CLASS_LIM                   32
#define PLAT_DIFFSERV_POLICY_LIM                  64

#define PLAT_MAX_CFG_QUEUES_PER_PORT              4

#define PLAT_MAX_CFG_DROP_PREC_LEVELS             3
/*********************************************************************
**  End of QOS parameters
**********************************************************************/


/*********************************************************************
**  Start of L3 parameters
**********************************************************************/

/* ARP constants */
#define     PLAT_L3_ARP_CACHE_SIZE_DUAL           4096  
#define     PLAT_L3_ARP_CACHE_SIZE_V4ONLY         4096  


/* Note: The ARP constants above and NDP constants below use the same
 * Hardware L3 table.
 */
#define     PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL        3000
#define     PLAT_IPV6_NDP_CACHE_SIZE              2048

#define     PLAT_IPV4_ROUTE_TBL_SIZE_DUAL         6000
#define     PLAT_IPV4_ROUTE_TBL_SIZE_V4ONLY       8000


#define     PLAT_MAX_NUM_LOOPBACK_INTF            0
#define     PLAT_MAX_NUM_TUNNEL_INTF              0

#define     PLAT_MAX_NUM_VLAN_INTF                0
#define     PLAT_MAX_NUM_ROUTER_INTF              0
#define     PLAT_MAX_NUM_WIRELESS_INTF            0
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

/*********************************************************************
**  End of IP multicast parameters
**********************************************************************/

#endif /* __INC_ROBO_H */

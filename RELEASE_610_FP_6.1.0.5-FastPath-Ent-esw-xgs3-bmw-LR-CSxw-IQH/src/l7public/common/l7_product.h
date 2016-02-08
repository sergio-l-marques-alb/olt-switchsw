/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_product.h
*
* @purpose System defines based on specific hardware/product
*
* @component 
*
* @comments This file is the interface to the rest of the system for all porting
*           parameters. All the paramters should have a prefix of L7_.
*           It is a replacement for old "platform.h". 
*
*
* @created 03/15/2007
*
* @author akulkarni 

* @end
*
**********************************************************************/

#ifndef  __INC_L7_PRODUCT_H__
#define  __INC_L7_PRODUCT_H__

#include "flex.h"
#include "l7_productspecs.h"
#include "l7_platformspecs.h"


/*********************************************************************
**  Start of Base paramters 
**********************************************************************/


/* number of stacking ports that can be configured on a unit
*/
#define L7_MAX_STACK_PORTS_PER_UNIT            PROD_MAX_STACK_PORTS_PER_UNIT

/* Maximum number of physical units per stack.
*/
#define L7_MAX_UNITS_PER_STACK                 PROD_MAX_UNITS_PER_STACK

/* Total number of physical ports */
#define L7_MAX_PORT_COUNT                      (L7_MAX_UNITS_PER_STACK * L7_MAX_PHYSICAL_PORTS_PER_UNIT)

/*********************************************************************
**  End of Base paramters 
**********************************************************************/




/*********************************************************************
**  Start of L2 paramters 
**********************************************************************/
#define L7_MIN_FRAME_SIZE                       PROD_MIN_FRAME_SIZE
#define L7_MAX_NUM_LAG_INTF                     min(PROD_MAX_NUM_LAG_INTF, \
                                                    PLAT_MAX_NUM_LAG_INTF)
#define L7_MFDB_MAX_MAC_ENTRIES                 min(PROD_MFDB_MAX_MAC_ENTRIES,\
	                                             PLAT_MAX_MFDB_MAC_ENTRIES)
#define L7_IPMC_ENTRIES_USED_FOR_L2             min(PROD_MFDB_MAX_MAC_ENTRIES,\
	                                             PLAT_IPMC_ENTRIES_USED_FOR_L2)
#define L7_MAX_FDB_STATIC_FILTER_ENTRIES        PROD_MAX_FDB_STATIC_FILTER_ENTRIES
#define L7_VLAN_IPSUBNET_MAX_VLANS              PROD_VLAN_IPSUBNET_MAX_VLANS
#define L7_VLAN_MAC_MAX_VLANS                   PROD_VLAN_MAC_MAX_VLANS

/* Defines whether the driver supports asynchronous setting of dot1s states*/
/* This is dependent on the number of spanning-tree instances to be supported. Async
   implementation would be chosen based on scaling factors and hence product specific.
   */
#define L7_MAX_VLANS                            min(PROD_MAX_VLANS, \
                                                    PLAT_MAX_VLANS)
#define L7_DOT1P_NUM_TRAFFIC_CLASSES            min(PROD_DOT1P_NUM_TRAFFIC_CLASSES, \
                                                    PLAT_DOT1P_NUM_TRAFFIC_CLASSES)
#define L7_MAX_MEMBERS_PER_LAG                  min(PROD_MAX_MEMBERS_PER_LAG, \
                                                    PLAT_MAX_MEMBERS_PER_LAG)
#define L7_MAX_FRAME_SIZE                       min(PROD_MAX_FRAME_SIZE, \
                                                    PLAT_MAX_FRAME_SIZE)
#define L7_MAX_FDB_MAC_ENTRIES                  min(PROD_MAX_FDB_MAC_ENTRIES, \
                                                    PLAT_MAX_FDB_MAC_ENTRIES)

#define L7_DOT1S_ASYNC_STATE_SET                (PROD_DOT1S_ASYNC_STATE_SET && \
                                                 PLAT_DOT1S_ASYNC_STATE_SET)


#define L7_DHCP_SNOOPING_MAX_STATIC_ENTRIES     PLAT_DHCP_SNOOPING_MAX_STATIC_ENTRIES

#define L7_DOT1X_MAX_USERS                    min(PLAT_MAX_DOT1X_USERS,L7_MAX_PORT_COUNT*PROD_DOT1X_MAX_USERS_CONST) /* maximum no.of dot1x clients supported 
                                                                                             throughout the system*/

/* If set to true, a separate per port/per VLAN entry is stored in the
   forwarding database for each port MAC address/VLAN combination If set to
   false, a separate per/port entry is not stored in the forwading database. */
#define L7_FDB_IVL_STORE_SELF_ADDRESSES                 L7_FALSE


/* Forwarding database type: IVL or SVL */
#define L7_FDB_TYPE                                     L7_IVL


/* Number of filtering databases supported  */
#define L7_FDB_MIN_AGING_TIMEOUT                        10      /* seconds */
#define L7_FDB_MAX_AGING_TIMEOUT                        1000000 /* seconds */

/* Number of Dvlantag Ethertypes (TPIDs) supported */
#define L7_DVLANTAG_MAX_TPIDS                 min(PROD_DVLANTAG_MAX_TPIDS, \
                                                  PLAT_DVLANTAG_MAX_TPIDS)

/*********************************************************************
**  End of L2 paramters 
**********************************************************************/

/*********************************************************************
**  Start of QOS paramters 
**********************************************************************/

/* ACL platform overrides */
#define L7_ACL_MAX_LISTS                        min(PROD_ACL_MAX_LISTS, \
                                                    PLAT_ACL_MAX_LISTS)
#define L7_ACL_MAX_RULES                        min(PROD_ACL_MAX_RULES, \
                                                    PLAT_ACL_MAX_RULES)
#define L7_ACL_MAX_RULES_PER_LIST               min(PROD_ACL_MAX_RULES_PER_LIST, \
                                                    PLAT_ACL_MAX_RULES_PER_LIST)
#define L7_ACL_VLAN_MAX_COUNT                   min(PROD_ACL_VLAN_MAX_COUNT, \
                                                    PLAT_ACL_VLAN_MAX_COUNT)
#define L7_ACL_MAX_RULES_PER_MULTILIST          min(PROD_ACL_MAX_RULES_PER_MULTILIST, \
                                                    PLAT_ACL_MAX_RULES_PER_MULTILIST)
#define L7_ACL_LOG_RULE_LIMIT                   min(PROD_ACL_LOG_RULE_LIMIT, \
                                                    PLAT_ACL_LOG_RULE_LIMIT)
#define L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT      min(PROD_ACL_LOG_RULE_PER_INTF_DIR_LIMIT, \
                                                    PLAT_ACL_LOG_RULE_PER_INTF_DIR_LIMIT)
#define L7_DIFFSERV_RULE_PER_CLASS_LIM          min(PROD_DIFFSERV_RULE_PER_CLASS_LIM, \
                                                    PLAT_DIFFSERV_RULE_PER_CLASS_LIM) 
#define L7_DIFFSERV_INST_PER_POLICY_LIM         min(PROD_DIFFSERV_INST_PER_POLICY_LIM, \
                                                    PLAT_DIFFSERV_INST_PER_POLICY_LIM)
#define L7_DIFFSERV_ATTR_PER_INST_LIM           min(PROD_DIFFSERV_ATTR_PER_INST_LIM, \
                                                    PLAT_DIFFSERV_ATTR_PER_INST_LIM)   

#define L7_DIFFSERV_SERVICE_INTF_LIM            (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF)

#define L7_DIFFSERV_CLASS_LIM                   min(PROD_DIFFSERV_CLASS_LIM, \
                                                    PLAT_DIFFSERV_CLASS_LIM)
#define L7_DIFFSERV_POLICY_LIM                  min(PROD_DIFFSERV_POLICY_LIM, \
                                                    PLAT_DIFFSERV_POLICY_LIM)
#define L7_MAX_CFG_QUEUES_PER_PORT              min(PROD_MAX_CFG_QUEUES_PER_PORT, \
                                                    PLAT_MAX_CFG_QUEUES_PER_PORT)
#define L7_MAX_CFG_DROP_PREC_LEVELS             min(PROD_MAX_CFG_DROP_PREC_LEVELS, \
                                                    PLAT_MAX_CFG_DROP_PREC_LEVELS)

/*********************************************************************
**  End of QOS paramters 
**********************************************************************/


/*********************************************************************
**  Start of L3 paramters 
**********************************************************************/
#define  L7_IPV6_NHRES_MAX                      PROD_IPV6_NHRES_MAX      /* max next hop resolution registrations */

/* Maximum number of IP addresses that may be configured on a routing
** interface (including primary and secondary)
*/
#define  L7_L3_NUM_IP_ADDRS      PROD_MAX_L3_NUM_IP_ADDRS 
#define  L7_L3_NUM_SECONDARIES   PROD_MAX_L3_NUM_SECONDARIES 

/* Number of 6to4 route nexthops allowed */
#define L7_MAX_6TO4_NEXTHOPS                16

#define  L7_MAX_NUM_LOOPBACK_INTF               min(PROD_MAX_NUM_LOOPBACK_INTF, \
                                                    PLAT_MAX_NUM_LOOPBACK_INTF)
#define  L7_MAX_NUM_TUNNEL_INTF                 min(PROD_MAX_NUM_TUNNEL_INTF, \
                                                    PLAT_MAX_NUM_TUNNEL_INTF)
#define  L7_MAX_NUM_VLAN_INTF                   min(PROD_MAX_NUM_VLAN_INTF, \
                                                    PLAT_MAX_NUM_VLAN_INTF)
#define  L7_MAX_NUM_WIRELESS_INTF               min(PROD_MAX_NUM_WIRELESS_INTF, \
                                                    PLAT_MAX_NUM_WIRELESS_INTF)
#define  L7_MAX_NUM_CAPWAP_TUNNEL_INTF          (L7_WIRELESS_MAX_PEER_SWITCHES + \
                                                 L7_WIRELESS_MAX_ACCESS_POINTS)
#define  L7_MAX_NUM_L2TUNNEL_VLANS              min(PROD_MAX_NUM_L2TUNNEL_VLANS, \
                                                    PLAT_MAX_NUM_L2TUNNEL_VLANS)


/*
**       Total number of routing interfaces.
*/
#define  L7_MAX_NUM_ROUTER_INTF                 min(PROD_MAX_NUM_ROUTER_INTF, \
                                                    PLAT_MAX_NUM_ROUTER_INTF)
#define  L7_L3_ARP_CACHE_SIZE                   min(PROD_L3_ARP_CACHE_SIZE, \
                                                    PLAT_L3_ARP_CACHE_SIZE )
/* If Static ARP entries limit is define by product spec, set the global
 * define to that value
 */
#ifdef   PROD_L3_ARP_CACHE_STATIC_MAX
   #define  L7_L3_ARP_CACHE_STATIC_MAX             PROD_L3_ARP_CACHE_STATIC_MAX
#endif

#define  L7_IPV6_ROUTE_TBL_SIZE_TOTAL           min(PROD_IPV6_ROUTE_TBL_SIZE_TOTAL, \
                                                    PLAT_IPV6_ROUTE_TBL_SIZE_TOTAL)
#define  L7_IPV6_NDP_CACHE_SIZE                 min(PROD_IPV6_NDP_CACHE_SIZE, \
                                                    PLAT_IPV6_NDP_CACHE_SIZE)
#define  L7_IPV4_ECMP_ROUTES_MAX                min(PROD_IPV4_ECMP_ROUTES_MAX, \
                                                    PLAT_IPV4_ECMP_ROUTES_MAX)
#define  L7_IPV6_ECMP_ROUTES_MAX                min(PROD_IPV6_ECMP_ROUTES_MAX, \
                                                    PLAT_IPV6_ECMP_ROUTES_MAX)
#define  L7_L3_ROUTE_TBL_SIZE_TOTAL             min(PROD_L3_ROUTE_TBL_SIZE_TOTAL, \
                                                    PLAT_L3_ROUTE_TBL_SIZE_TOTAL )
#define  L7_RT_MAX_EQUAL_COST_ROUTES            min(PROD_RT_MAX_EQUAL_COST_ROUTES, \
                                                    PLAT_RT_MAX_EQUAL_COST_ROUTES)

/*********************************************************************
**  End of L3 paramters 
**********************************************************************/

/*********************************************************************
**  Start of IP Multicast paramters 
**********************************************************************/
/* IP Multicast Heap Size */
#define L7_MULTICAST_V4_HEAP_SIZE            min(PROD_MULTICAST_V4_HEAP_SIZE, \
                                                 PLAT_MULTICAST_V4_HEAP_SIZE)
#define L7_MULTICAST_V6_HEAP_SIZE            min(PROD_MULTICAST_V6_HEAP_SIZE, \
                                                 PLAT_MULTICAST_V6_HEAP_SIZE)

/* Number of static RPs supported.*/
#define L7_PIMSM_MAX_STATIC_RP_NUM              PROD_PIMSM_MAX_STATIC_RP_NUM         
/* Number of SSM Range supported.*/
#define L7_PIMSM_MAX_SSM_RANGE                  PROD_PIMSM_MAX_SSM_RANGE         
/* Number of candidate RP supported.*/
#define L7_PIMSM_MAX_CAND_RP_NUM                PROD_PIMSM_MAX_CAND_RP_NUM         
#define L7_PIMSM_MAX_RP_GRP_ENTRIES             PROD_PIMSM_MAX_RP_GRP_ENTRIES
#define L7_PIMSM_MAX_PER_SCOPE_BSR_NODES        PROD_PIMSM_MAX_PER_SCOPE_BSR_NODES
#define L7_PIMSM_MAX_NBR                        PROD_PIMSM_MAX_NBR
#define L7_PIMDM_MAX_NBR                        PROD_PIMDM_MAX_NBR
#define L7_DVMRP_MAX_NBR                        PROD_DVMRP_MAX_NBR
#define L7_DVMRP_MAX_L3_TABLE_SIZE              PROD_DVMRP_MAX_L3_TABLE_SIZE
#define L7_MGMD_MAX_QUERY_SOURCES               PROD_MGMD_MAX_QUERY_SOURCES

/* MFC */
#define L7_MULTICAST_FIB_MAX_ENTRIES            min(PROD_MULTICAST_FIB_MAX_ENTRIES, \
                                                    PLAT_MULTICAST_FIB_MAX_ENTRIES)
/* PIMSM */
#define L7_PIMSM_MAX_S_G_RPT_IPV4_TABLE_SIZE    min(PROD_PIMSM_MAX_S_G_RPT_IPV4_TABLE_SIZE, \
                                                    PLAT_PIMSM_MAX_S_G_RPT_IPV4_TABLE_SIZE)
#define L7_PIMSM_MAX_S_G_RPT_IPV6_TABLE_SIZE    min(PROD_PIMSM_MAX_S_G_RPT_IPV6_TABLE_SIZE, \
                                                    PLAT_PIMSM_MAX_S_G_RPT_IPV6_TABLE_SIZE)
#define L7_PIMSM_MAX_S_G_IPV4_TABLE_SIZE        min(PROD_PIMSM_MAX_S_G_IPV4_TABLE_SIZE, \
                                                    PLAT_PIMSM_MAX_S_G_IPV4_TABLE_SIZE)
#define L7_PIMSM_MAX_S_G_IPV6_TABLE_SIZE        min(PROD_PIMSM_MAX_S_G_IPV6_TABLE_SIZE, \
                                                    PLAT_PIMSM_MAX_S_G_IPV6_TABLE_SIZE)
#define L7_PIMSM_MAX_STAR_G_IPV4_TABLE_SIZE     min(PROD_PIMSM_MAX_STAR_G_IPV4_TABLE_SIZE, \
                                                    PLAT_PIMSM_MAX_STAR_G_IPV4_TABLE_SIZE)
#define L7_PIMSM_MAX_STAR_G_IPV6_TABLE_SIZE     min(PROD_PIMSM_MAX_STAR_G_IPV6_TABLE_SIZE, \
                                                    PLAT_PIMSM_MAX_STAR_G_IPV6_TABLE_SIZE)
/* PIMDM */
#define L7_PIMDM_MAX_MRT_IPV4_TABLE_SIZE        min(PROD_PIMDM_MAX_MRT_IPV4_TABLE_SIZE, \
                                                    PLAT_PIMDM_MAX_MRT_IPV4_TABLE_SIZE)
#define L7_PIMDM_MAX_MRT_IPV6_TABLE_SIZE        min(PROD_PIMDM_MAX_MRT_IPV6_TABLE_SIZE, \
                                                    PLAT_PIMDM_MAX_MRT_IPV6_TABLE_SIZE)
/* DVMRP */
#define L7_DVMRP_MAX_MRT_IPV4_TABLE_SIZE        min(PROD_DVMRP_MAX_MRT_IPV4_TABLE_SIZE, \
                                                    PLAT_DVMRP_MAX_MRT_IPV4_TABLE_SIZE)
/* MGMD */
#define L7_MGMD_V4_HEAP_SIZE                    min(PROD_MGMD_V4_HEAP_SIZE, \
                                                    PLAT_MGMD_V4_HEAP_SIZE)
#define L7_MGMD_V6_HEAP_SIZE                    min(PROD_MGMD_V6_HEAP_SIZE, \
                                                    PLAT_MGMD_V6_HEAP_SIZE)
#define L7_MGMD_GROUPS_MAX_ENTRIES              min(PROD_MGMD_GROUPS_MAX_ENTRIES, \
                                                    PLAT_MGMD_GROUPS_MAX_ENTRIES)
#define L7_MGMD_PROXY_MRT_MAX_IPV4_ROUTE_ENTRIES min(PROD_MGMD_PROXY_MAX_MRT_IPV4_TABLE_SIZE, \
                                                     PLAT_MGMD_PROXY_MAX_MRT_IPV4_TABLE_SIZE)
#define L7_MGMD_PROXY_MRT_MAX_IPV6_ROUTE_ENTRIES min(PROD_MGMD_PROXY_MAX_MRT_IPV6_TABLE_SIZE, \
                                                     PLAT_MGMD_PROXY_MAX_MRT_IPV6_TABLE_SIZE)
/* PIM */
#define L7_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES  min(PROD_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES, \
                                                    PLAT_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES)

/*
 This value should be set to the maximum frame size supported in the platfrom.
 */
#define L7_MULTICAST_MAX_IP_MTU                 PROD_MULTICAST_MAX_IP_MTU 

#define L7_RTR_MAX_STATIC_MROUTES               PROD_MULTICAST_MAX_STATIC_MROUTES
#define L7_MCAST_MAX_ADMINSCOPE_ENTRIES         PROD_MULTICAST_MAX_ADMINSCOPE_ENTRIES

/*********************************************************************
**  End of IP Multicast paramters 
**********************************************************************/

/*********************************************************************
**  Start of Miscellaneous  paramters
**********************************************************************/
#define L7_DOT1S_BPDUFLOOD_SET                  PRODUCT_DOT1S_BPDUFLOOD_SET
/*********************************************************************
**  End of Miscellaneous  paramters
**********************************************************************/

#include "l7_productconfig.h"


/*********************************************************************
**  Start of Derived paramters 
**********************************************************************/
#define L7_MAX_SLOTS_PER_UNIT                   (L7_MAX_PHYSICAL_SLOTS_PER_UNIT + \
                                                 L7_MAX_LOGICAL_SLOTS_PER_UNIT  + \
                                                 L7_MAX_CPU_SLOTS_PER_UNIT)


/* The driver indexes the slot by ZERO base */
/* and the application code is ONE base     */
#define L7_LAG_SLOT_NUM                         (L7_MAX_PHYSICAL_SLOTS_PER_UNIT + 0) /* slot is ZERO base */
#define L7_VLAN_SLOT_NUM                        (L7_LAG_SLOT_NUM                + 1) /* slot is ZERO base */
#define L7_CPU_SLOT_NUM                         (L7_VLAN_SLOT_NUM               + 1)
#define L7_LOOPBACK_SLOT_NUM                    (L7_CPU_SLOT_NUM                + 1)
#define L7_TUNNEL_SLOT_NUM                      (L7_LOOPBACK_SLOT_NUM           + 1)
#define L7_WIRELESS_SLOT_NUM                    (L7_TUNNEL_SLOT_NUM             + 1)
#define L7_CAPWAP_TUNNEL_SLOT_NUM               (L7_WIRELESS_SLOT_NUM           + 1)

/* Maximum Number of Interfaces:
    Interfaces are referenced as 1-based.
*/
/*temp count for wireless intfs*/
#ifdef L7_WIRELESS_PACKAGE
#define  L7_NUM_WIRELESS_INTFS                   L7_MAX_NUM_WIRELESS_INTF \
                                                 + L7_MAX_NUM_CAPWAP_TUNNEL_INTF
#else
#define L7_NUM_WIRELESS_INTFS                    0
#endif

#define L7_MAX_INTERFACE_COUNT                  (L7_MAX_PORT_COUNT        \
                                                 + L7_MAX_CPU_SLOTS_PER_UNIT     \
                                                 + L7_MAX_NUM_LAG_INTF     \
                                                 + L7_MAX_NUM_VLAN_INTF    \
                                                 + L7_MAX_NUM_LOOPBACK_INTF \
                                                 + L7_MAX_NUM_TUNNEL_INTF  \
                                                 + L7_NUM_WIRELESS_INTFS \
                                                 + L7_MAX_NUM_STACK_INTF + 1)

#define L7_ALL_UNITS                            L7_MAX_UNITS_PER_STACK+1

#define L7_ALL_PHYSICAL_INTERFACES              L7_MAX_PORT_COUNT

/*********************************************************************
**  End of Derived paramters 
**********************************************************************/


#include "l7_resources.h"

#ifdef INCLUDE_L7_PRODUCT_OVERRIDES
#include "l7_product_overrides.h"
#endif

#endif

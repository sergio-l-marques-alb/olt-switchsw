/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_productspecs.h
*
* @purpose System defines based on specific product
*
* @component 
*
* @comments Parameters which are dependent on both the product and hardware should
*           be defined here. All the paramaters should have a prefix of 
*           PROD_. The value for the parameter denotes the product constraints.
*           Each parameter listed here should have a corresponding PLAT_xx value in 
*           l7_platspecs.h which denotes the constraints put by the hardware. However,
*           there may be cases where the value supported by the product is very small
*           as compared to what hardware can support. For these parameters, it is
*           not required to define a value in l7_platspecs.h. 
*           The final value for the system parameter will be derived from the
*           product and platform values in file l7_product.h.
*
*
* @created 03/15/2007
*
* @author nshrivastav 
* @end
*
**********************************************************************/

#ifndef  __INC_L7_PRODUCTSPECS_H__
#define  __INC_L7_PRODUCTSPECS_H__

#include "flex.h"


/*********************************************************************
**  Start of  Base paramters 
**********************************************************************/
#ifdef L7_STACKING_PACKAGE
#define PROD_MAX_UNITS_PER_STACK                          8
#else
#define PROD_MAX_UNITS_PER_STACK                          1
#endif

#define PROD_MAX_STACK_PORTS_PER_UNIT                     8
#define PROD_COMPANY_NAME                                 "Broadcom Corp."
#define PROD_COMPANY_OUI                                  0x000AF7
#define PROD_CLASS                                        "Ethernet Device"
/*********************************************************************
**  End of  Base paramters 
**********************************************************************/



/*********************************************************************
**  Start of L2 paramters 
**********************************************************************/

#define PROD_MFDB_MAX_MAC_ENTRIES                         2048    /* Maximum limit extended */
#define PROD_MAX_VLANS                                    4094
#define PROD_MAX_FDB_STATIC_FILTER_ENTRIES                20 
#define PROD_VLAN_IPSUBNET_MAX_VLANS                      128
#define PROD_VLAN_MAC_MAX_VLANS                           256
#define PROD_MAX_NUM_LAG_INTF                             36      /* PTin modified: 64 => 36 (CXP360G) */
#define PROD_MAX_MEMBERS_PER_LAG                          8
#define PROD_MAX_FDB_MAC_ENTRIES                          131072  /* PTin modified: 32768 => 131072 */
#define PROD_MIN_FRAME_SIZE                               1518
#define PROD_MAX_FRAME_SIZE                               9216
#define PROD_DOT1P_NUM_TRAFFIC_CLASSES                    8
#define PROD_DOT1X_MAX_USERS_CONST                        4         /*Note : this value should be between 1 and L7_DOT1X_PORT_MAX_MAC_USERS*/
#define PROD_DOT1S_ASYNC_STATE_SET                        L7_TRUE
#define PROD_DOT1S_MODE                                   L7_DISABLE  /* PTin modified: STP */
#define PROD_DVLANTAG_MAX_TPIDS                           4
#ifdef L7_PFC_PACKAGE
#define PROD_PFC_MAX_PG_PER_INTF                          8
#else
#define PROD_PFC_MAX_PG_PER_INTF                          0
#endif
/*********************************************************************
**  End of  L2 paramters 
**********************************************************************/

/*********************************************************************
**  Start of  QOS paramters 
**********************************************************************/
/* the ACL parameters are made sufficiently large to allow the platform settings to take effect */
#define PROD_ACL_MAX_LISTS                                1024
#define PROD_ACL_MAX_RULES_PER_LIST                       4096
#define PROD_ACL_MAX_RULES                                PROD_ACL_MAX_LISTS*(PROD_ACL_MAX_RULES_PER_LIST+1)  /* account for implicit deny-all rule */
#define PROD_ACL_VLAN_MAX_COUNT                           256
#define PROD_ACL_MAX_RULES_PER_MULTILIST                  PROD_ACL_MAX_RULES_PER_LIST
#define PROD_ACL_LOG_RULE_LIMIT                           512
#define PROD_ACL_LOG_RULE_PER_INTF_DIR_LIMIT              min(PROD_ACL_LOG_RULE_LIMIT, PROD_ACL_MAX_RULES_PER_MULTILIST)

#define PROD_DIFFSERV_RULE_PER_CLASS_LIM                  13
#define PROD_DIFFSERV_INST_PER_POLICY_LIM                 PROD_ACL_MAX_RULES_PER_MULTILIST 
#define PROD_DIFFSERV_ATTR_PER_INST_LIM                   3    
#define PROD_DIFFSERV_CLASS_LIM                           32
#define PROD_DIFFSERV_POLICY_LIM                          64
#define PROD_MAX_CFG_QUEUES_PER_PORT                      8
#define PROD_MAX_CFG_DROP_PREC_LEVELS                     3
#define PROD_MAX_VOIP_CALLS                               16
/*********************************************************************
**  End of  QOS paramters 
**********************************************************************/


/*********************************************************************
**  Start of L3 paramters 
**********************************************************************/
#define     PROD_L3_ARP_CACHE_SIZE                        (6 * 1024)
#define     PROD_L3_ROUTE_TBL_SIZE_TOTAL                  (12 * 1024)
#define     PROD_IPV6_NDP_CACHE_SIZE                      2560
#define     PROD_IPV6_ROUTE_TBL_SIZE_TOTAL                (4 * 1024)
#define     PROD_RT_MAX_EQUAL_COST_ROUTES                 16
#define     PROD_MAX_L3_NUM_IP_ADDRS                      32
#define     PROD_MAX_L3_NUM_SECONDARIES                   PROD_MAX_L3_NUM_IP_ADDRS - 1
#define     PROD_MAX_NUM_LOOPBACK_INTF                    8
#define     PROD_MAX_NUM_TUNNEL_INTF                      8
#define     PROD_MAX_NUM_VLAN_INTF                        128
#define     PROD_MAX_NUM_ROUTER_INTF                      128
#define     PROD_MAX_NUM_WIRELESS_INTF                    64
#define     PROD_MAX_NUM_L2TUNNEL_VLANS                   64
#define     PROD_IPV6_NHRES_MAX                           32
#define     PROD_MAX_NUM_VLAN_PORT_INTF                   1     /* PTin added: virtual ports */
/*********************************************************************
**  End of L3 paramters 
**********************************************************************/


/*********************************************************************
**  Start of IP Multicast paramters 
**********************************************************************/
/* IP Multicast Heap Size */
#define PROD_MULTICAST_V4_HEAP_SIZE                       ((60)*(1024)*(1024))   /* 60M Heap for IPv4 MRP data */
#define PROD_MULTICAST_V6_HEAP_SIZE                       ((15.5)*(1024)*(1024)) /* 15.4M Heap for IPv6 MRP data */
#define PROD_MGMD_V4_HEAP_SIZE                            ((4)*(1024)*(1024))    /* 4M Heap for IPv4 MGMD data */
#define PROD_MGMD_V6_HEAP_SIZE                            ((4)*(1024)*(1024))    /* 4M Heap for IPv6 MGMD data */

#define PROD_PIMSM_MAX_STATIC_RP_NUM                      5         
#define PROD_PIMSM_MAX_SSM_RANGE                          5         
#define PROD_PIMSM_MAX_CAND_RP_NUM                        20         
#define PROD_PIMSM_MAX_RP_GRP_ENTRIES                     64
#define PROD_PIMSM_MAX_PER_SCOPE_BSR_NODES                1
#define PROD_PIMSM_MAX_NBR                                256
#define PROD_PIMDM_MAX_NBR                                256
#define PROD_DVMRP_MAX_NBR                                256
#define PROD_DVMRP_MAX_L3_TABLE_SIZE                      224
#define PROD_DVMRP_MAX_MRT_IPV4_TABLE_SIZE                256
#define PROD_MULTICAST_FIB_MAX_ENTRIES                    2048
#define PROD_MULTICAST_IPV4_ROUTES_V4ONLY                 2048
#define PROD_MULTICAST_IPV4_ROUTES_DUAL                   1536
#define PROD_MULTICAST_IPV6_ROUTES                        512
#define PROD_MULTICAST_MAX_IP_MTU                         1500
#define PROD_MGMD_GROUPS_MAX_ENTRIES                      2048
#define PROD_PIM_NUM_OPTIMAL_OUTGOING_INTERFACES          48
#define PROD_MULTICAST_MAX_STATIC_MROUTES                 50
#define PROD_MULTICAST_MAX_ADMINSCOPE_ENTRIES             50

/* Number of Sources learned/processed per Group in a received 
 * IGMPv3 or MLDv2 message.
 * This is arrived based on the following calculation:
 * ((L7_PORT_ENET_ENCAP_DEF_MTU - 24 - 12) / sizeof(L7_inet_addr_t))
 */
#define PROD_MGMD_MAX_QUERY_SOURCES                       73
/*********************************************************************
**  End of IP Multicast parameters 
**********************************************************************/

/*********************************************************************
**  Start of Wireless parameters 
**********************************************************************/
/* Wireles Client QoS list/policy usage restrictions (conforming to Access Point limitations)  */
#define PROD_WIRELESS_CLTQOS_ACL_MAX_LISTS                      50
#define PROD_WIRELESS_CLTQOS_ACL_MAX_RULES_PER_LIST             10
#define PROD_WIRELESS_CLTQOS_DIFFSERV_CLASS_LIM                 50
#define PROD_WIRELESS_CLTQOS_DIFFSERV_POLICY_LIM                50
#define PROD_WIRELESS_CLTQOS_DIFFSERV_INST_PER_POLICY_LIM       10  
/*********************************************************************
**  End of Wireless parameters 
**********************************************************************/


/*********************************************************************
**  Start of miscellaneous paramters
**********************************************************************/
#define PRODUCT_DOT1S_BPDUFLOOD_SET                        L7_TRUE_DEF
/*********************************************************************
**  End of  miscellaneous paramters
**********************************************************************/

#endif /* __INC_L7_PRODUCTSPECS_H__ */


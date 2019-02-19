/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_productConfig.h
*
* @purpose System defines based on specific product
*
* @component 
*
* @comments Parameters which are only dependent on the product type and visible 
*           to external user should be defined here. 
*           The parameters should have a prefix of L7_.
*           
*
* @Notes   This file is included by l7_product.h
*
* @created 03/15/2007
*
* @author nshrivastav 
* @end
*
**********************************************************************/

#ifndef  __INC_L7_PRODUCTCONFIG_H__
#define  __INC_L7_PRODUCTCONFIG_H__

#include "flex.h"

/*********************************************************************
**  Start of Base paramters 
**********************************************************************/

/* Logical Unit */
#ifdef L7_STACKING_PACKAGE
/* In a stack, all logical cards exist on unit 0 */
#define L7_LOGICAL_UNIT                                     0
#else
#define L7_LOGICAL_UNIT                                     1
#endif


#define L7_MAX_LOGICAL_PORTS_PER_SLOT                       8
#define L7_MAX_LOGICAL_SLOTS_PER_UNIT                       6     
#define L7_MAX_CPU_PORTS_PER_SLOT                           1
#define L7_MAX_CPU_SLOTS_PER_UNIT                           1
#define L7_MAX_NUM_STACK_INTF                               0

/* Maximum number of physical card types supported by FASTPATH.
*/
#define L7_MAX_SUPPORTED_PHYSICAL_CARD_TYPES                8



#define L7_PERSISTENT_LOG_SUPPORTED                         L7_TRUE
#define L7_SYSAPI_FEATURE_KEYING_SUPPORTED           L7_FALSE
#define L7_DHCPS_MAX_POOL_NUMBER                            16 
#define L7_DHCPS_MAX_LEASE_NUMBER                           256 

/* Max allowed DISCOVER messages for ping detection at a time */
#define L7_DHCPS_MAX_ASYNC_MSGS                              50

/*********************************************************************
**  End of Base paramters 
**********************************************************************/

/*********************************************************************
**  Start of Management paramters :- These should be renamed to 
                                     start with L7_*
**********************************************************************/

#define FD_SSHD_MAX_SESSIONS                5 
#define FD_TELNET_DEFAULT_MAX_SESSIONS      5
#define FD_HTTP_DEFAULT_MAX_CONNECTIONS     16
#define FD_CP_HTTP_DEFAULT_MAX_CONNECTIONS  32
#define FD_CLI_WEB_DEFAULT_NUM_SESSIONS     5
#ifdef L7_MGMT_SECURITY_PACKAGE
  #define FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS     16
  #define FD_CP_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS  0  /* no need for additional */
#else
  #define FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS     0
  #define FD_CP_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS  0
#endif
/*********************************************************************
**  End of Management paramters 
**********************************************************************/

/*********************************************************************
**  Start of L2 paramters 
**********************************************************************/
#define L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT                   4  /* initial traffic class mapping */
#define L7_DOT1P_DEFAULT_USER_PRIORITY                      0
#define L7_MAX_NUM_DYNAMIC_LAG                              8
#define L7_MAX_MULTIPLE_STP_INSTANCES                       31 
#define L7_MAX_VLAN_PER_BRIDGE                              4096
#define L7_MACLOCKING_MAX_STATIC_ADDRESSES                  20
#define L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES                 600
#define L7_PROTECTED_PORT_MAX_GROUPS                        3
#define L7_BRIDGE_SPEC_SUPPORTED                            L7_BRIDGE_SPEC_802DOT1S
#define L7_LLDP_MAX_ENTRIES_PER_PORT                        2 

/*********************************************************************
**  End of L2 paramters 
**********************************************************************/


/*********************************************************************
**  Start of QOS paramters 
**********************************************************************/

#define L7_ISCSI_MAX_TARGET_TCP_PORTS         16
#define L7_ISCSI_MAX_SESSIONS                 192
#define L7_ISCSI_MAX_CONNECTIONS              192

/*********************************************************************
**  End of QOS paramters 
**********************************************************************/


/*********************************************************************
**  Start of L3 paramters 
**********************************************************************/

#define L3INTF_VRRP_DEVICE_NAME                            "vr"
/*VRRP VRID definitions
*/
#define L7_PLATFORM_ROUTING_VRRP_MIN_VRID                  1
#define L7_PLATFORM_ROUTING_VRRP_MAX_VRID                  255
/*L3 Interface Network name prefix
*/
#define L3INTF_DEVICE_NAME                                 "rt"

#define L7_RTR_MAX_SUBNETS                                 min(20, L7_MAX_NUM_ROUTER_INTF)
#define L7_RTR_MAX_RTR_INTERFACES                          L7_MAX_NUM_ROUTER_INTF             
/*       The number of VLANs and VLAN IDs reserved for box management.
**       This value plus L7_MAX_VLAN_ID must be less or equal to 4095, which means
**       that a switch that supports the full VLAN range can only have one reserved
**       internal VLAN.
*/
#define L7_NUM_RESERVED_VLANS                              1
/*       The maximum VLAN ID that can be created by management or GVRP.
**       This value must be equal or larger than L7_MAX_VLANS.
**       This value must be less or equal to 4094.
*/
#define L7_PLATFORM_MAX_VLAN_ID                            (4094 - L7_NUM_RESERVED_VLANS)

/*********************************************************************
**  End of L3 paramters 
**********************************************************************/


/*********************************************************************
**  Start of IP Multicast paramters 
**********************************************************************/

/*
 This value specifies the timeperiod (in secs) the mroute entry stays in multicast 
 forwarding cache after forwarding the last multicast data packet  received 
 */
#define L7_MULTICAST_FIB_ENTRY_LIFETIME 210

/*
 This value specifies the timeperiod (in secs) the mroute entry stays in multicast 
 forwarding cache before the upcall to MRP to update the mroute entry responds 
 */
#define L7_MULTICAST_FIB_UPCALL_LIFETIME 10

/*********************************************************************
**  End of IP Multicast paramters 
**********************************************************************/

/*********************************************************************
**  Start of Wireless paramters 
**********************************************************************/
/* Maximum number of APs this device can manage in the peer group when
** it is acting as the WIDS Controller
*/
#define L7_WIRELESS_MAX_PEER_GROUP_ACCESS_POINTS    96

/* Maximum number of APs in the peer group with which this device 
** can co-exist when it is not acting as the WIDS Controller.
*/
#define L7_WIRELESS_MAX_PEER_GROUP_PARTICIPATION_APS  2000

/* Maximum number of switches in the peer group.
*/
#define L7_WIRELESS_MAX_PEER_SWITCHES     64

/* Maximum number of Access Points that can be managed by one switch.
*/
#define L7_WIRELESS_MAX_ACCESS_POINTS                       48

/* Maximu number of wireless clients in the peer group.
*/
#define L7_WIRELESS_MAX_CLIENTS                             4000

/* Maximum number of wireless clients that can be associated
** to one radio on the AP.
*/
#define L7_WIRELESS_MAX_CLIENTS_PER_RADIO                   200

/* Amount of memory reserved for storing Network Visulalization
** floor plans.
*/
#define L7_WIRELESS_NV_IMAGES_NVRAM_SIZE                    (1024*1000) /* 1M */

/* Amount of memory reserved for storing Captive Portal
** branding images.
*/
#define L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE                 (1024*1000) /* 1M */

/*
 * L3 Tunneling definitions
 */
#define L7_L3_TUNNEL_MAX_COUNT                              (L7_WIRELESS_MAX_PEER_SWITCHES + \
                                                             L7_WIRELESS_MAX_ACCESS_POINTS)

/* Maximum number of TSPEC traffic streams allowed 
** per wireless switch.
*/
#define L7_WIRELESS_TSPEC_TS_POOL_MAX                       (L7_WIRELESS_MAX_CLIENTS / 4)

/*********************************************************************
**  End of Wireless paramters 
**********************************************************************/


/*********************************************************************
**  Start of Chasis paramters 
**********************************************************************/
#define L7_MAX_LINE_MODULES_PER_UNIT                        0
#define L7_MAX_POWER_MODULES_PER_UNIT                       0
#define L7_MAX_FAN_MODULES_PER_UNIT                         0
#define L7_MAX_CONTROL_MODULES_PER_UNIT                     0
#define L7_MAX_PHYSICAL_SLOTS_PER_CHASSIS                   0
#define L7_MAX_SLOTS_PER_CHASSIS                            0
#define L7_MAX_STACK_PORTS_PER_CFM                          0
/*********************************************************************
**  End of Chasis paramters 
**********************************************************************/


/*********************************************************************
**  Start of Obsoleted paramters 
**********************************************************************/
/* MPLS table sizes (contained in NP FIB) */
#define L7_TS_MPLS_FTN_TBL_SIZE_TOTAL     128
#define L7_TS_MPLS_ILM_TBL_SIZE_TOTAL     128

/* Size of Network Processor Forwarding Information Base when Traffic Services
 * Flex Package is used.
 *
 * NOTE:  This definition should be used instead of L7_NP_FIB_TABLE_SIZE
 *        (defined above) when using the TS package.
 */
#define L7_TS_NP_FIB_TABLE_SIZE           (L7_NP_FIB_TABLE_SIZE + \
                                           L7_TS_MPLS_FTN_TBL_SIZE_TOTAL + \
                                           L7_TS_MPLS_ILM_TBL_SIZE_TOTAL)
/*********************************************************************
**  End of Obsoleted paramters 
**********************************************************************/
#endif /*__INC_L7_PRODUCTCONFIG_H__*/



/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename default_configurator.h
*
* @purpose All Factory default settings are in this file
*
* @component cnfgr
*
* @comments none
*
* @create 11/01/2004
*
* @author jpickering
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_IPV6_DEFAULT_CNFGR
#define INCLUDE_IPV6_DEFAULT_CNFGR
#include "cnfgr.h"

#define L7_IPV6_DEFAULT_STACK_SIZE  L7_DEFAULT_STACK_SIZE
#define L7_IPV6_DEFAULT_TASK_SLICE   L7_DEFAULT_TASK_SLICE


/* START IPV6 */
/* 1 referenced */
/* The types included here must be the same types allowed in ip6MapIntfIsValidType() */
#define L7_IP6MAP_INTF_MAX_COUNT (L7_MAX_PORT_COUNT    +  \
                                 L7_MAX_NUM_VLAN_INTF + \
                                 L7_MAX_NUM_LOOPBACK_INTF + \
                                 L7_MAX_NUM_TUNNEL_INTF + 1)


#define L7_IP6MAP_MAX_ASYNC_EVENTS           (L7_IP6MAP_INTF_MAX_COUNT)
#define L7_IP6MAP_MAX_ASYNC_EVENT_TIMEOUT    60000      /* milliseconds */

   
#define L7_RTOV6_MAX_PROTOCOLS  16

/* pool size for secondary addresses, total per all interfaces */
/* as defined allows each interface to have 4 global unicast addresses */

#define     L7_RTR6_MAX_SECONDARY_ADDRS (3*L7_RTR_MAX_RTR_INTERFACES)

/* The maximum number of interfaces in the system where OSPFv3 could
 * be enabled. This count may be greater than the max number of 
 * router interfaces. Thus, OSPFv3 can't be enabled on every one of 
 * these interfaces simultaneously. This is really the sum of the 
 * number of interfaces of each type where IPv6 can be enabled. */
#define L7_OSPFV3_INTF_MAX_COUNT    L7_IP6MAP_INTF_MAX_COUNT


#endif 

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename cnfgr.c
*
* @purpose Platform Configuration Database
*
* @component cnfgr
*
* @comments none
*
* @create 03/21/2003
*
* @author Andrey Tsigler
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "platform_config.h"
#include "l7_product.h"
#include "cnfgr.h"


/*********************************************************************
* @purpose  Get maximum number of interfaces supported by the switch.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfMaxCountGet (void)
{
  return L7_MAX_INTERFACE_COUNT;
}

/*********************************************************************
* @purpose  Maximum number of Units in a stack
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platUnitTotalMaxPerStackGet (void)
{
  return L7_MAX_UNITS_PER_STACK;
}

/*********************************************************************
* @purpose  Maximum number of ports per unit
*
* @param    void    
*
* @returns  
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 platUnitMaxPhysicalPortsGet (void)
{
  return L7_MAX_PHYSICAL_PORTS_PER_UNIT;
}



/*********************************************************************
* @purpose  Get maximum number of slots per unit.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotTotalMaxPerUnitGet (void)
{
  return L7_MAX_SLOTS_PER_UNIT;
}

/*********************************************************************
* @purpose  Get maximum number of Physical slots per unit.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotMaxPhysicalSlotsPerUnitGet (void)
{
  return L7_MAX_PHYSICAL_SLOTS_PER_UNIT;
}

/*********************************************************************
* @purpose  Get maximum number of ports per slot.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotMaxPortsPerSlotGet (void)
{
  return L7_MAX_PORTS_PER_SLOT;
}

/*********************************************************************
* @purpose  Get maximum number of Physical ports per slot.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotMaxPhysicalPortsPerSlotGet (void)
{
  return L7_MAX_PHYSICAL_PORTS_PER_SLOT;
}


/*********************************************************************
* @purpose  Get maximum number of router interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfRouterMaxCountGet (void)
{
  return L7_MAX_NUM_ROUTER_INTF;
}

/*********************************************************************
* @purpose  Get the LAG slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotLagSlotNumGet (void)
{
  return L7_LAG_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the VLAN slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotVlanSlotNumGet (void)
{
  return L7_VLAN_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the CPU slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotCpuSlotNumGet (void)
{
  return L7_CPU_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the loopback slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotLoopbackSlotNumGet (void)
{
  return L7_LOOPBACK_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the tunnel slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotTunnelSlotNumGet (void)
{
  return L7_TUNNEL_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the wireless network slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotwirelessNetSlotNumGet (void)
{
    return L7_WIRELESS_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the wireless network slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotL2TunnelSlotNumGet (void)
{
    return L7_CAPWAP_TUNNEL_SLOT_NUM;
}

/*********************************************************************
* @purpose  Get the maximum number of interfaces for the device.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfTotalMaxCountGet (void)
{
  return L7_MAX_INTERFACE_COUNT;
}

/*********************************************************************
* @purpose  Get the default stack size for tasks in this platform.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskStackDefaultGet (void)
{
  return L7_DEFAULT_STACK_SIZE;
}

/*********************************************************************
* @purpose  Get the default task slice for tasks in this platform.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskSliceDefaultGet (void)
{
  return L7_DEFAULT_TASK_SLICE;
}

/*********************************************************************
* @purpose  Get the default task priority for tasks in this platform.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskPriorityDefaultGet (void)
{
  return L7_DEFAULT_TASK_PRIORITY;
}

/*********************************************************************
* @purpose  Get the medium task priority for tasks in this platform.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskPriorityMediumGet (void)
{
  return L7_MEDIUM_TASK_PRIORITY;
}

/*********************************************************************
* @purpose  Get the maximum number of FDB entries.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platFdbTotalMacEntriesGet (void)
{
  return L7_MAX_FDB_MAC_ENTRIES;
}

/*********************************************************************
* @purpose  Get the maximum number of static FDB entries.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platFdbStaticMaxEntriesGet (void)
{
  return L7_MAX_FDB_STATIC_FILTER_ENTRIES;
}

/*********************************************************************
* @purpose  Get the maximum number of Multicast FDB MAC entries.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platMfdbTotalMaxEntriesGet (void)
{
  return L7_MFDB_MAX_MAC_ENTRIES;
}

/*********************************************************************
* @purpose  Get the maximum number of VLANs supported by the switch.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 platVlanTotalMaxEntriesGet (void)
{
  return L7_MAX_VLANS;
}

/*********************************************************************
* @purpose  Get the maximum VLAN ID supported by the switch.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platVlanVlanIdMaxGet (void)
{
  return L7_PLATFORM_MAX_VLAN_ID;
}

/*********************************************************************
* @purpose  Get the maximum number of ARP cache entries.
*
* @param    void
*
* @returns
*
* @notes    This represents the device ARP cache size.  This is not necessarily
*           the same maximum that can be configured by the user, since certain
*           maintenance entries are needed.  See L7_IP_ARP_CACHE_SIZE_MAX
*           (and _MIN) for the user configurable upper (and lower) limit.
*
* @end
*********************************************************************/
L7_uint32 platRtrArpMaxEntriesGet (void)
{
  return L7_L3_ARP_CACHE_SIZE;
}

/*********************************************************************
* @purpose  Get the maximum route table size.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platRtrRouteMaxEntriesGet (void)
{
  return L7_L3_ROUTE_TBL_SIZE_TOTAL;
}

#ifdef L7_IPV6_PACKAGE
/*********************************************************************
  * @purpose  Get the maximum IPV6 route table size.
  *
  * @param    void
  *
  * @returns
  *
  * @notes    none
  *
  * @end
  *********************************************************************/
L7_uint32 platRtrIpv6RouteMaxEntriesGet (void)
{
    return L7_IPV6_ROUTE_TBL_SIZE_TOTAL;
}
#endif

/*********************************************************************
* @purpose  Get the size of routing heap in bytes.
*
* @param    void
*
* @returns
*
* @notes    Because of the increasing number of build combinations, this
*           number is now computed here rather than using a fixed value. 
*           The computation is based on the application note "Sizing the 
*           Routing Heap in FASTPATH." 
*
* @end
*********************************************************************/
L7_uint32 platRtrVirataRoutingHeapSizeGet (void)
{
#ifdef L7_ROUTING_PACKAGE
  /* This allocation covers fixed memory allocation needs for ARP (those 
     allocations that are not tied to the ARP cache size. */
  const L7_uint32 fixedArpAllocation = 150000;

  /* The number of bytes per ARP entry */
  const L7_uint32 arpScaleFactor = 300;

#ifdef L7_OSPF_PACKAGE
  /* Fixed allocation when OSPFv2 is included. 1.5 MBytes. */
  const L7_uint32 fixedOspfv2Allocation = 1500000;

  /* Number of bytes per OSPFv2 route. 2750 bytes per route. */
  const L7_uint32 ospfv2ScaleFactor = 2750;
#endif /* L7_OSPF_PACKAGE */
#endif /* L7_ROUTING_PACKAGE */

#ifdef L7_IPV6_PACKAGE
  /* Fixed allocation when OSPFv3 is included. 1.5 MBytes. */
  const L7_uint32 fixedOspfv3Allocation = 1500000;

  /* Number of bytes per OSPFv3 route. 3250 bytes per route. */
  const L7_uint32 ospfv3ScaleFactor = 3250;
#endif   /* L7_IPV6_PACKAGE */

#ifdef L7_BGP_PACKAGE
  /* Fixed allocation for BGP. 1.0 MB */
  const L7_uint32 fixedBgpAllocation = 1000000;

  /* Number of bytes per BGP route. 500 bytes per route. */
  const L7_uint32 bgpScaleFactor = 500;

  /* Maximum number of BGP routes. Even if we support more IPv4 routes, 
   * we only support up to 1000 BGP routes. BGP doesn't srictly enforce
   * this limit, but we do not support more and do not allocate memory
   * for more. */
  L7_uint32 bgpRoutes = 1000;
#endif /* L7_BGP_PACKAGE */

  L7_uint32 arpMem = 0;
  L7_uint32 ospfv2Mem = 0;
  L7_uint32 ospfv3Mem = 0;
  L7_uint32 bgpMem = 0;
  L7_uint32 totalMem;

#ifdef L7_ROUTING_PACKAGE
  arpMem = fixedArpAllocation + arpScaleFactor * L7_L3_ARP_CACHE_SIZE;

#ifdef L7_OSPF_PACKAGE
  ospfv2Mem = fixedOspfv2Allocation + ospfv2ScaleFactor * L7_L3_ROUTE_TBL_SIZE_TOTAL;
#endif   /* L7_OSPF_PACKAGE */
#endif   /* L7_ROUTING_PACKAGE */

#ifdef L7_IPV6_PACKAGE
  ospfv3Mem = fixedOspfv3Allocation + ospfv3ScaleFactor * L7_IPV6_ROUTE_TBL_SIZE_TOTAL;
#endif 

#ifdef L7_BGP_PACKAGE
  if (L7_L3_ROUTE_TBL_SIZE_TOTAL < bgpRoutes)
    bgpRoutes = L7_L3_ROUTE_TBL_SIZE_TOTAL;
  bgpMem = fixedBgpAllocation + bgpScaleFactor * bgpRoutes;
#endif

  totalMem = arpMem + ospfv2Mem + ospfv3Mem + bgpMem;
  return totalMem;
}

/*********************************************************************
* @purpose  Get the number of equal cost routes supported by this
*           platform.
*
* @param    void
*
* @returns
*
* @notes    The 5690 and 5695 share l3_platform.h, which defines
*           L7_RT_MAX_EQUAL_COST_ROUTES. However, the 5690 does not
*           support ECMP. So applications which query the max number of
*           next hops supported on a platform need to call this function
*           instead of using L7_RT_MAX_EQUAL_COST_ROUTES directly.
*
* @end
*********************************************************************/
L7_uint32 platRtrRouteMaxEqualCostEntriesGet (void)
{
    if (cnfgrBaseTechnologySubtypeGet() ==
        L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5690)
    {
        return 1;
    }
    return L7_RT_MAX_EQUAL_COST_ROUTES;
}

/*********************************************************************
* @purpose  Maximum number of IP multicast routing entries.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platMrtrRoutesMaxEntriesGet (void)
{
  return L7_MULTICAST_FIB_MAX_ENTRIES;
}

/*********************************************************************
* @purpose  Maximum number of Vlan Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfVlanIntfMaxCountGet (void)
{
  return L7_MAX_NUM_VLAN_INTF;
}

/*********************************************************************
* @purpose  Maximum number of Loopback Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfLoopbackIntfMaxCountGet (void)
{
  return L7_MAX_NUM_LOOPBACK_INTF;
}

/*********************************************************************
* @purpose  Maximum number of Tunnel Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfTunnelIntfMaxCountGet (void)
{
  return L7_MAX_NUM_TUNNEL_INTF;
}

/*********************************************************************
* @purpose  Maximum number of Wireless Network Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfwirelessNetIntfMaxCountGet (void)
{
  return L7_MAX_NUM_WIRELESS_INTF;
}

/*********************************************************************
* @purpose  Maximum number of L2 Tunnel Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfL2TunnelIntfMaxCountGet (void)
{
  return L7_MAX_NUM_CAPWAP_TUNNEL_INTF;
}

/*********************************************************************
* @purpose  Maximum number of CPU Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfCpuIntfMaxCountGet (void)
{
  return L7_MAX_CPU_SLOTS_PER_UNIT;
}

/*********************************************************************
* @purpose  Maximum number of Physical Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfPhysicalIntfMaxCountGet (void)
{
  return L7_MAX_PORT_COUNT;
}
/*********************************************************************
* @purpose  Maximum number of Lag Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfLagIntfMaxCountGet (void)
{
  return L7_MAX_NUM_LAG_INTF;
}

/*********************************************************************
* @purpose  Maximum number of Dynamic Lag Interfaces
*
* @param    void    
*
* @returns  
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 platIntfDynamicLagIntfMaxCountGet (void)
{
  return L7_MAX_NUM_DYNAMIC_LAG;
}

/*********************************************************************
* @purpose  Maximum number of Stack Interfaces
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfStackIntfMaxCountGet (void)
{
  return L7_MAX_NUM_STACK_INTF;
}

/*********************************************************************
* @purpose  Maximum number of Spanning Tree Instances
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platDot1sInstancesMaxEntriesGet(void)
{
  return L7_MAX_MULTIPLE_STP_INSTANCES;
}


/*********************************************************************
* @purpose  Maximum number of Peer Wireless Switches (WSs)
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

L7_uint32 platWirelessPeerSwitchesMaxCountGet(void)
{

  return L7_WIRELESS_MAX_PEER_SWITCHES;
}

/*********************************************************************
* @purpose  Maximum number of Access Points (APs) managed by a single
*           Wireless Switch (WS)
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

L7_uint32 platWirelessAccessPointsMaxCountGet(void)
{

  return L7_WIRELESS_MAX_ACCESS_POINTS;
}

/*********************************************************************
* @purpose  Maximum number of Access Points (APs) managed by a
*           the Wireless Peer Group
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

L7_uint32 platWirelessPeerGroupAccessPointsMaxCountGet(void)
{

  return L7_WIRELESS_MAX_PEER_GROUP_ACCESS_POINTS;
}

/*********************************************************************
* @purpose  Maximum number of Clients supported by the peer switch
*           group
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

L7_uint32 platWirelessClientsMaxCountGet(void)
{

  return L7_WIRELESS_MAX_CLIENTS;
}

/*************************************************************************
* @purpose  Maximum number of TSPEC Traffic Streams supported by a single
*           Wireless Switch (WS)
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*************************************************************************/

L7_uint32 platWirelessTspecTrafficStreamMaxCountGet(void)
{

  return L7_WIRELESS_TSPEC_TS_POOL_MAX;
}

/*********************************************************************
* @purpose  An outcall that provides location and size of vendor heap area
*           for the IP stack
*
* @param    *pHeapStart @b{(output)} output location to store heap start addr
* @param    *pHeapSize  @b{(output)} output location to store heap size
*
* @returns  none
*
* @notes    Setting *pHeapStart to L7_NULLPTR indicates there is no
*           pre-determined location for the heap, so it should be allocated
*           from the kernel's heap space.
*
* @notes    Setting *pHeapSize to 0 means no vendor heap space is required
*           for this platform.  For pre-allocated space, this heap size
*           must allow for some management overhead in its defined value.
*
* @end
*********************************************************************/
void vendorHeapIpInfoGet ( L7_uchar8 **pHeapStart, L7_uint32 *pHeapSize )
{
  *pHeapStart = L7_NULLPTR;
  *pHeapSize  = platRtrVirataRoutingHeapSizeGet();
}


/*********************************************************************
* @purpose  Maximum number of Wireless Tunnels
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

L7_uint32 platWirelessTunnelsMaxCountGet(void)
{

  return L7_WIRELESS_MAX_PEER_SWITCHES + L7_WIRELESS_MAX_ACCESS_POINTS;
}

/*********************************************************************
* @purpose  Maximum number of Wireless L2 Tunnels
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/

L7_uint32 platWirelessL2TunnelsMaxCountGet(void)
{

  return L7_MAX_NUM_CAPWAP_TUNNEL_INTF;
}


/*********************************************************************
* @purpose  Get the maximum number of power modules per unit
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platPowerModulesTotalMaxPerUnitGet(void)
{
  return L7_MAX_POWER_MODULES_PER_UNIT;
}

/*********************************************************************
* @purpose  Get the maximum number of Fan modules per unit
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platFanModulesTotalMaxPerUnitGet(void)
{
  return L7_MAX_FAN_MODULES_PER_UNIT;
}

/*********************************************************************
* @purpose  Get the maximum number of Line modules per unit
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platLineModulesTotalMaxPerUnitGet(void)
{
  return L7_MAX_LINE_MODULES_PER_UNIT;
}

/*********************************************************************
* @purpose  Get the maximum number of Control modules per unit
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platControlModulesTotalMaxPerUnitGet(void)
{
  return L7_MAX_CONTROL_MODULES_PER_UNIT;
}

/*********************************************************************
* @purpose  Get the maximum number of physical slots per chassis
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
**********************************************************************/
L7_uint32 platPhysicalSlotsTotalMaxPerChassisGet(void)
{
  return L7_MAX_PHYSICAL_SLOTS_PER_CHASSIS;
}

/*********************************************************************
*  @purpose  Get the maximum number of slots per chassis
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
**********************************************************************/
L7_uint32 platSlotsTotalMaxPerChassisGet(void)
{
  return L7_MAX_SLOTS_PER_CHASSIS;
}

/*********************************************************************
*  @purpose  Get the maximum number of Stack ports per CFM
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
**********************************************************************/
L7_uint32 platMaxStackPortsPerChassisCFMGet(void)
{
  return L7_MAX_STACK_PORTS_PER_CFM;
}

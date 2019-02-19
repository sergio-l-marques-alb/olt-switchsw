#ifndef PLATFORM_CONFIG_H_INCLUDED
#define PLATFORM_CONFIG_H_INCLUDED
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename fastpath_config.h
*
* @purpose FASTPATH configuration database.
*
* @component cnfgr
*
* @comments none
*
* @create 03/19/2003
*
* @author Andrey Tsigler
*
* @end
*
**********************************************************************/
#include "l7_common.h"


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
L7_uint32 platIntfMaxCountGet (void);

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
L7_uint32 platUnitTotalMaxPerStackGet (void);

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
L7_uint32 platUnitMaxPhysicalPortsGet (void);

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
L7_uint32 platSlotTotalMaxPerUnitGet (void);

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
L7_uint32 platSlotMaxPhysicalSlotsPerUnitGet (void);

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
L7_uint32 platSlotMaxPortsPerSlotGet (void);

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
L7_uint32 platSlotMaxPhysicalPortsPerSlotGet (void);

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
L7_uint32 platIntfRouterMaxCountGet (void);

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
L7_uint32 platSlotLagSlotNumGet (void);

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
L7_uint32 platSlotVlanSlotNumGet (void);

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
L7_uint32 platSlotCpuSlotNumGet (void);

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
L7_uint32 platSlotLoopbackSlotNumGet (void);

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
L7_uint32 platSlotTunnelSlotNumGet (void);

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
L7_uint32 platSlotwirelessNetSlotNumGet (void);

/*********************************************************************
* @purpose  Get the l2 tunnel slot number
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotL2TunnelSlotNumGet (void);

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
L7_uint32 platIntfTotalMaxCountGet (void);

/*********************************************************************
* @purpose  Get the default stack size for tasks in this platform.;
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskStackDefaultGet (void);

/*********************************************************************
* @purpose  Get the default task slice for tasks in this platform.;
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskSliceDefaultGet (void);

/*********************************************************************
* @purpose  Get the default task priority for tasks in this platform.;
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskPriorityDefaultGet (void);

/*********************************************************************
* @purpose  Get the medium task priority for tasks in this platform.;
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platTaskPriorityMediumGet (void);

/*********************************************************************
* @purpose  Get the service port interface name.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *platIntfServicePortNameGet (void);

/*********************************************************************
* @purpose  Get the service port interface name length.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platIntfServicePortNameLengthGet (void);

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
L7_uint32 platFdbTotalMacEntriesGet (void);

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
L7_uint32 platFdbStaticMaxEntriesGet (void);

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
L7_uint32 platMfdbTotalMaxEntriesGet (void);

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
L7_ushort16 platVlanTotalMaxEntriesGet (void);

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
L7_uint32 platVlanVlanIdMaxGet (void);

/*********************************************************************
* @purpose  Get the default baud rate for the serial port.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSerialBaudRateDefaultGet (void);

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
L7_uint32 platRtrArpMaxEntriesGet (void);

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
L7_uint32 platRtrRouteMaxEntriesGet (void);

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
L7_uint32 platRtrIpv6RouteMaxEntriesGet (void);

/*********************************************************************
* @purpose  Get the size of Virata heap.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platRtrVirataRoutingHeapSizeGet (void);

/*********************************************************************
* @purpose  Get the number of equal cost routes supported by this
*           platform.
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platRtrRouteMaxEqualCostEntriesGet (void);

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
L7_uint32 platMrtrRoutesMaxEntriesGet (void);

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
L7_uint32 platIntfVlanIntfMaxCountGet (void);

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
L7_uint32 platIntfLoopbackIntfMaxCountGet (void);

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
L7_uint32 platIntfTunnelIntfMaxCountGet (void);

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
L7_uint32 platIntfwirelessNetIntfMaxCountGet (void);

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
L7_uint32 platIntfL2TunnelIntfMaxCountGet (void);

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
L7_uint32 platIntfCpuIntfMaxCountGet (void);

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
L7_uint32 platIntfPhysicalIntfMaxCountGet (void);

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
L7_uint32 platIntfLagIntfMaxCountGet (void);

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
L7_uint32 platIntfDynamicLagIntfMaxCountGet (void);

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
L7_uint32 platIntfStackIntfMaxCountGet (void);

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
L7_uint32 platDot1sInstancesMaxEntriesGet(void);

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
L7_uint32 platWirelessPeerSwitchesMaxCountGet(void);

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
L7_uint32 platWirelessAccessPointsMaxCountGet(void);

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
L7_uint32 platWirelessPeerGroupAccessPointsMaxCountGet(void);

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
L7_uint32 platWirelessClientsMaxCountGet(void);

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

L7_uint32 platWirelessTspecTrafficStreamMaxCountGet(void);

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
L7_uint32 platWirelessTunnelsMaxCountGet(void);

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
L7_uint32 platWirelessL2TunnelsMaxCountGet(void);


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
void vendorHeapIpInfoGet ( L7_uchar8 **pHeapStart, L7_uint32 *pHeapSize );

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
L7_uint32 platPowerModulesTotalMaxPerUnitGet(void);


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
L7_uint32 platFanModulesTotalMaxPerUnitGet(void);

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
L7_uint32 platLineModulesTotalMaxPerUnitGet(void);

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
L7_uint32 platControlModulesTotalMaxPerUnitGet(void);

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
*********************************************************************/
L7_uint32 platPhysicalSlotsTotalMaxPerChassisGet(void);

/*********************************************************************
* @purpose  Get the maximum number of slots per chassis
*
* @param    void
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 platSlotsTotalMaxPerChassisGet(void);

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
***********************************************************************/
L7_uint32 platMaxStackPortsPerChassisCFMGet(void);

#endif

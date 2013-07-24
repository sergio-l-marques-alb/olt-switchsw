/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename   ipstk_api.h
*
* @purpose    ipstack Mapping layer external function prototypes
*
* @component  ipstack Mapping Layer
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/




#ifndef INCLUDE_IPSTK_API
#define INCLUDE_IPSTK_API


#include <l7_common.h>
#include "osapi_sockdefs.h"
#include "nimapi.h"
#include "ipstk_mib_api.h"

/* netlink msg types */
typedef enum{
  L7_NLMSG_UNKNOWN     = 0,
  L7_NLMSG_NDADD       = 1,
  L7_NLMSG_NDDEL       = 2,
  L7_NLMSG_IFADDR_ADD  = 3,
  L7_NLMSG_IFADDR_DEL  = 4
}L7_NLMSG_TYPES;


/*********************************************************************
* @purpose  initialize the IP stack mapping layer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkInit(void);

/*********************************************************************
* @purpose  Gets an ipv4 interface address from the stack
*
* @param    ifname      interface name
* @param    addr        pointer to address
*
* @returns  L7_SUCCESS, L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfAddrGet( L7_uchar8 *ifname, L7_uint32 *addr);


/*********************************************************************
* @purpose  Set the primary ipv4 address in stack
*
* @param    ifname       interface name
* @param    addr         interface address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfAddrSet( L7_uchar8 *ifname, L7_uint32 addr);


/*********************************************************************
* @purpose  Delete the primary ipv4 address in stack
*
* @param    ifname       interface name
* @param    addr         interface address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfAddrDel( L7_uchar8 *ifname, L7_uint32 addr);

/*********************************************************************
* @purpose  Gets the ipv4 interface mask from the stack
*
* @param    ifname      interface name
* @param    addr        pointer to mask
*
* @returns  L7_SUCCESS, L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfMaskGet( L7_uchar8 *ifname, L7_uint32 *addr);

/*********************************************************************
* @purpose  Set the primary ipv4 address mask in stack
*
* @param    ifname       interface name
* @param    addr         interface mask
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkIfMaskSet( L7_uchar8 *ifname, L7_uint32 addr);


/*********************************************************************
* @purpose  Gets an interface metric
*
* @param    ifname     interface name
* @param    metric     pointer to metric 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    currently always 0
*
* @end
*********************************************************************/
L7_RC_t ipstkIfMetricGet(L7_uchar8 *ifname, L7_uint32 *metric);


/*********************************************************************
* @purpose  Add an ipv6 address/prefixlen to stack
*
* @param    ifname       interface name
* @param    addr         pointer to address
* @param    prefixLen    address prefix length
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    no special status of primary address
*
* @end
*********************************************************************/
L7_RC_t ipstkNetAddrV6Add(L7_uchar8 *ifname,
                L7_in6_addr_t  *addr, L7_uint32 prefixlen);


/*********************************************************************
* @purpose  Delete an ipv6 address from stack
*
* @param    ifname       interface name
* @param    addr         pointer to address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    prefixLen not required
*
* @end
*********************************************************************/
L7_RC_t ipstkNetAddrV6Del( L7_uchar8 *ifname, L7_in6_addr_t *addr, L7_uint32 prefixLen);


/*********************************************************************
* @purpose  tell stack non-router interface is up.
* located here so caller doesnt need to include interpeak
*
* @param    ifname          ascii interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkMgmtIfUp(L7_uchar8 *ifname);


/*********************************************************************
* @purpose  tell stack non-router interface is down.
*
* @param    ifname          ascii interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkMgmtIfDown(L7_uchar8 *ifname);

/*********************************************************************
* @purpose  get stack's interface index
*
* @param    ifname          ascii interface name
* @param    ifIndex         interface index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkStackIfIndexGet(L7_uchar8 *ifname, L7_uint32 *ifIndex);


/*********************************************************************
* @purpose  initialize l3 shared lib
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRoutingSharedLibInit(void);

/*********************************************************************
* @purpose  allocate a shared router interface num from an intIfNum
*
* @param    intIfNum        Internal Interface Number
* @param    compId          componentId of caller
* @param    rtrIfNum        returned router interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfNumGet(L7_uint32 intIfNum,L7_uint32 compId, L7_uint32 *rtrIfNum);


/*********************************************************************
* @purpose  deallocate a shared router interface num from an intIfNum
*
* @param    rtrIfNum        router Interface Number
* @param    compId          componentId of caller
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfNumRelease(L7_uint32 rtrIfNum,L7_uint32 compId);

/*********************************************************************
* @purpose  tell stack router interface is up.
*
* @param    rtrIfNum        router Interface Number
* @param    compId          componentId of caller
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfUp(L7_uint32 rtrIfNum,L7_uint32 compId);


/*********************************************************************
* @purpose  tell stack router interface is down.
*
* @param    rtrIfNum        router Interface Number
* @param    compId          componentId of caller
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfDown(L7_uint32 rtrIfNum,L7_uint32 compId);


/*********************************************************************
* @purpose  Convert Internal Interface Number to Router Interface Number
*
* @param    intIfNum   internal interface number
* @param    *rtrIntf   pointer to router interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
*
* @end
*********************************************************************/
L7_RC_t ipstkIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum);

/*********************************************************************
* @purpose  check status of interface
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    usable only for router interfaces
*
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfIsUp(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  check status of interface
*
* @param    ifName      interface name
* @param    val         ptr to boolean, L7_TRUE if interface is up
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    usable for all interfaces
*
*
* @end
*********************************************************************/
L7_RC_t ipstkIfIsUp(L7_uchar8 *ifName, L7_BOOL *val);

/*********************************************************************
*
* @purpose  Notify NIM of an interface change. IP MAP tells NIM when
*           routing is enabled or disabled on an interface.
*
* @param    intIfNum  @b{(input)}  internal interface number
* @param    event     @b{(input)}  event from L7_PORT_EVENTS_t
* @parma    callback  @b{(input)}  callback function pointer. If non-NULL,
*                                  NIM will call this function when the
*                                  event is complete.
* @param    handle    @b{(output)} identifies the event in case the caller
*                                  requires notification of event completion
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkIntfChangeNimNotify(L7_uint32 intIfNum, L7_uint32 event,
                                 NIM_NOTIFY_CB_FUNC_t callback,
                                 NIM_HANDLE_t *handle,L7_uint32 compId);


/*********************************************************************
*
* @purpose  Notify DTL of an interface change. IP MAP tells DTL when
*           routing is enabled or disabled on an interface.
*
* @param    intIfNum  @b{(input)}  internal interface number
* @param    event     @b{(input)}  event from L7_ENABLE or DISABLE
*
* @returns  L7_RC_t
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipstkIntfChangeDtlNotify(L7_uint32 intIfNum, L7_uint32 event,
                                 L7_uint32 compId);

/*********************************************************************
* @purpose  convert statck ifIndex to intIfNum
*
* @parma    ifIndex   @b{(input)}  interface index
* @parma    intIfNum  @b{(output)}  internal interface
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    applies only to router interfaces
*
* @end
*********************************************************************/
L7_RC_t ipstkStackIfIndexToIntIfNum(L7_uint32 ifIndex,L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  convert intIfNum to stack ifIndex 
*
* @parma    ifIndex   @b{(input)}  interface index
* @parma    intIfNum  @b{(output)}  internal interface
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    applies only to router interfaces
*
* @end
*********************************************************************/
L7_RC_t ipstkIntIfNumToStackIfIndex(L7_uint32 intIfNum,L7_uint32 *ifIndex);


/*********************************************************************
* @purpose  Get the state of a network interface
*
* @param    ifname       name of interface
* @param    flag         name of flag to change
* @param    val          set flag if L7_TRUE, else clear
*
* @returns  L7_SUCCES,L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipstkIfFlagGet(L7_uchar8 *ifname, L7_uint32 flag, L7_BOOL *val);

/*********************************************************************
* @purpose  modify the state of a network interface
*
* @param    ifname       name of interface
* @param    flag         name of flag to change
* @param    val          set flag if L7_TRUE, else clear
*
* @returns  L7_SUCCES,L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
ipstkIfFlagChange(L7_uchar8 *ifname, L7_uint32 flag, L7_BOOL val);

/*********************************************************************
* @purpose  Manage the default gateway in the IP stack's routing table. 
*
* @param    none          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    FASTPATH can learn a default gateway for the service port, the
*           network port, or a routing interface. The IP stack can only 
*           have a single default gateway. (The stack may accept multiple
*           default routes, but if we let that happen we may end up with load
*           balancing across the network and service port or some other 
*           combination we don't want.) RTO may report an ECMP default route.
*           We only give the IP stack a single next hop in this case, since
*           it's not likely we need to additional capacity provided by load
*           sharing for packets originating on the box. 
*
*           This function is called whenever an event could cause a change
*           in the default route that should be installed in the stack's 
*           routing table. Events include user configuration of service
*           port or network port, learning a default gateway through DHCP,
*           management interface up or down, or a default route change in 
*           RTO. This function determines what if any default route 
*           should be installed given the current system state and gives
*           that gateway to the stack. 
* 
*           The precedence of default gateways is as follows:
*                 - via routing interface
*                 - via service port
*                 - via network port
*
* @end
*********************************************************************/
L7_RC_t ipstkDefGwUpdate(void);

/*********************************************************************
* @purpose  Manage the IPV6 default gateway in the IP stack's routing table. 
*
* @param    none          
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    FASTPATH can learn a default gateway for the service port, the
*           network port, or a routing interface. The IP stack can only 
*           have a single default gateway. (The stack may accept multiple
*           default routes, but if we let that happen we may end up with load
*           balancing across the network and service port or some other 
*           combination we don't want.) RTO may report an ECMP default route.
*           We only give the IP stack a single next hop in this case, since
*           it's not likely we need to additional capacity provided by load
*           sharing for packets originating on the box. 
*
*           This function is called whenever an event could cause a change
*           in the default route that should be installed in the stack's 
*           routing table. Events include user configuration of service
*           port or network port, learning a default gateway through DHCP,
*           management interface up or down, or a default route change in 
*           RTO. This function determines what if any default route 
*           should be installed given the current system state and gives
*           that gateway to the stack. 
* 
*           The precedence of default gateways is as follows:
*                 - via routing interface
*                 - via service port
*                 - via network port
*
* @end
*********************************************************************/
L7_RC_t ipstkIpv6DefGwUpdate(void);

/*********************************************************************
* @purpose  set unnumbered flag for an interface.
*
* @param    ifname          ascii interface name
* @param    flag            enable/disable unnumbered flag (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfUnnumbSet(L7_uchar8 *ifname, L7_BOOL flag);

/*********************************************************************
* @purpose  Get unnumbered flag for an interface
*
* @param    ifname          ascii interface name
* @param    flag            unnumbered interface flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipstkRtrIfUnnumbGet(L7_uchar8 *ifname, L7_BOOL *flag);


/*********************************************************************
* @purpose  Get the internal VLAN assigned to a port based routing interface
*
* @param    intIfNum		internal interface number of port based routing interface
*
* @returns  non-zero if VLAN ID found
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_uint32 ipstkInternalVlanIdGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the internal interface number associated with the 
*           port-based routing interface corresponding to this 
*          internal VLAN
*
* @param    vlanid			id of the vlan
* @param    intIfNum		Internal interface number of port-based routing interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if VLAN ID does not correspond to a port-based routing interface
*
* @notes    Cannot be used to find the intIfNum for a VLAN ID used for
*           a VLAN routing interface.
*                                                   
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanIdToRtrIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the port-based routing interface for the given assigned VLANID
*
* @param    vlanId VLAN ID
*
* @returns  non-zero if Routing interface is found.
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_uint32 ipstkInternalVlanRoutingInterfaceGet(L7_uint32 vlanId);

/*********************************************************************
* @purpose  Find the first VLAN ID (lowest numerical ID) claimed for internal use
*           for a port-based routing interface
*
* @param    vlanId          vlanId 
* @param    intIfNum        corresponding interface
*
* @returns  L7_SUCCESS if a VLAN ID found
* @returns  L7_FAILURE if no VLAN IDs used internally                         
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Find the next VLAN ID (in numeric order) claimed for internal use
*           for a port-based routing interface
*
* @param    prevVid      previous VLAN ID
* @param    vlanId       next vlanId 
* @param    intIfNum     interface corresponding to next VLAN ID
*
* @returns  L7_SUCCESS if next VLAN ID found
* @returns  L7_FAILURE if no more VLAN IDs used internally                         
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t ipstkInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum);
#endif /*INCLUDE_IPSTK_API*/



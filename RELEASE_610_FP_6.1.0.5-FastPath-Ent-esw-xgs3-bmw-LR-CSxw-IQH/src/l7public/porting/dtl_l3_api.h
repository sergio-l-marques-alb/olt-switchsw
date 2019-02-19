
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename   dtl_l3_api.h
 *
 * @purpose    This files contains the the necessary prototypes,
 *             for the layer3 Dtl calls
 *
 * @component  Device Transformation Layer
 *
 * @comments   none
 *
 * @create     3/19/2001
 *
 * @author     asuthan
 * @end
 *
 **********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_DTL_L3_API_H
#define INCLUDE_DTL_L3_API_H

#include <l3_comm_structs.h>
#include "l7_ip6_api.h"
#include "l7_rto6_api.h"
#include "dapi.h"

/*
********************************************************************
*                         DTL LAYER3 STRUCTURES
********************************************************************
*/

/* DTL IP Circuit(Router Interface) Configuration */

/* dtlRtrIntf_t Implementation Note:  

Two elements must describe the IP circuit (router interface): a
vlan ID and an internal interface number.  The internal interface
number must be the internal interface number of a physical port,
lag, or other port which is eligible to become a member of a VLAN.
The internal interface number must be 0 if the interface is a VLAN
which is configured for routing.
   
If multinetting occurs, intIfNum must be the intIfNum associated
with the port which is being multinetted, not an intIfNum caused by
the creation of a new circuit due to multinetting.
*/   

/*********************************************************************
 * @purpose This structure defines routing interface identification
 * @purpose parameters
 *
 * @notes         
 * 
 *********************************************************************/
typedef struct dtlRtrIntf_s
{
  L7_ushort16       vlanId;                /* VLAN ID, if intf is a VLAN   */
  L7_uint32       intIfNum;                /* Internal Interface Number    */

} dtlRtrIntf_t;

/*********************************************************************
 * @purpose This structure defines all the parameters to describe a
 * @purpose routing interface
 *
 * @notes         
 * 
 *********************************************************************/
typedef struct dtlRtrIntfDesc_s
{
  dtlRtrIntf_t    ipCircuit;                 /* circuit identification       */
  L7_uint32       ipAddress;                 /* IP Address                   */
  L7_uint32       subnetMask;                /* network mask                 */
  L7_ENCAPSULATION_t llEncapsType;             
  L7_uint32       mtu;                       /* interface MTU. Not IP MTU    */
  L7_uint32       flags;                     /* Flags                        */
#define DTL_IP_CIRC_ROUTING_ENABLED     0x01 /*                              */
#define DTL_IP_CIRC_BROADCAST_CAPABLE   0x02 /* Broadcast Capable Interface  */

  /* (i.e. LAN, not Point-to-point*/
#define DTL_IP_CIRC_NET_DIR_BCAST_FWD   0x04 /* Forward net-directed bcasts  */

  /* on this circuit              */
#define DTL_IP_CIRC_MCAST_FWD           0x08 /* Multicast Forwarding enabled */

} dtlRtrIntfDesc_t;

/*
  /------------------------------------------------------------------\
  *           Protocol independent APIs                              *
  \------------------------------------------------------------------/
*/

/*********************************************************************
 * @purpose  Enables or disables the routing capability for this 
 * @purpose  particular router interface
 *
 * @param intIfNum @b{(input)} The internal interface number for this
 * @param                      router interface
 * @param vlanid @b{(input)}   VLAN ID for port based routing interface
 * @param mode @b{(input)} L7_ENABLE, to enable routing capability in
 * @param this router interface or L7_DISABLE, to disable routing capability
 * @param in this router interface
 *
 * @returns  L7_SUCCESS  on a successful operation 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlRtrIntfModeSet(L7_uint32 intIfNum, L7_uint32 vlanId, 
                                  L7_uint32 mode);

/*********************************************************************
 * @purpose  Creates the hardware context for a routing interface.
 *
 * @param intIfNum @b{(input)} The internal interface number for this
 * @param                      router interface
 * @param mode @b{(input)} L7_ENABLE, to enable routing capability in
 * @param this router interface or L7_DISABLE, to disable routing capability
 * @param in this router interface
 *
 * @returns  L7_SUCCESS  on a successful operation 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes    This does not assign any attributes or make the interface
 *           active, it simply creates the context in which those operations
 *           may be performed.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlRtrIntfCreate(L7_uint32 intIfNum);


/*
  /------------------------------------------------------------------\
  *                          IPV4 APIs                                *
  \------------------------------------------------------------------/
*/


/*********************************************************************
 * @purpose  Enables or disables the IPv4 routing function,
 * @purpose  i.e. whether this entity has the ability to route
 * @purpose  IPv4 or not.
 *
 * @param mode @b{(input)} L7_ENABLE, to enable forwarding of PDUs
 *                         in this router.
 *                         L7_DISABLE, to disable forwarding in this
 *                         router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes Forwarding - The indication of whether this entity is
 * @notes acting as an IP gateway in respect to the forwarding of PDUs
 * @notes received by, but not addressed to, this entity.  IP gateways
 * @notes forward PDUs.  IP hosts do not (except those source-routed via
 * @notes the host).
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4ForwardingModeSet(L7_uint32 mode);


/*********************************************************************
 * @purpose  Enables or disables the ability to forward an based on IPv4 TOS 
 * @purpose  (Type Of Service).
 *
 * @param    mode  @b{(input)} L7_ENABLE, to enable TOS based Forwarding
 *                             in this router. 
 *                             L7_DISABLE, to disable TOS based forwarding
 *                             in this router.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4TOSForwardingModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Sets the ability of the routing function to fragment PDUs
 *
 * @param    mode     @b{(input)} as defined by L7_RT_FRAGMENTATION_MODE_t
 *                                {L7_RT_FRAGMENT_DISCARD,
 *                                L7_RT_FRAGMENT_SEND_TO_CPU,
 *                                L7_RT_FRAGMENT_SEND_TO_2ND_RTR}
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4FragmentationModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Configures the router whether to forward BOOTP/DHCP requests 
 * @purpose  through router or not.
 *
 * @param     @b{(input)} L7_ENABLE, to enabling forwarding of BOOTP/DHCP
 *                        requests. 
 *                        L7_DISABLE, to disable forwarding of BOOTP/DHCP
 *                        requests
 * @param     @b{(input)} The IP address of the BOOTP/DHCP server
 * 
 * @returns  L7_SUCCESS  on a successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes Forwarding BOOTP and DHCP frames is an exception to the
 * @notes specification that Layer2 broadcasts and multicasts packets
 * @notes should not be forwarded through the router.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4BootpDhcpRelaySet(L7_uint32 mode, L7_IP_ADDR_t ipAddr);

/*********************************************************************
 * @purpose  Enables or disables the ability of the router to perform 
 * @purpose  address checking of the source address
 *
 * @param    mode     @b{(input)} L7_ENABLE, to enable checking of
 *                                source address in this router. 
 *                                L7_DISABLE, to disable checking of
 *                                source address in this router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    Typically checking is done to isolate address beginning with 
 * @notes    127.x.x.x, 224.x.x.x, all ones and all zeros 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4SourceAddressCheckingModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Enables or disables IP Spoofing check
 *
 * @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes   none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4SpoofingModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Configure handling of IP Spoofing Errors
 *
 * @param    mode     @b{(input)} defined by L7_RT_NP_SPOOFING_HANDLER_MODE_t
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes   This configuration is valid only if spoofing is enabled.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4SpoofingHandlingSet(L7_uint32 mode);

/*********************************************************************
 * @purpose Enables or disables the ability of the router to send
 * @purpose ICMP Redirect error when it receives a PDU that should have
 * @purpose been to a different router.
 *
 * @param    mode     @b{(input)} L7_ENABLE, to enable ICMP Redirect in
 *                                           this router. 
 *                                L7_DISABLE, to disable ICMP Redirect
 *                                            in this router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4ICMPRedirectModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Modify parameters of a routing interface
*
* @param    ipCircDesc @b{(input)} new parameters for the interface
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @notes    Some of these parameters are not IPv4 specific (i.e., they also
*           apply to IPv6). We assign them here as a group because the 
*           driver requires we pass all these values in the command. 
*
*           Sets these parameters:
*             -- L2 encapsulation type
*             -- interface MTU
*             -- whether IPv4 routing is enabled
*             -- whether forwarding of net directed broadcasts is enabled
*             -- whether multicast forwarding is enabled
*             -- whether the interface is a broadcast interface
*
*           Does not set IPv4 interface addresses.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpv4RtrIntfModify(dtlRtrIntfDesc_t *ipCircDesc);

/*********************************************************************
 * @purpose  Sets an IP address for a particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                which the IP address is assigned.
 * @param    ipAddr   @b{(input)} The IP address to be assigned
 * @param    ipMask   @b{(input)} The subnet mask associated with the
 *                                IP address
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function sets the IP address for all external interfaces
 *           and for the  system
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4IntfIPAddrSet(L7_uint32 intIfNum, 
                                     L7_IP_ADDR_t ipAddr, 
                                     L7_IP_MASK_t ipMask);

/*********************************************************************
 * @purpose  Enables or disables the ability to forward network directed 
 * @purpose  broadcasts for this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    mode     @b{(input)} L7_ENABLE, to enable forwarding of
 *                                           network directed broadcasts
 *                                           in this router interface.
 *                                L7_DISABLE, to disable forwarding of
 *                                            network directed broadcasts
 *                                            in this router interface
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4NetDirectedBcastModeSet(L7_uint32 intIfNum,
                                               L7_uint32 mode);

/*********************************************************************
 * @purpose  Enables or disables the ability to forward multicast traffic for 
 * @purpose  this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    mode     @b{(input)} L7_ENABLE, to enable forwarding of
 *                                           multicast traffic in this
 *                                           router interface. 
 *                                L7_DISABLE, to disable forwarding of
 *                                            multicast traffic in this
 *                                            router interface.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4MulticastIntfForwardModeSet(L7_uint32 intIfNum,
                                                   L7_uint32 mode);

/*********************************************************************
 * @purpose  Adds an IP address to the Local Multicast List associated 
 * @purpose  with this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    ipAddress@b{(input)} The IP address to be added to this list  
 *
 *
 * @returns  L7_SUCCESS  on successful addition
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @notes    The Local Multicast List contains the list Multicast IP addresses 
 *           that this interface will interact with, respond to, etc.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4LocalMulticastAddrAdd(L7_uint32 intIfNum,
                                             L7_IP_ADDR_t ipAddress);

/*********************************************************************
 * @purpose  Deletes an IP address from the Local Multicast List 
 * @purpose  associated with this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    ipAddress@b{(input)} The IP address to be deleted from this list  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4LocalMulticastAddrDelete(L7_uint32 intIfNum,
                                                L7_IP_ADDR_t ipAddress);

/*********************************************************************
 * @purpose  Adds an IP address to a router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IP address to be added to this list  
 *
 *
 * @returns  L7_SUCCESS  on successful addition
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4IntfAddrAdd(L7_uint32 intIfNum,
                                   L7_IP_ADDR_t addr,
                                   L7_IP_MASK_t mask);

/*********************************************************************
 * @purpose  Deletes an IP address from a router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IP address to be deleted from
 *                                this list  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4IntfAddrDelete(L7_uint32 intIfNum,
                                      L7_IP_ADDR_t addr,
                                      L7_IP_MASK_t mask);

/*********************************************************************
 * @purpose  Adds an ARP entry in the ARP table
 * @purpose  maintained in the Forwarding Information Base of the
 * @purpose  network-processing device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the ARP entry
 *                    object containing information needed to add an entry
 *                    to the ARP table.  
 *
 *
 * @returns  L7_SUCCESS  on a successful addition of the ARP entry 
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @notes    ARP table entries correlate an IP address to
 *           a MAC address and Router Interface. The table is built as a
 *           result of static entries and information obtained from
 *           ARP  requests and replies. Dynamic  entries are periodically
 *           aged.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4ArpEntryAdd(L7_arpEntry_t *pNeigh);

/*********************************************************************
 * @purpose  Modifies an existing ARP entry in the
 * @purpose  ARP table maintained in the Forwarding
 * @purpose  Information Base of the network-processing device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the ARP entry
 *                                object, which contains modified parameters.  
 *
 *
 * @returns  L7_SUCCESS  on a successful modification of the entry
 * @returns  L7_FAILURE  if the modification failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4ArpEntryModify(L7_arpEntry_t *pNeigh);

/*********************************************************************
 * @purpose  Deletes an ARP entry in the ARP table maintained
 * @purpose  in the Forwarding Information Base of the network-processing
 * @purpose  device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the ARP entry
 *                                object, which describes the entry to delete.
 *
 * @returns  L7_SUCCESS  on a successful deletion of the entry 
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes    An ARP entry is uniquely identified by the
 *           IP address associated with it.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4ArpEntryDelete(L7_arpEntry_t *pNeigh);

/*********************************************************************
* @purpose  Get the status of a ARP entry in the hardware
*
* @param    *pArp    @b{(input)} A reference to the ARP entry object
*                    containing information about hit-bit and last-hit-time
*
*
* @returns  L7_SUCCESS  on a successful query of the ARP entry
* @returns  L7_FAILURE  for invalid arguments
*
* @notes    number of entries queried for in this API is 1.
*
*
*
* @end
*********************************************************************/
L7_RC_t dtlIpv4ArpEntryQuery(L7_arpQuery_t *pArp);

/*********************************************************************
 * @purpose  Callback function from the driver for ARP work.
 *
 * @param    *ddusp         @b{(input)} Device driver reference to unit
 * @param                               slot and port
 * @param	  family         @b{(input)} Device Driver family type 
 * @param    cmd            @b{(input)} Command of type DAPI_CMD_t
 * @param    event          @b{(input)} Event indication of type DAPI_EVENT_t
 * @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent data
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4ArpEntryCallback(DAPI_USP_t *ddusp, 
                                        DAPI_FAMILY_t family, 
                                        DAPI_CMD_t cmd, 
                                        DAPI_EVENT_t event,
                                        void *dapiEventInfo);

/*********************************************************************
 * @purpose  Adds a route to the Routing Table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed for a route entry.  
 *
 *
 * @returns  L7_SUCCESS  on successful addition 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    Route table entries correlate a route to a router interface or 
 * @notes    network interface. The routes kept in the table originate from
 *           static 
 * @notes    configuration and routing protocols, such as RIP, OSPF, and BGP.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4RouteEntryAdd(L7_routeEntry_t *pRoute);

/*********************************************************************
 * @purpose  Modifies a route in the routing table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed to identify a route and modify it.  
 *
 *
 * @returns  L7_SUCCESS  on successful modification 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4RouteEntryModify(L7_routeEntry_t *pRoute);

/*********************************************************************
 * @purpose  Deletes a route in the routing table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed to identify a route and delete it.  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4RouteEntryDelete(L7_routeEntry_t *pRoute);

/*********************************************************************
 * @purpose  Gets count of subsystem route entries
 *
 * @param    void
 *
 *
 * @returns  count of route entries in subsystem
 *
 * @notes    This routine is used for internal component synchronization.
 *           It is not intended to be used for external user management
 *           (i.e. SNMP, Web) purposes.
 *
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_uint32 dtlIpv4RouteEntryCountGet(void);

/*********************************************************************
 * @purpose  Add a VRRP Virtural Router ID to an interface
 *
 * @param    intIfNum @b{(input)} an internal interface number
 * @param    vrID     @b{(input)} VRRP Virtual Router ID
 * @param    ipAddr   @b((input)) VRRP IPv4 (IPv6) address
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function is valid only for routing interfaces.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4VrrpVridAdd(L7_uint32 intIfNum,
                                   L7_uint32 vrID,
                                   L7_uint32 ipAddr); 

/*********************************************************************
 * @purpose  Remove a VRRP Virtural Router ID from an interface
 *
 * @param    intIfNum @b{(input)} an internal interface number
 * @param    vrID     @b{(input)} VRRP Virtual Router ID
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function is valid only for routing interfaces.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv4VrrpVridDelete(L7_uint32 intIfNum, L7_uint32 vrID) ;

/*
  /------------------------------------------------------------------\
  *                          IPV6 APIs                                *
  \------------------------------------------------------------------/
*/


/*********************************************************************
 * @purpose  Enables or disables the routing function for IPv6,
 * @purpose  i.e. whether this entity has the ability to route IPv6
 * @purpose  or not.
 *
 * @param mode @b{(input)} L7_ENABLE, to enable forwarding of PDUs
 *                         in this router.
 *                         L7_DISABLE, to disable forwarding in this
 *                         router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes Forwarding - The indication of whether this entity is
 * @notes acting as an IP gateway in respect to the forwarding of PDUs
 * @notes received by, but not addressed to, this entity.  IP gateways
 * @notes forward PDUs.  IP hosts do not (except those source-routed via
 * @notes the host).
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6ForwardingModeSet(L7_uint32 mode);


/*********************************************************************
 * @purpose  Enables or disables the ability to forward an based on
 * @purpose  IPv6 Traffic Class for the router
 *
 * @param    mode  @b{(input)} L7_ENABLE, to enable Traffic Class based
 *                             Forwarding in this router. 
 *                             L7_DISABLE, to disable Traffic Class based
 *                             forwarding in this router.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6TrafficClassForwardingModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Configures the router whether to forward BOOTP/DHCP requests 
 * @purpose  through router or not.
 *
 * @param     @b{(input)} L7_ENABLE, to enabling forwarding of BOOTP/DHCP
 *                        requests. 
 *                        L7_DISABLE, to disable forwarding of BOOTP/DHCP
 *                        requests
 * @param     @b{(input)} The IP address of the BOOTP/DHCP server
 * 
 * @returns  L7_SUCCESS  on a successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes Forwarding BOOTP and DHCP frames is an exception to the
 * @notes specification that Layer2 broadcasts and multicasts packets
 * @notes should not be forwarded through the router.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6BootpDhcpRelaySet(L7_uint32 mode,
                                         L7_in6_addr_t *addr);

/*********************************************************************
 * @purpose  Enables or disables the ability of the router to perform 
 * @purpose  address checking of the source address
 *
 * @param    mode     @b{(input)} L7_ENABLE, to enable checking of
 *                                source address in this router. 
 *                                L7_DISABLE, to disable checking of
 *                                source address in this router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6SourceAddressCheckingModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Enables or disables IP Spoofing check
 *
 * @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes   none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6SpoofingModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Configure handling of IP Spoofing Errors
 *
 * @param    mode     @b{(input)} defined by L7_RT_NP_SPOOFING_HANDLER_MODE_t
 *
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_FAILURE  if other failure
 *
 * @notes   This configuration is valid only if spoofing is enabled.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6SpoofingHandlingSet(L7_uint32 mode);

/*********************************************************************
 * @purpose Enables or disables the ability of the router to send
 * @purpose ICMP Redirect error when it receives a PDU that should have
 * @purpose been to a different router.
 *
 * @param    mode     @b{(input)} L7_ENABLE, to enable ICMP Redirect in
 *                                           this router. 
 *                                L7_DISABLE, to disable ICMP Redirect
 *                                            in this router
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    none 
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6ICMPRedirectModeSet(L7_uint32 mode);

/*********************************************************************
 * @purpose  Assigns the Layer 2 encapsulation type for the particular
 * @purpose  router interface.                   
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                which encapsulation type is assigned
 * @param    type     @b{(input)} L7_LL_ENCAP_ENET, to set the encapsulation
 *                                according to Ethernet Encapsulation
 *                                (RFC 894) 
 *                                L7_LL_ENCAP_802, to set encapsulation
 *                                according to IEE 802.2/3 Encapsulation
 *                                (RFC1042)
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6L2EncapsulationTypeSet(L7_uint32 intIfNum,
                                              L7_uint32 type);

/*********************************************************************
 * @purpose  Assigns a MAC Address for a particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for 
 *                                which the MAC address is assigned
 * @param    *macAddr @b{(input)} A reference to the MAC address
 *                                to be assigned
 *
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   This command associates a MAC address with a router interface.
 * @notes   This command is valid with the layer 3 routing offering only.
 * @notes   It is completely distinct from the commands to store a MAC 
 * @notes   address in the layer 2 filtering database (i.e. address tables).
 * @notes   In particular, it is completely distinct from the commands
 * @notes   dtlFdbSystemMacSet() and dtlFdbMacAddrAdd().
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6IntfMacAddrSet(L7_uint32 intIfNum,
                                      L7_enetMacAddr_t *macAddr);

/*********************************************************************
 * @purpose  Sets the MTU (Maximum Transmission Unit) for this particular
 *           router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for this
 *                                router interface
 * @param    mtu      @b{(input)} The size of the Maximum Transmission Unit
 *
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6MtuSet(L7_uint32 intIfNum, L7_uint32 mtu);

/*********************************************************************
 * @purpose  Enables or disables the ability to forward multicast traffic for 
 * @purpose  this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    mode     @b{(input)} L7_ENABLE, to enable forwarding of
 *                                           multicast traffic in this
 *                                           router interface. 
 *                                L7_DISABLE, to disable forwarding of
 *                                            multicast traffic in this
 *                                            router interface.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6MulticastIntfForwardModeSet(L7_uint32 intIfNum,
                                                   L7_uint32 mode);

/*********************************************************************
 * @purpose  Adds an IP address to the Local Multicast List associated 
 * @purpose  with this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be added to this list  
 *
 *
 * @returns  L7_SUCCESS  on successful addition
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @notes    The Local Multicast List contains the list Multicast IP addresses 
 *           that this interface will interact with, respond to, etc.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6LocalMulticastAddrAdd(L7_uint32 intIfNum,
                                             L7_in6_addr_t *addr);

/*********************************************************************
 * @purpose  Deletes an IP address from the Local Multicast List 
 * @purpose  associated with this particular router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be deleted from
 *                                this list  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6LocalMulticastAddrDelete(L7_uint32 intIfNum,
                                                L7_in6_addr_t *addr);

/*********************************************************************
 * @purpose  Adds an IPv6 address to a router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be added to this list  
 *
 *
 * @returns  L7_SUCCESS  on successful addition
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6IntfAddrAdd(L7_uint32 intIfNum,
                                   L7_in6_addr_t *addr,
                                   L7_uint32 prefixLen);

/*********************************************************************
 * @purpose  Deletes an IPv6 address from a router interface
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    addr@b{(input)}      The IPv6 address to be deleted from
 *                                this list  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6IntfAddrDelete(L7_uint32 intIfNum,
                                      L7_in6_addr_t *addr,
                                      L7_uint32 prefixLen);

/*********************************************************************
 * @purpose  Harvest any hardware maintained IPv6 statistics from
 *           an interface.
 *
 * @param    intIfNum @b{(input)} The internal interface number for
 *                                this router interface
 * @param    stats     @b{(output)} A block to put the statistics in.
 *
 * @returns  L7_SUCCESS  on successful operation 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   The stats block will contain the hardware values of the
 *          the statistics.  Any statistic that is not kept by hardware
 *          will have a zero value on return.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6IntfStatsGet(L7_uint32 infIfNum,
                                    L7_ipv6InterfaceStats_t *stats);

/*********************************************************************
 * @purpose  Adds a Neighbor entry in the Neighbor table
 * @purpose  maintained in the Forwarding Information Base of the
 * @purpose  network-processing device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the Neighbor entry
 *                    object containing information needed to add an entry
 *                    to the Neighbor table.  
 *
 *
 * @returns  L7_SUCCESS  on a successful addition of the Neighbor entry 
 * @returns  L7_FAILURE  if the addition failed 
 *
 * @notes    Neighbor table entries correlate an IPv6 address to
 *           a MAC address and Router Interface. The table is built as a
 *           result of static entries and information obtained from
 *           Neighbor Discovery requests and replies. Dynamic 
 *           entries are periodically aged.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6NeighEntryAdd(L7_ip6NbrTblEntry_t *pNeigh);

/*********************************************************************
 * @purpose  Modifies an existing Neighbor entry in the
 * @purpose  Neighbor table maintained in the Forwarding
 * @purpose  Information Base of the network-processing device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the Neighbor entry
 *                                object, which contains modified parameters.  
 *
 *
 * @returns  L7_SUCCESS  on a successful modification of the entry
 * @returns  L7_FAILURE  if the modification failed or the entry does not exist
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6NeighEntryModify(L7_ip6NbrTblEntry_t *pNeigh);

/*********************************************************************
 * @purpose  Deletes a Neighbor entry in the Neighbor table maintained
 * @purpose  in the Forwarding Information Base of the network-processing
 * @purpose  device.
 *
 * @param    *pNeigh  @b{(input)} A reference to the Neighbor entry
 *                                object, which describes the entry to delete.
 *
 * @returns  L7_SUCCESS  on a successful deletion of the entry 
 * @returns  L7_FAILURE  if the deletion failed or the entry does not exist
 *
 * @notes    A Neighbor entry is uniquely identified by the
 *           IPv6 address associated with it.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6NeighEntryDelete(L7_ip6NbrTblEntry_t *pNeigh);


/*********************************************************************
* @purpose  Get the status of a NDP entry in the hardware
*
* @param    *pNdpQuery   @b{(input)} A reference to the NDP entry object
*           containing information about hit-bit and last-hit-time
*
* @returns  L7_SUCCESS  on a successful query for an NDP entry
* @returns  L7_FAILURE  if the query failed
*
* @notes    number of entries queried for in this API is 1.
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlIpv6NeighEntryQuery(L7_ndpQuery_t *pNdpQuery);


/*********************************************************************
 * @purpose  Gets count of subsystem Neighbor entries
 *
 * @param    void
 *
 *
 * @returns  count of Neighbor entries in subsystem
 *
 * @notes    This routine is used for internal component synchronization.
 *           It is not intended to be used for external user management
 *           (i.e. SNMP, Web) purposes.
 *
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_uint32 dtlIpv6NeighborEntryCountGet(void);

/*********************************************************************
 * @purpose  Callback function from the driver for NDP work.
 *
 * @param    *ddusp         @b{(input)} Device driver reference to unit
 * @param                               slot and port
 * @param	  family         @b{(input)} Device Driver family type 
 * @param    cmd            @b{(input)} Command of type DAPI_CMD_t
 * @param    event          @b{(input)} Event indication of type DAPI_EVENT_t
 * @param    *dapiEventInfo @b{(input)} Pointer to dapiEvent data
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6NeighEntryCallback(DAPI_USP_t *ddusp, 
                                          DAPI_FAMILY_t family, 
                                          DAPI_CMD_t cmd, 
                                          DAPI_EVENT_t event,
                                          void *dapiEventInfo);

/*********************************************************************
 * @purpose  Adds a route to the Routing Table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed for a route entry.  
 *
 *
 * @returns  L7_SUCCESS  on successful addition 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes    Route table entries correlate a route to a router interface or 
 * @notes    network interface. The routes kept in the table originate from
 *           static 
 * @notes    configuration and routing protocols, such as RIP, OSPF, and BGP.
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6RouteEntryAdd(L7_route6Entry_t *pRoute);

/*********************************************************************
 * @purpose  Modifies a route in the routing table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed to identify a route and modify it.  
 *
 *
 * @returns  L7_SUCCESS  on successful modification 
 * @returns  L7_FAILURE  if the operation failed
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6RouteEntryModify(L7_route6Entry_t *pRoute);

/*********************************************************************
 * @purpose  Deletes a route in the routing table maintained in the 
 * @purpose  Forwarding Information Base in the network-processing device.
 *
 * @param    *pRoute  @b{(input)} A reference to the route entry object 
 *                                that contains all the necessary information 
 *                                needed to identify a route and delete it.  
 *
 *
 * @returns  L7_SUCCESS  on successful deletion 
 * @returns  L7_FAILURE  if the operation failed 
 *
 * @notes   
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6RouteEntryDelete(L7_route6Entry_t *pRoute);


/*********************************************************************
 * @purpose  Gets count of subsystem route entries
 *
 * @param    void
 *
 *
 * @returns  count of route entries in subsystem
 *
 * @notes    This routine is used for internal component synchronization.
 *           It is not intended to be used for external user management
 *           (i.e. SNMP, Web) purposes.
 *
 *                                                   
 * @end
 *********************************************************************/
EXT_API L7_uint32 dtlIpv6RouteEntryCountGet(void);

/*********************************************************************
 * @purpose  Add a VRRP Virtural Router ID to an interface
 *
 * @param    intIfNum @b{(input)} an internal interface number
 * @param    vrID     @b{(input)} VRRP Virtual Router ID
 * @param    ipAddr   @b((input)) VRRP IPv4 (IPv6) address
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function is valid only for routing interfaces.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6VrrpVridAdd(L7_uint32 intIfNum,
                                   L7_uint32 vrID,
                                   L7_in6_addr_t *addr); 

/*********************************************************************
 * @purpose  Remove a VRRP Virtural Router ID from an interface
 *
 * @param    intIfNum @b{(input)} an internal interface number
 * @param    vrID     @b{(input)} VRRP Virtual Router ID
 *
 * @returns  L7_SUCCESS  on successful operation
 * @returns  L7_FAILURE  if the operation failed
 *
 *
 * @notes    This function is valid only for routing interfaces.
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t dtlIpv6VrrpVridDelete(L7_uint32 intIfNum, L7_uint32 vrID) ;

/*********************************************************************
* @purpose  Create a tunnel in the driver
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    nextHopAddr      @b{(input)} Transport Nexthop Address
* @param    nextHopIntIfNum  @b{(input)} Transport Nexthop interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlTunnelCreate(L7_uint32 intIfNum,
                                L7_TUNNEL_MODE_t tunnelMode,
                                L7_sockaddr_union_t *localAddr,
                                L7_sockaddr_union_t *remoteAddr,
                                L7_sockaddr_union_t *nextHopAddr,
                                L7_uint32 nextHopIntIfNum);

/*********************************************************************
* @purpose  Delete a tunnel in the driver
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlTunnelDelete(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the transport nexhop of a tunnel.
*
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    nextHopAddr      @b{(input)} Transport Nexthop Address
* @param    nextHopIntIfNum  @b{(input)} Transport Nexthop Interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlTunnelNextHopSet(L7_uint32 intIfNum,
                                    L7_sockaddr_union_t *nextHopAddr,
                                    L7_uint32 nextHopIntIfNum);

/*********************************************************************
* @purpose  Add a tunnel in the driver
*
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    nextHopAddr      @b{(input)} Transport Nexthop Address
* @param    nextHopIntIfNum  @b{(input)} Transport Nexthop interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL3TunnelAdd(L7_uint32 tunnelMode,
                               L7_sockaddr_union_t *localAddr,
                               L7_sockaddr_union_t *remoteAddr,
                               L7_sockaddr_union_t *nextHopAddr,
                               L7_uint32 nextHopIntIfNum);

/*********************************************************************
* @purpose  Delete a tunnel in the driver
*
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL3TunnelDelete(L7_uint32 tunnelMode,
                                  L7_sockaddr_union_t *localAddr,
                                  L7_sockaddr_union_t *remoteAddr);

/*********************************************************************
* @purpose  Add a tunnel MAC address in the driver
*
* @param    tunnelMode       @b{(input)} Tunnel mode
* @param    localAddr        @b{(input)} Source transport address
* @param    remoteAddr       @b{(input)} Destination transport address
* @param    macAddr          @b{(input)} MAC address
* @param    intIfNum         @b{(input)} Internal interface ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL3TunnelMacAddressAdd(L7_uint32 tunnelMode,
                                         L7_sockaddr_union_t *localAddr,
                                         L7_sockaddr_union_t *remoteAddr,
                                         L7_uchar8 *macAddr,
                                         L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Delete a tunnel MAC address in the driver
*
* @param    macAddr          @b{(input)} MAC address
* @param    intIfNum         @b{(input)} Internal interface ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @end
*********************************************************************/
EXT_API L7_RC_t dtlL3TunnelMacAddressDelete(L7_uchar8 *macAddr,
                                            L7_uint32 intIfNum);

#endif /* INCLUDE_DTL_L3_API_H */

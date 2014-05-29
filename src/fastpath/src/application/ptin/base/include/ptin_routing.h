/**
 * ptin_routing.c
 *  
 * Implements the Routing interface module 
 *
 * Created on: 2014/04/16
 * Author: Daniel Figueira
 *  
 * Notes:
 */

#ifndef _PTIN_ROUTING_H
#define _PTIN_ROUTING_H

#include "ptin_include.h"
#include "ptin_msghandler.h"

/*********************************************************** 
 * Defines
 ***********************************************************/
#define PTIN_ROUTING_ARPTABLE_TYPE_STATIC   CCMSG_ROUTING_ARPTABLE_TYPE_STATIC 
#define PTIN_ROUTING_ARPTABLE_TYPE_DYNAMIC  CCMSG_ROUTING_ARPTABLE_TYPE_DYNAMIC
#define PTIN_ROUTING_ARPTABLE_TYPE_LOCAL    CCMSG_ROUTING_ARPTABLE_TYPE_LOCAL  
#define PTIN_ROUTING_ARPTABLE_TYPE_GATEWAY  CCMSG_ROUTING_ARPTABLE_TYPE_GATEWAY

#define PTIN_ROUTING_INTF_TYPE_UPLINK       CCMSG_ROUTING_INTF_TYPE_UPLINK
#define PTIN_ROUTING_INTF_TYPE_LOOPBACK     CCMSG_ROUTING_INTF_TYPE_LOOPBACK

/***********************************************************
 * Typedefs
 ***********************************************************/

/*********************************************************** 
 * Function prototypes
 ***********************************************************/

/**
 * Initialization
 *  
 * @return none
 */
L7_RC_t ptin_routing_init(void);

/**
 * Create a new routing interface.
 * 
 * @param routingIntf    : Routing interface
 * @param intfType       : Routing interface type
 * @param physicalIntf   : Physical interface
 * @param routingVlanId  : Vlan ID to which the routing interface will be associated
 * @param internalVlanId : Fastpath's internal Vlan ID for the EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_create(ptin_intf_t* routingIntf, L7_uint8 intfType, ptin_intf_t* physicalIntf, L7_uint16 routingVlanId, L7_uint16 internalVlanId);

/**
 * Remove an existing routing interface.
 * 
 * @param routingIntf : Routing interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_remove(ptin_intf_t* routingIntf);

/**
 * Set routing interface's ip address.
 * 
 * @param routingIntf : Routing interface
 * @param ipFamily    : IP address family [L7_AF_INET4; L7_AF_INET6]
 * @param ipAddr      : IP address
 * @param subnetMask  : Subnet mask
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_ipaddress_set(ptin_intf_t* routingIntf, L7_uchar8 ipFamily, L7_uint32 ipAddr, L7_uint32 subnetMask);

/**
 * Get the physical interface currently associated with the requested routing interface.
 * 
 * @param routingIntfNum  : Routing interface
 * @param physicalIntfNum : Ptr to the physical intfNum
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note PTIN_ROUTING_INTF_TYPE_LOOPBACK routing interfaces do not have a specific physical interface associated with them.
 *       In those cases, 'physicalIntfNum' is set to (L7_uint16)-1.
 *  
 * @note If this prototype is modified, do not forget to update 'l3_intf.c' file as I was required to place an extern 
 *       prototype declaration of this method there.
 */
L7_RC_t ptin_routing_intf_physicalport_get(L7_uint16 routingIntfNum, L7_uint16 *physicalIntfNum);

/**
 * Get ARP table.
 * 
 * @param intfNum     : Desired intfNum. Use -1 if no filtering is desired
 * @param firstIdx    : Index of the first entry to copy
 * @param maxEntries  : Max number of entries to read
 * @param readEntries : Ptr to number of read entries
 * @param buffer      : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_arptable_get(L7_uint32 intfNum, L7_uint32 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingArpTableResponse* buffer);

/**
 * Remove ARP entry.
 * 
 * @param intfNum : Desired intfNum. Use -1 to delete all ARP entries for this IP address
 * @param ipAddr  : IP address 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_arpentry_purge(L7_uint32 intfNum, L7_uint32 ipAddr);

/**
 * Get route table.
 * 
 * @param intfNum     : Desired intfNum. Use -1 if no filtering is desired
 * @param firstIdx    : Index of the first entry to copy
 * @param maxEntries  : Max number of entries to read
 * @param readEntries : Ptr to number of read entries
 * @param buffer      : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_routetable_get(L7_uint32 intfNum, L7_uint32 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingRouteTableResponse* buffer);

/**
 * Configure a static route.
 * 
 * @param dstIpAddr   : Destination IP address
 * @param subnetMask  : Subnet mask
 * @param nextHopRtr  : Gateway router
 * @param pref        : Route preference value
 * @param isNullRoute : Set to L7_TRUE to redirect all traffic that matches this route to the null0 interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note If a static route with the same destination IP address, subnet mask and next hop router exists, its preference will be updated to the provided value. 
 */
L7_RC_t ptin_routing_staticroute_add(L7_uint32 dstIpAddr, L7_uint32 subnetMask, L7_uint32 nextHopRtr, L7_uint8 pref, L7_BOOL isNullRoute);

/**
 * Delete an existing static route.
 * 
 * @param dstIpAddr   : Destination IP address
 * @param subnetMask  : Subnet mask
 * @param nextHopRtr  : Gateway router
 * @param isNullRoute : Set to L7_TRUE if this static route is a null route
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note Next hop router is not a mandatory parameter (use the 0 when none is specified). 
 */
L7_RC_t ptin_routing_staticroute_delete(L7_uint32 dstIpAddr, L7_uint32 subnetMask, L7_uint32 nextHopRtr, L7_BOOL isNullRoute);

/**
 * Start a ping request.
 * 
 * @param sessionIdx    : Ping session index
 * @param ipAddr        : IP address to ping
 * @param probeCount    : Probe count
 * @param probeSize     : Probe size
 * @param probeInterval : Probe interval
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_create(L7_uint8 sessionIdx, L7_uint32 ipAddr, L7_uint16 probeCount, L7_uint16 probeSize, L7_uint16 probeInterval);

/**
 * Query an existing ping session.
 * 
 * @param buffer : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_query(msg_RoutingPingSessionQuery* buffer);

/**
 * Free an existing ping session.
 * 
 * @param sessionIdx : Ping session index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_pingsession_free(L7_uint8 sessionIdx);

/**
 * Start a traceroute request.
 * 
 * @param sessionIdx    : Traceroute session index
 * @param ipAddr        : IP address to ping
 * @param probeSize     : Probe size
 * @param probePerHop   : Probes per hop
 * @param probeInterval : Probe interval
 * @param dontFrag      : Don't frag
 * @param port          : Destination port
 * @param maxTtl        : Max ttl
 * @param initTtl       : Initial ttl
 * @param maxFail       : Max fails
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_create(L7_uint8 sessionIdx, L7_uint32 ipAddr, L7_uint16 probeSize, L7_uint16 probePerHop, L7_uint32 probeInterval,
                                              L7_BOOL dontFrag, L7_uint16 port, L7_ushort16 maxTtl, L7_ushort16 initTtl, L7_ushort16 maxFail);

/**
 * Query an existing traceroute session.
 * 
 * @param buffer : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_query(msg_RoutingTracertSessionQuery* buffer);

/**
 * Get known hops for an existing traceroute session.
 * 
 * @param sessionIdx  : Desired intfNum. Use -1 if no filtering is desired
 * @param firstIdx    : Index of the first entry to copy
 * @param maxEntries  : Max number of entries to read
 * @param readEntries : Ptr to number of read entries
 * @param buffer      : Ptr to the struct array where each entry will be placed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_gethops(L7_uint32 sessionIdx, L7_uint16 firstIdx, L7_uint32 maxEntries, L7_uint32* readEntries, msg_RoutingTracertSessionHopsResponse* buffer);

/**
 * Free an existing traceroute session.
 * 
 * @param sessionIdx : Traceroute session index
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_traceroutesession_free(L7_uint8 sessionIdx);


/*********************************************************** 
 * Debug methods
 ***********************************************************/

/**
 * Dump the current status of the routing interfaces.
 */
void ptin_routing_intf_dump(void);

/**
 * Dump the local ARP table snapshot.
 */
void ptin_routing_arptablesnapshot_dump(void);

/**
 * Dump the local Route table snapshot.
 */
void ptin_routing_routetablesnapshot_dump(void);

/**
 * Dump the current status of the ping sessions array.
 * 
 * @param index : Ping session index. Use -1 to dump all sessions
 */
void ptin_routing_pingsession_dump(L7_uint32 index);

/**
 * Dump the current status of the traceroute sessions array.
 * 
 * @param index : Traceroute session index. Use -1 to dump all sessions
 */
void ptin_routing_traceroutesession_dump(L7_uint32 index);

/**
 * Dump the local traceroute hops snapshot.
 */
void ptin_routing_traceroutehopssnapshot_dump(void);

#endif /* _PTIN_ROUTING_H */

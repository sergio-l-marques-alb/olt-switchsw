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
#define PTIN_ROUTING_ARPTABLE_TYPE_STATIC   1
#define PTIN_ROUTING_ARPTABLE_TYPE_DYNAMIC  2
#define PTIN_ROUTING_ARPTABLE_TYPE_LOCAL    3
#define PTIN_ROUTING_ARPTABLE_TYPE_GATEWAY  4

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
 * Deinitialization
 *  
 * @return none
 */
L7_RC_t ptin_routing_deinit(void);

/**
 * Create a new routing interface.
 * 
 * @param intf           : Routing interface
 * @param routingVlanId  : Vlan ID to which the routing interface will be associated
 * @param internalVlanId : Fastpath's internal Vlan ID for the EVC
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_create(ptin_intf_t* intf, L7_uint16 routingVlanId, L7_uint16 internalVlanId);

/**
 * Remove an existing routing interface.
 * 
 * @param intfId        : ID of the routing interface (this is not the intfNum)
 * @param routingVlanId : Vlan ID of the routing interface
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_remove(ptin_intf_t* intf, L7_uint16 routingVlanId);

/**
 * Set routing interface's ip address.
 * 
 * @param intf       : Routing interface
 * @param ipFamily   : IP address family [L7_AF_INET4; L7_AF_INET6]
 * @param ipAddr     : IP address
 * @param subnetMask : Subnet mask
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_ipaddress_set(ptin_intf_t* intf, L7_uchar8 ipFamily, L7_uint32 ipAddr, L7_uint32 subnetMask);

/**
 * Set routing interface's MAC address.
 * 
 * @param intfId  : Routing interface
 * @param macAddr : MAC address
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_routing_intf_macaddress_set(ptin_intf_t* intf, L7_enetMacAddr_t* macAddr);

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


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_ip_api.h
*
* @purpose    Ip Mapping apis
*
* @component  Ip Mapping Layer
*
* @comments   none
*
* @create     03/12/2001
*
* @author     asuthan
* @end
*
**********************************************************************/

#ifndef INCLUDE_IP_API_H
#define INCLUDE_IP_API_H

#include "l3_comm_structs.h"
#include "dtl_l3_api.h"
#include "sysnet_api.h"
#include "async_event_api.h"
#include "l3_commdefs.h"

/*------------------------------------*/
/*  IP NOTIFICATION LISTS             */
/*------------------------------------*/

#define IPMAP_NOTIFY_FUNC_NAME_SIZE     64
/* The following structure ipRoutingEventNotifyList_t 
	 is used by routing protocols to register 
	 for callbacks for various routing events as 
	 listed in the enum L7_RTR_EVENT_CHANGE_t.
*/   
typedef struct
{
	L7_uint32   rtrProtoId;
    L7_uchar8   name[IPMAP_NOTIFY_FUNC_NAME_SIZE];  /* Name of registered func - for debug */
	L7_RC_t     (*notify_routing_event)(L7_uint32 intIfNum, L7_uint32 event,
                                    void *pData,ASYNC_EVENT_NOTIFY_INFO_t *pNotify);

} ipRoutingEventNotifyList_t, *pIpRoutingEventNotifyList_t;

/*-----------------------------------------*/
/*  STATIC ROUTE CONFIGURATION STRUCTURES  */
/*-----------------------------------------*/

/* Used to convery static route configuration information amongst components */

typedef struct L7_rtrStaticRoute_s
{
	L7_uint32           ipAddr;		   /* IP Address                   */
	L7_uint32           ipMask;		   /* IP Mask                      */
	L7_uint32           nextHopRtr;	   /* Next Hop Router IP           */																								/* Used for unnumbered interfaces */
	L7_uint32           preference;	   /* route preference             */
	L7_uint32           flags;                   
#ifndef L7_RT_DEFAULT_ROUTE
#define L7_RT_DEFAULT_ROUTE     0x01 /* Note: This matches with L7_RTF_DEFAULT_ROUTE value
                                      *       defined in L7_RT_ENTRY_FLAGS_t.
                                      *       So, assigning the flag values of L7_RT_ENTRY_FLAGS_t
                                      *       to flags variable (like L7_RTF_REJECT, L7_RTF_DEFAULT_ROUTE) */
#endif
#ifndef L7_RT_GLOBAL_DEFAULT_GATEWAY
#define L7_RT_GLOBAL_DEFAULT_GATEWAY     0x02 /* Flag to indicate that the default 
                                                 route is the Global Default Gateway */
#endif
  L7_uint32           intIfNum;
} L7_rtrStaticRoute_t;



/*-------------------------------------*/
/*  INTERFACE CAPABILITY QUERY VALUES  */
/*-------------------------------------*/
typedef enum {
  IPMAP_INTFCAP_MULTICAST = 1,
  IPMAP_INTFCAP_ARP,
  IPMAP_INTFCAP_ROUTING_CONFIGURABLE,
} ipIntfCapability_t;




/* Begin Function Prototypes */

/*********************************************************************
* @purpose  Returns ip spoofing mode
*
* @param    void
*
* @returns  L7_ENABLE   If Ip spoofing is enabled
* @returns  L7_DISABLE  If Ip spoofing is disabled
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpSpoofingGet();

/*********************************************************************
* @purpose  Sets Ip Spoofing mode
*
* @param    L7_uint32 mode - enables or disables Ip spoofing
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpSpoofingSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the Ip Mtu value
*
* @param    @b{(input)} L7_uint32 intIfNum
* @param    @b{(input)} L7_uint32 ipMtu
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The current implementation does not check against
*           the interface mtu (nimGetIntfMtuSize). Cisco implementation
*           works as follows: "ip mtu set" will return an error if
*           ip mtu > ifmtu. Also "if mtu set" should also return an error
*           if ip mtu (has been configured by the user) and is > new value
*           of ifmtu.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfIpMtuSet(L7_uint32 intIfNum, L7_uint32 ipMtu);

/*********************************************************************
* @purpose  Returns the configured IP MTU value
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ipMtu @b{(output)}   configured IP MTU
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfIpMtuGet(L7_uint32 intIfNum, L7_uint32 *ipMtu);

/*********************************************************************
* @purpose  Get the maximum IP MTU that may be set on an interface.  
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    maxIpMtu @b{(output)} maximum IP MTU in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    On port based routing interfaces, the IP MTU may be set
*           as large as the link MTU. On VLAN routing interfaces, 
*           the IP MTU may be set as large as the largest jumbo
*           frame will allow.
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfMaxIpMtuGet( L7_uint32 intIfNum, L7_uint32 *maxIpMtu);

/*********************************************************************
* @purpose  Get the IP MTU being enforced on an interface. If the IP MTU
*           is not configured, it is derived from the link MTU.  
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    pval     @b{(output)} ptr to value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIntfEffectiveIpMtuGet( L7_uint32 intIfNum, L7_uint32 *pval);

/*********************************************************************
 * @purpose  Set interface bandwidth
 *
 * @param    @b{(input)} L7_uint32 intIfNum        
 * @param    @b{(input)} L7_uint32 bandwidth      
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapIntfBandwidthSet(L7_uint32 intIfNum, L7_uint32 bandwidth);

/*********************************************************************
 * @purpose  Get interface bandwidth
 *
 * @param    @b{(input)} L7_uint32 intIfNum      
 * @param    @b{(output)} L7_uint32 bandwidth
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapIntfBandwidthGet(L7_uint32 intIfNum, L7_uint32 *bandwidth);

/*********************************************************************
 * @purpose  Get interface bandwidth without modifying the SET value.
 *
 * @param    @b{(input)} L7_uint32 intIfNum
 * @param    @b{(output)} L7_uint32 bandwidth
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapIntfBWGet(L7_uint32 intIfNum, L7_uint32 *bandwidth);

/*********************************************************************
* @purpose  Returns ip default ttl
*
* @param    void
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpDefaultTTLGet();

/*********************************************************************
* @purpose  Sets Ip default ttl
*
* @param    ttl  time to live
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpDefaultTTLSet(L7_uint32 ttl);

/*********************************************************************
* @purpose  Returns contents of the specified ARP entry
*
* @param    ipAddr     @b{(input)} IP address of the ARP entry
* @param    intIfNum   @b{(input)} Internal interface number of the entry
* @param    *pArp      @b{(input)} pointer to output location to store
*                                    ARP entry information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    could not find requested ARP entry
* @returns  L7_FAILURE
*
* @notes    
* The intIfNum field in pArp will be L7_INVALID_INTF if no interface
* explicitly configured for the entry.
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryGet(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
    L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Returns contents of the next ARP entry following the one 
*           specified
*
* @param    ipAddr     @b{(input)} IP address of the ARP entry to 
*                                    start the search
* @param    intIfNum   @b{(input)} Internal interface number of the entry
* @param    *pArp      @b{(input)} pointer to output location to store
*                                    ARP entry information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more ARP entries exist
* @returns  L7_FAILURE
*
* @notes    
* The intIfNum field in pArp will be L7_INVALID_INTF if no interface
* explicitly configured for the entry.
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryNext(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum,
    L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Purges a specific dynamic/gateway entry from the ARP cache
*
* @param    ipAddr     @b{(input)} IP address of the ARP entry to purge 
* @param    intIfNum   @b{(input)} Internal interface number of the entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not support deleting local/static ARP entries, as there
*           are other APIs for handling those.
*       
*           If intIfNum is L7_INVALID_INTF, ARP entries for IP Address on all
*           interfaces will be purged.
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryPurge(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Clears the ARP cache of all dynamic/gateway entries
*
* @param    gateway    @b{(input)} flag to clear gateway entries as well
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All dynamic entries currently in the ARP cache are cleared.  
*           The gateway entries are conditionally cleared based on the
*           value of the gateway parm.  No local/static entries are 
*           affected.
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpCacheClear(L7_BOOL gateway);

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     pointer to ARP cache stats structure to be filled in
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for 'show' functions
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpCacheStatsGet(L7_arpCacheStats_t *pStats);

/*********************************************************************
* @purpose  Returns Arp entry ageout time
*
* @param    void
*
* @returns  L7_uint32  Arp entry ageout time
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpArpAgeTimeGet();

/*********************************************************************
* @purpose  Sets the ARP entry ageout time
*
* @param    L7_uint32  ARP entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpAgeTimeSet(L7_uint32 arpAgeTime);

/*********************************************************************
* @purpose  Returns Arp request response timeout value
*
* @param    void
*
* @returns  L7_uint32  Arp request response timeout value 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpArpRespTimeGet();

/*********************************************************************
* @purpose  Sets the ARP request response timeout
*
* @param    L7_uint32  ARP request response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpRespTimeSet(L7_uint32 arpRespTime);

/*********************************************************************
* @purpose  Returns Arp request max retries count
*
* @param    void
*
* @returns  L7_uint32  Arp request max retries count value
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpArpRetriesGet();

/*********************************************************************
* @purpose  Sets the ARP request max retries count
*
* @param    L7_uint32  ARP request max retries count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpRetriesSet(L7_uint32 arpRetries);

/*********************************************************************
* @purpose  Returns maximum number of entries allowed in ARP cache
*
* @param    void
*
* @returns  L7_uint32  Arp cache entries max value
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpArpCacheSizeGet();

/*********************************************************************
* @purpose  Sets the maximum number of entries allowed in ARP cache
*
* @param    L7_uint32  ARP cache entries max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpCacheSizeSet(L7_uint32 arpCacheSize);

/*********************************************************************
* @purpose  Returns ARP dynamic entry renew mode
*
* @param    void
*
* @returns  L7_uint32  Arp dynamic renew mode value (L7_ENABLE, L7_DISABLE)
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpArpDynamicRenewGet();

/*********************************************************************
* @purpose  Sets the ARP dynamic entry renew mode
*
* @param    L7_uint32  Arp dynamic renew mode value (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpArpDynamicRenewSet(L7_uint32 arpDynamicRenew);

/*********************************************************************
* @purpose  Queries the ARP entry from the hardware
*
* @param    pArp @b{(input)} ARP Query Entry
*
* @returns  L7_SUCCESS  on successful query of arp entry
* @returns  L7_FAILURE  if the query fails
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpArpEntryQuery(L7_arpQuery_t *pArp);

/*********************************************************************
* @purpose  Returns L7_ENABLE if Ip forwards net-directed broadcasts
*
* @param    void
*
* @returns  L7_ENABLE   If Ip net-directed bcasts is enabled
* @returns  L7_DISABLE  If Ip net-directed bcasts is disabled
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpNetDirectBcastsGet();

/*********************************************************************
* @purpose  Configures Ip regarding forwarding net directed broadcasts
*
* @param    L7_uint32  mode - enables or disables net-direct bcast mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpNetDirectBcastsSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Determines if Ip Source Checking is enabled or disabled
*
* @param    void
*
* @returns  L7_ENABLE   If Ip Source Checking is enabled
* @returns  L7_DISABLE  If Ip Source Checking is disabled
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapIpSourceCheckingGet();

/*********************************************************************
* @purpose  Sets the Ip Source Checking mode
*
* @param    L7_uint32 mode - enables or disables Ip Source Checking mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpSourceCheckingSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Return Router Preference
*
* @param    index   router protocol type
*
* @returns  The router preference associated with the specified
* @reutrns  protocol
*
* @notes    Ip state is always ENABLED 
*       
* @end
*********************************************************************/
L7_uint32 ipMapRouterPreferenceGet(L7_uint32 index);

/*********************************************************************
* @purpose  Config Router Preference
*
* @param    index   router protocol type
* @param    pref    preference associated with the protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE Preference value out of range
* @returns  L7_ERROR   Invalid preference change
* @returns  L7_ALREADY_CONFIGURED Preference value is in use by another
*                                 protocol
*
* @notes    Ip state is always ENABLED 
*       
* @end
*********************************************************************/
L7_RC_t ipMapRouterPreferenceSet(L7_uint32 index, L7_uint32 pref);

/*********************************************************************
* @purpose  Return the preference type for a route type
*
* @param    protocol @b{(input)} router protocol type
*
* @returns  The preference type associated with the specified
* @returns  protocol
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ipMapRouterPrefTypeGet(L7_uint32 protocol);


/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/
/********************* ROUTER GLOBAL APIS ****************************/


/*********************************************************************
* @purpose  Get the administrative mode of the router
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE 
*       
* @notes   
*
*
* @end
*********************************************************************/
L7_uint32  ipMapRtrAdminModeGet(void);

/*********************************************************************
* @purpose  Enable or disable the router.
*
* @param    mode     @b; L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes    This command is a global switch for enabling all routing
* @notes    functions, including forwarding and various routing protocols.
* @notes    Compare this command with ipMapIpForwardingSet(), which       
* @notes    simply enables or disables IP forwarding.
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrAdminModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of TOS forwarding
*
* @param    void
*
* @returns  mode     @b; L7_ENABLE or L7_DISABLE 
*       
* @notes   none
*
* @end
*********************************************************************/
L7_uint32  ipMapRtrTosForwardingModeGet(void);

/*********************************************************************
* @purpose  Enable or disable TOS forwarding
*
* @param    mode     @b; L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrTosForwardingModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE 
*       
* @notes   none
*
* @end
*********************************************************************/
L7_uint32  ipMapRtrICMPRedirectModeGet(void);


/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
*       
* @end
*********************************************************************/
L7_RC_t ipMapICMPRedirectModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Echo Replies
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE 
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrICMPEchoReplyModeGet(L7_uint32 *mode);

/*********************************************************************
* @purpose  Set Ignore mode for ICMP ECHO RQUESTS
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrICMPEchoReplyModeSet(L7_uint32 mode);

/*********************************************************************
* @purpose  setting ICMP Rate Limiting parameters like burst size and interval.
*
* @param    burstSize @b{(input)} number of ICMP messages the IPMAP is allowed to per
                                   interval
* @param    interval   @b{(input)} The time interval between tokens being placed
                                   in the bucket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  To disable ICMP rate limiting, set the interval to zero.
*
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrICMPRatelimitSet(L7_uint32 burstSize, L7_uint32 interval);

/*********************************************************************
* @purpose  Get ICMP Rate Limiting parameters like burst size and interval.
*
* @param    burstSize @b{(input)} number of ICMP messages the IPMAP is allowed to per
                                   interval
* @param    interval   @b{(input)} The time interval between tokens being placed
                                   in the bucket
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrICMPRatelimitGet(L7_uint32 *burstSize, L7_uint32 *interval);

/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/
/********************* ROUTER INTERFACE APIS *************************/

/*********************************************************************
* @purpose  Get the gratuitous ARP administrative status for an interface.
*
* @param    intIfNum @b{(input)} internal interface number
* @param    gratArpStatus @b{(output)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if interface is not a configurable interface
*       
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t ipMapGratArpGet(L7_uint32 intIfNum, L7_uint32 *gratArpStatus);

/*********************************************************************
* @purpose  Enable or disable gratuitous ARP on an interface.
*
* @param    intIfNum @b{(input)} internal interface number
* @param    grapArpState  @b{(input)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes   none
*
*       
* @end
*********************************************************************/
L7_RC_t ipMapGratArpSet(L7_uint32 intIfNum, L7_uint32 gratArpState);

/*********************************************************************
* @purpose  Determine whether proxy ARP is enabled on an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *proxyArpMode @b{(output)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapProxyArpGet(L7_uint32 intIfNum, L7_uint32 *proxyArpMode);

/*********************************************************************
* @purpose  Enable or disable proxy ARP on an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    proxyArpMode    @b{(input)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapProxyArpSet(L7_uint32 intIfNum, L7_uint32 proxyArpMode);

/*********************************************************************
* @purpose  Determine whether local proxy ARP is enabled on an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *localProxyArpMode @b{(output)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapLocalProxyArpGet(L7_uint32 intIfNum, L7_uint32 *localProxyArpMode);

/*********************************************************************
* @purpose  Enable or disable local proxy ARP on an interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    localProxyArpMode    @b{(input)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure 
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapLocalProxyArpSet(L7_uint32 intIfNum, L7_uint32 localProxyArpMode);

/*********************************************************************
* @purpose  Determine whether an interface is configured to forward
*           IPv4 network directed broadcast packets.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE   
* @returns  L7_DISABLE  
*
* @notes    Returns L7_DISABLE if interface is not configurable.
*
* @end
*********************************************************************/
L7_uint32 ipMapNetDirectBcastsCfgGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Determine whether an interface is configured to forward
*           IPv4 network directed broadcast packets.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE   
* @returns  L7_DISABLE  
*
* @notes    Returns L7_DISABLE if interface is not configurable.
*       
* @end
*********************************************************************/
L7_uint32 ipMapRtrIntfNetDirectBcastsGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Enable or disable forwarding of net directed broadcasts
*
* @param    intIfNum @b; Internal Interface Number
* @param    mode     @b; L7_ENABLE or L7_DISABLE  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfNetDirectBcastsSet( L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether a given interface is unnumbered and if it
*           is, determine the interface whose address the unnumbered 
*           interface borrows.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    isUnnumbered @b{(output)} L7_TRUE or L7_FALSE
* @param    numberedIfc @b{(output)} internal interface number of the 
*                                    numbered interface. Optional.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface not configurable or isUnnumbered NULL
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t ipMapUnnumberedGet(L7_uint32 intIfNum, L7_BOOL *isUnnumbered,
                           L7_uint32 *numberedIfc);

/*********************************************************************
* @purpose  Returns L7_TRUE if a given interface is configured to be 
*           unnumbered.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL ipMapIntfIsUnnumbered(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the unnumbered status of an interface
*
* @param    intIfNum  @b {(input)}  Internal Interface Number
* @param    isUnnumbered  @b{(input)}  L7_TRUE or L7_FALSE 
* @param    numberedIfc  @b{(input)}  internal interface number of the 
*                                     numbered interface whose IP address the
*                                     unnumbered interface is to borrow
*
* @returns  L7_SUCCESS
* @returns  L7_ALREADY_CONFIGURED if setting interface unnumbered but an 
*                                 IP address is already configured on the 
*                                 interface
* @returns  L7_FAILURE 
*
* @notes    Fails if an address is already configured on the interface.
*       
* @end
*********************************************************************/
L7_RC_t ipMapUnnumberedSet(L7_uint32 intIfNum, L7_BOOL isUnnumbered,
                           L7_uint32 numberedIfc);

/*********************************************************************
* @purpose  Get the borrowed address and mask for an unnnumbered interface
*
* @param    intIfNum   @b{(input)} internal interface number of unnumbered interface
*
* @returns  borrowed IPv4 address. 0 if an error occurs.
* 
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 ipMapBorrowedAddr(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Return administrative mode of router interface participation
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *mode    @b{(output)} L7_ENABLE, if enabled 
* @param    *mode   @b{(output)} L7_DISABLE, if disabled 
*
* @returns  L7_SUCCESS, if successful
* @returns  L7_FAILURE, if failure 
*
* @notes    This routine returns success only if the interface
* @notes    is configured as a router interface.
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Enable or disable interface participation in the router
*
* @param    intIfNum @b; Internal Interface Number
* @param    mode     @b; L7_ENABLE or L7_DISABLE  
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfModeSet( L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Determine whether an interface is configured to forward
*           multicast packets.
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_ENABLE, if enabled
* @returns  L7_DISABLE, if disabled
*
* @notes    Returns the configuration even if the interface is not 
*           currently a routing interface.
*
*           If the interface is not configurable, returns L7_DISABLE
*
* @end
*********************************************************************/
L7_uint32 ipMapMcastFwdModeCfgGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain multicast forwarding admin mode of router interface 
*
* @param    intIfNum @b; Internal Interface Number
*
* @returns  L7_ENABLE, if enabled   
* @returns  L7_DISABLE, if disabled or if the interface is not a 
*           routing interface
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 ipMapRtrIntfMcastFwdModeGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Enable or disable multicas forwarding on router interface
*
* @param    intIfNum @b; Internal Interface Number
* @param    mode     @b; L7_ENABLE or L7_DISABLE  
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfMcastFwdModeSet(L7_uint32 intIfNum, L7_uint32 mode);





/*********************************************************************
* @purpose  Returns the primary IP Address associated with the interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *ipAddr @b{(input)} Pointer to IP address of router interface
* @param    *mask   @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS  if the interface is configured as a router interface
* @returns  L7_FAILURE  if the interface is not configured as a router interface
*
* @notes    This routine returns a valid IP address only if the interface
* @notes    is configured as a router interface.
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfIpAddressGet(L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr, 
																	L7_IP_MASK_t *mask);


/*********************************************************************
* @purpose  Find the network mask associated with a specific IP address
*           on a given interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    ipAddr   @b{(input)} Pointer to IP address of router interface
* @param    mask     @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS    if router interface address found
* @returns  L7_NOT_EXIST  if router interface address not found
* @returns  L7_FAILURE    if interface is a valid IP interface
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfSpecificIpAddressGet(L7_uint32 intIfNum,
										  L7_IP_ADDR_t *ipAddr,
										  L7_IP_MASK_t *mask);

/*********************************************************************
* @purpose  Returns the primary IP Address associated with the routing
*           interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *pIpAddr @b{(input)} Pointer to IP address of router interface
* @param    *pMask   @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine returns the IP address and mask stored in the 
* @notes    configuration file.
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfCfgIpAddressGet(L7_uint32 intIfNum,
									 L7_IP_ADDR_t *ipAddr,
									 L7_IP_MASK_t *mask);

/*********************************************************************
* @purpose  Returns the matching subnet mask on
*           a routing interfaces given the ip address
*
* @param    intIfNum @b{(input)}  Internal Interface Number
* @param    ipAddr   @b{(input)}  IP address
* @param    ipMask   @b{(output)} IP mask
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfSubnetMaskGet(L7_uint32 intIfNum,
                                   L7_IP_ADDR_t ipAddr, 
                                   L7_IP_MASK_t *ipMask);

/*********************************************************************
* @purpose  Returns the IP Address Configuration Method of the Router
*           Interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *method  @b{(output)} Pointer to IP address method of router
*                                 interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine returns the IP address configuration method
*           stored in the configuration file.
*
* @end
*********************************************************************/
L7_RC_t
ipMapRtrIntfCfgIpAddressMethodGet (L7_uint32 intIfNum,
                                   L7_INTF_IP_ADDR_METHOD_t *method);

/*********************************************************************
*
* @purpose  Get IP subnet broadcast address for the primary IP address
*           on the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    bcast      (output) Interface IP broadcast address.
*
* @returns  L7_SUCCESS Interface is valid.
* @returns  L7_FAILURE Interface is not valid.
*
* @notes    Outputs the IP subnet broadcast address, not the layer 2
*           MAC broadcast address.
*
* @end
*********************************************************************/
L7_RC_t ipMapRouterIfIPBroadcastGet (L7_uint32 intIfNum, L7_uint32 *bcast);

/*********************************************************************
*
* @purpose  Get MAC address for the specified interface.
*
* @param    intIfNum   Internal Interface Number
* @param    mac_addr   (output) MAC address for the specified interface.
*
* @returns  L7_SUCCESS 
* @returns  L7_ERROR   NIM says interface not active
* @returns  L7_FAILURE 
*
* @notes    Doesn't touch any IP MAP data. No need to take IP MAP lock.
*
* @end
*********************************************************************/
L7_RC_t ipMapRouterIfMacAddressGet(L7_uint32 intIfNum, L7_uchar8 *mac_addr);

/*********************************************************************
* @purpose  Determine whether IPv4 routing is configured on a given interface.
*
* @param    intIfNum @b{(input)} Internal Interface Number to check
*
* @returns  L7_TRUE if routing is configured
*           L7_FALSE if routing is not configured
*
* @notes    Routing is considered configured under the following conditions:
*              - routing is enabled at the box level
*              - routing is enabled at the interface level
*              - the interface either is configured with a primary IP address, or
*                the interface is configured to be unnumbered
*       
* @end
*********************************************************************/
L7_BOOL ipMapRtrIntfConfigured(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Compares the IP address to see if it exists on the
*           internal interface
*
* @param    intIfNum @b{(input)} Internal Interface Number to check
* @param    *pIpAddr @b{(input)} IP address to check
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfCfgIpAddressCheck(L7_uint32 intIfNum,
									   L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Sets the primary IP address on an interface
*
* @param    intIfNum          Internal interface number out of which address
*                             is reachable
* @param    ipAddress         IP Address of the interface
* @param    subnetMask        Subnet Mask of the interface
* @param    method            IP Address Configuration Method
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_ERROR          subnet conflict between specified IP address and
*                             and address on a network or service port 
* @returns  L7_REQUEST_DENIED Return this error if the ipAddress is same as
*                             next hop address of a static route 
* @returns  L7_ALREADY_CONFIGURED  if one or more secondary addresses are configured
* @returns  L7_NOT_EXIST      if the interface is configured to be unnumbered
* @returns  L7_TABLE_IS_FULL  if the routing table is full and the corresponding
*                             local route can't be added      
* @returns  L7_FAILURE        other errors or failures
*
* @notes    If a primary address is already configured on the interface, removes
*           it first. This will temporarily bring the IP interface down.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressSet(L7_uint32 intIfNum,
                                 L7_IP_ADDR_t ipAddress,
                                 L7_IP_MASK_t subnetMask,
                                 L7_INTF_IP_ADDR_METHOD_t method);

/*********************************************************************
* @purpose  Adds a secondary IP address to an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with this IP Address
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_ERROR          subnet conflict between specified ip
*                             address & an already configured ip
*                             address
* @returns  L7_TABLE_IS_FULL  exceeded maximum number of addresses allowable
* @returns  L7_FAILURE        other errors or failures
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddrAdd(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                       L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Removes the primary IP address on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR        Attempt to remove primary ip address prior
*                           to removing secondary address configuration
*
* @notes    This function is called from usmdb 
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressRemove(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
																		L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Removes a secondary IP address to an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with this IP Address
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_ERROR        invalid input parameters
* @returns  L7_FAILURE      other failures
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfSecondaryIpAddrRemove(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                          L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Removes all IP addresses configured on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_FAILURE      other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfIpAddressesRemove(L7_uint32 intIfNum);
  
/*********************************************************************
* @purpose  Check to see if the ip address conflicts with that of
*           any other routing interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_TRUE         conflict
* @returns  L7_FALSE        no conflict
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_BOOL ipMapRtrIntfAddressConflict(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                    L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Sets the encapsulation type
*
* @param    intIfNum    @b{(input)} Internal Interface Number 
* @param    encapType  @b{(output)}Encapsulation Type (L7_ENCAPSULATION_t)
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfEncapsTypeSet(L7_uint32 intIfNum, 
                                  L7_uint32 encapType);

/*********************************************************************
* @purpose  Add a static ARP entry
*
* @param    ipAddress       IP Address
* @param    pLLAddr         Pointer to link layer address information
*                               in L7_linkLayerAddr_t
* @param    intIfNum        Interface number for the entry. Set to
*                           L7_INVALID_INTF if interface not explicitly given.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpAdd(L7_IP_ADDR_t ipAddress, L7_linkLayerAddr_t *pLLAddr,  L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Delete a static ARP entry
*
* @param    ipAddress       IP Address
* @param    intIfNum        Interface number for the entry. Set to
*                           L7_INVALID_INTF if interface not explicitly given
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpDelete(L7_IP_ADDR_t ipAddress, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Count the number of static ARP entries currently configured.
*
* @param    void
*
* @returns  The number of static ARP entries currently configured.
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipMapStaticArpCount(void);

/*********************************************************************
* @purpose  Get a list of all static ARP entries currently configured.
*
* @param    staticEntries @b{(input/output)} An array of L7_IP_ARP_CACHE_STATIC_MAX 
*                      ARP entries. On return, the array is filled with the
*                      static ARP entries. The first entry with an IP address 
*                      of 0 indicates the end of the list.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if API is called with firstEntry NULL
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapStaticArpGetAll(L7_arpEntry_t *staticEntries);

/*********************************************************************
* @purpose  Verifies if the specified static route is valid
*
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
*
* @returns  L7_TRUE         Specified route is valid
* @returns  L7_FALSE        Specified route is invalid
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_BOOL ipMapStaticRouteAddrIsValid(L7_uint32 ipAddr, L7_uint32 subnetMask);

/*********************************************************************
* @purpose  Add a static route entry
*
* @param    *pStaticRoute   Pointer to L7_rtrStaticRoute_t structure
* @param     unit           index of the unit
*
* @returns  L7_SUCCESS          If the route was successfully added to the
*                               configuration
* @returns  L7_FAILURE          If the specified IP address is invalid
* @returns  L7_ERROR            If the maximum number of configurable 
*                               next hops for the specified network has
*                               been exceeded
* @returns  L7_TABLE_IS_FULL    If the maximum number of configurable
*                               static routes has been exceeded
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapStaticRouteAdd(L7_uint32 unit,L7_rtrStaticRoute_t *pStaticRoute);

/*********************************************************************
* @purpose  Delete a static route entry
*
* @param    *pStaticRoute   Pointer to L7_rtrStaticRoute_t structure
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If pStaticRoute->nextHopRtr is 0, all next hops to the destination
*           are deleted.
*                                                   
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapStaticRouteDelete(L7_rtrStaticRoute_t *pStaticRoute);

/*********************************************************************
* @purpose  Return the preference of a static route to the default preference.
*
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    oldPref         Existing (non-default) preference of the route.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*                                                                                              
* @end
*********************************************************************/
L7_RC_t ipMapStaticRoutePrefRevert(L7_IP_ADDR_t ipAddr,
                                   L7_IP_MASK_t subnetMask, L7_uint32 oldPref);
/*********************************************************************
* @purpose  Get a list of all static routes
*
* @param    *pStaticRoute   Pointer to L7_routeEntry_t structure
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function populates the array staticEntries with 
*           all the configured static routes.
*
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticRouteGetAll(L7_routeEntry_t *staticEntries);

/*********************************************************************
* @purpose  Get the next static route entry expected in snmp order
*
* @param    *ipaddr          @b{(input/output)} route network
* @param    *ipmask          @b{(input/output)} network mask
* @param    *preference      @b{(input/output)} route preference
* @param    *nexthopaddr     @b{(input/output)} next hop
* @param    *nhIfNum         @b{(input/output)} next hop interface number
*
* @returns  L7_SUCCESS      If next static route is found
* @returns  L7_FAILURE      If next static route is not found
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticRouteGetNext(L7_uint32 *ipaddr, L7_uint32 *ipmask, L7_uint32 *preference,
                                L7_uint32 *nexthopaddr, L7_uint32 *nhIfNum);

/*********************************************************************
* @purpose  Get the nexthop interface number corresponding to the
*           static route entry
*
* @param    ipaddr          @b{(input)} route network
* @param    ipmask          @b{(input)} network mask
* @param    pref            @b{(input)} route preference
* @param    nexthop         @b{(input)} next hop
* @param    *index          @b{(output)} interface index
*
* @returns  L7_SUCCESS      If matching route is found
* @returns  L7_FAILURE      If matching route is not found
*
* @notes    For reject routes, we return cpu internal interface number as the nexthop
*           interface, since for such routes the packets are destined to CPU.
*
* @end
*********************************************************************/
L7_RC_t ipMapStaticRouteIfindexGet(L7_uint32 ipaddr, L7_uint32 ipmask,
                                   L7_uint32 pref, L7_uint32 nexthop, L7_uint32 *index);

/*********************************************************************
* @purpose  Determine if IP interface exists
*
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE   
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ipMapIpIntfExists(L7_uint32 intIfNum);

/* Interface mapping routines*/
/*********************************************************************
* @purpose  Convert Router Interface Number to Internal Interface Number
*
* @param    rtrIntf     router interface number
* @param    *intIfNum   pointer to internal interface number
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
L7_RC_t ipMapRtrIntfToIntIfNum(L7_uint32 rtrIfNum, L7_uint32* intIfNum);

/*********************************************************************
* @purpose  Find the interface attached to a subnet containing a given
*           IP address. 
*
* @param    ipAddr    ({input})  IP address to look for
* @param    intIfNum  ({output}) internal interface number of interface 
*                                containing the specified address 
*
* @returns  L7_SUCCESS  if an interface is found containing given address
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRouterIfResolve(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum);

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
L7_RC_t ipMapIntIfNumToRtrIntf(L7_uint32 intIfNum, L7_uint32* rtrIfNum);

/*********************************************************************
* @purpose  Determine whether a given IPv4 address is configured on a 
*           routing interface.
*
* @param    ipAddr    ({input})  IP address to look for
* @param    intIfNum  ({output}) internal interface number of interface with
*                                the specified address 
*
* @returns  L7_SUCCESS  if an interface is found with given address
* @returns  L7_FAILURE  otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapIpAddressToIntf(L7_IP_ADDR_t ipAddr, L7_uint32 *rtrIfNum);


/*********************************************************************
* @purpose  Register a routine to be called for routing events  
*
* @param    routerProtocol_ID     routine router protocol id  
*                                 (See L7_IPMAP_REGISTRANTS_t)      
* @param    *name                 pointer to the name of the registered routine,
*                                 up to IPMAP_NOTIFY_FUNC_NAME_SIZE characters will be stored.
* @param    *notify               pointer to a routine to be invoked for 
*                                 routing event changes as listed in 
*                                 L7_RTR_EVENT_CHANGE_t
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Pass intIfNum as 0 for non-interface specific events.
*                                 
* @end
*********************************************************************/
L7_RC_t ipMapRegisterRoutingEventChange(L7_uint32 routerProtocol_ID, L7_char8 *name, 
                                        L7_RC_t (*notify)(L7_uint32 intIfNum,
                                                          L7_uint32 event,
                                                          void *pData, 
                                                          ASYNC_EVENT_NOTIFY_INFO_t *response));


/*********************************************************************
* @purpose  DeRegister a routine to be called for routing events  
*
* @param    routerProtocol_ID     routine router protocol id  
*                                 (See L7_IPMAP_REGISTRANTS_t)      
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    Pass intIfNum as 0 for non-interface specific events.
*                                 
* @end
*********************************************************************/
L7_RC_t ipMapDeregisterRoutingEventChange(L7_uint32 routerProtocol_ID);


/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    void
*
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @notes    
*       
* @end
*********************************************************************/
L7_uint32 ipMapTraceModeGet(void);


/*********************************************************************
* @purpose  Calculates the CheckSum of the IP Header
*
* @param    ip_data @b{(input)} pointer to the start of the IP header
* @param    hlen L7_uint32 the length of the IP header
* @param    *cksum      L7_ushort16 Checksum Value
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapIpHeaderCksumCalculate(void *ip_data, L7_uint32 hlen, L7_ushort16 *cksum);


/*********************************************************************
* @purpose  Receives IP packets from sysnet
*
* @param    netBufHandle    Handle to buffer
* @param    pduInfo         pointer to pdu info structure
* @param                    which stores intIfNum and vlanId     
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*           
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapRecvIP(L7_netBufHandle netBufHandle, sysnet_pdu_info_t *pduInfo);

/*********************************************************************
* @purpose  Queue an incoming IP packet to the IP forwarding thread.
*
* @param    netBufHandle    Handle to buffer
* @param    vlanId          ingress VLAN ID
* @param    intIfNum        ingress interface
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapRecvQueue(L7_netBufHandle netBufHandle, 
                       L7_uint32 vlanId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Receives ARP packets from sysnet
*
* @param    netBufHandle    Handle to buffer
* @param    pduInfo         pointer to pdu info structure
* @param                    which stores intIfNum and vlanId     
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR  - Frame has not been consumed.
*           
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpRecvIP(L7_netBufHandle netBufHandle, sysnet_pdu_info_t *pduInfo);

/*********************************************************************
* @purpose  Queue an incoming ARP packet to the IP MAP forwarding thread.
*
* @param    netBufHandle    Handle to buffer
* @param    vlanId          Ingress VLAN ID
* @param    intIfNum        Ingress interface
*
* @returns  L7_SUCCESS  - Frame has been consumed.
* @returns  L7_FAILURE  - Frame has not been consumed.
* @returns  L7_ERROR    - Frame has not been consumed.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpRecvQueue(L7_netBufHandle netBufHandle, 
                          L7_uint32 vlanId, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the vlan corresponding to this interface number
*
* @param    intIfNum		Internal interface number of this vlan
* @param    vlanid			pointer to vlan id output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
* @notes    
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapVlanRtrIntIfNumToVlanId(L7_uint32 intIfNum, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Get the internal interface number of the VLAN routing 
*           interface with a given VLAN ID.
*
* @param    vlanid    @b{(input)}  VLAN ID
* @param    intIfNum  @b{(output)} internal interface number of VLAN routing interface
*
* @returns  L7_SUCCESS if VLAN is a VLAN routing interface
*           L7_FAILURE if VLAN is not a VLAN routing interface
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t ipMapVlanRoutingIntfGet(L7_uint32 vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Determine if the interface type is valid for participation in the component
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL ipMapIsValidIntfType(L7_uint32 sysIntfType);
 
/*********************************************************************
* @purpose  Determine if the interface is valid to participate in the component
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL ipMapIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the first ip address of any routing interface
*
* @param    ipAddress       IP Address of the interface
*
* @returns  L7_SUCCESS      An address was found
* @returns  L7_FAILURE      No addresses were configured
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfFirstAddress(L7_uint32 *ipAddress);


/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Unreachables
*
* @param    void
*
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrICMPUnreachablesModeGet(L7_uint32 intIfNum, L7_uint32 * mode);


/*********************************************************************
* @purpose  Enable or disable sending ICMP Unreachables
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*                      or other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrICMPUnreachablesModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    intIfNum @b{(input)} internal interface number
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*                      or other failure
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t ipMapIfICMPRedirectsModeSet (L7_uint32 intIfNum , L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t ipMapIfICMPRedirectsModeGet(L7_uint32 intIfNum, L7_uint32 * mode);

/*********************************************************************
* @purpose  Returns the list of IP addresses associated with an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       addresses associated with the specified interface. 
*
* @returns  L7_SUCCESS  The interface is configured as a router interface
*                       and the address information could be successfully
*                       retrieved
* @returns  L7_FAILURE  The interface is not a valid IP interface
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS. Caller must provide a buffer of
*           this size to store all IP addresses associated with this interface.
*
*           Equivalent to ipMapRtrIntfCfgIpAddrListGet().
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfIpAddrListGet(L7_uint32 intIfNum, L7_rtrIntfIpAddr_t *ipAddrList);

/*********************************************************************
* @purpose  Returns the list of IP Addresses associated with the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       Addresses associated with the specified interface. 
*
* @returns  L7_SUCCESS  The interface is configured as a router interface
*                       and the address information could be successfully
*                       retrieved
* @returns  L7_FAILURE  The interface is not configured as a router interface
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS, please provide a buffer of
*           this size to store all IP Addresses associated with this inter-
*           face.  This function differs from ipMapRtrIntfIpAddrListGet
*           because it returns the configured list of addresses, not the
*           currently running list
*       
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfCfgIpAddrListGet(L7_uint32 intIfNum, L7_rtrIntfIpAddr_t *ipAddrList);

/*********************************************************************
* @purpose  Get the index associated with the secondary ip address
*           for the specified routing interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number associated
*                       with this router port.
* @param    ipAddr      @b{(input)} Secondary IP Address
* @param    ipMask      @b{(input)} Subnet mask associated with the IP Address
* @param    index       @b{(output)} Router interface index.
*
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t  ipMapRtrIntfSecondaryAddrIndexGet(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                           L7_IP_MASK_t ipMask, L7_uint32 *index);

/*********************************************************************
* @purpose  Checks if the specified ip address matches the Net Directed
*           Broadcast address of one of the ip addresses configured
*           on the given interface.
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddr      @b{(input)} IP address to check
* @param    pMacAddr    @b{(output)} Pointer to a buffer to hold the Net
*                       Directed Broadcast MAC Addresses if a match is
*                       found
*
* @returns  L7_SUCCESS  Match found
* @returns  L7_FAILURE  Could not find a match
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t  ipMapNetDirBcastMatchCheck(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr,
                                    L7_uchar8 *pMacAddr);

/*********************************************************************
* @purpose  Determine if specified IP Address lies in the local subnet
*           for the given interface
*
* @param    intIfNum      Internal interface number
* @param    ipAddr        IP Address to be checked
*
* @returns  L7_TRUE    The IP Address lies in the local subnet
* @returns  L7_FALSE   The IP Address does not lie in the local subnet
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ipMapIpAddrIsLocal(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr);


/*********************************************************************
* @purpose  Determine if specified IP Address exactly matches a configued
*           IP Address for the given interface
*
* @param    intIfNum      Internal interface number
* @param    ipAddr        IP Address to be checked
*
* @returns  L7_TRUE    The IP Address lies in the local subnet
* @returns  L7_FALSE   The IP Address does not lie in the local subnet
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ipMapIpAddrMatchesConfigured(L7_uint32 intIfNum, L7_IP_ADDR_t ipAddr);

/*********************************************************************
* @purpose  Determine whether the specified interface supports the
*           specified capability.
*
* @param    intIfNum      Internal interface number
* @param    capability    Capability to be checked
*
* @returns  L7_TRUE    The interface supports the capability
* @returns  L7_FALSE   The interfaces does not support the capability
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL ipMapIntfSupports(L7_uint32 intIfNum, ipIntfCapability_t capability);

/*********************************************************************
* @purpose  Determine whether a given IP interface is up for IPv4. 
*
* @param    intIfNum @b{(input)} internal interface number
* @param    ifState  @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    An interface is considered up if IP MAP has issued an
*           L7_RTR_INTF_ENABLE for it. The interface is operational and
*           ready for use.
*
* @end
*********************************************************************/
L7_RC_t ipMapRtrIntfOperModeGet(L7_uint32 intIfNum, L7_uint32 *ifState);

/*********************************************************************
* @purpose  Given a neighbor's IPv4 address, get the local IPv4 address 
*           on a given interface in the same subnet. 
*
* @param    nbrAddr     @b{(input)}  neighbor's IP address.
* @param    intIfNum    @b{(input)}  interface to neighbor
* @param    localAddr   @b{(output)} local address in same subnet as neighbor
*
* @returns  L7_SUCCESS if local address found 
* @returns  L7_FAILURE  
*
* @notes    none 
*           
* @end
*********************************************************************/
L7_RC_t ipMapLocalAddrToNeighbor(L7_uint32 nbrAddr, L7_uint32 intIfNum,
                                 L7_uint32 *localAddr);

/*********************************************************************
* @purpose  Put an IP packet on the software forwarding path.
*
* @param    bufHandle   @b{(input)}  mbuf containing packet, starting with L2 header
*                                    Forwarding task takes ownership of mbuf if packet
*                                    is successfully queued.
* @param    rxPort      @b{(input)}  ingress physical port
* @param    rxIntf      @b{(output)} ingress routing interface
* @param    vlanId      @b{(output)} ingress VLAN ID
*
* @returns  L7_SUCCESS if packet is enqueued to the IP MAP forwarding task.
* @returns  L7_FAILURE  
*
* @notes    All packets go on the data packet queue 
*           
* @end
*********************************************************************/
L7_RC_t ipMapPacketForward(L7_netBufHandle bufHandle, L7_uint32 rxPort,
                           L7_uint32 rxIntf, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Indicate that a protocol intends to add routes to RTO following 
*           a warm restart.
*
* @param    protocol   @b{(input)}  protocol to add NSF routes
*
* @returns  L7_SUCCESS 
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t ipMapNsfRouteSource(L7_NSF_PROTOCOL_t protocol);

/*********************************************************************
* @purpose  A protocol that controls NSF routes calls this function to 
*           indicate it has updated RTO with all its initial routes after
*           a warm restart.
*
* @param    protocol   @b{(input)}  protocol to add NSF routes
*
* @returns  L7_SUCCESS 
*
* @notes    
*           
* @end
*********************************************************************/
L7_RC_t ipMapInitialRoutesDone(L7_NSF_PROTOCOL_t protocol);

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
L7_RC_t ipMapInternalVlanIdToRtrIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum);


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
L7_uint32 ipMapInternalVlanRoutingInterfaceGet(L7_uint32 vlanId);

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
L7_RC_t ipMapInternalVlanFindFirst(L7_uint32 *vlanId, L7_uint32 *intIfNum);

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
L7_RC_t ipMapInternalVlanFindNext(L7_uint32 prevVid, L7_uint32 *vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Test if warm restart has completed for IP MAP
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL ipMapWarmRestartCompleted(void);


/*********************************************************************
* @purpose  Test if backup is elected
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL ipMapBackupElected(void);

/*********************************************************************
* @purpose  Apply address conflict detect mode
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAddrConflictDetectApply();

/*********************************************************************
* @purpose  Dump the statistics of the address conflicts detected
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAddrConflictStatsShow();

/*********************************************************************
* @purpose  Reset the statistics of the address conflicts detected
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapAddrConflictStatsReset();

/*********************************************************************
* @purpose  Sets/Resets the DHCP IP address method on an interface
*
* @param    intIfNum          Internal interface number out of which 
*                             address is reachable
* @param    method            IP Address Configuration Method
* @param    actImmediate      Immediate action flag
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_FAILURE        other errors or failures
*
* @notes    If DHCP is configured, this API invokes the DHCP Client to
*           acquire an IP address for this interface.
*           If DHCP is unconfigured, this API invokes the DHCP Client to
*           release the IP address on this interface.
*
* @end
*********************************************************************/
L7_RC_t
ipMapRtrIntfIpAddressMethodSet (L7_uint32 intIfNum,
                                L7_INTF_IP_ADDR_METHOD_t method,
                                L7_BOOL actImmediate);

/*********************************************************************
* @purpose  Get the Global Default Gateway Address
*
* @param    globalDefGwAddr @b{(input)}  Pointer to the default global
*                                        gateway address
* @param    isActive        @b{(input)}  Flag to indicate if the
*                                        default gateway is active or
*                                        configured
*
* @returns  L7_SUCCESS if address is found
* @returns  L7_FAILURE if for any reason address not found
*
* @notes    To get the default gateway route in the RTO, set the 
*           argument 'activeFlag' to TRUE.
*           To get the configured default gateway , set the 
*           argument 'activeFlag' to FALSE.
*
* @end
*********************************************************************/
L7_RC_t
ipMapGlobalDefaultGatewayAddressGet (L7_IP_ADDR_t *globalDefGwAddr,
                                     L7_BOOL isActive);

/*********************************************************************
* @purpose  Get warm start dhcp addr
*
* @param   intIfNum  @b{(input)}  Internal Interface Number
* @param   ipAddr    @b{(output)} Checkpointed DHCP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
ipMapIntfCheckpointDhcpAddrGet (L7_uint32 intIfNum,
                                L7_uint32 *ipAddr);

#ifdef L7_QOS_FLEX_PACKAGE_ACL
/*********************************************************************
 *
 * @purpose Trace ip packets received
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *buff        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void ipMapDebugPacketRxTrace(L7_uint32 intIfNum, L7_uint32 vlanId, L7_ipHeader_t *ipHdr, void *layer4headr);

/*********************************************************************
 *
 * @purpose Trace ip  packets transmitted
 *
 * @param   intIfNum     @b{(input)} Internal Interface Number
 * @param   *buff        @b{(input)} pointer to packet
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void ipMapDebugPacketTxTrace(L7_uint32 intIfNum, L7_uint32 vlanId, L7_char8 *pdu);

/*********************************************************************
 * @purpose  Get the current status of displaying ip packet debug info
 *
 * @param    none
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapDebugPacketTraceFlagGet(L7_uint32 aclNum,L7_BOOL *flag );

/*********************************************************************
 * @purpose  Turns on/off the displaying of ip packet debug info
 *
 * @param    flag         new value of the Packet Debug flag
 *
 * @returns  L7_SUCCESS

 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t ipMapDebugPacketTraceFlagSet(L7_uint32 aclnum, L7_BOOL flag);

#endif
/* End Function Prototypes */

/*********************************************************************
* @purpose  To create Default Routing Vlan
*
* @param    void
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapDefaultRoutingVlanCreate (void);

#endif /* IP_API_H */

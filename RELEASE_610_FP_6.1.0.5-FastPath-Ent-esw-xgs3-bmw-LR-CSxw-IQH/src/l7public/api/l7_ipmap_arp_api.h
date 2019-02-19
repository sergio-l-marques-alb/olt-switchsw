/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    l7_ipmap_arp_api.h
* @purpose     ARP API definitions and functions for the IP Mapping layer
* @component   IP Mapping Layer -- ARP
* @comments    none
* @create      03/21/2001
* @author      gpaussa
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef _L7_IPMAP_ARP_API_H_
#define _L7_IPMAP_ARP_API_H_

#include "l7_common.h"
#include "l7_common_l3.h"


/*--------------------------------------*/
/*  IPM ARP Public Constants            */
/*--------------------------------------*/
#define IP_MAP_ARP_INTF_ALL       0     /* means 'all interfaces' in certain cases */

typedef enum {
  IP_MAP_ARP_CACHE_INSERT = 1,
  IP_MAP_ARP_CACHE_UPDATE,
  IP_MAP_ARP_CACHE_REMOVE,
} ipMapArpCacheEvent_t;


/*--------------------------------------*/
/*  IPM ARP Configuration Definitions   */
/*--------------------------------------*/


/*--------------------------------------*/
/*  IPM ARP Public Structures           */
/*--------------------------------------*/
/* User Function:  Address Resolution Callback             */
/*  -- callback function to handle queued addr resolutions */
/*  -- resultStatus indicates outcome:                     */
/*       L7_SUCCESS: address resolved                      */
/*       L7_FAILURE: resolution failed                     */
typedef void (*ipMapArpResCallback_ft)
(
L7_uint32       cbParm1,                /* callback parm 1 */
L7_uint32       cbParm2,                /* callback parm 2 */
L7_uchar8       *pMacAddr,              /* ptr to resolved MAC address */
L7_RC_t         resultStatus            /* return code of resolution attempt */
);

/* User Function:  Interface Raw Send Entry Point */
/*  -- raw send function to be used by IPM ARP    */
typedef L7_RC_t (*ipmRawSend_ft)
(
L7_uint32         intIfNum,             /* internal interface number */
L7_netBufHandle   netBufHandle          /* SYSAPI net buffer handle */
);

/* User Function:  ARP Cache Event Callback                        */
/*  -- callback function to handle ARP cache adds/updates/deletes  */
typedef void (*ipMapArpCacheCallback_ft)
(
L7_uint32       op,                     /* value from ipMapArpCacheEvent_t */
L7_arpEntry_t   *pArpEntry              /* ARP Entry */
);

/* User Parms:  required by ipMapArpIntfRegister() */
/*  -- interface registration parameters (user must provide all values) */
typedef struct
{
  L7_uint32     ipAddr;                 /* IP addr of router interface */
  L7_uint32     netMask;                /* subnet mask of router interface */
  L7_BOOL       unnumbered;             /* L7_TRUE if interface is unnumbered */
  L7_BOOL       proxyArp;               /* L7_TRUE if proxy ARP enabled */
  L7_BOOL       localProxyArp;          /* L7_TRUE if local proxy ARP enabled */
  L7_uchar8     *pMacAddr;              /* ptr to L2 MAC addr of router intf */
  ipmRawSend_ft rawSendFn;              /* address of intf raw send function */
  L7_uint32     sendBufSize;            /* size of send buffers being used */
  L7_uint32     intfHandle;             /* routing intf handle (e.g., pEndObj)*/
} ipMapArpIntfRegParms_t;


/*--------------------------------------*/
/*  IPM ARP API Function Prototypes     */
/*--------------------------------------*/

/*********************************************************************
* @purpose  Initializes the IP Mapping layer ARP component.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Causes Virata ATIC subsystem to be started if it is not already.
*       
* @end
*********************************************************************/
extern L7_RC_t ipMapArpInitPhase1Process(void);

/*********************************************************************
* @purpose  Initializes the IP Mapping layer ARP component.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
extern L7_RC_t ipMapArpInitPhase2Process(void);


/*********************************************************************
* @purpose  Fini's the IP Mapping layer ARP component Phase 1.
*
* @param    void
*
* @notes    cannot stop the atic system
*
* @end
*********************************************************************/
void ipMapArpFiniPhase1Process(void);

/*********************************************************************
* @purpose  Fini's the IP Mapping layer ARP component Phase 2.
*
* @param    void
*
* @notes    cannot stop the atic system
*       
* @end
*********************************************************************/
void ipMapArpFiniPhase2Process(void);

/*********************************************************************
* @purpose  Update the ARP gateway table.
*
* @param    routeEntry  Route information about the network that was added/
*           deleted
* @param    route_status information regarding the reason the callback was
*                        issued.  Either RTO_ADD_ROUTE or RTO_DELETE_ROUTE. 
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpGwTableUpdate(L7_routeEntry_t *routeEntry,
                           L7_uint32 route_status);
   
/*********************************************************************
* @purpose  Registers a routing interface with the IP Mapping layer ARP 
*           function to allow participation in ARP operation.
*
* @param    intIfNum    internal interface number
* @param    pParms      registration parms for this interface
*                         (@b{Input:} Note that the caller MUST fill in a
*                          structure of type ipMapArpIntfRegParms_t COMPLETELY
*                          before calling this function)
*
* @returns  L7_SUCCESS  router interface registration was successful
* @returns  L7_ERROR    maximum number of router interfaces already registered
* @returns  L7_FAILURE  program or usage error
*
* @notes    Each routing interface needs to register with IPM ARP in order
*           to be recognized for ARP-related operations, such as IP address
*           resolution.  The intent is to do this dynamically as routing
*           interfaces are enabled by the user, as opposed to statically
*           registering during system initialization.
*
* @notes    See ipMapArpIntfUnregister() to remove a routing interface from 
*           the IPM ARP operation.  If a routing interface is re-configured
*           from one subnet to another, the old interface must first be
*           unregistered before the new interface definition is registered.
*       
* @notes    The pParms->intfHandle is the interface context as it relates to 
*           the caller (e.g., an END object ptr).  It is treated as an 
*           opaque value in the IP Mapping Layer.
*
* @notes    Assumes the routing table object (RTO) component was previously
*           initialized.
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpIntfRegister(L7_uint32 intIfNum,
                             ipMapArpIntfRegParms_t *pParms);


/*********************************************************************
* @purpose  Unregisters a routing interface from the IP Mapping layer ARP 
*           function.
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    See ipMapArpIntfRegister() to register a routing interface with 
*           the IPM ARP operation.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpIntfUnregister(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Attempts to resolve an IP address to an Ethernet MAC address.
*
* @param    intIfNum    internal interface number
* @param    ipAddr      layer 3 IP address to resolve
* @param    *pMacAddr   ptr to output buffer where MAC address is returned if 
*                       ipAddr is already in the ARP cache
* @param    pCallbackFn address of callback function for asynchronous response
* @param    cbParm1     callback parameter 1 
* @param    cbParm2     callback parameter 2 
*
* @returns  L7_SUCCESS          address was resolved, MAC addr returned via pMacAddr
* @returns  L7_ASYNCH_RESPONSE  resolution in progress. will notify asynchronously 
*                               through callback.
* @returns  L7_FAILURE          cannot resolve address
*
* @notes    An L7_ASYNCH_RESPONSE return code indicates the address was not found in
*           the ARP cache and is in the process of being resolved.  If the
*           pCallbackFn pointer is non-null, an asynchronous callback will be 
*           made indicating the outcome of the resolution attempt.  If the 
*           pCallbackFn is null, there will not be a notification should the 
*           address get resolved.
*
* @notes    An L7_FAILURE can occur due to temporary resource constraints
*           (such as a full queue), or other programming/usage errors.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpAddrResolve(L7_uint32 intIfNum, L7_uint32 ipAddr,
                            L7_uchar8 *pMacAddr, 
                            ipMapArpResCallback_ft pCallbackFn,
                            L7_uint32 cbParm1, L7_uint32 cbParm2);

/*********************************************************************
* @purpose  Called when an ARP entry is used for software forwarding. 
*
* @param    ipAddr      IP address of neighbor
* @param    intIfNum    internal interface number of interface where packet
*                       is forwarded
*
* @notes    Increments a per ARP entry counter. If entry is older than
*           L2 table age, send an occasional ARP request to stimulate
*           station learning.
*
* @end
*********************************************************************/
extern
void ipMapArpAddrUsed(L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Sends a gratuitous ARP on a specified interface.
*
* @param    intIfNum    internal interface number of interface where ARP
*                       is to be sent.
* @param    localIpAddr local IP address to be sent as the target IP
*
* @returns  L7_SUCCESS  gratuitous ARP successfully sent
* @returns  L7_FAILURE  failed to send gratuitous ARP
*
* @notes    The initial motivation for this is to advise our neighbors 
*           of a new MAC address if the interface has bounced as a 
*           result of a top-of-stack change. Sending a gratuitous ARP
*           can also identify if another station on the attached subnet
*           has been (mis)configured with the same IP address as is 
*           configured on the local interface.
*
*           Unlike ipMapAddrArpResolve(), this function does not check the
*           ARP cache, since local IP addresses will be in the ARP cache. This
*           function does not accept a callback function, since we don't expect
*           to receive a response (and no action is pending on a response). 
*           If we do receive a response, we will handle it in our normal
*           ARP receive processing.
*
* @notes    It is necessary to specify localIpAddr since an interface may have
*           multiple IP addresses configured. This function needs to be called
*           separately for each local address.
*
* @end
*********************************************************************/
L7_RC_t ipMapGratArpSend(L7_uint32 intIfNum, L7_uint32 localIpAddr);

/*********************************************************************
* @purpose  Receive an ARP packet and pass it to the ARP code.
*
* @param    intIfNum    internal interface number
* @param    *pArpData   ptr to flat data buffer containing the layer 3 ARP
*                         packet
*
* @returns  L7_SUCCESS  packet processed 
* @returns  L7_ERROR    packet not processed
* @returns  L7_FAILURE  function failure
*
* @notes    The pData parm must point to the beginning of the layer 3
*           portion of the ARP packet, skipping over all layer 2 header
*           contents (including VLAN tags, etc.).
*
* @notes    This function assumes the packet is contained in a single,
*           contiguous data buffer.
*
* @notes    The packet contents are copied into an ARP frame object.
*           This function never assumes ownership of the original
*           packet buffer, so the caller must take care of its disposal.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpReceive(L7_uint32 intIfNum, L7_uchar8 *pArpData);


/*********************************************************************
* @purpose  Adds a static entry to the ARP table.
*
* @param    ipAddr      layer 3 IP address to resolve
* @param    intIfNum    Internal interface number of the entry
* @param    *pMacAddr   ptr to buffer containing layer 2 MAC address
*
* @returns  L7_SUCCESS  entry added to the ARP table
* @returns  L7_NOT_EXIST the router does not have an interface in the same
*                        subnet as the target of the static entry
* @returns  L7_ERROR    entry could not be added
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes    An L7_ERROR return code indicates the static entry could not
*           be added to the ARP table.  This could occur, for example,
*           because of internal resource limits (perhaps temporary).
*
* @notes    Do not use this function to add a net-directed broadcast
*           ARP entry for a local routing interface.  These are handled
*           by the ipMapArpNetDirBcastAdd() function.
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpStaticEntryAdd(L7_uint32 ipAddr, L7_uint32 intIfNum,
    L7_uchar8 *pMacAddr);


/*********************************************************************
* @purpose  Deletes a static entry from the ARP table.
*
* @param    ipAddr      layer 3 IP address to resolve
* @param    intIfNum    Internal interface number of the entry
*
* @returns  L7_SUCCESS  entry deleted from the ARP table
* @returns  L7_ERROR    entry could not be deleted
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes    An L7_ERROR return code indicates the static entry could not
*           be deleted from the ARP table.  This could occur, for example,
*           because the entry was not found, or there is no routing
*           routing interface registered for a subnet to which this 
*           address belongs.
*
* @notes    Do not use this function to delete a net-directed broadcast
*           ARP entry for a local routing interface.  These are handled
*           by the ipMapArpNetDirBcastDel() function.
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpStaticEntryDel(L7_uint32 ipAddr, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Gets ARP entry in the ARP table maintained in the
* @purpose  Application ARP table.
*
* @param    pArp->ipAddr @b{(input)} The ip address of the requested
*                        arp entry.
* @param    *pArp        @b{(output)} A reference to the ARP entry object
*                        containing information needed to add an entry to
*                        the ARP table.
*
*
* @returns  L7_SUCCESS  on a successful addition of the ARP entry
* @returns  L7_FAILURE  if the addition failed
* @returns  L7_ERROR    invalid parameter received.
*
* @notes    ARP table entries correlate an IP address to a MAC address and
*           Router Interface. The table is built as a result of static entries
*           and information obtained from ARP requests and replies. Dynamic
*           entries are periodically aged.
*
* @notes    The ARP information is obtained from the APPLICATION ARP TABLE
*           and NOT the DRIVER & BELOW as is normally the case with dtl
*           API.
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpEntryGet(L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Handle the ARP entry add collision event in the hardware
*           Winnow the arp cache whenever the ARP entries have
*           crossed a threshold or a h/w collision event occurs
*
* @param    none
*
* @returns  L7_SUCCESS  on a successful handling of the event
* @returns  L7_FAILURE  if the handling failed
*
* @notes    purge the arp entries in the cache and hardware whose
*           last-hit-time is more than L7_IP_ARP_CACHE_LAST_HIT_TIME
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpCollisionProcess(void);

/*********************************************************************
* @purpose  Purge a specific dynamic entry from the ARP cache.
*
* @param    ipAddr      IP address of entry to be purged (in host format)
* @param    intIfNum    Internal interface number of the entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ARP entries for local router interfaces and static ARP
*           entries are not affected by this command.
*
* @notes    To delete a static ARP entry, use the ipMapArpStaticEntryDel()
*           API command.
*       
* @notes    Deletion of a dynamic "gateway" ARP entry causes the entry to be
*           removed from the CPU ARP table; its removal from the NP FIB depends
*           on the conditions determined by the ARP change callback function.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpEntryPurge(L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Clears the ARP table of dynamic (and gateway) entries.
*
* @param    intIfNum    internal interface number whose entries should be
*                         purged, or 0 for all interfaces
*
* @param    allowGw     denotes whether gateway entries should be cleared
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ARP entries for local router interfaces and static ARP
*           entries are not affected by this command.
*
* @notes    If the allowGw parm is set to L7_TRUE, dynamic ARP entries labeled 
*           as "gateway" entries are removed from the CPU ARP table; their 
*           removal from the device FIB depends on the conditions determined by 
*           the ARP change callback function.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpCacheClear(L7_uint32 intIfNum, L7_BOOL allowGw);

/*********************************************************************
* @purpose  Clears the IP stack's ARP cache entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE

* @notes    All the IP stack's ARP cache entries are cleared.
*
* @end
*********************************************************************/
extern 
L7_RC_t ipMapArpSwitchClear(void);

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     @b{(input)} pointer to ARP cache stats structure
*                                   to be filled in
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for 'show' functions.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpCacheStatsGet(L7_arpCacheStats_t *pStats);

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the specified IP address
*
* @param    ipAddr      IP address of the ARP entry
* @param    intIfNum    Internal interface number of the entry
* @param    *pArp       pointer to output location to store ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    could not find requested ARP entry
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpEntryByAddrGet(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum, L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the next sequential
*           IP address following the one specified
*
* @param    ipAddr      IP address of the ARP entry to start the search
* @param    intIfNum    Internal interface number of the entry
* @param    *pArp       pointer to output location to store ARP entry info
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no more ARP entries exist
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpEntryNext(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum, L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Sets the ARP entry ageout time to a new value.
*
* @param    secs        new ARP entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
*
* @notes    none
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpAgeTimeSet(L7_uint32 secs);


/*********************************************************************
* @purpose  Sets the ARP resolution response timeout value.
*
* @param    secs        new ARP response timeout (in seconds)
*
* @returns  L7_SUCCESS
*
* @notes    none
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpRespTimeSet(L7_uint32 secs);


/*********************************************************************
* @purpose  Sets the number of retries for failed ARP resolution requests.
*
* @param    numRetry    number of retry requests
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpRetriesSet(L7_uint32 numRetry);


/*********************************************************************
* @purpose  Sets the ARP cache size.
*
* @param    maxEnt      maximum number of entries allowed in ARP cache
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An ARP cache size of zero is not allowed.  If maxEnt is specified
*           as 0, the default ARP cache size is substituted instead.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpCacheSizeSet(L7_uint32 maxEnt);


/*********************************************************************
* @purpose  Sets the dynamic ARP entry renew mode
*
* @param    mode        dynamic renew mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpDynamicRenewSet(L7_uint32 mode);


/*********************************************************************
* @purpose  Clears the ARP cache of dynamic (and gateway) entries.
*
* @param    intIfNum    internal interface number whose entries should be
*                         purged, or 0 for all interfaces
*
* @param    allowGw     denotes whether gateway entries should be cleared
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ARP entries for local router interfaces and static ARP
*           entries are not affected by this command.
*
* @notes    If the allowGw parm is set to L7_TRUE, dynamic ARP entries labeled 
*           as "gateway" entries are removed from the CPU ARP table; their 
*           removal from the device FIB depends on the conditions determined by 
*           the ARP change callback function.
*       
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpExtenCacheClear(L7_uint32 intIfNum, L7_BOOL allowGw);

/*********************************************************************
* @purpose  Registers a secondary IP Address configured on a routing 
*           interface with the IP Mapping layer ARP function to allow 
*           participation in ARP operation.
*
* @param    intIfNum      Internal interface number
* @param    primaryIp     primary IP address on interface where secondary is added
* @param    secondaryIp   Secondary IP Address to be registered with ARP
* @param    secondaryMask Mask associated with the secondary IP Address
*
* @returns  L7_SUCCESS  router interface registration was successful
* @returns  L7_ERROR    could not find routing interface
* @returns  L7_FAILURE  program or usage error
*
* @notes    Assumes the routing table object (RTO) component was previously
*           initialized.
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpSecondaryIpAddrRegister(L7_uint32 intIfNum,
                                        L7_uint32 primaryIp,
                                        L7_uint32 secondaryIp,
                                        L7_uint32 secondaryMask);

/*********************************************************************
* @purpose  Unregisters the specified secondary ip address with the 
*           ARP component
*
* @param    intIfNum      Internal interface number
* @param    primaryIp     Primary IP address to on interface where secondary is removed
* @param    secondaryIp   Secondary IP Address to be unregistered from ARP
* @param    secondaryMask Mask associated with the secondary IP Address
*
* @returns  L7_SUCCESS  router interface unregistration was successful
* @returns  L7_ERROR    could not find routing interface
* @returns  L7_FAILURE  program or usage error
*
* @notes    Assumes the routing table object (RTO) component was previously
*           initialized.
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpSecondaryIpAddrUnregister(L7_uint32 intIfNum,
                                          L7_uint32 primaryIp,
                                          L7_uint32 secondaryIp,
                                          L7_uint32 secondaryMask);

/*********************************************************************
* @purpose  Sets the proxy ARP mode for an interface
*
* @param    mode        proxy ARP mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function should only be called by IP MAP.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpProxyArpSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Sets the local proxy ARP mode for an interface
*
* @param    mode        proxy ARP mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function should only be called by IP MAP.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpLocalProxyArpSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Registers for ARP Cache update events
*
* @param    pCallbackFn   Callback function
*
* @returns  L7_SUCCESS  Was able to successfully register the function
* @returns  L7_FAILURE  Too many registered functions
*
* @notes    none
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpCacheRegister(ipMapArpCacheCallback_ft pCallbackFn);

/*********************************************************************
* @purpose  Unregisters for ARP Cache update events
*
* @param    pCallbackFn   Callback function
*
* @returns  L7_SUCCESS  If function was registered
* @returns  L7_FAILURE  If function was not registered
*
* @notes    none
*
* @end
*********************************************************************/
extern
L7_RC_t ipMapArpCacheUnregister(ipMapArpCacheCallback_ft pCallbackFn);

#endif /* _L7_IPMAP_ARP_API_H_ */


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_ip_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments tba
*
* @create 01/26/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

  
#ifndef USMDB_IP_API_H
#define USMDB_IP_API_H

#include "l3_comm_structs.h"
#include "l3_defaultconfig.h"
#include "rto_api.h"
#include "dot1q_exports.h"

#define   L7_IP_RTR_ROUTE_DEFAULT_COST    FD_RTR_ROUTE_DEFAULT_COST

/*
**********************************************************************
*                   IP MAPPING LAYER FUNCTION PROTOTYPES
**********************************************************************
*/

/*********************************************************************
* @purpose  Returns Ip Spoofing Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpSpoofingGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets Ip Spoofing mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpSpoofingSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Sets the Ip Mtu value
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param ipMtu      @b{(input)} the ip mtu value
* @param intIfNum   @b{(input)} the intIfNum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntfIpMtuSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 ipMtu);

/*********************************************************************
* @purpose  Gets the Ip Mtu value
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the intIfNum value
* @param ipMtu      @b{(output)} the ip mtu value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntfIpMtuGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 * ipMtu);

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
extern L7_RC_t usmDbIntfMaxIpMtuGet(L7_uint32 intIfNum, L7_uint32 *maxIpMtu);

/*********************************************************************
* @purpose  Gets the IP MTU value being enforced on a given interface
*
* @param intIfNum   @b{(output)} the intIfNum value
* @param ipMtu      @b{(output)} the ip mtu value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If the IP MTU is not configured, it is derived from the link MTU.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntfEffectiveIpMtuGet(L7_uint32 intIfNum, L7_uint32 *ipMtu);

/*********************************************************************
* @purpose  Returns contents of the specified ARP entry
*
* @param    UnitIndex  @b{(input)} the unit for this operation
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
extern L7_RC_t usmDbIpArpEntryGet(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr, 
    L7_uint32 intIfNum, L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Returns contents of the next ARP entry following the one 
*           specified
*
* @param    UnitIndex  @b{(input)} the unit for this operation
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
extern L7_RC_t usmDbIpArpEntryNext(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr, 
    L7_uint32 intIfNum, L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Purges a specific dynamic/gateway entry from the ARP cache
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    ipAddr     @b{(input)} IP address of the ARP entry to purge 
* @param    intIfNum   @b{(input)} Internal interface number of the entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not support deleting local/static ARP entries, as there
*           are other APIs for handling those.
*       
*           Set intIfNum to L7_INVALID_INTF to purge ARP entries for IP address
*           on all the interfaces.
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpEntryPurge(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
    L7_uint32 intIfNum );

/*********************************************************************
* @purpose  Clears the ARP cache of all dynamic/gateway entries
*
* @param    UnitIndex  @b{(input)} the unit for this operation
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
extern L7_RC_t usmDbIpArpCacheClear(L7_uint32 UnitIndex, L7_BOOL gateway);

/*********************************************************************
* @purpose  Clears the IP stack's ARP cache entries
*
* @param    UnitIndex  @b{(input)} the unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All the IP stack's ARP cache entries are cleared.
*
* @end
**********************************************************************/
extern L7_RC_t usmDbIpArpSwitchClear(L7_uint32 UnitIndex); 

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param UnitIndex  @b{(input)}  the unit for this operation
* @param *pStats    @b{(output)} pointer to ARP cache stats structure to
*                                  be filled in
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily used for 'show' functions
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpCacheStatsGet(L7_uint32 UnitIndex, L7_arpCacheStats_t *pStats);

/*********************************************************************
* @purpose  Returns Arp entry ageout time
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp entry ageout time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpAgeTimeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Arp entry ageout time
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpAgeTimeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Add a static ARP entry to the ARP table
*
* @param    unitIndex       UnitIndex
* @param    ipAddress       IP Address
* @param    intIfNum        Interface for the entry
* @param    *macAddr        Pointer to a buffer containing the mac address 
*                           of type L7_uchar8
*
* @returns  L7_SUCCESS  entry added to the ARP table
* @returns  L7_FAILURE  program malfunction or improper usage
*
* @notes    
* intIfNum must be L7_INVALID_INTF if no interface explicitly configured
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticArpAdd(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr, 
    L7_uint32 intIfNum, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  Delete a static ARP entry
*
* @param    unitIndex       Unit Index
* @param    ipAddress       IP Address
* @param    intIfNum        Interface for the entry
*
* @returns  L7_SUCCESS  entry deleted from the ARP table
* @returns  L7_FAILURE  program malfunction or improper usage
*        
* @notes    
* intIfNum must be L7_INVALID_INTF if no interface explicitly given
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpMapStaticArpDelete(L7_uint32 unitIndex, L7_IP_ADDR_t ipAddress,
    L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Count the number of static ARP entries currently configured.
*
* @param    unitIndex       Unit Index
*
* @returns  The number of static ARP entries currently configured.
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_uint32 usmDbIpMapStaticArpCount(L7_uint32 unitIndex);

/*********************************************************************
* @purpose  Get a list of all static ARP entries currently configured.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    staticEntries @b{(input/output)} An array of L7_IP_ARP_CACHE_STATIC_MAX 
*                      ARP entries. On return, the array is filled with the
*                      static ARP entries. The first entry with an IP address 
*                      of 0 indicates the end of the list.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if API is called with firstEntry NULL
*
* @notes     
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpMapStaticArpGetAll(L7_uint32 unitIndex, 
                                         L7_arpEntry_t *staticEntries);

/*********************************************************************
* @purpose  Returns Arp request response timeout value
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp response timeout value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpRespTimeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Arp request response timeout
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp request response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpRespTimeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Returns Arp request max retries count
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp request max retries count value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpRetriesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Arp request max retries count
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp request max retries count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpRetriesSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Returns maximum number of entries allowed in ARP cache
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Ip Arp cache max entries value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpCacheSizeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the maximum number of entries in the ARP cache
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Arp cache entries max
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpCacheSizeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Returns ARP dynamic entry renew mode 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} IP ARP dynamic entry renew mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpDynamicRenewGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the ARP dynamic entry renew mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} IP ARP dynamic entry renew mode value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIpArpDynamicRenewSet(L7_uint32 UnitIndex, L7_uint32 val);


/*********************************************************************
* @purpose  Returns the Outbound Access List checking administrative mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the internal interface number associated
*                               with this interface
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_DISABLE
* @returns  L7_ENABLE  
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpMcastsFwdModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 *val);


/*********************************************************************
* @purpose  Get the gratuitous ARP administrative state for an interface.
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} internal interface number
* @param gratArpState @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE 
*       
* @notes   none
*
* @end
*********************************************************************/
L7_uint32 usmDbGratArpGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                          L7_uint32 *gratArpState);

/*********************************************************************
* @purpose  Enable or disable gratuitous ARP on an interface.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param intIfNum  @b{(input)} internal interface number
* @param gratArpState @b{(input)} L7_ENABLE or L7_DISABLE 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @notes   none
*
*       
* @end
*********************************************************************/
L7_RC_t usmDbGratArpSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                        L7_uint32 gratArpState);

/*********************************************************************
* @purpose  Returns L7_ENABLE if Ip forwards net-directed broadcasts
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the internal interface number associated
*                               with this interface
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpNetDirectBcastsGet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose  Configures Ip regarding forwarding net directed broadcasts
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param intIfNum   @b{(input)} the internal interface number associated
*                               with this interface
* @param val        @b{(input)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpNetDirectBcastsSet(L7_uint32 UnitIndex, L7_uint32 intIfNum,
                                  L7_uint32 val);

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
L7_RC_t usmDbIpUnnumberedGet(L7_uint32 intIfNum, L7_BOOL *isUnnumbered,
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
L7_BOOL usmDbIntfIsUnnumbered(L7_uint32 intIfNum);

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
L7_RC_t usmDbIpUnnumberedSet(L7_uint32 intIfNum, L7_BOOL isUnnumbered,
                             L7_uint32 numberedIfc);


/*********************************************************************
* @purpose  Returns Number of Active Routes
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} Number of Active Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfActiveRoutesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Number of Active Routes
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Number of Active Routes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfActiveRoutesSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Returns Number of Interfaces
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} Number of Interfaces
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfInterfacesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Number of Interfaces
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} Number of Interfaces
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpNumOfInterfacesSet(L7_uint32 UnitIndex, L7_uint32 val);

 
/*********************************************************************
* @purpose  Determines if Ip Source Checking is enabled or disabled
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS   
* @returns  L7_FAILURE  
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpSourceCheckingGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Ip Source Checking mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} L7_ENABLED, L7_DISABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpSourceCheckingSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Determines if Ip is enabled or disabled
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLED
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ip state is always ENABLED 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpStateGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Return Router Preference
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param    origin  @b{(input)} router protocol type
* @param    pref    @b{(output)} preference associated with the protocol
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ip state is always ENABLED 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpRouterPreferenceGet(L7_uint32 UnitIndex, L7_uint32 origin, L7_uint32 *pref);

/*********************************************************************
* @purpose  Config Router Preference
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    origin     @b{(input)} router protocol type
* @param    pref       @b{(input)} preference associated with the protocol
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
L7_RC_t usmDbIpRouterPreferenceSet(L7_uint32 UnitIndex, L7_uint32 origin, L7_uint32 pref);

/*********************************************************************
* @purpose  Gets the router interface mode                     
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number 
* @param    *mode        pointer to mode        
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the router interface mode                     
*
* @param    UnitIndex   Unit Number
* @param    intIfNum    Internal Interface Number 
* @param    mode        Mode (L7_ENABLE or L7_DISABLE)        
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Check if routing is configurable on a given interface
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number 
*
* @returns  L7_TRUE     if "routing" is configurable
* @returns  L7_FALSE    otherwise
*
* @notes    Routing cannot be configured on some interfaces because
*           those interfaces do not support routing.  Others do
*           support routing, but it cannot be turned off (tunnels
*           and loopbacks are examples of the latter).
*       
* @end
*********************************************************************/
L7_BOOL usmDbIpRtrIntfModeConfigurable(L7_uint32 UnitIndex, L7_uint32 intIfNum);

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
L7_RC_t usmDbIpRtrIntfOperModeGet(L7_uint32 intIfNum, L7_uint32 *ifState);

/*********************************************************************
* @purpose  Obtain the subnet configuration of the router interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    *pIpAddr @b{(input)} Pointer to IP address of router interface
* @param    *pMask   @b{(input)} Pointer to subnet mask of router interface
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t  usmDbIpRtrIntfIpAddressGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t *ipAddr, 
                                    L7_IP_MASK_t *mask);
/*********************************************************************
* @purpose  Set an IP address on an interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS        ip address could be successfully configured
* @returns  L7_ERROR          subnet conflict between specified ip
*                             address & an already configured ip
*                             address
* @returns  L7_REQUEST_DENIED Return this error if the ipAddress is same as
*                             next hop address of a static route 
* @returns  L7_ALREADY_CONFIGURED  if the address is already configured on 
*                                  the same interface
* @returns  L7_NOT_EXIST      if the interface is configured to be unnumbered
* @returns  L7_FAILURE        other errors or failures
*
* @notes    The calling function must not allow the user to set a null 
*           IP address value (0.0.0.0) for a router interface.
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfIPAddressSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                   L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Adds a secondary IP address to an interface
*
* @param    UnitIndex       Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with this IP Address
*
* @returns  L7_SUCCESS      ip address could be successfully configured
* @returns  L7_ERROR        subnet conflict between specified ip
*                           address & an already configured ip
*                           address
* @returns  L7_FAILURE      other errors or failures
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfSecondaryIpAddrAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                         L7_IP_ADDR_t ipAddress, L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Remove an IP address on an interface
*
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       IP Address of the interface
* @param    subnetMask      Subnet Mask of the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfIPAddressRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_IP_ADDR_t ipAddress,
                                   L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Removes a secondary IP address configured on an interface
*
* @param    UnitIndex       Unit Number
* @param    intIfNum        Internal interface number out of which address
*                           is reachable
* @param    ipAddress       Secondary IP Address of the interface
* @param    subnetMask      Subnet Mask associated with the secondary IP
*
* @returns  L7_SUCCESS      ip address could be successfully removed
* @returns  L7_ERROR        invalid input parameters
* @returns  L7_FAILURE      other failures
*
* @notes    none 
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpRtrIntfSecondaryIpAddrRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                            L7_IP_ADDR_t ipAddress, L7_IP_MASK_t subnetMask);

/*********************************************************************
* @purpose  Removes all IP addresses configured on an interface
*
* @param    UnitIndex       Unit Number
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
L7_RC_t usmDbIpRtrIntfIpAddressesRemove(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
 * @purpose  Returns the list of IP Addresses associated with the interface
 *
* @param    UnitIndex   Unit Number
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
*           face.
*       
* @end
*********************************************************************/
L7_RC_t  usmDbIpRtrIntfIpAddrListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                     L7_rtrIntfIpAddr_t *ipAddrList);

/*********************************************************************
* @purpose  Returns the list of IP Addresses associated with the interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *ipAddrList @b{(output)} Pointer to a buffer to hold the IP
*                       Addresses associated with the specified interface. 
*
* @returns  L7_SUCCESS  The configured address information was successfully
*                       retrieved
* @returns  L7_ERROR    Invalid buffer pointer
*
* @notes    The maximum number of IP Addresses supported on an interface is
*           specified by L7_L3_NUM_IP_ADDRS, please provide a buffer of
*           this size to store all IP Addresses associated with this inter-
*           face.  This routine differs from usmDbIpRtrIntfIpAddrListGet
*           because it retrieves the configured information rather than
*           the currently running information
*       
* @end
*********************************************************************/
L7_RC_t  usmDbIpRtrIntfCfgIpAddrListGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, 
                                     L7_rtrIntfIpAddr_t *ipAddrList);

/*********************************************************************
* @purpose  Return next route in the routing table. 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    inOutRoute @b{(input/output)} Set to the previous route on 
*            input and set to the next route on output.
* @param    bestRouteOnly @b{(input)}  set to L7_TRUE if the user is 
*                                        interested only in best routes.
*
* @returns  L7_SUCCESS  Next route found.
* @returns  L7_ERROR    Next route is not found.
*
* @notes    This API can be used to walk the routing table, getting either
*           all routes or only best routes. In order to look up the
*           first route all fields of the inOutRoute structure must be
*           set to zero. In order to look up subsequent routes use previous
*           output as input.
*
*           The route returned includes all active next hops in the next route.
*
*           When searching the routing table for the input route, a route 
*           is considered a match if it has the same destination address and
*           mask, the same protocol, and the same preference. 
*
*           Assumes the host bits have been zeroed in inOutRoute->ipAddr.
*
* @end
*********************************************************************/
L7_RC_t usmDbNextRouteEntryGet(L7_uint32 UnitIndex, 
                               L7_routeEntry_t *inOutRoute, L7_uint32 bestRouteOnly);

/*******************************************************************************
* @purpose  It is an utility function to return the next ecmp route from the routing
*           table
*
* @param    UnitIndex   Unit for this operation
* @param    *routeIp    @{(input/output)} Destination network of the route
* @param    *routeMask  @{(input/output)} Destination networkmask of the route
* @param    *protocol   @{(input/output)} Source protocol of the route
* @param    *pref       @{(input/output)} Preference value of the route
* @param    *ipaddr     @{(input/output)} Ip address of the route's next hop
* @param    *IfindexValue @{(input/output)} Interface index of the route's next hop
* @param    routeType   @{(input)}        Route Type (Best or Not)
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*******************************************************************************/
L7_RC_t usmDbGetNextEcmpRoute(L7_uint32 unit, L7_uint32 *routeIp, L7_uint32 *routeMask, L7_uint32 *protocol,
                              L7_uint32 *pref, L7_uint32 *ipaddr, L7_uint32 *IfindexValue, L7_BOOL routeType);

/*********************************************************************
* @purpose  Find the route with the longest matching prefix to a 
*           given destination. 
*
* @param    dest_ip     @b{(input)}  Destination IP address.
* @param    routeEntry  @b{(output)} best route to the destination
*
*
* @returns  L7_FAILURE 	Route not found.
* @returns  L7_SUCCESS  Router found
*
* @notes    If there are multiple routes with the same prefix, always 
*           returns the best route (the route with lowest preference val).
*
* @end
*********************************************************************/
L7_RC_t usmDbBestMatchRouteLookup (L7_uint32 UnitIndex,
                                   L7_uint32 dest_ip, L7_routeEntry_t *routeEntry);

/*********************************************************************
* @purpose  Returns the number of routes in the routing table. 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    bestRoutesOnly  @b{(input)} If set to L7_TRUE, only best 
*                                       routes are counted.
*
* @returns  the number of routes
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 usmDbRouteCount(L7_uint32 UnitIndex, L7_BOOL bestRoutesOnly);

/*********************************************************************
* @purpose  Verifies if the specified static route is valid
*
* @param    UnitIndex       the unit for this operation
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
L7_BOOL usmDbIpStaticRouteAddrIsValid(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                      L7_uint32 subnetMask);

/*********************************************************************
* @purpose  Add a static route entry
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    nextHopRtr      Next Hop Router IP Address
* @param    preference      Preference of this route compared to routes to
*                           the same destination learned from other sources.
*                           If not configured, preference must be set to the
*                           default preference for static routes.
* @param    intIfNum        outgoing interface for the route. L7_INVALID_INTF if
*                           interface not specified
* @param    routeFlags      flags for the route entry type
*
* @returns  L7_SUCCESS          If the route was successfully added to the
*                               configuration
* @returns  L7_FAILURE          If the specified IP address is invalid
* @returns  L7_ERROR            If the maximum number of next hops for the specified 
*                               network and route preference has been exceeded
* @returns  L7_TABLE_IS_FULL    If the maximum number of static routes has been 
*                               exceeded
*
* @notes    If a static route already exists to ipAddr/subnetMask with next
*           hop nextHopRtr, the preference of the existing static route is
*           changed to the preference value passed in the last argument.
*
*           The next hop is accepted even if it is not on a local subnet. The
*           next hop will be added to RTO, which determines whether to announce
*           it to the forwarding table, to ARP, and other RTO registrants.
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticRouteAdd(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                              L7_uint32 subnetMask, L7_uint32 nextHopRtr,
                              L7_uint32 pref, L7_uint32 intIfNum,
                              L7_RT_ENTRY_FLAGS_t routeFlags);


/*********************************************************************
* @purpose  Delete a static route entry
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    nextHopRtr      Next Hop Router IP Address
* @param    intIfNum        outgoing interface for the route. L7_INVALID_INTF if
*                           interface not specified
* @param    routeFlags      flags for the route entry type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The next hop router IP address is generally used to reach the
*           route.  The internal interface number is needed when the route
*           is reachable via an unnumbered line.
*                                                   
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticRouteDelete(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr, 
                                 L7_IP_MASK_t subnetMask, L7_IP_ADDR_t nextHopRtr,
                                 L7_uint32 intIfNum, L7_RT_ENTRY_FLAGS_t routeFlags);

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
L7_RC_t usmDbStaticRouteIfindexGet(L7_uint32 ipaddr, L7_uint32 ipmask,
                                   L7_uint32 pref, L7_uint32 nexthop, L7_uint32 *index);

/*********************************************************************
* @purpose  Return the comparision result between 2 static route entries
*
* @param    ip1        @b{(input)} route network
* @param    mask1      @b{(input)} network mask
* @param    pref1      @b{(input)} route preference
* @param    nh1        @b{(input)} next hop
* @param    nhIfNum1   @b{(input)} next hop interface number
* @param    ip2        @b{(input)} route network
* @param    mask2      @b{(input)} network mask
* @param    pref2      @b{(input)} route preference
* @param    nh2        @b{(input)} next hop
* @param    nhIfNum2   @b{(input)} next hop interface number
*
* @returns  -1     If first route < second route
* @returns  0      If first route == second route
* @returns  1      If first route > second route
*
* @notes
*                                                   
* @end
*********************************************************************/
L7_int32 ipStaticRouteCompare(L7_uint32 ip1, L7_uint32 mask1, L7_uint32 pref1, L7_uint32 nh1, L7_uint32 nhIfNum1,
                              L7_uint32 ip2, L7_uint32 mask2, L7_uint32 pref2, L7_uint32 nh2, L7_uint32 nhIfNum2);

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
L7_RC_t usmDbGetNextStaticRoute(L7_uint32 *ipaddr, L7_uint32 *ipmask, L7_uint32 *preference,
                                L7_uint32 *nexthopaddr, L7_uint32 *nhIfNum);

/*********************************************************************
* @purpose  Revert the preference of a static route to the default preference.
*
* @param    UnitIndex       the unit for this operation
* @param    ipAddr          IP Address of subnet
* @param    subnetMask      Subnet Mask
* @param    pref            Existing (non-default) preference of the route.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*                                                   
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpStaticRoutePrefRevert(L7_uint32 UnitIndex, L7_IP_ADDR_t ipAddr,
                                     L7_IP_MASK_t subnetMask, L7_uint32 pref);

/*********************************************************************
* @purpose  Get all static routes
*
* @param    UnitIndex       The unit for this operation
* @param    staticEntries   Array of static routes of size L7_RTR_MAX_STATIC_ROUTES
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function populates the array staticEntries of type L7_routeEntry_t
*           with all the configured static routes.
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapStaticRouteGetAll(L7_uint32 UnitIndex, L7_routeEntry_t *staticEntries);


/*********************************************************************
* @purpose  Determine if IP interface exists
*
* @param    unitIndex       Unit Index
* @param    intIfNum        Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE   
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL usmDbIpIntfExists(L7_uint32 unitIndex, L7_uint32 intIfNum);

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
L7_RC_t usmDbIpRouterIfResolve(L7_IP_ADDR_t ipAddr, L7_uint32 *intIfNum);
/*********************************************************************
* @purpose  Get the EMCP admin mode.  Enabling ECMP on the router allows
*           it to load share traffic over known equal cost paths.
*
* @param    UnitIndex   @b{(input)} L7_uint32 the unit for this operation
* @param    val         @b{(output)} L7_ENABLE/ L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbIpEcmpModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose check to see if intIfNum is a valid ipmap Interface
*
* @param    unitIndex         Unit Index
* @param    intIfNum          Interface Number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface 
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_BOOL usmDbIpMapIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Get the first valid interface for participation
*           in the component
*
* @param    intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpMapValidIntfFirstGet(L7_uint32 *intIfNum);


/*********************************************************************
* @purpose  Get the next valid interface for participation
*           in the component
*
* @param     prevIfNum  @b{(input)} internal interface number
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t usmDbIpMapValidIntfNextGet(L7_uint32 prevIfNum, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Create a VLAN interface and enable it for routing
*
* @param    UnitIndex   Unit Number
* @param    vlanid      id of the vlan
* @param    intfId      (Optional) interface ID. If non-zero, dot1q uses
*                       this value as the index to its VLAN interface array,
*                       dot1qVlanIntf[]. If 0, dot1q uses the first available
*                       index.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine causes two actions to occur:
*               1. the creation of a NIM interface associated with the
*                  specified VLAN.
*               2. the enabling of the resultant interface for routing.
*
*           The user must use the intIfNum to refer to the routed vlan in 
*           any future correspondence.
*                                                   
*           Once a VLAN has been created as a routing interface, it can be
*           enabled or disabled for routing via usmDbIpRtrIntfModeSet().
*
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRoutingIntfCreate(L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uint32 intfId);
  
/*********************************************************************
* @purpose  Delete a VLAN routing interface
*
* @param    vlanid          id of the vlan
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine causes the deletion of a VLAN routing interface,  
* @notes    of the NIM interface (intIfNum) and  of the routing interface              
* @notes    (i.e. deallocates the router interface number corresponding to
* @notes    the intIfNum assigned to the VLAN interface).  The user will then
* @notes    lose any IP configuration associated with that VLAN interface.
* 
*                                                   
* @notes    Once a VLAN has been created as a routing interface, it can be
* @notes    enabled or disabled for routing via usmDbIpRtrIntfModeSet().
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRoutingIntfDelete(L7_uint32 UnitIndex, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Enable/Disable the wireless attribute on a VLAN interface
*
* @param    vlanId      id of the vlan
* @param    value       L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  If vlanId is valid
* @returns  L7_FAILURE  Otherwise
*
* @notes    This has the effect of setting the "wireless"
*           attribute of the interface and will cause a VLAN routing
*           interface to remain operationally up even when no
*           ports are participating in the VLAN.
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRoutingIntfWirelessSet(L7_uint32 vlanId, L7_uint32 value);

/*********************************************************************
* @purpose  Get the wireless attribute of a VLAN interface
*
* @param    vlanId      id of the vlan
* @param    pValue      returns L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS  If vlanId is valid
* @returns  L7_FAILURE  Otherwise
*
* @notes    none
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbIpVlanRoutingIntfWirelessGet(L7_uint32 vlanId, L7_uint32 *pValue);

/*********************************************************************
* @purpose  Check if routing is enabled on this vlan
*
* @param    vlanid          id of the vlan
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
L7_RC_t usmDbIpVlanRtrVlanIdGet(L7_uint32 UnitIndex, L7_uint32 vlanId);

/*********************************************************************
* @purpose  Get the next vlan after this vlan on which routing is enabled
*
* @param    vlanid          id of the vlan
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
L7_RC_t usmDbIpVlanRtrVlanIdGetNext(L7_uint32 UnitIndex, L7_uint32 *vlanId);

/*********************************************************************
* @purpose  Get the interface number corresponding to this vlan
*
* @param    vlanid          id of the vlan
* @param    intIfNum        Internal interface number of this vlan
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
L7_RC_t usmDbIpVlanRtrVlanIdToIntIfNum(L7_uint32 UnitIndex, L7_uint32 vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the index number corresponding to this vlan interface
*
* @param    UnitIndex   Unit Number
* @param    intIfNum        Internal interface number of this vlan
* @param    index          index of the vlan Id 
*
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
L7_RC_t usmDbIpVlanRtrIntIfNumToVlanId(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *index);

/*********************************************************************
* @purpose  Get the interface ID corresponding to this vlan interface
*
* @param    vlanId          id of the vlan 
* @param    intfId          interface ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The interface ID is an internally assigned integer in the
*           range [1, L7_MAX_NUM_VLAN_INTF]. The user may optionally 
*           specify the interface ID. The interface ID influences the
*           u/s/p and internal interface number assigned to the VLAN
*           interface. It is listed in the text configuration in order
*           to retain the internal interface number and u/s/p across
*           reboots.  
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbVlanIntfIdGet(L7_uint32 vlanId, L7_uint32 *intfId);

/*********************************************************************
* @purpose  Get the interface number corresponding to this internally-assigned
*           VLAN ID
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
L7_RC_t usmDbInternalVlanIdToIntIfNum(L7_uint32 vlanId, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Set the direction from the base VLAN ID when looking for an
*           unused VLAN ID to assign to a VLAN routing interface. 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbInternalVlanPolicySet(e_Internal_Vlan_Policy policy);

/*********************************************************************
* @purpose  Get the direction from the base VLAN ID when looking for an
*           unused VLAN ID to assign to a VLAN routing interface. 
*
* @param    policy  (input)   up or down 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
e_Internal_Vlan_Policy usmDbInternalVlanPolicyGet(void);

/*********************************************************************
* @purpose  Set the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    baseVlanId  (input)   VLAN ID 
*
* @returns  L7_SUCCESS
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_RC_t usmDbBaseInternalVlanIdSet(L7_uint32 baseVlanId);

/*********************************************************************
* @purpose  Get the VLAN ID used as a starting point to internal VLAN assignments.
*
* @param    void
*
* @returns  VLAN ID
*
* @notes   
*                                                   
* @end
*********************************************************************/
L7_uint32 usmDbBaseInternalVlanIdGet(void);

/*********************************************************************
* @purpose  Determine whether proxy ARP is enabled on an interface
*
* @param    UnitIndex @b{(input)} unit number
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
L7_RC_t usmDbProxyArpGet(L7_uint32 UnitIndex, 
                         L7_uint32 intIfNum, 
                         L7_uint32 *proxyArpMode);

/*********************************************************************
* @purpose  Enable or disable proxy ARP on an interface
*
* @param    UnitIndex @b{(input)} unit number
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
L7_RC_t usmDbProxyArpSet(L7_uint32 UnitIndex, 
                         L7_uint32 intIfNum, 
                         L7_uint32 proxyArpMode);

/*********************************************************************
* @purpose  Determine whether local proxy ARP is enabled on an interface
*
* @param    UnitIndex @b{(input)} unit number
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
L7_RC_t usmDbLocalProxyArpGet(L7_uint32 UnitIndex, 
                         L7_uint32 intIfNum, 
                         L7_uint32 *localProxyArpMode);

/*********************************************************************
* @purpose  Enable or disable local proxy ARP on an interface
*
* @param    UnitIndex @b{(input)} unit number
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
L7_RC_t usmDbLocalProxyArpSet(L7_uint32 UnitIndex, 
                         L7_uint32 intIfNum, 
                         L7_uint32 localProxyArpMode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Unreachables
*
* @param    void
*
* @param    UnitIndex   Unit Index
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapICMPUnreachablesModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 * mode);


/*********************************************************************
* @purpose  Enable or disable sending ICMP Unreachables
*
* @param    UnitIndex   Unit Index
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
L7_RC_t usmDbIpMapICMPUnreachablesModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @param    UnitIndex   Unit Index
* @param    intIfNum @b{(input)} internal interface number
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if interface is not a configurable interface
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapIfICMPRedirectsModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 * mode);

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    UnitIndex   Unit Index
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
L7_RC_t usmDbIpMapIfICMPRedirectsModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of sending ICMP Redirects
*
* @param    void
*
* @returns  mode     @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapRtrICMPRedirectsModeGet(L7_uint32 * mode);

/*********************************************************************
* @purpose  Enable or disable sending ICMP Redirects
*
* @param    mode     @b{(input)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
*
* @end
*********************************************************************/
L7_RC_t usmDbIpMapRtrICMPRedirectsModeSet( L7_uint32 mode);

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
L7_RC_t usmDbIpMapRtrICMPEchoReplyModeGet (L7_uint32 * mode);

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
L7_RC_t usmDbIpMapRtrICMPEchoReplyModeSet(L7_uint32 mode);

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
L7_RC_t usmDbIpMapRtrICMPRatelimitSet(L7_uint32 burstSize, L7_uint32 interval);

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
L7_RC_t usmDbIpMapRtrICMPRatelimitGet(L7_uint32 *burstSize, L7_uint32 *interval);

/*********************************************************************
 * @purpose  Sets the bandwidth of the specified interface.
 *
 * @param    intIfNum  @b{(input)} Internal Interface Number
 * @param    bandwidth @b{(input)} bandwidth
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR    if interface does not exist
 * @returns  L7_FAILURE  if other failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbIfBandwidthSet(L7_uint32 intIfNum, L7_uint32 bandwidth);

/*********************************************************************
 * @purpose  Get the bandwidth of the specified interface.
 *
 * @param    intIfNum  @b{(input)} Internal Interface Number
 * @param    bandwidth @b{(output)} bandwidth
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR    if interface does not exist
 * @returns  L7_FAILURE  if other failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbIfBandwidthGet(L7_uint32 intIfNum, L7_uint32 *bandwidth);

/*********************************************************************
 * @purpose  Get the bandwidth of the specified interface without 
 *           making any modification to the SET bandwidth.
 *
 * @param    intIfNum  @b{(input)} Internal Interface Number
 * @param    bandwidth @b{(output)} bandwidth
 *
 * @returns  L7_SUCCESS  if success
 * @returns  L7_ERROR    if interface does not exist
 * @returns  L7_FAILURE  if other failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbIfBWGet(L7_uint32 intIfNum, L7_uint32 *bandwidth);

/*********************************************************************
* @purpose  Get the routing max equal cost entries
*
* @param    UnitIndex @b{(input)} Unit number
* @param    maxhops   @b{(output)} maxhops
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrRouteMaxEqualCostEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxhops);

/*********************************************************************
* @purpose  Get the routing max routes entries 
*
* @param    UnitIndex @b{(input)} Unit number 
* @param    maxhops   @b{(output)} maxroutes 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbRtrRouteMaxRouteEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxroutes);

#endif

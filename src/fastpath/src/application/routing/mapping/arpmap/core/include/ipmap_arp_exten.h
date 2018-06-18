/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename    ipmap_arp_exten.h
* @purpose     ARP mapping internal API functions used by vendor source
* @component   IP Mapping Layer -- ARP
* @comments    none
* @create      08/10/2003
* @author      gpaussa
* @end
*             
**********************************************************************/

#ifndef _IPMAP_ARP_EXTEN_H_
#define _IPMAP_ARP_EXTEN_H_


/* headers */
#include "std.h"
#include "arp.h"


/*********************************************************************
* @purpose  Refresh the age of an existing entry in the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenEntryAgeRefresh(t_Handle arpCacheId, t_ADR *pArpInfo);

/*********************************************************************
* @purpose  Calculate the current age time for a given ARP entry timestamp
*
* @param    timeStamp     {@b(Input)}   ARP entry timestamp (in ticks)
* @param    *pAgeSecs     {@b(Output)}  age time (in seconds) output ptr
*
* @returns  void
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
void ipMapArpExtenEntryAgeCalc(ulng timeStamp, ulng *pAgeSecs);

/*********************************************************************
* @purpose  Handle conversion of an existing entry to static type
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  ARP entry info ptr
*
* @returns  E_OK
* @returns  E_BADPARM   already a permanent entry
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    Performs whatever processing is necessary during the conversion
*           of a dynamic/gateway entry to a static.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenEntryStaticConvert(t_Handle arpCacheId, t_ADR *pArpInfo);

/*********************************************************************
* @purpose  Insert an entry into the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  pointer to ARP information
* @param    **ppArpEntry  {@b(Output)} pointer to ARP entry ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    If the ARP cache is full, an attempt is made to replace the 
*           oldest (least-recently updated) dynamic ARP entry.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheInsert(t_Handle arpCacheId, t_ADR *pArpInfo, 
                               t_ADR **ppArpEntry);

/*********************************************************************
* @purpose  Update the contents of an existing ARP cache entry
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  pointer to ARP information
* @param    macHasChanged {@b(Input)}  indicates ARP entry MAC addr changed
*
* @returns  E_OK
* @returns  E_BADPARM   entry does not exist in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheUpdate(t_Handle arpCacheId, t_ADR *pArpInfo, 
                               Bool macHasChanged); 

/*********************************************************************
* @purpose  Remove an entry from the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    *pArpInfo     {@b(Input)}  pointer to ARP information
*
* @returns  E_OK
* @returns  E_BADPARM   entry does not exist in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    The original ARP entry node is no longer valid upon successful
*           return from this call.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheRemove(t_Handle arpCacheId, t_ADR *pArpInfo);

/*********************************************************************
* @purpose  Remove the older entries from the ARP cache
*
* @param    none
*
* @returns  E_OK
*
* @notes    This function is called whenever there is a hardware failure
*           in adding a new ARP entry (or) whenever the ARP cache size
*           has hit the high threshold.
*           This creates space and the probability for the new entries
*           to be added into the cache.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenPurgeOldEntries(void);

/*********************************************************************
* @purpose  Remove the oldest dynamic entry from the ARP cache
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
* @param    allowGw       {@b(Input)}   indicates if gateway entry can be removed
*
* @returns  E_OK
* @returns  E_BADPARM   no removable entry exists in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    The original ARP entry node is no longer valid upon successful
*           return from this call, so its arpEntry contents are copied to
*           the location specified by the caller.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheOldestRemove(t_Handle arpCacheId, Bool allowGw);

/*********************************************************************
* @purpose  Get the first entry in the ARP cache
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    **ppArpEntry  {@b(Output)} pointer to ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheGetFirst(t_Handle arpCacheId, t_ADR **ppArpEntry);

/*********************************************************************
* @purpose  Get the next sequential entry in the ARP cache
*
* @param    arpCacheId        {@b(Input)}  ARP cache instance identifier
* @param    *pArpEntry        {@b(Input)}  current ARP entry info ptr
* @param    **ppArpEntryNext  {@b(Output)} pointer to next ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheGetNext(t_Handle arpCacheId, t_ADR *pArpEntry, 
                                t_ADR **ppArpEntryNext);

/*********************************************************************
* @purpose  Find the entry in the ARP cache for the specified IP addr
*
* @param    arpCacheId    {@b(Input)}  ARP cache instance identifier
* @param    index         {@b(Input)}  ARP interface index number
* @param    pIpAddr       {@b(Input)}  pointer to IP address (octet array)    
* @param    **ppArpEntry  {@b(Output)} pointer to ARP entry info ptr
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheFind(t_Handle arpCacheId, byte *pIpAddr, 
                             word index, t_ADR **ppArpEntry);

/*********************************************************************
* @purpose  Find the oldest dynamic entry currently in the ARP cache
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
* @param    allowGw       {@b(Input)}   indicates if gateway entry can be removed
* @param    **ppArpEntry  {@b(Output)}  pointer to ARP entry info ptr         
*
* @returns  E_OK
* @returns  E_BADPARM   no removable entry exists in ARP cache
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @notes    There is no guarantee that the entry output by this function  
*           will remain the oldest entry at the time it is removed from the
*           cache, unless external measures are taken to protect the
*           current thread from being disrupted.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheOldestFind(t_Handle arpCacheId, Bool allowGw, 
                                   t_ADR **ppArpEntry);

/*********************************************************************
* @purpose  Change the size of the ARP cache
*
* @param    maxEnt      {@b(Input)}  maximum number of ARP entries allowed
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenCacheResize(word maxEnt);

/*********************************************************************
* @purpose  Check if the ARP cache is currently full
*
* @param    arpCacheId    {@b(Input)}   ARP cache instance identifier
*
* @returns  E_OK
* @returns  E_FAILED
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenCacheFullCheck(t_Handle arpCacheId);

/*********************************************************************
* @purpose  Check if the specified ip address is the primary address
*           on the given interface
*
* @param    index     {@b(Input)}   ARP interface index number
* @param    ipAddr    {@b(Input)}   Ip Address to be tested
*
* @returns  TRUE      This is the primary address on the interface
* @returns  FALSE     This is not the primary address on the interface  
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenIpAddrIsPrimary(word index, byte* pIpAddr);

/*********************************************************************
* @purpose  Check if two addresses on a given interface are in the same
*           IP subnet
*
* @param    index     {@b(Input)}   ARP interface index number
* @param    targetIP  {@b(Input)}   target IP address in an ARP request we're sending
* @param    srcIP     {@b(Input)}   a candidate for the source IP address 
*
* @returns  TRUE      targetIP and srcIP are in the same subnet
* @returns  FALSE     
*
* @notes    This ugly function is intended to be called by the ARP protocol code.
*
* @notes    Do NOT call this function from outside the ARP protocol code as
*           that will result in unprotected access to the ARP interface
*           structure.
*
* @end
*********************************************************************/
Bool ipMapArpExtenSameSubnet(word index, byte *targetIP, byte *srcIP);

/*********************************************************************
* @purpose  Returns internal intf number for intf registerd with ARP.
*
* @param    index     {@b(Input)}   ARP interface index number
* @param    intIfNum  {@b(Output)}  Internal interface number of ARP interface
*
* @returns  E_OK      Success
* @returns  E_FAILED  Could not find the internal interface number
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
e_Err ipMapArpExtenGetIntIfNum(word index, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Checks if the given interface is of type unnumbered.
*
* @param    index     {@b(Input)}   ARP interface index number
*
* @returns  TRUE if interface is unnumbered
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
Bool ipMapArpExtenIsUnnumbIntf(word index);

/*********************************************************************
* @purpose  Checks if proxy ARP is enabled on an interface.
*
* @param    index     {@b(Input)}   ARP interface index number
*
* @returns  TRUE if proxy ARP is enabled on the interface
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpExtenProxyArpEnabled(word index);

/*********************************************************************
* @purpose  Checks if the given interface is of type un-numbered.
*
* @param    index     {@b(Input)}   ARP interface index number
*
* @returns  TRUE if local proxy ARP is enabled on the interface
*
* @notes    This function is intended to be called by the vendor ARP code.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpExtenLocalProxyArpEnabled(word index);

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the specified IP address
*
* @param    ipAddr      IP address of the ARP entry
* @param    intIfNum    Internal Interface number for the entry
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
L7_RC_t ipMapArpExtenEntryGet(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum, 
                              L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Returns contents of the ARP entry for the next sequential
*           IP address following the one specified
*
* @param    ipAddr      IP address of the ARP entry to start the search
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
L7_RC_t ipMapArpExtenEntryNext(L7_IP_ADDR_t ipAddr, L7_uint32 intIfNum, 
                               L7_arpEntry_t *pArp);

/*********************************************************************
* @purpose  Sets the ARP entry ageout time to a new value
*
* @param    secs        new ARP entry ageout time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenAgeTimeSet(L7_uint32 secs);

/*********************************************************************
* @purpose  Sets the ARP resolution response timeout value
*
* @param    secs        new ARP response timeout (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenRespTimeSet(L7_uint32 secs);

/*********************************************************************
* @purpose  Sets the number of retries for failed ARP resolution requests
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
L7_RC_t ipMapArpExtenRetriesSet(L7_uint32 numRetry);

/*********************************************************************
* @purpose  Sets the ARP cache size
*
* @param    maxEnt      maximum number of entries allowed in ARP cache
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If request ARP cache size is outside of range then return
*           an error.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenCacheSizeSet(L7_uint32 maxEnt);

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
L7_RC_t ipMapArpExtenDynamicRenewSet(L7_uint32 mode);

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     pointer to output location for storing ARP cache stats
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipMapArpExtenCacheStatsGet(L7_arpCacheStats_t *pStats);

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
void ipMapArpExtenAddrConflictStatsShow(void);

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
void ipMapArpExtenAddrConflictStatsReset();

#endif /* _IPMAP_ARP_EXTEN_H_ */

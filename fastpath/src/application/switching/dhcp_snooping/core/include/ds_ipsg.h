
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_ipsg.h
*
* @purpose   IP Source Guard 
*
* @component DHCP snooping
*
* @comments  The DHCP snooping component hosts two applications:  DHCP 
*            snooping and IP Source Guard. IPSG uses the DHCP snooping 
*            bindings database to filter incoming traffic based on 
*            source IP address and optionally source MAC address.
*
* @create 3/27/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/

#ifdef L7_IPSG_PACKAGE

#include "l7_common.h"


/* Structure to represent a IPSG entry key in each tree node. */

typedef struct ipsgEntryKey_s
{
  /* physical port where client is attached. */
  L7_uint32 intIfNum;

  /* VLAN station is in. */
  L7_ushort16 vlanId;

  /* IP address assigned to the station */
#if 1 /* PTin modified: IPv6 Support */
  L7_inet_addr_t ipAddr;
#else
  L7_uint32 ipAddr;
#endif
  /* A MAC address uniquely identifies a node in the bindings tree. */
  L7_enetMacAddr_t macAddr;

}ipsgEntryKey_t;

/* Structure to represent a IPSG entry in each tree node.
 * Key is the interface, vland id , ip address and Mac addres . */
typedef struct ipsgEntryTreeNode_s
{

  ipsgEntryKey_t ipsgEntryKey;

  /* Time when binding learned (seconds since boot) */
  /* Will be useful to find the next free entry based on FCFS */
  L7_uint32 leaseStart;

  L7_BOOL ipsgEntryHwStatus;

  ipsgEntryType_t ipsgEntryType;

  void *next;                /* AVL next */

} ipsgEntryTreeNode_t;


L7_RC_t ipsgIntfEnableApply(L7_uint32 intIfNum, L7_BOOL addingMacFilter);
L7_RC_t ipsgIntfDisableApply(L7_uint32 intIfNum);
L7_BOOL ipsgIsEnabled(L7_uint32 intIfNum);
L7_BOOL ipsgPsIsEnabled(L7_uint32 intIfNum);
L7_RC_t ipsgVerifySourceApply(L7_uint32 intIfNum, L7_BOOL addingMacFilter);
L7_RC_t ipsgBindingHwAdd(L7_uint32 intIfNum,L7_inet_addr_t* ipAddr,L7_enetMacAddr_t *macAddr);
L7_RC_t ipsgBindingHwRemove(L7_uint32 intIfNum, L7_inet_addr_t* ipAddr);

L7_RC_t ipsgEntryTableCreate(void);
L7_RC_t ipsgEntryTableDelete(void);

L7_RC_t ipsgEntryTreeSearch(L7_uint32 intIfNum, L7_uint32 vlanId,
                                   L7_enetMacAddr_t *macAddr,L7_inet_addr_t* ipAddr,
                                   L7_uint32 matchType,ipsgEntryTreeNode_t **ipsgEntry);
L7_RC_t ipv4sgBindingNthEntryGet (ipsgBinding_t *ipsgBinding,
                               L7_uint32 n,
                               ipsgEntryType_t type);
L7_RC_t ipsgEntryAdd(ipsgEntryType_t entryType,
                     L7_uint32 intIfNum,
                     L7_ushort16 vlanId,
                     L7_enetMacAddr_t *macAddr,
                        L7_inet_addr_t *ipAddr);
L7_uint32 _ipsgEntriesCount(void);
L7_uint32 _ipsgStaticEntriesCount(void);
L7_RC_t ipsgFindHwFreeEntry(ipsgEntryTreeNode_t **ipsgEntry);
L7_RC_t ipsgEntryRemove (ipsgEntryType_t entryType,
                     L7_uint32 intIfNum,
                     L7_ushort16 vlanId,
                     L7_enetMacAddr_t *macAddr,
                     L7_inet_addr_t* ipAddr);
void ipsgEntryTableShow(void);
L7_RC_t _ipsgEntryClear(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Add a static entry to the IPSG entries database.
*
* @param    intIfNum @b((input))  internal interface number.
* @param    vlanId   @b((input))  client VLAN ID.
* @param    ipv4Addr @b((input))  client IP address.
* @param    macAddr  @b((input))  client MAC address.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes    Only supports IPv4
*
* @end
*********************************************************************/
L7_RC_t ipv4sgStaticEntryAdd(L7_uint32 intIfNum,
                           L7_ushort16 vlanId,
                           L7_enetMacAddr_t *macAddr,
                           L7_uint32 ipv4Addr);

/*********************************************************************
* @purpose  Remove a static entry to the IPSG entries database.
*
* @param    intIfNum @b((input))  internal interface number.
* @param    vlanId   @b((input))  client VLAN ID.
* @param    ipAddr   @b((input))  client IP address.
* @param    macAddr  @b((input))  client MAC address.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes    Only supports IPv4
*
* @end
*********************************************************************/
L7_RC_t ipv4sgStaticEntryRemove(L7_uint32 intIfNum,
                           L7_ushort16 vlanId,
                           L7_enetMacAddr_t *macAddr,
                           L7_uint32 ipv4Addr);

#endif

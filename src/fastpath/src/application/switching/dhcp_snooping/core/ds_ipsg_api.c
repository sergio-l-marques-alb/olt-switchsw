/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_api.c
*
* @purpose   IP Source Guard APIs
*
* @component DHCP snooping
*
* @comments  
*
* @create 3/27/2007
*
* @author Rob Rice (rrice)
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#ifdef L7_IPSG_PACKAGE
#include "osapi.h"
#include "log.h"

#include "ds_cfg.h"
#include "ds_util.h"
#include "ds_ipsg.h"

extern dsCfgData_t *dsCfgData;
extern osapiRWLock_t dsCfgRWLock;

static L7_uchar8 nullMacAddr[L7_ENET_MAC_ADDR_LEN] = {0,0,0,0,0,0};


/*********************************************************************
* @purpose  Get the number of  IPSG bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 ipsgBindingsCount (void)
{
   L7_uint32 count;
   if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;

   count = _ipsgEntriesCount();
   osapiReadLockGive(dsCfgRWLock);
   return count;
   
}

/*********************************************************************
* @purpose  Get the number of  IPSG static bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 ipsgStaticBindingsCount (void)
{
   L7_uint32 count;
   if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;

   count = _ipsgStaticEntriesCount();
   osapiReadLockGive(dsCfgRWLock);
   return count;

}



/*********************************************************************
* @purpose  Get the IPSG configuration for an interface.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    verifyIp   @b((output)) whether IPSG verifies the source 
*                                   IP address on this interface
* @param    verifyMac  @b((output)) whether IPSG verifies the source
*                                   MAC address on this interface
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipsgVerifySourceGet(L7_uint32 intIfNum, 
                            L7_BOOL *verifyIp,
                            L7_BOOL *verifyMac)
{
  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *verifyIp = ipsgIsEnabled(intIfNum);
  *verifyMac = ipsgPsIsEnabled(intIfNum);

  osapiReadLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the IPSG configuration for an interface.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    verifyIp   @b((input)) whether IPSG verifies the source 
*                                   IP address on this interface
* @param    verifyMac  @b((input)) whether IPSG verifies the source
*                                   MAC address on this interface
*
* @returns  L7_SUCCESS
*           L7_NOT_SUPPORTED, if IPSG is not supported on this 
*                             type of interface
*           L7_DEPENDENCY_NOT_MET, if attempting to enable IPSG port security
*                                  without enabling IPSG
*           L7_REQUEST_DENIED, if attempting to disable IPSG port security
*                              while leaving IPSG enabled
*           L7_FAILURE, other failures
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipsgVerifySourceSet(L7_uint32 intIfNum, 
                            L7_BOOL verifyIp,
                            L7_BOOL verifyMac)
{
  L7_BOOL addingMacFilter = L7_FALSE;

  if (!dsIntfIsValid(intIfNum))
  {
    return L7_NOT_SUPPORTED;
  }

  /* Can't enable IPSG PS w/o IPSG */
  if (!verifyIp && verifyMac)
    return L7_DEPENDENCY_NOT_MET;

  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Detect and ignore no change */
  if ((ipsgIsEnabled(intIfNum) == verifyIp) &&
      (ipsgPsIsEnabled(intIfNum) == verifyMac))
  {
    osapiWriteLockGive(dsCfgRWLock);
    return L7_SUCCESS;
  }

  /* We don't allow user to turn off IPSG PS while leaving IPSG on. */
  if (ipsgIsEnabled(intIfNum) && verifyIp && !verifyMac)
  {
    osapiWriteLockGive(dsCfgRWLock);
    return L7_REQUEST_DENIED;
  }

  /* If IPSG was previously enabled, and user has now asked to enable 
   * MAC filtering, we'll need to flush all HW classifier entries 
   * from this interface before adding them back with non-zero MACs. */
  if (ipsgIsEnabled(intIfNum) && verifyIp && verifyMac)
  {
    addingMacFilter = L7_TRUE;
  }

  dsCfgData->ipsgIntfCfg[intIfNum].verifyIp = verifyIp;
  dsCfgData->ipsgIntfCfg[intIfNum].verifyMac = verifyMac;

  ipsgVerifySourceApply(intIfNum, addingMacFilter);
  dsConfigDataChange();

  osapiWriteLockGive(dsCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next source binding for IP source guard.
*
* @param    intIfNum   @b((input/output)) internal interface number
* @param    vlanId     @b((input/output)) VLAN ID
* @param    ipAddr     @b((input/output)) authorized source IP address
* @param    macAddr    @b((input/output)) authorized source MAC address
* @param    entryType  @b((input/output)) Entry type of the row
*
* @returns  L7_SUCCESS
*           L7_FAILURE when no more entries
*
* @notes    Entry type will be populated. Entry type from the caller 
*           function will not be used while getting the next entry.
*
* @end
*********************************************************************/
L7_RC_t ipsgBindingGetNext(L7_uint32 *intIfNum, L7_ushort16 *vlanId, 
                           L7_inet_addr_t *ipAddr, L7_enetMacAddr_t *macAddr,
                           L7_uint32 *entryType,
                           L7_uint32 matchType)
{
  L7_RC_t rc = L7_FAILURE;
  ipsgEntryTreeNode_t *ipsgEntry;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  /* Get next binding from DHCP snooping bindings table. */
  
  while ( ipsgEntryTreeSearch (*intIfNum,
                               *vlanId,
                               macAddr,
                               ipAddr,
                               matchType,                             
                               &ipsgEntry) == L7_SUCCESS)

  {
    {
      /* IPSG is enabled on interface associated with binding. */
      *intIfNum = ipsgEntry->ipsgEntryKey.intIfNum;
      *vlanId = ipsgEntry->ipsgEntryKey.vlanId;      
      memcpy(ipAddr, &ipsgEntry->ipsgEntryKey.ipAddr, sizeof(L7_inet_addr_t));
      memcpy( macAddr->addr, 
              ipsgEntry->ipsgEntryKey.macAddr.addr,
              L7_ENET_MAC_ADDR_LEN);
      *entryType = ipsgEntry->ipsgEntryType;
      rc = L7_SUCCESS;
      break; 
    }
  }
  osapiReadLockGive(dsCfgRWLock);
  return rc;
}


/*********************************************************************
* @purpose  Query IPSG to find out if a source MAC address is valid. 
*
* @param    macAddr    @b((input)) authorized source MAC address
* @param    vlanId     @b((input)) VLAN ID
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_TRUE if source MAC is valid on given VLAN/port
*           L7_FALSE otherwise
*
* @notes    If IPSG port-security is not enabled on the given interface,
*           consider all clients authorized. 
*
*           Binding must match MAC, vlan ID, and port.
*
* @end
*********************************************************************/
L7_BOOL ipsgClientAuthorized(L7_enetMacAddr_t *macAddr,
                             L7_ushort16 vlanId, L7_uint32 intIfNum)
{
  L7_BOOL auth = L7_FALSE;
  L7_uint32 entryType;
  L7_inet_addr_t ipAddr;
  L7_enetMacAddr_t tmpMacAddr;
  L7_ushort16 tmpVlanId;
  L7_uint32 tmpIntIfNum;
  
  memset(&ipAddr, 0x00, sizeof(ipAddr));

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FALSE;

  if (!ipsgPsIsEnabled(intIfNum))
  {
    osapiReadLockGive(dsCfgRWLock);
    return L7_TRUE;
  }
  memcpy (tmpMacAddr.addr, macAddr->addr,L7_ENET_MAC_ADDR_LEN);
  tmpVlanId = vlanId;
  tmpIntIfNum = intIfNum;
 
  while ( ipsgBindingGetNext(&intIfNum,
                          &vlanId,
                          &ipAddr,
                          macAddr,
                          &entryType,
                          L7_MATCH_GETNEXT) == L7_SUCCESS)
  {
      if ( (memcmp(tmpMacAddr.addr, macAddr->addr,L7_ENET_MAC_ADDR_LEN) == 0)
            && (tmpVlanId == vlanId) && (tmpIntIfNum == intIfNum))
       {  
         auth = L7_TRUE;
         break;
       }
      else
      {
        continue;
      }
  }

  osapiReadLockGive(dsCfgRWLock);
  return auth;
}

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
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgStaticEntryAdd(L7_uint32 intIfNum,
                           L7_ushort16 vlanId,
                           L7_enetMacAddr_t *macAddr,
                           L7_uint32 ipv4Addr)
{
  L7_RC_t        rc;
  L7_inet_addr_t ipAddr;

  /* Validate IP address. Don't allow 0, mcast or above, loopback. */
  if ((ipv4Addr == 0) ||
      (ipv4Addr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK) ||
      (((ipv4Addr & 0xff000000) >> 24) == 127))
  {
    return L7_ERROR;
  }

  /* Validate MAC address */
  if ((memcmp(macAddr, nullMacAddr, L7_ENET_MAC_ADDR_LEN) == 0) ||
      ((macAddr->addr[0] & 1) == 1))
  {
    /* Don't allow the following invalid MAC addresses:
     * (a) Address 00:00:00:00:00:00
     * (b) Any multicast address (the first bit in the first byte being 1)
     *     It includes the broadcast address FF:FF:FF:FF:FF:FF
     */
    return L7_ERROR;
  }

  /* Validate VLAN ID */
  if ((vlanId == 0) || (vlanId > L7_PLATFORM_MAX_VLAN_ID))
  {
    return L7_ERROR;
  }
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  inetAddressSet(L7_AF_INET, &ipv4Addr, &ipAddr);

  rc = ipsgEntryAdd(IPSG_ENTRY_STATIC,intIfNum,vlanId, macAddr, &ipAddr);

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Remove a static entry to the IPSG entries database.
*
* @param    intIfNum @b((input))  internal interface number.
* @param    vlanId   @b((input))  client VLAN ID.
* @param    ipv4Addr @b((input))  client IP address.
* @param    macAddr  @b((input))  client MAC address.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgStaticEntryRemove(L7_uint32 intIfNum,
                           L7_ushort16 vlanId,
                           L7_enetMacAddr_t *macAddr,
                           L7_uint32 ipv4Addr)
{
  L7_RC_t        rc;
  L7_inet_addr_t ipAddr;

  /* Validate IP address. Don't allow 0, mcast or above, loopback. */
  if ((ipv4Addr == 0) ||
      (ipv4Addr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK) ||
      (((ipv4Addr & 0xff000000) >> 24) == 127))
  {
    return L7_ERROR;
  }

  /* Validate MAC address */
  if ((memcmp(macAddr, nullMacAddr, L7_ENET_MAC_ADDR_LEN) == 0) ||
      ((macAddr->addr[0] & 1) == 1))
  {
    /* Don't allow the following invalid MAC addresses:
     * (a) Address 00:00:00:00:00:00
     * (b) Any multicast address (the first bit in the first byte being 1)
     *     It includes the broadcast address FF:FF:FF:FF:FF:FF
     */
    return L7_ERROR;
  }

  /* Validate VLAN ID */
  if ((vlanId == 0) || (vlanId > L7_PLATFORM_MAX_VLAN_ID))
  {
    return L7_ERROR;
  }
  if (osapiWriteLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  inetAddressSet(L7_AF_INET, &ipv4Addr, &ipAddr);

  rc = ipsgEntryRemove(IPSG_ENTRY_STATIC,intIfNum,vlanId, macAddr, &ipAddr);

  osapiWriteLockGive(dsCfgRWLock);
  return rc;
}

#endif

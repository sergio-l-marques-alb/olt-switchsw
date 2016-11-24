/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename iscsi_api.c
*
* @purpose ISCSI API
*
* @component ISCSI
*
* @comments none
*
* @create 04/18/2008
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "iscsi_api.h"
#include "osapi_support.h"

extern iscsiCfgData_t  *iscsiCfgData;
extern osapiRWLock_t    iscsiCfgRWLock;

static L7_int32 iscsiTargetPortConfigCompare(iscsiTargetCfgData_t *r1, iscsiTargetCfgData_t *r2);

/*********************************************************************
* @purpose  Configures the global administrative mode iSCSI component
*
* @param  L7_uint32 adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiAdminModeSet(L7_uint32 adminMode)
{
  /* Validate Input */
  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  if (adminMode != iscsiCfgData->adminMode)
  {
    if (iscsiAdminModeApply(adminMode) != L7_SUCCESS)
    {
       (void)osapiWriteLockGive(iscsiCfgRWLock);
       return L7_FAILURE;
    }	 
  
    iscsiCfgData->adminMode = adminMode;
    iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the global administative mode iSCSI component
*
* @param  L7_uint32 *adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiAdminModeGet(L7_uint32 *adminMode)
{
  if (adminMode == L7_NULLPTR)
    return L7_FAILURE;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  *adminMode = iscsiCfgData->adminMode;

  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures the VLAN priority for treatment of iSCSI packets
*
* @param  L7_uint32 priority      (input)  VLAN tag priority 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiVlanPrioritySet(L7_uint32 priority)
{
  /* check input */
  if (iscsiVlanPriorityRangeCheck(priority) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
  
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  /* if already configured as requested, return success */
  if (priority == iscsiCfgData->vlanPriorityValue)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_SUCCESS;
  } 
  
  if (iscsiVlanPriorityApply(priority) != L7_SUCCESS)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_FAILURE;
  }	 

  iscsiCfgData->vlanPriorityValue = priority;
  iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve the VLAN priority value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *priority      (output)  VLAN tag priority 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiVlanPriorityGet(L7_uint32 *priority)
{
  if (priority == L7_NULLPTR)
    return L7_ERROR;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  *priority = iscsiCfgData->vlanPriorityValue;
  
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures the DSCP value for treatment of iSCSI packets
*
* @param  L7_uint32 dscp      (input)  DSCP field data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiDscpSet(L7_uint32 dscp)
{
  /* check input */
  if (iscsiDscpRangeCheck(dscp) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
  
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  /* if already configured as requested, return success */
  if (dscp == iscsiCfgData->dscpValue)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_SUCCESS;
  } 
  
  if (iscsiDscpApply(dscp) != L7_SUCCESS)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_FAILURE;
  }	 

  iscsiCfgData->dscpValue = dscp;
  iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve the DSCP value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *dscp      (output)  DSCP value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiDscpGet(L7_uint32 *dscp)
{
  if (dscp == L7_NULLPTR)
    return L7_ERROR;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  *dscp = iscsiCfgData->dscpValue;
  
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures the IP precedence value for treatment of iSCSI packets
*
* @param  L7_uint32 prec      (input)  IP precedence data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiPrecSet(L7_uint32 prec)
{
  /* check input */
  if (iscsiIpPrecRangeCheck(prec) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
  
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  /* if already configured as requested, return success */
  if (prec == iscsiCfgData->precValue)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_SUCCESS;
  } 
  
  if (iscsiIpPrecApply(prec) != L7_SUCCESS)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_FAILURE;
  }	 

  iscsiCfgData->precValue = prec;
  iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve the PREC value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *prec      (output)  PREC value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiPrecGet(L7_uint32 *prec)
{
  if (prec == L7_NULLPTR)
    return L7_ERROR;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  *prec = iscsiCfgData->precValue;
  
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures whether iSCSI session packets will be marked
*
* @param  L7_uint32 adminMode      (input) tagging enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiMarkingModeSet(L7_uint32 adminMode)
{
  /* Validate Input */
  if (adminMode != L7_ENABLE && adminMode != L7_DISABLE)
  {
    return L7_FAILURE;
  }
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  if (((L7_ENABLE == adminMode) && (L7_FALSE == iscsiCfgData->markingEnabled)) ||
      ((L7_DISABLE == adminMode) && (L7_TRUE == iscsiCfgData->markingEnabled)))
  {
    if (iscsiMarkingModeApply(adminMode) != L7_SUCCESS)
    {
       (void)osapiWriteLockGive(iscsiCfgRWLock);
       return L7_FAILURE;
    }	 
  
    iscsiCfgData->markingEnabled = (adminMode == L7_ENABLE) ? L7_TRUE : L7_FALSE;
    iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieves the marking mode iSCSI component
*
* @param  L7_uint32 *adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiMarkingModeGet(L7_uint32 *adminMode)
{
  if (adminMode == L7_NULLPTR)
    return L7_FAILURE;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  *adminMode = iscsiCfgData->markingEnabled ? L7_ENABLE : L7_DISABLE;

  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures which type of packet priority marking to use
*           for iSCSI session traffic
*
* @param  L7_QOS_COS_MAP_INTF_MODE_t selector      (input)  what type
*                    of marking to apply to session traffic
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTagFieldSet(L7_QOS_COS_MAP_INTF_MODE_t selector)
{
  /* check input */
  if (iscsiTagFieldRangeCheck(selector) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
  
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  /* if already configured as requested, return success */
  if (selector == iscsiCfgData->tagFieldSelector)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_SUCCESS;
  } 
  
  if (iscsiTagFieldApply(selector) != L7_SUCCESS)
  {
     (void)osapiWriteLockGive(iscsiCfgRWLock);
     return L7_FAILURE;
  }	 

  iscsiCfgData->tagFieldSelector = selector;
  iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
  
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Retrieve the PREC value used for iSCSI session
*           data packets
*
* @param  L7_QOS_COS_MAP_INTF_MODE_t *selector  (output)  what type
*                    of marking to apply to session traffic
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTagFieldGet(L7_QOS_COS_MAP_INTF_MODE_t *selector)
{
  if (selector == L7_NULLPTR)
    return L7_ERROR;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  *selector = iscsiCfgData->tagFieldSelector;
  
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures the session time out value used for iSCSI sessions
*
* @param  L7_uint32 seconds   (input) number of seconds to wait before
*                             deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTimeOutIntervalSet(L7_uint32 seconds)
{
  L7_RC_t rc;

  /* check input */
  if (iscsiTimeOutIntervalRangeCheck(seconds) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
  
  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  /* if already configured as requested, return success */
  if (seconds == iscsiCfgData->sessionTimeOutInterval)
  {
    rc = L7_SUCCESS;
  } 
  else
  {
    if ((rc = iscsiTimeOutIntervalApply(seconds)) == L7_SUCCESS)
    {
      iscsiCfgData->sessionTimeOutInterval = seconds;
      iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieve the session time out value used for iSCSI sessions
*
* @param  L7_uint32 *seconds   (output) number of seconds to wait before
*                               deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTimeOutIntervalGet(L7_uint32 *seconds)
{
  if (seconds == L7_NULLPTR)
    return L7_ERROR;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
   
  *seconds = iscsiCfgData->sessionTimeOutInterval;
  
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configures the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_uint32 ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 targetName (input) target's name 
*
* @returns  L7_SUCCESS 
*           L7_FAILURE
*           L7_ALREADY_CONFIGURED
*
* @notes    TCP ports can be specified without an IP address.  If so,
*           all TCP frames will sent to this TCP port will be examined for 
*           iSCSI protocol frames.  If the ipAddress is specified, only 
*           packets sent to the TCP port and this IP address will be 
*           examined.
*
*           The targetName is optional and only used for show commands as
*           a convenience to the user.  It is not used in the filtering of
*           which frames to examine.
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTcpPortAdd (L7_uint32 tcpPort, L7_inet_addr_t *ipAddress, L7_uchar8 *targetName)
{
  L7_RC_t rc;
  L7_uint32 entryId;
  L7_uint32 ip4Address;

  /* check inputs */
  if ((ipAddress == L7_NULLPTR) ||
      (L7_INET_GET_FAMILY(ipAddress) != L7_AF_INET))
  {
    return L7_ERROR;
  }

  ip4Address = osapiNtohl(ipAddress->addr.ipv4.s_addr);

  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  {
    L7_uint32 cosQueue, tagMode, tagData;

    /* single-pass loop for error checking*/
    do
    {
      /* see if we already have entry for this TCP port/IP address data */
      if ((rc = iscsiImpTargetTableLookUp(tcpPort, ip4Address, &entryId)) != L7_SUCCESS)
      {
        /* not found, need to add to table */
        if ((rc = iscsiImpTargetTcpPortFreeEntryGet(&entryId)) != L7_SUCCESS)
        {
          /* was unable to locate an entry for this data */
          rc = L7_TABLE_IS_FULL;
          break;
        }
        else
        {
          /* allocated new table entry update driver and config */

          /* only add to hardware if admin mode is enable, else just store in configuration */
      if (iscsiCfgData->adminMode == L7_ENABLE)
      {
        /* retrieve CoS queue assignment for configured tag type */
        if (iscsiImpCosDataGet(&cosQueue, &tagMode, &tagData) != L7_SUCCESS)
        {
          rc = L7_FAILURE;
          break;           /* error encountered, no further processing */
        }
        /* configure operational state to start monitoring this port */
        if (iscsiImpTargetTcpPortAdd(tcpPort, ip4Address, cosQueue, iscsiCfgData->markingEnabled, tagMode, tagData) != L7_SUCCESS)
        {
          rc = L7_FAILURE;
          break;           /* error encountered, no further processing */
        }
      }
      /* if we get here, we can update the configuration */
      iscsiCfgData->iscsiTargetCfgData[entryId].portNumber = tcpPort;
      iscsiCfgData->iscsiTargetCfgData[entryId].ipAddress = ip4Address;

          iscsiCfgData->iscsiTargetCfgData[entryId].inUse = L7_TRUE;
          iscsiCfgData->iscsiTargetCfgData[entryId].deletedDefaultCfgEntry = L7_FALSE;
          iscsiCfgData->numActiveTargetEntries++;
          iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
        }
      }

      /* if a target name string specified, copy it into the configuration */
      if (targetName != L7_NULLPTR)
        (void)osapiStrncpySafe(iscsiCfgData->iscsiTargetCfgData[entryId].targetName, targetName, sizeof(iscsiCfgData->iscsiTargetCfgData[entryId].targetName));
      else
        memset(iscsiCfgData->iscsiTargetCfgData[entryId].targetName, 0, sizeof(iscsiCfgData->iscsiTargetCfgData[entryId].targetName));

      rc = L7_SUCCESS;
    } while (0);
  }

  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Removes the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_inet_addr_t *ipAddress  (input) IP address to be monitored on this port
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTcpPortDelete(L7_uint32 tcpPort, L7_inet_addr_t *ipAddress)
{
  L7_RC_t rc;
  L7_uint32 entryId;
  L7_uint32 ip4Address;

  /* check inputs */
  if ((ipAddress == L7_NULLPTR) ||
      (L7_INET_GET_FAMILY(ipAddress) != L7_AF_INET))
  {
    return L7_ERROR;
  }

  ip4Address = osapiNtohl(ipAddress->addr.ipv4.s_addr);

  (void)osapiWriteLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  if (iscsiImpTargetTableLookUp(tcpPort, ip4Address, &entryId) == L7_SUCCESS)
  {
    /* found an entry with matching port number and ip address */
    if (iscsiImpTargetTcpPortDelete(entryId) == L7_SUCCESS)
    {
      if (iscsiCfgData->iscsiTargetCfgData[entryId].defaultCfgEntry == L7_TRUE)
      {
        iscsiCfgData->iscsiTargetCfgData[entryId].deletedDefaultCfgEntry = L7_TRUE;
      }
      else
      {
      memset(&iscsiCfgData->iscsiTargetCfgData[entryId], 0, sizeof(iscsiCfgData->iscsiTargetCfgData[entryId]));
      iscsiCfgData->iscsiTargetCfgData[entryId].inUse = L7_FALSE;
      }
      iscsiCfgData->numActiveTargetEntries--;
      iscsiCfgData->cfgHdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
    }
    else
    {
      rc =  L7_FAILURE;
    }
  }
  else
    /* entry not found */
    rc = L7_ERROR;

  (void)osapiWriteLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Finds the entry in the Target TCP port table the matches
*           the specified port number and IP address.
*
* @param  L7_uint32 port  (input) TCP port number 
* @param  L7_uint32 ip_addr  (input) IP address, this parameter is
*                              ignored if set to 0
* @param  L7_uint32 *entryId  (output) the entry identifier, if found
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableLookUp(L7_uint32 port, L7_uint32 ip_addr, L7_uint32 *entryId)
{
  L7_RC_t rc;

  /* check inputs */
  if (entryId == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  rc = iscsiImpTargetTableLookUp(port, ip_addr, entryId);
  (void)osapiReadLockGive(iscsiCfgRWLock);

  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the first entry in the Target TCP port table.
*           Entries are returned in a sorted order by TCP port number,
*           target IP address, and target name.
*
* @param  L7_uint32 *entryId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableFirstGet(L7_uint32 *entryId, L7_BOOL includeDeletedDfltCfgEntries)
{
  L7_uint32                 i;
  L7_int32                  bestMatchIndex;

  if (entryId == L7_NULLPTR)
    return L7_ERROR;

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);

  bestMatchIndex = -1;

  /* scan for first candidate record */
  for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
  {
    if (ISCSI_CFG_TARGET_ENTRY_ACTIVE(i, includeDeletedDfltCfgEntries) == L7_TRUE)
    {
      bestMatchIndex = i;
      break;
    }
  }
  /* if we found the candidate occupied record, scan the rest for one of lesser value */
  if (bestMatchIndex != -1)
  {
    for (; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
    {
      if (ISCSI_CFG_TARGET_ENTRY_ACTIVE(i, includeDeletedDfltCfgEntries) == L7_TRUE)
      {
        if (iscsiTargetPortConfigCompare(&iscsiCfgData->iscsiTargetCfgData[i], &iscsiCfgData->iscsiTargetCfgData[bestMatchIndex]) < 0)
        {
          bestMatchIndex = i;
        }
      }
    }
  }

  (void)osapiReadLockGive(iscsiCfgRWLock);
  if (bestMatchIndex != -1)
  {
    *entryId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Retrieves the next entry in the Target TCP port table.
*           Entries are returned in a sorted order by TCP port number,
*           target IP address, and target name.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextEntryId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableNextGet(L7_uint32 entryId, L7_uint32 *nextEntryId, L7_BOOL includeDeletedDfltCfgEntries)
{
  L7_uint32                 i;
  L7_int32                  bestMatchIndex;

  /* check inputs */
  if ((entryId >= L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT) || (nextEntryId == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  bestMatchIndex = -1;

  if (ISCSI_CFG_TARGET_ENTRY_ACTIVE(entryId, includeDeletedDfltCfgEntries) == L7_TRUE)
  {
    for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
    {
      if ((i != entryId) && 
          (ISCSI_CFG_TARGET_ENTRY_ACTIVE(i, includeDeletedDfltCfgEntries) == L7_TRUE))
      {
        if (iscsiTargetPortConfigCompare(&iscsiCfgData->iscsiTargetCfgData[i], &iscsiCfgData->iscsiTargetCfgData[entryId]) > 0)
        {
          if (bestMatchIndex == -1)
          {
            bestMatchIndex = i;
          }
          else
          {
            if (iscsiTargetPortConfigCompare(&iscsiCfgData->iscsiTargetCfgData[i], &iscsiCfgData->iscsiTargetCfgData[bestMatchIndex]) < 0)
            {
              bestMatchIndex = i;
            }
          }
        }
      }
    }
  }

  (void)osapiReadLockGive(iscsiCfgRWLock);
  if (bestMatchIndex != -1)
  {
    *nextEntryId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns the entry in the Target TCP port table that contains
*           the specified TCP port number and target IP address if it 
*           exists.
*
* @param  L7_uint32 tcpPortNumber    (input) TCP port to be monitored  
* @param  L7_inet_addr_t ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 *entryId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  not found
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryGet(L7_uint32 tcpPortNumber, L7_inet_addr_t *ipAddress, L7_uint32 *entryId)
{
  L7_uint32 ip4Address;

  /* check inputs */
  if ((ipAddress == L7_NULLPTR) || (ipAddress->family != L7_AF_INET))
  {
    return L7_ERROR;
  }

  ip4Address = osapiNtohl(ipAddress->addr.ipv4.s_addr);
  return(iscsiTargetTableLookUp(tcpPortNumber, ip4Address, entryId));
}

/*********************************************************************
* @purpose  Returns the next entry in the Target TCP port table after
*           the specified TCP port number and target IP address if it 
*           exists.
*
* @param  L7_uint32 tcpPortNumber    (input) TCP port to be monitored  
* @param  L7_inet_addr_t ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 *nextEntryId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  not found
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryNextGet(L7_uint32 tcpPortNumber, L7_inet_addr_t *ipAddress, L7_uint32 *nextEntryId)
{
  L7_uint32 i;
  L7_int32  bestMatchIndex;
  iscsiTargetCfgData_t currentData;

  /* check inputs */
  if ((ipAddress == L7_NULLPTR) || (ipAddress->family > L7_AF_INET))
  {
    return L7_ERROR;
  }

  memset(&currentData, 0, sizeof(currentData));
  currentData.portNumber = tcpPortNumber;
  currentData.ipAddress =  osapiNtohl(ipAddress->addr.ipv4.s_addr);

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  bestMatchIndex = -1;

  /* scan for first candidate record */
  for (i=0; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
  {
    if (ISCSI_CFG_TARGET_ENTRY_ACTIVE(i, L7_FALSE) == L7_TRUE)
    {
      /* candidate must be greater than the data passed in to be considered */
      if (iscsiTargetPortConfigCompare(&iscsiCfgData->iscsiTargetCfgData[i], &currentData) > 0)
      {
        bestMatchIndex = i;
        break;
      }
    }
  }
  /* if we found the candidate occupied record, scan the rest for one of lesser value that is still greater than data passed in */
  if (bestMatchIndex != -1)
  {
    i++;
    for (; i<L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT; i++)
    {
      if (ISCSI_CFG_TARGET_ENTRY_ACTIVE(i, L7_FALSE) == L7_TRUE)
      {
        if ((iscsiTargetPortConfigCompare(&iscsiCfgData->iscsiTargetCfgData[i], &iscsiCfgData->iscsiTargetCfgData[bestMatchIndex]) < 0) &&
            (iscsiTargetPortConfigCompare(&iscsiCfgData->iscsiTargetCfgData[i], &currentData) > 0))
        {
          bestMatchIndex = i;
        }
      }
    }
  }

  (void)osapiReadLockGive(iscsiCfgRWLock);
  if (bestMatchIndex != -1)
  {
    *nextEntryId = bestMatchIndex;
    return L7_SUCCESS;
  }
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Returns whether the Target Table entry was configured
*           as one of the Factory Default table entries.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
*
* @returns  L7_TRUE    table entry in the default config table 
* @returns  L7_FALSE   no match
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiTargetTableIsEntryInDefaultConfig(L7_uint32 entryId)
{
  L7_BOOL result = L7_FALSE;

  if (entryId < L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT)
  {
    (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
    result = iscsiCfgData->iscsiTargetCfgData[entryId].defaultCfgEntry;
    (void)osapiReadLockGive(iscsiCfgRWLock);
  }

  return(result);
}

/*********************************************************************
* @purpose  Returns the value of the deletedDefaultCfgEntry flag for the
*           Target Table entry.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
*
* @returns  L7_TRUE    table entry in the default config table 
* @returns  L7_FALSE   no match
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiTargetTableEntryDeletedDefaultConfigFlagGet(L7_uint32 entryId)
{
  L7_BOOL result = L7_FALSE;

  if (entryId < L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT)
  {
    (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
    result = iscsiCfgData->iscsiTargetCfgData[entryId].deletedDefaultCfgEntry;
    (void)osapiReadLockGive(iscsiCfgRWLock);
  }

  return(result);
}

/*********************************************************************
* @purpose  Retrieves the TCP port number for the specified
*           entry in the Target TCP port table.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *tcpPortNumber  (output) TCP port number value of the table entry.  
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryTcpPortGet(L7_uint32 entryId, L7_uint32 *tcpPortNumber)
{
  L7_RC_t rc = L7_FAILURE;

  if ((entryId >= L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT) || (tcpPortNumber == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  if (iscsiCfgData->iscsiTargetCfgData[entryId].inUse == L7_TRUE)
  {
    *tcpPortNumber = iscsiCfgData->iscsiTargetCfgData[entryId].portNumber;
    rc = L7_SUCCESS;
  }
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Reports whether the  Target TCP port table entry corresponding with
*           entryId is in use.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
*
* @returns  L7_TRUE     table entry is in use 
* @returns  L7_FALSE    table entry empty or entryId out of range
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiTargetTableEntryInUseStatusGet(L7_uint32 entryId)
{
  L7_BOOL result = L7_FALSE;

  if (entryId >= L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT)
  {
    return L7_FALSE;
  }

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  result = iscsiCfgData->iscsiTargetCfgData[entryId].inUse;
  (void)osapiReadLockGive(iscsiCfgRWLock);

  return(result);
}

/*********************************************************************
* @purpose  Retrieves the target IP address for the specified
*           entry in the Target TCP port table.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *tcpPortNumber  (output) TCP port number value of the table entry.  
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryIpAddressGet(L7_uint32 entryId, L7_inet_addr_t *ipAddress)
{
  L7_RC_t rc = L7_FAILURE;

  if ((entryId >= L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT) || 
      (ipAddress == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  if (iscsiCfgData->iscsiTargetCfgData[entryId].inUse == L7_TRUE)
  {
    ipAddress->family = L7_AF_INET;
    ipAddress->addr.ipv4.s_addr = osapiHtonl(iscsiCfgData->iscsiTargetCfgData[entryId].ipAddress);
    rc = L7_SUCCESS;
  }
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the target name for the specified
*           entry in the Target TCP port table.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *tcpPortNumber  (output) TCP port number value of the table entry.  
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryTargetNameGet(L7_uint32 entryId, L7_uchar8 *name)
{
  L7_RC_t rc = L7_FAILURE;

  if ((entryId >= L7_ISCSI_CFG_TARGET_TABLE_ENTRY_COUNT) || 
      (name == L7_NULLPTR))
  {
    return L7_ERROR;
  }

  (void)osapiReadLockTake(iscsiCfgRWLock, L7_WAIT_FOREVER);
  if (iscsiCfgData->iscsiTargetCfgData[entryId].inUse == L7_TRUE)
  {
    osapiStrncpySafe(name, iscsiCfgData->iscsiTargetCfgData[entryId].targetName, sizeof(iscsiCfgData->iscsiTargetCfgData[entryId].targetName));
    rc = L7_SUCCESS;
  }
  (void)osapiReadLockGive(iscsiCfgRWLock);
  return(rc);
}

/*********************************************************************
* @purpose  Checks whether an entry associated with the given session
*           identifier is in use.
*
* @param  L7_uint32 sessionId    (input) an identifier of a
*                                table entry.  
*
* @returns  L7_SUCCESS  session ID is in use 
* @returns  L7_FAILURE  session ID is not in use
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionTableIdIsValid(L7_uint32 sessionId)
{
  L7_RC_t rc;

  rc = iscsiImpSessionTableIdIsValid(sessionId);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 *sessionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionFirstGet(L7_uint32 *sessionId)
{
  L7_RC_t rc;

  rc = iscsiImpSessionFirstGet(sessionId);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  L7_RC_t rc;

  rc = iscsiImpSessionNextGet(sessionId, nextSessionId);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in order of increasing sessionId.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionNextByEntryIdGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  return(iscsiImpSessionNextByEntryIdGet(sessionId, nextSessionId));
}

/*********************************************************************
* @purpose  Retrieves the target IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  L7_RC_t rc;

  rc = iscsiImpSessionTargetNameGet(sessionId, name);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the initiator IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  L7_RC_t rc;

  rc = iscsiImpSessionInitiatorNameGet(sessionId, name);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the start time for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_clocktime *time   (output) the system time when the session 
*                                was detected
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time)
{
  L7_RC_t rc;

  rc = iscsiImpSessionStartTimeGet(sessionId, time);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the time since last data traffic for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uint32 *seconds   (output) seconds since last seen data for session 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds)
{
  L7_RC_t rc;

  rc = iscsiImpSessionSilentTimeGet(sessionId, seconds);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the ISID for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *isid      (output) the ISID associated with the specified
*                                 session
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid)
{
  L7_RC_t rc;

  rc = iscsiImpSessionIsidGet(sessionId, isid);
  return(rc);
}

/*********************************************************************
* @purpose  Checks whether an entry associated with the given connection
*           identifier is in use.
*
* @param  L7_uint32 connectionId (input) an identifier of a
*                                table entry.  
*
* @returns  L7_SUCCESS  connection ID is in use 
* @returns  L7_FAILURE  connection ID is not in use
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionTableIdIsValid(L7_uint32 connectionId)
{
  L7_RC_t rc;

  rc = iscsiImpConnectionTableIdIsValid(connectionId);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 *connectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId)
{
  L7_RC_t rc;

  rc = iscsiImpConnectionFirstGet(sessionId, connectionId);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 connectionId  (input) an identifier of the current connection table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  L7_RC_t rc;

  rc = iscsiImpConnectionNextGet(sessionId, connectionId, nextConnectionId);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table.
*           Entries are returned in order of increasing connectionId.
*
* @param  L7_uint32 connectionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionNextByEntryIdGet(L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  return(iscsiImpConnectionNextByEntryIdGet(connectionId, nextConnectionId));
}

/*********************************************************************
* @purpose  Retrieves the target IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_inet_addr_t *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_inet_addr_t *ipAddr)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 ip4Address;

  if ((ipAddr != L7_NULLPTR) &&
      ((rc = iscsiImpConnectionTargetIpAddressGet(connectionId, &ip4Address)) == L7_SUCCESS))
  {
    ipAddr->family = L7_AF_INET;
    ipAddr->addr.ipv4.s_addr = osapiHtonl(ip4Address);
  }
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the initiator IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_inet_addr_t *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_inet_addr_t *ipAddr)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 ip4Address;

  if ((ipAddr != L7_NULLPTR) &&
      ((rc = iscsiImpConnectionInitiatorIpAddressGet(connectionId, &ip4Address)) == L7_SUCCESS))
  {
    ipAddr->family = L7_AF_INET;
    ipAddr->addr.ipv4.s_addr = osapiHtonl(ip4Address);
  }
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the target TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  L7_RC_t rc;

  rc = iscsiImpConnectionTargetTcpPortGet(connectionId, port);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the initiator TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  L7_RC_t rc;

  rc = iscsiImpConnectionInitiatorTcpPortGet(connectionId, port);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the iSCSI connection id (CID) for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *cid         (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid)
{
  L7_RC_t rc;

  rc = iscsiImpConnectionCidGet(connectionId, cid);
  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the iSCSI Session table entry id for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *sessionId    (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionSessionIdGet(L7_uint32 connectionId, L7_uint32 *sessionId)
{
  return(iscsiImpConnectionSessionIdGet(connectionId, sessionId));
}

/*********************************************************************
* @purpose  Compares two entries in the Target TCP port table to determine
*           relative ordering with respect to sorting criteria.
*
* @param  iscsiTargetCfgData_t *r1, iscsiTargetCfgData_t *r2  (input) pointers
*                   to two table entries to be compared
*
* @returns  -1   entry r1 is less than r2 
* @returns  0    entries are equal
* @returns  1    entry r1 is greater than r2 
*
* @notes    Sort criteria is: portNumber, ipAddress. The IQN field
*           is not considered.
*
* @end
*********************************************************************/
L7_int32 iscsiTargetPortConfigCompare(iscsiTargetCfgData_t *r1, iscsiTargetCfgData_t *r2)
{
  L7_int32 result;

  if (r1->portNumber < r2->portNumber)
  {
    result = -1;
  }
  else if (r1->portNumber > r2->portNumber)
  {
    result = 1;
  }
  else  /* port number is equal, look at IP addresses */
  {
    if (r1->ipAddress < r2->ipAddress)
    {
      result = -1;
    }
    else if (r1->ipAddress > r2->ipAddress)
    {
      result = 1;
    }
    else
    {
      result = 0;
    }
  }
  return(result);
}




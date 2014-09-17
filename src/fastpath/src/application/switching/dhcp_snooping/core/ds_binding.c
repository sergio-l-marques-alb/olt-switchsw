/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ds_binding.c
*
* @purpose     manage the DHCP snooping bindings database
*
* @component   DHCP snooping
*
* @comments    All functions here assume the DHCP snooping lock is taken.
*
* @create      4/2/07
*
* @author      rrice
*
* @end
*
**********************************************************************/



#include "l7_common.h"
#include "avl_api.h"
#include "ds_util.h"
#include "ds_cfg.h"
#include "ptin_evc.h"

#ifdef L7_IPSG_PACKAGE
#include "ds_ipsg.h"
#endif

extern dsInfo_t *dsInfo;
#ifdef L7_IPSG_PACKAGE
extern ipsgInfo_t *ipsgInfo;
#endif
extern dsCfgData_t *dsCfgData;
extern void *Ds_Event_Queue;
extern void *dsMsgQSema;


osapiTimerDescr_t *dsLeaseTimer = L7_NULLPTR;
L7_uchar8 *dsBindingTypeNames[] = {"TENTATIVE", "STATIC", "DYNAMIC"};
L7_uchar8 *dsLeaseStatusNames[] = {"UNKNOWN", "DISCOVER", "OFFER", "REQUEST", "DECLINE", "ACK", "NACK", "RELEASE", "INFORM", "", "", "SOLICIT", 
                                   "ADVERTISE", "REQUEST", "CONFIRM", "RENEW", "REBIND","REPLY", "RELEASE", "RECONFIGURE", "INFORMATIONREQUEST"};


static L7_RC_t dsBindingTreeSearch(dsBindingTreeKey_t *inputKey, L7_uint32 matchType,
                                   dsBindingTreeNode_t **binding);

static L7_RC_t dsBindingCopy(dsBindingTreeNode_t *binding, dhcpSnoopBinding_t *extBinding);

static L7_RC_t dsLeaseStatusUpdate(dsBindingTreeKey_t *key, L7_uint inetFamily, dsLeaseStatus_t leaseStatus);

extern L7_RC_t dsCheckpointCallback(dsCkptEventType_t dsEvent, L7_enetMacAddr_t *macAddr);

/*********************************************************************
* @purpose  Create the bindings database using an AVL tree
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsBindingsTableCreate(void)
{
  L7_uint32 treeHeapSize, dataNodeSize, dataHeapSize;

  /* calculate the amount of memory needed... */
  treeHeapSize = dsInfo->bindingsTable.maxBindings * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(dsBindingTreeNode_t);
  dataHeapSize = dsInfo->bindingsTable.maxBindings * dataNodeSize;

  /* ...and allocate it from the system heap */
  dsInfo->bindingsTable.treeHeapSize = treeHeapSize;
  dsInfo->bindingsTable.treeHeap = osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID, treeHeapSize);
  if (dsInfo->bindingsTable.treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Unable to allocate the DHCP snooping bindings table.");
    return L7_FAILURE;
  }

  dsInfo->bindingsTable.dataHeapSize = dataHeapSize;
  dsInfo->bindingsTable.dataHeap = osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID, dataHeapSize);
  if (dsInfo->bindingsTable.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, dsInfo->bindingsTable.treeHeap);
    dsInfo->bindingsTable.treeHeap = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Unable to allocate the DHCP snooping bindings table data heap.");
    return L7_FAILURE;
  }

  memset(dsInfo->bindingsTable.treeHeap, 0, (size_t)treeHeapSize);
  memset(dsInfo->bindingsTable.dataHeap, 0, (size_t)dataHeapSize);
  memset(&dsInfo->bindingsTable.treeData, 0, sizeof(dsInfo->bindingsTable.treeData));

  avlCreateAvlTree(&dsInfo->bindingsTable.treeData, dsInfo->bindingsTable.treeHeap,
                   dsInfo->bindingsTable.dataHeap,
                   dsInfo->bindingsTable.maxBindings, dataNodeSize, 0x10,
                   L7_ENET_MAC_ADDR_LEN);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the DHCP bindings table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsBindingsTableDelete(void)
{
  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&dsInfo->bindingsTable.treeData, dsInfo->bindingsTable.maxBindings);

  if (dsInfo->bindingsTable.treeData.semId != L7_NULLPTR)
  {
    if (avlDeleteAvlTree(&dsInfo->bindingsTable.treeData) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Failed to delete DHCP snooping AVL tree.");
      /* keep going */
    }
    dsInfo->bindingsTable.treeData.semId = L7_NULLPTR;
  }

  /* free the data heap memory */
  if (dsInfo->bindingsTable.dataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, dsInfo->bindingsTable.dataHeap);
    dsInfo->bindingsTable.dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (dsInfo->bindingsTable.treeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, dsInfo->bindingsTable.treeHeap);
    dsInfo->bindingsTable.treeHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Add an entry to the bindings table
*
* @param    bindingType  @b((input))  type of binding (static, dynamic, tentative)
* @param    macAddr      @b((input))  client MAC address.
* @param    ipAddr       @b((input))  client IP address.
* @param    vlanId       @b((input))  client VLAN ID.
* @param    intIfNum     @b((input))  internal interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If an entry already exists for the given MAC address,
*           update the other parameters.
*
* @end
*********************************************************************/
L7_RC_t dsBindingAdd(dsBindingType_t bindingType,
                     L7_enetMacAddr_t *macAddr, L7_uint32 ipv4Addr,
                     L7_ushort16 vlanId, L7_ushort16 innerVlanId /*PTin modified: DHCP */, L7_uint32 intIfNum)
{
  dsBindingTreeNode_t  binding, *pNode;

#ifdef L7_IPSG_PACKAGE
  L7_inet_addr_t ipAddr;
  inetAddressReset(&ipAddr);
  inetAddressSet(L7_AF_INET, &ipv4Addr, &ipAddr);
#endif

  memset((L7_uchar8 *)&binding, 0, sizeof(binding));
  memcpy(&binding.key.macAddr, macAddr, L7_ENET_MAC_ADDR_LEN);

  if (dsInfo->bindingsTable.staticBindings == L7_DHCP_SNOOPING_MAX_STATIC_ENTRIES)
  {
    return L7_TABLE_IS_FULL;
  }

  pNode = avlInsertEntry(&dsInfo->bindingsTable.treeData, (void *)&binding);
  if (pNode == L7_NULLPTR)
  {
    /* new AVL tree node has been added */
    pNode = avlSearchLVL7(&dsInfo->bindingsTable.treeData, &binding, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      /* cannot find new entry */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Cannot find new DHCP snooping binding entry.");
      return L7_FAILURE;                  /* node not inserted in table */
    }
    pNode->bindingType = bindingType;
    inetAddressSet(L7_AF_INET, &ipv4Addr, &pNode->ipAddr);    
    pNode->vlanId = vlanId;
    pNode->innerVlanId = innerVlanId;     /* PTin added: DHCP */
    pNode->intIfNum = intIfNum;
    pNode->leaseStart = simSystemUpTimeGet();
#ifdef L7_NSF_PACKAGE
    pNode->ckptFlag = DS_CKPT_DONE;
#endif

    if (bindingType != DS_BINDING_TENTATIVE)
    {
       if (bindingType == DS_BINDING_DYNAMIC)
       {
         dsInfo->dsDbDataChanged = L7_TRUE;
       }
       else if(bindingType == DS_BINDING_STATIC)
       {
         dsInfo->cfgDataChanged = L7_TRUE;
       }

#ifdef L7_IPSG_PACKAGE     
       ipsgEntryAdd (IPSG_ENTRY_DYNAMIC,
                     intIfNum,
                     vlanId,
                     macAddr,
                     &ipAddr);
#endif
      /*ipsgBindingHwAdd(intIfNum, ipAddr, macAddr);*/
    }
    if (bindingType == DS_BINDING_STATIC)
    {
      dsInfo->bindingsTable.staticBindings++;
    }

    if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      dsMacToString(macAddr->addr, macAddrStr);
      osapiInetNtoa(ipv4Addr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP snooping added %s binding for %s to %s on interface %s in VLAN %u.",
                    dsBindingTypeNames[bindingType], macAddrStr, ipAddrStr, ifName, vlanId);
      dsTraceWrite(dsTrace);
    }
#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if ((cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID)) &&
        (pNode->bindingType == DS_BINDING_DYNAMIC))
    {
      if(dsCheckpointCallback(DS_ENTRY_ADD, macAddr) == L7_SUCCESS)
      {
        pNode->ckptFlag = DS_CKPT_ADD;
      }
    }
#endif
  }
  else if (pNode != &binding)
  {
    if ( (pNode->bindingType == DS_BINDING_STATIC) &&
         ( (bindingType == DS_BINDING_DYNAMIC)
           ||
           (bindingType == DS_BINDING_TENTATIVE)
         )
       )
    {
      /* Need to inform the Administrator
      return L7_SUCCESS; */
      return L7_REQUEST_DENIED;
    }

    /* A binding for this MAC already exists. Delete existing binding from HW. */
    if (!inetIsInAddressAny(&pNode->ipAddr))
    {
       if (pNode->bindingType == DS_BINDING_DYNAMIC)
       {
         dsInfo->dsDbDataChanged = L7_TRUE;
       }
       else if(bindingType == DS_BINDING_STATIC)
       {
         dsInfo->cfgDataChanged = L7_TRUE;
       }

#ifdef L7_IPSG_PACKAGE
       ipsgEntryRemove (IPSG_ENTRY_DYNAMIC,
                     pNode->intIfNum,
                     pNode->vlanId,
                     macAddr,
                     &pNode->ipAddr);
#endif
      /* ipsgBindingHwRemove(pNode->intIfNum, pNode->ipAddr);*/

    }
    if ( (pNode->bindingType == DS_BINDING_DYNAMIC)&&
         (bindingType == DS_BINDING_STATIC)
       )
    {
      dsInfo->bindingsTable.staticBindings++;
    }


    /* update binding */
    pNode->bindingType = bindingType;
    pNode->ipAddr.family = L7_AF_INET;
    pNode->ipAddr.addr.ipv4.s_addr = ipv4Addr;
    pNode->vlanId = vlanId;
    pNode->intIfNum = intIfNum;

    if (bindingType != DS_BINDING_TENTATIVE)
    {
      /* Add new binding in hardware */
       if (pNode->bindingType == DS_BINDING_DYNAMIC)
       {
         dsInfo->dsDbDataChanged = L7_TRUE;
       }
       else if(bindingType == DS_BINDING_STATIC)
       {
         dsInfo->cfgDataChanged = L7_TRUE;
       }

#ifdef L7_IPSG_PACKAGE  
       ipsgEntryAdd (IPSG_ENTRY_DYNAMIC,
                     intIfNum,
                     vlanId,
                     macAddr,
                     &ipAddr);
#endif

      /* ipsgBindingHwAdd(intIfNum, ipAddr, macAddr);*/
    }

#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if ((cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID)) &&
        (pNode->bindingType == DS_BINDING_DYNAMIC))
    {
      if(dsCheckpointCallback(DS_ENTRY_MODIFY, macAddr) == L7_SUCCESS)
      {
        pNode->ckptFlag = DS_CKPT_ADD;
      }
    }
#endif

    if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      dsMacToString(macAddr->addr, macAddrStr);
      osapiInetNtoa(ipv4Addr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP snooping updated %s binding for %s to %s on interface %s in VLAN %u.",
                    dsBindingTypeNames[bindingType], macAddrStr, ipAddrStr, ifName, vlanId);
      dsTraceWrite(dsTrace);
    }
  }
  else
  {
    return L7_FAILURE;                    /* node not inserted in table */
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add an entry to the bindings table
*
* @param    bindingType  @b((input))  type of binding (static, dynamic, tentative)
* @param    macAddr      @b((input))  client MAC address.
* @param    ipAddr       @b((input))  client IP address.
* @param    vlanId       @b((input))  client VLAN ID.
* @param    intIfNum     @b((input))  internal interface number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If an entry already exists for the given MAC address,
*           update the other parameters.
*
* @notes    IPv6 compatible
*
* @end
*********************************************************************/
L7_RC_t dsv6BindingAdd(dsBindingType_t bindingType,
                     L7_enetMacAddr_t *macAddr, L7_inet_addr_t ipAddr,
                     L7_ushort16 vlanId, L7_ushort16 innerVlanId, L7_uint32 intIfNum)
{
  dsBindingTreeNode_t  binding, *pNode;

  memset((L7_uchar8 *)&binding, 0, sizeof(binding));
  memcpy(&binding.key.macAddr, macAddr, L7_ENET_MAC_ADDR_LEN);

  if (dsInfo->bindingsTable.staticBindings == L7_DHCP_SNOOPING_MAX_STATIC_ENTRIES)
  {
    return L7_TABLE_IS_FULL;
  }

  pNode = avlInsertEntry(&dsInfo->bindingsTable.treeData, (void *)&binding);
  if (pNode == L7_NULLPTR)
  {
    /* new AVL tree node has been added */
    pNode = avlSearchLVL7(&dsInfo->bindingsTable.treeData, &binding, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      /* cannot find new entry */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Cannot find new DHCP snooping binding entry.");
      return L7_FAILURE;                  /* node not inserted in table */
    }
    pNode->bindingType = bindingType;
    pNode->leaseStatus = DS_LEASESTATUS_UNKNOWN;
    pNode->flags       = 0;
    memcpy(&pNode->ipAddr, &ipAddr, sizeof(L7_inet_addr_t));
    pNode->vlanId = vlanId;
    pNode->innerVlanId = innerVlanId;     /* PTin added: DHCP */
    pNode->intIfNum = intIfNum;
    pNode->leaseStart = simSystemUpTimeGet();
#ifdef L7_NSF_PACKAGE
    pNode->ckptFlag = DS_CKPT_DONE;
#endif

    if (bindingType != DS_BINDING_TENTATIVE)
    {
       if (bindingType == DS_BINDING_DYNAMIC)
       {
         dsInfo->dsDbDataChanged = L7_TRUE;
       }
       else if(bindingType == DS_BINDING_STATIC)
       {
         dsInfo->cfgDataChanged = L7_TRUE;
       }

#ifdef L7_IPSG_PACKAGE
       ipsgEntryAdd (IPSG_ENTRY_DYNAMIC,
                     intIfNum,
                     vlanId,
                     macAddr,
                     &ipAddr);
#endif
      /*ipsgBindingHwAdd(intIfNum, ipAddr, macAddr);*/
    }
    if (bindingType == DS_BINDING_STATIC)
    {
      dsInfo->bindingsTable.staticBindings++;
    }

    if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      dsMacToString(macAddr->addr, macAddrStr);
      inetAddrPrint(&ipAddr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP snooping added %s binding for %s to %s on interface %s in VLAN %u.",
                    dsBindingTypeNames[bindingType], macAddrStr, ipAddrStr, ifName, vlanId);
      dsTraceWrite(dsTrace);
    }
#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if ((cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID)) &&
        (pNode->bindingType == DS_BINDING_DYNAMIC))
    {
      if(dsCheckpointCallback(DS_ENTRY_ADD, macAddr) == L7_SUCCESS)
      {
        pNode->ckptFlag = DS_CKPT_ADD;
      }
    }
#endif
  }
  else if (pNode != &binding)
  {
    if ( (pNode->bindingType == DS_BINDING_STATIC) &&
         ( (bindingType == DS_BINDING_DYNAMIC)
           ||
           (bindingType == DS_BINDING_TENTATIVE)
         )
       )
    {
      /* Need to inform the Administrator
      return L7_SUCCESS; */
      return L7_REQUEST_DENIED;
    }

    /* A binding for this MAC already exists. Delete existing binding from HW. */
    if ((pNode->ipAddr.family == L7_AF_INET6) && !inetIsInAddressAny(&pNode->ipAddr))
    {
       if (pNode->bindingType == DS_BINDING_DYNAMIC)
       {
         dsInfo->dsDbDataChanged = L7_TRUE;
       }
       else if(bindingType == DS_BINDING_STATIC)
       {
         dsInfo->cfgDataChanged = L7_TRUE;
       }

#ifdef L7_IPSG_PACKAGE
       ipsgEntryRemove (IPSG_ENTRY_DYNAMIC,
                     pNode->intIfNum,
                     pNode->vlanId,
                     macAddr,
                     &pNode->ipAddr);
#endif
      /* ipsgBindingHwRemove(pNode->intIfNum, pNode->ipAddr);*/

    }
    if ( (pNode->bindingType == DS_BINDING_DYNAMIC)&&
         (bindingType == DS_BINDING_STATIC)
       )
    {
      dsInfo->bindingsTable.staticBindings++;
    }


    /* update binding */
    pNode->bindingType = bindingType;
    memcpy(&pNode->ipAddr, &ipAddr, sizeof(L7_inet_addr_t));
    pNode->vlanId = vlanId;
    pNode->intIfNum = intIfNum;

    if (bindingType != DS_BINDING_TENTATIVE)
    {
      /* Add new binding in hardware */
       if (pNode->bindingType == DS_BINDING_DYNAMIC)
       {
         dsInfo->dsDbDataChanged = L7_TRUE;
       }
       else if(bindingType == DS_BINDING_STATIC)
       {
         dsInfo->cfgDataChanged = L7_TRUE;
       }

#ifdef L7_IPSG_PACKAGE
       ipsgEntryAdd (IPSG_ENTRY_DYNAMIC,
                     intIfNum,
                     vlanId,
                     macAddr,
                     &ipAddr);
#endif

      /* ipsgBindingHwAdd(intIfNum, ipAddr, macAddr);*/
    }

#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if ((cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID)) &&
        (pNode->bindingType == DS_BINDING_DYNAMIC))
    {
      if(dsCheckpointCallback(DS_ENTRY_MODIFY, macAddr) == L7_SUCCESS)
      {
        pNode->ckptFlag = DS_CKPT_ADD;
      }
    }
#endif

    if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      dsMacToString(macAddr->addr, macAddrStr);
      inetAddrPrint(&ipAddr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP snooping updated %s binding for %s to %s on interface %s in VLAN %u.",
                    dsBindingTypeNames[bindingType], macAddrStr, ipAddrStr, ifName, vlanId);
      dsTraceWrite(dsTrace);
    }
  }
  else
  {
    return L7_FAILURE;                    /* node not inserted in table */
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Remove an entry from the bindings table
*
* @param    macAddr  @b((input))  client MAC address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ipv6 compatible
*
* @end
*********************************************************************/
L7_RC_t dsBindingRemove(dsBindingTreeKey_t *key)
{
  dsBindingTreeNode_t *pNode = NULL;
  dsBindingTreeNode_t binding;
  L7_BOOL             removeNow = L7_TRUE;

  /* If IPSG enabled, remove binding from HW */
  if (dsBindingTreeSearch(key, L7_MATCH_EXACT, &pNode) == L7_SUCCESS)
  {
    if ( (pNode->ipAddr.family == L7_AF_INET || pNode->ipAddr.family == L7_AF_INET6) &&  (!inetIsInAddressAny(&pNode->ipAddr)) )
    {

      if (pNode->bindingType == DS_BINDING_DYNAMIC)
      {
        dsInfo->dsDbDataChanged = L7_TRUE;
      }
      else if(pNode->bindingType == DS_BINDING_STATIC)
      {
        dsInfo->cfgDataChanged = L7_TRUE;
      }

#ifdef L7_IPSG_PACKAGE    
     ipsgEntryRemove (IPSG_ENTRY_DYNAMIC,
                    pNode->intIfNum,
                    pNode->vlanId,
                    &key->macAddr,
                    &pNode->ipAddr);  
#endif
      /* ipsgBindingHwRemove(pNode->intIfNum, pNode->ipAddr);*/
    }
  }

  if (pNode)
  {
    if (pNode->bindingType == DS_BINDING_STATIC)
    {
        if (dsInfo->bindingsTable.staticBindings)
          dsInfo->bindingsTable.staticBindings--;
    }
#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if ((cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID)) &&
        (pNode->bindingType == DS_BINDING_DYNAMIC))
    {
      if(dsCheckpointCallback(DS_ENTRY_DELETE, macAddr) == L7_SUCCESS)
      {
        pNode->ckptFlag = DS_CKPT_DELETE;
        removeNow       = L7_FALSE;
      }
    }
#endif
    if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
      nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);
      dsMacToString(key->macAddr.addr, macAddrStr);
      inetAddrPrint(&pNode->ipAddr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "DHCP snooping removing %s binding for %s to %s on "
                    "interface %s in VLAN %u.",
                    dsBindingTypeNames[pNode->bindingType], macAddrStr,
                    ipAddrStr, ifName, pNode->vlanId);
      dsTraceWrite(dsTrace);
    }
  }

  if (removeNow)
  {
    /* If NSF feature is present and Standby unit is recognized, we would have NSF module of
     * DHCP Snooping take care of deleting the binding from the tree once checkpointing is
     * complete to the Standby. In that case 'removeNow' would have been set to L7_FALSE above */

    memset((L7_uchar8 *)&binding, 0, sizeof(binding));
    memcpy(&binding.key.macAddr.addr, key->macAddr.addr, L7_ENET_MAC_ADDR_LEN);
    pNode = avlDeleteEntry(&dsInfo->bindingsTable.treeData, &binding);
    if (pNode == NULL)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the DHCP binding table for a specific entry
*
* @param    macAddr  @b((input))  client MAC address.
* @param    matchType @b{(input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    binding   @b{(output)} ptr to node in the bindings table
*
* @returns  L7_SUCCESS  found bindings table entry
* @returns  L7_FAILURE  not found
*
* @notes    Based on the matchType parameter, can be used to find a specific bindings
*           table entry or the next sequential entry.
*
*
* @end
*********************************************************************/
static L7_RC_t dsBindingTreeSearch(dsBindingTreeKey_t *inputKey, L7_uint32 matchType,
                                   dsBindingTreeNode_t **binding)
{
  dsBindingTreeNode_t *pNode;
  dsBindingTreeNode_t key;

  memset((L7_uchar8 *)&key, 0, sizeof(key));
  memcpy(&key.key, inputKey, sizeof(inputKey));
  pNode = avlSearchLVL7(&dsInfo->bindingsTable.treeData, &key,
                        (L7_uint32)((matchType == L7_MATCH_EXACT) ? AVL_EXACT : AVL_NEXT));
#ifdef L7_NSF_PACKAGE
  /* This is a zombie node that is marked for deletion
   * and yet to be checkpointed to the standby unit.
   * Don't return such entries in the search (either exact or next match) */
  if((pNode) && (L7_MATCH_EXACT == matchType) &&
     (DS_CKPT_DELETE == pNode->ckptFlag))
  {
    pNode = L7_NULLPTR;
  }
  else if(L7_MATCH_EXACT != matchType)
  {
    while ((pNode) && (DS_CKPT_DELETE == pNode->ckptFlag))
    {
      memcpy(&key.key, &pNode->key, sizeof(pNode->key));
      pNode = avlSearchLVL7(&dsInfo->bindingsTable.treeData, &key,
                            (L7_uint32)AVL_NEXT);
    }
  }
#endif
  if (pNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not found */

  if (binding != L7_NULLPTR)
    *binding = pNode;

  return L7_SUCCESS;
}

L7_RC_t dsBindingNthEntryGet (dhcpSnoopBinding_t *dsBinding,
                               L7_uint32 n,
                               dsBindingType_t type )
{

  dsBindingTreeNode_t *pNode;
  dsBindingTreeNode_t key;
  L7_uint32 count = 0;

  memset((L7_uchar8 *)&key, 0, sizeof(key));

  while (1)
  {
    pNode = avlSearchLVL7(&dsInfo->bindingsTable.treeData, &key,
                         L7_MATCH_GETNEXT);

    if (pNode == L7_NULLPTR)
    {
      break;
    }
    else if ( pNode->bindingType == type)
    {
      count= count+1;
      if (count == n)
       break;
    }
   memcpy(&key.key.macAddr, &pNode->key.macAddr, L7_ENET_MAC_ADDR_LEN);
  }
  if ( pNode != L7_NULLPTR)
  {
    memcpy ( &dsBinding->key.macAddr, &pNode->key.macAddr, L7_ENET_MAC_ADDR_LEN);
    dsBinding->ipAddr =  pNode->ipAddr.addr.ipv4.s_addr;
    dsBinding->vlanId =  pNode->vlanId;
    dsBinding->intIfNum = pNode->intIfNum;
    dsBinding->remLease = ((pNode->leaseTime + pNode->leaseStart)-(simSystemUpTimeGet()))/60;

  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Determine if a binding exists for a given set of client parameters.
*
* @param    macAddr  @b((input/output))  client MAC address.
* @param    ipAddr   @b((input/output))  client IP address.
* @param    vlanId   @b((input/output))  client VLAN ID.
*
* @returns  L7_TRUE  if a binding matches all 3 input params exactly.
* @returns  L7_FALSE otherwise
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dsBindingExists(dsBindingTreeKey_t *key, L7_uint32 ipAddr,
                        L7_ushort16 vlanId)
{
  dsBindingTreeNode_t *binding;
  L7_inet_addr_t inet_ipAddr;

  if (dsBindingTreeSearch(key, L7_MATCH_EXACT, &binding) != L7_SUCCESS)
    return L7_FALSE;

  inet_ipAddr.family = L7_AF_INET;
  inet_ipAddr.addr.ipv4.s_addr = ipAddr;

  return (L7_INET_ADDR_COMPARE(&binding->ipAddr, &inet_ipAddr) && (binding->vlanId == vlanId));
}

/*********************************************************************
* @purpose  Copy a binding from our internal representation to the
*           external representation
*
* @param    binding  @b((input/output))  internal representation of DHCP
*                                        snooping binding entry
* @param    extBinding  @b((input/output))  external representation of DHCP
*                                           snooping binding entry
*
* @returns  L7_SUCCESS
*
* @notes    Converts lease time to remaining lease time in minutes.
*
* @end
*********************************************************************/
static L7_RC_t dsBindingCopy(dsBindingTreeNode_t *binding,
                             dhcpSnoopBinding_t *extBinding)
{
  memcpy(extBinding->key.macAddr, binding->key.macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  extBinding->ipFamily = binding->ipAddr.family;
  if(binding->ipAddr.family == L7_AF_INET)
  {
     extBinding->ipAddr = binding->ipAddr.addr.ipv4.s_addr;
  }
  else if(binding->ipAddr.family == L7_AF_INET6)
  {
     memcpy(extBinding->ipv6Addr, binding->ipAddr.addr.ipv6.in6.addr16, 16*sizeof(L7_uchar8));
  }
  extBinding->vlanId = binding->vlanId;
  extBinding->innerVlanId = binding->innerVlanId;   /* PTin added: DHCP */
  extBinding->intIfNum = binding->intIfNum;
  extBinding->bindingType = binding->bindingType;
  extBinding->remLease = 0;
  extBinding->flags = binding->flags;
  extBinding->leaseStatus = binding->leaseStatus;
  if (binding->bindingType == DS_BINDING_DYNAMIC)
  {
    if ((binding->leaseStart + binding->leaseTime) > simSystemUpTimeGet())
    {
      /* time remaining on lease */
      extBinding->remLease =
        (((binding->leaseStart + binding->leaseTime) - simSystemUpTimeGet()));
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the lease status for an existing binding table entry
*
* @param    macAddr:     Mac address that identifies the binding table entry
* @param    inetFamily:  L7_AF_INET/L7_AF_INET6
* @param    messageType: DHCP message type in the received packet
*
* @returns  L7_SUCCESS
*
* @notes    Converts lease time to remaining lease time in minutes.
*
* @end
*********************************************************************/
static L7_RC_t dsLeaseStatusUpdate(dsBindingTreeKey_t *key, L7_uint inetFamily, L7_uint messageType)
{
  dsBindingTreeNode_t *binding;

  if (dsBindingTreeSearch(key, L7_MATCH_EXACT, &binding) != L7_SUCCESS)
    return L7_FAILURE;

  if (binding->bindingType == DS_BINDING_STATIC)
  {
    return L7_SUCCESS;
  }

  LOG_TRACE(LOG_CTX_PTIN_DHCP, "Updating lease status [macAddr:%02x:%02x:%02x:%02x:%02x:%02x messageType:%u]",
            key->macAddr.addr[0], key->macAddr.addr[1], key->macAddr.addr[2], key->macAddr.addr[3], key->macAddr.addr[4], key->macAddr.addr[5], messageType);

  dsInfo->dsDbDataChanged = L7_TRUE;
  binding->leaseStatus    = messageType;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the lease flags for an existing binding table entry
*
* @param    macAddr: Mac address that identifies the binding table entry
* @param    flags  : Flags
*
* @returns  L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t dsBindingFlagsUpdate(dsBindingTreeKey_t *key, L7_uint8 flags)
{
  dsBindingTreeNode_t *binding;

  if (dsBindingTreeSearch(key, L7_MATCH_EXACT, &binding) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_DHCP, "Unable to find requested entry [macAddr:%02x:%02x:%02x:%02x:%02x:%02x]",
            key->macAddr.addr[0], key->macAddr.addr[1], key->macAddr.addr[2], key->macAddr.addr[3], key->macAddr.addr[4], key->macAddr.addr[5]);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_PTIN_DHCP, "Updating lease flags [macAddr:%02x:%02x:%02x:%02x:%02x:%02x flags:%02X]",
            key->macAddr.addr[0], key->macAddr.addr[1], key->macAddr.addr[2], key->macAddr.addr[3], key->macAddr.addr[4], key->macAddr.addr[5], flags);

  dsInfo->dsDbDataChanged = L7_TRUE;
  binding->flags          = flags;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the bindings table for a specific entry
*
* @param    extBinding  @b((input/output))  external representation of DHCP
*                                           snooping binding entry
* @param    matchType @b{(input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
*
* @returns  L7_SUCCESS  found binding entry of interest
* @returns  L7_FAILURE  binding not found
*
* @notes    Based on the matchType, can be used to find a specific bindings
*           table entry or the next sequential entry.
*
* @end
*********************************************************************/
L7_RC_t dsBindingFind(dhcpSnoopBinding_t *extBinding, L7_uint32 matchType)
{
  dsBindingTreeNode_t *binding;
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  memcpy(&key.macAddr.addr, &extBinding->key.macAddr, L7_ENET_MAC_ADDR_LEN);
  if (dsBindingTreeSearch(&key, matchType, &binding) != L7_SUCCESS)
    return L7_FAILURE;

  return dsBindingCopy(binding, extBinding);
}

/*********************************************************************
* @purpose  Set the IP address on an existing binding.
*
* @param    macAddr  @b((input))  client MAC address.
* @param    ipAddr   @b((input))  client IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsBindingIpAddrSet(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr)
{
  dsBindingTreeNode_t *binding;
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  memcpy(&key.macAddr.addr, &macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  if (dsBindingTreeSearch(&key, L7_MATCH_EXACT, &binding) != L7_SUCCESS)
    return L7_FAILURE;

  if (binding->bindingType == DS_BINDING_STATIC)
  {
    return L7_SUCCESS;
  }

  if ( (binding->ipAddr.family == L7_AF_INET || binding->ipAddr.family == L7_AF_INET6) && (!inetIsInAddressAny(&binding->ipAddr)))
  {
    dsInfo->dsDbDataChanged = L7_TRUE;
#ifdef L7_IPSG_PACKAGE
    ipsgEntryRemove (IPSG_ENTRY_DYNAMIC,
                     binding->intIfNum,
                     binding->vlanId,
                     &binding->key.macAddr,
                     &binding->ipAddr);
#endif

    /* ipsgBindingHwRemove(binding->intIfNum, binding->ipAddr);*/
  }  
  inetAddressSet(L7_AF_INET, &ipAddr, &binding->ipAddr); 
  if (ipAddr)
  {
   dsInfo->dsDbDataChanged = L7_TRUE;
   binding->bindingType = DS_BINDING_DYNAMIC;

#ifdef L7_IPSG_PACKAGE
   ipsgEntryAdd (IPSG_ENTRY_DYNAMIC,
                 binding->intIfNum,
                 binding->vlanId,
                 &binding->key.macAddr,
                 &binding->ipAddr);
#endif

    /* ipsgBindingHwAdd(binding->intIfNum, binding->ipAddr, &binding->macAddr); */

#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
    {
      if(dsCheckpointCallback(DS_ENTRY_ADD, macAddr) == L7_SUCCESS)
      {
        binding->ckptFlag = DS_CKPT_ADD;
      }
    }
#endif
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the IP address on an existing binding.
*
* @param    macAddr  @b((input))  client MAC address.
* @param    ipAddr   @b((input))  client IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ipv6 compatible
*
* @end
*********************************************************************/
L7_RC_t dsv6BindingIpAddrSet(L7_enetMacAddr_t *macAddr, L7_inet_addr_t ipAddr)
{
  dsBindingTreeNode_t *binding;
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  memcpy(&key.macAddr.addr, &macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  if (dsBindingTreeSearch(&key, L7_MATCH_EXACT, &binding) != L7_SUCCESS)
    return L7_FAILURE;

  if (binding->bindingType == DS_BINDING_STATIC)
  {
    return L7_SUCCESS;
  }

  if ( (binding->ipAddr.family == L7_AF_INET6) && (!inetIsInAddressAny(&binding->ipAddr) ))
  {
    dsInfo->dsDbDataChanged = L7_TRUE;
  #ifdef L7_IPSG_PACKAGE
      ipsgEntryRemove (IPSG_ENTRY_DYNAMIC,
                       binding->intIfNum,
                       binding->vlanId,
                       &binding->key.macAddr,
                       &binding->ipAddr);
  #endif

    /* ipsgBindingHwRemove(binding->intIfNum, binding->ipAddr);*/
  }
  memcpy(&binding->ipAddr, &ipAddr, sizeof(L7_inet_addr_t));
  if ((ipAddr.family == L7_AF_INET6) && (!inetIsInAddressAny(&ipAddr)))
  {
   dsInfo->dsDbDataChanged = L7_TRUE;
   binding->bindingType = DS_BINDING_DYNAMIC;

  #ifdef L7_IPSG_PACKAGE
     ipsgEntryAdd (IPSG_ENTRY_DYNAMIC,
                   binding->intIfNum,
                   binding->vlanId,
                   &binding->key.macAddr,
                   &binding->ipAddr);
  #endif

    /* ipsgBindingHwAdd(binding->intIfNum, binding->ipAddr, &binding->macAddr); */

#ifdef L7_NSF_PACKAGE
    /* Don't checkpoint tentative and static bindings */
    if (cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID))
    {
      if(dsCheckpointCallback(DS_ENTRY_ADD, macAddr) == L7_SUCCESS)
      {
        binding->ckptFlag = DS_CKPT_ADD;
      }
    }
#endif
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the lease status of an existing binding.
*
* @param    macAddr       @b((input))  client MAC address.
* @param    messageType   @b((input))  DHCP message type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ipv4 only
*
* @end
*********************************************************************/
L7_RC_t dsv4LeaseStatusUpdate(L7_enetMacAddr_t *macAddr, L7_uint messageType)
{
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  memcpy(&key.macAddr.addr, &macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  return dsLeaseStatusUpdate(&key, L7_AF_INET, messageType);
}

/*********************************************************************
* @purpose  Update the lease status of an existing binding.
*
* @param    macAddr       @b((input))  client MAC address.
* @param    messageType   @b((input))  DHCP message type.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Ipv6 only
*
* @end
*********************************************************************/
L7_RC_t dsv6LeaseStatusUpdate(L7_enetMacAddr_t *macAddr, L7_uint messageType)
{
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  memcpy(&key.macAddr.addr, &macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  /* 
   * DHCPv6 message types start at value 1.
   * However, in our enum DHCPv6 lease status start at 11. So, we must add an hardcoded 10 to the mesage type
   */
  messageType += 10; 

  return dsLeaseStatusUpdate(&key, L7_AF_INET6, messageType);
}

/*********************************************************************
* @purpose  Set the lease time on an existing binding.
*
* @param    macAddr    @b((input))  client MAC address.
* @param    leaseTime  @b((input))  lease time (minutes)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsBindingLeaseSet(dsBindingTreeKey_t *key, L7_uint32 leaseTime)
{
  dsBindingTreeNode_t *binding;

  if (dsBindingTreeSearch(key, L7_MATCH_EXACT, &binding) != L7_SUCCESS)
    return L7_FAILURE;

  binding->leaseTime = leaseTime;   /* convert to seconds */
  binding->leaseStart = simSystemUpTimeGet();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the number of DHCP snooping bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 _dsBindingsCount(void)
{
  return avlTreeCount(&dsInfo->bindingsTable.treeData);
}

/*********************************************************************
* @purpose  Get the number of DHCP snooping static bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 _dsStaticBindingsCount(void)
{
  return dsInfo->bindingsTable.staticBindings;
}

/*********************************************************************
*
* @purpose  Stop the DHCP snooping tick timer.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsTimerStop(void)
{
  osapiTimerFree(dsLeaseTimer);
  dsLeaseTimer = L7_NULLPTR;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Start the DHCP snooping periodic timer. On expiration, see
*           if any leases have expired.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dsTimerCallback(void)
{
  dsEventMsg_t msg;
  L7_RC_t rc;

  if (Ds_Event_Queue == L7_NULLPTR)
    return;

  /* process event on our thread */
  memset((void *)&msg, 0, sizeof(dsEventMsg_t) );
  msg.msgType = DS_TIMER_EVENT;

  rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    static L7_uint32 lastMsg = 0;
    dsInfo->debugStats.eventMsgTxError++;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = simSystemUpTimeGet();
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "Failed to queue lease timer event to DHCP snooping thread.");
    }
  }
}

/*********************************************************************
* @purpose  Do periodic check for bindings with expired leases.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsLeaseCheck(void)
{
  dsBindingTreeNode_t *binding = NULL;
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  while (dsBindingTreeSearch(&key, L7_MATCH_GETNEXT, &binding) == L7_SUCCESS)
  {
    /* store key for use in next search */
    memcpy(&key.macAddr.addr, &binding->key.macAddr.addr, L7_ENET_MAC_ADDR_LEN);

    if (binding->bindingType == DS_BINDING_STATIC)
    {
      /* static bindings don't time out */
      continue;
    }
    else if (binding->bindingType == DS_BINDING_TENTATIVE)
    {
      /* time out tentative bindings after 2 minutes */
      if (binding->leaseStart + (120) < simSystemUpTimeGet())
      {
        dsBindingRemove(&key);
        dsInfo->debugStats.bindingsRemoved++;
      }
    }
    /* dynamic binding */
    else if ((binding->leaseStart + (binding->leaseTime)) < simSystemUpTimeGet())
    {
      dsBindingRemove(&key);
      dsInfo->debugStats.bindingsRemoved++;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Validate the bindings table
*
* @param    void
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsBindingsValidate(void)
{
  dsBindingTreeNode_t *binding = NULL;
  L7_RC_t rc = L7_SUCCESS;
  dsBindingTreeKey_t key;

  memset(&key, 0x00, sizeof(key));
  while (dsBindingTreeSearch(&key, L7_MATCH_GETNEXT, &binding) == L7_SUCCESS)
  {
    if ((binding->bindingType == DS_BINDING_TENTATIVE) && (!inetIsInAddressAny(&binding->ipAddr)))
    {
      L7_uchar8 macStr[DS_MAC_STR_LEN];
      L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      dsMacToString(binding->key.macAddr.addr, macStr);
      inetAddrPrint(&binding->ipAddr, ipAddrStr);
      nimGetIntfName(binding->intIfNum, L7_SYSNAME, ifName);
      printf("\nTentative binding for %s on interface %s has IP address set to %s.",
             macStr, ifName, ipAddrStr);
      rc = L7_ERROR;
    }
    else if ((binding->bindingType != DS_BINDING_TENTATIVE) &&
             inetIsInAddressAny(&binding->ipAddr))
    {
      L7_uchar8 macStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      dsMacToString(binding->key.macAddr.addr, macStr);
      nimGetIntfName(binding->intIfNum, L7_SYSNAME, ifName);
      printf("\nBinding for %s on interface %s does not have IP address set.",
             macStr, ifName);
      rc = L7_ERROR;
    }
    /* store key for use in next search */
    memcpy(&key.macAddr.addr, &binding->key.macAddr.addr, L7_ENET_MAC_ADDR_LEN);
  }
  return rc;
}

/*********************************************************************
* @purpose  Remove all DHCP leases that belong to a given EVC ID
*
* @end
*********************************************************************/
void dsBindingEvcRemoveAll(L7_uint32 ext_evc_id)
{
  dsBindingTreeNode_t *binding = NULL;
  L7_uint16 internalVlan;
  dsBindingTreeKey_t key;

  if (L7_SUCCESS != ptin_evc_intRootVlan_get(ext_evc_id, &internalVlan))
  {
    printf("Unable to find internal vlan associated with ext_evc_id %u\n", ext_evc_id);
    return;
  }

  memset(&key, 0x00, sizeof(key));
  while (dsBindingTreeSearch(&key, L7_MATCH_GETNEXT, &binding) == L7_SUCCESS)
  {
    /* store key for use in next search */
    memcpy(&key.macAddr.addr, &binding->key.macAddr.addr, L7_ENET_MAC_ADDR_LEN);

    /* If this entry belongs to our service, remove it */
    if(binding->vlanId == internalVlan)
    {
      dsBindingRemove(&key);
    }
  }
}

/*********************************************************************
* @purpose  Display the current DHCP snooping bindings database
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsBindingTableShow(void)
{
  dsBindingTreeNode_t *binding = NULL;
  L7_uint32            count;
  L7_uint32            leaseAge;
  L7_uint32            remainingLease;
  L7_uint8             flags;
  dsBindingTreeKey_t   key;

  count = avlTreeCount(&dsInfo->bindingsTable.treeData);
  if (count == 0)
  {
    printf("\nDHCP snooping bindings table is empty.\n");
    return;
  }

  printf("\nDHCP snooping bindings table (contains %u entries): \n", count);
  printf("  MAC Address                      IP Address                     VLAN    innerVLAN  Port        Type           Status          Lease (min) \n");
  printf("-----------------  ---------------------------------------------  ------  ---------  ----------- ----------- ------------------ -------------\n");

  memset(&key, 0x00, sizeof(key));
  while (dsBindingTreeSearch(&key, L7_MATCH_GETNEXT, &binding) == L7_SUCCESS)
  {
    L7_uchar8 macStr[DS_MAC_STR_LEN];
    L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    /* store key for use in next search */
    memcpy(&key.macAddr.addr, &binding->key.macAddr.addr, L7_ENET_MAC_ADDR_LEN);

    dsMacToString(binding->key.macAddr.addr, macStr);
    inetAddrPrint(&binding->ipAddr, ipAddrStr);
    nimGetIntfName(binding->intIfNum, L7_SYSNAME, ifName);
    flags = binding->flags;

    if (binding->bindingType == DS_BINDING_DYNAMIC)
    {
      leaseAge = simSystemUpTimeGet() - binding->leaseStart;
      remainingLease = binding->leaseTime - leaseAge;
    }
    else
    {
      remainingLease = 0;
    }

    printf("%17s  %-45s  %6u  %9u  %10s %11s %18s %13u %02X\n",
           macStr, ipAddrStr, binding->vlanId, binding->innerVlanId, ifName,
           dsBindingTypeNames[binding->bindingType], 
           dsLeaseStatusNames[binding->leaseStatus], 
           remainingLease / 60, flags);
  }
  printf("\n\n");
}


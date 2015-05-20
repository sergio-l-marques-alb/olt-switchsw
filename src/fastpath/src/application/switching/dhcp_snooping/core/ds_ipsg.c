/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_ipsg.c
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


#include "l7_common.h"

#ifdef L7_IPSG_PACKAGE

#include "osapi.h"
#include "log.h"
#include "dtlapi.h"

#include "ds_cfg.h"
#include "ds_util.h"
#include "ds_ipsg.h"

extern dsCfgData_t *dsCfgData;
extern osapiRWLock_t dsCfgRWLock;
extern L7_BOOL txtBasedConfigComplete;
ipsgInfo_t *ipsgInfo;


/*********************************************************************
* @purpose  Create the IPSG entries using an AVL tree
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
L7_RC_t ipsgEntryTableCreate(void)
{
  L7_uint32 treeHeapSize, dataNodeSize, dataHeapSize;

  /* calculate the amount of memory needed... */
  treeHeapSize = ipsgInfo->ipsgEntryTable.maxBindings * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(ipsgEntryTreeNode_t);
  dataHeapSize = ipsgInfo->ipsgEntryTable.maxBindings * dataNodeSize;

  /* ...and allocate it from the system heap */
  ipsgInfo->ipsgEntryTable.treeHeapSize = treeHeapSize;
  ipsgInfo->ipsgEntryTable.treeHeap = osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID, treeHeapSize);
  if (ipsgInfo->ipsgEntryTable.treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Unable to allocate the IPSG entry table.");
    return L7_FAILURE;
  }

  ipsgInfo->ipsgEntryTable.dataHeapSize = dataHeapSize;
  ipsgInfo->ipsgEntryTable.dataHeap = osapiMalloc(L7_DHCP_SNOOPING_COMPONENT_ID, dataHeapSize);
  if (ipsgInfo->ipsgEntryTable.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, ipsgInfo->ipsgEntryTable.treeHeap);
    ipsgInfo->ipsgEntryTable.treeHeap = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
           "Unable to allocate the IPSG entry table data heap.");
    return L7_FAILURE;
  }
  memset(ipsgInfo->ipsgEntryTable.treeHeap, 0, (size_t)treeHeapSize);
  memset(ipsgInfo->ipsgEntryTable.dataHeap, 0, (size_t)dataHeapSize);
  memset(&ipsgInfo->ipsgEntryTable.treeData, 0, sizeof(ipsgInfo->ipsgEntryTable.treeData));

  avlCreateAvlTree(&ipsgInfo->ipsgEntryTable.treeData, ipsgInfo->ipsgEntryTable.treeHeap,
                   ipsgInfo->ipsgEntryTable.dataHeap,
                   ipsgInfo->ipsgEntryTable.maxBindings, dataNodeSize, 0x10,
                   sizeof(ipsgEntryKey_t));

  return L7_SUCCESS;
}

L7_uint32 ptinIpsgMaxBindings(void)
{
return (ipsgInfo->ipsgEntryTable.maxBindings);
}

/*********************************************************************
* @purpose  Delete the IPSG entries table
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
L7_RC_t ipsgEntryTableDelete(void)
{
  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&ipsgInfo->ipsgEntryTable.treeData, ipsgInfo->ipsgEntryTable.maxBindings);

  if (ipsgInfo->ipsgEntryTable.treeData.semId != L7_NULLPTR)
  {
    if (avlDeleteAvlTree(&ipsgInfo->ipsgEntryTable.treeData) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Failed to delete IPSG AVL tree.");
      /* keep going */
    }
    ipsgInfo->ipsgEntryTable.treeData.semId = L7_NULLPTR;
  }

  /* free the data heap memory */
  if (ipsgInfo->ipsgEntryTable.dataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, ipsgInfo->ipsgEntryTable.dataHeap);
    ipsgInfo->ipsgEntryTable.dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (ipsgInfo->ipsgEntryTable.treeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_SNOOPING_COMPONENT_ID, ipsgInfo->ipsgEntryTable.treeHeap);
    ipsgInfo->ipsgEntryTable.treeHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

static void ipsgEntryCopy (ipsgEntryTreeNode_t *desti,
                           ipsgEntryTreeNode_t *source)
{

  memset((L7_uchar8 *)desti, 0, sizeof(ipsgEntryTreeNode_t));
  memcpy(&desti->ipsgEntryKey.macAddr,
         &source->ipsgEntryKey.macAddr,
          L7_ENET_MAC_ADDR_LEN);
  desti->ipsgEntryKey.intIfNum = source->ipsgEntryKey.intIfNum;
  desti->ipsgEntryKey.vlanId = source->ipsgEntryKey.vlanId;
  memcpy(&desti->ipsgEntryKey.ipAddr, &source->ipsgEntryKey.ipAddr, sizeof(L7_inet_addr_t));      
}


/*********************************************************************
* @purpose  Add an entry to the IPSG table
*
* @param    entryType    @b((input))  type of binding (static, dynamic)
* @param    intIfNum     @b((input))  internal interface number.
* @param    vlanId       @b((input))  client VLAN ID.
* @param    macAddr      @b((input))  client MAC address.
* @param    ipAddr       @b((input))  client IP address.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t ipsgEntryAdd(ipsgEntryType_t entryType,
                     L7_uint32 intIfNum,
                     L7_ushort16 vlanId,
                     L7_enetMacAddr_t *macAddr,
                     L7_inet_addr_t *ipAddr)
{
#ifndef IPSG_VLAN_FIELD_IS_CARE
  vlanId = 0;
#endif

  L7_INTF_STATES_t intIfState;
  ipsgEntryTreeNode_t  ipsgEntry, *pNode;

  if ( (entryType == IPSG_ENTRY_STATIC) &&
       (ipsgInfo->ipsgEntryTable.currentStaticBindings) >=
       (ipsgInfo->ipsgEntryTable.maxStaticBindings) )

  {
    /* return L7_TABLE_IS_FULL to warn user that Max static
       entries are added */
    return L7_TABLE_IS_FULL;
  }

  memset((L7_uchar8 *)&ipsgEntry, 0, sizeof(ipsgEntryTreeNode_t));
  memcpy(&ipsgEntry.ipsgEntryKey.macAddr, macAddr, L7_ENET_MAC_ADDR_LEN);
  ipsgEntry.ipsgEntryKey.intIfNum = intIfNum;
  ipsgEntry.ipsgEntryKey.vlanId = vlanId;
  memcpy(&ipsgEntry.ipsgEntryKey.ipAddr, ipAddr, sizeof(L7_inet_addr_t));    

  pNode = avlInsertEntry(&ipsgInfo->ipsgEntryTable.treeData, (void *)&ipsgEntry);
  if (pNode == L7_NULLPTR)
  {
    /* new AVL tree node has been added */
    pNode = avlSearchLVL7(&ipsgInfo->ipsgEntryTable.treeData, &ipsgEntry, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      /* cannot find new entry */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Cannot find new IPSG binding entry.");
      return L7_FAILURE;                  /* node not inserted in table */
    }
    pNode->ipsgEntryType = entryType;
    pNode->leaseStart = simSystemUpTimeGet();

    intIfState = nimGetIntfState(intIfNum);

    /* Don't add the IPSG entries on those interfaces that are not attached */
    if((L7_INTF_ATTACHED == intIfState) &&
       (ipsgBindingHwAdd(intIfNum, ipAddr, macAddr) == L7_SUCCESS))
    {
      pNode->ipsgEntryHwStatus = L7_TRUE;
    }
    else
    {
      pNode->ipsgEntryHwStatus = L7_FALSE;
    }

    if (entryType == IPSG_ENTRY_STATIC)
      ipsgInfo->ipsgEntryTable.currentStaticBindings++;

    if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      dsMacToString(macAddr->addr, macAddrStr);
      inetAddrPrint(ipAddr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "IPSG added %d binding for %s to %s on interface %s in VLAN %u.",
                   entryType, macAddrStr, ipAddrStr, ifName, vlanId);
      dsTraceWrite(dsTrace);
    }
  }
  else if (pNode != &ipsgEntry)
  {
    /* A binding for this key already exists. We need to check for static entries */
    if ( (pNode->ipsgEntryType == IPSG_ENTRY_DYNAMIC) &&
         (entryType == IPSG_ENTRY_STATIC))

    {

       pNode->ipsgEntryType = IPSG_ENTRY_STATIC;
       ipsgInfo->ipsgEntryTable.currentStaticBindings++;
    }
    else if ((pNode->ipsgEntryType == IPSG_ENTRY_STATIC))
    {
      return L7_SUCCESS;
    }
  }
  else
  {
    return L7_FAILURE;
  }

  /* If text based configuration is not yet complete, that means
   * this function is called during bootup while adding the IPSG entries
   * from the local db (or) checkpoint db.
   * Don't set the config data change flag in that case */
  if(txtBasedConfigComplete)
  {
    dsConfigDataChange();
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Get the nth source binding for IP source guard.
*
* @param    ipsgBinding @b((output)) nth IPSG entry in the IPSG table
* @param    n           @b((input))  nth row
* @param    type        @b((input/output)) Entry type of the row
*
* @returns  L7_SUCCESS
*           L7_NOT_IMPLEMENTED_YET when IPv6 entries is found
*           L7_FAILURE when no more entries
*
*
* @end
*********************************************************************/
L7_RC_t ipv4sgBindingNthEntryGet (ipsgBinding_t *ipsgBinding,
                               L7_uint32 n,
                               ipsgEntryType_t type)

{

  ipsgEntryTreeNode_t *pNode, key;
  L7_uint32 count = 0;

  memset((L7_uchar8 *)&key, 0, sizeof(ipsgEntryTreeNode_t));

  while (1)
  {
    pNode = avlSearchLVL7(&ipsgInfo->ipsgEntryTable.treeData, &key,
                          AVL_NEXT);

    if (pNode == L7_NULLPTR)
        return L7_FAILURE;                  /* node not found */
    else if (pNode->ipsgEntryType == type)
    {
      count++;
      if (count == n)
      {
        break;
      }
    }
    memcpy(&key.ipsgEntryKey.macAddr, &pNode->ipsgEntryKey.macAddr,
                                              L7_ENET_MAC_ADDR_LEN);
    key.ipsgEntryKey.intIfNum = pNode->ipsgEntryKey.intIfNum;
    key.ipsgEntryKey.vlanId = pNode->ipsgEntryKey.vlanId;
    memcpy(&key.ipsgEntryKey.ipAddr, &pNode->ipsgEntryKey.ipAddr, sizeof(L7_inet_addr_t));    
  }


  if (L7_AF_INET != pNode->ipsgEntryKey.ipAddr.family)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Implemented Yet- %d", pNode->ipsgEntryKey.ipAddr.family);
    return L7_NOT_IMPLEMENTED_YET;
  }
  memcpy(&ipsgBinding->macAddr, &pNode->ipsgEntryKey.macAddr,
                                           L7_ENET_MAC_ADDR_LEN);
  ipsgBinding->intIfNum = pNode->ipsgEntryKey.intIfNum;
  ipsgBinding->vlanId = pNode->ipsgEntryKey.vlanId;
  ipsgBinding->ipAddr = pNode->ipsgEntryKey.ipAddr.addr.ipv4.s_addr;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the nth source binding for IP source guard.
*
* @param    ipsgBinding @b((output)) nth IPSG entry in the IPSG table
* @param    n           @b((input))  nth row
* @param    type        @b((input/output)) Entry type of the row
*
* @returns  L7_SUCCESS
*           L7_NOT_IMPLEMENTED_YET when IPv6 entries is found
*           L7_FAILURE when no more entries
*
*
* @end
*********************************************************************/
L7_RC_t ipsgBindingNthEntryGet (ipsgBinding_t *ipsgBinding,
                               L7_uint32 n,
                               ipsgEntryType_t type)

{

  ipsgEntryTreeNode_t *pNode, key;
  L7_uint32 count = 0;

  memset((L7_uchar8 *)&key, 0, sizeof(ipsgEntryTreeNode_t));

  while (1)
  {
    pNode = avlSearchLVL7(&ipsgInfo->ipsgEntryTable.treeData, &key,
                          AVL_NEXT);

    if (pNode == L7_NULLPTR)
        return L7_FAILURE;                  /* node not found */
    else if (pNode->ipsgEntryType == type)
    {
      count++;
      if (count == n)
      {
        break;
      }
    }
    memcpy(&key.ipsgEntryKey.macAddr, &pNode->ipsgEntryKey.macAddr,
                                              L7_ENET_MAC_ADDR_LEN);
    key.ipsgEntryKey.intIfNum = pNode->ipsgEntryKey.intIfNum;
    key.ipsgEntryKey.vlanId = pNode->ipsgEntryKey.vlanId;
    memcpy(&key.ipsgEntryKey.ipAddr, &pNode->ipsgEntryKey.ipAddr, sizeof(L7_inet_addr_t));    
  }


  if (L7_AF_INET != pNode->ipsgEntryKey.ipAddr.family)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Implemented Yet- %d", pNode->ipsgEntryKey.ipAddr.family);
    return L7_NOT_IMPLEMENTED_YET;
  }
  memcpy(&ipsgBinding->macAddr, &pNode->ipsgEntryKey.macAddr,
                                           L7_ENET_MAC_ADDR_LEN);
  ipsgBinding->intIfNum = pNode->ipsgEntryKey.intIfNum;
  ipsgBinding->vlanId = pNode->ipsgEntryKey.vlanId;
  ipsgBinding->ipAddr = pNode->ipsgEntryKey.ipAddr.addr.ipv4.s_addr;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the entries table for a specific entry
*
* @param    intIfNum  @b((input))  client interface number.
* @param    vlanId    @b((input))  vlan Id.
* @param    macAddr   @b((input))  client MAC address.
* @param    ipAddr    @b((input))  client IP address.
* @param    matchType @b{(input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    ipsgEntry @b{(output)} ptr to node in the bindings table
*
* @returns  L7_SUCCESS  found entry in the table entry
* @returns  L7_FAILURE  not found
*
* @notes    Based on the matchType parameter, can be used to find a specific
*           table entry or the next sequential entry.
*
*
* @end
*********************************************************************/
L7_RC_t ipsgEntryTreeSearch( L7_uint32 intIfNum,
                             L7_uint32 vlanId,
                             L7_enetMacAddr_t *macAddr,L7_inet_addr_t *ipAddr,
                             L7_uint32 matchType,
                             ipsgEntryTreeNode_t **ipsgEntry)
{
  ipsgEntryTreeNode_t *pNode, key;

  memset((L7_uchar8 *)&key, 0, sizeof(ipsgEntryTreeNode_t));
  memcpy(key.ipsgEntryKey.macAddr.addr,macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  key.ipsgEntryKey.intIfNum = intIfNum;
  key.ipsgEntryKey.vlanId = vlanId;  
  memcpy(&key.ipsgEntryKey.ipAddr, ipAddr, sizeof(L7_inet_addr_t));
   
  pNode = avlSearchLVL7(&ipsgInfo->ipsgEntryTable.treeData, &key,
                         (L7_uint32)((matchType == L7_MATCH_EXACT) ? AVL_EXACT : AVL_NEXT));

  if (pNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not found */

  if (ipsgEntry != L7_NULLPTR)
    *ipsgEntry = pNode;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Get the number of IPSG bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 _ipsgEntriesCount(void)
{
  return avlTreeCount(&ipsgInfo->ipsgEntryTable.treeData);
}

/*********************************************************************
* @purpose  Get the number of Static IPSG  bindings.
*
* @param    void
*
* @returns  number of bindings
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 _ipsgStaticEntriesCount(void)
{
  return ipsgInfo->ipsgEntryTable.currentStaticBindings;
}




/*********************************************************************
* @purpose  Respond to enable of IPSG on an interface.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    addingMacFilter @b((input)) L7_TRUE if IPSG was previously
*                           enabled w/o MAC filtering and user has now
*                           enabled MAC filtering
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgIntfEnableApply(L7_uint32 intIfNum, L7_BOOL addingMacFilter)
{
  L7_inet_addr_t       clientIpAddr;
  L7_enetMacAddr_t     clientMacAddr;
  ipsgEntryTreeNode_t *pNode;
  ipsgEntryTreeNode_t  key;
  L7_RC_t              rc;

  PT_LOG_TRACE(LOG_CTX_IPSG, "Enabling IPSG on intIfNum %u addingMacFilter:%s",
            intIfNum,addingMacFilter==L7_TRUE?"Yes":"No");

  if (dsCfgData->dsTraceFlags & DS_TRACE_IPSG_PORT)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "Enabling IPSG on port %s.", ifName);
    dsTraceWrite(traceMsg);
  }

  /* Tell HW IPSG enabled. */
  if (dtlIpsgConfig(intIfNum, L7_TRUE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Failed to enable IP Source Guard in hardware on interface %s.",
            ifName);
    return L7_FAILURE;
  }

  memset((L7_uchar8 *)&key, 0, sizeof(key));
  while ( (ipsgEntryTreeSearch (key.ipsgEntryKey.intIfNum,
                                key.ipsgEntryKey.vlanId,
                                &key.ipsgEntryKey.macAddr,
                                &key.ipsgEntryKey.ipAddr,
                                L7_MATCH_GETNEXT, &pNode)) == L7_SUCCESS)
  {
    if (pNode->ipsgEntryKey.intIfNum != intIfNum)
    {
      ipsgEntryCopy (&key, pNode);
      continue;
    }
 
    inetCopy(&clientIpAddr, &pNode->ipsgEntryKey.ipAddr);

    if (addingMacFilter)
    {
        /* first delete existing classifier entry (w/ all 0s MAC addr) */
       if (L7_AF_INET == clientIpAddr.family)
       {
         dtlIpsgIpv4ClientRemove(intIfNum, clientIpAddr.addr.ipv4);
       }
       else if (L7_AF_INET6 == clientIpAddr.family)
       {
         dtlIpsgIpv6ClientRemove(intIfNum, clientIpAddr.addr.ipv6);
       }
       else
       {
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Supported - %u", clientIpAddr.family);
       }
    }
    if (ipsgPsIsEnabled(intIfNum))  
      memcpy(&clientMacAddr.addr, &pNode->ipsgEntryKey.macAddr.addr, L7_ENET_MAC_ADDR_LEN);
    else
      memset(&clientMacAddr.addr, 0, L7_ENET_MAC_ADDR_LEN);
     
    if (L7_AF_INET == clientIpAddr.family)
    {
      rc = dtlIpsgIpv4ClientAdd(intIfNum, clientIpAddr.addr.ipv4, clientMacAddr);
    }
    else if (L7_AF_INET6 == clientIpAddr.family)
    {
      rc = dtlIpsgIpv6ClientAdd(intIfNum, clientIpAddr.addr.ipv6, clientMacAddr);
    }
    else
    {
       L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Supported - %u", clientIpAddr.family);
       rc = L7_NOT_SUPPORTED;
    }

    if (L7_ALREADY_CONFIGURED == rc)
    {
      /* Treat already configured entry as success */
      rc = L7_SUCCESS;
    }

    if (rc != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
      static L7_uint32 lastMsg = 0;
      if (osapiUpTimeRaw() > lastMsg)
      {
        lastMsg = osapiUpTimeRaw();
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        inetAddrPrint(&pNode->ipsgEntryKey.ipAddr, ipAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                "Failed to install IPSG binding for %s on interface %s in hardware."
                " This message appears on failure of adding the IPSG entry.",
                ipAddrStr, ifName);
      }
      pNode->ipsgEntryHwStatus = L7_FALSE;
    }
    else
    {
      pNode->ipsgEntryHwStatus = L7_TRUE;
    }
    ipsgEntryCopy (&key, pNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Respond to disable of IPSG on an interface.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgIntfDisableApply(L7_uint32 intIfNum)
{
  ipsgEntryTreeNode_t *pNode, key, *freeNode=L7_NULLPTR;

  if (dsCfgData->dsTraceFlags & DS_TRACE_IPSG_PORT)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "Disabling IPSG on port %s.", ifName);
    dsTraceWrite(traceMsg);
  }

  memset((L7_uchar8 *)&key, 0, sizeof(key));
  while ( (ipsgEntryTreeSearch (key.ipsgEntryKey.intIfNum,
                                key.ipsgEntryKey.vlanId,
                                &key.ipsgEntryKey.macAddr,
                                &key.ipsgEntryKey.ipAddr,
                                L7_MATCH_GETNEXT, &pNode)) == L7_SUCCESS)
  {
    if ( (pNode->ipsgEntryKey.intIfNum == intIfNum)
         &&
         (pNode->ipsgEntryHwStatus == L7_TRUE)
       )
    {
      ipsgBindingHwRemove(intIfNum, &pNode->ipsgEntryKey.ipAddr);
      /* Find one free entry and add him from IPSG table */
      if ( ipsgFindHwFreeEntry(&freeNode) == L7_SUCCESS)
      {      
       if(ipsgBindingHwAdd (freeNode->ipsgEntryKey.intIfNum,
                          &freeNode->ipsgEntryKey.ipAddr,
                         &freeNode->ipsgEntryKey.macAddr) == L7_SUCCESS)
       {
         freeNode->ipsgEntryHwStatus = L7_TRUE;
       }
      }
      pNode->ipsgEntryHwStatus = L7_FALSE;
    }
    ipsgEntryCopy (&key, pNode);

  }


  /* Tell HW IPSG disabled. HW automatically clears all bindings
   * associated with this interface. */
  if (dtlIpsgConfig(intIfNum, L7_FALSE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Failed to disable IP Source Guard in hardware on interface %s.",
            ifName);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get whether IPSG is enabled on an interface
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_TRUE if IPSG is enabled on given interface
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ipsgIsEnabled(L7_uint32 intIfNum)
{
  return dsCfgData->ipsgIntfCfg[intIfNum].verifyIp;
}

/*********************************************************************
* @purpose  Get whether IPSG port security (MAC addr checking) is
*           enabled on an interface
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_TRUE if IPSG port security is enabled
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL ipsgPsIsEnabled(L7_uint32 intIfNum)
{
  return dsCfgData->ipsgIntfCfg[intIfNum].verifyMac;
}

/*********************************************************************
* @purpose  Apply interface admin mode change.
*
* @param    intIfNum        @b((input)) internal interface number
* @param    addingMacFilter @b((input)) L7_TRUE if IPSG was previously
*                           enabled w/o MAC filtering and user has now
*                           enabled MAC filtering
*
* @returns  L7_SUCCESS
*
* @notes    config has been updated before this call
*
* @end
*********************************************************************/
L7_RC_t ipsgVerifySourceApply(L7_uint32 intIfNum, L7_BOOL addingMacFilter)
{
  if (ipsgIsEnabled(intIfNum))
  {
    return ipsgIntfEnableApply(intIfNum, addingMacFilter);
  }
  else
  {
    return ipsgIntfDisableApply(intIfNum);
  }
}

/*********************************************************************
* @purpose  Validate IPSG config and status.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgValidate(void)
{
  L7_uint32 i;
  L7_RC_t rc = L7_SUCCESS;

  if (osapiReadLockTake(dsCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 0; i < DS_MAX_INTF_COUNT; i++)
  {
    if (!ipsgIsEnabled(i) && ipsgPsIsEnabled(i))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(i, L7_SYSNAME, ifName);
      printf("\nIPSG is configured to verify MAC but not IP address on port %s",
             ifName);
      rc = L7_ERROR;
    }

    /* Warn if IPSG enabled on port but DHCP snooping is not, or port is trusted. */
    if (ipsgIsEnabled(i))
    {
      if ((dsCfgData->dsGlobalAdminMode != L7_ENABLE) || !dsIntfIsSnooping(i))
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(i, L7_SYSNAME, ifName);
        printf("\nIPSG is enabled on interface %s, but DHCP snooping is not."
               "IPSG will drop all incoming traffic on this interface.",
               ifName);
      }
      else if (_dsIntfTrustGet(i))
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(i, L7_SYSNAME, ifName);
        printf("\nIPSG is enabled on interface %s.\nThis interface is trusted "
               "for DHCP snooping.\nIPSG will drop all incoming traffic on this "
               "interface.", ifName);
      }
    }
  }

  osapiReadLockGive(dsCfgRWLock);

  if (rc == L7_SUCCESS)
  {
    printf("\nIPSG configuration and state is valid.");
  }
  return rc;
}

/*********************************************************************
* @purpose  Add an IPSG binding to hardware.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    ipAddr     @b((input)) IP address of authorized client
* @param    macAddr    @b((input)) MAC address of authorized client. May be
*                                  all zeros if IPSG port security not enabled.
*
* @returns  L7_SUCCESS if binding installed in hardware
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgBindingHwAdd(L7_uint32 intIfNum, L7_inet_addr_t* ipAddr,
                         L7_enetMacAddr_t *macAddr)
{
  L7_inet_addr_t   clientIpAddr;
  L7_enetMacAddr_t clientMacAddr;
  L7_RC_t          rc;

  /* If IPSG not enabled on bindings' interface, nothing to do. */
  if (!ipsgIsEnabled(intIfNum))
  {
    PT_LOG_TRACE(LOG_CTX_IPSG, "IPSG not enabled on bindings' interface, nothing to do.");
    return L7_FAILURE;
  }

  /* Only install bindings with non-zero IP address. Ignore tentative
   * bindings */
  if (inetIsAddressZero(ipAddr) == L7_TRUE)
  {
    return L7_FAILURE;
  }
  
  inetCopy(&clientIpAddr, ipAddr);

  if (ipsgPsIsEnabled(intIfNum))
    memcpy(&clientMacAddr.addr, &macAddr->addr, L7_ENET_MAC_ADDR_LEN);
  else
    memset(&clientMacAddr.addr, 0, L7_ENET_MAC_ADDR_LEN);

  if (L7_AF_INET == ipAddr->family)
  {
    rc = dtlIpsgIpv4ClientAdd(intIfNum, clientIpAddr.addr.ipv4, clientMacAddr);
  }
  else if (L7_AF_INET6 == ipAddr->family)
  {
    rc = dtlIpsgIpv6ClientAdd(intIfNum, clientIpAddr.addr.ipv6, clientMacAddr);
  }
  else
  {
    rc = L7_NOT_SUPPORTED;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Supported - %u", ipAddr->family);
  }
  

  if (L7_ALREADY_CONFIGURED == rc)
  {
    /* Treat already configured entry as success */
    rc = L7_SUCCESS;
  }

  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];    
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    static L7_uint32 lastMsg = 0;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = osapiUpTimeRaw();
      inetAddrPrint(ipAddr, ipAddrStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "Failed to add IPSG binding for %s for interface %s in hardware (rc:%u).",
              ipAddrStr, ifName, rc);
    }
    return rc;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update hardware with a new or changed binding.
*
* @param    binding   @b((input)) binding in dhcp snooping binding tree
*
* @returns  L7_SUCCESS if binding removed
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgBindingHwRemove(L7_uint32 intIfNum, L7_inet_addr_t* ipAddr)
{
  L7_inet_addr_t clientIpAddr;
  L7_RC_t        rc;

  /* If IPSG not enabled on bindings' interface, nothing to do. */
  if (!ipsgIsEnabled(intIfNum))
    return L7_SUCCESS;

  /* Should not have installed a binding with non-zero IP address. If asked
   * to remove one, there's a problem. */
  if (ipAddr == 0)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "IPSG asked to remove binding with IP address set to 0 on interface %s.",
            ifName);
    return L7_FAILURE;
  }

  inetCopy(&clientIpAddr, ipAddr);

  if (L7_AF_INET == ipAddr->family)
  {
    rc = dtlIpsgIpv4ClientRemove(intIfNum, clientIpAddr.addr.ipv4);
  }
  else if (L7_AF_INET6 == ipAddr->family)
  {
    rc = dtlIpsgIpv6ClientRemove(intIfNum, clientIpAddr.addr.ipv6);
  }
  else
  {
    rc = L7_NOT_SUPPORTED;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "INET_ADDR: FamilyType Not Supported - %d", ipAddr->family);
  }

  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    static L7_uint32 lastMsg = 0;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = osapiUpTimeRaw();
      inetAddrPrint(ipAddr, ipAddrStr);
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "Failed to remove IPSG binding for %s on interface %s from hardware (rc:%u).",
              ipAddrStr, ifName, rc);
    }
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Find the Next free entry to add in to the HW basded on FCFS
*
* @param    ipsgEntry : Next ipsg Entry with HW Flag set to false
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipsgFindHwFreeEntry(ipsgEntryTreeNode_t **ipsgEntry)
{


  L7_RC_t rc = L7_FAILURE;
  ipsgEntryTreeNode_t *pNode, key;

  memset((L7_uchar8 *)&key, 0, sizeof(key));
  *ipsgEntry=L7_NULLPTR;

  while ( (ipsgEntryTreeSearch (key.ipsgEntryKey.intIfNum,
                                key.ipsgEntryKey.vlanId,
                                &key.ipsgEntryKey.macAddr,
                                &key.ipsgEntryKey.ipAddr,
                                L7_MATCH_GETNEXT, &pNode)) == L7_SUCCESS)
  {

    if ( ( pNode->ipsgEntryHwStatus == L7_FALSE )
          &&
         (ipsgIsEnabled(pNode->ipsgEntryKey.intIfNum) == L7_TRUE)
       )
    {

      if (*ipsgEntry == L7_NULLPTR)
      {
        rc = L7_SUCCESS;
        *ipsgEntry = pNode;

      }
      else if ( ( (*ipsgEntry)->leaseStart)
                   >
                  (pNode->leaseStart))
      {

        *ipsgEntry = pNode;
      }
    }
    ipsgEntryCopy (&key, pNode);

  }

  return rc;

}


/*********************************************************************
* @purpose  Remove an entry from the IPSG entry table
*
* @param    entryType    @b((input))  type of binding (static, dynamic)
* @param    intIfNum     @b((input))  internal interface number.
* @param    vlanId       @b((input))  client VLAN ID.
* @param    macAddr      @b((input))  client MAC address.
* @param    ipAddr       @b((input))  client IP address.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ipsgEntryRemove (ipsgEntryType_t entryType,
                     L7_uint32 intIfNum,
                     L7_ushort16 vlanId,
                     L7_enetMacAddr_t *macAddr,
                     L7_inet_addr_t* ipAddr)

{
#ifndef IPSG_VLAN_FIELD_IS_CARE
  vlanId = 0;
#endif

  ipsgEntryTreeNode_t *pNode = NULL, *freeNode=NULL, ipsgEntry;
  dhcpSnoopBinding_t dsNode;

  /* If IPSG enabled, remove binding from HW */
  if (ipsgEntryTreeSearch(intIfNum,
                          vlanId,
                          macAddr,
                          ipAddr, L7_MATCH_EXACT, &pNode) == L7_SUCCESS)
  {
    if ( (entryType == IPSG_ENTRY_DYNAMIC)
          &&
         (pNode->ipsgEntryType == IPSG_ENTRY_STATIC)
       )
    {

     return L7_ALREADY_CONFIGURED;
    }

    if ( (entryType == IPSG_ENTRY_STATIC)
         &&
        (pNode->ipsgEntryType == IPSG_ENTRY_STATIC))
    {
      if (ipsgInfo->ipsgEntryTable.currentStaticBindings)
        ipsgInfo->ipsgEntryTable.currentStaticBindings--;
      memset((L7_uchar8 *)&dsNode, 0, sizeof(dsNode));
      memcpy(&dsNode.key.macAddr, macAddr, L7_ENET_MAC_ADDR_LEN);
      if ( ipAddr->family == L7_AF_INET )
      {
        dsNode.key.ipType = L7_AF_INET;
      }
      else if ( ipAddr->family == L7_AF_INET6 )
      {
          dsNode.key.ipType = L7_AF_INET6;
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", ipAddr->family);
        return L7_FAILURE;
      }

      /* Check if there exists an Dynamic Entry */
      if (dsBindingFind(&dsNode,L7_MATCH_EXACT) == L7_SUCCESS)
      {
        L7_inet_addr_t dsIpAddr;

        if(dsNode.ipFamily == L7_AF_INET)
        {
          inetAddressSet(L7_AF_INET, &dsNode.ipAddr, &dsIpAddr);
        }
        else if (dsNode.ipFamily == L7_AF_INET6)
        {
          inetAddressSet(L7_AF_INET6, &dsNode.ipv6Addr, &dsIpAddr);
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
              "INET_ADDR:Invalid FamilyType - %d", dsNode.ipFamily);
          return L7_FAILURE;
        }

        if ( L7_INET_ADDR_COMPARE(&dsIpAddr,ipAddr) == 0  &&
             /*(dsNode.vlanId == vlanId) &&*/
             (dsNode.intIfNum == intIfNum) )
        {
           pNode->ipsgEntryType = IPSG_ENTRY_DYNAMIC;
           dsConfigDataChange();
           return L7_SUCCESS;
        }
      }
    }

    /* Remove the entry from the HW */
    ipsgBindingHwRemove(intIfNum, ipAddr);

    /* Remove the entry from the AVL tree */
    ipsgEntryCopy (&ipsgEntry,pNode);
    pNode = L7_NULLPTR;
    pNode = avlDeleteEntry(&ipsgInfo->ipsgEntryTable.treeData, &ipsgEntry);
    if (pNode == NULL)
    {
      return L7_FAILURE;
    }
    /* Find one free entry and add him from IPSG table */
    if ( ipsgFindHwFreeEntry(&freeNode) == L7_SUCCESS)
    {

     if(ipsgBindingHwAdd (freeNode->ipsgEntryKey.intIfNum,
                          &freeNode->ipsgEntryKey.ipAddr,
                         &freeNode->ipsgEntryKey.macAddr) == L7_SUCCESS)
     {
       freeNode->ipsgEntryHwStatus = L7_TRUE;
     }
    }


  }

  if (pNode)
  {
    /*if (dsCfgData->dsTraceFlags & DS_TRACE_BINDING)
    {
      L7_uchar8 dsTrace[DS_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[DS_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);
      dsMacToString(macAddr->addr, macAddrStr);
      osapiInetNtoa(pNode->ipAddr, ipAddrStr);
      osapiSnprintf(dsTrace, DS_MAX_TRACE_LEN,
                    "IPSG removing %d binding for %s to %s on "
                    "interface %s in VLAN %u.",
                    entryType, macAddrStr,
                    ipAddrStr, ifName, vlanId);
      dsTraceWrite(dsTrace);
    }*/
  }
  else /* No entry found for this */
  {
    return L7_SUCCESS;
  }

  dsConfigDataChange();
  return L7_SUCCESS;
}

L7_RC_t _ipsgEntryClear(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t retval = L7_SUCCESS;
  ipsgEntryTreeNode_t *entry = NULL, key;


  memset(&key, 0, sizeof(ipsgEntryTreeNode_t));

  key.ipsgEntryKey.intIfNum = intIfNum;

  while ((ipsgEntryTreeSearch (key.ipsgEntryKey.intIfNum,
                               key.ipsgEntryKey.vlanId,
                               &key.ipsgEntryKey.macAddr,
                               &key.ipsgEntryKey.ipAddr,
                               L7_MATCH_GETNEXT, &entry)) == L7_SUCCESS)
 {
   rc = ipsgEntryRemove ( entry->ipsgEntryType,
                     entry->ipsgEntryKey.intIfNum,
                     entry->ipsgEntryKey.vlanId,
                     &entry->ipsgEntryKey.macAddr,
                     &entry->ipsgEntryKey.ipAddr);
   if ( rc != L7_SUCCESS)
   {
     return L7_FAILURE;
   }


 }


  return retval;
}


/*********************************************************************
* @purpose  Display the current IPSG database entries
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipsgEntryTableShow(void)
{
  ipsgEntryTreeNode_t *entry = NULL, key;
  L7_enetMacAddr_t macAddr;
  L7_uint32         count;
  L7_uint32 remainingLease;

  count = avlTreeCount(&ipsgInfo->ipsgEntryTable.treeData);
  if (count == 0)
  {
    printf("\nIPSG entry table is empty.\n");
    return;
  }

  printf("\n IPSG entries table contains (%u) entries: ", count);
  printf("\n"
         "MAC Address        IP Address                                     VLAN    intIfNum  Type    HW Status\n");
  printf("-----------------  ---------------------------------------------  ------  --------  -------  ---------\n");

  memset((L7_uchar8 *)&key, 0, sizeof(key));

   while ( (ipsgEntryTreeSearch (key.ipsgEntryKey.intIfNum,
                                key.ipsgEntryKey.vlanId,
                                &key.ipsgEntryKey.macAddr,
                                &key.ipsgEntryKey.ipAddr,
                                L7_MATCH_GETNEXT, &entry)) == L7_SUCCESS)

  {
    L7_uchar8 macStr[DS_MAC_STR_LEN];
    L7_uchar8 ipAddrStr[IPV6_DISP_ADDR_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    /* store key for use in next search */
    memcpy(&macAddr, &entry->ipsgEntryKey.macAddr.addr, L7_ENET_MAC_ADDR_LEN);

    dsMacToString(entry->ipsgEntryKey.macAddr.addr, macStr);
    inetAddrPrint(&entry->ipsgEntryKey.ipAddr, ipAddrStr);    
    nimGetIntfName(entry->ipsgEntryKey.intIfNum, L7_SYSNAME, ifName);
    {
      remainingLease = 0;
    }

    printf("%17s  %-45s  %6u  %8u  %7s  %9s\n",
           macStr, ipAddrStr, entry->ipsgEntryKey.vlanId, entry->ipsgEntryKey.intIfNum,
           entry->ipsgEntryType==IPSG_ENTRY_STATIC?"Static":"Dynamic", entry->ipsgEntryHwStatus==L7_TRUE?"Enable":"Disable");
   ipsgEntryCopy (&key, entry);
  }
  printf("\n\n");
}



#endif


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2008
*
**********************************************************************
*
* @filename  relay_svr_tree.c
*
* @purpose   Collection of UDP relay entries. Manages the UDP server address
*            configuration.
*
* @component IP Helper (formerly known as DHCP relay or UDP relay)
*
* @comments  We want to avoid imposing an artificial limit on the number of
*            relay entries per interface or the number of servers for a given
*            {ingress interface, dest udp port} pair. So we maintain a global
*            collection of relay entries and only impose a limit on the total
*            number of relay entries. We seek to optimize lookups when
*            relaying packets (as opposed to optimizing operations at configuration
*            or show time), and thus orgainize the relay entries into an AVL
*            tree whose key is {ingress interface, dest udp port}. Each node
*            in the tree is the head of a list of server IPv4 addresses. Each
*            node indicates whether the entry is a discard entry. (We could
*            have an empty server list indicate a discard entry, but having
*            an explicit discard flag helps catch coding errors.) Entries on
*            the server list are drawn from a buffer pool and maintained as
*            a doubly linked list, ordered by IP address for easier reporting.
*
*            Note that this tree is maintaining part of this component's
*            configuration. This part of the configuration cannot be stored in
*            binary form, a requirement that has fallen by the wayside for
*            routing components in the text-based configuration persistent
*            storage era.
*
*            Because this role of this component is expanded beyond BOOTP and
*            DHCP, new functions have an "ih" prefix (for "IP Helper"). And
*            "ih" is a bit shorter than "bootpdhcprelay"!
*
* @create    5/12/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "relay_config.h"
#include "avl_api.h"
#include "buff_api.h"
#include "nimapi.h"
#include "relay_util.h"


/* Just an alias for pDhcpRelayCfgData, but shorter and more generic */
extern dhcpRelayCfg_t *ihCfg;
extern dhcpRelayInfo_t *ihInfo;  /* similar */


static ihRelayEntry_t *ihRelayEntryFind(L7_uint32 intIfNum, L7_ushort16 udpPort,
                                        L7_uint32 matchType);

/*********************************************************************
* @purpose  Initialize buffer pool for server list entries.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
static L7_RC_t ihServerPoolCreate(void)
{
  L7_uint32 buffer_size = sizeof(ihServerEntry_t);
  L7_uint32 num_buffers = L7_IP_HELPER_ENTRIES_MAX;

  if (bufferPoolInit(num_buffers, buffer_size, "IP Helper Svr",
                     &ihInfo->serverPoolId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, L7_DHCP_RELAY_COMPONENT_ID,
            "Failed to allocate buffer pool for IP helper server list.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Delete pool of client connection entries.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
static L7_RC_t ihServerPoolDelete(void)
{
   return bufferPoolTerminate(ihInfo->serverPoolId);
}

/*********************************************************************
* @purpose  Add an IP address to the server list on a relay entry.
*
* @param    relayEntry - entry whose server list is augmented
* @param    serverAddr - IP address of server
*
* @returns  L7_SUCCESS
*           L7_ALREADY_CONFIGURED if server already on list
*           L7_FAILURE if input parameters bad
*           L7_TABLE_IS_FULL if the max number of IP helper entries are
*                            already configured
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihServerAddrAdd(ihRelayEntry_t *relayEntry, L7_uint32 serverAddr)
{
  ihServerEntry_t *newEntry;
  ihServerEntry_t *next = NULL;
  ihServerEntry_t *prev = NULL;

  if (!relayEntry || !serverAddr)
    return L7_FAILURE;

  /* Allocate and initialize a new server entry */
  if (bufferPoolAllocate(ihInfo->serverPoolId, (L7_uchar8**) &newEntry) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
            "The maximum number of IP Helper entries, %u, has already been configured.",
            ihCfg->relayEntryList.maxEntries);
    return L7_TABLE_IS_FULL;
  }
  newEntry->serverAddr = serverAddr;

  next = relayEntry->serverList;
  while (next && (serverAddr > next->serverAddr))
  {
    prev = next;
    next = next->nextServer;
  }

  if (next && (serverAddr == next->serverAddr))
  {
    bufferPoolFree(ihInfo->serverPoolId, (L7_uchar8*) newEntry);
    return L7_ALREADY_CONFIGURED;
  }

  newEntry->prevServer = prev;
  newEntry->nextServer = next;
  if (prev)
    prev->nextServer = newEntry;
  else
    relayEntry->serverList = newEntry;  /* insert at head */

  if (next)
    next->prevServer = newEntry;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove an IP address from the server list on a relay entry.
*
* @param    relayEntry - entry whose server list is shrinking
* @param    serverAddr - IP address of server
*
* @returns  L7_SUCCESS
*           L7_NOT_EXIST if server address not found
*           L7_FAILURE if bad input parameters
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihServerAddrDelete(ihRelayEntry_t *relayEntry, L7_uint32 serverAddr)
{
  ihServerEntry_t *serverEntry;

  if (!relayEntry || !serverAddr)
    return L7_FAILURE;

  serverEntry = relayEntry->serverList;

  while (serverEntry)
  {
    if (serverEntry->serverAddr == serverAddr)
    {
      if (serverEntry->prevServer)
        serverEntry->prevServer->nextServer = serverEntry->nextServer;
      else
        relayEntry->serverList = serverEntry->nextServer;

      if (serverEntry->nextServer)
        serverEntry->nextServer->prevServer = serverEntry->prevServer;

      bufferPoolFree(ihInfo->serverPoolId, (L7_uchar8*) serverEntry);
      return L7_SUCCESS;
    }
    serverEntry = serverEntry->nextServer;
  }
  return L7_NOT_EXIST;
}

/*********************************************************************
* @purpose  Get the server address following a given address.
*
* @param    relayEntry - entry whose server list is shrinking
* @param    prevServer - previous server address. If 0, get first server on list.
*
* @returns  IPv4 address of next server
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ihServerAddrListNext(ihRelayEntry_t *relayEntry, L7_uint32 prevServer)
{
  ihServerEntry_t *serverEntry = relayEntry->serverList;

  if (prevServer)
  {
    while (serverEntry && (serverEntry->serverAddr <= prevServer))
    {
      serverEntry = serverEntry->nextServer;
    }
  }

  if (serverEntry)
    return serverEntry->serverAddr;
  else
    return 0;
}

/*********************************************************************
* @purpose  Find out if the server list for a given relay entry is empty.
*
* @param    relayEntry - an IP helper entry
*
* @returns  L7_TRUE if server list is empty
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ihServerListEmpty(ihRelayEntry_t *relayEntry)
{
  return (relayEntry->serverList == NULL);
}

/*********************************************************************
* @purpose  Clear all server addresses for a given entry.
*
* @param    relayEntry - an IP helper entry
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ihServerListClear(ihRelayEntry_t *relayEntry)
{
  ihServerEntry_t *serverEntry = relayEntry->serverList;
  ihServerEntry_t *nextServer;

  while (serverEntry)
  {
    nextServer = serverEntry->nextServer;
    bufferPoolFree(ihInfo->serverPoolId, (L7_uchar8*) serverEntry);
    serverEntry = nextServer;
  }
  relayEntry->serverList = NULL;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create the collection of relay entries
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
L7_RC_t ihRelayEntryListCreate(void)
{
  L7_uint32 treeHeapSize, dataNodeSize, dataHeapSize;

  /* Create buffer pool for linked lists of server addresses */
  if (ihServerPoolCreate() != L7_SUCCESS)
    return L7_FAILURE;

  /* calculate the amount of memory needed... */
  treeHeapSize = ihCfg->relayEntryList.maxEntries * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(ihRelayEntry_t);
  dataHeapSize = ihCfg->relayEntryList.maxEntries * dataNodeSize;

  /* ...and allocate it from the system heap */
  ihCfg->relayEntryList.treeHeapSize = treeHeapSize;
  ihCfg->relayEntryList.treeHeap = osapiMalloc(L7_DHCP_RELAY_COMPONENT_ID, treeHeapSize);
  if (ihCfg->relayEntryList.treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_RELAY_COMPONENT_ID,
           "Unable to allocate the IP helper server list.");
    ihServerPoolDelete();
    return L7_FAILURE;
  }

  ihCfg->relayEntryList.dataHeapSize = dataHeapSize;
  ihCfg->relayEntryList.dataHeap = osapiMalloc(L7_DHCP_RELAY_COMPONENT_ID, dataHeapSize);
  if (ihCfg->relayEntryList.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_DHCP_RELAY_COMPONENT_ID, ihCfg->relayEntryList.treeHeap);
    ihCfg->relayEntryList.treeHeap = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_RELAY_COMPONENT_ID,
           "Unable to allocate the IP helper server list data heap.");
    ihServerPoolDelete();
    return L7_FAILURE;
  }

  memset(ihCfg->relayEntryList.treeHeap, 0, (size_t)treeHeapSize);
  memset(ihCfg->relayEntryList.dataHeap, 0, (size_t)dataHeapSize);
  memset(&ihCfg->relayEntryList.treeData, 0, sizeof(ihCfg->relayEntryList.treeData));

  /* key is {ingress interface, dest udp port} */
  avlCreateAvlTree(&ihCfg->relayEntryList.treeData, ihCfg->relayEntryList.treeHeap,
                   ihCfg->relayEntryList.dataHeap,
                   ihCfg->relayEntryList.maxEntries, dataNodeSize, 0x10,
                   sizeof(L7_uint32) + 2 * sizeof(L7_ushort16));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the collection of relay entries
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
L7_RC_t ihRelayEntryListDelete(void)
{
  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&ihCfg->relayEntryList.treeData, ihCfg->relayEntryList.maxEntries);

  if (ihCfg->relayEntryList.treeData.semId != L7_NULLPTR)
  {
    if (avlDeleteAvlTree(&ihCfg->relayEntryList.treeData) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_RELAY_COMPONENT_ID,
             "Failed to delete IP helper server AVL tree.");
      /* keep going */
    }
    ihCfg->relayEntryList.treeData.semId = L7_NULLPTR;
  }

  /* free the data heap memory */
  if (ihCfg->relayEntryList.dataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_RELAY_COMPONENT_ID, ihCfg->relayEntryList.dataHeap);
    ihCfg->relayEntryList.dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (ihCfg->relayEntryList.treeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DHCP_RELAY_COMPONENT_ID, ihCfg->relayEntryList.treeHeap);
    ihCfg->relayEntryList.treeHeap = L7_NULLPTR;
  }

  ihServerPoolDelete();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add an entry to the list of IP helper servers
*
* @param    intIfNum     @b((input))  internal interface number
* @param    udpPort      @b((input))  destination UDP port number
* @param    discard      @b((input))  L7_TRUE if matching packets discarded
* @param    serverAddr   @b((input))  server IPv4 address
*
* @returns  L7_SUCCESS if configuration recorded
*           L7_FAILURE   if an error
*           L7_TABLE_IS_FULL if the maximum number of IP helper entries
*                            have already been configured.
*           L7_ALREADY_CONFIGURED if entry is a duplicate
*
* @notes    Assumes error checking on the data already done in component API.
*
* @end
*********************************************************************/
L7_RC_t ihRelayEntryAdd(L7_uint32 intIfNum, L7_ushort16 udpPort,
                        L7_BOOL discard, L7_uint32 serverAddr)
{
  ihRelayEntry_t  newEntry, *pNode;
  L7_RC_t rc;

  memset((L7_uchar8 *)&newEntry, 0, sizeof(newEntry));
  newEntry.intIfNum = intIfNum;
  newEntry.udpPort = udpPort;

  pNode = avlInsertEntry(&ihCfg->relayEntryList.treeData, (void *)&newEntry);
  if (pNode == L7_NULLPTR)
  {
    /* new AVL tree node has been added */
    pNode = avlSearchLVL7(&ihCfg->relayEntryList.treeData, &newEntry, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      /* cannot find new entry */
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_RELAY_COMPONENT_ID,
             "Cannot find new IP helper AVL entry.");
      return L7_FAILURE;                  /* node not inserted in table */
    }
    pNode->intIfNum = intIfNum;
    pNode->pad = 0;
    pNode->udpPort = udpPort;
    pNode->discard = discard;
    if (!discard)
    {
      rc = ihServerAddrAdd(pNode, serverAddr);
      if (rc != L7_SUCCESS)
      {
        /* probably reached max number of server addrs. Delete new relay entry. */
        avlDeleteEntry(&ihCfg->relayEntryList.treeData, pNode);
        return rc;
      }
    }
    return L7_SUCCESS;
  }
  else
  {
    if (pNode == &newEntry)
    {
      /* error. This can happen if tree is full */
      if (avlTreeCount(&ihCfg->relayEntryList.treeData) >= ihCfg->relayEntryList.maxEntries)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
                "Cannot configure more than %u IP Helper entries.",
                ihCfg->relayEntryList.maxEntries);
      }
      return L7_TABLE_IS_FULL;
    }
    else
    {
      /* Existing entry for this interface and udp port. Add server to list. */
      if (discard)
      {
        if (pNode->discard)
          /* duplicate */
          return L7_ALREADY_CONFIGURED;
        else
        {
          pNode->discard = L7_TRUE;
          /* Replacing non-discard entry with discard entry. Delete existing server addresses */
          return ihServerListClear(pNode);
        }
      }
      if (pNode->discard)
      {
        /* Replacing discard entry with non-discard */
        pNode->discard = L7_FALSE;
      }
      return ihServerAddrAdd(pNode, serverAddr);
    }
  }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Delete a relay entry.
*
* @param    intIfNum     @b((input))  internal interface number
* @param    udpPort      @b((input))  destination UDP port number
* @param    discard      @b((input))  L7_TRUE if matching packets discarded
* @param    serverAddr   @b((input))  server IPv4 address
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_NOT_EXIST if entry not found
*
* @notes    If client is authenticated, tell hw client is gone.
*
* @end
*********************************************************************/
L7_RC_t ihRelayEntryDelete(L7_uint32 intIfNum, L7_ushort16 udpPort,
                           L7_BOOL discard, L7_uint32 serverAddr)
{
  ihRelayEntry_t *pNode = NULL;
  L7_RC_t rc = L7_SUCCESS;

  pNode = ihRelayEntryFind(intIfNum, udpPort, L7_MATCH_EXACT);
  if (pNode)
  {
    if (serverAddr)
    {
      rc = ihServerAddrDelete(pNode, serverAddr);
      if (rc != L7_SUCCESS)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        L7_uchar8 udpPortName[IH_UDP_PORT_NAME_LEN];
        L7_uchar8 serverAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
        if (intIfNum == IH_INTF_ANY)
        {
          osapiSnprintf(ifName, L7_NIM_IFNAME_SIZE, "Any");
        }
        else
        {
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        }
        ihUdpPortToString(udpPort, udpPortName);
        osapiInetNtoa(serverAddr, serverAddrStr);
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_RELAY_COMPONENT_ID,
                "Failed to delete server address %s for IP helper entry "
                "on interface %s and UDP port %s.",
                serverAddrStr, ifName, udpPortName);
      }
    }

    if (discard || ihServerListEmpty(pNode))
    {
      /* If we deleted the last server for this entry or the entry was a
       * discard entry, remove from tree. */
      pNode = avlDeleteEntry(&ihCfg->relayEntryList.treeData, pNode);
      if (pNode == NULL)
      {
        return L7_FAILURE;
      }
    }
    return rc;
  }

  return L7_NOT_EXIST;
}

/*********************************************************************
* @purpose  Delete all global relay entries.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ihGlobalRelayEntriesClear(void)
{
  ihRelayEntry_t *relayEntry;
  ihRelayEntry_t *nextEntry;
  L7_uint32 serverAddr = 0;

  relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
  if (relayEntry == 0)
    relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_GETNEXT);

  while (relayEntry)
  {
    /* get next before delete */
    nextEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                 L7_MATCH_GETNEXT);

    if (relayEntry->intIfNum == IH_INTF_ANY)
    {
      if (relayEntry->discard)
      {
        ihRelayEntryDelete(relayEntry->intIfNum, relayEntry->udpPort,
                         relayEntry->discard, serverAddr);
      }
      else
      {
        while (relayEntry->serverList)
        {
          serverAddr = relayEntry->serverList->serverAddr;
          ihRelayEntryDelete(relayEntry->intIfNum, relayEntry->udpPort,
                             relayEntry->discard, serverAddr);
        }
      }
    }

    relayEntry = nextEntry;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete all relay entries on a given interface. If no interface
*           specified, clear the entire table.
*
* @param    intIfNum  @b((input))  interface. IH_INTF_ANY to delete all.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ihRelayEntriesClear(L7_uint32 intIfNum)
{
  ihRelayEntry_t *relayEntry;
  ihRelayEntry_t *nextEntry;
  L7_uint32 serverAddr = 0;

  relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
  if (relayEntry == NULL)
    relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_GETNEXT);

  while (relayEntry)
  {
    /* get next before delete */
    nextEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                 L7_MATCH_GETNEXT);

    if ((intIfNum == IH_INTF_ANY) || (relayEntry->intIfNum == intIfNum))
    {
      if (relayEntry->discard)
      {
        ihRelayEntryDelete(relayEntry->intIfNum, relayEntry->udpPort,
                         relayEntry->discard, serverAddr);
      }
      else
      {
        while (relayEntry->serverList)
        {
          serverAddr = relayEntry->serverList->serverAddr;
          ihRelayEntryDelete(relayEntry->intIfNum, relayEntry->udpPort,
                             relayEntry->discard, serverAddr);
        }
      }
    }

    relayEntry = nextEntry;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the relay list for entries on a given interface for
*           packets to a given destination UDP port.
*
* @param    intIfNum  @b((input))  ingress interface
* @param    udpPort   @b((input))  destination UDP port number
* @param    matchType @b{(input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
*
* @returns  pointer to entry in the tree
*
* @notes    Based on the matchType parameter, can be used to find a specific entry
*           or the next sequential entry.
*
*
* @end
*********************************************************************/
static ihRelayEntry_t *ihRelayEntryFind(L7_uint32 intIfNum, L7_ushort16 udpPort,
                                        L7_uint32 matchType)
{
  ihRelayEntry_t key;
  L7_uint32 mt = (L7_uint32)((matchType == L7_MATCH_EXACT) ? AVL_EXACT : AVL_NEXT);

  memset((L7_uchar8 *)&key, 0, sizeof(key));
  key.intIfNum = intIfNum;
  key.udpPort = udpPort;
  return (ihRelayEntry_t*) avlSearchLVL7(&ihCfg->relayEntryList.treeData, &key, mt);
}

/*********************************************************************
* @purpose  Get the first relay entry associated with a given interface.
*
* @param    intIfNum  @b((input))  ingress interface
* @param    udpPort   @b((output)) destination UDP port number
* @param    serverAddr @b{(output)}  IPv4 address of server
* @param    discard   @b{(output)}  L7_TRUE if a discard entry
* @param    hitCount  @b{(output)}  Number of times this entry has
*                                   been used to relay a packet
*
* @returns  L7_SUCCESS - entry returned
*           L7_ERROR - bad param
*           L7_FAILURE - no entry
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ihIntfEntryGetFirst(L7_uint32 intIfNum, L7_ushort16 *udpPort, L7_uint32 *serverAddr,
                            L7_BOOL *discard, L7_uint32 *hitCount)
{
  ihRelayEntry_t *relayEntry;

  if (!udpPort || !serverAddr || !discard)
    return L7_ERROR;

  relayEntry = ihRelayEntryFind(intIfNum, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
  if (relayEntry)
  {
    *udpPort = relayEntry->udpPort;
    *discard = relayEntry->discard;
    *hitCount = relayEntry->hitCount;
    if (relayEntry->discard)
      *serverAddr = 0;
    else
      *serverAddr = relayEntry->serverList->serverAddr;
    return L7_SUCCESS;
  }

  relayEntry = ihRelayEntryFind(intIfNum, IH_UDP_PORT_DEFAULT, L7_MATCH_GETNEXT);
  while (relayEntry)
  {
    if (relayEntry->intIfNum == intIfNum)
    {
      *udpPort = relayEntry->udpPort;
      *discard = relayEntry->discard;
      *hitCount = relayEntry->hitCount;
      if (relayEntry->discard)
        *serverAddr = 0;
      else
        *serverAddr = relayEntry->serverList->serverAddr;
      return L7_SUCCESS;
    }
    relayEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                  L7_MATCH_GETNEXT);
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get a specific relay entry for a given interface.
*
* @param    intIfNum  @b((input))  ingress interface
* @param    udpPort   @b((input)) destination UDP port number
* @param    serverAddr @b{(input)}  IPv4 address of server
* @param    discard   @b{(input)}  L7_TRUE if a discard entry
* @param    hitCount  @b{(output)}  Number of times this entry has
*                                   been used to relay a packet
*
* @returns  L7_SUCCESS - entry returned
*           L7_ERROR - bad param
*           L7_FAILURE - no entry
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ihIntfEntryGet(L7_uint32 intIfNum, L7_ushort16 udpPort, L7_uint32 serverAddr,
                       L7_BOOL discard, L7_uint32 *hitCount)
{
  ihRelayEntry_t *relayEntry;
  ihServerEntry_t *serverEntry;

  /* Can only use this function to get interface entries */
  if (intIfNum == IH_INTF_ANY)
    return L7_ERROR;

  if (discard && serverAddr)
    return L7_ERROR;

  relayEntry = ihRelayEntryFind(intIfNum, udpPort, L7_MATCH_EXACT);
  if (relayEntry)
  {
    if (discard)
    {
      if (relayEntry->discard)
      {
        *hitCount = relayEntry->hitCount;
        return L7_SUCCESS;
      }
      if (!relayEntry->discard)
      {
        return L7_FAILURE;
      }
    }
    else
    {
      /* Looking for a non-discard entry */
      if (relayEntry->discard)
      {
        return L7_FAILURE;
      }
      else
      {
        serverEntry = relayEntry->serverList;
        while (serverEntry)
        {
          if (serverAddr == serverEntry->serverAddr)
          {
            *hitCount = relayEntry->hitCount;
            return L7_SUCCESS;
          }
          serverEntry = serverEntry->nextServer;
        }
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get a specific relay entry for a given interface.
*
* @param    udpPort   @b((input)) destination UDP port number
* @param    serverAddr @b{(input)}  IPv4 address of server
* @param    hitCount  @b{(output)}  Number of times this entry has
*                                   been used to relay a packet
*
* @returns  L7_SUCCESS - entry returned
*           L7_ERROR - bad param
*           L7_FAILURE - no entry
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ihGlobalEntryGet(L7_ushort16 udpPort, L7_uint32 serverAddr, L7_uint32 *hitCount)
{
  ihRelayEntry_t *relayEntry;
  ihServerEntry_t *serverEntry;
  L7_uint32 intIfNum = IH_INTF_ANY;

  relayEntry = ihRelayEntryFind(intIfNum, udpPort, L7_MATCH_EXACT);
  if (relayEntry)
  {
    serverEntry = relayEntry->serverList;
    while (serverEntry)
    {
      if (serverAddr == serverEntry->serverAddr)
      {
        *hitCount = relayEntry->hitCount;
        return L7_SUCCESS;
      }
      serverEntry = serverEntry->nextServer;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next relay entry associated with a given interface.
*
* @param    intIfNum  @b((input))  ingress interface
* @param    udpPort   @b((in/out)) destination UDP port number
* @param    serverAddr @b{(in/out)}  IPv4 address of server
* @param    discard   @b{(in/out)}  L7_TRUE if a discard entry
* @param    hitCount  @b{(output)}  Number of times this entry has
*                                   been used to relay a packet
*
* @returns  L7_SUCCESS - entry returned
*           L7_ERROR - bad param
*           L7_FAILURE - no entry
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t ihIntfEntryGetNext(L7_uint32 intIfNum, L7_ushort16 *udpPort, L7_uint32 *serverAddr,
                            L7_BOOL *discard, L7_uint32 *hitCount)
{
  ihRelayEntry_t *relayEntry;
  L7_uint32 nextServerAddr;

  if (!udpPort || !serverAddr || !discard)
    return L7_ERROR;

  relayEntry = ihRelayEntryFind(intIfNum, *udpPort, L7_MATCH_EXACT);
  if (relayEntry)
  {
    /* Get next server address on this entry */
    if (!relayEntry->discard)
    {
      nextServerAddr = ihServerAddrListNext(relayEntry, *serverAddr);
      if (nextServerAddr)
      {
        *serverAddr = nextServerAddr;
        return L7_SUCCESS;
      }
    }
  }

  relayEntry = ihRelayEntryFind(intIfNum, *udpPort, L7_MATCH_GETNEXT);
  while (relayEntry)
  {
    if (relayEntry->intIfNum == intIfNum)
    {
      *udpPort = relayEntry->udpPort;
      *discard = relayEntry->discard;
      *hitCount = relayEntry->hitCount;
      if (relayEntry->discard)
      {
        *serverAddr = 0;
      }
      else
      {
        *serverAddr = relayEntry->serverList->serverAddr;
      }
      return L7_SUCCESS;
    }
    relayEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                  L7_MATCH_GETNEXT);
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Find the next server address to which a packet should be relayed,
*           given the packet's ingress interface, destination UDP port, and
*           a previous server.
*
* @param    intIfNum   @b((input))  ingress interface
* @param    udpPort    @b((input))  destination UDP port number
* @param    prevServer @b((input))  previous server
* @param    count      @b((input))  if L7_TRUE, increment hit count for server
*
* @returns  IPv4 address of next server. A return value of 0.0.0.0 indicates
*           that there are no more servers.
*
* @notes    Interface-specific entries take precedence over global entries.
*           Port-specific entries take precedence over default entries.
*
*           We always increment hitCount for discard entry match, even if
*           count parameter is L7_FALSE. count parameter is really for
*           non-discard matches, because we do a lookup on the IP MAP thread
*           in the intercept function to decide whether to queue to relay
*           thread. If we hit a discard match, we don't queue the packet.
*           If we find a server address, we do queue and on relay thread do
*           a second lookup to actually relay the packet. The count parameter
*           is false in the intercept function and true in the relay function.
*
* @end
*********************************************************************/
L7_uint32 ihServerAddrGetNext(L7_uint32 intIfNum, L7_ushort16 udpPort,
                              L7_uint32 prevServer, L7_BOOL count)
{
  ihRelayEntry_t *relayEntry;
  L7_BOOL destPortOnDefaultList;

  /* Look for a match on the ingress interface and destination UDP port */
  relayEntry = ihRelayEntryFind(intIfNum, udpPort, L7_MATCH_EXACT);
  if (relayEntry)
  {
    /* Only count once if entry has multiple servers */
    if (relayEntry->discard)
    {
      if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
      {
        L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
        osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX,
                      "IP Helper matched packet to UDP port %u on interface %s to discard entry.",
                      udpPort, ifName);
        ihTraceWrite(traceBuf);
      }
      relayEntry->hitCount++;
      ihInfo->ihStats.publicStats.matchDiscardEntry++;
      return 0;
    }
    if (count && !prevServer)
      relayEntry->hitCount++;
    return ihServerAddrListNext(relayEntry, prevServer);
  }

  /* Is the destination UDP port one of the ports on our default list? */
  destPortOnDefaultList = ihDestPortOnDefaultList(udpPort);

  if (destPortOnDefaultList)
  {
    /* See if there is an interface-specific entry that doesn't specify UDP port */
    relayEntry = ihRelayEntryFind(intIfNum, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
    if (relayEntry)
    {
      if (relayEntry->discard)
      {
        if (ihInfo->ihTraceFlags & IH_TRACE_PACKET)
        {
          L7_uchar8 traceBuf[IH_TRACE_LEN_MAX];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
          osapiSnprintf(traceBuf, IH_TRACE_LEN_MAX,
                        "IP Helper matched packet on interface %s to discard entry.",
                        ifName);
          ihTraceWrite(traceBuf);
        }
        relayEntry->hitCount++;
        ihInfo->ihStats.publicStats.matchDiscardEntry++;
        return 0;
      }
      if (count && !prevServer)
        relayEntry->hitCount++;
      return ihServerAddrListNext(relayEntry, prevServer);
    }
  }

  /* No matching interface-specific entry. Look for a global match. */
  relayEntry = ihRelayEntryFind(IH_INTF_ANY, udpPort, L7_MATCH_EXACT);
  if (relayEntry)
  {
    if (count && !prevServer)
      relayEntry->hitCount++;
    return ihServerAddrListNext(relayEntry, prevServer);
  }

  if (destPortOnDefaultList)
  {
    /* Finally, look for default global match */
    relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
    if (relayEntry)
    {
      if (count && !prevServer)
        relayEntry->hitCount++;
      return ihServerAddrListNext(relayEntry, prevServer);
    }
  }

  return 0;
}

/*********************************************************************
* @purpose  Get the number of relay entries currently configured.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 ihRelayEntryCount(void)
{
  ihRelayEntry_t *relayEntry;
  ihServerEntry_t *serverEntry;
  L7_uint32 count = 0;

  relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
  if (relayEntry == NULL)
    relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_GETNEXT);

  while (relayEntry)
  {
    if (relayEntry->discard)
    {
      count++;
    }
    else
    {
      serverEntry = relayEntry->serverList;
      while (serverEntry)
      {
        count++;
        serverEntry = serverEntry->nextServer;
      }
    }

    relayEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                  L7_MATCH_GETNEXT);
  }
  return count;
}

/*********************************************************************
* @purpose  Print the client list for debugging purposes.
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void _relayEntryListShow(void)
{
  ihRelayEntry_t *relayEntry;
  ihServerEntry_t *serverEntry;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 udpPortName[IH_UDP_PORT_NAME_LEN];
  L7_uchar8 serverAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_BOOL firstServer;

  printf("\n%-20s %12s %10s %10s %18s",
         "Interface", "UDP Port", "Discard", "Hit Count", "Server Address");

  relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
  if (relayEntry == NULL)
    relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_GETNEXT);

  while (relayEntry)
  {
    if (relayEntry->intIfNum == IH_INTF_ANY)
    {
      osapiSnprintf(ifName, L7_NIM_IFNAME_SIZE, "Any");
    }
    else
    {
      nimGetIntfName(relayEntry->intIfNum, L7_SYSNAME, ifName);
    }
    ihUdpPortToString(relayEntry->udpPort, udpPortName);
    printf("\n%-20s  %12s %10s %10u",
           ifName, udpPortName, relayEntry->discard ? "Yes" : "No", relayEntry->hitCount);

    if (!relayEntry->discard)
    {
      serverEntry = relayEntry->serverList;
      firstServer = L7_TRUE;
      while (serverEntry)
      {
        osapiInetNtoa(serverEntry->serverAddr, serverAddrStr);
        if (!firstServer)
          printf("\n%54s", "");
        printf("%18s", serverAddrStr);
        firstServer = L7_FALSE;
        serverEntry = serverEntry->nextServer;
      }
    }

    relayEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                  L7_MATCH_GETNEXT);
  }
}

/* Validate all relay entries */
L7_BOOL ihRelayEntriesValid(void)
{
  ihRelayEntry_t *relayEntry;
  L7_BOOL valid = L7_TRUE;

  relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_EXACT);
  if (relayEntry == NULL)
    relayEntry = ihRelayEntryFind(IH_INTF_ANY, IH_UDP_PORT_DEFAULT, L7_MATCH_GETNEXT);

  while (relayEntry)
  {
    /* Can't have a global relay entry. */
    if (relayEntry->discard && (relayEntry->intIfNum == IH_INTF_ANY))
    {
      printf("\nGlobal entry for port %u is marked as a discard entry.",
             relayEntry->udpPort);
      valid = L7_FALSE;
    }

    /* A discard entry should not have any server addresses configured */
    if (relayEntry->discard && relayEntry->serverList)
    {
      printf("\nDiscard entry for interface %u port %u has server addresses configured.",
             relayEntry->intIfNum, relayEntry->udpPort);
      valid = L7_FALSE;
    }

    /* Non-discard entries must have at least one server configured */
    if (!relayEntry->discard && !relayEntry->serverList)
    {
      printf("\nRelay entry for interface %u port %u has no server configured",
             relayEntry->intIfNum, relayEntry->udpPort);
      valid = L7_FALSE;
    }

    if ((relayEntry->intIfNum != IH_INTF_ANY) &&
        (dhcpRelayIsValidIntf(relayEntry->intIfNum) != L7_TRUE))
    {
      printf("\nRelay entry for port %u configured on an invalid interface %u.",
             relayEntry->udpPort, relayEntry->intIfNum);
      valid = L7_FALSE;
    }

    relayEntry = ihRelayEntryFind(relayEntry->intIfNum, relayEntry->udpPort,
                                  L7_MATCH_GETNEXT);
  }
  return valid;
}



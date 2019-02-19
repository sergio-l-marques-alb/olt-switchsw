/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename  wio_client_list.c
*
* @purpose   Manage captive portal client list for wired interfaces.
*
* @component captive portal wired interface owner
*
* @comments  none
*
* @create    2/25/2008
*
* @author    rrice
*
* @end
*
**********************************************************************/


#ifdef _L7_OS_LINUX_
#include <stddef.h>
#endif

#include "l7_common.h"
#include "l7_packet.h"
#include "avl_api.h"
#include "l7_hl_api.h"
#include "wio_util.h"
#include "log.h" 
#include "l7utils_api.h"
#include "fdb_api.h"


extern wioInfo_t *wioInfo;

L7_char8 *wioClientStateNames[WIO_CLIENT_AUTH + 1] = {"UNAUTH", "PENDING", "AUTH"};


/*********************************************************************
* @purpose  Create the CP wired interface client list using an AVL tree
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    We need to be able to look up a client both by its MAC address and
*           by its IP address. CP sends us notifications using the client MAC
*           address. When WIO intercepts outgoing frames, the destination MAC 
*           address may not be the client's MAC address; it could be the MAC 
*           address of a default gateway. So when intercepting outgoing packets
*           we must be able to look up a client by its IP address. There are
*           several ways we could support both types of lookups. Let's take it
*           as a given that we have an AVL tree indexed by client MAC address.
*           Then we could have a second AVL tree indexed by client IP address,
*           with entries in this tree either simply mapping IP address to MAC
*           address (requiring a second lookup on the MAC indexed tree to get
*           all info about a client), or the entries in the IP indexed tree 
*           could duplicate the information in the MAC indexed tree. Our AVL
*           tree implementation does not allow the two trees to contain the 
*           same objects. A second option is to create a hash list indexed on 
*           client IP address and insert into the hash list the same objects
*           that are stored in the MAC indexed AVL tree. This avoids allocating
*           memory for duplicate data. It turns out, that the memory scaling for
*           the hash list option is about the same as creating a second AVL tree
*           with nodes containing just MAC and IP, but the hash list option avoids
*           a second AVL lookup. So we go with the hash list option.
*       
* @end
*********************************************************************/
L7_RC_t wioClientListCreate(void)
{
  L7_uint32 treeHeapSize, dataNodeSize, dataHeapSize;

  /* calculate the amount of memory needed... */
  treeHeapSize = wioInfo->wioClientList.maxClients * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(wioClient_t);
  dataHeapSize = wioInfo->wioClientList.maxClients * dataNodeSize;

  /* ...and allocate it from the system heap */
  wioInfo->wioClientList.treeHeapSize = treeHeapSize;
  wioInfo->wioClientList.treeHeap = osapiMalloc(L7_CP_WIO_COMPONENT_ID, treeHeapSize);
  if (wioInfo->wioClientList.treeHeap == L7_NULLPTR)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
           "Unable to allocate the Captive Portal wired interface clients list.");
    return L7_FAILURE;
  }

  wioInfo->wioClientList.dataHeapSize = dataHeapSize;
  wioInfo->wioClientList.dataHeap = osapiMalloc(L7_CP_WIO_COMPONENT_ID, dataHeapSize);
  if (wioInfo->wioClientList.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_CP_WIO_COMPONENT_ID, wioInfo->wioClientList.treeHeap);
    wioInfo->wioClientList.treeHeap = L7_NULLPTR;
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
           "Unable to allocate the captive portal wired interface client list data heap.");
    return L7_FAILURE;
  }

  memset(wioInfo->wioClientList.treeHeap, 0, (size_t)treeHeapSize);
  memset(wioInfo->wioClientList.dataHeap, 0, (size_t)dataHeapSize);
  memset(&wioInfo->wioClientList.treeData, 0, sizeof(wioInfo->wioClientList.treeData));

  avlCreateAvlTree(&wioInfo->wioClientList.treeData, wioInfo->wioClientList.treeHeap, 
                   wioInfo->wioClientList.dataHeap, 
                   wioInfo->wioClientList.maxClients, dataNodeSize, 0x10, 
                   L7_ENET_MAC_ADDR_LEN);

  /* Create a client hash list indexed on IP address. Use the container option, 
   * since the AVL tree requires the first field of wioClient_t to be the MAC address,
   * and we can't therefore make the first fields of wioClient_t "next" and "prev" for
   * the hash list. This has the unfortunate consequence of requiring a buffer pool
   * in the hash list, which more than doubles the hash list memory per client, but 
   * that's how it goes. */
  hlInit(L7_CP_WIO_COMPONENT_ID, L7_HL_VAL_CONTAINER, sizeof(L7_uint32), 
         offsetof(wioClient_t, clientIpAddr), L7_HL_LARGE_TABLE_SIZE, 
         WIO_MAX_CLIENTS, &wioInfo->wioClientIpList);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the captive portal client list
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
L7_RC_t wioClientListDelete(void)
{
  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&wioInfo->wioClientList.treeData, wioInfo->wioClientList.maxClients); 

  if (wioInfo->wioClientList.treeData.semId != L7_NULLPTR)
  {
    if (avlDeleteAvlTree(&wioInfo->wioClientList.treeData) != L7_SUCCESS)
    {
      L7_LOG(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
             "Failed to delete captive portal wired interface client list AVL tree.");
      /* keep going */
    }
    wioInfo->wioClientList.treeData.semId = L7_NULLPTR;
  }

  /* free the data heap memory */
  if (wioInfo->wioClientList.dataHeap != L7_NULLPTR)
  {
    osapiFree(L7_CP_WIO_COMPONENT_ID, wioInfo->wioClientList.dataHeap);
    wioInfo->wioClientList.dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (wioInfo->wioClientList.treeHeap != L7_NULLPTR)
  {
    osapiFree(L7_CP_WIO_COMPONENT_ID, wioInfo->wioClientList.treeHeap);
    wioInfo->wioClientList.treeHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a client to the client list
*
* @param    macAddr      @b((input))  client MAC address. 
* @param    ipAddr       @b((input))  client IP address. 
* @param    intIfNum     @b((input))  internal interface number. 
* @param    vlanid       @b((input))  VLAN client is in
*
* @returns  pointer to new client
*
* @notes    Consider it an error if the client list already contains 
*           a client with this MAC address.
*       
* @end
*********************************************************************/
wioClient_t *wioClientAdd(L7_enetMacAddr_t *macAddr, L7_uint32 ipAddr,
                          L7_uint32 intIfNum, L7_uint32 vlanId)
{
  wioClient_t  client, *pNode;

  memset((L7_uchar8 *)&client, 0, sizeof(client));
  memcpy(&client.clientMacAddr, macAddr, L7_ENET_MAC_ADDR_LEN);

  pNode = avlInsertEntry(&wioInfo->wioClientList.treeData, (void *)&client);
  if (pNode == L7_NULLPTR)
  {
    /* new AVL tree node has been added */
    pNode = avlSearchLVL7(&wioInfo->wioClientList.treeData, &client, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      /* cannot find new entry */
      L7_LOG(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
             "Cannot find new captive portal wired interface client AVL entry.");
      return NULL;                  /* node not inserted in table */
    }
    pNode->clientIpAddr = ipAddr;
    pNode->intIfNum = intIfNum;
    pNode->clientVlan = vlanId;
    pNode->clientState = WIO_CLIENT_UNAUTH;

    if (wioInfo->wioTraceFlags & WIO_TRACE_CLIENT_LIST)
    {
      L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
      L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
      osapiInetNtoa(ipAddr, ipAddrStr);
      osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                    "Captive portal client %s added on interface %s with IP address %s.",
                    macAddrStr, ifName, ipAddrStr);
      wioTraceWrite(wioTrace);
    }

    /* Add node to IP hash list */
    if (hlInsert(wioInfo->wioClientIpList, (L7_uchar8*) &ipAddr, 
                 (void*) pNode) != L7_SUCCESS)
    {
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(ipAddr, ipAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Failed to add new client %s to IP hash list.", ipAddrStr);
    }

    return pNode;
  }
  else
  {
    if (pNode == &client)
    {
      /* error. This can happen if tree is full */
      L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Error adding captive portal wired client %s on interface %s.",
              macAddrStr, ifName);
      if (avlTreeCount(&wioInfo->wioClientList.treeData) == wioInfo->wioClientList.maxClients)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
                "Captive portal wired client list is full (%u clients).",
                wioInfo->wioClientList.maxClients);
      }
      return NULL;
    }
    else
    {
      /* client already on client list */
      L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      L7_uchar8 dupIfName[L7_NIM_IFNAME_SIZE + 1];
      L7_uchar8 dupIpAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      nimGetIntfName(pNode->intIfNum, L7_SYSNAME, dupIfName);
      l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
      osapiInetNtoa(ipAddr, ipAddrStr);
      osapiInetNtoa(pNode->clientIpAddr, dupIpAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Duplicate captive portal client %s learned on interface %s with IP address %s."
              "Existing client entry on interface %s with IP address %s.",
              macAddrStr, ifName, ipAddrStr, dupIfName, dupIpAddrStr);
      return NULL;
    }
  }
  
  return NULL;
}


/*********************************************************************
* @purpose  Remove a client from the client list
*
* @param    macAddr  @b((input))  client MAC address. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If client is authenticated, tell hw client is gone.
*
* @end
*********************************************************************/
L7_RC_t wioClientRemove(L7_enetMacAddr_t *macAddr)
{
  wioClient_t *pNode = NULL;
  wioClient_t client;

  if (wioClientFind(macAddr, L7_MATCH_EXACT, &pNode) == L7_SUCCESS)
  {
    if (pNode)
    {
      if (wioInfo->wioTraceFlags & WIO_TRACE_CLIENT_LIST)
      {
        L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
        L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);
        l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
        osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                      "Captive portal client %s removed on interface %s.",
                      macAddrStr, ifName);
        wioTraceWrite(wioTrace);
      }

      if (pNode->clientState == WIO_CLIENT_AUTH)
      {
        if (dtlFdbMacAddrDelete(macAddr->addr,
                                pNode->intIfNum,
                                pNode->clientVlan,
                                L7_FDB_ADDR_FLAG_STATIC,
                                L7_NULL) != L7_SUCCESS)
        {
          wioIntfInfo_t *intfInfo = (wioIntfInfo_t*) wioIntfInfoGet(pNode->intIfNum);
          L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);
          l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
                  "Failed to remove client %s on interface %s from L2 FDB.",
                  macAddrStr, ifName);
          intfInfo->debugStats.fdbDelFailure++;
        }
      }
    }
  }
  else
  {
    wioInfo->debugStats.clientRemoveFailed++;
    return L7_FAILURE;
  }

  if (pNode)
  {
    /* Remove all connections to this client */
    wioClientConnectionsRemove(pNode->intIfNum, pNode->clientIpAddr);

    /* Remove from IP hash list */
    if (hlDelete(wioInfo->wioClientIpList, (L7_uchar8*) &pNode->clientIpAddr, 
                 (void*) pNode) != L7_SUCCESS)
    {
      L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
      osapiInetNtoa(pNode->clientIpAddr, ipAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Failed to remove client %s from IP hash list.", ipAddrStr);
    }
  }

  memset((L7_uchar8 *)&client, 0, sizeof(client));
  memcpy(&client.clientMacAddr, macAddr, L7_ENET_MAC_ADDR_LEN);
  pNode = avlDeleteEntry(&wioInfo->wioClientList.treeData, &client);
  if (pNode == NULL)
    return L7_FAILURE;
  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove all clients on a given interface from the client list
*
* @param    intIfNum  @b((input))  internal interface number. If 0, 
*                                  deletes all clients. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Called when CP disabled on an interface
*
* @end
*********************************************************************/
L7_RC_t wioClientsOnIntfRemove(L7_uint32 intIfNum)
{
  wioClient_t *client = NULL;
  wioClient_t *nextClient;
  L7_enetMacAddr_t zeroMac;
  L7_RC_t rc;

  memset(&zeroMac, 0, sizeof(L7_enetMacAddr_t));

  rc = wioClientFind(&zeroMac, L7_MATCH_GETNEXT, &client);
  while ((rc == L7_SUCCESS) && (client != NULL))
  {
    rc = wioClientFind(&client->clientMacAddr, L7_MATCH_GETNEXT, &nextClient);
    if ((intIfNum == 0) || (client->intIfNum == intIfNum))
    {
      wioClientRemove(&client->clientMacAddr);
    }
    client = nextClient;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the client list for a client with a given MAC address.
*
* @param    macAddr  @b((input))  client MAC address. 
* @param    matchType @b{(input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    client   @b{(output)} ptr to node in the client list
*
* @returns  L7_SUCCESS  found client entry
* @returns  L7_FAILURE  not found
*
* @notes    Based on the matchType parameter, can be used to find a specific client
*           or the next sequential entry.
*
*
* @end
*********************************************************************/
L7_RC_t wioClientFind(L7_enetMacAddr_t *macAddr, L7_uint32 matchType, 
                      wioClient_t **client)
{
  wioClient_t *pNode;
  wioClient_t key;

  memset((L7_uchar8 *)&key, 0, sizeof(key));
  memcpy(&key.clientMacAddr, macAddr, L7_ENET_MAC_ADDR_LEN);
  pNode = avlSearchLVL7(&wioInfo->wioClientList.treeData, &key,
                        (L7_uint32)((matchType == L7_MATCH_EXACT) ? AVL_EXACT : AVL_NEXT));

  if (pNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not found */

  if (client != L7_NULLPTR)
    *client = pNode;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Search the client list for a client with a given IPv4 address.
*
* @param    ipAddr  @b((input))  IPv4 address 
*
* @returns  client entry or NULL
*
* @notes    
*
*
* @end
*********************************************************************/
wioClient_t *wioClientFindByIpAddr(L7_uint32 ipAddr)
{
  wioClient_t *client = NULL;

  if (hlFindFirst(wioInfo->wioClientIpList, (L7_uchar8*) &ipAddr, 
                  (void**) &client) == L7_SUCCESS)
  {
    return client;
  }
  return NULL;
}

/*********************************************************************
* @purpose  Set a client's authentication state.
*
* @param    macAddr  @b((input))  client MAC address. 
* @param    newState @b{(input)}  client's new state
*
* @returns  L7_SUCCESS    found client entry
*           L7_NOT_EXIST  if client does not exist
*           L7_ERROR      if newState is invalid
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t wioClientStateSet(L7_enetMacAddr_t *macAddr, e_WioClientState newState)
{
  wioClient_t *client;
  e_WioClientState prevState;

  if (wioClientFind(macAddr, L7_MATCH_EXACT, &client) != L7_SUCCESS)
  {
    L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
    l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Captive portal reported state change for unknown client %s.",
              macAddrStr);
    return L7_NOT_EXIST;                  /* node not found */
  }

  prevState = client->clientState;

  /* Verify that state transition is valid */
  if (newState < prevState)
  {
    L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(client->intIfNum, L7_SYSNAME, ifName);
    l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
            "Illegal state transition for client %s on interface %s from %s to %s.",
            macAddrStr, ifName, wioClientStateNames[prevState], 
            wioClientStateNames[newState]);
    return L7_FAILURE;
  }

  client->clientState = newState;

  if (wioInfo->wioTraceFlags & WIO_TRACE_CLIENT_LIST)
  {
    L7_uchar8 wioTrace[WIO_MAX_TRACE_LEN];
    L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(client->intIfNum, L7_SYSNAME, ifName);
    l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
    osapiSnprintf(wioTrace, WIO_MAX_TRACE_LEN, 
                  "Captive portal client %s on interface %s moved from state %s to state %s.",
                  macAddrStr, ifName, wioClientStateNames[prevState], 
                  wioClientStateNames[newState]);
    wioTraceWrite(wioTrace);
  }

  if ((newState == WIO_CLIENT_AUTH) && (prevState != WIO_CLIENT_AUTH))
  {
    /* Remove connections for this client to free resources */
    wioClientConnectionsRemove(client->intIfNum, client->clientIpAddr);

    /* Insert a static fdb entry for client. Don't put static entry in application
     * FDB, otherwise static entry could get saved as part of configuration. */
    if (dtlFdbMacAddrAdd(client->clientMacAddr.addr,
                         client->intIfNum,
                         client->clientVlan,
                         L7_FDB_ADDR_FLAG_STATIC,
                         L7_NULL) != L7_SUCCESS)
    {
      wioIntfInfo_t *intfInfo = (wioIntfInfo_t*) wioIntfInfoGet(client->intIfNum);
      L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(client->intIfNum, L7_SYSNAME, ifName);
      l7utilsMacAddrHexToString(macAddr->addr, WIO_MAC_STR_LEN, macAddrStr);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CP_WIO_COMPONENT_ID,
              "Failed to add client %s on interface %s to L2 FDB.",
              macAddrStr, ifName);
      intfInfo->debugStats.fdbAddFailure++;
    }
  }

  return L7_SUCCESS;
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
void _wioClientListShow(void)
{
  wioClient_t *client;
  L7_enetMacAddr_t zeroMac;
  L7_RC_t rc;
  L7_uchar8 macAddrStr[WIO_MAC_STR_LEN];
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 ipAddrStr[OSAPI_INET_NTOA_BUF_SIZE];

  printf("\n%-20s %16s %10s %10s %5s",
         "MAC Address", "Client IP Address", "State", "Interface", "VLAN");

  memset(&zeroMac, 0, sizeof(L7_enetMacAddr_t));
  rc = wioClientFind(&zeroMac, L7_MATCH_GETNEXT, &client);
  while ((rc == L7_SUCCESS) && (client != NULL))
  {
    nimGetIntfName(client->intIfNum, L7_SYSNAME, ifName);
    l7utilsMacAddrHexToString(client->clientMacAddr.addr, WIO_MAC_STR_LEN, macAddrStr);
    osapiInetNtoa(client->clientIpAddr, ipAddrStr);
    printf("\n%-20s  %16s %10s %10s %5u", 
           macAddrStr, ipAddrStr, wioClientStateNames[client->clientState], 
           ifName, client->clientVlan);

    rc = wioClientFind(&client->clientMacAddr, L7_MATCH_GETNEXT, &client);    
  }
}


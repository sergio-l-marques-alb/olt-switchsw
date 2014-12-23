/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    ipmap_arp_table.c
* @purpose     IP Mapping layer internal ARP table maintenance functions
* @component   IP Mapping Layer -- ARP
* @comments    Does not manage the actual ARP Table, just some lookup tables
*
* @comments    All functions in this file are internal to the ARP
*              component and must not be called without first taking
*              the ARP lock (via the API).
*
*              None of the functions in this file take the lock.
*
* @create      06/18/2001
* @author      gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#include "l7_common.h"
#include "ipmap_arp.h"
#include "avl_api.h"
#include "xx.ext"
#include "timer.ext"

/* Global data */
extern L7_BOOL        ipMapRoutingStarted;
extern ipMapArpCtx_t    ipMapArpCtx_g;
extern ipMapArpIntf_t   *ipMapArpIntfInUse_g[];
extern ipMapArpLockCtrl_t ipMapArpTableChangeMutex;

ipMapArpAgeList_t ipMapArpAgeList_g;

/* display string for debug show functions */
char *ipMapArpEntryTypeStr[] =
  { "Unused", "Local", "Static", "Gateway", "Dynamic", "Maint" };

/* internal function prototypes */
static L7_RC_t ipMapArpIpTableRemoveCommon(ipMapArpIpNode_t *pNode);
static L7_RC_t ipMapArpIpTableOldestFindCommon(L7_BOOL allowGw,
                                               ipMapArpIpNode_t **ppIp);

/* Timer expiry routine for sending the ARP requests for the GW entries */
static e_Err gwArpRequestTimeExp(t_Handle adrId, t_Handle timer, word flags);


/*********************************************************************
* @purpose  Compare keys for the AVL tree. Key consists of IP Address +
* Interface number
*
* @param    a     Pointer to first key
* @param    b     Pointer to second key
* @param    len   Length of the key
*
* @returns  0     if keys are equal
* @returns  1     if a > b
* @returns  -1    if a < b
*
* @notes    none
*
* @end
*********************************************************************/
static int ipMapArpIpIntfCompare(const void *a, const void *b, size_t len)
{
  L7_uint32 ip1, ip2;
  L7_uint32 intf1, intf2;

  if (len != (sizeof(ip1) + sizeof(intf1)))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "Invalid key length in ipMapArpIpIntfCompare\n" );
    L7_assert(1);                     /* need to know about this */
    return -1;
  }

  ip1 = *((L7_uint32 *)a);
  ip2 = *((L7_uint32 *)b);
  intf1 = *((L7_uint32 *)a+1);
  intf2 = *((L7_uint32 *)b+1);

  if (ip1 > ip2)
  {
    return 1;
  }

  if (ip1 < ip2)
  {
    return -1;
  }

  if (intf1 > intf2)
  {
    return 1;
  }

  if (intf1 < intf2)
  {
    return -1;
  }

  return 0;
}

/*********************************************************************
* @purpose  Create the gateway table using an AVL tree
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
L7_RC_t ipMapArpGwTableCreate(void)
{
  static const char *routine_name = "ipMapArpGwTableCreate";
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;
  L7_uint32     treeHeapSize, dataNodeSize, dataHeapSize;


  /* calculate the amount of memory needed... */
  treeHeapSize = pArpCtx->gwTbl.treeEntryMax * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(ipMapArpGwNode_t);
  dataHeapSize = pArpCtx->gwTbl.treeEntryMax * dataNodeSize;

  /* ...and allocate it from the system heap */
  pArpCtx->gwTbl.treeHeapSize = treeHeapSize;
  pArpCtx->gwTbl.treeHeap = osapiMalloc(L7_ARP_MAP_COMPONENT_ID, treeHeapSize);
  if (pArpCtx->gwTbl.treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to allocate Gateway Table tree heap\n", routine_name);
    return L7_FAILURE;
  }

  pArpCtx->gwTbl.dataHeapSize = dataHeapSize;
  pArpCtx->gwTbl.dataHeap = osapiMalloc(L7_ARP_MAP_COMPONENT_ID, dataHeapSize);
  if (pArpCtx->gwTbl.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->gwTbl.treeHeap);
    pArpCtx->gwTbl.treeHeap = L7_NULLPTR;
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to allocate Gateway Table data heap\n", routine_name);
    return L7_FAILURE;
  }

  memset(pArpCtx->gwTbl.treeHeap, 0, (size_t)treeHeapSize);
  memset(pArpCtx->gwTbl.dataHeap, 0, (size_t)dataHeapSize);
  memset(&pArpCtx->gwTbl.treeData, 0, sizeof(pArpCtx->gwTbl.treeData));

  avlCreateAvlTree(&pArpCtx->gwTbl.treeData, pArpCtx->gwTbl.treeHeap,
                   pArpCtx->gwTbl.dataHeap,
                   pArpCtx->gwTbl.treeEntryMax, dataNodeSize, 0x10,
                   (L7_uint32)(2*sizeof(L7_uint32)));
  (void)avlSetAvlTreeComparator(&pArpCtx->gwTbl.treeData,ipMapArpIpIntfCompare);

  if(TIMER_InitSec(1, pArpCtx, &pArpCtx->gwArpRqstTimer) != E_OK)
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Failed to init gwArpRqstTimer\n", routine_name);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the gateway table
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
L7_RC_t ipMapArpGwTableDelete(void)
{
  static const char *routine_name = "ipMapArpGwTableDelete";
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;


  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&pArpCtx->gwTbl.treeData, pArpCtx->gwTbl.treeEntryMax);

  /* NOTE:  The AVL utility currently does not have a deletion function,
   *        so the semaphore needs to be deleted manually here.
   */
  if (pArpCtx->gwTbl.treeData.semId != L7_NULLPTR)
  {
    if (osapiSemaDelete(pArpCtx->gwTbl.treeData.semId) == L7_SUCCESS)
    {
      pArpCtx->gwTbl.treeData.semId = L7_NULLPTR;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Could not delete AVL tree semaphore, id=0x%8.8x\n",
              routine_name, (L7_uint32)pArpCtx->gwTbl.treeData.semId);
      /* keep going */
    }
  }

  /* free the data heap memory */
  if (pArpCtx->gwTbl.dataHeap != L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->gwTbl.dataHeap);
    pArpCtx->gwTbl.dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (pArpCtx->gwTbl.treeHeap != L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->gwTbl.treeHeap);
    pArpCtx->gwTbl.treeHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Destroy the gateway table
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
L7_RC_t ipMapArpGwTableDestroy(void)
{
#if WPJ_DOUBLE_CHECK_NEW_REL_G_TREES
  if (avlDeleteAvlTree(&ipMapArpGwTreeData) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Insert an entry into the gateway table
*
* @param    ipAddr   @b{(input)}   gateway IP address to add to table
* @param    intIfNum    Internal Interface number for the entry
* @param    staticRouteGw @b{(input)}  if this is gateway of static route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Attempts to insert a new gateway node into the gateway
*           table.  If the gateway already exists in the table, its
*           existing node is used instead.
*
* @notes    The route reference count is set to 1 for a new gateway
*           node and is incremented for an existing node.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableInsert(L7_uint32 ipAddr, L7_uint32 intIfNum,
                              L7_BOOL staticRouteGw)
{
  static const char *routine_name = "ipMapArpGwTableInsert";
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  ipMapArpGwNode_t  gw, *pNode;
  L7_RC_t           rc;

  rc = L7_SUCCESS;

  osapiInetNtoa(ipAddr, ipStr);

  memset((L7_uchar8 *)&gw, 0, sizeof(gw));
  gw.ipAddr = ipAddr;
  gw.intIfNum = intIfNum;
  gw.refCnt = IPM_ARP_GW_REFCNT_START;  /* ALWAYS start a potential new entry with count=1 */
  gw.holdover = L7_FALSE;               /* never have a holdover for a new entry */
  gw.staticRouteGw = staticRouteGw;

  pNode = avlInsertEntry(&ipMapArpCtx_g.gwTbl.treeData, (void *)&gw);

  if (pNode == L7_NULLPTR)
  {
    /* new AVL entry has been added (NOTE: pNode == L7_NULLPTR here)
     *  - search for the new entry to obtain its pointer
     */
    pNode = avlSearchLVL7(&ipMapArpCtx_g.gwTbl.treeData, &gw, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* cannot find new entry */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Cannot find new ARP Gateway entry for %s intf %s\n",
              routine_name, (char *)ipStr, ifName);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not inserted in table */
    }

    else
    {
      ipMapArpIpNode_t  *pIp;

      /* new gateway:
       * if the gateway addr is contained in the IP lookup table, update its flag
       */
      if (ipMapArpIpTableSearch(pNode->ipAddr, intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) == L7_SUCCESS)
      {
        if (pIp->arpEntry.entryType == ARP_ENTRY_TYPE_DYNAMIC)
          pIp->arpEntry.entryType = ARP_ENTRY_TYPE_GATEWAY;
      }

      /* when the first gateway arp entry added, start the ARP request timer */
      if(avlTreeCount(&ipMapArpCtx_g.gwTbl.treeData) == 1)
      {
        if(TIMER_StartSec(ipMapArpCtx_g.gwArpRqstTimer, ARP_GW_RQSTTIME,
                          TRUE /* repetitive timer */, gwArpRequestTimeExp,
                          (t_Handle)ipMapArpCtx_g.timerExp.taskId) != E_OK)
          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
                  "%s: Start ARP Gateway timer failed\n", routine_name);
      }
    }
  }
  else if (pNode != &gw)
  {
    pNode->refCnt++;                    /* gw already exists, count reference */
    if (staticRouteGw)
      pNode->staticRouteGw = L7_TRUE;
  }
  else  /* pNode == &gw */
  {
    rc = L7_FAILURE;                    /* node not inserted in table */
  }

  return rc;
}

/*********************************************************************
* @purpose  Send an ARP request to each static entry in the ARP gw table.
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
L7_RC_t ipMapArpGwTableStaticRenew(void)
{
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;
  ipMapArpGwNode_t  *pNode;
  ipMapArpIpNode_t  ipNode;
  ipMapArpIntf_t *pIntf = NULL;

  /*
   * Problem:
   * If the GW MAC entries age out in the h/w, the L3 entry doesn't find
   * the next hop for the gateway and hence sends the frame up to the CPU, where
   * the software forwarding of the frame is done after resolving from ARP cache.
   * This will load the CPU if such traffic bombards the CPU before the ARP
   * timeout happens for the gateway entry.
   *
   * Solution:
   * Iterate over all the Gateway entries in the gw table and
   * just send the arp requests for these entries, without
   * any response timers triggered for these entries. ( Response timers
   * will be taken care in the regular ARP age timer exp routines ).
   * This is done to prevent the MAC entries for the gateways from
   * aging out in the h/w L2 table ( As the MAC entries for the gateways
   * get updated through learning of the ARP responses from the gateways )
   */
  if (avlTreeCount(&pArpCtx->gwTbl.treeData) == 0)
  {
    /* if the last gw entry got deleted before the timer expired */
    return L7_SUCCESS;
  }

  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.key = 0;
  ipNode.intIfNum = 0;
  while ((pNode = avlSearchLVL7(&pArpCtx->gwTbl.treeData, (void*) &ipNode.key, AVL_NEXT)) != L7_NULL)
  {
    ipNode.key = pNode->ipAddr;
    ipNode.intIfNum = pNode->intIfNum;

    if(pNode->staticRouteGw == L7_TRUE)
    {
      /* obtain interface instance */
      if (ipMapArpIntfInstanceGet(pNode->intIfNum, &pIntf) == L7_SUCCESS)
      {
        /* send ARP request for this entry */
        ARP_SendARPReqNoRxmt(pArpCtx->arpHandle, (byte *)&pNode->ipAddr, pIntf->index);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose          Gateway ARP Requests timer callback.
*
* @param adrId      @b{(input)}  address entry Id
* @param timer      @b{(input)}  timer handle
* @param flags      @b{(input)}  timer bit flag
*
* @returns          E_OK
* @returns
*
* @notes
*
* @end
**********************************************************************/
static e_Err gwArpRequestTimeExp(t_Handle adrId, t_Handle timer, word flags)
{
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;
  e_Err e;

  IPM_ARP_SEMA_TAKE(&pArpCtx->arpLock, L7_WAIT_FOREVER);

  if (ipMapArpGwTableStaticRenew() == L7_SUCCESS)
    e = E_OK;
  else
    e = E_FAILED;

  IPM_ARP_SEMA_GIVE(&pArpCtx->arpLock);

  return e;
}

/*********************************************************************
* @purpose  Remove an entry from the gateway table
*
* @param    ipAddr      gateway IP address to remove from table
* @param    intIfNum    internal interface number associated with
*                       ARP entry.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Checks if a particular IP address and interface exists in the
*           gateway table.  If so, its route reference count is decremented.
*           When this count reaches zero, the node is removed from
*           the table.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableRemove(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  static const char *routine_name = "ipMapArpGwTableRemove";
  L7_RC_t           rc = L7_SUCCESS;
  ipMapArpGwNode_t  *pNode;
  L7_arpEntry_t     arpEntry;
  L7_BOOL           holdover;
  ipMapArpGwNode_t  gw;

  memset((L7_uchar8 *)&gw, 0, sizeof(gw));
  gw.ipAddr = ipAddr;
  gw.intIfNum = intIfNum;
  pNode = avlSearchLVL7(&ipMapArpCtx_g.gwTbl.treeData, &gw, L7_MATCH_EXACT);

  if (pNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not deleted from table */

  if (pNode->refCnt > 0)                /* precautionary check */
    pNode->refCnt--;

  if (pNode->refCnt == 0)
  {
    ipMapArpIpNode_t  *pIp;

    /* Don't want to call ipMapArpIpTableSearch() while holding the
     * gw table semaphore, so cache some information and delete
     * the gw node so we can give the gw table sem. */
    holdover = pNode->holdover;

    /* Delete the entry from the gateway table. */
    if (avlDeleteEntry(&ipMapArpCtx_g.gwTbl.treeData, pNode) != pNode)
    {
      L7_assert(1);                       /* need to know about this */
      rc = L7_FAILURE;                    /* node not removed from table */
    }

    /* when the last gateway arp entry is deleted, stop the ARP request timer */
    if(avlTreeCount(&ipMapArpCtx_g.gwTbl.treeData) == 0)
    {
      if(TIMER_Stop(ipMapArpCtx_g.gwArpRqstTimer) != E_OK)
         L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_ARP_MAP_COMPONENT_ID,
                 "%s: Stop ARP Gateway timer failed\n", routine_name);
    }

    /*if the gateway addr is contained in the IP lookup table, update its flag*/
    if (ipMapArpIpTableSearch(ipAddr, intIfNum, (L7_uint32)L7_MATCH_EXACT, &pIp) == L7_SUCCESS)
    {
      if (pIp->arpEntry.entryType == ARP_ENTRY_TYPE_GATEWAY)
        pIp->arpEntry.entryType = ARP_ENTRY_TYPE_DYNAMIC;
    }
    else
    {
      /* delete a holdover entry from the device lookup table
       *
       * set up a dummy ARP entry structure for deletion
       */
      if (holdover == L7_TRUE)
      {
        memset(&arpEntry, 0, sizeof(arpEntry));
        arpEntry.ipAddr = ipAddr;
        arpEntry.intIfNum = intIfNum;
        arpEntry.flags |= L7_ARP_GATEWAY;
        if (ipMapArpEntryDelete(&arpEntry) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
                  "%s: Could not delete ARP entry 0x%8.8x\n",
                  routine_name, ipAddr);
        }
      }
    }
    return rc;
  }

  return rc;
}

/*********************************************************************
* @purpose  Search the gateway table for a specific entry
*
* @param    ipAddr      {@b(Input)}  IP address search key
* @param    intIfNum    {@b(Input)}  Internal interface number
* @param    mode        {@b(Input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    **ppGw      {@b(Output)} ptr to GW table node pointer location
*
* @returns  L7_SUCCESS  found gateway IP address of interest
* @returns  L7_FAILURE  gateway IP address not found
*
* @notes    Based on the mode flag, can be used to find a specific gateway
*           IP address, or the next sequential address following the one
*           specified.
*
*           This functions assumes the caller has done a IPM_ARP_SEMA_TAKE
*           on ipMapArpCtx_g.gwTbl.lockCtrl
*
* @end
*********************************************************************/
L7_RC_t ipMapArpGwTableSearch(L7_uint32 ipAddr, L7_uint32 intIfNum,
    L7_uint32 mode, ipMapArpGwNode_t **ppGw)
{
  ipMapArpGwNode_t  *pNode;
  ipMapArpGwNode_t  gw;

  memset((L7_uchar8 *)&gw, 0, sizeof(gw));
  gw.ipAddr = ipAddr;
  gw.intIfNum = intIfNum;
  pNode = avlSearchLVL7(&ipMapArpCtx_g.gwTbl.treeData, &gw,
                        (L7_uint32)((mode == L7_MATCH_EXACT) ? AVL_EXACT : AVL_NEXT));

  if (pNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not found */

  if (ppGw != L7_NULLPTR)
    *ppGw = pNode;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display the current gateway table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ipMapArpGwTableShow(void)
{
  ipMapArpGwNode_t  *pNode;
  L7_uint32         count;
  ipMapArpGwNode_t  gw;

  count = avlTreeCount(&ipMapArpCtx_g.gwTbl.treeData);
  if (count == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nIP MAP ARP Gateway Table is empty\n\n");
    return;
  }

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
              "\nIP MAP ARP Gateway Table (contains %u entries):\n",
              count);
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nStatic gateway renew timer is %s\n\n",
              ipMapArpCtx_g.gwArpRqstTimer ? "Enabled" : "Disabled");

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "Entry#      IP Address    Intf        Ref Cnt   Node Ptr  Hold    Static\n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "------    --------------- -------- ----------   --------  ------  ------\n");

  /* start search with intf=0 and IP addr 0.0.0.0 (never a valid gateway) */
  memset((L7_uchar8 *)&gw, 0, sizeof(gw));
  count = 0;
  while ((pNode = avlSearchLVL7(&ipMapArpCtx_g.gwTbl.treeData, &gw, AVL_NEXT)) != L7_NULL)
  {
    L7_uchar8   ipStr[OSAPI_INET_NTOA_BUF_SIZE];
    /* store key for use in next search */
    gw.ipAddr = pNode->ipAddr;
    gw.intIfNum = pNode->intIfNum;

    osapiInetNtoa(gw.ipAddr, ipStr);
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%6u    %-15s %8d %10d 0x%8.8x %5s %5s\n",
                ++count, ipStr, pNode->intIfNum, pNode->refCnt,
                (L7_uint32)pNode, (pNode->holdover == L7_TRUE) ? "Y" : "N",
                pNode->staticRouteGw ? "Y" : "N");
  }
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n\n");
}



/*********************************************************************
* @purpose  Create the ARP IP lookup table using an AVL tree
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  This table replaces the vendor code ARP Table.  It is used
*           for organizing the IP addresses in an ascending search order
*           to provide proper API operation for the get/next functions.
*           It contains all valid ARP entry types (local, static, gateway,
*           and dynamic).
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableCreate(void)
{
  static const char *routine_name = "ipMapArpIpTableCreate";
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;
  L7_uint32     treeHeapSize, dataNodeSize, dataHeapSize;
  L7_uint32     i;
  ipMapArpIpNode_t *pIp;


  /* calculate the amount of memory needed... */
  treeHeapSize = pArpCtx->ipTbl.treeEntryMax * (L7_uint32)sizeof(avlTreeTables_t);
  dataNodeSize = (L7_uint32)sizeof(ipMapArpIpNode_t);
  dataHeapSize = pArpCtx->ipTbl.treeEntryMax * dataNodeSize;

  /* ...and allocate it from the system heap */
  pArpCtx->ipTbl.treeHeapSize = treeHeapSize;
  pArpCtx->ipTbl.treeHeap = osapiMalloc(L7_ARP_MAP_COMPONENT_ID, treeHeapSize);
  if (pArpCtx->ipTbl.treeHeap == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to allocate ARP IP Table tree heap\n", routine_name);
    return L7_FAILURE;
  }

  pArpCtx->ipTbl.dataHeapSize = dataHeapSize;
  pArpCtx->ipTbl.dataHeap = osapiMalloc(L7_ARP_MAP_COMPONENT_ID, dataHeapSize);
  if (pArpCtx->ipTbl.dataHeap == L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->ipTbl.treeHeap);
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Unable to allocate ARP IP Table data heap\n", routine_name);
    return L7_FAILURE;
  }

  memset(pArpCtx->ipTbl.treeHeap, 0, (size_t)treeHeapSize);
  memset(pArpCtx->ipTbl.dataHeap, 0, (size_t)dataHeapSize);
  memset(&pArpCtx->ipTbl.treeData, 0, sizeof(pArpCtx->ipTbl.treeData));

  avlCreateAvlTree(&pArpCtx->ipTbl.treeData, pArpCtx->ipTbl.treeHeap,
                   pArpCtx->ipTbl.dataHeap,
                   pArpCtx->ipTbl.treeEntryMax, dataNodeSize, 0x10,
                   (L7_uint32)(2*sizeof(L7_uint32)));
  (void)avlSetAvlTreeComparator(&pArpCtx->ipTbl.treeData,ipMapArpIpIntfCompare);

  /* set up all the back pointers within the ARP entry structure */
  for (i = 0; i < pArpCtx->ipTbl.treeEntryMax; i++)
  {
    pIp = &pArpCtx->ipTbl.dataHeap[i];
    pIp->arpEntry.pNode = (void *)pIp;
  }

  ipMapArpIpAgeListInit();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the ARP IP lookup table
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableDelete(void)
{
  static const char *routine_name = "ipMapArpIpTableDelete";
  ipMapArpCtx_t *pArpCtx = &ipMapArpCtx_g;


  /* remove all entries from the AVL tree */
  avlPurgeAvlTree(&pArpCtx->ipTbl.treeData, pArpCtx->ipTbl.treeEntryMax);

  /* NOTE:  The AVL utility currently does not have a deletion function,
   *        so the semaphore needs to be deleted manually here.
   */
  if (pArpCtx->ipTbl.treeData.semId != L7_NULLPTR)
  {
    if (osapiSemaDelete(pArpCtx->ipTbl.treeData.semId) == L7_SUCCESS)
    {
      pArpCtx->ipTbl.treeData.semId = L7_NULLPTR;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Could not delete AVL tree semaphore, id=0x%8.8x\n",
              routine_name, (L7_uint32)pArpCtx->ipTbl.treeData.semId);
      /* keep going */
    }
  }

  /* free the data heap memory */
  if (pArpCtx->ipTbl.dataHeap != L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->ipTbl.dataHeap);
    pArpCtx->ipTbl.dataHeap = L7_NULLPTR;
  }

  /* free the tree heap memory */
  if (pArpCtx->ipTbl.treeHeap != L7_NULLPTR)
  {
    osapiFree(L7_ARP_MAP_COMPONENT_ID, pArpCtx->ipTbl.treeHeap);
    pArpCtx->ipTbl.treeHeap = L7_NULLPTR;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Destroy the ARP IP lookup table
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
L7_RC_t ipMapArpIpTableDestroy(void)
{

#if WPJ_DOUBLE_CHECK_NEW_REL_G_TREES
  if (avlDeleteAvlTree(&ipMapArpIpTreeData) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }

#endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Insert an entry into the ARP IP lookup table
*
* @param    *pIpInfo    {@b(Input)}  pointer to IP node information
* @param    **ppIp      {@b(Output)} pointer to ARP table entry node ptr
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          entry already exists in table
* @returns  L7_TABLE_IS_FULL  ARP IP table has reached capacity
* @returns  L7_FAILURE
*
* @comment  A table full condition is reported by this function, but no
*           action is taken to remove any entries.
*
* @comment  An entry that is already in the table is treated as an error
*           condition.  Expecting the caller to manage the proper insertion
*           and removal of entries so that this does not occur.
*
*           This function does not need to take a read lock on ipTbl since
*           it takes the table change mutex.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableInsert(ipMapArpIpNode_t *pIpInfo, ipMapArpIpNode_t **ppIp)
{
  static const char *routine_name = "ipMapArpIpTableInsert";
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  ipMapArpIpNode_t  *pNode;
  L7_RC_t           rc = L7_FAILURE;
  L7_uint32         ipAddr;
  L7_uint32         tmp;
  L7_uchar8         *tmpip;

  *ppIp = L7_NULLPTR;

  osapiInetNtoa(pIpInfo->key, ipStr);

  /* use one-pass loop for error exit control */
  do
  {
    /* check if AVL entry already exists for this IP address */
    if ((pNode = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, &pIpInfo->key, AVL_EXACT)) != L7_NULLPTR)
    {
      *ppIp = pNode;
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    /* reject insertion of static ARP entry if already at static maximum */
    if (pIpInfo->arpEntry.entryType == ARP_ENTRY_TYPE_STATIC)
    {
      if (ipMapArpCtx_g.staticCurrCt >= ipMapArpCtx_g.staticMaxCt)
      {
        rc = L7_FAILURE;
        break;                          /* exit control loop */
      }
    }

    /* check for table full condition */
    if (ipMapArpIpTableFullCheck() == L7_TRUE)
    {
      rc = L7_TABLE_IS_FULL;
      break;                            /* exit control loop */
    }

    pNode = avlInsertEntry(&ipMapArpCtx_g.ipTbl.treeData, pIpInfo);

    if (pNode == pIpInfo)
    {
      /* entry add failed */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure adding %s intf %d to ARP table\n",
              routine_name, (char *)ipStr, pIpInfo->intIfNum);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not inserted in table */
      break;                            /* exit control loop */
    }

    else if (pNode != L7_NULLPTR)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pIpInfo->intIfNum, L7_SYSNAME, ifName);

      /* entry exists in table -- was already checked above so something's wrong*/
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Entry for %s intf %s already exists in ARP table\n",
              routine_name, (char *)ipStr, ifName);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not inserted in table */
      break;                            /* exit control loop */
    }

    /* new AVL entry has been added (NOTE: pNode == L7_NULLPTR here)
     *  - search for the new entry to obtain its pointer
     */
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, &pIpInfo->key, AVL_EXACT);
    if (pNode == L7_NULLPTR)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pIpInfo->intIfNum, L7_SYSNAME, ifName);

      /* cannot find new entry */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Cannot find new ARP table entry for %s intf %s\n",
              routine_name, (char *)ipStr, ifName);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not inserted in table */
      break;                            /* exit control loop */
    }

    /* re-establish node back pointer */
    pNode->arpEntry.pNode = (void *)pNode;

    /* initialize node fields */
    pNode->inDevice = L7_FALSE;
    pNode->failedDevAdds = 0;

    /* check if a dynamic ARP entry is actually a gateway address */
    if (pNode->arpEntry.entryType == ARP_ENTRY_TYPE_DYNAMIC)
    {
      /* PTIn Modified */
      /* pGw is not used outside this scope */
      ipMapArpGwNode_t  *pGw;
      tmpip = pNode->arpEntry.ipAddr;
      tmp = (tmpip[0] << 24) | (tmpip[1] << 16) | (tmpip[2] << 8) | (tmpip[3]);
      //ipAddr = (L7_uint32)osapiNtohl(tmp);
      ipAddr = tmp;

      if (ipMapArpGwTableSearch(ipAddr, pIpInfo->intIfNum, (L7_uint32)L7_MATCH_EXACT, &pGw) == L7_SUCCESS)
        pNode->arpEntry.entryType = ARP_ENTRY_TYPE_GATEWAY;
    }

    if (ipMapArpIpAgeListAdd(pNode) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pIpInfo->intIfNum, L7_SYSNAME, ifName);

      (void)avlDeleteEntry(&ipMapArpCtx_g.ipTbl.treeData, pNode);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure to add %s intf %s to ARP age list\n",
              routine_name, (char *)ipStr, ifName);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not inserted in table */
      break;                            /* exit control loop */
    }

    /* maintain ARP cache counters */
    ipMapArpCtx_g.cacheCurrCt++;
    if (ipMapArpCtx_g.cacheCurrCt > ipMapArpCtx_g.cachePeakCt)
      ipMapArpCtx_g.cachePeakCt = ipMapArpCtx_g.cacheCurrCt;

    if (pNode->arpEntry.entryType == ARP_ENTRY_TYPE_STATIC)
      ipMapArpCtx_g.staticCurrCt++;

    *ppIp = pNode;

    if (ipMapArpDeviceTableApply(pNode, IPM_ARP_TABLE_INSERT) != L7_SUCCESS)
    {
      /* just log a message, but do not change outcome of table operation */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure adding %s in device ARP table\n",
              routine_name, (char *)ipStr);
    }

    /* debug message */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_LO,
                "[%10.10u] IPM_ARP: inserted %s intf %d, pNode=0x%8.8x t_Age=0x%8.8x t_Rsp=0x%8.8x  (inDev=%1s)\n",
                osapiTimeMillisecondsGet(), (L7_uchar8 *)ipStr,
                pIpInfo->intIfNum, (L7_uint32)pNode,
                (L7_uint32)pNode->arpEntry.arpAgeTimer,
                (L7_uint32)pNode->arpEntry.arpRspTimer,
                (pNode->inDevice == L7_TRUE) ? "Y" : "N");

    rc = L7_SUCCESS;                    /* all went well */

  } while (0);  /* enddowhile one-pass control loop */

  return rc;
}

/*********************************************************************
* @purpose  Update an entry in the ARP IP lookup table
*
* @param    *pIpInfo    {@b(Input)}  pointer to IP node information
* @param    event       {@b(Input)}  indicates update vs. reissue event
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    entry not found in table
* @returns  L7_FAILURE
*
* @comment  An entry that is not in the table is treated as an error
*           condition.  Expecting the caller to manage the proper insertion
*           and removal of entries so that this does not occur.
*
*           This function does not need to take a read lock on ipTbl since
*           it takes the table change mutex.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableUpdate(ipMapArpIpNode_t *pIpInfo,
                              ipMapArpTableEvent_t event)
{
  static const char *routine_name = "ipMapArpIpTableUpdate";
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  ipMapArpIpNode_t  *pNode;
  L7_RC_t           rc = L7_FAILURE;

  if ((event != IPM_ARP_TABLE_REISSUE) && (event != IPM_ARP_TABLE_UPDATE))
    return L7_FAILURE;

  osapiInetNtoa(pIpInfo->key, ipStr);

  /* use one-pass loop for error exit control */
  do
  {
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, &pIpInfo->key, L7_MATCH_EXACT);

    if (pNode == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    if (pNode->key != pIpInfo->key || pNode->intIfNum != pIpInfo->intIfNum)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pIpInfo->intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Wrong table entry obtained for addr %s intf %s\n",
              routine_name, (char *)ipStr, ifName);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not deleted from table */
      break;                            /* exit control loop */
    }

    /* NOTE:  There is no need to actually update any fields in the ARP
     *        entry here, since that was done elsewhere prior to this
     *        function being called.  The main thing to do is apply the
     *        change to the device.
     */

    if (ipMapArpDeviceTableApply(pNode, event) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pIpInfo->intIfNum, L7_SYSNAME, ifName);

      /* just log a message, but do not change outcome of table operation */
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure updating %s intf %s in device ARP table\n",
              routine_name, (char *)ipStr, ifName);
    }

    /* debug message */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_LO,
                "[%10.10u] IPM_ARP: %s %s %d, pNode=0x%8.8x t_Age=0x%8.8x t_Rsp=0x%8.8x  (inDev=%1s)\n",
                osapiTimeMillisecondsGet(), (event == IPM_ARP_TABLE_UPDATE) ? "updated" : "reissued",
                (L7_uchar8 *)ipStr, pIpInfo->intIfNum, (L7_uint32)pNode,
                (L7_uint32)pNode->arpEntry.arpAgeTimer,
                (L7_uint32)pNode->arpEntry.arpRspTimer,
                (pNode->inDevice == L7_TRUE) ? "Y" : "N");

    rc = L7_SUCCESS;                    /* all went well */

  } while (0);  /* enddowhile one-pass control loop */

  return rc;
}

/*********************************************************************
* @purpose  Remove an entry from the ARP IP lookup table
*
* @param    *pIpInfo    {@b(Input)} pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    entry not found in table
* @returns  L7_FAILURE
*
* @comment  An entry that is not in the table is treated as an error
*           condition.  Expecting the caller to manage the proper insertion
*           and removal of entries so that this does not occur.
*
* @comment  The caller is advised to make a copy of whatever data it needs
*           from the IP node entry prior to calling this function.  Once
*           the node is deleted from the ARP table, its contents are
*           no longer reliable.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableRemove(ipMapArpIpNode_t *pIpInfo)
{
  static const char *routine_name = "ipMapArpIpTableRemove";
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  ipMapArpIpNode_t  *pNode;
  L7_RC_t           rc = L7_FAILURE;
  L7_BOOL           inDeviceSaved;      /* save for debug msg */
  L7_uint32         ageTimerId, rspTimerId;


  osapiInetNtoa(pIpInfo->key, ipStr);

  /* use one-pass loop for error exit control */
  do
  {
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, &pIpInfo->key, L7_MATCH_EXACT);

    if (pNode == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    if (pNode->key != pIpInfo->key || pNode->intIfNum != pIpInfo->intIfNum)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Wrong table entry obtained for addr %s\n",
              routine_name, (char *)ipStr);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not deleted from table */
      break;                            /* exit control loop */
    }

    /* as a precaution, only delete an AVL entry marked local if the
     * incoming IP address being deleted is also marked as local
     */
    if ((pNode->arpEntry.entryType == ARP_ENTRY_TYPE_LOCAL) &&
        (pIpInfo->arpEntry.entryType != ARP_ENTRY_TYPE_LOCAL))
    {
      rc = L7_FAILURE;
      break;                            /* exit control loop */
    }

    /* save value of inDevice flag for debug msg before it is changed/deleted */
    inDeviceSaved = pNode->inDevice;
    ageTimerId = (L7_uint32)pNode->arpEntry.arpAgeTimer;
    rspTimerId = (L7_uint32)pNode->arpEntry.arpRspTimer;

    /* delete the entry from the ARP table */
    if (ipMapArpIpTableRemoveCommon(pNode) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure to remove %s intf %s from ARP table\n",
              routine_name, (char *)ipStr, ifName);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not removed from table */
      break;                            /* exit control loop */
    }

    /* debug message */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_LO,
                "[%10.10u] IPM_ARP: removed %s, intf %d pNode=0x%8.8x t_Age=0x%8.8x t_Rsp=0x%8.8x  (inDev=%1s)\n",
                osapiTimeMillisecondsGet(), ipStr, pNode->intIfNum,
                (L7_uint32)pNode, ageTimerId, rspTimerId,
                (inDeviceSaved == L7_TRUE) ? "Y" : "N");

    rc = L7_SUCCESS;                    /* all went well */

  } while (0);  /* enddowhile one-pass control loop */

  return rc;
}

/*********************************************************************
* @purpose  Remove the oldest entry from the ARP table
*
* @param    allowGw     {@b(Input))  gateway entry types can be removed
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no removable entry was found
* @returns  L7_FAILURE
*
* @comment  Normally, only a dynamic entry type is considered for removal.
*           If the allowGw parm is set to L7_TRUE, then gateway entries
*           will also be considered after all dynamic entries are gone.
*
* @comment  An empty list is considered an error, since this function is
*           typically called when the list is full, or when there are known
*           entries that need to be removed.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableOldestRemove(L7_BOOL allowGw)
{
  static const char *routine_name = "ipMapArpIpTableOldestRemove";
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  ipMapArpIpNode_t  *pNode = L7_NULLPTR;
  L7_RC_t           rc = L7_FAILURE, rc2;
  L7_BOOL           inDeviceSaved;      /* save for debug msg */
  L7_uint32         ageTimerId, rspTimerId;


  /* use one-pass loop for error exit control */
  do
  {
    /* find oldest entry in ARP table */
    if ((rc2 = ipMapArpIpTableOldestFindCommon(allowGw, &pNode)) != L7_SUCCESS)
    {
      rc = rc2;                         /* only update orginal rc for non-success */
      break;                            /* exit control loop */
    }

    /* indicate error if no entry found */
    if (pNode == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    /* NOTE: pNode points to oldest dynamic (or gateway) entry to be removed */

    osapiInetNtoa(pNode->key, ipStr);

    /* save value of inDevice flag for debug msg before it is changed/deleted */
    inDeviceSaved = pNode->inDevice;
    ageTimerId = (L7_uint32)pNode->arpEntry.arpAgeTimer;
    rspTimerId = (L7_uint32)pNode->arpEntry.arpRspTimer;

    /* delete the entry from the ARP table */
    if (ipMapArpIpTableRemoveCommon(pNode) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure to remove %s from ARP table\n",
              routine_name, (char *)ipStr);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not removed from table */
      break;                            /* exit control loop */
    }

    /* debug message */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_LO,
                "[%10.10u] IPM_ARP: oldest removed %s intf %d, pNode=0x%8.8x t_Age=0x%8.8x t_Rsp=0x%8.8x  (inDev=%1s)\n",
                osapiTimeMillisecondsGet(), ipStr, pNode->intIfNum,
                (L7_uint32)pNode, ageTimerId, rspTimerId,
                (inDeviceSaved == L7_TRUE) ? "Y" : "N");

    rc = L7_SUCCESS;                    /* all went well */

  } while (0);  /* enddowhile one-pass control loop */

  return rc;
}

/*********************************************************************
* @purpose  Common subroutine to remove ARP IP Table entry from AVL tree
*
* @param    *pIp        {@b(Input)} pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes caller has obtained the ipTbl.treeData semaphore.
*
*           This function does not need to take a read lock on ipTbl since
*           the caller takes the table change mutex.
*
* @end
*********************************************************************/
static L7_RC_t ipMapArpIpTableRemoveCommon(ipMapArpIpNode_t *pNode)
{
  static const char *routine_name = "ipMapArpIpTableRemoveCommon";
  L7_uint32     savedEntryType;
  L7_uchar8     ipStr[OSAPI_INET_NTOA_BUF_SIZE];

  /* need to save the entry type to check it after the AVL entry is deleted */
  savedEntryType = (L7_uint32)pNode->arpEntry.entryType;

  osapiInetNtoa(pNode->key, ipStr);

  /* stop all timers associated with this ARP entry
   *
   * NOTE:  Must be done before entry removed from ARP IP table.
   *
   * NOTE:  This is usually a re-entry path into the vendor ARP code,
   *        meaning the ARP lock was previously acquired.  Rely on the
   *        vendor ARP locking logic to permit re-entry via the same
   *        execution thread.
   */
  if (ARP_StopEntryTimers((t_ARP *)ipMapArpCtx_g.arpHandle,
                          (t_Handle)&pNode->arpEntry) != E_OK)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);

    /* just log a message, but do not change outcome of table operation */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Failure stopping ARP entry timers for %s intf %s\n",
            routine_name, (char *)ipStr, ifName);
  }

  /* NOTE: Must issue this before deleting entry from IP ARP table */
  if (ipMapArpDeviceTableApply(pNode, IPM_ARP_TABLE_REMOVE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);

    /* just log a message, but do not change outcome of table operation */
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: Failure removing %s intf %s in device ARP table\n",
            routine_name, (char *)ipStr, ifName);
  }

  /* delete the entry from the age list */
  if (ipMapArpIpAgeListRemove(pNode) != L7_SUCCESS)
  {
    L7_assert(1);                       /* need to know about this */
    return L7_FAILURE;                  /* node not removed from table */
  }

  ARP_DestroyAddrEntry(&pNode->arpEntry);

  /* delete the node from the AVL tree */
  if (avlDeleteEntry(&ipMapArpCtx_g.ipTbl.treeData, pNode) != pNode)
  {
    L7_assert(1);                       /* need to know about this */
    return L7_FAILURE;                  /* node not removed from table */
  }

  /* maintain ARP cache counters */
  ipMapArpCtx_g.cacheCurrCt--;
  if (savedEntryType == ARP_ENTRY_TYPE_STATIC)
    ipMapArpCtx_g.staticCurrCt--;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find the oldest entry currently in the ARP table
*
* @param    allowGw     {@b(Input))  gateway entry types can be removed
* @param    **ppIp      {@b(Output)} pointer to ARP table entry node ptr
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no removable entry was found
* @returns  L7_FAILURE
*
* @comment  Normally, only a dynamic entry type is considered for removal.
*           If the allowGw parm is set to L7_TRUE, then gateway entries
*           will also be considered after all dynamic entries are gone.
*
* @comment  An empty list is considered an error, since this function is
*           typically called when the list is full, or when there are known
*           entries that need to be removed.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableOldestFind(L7_BOOL allowGw, ipMapArpIpNode_t **ppIp)
{
  L7_RC_t       rc = L7_FAILURE;


  rc = ipMapArpIpTableOldestFindCommon(allowGw, ppIp);

  return rc;
}

/*********************************************************************
* @purpose  Common subroutine to find the oldest entry currently in the
*           ARP table
*
* @param    allowGw     {@b(Input))  gateway entry types can be removed
* @param    **ppIp      {@b(Output)} pointer to ARP table entry node ptr
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no removable entry was found
* @returns  L7_FAILURE
*
* @comment  Assumes caller has obtained the ipTbl.treeData semaphore.
*
* @comment  Normally, only a dynamic entry type is considered for removal.
*           If the allowGw parm is set to L7_TRUE, then gateway entries
*           will also be considered after all dynamic entries are gone.
*
* @comment  An empty list is considered an error, since this function is
*           typically called when the list is full, or when there are known
*           entries that need to be removed.
*
* @end
*********************************************************************/
static L7_RC_t ipMapArpIpTableOldestFindCommon(L7_BOOL allowGw,
                                               ipMapArpIpNode_t **ppIp)
{
  ipMapArpAgeList_t *pAgeList = &ipMapArpAgeList_g;
  ipMapArpIpNode_t  *pNode = L7_NULLPTR;
  L7_RC_t           rc = L7_FAILURE;


  /*--------------------------------------------------------------------------
   * This function makes the following assumptions about the age list:
   *   a. empty:  return with error
   *   b. allowGw=False, no dynamics found:  return with error
   *   c. allowGw=True, no dynamics or gateways found:  return with error
   *   d. allowGw=True, no dynamics found, gateway found:  output oldest gateway entry
   *   e. otherwise:  output oldest dynamic entry
   *--------------------------------------------------------------------------
   */

  /* use one-pass loop for error exit control */
  do
  {
    /* case (a):  check for empty list */
    if (pAgeList->head == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    /* first pass:
     *  - look for first dynamic entry in list (it is least-recently refreshed)
     */
    for (pNode = pAgeList->head; pNode != L7_NULLPTR; pNode = pNode->ageCtrl.next)
    {
      if (pNode->arpEntry.entryType == ARP_ENTRY_TYPE_DYNAMIC)
        break;                          /* found one */
    }

    /* check if could not find a dynamic entry to remove
     *  - case (b):  allowGw is false, return with error (below)
     *  - case (c,d):  allowGw is true, make second pass looking for gateway entries
     */
    if ((pNode == L7_NULLPTR) && (allowGw == L7_TRUE))
    {
      /* second pass:
       *  - look for first gateway entry in list (it is least-recently refreshed)
       */
      for (pNode = pAgeList->head; pNode != L7_NULLPTR; pNode = pNode->ageCtrl.next)
      {
        if (pNode->arpEntry.entryType == ARP_ENTRY_TYPE_GATEWAY)
          break;                        /* found one */
      }
    }

    /* fail if no entry found during preceding search(es) */
    if (pNode == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    /* NOTE: pNode points to oldest dynamic (or gateway) entry to be removed */

    rc = L7_SUCCESS;                    /* all went well */

  } while (0);  /* enddowhile one-pass control loop */

  *ppIp = pNode;

  return rc;
}

/*********************************************************************
* @purpose  Search the ARP IP lookup table
*
* @param    ipAddr      {@b(Input)}  IP address search key
* @param    intIfNum    {@b(Input)}  Internal Interface number for the entry
* @param    mode        {@b(Input)}  search mode (L7_MATCH_EXACT, L7_MATCH_GETNEXT)
* @param    **ppIp      {@b(Output)} ptr to IP lookup table node output location
*
* @returns  L7_SUCCESS  found IP entry of interest
* @returns  L7_FAILURE  IP entry not found
*
* @comment  Based on the mode flag, can be used to find a specific IP
*           entry, or the next sequential IP address following the one
*           specified.
*
*           This functions assumes the caller has done a IPM_ARP_SEMA_TAKE
*           on ipMapArpCtx_g.ipTbl.lockCtrl
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableSearch(L7_uint32 ipAddr, L7_uint32 intIfNum,
    L7_uint32 mode, ipMapArpIpNode_t **ppIp)
{
  ipMapArpIpNode_t  *pNode;
  ipMapArpIpNode_t  ipNode;


  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.key = ipAddr;
  ipNode.intIfNum = intIfNum;

  pNode = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, (void *)&ipNode.key,
                        (L7_uint32)((mode == L7_MATCH_EXACT) ? AVL_EXACT : AVL_NEXT));

  if (pNode == L7_NULLPTR)
    return L7_FAILURE;                  /* node not found */

  if (ppIp != L7_NULLPTR)
    *ppIp = pNode;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Change the size of the ARP IP table
*
* @param    maxEnt      {@b(Input)}  maximum number of ARP entries allowed
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Only changes the logical (functional) size of the ARP table, but
*           does not change the resource allocation of the underlying AVL tree.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableResize(L7_uint32 maxEnt)
{

  /* adjust ARP cache max and peak counts
   *
   * NOTE:  Decided it is best to reset the peak count whenever the
   *        max is changed (either up or down).
   */
  ipMapArpCtx_g.cacheMaxCt = maxEnt;
  ipMapArpCtx_g.cachePeakCt = ipMapArpCtx_g.cacheCurrCt;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if the ARP table has reached its maximum capacity
*
* @param    void
*
* @returns  L7_TRUE     table is full
* @returns  L7_FALSE    table not full
*
* @comment  Caller is responsible for any semaphore protection, if desired.
*
* @comment  This function also checks for table overflow error conditions.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpIpTableFullCheck(void)
{
  static const char *routine_name = "ipMapArpIpTableFullCheck";


  /* sanity check for table overflows (this is a serious error condition) */
  if (ipMapArpCtx_g.cacheCurrCt > ipMapArpCtx_g.cacheMaxCt)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: ARP table has overflowed\n", routine_name);
    L7_assert(1);                       /* need to know about this */
    /* continue processing */
  }
  if (ipMapArpCtx_g.staticCurrCt > ipMapArpCtx_g.staticMaxCt)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: ARP table static entries have overflowed\n", routine_name);
    L7_assert(1);                       /* need to know about this */
    /* continue processing */
  }

  /* here is the actual check for a table full condition */
  if (ipMapArpCtx_g.cacheCurrCt >= ipMapArpCtx_g.cacheMaxCt)
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check if maximum number of static entries are in ARP table
*
* @param    void
*
* @returns  L7_TRUE     maximum static entries exist
* @returns  L7_FALSE    still room for more static entries
*
* @comment  Caller is responsible for any semaphore protection, if desired.
*
* @comment  This function also checks for table overflow error conditions.
*
* @end
*********************************************************************/
L7_BOOL ipMapArpIpTableStaticMaxCheck(void)
{
  static const char *routine_name = "ipMapArpIpTableStaticMaxCheck";


  /* sanity check for table overflows (this is a serious error condition) */
  if (ipMapArpCtx_g.staticCurrCt > ipMapArpCtx_g.staticMaxCt)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
            "%s: ARP table static entries have overflowed\n", routine_name);
    L7_assert(1);                       /* need to know about this */
    /* continue processing */
  }

  /* here is the actual check for max static condition */
  if (ipMapArpCtx_g.staticCurrCt >= ipMapArpCtx_g.staticMaxCt)
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Handle conversion of existing entry to static type
*
* @param    *pIpInfo    {@b(Input)} pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    already a permanent entry
* @returns  L7_FAILURE
*
* @comment  The entry should not already be a permanent (i.e., static)
*           entry.  This is only intended to handle the special case
*           where an existing dynamic/gateway entry is being changed
*           to a static entry so that a delete/add is not needed (avoids
*           potential for routes to become temporarily unresolved).
*
*           This function assumes the caller has taken the ipTbl sema
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableEntryStaticConvert(ipMapArpIpNode_t *pIpInfo)
{
  static const char *routine_name = "ipMapArpIpTableEntryStaticConvert";
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_RC_t           rc = L7_FAILURE;


  osapiInetNtoa(pIpInfo->key, ipStr);

  /* use one-pass loop for error exit control */
  do
  {
    /* as a precaution, only delete an AVL entry marked local if the
     * incoming IP address being deleted is also marked as local
     */
    if (pIpInfo->arpEntry.permanent == L7_TRUE)
    {
      rc = L7_ERROR;
      break;                            /* exit control loop */
    }

    /* double check if currently at static entry maximum */
    if (ipMapArpIpTableStaticMaxCheck() != L7_FALSE)
    {
      rc = L7_FAILURE;
      break;                            /* exit control loop */
    }

    /* delete the entry from the ARP age list */
    if (ipMapArpIpAgeListRemove(pIpInfo) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_ARP_MAP_COMPONENT_ID,
              "%s: Failure to remove %s from ARP age list\n",
              routine_name, (char *)ipStr);
      L7_assert(1);                     /* need to know about this */
      rc = L7_FAILURE;                  /* node not removed from table */
      break;                            /* exit control loop */
    }

    /* increment the static current count */
    ipMapArpCtx_g.staticCurrCt++;

    /* debug message */
    IPM_ARP_PRT(IPM_ARP_MSGLVL_LO, "IPM_ARP: converted %s to static\n", ipStr);

    rc = L7_SUCCESS;                    /* all went well */

  } while (0);  /* enddowhile one-pass control loop */

  return rc;
}

/*********************************************************************
* @purpose  Retrieve various ARP cache statistics
*
* @param    *pStats     pointer to output location for storing ARP cache stats
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  none
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpTableStatsGet(L7_arpCacheStats_t *pStats)
{

  if (pStats == L7_NULLPTR)
    return L7_FAILURE;

  pStats->cacheCurrent = ipMapArpCtx_g.cacheCurrCt;
  pStats->cachePeak = ipMapArpCtx_g.cachePeakCt;
  pStats->cacheMax = ipMapArpCtx_g.cacheMaxCt;

  pStats->staticCurrent = ipMapArpCtx_g.staticCurrCt;
  pStats->staticMax = ipMapArpCtx_g.staticMaxCt;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display the current ARP IP lookup table contents
*
* @param    intIfNum    internal interface number, or 0 for all interfaces
* @param    numEnt      number of entries to display (0 means all)
*
* @returns  void
*
* @comment
*
* @end
*********************************************************************/
void ipMapArpIpTableShow(L7_uint32 intIfNum, L7_uint32 numEnt)
{
  ipMapArpIpNode_t  *pNode;
  ipMapArpIpNode_t  ipNode;
  L7_uint32         count, entryType;
  ulng              ageTime;
  ipMapArpIntf_t    *pIntf;
  L7_uint32         i;
  L7_uchar8         ipStr[OSAPI_INET_NTOA_BUF_SIZE];


  count = avlTreeCount(&ipMapArpCtx_g.ipTbl.treeData);
  if (count == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nIP MAP ARP IP Table is empty\n\n");
    return;
  }

  if ((numEnt == 0) || (numEnt > count))
    numEnt = count;

  if (intIfNum == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                "\nDisplaying ARP Table contents (%u entries exist):\n\n",
                count);
  }
  else if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                "\n\nInvalid interface %u (must be for a router intf or 0)\n\n",
                intIfNum);
    return;
  }
  else
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                "\n\nDisplaying ARP Table entries for interface %u\n\n",
                intIfNum);
  }

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "                                                                        Fail            \n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "Entry#     IP Address    Intf      MAC Addr     Intf   Type     Age    InDev Count  Node Ptr \n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "------   --------------- ---- ----------------- ---- -------- -------- ----- ----- ----------\n");

  count = 0;
  /* start search with IP addr 0.0.0.0 (to find first one) */
  ipNode.key = 0;
  ipNode.intIfNum = 0;

  while (numEnt > 0)
  {
    pNode = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, &ipNode, AVL_NEXT);
    if (pNode == L7_NULLPTR)
      break;                            /* done */

    /* use to start next search */
    ipNode.key = pNode->key;
    ipNode.intIfNum = pNode->intIfNum;

    pIntf = ipMapArpIntfInUse_g[(L7_uint32)pNode->arpEntry.intfNum];
    if (pIntf == L7_NULLPTR)
      pIntf = ipMapArpIntfInUse_g[0];

    if ((intIfNum == 0) || (intIfNum == pIntf->intIfNum))
    {
      osapiInetNtoa(ipNode.key, ipStr);

      entryType = (L7_uint32)pNode->arpEntry.entryType;

      if (pNode->arpEntry.timeStamp != 0)
        ipMapArpExtenEntryAgeCalc(pNode->arpEntry.timeStamp, (ulng *)&ageTime);
      else
        ageTime = 0;

      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%6u   %-15.15s %5d", ++count, ipStr, pNode->intIfNum);

      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%2.2x:", pNode->arpEntry.macAddr[i]);
      }
      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%2.2x ", pNode->arpEntry.macAddr[i]);

      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, " %2.1u  %-8.8s %8u   %1s    %2u   0x%8.8x\n",
                  pIntf->intIfNum, ipMapArpEntryTypeStr[entryType], ageTime,
                  (pNode->inDevice == L7_TRUE) ? "Y" : "N", pNode->failedDevAdds,
                  (L7_uint32)pNode->arpEntry.pNode);

      numEnt--;
    }

  } /* endwhile */

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n\n");
}



/*********************************************************************
* @purpose  Initialize the ARP age list
*
* @param    void
*
* @returns  void
*
* @comment
*
* @end
*********************************************************************/
void ipMapArpIpAgeListInit(void)
{
  ipMapArpAgeList_t *pAgeList = &ipMapArpAgeList_g;


  pAgeList->head = pAgeList->tail = L7_NULLPTR;
}

/*********************************************************************
* @purpose  Add an ARP entry node to the ARP age list
*
* @param    *pIp        pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes caller has obtained the ipTbl.treeData semaphore.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpAgeListAdd(ipMapArpIpNode_t *pIp)
{
  ipMapArpAgeList_t *pAgeList = &ipMapArpAgeList_g;


  if (pIp == L7_NULLPTR)
    return L7_FAILURE;

  pIp->ageCtrl.prev = pIp->ageCtrl.next = L7_NULLPTR;

  /* only gateway and dynamic entry types are included in the age list */
  if (pIp->arpEntry.permanent == L7_TRUE)
    return L7_SUCCESS;

  /* empty list */
  if (pAgeList->head == L7_NULLPTR)
  {
    if (pAgeList->tail != L7_NULLPTR)
      return L7_FAILURE;

    pAgeList->head = pAgeList->tail = pIp;
  }

  /* non-empty list */
  else
  {
    if (pAgeList->tail == L7_NULLPTR)
      return L7_FAILURE;

    pIp->ageCtrl.prev = pAgeList->tail;
    (pAgeList->tail)->ageCtrl.next = pIp;
    pAgeList->tail = pIp;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove an ARP entry node from the ARP age list
*
* @param    *pIp        pointer to IP node information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes caller has obtained the ipTbl.treeData semaphore.
*
* @comment  It is possible for a former dynamic entry to be replaced with
*           a static, so this function cannot assume only dynamic and
*           gateway entries are on the age list.  Such a static entry is
*           removed from the age list here.
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpAgeListRemove(ipMapArpIpNode_t *pIp)
{
  ipMapArpAgeList_t *pAgeList = &ipMapArpAgeList_g;


  if (pIp == L7_NULLPTR)
    return L7_FAILURE;

  /* only gateway and dynamic entry types are included in the age list */
  if (pIp->arpEntry.permanent == L7_TRUE)
    return L7_SUCCESS;

  if ((pAgeList->head == L7_NULLPTR) || (pAgeList->tail == L7_NULLPTR))
    return L7_FAILURE;

  /* remove element from the chain, updating the list anchor head/tail ptrs
   * as needed
   */
  if (pIp->ageCtrl.prev == L7_NULLPTR)
  {
    /* element is head of chain */
    if (pAgeList->head != pIp)
      return L7_FAILURE;
    pAgeList->head = pIp->ageCtrl.next;
  }
  else
  {
    /* element not head of chain */
    if ((pIp->ageCtrl.prev)->ageCtrl.next != pIp)
      return L7_FAILURE;
    (pIp->ageCtrl.prev)->ageCtrl.next = pIp->ageCtrl.next;
  }

  if (pIp->ageCtrl.next == L7_NULLPTR)
  {
    /* element is tail of chain */
    if (pAgeList->tail != pIp)
      return L7_FAILURE;
    pAgeList->tail = pIp->ageCtrl.prev;
  }
  else
  {
    /* element not tail of chain */
    if ((pIp->ageCtrl.next)->ageCtrl.prev != pIp)
      return L7_FAILURE;
    (pIp->ageCtrl.next)->ageCtrl.prev = pIp->ageCtrl.prev;
  }

  pIp->ageCtrl.prev = pIp->ageCtrl.next = L7_NULLPTR;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Refresh an ARP entry node in the ARP age list
*
* @param    ipAddr      IP address of ARP entry to refresh in age list
* @param    intIfNum    Internal Interface number for the entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comment  Assumes entry node already exists in the ARP age list, so
*           it needs to be moved to the end of the list (i.e., becomes
*           the newest entry).  By definition, the entry is type dynamic
*           or gateway (i.e., non-permanent).
*
* @end
*********************************************************************/
L7_RC_t ipMapArpIpAgeListRefresh(L7_uint32 ipAddr, L7_uint32 intIfNum)
{
  L7_RC_t           rc = L7_FAILURE;
  ipMapArpIpNode_t  *pIp;
  ipMapArpIpNode_t  ipNode;



  memset(&ipNode, 0, sizeof(ipNode));
  ipNode.key = ipAddr;
  ipNode.intIfNum = intIfNum;
  /* NOTE:  Do not call ipMapArpIpTableSearch here, since it too takes the
   *        same semaphore, resulting in a deadlock.
   */
  pIp = avlSearchLVL7(&ipMapArpCtx_g.ipTbl.treeData, (void *)&ipNode.key, AVL_EXACT);

  if (pIp != L7_NULLPTR)
  {
    rc = ipMapArpIpAgeListRemove(pIp);
    if (rc == L7_SUCCESS)
    {
      rc = ipMapArpIpAgeListAdd(pIp);
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Display the contents of the ARP age list
*
* @param    intIfNum    {@b(input)} internal interface number, or 0 for all interfaces
* @param    numEnt      {@b(input)} number of entries to display (0 means all)
*
* @returns  void
*
* @comment
*
* @end
*********************************************************************/
void ipMapArpIpAgeListShow(L7_uint32 intIfNum, L7_uint32 numEnt)
{
  ipMapArpAgeList_t *pAgeList = &ipMapArpAgeList_g;
  ipMapArpIpNode_t  *pNode;
  L7_uint32         count, ipAddr, entryType;
  ulng              ageTime;
  ipMapArpIntf_t    *pIntf;


  /* display the head and tail pointers */
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nAge List head = 0x%8.8x  tail = 0x%8.8x\n",
              (L7_uint32)pAgeList->head, (L7_uint32)pAgeList->tail);

  /* quick-scan the list to count the entries */
  for (pNode = pAgeList->head, count = 0;
       pNode != L7_NULLPTR;
       pNode = pNode->ageCtrl.next, count++);

  if (count == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\nIP MAP ARP age list is empty\n\n");
    return;
  }

  if (intIfNum == 0)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                "\nDisplaying entire ARP IP age list (contains %u entries):\n\n",
                count);
  }
  else if (ipMapArpIntfInstanceGet(intIfNum, &pIntf) != L7_SUCCESS)
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                "\n\nInvalid interface %u (must be for a router intf or 0)\n\n",
                intIfNum);
    return;
  }
  else
  {
    IPM_ARP_PRT(IPM_ARP_MSGLVL_ON,
                "\n\nDisplaying ARP IP age list entries for interface %u\n\n",
                intIfNum);
  }

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "Entry#     IP Address      Type   Intf   Age     Node Ptr \n");
  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "------   --------------- -------- ---- -------- ----------\n");

  if ((numEnt == 0) || (numEnt > count))
    numEnt = count;

  count = 0;
  pNode = pAgeList->head;
  while (numEnt > 0)
  {
    L7_uchar8   ipStr[OSAPI_INET_NTOA_BUF_SIZE];

    /* node ptr should not be null here */
    if (pNode == L7_NULLPTR)
      break;

    ipAddr = pNode->key;                /* use to start next search */

    pIntf = ipMapArpIntfInUse_g[(L7_uint32)pNode->arpEntry.intfNum];
    if (pIntf == L7_NULLPTR)
      pIntf = ipMapArpIntfInUse_g[0];

    if ((intIfNum == 0) || (intIfNum == pIntf->intIfNum))
    {
      osapiInetNtoa(ipAddr, ipStr);
      entryType = (L7_uint32)pNode->arpEntry.entryType;
      ipMapArpExtenEntryAgeCalc(pNode->arpEntry.timeStamp, (ulng *)&ageTime);

      IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "%6u   %-15.15s %-8.8s  %2.1u  %8u 0x%8.8x\n",
                  ++count, ipStr, ipMapArpEntryTypeStr[entryType],
                  pIntf->intIfNum, ageTime, (L7_uint32)pNode);

      numEnt--;
    }

    pNode = pNode->ageCtrl.next;

  } /* endwhile */

  IPM_ARP_PRT(IPM_ARP_MSGLVL_ON, "\n\n");
}


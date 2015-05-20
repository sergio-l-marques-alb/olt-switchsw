/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_db.c
*
* @purpose   dot1x data store file
*
* @component dot1x
*
* @comments  none
*
* @create    10/11/2007
*
* @author    PKB
*
* @end
*             
**********************************************************************/

#include "dot1x_include.h"
#include "avl_api.h"


/* Global parameters */
static avlTree_t                dot1xLogicalPortTreeDb;
static avlTreeTables_t          *dot1xLogicalPortTreeHeap=L7_NULLPTR;
static dot1xLogicalPortInfo_t   *dot1xLogicalPortDataHeap=L7_NULLPTR;

static dot1xAuthHistoryLogTableDb_t dot1xAuthHistoryLogDb;

static L7_uint32 dot1xAuthHistEntryIndexIntfCount[L7_MAX_PORT_COUNT] = {0};
static L7_uint32 dot1xAuthHistEntryIndexCount = 0;
static L7_uint32 dot1xAuthHistCurrentEntryIndex = 0;

static L7_RC_t dot1xAuthHistoryLogCreateEntryIndex(L7_uint32 *entryIndex);


/*********************************************************************
* @purpose  Initialize Logical Port Info Database
*
* @param    nodeCount    @b{(input)} The number of nodes to be created. 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoDBInit(L7_uint32 nodeCount)
{
  /* Allocate the Heap structures */
  dot1xLogicalPortTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                                            nodeCount * sizeof(avlTreeTables_t));

  dot1xLogicalPortDataHeap  = (dot1xLogicalPortInfo_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                                                    nodeCount * sizeof(dot1xLogicalPortInfo_t));

  /* validate the pointers*/
  if ((dot1xLogicalPortTreeHeap == L7_NULLPTR) ||(dot1xLogicalPortDataHeap == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "dot1xLogicalPortInfoDBInit: Error allocating data for dot1x DB Tree."
        " Error allocating memory for the Dot1x database. Possible cause Insufficient memory.");
    return L7_FAILURE;
  }

  /* AVL Tree creations - dot1xLogicalPortTreeDb*/
  avlCreateAvlTree(&(dot1xLogicalPortTreeDb),  dot1xLogicalPortTreeHeap,
                   dot1xLogicalPortDataHeap, nodeCount,
                   sizeof(dot1xLogicalPortInfo_t), 0x10,
                   sizeof(L7_uint32));
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  DeInitialize Logical Port Info Database
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoDBDeInit(void)
{
  /* Destroy the AVL Tree */
  if(dot1xLogicalPortTreeDb.semId != L7_NULLPTR)
  {
    avlDeleteAvlTree(&dot1xLogicalPortTreeDb);
  }

  /* Give up the memory */
  if (dot1xLogicalPortTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xLogicalPortTreeHeap);
    dot1xLogicalPortTreeHeap = L7_NULLPTR;
  }
    
  if (dot1xLogicalPortDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, dot1xLogicalPortDataHeap);
    dot1xLogicalPortDataHeap = L7_NULLPTR;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Take lock for the Logical Port Info Node
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments This lock needs to be taken only the API functions not running in 
*           the dot1x threads context.
*       
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoTakeLock(void)
{
  return osapiSemaTake(dot1xLogicalPortTreeDb.semId, L7_WAIT_FOREVER);
}

/*********************************************************************
* @purpose  To Giveup lock for the Logical Port Info Node
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments This lock needs to be taken only the API functions not running in 
*           the dot1x threads context.
*       
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoGiveLock(void)
{
  return osapiSemaGive(dot1xLogicalPortTreeDb.semId);
}


/*********************************************************************
* @purpose  To allocate a Logical Port Info Node
*
* @param    intIfNum  @b{(input)} The internal interface for which this 
*                                 logical interface is being created
*
* @returns  Logical Internal Interface Number
*
* @comments none
*       
* @end
*********************************************************************/
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoAlloc(L7_uint32 intIfNum)
{
  L7_uint32               lIntIfNum;
  dot1xLogicalPortInfo_t  newNode,*retNode,*tmpNode;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* iterate through the Logical interfaces to assign a empty slot to the new node.*/
  for (lIntIfNum = DOT1X_LOGICAL_PORT_START(intIfNum);
      lIntIfNum < DOT1X_LOGICAL_PORT_END(intIfNum); lIntIfNum++)
  {
    tmpNode = dot1xLogicalPortInfoGet(lIntIfNum);
    if (tmpNode == L7_NULLPTR)
    {
      /* found one - use it */
      memset(&newNode,0,sizeof(dot1xLogicalPortInfo_t));
      newNode.logicalPortNumber = lIntIfNum;
      newNode.physPort = intIfNum;

      /* add the node to the tree */
      osapiSemaTake(dot1xLogicalPortTreeDb.semId, L7_WAIT_FOREVER);
      retNode  = avlInsertEntry(&dot1xLogicalPortTreeDb,&newNode);
      osapiSemaGive(dot1xLogicalPortTreeDb.semId);
      if (retNode == &newNode)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
                "%s:%d Error Adding the node to the Dot1x Tree for phyintf %s \n",
                __FUNCTION__,__FILE__,ifName);
        return L7_NULLPTR;
      }
      return dot1xLogicalPortInfoGet(lIntIfNum);
    }
    else if(tmpNode->inUse == L7_FALSE)
    {
      return tmpNode;
    }
  }
  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
      "%s:%d Error allocating node for phyintf %s as it reached maximum limit per port."
      " Could not allocate memory for client as maximum number of clients  allowed per port"
      " has been reached.", __FUNCTION__,__FILE__,ifName);
  return L7_NULLPTR;
}

/*********************************************************************
* @purpose  To Deallocate a Logical Port Info Node
*
* @param    intIfNum  @b{(input)} The internal interface for which this 
*                                 logical interface is being destroyed
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoDeAlloc(dot1xLogicalPortInfo_t *node)
{
  if(node != L7_NULLPTR)
  {
    node->inUse = L7_FALSE;
    if(node->logicalPortNumber != DOT1X_LOGICAL_PORT_START(node->physPort))
    {
      osapiSemaTake(dot1xLogicalPortTreeDb.semId, L7_WAIT_FOREVER);
      avlDeleteEntry(&dot1xLogicalPortTreeDb,node);
      osapiSemaGive(dot1xLogicalPortTreeDb.semId);
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  To Get a Logical Port Info Node
*
* @param    lIntIfNum  @b{(input)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments none
*       
* @end
*********************************************************************/
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoGet(L7_uint32 lIntIfNum)
{
  L7_uint32 key = lIntIfNum;
  dot1xLogicalPortInfo_t  *entry = L7_NULLPTR;
  entry = (dot1xLogicalPortInfo_t *)avlSearchLVL7(&dot1xLogicalPortTreeDb,&key,AVL_EXACT);
  return entry;
}

/*********************************************************************
* @purpose  To Get Next Logical Port Info Node
*
* @param    lIntIfNum  @b{(input)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments none
*       
* @end
*********************************************************************/
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoGetNext(L7_uint32 lIntIfNum)
{
  L7_uint32 key = lIntIfNum;
  dot1xLogicalPortInfo_t  *entry = L7_NULLPTR;
  entry = (dot1xLogicalPortInfo_t *)avlSearchLVL7(&dot1xLogicalPortTreeDb,&key,AVL_NEXT);
  return entry;
}



/*********************************************************************
* @purpose  To get physical port's logical interfaces 
*
* @param    intIfNum  @b{(input)} The internal interface 
* @param    lIntIfNum  @b{(input/output)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments For the first iteration start with zero.
*       
* @end
*********************************************************************/
dot1xLogicalPortInfo_t *dot1xPhysicalPortInfoNodeGet(L7_uint32 intIfNum)
{
  L7_uint32 lIntIfNum;
  lIntIfNum = DOT1X_LOGICAL_PORT_START(intIfNum);
  return dot1xLogicalPortInfoGet(lIntIfNum);
}

/*********************************************************************
* @purpose  To get First logical interfaces for a physical interface
*
* @param    intIfNum  @b{(input)} The internal interface 
* @param    lIntIfNum  @b{(input/output)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments For the first iteration start with zero.
*       
* @end
*********************************************************************/
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoFirstGet(L7_uint32 intIfNum,
                                                    L7_uint32 *lIntIfNum)
{
  dot1xLogicalPortInfo_t *node;
  L7_uint32 maxintf = DOT1X_LOGICAL_PORT_END(intIfNum);

  *lIntIfNum = DOT1X_LOGICAL_PORT_START(intIfNum);
  while((node=dot1xLogicalPortInfoGet(*lIntIfNum))==L7_NULLPTR &&
        *lIntIfNum < maxintf)
  {
    *lIntIfNum = *lIntIfNum + 1;
  }
  return node;
}

/*********************************************************************
* @purpose  To iterate all the logical interfaces of a physical interface
*
* @param    intIfNum  @b{(input)} The internal interface 
* @param    lIntIfNum  @b{(input/output)} The logical internal interface number
*
* @returns  Logical Internal Interface node
*
* @comments For the first iteration start with zero.
*       
* @end
*********************************************************************/
dot1xLogicalPortInfo_t *dot1xLogicalPortInfoGetNextNode(L7_uint32 intIfNum,
                                                        L7_uint32 *lIntIfNum)
{
  dot1xLogicalPortInfo_t *node;
  if(*lIntIfNum == DOT1X_LOGICAL_PORT_ITERATE)
  {
    return dot1xLogicalPortInfoFirstGet(intIfNum,lIntIfNum);
  }
  node = dot1xLogicalPortInfoGetNext(*lIntIfNum);
  if(node)
  {
    if(node->logicalPortNumber < DOT1X_LOGICAL_PORT_END(intIfNum))
    {
      *lIntIfNum = node->logicalPortNumber;
      return node;
    }
  }
  return L7_NULLPTR;
}


/*********************************************************************
* @purpose  Debug Info of the Logical Port DB
*
* @param    None
*
* @returns  None
*
* @comments none
*       
* @end
*********************************************************************/
void dot1xDebugLogicalPortInfo(void)
{
  if(dot1xLogicalPortTreeHeap != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "The Dot1x Logical Port Info has %d Nodes\n",
    dot1xLogicalPortTreeDb.count);
  }
}

/*********************************************************************
 * * @purpose  Dot1x Auth History Log Db initializations
 * *
 * * @param    None
 * *
 * * @returns  L7_SUCCESS - Initialization complete
 * *           L7_FAILURE - Initilaization failed because of
 * *                        insufficient system resources
 * *
 * * @notes
 * *
 * * @end
 * *********************************************************************/
L7_RC_t dot1xAuthHistoryLogDbInit(void)
{
  dot1xAuthHistoryLogTableDb_t *pHistoryLogDb;
  
  pHistoryLogDb = &dot1xAuthHistoryLogDb;

  /* Create Heap for AVL tree */
  pHistoryLogDb->dot1xAuthHistoryTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                             L7_DOT1X_AUTHENTICATION_MAX_EVENTS *
                                             sizeof(avlTreeTables_t));
  pHistoryLogDb->dot1xAuthHistoryDataHeap  = (dot1xAuthHistoryLogInfo_t *)osapiMalloc(L7_DOT1X_COMPONENT_ID,
                                              L7_DOT1X_AUTHENTICATION_MAX_EVENTS *
                                              sizeof(dot1xAuthHistoryLogInfo_t));

  if ((pHistoryLogDb->dot1xAuthHistoryTreeHeap == L7_NULLPTR) ||
      (pHistoryLogDb->dot1xAuthHistoryDataHeap == L7_NULLPTR)
     )
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DOT1X_COMPONENT_ID,
      "Error allocating data for Dot1x Auth History Log AVL Tree \n");
    return L7_FAILURE;
  }

  /* AVL Tree creations */
  avlCreateAvlTree(&(pHistoryLogDb->dot1xAuthHistoryAvlTree), pHistoryLogDb->dot1xAuthHistoryTreeHeap,
                   pHistoryLogDb->dot1xAuthHistoryDataHeap, L7_DOT1X_AUTHENTICATION_MAX_EVENTS,
                   sizeof(dot1xAuthHistoryLogInfo_t), 0x10,
                   sizeof(dot1xAuthHistoryLogInfoKey_t));

  /* Initialize History Log Entry Index Array */
  memset(dot1xAuthHistEntryIndexIntfCount, 0x00, L7_MAX_PORT_COUNT);

  return L7_SUCCESS;
}

/*********************************************************************
 * * @purpose  Dot1x Auth History Log Db De-initializations
 * *
 * * @param    None
 * *
 * * @returns  void
 * *
 * * @notes
 * *
 * * @end
 * *********************************************************************/
void dot1xAuthHistoryLogDbDeInit(void)
{
  dot1xAuthHistoryLogTableDb_t *pHistoryLogDb;
  
  pHistoryLogDb = &dot1xAuthHistoryLogDb;
 
  /* Destroy the AVL Tree */
  if (pHistoryLogDb->dot1xAuthHistoryTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, pHistoryLogDb->dot1xAuthHistoryTreeHeap);
    pHistoryLogDb->dot1xAuthHistoryTreeHeap = L7_NULLPTR;
  }

  if (pHistoryLogDb->dot1xAuthHistoryDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_DOT1X_COMPONENT_ID, pHistoryLogDb->dot1xAuthHistoryDataHeap);
    pHistoryLogDb->dot1xAuthHistoryDataHeap = L7_NULLPTR;
  }
 
  if (pHistoryLogDb->dot1xAuthHistoryAvlTree.semId != L7_NULLPTR)
  {
    avlDeleteAvlTree(&pHistoryLogDb->dot1xAuthHistoryAvlTree);	    
  }	  


  /* Reset History Log Entry Index Array */
  memset(dot1xAuthHistEntryIndexIntfCount, 0x00, L7_MAX_PORT_COUNT);
}


/*********************************************************************
 * * @purpose  To Take lock for the History Log Info Node
 * *
 * * @param    None
 * *
 * * @returns  L7_SUCCESS or L7_FAILURE
 * *
 * * @comments This lock needs to be taken only the API functions not running in
 * *           the dot1x threads context.
 * *
 * * @end
 * *********************************************************************/
L7_RC_t dot1xAuthHistoryLogInfoTakeLock(void)
{
  return osapiSemaTake(dot1xAuthHistoryLogDb.dot1xAuthHistoryAvlTree.semId, L7_WAIT_FOREVER);
}

/*********************************************************************
 * * @purpose  To Giveup lock for the History Log Info Node
 * *
 * * @param    None
 * *
 * * @returns  L7_SUCCESS or L7_FAILURE
 * *
 * * @comments This lock needs to be taken only the API functions not running in
 * *           the dot1x threads context.
 * *
 * * @end
 * *********************************************************************/
L7_RC_t dot1xAuthHistoryLogInfoGiveLock(void)
{
  return osapiSemaGive(dot1xAuthHistoryLogDb.dot1xAuthHistoryAvlTree.semId);
}

/*********************************************************************
 * * @purpose  To allocate a History Log Info Node
 * *
 * * @param    intIfNum  @b{(input)} The Physical interface for which 
 * *                                 the histroy log node is being created
 * *           entryIndex @b{(input)} The entryIndex for which the history
 * *                                  log node is being created.
 * *
 * * @returns  Reference to History Log Node
 * *
 * * @comments none
 * *
 * * @end
 * *********************************************************************/
dot1xAuthHistoryLogInfo_t *dot1xAuthHistoryLogInfoAlloc(L7_uint32 intIfNum,
		                                        L7_uint32 entryIndex)
{
  dot1xAuthHistoryLogInfo_t    newNode,
                               *pData=L7_NULLPTR,
                               *pEntry=L7_NULLPTR;

  dot1xAuthHistoryLogTableDb_t *pHistoryLogDb=L7_NULLPTR;

  pHistoryLogDb = &dot1xAuthHistoryLogDb;

  pEntry = dot1xAuthHistoryLogInfoGet(intIfNum, entryIndex);
  if(pEntry == L7_NULLPTR) 
  {
    memset(&newNode, 0x00, sizeof(dot1xAuthHistoryLogInfo_t));
    newNode.dot1xAuthHistoryLogInfoKey.intIfNum = intIfNum;
    newNode.dot1xAuthHistoryLogInfoKey.entryIndex = entryIndex;

    pData = avlInsertEntry(&pHistoryLogDb->dot1xAuthHistoryAvlTree, &newNode);

    if (pData == L7_NULLPTR)
    {
      /*entry was added into the avl tree*/
      if ((pData = dot1xAuthHistoryLogInfoGet(intIfNum, entryIndex))  == L7_NULLPTR)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
         "Failed to find recently added entry to the Dot1x History Log Tree for PhyIntf:[%d]\n\r",intIfNum);
        return L7_NULLPTR;
      }
      return pData;
    }

    if (pData == &newNode)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "Failed to allocate Dot1x History Log node for intf[%d] and entryIndex[%d] as it reached the maximum limit!\n\r",
				intIfNum, entryIndex);
      return L7_NULLPTR;
    }
  }
  else
  {
    return pEntry;
  }

  return L7_NULLPTR;
}

/*********************************************************************
 * * @purpose  To Deallocate a History Log Info Node
 * *
 * * @param    node @b{(input)} Reference to the History Log Info node
 * *
 * * @returns  L7_SUCCESS or L7_FAILURE
 * *
 * * @comments none
 * *
 * * @end
 * *********************************************************************/
L7_RC_t dot1xAuthHistoryLogInfoDeAlloc(dot1xAuthHistoryLogInfo_t *node)
{
  dot1xAuthHistoryLogTableDb_t *pHistoryLogDb;
  pHistoryLogDb = &dot1xAuthHistoryLogDb;
  dot1xAuthHistoryLogInfo_t *pData=L7_NULLPTR;
  L7_RC_t rc = L7_FAILURE;

  if(node != L7_NULLPTR)
  {
    pData = avlDeleteEntry(&pHistoryLogDb->dot1xAuthHistoryAvlTree, node);
    if (pData == L7_NULL)
    {
      /* Entry does not exist */
      rc = L7_FAILURE;
    }
    else if (pData == node)
    {
      /* Entry deleted */
      rc = L7_SUCCESS;
    }
  }
  return rc;
}


/*********************************************************************
 * * @purpose  Create new EntryIndex 
 * *
 * * @param    entryIndex  @b{(output)} EntryIndex
 * *
 * * @returns  L7_SUCCESS - On Success
 * *           L7_FAILURE - if the given pointer is NULL
 * *
 * * @notes
 * *
 * * @end
 * *********************************************************************/
static L7_RC_t dot1xAuthHistoryLogCreateEntryIndex(L7_uint32 *pEntryIndex)
{
  if(pEntryIndex == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  dot1xAuthHistCurrentEntryIndex += 1;
  *pEntryIndex = dot1xAuthHistCurrentEntryIndex;
  return L7_SUCCESS; 
}


/*********************************************************************
 * * @purpose  Insert dot1x auth event to history log database
 * *
 * * @param    intIfNum        @b{(input)} Interface Number
 * * @param    pHistoryLogInfo @b{(input)} Reference to the History Log Information
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * * @notes none
 * *
 * * @end
 * *
 * *********************************************************************/
L7_RC_t dot1xAuthHistoryLogEntryAdd(L7_uint32 intIfNum, 
                                    dot1xAuthHistoryLogInfo_t *pHistoryLogInfo)
{
  L7_uint32                     entryIndex=0;
  dot1xAuthHistoryLogInfo_t     historyLogEntry;
  dot1xAuthHistoryLogInfo_t     *pData = L7_NULLPTR;
  dot1xAuthHistoryLogTableDb_t  *pHistoryLogDb;
 

  memset(&historyLogEntry, 0x00, sizeof(dot1xAuthHistoryLogInfo_t));

  /* Validate the given interface */
  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (pHistoryLogInfo == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  pHistoryLogDb = &dot1xAuthHistoryLogDb;

  /* Check for the Max Entries limitation */
  if(dot1xAuthHistEntryIndexCount >= L7_DOT1X_AUTHENTICATION_MAX_EVENTS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "Dot1x History Log Table Exceeds Maximum Entries %d\n\r", dot1xAuthHistEntryIndexCount);
    /* Flush All entries */
    dot1xAuthHistoryLogPurgeAll();
  }
  
  /* Get the new valid entryIndex */
  dot1xAuthHistoryLogCreateEntryIndex(&entryIndex);
  
  /* Check If the entryIndex reachs its max limit (2^maxBits - 1) */ 
  if(entryIndex >= DOT1X_UNSIGNED_INTERGER_MAX_LIMIT)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
       "Dot1x History Log Table EntryIndex[%u] reaches Maximum data type limit\n\r", entryIndex);
    /* Flush All entries */
    dot1xAuthHistoryLogPurgeAll();
    dot1xAuthHistCurrentEntryIndex = 0;
    dot1xAuthHistoryLogCreateEntryIndex(&entryIndex);
  }

  /* Check for the Max Entries limitation per interface */
  if(dot1xAuthHistEntryIndexIntfCount[intIfNum] >= L7_DOT1X_AUTHENTICATION_MAX_INTF_EVENTS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
     "Dot1x History Log Table Exceeds Maximum Interface[%d] Entries %d\n\r", intIfNum, dot1xAuthHistEntryIndexIntfCount[intIfNum]);
    /* Flush All entries for an interface */
    dot1xAuthHistoryLogInterfacePurge(intIfNum);
  }
   
  dot1xAuthHistoryLogInfoTakeLock();

  memcpy(&historyLogEntry, pHistoryLogInfo, sizeof(dot1xAuthHistoryLogInfo_t));
  historyLogEntry.dot1xAuthHistoryLogInfoKey.intIfNum = intIfNum;
  historyLogEntry.dot1xAuthHistoryLogInfoKey.entryIndex = entryIndex;

  pData = dot1xAuthHistoryLogInfoAlloc(intIfNum, entryIndex);
  if (pData != L7_NULLPTR)
  {
    memcpy(pData, &historyLogEntry, sizeof(dot1xAuthHistoryLogInfo_t));

    dot1xAuthHistEntryIndexCount++;
    dot1xAuthHistEntryIndexIntfCount[intIfNum]++;
    dot1xAuthHistoryLogInfoGiveLock();
    return L7_SUCCESS;
  }
  dot1xAuthHistoryLogInfoGiveLock();

  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Remove dot1x auth event entry from history log database
 * *
 * * @param    intIfNum        @b{(input)} Interface Number
 * * @param    entryIndex      @b{(input)} EntryIndex
 * *
 * * @returns  L7_SUCCESS
 * * @returns  L7_FAILURE
 * *
 * * @notes none
 * *
 * * @end
 * *********************************************************************/
L7_RC_t dot1xAuthHistoryLogEntryDelete(L7_uint32 intIfNum, L7_uint32 entryIndex)
{
  dot1xAuthHistoryLogInfo_t    *pData=L7_NULLPTR;
  dot1xAuthHistoryLogTableDb_t *pHistoryLogDb;


  pHistoryLogDb = &dot1xAuthHistoryLogDb;

  /* Validate the given interface */
  if (dot1xIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  pData = dot1xAuthHistoryLogEntryFind(intIfNum, entryIndex, L7_MATCH_EXACT);
  if(pData == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  
  if(dot1xAuthHistoryLogInfoDeAlloc(pData) == L7_SUCCESS)
  {
    /* Entry deleted */
    if(dot1xAuthHistEntryIndexCount == 0)
    {
      L7_LOG_ERROR("History Log Entry Delete Invoked with Entry Index Count as NULL!\n\r");
    }
    else
    {
      dot1xAuthHistEntryIndexCount--;
    }

    if(dot1xAuthHistEntryIndexIntfCount[intIfNum] == 0)
    {
      L7_LOG_ERROR("History Log Entry Delete Invoked with Interface Entry Index Count as NULL!\n\r");
    }
    else
    {
      dot1xAuthHistEntryIndexIntfCount[intIfNum]--;
    }
    return L7_SUCCESS; 
  }

  return L7_FAILURE;
}

/*********************************************************************
 * * @purpose  Finds a dot1x Auth History Log entry 
 * * 
 * * @param    intIfNum   @b{(input)} Interface Number
 * * @param    entryIndex @b{(input)} Entry Index
 * * @param    flag       @b{(input)} Flag type for search
 * *                                L7_MATCH_EXACT   - Exact match
 * *                                L7_MATCH_GETNEXT - Next entry greater
 * *                                                   than this one
 * *
 * * @returns  null pointer on failure
 * * @returns  Auth History Log entry pointer on success
 * *
 * * @notes    none
 * *
 * * @end
 * *********************************************************************/
dot1xAuthHistoryLogInfo_t *dot1xAuthHistoryLogEntryFind(L7_uint32 intIfNum,
                                                        L7_uint32 entryIndex,
                                                        L7_uint32 flag)
{
  dot1xAuthHistoryLogInfo_t    *pHistoryLogEntry=L7_NULLPTR;
  dot1xAuthHistoryLogTableDb_t *pHistoryLogDb;
  dot1xAuthHistoryLogInfoKey_t  key;

  pHistoryLogDb = &dot1xAuthHistoryLogDb;
  memset((void *)&key, 0x00, sizeof(dot1xAuthHistoryLogInfoKey_t));

  key.intIfNum   = intIfNum;
  key.entryIndex = entryIndex;
  
  pHistoryLogEntry = avlSearchLVL7(&pHistoryLogDb->dot1xAuthHistoryAvlTree, &key, flag);
  if (pHistoryLogEntry == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  else
  {
    return pHistoryLogEntry;
  }
}

/******************************************************************************
 * * @purpose  To Get the history log info node
 * *
 * * @param    intIfNum   @b{(input)} Physical Interface Number
 * * @param    entryIndex @b{(input)} EntryIndex
 * *
 * * @returns  reference to history log info node
 * * @returns  L7_NULLPTR  if there are no entries
 * *
 * * @notes  
 * *
 * * @end
 * ******************************************************************************/
dot1xAuthHistoryLogInfo_t *dot1xAuthHistoryLogInfoGet(L7_uint32 intIfNum, 
                                                      L7_uint32 entryIndex)
{
  dot1xAuthHistoryLogInfo_t *pEntry = L7_NULLPTR;

  pEntry = dot1xAuthHistoryLogEntryFind(intIfNum, entryIndex, AVL_EXACT);
  return pEntry;  
}

/******************************************************************************
* @purpose  Gets the next History Log interface Index
*
* @param    intIfNum   @b{(input/output)} Physical Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  If there are no entries
*
* @notes  
*
* @end
******************************************************************************/
L7_RC_t dot1xAuthHistoryLogInterfaceNextGet(L7_uint32 *intIfNum)
{
  dot1xAuthHistoryLogInfo_t *pHistoryLogEntry = L7_NULLPTR;
  L7_uint32 ifIndex=0,entryIndex=0;

  ifIndex = *intIfNum; 
  
  do
  { 
    pHistoryLogEntry = dot1xAuthHistoryLogEntryFind(ifIndex, entryIndex, L7_MATCH_GETNEXT);
    if(pHistoryLogEntry != L7_NULLPTR)
    {
      ifIndex = pHistoryLogEntry->dot1xAuthHistoryLogInfoKey.intIfNum;
      entryIndex = pHistoryLogEntry->dot1xAuthHistoryLogInfoKey.entryIndex;
    }
  }while((pHistoryLogEntry != L7_NULLPTR) && (ifIndex == *intIfNum));

  if(pHistoryLogEntry == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  *intIfNum = ifIndex;
  return L7_SUCCESS;
}


/******************************************************************************
* @purpose  Gets the next History Log entry indexs
*
* @param    intIfNum   @b{(input/output)} Physical Interface Number
* @param    entryIndex @b{(input/output)} EntryIndex
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  If there are no entries
*
* @notes  
*
* @end
******************************************************************************/
L7_RC_t dot1xAuthHistoryLogEntryIndexNextGet(L7_uint32 *intIfNum, 
                                             L7_uint32 *entryIndex)
{
  dot1xAuthHistoryLogInfo_t *pHistoryLogEntry = L7_NULLPTR;
  
  pHistoryLogEntry = dot1xAuthHistoryLogEntryFind(*intIfNum, *entryIndex, L7_MATCH_GETNEXT);
  if (pHistoryLogEntry != L7_NULLPTR)
  {
    *intIfNum = pHistoryLogEntry->dot1xAuthHistoryLogInfoKey.intIfNum;
    *entryIndex = pHistoryLogEntry->dot1xAuthHistoryLogInfoKey.entryIndex;
    return L7_SUCCESS;
  }
  *intIfNum=0;
  *entryIndex=0;	
  return L7_FAILURE;
}

/******************************************************************************
 * * @purpose  To Get the next history log eventId for a interface
 * *
 * * @param    intIfNum   @b{(input)} Physical Interface Number
 * * @param    entryIndex @b{(input/output)} EntryIndex
 * *
 * * @returns  L7_SUCCESSS 
 * * @returns  L7_FAILURE
 * *
 * * @notes  
 * *
 * * @end
 * ******************************************************************************/
L7_RC_t dot1xAuthHistoryLogInfoNextEntryIndexGet(L7_uint32 intIfNum, 
                                                 L7_uint32 *entryIndex)
{
  L7_uint32 ifIndex=0, index=0;

  ifIndex = intIfNum;
  index = *entryIndex;
  while(dot1xAuthHistoryLogEntryIndexNextGet(&ifIndex, &index) == L7_SUCCESS)
  {
    if(intIfNum == ifIndex)
    {
      *entryIndex = index;
      return L7_SUCCESS;
    }
    return L7_FAILURE;
  }

  return L7_FAILURE;
}

/******************************************************************************
 * * @purpose  Retrieve the total number of entries allocated in the History
 * *           Log table
 * *
 * * @param    count @b{(output)} Total number of EntryIndexs 
 * *
 * * @returns  void
 * *
 * * @notes  
 * *
 * * @end
 * ******************************************************************************/
void dot1xAuthHistoryLogTotalNumEntriesGet(L7_uint32 *count)
{
  if(count != L7_NULLPTR)
  {
    *count = dot1xAuthHistEntryIndexCount;
  }
}

/******************************************************************************
 * * @purpose  Retrieve the total number of entries per interface allocated in 
 * *           the History Log table
 * *
 * * @param    intIfNum   @b{(output)} Physical Interface Number
 * * @param    count @b{(output)} Total number of EntryIndexs 
 * *
 * * @returns  void
 * *
 * * @notes  
 * *
 * * @end
 * ******************************************************************************/
void dot1xAuthHistoryLogIntfTotalNumEntriesGet(L7_uint32 intIfNum, L7_uint32 *count)
{
  if(count != L7_NULLPTR)
  {
    *count = dot1xAuthHistEntryIndexIntfCount[intIfNum];
  }
}


/*********************************************************************
* @purpose  To Get Number of Monitor Mode clients Authenticated
*
* @param    count @b{(output)} Count
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dot1xAuthMonitorModeClientsNumGet(L7_uint32 *count)
{
  L7_uint32 lIntIfNum=0;
  dot1xLogicalPortInfo_t  *entry = L7_NULLPTR;

  *count = 0;

  do
  {
    entry = dot1xLogicalPortInfoGetNext(lIntIfNum);
    if(entry != L7_NULLPTR)
    {
      if((entry->apmState == APM_AUTHENTICATED) &&
         (entry->isMonitorModeClient == L7_TRUE))
      {
        *count += 1;
      } 
      lIntIfNum = entry->logicalPortNumber;
    }
  }while(entry != L7_NULLPTR); 
   
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To Get Number of Dot1x clients Authenticated
*
* @param    count @b{(output)} Count
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t dot1xAuthNonMonitorModeClientsNumGet(L7_uint32 *count)
{
  L7_uint32 lIntIfNum=0;
  dot1xLogicalPortInfo_t  *entry = L7_NULLPTR;

  *count = 0;

  do
  {
    entry = dot1xLogicalPortInfoGetNext(lIntIfNum);
    if(entry != L7_NULLPTR)
    {
      if((entry->apmState == APM_AUTHENTICATED) && 
         (entry->isMonitorModeClient == L7_FALSE) &&
         (dot1xPortInfo[entry->physPort].portControlMode != L7_DOT1X_PORT_FORCE_AUTHORIZED))
      {
        *count += 1;
      } 
      lIntIfNum = entry->logicalPortNumber;
    }
  }while(entry != L7_NULLPTR); 
   
  return L7_SUCCESS;
}


/*************************************** STUB FUNCTIONs ***************************/
L7_RC_t dot1xStubAuthHistoryInfoLog(L7_uint32 intIfNum, L7_uint32 ifCount)
{
  L7_uchar8 macAddr[6]={0};
  L7_uint32 i,ifCt,count;
  L7_uint32 ifIndex;
  L7_uint32 macCt = 0;

  if(intIfNum == 0)
  {
    count = L7_MAX_PORT_COUNT;
  }
  else
  {
    count = 1;
  }
  for(i=1; i <= count; i++)
  {
    for(ifCt=1; ifCt <= ifCount; ifCt++)
    {
      macCt += 1;
      if(macAddr[5] < 0xFF)
      {
        macAddr[5] = macCt;
      } 
      else if(macAddr[4] < 0xFF)
      {
        macAddr[4] = macCt;
      } 
      else if(macAddr[3] < 0xFF)
      {
        macAddr[3] = macCt;
      } 
      else if(macAddr[2] < 0xFF)
      {
        macAddr[2] = macCt;
      } 
      else if(macAddr[1] < 0xFF)
      {
        macAddr[1] = macCt;
      } 
      else if(macAddr[0] < 0xFF)
      {
        macAddr[0] = macCt;
      } 

      if(intIfNum == 0)
        ifIndex = i;
      else
        ifIndex = intIfNum;

      dot1xStubAuthHistoryInfoLogDebug(ifIndex, 1, macAddr, 1, 1, 1);    
    }
  }

  return L7_SUCCESS;
} 

L7_RC_t dot1xStubAuthHistoryInfoLogDebug(L7_uint32 iface, L7_uint32 vlanId, 
                                     L7_uchar8 *macAddr, L7_uint32 accessStatus,
                                     L7_uint32 authStatus, L7_uint32 reasonCode)
{
  dot1xLogicalPortInfo_t logicalPortInfo;
  
  memset(&logicalPortInfo, 0, sizeof(logicalPortInfo));
  logicalPortInfo.physPort = iface;
  logicalPortInfo.logicalPortStatus = authStatus;
#if 0  
  sscanf(macAddrStr, "%u:%u:%u:%u:%u:%u", &macAddr[0],&macAddr[1],&macAddr[2],&macAddr[3],&macAddr[4],&macAddr[5]);
#endif
  memcpy(logicalPortInfo.suppMacAddr.addr, macAddr, 6);
   
  dot1xAuthHistoryInfoLog(L7_NULL, L7_NULL, reasonCode, accessStatus, &logicalPortInfo);
  return L7_SUCCESS;
}


L7_RC_t dot1xAuthenticationHistoryStats()
{
  L7_uint32 ct,ct1;

  sysapiPrintf("Total number of entries in the history table......... %d\n\r",dot1xAuthHistEntryIndexCount);
  sysapiPrintf("Maximum history Table Size........................... %d\n\r",L7_DOT1X_AUTHENTICATION_MAX_EVENTS);
  sysapiPrintf("Maximum number of histroy entries per interface...... %d\n\r",L7_DOT1X_AUTHENTICATION_MAX_INTF_EVENTS);
  dot1xMonitorModeNumClientsGet(&ct);
  dot1xNonMonitorModeNumClientsGet(&ct1);
  sysapiPrintf("Number of successful authentications................. %d\n\r",ct+ct1);  
  sysapiPrintf("Number of clients authenticated using Monitor Mode... %d\n\r",ct);  
  sysapiPrintf("Number of failing clients authentications............ %d\n\r",ct);
 
  return L7_SUCCESS; 
}

L7_RC_t dot1xAuthHistoryEntryIndexArrStat()
{
  L7_uint32 ifIndex=0;
  L7_BOOL firstTime=0;

  while(dot1xAuthHistoryLogIfIndexNextGet(&ifIndex) == L7_SUCCESS)
  {
    if(firstTime == 0)
    {
      sysapiPrintf("Interface    EntryIndex\n\r");
      sysapiPrintf("---------    ----------\n\r");
      firstTime = 1;
    }
    sysapiPrintf("%-13d%d\n\r",ifIndex,dot1xAuthHistEntryIndexIntfCount[ifIndex]);
  }
  return L7_SUCCESS;
}

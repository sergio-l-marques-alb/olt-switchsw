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
    L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
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
        LOG_MSG("%s:%d Error Adding the node to the Dot1x Tree for phyintf %d \n",
                __FUNCTION__,__FILE__,intIfNum);
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
      "%s:%d Error allocating node for phyintf %d as it reached maximum limit per port."
      " Could not allocate memory for client as maximum number of clients  allowed per port"
      " has been reached.", __FUNCTION__,__FILE__,intIfNum);
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


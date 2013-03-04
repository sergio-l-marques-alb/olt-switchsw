/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    tree_api.c
*
* @purpose     To provide a wrapper for missing or slightly
*              deficient APIs for AVL trees
*
* @component   wireless (for now)
*
* @comments    TODO: Promote to system-wide API at some point
*
* @create      12/10/2007
*
* @author      dcaugherty
* @end
*
**********************************************************************/

#include "tree_api.h"


/*********************************************************************
* @purpose  Creates the AVL tree and our wrapper
*           
* @param    L7_COMPONENT_IDS_t @b{(input)} component ID for memory 
*           allocation
* @param    L7_uint32 max_entries @b{(input)} max data entries in tree
* @param    L7_uint32 data_length @b{(input)} length in bytes of an entry
* @param    L7_uint32 key_length @b{(input)} length in bytes of key
*
* @returns  pointer to tree_t if successful
* @returns  L7_NULLPTR otherwise
*
* @notes    allocates ALL memory required.  Failure of any one step
*           will cause all allocated memory to be freed.
* @end
*********************************************************************/
tree_t * 
treeCreate(L7_COMPONENT_IDS_t component_ID,
           L7_uint32          max_entries,
           L7_uint32          data_length,
           L7_uint32          key_length)
{
  tree_t          *pRetval  = L7_NULLPTR;
  avlTreeTables_t *pTHeap   = L7_NULLPTR;
  void            *pDHeap   = L7_NULLPTR;
  void            *pZeroKey = L7_NULLPTR;
  tree_t          *pTree    = L7_NULLPTR;

  do 
  {
    /* allocate tree heap */
    pTHeap = (avlTreeTables_t *)
             osapiMalloc(component_ID, 
                         max_entries * sizeof(avlTreeTables_t));
    if (L7_NULLPTR == pTHeap)
    {
      break;
    }
    
    /* allocate data heap */
    pDHeap = (void *) osapiMalloc(component_ID, 
                                  max_entries * data_length);
    if (L7_NULLPTR == pDHeap)
    {
      break;
    }

    /* allocate "zero key" */
    pZeroKey = (void *) osapiMalloc(component_ID, 
                                    key_length);
    if (L7_NULLPTR == pZeroKey)
    {
      break;
    }

    /* allocate tree structure */
    pTree = (tree_t *) osapiMalloc(component_ID, 
                                   sizeof(tree_t));
    if (L7_NULLPTR == pTree)
    {
      break;
    }

    /* init our tree wrapper if we get here */
    avlCreateAvlTree(&(pTree->avlTree), pTHeap, pDHeap, 
                     max_entries, data_length, 0, key_length);
    pTree->tree_heap    = pTHeap;
    pTree->data_heap    = pDHeap;
    pTree->component_ID = component_ID;
    pTree->max_entries  = max_entries;
    pTree->zero_key     = pZeroKey;
    memset(pZeroKey, 0, key_length);
    pRetval = pTree;
  } while (0);

  /* deallocate everything if we fail */
  if (L7_NULLPTR == pRetval)
  {
    if (L7_NULLPTR != pTHeap)
    {
      osapiFree(component_ID, (void *) pTHeap);
    }
    if (L7_NULLPTR != pDHeap)
    {
      osapiFree(component_ID, (void *) pDHeap);
    }
    if (L7_NULLPTR != pZeroKey)
    {
      osapiFree(component_ID, (void *) pZeroKey);
    }
    /* pTree will be NULL if we get here */
  }
  return pRetval;
}


/*********************************************************************
* @purpose  Sets comparator for tree
*           
* @param    tree_t          *pTree @b{(output)} tree to manipulate
* @param    avlComparator_t comp   @b{(input)} new comparator
*
* @returns  pointer to former comparator if successful
* @returns  L7_NULLPTR otherwise
*
* @end
*********************************************************************/
avlComparator_t 
treeComparatorSet(tree_t * pTree, avlComparator_t comp)
{
  if (!pTree)
  {
    return L7_NULLPTR;
  }
  return avlSetAvlTreeComparator(&(pTree->avlTree), comp);
}



/*********************************************************************
* @purpose  Deletes tree and all of its previously allocated memory
*           
* @param    tree_t   *pTree @b{(output)} tree to wipe
*
* @returns  void
*
* @end
*********************************************************************/
void
treeDelete(tree_t * pTree)
{
  if (!pTree)
    return;

  /* Delete tree */  
  (void) avlDeleteAvlTree(&(pTree->avlTree));
  /* Delete heaps */
  osapiFree(pTree->component_ID, (void *) pTree->tree_heap);
  osapiFree(pTree->component_ID, (void *) pTree->data_heap);
  osapiFree(pTree->component_ID, (void *) pTree->zero_key);
  /* Delete wrapper */
  osapiFree(pTree->component_ID, pTree);
}


/*********************************************************************
* @purpose  Removes all elements from a tree
*           
* @param    tree_t   *pTree @b{(output)} tree to prune
*
* @returns  void
*
* @end
*********************************************************************/
void
treePurge(tree_t * pTree)
{
  if (L7_NULLPTR != pTree)
  {
    avlPurgeAvlTree(&(pTree->avlTree), pTree->max_entries);
  }
}


/*********************************************************************
* @purpose  Gets current element count
*           
* @param    tree_t          *pTree @b{(output)} tree to count
*
* @returns  L7_uint32 
*
* @notes    returns 0 if pTree is a null pointer
* @end
*********************************************************************/
L7_uint32 
treeCount(tree_t * pTree)
{
  if (!pTree)
  {
    return 0;
  }
  return avlTreeCount(&(pTree->avlTree));
}


/*********************************************************************
* @purpose  Inserts an element into a tree
*           
* @param    tree_t   *pTree @b{(output)} tree to augment
* @param    tree_t   *pItem @b{(input)}  element to insert
*
* @returns  L7_TRUE  if insertion worked
* @returns  L7_FALSE otherwise
*
* @end
*********************************************************************/
L7_BOOL
treeEntryInsert(tree_t * pTree, void * pItem)
{
  L7_BOOL rc = L7_FALSE;

  do
  {  
    if (!pTree || !pItem)
    {
      break;
    }
    if (L7_NULLPTR == avlInsertEntry(&(pTree->avlTree), pItem))
    {
      rc = L7_TRUE;
    }
  } while (0);
  return rc;
}

/*********************************************************************
* @purpose  Delete an element from a tree
*           
* @param    tree_t   *pTree @b{(output)} tree to augment
* @param    tree_t   *pItem @b{(input)}  element to delete
*
* @returns  L7_TRUE  if deletion worked (i.e. element found)
* @returns  L7_FALSE otherwise
*
* @end
*********************************************************************/
L7_BOOL
treeEntryDelete(tree_t * pTree, void * pItem)
{
  L7_BOOL rc = L7_FALSE;

  do
  {  
    if (!pTree || !pItem)
    {
      break;
    }
    if (L7_NULLPTR != avlDeleteEntry(&(pTree->avlTree), pItem))
    {
      rc = L7_TRUE;
    }
  } while (0);
  return rc;
}


/*********************************************************************
* @purpose  Find an element within a tree
*           
* @param    tree_t *pTree @b{(input)} tree to search
* @param    void   *key   @b{(input)} key to search for
*
* @returns  L7_NULLPTR if matching element not found
* @returns  pointer to element otherwise
*
* @end
*********************************************************************/
void *
treeEntryFind(tree_t * pTree, void * key)
{
  void * pItem = L7_NULLPTR;
  do
  {  
    if (!pTree || !key)
    {
      break;
    }
    pItem = avlSearchLVL7(&(pTree->avlTree), key, AVL_EXACT);
  } while (0);

  return pItem;
}

/*********************************************************************
* @purpose  Find least element within a tree with key greater than
*           a given value
*           
* @param    tree_t *pTree @b{(input)} tree to search
* @param    void   *key   @b{(input)} key to be used in search
*
* @returns  L7_NULLPTR if matching element not found
* @returns  pointer to element otherwise
*
* @end
*********************************************************************/
void *
treeEntryNextFind(tree_t * pTree, void * key)
{
  void * pItem = L7_NULLPTR;
  do
  {  
    if (!pTree || !key)
    {
      break;
    }
    pItem = avlSearchLVL7(&(pTree->avlTree), key, AVL_NEXT);
  } while (0);

  return pItem;
}


/*********************************************************************
* @purpose  Perform an operation on every element in a tree, from
*           "smallest" to "largest" key value
*           
* @param    tree_t *pTree @b{(output)} tree to manipulate
* @param    treeEntryManipulatorFn @b{(input)} manipulation function
* @param    void   *args  @b{(input/output)} arguments to function
*
* @returns  void
* 
* @notes    Used to iterate over every element in a tree.  The
*           args parameter may point to a structure that holds 
*           all of the necessary data for the maninpulator function.
* @end
*********************************************************************/

void
treeForEachEntryDo(tree_t                  *pTree, 
                    treeEntryManipulatorFn manip, 
                    void                   *args)
{
  void * pIter = L7_NULLPTR; 
  if (!pTree || !manip)
  {
    return;
  }
  pIter = avlSearchLVL7(&(pTree->avlTree), 
                        pTree->zero_key, AVL_NEXT);
  while (L7_NULLPTR != pIter)
  {
    (*manip)(pIter, args);
    pIter = avlSearchLVL7(&(pTree->avlTree), 
                          pIter, AVL_NEXT);
  }
}

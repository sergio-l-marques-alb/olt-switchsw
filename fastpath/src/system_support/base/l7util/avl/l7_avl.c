/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   avl.c
*
* @purpose    File contains all the api functions for a generic avl tree 
*
* @component  avl
*
* @comments 
*
* @create     8/31/2000
*
* @author     bmutz
* @end
*
**********************************************************************/
/*************************************************************
                    
*************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "avl_api.h"
#include "avl.h"

/*********************************************************************
* @purpose  Creates the generic avl tree structure and initialize  
*           
* @param    tree              pointer to the tree structure
* @param    treeHeap          pointer to the tree heap 
* @param    dataHeap          pointer to the data heap 
* @param    max_entries       maximum number of entries in the tree 
* @param    dataLength        length of a data entry 
* @param    type              type of data
* @param    lengthSearchKey   length of the search key
*
* @returns  AvlTree_t   pointer to the generic avl tree structure if successful
* @returns  L7_NULL     if not successful   
*
* @notes    RESTRICTIONS:1. First field in the data structure 
*                           (of data heap) must be the key 
*                        2. Last field in the data structure 
*                           (of data heap) must be a void pointer type
*       
* @end
*********************************************************************/
void avlCreateAvlTree(avlTree_t *tree, avlTreeTables_t *treeHeap, void *dataHeap,
                      L7_uint32 max_entries, L7_uint32 dataLength, L7_uint32 type,
                      L7_uint32 lengthSearchKey)
{
  bzero((char*)&tree->root, sizeof(avlTreeTables_t));
  bzero((char*)treeHeap, max_entries*sizeof(avlTreeTables_t));
  bzero((char*)dataHeap, max_entries*dataLength);

  tree->type = type;
  tree->semId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  tree->lengthData = dataLength;
  tree->offset_next = dataLength - sizeof(void*);
  tree->lengthSearchKey = lengthSearchKey;
  
  tree->initialTableHeap = treeHeap;
  tree->initialDataHeap = dataHeap;
  tree->compare = memcmp;
  tree->tableHeap = L7_NULLPTR;
  tree->dataHeap = L7_NULLPTR;
  
  avlPurgeAvlTree(tree, max_entries);
}

/*********************************************************************
* @purpose  Set the comparator function for an AVL tree
*           
* @param    tree              pointer to the tree structure
* @param    compare           pointer to the new comparator function
*                             if NULL is used, the default comparator
*                             (memcmp()) will be used.
*
* @returns  pointer to the previous comparator (this function does not fail)
*
* @notes    The default comparator in a generic avl tree is memcmp()
*           There are also some canned comparators supplied (these are
*           declared below).  If the user wants to provide a new type
*           specific comparator it should have the same signature as
*           memcmp():
*
*               int bar(void *a, void *b, size_t key_len);
*
*           and it should have the following integer return values:
*
*               >0 (I generally use 1) if a > b
*               0  if a == b
*               <0 if a < b
*
*           The algorithm for the comparison and the definitions
*           of <, >, and == belong entirely to the comparator. The
*           only requirements are that < and > are consistent, and
*           == ensures uniqueness within the tree.
* @end
*********************************************************************/
avlComparator_t avlSetAvlTreeComparator(avlTree_t *t, avlComparator_t c)
{
  avlComparator_t ret = t->compare;
  if (c != L7_NULL)
  {
    /*
     * The caller supplied a non-NULL pointer, so use the supplied
     * comparator function.
     */
    t->compare = c;
  }
  else
  {
    /*
     * If the caller supplied a NULL pointer, then restore the default.
     */
    t->compare = memcmp;
  }
  return ret;
}

/*********************************************************************
* @purpose  Deletes an avl tree structure 
*           
* @param    tree              pointer to an avl tree structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR  
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t avlDeleteAvlTree(avlTree_t *tree)
{
  if (tree == L7_NULLPTR) 
      return L7_ERROR;

  if ( osapiSemaDelete(tree->semId) != L7_SUCCESS )
      return L7_FAILURE;

  if (tree->tableHeap != L7_NULLPTR)
    osapiFree(tree->componentId, tree->tableHeap);

  if (tree->dataHeap != L7_NULLPTR)
    osapiFree(tree->componentId, tree->dataHeap);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Resets all entries but the root node 
*
* @param    avl_tree    pointer to the avl tree structure
* @param    max_entries max number of entries in the structure
*
* @returns  void
*
* @notes   
*       
* @end
*********************************************************************/
void avlPurgeAvlTree (avlTree_t *avl_tree, L7_uint32 max_entries)
{
  L7_uint32       i;
  L7_uint32       offset_next, lengthData;
  avlTreeTables_t *treeHeap;
  void            *dataHeap;

  offset_next = avl_tree->offset_next;
  lengthData = avl_tree->lengthData;
  
  avl_tree->root.link[L7_LEFT] = L7_NULL;
  avl_tree->root.link[L7_RIGHT] = L7_NULL; 
  avl_tree->count = 0;
  avl_tree->value = L7_NULL;

  treeHeap = avl_tree->initialTableHeap;
  dataHeap = avl_tree->initialDataHeap;
  
  avl_tree->currentTableHeap = treeHeap;
  avl_tree->currentDataHeap = dataHeap;

  for (i = 0; i < max_entries; i++)
  {
    treeHeap[i].link[L7_LEFT] = L7_NULL;
    treeHeap[i].link[L7_RIGHT] = &treeHeap[i+1];

    *((L7_uint32*)((char*)dataHeap + offset_next)) = (L7_uint32)((char*)dataHeap + lengthData);
    dataHeap = (char*)dataHeap + lengthData;
  }
  
  treeHeap = avl_tree->initialTableHeap;
  dataHeap = avl_tree->initialDataHeap;
  if (i > 0)  /* Added this check to make sure there is no array bound violation */
  {
  treeHeap[i-1].link[L7_RIGHT] = L7_NULL;
  *(L7_uint32 *)((char*)dataHeap + ((i-1)*lengthData) + offset_next) = L7_NULL;
}
}

/*********************************************************************
* @purpose  Inserts ITEM into TREE.  Returns NULL if the item was inserted,
*           otherwise a pointer to the duplicate item 
*
* @param    avl_tree    pointer to the avl tree structure
* @param    item        pointer to item to be inserted 
*
* @returns  NULL        if item was inserted
* @returns  void  pointer to a duplicate item, if duplicate exists
* @returns  void  pointer to item, if error
*
* @notes   
*
* @end
*********************************************************************/

extern void ptin_timer_start(L7_uint16 timer_id, char *str);
extern void ptin_timer_stop(L7_uint16 timer_id);

void * avlInsertEntry (avlTree_t *avl_tree, void *item)
{
  void * rc;

  ptin_timer_start(16,"avlInsertEntry-avlAddEntry");
  rc = avlAddEntry (avl_tree, item);
  ptin_timer_stop(16);

  return(rc);
}

/*********************************************************************
* @purpose  Searches AVL tree for an item matching ITEM.  If found, the
*           item is removed from the tree and the actual item found is returned
*           to the caller.  If no item matching ITEM exists in the tree,
*           returns NULL. 
*
* @param    avl_tree pointer to the avl tree structure
* @param    item     pointer to item to be deleted
*
* @returns  void *   pointer to the item if deleted
* @returns  NULL     if item does not exist in the tree
*
* @notes   
*       
* @end
*********************************************************************/
void * avlDeleteEntry (avlTree_t *avl_tree, void *item)
{
  void * rc;

  rc = avlRemoveEntry( avl_tree, item);

  return(rc);
}

/*********************************************************************
* @purpose  Finds next Multicast group membership entry
*
* @param    cell_ptr  @b{(input)} pointer to avl tree cell
* @param    direction @b{(input)} Direction to search for in the AVL tree
* 
* @returns  null pointer on failure
* @returns  multicast entry pointer on success
*
* @notes    none
*
* @end
*********************************************************************/
void *avlGetNextEntry(avlTree_t *avlTree, avlTreeTables_t **cell_ptr, L7_uint direction)
{
  void *entry;

  entry = avlGetNextLVL7 (avlTree, cell_ptr, direction);

  if (entry == L7_NULL) {
    return L7_NULLPTR;
  }

  return entry;
}

/*****************************************************************
* @purpose  Searches AVL tree for exact or get next match
*
* @param    avl_tree    pointer to the avl tree structure
* @param    item        pointer to item to be found
* @param    matchType   AVL_EXACT or AVL_NEXT
*  
* @returns  void *      pointer to the item if matched
* @returns  L7_NULL     if item does not exist in the tree
*
* @notes   
*
* @end
*****************************************************************/
void * avlSearchLVL7 (avlTree_t *avl_tree, void *key, L7_uint32 flags)
{
  L7_int32        diff;
  L7_uint32       found_equal;
  avlTreeTables_t *ptr, *saved_node;

  ptr = avl_tree->root.link[L7_LEFT];
  saved_node = L7_NULL;
  found_equal = L7_FALSE;

  while (ptr != NULL)
  {

    diff = avlCompareKey(key, ptr->data,
                         avl_tree->lengthSearchKey,
                         avl_tree->compare); 

    if (diff == AVL_EQUAL)
    {
      if (flags & AVL_EXACT)
      {
        saved_node  = ptr;
        found_equal = L7_TRUE;
        break;
      }
      else
        ptr=ptr->link[L7_RIGHT];
    }
    else if (diff == AVL_LESS_THAN)
    { /*goto left subtree*/
      saved_node=ptr;
      ptr=ptr->link[L7_LEFT];
    }
    else
    {                              /*goto right subtree*/
      ptr=ptr->link[L7_RIGHT];
    }
  }   /* ptr */

  if ( ( found_equal == L7_TRUE) || 
       ( (flags & AVL_NEXT) && saved_node != L7_NULL) ) /* if found or doing a get next */
    return saved_node->data;
  else
    return L7_NULL;
}



/*****************************************************************
* @purpose  Obtains count of nodes in the tree            
*
* @param    avl_tree    pointer to the avl tree structure
*  
* @returns  count       count of items in the tree
*
* @notes   
*
* @end
*****************************************************************/
L7_uint32 avlTreeCount (avlTree_t *avl_tree)
{
  return(avl_tree->count);
}


/*****************************************************************
* @purpose  Compare short (16 bit signed keys) and indicate relationship
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   
*
* @end
*****************************************************************/
int avlCompareShort16(const void *a, const void *b, size_t len)
{
  L7_short16 va = *(L7_short16 *)a;
  L7_short16 vb = *(L7_short16 *)b;

  if (va == vb)
  {
    return 0;
  }
  if (va > vb)
  {
    return 1;
  }
  return -1;
}

/*****************************************************************
* @purpose  Compare long (32 bit signed keys) and indicate relationship
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   
*
* @end
*****************************************************************/
int avlCompareLong32(const void *a, const void *b, size_t len)
{
  L7_int32 va = *(L7_int32 *)a;
  L7_int32 vb = *(L7_int32 *)b;

  if (va == vb)
  {
    return 0;
  }
  if (va > vb)
  {
    return 1;
  }
  return -1;
}

/*****************************************************************
* @purpose  Compare ushort (16 bit unsigned keys) and indicate relationship
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   
*
* @end
*****************************************************************/
int avlCompareUShort16(const void *a, const void *b, size_t len)
{
  L7_ushort16 va = *(L7_ushort16 *)a;
  L7_ushort16 vb = *(L7_ushort16 *)b;

  if (va == vb)
  {
    return 0;
  }
  if (va > vb)
  {
    return 1;
  }
  return -1;
}

/*****************************************************************
* @purpose  Compare ulong (32 bit unsigned keys) and indicate relationship
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   
*
* @end
*****************************************************************/
int avlCompareULong32(const void *a, const void *b, size_t len)
{
  L7_uint32 va = *(L7_uint32 *)a;
  L7_uint32 vb = *(L7_uint32 *)b;

  if (va == vb)
  {
    return 0;
  }
  if (va > vb)
  {
    return 1;
  }
  return -1;
}

/*****************************************************************
* @purpose  Compare IP Address keys and indicate relationship
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   
*
* @end
*****************************************************************/
int avlCompareIPAddr(const void *a, const void *b, size_t len)
{
  L7_uint32 va = *(L7_uint32 *)a;
  L7_uint32 vb = *(L7_uint32 *)b;

  if (va == vb)
  {
    return 0;
  }
  if (va > vb)
  {
    return 1;
  }
  return -1;
}

/*****************************************************************
* @purpose  Compare IP Network Address (Address and Mask) keys and
*           indicate relationship
*
* @param    a           Pointer to the first key
* @param    b           Pointer to the second key
* @param    len         Length of the key (unused)
*  
* @returns   0 if the keys are equal
* @returns   1 if key 'a' is greater than key 'b'
* @returns  -1 if key 'a' is less than key 'b'
*
* @notes   At this time, these are compared the same way they would
*          be if they were compared with memcmp() on a big-endian platform.
*          It might be more useful to compare them by some other criterion
*          (shorter masks first, for example).
*
* @end
*****************************************************************/
int avlCompareIPNetAddr(const void *a, const void *b, size_t len)
{
  L7_uint32 addr_a = *(L7_uint32 *)a;
  L7_uint32 mask_a = *(((L7_uint32 *)a) + 1);
  L7_uint32 addr_b = *(L7_uint32 *)b;
  L7_uint32 mask_b = *(((L7_uint32 *)b) + 1);

  if (addr_a < addr_b)
  {
    return -1;
  }
  if (addr_a > addr_b)
  {
    return 1;
  }
  if (mask_a < mask_b)
  {
    return -1;
  }
  if (mask_a > mask_b)
  {
    return 1;
  }
  return 0;
}

/*********************************************************************
* @purpose  Creates the generic avl tree structure and initialize  
*           
* @param    avlTree           pointer to the tree structure
* @param    componentId       caller's component ID.
* @param    max_entries       maximum number of entries in the tree 
* @param    dataLength        length of a data entry 
* @param    type              type of data
* @param    compareFcn        pointer to the comparator function
*                             if NULL is used, the default comparator
*                             (memcmp()) will be used.
* @param    lengthSearchKey   length of the search key
*
* @returns  L7_SUCCESS  if avl tree initialization was successful
* @returns  L7_FAILURE  if not successful   
*
* @notes    RESTRICTIONS:1. First field in the data structure 
*                           (of data heap) must be the key 
*                        2. Last field in the data structure 
*                           (of data heap) must be a void pointer type
*       
* @end
*********************************************************************/
L7_RC_t avlAllocAndCreateAvlTree(avlTree_t *avlTree,
                                 L7_COMPONENT_IDS_t componentId, 
                                 L7_uint32 max_entries,
                                 L7_uint32 dataLength,
                                 L7_uint32 type,
                                 avlComparator_t compareFcn,
                                 L7_uint32 lengthSearchKey)
{

  avlTreeTables_t *avlTreeTable;
  void            *avlTreeData;

  /* Allocate the AVL Tree Tables */
  avlTreeTable = osapiMalloc(componentId, sizeof(avlTreeTables_t)*max_entries);
  if (avlTreeTable == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  /* Allocate the AVL data heap */
  avlTreeData = osapiMalloc(componentId, dataLength*max_entries);
  if (avlTreeData == L7_NULLPTR)
  {
    osapiFree(componentId, avlTreeTable);
    return L7_FAILURE;
  }

  avlCreateAvlTree(avlTree, avlTreeTable, avlTreeData,
                   max_entries, dataLength, type, lengthSearchKey);

  (void)avlSetAvlTreeComparator(avlTree, compareFcn);
  avlTree->componentId = componentId;
  avlTree->tableHeap = avlTreeTable;
  avlTree->dataHeap = avlTreeData;

  return L7_SUCCESS;
}

static void avlGetAllElements_recursive(avlTree_t *avl_tree, L7_uint max_elements,
                                        avlTreeTables_t *cell_ptr, void *output_list, L7_uint *element_index);
/**
 * Get all elements of an AVL tree
 * 
 * @param avl_tree           : AVL tree pointer
 * @param output_list        : return list
 * @param number_of_elements : input: maximum number of 
 *                                    elements;
 *                             output: effective number of
 *                                     elements
 */
void avlGetAllElements(avlTree_t *avl_tree, void *output_list, L7_uint *number_of_elements)
{
  L7_uint max_elements;

  /* Validate arguments */
  if (avl_tree==L7_NULLPTR || output_list==L7_NULLPTR || number_of_elements==L7_NULLPTR || *number_of_elements==0)
    return;

  max_elements = *number_of_elements;

  /* Recursive search */
  avlGetAllElements_recursive(avl_tree,max_elements,L7_NULLPTR,output_list,number_of_elements);
}

static void avlGetAllElements_recursive(avlTree_t *avl_tree, L7_uint max_elements,
                                        avlTreeTables_t *cell_ptr, void *output_list, L7_uint *element_index)
{
  void *entry;
  L7_uint direction;
  avlTreeTables_t *cell_ptr_prev;
  L7_uint entrySize = avl_tree->lengthData;

  if (cell_ptr == L7_NULLPTR)
    *element_index = 0;

  /* Save current pointer to the tree */
  cell_ptr_prev = cell_ptr;

  for (direction = 0; direction <= 1; direction++) {

    cell_ptr = cell_ptr_prev;

    /* If maximum number of channels was reached, break for and abandon function */
    if (*element_index >= max_elements)
      break;

    /* Get next entry in the AVL tree */
    entry = avlGetNextEntry(avl_tree, &cell_ptr, direction);

    if (cell_ptr != L7_NULLPTR ) {

      /* Store channel ONLY if it meets svlan or cvlan filtering */
      if (entry != L7_NULLPTR && output_list != L7_NULLPTR)
      {
        memcpy((void *) ((L7_uint8 *) output_list+(*element_index)*entrySize), entry, entrySize);
        (*element_index)++;
      }

      /* Recursive to the left */
      avlGetAllElements_recursive(avl_tree,max_elements,cell_ptr,output_list,element_index);

      if (cell_ptr_prev==L7_NULLPTR)
        break;
    }
  }
}


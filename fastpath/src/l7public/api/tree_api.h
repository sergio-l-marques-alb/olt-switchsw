/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename    tree_api.h
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

#ifndef TREE_API_H
#define TREE_API_H

#include "osapi.h"
#include "avl_api.h"

/*********************************************************************
 * 
 * Note that with this wrapper, element restrictions with the existing
 * AVL tree still apply:
 * 1. The search key MUST appear FIRST in the data structure.
 * 2. The last element in the data structure MUST be a void pointer.
 * (It would be nice to get rid of these, but that would involve
 *  re-writing the AVL tree code itself.  No.  Thank.  You.
 *
 *
 *
 *********************************************************************/
 
/* typedef int (*avlComparator_t)(const void *a, const void *b, size_t l); */

typedef struct tree_s {
  L7_COMPONENT_IDS_t component_ID; /* for memory alloc/free */
  avlTree_t          avlTree;     
  L7_uint32          max_entries;  
  void               *zero_key;    /* for finding "first" element */
  avlTreeTables_t    *tree_heap;   
  void               *data_heap;
} tree_t;


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
	   L7_uint32          key_length);


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
treeComparatorSet(tree_t *pTree,  avlComparator_t comp);


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
L7_uint32  treeCount(tree_t *pTree);

/*********************************************************************
* @purpose  Deletes tree and all of its previously allocated memory
*           
* @param    tree_t   *pTree @b{(output)} tree to wipe
*
* @returns  void
*
* @end
*********************************************************************/
void       treeDelete(tree_t *pTree);


/*********************************************************************
* @purpose  Removes all elements from a tree
*           
* @param    tree_t   *pTree @b{(output)} tree to prune
*
* @returns  void
*
* @end
*********************************************************************/
void        treePurge(tree_t *pTree);


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
L7_BOOL     treeEntryInsert(tree_t *pTree, void *pItem);

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
L7_BOOL     treeEntryDelete(tree_t *pTree,  void *pItem);


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
void *      treeEntryFind(tree_t *pTree, void   *key);


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
void *      treeEntryNextFind(tree_t *pTree,  void   *key);


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

typedef void (*treeEntryManipulatorFn)(void * element, void * args);

void
treeForEachEntryDo(tree_t                 *pTree, 
                   treeEntryManipulatorFn manip, 
                   void                   *args);
                    

#endif /* TREE_API_H */

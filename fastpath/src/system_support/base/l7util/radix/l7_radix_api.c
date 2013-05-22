/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_radix.c
*
* @purpose    File contains all the api functions for a generic radix tree 
*
* @component  system_support
*
* @comments   See radix_api.h for more on how all this works
*
* @create     4/26/2004
*
* @author     rkelkar
* @end
*
**********************************************************************/

#include "l7_radix_defs.h"
#include "radix_api.h"
#include "radix.h"
#include "osapi.h"
#include "sysapi.h"

extern struct radix_node * rn_match_count(void *v_arg, struct radix_node_head *head, int rn_bits);

/*********************************************************************
* @purpose  Resets all entries 
*
* @param    tree        @b{(input)} pointer to the radix tree structure
*
* @returns  void
*
* @notes   This function effectively resets the tree by clearing the
*          heaps and recreating the free node linked lists. 
*       
* @end
*********************************************************************/
void radixPurgeTree(radixTree_t * tree)
{
  L7_uint32         i, value;
  L7_uint32         max_keylen;
  L7_uint32         max_entries;
  L7_uint32         offset_next, dataLength;
  void              *dataHeap;
  void              *maskNodeHeap;
  struct radix_mask *maskListHeap;
  char *cp, *cplim, *localAddr;

  dataHeap    = tree->dataHeap;
  maskNodeHeap= tree->maskNodeHeap;
  maskListHeap= tree->maskListHeap;
  max_entries = tree->max_entries;
  offset_next = tree->offset_next;
  dataLength  = tree->dataLength;
  max_keylen  = tree->mask_rnhead.max_keylen;

  /* clear heaps */
  if (dataHeap)
  {
    memset(dataHeap,     0, RADIX_DATA_HEAP_SIZE(max_entries, dataLength));
  }
  memset(maskNodeHeap, 0, RADIX_TREE_HEAP_MASK_NODE_SIZE(max_keylen));
  memset(maskListHeap, 0, RADIX_TREE_HEAP_MASK_LIST_SIZE(max_entries));
  memset(tree->mask_rnhead.rn_zeros, 0, (3 * max_keylen));

  /* re-create linked lists */
  for (i = 0; i < max_entries - 1; i++)
  {
    if (dataHeap)
    {
      value = (L7_uint32)((char*)dataHeap + dataLength);
      localAddr = (char*)dataHeap + offset_next;
      memcpy(localAddr, &value, sizeof(L7_uint32));
      dataHeap = (char *)dataHeap + dataLength;
    }

    /* init linked list for mask list */
    maskListHeap->rm_mklist = (struct radix_mask *)((char*)maskListHeap + sizeof(struct radix_mask));
    maskListHeap = maskListHeap->rm_mklist;
  }
  for (i = 0; i < (max_keylen*8) - 1; i++)
  {
  	value = (L7_uint32)((char*)maskNodeHeap + RADIX_MASK_NODE_ITEM_SIZE(max_keylen));
  	localAddr = (char*)maskNodeHeap + RADIX_MASK_NODE_ITEM_OFFSET_NEXT(max_keylen);
  	memcpy(localAddr, &value, sizeof(L7_uint32));
    maskNodeHeap = (char *)maskNodeHeap + RADIX_MASK_NODE_ITEM_SIZE(max_keylen);
  }
 
  /* reset head ptrs */
  tree->rn_datanodefreelist             = tree->dataHeap; 
  tree->mask_rnhead.rn_masknodefreelist = tree->maskNodeHeap; 
  tree->mask_rnhead.rn_mkfreelist       = tree->maskListHeap;

  /* reset count */
  tree->count = 0;

	/* init rn_ones */
  cp    = tree->mask_rnhead.rn_ones;
  cplim = tree->mask_rnhead.addmask_key;
	while (cp < cplim)
		*cp++ = -1;

  /* initialize root nodes */
  rn_inithead(&tree->mask_rnhead);
}


/*********************************************************************
* @purpose  Create the generic radix tree structure and initialize  
*           
* @param    tree       @b{(input)} pointer to the tree structure
* @param    dataHeap   @b{(input)} pointer to the data heap. If this is
*                                  NULL, then the user supplies the data
*                                  structures rather than radix making
*                                  a copy.
* @param    treeHeap   @b{(input)} pointer to the tree heap 
* @param    max_entries @b{(input)} maximum number of entries in the tree 
* @param    dataLength @b{(input)} length of a data entry 
* @param    max_keylen @b{(input)} maximum length of the search key
*
* @returns  radixTree_t   pointer to the generic radix tree structure if successful
* @returns  L7_NULLPTR  if not successful   
*
* @notes    RESTRICTIONS & RULES
*           1. For the data structure of data heap:
*             a. First field must be a node pair i.e. struct radix_node[2]
*             b. This must be followed by key[max_keylen] and mask[max_keylen]
*             c. Last field must be a void pointer type
*
*           2. First byte of the key and mask should be set to its length 
*              e.g. sockaddr_in_new
*
*           3. Size of tree heap must be RADIX_TREE_HEAP_SIZE(max_entries, max_keylen)
*       
* @end
*********************************************************************/
radixTree_t *radixCreateTree(radixTree_t *tree, 
                             void *dataHeap, 
                             void *treeHeap,  
                             L7_uint32 max_entries, 
                             L7_uint32 dataLength, 
                             L7_uint32 max_keylen)
{
  if(0 == max_keylen || L7_NULLPTR == tree)
    return (radixTree_t *)L7_NULLPTR;

  memset(tree, 0, sizeof(radixTree_t));

  tree->max_entries   = max_entries;
  tree->dataLength    = dataLength;
  tree->offset_next   = dataLength - sizeof(void*);
  tree->dataHeap      = dataHeap;
  tree->maskNodeHeap  = treeHeap;
  tree->maskListHeap  = (struct radix_mask *)((L7_uchar8 *)treeHeap + 
                        RADIX_TREE_HEAP_MASK_NODE_SIZE(max_keylen));
  tree->mask_rnhead.rn_zeros = (char *)((L7_uchar8 *)treeHeap + 
                        RADIX_TREE_HEAP_MASK_NODE_SIZE(max_keylen) + 
                        RADIX_TREE_HEAP_MASK_LIST_SIZE(max_entries));
  tree->mask_rnhead.rn_ones = tree->mask_rnhead.rn_zeros + max_keylen;
  tree->mask_rnhead.addmask_key = tree->mask_rnhead.rn_ones + max_keylen;
  tree->mask_rnhead.max_keylen = max_keylen;
	
  /* the tree provides a sema, but this should really be part of the
   * component not the tree as its up to the component using the tree
   * to protect access to the tree. This should be removed when RTO is
   * modified to use an RW lock instead of this sema (defect 30699)
   */
  tree->semId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);

  radixPurgeTree(tree);

  return(tree);
}

/*********************************************************************
* @purpose  Deletes an radix tree structure 
*           
* @param    tree   @b{(input)} pointer to an redix tree structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR  
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t radixDeleteTree(radixTree_t *tree)
{
  if (tree == L7_NULLPTR) 
      return L7_ERROR;

  if ( osapiSemaDelete(tree->semId) != L7_SUCCESS )
      return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Inserts ITEM into TREE.  Returns NULL if the item was inserted,
*           otherwise a pointer to the duplicate item 
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    item   @b{(input)} pointer to item to be inserted 
*
* @returns  NULL if item was inserted
* @returns  void  pointer to duplicate item if duplicate exists
* @returns  void  pointer to item, if error
*
* @notes    Every item must follow the rules specified for radixCreateTree
*       
* @end
*********************************************************************/
void * radixInsertEntry(radixTree_t * tree, void * item)
{
  void * new_entry;
  void * exactEntry;
  void * added_entry;
  void * key;
  void * mask; 

  if (tree->dataHeap)
  {
    /* get a node from the heap */
    new_entry = radixTreeNewDataNode(tree);
  }
  else
  {
    /* If tree created without a data heap, insert the item itself. */
    new_entry = item;
  }

  if (new_entry != L7_NULLPTR)
  {
    if (new_entry != item)
    {
      memcpy(new_entry, item, tree->dataLength);
    }
    key = (void *)(((L7_uchar8 * )new_entry) + (2 * sizeof(struct radix_node)));
    mask = (void *)(((L7_uchar8 * )key) + tree->mask_rnhead.max_keylen);

    /* try to add */
    added_entry = rn_addroute(key, mask, &tree->mask_rnhead, new_entry);
    
    if(L7_NULLPTR == added_entry)
    {
      if (tree->dataHeap)
      {
        radixTreeFreeDataNode(tree, new_entry);
      }
      new_entry = L7_NULLPTR; /* signal that nothing was added */
    }
  }

  /* unable to add? */
  if (new_entry == L7_NULLPTR)
  {
    key = (void *)(((L7_uchar8 * )item) + (2 * sizeof(struct radix_node)));
    mask = (void *)(((L7_uchar8 * )key) + tree->mask_rnhead.max_keylen);
    
    /* look for a duplicate */
    exactEntry = radixLookupNode(tree, key, mask, L7_RN_EXACT);
    
    if (exactEntry != L7_NULLPTR)
    {
      /* duplicate entry */
      return exactEntry;
    }
    else
    {
      /* error adding item */
      return item;
    }
  }

  /* success! */
  tree->count += 1;
  return(L7_NULLPTR);
}

/*********************************************************************
* @purpose  Searches Radix tree for an item matching ITEM.  If found, the
*           item is removed from the tree and the actual item found is returned
*           to the caller.  If no item matching ITEM exists in the tree,
*           returns NULL. 
*
* @param    tree @b{(input)} pointer to the radix tree structure
* @param    item @b{(input)} pointer to item to be deleted
*
* @returns  void *   pointer to the item if deleted
* @returns  NULL     if item does not exist in the tree
*
* @notes   
*       
* @end
*********************************************************************/
void * radixDeleteEntry(radixTree_t * tree, void * item)
{  
  void * key;
  void * mask; 
  void * tree_entry;

  key = (void *)(((L7_uchar8 * )item) + (2 * sizeof(struct radix_node)));
  mask = (void *)(((L7_uchar8 * )key) + tree->mask_rnhead.max_keylen);

  tree_entry = rn_delete(key, mask, &tree->mask_rnhead);

  /* found? */
  if(L7_NULLPTR == tree_entry)
    return(L7_NULLPTR);
  
  /* copy entry to item */
  if (item != tree_entry)
  {
    memcpy(item, tree_entry, tree->dataLength);
  }

  if (tree->dataHeap)
  {
    /* free data entry - masks are managed via the mask list */
    radixTreeFreeDataNode(tree, tree_entry);
  }

  tree->count -= 1;

  return(item);
}

/*********************************************************************
* @purpose  Lookup the radix tree for a specific node based on
*           key and mask values.
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    key    @b{(input)} pointer to the key
* @param    mask   @b{(input)} pointer to the mask
* @param    type   @b{(input)} lookup type can be L7_RN_EXACT or
*                              L7_RN_GETNEXT
*
* @returns  void *   pointer to the item if found
* @returns  NULL     if item does not exist in the tree
*
* @notes    The key and mask must have length in the first byte.
*           Entries are returned in increasing order of ipaddr,
*           for two entries with the same address, the entry with
*           the longer mask is returned first.
*
* @end
*********************************************************************/
void * radixLookupNode(radixTree_t * tree, void * key, void * mask, int type)
{  
  if(L7_RN_GETNEXT_ASCEND == type)
    return (void *)rn_lookup_ascending(key, mask, &tree->mask_rnhead, type);
  else
    return (void *)rn_lookup(key, mask, &tree->mask_rnhead, type);
}

/*********************************************************************
* @purpose  Find the longest match for a key
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    key    @b{(input)} pointer to the key
* @param    skipentries @b{(input)} number of matches to skip
*
* @returns  void *   pointer to the longest match item if found
* @returns  NULL     if no item matches the key
*
* @notes    The key must have length in the first byte,
*           longest match referts to the matching item with the longest
*           mask.
*
*           Setting skipentries to 0 returns the best match.
*           To back track up the tree set skipentries to 1,2..n.
*           e.g. for a tree with 1.1.1.0/24 and 1.1.0.0/16, the address
*           1.1.1.10 will match A for skipentries=0, B for skipentries=1.
*           Setting skipentries to 3..n will return null as there are no
*           more matching entries.
*       
* @end
*********************************************************************/
void * radixMatchNode(radixTree_t * tree, void * key, int skipentries)
{
  return(rn_match(key, &tree->mask_rnhead, skipentries));
}

/*********************************************************************
* @purpose  Find the longest match for a key, ignoring mask
*
* @param    tree      @b{(input)} pointer to the radix tree structure
* @param    key       @b{(input)} pointer to the key
* @param    key_bits  @b{(input)} len of key required to match.
*
* @returns  void *   pointer to the match item if found
* @returns  NULL     if no item matches the key
*
* @notes    The key must have length in the first byte,
*           longest match referts to the matching item. key_bits must
*           include 8 bits for the key len byte.
*
*           the mask len of the item is ignored, so item may not
*           be a route that resolves key. in fact, this function
*           is intended to be find one of a set of routes (eg item)
*           that the key resolves. if key is a prefix route with trailing
*           0s, then item will be the lowest valued route (or host) that
*           key resolves.
*
*       
* @end
*********************************************************************/
void * radixFirstResolved(radixTree_t * tree, void * key, int key_bits)
{
  return(rn_match_count(key, &tree->mask_rnhead, key_bits));
}


/*****************************************************************
* @purpose  Obtains count of nodes in the tree            
*
* @param    tree   @b{(input)} pointer to the radix tree structure
*  
* @returns  count of items in the tree
*
* @notes   
*
* @end
*****************************************************************/
L7_uint32 radixTreeCount (radixTree_t * tree)
{
  return(tree->count);
}

/*****************************************************************
* @purpose  Determine whether the radix tree is full
*
* @param    tree   @b{(input)} pointer to the radix tree structure
*
* @returns  TRUE if the number of entries is the max
*
* @notes
*
* @end
*****************************************************************/
L7_BOOL radixTreeFull(radixTree_t *tree)
{
  return (tree->count >= tree->max_entries);
}

/*****************************************************************
* @purpose  Get the next entry in a radix tree
*
* @param    tree    pointer to the radix tree structure
* @param    item    pointer to a tree entry (radix_node)
*
* @returns  pointer to the next tree entry
* @returns  null pointer, if no more entries
*
* @notes    - The input entry must point to an existing entry in the
*           tree not just an entry with the key and mask set.
*           radixLookupNode can be used to get an entry based
*           on a key and mask.
*
*           - Pass a null ptr to get the first entry in the tree.
*
*           - Entries are returned in sorted order
*
* @end
*****************************************************************/
void * radixGetNextEntry(radixTree_t * tree, void * item)
{
  struct radix_node * rn;

  /* start with head on null ptr */
  if(L7_NULLPTR == item)
    rn = tree->mask_rnhead.rnh_treetop;
  else
    rn = (struct radix_node *)item;

  /* skip mask only nodes (annotations in the tree) */
  while(L7_NULLPTR != (rn = (struct radix_node *)rn_getnext(rn)))
  {
    /* has a mask, addr? */
    if(L7_NULLPTR != rn->rn_u.rn_leaf.rn_Mask && L7_NULLPTR != rn->rn_u.rn_leaf.rn_Key)
      break;
  }

  return(rn);
}


/*****************************************************************
* @purpose  Print a node to the console
*
* @notes    Called from walk_tree
*
* @end
*****************************************************************/
static L7_uint32 entryCount = 0;
int dumpNode(struct l7_radix_node * node, void * arg)
{
  L7_uint32 i = 0;
  radixTree_t * tree = (radixTree_t *) arg;

  if(node->rn_bit >= 0)
  {
    /* never really comes in here because of the way walk_tree is coded */
    printf("%x: tree node, test pos %d\tL(%x) R(%x)\n", 
           (L7_uint32)node, node->rn_u.rn_node.rn_Off, \
           (L7_uint32)node->rn_u.rn_node.rn_L,
           (L7_uint32)node->rn_u.rn_node.rn_R);
  }
  else
  {
    entryCount++;

    printf("%d)\t%x: leaf key(", entryCount, (L7_uint32)node);

    for(i = 0; i < tree->mask_rnhead.max_keylen; i++)
      printf("%2x ", node->rn_u.rn_leaf.rn_Key[i]);

    printf(") mask (");

    for(i = 0; i < tree->mask_rnhead.max_keylen; i++)
      printf("%2x ", node->rn_u.rn_leaf.rn_Mask[i]);

    printf(")\n");

  }

  return 0;
}

/*****************************************************************
* @purpose  Dump leaf nodes to the console
*
* @param    tree    pointer to the radix tree structure
*
* @end
*****************************************************************/
void radixDumpTree(radixTree_t * tree)
{
  entryCount = 0;
  rn_walktree(&(tree->mask_rnhead), dumpNode, tree);
}

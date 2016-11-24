/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_radix_util.c
*
* @purpose    File contains utility functions for a generic radix tree 
*
* @component  system_support
*
* @comments 
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

/*********************************************************************
* @purpose  Allocate a data node from the data heap
*
* @param    tree   @b{(input)} pointer to the radix tree structure
*
* @returns  void *   pointer to the item if allocated
* @returns  NULL     if allocation failed
*
* @notes    Allocation is based on presence of the next ptr and offset
*
* @end
*********************************************************************/
void* radixTreeNewDataNode(radixTree_t * tree)
{
  void * ptr;
  L7_uint32 offset_next, value;
  void *localAddr;

  offset_next = tree->offset_next;
  ptr = tree->rn_datanodefreelist;

  if (ptr == NULL)
    return NULL;

  localAddr = (char*)ptr+offset_next;
  memcpy(&value, localAddr, sizeof(L7_uint32)); 
  tree->rn_datanodefreelist = (void*)value; 

  return  ptr;
}

/*********************************************************************
* @purpose  Free a data node 
*
* @param    tree   @b{(input)} pointer to the radix tree structure
* @param    ptr    @b{(input)} pointer to the data node
*
* @notes    freed nodes are returned to the free list
*
* @end
*********************************************************************/
void radixTreeFreeDataNode(radixTree_t * tree, void * ptr)
{
  L7_uint32 offset_next;

  offset_next = tree->offset_next;
  memset((L7_uchar8 *)ptr, 0, tree->dataLength);

  *((L7_uint32*)((char*)ptr+offset_next)) = (L7_uint32)(tree->rn_datanodefreelist);
  tree->rn_datanodefreelist = ptr;
}

/*********************************************************************
* @purpose  Allocate a mask node from the data heap
*
* @param    head   @b{(input)} pointer to the radix node head structure
*
* @returns  void *   pointer to the item if allocated
* @returns  NULL     if allocation failed
*
* @notes    allocation is based on presence of the next ptr 
*
* @end
*********************************************************************/
void * radixTreeNewMaskNode(struct radix_node_head * head)
{
  void * ptr;
  L7_uint32 offset_next, value;
  char *localAddr;

  offset_next = RADIX_MASK_NODE_ITEM_OFFSET_NEXT(head->max_keylen);
  ptr = head->rn_masknodefreelist;

  if (ptr == L7_NULLPTR)
  {
    LOG_MSG("radixTreeNewMaskNode: unable to allocate mask node");
    return L7_NULLPTR;
  }
  localAddr = (char*)ptr+offset_next;
  memcpy(&value, localAddr, sizeof(L7_uint32)); 
  head->rn_masknodefreelist = (void*)value; 

  return ptr;
}

/*********************************************************************
* @purpose  Free a mask node
*
* @param    head   @b{(input)} pointer to the radix node head structure
* @param    ptr    @b{(input)} pointer to the mask node
*
* @notes    This fn is not currently used as freed mask nodes are
*           never freed.
*
* @end
*********************************************************************/
void radixTreeFreeMaskNode(struct radix_node_head * head, void * ptr)
{
  L7_uint32 offset_next;
  
  offset_next = RADIX_MASK_NODE_ITEM_OFFSET_NEXT(head->max_keylen);
  memset((L7_uchar8 *)ptr, 0, RADIX_MASK_NODE_ITEM_SIZE(head->max_keylen));

  *((L7_uint32*)((char*)ptr+offset_next)) = (L7_uint32)(head->rn_masknodefreelist);
  
  head->rn_masknodefreelist = ptr;
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   l7sll.c
*
* @purpose    Single Linked List library functions
*
* @component  System support utilities
*
* @comments   none
*
* @create     14-Jun-05
*
* @author     PKB
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7sll_api.h"
#include "osapi.h"
#include <string.h>

/*******************************************************************************
**                        Static Function Declaration                         **
*******************************************************************************/

static L7_RC_t SLLSeek(L7_sll_t *list,
                       L7_sll_member_t *node,
                       L7_sll_member_t **prev);

static void SLLNodeInsert(L7_sll_t *list,
                          L7_sll_member_t *node,
                          L7_sll_member_t *prev);

static L7_sll_member_t* SLLNodeExtract(L7_sll_t *list,
                                       L7_sll_member_t *prev);

static L7_int32 SLLDefaultFuncCompare(void *data1,
                                      void *data2,
                                      L7_uint32 keyLen);

static L7_RC_t SLLDefaultFuncDestroy(L7_COMPONENT_IDS_t compId, L7_sll_member_t *node);


/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Creation of a Single Linked List
*
* @param    compId   @b{(input)}ID of the component creating the list
* @param    sortType @b{(input)}Specifies the sort order for the list.
* @param    keySize  @b{(input)}It is the size of the index(key) for each node in the list
* @param    compFunc @b{(input)}A function pointer that would be used for comparision
*                               between the nodes.
* @param    desFunc  @b{(input)}A function pointer that would be used for the deletion of
*                               node.
* @param    list     @b{(input)}The output pointer to the linked list that is created.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to create a linked list, if the comparision
*           function and destroy function are not passed then the default
*           implementations are used. Make sure to give correct key length.
*           Make Sure that the return value of compare functions is similar to
*           the memcmp() function.
*
* @end
*
*********************************************************************/
L7_RC_t SLLCreate(L7_COMPONENT_IDS_t compId,
                  L7_SLL_SORT_TYPE   sortType,
                  L7_uint32          keySize,
                  L7_sllCompareFunc  compFunc,
                  L7_sllDestroyFunc  desFunc,
                  L7_sll_t           *list)
{
  /* Some basic validations */
  if(list == L7_NULL)
    return L7_FAILURE;

  switch (sortType)
  {
    case L7_SLL_NO_ORDER:
    case L7_SLL_ASCEND_ORDER:
    case L7_SLL_DESCEND_ORDER:
      break;
    default:
      return L7_FAILURE;
  }

  memset(list,0,sizeof(L7_sll_t));
  /* assign the parameters */
  list->sllSortType    = sortType;
  list->sllDupEnable   = L7_FALSE;
  list->sllKeySize     = keySize;
  list->sllCompId      = compId;
  list->sllCompareFunc = (compFunc)?compFunc:SLLDefaultFuncCompare;
  list->sllDestroyFunc = desFunc;
  list->semId = osapiSemaBCreate (OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);  
  list->inUse = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set/Reset flags on the Single Linked List
*
* @param    list      @b{(input)}The list to modify the flags on.
* @param    flagType  @b{(input)}Flag Identifier.
* @param    flagVal   @b{(input)}Value associated with the flag.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to change the default behavior of a linked list.
*           Any behaviorial changes do not alter the already present nodes, but
*           apply only to future list operations. Due to this, it is highly adviced
*           that this call follows the creation call immediately and before
*           any other operation is performed on the list.
*
* @end
*
*********************************************************************/
L7_RC_t SLLFlagsSet(L7_sll_t *list, L7_SLL_FLAG_t flagType, L7_uint32 flagVal)
{
  /* validate the sll */
  if (list == L7_NULL)
  {
    return L7_FAILURE;
  }

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* Set the flag on the list */
  switch(flagType)
  {
    case L7_SLL_FLAG_ALLOW_DUPLICATES:
      if(flagVal == L7_TRUE)
        list->sllDupEnable = L7_TRUE;
      else if(flagVal == L7_FALSE)
        list->sllDupEnable = L7_FALSE;
      else
        return L7_FAILURE;
      break;
    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deletion of the Single Linked List
*
* @param    compId  @b{(input)}ID of the component deleting the list
*           list    @b{(input)}The pointer to the linked list that is to be destroyed.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to destroy a linked list, All the nodes are
*           iteratively deleted using the provided destroy function. The
*           given list object is destroyed as well.
*
* @end
*
*********************************************************************/
L7_RC_t SLLDestroy(L7_COMPONENT_IDS_t compId, L7_sll_t *list)
{
  /* validate the sll */
  if (list == L7_NULL)
  {
    return L7_FAILURE;
  }
  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /*  Destroy all the nodes */
  if(SLLPurge(compId, list) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  osapiSemaDelete(list->semId);

  list->inUse = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Deletion of ALL the elements of the Single Linked List
*
* @param    compId  @b{(input)}ID of the component deleting the list
*           list    @b{(input)}The pointer to the linked list that is to be destroyed.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to destroy ALL the elements of the linked list, 
*           All the nodes are iteratively deleted using the provided destroy function.
*           The list object is maintained as is.
*
* @end
*
*********************************************************************/
L7_RC_t SLLPurge(L7_COMPONENT_IDS_t compId, L7_sll_t *list)
{
  L7_sll_member_t   *node,*delNode;

  /* validate the sll */
  if (list == L7_NULL)
  {
    return L7_FAILURE;
  }


  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* Iterate through the list and delete */
  for (node = list->sllStart; node != L7_NULL;)
  {
    delNode = node;
    node = node->next;
    if(list->sllDestroyFunc != L7_NULLPTR)
      list->sllDestroyFunc(delNode);
    else
      SLLDefaultFuncDestroy(compId, delNode);
  }

  /* Reset the list variables */
  list->sllNumElements = 0;
  list->sllStart = L7_NULL;
  list->sllEnd = L7_NULL;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add a node to the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list to which the node is to
*                              be added.
*           node    @b{(input)}The pointer to the node to be added.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    The Memory allocation of the node is upto the caller not the
*           responsibility of this function.
*
* @end
*
*********************************************************************/
L7_RC_t SLLAdd(L7_sll_t *list,L7_sll_member_t *node)
{
  L7_sll_member_t *prev;

  /* Validate the input parameters */
  if (list == L7_NULL || node == L7_NULL)
    return L7_FAILURE;


  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }
  /* find the place where to insert */
  if (SLLSeek(list,node,&prev) == L7_SUCCESS)
  {
    if(list->sllDupEnable == L7_TRUE)
    {
      if(prev == L7_NULL)
        prev = list->sllStart;
      else
        prev = prev->next;
      for(; prev->next != L7_NULL; prev = prev->next)
      {
        if(list->sllCompareFunc((void *)(prev->next),(void *)node,list->sllKeySize) != 0)
        {
          break;
        }
      }
    }
    else
    {
      return L7_FAILURE;
    }
  }

  /* add it at the appropriate position */
  SLLNodeInsert(list,node,prev);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Find a node in the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list.
* @param    node    @b{(input)}The pointer to a node containing the key to be used for searching.
*
* @returns  The pointer to the actual node if found, else NULL is returned.
*
* @notes
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLFind(L7_sll_t *list, L7_sll_member_t *node)
{
  L7_sll_member_t *prev = L7_NULL;

  if (list == L7_NULL || node == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }
  
  if(SLLSeek(list, node, &prev) != L7_SUCCESS)
    return L7_NULL;

  if (prev == L7_NULL)
    return list->sllStart;
  return prev->next;
}

/*********************************************************************
*
* @purpose  Find a node next in the order in the given Single Linked List
*
* @param    list   @b{(input)} The pointer to the linked list.
* @param    node   @b{(input)}The pointer to a node containing the key to be used for searching.
*
* @returns    The pointer to the node whose key is next to the key that is provided if found,
*             else NULL is returned.
*
* @notes      This API is only applicable for ordered lists that do not have Duplicates
*             enabled. The main purpose of this API is to be able to support SNMP kind
*             of NextGet operations.
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLFindNext(L7_sll_t *list, L7_sll_member_t *node)
{
  L7_sll_member_t *prev;

  if (list == L7_NULL || node == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }
  
  if((list->sllDupEnable == L7_TRUE) || (list->sllSortType == L7_SLL_NO_ORDER))
    return L7_NULL;

  if(SLLSeek(list, node, &prev) == L7_SUCCESS)
  {
    if(prev == L7_NULL)
      prev = list->sllStart;
    else
      prev = prev->next;
  }

  if (prev == L7_NULL)
    return list->sllStart;
  return prev->next;
}

/*********************************************************************
*
* @purpose  Removes a node from the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list.
* @param    node    @b{(input)}The pointer to a node containing the key to be used for searching.
*
* @returns  The pointer to the actual node if found, else NULL is returned.
*
* @notes    The Node is removed from the list and given to the caller.
*           No memory deallocation is done.
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLRemove(L7_sll_t *list, L7_sll_member_t *node)
{
  L7_sll_member_t *prev;

  if (list == L7_NULL || node == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }
  
  if(SLLSeek(list, node, &prev) == L7_SUCCESS)
    return SLLNodeExtract(list, prev);
  return L7_NULL;
}

/*********************************************************************
*
* @purpose  Deletes a node from the Single Linked List
*
* @param    list    - The pointer to the linked list.
*           node    - The pointer to a node containing the key to be used for searching.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    The Node is removed from the list and memory deallocation is also done.
*
* @end
*********************************************************************/
L7_RC_t SLLDelete(L7_sll_t *list, L7_sll_member_t *node)
{
  L7_sll_member_t *pNode = L7_NULL;

  if (list == L7_NULL || node == L7_NULL)
    return L7_FAILURE;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* Extract the node first from the list */
  pNode = SLLRemove(list,node);
  if (pNode == L7_NULL)
    return L7_FAILURE;
  if(list->sllDestroyFunc != L7_NULLPTR)
    list->sllDestroyFunc(pNode);
  else
    SLLDefaultFuncDestroy(list->sllCompId,pNode);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Return the first node in the Single Linked List
*
* @param    list  @b{(input)}The pointer to the linked list.
*
* @returns  The pointer to the first node, if there is one in the list.
* @returns  L7_NULLPTR, if there are no nodes in the list.
*
* @notes    This API does not remove the node from the list, but simply
*           returns the first node's reference. This could be used in
*           iterating through the list.
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLFirstGet(L7_sll_t *list)
{
  if (list == L7_NULLPTR)
  {
    return L7_NULLPTR;
  }
  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULLPTR;
  }  
  return (list->sllStart);
}

/*********************************************************************
*
* @purpose  Return the next node after the given node in the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list.
*           pNode   @b{(input)}The pointer to the current node in the linked list.
*
* @returns  The pointer to the next node, if there is one in the list.
* @returns  L7_NULLPTR, if there are no nodes in the list.
*
* @notes    This API does not remove the node from the list, but simply
*           returns the node's reference. This could be used in
*           iterating through the list.
* @notes    If L7_NULLPTR is given for the pNode; then the first entry in the
*           list is returned.
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLNextGet(L7_sll_t *list, L7_sll_member_t *pNode)
{
  /* Validate the input parameters */
  if(list == L7_NULLPTR)
    return L7_NULLPTR;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULLPTR;
  }  
  
  /* If the given node pointer is NULL, return the first node on the list */
  if(pNode == L7_NULLPTR)
  {
    return SLLFirstGet(list);
  }

  /* Ensure that the given node is actually in the list */
  if(SLLNodeFind(list, pNode) != pNode)
    return L7_NULLPTR;

  /* Return the pointer to the next node */
  return pNode->next;
}

/*********************************************************************
*
* @purpose  Find the given exact node in the Single Linked List
*
* @param    list      @b{(input)}The pointer to the linked list.
* @param    searchKey @b{(input)}The pointer to the actual mode to be found.
*
* @returns  The pointer to the node if found, else NULL is returned.
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLNodeFind(L7_sll_t *list, L7_sll_member_t *pNode)
{
  L7_sll_member_t *member = L7_NULL;

  if(list == L7_NULL || pNode == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }  
  
  for(member = list->sllStart; member != L7_NULL; member=member->next)
  {
    if(member == pNode)
    {
      return member;
    }
  }
  return L7_NULL;
}

/*********************************************************************
*
* @purpose  Removes the given exact node from the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list.
* @param    pNode   @b{(input)}The pointer to the actual node to be removed.
*
* @returns  The pointer to the node if found, else NULL is returned.
*
* @notes    The Node is removed from the list and given to the caller.
*           No memory deallocation is done.
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLNodeRemove(L7_sll_t *list, L7_sll_member_t *pNode)
{
  L7_sll_member_t *prev = L7_NULL;
  L7_sll_member_t *member = L7_NULL;

  if(list == L7_NULL || pNode == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }  
  
  for (member = list->sllStart; member != L7_NULL; member=member->next)
  {
    if(member == pNode)
    {
      return SLLNodeExtract(list, prev);
    }
    prev = member;
  }
  return L7_NULL;
}

/*********************************************************************
*
* @purpose  Deletes the given exact node from the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list.
*           pNode   @b{(input)}The pointer to the actual node to be deleted.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    The Node is removed from the list and memory deallocation
*           is also done.
*
* @end
*
*********************************************************************/
L7_RC_t SLLNodeDelete(L7_sll_t *list, L7_sll_member_t *pNode)
{
  L7_sll_member_t *prev = L7_NULL;
  L7_sll_member_t *member = L7_NULL;

  if(list == L7_NULL || pNode == L7_NULL)
    return L7_FAILURE;


  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* Extract the node first from the list */
  for (member = list->sllStart; member != L7_NULL; member=member->next)
  {
    if(member == pNode)
    {
      SLLNodeExtract(list, prev);
      if(list->sllDestroyFunc != L7_NULLPTR)
        list->sllDestroyFunc(member);
      else
        SLLDefaultFuncDestroy(list->sllCompId, member);
      return L7_SUCCESS;
    }
    prev = member;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Add a node to the End of the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list to which the node is to
*                              be added.
* @param    node    @b{(input)}The pointer to the node to be added.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t SLLAtEndPush(L7_sll_t *list,L7_sll_member_t *node)
{
  L7_sll_member_t *prev;

  /* Validate the input parameters */
  if (list == L7_NULL || node == L7_NULL)
    return L7_FAILURE;


  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* Insert the node at the end after validations */
  if(list->sllSortType == L7_SLL_NO_ORDER)
  {
     /* Non-ordered list */
    if(list->sllDupEnable != L7_TRUE)
    {
      /* If duplicates are not enabled, then check for duplicate entries
         in the list */
      if (SLLSeek(list,node,&prev) == L7_SUCCESS)
        return L7_FAILURE;
    }
    /* Insert the node at the end */
    SLLNodeInsert(list,node,list->sllEnd);
  }
  else
  {
    /* Ordered list */
    if(SLLSeek(list,node,&prev) == L7_SUCCESS)
    {
      /* Entry already in the list */
      if(list->sllDupEnable != L7_TRUE)
        return L7_FAILURE;

      /* Find the last duplicate entry */
      if(prev == L7_NULL)
        prev = list->sllStart;
      else
        prev = prev->next;
      for(; prev->next != L7_NULL; prev = prev->next)
      {
        if(list->sllCompareFunc((void *)(prev->next),(void *)node,list->sllKeySize) != 0)
        {
          break;
        }
      }
    }
    /* Insert at the appropriate location */
    SLLNodeInsert(list,node,prev);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Add a node to the start of the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list to which the node is to
*                              be added.
* @param    node    @b{(input)}The pointer to the node to be added.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t SLLAtStartPush(L7_sll_t *list,L7_sll_member_t *node)
{
  L7_sll_member_t *prev;

  /* Validate the input parameters */
  if (list == L7_NULL || node == L7_NULL)
    return L7_FAILURE;


  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_FAILURE;
  }

  /* Insert the node at the start after validations */
  if(list->sllSortType == L7_SLL_NO_ORDER)
  {
     /* Non-ordered list */
    if(list->sllDupEnable != L7_TRUE)
    {
      /* If duplicates are not enabled, then check for duplicate entries
         in the list */
      if (SLLSeek(list,node,&prev) == L7_SUCCESS)
        return L7_FAILURE;
    }
    /* Insert the node at the start */
    SLLNodeInsert(list,node,L7_NULL);
  }
  else
  {
    /* Ordered list */
    if(SLLSeek(list,node,&prev) == L7_SUCCESS)
    {
      /* Entry already in the list? */
      if(list->sllDupEnable != L7_TRUE)
        return L7_FAILURE;
    }
    /* Insert at the appropriate location */
    SLLNodeInsert(list,node,prev);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Extract a node from the End of the Single Linked List
*
* @param    list    @b{(input)}The pointer to the unsorted linked list.
*
* @returns  The pointer to the node, if there are nodes in the list.
* @returns  L7_NULL, if there are no nodes in the list.
*
* @notes
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLAtEndPop(L7_sll_t *list)
{
  L7_sll_member_t  *member;

  /* Validate the input parameters */
  if (list == L7_NULL)
    return L7_NULL;


  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }

  if (list->sllStart == L7_NULL)
    return L7_NULL;
  /* Traverse to the end but one */
  for (member = list->sllStart; member->next != L7_NULL &&
      member->next != list->sllEnd; member=member->next);

  /* Extract it from the end of the list */
  return (SLLNodeExtract(list,member));
}

/*********************************************************************
*
* @purpose  Extract a node from the start of the Single Linked List
*
* @param    list    @b{(input)}The pointer to the unsorted linked list.
*
* @returns  The pointer to the node, if there are nodes in the list.
* @returns  L7_NULL, if there are no nodes in the list.
*
* @notes
*
* @end
*
*********************************************************************/
L7_sll_member_t *SLLAtStartPop(L7_sll_t *list)
{
  /* Validate the input parameters */
  if (list == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }

  if (list->sllStart == L7_NULL)
    return L7_NULL;


  /* Extract it from the start of the list */
  return (SLLNodeExtract(list,L7_NULL));
}

/*********************************************************************
*
* @purpose  Retrieve the number of members/entries in the Single Linked List
*
* @param    list    @b{(input)}The pointer to the linked list.
*
* @returns  Number of members/entries/nodes in the list.
*
* @notes
*
* @end
*
*********************************************************************/
L7_uint32 SLLNumMembersGet(L7_sll_t *list)
{
  if(list == L7_NULL)
    return L7_NULL;

  /* validate the sll inUse */
  if (list->inUse == L7_FALSE)
  {
    return L7_NULL;
  }  
  return list->sllNumElements;
}

/*********************************************************************
*
* @purpose  Finds the position of a node in the Single Linked List
*
* @param    list @b{(input)}The pointer to the linked list.
* @param    node @b{(input)}The pointer to the key/node to be used for searching.
* @param    prev @b{(input)}the output pointer contains :
*             (i) the node previous to he node that matches the given key/node,
*                 if the return value is L7_SUCCESS.
*             (ii)the node after which the given node could be inserted,
*                 if the return value is L7_FAILURE.
*
* @returns  L7_SUCCESS, if node is found
* @returns  L7_FAILURE, if node is not found.
* @returns  L7_ERROR, if list is invalid.
*
* @notes    -  None.
*
* @end
*
*********************************************************************/
static
L7_RC_t SLLSeek(L7_sll_t *list,
                L7_sll_member_t *node,
                L7_sll_member_t **prev)
{
  L7_sll_member_t *member;
  L7_int32 cmp;

  *prev = L7_NULL;
  for (member = list->sllStart; member != L7_NULL; member=member->next)
  {
    /* use the compare function */
    cmp = list->sllCompareFunc((void *)member,(void *)node,list->sllKeySize);
    if(cmp == 0)
      return L7_SUCCESS;
    switch (list->sllSortType)
      {
        case L7_SLL_NO_ORDER:
          break;
        case L7_SLL_ASCEND_ORDER:
          if(cmp > 0)
            return L7_FAILURE;
          break;
        case L7_SLL_DESCEND_ORDER:
          if(cmp < 0)
            return L7_FAILURE;
          break;
        default:
          return L7_ERROR;
      }
      *prev = member;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Inserts the node into the linked list.
*
* @param    list  @b{(input)}The pointer to the linked list.
* @param    node  @b{(input)}The pointer to the node to be inserted
* @param    prev  @b{(input)}The previous pointer after which the node is inserted.
*
* @returns   None.
*
* @notes     None
*
* @end
*
*********************************************************************/
static
void SLLNodeInsert(L7_sll_t *list,
                   L7_sll_member_t *node,
                   L7_sll_member_t *prev)
{
  if (prev == L7_NULL)
  {
    node->next = list->sllStart;
    list->sllStart = node;
  }
  else
  {
    node->next = prev->next;
    prev->next = node;
  }
  if (node->next == L7_NULL)
    list->sllEnd = node;
  list->sllNumElements++;
}

/*********************************************************************
*
* @purpose  Extracts the node into the linked list.
*
* @param    list @b{(input)}The pointer to the linked list.
*           prev @b{(input)}The previous pointer after which the node is to be extracted.
*
* @returns  The pointer to the node which has been extracted.
*
* @notes    None
*
* @end
*
*********************************************************************/
static
L7_sll_member_t *SLLNodeExtract(L7_sll_t *list,
                                L7_sll_member_t *prev)
{
  L7_sll_member_t *node;

  if (list->sllStart == L7_NULL)
    return L7_NULL;

  if (prev == L7_NULL)
  {
    node = list->sllStart;
    list->sllStart = node->next;
  }
  else
  {
    node = prev->next;
    prev->next = node->next;
  }
  if (node->next == L7_NULL)
    list->sllEnd = prev;
  list->sllNumElements--;
  return node;
}

/*********************************************************************
*
* @purpose  The Defunct compare function to be used when compare function
*           is not provided during the creation of the linked list.
*
* @param    data1  @b{(input)}The pointer to the first key.
* @param    data2  @b{(input)}The pointer to the second key.
* @param    keyLen @b{(input)}The length of the key to be compared.
*
* @returns  Less than 0, if node1 < node 2.
* @returns  Zero, if node1 == node2
* @returns  More than 0, if node1 > node2.
*
* @notes    This function is valid only when key length is more than zero.
*
* @end
*
*********************************************************************/
static
L7_int32 SLLDefaultFuncCompare(void *data1,
                               void *data2,
                               L7_uint32  keyLen)
{
  L7_sll_member_t *pNode1, *pNode2;

  if(data1 == L7_NULL || data2 == L7_NULL)
    return 1;

  pNode1 = (L7_sll_member_t *)data1;
  pNode2 = (L7_sll_member_t *)data2;

  /* when key length is zero, comparision makes no sense */
  if (keyLen > 0)
    return memcmp(pNode1->data,pNode2->data,keyLen);
  else
    return 1;
}

/*********************************************************************
*
* @purpose  The Defunct destroy function to be used when destroy function
*           is not provided during the creation of the linked list.
*
* @param    node @b{(input)}The pointer to the node that is to be deleted.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    -   None.
*
* @end
*
*********************************************************************/
static
L7_RC_t SLLDefaultFuncDestroy(L7_COMPONENT_IDS_t compId, L7_sll_member_t *node)
{
  osapiFree(compId, node);
  return L7_SUCCESS;
}

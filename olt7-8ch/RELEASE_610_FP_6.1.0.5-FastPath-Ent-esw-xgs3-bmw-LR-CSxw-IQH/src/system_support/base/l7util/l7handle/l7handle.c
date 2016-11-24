
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  handle.c
*
* @purpose   To store and retrieve handle info
*
* @component pimsm
*
* @comments  
*
* @create   02/06/2006
*
* @author   dsatyanarayana
*
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "osapi.h"
#include "log.h"
#include "l7handle_api.h"


/*******************************************************************************
**                 Function Definitions
*******************************************************************************/

/*********************************************************************
*
* @purpose  Creation of a Handler List
*
* @param    compId - Component ID
*           Size -     The size of the list.
*           list    - The output pointer to the handle list.
*           handleListMemHndl - Handle to the Internal Handle List Memory.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to create a handle list, 
*           The general paradigm for this library is to be created as part of the 
*           control block and deleted with it. This is used to store the node pointers
*           that are passed outside the context (example timer thread).
*
*           IMPORTANT: The application has to ensure that the 'handleListMemHndl'
*                      is a valid memory and is allocated w.r.t the 'size',
*                      prior to making this call.
*                      Also, the application is responsible of freeing this
*                      memory.
*
* @end
*********************************************************************/
L7_RC_t handleListInit(L7_COMPONENT_IDS_t compId,
                       L7_int32           size,
                       handle_list_t      **list,
                       handle_member_t    *handleListMemHndl)
{
  L7_int32      index1;

  /* Some basic validations */
  if(list == L7_NULL)
  {
    LOG_MSG("\n HANDLE LIST : Null handle list");
    return L7_FAILURE;
  }

  /* Create memory for the list holder */
  *list = (handle_list_t*)osapiMalloc(compId, sizeof(handle_list_t));
  if ((*list) == L7_NULLPTR)
  {
    /* Return error as memory was not allocated */
    LOG_MSG("\n HANDLE LIST : Failed to allocate handle list memory for Component= %d",
            compId);
    return L7_FAILURE;
  }

  /* create the memory for the list */
  (*list)->list = handleListMemHndl;
  if ((*list)->list == L7_NULLPTR)
  {
    osapiFree(compId,*list);
    LOG_MSG("\n HANDLE LIST : Component (%d) returned invalid memory", compId);
    /* Return error as memory was not allocated */
    return L7_FAILURE;
  }

  (*list)->size = size;
  (*list)->next_free_node = 0;
  (*list)->last_free_node = size-1;

  /* do the proper setup for better indexing */
  for(index1=0;index1<size;index1++)
  {
    (*list)->list[index1].index = index1+1;
  }
  (*list)->list[size-1].index = size-1;
  
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Destroy of a Handler List
*
* @param    compId - Component ID
*           list    - The pointer to the handle list.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    This API is provided to delete a handle list, 
*
* @end
*********************************************************************/
L7_RC_t handleListDeinit(L7_COMPONENT_IDS_t compId, handle_list_t *list)
{
  if(list == L7_NULLPTR)
  {
    LOG_MSG("\n HANDLE LIST : Null handle list");
    return L7_FAILURE;
  }

  list->list = L7_NULLPTR;
  osapiFree(compId,list);
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Store the pointer 
*
* @param    list    - The pointer to the handle list.
*           ptr     - store the pointer.
*
* @returns  a unique handle that would be used to stored in the list.
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 handleListNodeStore(handle_list_t *list,void *ptr)
{
  L7_int32    rc,temp;
  if(list == L7_NULLPTR || ptr == L7_NULLPTR)
  {
    LOG_MSG("\n HANDLE LIST : Invalid handle list params");
    return 0;
  }

  /* associate a node in the list */
  rc =  list->next_free_node;

  if(rc == -1 || list->last_free_node == list->next_free_node)
  {
    /* something is definetly wrong */
    LOG_MSG("\n HANDLE LIST : handle list allocation exceeded");
    return 0;
  }

  temp = list->list[list->next_free_node].index;
  list->list[list->next_free_node].index = -1;
  list->list[list->next_free_node].node_ptr = ptr;
  list->next_free_node = temp;
  list->numAllocated++;

  return ((L7_uint32)&list->list[rc]);
}


/*********************************************************************
*
* @purpose  To Delete the node.
*
* @param    list    - The pointer to the handle list.
*           index  - the index to the node in the list.
*
* @returns  
*
* @notes    
*
* @end
*********************************************************************/
void handleListNodeDelete_track(handle_list_t *list,L7_uint32 *handle,
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum)
{
  handle_member_t   *node;
  L7_int32    index;

  if(list == L7_NULLPTR || handle == L7_NULLPTR)
  {
    LOG_MSG("\n HANDLE LIST : Null handle list params from %s(%d)", 
        fileName, lineNum);
    return;
  }
  if(*handle == 0)
  {
    LOG_MSG("\n HANDLE LIST : Null handle node to delete from %s(%d)", 
        fileName, lineNum);
    return;
  }

  node = ( handle_member_t *)*handle;
  *handle = 0;
  index = node->index;
  
  /* additional validation */
  if(index != -1)
  {
    LOG_MSG("\n HANDLE LIST : Invalid handle node to delete from %s(%d)", 
        fileName, lineNum);
    return;
  }

  index = ((L7_uint32)node - (L7_uint32)list->list)/sizeof(handle_member_t);
  
  list->list[list->last_free_node].index = index;
  
  /* cleanup */
  list->list[index].node_ptr = L7_NULLPTR;
  list->list[index].index = index;
  list->last_free_node = index;
  list->numAllocated--;

  return;
}

/*********************************************************************
*
* @purpose  To retrieve the node.
*
* @param    list    - The pointer to the handle list.
*           index  - the index to the node in the list.
*
* @returns  The Pointer stored in the handle list.
*
* @notes    This API is provided to delete a handle list, 
*
* @end
*********************************************************************/
void* handleListNodeRetrieve_track(L7_uint32 handle,
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum)
{
  handle_member_t   *node;

  if(handle == 0)
  {
    LOG_MSG("\n HANDLE LIST : Null handle to retrieve from %s(%d)", 
        fileName, lineNum);
    return L7_NULLPTR;
  }

  node = (handle_member_t*)handle;

  /* additional validation */
  if(node->index != -1)
  {
    LOG_MSG("\n HANDLE LIST : Invalid handle node to retrieve from %s(%d)",
        fileName, lineNum);
    return L7_NULLPTR;
  }

  return node->node_ptr;
}




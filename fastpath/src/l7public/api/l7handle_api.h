/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  handle.h
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
#ifndef _HANDLE_H_
#define _HANDLE_H_


/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/

#include "l7_common.h"


/*******************************************************************************
**                             Definitions                                    **
*******************************************************************************/


/* handle storage structure definition */
typedef struct handle_member_s
{
    L7_int32         index;
    void             *node_ptr;
} handle_member_t;


typedef struct handle_list_s
{
  L7_int32          size;
  L7_int32          next_free_node;
  L7_int32          last_free_node;
  handle_member_t   *list;
  L7_uint32         numAllocated;    /* gives current size of handle list (debugging purpose)*/
} handle_list_t;




/*******************************************************************************
**                 Function Prototype Declarations                            **
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
                       handle_member_t    *handleListMemHndl);

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
extern
L7_RC_t handleListDeinit(L7_COMPONENT_IDS_t compId, handle_list_t *list);


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
extern
L7_uint32 handleListNodeStore(handle_list_t *list,void *ptr);


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
#define handleListNodeDelete(list, handle)               \
        handleListNodeDelete_track(list, handle, __FILE__, __LINE__)
extern
void handleListNodeDelete_track(handle_list_t *list,L7_uint32 *handle,
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum);


/*********************************************************************
*
* @purpose  To retrieve the node.
*
* @param    list    - The pointer to the handle list.
*           index  - the index to the node in the list.
*
* @returns  None
*
* @notes    The Pointer stored in the handle list.
*
* @end
*********************************************************************/
#define handleListNodeRetrieve(index)               \
        handleListNodeRetrieve_track(index, __FILE__, __LINE__)
extern
void* handleListNodeRetrieve_track(L7_uint32 index, 
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum);

#endif


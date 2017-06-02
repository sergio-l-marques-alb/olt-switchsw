/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 * @filename dll_api.h
 *
 * @purpose Abstract Double Link List Structure
 *
 * @component dll
 *
 * @comments The DLL Heap functions can be used with the memory buffer
 *           component.  The L7_DLL_HEAP_SIZE macro should be used to
 *           determine how much memory will be needed for a given number
 *           of members.
 *
 * @create 04/26/2001
 *
 * @author cpverne
 *
 * @end
 *
 **********************************************************************/

#ifndef _DLL_API_H
#define _DLL_API_H

#include "l7_common.h"
#include "osapi.h"

/*********************************************************************
 *
 * @structures  dll_type_t
 *
 * @purpose     this determines the type of memory access this DLL uses
 *
 * @notes       members created dynamicly can not be destroyed using the
 *              heap routines.  This is used to flag the DLL to only
 *              allow member creation/destruction functions that match
 *              the initial dll create function.
 *
 *********************************************************************/
typedef enum
{
  L7_DLL_TYPE_DYNAMIC = 1,
  L7_DLL_TYPE_HEAP
} L7_dll_type_t;

/*********************************************************************
 *
 * @structures  dll_member_s
 *
 * @purpose     this is the generic double link list member structure.
 *
 * @notes       this stores all the information needed for the
 *              traversal of the list with pointers to the data item
 *              and links to its next and previous members
 *
 *********************************************************************/
typedef struct dll_member_s
{
  struct dll_member_s  *next;
  struct dll_member_s  *prev;
  void  *data;
} L7_dll_member_t;

/*********************************************************************
 *
 * @structures  dll_s
 *
 * @purpose     this is the generic double link list structure
 *
 * @notes       this stores all the information needed for the
 *              traversal of the list with pointers to the first and
 *              last members of the list.  Heap space information is
 *              also stored for use by heap member create/delete
 *              functions.
 *
 *********************************************************************/
typedef struct dll_s
{
  L7_dll_member_t *first;         /* first member in the list */
  L7_dll_member_t *last;          /* last member in the list */
  L7_dll_member_t *first_empty;   /* first member in the empty list */
  L7_dll_member_t *last_empty;    /* last member in the empty list */
  void *empty_space;              /* pointer to next available memory location */
  L7_uint32 max_entries;
  L7_dll_type_t type;             /* type of DLL memory management to allow */
} L7_dll_t;

/*********************************************************************
 *
 * @purpose  Calculates the ammount of heap space needed
 *
 * @param    max_entries number of members this array should hold
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
#define L7_DLL_HEAP_SIZE(max_entries) (sizeof(L7_dll_t) + (max_entries * sizeof(L7_dll_member_t)))

/* Begin Function Prototypes */

/*********************************************************************
 *
 * @purpose  Creates a Double Link List structure
 *
 * @param    **dll    pointer to a L7_dll_t pointer
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLCreate(L7_dll_t **dll);

/*********************************************************************
 *
 * @purpose  Creates a Double Link List structure using a heap
 *
 * @param    **dll       pointer to a L7_dll_t pointer
 *           *dataHeap   memory buffer to use
 *           max_entries limit to the number of members this list can hold
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLHeapCreate(L7_dll_t **dll, void *dataHeap, L7_uint32 max_entries);

/*********************************************************************
 *
 * @purpose  Destroys an empty Double Link List structure
 *
 * @param    *dll    pointer to an empty L7_dll_t structure
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLDestroy(L7_dll_t *dll);

/*********************************************************************
 *
 * @purpose  Creates a Double Link List member.
 *
 * @param    **dll  pointer to a L7_dll_member_t pointer
 * @param    *data  void pointer of the data to store in the member
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLMemberCreate(L7_dll_member_t **member, void *data);

/*********************************************************************
 *
 * @purpose  Creates a Double Link List Heap member.
 *
 * @param    *dll      pointer to a L7_dll_t structure
 * @param    **member  pointer to a L7_dll_member_t pointer
 * @param    *data     void pointer of the data to store in the member
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    will reuse destroyed members in the empty list
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLHeapMemberCreate(L7_dll_t *dll, L7_dll_member_t **member, void *data);

/*********************************************************************
 *
 * @purpose  Destroys an empty Double Link List member.
 *
 * @param    *dll  pointer to an empty L7_dll_member_t
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLMemberDestroy(L7_dll_member_t *member);

/*********************************************************************
 *
 * @purpose  Destroys an empty Double Link List Heap member.
 *
 * @param    *dll  pointer to an empty L7_dll_member_t
 *
 * @returns  L7_SUCCESS  on successful creation
 * @returns  L7_FAILURE  if creation didn't happen
 *
 * @notes    moves the member to the empty list.
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLHeapMemberDestroy(L7_dll_t *dll, L7_dll_member_t *member);

/*********************************************************************
 *
 * @purpose  Inserts a member at the beinning of the list.
 *
 * @param    *dll      pointer to list object
 * @param    *member   pointer to member to be inserted
 *
 * @returns  L7_SUCCESS  if member could be inserted
 * @returns  L7_FAILURE  if member could not be inserted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLTopInsert(L7_dll_t *dll, L7_dll_member_t *member);

/*********************************************************************
 *
 * @purpose  Inserts a member at the end of the list.
 *
 * @param    *dll      pointer to list object
 * @param    *member   pointer to member to be inserted
 *
 * @returns  L7_SUCCESS  if member could be inserted
 * @returns  L7_FAILURE  if member could not be inserted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLBottomInsert(L7_dll_t *dll, L7_dll_member_t *member);

/*********************************************************************
 *
 * @purpose  Inserts a member before annother member in a list
 *
 * @param    *dll      pointer to list structure
 * @param    *target   pointer to list member to insert before
 * @param    *member   pointer to member to insert
 *
 * @returns  L7_SUCCESS  if member could be inserted
 * @returns  L7_FAILURE  if member could not be inserted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLBeforeInsert(L7_dll_t *dll, L7_dll_member_t *target, L7_dll_member_t *member);

/*********************************************************************
 *
 * @purpose  Inserts a member after annother member in a list
 *
 * @param    *dll      pointer to list structure
 * @param    *target   pointer to list member to insert after
 * @param    *member   pointer to member to insert
 *
 * @returns  L7_SUCCESS  if member could be inserted
 * @returns  L7_FAILURE  if member could not be inserted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLAfterInsert(L7_dll_t *dll, L7_dll_member_t *target, L7_dll_member_t *member);

/*********************************************************************
 *
 * @purpose  Inserts a member based on the result of supplied compare 
 *           function
 *
 * @param    *dll      pointer to list structure
 * @param    *member   pointer to member to insert
 * @param    cmp       Supplied compare function
 *
 * @returns  L7_SUCCESS  if member could be inserted
 * @returns  L7_FAILURE  if member could not be inserted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t DLLInsert(L7_dll_t *dll, L7_dll_member_t **member, 
    L7_int32 (*cmp)(L7_dll_member_t *, L7_dll_member_t *));

/*********************************************************************
 *
 * @purpose  Deletes a member from a list
 *
 * @param    *dll      pointer to list structure
 * @param    *member   pointer to member to delete
 *
 * @returns  L7_SUCCESS  if member could be deleted
 * @returns  L7_FAILURE  if member could not be deleted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLMemberDelete(L7_dll_t *dll, L7_dll_member_t *member);


/*********************************************************************
 *
 * @purpose  Deletes the first member from a list
 *
 * @param    *dll      pointer to list structure
 * @param    **member  pointer to receive the deleted member
 *
 * @returns  L7_SUCCESS  if member could be deleted
 * @returns  L7_FAILURE  if member could not be deleted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLTopDelete(L7_dll_t *dll, L7_dll_member_t **member);

/*********************************************************************
 *
 * @purpose  Deletes the last member from a list
 *
 * @param    *dll      pointer to list structure
 * @param    **member  pointer to receive the deleted member
 *
 * @returns  L7_SUCCESS  if member could be deleted
 * @returns  L7_FAILURE  if member could not be deleted
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLBottomDelete(L7_dll_t *dll, L7_dll_member_t **member);

/*********************************************************************
 *
 * @purpose  Calculates the length of the list
 *
 * @param    *dll      pointer to list structure
 * @param    *size     L7_uint32 to receive the list size
 *
 * @returns  L7_SUCCESS  if size could be determined
 * @returns  L7_FAILURE  if member could not be determined
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLSizeGet(L7_dll_t *dll, L7_uint32 *size);

/*********************************************************************
 *
 * @purpose  Retrieves an element based on it's position in the list
 *
 * @param    *dll      pointer to list structure
 * @param    **member  pointer to receive the retrieved member
 * @param    index     index into list to retrieve
 *
 * @returns  L7_SUCCESS  if member could be retrieved
 * @returns  L7_FAILURE  if member could not be retrieved
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLIndexGet(L7_dll_t *dll, L7_dll_member_t **member, L7_uint32 index);

/*********************************************************************
 *
 * @purpose  Retrieves the first element in the list
 *
 * @param    *dll      pointer to list structure
 * @param    **member  pointer to receive the retrieved member
 *
 * @returns  L7_SUCCESS  if member could be retrieved
 * @returns  L7_FAILURE  if member could not be retrieved
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLFirstGet(L7_dll_t *dll, L7_dll_member_t **member);

/*********************************************************************
 *
 * @purpose  Retrieves the last element in the list
 *
 * @param    *dll      pointer to list structure
 * @param    **member  pointer to receive the retrieved member
 *
 * @returns  L7_SUCCESS  if member could be retrieved
 * @returns  L7_FAILURE  if member could not be retrieved
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLLastGet(L7_dll_t *dll, L7_dll_member_t **member);

/*********************************************************************
 *
 * @purpose  Retrieves the member after the current member
 *
 * @param    **member  pointer to receive the next member
 *
 * @returns  L7_SUCCESS  if member could be retrieved
 * @returns  L7_FAILURE  if member could not be retrieved
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLNextGet(L7_dll_member_t **member);

/*********************************************************************
 *
 * @purpose  Retrieves the member before the current member
 *
 * @param    **member  pointer to receive the previous member
 *
 * @returns  L7_SUCCESS  if member could be retrieved
 * @returns  L7_FAILURE  if member could not be retrieved
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t
DLLPreviousGet(L7_dll_member_t **member);

/*********************************************************************
 *
 * @purpose  Checks to see if the list is empty
 *
 * @param    *dll    pointer to list structure
 *
 * @returns  L7_TRUE     if the list is empty
 * @returns  L7_FALSE    if list contains any elements
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL
DLLIsEmpty(L7_dll_t *dll);

/*********************************************************************
 *
 * @purpose  Checks to see if the list is full
 *
 * @param    *dll    pointer to list structure
 * @param    size    length list needs to be to be full
 *
 * @returns  L7_TRUE     if the list is full
 * @returns  L7_FALSE    if list contains less than size elements
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL
DLLIsFull(L7_dll_t *dll, L7_uint32 size);

/* End Function Prototypes */
#endif /* _DLL_API_H */

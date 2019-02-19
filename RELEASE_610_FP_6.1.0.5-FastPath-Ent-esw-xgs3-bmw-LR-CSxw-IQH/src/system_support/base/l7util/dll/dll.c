/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 * @filename dll.c
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

#include <string.h>
#include "dll_api.h"

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
DLLCreate(L7_dll_t **dll)
{
  /* check arguments */
  if (dll == L7_NULLPTR)
    return L7_FAILURE;

  *dll = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(L7_dll_t));

  if (*dll == L7_NULLPTR)
    return L7_FAILURE;

  memset((char*)*dll, 0, sizeof(L7_dll_t));
  (*dll)->first = L7_NULLPTR;
  (*dll)->last = L7_NULLPTR;
  (*dll)->type = L7_DLL_TYPE_DYNAMIC;
  return L7_SUCCESS;
}

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
DLLHeapCreate(L7_dll_t **dll, void *dataHeap, L7_uint32 max_entries)
{
  /* check arguments */
  if (dll == L7_NULLPTR || dataHeap == L7_NULLPTR || max_entries == 0)
    return L7_FAILURE;

  *dll = (L7_dll_t*)dataHeap;

  memset((char*)*dll, 0, sizeof(L7_dll_t));
  (*dll)->first = L7_NULLPTR;
  (*dll)->last = L7_NULLPTR;
  (*dll)->first_empty = L7_NULLPTR;
  (*dll)->last_empty = L7_NULLPTR;
  (*dll)->empty_space = (L7_uchar8 *)dataHeap + sizeof(L7_dll_t);
  (*dll)->max_entries = max_entries;
  (*dll)->type = L7_DLL_TYPE_HEAP;
  return L7_SUCCESS;
}

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
DLLDestroy(L7_dll_t *dll)
{
  /* check arguments, dll structure must be empty */
  if (dll == L7_NULLPTR ||
      dll->first != L7_NULLPTR ||
      dll->last != L7_NULLPTR)
    return L7_FAILURE;

  osapiFree(L7_SIM_COMPONENT_ID, dll);

  return L7_SUCCESS;
}

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
DLLMemberCreate(L7_dll_member_t **member, void *data)
{
  /* check arguments */
  if (member == L7_NULLPTR)
    return L7_FAILURE;

  *member = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(L7_dll_member_t));

  if (*member == L7_NULLPTR)
    return L7_FAILURE;

  memset((char*)*member, 0, sizeof(L7_dll_member_t));
  (*member)->data = data;
  (*member)->next = L7_NULLPTR;
  (*member)->prev = L7_NULLPTR;
  return L7_SUCCESS;
}

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
DLLHeapMemberCreate(L7_dll_t *dll, L7_dll_member_t **member, void *data)
{
  /* check arguments */
  if (member == L7_NULLPTR ||
      dll->type != L7_DLL_TYPE_HEAP)
    return L7_FAILURE;

  if (dll->last_empty != L7_NULLPTR)
  {
    /* select the last deleted object */
    *member = dll->last_empty;
    if (dll->last_empty == dll->first_empty)
    {
      /* only object */
      dll->first_empty = L7_NULLPTR;
      dll->last_empty = L7_NULLPTR;
    }
    else
    {
      /* point last_empty to second to last member */
      dll->last_empty = dll->last_empty->prev;
      /* remove link to the last member */
      dll->last_empty->next = L7_NULLPTR;
    }
  }
  else
  {
    /* if there are no more empty members, then add to the end of the list in the heap */
    if (dll->empty_space == L7_NULLPTR ||
	(((char*)dll->empty_space - ((char*)dll + sizeof(L7_dll_t)))/sizeof(L7_dll_member_t)) >= dll->max_entries)
    {
      return L7_FAILURE;
    }

    /* clear out the memory space and assign the new member to that space */
    *member = (L7_dll_member_t*)dll->empty_space;
    memset((char*)*member, 0, sizeof(L7_dll_member_t));
    dll->empty_space = (L7_uchar8 *)dll->empty_space + sizeof(L7_dll_member_t);
  }

  /* initialize the member */
  (*member)->data = data;
  (*member)->next = L7_NULLPTR;
  (*member)->prev = L7_NULLPTR;
  return L7_SUCCESS;
}

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
DLLMemberDestroy(L7_dll_member_t *member)
{
  /* check arguments, link and data pointers must be null */
  if (member == L7_NULLPTR ||
      member->next != L7_NULLPTR ||
      member->prev != L7_NULLPTR ||
      member->data != L7_NULLPTR)
    return L7_FAILURE;

  osapiFree(L7_SIM_COMPONENT_ID, member);

  return L7_SUCCESS;
}

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
DLLHeapMemberDestroy(L7_dll_t *dll, L7_dll_member_t *member)
{
  /* check arguments, link and data pointers must be null */
  if (dll == L7_NULLPTR ||
      member == L7_NULLPTR ||
      member->data != L7_NULLPTR ||
      member->next != L7_NULLPTR ||
      member->prev != L7_NULLPTR ||
      dll->type != L7_DLL_TYPE_HEAP)
    return L7_FAILURE;

  /* check to see if there are any deleted members */
  if (dll->first_empty == L7_NULLPTR)
  {
    dll->first_empty = member;
    dll->last_empty = member;
  }
  else
  {
    /* add member to the end of the deleted members list */
    dll->last_empty->next = member;
    member->prev = dll->last_empty;
    dll->last_empty = member;
  }

  return L7_SUCCESS;
}

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
DLLTopInsert(L7_dll_t *dll, L7_dll_member_t *member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  if (dll->first == L7_NULLPTR || dll->last == L7_NULLPTR)
  {
    /* no existing entries */
    dll->first = member;
    dll->last = member;

    member->next = L7_NULLPTR;
    member->prev = L7_NULLPTR;
  }
  else
  {
    /* there is at least one entry */
    dll->first->prev = member;
    member->next = dll->first;
    member->prev = L7_NULLPTR;
    dll->first = member;
  }

  return L7_SUCCESS;
}

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
DLLBottomInsert(L7_dll_t *dll, L7_dll_member_t *member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  if (dll->last == L7_NULLPTR)                                            /* no existing entries */
  {
    dll->first = member;
    dll->last = member;

    member->next = L7_NULLPTR;
    member->prev = L7_NULLPTR;
  }
  else                                                                    /* there is at least one entry */
  {
    dll->last->next = member;
    member->next = L7_NULLPTR;
    member->prev = dll->last;
    dll->last = member;
  }

  return L7_SUCCESS;
}

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
DLLBeforeInsert(L7_dll_t *dll, L7_dll_member_t *target, L7_dll_member_t *member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || target == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  /* check boundries */
  if (dll->first == target)
  {
    /* insert at the top */
    return DLLTopInsert(dll, member);
  }
  else
  {
    /* insert member before target member */
    member->prev = target->prev;
    target->prev->next = member;

    target->prev = member;
    member->next = target;
  }

  return L7_SUCCESS;
}

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
DLLAfterInsert(L7_dll_t *dll, L7_dll_member_t *target, L7_dll_member_t *member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || target == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  /* check boundries */
  if (dll->last == target)
  {
    /* insert at the bottom */
    return DLLBottomInsert(dll, member);
  }
  else
  {
    /* insert member after target member */
    member->next = target->next;
    target->next->prev = member;

    target->next = member;
    member->prev = target;
  }

  return L7_SUCCESS;
}

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
    L7_int32 (*cmp)(L7_dll_member_t *, L7_dll_member_t *))
{
  L7_dll_member_t *tmp;

  for(tmp = dll->first; tmp; tmp = tmp->next)
  {
    L7_int32 cmpRetCode = cmp(tmp, *member);
    if (cmpRetCode == 0)
    {
      *member = tmp;
      return L7_FAILURE;
    }

    if (cmpRetCode > 0)
    {
      return DLLBeforeInsert(dll, tmp, *member);
    }
  }
  return DLLBottomInsert(dll, *member);
}

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
DLLMemberDelete(L7_dll_t *dll, L7_dll_member_t *member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  /* malformed member */
  if ((member->prev != L7_NULLPTR && member->prev->next != member) || 
      (member->next != L7_NULLPTR && member->next->prev != member))
    return L7_FAILURE;

  /* clean up list */
  if (dll->first == member)
    dll->first = member->next;

  if (dll->last == member)
    dll->last = member->prev;

  if (member->prev != L7_NULLPTR)
    member->prev->next = member->next;

  if (member->next != L7_NULLPTR)
    member->next->prev = member->prev;

  /* clean up deleted member: must be done last */
  member->next = L7_NULLPTR;
  member->prev = L7_NULLPTR;

  return L7_SUCCESS;
}


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
DLLTopDelete(L7_dll_t *dll, L7_dll_member_t **member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || dll->first == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  /* save pointer to removed element */
  *member = dll->first;

  /* clean up list */
  if (dll->first == dll->last)
  {
    /* if no more elements*/
    dll->first = L7_NULLPTR;
    dll->last = L7_NULLPTR;
  }
  else
  {
    /* if at least one more element */
    dll->first = dll->first->next;
    dll->first->prev = L7_NULLPTR;
  }

  /* clean up deleted member: must be done last */
  (*member)->next = L7_NULLPTR;
  (*member)->prev = L7_NULLPTR;

  return L7_SUCCESS;
}

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
DLLBottomDelete(L7_dll_t *dll, L7_dll_member_t **member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || dll->last == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;

  /* save pointer to removed element */
  *member = dll->last;

  /* clean up list */
  if (dll->first == dll->last)
  {
    /* if no more elements*/
    dll->first = L7_NULLPTR;
    dll->last = L7_NULLPTR;
  }
  else
  {
    /* if at least one more element */
    dll->last = dll->last->prev;
    dll->last->next = L7_NULLPTR;
  }

  /* clean up deleted member: must be done last */
  (*member)->next = L7_NULLPTR;
  (*member)->prev = L7_NULLPTR;

  return L7_SUCCESS;
}

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
DLLSizeGet(L7_dll_t *dll, L7_uint32 *size)
{
  /* check arguments */
  L7_dll_member_t *member = L7_NULLPTR;

  if (dll == L7_NULLPTR || size == L7_NULLPTR)
    return L7_FAILURE;

  *size = 0;
  for (member = dll->first; member != L7_NULLPTR; member = member->next)
  {
    (*size)++;
  }

  return L7_SUCCESS;
}

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
DLLIndexGet(L7_dll_t *dll, L7_dll_member_t **member, L7_uint32 index)
{
  L7_dll_member_t *temp_member = L7_NULLPTR;

  /* check arguments */
  if (dll == L7_NULLPTR || member == L7_NULLPTR)
    return L7_FAILURE;


  if (dll == L7_NULLPTR || dll->first == L7_NULLPTR)
    return L7_FAILURE;

  for (temp_member = dll->first; temp_member != L7_NULLPTR; temp_member = temp_member->next)
  {
    if (index-- == 0)
    {
      *member = temp_member;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

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
DLLFirstGet(L7_dll_t *dll, L7_dll_member_t **member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || dll->first == L7_NULLPTR)
    return L7_FAILURE;

  (*member) = dll->first;
  return L7_SUCCESS;
}

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
DLLLastGet(L7_dll_t *dll, L7_dll_member_t **member)
{
  /* check arguments */
  if (dll == L7_NULLPTR || dll->last == L7_NULLPTR)
    return L7_FAILURE;

  (*member) = dll->last;
  return L7_SUCCESS;
}

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
DLLNextGet(L7_dll_member_t **member)
{
  /* check arguments */
  if (member == L7_NULLPTR || member == L7_NULLPTR || *member == L7_NULLPTR || (*member)->next == L7_NULLPTR)
    return L7_FAILURE;

  *member = (*member)->next;
  return L7_SUCCESS;
}

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
DLLPreviousGet(L7_dll_member_t **member)
{
  /* check arguments */
  if (member == L7_NULLPTR || member == L7_NULLPTR || *member == L7_NULLPTR || (*member)->prev == L7_NULLPTR)
    return L7_FAILURE;

  *member = (*member)->prev;
  return L7_SUCCESS;
}

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
DLLIsEmpty(L7_dll_t *dll)
{
  /* check arguments */
  if (dll == L7_NULLPTR)
    return L7_TRUE;                                                       /* list doesn't exist */

  return((dll->first == L7_NULLPTR) ? L7_TRUE : L7_FALSE);
}

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
DLLIsFull(L7_dll_t *dll, L7_uint32 size)
{
  L7_uint32 temp_size;

  /* check arguments */
  if (dll == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  if (DLLSizeGet(dll, &temp_size) == L7_SUCCESS)
  {
    return(temp_size == size) ? L7_TRUE : L7_FALSE;
  }
  return L7_FALSE;
}

/* End Function Declarations */

#ifdef TEST_CODE

/*********************************************************************
 *
 * @purpose  Excersizes the list management functions
 *
 * @param    none
 *
 * @returns  L7_SUCCESS    if no errors were detected
 * @returns  L7_FAILURE    if any errors were detected
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
DLLPerformTest(L7_dll_t *dll, 
    L7_dll_member_t *member1, L7_uint32 *data1,
    L7_dll_member_t *member2, L7_uint32 *data2,
    L7_dll_member_t *member3, L7_uint32 *data3)
{
  L7_dll_member_t *test_member = L7_NULLPTR;

  L7_uint32 size = 0;

  if (DLLTopInsert(dll, member1) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLTopInsert(dll, member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLTopInsert(dll, member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLTopDelete(dll, &member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (member3->data != data3)
    return L7_FAILURE;

  if (DLLTopDelete(dll, &member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (member2->data != data2)
    return L7_FAILURE;

  if (DLLBottomInsert(dll, member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBottomInsert(dll, member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLTopDelete(dll, &member1) != L7_SUCCESS)
    return L7_FAILURE;

  if (member1->data != data1)
    return L7_FAILURE;

  if (DLLTopDelete(dll, &member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (member3->data != data3)
    return L7_FAILURE;

  if (DLLTopDelete(dll, &member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (member2->data != data2)
    return L7_FAILURE;

  if (DLLSizeGet(dll, &size) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBottomInsert(dll, member1) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLSizeGet(dll, &size) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBottomInsert(dll, member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLSizeGet(dll, &size) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBottomInsert(dll, member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLSizeGet(dll, &size) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLIndexGet(dll, &test_member, 0) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLIndexGet(dll, &test_member, 1) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLIndexGet(dll, &test_member, 2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBottomDelete(dll, &member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (member3->data != data3)
    return L7_FAILURE;

  if (DLLBottomDelete(dll, &member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (member2->data != data2)
    return L7_FAILURE;

  if (DLLBottomDelete(dll, &member1) != L7_SUCCESS)
    return L7_FAILURE;

  if (member1->data != data1)
    return L7_FAILURE;

  if (DLLTopInsert(dll, member1) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLAfterInsert(dll, member1, member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBottomDelete(dll, &member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (member2->data != data2)
    return L7_FAILURE;

  if (DLLAfterInsert(dll, member1, member3) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLBeforeInsert(dll, member3, member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLFirstGet(dll, &test_member) != L7_SUCCESS)
    return L7_FAILURE;

  if (test_member->data != data1)
    return L7_FAILURE;

  if (DLLNextGet(&test_member) != L7_SUCCESS)
    return L7_FAILURE;

  if (test_member->data != data2)
    return L7_FAILURE;

  if (DLLNextGet(&test_member) != L7_SUCCESS)
    return L7_FAILURE;

  if (test_member->data != data3)
    return L7_FAILURE;

  if (DLLLastGet(dll, &test_member) != L7_SUCCESS)
    return L7_FAILURE;

  if (test_member->data != data3)
    return L7_FAILURE;

  if (DLLPreviousGet(&test_member) != L7_SUCCESS)
    return L7_FAILURE;

  if (test_member->data != data2)
    return L7_FAILURE;

  if (DLLPreviousGet(&test_member) != L7_SUCCESS)
    return L7_FAILURE;

  if (test_member->data != data1)
    return L7_FAILURE;

  if (DLLMemberDelete(dll, member1) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLMemberDelete(dll, member2) != L7_SUCCESS)
    return L7_FAILURE;

  if (DLLMemberDelete(dll, member3) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Excersizes the list management functions
 *
 * @param    none
 *
 * @returns  L7_SUCCESS    if no errors were detected
 * @returns  L7_FAILURE    if any errors were detected
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
DLLTest()
{
  L7_RC_t rc = L7_FAILURE;

  L7_dll_t *dll = L7_NULLPTR;
  L7_dll_member_t *member1 = L7_NULLPTR;
  L7_dll_member_t *member2 = L7_NULLPTR;
  L7_dll_member_t *member3 = L7_NULLPTR;

  L7_uint32 data1 = 1;
  L7_uint32 data2 = 2;
  L7_uint32 data3 = 3;

  if (DLLCreate(&dll) == L7_SUCCESS &&
      DLLMemberCreate(&member1, &data1) == L7_SUCCESS &&
      DLLMemberCreate(&member2, &data2) == L7_SUCCESS &&
      DLLMemberCreate(&member3, &data3) == L7_SUCCESS)
  {
    rc = DLLPerformTest(dll, 
	member1, &data1, 
	member2, &data2, 
	member3, &data3);
  }

  member1->data = L7_NULLPTR;
  member2->data = L7_NULLPTR;
  member3->data = L7_NULLPTR;

  DLLMemberDestroy(member1);
  DLLMemberDestroy(member2);
  DLLMemberDestroy(member3);
  DLLDestroy(dll);

  return rc;
}

/*********************************************************************
 *
 * @purpose  Excersizes the list management functions
 *
 * @param    none
 *
 * @returns  L7_SUCCESS    if no errors were detected
 * @returns  L7_FAILURE    if any errors were detected
 *
 * @notes    uses a memory heap
 *
 * @end
 *********************************************************************/
  L7_RC_t
DLLHeapTest()
{
  L7_RC_t rc = L7_FAILURE;

  void *dll_heap = osapiMalloc(L7_SIM_COMPONENT_ID, L7_DLL_HEAP_SIZE(3));

  /* to test that there is no out of bounds error */
  void *end_test = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(L7_uint32));

  L7_dll_t *dll = L7_NULLPTR;
  L7_dll_member_t *member1 = L7_NULLPTR;
  L7_dll_member_t *member2 = L7_NULLPTR;
  L7_dll_member_t *member3 = L7_NULLPTR;

  L7_uint32 data1 = 1;
  L7_uint32 data2 = 2;
  L7_uint32 data3 = 3;

  if (DLLHeapCreate(&dll, dll_heap, 3) == L7_SUCCESS &&
      DLLHeapMemberCreate(dll, &member1, &data1) == L7_SUCCESS &&
      DLLHeapMemberCreate(dll, &member2, &data2) == L7_SUCCESS &&
      DLLHeapMemberCreate(dll, &member3, &data3) == L7_SUCCESS)
  {
    *(L7_uint32*)end_test = 0xFFFFFFFF;
    rc = DLLPerformTest(dll, 
	member1, &data1, 
	member2, &data2, 
	member3, &data3);
    if (*(L7_uint32*)end_test != 0xFFFFFFFF)
      rc = L7_FAILURE;
  }

  member1->data = L7_NULLPTR;
  member2->data = L7_NULLPTR;
  member3->data = L7_NULLPTR;

  if (DLLHeapMemberDestroy(dll, member1) != L7_SUCCESS)
    rc = L7_FAILURE;

  if (DLLHeapMemberDestroy(dll, member2) != L7_SUCCESS)
    rc = L7_FAILURE;

  if (DLLHeapMemberDestroy(dll, member3) != L7_SUCCESS)
    rc = L7_FAILURE;

  osapiFree(L7_SIM_COMPONENT_ID, dll_heap);
  osapiFree(L7_SIM_COMPONENT_ID, end_test);

  return rc;
}


#endif /* TEST_CODE */

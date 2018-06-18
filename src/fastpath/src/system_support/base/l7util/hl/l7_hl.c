/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      l7_hl.c
 *
 * @purpose       Hash list utility
 *
 * @component     None
 *
 * @comments      See l7_hl_api.h.
 *
 * @create       3/10/2008
 *
 * @author       Alex Ossinski (original code in routing utils)
 *               Rob Rice (L7 utils version)
 *
 * @end
 *
 *********************************************************************/


#include <string.h>
#include <stdio.h>

#include "osapi_support.h"
#include "log.h"
#include "buff_api.h"
#include "l7_cnfgr_api.h"

#include "l7_hl_api.h"
#include "l7_hl.h"



/*********************************************************************
 * @purpose          Link a HL entry into a DLL off one of the hash buckets
 *
 * @param  head       @b{(input)} DLL list head
 * @param  object     @b{(input)}  entry to be linked
 *
 * @returns           void
 *
 * @notes             Inserts at head of list.
 *
 * @end
 * ********************************************************************/
void hlAddToDLList(hl_value_t **head, hl_value_t *object)
{
  if (*head)
    (*head)->prev = object;
  object->next = *head;
  *head = object;
}

/*********************************************************************
 * @purpose          Unlink an entry from its DLL
 *
 * @param  head       @b{(input)} DLL list head
 * @param  object     @b{(input)} entry to be removed from DLL
 *
 * @returns           void
 *
 * @notes             Inserts at head of list.
 *
 * @end
 * ********************************************************************/
void hlDelFromDLList(hl_value_t **head, hl_value_t *object)
{
  if (object->prev)
      object->prev->next = object->next;
  else
     *head = object->next;
  if (object->next)
      object->next->prev = object->prev;
}

/*********************************************************************
 * @purpose        Apply hash algorithm to get hash bucket from key.
 *
 * @param  p_H     @b{(input)} hash list
 * @param  key     @b{(input)} pointer to first byte of key
 *
 * @returns        void
 *
 * @notes          Modulo of the last 4 bytes of the key, unless user
 *                 asks to use first 4 bytes of key as index.
 *
 * @end
 * ********************************************************************/
L7_ushort16 hlGetIndex(l7_hl_t *p_H, L7_uchar8 *key)
{
#if 0
/* This is the original hash algorithm. I didn't think it was very good, so
 * I replaced it with what's below. */
  L7_ushort16 hashVal = 0;
  L7_uchar8 *keyStart = key + (((p_H->keyLen > 4) && (p_H->keyAtStart == 0)) ? p_H->keyLen - 4 : 0);
  memcpy(&hashVal, keyStart, p_H->keyLen >= 4 ? 4 : p_H->keyLen);
  return (hashVal % p_H->numBuckets);
#endif

  register L7_uint32 nleft = p_H->keyLen;
  const    L7_ushort16 *w = (L7_ushort16*) key;
  register L7_uint32 sum = 0;

  while (nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }
  if(nleft == 1)
    sum += osapiHtons(*(L7_uchar8 *)w << 8);
  sum = (sum >> 16) + (sum & 0xffff);     /* add hi 16 to low 16 */

  return (sum % p_H->numBuckets);
}

/*********************************************************************
* @purpose  If hash list encapsulates entries on DLL, allocated a
*           pool of DLL containers.
*
* @param    None.
*
* @returns  L7_SUCCESS if ok
*           L7_FAILURE otherwise
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t hlBufferPoolCreate(l7_hl_t *p_H, L7_uint32 num_buffers)
{
  L7_uint32 buffer_size = sizeof(hl_value_t);

  if (bufferPoolInit(num_buffers, buffer_size, "HL Pool",
                     &p_H->bufferPoolId) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_EMERGENCY, p_H->compId,
            "Failed to allocate buffer pool for hash list.");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose           Delete all entries from the hash list. Free memory
*                    for each entry if hash list was created with the
*                    L7_HL_FREE_ON_DEL option.
*
* @param  objId      @b{(input)}  hash list pointer returned by hlInit().
*
* @returns           L7_SUCCESS
*                    L7_FAILURE
*
* @notes             Doesn't delete the hash list itself.
*
* @end
* ********************************************************************/
L7_RC_t hlCleanup(l7_hl_t *p_H)
{
  hl_value_t *value, *next_value;
  L7_ushort16    index;

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (index = 0; (index < p_H->numBuckets) && p_H->numEntries; index ++)
  {
    value = p_H->entry[index];
    while (value)
    {
      next_value = value->next;

      if (p_H->delData)
        osapiFree(p_H->compId, GET_VALUE(p_H, value));

      if (p_H->valContainer)
        bufferPoolFree(p_H->bufferPoolId, (L7_uchar8*) value);

      p_H->numEntries -= 1;
      p_H->entry[index] = L7_NULLPTR;
      value = next_value;
    }
  }

  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}


/******************* start of public API implmentation ********************/


/*********************************************************************
 * @purpose          Initialize HL object.
 *
 * @param  compId     @b{(input)}  component ID of component creating HL
 * @param  flags      @b{(input)}  control flags
 * @param  keyLen     @b{(input)}  key length in bytes
 * @param  keyOffset  @b{(input)}  For entries in the hash list, the number of bytes from
 *                                 the beginning of the item to the beginning of the key.
 * @param  tableSize  @b{(input)}  number of hash buckets. Should be a prime
 *                                 number. Suggested values are L7_HL_SMALL_TABLE_SIZE
 *                                 and L7_HL_LARGE_TABLE_SIZE, but there is no
 *                                 restriction against using other values.
 * @param  maxEntries @b{(input)}  maximum number of entries. Only enforced if
 *                                 L7_HL_VAL_CONTAINER flag set.
 * @param  p_objId    @b{(output)} pointer to put hash list objectId
 *
 * @returns           L7_SUCCESS      success
 * @returns           L7_NO_MEMORY    can't allocate memory
 * @returns           L7_ERROR        wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
L7_RC_t hlInit(L7_COMPONENT_IDS_t compId, e_HL_flags_t flags, L7_uchar8 keyLen,
               L7_ushort16 keyOffset, L7_ushort16 tableSize, L7_uint32 maxEntries,
               void **p_objId)
{
  l7_hl_t *p_H;

  if ((keyLen == 0) || (tableSize == 0) || !p_objId)
    return L7_ERROR;

  if ((flags & L7_HL_VAL_CONTAINER) && (maxEntries == 0))
    return L7_ERROR;

  /* allocate object header memory */
  if ((p_H = (l7_hl_t *)osapiMalloc(compId, sizeof(l7_hl_t))) == L7_NULLPTR)
    return L7_NO_MEMORY;

  memset (p_H, 0, sizeof(l7_hl_t));

  p_H->numBuckets = tableSize;
  p_H->maxEntries = maxEntries;
  p_H->delData   = (L7_BOOL)((flags & L7_HL_FREE_ON_DEL)? L7_TRUE: L7_FALSE);
  p_H->keyLen = keyLen;
  p_H->keyOffset = keyOffset;
  p_H->compId = compId;
  p_H->semId = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (p_H->semId == L7_NULLPTR)
  {
    osapiFree(compId, p_H);
    return L7_NO_MEMORY;
  }

  if (flags & L7_HL_VAL_CONTAINER)
  {
    p_H->valContainer = L7_TRUE;

    /* create pool of containers for DLLs */
    if (hlBufferPoolCreate(p_H, maxEntries) != L7_SUCCESS)
    {
      osapiSemaDelete(p_H->semId);
      osapiFree(compId, p_H);
      return L7_NO_MEMORY;
    }
  }

  /* allocate memory for the entries table */
  p_H->entry = (hl_value_t **)osapiMalloc(compId,
                                          (sizeof(hl_value_t **) * p_H->numBuckets));
  if (p_H->entry == L7_NULLPTR)
  {
    if (p_H->valContainer)
    {
      bufferPoolTerminate(p_H->bufferPoolId);
    }
    osapiSemaDelete(p_H->semId);
    osapiFree(compId, p_H);
    return L7_NO_MEMORY;
  }

  memset(p_H->entry, 0, sizeof(hl_value_t *) * p_H->numBuckets);
  *p_objId = p_H;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose            destroy HL object.
*
* @param  p_objId     @b{(input)}  hash list pointer returned by hlInit().
*
* @returns            L7_SUCCESS        success
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlDestroy(void *p_objId)
{
  l7_hl_t *p_H = (l7_hl_t *)p_objId;

  if (!p_H)
  {
    return L7_ERROR;
  }

  if (p_H->valContainer || p_H->delData)
    /* need to free memory associated with entries in the list */
    hlCleanup(p_H);

  if (p_H->valContainer)
  {
    bufferPoolTerminate(p_H->bufferPoolId);
  }
  osapiSemaDelete(p_H->semId);
  osapiFree(p_H->compId, p_H->entry);
  osapiFree(p_H->compId, p_H);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose          Insert new entry to the hash list.
*
*
* @param  objId     @b{(input)}  hash list pointer returned by hlInit().
* @param  key       @b{(input)}  key pointer
* @param  value     @b{(input)}  value pointer
*
* @returns          L7_SUCCESS     success
* @returns          L7_ERROR       wrong parameters
* @returns          L7_NO_MEMORY  failed to allocate memory
*                   L7_TABLE_IS_FULL if number of elements already at maximum
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlInsert(void *objId, L7_uchar8 *key, void *value)
{
  l7_hl_t *p_H = (l7_hl_t *)objId;
  hl_value_t *p_V = (hl_value_t *)value;
  L7_ushort16 index;

  if (!p_H)
    return L7_FAILURE;

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (p_H->numEntries == p_H->maxEntries)
  {
    osapiSemaGive(p_H->semId);
    return L7_TABLE_IS_FULL;
  }

  index = hlGetIndex(p_H, key);

  /* allocate entry memory if necessary */
  if (p_H->valContainer)
  {
    if (bufferPoolAllocate(p_H->bufferPoolId, (L7_uchar8**) &p_V) != L7_SUCCESS)
    {
      osapiSemaGive(p_H->semId);
      return L7_NO_MEMORY;
    }
    p_V->value = value;
  }

#ifdef HL_DEBUG
  /* check may be this same value is included already */
  else
  {
    hl_value_t *ctr = p_H->entry[index];
    for (;ctr != L7_NULLPTR; ctr = ctr->next)
    {
      if (ctr == p_V)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, p_H->compId,
               "Attempt to insert duplicate entry into hash list.");
        return L7_FAILURE;
      }
    }
  }
#endif

  /* add value to the entry */
  p_V->prev = p_V->next = L7_NULLPTR;
  hlAddToDLList(&p_H->entry[index], p_V);
  p_H->numEntries++;
  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose          Delete entry from the hash list.
*
* @param  objId     @b{(input)}  hash list pointer returned by hlInit().
* @param  key       @b{(input)}  key pointer
* @param  value     @b{(output)}  pointer to put value or L7_NULLPTR. If
*                                 free on delete option is used, this
*                                 is always set to NULL on return.
*
* @returns   L7_SUCCESS     success
*            L7_FAILURE
*            L7_ERROR       bad parameters
*            L7_NOT_EXIST   entry wasn't found
*
* @notes     Removes the first entry that matches the key.
*            Use this form of delete if you need a pointer to the
*            item removed from the hash list. This delete form is
*            useful for hash lists created without the
*            L7_HL_FREE_ON_DEL option, where the caller may need to get a
*            pointer to the object so its memory can be freed.
*
* @end
* ********************************************************************/
L7_RC_t hlDeleteFirst(void *objId, L7_uchar8 *key, void **value)
{
  l7_hl_t    *p_H = (l7_hl_t *)objId;
  hl_value_t *p_V;
  L7_ushort16 index;

  if (!p_H)
    return L7_ERROR;

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  index = hlGetIndex(p_H, key);

  /* find given entry */
  p_V = p_H->entry[index];
  while (p_V)
  {
    if (memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
      break;
    p_V = p_V->next;
  }
  if (!p_V)
  {
    osapiSemaGive(p_H->semId);
    return L7_NOT_EXIST;
  }

  if (value)
    *value = GET_VALUE(p_H, p_V);

  /* delete value */
  if (p_H->curValue == GET_VALUE(p_H, p_V))
  {
    p_H->curValue = L7_NULLPTR;
  }
  if (p_H->getValue == GET_VALUE(p_H, p_V))
  {
    p_H->getValue = L7_NULLPTR;
  }
  hlDelFromDLList(&p_H->entry[index], p_V);

  if (p_H->delData)
  {
    /* user asked HL to free data on delete */
    osapiFree(p_H->compId, GET_VALUE(p_H, p_V));
    if (value)
      *value = L7_NULLPTR;
  }
  if (p_H->valContainer)
  {
    /* return container to buffer pool */
    bufferPoolFree(p_H->bufferPoolId, (L7_uchar8*) p_V);
  }

  p_H->numEntries--;

  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose        Remove an entry from the hash list.
*
* @param objId    @b{(input)}  hash list pointer returned by hlInit().
* @param key      @b{(input)}  key pointer
* @param value    @b{(input)}  if NULL, first entry matching the key is
*                              deleted. If non-NULL, will delete this
*                              specific item.
*
* @returns    L7_SUCCESS     success
*             L7_ERROR       bad parameters
*             L7_FAILURE
*             L7_NOT_EXIST   entry wasn't found
*
* @notes      Use this form of delete if you want to identify the specific
*             object to be removed from the hash list.
*
* @end
* ********************************************************************/
L7_RC_t hlDelete(void *objId, L7_uchar8 *key, void *value)
{
  l7_hl_t    *p_H = (l7_hl_t *)objId;
  hl_value_t *p_V;
  L7_ushort16 index;

  if (!p_H)
  {
    return L7_ERROR;
  }

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  index = hlGetIndex(p_H, key);

  /* find given entry */
  p_V = p_H->entry[index];
  while (p_V)
  {
    if (memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
    {
      if (!value || (value == GET_VALUE(p_H, p_V)))
        break;
    }
    p_V = p_V->next;
  }
  if (!p_V)
  {
    osapiSemaGive(p_H->semId);
    return L7_NOT_EXIST;
  }

  /* delete value */
  if (p_H->curValue == GET_VALUE(p_H, p_V))
  {
    p_H->curValue = L7_NULLPTR;
  }
  if (p_H->getValue == GET_VALUE(p_H, p_V))
  {
    p_H->getValue = L7_NULLPTR;
  }
  hlDelFromDLList(&p_H->entry[index], p_V);
  if (p_H->delData)
  {
    /* user asked HL to free data on delete */
    osapiFree(p_H->compId, GET_VALUE(p_H, p_V));
  }
  if (p_H->valContainer)
  {
    /* return container to buffer pool */
    bufferPoolFree(p_H->bufferPoolId, (L7_uchar8*) p_V);
  }

  p_H->numEntries--;

  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose          Find first entry with the given key
*
* @param  objId     @b{(input)}  hash list pointer returned by hlInit().
* @param  key       @b{(input)}  key pointer
* @param  p_value   @b{(output)}  pointer to put value or L7_NULLPTR
*
* @returns   L7_SUCCESS    success
*            L7_ERROR      wrong parameters
*            L7_FAILURE
*            L7_NOT_EXIST  no matching entries
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlFindFirst(void *objId, L7_uchar8 *key, void **p_value)
{
  l7_hl_t    *p_H = (l7_hl_t *)objId;
  hl_value_t *p_V;
  L7_ushort16 index;

  if (!p_H)
  {
    return L7_ERROR;
  }

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  index = hlGetIndex(p_H, key);
  p_V = p_H->entry[index];

  while (p_V)
  {
    if (memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
      break;
    p_V = p_V->next;
  }
  if (!p_V)
  {
    osapiSemaGive(p_H->semId);
    return L7_NOT_EXIST;
  }
  if (p_value != L7_NULLPTR)
    *p_value = GET_VALUE(p_H, p_V);
  p_H->curValue = p_V;
  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose          Find next entry from the given value.
*
* @param  objId     @b{(input)}  hash list pointer returned by hlInit().
* @param  key       @b{(input)}  key pointer
* @param  curValue  @b{(input)}  start from this entry
* @param  p_value   @b{(input)}  pointer to put the value or L7_NULLPTR
*
* @returns   L7_SUCCESS  success
*            L7_ERROR    wrong parameters
*            L7_FAILURE
*            L7_NOT_EXIST   no more matching entries
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlFindNext(void *objId, L7_uchar8 *key, void *curValue, void **p_value)
{
  l7_hl_t    *p_H = (l7_hl_t *)objId;
  hl_value_t *p_V;
  L7_ushort16 index;

  if (!p_H || !key || !curValue)
  {
    return L7_ERROR;
  }

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Check for several conditions:
   * a) User passed the wrong previous entry
   * b) User changed the key
   * c) There are multiple iterations in progress and p_H->curValue is
   *    for the other iterator
   * d) curValue has been deleted from the hash list
   * e) curValue was deleted from the hash list and the same chunk
   *    of memory was added with new data, including a new key
   */
  if ((curValue != (void *)GET_VALUE(p_H, p_H->curValue)) ||
      (memcmp(key, GET_KEY(p_H, p_H->curValue), p_H->keyLen) != 0))
  {
    /* find current value */
    index = hlGetIndex(p_H, key);
    p_V = p_H->entry[index];

    while (p_V)
    {
      if ((memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0) &&
          (GET_VALUE(p_H, p_V) == curValue))
      {
        p_H->curValue = p_V;
        break;
      }
      p_V = p_V->next;
    }
    if (!p_V)
    {
      /* could not find current value */
      osapiSemaGive(p_H->semId);
      return L7_ERROR;
    }
  }
  else
    p_V = p_H->curValue;

  p_V = p_V->next;

  /* find next value */
  while (p_V)
  {
    if (memcmp(key, GET_KEY(p_H, p_V), p_H->keyLen) == 0)
    {
      p_H->curValue = p_V;
      break;
    }
    p_V = p_V->next;
  }
  if (!p_V)
  {
    osapiSemaGive(p_H->semId);
    return L7_NOT_EXIST;          /* no more matching entries */
  }
  if (p_value != L7_NULLPTR)
    *p_value = GET_VALUE(p_H, p_V);
  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}



/*********************************************************************
* @purpose               returns number of entries in a hash list
*
* @param  objId          @b{(input)}  hash list pointer returned by hlInit().
*
* @returns               L7_SUCCESS        success
*
* @notes
*
* @end
* ********************************************************************/
L7_ushort16 hlNumEntriesGet(void *objId)
{
  l7_hl_t *p_H = (l7_hl_t *)objId;
  L7_ushort16 numEntries;

  if (!p_H)
    return 0;

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return 0;

  numEntries = p_H->numEntries;

  osapiSemaGive(p_H->semId);
  return numEntries;
}


/*********************************************************************
* @purpose            Get first entry from the hash list
*
* @param  objId       @b{(input)}  hash list pointer returned by hlInit().
* @param  p_value     @b{(output)}  pointer to put the value
*
* @returns            L7_SUCCESS    success
*                     L7_ERROR      bad parameters
*                     L7_FAILURE    hash list is empty
*
* @notes
*
* @end
* ********************************************************************/
L7_RC_t hlGetFirst(void *objId, void **p_value)
{
  l7_hl_t *p_H = (l7_hl_t *)objId;

  if (!p_H)
    return L7_ERROR;

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  p_H->getIndex = 0;
  if (!p_H->numEntries)
  {
    /* hash list is empty */
    p_H->getValue = L7_NULLPTR;
    osapiSemaGive(p_H->semId);
    return L7_FAILURE;
  }
  /* find first bucket with non-NULL head */
  for (p_H->getValue = p_H->entry[p_H->getIndex];
        p_H->getIndex < p_H->numBuckets;
        p_H->getIndex++)
  {
    p_H->getValue = p_H->entry[p_H->getIndex];
    if (p_H->getValue)
      break;
  }
  if (p_H->getIndex >= p_H->numBuckets)
  {
    /* Shouldn't hit this since we checked above for an empty list. */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, p_H->compId,
            "getIndex %u invalid for hash list with %u buckets.",
            p_H->getIndex, p_H->numBuckets);
    return L7_ERROR;
  }
  *p_value = GET_VALUE(p_H, p_H->getValue);

  osapiSemaGive(p_H->semId);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose          Get next entry in hash list.
*
*
* @param  objId     @b{(input)}   hash list pointer returned by hlInit().
* @param  curValue  @b{(input)}   give next value after this one. Must be non-NULL.
* @param  p_value   @b{(output)}  pointer to put the value
*
* @returns          L7_SUCCESS    success
*                   L7_ERROR      invalid params
*                   L7_FAILURE
*                   L7_NOT_EXIST  end of the hash list
*
* @notes            p_H->getValue points to the last item returned from
*                   hlGetFirst() or hlGetNext(), and p_H->getIndex is the
*                   index of that items's hash bucket.
*
* @end
* ********************************************************************/
L7_RC_t hlGetNext(void *objId, void *curValue, void **p_value)
{
  l7_hl_t *p_H = (l7_hl_t *)objId;

  if (!p_H || !curValue)
    return L7_ERROR;

  if (osapiSemaTake(p_H->semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  /* Use cached current pointer if it exists, if it is the same object
   * that the user has passed as the current value, and if its key matches
   * the key of the object we last returned. */
  if ((p_H->getValue == L7_NULLPTR) ||
      (GET_VALUE(p_H, p_H->getValue) != curValue))
  {
    /* cached current value doesn't match the one given by the caller.
     * So walk entire hash list to find caller's current value. */
    for (p_H->getIndex = 0; p_H->getIndex < p_H->numBuckets; p_H->getIndex++)
    {
      p_H->getValue = p_H->entry[p_H->getIndex];
      while (p_H->getValue)
      {
        if (GET_VALUE(p_H, p_H->getValue) == (hl_value_t *)curValue )
          break;
        p_H->getValue = p_H->getValue->next;
      }
      if (p_H->getValue)
        break;
    }
    if (!p_H->getValue)
    {
      *p_value = L7_NULLPTR;
      osapiSemaGive(p_H->semId);
      return L7_ERROR;    /* could not find previous entry */
    }
  }

  /* see if there are more entries in DLL for same hash bucket */
  p_H->getValue = p_H->getValue->next;
  if (p_H->getValue != L7_NULLPTR)
  {
    *p_value = GET_VALUE(p_H, p_H->getValue);
    osapiSemaGive(p_H->semId);
    return L7_SUCCESS;
  }
  /* go to next hash bucket */
  p_H->getIndex++;
  for (p_H->getValue = L7_NULLPTR;
        p_H->getIndex < p_H->numBuckets; p_H->getIndex++)
  {
    if ((p_H->getValue = p_H->entry[p_H->getIndex]) != L7_NULLPTR)
      break;
  }

  if (p_H->getValue)
  {
    *p_value = GET_VALUE(p_H, p_H->getValue);
    osapiSemaGive(p_H->semId);
    return L7_SUCCESS;
  }
  /* this is the end of the hash list */
  *p_value = L7_NULLPTR;
  p_H->getIndex = 0;

  osapiSemaGive(p_H->semId);
  return L7_NOT_EXIST;
}

/**************** Debug utilities *****************/

void hlDebugShow(void *objId)
{
  l7_hl_t    *p_H = (l7_hl_t *)objId;
  L7_uchar8 compName[64];

  if (!p_H)
  {
    printf("\nNeed hash list pointer as sole argument.");
    return;
  }

  if (cnfgrApiComponentNameGet(p_H->compId, compName) == L7_SUCCESS)
  {
    printf("\nHL owned by component %s", compName);
  }
  printf("\nNumber of buckets:  %u", p_H->numBuckets);
  printf("\nMaximum entries:  %u", p_H->maxEntries);
  printf("\nCurrent number of entries:  %u", p_H->numEntries);
  if (p_H->delData)
    printf("\nFree entry on delete");
  if (p_H->valContainer)
    printf("\nUse entry containers for next, prev pointers");
  printf("\nKey length (bytes):  %u", p_H->keyLen);
  printf("\nKey offset (bytes):  %u", p_H->keyOffset);
  printf("\nGet index:  %u", p_H->getIndex);
  printf("\nGet value:  %#x", (L7_uint32) p_H->getValue);
  printf("\nCurrent value:  %#x", (L7_uint32) p_H->curValue);
  printf("\nSemaphore:  %#x", (L7_uint32) p_H->semId);
  printf("\nBuffer pool ID:  %#x", p_H->bufferPoolId);

  printf("\nMemory use:");
  printf("\n  Hash list:  %u bytes", sizeof(l7_hl_t));
  printf("\n  Hash bucket array:  %u bytes", sizeof(hl_value_t **) * p_H->numBuckets);
  if (p_H->valContainer)
  {
    printf("\n  Val container buffer pool:  %u bytes",
           bufferPoolSizeCompute(p_H->maxEntries, sizeof(hl_value_t)));
  }

  {
    /* print some stats to indicate how well hash function is doing */
    L7_ushort16 i, j;
    L7_uint32 dllLength[p_H->numBuckets];
    hl_value_t *value;
    L7_uint32 sumOfLengths = 0;
    /* avoiding floating point math here */
    L7_uint32 avgLength;
    L7_uint32 minLength = 0xFFFFFFFF;
    L7_uint32 maxLength = 0;
    L7_uint32 sumOfSquares = 0;
    L7_uint32 variance;

    /* find number of items per hash bucket */
    for (i = 0; i < p_H->numBuckets; i++)
    {
      j = 0;
      value = p_H->entry[i];
      while (value)
      {
        j++;
        value = value->next;
      }
      dllLength[i] = j;
      sumOfLengths += j;
      sumOfSquares += (j * j);
      if (j > maxLength)
        maxLength = j;
      if (j < minLength)
        minLength = j;
    }

    /* Compute stats */
    avgLength = sumOfLengths / p_H->numBuckets;
    variance = (sumOfSquares - (p_H->numBuckets * avgLength * avgLength)) / p_H->numBuckets;

    printf("\nMin entries per bucket:  %u", minLength);
    printf("\nMax entries per bucket:  %u", maxLength);
    printf("\nAverage entries per bucket:  %u", avgLength);
    printf("\nVariance of bucket lengths:  %u", variance);
  }
}



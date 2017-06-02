/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename timerange.c
*
* @purpose time ranges component internal functions
*
* @component Time Ranges
*
* @comments This file contains implementation functions that touch the
*           actual time ranges data structures. Functions in this file should
*           not call back out to functions timeRange_api.c 
*           since this risks creating deadlocks by attempting
*           to take the read/write lock more than once.
*
* @create 11/05/2009
*
* @author Siva Mannem
*
* @end
*
**********************************************************************/

#include <string.h>
#include <ctype.h>
#include "comm_mask.h"
#include "l7utils_api.h"
#include "buff_api.h"
#include "simapi.h"
#include "timerange.h"
#include "timerange_api.h"
#include "timerange_debug.h"
#include "timerange_cfg.h"
#include "timerange_control.h"

extern osapiRWLock_t           timeRangeRwLock;
extern timeRangeCfgFileData_t  *timeRangeCfgFileData;
extern timeRangeCnfgrState_t   timeRangeCnfgrState;
extern L7_uchar8               *timeRangeEventNames[];
extern L7_uint32               timeRangeBufferPoolId;

avlTree_t                      timeRangeTree;
avlTree_t                      *pTimeRangeTree = &timeRangeTree;
avlTreeTables_t                *timeRangeTreeHeap = L7_NULLPTR;
timeRangeStructure_t           *timeRangeDataHeap = L7_NULLPTR;
L7_uint32                      timeRange_max_entries    = L7_TIMERANGE_MAX_NUM;
L7_uint32                      timeRange_curr_entries_g = 0;    /* total current list entries */

/* Number of bytes in mask */
#define TIMERANGE_INDEX_INDICES \
         (((L7_TIMERANGE_MAX_NUM + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[TIMERANGE_INDEX_INDICES];
} TIMERANGE_INDEX_MASK_t;

static char *timeRangeEntryCfgMaskStr[] =
{
  "---", "EntryNum", "StartTime", "EndTime"
};

/* NOTE: Index 0 is reserved and must not be used */
static TIMERANGE_INDEX_MASK_t  timeRangeIndexInUseMask;
static L7_uint32               timeRangeIndexNextFree = 0;         /* next available time range index   */

/*********************************************************************
*
* @purpose  To create a timerange tree.
*
* @param    maxTimeRangeNum  @b{(input)} number of nodes to create in 
*                                        timerange tree
*
* @returns  void
*
* @comments Creates a new instance of timerange, allocating space for up to
*           maxTimeRangeNum timeranges.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeCreateTree(L7_uint32 maxTimeRangeNum)
{
  /* check validity of 'maxTimeRangeNum' */
  if ((maxTimeRangeNum == 0) || (maxTimeRangeNum > L7_TIMERANGE_MAX_NUM))
  {
    timeRange_max_entries = L7_TIMERANGE_MAX_NUM;
  }
  else
  {
    timeRange_max_entries = maxTimeRangeNum;
  }

  timeRangeTreeHeap = (avlTreeTables_t *)osapiMalloc(L7_TIMERANGES_COMPONENT_ID,
                              timeRange_max_entries * sizeof(avlTreeTables_t));
  timeRangeDataHeap = (timeRangeStructure_t *)osapiMalloc(L7_TIMERANGES_COMPONENT_ID,
                              timeRange_max_entries * sizeof(timeRangeStructure_t));

  if ((timeRangeTreeHeap == L7_NULLPTR) ||
     (timeRangeDataHeap == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  avlCreateAvlTree(&timeRangeTree, timeRangeTreeHeap, timeRangeDataHeap, timeRange_max_entries, 
                   (L7_uint32)sizeof(timeRangeStructure_t), 0x10, (L7_uint32)sizeof(L7_uint32));
  (void)avlSetAvlTreeComparator(&timeRangeTree, avlCompareULong32);

  timeRangeImpIndexInUseInit();

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To delete a timerange tree.
*
* @param    void
*
* @returne  void
*
* @comments Destroys the instance of timerange, releasing the previously
*           allocated database and control space.
*
* @end
*
*********************************************************************/
void timeRangeDeleteTree(void)
{
  avlPurgeAvlTree(&timeRangeTree, timeRange_max_entries);

  /* reset all timerange index in-use table entries */
  memset(&timeRangeIndexInUseMask, 0, sizeof(timeRangeIndexInUseMask));

  /* reset timerange index next value */
  timeRangeIndexNextFree = 0;
}

/*********************************************************************
*
* @purpose  Provide the current value of the timerange index next variable
*
* @param    *next       @b{(output)} timerange index next value
*
* @returns  void
*
* @comments Only provides an output value if the next parm is non-null.
*
* @comments Does not guarantee this index value will be valid at the time
*           an timerange create is attempted.
*
* @comments Assumes caller manages timerange write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void timeRangeImpIndexNextRead(L7_uint32 *next)
{
  if (next != L7_NULLPTR)
  {
    *next = timeRangeIndexNextFree;
  }
}

/*********************************************************************
*
* @purpose  Initialize in-use table entry for a timerange
*
* @param    void
*
* @returns  void  
*
* @comments Assumes caller manages timerange write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void timeRangeImpIndexInUseInit(void)
{
  /* initialize the timerange index in-use table */
  memset(&timeRangeIndexInUseMask, 0, sizeof(timeRangeIndexInUseMask));

  /* establish initial timerange index next values */
  timeRangeImpIndexNextUpdate();
  return;
}

/*********************************************************************
*
* @purpose  Update in-use table entry for a timerange
*
* @param    timeRangeIndex    @b{(input)} timerange index
* @param    inUse             @b{(input)} mark list in use or not
*
* @returns  void  
*
* @comments Assumes caller manages timerange write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void timeRangeImpIndexInUseUpdate(L7_uint32 timeRangeIndex, L7_BOOL inUse)
{
  if (L7_TRUE == inUse)
  {
    L7_INTF_SETMASKBIT(timeRangeIndexInUseMask, timeRangeIndex);
  }
  else
  {
    L7_INTF_CLRMASKBIT(timeRangeIndexInUseMask, timeRangeIndex);
  }
  return;
}

/*********************************************************************
*
* @purpose  Determine next available timerange index value and 
*           update the 'IndexNext' value
*
* @param    void
*
* @returns  void  
*
* @comments Always looks for first index not already in use, starting with 1.
*
* @comments Assumes caller manages timerange write semaphore, if needed.
*       
* @end
*
*********************************************************************/
void timeRangeImpIndexNextUpdate(void)
{
  L7_uint32     i, imax;

  imax = timeRange_max_entries;

  for (i = 1; i <= imax; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a timerange using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(timeRangeIndexInUseMask, i) == L7_FALSE)
    {
      break;
    }
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > imax)
  {
    timeRangeIndexNextFree = 0;
  }
  else
  {
    timeRangeIndexNextFree = (i - 1) + L7_TIMERANGE_MIN_INDEX;
  }
}

/*********************************************************************
*
* @purpose  Get the next sequential timerange index
*
* @param    timeRangeIndex    @b{(input)}  timerange index to begin search
* @param    *next             @b{(output)} next timerange index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no timeranges exist, or timeRangeIndex is the last
* @returns  L7_FAILURE
*
* @comments This function is also used for a 'getFirst' search by 
*           specifying an timeRangeIndex value of 0.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpIndexGetNext(L7_uint32 timeRangeIndex, L7_uint32 *next)
{
  L7_RC_t              rc;
  timeRangeStructure_t *p;

  p = (timeRangeStructure_t *)avlSearchLVL7(&timeRangeTree,
                                           &timeRangeIndex, L7_MATCH_GETNEXT);
  
  if (p == L7_NULLPTR)
  {
    rc = L7_ERROR;
  }
  else 
  {
    *next = p->timeRangeIndex;
    rc = L7_SUCCESS;
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if timeRangeIndex is a valid, configured  timerange
*
* @param    timeRangeIndex    @b{(input)}  timerange index to begin search
*
* @returns  L7_SUCCESS   timerange is valid
* @returns  L7_FAILURE   timerange not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpIndexCheckValid(L7_uint32 timeRangeIndex)
{
  L7_RC_t              rc = L7_SUCCESS;
  timeRangeStructure_t *p;

  p = (timeRangeStructure_t *)avlSearchLVL7(&timeRangeTree,
                                            &timeRangeIndex, L7_MATCH_EXACT); 
  if (p == L7_NULLPTR)
  {
    rc = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the timerange index is within proper range
*
* @param    timeRangeIndex    @b{(input)}  timerange index to begin search
*
* @returns  L7_SUCCESS   timerange index is in range
* @returns  L7_FAILURE   timerange index out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpIndexRangeCheck(L7_uint32 timeRangeIndex)
{
  L7_RC_t       rc = L7_FAILURE;

  if ((timeRangeIndex >= L7_TIMERANGE_MIN_INDEX) &&
         (timeRangeIndex <= L7_TIMERANGE_MAX_INDEX))
  {
    rc = L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To find a timerange identified by its timeRangeIndex.
*
* @param    timeRangeIndex    @b{(input)}  timerange index
*
* @returns  void *      timerange element pointer
* @returns  L7_NULLPTR  timerange element not found for this timeRangeIndex
*
* @comments
*
* @end
*
*********************************************************************/
void * timeRangeFindTimeRange(L7_uint32 timeRangeIndex)
{
  return avlSearchLVL7(&timeRangeTree, &timeRangeIndex, L7_MATCH_EXACT);
}

/*********************************************************************
*
* @purpose  Checks if specified timerange entry field is configured
*
* @param    *timeRange_ptr    @b{(input)}  timerange element pointer
* @param    entrynum     @b{(input)}  current entry number
* @param    field       @b{(input)}  entry field of interest
*
* @returns  L7_TRUE     entry field is configured
* @returns  L7_FALSE    entry field not configured
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeIsEntryFieldConfigured(void *timeRange_ptr, L7_uint32 entrynum, L7_uint32 field)
{
  L7_BOOL               found = L7_FALSE;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr;
  L7_uint32             result;

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  curr = ptr->head;

  while (curr != L7_NULLPTR)
  {
    if (entrynum == curr->entryNum)
    {
      found = L7_TRUE;
      break;
    }
    curr = curr->next;
  }

  if (found == L7_TRUE)
  {
    result = (curr->configMask) & (1 << field);
    if (result == 0)
    {
      found = L7_FALSE;
    }
  }

  return found;
} 


/*********************************************************************
*
* @purpose  To create a new timerange.
*
* @param    timeRangeIndex    @b{(input)} timerange index
*
* @returns  L7_SUCCESS  timerange created
* @returns  L7_FAILURE  invalid timeRange identifier, all other failures
* @returns  L7_ERROR    timerange already exists
* @returns  L7_TABLE_IS_FULL  maximum number of timeranges already created
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpCreate(L7_uint32 timeRangeIndex)
{
  L7_RC_t           rc;
  timeRangeStructure_t timeRange, *ptr;

  memset(&timeRange, 0, sizeof(timeRange));

  timeRange.timeRangeIndex = timeRangeIndex;
  timeRange.status = TIMERANGE_STATUS_ACTIVE; 
  timeRange.head = L7_NULLPTR;
  timeRange.nextTimeRange = L7_NULLPTR;

  ptr = avlInsertEntry(&timeRangeTree, &timeRange);
  if (ptr == L7_NULLPTR)
  {
    timeRange_curr_entries_g++;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    /* update the 'indexNext' value now that a new timerange was created */
    timeRangeImpIndexInUseUpdate(timeRangeIndex, L7_TRUE);
    timeRangeImpIndexNextUpdate();

    timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  }

  return rc;
}


/*********************************************************************
*
* @purpose  To delete the specified timerange
*
* @param    *timeRange_ptr    @b{(input)} timerange element pointer   
*
* @returns  L7_SUCCESS  timerange deleted
* @returns  L7_FAILURE  invalid timerange index, all other failures
* @returns  L7_ERROR    timerange does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpDelete(void *timeRange_ptr)
{
  L7_uint32                   timeRangeIndex;
  L7_uint32                   entry;
  L7_RC_t                     rc;
  timeRangeStructure_t        *ptr;

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  timeRangeIndex = ptr->timeRangeIndex;

  /* remove all timerange entries */
  while (timeRangeImpEntryGetFirst(timeRangeIndex, &entry) == L7_SUCCESS)
  {
    rc = timeRangeImpEntryRemove(ptr, entry);
  }

  ptr = avlDeleteEntry(&timeRangeTree, timeRange_ptr);
  if (ptr == L7_NULLPTR) /* item not found in AVL tree */
  {
    rc = L7_FAILURE;
  }
  else /* deleted */
  {
    if (timeRange_curr_entries_g > 0)
    {
      timeRange_curr_entries_g--;
    }

    /* update the 'indexNext' value now that this Time Range was deleted */
    timeRangeImpIndexInUseUpdate(timeRangeIndex, L7_FALSE);
    timeRangeImpIndexNextUpdate();

    rc = L7_SUCCESS;
  }

  timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  
  return rc;
}


/*********************************************************************
*
* @purpose  To add the name to this time range.
*
* @param    *timeRange_ptr    @b{(input)} time range element pointer
* @param    *name             @b{(input)} time range name
*                                            
* @returns  L7_SUCCESS  name is added
* @returns  L7_FAILURE  invalid time range index, invalid name, or other failure
* @returns  L7_ERROR    time range does not exist
*
* @comments Does not check for name validity (e.g. alphanumeric string).
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpNameAdd(void *timeRange_ptr, L7_uchar8 *name)
{
  timeRangeStructure_t *ptr;
  L7_uint32         nameLen;

  if ((timeRange_ptr == L7_NULLPTR) || (name == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_TIMERANGE_NAME_LEN_MIN) || (nameLen > L7_TIMERANGE_NAME_LEN_MAX))
  {
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t*)timeRange_ptr;
  osapiStrncpySafe((char *)ptr->timeRangeName, (char *)name,
                                               sizeof(ptr->timeRangeName));
  timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To get the name of this time range.
*
* @param    *timeRange_ptr    @b{(input)}  time range element pointer
* @param    *name             @b{(output)} time range name
*                                            
* @returns  void
*
* @comments Only outputs a value if the timeRange_ptr and name parms are both non-null.
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_TIMERANGE_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
void timeRangeImpNameGet(void *timeRange_ptr, L7_uchar8 *name)
{
  timeRangeStructure_t *ptr;

  if ((timeRange_ptr != L7_NULLPTR) && (name != L7_NULLPTR))
  {
    ptr = (timeRangeStructure_t*)timeRange_ptr;
    osapiStrncpySafe((char *)name, (char *)ptr->timeRangeName,
                                   sizeof(ptr->timeRangeName));
  }
}

/*********************************************************************
*
* @purpose  To get the index of a time range, given its name.
*
* @param    *name            @b{(input)} time range name
* @param    *timeRangeIndex  @b{(output)} time range index
*                                            
* @returns  L7_SUCCESS  time range index is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    time range name does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpNameToIndexGet(L7_uchar8 *name, L7_uint32 *timeRangeIndex)
{
  L7_RC_t           rc;
  timeRangeStructure_t *ptr;
  L7_uint32         nameLen;
  L7_uint32         i;

  /* check inputs */
  if ((name == L7_NULLPTR) || (timeRangeIndex == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_TIMERANGE_NAME_LEN_MIN) ||
      (nameLen > L7_TIMERANGE_NAME_LEN_MAX))
  {
    return L7_FAILURE;
  }
  /* search through all time ranges looking for this name */
  i = 0;
  rc = timeRangeImpIndexGetNext(i, &i);

  while (rc == L7_SUCCESS)
  {
    ptr = (timeRangeStructure_t *)timeRangeFindTimeRange(i);
    if (ptr == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    if (strcmp((char *)ptr->timeRangeName, (char *)name) == 0)
    {
      /* found list name */
      *timeRangeIndex = i;
      return L7_SUCCESS;
    }

    rc = timeRangeImpIndexGetNext(i, &i);

  } /* endwhile */
  
  /* time range by this name does not exist */
  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  To get the name of a time range, given its index
*
* @param    *timeRangeIndex   @b{(input)}  time range index
* @param    *name       @b{(output)} time range name
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    time range does not exist
*
* @comments Assumes caller provides a name buffer of at least 
*           (L7_TIMERANGE_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpNameFromIndexGet(L7_uint32 timeRangeIndex, L7_uchar8 *name)
{
  timeRangeStructure_t *ptr;

  if ((timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS) ||
      (name == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t *)timeRangeFindTimeRange(timeRangeIndex);
  if (ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  osapiStrncpySafe((char *)name, (char *)ptr->timeRangeName,
                                   sizeof(ptr->timeRangeName));

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To check if a entry number exists in a time range.
*
* @param    *headPtr    @b{(input)}  pointer to first time range entry
* @param    entryNum     @b{(input)}  entry number of interest
* @param    **entryPtr   @b{(output)} pointer to found time range entry
*
* @returns  L7_TRUE     entry number exists
* @returns  L7_FALSE    entry number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeEntryNumExist(timeRangeEntryParms_t *headPtr, L7_uint32 entryNum, 
                           timeRangeEntryParms_t **entryPtr)
{
  timeRangeEntryParms_t *curr;

  curr = headPtr;
  while (curr != L7_NULLPTR)
  {
    if (entryNum == curr->entryNum)
    {
      *entryPtr = curr;
      return L7_TRUE;
    }
    curr = curr->next;
  }

  return L7_FALSE;
}
/*********************************************************************
*
* @purpose  To check if a periodic time entry exists in a time range.
*
* @param    *headPtr    @b{(input)}  pointer to first time range entry
* @param    **entryPtr   @b{(output)} pointer to found time range entry
*
* @returns  L7_TRUE     entry number exists
* @returns  L7_FALSE    entry number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL timeRangePeriodicEntryExist(timeRangeEntryParms_t *headPtr,
                               timeRangeEntryParms_t **entryPtr)
{
  timeRangeEntryParms_t *curr;

  curr = headPtr;
  while (curr != L7_NULLPTR)
  {
    if (TIMERANGE_PERIODIC_ENTRY == curr->entryType)
    {
      *entryPtr = curr;
      return L7_TRUE;
    }
    curr = curr->next;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Get the periodic time range entry num given 
*           a time range index, start and end times
*
* @param    *headPtr          @b{(input)}  pointer to first time range entry
* @param    *startDateAndTime @b{(input)} date and time at which the 
*                                          time range becomes active.
* @param    *endDateAndTime   @b{(input)} date and time at which the 
*                                          time range becomes inactive.
* @param    **entryPtr        @b{(input)} pointer to found time range entry
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    time range index does not exist
* @returns  L7_FAILURE  Periodic timerange entry 
*                       with specified start and end time 
*                       does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangePeriodicEntryStartAndEndTimeExist(
                   timeRangeEntryParms_t            *headPtr,
                   timeRangeEntryDateAndTimeParms_t *startDateAndTime,
                   timeRangeEntryDateAndTimeParms_t *endDateAndTime,
                   timeRangeEntryParms_t            **entryPtr)
{
  timeRangeEntryParms_t *curr;
  L7_RC_t rc = L7_FAILURE;

  curr = headPtr;
  while (curr != L7_NULLPTR)
  {
    if ((TIMERANGE_PERIODIC_ENTRY == curr->entryType) &&
        (memcmp(startDateAndTime,
                &(curr->startDateAndTime),
                sizeof(timeRangeEntryDateAndTimeParms_t)) == 0)  &&
        (memcmp(endDateAndTime,
                &(curr->endDateAndTime),
                sizeof(timeRangeEntryDateAndTimeParms_t)) == 0))
    {
      *entryPtr = curr;
      return L7_SUCCESS;
    }
    curr = curr->next;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To check if a absolute time entry exists in a time range.
*
* @param    *headPtr    @b{(input)}  pointer to first time range entry
* @param    **entryPtr   @b{(output)} pointer to found time range entry
*
* @returns  L7_TRUE     entry number exists
* @returns  L7_FALSE    entry number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeAbsoluteEntryExist(timeRangeEntryParms_t *headPtr,
                               timeRangeEntryParms_t **entryPtr)
{
  timeRangeEntryParms_t *curr;

  curr = headPtr;
  while (curr != L7_NULLPTR)
  {
    if (TIMERANGE_ABSOLUTE_ENTRY == curr->entryType)
    {
      *entryPtr = curr;
      return L7_TRUE;
    }
    curr = curr->next;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Determine next available timerange entry index value given a 
*           time range pointer
*
* @param    *ptr               @b{(input)} time range element pointer   
* @param    *next              @b{(output)} pointer to next free entry index.
*
* @returns  L7_SUCCESS        
* @returns  L7_TABLE_IS_FULL  if maximum number of time entries 
*                             are already configured
*       
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryIndexNextFreeGet(timeRangeStructure_t *ptr, L7_uint32 *next)
{
  L7_uint32     i;

  for (i = 1; i <= L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE; i++)
  {
    if (L7_INTF_ISMASKBITSET(ptr->entryIndexInUseMask, i) == L7_FALSE)
    {
      break;
    }
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE)
  {
    *next = 0;
    return L7_TABLE_IS_FULL;
  }
  else
  {
    *next = i;
    return L7_SUCCESS;
  }
}

/*********************************************************************
*
* @purpose  To add a entry to a time range.
*
* @param    *timeRange_ptr    @b{(input)} time range element pointer   
* @param    entryNum          @b{(input)} time range entry number
* @param    entryType         @b{(input)} time range entry type
*
* @returns  L7_SUCCESS  entry is added
* @returns  L7_FAILURE  invalid entry number
* @returns  L7_TABLE_IS_FULL, if storage unavailable for entry
* @returns  L7_ALREADY_CONFIGURED, if specified entryType is
*           TIMERANGE_ABSOLUTE_ENTRY and if it is already configured
*           or if the specified entry num already exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryAdd(void *timeRange_ptr, 
                             L7_uint32 entryNum,
                             timeRangeEntryType_t entryType)
{
  L7_BOOL               found;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr, *tmp;

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  if (ptr->entryCount == L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE)
  {
    return L7_TABLE_IS_FULL;
  }

  found = timeRangeEntryNumExist(ptr->head, entryNum, &curr);
  
  if (found == L7_TRUE)
  {
    return L7_ERROR;
  }
  else
  {
    if (entryType == TIMERANGE_ABSOLUTE_ENTRY)
    {
      if (timeRangeAbsoluteEntryExist(ptr->head, &curr) == L7_TRUE)
      {
        return L7_ALREADY_CONFIGURED;
      }
    }
    /* need to create a new node */
    if (bufferPoolAllocate(timeRangeBufferPoolId, (L7_uchar8**) &tmp) != L7_SUCCESS)
    {
      return L7_TABLE_IS_FULL;
    }
    memset(tmp, 0, sizeof(*tmp));
    
    tmp->configMask = 0;
    tmp->entryNum = entryNum;
    tmp->entryType = entryType;
  
    /* if adding the first entry */
    if (ptr->head == L7_NULLPTR)
    {
      tmp->next = ptr->head;
      ptr->head = tmp;
    }
    
    /* OR if adding to the list of entries */
    else
    {
      curr = ptr->head;
      
      /* if 'entryNum' is less than the first entry number in the list */
      if (entryNum < curr->entryNum)
      {
        tmp->next = curr;
        ptr->head = tmp;
      }
      else
      {
        while (curr->next != L7_NULLPTR)
        {
          if (entryNum > curr->next->entryNum)
          {
            curr = curr->next;
          }
          else
          {
            break;
          }
        }
        tmp->next = curr->next;
        curr->next = tmp;
      }
    }
    L7_INTF_SETMASKBIT(ptr->entryIndexInUseMask, entryNum);
    ptr->entryCount++;

  } /* else create new node */

  timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To remove a entry from an existing time range.
*
* @param    *timeRange_ptr    @b{(input)} time range element pointer   
* @param    entryNum          @b{(input)} timerange entry number
*                                            
* @returns  L7_SUCCESS  entry is removed
* @returns  L7_FAILURE  invalid time range index or entry number
* @returns  L7_ERROR    time range index or entry number does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryRemove(void *timeRange_ptr, L7_uint32 entryNum)
{
  L7_RC_t               rc = L7_ERROR;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr, *prev;

  ptr = (timeRangeStructure_t *)timeRange_ptr;

  prev = L7_NULLPTR;
  curr = ptr->head;

  while (curr != L7_NULLPTR)
  {
    if (entryNum == curr->entryNum)
    {
      if (prev == L7_NULLPTR)
      {
        ptr->head = curr->next;         /* remove first entry from list */
      }
      else
      {
        prev->next = curr->next;        /* remove entry from middle of list */
      }

      bufferPoolFree(timeRangeBufferPoolId, (L7_uchar8 *)curr);

      ptr->entryCount--;
      L7_INTF_CLRMASKBIT(ptr->entryIndexInUseMask, entryNum);
      timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
      rc = L7_SUCCESS;
      break;
    }

    prev = curr;
    curr = curr->next;

  } /* endwhile */

  return rc;
}

/*********************************************************************
*
* @purpose  To check the validity of start date and time 
*           for a time range entry.
*
* @param    *timeRange_ptr    @b{(input)} time range pointer
* @param    entryType         @b{(input)} time range entry type
*                                         (It should be either
*                                         TIMERANGE_PERIODIC_ENTRY                           
*                                         or TIMERANGE_ABSOLUTE_ENTRY)
* @param    *startDateAndTime @b{(input)} date and time at which the time range
*                                         becomes active.
*
*
* @returns  L7_SUCCESS        
* @returns  L7_FAILURE    
*
* @comments
*
* @end
*                                                                                         
*********************************************************************/
L7_RC_t timeRangeImpEntryStartDateAndTimeValidityCheck(
                  void                             *timeRange_ptr,
                  L7_uint32                        entryNum,
                  timeRangeEntryType_t             entryType,
                  timeRangeEntryDateAndTimeParms_t *startDateAndTime)
{
  L7_RC_t               rc = L7_SUCCESS;
  L7_uint32             startSeconds = 0, endSeconds = ~0;
  L7_uint32             startMinute = 0, endMinute= ~0;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr, *absoluteEntryPtr;
  timeRangeEntryParms_t *periodicEntryPtr;
  L7_SUMMER_TIME_DAY_t  day;
  L7_uint32             currentStartMinute = 0, currentEndMinute = ~0;
  L7_BOOL               currentStartAndEndMinuteValid = L7_FALSE;
  L7_uchar8             configuredStartDays = 0;
  L7_uchar8             configuredEndDays = 0;

  ptr = (timeRangeStructure_t *)timeRange_ptr;

  if ((entryType == TIMERANGE_ABSOLUTE_ENTRY) &&
      (timeRangeAbsoluteEntryExist(ptr->head, &absoluteEntryPtr) == L7_TRUE))
  {
    startSeconds = timeRangeConvertTimeEntryToSeconds(startDateAndTime);
    
    if ((absoluteEntryPtr->configMask) & (1 << TIMERANGE_ENTRY_END_TIME))
    {
      endSeconds = timeRangeConvertTimeEntryToSeconds(&(absoluteEntryPtr->endDateAndTime));
    }
    else
    {
      return L7_SUCCESS;
    }
    if (startSeconds >= endSeconds)
    {
      TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                      "Absolute Start time mentioned is greater"
                      "than or equal to the the end time");
      return L7_FAILURE;
    }
  }
  
  if (entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    if ((timeRangeEntryGet(ptr->timeRangeIndex, entryNum,
                      &periodicEntryPtr) == L7_SUCCESS))
    {
      if (periodicEntryPtr->configMask & (1 << TIMERANGE_ENTRY_END_TIME))
      {
        configuredEndDays = 0;
        configuredStartDays = 0;
        for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
        {
          if ((periodicEntryPtr->endDateAndTime.date.daysOfTheWeek.dayMask) & \
                                                                   (1 << day))
          {
            configuredEndDays = configuredEndDays + 1;
          }
          if (startDateAndTime->date.daysOfTheWeek.dayMask & (1 << day))
          {
            configuredStartDays = configuredStartDays +1;
          }
        }
        if (configuredStartDays != configuredEndDays)
        {
          TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                   "Periodic entry's start days is not equal to end days");
          return L7_FAILURE;
        }
        if (configuredStartDays > 1)
        {
          if ((periodicEntryPtr->endDateAndTime.date.daysOfTheWeek.dayMask) != \
              (startDateAndTime->date.daysOfTheWeek.dayMask))
          {
            TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                     "Periodic entry's end days of the week "  
                     "paramater contains multiple days and start days "
                     "is not equal to end days");
                     return L7_FAILURE;
          }
        }
        if ((startDateAndTime->date.daysOfTheWeek.dayMask) == \
                            (periodicEntryPtr->endDateAndTime.date.daysOfTheWeek.dayMask))
        {
          for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
          {
            if ((startDateAndTime->date.daysOfTheWeek.dayMask) & (1 << day))
            {
              startMinute = ((day - 1) * 24 * 60) + \
                            (startDateAndTime->hour * 60) + \
                             startDateAndTime->minute;
              
              endMinute = ((day - 1) * 24 * 60) + (periodicEntryPtr->endDateAndTime.hour * 60)+ \
                           periodicEntryPtr->endDateAndTime.minute;
              if (startMinute >= endMinute)
              {
                TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                         "Periodic entry's end time is less than or"
                         "equal to the start time for same start and"
                         "end day(s)");
                return L7_FAILURE;
              }
              break;
            }
          }
        }
      }
    }
    curr = ptr->head;
    while (curr != L7_NULLPTR)
    {
      if ((TIMERANGE_PERIODIC_ENTRY == curr->entryType) &&
          (entryNum != curr->entryNum) &&
          (curr->configMask & (1 << TIMERANGE_ENTRY_START_TIME)) &&
          (curr->configMask & (1 << TIMERANGE_ENTRY_END_TIME)))
      {
        if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) != \
                        (curr->endDateAndTime.date.daysOfTheWeek.dayMask))
        {
         /* If multiple days are specified then start dayMask must be equal 
          * to end dayMask. As they are not equal then only one day is specified
          * start dayMask and end dayMask.
          */
          currentStartAndEndMinuteValid = L7_TRUE;
          for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
          {
            if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
            {
              currentStartMinute = ((day - 1) * 24 * 60) + (curr->startDateAndTime.hour * 60) + curr->startDateAndTime.minute;
              break;
            }
          }
          for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
          {
            if ((curr->endDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
            {
              currentEndMinute =  ((day - 1) * 24 * 60) + (curr->endDateAndTime.hour * 60) + curr->endDateAndTime.minute;
              break;
            }
          }
        }
        for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
        {
          if ((startDateAndTime->date.daysOfTheWeek.dayMask) & (1 << day))
          {
            startMinute = ((day - 1) * 24 * 60) + (startDateAndTime->hour * 60) + startDateAndTime->minute;
            if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) == \
                        (curr->endDateAndTime.date.daysOfTheWeek.dayMask))
            {
              if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
              {
                currentStartMinute = ((day - 1) * 24 * 60) + (curr->startDateAndTime.hour * 60) + curr->startDateAndTime.minute;
                currentEndMinute =  ((day - 1) * 24 * 60) + (curr->endDateAndTime.hour * 60) + curr->endDateAndTime.minute;
                currentStartAndEndMinuteValid = L7_TRUE;
              }
            }
            if (currentStartAndEndMinuteValid == L7_TRUE)
            {
              if (currentStartMinute < currentEndMinute)
              {
                if ((startMinute >= currentStartMinute) &&
                     (startMinute <= currentEndMinute))
                {
                  TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                      "Periodic entry's start time falls within the"
                      "range of already existing periodic time range entry");
                  return L7_FAILURE;
                }
              }
              else if (currentEndMinute < currentStartMinute)
              {
                if ((startMinute <= currentEndMinute) || 
                      (startMinute >= currentStartMinute ))
                {
                  TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                    "Periodic entry's start time falls within the"
                    "range of already existing periodic time range entry");
                  return L7_FAILURE;

                }
              }
            }
          }
        }
      }
      curr = curr->next;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To check the validity of end date and time 
*           for a time range entry.
*
* @param    *timeRange_ptr    @b{(input)} time range pointer
* @param    entryType         @b{(input)} time range entry type
*                                         (It should be either
*                                         TIMERANGE_PERIODIC_ENTRY                           
*                                         or TIMERANGE_ABSOLUTE_ENTRY)
* @param    *endDateAndTime @b{(input)} date and time at which the time range
*                                         becomes active.
*
*
* @returns  L7_SUCCESS        
* @returns  L7_FAILURE    
*
* @comments
*
* @end
*                                                                                         
*********************************************************************/
L7_RC_t timeRangeImpEntryEndDateAndTimeValidityCheck(
                  void                             *timeRange_ptr,
                  timeRangeEntryType_t             entryType,
                  L7_uint32                        entryNum,
                  timeRangeEntryDateAndTimeParms_t *endDateAndTime)
{
  L7_RC_t               rc = L7_SUCCESS;
  L7_uint32             startSeconds = 0, endSeconds = ~0;
  L7_uint32             startMinute = 0, endMinute = ~0;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr, *absoluteEntryPtr;
  timeRangeEntryParms_t *periodicEntryPtr;
  L7_SUMMER_TIME_DAY_t  day;
  L7_uint32             currentStartMinute = 0, currentEndMinute = ~0;
  L7_BOOL               currentStartAndEndMinuteValid = L7_FALSE;
  L7_uchar8             configuredStartDays = 0;
  L7_uchar8             configuredEndDays = 0;

  ptr = (timeRangeStructure_t *)timeRange_ptr;

  if ((entryType == TIMERANGE_ABSOLUTE_ENTRY) &&
      (timeRangeAbsoluteEntryExist(ptr->head, &absoluteEntryPtr) == L7_TRUE))
  {
    if ((absoluteEntryPtr->configMask) & (1 << TIMERANGE_ENTRY_START_TIME))
    {
      startSeconds = timeRangeConvertTimeEntryToSeconds(&(absoluteEntryPtr->startDateAndTime));
    }
    else
    {
      return L7_SUCCESS;
    }

    endSeconds = timeRangeConvertTimeEntryToSeconds(endDateAndTime);
    if (endSeconds <= startSeconds)
    {
      TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                      "Absolute end time mentioned is lesser"
                      "than or equal to the the start time");
      return L7_FAILURE;
    }
  }
  
  if (entryType == TIMERANGE_PERIODIC_ENTRY)
  {
    if ((timeRangeEntryGet(ptr->timeRangeIndex, entryNum,
                      &periodicEntryPtr) == L7_SUCCESS))
    {
      if (periodicEntryPtr->configMask & (1 << TIMERANGE_ENTRY_START_TIME))
      {
        configuredStartDays = 0;
        configuredEndDays   = 0;
        for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
        {
          if ((periodicEntryPtr->startDateAndTime.date.daysOfTheWeek.dayMask) & \
                                                                   (1 << day))
          {
            configuredStartDays = configuredStartDays + 1;
          }
          if (endDateAndTime->date.daysOfTheWeek.dayMask & (1 << day))
          {
            configuredEndDays = configuredEndDays + 1;
          }
        }
        if (configuredStartDays != configuredEndDays)
        {
          TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                   "Periodic entry's start days is not equal to end days");
          return L7_FAILURE;
        }
        if (configuredStartDays > 1)
        {
          if ((periodicEntryPtr->startDateAndTime.date.daysOfTheWeek.dayMask) != \
              (endDateAndTime->date.daysOfTheWeek.dayMask))
          {
            TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                     "Periodic entry's start days of the week "  
                     "paramater contains multiple days and start days "
                     "is not equal to end days");
            return L7_FAILURE;
          }
        }
        if ((periodicEntryPtr->startDateAndTime.date.daysOfTheWeek.dayMask) == \
                            (endDateAndTime->date.daysOfTheWeek.dayMask))
        {
          for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
          {
            if ((periodicEntryPtr->startDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
            {
              startMinute = ((day - 1) * 24 * 60) + \
                            (periodicEntryPtr->startDateAndTime.hour * 60) + \
                             periodicEntryPtr->startDateAndTime.minute;
              
              endMinute = ((day - 1) * 24 * 60) + (endDateAndTime->hour * 60)+ \
                           endDateAndTime->minute;
              if (startMinute >= endMinute)
              {
                TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                         "Periodic entry's end time is less than or"
                         " equal to the start time for same start and"
                         " end day(s)");
            return L7_FAILURE;
          }
          break;
        }
      }
    }
      }
    }
    curr = ptr->head;
    while (curr != L7_NULLPTR)
    {
      if ((TIMERANGE_PERIODIC_ENTRY == curr->entryType) && 
          (entryNum != curr->entryNum) &&
          (curr->configMask & (1 << TIMERANGE_ENTRY_START_TIME)) &&
          (curr->configMask & (1 << TIMERANGE_ENTRY_END_TIME)))
   
      {
        if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) != \
                        (curr->endDateAndTime.date.daysOfTheWeek.dayMask))
        {
         /* If multiple days are specified then start dayMask must be equal 
          * to end dayMask. As they are not equal then only one day is specified
          * start dayMask and end dayMask.
          */
          currentStartAndEndMinuteValid = L7_TRUE;
          for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
          {
            if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
            {
              currentStartMinute = ((day - 1) * 24 * 60) + (curr->startDateAndTime.hour * 60) + curr->startDateAndTime.minute;
              break;
            }
          }
          for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
          {
            if ((curr->endDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
            {
              currentEndMinute =  ((day - 1) * 24 * 60) + (curr->endDateAndTime.hour * 60) + curr->endDateAndTime.minute;
              break;
            }
          }
        }

        for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)
        {
          if ((endDateAndTime->date.daysOfTheWeek.dayMask) & (1 << day))
          {
            endMinute = ((day - 1) * 24 * 60) + (endDateAndTime->hour * 60) + endDateAndTime->minute;
            if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) == \
                        (curr->endDateAndTime.date.daysOfTheWeek.dayMask))
            {
              if ((curr->startDateAndTime.date.daysOfTheWeek.dayMask) & (1 << day))
              {
                currentStartMinute = ((day - 1) * 24 * 60) + (curr->startDateAndTime.hour * 60) + curr->startDateAndTime.minute;
                currentEndMinute =  ((day - 1) * 24 * 60) + (curr->endDateAndTime.hour * 60) + curr->endDateAndTime.minute;
                currentStartAndEndMinuteValid = L7_TRUE;
              }
            }
            if (currentStartAndEndMinuteValid == L7_TRUE)
            {
              if (currentStartMinute < currentEndMinute)
              {
                if ((endMinute >= currentStartMinute) &&
                     (endMinute <= currentEndMinute))
                {
                  TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                      "Periodic entry's end time falls within the"
                      "range of already existing periodic time range entry");
                  return L7_FAILURE;
                }
              }
              else if (currentEndMinute < currentStartMinute)
              {
                if ((endMinute <= currentEndMinute) || 
                      (endMinute >= currentStartMinute ))
                {
                  TIMERANGE_TRACE(TIMERANGE_DEBUG_MODIFY,
                    "Periodic entry's end time falls within the"
                    "range of already existing periodic time range entry");
                  return L7_FAILURE;

                }
              }
            }
          }
        } /*for (day = L7_DAY_SUN; day <= L7_DAY_SAT; day++)*/
      } /*if (TIMERANGE_PERIODIC_ENTRY == curr->entryType)*/
      curr = curr->next;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To get the status of a given periodic time entry
*
* @param    periodicEntry     @b{(input)}  time range index
* @param    *status           @b{(output)} time range status
*
* @returns  L7_SUCCESS  status is retrieved
* @returns  L7_FAILURE  invalid time range index, or other failure
*

* @end
*
*********************************************************************/
L7_RC_t timeRangeImpPeriodicEntryStatusGet(timeRangeEntryParms_t *periodicEntry, timeRangeStatus_t *status)
{
  L7_SUMMER_TIME_DAY_t             startDay, endDay;
  L7_localtime                     currLocalTime;
  L7_uint32                        periodicStartMinute, periodicEndMinute;
  L7_uint32                        currMinute;
  timeRangeEntryDateAndTimeParms_t *startPtr, *endPtr;
 
  if ( periodicEntry == L7_NULLPTR ||
      status == L7_NULLPTR)
  {
    return L7_FAILURE;
  } 
  
  startPtr = &(periodicEntry->startDateAndTime);
  endPtr   = &(periodicEntry->endDateAndTime);
  memset(&currLocalTime, 0x00, sizeof(L7_localtime));
  osapiLocalTime(simAdjustedTimeGet(), &currLocalTime);
  currMinute = (currLocalTime.L7_wday * 24 * 60) + (currLocalTime.L7_hour * 60) + currLocalTime.L7_min;
  
  for (startDay = L7_DAY_SUN; startDay <= L7_DAY_SAT; startDay++ )
  {
    if ((startPtr->date.daysOfTheWeek.dayMask) & (1 << startDay))
    {
      periodicStartMinute = ((startDay - 1) * 24 * 60) + (startPtr->hour * 60) + startPtr->minute;
      for (endDay = L7_DAY_SUN; endDay <= L7_DAY_SAT; endDay++)
      {
        if ((endPtr->date.daysOfTheWeek.dayMask) & (1 << endDay))
        {
          if ((((startPtr->date.daysOfTheWeek.dayMask) == (endPtr->date.daysOfTheWeek.dayMask)) && 
                (startDay == endDay)) || 
               (((startPtr->date.daysOfTheWeek.dayMask) != (endPtr->date.daysOfTheWeek.dayMask)) &&
                (startDay != endDay)))
          {
            periodicEndMinute = ((endDay - 1) * 24 * 60) + (endPtr->hour * 60) + endPtr->minute;
            if (periodicStartMinute < periodicEndMinute)
            {
              if ((currMinute >= periodicStartMinute) &&
                  (currMinute < periodicEndMinute) )
              {
                *status = TIMERANGE_STATUS_ACTIVE;
                return L7_SUCCESS;
              }
            }
            else if (periodicEndMinute < periodicStartMinute)
            {
              if ((currMinute < periodicEndMinute) ||
                  (currMinute >= periodicStartMinute))
              { 
                *status = TIMERANGE_STATUS_ACTIVE;
                return L7_SUCCESS;
              }
            }
          }
        }
      }
    }
  }
  *status = TIMERANGE_STATUS_INACTIVE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the status of a given time range
*
* @param    timeRange_ptr     @b{(input)}  time range ptr
* @param    *status           @b{(output)} time range status
*
* @returns  L7_SUCCESS  status is retrieved
* @returns  L7_FAILURE  invalid time range ptr, or other failure
*

* @end
*
*********************************************************************/
L7_RC_t timeRangeImpStatusGet(void *timeRange_ptr, timeRangeStatus_t *status)
{
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr;
  L7_uint32             absStartTime = 0, absEndTime = ~0, currTime = 0;        

  if ( timeRange_ptr == L7_NULLPTR ||
      status == L7_NULLPTR)
  {
    return L7_FAILURE;
  } 

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  if (timeRangeAbsoluteEntryExist(ptr->head, &curr) == L7_TRUE)
  {
    currTime     =  simAdjustedTimeGet();
    if ((curr->configMask) & (1 << TIMERANGE_ENTRY_START_TIME))
    {
      absStartTime = timeRangeConvertTimeEntryToSeconds(&(curr->startDateAndTime));
    }
    if ((curr->configMask) & (1 << TIMERANGE_ENTRY_END_TIME))
    {
      absEndTime = timeRangeConvertTimeEntryToSeconds(&(curr->endDateAndTime));
    }
   
    if ((currTime >= absStartTime) &&  (currTime < absEndTime))
    {
      if (timeRangePeriodicEntryExist(ptr->head, &curr) != L7_TRUE)
      {
        *status = TIMERANGE_STATUS_ACTIVE;
        return L7_SUCCESS;
      }
      curr = ptr->head;
      while (curr != L7_NULLPTR)
      {
        if (curr->entryType == TIMERANGE_PERIODIC_ENTRY)
        {
          if ((timeRangeImpPeriodicEntryStatusGet(curr, status) == L7_SUCCESS) &&
              (*status == TIMERANGE_STATUS_ACTIVE))
          {
            return L7_SUCCESS;
          }
        }
        curr = curr->next;
      }
    }
    else
    {
      *status = TIMERANGE_STATUS_INACTIVE;
      return L7_SUCCESS;  
    }
  }
  else
  {
    /* Neither periodic nor absolute time entries exist.
       i.e no time restrictions on configurations that referenced the 
       given time range. So time range status is active.
    */ 
    if (timeRangePeriodicEntryExist(ptr->head, &curr) != L7_TRUE)
    {
      *status = TIMERANGE_STATUS_ACTIVE;
      return L7_SUCCESS;
    }
    else
    {
      curr = ptr->head;
      while (curr != L7_NULLPTR)
      {
        if (curr->entryType == TIMERANGE_PERIODIC_ENTRY)
        {
          if ((timeRangeImpPeriodicEntryStatusGet(curr, status) == L7_SUCCESS) &&
              (*status == TIMERANGE_STATUS_ACTIVE))
          {
            return L7_SUCCESS;
          }
        }
        curr = curr->next;
      }
    }
  }
  *status = TIMERANGE_STATUS_INACTIVE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  To add the start date and time for a time range entry.
*
* @param    *timeRange_ptr    @b{(input)} time range pointer
* @param    entrynum          @b{(input)} time range entry number
* @param    entryType         @b{(input)} time range entry type
*                                         (It should be either
*                                         TIMERANGE_PERIODIC_ENTRY                           
*                                         or TIMERANGE_ABSOLUTE_ENTRY)
* @param    *startDateAndTime @b{(input)} date and time at which the time range
*                                         becomes active.
*
*
* @returns  L7_SUCCESS        start date and time successfully added
* @returns  L7_ERROR          time range or entryNum does not exist
* @returns  L7_FAILURE        invalid start dateandtime or other failure
*
* @comments
*
* @end
*                                                                                         
*********************************************************************/
L7_RC_t timeRangeImpEntryStartDateAndTimeAdd(
                  void                            *timeRange_ptr, 
                  L7_uint32                        entryNum,
                  timeRangeEntryType_t             entryType,
                  timeRangeEntryDateAndTimeParms_t *startDateAndTime)
{
  L7_BOOL               found;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *curr, *temp;
  
  if (timeRangeImpEntryStartDateAndTimeValidityCheck(timeRange_ptr,
                                         entryNum,
                                         entryType,
                                         startDateAndTime) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  found = timeRangeEntryNumExist(ptr->head, entryNum, &curr);
  
  if (found == L7_TRUE)
  {
    if (TIMERANGE_ABSOLUTE_ENTRY == entryType)
    {
      curr->startDateAndTime.date.absoluteDate = 
                        startDateAndTime->date.absoluteDate;
    }
    else if (TIMERANGE_PERIODIC_ENTRY == entryType)
    {
      curr->startDateAndTime.date.daysOfTheWeek.dayMask = 
                       startDateAndTime->date.daysOfTheWeek.dayMask;
    }
    curr->startDateAndTime.hour = startDateAndTime->hour;
    curr->startDateAndTime.minute = startDateAndTime->minute;
    curr->configMask |= (1 << TIMERANGE_ENTRY_START_TIME);
    if ((curr->configMask & (1 << TIMERANGE_ENTRY_END_TIME)) &&
        (TIMERANGE_PERIODIC_ENTRY == entryType))
    {
      temp = ptr->head;
      while (temp != L7_NULLPTR)
      {
        if ((entryNum != temp->entryNum) && 
            (TIMERANGE_PERIODIC_ENTRY == temp->entryType)) 
        {
          if((timeRangeImpEntryStartDateAndTimeValidityCheck(timeRange_ptr,
                                                temp->entryNum,
                                                temp->entryType,
                                                &(temp->startDateAndTime)) != L7_SUCCESS) ||
             (timeRangeImpEntryEndDateAndTimeValidityCheck(timeRange_ptr,
                                                temp->entryNum,
                                                temp->entryType,
                                                &(temp->endDateAndTime)) != L7_SUCCESS))
          {
            curr->startDateAndTime.date.daysOfTheWeek.dayMask = 0;
            curr->startDateAndTime.hour = 0;
            curr->startDateAndTime.minute = 0;
            curr->configMask &= ~(1 << TIMERANGE_ENTRY_START_TIME);
            return L7_FAILURE;
          }
        }
        temp = temp->next;
      } /* endwhile */
    }
    timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  To add the end date and time for a time range entry.
*
* @param    *timeRange_ptr    @b{(input)} time range pointer
* @param    entrynum          @b{(input)} time range entry number
* @param    entryType         @b{(input)} time range entry type
*                                         (It should be either
*                                         TIMERANGE_PERIODIC_ENTRY                           
*                                         or TIMERANGE_ABSOLUTE_ENTRY)
* @param    *endDateAndTime  @b{(input)} date and time at which the time range
*                                         becomes inactive.
*
*
* @returns  L7_SUCCESS        end date and time successfully added
* @returns  L7_ERROR          time range or entryNum does not exist
* @returns  L7_FAILURE        invalid end dateandtime or other failure
*
* @comments
*
* @end
*                                                                                         
*********************************************************************/
L7_RC_t timeRangeImpEntryEndDateAndTimeAdd(
                  void                             *timeRange_ptr, 
                  L7_uint32                        entryNum,
                  timeRangeEntryType_t             entryType,
                  timeRangeEntryDateAndTimeParms_t *endDateAndTime)
{
  L7_BOOL           found;
  timeRangeStructure_t *ptr;
  timeRangeEntryParms_t *curr,*temp;

  if (timeRangeImpEntryEndDateAndTimeValidityCheck(timeRange_ptr,
                                         entryType,
                                         entryNum,
                                         endDateAndTime) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  found = timeRangeEntryNumExist(ptr->head, entryNum, &curr);
  
  if (found == L7_TRUE)
  {
    if (TIMERANGE_ABSOLUTE_ENTRY == entryType)
    {
      curr->endDateAndTime.date.absoluteDate = 
                        endDateAndTime->date.absoluteDate;
    }
    else if (TIMERANGE_PERIODIC_ENTRY == entryType)
    {
      curr->endDateAndTime.date.daysOfTheWeek.dayMask = 
                       endDateAndTime->date.daysOfTheWeek.dayMask;
    }
    curr->endDateAndTime.hour = endDateAndTime->hour;
    curr->endDateAndTime.minute = endDateAndTime->minute;
    curr->configMask |= (1 << TIMERANGE_ENTRY_END_TIME);

    if ((curr->configMask & (1 << TIMERANGE_ENTRY_START_TIME)) &&
        (TIMERANGE_PERIODIC_ENTRY == entryType))
    {
      temp = ptr->head;
      while (temp != L7_NULLPTR)
      {
        if ((entryNum != temp->entryNum) && 
            (TIMERANGE_PERIODIC_ENTRY == temp->entryType)) 
        {
          if((timeRangeImpEntryStartDateAndTimeValidityCheck(timeRange_ptr,
                                                temp->entryNum,
                                                temp->entryType,
                                                &(temp->startDateAndTime)) != L7_SUCCESS) ||
             (timeRangeImpEntryEndDateAndTimeValidityCheck(timeRange_ptr,
                                                temp->entryNum,
                                                temp->entryType,
                                                &(temp->endDateAndTime)) != L7_SUCCESS))
          {
            curr->endDateAndTime.date.daysOfTheWeek.dayMask = 0;
            curr->endDateAndTime.hour = 0;
            curr->endDateAndTime.minute = 0;
            curr->configMask &= ~(1 << TIMERANGE_ENTRY_END_TIME);
            return L7_FAILURE;
          }
        }
        temp = temp->next;
      } /* endwhile */
    }

    timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
  }

  return L7_ERROR;
}

/*********************************************************************
*
* @purpose  Get a timeRangeStructure_t where the head is entry entryNum
*
* @param    timeRangeIndex    @b{(input)}  time range index to begin search
* @param    entryNum     @b{(input)}  current entry number
* @param    *timeRange_rp     @b{(output)} entry parameter info pointer
*
* @returns  L7_SUCCESS  entryNum was found
* @returns  L7_ERROR    timeRangeIndex or entryNum does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryGet(L7_uint32 timeRangeIndex, 
                          L7_uint32 entryNum,
                          timeRangeEntryParms_t **timeRange_rp)
{
  timeRangeStructure_t *timeRange_ptr;
  timeRangeEntryParms_t *rp;

  if ((entryNum < L7_TIMERANGE_MIN_RULE_NUM) || (entryNum > L7_TIMERANGE_MAX_RULE_NUM))
  {
    return L7_ERROR;
  }

  timeRange_ptr = (timeRangeStructure_t *)timeRangeFindTimeRange(timeRangeIndex);
  
  if (timeRange_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  rp = timeRange_ptr->head;

  while (rp != L7_NULLPTR)
  {
    if (rp->entryNum == entryNum)
    {
      /* found it */
      *timeRange_rp = rp;
      return L7_SUCCESS;
    }

    rp = rp->next;
  } 
  
  /* entry not found */
  return L7_ERROR; 
}

/*********************************************************************
*
* @purpose  Get the first timerange entry given a timerange index
*
* @param    timeRangeIndex    @b{(input)}  timerange index
* @param    *entry            @b{(output)} first configured entry number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          no entries exist for this timerange
* @returns  L7_FAILURE 
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryGetFirst(L7_uint32 timeRangeIndex, L7_uint32 *entry)
{
  void                 *p;
  timeRangeStructure_t *timeRange_ptr;

  p = timeRangeFindTimeRange(timeRangeIndex);
  if (p == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  timeRange_ptr = (timeRangeStructure_t *)p;
  
  if (timeRange_ptr->head == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  *entry = timeRange_ptr->head->entryNum;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the next timerange entry given a timerange index
*
* @param    timeRangeIndex    @b{(input)}  timerange index
* @param    entrynum          @b{(input)}  current entry number
* @param    *next             @b{(output)} next configured entry number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          no more entries exist for thistimerange
* @returns  L7_FAILURE 
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryGetNext(L7_uint32 timeRangeIndex, L7_uint32 entrynum, L7_uint32 *next)
{
  L7_RC_t               rc = L7_ERROR;
  void                  *p;
  timeRangeStructure_t  *timeRange_ptr;
  timeRangeEntryParms_t *timeRange_rp;

  if (entrynum >= L7_TIMERANGE_MAX_RULE_NUM)
  {
    return L7_FAILURE;
  }

  p = timeRangeFindTimeRange(timeRangeIndex);
  if (p == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = (timeRangeStructure_t *)p;

  timeRange_rp = timeRange_ptr->head; 
  
  while (timeRange_rp != L7_NULLPTR)
  {                      
    if (timeRange_rp->entryNum > entrynum)
    {
      *next = timeRange_rp->entryNum;
      rc = L7_SUCCESS;
      break;
    }

    timeRange_rp = timeRange_rp->next;

  } /* endwhile */
  
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the timerange entry number is within proper range
*
* @param    timeRangeIndex    @b{(input)}  timerange index
* @param    entrynum          @b{(input)}  current entry number
*
* @returns  L7_SUCCESS        entry in range
* @returns  L7_FAILURE          entry out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @comments The timeRangeIndex parameter is required by this function in case there 
*           is ever a need to support separate entry number ranges based
*           on the type of timerange (i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryNumRangeCheck(L7_uint32 timeRangeIndex,
                                       L7_uint32 entrynum)
{
  L7_RC_t       rc = L7_FAILURE;

  if ((entrynum >= L7_TIMERANGE_MIN_RULE_NUM) &&
         (entrynum <= L7_TIMERANGE_MAX_RULE_NUM))
  {
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if specified  timerange entry field is configured
*
* @param    timeRangeIndex    @b{(input)}  timerange index
* @param    entrynum          @b{(input)}  current entry number
* @param    field             @b{(input)}  entry field of interest
*
* @returns  L7_TRUE           entry field is configured
* @returns  L7_FALSE          entry field not configured
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeImpIsFieldConfigured(L7_uint32 timeRangeIndex, L7_uint32 entrynum, L7_uint32 field)
{
  void          *timeRange_ptr;

  /* check inputs */
  if ((timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS) ||
      (timeRangeImpEntryNumRangeCheck(timeRangeIndex, entrynum) != L7_SUCCESS))
  {
    return L7_FALSE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr == L7_NULLPTR)
  {
    return L7_FALSE;
  }
  
  return timeRangeIsEntryFieldConfigured(timeRange_ptr, entrynum, field);
}

/*************************************************************************
* @purpose  Get the current number of timeranges configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_RC_t timeRangeImpCurrNumGet(L7_uint32 *pCurr)
{
  *pCurr = avlTreeCount(&timeRangeTree);
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Check if timerange table is full
*
* @param    void
*
* @returns  L7_TRUE     timerange table is full
* @returns  L7_FALSE    timerange table not full
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*********************************************************************/
L7_BOOL timeRangeImpIsTableFull(void)
{
  L7_uint32     currCount;

  currCount = avlTreeCount(&timeRangeTree);
  return (currCount >= timeRange_max_entries) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
*
* @purpose  Gets the current number of configured entries in a timerange
*
* @param    timeRangeIndex    @b{(input)}  timerange index
* @param    *entryCount       @b{(output)} number of entries intimerange
*
* @returns  L7_SUCCESS        timerange entry count retrieved
* @returns  L7_ERROR          timerange does not exist
* @returns  L7_FAILURE        timerange index is out of range
*
* @comments This is an implementation function to be called both from
*           the API layer and from functions within this file.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryCountGet(L7_uint32 timeRangeIndex, L7_uint32 *entryCount)
{
  timeRangeStructure_t *ptr;

  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t*)timeRangeFindTimeRange(timeRangeIndex);
  if (ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  *entryCount = ptr->entryCount;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the status of the specified time range
*
* @param    timeRangeIndex    @b{(input)}  time range index to begin search
* @param    *status           @b{(output)} pointer to status output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    time range does not exist
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpStatusRead(L7_uint32 timeRangeIndex, timeRangeStatus_t *status)
{
  timeRangeStructure_t *ptr;

  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t*)timeRangeFindTimeRange(timeRangeIndex);
  if (ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  *status= ptr->status;

  return L7_SUCCESS;
}

/**************************************************************************
* @purpose  Take an Time Range read lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER  
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiReadLockTake() instead.
*
* @end
*************************************************************************/
L7_RC_t timeRangeReadLockTake(osapiRWLock_t rwlock,  char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiReadLockTake(rwlock, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
            "Read lock take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/**************************************************************************
* @purpose  Give an Time Range read lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t timeRangeReadLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiReadLockGive(rwlock);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
            "Read lock give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/**************************************************************************
* @purpose  Take an Time Range write lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate and the caller
*           typically waits forever), so any error gets logged.
*
* @notes    DO NOT USE if specifying a timeout other than L7_WAIT_FOREVER  
*           where awareness of the time expiration is needed.  Make a direct
*           call to osapiWriteLockTake() instead.
*
* @end
*************************************************************************/
L7_RC_t timeRangeWriteLockTake(osapiRWLock_t rwlock, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiWriteLockTake(rwlock, L7_WAIT_FOREVER);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
            "Write lock take failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/**************************************************************************
* @purpose  Give an Time Range write lock
*
* @param    rwlock  @{(input)} Read/Write lock id
* @param    file    @{(input)} File name of caller (for error logging)
* @param    line    @{(input)} File line number of caller (for error logging)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Wrapper function for OSAPI call, so that errors can be checked
*           in one place.  The assumption here is that there are no "expected"
*           errors (i.e., the r/w lock id is legitimate) so any error gets
*           logged.
*
* @end
*************************************************************************/
L7_RC_t timeRangeWriteLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line)
{
  L7_RC_t       rc;

  rc = osapiWriteLockGive(rwlock);
  if (rc != L7_SUCCESS)
  {
    l7utilsFilenameStrip(&file);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
            "Write lock give failure for file %s:%lu, rc=%u, id=0x%8.8x\n",
            file, line, rc, (L7_uint32)rwlock.handle);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the time range name string is alpha-numeric
*
* @param    *name       @b{(input)} time range name string
*
* @returns  L7_TRUE     time range name is alpha-numeric
* @returns  L7_FALSE    time range name string not alpha-numeric
*
* @comments This function only checks the name string syntax for a named time range.
*           It does not check if any named time range currently contains this name.
*
* @comments An alpha-numeric string is defined as consisting of only
*           alphabetic, numeric, dash, underscore, or space characters.
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeImpNameIsAlphanumeric(L7_uchar8 *name)
{
  L7_uint32     i, len;
  L7_uchar8     *p;

  if (name == L7_NULLPTR)
    return L7_FALSE;

  len = (L7_uint32)strlen((char *)name);

  /* cannot start with  a number */
  if (len == 0 || isalpha(*name) == 0)
  {
     return L7_FALSE;
  }

  /* the only allowed characters in a legal named time range name are:
   * alphabetic, numeric, dash, underscore, or period
   */
  for (i=0, p=name; i<len; i++, p++)
  {
    if (!(isalnum(*p) || *p=='-' || *p=='_' || *p=='.'))
    {
      return L7_FALSE;
    }
  }

  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  utility function to convert absolute time entry's 
*           start/end time into seconds since epoch.
*
* @param    *dateAndTime   @b{(input)} Pointer to date and time
*
* @returns  seconds since the  epoch 
*           -1 if the specified broken-down time cannot be 
*              represented as calendar time
*
* @end
*
*********************************************************************/
L7_uint32 timeRangeConvertTimeEntryToSeconds(timeRangeEntryDateAndTimeParms_t *dateAndTime)
{
  L7_localtime          localTime;

  memset(&localTime, 0x00, sizeof(L7_localtime));

  localTime.L7_sec   = 0;
  localTime.L7_min   = dateAndTime->minute;
  localTime.L7_hour  = dateAndTime->hour;
  localTime.L7_mday  = dateAndTime->date.absoluteDate.day;
  localTime.L7_mon   = dateAndTime->date.absoluteDate.month - 1;
  localTime.L7_year  = dateAndTime->date.absoluteDate.year - 1900;

  return osapiMkTime(&localTime);

}

/*********************************************************************
* @purpose  Display the current Time Range Table contents
*
* @param    showEntries   @b{(input)} Indicates if time range entrys are displayed
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void timeRangeShow(L7_BOOL showEntries)
{
  avlTree_t              *pTree = &timeRangeTree;
  L7_uint32              count;
  L7_uint32              timeRangeIndex;
  timeRangeStructure_t   *pTimeRange;

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return;
  }

  if ((count = avlTreeCount(pTree)) == 0)
  {
    TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
    TIMERANGE_DEBUG_PRINTF("Time Range table is empty\n\n");
    return;
  }

  TIMERANGE_DEBUG_PRINTF("\n\nTime Range Index Next:  %u\n\n", timeRangeIndexNextFree);

  TIMERANGE_DEBUG_PRINTF("\nTime Range Table (contains %u entries):\n\n", count);

  TIMERANGE_DEBUG_PRINTF("Index  Entries               Name            \n");
  TIMERANGE_DEBUG_PRINTF("------ ----- ------------------------------\n");


  timeRangeIndex = 0;                                 /* start with first entry */
  while ((pTimeRange = avlSearchLVL7(pTree, &timeRangeIndex, AVL_NEXT)) != L7_NULLPTR)
  {
    TIMERANGE_DEBUG_PRINTF("%6u  %3u              %s \n", 
            pTimeRange->timeRangeIndex, 
            pTimeRange->entryCount, 
            pTimeRange->timeRangeName); 

    /* update search keys for next pass */
    timeRangeIndex = pTimeRange->timeRangeIndex;
  }
  TIMERANGE_DEBUG_PRINTF("\n\n");

  /* optionally display the entrys for each time range */
  if (showEntries == L7_TRUE)
  {
    timeRangeIndex = 0;                               /* start with first entry */
    while ((pTimeRange = avlSearchLVL7(pTree, &timeRangeIndex, AVL_NEXT)) != L7_NULLPTR)
    {
      timeRangeEntriesShow(pTimeRange->timeRangeIndex);

      /* update search keys for next pass */
      timeRangeIndex = pTimeRange->timeRangeIndex;
    }
  }

  TIMERANGE_DEBUG_PRINTF("timeRangeIndexNextFree = %u\n\n", timeRangeIndexNextFree);
  
  TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
}

/*********************************************************************
* @purpose  Display content of all entrys for a specific time range
*           given its name
*
* @param    timeRangeName    @b{(input)} time range name
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void timeRangeDebugShow(L7_uchar8 *timeRangeName)
{
 
 L7_uint32 timeRangeIndex = 0;

 if (timeRangeNameStringCheck(timeRangeName) != L7_SUCCESS)
 {
   TIMERANGE_DEBUG_PRINTF("Invalid Time Range Name."
                           "Name string may include alphabetic, numeric, dash,"
                           "dot or underscore characters only. Name must start"
                           " with a letter and the size of the name string must"
                           " be less than or equal to 31 characters.");

 }
 
 if (timeRangeNameToIndex(timeRangeName, &timeRangeIndex) == L7_SUCCESS)
 {
   timeRangeEntriesShow(timeRangeIndex);
 }
 else
 {
   TIMERANGE_DEBUG_PRINTF("Time Range does not exist.");
 }


}
/*********************************************************************
* @purpose  Display content of all entrys for a specific time range 
*
* @param    timeRangeIndex    @b{(input)} time range index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void timeRangeEntriesShow(L7_uint32 timeRangeIndex)
{
  avlTree_t             *pTree = &timeRangeTree;
  timeRangeStructure_t  *pTimeRange;
  timeRangeEntryParms_t *pEntry;
  timeRangeStatus_t     status;
  L7_uint32             currentTime;
  struct tm             *LocalTime;
  const L7_uchar8       *dayName[] = {
                                      "sun",
                                      "mon",
                                      "tue",
                                      "wed",
                                      "thu",
                                      "fri",
                                      "sat"
                                      };

  pTimeRange = avlSearchLVL7(pTree, &timeRangeIndex, AVL_EXACT);
  if (pTimeRange == L7_NULLPTR)
  {
    TIMERANGE_DEBUG_PRINTF("Time Range entry not found for index %u\n\n", timeRangeIndex);
    return;
  }
  if (pTimeRange->timeRangeIndex != timeRangeIndex)
  {
    TIMERANGE_DEBUG_PRINTF("Time Range entry index %u does not match requested Time Range index %u\n\n",
            pTimeRange->timeRangeIndex, timeRangeIndex);
    return;
  }

  TIMERANGE_DEBUG_PRINTF("Time Range \"%s\" (Index %u) ", 
                         (char *)pTimeRange->timeRangeName,
                          pTimeRange->timeRangeIndex);
  if (timeRangeImpStatusGet(pTimeRange, &status) == L7_SUCCESS)
  {
    TIMERANGE_DEBUG_PRINTF("Status \"%s\" \n",
                      status == TIMERANGE_STATUS_ACTIVE ? "Active": "Inactive");
  
  }
  currentTime = simAdjustedTimeGet();
  LocalTime = localtime((time_t *)&currentTime);
  
  TIMERANGE_DEBUG_PRINTF(" Current Time(Day: \" %s \" dd/mm/yyyy hh:mm:ss) %02d/%02d/%04d %02d:%02d:%02d \n",
                                 dayName[LocalTime->tm_wday],
                                 LocalTime->tm_mday,
                                 LocalTime->tm_mon+1,
                                 LocalTime->tm_year + 1900,
                                 LocalTime->tm_hour,
                                 LocalTime->tm_min,
                                 LocalTime->tm_sec);
  TIMERANGE_DEBUG_PRINTF("\n");

  pEntry = pTimeRange->head;

  if (pEntry == L7_NULLPTR)
    return;


  while (pEntry != L7_NULLPTR) 
  {
    timeRangeEntryDisplay(pEntry);

    /* update search keys for next pass */
    pEntry = pEntry->next;
  }
  TIMERANGE_DEBUG_PRINTF("\n\n");
}

/*********************************************************************
* @purpose  Display the Time Range entry config mask contents
*
* @param    mask        @b{(input)} Time Range entry config mask
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void timeRangeEntryCfgMaskPrint(L7_uint32 mask)
{
  L7_BOOL       isFirst = L7_TRUE;
  L7_uint32     i, imax;
  char          **cfgMaskStr;

  imax = TIMERANGE_ENTRY_FIELDS_TOTAL;
  cfgMaskStr = timeRangeEntryCfgMaskStr;

  for (i = 1; i < imax; i++)
  {
    if ((mask & (1 << i)) != 0)
    {
      /* print a comma before each item except the first */
      if (isFirst == L7_TRUE)
      {
        isFirst = L7_FALSE;
      }
      else
      {
        TIMERANGE_DEBUG_PRINTF(",");
      }

      TIMERANGE_DEBUG_PRINTF("%s", cfgMaskStr[i]);
    }
  }
}

/*********************************************************************
* @purpose  Display the Time Range entry config mask contents
*
* @param    mask        @b{(input)} Time Range entry config mask
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void timeRangeEntryDaysOfWeekPrint(L7_uchar8 daysOfTheWeek)
{
  if ( daysOfTheWeek & (1 << L7_DAY_SUN ))
  {
    TIMERANGE_DEBUG_PRINTF("SUN ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_MON ))
  {
    TIMERANGE_DEBUG_PRINTF("MON ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_TUE ))
  {
    TIMERANGE_DEBUG_PRINTF("TUE ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_WED ))
  {
    TIMERANGE_DEBUG_PRINTF("WED ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_THU ))
  {
    TIMERANGE_DEBUG_PRINTF("THU ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_FRI ))
  {
    TIMERANGE_DEBUG_PRINTF("FRI ");
  }
  if ( daysOfTheWeek & (1 << L7_DAY_SAT ))
  {
    TIMERANGE_DEBUG_PRINTF("SAT ");
  }

}

/*********************************************************************
* @purpose  Display contents of a Time Range entry
*
* @param    *pEntry      @b{(input)}Time Range entry pointer
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void timeRangeEntryDisplay(timeRangeEntryParms_t *pEntry)
{
  char                *pSpacer;


  pSpacer = " ";
  TIMERANGE_DEBUG_PRINTF("%s Entry %u\n", pSpacer, pEntry->entryNum);
  pSpacer = "   ";

  TIMERANGE_DEBUG_PRINTF("%s cfgMask=0x%8.8x ", pSpacer, pEntry->configMask);
  if (pEntry->configMask != 0)
  {
    TIMERANGE_DEBUG_PRINTF("(");
    timeRangeEntryCfgMaskPrint(pEntry->configMask);
    TIMERANGE_DEBUG_PRINTF(")");
  }

  TIMERANGE_DEBUG_PRINTF("\n");
  if (pEntry->entryType == TIMERANGE_ABSOLUTE_ENTRY)
  {
    if ((pEntry->configMask) & (1 << TIMERANGE_ENTRY_START_TIME))
    {
      TIMERANGE_DEBUG_PRINTF("%s Absolute Start Time(dd/mm/yyyy hh:mm) "
                                   "%02d/%02d/%04d %02d:%02d \n",
                                   pSpacer,
                                   pEntry->startDateAndTime.date.absoluteDate.day,
                                   pEntry->startDateAndTime.date.absoluteDate.month,
                                   pEntry->startDateAndTime.date.absoluteDate.year,
                                   pEntry->startDateAndTime.hour,
                                   pEntry->startDateAndTime.minute); 
    }
  
    if ((pEntry->configMask) & (1 << TIMERANGE_ENTRY_END_TIME))
    {
      TIMERANGE_DEBUG_PRINTF("%s Absolute End Time(dd/mm/yyyy hh:mm) "
                                   "%02d/%02d/%04d %02d:%02d \n",
                                   pSpacer,
                                   pEntry->endDateAndTime.date.absoluteDate.day,
                                   pEntry->endDateAndTime.date.absoluteDate.month,
                                   pEntry->endDateAndTime.date.absoluteDate.year,
                                   pEntry->endDateAndTime.hour,
                                   pEntry->endDateAndTime.minute);


    }
  }
  if (pEntry->entryType == TIMERANGE_PERIODIC_ENTRY)
  {
      TIMERANGE_DEBUG_PRINTF("%s Periodic Start Time(daysoftheweek hh:mm)", pSpacer);
      timeRangeEntryDaysOfWeekPrint(pEntry->startDateAndTime.date.daysOfTheWeek.dayMask);
      TIMERANGE_DEBUG_PRINTF("%02d:%02d \n", pEntry->startDateAndTime.hour,
                                             pEntry->startDateAndTime.minute); 
      TIMERANGE_DEBUG_PRINTF("%s Periodic End Time  (daysoftheweek hh:mm)", pSpacer);
      timeRangeEntryDaysOfWeekPrint(pEntry->endDateAndTime.date.daysOfTheWeek.dayMask);
      TIMERANGE_DEBUG_PRINTF("%02d:%02d \n", pEntry->endDateAndTime.hour,
                                             pEntry->endDateAndTime.minute); 

  }
}

/*********************************************************************
* @purpose  callback function for debug
*
* @param    timeRangeName Name of the time range
* @param    event         Time range event type
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t timeRangeDebugCallCBFcn(L7_uchar8 *timeRangeName, timeRangeEvent_t  event)
{ 
  L7_localtime    currLocalTime;
  const L7_uchar8 *dayName[] = {
  "sun",
  "mon",
  "tue",
  "wed",
  "thu",
  "fri",
  "sat"
  };

  memset(&currLocalTime, 0x00, sizeof(L7_localtime));
  osapiLocalTime(simAdjustedTimeGet(), &currLocalTime);

  TIMERANGE_DEBUG_PRINTF("\n Current Time(Day: \" %s \" dd/mm/yyyy hh:mm:ss) %02d/%02d/%04d %02d:%02d:%02d \n",
                                   dayName[currLocalTime.L7_wday],
                                   currLocalTime.L7_mday,
                                   currLocalTime.L7_mon+1,
                                   currLocalTime.L7_year + 1900,
                                   currLocalTime.L7_hour,
                                   currLocalTime.L7_min,
                                   currLocalTime.L7_sec);
  TIMERANGE_DEBUG_PRINTF("Time Range Name: %s\n", timeRangeName);
  TIMERANGE_DEBUG_PRINTF("Event Recieved : %s\n", timeRangeEventNames[event]); 
  return L7_SUCCESS;
}

void timeRangeDebugRegister()
{
  timeRangeEventCallbackRegister(L7_TIMERANGES_COMPONENT_ID,
                                "timeRangeDebugRegister",
                                timeRangeDebugCallCBFcn);
}


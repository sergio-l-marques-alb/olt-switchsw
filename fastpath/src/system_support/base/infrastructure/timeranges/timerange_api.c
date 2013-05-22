/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename timerange_api.c
*
* @purpose Contains Time Range APIs
*
* @component Time Range
*
* @comments Many APIs in this file take a read/write lock. Control is 
*           then passed into one or more implementation functions in
*           timerange.c.  The lock is always managed at the API level
*           and assumed to be already taken, if necessary, in the 
*           implementation functions.
*
* @create 11/11/2009
*
* @author Siva Mannem
*
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "osapi.h"
#include "timerange.h"
#include "timerange_control.h"
#include "timerange_api.h"
#include "timerange_cfg.h"

extern osapiRWLock_t           timeRangeRwLock;
extern timeRangeCnfgrState_t   timeRangeCnfgrState;
extern timeRangeCfgFileData_t  *timeRangeCfgFileData;

static L7_RC_t timeRangeIndexAndEntryPtrGet(
                              L7_uint32             timeRangeIndex,
                              L7_uint32             entryNum,
                              void                  **timeRange_ptr,
                              timeRangeEntryParms_t **entry_ptr);

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new time range in the time range table
*
* @param    *next       @b{(output)} time range index next value
*
* @returns  L7_SUCCESS
* @returns  L7_TABLE_IS_FULL  time range table is currently full
* @returns  L7_FAILURE        all other failures
*
* @comments The index value remains free until used in a subsequent time range create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new time range
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @comments Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
L7_RC_t timeRangeNextAvailableIndexGet(L7_uint32 *next)
{
  L7_RC_t       rc = L7_SUCCESS;

  /* check inputs */
  if (next == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  do                                            /* single-pass control loop */
  {
    /* always output a value of 0 if maximum number of time ranges are already 
     * configured
     */
    if (timeRangeImpIsTableFull() == L7_TRUE)
    {
      /* table full */
      rc = L7_TABLE_IS_FULL;
      break;
    }

    timeRangeImpIndexNextRead(next);

  } while (0);

  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (rc != L7_SUCCESS)
  {
    *next = 0;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the first time range created in the system.
*
* @param    *timeRangeIndex   @b{(output)} first time range index value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          no time ranges exist
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeIndexFirstGet(L7_uint32 *timeRangeIndex)
{
  return timeRangeIndexNextGet(0, timeRangeIndex);
} 

/*********************************************************************
*
* @purpose  Get the next sequential time range index
*
* @param    timeRangeIndex    @b{(input)}  time range index to begin search
* @param    *next             @b{(output)} next time range index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          no time ranges exist, or timeRangeIndex 
*                             is the last
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeIndexNextGet(L7_uint32 timeRangeIndex, L7_uint32 *next)
{
  L7_RC_t rc;

  if (next == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpIndexGetNext(timeRangeIndex, next);
  
  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Checks if timeRangeIndex is a valid, configured time range
*
* @param    timeRangeIndex    @b{(input)}  time range index
*
* @returns  L7_SUCCESS  time range is valid
* @returns  L7_FAILURE  time range not valid
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeIndexCheckValid(L7_uint32 timeRangeIndex)
{
  L7_RC_t rc;

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpIndexCheckValid(timeRangeIndex);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the current number of entries defined for the specified time range
*
* @param    timeRangeIndex    @b{(input)}  time range index to begin search
* @param    *entryCount       @b{(output)} pointer to entry count output location
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
L7_RC_t timeRangeNumEntriesGet(L7_uint32 timeRangeIndex, L7_uint32 *entryCount)
{
  L7_RC_t rc;

  if (entryCount == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpEntryCountGet(timeRangeIndex, entryCount);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
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
L7_RC_t timeRangeStatusGet(L7_uint32 timeRangeIndex, timeRangeStatus_t *status)
{
  L7_RC_t rc;

  if (status == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpStatusRead(timeRangeIndex, status);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To create a new time range.
*
* @param    timeRangeIndex    @b{(input)} time range index
*
* @returns  L7_SUCCESS        time range created
* @returns  L7_FAILURE        invalid timeRange identifier, all other failures
* @returns  L7_ERROR          time range already exists
* @returns  L7_TABLE_IS_FULL  maximum number of time ranges already created
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeCreate(L7_uint32 timeRangeIndex)
{
  void          *timeRange_ptr;

  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr != L7_NULLPTR)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  /* check if maximum number of time ranges are already configured */
  if (timeRangeImpIsTableFull() == L7_TRUE)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_TABLE_IS_FULL;
  }

  if (timeRangeImpCreate(timeRangeIndex) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }


  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To delete an existing time ranges.
*
* @param    timeRangeIndex    @b{(input)} time range index
*
* @returns  L7_SUCCESS             time ranges deleted
* @returns  L7_ERROR               time ranges does not exist
* @returns  L7_FAILURE             invalid time ranges index, all other failures
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeDelete(L7_uint32 timeRangeIndex)
{
  void                 *timeRange_ptr;
  timeRangeStructure_t *ptr;
  L7_RC_t               rc = L7_SUCCESS;

  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr == L7_NULLPTR)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  if (timeRangeUpdate(ptr->timeRangeName, TIMERANGE_DELETE) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  if (timeRangeImpDelete(timeRange_ptr) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To add the name to this time range.
*
* @param    timeRangeIndex    @b{(input)} time range index
* @param    *name             @b{(input)} time range name 
*                               (1 to L7_TIMERANGE_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS             name is added.
* @returns  L7_FAILURE             invalid time range index, or other failure.
* @returns  L7_DEPENDENCY_NOT_MET  Invalid time range name. 
* @returns  L7_ERROR               time range does not exist.
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeNameAdd(L7_uint32 timeRangeIndex, L7_uchar8 *name)
{
  void              *timeRange_ptr;

  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (timeRangeNameStringCheck(name) != L7_SUCCESS)
  {
    return L7_DEPENDENCY_NOT_MET;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr == L7_NULLPTR)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  /* update app */
  if (timeRangeImpNameAdd(timeRange_ptr, name) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }
  
  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the name of this time range.
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    *name             @b{(output)} time range name 
*                             (1 to L7_TIMERANGE_NAME_LEN_MAX chars)
*                                            
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid time range index, name ptr, or other failure
* @returns  L7_ERROR    time range does not exist
*
* @comments Caller must provide a name buffer of at least 
*           (L7_TIMERANGE_NAME_LEN_MAX+1) characters.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeNameGet(L7_uint32 timeRangeIndex, L7_uchar8 *name)
{
  void          *timeRange_ptr;

  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (name == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr == L7_NULLPTR)
  {
    (void)TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }
  
  timeRangeImpNameGet(timeRange_ptr, name);
  
  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Checks if the time range name is a valid string
*
* @param    *name       @b{(input)} time range name string
*
* @returns  L7_SUCCESS  valid time range name string
* @returns  L7_FAILURE  invalid time range name string
*
* @comments This function only checks the name string syntax for a time range. 
*           It does not check if an time range currently contains this name.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeNameStringCheck(L7_uchar8 *name)
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     nameLen;

  /* check inputs */
  if (name == L7_NULLPTR)
    return L7_FAILURE;

  /* no need for read/write lock control here */

  nameLen = (L7_uint32)strlen((char *)name);

  if ((nameLen < L7_TIMERANGE_NAME_LEN_MIN) || 
      (nameLen > L7_TIMERANGE_NAME_LEN_MAX))
    return L7_FAILURE;

  if (timeRangeImpNameIsAlphanumeric(name) == L7_TRUE)
    rc = L7_SUCCESS;

  return rc;
}

/*********************************************************************
*
* @purpose  To get the index of a time range, given its name.
*
* @param    *name      @b{(input)} time range name
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
L7_RC_t timeRangeNameToIndex(L7_uchar8 *name, L7_uint32 *timeRangeIndex)
{
  L7_RC_t       rc;

  /* check inputs */
  if ((name == L7_NULLPTR) || 
      (strlen((char *)name) > L7_TIMERANGE_NAME_LEN_MAX))
  {
    return L7_FAILURE;
  }
 
  if (timeRangeIndex == L7_NULLPTR)
  { 
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  /* search for time range containing this name */
  rc = timeRangeImpNameToIndexGet(name, timeRangeIndex);
  
  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Determine next available timerange entry index value in
*           a given timerange
*
* @param    timeRangeIndex    @b{(input)} time range index
* @param    *next              @b{(output)} pointer to next free entry index.
*
* @returns  L7_SUCCESS        
* @returns  L7_ERROR          time range does not exist
* @returns  L7_TABLE_IS_FULL  if maximum number of time entries 
*                             are already configured
*       
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryIndexNextFreeGet(L7_uint32            timeRangeIndex,
                                       L7_uint32            *next)
{ 
  L7_RC_t               rc;
  void                  *timeRange_ptr;

  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr == L7_NULLPTR)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  if ((rc = timeRangeImpEntryIndexNextFreeGet(
                                 (timeRangeStructure_t*)timeRange_ptr,
                                  next)) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
  
  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}
/*********************************************************************
*
* @purpose  To add an entryType to a time range entry.
*
* @param    timeRangeIndex    @b{(input)} time range index
* @param    entryNum          @b{(input)} time range entry number
* @param    entryType         @b{(input)} entryType type (absolute/periodic)
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          time range does not exist
* @returns  L7_TABLE_IS_FULL  storage unavailable for entry
* @returns  L7_ALREADY_CONFIGURED, if specified entryType is
*                                  TIMERANGE_ABSOLUTE_ENTRY 
*                                  and if it is already configured
* @returns  L7_FAILURE        invalid timeRange identifier, all other failures
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryAdd(L7_uint32            timeRangeIndex,
                          L7_uint32            entryNum, 
                          timeRangeEntryType_t entryType)
{
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  L7_uchar8             timeRangeName[L7_TIMERANGE_NAME_LEN_MAX+1];

  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (timeRangeImpEntryNumRangeCheck(timeRangeIndex, entryNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
 
  if (entryType >= TIMERANGE_ENTRY_TYPE_TOTAL)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (timeRange_ptr == L7_NULLPTR)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  /* the time range name must be set before allowing any entries to be created */
  timeRangeImpNameGet(timeRange_ptr, timeRangeName);
  if (timeRangeNameStringCheck(timeRangeName) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }
  
  
  /* update app */
  if ((rc = timeRangeImpEntryAdd(timeRange_ptr, entryNum,
                                 entryType)) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
  
  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  To remove a entry from an existing time range.
*
* @param    timeRangeIndex    @b{(input)} time range index
* @param    entryNum     @b{(input)} time range entry number
* @param    force       @b{(input)} force deletion regardless of whether in use
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR          time range or entry number does not exist
* @returns  L7_FAILURE        invalid time range index or entry number
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryRemove(L7_uint32 timeRangeIndex, L7_uint32 entryNum)
{
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *p;

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeIndexAndEntryPtrGet(timeRangeIndex,
                                       entryNum, &timeRange_ptr, &p);
  if (rc != L7_SUCCESS)
  {
    TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
  /* update app */
  if (timeRangeImpEntryRemove(timeRange_ptr, entryNum) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  rc = timeRangeUpdate(ptr->timeRangeName, TIMERANGE_MODIFY);

  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}
/*********************************************************************
*
* @purpose  To add the start date and time for a time range entry.
*
* @param    timeRangeIndex    @b{(input)} time range index
* @param    entryNum          @b{(input)} time range entry number
* @param    entryType         @b{(input)} time range entry type
*                                         (It should be either
*                                         TIMERANGE_PERIODIC_ENTRY
*                                         or TIMERANGE_ABSOLUTE_ENTRY)
* @param    *startDateAndTime  @b{(input)} date and time at which the time range
*                                         becomes inactive.
*
*
* @returns  L7_SUCCESS        start date and time successfully added
* @returns  L7_ERROR          time range or entryNum does not exist
* @returns  L7_FAILURE        invalid start dateandtime or other failure
*
* @comments
*
* @start
*
*********************************************************************/
L7_RC_t timeRangeEntryStartDateAndTimeAdd(
                L7_uint32                        timeRangeIndex,
                L7_uint32                        entryNum,
                timeRangeEntryType_t             entryType,
                timeRangeEntryDateAndTimeParms_t *startDateAndTime)
{
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *p;

  /* check inputs */
  if (L7_NULL == startDateAndTime)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeIndexAndEntryPtrGet(timeRangeIndex,
                                       entryNum, &timeRange_ptr, &p);
  if (rc != L7_SUCCESS)
  {
    TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
  /* return successfully if existing entry value is same */
  if ((TIMERANGE_ENTRY_FIELD_IS_SET(p->configMask,
       TIMERANGE_ENTRY_START_TIME) == L7_TRUE) &&
      (0 == memcmp(startDateAndTime, &(p->startDateAndTime), 
       sizeof(timeRangeEntryDateAndTimeParms_t))))
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_SUCCESS;
  }

  /* update app */
  if (timeRangeImpEntryStartDateAndTimeAdd(timeRange_ptr, entryNum, 
                                 entryType, startDateAndTime) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  rc = timeRangeUpdate(ptr->timeRangeName, TIMERANGE_MODIFY);
  
  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  To add the end date and time for a time range entry.
*
* @param    timeRangeIndex    @b{(input)} time range index
* @param    entryNum          @b{(input)} time range entry number
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
L7_RC_t timeRangeEntryEndDateAndTimeAdd(
                L7_uint32                        timeRangeIndex,
                L7_uint32                        entryNum,
                timeRangeEntryType_t             entryType,
                timeRangeEntryDateAndTimeParms_t *endDateAndTime)
{
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  timeRangeStructure_t  *ptr;
  timeRangeEntryParms_t *p;

  /* check inputs */
  if (L7_NULL == endDateAndTime)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeIndexAndEntryPtrGet(timeRangeIndex,
                                       entryNum, &timeRange_ptr, &p);
  if (rc != L7_SUCCESS)
  {
    TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
  /* return successfully if existing entry value is same */
  if ((TIMERANGE_ENTRY_FIELD_IS_SET(p->configMask,
       TIMERANGE_ENTRY_END_TIME) == L7_TRUE) &&
      (0 == memcmp(endDateAndTime, &(p->endDateAndTime), 
       sizeof(timeRangeEntryDateAndTimeParms_t))))
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_SUCCESS;
  }

  /* update app */
  if (timeRangeImpEntryEndDateAndTimeAdd(timeRange_ptr, entryNum,
                        entryType, endDateAndTime) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  ptr = (timeRangeStructure_t *)timeRange_ptr;
  rc = timeRangeUpdate(ptr->timeRangeName, TIMERANGE_MODIFY);
  
  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Get the first time range entry given a time range index
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    *entryNum    @b{(output)} first configured entry number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    no entries exist for this time range
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryGetFirst(L7_uint32 timeRangeIndex, L7_uint32 *entryNum)
{
  L7_RC_t       rc;

  if (entryNum == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpEntryGetFirst(timeRangeIndex, entryNum);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the absolute time range entry num given 
*           a time range index
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    *entryNum         @b{(output)} configured absolute time range
*                                          entry number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    time range index does not exist
* @returns  L7_FAILURE  Absolute timerange entry does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeAbsoluteEntryNumGet(L7_uint32 timeRangeIndex, L7_uint32 *entryNum)
{
  L7_RC_t       rc = L7_FAILURE;
  timeRangeEntryParms_t *curr;
  timeRangeStructure_t  *timeRange_ptr;

  if (entryNum == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = (timeRangeStructure_t *)timeRangeFindTimeRange(timeRangeIndex);
  if (L7_NULLPTR == timeRange_ptr)
  {
    (void)TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  if (timeRangeAbsoluteEntryExist(timeRange_ptr->head, &curr) == L7_TRUE)
  {
    *entryNum = curr->entryNum;
    rc = L7_SUCCESS;
  }

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Delete the periodic time range entry num given 
*           a time range index, start and end times
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    *startDateAndTime @b{(input)} date and time at which the 
*                                          time range becomes active.
* @param    *endDateAndTime   @b{(input)} date and time at which the 
*                                          time range becomes inactive.
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
L7_RC_t timeRangePeriodicEntryDelete(
                   L7_uint32                        timeRangeIndex,
                   timeRangeEntryDateAndTimeParms_t *startDateAndTime,
                   timeRangeEntryDateAndTimeParms_t *endDateAndTime)
{
  L7_RC_t               rc = L7_FAILURE;
  timeRangeEntryParms_t *curr;
  timeRangeStructure_t  *timeRange_ptr;
  L7_uint32             entryNum;

  if (startDateAndTime == L7_NULLPTR ||
      endDateAndTime   == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_WRITE_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  timeRange_ptr = (timeRangeStructure_t *)timeRangeFindTimeRange(timeRangeIndex);
  if (L7_NULLPTR == timeRange_ptr)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_ERROR;
  }

  rc = timeRangePeriodicEntryStartAndEndTimeExist(timeRange_ptr->head,
                                                 startDateAndTime,
                                                 endDateAndTime,
                                                 &curr);
  if (rc == L7_SUCCESS)
  {
    entryNum = curr->entryNum;
  }
  else
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  /* update app */
  if (timeRangeImpEntryRemove((void *)timeRange_ptr, entryNum) != L7_SUCCESS)
  {
    (void)TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock);
    return L7_FAILURE;
  }

  rc = timeRangeUpdate(timeRange_ptr->timeRangeName, TIMERANGE_MODIFY);

  if (TIMERANGE_WRITE_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Get the next time range entry given a time range index
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    entryNum          @b{(input)}  current entry number
* @param    *next             @b{(output)} next configured entry number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          no more entries exist for this time range
* @returns  L7_FAILURE 
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryGetNext(L7_uint32 timeRangeIndex,
                              L7_uint32 entryNum, 
                              L7_uint32 *next)
{
  L7_RC_t       rc;

  if (next == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpEntryGetNext(timeRangeIndex, entryNum, next);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if entryNum is valid, configured for this time range
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    entryNum          @b{(input)}  current entry number
*
* @returns  L7_SUCCESS  entry is valid
* @returns  L7_ERROR    entry not valid
* @returns  L7_FAILURE  time range does not exist, or other failures
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryCheckValid(L7_uint32 timeRangeIndex, L7_uint32 entryNum)
{
  L7_RC_t           rc;
  timeRangeEntryParms_t *timeRange_rp;

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeEntryGet(timeRangeIndex, entryNum, &timeRange_rp);
  
  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Checks if the time range entry number is within proper range
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    entryNum     @b{(input)}  current entry number
*
* @returns  L7_SUCCESS  entry in range
* @returns  L7_FAILURE  entry out of range
*
* @comments The timeRangeIndex parameter is required by this function in case there 
*           is ever a need to support separate entry number ranges based
*           on the type oftime range(i.e., standard vs. extended).
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryNumRangeCheck(L7_uint32 timeRangeIndex, L7_uint32 entryNum)
{
  /* no need for read/write lock control here */
  return timeRangeImpEntryNumRangeCheck(timeRangeIndex, entryNum);
}

/*********************************************************************
*
* @purpose  Checks if specified time range entry field is configured
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    entryNum          @b{(input)}  current entry number
* @param    field             @b{(input)}  entry field of interest
*
* @returns  L7_TRUE     entry field is configured
* @returns  L7_FALSE    entry field not configured
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeIsFieldConfigured(L7_uint32 timeRangeIndex,
                                   L7_uint32 entryNum,
                                   L7_uint32 field)
{
  L7_BOOL rc;

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  rc = timeRangeImpIsFieldConfigured(timeRangeIndex, entryNum, field);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FALSE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To get the entry type for a time range entry num.
*
* @param    timeRangeIndex     @b{(input)} time range index
* @param    entryNum           @b{(input)} time range entry number
* @param    *entryType         @b{(output)} time range entry type
*
* @returns  L7_SUCCESS  start date and time is successfully retrieved.
* @returns  L7_ERROR    time range or entryNum does not exist
* @returns  L7_FAILURE  invalid start dateandtime or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryTypeGet(L7_uint32            timeRangeIndex,
                              L7_uint32            entryNum,
                              timeRangeEntryType_t *entryType)
{ 
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  timeRangeEntryParms_t *p;

  /* check inputs */
  if (entryType  == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeIndexAndEntryPtrGet(timeRangeIndex,
                                       entryNum, &timeRange_ptr, &p);
  if (rc != L7_SUCCESS)
  {
    TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
 
  *entryType = p->entryType;

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the start date and time for a time range entry.
*
* @param    timeRangeIndex     @b{(input)} time range index
* @param    entryNum           @b{(input)} time range entry number
* @param    *entryType         @b{(output)} time range entry type
* @param    *startDateAndTime  @b{(output)} date and time at which the 
*                                          time range becomes active.
*
*
* @returns  L7_SUCCESS  start date and time is successfully retrieved.
* @returns  L7_ERROR    time range or entryNum does not exist
* @returns  L7_FAILURE  invalid start dateandtime or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryStartDateAndTimeGet(L7_uint32 timeRangeIndex, 
                   L7_uint32                        entryNum,
                   timeRangeEntryType_t             *entryType,
                   timeRangeEntryDateAndTimeParms_t *startDateAndTime)
{
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  timeRangeEntryParms_t *p;

  /* check inputs */
  if ((entryType  == L7_NULLPTR) ||
       (startDateAndTime == L7_NULLPTR) )
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeIndexAndEntryPtrGet(timeRangeIndex,
                                       entryNum, &timeRange_ptr, &p);
  if (rc != L7_SUCCESS)
  {
    TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
 
  *entryType = p->entryType;
  memcpy(startDateAndTime, &(p->startDateAndTime),
            sizeof(timeRangeEntryDateAndTimeParms_t));

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To get the end date and time for a time range entry.
*
* @param    timeRangeIndex     @b{(input)} time range index
* @param    entryNum           @b{(input)} time range entry number
* @param    *entryType         @b{(output)} time range entry type
* @param    *endDateAndTime  @b{(output)} date and time at which the 
*                                          time range becomes active.
*
*
* @returns  L7_SUCCESS  end date and time is successfully retrieved.
* @returns  L7_ERROR    time range or entryNum does not exist
* @returns  L7_FAILURE  invalid end dateandtime or other failure
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeEntryEndDateAndTimeGet(L7_uint32 timeRangeIndex, 
                 L7_uint32                        entryNum,
                 timeRangeEntryType_t             *entryType,
                 timeRangeEntryDateAndTimeParms_t *endDateAndTime)
{
  L7_RC_t               rc;
  void                  *timeRange_ptr;
  timeRangeEntryParms_t *p;

  /* check inputs */
  if ((entryType  == L7_NULLPTR) ||
       (endDateAndTime == L7_NULLPTR) )
  {
    return L7_FAILURE;
  }

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeIndexAndEntryPtrGet(timeRangeIndex,
                                       entryNum, &timeRange_ptr, &p);
  if (rc != L7_SUCCESS)
  {
    TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock);
    return rc;
  }
 
  *entryType = p->entryType;
  memcpy(endDateAndTime, &(p->endDateAndTime),
            sizeof(timeRangeEntryDateAndTimeParms_t));

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the maximum number of time ranges that can be configured
*
* @param    pMax  @b{(output)} Pointer to the maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t timeRangeMaxNumGet(L7_uint32 *pMax)
{
  if (pMax != L7_NULLPTR)
  {
    *pMax = L7_TIMERANGE_MAX_NUM;
  }
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Get the current number of  time ranges configured
*
* @param    pCurr  @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t timeRangeCurrNumGet(L7_uint32 *pCurr)
{
  L7_RC_t   rc;

  if (TIMERANGE_READ_LOCK_TAKE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = timeRangeImpCurrNumGet(pCurr);

  if (TIMERANGE_READ_LOCK_GIVE(timeRangeRwLock) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
*
* @purpose  To obtain the time range pointer and the time entry pointer
*           for a given time range index and time range entry.
*
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    entryNum          @b{(input)}  time range entry number
* @param    **timeRange_ptr   @b{(output)} time range element pointer
* @param    **entry_ptr       @b{(output)} time range entry content pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    time range or entryNum does not exist
* @returns  L7_FAILURE  other failure
*
* @comments 
*
* @comments This is an internal function, not part of the time range API.
*
* @end
*
*********************************************************************/
static L7_RC_t timeRangeIndexAndEntryPtrGet(
                                         L7_uint32             timeRangeIndex, 
                                         L7_uint32             entryNum,
                                         void                  **timeRange_ptr, 
                                         timeRangeEntryParms_t **entry_ptr)
{
  /* check inputs */
  if (timeRangeImpIndexRangeCheck(timeRangeIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if (timeRangeImpEntryNumRangeCheck(timeRangeIndex, entryNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((timeRange_ptr == L7_NULLPTR) || (entry_ptr == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  *timeRange_ptr = timeRangeFindTimeRange(timeRangeIndex);
  if (*timeRange_ptr == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  if (timeRangeEntryGet(timeRangeIndex, entryNum, entry_ptr) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  return L7_SUCCESS;
}


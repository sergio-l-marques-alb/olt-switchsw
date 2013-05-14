/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename timerange.h
*
* @purpose Time Ranges Component Header File
*
* @component Time Ranges
*
* @comments
*
* @create 10/27/2009
*
* @author Siva Mannem.
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_TIMERANGE_H
#define INCLUDE_TIMERANGE_H
#include "osapi.h"
#include "l7_cnfgr_api.h"
#include "timerange_exports.h"
#include "comm_mask.h"

#define TIMERANGE_IS_READY \
 ( ((timeRangeCnfgrState == TIMERANGE_PHASE_INIT_3) || \
    (timeRangeCnfgrState == TIMERANGE_PHASE_EXECUTE) || \
    (timeRangeCnfgrState == TIMERANGE_PHASE_UNCONFIG_1)) \
                         ? (L7_TRUE) : (L7_FALSE) \
 )

/* Semaphore take/give macros
 *
 */
#define TIMERANGE_READ_LOCK_TAKE(_l) \
  timeRangeReadLockTake((_l), __FILE__, __LINE__)

#define TIMERANGE_READ_LOCK_GIVE(_l) \
  timeRangeReadLockGive((_l), __FILE__, __LINE__)

#define TIMERANGE_WRITE_LOCK_TAKE(_l) \
  timeRangeWriteLockTake((_l), __FILE__, __LINE__)

#define TIMERANGE_WRITE_LOCK_GIVE(_l) \
  timeRangeWriteLockGive((_l), __FILE__, __LINE__)

/* checks if a particular time range entry field is currently configured  */
#define TIMERANGE_ENTRY_FIELD_IS_SET(_cfgmask, _field) \
  ((((_cfgmask) & (1 << (_field))) != 0) ? L7_TRUE : L7_FALSE)

#define TIMERANGE_MSG_SIZE                sizeof(timeRangeMsg_t)
#define TIMERANGE_NOTIFY_FUNC_NAME_SIZE   32

typedef struct timeRangeEventNotifyList_s {
  L7_COMPONENT_IDS_t componentID;
 /* Name of registered func - for debug */
  L7_uchar8   name[TIMERANGE_NOTIFY_FUNC_NAME_SIZE];
 /*Pointer to the callback function*/
  L7_RC_t (*notify)(L7_uchar8 *timeRangeName, timeRangeEvent_t  event);
} timeRangeEventNotifyList_t;

typedef enum {
  TIMERANGE_PHASE_INIT_0 = 0,
  TIMERANGE_PHASE_INIT_1,
  TIMERANGE_PHASE_INIT_2,
  TIMERANGE_PHASE_WMU,
  TIMERANGE_PHASE_INIT_3,
  TIMERANGE_PHASE_EXECUTE,
  TIMERANGE_PHASE_UNCONFIG_1,
  TIMERANGE_PHASE_UNCONFIG_2
} timeRangeCnfgrState_t;

typedef enum {
  TIMERANGE_TIMER_MSG = 1,
  TIMERANGE_MODIFY_MSG, 
  TIMERANGE_DELETE_MSG, 
  TIMERANGE_CNFGR_MSG,
  TIMERANGE_LAST_MSG
}timeRangeMessageType_t;

typedef enum {
  TIMERANGE_MODIFY = 1,
  TIMERANGE_DELETE
}timeRangeActions_t;


/* Number of bytes in mask */
#define TIMERANGE_ENTRY_INDEX_INDICES \
         (((L7_TIMERANGE_MAX_ENTRIES_PER_TIMERANGE + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[TIMERANGE_ENTRY_INDEX_INDICES];
} TIMERANGE_ENTRY_INDEX_MASK_t;


/* TimeRange Entry definition */
typedef struct timeRangeEntryParms_s {
  L7_uint32                        configMask;

  /* timerange Entry num*/
  L7_uint32                        entryNum;

  /* Type of time range entry (absolute/periodic)*/
  timeRangeEntryType_t             entryType;

  /*time and date after which associated
    configuration statement starts going into effect.*/
  timeRangeEntryDateAndTimeParms_t startDateAndTime;

  /*time after which the associated
    configuration statement is no longer in effect*/
  timeRangeEntryDateAndTimeParms_t endDateAndTime;

  /*pointer to next time entry in the timerange */
  struct timeRangeEntryParms_s    *next;

} timeRangeEntryParms_t;

/* Time Range Definition */
typedef struct timeRangeStructure_s {
  /* TimeRange identifying number */
  L7_uint32                        timeRangeIndex;

  /* number of time range entries */
  L7_uint32                        entryCount;

  TIMERANGE_ENTRY_INDEX_MASK_t     entryIndexInUseMask;
  /* Name of the time range */
  L7_uchar8                        timeRangeName[L7_TIMERANGE_NAME_LEN_MAX+1];

  /*Status of the timerange*/
  timeRangeStatus_t                status;

  /* pointer to the first timerange entry */
  timeRangeEntryParms_t            *head;

  /* pointer needed by AVL library*/
  struct timeRangeStructure_s      *nextTimeRange;

} timeRangeStructure_t;

typedef struct timeRangeMsg_s {
  timeRangeMessageType_t msgType;

  union {
    L7_CNFGR_CMD_DATA_t    CmdData;
    L7_uchar8              timeRangeName[L7_TIMERANGE_NAME_LEN_MAX+1];
  }u;

}timeRangeMsg_t;

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
L7_RC_t timeRangeCreateTree(L7_uint32 maxTimeRangeNum);

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
void timeRangeDeleteTree(void);

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
void timeRangeImpIndexNextRead(L7_uint32 *next);

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
void timeRangeImpIndexInUseInit(void);

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
void timeRangeImpIndexInUseUpdate(L7_uint32 timeRangeIndex, L7_BOOL inUse);

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
void timeRangeImpIndexNextUpdate(void);

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
L7_RC_t timeRangeImpIndexGetNext(L7_uint32 timeRangeIndex, L7_uint32 *next);

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
L7_RC_t timeRangeImpIndexCheckValid(L7_uint32 timeRangeIndex);

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
L7_RC_t timeRangeImpIndexRangeCheck(L7_uint32 timeRangeIndex);

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
void * timeRangeFindTimeRange(L7_uint32 timeRangeIndex);

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
L7_BOOL timeRangeIsEntryFieldConfigured(void *timeRange_ptr, L7_uint32 entrynum, L7_uint32 field);


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
L7_RC_t timeRangeImpCreate(L7_uint32 timeRangeIndex);


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
L7_RC_t timeRangeImpDelete(void *timeRange_ptr);


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
L7_RC_t timeRangeImpNameAdd(void *timeRange_ptr, L7_uchar8 *name);


/*********************************************************************
*
* @purpose  To get the name of this time range.
*
* @param    *timeRange_ptr    @b{(input)}  access list element pointer
* @param    *name             @b{(output)} access list name
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
void timeRangeImpNameGet(void *timeRange_ptr, L7_uchar8 *name);

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
L7_RC_t timeRangeImpNameToIndexGet(L7_uchar8 *name, L7_uint32 *timeRangeIndex);

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
L7_RC_t timeRangeImpNameFromIndexGet(L7_uint32 timeRangeIndex, L7_uchar8 *name);

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
                           timeRangeEntryParms_t **entryPtr);

/*********************************************************************
*
* @purpose  To check if a periodic time entry exists in a time range.
*
* @param    *headPtr    @b{(input)}  pointer to first time range entry
* @param    **entryPtr   @b{(output)} pointer to found time range entry
*
* @returns  L7_TRUE     periodic entry  exists
* @returns  L7_FALSE    periodic entry does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL timeRangePeriodicEntryExist(timeRangeEntryParms_t *headPtr,
                               timeRangeEntryParms_t **entryPtr);

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
                   timeRangeEntryParms_t            **entryPtr);

/*********************************************************************
*
* @purpose  To check if a absolute time entry exists in a time range.
*
* @param    *headPtr    @b{(input)}  pointer to first time range entry
* @param    **entryPtr   @b{(output)} pointer to found time range entry
*
* @returns  L7_TRUE     absolute entry  exists
* @returns  L7_FALSE    absolute entry does not exist
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL timeRangeAbsoluteEntryExist(timeRangeEntryParms_t *headPtr,
                               timeRangeEntryParms_t **entryPtr);

/*********************************************************************
*
* @purpose  Determine next available timerange entry index value given a 
*           time range pointer
*
* @param    *ptr              @b{(input)} time range element pointer   
* @param    *next              @b{(output)} pointer to next free entry index.
*
* @returns  L7_SUCCESS        
* @returns  L7_TABLE_IS_FULL  if maximum number of time entries 
*                             are already configured
*       
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryIndexNextFreeGet(timeRangeStructure_t *ptr,
                                          L7_uint32 *next);

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
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeImpEntryAdd(void *timeRange_ptr,
                             L7_uint32 entryNum,
                             timeRangeEntryType_t entryType);

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
L7_RC_t timeRangeImpEntryRemove(void *timeRange_ptr, L7_uint32 entryNum);

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
L7_RC_t timeRangeImpStatusGet(void *timeRange_ptr, timeRangeStatus_t *status);

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
                  void                             *timeRange_ptr,
                  L7_uint32                        entryNum,
                  timeRangeEntryType_t             entryType,
                  timeRangeEntryDateAndTimeParms_t *startDateAndTime);

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
                  timeRangeEntryDateAndTimeParms_t *endDateAndTime);

/*********************************************************************
*
* @purpose  Get a timeRangeStructure_t where the head is entry entryNum
*
* @param    timeRangeIndex    @b{(input)}  access list index to begin search
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
                          timeRangeEntryParms_t **timeRange_rp);
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
L7_RC_t timeRangeImpEntryGetFirst(L7_uint32 timeRangeIndex, L7_uint32 *entry);

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
L7_RC_t timeRangeImpEntryGetNext(L7_uint32 timeRangeIndex, L7_uint32 entrynum, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if the timerange entry number is within proper range
*
* @param    timeRangeIndex    @b{(input)}  timerange index
* @param    entrynum          @b{(input)}  current entry number
*
* @returns  L7_SUCCESS        entry in range
* @returns  L7_ERROR          entry out of range
* @returns  L7_FAILURE        timerange does not exist, or other failures
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
                                       L7_uint32 entrynum);

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
L7_BOOL timeRangeImpIsFieldConfigured(L7_uint32 timeRangeIndex, L7_uint32 entrynum, L7_uint32 field);

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
L7_RC_t timeRangeImpCurrNumGet(L7_uint32 *pCurr);

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
L7_BOOL timeRangeImpIsTableFull(void);

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
L7_RC_t timeRangeImpEntryCountGet(L7_uint32 timeRangeIndex, L7_uint32 *entryCount);

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
L7_RC_t timeRangeImpStatusRead(L7_uint32 timeRangeIndex, timeRangeStatus_t *status);

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
* @end
*************************************************************************/
L7_RC_t timeRangeReadLockTake(osapiRWLock_t rwlock, char *file, L7_ulong32 line);

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
L7_RC_t timeRangeReadLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line);

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
*
* @end
*************************************************************************/
L7_RC_t timeRangeWriteLockTake(osapiRWLock_t rwlock, char *file, L7_ulong32 line);

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
L7_RC_t timeRangeWriteLockGive(osapiRWLock_t rwlock, char *file, L7_ulong32 line);

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
L7_BOOL timeRangeImpNameIsAlphanumeric(L7_uchar8 *name);

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
L7_uint32 timeRangeConvertTimeEntryToSeconds(timeRangeEntryDateAndTimeParms_t *dateAndTime);

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
void timeRangeShow(L7_BOOL showEntries);

/*********************************************************************
* @purpose  Display content of all entrys for a specific time range
*
* @param    timeRangeIndex    @b{(input)} MAC access list index
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void timeRangeEntriesShow(L7_uint32 timeRangeIndex);

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
void timeRangeEntryCfgMaskPrint(L7_uint32 mask);

/*********************************************************************
* @purpose  Display contents of a Time Range entry
*
* @param    *pEntry      @b{(input)}Time Range entry pointer
* @param    msgLvlReqd   @b{(input)} Desired output message level
*
* @returns  void
*
* @comments Assumes caller handles any necessary semaphore protection.
*
* @end
*********************************************************************/
void timeRangeEntryDisplay(timeRangeEntryParms_t *pEntry);
#endif  /* INCLUDE_TIMERANGE_H */

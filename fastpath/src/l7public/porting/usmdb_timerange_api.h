/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename    usmDb_timerange_api.h
* @purpose     usmDb Time Range API functions
* @component   Time Range
* @comments    none
* @create      09 Dec 2009
* @author      Siva Mannem
* @end
*             
**********************************************************************/

#ifndef USMDB_TIMERANGE_API_H
#define USMDB_TIMERANGE_API_H
#include "timerange_exports.h"

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new time range in the time range table
*
* @param    unitIndex   @b{(input)}  the unit for this operation
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
L7_RC_t usmdbTimeRangeNextAvailableIndexGet(L7_uint32 unitIndex, L7_uint32 *next);

/*********************************************************************
*
* @purpose  Get the first time range created in the system.
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeIndexFirstGet(L7_uint32 unitIndex,
                                    L7_uint32 *timeRangeIndex); 

/*********************************************************************
*
* @purpose  Get the next sequential time range index
*
* @param    unitIndex         @b{(input)}  the unit for this operation
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
L7_RC_t usmDbTimeRangeIndexNextGet(L7_uint32 unitIndex,
                                   L7_uint32 timeRangeIndex,
                                   L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if timeRangeIndex is a valid, configured time range
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeIndexCheckValid(L7_uint32 unitIndex,
                                      L7_uint32 timeRangeIndex);

/*********************************************************************
*
* @purpose  Get the current number of entrys defined for the specified time range
*
* @param    unitIndex         @b{(input)}  the unit for this operation
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
L7_RC_t usmDbTimeRangeNumEntriesGet(L7_uint32 unitIndex,
                                    L7_uint32 timeRangeIndex,
                                    L7_uint32 *entryCount);

/*********************************************************************
*
* @purpose  Get the status of the specified time range
*
* @param    unitIndex         @b{(input)}  the unit for this operation
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
L7_RC_t usmDbTimeRangeStatusGet(L7_uint32         unitIndex,
                                L7_uint32         timeRangeIndex,
                                timeRangeStatus_t *status);

/*********************************************************************
*
* @purpose  To create a new time range.
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)} time range index
*
* @returns  L7_SUCCESS        time range created
* @returns  L7_FAILURE        invalid timeRange identifier, all other failures
* @returns  L7_ERROR          time range already exists
* @returns  L7_TABLE_IS_FULL  maximum number of time ranges already created
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTimeRangeCreate(L7_uint32 unitIndex, L7_uint32 timeRangeIndex);

/*********************************************************************
*
* @purpose  To delete an existing time ranges.
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)} time range index
*
* @returns  L7_SUCCESS             time ranges deleted
* @returns  L7_ERROR               time ranges does not exist
* @returns  L7_REQUEST_DENIED      time ranges config change is not approved
* @returns  L7_FAILURE             invalid time ranges index, all other failures
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTimeRangeDelete(L7_uint32 unitIndex, L7_uint32 timeRangeIndex);

/*********************************************************************
*
* @purpose  To add the name to this time range.
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeNameAdd(L7_uint32 unitIndex,
                              L7_uint32 timeRangeIndex,
                              L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To get the name of this time range.
*
* @param    unitIndex         @b{(input)}  the unit for this operation
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
L7_RC_t usmDbTimeRangeNameGet(L7_uint32 unitIndex,
                              L7_uint32 timeRangeIndex,
                              L7_uchar8 *name);

/*********************************************************************
*
* @purpose  Checks if the time range name is a valid string
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    *name             @b{(input)} time range name string
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
L7_RC_t usmDbTimeRangeNameStringCheck(L7_uint32 unitIndex,
                                      L7_uchar8 *name);

/*********************************************************************
*
* @purpose  To get the index of a time range, given its name.
*
* @param    unitIndex        @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeNameToIndex(L7_uint32 unitIndex,
                                  L7_uchar8 *name,
                                  L7_uint32 *timeRangeIndex);

/*********************************************************************
*
* @purpose  Determine next available timerange entry index value in
*           a given timerange
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)} time range index
* @param    *next             @b{(output)} pointer to next free entry index.
*
* @returns  L7_SUCCESS        
* @returns  L7_ERROR          time range does not exist
* @returns  L7_TABLE_IS_FULL  if maximum number of time entries 
*                             are already configured
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbTimeRangeEntryIndexNextFreeGet(L7_uint32 timeRangeIndex,
                                            L7_uint32 *next);

/*********************************************************************
*
* @purpose  To add an entry to a time range.
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)} time range index
* @param    entryNum          @b{(input)} time range entry number
* @param    entryType         @b{(input)} entryType type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE        invalid timeRange identifier, all other failures
* @returns  L7_ERROR          time range does not exist
* @returns  L7_REQUEST_DENIED time range config change is not approved
* @returns  L7_TABLE_IS_FULL  storage unavailable for entry
*
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTimeRangeEntryAdd(L7_uint32            unitIndex,
                               L7_uint32            timeRangeIndex,
                               L7_uint32            entryNum,
                               timeRangeEntryType_t entryType);

/*********************************************************************
*
* @purpose  To remove a entry from an existing time range.
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)} time range index
* @param    entryNum     @b{(input)} time range entry number
*                                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR          time range or entry number does not exist
* @returns  L7_FAILURE        invalid time range index or entry number
* @returns  L7_HARDWARE_ERROR hardware update failed
* @returns  L7_REQUEST_DENIED time range config change is not approved
*
* @comments An L7_REQUEST_DENIED response means another application   
*           has denied approval of this configuration change.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTimeRangeEntryRemove(L7_uint32 unitIndex,
                                  L7_uint32 timeRangeIndex,
                                  L7_uint32 entryNum);
/*********************************************************************
*
* @purpose  To add the start date and time for a time range entry.
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    *timeRange_ptr    @b{(input)} time range pointer
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
L7_RC_t usmDbTimeRangeEntryStartDateAndTimeAdd(L7_uint32     unitIndex,
                            L7_uint32                        timeRangeIndex,
                            L7_uint32                        entryNum,
                            timeRangeEntryType_t             entryType,
                            timeRangeEntryDateAndTimeParms_t *startDateAndTime);

/*********************************************************************
*
* @purpose  To add the end date and time for a time range entry.
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    *timeRange_ptr    @b{(input)} time range pointer
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
L7_RC_t usmDbTimeRangeEntryEndDateAndTimeAdd(L7_uint32     unitIndex,
                            L7_uint32                        timeRangeIndex,
                            L7_uint32                        entryNum,
                            timeRangeEntryType_t             entryType,
                            timeRangeEntryDateAndTimeParms_t *endDateAndTime);

/*********************************************************************
*
* @purpose  Get the first time range entry given a time range index
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    *entryNum         @b{(output)} first configured entry number
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
L7_RC_t usmDbTimeRangeEntryGetFirst(L7_uint32 unitIndex,
                                    L7_uint32 timeRangeIndex,
                                    L7_uint32 *entryNum);

/*********************************************************************
*
* @purpose  Get the absolute time range entry num given 
*           a time range index
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeAbsoluteEntryNumGet(L7_uint32 unitIndex,
                                     L7_uint32 timeRangeIndex,
                                     L7_uint32 *entryNum);

/*********************************************************************
*
* @purpose  Delete the periodic time range entry num given 
*           a time range index, start and end times
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    *startDateAndTime @b{(output)} date and time at which the 
*                                          time range becomes active.
* @param    *endDateAndTime   @b{(output)} date and time at which the 
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
L7_RC_t usmDbTimeRangePeriodicEntryDelete(L7_uint32 unitIndex,
                   L7_uint32                        timeRangeIndex,
                   timeRangeEntryDateAndTimeParms_t *startDateAndTime,
                   timeRangeEntryDateAndTimeParms_t *endDateAndTime);

/*********************************************************************
*
* @purpose  Get the next time range entry given a time range index
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeEntryGetNext(L7_uint32 unitIndex,
                                   L7_uint32 timeRangeIndex,
                                   L7_uint32 entryNum,
                                   L7_uint32 *next);

/*********************************************************************
*
* @purpose  Checks if entryNum is valid, configured for this time range
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeEntryCheckValid(L7_uint32 unitIndex,
                                      L7_uint32 timeRangeIndex,
                                      L7_uint32 entryNum);

/*********************************************************************
*
* @purpose  Checks if the time range entry number is within proper range
*
* @param    unitIndex         @b{(input)}  the unit for this operation
* @param    timeRangeIndex    @b{(input)}  time range index
* @param    entryNum          @b{(input)}  current entry number
*
* @returns  L7_SUCCESS  entry in range
* @returns  L7_FAILURE  entry out of range
*
* @comments The timeRangeIndex parameter is required by this function in case there 
*           is ever a need to support separate entry number ranges based
*           on the type of time range.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTimeRangeEntryNumRangeCheck(L7_uint32 unitIndex,
                                         L7_uint32 timeRangeIndex,
                                         L7_uint32 entryNum);

/*********************************************************************
*
* @purpose  Checks if specified time range entry field is configured
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_BOOL usmDbTimeRangeIsFieldConfigured(L7_uint32 unitIndex,
                                        L7_uint32 timeRangeIndex,
                                        L7_uint32 entryNum,
                                        L7_uint32 field);

/*********************************************************************
*
* @purpose  To get the entry type for a time range entry num.
*
* @param    unitIndex          @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeEntryTypeGet(L7_uint32     unitIndex,
                            L7_uint32            timeRangeIndex,
                            L7_uint32            entryNum,
                            timeRangeEntryType_t *entryType);

/*********************************************************************
*
* @purpose  To get the start date and time for a time range entry.
*
* @param    unitIndex          @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeEntryStartDateAndTimeGet(L7_uint32     unitIndex,
                            L7_uint32                        timeRangeIndex,
                            L7_uint32                        entryNum,
                            timeRangeEntryType_t             *entryType,
                            timeRangeEntryDateAndTimeParms_t *startDateAndTime);

/*********************************************************************
*
* @purpose  To get the end date and time for a time range entry.
*
* @param    unitIndex         @b{(input)} the unit for this operation
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
L7_RC_t usmDbTimeRangeEntryEndDateAndTimeGet(L7_uint32     unitIndex,
                            L7_uint32                        timeRangeIndex,
                            L7_uint32                        entryNum,
                            timeRangeEntryType_t             *entryType,
                            timeRangeEntryDateAndTimeParms_t *endDateAndTime);


/*************************************************************************
* @purpose  Get the maximum number of time ranges that can be configured
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    *pMax             @b{(output)} Pointer to the maximum value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbTimeRangeMaxNumGet(L7_uint32 unitIndex, L7_uint32 *pMax);

/*************************************************************************
* @purpose  Get the current number of  time ranges configured
*
* @param    unitIndex         @b{(input)} the unit for this operation
* @param    *pCurr            @b{(output)} Pointer to the current value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbTimeRangeCurrNumGet(L7_uint32 unitIndex, L7_uint32 *pCurr);

#endif /* USMDB_TIMERANGE_API_H */

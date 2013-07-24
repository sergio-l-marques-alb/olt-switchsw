/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename rmon_event.h
*
* @purpose  this is the header file for RMON Event group
*           
* @component SNMP-RMON
*
*
* @create  08/13/2001
*
* @author  K Pragash 
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef RMON_EVENT_H
#define RMON_EVENT_H

#include <l7_common.h>
#include <stdio.h>

#include <string.h>
#include <osapi.h>
#include <snmp_trap_api.h>
#include <usmdb_trapmgr_api.h>

/********************* FUNCTIONS ******************************/

/************** GET FUNCTIONS for EVENT GROUP ********************/

/*********************************************************************
*
* @purpose    Checks index for the valid Event Entry
*
* @param      Index         Index of EventEntry to check
*             searchType    L7_uint32 (Get or GetNext or Set)
*             integerValue  L7_int32  (depends on get or set request)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventEntryGet(L7_uint32 searchType, L7_uint32 Index);

/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      Index         Index of EventEntry to check
*             
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonEventEntryNextGet(L7_uint32 *Index);

/*********************************************************************
*
* @purpose    Gets value of Description for Event Entry
*
* @param      Index         index of EventEntry
*             buf(output)    pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventDescriptionGet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of EventType for EventEntry
*
* @param      Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventTypeGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of community for Event Entry
*
* @param      Index         index of EventEntry
*             buf(output)   pointer to L7_uchar8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventCommunityGet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of lastTimeSent for Event Entry
*
* @param      Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventLastTimeSentGet(L7_uint32 Index, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of Owner for Event Entry
*
* @param      Index         index of EventEntry
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventOwnerGet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Gets value of Status for Event Entry
*
* @param      Index         index of EventEntry
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventStatusGet(L7_uint32 Index, L7_uint32 *val);

/********************* SET FUNCTIONS FOR EVENT GROUP ********************/

/*********************************************************************
*
* @purpose    Sets value of Description for Event entry
*
* @param      Index         index of the EventEntry
*             buf(input)    Pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventDescriptionSet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of EventType for Event entry
*
* @param      Index         index of the EventEntry
*             val(input)    pointer to L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventTypeSet(L7_uint32 Index, L7_uint32 val);

/*********************************************************************
*
* @purpose    Sets value of community for Event entry
*
* @param      Index         index of the EventEntry
*             buf(input)    Pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventCommunitySet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Owner for Event entry
*
* @param      Index         index of the EventEntry
*             buf(input)    pointer to L7_char8 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventOwnerSet(L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose    Sets value of Status for Event entry
*
* @param      Index         index of the EventEntry
*             val(input)    L7_uint32 to set the value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonEventStatusSet(L7_uint32 Index, L7_uint32 val);


/********************************** For LOG Table***********************************/ 

/************** GET FUNCTIONS for LOG TABLE ********************/

/*********************************************************************
*
* @purpose    Checks index for the valid Log Entry
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             searchType    L7_uint32 (Get or GetNext or Set)
*             integerValue  L7_int32  (depends on get or set request)
*
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonLogEntryGet(L7_uint32 searchType, L7_uint32 logEventIndex, L7_uint32 queueIndex);

/*********************************************************************
*
* @purpose    Checks index for  GET NEXT requests
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             
* @returntype L7_SUCCESS    if valid index
*             L7_FAILURE    if not a valid index
*          
* @notes      none
*
* @end
*
*********************************************************************/
L7_RC_t 
rmonLogEntryNextGet(L7_uint32 *logEventIndex, L7_uint32 *queueIndex);

/*********************************************************************
*
* @purpose    Gets value of LogTime for Log Entry
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             val(output)   pointer to L7_uint32 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonLogTimeGet(L7_uint32 logEventIndex, L7_uint32 queueIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose    Gets value of LogDescription for Log Entry
*
* @param      logEventIndex Index of LogEntry to check
*             queueIndex    Index of log in the logQueue
*             buf(output)   pointer to L7_char8 to receive value
*
* @returntype L7_SUCCESS  if member was found
*             L7_FAILURE  if member was not found
*          
* @notes      none
*
* @end
*********************************************************************/
L7_RC_t 
rmonLogDescriptionGet(L7_uint32 logEventIndex, L7_uint32 queueIndex, L7_char8 *buf);

#endif  /* RMON_EVENT_H */

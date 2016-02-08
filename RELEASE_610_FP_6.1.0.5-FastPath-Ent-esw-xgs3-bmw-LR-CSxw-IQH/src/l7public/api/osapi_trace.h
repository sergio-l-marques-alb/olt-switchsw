/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  osapi_trace.h
*
* @purpose   OS independent prototype declarations
*
* @component osapi
*
* @comments  none 
*
* @create    11/02/2003
*
* @author     
*
* @end
*
*********************************************************************/
#ifndef OSAPITRACE_HEADER
#define OSAPITRACE_HEADER

#include "l7_common.h"

#ifdef L7_TRACE_ENABLED
 #define OSAPI_TRACE_EVENT(event, buff, buff_len) (osapi_trace_event(event, (L7_uchar8*)buff, buff_len))
#if 0 /* Triggers don't work, and cause machine checks for some people */
 #define OSAPI_TRACE_TRIGGER(trigger)             (osapi_trace_trigger(trigger))
#else
 #define OSAPI_TRACE_TRIGGER(trigger) 
#endif
#else
 #define OSAPI_TRACE_EVENT(event, buff, buff_len) 
 #define OSAPI_TRACE_TRIGGER(trigger) 
#endif /* L7_TRACE_ENABLED */

typedef enum 
{
	L7_TRACE_EVENT_DRIVER_ENTER = 1,
	L7_TRACE_EVENT_DRIVER_EXIT = 2,

  L7_TRACE_EVENT_USMDB_VLAN_ADD_ENTER = 100,
  L7_TRACE_EVENT_USMDB_VLAN_ADD_EXIT = 101,
	/* Define more events here....
	*/

	L7_TRACE_EVENT_LAST = 39999
} osapiTraceEvents;

typedef enum 
{
  L7_TRACE_TRIGGER_FIRST = 40000,

  L7_TRACE_TRIGGER_USMDB_VLAN_ADD_ENTER = 40100,
  L7_TRACE_TRIGGER_USMDB_VLAN_ADD_EXIT = 40101,

	/* Define more triggers here....
	*/

	L7_TRACE_TRIGGER_LAST = 65535
} osapiTraceTriggers;

/**************************************************************************
*
* @purpose  Add event to the Operating System event utility.
*
* @param    event       Event Number
* @param    buff        Event Data
* @param    buff_len    Number of bytes in the event data
*
* @returns  None
*
* @comments   
*
* @end
*
*************************************************************************/
void osapi_trace_event (osapiTraceEvents event,
                        L7_uchar8 * buff,
                        L7_uint32 buff_len);

/**************************************************************************
*
* @purpose  Add trigger to the Operating System event utility.
*
* @param    trigger  Trigger Number
*
* @returns  None
*
* @comments   
*
* @end
*
*************************************************************************/
void osapi_trace_trigger (osapiTraceTriggers trigger);

#endif


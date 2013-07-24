/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_trace.h
*
* @purpose    Handle tracing for NIM
*
* @component  NIM
*
* @comments   none
*
* @create     03/25/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#ifndef NIM_TRACE_H
#define NIM_TRACE_H

#include "l7_common.h"
#include "trace_api.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "nimapi.h"

#define NIM_TRACE_REGISTRATION_NAME "NIM"
#define NIM_TRACE_DISPLAY_CLUSTER   8

/*
 * Bit definitions for the trace mask. By default, only trace start of each event.
 * To also trace event complete, set NIM_TRACE_FINI.
 */
#define NIM_TRACE_PORT_EVENTS              ((L7_uint32)(1 << 0))
#define NIM_TRACE_PORT_EVENT_PER_COMPONENT ((L7_uint32)(1 << 1))
#define NIM_TRACE_EVENT_ERROR              ((L7_uint32)(1 << 2))
#define NIM_TRACE_FINI                     ((L7_uint32)(1 << 3))

/* TRACE Events */
#define NIM_PORT_EVENTS_START              ((L7_ushort16)0x1)
#define NIM_PORT_EVENTS_FINI               ((L7_ushort16)0x2)
#define NIM_PORT_EVENTS_STRING             "NIM_EVENT>"      
#define NIM_PORT_EVENTS_PER_COMP_START     ((L7_ushort16)0x3)
#define NIM_PORT_EVENTS_PER_COMP_FINI      ((L7_ushort16)0x4)
#define NIM_PORT_EVENT_PER_COMP_STRING     "NIM_EVENTPCOMP>"      
#define NIM_ERR                            ((L7_ushort16)0x5)
#define NIM_ERR_STRING                     "NIMERR>"

/* 
 * Progress on a EVENT 
 */
#define NIM_EVENT_PROGRESS_START_STRING     "Start"
#define NIM_EVENT_PROGRESS_FINISHED_STRING  "Fini "

/*
 * NIM ERR Codes for Trace utility
 */
typedef enum 
{
  NIM_ERR_EVENT_INTF_DNE = 1,
  NIM_ERR_EVENT_OUT_OF_ORDER,
  NIM_ERR_EVENT_WRONG_PHASE,
  NIM_ERR_EVENT_INVALID_USP,
  NIM_ERR_LAST
} nimTraceErr_e;

typedef compTracePtHdr_t nimTracePtHdr_t;

/*
** Declarations used for profiling interface events
*/
typedef struct
{
  L7_uint32 totalTime;
  L7_uint32 tempTime;
} nimProfileInfo_t;

typedef struct
{
  nimProfileInfo_t eventInfo[L7_LAST_PORT_EVENT];
} nimProfileEvent_t;

typedef struct
{
  nimProfileEvent_t intfInfo[L7_MAX_INTERFACE_COUNT];
} nimProfileEventIntfInfo_t;

#define NIM_PROFILE_PORT_EVENT_PER_INTF      0x00000001
#define NIM_PROFILE_PORT_EVENT_PER_COMP      0x00000002
#define NIM_PROFILE_PORT_EVENT_PER_COMP_INTF 0x00000004


/* prototype of trace id formatting functions */
typedef L7_RC_t (*nimTraceFmtFunc_t)(L7_ushort16 traceId, L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Initialize the trace utility for NIM
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTraceInit(L7_uint32 num_entries, L7_uint32 width);

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTraceFini(void);

/*********************************************************************
* @purpose  Display the trace log for NIM
*
* @param    count     {(input)} The number of traces to show. 0 for all traces
* @param    db        {(input)} A search string on the first column of trace
* @param    unformat  {(input)} L7_TRUE if the output is to be unformatted.  
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTraceShow(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat);

/*********************************************************************
* @purpose  Enable tracing in the NIM component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTraceModeApply(L7_BOOL enable);

/*********************************************************************
* @purpose  Trace a port event
*
* @param    component   {(input)}   The component issuing the event
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    start       {(input)}   L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTracePortEvent(L7_COMPONENT_IDS_t component,
                              L7_PORT_EVENTS_t   event,
                              L7_uint32          intIfNum,
                              L7_BOOL            start,
                              NIM_HANDLE_t       handle);

/*********************************************************************
* @purpose  Trace a port event on a per component basis
*
* @param    component   {(input)} The component being traced
* @param    event       {(input)} The L7 event being issued
* @param    intIfNum    {(input)} The internal interface for the event
* @param    start       {(input)}  L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTracePortEventComp(L7_COMPONENT_IDS_t component,
                                  L7_PORT_EVENTS_t   event,
                                  L7_uint32          intIfNum,
                                  L7_BOOL            start,
                                  NIM_HANDLE_t       handle);

/*********************************************************************
* @purpose  Trace a event error
*
* @param    component   {(input)}   The component issuing the event
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    nimErr      {(input)}   an identifier for the type of error
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimTraceEventError(L7_COMPONENT_IDS_t component,
                        L7_PORT_EVENTS_t   event,
                        L7_uint32          intIfNum,
                        nimTraceErr_e      nimErr
                        );

/*********************************************************************
* @purpose  Profile a port event
*
* @param    component   {(input)}   The component issuing the event
* @param    event       {(input)}   The L7 event being issued
* @param    intIfNum    {(input)}   The internal interface for the event
* @param    start       {(input)}   L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimProfilePortEvent(L7_COMPONENT_IDS_t component,
                                L7_PORT_EVENTS_t   event,
                                L7_uint32          intIfNum,
                                L7_BOOL            start
                               );

/*********************************************************************
* @purpose  Profile a port event on a per component basis
*
* @param    component   {(input)} The component being traced
* @param    event       {(input)} The L7 event being issued
* @param    intIfNum    {(input)} The internal interface for the event
* @param    start       {(input)}  L7_TRUE if event is starting, L7_FALSE otherwise
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimProfilePortEventComp(L7_COMPONENT_IDS_t component,
                                    L7_PORT_EVENTS_t   event,
                                    L7_uint32          intIfNum,
                                    L7_BOOL            start
                                   );
/*********************************************************************
* @purpose  Allocate the resources required for profiling NIM
*
*
* @returns  void
*
* @end
*********************************************************************/
extern void nimProfileInit();

#endif

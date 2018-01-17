/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cardmgr_trace.h
*
* @purpose    Trace implementation for CMGR
*
* @component  CMGR
*
* @comments   none
*
* @create     02/08/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/
#ifndef CMGR_TRACE_H
#define CMGR_TRACE_H

#include "l7_common.h"
#include "trace_api.h"
#include "cardmgr_api.h"
#include "cardmgr.h"

/* Definitions for the trace mask */
#define CMGR_TRACE_CMPDU_EVENTS              ((L7_uint32)(1 << 0))
#define CMGR_TRACE_CARD_EVENTS               ((L7_uint32)(1 << 1))

/* CMPDU trace events */
#define CMGR_CMPDU_EVENTS_STRING   "CMPDU_EVENT>"      

/* Card insert/remove trace events */
#define CMGR_CARD_EVENTS_STRING    "CARD_EVENT>"      

/* Progress on a event */
#define CMGR_EVENT_PROGRESS_START_STRING     "Start"
#define CMGR_EVENT_PROGRESS_FINISHED_STRING  "Fini "

#define CMGR_TRACE_ENTRY_MAX                512                                  
#define CMGR_TRACE_ENTRY_SIZE_MAX           32                                   
#define CMGR_TRACE_REGISTRATION_NAME        "CMGR"
#define CMGR_TRACE_DISPLAY_CLUSTER          8
#define CMGR_TRACE_OBJ_VAL_LEN_MAX          8                                    
                                                                                 
/* Trace point identifiers */
#define CMGR_TRACE_ID_CMPDU_EVENT            0x1000
#define CMGR_TRACE_ID_CARD_EVENT             0x1001

/* cmgrTrace Header */
typedef compTracePtHdr_t cmgrTracePtHdr_t;

/* Cmpdu event trace fields */
typedef struct
{
  L7_cmpdu_packet_t eventId;                   /* 08-11 : Event-id    */
  L7_BOOL           eventStatus;               /* 12-15 : Status      */ 
  L7_uint32         fromUnitNum;               /* 16-19 : Unit number */
} cmgrTracePtCmpduEvent_t;

/* Card event trace fields */
typedef struct
{
  L7_uint32 eventId;                            /* 08-11 : Event-id    */
  L7_BOOL   eventStatus;                        /* 12-15 : Status      */ 
  L7_uint32 unitNum;                            /* 16-19 : Unit number */
  L7_uint32 slotNum;                            /* 20-23 : Slot number */
} cmgrTracePtCardEvent_t;

typedef enum
{
  CMGR_PROFILE_EVENT_CARD_CREATE = 0,
  CMGR_PROFILE_EVENT_CARD_INSERT,
  CMGR_PROFILE_EVENT_CARD_ATTACH,
  CMGR_PROFILE_EVENT_CARD_CONFIG_SLOTS,
  CMGR_PROFILE_EVENT_CARD_CONFIG_PORTS,
  CMGR_PROFILE_EVENT_CARD_LOCAL_UNIT_MGR,
  CMGR_PROFILE_EVENT_STARTUP_CREATE,
  CMGR_PROFILE_EVENT_STARTUP_ACTIVATE, 
  CMGR_PROFILE_EVENT_LAST
} cmgrProfileEvents_t;

typedef struct
{
  L7_uint32 tempTime;
  L7_uint32 totalTime;
} cmgrProfileInfo_t;

typedef struct
{
  cmgrProfileInfo_t eventInfo[CMGR_PROFILE_EVENT_LAST];
} cmgrProfileEvent_t;

/* prototype of trace id formatting functions */
typedef L7_RC_t (*cmgrTraceFmtFunc_t)(L7_ushort16 traceId, L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Initialize the trace utility for CMGR
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrTraceInit(L7_uint32 num_entries, L7_uint32 width);

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrTraceFini(void);

/*********************************************************************
* @purpose  Display the trace log for CMGR
*
* @param    count     {(input)} The number of traces to show. 0 for all traces
* @param    format  {(input)} L7_TRUE if the output is to be formatted.  
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
extern L7_RC_t cmgrTraceShow(L7_uint32 count, L7_BOOL format);

/*********************************************************************
* @purpose  Enable tracing in the CMGR component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrTraceModeApply(L7_BOOL enable);

/*********************************************************************
* @purpose  Trace a cmpdu event
*
* @param    traceId     {(input)}   Trace identifier
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
* @param    fromUnitNum {(input)}   Sender unit number
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrTraceCmpduEvent(L7_ushort16         traceId,
                                L7_cmpdu_packet_t   event,
                                L7_BOOL             start,
                                L7_uint32           fromUnitNum
                               );


/*********************************************************************
* @purpose  Trace a card event
*
* @param    traceId     {(input)}   Trace identifier
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
* @param    unitNum     {(input)}   Unit number
* @param    slotNum     {(input)}   Slot number
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrTraceCardEvent(L7_ushort16         traceId,
                               L7_cmpdu_packet_t   event,
                               L7_BOOL             start,
                               L7_uint32           unitNum,
                               L7_uint32           slotNum
                              );

/*********************************************************************
* @purpose  Profile a card event
*
* @param    event       {(input)}   The event being profiled
* @param    start       {(input)}   Indicates start or end of the event
* @param    unitNum     {(input)}   Unit number
* @param    slotNum     {(input)}   Slot number
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrProfileEvent(cmgrProfileEvents_t event,
                             L7_BOOL             start,
                             L7_uint32           unitNum,
                             L7_uint32           slotNum
                            );
/*********************************************************************
* @purpose  Allocate the resources required for profiling CMGR
*
*
* @returns  void
*
* @end
*********************************************************************/
extern void cmgrProfileInit();

/* MACRO defines to be called */
#define CMGR_TRACE_CMPDU_EVENT(event_id, start, from_unit_num) \
  cmgrTraceCmpduEvent(CMGR_TRACE_ID_CMPDU_EVENT, event_id, start, from_unit_num)

#define CMGR_TRACE_CARD_EVENT(event_id, start, unit_num, slot_num) \
  cmgrTraceCardEvent(CMGR_TRACE_ID_CARD_EVENT, event_id, start, unit_num, slot_num)

#define CMGR_PROFILE_EVENT(event_id, start, unit_num, slot_num) \
  cmgrProfileEvent(event_id, start, unit_num, slot_num)

#endif /* CMGR_TRACE_H */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   cnfgr_trace.h
*
* @purpose    Trace implementation for CNFGR
*
* @component  CNFGR
*
* @comments   none
*
* @create     02/25/2008
*
* @author     nshrivastav
* @end
*
**********************************************************************/
#ifndef CNGR_TRACE_H
#define CNGR_TRACE_H

#include "l7_common.h"
#include "trace_api.h"
#include "cnfgr_include.h"
#include "l7_cnfgr_api.h"

/*
 * Bit definitions for the trace mask 
 */
#define CNFGR_TRACE_EVENT               ((L7_uint32)(1 << 0))
#define CNFGR_TRACE_EVENT_PER_COMPONENT ((L7_uint32)(1 << 1))

/* TRACE Events */
#define CNFGR_EVENT_START              ((L7_ushort16)0x1)
#define CNFGR_EVENT_FINI               ((L7_ushort16)0x2)
#define CNFGR_EVENT_STRING             "CNFR_EVENT>"      
#define CNFGR_EVENT_PER_COMP_START     ((L7_ushort16)0x3)
#define CNFGR_EVENT_PER_COMP_FINI      ((L7_ushort16)0x4)
#define CNFGR_EVENT_PER_COMP_STRING     "CNFGR_EVENTPCOMP>"      

/* Progress on a event */
#define CNFGR_EVENT_PROGRESS_START_STRING     "Start"
#define CNFGR_EVENT_PROGRESS_FINISHED_STRING  "Fini "

#define CNFGR_TRACE_ENTRY_MAX                1024                                  
#define CNFGR_TRACE_ENTRY_SIZE_MAX           32                                  
#define CNFGR_TRACE_REGISTRATION_NAME        "CNGR"
#define CNFGR_TRACE_DISPLAY_CLUSTER          8
#define CNFGR_TRACE_OBJ_VAL_LEN_MAX          8                                    
                                                                                 
/* Header used by all trace points */
typedef compTracePtHdr_t cnfgrTracePtHdr_t;


/* Cnfgr event trace fields */
typedef struct
{
  L7_uint32         eventId;                   /* 08-11 : Event-id    */
  L7_uint32         correlator;                /* 12-15 : Component id */
  L7_uchar8         targetCompId;              /* 16    : Component id */
} cnfgrTracePt_t;


/*
** Declarations used for profiling interface events
*/
typedef struct
{
  L7_uint32 totalTime;
  L7_uint32 tempTime;
} cnfgrProfileInfo_t;

typedef struct
{
  cnfgrProfileInfo_t eventInfo[CNFGR_EVNT_LAST];
} cnfgrProfileEvent_t;

typedef struct
{
  cnfgrProfileInfo_t     eventInfo[L7_CNFGR_RQST_LAST -L7_CNFGR_RQST_FIRST];
  L7_uint32              correlator;
} cnfgrProfileCompInfo_t;

/* prototype of trace id formatting functions */
typedef L7_RC_t (*cnfgrTraceFmtFunc_t)(L7_ushort16 traceId, L7_uchar8 *pDataStart);

/*********************************************************************
* @purpose  Initialize the trace utility for CNFGR
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrTraceInit(L7_uint32 num_entries, L7_uint32 width);

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrTraceFini(void);

/*********************************************************************
* @purpose  Display the trace log for CNFGR
*
* @param    count     {(input)} The number of traces to show. 0 for all traces
* @param    format  {(input)} L7_TRUE if the output is to be formatted.  
*
* @returns  L7_RC_t
*
* @end
*********************************************************************/
extern L7_RC_t cnfgrTraceShow(L7_uint32 count, L7_BOOL format);

/*********************************************************************
* @purpose  Enable tracing in the CNFGR component
*
* @param    enable    {(input)} L7_TRUE if tracing is to be enabled
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrTraceModeApply(L7_BOOL enable);

/*********************************************************************
* @purpose  Trace a cnfgr event
*
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrTraceEvent(CNFGR_CTLR_EVNT_t   event,
                            L7_BOOL             start
                           );

/*********************************************************************
* @purpose  Trace a cnfgr event issued to a component
*
* @param    event       {(input)}   The event being traced
* @param    start       {(input)}   Indicates start or end of the event
* @param    compId      {(input)}   Indicates the compId acting on the event
* @param    correlator  {(input)}   Indicates the correlator for the component
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrTraceEventPerComp(L7_CNFGR_RQST_t     event,
                                   L7_BOOL             start,
                                   L7_uchar8           compId,
                                   L7_uint32           correlator
                                  );

/*********************************************************************
* @purpose  Start profiling of a cnfgr event
*
* @param    event       {(input)}   The cnfgr event being issued
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrProfileEventStart(CNFGR_CTLR_EVNT_t   event);

/*********************************************************************
* @purpose  Stop profiling of the current cnfgr event
*
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrProfileEventStop(void);

/*********************************************************************
* @purpose  Start profiling of current cnfgr event on a per component basis
*
* @param    event            {(input)}   The event being traced
* @param    compId           {(input)}   The compId being traced
* @param    correlator       {(input)}   The correlator of the component being traced
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrProfileEventCompStart(L7_CNFGR_RQST_t     event,
                                       L7_uchar8           compId,
                                       L7_uint32           correlator
                                      );

/*********************************************************************
* @purpose  Stop profiling of current cnfgr event on a per component basis
*
* @param    correlator       {(input)}   The correlator of the component being traced
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrProfileEventCompStop(L7_uint32 correlator);

/*********************************************************************
* @purpose  Allocate the resources required for profiling CNFGR
*
*
* @returns  void
*
* @end
*********************************************************************/
extern void cnfgrProfileInit(void);
#endif /* CNGR_TRACE_H */

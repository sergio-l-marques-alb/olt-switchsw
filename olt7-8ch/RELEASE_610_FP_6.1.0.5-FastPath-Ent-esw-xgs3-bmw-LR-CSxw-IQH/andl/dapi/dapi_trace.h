/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  dapi_trace.h
*
* @purpose   This file contains dapi trace API
*
* @component dapi
*
* @comments
*
* @create    11/1/2002
*
* @author    mbaucom
*
* @end
*
**********************************************************************/
#ifndef DAPI_TRACE_H
#define DAPI_TRACE_H

#include "l7_common.h"
#include "dapi.h"

#define DAPI_TRACE_REGISTRATION_NAME "DAPI"

#define DAPI_TRACE_DAPI_CTL       0
#define DAPI_TRACE_DAPI_CTL_MASK  (1 << DAPI_TRACE_DAPI_CTL) 

#define DAPI_TRACE_GENERIC        1
#define DAPI_TRACE_GENERIC_MASK   (1 << DAPI_TRACE_GENERIC)      

#define DAPI_TRACE_DAPI_CB        2
#define DAPI_TRACE_DAPI_CB_MASK   (1 << DAPI_TRACE_DAPI_CB)

#define DAPI_TRACE_FRAME	  3
#define DAPI_TRACE_FRAME_MASK     (1 << DAPI_TRACE_FRAME)

#define DAPI_TRACE_FRAME_SEND     ((L7_uchar8)1)
#define DAPI_TRACE_FRAME_DROP     ((L7_uchar8)2)
#define DAPI_TRACE_FRAME_FWD      ((L7_uchar8)3)

typedef struct
{
  L7_uint32 startTime;
  L7_uint32 totalTime;
  L7_uint32 maxTime;
  L7_uint32 numInvocations;
} dapiCmdProfileInfo_t;


/*********************************************************************
* @purpose  Enable the trace
*
* @param    enable		@b{(input)}	L7_TRUE to enable the trace
*									L7_FALSE to disable the trace
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceModeApply(L7_BOOL enable);

/*********************************************************************
* @purpose  Enable trace for a specific interface only
*
* @param    intIf	@b{(input)}	The compressed usp
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceIntIfSet(L7_uint32 unit, L7_uint32 slot, L7_uint32 port);

/*********************************************************************
* @purpose  Disable tracing for a specific dapi Command
*
* @param    cmd		@b{(input)}	The command being ignored
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceIgnoreCmdSet(DAPI_CMD_t cmd);

/*********************************************************************
* @purpose  Only let the trace buffer fill once
*
* @param    enable		@b{(input)}	L7_TRUE/L7_FALSE to enable/disable oneshot 
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceOneShotSet(L7_BOOL enable);

/*********************************************************************
* @purpose  Set the trace mask of what to trace
*
* @param    mask		@b{(input)}	The mask
*
* @returns  void
*
* @end
*********************************************************************/
L7_uint32 dapiTraceMaskSet(L7_uint32 mask);

/*********************************************************************
* @purpose  Initialize the trace utility for dapi
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
void  dapiTraceInit(L7_uint32 numEntries,L7_uint32 width);

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dapiTraceFini(void);

/*********************************************************************
* @purpose  Trace a dapiCtl call
*
* @param    cmd		@b{(input)} 	The command being traced
* @param    pUsp	@b{(input)} 	The port being acted upon
* @param    enter	@b{(input)} 	L7_TRUE if trace is prior to invocation of dapiCtl
*								L7_FALSE if the trace is after the invocation
* @param    rc		@b{(input)} 	The return code of the dapiCtl call
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceDapiCtl(DAPI_CMD_t cmd, DAPI_USP_t *pUsp, L7_BOOL enter,L7_RC_t rc);

/*********************************************************************
* @purpose  Trace a preformated string
*
* @param    format		@b{(input)} 	a format string to be placed directly in the trace
* @param    ...       @b{(input)} 	variable args for printf like formatting
*
* @returns  void
*
* @notes	The string will be truncated to the trace width
* @end
*********************************************************************/
void dapiTraceGeneric(L7_char8 * format, ...);



/*********************************************************************
* @purpose  Trace a Callback
*
* @param    event  	@b{(input)} The callback event being traced
* @param    pUsp  	@b{(input)} The port the callback is with reference to
* @param    enter	  @b{(input)} L7_TRUE if trace is prior to invocation of dapiCtl
*								                L7_FALSE if the trace is after the invocation
* @param    rc		  @b{(input)} The return code of the dapiCtl call
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceDapiCallback(DAPI_EVENT_t event, DAPI_USP_t *pUsp, L7_BOOL enter,L7_RC_t rc);

/*********************************************************************
* @purpose  Trace a frame
*
* @param    pUsp  @{(input)} The port receiving the packet
* @param    pkt  	@{(input)} The packet being traced
* @param    drop  @b{(input)} DAPI_TRACE_FRAME_SEND - Send to port from CPU
*                             DAPI_TRACE_FRAME_DROP - Drop a received packet
*                             DAPI_TRACE_FRAME_FWD  - Send to application from port  
* @param    cookie  @b{(input)} Driver dependent data placeholder
*
* @returns  void
*
* @end
*********************************************************************/
void dapiTraceFrame(DAPI_USP_t *pUsp, L7_uchar8 *pkt,L7_uchar8 drop,L7_uint32 cookie);

/*********************************************************************
* @purpose  Show the trace buffer
*
* @param    count  		@b{(input)} The number of events to show
* @param    db  		  @b{(input)} A 3 char string id to search on
* @param    unformat	@b{(input)} L7_TRUE  - for unformatted output
*           						          L7_FALSE - for formatted output
*
* @returns  void
*
* notes     Used with the trace show
* @end
*********************************************************************/
void dapiTraceShow(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat);


/*********************************************************************
* @purpose  Start profiling of a dapi cmd 
*
* @param    dapiCmd    {(input)}   The dapi command being profiled
* @param    start      {(input)}   Indicates start or end of the cmd
*
* @returns  void
*
* @end
*********************************************************************/
void dapiCmdProfile(DAPI_CMD_t dapiCmd, L7_BOOL start);

/*********************************************************************
* @purpose  Allocate the resources required for profiling CNFGR
*
*
* @returns  void
*
* @end
*********************************************************************/
void dapiProfileInit();

#endif

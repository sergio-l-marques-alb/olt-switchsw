/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2010
*
**********************************************************************
*
* @filename    timerange_debug.h
*
* @purpose     TIMERANGE debug functions
*
* @component   TIMERANGES
*
* @comments    none
*
* @create      29/11/2009
*
* @author      Siva Mannem
*
* @end
*
**********************************************************************/

#ifndef TIMERANGE_DEBUG_H
#define TIMERANGE_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "timerange.h"

#define TIMERANGE_USER_TRACE(__fmt__, __args__...)                              \
           LOG_USER_TRACE(L7_TIMERANGES_COMPONENT_ID, __fmt__, ##__args__);

#define TIMERANGE_DEBUG_PRINTF sysapiPrintf

#define TIMERANGE_MAX_DEBUG_MSG_SIZE 512

#define TIMERANGE_TRACE(traceFlag, __fmt__, __args__...)  { \
   if (timeRangeDebugTraceFlagCheck((traceFlag)) == L7_TRUE)\
   {\
     L7_char8  __buf1__[TIMERANGE_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf2__[TIMERANGE_MAX_DEBUG_MSG_SIZE];    \
     L7_timespec time;                                \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, TIMERANGE_MAX_DEBUG_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, TIMERANGE_MAX_DEBUG_MSG_SIZE,                    \
               "\n%02d:%02d:%02d TIMERANGE: In %s() %d - %s",                             \
       time.hours, time.minutes, time.seconds,\
       __FUNCTION__,__LINE__,__buf1__);       \
        TIMERANGE_DEBUG_PRINTF(__buf2__); \
   }\
 }


typedef enum
{
  TIMERANGE_DEBUG_TIMER = 0,
  TIMERANGE_DEBUG_STATE_CHANGE,
  TIMERANGE_DEBUG_CNFGR,
  TIMERANGE_DEBUG_MODIFY,
  TIMERANGE_DEBUG_FLAGS_BITS
} TIMERANGE_TRACE_FLAGS_t;



/*********************************************************************
* @purpose  Enable Debug Tracing in TIMERANGE
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t timeRangeDebugTraceEnable(void);


/*********************************************************************
* @purpose  Disable Debug Tracing in TIMERANGE
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/

L7_RC_t timeRangeDebugTraceDisable(void);


/*********************************************************************
* @purpose  Shows the usage of the TIMERANGE Debug Trace Utility
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void timeRangeDebugTraceHelp(void);


/*********************************************************************
* @purpose  Clear all trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void timeRangeDebugTraceAllFlagsReset(void);


/*********************************************************************
* @purpose  Shows the Enabled/Disabled Trace flags
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void timeRangeDebugTraceFlagsShow(void);


/*********************************************************************
* @purpose  Set a particular tracelevel
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void timeRangeDebugTraceFlagsSet(L7_uint32 flag);


/*********************************************************************
* @purpose  Reset a particular tracelevel
*
* @param    flag   {(input)} Trace Level
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/

void timeRangeDebugTraceFlagsReset(L7_uint32 flag);


/*********************************************************************
* @purpose
*
* @param    flag   {(input)} Trace Level
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/

L7_BOOL timeRangeDebugTraceFlagCheck (L7_uint32 traceFlag);

#endif /* TIMERANGE_DEBUG_H */

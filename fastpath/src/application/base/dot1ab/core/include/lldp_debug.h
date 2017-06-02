/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    lldp_debug.h
* @purpose     LLDP debug functions
* @component   LLDP
* @comments    none
* @create      03/14/2009
* @end
*
**********************************************************************/

#ifndef LLDP_DEBUG_H
#define LLDP_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "lldp.h"

#define LLDP_USER_TRACE(__fmt__, __args__...)                              \
           LOG_USER_TRACE(L7_LLDP_COMPONENT_ID, __fmt__, ##__args__);

#define LLDP_DEBUG_PRINTF sysapiPrintf

#define LLDP_MAX_DEBUG_MSG_SIZE 512

#define LLDP_TRACE(traceFlag, __fmt__, __args__...)  { \
   if (lldpDebugTraceFlagCheck((traceFlag)) == L7_TRUE)\
   {\
     L7_char8  __buf1__[LLDP_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf2__[LLDP_MAX_DEBUG_MSG_SIZE];    \
     L7_timespec time;                                \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, LLDP_MAX_DEBUG_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, LLDP_MAX_DEBUG_MSG_SIZE,                    \
               "\n%02d:%02d:%02d LLDP: In %s() %d - %s",                             \
       time.hours, time.minutes, time.seconds,\
       __FUNCTION__,__LINE__,__buf1__);       \
        LLDP_DEBUG_PRINTF(__buf2__); \
   }\
 }


typedef enum
{
  LLDP_DEBUG_TIMER = 0,
  LLDP_DEBUG_PROTO,
  LLDP_DEBUG_MED,
  LLDP_DEBUG_POE_MED,
  LLDP_DEBUG_CHECKPOINT,
  LLDP_DEBUG_CHECKPOINT_VERBOSE,
  LLDP_DEBUG_PROTO_VERBOSE,

  LLDP_DEBUG_FLAGS_BITS
} LLDP_TRACE_FLAGS_t;



/*********************************************************************
* @purpose  Enable Debug Tracing in LLDP
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

L7_RC_t lldpDebugTraceEnable(void);


/*********************************************************************
* @purpose  Disable Debug Tracing in LLDP
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

L7_RC_t lldpDebugTraceDisable(void);


/*********************************************************************
* @purpose  Shows the usage of the LLDP Debug Trace Utility
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

void lldpDebugTraceHelp(void);


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

void lldpDebugTraceAllFlagsReset(void);


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

void lldpDebugTraceFlagsShow(void);


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

void lldpDebugTraceFlagsSet(L7_uint32 flag);


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

void lldpDebugTraceFlagsReset(L7_uint32 flag);


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

L7_BOOL lldpDebugTraceFlagCheck (L7_uint32 traceFlag);

#endif /* LLDP_DEBUG_H */

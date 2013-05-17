/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    voice_vlan_debug.h
* @purpose     Voice VLAN debug functions
* @component   Voice VLAN
* @comments    none
* @create      03/17/2009
* @end
*
**********************************************************************/

#ifndef VOICE_VLAN_DEBUG_H
#define VOICE_VLAN_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "voice_vlan.h"

#define VOICE_VLAN_USER_TRACE(__fmt__, __args__...)                              \
           LOG_USER_TRACE(L7_VOICE_VLAN_COMPONENT_ID, __fmt__, ##__args__);

#define VOICE_VLAN_DEBUG_PRINTF sysapiPrintf

#define VOICE_VLAN_MAX_DEBUG_MSG_SIZE 512

#define VOICE_VLAN_TRACE(traceFlag, __fmt__, __args__...)  { \
   if (voiceVlanDebugTraceFlagCheck((traceFlag)) == L7_TRUE)\
   {\
     L7_char8  __buf1__[VOICE_VLAN_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf2__[VOICE_VLAN_MAX_DEBUG_MSG_SIZE];    \
     L7_timespec time;                                \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, VOICE_VLAN_MAX_DEBUG_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, VOICE_VLAN_MAX_DEBUG_MSG_SIZE,                    \
               "\n%02d:%02d:%02d VOICE_VLAN: In %s() %d - %s",                             \
       time.hours, time.minutes, time.seconds,\
       __FUNCTION__,__LINE__,__buf1__);       \
        VOICE_VLAN_DEBUG_PRINTF(__buf2__); \
   }\
 }


typedef enum
{
    VOICE_VLAN_TRACE_UNUSED = 0,
    VOICE_VLAN_TRACE_HIGH,
    VOICE_VLAN_TRACE_MEDIUM,
    VOICE_VLAN_TRACE_DEBUG,
    VOICE_VLAN_TRACE_CHECKPOINT,

    VOICE_VLAN_TRACE_FLAGS_BITS
} VOICE_VLAN_TRACE_FLAGS_t;



/*********************************************************************
* @purpose  Enable Debug Tracing in Voice VLAN
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

L7_RC_t voiceVlanDebugTraceEnable(void);


/*********************************************************************
* @purpose  Disable Debug Tracing in Voice VLAN
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

L7_RC_t voiceVlanDebugTraceDisable(void);


/*********************************************************************
* @purpose  Shows the usage of the Voice VLAN Debug Trace Utility
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

void voiceVlanDebugTraceHelp(void);


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

void voiceVlanDebugTraceAllFlagsReset(void);


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

void voiceVlanDebugTraceFlagsShow(void);


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

void voiceVlanDebugTraceFlagsSet(L7_uint32 flag);


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

void voiceVlanDebugTraceFlagsReset(L7_uint32 flag);


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

L7_BOOL voiceVlanDebugTraceFlagCheck (L7_uint32 traceFlag);


/*********************************************************************
* @purpose Trace voice vlan events - kept for compatibility with old
*          debug trace types
*
* @param   1 - enable tracing
*          0 - disable tracing
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/

void voiceVlanTraceEvent(L7_uint32 debug);

#endif /* VOICE_VLAN_DEBUG_H */

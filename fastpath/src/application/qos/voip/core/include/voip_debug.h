/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    voip_debug.h
* @purpose     voip debug functions
* @component   VOIP
* @comments    none
* @create      10/30/2007
* @author      pmurali
*              
* @end
*
**********************************************************************/

#ifndef VOIP_DEBUG_H
#define VOIP_DEBUG_H

#include "l7_common.h"
#include "log.h"
#include "sysapi.h"
#include "osapi.h"

#define  VOIP_MAX_DEBUG_MSG_SIZE 512

#define VOIP_DEBUG_PRINTF sysapiPrintf
/*********************************************************************
*
* @purpose  Sets the VOIP Protocol Trace Flag
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_uint32 Flag      (input)  Flag value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
voipDebugTraceFlagSet (L7_uint32 Protocol,L7_uint32 Flag);


/*********************************************************************
*
* @purpose  Gets the VOIP Protocol Trace Flag
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_uint32 Flag      (input)  Flag value
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
voipDebugTraceFlagGet (L7_uint32 Protocol,L7_uint32 *Flag);

/*********************************************************************
*
* @purpose  Gets the VOIP Protocol String
*
* @param  L7_uint32 Protocol  (input)  VoIP Protocol
* @param  L7_char8 *Str      (output) VoIP Protocol String
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t
voipDebugVoIPProtoGet (L7_uint32 Protocol,L7_char8 *Str);

/*********************************************************************
*
* @purpose
*
* @param    flag   {(input)} Trace Level
* @param    family {(input)} Snoop Instance
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/

L7_BOOL
voipDebugTraceFlagCheck (L7_uint32 traceFlag, L7_uint32 protocol);

typedef enum voipDebugInfo_s
{
   VOIP_DEBUG_NONE=0,
   VOIP_DEBUG_INFO,
   VOIP_DEBUG_ERROR,
   VOIP_DEBUG_CALL
}voipDebugInfo_t;

#define VOIP_TRACE(traceFlag , protocol, __fmt__, __args__...)  { \
   if (voipDebugTraceFlagCheck ((traceFlag),(protocol)) == L7_ENABLE)\
   {\
     L7_char8  __buf1__[VOIP_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf2__[VOIP_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf3__[VOIP_MAX_DEBUG_MSG_SIZE];    \
     L7_timespec time;                                \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, VOIP_MAX_DEBUG_MSG_SIZE, __fmt__, ## __args__);  \
      if (voipDebugVoIPProtoGet((protocol), (__buf3__)) == L7_SUCCESS)   \
      {\
        (void)osapiSnprintf (__buf2__, VOIP_MAX_DEBUG_MSG_SIZE,                    \
                 "\n%02d:%02d:%02d :%s: Function:%s() Line:%d - %s",                             \
         time.hours, time.minutes, time.seconds, __buf3__,\
         __FUNCTION__,__LINE__,__buf1__);       \
          VOIP_DEBUG_PRINTF(__buf2__); \
      }\
   }\
 }

#define VOIP_GET_LONG(val, cp) \
  do { \
       memcpy(&(val),(cp), sizeof(L7_uint32));\
       (val) = osapiNtohl((val));\
     } while (0)

#endif

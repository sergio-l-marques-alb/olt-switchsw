/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename sflow_debug.h
*
* @purpose   defines of sFlow debug routines
*
* @component sflow
*
* @comments  
*
* @create 20-Nov-2007
*
* @author drajendra
* @end
*
**********************************************************************/
#ifndef SFLOW_DEBUG_H
#define SFLOW_DEBUG_H
#include "osapi.h"

#define SFLOW_USER_TRACE_TX(__fmt__, __args__... )                              \
           LOG_USER_TRACE(L7_SFLOW_COMPONENT_ID, __fmt__,##__args__);


#define SFLOW_DEBUG_PRINTF sysapiPrintf
#define SFLOW_MAX_DEBUG_MSG_SIZE 512
#define SFLOW_IP6_ADDR_BUFF_SIZE 42

#define SFLOW_TRACE(traceFlag,__fmt__, __args__...)  { \
   if (sFlowDebugTraceFlagCheck ((traceFlag)) == L7_TRUE)\
   {\
     L7_char8  __buf1__[SFLOW_MAX_DEBUG_MSG_SIZE];    \
     L7_char8  __buf2__[SFLOW_MAX_DEBUG_MSG_SIZE];    \
     L7_timespec time;                                \
      osapiUpTime(&time);                             \
     (void)osapiSnprintf (__buf1__, SFLOW_MAX_DEBUG_MSG_SIZE, __fmt__, ## __args__);  \
          (void)osapiSnprintf (__buf2__, SFLOW_MAX_DEBUG_MSG_SIZE,                    \
               "\n%02d:%02d:%02d SFLOW: In %s() %d - %s",                             \
       time.hours, time.minutes, time.seconds,\
       __FUNCTION__,__LINE__,__buf1__);       \
        SFLOW_DEBUG_PRINTF(__buf2__); \
   }\
 }

typedef enum
{
  SFLOW_DEBUG_TIMER = 0,
  SFLOW_DEBUG_SAMPLER,
  SFLOW_DEBUG_INTERNAL,
  SFLOW_DEBUG_EVENTS,

  SFLOW_DEBUG_FLAGS_BITS
} SFLOW_TRACE_FLAGS_t;
/*********************************************************************
* @purpose Trace sFlow packets transmitted
*
* @param   rcvrIp         @b{(input)} destination receiver ip address
* @param   payloadLen     @b{(input)} sFlow datagram length
* @param   dgramSqN       @b{(input)} sFlow datagram sequence number
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void sFlowDebugPacketTxTrace(L7_inet_addr_t *rcvrIp, L7_uint32 payloadLen,
                             L7_uint32 dgramSqNo);
/*********************************************************************
*
* @purpose
*
* @param    flag   {(input)} Trace Level
*
* @returns  L7_TRUE,  if enabled
* @returns  L7_FALSE, if disabled
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL
sFlowDebugTraceFlagCheck (L7_uint32 traceFlag);
/*********************************************************************
* @purpose  Save configuration settings for sFlow trace data of a
*           sFlow agent
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t sFlowDebugPacketTraceFlagSave(void);
/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void sFlowpDebugCfgUpdate(void);
/*********************************************************************
* @purpose  Read and apply the debug config of a sFlow agent
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void sFlowDebugCfgRead(void);
/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void sFlowDebugRegister(void);
/*********************************************************************
* @purpose  Restores sFlow debug configuration of sFlow agent
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sFlowDebugRestore(void);
#endif /* SFLOW_DEBUG_H */

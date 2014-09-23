/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   rip_map_debug.h
*
* @purpose    RIP Mapping layer debug header files
*
* @component  RIP Mapping Layer
*
* @comments   none
*
* @create    08/21/2003
*
* @author     wjacobs
* @end
*
**********************************************************************/

#include "log.h"

#ifndef _RIP_MAP_DEBUG_H_
#define _RIP_MAP_DEBUG_H_

extern L7_BOOL ripMapPacketDebugFlag;


#ifdef RIPMAP_DEBUG_LOG


#define RIPMAP_TRACE(format,args...)                       \
{                                                           \
   if ( ripMapTraceModeGet() == L7_ENABLE)                 \
   {                                                        \
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,format,##args); \
   }                                                        \
}

#define RIPMAP_ERROR(format,args...)                       \
{                                                           \
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,format,##args); \
}


#else

#define RIPMAP_TRACE(format,args...)                       \
{                                                           \
   if ( ripMapTraceModeGet() == L7_ENABLE)                 \
   {                                                        \
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_RIPMAP,format,##args);  \
   }                                                        \
}

#define RIPMAP_ERROR(format,args...)                           \
{                                                               \
   SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_RIPMAP_ERROR,format,##args);   \
}

#endif


/* Debugging prototypes */
void ripMapTraceLinkerAssist(void);

/*********************************************************************
*
* @purpose Trace rip packets received
*
* @param   rtrIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
* @param   cc   @b{(input)} packet size
* @param   *srcIp   @b{(input)} Source Ip Address
* @param   *destIp  @b{(input)} Dest IP Address
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void ripMapDebugPacketRxTrace(L7_uint32 rtrIfNum,L7_uchar8 *buff, L7_int32 cc, L7_uchar8 *srcIp, L7_uchar8 *destIp);

/*********************************************************************
*
* @purpose Trace rip packets transmitted
*
* @param   rtrIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
* @param   cc   @b{(input)} packet size
* @param   *srcIp   @b{(input)} Source Ip Address
* @param   *destIp  @b{(input)} Dest IP Address
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void ripMapDebugPacketTxTrace(L7_uint32 rtrIfNum,L7_uchar8 *buff, L7_int32 cc, L7_uchar8 *srcIp, L7_uchar8 *destIp);

#endif /* _RIP_MAP_DEBUG_H_ */

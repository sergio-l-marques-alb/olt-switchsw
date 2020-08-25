
#ifndef _PIMSM_DEBUG_H
#define _PIMSM_DEBUG_H

#include "pimsmdefs.h"



/***********************************************************************/
/* ********************** DEFINES ********************************** */

int  pimsm_global_debug_trace;

typedef enum
{
  PIMSM_DEBUG_TRACE_ALWAYS_PRINT = 0x0000,
  PIMSM_DEBUG_TRACE_GENERAL = 0x0001,
  PIMSM_DEBUG_TRACE_JOINPRUNE = 0x0002,
  PIMSM_DEBUG_TRACE_ASSERT = 0x0004,
  PIMSM_DEBUG_TRACE_TIMERS = 0x0008,
  PIMSM_DEBUG_TRACE_DUMP_PKT = 0x0010,
  PIMSM_DEBUG_TRACE_REGISTER = 0x0020,
  PIMSM_DEBUG_TRACE_IGMP = 0x0040,
  PIMSM_DEBUG_TRACE_CACHE_UPDATE = 0x0080,
  PIMSM_DEBUG_TRACE_MAX = 0xffff
}PIMSM_TRACE_LEVEL;




/***************************************************************************/
/**************************Prototype Declarations **************************/
extern void pimsm_debug_trace(PIMSM_TRACE_LEVEL trace_level,char *fmt, ...);

extern void  pimsm_dump_pkt_buffer(char *buffer,int len);

extern void pimsm_set_debug_trace(int level);

#endif

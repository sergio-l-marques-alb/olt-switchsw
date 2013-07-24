/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   rip_debug.h
*
* @purpose    RIP application debug function
*
* @component  RIP
*
* @comments   none
*
* @create     03/31/2003
*
* @author     skanchi
* @end
*
**********************************************************************/

#ifndef _RIP_DEBUG_H_
#define _RIP_DEBUG_H_

#define RIP_DEBUG_TRACE(tracLev,format, args...)  {\
        if (ripTrace.level & (tracLev)) \
        { \
          char trace_buf[(RIP_TRACE_ENTRY_LEN + 1) * 2]; \
          osapiSnprintf(trace_buf,sizeof(trace_buf), format, ##args); \
          ripDebugTraceLog(trace_buf); \
        } \
        }


#define RIP_TRACE_OFF1             0x0001
#define RIP_TRACE_REQUEST          0x0002      /* ALL includes this */
#define RIP_TRACE_REQUEST_DETAIL   0x0004
#define RIP_TRACE_RESPONSE         0x0008      /* ALL includes this */
#define RIP_TRACE_RESPONSE_DETAIL  0x0010
#define RIP_TRACE_NORMAL           0x0020      /* ALL includes this */
                                               /* NORMAL includes interface transitions */
#define RIP_TRACE_POLICY           0x0040      /* ALL includes this */
#define RIP_TRACE_ROUTE            0x0080      /* ALL includes this */
#define RIP_TRACE_STATE            0x0100      /* ALL includes this */
#define RIP_TRACE_TASK             0x0200      /* ALL includes this */
#define RIP_TRACE_TIMER            0x0400      /* ALL includes this */
#define RIP_TRACE_OUTPUT_DETAIL    0x0800
#define RIP_TRACE_ALL              0x17EA


typedef struct ripTraceBuf_s
{
  char * buf;
  unsigned int buf_length;
  char * curr_buf;
  unsigned int curr_buf_length;
  unsigned int intf_ip;
  unsigned int num_entries;
}ripTraceBuf_t;

typedef struct ripTraceLevel_s
{
  unsigned int level;
  ripTraceBuf_t *ripTraceBuf;
} ripTraceLevel_t;

#define RIP_TRACE_ENTRY_LEN     124
#define RIP_TRACE_NUM_ENTRIES   1000

typedef struct ripTraceMsgFmt_s
{
  unsigned int now_time;
  char log_msg_fmt[RIP_TRACE_ENTRY_LEN + 1]; /* calc the size of now_time and deduct it from this */
} ripTraceMsgFmt_t;


extern ripTraceLevel_t ripTrace;

extern void ripDebugSystemInitalize();
extern void ripDebugInit();
extern void ripDebugDeInit();
extern void ripDebugTraceStart(unsigned int level, unsigned int num_entries, unsigned int target_intf_ip);
extern void ripDebugTraceStop();
extern void ripDebugTraceLevelChg(unsigned int level);
extern void ripDebugTraceShow();
extern void ripDebugTraceLog(char * rip_log);
extern void ripDebugTraceHelp();


#endif /* _RIP_DEBUG_H_ */


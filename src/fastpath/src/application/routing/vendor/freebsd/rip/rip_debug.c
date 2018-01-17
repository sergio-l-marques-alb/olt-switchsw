/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   rip_debug.c
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

#include "defs.h"
#undef printf
#include "pathnames.h"
#include "l7_common.h"
#include "l7_common_l3.h"
#include "osapi.h"

ripTraceLevel_t ripTrace;


void ripDebugSystemInitalize();
void ripDebugInit();
void ripDebugDeInit();
void ripDebugTraceStart(unsigned int level, unsigned int num_entries, unsigned int target_intf_ip);
void ripDebugTraceStop();
void ripDebugTraceLevelChg(unsigned int level);
void ripDebugTraceShow();
void ripDebugTraceLog(char * rip_log);
void ripDebugTraceHelp();


void ripDebugSystemInitalize()
{
  bzero((char *)&ripTrace, sizeof(ripTrace));
  return;
}

void ripDebugInit()
{
  if (ripTrace.ripTraceBuf != 0)
  {
    printf("Trace already initialized.\n");
    return;
  }
  ripTrace.ripTraceBuf = (ripTraceBuf_t *) malloc(sizeof(ripTraceBuf_t));
  if (ripTrace.ripTraceBuf == 0)
  {
    printf("Mem alloc failure. Trace init failed!!!!!!!!!!!!\n");
    return;
  }
  bzero((char *)ripTrace.ripTraceBuf, sizeof(ripTraceBuf_t));
  printf("Trace init completed.\n");
  return;
}

void ripDebugDeInit()
{
  if (ripTrace.ripTraceBuf == 0)
  {
    printf("Trace already deinitalized.\n");
    return;
  }
  if (ripTrace.ripTraceBuf->buf != 0)
    free((void *) ripTrace.ripTraceBuf->buf);
  bzero((char *)ripTrace.ripTraceBuf, sizeof(ripTraceBuf_t));
  if (ripTrace.ripTraceBuf != 0)
    free ((void *) ripTrace.ripTraceBuf);
  ripTrace.ripTraceBuf = 0;
  ripTrace.level = 0;
  printf("Trace deinitalized.\n");
  return;
}

void ripDebugTraceStart(unsigned int level, unsigned int num_entries, unsigned int target_intf_ip)
{
  if ((ripTrace.level != 0) && (ripTrace.level != RIP_TRACE_OFF1))
  {
	printf("Trace already in progress.\n");
	return;
  }
  if ((level & RIP_TRACE_OFF1) == RIP_TRACE_OFF1)
  {
    printf("Bad level.\n");
    return;
  }
  if (ripTrace.ripTraceBuf == 0)
  {
    printf("Trace not initialized. Use ripDebugInit() to initalize.\n");
    return;
  }
  if (num_entries == 0)
    num_entries = RIP_TRACE_NUM_ENTRIES;

  if (ripTrace.ripTraceBuf->buf != 0)
  {
	bzero(ripTrace.ripTraceBuf->buf, ripTrace.ripTraceBuf->buf_length);
    free((void *) ripTrace.ripTraceBuf->buf);
  }
  ripTrace.ripTraceBuf->buf = 0;
  ripTrace.ripTraceBuf->buf = (char *) malloc (num_entries * (RIP_TRACE_ENTRY_LEN + 1));
  if (ripTrace.ripTraceBuf->buf == 0)
  {
    printf("Mem alloc failure. Trace level set failed !!!!!!!.\n");
    return;
  }
  ripTrace.ripTraceBuf->buf_length = num_entries * (RIP_TRACE_ENTRY_LEN + 1);
  bzero(ripTrace.ripTraceBuf->buf, ripTrace.ripTraceBuf->buf_length);
  ripTrace.ripTraceBuf->curr_buf = ripTrace.ripTraceBuf->buf;
  ripTrace.ripTraceBuf->curr_buf_length = 0;
  ripTrace.ripTraceBuf->num_entries = num_entries;
  ripTrace.ripTraceBuf->intf_ip = target_intf_ip;
  ripTrace.level = level;
  printf("Trace started..\n");
  return;
}

void ripDebugTraceStop()
{
  ripTrace.level = RIP_TRACE_OFF1;
  printf("Trace stopped...\n");
  return;
}

void ripDebugTraceLevelSet(unsigned int level)
{
  if(0 == level)
    ripTrace.level = RIP_TRACE_OFF1;
  else
    ripTrace.level = level;
}

void ripDebugTraceLevelChg(unsigned int level)
{
  if ((level & RIP_TRACE_OFF1) == RIP_TRACE_OFF1)
  {
    printf("Bad level.\n");
    return;
  }
  
  if (ripTrace.ripTraceBuf == 0)
  {
    printf("Trace not initialized. Use ripDebugInit() to initalize.\n");
    return;
  }
  
  if (ripTrace.ripTraceBuf->buf == 0)
  {
	printf("Trace not started. Use ripDebugTraceStart().\n");
	return;
  }
  ripTrace.level = level;
  printf("Trace level changed.\n");
  return;
}

void ripDebugTraceShow()
{
  unsigned i, tmp_buf_len;
  char *tmp_buf;
  L7_timespec time;
  ripTraceMsgFmt_t * ripTraceMsgFmt;


  if (ripTrace.ripTraceBuf == 0)
  {
    printf("Trace not initialized. Use ripDebugInit() to initalize.\n");
    return;
  }
  if (ripTrace.ripTraceBuf->buf == 0)
  {
    printf("Trace not started. Use ripDebugTraceLevelSet to start trace.\n");
    return;
  }
  if ((ripTrace.level & RIP_TRACE_OFF1) != RIP_TRACE_OFF1)
  {
    printf("Turn off trace first.\n");
    return;
  }

  tmp_buf = ripTrace.ripTraceBuf->buf;
  tmp_buf_len = 0;
  for (i = 0; i < ripTrace.ripTraceBuf->num_entries; i++)
  {
	ripTraceMsgFmt = ( ripTraceMsgFmt_t * )tmp_buf;
	osapiConvertRawUpTime ( ripTraceMsgFmt->now_time, &time);
	printf("%02d:%02d:%02d %s\n",
			time.hours,
			time.minutes,
			time.seconds,
			ripTraceMsgFmt->log_msg_fmt);
    tmp_buf += (RIP_TRACE_ENTRY_LEN + 1);
    tmp_buf_len += (RIP_TRACE_ENTRY_LEN + 1);
    if (tmp_buf_len >= ripTrace.ripTraceBuf->curr_buf_length)
      return;
  }
  return;
}

void ripDebugTraceLog(char * rip_log)
{
  ripTraceMsgFmt_t * ripTraceMsgFmt;
  int length;
  L7_timespec time;

  if (ripTrace.ripTraceBuf == 0)
  {
    osapiConvertRawUpTime (osapiUpTimeRaw(), &time);
    printf("%02d:%02d:%02d %s\n",
        time.hours,
        time.minutes,
        time.seconds,
        rip_log);
    return;
  }

  if (ripTrace.ripTraceBuf->curr_buf_length  >= ripTrace.ripTraceBuf->buf_length)
  {
    printf("Trace buffer full. Stopping the trace.\n");
    ripTrace.level = RIP_TRACE_OFF1;
    return;
  }
  ripTraceMsgFmt = (ripTraceMsgFmt_t *)ripTrace.ripTraceBuf->curr_buf;
  ripTraceMsgFmt->now_time = osapiUpTimeRaw();
  length = (RIP_TRACE_ENTRY_LEN - sizeof(ripTraceMsgFmt->now_time)) > strlen(rip_log) ? strlen(rip_log) :
                                                 (RIP_TRACE_ENTRY_LEN - sizeof(ripTraceMsgFmt->now_time));
  strncpy(ripTraceMsgFmt->log_msg_fmt , rip_log, length);
  ripTraceMsgFmt->log_msg_fmt[length + sizeof(ripTraceMsgFmt->now_time)] = '\0';
  ripTrace.ripTraceBuf->curr_buf += (RIP_TRACE_ENTRY_LEN + 1);
  ripTrace.ripTraceBuf->curr_buf_length += (RIP_TRACE_ENTRY_LEN + 1);
  return;
}

void ripDebugTraceHelp()
{
  printf("ripDebugInit() : Initalize debugging.\n");
  printf("ripDebugDeInit() : Deinitialize debugging.\n");
  printf("ripDebugTraceStart(unsigned int level, unsigned int num_entries, unsigned int target_intf_ip) : \n");
  printf("                     Start the trace. Clears memory of previous trace.\n");
  printf("ripDebugTraceStop() : Stops the trace.\n");
  printf("ripDebugTraceLevelChg(unsigned int level) : Changes the trace level.\n");
  printf("ripDebugTraceShow() : Display the trace log.\n");

  printf("\n\nLevels: \n");
  printf("RIP_TRACE_OFF1             0x0001\n");
  printf("RIP_TRACE_REQUEST          0x0002      /* ALL includes this */\n");
  printf("RIP_TRACE_REQUEST_DETAIL   0x0004\n");
  printf("RIP_TRACE_RESPONSE         0x0008      /* ALL includes this */\n");
  printf("RIP_TRACE_RESPONSE_DETAIL  0x0010\n");
  printf("RIP_TRACE_NORMAL           0x0020      /* ALL includes this */\n");
  printf("                                       /* NORMAL includes interface transitions */\n");
  printf("RIP_TRACE_POLICY           0x0040      /* ALL includes this */\n");
  printf("RIP_TRACE_ROUTE            0x0080      /* ALL includes this */\n");
  printf("RIP_TRACE_STATE            0x0100      /* ALL includes this */\n");
  printf("RIP_TRACE_TASK             0x0200      /* ALL includes this */\n");
  printf("RIP_TRACE_TIMER            0x0400      /* ALL includes this */\n");
  printf("RIP_TRACE_OUTPUT_DETAIL    0x0800\n");
  printf("RIP_TRACE_ALL              0x07EA\n");
}

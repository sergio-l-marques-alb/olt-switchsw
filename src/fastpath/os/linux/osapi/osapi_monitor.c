/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  osapi_monitor.c
*
* @purpose   Monitor FastPath operation
*            
*
* @component osapi
*
* @create    06/15/2006
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>

#include "l7_common.h"
#include "l7_resources.h"
#include "osapi.h"
#include "sysapi.h"
#include "bspapi.h"
#include "simapi.h"

#define CPU_MEMORY_MONITOR_INTERVAL (60*1000) /* msec */

L7_int32 serveWDflag = 1;
static L7_uint32 osapiMemoryMonitorInterval = CPU_MEMORY_MONITOR_INTERVAL;
static L7_uint32 osapiMonitorTaskSleepInterval = CPU_MEMORY_MONITOR_INTERVAL;

#ifdef L7_MEMORY_PROFILER_ENABLE
extern void osapiMemoryProfilerNotify(void);
#endif

void setWDflag(int value)
{
  serveWDflag = value;
  printf("serveWDflag = %d\n", serveWDflag);
}

/*********************************************************************
* @purpose  Set the monitor task sleep interval
*
* @param    Monitor interval in milli-seconds
*
* @returns  none.
* 
* @comments If the stack margin is less than this, then error messages will be logged. 
*
* @end
*********************************************************************/
void osapiMonitorTaskSleepIntervalSet(L7_uint32 interval)
{
  osapiMonitorTaskSleepInterval = interval;
}

/*********************************************************************
* @purpose  Task responsible for monitoring FastPath performance.
*
* @param    none.
*
* @returns  none.
* 
* @comments This task is responsible for monitoring system performance.
*           It must run continiously, therefore it cannot be modified to
*           wait an a semaphore.
*
* @end
*********************************************************************/
void osapiMonitorTask()
{
  L7_uint64 currentTime = 0, lastMemoryMonitorTime = 0;

  do
  {
    osapiSleepMSec(osapiMonitorTaskSleepInterval);

  if (serveWDflag)
  {
    bspapiWatchdogService();
  }

#ifdef L7_MEMORY_PROFILER_ENABLE
  osapiMemoryProfilerNotify();
#endif

    currentTime = osapiTimeMillisecondsGet64();
    if ((currentTime - lastMemoryMonitorTime) >= osapiMemoryMonitorInterval)
    {
      simMemoryMonitor();    
      lastMemoryMonitorTime = currentTime;
    }

  } while (1);
  
  return;
}

/*********************************************************************
* @purpose  Initialize the osapiMonitor timer
*
* @param    none.
*
* @returns  none.
* 
* @comments none.
*
* @end
*********************************************************************/
void osapiMonitorInit()
{

  osapiMonitorTaskSleepInterval = bspapiWatchdogInterval();

  if (osapiTaskCreate("osapiMonTask", (void *)osapiMonitorTask, 0, 0,
                      L7_DEFAULT_STACK_SIZE, L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Error starting osapiMonitorTask\n");
    L7_LOG_ERROR(0);
  }

  bspapiWatchdogEnable();

  return;
}


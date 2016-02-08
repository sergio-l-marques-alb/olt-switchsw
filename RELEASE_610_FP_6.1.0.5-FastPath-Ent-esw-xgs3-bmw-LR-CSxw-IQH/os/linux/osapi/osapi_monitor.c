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

int serveWDflag = 1;
static osapiTimerDescr_t *osapiMonitorTimer       = L7_NULLPTR;

#ifdef L7_MEMORY_PROFILER_ENABLE
extern void osapiMemoryProfilerNotify(void);
#endif

void setWDflag(int value)
{
  serveWDflag = value;
  printf("serveWDflag = %d\n", serveWDflag);
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
  if (serveWDflag)
  {
    bspapiWatchdogService();
  }

#ifdef L7_MEMORY_PROFILER_ENABLE
  osapiMemoryProfilerNotify();
#endif

  osapiTimerAdd((void *)osapiMonitorTask, L7_NULL, L7_NULL,
                bspapiWatchdogInterval(), &osapiMonitorTimer);

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

  osapiTimerAdd((void *)osapiMonitorTask, L7_NULL, L7_NULL,
                bspapiWatchdogInterval(), &osapiMonitorTimer);

  bspapiWatchdogEnable();

  return;
}


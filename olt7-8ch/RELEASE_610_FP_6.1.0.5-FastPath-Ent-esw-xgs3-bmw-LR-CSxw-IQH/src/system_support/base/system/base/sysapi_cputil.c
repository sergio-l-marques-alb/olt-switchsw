/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename  sysapi_cpuutil.c
 *
 * @purpose   Track CPU utilization
 *
 * @component sysapi
 *
 * @create    07/01/2008
 *
 * @author    bradyr
 * @end
 *
 *********************************************************************/
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "l7_common.h"
#include "osapi_sem.h"
#include "osapi.h"
#include "osapi_support.h"
#include "pthread.h"
#include "sysapi.h"

task_util_t bigIdList[LVL7_TASK_UTIL_MAX_TASKS];
L7_int32 bigListTotal;

L7_int32 lvl7percentutiltbl_idx;
void *lvl7taskSema;

L7_int32 lvl7percentutiltbl[(LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS];

/*********************************************************************
* @purpose  Insert a task ID in the global task list
*
* @param    index    @b{(input)}  task ID to insert
*
* @returns  L7_uint32 index of bigIdList insertion point
*
* @comments none
*
* @end
*
*********************************************************************/
L7_uint32 bigIdListInsert(L7_int32 taskId)
{
  bigIdList[bigListTotal++].taskId = taskId;

  return (bigListTotal-1);
}

/*********************************************************************
* @purpose  Sort the global task list to allow binary searches.
*
* @returns  none
*
* @comments Bubble sort.
*
* @end
*
*********************************************************************/
void bigIdListSort()
{
  L7_uint32 i, j;
  task_util_t temp;

  /* bubble sort the list */
  j = 0;
  for (i=0; i<bigListTotal-1; i++)
  {
    for (j=bigListTotal-1; j>i; j--)
    {
      memcpy(&temp,&bigIdList[j],sizeof(task_util_t));
      if (temp.taskId < bigIdList[j-1].taskId)
      {
        memcpy(&bigIdList[j],&bigIdList[j-1],sizeof(task_util_t));
        memcpy(&bigIdList[j-1],&temp,sizeof(task_util_t));
      }
    }
  }
}

/*********************************************************************
* @purpose  Find the index in the task list, given the task ID.
*
* @param    index    @b{(input)}  task ID to search for
* @param    index    @b{(output)}  pointer to index into global task list
*
* @returns  L7_TRUE  if task ID was found in the list
*           L7_FALSE if task ID not found
*
* @comments Search uses binary search (instead of sequential)
*           to save cycles.
*
* @end
*
*********************************************************************/
L7_BOOL bigIdListFind(L7_int32 taskId, L7_uint32 *index)
{
  L7_int32 i;
  L7_int32 l = -1;
  L7_int32 r = bigListTotal;     /* l and r are beyond the bounds of list */

  /* binary search the list */
  while (l+1 != r)               /* Stop when l and r meet */
  {
    i = (l+r)/2;                 /* Look at middle of remaining subarray */
    if (taskId < bigIdList[i].taskId)
      r = i;                     /* In left half */
    if (taskId == bigIdList[i].taskId)
    {                            /* Found it */
      *index = i;
      return L7_TRUE;
    }
    if (taskId > bigIdList[i].taskId)
      l = i;                     /* In right half */
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Delete a task ID in the global task list
*
* @param    index    @b{(input)}  task ID to delete
*
* @returns  L7_RC_t L7_SUCCESS if deleted, else L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t bigIdListDelete(L7_int32 taskId)
{
  L7_uint32 index;

  if (bigIdListFind(taskId, &index) == L7_TRUE)
  {
    memset(&bigIdList[index], 0x00, sizeof(task_util_t));
    memcpy(&bigIdList[index],&bigIdList[bigListTotal-1],sizeof(task_util_t));
    memset(&bigIdList[bigListTotal-1], 0x00, sizeof(task_util_t));

    bigListTotal--;
    bigIdListSort();

    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}

/*********************************************************************
* @purpose  Print CPU utilization for a single task in the task list.
*
* @param    index        @b{(input)}   index into global task list
* @param    fiveSeconds  @b{(output)}  percentage utilization last 5 seconds
* @param    oneMinute    @b{(output)}  percentage utilization last 1 minute
* @param    fiveMinutes  @b{(output)}  percentage utilization last 5 minutes
*
* @returns  
*
* @comments The output values are multiplied by LVL7_CPU_UTIL_SCALAR
*           to allow access to the decimal values.
*
* @end
*
*********************************************************************/
static void calcTaskCpuUtilization(L7_int32 index, L7_int32 *fiveSeconds, L7_int32 *oneMinute, L7_int32 *fiveMinutes)
{
  L7_int32 lvl7percentutiltbl_idx_local;
  L7_int32 i;
  L7_int32 EMAprevious;

  lvl7percentutiltbl_idx_local = bigIdList[index].lvl7percentutiltbl_idx - 1;
  if (lvl7percentutiltbl_idx_local < 0)
  {
    lvl7percentutiltbl_idx_local = ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS) - 1;
  }
  *fiveSeconds = bigIdList[index].lvl7percentutiltbl[lvl7percentutiltbl_idx_local];

  /*
  * Go back to the first entry to be used. The minus 1 in the for loop is to account for the
  * decrement above.
  */
  for (i = 0; i < (60 / LVL7_CPU_UTIL_SECONDS) - 1; i++)
  {
    lvl7percentutiltbl_idx_local--;
    if (lvl7percentutiltbl_idx_local < 0)
    {
      lvl7percentutiltbl_idx_local = ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS) - 1;
    }
  }
  *oneMinute = 0;
  if (osapiUpTimeRaw() >= 60)
  {
    EMAprevious = bigIdList[index].lvl7percentutiltbl[lvl7percentutiltbl_idx_local];
    for (i = 0; i < (60 / LVL7_CPU_UTIL_SECONDS); i++)
    {
      /* Calculate Exponential Moving Average. */
      *oneMinute = (((bigIdList[index].lvl7percentutiltbl[lvl7percentutiltbl_idx_local] - EMAprevious) * 2) /
                   (1 + (60 / LVL7_CPU_UTIL_SECONDS))) + EMAprevious;
      EMAprevious = *oneMinute;
  
      lvl7percentutiltbl_idx_local++;
      if (lvl7percentutiltbl_idx_local >= ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS))
      {
        lvl7percentutiltbl_idx_local = 0;
      }
    }
  }

  *fiveMinutes = 0;
  if (osapiUpTimeRaw() >= 300)
  {
    lvl7percentutiltbl_idx_local = bigIdList[index].lvl7percentutiltbl_idx; /* Already pointing to the oldest entry */
    EMAprevious = bigIdList[index].lvl7percentutiltbl[lvl7percentutiltbl_idx_local];
    for (i = 0; i < ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS); i++)
    {
      /* Calculate Exponential Moving Average. */
      *fiveMinutes = (((bigIdList[index].lvl7percentutiltbl[lvl7percentutiltbl_idx_local] - EMAprevious) * 2) /
                    (1 +  ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS))) + EMAprevious;
      EMAprevious = *fiveMinutes;
  
      lvl7percentutiltbl_idx_local++;
      if (lvl7percentutiltbl_idx_local >= ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS))
      {
        lvl7percentutiltbl_idx_local = 0;
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Print CPU utilization for a single task in the task list.
*
* @param    index    @b{(input)}  index into global task list
* @param    taskBuf  @b{(input)}  pointer to print buffer
* @param    bufferSize @b{(input)}  size of print buffer
* @param    print5min  @b{(input)}  print 5 minute duration
*
* @returns  number of characters placed in print buffer
*
* @comments
*
* @end
*
*********************************************************************/
static L7_int32 printTaskCpuUtilization(L7_int32 index,      L7_char8 *taskBuf,
                                        L7_int32 bufferSize, L7_BOOL print5min,
                                        cpuUtlization_t  *cpuUtil)
{
  L7_int32 n=0;
  L7_int32 fiveSeconds;
  L7_int32 oneMinute;
  L7_int32 fiveMinutes;
  L7_char8 taskName[40];

  calcTaskCpuUtilization(index, &fiveSeconds, &oneMinute, &fiveMinutes);

  if (osapiTaskNameGet(bigIdList[index].taskId, taskName) != L7_SUCCESS)
    strncpy(taskName, "task no longer valid", sizeof(taskName));

  if(fiveSeconds || oneMinute || fiveMinutes)
  {
    n += osapiSnprintf(&taskBuf[n],bufferSize-n,
#ifdef _L7_OS_LINUX_
           "\n\r%5d    %-22.22s %3d.%.2d%%%5d.%.2d%%",
           osapiTaskPidGet(bigIdList[index].taskId),
#else
           "\n\r%8x %-22.22s %3d.%.2d%%%5d.%.2d%%",
           bigIdList[index].taskId,
#endif
           taskName,
           fiveSeconds / LVL7_CPU_UTIL_SCALAR,
           fiveSeconds % LVL7_CPU_UTIL_SCALAR,
           oneMinute   / LVL7_CPU_UTIL_SCALAR,
           oneMinute   % LVL7_CPU_UTIL_SCALAR);

    if (print5min == L7_TRUE)
      n += osapiSnprintf(&taskBuf[n],bufferSize-n,"%5d.%.2d%%", 
           fiveMinutes / LVL7_CPU_UTIL_SCALAR,
           fiveMinutes % LVL7_CPU_UTIL_SCALAR);
    cpuUtil->fiveSeconds += fiveSeconds;
    cpuUtil->oneMinute   += oneMinute;
    cpuUtil->fiveMinutes += fiveMinutes;
  }
  return n;
}


/*
* Exponential Moving Average Calculation:
* EMA = ((Current Value - Previous Value) * Multiplier) + Previous Value
*
* The first value is used as the first Previous Value.
* For a period-based EMA, "Multiplier" is equal to 2 / (1 + N) where N is
* the specified number of periods.
*/



int sysapiTaskUtilPrint (L7_char8 *outputBuf,
                         L7_uint32 bufferSize,
                         L7_BOOL   tasksFlag)
{

  L7_int32 i;
  L7_int32 n=0;
  cpuUtlization_t cpuUtil;
  L7_BOOL  print5min = L7_TRUE;

  /* Only print 5 minute interval is system has been up long enough */
  if (osapiUpTimeRaw() < 300)
    print5min = L7_FALSE;

  n += osapiSnprintf(&outputBuf[n],bufferSize-n,"\r\n\r\nCPU Utilization:\r\n");
  if (tasksFlag)
  {
    osapiSemaTake(lvl7taskSema, L7_WAIT_FOREVER);

    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
          "\r\n%-8s %-22s  %6s   %6s"," PID","Name","5 Sec","1 Min");
    if (print5min == L7_TRUE)
      n += osapiSnprintf(&outputBuf[n],bufferSize-n, "   %6s","5 Min");

    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
       "\r\n---------------------------------------------------------");
    /* print table of tasks */
    memset(&cpuUtil, 0, sizeof(cpuUtil));
    for (i=0;i<bigListTotal;i++)
    {
      n += printTaskCpuUtilization(i, &outputBuf[n],bufferSize-n,
                                   print5min, &cpuUtil);
    }
    osapiSemaGive(lvl7taskSema);
  }
  else
  {
    sysapiCpuUtilGet(&cpuUtil);
  }

  n += osapiSnprintf(&outputBuf[n],bufferSize-n,
     "\r\n---------------------------------------------------------\r\n");
  n += osapiSnprintf(&outputBuf[n],bufferSize-n,
         " Total CPU Utilization %12d.%.2d%%%5d.%.2d%%",
         cpuUtil.fiveSeconds / LVL7_CPU_UTIL_SCALAR,
         cpuUtil.fiveSeconds % LVL7_CPU_UTIL_SCALAR,
         cpuUtil.oneMinute   / LVL7_CPU_UTIL_SCALAR,
         cpuUtil.oneMinute   % LVL7_CPU_UTIL_SCALAR);
  if (print5min == L7_TRUE)
    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
         "%5d.%.2d%%\r\n",
         cpuUtil.fiveMinutes / LVL7_CPU_UTIL_SCALAR,
         cpuUtil.fiveMinutes % LVL7_CPU_UTIL_SCALAR);

  return n;
}

/*********************************************************************
* @purpose  Fill print buffer with string describing the
*           task utilization for the specified task index.
*
* @param    processIndex
* @param    taskNameString
* @param    taskNameSize
* @param    taskPercentString
* @param    taskPercentSize
*
* @returns  
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskSpecificUtilizationGet (L7_uint32 processIndex, 
                                          L7_uchar8 *taskNameString, L7_uint32 taskNameSize,
                                          L7_uchar8 *taskPercentString, L7_uint32 taskPercentSize)
{
  L7_int32 fiveSeconds;
  L7_int32 oneMinute;
  L7_int32 fiveMinutes;
  L7_BOOL  print5min = L7_TRUE;
  L7_uint32 i, n = 0;
  L7_uint32 activeTasks = 0;

  /* Only print 5 minute interval is system has been up long enough */
  if (osapiUpTimeRaw() < 300)
    print5min = L7_FALSE;

  /* Not the most efficient approach as far as CPU cycles, but uses less memory than if we cached everything. */
  for (i = 0;i < bigListTotal; i++)
  {
    calcTaskCpuUtilization(i, &fiveSeconds, &oneMinute, &fiveMinutes);
    if (fiveSeconds || oneMinute || fiveMinutes)
    {
      activeTasks++;
      if ((processIndex + 1) == activeTasks)
      {
        break;
      }
    }
  }
  if ((i == bigListTotal) || ((processIndex + 1) > activeTasks))
  {
    return L7_FAILURE;
  }

  if (osapiTaskNameGet(bigIdList[i].taskId, taskNameString) != L7_SUCCESS)
    strncpy(taskNameString, "task no longer valid", taskNameSize);

  n += osapiSnprintf(&taskPercentString[n],taskPercentSize-n,
                     "Five Seconds(%3d.%.2d%%) One Minute(%5d.%.2d%%)",
                     fiveSeconds / LVL7_CPU_UTIL_SCALAR,
                     fiveSeconds % LVL7_CPU_UTIL_SCALAR,
                     oneMinute   / LVL7_CPU_UTIL_SCALAR,
                     oneMinute   % LVL7_CPU_UTIL_SCALAR);

  if (print5min == L7_TRUE)
    n += osapiSnprintf(&taskPercentString[n],taskPercentSize-n,
                       "Five Minutes(%5d.%.2d%%)", 
                       fiveMinutes / LVL7_CPU_UTIL_SCALAR,
                       fiveMinutes % LVL7_CPU_UTIL_SCALAR);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next task ID.
*
* @param    processIndex
* @param    taskId
*
* @returns  
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskIdGet (L7_uint32 processIndex,  L7_char8 *taskIdString, L7_uint32 taskIdSize)
{
  L7_int32 fiveSeconds;
  L7_int32 oneMinute;
  L7_int32 fiveMinutes;
  L7_uint32 i;
  L7_uint32 activeTasks = 0;
  L7_uint32 taskId;
 
  /* Not the most efficient approach as far as CPU cycles, but uses less memory than if we cached everything. */
  for (i = 0;i < bigListTotal; i++)
  {
    calcTaskCpuUtilization(i, &fiveSeconds, &oneMinute, &fiveMinutes);
    if (fiveSeconds || oneMinute || fiveMinutes)
    {
      activeTasks++;
      if ((processIndex + 1) == activeTasks)
      {
        break;
      }
    }
  }
  if ((i == bigListTotal) || ((processIndex + 1) > activeTasks))
  {
    return L7_FAILURE;
  }
#ifdef _L7_OS_LINUX_
  taskId=osapiTaskPidGet(bigIdList[i].taskId);
  osapiSnprintf(taskIdString, taskIdSize, "%d", taskId);
#else
  taskId=bigIdList[i].taskId;
  osapiSnprintf(taskIdString, taskIdSize, "%x", taskId);
#endif
 
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose returns the CPU utilization
*
* @param cpuUtil : cpu utlization values
*
* @returns return code
*
* @notes Return utilization values are scaled with LVL7_CPU_UTIL_SCALAR
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCpuUtilGet (cpuUtlization_t *cpuUtil)
{

  L7_int32 lvl7percentutiltbl_idx_local;
  L7_int32 i;
  L7_int32 EMAprevious;
  L7_int32 fiveSeconds;
  L7_int32 oneMinute;
  L7_int32 fiveMinutes;

  osapiSemaTake(lvl7taskSema, L7_WAIT_FOREVER);

  lvl7percentutiltbl_idx_local = lvl7percentutiltbl_idx - 1;
  if (lvl7percentutiltbl_idx_local < 0)
  {
    lvl7percentutiltbl_idx_local = ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS) - 1;
  }
  fiveSeconds = lvl7percentutiltbl[lvl7percentutiltbl_idx_local];

  /*
  * Go back to the first entry to be used. The minus 1 in the for loop is to account for the
  * decrement above.
  */
  for (i = 0; i < (60 / LVL7_CPU_UTIL_SECONDS) - 1; i++)
  {
    lvl7percentutiltbl_idx_local--;
    if (lvl7percentutiltbl_idx_local < 0)
    {
      lvl7percentutiltbl_idx_local = ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS) - 1;
    }
  }
  oneMinute = 0;
  EMAprevious = lvl7percentutiltbl[lvl7percentutiltbl_idx_local];
  for (i = 0; i < (60 / LVL7_CPU_UTIL_SECONDS); i++)
  {
    /* Calculate Exponential Moving Average. */
    oneMinute = (((lvl7percentutiltbl[lvl7percentutiltbl_idx_local] - EMAprevious) * 2) /
                 (1 + (60 / LVL7_CPU_UTIL_SECONDS))) + EMAprevious;
    EMAprevious = oneMinute;

    lvl7percentutiltbl_idx_local++;
    if (lvl7percentutiltbl_idx_local >= ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS))
    {
      lvl7percentutiltbl_idx_local = 0;
    }
  }

  fiveMinutes = 0;
  lvl7percentutiltbl_idx_local = lvl7percentutiltbl_idx; /* Already pointing to the oldest entry */
  EMAprevious = lvl7percentutiltbl[lvl7percentutiltbl_idx_local];
  for (i = 0; i < ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS); i++)
  {
    /* Calculate Exponential Moving Average. */
    fiveMinutes = (((lvl7percentutiltbl[lvl7percentutiltbl_idx_local] - EMAprevious) * 2) /
                  (1 +  ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS))) + EMAprevious;
    EMAprevious = fiveMinutes;

    lvl7percentutiltbl_idx_local++;
    if (lvl7percentutiltbl_idx_local >= ((LVL7_CPU_UTIL_MINUTES * 60) / LVL7_CPU_UTIL_SECONDS))
    {
      lvl7percentutiltbl_idx_local = 0;
    }
  }

  cpuUtil->fiveSeconds = fiveSeconds;
  cpuUtil->oneMinute = oneMinute;
  cpuUtil->fiveMinutes = fiveMinutes;

  osapiSemaGive(lvl7taskSema);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Start task that measures system-wide task utilization.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t sysapiCpuUtilTaskStart ()
{
  L7_RC_t   rc = L7_SUCCESS;
#ifndef L7_PRODUCT_SMARTPATH
  L7_int32  cpuUtilMonitorTaskPtr;
  L7_uint32 argv[2];

  cpuUtilMonitorTaskPtr = osapiTaskCreate( "cpuUtilMonitorTask",
                                            osapiCpuUtilMonitorTask,
                                            2,
                                            &argv,
                                            L7_DEFAULT_STACK_SIZE,
                                            1,
                                            1);

  if ( cpuUtilMonitorTaskPtr == L7_ERROR )
  {
    printf("Could not create task cpuUtilMonitorTaskPtr\n");
    rc = L7_FAILURE;
  }
#endif
  return rc;
}


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
#include "trapapi.h"


/* Task level utilization table. The table is kept sorted based on the taskIds. */
sysapiTaskCpuUtil_t sysapiTaskCpuUtilTable[L7_CPU_UTIL_MAX_TASKS];

/* Number of entries in the task util table */
L7_int32 sysapiTaskCpuUtilTableNumEntries = 0;

/* Total Cpu utilization for various periods. The table is kept sorted based
 * on timePeriod value
 */
sysapiTotalCpuUtil_t sysapiTotalCpuUtilTable[L7_CPU_UTIL_MEASURE_MAX_PERIODS];

/* Number of entries in the total util table */
L7_int32 sysapiTotalCpuUtilTableNumEntries = 0;

/* Default Utilization measurement time-periods (secs)  */
L7_uint32 sysapiCpuUtilDefaultMonitorPeriods[] = {5, 60, 300}; 

/* Semaphore to protect sysapiTaskCpuUtilTable and sysapiTotalCpuUtilTable access */
void *sysapiCpuUtilTblSema = L7_NULLPTR;

/* Indicates whether the cpu util measurement is suspended or not. 
 * Use sysapiCpuUtilMonitorSuspend to change this flag
 */
L7_BOOL sysapiCpuUtilMonitorSuspended = L7_FALSE;

/*********************************************************************
* @purpose   Take the cpu utilization table lock
*
* @param     none
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void sysapiCpuUtilLockTake(void)
{
  if (osapiSemaTake(sysapiCpuUtilTblSema, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSAPI_COMPONENT_ID,
            "Failed to take cpuUtilSema %x\n", sysapiCpuUtilTblSema);
  }
}


/*********************************************************************
* @purpose   Give the cpu utilization table lock
*
* @param     none
*
* @returns  none
*
* @comments none
*
* @end
*
*********************************************************************/
void sysapiCpuUtilLockGive(void)
{
  if (osapiSemaGive(sysapiCpuUtilTblSema) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSAPI_COMPONENT_ID,
            "Failed to give cpuUtilSema %x\n", sysapiCpuUtilTblSema);
  }
}


/*********************************************************************
* @purpose  Arrange the utilization time periods in sorted order for 
*           task index
*
* @returns  none
*
* @comments Bubble sort.
*
* @end
*
*********************************************************************/
void sysapiTaskCpuUtilPeriodTableSort(L7_uint32 taskIdx)
{
  L7_uint32             i, j, numPeriods;
  cpuPeriodUtilInfo_t   temp;

  numPeriods = sysapiTaskCpuUtilTable[taskIdx].numPeriods;

  if (numPeriods == 0)
  {
    return;      
  }

  /* bubble sort the list */
  j = 0;
  for (i = 0; i < (numPeriods - 1); i++)
  {
    for (j = (numPeriods - 1); j > i; j--)
    {
      memcpy(&temp, &(sysapiTaskCpuUtilTable[taskIdx].periodInfo[j]),sizeof(temp));
      if (temp.timePeriod < sysapiTaskCpuUtilTable[taskIdx].periodInfo[j-1].timePeriod)
      {
        memcpy(&(sysapiTaskCpuUtilTable[taskIdx].periodInfo[j]),
               &(sysapiTaskCpuUtilTable[taskIdx].periodInfo[j-1]),
               sizeof(sysapiTaskCpuUtilTable[taskIdx].periodInfo[0]));
        memcpy(&(sysapiTaskCpuUtilTable[taskIdx].periodInfo[j-1]),
               &temp, sizeof(temp));
      }
    }
  }
}

/*********************************************************************
* @purpose   Adds a new utilization monitoring period for given task
*
* @param     taskIdx     {(input)}  Index for the task
* @param     timePeriod  {(input)}  Utilization monitoring period
* @param     sortTable   {(input)}  L7_TRUE: Sort the period table
*                                   L7_FALSE: Don't sort the period table
*
* @returns  L7_RC_t
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskCpuUtilTablePeriodInsert(L7_uint32 taskIdx, 
                                           L7_uint32 timePeriod,
                                           L7_BOOL   sortTable)
{
  L7_BOOL   found = L7_FALSE;
  L7_uint32 periodIdx, numPeriods;
  L7_RC_t   rc = L7_SUCCESS;

  numPeriods = sysapiTaskCpuUtilTable[taskIdx].numPeriods;

  for (periodIdx = 0; periodIdx < numPeriods;  periodIdx++)
  {
    if (sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod == timePeriod)
    {
      found = L7_TRUE;
      break;
    }
  }
  
  if (found == L7_FALSE) 
  {
    /* No more free indexes. */
    if (numPeriods >= L7_CPU_UTIL_MEASURE_MAX_PERIODS)
    {
      rc = L7_FAILURE;
    }
    else
    {
      memset(&(sysapiTaskCpuUtilTable[taskIdx].periodInfo[numPeriods]), 0, 
             sizeof(sysapiTaskCpuUtilTable[taskIdx].periodInfo[0]));
      sysapiTaskCpuUtilTable[taskIdx].periodInfo[numPeriods].timePeriod = timePeriod;
      sysapiTaskCpuUtilTable[taskIdx].periodInfo[numPeriods].measureStartTime = 
                                                               osapiUpTimeRaw();
      sysapiTaskCpuUtilTable[taskIdx].numPeriods++;

      if (sortTable == L7_TRUE)
      {
        sysapiTaskCpuUtilPeriodTableSort(taskIdx);
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose   Removess a given utilization monitoring period for the task
*
* @param     taskIdx     {(input)}  Index for the task
* @param     timePeriod  {(input)}  Utilization monitoring period
*
* @returns  L7_RC_t
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskCpuUtilTablePeriodRemove(L7_uint32 taskIdx, L7_uint32 timePeriod)
{
  L7_uint32 periodIdx, numPeriods;

  numPeriods = sysapiTaskCpuUtilTable[taskIdx].numPeriods;

  for (periodIdx = 0; periodIdx < numPeriods;  periodIdx++)
  {
    if (sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod == timePeriod)
    {
      memset(&(sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx]), 0, 
                               sizeof(sysapiTaskCpuUtilTable[taskIdx].periodInfo[0]));

      /* Put the last element at this index and then sort the table */
      memcpy(&(sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx]), 
             &(sysapiTaskCpuUtilTable[taskIdx].periodInfo[numPeriods - 1]),
             sizeof(sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx]));

      memset(&(sysapiTaskCpuUtilTable[taskIdx].periodInfo[numPeriods - 1]), 0x00, 
              sizeof(sysapiTaskCpuUtilTable[taskIdx].periodInfo[0]));
    
      sysapiTaskCpuUtilTable[taskIdx].numPeriods--;
    
      sysapiTaskCpuUtilPeriodTableSort(taskIdx);
    
      break;
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Insert a task ID in the global task list
*
* @param    index    @b{(input)}  task ID to insert
*
* @returns  L7_uint32 index of sysapiTaskCpuUtilTable insertion point
*
* @comments none
*
* @end
*
*********************************************************************/
L7_uint32 sysapiTaskCpuUtilTableInsert(L7_int32 taskId)
{
  L7_uint32 taskIdx = sysapiTaskCpuUtilTableNumEntries;
  L7_uint32 periodIdx;

  sysapiTaskCpuUtilTable[taskIdx].taskId = taskId;

  memset(sysapiTaskCpuUtilTable[taskIdx].periodInfo, 0, 
                                 sizeof(sysapiTaskCpuUtilTable[taskIdx].periodInfo));

  /* Add all the known monitoring time-periods */
  for (periodIdx = 0; periodIdx < sysapiTotalCpuUtilTableNumEntries; periodIdx++)
  {
    sysapiTaskCpuUtilTablePeriodInsert(taskIdx, 
                                       sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod,
                                       L7_FALSE);
  }

  sysapiTaskCpuUtilPeriodTableSort(taskIdx);

  sysapiTaskCpuUtilTableNumEntries++;
 
  return (taskIdx);
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
void sysapiTaskCpuUtilTableSort()
{
  L7_uint32     i, j;
  sysapiTaskCpuUtil_t temp;

  if (sysapiTaskCpuUtilTableNumEntries == 0)
  {
    return;      
  }

  /* bubble sort the list */
  j = 0;
  for (i = 0; i < sysapiTaskCpuUtilTableNumEntries-1; i++)
  {
    for (j = sysapiTaskCpuUtilTableNumEntries-1; j > i; j--)
    {
      memcpy(&temp, &sysapiTaskCpuUtilTable[j], sizeof(temp));
      if (temp.taskId < sysapiTaskCpuUtilTable[j-1].taskId)
      {
        memcpy(&sysapiTaskCpuUtilTable[j],&sysapiTaskCpuUtilTable[j-1],sizeof(sysapiTaskCpuUtilTable[0]));
        memcpy(&sysapiTaskCpuUtilTable[j-1],&temp,sizeof(temp));
      }
    }
  }

  return;
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
L7_BOOL sysapiTaskCpuUtilTableFind(L7_int32 taskId, L7_uint32 *index)
{
  L7_int32 i;
  L7_int32 l = -1;
  L7_int32 r = sysapiTaskCpuUtilTableNumEntries;     /* l and r are beyond the bounds of list */

  /* binary search the list */
  while (l+1 != r)               /* Stop when l and r meet */
  {
    i = (l+r)/2;                 /* Look at middle of remaining subarray */
    if (taskId < sysapiTaskCpuUtilTable[i].taskId)
      r = i;                     /* In left half */
    if (taskId == sysapiTaskCpuUtilTable[i].taskId)
    {                            /* Found it */
      *index = i;
      return L7_TRUE;
    }
    if (taskId > sysapiTaskCpuUtilTable[i].taskId)
      l = i;                     /* In right half */
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Delete a task ID in the global task list
*
* @param    index    @b{(input)}  Table Index of the task to be deleted
*
* @returns  L7_RC_t L7_SUCCESS if deleted, else L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTaskCpuUtilTableDelete(L7_uint32 index)
{
  memset(&(sysapiTaskCpuUtilTable[index]), 0x00, sizeof(sysapiTaskCpuUtilTable[0]));

  /* Copy the last element at this index and then sort */
  memcpy(&sysapiTaskCpuUtilTable[index], 
         &sysapiTaskCpuUtilTable[sysapiTaskCpuUtilTableNumEntries-1],
         sizeof(sysapiTaskCpuUtilTable[0]));
  memset(&sysapiTaskCpuUtilTable[sysapiTaskCpuUtilTableNumEntries-1], 0x00, 
         sizeof(sysapiTaskCpuUtilTable[0]));

  sysapiTaskCpuUtilTableNumEntries--;
  sysapiTaskCpuUtilTableSort();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the task utilization table
*
* @param    index      {(input)}  Index of the task in the taskUtilTable
* @param    scaledUtil {(input)}  Scaled task utilization 
*
* @returns  none
*
* @comments The first Avg is a simple mean of first n obervations, 
*           where n = (TotalTimePeriod/SamplingPeriod).
*           
*           After the first Avg is calculated, the new Avg is calculated 
*           every sampling period based on following exponential avg equation:
*
*           EMA = lastEMA + (currentUtilization - lastEMA) * weight
*             where weight = 2 / ((TotalTimePeriod/samplePeriod) + 1).
*
*
* @end
*
*********************************************************************/
void sysapiTaskCpuUtilUpdate(L7_uint32 taskIdx, L7_int32 scaledUtil)
{
  L7_uint32 periodIdx, timeDiff, currentTime, timePeriod;
  L7_int32  lastExpMovingAvg, avgDiff, weightNum;

  if (taskIdx >= L7_CPU_UTIL_MAX_TASKS)
  {
    return;
  }

  for (periodIdx = 0; periodIdx < sysapiTaskCpuUtilTable[taskIdx].numPeriods; periodIdx++)
  {
    /* If this timeperiod is not active, then skip it */
    if (sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod == 0)
    {
      continue;        
    }

    /* Calculate Exp moving avg */
    timePeriod =  sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod;
    lastExpMovingAvg = sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg;


    weightNum = (sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod) / 
                                                     (L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS);    

    avgDiff = (((scaledUtil - lastExpMovingAvg) * ((L7_int32)(2))) / (weightNum + 1));
    if (avgDiff != 0)
    {
      sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg = 
                                                      lastExpMovingAvg + avgDiff;
        
    }
    else
    {
      /* Difference between the current reading and last average is very minute.
       * So set the average as current reading as the new avg
       */
      sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg = scaledUtil;
    }

    if (sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg  < 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSAPI_COMPONENT_ID,
              "Total CPU Utilization %d for period %d task %x has become negative."
              " Resettting it to 0\n",
              sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg,
              sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod,
              sysapiTaskCpuUtilTable[taskIdx].taskId);
      sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg = 0;
    }

    /* If the first avg has not been calculated */
    if (sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].firstAvgDone == L7_FALSE)
    {
      currentTime = osapiUpTimeRaw();
      timeDiff = currentTime - sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].measureStartTime;

      if (timeDiff >= sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod)
      {
        sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].firstAvgDone = L7_TRUE;
      }
    }

  }

  return;
}


/*********************************************************************
* @purpose  Age out entries for tasks that have been deleted
*           
* @params   none
* @returns  none
*
* @comments  We do aging in two pass. In the first iteration, entries are 
*            marked for aging and are actually deleted in the second pass. 
*            This allows to display utilization data for tasks that are gone 
*            for some additional time.
* @end
*
*********************************************************************/
void sysapiCpuUtilTaskAge(void)
{
  L7_uint32 i;

  for (i = 0; i < sysapiTaskCpuUtilTableNumEntries; i++)
  { 
    if ((osapiTaskIdVerify(sysapiTaskCpuUtilTable[i].taskId ) != L7_SUCCESS) &&
        (sysapiTaskCpuUtilTable[i].pendingAge == L7_TRUE))
    {
      sysapiTaskCpuUtilTableDelete(i);
    }
    else if (osapiTaskIdVerify(sysapiTaskCpuUtilTable[i].taskId) != L7_SUCCESS)
    {  
      sysapiTaskCpuUtilTable[i].pendingAge = L7_TRUE; 
    }
    else
    {
      sysapiTaskCpuUtilTable[i].pendingAge = L7_FALSE; 
    }
  }

  return;
} 

/*********************************************************************
* @purpose  Return the top <n> task cpu utilization for a given time-period
*           
* @param    periodIdx {(input)} Time-period index for the Cpu utilization
* @param    num_entires {(input)} Number of top entries to return          
* @param    task_util_array {(output)} Memory to store the task info
*
* @returns  none
*
* @comments 
*
* @end
*
*********************************************************************/
void sysapiGetTopTaskCpuUtil(L7_uint32 periodIdx,
                             L7_uint32 num_entries,
                             cpuTaskUtilSort_t *task_util_array)
{
  L7_uint32 i, j, idx;
  cpuTaskUtilSort_t taskInfo[L7_CPU_UTIL_MAX_TASKS], temp;

  memset(taskInfo, 0, sizeof(taskInfo));

  for (idx = 0; idx < sysapiTaskCpuUtilTableNumEntries; idx++)
  {
    taskInfo[idx].taskId = sysapiTaskCpuUtilTable[idx].taskId;
    taskInfo[idx].util = (sysapiTaskCpuUtilTable[idx].periodInfo[periodIdx].expMovingAvg) /
                                                                  (L7_CPU_UTIL_SCALAR);
  }

  /* Bubble sort the taskInfo array based on utilization */
  j = 0;
  for (i = 0; i < sysapiTaskCpuUtilTableNumEntries-1; i++)
  {
    for (j = sysapiTaskCpuUtilTableNumEntries-1; j > i; j--)
    {
      memcpy(&temp, &taskInfo[j], sizeof(temp));
      if (temp.util < taskInfo[j-1].util)
      {
        memcpy(&taskInfo[j],&taskInfo[j-1],sizeof(taskInfo[0]));
        memcpy(&taskInfo[j-1], &temp, sizeof(temp));
      }
    }
  }

  /* Copy the requested entries to task_util_array */
  for (j = sysapiTaskCpuUtilTableNumEntries, i = 0; 
      ((j > 0) && (i < num_entries)); 
       j--, i++)
  {
    memcpy(&task_util_array[i], &taskInfo[j-1], sizeof(*task_util_array));
  }
 
  return;
}

/*********************************************************************
* @purpose  Check if the total cpu utilization has crossed the threshold
*           
*           
*
* @returns  none
*
* @comments Bubble sort.
*
* @end
*
*********************************************************************/
void sysapiTotalCpuUtilThresholdMonitor(L7_uint32 periodIdx)
{
  L7_uchar8 buffer[256], taskName[128];
  L7_uint32 totalCpuUtil, j;
  cpuTaskUtilSort_t topTaskUtil[L7_CPU_RISING_TRAP_MAX_TASKS_REPORT];

  memset(topTaskUtil, 0, sizeof(topTaskUtil));
  memset(buffer, 0, sizeof(buffer));

  if (sysapiTotalCpuUtilTable[periodIdx].risingThreshold == 0)
  {
    return;
  }

  if (sysapiTotalCpuUtilTable[periodIdx].periodInfo.firstAvgDone == L7_FALSE)
  {
    return;    
  }

  /* Rising trap has been sent. Check for falling threshold trap. */  
  if (sysapiTotalCpuUtilTable[periodIdx].risingTrapSent == L7_TRUE)
  {
    if ((sysapiTotalCpuUtilTable[periodIdx].fallingThreshold == 0) ||
        (sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.firstAvgDone == L7_FALSE))
    {
      return;    
    }

    totalCpuUtil = (sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg) / 
                                                             (L7_CPU_UTIL_SCALAR);
    if (totalCpuUtil < sysapiTotalCpuUtilTable[periodIdx].fallingThreshold)
    {
      /* Generate falling trap */
      trapMgrSwitchCpuFallingThresholdTrap(sysapiTotalCpuUtilTable[periodIdx].fallingThreshold); 
      sysapiTotalCpuUtilTable[periodIdx].risingTrapSent = L7_FALSE;

      
      sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg = 0;
      sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.firstAvgDone = L7_FALSE;
      sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.measureStartTime = 0;
      
    }
        
  }
  else if (sysapiTotalCpuUtilTable[periodIdx].risingTrapSent == L7_FALSE)
  {
    totalCpuUtil = (sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg) / 
                                                             (L7_CPU_UTIL_SCALAR);
    if (totalCpuUtil >= sysapiTotalCpuUtilTable[periodIdx].risingThreshold)
    {

      /* Get the top <n> task utilization to report */
      sysapiGetTopTaskCpuUtil(periodIdx, L7_CPU_RISING_TRAP_MAX_TASKS_REPORT,
                              topTaskUtil);
                              
      osapiSnprintf(buffer, sizeof(buffer),
                    "Top %d Tasks (Task-Name/Percent-Util):",
                    L7_CPU_RISING_TRAP_MAX_TASKS_REPORT);


      for (j = 0; j < L7_CPU_RISING_TRAP_MAX_TASKS_REPORT; j++)
      {
        if (osapiTaskNameGet(topTaskUtil[j].taskId, taskName) == L7_FAILURE)
        {
          osapiStrncpy(taskName, "Task Deleted.", sizeof(taskName));
        }

        osapiSnprintfcat(buffer, sizeof(buffer), " (%s/%d)",
                         taskName,
                         topTaskUtil[j].util);     
      }
                   
      osapiSnprintfcat(buffer, sizeof(buffer),
                       ".\n");

      trapMgrSwitchCpuRisingThresholdTrap(sysapiTotalCpuUtilTable[periodIdx].risingThreshold,
                                          buffer);
              
      sysapiTotalCpuUtilTable[periodIdx].risingTrapSent = L7_TRUE;

      /* Start monitoring of the falling threshold */
      sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg = 0;
      sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.firstAvgDone = L7_FALSE;
      sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.measureStartTime = osapiUpTimeRaw();
    }
  }

  return;
}

/*********************************************************************
* @purpose  Arrange the utilization time periods in sorted order 
*           
*
* @returns  none
*
* @comments Bubble sort.
*
* @end
*
*********************************************************************/
void sysapiTotalCpuUtilPeriodTableSort(void)
{
  L7_uint32              i, j;
  sysapiTotalCpuUtil_t   temp;

  if (sysapiTotalCpuUtilTableNumEntries == 0)
  {
    return;      
  }

  /* bubble sort the list */
  j = 0;
  for (i = 0; i < sysapiTotalCpuUtilTableNumEntries-1; i++)
  {
    for (j = sysapiTotalCpuUtilTableNumEntries-1; j > i; j--)
    {
      memcpy(&temp, &(sysapiTotalCpuUtilTable[j]), sizeof(temp));
      if (temp.periodInfo.timePeriod < sysapiTotalCpuUtilTable[j-1].periodInfo.timePeriod)
      {
        memcpy(&(sysapiTotalCpuUtilTable[j]),
               &(sysapiTotalCpuUtilTable[j-1]),
               sizeof(sysapiTotalCpuUtilTable[j]));
        memcpy(&(sysapiTotalCpuUtilTable[j-1]),
               &temp, sizeof(temp));
      }
    }
  }

  return;
}

/*********************************************************************
* @purpose  Make an entry in the sysapiTotalCpuUtilTable
*
* @param    risingPeriod {(input)}  Time period in secs over which util is 
*                                     measured.
* @param    risingThreshold {(input)} Util threshold in percent. If this is crossed
*                                     over the risingPeriod, then traps are generated.
* @param    fallingPeriod {(input)} Time period in secs over which the falling 
*                                   threshold is to be measured.
* @param    fallingThreshold {(input)} Util threshold in percent. If Util goes
*                                      below over this measured over the fallingPeriod, 
*                                      then traps are generated.
*
* @returns  L7_RC_t L7_SUCCESS : Insertion success
*                   L7_FAILURE : Insertion failed
*
* @comments A value of 0 for rising/falling threshold indicates traps are not
*           to be send.
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTotalCpuUtilTablePeriodInsert(L7_uint32 risingPeriod, L7_uint32 risingThreshold,
                                            L7_uint32 fallingPeriod, L7_uint32 fallingThreshold)
{
  L7_uint32 idx;
  L7_BOOL   found = L7_FALSE;
  L7_RC_t   rc = L7_FAILURE;

  sysapiCpuUtilLockTake();

  do
  {

    /* Check if the period is already configured. */
    for (idx = 0; idx < sysapiTotalCpuUtilTableNumEntries; idx++)
    {
      if (sysapiTotalCpuUtilTable[idx].periodInfo.timePeriod == risingPeriod)
      { 
        found = L7_TRUE; 
        break;    
      }
    }

    if (found == L7_FALSE)
    {
      idx = sysapiTotalCpuUtilTableNumEntries;

      /* No free entries */
      if (idx >= L7_CPU_UTIL_MEASURE_MAX_PERIODS)
      {
        rc = L7_FAILURE;    
        break;
      }
    }

    memset(&(sysapiTotalCpuUtilTable[idx]), 0,
                        sizeof(sysapiTotalCpuUtilTable[idx]));

    sysapiTotalCpuUtilTable[idx].periodInfo.timePeriod = risingPeriod;
    sysapiTotalCpuUtilTable[idx].risingThreshold = risingThreshold;
    sysapiTotalCpuUtilTable[idx].fallingPeriodInfo.timePeriod = fallingPeriod;
    sysapiTotalCpuUtilTable[idx].fallingThreshold = fallingThreshold;
    sysapiTotalCpuUtilTable[idx].periodInfo.measureStartTime = osapiUpTimeRaw();

    if (found == L7_FALSE)
    {
      sysapiTotalCpuUtilTableNumEntries++;

      sysapiTotalCpuUtilPeriodTableSort();

      /* Add this monitoring period for all the active tasks */
      for (idx = 0; idx < sysapiTaskCpuUtilTableNumEntries; idx++)
      {
        sysapiTaskCpuUtilTablePeriodInsert(idx, risingPeriod, L7_TRUE);
      }
    }
    else
    {
      /* Remove/Add this monitoring period for all the active tasks so that 
       * values are reset.
       */
      for (idx = 0; idx < sysapiTaskCpuUtilTableNumEntries; idx++)
      {
        sysapiTaskCpuUtilTablePeriodRemove(idx, risingPeriod);
        sysapiTaskCpuUtilTablePeriodInsert(idx, risingPeriod, L7_TRUE);
      }
    }

    rc = L7_SUCCESS;
  } while (0);

  sysapiCpuUtilLockGive();

  return rc;
}


/*********************************************************************
* @purpose  Delete an entry from the sysapiTotalCpuUtilTable
*
* @param    risingPeriod {(input)}  Time period in secs over which util is 
*                                     measured.
*
* @returns  L7_RC_t L7_SUCCESS : Deletion success
*                   L7_FAILURE : Deletion failed
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t sysapiTotalCpuUtilTablePeriodRemove(L7_uint32 risingPeriod)
{
  L7_uint32 idx, found = L7_FALSE, defaultPeriod = L7_FALSE, defaultIdx;
  L7_RC_t   rc = L7_FAILURE;

  sysapiCpuUtilLockTake();

  do
  {

    for (idx = 0 ; idx < sysapiTotalCpuUtilTableNumEntries; idx++)
    {
      if (sysapiTotalCpuUtilTable[idx].periodInfo.timePeriod == risingPeriod) 
      {
        found = L7_TRUE;
        break;
      }
    }

    /* Entry not found */
    if (found == L7_FALSE) 
    {
      rc = L7_FAILURE;
    }
    else
    { 
      /* Check if this is a default monitoring period being deleted */
      for (defaultIdx = 0; 
           defaultIdx < (sizeof(sysapiCpuUtilDefaultMonitorPeriods)/sizeof(L7_uint32)); 
           defaultIdx++)
      {
        if (sysapiCpuUtilDefaultMonitorPeriods[defaultIdx] == risingPeriod)
        {
          defaultPeriod = L7_TRUE; 
          break;    
        }
      }

      if (defaultPeriod == L7_FALSE)
      {
        /* Entry found. Copy the last entry in this position and
         * sort.
         */
        memset(&(sysapiTotalCpuUtilTable[idx]), 0, sizeof(sysapiTotalCpuUtilTable[idx]));

        memcpy(&(sysapiTotalCpuUtilTable[idx]), 
               &(sysapiTotalCpuUtilTable[sysapiTotalCpuUtilTableNumEntries - 1]),
               sizeof(sysapiTotalCpuUtilTable[0]));
        memset(&(sysapiTotalCpuUtilTable[sysapiTotalCpuUtilTableNumEntries - 1]), 0x00,
               sizeof(sysapiTotalCpuUtilTable[0]));
        sysapiTotalCpuUtilTableNumEntries--;
        sysapiTotalCpuUtilPeriodTableSort();

        /* Remove this period from all the active tasks */
        for (idx = 0; idx < sysapiTaskCpuUtilTableNumEntries; idx++)
        {
          sysapiTaskCpuUtilTablePeriodRemove(idx, risingPeriod);
        }
      }
      else
      {
        sysapiTotalCpuUtilTable[idx].fallingThreshold = 0;
        sysapiTotalCpuUtilTable[idx].risingThreshold = 0;
        sysapiTotalCpuUtilTable[idx].risingTrapSent = L7_FALSE;
        memset(&(sysapiTotalCpuUtilTable[idx].fallingPeriodInfo), 0,
               sizeof(sysapiTotalCpuUtilTable[idx].fallingPeriodInfo));
      }

      rc = L7_SUCCESS;
    }

  } while (0);

  sysapiCpuUtilLockGive();

  return rc;
}

/*********************************************************************
* @purpose  Update the total utilization table
*
* @param    scaledUtil {(input)}  Scaled total utilization for the period
*
* @returns  none
*
* @comments The first Avg is a simple mean of first n obervations, 
*           where n = (TotalTimePeriod/SamplingPeriod).
*           
*           After the first Avg is calculated, the new Avg is calculated 
*           every sampling period based on following exponential avg equation:
*
*           EMA = lastEMA + (currentUtilization - lastEMA) * weight
*             where weight = 2 / ((TotalTimePeriod/samplePeriod) + 1).
*
*
* @end
*
*********************************************************************/
void sysapiTotalCpuUtilUpdate(L7_int32 scaledUtil)
{
  L7_uint32 periodIdx, timeDiff, currentTime, timePeriod;
  L7_int32  lastExpMovingAvg, avgDiff, weightNum;

  for (periodIdx = 0; periodIdx < sysapiTotalCpuUtilTableNumEntries; periodIdx++)
  {
    /* If this timeperiod is not active, then skip it */
    if (sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod == 0)
    {
      continue;        
    }

    /* Update the CPU util for Rising period */
    timePeriod =  sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod;
    lastExpMovingAvg = sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg;

    weightNum = (sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod) / 
                                            (L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS);    

    avgDiff = (((scaledUtil - lastExpMovingAvg) * ((L7_int32)(2))) / (weightNum + 1));
    if (avgDiff != 0)
    {
      sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg = 
                                                      lastExpMovingAvg + avgDiff;
        
    }
    else
    {
      /* Difference between the current reading and last average is very minute.
       * So set the average as current reading as the new avg
       */
      sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg = scaledUtil;
    }

    if (sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg < 0)
    {
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_OSAPI_COMPONENT_ID,
              "Total CPU Utilization %d for period %d has become negative."
              " Resettting it to 0\n",
              sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg,
              sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod);
      sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg = 0;
    }

    /* Update the firstAvgDone flag */
    if (sysapiTotalCpuUtilTable[periodIdx].periodInfo.firstAvgDone == L7_FALSE)
    {
      currentTime = osapiUpTimeRaw();
      timeDiff = currentTime - sysapiTotalCpuUtilTable[periodIdx].periodInfo.measureStartTime;
      
      if (timeDiff >= sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod)
      {
        sysapiTotalCpuUtilTable[periodIdx].periodInfo.firstAvgDone = L7_TRUE;
      }
    }


    /* If Rising threshold trap was sent, then calculate the falling cpu util */
    if (sysapiTotalCpuUtilTable[periodIdx].risingTrapSent == L7_TRUE)
    {
      timePeriod =  sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.timePeriod;
      lastExpMovingAvg = sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg;

      weightNum = (sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.timePeriod) / 
                                            (L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS);    

      avgDiff = (((scaledUtil - lastExpMovingAvg) * ((L7_int32)(2))) / (weightNum + 1));
      if (avgDiff != 0)
      {
        sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg = 
                                                      lastExpMovingAvg + avgDiff;
        
      }
      else
      {
        /* Difference between the current reading and last average is very minute.
         * So set the average as current reading 
         */
        sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg = scaledUtil;
      }

      if (sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.firstAvgDone == L7_FALSE)
      {
        currentTime = osapiUpTimeRaw();
        timeDiff = currentTime - sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.measureStartTime;
      
        if (timeDiff >= sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.timePeriod)
        {
          sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.firstAvgDone = L7_TRUE;
        }
      }
    }


    sysapiTotalCpuUtilThresholdMonitor(periodIdx);

  }

  return;
}

/*********************************************************************
* @purpose  Initialize and setup the totalCpuUtil table with default 
*           values.
*
* @param    none
*
* @returns  none
*
* @comments
*
* @end
*
*********************************************************************/
void sysapiTotalCpuUtilTableInit(void)
{
  L7_uint32 i;

  memset(sysapiTotalCpuUtilTable, 0, sizeof(sysapiTotalCpuUtilTable));

  for (i = 0; i < (sizeof(sysapiCpuUtilDefaultMonitorPeriods)/sizeof(L7_uint32)); i++)
  {
    if (sysapiTotalCpuUtilTablePeriodInsert(sysapiCpuUtilDefaultMonitorPeriods[i], 0, 0, 0) != L7_SUCCESS)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "CPU Util Monitor: Failed to insert an entry in the table\n");
    }
  }

  return;
}

/*********************************************************************
* @purpose  Print CPU utilization for a single task in the task list.
*
* @param    index         {(input)}  index into global task list
* @param    taskBuf       {(input)}  pointer to print buffer
* @param    bufferSize    {(input)}  size of print buffer
* @param    numEntries    {(input)}  number of period entries to report
*
* @returns  number of characters placed in print buffer
*
* @comments
*
* @end
*
*********************************************************************/
L7_int32 printTaskCpuUtilization(L7_int32   index,  
                                 L7_char8  *taskBuf,
                                 L7_int32   bufferSize, 
                                 L7_uint32  numEntries)
{
  L7_uint32 periodIdx;
  L7_int32  n=0, quot, rem;
  L7_BOOL   taskInfoPrinted = L7_FALSE, printTask = L7_FALSE;
  L7_char8  taskName[40];


  if (osapiTaskNameGet(sysapiTaskCpuUtilTable[index].taskId, taskName) != L7_SUCCESS)
  {
    return n;
  }

  for (periodIdx = 0; periodIdx < numEntries; periodIdx++)
  {
    if (sysapiTaskCpuUtilTable[index].periodInfo[periodIdx].expMovingAvg == 0)
    {
      continue;    
    }

    quot = sysapiTaskCpuUtilTable[index].periodInfo[periodIdx].expMovingAvg / L7_CPU_UTIL_SCALAR;
    if (quot == 0)
    {
      rem = (sysapiTaskCpuUtilTable[index].periodInfo[periodIdx].expMovingAvg % L7_CPU_UTIL_SCALAR)/100;
      if (rem == 0)
      {
        continue;        
      }
    }

    printTask = L7_TRUE;
    break;
  }

  if (printTask == L7_FALSE)
  {
    return n;    
  }

  for (periodIdx = 0; periodIdx < numEntries; periodIdx++)
  {
    if (taskInfoPrinted == L7_FALSE)
    {
      n += osapiSnprintf(&taskBuf[n],bufferSize-n,
#ifdef _L7_OS_LINUX_
                         "\n\r%5d    %-19.19s   ",
                         osapiTaskPidGet(sysapiTaskCpuUtilTable[index].taskId),
#else
                         "\n\r%8x %-19.19s   ",
                         sysapiTaskCpuUtilTable[index].taskId,
#endif
                         taskName);

      taskInfoPrinted = L7_TRUE;
    }

    n += osapiSnprintf(&taskBuf[n],bufferSize-n,
                       "   %3d.%2.2d%%  ",
                       sysapiTaskCpuUtilTable[index].periodInfo[periodIdx].expMovingAvg / L7_CPU_UTIL_SCALAR,
                       ((sysapiTaskCpuUtilTable[index].periodInfo[periodIdx].expMovingAvg % L7_CPU_UTIL_SCALAR)/100));
  }

  return n;
}


int sysapiTaskUtilPrint (L7_char8 *outputBuf,
                         L7_uint32 bufferSize)
                         
{

  L7_uint32 i, numEntries;
  L7_int32 n=0;
  cpuPeriodUtilInfo_t cpuUtil[L7_CPU_UTIL_MEASURE_MAX_PERIODS];

  memset(cpuUtil, 0, sizeof(cpuUtil));

  numEntries = sysapiTotalCpuUtilGet(cpuUtil);

  n += osapiSnprintf(&outputBuf[n],bufferSize-n,"\r\n\r\nCPU Utilization:\r\n");

  n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                     "\r\n");

  n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                    "\r\n %-8s  %-19s "," PID","Name");

  for (i = 0; i <  numEntries; i++)
  {
    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                       "%5d %4s  ", 
                       cpuUtil[i].timePeriod, "Secs");
  }


  n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                     "\r\n-------------------------------");
  for (i = 0; i <  numEntries; i++)
  {
    if (i > 0)
    {
      n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                         "--");
    }
    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                       "----------");
  }

  /* Print task level utilization */

  sysapiCpuUtilLockTake();

  for (i = 0; i < sysapiTaskCpuUtilTableNumEntries; i++)
  {
    if (n > bufferSize)
    {
      break;    
    }
  
    if (osapiTaskIdVerify(sysapiTaskCpuUtilTable[i].taskId) == L7_SUCCESS)
    {
      n += printTaskCpuUtilization(i, &outputBuf[n], bufferSize-n, numEntries);
    }
  }

  sysapiCpuUtilLockGive();

    
  if (n <= bufferSize)
  {
    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                       "\r\n-------------------------------");
  }

  for (i = 0; i <  numEntries; i++)
  {
    if (n > bufferSize)
    {
      break;    
    }

    if (i > 0)
    {
      n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                         "--");
    }

    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                       "----------");
  }

  if (n <= bufferSize)
  {
    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                      "\r\n Total CPU Utilization        ");
  }

  for (i = 0; i <  numEntries; i++)
  {
    if (n > bufferSize)
    {
      break;    
    }
    n += osapiSnprintf(&outputBuf[n],bufferSize-n,
                       "    %3d.%2.2d%% ",
                      cpuUtil[i].expMovingAvg / L7_CPU_UTIL_SCALAR,
                      ((cpuUtil[i].expMovingAvg % L7_CPU_UTIL_SCALAR)/100));
  }

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
  L7_uint32 i, n = 0, periodIdx;
  L7_uint32 activeTasks = 0;
  L7_int32  quot,rem;
  L7_BOOL   found;
  L7_RC_t   rc = L7_SUCCESS;

  sysapiCpuUtilLockTake();

  for (i = 0;i < sysapiTaskCpuUtilTableNumEntries; i++)
  {
    found = L7_FALSE;
    for (periodIdx = 0; periodIdx < sysapiTaskCpuUtilTable[i].numPeriods; periodIdx++)
    {
      if ((sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg == 0) ||
          (sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].firstAvgDone == L7_FALSE))
      {
        continue;    
      }

      quot = sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg / L7_CPU_UTIL_SCALAR;
      if (quot == 0)
      {
        rem = (sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg % L7_CPU_UTIL_SCALAR)/100;
        if (rem == 0)
        {
          continue;        
        }
      }

        found = L7_TRUE;
        break;          
      }
    
    if (found == L7_TRUE)
    {
      activeTasks++;
      if ((processIndex + 1) == activeTasks)
      {
        break;
      }
    }
  }

  if ((i == sysapiTaskCpuUtilTableNumEntries) || ((processIndex + 1) > activeTasks))
  {
    rc = L7_FAILURE;
  }
  else
  {
    if (osapiTaskNameGet(sysapiTaskCpuUtilTable[i].taskId, taskNameString) != L7_SUCCESS)
      osapiStrncpy(taskNameString, "Task Deleted.", taskNameSize);


    for (periodIdx = 0; periodIdx < sysapiTaskCpuUtilTable[i].numPeriods; periodIdx++)
    {
      if (n > taskPercentSize)
      {
        rc = L7_FAILURE;
        break;          
      }

      if (sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].firstAvgDone == L7_TRUE)

      {

      n += osapiSnprintf(&taskPercentString[n],taskPercentSize-n,
                         "%5d Sec (%3d.%2.2d%%) ",
                         sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].timePeriod,
                         sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg / L7_CPU_UTIL_SCALAR,
                         ((sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg % L7_CPU_UTIL_SCALAR)/100));
    }
    }

  }

  sysapiCpuUtilLockGive();

  return rc;
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
  L7_uint32 i, periodIdx;
  L7_BOOL   found;
  L7_uint32 activeTasks = 0;
  L7_uint32 taskId;
  L7_int32  quot, rem;
  L7_RC_t   rc = L7_SUCCESS;
 
  sysapiCpuUtilLockTake();

  for (i = 0;i < sysapiTaskCpuUtilTableNumEntries; i++)
  {
    found = L7_FALSE;
    for (periodIdx = 0; periodIdx < sysapiTaskCpuUtilTable[i].numPeriods; periodIdx++)
    {
      if ((sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg == 0) ||
          (sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].firstAvgDone == L7_FALSE))
      {
        continue;    
      }

      quot = sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg / L7_CPU_UTIL_SCALAR;
      if (quot == 0)
      {
        rem = (sysapiTaskCpuUtilTable[i].periodInfo[periodIdx].expMovingAvg % L7_CPU_UTIL_SCALAR)/100;
        if (rem == 0)
        {
          continue;        
        }
      }

        found = L7_TRUE;
        break;          
      }
    
    if (found == L7_TRUE)
    {
      activeTasks++;
      if ((processIndex + 1) == activeTasks)
      {
        break;
      }
    }
  }

  if ((i == sysapiTaskCpuUtilTableNumEntries) || ((processIndex + 1) > activeTasks))
  {
    rc = L7_FAILURE;  
  }
  else
  {
#ifdef _L7_OS_LINUX_
    taskId=osapiTaskPidGet(sysapiTaskCpuUtilTable[i].taskId);
    osapiSnprintf(taskIdString, taskIdSize, "%d", taskId);
#else
    taskId=sysapiTaskCpuUtilTable[i].taskId;
    osapiSnprintf(taskIdString, taskIdSize, "%x", taskId);
#endif
  }
 
  sysapiCpuUtilLockGive();

  return rc;
}

/*********************************************************************
*
* @purpose Returns the total CPU utilization for various periods
*
* @param cpuUtil {(input)} Pointer to memory to store cpu util. The memory
*                          must be large enough to store info for up to
*                          L7_CPU_UTIL_MEASURE_MAX_PERIODS.
*
* @returns Number of entries in the array.
*
* @notes Return utilization values are scaled with L7_CPU_UTIL_SCALAR
*
* @end
*
*********************************************************************/
L7_uint32 sysapiTotalCpuUtilGet (cpuPeriodUtilInfo_t *cpuUtil)
{
  L7_uint32 idx, numEntries = 0;

  sysapiCpuUtilLockTake();

  for (idx = 0; idx < sysapiTotalCpuUtilTableNumEntries; idx++)
  {
    /* Skip entries for which first avg has not yet been calculated */
    if (sysapiTotalCpuUtilTable[idx].periodInfo.firstAvgDone == L7_FALSE)
    {
      continue;    
    }

    memcpy(cpuUtil, &(sysapiTotalCpuUtilTable[idx].periodInfo), sizeof(*cpuUtil));
    cpuUtil++;
    numEntries++;
  }

  sysapiCpuUtilLockGive();

  return numEntries;
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
  L7_uint32 totalPeriods;
  L7_uint32 argv[2];

  do
  {

    totalPeriods = sizeof(sysapiCpuUtilDefaultMonitorPeriods)/ sizeof(L7_uint32);
    if (totalPeriods >= L7_CPU_UTIL_MEASURE_MAX_PERIODS)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Cpu utilization default measurement table is incorrectly set."
                    " The number of entries in default measurement table should be"
                    " less than L7_CPU_UTIL_MEASURE_MAX_PERIODS\n");     
      rc = L7_FAILURE;
      break;
    }
  
    sysapiCpuUtilTblSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
    if (sysapiCpuUtilTblSema == L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Cpu utilization table semaphore creation failed\n");     
      rc = L7_FAILURE;
      break;
    }

    sysapiTotalCpuUtilTableInit();

    memset(sysapiTaskCpuUtilTable, 0x00, sizeof(sysapiTaskCpuUtilTable));

    cpuUtilMonitorTaskPtr = osapiTaskCreate("cpuUtilMonitorTask",
                                            osapiCpuUtilMonitorTask,
                                            2,
                                            &argv,
                                            L7_DEFAULT_STACK_SIZE,
                                            1,
                                            1);

    if ( cpuUtilMonitorTaskPtr == L7_ERROR )
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                    "Cpu utilization task creation failed\n");     
      rc = L7_FAILURE;
      break;
    }

  } while (0);

#endif

  return rc;
}

/*********************************************************************
* @purpose  Suspend the cpu utilization monitoring. Useful if spy needs 
*           to be run.
*
* @param    flag {(input)}  L7_TRUE: Suspend the cpu util monitoring
*                           L7_FALSE: Resume the cpu util monitoring
*
* @returns  none
*
* @comments 
*
* @end
*
*********************************************************************/
void sysapiCpuUtilMonitorSuspend(L7_BOOL flag)
{
  sysapiCpuUtilMonitorSuspended = flag;
}


/*********************************************************************
* @purpose  Returns if cpu util monitoring is active
*
* @param    none
*
* @returns  L7_TRUE: Cpu util monitoring is active
*           L7_FALSE: Cpu util monitoring is inactive
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL sysapiCpuUtilMonitorActive()
{
  L7_BOOL active = L7_TRUE;

  if (sysapiCpuUtilMonitorSuspended == L7_TRUE)
  {
    active = L7_FALSE;    
  }

  return active;
  
}

void sysapiCpuUtilTableDump(L7_BOOL showTasks)
{
  L7_uint32 periodIdx, taskIdx;

  sysapiCpuUtilLockTake();

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                "Total number of entries in sysapiTotalCpuUtilTable: %d\n",
                sysapiTotalCpuUtilTableNumEntries);

  for (periodIdx = 0; periodIdx < sysapiTotalCpuUtilTableNumEntries; periodIdx++)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "\nIndex %d details \n", periodIdx);
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "timePeriod %d startTime %d firstAvgDone %d EMA %d \n",
                  sysapiTotalCpuUtilTable[periodIdx].periodInfo.timePeriod,
                  sysapiTotalCpuUtilTable[periodIdx].periodInfo.measureStartTime,
                  sysapiTotalCpuUtilTable[periodIdx].periodInfo.firstAvgDone,
                  sysapiTotalCpuUtilTable[periodIdx].periodInfo.expMovingAvg); 

    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "rThr %d rTrapSent %d fThr %d fTimePeriod %d fStartTime %d"
                  "fFirstAvgDone %d fEMA %d \n",
                  sysapiTotalCpuUtilTable[periodIdx].risingThreshold,
                  sysapiTotalCpuUtilTable[periodIdx].risingTrapSent,
                  sysapiTotalCpuUtilTable[periodIdx].fallingThreshold,
                  sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.timePeriod,
                  sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.measureStartTime,
                  sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.firstAvgDone,
                  sysapiTotalCpuUtilTable[periodIdx].fallingPeriodInfo.expMovingAvg);
  }
  
  if (showTasks == L7_TRUE)
  {
    for (taskIdx = 0; taskIdx < sysapiTaskCpuUtilTableNumEntries; taskIdx++)
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, 
                    "Task-id %x numPeriods %d pendingAge %d\n",
                    sysapiTaskCpuUtilTable[taskIdx].taskId,
                    sysapiTaskCpuUtilTable[taskIdx].numPeriods,
                    sysapiTaskCpuUtilTable[taskIdx].pendingAge);

      for (periodIdx = 0; periodIdx < sysapiTotalCpuUtilTableNumEntries; periodIdx++)
      {
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                     "\nPeriod Index %d details \n", periodIdx);
        SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                      "timePeriod %d startTime %d firstAvgDone %d EMA %d \n",
                       sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].timePeriod,
                       sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].measureStartTime,
                       sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].firstAvgDone,
                       sysapiTaskCpuUtilTable[taskIdx].periodInfo[periodIdx].expMovingAvg); 
      
      }  
    }
  }

  sysapiCpuUtilLockGive();
}


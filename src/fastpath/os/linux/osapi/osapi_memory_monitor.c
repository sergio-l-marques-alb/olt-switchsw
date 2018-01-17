/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename  osapi_memory_monitor.c
*
* @purpose   To profile RAM usage of FASTPATH components.
*            
*
* @component osapi
*
* @create    09/25/2007
*
* @author    Nitin Shrivastav
*
* @end
*
*********************************************************************/


#include "l7_common.h"
#include "l7_resources.h"

#ifdef L7_MEMORY_PROFILER_ENABLE


#include <string.h>

#include "osapi.h"
#include "osapi_support.h"
#include "bspapi.h"
#include "sysapi.h"
#include "trace_api.h"
#include "l7_cnfgr_api.h"
#include "registry.h"

/* Maximum number of samples. */
#define  MEMORY_PROFILER_MAX_SAMPLE_INDEX 5

#define  MEMORY_PROFILER_MAX_TRACE_ENTRIES 1000
#define  MEMORY_PROFILER_TRACE_ENTRY_WIDTH 16

#define MEMORY_PROFILER_MEM_DECREASE 0
#define MEMORY_PROFILER_MEM_INCREASE 1

#define MEMORY_PROFILER_LOW_MEM_THRESHOLD (1024*1024)
#define MEM_PROFILER_COMP_MAX_ALLOCATION  (500)
#define MEM_PROFILER_COMP_MEM_ALLOC_FILE   "memSnapshot.log"

typedef struct totalMemInfo_s
{
  L7_uint32 numBytesAllocated;
  L7_uint32 numBytesFree;
} totalMemInfo_t;

typedef struct sampleInfo_s
{
  L7_uint32 interval;/* Sampling intervals in seconds. */
  L7_uint32 counter; 
  L7_uint32 traceHandle; 
  L7_BOOL   sampleTotalMem; /* Whether to sample total memory or not. */
  L7_BOOL   traceFlag; /* Whether to write changes in trace block or not. */
} sampleInfo_t;

/* This table stores the allocations for various components for last sampling period. */
static L7_uint32 compMemAllocHistory[L7_LAST_COMPONENT_ID][MEMORY_PROFILER_MAX_SAMPLE_INDEX];

/* This table stores the changes in allocations for various components for the sampling period. */
static L7_int32 compMemAllocChange[L7_LAST_COMPONENT_ID][MEMORY_PROFILER_MAX_SAMPLE_INDEX];

/* This stores the total memory allocation for last sampling period. */
static totalMemInfo_t totalMemAllocHistory[MEMORY_PROFILER_MAX_SAMPLE_INDEX];

/* This table stores the changes in total memory allocations. */
static L7_int32 totalMemAllocChange[MEMORY_PROFILER_MAX_SAMPLE_INDEX];

static sampleInfo_t sampleData[MEMORY_PROFILER_MAX_SAMPLE_INDEX] = { {5*60, 0, ~0, 1, 1},
                                                                     {30*60, 0, ~0, 1, 1},   
                                                                     {60*60, 0, ~0, 1, 1},
                                                                     {12*60*60, 0, ~0, 1, 1},   
                                                                     {24*60*60, 0, ~0, 1, 1}   
															       };   

static L7_BOOL traceBlocksCreated = L7_FALSE;

static L7_BOOL memProfilerFlashLoggingEnable = L7_TRUE;
static L7_BOOL memProfilerFreeMemThreshold = MEMORY_PROFILER_LOW_MEM_THRESHOLD;
static L7_BOOL memProfilerFlashLogSaved = L7_FALSE;
static L7_uint32 memProfilerCompMaxAllocation = MEM_PROFILER_COMP_MAX_ALLOCATION;

static L7_BOOL memoryProfilerEnable = L7_FALSE;
static L7_BOOL memoryProfilerInit = L7_FALSE;

extern void osapiMemProfilerUpdate(void);
extern void osapiMemProfilerInit(void);
extern L7_RC_t cnfgrApiSystemStateGet (L7_CNFGR_STATE_t * currentState);


extern L7_osapiMallocUsage_t osapiMallocMemUsage[L7_LAST_COMPONENT_ID];
extern void osapiDebugMallocDetailEnable(L7_BOOL state, L7_uint32 numEntries);
extern void osapiDebugMallocDetail(L7_uint32 comp_id, L7_uint32 size,
                                   L7_uint32 time_stamp, L7_uint32 allocationsRequested,
						           FILE *filePtr);

/*********************************************************************
* @purpose  Debug routine to enable Memory Profiler.
*
* @param    enable    Enable/Disable memory profiler functionality.
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemoryProfilerEnable(L7_BOOL flag)
{
  if (flag == L7_FALSE) 
  {
    memoryProfilerInit = L7_FALSE;
  }
  memoryProfilerEnable = flag;
}

/*********************************************************************
* @purpose  Debug routine to set various parameters of a sampleId.
*
* @param    sampleId        There are five available sampleIds (0 to 4).
* @param    sampleFrequency Sampling interval in secs. 
* @param    sampleTotalMem  Sample total memory also along with per 
*                           component memory. 
* @param    traceFlag       Write the sampling reports in a trace buffer.
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemProfilerSetSampleFrequenecy(L7_uint32 sampleId, L7_uint32 sampleFrequency,
										 L7_BOOL sampleTotalMem, L7_BOOL traceFlag)
{
  L7_uint32 compId;
   
  if (sampleId >= MEMORY_PROFILER_MAX_SAMPLE_INDEX) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "Incorrect sampleId. SampleId should be between 0 and 4\n");
	return;
  }

  for (compId = L7_FIRST_COMPONENT_ID; compId < L7_LAST_COMPONENT_ID; compId++) 
  {
    compMemAllocHistory[compId][sampleId] = 0;
	compMemAllocChange[compId][sampleId] = 0;
  }
  totalMemAllocHistory[sampleId].numBytesAllocated = 0;
  totalMemAllocHistory[sampleId].numBytesFree = 0;
  totalMemAllocChange[sampleId] = 0;

  sampleData[sampleId].interval= sampleFrequency; 
  sampleData[sampleId].counter = 0; 
  sampleData[sampleId].sampleTotalMem = sampleTotalMem;
  sampleData[sampleId].traceFlag = traceFlag;

}

/*********************************************************************
* @purpose  If the system free memory is below a threshold, memory monitor
*           can be enabled to take a snapshot of the per component allocation and
*           last 'x' number of allocations of component that has allocated
*           maximum memory and write it in the flash.
*           This debug routine sets various paramter for this.
*
* @param    enableFlashLog   Enable/Disable writing of memory snapshot to flash.
* @param    memoryThreshold  Log to the flash if the available memory falls below
*                            this threshold.
* @param    numAllocations   Write these many allocations of the component taking
*                            maximum memory. 
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemProfilerSetFlashLoggingParam(L7_uint32 enableFlashLogging, L7_uint32 memoryThreshold,
									      L7_uint32 numAllocations)
{
  memProfilerFlashLoggingEnable = enableFlashLogging;
  memProfilerFreeMemThreshold = memoryThreshold;
  memProfilerCompMaxAllocation = numAllocations;  
}


static void osapiMemProfilerPrintComponentStats(L7_uint32 compId)
{
  L7_uint32 sampleIndex = 0;

  for (sampleIndex = 0; sampleIndex < MEMORY_PROFILER_MAX_SAMPLE_INDEX; sampleIndex++) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "%10d             %-10d                            %-10d\n", 
				  sampleData[sampleIndex].interval,
				  compMemAllocHistory[compId][sampleIndex],
				  compMemAllocChange[compId][sampleIndex]);
  }
}

/*********************************************************************
* @purpose  Debug routine to print sampling report for a component
*
* @param    compId 
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemProfilerReport(L7_uint32 component)
{
  L7_uint32 sampleIndex = 0, compId;


  if (component == 0) 
  {
    for (compId = L7_FIRST_COMPONENT_ID; compId < L7_LAST_COMPONENT_ID; compId++) 
    {
      SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
	              "Component Id %d \n",compId);
	  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
	                "Sample-Interval    MemoryAllocated                 MemoryChange \n");
	  osapiMemProfilerPrintComponentStats(compId);
	}

  }
  else
  {
	if (component >= L7_LAST_COMPONENT_ID) 
	{
	  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
					"Invalid component id\n");
	  return;
	  
	}

	SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				"Component Id %d \n",component);
	SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "Sample-Interval    MemoryAllocated                 MemoryChange \n");
    osapiMemProfilerPrintComponentStats(component);
  }

  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
			  "Total Allocation stats\n");
  SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				"Sample-Interval    MemoryAllocated                 MemoryChange \n");
  for (sampleIndex = 0; sampleIndex < MEMORY_PROFILER_MAX_SAMPLE_INDEX; sampleIndex++) 
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "%10d             %-10d                            %-10d\n", 
				  sampleData[sampleIndex].interval,
				  totalMemAllocHistory[sampleIndex].numBytesAllocated,
				  totalMemAllocChange[sampleIndex]);
  }

  return;
}

/*********************************************************************
* @purpose  Debug routine to dump sampling data-structures. Useful
*           to get the traceId for a sampling period.
*
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemProfilerSampleInfoShow()
{
  L7_uint32 sampleIndex;
  for (sampleIndex = 0; sampleIndex < MEMORY_PROFILER_MAX_SAMPLE_INDEX; sampleIndex++) 
  {
	SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "\nSample Id %d\n", sampleIndex);
	SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "interval %d counter %d\n", 
				  sampleData[sampleIndex].interval,
				  sampleData[sampleIndex].counter);
	SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
				  "traceHandle %d traceFlag %d sampleTotalMem %d\n", 
				  sampleData[sampleIndex].traceHandle,
				  sampleData[sampleIndex].traceFlag,
				  sampleData[sampleIndex].sampleTotalMem);
  }
}

/*********************************************************************
* @purpose Initialize the Memory Profiler.
*
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemProfilerInit(void)
{
  L7_uint32 compId, sampleIndex;
  L7_uint32 totalMemory;


  memset(totalMemAllocHistory, 0, sizeof(totalMemAllocHistory));
  memset(totalMemAllocChange, 0, sizeof(totalMemAllocChange));

  for (sampleIndex = 0; sampleIndex < MEMORY_PROFILER_MAX_SAMPLE_INDEX; sampleIndex++) 
  {
    for (compId = L7_FIRST_COMPONENT_ID; compId < L7_LAST_COMPONENT_ID; compId++) 
	{
	  compMemAllocHistory[compId][sampleIndex] = osapiMallocMemUsage[compId].currentMemUsage;
	}

	(void)osapiGetMemInfo(&totalMemory,
                          &totalMemAllocHistory[sampleIndex].numBytesAllocated,
						  &totalMemAllocHistory[sampleIndex].numBytesFree);

	if (traceBlocksCreated == L7_FALSE) 
	{
	  if (traceBlockCreate(MEMORY_PROFILER_MAX_TRACE_ENTRIES, 
						   MEMORY_PROFILER_TRACE_ENTRY_WIDTH,
						   (L7_uchar8 *)"OSAPI_MEMORY_PROFILER", 
						   &(sampleData[sampleIndex].traceHandle)) != L7_SUCCESS)
	  {
	  }
	  else
	  {
	    traceBlockStart(sampleData[sampleIndex].traceHandle);
	  }
	}
  }

  traceBlocksCreated = L7_TRUE;


  return;
}

/*********************************************************************
* @purpose Internal routine to add trace.
*
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
static void osapiMemProfilerAddTrace(L7_uint32 sampleIndex, L7_uint32 compId, L7_int32 bytesChange)
{
  if (bytesChange != 0) 
  {
    TRACE_BEGIN(sampleData[sampleIndex].traceHandle, L7_OSAPI_COMPONENT_ID);
	TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiTimeMillisecondsGet()));
	TRACE_ENTER_2BYTE(osapiHtons((L7_ushort16)compId));

	if (bytesChange < 0) 
	{
	  TRACE_ENTER_BYTE(MEMORY_PROFILER_MEM_DECREASE);
	  bytesChange *= (-1);
	}
	else
	{
	  TRACE_ENTER_BYTE(MEMORY_PROFILER_MEM_INCREASE);
	}

    TRACE_ENTER_4BYTE(osapiHtonl(bytesChange));
	TRACE_END();
  }

  return;
}

/*********************************************************************
* @purpose Internal routine to save the memory snapshot in the flash.
*
*
*
*
* @returns  none.
* 
*
* @end
*********************************************************************/
void osapiMemProfilerLogToFlash(void)
{
  FILE *fp;
  L7_uint32 compId, maxCompId = L7_FIRST_COMPONENT_ID, maxCompMemUsage = 0;
  L7_char8 componentName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 currentMemUsage;
  L7_uint32 maxMemUsage;
  L7_uint32 totalUsage = 0;
  char      swVersion[80];
  int       currTime = osapiUTCTimeNow();

  fp = fopen(MEM_PROFILER_COMP_MEM_ALLOC_FILE, "w");
  if (fp == L7_NULL) 
  {
    return;
  }

  memset(swVersion, 0, sizeof(swVersion));
  sysapiRegistryGet(SW_VERSION, STR_ENTRY, (void *) swVersion);

  fprintf(fp,"Software Version: %s\n",swVersion);
  fprintf(fp,"Memory allocation log created %s\n\n",
          ctime((void *)&currTime));
  fprintf(fp, "Summary of component memory allocation\n");
  fprintf(fp, "Component                    CurrentAllocated          MaxAllocated \n");
  for (compId = L7_FIRST_COMPONENT_ID; compId < L7_LAST_COMPONENT_ID; compId++) 
  {
    memset(componentName, 0, sizeof(componentName));
    cnfgrApiComponentNameGet(compId, componentName);
    currentMemUsage = osapiMallocMemUsage[compId].currentMemUsage;
	if (currentMemUsage > maxCompMemUsage) 
	{
	  maxCompMemUsage = currentMemUsage;
	  maxCompId = compId;
	}
    maxMemUsage = osapiMallocMemUsage[compId].maxMemUsage;

    if (currentMemUsage || maxMemUsage) 
    {
      totalUsage += currentMemUsage;
      fprintf(fp, "%d - %-31.30s %8d                 %8d\n", 
              compId,componentName, currentMemUsage, maxMemUsage);
    }
  }
  fprintf(fp, "Total Current Usage = %d\n",totalUsage);

  fprintf(fp, "\n Last %d allocations for component %d\n", 
		  memProfilerCompMaxAllocation, maxCompId);

  osapiDebugMallocDetailEnable(1, memProfilerCompMaxAllocation);
  osapiDebugMallocDetail(maxCompId, 0, 0, memProfilerCompMaxAllocation, fp);
  osapiDebugMallocDetailEnable(0, memProfilerCompMaxAllocation);

  fclose(fp);

  return;


}
/*********************************************************************
* @purpose Check if memory usage has to be sampled.  
*
* @param    none.
*
* @returns  none.
* 
* @comments none.
*
* @end
*********************************************************************/
void osapiMemProfilerUpdate(void)
{
  L7_uint32 sampleIndex = 0, compId;
  L7_int32  changeUsage = 0;
  totalMemInfo_t currentMemInfo;
  L7_uint32 totalMemory;


  for (sampleIndex = 0; sampleIndex < MEMORY_PROFILER_MAX_SAMPLE_INDEX; sampleIndex++) 
  {
	sampleData[sampleIndex].counter += (bspapiWatchdogInterval() / 1000);

	/* Time to update this sample. */
    if (sampleData[sampleIndex].counter >= sampleData[sampleIndex].interval) 
	{
	  for (compId = L7_FIRST_COMPONENT_ID; compId < L7_LAST_COMPONENT_ID; compId++) 
	  {
		changeUsage = 0;
		changeUsage =  (osapiMallocMemUsage[compId].currentMemUsage - 
						compMemAllocHistory[compId][sampleIndex]);

	    compMemAllocChange[compId][sampleIndex] = changeUsage;
		compMemAllocHistory[compId][sampleIndex] = osapiMallocMemUsage[compId].currentMemUsage;

		if (sampleData[sampleIndex].traceFlag == L7_TRUE) 
		{
		  osapiMemProfilerAddTrace(sampleIndex, compId, changeUsage);
		}
	  }

	  if (sampleData[sampleIndex].sampleTotalMem == L7_TRUE) 
	  {
        changeUsage = 0;
		currentMemInfo.numBytesAllocated = totalMemAllocHistory[sampleIndex].numBytesAllocated;
		currentMemInfo.numBytesFree = totalMemAllocHistory[sampleIndex].numBytesFree;
		(void)osapiGetMemInfo(&totalMemory, &currentMemInfo.numBytesAllocated,&currentMemInfo.numBytesFree);

		changeUsage = currentMemInfo.numBytesAllocated - totalMemAllocHistory[sampleIndex].numBytesAllocated;
	    totalMemAllocChange[sampleIndex] = changeUsage;

		totalMemAllocHistory[sampleIndex].numBytesAllocated = currentMemInfo.numBytesAllocated;
		totalMemAllocHistory[sampleIndex].numBytesFree = currentMemInfo.numBytesFree;

		if (sampleData[sampleIndex].traceFlag == L7_TRUE) 
		{
		  osapiMemProfilerAddTrace(sampleIndex, 0xff, changeUsage);
		}

		/* Available memory is too low. Save the current allocation of each component in
		** a file in the flash. Also, save last few allocations of the component taking
		** maximum memory.
		*/
		if ((memProfilerFlashLoggingEnable == L7_TRUE) && (currentMemInfo.numBytesFree < memProfilerFreeMemThreshold)) 
		{
		  if (memProfilerFlashLogSaved == L7_FALSE) 
		  {
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
						  "Available memory in the system is less than %d bytes, taking a snapshot of the memory allocation...\n",memProfilerFreeMemThreshold );
			l7_logf(L7_LOG_SEVERITY_ALERT, L7_OSAPI_COMPONENT_ID, __FILE__, __LINE__,
					"Available memory in the system is less than %d bytes, taking a snapshot of the memory allocation..."
                    "A memory profiler task is enabled, and has decided to take a snapshot of RAM allocations per component.", memProfilerFreeMemThreshold);
		    osapiMemProfilerLogToFlash();
			memProfilerFlashLogSaved = L7_TRUE;
            SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
						  "Finished taking snapshot of the memory allocation.\n");
			l7_logf(L7_LOG_SEVERITY_ALERT, L7_OSAPI_COMPONENT_ID, __FILE__, __LINE__,
					"Finished taking snapshot of the memory allocation.\n");
		  }
		}
	  }

	  sampleData[sampleIndex].counter = 0;
	}
  }

  return;
}

/*********************************************************************
* @purpose Outcall from the osapiMonitor task.  
*
* @param    none.
*
* @returns  none.
* 
* @comments none.
*
* @end
*********************************************************************/
void osapiMemoryProfilerNotify(void)
{
  L7_CNFGR_STATE_t systemState;

  if (memoryProfilerEnable) 
  {
	if (memoryProfilerInit == L7_FALSE) 
	{
	  /* wait for configurator to be in execute state */
	  systemState = L7_CNFGR_STATE_IDLE;
      cnfgrApiSystemStateGet (&systemState);
      if (L7_CNFGR_STATE_E == systemState)
	  {
	    osapiMemProfilerInit();
		memoryProfilerInit = L7_TRUE;
	  }
	}
	else
	{
	  osapiMemProfilerUpdate();
	}
  }

  return;

}
#endif


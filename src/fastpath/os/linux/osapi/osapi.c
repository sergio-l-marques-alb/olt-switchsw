/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi.c
*
* @purpose   OS independent API's
*
* @component osapi
*
* @comments
*
* @create    11/20/2001
*
* @author    John Linville
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/times.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <sys/vfs.h>

#include "l7_common.h"
#include "osapi.h"
#include "osapi_support.h"
#include "nvstoreapi.h"
#include "usmdb_cnfgr_api.h"
#include "support_api.h"
#include "sysapi.h"
#include "registry.h"
#include "simapi.h"

#include "bspapi.h"
#include "log.h"
#include "l7utils_api.h"

#ifdef OSAPI_NATIVE_MALLOC
#undef malloc
#undef free
#undef realloc
#undef strdup
#undef calloc

void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
#endif

static pthread_mutex_t threadsafe_lock = PTHREAD_MUTEX_INITIALIZER;

/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
static pthread_once_t nvstore_once = PTHREAD_ONCE_INIT;
#endif

static L7_BOOL osapiStackDumpLogMode = L7_FALSE;

void osapiDebugSysInfoDumpRegister(void);
void osapiDebugRegisteredSysInfoDump(void);

int osapi_printf(const char *fmt, ...)
{
  int rc;
  va_list args;

  va_start(args, fmt);
  rc = vprintf(fmt, args);
  va_end(args);

  return(rc);
}



/* In order to simplify debugging of memory corruption problems we
** provide debug information inside each memory block allocated with
** osapiMalloc() macro. The information includes file name and line number
** of the allocating function as well as memory size and task id.
** The allocated memory block has start and end sentinel that can be
** checked to make sure that memory is not corrupted.
**
** The following debug functions are available:
**   osapiDebugTestMemorySet() - Sets flag that when enabled will cause a LOG_ERROR
**                               if the sentinel is corrupted when memory is freed.
**   osapiDebugMemoryCheck() - Searches the list of allocated memory blocks for
**                             corrupted memory. Information about the first block
**                             is displayed.
**   osapiDebugMemoryStats() - Display memory allocation statistics for each task.
**   osapiDebugMemoryInfo() - Display information about allocated memory blocks.
**
**   osapiDebugMallocSummary() - Display memory usage by component 
**
**   osapiDebugMallocDetailEnable() - Enable or Disable osapiDebugMallocDetail() functionality.
**                                       
**   osapiDebugMallocDetail()- Display requested outstanding allocations for a Component Id 
**                               and a total summary. This is production safe and only is enabled
**                               if the osapiDebugMallocDetailEnable(enable) is called.
**
** Define this constant to enable basic memory debugging. This constant only
** enables sentinels.
**
** #undef OSAPI_MEM_DEBUG
**
** This define is needed for MemoryCheck, MemoryStats and MemoryInfo, 
** MallocDetail and MallocDetailEnable functions.
**
** #undef OSAPI_MEM_LIST
*/

/* For valgrind the memory checker we do not need any sentinel code*/
#ifndef L7_TOOL_VALGRIND
#define OSAPI_MEM_DEBUG
#define OSAPI_MEM_LIST
#endif
/* If MEM_LIST is defined then we must also define MEM_DEBUG.
*/
#ifdef OSAPI_MEM_LIST
#define OSAPI_MEM_DEBUG
#endif


#ifdef OSAPI_MEM_DEBUG
#define OSAPI_TRACK_FILE_NAME_SIZE 16

/* Memory allocation structure
*/
typedef struct osapiMemAllocType_s {
  L7_uint32 data_start;
  L7_uint32 length;
  L7_uint32 task_id;
  L7_uint32 in_use;
  L7_uint32 component_id;
#ifdef OSAPI_MEM_LIST
  struct osapiMemAllocType_s *prev;
  struct osapiMemAllocType_s *next;
  L7_uchar8 file_name [OSAPI_TRACK_FILE_NAME_SIZE];
  L7_uint32 line_number;
  L7_uint32 time_stamp;
#endif
  L7_uint32 user_data[1];
} osapiMemAllocType;

#define OSAPI_MEM_START (0xA7A7A7A7)
#define OSAPI_MEM_END (0xB7B7B7B7)

/* Set this flag to 1 in order to change default behavior to LOG_ERROR
** when corrupted sentinel is detected on osapiFree()
*/
static L7_uint32 osapiTestMemory = 1;

#ifdef OSAPI_MEM_LIST
void osapi_heap_corruption_dump(osapiMemAllocType *mem, int beginning);
static osapiMemAllocType *allocMemList = 0;

/* Used for osapiDebugMallocDetail */
#define OSAPI_DEBUG_MALLOC_DETAIL_NUM_ALLOCS 5000
L7_uint32 osapiDebugMallocDetailNumAllocs = OSAPI_DEBUG_MALLOC_DETAIL_NUM_ALLOCS;
static osapiMemAllocType *osapiDebugMallocDetailBuffer = L7_NULLPTR;

#endif

/* Used to change the way osapiMalloc_track is implemented  */
static int timMemDebug = 0;
static int debugCompId = 0;
static int debugNumBytes = 0;

/* Declaration for osapiMalloc() debug functionality */
L7_osapiMallocUsage_t osapiMallocMemUsage[L7_LAST_COMPONENT_ID];

static L7_uint32 min_mem_alloc = 0xFFFFFFFF;
static L7_uint32 max_mem_alloc = 0;

/**************************************************************************
*
* @purpose  Debug function to enable/disable checking of allocated memory
*           during osapiFree. When this flag is enabled the code verifies
*           that start and end sentinel are set properly.
*
* @param    flag  1 - Enable checking
*                 0 - Disable checking.
*
* @returns  none
*
* @comments
*
* @end
*
*************************************************************************/
void osapiDebugTestMemorySet(L7_uint32 flag)
{
  osapiTestMemory = flag;
}

/**************************************************************************
*
* @purpose  Display the amount of memory Malloc'd in the system per component
*
* @param    compId    @b{(input)}  Component Id from L7_COMPONENT_IDS_t
* @param    delta     @b{(input)}  specify whether only delta should be shown
*
* @returns
*
* @comments  If the compId is zero, then all components are displayed
*
* @end
*
*************************************************************************/
void osapiDebugMallocSummary(L7_COMPONENT_IDS_t compId, L7_uint32 delta)
{
  L7_osapiMallocUsage_t *pMallocUsage;
  static L7_uint32 prev_reported[L7_LAST_COMPONENT_ID];
  L7_char8 componentName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 currentMemUsage;
  L7_int32 change;
  L7_uint32 maxMemUsage;
  L7_uint32 totalUsage = 0;
  L7_uint32 overhead = 0;

  memset(componentName, 0, sizeof(componentName));

  printf ("%-31.30s %19s   %s\r\n", "Component",
          "CurrentAllocated[change]", "MaxAllocated[Allocs/Frees]");

  if(compId == 0)
  {
    for(compId = 1; compId < L7_LAST_COMPONENT_ID; compId++)
    {
      pMallocUsage = &osapiMallocMemUsage[compId];
      usmDbComponentNameGet(compId, componentName);
      currentMemUsage = pMallocUsage->currentMemUsage;
      maxMemUsage = pMallocUsage->maxMemUsage;

      if (currentMemUsage || maxMemUsage)
      {
        totalUsage += currentMemUsage;

        if((delta == 0) || (prev_reported[compId] != currentMemUsage))
        {
          change = currentMemUsage - prev_reported[compId];
          printf ("%-3d - %-31.30s %8d[%+8d]   %8d [%d/%d]\r\n",
                  compId,componentName, currentMemUsage, change, maxMemUsage,
                  pMallocUsage->countAlloc, pMallocUsage->countFree);
        }
        prev_reported[compId] = currentMemUsage;
        overhead += (sizeof(osapiMemAllocType) * (pMallocUsage->countAlloc - pMallocUsage->countFree));
      }
    }
    printf ("\r\n");
    printf ("Total Current Usage = %d\r\n",totalUsage);
    printf ("osapi Debug overhead = %d\r\n",overhead);
  }
  else
  {
    pMallocUsage = &osapiMallocMemUsage[compId];
    usmDbComponentNameGet(compId, componentName);
    currentMemUsage = pMallocUsage->currentMemUsage;
    maxMemUsage = pMallocUsage->maxMemUsage;
    if((delta == 0) || (prev_reported[compId] != currentMemUsage))
    {
      change = currentMemUsage - prev_reported[compId];
      printf ("%d - %-16.15s    %6d[%+8d]                   %6d [%d/%d]\n",
              compId,componentName, currentMemUsage, change, maxMemUsage,
              pMallocUsage->countAlloc, pMallocUsage->countFree);
    }
    prev_reported[compId] = currentMemUsage;
  }
}

#endif /* OSAPI_MEM_DEBUG */


#ifdef OSAPI_MEM_LIST
static void osapiDebugMemoryPrint (osapiMemAllocType *mem_alloc)
{
  L7_uint32 mem_size_word = (mem_alloc->length + 3) / 4;

  printf("Addr:    0x%x\n", (L7_uint32) &mem_alloc->user_data[0]);
  printf("Start:   0x%x\n", mem_alloc->data_start);
  printf("End:     0x%x\n", mem_alloc->user_data[mem_size_word]);
  printf("Size:    0x%x\n", mem_alloc->length);
  printf("Task:    0x%x\n", mem_alloc->task_id);
  printf("CompId:  %u\n", mem_alloc->component_id);
  printf("File:    %s\n", mem_alloc->file_name);
  printf("Line:    %d\n", mem_alloc->line_number);
  printf("In Use:  0x%x\n", mem_alloc->in_use);
  printf("Up Time: %d\n", mem_alloc->time_stamp);
  printf("next:    %p\n", mem_alloc->next);
  printf("prev:    %p\n", mem_alloc->prev);
}

static int osapiMemAllocValid(osapiMemAllocType *mem_alloc, char *src)
{
  L7_uint32 memloc = (L7_uint32)mem_alloc;
  L7_uint32 mem_size_word;
  L7_uint32 errtype = 0;

  do
  {
    if(memloc < min_mem_alloc) break;
    if(memloc > max_mem_alloc) break;
    errtype = 1;
    if(mem_alloc->data_start != OSAPI_MEM_START) break;
    memloc = (L7_uint32)mem_alloc->next;
    mem_size_word = (mem_alloc->length + 3) / 4;
    if((memloc != 0) && (memloc < min_mem_alloc)) break;
    if(memloc > max_mem_alloc) break;
    if (mem_alloc->user_data[mem_size_word] != OSAPI_MEM_END)
    {
      printf("%s: memory corruption %p invalid end\n", src, mem_alloc);
      osapiDebugMemoryPrint(mem_alloc);
      return 0;
    }
    return 1;
  }
  while(0);

  if(errtype == 0)
  {
    printf("%s: memory corruption %p outof bounds\n", src, mem_alloc);
  }
  else
  {
    printf("%s: memory corruption %p invalid\n", src, mem_alloc);
    osapiDebugMemoryPrint(mem_alloc);
  }
  return 0;
}

/**************************************************************************
*
* @purpose  Find First corrupted memory region.
*
* @param    none
*
* @returns  none
*
* @comments We don't lock memory while looking for buffers,
*           so the code may crash. Avoid using in production.
*
* @end
*
*************************************************************************/
void osapiDebugMemoryCheck (void)
{
  L7_uint32 num_blocks = 0;
  osapiMemAllocType *mem_alloc;
  L7_uint32 mem_size_word;
  pthread_t self;
  L7_uint32 mem_addr;

  self = pthread_self();
  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
      (void *)&threadsafe_lock);
  pthread_mutex_lock(&threadsafe_lock);

  printf("Checking memory for consistency....\n");

  mem_alloc = allocMemList;

  while ((mem_alloc != 0) && osapiMemAllocValid(mem_alloc, "osapiDebugMemoryCheck"))
  {
    num_blocks++;
    if (mem_alloc->data_start != OSAPI_MEM_START)
    {
      osapiDebugMemoryPrint (mem_alloc);
      osapi_heap_corruption_dump(mem_alloc, 1);
      break;
    }

    mem_size_word = (mem_alloc->length + 3) / 4;
    mem_addr = (unsigned int) mem_alloc->user_data + (mem_size_word * 4);
    if ((mem_addr > max_mem_alloc) || (mem_addr < min_mem_alloc))
    {
      printf("Memory size is too large, mem_addr=0x%x, max_mem_alloc=0x%x, min_mem_alloc=0x%x\n",
		      mem_addr, max_mem_alloc, min_mem_alloc);

      osapiDebugMemoryPrint (mem_alloc);
      break;
    }

    if (mem_alloc->user_data[mem_size_word] != OSAPI_MEM_END)
    {
      osapiDebugMemoryPrint (mem_alloc);
      osapi_heap_corruption_dump(mem_alloc, 0);
      break;
    }

  if ((mem_alloc->next != L7_NULLPTR) &&
	(((L7_uint32) mem_alloc->next > max_mem_alloc) ||
        ((L7_uint32) mem_alloc->next < min_mem_alloc)))
    {
      printf("Pointer to next memory block is invalid: 0x%x\n", (unsigned int) mem_alloc->next);
      osapiDebugMemoryPrint (mem_alloc);
      break;
    }

    mem_alloc = mem_alloc->next;
  }

  printf("Total Blocks Examined = %d\n", num_blocks);

  pthread_cleanup_pop(1);
}

/**************************************************************************
*
* @purpose  Count the number of memory buffers in use by each task.
*
* @param    none
*
* @returns  none
*
* @comments We don't lock memory while looking for buffers,
*           so the code may crash. Don't use in production.
*
*           This function works with up to 1000 tasks.
*
* @end
*
*************************************************************************/
#define OSAPI_MAX_STATS_TASKS 1000
void osapiDebugMemoryStats (void)
{
  static L7_uint32 task_id_table [OSAPI_MAX_STATS_TASKS];
  static L7_uint32 task_id_count [OSAPI_MAX_STATS_TASKS];
  static L7_uint32 task_id_total [OSAPI_MAX_STATS_TASKS];
  L7_uint32 num_blocks = 0;
  L7_uint32 num_overflow = 0;
  L7_uint32 num_tasks = 0;
  osapiMemAllocType *mem_alloc;
  L7_uint32 i;
  L7_uint32 task_id;
  L7_uchar8 task_name[128];
  L7_uint32 total_size = 0;
  L7_RC_t rc;

  memset (task_id_table, 0, sizeof (task_id_table));
  memset (task_id_count, 0, sizeof (task_id_count));
  memset (task_id_total, 0, sizeof (task_id_total));

  printf("Counting memory blocks for each task....\n");

  mem_alloc = allocMemList;

  while ((mem_alloc != 0) && osapiMemAllocValid(mem_alloc, "osapiDebugMemoryStats"))
  {
    num_blocks++;
    task_id = mem_alloc->task_id;

    for (i = 0; i < OSAPI_MAX_STATS_TASKS; i++)
    {
      if ((task_id_table[i] == 0) ||
          (task_id_table[i] == task_id))
      {
        break;
      }
    }

    /* If task table is full then increment overflow counter.
    */
    if (i == OSAPI_MAX_STATS_TASKS)
    {
      num_overflow++;
    } else
    {

      /* If task is not in the table then add it to the table.
      */
      if (task_id_table[i] == 0)
      {
        task_id_table[i] = task_id;
        num_tasks++;
      }

      /* Increment counter for the task.
      */
      task_id_count[i]++;

      task_id_total[i] += mem_alloc->length;

      total_size += mem_alloc->length;
    }

    mem_alloc = mem_alloc->next;

  }

  printf("Num Tasks: %d, Num Blocks: %d, Total Size: %d\n",
         num_tasks, num_blocks, total_size);
  for (i = 0; i < num_tasks; i++)
  {
    memset (task_name, 0, sizeof (task_name));
    if(task_id_table[i] == 0)
    {
      rc = L7_FAILURE;
    }
    else
    {
      rc = osapiTaskNameGet (task_id_table[i], task_name);
    }
    if (rc != L7_SUCCESS)
    {
      strcpy (task_name, "Unknown");
    }
    printf("Task id: 0x%08x -- Task Name: %32s -- Blocks: %6d -- Size: %d\n",
              task_id_table[i],
              task_name,
              task_id_count[i],
              task_id_total[i]);
  }
}

/**************************************************************************
*
* @purpose  Display information about allocated memory blocks.
*
* @param    task_id - For which task to report. 0 - All tasks.
* @param    size - Report blocks greater than this size.
* @param    time_stamp - Report blocks with time stamp greater than this value.
*
* @returns  none
*
* @comments We don't lock memory while looking for buffers,
*           so the code may crash. Don't use in production.
*
* @end
*
*************************************************************************/
void osapiDebugMemoryInfo (L7_uint32 task_id,
                           L7_uint32 size,
                           L7_uint32 time_stamp)
{
  osapiMemAllocType *mem_alloc;
  L7_uint32 total_blocks = 0;
  L7_uint32 total_blocks_match = 0;
  L7_uint32 total_size = 0;
  L7_uint32 total_size_match = 0;

  mem_alloc = allocMemList;
  while ((mem_alloc != 0) && osapiMemAllocValid(mem_alloc, "osapiDebugMemoryInfo"))
  {
    total_blocks++;
    total_size += mem_alloc->length;

    if (((task_id == 0) || (mem_alloc->task_id == task_id)) &&
        (mem_alloc->length > size) &&
        (mem_alloc->time_stamp > time_stamp))
    {
      total_blocks_match++;
      total_size_match += mem_alloc->length;
      printf("Tid: 0x%x - Addr: 0x%x -  %16s (%5d) - Size: %5d - Time: %5d\n",
             mem_alloc->task_id,
             (L7_uint32) &mem_alloc->user_data[0],
             mem_alloc->file_name,
             mem_alloc->line_number,
             mem_alloc->length,
             mem_alloc->time_stamp);
    }

    mem_alloc = mem_alloc->next;

  }

  printf("Total Blocks: %d  -- Total Size: %d\n",
            total_blocks, total_size);
  printf("Total Matching Blocks: %d -- Total Matching Size: %d\n",
          total_blocks_match,
         total_size_match);
}

void osapiDebugMemoryChangedInfo (L7_uint32 task_id, L7_uint32 size)
{
  static L7_uint32 time_stamp = 0;

  if(time_stamp != 0)
  {
    osapiDebugMemoryInfo(task_id, size, time_stamp);
  }

  time_stamp = osapiUpTimeRaw();
}

/**************************************************************************
*
* @purpose  Record information about the allocated blocks in the given file.
*
*
* @param    fp {(input)} File in which to record. Caller is responsible for 
*                        opening the file before calling the routine and closing
*                        the file.
* @param   allocBuffer {(input)} Pointer to a temporary buffer to copy the 
*                                 malloc details. The temporary buffer is needed
*                                 so that the malloc is not blocked for long
*                                 period of time.
* @param    sizeAllocBuffer {(input)} Size of the passed allocBuffer
* @param    comp_id - For which component to report. 0 = All components.
* @param    size - Report blocks greater than this size.
* @param    time_stamp - Report blocks with time stamp greater than this value.
* @param    allocationsRequested - Report this number of allocations, if 0 
*                                  all allocations are displayed.
*
* @returns  none
*
* @comments 
*
* @end
*
*************************************************************************/
static void osapiMallocDetailRecord(FILE *fp,
                                    osapiMemAllocType *allocBuffer,
                                    L7_uint32  sizeAllocBuffer,
                                    L7_uint32 comp_id,
                           L7_uint32 size,
                           L7_uint32 time_stamp,
                                    L7_uint32 allocationsRequested)
                                  
{
  osapiMemAllocType *mem_alloc;
  L7_uint32 total_blocks = 0;
  L7_uint32 total_blocks_match = 0;
  L7_uint32 total_size = 0;
  L7_uint32 total_size_match = 0;
  int i, cnt = 0;
  pthread_t self;

  /* Walk through the alloc list and copy it to the allocBuffer. Then write
   * from the allocBuffer to the file. This step is done to reduce the time
   * for which intLock is done
   */
    self = pthread_self();
    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&threadsafe_lock);
    pthread_mutex_lock(&threadsafe_lock);

    mem_alloc = allocMemList;
    while (mem_alloc != 0)
    {
      total_blocks++;
      total_size += mem_alloc->length;
  
      if (((comp_id == 0) || (mem_alloc->component_id == comp_id)) &&
          (mem_alloc->length > size) &&
          (mem_alloc->time_stamp > time_stamp))
      {
      if (cnt < sizeAllocBuffer)
        memcpy(&allocBuffer[cnt++], mem_alloc, sizeof(osapiMemAllocType));

        total_blocks_match++;
        total_size_match += mem_alloc->length;
      }
      
      mem_alloc = mem_alloc->next;
    }

	pthread_cleanup_pop(1);

    /* Print only those allocations requested, if zero you get all */
    if ((allocationsRequested < cnt) && (allocationsRequested != 0))
    {
      cnt = allocationsRequested;
    }
    
    /* Print the allocations found/requested */
    for(i = 0; i < cnt; i++)
    {
    mem_alloc = &allocBuffer[i];

      fprintf(fp, "Tid: 0x%x - Addr: 0x%x -  %16s (%5d) - Size: %5d - Inuse: %d - Time: %5d\n",
             mem_alloc->task_id,
             (L7_uint32) &mem_alloc->user_data[0],
             mem_alloc->file_name,
             mem_alloc->line_number,
             mem_alloc->length,
             mem_alloc->in_use,
             mem_alloc->time_stamp);
    }
  
    fprintf(fp, "Total Blocks: %d  -- Total Size: %d\n",
              total_blocks, total_size);
    fprintf(fp, "Total Matching Blocks: %d -- Total Matching Size: %d\n",
            total_blocks_match,
            total_size_match);

  return;
}

/**************************************************************************
*
* @purpose  Enable/Disable osapiDebugMallocDetail() by allocating
*           memory for Enable,and deallocating memory for a Disable.
*
* @param    state = Enable = 1 and Disable = 0.
* @param    numEntries Number of entries to be allocated. If 0 is
*           specified, then osapiMallocArraySize entries are allocated.

* @returns  none
*
* @comments none
*
* @end
*
*************************************************************************/
void osapiDebugMallocDetailEnable(L7_BOOL state, L7_uint32 numEntries)
{
  L7_uint32 numAllocEntries = osapiDebugMallocDetailNumAllocs;

  if (numEntries != 0) 
  {
    numAllocEntries = numEntries;
  }

  /* Enable osapiDebugMallocDetail() and setup memory */
  if( (state == L7_TRUE) && (osapiDebugMallocDetailBuffer == L7_NULLPTR))
  {
    osapiDebugMallocDetailBuffer = osapiMalloc(L7_OSAPI_COMPONENT_ID,
                                               numAllocEntries * sizeof(osapiMemAllocType));
    osapiDebugMallocDetailNumAllocs = numAllocEntries;
  }
  /* Disable osapiDebugMallocDetail() and free allocated memory */
  else if( (state == L7_FALSE) && (osapiDebugMallocDetailBuffer != L7_NULLPTR))
  {
    osapiFree(L7_OSAPI_COMPONENT_ID, osapiDebugMallocDetailBuffer);
    osapiDebugMallocDetailBuffer = L7_NULLPTR;
    osapiDebugMallocDetailNumAllocs = OSAPI_DEBUG_MALLOC_DETAIL_NUM_ALLOCS;
  }

}

/**************************************************************************
*
* @purpose  Display malloc allocation details
*
* @param    comp_id - For which component to report. 0 = All components.
* @param    size - Report blocks greater than this size.
* @param    time_stamp - Report blocks with time stamp greater than this value.
* @param    allocationsRequested - Report this number of allocations, if 0 
*                                  all allocations are displayed.
*
* @returns  none
*
* @comments We do lock memory while looking for buffers,
*           so the code will not crash. Use this in production.
*
* @end
*
*************************************************************************/
void osapiDebugMallocDetail(L7_uint32 comp_id,
                           L7_uint32 size,
                           L7_uint32 time_stamp,
                           L7_uint32 allocationsRequested)
{
  FILE *fp = stdout;

  if(osapiDebugMallocDetailBuffer == L7_NULLPTR)
  {
    printf("RealTime osapiDebugMallocDetail() is not enabled !\n");
    printf("Use osapiDebugMallocDetailEnable(1 or 0) to enable and disable.\n");
    return;
  } 

  osapiMallocDetailRecord(fp, osapiDebugMallocDetailBuffer, 
                          osapiDebugMallocDetailNumAllocs, comp_id, size, 
                          time_stamp, allocationsRequested);
                               
}

#endif  /* OSAPI_MEM_LIST */

#ifdef OSAPI_MEM_DEBUG
void osapi_heap_corruption_dump(osapiMemAllocType *mem, int beginning)
{
  L7_uint32 *dump_start, *dump_end;
  int i;
  L7_uchar8 *c, *a;
  L7_uchar8 file_name[OSAPI_TRACK_FILE_NAME_SIZE];
  L7_uint32 line_number = 0;
  L7_uint32 time_stamp = 0;
  osapiMemAllocType *prev_blk = NULL;

  memset(file_name, 0, sizeof(file_name));
  c = (L7_uchar8 *)mem;
  while (((L7_uint32)c)%4 != 0) {
    c--;
  }
  dump_start = (L7_uint32 *)c;
  dump_end = dump_start + 1;
  /* Go back and forward 2 blocks if possible, but at least 128 bytes, 
     and at most 1KB */
  i = 0;
  while (i < 3) 
  {
    while (*dump_start != OSAPI_MEM_START) 
    {
      dump_start--;
    }
    if ((beginning && (i == 1)) || ((!beginning) && (i == 0))) {
      /* Interesting block - get its info */
      prev_blk = (osapiMemAllocType *)dump_start;
      memcpy(file_name, prev_blk->file_name, sizeof(file_name));
      file_name[sizeof(file_name) - 1] = 0;
      line_number = prev_blk->line_number;
      time_stamp = prev_blk->time_stamp;
    }
    i++;
  }
  i = 0;
  while (i < 3) 
  {
    while (*dump_end != OSAPI_MEM_END) 
    {
      dump_end++;
    }
  i++;
  }
  if ((c - ((L7_uchar8 *)dump_start)) < 128) {
    dump_start = (L7_uint32 *)c - 32;
  }
  if ((((L7_uchar8 *)dump_end) - c) < 128) {
    dump_end = (L7_uint32 *)c + 32;
  }
  if ((c - ((L7_uchar8 *)dump_start)) > 1024) {
    dump_start = (L7_uint32 *)c - 256;
  }
  if ((((L7_uchar8 *)dump_end) - c) > 1024) {
    dump_end = (L7_uint32 *)c + 256;
  }

  printf("Dumping heap from 0x%08x to 0x%08x. Block being freed = 0x%08x\n",
         (L7_uint32) dump_start, (L7_uint32) dump_end, (L7_uint32) mem);
  if (prev_blk != NULL) {
    printf("%sorrupt block allocated at time %d, from %s line %d\n",
	   beginning ? "Block before c" : "C", 
	   time_stamp, file_name, line_number);
  }

  c = (L7_uchar8 *)dump_start;
  i = 0;
  while (c < ((L7_uchar8 *)dump_end)) 
  {
    if (i % 16 == 0) 
    {
      printf("%08x: ", (L7_uint32) c);
    }
    printf("%02x ", *c);
    c++;
    i++;
    if (i % 16 == 0) 
    {
      printf("   ");
      for(a = c - 16; a < c; a++) 
      {
	if (isprint(*a)) 
	{
          printf("%c", *a);
        } else 
	{
          printf(".");
        }
      }
      printf("\n");
    }
  }
  printf("\n\n");
  fflush(stdout);
  sleep(5);
}

void osapiDebugCorruptHeap(int begin) 
{
  char *mem = osapiMalloc(L7_OSAPI_COMPONENT_ID, 15);

  if (begin) {
    mem -= 64;
  }
  sprintf(mem, "The quick brown fox jumped over the lazy dog.\n");
  osapiFree(L7_OSAPI_COMPONENT_ID, mem);
  printf("Heap corruption test finished.\n");
}

void changeMallocDebug(int enable, int compId, int numBytes)
{
  timMemDebug = enable;
  debugCompId = compId;
  debugNumBytes = numBytes;
}

#endif /* OSAPI_MEM_DEBUG */


/**************************************************************************
*
* @purpose  Free memory
*
* @param    compId @b{(input)}   The component Id from L7_COMPONENT_IDS_t
* @param    memory @b{(input)}   Pointer to the memory to free.
*
* @returns  none.
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapiFree(L7_COMPONENT_IDS_t compId, void * memory)
{

  pthread_t self;

  self = pthread_self();
  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&threadsafe_lock);
  pthread_mutex_lock(&threadsafe_lock);

  if (compId >= L7_LAST_COMPONENT_ID)
  {
    LOG_ERROR (compId);
  }

  if (memory == 0)
  {
    LOG_ERROR(0);
  }

  {
#ifdef OSAPI_MEM_DEBUG
  L7_osapiMallocUsage_t *pMallocUsage = &osapiMallocMemUsage[compId];
  osapiMemAllocType *mem_alloc;
  L7_uint32 mem_size_word;

  mem_alloc = (osapiMemAllocType *) ((L7_uchar8 *) memory -
                                     (sizeof (osapiMemAllocType) - sizeof (mem_alloc->user_data)));
  if (osapiTestMemory != 0)
  {
    if (mem_alloc->data_start != OSAPI_MEM_START )
    {
      printf("*** Start-of-block heap corruption found\n");
      osapi_heap_corruption_dump(mem_alloc, 1);
      LOG_ERROR ((L7_uint32) memory);
    }

    mem_size_word = (mem_alloc->length + 3) / 4;

    if (mem_alloc->user_data[mem_size_word] != OSAPI_MEM_END)
    {
      printf("*** End-of-block heap corruption found\n");
      osapi_heap_corruption_dump(mem_alloc, 0);
      LOG_ERROR ((L7_uint32) memory);
    }
    if (mem_alloc->in_use == 0)
    {
      printf("*** Free of freed block\n");
      printf("Block had been allocated by %s line %d at time %d\n",
             mem_alloc->file_name, mem_alloc->line_number,
             mem_alloc->time_stamp);
      LOG_ERROR(0);
    }
  }

#ifdef OSAPI_MEM_LIST
  if (allocMemList == mem_alloc)
  {
    allocMemList = mem_alloc->next;
  }

  if (mem_alloc->next != 0)
  {
    mem_alloc->next->prev = mem_alloc->prev;
  }

  if (mem_alloc->prev != 0)
  {
    mem_alloc->prev->next = mem_alloc->next;
  }
#endif /* OSAPI_MEM_LIST */

  mem_alloc->in_use = 0;

  /* We are going to free the memory, decrement the counter for current memory used by
  ** the component
  */
  compId = mem_alloc->component_id;
  pMallocUsage->currentMemUsage = pMallocUsage->currentMemUsage - mem_alloc->length;
  pMallocUsage->countFree++;

  free( mem_alloc );

#else  /* This is the free without debugging */
  free( memory );
#endif /* OSAPI_MEM_DEBUG */
  }

  pthread_cleanup_pop(1);
}

/**************************************************************************
*
* @purpose  Allocate numberofbytes of memory.
*
* @param    numberofbytes  @b{(input)}  number of bytes to allocate.
* @param    compId         @b{(input)}  the component Id from L7_COMPONENT_IDS_t
* @param    file_name      Name of the file invoking this function.
* @param    line_number    Line number in the invoking file.
*
* @returns  pointer to memory allocated or L7_NULLPTR if memory could not be allocated.
*
* @comments
*
* @end
*
*************************************************************************/
void *osapiMalloc_track_try( L7_uint32 numberofbytes, 
                         L7_COMPONENT_IDS_t compId, 
                         L7_char8 *file_name, 
                         L7_uint32 line_number )
{

  pthread_t self;
  void *mem_address = NULL;

  self = pthread_self();

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&threadsafe_lock);
  pthread_mutex_lock(&threadsafe_lock);


  {
#ifdef OSAPI_MEM_DEBUG
  void *address;
  L7_uint32 mem_size_word;
  osapiMemAllocType *mem_alloc;
  L7_osapiMallocUsage_t *pMallocUsage = &osapiMallocMemUsage[compId];

  l7utilsFilenameStrip(&file_name);

  if ((timMemDebug) && ((compId == 0) || (compId == debugCompId)) && 
  ((numberofbytes == 0) || (numberofbytes == debugNumBytes)))
  {
    printf("%s, %d, numberofbytes=%d, filename=%s, line=%d, compId=%d\n", 
      __FUNCTION__, __LINE__, numberofbytes, file_name, line_number, compId);
  }

  if (compId >= L7_LAST_COMPONENT_ID)
  {
    LOG_ERROR (compId);
  }

  mem_size_word = (numberofbytes + 3) / 4;

  address = (void *)malloc( (mem_size_word * 4) + sizeof (osapiMemAllocType));
  if ( address != NULL )
  {
    mem_alloc = (osapiMemAllocType *) address;
    mem_alloc->data_start = OSAPI_MEM_START;
    mem_alloc->length = numberofbytes;
    mem_alloc->task_id = osapiTaskIdSelf ();
    mem_alloc->in_use = 1;
    mem_alloc->component_id = compId;
    mem_alloc->user_data[mem_size_word] = OSAPI_MEM_END;

    (void)memset(&mem_alloc->user_data[0], 0, (mem_size_word * 4));

    /* Memory has been allocated successfully, increment the value of current memory used
    ** by the component, also change the value of maximum memory used by the component
    ** if required
    **/
    pMallocUsage->countAlloc++;
    pMallocUsage->currentMemUsage = pMallocUsage->currentMemUsage + numberofbytes;
    pMallocUsage->maxMemUsage = max(pMallocUsage->maxMemUsage, pMallocUsage->currentMemUsage);

#ifdef OSAPI_MEM_LIST
    mem_alloc->line_number = line_number;
    strncpy (mem_alloc->file_name, file_name, OSAPI_TRACK_FILE_NAME_SIZE);
    mem_alloc->file_name[OSAPI_TRACK_FILE_NAME_SIZE - 1] = 0;

    mem_alloc->time_stamp = osapiUpTimeRaw();

    if (allocMemList != 0)
    {
      allocMemList->prev = mem_alloc;
    }
    mem_alloc->next = allocMemList;
    mem_alloc->prev = 0;
    allocMemList = mem_alloc;

    min_mem_alloc = min(min_mem_alloc, (L7_uint32)mem_alloc);
    max_mem_alloc = max(max_mem_alloc, 
		    ((L7_uint32)mem_alloc + 
		    (mem_size_word * 4) + 
		    sizeof (osapiMemAllocType) + 4));   

#endif  /* OSAPI_MEM_LIST */

    mem_address = &mem_alloc->user_data[0];
  } 
#else  /* This code is executed when debugging is disabled. */
  mem_address = (void *)malloc( numberofbytes );

  if ( mem_address != NULL )
  {
    (void)memset(mem_address, 0, numberofbytes);
  } 

#endif /* OSAPI_MEM_DEBUG */
}

  pthread_cleanup_pop(1);

  return(mem_address);
}

/**************************************************************************
*
* @purpose  Allocate nbytes of memory non fatal
*
* @param    compId    @b{(input)}  the component Id from L7_COMPONENT_IDS_t
* @param    nbytes    @b{(input)}  number of bytes to allocate.
*
* @returns  pointer to memory allocated.
*
* @comments  Under VxWorks this function is implemented as a macro that
*            passes the file name and line number to the memory allocation
*            function. The file and line information is used for debugging
*            memory corruption or memory loss problems.
*
* @end
*
*************************************************************************/
void *osapiMalloc_track( L7_uint32 numberofbytes,
                         L7_COMPONENT_IDS_t compId,
                         L7_char8 *file_name,
                         L7_uint32 line_number )
{
  void *ret = osapiMalloc_track_try(numberofbytes, compId, file_name, line_number);
  if(ret == L7_NULLPTR)
  {
    log_error_code (numberofbytes, file_name, line_number);
  }
  return ret;
}

/**************************************************************************
*
* @purpose  Reallocate numberofbytes of memory.
*
* @param    pBlock         @b{(input)}  memory to reallocate.
* @param    numberofbytes  @b{(input)}  number of bytes to reallocate.
* @param    compId         @b{(input)}  the component Id from L7_COMPONENT_IDS_t
* @param    file_name      Name of the file invoking this function.
* @param    line_number    Line number in the invoking file.
*
* @returns  pointer to memory reallocated or L7_NULLPTR if memory could not be reallocated.
*
* @comments
*
* @end
*
*************************************************************************/
void *osapiRealloc_track(void *pBlock,
                         L7_uint32 numberofbytes,
                         L7_COMPONENT_IDS_t compId,
                         L7_char8 *file_name,
                         L7_uint32 line_number)
{
    pthread_t self;
    void *address;

    self = pthread_self();

    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&threadsafe_lock);
    pthread_mutex_lock(&threadsafe_lock);

#ifdef OSAPI_MEM_DEBUG
  {
  L7_uint32 mem_size_word;
  osapiMemAllocType *old_alloc = NULL;
  L7_uint32 old_size = 0;
  osapiMemAllocType *mem_alloc;
  L7_osapiMallocUsage_t *pMallocUsage = &osapiMallocMemUsage[compId];

  l7utilsFilenameStrip(&file_name);

  if (pBlock != NULL)
  {
      old_alloc = (osapiMemAllocType *) ((L7_uchar8 *) pBlock -
                                         (sizeof (osapiMemAllocType) - sizeof (L7_uint32)));
      old_size = old_alloc->length;

      if (compId != old_alloc->component_id)
      {
          LOG_ERROR (compId);
      }
#ifdef OSAPI_MEM_LIST
    {
        if (allocMemList == old_alloc)
        {
          allocMemList = old_alloc->next;
        }

        if (old_alloc->next != 0)
        {
          old_alloc->next->prev = old_alloc->prev;
        }

        if (old_alloc->prev != 0)
        {
          old_alloc->prev->next = old_alloc->next;
        }
    }
#endif /* OSAPI_MEM_LIST */

  }

  if (compId >= L7_LAST_COMPONENT_ID)
  {
    LOG_ERROR (compId);
  }

  mem_size_word = (numberofbytes + 3) / 4;

  address = (void *)realloc(old_alloc, (mem_size_word * 4) + sizeof (osapiMemAllocType));

  if ( address != NULL )
  {
    mem_alloc = (osapiMemAllocType *) address;
    mem_alloc->data_start = OSAPI_MEM_START;
    mem_alloc->length = numberofbytes;
    mem_alloc->task_id = osapiTaskIdSelf ();
    mem_alloc->in_use = 1;
    mem_alloc->component_id = compId;
    mem_alloc->user_data[mem_size_word] = OSAPI_MEM_END;
    address = (void *) &mem_alloc->user_data[0];

    if (old_alloc == NULL)
    {
        (void)memset(&mem_alloc->user_data[0], 0, (mem_size_word * 4));
    }
    else
    {
        (void)memset(((L7_uchar8 *)(&mem_alloc->user_data[0]))+old_size, 0,
                     ((mem_size_word * 4) - old_size));
    }


    /* Memory has been allocated successfully, increment the value of current memory used
    ** by the component, also change the value of maximum memory used by the component
    ** if required
    **/
      pMallocUsage->countAlloc++;
      pMallocUsage->currentMemUsage = pMallocUsage->currentMemUsage + numberofbytes-old_size;
      pMallocUsage->maxMemUsage = max(pMallocUsage->maxMemUsage, pMallocUsage->currentMemUsage);
    }
  else
  {
      address = pBlock;
      mem_alloc = old_alloc;
  }

#ifdef OSAPI_MEM_LIST
  if (mem_alloc != NULL)
  {
    mem_alloc->line_number = line_number;
    strncpy (mem_alloc->file_name, file_name, OSAPI_TRACK_FILE_NAME_SIZE);
    mem_alloc->file_name[OSAPI_TRACK_FILE_NAME_SIZE - 1] = 0;

    mem_alloc->time_stamp = osapiUpTimeRaw();

    if (allocMemList != 0)
    {
      allocMemList->prev = mem_alloc;
    }
    mem_alloc->next = allocMemList;
    mem_alloc->prev = 0;
    allocMemList = mem_alloc;
  }
#endif  /* OSAPI_MEM_LIST */
  }
#else  /* This code is executed when debugging is disabled. */
  address = (void *)realloc( pBlock, numberofbytes );
#endif /* OSAPI_MEM_DEBUG */

  pthread_cleanup_pop(1);

  return address;
}

/**************************************************************************
*
* @purpose  Allocate numberofbytes of memory on a given boundary.
*
* @param    compId     @b{(input)} The component Id from L7_COMPONENT_IDS_t
* @param    alignment  @b{(input)} boundary to align the allocate - power of 2.
* @param    size       @b{(input)} number of bytes to allocate.
*
* @returns  pointer to memory allocated or L7_NULLPTR if memory could not be allocated.
*
* @comments    none.
*
* @end
*
*************************************************************************/
void *osapiMalloc_aligned(L7_COMPONENT_IDS_t compId, 
                          L7_uint32 alignment, 
                          L7_uint32 size)
{

#if 1  /* ALT - This function should be depricated. */
	LOG_ERROR (size);
#endif
  /* Not sure how to do this in a way that makes the returned pointer
     freeable.  Should only be needed for drivers which will need to
     be reimplemented for Linux anyway.  Just use osapiMalloc instead... */

  return(osapiMalloc(compId, size));

}


/**************************************************************************
*
* @purpose     Save the Serial Port's baud rate
*
* @returns     L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapi_nvstore_save(void)
{
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  struct termios tio;
  speed_t speed;
  int baudrate;
#if !defined(BOOTENV_SUPPORT)
  FILE *baudrate_cfg;
#else
  char cmd[80];
#endif

  tcgetattr(0, &tio);

  speed = cfgetospeed(&tio);

  switch (speed)
  {

  case B1200:
    baudrate = 1200;
    break;

  case B2400:
    baudrate = 2400;
    break;

  case B4800:
    baudrate = 4800;
    break;

  case B9600:
    baudrate = 9600;
    break;

  case B19200:
    baudrate = 19200;
    break;

  case B38400:
    baudrate = 38400;
    break;

  case B57600:
    baudrate = 57600;
    break;

  case B115200:
    baudrate = 115200;
    break;

  default:
    return L7_ERROR;

  }

#if !defined(BOOTENV_SUPPORT)
  if ((baudrate_cfg = fopen(CONFIG_PATH "/serial_speed.pending", "w+"))
   != NULL) {

    fprintf(baudrate_cfg, "%d", baudrate);

    fclose(baudrate_cfg);

  }
#else
  sprintf(cmd, "bootenv -f -s baudrate %d >/dev/null 2>&1\n", baudrate);
  if (system(cmd) == -1) {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
            "Could not save baudrate to boot environment...");
  }
#endif
#endif
  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose     Register w/ nvStore to save the Serial Port's baud rate
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapi_nvstore_register(void)
{
  nvStoreFunctionList_t nvStoreList;

  memset(&nvStoreList, 0, sizeof(nvStoreList));
  nvStoreList.registrar_ID = L7_OSAPI_COMPONENT_ID;
  nvStoreList.notifySave = osapi_nvstore_save;
  nvStoreList.hasDataChanged = L7_NULLPTR;
  nvStoreList.resetDataChanged = L7_NULLPTR;
  nvStoreList.notifyConfigDump = L7_NULLPTR;
  nvStoreList.notifyDebugDump = L7_NULLPTR;

  if (nvStoreRegister(nvStoreList) != L7_SUCCESS) {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
            "nvStoreRegister failed!");
  }

  return;
}

/**************************************************************************
*
* @purpose     Set the Serial Port's baud rate
*
* @param       rate  @b{(input)}
* @param       waitForFlush @b{(input)} Delay for flush to complete.
*
* @returns     L7_SUCCESS if rate is valid and could be set
* @returns     L7_ERROR
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiBaudRateChange(L7_uint32 rate, L7_BOOL waitForFlush)
{
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  struct termios tio;
  speed_t speed;

  (void)pthread_once(&nvstore_once, osapi_nvstore_register); /* always 0 */

  tcgetattr(0, &tio);

  switch (rate)
  {

  case L7_BAUDRATE_1200:
    speed = B1200;
    break;

  case L7_BAUDRATE_2400:
    speed = B2400;
    break;

  case L7_BAUDRATE_4800:
    speed = B4800;
    break;

  case L7_BAUDRATE_9600:
    speed = B9600;
    break;

  case L7_BAUDRATE_19200:
    speed = B19200;
    break;

  case L7_BAUDRATE_38400:
    speed = B38400;
    break;

  case L7_BAUDRATE_57600:
    speed = B57600;
    break;

  case L7_BAUDRATE_115200:
    speed = B115200;
    break;

  default:
    return L7_ERROR;

  }

  cfsetospeed(&tio, speed);

  tcsetattr(0, TCSANOW, &tio);
#endif
  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  convert raw uptime into number of days, hours, minutes, seconds
*
* @param    rawTime @b{(input)}  raw time value
* @param    ts @b{(input)}    ptr to L7_timespec structure
*
* @returns  void
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapiConvertRawUpTime(L7_uint32 rawTime, L7_timespec *ts)
{

  ts->days    = rawTime / SECONDS_PER_DAY;
  rawTime     = rawTime % SECONDS_PER_DAY;
  ts->hours   = rawTime / SECONDS_PER_HOUR;
  rawTime     = rawTime % SECONDS_PER_HOUR;
  ts->minutes = rawTime / SECONDS_PER_MINUTE;
  rawTime     = rawTime % SECONDS_PER_MINUTE;
  ts->seconds = rawTime;

}

/**************************************************************************
*
* @purpose  Retrieve number of seconds since last reset
*
* @param    void
*
* @returns  rawtime
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_uint32 osapiUpTimeRaw(void)
{
  struct timespec tp;
  int rc;
  static L7_uint32 beginningOfTime = 0;

  rc = clock_gettime(CLOCK_MONOTONIC, &tp);
  if (rc < 0)
  {
    struct timeval tv;
    memset (&tv, 0, sizeof (tv));
    gettimeofday(&tv,0);

    if (beginningOfTime == 0)
    {
      beginningOfTime = tv.tv_sec;
    }
    return (tv.tv_sec - beginningOfTime);
    /*return(0);*/
  }
  return(tp.tv_sec);
}

/**************************************************************************
*
* @purpose  Retrieve number of seconds since last reset
*
* @param    void
*
* @returns  rawtime in milliseconds
*
* @comments
*
* @end
*
*************************************************************************/
L7_uint32 osapiUpTimeMillisecondsGet ( void )
{
  int rc;
  struct timespec tp;

  rc = clock_gettime(CLOCK_MONOTONIC, &tp);
  if (rc < 0)
  {
    return(0);
  }
  return(( tp.tv_sec * 1000 ) + ( tp.tv_nsec / 1000000 ));

}

/**************************************************************************
*
* @purpose  Retrieve number of milliseconds
*
* @param    void
*
* @returns  milliseconds since last reset
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint32 osapiTimeMillisecondsGet( void )
{
  struct timespec tm;
  struct timeval tv;
  memset (&tv, 0, sizeof (tv));
  gettimeofday(&tv,0);

  /* Return the time in a TIMESPEC structure. */
  memset (&tm, 0, sizeof (tm));
  tm.tv_sec = tv.tv_sec ;
  tm.tv_nsec = tv.tv_usec * 1000;

  return(( tm.tv_sec * 1000 ) + ( tm.tv_nsec / 1000000 ));
}

/**************************************************************************
*
* @purpose  Retrieve number of microseconds
*
* @param    void
*
* @returns  microseconds since last reset (maximum is 1000.000.000 microseconds)
*
* @notes
*
* @end
*
*************************************************************************/
L7_uint64 osapiTimeMicrosecondsGet( void )
{
  struct timeval tv;
  memset (&tv, 0, sizeof (tv));
  gettimeofday(&tv,0);

  return(( (L7_uint64) tv.tv_sec * 1000000ULL ) + (L7_uint64) tv.tv_usec );
  //return ( times(L7_NULLPTR)*(1000000/sysconf(_SC_CLK_TCK)) );
}

/**************************************************************************
*
* @purpose  Retrieve number of seconds and nanoseconds since last reset
*
* @param    ct ptr to place values
*
* @returns  void
*
* @notes
*
* @end
*
*************************************************************************/
void osapiClockTimeRaw ( L7_clocktime *ct )
{

  struct timespec tm;
  struct timeval tv;
  memset (&tv, 0, sizeof (tv));
  gettimeofday(&tv,0);

  /* Return the time in a TIMESPEC structure. */
  memset (&tm, 0, sizeof (tm));
  tm.tv_sec = tv.tv_sec ;
  tm.tv_nsec = tv.tv_usec * 1000 ;

  ct->seconds = tm.tv_sec;
  ct->nanoseconds = tm.tv_nsec;
  return;

}



/**************************************************************************
*
* @purpose  Retrieve number of seconds since last reset and return in number
*           of days, hours, minutes, seconds
*
* @param    ts @b{(input)}    ptr to L7_timespec structure
*
* @returns  void
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapiUpTime(L7_timespec *ts)
{

  L7_uint32 rawTime;

  rawTime = osapiUpTimeRaw();

  osapiConvertRawUpTime(rawTime, ts);

}

/**************************************************************************
*
* @purpose  Converts a broken-down time structure,  expressed as localtime
*            , to calendar time representation.
*
* @param    lt @b{(input)}    ptr to L7_localtime structure
*
* @returns  seconds since the  epoch 
*           -1 if the specified broken-down time cannot be 
*              represented as calendar time
*
* @comments The function ignores the specified contents of the structure 
*           members L7_yday and L7_wday and recomputes  them  from  the  other  
*           information in the broken-down time structure. If structure members 
*           are  outside their legal interval, they will  be normalized 
*           (so that, e.g., 40 October is changed into 9 November). This also
*           also sets the  external  variable  tzname  with information  
*           about the current time zone.
*
* @end
*
*************************************************************************/
L7_uint32 osapiMkTime(L7_localtime *lt)
{
  L7_uint32 seconds;
  struct tm usrTmStruct;

  usrTmStruct.tm_sec   = lt->L7_sec;   /* seconds after the minute - [0, 59] */
  usrTmStruct.tm_min   = lt->L7_min;   /* minutes after the hour   - [0, 59] */
  usrTmStruct.tm_hour  = lt->L7_hour;  /* hours after midnight     - [0, 23] */
  usrTmStruct.tm_mday  = lt->L7_mday;  /* day of the month         - [1, 31] */
  usrTmStruct.tm_mon   = lt->L7_mon;   /* months since January     - [0, 11] */
  usrTmStruct.tm_year  = lt->L7_year;  /* years since 1900     */
  usrTmStruct.tm_isdst = lt->L7_isdst; /* Daylight Saving Time flag */

  seconds =  (L7_uint32) (mktime(&usrTmStruct));
  lt->L7_wday = usrTmStruct.tm_wday; /* day of the week       - [0, 6] */
  lt->L7_yday = usrTmStruct.tm_yday; /* day of the year     - [0, 365] */
  return seconds;
}

/**************************************************************************
*
* @purpose  Converts the calendar time to broken-time representation
*
* @param    seconds @b{(input)}    seconds since epoch
* @param    lt      @b{(output)}   ptr to L7_localtime structure
*
* @returns  L7_SUCCESS upon successful conversion
*           L7_FAULURE upon error
*
* @comments The function ignores the specified contents of the structure
*
* @end
*
*************************************************************************/
L7_RC_t osapiLocalTime(L7_uint32 seconds, L7_localtime *lt)
{
  time_t     stdSecs;
  struct tm  usrTmStruct;
  L7_RC_t    rc = L7_FAILURE;
  
  stdSecs = (time_t)seconds;
  if (localtime_r(&stdSecs, &usrTmStruct) != NULL)
  {
    lt->L7_sec   = usrTmStruct.tm_sec;   /* seconds after the minute-[0, 59] */
    lt->L7_min   = usrTmStruct.tm_min;   /* minutes after the hour - [0, 59] */
    lt->L7_hour  = usrTmStruct.tm_hour;  /* hours after midnight   - [0, 23] */
    lt->L7_mday  = usrTmStruct.tm_mday;  /* day of the month       - [1, 31] */
    lt->L7_mon   = usrTmStruct.tm_mon;   /* months since January   - [0, 11] */
    lt->L7_year  = usrTmStruct.tm_year;  /* years since 1900     */
    lt->L7_isdst = usrTmStruct.tm_isdst; /* Daylight Saving Time flag */
    lt->L7_wday = usrTmStruct.tm_wday;   /* day of the week     - [0, 6] */
    lt->L7_yday = usrTmStruct.tm_yday;   /* day of the year     - [0, 365] */
    rc = L7_SUCCESS;
  }
  return rc;
}

/* This value represents how much the local clock is
 * off from UTC (should be implemented in osapi)
 */
static L7_int32 localClockError = 0;

/**********************************************************************************
* @purpose  Set the clock-time to zero.
*
* @returns  none
*
* @notes    used when setting the default config data.
*
* @end
*************************************************************************************/

void osapiSetTimeToDefault()
{
struct timespec   tp;

 tp.tv_sec  = 0;
 tp.tv_nsec = 0;
 clock_settime(CLOCK_REALTIME, &tp);
 localClockError=0;
}



/**********************************************************************
* @purpose  Get the current UTC time since the Unix Epoch.
*
* @param    ct @b{(output)} UTC time
*
* @returns  L7_SUCCESS
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/


L7_RC_t osapiUTCTimeGet(L7_clocktime * ct)
{
  osapiClockTimeRaw(ct);
  ct->seconds += (L7_uint32)localClockError;
  ct->nanoseconds += (L7_uint32)((localClockError -
            (L7_uint32)localClockError) * 1000000000);
  while (ct->nanoseconds >= 1000000000)
  {
    ct->seconds++;
    ct->nanoseconds -= 1000000000;
  }
  return L7_SUCCESS;
}


/**********************************************************************
* @purpose  Get the current NTP time since the NTP Epoch.
*
* @param    none
*
* @returns  seconds since NTP epoch as a double.
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/
L7_int64 osapiNTPTimeGet()
{
  L7_clocktime current;
  osapiClockTimeRaw(&current);

  /* Jan 1970 in NTP seconds + UTC seconds till uptime + current uptime = NTC now */
  return (JAN_1970_OFFSET
          + (L7_int64)current.seconds
          + ((L7_int64)(current.nanoseconds) / 1000000000)
          + (L7_int64)localClockError);
}


/**********************************************************************
* @purpose  Update the local clock with the correction
*
* @param    offset @b{(input)} the correction to be applied.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t osapiNTPTimeSet(L7_int32 offset)
{
  localClockError += offset;
  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Set the local clock to a specific date/time.
*
* @param    year @b{(input)}    year portion of the date
* @param    month @b{(input)}   month portion of the date
* @param    day @b{(input)}     day portion of the date
* @param    hour @b{(input)}    hour portion of the date
* @param    minute @b{(input)}  minute portion of the date
* @param    second @b{(input)}  second portion of the date
*
* @returns  L7_SUCCESS if the correction was applied.
*
* @notes    This routine uses the date and time provided to determine the
*           correct time correction to apply to the clock.
*
* @end
*********************************************************************/

L7_RC_t osapiNTPTimeSetRaw(int year, int month, int day, int hour, int minute, int second)
{
  /** Our processing time is the difference between the start
  * time and time now.
  */

  time_t starttime;
  time_t newtime;
  struct tm * timeinfo;

  time(&starttime);

  timeinfo = localtime(&starttime);

  timeinfo->tm_year = year-1900;
  timeinfo->tm_mon = month-1;
  timeinfo->tm_mday = day;
  timeinfo->tm_hour = hour;
  timeinfo->tm_min = minute;
  timeinfo->tm_sec = second;

  newtime = mktime(timeinfo);


  localClockError = (L7_int32)(newtime - starttime);

  return L7_SUCCESS;
}

/**********************************************************************
* @purpose  Get the current synchronized UTC time since the Unix Epoch.
*
* @returns  seconds since Unix epoch.
*
* @notes    This is a utility routine for SNTP internal timestamping.
*           Note that NTP epoch is different from Unix epoch. This
*           routine attempts not to return negative time if the clock
*           has not been synchronized at least once.
*
* @end
*********************************************************************/
L7_uint32 osapiUTCTimeNow()
{
  L7_clocktime current;

  (void)osapiUTCTimeGet(&current);

  return current.seconds;
}

/**********************************************************************
* @purpose  Convert sysUpTime in seconds to utcTime in seconds.
*
* @returns
*
* @notes    This is needed for certain MIB objects that are kept
*           as time ticks based on the system uptime. The localClockError
*           will be applied to determine the utcTime.
*
* @end
*********************************************************************/
void osapiRawTimeToUTCTime(L7_uint32 rawTime, L7_uint32 *utcTime)
{
  L7_clocktime current;

  current.seconds = rawTime;
  current.nanoseconds = 0;

  current.seconds += (L7_uint32)localClockError;
  current.nanoseconds += (L7_uint32)((localClockError -
                                      (L7_uint32)localClockError) * 1000000000);
  while (current.nanoseconds >= 1000000000)
  {
    current.seconds++;
    current.nanoseconds -= 1000000000;
  }

  *utcTime = current.seconds;

  return;
}

/* End of SNTP */

/**************************************************************************
*
* @purpose  manipulates the underlying device parameters of special files
*
* @param    fd        @b{(input)}  File descriptor
* @param    function  @b{(input)}  device-dependent request
* @param    arg       @b{(input)}  value to set

* @returns     none
*
* @comments    none
*
* @end
*
*************************************************************************/
L7_RC_t osapiIoctl ( L7_int32 fd, L7_int32 function, L7_int32 arg )
{
  if ( ioctl ( fd, function, arg))
  {
    return L7_ERROR;
  }
  return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Helper function to execute a command
*
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ...         @b{(input)}  variable arguments
*
* @returns  exit code of the command
*
* @end
*
*************************************************************************/
int osapiSystem(const char *fmt, ...)
{
  int rc;
  va_list args;
  char buf[1024];

  va_start(args, fmt);

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&threadsafe_lock);
  pthread_mutex_lock(&threadsafe_lock);

  vsnprintf(buf, sizeof(buf), fmt, args);
  rc = WEXITSTATUS(system(buf));

  pthread_cleanup_pop(1);

  va_end(args);

  return(rc);
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void osapiDebugSysInfoDumpRegister(void)
{
    supportDebugCategory_t supportDebugCategory;

    memset(&supportDebugCategory, 0x00, sizeof(supportDebugCategory));
    supportDebugCategory.componentId = L7_OSAPI_COMPONENT_ID;

    /*-----------------------------*/
    /* Register sysInfo debug dump */
    /*-----------------------------*/
    supportDebugCategory.category = SUPPORT_CATEGORY_SYSTEM;
    supportDebugCategory.info.supportRoutine = osapiDebugRegisteredSysInfoDump;
    (void)strncpy(supportDebugCategory.info.supportRoutineName, "osapiDebugRegisteredSysInfoDump",
            SUPPORT_DEBUG_HELP_NAME_SIZE);
    (void) supportDebugCategoryRegister(supportDebugCategory);
}

/*********************************************************************
* @purpose  Registered support debug dump routine for general system issues
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @notes 
*                                 
* @end
*********************************************************************/
void osapiDebugRegisteredSysInfoDump(void)
{
    char buf[1000];

    sysapiPrintf("\r\n");
    sysapiPrintf("/*=====================================================================*/\n");
    sysapiPrintf("/*                            OS  INFORMATION                          */\n");
    sysapiPrintf("/*=====================================================================*/\n");
    
    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("/* Output of osapiShowTasks():  */\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("\r\n");

    osapiShowTasks();

    memset(buf, 0, sizeof(buf));

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("/* Output of cat /proc/meminfo  */\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("\r\n");

    osapiFsRead("/proc/meminfo",buf,sizeof(buf));

    if(strlen(buf) > 0)
    sysapiPrintf("%s",buf);

    memset(buf, 0, sizeof(buf));

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("/* Output of cat /proc/cpuinfo  */\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("\r\n");

    osapiFsRead("/proc/cpuinfo",buf,sizeof(buf));

    if(strlen(buf) > 0)
    sysapiPrintf("%s",buf);

    memset(buf, 0, sizeof(buf));

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("/* Output of cat /proc/interrupts  */\n");
    sysapiPrintf("/*---------------------------------*/\n");
    sysapiPrintf("\r\n");

    osapiFsRead("/proc/interrupts",buf,sizeof(buf));

    if(strlen(buf) > 0)
    sysapiPrintf("%s",buf);

    memset(buf, 0, sizeof(buf));

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("/* Output of cat /proc/version  */\n");
    sysapiPrintf("/*------------------------------*/\n");
    sysapiPrintf("\r\n");

    osapiFsRead("/proc/version",buf,sizeof(buf));

    if(strlen(buf) > 0)
    sysapiPrintf("%s",buf);

    memset(buf, 0, sizeof(buf));

    sysapiPrintf("\r\n");
    sysapiPrintf("\n");
    sysapiPrintf("/*-----------------------------------*/\n");
    sysapiPrintf("/* Output of cat /proc/net/sockstat  */\n");
    sysapiPrintf("/*-----------------------------------*/\n");
    sysapiPrintf("\r\n");

    osapiFsRead("/proc/net/sockstat",buf,sizeof(buf));

    if(strlen(buf) > 0)
    sysapiPrintf("%s",buf);  

 }

/***************************************************************************
*
* @purpose Setup stdin to forward on every character.
*
* @param    fd        @b{(input)}  File descriptor
*
* @comments Configure stdin to forward every character to emweb.
*
* @returns L7_SUCCESS
*
* @end
*
***************************************************************************/
EXT_API L7_RC_t osapiStdInConfig(int fd)
{
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  struct termios tio;

  memset(&tio, 0, sizeof(tio));

  tcgetattr(fd, &tio);

  tio.c_iflag &= ~(ISTRIP|INLCR|IGNCR|ICRNL);
  tio.c_lflag &= ~(ECHO|ECHONL|ICANON);
  tio.c_cc[VSUSP] = _POSIX_VDISABLE;
  /*
     Defined in man pages but not in .h file
     tio.c_cc[VDSUSP] = _POSIX_VDISABLE;
   */

  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &tio);
#endif
  return L7_SUCCESS;
}

/***************************************************************************
 ** NAME: osapiGetChoice
 **
 ** This function lets the user choose a menu option.
 **
 ** RETURNS: choice
 ***************************************************************************/
L7_uchar8 osapiGetChoice()
{
  L7_char8 string[80];
  L7_char8 choice[80];
  int rc;
  L7_int32 i;
  L7_int32 j=0;

  do
  {
    bzero(string,sizeof(string));
    bzero(choice,sizeof(choice));

    rc = osapiRead(bspapiConsoleFdGet(), string, sizeof(string));
    if ( rc  <= 0 )
    {
      continue;
    }
    if ( string[0] == '?' )
    {
      return 99;
    }
    for ( i = 0; i < (L7_int32)strlen(string); i++ )
    {
      if ( ( string[i] == (L7_char8)0x08 ) && ( j > 0 ) )
      {
        j--; /*backspace*/
      }
      else
      {
        choice[j++]=string[i];
      }
    }
    break;
  } while ( 1 );
  return((L7_uchar8)atoi(choice));

}

#ifdef LVL7_DEBUG_BREAKIN
#define CONSOLE_PIPE "/tmp/consolepipe"
extern void osapiDebugShell(void);
static L7_uint32 consoleBreakinFlag = L7_TRUE;
static L7_char8  breakinString[L7_PASSWORD_SIZE];

/***************************************************************************
*
* @purpose Task responsible for reading data from stdin.
*
* @comments Entry point for task responsible for reading data from stdin.
*
* @returns none
*
* @end
*
***************************************************************************/
void osapiInputTask(void)
{
  static char inputBuffer[BUFSIZ];
  int  debugCnt = 0;
  int  inputSize;
  int  cmpCnt = 0;
  int  shellActive = L7_FALSE;
  int  inputHandle = -1;

  /* stdin is globally redirected to pipe2, then this     */
  /* task pends on stdin and write to the emweb input fd. */
  /* Create the pipe, then open it                        */
  if (osapiPipeCreate (CONSOLE_PIPE, 10, 1024) != L7_FAILURE)
  {
    inputHandle = open(CONSOLE_PIPE, O_RDWR ,0666);
  }

  if (inputHandle >= 0)
  {
    bspapiConsoleFdSet(inputHandle);
  }

  while (L7_TRUE)
  {
    /* If the console FD has been modified suspend reading until it has */
    /* been reset. This is necessary when performing xmodem receive     */
    /* to ensure the debug string is not mistakenly detected within the */
    /* contents of a file.                                              */
    if (bspapiConsoleFdGet() == inputHandle)
    {
      inputSize = osapiRead(STDIN_FILENO, &inputBuffer[0], sizeof(inputBuffer));
      /* Prevent tight loops in the event of read failure */
      if (inputSize < 0) 
      {
        perror("osapiInputTask() stdin read failed");
        sleep(1);
        continue;
      } 
      else if (inputSize == 0) 
      {
        printf("osapiInputTask stdin read of 0 bytes\n");
        sleep(1);
        continue;
      }
      /* The Console FD has been modified while pending on */
      /* the read, discard data so it is not echo'ed back. */
      if (bspapiConsoleFdGet() != inputHandle)
      {
        continue;
      }
      write(inputHandle, &inputBuffer[0], inputSize);
      /* The debug shell was activated, but has exited. Reset */
      /* any parameters needed to resume normal operation.    */
      if (shellActive == L7_TRUE)
      {
        shellActive = L7_FALSE;
      }

      if(consoleBreakinFlag == L7_TRUE)
      {
        cmpCnt = 0;
        while ((cmpCnt < inputSize) && (debugCnt != strlen(breakinString)))
        {
          if (inputBuffer[cmpCnt] == breakinString[debugCnt])
          {
            debugCnt++;
          }
          else
          {
            debugCnt=0;
          }
          cmpCnt++;
        }

        if ( (debugCnt != 0 ) && debugCnt == strlen(breakinString) )
	{
          shellActive = L7_TRUE;
          osapiDebugShell();
          debugCnt=0;
        }
      }
    }
    else
    {
      sleep(1);
    }
  }
  bspapiConsoleFdSet(STDIN_FILENO);
  close(inputHandle);
}

/***********************************************************************
*@purpose  Enable/Disable console.
*
* @param    enable_flag - L7_TRUE.
*                         L7_FALSE.

* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t osapiConsoleBreakinModeSet(L7_BOOL enable_flag)
{
  consoleBreakinFlag = enable_flag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the console break-in  Mode setting for selected session.
*
*
*
* @returns  L7_TRUE -  display of Debug Console Mode Enabled.
*           L7_FALSE - display of Debug Console Mode Disabled.
*
* @notes
*
*
* @end
*********************************************************************/
L7_BOOL osapiConsoleBreakinModeGet(void)
{
  return consoleBreakinFlag;
}

/*********************************************************************
* @purpose  Gets the console breakin string
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiConsoleBreakinstringGet(L7_char8 *password)
{
  if(password == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  strcpy(password, breakinString);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets  console breakin string.
*
* @param    index       table index
* @param    password    pointer to password
*
* @returns  L7_SUCCESS
*           L7_FAILURE if argument passed to this function is a null pointer.
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiConsoleBreakinstringSet(L7_char8 *password)
{
  if(password == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  memset(breakinString, 0x00, sizeof(breakinString));
  osapiStrncpy(breakinString, password, sizeof(breakinString));
  return L7_SUCCESS;
}
#endif
/**************************************************************************
*
* @purpose     Return a string that dumps the CPU utilization
*
* @param       none
* @returns     none
*
* @comments    none
*
* @end
*
*************************************************************************/
L7_RC_t osapiCpuUtilizationStringGet(L7_char8 *cpuUtilBuffer, L7_uint32 bufferSize)
{
  L7_int32 n=0;
  L7_uint32 numBytesTotal;
  L7_uint32 numBytesAlloc;
  L7_uint32 numBytesFree;
  L7_RC_t   rc = L7_SUCCESS;

  if (cpuUtilBuffer == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (osapiGetMemInfo(&numBytesTotal,&numBytesAlloc,&numBytesFree) != L7_SUCCESS)
  {
    n = osapiSnprintf(cpuUtilBuffer, bufferSize,
                      "Unable to retrieve memory statistics\r\n");
  }
  else
  {
  
    n += osapiSnprintf(&cpuUtilBuffer[n],bufferSize-n,"\r\nMemory Utilization Report\r\n\r\n");

    n += osapiSnprintf(&cpuUtilBuffer[n],bufferSize-n,"status      bytes\r\n");
    n += osapiSnprintf(&cpuUtilBuffer[n],bufferSize-n,"------ ----------\r\n");
    n += osapiSnprintf(&cpuUtilBuffer[n],bufferSize-n,"free   %10d\r\n", numBytesFree);
    n += osapiSnprintf(&cpuUtilBuffer[n],bufferSize-n,"alloc  %10d\r\n\r\n", numBytesAlloc);
  }

  n += sysapiTaskUtilPrint(&cpuUtilBuffer[n], bufferSize - n);
  return rc;
}

static L7_uint32 get_dir_usage(char *dirname)
{
  L7_uint32 sum = 0;
  DIR *d;
  struct dirent *de;
  struct stat st;
  L7_uint32 filesize;
  /* Would use malloc but we want this to work when memory is low.
     PATH_MAX seems like overkill. If we're crawling directory structures 
     that have paths >256 we've probably already lost. */
  char path[256];
 
  d = opendir(dirname);
  if (d != NULL)
  {
    while ((de = readdir(d)) != NULL)
    {
      if ((de->d_type == DT_REG) || (de->d_type == DT_UNKNOWN))
      { /* JFFS2 files come up as DT_UNKNOWN */
        snprintf(path, 255, "%s/%s", dirname, de->d_name);
 
        path[255] = '\0';
        if (stat(path, &st) == 0)
        { 
          filesize = st.st_size;
          /* Round up to multiple of 4K */
          filesize += 4095;
          filesize >>= 12;
          filesize <<= 12;
          /* Keep sum in kilobytes */
          sum += (filesize >> 10);
        }
      } else if (de->d_type == DT_DIR)
      {
        if ((dirname[0] == '/') && (dirname[1] == '\0'))
        {
          if ((strcmp(de->d_name, "proc") == 0) ||
              (strcmp(de->d_name, "sys") == 0) ||
              (strcmp(de->d_name, "dev") == 0))
          {
            continue; /* Skip these if in the root dir */
          }
        }
        if ((de->d_name[0] == '.') && 
            ((de->d_name[1] == '\0') || 
             ((de->d_name[1] == '.') && (de->d_name[2] == '\0'))))
        {
          continue; /* Skip . and .. anywhere */
        }
        snprintf(path, 255, "%s/%s", dirname, de->d_name);
        path[255] = '\0';
        sum += get_dir_usage(path);
      }
    }
    closedir(d);
  }
  return sum;
}

L7_RC_t osapiGetMemInfo(L7_uint32 *numBytesTotal, L7_uint32 *numBytesAllocated, L7_uint32 *numBytesFree)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_uint32 memTotal = 0;
  L7_uint32 memFree = 0;
  L7_uint32 cached = 0;
#ifndef PC_LINUX_HOST
  struct statfs sf;
#endif
  int       fd;
  char     *pStr;

  /* Ideally this buffer would be allocated based on the size of */
  /* proc/meminfo, however a static buffer is used on the stack  */
  /* to ensure the command is available when system memory has   */
  /* been depleted.                                              */
#define MAX_FILE_LENGTH 624
  L7_uchar8  buffer[MAX_FILE_LENGTH+1] = {0};

  fd = open("/proc/meminfo",O_RDONLY);
  if (fd == -1) 
  {
    return L7_FAILURE;
  }
  osapiRead(fd, buffer, MAX_FILE_LENGTH);
  close(fd);

  pStr = strstr(buffer, "MemTotal:");
  if (pStr != NULL)
  {
    if (sscanf(pStr,"MemTotal:%u", &memTotal) == 1)
    {
      pStr = strstr(buffer, "MemFree:");
      if (pStr != L7_NULL)
      {
        if (sscanf(pStr,"MemFree:%u", &memFree) == 1)
        { 
          pStr = strstr(buffer, "Cached:");
          if (pStr != NULL)
          {
            if (sscanf(pStr,"Cached:%u", &cached) == 1)
            {
              rc = L7_SUCCESS;
            }
          }
        }
      }
    }
  }

#ifndef PC_LINUX_HOST
  /* Only proceed if /proc/meminfo was parsed successfully */
  if (rc == L7_SUCCESS)
  {
    L7_uint32 fsTotal = 0;
    L7_uint32 jffsTotal = 0;
    /* Calculate the size of the RAM disks:     */
    /* 1. Obtain size of the entire file system */
    fsTotal = get_dir_usage("/");
    /* 2. Obtain size of the jffs2 file system to subtract */
    /*    from total. (leaving rootfs, tmpfs'es)    */
    if (statfs(CONFIG_PATH, &sf) == 0)
    {
      jffsTotal = sf.f_bsize * (sf.f_blocks - sf.f_bfree) / 1024;  
    }

    /* Convert blocks to bytes. */
    memFree = memFree + cached - (fsTotal - jffsTotal);
    *numBytesAllocated = (memTotal - memFree) * (1024) ;
    *numBytesFree = memFree * 1024;
    *numBytesTotal = *numBytesAllocated + *numBytesFree;
  }
#endif
  return rc;
}

/*********************************************************************
* @purpose  Gets the process wide CPU utilization (from cache table)
*
* @param    processIndex
* @param    taskNameString
* @param    taskNameSize
* @param    taskPercentString
* @param    taskPercentSize
*
* @returns L7_SUCCESS if successful
* @returns L7_FAILURE  otherwise
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t osapiSwitchCpuUtilizationGet(L7_uint32 processIndex, 
                                     L7_uchar8 *taskNameString, L7_uint32 taskNameSize,
                                     L7_uchar8 *taskPercentString, L7_uint32 taskPercentSize)
{
  return sysapiTaskSpecificUtilizationGet(processIndex, 
                                          taskNameString, taskNameSize,
                                          taskPercentString, taskPercentSize);
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
L7_RC_t osapiTaskIdGet (L7_uint32 processIndex, L7_char8 *taskIdString,
                        L7_uint32 taskIdSize)
{
  return sysapiTaskIdGet (processIndex, taskIdString, taskIdSize);
}

/*********************************************************************
* @purpose  Get the Total CPU Utilization.
*
* @param    taskPercentString
* @param    taskPercentSize
*
* @returns  
*
* @end
*
*********************************************************************/
L7_RC_t osapiTotalCpuUtilGet (L7_char8 *taskPercentString, L7_uint32 taskPercentSize)
{
  cpuPeriodUtilInfo_t cpuUtil[L7_CPU_UTIL_MEASURE_MAX_PERIODS];
  L7_uint32 n = 0, numEntries, idx;
  
  memset(cpuUtil, 0, sizeof(cpuUtil));

  numEntries = sysapiTotalCpuUtilGet(cpuUtil);

  for (idx = 0; idx < numEntries; idx++)
  {
    n += osapiSnprintf(&taskPercentString[n],taskPercentSize-n,
                       "%5d Secs (%3d.%2.2d%%)",
                       cpuUtil[idx].timePeriod,
                       cpuUtil[idx].expMovingAvg / L7_CPU_UTIL_SCALAR,
                       cpuUtil[idx].expMovingAvg % L7_CPU_UTIL_SCALAR);

  }

  return L7_SUCCESS;
}

void osapiMiscInit(void)
{
  /* Initialize the FastPath monitor */
  osapiMonitorInit(); 

#ifdef LVL7_DEBUG_BREAKIN
/* PTin removed: serial port */
#if (L7_SERIAL_COM_ATTR)
  osapiTaskCreate("serialInput", (void *)osapiInputTask, 0, L7_NULLPTR, 16*1024, 5, 0);
#endif
#endif
}

/**************************************************************************
*
* @purpose     Debug function to set the osapiStackDumpLogMode variable.
*              This variable controls whether the stack trace would be
*              saved to Flash or not when osapiLogStackTraceFlash routine
*              is invoked.
*
* @param       enable/disable 
* @returns     none
*
* @comments    none
*
* @end
*
*************************************************************************/
void osapiDebugSetStackDumpLogMode(L7_BOOL enable)
{
  osapiStackDumpLogMode = enable;
}

extern void osapiDebugStackTrace (L7_uint32 debug_task_id, FILE *filePtr);
/*********************************************************************
* @purpose  Routine to take a snapshot of the task stack
*           traces and write it to the flash. This can be
*           invoked by rest of the system when a fatal error
*           happens and system is going down.
*
* @param     fileName {(input)} Name of the file to write stack trace
* @param     format  {(input)} Format of the output.
* @param     ... - Variable output list.
*
* @returns  none.
* 
* @comments Currently being invoked by NIM_LOG_ERROR. By default,
*           it is turned-off for Linux as running osapiDebugStackTrace
*           takes a long time.
*           Not supported on MIPS cpu as osapiDebugStackTrace is not
*           very reliable on MIPS.
*
* @end
*********************************************************************/
void osapiLogStackTraceFlash(L7_char8 *fileName, L7_char8 * format, ...)
{
#if defined(CPU_FAMILY) && (CPU_FAMILY == PPC)
  FILE          *fp;
  L7_uchar8     swVersion[80];
  L7_int32      currTime = osapiUTCTimeNow();
  L7_uchar8     log_buf[LOG_MSG_MAX_MSG_SIZE];
  va_list       ap;
  L7_int32      rc;


  if (osapiStackDumpLogMode == L7_TRUE) 
  {
    fp = fopen(fileName, "w");
    if (fp == L7_NULL) 
    {
      return;
    }

    memset(swVersion, 0, sizeof(swVersion));
    sysapiRegistryGet(SW_VERSION, STR_ENTRY, (void *) swVersion);

    fprintf(fp, "Software Version: %s\n",swVersion);
    fprintf(fp, "Creation time %s\n\n",
            ctime((void *)&currTime));

	va_start(ap, format);
	rc = osapiVsnprintf(log_buf, sizeof (log_buf), format, ap);
	va_end(ap);

	fprintf(fp, log_buf);
	osapiDebugStackTrace(0x0, fp);
	fclose(fp);
  }
#endif
  return;
}

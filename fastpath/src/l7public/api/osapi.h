/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi.h
*
* @purpose   OS independent prototype declarations
*
* @component osapi
*
* @comments
*
* @create    06/23/2000
*
* @author    paulq
*
* @end
*
*********************************************************************/

#ifndef OSAPIPROTO_HEADER
#define OSAPIPROTO_HEADER

#include <stdarg.h>                     /* CRC_OK: deviation from Guidelines */
#include <time.h>

#include "l7_common.h"
#include "comm_structs.h"
#include "l3_addrdefs.h"
#include "osapi_file.h"
#include "datatypes.h"
#include "l7utils_inet_addr_api.h"
#include "cda_api.h"

/*********************************************************************
*
* @structures L7_timespec_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct L7_timespec_t
{
  L7_uint32 days;
  L7_uint32 hours;
  L7_uint32 minutes;
  L7_uint32 seconds;
}  L7_timespec;

/*********************************************************************
*
* @structures L7_clocktime_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct L7_clocktime_t
{
  L7_uint32 seconds;
  L7_uint32 nanoseconds;
}  L7_clocktime;


/*********************************************************************
*
* @structures L7_osapiMallocUsage_s
*
* @purpose
*
* @comments
*
**********************************************************************/
typedef struct L7_osapiMallocUsage_s
{
  L7_uint32 currentMemUsage;
  L7_uint32 maxMemUsage;
  L7_uint32 countAlloc;
  L7_uint32 countFree;
}  L7_osapiMallocUsage_t;


#define SECONDS_PER_DAY        86400
#define SECONDS_PER_HOUR       3600
#define SECONDS_PER_MINUTE     60


/*********************************************************************
*
* @constants SNTP Time Constants
*
* @purpose
*
* @comments Do NOT adjust
*
*********************************************************************/


/** 1970 - 1900 in seconds */
#define JAN_1970_OFFSET   2208988800UL

#define OSAPI_FREE_MEMORY(buffer)   \
{                                   \
    osapiFree(L7_OSAPI_COMPONENT_ID, buffer);              \
    buffer = L7_NULLPTR;            \
}
extern L7_int32 ttyspeed;
/*********************************************************************
*
* @structures OSAPI_SEM_B_STATE
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef enum
{
  OSAPI_SEM_EMPTY,   /* 0: semaphore not available */
  OSAPI_SEM_FULL     /* 1: semaphore available */
} OSAPI_SEM_B_STATE;

/* This allows us to use a real type, while allowing the implementation
   to vary between operating systems... */
typedef struct osapiRWLock_s {
   void *handle;
} osapiRWLock_t;

typedef enum osapiRWLockOptions_e {
   OSAPI_RWLOCK_Q_FIFO = 0,
   OSAPI_RWLOCK_Q_PRIORITY
} osapiRWLockOptions_t;

#define OSAPI_SEM_Q_FIFO             0x00
#define OSAPI_SEM_Q_PRIORITY         0x01
#define OSAPI_SEM_DELETE_SAFE        0x04
#define OSAPI_SEM_INVERSION_SAFE     0x08

#define OSAPI_SERVPORT_MONITOR_SLEEP 3
#define OSAPI_INET_NTOA_BUF_SIZE     16
#define OSAPI_INET_NTOP_BUF_SIZE     256
#define VXWORKS_LOWEST_PRIORITY      255
#define OSAPI_LINUX_BUFFER_SIZE 1024

/*********************************************************************
*
* @structures OSAPI_SOCKET_LIB_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef enum
{
  OSAPI_SOCKETLIB_INET_BSD,
  OSAPI_SOCKETLIB_ROUTE_BSD
} OSAPI_SOCKET_LIB_t;



typedef enum
{
  L7_INSTRUCTION_CACHE,
  L7_DATA_CACHE
} L7_CACHE_TYPE;

/**************************************************************************
*
* @purpose  Create a binary Semaphore
*
* @param    options         @b{(input)}  queueing style - either Priority style or FIFO style
* @param    initialState    @b{(input)}  FULL(available) or EMPTY (not available)
*
* @returns  ptr to the semaphore or NULL if memory cannot be allocated
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void *osapiSemaBCreate (L7_int32 options, OSAPI_SEM_B_STATE initial_state);

/**************************************************************************
*
* @purpose  create a Mutual Exclusion Semaphore
*
* @param    options  @b{(input)}  queueing style - either Priority style or FIFO style
*
* @returns  ptr to the semaphore or NULL if memory cannot be allocated
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void *osapiSemaMCreate (L7_int32 options);

/**************************************************************************
*
* @purpose  Take a Semaphore
*
* @param    SemaID    @b{(input)}  ID of the requested semaphore returned from the create
* @param    timeout   @b{(input)}  time to wait in milliseconds, forever (-1), or no wait (0)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if timeout or if semaphore does not exist
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t    osapiSemaTake (void *SemaID, L7_int32 timeout);

/**************************************************************************
*
* @purpose  Give a Semaphore
*
* @param    SemaID @b{(input)}  ID of the semaphore, returned from the create, to release
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if semaphore is invalid
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t    osapiSemaGive (void *SemaID);

/**************************************************************************
*
* @purpose  Unblock any and all tasks waiting on the semaphore
*
* @param    SemaID    @b{(input)}  ID of the semaphore returned from the create
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if semaphore is invalid
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t    osapiSemaFlush (void *SemaID);

/**************************************************************************
*
* @purpose  Delete a Semaphore
*
* @param    SemaID    @b{(input)}  ID of the semaphore to delete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if semaphore is invalid
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t    osapiSemaDelete (void *SemaID);

/**************************************************************************
*
* @purpose  create a Counting Semaphore
*
* @param    options         @b{(input)}  queueing style - either Priority style or FIFO style
* @param    initialCount    @b{(input)}  initialized to the specified initial count
*
* @returns  ptr to the semaphore or NULL if memory cannot be allocated
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void * osapiSemaCCreate (L7_int32 options, L7_int32 initialCount);

/**************************************************************************
*
* @purpose  Check for interrupt context
*
* @param    none
*
* @returns  L7_TRUE if the OS is currently processing an interrupt thread
* @returns  L7_FALSE is the OS is in normal processing mode
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_BOOL osapiIntContext( void );

/**************************************************************************
*
* @purpose  Receive a message from a message queue.
*
* @param    Queue_ptr    @b{(input)}  Pointer to message queue.
* @param    Message      @b{(input)}  Place to put the message.
* @param    Size         @b{(input)}  Number of bytes to move into the message.
* @param    Wait         @b{(input)}  a flag to wait or not. L7_NO_WAIT or L7_WAIT_FOREVER.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    This routine receives a message from the message queue queue_ptr. The received message is
*           copied into the specified buffer, Message, which is Size bytes in length.
*           If the message is longer than Size, the remainder of the message is discarded (no
*           error indication is returned).
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiMessageReceive(  void *queue_ptr, void *Message, L7_uint32 Size, L7_uint32 Wait);
/* JWL -- Wait should be L7_int32... */

/**************************************************************************
*
* @purpose  DEBUG: Peek a message from a message queue.
*
* @param    Queue_ptr    @b{(input)}  Pointer to message queue.
* @param    Message      @b{(input)}  Place to put the message.
* @param    Size         @b{(input)}  Number of bytes to move into the message.
* @param    msgOffset    @b{(input)}  Offset from the head of the list
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments !!This is a debug routine and is NOT THREAD-SAFE!!
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiMessagePeek(void *queue_ptr, void *Message, L7_uint32 Size, L7_uint32 msgOffset);

/**************************************************************************
*
* @purpose  Send a message to a message queue.
*
* @param    queue_ptr    @b{(input)}  Pointer to message queue.
* @param    Message      @b{(input)}  pointer to the message.
* @param    Size         @b{(input)}  size of the message in bytes.
* @param    Wait         @b{(input)}  a flag to wait or not. L7_NO_WAIT or L7_WAIT_FOREVER.
* @param    Priority     @b{(input)}  L7_MSG_PRIORITY_NORM or L7_MSG_PRIORITY_HI.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    This routine sends the message in buffer Message of length Size bytes
*           to the message queue queue_ptr. If any tasks are already waiting to
*           receive messages on the queue, the message will immediately
*           be delivered to the first waiting task. If no task is waiting to receive
*           messages, the message is saved in the message queue.
*
* @end
*
*************************************************************************/
L7_RC_t osapiMessageSend( void *queue_ptr, void *Message, L7_uint32 Size, L7_uint32 Wait, L7_uint32 Priority);
/* JWL -- Wait should be L7_int32... */

/**************************************************************************
*
* @purpose  Create a message queue.
*
* @param    queue_name      @b{(input)}  Caller's desired name for this queue.
* @param    queue_size      @b{(input)}  is the max number of the messages on the queue.
* @param    message_size    @b{(input)}  is the size of each message in bytes.
*
* @returns  pointer to the Queue ID structure or L7_NULLPTR if the create failed.
*
* @comments    This routine creates a message queue capable of holding up to
*           queue_size messages, each up to message_size bytes long. The
*           routine returns a void ptr used to identify the created message queue
*           in all subsequent calls to routines in this library. The queue will be
*           created as a FIFO queue.
*
* @end
*
*************************************************************************/
void * osapiMsgQueueCreate(   L7_char8 *queue_name, L7_uint32 queue_size, L7_uint32 message_size);

/**************************************************************************
*
* @purpose  Delete a message queue.
*
* @param    queue_ptr       @b{(input)}  Pointer to message queue to delete
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments    This routine deletes a message queue.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t  osapiMsgQueueDelete( void * queue_ptr);

/**************************************************************************
* @purpose  Returns the current number of messages on the specified message queue.
*
* @param    queue_ptr   @b{(input)}  Pointer to message queue.
* @param    bptr        @b{(input)}  Pointer to area to return number.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiMsgQueueGetNumMsgs( void * queue_ptr, L7_int32 *bptr);

/**************************************************************************
*
* @purpose  Create a task.
*
* @param    task_name    @b{(input)}  name to give the task.
* @param    task_entry   @b{(input)}  function pointer to begin the task.
* @param    argc         @b{(input)}  number of arguments. Must be < 10.
* @param    argv         @b{(input)}  pointer to start of list for arguments.
* @param    stack_size   @b{(input)}  number of bytes for the stack. Must be > 0.
* @param    priority     @b{(input)}  task priority. Must be <= 255.
* @param    time_slice   @b{(input)}  flag to allow time slicing for the task.
*
* @returns  task ID
* @returns  L7_ERROR
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiTaskCreate(L7_char8 *task_name,
                                void *task_entry,
                                L7_uint32 argc,
                                /*@null@*/void *argv,
                                L7_uint32 stack_size,
                                L7_uint32 priority,
                                L7_uint32 time_slice );

/**************************************************************************
*
* @purpose  Delay a task for the number of system ticks
*
* @param    ticks   @b{(input)}  number of clock ticks to delay
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if called from interrupt level or if the calling task receives a signal that is
*           not blocked or ignored.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t  osapiTaskDelay ( L7_int32 ticks );

/**************************************************************************
*
* @purpose  Delete a task.
*
* @param    task_id   @b{(input)}  handle for the task to be deleted.
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiTaskDelete( L7_int32 task_id);

/**************************************************************************
*
* @purpose  Send a Signal to a task.
*
* @param    task_id @b{(input)}    handle for the task to be deleted.
* @param    signal_id @b{(input)}  Signal code
*
* @returns  none.
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API void osapiTaskSignal( L7_int32 task_id, int sig);

/**************************************************************************
*
* @purpose  Show all active tasks in system
*
* @param    none.
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void     osapiShowTasks( void );

/**************************************************************************
*
* @purpose  Get the task ID of the calling task.
*
* @param    TaskIDPtr @b{(input)}  ptr where to place the Task ID of the the calling task.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE If called from interrupt context
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiTaskIDSelfGet( L7_int32 *TaskIDPtr );

/**************************************************************************
*
* @purpose  Change the priority of a task.
*
* @param    TaskID   @b{(input)}  ID of task to change priority.
* @param    Priority @b{(input)}  new task priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if requested priority is not within the range of 0-255.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiTaskPrioritySet ( L7_int32 TaskID, L7_int32 Priority );

/**************************************************************************
*
* @purpose  Sleep for a given number of seconds.
*
* @param    seconds    @b{(input)}  number of seconds to sleep.
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void     osapiSleep( L7_uint32 seconds);

/**************************************************************************
*
* @purpose  Sleep for a given number of micro seconds
*
* @param    usec   @b{(input)}  Number of micro-seconds to sleep.
*
* @returns  none.
*
* @comments    CAUTION! The precision is in system ticks per second as determined
*           by the system clock rate, even though the units are in microseconds.
*
* @end
*
*************************************************************************/
EXT_API void     osapiSleepUSec( L7_uint32 usec);

/**************************************************************************
*
* @purpose  Sleep for a given number of milliseconds
*
* @param    msec   @b{(input)}  Number of milliseconds to sleep.
*
* @returns  none.
*
* @comments    CAUTION! The precision is in system ticks per second as determined
*           by the system clock rate, even though the units are in milliseconds.
*
* @end
*
*************************************************************************/
EXT_API void     osapiSleepMSec( L7_uint32 msec);

#ifndef _L7_OS_LINUX_

/**************************************************************************
*
* @purpose  Sleep for a given number of milliseconds
*
* @param    milliseconds   @b{(input)}  Number of milliseconds to sleep.
*
* @returns  none.
*
* @comments This macro's primary purpose is to reduce boot code size
*           by not having to include osapi routines and other functions not
*           used by boot code.
*
* @end
*
*************************************************************************/
#define OSAPI_MSEC_SLEEP(milliseconds) \
{ \
  struct timespec delay, remains; \
  delay.tv_sec = (milliseconds) / 1000; \
  delay.tv_nsec = ((milliseconds) % 1000)*1000000; \
  while (nanosleep (&delay, &remains) != 0) \
  { \
    delay = remains; \
  } \
}

/**************************************************************************
*
* @purpose  Sleep for a given number of seconds
*
* @param    seconds   @b{(input)}  Number of seconds to sleep.
*
* @returns  none.
*
* @comments This macro's primary purpose is to reduce boot code size
*           by not having to include osapi routines and other functions not
*           used by boot code.
*
* @end
*
*************************************************************************/
#define OSAPI_SEC_SLEEP(seconds) \
{ \
  struct timespec delay, remains; \
  delay.tv_sec = (seconds); \
  delay.tv_nsec = 0; \
  while (nanosleep (&delay, &remains) != 0) \
  { \
    delay = remains; \
  } \
}

#else

#define OSAPI_MSEC_SLEEP(milliseconds) osapiSleepMSec(milliseconds)
#define OSAPI_SEC_SLEEP(seconds) osapiSleep(seconds)

#endif

#ifdef _L7_OS_VXWORKS_

/* Deprecated in Linux */

/**************************************************************************
*
* @purpose  Lock a task.
*
* @param    none.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    This routine disables task context switching. The task that calls
*           this routine will be the only task that is allowed to execute,
*           unless the task explicitly gives up the CPU by making itself no
*           longer ready. Typically this call is paired with osapiTaskUnlock();
*           together they surround a critical section of code. These preemption
*           locks are implemented with a counting variable that allows nested
*           preemption locks. Preemption will not be unlocked until
*           osapiTaskUnlock() has been called as many times as osapiTaskLock().
*
* @notes    This routine does not lock out interrupts; use osapiDisableInts()
*           to lock out interrupts.
*
* @notes    A osapiTaskLock()is preferable to osapiDisableInts() as a means of
*           mutual exclusion, because interrupt lock-outs add interrupt latency
*           to the system.
*
* @notes    A osapiSemaTake() is preferable to osapiTaskLock() as a means of mutual
*           exclusion, because preemption lock-outs add preemptive latency to the system.
*
* @notes    The osapiTaskLock() routine is not callable from interrupt service routines.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiTaskLock( void );

#endif /* _L7_OS_VXWORKS_ */

#ifdef _L7_OS_VXWORKS_

/* Deprecated in Linux */

/**************************************************************************
*
* @purpose  Resume task scheduling.
*
* @param    none.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @comments    Typically this call is paired with osapiTaskLock() and concludes a
*           critical section of code. Preemption will not be unlocked until
*           osapiTaskUnlock() has been called as many times as osapiTaskLock().
*           When the lock count is decremented to zero, any tasks that were eligible
*           to preempt the current task will execute. The osapiTaskUnlock() routine
*           is not callable from interrupt service routines.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiTaskUnlock( void );

#endif /* _L7_OS_VXWORKS_ */

/**************************************************************************
*
* @purpose  Waits for a task to complete initializaiton
*
* @param    synchandle  @b{(input)}  handle of task to wait for
* @param    timeout     @b{(input)}  timeout in milliseconds to wait for task
*
* @returns  L7_SUCCESS  task is initialized, continue normal
* @returns  L7_FAILURE  task is not init or could not sync up
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiWaitForTaskInit( L7_uint32 syncHandle, L7_int32 timeout );

/**************************************************************************
*
* @purpose  Signals to a waiting task that this task has completed initialization
*
* @param    syncHandle  @b{(input)}  handle of the giving task
*
* @returns  L7_SUCCESS if a task is waiting on the syncHandle
* @returns  L7_FAILURE if a task is not waiting on the syncHandle
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiTaskInitDone(L7_uint32 syncHandle);

/**************************************************************************
*
* @purpose  Free memory
*
* @param    compId    @b{(input)}  the component Id from L7_COMPONENT_IDS_t
* @param    memory    @b{(input)}  Pointer to the memory to free.
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiFree(L7_COMPONENT_IDS_t compId, void * );

/**************************************************************************
*
* @purpose  Allocate nbytes of memory.
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
#define osapiMalloc(compId, mem_size)  osapiMalloc_track(mem_size, compId, __FILE__, __LINE__)
EXT_API void *osapiMalloc_track( L7_uint32 nbytes,
                                 L7_COMPONENT_IDS_t compId,
                                 L7_char8 *file_name,
                                 L7_uint32 line_number );

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
#define osapiMallocTry(compId, mem_size)  osapiMalloc_track_try(mem_size, compId, __FILE__, __LINE__)
void *osapiMalloc_track_try( L7_uint32 numberofbytes,
                             L7_COMPONENT_IDS_t compId,
                             L7_char8 *file_name,
                             L7_uint32 line_number );

/**************************************************************************
*
* @purpose  Reallocate nbytes of memory.
*
* @param    compId    @b{(input)}  the component Id from L7_COMPONENT_IDS_t
* @param    pBlock    @b{(input)}  memory to reallocate.
* @param    nbytes    @b{(input)}  number of bytes to reallocate.
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
#define osapiRealloc(compId, pBlock, mem_size)  osapiRealloc_track(pBlock, mem_size, compId, __FILE__, __LINE__)
EXT_API void *osapiRealloc_track(void *pBlock,
                                 L7_uint32 nbytes,
                                 L7_COMPONENT_IDS_t compId,
                                 L7_char8 *file_name,
                                 L7_uint32 line_number);

/**************************************************************************
*
* @purpose  Allocate nbytes of memory on a given boundary.
*
* @param    compId          @b{(input)}  the component Id from L7_COMPONENT_IDS_t
* @param    alignment       @b{(input)}  boundary to align the allocate - power of 2.
* @param    numberofbytes   @b{(input)}  number of bytes to allocate.
*
* @returns  pointer to memory allocated or L7_NULLPTR if memory could not be allocated.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void *osapiMalloc_aligned( L7_COMPONENT_IDS_t compId,
                                   L7_uint32 alignment,
                                   L7_uint32 numberofbytes);

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
EXT_API void osapiDebugMallocSummary(L7_COMPONENT_IDS_t compId, L7_uint32 delta);

/**************************************************************************
*
* @purpose  Retrieve number of seconds since last reset and return in number
*           of days, hours, minutes, seconds
*
* @param    ts     @b{(input)}  ptr to L7_timespec structure
*
* @returns  void
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiUpTime ( L7_timespec *ts );

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
EXT_API L7_RC_t osapiLocalTime(L7_uint32 seconds, L7_localtime *lt);

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
EXT_API L7_uint32 osapiMkTime(L7_localtime *lt);

/**************************************************************************
*
* @purpose  Retrieve number of seconds since last reset
*
* @param    none
*
* @returns  rawtime
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiUpTimeRaw ( void );

/**************************************************************************
*
* @purpose  Retrieve number of seconds since last reset
*
* @param    none
*
* @returns  rawtime in milliseconds
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiUpTimeMillisecondsGet ( void );

/**************************************************************************
*
* @purpose  Retrieve number of seconds and nanoseconds since last reset
*
* @param    ct @b{(input)}  ptr to place values
*
* @returns  void
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiClockTimeRaw ( L7_clocktime *ct );


/**************************************************************************
*
* @purpose  Get the value of the kernel's tick counter
*
* @param    none
*
* @returns  returns the current value of the tick counter.
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiTickGet(void);

/**************************************************************************
*
* @purpose  Retrieve number of milliseconds
*
* @param    none
*
* @returns  milliseconds since last reset
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiTimeMillisecondsGet( void );

/**************************************************************************
*
* @purpose  Retrieve number of milliseconds (64 bit version)
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
EXT_API L7_uint64 osapiTimeMillisecondsGet64( void );

/**************************************************************************
*
* @purpose  Retrieve number of milliseconds since aux_old 
*
* @param    void
*
* @returns  milliseconds since aux_old
*
* @notes
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiTimeMillisecondsGetOffset( L7_uint32 aux_old);

/**************************************************************************
*
* @purpose  Retrieve number of milliseconds from  aux_old to aux_current
*
* @param    void
*
* @returns  milliseconds from aux_old to aux_current
*
* @notes
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiTimeMillisecondsDiff( L7_uint32 aux_current, L7_uint32 aux_old);

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
EXT_API L7_uint64 osapiTimeMicrosecondsGet( void );

/**************************************************************************
*
* @purpose  convert raw uptime into number of days, hours, minutes, seconds
*
* @param    rawTime   @b{(input)}  raw time value
* @param    ts        @b{(input)}  ptr to L7_timespec structure
*
* @returns  void
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiConvertRawUpTime ( L7_uint32 rawTime, L7_timespec *ts );


/**********************************************************************
* @purpose  Get the current UTC time since the Unix Epoch.
*
* @param    ct @b{(output)} pointer to returned UTC time
*
* @returns  L7_SUCCESS
*
* @notes    NTP epoch is different from Unix epoch.
*
* @end
*********************************************************************/


EXT_API L7_RC_t osapiUTCTimeGet(L7_clocktime * ct);

/**********************************************************************************
* @purpose  Set the clock-time to zero.
*
* @returns  none
*
* @notes    used when setting the default config data.
*
* @end
*************************************************************************************/

EXT_API void osapiSetTimeToDefault();


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
EXT_API L7_int64 osapiNTPTimeGet(void);


/**********************************************************************
* @purpose  Update the local clock. For use by SNTP client only.
*
* @param    offset @b{(input)} the correction to be applied.
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

EXT_API L7_RC_t osapiNTPTimeSet(L7_int32 offset);

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

EXT_API L7_RC_t osapiNTPTimeSetRaw(int year, int month, int day, int hour, int minute, int second);

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
EXT_API L7_uint32 osapiUTCTimeNow();

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
void osapiRawTimeToUTCTime(L7_uint32 rawTime, L7_uint32 *utcTime);

/**************************************************************************
*
* @purpose  Manage vxworks mbufs
*
* @param    bufHandle   @b{(input)}  vxWorks mubf
* @param    bufData      @b{(output)} start of data for vxWorks mubf
* @param    bufSize      @b{(output)} size of data for vxWorks mubf
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiNetMbufGetNextBuffer( void *bufHandle, L7_uchar8 **bufData, L7_uint32 *bufSize );

/**************************************************************************
*
* @purpose  Free vxworks mbufs
*
* @param    bufHandle   @b{(input)}  vxWorks mubf
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiNetMbufFree( void *bufHandle );

/**************************************************************************
*
* @purpose  Get the next mbuf in chain
*
* @param    mbuf    @b{(input)}  vxWorks mubf
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void *osapiNetMbufGetNextMbuf( void *mbuf );

/**************************************************************************
*
* @purpose  Get the total length of data in a mbuf chain
*
* @param    mbuf    @b{(input)}  vxWorks mubf
*
* @returns  frameLength
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiNetMbufGetFrameLength( void *mbuf );

/**************************************************************************
*
* @purpose  Initializes the VxWorks IP Network
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiNetworkInit ( void );

/**************************************************************************
*
* @purpose  Conifgures an ip interface.
*
* @param    ifName   @b{(input)}  pointer to the interface name
* @param    ipAddr   @b{(input)}  32bit ip address
* @param    ifName   @b{(input)}  32bit netmask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiNetIfConfig( L7_uchar8 *ifName, L7_uint32 ipAddr, L7_uint32 ipNetMask );


/**************************************************************************
*
* @purpose  Delete ARP entries from the operating system ARP table.
*
* @param    ifName  @b{(input)}  interface name (null ptr for all interfaces)
*
* @returns     none
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiArpFlush(L7_uchar8 *ifName);



/**************************************************************************
* @purpose  Adds a gateway route
*
* @param    gateway  @b{(input)}  32bit gateway ip address
* @param    intIfNum @b{(input)}  32bit outgoing internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments    For VxWorks, it seems only the first added gateway is used, no matter how
*           many are in the route table. For this design, there will be only one gateway
*           in the table. If the Service Port is up and the Service Port has a non-zero
*           gateway, the Network gateway will be removed from the table and the Service
*           Port's added. If the Service Port is up but its gateway is zero, the Network's
*           gateway will not be removed.  The bulk of the logic is in simSetServPortIPGateway().
*
*           Note that there is another function osapiAddDefaultGateway.
*           Do not use that function. For all purposes use this function.
*           For more info on why that function was created, check its
*           prologue.
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiAddGateway( L7_uint32 gateway, L7_uint32 intIfNum );

/**************************************************************************
*
* @purpose  Deletes a gateway route
*
* @param    gateway  @b{(input)}  32bit gateway ip address
* @param    intIfNum @b{(input)}  32bit outgoing internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiDelGateway( L7_uint32 gateway, L7_uint32 intIfNum );

/**************************************************************************
* @purpose  Removes the interface from the route table
*
* @param    intf   @b{(input)}  pointer to the interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API void osapiCleanupIf(const L7_char8 *intf);

/*********************************************************************
 * @purpose  Notify IP stack of creation of virtual router
 *
 * @param    vrid    virtual router ID (1 to L7_VR_MAX_VIRTUAL_ROUTERS - 1)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_ERROR   if vrid is out of range.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t osapiIpStackVrAdd(L7_uint32 vrid);

/*********************************************************************
 * @purpose  Notify IP stack of deletion of virtual router
 *
 * @param    vrid    virtual router ID (1 to L7_VR_MAX_VIRTUAL_ROUTERS)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 * @returns  L7_ERROR   if vrid is out of range.
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t osapiIpStackVrDelete(L7_uint32 vrid);

/*********************************************************************
 * @purpose  Notify the IP stack that an interface has been attached to
 *           a virtual router.
 *
 * @param    intIfNum         internal interface number of interface
 * @param    vrid             virtual router ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t osapiIpStackIntfVrAttach(L7_uint32 intIfNum,
    L7_uint32 vrid);
/*********************************************************************
 * @purpose  Ask the IP stack which virtual router an interface has been
 *           attached to.
 *
 * @param    ifname         interface name
 * @param    vrid           virtual router ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t osapiIpStackIntfVrGet(L7_uchar8 *ifname,
    L7_uint32 *vrid);

/*********************************************************************
 * @purpose  Associate a socket with a virtual router. Packets sent on
 *           the socket will be routed using the virtual router's
 *           routing table.
 *
 * @param    fd         socket file descriptor
 * @param    vrid       virtual router ID
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
EXT_API L7_RC_t osapiIpStackSocketVrAttach(L7_int32 fd,
    L7_uint32 vrid);

/**************************************************************************
* @purpose  Add a route to a destination that is a network
*
* @param    destination  @b{(input)}  32bit destination ip address
* @param    gateway      @b{(input)}  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiAddNetRoute( L7_uint32 destination, L7_uint32 gateway );

/**************************************************************************
* @purpose  Add a route
*
* @param    destination  @b{(input)}  32bit destination ip address
* @param    gateway      @b{(input)}  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiAddRoute( L7_uint32 destination, L7_uint32 gateway );

/**************************************************************************
* @purpose  Delete a route
*
* @param    destination  @b{(input)}  32bit destination ip address
* @param    gateway      @b{(input)}  32bit gateway ip address
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiDeleteRoute( L7_uint32 destination, L7_uint32 gateway );

/**************************************************************************
* @purpose  Add a route
*
* @param    ipAddr    @b{(input)}  32bit destination ip address
* @param    netmask   @b{(input)}  32bit network mask
* @param    gateway   @b{(input)}  32bit gateway ip address
* @param    intIfNum  @b{(input)}  32bit outgoing interface number
* @param    Flags        route flags
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiAddMRoute( L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway, L7_uint32 intIfNum, L7_uint32 flags );


/**************************************************************************
* @purpose  Add a v6 route
*
* @param    network      128bit destination ip address
* @param    gateway      128bit gateway ip address
* @param    netmask      128 bit net mask
* @param    Flags        route flags
* @param    intIfName    interface name (used only when intIfNum is 0)
*
* @notes    none
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiAddMRoute6(L7_in6_addr_t *network, L7_uint32 prefixLen,
                        L7_in6_addr_t *gateway, L7_uint32 intIfNum, L7_uint32 flags,
                        L7_uchar8 *intIfName);

/**************************************************************************
* @purpose  Delete a v6 route
*
* @param    network      128bit destination ip address
* @param    gateway      128bit gateway ip address
* @param    netmask      128 bit net mask
* @param    intIfName    interface name (used only when intIfNum is 0)
*
* @notes    none
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiDelMRoute6(L7_in6_addr_t *network, L7_uint32 prefixLen,
                        L7_in6_addr_t *gateway, L7_uint32 intIfNum, L7_uint32 flags,
                        L7_uchar8 *intIfName);





/*********************************************************************
* @purpose  Add an ipv6 address/prefixlen to stack
*
* @param    ifname       interface name
* @param    addr         pointer to address
* @param    prefixLen    address prefix length
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    no special status of primary address
*
* @end
*********************************************************************/
L7_RC_t osapiNetAddrV6Add(L7_uchar8 *ifname,
                L7_in6_addr_t  *addr, L7_uint32 prefixlen);


/*********************************************************************
* @purpose  Delete an ipv6 address from stack
*
* @param    ifname       interface name
* @param    addr         pointer to address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    prefixLen not required
*
* @end
*********************************************************************/
L7_RC_t osapiNetAddrV6Del( L7_uchar8 *ifname, L7_in6_addr_t *addr,L7_uint32 prefixlen);


/*********************************************************************
* @purpose  Set router advertise nd ns interval (ie retransmit)
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6NdNsIntervalSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Set interface ipv6_enable
*
* @param    ifName   @b{(input)} Internal Name
* @param    val      @b{(input)} boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6EnableSet( L7_uchar8 *ifName, L7_BOOL enable);

/*********************************************************************
* @purpose  Set interface ipv6 address autoconfiguration mode
*
* @param    ifName @b{(input)} Interface Name
* @param    enable @b{(input)} if enabled or not
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6AddrAutoConfigSet(L7_uchar8 *ifName, L7_uint32 enable);

/*********************************************************************
* @purpose  Set router advertise nd reachable time
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in milliseconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6NdReachableTimeSet( L7_uint32 intIfNum, L7_uint32 val);



/*********************************************************************
* @purpose  Set number of dad transmits
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6DadTransmitsSet( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Set mode of sending ICMPv6 destination unreachables
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6IcmpSendDestinationUnreachableSet ( L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Set the ICMPv6 Rate limit
*
* @param    burstSize  @b{(input)} value
* @param    interval @b{(input)} error interval
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIpv6IcmpRatelimitSet ( L7_uint32 bursttSize, L7_uint32 interval);

/*********************************************************************
* @purpose  Set the amount of time that a neighbor cache entry remains
*           stale before being garbage collected.
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is analogous to the ARP max age time. The draft update to
*           RFC 2461 says "ten minutes or more" should be adequate. Linux 2.6
*           uses 1 minute by default. Linux 2.4 used 1 day by default. This
*           function is introduced to allow us to set a common value (one that
*           approximates out VxWorks behavior).
*
*           Since we forward in hardware, neighbor entries usually sit in
*           stale state.
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6StaleTimeSet(L7_uint32 intIfNum, L7_uint32 val);


/*********************************************************************
* @purpose  Set accept router advertisement
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} boolean
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6AcceptRASet( L7_uint32 intIfNum, L7_BOOL val);


/*********************************************************************
* @purpose  Set ipv6 mtu
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} mtu
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6MtuSet( L7_uint32 intIfNum, L7_uint32 val);


/*********************************************************************
* @purpose  Set ipv6 interface hop-limit
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    val      @b{(input)} value in bytes
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    linux does this per interface, rest of universe treats as
*           global, so ip6map calls once with 0 for intIfNu and once per
*           enabled interface.
*
* @end
*********************************************************************/
L7_RC_t osapiIfIpv6HopLimitSet( L7_uint32 intIfNum, L7_uint32 val);

/**************************************************************************
*
* @purpose  Delete all NDP entries from the operating system NDP table.
*           Or single entry if ip6Addr not null.
*
* @param       intIfNum   internal interface number
* @param       ip6Addr    point to NDP address
*
* @returns     none
*
* @comments
*
* @end
*
*************************************************************************/
void osapiIpv6NdpFlush( L7_uint32 intIfNum, L7_in6_addr_t *ip6Addr);



/**************************************************************************
*
* @purpose  Set ndp cache size
*
* @param       val        max cache
*
* @returns     none
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t osapiIpv6NdpCacheSizeSet( L7_uint32 val);


/*********************************************************************
 * @purpose  Enables IPV6 on an interface
 *
 * @param    ifname         interface name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t osapiIfIpv6Enable( L7_uchar8 *ifname );


/*********************************************************************
 * @purpose  Disables IPV6 on an interface
 *
 * @param    ifname         interface name
 *
 * @returns  L7_SUCCESS or L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t osapiIfIpv6Disable( L7_uchar8 *ifname );


/**************************************************************************
* @purpose  Delete a route
*
* @param    ipAddr    @b{(input)}  32bit destination ip address
* @param    netmask   @b{(input)}  32bit network mask
* @param    gateway   @b{(input)}  32bit gateway ip address
* @param    intIfNum  @b{(input)}  32bit internal interface number for entry
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiDeleteMRoute( L7_uint32 ipAddr, L7_uint32 netmask, L7_uint32 gateway, L7_uint32 intIfNum );

/**************************************************************************
* @purpose  Adds the service port gateway when link goes up
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API void osapiServicePortLinkUp (void);

/**************************************************************************
* @purpose  Deletes the service port gateway and adds the network gateway when link goes down
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API void osapiServicePortLinkDown (void);


/**************************************************************************
*
* @purpose  Send ICMP ECHO_REPLY packets to a particular network host with
*           time between two requests specified by the user.
*
* @param    hostName       @b{(input)}  destination host's dotted decimal ip address or name
* @param    numPackets     @b{(input)}  number of echo replies to send and receive
* @param    msWait         @b{(input)}  Wait time in milliseconds between two requests
*
* @returns  Number of packets echoed
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiPingTimed ( L7_char8 *hostName, L7_int32 numPackets, L7_uint32 msWait );

/**************************************************************************
*
* @purpose  Send ICMP ECHO_REPLY packets to a particular network host
*
* @param    hostName       @b{(input)}  destination host's dotted decimal ip address or name
* @param    numPackets     @b{(input)}  number of echo replies to send and receive
*
* @returns  Number of packets echoed
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiPing ( L7_char8 *hostName, L7_int32 numPackets );

/**************************************************************************
*
* @purpose  get the next IP MIB-II arp entry
*
* @param    index       @b{(input)/(output)} starting index to find next index
* @param    ipaAddr     @b{(input)/(output)} entry's ip address
* @param    mac         @b{(output)} entry's mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if the next entry does not exist
*
* @comments index of zero is used to start search
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiM2IpArpTblEntryGet(L7_long32 *index, L7_ulong32 *ipAddr, L7_uchar8 *mac);

/**************************************************************************
*
* @purpose  Turn off all of the interrupts for this system.
*
* @param    none.
*
* @returns  The MSR register value (Lock Key) used to unlock interrupts later.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiDisableInts ( void );

/**************************************************************************
*
* @purpose  ReEnable EXT_APIal interrupts for this system.
*
* @param    LockKey   @b{(input)}  The MSR register value previously returned from the lockout routine.
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void     osapiEnableInts  ( L7_uint32 LockKey );


/*********************************************************************
*
* @structures osapiTimerCallback_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef void (*osapiTimerCallback_t)( L7_uint32 parm1, L7_uint32 parm2 );

/*********************************************************************
*
* @structures osapiTimerDescr_t
*
* @purpose
*
* @comments
*
*********************************************************************/
typedef struct osapiTimerDescr_s
{
  osapiTimerCallback_t  callback;
  L7_uint32         parm1;
  L7_uint32         parm2;
  L7_uint32         timer_in_use;
  L7_uint32         timer_running;
  L7_uint32         time_count;
  L7_uint32         orig_count;

} osapiTimerDescr_t;

#define OSAPI_TIMER_TASK_SLEEP (40) /* in milliseconds */





/**************************************************************************
* @purpose  restarts an already running timer
*
* @param    osapitimer @b{(input)}  ptr to an osapi timer descriptor to reset
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiRestartUserTimer (osapiTimerDescr_t * osapitimer);

/**************************************************************************
* @purpose  change a timer's count value
*
* @param    osapitimer   @b{(input)}  ptr to an osapi timer descriptor
* @param    newTimeCount @b{(input)}  new value timer is to assume
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiChangeUserTimer (osapiTimerDescr_t * osapitimer, L7_uint32 newTimeCount );

/**************************************************************************
* @purpose  stop an already running timer
*
* @param    osapitimer @b{(input)}  ptr to an osapi timer descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiStopUserTimer (osapiTimerDescr_t * osapitimer);

/**************************************************************************
* @purpose  Task that wakes up periodically and invokes active timers.
*
* @param    none.
*
* @returns  none.
*
* @comments
*
* @end
*************************************************************************/
EXT_API void osapiTimerHandler(void) ;


/**************************************************************************
* @purpose  Task that wakes up periodically and invokes active timers.
*
* @param    none.
*
* @returns  none.
*
* @comments
*
* @end
*************************************************************************/
/* void osapiTimerDispatcher(L7_int32 index); */

/**************************************************************************
*
* @purpose  This is the user function to setup a new timeout call.
*
* @param    func           @b{(input)}  is the function to call when expired.
* @param    arg(2)         @b{(input)}  arguments to pass to the callback function.
* @param    milliseconds   @b{(input)}  number of milli-seconds to wait before timeout.
* @param    pTimerHolder   @b{(input)}  ptr to an osapiTimerDescr_t struct used to deallocate this timer by the user
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiTimerAdd ( void (*func)( L7_uint32, L7_uint32 ), L7_uint32 arg1, L7_uint32 arg2, L7_int32 milliseconds, osapiTimerDescr_t **pTimerHolder );

/**************************************************************************
*
* @purpose  Remove a function for the timeout list.
*
* @param    pTimerHolder   @b{(input)}  ptr to an osapi timer descriptor, that was returned in osapiTimerAdd(), to deallocate.
*
* @returns  none.
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiTimerFree ( osapiTimerDescr_t *pTimer );

/**************************************************************************
* @purpose  Provide periodic timer indications to delay sensitive tasks. Use
*           of these utilities minimizes accumulated skew.
*
* @param    L7_uint32  period   @b{(input)}  The fixed period.
*           L7_uint32 *handle   @b{(input)}  User provided storage for assigned handle.
*
* @returns  Failure message is no timers remain
*
* @comments The first available timer is assigned and the handle is returned
*           via the handle argument.
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiPeriodicUserTimerRegister(L7_uint32 period, L7_uint32 *handle);

/**************************************************************************
* @purpose  Wait on a previously defined periodic timer.
*
* @param    L7_uint32 handle   @b{(input)}  Assigned periodic timer handle.
*
* @returns  Nothing
*
* @comments Timer period previously supplied is used.
*
* @end
*************************************************************************/
EXT_API void osapiPeriodicUserTimerWait(L7_uint32 handle);

/**************************************************************************
* @purpose  Wait on a previously defined periodic timer.
*
* @param    L7_uint32 handle   @b{(input)}  Assigned periodic timer handle.
*
* @returns  Nothing
*
* @comments Timer period previously supplied is used.
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiPeriodicUserTimerDeregister(L7_uint32 handle);

/**************************************************************************
*
* @purpose     Set the Serial Port's baud rate
*
* @param       rate   @b{(input)}  The serail baudrate
* @param       waitForFlush @b{(input)} Delay for flush to complete.
*
* @returns     L7_SUCCESS if rate is valid and could be set
* @returns     L7_ERROR
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiBaudRateChange( L7_uint32 rate, L7_BOOL waitForFlush );

/**************************************************************************
* @purpose  Switch the system network mac address between Burned-In or
*           Locally Administered Mac Address
*
* @param    newMac  @b{(input)}  pointer to a mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_int32 osapiUpdateNetworkMac( L7_uchar8 *newMac );


/**************************************************************************
*
* @purpose     Call the OS I/O Control Function
*
* @param       fd           @b{(input)}  I/O file descriptor number
* @param       function     @b{(input)}  I/O function to perform
* @param       arg          @b{(input)}  argument to pass to I/O function
*
* @returns     L7_SUCCESS
* @returns     L7_ERROR driver returned an error or unsupported function
*
* @comments    see osapi.h for function and argument definitions
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiIoctl ( L7_int32 fd, L7_int32 function, L7_int32 arg );


/* DO NOT ADD SOCKET or INTERFACE DEFS!!! */
/* ioctl function codes */

#define OSAPI_FIONREAD        FIONREAD       /* get num chars available to read */
#define OSAPI_FIOFLUSH        FIOFLUSH       /* flush any chars in buffers */
#define OSAPI_FIOOPTIONS      FIOOPTIONS     /* set options (FIOSETOPTIONS) */
#define OSAPI_FIOBAUDRATE     FIOBAUDRATE    /* set serial baud rate */
#define OSAPI_FIODISKFORMAT   FIODISKFORMAT  /* format disk */
#define OSAPI_FIODISKINIT     FIODISKINIT    /* initialize disk directory */
#define OSAPI_FIOSEEK         FIOSEEK        /* set current file char position */
#define OSAPI_FIOWHERE        FIOWHERE       /* get current file char position */
#define OSAPI_FIODIRENTRY     FIODIRENTRY    /* return a directory entry (obsolete)*/
#define OSAPI_FIORENAME       FIORENAME      /* rename a directory entry */
#define OSAPI_FIOREADYCHANGE  FIOREADYCHANGE /* return TRUE if there has been a media change on the device */
#define OSAPI_FIONWRITE       FIONWRITE      /* get num chars still to be written */
#define OSAPI_FIODISKCHANGE   FIODISKCHANGE  /* set a media change on the device */
#define OSAPI_FIOCANCEL       FIOCANCEL      /* cancel read or write on the device */
#define OSAPI_FIOSQUEEZE      FIOSQUEEZE     /* squeeze out empty holes in rt-11 file system */
#define OSAPI_FIONBIO         FIONBIO        /* set non-blocking I/O; SOCKETS ONLY!*/
#define OSAPI_FIONMSGS        FIONMSGS       /* return num msgs in pipe */
#define OSAPI_FIOGETNAME      FIOGETNAME     /* return file name in arg */
#define OSAPI_FIOGETOPTIONS   FIOGETOPTIONS  /* get options */
#define OSAPI_FIOSETOPTIONS   FIOSETOPTIONS  /* set options */
#define OSAPI_FIOISATTY       FIOISATTY      /* is a tty */
#define OSAPI_FIOSYNC         FIOSYNC        /* sync to disk */
#define OSAPI_FIOPROTOHOOK    FIOPROTOHOOK   /* specify protocol hook routine */
#define OSAPI_FIOPROTOARG     FIOPROTOARG    /* specify protocol argument */
#define OSAPI_FIORBUFSET      FIORBUFSET     /* alter the size of read buffer  */
#define OSAPI_FIOWBUFSET      FIOWBUFSET     /* alter the size of write buffer */
#define OSAPI_FIORFLUSH       FIORFLUSH      /* flush any chars in read buffers */
#define OSAPI_FIOWFLUSH       FIOWFLUSH      /* flush any chars in write buffers */
#define OSAPI_FIOSELECT       FIOSELECT      /* wake up process in select on I/O */
#define OSAPI_FIOUNSELECT     FIOUNSELECT    /* wake up process in select on I/O */
#define OSAPI_FIONFREE        FIONFREE       /* get free byte count on device */
#define OSAPI_FIOMKDIR        FIOMKDIR       /* create a directory */
#define OSAPI_FIORMDIR        FIORMDIR       /* remove a directory */
#define OSAPI_FIOLABELGET     FIOLABELGET    /* get volume label */
#define OSAPI_FIOLABELSET     FIOLABELSET    /* set volume label */
#define OSAPI_FIOATTRIBSET    FIOATTRIBSET   /* set file attribute */
#define OSAPI_FIOCONTIG       FIOCONTIG      /* allocate contiguous space */
#define OSAPI_FIOREADDIR      FIOREADDIR     /* read a directory entry (POSIX) */
#define OSAPI_FIOFSTATGET     FIOFSTATGET    /* get file status info */
#define OSAPI_FIOUNMOUNT      FIOUNMOUNT     /* unmount disk volume */
#define OSAPI_FIOSCSICOMMAND  FIOSCSICOMMAND /* issue a SCSI command */
#define OSAPI_FIONCONTIG      FIONCONTIG     /* get size of max contig area on dev */
#define OSAPI_FIOTRUNC        FIOTRUNC       /* truncate file to specified length */
#define OSAPI_FIOGETFL        FIOGETFL       /* get file mode, like fcntl(F_GETFL) */
#define OSAPI_FIOTIMESET      FIOTIMESET     /* change times on a file for utime() */
#define OSAPI_FIOINODETONAME  FIOINODETONAME /* given inode number, return filename*/
#define OSAPI_FIOFSTATFSGET   FIOFSTATFSGET  /* get file system status info */


/* ioctl option values */

#define OSAPI_OPT_ECHO      OPT_ECHO      /* echo input */
#define OSAPI_OPT_CRMOD     OPT_CRMOD     /* lf -> crlf */
#define OSAPI_OPT_TANDEM    OPT_TANDEM    /* ^S/^Q flow control protocol */
#define OSAPI_OPT_7_BIT     OPT_7_BIT     /* strip parity bit from 8 bit input */
#define OSAPI_OPT_MON_TRAP  OPT_MON_TRAP  /* enable trap to monitor */
#define OSAPI_OPT_ABORT     OPT_ABORT     /* enable shell restart */
#define OSAPI_OPT_LINE      OPT_LINE      /* enable basic line protocol */





/*---------------------------*/
/* IP statistics retrievers  */
/*---------------------------*/

/*********************************************************************
* @purpose Determines if ip is forwarding or not
*
* @param   val  @b{(output)}  pointer to return forwarding status. L7_ENABLED or L7_DISABLED.
*
* @returns L7_SUCCESS
* @returns L7_FAILURE if MIB-II structure could not be retrieved.
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpForwardingGet(L7_int32 *val);

/*********************************************************************
* @purpose Get the default ttl
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpDefaultTTLGet(L7_int32 *val);

/*********************************************************************
* @purpose Get the total number of input datagrams received from interfaces
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpInReceivesGet(L7_uint32 *val);

/*********************************************************************
* @purpose Get the total number of input datagrams discarded due to headers
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpInHdrErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of input datagrams discarded because
*          the IP address in their IP header's destination
*          field was not a valid address to be received at
*          this entity.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpInAddrErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose          Gets the number of input datagrams for which this
*                   entity was not their final IP destination, as a
*                   result of which an attempt was made to find a
*                   route to forward them to that final destination.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpForwDatagramsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of locally-addressed datagrams
*          received successfully but discarded because of an
*          unknown or unsupported protocol.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpInUnknownProtosGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of input IP datagrams for which no
*          problems were encountered to prevent their
*          continued processing, but which were discarded
*
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpInDiscardsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the total number of input datagrams successfully
*          delivered to IP user-protocols (including ICMP).
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpInDeliversGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the total number of IP datagrams which local IP
*          user-protocols (including ICMP) supplied to IP in
*          requests for transmission.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments    Note that this counter does not include any datagrams counted in ipForwDatagrams
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpOutRequestsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of output IP datagrams for which no
*          problem was encountered to prevent their
*          transmission to their destination, but which were
*          discarded (e.g., for lack of buffer space).
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpOutDiscardsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of IP datagrams discarded because no
*          route could be found to transmit them to their
*          destination.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpOutNoRoutesGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the maximum number of seconds which received
*          fragments are held while they are awaiting
*          reassembly at this entity
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpReasmTimeoutGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of IP fragments received which needed
*          to be reassembled at this entity
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpReasmReqdsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of IP datagrams successfully re-assembled
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpReasmOKsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of failures detected by the IP re-
*          assembly algorithm (for whatever reason: timed
*          out, errors, etc).
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpReasmFailsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of IP datagrams that have been
*          successfully fragmented at this entity
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpFragOKsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of IP datagrams that have been
*          discarded because they needed to be fragmented at
*          this entity but could not be, e.g., because their
*          Don't Fragment flag was set.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpFragFailsGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of IP datagram fragments that have
*          been generated as a result of fragmentation at
*          this entity.
*
* @param val         @b{(output)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpFragCreatesGet(L7_uint32 *val);

/*********************************************************************
* @purpose Gets the number of routing entries which were chosen
*          to be discarded even though they are valid.
*
* @param val         @b{(output)}  Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpRoutingDiscardsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param ipAddr        @b{(input)}
* @param *maxSize      @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpAdEntReasmMaxSizeGet(L7_uint32 ipAddr, L7_uint32 *maxSize);

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param ipAddr        @b{(input)}
* @param *maxSize      @b{(output)}
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments As the OS performs reassembly this value will be retrieved
*           from the OS.
*           For Linux: The value will be obtained from the file
*           /proc/sys/net/ipv4/ipfrag_high_thresh
*           For VxWorks: The value will be obtained from the mib
*           using the call m2IpAddrTblEntryGet()
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpAdEntReasmMaxMemorySizeGet(L7_uint32 ipAddr, L7_uint32 *maxSize);

/**************************************************************************
*
* @purpose  Poll the service port's link status and set the gateway table if link changed
*
* @param    none
*
* @returns  none
*
* @end
*
*************************************************************************/
EXT_API void osapiServPortMonitorTask ( void );


/**************************************************************************
*
* @purpose  Enable a socket library function specific to this OS
*
* @param    socketType @b{(input)}  enum of which library to init
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiSocketLibInit ( OSAPI_SOCKET_LIB_t socketType );

/*********************************************************************
* @purpose  Retrieves the current link up/down status of the service port
*
* @param    none
*
* @returns  L7_UP
* @returns  L7_DOWN
*
* @end
*********************************************************************/
EXT_API L7_uint32 osapiServicePortLinkStatusGet ( void );

/**************************************************************************
* @purpose  Update the MAC address for a given interface
*
* @param    newMac   L7_uchar8 pointer to a mac address
* @param    intfName interface name
* @param    intfNun  interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiMacAddrChange(L7_uchar8 *newMac, 
                                   L7_uchar8 *intfName, int intfNum);

/**************************************************************************
* @purpose  Trigger sending gratuitous ARP on an interface for all its
*           IP addresses
*
* @param    intfName @b{(input)} interface name
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiIfGratArpSend(L7_uchar8 *intfName);

/*********************************************************************
* @purpose  Retrieves the current link up/down status of the serial port
*
* @param    none
*
* @returns  L7_UP
* @returns  L7_DOWN
*
* @end
*********************************************************************/
EXT_API L7_uint32 osapiSerialStatusGet ( void ) ;

/**************************************************************************
* @purpose  Sets the IP address for an interface.
*
* @param    ifName   @b{(input)}  pointer to the interface name
* @param    ipAddr   @b{(input)}  ip address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiNetIPSet( L7_uchar8 *ifName, L7_uint32 ipAddr );

/**************************************************************************
* @purpose  Sets the IP mask for an interface.
*
* @param    ifName   @b{(input)}  pointer to the interface name
* @param    ipmask   @b{(input)}  ip mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiNetMaskSet( L7_uchar8 *ifName, L7_uint32 ipMask );

/**************************************************************************
* @purpose  Attaches an interface.
*
* @param    ifunit   @b{(input)}  unit number
* @param    ifName   @b{(input)}  pointer to the interface name
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiIfAttach( L7_uint32 ufunit, L7_uchar8 *ifName);

/**************************************************************************
* @purpose  set or clear ipv6 accept rtadv on interface
*
* @param    ifName   @b{(input)}  pointer to the interface name
* @param    val      @b{(input)}  boolean if accept rtadv (eg host port)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiIfInet6AcceptRtadvSet( L7_uchar8 *ifName, L7_BOOL val);

#ifdef _L7_OS_LINUX_
/**************************************************************************
* @purpose  Gets the IP address validity for an interface
*
* @param    ifName   pointer to the interface name
* @param    ipAddr   host order IP address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    none.
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiNetIPGet( L7_uchar8 *ifName, L7_uint32 ipAddr );

/**************************************************************************
*
* @purpose  Process a transferred Kernel file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiKernelUpdate(void);

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
int osapiSystem(const char *fmt, ...);
#endif

/**************************************************************************
* @purpose  Gets the IP address for an interface
*
* @param    ifName   pointer to the interface name
* @param    ipAddr   network order IP address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments    dont confuse with linux version which gets valiidty as
*              opposed to the address
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiNetIPGetAddr( L7_uchar8 *ifName, L7_uint32 *ipAddr );

/**************************************************************************
*
* @purpose  Get list of ipv6 addresses on interface
*
* @param    ifName  interface name
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiIfIpv6AddrsGet(L7_uchar8 *ifName, L7_in6_prefix_t *addrs, L7_uint32 *acount);

/**************************************************************************
*
* @purpose  Get list of ipv6 default routers on interface
*
* @param    ifName  interface name
* @param    addrs   pointer to prefix array
* @param    acount  point to count (in = max, out = actual)
*
* @returns     none
*
* @comments    none.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiIfIpv6DefaultRoutersGet(L7_uchar8 *ifName, L7_in6_addr_t *addrs, L7_uint32 *acount);


/*********************************************************************
* @purpose  The total number of ICMP messages which the entity
*           received.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments Note that this counter includes all those counted by
*            icmpInErrors
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInMsgsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*
* @param val      @b{(input)}    datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           received.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInDestUnreachsGet(L7_uint32 *val);

/*********************************************************************
* @purpose The number of ICMP Time Exceeded messages received.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInTimeExcdsGet(L7_uint32 *val);

/*********************************************************************
* @purpose The number of ICMP Parameter Problem messages received
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInParmProbsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInSrcQuenchsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Redirect messages received.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInRedirectsGet(L7_uint32 *val);

/*********************************************************************
* @purpose The number of ICMP Echo (request) messages received.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInEchosGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages received
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInEchoRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages received
*
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInTimestampsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInTimestampRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Request messages received
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInAddrMasksGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages received
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpInAddrMaskRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The total number of ICMP messages which this entity
*           attempted to send.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  Note that this counter includes all those counted by
*            icmpOutErrors
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutMsgsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP messages which this entity did not
*           send due to problems discovered within ICMP.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments     This includes problems such as lack of buffers. This
*            value should not include errors discovered outside of
*            the ICMP layer such as the inability of IP to route the
*            resultant datagram. In some implementations there may
*            be no types of error which contribute to this counter's
*            value.
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutErrorsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           sent
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutDestUnreachsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Time Exceeded messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutTimeExcdsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Parameter Problem messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutParmProbsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutSrcQuenchsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Redirect messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments  For a host, this object will always be zero, since
*            hosts do not send redirects.
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutRedirectsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo (request) messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutEchosGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutEchoRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutTimestampsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages sent
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutTimestampRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address mask Request messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutAddrMasksGet(L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages sent.
*
* @param val        @b{(input)}  datagrams
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIcmpOutAddrMaskRepsGet(L7_uint32 *val);

/*********************************************************************
* @purpose determines if the entry exists in the ipNetToMedia table
*
* @param ipNetToMediaIfIndex         @b{(input)} internal interface number
* @param ipNetToMediaNetAddress      @b{(input)} network address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpNetToMediaEntryGet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress);

/*********************************************************************
* @purpose retrieves the next valid entry in the ipNetToMedia table
*
* @param ipNetToMediaIfIndex         @b{(input)} internal interface number
* @param ipNetToMediaNetAddress      @b{(input)} network address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpNetToMediaEntryNextGet(L7_uint32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress);

/*********************************************************************
* @purpose return the ipNetToMediaPhysAddress object from vxworks
*
* @param ipNetToMediaIfIndex         @b{(input)} internal interface number
* @param ipNetToMediaNetAddress      @b{(input)} network address
* @param physAddress                 @b{(output)} physAddress buffer
* @param addrLength                  @b{(output)} physAddress buffer length
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpNetToMediaPhysAddressGet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 *addrLength);

/*********************************************************************
* @purpose set the ipNetToMediaPhysAddress object
*
* @param ipNetToMediaIfIndex          @b{(input)} internal interface number
* @param ipNetToMediaNetAddress       @b{(input)} network address
* @param physAddress                  @b{(input)} physAddress buffer
* @param addrLength                   @b{(input)} physAddress buffer length
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpNetToMediaPhysAddressSet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 addrLength);

/*********************************************************************
* @purpose get the ipNetToMediaType object
*
* @param ipNetToMediaIfIndex          @b{(input)} internal interface number
* @param ipNetToMediaNetAddress       @b{(input)} network address
* @param ipNetToMediaType         @b{(output)} ipNetToMediaType
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpNetToMediaTypeGet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 *ipNetToMediaType);

/*********************************************************************
* @purpose set the ipNetToMediaType object
*
* @param ipNetToMediaIfIndex          @b{(input)} internal interface number
* @param ipNetToMediaNetAddress       @b{(input)} network address
* @param ipNetToMediaType             @b{(input)} ipNetToMediaType
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments   includes errors received
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiIpNetToMediaTypeSet(L7_uint32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 ipNetToMediaType);

/*********************************************************************
* @purpose  Add the VRRP interface name,VRID and VRIP in the stack
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    vrid        @b{(input)} VRRP group ID
* @param    addresses   @b{(input)} Pointer to vr ip addresses
* @param    netmask     @b{(input)} Pointer to subnet mask array
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*   
* @end
*********************************************************************/
EXT_API L7_RC_t osapiVrrpMasterVrrpAddressAdd(L7_uint32 intIfNum, L7_uchar8 vrid,
                                              L7_uint32 *addresses, L7_uint32 *netmask);

/*********************************************************************
* @purpose  Delete the VRRP interface name,VRID and VRIP in the stack
*
* @param    intIfNum      @b{(input)} value
* @param    vrid             @b{(input)} value
* @param    numAddrs    @b{(input)} value
* @param    addresses    @b{(input)} value*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*   
* @end
*********************************************************************/
EXT_API L7_RC_t osapiVrrpMasterVrrpAddressDelete(L7_uint32 intIfNum, L7_uchar8 vrid,
                                                 L7_uint32 *addresses);

/**************************************************************************
*
* @purpose     Set the status of service port default gateway
*
* @param       status @b{(input)} status
*
* @returns     void
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API void osapiServPortDefaultRouteEnableSet(L7_uint32 status);


/**************************************************************************
*
* @purpose     Get the status of serv port default gateway
*
* @param       none
*
* @returns     L7_TRUE      If serv port should not use IP Map def route
* @returns     L7_FALSE     If serv port should use IP Map def route
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiServPortDefaultRouteEnableGet(void);


/**************************************************************************
*
* @purpose     Convert an IP address from a hex value to an ASCII string
*
* @param       ipAddr       @b{(input)} IP address to be converted (host byte
* @param       buf          @b{(output)} location to store IP address string
*
* @returns     none
*
* @comments    Caller must provide an output buffer of at least
*              OSAPI_INET_NTOA_BUF_SIZE bytes (as defined in
*              osapi.h).
*
* @end
*
*************************************************************************/
EXT_API void osapiInetNtoa ( L7_uint32 ipAddr, L7_uchar8 *buf );

/**************************************************************************
* @purpose  Lowers a task's priority to the lowest priority then returns
* @purpose  the task to it's previous priority
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE If called from interrupt context
*
* @end
*************************************************************************/
EXT_API L7_RC_t osapiTaskYield ( void );

/**************************************************************************
*
* @purpose  Get the priority for a specific task.
*
* @param    task_id      @b{(input)} id for the given task.
* @param    task_name     @b{(input)} task name.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t  osapiTaskNameGet( L7_int32 task_id, L7_char8 *task_name);

/**************************************************************************
*
* @purpose  Get the priority for a specific task.
*
* @param    task_id      @b{(input)} id for the given task.
* @param    priority     @b{(input)} task priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t  osapiTaskPriorityGet( L7_int32 task_id, L7_uint32 *priority);

/**************************************************************************
*
* @purpose  Verifies the existence of a specified task.
*
* @param    task_id   @b{(input)} task ID to be verified.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t  osapiTaskIdVerify( L7_int32 task_id);

/**************************************************************************
*
* @purpose  Get the task ID of a running task.
*
* @param    none
*
* @returns  task_id
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_int32  osapiTaskIdSelf( void );

/*********************************************************************
* @purpose  Duplicate a string given Component
*
* @param    sourceString Pointer to string to duplicate
*
* @returns  Pointer to duplicated sourceString or L7_NULLPTR if an error occured
*
* @comments none.
*
* @end
*
*********************************************************************/
L7_char8 *osapiCompStrDup(L7_COMPONENT_IDS_t compId, L7_char8 *sourceString);

/*********************************************************************
* @purpose  Duplicate a string
*
* @param    sourceString @b{(input)} Pointer to string to duplicate
*
* @returns  Pointer to duplicated sourceString or L7_NULLPTR if an error occured
*
* @comments
*
* @end
*
*********************************************************************/
EXT_API L7_char8 *osapiStrDup(L7_char8 *sourceString);

/**************************************************************************
*
* @purpose  Concatenate first n characters of one string to another
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
* @param    n           @b{(input)}  maximum number of characters to append
*
* @returns  Pointer to destination location (always)
*
* @comments The dest and src strings must not overlap.  The dest location
*           must have enough space for n additional characters.
*
* @comments Only the first n characters of src are appended to dest.  The
*           resulting dest string is always null terminated.
*
* @comments To maintain consistency with the POSIX functions, the dest
*           pointer is always returned, even if no characters were actually
*           appended.
*
* @end
*
*************************************************************************/
EXT_API L7_char8 *osapiStrncat(L7_char8 *dest, const L7_char8 *src, L7_uint32 n);

/**************************************************************************
*
* @purpose  Concatenate first n characters of one string to another
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
*
* @returns  Pointer to destination location (always)
*
* @comments Macro form of osapiStrncat API provided for notational
*           convenience.  This may only be used to replace the
*           "normal" form of function invocation, e.g.:
*               osapiStrncat(buf, pSrc, sizeof(buf)-strlen(buf)-1);
*           becomes
*               OSAPI_STRNCAT(buf, pSrc);
*
* @comments See osapiStrncat for a complete functional description.
*
* @end
*
*************************************************************************/
#define OSAPI_STRNCAT(_dst, _src) \
  osapiStrncat((_dst), (_src), sizeof((_dst))-strlen((_dst))-1)

/**************************************************************************
*
* @purpose  Compare at most n characters of two strings
*
* @param    *s1         @b{(input)}  string being compared
* @param    *s2         @b{(input)}  string s1 is compared against
* @param    n           @b{(input)}  maximum number of characters to compare
*
* @returns  <0          s1 less than s2
* @returns  0           s1 matches s2
* @returns  >0          s1 greater than s2
*
* @comments A lexicographical comparison is performed.  At most n
*           characters of s1 are compared to s2.
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiStrncmp(const L7_char8 *s1, const L7_char8 *s2, L7_uint32 n);

/**************************************************************************
*
* @purpose  Copy a string to a buffer with a bounded length
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
* @param    n           @b{(input)}  maximum number of characters to copy
*
* @returns  Pointer to destination location (always)
*
* @comments The dest and src strings must not overlap.  The dest location
*           must have enough space for n additional characters.
*
* @comments No more than n characters are copied from src to dest.  If there
*           is no '\0' character within the first n characters of src, the
*           dest string will be UNTERMINATED.  See osapiStrncpySafe()
*           for an alternative implementation.
*
* @comments If the src string length is less than n, the remainder of
*           dest is padded with nulls (total of n characters).
*
* @comments To maintain consistency with the POSIX functions, the dest
*           pointer is always returned, even if no characters were actually
*           copied.  Invalid input parameters (such as null pointers)
*           are not checked here.  See osapiStrncpySafe() for an
*           alternative implementation offering more protection.
*
* @end
*
*************************************************************************/
EXT_API L7_char8 *osapiStrncpy(L7_char8 *dest, const L7_char8 *src, L7_uint32 n);

/**************************************************************************
*
* @purpose  Copy a string to a buffer with a bounded length (with safeguards)
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
* @param    n           @b{(input)}  maximum number of characters to copy
*
* @returns  Pointer to destination location (always)
*
* @comments The dest and src strings must not overlap.  The dest location
*           must have enough space for n additional characters.
*
* @comments No more than n characters are copied from src to dest.  If there
*           is no '\0' character within the first n > 0 characters of src, the
*           n-th byte of dest string will be written with the '\0'
*           string termination character (e.g., if n=10, *(dest+9)='\0').
*
* @comments If the src string length is less than n, the remainder of
*           dest is padded with nulls.
*
* @comments To maintain consistency with the POSIX functions, the dest
*           pointer is returned, even if no characters were actually copied
*           (e.g., src string has zero length, src ptr is null, n is 0,
*           etc.)
*
* @end
*
*************************************************************************/
EXT_API L7_char8 *osapiStrncpySafe(L7_char8 *dest, const L7_char8 *src, L7_uint32 n);

/**************************************************************************
*
* @purpose  Copy a string to a buffer with a bounded length (with safeguards)
*
* @param    *dest       @b{(input)}  destination location
* @param    *src        @b{(input)}  source string ptr
*
* @returns  Pointer to destination location (always)
*
* @comments Macro form of osapiStrncpySafe API provided for notational
*           convenience.  This may only be used to replace the
*           "normal" form of function invocation, e.g.:
*               osapiStrncpySafe(buf, pSrc, sizeof(buf));
*           becomes
*               OSAPI_STRNCPY_SAFE(buf, pSrc);
*
* @comments See osapiStrncpySafe for a complete functional description.
*
* @end
*
*************************************************************************/
#define OSAPI_STRNCPY_SAFE(_dst, _src) \
  osapiStrncpySafe((_dst), (_src), sizeof((_dst)))

/**************************************************************************
*
* @purpose  Print a formatted string to a buffer with a bounded length
*
* @param    *str        @b{(input)}  destination location
* @param    n           @b{(input)}  maximum bytes to copy (including '\0')
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ...         @b{(input)}  variable arguments
*
* @returns  Number of characters printed (not including the trailing '\0'),
*           or that would have been printed if the str location had been
*           large enough (per ANSI C99).
*
* @returns  Negative value indicates an error ('errno' contains reason code).
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiSnprintf(L7_char8 *str, L7_uint32 n, L7_char8 const *fmt, ...);

/**************************************************************************
*
* @purpose  Print a formatted string to a buffer with a bounded length
*
* @param    *str        @b{(input)}  destination location
* @param    n           @b{(input)}  maximum bytes to copy (including '\0')
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ...         @b{(input)}  variable arguments
*
* @returns  Number of characters printed (not including the trailing '\0'),
*           or that would have been printed if the str location had been
*           large enough (per ANSI C99).
*
* @returns  Negative value indicates an error ('errno' contains reason code).
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiSnprintfcat(L7_char8 *str, L7_uint32 n, L7_char8 const *fmt, ...);

/**************************************************************************
*
* @purpose  Print a formatted string to a buffer with a bounded length
*           using a variable arg list
*
* @param    *str        @b{(input)}  destination location
* @param    n           @b{(input)}  maximum bytes to copy (including '\0')
* @param    *fmt        @b{(input)}  format string (like printf)
* @param    ap          @b{(input)}  variable argument list
*
* @returns  Number of characters printed (not including the trailing '\0'),
*           or that would have been printed if the str location had been
*           large enough (per ANSI C99).
*
* @returns  Negative value indicates an error ('errno' contains reason code).
*
* @comments The standard library typically returns -1 for all errors.  ANSI C99
*           permits any negative value to indicate an error, so negative
*           values other than -1 denote an error detected by OSAPI-specific
*           checking.
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiVsnprintf(L7_char8 *str, L7_uint32 n, L7_char8 const *fmt, va_list ap);

/*********************************************************************
* @purpose  Compare the first n characters of the two string.
*
* @param    char *str1, char *str2, int n
*
* @returns  L7_SUCCESS  If two string matched each other.
* @returns  L7_FAILURE  otherwise
*
* @comments none.
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t osapiStrCaseCmp(char *str1, char * str2, int n);

/*********************************************************************
*
* @purpose compare 2 buffers.
*
* @param L7_char8 *buf1
* @param L7_char8 *buf2
*
* @returns  L7_TRUE if buffers are the same.  L7_FALSE otherwise.
*
* @notes If the buffers can not be compared, L7_FALSE is returned.
* @notes The buffers must be null terminated.
*
* @end
*
*********************************************************************/
EXT_API L7_BOOL osapiNoCaseCompare(L7_char8 *buf1, L7_char8 *buf2 );

/*********************************************************************
*
* @purpose convert all letters inside a buffer (char8) to lower case
*
* @param L7_char8 *buf
*
* @returns  void
*
* @notes   This f(x) returns the same letter in the same buffer but all
*          lower case, checking the buffer for empty string
* @end
*
*********************************************************************/
EXT_API void osapiConvertToLowerCase(L7_char8 *buf);

/**************************************************************************
* @purpose  Flush all or some of a specified cache
*
* @parms    cache   Cache to flush
* @parms    address Virtual address
* @parms    bytes   Number of bytes to flush
*
* @returns  L7_SUCCESS or L7_FAILURE if the cache type is invalid or the cache control is not supported.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiCacheFlush (L7_CACHE_TYPE cache, void *address, L7_uint32 bytes);


/**************************************************************************
* @purpose  Invalidate all or some of a specified cache
*
* @parms    cache   Cache to Invalidate
* @parms    address Virtual address
* @parms    bytes   Number of bytes to flush
*
* @returns  L7_SUCCESS or L7_FAILURE if the cache type is invalid or the cache control is not supported.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiCacheInvalidate (L7_CACHE_TYPE cache, void *address, L7_uint32 bytes);

/*********************************************************************
* @purpose  Initializes pipes.
*
*
* @returns     L7_SUCCESS
* @returns     L7_ERROR driver returned an error
*
* @comments
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t osapiPipesInitialize (void);

/*********************************************************************
* @purpose  Creates a pipe.
*
* @param    name        @b{(input)} Name of pipe to be created.
* @param    nMessages   @b{(input)} Max. number of messages in pipe.
* @param    nBytes      @b{(input)} Size of each message.
*
* @returns     L7_SUCCESS
* @returns     L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/
EXT_API  L7_RC_t osapiPipeCreate(L7_char8* name, L7_int32 nMessages, L7_int32 nBytes);


/*********************************************************************
* @purpose  Opens a pipe.
*
* @param    name        @b{(input)} Name of pipe to be opened.
* @param    flag        @b{(input)} O_RDONLY, O_WRONLY, O_RDWR, or O_CREAT.
* @param    mode        @b{(input)} Mode of file to create (UNIX chmod style) )
* @param    descriptor  @b{(output)} ptr to pipe descriptor
*
* @returns     L7_SUCCESS If pipe has been opened succesfully
* @returns     L7_ERROR   If any error occurs while opening pipe
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t osapiPipeOpen(L7_char8* name, L7_int32 flags,L7_int32 mode, L7_uint32 *descriptor);

/*********************************************************************
* @purpose  Closes a pipe.
*
* @param    fd          @b{(input)} The file descriptor to close.
*
* @returns     L7_SUCCESS
* @returns     L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/
EXT_API L7_RC_t osapiPipeClose(L7_int32 fd);

/*********************************************************************
* @purpose  Reads from a pipe.
*
* @param    fd          @b{(input)} The file descriptor from which to read.
* @param    buffer      @b{(input)} Pointer to buffer to receive bytes.
* @param    mode        @b{(input)} Max no. of bytes to read into buffer.
*
* @returns      The number of bytes read (between 1 and maxbytes, 0 if end of file).
* @returns      L7_ERROR  on error
*
* @comments
*
* @end
*
*********************************************************************/
EXT_API L7_int32 osapiPipeRead(L7_int32 fd, L7_char8* buffer, L7_int32 maxbytes);


/*********************************************************************
* @purpose  Writes to a pipe.
*
* @param    fd          @b{(input)} The file descriptor on which to write.
* @param    buffer      @b{(input)} Buffer containing bytes to be written.
* @param    mode        @b{(input)} Number of bytes to write.
*
* @returns      The number of bytes written (if not equal to nbytes, an error has occurred).
* @returns      L7_ERROR  on error
*
* @comments
*
* @end
*
*********************************************************************/
EXT_API L7_int32 osapiPipeWrite(L7_int32 fd, L7_char8* buffer, L7_int32 nbytes );

/**************************************************************************
*
* @purpose  Create a read/write lock
*
* @param    rwlock    ptr to ID of the requested lock
* @param    options @b{(input)} queueing style - either Priority or FIFO
*
* @returns  ptr to the lock or NULL if lock cannot be allocated
*
* @comments None.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiRWLockCreate(osapiRWLock_t *rwlock,
                                  osapiRWLockOptions_t options);

/**************************************************************************
*
* @purpose  Take a read lock
*
* @param    rwlock    ID of the requested lock returned from osapiRWLockCreate
* @param    timeout   time to wait in milliseconds, L7_WAIT_FOREVER,
*                     or L7_NO_WAIT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if timeout or if lock does not exist
*
* @comments None.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiReadLockTake(osapiRWLock_t rwlock, L7_int32 timeout);

/**************************************************************************
*
* @purpose  Give a read lock
*
* @param    rwlock     ID of the requested lock returned from osapiRWLockCreate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if lock is invalid
*
* @comments None.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiReadLockGive(osapiRWLock_t rwlock);

/**************************************************************************
*
* @purpose  Take a write lock
*
* @param    rwlock    ID of the requested lock returned from osapiRWLockCreate
* @param    timeout   time to wait in milliseconds, L7_WAIT_FOREVER,
*                     or L7_NO_WAIT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if timeout or if lock does not exist
*
* @comments None.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiWriteLockTake(osapiRWLock_t rwlock, L7_int32 timeout);

/**************************************************************************
*
* @purpose  Give a write lock
*
* @param    rwlock     ID of the requested lock returned from osapiRWLockCreate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if lock is invalid
*
* @comments None.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiWriteLockGive(osapiRWLock_t rwlock);

/**************************************************************************
*
* @purpose  Delete a read/write lock
*
* @param    rwlock     ID of the lock to delete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if lock is invalid
*
* @comments Routine will suspend until there are no outstanding reads or writes.
*
* @end
*
*************************************************************************/
EXT_API L7_RC_t osapiRWLockDelete(osapiRWLock_t rwlock);

/**************************************************************************
*
* @purpose  Check if running task is requested task.
*
* @param    Task @b{(input)} Name of requested task
*
* @returns  L7_TRUE if this is the task requested, else L7_FALSE
*
* @comments
*
* @end
*
*************************************************************************/
EXT_API L7_BOOL  osapiTaskIdIs( L7_uchar8 *taskName);

/*********************************************************************
* @purpose  The number of L7 ICMP Echo (request) messages sent.
*
* @param    None.
*
* @returns Number of packets sent
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_uint32 osapiL7IcmpOutEchosGet(void);

/*********************************************************************
* @purpose  To Increment the number of L7 ICMP Echo (request) messages sent.
*
* @param    None.
*
* @returns None.
*
* @comments
*
* @end
*********************************************************************/
EXT_API void osapiL7IcmpOutEchosInc(void);


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
EXT_API L7_RC_t osapiStdInConfig(int fd);

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
void osapiInputTask(void);

/***************************************************************************
*
* @purpose This function lets the user choose a menu option.
*
* @comments
*
* @returns choice
*
* @end
*
***************************************************************************/
L7_uchar8 osapiGetChoice(void);

/*********************************************************************
* @purpose  Initialize the osapi monitor task
*
* @param    none.
*
* @returns  none.
*
* @comments none.
*
* @end
*********************************************************************/
void osapiMonitorInit(void);

/*********************************************************************
* @purpose  Tell the monitor task to stop monitoring for suspended tasks.
*
* @param    none.
*
* @returns  none.
*
* @comments none.
*
* @end
*********************************************************************/
void osapiTaskStateMonitorOff(void);

/*********************************************************************
* @purpose  Tell the monitor task to resume monitoring for suspended tasks.
*
* @param    none.
*
* @returns  none.
*
* @comments none.
*
* @end
*********************************************************************/
void osapiTaskStateMonitorOn(void);

/**************************************************************************
 *
 * @purpose  Populate registry with the software version
 *
 * @param    none
 *
 * @returns  none
 *
 * @comments Sets version to "Unknown" if able to determine version
 *
 * @end
 *
 *************************************************************************/
void osapiSwVersionRegister(void);

/**************************************************************************
 *
 * @purpose  Populate registry with OS version
 *
 * @param    none
 *
 * @returns  none
 *
 * @comments none
 *
 * @end
 *
 *************************************************************************/
void osapiOSVersionRegister(void);

/*********************************************************************
* @purpose  Returns the address of a given function
*
* @param    funcName, a string
*
* @returns  L7_uint32 with address of function if it succeeds
*           0 if it fails
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_uint32 osapiAddressLookup(L7_char8 *funcName);

/*********************************************************************
* @purpose  Returns the function containing a given address
*
* @param    addr, L7_uint32
*           funcName, L7_char8* buffer allocated by caller
*           funcNameLen, L7_uint32 length of name buffer
*           offset, L7_uint32* pointer to L7_uint32 allocated by caller.
*                   Address's offset from beginning of function stored there.
*
* @returns  L7_SUCCESS if function found, name printed into funcName buffer,
*                      offset set into *offset.
*           L7_FAILURE if function not found, buffers untouched.
*
* @comments
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiFunctionLookup(L7_uint32 addr, L7_char8 *funcName,
				    L7_uint32 funcNameLen, L7_uint32 *offset);


/**************************************************************************
*
* @purpose  convert string to long unsigned integer
*
* @param    *s1         @b{(input)}  Points to a character string for strtoul() to convert.
* @param    *s2         @b{(input)}  Is a result parameter that, if not NULL, is
*                       loaded with the address of the first character that strtoul()
*                       does not attempt to convert
* @param    n           @b{(input)}  Is the base of the string, a value between 0 and 36.
*
* @returns  value       converted value
* @returns  0           Error
*
* @end
*
*************************************************************************/
EXT_API L7_int32 osapiStrtoul(const L7_char8 *s1, L7_uint32 base, L7_uint32 *seqNum);

/**************************************************************************
*
* @purpose  Returns the number of characters in the string pointed to by s, 
*                 not including the terminating '\0' character, but at most maxlen.
*
* @param    *s         @b{(input)}  Points to a character string.
* @param    maxlen  @b{(input)}  Maximum length of the string.
*
* @returns  length of s (max = maxlen)       
*
* @end
*
*************************************************************************/
EXT_API L7_uint32 osapiStrnlen(const L7_char8 *s, L7_uint32 maxlen);

/*********************************************************************
* @purpose  Returns the name of the task whose stack guard page
*           the specified address is in (if any), for debugging
*           stack overflow segmentation faults.
*
* @param    addr, L7_uint32
*           buf, L7_char8* buffer allocated by caller
*           bufSize, L7_uint32 length of name buffer
*
* @returns  L7_SUCCESS if task found, name printed into buffer.
*           L7_FAILURE if task not found, buffers untouched.
*
* @comments
*           Unimplemented on VxWorks.
*
* @end
*********************************************************************/
EXT_API L7_RC_t osapiWhichStack(L7_uint32 addr, L7_char8 *buf,
				    L7_uint32 bufSize);

/****** Serviceability ******/
#ifdef LVL7_DEBUG_BREAKIN
/***********************************************************************
*@purpose  Enable/Disable console break-in
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
L7_RC_t osapiConsoleBreakinModeSet(L7_BOOL enable_flag);

/*********************************************************************
* @purpose  Gets the console break-in mode setting for selected session.
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
L7_BOOL osapiConsoleBreakinModeGet(void);
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
L7_RC_t  osapiConsoleBreakinstringGet(L7_char8 *password);

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
L7_RC_t  osapiConsoleBreakinstringSet(L7_char8 *password);
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
L7_RC_t osapiCpuUtilizationStringGet (L7_char8 *cpuUtilBuffer, L7_uint32 bufferSize);

#ifndef _L7_OS_LINUX_
/*
* Task utilization monitoring  and reporting defines
*/

/*********************************************************************
*
* @structures LVL7TaskPercentageTable
*
* @purpose    Store task names and utilization info from Spy.
*
* @comments
*
*********************************************************************/
typedef struct
{
  L7_char8  taskName[36];
  double    taskPercentage;
} LVL7TaskPercentageTable;

#define AUX_TMR_BASE INTERNAL_MEM_MAP_ADDR

#else /* not Linux above, Linux below */

#ifdef L7_LINUX_24
#define osapiGetpid()  (unsigned long) getpid()
#else
#ifdef __powerpc__
#define osapiGetpid()  (unsigned long) syscall(207) /* __NR_gettid */
#else
#ifdef __mips__
#define osapiGetpid()  (unsigned long) syscall(4222) /* __NR_gettid */
#else
#ifdef __i386__
#define osapiGetpid()  (unsigned long) syscall(224) /* __NR_gettid */
#else
/* PTin added: ARM processor */
#ifdef __arm__
#define osapiGetpid()  (unsigned long) syscall(224) /* __NR_gettid */
#else
#define osapiGetpid()  (unsigned long) gettid()
#endif /* arm */
#endif /* i386 */
#endif /* mips */
#endif /* powerpc */
#endif /* Linux 2.4 */

#endif /* _L7_OS_LINUX_ */

void osapiMiscInit(void);
void osapiDevShellSet(L7_uint32 (*func)(L7_char8 *));
L7_uint32 osapiDevShellExec(L7_char8 *cmd);
/*********************************************************************
* @purpose  Get the total memory allocated and free in bytes.
*
* @param    numBytesAlloc Pointer to integer to return number of bytes allocated
* @param    numBytesFree  Pointer to integer to return number of bytes free
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t osapiGetMemInfo(L7_uint32 *numBytesTotal, L7_uint32 *numBytesAlloc, L7_uint32 *numBytesFree);

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
                                     L7_uchar8 *taskPercentString, L7_uint32 taskPercentSize);

/*********************************************************************
* @purpose  Get the next task ID.
*
* @param    processIndex
* @param    taskId
* @param    taskIdSize
*
* @returns  
*
* @end
*
*********************************************************************/
L7_RC_t osapiTaskIdGet (L7_uint32 processIndex, L7_char8 *taskIdString,
                        L7_uint32 taskIdSize);

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
L7_RC_t osapiTotalCpuUtilGet (L7_char8 *taskPercentString, L7_uint32 taskPercentSize);

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
void osapiLogStackTraceFlash(L7_char8 *fileName, L7_char8 * format, ...);


#ifdef _L7_OS_LINUX_
/**************************************************************************
*
* @purpose  Get the PID for a specific task.
*
* @param    task_id      @b{(input)} id for the given task.
*
* @returns  PID
*
* @comments
*
* @end
*
*************************************************************************/
int osapiTaskPidGet( L7_int32 task_id);
#endif

/*********************************************************************
* @purpose  Task that measures system-wide task utilization.
*
* @param    numArgs @b{(input)}  Number of arguments passed from caller
* @param    argv    @b{(input)}  pointer to arguments passed from caller
*
* @returns  none
*
* @end
*
*********************************************************************/
void osapiCpuUtilMonitorTask(L7_ulong32 numArgs, L7_uint32 *argv );
/**************************************************************************
*
* @purpose  To convert a formatted time string to tm struct
*
* @param  buf    Is the buffer containing the time specification.
* @param  format Is the format specification for the conversion.
* @param  tm     Points to the resulting converted time.
*
*
* @returns  Upon successful conversion returns a pointer to the character
            following the last character parsed
*           Fail case upon error a null pointer is returned.
*
* @comments Check strptime()
*
* @end
*
*************************************************************************/
L7_uchar8* osapiStrptime(const L7_uchar8* s, const L7_uchar8* format, struct tm* tm);
#ifdef _L7_OS_ECOS_
/*********************************************************************
* @purpose  Get the available free memory of the system.
*
* @param  none 
*
* @returns available free memory 
*
* @end
*
*********************************************************************/

L7_uint32 osapiGetAvailableFreeMemory(void);

/*********************************************************************
* @purpose  Get the largest free block available in the system.
*
* @param    none
*
* @returns  largest available free block size
*
* @end
*
*********************************************************************/
L7_uint32 osapiGetFreeLargeMemoryBlockSize(void); 

#endif

/**************************************************************************
 *
 * @purpose  Copy a string to a buffer with a bounded length
 *
 * @param    *dest       @b{(input)}  destination location
 * @param    *src        @b{(input)}  source string ptr
 *
 * @returns  If src is an empty string, dest is returned;
 *           If src occurs nowhere in dest, NULL is returned;
 *           If src occurs in dest, a pointer to the first character of the
 *                                  first occurrence of src is returned.
 *
 * @comments The dest and src strings must not overlap.  The dest location
 *           must have enough space for n additional characters.
 *
 * @comments No more than n characters are copied from src to dest.  If there
 *           is no '\0' character within the first n characters of src, the
 *           dest string will be UNTERMINATED.  See osapiStrncpySafe()
 *           for an alternative implementation.
 *
 * @comments If the src string length is less than n, the remainder of
 *           dest is padded with nulls (total of n characters).
 *
 * @comments To maintain consistency with the POSIX functions, the dest
 *           pointer is always returned, even if no characters were actually
 *           copied.  Invalid input parameters (such as null pointers)
 *           are not checked here.  See osapiStrncpySafe() for an
 *           alternative implementation offering more protection.
 *
 * @end
 *
 * *************************************************************************/
L7_char8 *osapiStrStr(L7_char8 *dest, const L7_char8 *src);

/**************************************************************************
*
* @purpose  The function shall test whether the value is a lowercase letter
*
* @param    c       @b{(input)}  A character
*
* @returns  non-zero value if c is a lowercase letter; 
*           otherwise, it returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsLower(L7_int32 c);

/**************************************************************************
*
* @purpose  The function shall test whether the value is a uppercase letter
*
* @param    c       @b{(input)}  A character
*
* @returns  non-zero value if c is a uppercase letter; 
*           otherwise, it returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsUpper(L7_int32 c);

/**************************************************************************
*
* @purpose  The function shall test whether the value is a decimal number
*
* @param    c       @b{(input)}  A number
*
* @returns  non-zero value if c is a decimal number; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsDigit(L7_int32 c);


/**************************************************************************
*
* @purpose  The function shall test whether the value is a printable character
*
* @param    c       @b{(input)}  A Character
*
* @returns  non-zero value if c is a printable character; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsPrint(L7_int32 c);


/**************************************************************************
*
* @purpose  The function shall test whether the value is a special character
*
* @param    c       @b{(input)}  A character
*
* @returns  non-zero value if c is a special character; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsSpecial(L7_int32 c);

/**************************************************************************
*
* @purpose  The function shall test whether the value is a alphanumeric 
*           character
*
* @param    c       @b{(input)}  A Character
*
* @returns  non-zero value if c is a alphanumeric character; 
*           otherwise, returns 0.       
*
* @end
*
*************************************************************************/
L7_int32 osapiIsAlnum(L7_int32 c);


/**************************************************************************
*
* @purpose  Initialize memory to create a allocation log that can record malloc
*           details of specified number of entries
*
* @param    numEntries @b{(input)} Number of entries to store. 0 indicates 
*                                  default.
*
* @returns  L7_SUCCESS 
*           L7_FAULURE 
*
* @comments 
*
* @end
*
*************************************************************************/
L7_RC_t osapiMallocPersistentLogBufferInit(L7_uint32 numEntries);


/**************************************************************************
*
* @purpose  Write the malloc allocation details to persistent memory (flash).
*           osapiInitMemoryAllocLog must be called before calling this routine.
*           Other parameters can be set using osapiMallocPersistentLogParamsSet.
*
* @param    none
*
* @returns  none
*
* @comments 
*
* @end
*
*************************************************************************/
L7_RC_t osapiGenerateMallocPersistentLog(void);

#endif /* OSAPIPROTO_HEADER */

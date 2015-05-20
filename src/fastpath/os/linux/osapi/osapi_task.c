/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_task.c
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"

#include <signal.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>

#include "osapi_priv.h"
#include "osapi_support.h"

/* taken from VxWorks OSAPI implementation */
static void *syncSemaArray[L7_TASK_SYNC_LAST] = {L7_NULL};

pthread_key_t osapi_task_key;
pthread_key_t osapi_name_key;
pthread_key_t osapi_signal_key;

static pthread_mutex_t task_list_lock = PTHREAD_MUTEX_INITIALIZER;
static osapi_task_t *task_list_head = NULL;

static pthread_mutex_t zombie_tasks_lock = PTHREAD_MUTEX_INITIALIZER;
static osapi_task_t *zombie_tasks_list = NULL;
static pthread_cond_t zombie_tasks_cond;
static pthread_mutex_t zombie_tasks_cond_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t stack_monitor_thread;

static pthread_once_t osapi_task_once = PTHREAD_ONCE_INIT;

#if 1 /* osapiTaskLock() is now deprecated */
static pthread_mutex_t osapi_task_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t osapi_task_lock_cond = PTHREAD_COND_INITIALIZER;
static osapi_task_t *osapi_task_lock_owner = NULL;
static L7_uint32 osapi_task_lock_depth = 0;
#endif

#if 1 /* osapiDisableInts() is now deprecated */
static pthread_mutex_t osapi_task_int = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t osapi_task_int_cond = PTHREAD_COND_INITIALIZER;
static osapi_task_t *osapi_task_int_owner = NULL;
static L7_uint32 osapi_task_int_depth = 0;
#endif

static L7_uint32 osapi_task_max_fifo_prio, osapi_task_min_fifo_prio,
                 osapi_task_fifo_prio_range, osapi_task_fifo_prio_base,
                 osapi_task_lock_fifo_prio;
static L7_uint32 osapi_task_max_rr_prio, osapi_task_min_rr_prio,
                 osapi_task_rr_prio_range, osapi_task_rr_prio_base,
                 osapi_task_lock_rr_prio;

static L7_uint32 system_page_size = 0;

#define OSAPI_INT_LEVELS  1

#ifdef L7_STACK_USAGE_STATS
L7_uint32 osapiStackFree(osapi_task_t *task)
{
  L7_uint32 *tst;
  L7_uint32 stack_usage;

  if (task==NULL)
  {
    return(0);
  }
  stack_usage = task->stack_size;
  tst = (L7_uint32 *) (task->stack_base_addr);
  while ((stack_usage > 0) && (*tst == 0xD7D7D7D7)) {
    stack_usage -= 4;
    tst++;
  }
  return(task->stack_size - stack_usage);
}
#endif /* L7_STACK_USAGE_STATS */

int osapiDebugStackConsume(int levels)
{
  char buf[128];

  buf[0] = 0;
  if (levels > 0) 
  {
    printf("Calling self\n");
    osapiDebugStackConsume(levels - 1);
  }
  return(0);
}

static void osapi_task_destroy(osapi_task_t *task)
{
  int rc;

  pthread_mutex_destroy(&(task->lock));
  pthread_cond_destroy(&(task->control));
  pthread_cond_destroy(&(task->fifo_cond));
  pthread_attr_destroy(&(task->attr));

  rc = munmap((void *)(task->stack_base_addr - system_page_size),
      (task->stack_size + system_page_size));
  if (rc < 0)
  {
    osapi_printf("osapi_task_destroy: Error %d freeing stack of task %s\n", errno, task->name);
  }

  osapiFree(L7_OSAPI_COMPONENT_ID, task);
}

/* This task has two purposes: 
   1) Finish the reaping of tasks that called osapiTaskDelete() upon
      themselves, since you can't free up the stack until the thread is 
      no longer running.
   2) Watch for tasks to get low on stack space, if stack usage stats are 
      enabled. 
*/
void *osapi_stack_monitor_task(void *arg) 
{
  int rc;
  osapi_task_t *zombie, *dying;
  struct timespec tp;
#if (!defined(_POSIX_CLOCK_SELECTION) || defined(L7_PTHREAD_CONDATTR_SETCLOCK_BROKEN))
  struct timeval tv;
#endif
#ifdef L7_STACK_USAGE_STATS
  L7_uint32 oldest_time, cur_free, now;
  osapi_task_t *checking, *oldest;
#endif

  while (1) 
  {
    /* Sleep for 10 seconds or until signalled */
#if (defined(_POSIX_CLOCK_SELECTION) && (!defined(L7_PTHREAD_CONDATTR_SETCLOCK_BROKEN)))
    rc = clock_gettime(CLOCK_MONOTONIC, &tp);
#else
    rc = gettimeofday(&tv, NULL);
    tp.tv_sec = tv.tv_sec;
    tp.tv_nsec = tv.tv_usec * 1000;
#endif
    if (rc == 0) {
      tp.tv_sec += 10;
      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                                           (void *)&zombie_tasks_cond_lock);
      pthread_mutex_lock(&zombie_tasks_cond_lock);
      pthread_cond_timedwait(&zombie_tasks_cond, &zombie_tasks_cond_lock, 
                             &tp);
      pthread_cleanup_pop(1);
    } else {
      sleep(10); /* safety in case of error */
    }

    /* Check for tasks that killed themselves, then free their resources.
       Can't free the stack from a thread's own context, even while
       exiting. */
    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                                           (void *)&zombie_tasks_lock);
    if (pthread_mutex_lock(&zombie_tasks_lock) == 0)
    {
      zombie = zombie_tasks_list;
      while(zombie != NULL)
      {
        if (zombie->chain_prev != NULL) 
        {
          zombie->chain_prev->chain_next = zombie->chain_next;
        } 
        else 
        {
          zombie_tasks_list = zombie->chain_next;
        }
        if (zombie->chain_next != NULL) 
        {
          zombie->chain_next->chain_prev = zombie->chain_prev;
        }

        dying = zombie;
        zombie = zombie->chain_next;
        pthread_cancel(dying->thread);
        pthread_join(dying->thread, (void **)NULL);
        /* At this point the target task has exited; free stuff */
        osapi_task_destroy(dying);
      }
    }

    pthread_cleanup_pop(1);

#ifdef L7_STACK_USAGE_STATS
    /* Find the task that has had its stack usage checked the least 
       recently, check it, and report if too low. */
    /* Lock the task list for walking. Leave it locked for the check, 
       to make sure the task struct and stack are not freed out from 
       under us. */
    now = osapiUpTimeRaw();
    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&task_list_lock);
    if (pthread_mutex_lock(&task_list_lock) == 0)
    {
      checking = task_list_head;
      oldest_time = 0xFFFFFFFF;
      oldest = NULL;
      while ((checking != NULL) && (oldest_time > 0)) 
      {
        if (checking->stack_usage_read_time < oldest_time) 
        {
          oldest_time = checking->stack_usage_read_time;
          oldest = checking;
        }
        checking = checking->chain_next;
      }
      if (oldest != NULL)
      {
        cur_free = osapiStackFree(oldest);
        oldest->stack_usage_read_time = now;
        if (cur_free < 1024)
        {
          printf("\n*** WARNING: Task %s has dangerously low free stack space (%d bytes)\n", 
                 oldest->name, cur_free);
          /* We can't use log functions because they depend on our 
             osapiTask structures, which we don't have as we're started 
             manually with pthread_create(). */
        }
      }
    }

  pthread_cleanup_pop(1);
#endif
  }
  return NULL;
}

static void osapi_task_init(void)
{
  pthread_attr_t monitor_attr;
  struct sched_param monitor_schedparam;
  pthread_condattr_t cat;
  int rc;

  osapi_task_max_fifo_prio = sched_get_priority_max(SCHED_FIFO);

  osapi_task_min_fifo_prio = sched_get_priority_min(SCHED_FIFO);

  osapi_task_lock_fifo_prio = osapi_task_max_fifo_prio - OSAPI_INT_LEVELS;

  osapi_task_fifo_prio_base = osapi_task_min_fifo_prio;

  osapi_task_fifo_prio_range = ((osapi_task_lock_fifo_prio
                                 - osapi_task_min_fifo_prio) - 1);

  osapi_task_max_rr_prio = sched_get_priority_max(SCHED_RR);

  osapi_task_min_rr_prio = sched_get_priority_min(SCHED_RR);

  osapi_task_lock_rr_prio = osapi_task_max_rr_prio - OSAPI_INT_LEVELS;

  osapi_task_rr_prio_base = osapi_task_min_rr_prio;

  osapi_task_rr_prio_range = ((osapi_task_lock_rr_prio
                               - osapi_task_min_rr_prio) - 1);

  pthread_key_create(&osapi_task_key, NULL);
  pthread_key_create(&osapi_name_key, NULL);
  pthread_key_create(&osapi_signal_key, NULL);

  system_page_size = sysconf(_SC_PAGESIZE);

  pthread_condattr_init(&cat);
#if (defined(_POSIX_CLOCK_SELECTION) && (!defined(L7_PTHREAD_CONDATTR_SETCLOCK_BROKEN)))
  pthread_condattr_setclock(&cat, CLOCK_MONOTONIC);
#endif
  pthread_cond_init(&zombie_tasks_cond, &cat);
  /* Start thread that watches for tasks to be deleted, so it can free
     their stacks. */
  pthread_attr_init(&monitor_attr);
  pthread_attr_setstacksize(&monitor_attr, PTHREAD_STACK_MIN + 32768);
  pthread_attr_setinheritsched(&monitor_attr, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&monitor_attr, SCHED_RR);
  monitor_schedparam.sched_priority = osapi_task_max_rr_prio;
  pthread_attr_setschedparam(&monitor_attr, &monitor_schedparam);
  rc = pthread_create(&stack_monitor_thread, &monitor_attr,
                      osapi_stack_monitor_task, NULL);
  if (rc != 0)
  {
    osapi_printf("osapi_task_init(): Fatal error %d creating stack monitor thread\n", rc);
    LOG_ERROR(rc);
  }  
}

static L7_uint32 osapi_scale_prio(L7_uint32 priority, int policy)
{

  L7_uint32 prio;

  /* Given that scheduling policies may have different priority levels,
     it seems likely that the interaction between threads scheduled w/
     different policies may be unclear.  Therefore, it is recommended
     that all OSAPI tasks be run either with a time slice or without,
     but not a combination of the two. */

  /* If OSAPI piorities used are close together, it may be better to
     map them directly within a range of Pthread priorities.  The
     scaling method used here could cause numerically close (but distinct)
     OSAPI priorities to map to the same exact OSAPI priority, which may have
     undesirable scheduling effects. */

  /* Pthread priorities are such that threads with numerically higher
     priority values are scheduled first.  This is in contrast to the VxWorks
     (and OSAPI) model which schedules numerically lower priorities first. */

  if (policy == SCHED_FIFO)
  {
    prio = osapi_task_fifo_prio_base + osapi_task_fifo_prio_range
           - ((priority * osapi_task_fifo_prio_range) / OSAPI_MAX_PRIO);
  }
  else
  {
    prio = osapi_task_rr_prio_base + osapi_task_rr_prio_range
           - ((priority * osapi_task_rr_prio_range) / OSAPI_MAX_PRIO);
  }

  return(prio);

}

void *osapi_task_wrapper(void *arg)
{
  osapi_task_t *task = (osapi_task_t *)arg;

  task->PID = osapiGetpid();

  pthread_setspecific(osapi_task_key, (void *)task);
  pthread_setspecific(osapi_name_key, (void *)(task->name));
  pthread_setspecific(osapi_signal_key, (void *)&(task->signal_lock));

  (*((void (*)(int, void *))(task->entry)))(task->argc, task->argv);

  /*
  Remove this task from task list
  */
  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                                           (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
          osapi_printf("osapi_task_wrapper: task_list_lock error\n");
  }

  if (task->chain_next != NULL)
  {
        task->chain_next->chain_prev = task->chain_prev;
  }

  if (task->chain_prev != NULL)
  {
        task->chain_prev->chain_next = task->chain_next;
  }
  else
  {
        /* must be head of list */

        task_list_head = task->chain_next;
  }

  pthread_cleanup_pop(1);

  /* Put this task on a list to have its stack freed by the 
     stack monitor task */  
  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&zombie_tasks_lock);
  if (pthread_mutex_lock(&zombie_tasks_lock) != 0)
  {
    osapi_printf("osapiTaskDelete: zombie_tasks_lock error\n");
  }
  
  if (zombie_tasks_list != NULL)
  {
    zombie_tasks_list->chain_prev = task;
  }
  task->chain_next = zombie_tasks_list;
  task->chain_prev = NULL;
  zombie_tasks_list = task;
  
  pthread_cleanup_pop(1);
  /* Wake up stack monitor task to wait for our death throes, and 
     then call osapi_task_destroy. */
  pthread_cond_signal(&zombie_tasks_cond);
  /* Wait here for that task to reap us */
  while(1) 
  {
    sleep(600);
  }
  return(NULL);
}

/**************************************************************************
*
* @purpose  Create a task.
*
* @param    task_name   @b{(input)} name to give the task.
* @param    task_entry  @b{(input)} function pointer to begin the task.
* @param    argc        @b{(input)} number of arguments.
* @param    argv        @b{(input)} pointer to start of list for arguments.
* @param    stack_size  @b{(input)} number of bytes for the stack.
* @param    priority    @b{(input)} task priority.
* @param    time_slice  @b{(input)} flag to allow time slicing for the task.
*
* @returns  task ID or L7_ERROR.
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_int32  osapiTaskCreate( L7_char8 *task_name,
                           void *task_entry,
                           L7_uint32 argc,
                           void *argv,
                           L7_uint32 stack_size,
                           L7_uint32 priority,
                           L7_uint32 time_slice )
{

  L7_uint32 namelen;
  char *namePtr;
  osapi_task_t *newTask;
  int rc;
  L7_int32 task_id;
  pthread_mutexattr_t attr;
  void *alloced_stack;
  
  pthread_once(&osapi_task_once, osapi_task_init);

  if (task_name == NULL)
  {
    namelen = 10;
  } 
  else
  {
    namelen = strlen(task_name) + 1;
  }

  if ((newTask = (osapi_task_t *)osapiMalloc(L7_OSAPI_COMPONENT_ID, sizeof(osapi_task_t) + namelen)) == NULL)
  {
    return(L7_ERROR);
  }

  task_id = (L7_int32) newTask;
  namePtr = ((char *)newTask) + sizeof(osapi_task_t); 
  newTask->name = namePtr;

  if (task_name == NULL)
  {
    snprintf(namePtr, namelen, "t%08x", (unsigned int)newTask);
  }
  else
  {
    strncpy(namePtr, task_name, namelen);
  }

  if (stack_size < PTHREAD_STACK_MIN) {
    stack_size = PTHREAD_STACK_MIN;
  }
  /* Make stack sizes that work for VxWorks more palatable, by 
     compensating for the TLS area, and osapi_task_wrapper() and 
     predecessors' stack usage. */
  stack_size += 4096;
  newTask->flags = (time_slice == 0) ? 0 : TASK_TIME_SLICED;

  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
  pthread_mutex_init(&(newTask->lock), (pthread_mutexattr_t *)&attr);
  pthread_cond_init(&(newTask->control), NULL);

  newTask->osapi_prio = priority;
  newTask->entry = task_entry;
  newTask->argc = argc;
  newTask->argv = argv;
  newTask->waiting = NULL;
  newTask->fifo_prev = newTask->fifo_next = NULL;
  newTask->pthread_prio = osapi_scale_prio(priority, ((time_slice == 0)
                                                      ? SCHED_FIFO : SCHED_RR));
  newTask->cur_prio = newTask->pthread_prio;
  newTask->signal_lock = 0;

  pthread_cond_init(&(newTask->fifo_cond), NULL);
  pthread_attr_init(&(newTask->attr));

  pthread_attr_getschedparam(&(newTask->attr), &(newTask->param));
  /* Tasks that have default priority use SCHED_OTHER policy. This allows
  ** tasks to time slice with other user and kernel tasks.
  */
  if (priority == L7_DEFAULT_TASK_PRIORITY)
  {
    pthread_attr_setschedpolicy(&(newTask->attr), SCHED_OTHER);
  }
  else
  {
    pthread_attr_setinheritsched(&(newTask->attr),PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&(newTask->attr),
                                ((time_slice == 0) ? SCHED_FIFO : SCHED_RR));
  };
  newTask->param.sched_priority = newTask->pthread_prio;
  pthread_attr_setschedparam(&(newTask->attr), &(newTask->param));

  /* Set up stack */
  /* Stack must be a whole number of pages */
  if ((stack_size & (~(system_page_size - 1))) != stack_size) {
    stack_size += system_page_size;
    stack_size &= ~(system_page_size - 1);
  } 
  alloced_stack = mmap(NULL, stack_size + system_page_size, 
                       PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 
                       -1, 0);
  if (alloced_stack == MAP_FAILED)
  {
    osapi_printf("osapiTaskCreate: Could not allocate memory for stack, error %d\n", 
                 errno);
    osapiFree(L7_OSAPI_COMPONENT_ID, (void *)newTask);
    return(L7_ERROR);
  }
  /* Set first page of stack (we asked for +1) to be inaccessible, so that 
     stack overflows will segfault */
  if (mprotect((void *)alloced_stack , system_page_size, PROT_NONE) < 0)
  {
    osapi_printf("osapiTaskCreate: Could not set stack guard page at 0x%08X inaccessible, error %d\n", 
                 (unsigned int)(((char *)alloced_stack) + stack_size), errno);
    /* nonfatal */
  }
  alloced_stack = (void *)(((char *)alloced_stack) + system_page_size);
  rc = pthread_attr_setstack(&(newTask->attr), alloced_stack, stack_size);
  if (rc != 0)
  {
    osapi_printf("osapiTaskCreate: Could not set stack address / size, error %d\n", 
                 rc);
    munmap((void *)(((char *)alloced_stack) - system_page_size),
           (stack_size + system_page_size));
    osapiFree(L7_OSAPI_COMPONENT_ID, (void *)newTask);
    return(L7_ERROR);
  }

  newTask->stack_base_addr = (unsigned long)alloced_stack;
  newTask->stack_size = stack_size;
#ifdef L7_STACK_USAGE_STATS
  memset(alloced_stack, 0xD7, stack_size);
#endif /* L7_STACK_USAGE_STATS */

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
    osapi_printf("osapiTaskCreate: task_list_lock error\n");
  }

  if (task_list_head != NULL)
  {
    task_list_head->chain_prev = newTask;
  }

  newTask->chain_next = task_list_head;

  task_list_head = newTask;

  newTask->chain_prev = NULL;

  pthread_cleanup_pop(1);

  rc = pthread_create(&(newTask->thread), &(newTask->attr),
                      (void * (*)(void *))osapi_task_wrapper, (void *)newTask);

  if (rc != 0)
  {
    osapi_printf ("osapiTaskCreate: Error creating task 0x%08X [%s], error %d\n", (unsigned int)task_id, namePtr, rc);
    
    /* Remove the task structure from the list of tasks */
    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&task_list_lock);
    if (pthread_mutex_lock(&task_list_lock) != 0)
    {
      osapi_printf("osapiTaskCreate cleanup: task_list_lock error\n");
    }
    if (newTask->chain_next != NULL)
    {
      newTask->chain_next->chain_prev = newTask->chain_prev;
    }
    if (newTask->chain_prev != NULL)
    {
      newTask->chain_prev->chain_next = newTask->chain_next;
    }
    else
    { /* must be head of list */
      task_list_head = newTask->chain_next;
    }
    pthread_cleanup_pop(1);

    /* Free the stack and task structure */
    osapi_task_destroy(newTask);
    
    return(L7_ERROR);
  }

  LOG_PT_NOTICE(LOG_CTX_STARTUP, "New pthread created: \"%s\" -> pthread_id=%u (task_id=%u)",
             namePtr, newTask->thread, (unsigned int)task_id);

  return(task_id);

}

/**************************************************************************
*
* @purpose  Delete a task.
*
* @param    task_id @b{(input)}  handle for the task to be deleted.
*
* @returns  none.
*
* @comments    none.
*
* @end
*
*************************************************************************/

void osapiTaskDelete( L7_int32 task_id)
{

  int rc, rc2 = 0;
  L7_uint32 delete_safe;
  osapi_task_t *self;
  osapi_task_t *osapiTask = (osapi_task_t *)task_id;
  struct osapi_waitq_s *queue = NULL;

  /* need task scheduling lock? */

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&(osapiTask->lock));
  if (pthread_mutex_lock(&(osapiTask->lock)) != 0)
  {
    osapi_printf("osapiTaskDelete: osapiTask->lock error\n");
  }

  delete_safe = ((osapiTask->flags & TASK_DELETE_SAFE) != 0)
                ? L7_TRUE : L7_FALSE;

  self = pthread_getspecific(osapi_task_key);

  if ((delete_safe == L7_TRUE) && (self != osapiTask))
  {
    do
    {
      pthread_cond_wait(&(osapiTask->control), &(osapiTask->lock));

    } while ((osapiTask->flags & TASK_DELETE_SAFE) != 0);

    delete_safe = L7_FALSE;
  }

  if (delete_safe == L7_FALSE)
  {
  /*
  Mark task as having been deleted
  */

    osapiTask->flags |= TASK_DELETED;

    if (osapiTask->waiting != NULL)
    {
    /*
    Lower count since we are deleting this task
    */

    osapiTask->waiting->count--;

    queue = osapiTask->waiting;

    }
  }

  /* osapiTask->lock cannot be locked when task is deleted */
  pthread_cleanup_pop(1);

  /*
  Here we had to release the task semaphore, or there was a
  deadlock situation (inverted semaphores) when getting the
  queue lock before removing the task from the waitq

  So here, we get the queue lock first, and then the task
  lock, exactly as it is done in osapi_waitq_enqueue
  */

  if (queue)
  {
      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                           (void *)queue->lock);
    if ((rc = pthread_mutex_lock(queue->lock)) != 0)
    {
      osapi_printf("osapiTaskDelete: queue->lock error, rc = %d, can't delete!\n", rc);
    }
    else
    {
      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                 (void *)&(osapiTask->lock));
      if ((rc2 = pthread_mutex_lock(&(osapiTask->lock))) != 0)
      {
        osapi_printf("osapiTaskDelete: osapiTask->lock error, rc2 = %d 2\n",
          rc2);
      }
      else
      {
        /*
        Here the task will be removed from the waitq if necessary;
        It is possible that the task will already be off the queue
        when this runs, but osapi_waitq_remove checks for that
        */

        osapi_waitq_remove(queue, osapiTask);

        pthread_mutex_unlock(&(osapiTask->lock));
      }

      pthread_cleanup_pop(0); /* osapiTask->lock */

      pthread_mutex_unlock(queue->lock);
    }

    pthread_cleanup_pop(0); /* queue->lock */

    if (rc != 0 || rc2 != 0)
    {
      /*
      If there were semaphore errors (i.e. deadlock would occur),
      then don't do the task delete; this is still a terminal
      condition, but at least the switch won't lock up if this
      occurs
      */

      return;
    }
  }

  if (delete_safe == L7_FALSE)
  {

    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&task_list_lock);
    if (pthread_mutex_lock(&task_list_lock) != 0)
    {
      osapi_printf("osapiTaskDelete: task_list_lock error\n");
    }

    if (osapiTask->chain_next != NULL)
    {
      osapiTask->chain_next->chain_prev = osapiTask->chain_prev;
    }

    if (osapiTask->chain_prev != NULL)
    {
      osapiTask->chain_prev->chain_next = osapiTask->chain_next;
    }
    else
    { /* must be head of list */
      task_list_head = osapiTask->chain_next;
    }

    pthread_cleanup_pop(1);

    if (self == osapiTask)
    {
      /* Put this task on a list to have its stack freed by the 
         stack monitor task */  
      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                           (void *)&zombie_tasks_lock);
      if (pthread_mutex_lock(&zombie_tasks_lock) != 0)
      {
        osapi_printf("osapiTaskDelete: zombie_tasks_lock error\n");
      }
  
      if (zombie_tasks_list != NULL)
      {
        zombie_tasks_list->chain_prev = osapiTask;
      }
      osapiTask->chain_next = zombie_tasks_list;
      osapiTask->chain_prev = NULL;
      zombie_tasks_list = osapiTask;
      
      pthread_cleanup_pop(1);
      /* Wake up stack monitor task to wait through our death throes, and 
         then call osapi_task_destroy. */
      pthread_cond_signal(&zombie_tasks_cond);
      /* Wait here for that task to kill and reap us */
      while(1) 
      {
        sleep(600);
      }
    }
    else
    {
      pthread_cancel(osapiTask->thread);
      pthread_join(osapiTask->thread, (void **)NULL);
      osapi_task_destroy(osapiTask);
    }
  }

  /* need task scheduling unlock? */

}


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

void osapiTaskSignal( L7_int32 task_id, int sig)
{
  osapi_task_t *osapiTask = (osapi_task_t *)task_id;

  pthread_kill(osapiTask->thread, sig);
  
  /* need task scheduling unlock? */

}

/**************************************************************************
*
* @purpose  Signals to a waiting task that this task has completed initialization
*
* @param    syncHandle  handle of the giving task
*
* @returns  L7_SUCCESS if a task is waiting on the syncHandle
* @returns  L7_FAILURE if a task is not waiting on the syncHandle
*
* @comments    none.
*
* @end
*
*************************************************************************/

L7_RC_t osapiTaskInitDone(L7_uint32 syncHandle)
{
  L7_RC_t rc;
  void *syncSema;

  /* Ensure waiting task has set up sync semaphore */
  while((syncSema = ( void * )syncSemaArray[syncHandle]) == L7_NULL)
  {
    osapiTaskDelay(1);
  }

  /*
  * If a task is waiting....
  */
  if ( syncSema != L7_NULL )
  {
    /*
    * Tell waiting task that I am done and can continue...
    */
    rc = osapiSemaGive( syncSema );
    return( rc );
  }

  return( L7_FAILURE );

} /* taken from VxWorks OSAPI implementation */

/**************************************************************************
*
* @purpose  Waits for a task to complete initializaiton
*
* @param    synchandle  handle of task to wait for
* @param    timeout     timeout in milliseconds to wait for task
*
* @returns  L7_SUCCESS  task is initialized, continue normal
* @returns  L7_FAILURE  task is not init or could not sync up
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiWaitForTaskInit( L7_uint32 syncHandle, L7_int32 timeout )
{
  L7_RC_t rc = L7_FAILURE;

  /* Get a Binary Semaphore for synchronization with a task */
  void *syncSema;

  syncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

  if (syncSema == L7_NULL)
  {
    LOG_ERROR((L7_ulong32)syncSema);
  }

  syncSemaArray[syncHandle] = syncSema;

  /* Take the semaphore and wait for the task to give it back */
  if ( osapiSemaTake(syncSema, timeout) != L7_ERROR )
  {
    rc = L7_SUCCESS;
  }

  osapiSemaDelete( syncSema );

  syncSemaArray[syncHandle] = L7_NULL;

  return(rc);

} /* taken from VxWorks OSAPI implementation */

/**************************************************************************
*
* @purpose  Delay a task for the number of ticks. Ticks are usually 1/60th of a second
*
* @param    ticks       number of clock ticks to delay
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  if called from interrupt level or if the calling task 
*                       receives a signal that is not blocked or ignored.
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskDelay(L7_int32 ticks)
{

  struct timespec timereq, timerem;
  L7_int32 usec;

  /* don't really need to check for "interrupt context" */

  usec = ticks * OSAPI_TICK_USEC;

  timereq.tv_sec  = ((usec >= 1000000) ? (usec / 1000000) : 0);
  timereq.tv_nsec = ((usec >= 1000000) ? ((usec % 1000000) * 1000)
                     : (usec * 1000));

  if (usec > 0)
  {
    for (;;)
    {
      pthread_testcancel();

      if (nanosleep(&timereq, &timerem) != 0)
      {
        timereq = timerem;
      }
      else
      {
        break;
      }
    }

  }
  else
  {
    sched_yield();
  }

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Show all active tasks in system
*
* @param    none.
*
* @returns  none.
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapiShowTasks(void)
{
  osapi_task_t *curTask;

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
#define SHOW_TASK_HEADING " Name            OPri  PPri CPri sSize     Stat     WaitQ    TaskID       PID\n"
#define SHOW_TASK_FMT " %-14.14s %4d %4d %4d  %8d %8x %8x %#8x %8d\n"
#else
#define SHOW_TASK_HEADING " Name            OPri  PPri CPri     Stat     WaitQ    TaskID       PID\n"
#define SHOW_TASK_FMT " %-14.14s %4d %4d %4d %8x %8x %#8x %8d\n"
#endif

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
    osapi_printf("osapiShowTasks: task_list_lock error\n");
  }

  osapi_printf(SHOW_TASK_HEADING);
  for (curTask = task_list_head; curTask != NULL;
      curTask = curTask->chain_next)
  {
    osapi_printf(SHOW_TASK_FMT,
                 curTask->name,
                 curTask->osapi_prio,
                 curTask->pthread_prio,
                 curTask->cur_prio,
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
                 curTask->stack_size,
#endif
                 curTask->flags,
                 curTask->waiting,
                 curTask,
                 curTask->PID);

  }

  pthread_cleanup_pop(1);

}

#if 1 /* osapiTaskLock() is now deprecated */
/**************************************************************************
*
* @purpose  Suspend task scheduling.
*
* @param    none.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @comments    This routine disables task context switching. The task that calls
* @comments    this routine will be the only task that is allowed to execute,
* @comments    unless the task explicitly gives up the CPU by making itself no
* @comments    longer ready. Typically this call is paired with osapiTaskUnlock();
* @comments    together they surround a critical section of code. These preemption
* @comments    locks are implemented with a counting variable that allows nested
* @comments    preemption locks. Preemption will not be unlocked until
* @comments    osapiTaskUnlock() has been called as many times as osapiTaskLock().
* @comments
* @comments    This routine does not lock out interrupts; use osapiDisableInts()
* @comments    to lock out interrupts.
* @comments
* @comments    A osapiTaskLock()is preferable to osapiDisableInts() as a means of
* @comments    mutual exclusion, because interrupt lock-outs add interrupt latency
* @comments    to the system.
* @comments
* @comments    A osapiSemaTake() is preferable to osapiTaskLock() as a means of mutual
* @comments    exclusion, because preemption lock-outs add preemptive latency to the system.
* @comments
* @comments    The osapiTaskLock() routine is not callable from interrupt service routines.
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskLock(void)
{

  int policy;
  osapi_task_t *self = pthread_getspecific(osapi_task_key);

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&osapi_task_lock);
  if (pthread_mutex_lock(&osapi_task_lock) != 0)
  {
    osapi_printf("osapiTaskLock: osapi_task_lock error\n");
  }

  if (self != osapi_task_lock_owner)
  {
    while (osapi_task_lock_owner != NULL)
    {
      pthread_cond_wait(&osapi_task_lock_cond, &osapi_task_lock);
    }

    osapi_task_lock_owner = self;

    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&(self->lock));
    if (pthread_mutex_lock(&(self->lock)) != 0)
    {
      osapi_printf("osapiTaskLock: self->lock error\n");
    }

    policy = (((self->flags & TASK_TIME_SLICED) == 0) ? SCHED_FIFO : SCHED_RR);

    /* Tasks that have default priority normally use SCHED_OTHER policy.
    ** Priority adjustment won't work unless policy is changed as well.
    */
    if (self->osapi_prio == L7_DEFAULT_TASK_PRIORITY)
    {
      pthread_attr_setschedpolicy(&(self->attr), policy);
    }

    self->cur_prio = (policy == SCHED_FIFO) ?
                     osapi_task_lock_fifo_prio: osapi_task_lock_rr_prio;

    pthread_attr_getschedparam(&(self->attr), &(self->param));

    self->param.sched_priority = self->cur_prio;

    pthread_attr_setschedparam(&(self->attr), &(self->param));

    pthread_cleanup_pop(1);

  }

  osapi_task_lock_depth++;

  pthread_cleanup_pop(1);

  return(L7_SUCCESS);

}

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
* @comments    critical section of code. Preemption will not be unlocked until
* @comments    osapiTaskUnlock() has been called as many times as osapiTaskLock().
* @comments    When the lock count is decremented to zero, any tasks that were eligible
* @comments    to preempt the current task will execute. The osapiTaskUnlock() routine
* @comments    is not callable from interrupt service routines.
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskUnlock(void)
{

  L7_RC_t rc;
  osapi_task_t *self = pthread_getspecific(osapi_task_key);

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&osapi_task_lock);
  if (pthread_mutex_lock(&(osapi_task_lock)) != 0)
  {
    osapi_printf("osapiTaskUnlock: osapi_task_lock error\n");
  }

  if (self == osapi_task_lock_owner)
  {

    if (--osapi_task_lock_depth == 0)
    {

      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                           (void *)&(self->lock));
      if (pthread_mutex_lock(&(self->lock)) != 0)
      {
        osapi_printf("osapiTaskUnlock: self->lock error\n");
      }

      self->cur_prio = self->pthread_prio;

      /* Tasks that have default priority use SCHED_OTHER policy. This allows
      ** tasks to time slice with other user and kernel tasks.
      */
      if (self->osapi_prio == L7_DEFAULT_TASK_PRIORITY)
      {

        pthread_attr_setschedpolicy(&(self->attr), SCHED_OTHER);

      }

      pthread_attr_getschedparam(&(self->attr), &(self->param));

      self->param.sched_priority = self->cur_prio;

      pthread_attr_setschedparam(&(self->attr), &(self->param));

      pthread_cleanup_pop(1);

      osapi_task_lock_owner = NULL;

      pthread_cond_signal(&osapi_task_lock_cond);

    }

    rc = L7_SUCCESS;

  }
  else
  {

    rc = L7_FAILURE;

  }

  pthread_cleanup_pop(1);

  return(rc);

}
#endif

/**************************************************************************
*
* @purpose  Check for interrupt context
*
* @param    none
*
* @returns  L7_TRUE if the OS is currently processing an interrupt thread
* @returns  L7_FALSE is the OS is in normal processing mode
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_BOOL osapiIntContext(void)
{
  return(L7_FALSE);
}

#if 1 /* osapiDisableInts() is now deprecated */
/**************************************************************************
*
* @purpose  Turn off all of the interrupts for this system.
*
* @param    none.
*
* @returns  The MSR register value (Lock Key) used to unlock interrupts later.
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_uint32 osapiDisableInts(void)
{

  int policy;
  osapi_task_t *self = pthread_getspecific(osapi_task_key);

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&osapi_task_int);
  if (pthread_mutex_lock(&(osapi_task_int)) != 0)
  {
    osapi_printf("osapiDisableInts: osapi_task_int error\n");
  }

  if (self != osapi_task_int_owner)
  {

    while (osapi_task_int_owner != NULL)
    {
      pthread_cond_wait(&osapi_task_int_cond, &osapi_task_int);
    }

    osapi_task_int_owner = self;

    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&(self->lock));
    if (pthread_mutex_lock(&(self->lock)) != 0)
    {
      osapi_printf("osapiDisableInts: self->lock error\n");
    }

    policy = (((self->flags & TASK_TIME_SLICED) == 0) ? SCHED_FIFO : SCHED_RR);

    /* Tasks that have default priority normally use SCHED_OTHER policy.
    ** Priority adjustment won't work unless policy is changed as well.
    */
    if (self->osapi_prio == L7_DEFAULT_TASK_PRIORITY)
    {
      pthread_attr_setschedpolicy(&(self->attr), policy);
    }

    self->cur_prio = (policy == SCHED_FIFO) ?
                     osapi_task_max_fifo_prio: osapi_task_max_rr_prio;

    pthread_attr_getschedparam(&(self->attr), &(self->param));

    self->param.sched_priority = self->cur_prio;

    pthread_attr_setschedparam(&(self->attr), &(self->param));

    pthread_cleanup_pop(1);

  }

  osapi_task_int_depth++;

  pthread_cleanup_pop(1);

  return(0);

}

/**************************************************************************
*
* @purpose  ReEnable external interrupts for this system.
*
* @param    LockKey @b{(input)}  The MSR register value previously returned from the lockout routine.
*
* @returns  none.
*
* @comments    none.
*
* @end
*
*************************************************************************/
void osapiEnableInts(L7_uint32 LockKey)
{

  L7_RC_t rc;
  osapi_task_t *self = pthread_getspecific(osapi_task_key);

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&osapi_task_int);
  if (pthread_mutex_lock(&(osapi_task_int)) != 0)
  {
    osapi_printf("osapiEnableInts: osapi_task_int error\n");
  }

  if (self == osapi_task_int_owner)
  {

    if (--osapi_task_int_depth == 0)
    {

      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                           (void *)&(self->lock));
      if (pthread_mutex_lock(&(self->lock)) != 0)
      {
        osapi_printf("osapiEnableInts: self->lock error\n");
      }

      self->cur_prio = self->pthread_prio;

      /* Tasks that have default priority use SCHED_OTHER policy. This allows
      ** tasks to time slice with other user and kernel tasks.
      */
      if (self->osapi_prio == L7_DEFAULT_TASK_PRIORITY)
      {
        pthread_attr_setschedpolicy(&(self->attr), SCHED_OTHER);
      }

      pthread_attr_getschedparam(&(self->attr), &(self->param));

      self->param.sched_priority = self->cur_prio;

      pthread_attr_setschedparam(&(self->attr), &(self->param));

      pthread_cleanup_pop(1);

      osapi_task_int_owner = NULL;

      pthread_cond_signal(&osapi_task_int_cond);

    }

    rc = L7_SUCCESS;

  }
  else
  {

    rc = L7_FAILURE;

  }

  pthread_cleanup_pop(1);

}
#endif

/**************************************************************************
*
* @purpose  Change the priority of a task.
*
* @param    TaskID ID of task to change priority.
* @param    Priority new task priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if requested priority is not within the range of 0-255.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskPrioritySet(L7_int32 TaskID, L7_int32 Priority)
{

  int policy;
  osapi_task_t *osapiTask = (osapi_task_t *)TaskID;
  struct sched_param sp;

  memset(&sp, 0, sizeof(sp));

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
		       (void *)&(osapiTask->lock));
  pthread_mutex_lock(&(osapiTask->lock));

  osapiTask->osapi_prio = Priority;

  if (Priority == L7_DEFAULT_TASK_PRIORITY)
  {
    policy = SCHED_OTHER;
    osapiTask->pthread_prio = 0;
  }
  else
  {
    policy = (((osapiTask->flags & TASK_TIME_SLICED) == 0) ? SCHED_FIFO : SCHED_RR);
    osapiTask->pthread_prio = osapi_scale_prio(Priority, policy);    
  }

  osapiTask->cur_prio = osapiTask->pthread_prio;
  sp.sched_priority = osapiTask->cur_prio;
  pthread_cleanup_pop(1);

  pthread_setschedparam(osapiTask->thread, policy, &sp);

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Get the configured priority of a task.
*
* @param    TaskID ID of task to query priority.
* @param    Priority task priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if task doesn't exist
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskPriorityGet(L7_int32 TaskID, L7_uint32 *Priority)
{

  osapi_task_t *osapiTask = (osapi_task_t *)TaskID;

  *Priority = osapiTask->osapi_prio;

  return(L7_SUCCESS);

}

/**************************************************************************
* @purpose  Lowers a task's priority to the lowest priority then returns
* @purpose  the task to it's previous priority
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @end
*************************************************************************/

L7_RC_t osapiTaskYield ( void )
{
#ifdef L7_LINUX_24
  osapi_task_t *self = pthread_getspecific(osapi_task_key);
  L7_uint32     sched_policy;

  pthread_attr_getschedpolicy(&(self->attr), &sched_policy);

  if ((SCHED_FIFO == sched_policy) || (SCHED_RR == sched_policy))
  {
    sched_yield();
  }
#else
  sched_yield();
#endif

  return(L7_SUCCESS);
}

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
L7_RC_t  osapiTaskNameGet( L7_int32 task_id, L7_char8 *task_name)
{

  osapi_task_t *task_block;
  osapi_task_t *curTask;
  L7_RC_t rc;

  task_block = (osapi_task_t *) task_id;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
    LOG_ERROR (0);
  }

  for (curTask = task_list_head; curTask != NULL;
      curTask = curTask->chain_next)
  {
    if (task_block == curTask)
    {
      break;
    }

  }

  /* If task is valid then fill in the name.
  */
  if (curTask == task_block)
  {
    strncpy (task_name, task_block->name, 31);
    rc = L7_SUCCESS;
  } else
  {
    rc = L7_FAILURE;
  }

  pthread_cleanup_pop(1);

  return rc;
}

/**************************************************************************
*
* @purpose  Get the task ID of the calling task.
*
* @param    TaskIDPtr ptr where to place the Task ID of the the calling
task.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE If called from interrupt context
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskIDSelfGet(L7_int32 *TaskIDPtr)
{

  *TaskIDPtr = (L7_int32)pthread_getspecific(osapi_task_key);

  return(L7_SUCCESS);

}

/**************************************************************************
*
* @purpose  Checks validity of the task ID.
*
* @param    TaskID to be checked.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE If TaskID is invalid.
*
* @comments
*
* @end
*
*************************************************************************/
L7_RC_t osapiTaskIdVerify(L7_int32 task_id)
{

  osapi_task_t *task_block;
  osapi_task_t *curTask;
  L7_RC_t rc;

  task_block = (osapi_task_t *) task_id;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
    LOG_ERROR (0);
  }

  for (curTask = task_list_head; curTask != NULL;
      curTask = curTask->chain_next)
  {
    if (task_block == curTask)
    {
      break;
    }
  }

  if (curTask == task_block)
  {
    rc = L7_SUCCESS;
  } else
  {
    rc = L7_FAILURE;
  }

  pthread_cleanup_pop(1);

  return rc;
}

/**************************************************************************
*
* @purpose  Get the task ID of a running task.
*
* @param    void
*
* @returns  task_id
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_int32  osapiTaskIdSelf( void)
{
  return((L7_int32)pthread_getspecific(osapi_task_key));
}

/**************************************************************************
*
* @purpose  Check if running task is requested task.
*
* @param    Task Name of requested task
*
* @returns  L7_TRUE if this is the task requested, else L7_FALSE
*
* @comments None.
*
* @end
*
*************************************************************************/
L7_BOOL osapiTaskIdIs( L7_uchar8 *taskNameToMatch )
{
   L7_char8 *task_name;

   task_name = pthread_getspecific(osapi_name_key);

   if ((task_name != L7_NULL) && memcmp(task_name, taskNameToMatch, strlen(taskNameToMatch)) == L7_NULL)
    return L7_TRUE;
  else
    return L7_FALSE;

}

typedef struct {
   char *name;
   FUNCPTR value;
} SYMBOL;  /* need tools to build symbol table */
typedef unsigned long ULONG;

#if defined(__mips__)
unsigned long *mips_backtrace(unsigned long *pc, unsigned long **sp,
                              FILE *logfd);
#endif
/**************************************************************************
*
* @purpose  Display stack trace of specified task.
*
* @param    task_id        - Linux task ID returned by osapiShowTasks()
* @param    FILE *filePtr  - If filePtr is 0, then the output is written to stdout. 
*                            Else the output is written to the file handle specified.
*
*
* @returns  none
*
* @comments
*
* @end
*
*************************************************************************/
void osapiDebugStackTrace (L7_uint32 debug_task_id, FILE *filePtr)
{
#if defined (__powerpc__) || defined(__mips__)
  osapi_task_t *task_block;
  osapi_task_t *curTask;
  int rc;
  int   wait_status;
  unsigned int pc_addr;
  unsigned int sp;
  L7_RC_t    l7rc;
  unsigned int sym_addr;
  char name_buf[32];
  L7_uint32 offset;
  FILE *fp = stdout;

#ifdef L7_LINUX_26
  int pid;
#endif

#if defined(__powerpc__)
  unsigned int prev_sp;
  unsigned int lr;
#endif

  if (debug_task_id == 0)
  {
    (void)osapiDevshellSymbolsLoad();
  }
  /* This function does the following:
   **   - Wait for signal to start work.
   **   - Stop all Fastpath tasks or requested task.
   **   - Display stack trace for all tasks or requested task.
   */

  /* Lock task list. No tasks can be added or deleted while we are
   ** getting stack info.
   */
  if (filePtr != L7_NULL) 
  {
    fp = filePtr;
  }
  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
      (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
    LOG_ERROR (0);
  }

#ifdef L7_LINUX_26
  pid = fork();
  if (pid < 0)
  {
    perror("fork");
    return;
  }
  if (pid == 0)
  {
#endif
    task_block = (osapi_task_t *) debug_task_id;
    for (curTask = task_list_head; curTask != NULL;
        curTask = curTask->chain_next)
    {
      if ((task_block == curTask) ||
          (task_block == 0))
      {
        /* Don't stop this task.
         */
        if (curTask->PID == osapiGetpid ())
        {
          continue;
        }

	if ((debug_task_id == 0) && ((strcmp(curTask->name, "bcmDISC") == 0) ||
                                 (strcmp(curTask->name, "bcmXGS3AsyncTX") == 0) ||
                                 (strcmp(curTask->name, "bcmTX") == 0)))
	{
	  fprintf(fp, "\n\nSkipping %s task as it can crash when traced.\n", curTask->name);
	  fprintf(fp, "If you want to trace that task, execute:\n");
	  fprintf(fp, "devshell osapiDebugStackTrace(0x%x,0x%x)\n\n", (int) curTask, (int)fp);
	  continue;
	}

        fprintf(fp, "\n\nStopping task 0x%x\n",  (int) curTask);
        rc = ptrace (PTRACE_ATTACH, curTask->PID, 0, 0);
        if (rc != 0)
        {
          fprintf(fp, "Error %d setting PTRACE_ATTACH to PID %d\n",
              osapiErrnoGet (),
              curTask->PID);
          continue;
        }

        rc = waitpid (curTask->PID, &wait_status, __WALL);
        fprintf(fp, "waitpid returned %d. Errno = %d\n",
            rc, osapiErrnoGet());
        if (WIFSTOPPED(wait_status))
        {
          fprintf(fp, "Child stopped, signal = %d\n", WSTOPSIG(wait_status));
        } else
        {
          fprintf(fp, "Wait status = %d (0x%x)\n", wait_status, wait_status);
        }

        rc = 0;
#if defined(__powerpc__)
        sp      = ptrace(PTRACE_PEEKUSER, curTask->PID,
            &(((struct user *) 0)->regs.gpr[1]), 0);

        pc_addr = ptrace(PTRACE_PEEKUSER, curTask->PID,
            &(((struct user *) 0)->regs.nip), 0);
#elif defined(__mips__)
        sp      = ptrace(PTRACE_PEEKUSER, curTask->PID, 29, 0);
        pc_addr = ptrace(PTRACE_PEEKUSER, curTask->PID, PC, 0);
#endif /* __powerpc__ || __mips__ */
        if ((sp == -1) || (pc_addr == -1))
        {
          rc = osapiErrnoGet ();
        }
        if (rc ==  0)
        {
          /* Analyze the stack.....
           */
          fprintf(fp, "\n------------\n");
          fprintf(fp, "Task ID:       0x%x\n", (unsigned int)curTask);
          fprintf(fp, "Task PID:      0x%x\n", curTask->PID);
          fprintf(fp, "Task pthread:  0x%x\n", (unsigned int)curTask->thread);
          fprintf(fp, "Task Name:     %s\n", curTask->name);
          fprintf(fp, "PC:            0x%x\n", pc_addr);
          fprintf(fp, "SP:            0x%x\n", sp);

          l7rc = osapiFunctionLookup(pc_addr, name_buf, 32, &offset);
          if (l7rc == L7_SUCCESS)
          {
            sym_addr = pc_addr - offset;
          } else
          {
            snprintf(name_buf, sizeof(name_buf),"?????" );
            name_buf[31] = '\0';
            sym_addr = 0;
            offset = 0;
          }
          fprintf(fp, "0x%x: %s (0x%x) + 0x%x\n", pc_addr, name_buf, sym_addr, offset);

#if defined(__powerpc__)
          prev_sp = 0;
          pc_addr = 0;

          while ((prev_sp != -1) && (pc_addr != -1))
          {
            prev_sp = ptrace(PTRACE_PEEKDATA, curTask->PID,
                (unsigned int *) sp, 0);
            if (prev_sp == -1)
            {
              continue;
            }

            lr = prev_sp + 4;

            pc_addr = ptrace(PTRACE_PEEKDATA, curTask->PID,
                (unsigned int *) lr, 0);
            if (pc_addr == -1)
            {
              continue;
            }

            sp = prev_sp;

            l7rc = osapiFunctionLookup(pc_addr, name_buf, 32, &offset);
            if (l7rc == L7_SUCCESS)
            {
              sym_addr = pc_addr - offset;
            } else
            {
              snprintf(name_buf, sizeof(name_buf),"?????");
              name_buf[31] = '\0';
              sym_addr = 0;
              offset = 0;
            }
            fprintf(fp, "0x%x: %s (0x%x) + 0x%x\n", pc_addr, name_buf, sym_addr, offset);
          }
#elif defined(__mips__)
          {
            int stackDepth = 0;
            unsigned long *retAddr = (unsigned long *)pc_addr;
            unsigned long *trc_sp = (unsigned long *)sp;

            while ((retAddr) && (stackDepth < 40))
            {
              retAddr = mips_backtrace(retAddr, &trc_sp, 0);
              stackDepth++;
              if (retAddr)
              {
                l7rc = osapiFunctionLookup((int)retAddr, name_buf, 32, &offset);
                if (l7rc == L7_SUCCESS)
                {
                  sym_addr = (int)retAddr - offset;
                }
                else
                {
                  snprintf(name_buf, sizeof(name_buf),"?????");
                  name_buf[31] = '\0';
                  sym_addr = 0;
                  offset = 0;
                }
                fprintf(fp, "0x%x: %s (0x%x) + 0x%x\n",
                    (int)retAddr, name_buf, sym_addr, offset);
              }
            }
          }

#endif /* __powerpc__ || __mips__ */

        } else
        {
          fprintf(fp, "Error %d getting PTRACE_PEEKUSER from PID %d\n",
              osapiErrnoGet (),
              curTask->PID);
        }



        /* Restart the task.
         */
        fprintf(fp, "Restarting task 0x%x\n", (int) curTask);
        rc = ptrace (PTRACE_DETACH, curTask->PID, 0, 0);
        if (rc != 0)
        {
          fprintf(fp, "Error %d sentting PTRACE_DETACH to PID %d\n",
              osapiErrnoGet (),
              curTask->PID);
          continue;
        }

      }

    }
#ifdef L7_LINUX_26
    exit(0);
  }
  else
  {
    waitpid(pid, NULL, 0);
  }
#endif
  pthread_cleanup_pop(1);

#else  /* __powerpc__ || __mips__ */
  FILE *fp = stdout;

  if (filePtr != L7_NULL) 
  {
    fp = filePtr;
  }
  fprintf(fp, "Stack trace is not supported on this architecture.\n");
#endif /* __powerpc__ || __mips__ */

}
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
L7_RC_t osapiWhichStack(L7_uint32 addr, L7_char8 *buf, L7_uint32 bufSize)
{
  osapi_task_t *checking;
  L7_uint32 cur_stack_guard_begin, cur_stack_guard_end;
  L7_RC_t rc = L7_FAILURE;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) == 0)
  {
    checking = task_list_head;
    while (checking != NULL) 
    {
      cur_stack_guard_end = checking->stack_base_addr - 1;
      cur_stack_guard_begin = checking->stack_base_addr - system_page_size;
      if ((addr >= cur_stack_guard_begin) && (addr <= cur_stack_guard_end))
      { 
        rc = L7_SUCCESS;
        osapiStrncpySafe(buf, checking->name, bufSize);
        break;
      }
      checking = checking->chain_next;
    }
  }
  pthread_cleanup_pop(1);
  return(rc);
}

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
int osapiTaskPidGet( L7_int32 task_id)
{

  return ((osapi_task_t *)(task_id))->PID;
}


#define STAT_BUFSIZE 1024
static unsigned int getCpuTime(char *taskStatFileName)
{
  int fd;
  char statBuf[STAT_BUFSIZE];
  int bufSize;
  char procState;
  int  ppid;
  char procName[24];
  unsigned int utime, stime, cpuTime = 0;

  fd = open(taskStatFileName, O_RDONLY);
  if (fd < 0)
  {
    /*printf("%s: file open failed for %s\n", __FUNCTION__, taskStatFileName);*/
    return 0;
  }
  bufSize = read(fd, statBuf, STAT_BUFSIZE);
  if (bufSize >= 0)
  {
    if (sscanf(&statBuf[0], "%d %s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %u %u",
         &ppid, &procName[0], &procState,
         &utime, &stime) == 5)
    {
      cpuTime = utime + stime;
    }
  }
  close(fd);
  return (cpuTime);
}

#define L7_CPU_UTIL_TICKSPERSECOND 100

/*********************************************************************
* @purpose  Task that measures Cpu utilization.
*
* @param    numArgs @b{(input)}  Number of arguments passed from caller
* @param    argv    @b{(input)}  pointer to arguments passed from caller
*
* @returns  none
*
* @comments The task collects a sample every L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS
*           and updates the utilization for various tasks. The utilization
*           is calculated as a Exponential moving average.
*
*           Floating point operations have been purposely avoided due to
*           various legacy issues on some architecture. To get more accuracy
*           in integer arithmetic, values are scaled by L7_CPU_UTIL_SCALAR 
*           and stored.
*
* @end
*
*********************************************************************/
void osapiCpuUtilMonitorTask(L7_ulong32 numArgs, L7_uint32 *argv )
{
  L7_char8          taskStatFileName[40];
  osapi_task_t     *curTask;
  L7_int32          fpPid = getpid(), taskPercent = 0;
  L7_int32          totalPercent, expectedTotalTicks;
  L7_uint32         curCpuTime, cpuTime, currentTime, lastAgeScanTime = 0;
  L7_uint32         samplePeriod, index;
  L7_BOOL insertFlag = L7_FALSE;
  struct            timespec t;
  static struct     timespec oldtime;
  long int          elapsed_time; /* PTin corrected: CPU utilization bug! */


  samplePeriod    = L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS;
  expectedTotalTicks = L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS * L7_CPU_UTIL_TICKSPERSECOND;

  /* Initialize list of tasks */

  sysapiCpuUtilLockTake();

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&task_list_lock);
  if (pthread_mutex_lock(&task_list_lock) != 0)
  {
    osapi_printf("%s: task_list_lock error\n", __FUNCTION__);
  }

  for (curTask = task_list_head; curTask != NULL;
      curTask = curTask->chain_next)
  {
    sysapiTaskCpuUtilTableInsert((L7_int32)curTask);
  }
  pthread_cleanup_pop(1);

  sysapiTaskCpuUtilTableSort();
  sysapiCpuUtilLockGive();

  do
  {
    osapiSleep(L7_CPU_UTIL_SAMPLE_PERIOD_SECONDS);

    if (sysapiCpuUtilMonitorActive() == L7_FALSE)
    {
      continue;
    }

    sysapiCpuUtilLockTake();

    pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                         (void *)&task_list_lock);
    if (pthread_mutex_lock(&task_list_lock) != 0)
    {
      osapi_printf("%s: task_list_lock error\n", __FUNCTION__);
    }

    /*
     * Finds the current time (in microseconds) and calculates the time
     * elapsed since the last update.
     */
    clock_gettime(CLOCK_MONOTONIC, &t);
    /* PTin corrected: CPU utilization bug! */
//  elapsed_time = (t.tv_sec - oldtime.tv_sec)   * L7_CPU_UTIL_TICKSPERSECOND
//               + (t.tv_nsec - oldtime.tv_nsec) * L7_CPU_UTIL_TICKSPERSECOND
//               / 100000000.0;
    /* PTin: now operation is integer and nsec are divided by 1000Ms and not 100Ms */
    elapsed_time  = (t.tv_sec - oldtime.tv_sec)   * L7_CPU_UTIL_TICKSPERSECOND
                  + (t.tv_nsec - oldtime.tv_nsec) / (1000000000 / L7_CPU_UTIL_TICKSPERSECOND);
    oldtime.tv_sec  = t.tv_sec;
    oldtime.tv_nsec = t.tv_nsec;
    totalPercent = 0;


    for (curTask = task_list_head; curTask != NULL;
        curTask = curTask->chain_next)
    {
      osapiSnprintf(taskStatFileName, sizeof(taskStatFileName), 
                    "/proc/%d/task/%d/stat", fpPid, curTask->PID);
      cpuTime = 0;
      taskPercent = 0;
      curCpuTime = getCpuTime(taskStatFileName);
      if (curCpuTime)
        cpuTime = curCpuTime - curTask->prevCpuTime;

      /* Task has CPU cycles, compute percentage */
      if (cpuTime)
      {
        taskPercent = (cpuTime * L7_CPU_UTIL_SCALAR * 100) / elapsed_time;
        if (taskPercent >= L7_CPU_UTIL_SCALAR * 100)
           taskPercent = (L7_CPU_UTIL_SCALAR * 100) - 1; /* 99.99% */
        totalPercent += taskPercent;
        curTask->prevCpuTime = curCpuTime;
      }


      insertFlag = L7_FALSE;
      if (sysapiTaskCpuUtilTableFind((L7_int32)curTask, &index) == L7_FALSE)
      {
        index = sysapiTaskCpuUtilTableInsert((L7_int32)curTask);
        insertFlag = L7_TRUE;
      }

      /* Update the utilization in the task table */
      sysapiTaskCpuUtilUpdate(index, taskPercent);

      if (insertFlag == L7_TRUE)
        sysapiTaskCpuUtilTableSort();

     
    }
    /* end of task operations */
    pthread_cleanup_pop(1);


    /* Age out inactive tasks. */
    currentTime = osapiUpTimeRaw();
    if ((currentTime - lastAgeScanTime) >= L7_CPU_UTIL_TASK_AGE_TIME_SEC)
    {
      sysapiCpuUtilTaskAge();
      lastAgeScanTime = currentTime;
    }

    /* Calculate total util for this period and update totalUtilTable. 
     * As we are avoiding Floating point operations, the utilization is scaled 
     * by L7_CPU_UTIL_SCALAR to get better precision. 
     */ 
    
    if (totalPercent >= L7_CPU_UTIL_SCALAR * 100)
    {
      totalPercent = (L7_CPU_UTIL_SCALAR * 100) - 1; /* 99.99% */
    }

    sysapiTotalCpuUtilUpdate(totalPercent);

    sysapiCpuUtilLockGive();

  } while (1);

  return;
}

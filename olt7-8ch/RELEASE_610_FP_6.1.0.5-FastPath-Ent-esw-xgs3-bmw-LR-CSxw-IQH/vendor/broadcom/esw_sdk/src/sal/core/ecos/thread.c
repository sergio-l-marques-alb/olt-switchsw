/*
 * $Id: thread.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2008 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File: 	thread.c
 * Purpose:	Defines SAL routines for eCos threads
 *
 * Thread Abstraction
 */

#include <sal/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <pkgconf/kernel.h>
#include <cyg/kernel/kapi.h>
#include <time.h>
#include <sys/time.h>

#include <assert.h>
#include <sal/core/alloc.h>
#include <sal/core/thread.h>

#define ECOS_THREAD_DEBUG 0


#if ECOS_THREAD_DEBUG
#include "cyg/infra/diag.h" /* diag_printf */
#define THREAD_DBG(x)  do { diag_printf("SAL_THREAD: "); \
                       diag_printf x; diag_printf("\n");\
                       } while(0);
#else
#define THREAD_DBG(x)
#endif

#define NUMBER_OF_WORKERS   32
#define THREAD_STATE_EXITED 16
#define MONITOR_THREAD_STACKSIZE   1024
#define MONITOR_THREAD_NAME "monitor"
/* We give it highest prioriy now */
#define MONITOR_THREAD_PRIORITY  3

typedef struct thread_ctrl_s {
    void (*fn)(void *);
    void *arg;
    cyg_bool_t available;
} thread_ctrl_t;

#ifdef LVL7_FIXUP
#include <l7_common.h>
#include <osapi.h>
extern pthread_key_t osapi_name_key;
#else
static thread_ctrl_t worker_entries[NUMBER_OF_WORKERS];
static cyg_handle_t worker_handles[NUMBER_OF_WORKERS];

static cyg_uint8 *worker_stacks[NUMBER_OF_WORKERS];
static cyg_thread worker_threads[NUMBER_OF_WORKERS];

static cyg_mutex_t	thread_mutex;
static cyg_sem_t	thread_sem;

extern cyg_tick_count_t _sal_usec_to_ticks(uint32 usec);	/* time.c */

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
static unsigned int _sal_thread_count_curr;
static unsigned int _sal_thread_count_max;
static unsigned int _sal_thread_stack_size_curr;
static unsigned int _sal_thread_stack_size_max;
#define SAL_THREAD_RESOURCE_USAGE_INCR(a_cnt, a_cnt_max, a_sz,      \
                                       a_sz_max, n_ssize, ilock)    \
    a_cnt++;                                                        \
    a_sz += (n_ssize);                                              \
    a_cnt_max = ((a_cnt) > (a_cnt_max)) ? (a_cnt) : (a_cnt_max);    \
    a_sz_max = ((a_sz) > (a_sz_max)) ? (a_sz) : (a_sz_max)

#define SAL_THREAD_RESOURCE_USAGE_DECR(a_count, a_ssize, n_ssize, ilock)\
        a_count--;                                                      \
        a_ssize -= (n_ssize)

/*
 * Function:
 *      sal_thread_resource_usage_get
 * Purpose:
 *      Provides count of active threads and stack allocation
 * Parameters:
 *      alloc_curr - Current memory usage.
 *      alloc_max - Memory usage high water mark
 */

void
sal_thread_resource_usage_get(unsigned int *sal_thread_count_curr,
                              unsigned int *sal_stack_size_curr,
                              unsigned int *sal_thread_count_max,
                              unsigned int *sal_stack_size_max)
{
    if (sal_thread_count_curr != NULL) {
        *sal_thread_count_curr = _sal_thread_count_curr;
    }
    if (sal_stack_size_curr != NULL) {
        *sal_stack_size_curr = _sal_thread_stack_size_curr;
    }
    if (sal_thread_count_max != NULL) {
        *sal_thread_count_max = _sal_thread_count_max;
    }
    if (sal_stack_size_max != NULL) {
        *sal_stack_size_max = _sal_thread_stack_size_max;
    }
}

#endif
#endif


/*
 * Function:
 *	thread_find_free
 * Purpose:
 *	Find a free worker entry.
 * Parameters:
 *	NONE
 * Returns:
 *	index in worker table, -1 = no free entry
 */
static int thread_find_free(void)
{
    int i;
	
    for(i=1; i< NUMBER_OF_WORKERS; i++) {
        if (worker_entries[i].available == true)
            break;
	   }
    if (i == NUMBER_OF_WORKERS)
        return -1;
	
    return i;
}

/*
 * Function:
 *	thread_find_handle(
 * Purpose:
 *	Find the index of the given handle in worker table.
 * Parameters:
 *	h - handle to compare.
 * Returns:
 *	index in worker table, -1 = no found
 */
static int thread_find_handle(cyg_handle_t h)
{
    int i;
	
    for(i=1; i< NUMBER_OF_WORKERS; i++) {
        if (worker_handles[i] == h) 
            break;
    }
    if (i == NUMBER_OF_WORKERS)
        return -1;
    return i;
}

/*
 * Function:
 *	main_entry
 * Purpose:
 *	Thread entry wrapper because different prototypes in SDK and ECOS.
 * Parameters:
 *	data - parameter passed when calling thread_create
 * Returns:
 *	NONE
 */
static void main_entry(cyg_addrword_t data)
{
    worker_entries[data].fn(worker_entries[data].arg);
}

/*
 * Function:
 *	monitor_entry
 * Purpose:
 *	Monitor thread exit states and maintain worker tables
 * Parameters:
 *	data - parameter passed when calling thread_create
 * Returns:
 *	NONE
 */
static void monitor_entry(cyg_addrword_t data)
{
    cyg_thread_info info;
    cyg_handle_t qthread=0;
    cyg_uint16 id = 0;
    int i;
    static int found = 0;
    
    cyg_semaphore_init(&thread_sem, 0);
    
    while(1) {
        
        cyg_semaphore_wait(&thread_sem);
        
        do {
            
            if (found == 0) /* let the thread terminate itself first */
                sal_usleep(50000);
            else
                break;
            
            while( cyg_thread_get_next( &qthread, &id ) ) {
            
                cyg_thread_get_info( qthread, id, &info );
    	
                if (info.state == THREAD_STATE_EXITED) {
                    i = thread_find_handle(qthread);
                    if (i != -1) {
                        cyg_mutex_lock(&thread_mutex);
                        worker_entries[i].available = true;
                        worker_handles[i] = (cyg_handle_t)NULL;
                        THREAD_DBG(("free thread %s stack...", info.name));
                        sal_free(worker_stacks[i]);
                        cyg_mutex_unlock(&thread_mutex);
                        found = 1;
                    }
                    /* also delete those threads with state exited */
                    THREAD_DBG(("delete thread %s...", info.name));
                    cyg_thread_delete(qthread);
                }
            }
        }while(1);
        
        found = 0;
    } /* first while */
}
#endif
/*
 * Function:
 *	sal_thread_create
 * Purpose:
 *	Abstraction for task creation
 * Parameters:
 *	name - name of task
 *	ss - stack size requested
 *	prio - scheduling prio (0 = highest, 255 = lowest)
 *        For eCos, max prio depends on CYGNUM_KERNEL_SCHED_PRIORITIES.
 *        Default setting is 32.
 *	func - address of function to call
 *	arg - argument passed to func.
 * Returns:
 *	Thread ID
 */

sal_thread_t
sal_thread_create(char *name, int ss, int prio, void (f)(void *), void *arg)
{
#ifdef LVL7_FIXUP
  L7_int32 salTaskHandle;
  salTaskHandle = osapiTaskCreate(name, f, (L7_uint32) arg,
                                  L7_NULLPTR, ss,
                                  L7_DEFAULT_TASK_PRIORITY,
   		      L7_DEFAULT_TASK_SLICE);

  return (sal_thread_t) salTaskHandle;
#else
    static int initialized = 0;
	
    int i;
	   
	   THREAD_DBG(("create thread %s, ss=%d, prio=%d", name, ss, prio));
	   
    if (initialized == 0)
    {
        cyg_mutex_init(&thread_mutex);
        initialized = 1;
        for(i=1; i< NUMBER_OF_WORKERS; i++) {
            worker_entries[i].available = true;
            worker_handles[i] = (cyg_handle_t)NULL;
        }
        i = 0; /* let 0 be the monitor thread */
        THREAD_DBG(("create thread %s, ss=%d, prio=%d", 
                     MONITOR_THREAD_NAME, MONITOR_THREAD_STACKSIZE, 
                     MONITOR_THREAD_PRIORITY));
        worker_stacks[i] = (cyg_uint8 *)sal_alloc(MONITOR_THREAD_STACKSIZE, "SAL_THREAD");
        /* let it have the highest priority */
        cyg_thread_create(MONITOR_THREAD_PRIORITY, monitor_entry, i,
                          MONITOR_THREAD_NAME, worker_stacks[i],
                          MONITOR_THREAD_STACKSIZE,
                          &(worker_handles[i]), &(worker_threads[i]));
        cyg_thread_resume(worker_handles[i]);
    }
	
    cyg_mutex_lock(&thread_mutex);
    i = thread_find_free();
    
    if (i == -1) {			
        cyg_mutex_unlock(&thread_mutex);
        return NULL;
    }
	
    worker_entries[i].available = false;
    worker_entries[i].fn = f;
    worker_entries[i].arg = arg;
    
    cyg_mutex_unlock(&thread_mutex);
	
#ifdef SAL_THREAD_PRIORITY
    prio = SAL_THREAD_PRIORITY;
#endif /* SAL_THREAD_PRIORITY */
	
    /* prio = (prio * 32)/256; */
    /* can't be mapped from priority of sdk's thread */
    prio = 8;
    if (!strcmp(name, "bcmDPC") || 
        !strncmp(name, "bcmRX", 5) || 
        !strncmp(name, "bcmTX", 5)) {
        prio = 7;
    }
    worker_stacks[i] = (cyg_uint8 *)sal_alloc(ss, "SAL_THREAD");
    cyg_thread_create(prio, main_entry, i,
		    name, worker_stacks[i], ss, 
		    &(worker_handles[i]), &(worker_threads[i]));
    cyg_thread_resume(worker_handles[i]);
	
    return (sal_thread_t)worker_handles[i];

#endif

}

/*
 * Function:
 *	sal_thread_destroy
 * Purpose:
 *	Abstraction for task deletion
 * Parameters:
 *	thread - thread ID
 * Returns:
 *	0 on success, -1 on failure
 * Notes:
 *	This routine is not generally used by Broadcom drivers because
 *	it's unsafe.  If a task is destroyed while holding a mutex or
 *	other resource, system operation becomes unpredictable.  Also,
 *	some RTOS's do not include kill routines.
 *
 *	Instead, Broadcom tasks are written so they can be notified via
 *	semaphore when it is time to exit, at which time they call
 *	sal_thread_exit().
 */

int
sal_thread_destroy(sal_thread_t thread)
{
#ifdef LVL7_FIXUP
  L7_int32 salTaskHandle = (L7_int32) thread;

  osapiTaskDelete(salTaskHandle);
  return 0;
#else

    cyg_thread_kill((cyg_handle_t)PTR_TO_INT(thread));
    cyg_semaphore_post(&thread_sem);
    return 0;
#endif
}

/*
 * Function:
 *	sal_thread_self
 * Purpose:
 *	Return thread ID of caller
 * Parameters:
 *	None
 * Returns:
 *	Thread ID
 */

sal_thread_t
sal_thread_self(void)
{
#ifdef LVL7_FIXUP
   return (sal_thread_t) osapiTaskIdSelf();
#else
    return (sal_thread_t) INT_TO_PTR(cyg_thread_self());
#endif
}

/*
 * Function:
 *	sal_thread_name
 * Purpose:
 *	Return name given to thread when it was created
 * Parameters:
 *	thread - thread ID
 *	thread_name - buffer to return thread name;
 *		gets empty string if not available
 *	thread_name_size - maximum size of buffer
 * Returns:
 *	NULL, if name not available
 *	thread_name, if name available
 */

char *
sal_thread_name(sal_thread_t thread, char *thread_name, int thread_name_size)
{
#ifdef LVL7_FIXUP
  char *task_name;
  task_name = (char *) pthread_getspecific(osapi_name_key);
                       strncpy(thread_name, task_name, thread_name_size);
  return thread_name;
#else
    cyg_handle_t th = 0;
    cyg_uint16 id = 0;

    while( cyg_thread_get_next( &th, &id ) )
    {
        cyg_thread_info info;
        
        if (th != PTR_TO_INT(thread))
        	continue;

        if( !cyg_thread_get_info( th, id, &info ) )
        	return NULL;
            
        if (info.name != NULL) {
            strncpy(thread_name, info.name, thread_name_size);
            thread_name[thread_name_size - 1] = 0;
            return thread_name;
        } else {
            thread_name[0] = 0;
            return NULL;
        }
    }
    return NULL;
#endif
}

/*
 * Function:
 *	sal_thread_exit
 * Purpose:
 *	Exit the calling thread
 * Parameters:
 *	rc - return code from thread.
 * Notes:
 *	Never returns.
 */

void
sal_thread_exit(int rc)
{
#ifdef LVL7_FIXUP
  L7_int32 salTaskHandler = osapiTaskIdSelf();

  osapiTaskDelete(salTaskHandler);

  return;
#else
    cyg_semaphore_post(&thread_sem);    
    cyg_thread_exit();
#endif
}

/*
 * Function:
 *	sal_thread_yield
 * Purpose:
 *	Yield the processor to other tasks.
 * Parameters:
 *	None
 */

void
sal_thread_yield(void)
{
    cyg_thread_yield();
}

/*
 * Function:
 *	sal_thread_main_set
 * Purpose:
 *	Set which thread is the main thread
 * Parameters:
 *	thread - thread ID
 * Notes:
 *	The main thread is the one that runs in the foreground on the
 *	console.  It prints normally, takes keyboard signals, etc.
 */

static sal_thread_t _sal_thread_main = 0;

void
sal_thread_main_set(sal_thread_t thread)
{
    _sal_thread_main = thread;
}

/*
 * Function:
 *	sal_thread_main_get
 * Purpose:
 *	Return which thread is the main thread
 * Returns:
 *	Thread ID
 * Notes:
 *	See sal_thread_main_set().
 */

sal_thread_t
sal_thread_main_get(void)
{
    return _sal_thread_main;
}

/*
 * Function:
 *	sal_sleep
 * Purpose:
 *	Suspend calling thread for a specified number of seconds.
 * Parameters:
 *	sec - number of seconds to suspend
 * Notes:
 *	Other tasks are free to run while the caller is suspended.
 */

void
sal_sleep(int sec)
{
    cyg_tick_count_t	ticks;
    ticks = _sal_usec_to_ticks(sec*1000000);
    cyg_thread_delay(ticks);
}

/*
 * Function:
 *	sal_usleep
 * Purpose:
 *	Suspend calling thread for a specified number of microseconds.
 * Parameters:
 *	usec - number of microseconds to suspend
 * Notes:
 *	The actual delay period depends on the resolution of the
 *	VxWorks taskDelay() routine, whose precision is limited to the
 *	the period of the scheduler tick, generally 1/60 or 1/100 sec.
 *	Other tasks are free to run while the caller is suspended.
 */

void
sal_usleep(uint32 usec)
{
   struct timespec delay, remains;

   if (usec < (10000)) {
      usec = 10000;
   }

   delay.tv_sec = usec / 1000000;
   delay.tv_nsec = (usec % 1000000)*1000;

   while (nanosleep (&delay, &remains) != 0) {
      delay = remains;
   }

   return;
}

/*
 * Function:
 *	sal_udelay
 * Purpose:
 *	Spin wait for an approximate number of microseconds
 * Parameters:
 *	usec - number of microseconds
 *
 */

void
sal_udelay(uint32 usec)
{
    hal_delay_us(usec);
}

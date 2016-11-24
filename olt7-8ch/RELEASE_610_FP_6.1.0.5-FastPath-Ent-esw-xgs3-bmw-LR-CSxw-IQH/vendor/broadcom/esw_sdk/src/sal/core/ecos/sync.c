/*
 * $Id: sync.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 * File: 	sync.c
 * Purpose:	Defines SAL routines for mutexes and semaphores
 *
 * Mutex and Binary Semaphore abstraction
 */

#include <sys/types.h>
#include <sal/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <pkgconf/kernel.h>
#include <cyg/kernel/kapi.h>

#include <assert.h>
#include <sal/core/sync.h>
#include <sal/core/spl.h>
#include <sal/core/thread.h>
#include <sal/core/alloc.h>

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
static unsigned int _sal_sem_count_curr;
static unsigned int _sal_sem_count_max;
static unsigned int _sal_mutex_count_curr;
static unsigned int _sal_mutex_count_max;
#define SAL_SEM_RESOURCE_USAGE_INCR(a_curr, a_max, ilock)               \
        a_curr++;                                                       \
        a_max = ((a_curr) > (a_max)) ? (a_curr) : (a_max)
    
#define SAL_SEM_RESOURCE_USAGE_DECR(a_curr, ilock)                      \
        a_curr--

/*
 * Function:
 *      sal_sem_resource_usage_get
 * Purpose:
 *      Provides count of active sem and maximum sem allocation
 * Parameters:
 *      sem_curr - Current semaphore allocation.
 *      sem_max - Maximum semaphore allocation.
 */

void
sal_sem_resource_usage_get(unsigned int *sem_curr, unsigned int *sem_max)
{
    if (sem_curr != NULL) {
        *sem_curr = _sal_sem_count_curr;
    }
    if (sem_max != NULL) {
        *sem_max = _sal_sem_count_max;
    }
}

/*
 * Function:
 *      sal_mutex_resource_usage_get
 * Purpose:
 *      Provides count of active mutex and maximum mutex allocation
 * Parameters:
 *      mutex_curr - Current mutex allocation.
 *      mutex_max - Maximum mutex allocation.
 */

void
sal_mutex_resource_usage_get(unsigned int *mutex_curr, unsigned int *mutex_max)
{
    if (mutex_curr != NULL) {
        *mutex_curr = _sal_mutex_count_curr;
    }
    if (mutex_max != NULL) {
        *mutex_max = _sal_mutex_count_max;
    }
}
#endif
#endif

extern cyg_tick_count_t _sal_usec_to_ticks(uint32 usec);	/* time.c */

#define INT_CONTEXT()	sal_int_context()

/*
 * Keyboard interrupt protection
 *
 *   When a thread is running on a console, the user could Control-C
 *   while a mutex is held by the thread.  Control-C results in a signal
 *   that longjmp's somewhere else.  We prevent this from happening by
 *   blocking Control-C signals while any mutex is held.
 */

#if 0 

static int ctrl_c_depth = 0;

static void
ctrl_c_block(void)
{
    assert(!INT_CONTEXT());

#ifndef NO_CONTROL_C
    if (sal_thread_self() == sal_thread_main_get()) {
	if (ctrl_c_depth++ == 0) {
	    sigset_t set;
	    sigemptyset(&set);
	    sigaddset(&set, SIGINT);
	    sigprocmask(SIG_BLOCK, &set, NULL);
	}
    }
#endif
}

static void
ctrl_c_unblock(void)
{
    assert(!INT_CONTEXT());

#ifndef NO_CONTROL_C
    if (sal_thread_self() == sal_thread_main_get()) {
	assert(ctrl_c_depth > 0);
	if (--ctrl_c_depth == 0) {
	    sigset_t set;
	    sigemptyset(&set);
	    sigaddset(&set, SIGINT);
	    sigprocmask(SIG_UNBLOCK, &set, NULL);
	}
    }
#endif
}
#endif 

typedef struct recursive_mutex_s {
    cyg_mutex_t	lock;
    cyg_cond_t	wait;
    sal_thread_t owner;
    int count;
} ecos_mutex_t;

/*
 * Mutex and semaphore abstraction
 */

sal_mutex_t
sal_mutex_create(char *desc)
{
    ecos_mutex_t *em;
	
    assert(!INT_CONTEXT());
	
    if ((em = (ecos_mutex_t*)sal_alloc(sizeof (ecos_mutex_t), desc)) == NULL) {
        return NULL;
    }
	
    cyg_mutex_init(&(em->lock));
    cyg_cond_init(&(em->wait), &(em->lock));
    em->count = 0;
    em->owner = NULL;
    
#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_INCR(
            _sal_mutex_count_curr,
            _sal_mutex_count_max,
            ilock);
#endif
#endif
    return (sal_mutex_t) em;
}

void
sal_mutex_destroy(sal_mutex_t m)
{
    assert(!INT_CONTEXT());
    	
    ecos_mutex_t *em = (ecos_mutex_t *) m;

    cyg_mutex_destroy(&(em->lock));
    cyg_cond_destroy(&(em->wait));

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_DECR(
            _sal_mutex_count_curr,
            ilock);
#endif
#endif
    sal_free(em);
}

int
sal_mutex_take(sal_mutex_t m, int usec)
{
    ecos_mutex_t *em = (ecos_mutex_t *) m;
    cyg_tick_count_t	ticks;
    int rv=0;

    assert(!INT_CONTEXT());
    
    if (em->owner == sal_thread_self()) {
		      em->count++;
		      return rv;
    }
    
    cyg_mutex_lock(&(em->lock));
    
    while (em->count > 0) {
        if (usec == (uint32) sal_mutex_FOREVER) {
            if (!cyg_cond_wait(&em->wait)) {
                cyg_mutex_unlock(&(em->lock));
                return -1;
            }
        } else {
            ticks = _sal_usec_to_ticks(usec);
            if (!cyg_cond_timed_wait(&(em->wait), cyg_current_time()+ticks)) {
                cyg_mutex_unlock(&(em->lock));
                return -1;
            }
        }
    }
    
    if (em->count == 0) {
        em->owner = sal_thread_self();
    }
	   
    em->count++;
    cyg_mutex_unlock(&(em->lock));
    
    return rv;
}

int
sal_mutex_give(sal_mutex_t m)
{
    ecos_mutex_t *em = (ecos_mutex_t *) m;

    assert(!INT_CONTEXT());
    
    cyg_mutex_lock(&(em->lock));

    assert(em->owner == sal_thread_self());
    
    /* In release stage, the assertion may be turned off */
    if (em->owner != sal_thread_self()) {
        cyg_mutex_unlock(&(em->lock));
        return -1;
    }
    
    if (em->count > 0) {
        em->count--;
    }
    
    if (em->count == 0) {
        em->owner = NULL;
        cyg_cond_signal(&(em->wait));
    } 
    cyg_mutex_unlock(&(em->lock));
    
    return 0;
}


typedef struct sem_ctrl_s {
    cyg_sem_t	sem;
    cyg_flag_t	flag;
    int         binary;
} sem_ctrl_t;

sal_sem_t
sal_sem_create(char *desc, int binary, int initial_count)
{
    sem_ctrl_t *sem;
	
    assert(!INT_CONTEXT());
	
    if ((sem = (sem_ctrl_t*)sal_alloc(sizeof (sem_ctrl_t), desc)) == NULL) {
        return NULL;
    }
    
    if (binary) {
        sem->binary = 1;
        cyg_flag_init(&(sem->flag));
        if (initial_count >= 1)
            cyg_flag_setbits(&(sem->flag), 0x1);
    }
    else {
        sem->binary = 0;
        cyg_semaphore_init(&(sem->sem), initial_count);
    }

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_INCR(
            _sal_sem_count_curr,
            _sal_sem_count_max,
            ilock);
#endif
#endif
    return (sal_sem_t) sem;
}

void
sal_sem_destroy(sal_sem_t b)
{
    sem_ctrl_t *sem = (sem_ctrl_t *)b;
	
    assert(sem);
	
    if (sem->binary)
        cyg_flag_destroy(&(sem->flag));
    else
        cyg_semaphore_destroy(&(sem->sem));
    
#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
        SAL_SEM_RESOURCE_USAGE_DECR(
            _sal_sem_count_curr,
            ilock);
#endif
#endif
    sal_free(sem);
}

int
sal_sem_take(sal_sem_t b, int usec)
{
    sem_ctrl_t *sem = (sem_ctrl_t *)b;
    cyg_tick_count_t	ticks;
    int rv=0;
    
    assert(!INT_CONTEXT());
    assert(sem);

    if (usec == (uint32) sal_sem_FOREVER) {
    	
        if (sem->binary) {
    		      if (!cyg_flag_wait(&(sem->flag), 0x1, CYG_FLAG_WAITMODE_AND | CYG_FLAG_WAITMODE_CLR))
                rv = -1;
        }
        else {
            if (!cyg_semaphore_wait(&(sem->sem)))
                rv = -1;
        }
    } else {
    
        ticks = _sal_usec_to_ticks(usec);
        if (sem->binary) {
			
            if (!cyg_flag_timed_wait(&(sem->flag), 0x1, 
                                     CYG_FLAG_WAITMODE_AND | CYG_FLAG_WAITMODE_CLR,
                                     cyg_current_time()+ticks))
                rv = -1;
         }
         else {
            if (!cyg_semaphore_timed_wait(&(sem->sem), cyg_current_time()+ticks))
                rv = -1;
         }
    }
    return rv;
}

int
sal_sem_give(sal_sem_t b)
{
    sem_ctrl_t *sem = (sem_ctrl_t *)b;
    
    assert(sem);
    
    if (sem->binary) {
        cyg_flag_setbits(&(sem->flag), 0x1);
    }
    else {
        cyg_semaphore_post(&(sem->sem));
    }
    return 0;
}



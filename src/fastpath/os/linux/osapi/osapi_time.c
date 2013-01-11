/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_time.c
*
* @purpose   OSAPI time related functions
*
* @component osapi
*
* @comments
*
* @create    09/27/2001
*
* @author    John W. Linville
*
* @end
*
*********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>

#include <l7_common.h>
#include <osapi.h>
#include <log.h>

#include "osapi_priv.h"

typedef struct osapiTimerListEntry_s {

   osapiTimerDescr_t timer;
   struct timespec ts_expiry;
   struct osapiTimerListEntry_s *next, *prev;

} osapiTimerListEntry_t;

typedef struct osapiTimerAddEntry_s
{
	void				(*func)(L7_uint32, L7_uint32);
	L7_uint32			arg1;
	L7_uint32			arg2;
	L7_uint32			milliseconds;
	osapiTimerDescr_t	**pTimerHolder;

} osapiTimerAddEntry;

typedef struct osapiTimerChangeEntry_s
{
	osapiTimerDescr_t	*osapitimer;
	L7_uint32			newTimeCount;

} osapiTimerChangeEntry;

static osapiTimerListEntry_t *osapiTimerList = NULL;
static osapiTimerListEntry_t *osapiTimerExpired = NULL;
static osapiTimerListEntry_t *osapiTimerTmp = NULL;
static osapiTimerListEntry_t *osapiTimerListOrig = NULL;
static osapiTimerListEntry_t *osapiTimerListEndOrig = NULL;

static osapiTimerListEntry_t *osapiTimerFreeListHead = NULL;
static osapiTimerListEntry_t *osapiTimerFreeListTail = NULL;

#ifdef COMMENTED_OUT

static pthread_mutex_t osapiTimerFreeLock = PTHREAD_MUTEX_INITIALIZER;

#define OSAPI_TIMER_FREE_SEM_TAKE \
   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, \
                        (void *)&osapiTimerFreeLock); \
   pthread_mutex_lock(&osapiTimerFreeLock)

#define OSAPI_TIMER_FREE_SEM_GIVE pthread_cleanup_pop(1)

#endif /* COMMENTED_OUT */

static pthread_mutex_t osapiTimerLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t osapiPeriodicTimerLock = PTHREAD_MUTEX_INITIALIZER;

#define OSAPI_TIMER_SYNC_SEM_TAKE \
   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, \
                        (void *)&osapiTimerLock); \
   pthread_mutex_lock(&osapiTimerLock)

#define OSAPI_TIMER_SYNC_SEM_GIVE pthread_cleanup_pop(1)

#define OSAPI_TIMER_PERIODIC_SEM_TAKE \
   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock, \
                        (void *)&osapiPeriodicTimerLock); \
   pthread_mutex_lock(&osapiPeriodicTimerLock)

#define OSAPI_TIMER_PERIODIC_SEM_GIVE pthread_cleanup_pop(1)

static pthread_cond_t osapiTimerCond = PTHREAD_COND_INITIALIZER;

/**************************************************************************
* Provide periodic timer resources
*************************************************************************/
/* define the number of periodic timers (COUNT) and a miss cather (MISS_CNT) */
#define OSAPI_PERIODIC_TIMER_COUNT     20
#define OSAPI_PERIODIC_TIMER_MISS_CNT   2
typedef struct {
  L7_uint32     handle;
  L7_uint32     period;
  L7_uint32     nextTime;
  L7_uint32     taskId;
} OSAPI_PERIODIC_TIMER_t;
static OSAPI_PERIODIC_TIMER_t  osapiPeriodicTimer[OSAPI_PERIODIC_TIMER_COUNT + 1];

/**************************************************************************
* @purpose  Sleep for a given number of seconds.
*
* @param    sec @b{(input)}   number of seconds to sleep.
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiSleep( L7_uint32 sec) {

   struct timespec delay, remains;

   if ( sec == 0 ) {

     sec = 1;

   }

   delay.tv_sec = sec;
   delay.tv_nsec = 0;

   while (nanosleep (&delay, &remains) != 0) {

      delay = remains;

   }

   return;

}

/**************************************************************************
* @purpose  Sleep for a given number of micro seconds
*
* @param    usec @b{(input)}  Number of micro-seconds to sleep.
*
* @returns  none.
*
* @comments    CAUTION! The precision is in system ticks per second as
* @comments    determined by the system clock rate, even though the units
* @comments    are in microseconds.
*
* @end
*************************************************************************/
void osapiSleepUSec( L7_uint32 usec) {

   struct timespec delay, remains;

   if (usec < OSAPI_TICK_USEC) {

      usec = OSAPI_TICK_USEC;

   }

   delay.tv_sec = usec / 1000000;
   delay.tv_nsec = (usec % 1000000)*1000;

   while (nanosleep (&delay, &remains) != 0) {

      delay = remains;

   }

   return;

}

/**************************************************************************
* @purpose  Sleep for a given number of milliseconds
*
* @param    msec @b{(input)}  Number of milliseconds to sleep.
*
* @returns  none.
*
* @comments    CAUTION! The precision is in system ticks per second as
* @comments    determined by the system clock rate, even though the units
* @comments    are in milliseconds.
*
* @end
*************************************************************************/
void osapiSleepMSec( L7_uint32 msec) {

   struct timespec delay, remains;

   if (msec < (OSAPI_TICK_USEC / 1000)) {

      msec = (OSAPI_TICK_USEC / 1000);

   }

   delay.tv_sec = msec / 1000;
   delay.tv_nsec = (msec % 1000)*1000000;

   while (nanosleep (&delay, &remains) != 0) {

      delay = remains;

   }

   return;

}

/**************************************************************************
* @purpose  stop an already running timer
*
* @param    osapitimer ptr to an osapi timer descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiStopUserTimerMain (osapiTimerDescr_t *osapitimer)
{
	osapiTimerListEntry_t *curEntry = (osapiTimerListEntry_t *)osapitimer;
	L7_uint32 running;

	running = curEntry->timer.timer_running;

	if (running != 0)
	{
		if (curEntry->next != NULL) {

		   curEntry->next->prev = curEntry->prev;

		}

		if (curEntry->prev != NULL) {

		   curEntry->prev->next = curEntry->next;

		} else { /* This timer is head of list... */

			if (curEntry == osapiTimerList)
			{
				osapiTimerList = curEntry->next;

				/* Notify timer task of new list head... */
				pthread_cond_signal(&osapiTimerCond);
			}
		}

		curEntry->next = NULL;
		curEntry->prev = NULL;
		curEntry->timer.timer_running = 0;
	}

	return L7_SUCCESS;
}

/**************************************************************************
* @purpose  stop an already running timer
*
* @param    osapitimer ptr to an osapi timer descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiStopUserTimer (osapiTimerDescr_t *osapitimer)
{
	int SaveCancelType;

	if (osapitimer < (osapiTimerDescr_t *) osapiTimerListOrig)
	{
		osapi_printf("osapiStopUserTimer: Timer 0x%08x out of range (<)!\n", osapitimer);
		return L7_FAILURE;
	}
	if (osapitimer > (osapiTimerDescr_t *) osapiTimerListEndOrig)
	{
		osapi_printf("osapiStopUserTimer: Timer 0x%08x out of range (>)!\n", osapitimer);
		return L7_FAILURE;
	}

	OSAPI_TIMER_SYNC_SEM_TAKE;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &SaveCancelType);
	pthread_cleanup_push((void (*)(void *)) osapiStopUserTimerMain, (void *)osapitimer);
	pthread_cleanup_pop(1);
	pthread_setcanceltype(SaveCancelType, NULL);

	OSAPI_TIMER_SYNC_SEM_GIVE;

	return L7_SUCCESS;
}

/**************************************************************************
* @purpose  restarts a stopped timer
*
* @param    osapitimer ptr to an osapi timer descriptor to reset
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiRestartUserTimerMain (osapiTimerDescr_t *osapitimer)
{
	struct timeval curTime;
	osapiTimerListEntry_t *curEntry, *prevEntry;
	osapiTimerListEntry_t *newEntry = (osapiTimerListEntry_t *)osapitimer;
	L7_uint32 running, in_use;

	in_use = newEntry->timer.timer_in_use;

	/*
	If the timer has been freed before the call to
	osapiRestartUserTimer, make sure the timer does not get
	added to the queue
	*/

	if (in_use != 0)
	{
		/*
		If this timer is already running, don't start it again
		*/

		running = newEntry->timer.timer_running;

		if (running != 1)
		{
			if (gettimeofday(&curTime, NULL) != 0) {

			   LOG_EVENT(errno);

			}

			newEntry->ts_expiry.tv_sec = (curTime.tv_sec
									   + (newEntry->timer.time_count / 1000));
			newEntry->ts_expiry.tv_nsec = ((curTime.tv_usec
										  + ((newEntry->timer.time_count % 1000) * 1000))
										 * 1000);

			if (newEntry->ts_expiry.tv_nsec >= 1000000000) {

			   newEntry->ts_expiry.tv_nsec -= 1000000000;
			   newEntry->ts_expiry.tv_sec++;

			}

			/* Check for new timer list head... */
			if ((osapiTimerList == NULL)
			 || (newEntry->ts_expiry.tv_sec < osapiTimerList->ts_expiry.tv_sec)
			 || ((newEntry->ts_expiry.tv_sec == osapiTimerList->ts_expiry.tv_sec)
			  && (newEntry->ts_expiry.tv_nsec < osapiTimerList->ts_expiry.tv_nsec))) {

			   /* Notify timer task of new list head... */
			   pthread_cond_signal(&osapiTimerCond);

			   /* Establish proper links in chain... */
			   newEntry->next = osapiTimerList;
			   newEntry->prev = NULL;
			   osapiTimerList = newEntry;

			} else { /* Insert new timer at proper list position... */

			   curEntry = osapiTimerList;

			   do { /* Calculate correct time delta... */

				  prevEntry = curEntry;

				  curEntry = curEntry->next;

			   } while ((curEntry != NULL)
					 && ((newEntry->ts_expiry.tv_sec > curEntry->ts_expiry.tv_sec)
					  || ((newEntry->ts_expiry.tv_sec == curEntry->ts_expiry.tv_sec)
					   && (newEntry->ts_expiry.tv_nsec > curEntry->ts_expiry.tv_nsec))));

			   /* Establish proper links in chain... */
			   newEntry->next = curEntry;
			   prevEntry->next = newEntry;
			   newEntry->prev = prevEntry;

			}

			if (newEntry->next != NULL)
			{
			   newEntry->next->prev = newEntry;
			}

			newEntry->timer.timer_running = 1;
		}
	}

	return L7_SUCCESS;
}

/**************************************************************************
* @purpose  restarts a stopped timer
*
* @param    osapitimer ptr to an osapi timer descriptor to reset
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiRestartUserTimer (osapiTimerDescr_t *osapitimer)
{
	int SaveCancelType;

	if (osapitimer < (osapiTimerDescr_t *) osapiTimerListOrig)
	{
		osapi_printf("osapiRestartUserTimer: Timer 0x%08x out of range (<)!\n", osapitimer);
		return L7_FAILURE;
	}
	if (osapitimer > (osapiTimerDescr_t *) osapiTimerListEndOrig)
	{
		osapi_printf("osapiRestartUserTimer: Timer 0x%08x out of range (>)!\n", osapitimer);
		return L7_FAILURE;
	}

	OSAPI_TIMER_SYNC_SEM_TAKE;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &SaveCancelType);
	pthread_cleanup_push((void (*)(void *)) osapiRestartUserTimerMain, (void *)osapitimer);
	pthread_cleanup_pop(1);
	pthread_setcanceltype(SaveCancelType, NULL);

	OSAPI_TIMER_SYNC_SEM_GIVE;

	return L7_SUCCESS;
}

/**************************************************************************
* @purpose  change a running timer's count value
*
* @param    osapitimer ptr to an osapi timer descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiChangeUserTimerMain(osapiTimerChangeEntry *Entry)
{
	osapiStopUserTimerMain(Entry->osapitimer);

	(Entry->osapitimer)->time_count = Entry->newTimeCount;
	(Entry->osapitimer)->orig_count = Entry->newTimeCount;

	osapiRestartUserTimerMain(Entry->osapitimer);

	return L7_SUCCESS;
}

/**************************************************************************
* @purpose  change a running timer's count value
*
* @param    osapitimer ptr to an osapi timer descriptor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if osapitimer is not in timer table
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiChangeUserTimer(osapiTimerDescr_t *osapitimer,
                             L7_uint32 newTimeCount)
{
	osapiTimerChangeEntry Entry;
	int SaveCancelType;

	if (osapitimer < (osapiTimerDescr_t *) osapiTimerListOrig)
	{
		osapi_printf("osapiChangeUserTimer: Timer 0x%08x out of range (<)!\n", osapitimer);
		return L7_FAILURE;
	}
	if (osapitimer > (osapiTimerDescr_t *) osapiTimerListEndOrig)
	{
		osapi_printf("osapiChangeUserTimer: Timer 0x%08x out of range (>)!\n", osapitimer);
		return L7_FAILURE;
	}

	Entry.osapitimer = osapitimer;
	Entry.newTimeCount = newTimeCount;

	OSAPI_TIMER_SYNC_SEM_TAKE;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &SaveCancelType);
	pthread_cleanup_push((void (*)(void *)) osapiChangeUserTimerMain, (void *)&Entry);
	pthread_cleanup_pop(1);
	pthread_setcanceltype(SaveCancelType, NULL);

	OSAPI_TIMER_SYNC_SEM_GIVE;

	return L7_SUCCESS;
}

/**************************************************************************
* @purpose  This is the user function to setup a new timeout call.
*
* @param    func           is the function to call when expired.
* @param    arg(2)         arguments to pass to the callback function.
* @param    milliseconds   number of milli-seconds to wait before timeout.
* @param    pTimerHolder   ptr to an osapiTimerDescr_t struct used to deallocate this timer by the user
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiTimerAddMain (osapiTimerAddEntry *Entry)
{
	osapiTimerListEntry_t *tmpEntry;

	/* this is not a mistake; head of osapiTimerListEntry_t is an
	  osapiTimerDescr_t */

	if ((*(Entry->pTimerHolder) = (osapiTimerDescr_t *)osapiTimerFreeListHead) != NULL)
	{
		tmpEntry = osapiTimerFreeListHead;
		osapiTimerFreeListHead = osapiTimerFreeListHead->next;

		if (osapiTimerFreeListHead == NULL)
		{
			osapiTimerFreeListTail = NULL;
		}

		tmpEntry->next = NULL;

		(*(Entry->pTimerHolder))->callback = Entry->func;
		(*(Entry->pTimerHolder))->parm1 = Entry->arg1;
		(*(Entry->pTimerHolder))->parm2 = Entry->arg2;
		(*(Entry->pTimerHolder))->timer_in_use = 1;
		(*(Entry->pTimerHolder))->timer_running = 0;
		(*(Entry->pTimerHolder))->time_count = Entry->milliseconds;
		(*(Entry->pTimerHolder))->orig_count = Entry->milliseconds;
	}
	else
	{
		osapi_printf("osapiTimerAddMain: No free timers available!\n");
	}

	if (*(Entry->pTimerHolder) != NULL)
	{
		osapiRestartUserTimerMain(*(Entry->pTimerHolder));
	}

	return;
}

/**************************************************************************
* @purpose  This is the user function to setup a new timeout call.
*
* @param    func           is the function to call when expired.
* @param    arg(2)         arguments to pass to the callback function.
* @param    milliseconds   number of milli-seconds to wait before timeout.
* @param    pTimerHolder   ptr to an osapiTimerDescr_t struct used to deallocate this timer by the user
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiTimerAdd(void (*func)( L7_uint32, L7_uint32 ),
                   L7_uint32 arg1, L7_uint32 arg2,
                   L7_int32 milliseconds, osapiTimerDescr_t **pTimerHolder)
{
	osapiTimerAddEntry Entry;
	int SaveCancelType;

	if (func == NULL)
	{
		osapi_printf("osapiTimerAdd: Timer with NULL callback NOT added!\n");
		return;
	}

	Entry.func = func;
	Entry.arg1 = arg1;
	Entry.arg2 = arg2;
	Entry.milliseconds = milliseconds;
	Entry.pTimerHolder = pTimerHolder;

	OSAPI_TIMER_SYNC_SEM_TAKE;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &SaveCancelType);
	pthread_cleanup_push((void (*)(void *)) osapiTimerAddMain, (void *)&Entry);
	pthread_cleanup_pop(1);
	pthread_setcanceltype(SaveCancelType, NULL);

	OSAPI_TIMER_SYNC_SEM_GIVE;

	return;

}

/**************************************************************************
* @purpose  Remove a function from the timeout list.
*
* @param    pTimerHolder   ptr to an osapi timer descriptor, that was returned in osapiTimerAdd(), to deallocate.
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiTimerFreeMain(osapiTimerDescr_t *pTimer)
{
	L7_uint32	in_use;

	in_use = pTimer->timer_in_use;

	/*
	Don't allow a timer that has just been freed to be freed
	again
	*/

	if (in_use != 0)
	{
	   osapiStopUserTimerMain(pTimer);

	   /* this is not a mistake; head of osapiTimerListEntry_t is an
		  osapiTimerDescr_t */

	   if (osapiTimerFreeListTail == NULL)
	   {
		   osapiTimerFreeListTail = (osapiTimerListEntry_t *)pTimer;
		   osapiTimerFreeListHead = (osapiTimerListEntry_t *)pTimer;
	   } else
	   {
		   osapiTimerFreeListTail->next = (osapiTimerListEntry_t *)pTimer;
		   osapiTimerFreeListTail = osapiTimerFreeListTail->next;
	           ((osapiTimerListEntry_t *)pTimer)->next = NULL;
	   }
		

	   /*
	   osapiStopUserTimerMain does set the prev pointer to null,
	   but it may not execute if the timer has already stopped
	   */

	   ((osapiTimerListEntry_t *)pTimer)->prev = NULL;


	   pTimer->callback = NULL;
	   pTimer->timer_in_use = 0;
	}

	return;
}

/**************************************************************************
* @purpose  Remove a function from the timeout list.
*
* @param    pTimerHolder   ptr to an osapi timer descriptor, that was returned in osapiTimerAdd(), to deallocate.
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiTimerFree(osapiTimerDescr_t *pTimer)
{
	int SaveCancelType;

    if (pTimer == L7_NULL) 
    {
        return;
    }

	if (pTimer < (osapiTimerDescr_t *) osapiTimerListOrig)
	{
		osapi_printf("osapiTimerFree: Timer 0x%08x out of range (<)!\n", pTimer);
		return;
	}
	if (pTimer > (osapiTimerDescr_t *) osapiTimerListEndOrig)
	{
		osapi_printf("osapiTimerFree: Timer 0x%08x out of range (>)!\n", pTimer);
		return;
	}

	OSAPI_TIMER_SYNC_SEM_TAKE;

	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &SaveCancelType);
	pthread_cleanup_push((void (*)(void *)) osapiTimerFreeMain, (void *)pTimer);
	pthread_cleanup_pop(1);
	pthread_setcanceltype(SaveCancelType, NULL);

	OSAPI_TIMER_SYNC_SEM_GIVE;

	return;
}

/**************************************************************************
* @purpose  Task that wakes up periodically and invokes active timers.
*
* @param    none.
*
* @returns  none.
*
* @comments    none.
*
* @end
*************************************************************************/
void osapiTimerHandler(void)
{
   int i;
   osapiTimerDescr_t expTimer;
   struct timespec pop_time;
   struct timespec diff_time;

   expTimer.parm1 = 0;
   expTimer.parm2 = 0;
   OSAPI_TIMER_SYNC_SEM_TAKE;

   /* Allocate and initialize timer free list */
   osapiTimerFreeListHead = (osapiTimerListEntry_t *)osapiMalloc(L7_OSAPI_COMPONENT_ID, 
                              sizeof(osapiTimerListEntry_t) * OSAPI_MAX_TIMERS);

   if (osapiTimerFreeListHead != NULL) {

      osapiTimerTmp = osapiTimerFreeListHead;

      for (i = 0; i < (OSAPI_MAX_TIMERS - 1); i++) {

         osapiTimerTmp->next = (osapiTimerTmp + 1);
         osapiTimerTmp++;

      }

      osapiTimerFreeListTail = osapiTimerTmp;
      osapiTimerListEndOrig = osapiTimerTmp;
      osapiTimerTmp = osapiTimerTmp->next = NULL;

   }

   osapiTimerListOrig = osapiTimerFreeListHead;

   OSAPI_TIMER_SYNC_SEM_GIVE;

   (void) osapiTaskInitDone(L7_OSAPI_TIMER_TASK_SYNC);

   if (osapiTimerFreeListHead == NULL) {

      return; /* kills this task */

   }

   expTimer.callback = NULL;
   expTimer.parm1 = 0;
   expTimer.parm2 = 0;

   for (;;) {

      OSAPI_TIMER_SYNC_SEM_TAKE;

      if (osapiTimerList == NULL) {

         pthread_cond_wait(&osapiTimerCond, &osapiTimerLock);

      } else {

         /* setup time value */

         if (pthread_cond_timedwait(&osapiTimerCond, &osapiTimerLock,
                                    &(osapiTimerList->ts_expiry)) != 0) {

            /* Get popped timer... */
            osapiTimerExpired = osapiTimerList;

			if (osapiTimerExpired != NULL)
			{
				/*
				Note that the pthread_cond_timedwait must unlock the
				osapiTimerLock semaphore.  That means that the list
				could have changed while waiting on the timer.
				*/

				/* copy the time we popped for later use */
				memcpy(&pop_time,&(osapiTimerExpired->ts_expiry),sizeof(struct timespec));

				/* Advance to next list entry... */
				osapiTimerList = osapiTimerExpired->next;

				if (osapiTimerList != NULL)
				{
					/*
					Head of list must have NULL prev pointer, or
					a subsequent osapiStopUserTimer call will
					not notice that the timer stopped was at the
					head of the list
					*/

					osapiTimerList->prev = NULL;
				}

				if (osapiTimerExpired->timer.callback == NULL)
				{
					osapi_printf("osapiTimerHandler: Timer 0x%08x callback NULL!, next 0x%08x, prev 0x%08x\n",
								 &(osapiTimerExpired->timer),
								 osapiTimerExpired->next,
								 osapiTimerExpired->prev);
				}

				osapiTimerExpired->timer.time_count = 0;
				osapiTimerExpired->timer.timer_running = 0;

				expTimer.callback = osapiTimerExpired->timer.callback;
				expTimer.parm1 = osapiTimerExpired->timer.parm1;
				expTimer.parm2 = osapiTimerExpired->timer.parm2;

				osapiTimerFreeMain(&(osapiTimerExpired->timer));

				osapiTimerExpired = NULL;
			}
         }
      }

      OSAPI_TIMER_SYNC_SEM_GIVE;

      if (expTimer.callback != NULL)
	  {
		 /* Execute popped timer's callback... */
		 (*expTimer.callback)(expTimer.parm1, expTimer.parm2);

		 expTimer.callback = NULL;
		 expTimer.parm1 = 0;
		 expTimer.parm2 = 0;

          /* set diff time to invalid values to ensure that we
          * don't find ourselves in a situation where
          * osapiTimerList is NULL when testing the if conditional
          * but non NULL when testing the while loop condition
          * if that happens we may be expiring a timer that is not
          * yet ready to pop
          * This situation can happen if our schedule quanta
          * expires after the if but before the while
          */

         diff_time.tv_sec = 0x7fffffff; /*signed value*/
         diff_time.tv_nsec = 0x7fffffff; /*signed value*/

         /* Check to see if any subsequent timers have expired */
         if(osapiTimerList != NULL)
         {
            diff_time.tv_sec = osapiTimerList->ts_expiry.tv_sec - pop_time.tv_sec;
            diff_time.tv_nsec = osapiTimerList->ts_expiry.tv_nsec - pop_time.tv_nsec;
         }

         while((osapiTimerList != NULL) &&
               (diff_time.tv_sec == 0) &&
               (diff_time.tv_nsec < 10000000) &&
               (diff_time.tv_nsec > 0))
          {
            /*
             *falling into the while loop condition
             *indicates that the next timer pop is
             *less than 1 jiffy away (10000000 nanoseconds)
             *which means we may as well handle it now
             *The alternative is to sleep until we are
             *next scheduled, which will guarantee
             *a timer skew
             */

            OSAPI_TIMER_SYNC_SEM_TAKE;

            /* Get popped timer... */
            osapiTimerExpired = osapiTimerList;

			if (osapiTimerExpired != NULL)
			{
				/* Advance to next list entry... */
				osapiTimerList = osapiTimerExpired->next;

				if (osapiTimerList != NULL)
				{
					/*
					Head of list must have NULL prev pointer, or
					a subsequent osapiStopUserTimer call will
					not notice that the timer stopped was at the
					head of the list
					*/

					osapiTimerList->prev = NULL;
				}

				osapiTimerExpired->timer.time_count = 0;
				osapiTimerExpired->timer.timer_running = 0;

				expTimer.callback = osapiTimerExpired->timer.callback;
				expTimer.parm1 = osapiTimerExpired->timer.parm1;
				expTimer.parm2 = osapiTimerExpired->timer.parm2;

				osapiTimerFreeMain(&(osapiTimerExpired->timer));

				osapiTimerExpired = NULL;
			}

			OSAPI_TIMER_SYNC_SEM_GIVE;

			if (expTimer.callback != NULL)
			{
			   /* Execute popped timer's callback... */
			   (*expTimer.callback)(expTimer.parm1, expTimer.parm2);

			   expTimer.callback = NULL;
			   expTimer.parm1 = 0;
			   expTimer.parm2 = 0;
			}

            /*
             *recompute the difference to the next timer in the list
             *Note: We don't move the pop_time structure forward here.
             *not sure if we should or not. It seems doing so would
             *open up the possibility that the timer may begin to
             *execute timers before they actually expire (by more
             *than 1 jiffy at least)
             */

            diff_time.tv_sec = 0x7fffffff;
            diff_time.tv_nsec = 0x7fffffff;

            if(osapiTimerList != NULL)
            {
               diff_time.tv_sec = osapiTimerList->ts_expiry.tv_sec - pop_time.tv_sec;
               diff_time.tv_nsec = osapiTimerList->ts_expiry.tv_nsec - pop_time.tv_nsec;
            }

         } /* end while */
      } /* end if */
   } /* end for */

   return;

}

/**************************************************************************
* @purpose  Provide periodic timer indications to delay sensitive tasks. Use
*           of these utilities minimizes accumulated skew.
*
* @param    L7_uint32  period   The fixed period.
*           L7_uint32 *handle   User provided storage for assigned handle.
*
* @returns  Failure message is no timers remain
*
* @comments The first available timer is assigned and the handle is returned
*           via the handle argument.
*
* @end
*************************************************************************/
L7_RC_t osapiPeriodicUserTimerRegister(L7_uint32 period, L7_uint32 *handle)
{
  L7_uint32 index;
  L7_RC_t rc = L7_TABLE_IS_FULL;

  OSAPI_TIMER_PERIODIC_SEM_TAKE;
  for (index = 1; index < OSAPI_PERIODIC_TIMER_COUNT; index++)
  {
    if (osapiPeriodicTimer[index].handle == L7_NULL)
    {
      osapiPeriodicTimer[index].handle   = index;
      osapiPeriodicTimer[index].period   = period;
      osapiPeriodicTimer[index].nextTime = osapiTimeMillisecondsGet();
      osapiPeriodicTimer[index].taskId   = L7_NULL;
      *handle = osapiPeriodicTimer[index].handle;
      rc = L7_SUCCESS;
	  break;
    }
  }

  OSAPI_TIMER_PERIODIC_SEM_GIVE;
  return rc;
}

/**************************************************************************
* @purpose  Wait on a previously defined periodic timer.
*
* @param    L7_uint32 handle   Assigned periodic timer handle.
*
* @returns  Nothing
*
* @comments Timer period previously supplied is used.
*
* @end
*************************************************************************/
void osapiPeriodicUserTimerWait(L7_uint32 handle)
{
  L7_uint32 now, waitTime;

  OSAPI_TIMER_PERIODIC_SEM_TAKE;
  osapiPeriodicTimer[handle].nextTime += osapiPeriodicTimer[handle].period;
  now = osapiTimeMillisecondsGet();
  waitTime = osapiPeriodicTimer[handle].nextTime - now;


  if((now > osapiPeriodicTimer[handle].nextTime) || (waitTime > osapiPeriodicTimer[handle].period))
  {
    osapiPeriodicTimer[handle].nextTime = now + osapiPeriodicTimer[handle].period;
    waitTime = osapiPeriodicTimer[handle].period;
  }

  OSAPI_TIMER_PERIODIC_SEM_GIVE;
  osapiSleepMSec(waitTime);
}

/**************************************************************************
* @purpose  Wait on a previously defined periodic timer.
*
* @param    L7_uint32 handle   Assigned periodic timer handle.
*
* @returns  Nothing
*
* @comments Timer period previously supplied is used.
*
* @end
*************************************************************************/
L7_RC_t osapiPeriodicUserTimerDeregister(L7_uint32 handle)
{
  if ((L7_NULL == osapiPeriodicTimer[handle].taskId) &&
      (handle  == osapiPeriodicTimer[handle].handle))
  {
    osapiPeriodicTimer[handle].handle = L7_NULL;
    osapiPeriodicTimer[handle].taskId = L7_NULL;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

void osapiPrintTimerDetail(osapiTimerDescr_t *ptimer)
{
	osapiTimerListEntry_t *Entry = (osapiTimerListEntry_t *) ptimer;

	osapi_printf("  Timer.callback: 0x%08x\n", ptimer->callback);
	osapi_printf("  Timer.parm1: 0x%08x\n", ptimer->parm1);
	osapi_printf("  Timer.parm2: 0x%08x\n", ptimer->parm2);
	osapi_printf("  Timer.timer_running: %d\n", ptimer->timer_running);
	osapi_printf("  Timer.timer_in_use: %d\n", ptimer->timer_in_use);
	osapi_printf("  Timer.time_count: %d\n", ptimer->time_count);
	osapi_printf("  Timer.orig_count: %d\n", ptimer->orig_count);
	osapi_printf("  next: 0x%08x\n", Entry->next);
	osapi_printf("  prev: 0x%08x\n", Entry->prev);
}

void osapiPrintTimerList(int type, int detail)
{
	int i = 0;

	OSAPI_TIMER_SYNC_SEM_TAKE;

	if (type == 0)
	{
		osapiTimerTmp = osapiTimerFreeListHead;
	}
	else if (type == 1)
	{
		osapiTimerTmp = osapiTimerList;
	}

	while (osapiTimerTmp != NULL)
	{
		if (detail == 0)
		{
			osapi_printf("Timer %d, 0x%08x, running: %d, next: 0x%08x, prev: 0x%08x\n",
						 i, osapiTimerTmp,
						 osapiTimerTmp->timer.timer_running,
						 osapiTimerTmp->next,
						 osapiTimerTmp->prev);
		}
		else
		{
			osapi_printf("Timer %d, 0x%08x:\n", i, osapiTimerTmp);
			osapiPrintTimerDetail(&(osapiTimerTmp->timer));
		}

		i++;
		osapiTimerTmp = osapiTimerTmp->next;
	}

	OSAPI_TIMER_SYNC_SEM_GIVE;

	return;
}

void osapiPrintOrphanTimers(int type, int detail, int start, int end)
{
	int i;

	OSAPI_TIMER_SYNC_SEM_TAKE;

	if (start < 0)
	{
		start = 0;
	}

	if ((end <= 0) || (end > OSAPI_MAX_TIMERS))
	{
		end = OSAPI_MAX_TIMERS;
	}

	if (start > end)
	{
		start = end;
	}

	for (i=start; i<end; i++)
	{
		if (type == 0)
		{
			if (osapiTimerListOrig[i].timer.timer_in_use == 1 &&
				osapiTimerListOrig[i].next == NULL &&
				osapiTimerListOrig[i].prev == NULL)
			{
				if (detail == 0)
				{
					osapi_printf("Timer %d, 0x%08x, running: %d, next: 0x%08x, prev: 0x%08x\n",
								 i, &(osapiTimerListOrig[i]),
								 osapiTimerListOrig[i].timer.timer_running,
								 osapiTimerListOrig[i].next,
								 osapiTimerListOrig[i].prev);
				}
				else
				{
					osapi_printf("Timer %d, 0x%08x:\n", i, &(osapiTimerListOrig[i]));
					osapiPrintTimerDetail(&(osapiTimerListOrig[i].timer));
				}
			}
		}
		else if (type == 1)
		{
			if (detail == 0)
			{
				osapi_printf("Timer %d, 0x%08x orphaned, running = %d\n", i, &(osapiTimerListOrig[i]),
							 osapiTimerListOrig[i].timer.timer_running);
			}
			else
			{
				osapi_printf("Timer %d, 0x%08x:\n", i, &(osapiTimerListOrig[i]));
				osapiPrintTimerDetail(&(osapiTimerListOrig[i].timer));
			}
		}
	}

	OSAPI_TIMER_SYNC_SEM_GIVE;
}


/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <sys/types.h>

#include <stdlib.h>

#include "sr_type.h"
#include "sr_time.h"	/* all time related .h's pulled in here */

#include <sys/select.h>

#include <malloc.h>


#include <string.h>




#include "sr_snmp.h"
#include "tmq.h"
#include "diag.h"
SR_FILENAME


/* %%% DLB need queue size control code. */

static SR_UINT32 NextUnusedTimerId
    SR_PROTOTYPE((TimerQueue *TmQ));
static SR_UINT32 TimerIdUsed
    SR_PROTOTYPE((const TimerQueue *TmQ,
                  SR_UINT32 i));

/* 
 * timerq_update and previous_time are used to check a wrap of 
 * sysUpTime so that the timer queue can be updated on a wrap of 
 * sysUpTime.
 */
extern int timerq_update;
extern SR_UINT32 previous_time;


#define TMQ_SIZE  (sizeof(TimerQueue) + 2 * sizeof(TimerEvent))

TimerQueue *
InitTimerQueue(apinfo)
void *apinfo;
{
    TimerQueue *TmQ;

    if((TmQ = (TimerQueue *)malloc(TMQ_SIZE)) == NULL) {
	return NULL;
    }
#ifdef SR_CLEAR_MALLOC
    memset((char *)TmQ, 0, TMQ_SIZE);
#endif	/* SR_CLEAR_MALLOC */

    TmQ->head = (TimerEvent *)(((char *)TmQ) +
			       sizeof(TimerQueue));

    TmQ->tail = (TimerEvent *)(((char *)TmQ) +
			       sizeof(TimerQueue) +
			       sizeof(TimerEvent));

    TmQ->head->when = 0x00000000;
    TmQ->head->period = 0;
    TmQ->head->id = 0;
    TmQ->head->info = NULL;
    TmQ->head->prev = NULL;
    TmQ->head->next = TmQ->tail;
    TmQ->head->fired = FALSE;
    TmQ->tail->invoke = DONOT_CALL_CALLBACK;
    TmQ->head->type = CURRENT;


    TmQ->tail->when = 0xffffffff;
    TmQ->tail->period = 0;
    TmQ->tail->id = 0;
    TmQ->tail->info = NULL;
    TmQ->tail->prev = TmQ->head;
    TmQ->tail->next = NULL;
    TmQ->tail->fired = FALSE;
    TmQ->tail->invoke = DONOT_CALL_CALLBACK;
    TmQ->tail->type = CURRENT;

    TmQ->start = 1;

    TmQ->apinfo = apinfo;

    return TmQ;
}

void
DestroyTimerQueue(TmQ)
TimerQueue *TmQ;
{
    TimerEvent *t, *next;
    TimeOutDescriptor *tdp = NULL;

    if(TmQ == NULL) return;
    for(t = TmQ->head->next; t != TmQ->tail; t = next) {
        if (t->invoke == DONOT_CALL_CALLBACK) {
            tdp = (TimeOutDescriptor *)t->info;
            if (tdp != NULL) {
                free(tdp);
                tdp = NULL;
                t->info = NULL;
             }
        }
	next = t->next;
	free((char *)t);
    }
}

int
TimeQEmpty(TmQ)
    const TimerQueue *TmQ;
{
    if(TmQ == NULL) return 1;
    return TmQ->head->next == TmQ->tail;
}

int
TimersInQ(TmQ)
    const TimerQueue *TmQ;
{
    if(TmQ == NULL) return 0;
    return !(TimeQEmpty(TmQ));
}

void
TimeQInsertBefore(TmQ, newtm, beforetm)
     TimerQueue *TmQ;
     TimerEvent     *newtm, *beforetm;
{
    FNAME("TimeQInsertBefore")

    if(TmQ == NULL) return;
    newtm->next = beforetm;
    newtm->prev = beforetm->prev;
    beforetm->prev->next = newtm;
    beforetm->prev = newtm;
    if(TimeQEmpty(TmQ)) {
	DPRINTF((APWARN, "%s: Timer queue empty after inserting timer\n",
		 Fname));
    }
}

void
TimeQEnqueue(TmQ, tm)
     TimerQueue *TmQ;
     TimerEvent     *tm;
{
    TimerEvent     *p;

    if(TmQ == NULL) return;
    if (tm->type == WRAP) {
        /*
         * enqueue wrapped value of timer
         */
        for (p = TmQ->head->next; 
                  ((p != TmQ->tail) && (p->type == 0)); p = p->next) {
	    ;
        }
        for (/* p */; tm->when > p->when; p = p->next) {
            ;
        }
    }
    else {
        for (p = TmQ->head->next; tm->when > p->when; p = p->next) {
	    ;
        }
    }
    TimeQInsertBefore(TmQ, tm, p);
}

static SR_UINT32
TimerIdUsed(TmQ, i)
     const TimerQueue *TmQ;
     SR_UINT32   i;
{
    TimerEvent *p;

    if(TmQ == NULL) return 0;
    for(p = TmQ->head->next; p != TmQ->tail; p = p->next) {
	if(i == p->id) {
	    return 1;
	}
    }
    return 0;
}

#define successor(i) (((i)==0xfffffffe)?1:((i)+1))

static SR_UINT32
NextUnusedTimerId(TmQ)
    TimerQueue *TmQ;
{
    SR_UINT32 i;


    if(TmQ == NULL) return 0xffffffff;
    i = TmQ->start;
    while(1) {
	if(!TimerIdUsed(TmQ, i)) {
	    TmQ->start = successor(i);
	    return i;
	}

	i = successor(i);

	if(i == TmQ->start) {
	    break;
	}
    }
    DPRINTF((APWARN, "4 billion outstanding timers??\n"));
    return 0xffffffff;
}


/* Remove and return the indicated timer event from the indicated queue. */
TimerEvent     *
TimeQDequeue(TmQ, tm)
     TimerQueue *TmQ;
     TimerEvent     *tm;
{
    FNAME("TimeQDequeue")

    if(TmQ == NULL) return NULL;
    if (tm == TmQ->head || tm == TmQ->tail) {
	DPRINTF((APERROR, "%s: attempt to remove head or tail sentinal\n",
		 Fname));
	return NULL;
    }
    tm->prev->next = tm->next;
    tm->next->prev = tm->prev;

    tm->next = NULL;
    tm->prev = NULL;

    return tm;
}

TimerEvent     *
TimeQFirst(TmQ)
     TimerQueue *TmQ;
{
    FNAME("TimeQFirst")

    if(TmQ == NULL) return NULL;
    if (TimeQEmpty(TmQ)) {
	DPRINTF((APERROR, "%s: attempt to get first in empty queue\n", Fname));
	return NULL;
    }
    return TimeQDequeue(TmQ, TmQ->head->next);
}

int
CheckTimers(TmQ)
     TimerQueue *TmQ;
{
    TimerEvent     *p = NULL, *rq = NULL;
    TimeOutDescriptor *tdp = NULL, *tmp_tdp = NULL;
    SR_UINT32      curtime;
    int            wrap = 0;

    if (TmQ == NULL) {
	return -1;
    }
    p = TmQ->head->next;
    curtime = GetTimeNowWrap(&wrap);

    /* 
     * detect clock wrap(sysUpTime) and adjust timer value
     * in the timer queue.
     */

    if ((wrap) || (timerq_update)) {
	for (p = TmQ->head->next; p != TmQ->tail; p = p->next) {
            if (p->type == CURRENT) {
                /* 
                 * all "CURRENT" type should be fired on a clock wrap.
                 */
                p->when = 0;
            }
            else if (p->type == WRAP) {
                /*
                 * change all timer type WRAP to CURRENT.
                 */
                p->type = CURRENT;
            }
        }
        timerq_update = 0;
    }

    p = TmQ->head->next;
    while (p != TmQ->tail && p->when <= curtime && p->type != WRAP) {

        if (p->invoke == DONOT_CALL_CALLBACK) {
            if (p->info != NULL) {
                tmp_tdp = 
                    (TimeOutDescriptor *)malloc(sizeof(TimeOutDescriptor));
                if (tmp_tdp == NULL) {
                    return -1;
                }
	        tdp = (TimeOutDescriptor *)p->info; 
	        memcpy(tmp_tdp, tdp, sizeof(TimeOutDescriptor));
	        if (PostTimerHook(TmQ, p->id, 
                                  (void *)tmp_tdp, p->invoke) == -1) {
                    free(tmp_tdp);
                    tmp_tdp = NULL;
	            return -1;
	        }
            }
        }
        else {
	    if (PostTimerHook(TmQ, p->id, p->info, p->invoke) == -1) {
	        return -1;
            }
	}
        p->fired = TRUE;
        p = p->next;
    }

    p = TmQ->head->next;
    while (p != TmQ->tail && p->when <= curtime && p->type != WRAP) {
        rq = p;
        p = p->next;
        if (rq->fired) {
	    rq->fired = FALSE;
	    if (rq->period == ((SR_INT32)0)) {
	        /* Delete oneshot timers. */

                if (rq->invoke == DONOT_CALL_CALLBACK) {
                    tdp = (TimeOutDescriptor *)rq->info;
                    if (tdp != NULL) {
                        free(tdp);
                        tdp = NULL;
                        rq->info = NULL;
                    }
                }
	        free((char *) TimeQDequeue(TmQ, rq));
	    }
	    else {
	        /* Handle rescheduling periodic timers. */

	        rq = TimeQDequeue(TmQ, rq);

                /* Need to schedule next timeout for curtime + period */
	        rq->when = curtime + rq->period;

                /* check if value of timer is wrapped */
                if (rq->when < curtime) {
                    rq->type = WRAP;
                }
                else {
                    rq->type = CURRENT;
                }
	        TimeQEnqueue(TmQ, rq);
	    }
        }
    }
    return 1;
}

/* Time units are hundreths of a second. */

/* if CanSetTimeout does not return -1, the next call to SetPeriodicTimeout
   or SetOneshotTimeout is guaranteed to succeed. */

int
TmCanSetTimeout(TmQ)
TimerQueue *TmQ;
{
    FNAME("TmCanSetTimeout")

    if(TmQ == NULL) {
      DPRINTF((APWARN, "%s: incoming NULL parameter?\n", Fname));
      return -1;
    }
    if (TmQ->tm != NULL) {
	DPRINTF((APWARN, "%s: timer already set in TimerQueue?\n", Fname));
	return 1;
    }
    if ((TmQ->tm = (TimerEvent *) malloc(sizeof(TimerEvent))) != NULL) {
	return 1;
    }
    DPRINTF((APWARN, "%s: failed to malloc TimerEvent\n", Fname));
    return -1;
}

/* call this if you called CanSetTimeout but decided you don't need to
   set the timeout afterall */
void
TmDontNeedTimeout(TmQ)
TimerQueue *TmQ;
{
    FNAME("DontNeedTimeout")

    if(TmQ == NULL) return;
    if (TmQ->tm == NULL) {
	DPRINTF((APWARN, "%s: CanSetTimeout not successfully called\n", Fname));
    }
    free((char *) TmQ->tm);
    TmQ->tm = NULL;
}


/*
 * sr_SetPeriodicTimeout
 * 
 * this function set periodic timeout by arranging Ev_Time_Out 
 * event based on the incoming timeout type.
 *
 * return value
 *    success : timer id
 *    fail    : -1
 */


int
sr_SetPeriodicTimeout(TimerQueue  *TmQ,
                      SR_UINT32   when,
                      SR_UINT32   period,
		      void        *info,
                      SR_INT32    invoke)
{


    /*
     * Arrange for an Ev_Time_Out event to be posted every t time units
     * starting at time when. Returns a timer id which can be passed
     * to CancelTimeout().
     */
    int             id;
    SR_UINT32       curtime;
    TimerEvent      *p = NULL;
    int             wrap = 0;

    FNAME("sr_SetPeriodicTimeout")

    if(TmQ == NULL) {
      DPRINTF((APWARN, "%s: incoming NULL TmQ parameter?\n", Fname));
      return -1;
    }
    /*
     * we MUST NOT FAIL if this is the first timeout set since CanSetTimeout
     * returned success
     */

    if (TmQ->tm == NULL) {
	if ((TmQ->tm = (TimerEvent *) malloc(sizeof(TimerEvent))) == NULL) {
	    DPRINTF((APWARN, "%s: failed to malloc TimerEvent\n", Fname));
	    return -1;
	}
    }

    /* 
     * detect clock wrap(sysUpTime) and adjust timer value
     * in the timer queue.
     */

    curtime = GetTimeNowWrap(&wrap);
    if ((wrap) || (timerq_update == 1)) {
	for( p = TmQ->head->next; p != TmQ->tail; p = p->next) {
            if (p->type == CURRENT) {
                /* 
                 * all "CURRENT" type should be fired on a clock wrap.
                 */
                p->when = 0;
            }
            else if (p->type == WRAP) {
                /*
                 * change all timer type WRAP to CURRENT.
                 */
                p->type = CURRENT;
            }
        }
        timerq_update = 0;
        /* 
         * it is possible that "when" is calculated and passed before 
         * a clock wrap. fire immediately.
         */
        if (previous_time < when) {
            when = 0;
        }
        TmQ->tm->type = CURRENT;
    }
    else {
        TmQ->tm->type = CURRENT;
        /* 
         * it is possible that there is a slight time gap between
         * "when" and "curtime".
         * if time gap is less than 1sec, then put it as CURRENT
         * type timer.
         */
        if (curtime > 100) {
            if (when < (curtime - 100)) {
                TmQ->tm->type = WRAP;
            }
        }
    }
    TmQ->tm->when = when;
    TmQ->tm->period = period;
    id = TmQ->tm->id = NextUnusedTimerId(TmQ);
    TmQ->tm->info = info;
    TmQ->tm->fired = FALSE;
    TmQ->tm->invoke = invoke;
    TimeQEnqueue(TmQ, TmQ->tm);

    TmQ->tm = NULL;
    return id;
}

int
TmSetPeriodicTimeout(TmQ, when, period, info)
     TimerQueue  *TmQ;
     SR_UINT32   when;
     SR_UINT32   period;
     void        *info;
{
    /*
     * Arrange for an Ev_Time_Out event to be posted every t time units
     * starting at time when. Returns a timer id which can be passed
     * to CancelTimeout().
     */

    /*
     * call sr_SetPeriodicTimeout with CALL_CALLBACK.
     */
    return sr_SetPeriodicTimeout(TmQ, when, period, 
                                 info, CALL_CALLBACK);
}

/*
 * SrSetPeriodicTimeout
 *
 * visible to developer. return sr_SetPeriodicTimeout()
 *
 * return value
 *    success : timer_id
 *    fail    : -1
 */
int
SrSetPeriodicTimeout(SR_UINT32 when, 
		     SR_UINT32 period, 
                     SR_INT32  timeout_type, 
                     void *call_back, 
                     SR_INT32  userdata1,
                     void      *userdata2)
{
    FNAME("SrSetPeriodicTimeout")

    TimeOutDescriptor * tod;

    tod = (TimeOutDescriptor *)malloc(sizeof(TimeOutDescriptor));

    if (tod == NULL) {
	DPRINTF((APERROR, "%s: Can not allocate TimeOutDescriptor strcuture\n",
                                                                    Fname));
        return -1;
    }
    memset(tod, '\0', sizeof(TimeOutDescriptor));
    tod->TimeOutType = timeout_type;
    tod->CallBack = (TodCallbackFunction)call_back;
    tod->UserData1 = userdata1;
    tod->UserData2 = userdata2;
    tod->TimerId = sr_SetPeriodicTimeout(GetTimerQueue(), when, 
                                   period, (void *)tod, DONOT_CALL_CALLBACK);
    return tod->TimerId;
}

/*
 * sr_SetOneshotTimeout
 * 
 * this function set one shot timeout by arranging Ev_Time_Out
 * event based on the timeout type.
 *
 * return value
 *    success : timer id
 *    fail    : -1
 */

int
sr_SetOneshotTimeout(TimerQueue *TmQ,
                     SR_UINT32  when,
                     void       *info,
                     SR_INT32   invoke)
{
    FNAME("sr_SetOneshotTimeout")

    /*
     * Arrange for an Ev_Time_Out event to be posted at time when.
     * Returns a timer id which can be passed to CancelTimeout().
     */
    if(TmQ == NULL) {
      DPRINTF((APWARN, "%s: incoming NULL TmQ parameter\n", Fname));
      return -1;
    }
    return sr_SetPeriodicTimeout(TmQ, when, 0, info, invoke);
}

int
TmSetOneshotTimeout(TmQ, when, info)
     TimerQueue *TmQ;
     SR_UINT32       when;
     void           *info;
{
    FNAME("TmSetOneshotTimeout")

    /*
     * Arrange for an Ev_Time_Out event to be posted at time when.
     * Returns a timer id which can be passed to CancelTimeout().
     */
    if(TmQ == NULL) {
      DPRINTF((APWARN, "%s: incoming NULL TmQ parameter\n", Fname));
      return -1;
    }
    return TmSetPeriodicTimeout(TmQ, when, 0, info);
}

/*
 * SrSetOneshotTimeout
 *
 * visible to developer. return sr_SetPeriodicTimeout()
 *
 * return value
 *    success : timer_id
 *    fail    : -1
 */
int
SrSetOneshotTimeout(SR_UINT32 when,
                    SR_INT32  timeout_type, 
                    void      *call_back, 
                    SR_INT32  userdata1,
                    void      *userdata2)
{
    FNAME("SrSetPeriodicTimeout")

    TimeOutDescriptor * tod;

    tod = (TimeOutDescriptor *)malloc(sizeof(TimeOutDescriptor));

    if (tod == NULL) {
	DPRINTF((APERROR, "%s: Can not allocate TimeOutDescriptor strcuture\n",
                                                                    Fname));
        return -1;
    }
    memset(tod, '\0', sizeof(TimeOutDescriptor));
    tod->TimeOutType = timeout_type;
    tod->CallBack = (TodCallbackFunction)call_back;
    tod->UserData1 = userdata1;
    tod->UserData2 = userdata2;
    tod->TimerId = sr_SetPeriodicTimeout(GetTimerQueue(), when, 
                                         0, (void *)tod, DONOT_CALL_CALLBACK);
    return tod->TimerId;
}

int
TmCancelTimeout(TmQ, id)
     TimerQueue *TmQ;
     int             id;
{
    FNAME("CancelTimeout")
    TimerEvent     *p;
    int             retval = -1;
    TimeOutDescriptor *tdp = NULL;

    /*
     *   Return if the timer id specified is invalid.  Don't
     *   return an error condition, the timer doesn't exist.
     */
    if (id <= 0) {
        return 1;
    }

    if(TmQ == NULL) return -1;
    for (p = TmQ->head->next; p != TmQ->tail; p = p->next) {
	if (p->id == id) {
	    break;
	}
    }
    if (p == TmQ->tail) {
	DPRINTF((APTRACE, "%s: unknown timeout: %d\n", Fname, id));
    } else {
        if (p->invoke == DONOT_CALL_CALLBACK) {
            tdp = (TimeOutDescriptor *)p->info;
            if (tdp != NULL) {
                free(tdp);
                tdp = NULL;
                p->info = NULL; 
            }
        }
        free((char *) TimeQDequeue(TmQ, p));
        retval = 1;
    }

    if(CancelTimerHook(TmQ, id, (TmQ->head->next != TmQ->tail)) == 1) {
	retval = 1;
    }

    if (retval == -1) {
	DPRINTF((APTRACE, "%s: timeout not found in evq or TimerQ: %d\n",
                 Fname, id));
    }
    return retval;
}

/*
 * DeferTimeout
 *
 * Reschedule a timeout relative to the current time (absolute=1), or relative
 * to the remaining time until the timeout was to expire (absolute=0).  The
 * timeout is specified by id and the new time is specified by when.
 *
 * Return value is -1 on failure, 0 on success.
 */

int
TmDeferTimeout(TmQ, id, when, absolute)
     TimerQueue *TmQ;
     int             id;
     SR_UINT32       when;
     int             absolute;
{
    FNAME("TmDeferTimeout")
    TimerEvent     *p;
    SR_UINT32      tmp;

    if(TmQ == NULL) return -1;
    /* Find the timeout */
    for (p = TmQ->head->next; p != TmQ->tail; p = p->next) {
	if (p->id == id) {
	    break;
	}
    }
    if (p == TmQ->tail) {
	DPRINTF((APWARN, "%s: unknown timeout: %d\n", Fname, id));
        return -1;
    }

    TimeQDequeue(TmQ, p);

    if (absolute) {
        if (p->when > when) {
            p->type = WRAP;
        }
        p->when = when;
    } else {
        tmp = p->when;
        p->when += when;
        if (p->when < tmp) {
            p->type = WRAP;
        }
    }
    TimeQEnqueue(TmQ, p);
    return 0;
}

/*
 * GetTimeoutInfo
 *
 * Returns information about an existing timer.  The timer is specified by
 * id.  The rest of the arguments are pointers to the caller's storage in
 * which to store the results.  Any of these may be specified as NULL for
 * values that are uninteresting.
 *
 * The return value is 0 on failure, and on success is the remaining amount
 * of time until the timeout should (next) expire.
 */
SR_UINT32
TmGetTimeoutInfo(TmQ, id, when, period, info, fired)
     TimerQueue *TmQ;
     SR_UINT32 id;
     SR_UINT32 *when;
     SR_UINT32 *period;
     void      **info;
     char      *fired;
{
    FNAME("GetTimeoutInfo")
    TimerEvent     *p;

    if(TmQ == NULL) return 0;
    /* Find the timeout */
    for (p = TmQ->head->next; p != TmQ->tail; p = p->next) {
	if (p->id == id) {
	    break;
	}
    }
    if (p == TmQ->tail) {
	DPRINTF((APWARN, "%s: unknown timeout: %d\n", Fname, id));
        return 0;
    }

    if (when != NULL) {
        *when = p->when;
    }
    if (period != NULL) {
        *period = p->period;
    }
    if (info != NULL) {
        *info = p->info;
    }
    if (fired != NULL) {
        *fired = p->fired;
    }
    return GetTimeNow() - p->when;
}

/*
 * AdjustTimeouts
 *
 * This function will adjust the 'when' field of all existing timeouts.
 * This must be done when an EMANATE subagent connects to the master agent
 * and synchronizes it's time with the master agent.
 */
void
TmAdjustTimeouts(TmQ, delta)
     TimerQueue *TmQ;
     SR_INT32 delta;
{
    TimerEvent     *p;
    SR_UINT32      when;
    
    if(TmQ == NULL) return;
    for (p = TmQ->head->next; p != TmQ->tail; p = p->next) {
        when = p->when;
        p->when += delta;
        if (delta < 0) {
            /* check value underflow */
            if (when < (SR_UINT32)abs(delta)){
                p->when = 0;
            }
        } else {
            /* check value overflow(wrap) */
            if (p->when < when) {  
                p->type = WRAP;
            }
        }
    }
}

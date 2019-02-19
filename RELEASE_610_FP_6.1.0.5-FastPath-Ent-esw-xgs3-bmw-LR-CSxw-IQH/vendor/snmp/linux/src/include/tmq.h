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

#ifndef SR_TMQ_H
#define SR_TMQ_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _TimerEvent {
    SR_UINT32       when;	/* value of GetTimeNow() when timer fires */
    SR_UINT32       period;	/* period of timer, 0 -> oneshot, centi-secs */
    SR_UINT32       id;
    void           *info;
    int             type;       /* wrapped or current timer value */
    SR_INT32        invoke;     /* who is going to invoke a call_back */
    struct _TimerEvent *prev;
    struct _TimerEvent *next;
    char            fired;
}               TimerEvent;

typedef struct _TimeOutDescriptor TimeOutDescriptor;

typedef void (*TodCallbackFunction) (TimeOutDescriptor *);

/*
 * new timer call_back function prototype
 * (*SrTodCallbackFunction)(TimeOutType, TimerId, UserData1, UserData2)
 */
typedef void (*SrTodCallbackFunction) (SR_UINT32, int, SR_INT32, void *);

struct _TimeOutDescriptor {
    SR_INT32             TimeOutType;
    SR_INT32             TimerId;
    TodCallbackFunction  CallBack;
    SR_INT32             UserData1;
    void                *UserData2;
};

#define Info_Pdu_Timer ((void *)-1)

typedef struct TimerQueue_s {
    TimerEvent *head;
    TimerEvent *tail;
    SR_UINT32 start; /* starting point to search for unused timer id */
    TimerEvent *tm;  /* pre-allocated timer event for CanSetTimeout() */
    void *apinfo;    /* application-specific info, eg EvQ in master/subagent */
} TimerQueue;

TimerQueue *InitTimerQueue(void *apinfo);

SR_UINT32 TvpToTimeOfDay
    SR_PROTOTYPE((const struct timeval * tp));

int TimeQEmpty(const TimerQueue *TmQ);

int TimersInQ(const TimerQueue *TmQ);

void TimeQInsertBefore(
    TimerQueue *TmQ,
    TimerEvent * newtm,
    TimerEvent * beforetm);

void TimeQEnqueue(TimerQueue *TmQ, TimerEvent * tm);

TimerEvent *TimeQDequeue(TimerQueue *TmQ, TimerEvent * tm);

TimerEvent *TimeQFirst(TimerQueue *TmQ);

int CheckTimers(TimerQueue *TmQ);

int CanSetTimeout(void);

void DontNeedTimeout(void);

int SetPeriodicTimeout(SR_UINT32 when, SR_UINT32 period, void *info);

int SetOneshotTimeout(SR_UINT32 when, void *info);

int SrSetPeriodicTimeout(SR_UINT32 when, SR_UINT32 period, 
                         SR_INT32 timeout_type, void *call_back,
                         SR_INT32 userdata1, void *userdata2);

int SrSetOneshotTimeout(SR_UINT32 when, SR_INT32 timeout_type, 
                        void *call_back, SR_INT32  userdata1,
                        void *userdata2);

int sr_SetPeriodicTimeout(TimerQueue *TmQ, SR_UINT32 when, 
                          SR_UINT32 period, void *info, SR_INT32 invoke);

int sr_SetOneshotTimeout(TimerQueue *TmQ, SR_UINT32 when, 
                         void *info, SR_INT32 invoke);

int CancelTimeout(int id);

int DeferTimeout(int id, SR_UINT32 when, int absolute);

SR_UINT32 GetTimeoutInfo(
    SR_UINT32 id,
    SR_UINT32 *when,
    SR_UINT32 *period,
    void **info,
    char *fired);

void AdjustTimeouts(SR_INT32 delta);

int PostTimerHook(TimerQueue *TmQ, SR_UINT32 id, void *info, SR_INT32 invoke);

int CancelTimerHook(TimerQueue *TmQ, SR_UINT32 id, int anymore);

void DestroyTimerQueue(TimerQueue *TmQ);

TimerQueue *GetTimerQueue(void);

int TmCanSetTimeout(TimerQueue *TmQ);

void TmDontNeedTimeout(TimerQueue *TmQ);

int TmSetPeriodicTimeout(
    TimerQueue *TmQ,
    SR_UINT32 when,
    SR_UINT32 period,
    void *info);

int TmSetOneshotTimeout(TimerQueue *TmQ, SR_UINT32 when, void *info);

int TmCancelTimeout(TimerQueue *TmQ, int id);

int TmDeferTimeout(TimerQueue *TmQ, int id, SR_UINT32 when, int absolute);

SR_UINT32 TmGetTimeoutInfo(
    TimerQueue *TmQ,
    SR_UINT32 id,
    SR_UINT32 *when,
    SR_UINT32 *period,
    void      **info,
    char      *fired);

void TmAdjustTimeouts(TimerQueue *TmQ, SR_INT32 delta);

SR_UINT32 TimerCurTime(void);

#define CURRENT 0
#ifndef WRAP
#define WRAP    1
#endif /* WRAP */
#define DONOT_CALL_CALLBACK  0
#define CALL_CALLBACK        1

#ifdef  __cplusplus
}
#endif

#endif				/* SR_TMQ_H */

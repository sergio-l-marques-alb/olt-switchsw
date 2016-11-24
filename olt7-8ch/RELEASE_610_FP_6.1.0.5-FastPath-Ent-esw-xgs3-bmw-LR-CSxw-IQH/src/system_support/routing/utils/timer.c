/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename    timer.c
 *
 * @purpose     This file implements the timer object,
 *              and the basic clock object driven by an external
 *              external clock.
 *
 * @component    Routing Utils Component
 *
 * @comments
 *       External routines of the timer object:
 * TIMER_InitAll
 * TIMER_KillAll
 * TIMER_SysTime
 * TIMER_Init
 * TIMER_InitSec
 * TIMER_InitMilli
 * TIMER_Delete
 * TIMER_Start
 * TIMER_StartSec
 * TIMER_StartMilli
 * TIMER_Stop
 * TIMER_StartIfStopped
 * TIMER_StartIfStoppedSec
 * TIMER_StartIfStoppedMilli
 * TIMER_ToExpire
 * TIMER_ToExpireSec
 * TIMER_ToExpireMilli
 * TIMER_IsValid
 * TIMER_Active
 * TIMER_Tick
 * TIMER_FreezeAll
 * TIMER_ThawAll
 *
 * External routines of the basic clock object:
 * BCLOCK_SysTime
 * BCLOCK_Init
 * BCLOCK_StartIfStoppedMilli
 * BCLOCK_Tick
 *
 * @create            01/06/1995
 *
 * @author           Jonathan Masel
 *                   Alex Osinsky
 *
 * @end
 *
 * ********************************************************************/

#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/timer.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif



/* --- include files --- */

#include <string.h>
#include "std.h"

/* --- external prototypes --- */
#include "os_xxcmn.h"
#include "xx.ext"
#include "timer.ext"
#include "log.h"


/* --- internal typedefs and definitions --- */

static  t_Time   _SystemTime;
static  byte     _tick1, _tick10;
static  t_Handle TimerTask;
static  ulng     SeqNmb;
static  Bool     TimersFrozen;

#ifdef ERRCHK
t_TMRList *TIMER_List;
#define ADD_ToList(t) \
{ \
    t_TMRList *data = XX_Malloc(sizeof(t_TMRList)); \
    ASSERT(data); \
    data->timer = t; \
    data->prev = data->next = NULLP;  \
    XX_AddToDLList(data, TIMER_List); \
}
#define DEL_FromList(t) \
{ \
   t_TMRList *data; \
   for(data = TIMER_List; data != NULLP; data = data->next) \
   {\
      if(data->timer == t) \
         break;    \
   }\
   ASSERT(data); \
   XX_DelFromDLList(data, TIMER_List); \
   XX_Free(data); \
}
#else
#define ADD_ToList(t)
#define DEL_FromList(t)
#endif


typedef struct tagt_TimeTblDesc
{
   t_Timer        **pTimeTbl;
   word           size;
   word           index;
} t_TimeTblDesc;


#define SIZE1   600 /* Timer entry table size for resolution = 1   */
#define SIZE10  60  /* Timer entry table size for resolution = 10  */
#define SIZE100 6   /* Timer entry table size for resolution = 100 */

static t_Timer *TimeT1[SIZE1];     /* Timer entry table for resolution = 1   */
static t_Timer *TimeT10[SIZE10];   /* Timer entry table for resolution = 10  */
static t_Timer *TimeT100[SIZE100]; /* Timer entry table for resolution = 100 */

/* timer table descriptors */
static t_TimeTblDesc TimeTblDesc1   = { TimeT1, SIZE1, 0 };
static t_TimeTblDesc TimeTblDesc10  = { TimeT10, SIZE10, 0 };
static t_TimeTblDesc TimeTblDesc100 = { TimeT100, SIZE100, 0 };

#if L7_BCLOCK

#define BCTSIZE   600               /* Bclock timer entry table size    */
static t_Time     _BCSystemTime;    /* System time of the basic clock   */
static t_Timer *BCTimeTbl[BCTSIZE]; /* Bclock timer table               */
static t_TimeTblDesc BCTimeTblDesc  = { BCTimeTbl, BCTSIZE, 0 };

#endif /* #if L7_BCLOCK */


#define IS_CONNECTED(p_T) (p_T->entry != NULLP)

static void  _checkTimers( t_TimeTblDesc *pTblDesc );
static void  _connect (t_Timer *p_T);
static void  _disconnect(t_Timer *p_T);
static e_Err _unpackFun(void *callInfo);
static e_Err f_NilExpired(t_Handle owner, t_Handle timer, word flag);



/* declare the timer thread IDs as global, since now needed in os_xxvxw.c */
t_Handle TIMER_Thread;



/*********************************************************************
 * @purpose    Global initialization for system timer object.
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void TIMER_InitAll( void )
{
   memset(&TimeT1,0, SIZE1 * sizeof(t_Timer *));
   memset(&TimeT10,0, SIZE10 * sizeof(t_Timer *));
   memset(&TimeT100,0, SIZE100 * sizeof(t_Timer *));

#if L7_BCLOCK
   memset(&BCTimeTbl, 0, BCTSIZE * sizeof(t_Timer *));
   _BCSystemTime = (t_Time)0;
#endif /* #if L7_BCLOCK */

   if(XX_CreateTmrTsk(OS_TICKS, TIMER_Tick, &TimerTask) != E_OK)
      ASSERT(FALSE);

   _SystemTime = (t_Time)0;
   TimersFrozen = FALSE;
   if(XX_CreateThread(DEFAULT_TIMER_THREAD_PRIORITY, _unpackFun, FALSE, 
                      &TIMER_Thread, "Routing Timer") != E_OK)
       ASSERT(FALSE);
}


/*********************************************************************
 * @purpose     Global deinitialization for system timer object.
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void TIMER_KillAll(void)
{
   ASSERT(TimerTask != NULLP);
   XX_KillTmrTsk(TimerTask);
   TimerTask = NULLP;
   ASSERT(TIMER_Thread != NULLP);
   if(XX_KillThread(TIMER_Thread) != E_OK)
       ASSERT(FALSE);
   TIMER_Thread = NULLP;
}

/*********************************************************************
 * @purpose     Get the system time (ticks since start-up).
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     System time (32-bits unsigned integer
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng TIMER_SysTime( void )
{
   ASSERT(TimerTask != NULLP);

   return _SystemTime;
}

/*********************************************************************
 * @purpose            Allocate a new timer entry for the system ticks use.
 *
 *
 * @param Resolution   @b{(input)}  1, 10 or 100 ticks
 *                                  (for any other values, 1 is assumed)
 * @param Owner        @b{(input)}  owner of timer (handle)
 * @param p_Handle     @b{(output)}  handle to new segment is returned through here
 *
 * @returns            E_OK            success
 * @returns            E_NOMEMORY      out of memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_Init( word Resolution, t_Handle Owner, t_Handle *p_Handle )
{
   t_Timer       *p_T;

   ASSERT(TimerTask != NULLP);

   if((p_T = (t_Timer *)XX_Malloc(sizeof(t_Timer))) == NULLP)
      return E_NOMEMORY;

   memset(p_T,0, sizeof(t_Timer));

   /* record information about timer in the table */
   p_T->bitFlag = TIMER_TAKEN;
   p_T->status = TM_STAT_Valid;
   p_T->owner = Owner;
   XX_Freeze();
   p_T->seqNmb = SeqNmb++;
   XX_Thaw();
   p_T->f_Expired = f_NilExpired;
   p_T->resolution = Resolution<10? 1 : Resolution<100? 10: 100;

   p_T->tblDesc = (void*)(p_T->resolution == 1  ? &TimeTblDesc1  :
                     p_T->resolution == 10 ? &TimeTblDesc10 : &TimeTblDesc100);

   /* return handle to this timer back to the calling routine */
   *p_Handle = (t_Handle)p_T;
   ADD_ToList(p_T);
   return E_OK;
}

/*********************************************************************
 * @purpose          Allocate a new timer entry for the second ticks use.
 *
 *
 * @param minTime    @b{(input)}  munimum value of a time in second for
 *                                resolution decision
 *
 * @param Owner      @b{(input)}  owner of timer (handle)
 * @param p_Handle   @b{(output)}  handle to new segment is returned through
 *                                here
 *
 * @returns          E_OK          success
 * @returns          E_FAIL          can't lock scheduling
 * @returns          E_NOMEMORY      out of memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_InitSec( word minTime, t_Handle Owner, t_Handle *p_Handle )
{
   word resolution = GET_RESOLUTION(minTime * TM_TICKS_PER_SECOND);
   ASSERT(TimerTask != NULLP);

   if(resolution == 0) resolution = 1;
   return TIMER_Init(resolution, Owner, p_Handle);
}

/*********************************************************************
 * @purpose         Allocate a new timer entry for the millisecond
 *                  ticks use.
 *
 *
 * @param minTime   @b{(input)}  munimum value of a time in millisecond for
 *                                  resolution decision
 * @param Owner     @b{(input)}  owner of timer (handle)
 * @param p_Handle  @b{(output)}  handle to new segment is returned through
 *                               here
 *
 * @returns         E_OK          success
 * @returns         E_FAIL        can't lock scheduling
 * @returns         E_NOMEMORY     out of memory
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_InitMilli( word minTime, t_Handle Owner, t_Handle *p_Handle )
{
   word resolution = GET_RESOLUTION(minTime * TM_TICKS_PER_SECOND/1000);
   ASSERT(TimerTask != NULLP);
   if(resolution == 0) resolution = 1;
   return TIMER_Init(resolution, Owner, p_Handle);
}

/*********************************************************************
 * @purpose           Delete a timer entry
 *
 *
 * @param  Handle     @b{(input)}  handle of timer entry
 *
 * @returns           n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void TIMER_Delete( t_Handle handle )
{
   t_Timer *p_T = (t_Timer *)handle;
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   ASSERT(TimerTask != NULLP);

   XX_Freeze();
   if(p_T->bitFlag & TIMER_BLOCKED)
   {
      p_T->bitFlag |= TIMER_DEL_MARKED;
      XX_Thaw();
      return;
   }
   _disconnect(p_T);
   p_T->status = TM_STAT_Invalid;
   p_T->seqNmb = 0;
   p_T->bitFlag = 0;
   XX_Thaw();
   DEL_FromList(p_T);
   XX_Free(p_T);
}


/*********************************************************************
 * @purpose            Start running a timer. If the timer is already
 *                     running,this will in effect restart the timer.
 *
 *
 * @param  Handle      @b{(input)}  handle of timer
 * @param  Ticks       @b{(input)}  run for this number of ticks
 * @param  Repetitive  @b{(input)}  TRUE/FALSE for repitive timer
 * @param  f_Expired   @b{(input)}  call this routine on expiry
 * @param  QueueId     @b{(input)}  for expiration notices
 *
 * @returns            E_OK            success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_Start( t_Handle Handle, ulng Ticks, Bool Repetitive,
                   F_Expired expFun, t_Handle ThreadId, ulng QueueId)
{
   t_Timer *p_T = (t_Timer *)Handle;
   ASSERT(TimerTask != NULLP);

   /* check that timer is not deleted */
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);

   XX_Freeze();
   /* record information for this entry */
   if( Repetitive )
      p_T->bitFlag |= TIMER_REPETITIVE;
   else
      p_T->bitFlag &= ~TIMER_REPETITIVE;
   p_T->f_Expired = expFun;
   p_T->threadId = ThreadId;
   p_T->queueId = QueueId;
   if(p_T->threadId == (t_Handle)0xffffff)
   {
      ASSERT(TIMER_Thread != NULLP);
      p_T->threadId = TIMER_Thread;
   }
   Ticks /= p_T->resolution;
   p_T->value  = Ticks == 0? 1: Ticks;

   /* disconnect timer entry form the timer table in case it is connected */
   _disconnect(p_T);
   /* connect timer entry to the timer table */
   _connect(p_T);

   /* finally, mark the timer as running */
   p_T->bitFlag |= (TIMER_RUNNING | TIMER_TOUCHED);
   XX_Thaw();

   return E_OK;
}

/*********************************************************************
 * @purpose           Start running a timer. If the timer is already
 *                    running, this will in effect restart the timer.
 *
 *
 * @param  Handle      @b{(input)}  handle of timer
 * @param  secTime     @b{(input)}  run for this number of seconds
 * @param  Repetitive  @b{(input)}  TRUE/FALSE for repitive timer
 * @param  f_Expired   @b{(input)}  call this routine on expiry
 * @param  ThreadId    @b{(input)}  for expiration notices
 *
 * @returns            E_OK            success
 * @returns            E_FAIL          timer is not taken
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartSec(t_Handle Handle, ulng secTime, Bool Repetitive,
                F_Expired expFun, t_Handle ThreadId)
{
#ifdef ERRCHK
   t_Timer *p_T = (t_Timer *)Handle;
#endif
   ASSERT(TimerTask != NULLP);

   /* check that timer is not deleted */
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   return TIMER_Start(Handle, (ulng)(secTime * TM_TICKS_PER_SECOND),
         Repetitive, expFun, ThreadId, 0);
}


/*********************************************************************
 * @purpose           Start running a timer. If the timer is already
 *                    running, this will in effect restart the timer.
 *
 *
 * @param  Handle      @b{(input)}  handle of timer
 * @param  secTime     @b{(input)}  run for this number of seconds
 * @param  Repetitive  @b{(input)}  TRUE/FALSE for repitive timer
 * @param  f_Expired   @b{(input)}  call this routine on expiry
 * @param  ThreadId    @b{(input)}  for expiration notices
 * @param  QueueId     @b{(input)}  for expiration notices
 *
 * @returns            E_OK            success
 * @returns            E_FAIL          timer is not taken
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartSecMQueue(t_Handle Handle, ulng secTime, Bool Repetitive,
                F_Expired expFun, t_Handle ThreadId, ulng QueueId)
{
#ifdef ERRCHK
   t_Timer *p_T = (t_Timer *)Handle;
#endif
   ASSERT(TimerTask != NULLP);

   /* check that timer is not deleted */
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   return TIMER_Start(Handle, (ulng)(secTime * TM_TICKS_PER_SECOND),
         Repetitive, expFun, ThreadId, QueueId);
}


/*********************************************************************
 * @purpose           Start running a timer. If the timer is already
 *                    running,this will in effect restart the timer.
 *
 *
 * @param Handle      @b{(input)}  handle of timer
 * @param milliTime   @b{(input)}  run for this number of milliseconds
 * @param Repetitive  @b{(input)}  TRUE/FALSE for repitive timer
 * @param f_Expired   @b{(input)}  call this routine on expiry
 * @param QueueId     @b{(input)}  for expiration notices
 *
 * @returns           E_OK            success
 * @returns           E_FAIL          timer is not taken
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartMilli(t_Handle Handle, ulng milliTime, Bool Repetitive,
                F_Expired expFun, t_Handle ThreadId)
{
#ifdef ERRCHK
   t_Timer *p_T = (t_Timer *)Handle;
#endif
   /* check that timer is not deleted */
   ASSERT(TimerTask != NULLP);
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   return TIMER_Start(Handle, (ulng)(milliTime * TM_TICKS_PER_SECOND / 1000),
         Repetitive, expFun, ThreadId, 0);
}


/*********************************************************************
 * @purpose           Start running a timer. If the timer is already
 *                    running,this will in effect restart the timer.
 *
 *
 * @param Handle      @b{(input)}  handle of timer
 * @param milliTime   @b{(input)}  run for this number of milliseconds
 * @param Repetitive  @b{(input)}  TRUE/FALSE for repitive timer
 * @param f_Expired   @b{(input)}  call this routine on expiry
 * @param QueueId     @b{(input)}  for expiration notices
 *
 * @returns           E_OK            success
 * @returns           E_FAIL          timer is not taken
 *
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartMilliMQueue(t_Handle Handle, ulng milliTime, Bool Repetitive,
                F_Expired expFun, t_Handle ThreadId, ulng QueueId)
{
#ifdef ERRCHK
   t_Timer *p_T = (t_Timer *)Handle;
#endif
   /* check that timer is not deleted */
   ASSERT(TimerTask != NULLP);
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   return TIMER_Start(Handle, (ulng)(milliTime * TM_TICKS_PER_SECOND / 1000),
         Repetitive, expFun, ThreadId, QueueId);
}


/*********************************************************************
 * @purpose           Stop a timer
 *
 *
 * @param Handle      @b{(input)} handle of timer entry
 *
 * @returns           E_OK        cannot fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_Stop( t_Handle Handle )
{
   t_Timer *p_T = (t_Timer *)Handle;

   /* check that the timer is not deleted */
   ASSERT(TimerTask != NULLP);
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);

   XX_Freeze();
   p_T->bitFlag &= ~TIMER_RUNNING;
   p_T->bitFlag |= TIMER_TOUCHED;
   _disconnect(p_T);
   XX_Thaw();
   return E_OK;
}

/*********************************************************************
 * @purpose           Start a timer only if it is stopped.
 *
 *
 * @param  Handle     @b{(input)}  handle of timer
 * @param  Ticks      @b{(input)}  run for this number of ticks
 * @param  f_Expired  @b{(input)}  call this routine on expiry
 *
 * @returns           E_OK            cannot fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartIfStopped( t_Handle Handle, word Ticks, F_Expired f_Expired, t_Handle ThreadId )
{
   ASSERT(TimerTask != NULLP);
    if( ((t_Timer *)Handle)->entry == NULLP )
      return TIMER_Start(Handle, Ticks, FALSE, f_Expired, ThreadId, 0);

    return E_OK;
}


/*********************************************************************
 * @purpose            Start a timer only if it is stopped.
 *
 *
 * @param  Handle      @b{(input)}  handle of timer
 * @param  secTime     @b{(input)}  run for this number of seconds
 * @param  f_Expired   @b{(input)}  call this routine on expiry
 *
 * @returns            E_OK            cannot fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartIfStoppedSec( t_Handle Handle, ulng secTime,
                  F_Expired expFun, t_Handle QueueId )
{
   ASSERT(TimerTask != NULLP);
    if( ((t_Timer *)Handle)->entry == NULLP )
      return TIMER_StartSec(Handle, secTime, FALSE, expFun, QueueId);

    return E_OK;
}


/*********************************************************************
 * @purpose          Start a timer only if it is stopped.
 *
 *
 * @param Handle      @b{(input)}  handle of timer
 * @param milliTime   @b{(input)}  run for this number of milliseconds
 * @param f_Expired   @b{(input)}  call this routine on expiry
 *
 * @returns           E_OK            cannot fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_StartIfStoppedMilli( t_Handle Handle, ulng milliTime,
                  F_Expired expFun, t_Handle QueueId )
{
   ASSERT(TimerTask != NULLP);
   if( ((t_Timer *)Handle)->entry == NULLP )
      return TIMER_StartMilli(Handle, milliTime, FALSE, expFun, QueueId);

   return E_OK;
}


/*********************************************************************
 * @purpose           Returns number of tics to expire.
 *
 *
 * @param Handle      @b{(input)}  handle of timer
 *
 * @returns           number of tics
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng  TIMER_ToExpire(t_Handle Handle)
{
   t_Timer *p_T = (t_Timer *)Handle;
   t_Timer **table;
   word  tableSize;
   word  index;
   word  steps;
   ulng  exp;
   ASSERT(TimerTask != NULLP);

   /* check that timer is valid */
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);

   XX_Freeze();
   /* if timer is expired allready return 0 */
   if(!IS_CONNECTED(p_T))
   {
      XX_Thaw();
      return 0;
   }

   /* Set table and index according to the resolution */
   table     = ((t_TimeTblDesc*)(p_T->tblDesc))->pTimeTbl;
   index     = ((t_TimeTblDesc*)(p_T->tblDesc))->index;
   tableSize = ((t_TimeTblDesc*)(p_T->tblDesc))->size;

   if(&table[index] <= p_T->entry)
     steps = (p_T->entry - &table[index]);
   else
     steps = tableSize - index + (p_T->entry - &table[0]);

   exp = (word)(steps * p_T->resolution +
         (p_T->nturns == 0? 0: p_T->nturns - 1)
         * tableSize * p_T->resolution);

   XX_Thaw();
   return exp;

}


/*********************************************************************
 * @purpose           Returns number of seconds to expire.
 *
 *
 * @param  Handle     @b{(input)}  handle of timer
 *
 * @returns           number of seconds
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng  TIMER_ToExpireSec(t_Handle Handle)
{
#ifdef ERRCHK
   t_Timer *p_T = (t_Timer *)Handle;
#endif
   ASSERT(TimerTask != NULLP);

   /* check that timer is valid */
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   return TIMER_ToExpire(Handle) / TM_TICKS_PER_SECOND;
}


/*********************************************************************
 * @purpose           Returns number of miliseconds to expire.
 *
 *
 * @param  Handle     @b{(input)}  handle of timer
 *
 * @returns           number of miliseconds
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng  TIMER_ToExpireMilli(t_Handle Handle)
{
#ifdef ERRCHK
   t_Timer *p_T = (t_Timer *)Handle;
#endif
   ASSERT(TimerTask != NULLP);

   /* check that timer is valid */
   ASSERT(p_T != NULLP);
   ASSERT(p_T->status == TM_STAT_Valid);
   ASSERT(p_T->bitFlag & TIMER_TAKEN);
   return TIMER_ToExpire(Handle) * 1000 / TM_TICKS_PER_SECOND;
}


/*********************************************************************
 * @purpose           This routine is used to check that an expiration
 *                    of a timer was a valid event.
 *
 *
 * @param Handle      @b{(input)}  handle of timer
 *
 * @returns           TRUE        timer expiration notification was valid
 * @returns           FALSE       expiration is outdated: ignore
 *
 * @notes
 *            This is provided to solve a problem that can arise in
 * multi-tasking systems: a timer may expire and a message be sent
 * to a task's message queue. If the task restarts the timer before
 * servicing the expiration message in its queue, it will believe the
 * expiration message to belong to the new request and not the timer
 * that originally expired. This routine is called on timer expiration
 * to check that an expiration request is not "outdated" in this sense.
 * It checks whether or not the timer has been touched since expiration.
 *
 *
 *
 * @end
 * ********************************************************************/
Bool TIMER_IsValid( t_Handle Handle )
{
   Bool valid;
   t_Timer *p_T = (t_Timer *)Handle;
   ASSERT(TimerTask != NULLP);
   XX_Freeze();
   valid = (Bool)((p_T->status == TM_STAT_Valid) && !(p_T->bitFlag & TIMER_TOUCHED));
   XX_Thaw();
   return valid;
}


/*********************************************************************
 * @purpose     Returns TRUE if timer is runing.
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     TRUE
 * @returns     FALSE
 *
 * @notes
 *
 * @end
 * ********************************************************************/
Bool TIMER_Active(t_Handle Handle)
{
   Bool active;
   t_Timer *p_T = (t_Timer *)Handle;
   ASSERT(TimerTask != NULLP);
   XX_Freeze();
   active = (Bool)((p_T->status == TM_STAT_Valid) && (p_T->bitFlag & TIMER_RUNNING));
   XX_Thaw();
   return active;
}

/*********************************************************************
 * @purpose     Informs the timer object that a timer tick has occured
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err TIMER_Tick( void )
{
   /* The timer tick may get dispatched before the TimerTask is
   ** initialized. This is particularly likely to happen when debugging
   ** with GDB.
   */
   if (TimerTask == NULLP)
   {
     return E_OK;
   }

   if (TimersFrozen)
      return E_OK;

   _SystemTime++;
   _tick1++;

   /* check single-tick timers */
   _checkTimers( &TimeTblDesc1 );

   /* check 10-tick timers */
   if( _tick1 >= 10 )
   {
      _tick1 = 0;
      _tick10++;
      _checkTimers( &TimeTblDesc10 );

      /* now check 100-tick timers */
      if( _tick10 >= 10 )
      {
         _tick10 = 0;
         _checkTimers( &TimeTblDesc100 );
      }
   }
   return E_OK;
}

void TIMER_FreezeAll()
{
   ASSERT(TimerTask != NULLP);

   TimersFrozen = TRUE;
}

void TIMER_ThawAll()
{
   ASSERT(TimerTask != NULLP);

   TimersFrozen = FALSE;
}

Bool TIMER_AreTimersFrozen()
{
   return TimersFrozen;
}


/*********************************************************************
 * @purpose           Checks for expiration for all timers in the given
 *                    table for given index.
 *
 *
 * @param  pTblDesc   @b{(input)}  pointer to a timer table descriptor
 *
 * @returns           n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void _checkTimers( t_TimeTblDesc *pTblDesc )
{
   t_Timer *p_T;
   t_Timer **p_table = pTblDesc->pTimeTbl;
   word     *p_index = &(pTblDesc->index);
   word      size    = pTblDesc->size;


   if(p_table[*p_index] == NULLP)
   {
      /* nothing to do */
      if (++(*p_index) == size)
         *p_index = 0;
      return;
   }
   /* to be protected from the expiration functions action this loop
   goes every time from the begin of the table */
   XX_Freeze();
   p_T = p_table[*p_index];
   while(p_T != NULLP)
   {
      if(p_T->nturns > 1)
      {
         p_T = p_T->next;
         continue;
      }

      _disconnect(p_T); /* set p_table[] to the next entry */
      p_T->bitFlag &= ~TIMER_TOUCHED;
      if(p_T->bitFlag & TIMER_REPETITIVE)
         _connect(p_T);
      else
         p_T->bitFlag &= ~TIMER_RUNNING;

      p_T->bitFlag |= TIMER_BLOCKED;

      if(p_T->threadId == NULLP)
      {
         XX_Thaw();
         if(!(p_T->bitFlag & TIMER_DEL_MARKED))
         {
            /* Call Expiration function if entry is not marked for deletion */
            p_T->f_Expired(p_T->owner, (t_Handle) p_T, 0);
         }

         XX_Freeze();
         p_T->bitFlag &=(~TIMER_BLOCKED);
         if(p_T->bitFlag & TIMER_DEL_MARKED)
         {
            p_T->bitFlag &= (~TIMER_DEL_MARKED);
            XX_Thaw();
            TIMER_Delete(p_T);
            XX_Freeze();
         }
      }
      else
      {
         t_XXCallInfo   *callInfo;
         ulng           seq      = p_T->seqNmb;
         OS_Thread      *thread   = (OS_Thread*) p_T->threadId;

         XX_Thaw();

         PACKET_INIT_MQUEUE(callInfo, _unpackFun, 0, 0, p_T->queueId, 2, p_T);
         if(callInfo)
         {
            PACKET_PUT(callInfo, seq);
            (void) XX_Call(thread, callInfo);  /* May fail if queue full. OK for some queues. */
         }
         else
         {
           L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_LOG_COMPONENT_DEFAULT,
                   "Memory allocation failure in _checkTimers for queue %d thread 0x%x", 
                   p_T->queueId, thread->TaskID);
         }
         XX_Freeze();
      }

      /* set p_T to the begin of the table */
      p_T = p_table[*p_index];
   }
   /* modify nturns */
   for(p_T = p_table[*p_index]; p_T != NULLP; p_T = p_T->next)
      p_T->nturns--;
   XX_Thaw();

   if (++(*p_index) == size)
      *p_index = 0;
}

/*********************************************************************
 * @purpose        connects timer object to the timer table.
 *                 this routine always runs with closed interrupts
 *
 *
 * @param  p_T     @b{(input)}  timer pointer
 *
 * @returns        n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void _connect (t_Timer *p_T)
{
   t_Timer **table;
   word  tableSize;
   word  index;
   word  offset;

   /* Set table and index according to the resolution */
   table = ((t_TimeTblDesc*)(p_T->tblDesc))->pTimeTbl;
   index = ((t_TimeTblDesc*)(p_T->tblDesc))->index;
   tableSize = ((t_TimeTblDesc*)(p_T->tblDesc))->size;

   offset = (word)((index + p_T->value) % tableSize);

   if(table[offset] == NULLP)
      table[offset] = p_T;
   else
   {
      p_T->next = table[offset];
      p_T->next->prev = p_T;
      table[offset] = p_T;
   }

   p_T->entry = table + offset;
   p_T->nturns = p_T->value / tableSize + 1;
}


/*********************************************************************
 * @purpose        disconnects timer object from  the timer table
 *                 this routine always runs with closed interrupts
 *
 *
 * @param  p_T     @b{(input)}  timer pointer
 *
 * @returns        n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static void _disconnect(t_Timer *p_T)
{
   if(p_T->entry == NULLP)
      return;
   if(p_T->prev != NULLP)
      p_T->prev->next = p_T->next;
   else
      *p_T->entry = p_T->next;
   if(p_T->next != NULLP)
      p_T->next->prev = p_T->prev;
   p_T->entry = NULLP;
   p_T->next = p_T->prev = NULLP;
}



/*********************************************************************
 * @purpose         unpack parameters and call Expired function
 *
 *
 * @param  data     @b{(input)}  call info structure pointer
 *
 * @returns         n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err _unpackFun(void *data)
{
   t_XXCallInfo   *callInfo = (t_XXCallInfo *)data;
   t_Timer        *p_T      = (t_Timer *)PACKET_GET(callInfo,0);
   ulng            seqNmb = PACKET_GET(callInfo, 1);
   F_Expired       f_Expired;
   t_Handle        owner;
   e_Err           e = E_OK;

   /* Check if timer is deleted until XX_Call got control */
   f_Expired = p_T->f_Expired;
   owner = p_T->owner;
   if(p_T->status != TM_STAT_Valid ||
       p_T->seqNmb != seqNmb)
       return E_OK;

   /* Check if timer is stoped or restarted until XX_Call got control */
   if((p_T->bitFlag & TIMER_TOUCHED) != 0)
       return E_OK;

   if(!(p_T->bitFlag & TIMER_DEL_MARKED))
   {
      /* Call Expiration function if entry is not marked for deletion */
      e = f_Expired(owner, (t_Handle)p_T, 0);
   }

   XX_Freeze();
   p_T->bitFlag &=(~TIMER_BLOCKED);
   if(p_T->bitFlag & TIMER_DEL_MARKED)
   {
      p_T->bitFlag &= (~TIMER_DEL_MARKED);
      XX_Thaw();
      TIMER_Delete(p_T);
   }
   else
   {
      XX_Thaw();
   }
   return e;
}

static e_Err f_NilExpired(t_Handle owner, t_Handle timer, word flag)
{
   UNUSED( owner );
   UNUSED( timer );
   UNUSED( flag );
   return E_OK;
}


#if L7_BCLOCK


/*********************************************************************
 * @purpose     Get the system time of the basic clock (ticks since
 *              start-up).
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     System time (32-bits unsigned integer
 *
 * @notes
 *
 * @end
 * ********************************************************************/
ulng BCLOCK_SysTime( void )
{
   return _BCSystemTime;
}


/*********************************************************************
 * @purpose     Allocate a new basic clock timer.
 *
 *
 * @param  Owner     @b{(input)}  owner of timer (handle)
 * @param  p_Handle  @b{(input)}  handle to new segment is returned through here
 *
 * @returns          E_OK            success
 * @returns          E_NOMEMORY      out of memory
 *
 * @notes
 *              A timer connected to the system clock is allocated first.
 *              The table descriptor field of the timer structure is
 *              changed later to cause the timer be connected to the
 *              basic clock table.
 *
 *
 * @end
 * ********************************************************************/
e_Err BCLOCK_Init( t_Handle Owner, t_Handle *p_Handle )
{
   e_Err rc;
   t_Timer *p_T;
   if((rc = TIMER_Init( 1, Owner, p_Handle )) == E_OK)
   {
      p_T = (t_Timer *)(*p_Handle);
      p_T->tblDesc = (void *)&BCTimeTblDesc;
   }
   return rc;
}


/*********************************************************************
 * @purpose           Start a basic clock timer only if it is stopped.
 *
 *
 * @param  Handle     @b{(input)}  handle of timer
 * @param  milliTime  @b{(input)}  run for this number of milliseconds
 * @param  f_Expired  @b{(input)}  call this routine on expiry
 *
 * @returns           E_OK               cannot fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BCLOCK_StartIfStoppedMilli( t_Handle Handle,  ulng milliTime,
                                  F_Expired expFun, t_Handle QueueId )
{
   t_Timer *p_T = (t_Timer *)Handle;
   ASSERT(p_T != NULLP);
   if( p_T->entry == NULLP )
      return BCLOCK_StartMilli(Handle, milliTime, FALSE, expFun, QueueId);
   return E_OK;
}

/*********************************************************************
 * @purpose     Informs the bclock object that a bclock timer tick has
 *              occured.
 *
 *
 * @param       @b{(input)}  None
 *
 * @returns     n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void  BCLOCK_Tick( void )
{
   /* update the Basic Clock system time */
   _BCSystemTime++;
   /* check the Basic Clock timers */
   _checkTimers( &BCTimeTblDesc );
}


#endif /* #if L7_BCLOCK */

/* --- end of file timer.c --- */


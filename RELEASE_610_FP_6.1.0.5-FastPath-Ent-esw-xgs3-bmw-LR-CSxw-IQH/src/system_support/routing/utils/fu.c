/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename        fu.c
 *
 * @purpose         Funnel handler.
 *
 * @component       Routing Utils Component
 *
 * @comments
 *                 A funnel is an object that monitors and controls
 *    bandwidth consumption. They are attached to
 *   either the receive or transmit side of a
 *   connection and try to keep the connection's data
 *   throughput rate to within pre-defined limits.
 *
 *   Funnels are implemented as leaky buckets and have
 *   the following attributes:
 *       units  - time units which define the
 *                resolution of data rates
 *                (system parameter: same for all)
 *       window - period of time over which data rates
 *                are measured. The window slides in
 *                increments of "units".
 *       gauge  - outgoing data rate (bytes/window)
 *                (this may be altered dynamically)
 *       depth  - maximum number of frames that may be
 *                stored within the funnel
 *       committed - the committed information rate
 *       excess - the excess (burst) information rate
 *
 *External Routines:
 *   FU_InitAll          initialize all funnels
 *   FU_Init             allocate and innitialize
 *   FU_Clear            discard all frames in funnel
 *   FU_Delete           delete a funnel
 *   FU_SetGauge         set the gauge
 *   FU_CongestionOn     congestion is set
 *   FU_CongestionOff    congestion is clear
 *   FU_Pour             pour frame into funnel
 *   FU_Tick             update bandwidth measurements
 *
 *Internal Routines:
 *   AllocFunnel         allocate memory for funnel
 *   Flush               tx all frames within gauge
 *   Process             process after funnel
 *   StoreInFunnel       store frame in a funnel
 *
 *
 *
 * @create               09/01/1994
 *
 * @author               Jonathan Masel
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/fu.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif


/* --- standard include files --- */

#include <stdio.h>
#include <string.h>

#include "std.h"
#include "local.h"




/* --- specific include files --- */

#include "funnel.h"

/* --- external object interfaces --- */

#include "fu.ext"
#include "timer.ext"
#include "frame.h"
#include "frame.ext"
#include "xx.ext"


/* --- internal (static) data and prototypes --- */

static t_Mutex   FU_Mutex;

#if MAX_PREALLOC_FUNNEL > 0
static t_Funnel _Funnel[MAX_PREALLOC_FUNNEL];
#endif

/* pre-allocated funnel data structures */
static t_Funnel *AllocFunnel( void );
static e_Err StoreInFunnel( t_Funnel *p_Funnel, t_Handle Owner, void *p_Frame,
                              ulng Length, word CongBits );
static void Flush( t_Funnel *p_Funnel );
static e_Err Process( t_Funnel *p_Funnel, t_Handle Owner, void *p_Frame,
                       ulng Length, word CongBits );




/*********************************************************************
 * @purpose     Initialize all funnel structures.
 *              Called during system initialization.
 *
 * @returns     E_OK          success (can't fail)
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_InitAll( void )
{
#if MAX_PREALLOC_FUNNEL > 0
   int i;
#endif

   FU_Mutex = 0;

#if MAX_PREALLOC_FUNNEL > 0
   for(i = 0; i < MAX_PREALLOC_FUNNEL; i++)
      _Funnel[i].Taken = FALSE;
#endif

   return E_OK;
}

/*********************************************************************
 * @purpose     Returns the size of the funnel data structure.
 *              May be used in user-supplied allocation routines if required.
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     size of funnel structure in bytes
 *
 * @notes
 *
 * @end
 * ********************************************************************/
int FU_Sizeof( void )
{
   return sizeof(t_Funnel);
}



/*********************************************************************
 * @purpose              Allocate and initialize a new funnel structure
 *
 *
 * @param  QueueId       @b{(input)}  timer communicates with funnel via
 *                                    this queue
 * @param  *p_f_Handler  @b{(output)} pointer to pointer to owner's handler
 *                                    This is a pointer to the pointer to
 *                                    routine called by the funnel object
 *                                    in order to pass frames to the onwer
 *                                    after they have been poured through
 *                                    the funnel.
 * @param  Committed     @b{(input)}  committed info rate (bytes per measur
 *                                                       ement interval)
 * @param  Excess        @b{(input)}  excess info rate (bytes per measurement
 *                                                       interval)
 * @param  Throughput    @b{(input)}  expected average throughput (bytes/second)
 * @param  Gauge         @b{(input)}  initial gauge value
 * @param  Depth         @b{(input)}  maximum number of bytes to store in funnel
 * @param  BusyBuffs     @b{(input)   set busy when this many buffers in funnel
 * @param  ClearBuffs    @b{(input)   clear busy when this many buffers in funne
 * @param  f_Busy        @b{(input)   owner's busy indication routine
 *
 * @returns              pointer to funnel on success
 * @returns              0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
t_Handle FU_Init( t_Handle QueueId,
               F_Handler *p_f_Handler,
               ulng Committed, ulng Excess, ulng Throughput,
               ulng Gauge, ulng Depth, word BusyBuffs, word ClearBuffs,
               e_Err (* f_Busy)(t_Handle, word) )
{
   t_Funnel *p_Funnel;
   e_Err     e;

   UNUSED( Gauge );

   /* allocate new funnel */
   p_Funnel = AllocFunnel();
   if( !p_Funnel )
      return 0;

   p_Funnel->QueueId = QueueId;
   p_Funnel->p_f_Handler = p_f_Handler;

   e = FU_Config( p_Funnel, Committed, Excess, Throughput,
                  BusyBuffs, ClearBuffs );
   if( e != E_OK )
   {
      FU_Delete( p_Funnel );
      return 0;
   }

   /* set up dept as reqested */
   p_Funnel->MaxDepth = Depth;
   p_Funnel->f_Busy = f_Busy;

   /* return the pointer to the funnel */
   return (t_Handle)p_Funnel;
}



/*********************************************************************
 * @purpose            Re-configure bandwidth parameters for a funnel.
 *
 *
 * @param Funnel      @b{(input)}  handle to funnel
 * @param Committed   @b{(input)}  committed burst rate (bytes per measurement
 *                                                         interval)
 * @param Excess      @b{(input)}  excess burst rate (bytes per measurement
 *                                                          interval)
 * @param Throughput  @b{(input)}  expected average throughput (bytes/second)
 * @param BusyBuffs   @b{(input)}  set busy when this many buffers in funnel
 * @param ClearBuffs  @b{(input)}  clear busy when this many buffers in funnel
 *
 * @returns           E_OK           can't fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_Config( t_Handle Funnel, ulng Committed, ulng Excess,
                 ulng Throughput, word BusyBuffs, word ClearBuffs )
{
   t_Funnel *p_Funnel = (t_Funnel *)Funnel;
   word      Width;        /* sampling window width in seconds */
   word      Resolution;   /* funnel's timer resolution */
   e_Err     e;

   /* work out how many ticks in entire window */
   if( Committed > 0 )
      Width = (word)(SECONDS_TO_TICKS(Committed)/Throughput);
   else
      Width = (word)(SECONDS_TO_TICKS(Excess)/Throughput);

   /* width must be at least MAX_SAMPLING_WINDOW ticks */
   if( Width < MAX_SAMPLING_WINDOW )
   {
      if( Width == 0 )
         Width = 1;
      Committed = (Committed * MAX_SAMPLING_WINDOW)/Width;
      Excess = (Excess * MAX_SAMPLING_WINDOW)/Width;
      Width = MAX_SAMPLING_WINDOW;
   }

   /* work out the optimal resolution for the timer */
   /* start off by calculating the number of ticks in window entry */
   Resolution = Width/MAX_SAMPLING_WINDOW;

   if( Resolution > 1000 )
      Resolution = 100;
   else if( Resolution > 100 )
      Resolution = 10;
   else
      Resolution = 1;

   /* set the timer for this funnel */
   if( p_Funnel->Timer )
   {
      TIMER_Stop( p_Funnel->Timer );
      TIMER_Delete( p_Funnel->Timer );
   }

   e = TIMER_Init( Resolution, (t_Handle)p_Funnel, &p_Funnel->Timer );
   if( e != E_OK )
      return e;

   /* set parameters as required */
   p_Funnel->Committed.Max = Committed;
   p_Funnel->Excess.Max = Excess;
   p_Funnel->CurrentGauge = Excess + Committed;
   p_Funnel->InitialGauge = Excess + Committed;
   p_Funnel->CongestedGauge = Committed;
   p_Funnel->Step = (Excess + Committed)/4;
   p_Funnel->MeasurementInterval = (Width/MAX_SAMPLING_WINDOW);
   p_Funnel->BuffsToBusy = BusyBuffs;
   p_Funnel->BuffsToClear = ClearBuffs;

   /* if there's data buffered internally, (re)start the funnel's timer */
   if( p_Funnel->Depth > 0 )
      TIMER_Start( p_Funnel->Timer, p_Funnel->MeasurementInterval,
      FALSE, FU_Tick, p_Funnel->QueueId, 0);

   return E_OK;
}



/*********************************************************************
 * @purpose           Note that a connction has been linked to the funnel.
 *
 *
 * @param Funnel      @b{(input)}  handle to funnel
 *
 * @returns           E_OK           can't fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_Link( t_Handle Funnel )
{
   t_Funnel *p = (t_Funnel *)Funnel;

   XX_Lock(&FU_Mutex);
   if( p->Taken )
      p->Links++;
   XX_Unlock(&FU_Mutex);
   return E_OK;
}


/*********************************************************************
 * @purpose           Note that a connction has been unlinked from
 *                    the funnel.
 *
 *
 * @param  Funnel     @b{(input)}  handle to funnel
 *
 * @returns           E_OK           can't fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_Unlink( t_Handle Funnel )
{
   t_Funnel *p = (t_Funnel *)Funnel;

   XX_Lock(&FU_Mutex);
   if( p->Links > 0 )
   {
      p->Links--;
      if( p->Links == 0 )
         FU_Delete(p);
   }

   XX_Unlock(&FU_Mutex);

   return E_OK;
}


/*********************************************************************
 * @purpose            Clear the funnel. This discards all frames that
 *                     are within the funnel prior to the funnel being
 *                     deleted.
 *
 *
 * @param  p_Funnel    @b{(input)}  pointer to funnel
 *
 * @returns             n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void FU_Clear( t_Funnel *p_Funnel )
{
   t_FunnelEntry *p_E;

   /*
    * while a. there are more frames buffered and
    *       b. there is bandwidth available
    *    get next frame from buffer and process
    */
   while( p_Funnel->Head != p_Funnel->Tail )
   {
      p_E = &p_Funnel->Entry[p_Funnel->Head];

      /* delete frame */
      F_Delete( p_E->p_Frame );

      /* remove frame from internal buffer */
      p_Funnel->Head++;
      if( p_Funnel->Head > FUNNEL_DEPTH )
         p_Funnel->Head = 0;
   }

   /* returns void */
}

/*********************************************************************
 * @purpose             Delete a funnel, removing all frames stored
 *                      within it.
 *
 *
 * @param  p_Funnel     @b{(input)}  pointer to funnel
 *
 * @returns             n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void FU_Delete( t_Funnel *p_Funnel )
{
   /* stop and delete the timer */
   if( p_Funnel->Timer )
   {
      TIMER_Stop( p_Funnel->Timer );
      TIMER_Delete( p_Funnel->Timer );
   }

   /* discard all frames in funnel */
   FU_Clear( p_Funnel );

   p_Funnel->Taken = FALSE;

   if( p_Funnel->Prealloc == FALSE )
      XX_Free( p_Funnel );
}


/*********************************************************************
 * @purpose        Explicitly set the funnel's gauge.
 *
 *
 * @param  Id      @b{(input)} funnel ID
 * @param  Gauge   @b{(input)} new gauge value
 *
 * @returns        E_OK           success
 * @returns        E_BADPARM      invalid parameter
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_SetGauge( t_Handle Id, ulng Gauge )
{
   t_Funnel *p_Funnel = (t_Funnel *)Id;

   /* set gauge */
   p_Funnel->CurrentGauge = Gauge;

   return E_OK;
}

/*********************************************************************
 * @purpose          Congestion is currently set for this direction.
 *
 *
 *
 * @param p_Funnel   @b{(input)}  pointer to funnel
 *
 * @returns          E_OK           success
 *
 * @notes
 *
 *
 *
 * @end
 * ********************************************************************/
e_Err FU_CongestionOn( t_Funnel *p_Funnel )
{
   p_Funnel->ExtCongestion = TRUE;
    if( p_Funnel->CurrentGauge > p_Funnel->CongestedGauge )
    {

        XX_Event( EV_TIGHTEN_GAUGE, "FUNL" );
        FU_SetGauge( p_Funnel, p_Funnel->CongestedGauge );

        /* make sure the funnel's timer is running */
        TIMER_StartIfStopped( p_Funnel->Timer, p_Funnel->MeasurementInterval,
         FU_Tick, p_Funnel->QueueId );
   }

   return E_OK;
}

/*********************************************************************
 * @purpose             Congestion is cleared for this direction.
 *
 *
 * @param p_Funnel      @b{(input)}  pointer to funnel
 *
 * @returns             E_OK           success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_CongestionOff( t_Funnel *p_Funnel )
{
   if( p_Funnel->Blocked )
     return E_OK;

   p_Funnel->ExtCongestion = FALSE;

   return E_OK;
}

/*********************************************************************
 * @purpose             Block the funnel's transmission rate to its
 *                      congested gauge value.the congestion condition.
 *
 *
 * @param Funnel        @b{(input)}  handle of the funnel
 *
 * @returns             n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void FU_Block( t_Handle Funnel )
{
   t_Funnel *p_Funnel = (t_Funnel *)Funnel;

   p_Funnel->Blocked = TRUE;
   FU_CongestionOn( p_Funnel );
}

void FU_Unblock( t_Handle Funnel )
{
   t_Funnel *p_Funnel = (t_Funnel *)Funnel;

   p_Funnel->Blocked = FALSE;
}


/*********************************************************************
 * @purpose     Pour a frame through the funnel.
 *
 *
 *
 * @param  p_Funnel    @b{(input)}    pointer to funnel
 * @param  Owner       @b{(input)}    the owner of the frame
 * @param  p_Frame     @b{(input)}    pointer to frame
 * @param  CongBits    @b{(input)}    congestion bits (FECN,BECN,DE)
 *                                         in original frame
 *
 * @returns            E_OK           success
 * @returns            E_BADPARM      invalid parameter
 * @returns            E_FAIL         can't lock scheduling
 *
 * @notes
 *                     1. If unsuccessful, an error code is returned but
 *                        the frame is not released. This should be done
 *                        by the calling routine.
 *
 *              On the transmit side,
 *              this is a request for transmission, on the receive side
 *              it is called in order to pass received frames up to the
 *              application at the desired rate.
 *              In either case, if the request can be handled immediately
 *              it is,otherwise it is buffered inside the funnel until it
 *              can be.

 *
 * @end
 * ********************************************************************/
e_Err FU_Pour( t_Funnel *p_Funnel, t_Handle Owner,
               void *p_Frame, word CongBits )
{
   e_Err    e = E_OK;
   ulng     Length = F_GetLength(p_Frame);

   XX_Trace( TRC_FU_POUR, "FUNL" );

   /* disable scheduling (multi-threading) */
   XX_Lock(&p_Funnel->Mutex);

   /*
    * if there is already data buffered in the funnel,
    * or if the gauge does not permit transmission of
    * the frame, store it inside the funnel
    */
   if( (p_Funnel->Frames > 0) ||
       ((p_Funnel->Bandwidth + Length) > p_Funnel->CurrentGauge) )
   {
      /* store this frame in funnel */
      e = StoreInFunnel( p_Funnel, Owner, p_Frame, Length, CongBits );
   } else
   {
      /* all clear - process the frame */
      e = Process( p_Funnel, Owner, p_Frame, Length, CongBits );
   }

   /* re-enable scheduling */
   XX_Unlock(&p_Funnel->Mutex);
   return e;
}


/*********************************************************************
 * @purpose         A tick has occured: update sliding window and the
 *                  bandwidth calculations for this funnel.
 *
 *
 * @param Funnel    @b{(input)}  handle to funnel
 * @param h         @b{(input)}  not used
 * @param n         @b{(input)}  not used
 *
 * @returns         E_OK        can't fail
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err FU_Tick( t_Handle Funnel, t_Handle h, word n )
{
   t_Funnel    *p_Funnel = (t_Funnel *)Funnel;
   t_InfoRate  *p_I;
   ulng        up_gauge;
   ulng        full_gauge;

   UNUSED( h );
   UNUSED( n );

   /* committed info rate usage */
   p_I = &p_Funnel->Committed;

   /* advance time index */
   p_I->Now++;
   if( p_I->Now >= MAX_SAMPLING_WINDOW )
      p_I->Now = 0;

   /* subtract oldest bandwidth used */
   p_I->Curr -= p_I->Window[p_I->Now];
   if( p_I->Window[p_I->Now] > p_Funnel->Bandwidth )
      p_Funnel->Bandwidth = 0L;
   else
      p_Funnel->Bandwidth -= p_I->Window[p_I->Now];
   p_I->Window[p_I->Now] = 0L;

   /* excess info rate usage */
   p_I = &p_Funnel->Excess;

   /* advance time index */
   p_I->Now++;
   if( p_I->Now >= MAX_SAMPLING_WINDOW )
      p_I->Now = 0;

   /* subtract oldest bandwidth used */
   p_I->Curr -= p_I->Window[p_I->Now];
   if( p_I->Window[p_I->Now] > p_Funnel->Bandwidth )
      p_Funnel->Bandwidth = 0L;
   else
      p_Funnel->Bandwidth -= p_I->Window[p_I->Now];
   p_I->Window[p_I->Now] = 0L;

   /* check congestion timer */
   if( p_Funnel->Bandwidth == 0L )
      FU_Unblock( p_Funnel );

   if ( !p_Funnel->ExtCongestion )
   {
      /* increase gauge by one step */
      full_gauge = p_Funnel->Committed.Max + p_Funnel->Excess.Max;
      up_gauge = p_Funnel->Step;
      if( (p_Funnel->CurrentGauge + up_gauge) <= full_gauge )
      {
         XX_Event( EV_LOOSEN_GAUGE, "FUNL" );
         FU_SetGauge( p_Funnel, p_Funnel->CurrentGauge + up_gauge );
      }
      else
         FU_SetGauge( p_Funnel, full_gauge );
   }
   /* transmit all frames now within the gauge */
   Flush( p_Funnel );

   /* if this funnel is still active, re-start its timer */
   if( (p_Funnel->Depth > 0) || (p_Funnel->CurrentGauge < p_Funnel->Excess.Max) )
      TIMER_Start( p_Funnel->Timer, p_Funnel->MeasurementInterval,
      FALSE, FU_Tick, p_Funnel->QueueId, 0);

   return E_OK;
}

/*********************************************************************
 * @purpose     Internal routine.
 *              Allocate a new data structure for a funnel.
 *
 *
 * @param       @b{(input)}  n/a
 *
 * @returns     pointer to funnel on success
 * @returns     0 otherwise
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static t_Funnel *AllocFunnel( void )
{
   t_Funnel *p;

#if MAX_PREALLOC_FUNNEL > 0
   XX_Lock( &FU_Mutex );

   for(p = &_Funnel[0]; p <= &_Funnel[MAX_PREALLOC_FUNNEL-1]; p++)
   {
      if( p->Taken == FALSE )
      {
         memset( p, 0, sizeof(t_Funnel) );
         p->Prealloc = TRUE;
         p->Taken = TRUE;
         p->Links = 1;
         XX_Unlock(&FU_Mutex);
         return p;
      }
   }

   XX_Unlock(&FU_Mutex);
#endif

   /* try and allocate from user memory */
   p = (t_Funnel *)XX_Malloc( sizeof(t_Funnel) );
   if( p )
   {
         memset( p, 0, sizeof(t_Funnel) );
         p->Prealloc = FALSE;
         p->Taken = TRUE;
         p->Links = 1;
   }

   return p;
}

/*********************************************************************
 * @purpose     Store the frame in the funnel. If unsuccessful, return
 *              an error code, but don't release the frame: this is done
 *              by the calling routine.
 *
 *
 * @param p_Funnel    @b{(input)}  pointer to funnel
 * @param Owner       @b{(input)}  the owner of the frame
 * @param p_Frame     @b{(input)}  pointer to frame
 * @param Length      @b{(input)}  length of frame (in bytes)
 * @param CongBits    @b{(input)}  settings of congestion bits
 *
 * @returns           E_OK           success
 * @returns           E_FAIL         can't disable scheduling
 * @returns           E_NO_ROOM      no room in buffer - discard
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err StoreInFunnel( t_Funnel *p_Funnel, t_Handle Owner,
                            void *p_Frame, ulng Length, word CongBits )
{
   word           Next;
   t_FunnelEntry *p_E;

   /* make sure the funnel's timer is running */
   TIMER_StartIfStopped( p_Funnel->Timer, p_Funnel->MeasurementInterval,
      FU_Tick, p_Funnel->QueueId );

   /* get next index */
   Next = p_Funnel->Tail+1;
   if( Next > FUNNEL_DEPTH )
      Next = 0;

   /* see if there's room in the funnel */
   if( (Next == p_Funnel->Head) ||
         ((p_Funnel->Depth+Length) > p_Funnel->MaxDepth) )
   {
      p_Funnel->Congested = TRUE;
      if( p_Funnel->f_Busy )
         p_Funnel->f_Busy( Owner, 1 );
      return E_NO_ROOM;
   }

   /* O.K. - add the frame to the funnel's internal buffer */
   p_E = &p_Funnel->Entry[p_Funnel->Tail];
   p_E->Owner = Owner;
   p_E->p_Frame = p_Frame;
   p_E->Length = Length;
   p_E->CongBits = CongBits;
   p_Funnel->Tail = Next;
   p_Funnel->Frames++;
   p_Funnel->Buffs += F_HowManyBuffs(p_Frame);
   p_Funnel->Depth += Length;

   /* check for busy (call on each frame if necessary) */
   if( p_Funnel->f_Busy )
   {
      if( p_Funnel->BuffsToBusy && (p_Funnel->Buffs >= p_Funnel->BuffsToBusy) )
      {
         p_Funnel->Congested = TRUE;
         p_Funnel->f_Busy( Owner, 1 );
      }
   }

   return E_OK;
}

/*********************************************************************
 * @purpose        Flush the funnel. This transmits all frames that are
 *                 within the current gauge.
 *
 *
 * @param  p_Funnel     @b{(input)}  pointer to funnel
 *
 * @returns             n/a
 *
 * @notes
 *                 It is called each time the
 *                 gauge is increased or more bandwidth is freed
 *                 (from FU_Tick).
 *
 *
 * @end
 * ********************************************************************/
static void Flush( t_Funnel *p_Funnel )
{
   t_FunnelEntry *p_E;
   e_Err          e;

   /*
    * while a. there are more frames buffered and
    *       b. there is bandwidth available
    *    get next frame from buffer and process
    */
   while( p_Funnel->Head != p_Funnel->Tail )
   {
      ulng Length;
      word n;

      p_E = &p_Funnel->Entry[p_Funnel->Head];
      Length = p_E->Length;

      /* check if bandwidth is available */
      if( (Length + p_Funnel->Bandwidth) > p_Funnel->CurrentGauge )
         break;

      n = F_HowManyBuffs( p_E->p_Frame );

      /* process frame */
      e = Process( p_Funnel, p_E->Owner, p_E->p_Frame,
                   Length, p_E->CongBits );

      /* discard frame if necessary */
      if( e != E_OK )
      {
         F_Delete( p_E->p_Frame );
         /* notify error logger */
         XX_Error( ERR_FLUSH_FAILED, "FUNL" );
      }

      /* remove frame from internal buffer */
      p_Funnel->Depth -= Length;
      p_Funnel->Frames--;
      p_Funnel->Buffs -= n;
      /* tell owner if we're now clear */
      if( p_Funnel->Congested && p_Funnel->BuffsToBusy )
      {
         if( p_Funnel->Buffs <= p_Funnel->BuffsToClear )
         {
            p_Funnel->Congested = FALSE;
            p_Funnel->f_Busy( p_E->Owner, 0 );
         }
      }

      p_Funnel->Head++;
      if( p_Funnel->Head > FUNNEL_DEPTH )
         p_Funnel->Head = 0;
   }

   /* returns void */
}



/*********************************************************************
 * @purpose     Processes the frames from the specified funnel.
 *
 *
 * @param  p_Funnel   @b{(input)}  pointer to funnel
 * @param  Owner      @b{(input)}  owner of frame
 * @param  p_Frame    @b{(input)}  pointer to frame
 * @param  Length     @b{(input)}  length of frame (in bytes)
 * @param  CongBits   @b{(input)}  current congestion bits settings
 *
 * @returns           E_OK           success
 * @returns           failure codes may be returned by connection's
 *                    transmit routine
 *
 * @notes
 *       The algorithm is defined as follows:
 * if DE was 0
 * if within the committed rate, pass along with DE=0
 * else if within the excess rate, pass along with DE=1
 * else discard
 * if DE was 1
 * if within the excess rate, pass along with DE=1
 * else if within the committed rate, pass along with DE=0
 * else discard
 *
 *            The gauge settings have alreay been checked - this routine
 *            checks whether *or not the DE bit should be set (according
 *            to committed/excess info rates) and passes the frame to
 *            the connection handler for further handling.
 *
 *
 *
 * @end
 * ********************************************************************/
static e_Err Process( t_Funnel *p_Funnel, t_Handle Owner, void *p_Frame,
                       ulng Length, word CongBits )
{
   e_Err e = E_OK;

   if( !(*p_Funnel->p_f_Handler) )
      return E_DISCARD;

   /* test current DE bit setting */
   if( !(CongBits & FRAME_DE) )
   {
      /* DE bit wasn't originally set */

      if( (p_Funnel->Committed.Curr + Length) <= p_Funnel->Committed.Max )
      {
         /* add to committed info rate usage */
         p_Funnel->Committed.Curr += Length;
         p_Funnel->Committed.Window[p_Funnel->Committed.Now] += Length;
         CongBits |= FRAME_RATE1;
      }
      else if( (p_Funnel->Excess.Curr + Length) <= p_Funnel->Excess.Max )
      {
         XX_Event( EV_CIR_EXCEDED, "FUNL" );
         /* add to excess info rate usage */
         CongBits |= FRAME_DE;
         p_Funnel->Excess.Curr += Length;
         p_Funnel->Excess.Window[p_Funnel->Excess.Now] += Length;
         CongBits |= FRAME_RATE2;
      }
      else
      {
         /* can't process frame - mark for discard */
         XX_Event( EV_EIR_EXCEDED, "FUNL" );
         e = E_DISCARD;
      }
   }
   else
   {
      /* DE bit was originally set */

      if( (p_Funnel->Excess.Curr + Length) <= p_Funnel->Excess.Max )
      {
         /* add to excess info rate usage */
         XX_Event( EV_CIR_EXCEDED, "FUNL" );
         p_Funnel->Excess.Curr += Length;
         p_Funnel->Excess.Window[p_Funnel->Excess.Now] += Length;
         CongBits |= FRAME_RATE2;
      }
      else if( (p_Funnel->Committed.Curr + Length) <= p_Funnel->Committed.Max)
      {
         /* add to committed info rate usage */
         p_Funnel->Committed.Curr += Length;
         p_Funnel->Committed.Window[p_Funnel->Committed.Now] += Length;
         CongBits |= FRAME_RATE1;
      } else
      {
         /* can't process frame - mark for discard */
         XX_Event( EV_EIR_EXCEDED, "FUNL" );
         e = E_DISCARD;
      }
   }

   /* if O.K., pass frame to handler */
   if( e == E_OK )
      (*(*p_Funnel->p_f_Handler))( Owner, p_Frame, CongBits );

   /* make sure the funnel's timer is running */
   TIMER_StartIfStopped( p_Funnel->Timer, p_Funnel->MeasurementInterval,
      FU_Tick, p_Funnel->QueueId );

   /* add to gauge whether transmitted successfully or not */
   p_Funnel->Bandwidth += Length;

   return e;
}



/* --- end of file fu.c --- */

/*
 *
 * Product: EmWeb/UPNP
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * Timer services definitions
 *
 */
#ifndef _EWU_TIMER_H_
#define _EWU_TIMER_H_

#ifdef EW_CONFIG_OPTION_UPNP

/*
 * The type of a timer event function.
 * When the event triggers, the event function will be called with the
 * applicable context as the first argument, and the opaque poiner as the
 * second argument.  Both of these values were stored in the timer event object
 * by the call to ewaTimerSetEvent/Random from arguments to that call.
 */
typedef void (*EwaTimerWakeupFunc)(EwsContext, void *);

/*
 * The type of the timer event object.
 * The type of the triggerTime field is set in config.xxx/ew_config.h, since
 * it depends on the underlying operating system.
 */
typedef struct ewaTimerEvent_s
{
  uint32 magic;                  /* magic value to show validity of timer
                                  * event handles */
  uint32 seqNo;                  /* sequence number of timer event */
  struct ewaTimerEvent_s *next;  /* pointer to next in the linked list */
                                 /* must be first field for algorithms to
                                  * work */
  EwaTimerWakeupFunc wakeupFunc; /* wakeupFunc is the wakeup function to
                                  * call when the event happens */
  EwsContext context;            /* context to pass to the wakeup func */
  void *opaquePtr;               /* opaque pointer to pass to wakeup func */
  EwaTime triggerTime;           /* time to wake up */
} EwaTimerEvent;

typedef struct
{
  EwaTimerEvent *event;         /* event structure */
  uint32 seqNo;                 /* sequence number that should match the
                                 * sequence number in the event structure */
} EwaTimerHandle;

/* References to the 'null' timer event handle. */
extern EwaTimerHandle ewaTimerHandleNull;
/* Suitable as an expression. */
#define EWA_TIMER_HANDLE_NULL ewaTimerHandleNull
/* Suitable for an initializer. */
#define EWA_TIMER_HANDLE_NULL_INITIALIZER      { NULL, 0xFFFFFFFF}
/* Test for null-ness of a timer handle. */
#define EWA_TIMER_HANDLE_IS_NULL(handle) (NULL == handle.event)

/* Magic number for event structures. */
#define EWA_TIMER_MAGIC 0x1517C0DE

/*
 * ewaTimerSetEvent
 * This function is called to set a timer and queue the request on a linked
 * list.
 * timerHandle = ewaTimerSetEvent(timeout, wakeupFunc, context, opaque_ptr);
 *
 * Input Args: timeout - period for the timeout in seconds from now
 *             wakeupFunc - function to call once woken up (when timer
 *                          expires).
 *             context - context associated with this request.
 *             opaque_ptr - void *, opaque pointer to be passed to the wakeup
 *                          function.
 *
 * Returns - an ewaTimerHandle which is a pointer to this entry in the
 *           linked list of timer requests.
 *
 * Unless the timer is cancelled earlier by a call to ewaTimerCancelEvent,
 * ewaTimerCheck() (in the application code) will call
 * 'wakeupFunc(context, opaque_ptr)' at or shortly after the time
 * period expires.
 *
 * Memory is allocated for the timer request structure on the linked list.
 * Note that the memory is freed either by making a call to
 * ewaTimerCancelEvengt() or by ewaTimerCheck() when wakupFunc is called.
 */
EwaTimerHandle ewaTimerSetEvent( uint32 timeout
                                ,EwaTimerWakeupFunc wakeupFunc
                                ,EwsContext context
                                ,void *opaque);

/*
 * ewaTimerSetEventRandom
 * Just like timerSet, except that the timeout value is a maximum delay.
 * The delay used will be randomly chosen from the set 0..timeout.
 */
EwaTimerHandle ewaTimerSetEventRandom( uint32 max_timeout
                                      ,EwaTimerWakeupFunc wakeupFunc
                                      ,EwsContext context
                                      ,void *opaque);

/*
 * ewaTimerCancelEvent
 * This function is called to cancel an ewaTimerSet request.  It will search
 * the linked list of timer requests for a matching timerHandle.
 * If it is found, then the list will be adjusted properly and the
 * memory for the timer request structure will be deallocated.
 * Return status indicates if function is able to find tHandle as an active
 * timer handle or not.
 * Because tHandle is only used in pointer comparisons, it is safe to call
 * timerCancel on handles that may have triggered or been canceled as long as
 * no new timer events have been scheduled since then (since the memory for the
 * event structure may have been re-used).
 *
 * status = ewaTimerCancelEvent(timerHandle);
 *
 * Input Args: timerHandle
 *
 * Returns - status: an EwaStatus to indicate if we were able to successfully
 *           cancel the timer request or not.
 */
EwaStatus ewaTimerCancelEvent(EwaTimerHandle timerHandle);

/*
 * ewaTimerGetClock
 * Returns a 32-bit representation of the current time with the highest
 * precision available.  This is used for generation of unique identifiers in
 * the system, not timing, so what is needed are the fastest-changing bits
 * of the time, and it does not matter if high-order bits are lost.
 */
uint32 ewaTimerGetClock(void);

#endif /* EW_CONFIG_OPTION_UPNP */
#endif /* _EWU_TIMER_H_ */

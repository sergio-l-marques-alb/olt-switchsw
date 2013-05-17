/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garp_timer.h
* @purpose     Header file for GARP Timer task
* @component   GARP
* @comments    none
* @create      11/02/2005
* @author      vrynkov
* @author      
* @end
*             
**********************************************************************/
#ifndef _INCLUDE_GARP_TIMER_H_
#define _INCLUDE_GARP_TIMER_H_

#include "commdefs.h"
#include "l7_common.h"
#include "datatypes.h"
#include "dot1dgarp.h"

                                        
#define GARP_TIMER_GRANULARITY_SHORT 100
  
typedef enum
{
    GARP_TIMER_UNDEFINED = 0,
    GARP_TIMER_PENDING   = 1,
    GARP_TIMER_POPPED

} GARP_TIMER_STATE_t;

typedef enum
{
    GARP_TIMER_TIMER_UNDEFINED  = 0,
    GARP_TIMER_GVRP_LEAVE       = 1,
    GARP_TIMER_GMRP_LEAVE,
    GARP_TIMER_GVRP_JOIN,
    GARP_TIMER_GMRP_JOIN
  
} GARP_TIMER_TYPE_t;



typedef struct garpTimerDescr_s
{       
    GARP_TIMER_TYPE_t timerType;
    L7_uint32       vid;                /* parm1 to be passed to timeOutFunc by garpTimerProcess() */
    L7_uint32       port_no;            /* parm2 to be passed to timeOutFunc by garpTimerProcess() */
    L7_int32        expiryTime;         /* in milliseconds */
    GARP_TIMER_STATE_t timer_status;
    struct garpTimerDescr_s *next;

} garpTimerDescr_t;


typedef struct garpTimerList_s
{
   garpTimerDescr_t     *head;
   void                 *sema;
   L7_uint32            listCount;
   GARP_TIMER_TYPE_t    listType;

}garpTimerList_t;

typedef struct garpTimerInfo_s
{
	garpTimerList_t    garpTimersPendingList;    /* list of GARP timers which have not timed out */
    garpTimerList_t    garpTimersPoppedList;     /* list of GARP timers which have popped and 
                                                 need to be serviced by the GARP task */
} garpTimerInfo_t;



void garpTimerListInit();
void garpTimerListFini();

void garpTimerAdd(L7_uint32 vid, L7_uint32 port_no,
                  L7_int32 milliseconds, garpTimerDescr_t *pTimerHolder,
                  GARP_TIMER_TYPE_t timer_type);

L7_RC_t garpTimerFree(garpTimerDescr_t *pTimerInstance);
L7_uint32 garpTimerListCountGet(garpTimerList_t *timerList);
garpTimerDescr_t * garpTimerListDeleteFirst(garpTimerList_t *timerList);
void garpTimerListPrint(garpTimerList_t *tempList, L7_uint32 timeToExpire);

L7_RC_t garpTimerActivateProcess(void);
L7_RC_t garpTimerDeactivateProcess(GARPApplication application);
void garpTimerCallback(void);
void garpTimerAction();

extern void *garpTaskSyncSema;
extern void *garpQueue;
extern garpTimerInfo_t garpTimers;


#endif  /* #ifndef _INCLUDE_GARP_TIMER_H_ */

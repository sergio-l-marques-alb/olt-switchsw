/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_timer.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_TIMER_H
#define _PIMDM_TIMER_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l7apptimer_api.h"
#include "pimdm_main.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/
/* None */

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmTimersDeInit (pimdmCB_t *pimdmCB);

extern L7_RC_t
pimdmTimersInit (pimdmCB_t *pimdmCB);

extern void
pimdmAppTimerExpiryHandler (L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                            void *timerArg);

#endif  /* _PIMDM_TIMER_H */


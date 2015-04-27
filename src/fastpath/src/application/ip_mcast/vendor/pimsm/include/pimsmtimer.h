/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmTimer.h
*
* @purpose Contains all timer related Macros.
*
* @component pimsm
*
* @comments 
*
* @create 01/01/2006
*
* @author dsatyanarayana
* @end
*
**********************************************************************/
#ifndef PIMSM_TIMER_H
#define PIMSM_TIMER_H

#include "l7apptimer_api.h"

struct pimsmCB_s;
struct pimsmEB_s;
struct pimsmSGNode_s;
struct pimsmStarGNode_s;
struct pimsmStarStarRpNode_s;
struct pimsmSGRptNode_s;
struct pimsmNeighborEntry_s;

/* Add all params for timer event to process */
typedef struct pimsmTimerData_s
{
  struct pimsmCB_s *pimsmCb;  
  L7_uint32 rtrIfNum; 
  L7_inet_addr_t addr1; /* store srcAddr or rpAddr or nbrAddr */
  L7_inet_addr_t addr2; /* store grpAddr only */
} pimsmTimerData_t;

extern L7_RC_t pimsmTimersInit(struct pimsmCB_s *pimsmCb);
extern L7_RC_t pimsmTimersDeInit(struct pimsmCB_s *pimsmCb);
extern void pimsmSPTTimerExpiresHandler(void *pParam);

extern L7_RC_t
pimsmUtilAppTimerSet (struct pimsmCB_s *pimsmCB,
                      L7_app_tmr_fn pFunc,
                      void *pParam,
                      L7_uint32 timeOut,
                      L7_APP_TMR_HNDL_t *tmrHandle,
                      L7_uchar8 *timerName);

extern L7_RC_t
pimsmUtilAppTimerCancel (struct pimsmCB_s *pimsmCB,
                         L7_APP_TMR_HNDL_t *tmrHandle);

#endif /*PIMSM_TIMER_H */


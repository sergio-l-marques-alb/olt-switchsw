/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDnStrmSGFSM.h
*
* @purpose Contains PIM-SM Down stream per interface State Machine 
* declarations for (S,G) entry.
*
* @component    pimsm 
*
* @comments 
*
* @create 01/01/2006
*
* @author dsatyanarayana / vmurali
* @end
*
***********************************************************************/
#ifndef _PIMSM_DNSTRM_SG_FSM_H_
#define _PIMSM_DNSTRM_SG_FSM_H_
/*#include "pimsmmain.h"*/

typedef enum
{
  PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO = 0,
  PIMSM_DNSTRM_S_G_SM_STATE_JOIN,
  PIMSM_DNSTRM_S_G_SM_STATE_PRUNE_PENDING,
  PIMSM_DNSTRM_S_G_SM_STATE_MAX
} pimsmDnStrmPerIntfSGStates_t;

typedef enum
{
  PIMSM_DNSTRM_S_G_SM_EVENT_RECV_JOIN = 0,
  PIMSM_DNSTRM_S_G_SM_EVENT_RECV_PRUNE,
  PIMSM_DNSTRM_S_G_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES,
  /* Note: the following event will be triggered when pimsmSGIJoinExpiryTimer
   * expires
   */
  PIMSM_DNSTRM_S_G_SM_EVENT_EXPIRY_TIMER_EXPIRES,
  PIMSM_DNSTRM_S_G_SM_EVENT_MAX
} pimsmDnStrmPerIntfSGEvents_t;

typedef struct pimsmDnStrmPerIntfSGEventInfo_s
{
  pimsmDnStrmPerIntfSGEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_short16  holdtime;
}pimsmDnStrmPerIntfSGEventInfo_t;

struct pimsmSGNode_s;
struct pimsmCB_s;

extern L7_RC_t pimsmDnStrmPerIntfSGExecute (struct pimsmCB_s * pimsmCb, 
                                            struct pimsmSGNode_s * pSGNode, 
                                            pimsmDnStrmPerIntfSGEventInfo_t
                                            *dnStrmPerIntfSGEventInfo);

#endif /* _PIMSM_DNSTRM_SG_FSM_H_ */

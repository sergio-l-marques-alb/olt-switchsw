/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDnStrmStarGFSM.h
*
* @purpose Contains PIM-SM Down stream per interface State Machine 
* declarations for (*,G) entry.
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
#ifndef _PIMSM_DWSTRM_STR_G_FSM_H_
#define _PIMSM_DWSTRM_STR_G_FSM_H_
/*#include "pimsmmain.h"*/

struct pimsmCB_s;

typedef enum
{
  PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO = 0,
  PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN,
  PIMSM_DNSTRM_STAR_G_SM_STATE_PRUNE_PENDING,
  PIMSM_DNSTRM_STAR_G_SM_STATE_MAX
} pimsmDnStrmPerIntfStarGStates_t;

typedef enum
{
  PIMSM_DNSTRM_STAR_G_SM_EVENT_RECV_JOIN = 0,
  PIMSM_DNSTRM_STAR_G_SM_EVENT_RECV_PRUNE,
  PIMSM_DNSTRM_STAR_G_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES,
  PIMSM_DNSTRM_STAR_G_SM_EVENT_EXPIRY_TIMER_EXPIRES,
  PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX
} pimsmDnStrmPerIntfStarGEvents_t;

typedef struct pimsmDnStrmPerIntfStarGEventInfo_s
{
  pimsmDnStrmPerIntfStarGEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_short16  holdtime;
}pimsmDnStrmPerIntfStarGEventInfo_t;

struct pimsmStarGNode_s;

extern L7_RC_t pimsmDnStrmPerIntfStarGExecute 
(struct pimsmCB_s * pimsmCb,
 struct pimsmStarGNode_s  *pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);

#endif /* _PIMSM_DWSTRM_STR_G_FSM_H_ */

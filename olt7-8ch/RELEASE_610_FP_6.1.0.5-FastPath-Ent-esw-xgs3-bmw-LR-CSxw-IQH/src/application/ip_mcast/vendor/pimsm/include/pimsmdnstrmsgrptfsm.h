/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmDnStrmSGRptFSM.h
*
* @purpose Contains PIM-SM Down stream per interface State Machine 
* declarations for (S,G,Rpt) entry.
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
#ifndef _PIMSM_DWSTRM_SG_RPT_FSM_H
#define _PIMSM_DWSTRM_SG_RPT_FSM_H
/*#include "pimsmmain.h"*/

typedef enum
{
  PIMSM_DNSTRM_S_G_RPT_SM_STATE_NO_INFO = 0, 
  PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE,
  PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING,
  PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_TMP,
  PIMSM_DNSTRM_S_G_RPT_SM_STATE_PRUNE_PENDING_TMP,
  PIMSM_DNSTRM_S_G_RPT_SM_STATE_MAX
} pimsmDnStrmPerIntfSGRptStates_t;


typedef enum
{
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_RECV_JOIN_STAR_G = 0, 
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_RECV_JOIN_SG_RPT,
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_RECV_PRUNE_SG_RPT,
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_END_OF_MSG,
  /* Note: both Expiry and Prune-Pending timers are  
   * set when a valid Prune(S,G,rpt) is received.
   */
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES,
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_EXPIRY_TIMER_EXPIRES,
  PIMSM_DNSTRM_S_G_RPT_SM_EVENT_MAX
} pimsmDnStrmPerIntfSGRptEvents_t;

typedef struct pimsmDnStrmPerIntfSGRptEventInfo_s
{
  pimsmDnStrmPerIntfSGRptEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_short16  holdtime;
}pimsmDnStrmPerIntfSGRptEventInfo_t;

struct pimsmSGRptNode_s;
struct pimsmCB_s;

extern L7_RC_t pimsmDnStrmPerIntfSGRptExecute 
(struct pimsmCB_s * pimsmCb, 
 struct pimsmSGRptNode_s  *pSGRptNode, 
 pimsmDnStrmPerIntfSGRptEventInfo_t * pDnStrmPerIntfSGRptEventInfo);

#endif /* _PIMSM_DWSTRM_SG_RPT_FSM_H */

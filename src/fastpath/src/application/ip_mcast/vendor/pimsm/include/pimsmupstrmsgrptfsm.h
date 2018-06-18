/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmSGRptFSM.h
*
* @purpose Contains declarations for (S,G,Rpt) Upstream State machine
*
* @component 
*
* @comments 
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda
* @end
*
**********************************************************************/
#ifndef _PIMSM_UPSTRM_SG_RPT_FSM_H
#define _PIMSM_UPSTRM_SG_RPT_FSM_H
#include "pimsmsgrpttree.h"
/*#include "pimsmmain.h"*/
typedef enum pimsmUpStrmSGRptStates_s
{
  PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED = 0,
  PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
  PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
  PIMSM_UPSTRM_S_G_RPT_SM_STATE_MAX
} pimsmUpStrmSGRptStates_t;

typedef enum
{
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_TRUE = 0,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_PRUNE_DESIRED_FALSE,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_RPT_JOIN_DESIRED_FALSE,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_INHERITED_OLIST_NOT_NULL,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_OVERRIDE_TIMER_EXPIRES,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_PRUNE,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_JOIN,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_SEE_PRUNE_S_G,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_RPF_CHANGED,
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_MAX
} pimsmUpStrmSGRptEvents_t;

typedef struct pimsmUpStrmSGRptEventInfo_s
{
  pimsmUpStrmSGRptEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_uint32 data;
  L7_short16 holdtime; 
}pimsmUpStrmSGRptEventInfo_t;

struct pimsmSGRptNode_s;
struct pimsmCB_s;

extern L7_RC_t pimsmUpStrmSGRptExecute (struct pimsmCB_s * pimsmCb,
                                        struct pimsmSGRptNode_s * pSGRptNode,
                                        pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);

#endif /*_PIMSM_UPSTRM_SG_RPT_FSM_H*/

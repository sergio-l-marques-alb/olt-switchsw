/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmSGFSM.h
*
* @purpose Contains declarations for Upstream (S,G) state machine
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
#ifndef _PIMSM_UPSTRMSTRM_SG_FSM_H
#define _PIMSM_UPSTRMSTRM_SG_FSM_H
/*#include "pimsmmain.h"*/
#include "pimsmsgtree.h"
#include "pimsmcontrol.h"
#include "pimsmdefs.h"

struct pimsmCB_s;

typedef enum
{
  PIMSM_UPSTRM_S_G_SM_STATE_NOT_JOINED = 0,
  PIMSM_UPSTRM_S_G_SM_STATE_JOINED,
  PIMSM_UPSTRM_S_G_SM_STATE_MAX
} pimsmUpStrmSGStates_t;

typedef enum
{
  PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE = 0,
  PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE,
  PIMSM_UPSTRM_S_G_SM_EVENT_TIMER_EXPIRES,
  PIMSM_UPSTRM_S_G_SM_EVENT_SEE_JOIN,
  PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_S_G,
  PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_S_G_RPT,
  PIMSM_UPSTRM_S_G_SM_EVENT_SEE_PRUNE_STAR_G,
  PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT,
  PIMSM_UPSTRM_S_G_SM_EVENT_GEN_ID_CHANGED,
  PIMSM_UPSTRM_S_G_SM_EVENT_RPF_CHANGED_DUE_TO_ASSERT,
  PIMSM_UPSTRM_S_G_SM_EVENT_MAX
} pimsmUpStrmSGEvents_t;

typedef struct pimsmUpStrmSGEventInfo_s
{
  pimsmUpStrmSGEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_inet_addr_t            upStrmNbrAddr;  
  mcastRPFInfo_t rpfInfo;
  L7_short16 holdtime;
  L7_BOOL   sendImmdFlag;    /* Send JP pkt immediately, dont do bundling */  
}pimsmUpStrmSGEventInfo_t;

struct pimsmSGNode_s;

extern L7_RC_t pimsmUpStrmSGExecute (struct pimsmCB_s * pimsmCb, 
                                     struct pimsmSGNode_s *pSGNode, 
                                     pimsmUpStrmSGEventInfo_t *pUpStrmSGEventInfo);

extern void pimsmSGKeepaliveTimerExpiresHandler(void *pParam);
#endif /* _PIMSM_UPSTRMSTRM_SG_FSM_H */

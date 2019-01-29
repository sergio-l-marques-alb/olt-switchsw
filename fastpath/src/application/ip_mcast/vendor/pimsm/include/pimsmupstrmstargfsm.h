/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmStarGFSM.h
*
* @purpose Contains declarations (*,G) Upstream State Machine
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
#ifndef _PIMSM_UPSTRM_STAR_G_FSM_H
#define _PIMSM_UPSTRM_STAR_G_FSM_H
/*#include "pimsmmain.h"*/
#include "pimsmstargtree.h"
#include "pimsmdefs.h"

struct pimsmCB_s;

typedef enum
{
  PIMSM_UPSTRM_STAR_G_SM_STATE_NOT_JOINED = 0,
  PIMSM_UPSTRM_STAR_G_SM_STATE_JOINED,
  PIMSM_UPSTRM_STAR_G_SM_STATE_MAX
} pimsmUpStrmStarGStates_t;

typedef enum
{
  PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE = 0,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_TIMER_EXPIRES,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_SEE_JOIN,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_SEE_PRUNE,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_DUE_TO_ASSERT,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_RPF_CHANGED_NOT_DUE_TO_ASSERT,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_GEN_ID_CHANGED,
  PIMSM_UPSTRM_STAR_G_SM_EVENT_MAX
} pimsmUpStrmStarGEvents_t;

typedef struct pimsmUpStrmStarGEventInfo_s
{
  pimsmUpStrmStarGEvents_t  eventType;
  L7_uint32                 rpfIfIndex;
  L7_inet_addr_t            rpAddr;
  L7_inet_addr_t            upStrmNbrAddr;
  L7_short16 holdtime;
  mcastRPFInfo_t rpfInfo;
  L7_uint32 rtrIfNum;
  L7_BOOL   sendImmdFlag;    /* Send JP pkt immediately, dont do bundling */  
}pimsmUpStrmStarGEventInfo_t;

struct pimsmStarGNode_s;

extern L7_RC_t pimsmUpStrmStarGExecute (struct pimsmCB_s * pimsmCb,
                                        struct pimsmStarGNode_s * pStarGNode,
                                        pimsmUpStrmStarGEventInfo_t * pUpStrmStarGEventInfo);

#endif /*_PIMSM_UPSTRM_STAR_G_FSM_H*/

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmStarStarRpFSM.h
*
* @purpose Contains declarations for (*,*,Rp) upstream state machine
*
* @component 
*
* @comments 
*
* @create 08/18/2005
*
* @author Muralidhar Vakkalagadda / Satya Dillikar
* @end
*
**********************************************************************/
#ifndef _PIMSM_UPSTRM_STAR_STAR_RP_FSM_H_
#define _PIMSM_UPSTRM_STAR_STAR_RP_FSM_H_
/*#include "pimsmmain.h"*/
#include "pimsmstarstarrptree.h"
#include "pimsmdefs.h"

struct pimsmCB_s;
typedef enum
{
  PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_NOT_JOINED = 0,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_JOINED,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_STATE_MAX
} pimsmUpStrmStarStarRPStates_t;

typedef enum
{
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_TRUE= 0,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_JOIN_DESIRED_FALSE,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_TIMER_EXPIRES,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_SEE_JOIN,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_SEE_PRUNE,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_NBR_CHANGED,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_GEN_ID_CHANGED,
  PIMSM_UPSTRM_STAR_STAR_RP_SM_EVENT_MAX
} pimsmUpStrmStarStarRPEvents_t;

typedef struct pimsmUpStrmStarStarRPEventInfo_s
{
  pimsmUpStrmStarStarRPEvents_t    eventType;
  L7_short16 holdtime;
  mcastRPFInfo_t rpfInfo;
  L7_uint32 rtrIfNum;
  L7_inet_addr_t            upStrmNbrAddr;  
  L7_BOOL   sendImmdFlag;    /* Send JP pkt immediately, dont do bundling */  
}pimsmUpStrmStarStarRPEventInfo_t;

struct pimsmStarStarRpNode_s;
extern L7_RC_t pimsmUpStrmStarStarRpExecute (struct pimsmCB_s * pimsmCb,
                                             struct pimsmStarStarRpNode_s * pStarStarRPNode,
                                             pimsmUpStrmStarStarRPEventInfo_t *pUpstrmStarStarRpEventInfo);
#endif /* _PIMSM_UPSTRM_STAR_STAR_RP_FSM_H_ */

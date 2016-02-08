/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmCandBsrFSM.h
*
* @purpose Contains Candidate BSR FSM related structs, events definitions
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

#ifndef _PIMSM_CAND_BSR_FSM_H_
#define _PIMSM_CAND_BSR_FSM_H_

typedef enum
{
  PIMSM_CANDIDATE_BSR_SM_STATE_CANDIDATE_BSR = 0,
  PIMSM_CANDIDATE_BSR_SM_STATE_PENDING_BSR,
  PIMSM_CANDIDATE_BSR_SM_STATE_ELECTED_BSR,
  PIMSM_CANDIDATE_BSR_SM_STATE_MAX                      
}pimsmPerScopeZoneCandBSRStates_t;

typedef enum
{
  PIMSM_CANDIDATE_BSR_SM_EVENT_RECV_PREFERRED_BSM = 0, 
  PIMSM_CANDIDATE_BSR_SM_EVENT_BOOTSTRAP_TIMER_EXPIRES,
  PIMSM_CANDIDATE_BSR_SM_EVENT_RECV_NON_PREFERRED_BSM,
  PIMSM_CANDIDATE_BSR_SM_EVENT_RECV_NON_PREFERRED_BSM_FROM_ELECTED_BSR,
  PIMSM_CANDIDATE_BSR_SM_EVENT_MAX
} pimsmPerScopeZoneCandBSREvents_t;

struct   pimsmBsrPerScopeZone_s;

typedef struct pimsmPerScopeZoneCandBSREventInfo_s
{
  pimsmPerScopeZoneCandBSREvents_t    eventType;
  struct pimsmBsrPerScopeZone_s      *pBsrNode;
  struct
  {
    L7_uint32  rtrIfNum;
    L7_uchar8 *pimHeader;
    L7_uint32  pimPktLen;
  } pimsmPktInfo;
}pimsmPerScopeZoneCandBSREventInfo_t;


extern 
L7_RC_t pimsmPerScopeZoneCandBSRExecute (pimsmCB_t *pimsmCb,
                                         pimsmPerScopeZoneCandBSREventInfo_t
                                          *candBsrEventInfo);

extern void pimsmBsrCandidateBootStrapTimerExpiresHandler (void *pParam);
#endif /* _PIMSM_CAND_BSR_FSM_H_ */

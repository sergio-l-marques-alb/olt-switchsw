/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmNonCandBsrFSM.h
*
* @purpose Contains PIM-SM Non-Candidate BSR state Machine implementation
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
#ifndef _PIMSM_NON_CAND_BSR_FSM_H_
#define _PIMSM_NON_CAND_BSR_FSM_H_

typedef enum
{
  PIM_NON_CANDIDATE_BSR_SM_STATE_NO_INFO = 0,
  PIM_NON_CANDIDATE_BSR_SM_STATE_ACCEPT_ANY,
  PIM_NON_CANDIDATE_BSR_SM_STATE_PREFERRED,
  PIM_NON_CANDIDATE_BSR_SM_STATE_MAX                      
}pimsmPerScopeZoneNonCandBSRStates_t;

typedef enum
{
  PIMSM_NON_CANDIDATE_BSR_SM_EVENT_RECV_BSM = 0, 
  PIMSM_NON_CANDIDATE_BSR_SM_EVENT_SCOPEZONE_TIMER_EXPIRES,
  PIMSM_NON_CANDIDATE_BSR_SM_EVENT_RECV_PREFERRED_BSM,
  PIMSM_NON_CANDIDATE_BSR_SM_EVENT_BOOTSTRAP_TIMER_EXPIRES,
  PIMSM_NON_CANDIDATE_BSR_SM_EVENT_RECV_NON_PREFERRED_BSM,
  PIMSM_NON_CANDIDATE_BSR_SM_EVENT_MAX
} pimsmPerScopeZoneNonCandBSREvents_t;

struct   pimsmBsrPerScopeZone_s;

typedef struct pimsmPerScopeZoneNonCandBSREventInfo_s
{
  pimsmPerScopeZoneNonCandBSREvents_t    eventType;
  struct pimsmBsrPerScopeZone_s         *pBsrNode;
  struct
  {
    L7_uint32  rtrIfNum;
    L7_uchar8 *pimHeader;
    L7_uint32  pimPktLen;
  } pimsmPktInfo;
}pimsmPerScopeZoneNonCandBSREventInfo_t;


extern L7_RC_t pimsmPerScopeZoneNonCandBSRExecute (pimsmCB_t *pimsmCb,
                                                   pimsmPerScopeZoneNonCandBSREventInfo_t 
                                                   *nonCandBsrEventInfo);

extern void pimsmBsrNonCandidateScopeZoneTimerExpiresHandler(void *pParam);

#endif /* _PIMSM_NON_CAND_BSR_FSM_H_ */

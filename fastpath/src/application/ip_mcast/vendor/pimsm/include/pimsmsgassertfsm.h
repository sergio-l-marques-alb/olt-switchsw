/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGAssertFSM.h
*
* @purpose Contains PIM-SM  per interface assert State Machine 
* declarations for (S,G) entry
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
#ifndef _PIMSM_SG_ASSERT_FSM_H
#define _PIMSM_SG_ASSERT_FSM_H
#include "l3_addrdefs.h"
/*#include "pimsmmain.h"*/
#include "pimsmsgtree.h"

struct pimsmCB_s;
typedef enum
{
  PIMSM_ASSERT_S_G_SM_STATE_NO_INFO = 0,
  PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_WINNER,
  PIMSM_ASSERT_S_G_SM_STATE_IAM_ASSERT_LOSER,
  PIMSM_ASSERT_S_G_SM_STATE_MAX
} pimsmPerIntfSGAssertStates_t;

typedef enum
{
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT = 0,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_ASSERT_RPT,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_DATA_PKT,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT,    
  PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TIMER_EXPIRES,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_PREF_ASSERT,
  PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER,
  PIMSM_ASSERT_S_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED,
  PIMSM_ASSERT_S_G_SM_EVENT_ASSERT_TRACKING,
  PIMSM_ASSERT_S_G_SM_EVENT_METRIC_BETTER_THAN_WINNER,
  PIMSM_ASSERT_S_G_SM_EVENT_STOP_BEING_RPF_IFACE,
  PIMSM_ASSERT_S_G_SM_EVENT_RECV_JOIN,
  PIMSM_ASSERT_S_G_SM_EVENT_MAX
} pimsmPerIntfSGAssertEvents_t;

typedef struct pimsmPerIntfSGAssertEventInfo_s
{
  pimsmPerIntfSGAssertEvents_t    eventType;
  L7_uint32     rtrIfNum;
  L7_inet_addr_t  winnerAddress;
  L7_uint32       winnerMetricPref;
  L7_uint32       winnerMetric;
}pimsmPerIntfSGAssertEventInfo_t;


struct pimsmSGNode_s;
L7_RC_t pimsmPerIntfSGAssertExecute (struct pimsmCB_s * pimsmCb,
                                     struct pimsmSGNode_s *pSGNode, 
                                     pimsmPerIntfSGAssertEventInfo_t 
                                     *pPerIntfSGAssertEventInfo);


#endif /* _PIMSM_SG_ASSERT_FSM_H */

/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmStarGAssertFSM.h
*
* @purpose Contains PIM-SM per (*,G) assert State Machine 
* declaration
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
#ifndef _PIMSM_STAR_G_ASSERT_FSM_H_
#define _PIMSM_STAR_G_ASSERT_FSM_H_
/*#include "pimsmmain.h"*/
#include "l3_addrdefs.h"
#include "pimsmstargtree.h"

struct pimsmCB_s;
typedef enum
{
  PIMSM_ASSERT_STAR_G_SM_STATE_NO_INFO = 0,
  PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_WINNER,
  PIMSM_ASSERT_STAR_G_SM_STATE_ASSERT_LOSER,
  PIMSM_ASSERT_STAR_G_SM_STATE_MAX
} pimsmPerIntfStarGAssertStates_t;

typedef enum
{
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT_COULD_ASSERT = 0,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_DATA_PKT,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_ACCEPT_ASSERT,
  PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TIMER_EXPIRES,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_PREF_ASSERT,
  PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_ACCEPT_ASSERT_FROM_CURR_WINNER,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_INFR_ASSERT_FROM_CURR_WINNER,
  PIMSM_ASSERT_STAR_G_SM_EVENT_CURR_WINNER_GEN_ID_CHANGED,
  PIMSM_ASSERT_STAR_G_SM_EVENT_ASSERT_TRACKING,
  PIMSM_ASSERT_STAR_G_SM_EVENT_METRIC_BETTER_THAN_WINNER,
  PIMSM_ASSERT_STAR_G_SM_EVENT_STOP_BEING_RPF_IFACE,
  PIMSM_ASSERT_STAR_G_SM_EVENT_RECV_JOIN,
  PIMSM_ASSERT_STAR_G_SM_EVENT_MAX
} pimsmPerIntfStarGAssertEvents_t;


typedef struct pimsmPerIntfStarGAssertEventInfo_s
{
  pimsmPerIntfStarGAssertEvents_t    eventType;
  L7_uint32 rtrIfNum;
  L7_uint32 data;
  L7_inet_addr_t  winnerAddress;
  L7_uint32          winnerMetricPref;
  L7_uint32          winnerMetric;
  L7_inet_addr_t  srcAddr;
}pimsmPerIntfStarGAssertEventInfo_t;

struct pimsmStarGNode_s;
extern L7_RC_t pimsmPerIntfStarGAssertExecute (struct pimsmCB_s * pimsmCb,
                                               struct pimsmStarGNode_s * pStarGNode,
                                               pimsmPerIntfStarGAssertEventInfo_t *perIntfStarGAssertEventInfo);

#endif /* _PIMSM_STAR_G_ASSERT_FSM_H_ */

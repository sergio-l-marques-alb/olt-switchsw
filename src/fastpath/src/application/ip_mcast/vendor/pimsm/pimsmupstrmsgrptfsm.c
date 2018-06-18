/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmUpStrmSGRptFSM.c
*
* @purpose Contains implementation of (S,G,Rpt) Upstream state machine
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
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmupstrmsgrptfsm.h"
#include "pimsmtimer.h"
#include "pimsmcontrol.h"
#include "pimsmwrap.h"

static L7_RC_t  pimsmUpStrmSGRptJoinSend
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);
static L7_RC_t  pimsmUpStrmSGRptSendPruneOverrideTimerCancel
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);
static L7_RC_t  pimsmUpStrmSGRptOverrideTimerCancel
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);
static L7_RC_t  pimsmUpStrmSGRptJoinSendOverrideTimerLeave
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);
static L7_RC_t  pimsmUpStrmSGRptOverrideTimerSet
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);
static L7_RC_t  pimsmUpStrmSGRptDoNothing
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);
static L7_RC_t  pimsmUpStrmSGRptErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmSGRptNode_t * pSGRptNode,
 pimsmUpStrmSGRptEventInfo_t * pUpStrmSGRptEventInfo);

typedef struct pimsmUpStrmSGRptSM_s
{
  pimsmUpStrmSGRptStates_t nextState;
  L7_RC_t (*pimsmUpStrmSGRptAction) (pimsmCB_t * pimsmCb,
                                     struct pimsmSGRptNode_s * pSGRptNode,
                                     pimsmUpStrmSGRptEventInfo_t
                                     * pUpStrmSGRptEventInfo);
} pimsmUpStrmSGRptSM_t;

static pimsmUpStrmSGRptSM_t pimsmUpStrmSGRpt
[PIMSM_UPSTRM_S_G_RPT_SM_EVENT_MAX]
[PIMSM_UPSTRM_S_G_RPT_SM_STATE_MAX] =
{
  {/*EVENT_PRUNE_DESIRED_TRUE*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptDoNothing},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptSendPruneOverrideTimerCancel}
  },
  {/*EVENT_PRUNE_DESIRED_FALSE*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptJoinSend},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn}
  },
  {/*EVENT_RPT_JOIN_DESIRED_FALSE*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptDoNothing},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptOverrideTimerCancel}
  },
  {/*EVENT_INHERITED_OLIST_NOT_NULL*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptDoNothing},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn}
  },
  {/*EVENT_OVERRIDE_TIMER_EXPIRES*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptJoinSendOverrideTimerLeave}
  },
  {/*EVENT_SEE_PRUNE*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptOverrideTimerSet}
  },
  {/*EVENT_SEE_JOIN*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptOverrideTimerCancel}
  },
  {/*EVENT_SEE_PRUNE_S_G*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptOverrideTimerSet}
  },
  {/*EVENT_RPF_CHANGED*/
    /*RPT_NOT_JOINED*/  {PIMSM_UPSTRM_S_G_RPT_SM_STATE_RPT_NOT_JOINED,
      pimsmUpStrmSGRptErrorReturn},
    /*PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_PRUNED_SG_RPT,
      pimsmUpStrmSGRptErrorReturn},
    /*NOT_PRUNED_SG_RPT*/ {PIMSM_UPSTRM_S_G_RPT_SM_STATE_NOT_PRUNED_SG_RPT,
      pimsmUpStrmSGRptOverrideTimerSet}
  }
};

static void pimsmUpStrmSGRptOverrideTimerExpiresHandler(void *pParam);

/******************************************************************************
* @purpose   execute the Upstream (S,G.Rpt) FSM and process the events received
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmUpStrmSGRptExecute(pimsmCB_t * pimsmCb,
                                pimsmSGRptNode_t *  pSGRptNode,
                                pimsmUpStrmSGRptEventInfo_t
                                * pUpStrmSGRptEventInfo)
{
  pimsmSGRptEntry_t  *pSGRptEntry;
  pimsmUpStrmSGRptStates_t    upStrmSGRptState;
  pimsmUpStrmSGRptEvents_t    upStrmSGRptEvent;
  L7_RC_t             rc = L7_SUCCESS;


  if(pSGRptNode == (pimsmSGRptNode_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR, "Invalid (S,G,Rpt) node");
    return L7_FAILURE;
  }

  if((pSGRptNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  if(pUpStrmSGRptEventInfo == (pimsmUpStrmSGRptEventInfo_t *)L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR, "Invalid event info");
    return L7_FAILURE;
  }

  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;

  upStrmSGRptState = pSGRptEntry->pimsmSGRptUpstreamPruneState;
  upStrmSGRptEvent = pUpStrmSGRptEventInfo->eventType;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO, "State = %s, \n Event = %s ",
              pimsmUpStrmSGRptStateName[upStrmSGRptState],
              pimsmUpStrmSGRptEventName[upStrmSGRptEvent]);

  if(((pimsmUpStrmSGRpt[upStrmSGRptEvent][upStrmSGRptState]).pimsmUpStrmSGRptAction
      (pimsmCb, pSGRptNode,pUpStrmSGRptEventInfo)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_NORMAL, "UpStrmSGRptAction Failed ");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR,"UpStrmSGRptAction Failed. State = %s, \n Event = %s",
      pimsmUpStrmSGRptStateName[upStrmSGRptState],
      pimsmUpStrmSGRptEventName[upStrmSGRptEvent]);
    pimsmSGRptNodeTryRemove(pimsmCb, pSGRptNode);
    return L7_FAILURE;
  }
  pSGRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamPruneState =
  (pimsmUpStrmSGRpt[upStrmSGRptEvent][upStrmSGRptState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO, "Next State = %s \n ",
              pimsmUpStrmSGRptStateName[pSGRptEntry->pimsmSGRptUpstreamPruneState]);

  pimsmSGRptNodeTryRemove(pimsmCb, pSGRptNode);
  return rc;
}

/******************************************************************************
* @purpose   Send upstream (S,G,Rpt) join
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGRptJoinSend(pimsmCB_t * pimsmCb,
                                        pimsmSGRptNode_t *  pSGRptNode,
                                         pimsmUpStrmSGRptEventInfo_t
                                         * pUpStrmSGRptEventInfo)
{
  pimsmSGRptEntry_t  *pSGRptEntry;
  L7_uchar8 maskLen =0;
  pimsmSendJoinPruneData_t jpData;
  L7_uint32  srcRpfIface = 0;

  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;

  if ((mcastRPFInterfaceGet(&pSGRptEntry->pimsmSGRptSrcAddress,
                           &srcRpfIface) == L7_SUCCESS) &&
      (mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family, srcRpfIface) != L7_TRUE))
  {
    if (mcastIpMapIsDirectlyConnected(&pSGRptEntry->pimsmSGRptSrcAddress,
                                      L7_NULLPTR) == L7_TRUE)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO,
                  "Upstream is a local address");
      return L7_SUCCESS;
    }
  }

  /*send a Join(S,G,rpt) to RPF'(S,G,rpt).*/
  /*Upstream (S,G,Rpt) PruneDesired(S,G,rpt)->FALSE*/
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = pSGRptEntry->pimsmSGRptRPFIfIndex;
  jpData.pNbrAddr = &pSGRptEntry->pimsmSGRptUpstreamNeighbor;
  if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum, &jpData.holdtime)
                                             != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,
      PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed for rtrIfNum (%d)",
            jpData.rtrIfNum);
    return L7_FAILURE;
  }
  jpData.addrFlags =  PIMSM_ADDR_RP_BIT;
  jpData.pSrcAddr = &pSGRptEntry->pimsmSGRptSrcAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pSGRptEntry->pimsmSGRptGrpAddress;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag  = L7_TRUE;

  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Send upstream (S,G,Rpt) join snd Cancel override timer
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGRptSendPruneOverrideTimerCancel(pimsmCB_t * pimsmCb,
                                                  pimsmSGRptNode_t *  pSGRptNode,
                                                  pimsmUpStrmSGRptEventInfo_t
                                                  * pUpStrmSGRptEventInfo)
{
  pimsmSGRptEntry_t  *  pSGRptEntry;
  L7_uchar8 maskLen =0;
  pimsmSendJoinPruneData_t jpData;
  L7_uint32  srcRpfIface = 0;

  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;

  if ((mcastRPFInterfaceGet(&pSGRptEntry->pimsmSGRptSrcAddress,
                           &srcRpfIface) == L7_SUCCESS) &&
      ((mcastIpMapIsRtrIntfUnnumbered(pimsmCb->family, srcRpfIface) == L7_TRUE) ||
       (mcastIpMapIsDirectlyConnected(&pSGRptEntry->pimsmSGRptSrcAddress,
                                      L7_NULLPTR) != L7_TRUE)))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO,
                "Upstream is a not local address");
    /*Send Prune (S,G,rpt) to RPF'(S,G,rpt)*/
    memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
    jpData.rtrIfNum = pSGRptEntry->pimsmSGRptRPFIfIndex;
    jpData.pNbrAddr = &pSGRptEntry->pimsmSGRptUpstreamNeighbor;
    if (pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum ,
                                           &jpData.holdtime)
        != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,
                  PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed for rtrIfNum (%d)",
                  jpData.rtrIfNum);
      return L7_FAILURE;
    }
    jpData.addrFlags =  PIMSM_ADDR_RP_BIT;
    jpData.pSrcAddr = &pSGRptEntry->pimsmSGRptSrcAddress;
    pimSrcMaskLenGet(pimsmCb->family,&maskLen);
    jpData.srcAddrMaskLen = maskLen;
    jpData.pGrpAddr = &pSGRptEntry->pimsmSGRptGrpAddress;
    pimGrpMaskLenGet(pimsmCb->family,&maskLen);
    jpData.grpAddrMaskLen = maskLen;
    jpData.joinOrPruneFlag  = L7_FALSE;

    if (pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR,"Failed to send Join/Prune Message");
    }
  }
  /* If the router was previously in NotPruned state, then the action is
     to send a Prune(S,G,rpt) to RPF'(S,G,rpt), and to cancel the
     Override Timer   */

  /* Cancel Override Timer */
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptEntry->pimsmSGRptUpstreamOverrideTimer));
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Cancel upstream (s,G,Rpt) override timer
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGRptOverrideTimerCancel(pimsmCB_t * pimsmCb,
                                        pimsmSGRptNode_t *  pSGRptNode,
                                        pimsmUpStrmSGRptEventInfo_t
                                        * pUpStrmSGRptEventInfo)
{
  pimsmSGRptEntry_t  *pSGRptEntry;

  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;

  /* Cancel Override Timer */
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGRptEntry->pimsmSGRptUpstreamOverrideTimer));
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Send upstream (S,G,Rpt) join and Leave  override timer
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGRptJoinSendOverrideTimerLeave(pimsmCB_t * pimsmCb,
                                                pimsmSGRptNode_t *  pSGRptNode,
                                                pimsmUpStrmSGRptEventInfo_t
                                                * pUpStrmSGRptEventInfo)
{
  pimsmSGRptEntry_t  *pSGRptEntry;
  pimsmStarGNode_t   *pStarGNode;
  L7_inet_addr_t    *sgrptNbrAddr, *starGNbrAddr;
  L7_uchar8 maskLen;
  pimsmSendJoinPruneData_t jpData;
  L7_RC_t rc;

  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;

  /* When the Override Timer expires, we must send a Join(S,G,rpt) to
     RPF'(S,G,rpt) to override the Prune message that caused the timer
     to be running.  We only send this if RPF'(S,G,rpt) equals RPF'(*,G)*/

  /* Send Join (S,G,Rpt) to RPF'(S,G,rpt) */

  rc = pimsmStarGFind(pimsmCb, &pSGRptEntry->pimsmSGRptGrpAddress,
                 &pStarGNode);
  if(rc  != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR,
         "Failed to find (*,G) entry");
      return L7_FAILURE;
  }
  starGNbrAddr = &pStarGNode->pimsmStarGEntry.pimsmStarGUpstreamNeighbor;
  sgrptNbrAddr = &pSGRptEntry->pimsmSGRptUpstreamNeighbor;
  if(L7_INET_ADDR_COMPARE(sgrptNbrAddr,starGNbrAddr) !=L7_NULL)
  {
    memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
    jpData.rtrIfNum = pSGRptEntry->pimsmSGRptRPFIfIndex;
    if(pimsmInterfaceJoinPruneHoldtimeGet(pimsmCb, jpData.rtrIfNum, &jpData.holdtime)
                                     != L7_SUCCESS)
    {
       PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,
         PIMSM_TRACE_ERROR,"pimsmInterfaceJoinPruneHoldtimeGet failed for rtrIfNum (%d)",
            jpData.rtrIfNum);
      return L7_FAILURE;
    }

    jpData.addrFlags = PIMSM_ADDR_RP_BIT ;
    jpData.pGrpAddr = &pSGRptEntry->pimsmSGRptGrpAddress;
    pimGrpMaskLenGet(pimsmCb->family,&maskLen);
    jpData.grpAddrMaskLen = maskLen;
    jpData.pSrcAddr = &pSGRptEntry->pimsmSGRptSrcAddress;
    pimSrcMaskLenGet(pimsmCb->family,&maskLen);
    jpData.srcAddrMaskLen = maskLen;
    jpData.joinOrPruneFlag = L7_TRUE;
    jpData.pNbrAddr = sgrptNbrAddr;
    if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR,"Failed to send sending Join/Prune Message");
    }
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Set override timer
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmUpStrmSGRptOverrideTimerSet(pimsmCB_t * pimsmCb,
                                     pimsmSGRptNode_t *  pSGRptNode,
                                     pimsmUpStrmSGRptEventInfo_t
                                     * pUpStrmSGRptEventInfo)
{
  pimsmSGRptEntry_t  *pSGRptEntry;
  L7_uint32           overrideInterval, timerVal;
  L7_uint32           timeout;
  pimsmTimerData_t   *pTimerData;

  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  pSGRptEntry = &pSGRptNode->pimsmSGRptEntry;

  pimsmEffectiveOverrideInterval(pimsmCb, pSGRptEntry->pimsmSGRptRPFIfIndex,
                                 &overrideInterval);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_INFO, "overrideInterval : %d ", overrideInterval);
  /* OT = min(OT, t_override); */

  if(pSGRptEntry->pimsmSGRptUpstreamOverrideTimer == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR, "(*,G) upstrm join timer is not started");
    return L7_FAILURE;
  }
  appTimerTimeLeftGet(pimsmCb->timerCb,
                      pSGRptEntry->pimsmSGRptUpstreamOverrideTimer, &timerVal);
  timeout = min(timerVal, overrideInterval);

  pTimerData = &pSGRptEntry->pimsmSGRptUpstreamOverrideTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1,
        &pSGRptNode->pimsmSGRptEntry.pimsmSGRptSrcAddress);
  inetCopy(&pTimerData->addr2,
        &pSGRptNode->pimsmSGRptEntry.pimsmSGRptGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmUpStrmSGRptOverrideTimerExpiresHandler,
                            (void*)pSGRptEntry->pimsmSGRptUpstreamOverrideTimerHandle,
                            timeout,
                            &(pSGRptEntry->pimsmSGRptUpstreamOverrideTimer),
                            "SGr-OT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_ERROR, "Failed to add (S,G,Rpt) upstrm Override timer");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose    Change the state and return
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmUpStrmSGRptDoNothing(pimsmCB_t * pimsmCb,
                                        pimsmSGRptNode_t *  pSGRptNode,
                                        pimsmUpStrmSGRptEventInfo_t
                                        * pUpStrmSGRptEventInfo)
{
  MCAST_UNUSED_PARAM(pSGRptNode);
  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  return(L7_SUCCESS); /* Change to new state */
}
/******************************************************************************
* @purpose   Unexpected event, return Error
*
* @param    pimsmCb                @b{(input)} PIMSM Control Block
* @param    pSGNode                @b{(input)} (S,G,Rpt) node
* @param    pUpStrmSGRptEventInfo  @b{(input)} Upstream SG Rpt FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmUpStrmSGRptErrorReturn(pimsmCB_t * pimsmCb,
                                          pimsmSGRptNode_t *  pSGRptNode,
                                          pimsmUpStrmSGRptEventInfo_t
                                          * pUpStrmSGRptEventInfo)
{
  MCAST_UNUSED_PARAM(pSGRptNode);
  MCAST_UNUSED_PARAM(pUpStrmSGRptEventInfo);
  return(L7_FAILURE);
}
/******************************************************************************
* @purpose  Action Routine when (S,G) Upstream Override Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmUpStrmSGRptOverrideTimerExpiresHandler(void *pParam)
{
  pimsmUpStrmSGRptEventInfo_t upStrmSGRptEventInfo;
  pimsmSGRptNode_t *pSGRptNode;
  pimsmCB_t * pimsmCb;
  L7_int32    handle = (L7_int32)pParam;
  pimsmTimerData_t *pTimerData;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;
  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  pimsmCb = pTimerData->pimsmCb;
  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G_RPT, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);
  if (pimsmSGRptFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGRptNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_NORMAL,
        "(S,G,rpt) Find failed");
    return;
  }
  if (pSGRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamOverrideTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmSGRptUpstreamOverrideTimer is NULL, But Still Expired");
    return;
  }
  pSGRptNode->pimsmSGRptEntry.pimsmSGRptUpstreamOverrideTimer = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G_RPT,PIMSM_TRACE_NORMAL,"(S,G,Rpt) Upstream Override Timer Expired ");
  memset(&upStrmSGRptEventInfo, 0, sizeof(pimsmUpStrmSGRptEventInfo_t));

  upStrmSGRptEventInfo.eventType =
  PIMSM_UPSTRM_S_G_RPT_SM_EVENT_OVERRIDE_TIMER_EXPIRES;

  pimsmUpStrmSGRptExecute(pimsmCb, pSGRptNode, &upStrmSGRptEventInfo);
}


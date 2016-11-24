/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmdnstrmsgfsm.c
*
* @purpose Contains PIM-SM Down stream per interface State Machine 
* definitions for (S,G).
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
#include "pimsmdefs.h"
#include "pimsmmacros.h"
#include "pimsmdnstrmsgfsm.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"
#include "mcast_wrap.h"

static L7_RC_t pimsmDnStrmPerIntfSGExpireTimerStart
(pimsmCB_t * pimsmCb, 
 pimsmSGNode_t * pimsmSGNode, 
 pimsmDnStrmPerIntfSGEventInfo_t * pDnStrmPerIntfSGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGExpireTimerRestart
(pimsmCB_t * pimsmCb, 
 pimsmSGNode_t * pimsmSGNode,
 pimsmDnStrmPerIntfSGEventInfo_t * pDnStrmPerIntfSGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGPrunePendingTimerStart
(pimsmCB_t * pimsmCb, 
 pimsmSGNode_t * pimsmSGNode,
 pimsmDnStrmPerIntfSGEventInfo_t * pDnStrmPerIntfSGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGPruneEchoSend
(pimsmCB_t * pimsmCb, 
 pimsmSGNode_t * pimsmSGNode,
 pimsmDnStrmPerIntfSGEventInfo_t * pDnStrmPerIntfSGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfDoNothing
(pimsmCB_t * pimsmCb, 
 pimsmSGNode_t * pimsmSGNode,
 pimsmDnStrmPerIntfSGEventInfo_t * pDnStrmPerIntfSGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfSGErrorReturn
(pimsmCB_t * pimsmCb, 
 pimsmSGNode_t * pimsmSGNode,
 pimsmDnStrmPerIntfSGEventInfo_t * pDnStrmPerIntfSGEventInfo);

typedef struct pimsmDnStrmPerIntfSGSM_s 
{
  pimsmDnStrmPerIntfSGStates_t nextState;
  L7_RC_t (*pimsmDnStrmPerIntfSGAction)(pimsmCB_t * pimsmCb, 
                                        struct pimsmSGNode_s * pimsmSGNode, 
                                        pimsmDnStrmPerIntfSGEventInfo_t 
                                        * pDnStrmPerIntfSGEventInfo);
} pimsmDnStrmPerIntfSGSM_t;

static pimsmDnStrmPerIntfSGSM_t pimsmDnStrmPerIntfSG
[PIMSM_DNSTRM_S_G_SM_EVENT_MAX]
[PIMSM_DNSTRM_S_G_SM_STATE_MAX] =
{
  {/*RECV_JOIN*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfSGExpireTimerStart},
    /*JOIN*/  {PIMSM_DNSTRM_S_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfSGExpireTimerRestart},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfSGExpireTimerRestart}
  },
  {/*RECV_PRUNE*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfDoNothing},
    /*JOIN*/  {PIMSM_DNSTRM_S_G_SM_STATE_PRUNE_PENDING,   
      pimsmDnStrmPerIntfSGPrunePendingTimerStart},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_SM_STATE_PRUNE_PENDING,
      pimsmDnStrmPerIntfDoNothing}
  },
  {/*PENDING_TIMER_EXPIRES*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfSGErrorReturn},
    /*JOIN*/  {PIMSM_DNSTRM_S_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfSGErrorReturn},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO,
      pimsmDnStrmPerIntfSGPruneEchoSend}
  },
  {/*EXPIRY_TIMER_EXPIRES*/
    /*NO_INFO*/ {PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfSGErrorReturn},
    /*JOIN*/  {PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfDoNothing},
    /*PRUNE_PENDING*/ {PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfDoNothing}
  }
};

static void pimsmSGIPrunePendingTimerExpiresHandler(void *pParam);
static void pimsmSGIJoinTimerExpiresHandler(void *pParam);
/******************************************************************************
* @purpose   execute the DnStream per (S,G) FSM and process the events received
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event 
*                                                   related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
*
******************************************************************************/
L7_RC_t pimsmDnStrmPerIntfSGExecute(pimsmCB_t           *pimsmCb,
                                    pimsmSGNode_t       *pSGNode, 
                                    pimsmDnStrmPerIntfSGEventInfo_t 
                                    * pDnStrmPerIntfSGEventInfo)
{
  L7_uint32                     rtrIfNum;
  pimsmSGIEntry_t              *pSGIEntry;
  pimsmDnStrmPerIntfSGStates_t  dnStrmPerIntfSGState;
  pimsmDnStrmPerIntfSGEvents_t  dnStrmPerIntfSGEvent;
  L7_RC_t   rc = L7_SUCCESS;

  if((pSGNode == (pimsmSGNode_t *)L7_NULLPTR)||
     (pDnStrmPerIntfSGEventInfo == (pimsmDnStrmPerIntfSGEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "invalid input parameters");            
    return L7_FAILURE;
  }
  if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    return L7_FAILURE;
  }  
  rtrIfNum = pDnStrmPerIntfSGEventInfo->rtrIfNum;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(S,G,I) entry is not present for rtrIfNum = %d",
                                 rtrIfNum);              
    return L7_FAILURE;
  }
  dnStrmPerIntfSGEvent = pDnStrmPerIntfSGEventInfo->eventType;
  dnStrmPerIntfSGState = pSGIEntry->pimsmSGIJoinPruneState;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "\nState = %s, \n Event = %s",
              pimsmDnStrmPerIntfSGStateName[dnStrmPerIntfSGState], 
              pimsmDnStrmPerIntfSGEventName[dnStrmPerIntfSGEvent]);
  if(((pimsmDnStrmPerIntfSG[dnStrmPerIntfSGEvent][dnStrmPerIntfSGState]).pimsmDnStrmPerIntfSGAction
      (pimsmCb, pSGNode,pDnStrmPerIntfSGEventInfo)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, "DnStrmPerIntfSGAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"DnStrmPerIntfSGAction Failed. State = %s, \n Event = %s",
      pimsmDnStrmPerIntfSGStateName[dnStrmPerIntfSGState], 
      pimsmDnStrmPerIntfSGEventName[dnStrmPerIntfSGEvent]);
    pimsmSGNodeTryRemove(pimsmCb, pSGNode);
    return L7_FAILURE;
  }

  pSGIEntry->pimsmSGIJoinPruneState = 
  (pimsmDnStrmPerIntfSG[dnStrmPerIntfSGEvent][dnStrmPerIntfSGState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "Next State = %s \n ", 
              pimsmDnStrmPerIntfSGStateName[pSGIEntry->pimsmSGIJoinPruneState]);
  pimsmSGNodeTryRemove(pimsmCb, pSGNode);
  return rc;
}
/******************************************************************************
* @purpose  Start Expiry timer of (S,G) entry supplied   
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event
*                                                   related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmDnStrmPerIntfSGExpireTimerStart(pimsmCB_t     *pimsmCb,
                                        pimsmSGNode_t *pSGNode,
                                        pimsmDnStrmPerIntfSGEventInfo_t
                                        * pDnStrmPerIntfSGEventInfo)
{
  L7_uint32            holdtime;
  L7_uint32            rtrIfNum;
  pimsmSGIEntry_t     *pSGIEntry;
  pimsmTimerData_t *   pTimerData = L7_NULLPTR;

  rtrIfNum = pDnStrmPerIntfSGEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);               
    return L7_FAILURE;
  }

  /* Start Expiry Timer; 
   * set to the HoldTime from the triggering  Join/Prune message 
   */
  holdtime = (L7_uint32) pDnStrmPerIntfSGEventInfo->holdtime;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "(S,G) dnstrm join expire timer holdtime =%d", 
                                 holdtime);
  pTimerData = &pSGIEntry->pimsmSGIJoinExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;  
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGIJoinTimerExpiresHandler,
                            (void*)pSGIEntry->pimsmSGIJoinExpiryTimerHandle,
                            holdtime,
                            &(pSGIEntry->pimsmSGIJoinExpiryTimer),
                            "SGI-JT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
                "Failed to add (S,G,%d) Join Expire timer", rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);  
}
/******************************************************************************
* @purpose  Re start the expiry timer of the (S,G) entry   
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event
*                                                   related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmDnStrmPerIntfSGExpireTimerRestart(pimsmCB_t    *pimsmCb,
                                                      pimsmSGNode_t *pSGNode,
                                                      pimsmDnStrmPerIntfSGEventInfo_t
                                                      * pDnStrmPerIntfSGEventInfo)
{
  L7_uint32            holdtime;
  L7_uint32            timeLeft = 0;
  L7_uint32            rtrIfNum;
  pimsmSGIEntry_t     *pSGIEntry;
  pimsmTimerData_t    *pTimerData = L7_NULLPTR;

  rtrIfNum = pDnStrmPerIntfSGEventInfo->rtrIfNum;
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);
    return L7_FAILURE;
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pSGIEntry->pimsmSGIPrunePendingTimer));

  /* Restart Expiry Timer; 
   * set to maximum of its current value and the HoldTime from the
   * triggering Join/Prune message.
   */
  holdtime = (L7_uint32) pDnStrmPerIntfSGEventInfo->holdtime;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "(S,G) dnstrm Expire timer holdtime = %d", holdtime);
  pTimerData = &pSGIEntry->pimsmSGIJoinExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);

  if (appTimerTimeLeftGet (pimsmCb->timerCb, pSGIEntry->pimsmSGIJoinExpiryTimer,
                           &timeLeft) == L7_SUCCESS)
  {
    holdtime = max (timeLeft, holdtime);
  }

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGIJoinTimerExpiresHandler,
                            (void*)pSGIEntry->pimsmSGIJoinExpiryTimerHandle,
                            holdtime,
                            &(pSGIEntry->pimsmSGIJoinExpiryTimer),
                            "SGI-JT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
                "Failed to add (S,G,%d) Join Expire timer", rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Start Prune Pending Timer   
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event
*                                                   related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmDnStrmPerIntfSGPrunePendingTimerStart(pimsmCB_t     *pimsmCb,
                                                 pimsmSGNode_t *pSGNode, 
                                                 pimsmDnStrmPerIntfSGEventInfo_t 
                                                 * pdnStrmPerIntfSGEventInfo)

{
  L7_uint32           rtrIfNum, timeOut = 0;
  pimsmSGIEntry_t    *pSGIEntry;
  pimsmTimerData_t   *pTimerData = L7_NULLPTR;
  pimsmInterfaceEntry_t *pIntfEntry  = L7_NULLPTR;


  rtrIfNum = pdnStrmPerIntfSGEventInfo->rtrIfNum;
  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum);  
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);  ;            
    return L7_FAILURE;
  }
  /* Start Prune-Pending Timer; 
   * The Prune-Pending Timer is started; it is set to the 
   * J/P_Override_Interval(I) if the router has more than one 
   * neighbor on that interface; otherwise it is set to zero.
     */
  if(pIntfEntry->pimsmNbrCount > 1)
  {
   timeOut = pimsmJoinPruneOverrideIntervalGet(pimsmCb,rtrIfNum);
  }
  pTimerData = &pSGIEntry->pimsmSGIPrunePendingTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  pTimerData->rtrIfNum = rtrIfNum;
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGIPrunePendingTimerExpiresHandler,
                            (void*)pSGIEntry->pimsmSGIPrunePendingTimerHandle,
                            timeOut,
                            &(pSGIEntry->pimsmSGIPrunePendingTimer),
                            "SGI-PPT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, 
                "Failed to add (S,G,%d) prune pending timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose  Send (S,G) Prune Echo message   
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event
*                                                   related data 
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t  pimsmDnStrmPerIntfSGPruneEchoSend(pimsmCB_t        *pimsmCb,
                                      pimsmSGNode_t    *pSGNode,
                                      pimsmDnStrmPerIntfSGEventInfo_t 
                                      * pDnStrmPerIntfSGEvnetInfo)
{
  pimsmSGIEntry_t         *pSGIEntry;
  pimsmSGEntry_t          *pSGEntry;
  L7_uint32                rtrIfNum;
  pimsmInterfaceEntry_t   *pIntfEntry = L7_NULLPTR;
  pimsmSendJoinPruneData_t jpData;
  L7_uchar8                maskLen =0;     

  rtrIfNum = pDnStrmPerIntfSGEvnetInfo->rtrIfNum;
  if(pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
     !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum);   
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if(pSGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(S,G,%d) entry is not present",rtrIfNum);                
    return L7_FAILURE;
  }

  /* A PruneEcho(S,G) need not be sent on an interface
   * that contains only a single PIM neighbor 
   */
  if(pIntfEntry->pimsmNbrCount <= 1)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, "nbrCount = %d on rtrIfNum = %d",
                pIntfEntry->pimsmNbrCount, rtrIfNum);   
    return(L7_SUCCESS);
  }

  pSGEntry = &pSGNode->pimsmSGEntry;
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));
  jpData.rtrIfNum = rtrIfNum;

  /* put router's  own address in  the 
   * Upstream Neighbor Address field
   */
  jpData.holdtime = pIntfEntry->pimsmInterfaceJoinPruneHoldtime;
  jpData.pNbrAddr = &pIntfEntry->pimsmInterfaceAddr;
  jpData.addrFlags = 0;
  jpData.pSrcAddr = &pSGEntry->pimsmSGSrcAddress;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  jpData.pGrpAddr = &pSGEntry->pimsmSGGrpAddress;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.joinOrPruneFlag = L7_FALSE;
  jpData.pruneEchoFlag = L7_TRUE;

  /* Send Prune Echo (S,G); */
  if(pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NOTICE,
              "Failed to send Prune Echo Message on rtrIfNum = %d",rtrIfNum);
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Change the state and return   
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event
*                                                   related data 
*
* @returns   L7_SUCCESS
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfDoNothing(pimsmCB_t     *pimsmCb,
                                           pimsmSGNode_t *pSGNode,
                                           pimsmDnStrmPerIntfSGEventInfo_t 
                                           * pDnStrmPerIntfSGEventInfo)
{
  MCAST_UNUSED_PARAM(pSGNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfSGEventInfo);
  return(L7_SUCCESS); /* Change to new state */
}
/******************************************************************************
* @purpose  Unexpected event, return Error   
*
* @param    pimsmCb                     @b{(input)} PIMSM Control Block
* @param    pSGNode                     @b{(input)} (S,G) node
* @param    pDnStrmPerIntfSGEventInfo   @b{(input)} Downstream SG FSM event 
*                                                   related data 
*
* @returns   L7_FAILURE
*
* @comments     
*      
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfSGErrorReturn(pimsmCB_t     *pimsmCb,
                                               pimsmSGNode_t *pSGNode,
                                               pimsmDnStrmPerIntfSGEventInfo_t 
                                               * dnStrmPerIntfSGEventInfo)
{
  MCAST_UNUSED_PARAM(pSGNode);
  MCAST_UNUSED_PARAM(dnStrmPerIntfSGEventInfo);
  return(L7_FAILURE); /* Change to new state */
}

/******************************************************************************
* @purpose  Action Routine when (S,G) Prune Pending Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments     
*      
* @end
******************************************************************************/
static void pimsmSGIPrunePendingTimerExpiresHandler(void *pParam)
{
  pimsmCB_t               *pimsmCb;
  pimsmTimerData_t        *pTimerData;
  L7_int32                 handle = (L7_int32)pParam;  
  pimsmSGNode_t           *pSGNode;
  pimsmSGIEntry_t         *pSGIEntry = L7_NULLPTR;
  L7_uint32                rtrIfNum;                               
  L7_BOOL                  joinDesired;  
  pimsmDnStrmPerIntfSGEventInfo_t dnStrmPerIntfSGEventInfo;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_BOOL couldAssert;
  pimsmPerIntfSGAssertEventInfo_t  sgAssertEventInfo;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;
  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;
  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, 
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);    
  if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL,
        "(S,G) Find failed");
    return;
  } 
  pSGIEntry =  pSGNode->pimsmSGIEntry[rtrIfNum];
  if (pSGIEntry->pimsmSGIPrunePendingTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM Downstream pimsmSGIPrunePendingTimer is NULL, But Still Expired");
    return;
  }
  pSGIEntry->pimsmSGIPrunePendingTimer = L7_NULLPTR;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL,
               "(S,G,I) Prune Pending Timer Expired for rtrIfNum = %d",rtrIfNum);

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, " Source Address :",
                    &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, "Group Address :", 
                    &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, "RtrIfNum: %d",rtrIfNum); 
  memset(&dnStrmPerIntfSGEventInfo, 0, sizeof(pimsmDnStrmPerIntfSGEventInfo_t));
  dnStrmPerIntfSGEventInfo.eventType = 
  PIMSM_DNSTRM_S_G_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES;
  dnStrmPerIntfSGEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfSGExecute(pimsmCb, pSGNode, &dnStrmPerIntfSGEventInfo);
  memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
  joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmSGEventInfo.eventType 
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmSGEventInfo.eventType 
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);

  memset(&sgAssertEventInfo, 0, sizeof(pimsmPerIntfSGAssertEventInfo_t));
  sgAssertEventInfo.rtrIfNum = rtrIfNum;
  couldAssert = pimsmSGICouldAssert(pimsmCb, pSGNode, rtrIfNum);
  if(couldAssert == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
       "(couldAssert == false)");
    sgAssertEventInfo.eventType = 
    PIMSM_ASSERT_S_G_SM_EVENT_COULD_ASSERT_FALSE;
    pimsmPerIntfSGAssertExecute(pimsmCb, pSGNode,&sgAssertEventInfo);
  } 
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "(couldAssert == True)");
  }   
  if(pimsmSGMFCUpdate (pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR, "MFC Updation failed");
    return;
  }
}

/******************************************************************************
* @purpose   Action Routine SG Join Expiry Timer expires   
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments     
*      
* @end
******************************************************************************/
static void pimsmSGIJoinTimerExpiresHandler(void *pParam)
{
  pimsmTimerData_t   *pTimerData;
  pimsmCB_t          *pimsmCb;
  L7_int32            handle = (L7_int32)pParam;  
  pimsmSGNode_t      *pSGNode;
  pimsmSGIEntry_t    *pSGIEntry = L7_NULLPTR;
  L7_uint32           rtrIfNum;                               
  L7_BOOL             joinDesired;  
  pimsmDnStrmPerIntfSGEventInfo_t dnStrmPerIntfSGEventInfo;
  pimsmUpStrmSGEventInfo_t upStrmSGEventInfo;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);

  if(pTimerData == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,"Timer Data is NULL");
    return;
  }
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;

  pSrcAddr = &pTimerData->addr1;
  pGrpAddr = &pTimerData->addr2;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO, 
      "Source Address :", pSrcAddr);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);    
  if (pimsmSGFind(pimsmCb, pGrpAddr, pSrcAddr, &pSGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_NORMAL,
        "(S,G) Find failed");
    return;
  }   
  pSGIEntry = pSGNode->pimsmSGIEntry[rtrIfNum];
  if (pSGIEntry->pimsmSGIJoinExpiryTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM Downstream pimsmSGIJoinExpiryTimer is NULL, But Still Expired");
    return;
  }
  pSGIEntry->pimsmSGIJoinExpiryTimer = L7_NULLPTR;  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_NORMAL, 
               "(S,G,I) Join expire Timer Expired for rtrIfNum = %d",rtrIfNum); 
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, "Source Address :", 
                    &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, "Group Address :", 
                    &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_DEBUG, "RtrIfNum: %d",rtrIfNum); 
  memset(&dnStrmPerIntfSGEventInfo, 0, sizeof(pimsmDnStrmPerIntfSGEventInfo_t));
  dnStrmPerIntfSGEventInfo.eventType = 
  PIMSM_DNSTRM_S_G_SM_EVENT_EXPIRY_TIMER_EXPIRES;
  dnStrmPerIntfSGEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfSGExecute(pimsmCb, pSGNode, &dnStrmPerIntfSGEventInfo);
  memset(&upStrmSGEventInfo,0,sizeof(pimsmUpStrmSGEventInfo_t));
  joinDesired = pimsmSGJoinDesired(pimsmCb, pSGNode);
  if(joinDesired == L7_TRUE)
  {
    upStrmSGEventInfo.eventType 
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    upStrmSGEventInfo.eventType 
    = PIMSM_UPSTRM_S_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmSGExecute(pimsmCb, pSGNode, &upStrmSGEventInfo);
  if(pimsmSGMFCUpdate(pimsmCb,pSGNode, MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G, PIMSM_TRACE_ERROR,"MFC Updation failed");
    return;
  }  
}



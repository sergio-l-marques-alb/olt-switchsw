/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename pimsmdnstrmstargfsm.c
*
* @purpose Contains PIM-SM Down stream per interface State Machine 
* implementation for (*,G) entry.
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
#include "pimsmdnstrmstargfsm.h"
#include "pimsmtimer.h"
#include "pimsmwrap.h"

static L7_RC_t pimsmDnStrmPerIntfStarGExpireTimerStart 
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfStarGExpireTimerRestart 
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfStarGPrunePendingTimerStart 
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfStarGPruneEchoSend 
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfStarGDoNothing
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);
static L7_RC_t pimsmDnStrmPerIntfStarGErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);

typedef struct pimsmDnStrmPerIntfStarGSM_s
{
  pimsmDnStrmPerIntfStarGStates_t nextState;
  L7_RC_t (*pimsmDnStrmPerIntfStarGAction) 
  (pimsmCB_t * pimsmCb,
   struct pimsmStarGNode_s * pStarGNode, 
   pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo);
} pimsmDnStrmPerIntfStarGSM_t;

static pimsmDnStrmPerIntfStarGSM_t pimsmDnStrmPerIntfStarG
[PIMSM_DNSTRM_STAR_G_SM_EVENT_MAX]
[PIMSM_DNSTRM_STAR_G_SM_STATE_MAX] = 
{
  {/* EVENT_RECV_JOIN */
    /* NO_INFO */ {PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfStarGExpireTimerStart},
    /* JOIN */  {PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfStarGExpireTimerRestart},
    /* PRUNE_PENDING */ {PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfStarGExpireTimerRestart}
  },
  {/* EVENT_RECV_PRUNE */
    /* NO_INFO */ {PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfStarGDoNothing},
    /* JOIN */  {PIMSM_DNSTRM_STAR_G_SM_STATE_PRUNE_PENDING, 
      pimsmDnStrmPerIntfStarGPrunePendingTimerStart},
    /* PRUNE_PENDING */ {PIMSM_DNSTRM_STAR_G_SM_STATE_PRUNE_PENDING, 
      pimsmDnStrmPerIntfStarGDoNothing}
  },
  {/* EVENT_PRUNE_PENDING_TIMER_EXPIRES */
    /* NO_INFO */ {PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfStarGErrorReturn},
    /* JOIN */  {PIMSM_DNSTRM_STAR_G_SM_STATE_JOIN, 
      pimsmDnStrmPerIntfStarGErrorReturn},
    /* PRUNE_PENDING */ {PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfStarGPruneEchoSend}
  },
  {/* EVENT_EXPIRY_TIMER_EXPIRES*/
    /* NO_INFO */ {PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfStarGErrorReturn},
    /* JOIN */  {PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfStarGDoNothing},
    /* PRUNE_PENDING */ {PIMSM_DNSTRM_STAR_G_SM_STATE_NO_INFO, 
      pimsmDnStrmPerIntfStarGDoNothing}
  }
};

static void pimsmStarGIJoinExpiryTimerExpiresHandler(void *pParam);
static void pimsmStarGIPrunePendingTimerExpiresHandler(void *pParam);
/******************************************************************************
*
* @purpose  execute the Down Stream per (*,G) FSM and process the events received
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM 
*                                                    event related data 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
*
******************************************************************************/
L7_RC_t pimsmDnStrmPerIntfStarGExecute 
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo)
{
  L7_uint32             rtrIfNum;
  pimsmStarGIEntry_t   *pStarGIEntry;
  pimsmDnStrmPerIntfStarGStates_t  dnStrmPerIntfStarGState;
  pimsmDnStrmPerIntfStarGEvents_t dnStrmPerIntfStarGEvent;
  L7_RC_t rc = L7_SUCCESS;            

  if ((pStarGNode == (pimsmStarGNode_t *)L7_NULLPTR)||
      (pDnStrmPerIntfStarGEventInfo == (pimsmDnStrmPerIntfStarGEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "invalid input parameters");            
    return L7_FAILURE;
  }
  if((pStarGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_ERROR, 
        "Node is deleted");            
    return L7_FAILURE;
  }   
  rtrIfNum = pDnStrmPerIntfStarGEventInfo->rtrIfNum;
  if ( rtrIfNum >=MCAST_MAX_INTERFACES)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
        "wrong  rtrIfNum = %d", rtrIfNum);
     return L7_FAILURE;   
  }
  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "rtrIfNum =%d", rtrIfNum);
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum]; 
  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G,I) entry is not present for rtrIfNum = %d",
                rtrIfNum);              
    return L7_FAILURE;
  }
  dnStrmPerIntfStarGState = pStarGIEntry->pimsmStarGIJoinPruneState;
  dnStrmPerIntfStarGEvent = pDnStrmPerIntfStarGEventInfo->eventType;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "State = %s, \n Event = %s ",
              pimsmDnStrmPerIntfStarGStateName[dnStrmPerIntfStarGState], 
              pimsmDnStrmPerIntfStarGEventName[dnStrmPerIntfStarGEvent]);

  if (((pimsmDnStrmPerIntfStarG[dnStrmPerIntfStarGEvent][dnStrmPerIntfStarGState]).
       pimsmDnStrmPerIntfStarGAction(pimsmCb, 
                                     pStarGNode,pDnStrmPerIntfStarGEventInfo))
      != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, 
                "DownStrmPerIntfStarGAction Failed ");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"DownStrmPerIntfStarGAction Failed.  State = %s, \n Event = %s",
       pimsmDnStrmPerIntfStarGStateName[dnStrmPerIntfStarGState], 
       pimsmDnStrmPerIntfStarGEventName[dnStrmPerIntfStarGEvent]);
    pimsmStarGNodeTryRemove(pimsmCb, pStarGNode); 
    return  L7_FAILURE;
  }
  pStarGIEntry->pimsmStarGIJoinPruneState = 
  (pimsmDnStrmPerIntfStarG[dnStrmPerIntfStarGEvent][dnStrmPerIntfStarGState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "Next State = %s \n ", 
              pimsmDnStrmPerIntfStarGStateName[pStarGIEntry->pimsmStarGIJoinPruneState]);

  pimsmStarGNodeTryRemove(pimsmCb, pStarGNode); 
  return(rc);
}
/******************************************************************************
* @purpose  Start Expiry timer of (*,G) entry   
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM  
*                                                    event related data 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfStarGExpireTimerStart(
                                                      pimsmCB_t * pimsmCb,
                                                      pimsmStarGNode_t * pStarGNode, 
                                                      pimsmDnStrmPerIntfStarGEventInfo_t 
                                                      * pDnStrmPerIntfStarGEventInfo)
{
  L7_uint32           holdtime;
  L7_uint32           rtrIfNum;
  pimsmStarGIEntry_t *pStarGIEntry;
  pimsmTimerData_t   *pTimerData = L7_NULLPTR;

  rtrIfNum = pDnStrmPerIntfStarGEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "rtrIfNum =%d ", rtrIfNum);
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);        
    return L7_FAILURE;
  }

  /* Start Expiry Timer; */
  /* set to the HoldTime from the triggering  Join/Prune message */
  holdtime = pDnStrmPerIntfStarGEventInfo->holdtime;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "(*,G) dnstrm Expire timer holdtime = %d", holdtime); 
  pTimerData = &pStarGIEntry->pimsmStarGIJoinExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;

  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGIJoinExpiryTimerExpiresHandler,
                            (void*)pStarGIEntry->pimsmStarGIJoinExpiryTimerHandle,
                            holdtime,
                            &(pStarGIEntry->pimsmStarGIJoinExpiryTimer),
                            "xGI-JT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                "Failed to add (*,G,%d) join expire timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Restart Expiry timer of (*,G) entry 
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM 
*                                                    event related data 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfStarGExpireTimerRestart (
                                                         pimsmCB_t * pimsmCb,
                                                         pimsmStarGNode_t * pStarGNode, 
                                                         pimsmDnStrmPerIntfStarGEventInfo_t 
                                                         * pDnStrmPerIntfStarGEventInfo)
{
  L7_uint32    holdtime;
  L7_uint32    timeLeft = 0;
  L7_uint32    rtrIfNum;
  pimsmStarGIEntry_t *pStarGIEntry;
  pimsmTimerData_t   *pTimerData = L7_NULLPTR;

  rtrIfNum = pDnStrmPerIntfStarGEventInfo->rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "rtrIfNum =%d ", rtrIfNum);
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum); ;      
    return L7_FAILURE;
  }

  pimsmUtilAppTimerCancel (pimsmCb, &(pStarGIEntry->pimsmStarGIPrunePendingTimer));

  /* Restart Expiry Timer; */
  /* set to the HoldTime from the triggering  Join/Prune message */
  holdtime = pDnStrmPerIntfStarGEventInfo->holdtime;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "(*,G) dnstrm Expire timer holdtime = %d", holdtime);

  pTimerData = &pStarGIEntry->pimsmStarGIJoinExpiryTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;
  pTimerData->pimsmCb = pimsmCb;

  if (appTimerTimeLeftGet (pimsmCb->timerCb, pStarGIEntry->pimsmStarGIJoinExpiryTimer,
                           &timeLeft) == L7_SUCCESS)
  {
    holdtime = max (timeLeft, holdtime);
  }

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGIJoinExpiryTimerExpiresHandler,
                            (void*)pStarGIEntry->pimsmStarGIJoinExpiryTimerHandle,
                            holdtime,
                            &(pStarGIEntry->pimsmStarGIJoinExpiryTimer),
                            "xGI-JT2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                "Failed to add (*,G,%d) join expire timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Start Prune Pending timer of (*,G) entry  
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM 
*                                                    event related data 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfStarGPrunePendingTimerStart (
                                                             pimsmCB_t * pimsmCb,
                                                             pimsmStarGNode_t * pStarGNode, 
                                                             pimsmDnStrmPerIntfStarGEventInfo_t 
                                                             * pDnStrmPerIntfStarGEventInfo)
{
  L7_uint32            rtrIfNum, timeOut = 0;
  pimsmStarGIEntry_t  *pStarGIEntry;
  pimsmTimerData_t    *pTimerData = L7_NULLPTR;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;

  rtrIfNum = pDnStrmPerIntfStarGEventInfo->rtrIfNum;
  if (pimsmIntfEntryGet( pimsmCb, rtrIfNum, &pIntfEntry )
      !=L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum); 
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "rtrIfNum = %d ", rtrIfNum);
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);       
    return L7_FAILURE;
  }
  /* The Prune-Pending Timer is started; it is set to the 
     J/P_Override_Interval(I) if the router has more than one neighbor 
     on that interface; otherwise it is set to zero.
     */
  if (pIntfEntry->pimsmNbrCount > 1)
  {
   timeOut = pimsmJoinPruneOverrideIntervalGet(pimsmCb,rtrIfNum);
  }
  pTimerData = &pStarGIEntry->pimsmStarGIPrunePendingTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr2, &pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress);
  pTimerData->rtrIfNum = rtrIfNum;
 
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmStarGIPrunePendingTimerExpiresHandler,
                            (void*)pStarGIEntry->pimsmStarGIPrunePendingTimerHandle,
                            timeOut,
                            &(pStarGIEntry->pimsmStarGIPrunePendingTimer),
                            "xGI-PPT")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, 
                "Failed to add (*,G,%d) prune pending timer",rtrIfNum);
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Send (*,G) Prune Echo message 
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM 
*                                                    event related data 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfStarGPruneEchoSend(
                                                   pimsmCB_t * pimsmCb,
                                                   pimsmStarGNode_t * pStarGNode, 
                                                   pimsmDnStrmPerIntfStarGEventInfo_t 
                                                   * pDnStrmPerIntfStarGEventInfo)
{
  pimsmStarGIEntry_t *pStarGIEntry;
  pimsmStarGEntry_t  *pStarGEntry;
  pimsmSendJoinPruneData_t jpData;
  pimsmInterfaceEntry_t *pIntfEntry = L7_NULLPTR;
  L7_uint32 rtrIfNum ;
  L7_RC_t rc;
  L7_uchar8 maskLen =0;

  rtrIfNum = pDnStrmPerIntfStarGEventInfo->rtrIfNum;
  rc = pimsmIntfEntryGet(pimsmCb, rtrIfNum, &pIntfEntry);
  if (rc != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to get interface entry for %d",rtrIfNum);     
    return L7_FAILURE;
  }
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "rtrIfNum =%d ", rtrIfNum);    
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pStarGIEntry == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(*,G,%d) entry is not present",rtrIfNum);     
    return L7_FAILURE;
  }
  /* A PruneEcho(*,G) need not be sent on an interface
        that contains only a single PIM neighbor */
  if (pIntfEntry->pimsmNbrCount <= 1)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "nbrCount = %d for rtrIfNum = %d",
                pIntfEntry->pimsmNbrCount, rtrIfNum);   
    return(L7_SUCCESS);
  }

  /* Send Prune Echo(*,G);*/
  pStarGEntry = &pStarGNode->pimsmStarGEntry;
  memset(&jpData, 0, sizeof(pimsmSendJoinPruneData_t));    
  jpData.rtrIfNum = rtrIfNum;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO, "rtrIfNum =%d ", rtrIfNum);    

  jpData.holdtime = pIntfEntry->pimsmInterfaceJoinPruneHoldtime;  
  /* put router's  own address in  the Upstream Neighbor Address field*/
  jpData.pNbrAddr = &pIntfEntry->pimsmInterfaceAddr;
  jpData.addrFlags = PIMSM_ADDR_WC_BIT | PIMSM_ADDR_RP_BIT;
  jpData.joinOrPruneFlag = L7_FALSE;
  pimSrcMaskLenGet(pimsmCb->family,&maskLen);
  jpData.srcAddrMaskLen = maskLen;
  pimGrpMaskLenGet(pimsmCb->family,&maskLen);
  jpData.grpAddrMaskLen = maskLen;
  jpData.pSrcAddr = 
  &pStarGEntry->pimsmStarGRPAddress;
  jpData.pGrpAddr = 
  &pStarGEntry->pimsmStarGGrpAddress;
  jpData.pruneEchoFlag = L7_TRUE;

  if (pimsmJoinPruneSend(pimsmCb, &jpData) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,
                "Failed to send (S,G) Prune Echo Message on rtrIfNum = %d",rtrIfNum);
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose  Change the state and return   
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM
*                                                    event related data 
*
* @returns  L7_SUCCESS
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfStarGDoNothing
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo)
{
  MCAST_UNUSED_PARAM(pStarGNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfStarGEventInfo);
  return(L7_SUCCESS); /* Change to new state */
}
/******************************************************************************
* @purpose  Unexpected event, return the error  
*
* @param    pimsmCb                      @b{(input)} PIMSM Control Block
* @param    pStarGNode                   @b{(input)} (*,G) node
* @param    pDnStrmPerIntfStarGEventInfo @b{(input)} Downstream (*,G) FSM 
*                                                    event related data 
*
* @returns  L7_FAILURE
*
* @comments     
*     
* @end
******************************************************************************/
static L7_RC_t pimsmDnStrmPerIntfStarGErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmStarGNode_t * pStarGNode, 
 pimsmDnStrmPerIntfStarGEventInfo_t * pDnStrmPerIntfStarGEventInfo)
{
  /* Invalid State */
  MCAST_UNUSED_PARAM(pStarGNode);
  MCAST_UNUSED_PARAM(pDnStrmPerIntfStarGEventInfo);
  return(L7_FAILURE); 
}

/******************************************************************************
* @purpose  Action Routine when (*,G) Prune Pending Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns  none
*
* @comments     
*     
* @end
******************************************************************************/
static void pimsmStarGIPrunePendingTimerExpiresHandler(void *pParam)
{
  pimsmDnStrmPerIntfStarGEventInfo_t dnStrmPerIntfStarGEventInfo;
  pimsmTimerData_t   *pTimerData;
  pimsmStarGNode_t   *pStarGNode;
  pimsmStarGIEntry_t *pStarGIEntry = L7_NULLPTR;
  L7_uint32           rtrIfNum;
  pimsmCB_t          *pimsmCb;
  L7_int32            handle = (L7_int32)pParam;
  pimsmUpStrmStarGEventInfo_t UpStrmStarGEventInfo;
  L7_BOOL  joinDesired =L7_FALSE; 
  L7_BOOL couldAssert;
  pimsmPerIntfStarGAssertEventInfo_t  starGAssertEventInfo;
  L7_inet_addr_t *pGrpAddr;
  
  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);   
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  } 
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb;
  pGrpAddr = &pTimerData->addr2;
  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);  
  if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_NORMAL,
        "(*,G) Find failed");
    return;
  }   
 
  pStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pStarGIEntry->pimsmStarGIPrunePendingTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM Downstream pimsmStarGIPrunePendingTimer is NULL, But Still Expired");
    return;
  }
  pStarGIEntry->pimsmStarGIPrunePendingTimer = L7_NULLPTR;  


  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NOTICE, "(*,G,I) PrunePending Timer Expired ");
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_DEBUG, "rtrIfNum: %d ",rtrIfNum);  
  memset(&dnStrmPerIntfStarGEventInfo, 0, sizeof(pimsmDnStrmPerIntfStarGEventInfo_t));

  dnStrmPerIntfStarGEventInfo.eventType = 
  PIMSM_DNSTRM_STAR_G_SM_EVENT_PRUNE_PENDING_TIMER_EXPIRES;
  dnStrmPerIntfStarGEventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfStarGExecute(pimsmCb, pStarGNode, &dnStrmPerIntfStarGEventInfo);
  memset(&UpStrmStarGEventInfo,0,sizeof(pimsmUpStrmStarGEventInfo_t));
  joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
  if (joinDesired == L7_TRUE)
  {
    UpStrmStarGEventInfo.eventType 
    = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    UpStrmStarGEventInfo.eventType 
    = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &UpStrmStarGEventInfo);


  memset(&starGAssertEventInfo, 0, sizeof(pimsmPerIntfStarGAssertEventInfo_t));
  inetAddressZeroSet(pimsmCb->family,&starGAssertEventInfo.srcAddr);
  starGAssertEventInfo.rtrIfNum = rtrIfNum;
  couldAssert = pimsmStarGICouldAssert(pimsmCb, pStarGNode, rtrIfNum);
  if(couldAssert == L7_FALSE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
       "(couldAssert == false)");
    starGAssertEventInfo.eventType = 
    PIMSM_ASSERT_STAR_G_SM_EVENT_COULD_ASSERT_FALSE;
    pimsmPerIntfStarGAssertExecute(pimsmCb, pStarGNode,&starGAssertEventInfo);
  } 
  else
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "(couldAssert == True)");
  }

  /* Update to MFC about the (*,G) entry update and also (S,G) entry update
   * as (S,G) OIF list is inherited from (*,G).
   */
  if (pimsmStarGMFCUpdate (pimsmCb, pStarGNode, 
           MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to update (*,G) to MFC");
  }
  if (pimsmSGTreePruneRecvEntryUpdate (pimsmCb,
                            &(pStarGNode->pimsmStarGEntry.pimsmStarGGrpAddress),
                            rtrIfNum)
                         != L7_SUCCESS)
  {
    PIMSM_TRACE (PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR, "Failed to update (S,G) to MFC");
  }

  return;
}

/******************************************************************************
* @purpose  Action Routine when (*,G) Join Expiry Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns  none
*
* @comments     
*     
* @end
******************************************************************************/
static void pimsmStarGIJoinExpiryTimerExpiresHandler(void *pParam)
{
  pimsmDnStrmPerIntfStarGEventInfo_t eventInfo;

  pimsmTimerData_t *pTimerData ;
  pimsmStarGNode_t * pStarGNode;
  pimsmStarGIEntry_t *pimsmStarGIEntry =L7_NULLPTR;
  L7_uint32 rtrIfNum;
  pimsmCB_t * pimsmCb;
  L7_int32      handle = (L7_int32)pParam; 
  pimsmUpStrmStarGEventInfo_t starGUpStrmEvData;
  L7_BOOL  joinDesired =L7_FALSE; 
  L7_inet_addr_t *pGrpAddr;
  
  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);   
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  } 
  rtrIfNum = pTimerData->rtrIfNum;
  pimsmCb = pTimerData->pimsmCb; 
  pGrpAddr = &pTimerData->addr2;

  PIMSM_TRACE_ADDR(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_INFO,
     "Group Address :", pGrpAddr);  
  if (pimsmStarGFind(pimsmCb, pGrpAddr, &pStarGNode) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G,PIMSM_TRACE_NORMAL,
        "(*,G) Find failed");
    return;
  }  
  pimsmStarGIEntry = pStarGNode->pimsmStarGIEntry[rtrIfNum];
  if (pimsmStarGIEntry->pimsmStarGIJoinExpiryTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMSM Downstream pimsmStarGIJoinExpiryTimer is NULL, But Still Expired");
    return;
  }
  pimsmStarGIEntry->pimsmStarGIJoinExpiryTimer = L7_NULLPTR;  
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_NOTICE, "(*,G,I) JoinExpiry Timer Expired ");     
  PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_DEBUG, "RtrIfNum: %d ",rtrIfNum);  
  memset(&eventInfo, 0, sizeof(pimsmDnStrmPerIntfStarGEventInfo_t));
  eventInfo.eventType = 
  PIMSM_DNSTRM_STAR_G_SM_EVENT_EXPIRY_TIMER_EXPIRES;
  eventInfo.rtrIfNum = rtrIfNum;

  pimsmDnStrmPerIntfStarGExecute(pimsmCb, pStarGNode,
                                 &eventInfo);
  memset(&starGUpStrmEvData,0,sizeof(pimsmUpStrmStarGEventInfo_t));
  joinDesired = pimsmStarGJoinDesired(pimsmCb, pStarGNode);
  if (joinDesired == L7_TRUE)
  {
    starGUpStrmEvData.eventType 
    = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_TRUE;
  }
  else
  {
    starGUpStrmEvData.eventType 
    = PIMSM_UPSTRM_STAR_G_SM_EVENT_JOIN_DESIRED_FALSE;
  }
  pimsmUpStrmStarGExecute(pimsmCb, pStarGNode, &starGUpStrmEvData);
  if(pimsmStarGMFCUpdate(pimsmCb,pStarGNode, 
                   MFC_UPDATE_ENTRY, L7_FALSE) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_STAR_G, PIMSM_TRACE_ERROR,"Failed to update MFC");
    return ;
  }  
}




/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename pimsmSGRegisterFSM.c
*
* @purpose Contains PIM-SM per (S,G) register State Machine
* implementation
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
#include "buff_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"
#include "pimsmdefs.h"
#include "pimsmsgtree.h"
#include "pimsmsgregisterfsm.h"
#include "pimsmtimer.h"
#include "pimsmrp.h"

static L7_RC_t   pimsmPerSGRegisterRegTunnelAdd
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterRegTunnelRemove
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterRegStopTimerSet
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterRegTunnelUpdate
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterNullRegisterSend
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterRegTunnelRemoveRegStopTimerSet
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterRegTunnelAddRegStopTimerCancel
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterDoNothing
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);
static L7_RC_t   pimsmPerSGRegisterErrorReturn
(pimsmCB_t * pimsmCb,
 pimsmSGNode_t * pSGNode,
 pimsmPerSGRegisterEventInfo_t * perSGRegisterEventInfo);

typedef struct pimsmPerSGRegisterSM_s
{
  L7_uint32 nextState;
  L7_RC_t (*pimsmPerSGRegisterAction) (pimsmCB_t * pimsmCb,
                                       pimsmSGNode_t * pSGNode,
                                       pimsmPerSGRegisterEventInfo_t
                                       * perSGRegisterEventInfo);
} pimsmPerSGRegisterSM_t;

static pimsmPerSGRegisterSM_t pimsmPerSGRegister[PIMSM_REG_PER_S_G_SM_EVENT_MAX]
[PIMSM_REG_PER_S_G_SM_STATE_MAX] =
{
  {/* EVENT_Register-Stop Timer expires */
   /* NO_INFO */  {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterErrorReturn},
    /* JOIN */   {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterErrorReturn},
    /* JOIN_PENDING */ {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterRegTunnelAdd},
    /* PRUNE */  {PIMSM_REG_PER_S_G_SM_STATE_JOIN_PENDING,
      pimsmPerSGRegisterNullRegisterSend}
  },
  {/* EVENT_Could Register->L7_TRUE */
    /* NO_INFO */ {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterRegTunnelAdd},
    /* JOIN */  {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterErrorReturn},
    /* JOIN_PENDING */  {PIMSM_REG_PER_S_G_SM_STATE_JOIN_PENDING,
      pimsmPerSGRegisterErrorReturn},
    /* PRUNE */ {PIMSM_REG_PER_S_G_SM_STATE_PRUNE,
      pimsmPerSGRegisterErrorReturn}
  },
  {/*  EVENT_Could Register->L7_FALSE */
    /* NO_INFO */ {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterErrorReturn},
    /* JOIN */  {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterRegTunnelRemove},
    /* JOIN_PENDING */  {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterDoNothing},
    /* PRUNE */ {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterDoNothing}
  },
  {/*EVENT_Register-Stop received */
    /* NO_INFO */ {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterErrorReturn},
    /* JOIN */  {PIMSM_REG_PER_S_G_SM_STATE_PRUNE,
      pimsmPerSGRegisterRegTunnelRemoveRegStopTimerSet},
    /* JOIN_PENDING */  {PIMSM_REG_PER_S_G_SM_STATE_PRUNE,
      pimsmPerSGRegisterRegStopTimerSet},
    /* PRUNE */ {PIMSM_REG_PER_S_G_SM_STATE_PRUNE,
      pimsmPerSGRegisterErrorReturn}
  },
  {/* EVENT_RP changed */
    /* NO_INFO */ {PIMSM_REG_PER_S_G_SM_STATE_NO_INFO,
      pimsmPerSGRegisterErrorReturn},
    /* JOIN */  {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterRegTunnelUpdate},
    /* JOIN_PENDING */  {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterRegTunnelAddRegStopTimerCancel},
    /* PRUNE */ {PIMSM_REG_PER_S_G_SM_STATE_JOIN,
      pimsmPerSGRegisterRegTunnelAddRegStopTimerCancel}
  }
};

static void pimsmSGDRRegisterStopTimerExpiresHandler(void *pParam);

/******************************************************************************
* @purpose   execute the per interface (S,G) register FSM and process the events
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
L7_RC_t pimsmPerSGRegisterExecute (pimsmCB_t * pimsmCb,
                                  pimsmSGNode_t * pSGNode,
                                  pimsmPerSGRegisterEventInfo_t
                                  * perSGRegisterEventInfo)
{
  pimsmRegPerSGStates_t   regPerSGState;
  pimsmRegPerSGEvents_t   regPerSGEvent;
  L7_RC_t                 rc = L7_SUCCESS;

  if((pSGNode == (pimsmSGNode_t *)L7_NULLPTR) ||
     (perSGRegisterEventInfo ==(pimsmPerSGRegisterEventInfo_t *)L7_NULLPTR))
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_NORMAL, "invalid input parameters");
    return L7_SUCCESS;
  }
  if((pSGNode->flags & PIMSM_NODE_DELETE) != L7_NULL)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_S_G,PIMSM_TRACE_ERROR,
        "Node is deleted");
    return L7_FAILURE;
  }
  regPerSGState = pSGNode->pimsmSGEntry.pimsmSGDRRegisterState;
  regPerSGEvent = perSGRegisterEventInfo->eventType;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "State = %s, \n Event = %s",
              pimsmRegPerSGStateName[regPerSGState],
              pimsmRegPerSGEventName[regPerSGEvent]);

  if(((pimsmPerSGRegister[regPerSGEvent][regPerSGState]).pimsmPerSGRegisterAction
      (pimsmCb, pSGNode,perSGRegisterEventInfo)) != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_NORMAL, "PerSGRegisterAction Failed");
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,"PerSGRegisterAction Failed. State = %s, \n Event = %s",
      pimsmRegPerSGStateName[regPerSGState],
      pimsmRegPerSGEventName[regPerSGEvent]);
    pimsmSGNodeTryRemove(pimsmCb, pSGNode);
    return L7_FAILURE;
  }
  pSGNode->pimsmSGEntry.pimsmSGDRRegisterState =
  (pimsmPerSGRegister[regPerSGEvent][regPerSGState]).nextState;
  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO, "Next State = %s",
              pimsmRegPerSGStateName[pSGNode->pimsmSGEntry.pimsmSGDRRegisterState])
  pimsmSGNodeTryRemove(pimsmCb, pSGNode);
  return rc;
}
/******************************************************************************
* @purpose   Add a register channel
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t  pimsmPerSGRegisterRegTunnelAdd(pimsmCB_t * pimsmCb,
                                               pimsmSGNode_t * pSGNode,
                                               pimsmPerSGRegisterEventInfo_t
                                               * perSGRegisterEventInfo)
{
  pimsmSGIEntry_t *pSGIEntry;
  pimsmSGEntry_t  *pSGEntry;
  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);

  pSGIEntry = pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM];

  if(pSGIEntry == (pimsmSGIEntry_t *) L7_NULLPTR)
  {
    if ((pSGIEntry = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmSGIEntry_t)))
                               == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (S,G) interface pool");
      return L7_FAILURE;
    }
    pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] = pSGIEntry;
  }
  pSGEntry = &pSGNode->pimsmSGEntry;
  /*
    It is important to delete the (S,G)  entry in MFC here
    before adding a tunnel so that data comes to CPU and register messages
    are sent to the RP to ensure correct behavior of the protocol
   */
  {
     pimsmSGMFCUpdate(pimsmCb, pSGNode, MFC_DELETE_ENTRY, L7_FALSE);
  }
  pSGNode->pimsmSGEntry.pimsmSGSPTBit = L7_FALSE;

  /* Add Reg Tunnel;*/
  pSGIEntry->pimsmSGIJoinPruneState =
  PIMSM_DNSTRM_S_G_SM_STATE_JOIN;

  inetAddressZeroSet(pimsmCb ->family,&pSGIEntry->pimsmSGIAssertWinnerAddress);

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Remove the register channel
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t   pimsmPerSGRegisterRegTunnelRemove(pimsmCB_t * pimsmCb,
                                               pimsmSGNode_t * pSGNode,
                                               pimsmPerSGRegisterEventInfo_t
                                               * perSGRegisterEventInfo)
{
  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);
  if(pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] !=
     (pimsmSGIEntry_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family,
                (void*) pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM]);
    pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] = L7_NULLPTR;
  }
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Set the Register Stop Timer
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t   pimsmRegStopTimerSet (pimsmCB_t * pimsmCb,
                                   pimsmSGNode_t * pSGNode)
{
  L7_uint32     low, high, regStopTime;
  pimsmTimerData_t *pTimerData;

  /* Set Register-Stop Timer;*/
  /* (0.5 * PIMSM_DEFAULT_REGISTER_SUPPRESSION_TIME)
                  + (PIMSM_RANDOM() % (PIMSM_DEFAULT_REGISTER_SUPPRESSION_TIME + 1)));*/

  low =    (L7_uint32) (PIMSM_DEFAULT_REGISTER_SUPPRESSION_TIME / 2);
  high =   (L7_uint32) ((15 *  PIMSM_DEFAULT_REGISTER_SUPPRESSION_TIME)/10);

  regStopTime  = (L7_uint32)((low + PIMSM_RANDOM() % ((high - low)))
                             - PIMSM_DEFAULT_REGISTER_PROBE_TIME);

  pTimerData =  &pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;

  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGDRRegisterStopTimerExpiresHandler,
                            (void*)pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimerHandle,
                            regStopTime,
                            &(pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimer),
                            "SM-RST")
                         != L7_SUCCESS)
  {
     PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                 "Failed to add (S,G) DR Register-Stop Timer");
    return L7_FAILURE;
  }

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Set the Register Stop Timer
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t   pimsmPerSGRegisterRegStopTimerSet (pimsmCB_t * pimsmCb,
                                                pimsmSGNode_t * pSGNode,
                                                pimsmPerSGRegisterEventInfo_t
                                                * perSGRegisterEventInfo)
{
  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);
  return pimsmRegStopTimerSet(pimsmCb, pSGNode);
}
/******************************************************************************
* @purpose   Update the register tunnel
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t   pimsmPerSGRegisterRegTunnelUpdate (pimsmCB_t * pimsmCb,
                                                   pimsmSGNode_t * pSGNode,
                                                   pimsmPerSGRegisterEventInfo_t
                                                   * perSGRegisterEventInfo)
{
  pimsmSGIEntry_t *pSGIEntry;
  L7_inet_addr_t rpAddr;

  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);

  pSGIEntry = pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM];

  if(pSGIEntry == (pimsmSGIEntry_t *) L7_NULLPTR)
  {
    if ((pSGIEntry = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmSGIEntry_t)))
                               == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (S,G) interface pool");
      return L7_FAILURE;
    }
    pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] = pSGIEntry;
  }

  /* Update Reg Tunnel;*/

  pimsmRpAddressGet(pimsmCb, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress, &rpAddr);

  if (mcastIpMapUnnumberedIsLocalAddress(&rpAddr, L7_NULLPTR) == L7_TRUE)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,"setting Dnstrm state to NO_INFO");
     pSGIEntry->pimsmSGIJoinPruneState = PIMSM_DNSTRM_S_G_SM_STATE_NO_INFO;
  }
  else
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,"setting Dnstrm state to JOIN");
     pSGIEntry->pimsmSGIJoinPruneState = PIMSM_DNSTRM_S_G_SM_STATE_JOIN;
  }

  inetAddressZeroSet(pimsmCb ->family,&pSGIEntry->pimsmSGIAssertWinnerAddress);

  return(L7_SUCCESS);
}

/******************************************************************************
* @purpose   Add Register tunnel and cancel register stop timer
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmPerSGRegisterRegTunnelAddRegStopTimerCancel (pimsmCB_t * pimsmCb,
                                                         pimsmSGNode_t * pSGNode,
                                                         pimsmPerSGRegisterEventInfo_t
                                                         * perSGRegisterEventInfo)
{
  pimsmSGIEntry_t *pSGIEntry;

  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);
  /* Add Reg Tunnel;*/
  pSGIEntry = pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM];
  if(pSGIEntry == (pimsmSGIEntry_t *) L7_NULLPTR)
  {
    if ((pSGIEntry = PIMSM_ALLOC (pimsmCb->family, sizeof(pimsmSGIEntry_t)))
                               == L7_NULLPTR)
    {
      PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM, PIMSM_TRACE_ERROR,
                  "PIMSM : Failed to allocate memory from (S,G) interface pool");
      return L7_FAILURE;
    }
    pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] = pSGIEntry;
  }
  pSGIEntry->pimsmSGIJoinPruneState =
  PIMSM_DNSTRM_S_G_SM_STATE_JOIN;

  /* Cancel Register-Stop Timer;*/
  pimsmUtilAppTimerCancel (pimsmCb, &(pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimer));

  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Send a NULL register packet
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmPerSGRegisterNullRegisterSend(pimsmCB_t * pimsmCb,
                                                pimsmSGNode_t * pSGNode,
                                                pimsmPerSGRegisterEventInfo_t
                                                * perSGRegisterEventInfo)
{
  L7_uint32     regStopTime;
  pimsmTimerData_t *pTimerData;

  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);

  /*check if timerHandle is already set*/
  if(pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimer != L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_INFO,
      "(S,G) DR Register-Stop Timer is already started");
    return L7_FAILURE;
  }
  regStopTime  =  PIMSM_DEFAULT_REGISTER_PROBE_TIME;
  pTimerData = &pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimerParam;
  memset(pTimerData, 0 , sizeof(pimsmTimerData_t));
  inetCopy(&pTimerData->addr1, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress);
  inetCopy(&pTimerData->addr2, &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  pTimerData->pimsmCb = pimsmCb;
  if (pimsmUtilAppTimerSet (pimsmCb, pimsmSGDRRegisterStopTimerExpiresHandler,
                            (void*)pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimerHandle,
                            regStopTime,
                            &(pSGNode->pimsmSGEntry.pimsmSGDRRegisterStopTimer),
                            "SM-RST2")
                         != L7_SUCCESS)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_ERROR,
                "Failed to add (S,G) DR Register-Stop Timer");
    return L7_FAILURE;
  }

  pimsmNullRegisterSend(pimsmCb, &pSGNode->pimsmSGEntry.pimsmSGSrcAddress,
                        &pSGNode->pimsmSGEntry.pimsmSGGrpAddress);
  return(L7_SUCCESS);
}
/******************************************************************************
* @purpose   Remove the register channel and set register stop timer
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmPerSGRegisterRegTunnelRemoveRegStopTimerSet(pimsmCB_t * pimsmCb,
                                                         pimsmSGNode_t * pSGNode,
                                                         pimsmPerSGRegisterEventInfo_t
                                                         * perSGRegisterEventInfo)
{

  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);
  /* Remove Reg Tunnel;*/
  if(pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] !=
     (pimsmSGIEntry_t *)L7_NULLPTR)
  {
    PIMSM_FREE (pimsmCb->family,
                (void*) pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM]);
    pSGNode->pimsmSGIEntry[PIMSM_REGISTER_INTF_NUM] = L7_NULLPTR;
  }
  /* Set Register-Stop Timer;*/
  return pimsmRegStopTimerSet(pimsmCb, pSGNode);
}
/******************************************************************************
* @purpose   return after changing the FSM state
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_SUCCESS
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmPerSGRegisterDoNothing(pimsmCB_t * pimsmCb,
                                          pimsmSGNode_t * pSGNode,
                                          pimsmPerSGRegisterEventInfo_t
                                          * perSGRegisterEventInfo)
{
  MCAST_UNUSED_PARAM(pSGNode);
  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);
  return(L7_SUCCESS);  /* Only change the state */
}
/******************************************************************************
* @purpose   return Error (unexpected event recived)
*
* @param    pimsmCb                  @b{(input)} PIMSM Control Block
* @param    pSGNode                  @b{(input)} (S,G) node
* @param    perSGRegisterEventInfo   @b{(input)} SG Register FSM event related data
*
* @returns   L7_FAILURE
*
* @comments
*
* @end
******************************************************************************/
static L7_RC_t pimsmPerSGRegisterErrorReturn(pimsmCB_t * pimsmCb,
                                            pimsmSGNode_t * pSGNode,
                                            pimsmPerSGRegisterEventInfo_t
                                            * perSGRegisterEventInfo)
{
  MCAST_UNUSED_PARAM(pSGNode);
  MCAST_UNUSED_PARAM(perSGRegisterEventInfo);
  return(L7_FAILURE);  /* Error State */
}

/******************************************************************************
* @purpose  Action Routine when (S,G) DR Register Stop Timer expires
*
* @param    pParam    @b{(input)} timer handle node
*
* @returns   none
*
* @comments
*
* @end
******************************************************************************/
static void pimsmSGDRRegisterStopTimerExpiresHandler(void *pParam)
{
  pimsmSGNode_t *pSGNode;
  pimsmPerSGRegisterEventInfo_t perSGRegisterEventInfo;
  pimsmSGEntry_t *pSGEntry= L7_NULLPTR;
  pimsmCB_t   *pimsmCb;
  L7_int32     handle = (L7_int32)pParam;
  pimsmTimerData_t *pTimerData;
  L7_inet_addr_t *pGrpAddr, *pSrcAddr;

  pTimerData = (pimsmTimerData_t*)handleListNodeRetrieve(handle);
  if (pTimerData  == L7_NULLPTR)
  {
    PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_NORMAL, "pTimerData == L7_NULLPTR");
    return ;
  }
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

  pSGEntry = &pSGNode->pimsmSGEntry;

  if (pSGEntry->pimsmSGDRRegisterStopTimer == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_PIMSM_MAP_COMPONENT_ID,
            "PIMSM pimsmSGDRRegisterStopTimer is NULL, But Still Expired");
    return;
  }
  pSGEntry->pimsmSGDRRegisterStopTimer = L7_NULLPTR;

  PIMSM_TRACE(PIMSM_DEBUG_BIT_REG_RX_TX_FSM,  PIMSM_TRACE_NORMAL, "(S,G) DR RegisterStop Timer Expired");
  memset(&perSGRegisterEventInfo, 0, sizeof(pimsmPerSGRegisterEventInfo_t));
  perSGRegisterEventInfo.eventType = PIMSM_REG_PER_S_G_SM_EVENT_REG_STOP_TIMER_EXPIRES;
  pimsmPerSGRegisterExecute(pimsmCb, pSGNode, &perSGRegisterEventInfo);
}




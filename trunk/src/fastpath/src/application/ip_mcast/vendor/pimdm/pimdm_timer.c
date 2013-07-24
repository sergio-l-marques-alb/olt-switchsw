/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_timer.c
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l7sll_api.h"
#include "mcast_bitset.h"
#include "mcast_map.h"
#include "pimdm_map.h"
#include "pimdm_defs.h"
#include "pimdm_util.h"
#include "pimdm_mrt.h"
#include "pimdm_debug.h"

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  PIM-DM Application Timer Expiry Handler
*
* @param    timerCtrlBlk @b{ (input) } PIM-DM Appn Timer Control Block
*           timerArg     @b{ (input) } Timer Argument 
*
* @returns  void, if success
* @returns  void, if failure
*
* @comments    timerArg has the Pointer to the PIM-DM Control Block.
*
* @end
*********************************************************************/
void
pimdmAppTimerExpiryHandler (L7_APP_TMR_CTRL_BLK_t timerCtrlBlk,
                            void *timerArg)
{
  pimdmCB_t *pimdmCB = (pimdmCB_t*) timerArg;

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Entry"); */

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"Invalid Ctrl Block");
    return;
  }

  if (pimdmMapMessageQueueSend (pimdmCB->addrFamily,
                                MCAST_EVENT_PIMDM_TIMER_EXPIRY, 0, L7_NULLPTR)
                             != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM-DM App Timer Event Post Failed");
    return;
  }

  /* PIMDM_TRACE (PIMDM_DEBUG_API, "Exit"); */
  return;
}

/*********************************************************************
*
* @purpose  To Initialize the PIM-DM Timers
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmTimersInit (pimdmCB_t *pimdmCB)
{
  L7_uint32 intfTimers = 0;
  L7_uint32 nbrTimers = 0;
  L7_uint32 mrtTimers = 0;
  L7_uint32 rtoTimers = 0;
  L7_uint32 appTimerbufPoolId;
  void *handleListMemHndl = L7_NULLPTR;
  L7_uint32 mrtTableSize = 0;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    mrtTableSize = PIMDM_MRT_MAX_IPV4_ROUTE_ENTRIES;
  }
  else if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    mrtTableSize = PIMDM_MRT_MAX_IPV6_ROUTE_ENTRIES;
  }
  else
  {
    return L7_FAILURE;
  }

  /* Compute the Maximum number of Timers for this instance.
   */
  intfTimers = (PIMDM_MAX_INTERFACES); /* One Hello Timer per Interface */
  nbrTimers =  (PIMDM_MAX_NEIGHBORS);   /* One Liveness Timer per Neighbor */
  rtoTimers =  (1);                     /* RTO Route Change One Sec. Timer */
  mrtTimers =  ((mrtTableSize) *
                ((3) +                                 /* Upstream FSM Timers */
                 (2) +                                 /* Originator FSM Timers */
                 ((2) * (PIMDM_NUM_OPTIMAL_OUTGOING_INTERFACES)) +  /* Downstream FSM Timers */
                 ((1) * (PIMDM_NUM_OPTIMAL_OUTGOING_INTERFACES)))); /* Assert FSM Timers */

  pimdmCB->maxPimdmTimers = intfTimers + nbrTimers + rtoTimers + mrtTimers;

  if (mcastMapGetAppTimerBufPoolId(&appTimerbufPoolId) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Unable to get Buffer Pool ID for App Timer\n");
    return L7_FAILURE;
  }

  /* Create the PIM-DM Application Timer Control Block */
  if ((pimdmCB->appTimer = appTimerInit (L7_FLEX_PIMDM_MAP_COMPONENT_ID,
                                   pimdmAppTimerExpiryHandler, (void*) pimdmCB,
                                   L7_APP_TMR_1SEC, appTimerbufPoolId))
                        == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "App Timer Creation Failed");
    return L7_FAILURE;
  }

  if (mcastMapGetHandleListHandle (pimdmCB->addrFamily,&handleListMemHndl) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Failed to get HandleList Handle");
    return L7_FAILURE;
  }
  if (handleListInit (L7_FLEX_PIMDM_MAP_COMPONENT_ID, pimdmCB->maxPimdmTimers,
                      &pimdmCB->handleList, (handle_member_t*) handleListMemHndl)
                   != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Timer Handle List Creation Failed");
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Timers Creation Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-initialize the PIM-DM Timers
*
* @param    pimdmCB  @b{ (input) } Pointer to the PIM-DM Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t
pimdmTimersDeInit (pimdmCB_t *pimdmCB)
{
  L7_RC_t retVal = L7_SUCCESS;

  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Destroy the PIM-DM Application Timer Control Block */
  if (pimdmCB->appTimer != L7_NULLPTR)
  {
    if (appTimerDeInit (pimdmCB->appTimer) != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "App Timer Destroy Failed");
      retVal = L7_FAILURE;
    }
  }

  /* Destroy the Timers Handle List */
  if (handleListDeinit (L7_FLEX_PIMDM_MAP_COMPONENT_ID, pimdmCB->handleList)
                     != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Handle List Destroy Failed");
    retVal = L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "Timers Destroy Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return retVal;
}


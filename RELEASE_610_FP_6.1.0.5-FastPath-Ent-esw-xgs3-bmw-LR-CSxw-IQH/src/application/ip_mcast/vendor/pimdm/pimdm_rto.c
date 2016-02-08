/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_rto.c
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
#include "l7_pimdm_api.h"
#include "pimdm_map.h"
#include "pimdm_defs.h"
#include "pimdm_util.h"
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
* @purpose  To Initialize the RTO Callback related resources
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
pimdmRTOInit (pimdmCB_t* pimdmCB)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Allocate the RTO Route Change Buffer */
  if (pimdmRTORouteChangeBufferAllocate (pimdmCB) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Route Change Buf Alloc Failure");
    return L7_FAILURE;
  }

  /* Create the Semaphore for RTO Timer */
  if ((pimdmCB->rtoTimerSemId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                  OSAPI_SEM_FULL))
                             == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM RTO Timer Semaphore Creation Failed");
    return L7_FAILURE;
  }

  pimdmCB->rtoTimer = L7_NULLPTR;

  if (pimdmCB->addrFamily == L7_AF_INET)
  {
    pimdmCB->rtoCallback = pimdmMapRto4BestRouteClientCallback;
  }
  else if (pimdmCB->addrFamily == L7_AF_INET6)
  {
    pimdmCB->rtoCallback = pimdmMapRto6BestRouteClientCallback;
  }
  else
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "Invalid Address Family - %d", pimdmCB->addrFamily);
    return L7_FAILURE;
  }

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM RTO Callback Resources Init Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To De-Initialize the RTO Callback related resources
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
pimdmRTODeInit (pimdmCB_t* pimdmCB)
{
  PIMDM_TRACE (PIMDM_DEBUG_API, "Entry");

  /* Free the RTO Route Change Buffer */
  PIMDM_FREE (pimdmCB->addrFamily, (void*) pimdmCB->routeChangeBuffer);

  /* Delete the PIMDM RTO Timer Semaphore */
  if (pimdmCB->rtoTimerSemId != L7_NULLPTR)
  {
    osapiSemaDelete(pimdmCB->rtoTimerSemId);
    pimdmCB->rtoTimerSemId = L7_NULLPTR;
  }

  pimdmCB->rtoTimer = L7_NULLPTR;

  pimdmCB->rtoCallback = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_INIT, "PIM-DM RTO Callback Resources De-Init Successful");

  PIMDM_TRACE (PIMDM_DEBUG_API, "Exit");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Process RTO Timer Event
*
* @param    timerArg     @b{(input)} PIM-DM Protocol Control Block
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none    
*
* @end
*********************************************************************/
void
pimdmRTOTimerExpiryHandler (void *timerArg)
{
  pimdmCB_t *pimdmCB = (pimdmCB_t*) timerArg;

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIM-DM Control Block is Invalid");
    return;
  }

  /* Acquire the RTO Timer Semaphore */
  if (osapiSemaTake (pimdmCB->rtoTimerSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM RTO Timer Semaphore Take Failed");
    return;
  }

  if (pimdmCB->rtoTimer == L7_NULLPTR)
  {
    LOG_MSG ("PIMDM rtoTimer is NULL, But Still Expired");

    /* Release the RTO Timer Semaphore */
    osapiSemaGive (pimdmCB->rtoTimerSemId);
    return;
  }
  pimdmCB->rtoTimer = L7_NULLPTR;

  /* Release the RTO Timer Semaphore */
  osapiSemaGive (pimdmCB->rtoTimerSemId);

  if (pimdmMapMessageQueueSend (pimdmCB->addrFamily, MCAST_EVENT_RTO_BEST_ROUTE_CHANGE,
                                0, L7_NULLPTR)
                             != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE,"PIM-DM RTO Event Post Failed");
    return;
  }

  return;
}

/*********************************************************************
*
* @purpose  To Start RTO Timer
* 
* @param    cbHandle   @b{(input)} PIM-DM Protocol Control Block
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/
void
pimdmRTOTimerStart (MCAST_CB_HNDL_t cbHandle)
{
  pimdmCB_t *pimdmCB = (pimdmCB_t*) cbHandle;

  if (pimdmCB == L7_NULLPTR)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM CB is null");
    return;
  }

  /* Acquire the RTO Timer Semaphore */
  if (osapiSemaTake (pimdmCB->rtoTimerSemId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM RTO Timer Semaphore Take Failed");
    return;
  }

  /* Start the PIM-DM RTO Timer */
  if (pimdmCB->rtoTimer == L7_NULLPTR)
  {
    if (pimdmUtilAppTimerSet (pimdmCB, pimdmRTOTimerExpiryHandler, (void*) pimdmCB,
                              L7_PIM_BEST_ROUTE_DELAY, L7_NULLPTR, L7_NULLPTR,
                              &pimdmCB->rtoTimer,
                              "DM-RTO")
                           != L7_SUCCESS)
    {
      PIMDM_TRACE (PIMDM_DEBUG_FAILURE, "PIMDM RTO Timer Start Failed");

      /* Release the RTO Timer Semaphore */
      osapiSemaGive (pimdmCB->rtoTimerSemId);
      return;
    }
  }

  /* Release the RTO Timer Semaphore */
  osapiSemaGive (pimdmCB->rtoTimerSemId);
  return;
}


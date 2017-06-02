/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pimdm_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 04/23/2006
*
* @author gkiran
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "l7_ip_api.h"
#include "l7_mcast_api.h"
#include "l7_pimdm_api.h"
#include "rto_api.h"
#include "pimdm_map_cnfgr.h"
#include "pimdmmap_sid.h"
#include "pimdm_map_util.h"
#include "pimdm_map.h"
#include "pimdm_map_v6_wrappers.h"
#include "pimdm_init.h"
#include "pimdm_map_debug.h"
#include "pimdm_map_rto.h"
#include "sdm_api.h"


static L7_RC_t pimdmMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData);
static void pimdmMapTaskInitUndo();
static L7_RC_t pimdmCnfgrControlBlockInit(pimdmMapCB_t *pimdmMapCbPtr,
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason);
static void pimdmCnfgrFiniDeallocate(pimdmMapCB_t *pimdmMapCbPtr);
static L7_RC_t pimdmCnfgrConfigFileRead(pimdmMapCB_t *pimdmMapCbPtr,
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIM-DM component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the pimdm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimdmApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  pimdmMapMsg_t        msg;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The mapping task will handle everything.
 */
  if (pCmdData == L7_NULLPTR)
  {
    L7_CNFGR_CB_DATA_t  cbData;

    /* game over... pCmdData is a null pointer */
    cbData.correlator             = 0;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
  }
  else
  {
    if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
        (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
    {
      /* start the PIM-DM Map task to prepare for processing
       * configurator (and other) events                     
       */
      if (pimdmMapTaskInit(pCmdData) != L7_SUCCESS)
      {
        pimdmMapTaskInitUndo();
      }
    }
    else
    {
      /* send the configurator request to the task thread for processing */
      memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.msgId = PIMDMMAP_CNFGR_MSG;
      if (pimdmMessageSend (PIMDM_EVENT_Q,(L7_VOIDPTR)&msg)!= L7_SUCCESS)
      {
        PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to send Configurator request \
                          message to PIM-DM Map Queue");
        return;
      }
    }
  }
}

/*********************************************************************
*
* @purpose  Initialization for PIM-DM Mapping component task
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  pimdmMapMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;
  PIMDM_QUEUE_ID_t count;

  memset (&pimdmGblVariables_g, 0, sizeof(pimdmGblVars_t));
  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_0;

  /* Create a counting semaphore to indicate when a message is available to
   * be read in any of the message queues. */

  pimdmGblVariables_g.msgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);

  if (pimdmGblVariables_g.msgQSema == L7_NULL)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Semaphore Creation failed");  
    return L7_FAILURE;
  }

  /* Create the PIMSM-MAP Counters Semaphore */
  if ((pimdmGblVariables_g.queueCountersSemId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                                  OSAPI_SEM_FULL))
                        == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error Creating Counters Semaphore");
    osapiSemaDelete(pimdmGblVariables_g.msgQSema);

    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Semaphore Creation failed");  
    return L7_FAILURE;
  }

  memcpy(pimdmGblVariables_g.pimdmQueue[PIMDM_APP_TIMER_Q].QName,
         PIMDMMAP_APP_TMR_QUEUE,sizeof(PIMDMMAP_APP_TMR_QUEUE));

  pimdmGblVariables_g.pimdmQueue[PIMDM_APP_TIMER_Q].QSize  = PIMDMMAP_APP_TMR_MSG_SIZE;
  pimdmGblVariables_g.pimdmQueue[PIMDM_APP_TIMER_Q].QCount = PIMDMMAP_APP_TMR_MSG_COUNT;

  memcpy(pimdmGblVariables_g.pimdmQueue[PIMDM_EVENT_Q].QName,
            PIMDMMAP_QUEUE,sizeof(PIMDMMAP_QUEUE));
  pimdmGblVariables_g.pimdmQueue[PIMDM_EVENT_Q].QSize      = PIMDMMAP_MSG_SIZE;
  pimdmGblVariables_g.pimdmQueue[PIMDM_EVENT_Q].QCount     = PIMDMMAP_MSG_COUNT;


  memcpy(pimdmGblVariables_g.pimdmQueue[PIMDM_CTRL_PKT_Q].QName,
            PIMDMMAP_CTRL_PKT_QUEUE,sizeof(PIMDMMAP_CTRL_PKT_QUEUE));
  pimdmGblVariables_g.pimdmQueue[PIMDM_CTRL_PKT_Q].QSize   = PIMDMMAP_CTRL_PKT_MSG_SIZE;
  pimdmGblVariables_g.pimdmQueue[PIMDM_CTRL_PKT_Q].QCount  = PIMDMMAP_CTRL_PKT_MSG_COUNT;


  for(count=0;count < PIMDM_MAX_Q;count++)
  {
    pimdmGblVariables_g.pimdmQueue[count].QPointer=
        (void *)osapiMsgQueueCreate(pimdmGblVariables_g.pimdmQueue[count].QName,
                                    pimdmGblVariables_g.pimdmQueue[count].QCount,
                                    pimdmGblVariables_g.pimdmQueue[count].QSize);

    if(pimdmGblVariables_g.pimdmQueue[count].QPointer == L7_NULLPTR)
    {
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);

      pimdmMapQDelete();
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "msgQueue creation error for Q %d\n",count);
      return L7_FAILURE;
    }
  }

  /* Create the PIMDM-MAP Counters Semaphore */
  if ((pimdmGblVariables_g.eventCountersSemId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                                  OSAPI_SEM_FULL))
                        == L7_NULLPTR)
  {
    pimdmMapQDelete();
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Counters Semaphore Creation Failed");
    return L7_FAILURE;
  }

  pimdmGblVariables_g.pimdmMapTaskId = osapiTaskCreate("pimdmMapTask", 
                                   pimdmMapTask, 0, 0, 
                                   (pimdmMapSidDefaultStackSize() *2), 
                                   pimdmMapSidDefaultTaskPriority(), 
                                   pimdmMapSidDefaultTaskSlice());

  if ((pimdmGblVariables_g.pimdmMapTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit (L7_PIMDM_MAP_TASK_SYNC, 
                             L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    pimdmMapQDelete();
    cnfgrApiCallback(&cbData);
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Task creation failed");
    return L7_FAILURE;
  }

  memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = PIMDMMAP_CNFGR_MSG;

  if (pimdmMessageSend(PIMDM_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_BUSY;
    cnfgrApiCallback(&cbData);
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to post message to PIM-DM Map Queue");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for PIM-DM Mapping component
*
* @param    none
*                            
* @comments none
*
* @end
*********************************************************************/
void pimdmMapTaskInitUndo()
{
  if (pimdmGblVariables_g.msgQSema != L7_NULLPTR)
  {
    osapiSemaDelete(pimdmGblVariables_g.msgQSema);
  }

  pimdmMapQDelete();

  if (pimdmGblVariables_g.pimdmMapTaskId != L7_ERROR)
  {
    osapiTaskDelete(pimdmGblVariables_g.pimdmMapTaskId);
  }

  /* Delete the PIMDM-MAP Counters Semaphore */
  if (pimdmGblVariables_g.eventCountersSemId != L7_NULLPTR)
  {
    osapiSemaDelete(pimdmGblVariables_g.eventCountersSemId);
  }

  /* Delete the PIMDM-MAP Counters Semaphore */
  if (pimdmGblVariables_g.queueCountersSemId != L7_NULLPTR)
  {
    osapiSemaDelete(pimdmGblVariables_g.queueCountersSemId);
  }

  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIMDM component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the pimdm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimdmCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_BOOL               warmRestart = L7_FALSE;

  L7_RC_t             pimdmRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ( request > L7_CNFGR_RQST_FIRST && 
           request < L7_CNFGR_RQST_LAST )
      {
        /* validate command/event pair */
        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((pimdmRC = pimdmCnfgrInitPhase1Process(&response, 
                     &reason )) != L7_SUCCESS)
                {
                  pimdmCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((pimdmRC = pimdmCnfgrInitPhase2Process(&response, 
                     &reason )) != L7_SUCCESS)
                {
                  pimdmCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                /* Determine if this is a cold or warm restart */
                if (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM)
                  warmRestart = L7_TRUE;
                else
                  warmRestart = L7_FALSE;

                if ((pimdmRC = pimdmCnfgrInitPhase3Process(warmRestart, &response, &reason)) != L7_SUCCESS)
                {
                  pimdmCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                pimdmRC = pimdmCnfgrNoopProcess( &response, &reason );
                pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
              case L7_CNFGR_RQST_E_START:
                if (pimdmGblVariables_g.warmRestart == L7_FALSE)
                {
                  (void) cnfgrApiComponentHwUpdateDone(L7_FLEX_PIMDM_MAP_COMPONENT_ID, 
                                                       L7_CNFGR_HW_APPLY_IPMCAST);
                }
                pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_EXECUTE;

                pimdmRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                pimdmRC = pimdmCnfgrNoopProcess( &response, &reason );
                pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                pimdmRC = pimdmCnfgrUconfigPhase2( &response, &reason );
                pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            pimdmRC = pimdmCnfgrNoopProcess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */
      } 
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */
    } 
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    } /* endif validate command type */
  } 
  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  } /* check for command valid pointer */

  /* return value to caller - 
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = pimdmRC;
  if (pimdmRC == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - In case of other failures.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked.
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                              L7_CNFGR_ERR_RC_t *pReason )
{
  L7_uint32       cbIndex = L7_NULL;
  L7_uint32       protoMax = L7_NULL;
  pimdmMapCB_t    *pimdmMapCbPtr = L7_NULLPTR;
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  protoMax =  pimdmMapProtocolsMaxGet();

  pimdmMapCB_g = (pimdmMapCB_t *)osapiMalloc(L7_FLEX_PIMDM_MAP_COMPONENT_ID,sizeof(pimdmMapCB_t) * protoMax);

  if(pimdmMapCB_g == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Failed to allocate memory for CB structure");
    return L7_FAILURE;
  }

  for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];
    if (cbIndex == PIMDM_MAP_IPV4_CB)
    {
      pimdmMapCbPtr->familyType = L7_AF_INET;
    }
    else if (cbIndex == PIMDM_MAP_IPV6_CB)
    {
      pimdmMapCbPtr->familyType = L7_AF_INET6;
    }

    /* Intialize & allocate memory. */
    if (pimdmCnfgrControlBlockInit (pimdmMapCbPtr, pResponse, pReason)
                                 != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,"Initialization of \
                         PIM-DM Mapping Layer Control Block failed for \
                          familyType %d\n",pimdmMapCbPtr->familyType);
      return L7_FAILURE;
    }
  }

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  pimdmDebugCfgRead();
  pimdmApplyDebugConfigData();

  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_1;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimdmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimdmMapMemoryInit(pimdmMapCB_t* pimdmMapCb)
{
  if(pimdmMapCb->pPimdmInfo->pimdmheapAlloc == L7_TRUE)
  {
    /* calling this API second time is an error*/
    return L7_FAILURE;
  }

  /* Initialize the Memory for PIM-DM */
  if (pimdmMemoryInit (pimdmMapCb->familyType) != L7_SUCCESS)
  {
    PIMDM_MAP_LOG_MSG (pimdmMapCb->familyType, "PIMDM Protocol Heap Memory Init "
                       "Failed; Family - %d", pimdmMapCb->familyType);
    return L7_FAILURE;
  }

  pimdmMapCb->pPimdmInfo->pimdmheapAlloc = L7_TRUE; 
  return L7_SUCCESS;  
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimdmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimdmMapMemoryDeInit(pimdmMapCB_t* pimdmMapCb)
{
  if(pimdmMapCb->pPimdmInfo->pimdmheapAlloc == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* Initialize the Memory for PIM-DM */
  if (pimdmMemoryDeInit (pimdmMapCb->familyType) != L7_SUCCESS)
  {
    PIMDM_MAP_LOG_MSG (pimdmMapCb->familyType,
                       "PIMDM Protocol Heap Memory De-Init Failed; Family - %d",
                       pimdmMapCb->familyType);
    return L7_FAILURE;
  }

  pimdmMapCb->pPimdmInfo->pimdmheapAlloc = L7_FALSE; 
  return L7_SUCCESS;  
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimdmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase1DynamicProcess (pimdmMapCB_t* pimdmMapCb)
{
  if(pimdmMapCb == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Invalid PIM-DM Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if ((pimdmMapCb->familyType == L7_AF_INET6) && !sdmTemplateSupportsIpv6())
  {
    return L7_SUCCESS;
  }

  /* Initialize the PIM-DM Control Block */
  if ((pimdmMapCb->cbHandle = pimdmCtrlBlockInit (pimdmMapCb->familyType))
                           == L7_NULLPTR)
  {
    PIMDM_MAP_LOG_MSG (pimdmMapCb->familyType,
                       "PIMDM Protocol Initialization Failed; Family - %d",
                       pimdmMapCb->familyType);
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Vendor Layer Dynamic Init Failed for Family - %d",
                     pimdmMapCb->familyType); 
    pimdmMemoryDeInit (pimdmMapCb->familyType);
    return L7_FAILURE;
  }


  /* Regsiter with RTO for Unicast Routing Changes when PIM-DM Global
   * Admin Mode is Enabled.
   */
  if (pimdmMapRTOBestRouteClientRegister(pimdmMapCb->familyType) != L7_SUCCESS)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with RTO");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function allocates memory.
*
* @param    pimdmMapCbPtr  @b{(input)}   Mapping Control Block.
* @param    pResponse  -   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    -   @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrControlBlockInit(pimdmMapCB_t *pimdmMapCbPtr,
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason)
{
  /*-----------------------------------------------------------*/
  /* Initialize the pimdmMap configuration data structure  */
  /*-----------------------------------------------------------*/
  pimdmMapCbPtr->pPimdmMapCfgData = &(pimdmMapCbPtr->pimdmMapCfgData);
  memset(pimdmMapCbPtr->pPimdmMapCfgData, 0, sizeof(L7_pimdmMapCfg_t));

  pimdmMapCbPtr->pPimdmMapCfgMapTbl = pimdmMapCbPtr->pimdmMapCfgMapTbl;
  memset(pimdmMapCbPtr->pPimdmMapCfgMapTbl, 0,
                                    sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);

  /*----------------------------------------------*/
  /* malloc space for various PIMDM info structures */
  /*----------------------------------------------*/
  pimdmMapCbPtr->pPimdmInfo = &(pimdmMapCbPtr->pimdmInfo);
  memset(pimdmMapCbPtr->pPimdmInfo, 0, sizeof(pimdmInfo_t));

  pimdmMapCbPtr->pPimdmIntfInfo = (pimdmMapCbPtr->pimdmIntfInfo);
  memset(pimdmMapCbPtr->pPimdmIntfInfo, 0, 
                           sizeof(pimdmMapIntfInfo_t)*(MCAST_MAX_INTERFACES+1));

  /* Updating Global Variable Pointer */
  pimdmMapCbPtr->gblVars = &pimdmGblVariables_g;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes pimdmCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniPhase1Process()
{
  L7_uint32        cbIndex = L7_NULL;
  L7_uint32        protoMax = L7_NULL;
  pimdmMapCB_t     *pimdmMapCbPtr = L7_NULLPTR;

  protoMax =  pimdmMapProtocolsMaxGet();
  for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];

    /* De-initialize the Vendor PIM-DM component */
    pimdmCnfgrFiniPhase1DynamicProcess (pimdmMapCbPtr);

    /* Deallocate anything that was allocated */
    pimdmCnfgrFiniDeallocate(pimdmMapCbPtr);
  }
  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_0;
  osapiFree(L7_FLEX_PIMDM_MAP_COMPONENT_ID,pimdmMapCB_g);
}

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    pimdmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrFiniPhase1DynamicProcess (pimdmMapCB_t* pimdmMapCb)
{
  L7_RC_t retVal = L7_SUCCESS;

  if(pimdmMapCb == L7_NULLPTR)
  {
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES,
                     "Invalid PIM-DM Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if(pimdmMapCb->pPimdmInfo->pimdmheapAlloc == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if ((pimdmMapCb->familyType == L7_AF_INET) ||
      ((pimdmMapCb->familyType == L7_AF_INET6) && sdmTemplateSupportsIpv6()))
  {
    /* De-Initialize the PIM-DM Protocol */
    if (pimdmCtrlBlockDeInit (pimdmMapCb->cbHandle) != L7_SUCCESS)
    {
      PIMDM_MAP_LOG_MSG (pimdmMapCb->familyType,
                         "PIMDM Protocol De-Initialization Failed; Family - %d",
                         pimdmMapCb->familyType); 
      retVal = L7_FAILURE;
    }

    /* De-Initialize the Memory for PIM-DM */
    if (pimdmMemoryDeInit (pimdmMapCb->familyType) != L7_SUCCESS)
    {
      PIMDM_MAP_LOG_MSG (pimdmMapCb->familyType,
                         "PIMDM Protocol Heap Memory De-Init Failed; Family - %d",
                         pimdmMapCb->familyType);
      retVal = L7_FAILURE;
    }
  
    /* De-Regsiter with RTO for Unicast Routing Changes when PIM-DM Global
     * Admin Mode is Disabled.
     */
    if (pimdmMapRTOBestRouteClientDeRegister(pimdmMapCb->familyType) != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error De-registering with RTO");
      retVal = L7_FAILURE;
    }
  }
  pimdmMapCb->pPimdmInfo->pimdmheapAlloc = L7_FALSE;  
  pimdmMapCb->cbHandle = L7_NULLPTR;
  return retVal;
}

/*********************************************************************
* @purpose  This function deallocate that was allocated.
*
* @param    pimdmMapCbPtr  @b{(input)}   Mapping Control Block.
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniDeallocate(pimdmMapCB_t *pimdmMapCbPtr)
{
  /* Deallocate anything that was allocated */
  if (pimdmMapCbPtr->pPimdmMapCfgData != L7_NULL)
  {
    osapiFree(L7_FLEX_PIMDM_MAP_COMPONENT_ID, 
                                    pimdmMapCbPtr->pPimdmMapCfgData);
  }

  if (pimdmMapCbPtr->pPimdmMapCfgMapTbl != L7_NULL)
  {
    osapiFree(L7_FLEX_PIMDM_MAP_COMPONENT_ID, 
                                  pimdmMapCbPtr->pPimdmMapCfgMapTbl);
  }

  if (pimdmMapCbPtr->pPimdmInfo != L7_NULL)
  {
    osapiFree(L7_FLEX_PIMDM_MAP_COMPONENT_ID, pimdmMapCbPtr->pPimdmInfo);
  }

  if (pimdmMapCbPtr->pPimdmIntfInfo != L7_NULL)
  {
    osapiFree(L7_FLEX_PIMDM_MAP_COMPONENT_ID, pimdmMapCbPtr->pPimdmIntfInfo);
  }
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
* 
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - In case of other failures.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked.
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  nvStoreFunctionList_t      notifyFunctionList;
  L7_RC_t                    pimdmRC = L7_FAILURE;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  pimdmRC    = L7_SUCCESS;

  /*----------------------*/
  /* nvStore registration */
  /*----------------------*/
  notifyFunctionList.registrar_ID   = L7_FLEX_PIMDM_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave     = pimdmSave;
  notifyFunctionList.hasDataChanged = pimdmHasDataChanged;
  notifyFunctionList.notifyConfigDump = L7_NULLPTR;
  notifyFunctionList.notifyDebugDump = L7_NULLPTR;
  notifyFunctionList.resetDataChanged = pimdmResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    pimdmRC = L7_ERROR;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with nvStoreRegister");
  }

  /*------------------------------------------*/
  /* register callbacks with IP Mapping layer */
  /*------------------------------------------*/
  /* register PIMDM to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (ipMapRegisterRoutingEventChange(L7_IPRT_PIMDM, 
                       "pimdmMapRoutingEventChangeCallBack",
                        pimdmMapRoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with ipmap for intf state \
                      change callback");
    return L7_ERROR;
  }

  /*------------------------------------------*/
  /* register callbacks with IP6 Mapping layer */
  /*------------------------------------------*/
  /* register PIMDM to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (pimdmMapV6Register(L7_IPRT_PIMDM, 
                       "pimdm6MapRoutingEventChangeCallBack",
                        pimdm6MapRoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with ip6map for intf \
                      state change callback");
    return L7_ERROR;
  }

  /********************************************
   * Register callback with MCAST mapping layer*
   ********************************************/

  /*register mcast mode change event */
  if (mcastMapRegisterAdminModeEventChange(L7_MRP_PIMDM, 
                                           pimdmMapMcastEventCallBack)
                                        != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with MCAST");
    return L7_ERROR;
  }

  /********************************************
   * Register callback with MFC               *
   ********************************************/
  /* NOTE: This Registration will now be done when the Protocol is 
   * Enabled.
   */

  /********************************************
   * Register callback with MCAST-MAP         *
   * for AdminScope Changes                   *
   ********************************************/
  if (mcastMapRegisterAdminScopeEventChange (L7_MRP_PIMDM,
                                          pimdmMapAdminScopeEventChangeCallback)
                                       != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, "Error registering with MCAST component for \
                      Admin Scope Changes");
    return L7_ERROR;
  }

  /* Register for debug */
  (void)pimdmDebugRegister();

  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_2;

  return pimdmRC;
}
/*********************************************************************
* @purpose  This function undoes pimdmpCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  (void)ipMapDeregisterRoutingEventChange(L7_IPRT_PIMDM);
  (void)pimdmMapV6Deregister(L7_IPRT_PIMDM);
   mcastMapDeregisterAdminModeEventChange(L7_MRP_PIMDM); 
  (void)nimDeRegisterIntfChange(L7_FLEX_PIMDM_MAP_COMPONENT_ID);
  mcastMapDeregisterAdminScopeEventChange (L7_MRP_PIMDM);

  /* De-register with MFC */
  /* NOTE: This De-Registration will now be done when the Protocol is 
   * Disabled.
   */
  /* DeRegister for debug */
  (void)pimdmDebugDeRegister();

  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_1;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
*     
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. 
* @returns  L7_ERROR   - There were errors. 
* @returns  L7_FAILURE - In case of other failures.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked.
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrInitPhase3Process(L7_BOOL warmRestart,
                                    L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t      pimdmRC;
  L7_RC_t      rc = L7_FAILURE;
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  pimdmMapCB_t *pimdmMapCbPtr = L7_NULLPTR;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  pimdmRC   = L7_SUCCESS;

  /* Getting Max. protocols based on L7_IPV6_PACKAGE defined. Need 
   * to have two component Id's for v4 & v6.
   */
  protoMax =  pimdmMapProtocolsMaxGet();

  for (cbIndex = PIMDM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimdmMapCbPtr = &pimdmMapCB_g[cbIndex];
    pimdmMapCbPtr->warmRestartInProgress = warmRestart;

    /* Read File. */
    rc = pimdmCnfgrConfigFileRead(pimdmMapCbPtr, pResponse, pReason);
    if (rc != L7_SUCCESS)
    {
      PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                        "Failed to read PIMDM configuration file");
      return rc;
    }
  }

  pimdmGblVariables_g.warmRestart = warmRestart;
  pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_INIT_3;

  return pimdmRC;
}

/*********************************************************************
* @purpose  This function reads configuration file.
*
* @param    pimdmMapCbPtr @b{(input)}   Mapping Control Block.
* @param    pResponse     @b{(output)}  Response if L7_SUCCESS.
* @param    pReason       @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrConfigFileRead(pimdmMapCB_t *pimdmMapCbPtr,
                                 L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t pimdmRC = L7_FAILURE;

  if (pimdmMapCbPtr->familyType == L7_AF_INET)
  {
    pimdmBuildDefaultConfigData(L7_PIMDM_CFG_VER_CURRENT);
  }
  else
  {
    pimdm6BuildDefaultConfigData(L7_PIMDM_CFG_VER_CURRENT);
  }

  if (pimdmApplyConfigData(pimdmMapCbPtr) != L7_SUCCESS)
  {
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    pimdmRC    = L7_ERROR;
    PIMDM_MAP_DEBUG (PIMDM_MAP_DEBUG_FAILURES, 
                      "Error applying PIMDM config data");
    return pimdmRC;
  }
  pimdmMapCbPtr->pPimdmMapCfgData->cfgHdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes pimdmCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimdmCnfgrFiniPhase3Process()
{  
  if(pimdmRestore() == L7_SUCCESS)
  {
    pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_WMU;
  }

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse  - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*          be blocked
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                                L7_CNFGR_ERR_RC_t   *pReason)
{

  L7_RC_t pimdmRC = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if ((pimdmRC = pimdmRestore()) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
  }
  else
  {
    pimdmGblVariables_g.pimdmCnfgrState = PIMDM_PHASE_WMU;
  }

  return pimdmRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse  - @b{(output)}  Response always command complete.
* @param    pReason    - @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           None.
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*          be blocked.
*       
* @end
*********************************************************************/
L7_RC_t pimdmCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pimdmRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(pimdmRC);
}


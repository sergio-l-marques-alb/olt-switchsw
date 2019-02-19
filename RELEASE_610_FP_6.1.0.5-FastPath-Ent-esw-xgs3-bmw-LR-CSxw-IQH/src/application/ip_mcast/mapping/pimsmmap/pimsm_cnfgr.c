/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename pimsm_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 06/23/2003
*
* @author gkiran/dsatyanarayana
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "l7_pimsminclude.h"
#include "pimsmmap_sid.h"
#include "mfc_api.h"
#include "pimsmuictrl.h"
#include "pimsm_map.h"
#include "l7_mcast_api.h"
#include "l7_mgmd_api.h"
#include "pimsm_migrate.h"
#include "pimsm_cnfgr.h"
#include "pimsm_vend_ctrl.h"
#include "l7_pimsm_api.h"
#include "pimsm_v6_wrappers.h"

extern pimsmGblVariables_t pimsmGblVariables_g;
extern pimsmMapCB_t        *pimsmMapCB;

pimsmCfgDefs_t pimsmConstsDefs[PIMSM_MAP_CB_MAX] = {
 {L7_AF_INET,  L7_PIMSM_CFG_FILENAME, "PIM-SM-Map_v4",
   rtoBestRouteClientRegister, 
   pimsmMapRto4BestRouteClientCallback,
   rtoBestRouteClientDeregister,
   L7_IPRT_PIMSM, 
   ipMapRegisterRoutingEventChange, 
   pimsmMapRoutingEventChangeCallBack,
   ipMapDeregisterRoutingEventChange,
   pimsmBuildDefaultConfigData,
   pimsmMigrateConfigData},
 {L7_AF_INET6, L7_PIMSM6_CFG_FILENAME,  "PIM-SM-Map_v6",
   pimsmMapRto6BestRouteRegister, 
   pimsmMapRto6BestRouteClientCallback,
   pimsmMapRto6BestRouteDeRegister,
   L7_IPRT_PIMSM, 
   pimsmMapV6Register, 
   pimsm6MapRoutingEventChangeCallBack,
   pimsmMapV6Deregister,
   pimsm6BuildDefaultConfigData,
   pimsm6MigrateConfigData}
};


/*********************************************************************
*
* @purpose  Initialization for PIM-SM Mapping component task
*
* @param    @b{(input)} pCmdData 
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  pimsmMapMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;
  L7_RC_t count=0;

  memset (&pimsmGblVariables_g, 0, sizeof(pimsmGblVariables_t));
  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_0;

  /* Create a counting semaphore to indicate when a message is available to
   * be read in any of the message queues. */

  pimsmGblVariables_g.msgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);

  if (pimsmGblVariables_g.msgQSema == L7_NULL)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Semaphore Creation failed");  
    return L7_FAILURE;
  }

  /* Create the PIMSM-MAP Counters Semaphore */
  if ((pimsmGblVariables_g.queueCountersSemId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                                  OSAPI_SEM_FULL))
                        == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error Creating Counters Semaphore");
    osapiSemaDelete(pimsmGblVariables_g.msgQSema);

    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(pimsmGblVariables_g.pimsmQueue[PIMSM_APP_TIMER_Q].QName,
         PIMSMMAP_APP_TMR_QUEUE,sizeof(PIMSMMAP_APP_TMR_QUEUE));

  pimsmGblVariables_g.pimsmQueue[PIMSM_APP_TIMER_Q].QSize  = PIMSMMAP_APP_TMR_MSG_SIZE;
  pimsmGblVariables_g.pimsmQueue[PIMSM_APP_TIMER_Q].QCount = PIMSMMAP_APP_TMR_MSG_COUNT;

  memcpy(pimsmGblVariables_g.pimsmQueue[PIMSM_EVENT_Q].QName,
            PIMSMMAP_QUEUE,sizeof(PIMSMMAP_QUEUE));
  pimsmGblVariables_g.pimsmQueue[PIMSM_EVENT_Q].QSize      = PIMSMMAP_MSG_SIZE;
  pimsmGblVariables_g.pimsmQueue[PIMSM_EVENT_Q].QCount     = PIMSMMAP_MSG_COUNT;


  memcpy(pimsmGblVariables_g.pimsmQueue[PIMSM_CTRL_PKT_Q].QName,
            PIMSMMAP_CTRL_PKT_QUEUE,sizeof(PIMSMMAP_CTRL_PKT_QUEUE));
  pimsmGblVariables_g.pimsmQueue[PIMSM_CTRL_PKT_Q].QSize   = PIMSMMAP_CTRL_PKT_MSG_SIZE;
  pimsmGblVariables_g.pimsmQueue[PIMSM_CTRL_PKT_Q].QCount  = PIMSMMAP_CTRL_PKT_MSG_COUNT;


  memcpy(pimsmGblVariables_g.pimsmQueue[PIMSM_DATA_PKT_Q].QName,
            PIMSMMAP_DATA_PKT_QUEUE,sizeof(PIMSMMAP_DATA_PKT_QUEUE));
  pimsmGblVariables_g.pimsmQueue[PIMSM_DATA_PKT_Q].QSize   = PIMSMMAP_DATA_PKT_MSG_SIZE;
  pimsmGblVariables_g.pimsmQueue[PIMSM_DATA_PKT_Q].QCount  = PIMSMMAP_DATA_PKT_MSG_COUNT;

  for(count=0;count < PIMSM_MAX_Q;count++)
  {
    pimsmGblVariables_g.pimsmQueue[count].QPointer=
        (void *)osapiMsgQueueCreate(pimsmGblVariables_g.pimsmQueue[count].QName,
                                    pimsmGblVariables_g.pimsmQueue[count].QCount,
                                    pimsmGblVariables_g.pimsmQueue[count].QSize);

    if(pimsmGblVariables_g.pimsmQueue[count].QPointer == L7_NULLPTR)
    {
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);

      pimsmMapQDelete();
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "msgQueue creation error for Q %d\n",count);
      return L7_FAILURE;
    }
  }

  /* Create the PIMSM-MAP Counters Semaphore */
  if ((pimsmGblVariables_g.eventCountersSemId = osapiSemaBCreate (OSAPI_SEM_Q_FIFO,
                                                                  OSAPI_SEM_FULL))
                        == L7_NULLPTR)
  {
    pimsmMapQDelete();
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error Creating Counters Semaphore");
    return L7_FAILURE;
  }

  pimsmGblVariables_g.pimsmMapTaskId = osapiTaskCreate("pimsmMapTask", 
                                   pimsmMapTask, 0, 0,
                                   (pimsmMapSidDefaultStackSize() *2), 
                                   pimsmMapSidDefaultTaskPriority(), 
                                   pimsmMapSidDefaultTaskSlice());

  if ((pimsmGblVariables_g.pimsmMapTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit (L7_PIMSM_MAP_TASK_SYNC, 
                             L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    pimsmMapQDelete();
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error Creating PIMSM Map Task");
    return L7_FAILURE;
  }

  memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = PIMSMMAP_CNFGR_MSG;

  if (pimsmMessageSend(PIMSM_EVENT_Q,(L7_VOIDPTR)&msg) != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_BUSY;
    cnfgrApiCallback(&cbData);
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error in sending configurator request data");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  System Init Undo for PIM-SM Mapping component
*
* @param    none
*                            
* @comments none
*
* @end
*********************************************************************/
void pimsmMapTaskInitUndo()
{
  if (pimsmGblVariables_g.msgQSema != L7_NULLPTR)
  {
    osapiSemaDelete(pimsmGblVariables_g.msgQSema);
  }

  pimsmMapQDelete();
  if (pimsmGblVariables_g.pimsmMapTaskId != L7_ERROR)
  {
    osapiTaskDelete(pimsmGblVariables_g.pimsmMapTaskId);
  }

  /* Delete the PIMSM-MAP Counters Semaphore */
  if (pimsmGblVariables_g.eventCountersSemId != L7_NULLPTR)
  {
    osapiSemaDelete(pimsmGblVariables_g.eventCountersSemId);
  }

  /* Delete the PIMSM-MAP Counters Semaphore */
  if (pimsmGblVariables_g.queueCountersSemId != L7_NULLPTR)
  {
    osapiSemaDelete(pimsmGblVariables_g.queueCountersSemId);
  }
  

  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIM-SM component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the pimsm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimsmApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  pimsmMapMsg_t        msg;

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
      /* start the PIM-SM Map task to prepare for processing **
      ** configurator (and other) events                     */
      if (pimsmMapTaskInit(pCmdData) != L7_SUCCESS)
      {
        pimsmMapTaskInitUndo();
      }
    }
    else
    {
      /* send the configurator request to the task thread for processing */
      memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.msgId = PIMSMMAP_CNFGR_MSG;
      if (pimsmMessageSend(PIMSM_EVENT_Q,( L7_VOIDPTR)&msg) != L7_SUCCESS)
      {
        PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES, "\nMessae Send Failed" 
                        "for eventType.\n");
        return ;
      }
    }
  }
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for PIMSM component
*
* @param    pCmdData    @b{(output)}   Data structure for this   
*                                      CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the pimsm comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void pimsmCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             pimsmRC = L7_ERROR;
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
                if ((pimsmRC = pimsmCnfgrInitPhase1Process(&response, 
                               &reason )) != L7_SUCCESS)
                {
                  pimsmCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((pimsmRC = pimsmCnfgrInitPhase2Process(&response, 
                               &reason )) != L7_SUCCESS)
                {
                  pimsmCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((pimsmRC = pimsmCnfgrInitPhase3Process(&response, 
                     &reason)) != L7_SUCCESS)
                {
                  pimsmCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                pimsmRC = pimsmCnfgrNoopProcess( &response, &reason );
                pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_WMU;
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
                pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_EXECUTE;

                pimsmRC  = L7_SUCCESS;
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
                pimsmRC = pimsmCnfgrNoopProcess( &response, &reason );
                pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                pimsmRC = pimsmCnfgrUconfigPhase2( &response, &reason );
                pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            pimsmRC = pimsmCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = pimsmRC;
  if (pimsmRC == L7_SUCCESS)
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
* @purpose  This function allocates memory.
*
* @param    pimsmMapCbPtr @b{(input)}   Mapping Control Block.
* @param    pResponse     @b{(output)}  Response if L7_SUCCESS.
* @param    pReason       @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrControlBlockInit(pimsmMapCB_t *pimsmMapCbPtr,
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t  *pReason)
{
    if (pimsmMapCbPtr == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    *pReason =0;
    pimsmMapCbPtr->pPimsmMapCfgData = &pimsmMapCbPtr->pimsmMapCfgData;
    pimsmMapCbPtr->pPimsmMapCfgMapTbl = pimsmMapCbPtr->pimsmMapCfgMapTbl;
    pimsmMapCbPtr->pPimsmInfo = &pimsmMapCbPtr->pimsmInfo;
    pimsmMapCbPtr->pPimsmIntfInfo = pimsmMapCbPtr->pimsmIntfInfo;
    pimsmMapCbPtr->gblVars = &pimsmGblVariables_g;

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - If other failures.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. 
*           This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason )
{
  PIMSM_MAP_CB_t   cbIndex = L7_NULL;
  L7_uint32        protoMax = L7_NULL;
  pimsmMapCB_t     *pimsmMapCbPtr = L7_NULLPTR;

  
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  protoMax =  pimsmMapProtocolsMaxGet();

  pimsmMapCB = (pimsmMapCB_t *)osapiMalloc(L7_FLEX_PIMSM_MAP_COMPONENT_ID,sizeof(pimsmMapCB_t) * protoMax);

  if(pimsmMapCB == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Failed to allocate memory for CB structure");
    return L7_FAILURE;
  }


  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];
    memset(pimsmMapCbPtr, 0, sizeof(pimsmMapCB_t));
    pimsmMapCbPtr->familyType = pimsmConstsDefs[cbIndex].familyType;
    osapiStrncpy(pimsmMapCbPtr->cfgFilename, 
      pimsmConstsDefs[cbIndex].cfgFilename, L7_MAX_FILENAME);
    /* Intialize & allocate memory. */
    if (pimsmCnfgrControlBlockInit (pimsmMapCbPtr, pResponse, pReason)
                                 != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Initialization of "
                        "PIM-SM Mapping Layer Control Block failed");
      return L7_FAILURE;
    }

    pimsmMapCbPtr->cbIndex = cbIndex;

    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_APIS, 
                    "\n pimsmMapCbPtr->familyType = %d\n", 
                    pimsmMapCbPtr->familyType);
  }

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  pimsmDebugCfgRead();
  pimsmApplyDebugConfigData();

  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_1;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimsmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimsmMapMemoryInit (pimsmMapCB_t* pimsmMapCb)
{
  if(pimsmMapCb->pPimsmInfo->pimsmHeapAlloc == L7_TRUE)
  {
    /* calling this API second time is an error*/
    return L7_FAILURE;
  }

  /* Initialize the Memory for PIM-SM */
  if (pimsmMemoryInit (pimsmMapCb->familyType) != L7_SUCCESS)
  {
    PIMSM_MAP_LOG_MSG (pimsmMapCb->familyType,
                       "PIMSM Protocol Heap Memory Init Failed; Family - %d",
                       pimsmMapCb->familyType);
    return L7_FAILURE;
  }

  pimsmMapCb->pPimsmInfo->pimsmHeapAlloc = L7_TRUE;
  return L7_SUCCESS;  
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimsmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimsmMapMemoryDeInit (pimsmMapCB_t* pimsmMapCb)
{
  if(pimsmMapCb->pPimsmInfo->pimsmHeapAlloc == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* Initialize the Memory for PIM-SM */
  if (pimsmMemoryDeInit (pimsmMapCb->familyType) != L7_SUCCESS)
  {
    PIMSM_MAP_LOG_MSG (pimsmMapCb->familyType,
                       "PIMSM Protocol Heap Memory De-Init Failed; Family - %d",
                       pimsmMapCb->familyType);
    return L7_FAILURE;
  }

  pimsmMapCb->pPimsmInfo->pimsmHeapAlloc = L7_FALSE;
  return L7_SUCCESS;  
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    pimsmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrInitPhase1DynamicProcess (pimsmMapCB_t* pimsmMapCb)
{
  PIMSM_MAP_CB_t cbIndex;
  
  if(pimsmMapCb == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,
                     "Invalid PIM-SM Mapping Control Block Ptr");
    return L7_FAILURE;
  }


  /* Initialize the PIM-SM Control Block */
  pimsmMapCb->cbHandle = pimsmCtrlBlockInit(pimsmMapCb->familyType);
  if (pimsmMapCb->cbHandle == L7_NULLPTR)
  {
    PIMSM_MAP_LOG_MSG(pimsmMapCb->familyType,
                      "PIMSM Protocol Initialization Failed; Family - %d",
                      pimsmMapCb->familyType);
    pimsmMemoryDeInit (pimsmMapCb->familyType);
    return L7_FAILURE;
  }

  /* Regsiter with RTO for Unicast Routing Changes when PIM-SM Global
   * Admin Mode is Enabled.
   */
  cbIndex = pimsmMapCb->cbIndex;
  if (pimsmConstsDefs[cbIndex].rtoRegisterFn(pimsmConstsDefs[cbIndex].regName, 
                                             pimsmConstsDefs[cbIndex].rtoCallbackFn)
                            != L7_SUCCESS)
  {
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with RTO component");
      return L7_FAILURE;
  } 
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes pimsmCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmCnfgrFiniPhase1Process()
{
  L7_uint32        cbIndex = L7_NULL;
  L7_uint32        protoMax = L7_NULL;
  pimsmMapCB_t     *pimsmMapCbPtr = L7_NULLPTR;

  protoMax =  pimsmMapProtocolsMaxGet();
  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];

    /* De-initialize the Vendor PIM-DM component */
    pimsmCnfgrFiniPhase1DynamicProcess (pimsmMapCbPtr);
  }

  osapiFree(L7_FLEX_PIMSM_MAP_COMPONENT_ID,pimsmMapCB);
  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    pimsmMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrFiniPhase1DynamicProcess (pimsmMapCB_t* pimsmMapCb)
{
  L7_RC_t retVal = L7_SUCCESS;
  PIMSM_MAP_CB_t cbIndex;

  if(pimsmMapCb == L7_NULLPTR)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES,
                     "Invalid PIM-SM Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if(pimsmMapCb->pPimsmInfo->pimsmHeapAlloc == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* De-Initialize the PIM-SM Vendor Layer */
  if (pimsmCtrlBlockDeInit(pimsmMapCb->cbHandle) != L7_SUCCESS)
  {
    PIMSM_MAP_LOG_MSG (pimsmMapCb->familyType,
                      "PIMSM Protocol De-Initialization Failed; Family - %d",
                      pimsmMapCb->familyType); 
    retVal = L7_FAILURE;
  }

  /* De-Initialize the Memory for PIM-SM */
  if (pimsmMemoryDeInit (pimsmMapCb->familyType) != L7_SUCCESS)
  {
    PIMSM_MAP_LOG_MSG (pimsmMapCb->familyType,
                       "PIMSM Protocol Heap Memory De-Init Failed; Family - %d",
                       pimsmMapCb->familyType);
    retVal = L7_FAILURE;
  }

  /* De-Regsiter with RTO for Unicast Routing Changes when PIM-SM Global
   * Admin Mode is Disabled.
   */
  cbIndex = pimsmMapCb->cbIndex;
  if (pimsmConstsDefs[cbIndex].rtoDeRegisterFn 
       (pimsmConstsDefs[cbIndex].regName, 
        pimsmConstsDefs[cbIndex].rtoCallbackFn)
                               != L7_SUCCESS)
  {
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error de-registering with RTO component");
  } 

  pimsmMapCb->cbHandle = L7_NULLPTR;
  pimsmMapCb->pPimsmInfo->pimsmHeapAlloc = L7_FALSE;  
  return retVal;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - If other failures.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                     L7_CNFGR_ERR_RC_t *pReason )
{
  nvStoreFunctionList_t      notifyFunctionList;

  L7_uint32                  protoMax = L7_NULL;
  L7_uint32                  cbIndex = L7_NULL;
  pimsmMapCB_t               *pimsmMapCbPtr = L7_NULLPTR;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  /*----------------------*/
  /* nvStore registration */
  /*----------------------*/
  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID     = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave       = pimsmSave;
  notifyFunctionList.hasDataChanged   = pimsmHasDataChanged;
  notifyFunctionList.notifyConfigDump = pimsmMapCfgDataShow;
  notifyFunctionList.resetDataChanged = pimsmResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with NVStore");
    return L7_ERROR;
  }

  /********************************************
   * Register callback with MCAST mapping layer*
   ********************************************/
  /*register mcast mode change event */
  if (mcastMapRegisterAdminModeEventChange(L7_MRP_PIMSM,
      pimsmMapMcastEventCallBack) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with MCAST component");
    return L7_ERROR;
  }

  /********************************************
   * Register callback with MCAST-MAP         *
   * for AdminScope Changes                   *
   ********************************************/
  if (mcastMapRegisterAdminScopeEventChange (L7_MRP_PIMSM,
                                          pimsmMapAdminScopeEventChangeCallback)
                                       != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with MCAST component for \
                      Admin Scope Changes");
    return L7_ERROR;
  }

  /********************************************
   * Register callback with IGMP              *
   ********************************************/
  protoMax =  pimsmMapProtocolsMaxGet();
  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {  
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];   
    if (pimsmConstsDefs[cbIndex].ipMapRegisterFn
         (pimsmConstsDefs[cbIndex].ipMapRegId, 
          pimsmConstsDefs[cbIndex].regName, 
          pimsmConstsDefs[cbIndex].ipMapCallbackFn) != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with ipMap component");
      return L7_ERROR;
    }
  }

  /*--------------------------------------------------*/
  /* register callback with NIM for interface changes */
  /*--------------------------------------------------*/
  if (nimRegisterIntfChange(L7_FLEX_PIMSM_MAP_COMPONENT_ID, 
                            pimsmMapIntfChangeCallback) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error registering with NIM");
    return L7_ERROR;
  }

  /* Register with MFC */
  /* NOTE: This Registration will now be done when the Protocol is 
   * Enabled.
   */

  /* Register for debug */
  (void)pimsmDebugRegister();

  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_2;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function undoes pimsmpCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmCnfgrFiniPhase2Process()
{
  L7_uint32                  protoMax = L7_NULL;
  L7_uint32                  cbIndex = L7_NULL;
  pimsmMapCB_t               *pimsmMapCbPtr = L7_NULLPTR;

  (void)nvStoreDeregister(L7_FLEX_PIMSM_MAP_COMPONENT_ID);
  mcastMapDeregisterAdminModeEventChange(L7_MRP_PIMSM);
  protoMax =  pimsmMapProtocolsMaxGet();
  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];
    if (pimsmConstsDefs[cbIndex].ipMapDeRegisterFn
         (pimsmConstsDefs[cbIndex].ipMapRegId) != L7_SUCCESS)
    {
      PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, "Error de-registering with IpMap component");
    }
  }

  (void)nimDeRegisterIntfChange(L7_FLEX_PIMSM_MAP_COMPONENT_ID);

  /* De-register with MFC */
  /* NOTE: This De-Registration will now be done when the Protocol is 
   * Disabled.
   */
  /* Register for debug */
  (void)pimsmDebugDeRegister();

  /* De Register with IGMP also */
  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_1;
}


/*********************************************************************
* @purpose  This function reads configuration file.
*
* @param    pimsmMapCbPtr @b{(input)}   Mapping Control Block.
* @param    pResponse     @b{(output)}  Response if L7_SUCCESS.
* @param    pReason       @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS 
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
static L7_RC_t pimsmCnfgrConfigFileRead( L7_uint32 cbIndex, pimsmMapCB_t *pimsmMapCbPtr,
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rc = L7_FAILURE;

  rc = sysapiCfgFileGet(L7_FLEX_PIMSM_MAP_COMPONENT_ID, 
     pimsmMapCbPtr->cfgFilename, (L7_char8 *)pimsmMapCbPtr->pPimsmMapCfgData, 
     sizeof(L7_pimsmMapCfg_t),
     &pimsmMapCbPtr->pPimsmMapCfgData->checksum, L7_PIMSM_CFG_VER_CURRENT,
     pimsmConstsDefs[cbIndex].buildDefaultConfigData, 
     pimsmConstsDefs[cbIndex].migrateConfigData); 
  if (rc != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                      "Error reading configuration file");
    return L7_FAILURE;
  }
  if (pimsmApplyConfigData(pimsmMapCbPtr) != L7_SUCCESS)
  {
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    PIMSM_MAP_DEBUG (PIMSM_MAP_DEBUG_FAILURES, 
                      "Error applying configuration data");
    return L7_FAILURE;
  }
  pimsmMapCbPtr->pPimsmMapCfgData->cfgHdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread. 
*           This thread MUST NOT be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t *pReason )
{
  L7_RC_t       rc = L7_FAILURE;
  L7_uint32     cbIndex = L7_NULL;
  L7_uint32     protoMax = L7_NULL;
  pimsmMapCB_t  *pimsmMapCbPtr = L7_NULLPTR;

  

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;


  protoMax =  pimsmMapProtocolsMaxGet();
  for (cbIndex = PIMSM_MAP_IPV4_CB; cbIndex < protoMax; cbIndex++)
  {
    pimsmMapCbPtr = &pimsmMapCB[cbIndex];

    /* Read File. */
    rc = pimsmCnfgrConfigFileRead(cbIndex, pimsmMapCbPtr, pResponse, pReason);
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
  }

  pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_INIT_3;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function undoes pimsmCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void pimsmCnfgrFiniPhase3Process()
{
  if ((pimsmRestore() == L7_SUCCESS))
  {
    pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_WMU;
  }
  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @comments The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @comments This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, 
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pimsmRC = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if ((pimsmRC = pimsmRestore()) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
  }
  else
  {
    pimsmGblVariables_g.pimsmCnfgrState = PIMSM_PHASE_WMU;
  }

  return pimsmRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse - @b{(output)}  Response always command complete.
*     
* @param    pReason   - @b{(output)}  Always 0                    
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
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t pimsmCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t pimsmRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(pimsmRC);
}


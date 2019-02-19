/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename mgmd_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component MGMD Mapping component
*
* @comments 
*
* @create 06/23/2003
*
* @author jeffr
* @end
*
**********************************************************************/
#include "l7_mgmdmap_include.h"
#include "mgmdmap_sid.h" 
#include "mgmdmap_migrate.h"
#include "mfc_api.h"
#include "l7_mcast_api.h"
#include "heap_api.h"

extern void mgmdDebugRegister(void);
extern void mgmdDebugCfgRead(void);

static L7_RC_t mgmdMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData);
static void mgmdMapTaskInitUndo();
static L7_RC_t mgmdCnfgrControlBlockInit(mgmdMapCB_t *mgmdMapCbPtr, 
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason);
static L7_RC_t mgmdCnfgrConfigFileRead(mgmdMapCB_t *mgmdMapCbPtr, 
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason);
/*********************************************************************
*
* @purpose  CNFGR System Initialization for MGMD component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the mgmd comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mgmdApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  mgmdMapMsg_t        msg;
  L7_uint32           status;

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
      /* start the MGMD Map task to prepare for processing
       * configurator (and other) events
       */
      if (mgmdMapTaskInit(pCmdData) != L7_SUCCESS)
      {
        mgmdMapTaskInitUndo();
      }
    }
    else
    {
      /* send the configurator request to the task thread for processing */
      memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.msgId = MGMDMAP_CNFGR_MSG;

      if((status = mgmdMessageSend(MGMD_EVENT_Q,&msg)) != L7_SUCCESS)
      {
       return;
      }
    }
  }
}

/*********************************************************************
*
* @purpose  Initialization for MGMD Mapping component task
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
L7_RC_t mgmdMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  mgmdMapMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;
  L7_int32 QIndex=0;

  /* Intializing Global variables */
  memset (&mgmdMapGblVariables_g, 0, sizeof(mgmdMapGblVars_t));

  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_0;

  memcpy(mgmdMapGblVariables_g.mgmdQueue[MGMD_APP_TIMER_Q].QName,
            MGMDMAP_APPTIMER_QUEUE,sizeof(MGMDMAP_APPTIMER_QUEUE));
  mgmdMapGblVariables_g.mgmdQueue[MGMD_APP_TIMER_Q].QSize  = MGMDMAP_APPTIMER_Q_SIZE; 
  mgmdMapGblVariables_g.mgmdQueue[MGMD_APP_TIMER_Q].QCount = MGMDMAP_APPTIMER_Q_COUNT;
  memcpy(mgmdMapGblVariables_g.mgmdQueue[MGMD_EVENT_Q].QName,
            MGMDMAP_MSG_QUEUE,sizeof(MGMDMAP_MSG_QUEUE));
  mgmdMapGblVariables_g.mgmdQueue[MGMD_EVENT_Q].QSize      = MGMDMAP_MSG_Q_SIZE; 
  mgmdMapGblVariables_g.mgmdQueue[MGMD_EVENT_Q].QCount     = MGMDMAP_MSG_Q_COUNT;   
  memcpy(mgmdMapGblVariables_g.mgmdQueue[MGMD_CTRL_PKT_Q].QName,
            MGMDMAP_PKT_QUEUE,sizeof(MGMDMAP_PKT_QUEUE));
  mgmdMapGblVariables_g.mgmdQueue[MGMD_CTRL_PKT_Q].QSize   = MGMDMAP_PKT_Q_SIZE; 
  mgmdMapGblVariables_g.mgmdQueue[MGMD_CTRL_PKT_Q].QCount  = MGMDMAP_PKT_Q_COUNT; 



  mgmdMapGblVariables_g.mgmdMapMsgQSema = (void *) osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (mgmdMapGblVariables_g.mgmdMapMsgQSema == L7_NULLPTR)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failure creating MGMD message queue semaphore.");
    return L7_FAILURE;
  }

  for(QIndex=0;QIndex <MGMD_MAX_Q;QIndex++)
  {
    mgmdMapGblVariables_g.mgmdQueue[QIndex].QPointer=
        (void *)osapiMsgQueueCreate(mgmdMapGblVariables_g.mgmdQueue[QIndex].QName, 
                                    mgmdMapGblVariables_g.mgmdQueue[QIndex].QCount,
                                    mgmdMapGblVariables_g.mgmdQueue[QIndex].QSize);
    if(mgmdMapGblVariables_g.mgmdQueue[QIndex].QPointer == L7_NULLPTR)
    {
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "msgQueue creation error\n");
      return L7_FAILURE;
    }
  }      

  mgmdMapGblVariables_g.mgmdMapTaskId = osapiTaskCreate("mgmdMapTask",
                                  mgmdMapTask, 0, 0, 
                                  (mgmdMapSidDefaultStackSize() *2), 
                                  mgmdMapSidDefaultTaskPriority(), 
                                  mgmdMapSidDefaultTaskSlice());

  if ((mgmdMapGblVariables_g.mgmdMapTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit (L7_MGMD_MAP_TASK_SYNC, 
                             L7_WAIT_FOREVER) != L7_SUCCESS))
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to create task\n");
    return L7_FAILURE;
  }

  memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = MGMDMAP_CNFGR_MSG;


  if(mgmdMessageSend(MGMD_EVENT_Q,&msg) != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_BUSY;
    cnfgrApiCallback(&cbData);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to send message\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for MGMD Mapping component
*
* @param    none
*                            
* @comments none
*
* @end
*********************************************************************/
void mgmdMapTaskInitUndo()
{
  L7_int32 QIndex=0;
 
  for(QIndex=0;QIndex <MGMD_MAX_Q;QIndex++)
  {
    if (mgmdMapGblVariables_g.mgmdQueue[QIndex].QPointer != L7_NULLPTR)
    {
      osapiMsgQueueDelete(mgmdMapGblVariables_g.mgmdQueue[QIndex].QPointer);
    }
  }      

  if (mgmdMapGblVariables_g.mgmdMapTaskId != L7_ERROR)
  {
    osapiTaskDelete(mgmdMapGblVariables_g.mgmdMapTaskId);
  }

  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for MGMD component
*
* @param    pCmdData     @b{(input)} Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the mgmd comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void mgmdCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             mgmdRC = L7_ERROR;
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
                if ((mgmdRC = mgmdCnfgrInitPhase1Process(&response, 
                     &reason )) != L7_SUCCESS)
                {
                  mgmdCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((mgmdRC = mgmdCnfgrInitPhase2Process(&response, 
                     &reason )) != L7_SUCCESS)
                {
                  mgmdCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((mgmdRC = mgmdCnfgrInitPhase3Process(&response, 
                     &reason )) != L7_SUCCESS)
                {
                  mgmdCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                mgmdRC = mgmdCnfgrNoopProcess( &response, &reason );
                mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_WMU;
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
                mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_EXECUTE;

                mgmdRC  = L7_SUCCESS;
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
                mgmdRC = mgmdCnfgrNoopProcess( &response, &reason );
                mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                mgmdRC = mgmdCnfgrUconfigPhase2( &response, &reason );
                mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            mgmdRC = mgmdCnfgrNoopProcess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      } else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;

      } /* endif validate request */

    } else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;

    } /* endif validate command type */

  } else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;

  } /* check for command valid pointer */

  /* return value to caller - 
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = mgmdRC;
  if (mgmdRC == L7_SUCCESS)
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
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS  There were no errors. Response is available.
* @returns  L7_ERROR    There were errors. Reason code is available.
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
L7_RC_t mgmdCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse, 
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_uint32        cbIndex = L7_NULL;
  L7_uint32        protoMax = L7_NULL;
  L7_RC_t          rc = L7_FAILURE;
  mgmdMapCB_t      *mgmdMapCbPtr = L7_NULLPTR;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  /* Getting Max. protocols based on L7_IPV6_PACKAGE defined. Need 
   * to have two component Id's for v4 & v6.                        
   */
  protoMax =  mgmdMapProtocolsMaxGet();

  /* Allocate memory for enough CBs based on proto max */
  mgmdMapCB_g = (mgmdMapCB_t *)osapiMalloc(L7_FLEX_MGMD_MAP_COMPONENT_ID, 
                                           sizeof(mgmdMapCB_t) * MGMD_MAP_CB_MAX);

  if (mgmdMapCB_g == L7_NULLPTR) 
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Could not allocate memory for Control Blocks.\n");
    return L7_ERROR;
  }

  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0]; 
                       cbIndex < protoMax; cbIndex++, mgmdMapCbPtr++)
  {
    memset(mgmdMapCbPtr, 0, sizeof(mgmdMapCB_t));
    mgmdMapCbPtr->familyType = (cbIndex == L7_NULL)
                                     ? L7_AF_INET : L7_AF_INET6;
    /* Intialize & allocate memory. */
    rc = mgmdCnfgrControlBlockInit(mgmdMapCbPtr, pResponse, pReason);
    if (rc != L7_SUCCESS)
    {
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to initialize and allocate memory for control block\n");
      return rc;
    }
  }
  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  mgmdDebugCfgRead();
  mgmdApplyDebugConfigData();

  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_1;

  return L7_SUCCESS;

}


/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdMapMemoryInit(mgmdMapCB_t* mgmdMapCb)
{
  /* Initialize the Memory for PIM-DM */
  if (mgmdMemoryInit (mgmdMapCb->familyType) != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_ERROR, L7_FLEX_MGMD_MAP_COMPONENT_ID,
            "MGMD Protocol Heap Memory Init Failed; Family - %d"
            " MGMD Heap memory initialization is Failed for the"
            " specified address family. This message appears when"
            " trying to enable MGMD Protocol. ",
             mgmdMapCb->familyType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function de-allocates the memory for the Vendor protocol 
*           upon its Disable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdMapMemoryDeInit(mgmdMapCB_t* mgmdMapCb)
{
  /* Initialize the Memory for PIM-DM */
  if (mgmdMemoryDeInit (mgmdMapCb->familyType) != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_ERROR, L7_FLEX_MGMD_MAP_COMPONENT_ID,
        "MGMD Protocol Heap Memory De-Init Failed; Family - %d"
        " MGMD Heap memory de-initialization is Failed for the "
        "specified address family. This message appears when trying"
        " to disable MGMD (IGMP/MLD) Protocol.As a result of this,"
        " the subsequent attempts to enable/disable MGMD will also fail.",
             mgmdMapCb->familyType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;  
}

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrMgmdInitPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb)
{
  if(mgmdMapCb == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                     "Invalid MGMD Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if(mgmdMapCb->cbHandle != L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* Initialize the MGMD Control Block */
  mgmdMapCb->cbHandle = mgmdCtrlBlockInit(mgmdMapCb->familyType);
  if (mgmdMapCb->cbHandle == L7_NULLPTR)
  {
    L7_LOGF (L7_LOG_SEVERITY_ERROR, L7_FLEX_MGMD_MAP_COMPONENT_ID,
        "MGMD Protocol Initialization Failed; Family - %d"
        " MGMD protocol initialization sequence Failed. This could"
        " be due to the non-availability of some resources. This"
        " message appears when trying to enable MGMD Protocol.  ",
             mgmdMapCb->familyType);
    mgmdMemoryDeInit (mgmdMapCb->familyType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrProxyInitPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb)
{
  if(mgmdMapCb == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                     "Invalid MGMD Proxy Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if(mgmdMapCb->proxyCbHandle != L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  mgmdMapCb->proxyCbHandle = mgmdProxyCtrlBlockInit(mgmdMapCb->familyType);
  if (mgmdMapCb->proxyCbHandle == L7_NULLPTR)
  {
    L7_LOGF (L7_LOG_SEVERITY_ERROR, L7_FLEX_MGMD_MAP_COMPONENT_ID,
            "MGMD-Proxy Protocol Initialization Failed; Family - %d"
            "MGMD-Proxy protocol initialization sequence Failed. This"
            " could be due to the non-availability of some resources."
            " This message appears when trying to enable MGMD-Proxy Protocol.",
            mgmdMapCb->familyType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes mgmdCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCnfgrFiniPhase1Process()
{
  L7_uint32        cbIndex = L7_NULL;
  L7_uint32        protoMax = L7_NULL;
  mgmdMapCB_t      *mgmdMapCbPtr = L7_NULLPTR;

  protoMax =  mgmdMapProtocolsMaxGet();
  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0]; 
                          cbIndex < protoMax; cbIndex++, mgmdMapCbPtr++)
  {
    mgmdMapCbPtr->familyType = (cbIndex == L7_NULL)
                                     ? L7_AF_INET : L7_AF_INET6;
    /* De-initialize the Vendor MGMD component */
    mgmdCnfgrMgmdFiniPhase1DynamicProcess (mgmdMapCbPtr);
    mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCbPtr);

    if (mgmdMapCbPtr->mgmdHeapId != L7_NULL)
    {
      heapDestroy(mgmdMapCbPtr->mgmdHeapId, L7_FALSE);
      mgmdMapCbPtr->mgmdHeapId = L7_NULL;
    }
  } 

  memset (mgmdMapCbPtr->pMgmdInfo->mgmdGrpInfoNotifyList, 0, 
          sizeof(mgmdMapEventsNotifyList_t)*L7_MRP_MAXIMUM);
  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_0;

  osapiFree(L7_FLEX_MGMD_MAP_COMPONENT_ID, mgmdMapCB_g);
  mgmdMapCB_g = L7_NULLPTR;
}


/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrMgmdFiniPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb)
{
  L7_RC_t retVal = L7_SUCCESS;

  if(mgmdMapCb == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                     "Invalid MGMD Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if(mgmdMapCb->cbHandle == L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* De-Initialize the MGMD Vendor Layer */
  if (mgmdCtrlBlockDeInit(mgmdMapCb->cbHandle) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                     "Vendor Layer Dynamic De-Init Failed for Family - %d",
                     mgmdMapCb->familyType); 
    retVal = L7_FAILURE;
  }

  /* De-Initialize the Memory for PIM-SM */
  if (mgmdMemoryDeInit (mgmdMapCb->familyType) != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_ERROR, L7_FLEX_MGMD_MAP_COMPONENT_ID,
        "MGMD Protocol Heap Memory De-Init Failed; Family - %d"
        " MGMD Heap memory de-initialization is Failed for the "
        "specified address family. This message appears when trying"
        " to disable MGMD (IGMP/MLD) Protocol.As a result of this,"
        " the subsequent attempts to enable/disable MGMD will also fail.",
             mgmdMapCb->familyType);
    retVal = L7_FAILURE;
  }

  mgmdMapCb->cbHandle = L7_NULLPTR;
  return retVal;
}

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    mgmdMapCb  @b{(input)}   Mapping Control Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCB_t* mgmdMapCb)
{
  L7_RC_t retVal = L7_SUCCESS;

  if(mgmdMapCb == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                     "Invalid MGMD Proxy Mapping Control Block Ptr");
    return L7_FAILURE;
  }

  if(mgmdMapCb->proxyCbHandle == L7_NULLPTR)
  {
    return L7_SUCCESS;
  }

  /* De-Initialize the MGMD Vendor Layer */
  if (mgmdProxyCtrlBlockDeInit(mgmdMapCb->proxyCbHandle) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                     "Vendor Layer Dynamic De-Init Failed for Family - %d",
                     mgmdMapCb->familyType); 
    retVal = L7_FAILURE;
  }

  /* De-Initialize the Memory for PIM-SM */
  if (mgmdProxyMemoryDeInit (mgmdMapCb->familyType) != L7_SUCCESS)
  {
    L7_LOGF (L7_LOG_SEVERITY_ERROR, L7_FLEX_MGMD_MAP_COMPONENT_ID,
        "MGMD-Proxy Protocol Heap Memory De-Init Failed; Family - %d"
        "MGMD-Proxy Heap memory de-initialization is Failed for the"
        " specified address family. This message appears when trying"
        " to disable MGMD-Proxy Protocol. As a result of this, the"
        " subsequent attempts to enable/disable MGMD-Proxy will also fail.",
             mgmdMapCb->familyType);
    retVal = L7_FAILURE;
  }

  mgmdMapCb->proxyCbHandle = L7_NULLPTR;
  return retVal;
}


/*********************************************************************
* @purpose  This function allocates memory.
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    pResponse           @b{(output)}  Response if L7_SUCCESS.
* @param    pReason             @b{(output)}  Reason if L7_ERROR.         
*     
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrControlBlockInit(mgmdMapCB_t *mgmdMapCbPtr, 
  L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason)
{

  mgmdMapCbPtr->pMgmdMapCfgData = &(mgmdMapCbPtr->mgmdMapCfgData);
  memset(( void * )mgmdMapCbPtr->pMgmdMapCfgData, 0,
                   sizeof(L7_mgmdMapCfg_t));

  mgmdMapCbPtr->pMgmdMapCfgMapTbl  = mgmdMapCbPtr->mgmdMapCfgMapTbl;
  memset(( void * )mgmdMapCbPtr->pMgmdMapCfgMapTbl, 
                            0, sizeof(L7_uint32) * L7_MAX_INTERFACE_COUNT);

  mgmdMapCbPtr->pMgmdInfo = &(mgmdMapCbPtr->mgmdInfo);
  memset(( void * )mgmdMapCbPtr->pMgmdInfo, 0, sizeof( mgmdInfo_t));

  memset (mgmdMapCbPtr->pMgmdInfo->mgmdGrpInfoNotifyList, 0, 
          sizeof(mgmdMapEventsNotifyList_t)*L7_MRP_MAXIMUM);
    /* Create the Multicast Heap */
  mgmdMapCbPtr->mgmdHeapId = heapCreate(L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                               MGMDMAP_HEAP_SIZE_GET(mgmdMapCbPtr->familyType));
  if( mgmdMapCbPtr->mgmdHeapId == L7_NULL)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "\nMGMD_MAP: failed to create multicast heap.\n");
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_FAILURE;
  }

  /*------------------------*/
  /*     Start Application  */
  /*------------------------*/
  mgmdMapCbPtr->pMgmdInfo->initialized = L7_FALSE;

  /* Updating Global Variable Pointer */
  mgmdMapCbPtr->gblVars = &mgmdMapGblVariables_g;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.         
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
* @comments This function runs in the configurator's thread.
*           This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  nvStoreFunctionList_t notifyFunctionList;
  L7_RC_t mgmdRC = L7_SUCCESS;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

/*---------------------------*/
/*     nvStore Registration  */
/*---------------------------*/

  notifyFunctionList.registrar_ID   = L7_FLEX_MGMD_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave     = mgmdSave;
  notifyFunctionList.hasDataChanged = mgmdHasDataChanged;
  notifyFunctionList.notifyConfigDump = L7_NULLPTR;
  notifyFunctionList.notifyDebugDump = L7_NULLPTR;
  notifyFunctionList.resetDataChanged = mgmdResetdataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    mgmdRC = L7_ERROR;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error registering with nvStoreRegister().\n");
  }

  /*------------------------------------------*/
  /* register callbacks with IP Mapping layer */
  /*------------------------------------------*/
  /* register MGMD to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes                                       
   */
  if (ipMapRegisterRoutingEventChange(L7_IPRT_MGMD, 
                                   "mgmdMapRoutingEventCallback",
                                   mgmdMapRoutingEventCallback) != L7_SUCCESS)
  {
    mgmdRC = L7_ERROR;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error registering with nvStoreRegister().\n");
  }
  /********************************************
   * Register callback with MCAST mapping layer*
   ********************************************/

  /*register mcast mode change event */
  if (mcastMapRegisterAdminModeEventChange((L7_uint32)L7_MRP_MGMD_PROXY,
      mgmdMapMcastEventCallback) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Unable to register callback to MCAST.\n");
    return L7_ERROR;
  }

  /********************************************
  * Register callback with MCAST-MAP         *
  * for AdminScope Changes                   *
  ********************************************/
  if (mcastMapRegisterAdminScopeEventChange (L7_MRP_MGMD_PROXY,
                                          mgmdMapAdminScopeEventChangeCallback)
                                       != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error registering with MCAST component for \
                      Admin Scope Changes");
    return L7_ERROR;
  }
  /*--------------------------------------------------*/
  /* register callback with NIM for interface changes */
  /*--------------------------------------------------*/
  if (nimRegisterIntfChange(L7_FLEX_MGMD_MAP_COMPONENT_ID,
        mgmdMapIntfChangeCallback) != L7_SUCCESS)
  {
    mgmdRC = L7_ERROR;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Unable to register MGMD Map callback with NIM\n");
  }
  /* Register MGMD to receive notification of routing events. */
  if (mgmdMapV6Register(L7_IPRT_MGMD,
                        "mgmd6MapRoutingEventChangeCallBack",
                        mgmd6MapRoutingEventCallback) != L7_SUCCESS)
  {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error registering with ip6Map.\n");
      return L7_FAILURE;
  }

  /* Register for debug */
  (void)mgmdDebugRegister();

  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_2;

  return mgmdRC;
}
/*********************************************************************
* @purpose  This function undoes mgmdCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_FLEX_MGMD_MAP_COMPONENT_ID);
  (void)ipMapDeregisterRoutingEventChange(L7_IPRT_MGMD);
   mcastMapDeregisterAdminModeEventChange(L7_MRP_MGMD_PROXY); 
   mcastMapDeregisterAdminScopeEventChange(L7_MRP_MGMD_PROXY);
  (void)nimDeRegisterIntfChange(L7_FLEX_MGMD_MAP_COMPONENT_ID);
  (void)mgmdMapV6Deregister(L7_IPRT_MGMD);

  (void)mgmdDebugDeRegister();

  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_1;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse    @b{(output)}  Response if L7_SUCCESS.
* @param    pReason      @b{(output)}  Reason if L7_ERROR.         
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
* @comments This function runs in the configurator's thread.This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t      mgmdRC = L7_SUCCESS;
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  protoMax =  mgmdMapProtocolsMaxGet();

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0]; 
                       cbIndex < protoMax; cbIndex++, mgmdMapCbPtr++)
  {
    /* Intialize & allocate memory. */
    mgmdRC = mgmdCnfgrConfigFileRead(mgmdMapCbPtr, pResponse, pReason);
    if (mgmdRC != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to read configuration file\n");
      return mgmdRC;
    }
  }

  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_INIT_3;

  return mgmdRC;
}

/*********************************************************************
* @purpose  This function reads configuration file.
*
* @param    mgmdMapCbPtr        @b{(input)}   Mapping Control Block.
* @param    pResponse           @b{(output)}  Response if L7_SUCCESS.
* @param    pReason             @b{(output)}  Reason if L7_ERROR.         
*     
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrConfigFileRead(mgmdMapCB_t *mgmdMapCbPtr, 
                 L7_CNFGR_RESPONSE_t *pResponse, L7_CNFGR_ERR_RC_t   *pReason)
{
  /*L7_RC_t mgmdRC = L7_FAILURE;*/
  L7_RC_t rc = L7_FAILURE;

  if (mgmdMapCbPtr->familyType == L7_AF_INET)
  {
       rc = sysapiCfgFileGet(L7_FLEX_MGMD_MAP_COMPONENT_ID, 
          L7_IGMP_CFG_FILENAME,
          (L7_char8 *)mgmdMapCbPtr->pMgmdMapCfgData, 
          sizeof(L7_mgmdMapCfg_t),
          &mgmdMapCbPtr->pMgmdMapCfgData->checkSum, 
          L7_MGMD_CFG_VER_CURRENT, mgmdBuildDefaultConfigData, 
          mgmdMigrateConfigData);
  }
  else
  {
      rc  = sysapiCfgFileGet(L7_FLEX_MGMD_MAP_COMPONENT_ID, 
          L7_MLD_CFG_FILENAME,
          (L7_char8 *)mgmdMapCbPtr->pMgmdMapCfgData, 
          sizeof(L7_mgmdMapCfg_t),
          &mgmdMapCbPtr->pMgmdMapCfgData->checkSum, 
          L7_MGMD_CFG_VER_CURRENT, mgmd6BuildDefaultConfigData, 
          mgmdMigrateConfigData);
  }

  if (rc != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error retrieving config sysapiCfgFileGet().\n");
    return L7_FAILURE;
  }

  if (mgmdApplyConfigData(mgmdMapCbPtr) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error applying config mgmdApplyConfigData().\n");
    return L7_FAILURE;
  }
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function undoes mgmdCnfgrInitPhase3Process
*
* @param    none
*     
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCnfgrFiniPhase3Process()
{
  mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_WMU;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse   @b{(output)}  Response if L7_SUCCESS.
* @param    pReason     @b{(output)}  Reason if L7_ERROR.         
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
* @comments This function runs in the configurator's thread.This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse, 
                                L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t mgmdRC = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if ((mgmdRC = mgmdRestore()) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
  }
  else
  {
    mgmdMapGblVariables_g.mgmdCnfgrState = MGMD_PHASE_WMU;
  }

  return mgmdRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse   @b{(output)}  Response always command complete.
* @param    pReason     @b{(output)}  Always 0                    
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
* @comments This function runs in the configurator's thread.This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t mgmdCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t mgmdRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(mgmdRC);
}

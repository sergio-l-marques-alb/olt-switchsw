/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component 
*
* @comments 
*
* @create 06/23/2003
*
* @author jeffr
* @end
*
**********************************************************************/
#include "l7_common_l3.h" 
#include "l3_mcast_commdefs.h" 
#include "l3_mcast_defaultconfig.h" 
#include "dvmrp_api.h"                   
#include "l7_ip_api.h"                    
#include "l3_mcast_default_cnfgr.h" 
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h" 
#include "dvmrp_map_util.h"
#include "dvmrp_map.h"
#include "dvmrp_vend_ctrl.h" 
#include "dvmrp_map_debug.h" 
#include "dvmrp_migrate.h" 
#include "dvmrpmap_sid.h" 
#include "dvmrp_api.h" 
#include "l7_mcast_api.h" 
#include "l7_mgmd_api.h" 


static L7_RC_t dvmrpMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData);
static void dvmrpMapTaskInitUndo();

/*********************************************************************
*
* @purpose  CNFGR System Initialization for DVMRP component
*
* @param    pCmdData    @b{(input)}  Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the dvmrp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dvmrpApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  dvmrpMapMsg_t        msg;

/*
 * Let all but PHASE 1 start fall through into an osapiMessageSend.
 * The mapping task will handle everything.
 */
  if (pCmdData == L7_NULLPTR)
  {
    L7_CNFGR_CB_DATA_t  cbData;

    /* game over... pCmdData is a null pointer */
    cbData.correlator             = L7_NULL;
    cbData.asyncResponse.rc       = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
  }
  else
  {
    if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) && 
        (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
    {
      /* start the DVMRP Map task to prepare for processing configurator 
       * (and other) events 
       */
      if (dvmrpMapTaskInit(pCmdData) != L7_SUCCESS)
      {
        dvmrpMapTaskInitUndo();
      }
    }
    else
    {
      /* send the configurator request to the task thread for processing */
      memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
      msg.msgId = DVMRPMAP_CNFGR_MSG;

      if(dvmrpMessageSend(DVMRP_EVENT_Q,&msg) != L7_SUCCESS)
      {
        return;
      }


    }
  }
}

/*********************************************************************
*
* @purpose  Initialization for DVMRP Mapping component task
*
* @param    pCmdData    @b{(input)}  Data structure for this   
*                                    CNFGR request
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dvmrpMapTaskInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  dvmrpMapMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;
  L7_int32 QIndex=0;

  memset (&dvmrpGblVar_g, 0, sizeof(dvmrpGblVaribles_t));
  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_0;

  /* initialize protocol data-structures */


  memcpy(dvmrpGblVar_g.dvmrpQueue[DVMRP_APP_TIMER_Q].QName,
            DVMRPMAPAPPTIMER_QUEUE,sizeof(DVMRPMAPAPPTIMER_QUEUE));
  dvmrpGblVar_g.dvmrpQueue[DVMRP_APP_TIMER_Q].QSize  = DVMRPMAP_APPTIMER_SIZE; 
  dvmrpGblVar_g.dvmrpQueue[DVMRP_APP_TIMER_Q].QCount = DVMRPMAP_APPTIMER_COUNT;
  memcpy(dvmrpGblVar_g.dvmrpQueue[DVMRP_EVENT_Q].QName,
            DVMRPMAPMSG_QUEUE,sizeof(DVMRPMAPMSG_QUEUE));
  dvmrpGblVar_g.dvmrpQueue[DVMRP_EVENT_Q].QSize      = DVMRPMAP_MSG_SIZE; 
  dvmrpGblVar_g.dvmrpQueue[DVMRP_EVENT_Q].QCount     = DVMRPMAP_MSG_COUNT;   
  memcpy(dvmrpGblVar_g.dvmrpQueue[DVMRP_CTRL_PKT_Q].QName,
            DVMRPMAPPKT_QUEUE,sizeof(DVMRPMAPPKT_QUEUE));
  dvmrpGblVar_g.dvmrpQueue[DVMRP_CTRL_PKT_Q].QSize   = DVMRPMAP_PKT_SIZE; 
  dvmrpGblVar_g.dvmrpQueue[DVMRP_CTRL_PKT_Q].QCount  = DVMRPMAP_PKT_COUNT; 
  

  dvmrpGblVar_g.msgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (dvmrpGblVar_g.msgQSema == L7_NULL)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Failure creating DVMRP message queue semaphore\n.");
    return L7_FAILURE;
  }

  for(QIndex=0;QIndex < DVMRP_MAX_Q;QIndex++)
  {
    dvmrpGblVar_g.dvmrpQueue[QIndex].QPointer=
        (void *)osapiMsgQueueCreate(dvmrpGblVar_g.dvmrpQueue[QIndex].QName, 
                                    dvmrpGblVar_g.dvmrpQueue[QIndex].QCount,
                                    dvmrpGblVar_g.dvmrpQueue[QIndex].QSize);
    if(dvmrpGblVar_g.dvmrpQueue[QIndex].QPointer == L7_NULLPTR)
    {
      cbData.correlator = pCmdData->correlator;
      cbData.asyncResponse.rc = L7_FAILURE;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "msgQueue creation error\n");
      return L7_FAILURE;
    }
  }      

  dvmrpGblVar_g.dvmrpMapTaskId = osapiTaskCreate("dvmrpMapTask", 
                                                 dvmrpMapTask, 0, 0, 
                                                 (dvmrpMapSidDefaultStackSize()*2) , 
                                                 dvmrpMapSidDefaultTaskPriority(), 
                                                 dvmrpMapSidDefaultTaskSlice());

  if ((dvmrpGblVar_g.dvmrpMapTaskId == L7_ERROR) ||
      (osapiWaitForTaskInit (L7_DVMRP_MAP_TASK_SYNC, L7_WAIT_FOREVER) != 
       L7_SUCCESS))
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to create  DVMRP Mapping Task\n"); 
    return L7_FAILURE;
  }


  memcpy(&msg.u.cnfgrCmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = DVMRPMAP_CNFGR_MSG;

  if(dvmrpMessageSend(DVMRP_EVENT_Q,&msg) != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_BUSY;
    cnfgrApiCallback(&cbData);
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Failed to send message\n");
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for DVMRP Mapping component
*
* @param    none
*                            
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpMapTaskInitUndo()
{
  L7_int32 QIndex=0;
 
  for(QIndex=0;QIndex <DVMRP_MAX_Q;QIndex++)
  {
    if (dvmrpGblVar_g.dvmrpQueue[QIndex].QPointer != L7_NULLPTR)
    {
      osapiMsgQueueDelete(dvmrpGblVar_g.dvmrpQueue[QIndex].QPointer);
    }
  }      
  if (dvmrpGblVar_g.dvmrpMapTaskId != L7_ERROR)
  {
    osapiTaskDelete(dvmrpGblVar_g.dvmrpMapTaskId);
  }
  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for DVMRP component
*
* @param    pCmdData    @b{(input)}  Data structure for this   
*                                    CNFGR request
*                            
* @returns  void
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the dvmrp comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void dvmrpCnfgrCommandProcess(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             dvmrpRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  /* validate command type */
  if (pCmdData != L7_NULL)
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if (request > L7_CNFGR_RQST_FIRST && 
          request < L7_CNFGR_RQST_LAST)
      {
        /* validate command/event pair */
        switch (command)
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch (request)
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                if ((dvmrpRC = dvmrpCnfgrInitPhase1Process(&response, 
                                                           &reason)) != L7_SUCCESS)
                {
                  dvmrpCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((dvmrpRC = dvmrpCnfgrInitPhase2Process( &response, 
                                                            &reason )) != L7_SUCCESS)
                {
                  dvmrpCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((dvmrpRC = dvmrpCnfgrInitPhase3Process( &response, 
                                                            &reason )) != L7_SUCCESS)
                {
                  dvmrpCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                dvmrpRC = dvmrpCnfgrNoopProcess( &response, &reason );
                dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_WMU;
                break;
              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            switch (request)
            {
              case L7_CNFGR_RQST_E_START:
                dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_EXECUTE;

                dvmrpRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = 0;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:
            switch (request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                dvmrpRC = dvmrpCnfgrNoopProcess( &response, &reason );
                dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                dvmrpRC = dvmrpCnfgrUconfigPhase2( &response, &reason );
                dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            dvmrpRC = dvmrpCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = dvmrpRC;
  if (dvmrpRC == L7_SUCCESS)
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
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
* @returns  L7_FAILURE - In case of other failures.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t *pReason )
{

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;


  /*----------------------------------------------*/
  /* malloc space for various DVMRP info structures */
  /*----------------------------------------------*/

  dvmrpGblVar_g.family = L7_AF_INET;

  /* Read and apply trace configuration at phase 1.
     This allows for tracing during system initialization and
     during clear config */
  dvmrpDebugCfgRead();
  dvmrpApplyDebugConfigData();

  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_1;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    dvmrpGblVar_g  @b{(input)}   DVMRP Global Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dvmrpMapMemoryInit (dvmrpGblVaribles_t *pDvmrpGblVar)
{

  if(pDvmrpGblVar->heapAlloc == L7_TRUE)
  {
    /* calling this API second time is an error*/
    return L7_FAILURE;
  }

  /* Initialize the Memory for DVMRP */
  if (dvmrpMemoryInit (pDvmrpGblVar->family) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
           "DVMRP Heap memory initialization is Failed for the specified address family."
           " This message appears when trying to enable DVMRP Protocol.");
    return L7_FAILURE;
  }

  pDvmrpGblVar->heapAlloc = L7_TRUE; 
  return L7_SUCCESS;   
}

/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    dvmrpGblVar_g  @b{(input)}   DVMRP Global Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dvmrpMapMemoryDeInit (dvmrpGblVaribles_t *pDvmrpGblVar)
{

  if(dvmrpGblVar_g.heapAlloc == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* Initialize the Memory for DVMRP */
  if (dvmrpMemoryDeInit (pDvmrpGblVar->family) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
           "DVMRP Heap memory de-initialization is Failed for the specified address family."
           " This message appears when trying to disable DVMRP Protocol."
           " As a result of this, the subsequent attempts to enable/disable DVMRP will also fail.");
    return L7_FAILURE;
  }

  pDvmrpGblVar->heapAlloc = L7_FALSE; 
  return L7_SUCCESS;   
}
/*********************************************************************
* @purpose  This function allocates the memory for the Vendor protocol 
*           upon its Enable.
*
* @param    dvmrpGblVar_g  @b{(input)}   DVMRP Global Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrInitPhase1DynamicProcess (dvmrpGblVaribles_t *pDvmrpGblVar)
{
  if(pDvmrpGblVar->family != L7_AF_INET)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid Address Family");
    return L7_FAILURE;
  }
 
  /* Initialize the Control Block of the Vendor DVMRP component */
  if (dvmrp_init (&pDvmrpGblVar->dvmrpCb) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
            "DVMRP protocol initialization sequence Failed."
            " This could be due to the non-availability of some resources."
            " This message appears when trying to enable DVMRP Protocol.  ");
    dvmrpMemoryDeInit (pDvmrpGblVar->family);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function undoes dvmrpCnfgrInitPhase1Process
*
* @param    none
*     
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpCnfgrFiniPhase1Process()
{
  dvmrpCnfgrFiniPhase1DynamicProcess (&dvmrpGblVar_g);
  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function De-allocates the memory for the Vendor
*           Protocol upon its Disable.
*
* @param    dvmrpGblVar_g  @b{(input)}   DVMRP Global Block.
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrFiniPhase1DynamicProcess (dvmrpGblVaribles_t *pDvmrpGblVar)
{
  if(pDvmrpGblVar == L7_NULLPTR)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid CB Pointer");
    return L7_FAILURE;
  }

  if(dvmrpGblVar_g.heapAlloc == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if(pDvmrpGblVar->family != L7_AF_INET)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Invalid Address Family - %d",
                     pDvmrpGblVar->family);
    return L7_FAILURE;
  }

  /* De-Initialize the DVMRP Vendor Layer */
  dvmrpClearInit (pDvmrpGblVar->dvmrpCb);

  /* De-Initialize the Memory for DVMRP */
  if (dvmrpMemoryDeInit (pDvmrpGblVar->family) != L7_SUCCESS)
  {
    L7_LOG(L7_LOG_SEVERITY_ERROR, L7_FLEX_DVMRP_MAP_COMPONENT_ID,
           "DVMRP Heap memory de-initialization is Failed for the specified address family."
           " This message appears when trying to disable DVMRP Protocol."
           " As a result of this, the subsequent attempts to enable/disable DVMRP will also fail.");
    return L7_FAILURE;
  }

  pDvmrpGblVar->dvmrpCb = L7_NULLPTR;  
  pDvmrpGblVar->heapAlloc =L7_FALSE;

  return L7_SUCCESS;
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
* @returns  L7_FAILURE - In case of other failures.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t *pReason )
{
  L7_RC_t dvmrpRC;
  nvStoreFunctionList_t      notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  dvmrpRC    = L7_SUCCESS;

  /*----------------------*/
  /* nvStore registration */
  /*----------------------*/
  notifyFunctionList.registrar_ID   = L7_FLEX_DVMRP_MAP_COMPONENT_ID;
  notifyFunctionList.notifySave     = dvmrpSave;
  notifyFunctionList.hasDataChanged = dvmrpHasDataChanged;
  notifyFunctionList.notifyConfigDump = L7_NULLPTR;
  notifyFunctionList.notifyDebugDump = L7_NULLPTR;
  notifyFunctionList.resetDataChanged = dvmrpResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES, "Error registering with NVStore");
    return L7_ERROR;
  }

  /*------------------------------------------*/
  /* register callbacks with IP Mapping layer */
  /*------------------------------------------*/
  /* register DVMRP to receive a callback when an interface is enabled
   * or disabled for routing, or when the IP address of a router
   * interface changes
   */
  if (ipMapRegisterRoutingEventChange(L7_IPRT_DVMRP, 
                                      "dvmrpMapRoutingEventCallBack", 
                                      dvmrpMapRoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                  "Error registering for routing intf state change callback\n");
    return L7_ERROR;
  }

  /********************************************
   * Register callback with MFC               *
   ********************************************/
  /* NOTE: This Registration will now be done when the Protocol is 
   * Enabled.
   */

  /*register mcast mode change event */
  if (mcastMapRegisterAdminModeEventChange(L7_MRP_DVMRP,
                                           dvmrpMapMcastEventCallBack) != 
      L7_SUCCESS)
  {
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Error registering mode change event callback\n");
    return L7_FAILURE;
  }

  /********************************************
   * Register callback with MCAST-MAP         *
   * for AdminScope Changes                   *
   ********************************************/
  if (mcastMapRegisterAdminScopeEventChange (L7_MRP_DVMRP,
                                             dvmrpMapAdminScopeCallback)
      != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Unable to register callback with MCASTMAP.\n");
    return L7_ERROR;
  }

  /*--------------------------------------------------*/
  /* register callback with NIM for interface changes */
  /*--------------------------------------------------*/
  if (nimRegisterIntfChange(L7_FLEX_DVMRP_MAP_COMPONENT_ID, 
                            dvmrpMapIntfChangeCallback)
      != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Unable to register callback with NIM.\n");
    return L7_ERROR;
  }

  /* Register for debug */
  (void)dvmrpDebugRegister();

  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_2;

  return dvmrpRC;
}

/*********************************************************************
* @purpose  This function undoes dvmrppCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_FLEX_DVMRP_MAP_COMPONENT_ID);
  (void)ipMapDeregisterRoutingEventChange(L7_IPRT_DVMRP);

  /* De-register with MFC */
  /* NOTE: This De-Registration will now be done when the Protocol is 
   * Disabled.
   */

  mcastMapDeregisterAdminModeEventChange(L7_MRP_DVMRP); 
  (void)nimDeRegisterIntfChange(L7_FLEX_DVMRP_MAP_COMPONENT_ID);

  /* Register for debug */
  (void)dvmrpDebugDeRegister();

  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_1;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread 
*           MUST NOT be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t *pReason )
{
  L7_RC_t dvmrpRC = L7_SUCCESS;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  if (sysapiCfgFileGet(L7_FLEX_DVMRP_MAP_COMPONENT_ID, L7_DVMRP_CFG_FILENAME, 
                       (L7_char8 *)&dvmrpGblVar_g.DvmrpMapCfgData, sizeof(L7_dvmrpMapCfg_t),
                       &dvmrpGblVar_g.DvmrpMapCfgData.checkSum, L7_DVMRP_CFG_VER_CURRENT,
                       dvmrpBuildDefaultConfigData, dvmrpMigrateConfigData) != L7_SUCCESS)
  {
    dvmrpRC = L7_ERROR;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Error retrieving config sysapiCfgFileGet()\n");
    return dvmrpRC;
  }
  if (dvmrpApplyConfigData() != L7_SUCCESS)
  {
    dvmrpRC = L7_ERROR;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    dvmrpRC   = L7_ERROR;
    DVMRP_MAP_DEBUG (DVMRP_MAP_DEBUG_FAILURES,
                     "Error applying config dvmrpApplyConfigData().\n");
    return dvmrpRC;
  }

  dvmrpGblVar_g.DvmrpMapCfgData.cfgHdr.dataChanged = L7_FALSE;

  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_INIT_3;

  return dvmrpRC;
}
/*********************************************************************
* @purpose  This function undoes dvmrpCnfgrInitPhase2Process
*
* @param    none
*     
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dvmrpCnfgrFiniPhase3Process()
{

  if (dvmrpRestore() == L7_SUCCESS)
  {
    dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_WMU;
  }

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse  @b{(output)}  Response if L7_SUCCESS.
* @param    pReason    @b{(output)}  Reason if L7_ERROR.         
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t dvmrpRC = L7_SUCCESS;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if ((dvmrpRC = dvmrpRestore()) != L7_SUCCESS)
  {
    *pReason = L7_CNFGR_ERR_RC_FATAL;
  }
  dvmrpGblVar_g.dvmrpCnfgrState = DVMRP_PHASE_WMU;

  return dvmrpRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.   
*
* @param    pResponse  @b{(output)}  Response always command complete.
* @param    pReason    @b{(output)}  Always 0                    
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response: 
*           L7_CNFGR_CMD_COMPLETE 
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*       
* @end
*********************************************************************/
L7_RC_t dvmrpCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t dvmrpRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  return(dvmrpRC);
}

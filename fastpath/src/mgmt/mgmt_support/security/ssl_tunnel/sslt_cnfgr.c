/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename sslt_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component sslt
*
* @comments none
*
* @create 10/22/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "cnfgr.h"
#include "sslt_cfg.h"
#include "osapi.h"

ssltCnfgrState_t ssltCnfgrState = SSLT_PHASE_INIT_0;
extern ssltDeregister_t ssltDeregister;

extern ssltCfg_t *ssltCfg;
extern L7_int32 ssltTaskId;
extern void *ssltQueue;

extern void *ssltCertSema;
extern void *ssltTaskSyncSema;

/*********************************************************************
*
* @purpose  CNFGR System Initialization
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the SSLT comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void ssltApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /*
   * Let all but PHASE 1 start fall through into an osapiMessageSend.
   * The application task will handle everything.
   * Phase 1 will do a osapiMessageSend after a few pre reqs have been
   * completed
   */

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* this function will invoke the message send for phase 1 */
    if (ssltInit(pCmdData) != L7_SUCCESS)
    {
      ssltInitUndo();
    }
  }
  else
  {
    if (ssltIssueCmd(SSLT_CNFGR_EVENT, pCmdData) != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID, "ssltApiCnfgrCommand: Failed calling "
                               "ssltIssueCmd. Failed to send the message to the SSLT message queue.\n");
  }
}


/*********************************************************************
*
* @purpose  System Initialization for SSLT component
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t ssltInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CB_DATA_t cbData;

  ssltQueue = (void*)osapiMsgQueueCreate("ssltQueue",
                                         FD_CNFGR_SSLT_MSG_COUNT,
                                         (L7_uint32)sizeof(ssltMsg_t));
  if (ssltQueue == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
            "ssltInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (ssltStartTasks() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (ssltIssueCmd(SSLT_CNFGR_EVENT, pCmdData) != L7_SUCCESS)
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
            "ssltInit: Failed calling ssltIssueCmd.\n");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for SSLT component
*
* @comments none
*
* @end
*********************************************************************/
void ssltInitUndo()
{
  if (ssltQueue != L7_NULL)
  {
    osapiMsgQueueDelete(ssltQueue);
  }

  if (ssltTaskId != L7_ERROR)
  {
    osapiTaskDelete(ssltTaskId);
  }

  ssltCnfgrState = SSLT_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
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
* @end
*********************************************************************/
L7_RC_t ssltCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t ssltRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  ssltRC      = L7_SUCCESS;

  ssltCfg = osapiMalloc(L7_FLEX_SSLT_COMPONENT_ID, sizeof(ssltCfg_t));
  if (ssltCfg == L7_NULL)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    ssltRC     = L7_ERROR;

    return ssltRC;
  }

  ssltCertSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (ssltCertSema == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  memset((void *)ssltCfg, L7_NULL, sizeof(ssltCfg_t));
  ssltCnfgrState = SSLT_PHASE_INIT_1;

  return ssltRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
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
* @end
*********************************************************************/
L7_RC_t ssltCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t ssltRC;
  nvStoreFunctionList_t ssltNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  ssltRC      = L7_SUCCESS;

  memset((void *) &ssltNotifyFunctionList, L7_NULL, sizeof(ssltNotifyFunctionList));
  ssltNotifyFunctionList.registrar_ID   = L7_FLEX_SSLT_COMPONENT_ID;
  ssltNotifyFunctionList.notifySave     = ssltSave;
  ssltNotifyFunctionList.hasDataChanged = ssltHasDataChanged;
  ssltNotifyFunctionList.resetDataChanged = ssltResetDataChanged;

  if (nvStoreRegister(ssltNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = L7_NULL;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    ssltRC      = L7_ERROR;

    return ssltRC;

  }
  ssltDeregister.ssltSave                     = L7_FALSE;
  ssltDeregister.ssltRestore                  = L7_FALSE;
  ssltDeregister.ssltHasDataChanged           = L7_FALSE;

  ssltCnfgrState = SSLT_PHASE_INIT_2;

  return ssltRC;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @comments The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t ssltCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t ssltRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  ssltRC      = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_FLEX_SSLT_COMPONENT_ID,
                       SSLT_CFG_FILENAME,
                       (L7_char8 *)ssltCfg,
                       sizeof(ssltCfg_t),
                       &ssltCfg->checkSum,
                       SSLT_CFG_VER_CURRENT,
                       ssltBuildDefaultConfigData,
                       ssltMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = L7_NULL;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    ssltRC      = L7_ERROR;

    return ssltRC;
  }

  ssltApplyConfigData();

  ssltCnfgrState = SSLT_PHASE_INIT_3;

  return ssltRC;
}

/*********************************************************************
* @purpose  This function undoes ssltCnfgrInitPhase1Process
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void ssltCnfgrFiniPhase1Process()
{
  if (ssltCfg != L7_NULL)
  {
    osapiFree(L7_FLEX_SSLT_COMPONENT_ID, ssltCfg);
    ssltCfg = L7_NULL;
  }

  if (ssltCertSema != L7_NULLPTR)
  {
    osapiSemaDelete(ssltCertSema);
    ssltCertSema = L7_NULLPTR;
  }

  /* this was never deleted in the FP code? */
  if (ssltTaskSyncSema != L7_NULLPTR)
  {
    osapiSemaDelete(ssltTaskSyncSema);
    ssltTaskSyncSema = L7_NULLPTR;
  }

  ssltDiffieHellmanParamsFree();

  ssltInitUndo();

  ssltCnfgrState = SSLT_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes ssltCnfgrInitPhase2Process
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void ssltCnfgrFiniPhase2Process()
{
  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the ssltDeregister is set to L7_FALSE;
   */

  ssltDeregister.ssltSave                     = L7_TRUE;
  ssltDeregister.ssltRestore                  = L7_TRUE;
  ssltDeregister.ssltHasDataChanged           = L7_TRUE;

  ssltCnfgrState = SSLT_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes ssltCnfgrInitPhase3Process
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void ssltCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t reason;

  /* this func will place ssltCnfgrState to WMU */
  ssltCnfgrUconfigPhase2(&response, &reason);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
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
* @end
*********************************************************************/
L7_RC_t ssltCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t ssltRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  return(ssltRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
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
*
* @end
*********************************************************************/
L7_RC_t ssltCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t ssltRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  ssltRC      = L7_SUCCESS;

  if (ssltEventAdminModeSet(L7_DISABLE) != L7_SUCCESS)
  {
    *pResponse  = L7_NULL;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    ssltRC      = L7_ERROR;

    return ssltRC;
  }

  if (ssltEventAuxModeSet(L7_DISABLE) != L7_SUCCESS)
  {
    *pResponse  = L7_NULL;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    ssltRC      = L7_ERROR;

    return ssltRC;
  }

  memset(ssltCfg, L7_NULL, sizeof(ssltCfg_t));

  ssltCnfgrState = SSLT_PHASE_WMU;

  return ssltRC;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands send to ssltEvTask
*
* @returns
*
* @comments none
*
* @end
*********************************************************************/
void ssltCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             ssltRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((ssltRC = ssltCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  ssltCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((ssltRC = ssltCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  ssltCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((ssltRC = ssltCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  ssltCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                ssltRC = ssltCnfgrNoopProccess( &response, &reason );
                ssltCnfgrState = SSLT_PHASE_WMU;
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
                ssltCnfgrState = SSLT_PHASE_EXECUTE;

                ssltRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = L7_NULL;
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
                ssltRC = ssltCnfgrNoopProccess( &response, &reason );
                ssltCnfgrState = SSLT_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                ssltRC = ssltCnfgrUconfigPhase2( &response, &reason );
                ssltCnfgrState = SSLT_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            ssltRC = ssltCnfgrNoopProccess( &response, &reason );
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
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = ssltRC;
  if (ssltRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}


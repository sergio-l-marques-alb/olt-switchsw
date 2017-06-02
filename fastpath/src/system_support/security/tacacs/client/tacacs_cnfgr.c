/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename tacacs_cnfgr.c
*
* @purpose Contains functions to support the tacacs configurator API
*
* @component
*
* @comments
*
* @create 03/21/2005
*
* @author gkiran
*         dfowler 06/23/05
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_common.h"
#include "nvstoreapi.h"
#include "log.h"
#include "tacacs_api.h"
#include "tacacs_cfg.h"
#include "tacacs_aaa.h"
#include "tacacs_authen.h"
#include "tacacs_control.h"
#include "tacacs_cnfgr.h"
#include "tacacs_migrate.h"
#include "osapi.h"
#include "sysapi.h"

extern void                 *tacacsQueue;
extern tacacsCfg_t          *tacacsCfg;
extern tacacsOprData_t      *tacacsOprData;
extern tacacsNotifyEntry_t  *tacacsNotifyList;

tacacsCnfgrState_t tacacsCnfgrState = TACACS_PHASE_INIT_0;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for TACACS+ component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData @b{(input)} Data structure for this
*                                                      CNFGR request.
* @returns  NONE
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the tacacs component.
*           This function is re-entrant.
*
* @end
*********************************************************************/
void tacacsApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{

  tacacsMsg_t           msg;
  L7_CNFGR_CB_DATA_t    cbData;
/*
 * Let all PHASE 1 start fall through into an osapiMessageSend.
 * The application task will handle everything.
 * Phase 1 will do a osapiMessageSend after a few pre reqs have been
 * completed
 */

  if (pCmdData == L7_NULL)
  {
    cbData.correlator               = L7_NULL;
    cbData.asyncResponse.rc         = L7_ERROR;
    cbData.asyncResponse.u.reason   = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* This function will invoke the message send for phase 1 */
    if (tacacsInit(pCmdData) != L7_SUCCESS)
    {
      tacacsInitUndo();
    }
  }
  else
  {
    memcpy(&msg.data.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.event = TACACS_EVENT_CNFGR_INIT;
    (void)osapiMessageSend(tacacsQueue, &msg,
                           sizeof(tacacsMsg_t),
                           L7_NO_WAIT,
                           L7_MSG_PRIORITY_NORM);
  }
  return;
}

/*********************************************************************
*
* @purpose  To Handle cofigurator commands sent to tacacs+ task
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData @b{(input)} Data structure for this
*                                                      CNFGR request.
*
* @returns L7_SUCCESS - There were no errors.
* @returns L7_ERROR   - There were errors.
*
* @notes    none
*
*
*
* @end
*********************************************************************/
L7_RC_t tacacsHandleCnfgrCmd(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t               tacacsRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;


  /* validate command type */
  if ( pCmdData != L7_NULL )
  {
    if (pCmdData->type == L7_CNFGR_RQST)
    {
      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
      if ((request > L7_CNFGR_RQST_FIRST) && (request < L7_CNFGR_RQST_LAST))
      {
        /*Validate Command/Event Pair*/
        switch (command)
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch (request)
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                tacacsRC = tacacsCnfgrInitPhase1Process(&response, &reason);
                if (tacacsRC != L7_SUCCESS)
                {
                  tacacsCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                tacacsRC = tacacsCnfgrInitPhase2Process(&response, &reason );
                if (tacacsRC != L7_SUCCESS)
                {
                  tacacsCnfgrFiniPhase2Process();
                }
               break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                tacacsRC = tacacsCnfgrInitPhase3Process(&response, &reason );
                if (tacacsRC != L7_SUCCESS)
                {
                  tacacsCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                tacacsRC = tacacsCnfgrNoopProcess(&response, &reason);
                tacacsCnfgrState = TACACS_PHASE_WMU;
                break;
              default:
                /* Invalid command/request pair */
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
                        "TACACS+: Invalid Request in Configurator"
                    "Command Initialize.\n");
                break;
            } /* End of the Switch : Intialize Requests */
            break;
          case L7_CNFGR_CMD_EXECUTE:
            switch (request)
            {
              case L7_CNFGR_RQST_E_START:
                tacacsCnfgrState = TACACS_PHASE_EXECUTE;
                tacacsRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = L7_NULL;
                break;
              default:
                /* Invalid command/request pair */
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
                        "TACACS+: Invalid Request in Configurator"
                    "Command Execute.\n");
                break;
            } /* End of the Switch : Command Execute */
            break;
          case L7_CNFGR_CMD_UNCONFIGURE:
            switch (request)
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                tacacsRC = tacacsCnfgrNoopProcess(&response, &reason);
                tacacsCnfgrState = TACACS_PHASE_UNCONFIG_1;
                break;
              case L7_CNFGR_RQST_U_PHASE2_START:
                tacacsRC = tacacsCnfgrUconfigPhase2(&response, &reason);
                tacacsCnfgrState = TACACS_PHASE_UNCONFIG_2;
                break;
              default:
                /* Invalid command/request pair */
                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
                        "TACACS+: Invalid Request in Configurator"
                    "Command Unconfigure.\n");
                break;
            }/* End of the Switch : Command Unconfigure */
            break;
          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
                tacacsRC = tacacsCnfgrNoopProcess(&response, &reason);
            break;

          default:
            /* Invalid command/event pair */
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
                    "TACACS+: Invalid Command/Event Pair.\n");
            break;
        }/* End of the Switch : Command/Event Pair */

      }/* End of if : Validate Request */
      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
                "TACACS+: Invalid Request.\n");
      }/* End of else : Validate Request */

    }/* End of if : Validate Command Type */
    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
              "TACACS+: Invalid Command Type.\n");
    }/* End of else : Validate Command Type */

  } /* end of if cmdData not Null */
  else /* cmdData is NULL */
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
            "TACACS+: Invalid Command Pointer.\n");
  }/* End of if-else : Validate Command Pointer */

  /* - Return value to caller.
  ** - Prepare complete response
  ** - Callback the Configurator
  */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = tacacsRC;

  if (tacacsRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return tacacsRC;
}

/*********************************************************************
*
* @purpose This function process the configurator control commands
*          request pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*        L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
*
* @end
*
*********************************************************************/
L7_RC_t tacacsCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  /*
  ** Initialize the TACACS+ global configuration and operational data.
  ** osapiFree() is called in tacacsCnfgrFiniPhase1Process()
  */
  tacacsCfg         = osapiMalloc(L7_TACACS_COMPONENT_ID, (L7_uint32)sizeof(tacacsCfg_t));
  tacacsOprData     = osapiMalloc(L7_TACACS_COMPONENT_ID, (L7_uint32)sizeof(tacacsOprData_t));
  tacacsNotifyList  = osapiMalloc(L7_TACACS_COMPONENT_ID, L7_LAST_COMPONENT_ID * sizeof(tacacsNotifyEntry_t));

  if ((tacacsCfg == L7_NULLPTR)||
      (tacacsOprData == L7_NULLPTR) ||
      (tacacsNotifyList == L7_NULLPTR))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    return L7_ERROR;
  }

  memset(tacacsCfg, 0, sizeof(tacacsCfg_t));
  memset(tacacsOprData, 0, sizeof(tacacsOprData_t));
  memset((void *)tacacsNotifyList, 0, L7_LAST_COMPONENT_ID * sizeof(tacacsNotifyEntry_t));

  tacacsCnfgrState = TACACS_PHASE_INIT_1;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*        L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
*
* @end
*
*********************************************************************/
L7_RC_t tacacsCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason)
{
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  memset((void *)&notifyFunctionList, 0, sizeof(notifyFunctionList));

  notifyFunctionList.registrar_ID   = L7_TACACS_COMPONENT_ID;
  notifyFunctionList.notifySave     = tacacsSave;
  notifyFunctionList.hasDataChanged = tacacsHasDataChanged;
  notifyFunctionList.resetDataChanged = tacacsResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  tacacsCnfgrState = TACACS_PHASE_INIT_2;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*        L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t tacacsCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  /* read configuration data */
  if (sysapiCfgFileGet(L7_TACACS_COMPONENT_ID,
                       L7_TACACS_CFG_FILE_NAME,
                       (L7_char8 *)(tacacsCfg),
                       (L7_uint32)sizeof(tacacsCfg_t),
                       &(tacacsCfg->checkSum),
                       L7_TACACS_CFG_VER_CURRENT,
                       tacacsBuildDefaultConfigData,
                       tacacsMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_NOT_FOUND;
    return L7_ERROR;
  }

  /* apply configuration data */
  tacacsApplyConfigData();

  tacacsCnfgrState = TACACS_PHASE_INIT_3;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This function undoes tacacsCnfgrInitPhase1Process
*
* @param    none
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void tacacsCnfgrFiniPhase1Process()
{
  if (tacacsCfg != L7_NULLPTR)
  {
    osapiFree(L7_TACACS_COMPONENT_ID, tacacsCfg);
    tacacsCfg = L7_NULLPTR;
  }

  if (tacacsOprData != L7_NULLPTR)
  {
    osapiFree(L7_TACACS_COMPONENT_ID, tacacsOprData);
    tacacsOprData = L7_NULLPTR;
  }

  if (tacacsNotifyList != L7_NULLPTR)
  {
    osapiFree(L7_TACACS_COMPONENT_ID, tacacsNotifyList);
    tacacsNotifyList = L7_NULLPTR;
  }

  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_TACACS_COMPONENT_ID,
          "TACACS+: Failed Intialization.\n");

  tacacsCnfgrState = TACACS_PHASE_INIT_0;

  return;
}

/*********************************************************************
*
* @purpose This function undoes tacacsCnfgrInitPhase2Process
*
* @param    none
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void tacacsCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_TACACS_COMPONENT_ID);

  tacacsCnfgrState = TACACS_PHASE_INIT_1;

  return;
}
/*********************************************************************
*
* @purpose This function undoes tacacsCnfgrInitPhase3Process
*
* @param    none
*
* @retruns void
*
* @notes none
*
* @end
*
*********************************************************************/
void tacacsCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  (void)tacacsCnfgrUconfigPhase2(&response,&reason);

  return;
}
/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
* @param    pReason   - @b{(output)}  Always 0
*
* @returns L7_SUCCESS - Always return this value.
*
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        None.
*
*
* @end
*
*********************************************************************/
L7_RC_t tacacsCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t tacacsCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;

  /* clear out the configuration */
  memset((void *)(tacacsCfg), L7_NULL, sizeof(tacacsCfg_t));

  if (tacacsRestore() != L7_SUCCESS)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  return L7_SUCCESS;
}

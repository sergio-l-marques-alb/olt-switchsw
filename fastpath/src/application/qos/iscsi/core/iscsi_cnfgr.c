/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   iscsi_cnfgr.c
*
* @purpose    Contains definitions to support the new configurator API
*
* @component  iSCSI
*
* @comments   none
*
* @create     04/18/2008
* @end
*
**********************************************************************/

#include <string.h>
#include "buff_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "iscsi_cnfgr.h"
#include "iscsi.h"
#include "iscsi_migrate.h"
#include "sysnet_api.h"

#ifdef L7_NSF_PACKAGE
#include "iscsi_ckpt.h"
#endif
#ifdef L7_CLI_PACKAGE
#include "cli_txt_cfg_api.h"
#endif

/* externs */
extern iscsiCfgData_t      *iscsiCfgData;
extern void                *iscsiQueue;
extern L7_uint32            iscsiTaskId;
extern L7_uint32            iscsiPacketBufferPoolId;
extern osapiRWLock_t        iscsiCfgRWLock;

/* global variables */
iscsiCnfgrState_t           iscsiCnfgrState_g = ISCSI_PHASE_INIT_0;
L7_BOOL                     iscsiWarmStart;

/*********************************************************************
* @purpose  CNFGR System Initialization for ISCSI component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the iSCSI component.  This function is re-entrant.
*
* @end
*********************************************************************/
void iscsiApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t               rc = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason = L7_CNFGR_ERR_RC_INVALID_PAIR;
  L7_BOOL               warmRestart = L7_FALSE;

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
                if ((rc = iscsiCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
                {
                  iscsiCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = iscsiCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  iscsiCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = iscsiCnfgrNoopProccess(&response, &reason);
                iscsiCnfgrState_g = ISCSI_PHASE_WMU;
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM) != L7_NULL)
                {
                  warmRestart = L7_TRUE;
                }
                if ((rc = iscsiCnfgrInitPhase3Process(warmRestart, &response, &reason)) != L7_SUCCESS)
                {
                  iscsiCnfgrFiniPhase3Process();
                }
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
                iscsiCnfgrState_g = ISCSI_PHASE_EXECUTE;

                rc        = L7_SUCCESS;
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
                rc = iscsiCnfgrNoopProccess(&response, &reason);
                iscsiCnfgrState_g = ISCSI_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = iscsiCnfgrUconfigPhase2(&response, &reason);
                iscsiCnfgrState_g = ISCSI_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = iscsiCnfgrNoopProccess(&response, &reason);
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */

      } /* endif validate request */

      else
      {
        reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      }

    } /* endif cnfgr request */

    else
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
    }

  } /* endif validate command type */

  else
  {
    correlator = L7_NULL;
    reason = L7_CNFGR_ERR_RC_INVALID_CMD;
  }

  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*             L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t iscsiCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t       rc;
  L7_uint32     errLine;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  /* use single-pass loop for exception control */
  do
  {
    /* malloc space for the ISCSI configuration data structure */
    iscsiCfgData = (iscsiCfgData_t *)osapiMalloc(L7_FLEX_QOS_ISCSI_COMPONENT_ID, (L7_uint32)L7_ISCSI_CFG_DATA_SIZE);
    if (iscsiCfgData == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }

    if (osapiRWLockCreate(&iscsiCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
    {
      errLine = __LINE__;
      break;
    }
    if (iscsiSemaCreate() == L7_FAILURE)
    {
      errLine = __LINE__;
      break;
    }

    iscsiQueue = (void *)osapiMsgQueueCreate(ISCSI_QUEUE_NAME, ISCSI_MSG_COUNT, ISCSI_MSG_SIZE);

    if (iscsiQueue == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }

    if (bufferPoolInit(ISCSI_PACKET_BUFFER_COUNT, ISCSI_PACKET_BUFFER_SIZE,
                       "iSCSI pkt buf", &iscsiPacketBufferPoolId) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    if (iscsiSessionDataBaseCreate() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    if (iscsiStartTask() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

#ifdef L7_NSF_PACKAGE
    iscsiCkptInit();
#endif

    iscsiCnfgrState_g = ISCSI_PHASE_INIT_1;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;
  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
            "%u: Error initializing iSCSI component", errLine);
  }

  return rc;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*             L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t iscsiCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t       rc;
  L7_uint32     errLine;
  nvStoreFunctionList_t   notifyFunctionList;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_FATAL;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */
  do
  {
    /* nvStore registration */
    notifyFunctionList.registrar_ID     = L7_FLEX_QOS_ISCSI_COMPONENT_ID;
    notifyFunctionList.notifySave       = iscsiSave;
    notifyFunctionList.hasDataChanged   = iscsiHasDataChanged;
    notifyFunctionList.notifyConfigDump = L7_NULLPTR;
    notifyFunctionList.notifyDebugDump  = L7_NULLPTR;
    notifyFunctionList.resetDataChanged = iscsiResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    iscsiCnfgrState_g = ISCSI_PHASE_INIT_2;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

#ifdef L7_NSF_PACKAGE
  if (iscsiCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "iSCSI failed to register for checkpoint service callbacks.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
#endif

#ifdef L7_CLI_PACKAGE
  /* Register with cli text config componet to get notifications after
    apply config completion */
  if (txtCfgApplyCompletionNotifyRegister(L7_FLEX_QOS_ISCSI_COMPONENT_ID,
                                          iscsiTxtCfgApplyCompletionCallback))
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "iSCSI failed to register for apply config completions.");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc  = L7_ERROR;
    return rc;
  }
#endif

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
            "%u: Error initializing iSCSI component", errLine);
  }

  return rc;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3
*
* @param    warmRestart @b{(input)}  indicates warm/cold restart
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*             L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t iscsiCnfgrInitPhase3Process(L7_BOOL              warmRestart,
                                    L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t               rc;
  L7_uint32             errLine;
  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_FATAL;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  iscsiWarmStart = warmRestart;

  do
  {
    if (sysapiCfgFileGet(L7_FLEX_QOS_ISCSI_COMPONENT_ID, ISCSI_CFG_FILENAME,
                        (L7_char8 *)iscsiCfgData, (L7_uint32)sizeof(iscsiCfgData_t),
                        &iscsiCfgData->checkSum, ISCSI_CFG_VER_CURRENT,
                        iscsiBuildDefaultConfigData, iscsiMigrateConfigData) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* apply the saved configuration data (if any) */
    if (iscsiApplyConfigData() == L7_SUCCESS)
    {
      /* force data changed flag to false since nothing has really changed yet*/
      iscsiCfgData->cfgHdr.dataChanged = L7_FALSE;
    }
    else
    {
      /* could not apply saved config data, so attempt to init using the
       * factory default configuration
       *
       * NOTE: the restore function updates data changed flag accordingly
       */
      if (iscsiRestore() == L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
               "Unable to apply saved config -- using factory defaults instead");
      }
      else
      {
        errLine = __LINE__;
        break;
      }
    }

    iscsiCnfgrState_g = ISCSI_PHASE_INIT_3;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;
  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    /* NOTE: only update if iscsiCfgData was previously established */
    if (iscsiCfgData != L7_NULLPTR)
       iscsiCfgData->cfgHdr.dataChanged = L7_FALSE;

    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
            "%u: Error initializing iSCSI component", errLine);
  }

  return rc;
}


/*********************************************************************
* @purpose  This function undoes iscsiCnfgrInitPhase1Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void iscsiCnfgrFiniPhase1Process(void)
{
  /* deallocate anything that was allocated */
  if (iscsiCfgData != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_ISCSI_COMPONENT_ID, iscsiCfgData);
    iscsiCfgData = L7_NULLPTR;
  }

  if (iscsiQueue != L7_NULLPTR)
    osapiMsgQueueDelete(iscsiQueue);

  if (iscsiPacketBufferPoolId != 0)
  {
    /* bufferPoolTerminate() frees associated memory */
    (void)bufferPoolTerminate(iscsiPacketBufferPoolId);
    iscsiPacketBufferPoolId = 0;
  }

  if (iscsiTaskId != L7_ERROR)
    osapiTaskDelete(iscsiTaskId);

  iscsiSessionDataBaseFree();

  if (iscsiCfgRWLock.handle != L7_NULLPTR)
  {
    (void)osapiRWLockDelete(iscsiCfgRWLock);
  }

  iscsiSemaDelete();

#ifdef L7_NSF_PACKAGE
  iscsiCkptFini();
#endif

  iscsiCnfgrState_g = ISCSI_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes iscsiCnfgrInitPhase2Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void iscsiCnfgrFiniPhase2Process(void)
{
  (void)nvStoreDeregister(L7_FLEX_QOS_ISCSI_COMPONENT_ID);

  iscsiCnfgrState_g = ISCSI_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes iscsiCnfgrInitPhase3Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void iscsiCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place iscsiCnfgrState_g in WMU */
  if (iscsiCnfgrUconfigPhase2(&response, &reason) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_ISCSI_COMPONENT_ID,
           "Error unconfiguring ISCSI phase 2");
  }
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP
*
* @param    pResponse   @b{(output)} Response always command complete
* @param    pReason     @b{(output)} Always 0
*
* @returns  L7_SUCCESS  Always return this value (pResponse is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             (none used)
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t iscsiCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason)
{
  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 2.
*
* @param    pResponse   @b{(output)} Response (only if L7_SUCCESS)
* @param    pReason     @b{(output)} Reason (only if L7_ERROR)
*
* @returns  L7_SUCCESS  There were no errors (pResponse is available)
* @returns  L7_ERROR    There were errors (pReason code is available)
*
* @comments The following responses are valid:
*             L7_CNFGR_CMD_COMPLETE
*
* @comments The following error reason codes are valid:
*             L7_CNFGR_ERR_RC_FATAL
*
* @comments This function runs in the configurator's thread, which MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t iscsiCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* reverting to default ISCSI configuration is the closest we can get to
   * unconfiguring the device
   */
  if (iscsiRestore() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  if (iscsiCfgData != L7_NULLPTR)
    memset(iscsiCfgData, 0, L7_ISCSI_CFG_DATA_SIZE);

  iscsiCnfgrState_g = ISCSI_PHASE_WMU;

  return L7_SUCCESS;
}


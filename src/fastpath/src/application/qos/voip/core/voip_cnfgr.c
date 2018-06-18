/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   voip_cnfgr.c
*
* @purpose    Contains definitions to support the new configurator API
*
* @component  VOIP
*
* @comments   none
*
* @create     05/08/2007
*
* @author     parora
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "voip_cnfgr.h"
#include "voip.h"
#include "voip_util.h"
#include "voip_migrate.h"
#include "voip_parse.h"
#include "voip_control.h"
#include "voip_sip.h"
#include "voip_sccp.h"
#include "voip_h323.h"
#include "sysnet_api_ipv4.h"
#ifdef L7_NSF_PACKAGE
#include "voip_ckpt.h"
#endif

/* externs */
extern voipCfgData_t       *voipCfgData;
extern L7_uint32           *voipMapTbl;
extern void                *voipQueue;
extern L7_int32             voipTaskId;
extern osapiRWLock_t        voipCfgRWLock;
extern voipIntfInfo_t      *voipIntfInfo;
L7_uint32                  voipTimerTaskId;

L7_BOOL                 voipWarmStart;
extern PORTEVENT_MASK_t voipPortEventMask_g;
extern void            *voipDbSema;

/* global variables */
voipCnfgrState_t voipCnfgrState_g = VOIP_PHASE_INIT_0;

/*********************************************************************
* @purpose  CNFGR System Initialization for VOIP component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the VOIP component.  This function is re-entrant.
*
* @end
*********************************************************************/
void voipApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_BOOL               warmRestart = L7_FALSE;

  L7_RC_t             rc     = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((rc = voipCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
                {
                  voipCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = voipCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  voipCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = voipCnfgrNoopProccess(&response, &reason);
                voipCnfgrState_g = VOIP_PHASE_WMU;
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                warmRestart = (pCmdData->u.rqstData.data & L7_CNFGR_RESTART_WARM);
                if ((rc = voipCnfgrInitPhase3Process(warmRestart, &response, &reason)) != L7_SUCCESS)
                {
                  voipCnfgrFiniPhase3Process();
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
                voipCnfgrState_g = VOIP_PHASE_EXECUTE;

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
                rc = voipCnfgrNoopProccess(&response, &reason);
                voipCnfgrState_g = VOIP_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = voipCnfgrUconfigPhase2(&response, &reason);
                voipCnfgrState_g = VOIP_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = voipCnfgrNoopProccess(&response, &reason);
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
L7_RC_t voipCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
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
    /* malloc space for the VOIP configuration data structure */
    voipCfgData = (voipCfgData_t *)osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID, (L7_uint32)L7_VOIP_CFG_DATA_SIZE);
    if (voipCfgData == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    else
    {
      memset(voipCfgData, 0, L7_VOIP_CFG_DATA_SIZE);
    }

    if (osapiRWLockCreate(&voipCfgRWLock, OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
    {
      errLine = __LINE__;
      break;
    }
    voipQueue = (void *)osapiMsgQueueCreate(VOIP_QUEUE, VOIP_MSG_COUNT, VOIP_MSG_SIZE);

    if (voipQueue == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }

    if (voipStartTask() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    voipMapTbl    = osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
    if(voipMapTbl == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    else
    {
      memset((void *)voipMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());
    }
    voipIntfInfo  = osapiMalloc(L7_FLEX_QOS_VOIP_COMPONENT_ID, sizeof(voipIntfInfo_t) * platIntfMaxCountGet());

    if(voipIntfInfo == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    else
    {
      memset((void *)voipIntfInfo, 0, sizeof(voipIntfInfo_t) * platIntfMaxCountGet());
    }

    voipDbSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    if (voipDbSema == L7_NULL)
    {
      errLine = __LINE__;
      break;
    }

#ifdef L7_NSF_PACKAGE
    voipCkptInit();
#endif

    voipCnfgrState_g = VOIP_PHASE_INIT_1;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;
  } while (0);
  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "%s %u: %s: Error initializing VOIP component\n",
             __FILE__, errLine, __FUNCTION__);
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
L7_RC_t voipCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t       rc;
  L7_uint32     errLine;
  nvStoreFunctionList_t   notifyFunctionList;
  sysnetPduIntercept_t sysnetPduIntercept;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_FATAL;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */
  do
  {
    /* nvStore registration */
    notifyFunctionList.registrar_ID     = L7_FLEX_QOS_VOIP_COMPONENT_ID;
    notifyFunctionList.notifySave       = voipSave;
    notifyFunctionList.hasDataChanged   = voipHasDataChanged;
    notifyFunctionList.notifyConfigDump = L7_NULLPTR;
    notifyFunctionList.notifyDebugDump  = L7_NULLPTR;
    notifyFunctionList.resetDataChanged = voipResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* register with NIM for link state change callbacks  */
    if (nimRegisterIntfChange(L7_FLEX_QOS_VOIP_COMPONENT_ID,
                              voipIntfChangeCallback,
                              voipStartupNotifyCallback,
                              NIM_STARTUP_PRIO_QOS_VOIP) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }


    voipCnfgrState_g = VOIP_PHASE_INIT_2;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

#ifdef L7_NSF_PACKAGE
  if (voipCkptCallbacksRegister() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_FLEX_QOS_VOIP_COMPONENT_ID,
           "VOIP failed to register for checkpoint service callbacks.");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }
#endif

  sysnetPduIntercept.addressFamily   = L7_AF_INET;
  sysnetPduIntercept.hookId          = SYSNET_INET_RECV_IN;
  sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_VOIP_PRECEDENCE;
  sysnetPduIntercept.interceptFunc   = voipPktIntercept;
  strcpy(sysnetPduIntercept.interceptFuncName, "voipPktIntercept");

      /* Register with sysnet */
  if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "%s %u: %s: Error initializing VOIP component\n",
            __FILE__, errLine, __FUNCTION__);
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
L7_RC_t voipCnfgrInitPhase3Process(L7_BOOL              warmRestart,
                                   L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t               rc;
  L7_uint32             errLine;
  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_FATAL;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  voipWarmStart = warmRestart;

  /* Clear the NIM port event mask until ready to receive events */
  memset(&voipPortEventMask_g, 0, sizeof(voipPortEventMask_g));
  nimRegisterIntfEvents(L7_FLEX_QOS_VOIP_COMPONENT_ID, voipPortEventMask_g);

  do
  {
    if (sysapiCfgFileGet(L7_FLEX_QOS_VOIP_COMPONENT_ID, VOIP_CFG_FILENAME,
                        (L7_char8 *)voipCfgData, (L7_uint32)sizeof(voipCfgData_t),
                        &voipCfgData->checkSum, VOIP_CFG_VER_CURRENT,
                        voipBuildDefaultConfigData,voipMigrateConfigData) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* apply the saved configuration data (if any) */
    if (voipApplyConfigData() == L7_SUCCESS)
    {
      /* force data changed flag to false since nothing has really changed yet*/
      voipCfgData->cfgHdr.dataChanged = L7_FALSE;
    }
    else
    {
      /* could not apply saved config data, so attempt to init using the
       * factory default configuration
       *
       * NOTE: the restore function updates data changed flag accordingly
       */
      if (voipRestore() == L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
                "%s: Unable to apply saved config -- using factory defaults "
                "instead\n", __FUNCTION__);
      }
      else
      {
        errLine = __LINE__;
        break;
      }
    }

    voipCnfgrState_g = VOIP_PHASE_INIT_3;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;
  } while (0);
  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    /* NOTE: only update if voipCfgData was previously established */
    if (voipCfgData != L7_NULLPTR)
       voipCfgData->cfgHdr.dataChanged = L7_FALSE;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "%s %u: %s: Error initializing VOIP component\n",
            __FILE__, errLine, __FUNCTION__);
  }

  return rc;
}


/*********************************************************************
* @purpose  This function undoes voipCnfgrInitPhase1Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void voipCnfgrFiniPhase1Process(void)
{
  /* deallocate anything that was allocated */
  if (voipCfgData != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, voipCfgData);
    voipCfgData = L7_NULLPTR;
  }

  if (voipMapTbl != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, voipMapTbl);
    voipMapTbl = L7_NULLPTR;
  }

  if (voipQueue != L7_NULLPTR)
    osapiMsgQueueDelete(voipQueue);

  if (voipTaskId != L7_ERROR)
    osapiTaskDelete(voipTaskId);

  if (voipIntfInfo != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_VOIP_COMPONENT_ID, voipIntfInfo);
    voipIntfInfo = L7_NULLPTR;
  }

  if (voipCfgRWLock.handle != L7_NULLPTR)
  {
    (void)osapiRWLockDelete(voipCfgRWLock);
  }

  if (voipDbSema != L7_NULL)
  {
    osapiSemaDelete(voipDbSema);
  }

#ifdef L7_NSF_PACKAGE
  voipCkptFini();
#endif

  voipCnfgrState_g = VOIP_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes voipCnfgrInitPhase2Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void voipCnfgrFiniPhase2Process(void)
{
  sysnetPduIntercept_t sysnetPduIntercept;

  sysnetPduIntercept.addressFamily   = L7_AF_INET;
  sysnetPduIntercept.hookId          = SYSNET_INET_RECV_IN;
  sysnetPduIntercept.hookPrecedence  = FD_SYSNET_HOOK_VOIP_PRECEDENCE;
  sysnetPduIntercept.interceptFunc   = voipPktIntercept;
  strcpy(sysnetPduIntercept.interceptFuncName, "voipPktIntercept");

  (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);
  (void)nimDeRegisterIntfChange(L7_FLEX_QOS_VOIP_COMPONENT_ID);
  (void)nvStoreDeregister(L7_FLEX_QOS_VOIP_COMPONENT_ID);

  voipCnfgrState_g = VOIP_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes voipCnfgrInitPhase3Process
*
* @param    void
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void voipCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place voipCnfgrState_g in WMU */
  if (voipCnfgrUconfigPhase2(&response, &reason) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_VOIP_COMPONENT_ID,
            "%s %u: %s: Error unconfiguring VOIP phase 2\n",
            __FILE__, __LINE__, __FUNCTION__);
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
L7_RC_t voipCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t voipCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* reverting to default VOIP configuration is the closest we can get to
   * unconfiguring the device
   */
  /* Delete all the calls. */
  (void)voipSipClearInterfaceEntries();
  (void)voipSccpClearInterfaceEntries();
  (void)voipH323ClearInterfaceEntries();

  if (voipRestore() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  if (voipCfgData != L7_NULLPTR)
    memset(voipCfgData, 0, L7_VOIP_CFG_DATA_SIZE);

  voipCnfgrState_g = VOIP_PHASE_WMU;

  return L7_SUCCESS;
}


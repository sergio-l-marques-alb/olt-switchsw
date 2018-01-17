/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename diffserv_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component
*
* @comments
*
* @create 06/09/2003
*
* @author markl
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "l7_diffserv_include.h"
#include "clustering_api.h"
#include "diffserv_cluster.h"

/* global variables */
diffServCnfgrState_t      diffServCnfgrState_g = DIFFSERV_PHASE_INIT_0;
extern PORTEVENT_MASK_t   diffServPortEventMask_g;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for DiffServ component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the DiffServ comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void diffServApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t               rc = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((rc = diffServCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  diffServCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = diffServCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  diffServCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = diffServCnfgrNoopProccess( &response, &reason );
                diffServCnfgrState_g = DIFFSERV_PHASE_WMU;
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((rc = diffServCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  diffServCnfgrFiniPhase3Process();
                }
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
                diffServCnfgrState_g = DIFFSERV_PHASE_EXECUTE;

                rc       = L7_SUCCESS;
                response = L7_CNFGR_CMD_COMPLETE;
                reason   = 0;
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
                rc = diffServCnfgrNoopProccess( &response, &reason );
                diffServCnfgrState_g = DIFFSERV_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = diffServCnfgrUconfigPhase2( &response, &reason );
                diffServCnfgrState_g = DIFFSERV_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = diffServCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = rc;
  if (rc == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
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
L7_RC_t diffServCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
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
    /* Malloc space for various DiffServ info structure  */
    pDiffServInfo_g =
    (diffServInfo_t *) osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)sizeof(diffServInfo_t));
    if (pDiffServInfo_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(pDiffServInfo_g, 0, sizeof(diffServInfo_t));

    /* malloc space for the diffServ configuration data structure */
    pDiffServCfgData_g =
    (L7_diffServCfg_t *) osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE);
    if (pDiffServCfgData_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(pDiffServCfgData_g, 0, L7_DIFFSERV_CFG_DATA_SIZE);

    diffServMapTbl_g = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());
    if (diffServMapTbl_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(diffServMapTbl_g, 0, sizeof(L7_uint32) * platIntfMaxCountGet());

    /* acquire a DiffServ trace buffer */
    if (traceBlockCreate(DS_TRACE_ENTRY_MAX, DS_TRACE_ENTRY_SIZE_MAX,
                         (L7_uchar8 *)DS_TRACE_REGISTRATION_NAME,
                         &DS_TRACE_HANDLE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "%s: Unable to obtain DiffServ trace buffer\n", __FUNCTION__);
      errLine = __LINE__;
      break;
    }
    else
    {
      /* always start tracing during DiffServ init -- will be turned off
       * later based on config data value
       */
      dsTraceModeApply(L7_ENABLE);
    }

    /* initialize callback support */
    if (diffServCallbackInit() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* initialize MIB support */
    if (diffServStandardMibInit() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    if (diffServPrivateMibInit() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* initialize the Distiller sub-component */
    if (dsDistillerInitPhase1Process() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    if (clusterSupportGet() == L7_SUCCESS)
    {
      if (diffServClusterPhase1Init() != L7_SUCCESS)
      {
        errLine = __LINE__;
        break;
      }
    }

    diffServCnfgrState_g = DIFFSERV_PHASE_INIT_1;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    /* NOTE: only update if pDiffServCfgData_g was established above */
    if (pDiffServCfgData_g != L7_NULLPTR)
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s %u: %s: Error initializing DiffServ\n",
            __FILE__, errLine, __FUNCTION__);
  }

  return rc;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
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
L7_RC_t diffServCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t               rc;
  L7_uint32             errLine;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  do
  {
    /* initialize the Distiller sub-component */
    if (dsDistillerInitPhase2Process() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* nvStore registration */
    notifyFunctionList.registrar_ID     = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
    notifyFunctionList.notifySave       = diffServSave;
    notifyFunctionList.hasDataChanged   = diffServHasDataChanged;
    notifyFunctionList.notifyConfigDump = L7_NULLPTR;
    notifyFunctionList.notifyDebugDump  = L7_NULLPTR;
    notifyFunctionList.resetDataChanged = diffservResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    diffServDeregister_g.diffServSave = L7_FALSE;
    diffServDeregister_g.diffServRestore = L7_FALSE;
    diffServDeregister_g.diffServHasDataChanged = L7_FALSE;

    /* register with NIM for link state change callbacks */
    if (nimRegisterIntfChange(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                              diffServIntfChangeCallback,
                              diffServStartupNotifyCallback,
                              NIM_STARTUP_PRIO_DIFFSERV) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }
    diffServDeregister_g.diffServIntfChange = L7_FALSE;

    /* register with clustering */
    if (clusterSupportGet() == L7_SUCCESS)
    {
      if (diffServClusterPhase2Init() != L7_SUCCESS)
      {
        errLine = __LINE__;
        break;
      }
    }

    diffServCnfgrState_g = DIFFSERV_PHASE_INIT_2;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    /* NOTE: only update if pDiffServCfgData_g was established above */
    if (pDiffServCfgData_g != L7_NULLPTR)
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s %u: %s: Error initializing DiffServ\n",
            __FILE__, errLine, __FUNCTION__);
  }

  return rc;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
* @returns  L7_ERROR   - There were errors. Reason code is available.
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
L7_RC_t diffServCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                        L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t       rc;
  L7_uint32     errLine;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  /* Clear the NIM port event mask until ready to receive events */
  memset(&diffServPortEventMask_g, 0, sizeof(diffServPortEventMask_g));
  nimRegisterIntfEvents(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, diffServPortEventMask_g);

  do
  {
    /* initialize the Distiller sub-component */
    if (dsDistillerInitPhase3Process() != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    if (sysapiCfgFileGet(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CFG_FILENAME,
                        (L7_char8 *)pDiffServCfgData_g, (L7_uint32)sizeof(L7_diffServCfg_t),
                        &pDiffServCfgData_g->checkSum, L7_DIFFSERV_CFG_VER_CURRENT,
                        diffServBuildDefaultConfigData, diffServMigrateConfigData) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    /* DIFFSERV COMPONENT IS NOW INITIALIZED AND READY!
     *
     * NOTE:  For DiffServ, there are no other conditions required in the
     *        system for the component to be ready to run once it is
     *        initialized.  Readiness means DiffServ operation can occur
     *        (the admin mode controls the actual operating state, however).
     */
    pDiffServInfo_g->diffServInitialized = L7_TRUE;
    pDiffServInfo_g->diffServReady = L7_TRUE;


    /* apply the saved configuration data (if any) */
    if (diffServConfigDataApply() == L7_SUCCESS)
    {
      /* force data changed flag to false since nothing has really changed yet*/
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;
    }
    else
    {
      /* could not apply saved config data, so attempt to init using the
       * factory default configuration
       *
       * NOTE: the restore function updates data changed flag accordingly
       */
      if (diffServRestore() == L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s: Error activating saved config -- using factory defaults instead\n",
                __FUNCTION__);
      }
      else
      {
        errLine = __LINE__;
        break;
      }
    }

    diffServCnfgrState_g = DIFFSERV_PHASE_INIT_3;

    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    /* NOTE: only update if pDiffServCfgData_g was established above */
    if (pDiffServCfgData_g != L7_NULLPTR)
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s %u: %s: Error initializing DiffServ\n",
            __FILE__, errLine, __FUNCTION__);
  }

  return rc;
}


/*********************************************************************
* @purpose  This function undoes diffServCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void diffServCnfgrFiniPhase1Process()
{
  /* Deallocate anything that was allocated */

  dsDistillerFiniPhase1Process();

  diffServStandardMibFini();

  diffServPrivateMibFini();

  diffServCallbackFini();

  dsTraceModeApply(L7_DISABLE);
  if (traceBlockDelete(DS_TRACE_HANDLE) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to delete DiffServ trace buffer\n", __FUNCTION__);
  }

  if (diffServMapTbl_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, diffServMapTbl_g);
    diffServMapTbl_g = L7_NULLPTR;
  }

  if (pDiffServCfgData_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pDiffServCfgData_g);
    pDiffServCfgData_g = L7_NULLPTR;
  }

  if (pDiffServInfo_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pDiffServInfo_g);
    pDiffServInfo_g = L7_NULLPTR;
  }

  diffServCnfgrState_g = DIFFSERV_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes diffServCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void diffServCnfgrFiniPhase2Process()
{

  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the diffServDeregister_g is set to L7_FALSE;
   */

  diffServDeregister_g.diffServSave = L7_TRUE;
  diffServDeregister_g.diffServRestore = L7_TRUE;
  diffServDeregister_g.diffServHasDataChanged = L7_TRUE;
  diffServDeregister_g.diffServIntfChange = L7_TRUE;

  dsDistillerFiniPhase2Process();

  diffServCnfgrState_g = DIFFSERV_PHASE_INIT_1;

}

/*********************************************************************
* @purpose  This function undoes diffServCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void diffServCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  dsDistillerFiniPhase3Process();

  /* this func will place diffServCnfgrState_g to WMU */
  if (diffServCnfgrUconfigPhase2(&response, &reason) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unconfig phase 2 function failed with resp=%u, reas=%u\n",
            __FUNCTION__, response, reason);
  }
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. Response is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @notes    This function runs in the configurator's thread. This thread
*           MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t diffServCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason )
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
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
*
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
L7_RC_t diffServCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t       rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc          = L7_SUCCESS;

  /* clear out the applied configuration */
  if (diffServResetCleanUp() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rc         = L7_ERROR;

    return rc;
  }

  if (pDiffServCfgData_g != L7_NULLPTR)
    memset(pDiffServCfgData_g, 0, L7_DIFFSERV_CFG_DATA_SIZE);

  diffServCnfgrState_g = DIFFSERV_PHASE_WMU;

  return rc;
}



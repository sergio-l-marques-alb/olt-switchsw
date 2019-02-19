/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   cos_cnfgr.c
*
* @purpose    Contains definitions to support the new configurator API
*
* @component  cos
*
* @comments   none
*
* @create     03/17/2004
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "cos_cnfgr.h"
#include "cos_config.h"
#include "cos_util.h"

/* externs */
extern L7_uint32          *cosMapTbl_g;
extern cosIntfInfo_t      *pCosIntfInfo_g;

/* global variables */
cosCnfgrState_t cosCnfgrState_g = COS_PHASE_INIT_0;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for COS component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this   
*                                             CNFGR request
*                            
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments This API is provided to allow the Configurator to issue a
*           request to the COS component.  This function is re-entrant.
*
* @end
*********************************************************************/
void cosApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

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
                if ((rc = cosCnfgrInitPhase1Process(&response, &reason)) != L7_SUCCESS)
                {
                  cosCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = cosCnfgrInitPhase2Process(&response, &reason)) != L7_SUCCESS)
                {
                  cosCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = cosCnfgrNoopProccess(&response, &reason);
                cosCnfgrState_g = COS_PHASE_WMU;
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((rc = cosCnfgrInitPhase3Process(&response, &reason)) != L7_SUCCESS)
                {
                  cosCnfgrFiniPhase3Process();
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
                cosCnfgrState_g = COS_PHASE_EXECUTE;

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
                rc = cosCnfgrNoopProccess(&response, &reason);
                cosCnfgrState_g = COS_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = cosCnfgrUconfigPhase2(&response, &reason);
                cosCnfgrState_g = COS_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = cosCnfgrNoopProccess(&response, &reason);
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
L7_RC_t cosCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t       rc;
  L7_uint32     errLine;
  L7_uint32     cosMapTblSize;
  L7_uint32     cosIntfInfoSize;
  L7_uint32     i, tc;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  /* use single-pass loop for exception control */
  do
  {
    /* Malloc space for various COS info structure  */
    pCosInfo_g = (cosInfo_t *)osapiMalloc(L7_FLEX_QOS_COS_COMPONENT_ID, (L7_uint32)sizeof(cosInfo_t));
    if (pCosInfo_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(pCosInfo_g, 0, sizeof(cosInfo_t));  

    /* initialize default for numTrafficClasses now (these will be updated 
     * during dot1p component initialization phase 3)
     */
    pCosInfo_g->globalNumTrafficClasses = L7_DOT1P_NUM_TRAFFIC_CLASSES;
    for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
      pCosInfo_g->numTrafficClasses[i] = L7_DOT1P_NUM_TRAFFIC_CLASSES;

    /* initialize port default priority now (these will be updated 
     * during dot1p component initialization phase 3)
     */
    if (cosMapTableInitLookup(L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT, L7_DOT1P_DEFAULT_USER_PRIORITY,
                              &tc) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }
    pCosInfo_g->globalPortDefaultPriority = L7_DOT1P_DEFAULT_USER_PRIORITY;
    pCosInfo_g->globalPortDefaultTrafficClass = tc;
    for (i = 1; i < L7_MAX_INTERFACE_COUNT; i++)
    {
      pCosInfo_g->portDefaultPriority[i] = L7_DOT1P_DEFAULT_USER_PRIORITY;
      pCosInfo_g->portDefaultTrafficClass[i] = tc;
    }

    /* malloc space for the COS configuration data structure */
    pCosCfgData_g = (L7_cosCfgData_t *)osapiMalloc(L7_FLEX_QOS_COS_COMPONENT_ID, (L7_uint32)L7_COS_CFG_DATA_SIZE);
    if (pCosCfgData_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(pCosCfgData_g, 0, L7_COS_CFG_DATA_SIZE);

    cosMapTblSize = platIntfMaxCountGet() * (L7_uint32)sizeof(L7_uint32);
    cosMapTbl_g = osapiMalloc(L7_FLEX_QOS_COS_COMPONENT_ID, cosMapTblSize);
    if (cosMapTbl_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(cosMapTbl_g, 0, (size_t)cosMapTblSize);

    cosIntfInfoSize = platIntfMaxCountGet() * (L7_uint32)sizeof(cosIntfInfo_t);
    pCosIntfInfo_g = osapiMalloc(L7_FLEX_QOS_COS_COMPONENT_ID, cosIntfInfoSize);
    if (pCosIntfInfo_g == L7_NULLPTR)
    {
      errLine = __LINE__;
      break;
    }
    memset(pCosIntfInfo_g, 0, (size_t)cosIntfInfoSize);

    cosCnfgrState_g = COS_PHASE_INIT_1;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    LOG_MSG("%s %u: %s: Error initializing COS component\n",
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
L7_RC_t cosCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
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
    notifyFunctionList.registrar_ID     = L7_FLEX_QOS_COS_COMPONENT_ID;
    notifyFunctionList.notifySave       = cosSave;
    notifyFunctionList.hasDataChanged   = cosHasDataChanged;
    notifyFunctionList.notifyConfigDump = cosConfigDataShowAll;
    notifyFunctionList.notifyDebugDump  = L7_NULLPTR;
    notifyFunctionList.resetDataChanged = cosResetDataChanged;

    if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }

    cosDeregister_g.cosSave = L7_FALSE;
    cosDeregister_g.cosRestore = L7_FALSE;
    cosDeregister_g.cosHasDataChanged = L7_FALSE;

    /* register with NIM for link state change callbacks */
    if (nimRegisterIntfChange(L7_FLEX_QOS_COS_COMPONENT_ID, 
                              cosIntfChangeCallback) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }
    cosDeregister_g.cosIntfChangeCallback = L7_FALSE;

    cosCnfgrState_g = COS_PHASE_INIT_2;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    LOG_MSG("%s %u: %s: Error initializing COS component\n",
            __FILE__, errLine, __FUNCTION__);
  }

  return rc;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3
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
L7_RC_t cosCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t               rc;
  L7_uint32             errLine;
  L7_CNFGR_QUERY_DATA_t dot1qState;

  *pResponse = 0;
  *pReason   = L7_CNFGR_ERR_RC_FATAL;
  rc         = L7_ERROR;
  errLine    = 0;                       /* will be updated with error line number */

  do
  {
    if (sysapiCfgFileGet(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_CFG_FILENAME,
                        (L7_char8 *)pCosCfgData_g, (L7_uint32)sizeof(L7_cosCfgData_t),
                        &pCosCfgData_g->checkSum, L7_COS_CFG_VER_CURRENT,
                        cosBuildDefaultConfigData, cosMigrateConfigData) != L7_SUCCESS)
    {
      errLine = __LINE__;
      break;
    }
    
    /* cosConfigDataApply() depends on dot1q config data (for example,
     * dot1p mapping). Make sure dot1q has finished phase 3
     * init before proceeding.
     */
    memset(&dot1qState, 0, sizeof(dot1qState));  
    dot1qState.type = L7_CNFGR_QRY_TYPE_SINGLE;
    dot1qState.args.cid = L7_DOT1Q_COMPONENT_ID;
    dot1qState.request = L7_CNFGR_QRY_RQST_STATE;
    dot1qState.data.state.current = L7_CNFGR_STATE_NULL;

    while ((dot1qState.data.state.current != L7_CNFGR_STATE_P3) &&
           (dot1qState.data.state.current != L7_CNFGR_STATE_WMU) &&
           (dot1qState.data.state.current != L7_CNFGR_STATE_E))
    {
      osapiSleepMSec(10);
      if (cnfgrApiQuery(&dot1qState) != L7_SUCCESS)
      {
        errLine = __LINE__;
        break;
      }
    }
    if (errLine != 0)
      break;

    /* apply the saved configuration data (if any) */ 
    if (cosConfigDataApply() == L7_SUCCESS)
    {
      /* force data changed flag to false since nothing has really changed yet*/
      pCosCfgData_g->cfgHdr.dataChanged = L7_FALSE;
    }
    else
    {
      /* could not apply saved config data, so attempt to init using the
       * factory default configuration
       *
       * NOTE: the restore function updates data changed flag accordingly 
       */
      if (cosRestore() == L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_COS_COMPONENT_ID, 
                "%s: Unable to apply saved config -- using factory defaults "
                "instead."
                " The COS component was unable to apply the saved configuration"
                " and has initialized to the factory default settings.", __FUNCTION__);
      } 
      else
      {
        errLine = __LINE__;
        break;
      }
    }

    cosCnfgrState_g = COS_PHASE_INIT_3;

    /* init phase completed successfully */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
    rc         = L7_SUCCESS;

  } while (0);

  if ((rc != L7_SUCCESS) && (errLine != 0))
  {
    /* NOTE: only update if pCosCfgData_g was previously established */
    if (pCosCfgData_g != L7_NULLPTR)
      pCosCfgData_g->cfgHdr.dataChanged = L7_FALSE;

    LOG_MSG("%s %u: %s: Error initializing COS component\n",
            __FILE__, errLine, __FUNCTION__);
  }

  return rc;
}


/*********************************************************************
* @purpose  This function undoes cosCnfgrInitPhase1Process
*
* @param    void
*     
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void cosCnfgrFiniPhase1Process(void)
{
  /* deallocate anything that was allocated */
  if (pCosIntfInfo_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_COS_COMPONENT_ID, pCosIntfInfo_g);
    pCosIntfInfo_g = L7_NULLPTR;
  }

  if (cosMapTbl_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_COS_COMPONENT_ID, cosMapTbl_g);
    cosMapTbl_g = L7_NULLPTR;
  }

  if (pCosCfgData_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_COS_COMPONENT_ID, pCosCfgData_g);
    pCosCfgData_g = L7_NULLPTR;
  }

  if (pCosInfo_g != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_COS_COMPONENT_ID, pCosInfo_g);
    pCosInfo_g = L7_NULLPTR;
  }

  cosCnfgrState_g = COS_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes cosCnfgrInitPhase2Process
*
* @param    void
*     
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void cosCnfgrFiniPhase2Process(void)
{
  /* 
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now,
   * a temporary solution is set in place, wherein a registered
   * function can execute its callback only if its corresponding
   * member in the cosDeregister_g struct is set to L7_FALSE;
   */

  cosDeregister_g.cosSave = L7_TRUE;
  cosDeregister_g.cosRestore = L7_TRUE;
  cosDeregister_g.cosHasDataChanged = L7_TRUE;
  cosDeregister_g.cosIntfChangeCallback = L7_TRUE;

  cosCnfgrState_g = COS_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes cosCnfgrInitPhase3Process
*
* @param    void
*     
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void cosCnfgrFiniPhase3Process(void)
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place cosCnfgrState_g in WMU */
  if (cosCnfgrUconfigPhase2(&response, &reason) != L7_SUCCESS)
  {
    LOG_MSG("%s %u: %s: Error unconfiguring COS phase 2\n",
            __FILE__, __LINE__, __FUNCTION__);
    /* keep going */
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
L7_RC_t cosCnfgrNoopProccess(L7_CNFGR_RESPONSE_t *pResponse,
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
L7_RC_t cosCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                              L7_CNFGR_ERR_RC_t   *pReason)
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* reverting to default COS configuration is the closest we can get to
   * unconfiguring the device
   */
  if (cosRestore() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  if (pCosCfgData_g != L7_NULLPTR)
    memset(pCosCfgData_g, 0, L7_COS_CFG_DATA_SIZE);

  cosCnfgrState_g = COS_PHASE_WMU;

  return L7_SUCCESS;
}


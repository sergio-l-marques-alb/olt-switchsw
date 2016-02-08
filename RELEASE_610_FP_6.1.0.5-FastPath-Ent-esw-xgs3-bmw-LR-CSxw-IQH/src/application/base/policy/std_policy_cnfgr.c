/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename std_policy_cnfgr.c
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
#include <string.h>
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "std_policy.h"
#include "std_policy_api.h"

policyCnfgrState_t policyCnfgrState = POLICY_PHASE_INIT_0;

extern policyCfgData_t  *policyCfgData;
extern L7_uint32 *policyMapTbl;
extern policyDeregister_t policyDeregister;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Policy component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the policy comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void policyApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             policyRC = L7_ERROR;
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
                if ((policyRC = policyCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  policyCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((policyRC = policyCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  policyCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((policyRC = policyCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  policyCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                policyRC = policyCnfgrNoopProcess( &response, &reason );
                policyCnfgrState = POLICY_PHASE_WMU;
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
                policyCnfgrState = POLICY_PHASE_EXECUTE;

                policyRC  = L7_SUCCESS;
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
                policyRC = policyCnfgrNoopProcess( &response, &reason );
                policyCnfgrState = POLICY_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                policyRC = policyCnfgrUconfigPhase2( &response, &reason );
                policyCnfgrState = POLICY_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            policyRC = policyCnfgrNoopProcess( &response, &reason );
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
  cbData.asyncResponse.rc = policyRC;
  if (policyRC == L7_SUCCESS)
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
L7_RC_t policyCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t policyRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  policyRC   = L7_SUCCESS;


  /* Allocate memory for global data */
  policyCfgData = osapiMalloc(L7_POLICY_COMPONENT_ID, sizeof(policyCfgData_t));
  policyMapTbl  = osapiMalloc(L7_POLICY_COMPONENT_ID, sizeof(L7_uint32) * platIntfMaxCountGet());

  /* Make sure that allocation succeded */
  if ((policyMapTbl == L7_NULL) || (policyCfgData == L7_NULL))
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    policyRC   = L7_ERROR;

    return policyRC;
  }


  memset(policyMapTbl, 0, sizeof(L7_uint32) * platIntfMaxCountGet());
  memset(policyCfgData, 0, sizeof(policyCfgData_t));

  policyCnfgrState = POLICY_PHASE_INIT_1;

  return policyRC;

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
L7_RC_t policyCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t policyRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  policyRC   = L7_SUCCESS;

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_POLICY_COMPONENT_ID;
  notifyFunctionList.notifySave     = policySave;
  notifyFunctionList.hasDataChanged = policyHasDataChanged;
  notifyFunctionList.resetDataChanged = policyResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    policyRC   = L7_ERROR;

    return policyRC;
  }
  policyDeregister.policySave = L7_FALSE;
  policyDeregister.policyRestore = L7_FALSE;
  policyDeregister.policyHasDataChanged = L7_FALSE;

  /* register NIM callback to support interface changes */
  if ((policyRC = nimRegisterIntfChange(L7_POLICY_COMPONENT_ID,
                                        policyIntfChangeCallback)) != L7_SUCCESS)
  {
    LOG_MSG("Failed nim registration for Interface Change Callback\n");
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    policyRC     = L7_ERROR;

    return policyRC;
  }
  policyDeregister.policyIntfChangeCallback = L7_FALSE;

  policyCnfgrState = POLICY_PHASE_INIT_2;

  return policyRC;
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
L7_RC_t policyCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t policyRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  policyRC   = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_POLICY_COMPONENT_ID, POLICY_CFG_FILENAME,
                       (L7_char8 *)policyCfgData, sizeof(policyCfgData_t),
                       &policyCfgData->checkSum, POLICY_CFG_VER_CURRENT,
                       policyBuildDefaultConfigData, policyMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    policyRC   = L7_ERROR;

    return policyRC;
  }

  if (policyApplyConfigData() != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    policyRC   = L7_ERROR;

    return policyRC;
  }

  policyCfgData->cfgHdr.dataChanged = L7_FALSE;

  policyCnfgrState = POLICY_PHASE_INIT_3;

  return policyRC;
}


/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void policyCnfgrFiniPhase1Process()
{
  /* Deallocate anything that was allocated */
  if (policyMapTbl != L7_NULL)
  {
    osapiFree(L7_POLICY_COMPONENT_ID, policyMapTbl);
    policyMapTbl = L7_NULL;
  }

  if (policyCfgData != L7_NULL)
  {
    osapiFree(L7_POLICY_COMPONENT_ID, policyCfgData);
    policyCfgData = L7_NULL;
  }

  policyCnfgrState = POLICY_PHASE_INIT_0;
}


/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void policyCnfgrFiniPhase2Process()
{

  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the policyDeregister is set to L7_FALSE;
   */

  policyDeregister.policySave = L7_TRUE;
  policyDeregister.policyRestore = L7_TRUE;
  policyDeregister.policyHasDataChanged = L7_TRUE;
  policyDeregister.policyIntfChangeCallback = L7_TRUE;

  policyCnfgrState = POLICY_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes policyCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void policyCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place policyCnfgrState to WMU */
  policyCnfgrUconfigPhase2(&response, &reason);
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
L7_RC_t policyCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t policyRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(policyRC);
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

L7_RC_t policyCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t policyRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  policyRC    = L7_SUCCESS;

  /* Clear out the configuration */
  memset(policyCfgData, 0, sizeof(policyCfgData_t));

  if (policySystemBcastStormModeSet(L7_DISABLE) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    policyRC   = L7_ERROR;

    return policyRC;
  }

  if (cnfgrIsFeaturePresent(L7_POLICY_COMPONENT_ID,
                            L7_POLICY_PORT_FLOW_CONTROL_FEATURE_ID) == L7_FALSE)
  {
    if (policySystemFlowControlModeSet(L7_DISABLE) != L7_SUCCESS)
    {
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      policyRC   = L7_ERROR;

      return policyRC;
    }
  }

  policyCnfgrState = POLICY_PHASE_WMU;

  return policyRC;
}



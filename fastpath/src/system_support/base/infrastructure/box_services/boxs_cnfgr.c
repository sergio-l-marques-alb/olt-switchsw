/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename   boxs_cnfgr.c
*
* @purpose    The functions that interface to the CNFGR component
*
* @component  boxs
*
* @comments
*
* @create     01/15/2008
*
* @author     vkozlov
* @end
*
**********************************************************************/

#include "commdefs.h"

#include "l7_common.h"
#include "l7_cnfgr_api.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "boxs.h"
#include "boxs_cfg.h"
#include "boxs_debug.h"
#include <string.h>


boxsCfg_t * boxsCfg              = L7_NULLPTR;


/* The last phase that was completed */
static L7_CNFGR_STATE_t boxsState = L7_CNFGR_STATE_NULL;


/* Prototypes for this module only */
L7_RC_t boxsCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

void    boxsCnfgrFiniPhase1Process(void);

L7_RC_t boxsCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

void    boxsCnfgrFiniPhase2Process(void);

L7_RC_t boxsCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason );

void    boxsCnfgrFiniPhase3Process(void);

L7_RC_t boxsCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason );

L7_CNFGR_STATE_t boxsCnfgrPhaseGet(void);

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.
*
* @returns  None
*
* @notes    This function completes synchronously. The return value
*           is presented to the configurator by calling the cnfgrApiCallback().
*           The following are the possible return codes:
*           L7_SUCCESS - There were no errors. Response is available.
*           L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_CB_ERR_RC_INVALID_RQST
*           L7_CNFGR_CB_ERR_RC_INVALID_CMD
*           L7_CNFGR_ERR_RC_INVALID_CMD_TYPE
*           L7_CNFGR_ERR_RC_INVALID_PAIR
*           L7_CNFGR_ERR_RC_FATAL
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
void boxsApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             boxsRC    = L7_ERROR;
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
                if ((boxsRC = boxsCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  boxsCnfgrFiniPhase1Process();
                }
                else
                {
                  boxsState = L7_CNFGR_STATE_P1;
                }

                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((boxsRC = boxsCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  boxsCnfgrFiniPhase2Process();
                }
                else
                {
                  boxsState = L7_CNFGR_STATE_P2;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:  /* no configuration data to be read */
                if ((boxsRC = boxsCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  boxsCnfgrFiniPhase3Process();
                }
                else
                {
                  boxsState = L7_CNFGR_STATE_P3;
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                boxsRC = boxsCnfgrNoopProccess( &response, &reason);
                boxsState = L7_CNFGR_STATE_WMU;
                break;

              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            boxsRC = boxsCnfgrNoopProccess( &response, &reason );
            boxsState = L7_CNFGR_STATE_E;
            break;
          case L7_CNFGR_CMD_UNCONFIGURE:
            boxsRC = boxsCnfgrNoopProccess( &response, &reason );
            boxsState = L7_CNFGR_STATE_U2;
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            boxsRC = boxsCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = boxsRC;

  if (boxsRC == L7_SUCCESS)
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

/*
 *********************************************************************
 *             boxs cnfgr Internal Function Calls
 *********************************************************************
*/

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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t boxsCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t boxsRC = L7_SUCCESS;

  boxsCfg = (boxsCfg_t *)osapiMalloc(L7_BOX_SERVICES_COMPONENT_ID, (L7_uint32)sizeof(boxsCfg_t));

  if (boxsCfg == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    boxsRC   = L7_ERROR;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
            "Init phase 1 faield, reason %d\n",
                     L7_CNFGR_ERR_RC_LACK_OF_RESOURCES);
    return boxsRC;
  }

  boxsRC = boxsInitialize();

  if (boxsRC != L7_SUCCESS)
  {
    boxsRC   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  memset(boxsCfg, 0, sizeof(boxsCfg_t));


  return(boxsRC);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t boxsCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  nvStoreFunctionList_t boxsNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  memset((void *) &boxsNotifyFunctionList, 0, sizeof(boxsNotifyFunctionList));

  boxsNotifyFunctionList.registrar_ID         = L7_BOX_SERVICES_COMPONENT_ID;
  boxsNotifyFunctionList.notifySave           = boxsSave;
  boxsNotifyFunctionList.hasDataChanged       = boxsHasDataChanged;
  boxsNotifyFunctionList.resetDataChanged     = boxsResetDataChanged;

  if (nvStoreRegister(boxsNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc     = L7_ERROR;

    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
            "Init phase 2 failed, reason %d\n",
                     L7_CNFGR_ERR_RC_FATAL);

    return rc;
  }

  rc = sysapiHpcReceiveCallbackRegister((void*)boxsHPCReceiveCallback, L7_BOX_SERVICES_COMPONENT_ID);

  if (rc != L7_SUCCESS)
  {
    rc = L7_ERROR;
    *pReason    =  L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  =  0;
  }
  else
  {
    /* Return Value to caller */
    *pResponse = L7_CNFGR_CMD_COMPLETE;
    *pReason   = 0;
  }

  return(rc);
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
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t boxsCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;


  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  if (sysapiCfgFileGet(L7_BOX_SERVICES_COMPONENT_ID, BOXS_CFG_FILENAME,
                   (L7_char8 *)boxsCfg, sizeof(boxsCfg_t),
                   &boxsCfg->checkSum, BOXS_CFG_VER_CURRENT,
                   boxsBuildDefaultConfigData, boxsMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    rc     = L7_ERROR;
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_BOX_SERVICES_COMPONENT_ID,
            "sysapiCfgFileGet(%s) failed, init phase 3 failed, reason %d\n",
                     BOXS_CFG_FILENAME, L7_CNFGR_ERR_RC_FATAL);
    return rc;
  }

  return rc;
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
L7_RC_t boxsCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t boxsRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(boxsRC);
}

/*********************************************************************
* @purpose  This function returns the phase that boxs believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until boxs has completed phase 1, the value will be L7_CNFGR_RQST_FIRST
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t boxsCnfgrPhaseGet(void)
{
  return(boxsState);
}


/*********************************************************************
* @purpose  Release all resources collected during phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsCnfgrFiniPhase1Process(void)
{
  if (boxsCfg != L7_NULLPTR)
  {
    osapiFree(L7_BOX_SERVICES_COMPONENT_ID, (void *)boxsCfg);
    boxsCfg = L7_NULLPTR;
  }

  boxsUninitialize();
}

/*********************************************************************
* @purpose  Release all resources collected during phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsCnfgrFiniPhase2Process(void)
{
}

/*********************************************************************
* @purpose  Release all resources collected during phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void boxsCnfgrFiniPhase3Process(void)
{
}



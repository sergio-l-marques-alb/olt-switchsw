/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2006-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
***********************************************************************
*
* @filename link_dependency_cnfgr.c
*
* @purpose Provide the Configurator hooks for Initialization and Teardown
*
* @component link dependency
*
* @comments
*
* @create 12/06/2006
*
* @author dflint
* @end
*
**********************************************************************/

#include <string.h>
#include "osapi.h"
#include "l7_cnfgr_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "nimapi.h"

#include "link_dependency.h"
#include "link_dependency_api.h"
#include "link_dependency_data.h"

extern L7_RC_t linkDependencyApplyConfigData(void);

/*
 * Local prototypes
 */
L7_RC_t linkDependencyCnfgrInitPhase1Process      ( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t linkDependencyCnfgrInitPhase2Process      ( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t linkDependencyCnfgrInitPhase3Process      ( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t linkDependencyCnfgrInitPhaseExecuteProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t linkDependencyCnfgrNoopProccess           ( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t linkDependencyCnfgrUnconfigureProcess1    ( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );
L7_RC_t linkDependencyCnfgrUnconfigureProcess2    ( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason );

L7_RC_t   linkDependencyPhaseOneInit    (void);
void      linkDependencyPhaseOneFini    (void);
L7_RC_t   linkDependencyPhaseTwoInit    (void);
void      linkDependencyPhaseTwoFini    (void);
L7_RC_t   linkDependencyPhaseThreeInit  (void);
void      linkDependencyPhaseThreeFini  (void);
L7_RC_t   linkDependencyPhaseExecuteInit(void);

/* The last phase that was completed */
static L7_CNFGR_STATE_t linkDependencyState = L7_CNFGR_STATE_NULL;

extern void *LinkDependencySemaphore;

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
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
void linkDependencyApiCnfgrCommand( L7_CNFGR_CMD_DATA_t *pCmdData )
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator      = L7_NULL;
  L7_BOOL               performCallback = L7_TRUE;

  L7_RC_t               rc              = L7_ERROR;
  L7_CNFGR_ERR_RC_t     reason          = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                rc = linkDependencyCnfgrInitPhase1Process( &response, &reason );
                if (L7_SUCCESS != rc)
                {
                  linkDependencyPhaseOneFini();
                }
                else
                {
                  linkDependencyState = L7_CNFGR_STATE_P1;
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                rc = linkDependencyCnfgrInitPhase2Process( &response, &reason );
                if (L7_SUCCESS != rc)
                {
                  linkDependencyPhaseTwoFini();
                }
                else
                {
                  linkDependencyState = L7_CNFGR_STATE_P2;
                }
                break;

              case L7_CNFGR_RQST_I_PHASE3_START:
                rc = linkDependencyCnfgrInitPhase3Process( &response, &reason );
                if (L7_SUCCESS != rc)
                {
                  linkDependencyPhaseThreeFini();
                }
                else
                {
                  linkDependencyState = L7_CNFGR_STATE_P3;
                }
                break;

              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = linkDependencyCnfgrNoopProccess( &response, &reason );
                linkDependencyState = L7_CNFGR_STATE_WMU;
                break;

              default:
                /* invalid command/request pair */
                break;
            } /* endswitch initialize requests */
            break;

          case L7_CNFGR_CMD_EXECUTE:
            rc = linkDependencyCnfgrInitPhaseExecuteProcess( &response, &reason );
            linkDependencyState = L7_CNFGR_STATE_E;
            break;

          case L7_CNFGR_CMD_UNCONFIGURE:

            if ( L7_CNFGR_RQST_U_PHASE1_START == request )
            {
              cbData.correlator = correlator;
              rc = linkDependencyCnfgrUnconfigureProcess1( &response, &reason);
            }
            if ( L7_CNFGR_RQST_U_PHASE2_START == request )
            {
              rc = linkDependencyCnfgrUnconfigureProcess2(&response, &reason);
              if ( L7_SUCCESS == rc )
              {
                linkDependencyState = L7_CNFGR_STATE_U2;
              }
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = linkDependencyCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = rc;

  if (rc == L7_SUCCESS)
  {
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    cbData.asyncResponse.u.reason   = reason;
  }

  if (performCallback == L7_TRUE)
  {
    cnfgrApiCallback(&cbData);
  }

  return;
}

/*********************************************************************
* @purpose  This function returns the phase that linkDependency believes it has completed
*
* @param    void
*
* @returns  L7_CNFGR_RQST_t - the last phase completed
*
* @notes    Until linkDependency has completed phase 1, the value will be L7_CNFGR_STATE_NULL
*
* @end
*********************************************************************/
L7_CNFGR_STATE_t linkDependencyCnfgrPhaseGet(void)
{
  return(linkDependencyState);
}

/*
 *********************************************************************
 *             linkDependency cnfgr Internal Function Calls
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
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes    This function runs in the configurator's thread. This thread MUST NOT
*           be blocked!
*
* @end
*********************************************************************/
L7_RC_t linkDependencyCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = linkDependencyPhaseOneInit();

  if (rc != L7_SUCCESS)
  {
    rc   = L7_ERROR;
    *pReason    = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    *pResponse  = 0;
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
L7_RC_t linkDependencyCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = linkDependencyPhaseTwoInit();

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
L7_RC_t linkDependencyCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                              L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = linkDependencyPhaseThreeInit();

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
L7_RC_t linkDependencyCnfgrInitPhaseExecuteProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = linkDependencyPhaseExecuteInit();

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
L7_RC_t linkDependencyCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                                         L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rc);
}

/*********************************************************************
* @purpose  Transitions the linkDependency to configurator state WMU from PHASE 3
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t linkDependencyCnfgrUnconfigureProcess1( L7_CNFGR_RESPONSE_t *pResponse,
                                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;

  return(rc);
}

/*********************************************************************
* @purpose  Transitions the linkDependency to configurator state WMU from PHASE 3
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
*
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t linkDependencyCnfgrUnconfigureProcess2( L7_CNFGR_RESPONSE_t *pResponse,
                                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc = L7_SUCCESS;

  linkDependencyPhaseThreeFini();

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rc);
}


/*********************************************************************
* @purpose  phase 1 to Initialize linkDependency
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t linkDependencyPhaseOneInit(void)
{
  /* initialize the global data to zero */
  memset(&LinkDependencyData_g, 0, sizeof(LinkDependencyData_g));

  /* create the semaphore */
  LinkDependencySemaphore = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

  if ( L7_NULL == LinkDependencySemaphore )
  {
    LOG_MSG("Unable to create link dependency semaphore()\n");
    return(L7_FAILURE);
  }

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 1
*
* @param    void
*
* @returns  void
*
* @notes    none.
*
* @end
*********************************************************************/
void linkDependencyPhaseOneFini(void)
{
  if ( L7_NULL != LinkDependencySemaphore )
  {
    osapiSemaDelete( LinkDependencySemaphore );
  }

  return;
}

/*********************************************************************
* @purpose  phase 2 to Initialize linkDependency component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t linkDependencyPhaseTwoInit(void)
{
  L7_RC_t rc = L7_SUCCESS;
  nvStoreFunctionList_t notifyFunctionList = { 0 }; /* NVRAM storage routines */

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.hasDataChanged   = linkDependencyHasDataChanged;
  notifyFunctionList.notifySave       = linkDependencyCfgSave;
  notifyFunctionList.registrar_ID     = L7_LINK_DEPENDENCY_COMPONENT_ID;
  notifyFunctionList.resetDataChanged = linkDependencyResetDataChanged;

  rc = nvStoreRegister(notifyFunctionList);
  if ( L7_SUCCESS != rc )
  {
    LOG_MSG("Link Dependency: Failed to register NV routines\n");
  }
  /* register callback with NIM for L7_UPs and L7_DOWNs */
  else if ((rc = nimRegisterIntfChange(L7_LINK_DEPENDENCY_COMPONENT_ID, linkDependencyIntfChangeCallBack)) != L7_SUCCESS)
  {
    LOG_MSG("Link Dependency: failed to register to NIM\n");
  }
  else
  {
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 2
*
* @param    void
*
* @returns  void
*
* @notes    none.
*
* @end
*********************************************************************/
void linkDependencyPhaseTwoFini(void)
{
  /* We don't want interface changes anymore */
  nimDeRegisterIntfChange(L7_LINK_DEPENDENCY_COMPONENT_ID);

  /* need a mechanism to de-register for PDUs */

  return;
}

/*********************************************************************
* @purpose  Builds the Default configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    None
*
* @end
*********************************************************************/
void linkDependencyBuildDefaultConfigData(L7_uint32 ver)
{
  memset( (void*)&LinkDependencyData_cfg, 0, sizeof(link_dependency_cfg_data_t));

  /* build header */
  strcpy((char*)LinkDependencyData_cfg.hdr.filename, LINK_DEPENDENCY_FILENAME);
  LinkDependencyData_cfg.hdr.version = ver;
  LinkDependencyData_cfg.hdr.componentID = L7_DOT1Q_COMPONENT_ID;
  LinkDependencyData_cfg.hdr.type = L7_CFG_DATA;
  LinkDependencyData_cfg.hdr.length = (L7_uint32)sizeof(link_dependency_cfg_data_t);
  LinkDependencyData_cfg.hdr.dataChanged = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  phase 3 to Initialize linkDependency component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Read the configuration during phase 3
*
* @end
*********************************************************************/
L7_RC_t   linkDependencyPhaseThreeInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* nvstore, register, */
  rc = sysapiCfgFileGet(L7_LINK_DEPENDENCY_COMPONENT_ID, LINK_DEPENDENCY_FILENAME, (L7_char8*)&LinkDependencyData_cfg,
                        (L7_uint32)sizeof(link_dependency_cfg_data_t), &LinkDependencyData_cfg.checkSum,
                        LINK_DEPENDENCY_CFG_VER_CURRENT, linkDependencyBuildDefaultConfigData, L7_NULLPTR/* migration required */);

  LinkDependencyData_cfg.hdr.dataChanged = L7_FALSE;

  /* update the interface config data into the application */
  rc = linkDependencyApplyConfigData();
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("linkDependencyApplyConfigData: failed \n");
  }

  return(rc);
}

/*********************************************************************
* @purpose  Remove all resources acquired during Phase 3
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void linkDependencyPhaseThreeFini(void)
{
  /* erase volatile copy of the config */
  memset(&LinkDependencyData_g, 0, sizeof(LinkDependencyData_g));
  memset(&LinkDependencyData_cfg, 0, sizeof(LinkDependencyData_cfg));
  return;
}

/*********************************************************************
* @purpose  phase Execute to Initialize linkDependency component
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Read the configuration during phase 3
*
* @end
*********************************************************************/
L7_RC_t   linkDependencyPhaseExecuteInit(void)
{
  L7_RC_t rc = L7_SUCCESS;

  LinkDependencyData_g.configChanged = L7_FALSE;
  return(rc);
}

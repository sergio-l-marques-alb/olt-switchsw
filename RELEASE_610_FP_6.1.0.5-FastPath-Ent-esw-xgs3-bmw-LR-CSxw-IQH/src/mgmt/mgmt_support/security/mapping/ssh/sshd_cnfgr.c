/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  sshd_cnfgr.c
*
* @purpose   Contains definitions to support the new configurator API
*
* @component sshd
*
* @comments
*
* @create    10/13/2003
*
* @author    mfiorito
*
* @end
*
**********************************************************************/

#include <stdio.h>

#include "commdefs.h"
#include "datatypes.h"
#include "sysapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "log.h"

#include "sshd_cfg.h"
#include "sshd_control.h"

extern void *sshdQueue;
sshdCnfgrState_t sshdCnfgrState = SSHD_PHASE_INIT_0;
extern sshdDeregister_t sshdDeregister;
extern sshdCfg_t *sshdCfg;
extern L7_int32 sshdEvTaskId;
void *sshdGlobalSema;
void *sshdChannelSema;
void *sshdSessionSema;
void *sshdLoggingSema;
void *sshdExitSema;
void *sshdSysExitSema;
extern void debug_reset(void);
extern void sshd_dounconfig(void);

/*********************************************************************
*
* @purpose  CNFGR System Initialization for ACL component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the SSHD comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void sshdApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
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
    if (sshdInit(pCmdData) != L7_SUCCESS)
      sshdInitUndo();
  }
  else
  {
    if (sshdIssueCmd(SSHD_CNFGR_EVENT, pCmdData) != L7_SUCCESS)
	  L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdApiCnfgrCommand: Failed calling "
								 "sshdIssueCmd. Failed to send the message to the SSHD message queue\n");
  }
}


/*********************************************************************
*
* @purpose  System Initialization for SSHD component
*
* @param    none
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t sshdInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CB_DATA_t cbData;

  sshdQueue = (void*)osapiMsgQueueCreate("sshdQueue",
                                          FD_CNFGR_SSHD_MSG_COUNT,
                                          (L7_uint32)sizeof(sshdMsg_t));
  if (sshdQueue == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdInit: msgQueue creation error. "
											   "Could not create the SSHD message (event) queue.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (sshdStartTasks() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (sshdIssueCmd(SSHD_CNFGR_EVENT, pCmdData) != L7_SUCCESS)
    LOG_COMPONENT_MSG(L7_FLEX_SSHD_COMPONENT_ID, "sshdInit: Failed calling sshdIssueCmd.\n");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for SSHD component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void sshdInitUndo()
{
  if (sshdQueue != L7_NULLPTR)
    osapiMsgQueueDelete(sshdQueue);


  if (sshdEvTaskId != L7_ERROR)
    osapiTaskDelete(sshdEvTaskId);

  sshdCnfgrState = SSHD_PHASE_INIT_0;
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
* @end
*********************************************************************/
L7_RC_t sshdCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t sshdRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  sshdRC      = L7_SUCCESS;


  sshdCfg    = osapiMalloc(L7_FLEX_SSHD_COMPONENT_ID, sizeof(sshdCfg_t));
  if (sshdCfg  == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    sshdRC     = L7_ERROR;

    return sshdRC;
  }

  memset((void *)sshdCfg, 0, sizeof(sshdCfg_t));
  /*
  Semaphore creation for global data protection -- this type of
  sema allows the same task to take the semaphore more than once
  as long as it is given the same number of times
  */

  sshdGlobalSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (sshdGlobalSema == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the global (data) "
							  "semaphore. Failed to create semaphore for global data protection.\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdChannelSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (sshdChannelSema == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the channel (data) "
				  "semaphore. Failed to create semaphore for protection of the SSHD data channels.\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdSessionSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (sshdSessionSema == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the session (data) "
						  "semaphore. Failed to create semaphore for protection of SSHD sessions.\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdLoggingSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (sshdLoggingSema == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the logging semaphore."
									    " Failed to create semaphore for protection of SSHD logging.\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdExitSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (sshdExitSema == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the clean exit semaphore."
							   " Failed to create semaphore for protection of SSHD clean exit routines.\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdSysExitSema = osapiSemaMCreate(OSAPI_SEM_Q_FIFO | OSAPI_SEM_DELETE_SAFE);
  if (sshdSysExitSema == L7_NULL)
  {
	L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "Unable to create the immediate exit semaphore."
					   " Failed to create semaphore for protection of SSHD immediate (error) exit routines.\n");
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdCnfgrState = SSHD_PHASE_INIT_1;

  return sshdRC;
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
* @end
*********************************************************************/
L7_RC_t sshdCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t sshdRC;
  nvStoreFunctionList_t sshdNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  sshdRC      = L7_SUCCESS;


  memset((void *) &sshdNotifyFunctionList, 0, sizeof(sshdNotifyFunctionList));
  sshdNotifyFunctionList.registrar_ID   = L7_FLEX_SSHD_COMPONENT_ID;
  sshdNotifyFunctionList.notifySave     = sshdSave;
  sshdNotifyFunctionList.hasDataChanged = sshdHasDataChanged;
  sshdNotifyFunctionList.resetDataChanged = sshdResetDataChanged;

  if (nvStoreRegister(sshdNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;

  }
  sshdDeregister.sshdSave                     = L7_FALSE;
  sshdDeregister.sshdRestore                  = L7_FALSE;
  sshdDeregister.sshdHasDataChanged           = L7_FALSE;

  sshdCnfgrState = SSHD_PHASE_INIT_2;

  return sshdRC;
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
* @end
*********************************************************************/
L7_RC_t sshdCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t sshdRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  sshdRC      = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_FLEX_SSHD_COMPONENT_ID, SSHD_CFG_FILENAME,
                   (L7_char8 *)sshdCfg, sizeof(sshdCfg_t),
                   &sshdCfg->checkSum, SSHD_CFG_VER_CURRENT,
                   sshdBuildDefaultConfigData, sshdMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    return sshdRC;
  }

  sshdApplyConfigData();

  sshdCnfgrState = SSHD_PHASE_INIT_3;

  return sshdRC;
}


/*********************************************************************
* @purpose  This function undoes sshdCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void sshdCnfgrFiniPhase1Process()
{

  if (sshdCfg != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_SSHD_COMPONENT_ID, sshdCfg);
    sshdCfg = L7_NULLPTR;
  }

  sshdInitUndo();

  sshdCnfgrState = SSHD_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes sshdCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void sshdCnfgrFiniPhase2Process()
{
  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the sshdDeregister is set to L7_FALSE;
   */

  sshdDeregister.sshdSave                     = L7_TRUE;
  sshdDeregister.sshdRestore                  = L7_TRUE;
  sshdDeregister.sshdHasDataChanged           = L7_TRUE;

  sshdCnfgrState = SSHD_PHASE_INIT_1;

}

/*********************************************************************
* @purpose  This function undoes sshdCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void sshdCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place sshdCnfgrState to WMU */
  sshdCnfgrUconfigPhase2(&response, &reason);

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
* @end
*********************************************************************/
L7_RC_t sshdCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t sshdRC = L7_SUCCESS;


  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(sshdRC);
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
* @end
*********************************************************************/

L7_RC_t sshdCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t sshdRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  sshdRC      = L7_SUCCESS;


  osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);
  sshd_dounconfig();
  if (sshdEventAdminModeSet(L7_DISABLE) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;
    sshdRC      = L7_ERROR;

    osapiSemaGive(sshdGlobalSema);
    return sshdRC;
  }
  osapiSemaGive(sshdGlobalSema);

  memset(sshdCfg, 0, sizeof(sshdCfg_t));

  sshdCnfgrState = SSHD_PHASE_WMU;

  return sshdRC;
}


/*********************************************************************
*
* @purpose  To parse the configurator commands send to sshdEvTask
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void sshdCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             sshdRC = L7_ERROR;
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
                if ((sshdRC = sshdCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  sshdCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((sshdRC = sshdCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  sshdCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((sshdRC = sshdCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  sshdCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                sshdRC = sshdCnfgrNoopProccess( &response, &reason );
                sshdCnfgrState = SSHD_PHASE_WMU;
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
                sshdCnfgrState = SSHD_PHASE_EXECUTE;

                sshdRC  = L7_SUCCESS;
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
                sshdRC = sshdCnfgrNoopProccess( &response, &reason );
                sshdCnfgrState = SSHD_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                sshdRC = sshdCnfgrUconfigPhase2( &response, &reason );
                sshdCnfgrState = SSHD_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            sshdRC = sshdCnfgrNoopProccess( &response, &reason );
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
  cbData.asyncResponse.rc = sshdRC;
  if (sshdRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);


  return;
}


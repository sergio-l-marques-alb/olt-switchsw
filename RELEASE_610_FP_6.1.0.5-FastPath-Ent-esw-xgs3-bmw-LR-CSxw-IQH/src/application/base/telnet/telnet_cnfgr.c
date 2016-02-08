/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  telnet_cnfgr.c
*
* @purpose   Implements the new stacking initialization sequence
*
* @component
*
* @comments  The main configurator dictates what sequence of state changes
*            by using the apt command/request pair. The current initialization
*            sequence for non-management units is: INIT_0, INIT_1, INIT_2 and WMU.
*            Management units have the sequence: INIT_0, INIT_1, INIT_2, WMU and INIT_3.
*            After the management unit (MU) has transitioned into INIT_3 the configurator
*            issues a command to all units (MU and non-MU) to move into the EXECUTE phase.
*
*            When a "clear config" command is issued, the sequence of state changes
*            is: EXECUTE,UNCONFIG_1, UNCONFIG_2, WMU.
*            The MU then goes into INIT_3 and subsequently all units go into EXECUTE phase.
*
* @create    02/27/2004
*
* @author    anindya
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "l7_common_l3.h"
#include "l7_cnfgr_api.h"
#include "defaultconfig.h"
#include "nvstoreapi.h"
#include "sysnet_api_ipv4.h"
#include "nimapi.h"
#include "l7_ip_api.h"
#include "l3_default_cnfgr.h"
#include "l7_telnetinclude.h"

/*************************************************************

*************************************************************/

extern L7_telnetMapCfg_t   *pTelnetMapCfgData;
extern telnetMapCtrl_t     telnetMapCtrl_g;

extern void telnetProcTask(void);

void *      telnetProcQueue;         /* TELNET Processing queue */
L7_int32    telnetProcTaskHandle;    /* TELNET Processing task handle */

L7_char8 *telnetCnfgrPhaseNames[] = {"INIT 0", "INIT 1", "INIT 2", "WMU", "INIT 3",
                                 "EXECUTE", "UNCONFIG 1", "UNCONFIG 2"};

L7_BOOL traceTELNETonfigState = L7_FALSE;
telnetCnfgrState_t telnetCnfgrState = TELNET_PHASE_INIT_0;/* To be init'ed to INIT_0 */

static L7_RC_t telnetCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                             L7_CNFGR_RESPONSE_t *response,
                                             L7_CNFGR_ERR_RC_t *reason);
static L7_RC_t telnetUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                            L7_CNFGR_RESPONSE_t *response,
                                            L7_CNFGR_ERR_RC_t *reason);


/*********************************************************************
*
* @purpose  Enabling tracing of configuration state changes.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void telnetCnfgrStateTraceEnable(void)
{
    traceTELNETonfigState = L7_TRUE;
}

/*********************************************************************
*
* @purpose  Disabling tracing of configuration state changes.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void telnetCnfgrStateTraceDisable(void)
{
    traceTELNETonfigState = L7_FALSE;
}

/*********************************************************************
*
* @purpose  Set the Outbound Telnet configuration state
*
* @param    newState - @b{(input)}  The configuration state that Outbound
*                                   Telnet is transitioning to.
*
* @returns  void
*
* @notes    Set traceTELNETonfigState to L7_TRUE to trace configuration state
*           transitions.
*
* @end
*********************************************************************/
static void telnetCnfgrStateSet(telnetCnfgrState_t newState)
{
    /* anindya: Check if the correct logging flag has been used */
    if (traceTELNETonfigState == L7_TRUE) {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Outbound Telnet configuration state set to %s\n",
                      telnetCnfgrPhaseNames[newState]);
    }
    telnetCnfgrState = newState;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair.
*
* @param    pCmdData  - @b{(input)}  command to be processed.
*
* @returns  void
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
* @notes    This function runs in the configurator's thread.
*
* @notes    Handles only Phase 1 initialization on the configurator thread.
*           Enqueues all other command/request pairs for processing task to handle.
*
* @end
*********************************************************************/
void telnetApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t         command;
  L7_CNFGR_RQST_t        request;
  L7_CNFGR_CB_DATA_t     cbData;
  L7_CNFGR_RESPONSE_t    response;
  L7_CNFGR_CORRELATOR_t  correlator = L7_NULL;

  L7_RC_t             rc     = L7_ERROR;
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
                if ((rc = telnetCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  telnetCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = telnetCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  telnetCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((rc = telnetCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  telnetCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = telnetCnfgrNoopProcess( &response, &reason );
                telnetCnfgrState = TELNET_PHASE_WMU;
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
                telnetCnfgrState = TELNET_PHASE_EXECUTE;

                rc  = L7_SUCCESS;
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
                rc = telnetCnfgrNoopProcess( &response, &reason );
                telnetCnfgrState = TELNET_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = telnetCnfgrUnconfigPhase2( &response, &reason );
                telnetCnfgrState = TELNET_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = telnetCnfgrNoopProcess( &response, &reason );
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
* @purpose  This function processes the configurator control commands/request
*           pair Init Phase 1. Init phase 1 mainly invloves allocation of memory,
*           creation of semaphores, message queues, tasks and other resources
*           used by the protocol
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
* @notes    This function runs in the configurator's thread.
*
* @end
*********************************************************************/
L7_RC_t telnetCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rc = L7_SUCCESS;

  /*-----------------------------------------------------------*/
  /* malloc space for the telnetMap configuration data structure  */
  /*-----------------------------------------------------------*/
  pTelnetMapCfgData = osapiMalloc(L7_SIM_COMPONENT_ID, sizeof(L7_telnetMapCfg_t));

  /* Make sure that allocation succeded */
  if (pTelnetMapCfgData == L7_NULL)
  {
      LOG_MSG("%s: %d: telnetCnfgrInitPhase1Process: Error allocating memory for Outbound Telnet \
                Config structure\n",__FILE__, __LINE__);

      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      rc = L7_ERROR;

      return rc;
  }

  memset(pTelnetMapCfgData, 0, sizeof(L7_telnetMapCfg_t));

  if(telnetMapAppsInit() != L7_SUCCESS)
  {
    LOG_MSG("%s: %d: telnetCnfgrInitPhase1Process: Error starting Outbound Telnet apps\n",
            __FILE__, __LINE__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;

    return rc;
  }

  telnetCnfgrState = TELNET_PHASE_INIT_1;

  return rc;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2. Phase 2 mainly involves callback registration.
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
* @notes    This function runs on the processing task.
*
*
* @end
*********************************************************************/
L7_RC_t telnetCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;
  nvStoreFunctionList_t   telnetNotifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rc = L7_SUCCESS;


    /*----------------------*/
    /* nvStore registration */
    /*----------------------*/
    memset((void *) &telnetNotifyFunctionList, 0, sizeof(nvStoreFunctionList_t));
    telnetNotifyFunctionList.registrar_ID   = L7_TELNET_MAP_COMPONENT_ID;
    telnetNotifyFunctionList.notifySave     = telnetSave;
    telnetNotifyFunctionList.hasDataChanged = telnetHasDataChanged;
    telnetNotifyFunctionList.resetDataChanged = telnetResetDataChanged;

    if(nvStoreRegister(telnetNotifyFunctionList) != L7_SUCCESS)
    {
        LOG_MSG("%s: %d: telnetCnfgrInitPhase2Process: Could not register with nvStore\n",
                __FILE__, __LINE__);
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;

    return rc;

    }

  telnetCnfgrState = TELNET_PHASE_INIT_2;

  return rc;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3. Phase 3 involves applying building and
*           applying the configuration onto the hardware.
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
L7_RC_t telnetCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                       L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rc = L7_SUCCESS;

    if (sysapiCfgFileGet(L7_TELNET_MAP_COMPONENT_ID, L7_TELNET_CFG_FILENAME,
                         (L7_char8 *)pTelnetMapCfgData, sizeof(L7_telnetMapCfg_t),
                         &pTelnetMapCfgData->checkSum, L7_TELNET_CFG_VER_CURRENT,
                         telnetBuildDefaultConfigData, telnetMigrateConfigData) != L7_SUCCESS)
    {
        LOG_MSG("%s: %d: telnetCnfgrInitPhase3Process: Outbound Telnet unable to read \
                config file\n", __FILE__, __LINE__);
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rc = L7_ERROR;

        return rc;
    }


    if(telnetApplyConfigData() != L7_SUCCESS)
    {
        LOG_MSG("%s: %d: telnetCnfgrInitPhase3Process: Outbound Telnet unable to apply \
                config\n", __FILE__, __LINE__);
        *pResponse = 0;
        *pReason   = L7_CNFGR_ERR_RC_FATAL;
        rc = L7_ERROR;

        return rc;
    }

  pTelnetMapCfgData->cfgHdr.dataChanged = L7_FALSE;

  telnetCnfgrState = TELNET_PHASE_INIT_3;

  return rc;
}


/*********************************************************************
* @purpose  This function undoes telnetCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void telnetCnfgrFiniPhase1Process()
{
  if (pTelnetMapCfgData != L7_NULLPTR)
  {
    osapiFree(L7_SIM_COMPONENT_ID, pTelnetMapCfgData);
    pTelnetMapCfgData = L7_NULLPTR;
  }

  telnetCnfgrState = TELNET_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes telnetCnfgrInitPhase2Process, i.e.
*           deregister from all callbacks.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void telnetCnfgrFiniPhase2Process()
{
  nvStoreDeregister(L7_TELNET_MAP_COMPONENT_ID);

  telnetCnfgrState = TELNET_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes telnetCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    telnetCnfgrUnconfigPhase2 unapplies the configuration.
*           Hence, is reused to undo INIT_3
*
* @end
*********************************************************************/
void telnetCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place telnetCnfgrState to WMU */
  telnetCnfgrUnconfigPhase2(&response, &reason);
}

/*********************************************************************
* @purpose  This function processes the configurator control commands/request
*           pair Unconfigure Phase 2. Mainly unapplies the current
*           configuration
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
*           telnetRestoreProcess only builds default configuration and
*           applies it, while UNCONFIG_2 requires configuration information
*           to be reset. Hence, telnetCfgData is reset to 0.
*
* @end
*********************************************************************/
L7_RC_t telnetCnfgrUnconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                    L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t rc;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;
  rc  = L7_SUCCESS;

  /* Clear out the configuration */
  memset(pTelnetMapCfgData, 0, sizeof(L7_telnetMapCfg_t));
  telnetMapAdminModeSet(L7_ENABLE);

  telnetCnfgrState = TELNET_PHASE_WMU;

  return rc;
}

/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator.
*
* @param    pCmdData - @b{(input)}  Indicates the command and request
*                                   from the configurator
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void telnetCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
    L7_CNFGR_CMD_t command;
    L7_CNFGR_RQST_t request;
    L7_CNFGR_CB_DATA_t cbData;
    L7_CNFGR_RESPONSE_t response;
    L7_RC_t telnetProcRC = L7_ERROR;
    L7_CNFGR_ERR_RC_t reason = L7_CNFGR_ERR_RC_INVALID_PAIR;

    if (pCmdData->type != L7_CNFGR_RQST)
    {
        cbData.correlator = L7_NULL;
        cbData.asyncResponse.rc = L7_ERROR;
        cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
        cnfgrApiCallback(&cbData);
        return;
    }

    command = pCmdData->command;
    request = pCmdData->u.rqstData.request;
    cbData.correlator = pCmdData->correlator;
    if ((request <= L7_CNFGR_RQST_FIRST) || (request >= L7_CNFGR_RQST_LAST))
    {
        /* invalid request */
        cbData.asyncResponse.rc = L7_ERROR;
        cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
        cnfgrApiCallback(&cbData);
        return;
    }

    switch(command)
    {
        case L7_CNFGR_CMD_INITIALIZE:
            telnetProcRC = telnetCnfgrInitRequestHandle(request, &response, &reason);
            break;

        case L7_CNFGR_CMD_EXECUTE:
            switch ( request )
            {
                case L7_CNFGR_RQST_E_START:
                    telnetCnfgrStateSet(TELNET_PHASE_EXECUTE);
                    telnetProcRC  = L7_SUCCESS;
                    response  = L7_CNFGR_CMD_COMPLETE;
                    reason    = 0;
                    break;

                default:
                    /* invalid command/request pair */
                    reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
                    break;
            }
            break;

        case L7_CNFGR_CMD_UNCONFIGURE:
            telnetProcRC = telnetUnconfigRequestHandle(request, &response, &reason);
            break;

        case L7_CNFGR_CMD_TERMINATE:
        case L7_CNFGR_CMD_SUSPEND:
            telnetProcRC = telnetCnfgrNoopProcess( &response, &reason );
            break;

        default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
    } /* endswitch command/event pair */

    /* respond to configurator */
    cbData.asyncResponse.rc = telnetProcRC;
    if (telnetProcRC == L7_SUCCESS)
        cbData.asyncResponse.u.response = response;
    else
        cbData.asyncResponse.u.reason = reason;

    cnfgrApiCallback(&cbData);
    return;
}

/*********************************************************************
*
* @purpose  Handles an initialization request from the configurator.
*
* @param @b{(input)} request - indicates the initialization phase to
*                              be executed
* @param @b{(output)} response - response if request successfully handled
* @param @b{(input)} reason - if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t telnetCnfgrInitRequestHandle(L7_CNFGR_RQST_t request,
                                             L7_CNFGR_RESPONSE_t *response,
                                             L7_CNFGR_ERR_RC_t *reason)
{
    L7_RC_t rc;
    switch ( request )
    {
        case L7_CNFGR_RQST_I_PHASE1_START:
            rc = telnetCnfgrInitPhase1Process(response, reason);
            if (rc != L7_SUCCESS)
            {
                /* Configurator will reset router, so no need to take
                 * any action.
                 */
                /* telnetCnfgrFiniPhase1Process(); */
            }
            break;
        case L7_CNFGR_RQST_I_PHASE2_START:
            rc = telnetCnfgrInitPhase2Process(response, reason);
            if (rc != L7_SUCCESS)
            {
                /* Configurator will reset router, so no need to take
                 * any action.
                 */
                /*  telnetCnfgrFiniPhase2Process(); */
            }
            break;
        case L7_CNFGR_RQST_I_PHASE3_START:
            rc = telnetCnfgrInitPhase3Process(response, reason);
            if (rc != L7_SUCCESS)
            {
                /* Configurator will reset router, so no need to take
                 * any action.
                 */
                /*  telnetCnfgrFiniPhase3Process(); */
            }
            break;
        case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            rc = telnetCnfgrNoopProcess(response, reason);
            telnetCnfgrStateSet(TELNET_PHASE_WMU);
            break;
        default:
            /* invalid request */
            rc = L7_ERROR;
            *reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
    }
    return rc;
}

/*********************************************************************
*
* @purpose  Handles an unconfigure request from the configurator.
*
* @param @b{(input)} request - indicates the level to which Outbound Telnet
*                              should be unconfigured
* @param @b{(output)} response - response if request successfully handled
* @param @b{(input)} reason - if an error, gives the reason
*
* @returns  L7_SUCCESS if request successfully handled
*           L7_ERROR if request is invalid
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t telnetUnconfigRequestHandle(L7_CNFGR_RQST_t request,
                                            L7_CNFGR_RESPONSE_t *response,
                                            L7_CNFGR_ERR_RC_t *reason)
{
    L7_RC_t rc;
    switch ( request )
    {
        case L7_CNFGR_RQST_U_PHASE1_START:
            rc = telnetCnfgrNoopProcess(response, reason);
            telnetCnfgrStateSet(TELNET_PHASE_UNCONFIG_1);
            break;

        case L7_CNFGR_RQST_U_PHASE2_START:
            rc = telnetCnfgrUnconfigPhase2(response, reason);
            telnetCnfgrStateSet(TELNET_PHASE_UNCONFIG_2);
            /* we need to do something with the stats in the future */
            break;

        default:
            /* invalid command/request pair */
            rc = L7_ERROR;
            *reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
            break;
    }
    return rc;
}

/*********************************************************************
* @purpose  Serves as a no-op for configurator stages that require
*           no action for Outbound Telnet. Simply sets pResponse to
*           L7_CNFGR_CMD_COMPLETE and returns L7_SUCCESS;
*
* @param    pResponse - @b{(output)}  Always set to L7_CNFGR_CMD_COMPLETE
*
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
*
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t telnetCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t *pReason)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return rc;
}

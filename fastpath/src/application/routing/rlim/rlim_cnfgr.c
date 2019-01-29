/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename rlim_cnfgr.c
*
* @purpose Contains definitions to support the new configurator API
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/16/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_cnfgr_api.h"
#include "platform_config.h"
#include "cnfgr.h"
#include "nvstoreapi.h"
#include "l3_commdefs.h"
#include "l7_ip_api.h"
#include "rto_api.h"
#include "rlim.h"

rlimCnfgrState_t rlimCnfgrState = RLIM_PHASE_INIT_0;

extern rlimDeregister_t rlimDeregister;


/*********************************************************************
*
* @purpose  CNFGR System Initialization for RLIM component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the rlim comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void rlimApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  rlimMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  if (pCmdData == L7_NULL)
  {
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

  /* Do minimum amound of work on configurator thread. Pass other work to
   * RLIM thread. */
  if ((pCmdData->command == L7_CNFGR_CMD_INITIALIZE) &&
      (pCmdData->u.rqstData.request == L7_CNFGR_RQST_I_PHASE1_START))
  {
    /* Create the message queue and the RLIM thread, then pass
     * a message via the queue to complete phase 1 initialization.
     */
    if (rlimThreadCreate() != L7_SUCCESS)
    {
      cbData.correlator = L7_NULL;
      cbData.asyncResponse.rc = L7_ERROR;
      cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      cnfgrApiCallback(&cbData);
      return;
    }
  }

  memcpy(&msg.type.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = RLIM_CNFGR_EVENT;
  if (osapiMessageSend(rlimOpData->cnfgrQueue, &msg, sizeof(rlimMsg_t),
                       L7_NO_WAIT, L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RLIM_COMPONENT_ID,
           "Failure sending RLIM configurator init message.");
  }

  return;
}


/*********************************************************************
*
* @purpose  Handles initialization messages from the configurator on
*           the RLIM thread.
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
void rlimCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;
  L7_RC_t             rlimRC = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason   = L7_CNFGR_ERR_RC_INVALID_PAIR;

  if ((pCmdData == L7_NULLPTR) || (pCmdData->type != L7_CNFGR_RQST))
  {
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_CMD;
    cnfgrApiCallback(&cbData);
    return;
  }

      command    = pCmdData->command;
      request    = pCmdData->u.rqstData.request;
      correlator = pCmdData->correlator;
  if ((request <= L7_CNFGR_RQST_FIRST) || (request >= L7_CNFGR_RQST_LAST))
  {
    /* invalid request */
    cbData.correlator = L7_NULL;
    cbData.asyncResponse.rc = L7_ERROR;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
    cnfgrApiCallback(&cbData);
    return;
  }

        switch ( command )
        {
          case L7_CNFGR_CMD_INITIALIZE:
            switch ( request )
            {
              case L7_CNFGR_RQST_I_PHASE1_START:
                rlimRC = rlimCnfgrInitPhase1Process(&response, &reason);
                if (rlimRC != L7_SUCCESS)
                {
                  rlimCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                rlimRC = rlimCnfgrInitPhase2Process(&response, &reason);
                if (rlimRC != L7_SUCCESS)
                {
                  rlimCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                rlimRC = rlimCnfgrInitPhase3Process(&response, &reason);
                if (rlimRC != L7_SUCCESS)
                {
                  rlimCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rlimRC = rlimCnfgrNoopProcess( &response, &reason );
                rlimCnfgrState = RLIM_PHASE_WMU;
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
                RLIM_SEMA_TAKE(rlimCreateDeleteMutex, L7_WAIT_FOREVER);
                RLIM_WRITE_LOCK_TAKE(rlimRWLock, L7_WAIT_FOREVER);
                rlimApplyConfigData();
                RLIM_WRITE_LOCK_GIVE(rlimRWLock);
                RLIM_SEMA_GIVE(rlimCreateDeleteMutex);
                rlimCnfgrState = RLIM_PHASE_EXECUTE;

                rlimRC  = L7_SUCCESS;
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
                rlimRC = rlimCnfgrUconfigPhase1( &response, &reason );
                rlimCnfgrState = RLIM_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rlimRC = rlimCnfgrUconfigPhase2( &response, &reason );
                rlimCnfgrState = RLIM_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rlimRC = rlimCnfgrNoopProcess( &response, &reason );
            break;

          default:
            reason = L7_CNFGR_ERR_RC_INVALID_CMD;
            break;
        } /* endswitch command/event pair */


  /* return value to caller -
   * <prepare complesion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rlimRC;
  if (rlimRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);
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
* @notes    This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t rlimCnfgrInitPhase1Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rlimRC;
  void *sema;
  L7_uint32 i;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  rlimRC = L7_SUCCESS;

  rlimDebugInit();

  if (osapiRWLockCreate(&rlimRWLock, OSAPI_RWLOCK_Q_PRIORITY) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rlimRC = L7_ERROR;

    return rlimRC;
  }

  rlimCreateDeleteMutex = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (rlimCreateDeleteMutex == L7_NULL)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rlimRC = L7_ERROR;

    return rlimRC;
  }

  rlimNhopChangeEventMutex = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (rlimNhopChangeEventMutex == L7_NULL)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rlimRC = L7_ERROR;

    return rlimRC;
  }

  /* Allocate memory for global data */
  rlimCfgData = osapiMalloc(L7_RLIM_COMPONENT_ID, sizeof(*rlimCfgData));
  rlimLoopbackOpData =
      osapiMalloc(L7_RLIM_COMPONENT_ID,
                  sizeof(*rlimLoopbackOpData) * RLIM_MAX_LOOPBACK_COUNT);
  rlimTunnelOpData =
      osapiMalloc(L7_RLIM_COMPONENT_ID,
                  sizeof(*rlimTunnelOpData) * RLIM_MAX_TUNNEL_COUNT);

  /* Make sure that allocation succeeded */
  if (rlimCfgData == L7_NULL ||
      rlimLoopbackOpData == L7_NULL || rlimTunnelOpData == NULL)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    rlimRC = L7_ERROR;
    return rlimRC;
  }

  memset(rlimCfgData, 0, sizeof(*rlimCfgData));
  memset(rlimLoopbackOpData, 0,
         sizeof(*rlimLoopbackOpData) * RLIM_MAX_LOOPBACK_COUNT);
  memset(rlimTunnelOpData, 0,
         sizeof(*rlimTunnelOpData) * RLIM_MAX_TUNNEL_COUNT);

  for (i = 0; i < RLIM_MAX_LOOPBACK_COUNT; i++)
  {
    sema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
    if (sema == L7_NULL)
    {
      *pResponse = 0;
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      rlimRC = L7_ERROR;

      return rlimRC;
    }
    rlimLoopbackOpData[i].deleteSema = sema;
  }

  for (i = 0; i < RLIM_MAX_TUNNEL_COUNT; i++)
  {
    sema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
    if (sema == L7_NULL)
    {
      *pResponse = 0;
      *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
      rlimRC = L7_ERROR;

      return rlimRC;
    }
    rlimTunnelOpData[i].deleteSema = sema;
  }

  for (i = 0; i <= L7_MAX_INTERFACE_COUNT; i++){
    rlimIntIfNumToTunnelId[i] = RLIM_MAX_TUNNEL_ID +1;
  }
#ifdef L7_IPV6_PACKAGE
  if(rlim6to4TunnelDestListCreate() != L7_SUCCESS){
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    rlimRC = L7_ERROR;

    return rlimRC;
  }
#endif
  rlimCnfgrState = RLIM_PHASE_INIT_1;

  return rlimRC;
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
* @notes    This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t rlimCnfgrInitPhase2Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rlimRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rlimRC   = L7_SUCCESS;

  memset(&notifyFunctionList, 0, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_RLIM_COMPONENT_ID;
  notifyFunctionList.notifySave     = rlimSave;
  notifyFunctionList.hasDataChanged = rlimHasDataChanged;
  notifyFunctionList.resetDataChanged = rlimResetDataChanged;

  if (nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rlimRC   = L7_ERROR;

    return rlimRC;
  }
  rlimDeregister.rlimSave = L7_FALSE;
  rlimDeregister.rlimRestore = L7_FALSE;
  rlimDeregister.rlimHasDataChanged = L7_FALSE;

  if (nimRegisterIntfChange(L7_RLIM_COMPONENT_ID,
                            rlimNimIntfChangeCallBack,
                            rlimNimIntfStartupCallback, NIM_STARTUP_PRIO_RLIM) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rlimRC = L7_ERROR;
  }

  if (ipMapRegisterRoutingEventChange(L7_IPRT_RLIM,
                          "rlimIp4RoutingEventChangeCallBack",
                          rlimIp4RoutingEventChangeCallBack) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rlimRC = L7_ERROR;
  }

  rlimCnfgrState = RLIM_PHASE_INIT_2;

  return rlimRC;
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
* @notes    This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t rlimCnfgrInitPhase3Process(L7_CNFGR_RESPONSE_t *pResponse,
                                   L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rlimRC;


  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  rlimRC   = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_RLIM_COMPONENT_ID, RLIM_CFG_FILENAME,
                       (L7_char8 *)rlimCfgData, sizeof(rlimCfgData_t),
                       &rlimCfgData->checkSum, RLIM_CFG_VER_CURRENT,
                       rlimBuildDefaultConfigData, L7_NULL) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    rlimRC   = L7_ERROR;

    return rlimRC;
  }

  rlimCfgData->cfgHdr.dataChanged = L7_FALSE;

  /* Reset the NIM port event mask for the events we are interested in receiving */
  memset(&rlimOpData->nimEventMask, 0, sizeof(rlimOpData->nimEventMask));

  nimRegisterIntfEvents(L7_RLIM_COMPONENT_ID, rlimOpData->nimEventMask);

  rlimCnfgrState = RLIM_PHASE_INIT_3;

  return rlimRC;
}

/*********************************************************************
* @purpose  This function undoes rlimCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void rlimCnfgrFiniPhase1Process()
{
  void *sema;
  L7_uint32 i;

  /* Deallocate anything that was allocated */

  osapiRWLockDelete(rlimRWLock);

  if (rlimCreateDeleteMutex != L7_NULL)
  {
    osapiSemaDelete(rlimCreateDeleteMutex);
    rlimCreateDeleteMutex = L7_NULL;
  }

  if (rlimNhopChangeEventMutex  != L7_NULL)
  {
    osapiSemaDelete(rlimNhopChangeEventMutex);
    rlimNhopChangeEventMutex = L7_NULL;
  }

  if (rlimCfgData != L7_NULL)
  {
    osapiFree(L7_RLIM_COMPONENT_ID, rlimCfgData);
    rlimCfgData = L7_NULL;
  }

  for (i = 0; i < RLIM_MAX_LOOPBACK_COUNT; i++)
  {
    sema = rlimLoopbackOpData[i].deleteSema;
    if (sema != L7_NULL)
    {
      osapiSemaDelete(sema);
    }
  }

  for (i = 0; i < RLIM_MAX_TUNNEL_COUNT; i++)
  {
    sema = rlimTunnelOpData[i].deleteSema;
    if (sema != L7_NULL)
    {
      osapiSemaDelete(sema);
    }
  }

  if (rlimOpData != L7_NULL)
  {
    osapiFree(L7_RLIM_COMPONENT_ID, rlimOpData);
    rlimOpData = L7_NULL;
  }

  if (rlimLoopbackOpData != L7_NULL)
  {
    osapiFree(L7_RLIM_COMPONENT_ID, rlimLoopbackOpData);
    rlimLoopbackOpData = L7_NULL;
  }

  if (rlimTunnelOpData != L7_NULL)
  {
    osapiFree(L7_RLIM_COMPONENT_ID, rlimTunnelOpData);
    rlimTunnelOpData = L7_NULL;
  }

#ifdef L7_IPV6_PACKAGE
  rlim6to4TunnelDestListDestroy();
#endif
  rlimCnfgrState = RLIM_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes rlimCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void rlimCnfgrFiniPhase2Process()
{

  /*
   * Currently there is no real way of deregistering with other
   * components this will be a work item for the future. For now
   * a temporary solution is set in place, where in the registered
   * function if we can execute the callback only if it's corresponding
   * member in the rlimDeregister is set to L7_FALSE;
   */

  rlimDeregister.rlimSave = L7_TRUE;
  rlimDeregister.rlimRestore = L7_TRUE;
  rlimDeregister.rlimHasDataChanged = L7_TRUE;

  rlimCnfgrState = RLIM_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes rlimCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void rlimCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place rlimCnfgrState to WMU */
  rlimCnfgrUconfigPhase2(&response, &reason);
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
* @notes    This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t rlimCnfgrNoopProcess(L7_CNFGR_RESPONSE_t *pResponse,
                             L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rlimRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = 0;
  return(rlimRC);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Unconfigure Phase 1.
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
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t rlimCnfgrUconfigPhase1(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason)
{

  L7_RC_t rlimRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  rlimRC = L7_SUCCESS;

  rlimRestoreProcess();

  rlimCnfgrState = RLIM_PHASE_WMU;

  return rlimRC;
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
* @notes    This function runs in the configurator's thread.
*           This thread MUST NOT be blocked!
*
* @end
*********************************************************************/
L7_RC_t rlimCnfgrUconfigPhase2(L7_CNFGR_RESPONSE_t *pResponse,
                               L7_CNFGR_ERR_RC_t   *pReason)
{
  L7_RC_t rlimRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  rlimRC = L7_SUCCESS;

  memset(rlimCfgData, 0, sizeof(*rlimCfgData));

  rlimCnfgrState = RLIM_PHASE_WMU;

  return rlimRC;
}

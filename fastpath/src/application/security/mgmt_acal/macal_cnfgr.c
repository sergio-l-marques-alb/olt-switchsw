/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2003-2006
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename macal_cnfgr.c
*
* @purpose Contains definitions to support the configurator API
*
* @component Management Access Control and Administration List
*
* @comments
*
* @create 05/0452005
*
* @author stamboli
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_cnfgr_api.h"
#include "log.h"
#include "osapi.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "macal.h"
#include "macal_cnfgr.h"

macalCnfgrState_t macalCnfgrState = MACAL_PHASE_INIT_0;

extern L7_int32       macalTaskId;
extern void          *macalQueue;
extern macalCfgData_t *macalCfgData;
/*extern void          *macalSemaphore;*/
extern osapiRWLock_t    macalRwLock;

/*********************************************************************
*
* @purpose  CNFGR System Initialization for Management Access Control and Administration List component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the MACAL comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void macalApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  macalMgmtMsg_t msg;

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
    if (macalInit(pCmdData) != L7_SUCCESS)
    {
      macalInitUndo();
    }
  }
  else
  {
    memcpy(&msg.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
    msg.msgId = macalCnfgr;
    osapiMessageSend(macalQueue, &msg, MACAL_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_HI);
  }
}

/*********************************************************************
*
* @purpose  System Initialization for Management Access Control and Administration List Component
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
L7_RC_t macalInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  macalMgmtMsg_t msg;
  L7_CNFGR_CB_DATA_t cbData;

  macalQueue = (void *)osapiMsgQueueCreate(MACAL_QUEUE, MACAL_MSG_COUNT, MACAL_MSG_SIZE);
  if (macalQueue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_MGMT_ACAL_COMPONENT_ID,
            "macalInit: msgQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    return L7_FAILURE;
  }

  if (macalStartTask() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    return L7_FAILURE;
  }

  memcpy(&msg.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.msgId = macalCnfgr;

  osapiMessageSend(macalQueue, &msg, MACAL_MSG_SIZE, L7_NO_WAIT, L7_MSG_PRIORITY_HI);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  System Init Undo for Management Access Control and Administration List component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void macalInitUndo()
{
  if (macalQueue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(macalQueue);
  }

  if (macalTaskId != L7_ERROR)
  {
    osapiTaskDelete(macalTaskId);
  }

  macalCnfgrState = MACAL_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  macalCfgData = osapiMalloc(L7_MGMT_ACAL_COMPONENT_ID, sizeof(macalCfgData_t));

  if (macalCfgData == L7_NULLPTR)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;

    return L7_ERROR;
  }

  memset((void *)macalCfgData, 0, sizeof(macalCfgData_t));

  if (osapiRWLockCreate(&macalRwLock, OSAPI_RWLOCK_Q_PRIORITY) != L7_SUCCESS)
  {
    *pResponse = 0;
    *pReason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;

    return L7_ERROR;
  }

  macalDebugLevel = 0;

  macalCnfgrState = MACAL_PHASE_INIT_1;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  nvStoreFunctionList_t macalNotifyFunctionList;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* Register to the NV Store */
  memset((void *) &macalNotifyFunctionList, 0, sizeof(macalNotifyFunctionList));
  macalNotifyFunctionList.registrar_ID     = L7_MGMT_ACAL_COMPONENT_ID;
  macalNotifyFunctionList.notifySave       = macalSave;
  macalNotifyFunctionList.hasDataChanged   = macalHasDataChanged;
  macalNotifyFunctionList.notifyConfigDump = L7_NULLPTR;

  if (nvStoreRegister(macalNotifyFunctionList) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;

    return L7_ERROR;
  }

  macalCnfgrState = MACAL_PHASE_INIT_2;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                     L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* Get the saved configuration information */
  if (sysapiCfgFileGet(L7_MGMT_ACAL_COMPONENT_ID, MACAL_CFG_FILENAME,
                       (L7_char8 *)macalCfgData, sizeof(macalCfgData_t),
                       &macalCfgData->checkSum, MACAL_CFG_VER_CURRENT,
                       macalBuildDefaultConfigData, L7_NULLPTR) != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;

    return L7_ERROR;
  }

  /* Apply the configuration */
  if (macalApplyConfigData() != L7_SUCCESS)
  {
    *pResponse  = 0;
    *pReason    = L7_CNFGR_ERR_RC_FATAL;

    return L7_ERROR;
  }

  macalCnfgrState = MACAL_PHASE_INIT_3;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This function undoes macalCnfgrInitPhase1Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrFiniPhase1Process()
{
  if (macalCfgData != L7_NULLPTR)
  {
    osapiFree(L7_MGMT_ACAL_COMPONENT_ID, macalCfgData);
    macalCfgData = L7_NULLPTR;
  }

  osapiRWLockDelete(macalRwLock);

  macalInitUndo();

  macalCnfgrState = MACAL_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  This function undoes macalCnfgrInitPhase2Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_MGMT_ACAL_COMPONENT_ID);

  macalCnfgrState = MACAL_PHASE_INIT_1;
}

/*********************************************************************
* @purpose  This function undoes macalCnfgrInitPhase3Process
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;

  /* this func will place macalCnfgrState to WMU */
  macalCnfgrUconfigPhase2(&response, &reason);
}

/*********************************************************************
* @purpose  This function process the configurator control commands/request
*           pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
* @param    pReason   - @b{(output)}  Always 0
*
* @returns  L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
* @notes    The following are valid error reason code:
*           None.
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrNoopProccess( L7_CNFGR_RESPONSE_t *pResponse,
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
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors. Response is available.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid response:
*           L7_CNFGR_CMD_COMPLETE
* @notes    The following are valid error reason code:
*           L7_CNFGR_ERR_RC_FATAL
*
* @end
*********************************************************************/
L7_RC_t macalCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                 L7_CNFGR_ERR_RC_t   *pReason )
{
  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = 0;

  /* Sysnet deregister for Management Access Control and Administration List */
  (void)macalSysnetRegisterDeregister(L7_FALSE);

  memset(macalCfgData, 0, sizeof(macalCfgData));

  macalCnfgrState = MACAL_PHASE_WMU;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To parse the configurator commands send to macalTask
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void macalCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             rc      = L7_ERROR;
  L7_CNFGR_ERR_RC_t   reason  = L7_CNFGR_ERR_RC_INVALID_PAIR;

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
                if ((rc = macalCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  macalCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((rc = macalCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  macalCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((rc = macalCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  macalCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                rc = macalCnfgrNoopProccess( &response, &reason );
                macalCnfgrState = MACAL_PHASE_WMU;
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
                macalCnfgrState = MACAL_PHASE_EXECUTE;

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
            switch ( request )
            {
              case L7_CNFGR_RQST_U_PHASE1_START:
                rc = macalCnfgrNoopProccess( &response, &reason );
                macalCnfgrState = MACAL_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                rc = macalCnfgrUconfigPhase2( &response, &reason );
                macalCnfgrState = MACAL_PHASE_UNCONFIG_2;
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            rc = macalCnfgrNoopProccess( &response, &reason );
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

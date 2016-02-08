/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename auto_install_control.c
*
* @purpose   Auto-instal Control file
*
* @component auto_install
*
* @comments
*
* @create    28/12/2007
*
* @author    lkhedyk
*
* @end
*
**********************************************************************/

#include <string.h>
#include <stdio.h>
#include "log.h"
#include "nvstoreapi.h"
#include "auto_install_control.h"
#include "auto_install_cnfgr.h"
#include "auto_install_debug.h"
#include "config_script_api.h"
#include "dns_client_api.h"
#include "simapi.h"
#include "bspapi.h"
#include "dtlapi.h"

#include "osapi_support.h"

#include "auto_install_exports.h"
#include "usmdb_sim_api.h"

/* auto-install retry timer */
static osapiTimerDescr_t *autoInstallRestartTimer = L7_NULLPTR;

static L7_BOOL autoInstallNetCfgFailed = L7_FALSE;

/* auto-install run time variables/parameters */
autoInstallRunTimeData_t *autoInstallRunTimeData = L7_NULLPTR;

/* Externals */

/* auto_istall message queue ID */
extern void * autoInstallMsgQueue;

/* auto_istall task queue ID */
extern L7_int32 autoInstallTaskId;

/* current auto-install configuration phase */
extern autoInstallCnfgrState_t autoInstallCnfgrState;

static L7_RC_t autoInstallRegisteredComponentsNotify(autoInstallNotifyEvent_t event);
/* Forwards && externals */

       void autoInstallDispatchCmd(autoInstallMsg_t *msg);
extern void autoInstallCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

extern L7_RC_t cliAutoInstallConfigScriptApply(L7_char8 * filename);

/* auto-install state machine states strings */
L7_uchar8 *autoInstallStateStrs[AUTO_INSTALL_MAX_STATE + 1] =
{
  "AUTO_INSTALL_START_STATE",
  "AUTO_INSTALL_DHCP_STATE",
  "AUTO_INSTALL_GET_SPECIFIC_FILE_STATE",
  "AUTO_INSTALL_GET_PREDEFINED_FILE_STATE",
  "AUTO_INSTALL_GET_HOST_FILE_STATE",
  "AUTO_INSTALL_APPLY_STATE",
  "AUTO_INSTALL_WAIT_FOR_RESTART_STATE",
  "AUTO_INSTALL_AUTO_SAVE_STATE",
  "AUTO_INSTALL_FINISHED_STATE",
  "AUTO_INSTALL_STOP_STATE",
  "AUTO_INSTALL_MAX_STATE"
};

/* auto-install state machine events strings */
static char *autoInstallStateMachineEventStrs[AUTO_INSTALL_MAX_EVENT + 1] =
{
  "AUTO_INSTALL_DHCP_MSG_EVENT",
  "AUTO_INSTALL_DHCP_DONE_EVENT",
  "AUTO_INSTALL_EXECUTION_EVENT",
  "AUTO_INSTALL_SPECIFIC_FILE_FAILED_EVENT",
  "AUTO_INSTALL_SPECIFIC_COUNT_MORE_6",
  "AUTO_INSTALL_SPECIFIC_FILE_OK_EVENT",
  "AUTO_INSTALL_PREDEFINED_FILE_FAILED_EVENT",
  "AUTO_INSTALL_PREDEFINED_COUNT_MORE_6",
  "AUTO_INSTALL_PREDEFINED_FILE_OK_EVENT",
  "AUTO_INSTALL_HOSTNAME_RESOLVED_EVENT",
  "AUTO_INSTALL_HOST_COUNT_MORE_6",
  "AUTO_INSTALL_HOST_FILE_OK_EVENT",
  "AUTO_INSTALL_HOST_FILE_FAILED_EVENT",
  "AUTO_INSTALL_SAVE_CONFIG_EVENT",
  "AUTO_INSTALL_FINISHED_EVENT",
  "AUTO_INSTALL_TIMER_EXPIRE_EVENT",
  "AUTO_INSTALL_UI_STOP_EVENT",
  "AUTO_INSTALL_UI_START_EVENT",
  "AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT",
  "AUTO_INSTALL_UI_RETRY_INFINITE_DISABLE_EVENT",
  "AUTO_INSTALL_UI_AUTO_SAVE_ENABLE_EVENT",      
  "AUTO_INSTALL_AUTO_SAVE_DISABLE_EVENT", 
  "AUTO_INSTALL_MAX_EVENT"
};

static L7_uchar8 bindIntfName[16];
static L7_uchar8 ipCompleteString[OSAPI_INET_NTOA_BUF_SIZE];
static L7_uchar8 completeFileName[BOOTP_FILE_LEN];

/*********************************************************************
* @purpose  auto_install task which serves the request queue
*
* @param
*
* @returns  void
*
* @comments Handling Configurator events.
*
* @end
*********************************************************************/
void autoInstallTask()
{
  L7_RC_t          rc  = L7_FAILURE;
  autoInstallMsg_t msg;

  memset(&msg, 0x00, sizeof(autoInstallMsg_t));

  rc = osapiTaskInitDone(L7_AUTO_INSTALL_TASK_SYNC);

  while(1)
  {
    /* Get next message. */
    rc = osapiMessageReceive(autoInstallMsgQueue,
                             &msg,
                             sizeof(autoInstallMsg_t),
                             L7_WAIT_FOREVER);

    if (rc == L7_SUCCESS)
    {
      autoInstallDispatchCmd(&msg);
    }
    else
    {
      LOG_MSG("autoInstallTask: message recv failed event %d\n", msg.event);
    } 
  
  } /* while(1) */
}

/*********************************************************************
* @purpose  Route the event to a handling function and grab the parms
*
* @param    msg  @b{(input)} auto_install message
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallDispatchCmd(autoInstallMsg_t *msg)
{
  switch (msg->event)
  {
    case AUTO_INSTALL_CNFGR_EVENT:
      autoInstallCnfgrParse( &(msg->msgData.cnfgrData));
      break;
  
    case AUTO_INSTALL_DHCP_BOOTP_EVENT:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Dhcp_Bootp_Interact,
                            "%s [%u]:received options from DHCP/BOOTP\n",
                            __FUNCTION__, __LINE__);
      /* store recieved options */
      memcpy(&(autoInstallRunTimeData->bootConfigParams),
             &(msg->msgData.bootParams),
             sizeof(L7_bootp_dhcp_t));
      /* generate appropriate event */
      autoInstallStateMachineEventProcess(AUTO_INSTALL_DHCP_MSG_EVENT);
      break;
    
    case AUTO_INSTALL_RESTART_TIMER_EVENT:
      autoInstallStateMachineEventProcess(AUTO_INSTALL_TIMER_EXPIRE_EVENT);
      break;
    
    case AUTO_INSTALL_STATE_MACHINE_EVENT:
      autoInstallStateMachineEventProcess(msg->msgData.smEvent);
      break;
      
    case AUTO_INSTALL_UI_EVENT:
      if (msg->msgData.uiEventMsg.uiEvent == AUTO_INSTALL_UI_START_STOP_EVENT)
      {
        if (msg->msgData.uiEventMsg.uiMsg.startStop == L7_TRUE)
        {
          autoInstallStateMachineEventProcess(AUTO_INSTALL_UI_START_EVENT);
        }
        else
        {
          autoInstallRunTimeData->isFirstCycle = L7_TRUE;   
          autoInstallStateMachineEventProcess(AUTO_INSTALL_UI_STOP_EVENT);
        }
      }
      else if (msg->msgData.uiEventMsg.uiEvent == AUTO_INSTALL_UI_AUTO_SAVE_EVENT)
      {
        if (msg->msgData.uiEventMsg.uiMsg.autoSave == L7_TRUE)
        {
          autoInstallStateMachineEventProcess(AUTO_INSTALL_UI_AUTO_SAVE_ENABLE_EVENT);
        }
      }
      break;

    case AUTO_INSTALL_TRANSFER_STATUS_EVENT:
        if (autoInstallRunTimeData->dlCompletionHandler != L7_NULLPTR)
        {
            autoInstallRunTimeData->transferStatus = msg->msgData.transferStatus;   
            autoInstallConfigFileViaTFTPComplete();
        }
      break;

    default:
      LOG_MSG("AutoInstall : Unrecognized event %d\n", msg->event);
      break;
      
  } /* switch (msg.event) */
}

/*********************************************************************
* @purpose  Send auto-install state machine event to input message queue
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallStateMachineEventGenerate (autoInstalStateMachineEvent_t event)
{
  autoInstallMsg_t msg;
  
  msg.event = AUTO_INSTALL_STATE_MACHINE_EVENT;
  msg.msgData.smEvent = event;

  if (osapiMessageSend(autoInstallMsgQueue,
                       &msg, 
                       sizeof(autoInstallMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallStateMachineEventGenerate(): message send failed!\n");
  }
  
  return; 
}

/*********************************************************************
* @purpose  Process auto-install state machine events
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallStateMachineEventProcess (autoInstalStateMachineEvent_t event)
{
  /* range check */
  if ((event <  AUTO_INSTALL_DHCP_MSG_EVENT) || 
      (event >= AUTO_INSTALL_MAX_EVENT))
  {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
                "AutoInstall : Event is out of range %d", event);
      return;
  }
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "%s (%d): state machine recieved event (%s)\n",
                        __FUNCTION__, __LINE__, autoInstallStateMachineEventStrs[event]);
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "%s (%d): state machine current state (%s)\n",
                        __FUNCTION__, __LINE__, autoInstallStateStrs[autoInstallRunTimeData->currentState]);
  
  /* call the appropriate event handler */
  if (autoInstallRunTimeData->eventHandlers[autoInstallRunTimeData->currentState] == L7_NULLPTR)
  {
    LOG_MSG("AutoInstall : No event handler for state (%s)", autoInstallStateStrs[autoInstallRunTimeData->currentState]);
    return;
  }
  (void)(*(autoInstallRunTimeData->eventHandlers[autoInstallRunTimeData->currentState]))(event);
}

/*********************************************************************
* @purpose  Switch to the specified state and perform 
*           appropriate actions.
*
* @param    autoInstalState_t state  @b{(input)} state to switch
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallStateMachineTransit (autoInstalState_t state)
{
  /* range check */
  if ((state <  AUTO_INSTALL_START_STATE) || 
      (state >= AUTO_INSTALL_MAX_STATE))
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
              "AutoInstall : State is out of range %d", state);
    return;
  }
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "%s (%d): state machine transits to state (%s)\n",
                        __FUNCTION__, __LINE__, autoInstallStateStrs[state]);
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "%s (%d): state machine current state (%s)\n",
                        __FUNCTION__, __LINE__, autoInstallStateStrs[autoInstallRunTimeData->currentState]);
  
  /* switch current state */
  autoInstallRunTimeData->currentState = state;
  
  /* call the appropriate state handler */
  if (autoInstallRunTimeData->stateHandlers[state] == L7_NULLPTR)
  {
    LOG_MSG("AutoInstall : No state handler for state (%d)\n", state);
    return;
  }
  (void)(*(autoInstallRunTimeData->stateHandlers[state]))();
}

/*********************************************************************
*
*  auto-install events handlers
*
*********************************************************************/

/*********************************************************************
* @purpose  Handle auto-install events for START state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallStartStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_DHCP_MSG_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_DHCP_MSG_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_EXECUTION_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_EXECUTION_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }
  
  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }

  if ((autoInstallRunTimeData->currentEvents[AUTO_INSTALL_DHCP_MSG_EVENT] == L7_TRUE) &&
      (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_EXECUTION_EVENT] == L7_TRUE))
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_DHCP_STATE);
    return;
  }
}

/*********************************************************************
* @purpose  Handle auto-install events for DHCP state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallDhcpStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_DHCP_DONE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_DHCP_DONE_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_FINISHED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_FINISHED_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }

  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_FINISHED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_FINISHED_STATE);
    return;
  }

  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_DHCP_DONE_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_SPECIFIC_FILE_STATE);
    return;
  }
}

/*********************************************************************
* @purpose  Handle auto-install events for GET_SPECIFIC_FILE state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallSpecificFileStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_SPECIFIC_FILE_FAILED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SPECIFIC_FILE_FAILED_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_SPECIFIC_COUNT_MORE_6:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SPECIFIC_COUNT_MORE_6] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_SPECIFIC_FILE_OK_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SPECIFIC_FILE_OK_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_RETRY_INFINITE_DISABLE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_DISABLE_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SPECIFIC_FILE_FAILED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_SPECIFIC_FILE_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SPECIFIC_COUNT_MORE_6] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_PREDEFINED_FILE_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SPECIFIC_FILE_OK_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_APPLY_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_DISABLE_EVENT] == L7_TRUE)
  {
    /* Since the unicast retries is finite now, the TFTP fetch should continue from broadcast 
       fetches of specific file. The succeeding fetch for predefined files follows if needed based 
       on the status of broadcast TFTP for specified file. */
    if (autoInstallRunTimeData->specificFileRetryCounter >= autoInstallRunTimeData->unicastRetryCnt)
    {
      autoInstallRunTimeData->specificFileRetryCounter = autoInstallRunTimeData->unicastRetryCnt;
    }
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_SPECIFIC_FILE_STATE);
    return;
  }

}

/*********************************************************************
* @purpose  Handle auto-install events for GET_PREDEFINED_FILE state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallPredefinedFileStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_PREDEFINED_FILE_FAILED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_PREDEFINED_FILE_FAILED_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_PREDEFINED_COUNT_MORE_6:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_PREDEFINED_COUNT_MORE_6] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_PREDEFINED_FILE_OK_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_PREDEFINED_FILE_OK_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_HOSTNAME_RESOLVED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOSTNAME_RESOLVED_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT] = L7_TRUE;
      break;
      
    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_PREDEFINED_FILE_FAILED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_PREDEFINED_FILE_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_PREDEFINED_COUNT_MORE_6] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_WAIT_FOR_RESTART_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_PREDEFINED_FILE_OK_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_APPLY_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOSTNAME_RESOLVED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_HOST_FILE_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_SPECIFIC_FILE_STATE);
    return;
  }
}

/*********************************************************************
* @purpose  Handle auto-install events for GET_HOST_FILE state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallHostFileStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_HOST_FILE_FAILED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOST_FILE_FAILED_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_HOST_COUNT_MORE_6:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOST_COUNT_MORE_6] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_HOST_FILE_OK_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOST_FILE_OK_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOST_FILE_FAILED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_HOST_FILE_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOST_COUNT_MORE_6] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_WAIT_FOR_RESTART_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_HOST_FILE_OK_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_APPLY_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_SPECIFIC_FILE_STATE);
    return;
  }
  
}

/*********************************************************************
* @purpose  Handle auto-install events for APPLY state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallApplyStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_FINISHED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_FINISHED_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_SAVE_CONFIG_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SAVE_CONFIG_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_FINISHED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_FINISHED_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_SAVE_CONFIG_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_AUTO_SAVE_STATE);
    return;
  }

}

/*********************************************************************
* @purpose  Handle auto-install events for WAIT_FOR_RESTART state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallRetryStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_TIMER_EXPIRE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_TIMER_EXPIRE_EVENT] = L7_TRUE;
      break;
      
    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallRestartTimerDelete();
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_TIMER_EXPIRE_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_DHCP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_GET_SPECIFIC_FILE_STATE);
    return;
  }

}

/*********************************************************************
* @purpose  Handle auto-install events for FINISHED state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallFinishedStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_UI_AUTO_SAVE_ENABLE_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_AUTO_SAVE_ENABLE_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_AUTO_SAVE_ENABLE_EVENT] == L7_TRUE)
  {
    /* Go to AUTO_SAVE state only when the configuration is not saved explicitly.*/
    if (autoInstallStartupConfigIsExist() != L7_TRUE)
    {
      autoInstallStateMachineTransit(AUTO_INSTALL_AUTO_SAVE_STATE);
    }
    return;
  }
}

/*********************************************************************
* @purpose  Handle auto-install events for AUTO_SAVE state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallAutoSaveStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_UI_STOP_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] = L7_TRUE;
      break;

    case AUTO_INSTALL_FINISHED_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_FINISHED_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_STOP_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_STOP_STATE);
    return;
  }
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_FINISHED_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_FINISHED_STATE);
    return;
  }
}

/*********************************************************************
* @purpose  Handle auto-install events for STOPPED state
*
* @param    autoInstalStateMachineEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallStoppedStateEventProcess (autoInstalStateMachineEvent_t event)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* raise the event */
  switch (event)
  {
    case AUTO_INSTALL_UI_START_EVENT:
      autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_START_EVENT] = L7_TRUE;
      break;

    default:
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "%s [%u]:Unexpected event %d at state %d\n",
                            __FUNCTION__, __LINE__,
                            event, autoInstallRunTimeData->currentState);
      return;
      break;
  }

  /* perform transitions depending on raised events */
  if (autoInstallRunTimeData->currentEvents[AUTO_INSTALL_UI_START_EVENT] == L7_TRUE)
  {
    autoInstallStateMachineTransit(AUTO_INSTALL_DHCP_STATE);
    return;
  }
}

/*********************************************************************
*
*  auto-install state handlers
*
*********************************************************************/

/*********************************************************************
* @purpose  Handle activities for START state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallStartStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();
  
  /* Seems, we had been restarted by user */
  /* Just wait for DHCP options notification */
  return;
}

/*********************************************************************
* @purpose  Handle activities for DHCP state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallDhcpStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();
  
  if (autoInstallRunTimeData->isFirstCycle == L7_TRUE)
  {
    LOG_MSG("AutoInstall process started...\n");  
  }
  
  autoInstallRunTimeData->isFirstCycle = L7_FALSE;
  
  /* if startup config is present finish the auto-install procedure */
  if (autoInstallStartupConfigIsExist() == L7_TRUE)
  {
    LOG_MSG("AutoInstall : startup configuration exists. AutoInstall is stopped.\n");
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_FINISHED_EVENT);
    return;
  }

  /* set up download retry counters */
  autoInstallRunTimeData->specificFileRetryCounter   = L7_NULL;
  autoInstallRunTimeData->predefinedFileRetryCounter = L7_NULL;
  autoInstallRunTimeData->hostFileRetryCounter       = L7_NULL;  
  
  /* look for TFTP server IP in DHCP/BOOTP options */
  (void)autoInstallTftpSvrIpAddressResolve(&(autoInstallRunTimeData->tftpIpAddress));
  /* If broadcast destination is specified then skip unicats requests */
  if (autoInstallRunTimeData->tftpIpAddress == L7_IP_LTD_BCAST_ADDR)
  {
    autoInstallRunTimeData->specificFileRetryCounter   = autoInstallRunTimeData->unicastRetryCnt;
    autoInstallRunTimeData->predefinedFileRetryCounter = autoInstallRunTimeData->unicastRetryCnt;
  }
  
  /* look for bootfile name in DHCP/BOOTP options */
  if ((autoInstallRunTimeData->bootConfigParams.offeredOptionsMask & BOOTP_DHCP_BOOTFILE) != L7_NULL) 
  {
    osapiStrncpy(autoInstallRunTimeData->bootFileName, autoInstallRunTimeData->bootConfigParams.bootFile, BOOTP_FILE_LEN);
  }
  if ((autoInstallRunTimeData->bootConfigParams.offeredOptionsMask & BOOTP_DHCP_OPTION_BOOTFILE) != L7_NULL)
  {
    osapiStrncpy(autoInstallRunTimeData->bootFileName, autoInstallRunTimeData->bootConfigParams.optBootFile, BOOTP_FILE_LEN);
  }
  
  autoInstallStateMachineEventGenerate(AUTO_INSTALL_DHCP_DONE_EVENT);
  return;
}

/*********************************************************************
* @purpose  Handle activities for GET_SPECIFIC_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallSpecificFileStateProcess (void)
{
  L7_inet_addr_t        tftpIpAddress;
  L7_uchar8            *bootFileName   = autoInstallRunTimeData->bootFileName; 
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;

  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);
 
  /* clean up raised events first */
  autoInstallStateMachineEventsClear();

  inetAddressSet(L7_AF_INET, &autoInstallRunTimeData->tftpIpAddress, &tftpIpAddress);
  /* After unicast requests failed, set destination address to broadcast */
  if (autoInstallRunTimeData->specificFileRetryCounter >= autoInstallRunTimeData->unicastRetryCnt)
  {
    L7_uint32 ipAddr = L7_IP_LTD_BCAST_ADDR;
    inetAddressSet(L7_AF_INET, &ipAddr, &tftpIpAddress);
  }
    
  /* no <bootfile>.cfg specefied -> force switching to the predefined files */
  if ((bootFileName == L7_NULLPTR) ||
      (strlen(bootFileName) == L7_NULL))
  {
    autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup, "No Boot File specified\n");
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_SPECIFIC_COUNT_MORE_6);
    return;
  }
  
  /* Set up the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = autoInstallSpecificFileStateComplete;

  /* Initiate download config file to temporary script file on local system */
  if (autoInstallConfigFileDownload(&tftpIpAddress, bootFileName, &transferStatus) != L7_SUCCESS)
  {
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "Failed initaiting download for file %s \n",
                            bootFileName);
      /* Attempt failed.  Force switching to the predefined files */
      autoInstallStateMachineEventGenerate(AUTO_INSTALL_SPECIFIC_COUNT_MORE_6);
      return;
  }

}

/*********************************************************************
* @purpose  Handle activities for GET_SPECIFIC_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallSpecificFileStateComplete (void)
{
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;

  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* Clear the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = L7_NULLPTR;
  
  /* Download config file to temporary script file on local system */
  if (autoInstallRunTimeData->transferStatus == L7_TRANSFER_SUCCESS)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_SPECIFIC_FILE_OK_EVENT);
    return;
  }
  else if (transferStatus == L7_TRANSFER_FILE_NOT_FOUND)
  {
    /* take a break before further actions */
    osapiSleep(AUTO_INSTALL_FAILED_REQUEST_DELAY);
  }
  
  /* TFTP get failed - retry */
  autoInstallRunTimeData->specificFileRetryCounter++;

  /* Notify registered components when 
     (i) unicast attempts for specified file crosses AUTO_INSTALL_TFTP_UNICAST_REQUESTS_NUMBER(3).
     (ii)broadcast attempts for specified file crosses AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER(6).
     i.e to notify users only three retries are  attempted. Broadcast also need to be considered 
     as that may be initial tries(then counter starts from 3)if the TFTP server name is not 
     specified by the DHCP response. */
  if (((autoInstallRunTimeData->tftpIpAddress != L7_IP_LTD_BCAST_ADDR) &&
      (autoInstallRunTimeData->specificFileRetryCounter >= autoInstallRunTimeData->unicastRetryCnt)) ||
      ((autoInstallRunTimeData->tftpIpAddress == L7_IP_LTD_BCAST_ADDR) &&
      (autoInstallRunTimeData->specificFileRetryCounter >= AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER)))
  {    
    /* Its now resposibility of the receiver to fetch the configuration file and apply.
       In Metro, TR-069 is notified here.*/
    if (autoInstallRegisteredComponentsNotify(AUTO_INSTALL_SPECIFIED_FILE_FAILED) != L7_SUCCESS)
    {
      if (dtlLedBlinkSet(L7_TRUE) != L7_SUCCESS)
      {
        LOG_MSG("AutoInstall : Failed to set the port to blink.\n");
      }
    }
  }

  if (autoInstallRunTimeData->specificFileRetryCounter < AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_SPECIFIC_FILE_FAILED_EVENT);
    return;
  }

  /* Too much retries - switch to predefined files */
  autoInstallStateMachineEventGenerate(AUTO_INSTALL_SPECIFIC_COUNT_MORE_6);
  return;  
}

/*********************************************************************
* @purpose  Handle activities for GET_PREDEFINED_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallPredefinedFileStateProcess (void)
{
  L7_inet_addr_t        tftpIpAddress;
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;
  L7_uchar8             configFileName[AUTO_INSTALL_HOST_FILENAME_LENGTH];
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();
 
  memset(configFileName, 0, sizeof(configFileName));
  
  tftpIpAddress.family = L7_AF_INET;
  tftpIpAddress.addr.ipv4.s_addr = autoInstallRunTimeData->tftpIpAddress;
  
  /* After unicast requests failed, set destination address to broadcast */
  if (autoInstallRunTimeData->predefinedFileRetryCounter >= autoInstallRunTimeData->unicastRetryCnt)
  {
    L7_uint32 ipAddr = L7_IP_LTD_BCAST_ADDR;

    inetAddressSet(L7_AF_INET, &ipAddr, &tftpIpAddress);
  }

  /* Set up the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = autoInstallPredefinedFileStateComplete1;

  /* Download and apply fp-net.cfg to determine own hostname */    
  if (autoInstallConfigFileDownload(&tftpIpAddress, AUTO_INSTALL_NETWORK_CONFIG_FILE, &transferStatus) != L7_SUCCESS)
  {
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "Failed initiating download for file %s \n",
                            AUTO_INSTALL_NETWORK_CONFIG_FILE);
      /* Signal next completion routine that the network config file download attempt failed */
      autoInstallNetCfgFailed = L7_TRUE;
      autoInstallPredefinedFileStateComplete1();
      return;
  }

}

/*********************************************************************
* @purpose  Handle activities for GET_PREDEFINED_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallPredefinedFileStateComplete1 (void)
{
  L7_inet_addr_t        tftpIpAddress;
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;
  L7_uchar8             configFileName[AUTO_INSTALL_HOST_FILENAME_LENGTH];
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);


  /* Clear the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = L7_NULLPTR;
  /* Download and apply fp-net.cfg to determine own hostname */ 
  if (autoInstallNetCfgFailed == L7_FALSE)
  {
      /* Check for successful status */
      if (autoInstallRunTimeData->transferStatus == L7_TRANSFER_SUCCESS)
      {
          /* WPJ TBD:  Look into whether the underlying cliTxtCfg code needs protection from dual accesses */
    (void)cliAutoInstallConfigScriptApply(AUTO_INSTALL_TEMP_CONFIG_FILENAME);
          L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
                  "AutoInstall<->CLI : Applied %s", autoInstallRunTimeData->currentFileName);
          osapiFsDeleteFile(AUTO_INSTALL_TEMP_CONFIG_FILENAME);
      }
  }

  /* resolve the switch hostname and make up <hostname>.cfg filename */
  if (autoInstallBootFileNameResolve(configFileName) == L7_SUCCESS)
  {
    /* download <hostname>.cfg on GET_HOST_FILE state*/
    memset(autoInstallRunTimeData->hostFileName, 0 , sizeof(autoInstallRunTimeData->hostFileName));
    osapiStrncpySafe(autoInstallRunTimeData->hostFileName, configFileName, AUTO_INSTALL_HOST_FILENAME_LENGTH);
    
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_HOSTNAME_RESOLVED_EVENT);
    return; 
  }
    
  tftpIpAddress.family = L7_AF_INET;
  tftpIpAddress.addr.ipv4.s_addr = autoInstallRunTimeData->tftpIpAddress;

  /* Set up the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = autoInstallPredefinedFileStateComplete2;

  /* attempt to download router.cfg - default config file */
  /* Initiate download config file to temporary script file on local system */
  if (autoInstallConfigFileDownload(&tftpIpAddress, AUTO_INSTALL_DEFAULT_CONFIG_FILE, &transferStatus) != L7_SUCCESS)
  {

      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "Failed initaiting download for file %s \n",
                            AUTO_INSTALL_DEFAULT_CONFIG_FILE);
      /* Attempt failed.  Force switching to the predefined files 

         From the existing code path, this attempt should only fail in parsing the config file.
         This should not happen, but just in case it does, force the next transition */

      autoInstallStateMachineEventGenerate(AUTO_INSTALL_PREDEFINED_COUNT_MORE_6);
      return;
  }

}

/*********************************************************************
* @purpose  Handle activities for GET_PREDEFINED_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallPredefinedFileStateComplete2 (void)
{
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;

  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* Clear the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = L7_NULLPTR;
    
  /* attempt to download router.cfg - default config file */
  if (autoInstallRunTimeData->transferStatus == L7_TRANSFER_SUCCESS)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_PREDEFINED_FILE_OK_EVENT);
    return;
  }

  if (transferStatus != L7_TRANSFER_SUCCESS)
  {
    /* take a break before further requests */
    osapiSleep(AUTO_INSTALL_FAILED_REQUEST_DELAY);
  }
   
  /* TFTP get failed - retry */
  autoInstallRunTimeData->predefinedFileRetryCounter++;

  /* Notify registered components when 
     (i) unicast attempts for specified file crosses AUTO_INSTALL_TFTP_UNICAST_REQUESTS_NUMBER(3).
     (ii)broadcast attempts for specified file crosses AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER(6).
     i.e to notify users only three retries are  attempted. Broadcast also need to be considered 
     as that may be initial tries(then counter starts from 3)if the TFTP server name is not 
     specified by the DHCP response. */
  if (((autoInstallRunTimeData->tftpIpAddress != L7_IP_LTD_BCAST_ADDR) &&
      (autoInstallRunTimeData->predefinedFileRetryCounter >= autoInstallRunTimeData->unicastRetryCnt)) ||
      ((autoInstallRunTimeData->tftpIpAddress == L7_IP_LTD_BCAST_ADDR) &&
      (autoInstallRunTimeData->predefinedFileRetryCounter >= AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER)))
  {    
    /* Its now resposibility of the receiver to fetch the configuration file and apply.
       In Metro, TR-069 is notified here.*/
    if (autoInstallRegisteredComponentsNotify(AUTO_INSTALL_SPECIFIED_FILE_FAILED) != L7_SUCCESS)
    {
      if (dtlLedBlinkSet(L7_TRUE) != L7_SUCCESS)
      {
        LOG_MSG("AutoInstall : Failed to set the port to blink.\n");
      }
    }
    return;
  }

  if (autoInstallRunTimeData->predefinedFileRetryCounter < AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_PREDEFINED_FILE_FAILED_EVENT);
    return;
  }

  /* Too much retries - switch to predefined files */
  autoInstallStateMachineEventGenerate(AUTO_INSTALL_PREDEFINED_COUNT_MORE_6);
  return;  
}

/*********************************************************************
* @purpose  Handle activities for GET_HOST_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallHostFileStateProcess(void)
{
  L7_inet_addr_t        tftpIpAddress;
  L7_uchar8            *hostFileName   = autoInstallRunTimeData->hostFileName; 
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;

  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();

  tftpIpAddress.family = L7_AF_INET;
  tftpIpAddress.addr.ipv4.s_addr = autoInstallRunTimeData->tftpIpAddress;
  
  /* After unicast requests failed, set destination address to broadcast */
  if (autoInstallRunTimeData->hostFileRetryCounter >= autoInstallRunTimeData->unicastRetryCnt)
  {
    tftpIpAddress.addr.ipv4.s_addr = L7_IP_LTD_BCAST_ADDR;
  }
    
  /* Set up the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = autoInstallHostFileStateComplete;

  /* Download and apply fp-net.cfg to determine own hostname */    
  if (autoInstallConfigFileDownload(&tftpIpAddress, hostFileName, &transferStatus) != L7_SUCCESS)
  {
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                            "Failed initaiting download for file %s \n",
                            hostFileName);
      /* Attempt failed.  Force switching to the predefined files 

         From the existing code path, this attempt should only fail in parsing the config file.
         This should not happen, but just in case it does, force the next transition */

      autoInstallStateMachineEventGenerate(AUTO_INSTALL_HOST_COUNT_MORE_6);
      return;
  }

}

/*********************************************************************
* @purpose  Handle activities for GET_HOST_FILE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallHostFileStateComplete(void)
{
  L7_TRANSFER_STATUS_t  transferStatus = L7_TRANSFER_FAILED;

  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* Clear the completion handler for the download */
  autoInstallRunTimeData->dlCompletionHandler   = L7_NULLPTR;
    
  /* Download config file to temporary script file on local system */
  if (autoInstallRunTimeData->transferStatus == L7_TRANSFER_SUCCESS)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_HOST_FILE_OK_EVENT);
    return;
  }
  else if (transferStatus == L7_TRANSFER_FILE_NOT_FOUND)
  {
    /* take a break before further actions */
    osapiSleep(AUTO_INSTALL_FAILED_REQUEST_DELAY);
  }
  
  /* TFTP get failed - retry */
  autoInstallRunTimeData->hostFileRetryCounter++;
  if (autoInstallRunTimeData->hostFileRetryCounter < AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_HOST_FILE_FAILED_EVENT);
    return;
  }

  /* Too much retries - switch to predefined files */
  autoInstallStateMachineEventGenerate(AUTO_INSTALL_HOST_COUNT_MORE_6);
  return; 
}

/*********************************************************************
* @purpose  Handle activities for APPLY state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallApplyStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();

  /* Validate downloaded configuration and apply it on running configuration */
  if (cliAutoInstallConfigScriptApply(AUTO_INSTALL_TEMP_CONFIG_FILENAME) == L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall<->CLI : Applied %s", autoInstallRunTimeData->currentFileName);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall<->CLI : File %s is inconsistent", autoInstallRunTimeData->currentFileName);
  }

  osapiFsDeleteFile(AUTO_INSTALL_TEMP_CONFIG_FILENAME);

  /* Switch to the next state */
  if (autoInstallRunTimeData->autoSave == L7_TRUE)
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_SAVE_CONFIG_EVENT);
  }
  else
  {
    autoInstallStateMachineEventGenerate(AUTO_INSTALL_FINISHED_EVENT);
  }
  
  return;
}

/*********************************************************************
* @purpose  Handle activities for WAIT_FOR_RESTART state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallRetryStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();

  /* start retry timer */
  autoInstallRestartTimerAdd();

  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, 
          "AutoInstall : Waiting for retry timeout\n");

  return;
}

/*********************************************************************
* @purpose  Handle activities for FINISHED state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallFinishedStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();

  L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_AUTO_INSTALL_COMPONENT_ID, 
                      "%s: AutoInstall process is completed.",
                      __FUNCTION__);

  return;
}


/*********************************************************************
* @purpose  Handle activities for AUTO_SAVE state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallAutoSaveStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();

  if (autoInstallRunTimeData->autoSave == L7_TRUE)
  {
    if (nvStoreSave(L7_SAVE_CONFIG_TYPE_STARTUP) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, 
              "Saving AutoInstall configuration failed.\n");
      return;
    }
  }
  
  sysapiPrintf("AutoInstalled configuration is saved.\n");
  L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, "AutoInstalled configuration is saved.\n");

  /* Switch to the next state */
  autoInstallStateMachineEventGenerate(AUTO_INSTALL_FINISHED_EVENT);

  return;
}
/*********************************************************************
* @purpose  Handle activities for STOPPED state
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
static void autoInstallStoppedStateProcess (void)
{
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "entering %s \n",
                        __FUNCTION__);

  /* clean up raised events first */
  autoInstallStateMachineEventsClear();
  
  /* now perform run-time variables cleanup */
  memset(autoInstallRunTimeData->bootFileName, 0, BOOTP_FILE_LEN + 1);
  autoInstallRunTimeData->currentIpAddress = L7_NULL;
  
  LOG_MSG("AutoInstall : process is stopped\n");
    
  return;
}

/*********************************************************************
*
*  auto-install core functions
*
*********************************************************************/

/*********************************************************************
* @purpose  Send our own internal message to notify task of 
*           obtaining configuration
*
* @param    L7_bootp_dhcp_t pBootConfigParams @b{(input)} Boot params
*           structure
*
* @returns  L7_RC_t  
*
* @notes   we register this notify function in cnfgr phase 2, using
*          the L7_AUTO_INSTALL_COMPONENT_ID. 
*       
* @end
*********************************************************************/
L7_RC_t autoInstallNotificationCallback(L7_bootp_dhcp_t *pBootConfigParams)
{
  autoInstallMsg_t msg;
  
  autoInstallDebugTrace( AUTO_INSTALL_DBG_FLAG_Dhcp_Bootp_Interact,
                         "AutoInstall : recieved boot options from DHCP/BOOTP\n");
  if (autoInstallRunTimeData->startStop == L7_FALSE)
  {
    return L7_SUCCESS;
  }  

  msg.event = AUTO_INSTALL_DHCP_BOOTP_EVENT;
  msg.msgData.bootParams = *pBootConfigParams;

  if (osapiMessageSend(autoInstallMsgQueue, 
                       &msg, 
                       sizeof(autoInstallMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallNotificationCallback(): message send failed!\n");
    return L7_ERROR;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if text-based startup-config file is present
*
* @param    
*
* @returns  L7_BOOL  L7_TRUE if file exist, otherwise L7_FALSE
*       
* @end
*********************************************************************/
L7_BOOL autoInstallStartupConfigIsExist()
{
  L7_int32 fileDesc;
  
  /* SYSAPI_TXTCFG_FILENAME is defined as "startup-config" */
  fileDesc = osapiFsOpen(SYSAPI_TXTCFG_FILENAME);
      
  if ( fileDesc != L7_ERROR )
  {
    L7_uint32 startupFileLen = L7_NULL;
    L7_RC_t   rc             = L7_SUCCESS;
    
    osapiFsClose(fileDesc);
    rc = osapiFsFileSizeGet(SYSAPI_TXTCFG_FILENAME, &startupFileLen);
    if ((rc == L7_SUCCESS) && (startupFileLen != L7_NULL))
    {
      return L7_TRUE;
    }
  }
  
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Resolves hostname for current system
*
* @param    L7_uint32 pIpAddr   @b{(output)} pointer to hold
*           resolved ip address
*
* @returns  L7_RC_t  
*
* @end
*********************************************************************/
L7_RC_t autoInstallTftpSvrIpAddressResolve(L7_uint32 *pIpAddr)
{
  L7_RC_t                 rc          = L7_FAILURE;
  L7_char8               *pHostName   = L7_NULLPTR;
  L7_bootp_dhcp_t        *pBootParams = &(autoInstallRunTimeData->bootConfigParams);
  dnsClientLookupStatus_t status      = DNS_LOOKUP_STATUS_FAILURE;
  L7_char8                hostFQDN[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_inet_addr_t inetIp;
  L7_uchar8 dispStr[IPV6_DISP_ADDR_LEN];

  memset(hostFQDN, 0, DNS_DOMAIN_NAME_SIZE_MAX);

  *pIpAddr = 0;
  
  if ((pBootParams->offeredOptionsMask & BOOTP_DHCP_SNAME) != L7_NULL)
  {
    pHostName = pBootParams->sname;
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall<->DNS : Resolving IP for hostname (%s)\n", pHostName);
    rc = dnsClientNameLookup(pHostName, &status, hostFQDN, pIpAddr);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall<->DNS lookup for TFTP server address. DNS lookup status: %s\n",
            (status == DNS_LOOKUP_STATUS_SUCCESS) ? "success" : "failed");   
  }
  
  if (rc != L7_SUCCESS && ((pBootParams->offeredOptionsMask & BOOTP_DHCP_OPTION_TFTP_SVR_NAME) != L7_NULL))
  {
    pHostName = pBootParams->optTftpSvrName;
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall<->DNS : Resolving IP for hostname (%s)\n", pHostName);
    rc = dnsClientNameLookup(pHostName, &status, hostFQDN, pIpAddr);
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall<->DNS lookup for TFTP server address. DNS lookup status: %s\n",
            (status == DNS_LOOKUP_STATUS_SUCCESS) ? "success" : "failed");
  }
  
  if ( rc != L7_SUCCESS )
  {
    if ( (pBootParams->offeredOptionsMask & BOOTP_DHCP_OPTION_TFTP_SVR_ADDR) != L7_NULL )
    {
      *pIpAddr = pBootParams->optTftpSvrIpAddr;
      rc = L7_SUCCESS;
    }
    else if ( (pBootParams->offeredOptionsMask & BOOTP_DHCP_SIADDR) != L7_NULL )
    {
      *pIpAddr = pBootParams->siaddr;
      rc = L7_SUCCESS;
    }
    else
    {
      *pIpAddr = L7_IP_LTD_BCAST_ADDR;
      rc = L7_SUCCESS;      
    }
  }
    
  inetAddressSet(L7_AF_INET, pIpAddr, &inetIp);
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "TFTP IP address : %s, in decimal %d\n", inetAddrPrint(&inetIp,dispStr), *pIpAddr);
    
  return rc;
}

/*********************************************************************
* @purpose  Get specified configuration file via TFTP and save
*           it to temporary config file
*
* @param    L7_inet_addr_t inet_address  @b{(input)} ip address of
*           TFTP server
*
* @param    L7_uchar8 pConfigFileName    @b{(input)} name of the file
*           to download
*
* @param    L7_TRANSFER_STATUS_t pTransferStatus @b{(output)} status
*           of file transfer
*
* @returns  L7_RC_t  
*
* @end
*********************************************************************/
L7_RC_t autoInstallConfigFileDownload( L7_inet_addr_t *inet_address, L7_uchar8 *pConfigFileName,
                                       L7_TRANSFER_STATUS_t *pTransferStatus )
{
  L7_RC_t rc = L7_SUCCESS;
  
  /* set up current auto-install parameters */
  autoInstallRunTimeData->currentIpAddress = inet_address->addr.ipv4.s_addr;
  osapiStrncpy(autoInstallRunTimeData->currentFileName, pConfigFileName, BOOTP_FILE_LEN);
  
  /* get specified file */
  if (autoInstallConfigFileViaTFTPGet(inet_address, pConfigFileName) != L7_SUCCESS)
  {
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup, 
                            "Failed in %s\n",
                            __FUNCTION__);
      return L7_FAILURE;
  }
  
  osapiFsDeleteFile(pConfigFileName);
  
  return rc;
}

/*********************************************************************
* @purpose  Get configuration file via TFTP and save it to
*           temporary config file
*
* @param    L7_inet_addr_t inet_addr   @b{(input)} ip address of
*           TFTP server
*
* @param    L7_uchar8 pConfigFileName  @b{(input)} name of the file
*           to download
*
*
* @returns  L7_TRANSFER_STATUS_t  
*
* @end
*********************************************************************/
L7_RC_t autoInstallConfigFileViaTFTPGet(L7_inet_addr_t *inet_addr, L7_uchar8 *pConfigFileName)
{ 
  L7_uint32 servPortConfigMode = L7_SYSCONFIG_MODE_NOT_COMPLETE;
  L7_uint32 networkConfigMode  = L7_SYSCONFIG_MODE_NOT_COMPLETE;
  L7_uchar8 portString[AUTO_INSTALL_PORT_DESCR_STR_LEN]; /* Holds string that decribes type of port used.
                                                            Possible values: "Service port" or "Network port" */
  L7_char8 remoteFilePath[L7_MAX_FILEPATH+1];
  L7_char8 remoteFileName[L7_MAX_FILENAME+1];    
  memset(remoteFilePath, 0, sizeof(remoteFilePath));
  memset(remoteFileName, 0, sizeof(remoteFileName));  

  /* Initialize parms */
  memset(bindIntfName, 0, sizeof(bindIntfName));
  memset(ipCompleteString, 0, sizeof(ipCompleteString));    
  memset(portString, 0, sizeof(portString));

  osapiStrncpy(completeFileName, pConfigFileName, BOOTP_FILE_LEN);
  
  osapiInetNtoa(inet_addr->addr.ipv4.s_addr, ipCompleteString);  
  servPortConfigMode = simGetServPortConfigMode();
  networkConfigMode  = simGetSystemConfigMode();
  
  /* TFTP traffic must be binded to active port */
  /* Get service port name */
  if ((servPortConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
      (servPortConfigMode == L7_SYSCONFIG_MODE_BOOTP))
  {
    osapiSnprintf(bindIntfName, sizeof(bindIntfName), "%s0", bspapiServicePortNameGet());
    osapiSnprintf(portString, sizeof(portString), "%s", "Service port");
  }
  /* Get network port name*/
  else if ((networkConfigMode == L7_SYSCONFIG_MODE_DHCP) ||
           (networkConfigMode == L7_SYSCONFIG_MODE_BOOTP))
  {
    osapiSnprintf(bindIntfName, sizeof(bindIntfName), "%s0", L7_DTL_PORT_IF);
    osapiSnprintf(portString, sizeof(portString), "%s", "Network port");
  }
  
  if (autoInstallTftpFileNameParse(pConfigFileName, remoteFilePath, remoteFileName) != L7_SUCCESS) 
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, 
              "autoInstallConfigFileViaTFTPGet() : error while parsing DHCP/BOOTP offered bootfile name");  
    return L7_FAILURE;  
  }
  
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Tftp_Interact, 
                        "AutoInstall<->TFTP : Downloading tftp://%s/%s (via %s)\n", 
                        ipCompleteString, pConfigFileName, bindIntfName);
  
  /* Initialize the transfer status */
  autoInstallRunTimeData->transferStatus = L7_NO_STATUS_CHANGE;  /* WPJ TBD :  This may not matter, or be reliable */

  if (simTransferInProgressGet() == L7_TRUE) 
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
              "autoInstallConfigFileViaTFTPGet() : Another Transfer is in Progress");
    return L7_FAILURE;
  }

  /* setup transfer parameters */
  simSetTransferUnitNumber(L7_NULL);
  simSetTransferDownloadFileType(L7_FILE_TYPE_AUTO_INSTALL_SCRIPT);
  simSetTransferMode(L7_TRANSFER_TFTP);
  simSetTransferFileNameLocal(AUTO_INSTALL_TEMP_SCRIPT_FILENAME);
  simSetTransferFilePathRemote(remoteFilePath);
  simSetTransferFileNameRemote(remoteFileName);
  simSetTransferServerAddressType((L7_uint32)inet_addr->family);
  simSetTransferServerIp((L7_uchar8 *)(&(inet_addr->addr)));

  /* initiate the transfer  */
  /* All downloading will be done underneath the L7_transfer_task, 
     and the transfer status will be freed in that task.

     The autoInstall task will wait for completion of the transfer, 
     and then invoke the completion routines for whatever phase of 
     autoInstall is in progress.

     */
  simSetTransferCompleteNotification(autoInstallTransferStatusCallback);  
  L7_transfer_start(L7_TRANSFER_DIRECTION_DOWN);

  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                        "Exiting function %s\n",
                        __FUNCTION__);

  return L7_SUCCESS;    
}

/*********************************************************************
* @purpose  Complete the download of configuration file via TFTP and save it to
*           temporary config file
*
* @param    void
*
*
* @returns  L7_TRANSFER_STATUS_t  
*
* @end
*********************************************************************/
void autoInstallConfigFileViaTFTPComplete(void)
{ 
    L7_uint32 fileSize;
    L7_RC_t rc = L7_FAILURE;

/* WPJ_TBD:  Store the following parms for retrieval before starting the transfer

   L7_inet_addr_t *inet_addr;
    L7_uchar8 *pConfigFileName;
    L7_uchar8 bindIntfName[IFNAMSIZ];
    L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];

*/

    /* Initialize parms */

    fileSize            = L7_NULL;

  /* the status should be updated by callback */
  if ( autoInstallRunTimeData->transferStatus == L7_TRANSFER_SUCCESS )
  {
    autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Tftp_Interact,
                          "AutoInstall<->TFTP : Received %s from %s (via %s)\n", 
                          completeFileName, ipCompleteString, bindIntfName);
    rc = osapiFsFileSizeGet(completeFileName, &fileSize);
    if ((rc == L7_SUCCESS) && (fileSize != L7_NULL)) 
    {
        autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Tftp_Interact,
                              "[OK - %u bytes]\n",
                              fileSize); 
    }
  }
  else
  {
    if ( autoInstallRunTimeData->transferStatus == L7_TRANSFER_FILE_NOT_FOUND )
    {
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
              "AutoInstall<->TFTP : Error opening tftp://%s/%s (via %s)"
              "(No such file or directory)\n", ipCompleteString, 
               completeFileName, bindIntfName);  
    }
    
    else if ( autoInstallRunTimeData->transferStatus == L7_FLASH_FAILED )
    {

        LOG_MSG("AutoInstall<->TFTP : Error writing file to memory: tftp://%s/%s (via %s) (No such file or directory)\n",
                ipCompleteString, completeFileName, bindIntfName);  
    }
    
    else if (autoInstallRunTimeData->transferStatus == L7_TRANSFER_FAILED)
    {
      LOG_MSG("AutoInstall<->TFTP : Error opening tftp://%s/%s (via %s) (Timed out)\n",
              ipCompleteString, completeFileName, bindIntfName);  
    }
  }

  autoInstallRunTimeData->dlCompletionHandler();

  memset(bindIntfName, 0, sizeof(bindIntfName));
  memset(ipCompleteString, 0, sizeof(ipCompleteString));
  memset(completeFileName, 0, sizeof(completeFileName));

  return; 
}

/*********************************************************************
* @purpose  Creates specified file on local filesystem
*
* @param    L7_uchar8 pFileName  @b{(input)}  file name
*
* @param    L7_int32             @b{(output)} descriptor of created file
*
* @returns  void  
*       
* @end
*********************************************************************/
L7_int32 autoInstallTmpScriptFileCreate(L7_uchar8 *pFileName)
{
  L7_int32 cfgFileDesc = L7_NULL;
  
  osapiFsDeleteFile(pFileName);
  if (osapiFsCreateFile(pFileName) == L7_ERROR)
  {
    return L7_NULL;
  }

  cfgFileDesc = osapiFsOpen (pFileName);
    
  if (cfgFileDesc == L7_ERROR)
  {
    return L7_NULL;
  }
    
  return cfgFileDesc;
}

/*********************************************************************
* @purpose  Resolves hostname for current system
*
* @param    L7_char8 configFileName   @b{(output)} configuration file name
*
* @param    L7_uint32 fileNameLength  @b{(input)} length of configuration
*           file name buffer
*
* @returns  L7_RC_t  
*
* @end
*********************************************************************/
L7_RC_t autoInstallBootFileNameResolve(L7_char8 *configFileName)
{
  L7_bootp_dhcp_t         *pBootParams   = &(autoInstallRunTimeData->bootConfigParams);
  dnsClientLookupStatus_t  status        = DNS_LOOKUP_STATUS_FAILURE;
  L7_char8                 hostFQDN[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_char8                 hostname[DNS_DOMAIN_NAME_SIZE_MAX];
  L7_char8                 ipString[OSAPI_INET_NTOA_BUF_SIZE];
  L7_inet_addr_t           inetIp;

  memset(hostFQDN, 0, DNS_DOMAIN_NAME_SIZE_MAX);
  memset(hostname, 0, DNS_DOMAIN_NAME_SIZE_MAX);  
  memset(configFileName, 0, AUTO_INSTALL_HOST_FILENAME_LENGTH);     
  memset(ipString,0, sizeof(ipString));

  osapiInetNtoa(pBootParams->ip, ipString);
  autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Dns_Interact, "%s [%u]:"
                        "AutoInstall<->DNS: Resolving hostname for IP" 
                        "address (%s)\n", __FUNCTION__, __LINE__, ipString);
  
  if ((pBootParams->offeredOptionsMask & BOOTP_DHCP_IP_ADDR) == L7_NULL)
  {
    return L7_FAILURE;
  }

  inetAddressZeroSet(L7_AF_INET, &inetIp);
  inetAddressSet(L7_AF_INET, &pBootParams->ip, &inetIp);
  if (dnsClientReverseNameLookup(inetIp, hostname, &status, hostFQDN) == L7_SUCCESS )
  {
    /* Config filename is "<hostname>.cfg", where <hostname> is 32 first characters of hostname */
    osapiSnprintf(configFileName, AUTO_INSTALL_HOST_FILENAME_LENGTH, "%.32s.cfg", hostname);
  }
  
  if (strlen(configFileName) == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_AUTO_INSTALL_COMPONENT_ID, 
            "AutoInstall : DNS lookup for hostname. DNS lookup status: failed.");
    return L7_FAILURE;
  }
  else
  {
    autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Dns_Interact, 
                          "%s [%u]:AutoInstall<->DNS lookup for hostname."
                          "DNS lookup status: success. hostname - %s\n", 
                          __FUNCTION__, __LINE__, hostname);
    return L7_SUCCESS;
  }
}

/*********************************************************************
*
* @purpose Adds restart timer
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void autoInstallRestartTimerAdd(void)
{
  osapiTimerAdd((void*) autoInstallRestartTimerCallback,
                 L7_NULL,
                 L7_NULL,
                 AUTO_INSTALL_RESTART_TIMEOUT,
                 &autoInstallRestartTimer);
}

/*********************************************************************
*
* @purpose callback to handle the restart timer events
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void autoInstallRestartTimerCallback()
{
  autoInstallMsg_t msg;
  
  msg.event = AUTO_INSTALL_RESTART_TIMER_EVENT;

  if (osapiMessageSend(autoInstallMsgQueue,
                       &msg, 
                       sizeof(autoInstallMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallRestartTimerCallback(): message send failed!\n");
  }
  
  autoInstallRestartTimerDelete();
}

/*********************************************************************
*
* @purpose Deletes restart timer
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void autoInstallRestartTimerDelete()
{
  if (autoInstallRestartTimer != L7_NULLPTR)
  {
    osapiTimerFree(autoInstallRestartTimer);  /* Destroy the timer */
    autoInstallRestartTimer = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose callback to handle the transfer status check
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void autoInstallTransferStatusCallback(L7_TRANSFER_STATUS_t status)
{

  autoInstallMsg_t msg;

  autoInstallDebugTrace( AUTO_INSTALL_DBG_FLAG_ControlGroup,
                         "AutoInstall : received transfer status callback: %x\n", status);

  msg.event = AUTO_INSTALL_TRANSFER_STATUS_EVENT;
  msg.msgData.transferStatus = status;

  if (osapiMessageSend(autoInstallMsgQueue,
                       &msg, 
                       sizeof(autoInstallMsg_t), 
                       L7_NO_WAIT, 
                       L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
  {
    LOG_MSG("autoInstallTransferStatusCallback(): message send failed!\n");
    return;
  }
  return;
}

/*********************************************************************
*
*  auto-install state machine helper functions
*
*********************************************************************/

/*********************************************************************
* @purpose  Clear all raised state machine events
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallStateMachineEventsClear(void)
{
  L7_uint8 i = 0;
  
  for (i = 0; i < AUTO_INSTALL_MAX_EVENT; i++)
  {
    autoInstallRunTimeData->currentEvents[i] = L7_FALSE;
  }
  return;
}

/*********************************************************************
* @purpose  Bind state handling function pointers
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallBindStateHandlers (void)
{
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_START_STATE]               = autoInstallStartStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_DHCP_STATE]                = autoInstallDhcpStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_GET_SPECIFIC_FILE_STATE]   = autoInstallSpecificFileStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_GET_PREDEFINED_FILE_STATE] = autoInstallPredefinedFileStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_GET_HOST_FILE_STATE]       = autoInstallHostFileStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_APPLY_STATE]               = autoInstallApplyStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_WAIT_FOR_RESTART_STATE]    = autoInstallRetryStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_FINISHED_STATE]            = autoInstallFinishedStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_AUTO_SAVE_STATE]           = autoInstallAutoSaveStateProcess;
  autoInstallRunTimeData->stateHandlers[AUTO_INSTALL_STOP_STATE]                = autoInstallStoppedStateProcess;
  return;
}

/*********************************************************************
* @purpose  Bind event handling function pointers
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void autoInstallBindEventHandlers (void)
{
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_START_STATE]               = autoInstallStartStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_DHCP_STATE]                = autoInstallDhcpStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_GET_SPECIFIC_FILE_STATE]   = autoInstallSpecificFileStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_GET_PREDEFINED_FILE_STATE] = autoInstallPredefinedFileStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_GET_HOST_FILE_STATE]       = autoInstallHostFileStateEventProcess;  
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_APPLY_STATE]               = autoInstallApplyStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_WAIT_FOR_RESTART_STATE]    = autoInstallRetryStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_FINISHED_STATE]            = autoInstallFinishedStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_AUTO_SAVE_STATE]           = autoInstallAutoSaveStateEventProcess;
  autoInstallRunTimeData->eventHandlers[AUTO_INSTALL_STOP_STATE]                = autoInstallStoppedStateEventProcess;
}
/*********************************************************************
* @purpose  Notify the registered components for an event
*
* @param    autoInstallNotifyEvent_t event  @b{(input)} event to process
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t autoInstallRegisteredComponentsNotify(autoInstallNotifyEvent_t event)
{
  L7_uint32 index;

  for (index = 0; index <AUTO_INSTALL_NUM_EVENT_REGISTRATIONS_MAX; index ++)
  {
    if ((autoInstallRunTimeData->notifyList[index].notifyEvent & event) > L7_NULL)
    {
      if (autoInstallRunTimeData->notifyList[index].notifyChange != L7_NULLPTR)
      {
        if (autoInstallRunTimeData->notifyList[index].notifyChange() != L7_SUCCESS)
        {
          autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_ControlGroup,
                                "Notification failed for component %d \n",
                                __FUNCTION__, autoInstallRunTimeData->notifyList[index].compId);
          return L7_FAILURE;
        }
      }
      return L7_SUCCESS;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Extracts TFTP remote file path and remote file name
*
* @param    incomingStr     @b{(intput)} input filepath to parse
*
* @param    remoteFilePath  @b{(output)} remote filepath
*           
* @param    remoteFileName  @b{(output)} remote filename
*
* @returns  L7_RC_t  
*
* @end
*********************************************************************/
L7_RC_t autoInstallTftpFileNameParse(L7_char8 *incomingStr, L7_char8 *remoteFilePath, L7_char8 *remoteFileName)
{
  L7_char8        *pos = L7_NULLPTR;
  L7_char8      *slash = L7_NULLPTR;
  L7_uint8 filePathLen = L7_NULL;

  if (incomingStr    == L7_NULLPTR ||
      remoteFilePath == L7_NULLPTR || 
      remoteFileName == L7_NULLPTR)
  {
    autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Tftp_Interact,
                          "%s: LINE %d: wrong params\n",
                          __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }
  
  if (strlen(incomingStr) < 1)
  {
    autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Tftp_Interact,
                          "%s: LINE %d: string to parse has zero length\n",
                          __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  pos = incomingStr;
  
  /* look whether directory is specified */
  if ((pos = strchr(pos, '/')) != L7_NULLPTR)
  {
    /* find the last directory delimiter */
    while(pos != L7_NULLPTR)    
    {
      slash = pos;
      pos = strchr(++pos, '/');
    }

    /* separately save filepath and filename */
    filePathLen = slash - incomingStr +1;
    
    if (filePathLen > L7_MAX_FILEPATH)
    {
      autoInstallDebugTrace(AUTO_INSTALL_DBG_FLAG_Tftp_Interact,
                            "%s: LINE %d: file path to parse is too long\n",
                            __FUNCTION__, __LINE__);
      return L7_FAILURE;
    }

    osapiStrncat(remoteFilePath, incomingStr, filePathLen);
    osapiStrncat(remoteFileName, incomingStr + filePathLen, L7_MAX_FILENAME);   
  }
  else
  {
    /* only filename is specified */
    osapiSnprintf(remoteFileName, L7_MAX_FILENAME+1, "%s", incomingStr);  
  }

  return L7_SUCCESS;
}

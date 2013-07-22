/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
* @filename  auto_istall_api.c
*
* @purpose   auto-install API implementation
*
* @component autoinstall
*
* @comments
*
* @create    30/01/2008
*
* @author    lkhedyk
*
* @end
*
**********************************************************************/
#include <string.h>
#include "auto_install_api.h"
#include "auto_install_debug.h"
#include "auto_install_control.h"
#include "osapi.h"
#include "log.h"
#include "sysapi.h"

/* auto-install configuration lock */
extern void  *autoInstallCfgSemaphore;

/* auto_istall message queue ID */
extern void * autoInstallMsgQueue;

/* auto-install run time variables/parameters */
extern autoInstallRunTimeData_t *autoInstallRunTimeData;

extern L7_RC_t autoInstallGblErrorCodeGet(void);

L7_char8 *pStrInfo_AutoInstallWaitingForBootOptions = "Waiting for boot options";
L7_char8 *pStrInfo_ProcessingOptions = "Processing DHCP/BOOTP options, checking preconditions";
L7_char8 *pStrInfo_DownloadingConfigFile = "Downloading tftp://%s/%s";
L7_char8 *pStrInfo_ApplyingConfigFile = "Applying downloaded configuration";
L7_char8 *pStrInfo_WaitingForRestart = "Waiting for restart timeout";
L7_char8 *pStrInfo_AutoSaveConfigFile = "Saving the downloaded configuration";
L7_char8 *pStrInfo_Stopped = "Stopped";
L7_char8 *pStrInfo_AutoInstallFinished = "AutoInstall is completed.";
L7_char8 *pStrInfo_AutoInstallSuccessfullyFinished = "AutoInstall is completed. The administrator must save running configuration in order to apply it on the next reboot.";
L7_char8 *pStrInfo_AutoInstallFailure = "AutoInstall is completed. The configuration file has errors. The administrator must not save running configuration.";

/*********************************************************************
*
* @purpose  Start/stop autoinstall procedure
*
* @param    L7_BOOL  start  @b((input)) start/stop flag to set. 
 *                                      L7_TRUE - start, L7_FALSE - stop 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallStartStopSet(L7_BOOL start)
{
  if ((start != L7_TRUE) && (start != L7_FALSE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: incorrect input: %d\n",
            __FUNCTION__, __LINE__, start);
                          
    return L7_FAILURE;
  }
  
  if(autoInstallRunTimeData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s (%d): configuration is not initialized\n",
            __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }
  
  osapiSemaTake(autoInstallCfgSemaphore, L7_WAIT_FOREVER);
  
  if (autoInstallRunTimeData->startStop != start)
  {
    autoInstallMsg_t msg;

    msg.event = AUTO_INSTALL_UI_EVENT;
    /* Update the UI specific data*/
    msg.msgData.uiEventMsg.uiEvent = AUTO_INSTALL_UI_START_STOP_EVENT;
    msg.msgData.uiEventMsg.uiMsg.startStop = start;

    if (osapiMessageSend(autoInstallMsgQueue,
                         &msg, 
                         sizeof(autoInstallMsg_t), 
                         L7_NO_WAIT, 
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "autoInstallStartStopSet(): message send failed! input = %d\n", start);
    }
  

    autoInstallRunTimeData->startStop   = start;
  }
  
  osapiSemaGive(autoInstallCfgSemaphore);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get current auto-istal state (started/stopped)
*
* @param     L7_BOOL  *start   @b((input)) start/stop flag to set. 
*                                          L7_TRUE - start, L7_FALSE - stop
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallStartStopGet(L7_BOOL *start)
{
  if (start == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: wrong params:\n",
            __FUNCTION__, __LINE__);
                          
    return L7_FAILURE;
  }
  
  if(autoInstallRunTimeData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s (%d): configuration is not initialized\n",
            __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }
  
  osapiSemaTake(autoInstallCfgSemaphore, L7_WAIT_FOREVER);
  
  *start = autoInstallRunTimeData->startStop;
  
  osapiSemaGive(autoInstallCfgSemaphore);
  
  return L7_SUCCESS;  
}
/*********************************************************************
*
* @purpose  Enable or disable auto saving the installed configuration.
*
* @param    L7_BOOL  save  @b((input)) Auto save flag to set. 
*                                      L7_TRUE - save downloaded config by default,
*                                      L7_FALSE - prompt user for explicit save 
*                                                 when config is dowmloaded.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallAutoSaveSet(L7_BOOL save)
{
  if ((save != L7_TRUE) && (save != L7_FALSE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: wrong params\n",
            __FUNCTION__, __LINE__);
                          
    return L7_FAILURE;
  }
  
  if(autoInstallRunTimeData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s (%d): configuration is not initialized\n",
            __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }
  
  osapiSemaTake(autoInstallCfgSemaphore, L7_WAIT_FOREVER);
  
  if (autoInstallRunTimeData->autoSave != save)
  {
    autoInstallMsg_t msg;

    msg.event = AUTO_INSTALL_UI_EVENT;
    /* Update the UI specific data*/
    msg.msgData.uiEventMsg.uiEvent = AUTO_INSTALL_UI_AUTO_SAVE_EVENT;
    msg.msgData.uiEventMsg.uiMsg.autoSave = save;
    
    if (osapiMessageSend(autoInstallMsgQueue,
                         &msg, 
                         sizeof(autoInstallMsg_t), 
                         L7_NO_WAIT, 
                         L7_MSG_PRIORITY_NORM) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: message send failed!\n", __FUNCTION__);
    }
    autoInstallRunTimeData->autoSave   = save;
  }
  
  osapiSemaGive(autoInstallCfgSemaphore);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Get auto saving status.
*
* @param     L7_BOOL  *start   @b((output)) Auto save flag to set. 
*                              L7_TRUE - save downloaded config by default,
*                              L7_FALSE - prompt user for explicit save 
*                                         when config is dowmloaded.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallAutoSaveGet(L7_BOOL *save)
{
  if (save == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: wrong params\n",
            __FUNCTION__, __LINE__);
                          
    return L7_FAILURE;
  }
  
  if(autoInstallRunTimeData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s (%d): configuration is not initialized\n",
            __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }
  
  osapiSemaTake(autoInstallCfgSemaphore, L7_WAIT_FOREVER);
  
  *save = autoInstallRunTimeData->autoSave;
  
  osapiSemaGive(autoInstallCfgSemaphore);
  
  return L7_SUCCESS;  
}

/*********************************************************************
*
* @purpose  Set the retrycount for the DHCP specified configuration file.
*
* @param    L7_uint32  retryCnt  @b((input)) Number of times the unicast 
*                                        TFTP tries should be made for the DHCP 
*                                        specified file before falling back for 
*                                        broadcast TFTP tries.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallFileDownLoadRetryCountSet(L7_uint32 retryCnt)
{
  if (retryCnt == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: wrong params\n",
            __FUNCTION__, __LINE__);
                          
    return L7_FAILURE;
  }
  
  if(autoInstallRunTimeData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s (%d): configuration is not initialized\n",
            __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }
  
  osapiSemaTake(autoInstallCfgSemaphore, L7_WAIT_FOREVER);
  
  if (autoInstallRunTimeData->unicastRetryCnt != retryCnt)
  {
    autoInstallRunTimeData->unicastRetryCnt   = retryCnt;
  }
  
  osapiSemaGive(autoInstallCfgSemaphore);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Set the retrycount for the DHCP specified configuration file.
*
* @param    L7_uint32  cnt  @b((output)) Number of times the unicast TFTP tries 
*                                        should be made for the DHCP specified 
*                                        file before falling back for broadcast 
*                                        TFTP tries.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallFileDownLoadRetryCountGet(L7_uint32 *retryCnt)
{
  if (retryCnt == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: wrong params\n",
            __FUNCTION__, __LINE__);
                          
    return L7_FAILURE;
  }
  
  if(autoInstallRunTimeData == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s (%d): configuration is not initialized\n",
            __FUNCTION__, __LINE__);

    return L7_FAILURE;
  }
  
  osapiSemaTake(autoInstallCfgSemaphore, L7_WAIT_FOREVER);
  
  *retryCnt = autoInstallRunTimeData->unicastRetryCnt;
  
  osapiSemaGive(autoInstallCfgSemaphore);
  
  return L7_SUCCESS;  
}
/*********************************************************************
*
* @purpose  Get the status of auto-install
*
* @param    L7_uchar8 *status @b((output)) Current autoInstall status
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    status buffer should heve size L7_CLI_MAX_STRING_LENGTH
*
* @end
*********************************************************************/
L7_RC_t autoInstallCurrentStatusGet(L7_uchar8 *status)
{
  L7_uchar8 ipString[OSAPI_INET_NTOA_BUF_SIZE];
  
  if (status == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: wrong params\n",
            __FUNCTION__, __LINE__);
                          
    return L7_FAILURE;
  }
  
  switch (autoInstallRunTimeData->currentState)
  {
    case AUTO_INSTALL_START_STATE:
      osapiStrncpy(status, pStrInfo_AutoInstallWaitingForBootOptions, L7_CLI_MAX_STRING_LENGTH);
      break;
      
    case AUTO_INSTALL_DHCP_STATE:
      osapiStrncpy(status, pStrInfo_ProcessingOptions, L7_CLI_MAX_STRING_LENGTH);
      break;
      
    case AUTO_INSTALL_GET_SPECIFIC_FILE_STATE:
      osapiInetNtoa(autoInstallRunTimeData->currentIpAddress, ipString);
      osapiSnprintf(status, L7_CLI_MAX_STRING_LENGTH,
                    pStrInfo_DownloadingConfigFile, 
                    ipString, autoInstallRunTimeData->currentFileName);
      break;
      
    case AUTO_INSTALL_GET_PREDEFINED_FILE_STATE:
      osapiInetNtoa(autoInstallRunTimeData->currentIpAddress, ipString);
      osapiSnprintf(status, L7_CLI_MAX_STRING_LENGTH,
                    pStrInfo_DownloadingConfigFile, 
                    ipString, autoInstallRunTimeData->currentFileName);
      break;

    case AUTO_INSTALL_GET_HOST_FILE_STATE:
      osapiInetNtoa(autoInstallRunTimeData->currentIpAddress, ipString);
      osapiSnprintf(status, L7_CLI_MAX_STRING_LENGTH,
                    pStrInfo_DownloadingConfigFile, 
                    ipString, autoInstallRunTimeData->currentFileName);
      break;
            
    case AUTO_INSTALL_APPLY_STATE:
      osapiStrncpy(status, pStrInfo_ApplyingConfigFile, L7_CLI_MAX_STRING_LENGTH);
      break;
      
    case AUTO_INSTALL_WAIT_FOR_RESTART_STATE:
      osapiStrncpy(status, pStrInfo_WaitingForRestart, L7_CLI_MAX_STRING_LENGTH);
      break;
      
    case AUTO_INSTALL_FINISHED_STATE:
      if ((autoInstallGblErrorCodeGet() != L7_SUCCESS) && (autoInstallGblErrorCodeGet() != L7_NO_VALUE))
      {
        osapiStrncpy(status, pStrInfo_AutoInstallFailure, L7_CLI_MAX_STRING_LENGTH);
      }
      else if ((autoInstallStartupConfigIsExist() == L7_TRUE) ||
               (autoInstallRunTimeData->autoSave == L7_TRUE))
      {
        osapiStrncpy(status, pStrInfo_AutoInstallFinished, L7_CLI_MAX_STRING_LENGTH);
      }
      else
      {
        osapiStrncpy(status, pStrInfo_AutoInstallSuccessfullyFinished, L7_CLI_MAX_STRING_LENGTH);
      }   
      break;

    case AUTO_INSTALL_AUTO_SAVE_STATE:
      osapiStrncpy(status, pStrInfo_AutoSaveConfigFile, L7_CLI_MAX_STRING_LENGTH);
      break;

    case AUTO_INSTALL_STOP_STATE:
      osapiStrncpy(status, pStrInfo_Stopped, L7_CLI_MAX_STRING_LENGTH);
      break;
      
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: invalid AutoInstall status %d\n",
              __FUNCTION__, __LINE__,
                            autoInstallRunTimeData->currentState);
      return L7_FAILURE;
      break;
      
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Register with AutoInstall for notification of config file 
*           attempt failure.
* @param     L7_COMPONENT_IDS_t       compId   @b((input)) Component being 
*                                                          registered. 
* @param     autoInstallNotifyEvent_t event    @b((input)) event signifying
*                                                           when to notify. 
* @param     L7_RC_t (*notifyFn)()    notifyFn @b((input)) Callback function 
*                                                           registered. 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallNotifyRegister(L7_COMPONENT_IDS_t compId, 
                                  L7_uint32  notifyEventMask,
                                  L7_RC_t (*notifyFn)())
{
  L7_uint32 index;

  if (compId <= L7_FIRST_COMPONENT_ID || compId >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: invalid Component Id %d for registration.\n",
            __FUNCTION__, __LINE__, compId);
    return L7_FAILURE;
  }
  if (notifyFn == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: invalid Callback function by component Id %d for registration.\n",
            __FUNCTION__, __LINE__, compId);
    return L7_FAILURE;
  }
  for (index = 0; index < AUTO_INSTALL_NUM_EVENT_REGISTRATIONS_MAX; index ++)
  {
    if (autoInstallRunTimeData->notifyList[index].compId == L7_NULL)
    {
      autoInstallRunTimeData->notifyList[index].compId = compId;
      autoInstallRunTimeData->notifyList[index].notifyEvent = notifyEventMask;
      autoInstallRunTimeData->notifyList[index].notifyChange = notifyFn;
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Register with AutoInstall for notification of config file 
*           attempt failure.
*
* @param     L7_COMPONENT_IDS_t       compId   @b((input)) Component being 
*                                                          registered. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t autoInstallNotifyDeregister(L7_COMPONENT_IDS_t compId)
{
  L7_uint32 index;

  if (compId <= L7_FIRST_COMPONENT_ID || compId >= L7_LAST_COMPONENT_ID)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_AUTO_INSTALL_COMPONENT_ID, "%s: LINE %d: invalid Component Id %d for deregistration.\n",
            __FUNCTION__, __LINE__, compId);
    return L7_FAILURE;
  }
  for (index = 0; index < AUTO_INSTALL_NUM_EVENT_REGISTRATIONS_MAX; index ++)
  {
    if (autoInstallRunTimeData->notifyList[index].compId == compId)
    {
      memset(&autoInstallRunTimeData->notifyList[index], 0, sizeof(eventNotifyList_t));
      return L7_SUCCESS;
    }
  }
  return L7_SUCCESS;
}


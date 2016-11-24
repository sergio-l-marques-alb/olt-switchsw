/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_cfg.c
*
* @purpose   ISDP configuration file
*
* @component isdp
*
* @comments
*
* @create    9/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/

#include "isdp_include.h"
#include "support_api.h"

extern isdpCfg_t        *isdpCfg;
extern isdpCnfgrState_t isdpCnfgrState;

void                    *isdpTaskSyncSema   = L7_NULLPTR;
void                    *isdpProcessQueue   = L7_NULLPTR;
void                    *isdpPduQueue       = L7_NULLPTR;
void                    *isdpSemaphore      = L7_NULLPTR;
L7_uint32               isdpTaskId          = 0;
L7_uint32               isdpIpAddrPoolId    = 0;
isdpDebugCfg_t          isdpDebugCfg;

/*********************************************************************
* @purpose  Saves isdp configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments isdpCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t isdpSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (isdpHasDataChanged() == L7_TRUE)
  {
    isdpCfg->hdr.dataChanged = L7_FALSE;
    isdpCfg->checkSum = nvStoreCrc32((L7_uchar8 *)isdpCfg,
                                     (L7_uint32)(sizeof(isdpCfg_t) - sizeof(isdpCfg->checkSum)));

    if (sysapiCfgFileWrite(L7_ISDP_COMPONENT_ID, ISDP_CFG_FILENAME,
                           (L7_char8 *)isdpCfg, sizeof(isdpCfg_t)) == L7_ERROR)
    {
      rc = L7_ERROR;
      LOG_MSG("Error on call to sysapiCfgFileWrite routine on config file %s\n",
              ISDP_CFG_FILENAME);
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Checks if isdp user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL isdpHasDataChanged(void)
{
  return isdpCfg->hdr.dataChanged;
}

void isdpResetDataChanged (void)
{
  isdpCfg->hdr.dataChanged =  L7_FALSE;
  return;
}

/*********************************************************************
*
* @purpose  System Initialization for ISDP
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
L7_RC_t isdpInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t            rc     = L7_SUCCESS;
  isdpMsg_t          msg;
  L7_CNFGR_CB_DATA_t cbData;

  memset(&msg,    0, sizeof(isdpMsg_t));
  memset(&cbData, 0, sizeof(L7_CNFGR_CB_DATA_t));

  isdpSemaphore = osapiSemaBCreate(OSAPI_SEM_Q_PRIORITY, OSAPI_SEM_FULL);
  if (isdpSemaphore == L7_NULLPTR)
  {
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /*semaphore creation for task protection over the common data*/
  isdpTaskSyncSema = osapiSemaCCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (isdpTaskSyncSema == L7_NULL)
  {
    LOG_MSG("Unable to create isdp task semaphore()\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  isdpProcessQueue = (void*)osapiMsgQueueCreate("isdpProcessQueue", ISDP_MSG_COUNT,
                                                (L7_uint32)sizeof(isdpMsg_t));

  if (isdpProcessQueue == L7_NULLPTR)
  {
    LOG_MSG("isdpInit: isdpProcessQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  isdpPduQueue = (void*)osapiMsgQueueCreate("isdpPduQueue", ISDP_MSG_COUNT,
                                            (L7_uint32)sizeof(isdpMsg_t));

  if (isdpPduQueue == L7_NULLPTR)
  {
    LOG_MSG("isdpInit: isdpPduQueue creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  isdpTaskId = (L7_uint32)osapiTaskCreate("isdpTask", (void *)isdpTask, 0, 0,
                                             L7_ISDP_STACK_SIZE,
                                             L7_ISDP_DEFAULT_ISDP_TASK_PRI,
                                             L7_DEFAULT_TASK_SLICE);
  if (isdpTaskId == L7_ERROR)
  {
    LOG_MSG("isdpInit: task creation error.\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit (L7_ISDP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    LOG_MSG("Unable to initialize isdp_task()\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /*memcpy(&msg.data.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));*/
  msg.data.CmdData = *pCmdData;
  msg.event = ISDP_CNFGR_EVENT;

  rc = isdpProcessMsgQueue(msg);

  return rc;
}

/*********************************************************************
*
* @purpose  System Init Undo for ISDP
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void isdpInitUndo()
{
  if (isdpProcessQueue != L7_NULLPTR)
    osapiMsgQueueDelete(isdpProcessQueue);

  if (isdpPduQueue != L7_NULLPTR)
      osapiMsgQueueDelete(isdpPduQueue);

  if (isdpTaskId != L7_ERROR)
    osapiTaskDelete(isdpTaskId);

  if (isdpTaskSyncSema != L7_NULL)
  {
    (void)osapiSemaDelete(isdpTaskSyncSema);
  }

  if (isdpSemaphore != L7_NULL)
  {
    (void)osapiSemaDelete(isdpSemaphore);
  }

  isdpCnfgrState = ISDP_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  Build default isdp config data
*
* @param    ver @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void isdpBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32     index;
  nimConfigID_t cfgId[L7_ISDP_INTF_MAX_COUNT];

  /* save the config id's */
  memset(&cfgId[0], 0, sizeof(nimConfigID_t) * L7_ISDP_INTF_MAX_COUNT);

   for (index = 1; index < L7_ISDP_INTF_MAX_COUNT; index++)
   {
     NIM_CONFIG_ID_COPY(&cfgId[index],
                        &isdpCfg->cfg.intfCfgData[index].cfgId);
   }

   memset((void *)&isdpCfg->cfg, 0, sizeof(isdpCfgData_t));

   for (index = 1; index < L7_ISDP_INTF_MAX_COUNT; index++)
   {
     isdpBuildDefaultIntfConfigData(&cfgId[index],
                                    &isdpCfg->cfg.intfCfgData[index]);
   }

  /* setup file header */
  isdpCfg->hdr.version                       = ver;
  isdpCfg->hdr.componentID                   = L7_ISDP_COMPONENT_ID;
  isdpCfg->hdr.type                          = L7_CFG_DATA;
  isdpCfg->hdr.length                        = (L7_uint32)sizeof(isdpCfg_t);
  isdpCfg->hdr.dataChanged                   = L7_FALSE;

  isdpCfg->cfg.globalCfgData.mode            = FD_ISDP_DEFAULT_MODE;
  isdpCfg->cfg.globalCfgData.timer           = FD_ISDP_DEFAULT_TIMER;
  isdpCfg->cfg.globalCfgData.holdTime        = FD_ISDP_DEFAULT_HOLDTIME;
  isdpCfg->cfg.globalCfgData.v2Mode          = FD_ISDP_DEFAULT_V2MODE;

  osapiStrncpy((L7_char8 *)isdpCfg->hdr.filename, ISDP_CFG_FILENAME,
               sizeof(isdpCfg->hdr.filename));

}

/*********************************************************************
* @purpose  Build default isdp Interface config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void isdpBuildDefaultIntfConfigData(nimConfigID_t *cfgId,
                                    isdpIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->cfgId, cfgId);
  pCfg->mode = FD_ISDP_DEFAULT_MODE;
  return;
}

/*********************************************************************
* @purpose  Saves isdp debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    isdpDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t isdpDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (isdpDebugHasDataChanged() == L7_TRUE)
  {
    /* Copy the operational states into the config file */
    isdpDebugCfgUpdate();

    /* Store the config file */
    isdpDebugCfg.hdr.dataChanged = L7_FALSE;
    isdpDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&isdpDebugCfg,
        (L7_uint32)(sizeof(isdpDebugCfg) - sizeof(isdpDebugCfg.checkSum)));

    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_ISDP_COMPONENT_ID, ISDP_DEBUG_CFG_FILENAME,
        (L7_char8 *)&isdpDebugCfg, (L7_uint32)sizeof(isdpDebugCfg_t))) == L7_ERROR)
    {
      LOG_MSG("Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",
              ISDP_DEBUG_CFG_FILENAME);
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Restores isdp debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    isdpDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t isdpDebugRestore(void)
{
  L7_RC_t rc = L7_SUCCESS;
  isdpDebugBuildDefaultConfigData(ISDP_DEBUG_CFG_VER_CURRENT);

  isdpDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = isdpDebugApplyConfigData();

  return rc;
}


/*********************************************************************
* @purpose  Checks if isdp debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL isdpDebugHasDataChanged(void)
{
  return isdpDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default isdp config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void isdpDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  isdpDebugCfg.hdr.version = ver;
  isdpDebugCfg.hdr.componentID = L7_ISDP_COMPONENT_ID;
  isdpDebugCfg.hdr.type = L7_CFG_DATA;
  isdpDebugCfg.hdr.length = sizeof(isdpDebugCfg_t);
  osapiStrncpy((L7_char8 *)isdpDebugCfg.hdr.filename, ISDP_DEBUG_CFG_FILENAME,
               sizeof(isdpDebugCfg.hdr.filename));
  isdpDebugCfg.hdr.dataChanged = L7_FALSE;

  isdpDebugCfg.cfg.isdpDebugPacketTraceTxFlag = FD_ISDP_DEFAULT_TX_TRACE_MODE;
  isdpDebugCfg.cfg.isdpDebugPacketTraceRxFlag = FD_ISDP_DEFAULT_RX_TRACE_MODE;
  isdpDebugCfg.cfg.isdpDebugEventTraceFlag    = FD_ISDP_DEFAULT_EVENTS_TRACE_MODE;
}

/*********************************************************************
* @purpose  Apply isdp debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t isdpDebugApplyConfigData(void)
{
  L7_RC_t rc = L7_SUCCESS;

  isdpDebugPacketTraceTxFlagSet(isdpDebugCfg.cfg.isdpDebugPacketTraceTxFlag);
  isdpDebugPacketTraceRxFlagSet(isdpDebugCfg.cfg.isdpDebugPacketTraceRxFlag);
  isdpDebugEventTraceFlagSet(isdpDebugCfg.cfg.isdpDebugEventTraceFlag);

  return rc;
}


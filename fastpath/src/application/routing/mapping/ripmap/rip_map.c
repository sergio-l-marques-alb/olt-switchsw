/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  rip_map.c
* @purpose   RIP Mapping system infrastructure
* @component RIP Mapping Layer
* @comments  none
* @create    05/21/2001
* @author    gpaussa
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "l7_ripinclude.h"
#include "rip_map_table.h"
#include "xx.ext"
#include "l7_rip_debug_api.h"
#include "rip_util.h"
#include "support_api.h"

/* Max messages in the RIP redistribution queue */
static L7_int32 redistQHighWaterMark = 0;

extern L7_uchar8 *ipMapRouterEventNames[L7_LAST_RTR_STATE_CHANGE];

/* external declarations */
extern ripMapCnfgrState_t ripMapCnfgrState;

ripMapDebugCfg_t ripMapDebugCfg;    /* Debug Configuration File Overlay */
extern void ripMapDebugCfgUpdate(void);

extern void rip_Task(void);
extern void ripMapCnfgrParse(L7_CNFGR_CMD_DATA_t *pCmdData);

/* RIP mapping component globals */
ripMapCfg_t        *pRipMapCfgData;
L7_uint32          *pRipMapMapTbl;
ripInfo_t          *pRipInfo;
ripIntfInfo_t      *pRipIntfInfo;

/* RIP mapping semaphore, used to protect all of the above globals */
void               *ripMapSemaphore = L7_NULLPTR;

rtoRouteChange_t *ripRouteChangeBuf;

/* Check if the default defines have proper values. */
#if (!(((FD_RIP_DEFAULT_DEFAULT_METRIC >= L7_RIP_CONF_METRIC_MIN) && \
        (FD_RIP_DEFAULT_DEFAULT_METRIC <= L7_RIP_CONF_METRIC_MAX)    \
       ) ||                                                          \
       (FD_RIP_DEFAULT_DEFAULT_METRIC == L7_RIP_METRIC_INVALID)      \
      )                                                              \
    )
#error FD_RIP_DEFAULT_DEFAULT_METRIC is set to a wrong value
#endif

#if (!(((FD_RIP_DEFAULT_REDIST_ROUTE_METRIC >= L7_RIP_CONF_METRIC_MIN) &&  \
        (FD_RIP_DEFAULT_REDIST_ROUTE_METRIC <= L7_RIP_CONF_METRIC_MAX)     \
       ) ||                                                                \
       (FD_RIP_DEFAULT_REDIST_ROUTE_METRIC == L7_RIP_METRIC_INVALID)       \
      )                                                                    \
    )
#error FD_RIP_DEFAULT_REDIST_ROUTE_METRIC is set to a wrong value
#endif

#if ((FD_RIP_DEFAULT_REDIST_ROUTE_FILTER_MODE == L7_DISABLE) &&                  \
     (FD_RIP_DEFAULT_REDIST_ROUTE_FILTER != L7_RIP_ROUTE_REDIST_FILTER_INVALID)  \
    )
#error FD_RIP_DEFAULT_REDIST_ROUTE_FILTER is set to a wrong value
#endif

static void ripMapBestRouteEventPost(void);


/*********************************************************************
* @purpose  Initialize the RIP layer application
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    Requires that the system router ID be configured.
* @notes    This is called on cnfgr init, do not lock ripMapSemaphore
*
* @end
*********************************************************************/
L7_RC_t ripMapAppsInit(void)
{
  ripMapApplyConfigData();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize RIP tasks
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapStartTasks(void)
{
  /* clear task init flag prior to creating the rip_Task; it is used as
   * part of the control mechanism to wait for rip_Task to initialize
   */
  ripMapCtrl_g.taskCtrl.taskInit = L7_FALSE;


  /*-----------------------------------*/
  /* initialize the vendor code task   */
  /*-----------------------------------*/

  /* create RIP task */
  pRipInfo->ripTaskId = osapiTaskCreate("tRipTask", rip_Task, 0, 0,
                                        (L7_L3_DEFAULT_STACK_SIZE*2),
                                        L7_DEFAULT_TASK_PRIORITY,
                                        L7_DEFAULT_TASK_SLICE);


  if (pRipInfo->ripTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripStartTasks: Failed to create RIP task\n");
    return L7_FAILURE;
  }

  /* must wait here until the rip_Task has reached a certain state
   * of initialization, or else things will fail when trying to
   * apply the configuration data for the RIP interfaces (due to
   * uninitialized pointers, etc.)
   */
  if (osapiWaitForTaskInit(L7_RIP_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripStartTasks: Unable to synchronize with RIP task\n");
    return L7_FAILURE;
  }
  /*-----------------------------------*/
  /* initialize the mapping layer task */
  /*-----------------------------------*/

  /* Create a counting semaphore to indicate when a message is available to
   * be read in any of the message queues. */
  pRipInfo->msgQSema = osapiSemaCCreate(OSAPI_SEM_Q_FIFO, 0);
  if (pRipInfo->msgQSema == L7_NULL)
  {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RIP_MAP_COMPONENT_ID,
             "Failure creating RIP message queue semaphore.");
      return L7_FAILURE;
  }

  pRipInfo->ripMapProcessQueue = (void *)osapiMsgQueueCreate( RIPMAP_PROCESS_QUEUE,
                                                              RIPMAP_PROC_MSG_COUNT,
                                                              sizeof(ripMapMsg_t) );

  pRipInfo->ripMapRedistQueue = (void *)osapiMsgQueueCreate( RIPMAP_REDIST_QUEUE,
                                                             RIPMAP_REDIST_MSG_COUNT,
                                                             sizeof(ripMapMsg_t) );

  if ((pRipInfo->ripMapProcessQueue == L7_NULLPTR) ||
      (pRipInfo->ripMapRedistQueue == L7_NULLPTR))
  {
      L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_RIP_MAP_COMPONENT_ID,
             "Failure creating RIP message queues.");
  }

  /* create RIP task */
  pRipInfo->ripMapProcessingTaskId = osapiTaskCreate("ripMapProcessingTask",
                                                     ripMapProcessingTask, 0, 0,
                                                     L7_DEFAULT_STACK_SIZE,
                                                     L7_DEFAULT_TASK_PRIORITY,
                                                     L7_DEFAULT_TASK_SLICE);

  if (pRipInfo->ripMapProcessingTaskId == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripStartTasks: Failed to create RIP processing task\n");
    return L7_FAILURE;
  }

  if (osapiWaitForTaskInit(  L7_RIP_PROCESSING_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripStartTasks: Unable to synchronize with RIP processing task\n");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deinitialize RIP tasks
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapFiniTasks(void)
{

  /* clear task init flag prior to deleting the rip_Task; it is used as
  * part of the control mechanism to wait for rip_Task to initialize
  */
  ripMapCtrl_g.taskCtrl.taskInit = L7_FALSE;

  if (pRipInfo->ripTaskId != L7_ERROR)
  {
    osapiTaskDelete(pRipInfo->ripTaskId);
    pRipInfo->ripTaskId = L7_NULL;
  }

  if (pRipInfo->ripMapProcessingTaskId != L7_ERROR)
  {
    osapiTaskDelete( pRipInfo->ripMapProcessingTaskId);
    pRipInfo->ripMapProcessingTaskId = L7_NULL;
  }

  if (pRipInfo->ripMapProcessQueue != L7_NULLPTR)
  {
    (void) osapiMsgQueueDelete(pRipInfo->ripMapProcessQueue);
    pRipInfo->ripMapProcessQueue = L7_NULLPTR;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  RIP Processing Task
*
* @param    void
*
* @returns  void
*
* @notes    rip processing functions called here are entry points
*           for configuration, they must lock ripMapSemaphore where
*           configuration data is read/modified.
*
* @end
*********************************************************************/
void ripMapProcessingTask(void)
{
  ripMapMsg_t  Message;
  L7_uint32    status;

  osapiTaskInitDone(L7_RIP_PROCESSING_TASK_SYNC);

  /* Main body of task */
  for (;;)
  {
    /* Since we are reading from multiple queues, we cannot wait forever
     * on the message receive from each queue. Rather than sleep between
     * queue reads, use a semaphore to indicate whether any queue has
     * data. Wait until data is available. */
        if (osapiSemaTake(pRipInfo->msgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
                   "Failure taking RIP message queue semaphore.");
            continue;
        }
    memset(&Message,0x00,sizeof(ripMapMsg_t));

    /* Always try to read from the main RIP queue first. */
    status = osapiMessageReceive(pRipInfo->ripMapProcessQueue, &Message,
                                 sizeof(ripMapMsg_t), L7_NO_WAIT);

    if (status != L7_SUCCESS)
    {
        /* The main queue is empty. See if a redistribution event is queued. */
        status = osapiMessageReceive(pRipInfo->ripMapRedistQueue, &Message,
                                   sizeof(ripMapMsg_t), L7_NO_WAIT);
    }

    if (status == L7_SUCCESS)
      {
        switch (Message.msgId)
        {
        case RIPMAP_RTR_EVENT:
          (void)ripMapRoutingEventChangeProcess(Message.type.rtrEvent.intIfNum,
                                                Message.type.rtrEvent.event,
                                                Message.type.rtrEvent.pData,
                                                &(Message.type.rtrEvent.response));
          break;

        case RIPMAP_ADMIN_MODE_EVENT:
          ripMapAdminModeProcess(Message.type.modeEvent.mode);
          break;

        case RIPMAP_INTF_MODE_EVENT:
          ripMapIntfAdminModeProcess(Message.type.modeEvent.intIfNum,
                                     Message.type.modeEvent.mode);
          break;

        case RIPMAP_RESTORE_EVENT:
          ripMapRestoreProcess();
          break;

        case RIPMAP_CNFGR:
          ripMapCnfgrParse(&Message.type.cnfgrData);
          break;

        case RIPMAP_RTO_EVENT:
        {
            L7_int32 redistQLen;
            if (osapiMsgQueueGetNumMsgs(pRipInfo->ripMapRedistQueue,
                                        &redistQLen) == L7_SUCCESS)
            {
                if (redistQLen > redistQHighWaterMark)
                    redistQHighWaterMark = redistQLen;
            }
            ripMapBestRoutesGet();
        }

        break;

        default:
            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
                   "Unknown message type received in the RIP processing task.");
            break;
        } /* endswitch */
    }
    else  /* status != L7_SUCCESS */
    {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
                "Bad status from RIP message queue read: %d.", status);
    }
  } /* for (;,;,;) */
}

void ripRedistQLenShow(void)
{
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_RIPMAP,
                  "\n\tRIP redistribution queue high water mark:  %d (limit: %d)\n",
                  redistQHighWaterMark, RIPMAP_REDIST_MSG_COUNT);
}

/*********************************************************************
* @purpose  Save RIP user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    config entry point, must lock ripMapSemaphore to read
*
* @end
*********************************************************************/
L7_RC_t ripMapSave(void)
{
  ripMapSemaTake();
  if (pRipMapCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    pRipMapCfgData->cfgHdr.dataChanged = L7_FALSE;
    pRipMapCfgData->checkSum = nvStoreCrc32((L7_char8 *)pRipMapCfgData,
                                            sizeof(ripMapCfg_t) - sizeof(pRipMapCfgData->checkSum));

    /* call NVStore save routine */
    if (sysapiCfgFileWrite(L7_RIP_MAP_COMPONENT_ID,
                           L7_RIP_MAP_CFG_FILENAME, (L7_char8 *)pRipMapCfgData,
                           sizeof(ripMapCfg_t)) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "ripSave: Error during osapiFsWrite for config file %s\n",
              L7_RIP_MAP_CFG_FILENAME);
      ripMapSemaGive();
      return L7_FAILURE;
    }
  }
  ripMapSemaGive();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if RIP user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    config entry point, must lock ripMapSemaphore to read
*
* @end
*********************************************************************/
L7_BOOL ripMapHasDataChanged(void)
{
  L7_BOOL rc = L7_FALSE;

  ripMapSemaTake();
  rc = pRipMapCfgData->cfgHdr.dataChanged;
  ripMapSemaGive();

  return rc;
}
void ripMapResetDataChanged(void)
{
  ripMapSemaTake();
  pRipMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  ripMapSemaGive();

  return;

}
/*********************************************************************
* @purpose  Build default RIP config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    ripMapSemaphore must be locked when calling this function
*           (except for component init)
*
* @end
*********************************************************************/
void ripMapBuildDefaultConfigData(L7_uint32 ver)
{
  L7_REDIST_RT_INDICES_t index;
  L7_uint32              cfgIndex;
  nimConfigID_t          configId[L7_RIP_MAP_INTF_MAX_COUNT];

  /*----------------------------------------------------------*/
  /* Ensure that interface config ID information is preserved */
  /*----------------------------------------------------------*/

  memset(configId, 0, sizeof(nimConfigID_t) * L7_RIP_MAP_INTF_MAX_COUNT);

  for (cfgIndex = 1; cfgIndex < L7_RIP_MAP_INTF_MAX_COUNT; cfgIndex++)
  {
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &pRipMapCfgData->intf[cfgIndex].configId);
  }

  /*--------------------------*/
  /* build config file header */
  /*--------------------------*/

  memset(pRipMapCfgData, 0, sizeof(ripMapCfg_t));
  strcpy(pRipMapCfgData->cfgHdr.filename, L7_RIP_MAP_CFG_FILENAME);
  pRipMapCfgData->cfgHdr.version       = ver;
  pRipMapCfgData->cfgHdr.componentID   = L7_RIP_MAP_COMPONENT_ID;
  pRipMapCfgData->cfgHdr.type          = L7_CFG_DATA;
  pRipMapCfgData->cfgHdr.length        = sizeof(ripMapCfg_t);
  pRipMapCfgData->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* build default config data */
  /*---------------------------*/

  /* generic RIP cfg */
  pRipMapCfgData->ripAdminMode = FD_RIP_MAP_DEFAULT_ADMIN_MODE;
  pRipMapCfgData->ripSplitHorizon = FD_RIP_MAP_DEFAULT_SPLITHORIZON_MODE;
  pRipMapCfgData->ripAutoSummarization = FD_RIP_MAP_DEFAULT_AUTOSUMMARY_MODE;
  pRipMapCfgData->ripHostRoutesAccept = FD_RIP_MAP_DEFAULT_HOSTROUTESACCEPT_MODE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
  {
    ripMapIntfBuildDefaultConfigData(&configId[cfgIndex], &pRipMapCfgData->intf[cfgIndex]);
  }

  pRipMapCfgData->ripDefaultMetric = FD_RIP_DEFAULT_DEFAULT_METRIC;
  pRipMapCfgData->ripDefRtAdv.defRtAdv = FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE;
  for (index = REDIST_RT_FIRST+1; index < REDIST_RT_LAST; index ++)
  {
    pRipMapCfgData->ripRtRedist[index].redistribute = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
    pRipMapCfgData->ripRtRedist[index].metric = FD_RIP_DEFAULT_REDIST_ROUTE_METRIC;
    pRipMapCfgData->ripRtRedist[index].matchType = FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE;
    pRipMapCfgData->ripRtRedist[index].ripRtRedistFilter.mode = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
    pRipMapCfgData->ripRtRedist[index].ripRtRedistFilter.filter = FD_RIP_DEFAULT_REDIST_ROUTE_FILTER;
  }

  return;
}


/*********************************************************************
* @purpose  Build default RIP config data for an intf
*
* @param    configId     configID associated with an interface
* @param    *pCfg        pointer to RIP interface configuration structure
*
* @returns  void
*
* @notes    ripMapSemaphore must be locked when calling this function
*           (except for component init)
*
* @end
*********************************************************************/
void ripMapIntfBuildDefaultConfigData(nimConfigID_t *configId, ripMapCfgIntf_t *pCfg)
{
  L7_uint32 k;
  /* need to fix this, configId is _not_ maintained if called with &pCfg->configId */

  memset(pCfg, 0, sizeof(ripMapCfgIntf_t));

  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->adminMode     = FD_RIP_INTF_DEFAULT_ADMIN_MODE;
  pCfg->authType      = FD_RIP_INTF_DEFAULT_AUTH_TYPE;

  for (k = 0; k < L7_AUTH_MAX_KEY_RIP; k++)
    pCfg->authKey[k]  = FD_RIP_INTF_DEFAULT_AUTH_KEY_CHAR;

  pCfg->authKeyLen    = FD_RIP_INTF_DEFAULT_AUTH_KEY_LEN;
  pCfg->authKeyId     = FD_RIP_INTF_DEFAULT_AUTH_KEY_ID;
  pCfg->verSend       = FD_RIP_INTF_DEFAULT_VER_SEND;
  pCfg->verRecv       = FD_RIP_INTF_DEFAULT_VER_RECV;
  pCfg->intfCost      = FD_RIP_INTF_DEFAULT_INTF_COST;
}


/*********************************************************************
* @purpose  Apply RIP config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied, or not a RIP interface
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    ripMapSemaphore must be locked when calling this function
*           (except for component init)
*
* @end
*********************************************************************/
L7_RC_t ripMapApplyConfigData(void)
{
  L7_uint32     ripMode;

  L7_uint32 cfgIndex;   /* interface configuration indices */
  L7_uint32 intIfNum;
  L7_REDIST_RT_INDICES_t   index;


  if (pRipInfo->ripInitialized != L7_TRUE)
    return L7_SUCCESS;

  ripMode = pRipMapCfgData->ripAdminMode;

  /*  Def rt adv mode and redist mode for all protocols is disabled
      in pRipInfo structure.
  */
  if (ripMode == L7_ENABLE)
    ripMapAdminModeEnable();

  else if (ripMode == L7_DISABLE)
    ripMapAdminModeDisable();

  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripApplyConfigData: Invalid RIP admin mode config value (%u)\n",
            ripMode);
    L7_assert(1);
    return L7_FAILURE;
  }


  if (ripMapSplitHorizonModeApply(pRipMapCfgData->ripSplitHorizon)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripApplyConfigData: Could not apply split horizon mode.\n");
  }
  if (ripMapAutoSummarizationModeApply(pRipMapCfgData->ripAutoSummarization)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripApplyConfigData: Could not apply auto-summarization mode.\n");
  }
  if (ripMapHostRoutesAcceptModeApply(pRipMapCfgData->ripHostRoutesAccept)
      != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripApplyConfigData: Could not apply host routes accept mode.\n");
  }

  /*-----------------------------------*/
  /* configure per-interface RIP parms */
  /*-----------------------------------*/
  if (RIPMAP_IS_READY == L7_TRUE)
  {
    for (cfgIndex = 1; cfgIndex < L7_RIP_MAP_INTF_MAX_COUNT; cfgIndex++)
    {
        if ((nimIntIfFromConfigIDGet(&(pRipMapCfgData->intf[cfgIndex].configId),
                                     &intIfNum) == L7_SUCCESS) &&
            (ripMapIntfMayParticipate(intIfNum) == L7_TRUE))
        {
            if (ripMapRouterIntfConfigure(intIfNum) == L7_SUCCESS)
            {
                ripMapIntfAdminModeEnable(intIfNum);
            }
            else
            {
                L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
                nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

                L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
                        "ripApplyConfigData: Failure to configure RIP interface " \
                          "%s\n", ifName);
            }
        }
    }  /* cfgIndex */
  }  /* RIPMAP_IS_READY */


  /* No need to apply the default metric. Since it is used by default route
     advertise mode apply and route redistribution parms apply functions
     anyway, we have nothing to do about it.
  */

  /* No need to apply the route filter. Since it is used by route redistribution parms
     apply function, we have nothing to do about it.
  */

  /* Apply the default route adv mode. */
  if (ripMapDefaultRouteAdvertiseModeApply(pRipMapCfgData->ripDefRtAdv.defRtAdv) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripApplyConfigData: Could not apply Default Route Advertise mode.\n");
  }

  /* Apply the route redist parms for all protocols */
  for (index = REDIST_RT_FIRST+1; index < REDIST_RT_LAST; index ++)
  {
    if (index == REDIST_RT_RIP)
      continue;

    if (ripMapRouteRedistributeModeApply(index,
                                         pRipMapCfgData->ripRtRedist[index].redistribute)
        != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "ripApplyConfigData: Could not apply Route Redist for routes from -- protocol.\n");
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Configure RIP router interfaces
*
* @param    intIfNum    Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapRouterIntfConfigure(L7_uint32 intIfNum)
{
  ripMapCfgIntf_t   *pCfg;

  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* NOTE:  The auth key was stored in its fully-padded format, so
     *        there is no need to apply the auth key length here
     */
    ripMapIntfAuthTypeApply(intIfNum, pCfg->authType);
    ripMapIntfAuthKeyApply(intIfNum, pCfg->authKey);
    ripMapIntfAuthKeyIdApply(intIfNum, pCfg->authKeyId);
    ripMapIntfVerSendApply(intIfNum, pCfg->verSend);
    ripMapIntfVerRecvApply(intIfNum, pCfg->verRecv);
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*
*********************************************************************/
L7_RC_t ripMapIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t    configId;
  ripMapCfgIntf_t *pCfg = NULL;

  if (ripMapIntfIsValid(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  /* There should never be an entry in the RIP interface array at this point. If
   * one exists, holler. */
  if (pRipMapMapTbl[intIfNum] != 0)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
            "Interface structure already exists for interface %s when L7_CREATE event received.",
            ifName);
    return L7_SUCCESS;
  }

  /* Find an unused element in the interface array */
  if (ripMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
            "Failed to get configuration entry for RIP interface %s.",
            ifName);
    return L7_FAILURE;
  }

  /* Update the configuration structure with the config ID */
  if (pCfg)
  {
    ripMapIntfBuildDefaultConfigData(&configId, pCfg);
  }

  ripIntfStateSet(intIfNum, RIP_MAP_INTF_STATE_DOWN);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*
*********************************************************************/
L7_RC_t ripMapIntfDelete(L7_uint32 intIfNum)
{
  ripMapCfgIntf_t *pCfg;

  if (ripMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((void *)&pRipMapMapTbl[intIfNum], 0, sizeof(L7_uint32));
    ripIntfStateSet(intIfNum, RIP_MAP_INTF_STATE_NONE);
    pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Put RIP admin mode events on the queue to be handled by RIP
*
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapAdminModeQueue( L7_uint32 mode)
{
  L7_RC_t rc;
  ripMapMsg_t msg;

  if (pRipInfo->ripMapProcessQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset((void *)&msg, 0, sizeof(ripMapMsg_t));
  msg.msgId                     = RIPMAP_ADMIN_MODE_EVENT;
  msg.type.modeEvent.mode       = mode;

  rc = osapiMessageSend(pRipInfo->ripMapProcessQueue, &msg, RIP_MAP_MSG_SIZE,
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

  if (rc == L7_SUCCESS)
  {
      osapiSemaGive(pRipInfo->msgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripMapAdminModeQueue: osapiMessageSend failed\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Process RIP admin mode events
*
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    config entry point, must lock ripMapSemaphore to read/modify
*
* @end
*********************************************************************/
void ripMapAdminModeProcess(L7_uint32 mode)
{
  ripMapSemaTake();
  /* pRipMapCfgData->ripAdminMode is set in the API
     thread, ensure it is in sync and apply
     the mode in this processing thread, otherwise
     ignore this request, mode has been modified */
  if (pRipMapCfgData->ripAdminMode != mode)
  {
    ripMapSemaGive();
    return;
  }
  if (mode == L7_ENABLE)
  {
    (void)ripMapAdminModeEnable();
  }
  else if (mode == L7_DISABLE)
  {
    (void)ripMapAdminModeDisable();
  }

  ripMapSemaGive();
}

/*********************************************************************
* @purpose  Put RIP admin mode events on the queue to be handled by RIP
*
* @param    intIfNum    internal interface number
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapIntfAdminModeQueue( L7_uint32 intIfNum, L7_uint32 mode)
{
  L7_RC_t rc;
  ripMapMsg_t msg;

  if (pRipInfo->ripMapProcessQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset( (void *)&msg, 0, sizeof(ripMapMsg_t) );
  msg.msgId                     = RIPMAP_INTF_MODE_EVENT;
  msg.type.modeEvent.intIfNum   = intIfNum;
  msg.type.modeEvent.mode       = mode;

  rc = osapiMessageSend( pRipInfo->ripMapProcessQueue, &msg, RIP_MAP_MSG_SIZE,
                         L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

  if (rc == L7_SUCCESS)
  {
      osapiSemaGive(pRipInfo->msgQSema);
  }
  else
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "ripMapIntfAdminModeQueue: osapiMessageSend failed\n");
  }

  return rc;
}

/*********************************************************************
* @purpose  Process RIP intf admin mode events
*
* @param    intIfNum    internal interface number
* @param    mode        L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    config entry point, must lock ripMapSemaphore to read/modify
*
* @end
*********************************************************************/
void ripMapIntfAdminModeProcess(L7_uint32 intIfNum, L7_uint32 mode)
{
  ripMapSemaTake();
  if (mode == L7_ENABLE)
  {
    if (ripMapIntfMayParticipate(intIfNum))
    {
      (void)ripMapIntfAdminModeEnable(intIfNum);
    }
  }
  else if (mode == L7_DISABLE)
  {
    (void)ripMapIntfAdminModeDisable(intIfNum);
  }
  ripMapSemaGive();
}


/*********************************************************************
* @purpose  Enable RIP on an interface.
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
* @notes    Assumes caller has already verified that all configuration
*           conditions are met for RIP to be active on this interface.
*
* @end
*********************************************************************/
L7_RC_t ripMapIntfAdminModeEnable(L7_uint32 intIfNum)
{
    return ripMapIntfAdminModeApply(intIfNum, L7_ENABLE);
}

/*********************************************************************
* @purpose  Disable RIP intf admin mode
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapIntfAdminModeDisable(L7_uint32 intIfNum)
{
    return ripMapIntfAdminModeApply(intIfNum, L7_DISABLE);
}


/*********************************************************************
* @purpose  Process routing-related events
*
* @param    intIfnum    port changing state
* @param    event       one of L7_PORT_EVENTS_t
* @param    *pData      pointer to event specific data
* @param    *pResponse  pointer to event specific data
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ripMapRoutingEventChangeCallBack (L7_uint32 intIfNum,
                                          L7_uint32 event,
                                          void *pData,
                                          ASYNC_EVENT_NOTIFY_INFO_t *pResponse)
{
  L7_RC_t rc;
  ripMapMsg_t msg;

  if (pRipInfo->ripMapProcessQueue == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  memset( (void *)&msg, 0, sizeof(ripMapMsg_t) );
  msg.msgId                     = RIPMAP_RTR_EVENT;
  msg.type.rtrEvent.intIfNum    = intIfNum;
  msg.type.rtrEvent.event       = event;
  msg.type.rtrEvent.pData       = pData;    /* Warning: when/if event-specific data is used,
                                               need to ensure that the information pointed to
                                               will still be valid (i.e. is fairly persistent
                                               until all tasks which need to use it can access
                                               it, or that, for a given event, the information
                                               is copied. */
  if (pResponse != L7_NULL)
  {
    memcpy(&(msg.type.rtrEvent.response), pResponse, sizeof(ASYNC_EVENT_NOTIFY_INFO_t) );
  }


  rc = osapiMessageSend(pRipInfo->ripMapProcessQueue, &msg, RIP_MAP_MSG_SIZE,
                        L7_NO_WAIT, L7_MSG_PRIORITY_NORM );

  if (rc == L7_SUCCESS)
  {
      osapiSemaGive(pRipInfo->msgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripMapRoutingEventChangeCallback: osapiMessageSend failed\n");
  }

  return rc;
}




/*********************************************************************
* @purpose  This routine is called when a routing event occurs that
*           may change the operational state of the RIP stack.
*
* @param    intIfnum    port changing state
* @param    event       one of L7_PORT_EVENTS_t
* @param    *pData      pointer to event specific data
* @param    *pResponse  pointer to event specific data
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    config entry point, must lock ripMapSemaphore to read/modify
*
* @end
*********************************************************************/
L7_RC_t ripMapRoutingEventChangeProcess(L7_uint32 intIfNum,
                                        L7_uint32 event,
                                        void *pData,
                                        ASYNC_EVENT_NOTIFY_INFO_t *pResponse)
{
  L7_uint32        mode;
  L7_BOOL          must_respond = L7_FALSE;
  L7_BOOL          event_completed = L7_FALSE;
  ASYNC_EVENT_COMPLETE_INFO_t event_completion;

  memset(&event_completion, 0, sizeof( ASYNC_EVENT_COMPLETE_INFO_t) );

  if (pResponse != L7_NULLPTR)
  {
    if ((pResponse->handlerId != L7_NULL) && (pResponse->correlator != L7_NULL))
    {
      must_respond = L7_TRUE;

      /* Initialize event completion data */
      /* NOTE:  Purposely overloading componentID with L7_IPMAP_REGISTRANTS_t */
      event_completion.componentId  = L7_IPRT_RIP;
      event_completion.handlerId    = pResponse->handlerId;
      event_completion.correlator   = pResponse->correlator;

      /* Default to return codes  and status.  Only select issues are not
         completed on this thread. */
      event_completion.async_rc.rc      = L7_SUCCESS;
      event_completion.async_rc.reason  = ASYNC_EVENT_REASON_RC_SUCCESS;
      event_completed                   = L7_TRUE;

      /* Initialize vendor completion signal */
      pRipInfo->ripVendIfCompletionSignal.signalInfo.async_event.handlerId
          = pResponse->handlerId;
      pRipInfo->ripVendIfCompletionSignal.signalInfo.async_event.correlator
          = pResponse->correlator;

    } /*  non null handlerId and correlator */
  }

  /*------------------*/
  /* Process event    */
  /*------------------*/

  /* add RIP interface */
  switch (event)
  {

    case L7_RTR_ENABLE:
    ripMapSemaTake();
    if (ripMapAdminModeGet(&mode) == L7_SUCCESS && mode == L7_ENABLE)
    {
      ripMapAdminModeEnable();
    }
    ripMapSemaGive();
    break;


  case L7_RTR_DISABLE_PENDING:
    /* Disable if curently operationally enabled. Will disable all interfaces.
     * Respond to IP MAP here. Leave callback set to NULL so individual
     * interfaces don't respond to IP MAP. */
    ripMapSemaTake();
    if (pRipInfo->ripAdminMode == L7_ENABLE)
    {
      ripMapAdminModeDisable();
    }
    ripMapSemaGive();
    break;

  case L7_RTR_DISABLE:
    break;

  case L7_RTR_INTF_CREATE:
    ripMapSemaTake();
    ripMapIntfCreate(intIfNum);
    ripMapSemaGive();
    break;

  case L7_RTR_INTF_DELETE:
    ripMapSemaTake();
    ripMapIntfDelete(intIfNum);
    ripMapSemaGive();
    break;

  case L7_RTR_INTF_ENABLE:
    ripMapSemaTake();
    if (ripMapIntfMayParticipate(intIfNum))
    {
      ripMapIntfAdminModeEnable(intIfNum);
    }
    ripMapSemaGive();
    break;


  case L7_RTR_INTF_DISABLE_PENDING:
    ripMapSemaTake();
    if (ripMapAdminModeGet(&mode) == L7_SUCCESS && mode == L7_ENABLE &&
        ripMapIntfIsValid(intIfNum) == L7_TRUE)
    {
      pRipInfo->ripVendIfWaitForCompletionSignal = L7_FALSE;
      if (must_respond)
      {
        pRipInfo->ripVendIfCompletionSignal.callback_func =
          (void *)ripMapRoutingIfEventCompletion;
      }
      else
      {
        pRipInfo->ripVendIfCompletionSignal.callback_func = L7_NULLPTR;
      }
      if (ripMapIntfAdminModeApply(intIfNum, L7_DISABLE) == L7_SUCCESS)
      {
        if (pRipInfo->ripVendIfWaitForCompletionSignal == L7_TRUE)
          event_completed = L7_FALSE;
        else
          pRipInfo->ripVendIfCompletionSignal.callback_func = L7_NULLPTR;
      }
    }
    ripMapSemaGive();
    break;


  case L7_RTR_INTF_DISABLE:
    /* Nothing to do. We cleaned up on the disable pending */
    break;

  case L7_RTR_INTF_SECONDARY_IP_ADDR_ADD:
  case L7_RTR_INTF_SECONDARY_IP_ADDR_DELETE:
      /* ripMapIpAddressAdd() simply makes sure the interface is activated and
     * causes RIP to re-evaluate its interfaces.  This would happen anyway
     * every 30 seconds, but this allows RIP to be updated as soon as the
     * new IP address is known. */

    ripMapSemaTake();
    ripMapIpAddressAdd(intIfNum);
    ripMapSemaGive();
    break;

  default:
    break;
  }

  /*-------------------*/
  /* Event completion? */
  /*-------------------*/

  if ((must_respond == L7_TRUE) && (event_completed == L7_TRUE))
  {
    /*  As an alternative to having event completion signaled elsewhere, it would be
       safe to do an routing event status callback here because this routine is
       running on the RIP processing task.  The RIP processing task will suspend
       awaiting a signal until the RIP task has a completed processing the request.
       */


    /* Return event completion status  */
    if (asyncEventCompleteTally(&event_completion) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
              "RIP response to completion of event %s, seqno %u, on interface %s failed.",
              ipMapRouterEventNames[event], event_completion.correlator, ifName);
    }
  }

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Return asynchronous event completion to the initiator of
*           the event.
*
* @param    *pResponse   pointer to ASYNC_RESPONSE_t structure
* @param    *eventData   pointer to event specific data
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapRoutingIfEventCompletion (ASYNC_RESPONSE_t *pResponse, L7_char8 *eventData)
{
  ASYNC_EVENT_COMPLETE_INFO_t event_completion;

  /* Initialize the structure for the final tally */
  memset(&event_completion, 0, sizeof( ASYNC_EVENT_COMPLETE_INFO_t) );

  event_completion.componentId  = L7_IPRT_RIP;
  event_completion.handlerId    = pRipInfo->ripVendIfCompletionSignal.signalInfo.async_event.handlerId;
  event_completion.correlator   = pRipInfo->ripVendIfCompletionSignal.signalInfo.async_event.correlator;

  memcpy( &(event_completion.async_rc), pResponse, sizeof(ASYNC_RESPONSE_t) );

  /* Return event completion status  */
  asyncEventCompleteTally(&event_completion);

  /* Reset the completion signal so that unnecessary callbacks do not occur
   * (i.e. on the next event) */
  pRipInfo->ripVendIfCompletionSignal.callback_func = L7_NULLPTR;
}


/*********************************************************************
* @purpose  Restore RIP user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    config entry point, must lock ripMapSemaphore to read/modify
*
* @end
*********************************************************************/
void ripMapRestoreProcess(void)
{
  ripMapSemaTake();

  /* First disable rip if it is currently enabled */
  if (pRipMapCfgData->ripAdminMode != L7_DISABLE)
  {
    pRipMapCfgData->ripAdminMode = L7_DISABLE;
    (void)ripMapAdminModeDisable();
  }

  /* reset the preference for RIP routes */
  (void)ipMapRouterPreferenceSet(ROUTE_PREF_RIP, FD_RTR_RTO_PREF_RIP);

  /* Restore default configuration */
  pRipMapCfgData->cfgHdr.dataChanged = L7_TRUE;
  ripMapBuildDefaultConfigData(L7_RIP_MAP_CFG_VER_CURRENT);
  (void)ripMapApplyConfigData();

  ripMapSemaGive();
  return;
}


/*********************************************************************
* @purpose  Callback function to process ACL changes. OSPF is interested in
*           ACL changes if it is configured to use an ACL to filter routes
*           for redistribution.
*
* @param    aclNum ACL number of ACL that has changed
* @param    event  add, modify, or delete ACL
*
* @returns  void
*
* @notes    For now, we reevaluate all best routes, regardless of the
*           type of event.
* @notes    config entry point, must lock ripMapSemaphore to read/modify
*
* @end
*********************************************************************/
void ripMapAclCallback(L7_uint32 aclNum, aclRouteFilter_t event)
{
  /* See if this ACL is used to filter routes from any source. */
  L7_uint32 i;
  L7_BOOL change = L7_FALSE;

  ripMapSemaTake();
  for (i = REDIST_RT_FIRST + 1; i < REDIST_RT_LAST; i++)
  {
    if (i == REDIST_RT_RIP)
    {
      continue;
    }
    if ((pRipInfo->ripRtRedistMode[i] == L7_ENABLE) &&
        (pRipMapCfgData->ripRtRedist[i].ripRtRedistFilter.mode == L7_ENABLE) &&
        (pRipMapCfgData->ripRtRedist[i].ripRtRedistFilter.filter == aclNum))
    {
      change = L7_TRUE;
      break;
    }
  }
  if (change)
  {
    ripMapExtenRouteRedistFilterChange(aclNum);
  }
  ripMapSemaGive();
  return;
}

/*********************************************************************
* @purpose  Respond to the addition of an IP address on an interface.
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if RIP is not initialized
*
* @notes    ripMapSemaphore must be locked when calling this function
* @notes    Adding an IP address to the interface may be the last
*           condition required to bring up the interface. If so,
*           tell protocol engine to start using the interface.
*
* @end
*********************************************************************/
L7_RC_t ripMapIpAddressAdd(L7_uint32 intIfNum)
{
    /* should never get this callback if RIP app not initialized */
  if (ripMapIsInitialized() != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripMapIpAddressChangeCallback: RIP app not initialized\n");
    return L7_FAILURE;
  }

  if (ripMapIntfMayParticipate(intIfNum))
  {
    return ripMapIntfAdminModeEnable(intIfNum);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine is called whenever the IP address of a router
*           interface changes.
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if RIP is not initialized
*
* @notes    ripMapSemaphore must be locked when calling this function
* @notes    This function assumes the calling function checks for:
*           1] The user trying to set the ip address to null
*           2] The old ip address != new ip address specified by the
*              user
*
*           This function is called in response to the event
*           L7_RTR_INTF_IPADDR_CHANGE. Prior to this event, we
*           probably received a L7_RTR_INTF_IPADDR_CHANGE_PENDING
*           event, and responded to that event by deleting the
*           original IP address. This seems inefficient (RIP has
*           to reinitialize all interfaces twice instead of once), but
*           is presumably necessary because of idiosyncracies in the
*           IP mapping code. So the action here is the same as for an
*           add.
*
* @end
*********************************************************************/
L7_RC_t ripMapIpAddressChangeCallBack(L7_uint32 intIfNum)
{
  /* should never get this callback if RIP app not initialized */
  if (ripMapIsInitialized() != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripMapIpAddressChangeCallback: RIP app not initialized\n");
    return L7_FAILURE;
  }

  if (ripMapIntfMayParticipate(intIfNum) == L7_TRUE)
  {
      return ripMapIntfAdminModeEnable(intIfNum);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This routine is called whenever the IP address of a router
*           interface is deleted
*
* @param    intIfnum    internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapSemaphore must be locked when calling this function
*
* @end
*********************************************************************/
L7_RC_t ripMapIpAddressDeleteCallBack(L7_uint32 intIfNum)
{
  /* should never get this callback if RIP app not initialized */
  if (ripMapIsInitialized() != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
            "ripMapIpAddressChangeCallback: RIP app not initialized\n");
    return L7_FAILURE;
  }

  /* The protocol engine only knows about UP interfaces */
  if (pRipIntfInfo[intIfNum].state == RIP_MAP_INTF_STATE_UP)
  {
    ripMapIntfAdminModeDisable(intIfNum);
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Route table change report callback function
*
*
* @param    void
*
* @returns  void
*
* @notes    Currently, the best route change notifications are used to control redistribution
*           of routes into RIP from other protocols.
*           The passthrough parameter is not currently used here.
*
* @end
*********************************************************************/
void ripMapBestRouteChangeCallback(void)
{
  if (!pRipInfo->bestRouteTimer)
    /* schedule an event to request changes from RTO */
    osapiTimerAdd((void*)ripMapBestRouteTimerCb, L7_NULL, L7_NULL,
                  L7_RIP_BEST_ROUTE_DELAY, &pRipInfo->bestRouteTimer);

  return;
}

/*********************************************************************
*
* @purpose Timer callback to trigger RIP to get best routes from RTO.
*
* @param void
*
* @returns void
*
* @comments  This callback tells RIP that best route changes are pending
*            in RTO. RIP has to go back to RTO and ask for the changes.
*
* @end
*
*********************************************************************/
void ripMapBestRouteTimerCb(void)
{
  pRipInfo->bestRouteTimer = NULL;
  ripMapBestRouteEventPost();
}

/*********************************************************************
*
* @purpose Put an event on the RIP map thread message queue to
*          retrieve best route changes from RTO.
*
* @param void
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
static void ripMapBestRouteEventPost(void)
{
    ripMapMsg_t msg;
    L7_RC_t rc;

    if (pRipInfo->ripMapRedistQueue == L7_NULLPTR) {
      return;
    }

    /* RIP could have deregistered after being notified of
     * an outstanding change. No need to request the change. */
    if (!ripIsRegisteredWithRto())
      return;

    memset((void*) &msg, 0, sizeof(ripMapMsg_t));
    msg.msgId = RIPMAP_RTO_EVENT;

    rc = osapiMessageSend(pRipInfo->ripMapRedistQueue, &msg, sizeof(ripMapMsg_t),
                          L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

    if (rc == L7_SUCCESS)
    {
        osapiSemaGive(pRipInfo->msgQSema);
    }
    else
    {
        RIPMAP_ERROR("Failed to send best route notification message to RIP mapping thread.");
    }
    return;
}

/*********************************************************************
*
* @purpose Request a set of best route changes from RTO. If more changes
*          remain pending, generate another event to ask for more.
*
* @param void
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ripMapBestRoutesGet(void)
{
  L7_uint32 i;
  L7_uint32 numChanges = 0;
  L7_BOOL moreChanges = L7_FALSE;
  rtoRouteChange_t *routeChange;

  /* RIP could have deregistered after being notified of
     * an outstanding change. No need to request the change. */
  if (!ripIsRegisteredWithRto())
    return L7_SUCCESS;

  if (rtoRouteChangeRequest(ripMapBestRouteChangeCallback, L7_RIP_MAX_BEST_ROUTE_CHANGES,
                            &numChanges, &moreChanges,
                            ripRouteChangeBuf) != L7_SUCCESS)
  {
    /* Don't schedule another try. */
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RIP_MAP_COMPONENT_ID,
            "Failed to retrieve best route changes from RTO.");
    return L7_FAILURE;
  }

  /* process each best route change */
  for (i = 0; i < numChanges; i++)
  {
    routeChange = &ripRouteChangeBuf[i];
    ripBestRouteChange(&routeChange->routeEntry, routeChange->changeType);
  }

  if (moreChanges)
  {
    /* post another event to the OSPF message queue to request more
     * changes from RTO */
    ripMapBestRouteEventPost();
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process a best route change notification from RTO.
*
* @param  routeEntry  @b{(input)} Route that was added/ deleted/ modified
* @param  route_status  @b{(input)} One of RTO_ADD_ROUTE, RTO_DELETE_ROUTE,
*                                    RTO_CHANGE_ROUTE
*
* @returns  L7_SUCCESS
*
* @notes    Best route change notifications are used to trigger redistribution
*           of routes into RIP from other protocols.
*
* @end
*********************************************************************/
L7_RC_t ripBestRouteChange(L7_routeEntry_t *routeEntry,
                           RTO_ROUTE_EVENT_t route_status)
{
    switch (route_status)
    {
    case RTO_ADD_ROUTE:
        (void)ripMapExtenRouteAdd(routeEntry->ipAddr, routeEntry->subnetMask,
                                  routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.ipAddr,
                                  routeEntry->protocol, routeEntry->flags);
        break;

    case RTO_DELETE_ROUTE:
        (void)ripMapExtenRouteDelete(routeEntry->ipAddr, routeEntry->subnetMask);
        break;

    case RTO_CHANGE_ROUTE:
        (void)ripMapExtenRouteChange(routeEntry->ipAddr, routeEntry->subnetMask,
                                     routeEntry->ecmpRoutes.equalCostPath[0].arpEntry.ipAddr,
                                     routeEntry->protocol, routeEntry->flags);
        break;

    default:
        break;
    }

    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert an IP address to an ASCII string
*
* @param    ipAddr      IP address
* @param    pBuf        @b{(output)} location of output buffer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Caller must ensure the output buffer is large enough
*           (RIP_MAP_IPADDR_STR_LEN size is sufficient).
*
* @end
*********************************************************************/
void ripMapIpAddrToStr(L7_uint32 ipAddr, L7_uchar8 *pBuf)
{
  sprintf(pBuf, "%d.%d.%d.%d",
          ((ipAddr >> 24) & 0xFF), ((ipAddr >> 16) & 0xFF),
          ((ipAddr >> 8) & 0xFF), ((ipAddr >> 0) & 0xFF));
}

/*********************************************************************
* @purpose  Sets the level of RIP Mapping layer debugging messages to display
*
* @param    msgLvl      message level (0 = off, >0 on)
*
* @returns  void
*
* @notes    Messages are defined according to interest level.  Higher
*           values generally display more debug messages.
*
* @end
*********************************************************************/
void ripMapMsgLvlSet(L7_uint32 msgLvl)
{
  ripMapCtrl_g.msgLvl = msgLvl;
}

/*********************************************************************
* @purpose  Discard a RIPv1 packet addressed to a multicast (class D) address.
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_DISCARD     if frame is to be discarded
* @returns  SYSNET_PDU_RC_IGNORED     continue processing this frame
*
* @notes    Recall that RIP packets are targeted to UDP port 520.
*           This is a convenient way to filter out such packets, since the RIP
*           code uses a datagram socket instead of a raw socket and therefore
*           does not have access to the destination IP address.
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t ripMapFrameFilter(L7_uint32 hookId,
                                  L7_netBufHandle bufHandle,
                                  sysnet_pdu_info_t *pduInfo,
                                  L7_FUNCPTR_t continueFunc)
{
  L7_char8 *data;
  L7_ipHeader_t *ipHeader;
  L7_udp_header_t *udpHeader;
  typedef struct netinfo
  {
    struct sockaddr rip_dst;
    int    rip_metric;
  }NETINFO;
  typedef struct rip_pkt
  {
    u_char  rip_cmd;
    u_char  rip_vers;
    u_char  rip_domain[2];
    union
    {
      struct netinfo ru_nets[1];
    }ripun;
      #ifndef rip_nets
      #define rip_nets ripun.ru_nets;
      #endif
  }RIP_PKT;
  RIP_PKT *rip_pkt;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  ipHeader = (L7_ipHeader_t *)(data + sysNetDataOffsetGet(data));
  if ( (ipHeader->iph_versLen == ((L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN)) &&
       (ipHeader->iph_prot == IPPROTO_UDP) )
  {
    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + sizeof(L7_ipHeader_t));
    if (osapiNtohs(udpHeader->destPort) == RIP_PORT)
    {
      rip_pkt = (RIP_PKT *)((L7_char8 *)udpHeader + sizeof(L7_udp_header_t));
      if ((rip_pkt->rip_vers == 1) && IN_MULTICAST(osapiNtohl(ipHeader->iph_dst)))
      {
        return SYSNET_PDU_RC_DISCARD; /* discard frame */
      }
    }
  }

  return SYSNET_PDU_RC_IGNORED; /* continue processing frame */
}

/*********************************************************************
* @purpose  Display internal RIP information for debugging
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void ripMapDebugShow(void)
{
  ripIntfInfo_t *pIntf;
  L7_uint32     i;
  L7_uchar8 *ripActiveState = "Disabled";

  ripMapSemaTake();

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "RIP Internal Debug Information\n");

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  RIP Control Ctx:  taskSemId=0x%8.8x  taskSemAvail=%d  msgLvl=%d\n",
              (L7_uint32)ripMapCtrl_g.taskCtrl.semId, ripMapCtrl_g.taskCtrl.semAvail,
              ripMapCtrl_g.msgLvl);

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "    traceMode=%d  RIP Initialized=%d  Task Id=0x%8.8x\n",
              ripMapCtrl_g.traceEnabled, pRipInfo->ripInitialized, pRipInfo->ripTaskId);

  if (pRipInfo->ripAdminMode == L7_ENABLE)
      ripActiveState = "Enabled";
  else
      ripActiveState = "Disabled";
  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "  RIP active state (pRipInfo->ripAdminMode):  %s",
              ripActiveState);

  if (pRipInfo->registeredWithRto)
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "  RIP is registered for best route changes.");

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  Interfaces (internal interface number):\n");
  for (i = 1, pIntf = pRipIntfInfo; i <= L7_MAX_INTERFACE_COUNT; i++, pIntf++)
  {
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
                "    [%2.1d]:  RIP interface state: %s\n", i,
                (pRipIntfInfo[i].state == RIP_MAP_INTF_STATE_NONE) ? "NONE" :
                (pRipIntfInfo[i].state == RIP_MAP_INTF_STATE_DOWN) ? "DOWN" :
                (pRipIntfInfo[i].state == RIP_MAP_INTF_STATE_UP) ? "UP" : "ERROR");
  }

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "\n");

  ripMapSemaGive();
  return;
}

/*********************************************************************
* @purpose  Display RIP config structure contents for debugging
*
* @param    slot        slot number to display (or 0 for all slots)
* @param    port        port number to display (or 0 for all ports w/in slot)
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ripMapConfigDataShow(L7_uint32 slot, L7_uint32 port)
{
  ripMapCfg_t     *pCfg;
  ripMapCfgIntf_t *pIntf;
  L7_uint32         i;
  L7_uchar8         key[L7_AUTH_MAX_KEY_RIP+1];
  L7_uint32         keyLen;

  if ((slot > L7_MAX_SLOTS_PER_UNIT) || (port > L7_MAX_PORTS_PER_SLOT))
  {
    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
                "Invalid parameter: slot=%u, port=%u\n", slot, port);
    return;
  }

  pCfg = pRipMapCfgData;

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "RIP Config Data Contents\n");

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  RIP Admin Mode:  %u\n", pCfg->ripAdminMode);

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  RIP Split Horizon Mode:  %u\n", pCfg->ripSplitHorizon);

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  RIP Auto Summarization Mode:  %u\n", pCfg->ripAutoSummarization);

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  RIP Host Routes Accept Mode:  %u\n", pCfg->ripHostRoutesAccept);

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
              "  Routing interfaces:\n");


  for (i = 1; i < L7_RIP_MAP_INTF_MAX_COUNT; i++)
  {
    pIntf = &pCfg->intf[i];

    keyLen = pIntf->authKeyLen;
    memcpy(key, pIntf->authKey, (size_t)keyLen);
    key[keyLen] = L7_EOS;

    RIP_MAP_PRT(RIP_MAP_MSGLVL_ON,
                "    %u :  mode=%u auth=%u keyid=%u vsnd=%u vrcv=%u cost=%u key=\"%s\"\n",
                i, pIntf->adminMode, pIntf->authType, pIntf->authKeyId, pIntf->verSend,
                pIntf->verRecv, pIntf->intfCost, key);
  } /* endfor i */

  RIP_MAP_PRT(RIP_MAP_MSGLVL_ON, "\n");
}



/*********************************************************************
* @purpose  Saves ripMap debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t ripMapDebugSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  if (ripMapDebugHasDataChanged() == L7_TRUE)
  {
    /* Copy the operational states into the config file */
    ripMapDebugCfgUpdate();

    /* Store the config file */
    ripMapDebugCfg.hdr.dataChanged = L7_FALSE;
    ripMapDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&ripMapDebugCfg,
                                      (L7_uint32)(sizeof(ripMapDebugCfg) - sizeof(ripMapDebugCfg.checkSum)));
        /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_RIP_MAP_COMPONENT_ID, RIPMAP_DEBUG_CFG_FILENAME ,
                            (L7_char8 *)&ripMapDebugCfg, (L7_uint32)sizeof(ripMapDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RIP_MAP_COMPONENT_ID,
              "Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",RIPMAP_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores ripMap debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ripMapDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t ripMapDebugRestore(void)
{
  L7_RC_t rc;
  ripMapDebugBuildDefaultConfigData(RIPMAP_DEBUG_CFG_VER_CURRENT);

  ripMapDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = ripMapApplyDebugConfigData();

  return rc;
}

/*********************************************************************
* @purpose  Checks if ripMap debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL ripMapDebugHasDataChanged(void)
{
  return ripMapDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default rip config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ripMapDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  ripMapDebugCfg.hdr.version = ver;
  ripMapDebugCfg.hdr.componentID = L7_RIP_MAP_COMPONENT_ID;
  ripMapDebugCfg.hdr.type = L7_CFG_DATA;
  ripMapDebugCfg.hdr.length = (L7_uint32)sizeof(ripMapDebugCfg);
  strcpy((L7_char8 *)ripMapDebugCfg.hdr.filename, RIPMAP_DEBUG_CFG_FILENAME);
  ripMapDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&ripMapDebugCfg.cfg, 0, sizeof(ripMapDebugCfg.cfg));
}

/*********************************************************************
* @purpose  Apply rip debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t ripMapApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc = ripMapDebugPacketTraceFlagSet(ripMapDebugCfg.cfg.ripMapDebugPacketTraceFlag);

  return rc;
}


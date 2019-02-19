/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_cfg.c
*
* @purpose RADIUS configuration file
*
* @component radius
*
* @comments none
*
* @create 03/24/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#include "radius_include.h"
#ifdef L7_ROUTING_PACKAGE
#include "l7_ip_api.h"
#endif

radiusClient_t radiusClient;
radiusCfg_t radiusCfg;

extern radiusCnfgrState_t radiusCnfgrState;
extern void * radius_queue;
extern void * radiusClusteringQueue;
extern void * radiusTaskSyncSema;
extern void * radiusServerDbSyncSema;
extern L7_uint32 radius_task_id;
extern L7_uint32 radius_rx_task_id;
extern L7_uint32 radius_cluster_task_id;

L7_uint32 radius_sm_bp_id = L7_NULL;
L7_uint32 radius_lg_bp_id = L7_NULL;

/*********************************************************************
*
* @purpose System Initialization for radius component
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t radiusInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  L7_RC_t radiusRC = L7_SUCCESS;
  L7_CNFGR_CB_DATA_t cbData;
  radiusMsg_t msg;

  /* create the task queue  */
  radius_queue = (void*)osapiMsgQueueCreate("radius_queue",
                                            FD_CNFGR_RADIUS_MSG_COUNT,
                                            (L7_uint32)sizeof(radiusMsg_t));

  if (radius_queue == L7_NULL)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Severe error, unable to create msg queue!\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    radiusRC   = L7_FAILURE;

    return radiusRC;
  }

  /* create the task queue  */
  radiusClusteringQueue = (void*)osapiMsgQueueCreate("radiusClusteringQueue",
                                                     2,(L7_uint32)sizeof(radiusMsg_t));

  if (radiusClusteringQueue == L7_NULL)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Severe error, unable to create cluster msg queue!\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    radiusRC   = L7_FAILURE;

    return radiusRC;
  }

  /* semaphore creation for task protection over the common data*/
  radiusTaskSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (radiusTaskSyncSema == L7_NULL)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Unable to create the task semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    radiusRC   = L7_FAILURE;

    return radiusRC;
  }
  
  radiusServerDbSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (radiusServerDbSyncSema == L7_NULL)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Unable to create the server Db semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    radiusRC   = L7_FAILURE;
    return radiusRC;
  }

  radiusServerDbSyncSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (radiusServerDbSyncSema == L7_NULL)
  {
    RADIUS_ERROR_SEVERE("RADIUS: Unable to create the server Db semaphore\n");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    radiusRC   = L7_FAILURE;
    return radiusRC;
  }


  if (radiusBufferPoolInit() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    radiusRC   = L7_FAILURE;

    return radiusRC;
  }

  if (radiusStartTasks() != L7_SUCCESS)
  {
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);

    radiusRC   = L7_FAILURE;

    return radiusRC;
  }

  memcpy(&msg.data.cmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.event = RADIUS_CNFGR_INIT;

  (void)osapiMessageSend(radius_queue, &msg, sizeof(radiusMsg_t), L7_NO_WAIT, L7_MSG_PRIORITY_NORM);

  return radiusRC;
}

/*********************************************************************
*
* @purpose System Init Undo for IGMP Snooping component
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void radiusInitUndo()
{
  if (radius_queue != L7_NULL)
    (void)osapiMsgQueueDelete(radius_queue);

  if (radius_task_id != L7_ERROR)
    osapiTaskDelete(radius_task_id);

  if (radius_rx_task_id != L7_ERROR)
    osapiTaskDelete(radius_rx_task_id);

  if (radius_cluster_task_id != L7_ERROR)
    osapiTaskDelete(radius_cluster_task_id);

  if (radiusServerDbSyncSema != L7_NULL)
    (void)osapiSemaDelete(radiusServerDbSyncSema);

  if (radiusTaskSyncSema != L7_NULL)
    (void)osapiSemaDelete(radiusTaskSyncSema);

  radiusCnfgrState = RADIUS_PHASE_INIT_0;
}

/*********************************************************************
*
* @purpose To handle the configurator commands sent to radius_task
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void radiusCnfgrHandle(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response;
  L7_CNFGR_CORRELATOR_t correlator = L7_NULL;

  L7_RC_t             radiusRC = L7_ERROR;
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
                if ((radiusRC = radiusCnfgrInitPhase1Process( &response, &reason )) != L7_SUCCESS)
                {
                  radiusCnfgrFiniPhase1Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE2_START:
                if ((radiusRC = radiusCnfgrInitPhase2Process( &response, &reason )) != L7_SUCCESS)
                {
                  radiusCnfgrFiniPhase2Process();
                }
                break;
              case L7_CNFGR_RQST_I_PHASE3_START:
                if ((radiusRC = radiusCnfgrInitPhase3Process( &response, &reason )) != L7_SUCCESS)
                {
                  radiusCnfgrFiniPhase3Process();
                }
                break;
              case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
                radiusRC = radiusCnfgrNoopProcess( &response, &reason );
                radiusCnfgrState = RADIUS_PHASE_WMU;
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
                radiusCnfgrState = RADIUS_PHASE_EXECUTE;

                radiusRC  = L7_SUCCESS;
                response  = L7_CNFGR_CMD_COMPLETE;
                reason    = L7_NULL;
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
                radiusRC = radiusCnfgrNoopProcess( &response, &reason );
                radiusCnfgrState = RADIUS_PHASE_UNCONFIG_1;
                break;

              case L7_CNFGR_RQST_U_PHASE2_START:
                radiusRC = radiusCnfgrUconfigPhase2( &response, &reason );
                radiusCnfgrState = RADIUS_PHASE_UNCONFIG_2;
                /* we need to do something with the stats in the future */
                break;

              default:
                /* invalid command/request pair */
                break;
            }
            break;

          case L7_CNFGR_CMD_TERMINATE:
          case L7_CNFGR_CMD_SUSPEND:
            radiusRC = radiusCnfgrNoopProcess( &response, &reason );
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
   * <prepare completion response>
   * <callback the configurator>
   */
  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = radiusRC;
  if (radiusRC == L7_SUCCESS)
    cbData.asyncResponse.u.response = response;
  else
    cbData.asyncResponse.u.reason   = reason;

  cnfgrApiCallback(&cbData);

  return;
}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Init Phase 1.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*        L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t radiusCnfgrInitPhase1Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t radiusRC;
  L7_uint32 i;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  radiusRC   = L7_SUCCESS;

  /*
  ** Initialize the RADIUS Global Data.
  */
  radiusServerEntries = L7_NULL;
  memset((void *)&radiusNotifyList, L7_NULL, sizeof(radiusNotifyList));
  memset((void *)&radiusGlobalData, L7_NULL, sizeof(radiusGlobalData));

  for(i=0;i<L7_RADIUS_MAX_AUTH_SERVERS;i++)
  {
    memset(authServerNameArray[i].serverName,L7_NULL,L7_RADIUS_SERVER_NAME_LENGTH+1);    
    authServerNameArray[i].currentEntry.auth.authServer      = L7_NULLPTR;
    authServerNameArray[i].currentEntry.auth.authDot1xServer = L7_NULLPTR;
    authServerNameArray[i].currentEntry.auth.authLoginServer = L7_NULLPTR;
    authServerNameArray[i].serverCount = L7_NULL;
  }
  for(i=0;i<L7_RADIUS_MAX_ACCT_SERVERS;i++)
  {
    memset(acctServerNameArray[i].serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);
    acctServerNameArray[i].currentEntry.acct.acctServer = L7_NULLPTR;
    acctServerNameArray[i].serverCount = L7_NULL;
  }

  /*
  ** Read the dictionary file and initialize the dictionary defs in memory
  */
  if (radiusDictionaryInit() != L7_SUCCESS)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    radiusRC   = L7_ERROR;

    return radiusRC;
  }

  radiusCnfgrState = RADIUS_PHASE_INIT_1;

  return radiusRC;

}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Init Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*        L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t radiusCnfgrInitPhase2Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t radiusRC;
  nvStoreFunctionList_t notifyFunctionList;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  radiusRC   = L7_SUCCESS;

  memset((void *) &notifyFunctionList, L7_NULL, sizeof(notifyFunctionList));
  notifyFunctionList.registrar_ID   = L7_RADIUS_COMPONENT_ID;
  notifyFunctionList.notifySave     = radiusSave;
  notifyFunctionList.hasDataChanged = radiusHasDataChanged;
  notifyFunctionList.resetDataChanged = radiusResetDataChanged;

  if ( nvStoreRegister(notifyFunctionList) != L7_SUCCESS )
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    radiusRC   = L7_ERROR;

    return radiusRC;
  }

  if ( radiusDebugInit() != L7_SUCCESS )
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    radiusRC   = L7_ERROR;

    return radiusRC;
  }

  radiusCnfgrState = RADIUS_PHASE_INIT_2;
  
  #ifdef L7_ROUTING_PACKAGE
    radiusGlobalData.totalNwInterfaces += L7_RTR_MAX_RTR_INTERFACES;
  #endif

  #ifdef L7_RLIM_PACKAGE 
    radiusGlobalData.totalNwInterfaces +=L7_MAX_NUM_LOOPBACK_INTF;
  #endif


#ifdef L7_RADIUS_ROUTING_LOOPBACK_SELECTED
  radiusGlobalData.socketInactiveTime = 
                      FD_RADIUS_SOCKET_INACTIVE_TIME * RADIUS_TIMER_GRANULARITY;
#endif

#ifdef L7_ROUTING_PACKAGE
  /* Register RADIUS to receive notification of routing events. */
  if (ipMapRegisterRoutingEventChange(L7_IPRT_RADIUS,
                                      "radiusRoutingEventChangeCallBack",
                                      radiusRoutingEventChangeCallBack) != L7_SUCCESS)
  {
      RADIUS_ERROR_SEVERE("Error registering OSPF for routing events.\n");
      *pResponse = 0;
      *pReason   = L7_CNFGR_ERR_RC_FATAL;
      return L7_ERROR;
  }
#endif

  return radiusRC;
}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Init Phase 3.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*        L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t radiusCnfgrInitPhase3Process( L7_CNFGR_RESPONSE_t *pResponse,
                                      L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t radiusRC;

  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;
  radiusRC   = L7_SUCCESS;

  if (sysapiCfgFileGet(L7_RADIUS_COMPONENT_ID, RADIUS_CFG_FILENAME,
                   (L7_char8 *)(&radiusCfg), (L7_uint32)sizeof(radiusCfg_t),
                   &(radiusCfg.checkSum), RADIUS_CFG_VER_CURRENT,
                   radiusBuildConfigData, radiusMigrateConfigData) != L7_SUCCESS)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_NOT_FOUND;
    radiusRC   = L7_ERROR;

    return radiusRC;
  }

  radiusApplyConfigData();

  radiusCnfgrState = RADIUS_PHASE_INIT_3;

  return radiusRC;
}


/*********************************************************************
*
* @purpose This function undoes policyCnfgrInitPhase1Process
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void radiusCnfgrFiniPhase1Process()
{
  (void)radiusClientShutdown();
  LOG_MSG("RADIUS: Failed to initialize the dictionary\n");

  radiusCnfgrState = RADIUS_PHASE_INIT_0;
}


/*********************************************************************
*
* @purpose This function undoes policyCnfgrInitPhase2Process
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void radiusCnfgrFiniPhase2Process()
{
  (void)nvStoreDeregister(L7_RADIUS_COMPONENT_ID);

  radiusCnfgrState = RADIUS_PHASE_INIT_1;
}

/*********************************************************************
*
* @purpose This function undoes policyCnfgrInitPhase2Process
*
* @retruns void
*
* @notes none
*
* @end
*
*********************************************************************/
void radiusCnfgrFiniPhase3Process()
{
  L7_CNFGR_RESPONSE_t response;
  L7_CNFGR_ERR_RC_t   reason;
  
  (void)radiusCnfgrUconfigPhase2(&response, &reason);
}


/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair as a NOOP.
*
* @param    pResponse - @b{(output)}  Response always command complete.
* @param    pReason   - @b{(output)}  Always 0
*
* @returns L7_SUCCESS - Always return this value. onse is available.
*
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        None.
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t radiusCnfgrNoopProcess( L7_CNFGR_RESPONSE_t *pResponse,
                                L7_CNFGR_ERR_RC_t   *pReason )
{
  L7_RC_t radiusRC = L7_SUCCESS;

  /* Return Value to caller */
  *pResponse = L7_CNFGR_CMD_COMPLETE;
  *pReason   = L7_NULL;

  return(radiusRC);
}

/*********************************************************************
*
* @purpose This function process the configurator control commands/request
*          pair Unconfigure Phase 2.
*
* @param    pResponse - @b{(output)}  Response if L7_SUCCESS.
* @param    pReason   - @b{(output)}  Reason if L7_ERROR.
*
* @returns L7_SUCCESS - There were no errors. Response is available.
* @returns L7_ERROR   - There were errors. Reason code is available.
*
* @notes The following are valid response:
*        L7_CNFGR_CMD_COMPLETE
*
* @notes The following are valid error reason code:
*        L7_CNFGR_ERR_RC_FATAL
*
* @notes This function runs in the configurator's thread. This thread MUST NOT
*        be blocked!
*
* @end
*
*********************************************************************/
L7_RC_t radiusCnfgrUconfigPhase2( L7_CNFGR_RESPONSE_t *pResponse,
                                  L7_CNFGR_ERR_RC_t   *pReason )
{

  L7_RC_t radiusRC;

  *pResponse  = L7_CNFGR_CMD_COMPLETE;
  *pReason    = L7_NULL;
  radiusRC    = L7_SUCCESS;

  /* Clear out the configuration */
  memset((void *)(&radiusCfg), L7_NULL, sizeof(radiusCfg_t));

  if (radiusRestore() != L7_SUCCESS)
  {
    *pResponse = L7_NULL;
    *pReason   = L7_CNFGR_ERR_RC_FATAL;
    radiusRC   = L7_ERROR;

    return radiusRC;
  }

  radiusCnfgrState = RADIUS_PHASE_WMU;

  return radiusRC;
}


/*********************************************************************
*
* @purpose Initialize the buffer pool for the RADIUS component
*
* @returns L7_SUCCESS - Buffer pool created.
* @returns L7_FAILURE - Buffer pool not created.
*
* @end
*
*********************************************************************/
L7_RC_t radiusBufferPoolInit(void)
{
  if ((bufferPoolInit(RADIUS_SM_BUF_NUM,
                      RADIUS_SM_BUF_SIZE,
                      RADIUS_SM_BUF_LABEL,
                      &radius_sm_bp_id) != L7_SUCCESS) ||
      (bufferPoolInit(RADIUS_LG_BUF_NUM,
                      RADIUS_LG_BUF_SIZE,
                      RADIUS_LG_BUF_LABEL,
                      &radius_lg_bp_id) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Saves the RADIUS Client configuration
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (radiusHasDataChanged() == L7_TRUE)
  {
    radiusCfgUpdate();
    radiusCfg.hdr.dataChanged = L7_FALSE;
    radiusCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&radiusCfg,
                                      (L7_uint32)(sizeof(radiusCfg) - sizeof(radiusCfg.checkSum)));

    if ((rc = sysapiCfgFileWrite(L7_RADIUS_COMPONENT_ID, RADIUS_CFG_FILENAME,
                     (L7_char8 *)&radiusCfg,
                     (L7_int32)sizeof(radiusCfg))) != L7_SUCCESS)
    {
      LOG_MSG("RADIUS: could not save config file %s\n", RADIUS_CFG_FILENAME);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose Restores the RADIUS Client configuration
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments radiusCfg is the overlay
*
* @end
*
*********************************************************************/
L7_RC_t radiusRestore(void)
{
  /* remove current config if necessary */
  radiusConfiguredDataRemove();

  /* build the default config */
  radiusBuildConfigData(RADIUS_CFG_VER_CURRENT);

  /* apply the stored config */
  radiusApplyConfigData();

  radiusCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Checks if RADIUS Client user config data has changed
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL radiusHasDataChanged(void)
{
  return radiusCfg.hdr.dataChanged;
}
void radiusResetDataChanged(void)
{
  radiusCfg.hdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
*
* @purpose Removes any RADIUS Client configured data
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusConfiguredDataRemove(void)
{
  /*
  ** Remove all configured servers and associated data
  */
  (void)radiusServerRemoveAll();

  return;
}

/*********************************************************************
*
* @purpose Build default RADIUS Client config data
*
* @param ver @b((input)) software version of Config Data
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusBuildConfigData(L7_uint32 ver)
{
  L7_uint32 i;
  radiusClient_t *cfgClient;

  /* setup file header */
  radiusCfg.hdr.version = ver;
  radiusCfg.hdr.componentID = L7_RADIUS_COMPONENT_ID;
  radiusCfg.hdr.type = L7_CFG_DATA;
  radiusCfg.hdr.length = (L7_uint32)sizeof(radiusCfg);

  strcpy((L7_char8 *)radiusCfg.hdr.filename, RADIUS_CFG_FILENAME);
  radiusCfg.hdr.dataChanged = L7_FALSE;

  /* zero overlay */
  bzero((L7_char8 *)&radiusCfg.cfg.radiusClient, sizeof(radiusCfg.cfg.radiusClient));

  /* default values for the radiusClient structure */
  cfgClient = &radiusCfg.cfg.radiusClient;

  cfgClient->authOperMode = FD_RADIUS_AUTH_OPER_MODE;
  cfgClient->acctAdminMode = FD_RADIUS_ACCT_ADMIN_MODE;
  cfgClient->maxNumRetrans = FD_RADIUS_MAX_RETRANS;
  cfgClient->timeOutDuration = FD_RADIUS_TIMEOUT_DURATION * RADIUS_TIMER_GRANULARITY;
  cfgClient->retryPrimaryTime = FD_RADIUS_RETRY_PRIMARY_TIME * RADIUS_TIMER_GRANULARITY;
  cfgClient->nasIpMode = FD_RADIUS_NAS_IP_MODE;
  cfgClient->nasIpAddress = 0;
  cfgClient->numAuthServers = L7_NULL;
  cfgClient->radiusServerDeadTime = FD_RADIUS_DEADTIME * DEADTIME_EXTRA_GRANULARITY * RADIUS_TIMER_GRANULARITY;
  cfgClient->sourceIpAddress = L7_NULL;
  for (i=0; i < sizeof(cfgClient->radiusServerSecret); i++)
  {
    cfgClient->radiusServerSecret[i] = L7_NULL;
  }

  for (i = L7_NULL; (i < L7_RADIUS_MAX_SERVERS); i++)
  {
    L7_uint32 j;
    cfgClient->serverConfigEntries[i].radiusServerConfigIpAddress = L7_NULL;
    cfgClient->serverConfigEntries[i].radiusServerDnsHostName.hostAddrType =
              L7_IP_ADDRESS_TYPE_UNKNOWN;
    memset(cfgClient->serverConfigEntries[i].radiusServerDnsHostName.host.hostName,0, L7_DNS_HOST_NAME_LEN_MAX);

    cfgClient->serverConfigEntries[i].radiusServerConfigUdpPort = FD_RADIUS_ACCT_PORT;
    cfgClient->serverConfigEntries[i].radiusServerConfigServerType = RADIUS_SERVER_TYPE_UNKNOWN;
    cfgClient->serverConfigEntries[i].usageType = FD_RADIUS_USAGE;
    cfgClient->serverConfigEntries[i].oldUsageType = FD_RADIUS_USAGE;
    cfgClient->serverConfigEntries[i].maxNumRetrans = FD_RADIUS_MAX_RETRANS;
    cfgClient->serverConfigEntries[i].timeOutDuration = FD_RADIUS_TIMEOUT_DURATION * RADIUS_TIMER_GRANULARITY;
    cfgClient->serverConfigEntries[i].radiusServerDeadTime = FD_RADIUS_DEADTIME * RADIUS_TIMER_GRANULARITY;
    cfgClient->serverConfigEntries[i].sourceIpAddress = L7_NULL;
    cfgClient->serverConfigEntries[i].localConfigMask = L7_NULL;

    for(j=0; j < sizeof(cfgClient->serverConfigEntries[i].radiusServerConfigSecret); j++)
    {
      cfgClient->serverConfigEntries[i].radiusServerConfigSecret[j] = L7_NULL;
    }

    cfgClient->serverConfigEntries[i].radiusServerIsSecretConfigured = L7_FALSE;
    cfgClient->serverConfigEntries[i].radiusServerConfigRowStatus = RADIUS_SERVER_NOTCONFIGURED;
    cfgClient->serverConfigEntries[i].radiusServerConfigServerEntryType = FD_RADIUS_SERVER_ENTRY_TYPE;
    cfgClient->serverConfigEntries[i].radiusServerConfigPriority = FD_RADIUS_PRIORITY;
    cfgClient->serverConfigEntries[i].incMsgAuthMode = FD_RADIUS_INC_MSG_AUTH_MODE;
    memset(cfgClient->authServerNameArray,L7_NULL,sizeof(cfgClient->authServerNameArray));    
    memset(cfgClient->acctServerNameArray,L7_NULL,sizeof(cfgClient->acctServerNameArray));    
  }
  return;
}

/*********************************************************************
*
* @purpose Apply radius config data
*
* @retruns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusApplyConfigData(void)
{
  L7_uint32 i;
  radiusServerConfigEntry_t *configEntry;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_RADIUS_SERVER_CONFIGPARAMS_FLAG_t paramStatus;
/*  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];*/
  /*
  ** Clear the radius client structure and apply the configured values
  */

  bzero((L7_char8 *)&radiusClient, (L7_int32)sizeof(radiusClient));
  memcpy(&radiusClient, &radiusCfg.cfg.radiusClient, sizeof(radiusClient));

  for (i = L7_NULL; i < L7_RADIUS_MAX_SERVERS; i++)
  {
    dnsHost_t hostAddr;

    configEntry = &radiusCfg.cfg.radiusClient.serverConfigEntries[i];

    if (configEntry->radiusServerConfigRowStatus == RADIUS_SERVER_NOTCONFIGURED)
        continue;

    addrType = configEntry->radiusServerDnsHostName.hostAddrType;
    hostAddr.hostAddrType = addrType;
    if(addrType == L7_IP_ADDRESS_TYPE_IPV4)
    {
      hostAddr.host.ipAddr  = configEntry->radiusServerConfigIpAddress;
    }
    else
    {
      strcpy(hostAddr.host.hostName, configEntry->radiusServerDnsHostName.host.hostName);
    }

    if (configEntry->radiusServerConfigServerType == RADIUS_SERVER_TYPE_AUTH)
    {
      /* Add an Auth Server */      
      (void)radiusEventServerHostNameAdd(&hostAddr, RADIUS_SERVER_TYPE_AUTH,radiusClient.authServerNameArray[configEntry->serverNameIndex].serverName);
      (void)radiusEventServerPortNumSet(&hostAddr, configEntry->radiusServerConfigUdpPort);

      if (configEntry->radiusServerIsSecretConfigured == L7_TRUE)
      {
        if (configEntry->localConfigMask & L7_RADIUS_SERVER_CONFIG_SECRET)
        {
          paramStatus = L7_RADIUSLOCAL;
        }
        else
        {
          paramStatus = L7_RADIUSGLOBAL;
        }
        (void)radiusEventServerSharedSecretSet(&hostAddr, configEntry->radiusServerConfigSecret, paramStatus);
      }

      (void)radiusEventServerEntryTypeSet(&hostAddr, configEntry->radiusServerConfigServerEntryType);
      (void)radiusEventServerIncMsgAuthModeSet(&hostAddr, configEntry->incMsgAuthMode);

      (void)radiusEventServerPrioritySet(&hostAddr, configEntry->radiusServerConfigPriority);
      if (configEntry->localConfigMask & L7_RADIUS_SERVER_CONFIG_MAX_NUM_RETRANS)
      {
        paramStatus = L7_RADIUSLOCAL;
      }
      else
      {
        paramStatus = L7_RADIUSGLOBAL;
      }
      (void)radiusEventMaxNoOfRetransSet(&hostAddr, configEntry->maxNumRetrans, paramStatus);
      (void)radiusEventServerUsageTypeSet(&hostAddr, configEntry->usageType);
      if (configEntry->localConfigMask & L7_RADIUS_SERVER_CONFIG_TIMEOUT)
      {
        paramStatus = L7_RADIUSLOCAL;
      }
      else
      {
        paramStatus = L7_RADIUSGLOBAL;
      }
      (void)radiusEventTimeOutDurationSet(&hostAddr, configEntry->timeOutDuration / RADIUS_TIMER_GRANULARITY, paramStatus);
      if (configEntry->localConfigMask & L7_RADIUS_SERVER_CONFIG_DEAD_TIME)
      {
        paramStatus = L7_RADIUSLOCAL;
      }
      else
      {
        paramStatus = L7_RADIUSGLOBAL;
      }
      (void)radiusEventDeadtimeSet(&hostAddr, configEntry->radiusServerDeadTime / RADIUS_TIMER_GRANULARITY, paramStatus);
      if (configEntry->localConfigMask & L7_RADIUS_SERVER_CONFIG_SOURCE_IP_ADDRESS)
      {
        paramStatus = L7_RADIUSLOCAL;
      }
      else
      {
        paramStatus = L7_RADIUSGLOBAL;
      }
      (void)radiusEventSourceIPSet(&hostAddr, configEntry->sourceIpAddress, paramStatus);
    }
    else
    {
      /* Add an Acct Server */
      (void)radiusEventServerHostNameAdd(&hostAddr, RADIUS_SERVER_TYPE_ACCT,acctServerNameArray[configEntry->serverNameIndex].serverName);
      (void)radiusEventAccountingServerPortNumSet(&hostAddr, configEntry->radiusServerConfigUdpPort);

      if (configEntry->radiusServerIsSecretConfigured == L7_TRUE)
      {
        (void)radiusEventAccountingServerSharedSecretSet(&hostAddr, configEntry->radiusServerConfigSecret);
      }
    } /* End of else */
  } /* End of for */

  radiusCfg.hdr.dataChanged = L7_FALSE;

  return;
}

/*********************************************************************
*
* @purpose Update RADIUS Client information in the Cfg structure
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusCfgUpdate(void)
{
  L7_uint32 i=0;
  radiusServerEntry_t *serverEntry = radiusServerEntries;
  
  while (serverEntry != L7_NULL)
  {
    radiusServerConfigEntry_t *cfgEntry;

    cfgEntry = &(radiusClient.serverConfigEntries[i]);
    memcpy(cfgEntry, &(serverEntry->serverConfig), sizeof(serverEntry->serverConfig));
    i++;
    serverEntry = serverEntry->nextEntry;
  }
  
  memcpy(&radiusCfg.cfg.radiusClient, &radiusClient, sizeof(radiusClient));
  return;
}


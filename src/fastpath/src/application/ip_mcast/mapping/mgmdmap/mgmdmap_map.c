/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  mgmd_map.c
*
* @purpose   Mgmd map functions
*
* @component Mgmd Mapping Layer
*
* @comments  none
*
* @create    02/06/2002
*
* @author    ramakrishna
*
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"
#include "heap_api.h"
#include "support_api.h"

mgmdMapGblVars_t mgmdMapGblVariables_g;
mgmdMapCB_t     *mgmdMapCB_g;

mgmdDebugCfg_t mgmdDebugCfg;


static void mgmdMapProtoEventChangeProcess(L7_uchar8 familyType,
                                           L7_uint32 event, void *pData);

static void mgmdMapIp4RoutingEventProcess(L7_uint32 intIfNum,
                                          L7_uint32 event, void *pData,
                                          L7_BOOL asyncRespReq,
                                          ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

static void mgmdMapMcastEventProcess(L7_uint32 event, void *pdata,
                                     ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

static void mgmd6MapRoutingEventProcess(L7_uint32 intIfNum, L7_uint32 event,
                                        void *pData, L7_BOOL asyncRespReq,
                                        ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo);

static void mgmdMapUIEventChangeProcess (MCAST_CB_HNDL_t cbHandle,
                                         MGMD_UI_EVENT_TYPE_t event,
                                         void *pData);

static void mgmdMapMcastCommonEventProcess(mgmdMapCB_t *mgmdMapCbPtr,
                                           L7_uint32 event);

static L7_RC_t mgmdIntfCreate(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum);

static L7_RC_t mgmdIntfDelete(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum);

static L7_RC_t mgmdResetCleanUp(mgmdMapCB_t *mgmdMapCbPtr);

static void mgmdMapStartupTimerExpireCallback(L7_uint32 familyType, L7_uint32 arg2);

/*********************************************************************
*
* @purpose task to handle all MGMD Mapping messages
*
* @param    none
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void mgmdMapTask()
{
  mgmdMapMsg_t    msg;
  mgmdMapPktMsg_t pktMsg;
  mgmdAppTimerMsg_t appTimerMsg;
  L7_RC_t           status = L7_FAILURE;
  L7_uint32       QIndex=0;

  osapiTaskInitDone(L7_MGMD_MAP_TASK_SYNC);

  mgmdMapGblVariables_g.mgmdQueue[MGMD_APP_TIMER_Q].QRecvBuffer =&appTimerMsg;
  mgmdMapGblVariables_g.mgmdQueue[MGMD_EVENT_Q].QRecvBuffer     =&msg;
  mgmdMapGblVariables_g.mgmdQueue[MGMD_CTRL_PKT_Q].QRecvBuffer  =&pktMsg;


  do
  {

    /* Since we are reading from multiple queues, we cannot wait forever
    * on the message receive from each queue. Rather than sleep between
    * queue reads, use a semaphore to indicate whether any queue has
    * data. Wait until data is available. */
    if (osapiSemaTake(mgmdMapGblVariables_g.mgmdMapMsgQSema, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failure taking MGMDMAP message queue semaphore.");
      continue;
    }


    for(QIndex=0;QIndex<MGMD_MAX_Q;QIndex++)
    {

       status = osapiMessageReceive(mgmdMapGblVariables_g.mgmdQueue[QIndex].QPointer,
                                 mgmdMapGblVariables_g.mgmdQueue[QIndex].QRecvBuffer,
                                 mgmdMapGblVariables_g.mgmdQueue[QIndex].QSize,
                                 L7_NO_WAIT);
       if(status == L7_SUCCESS)
       {
          break;
       }
    }

    if(status == L7_SUCCESS)
    {
      mgmdMapGblVariables_g.mgmdQueue[QIndex].QCurrentMsgCnt--;
      mgmdMapGblVariables_g.mgmdQueue[QIndex].QRxSuccess++;


      switch(QIndex)
      {
        case MGMD_APP_TIMER_Q:

          mgmdMapProtoEventChangeProcess(L7_NULL,
                                         appTimerMsg.event,
                                         (void *)(appTimerMsg.timerCtrlBlk));
          break;
        case MGMD_EVENT_Q:
        {
           switch (msg.msgId)
           {
             case (MGMDMAP_CNFGR_MSG):
               mgmdCnfgrCommandProcess(&msg.u.cnfgrCmdData);
               break;

             case (MGMDMAP_ROUTING_EVENT_MSG):
               /* Processing Events from IpMap*/
               mgmdMapIp4RoutingEventProcess(msg.u.mgmdMapRoutingEventParms.intIfNum,
                                   msg.u.mgmdMapRoutingEventParms.event,
                                   msg.u.mgmdMapRoutingEventParms.pData,
                                   msg.u.mgmdMapRoutingEventParms.asyncResponseRequired,
                                   &(msg.u.mgmdMapRoutingEventParms.eventInfo));
               break;
             case (MGMDMAP_MCAST_EVENT_MSG):
               mgmdMapMcastEventProcess(msg.u.mgmdMapRoutingEventParms.event,
                                        msg.u.mgmdMapRoutingEventParms.pData,
                                        &(msg.u.mgmdMapRoutingEventParms.eventInfo));
               break;

             case (MGMDMAP_RTR6_EVENT_MSG):
               /* Processing Events from Ip6Map*/
               mgmd6MapRoutingEventProcess(msg.u.mgmdMapRoutingEventParms.intIfNum,
                                   msg.u.mgmdMapRoutingEventParms.event,
                                   msg.u.mgmdMapRoutingEventParms.pData,
                                   msg.u.mgmdMapRoutingEventParms.asyncResponseRequired,
                                   &(msg.u.mgmdMapRoutingEventParms.eventInfo));
               break;
             case (MGMDMAP_UI_EVENT_MSG):
               /* Processing Events from UI */
               mgmdMapUIEventChangeProcess (msg.u.mgmdMapUiEventParms.cbHandle,
                                            msg.u.mgmdMapUiEventParms.event,
                                            (void *)&(msg.u.mgmdMapUiEventParms));
               break;
             case (MGMDMAP_PROTOCOL_EVENT_MSG):
               /* Processing Events from UI */
               mgmdMapProtoEventChangeProcess(msg.u.mgmdMapProtocolEvenetParms.familyType,
                                              msg.u.mgmdMapProtocolEvenetParms.event,
                                              (void *)&(msg.u.mgmdMapProtocolEvenetParms.u));
               break;
             default:
               MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "invalid message type.\n");
               break;
            }

        }
        break;
        case MGMD_CTRL_PKT_Q:

          mgmdMapProtoEventChangeProcess(pktMsg.familyType,
                                         pktMsg.event,
                                         (void *)&(pktMsg.ctrlPkt));
          break;
       default:
          MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "invalid Queue type.\n");
          break;

      }
    }

  } while (1);
}
/*********************************************************************
* @purpose  Checks if mgmd user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdHasDataChanged(void)
{
  return (mgmdV4HasDataChanged() || mgmdV6HasDataChanged());
}
void mgmdResetdataChanged(void)
{
  mgmdMapCB_t *mgmdMapCbPtr;

  if (mgmdMapCtrlBlockGet(L7_AF_INET, &mgmdMapCbPtr) == L7_SUCCESS)
  {
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  }
  if (mgmdMapCtrlBlockGet(L7_AF_INET6, &mgmdMapCbPtr) == L7_SUCCESS)
  {
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_FALSE;
  }
  return;
}
/*********************************************************************
* @purpose  Checks if Ipv4 mgmd user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL mgmdV4HasDataChanged(void)
{
  mgmdMapCB_t    *mgmdMapCbPtr = L7_NULLPTR;

  (void)mgmdMapCtrlBlockGet(L7_AF_INET, &mgmdMapCbPtr);

  return (mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged);
}

/*********************************************************************
* @purpose  Saves mgmd user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdSave(void)
{
  L7_RC_t      mgmdRC = L7_FAILURE;
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  protoMax =  mgmdMapProtocolsMaxGet();

  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0];
                       cbIndex < protoMax; cbIndex++, mgmdMapCbPtr++)
  {
    if (mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged == L7_TRUE)
    {
      mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_FALSE;
      mgmdMapCbPtr->pMgmdMapCfgData->checkSum =
               nvStoreCrc32((L7_char8 *)mgmdMapCbPtr->pMgmdMapCfgData,
               sizeof(L7_mgmdMapCfg_t) -
               sizeof(mgmdMapCbPtr->pMgmdMapCfgData->checkSum));
      mgmdRC = sysapiCfgFileWrite(L7_FLEX_MGMD_MAP_COMPONENT_ID,
                             mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.filename,
                             (L7_char8 *)mgmdMapCbPtr->pMgmdMapCfgData,
                             sizeof(L7_mgmdMapCfg_t));
      if (mgmdRC != L7_SUCCESS)
      {
        L7_LOGF (L7_LOG_SEVERITY_WARNING, L7_FLEX_MGMD_MAP_COMPONENT_ID,
                 "MGMD Config-Save Failed; File - %s"
                 " MGMD current running configuration write to file/memory Failed.",
                 mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.filename);
        return L7_FAILURE;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restores MGMD user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdRestore(void)
{
  L7_RC_t      rc = L7_FAILURE;
  L7_uint32    cbIndex = L7_NULL;
  L7_uint32    protoMax = L7_NULL;
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  protoMax =  mgmdMapProtocolsMaxGet();

  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0];
                       cbIndex < protoMax; cbIndex++, mgmdMapCbPtr++)
  {
    if (mgmdResetCleanUp(mgmdMapCbPtr) == L7_SUCCESS)
    {
      mgmdCommonBuildDefaultConfigData (mgmdMapCbPtr, L7_MGMD_CFG_VER_CURRENT);
      mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;
      rc = mgmdApplyConfigData(mgmdMapCbPtr);
    }
    else
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                      "Failed to reset MGMD-Recommend resetting device\n");
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Clean up MGMD as part of restoring to default configuration.
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @comments This routine performs a partial clean up of the MGMD stack.
*           It removes optional configuration parameters such interfaces
            on which mgmd configuration is enabled , etc.
*
* @end
*********************************************************************/
L7_RC_t mgmdResetCleanUp(mgmdMapCB_t *mgmdMapCbPtr)
{
  L7_uint32     index = L7_NULL;
  L7_uint32     intfType;

  /*------------------------------------------------*/
  /* Clean up all interfaces  configuration, if any */
  /*------------------------------------------------*/
  /* examine each entry in the intIfNum <-> config data table **
  **  mapping array for used entries                          */
  for (index=1; index < L7_MAX_INTERFACE_COUNT; index++)
  {
    /* if mapping table entry is not 0; the contents of the location **
    ** is the index into the config table for a created interface    */
    if (mgmdMapCbPtr->pMgmdMapCfgMapTbl[index] != 0)
    {
      /* if the config data shows that the interface is enabled, disable it */
      if (mgmdMapCbPtr->
       pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->pMgmdMapCfgMapTbl[index]].
                           adminMode == L7_ENABLE)
      {
        intfType = mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[mgmdMapCbPtr->pMgmdMapCfgMapTbl[index]].interfaceMode;
        if (mgmdMapIntfModeDisable(mgmdMapCbPtr, index, intfType) != L7_SUCCESS )
        {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error in applying interface disable\n");
        }
      }
    }
  }

  /*--------------------------------------------------*/
  /* Lastly, disable MGMD if it is currently enabled  */
  /*--------------------------------------------------*/
  if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode != L7_DISABLE)
  {
    (void)mgmdMapExtenMgmdAdminModeSet(mgmdMapCbPtr, L7_DISABLE, L7_TRUE);
    mgmdMapCbPtr->pMgmdMapCfgData->adminMode = L7_DISABLE;
  }

  /* De-Initialize the Proxy protocol's Memory & Control Block as
     the MGMD memory is anyway freed during above call
   */

   mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCbPtr);

  /* Return success if MGMD has not been initialized.
   This allows for dynamic configurability of MGMD. */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default mgmd config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmdBuildDefaultConfigData(L7_uint32 ver)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(L7_AF_INET, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get Control Block\n");
    return;
  }
  mgmdCommonBuildDefaultConfigData (mgmdMapCbPtr, ver);
  return;
}
/*********************************************************************
* @purpose  Build default mgmd6 config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmd6BuildDefaultConfigData(L7_uint32 ver)
{
  mgmdMapCB_t *mgmd6MapCbPtr = L7_NULLPTR;

  if (mgmdMapCtrlBlockGet(L7_AF_INET6, &mgmd6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get Control Block\n");
    return;
  }
  mgmdCommonBuildDefaultConfigData (mgmd6MapCbPtr, ver);
  return;
}
/*********************************************************************
* @purpose  Build default mgmd config data
*
* @param    ver             @b{(input)} Software version of Config Data
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmdCommonBuildDefaultConfigData(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 ver)
{
  L7_uint32 index;

  if (mgmdMapCbPtr->pMgmdMapCfgData != L7_NULL)
  {
    /* Build Config File Header */
    memset(( void * )&mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr,
           0, sizeof( L7_fileHdr_t));
    if (mgmdMapCbPtr->familyType == L7_AF_INET)
    {
    strcpy(mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.filename,
                                                 L7_IGMP_CFG_FILENAME);
    }
    else
    {
    strcpy(mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.filename,
                                                 L7_MLD_CFG_FILENAME);
    }
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.version       = ver;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.componentID   =
                                                 L7_FLEX_MGMD_MAP_COMPONENT_ID;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.type          = L7_CFG_DATA;
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.length        =
                                                 sizeof(L7_mgmdMapCfg_t);
    mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged   = L7_FALSE;

    /*---------------------------*/
    /* Build Default Config Data */
    /*---------------------------*/
    mgmdMapCbPtr->pMgmdMapCfgData->adminMode             = (mgmdMapCbPtr->familyType == L7_AF_INET) ?
                                                 FD_IGMP_DEFAULT_ADMIN_MODE : FD_MLD_DEFAULT_ADMIN_MODE;
    mgmdMapCbPtr->pMgmdMapCfgData->checkRtrAlert =  (mgmdMapCbPtr->familyType == L7_AF_INET) ?
                                                 FD_IGMP_DEFAULT_ROUTER_ALERT_CHECK : FD_MLD_DEFAULT_ROUTER_ALERT_CHECK;


    /* we are preserving any previous configId fields in the table */
    for (index=1; index < L7_IPMAP_INTF_MAX_COUNT; index++)
    {
      if (mgmdIntfBuildDefaultConfigData(mgmdMapCbPtr, &(mgmdMapCbPtr->
                              pMgmdMapCfgData->mgmdIntf[index])) != L7_SUCCESS)
      {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed building MGMD default config record for interface\n");
      }
    }
    /* mgmdMapCbPtr->pMgmdMapCfgData->mgmd_proxy_UpStreamDisable = L7_FALSE;    */
    mgmdMapCbPtr->pMgmdMapCfgData->checkSum = 0;
  }
  return;
  /* End of MGMD Component's Factory Defaults */
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t mgmdIntfCreate(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  mgmdIntfCfgData_t *pCfg;
  L7_RC_t mgmdRC = L7_FAILURE;
  L7_uint32 index = L7_NULL;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "nimConfigIdGet failed for intf %d\n", intIfNum);
    return(mgmdRC);
  }

  pCfg = L7_NULLPTR;

  /* scan for any preexisting config records that match this configId;
   * if found, update the config mapping table
   */
  for (index = 1; index < L7_IPMAP_INTF_MAX_COUNT; index++ )
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&mgmdMapCbPtr->pMgmdMapCfgData->
                               mgmdIntf[index].configId, &configId))
    {
      mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum] = index;
      break;
    }
  }
  MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_APIS, "Index is %d for intf %d (1)\n", mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum], intIfNum);


  /* If an interface configuration entry is not already assigned to
   * the interface, assign one
   *
   * Note: mgmdMapIntfIsConfigurable() can return L7_FALSE for two reasons:
   * no matching config table entry was found, or the intIfNum is not valid
   * for MGMD... the above call to mgmdIntfIsValid() assures that if we get
   * L7_FALSE it is due to the first reason and we should try to assign a
   * config table entry for this intf.
   */

  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                &pCfg) == L7_FALSE)
  {
    if (mgmdMapIntfConfigEntryGet(mgmdMapCbPtr, intIfNum,
                                  &pCfg) == L7_TRUE)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_APIS, "Index is %d for intf %d (2)\n", mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum], intIfNum);
      /* successfully assigned a new config table entry for this intf,
       * so initialize the entry to defaults
       */
      /* Update the configuration structure with the config id */
      NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
      /* since this is a newly assigned configuration entry, initialize
       * it to default settings
       */
      mgmdRC = mgmdIntfBuildDefaultConfigData(mgmdMapCbPtr, pCfg);
    }
    else
    {
      /* unsuccessful in assigning config table entry for this intf... **
      ** report failure                                                */
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Unsuccessful in assigning config table entry for intf %d\n", intIfNum);
      mgmdRC = L7_FAILURE;
    }
  }
  else
  {
    /* we found an existing config table entry for this intf */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_APIS, "Index is %d for intf %d (3)\n", mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum], intIfNum);
    mgmdRC = L7_SUCCESS;
  }
  return(mgmdRC);
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t mgmdIntfDelete(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum)
{
  mgmdIntfCfgData_t *pCfg = L7_NULLPTR;
  L7_uint32 currentMcastProtocol = L7_MCAST_IANA_MROUTE_UNASSIGNED;

  /* remove the configuration data for the interface */
  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                &pCfg) == L7_TRUE)
  {
    mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum] = 0;
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
  }
  /* since Proxy setting is not done globally , but interface specific, whenever
     proxy interface is deleted ( instead of disabling) by vlan routing interface
     delete or stack unit is deleted from mananger , the  current mcast protocol
     need to be updated accordingly */
  if (pCfg->interfaceMode != MGMD_PROXY_INTERFACE)
  {
    return L7_SUCCESS;
  }

  if (mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCbPtr) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Memory De-init failed\n");
    return L7_FAILURE;    
  }
  
  /* set the config value */
  pCfg->interfaceMode = MGMD_INTERFACE_MODE_NULL;
  pCfg->adminMode = L7_DISABLE;
  mgmdMapCbPtr->pMgmdMapCfgData->cfgHdr.dataChanged = L7_TRUE;

  if ((mcastMapIpCurrentMcastProtocolGet (mgmdMapCbPtr->familyType, &currentMcastProtocol)
                                       == L7_SUCCESS) &&
                currentMcastProtocol == L7_MCAST_IANA_MROUTE_IGMP_PROXY)
  {
    mcastMapIpCurrentMcastProtocolSet(mgmdMapCbPtr->familyType, L7_MCAST_IANA_MROUTE_UNASSIGNED);
  }
  return(L7_SUCCESS);
}
/*********************************************************************
* @purpose  Applies mgmd config data
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdApplyConfigData(mgmdMapCB_t *mgmdMapCbPtr)
{
  L7_uint32    intIfNum = L7_NULL;
  L7_uint32    cfgIndex = L7_NULL;
  mgmdMapCB_t *mgmdMapCb = mgmdMapCbPtr;

  if (mgmdMapMgmdInitialized(mgmdMapCbPtr) == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_ENABLE)
  {
    /* Check if IP Routing has been enabled */
    if (mcastIpMapRtrAdminModeGet(mgmdMapCbPtr->familyType) == L7_DISABLE)
    {
      return L7_SUCCESS;
    }
    mgmdMapMgmdAdminModeEnable(mgmdMapCbPtr, L7_TRUE);
  }
  else if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_DISABLE)
  {
    mgmdMapMgmdAdminModeDisable(mgmdMapCbPtr, L7_TRUE);
  }

  /* apply any interface configuration data for interfaces that
   * exist at this time
   */
  if (MGMD_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT; cfgIndex++)
    {
      if (nimIntIfFromConfigIDGet(&(mgmdMapCbPtr->
              pMgmdMapCfgData->mgmdIntf[cfgIndex].configId), &intIfNum)
                                                                == L7_SUCCESS)
      {
        mgmdApplyIntfConfigData(mgmdMapCbPtr, intIfNum);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply configuration data to an MGMD Interface
*
* @param    mgmdMapCbPtr    @b{(input)} Mapping Control Block.
* @param    intIfNum        @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdApplyIntfConfigData(mgmdMapCB_t *mgmdMapCbPtr, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  mgmdIntfCfgData_t *intfCfg = L7_NULLPTR;

  if (mgmdMapInterfaceConfigure(intIfNum, mgmdMapCbPtr) == L7_SUCCESS)
  {
    if ((mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                           &intfCfg) == L7_TRUE) && (intfCfg != L7_NULLPTR))
    {
      if (intfCfg->adminMode == L7_ENABLE)
      {
        if (intfCfg->interfaceMode == MGMD_PROXY_INTERFACE)
        {
         if (mgmdCnfgrProxyInitPhase1DynamicProcess(mgmdMapCbPtr) != L7_SUCCESS)
          {
            MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                            "Proxy Vendor Layer Dynamic Init Failed for Family - %d",
                            mgmdMapCbPtr->familyType);
            return L7_FAILURE;
          }
        }
        rc = mgmdMapIntfModeEnable(mgmdMapCbPtr, intIfNum, intfCfg->interfaceMode);
      }
      else if (intfCfg->adminMode == L7_DISABLE)
      {
        rc = mgmdMapIntfModeDisable(mgmdMapCbPtr, intIfNum, intfCfg->interfaceMode);
        if (intfCfg->interfaceMode == MGMD_PROXY_INTERFACE)
        {
          /* De-Initialize the protocol's Memory & Control Block */
          mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCbPtr);
        }
      }
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  Configure MGMD Interface
*
* @param    intIfNum        @b{(input)} Internal Interface Number
* @param    mgmdMapCBPtr    @b{(input)} Mapping Control Block.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapInterfaceConfigure(L7_uint32 intIfNum, mgmdMapCB_t *mgmdMapCbPtr)
{
  L7_uint32 rtrIfNum = L7_NULL;
  mgmdIntfCfgData_t* intfCfg = L7_NULLPTR;

  /* call ipMap to validate that the interface represents a valid **
  ** router interface                                             */
  if (mcastIpMapIntIfNumToRtrIntf(mgmdMapCbPtr->familyType,
                                      intIfNum, &rtrIfNum) != L7_SUCCESS)
  {
    return L7_SUCCESS;
  }

  /* retrieve pointer to MGMD's configuration record for this interface */
  if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum, &intfCfg) == L7_TRUE)
  {
    /* access and apply each parameter in the configuration record **
    ** for this interface                                          */

    mgmdMapInterfaceQueryIntervalSet(mgmdMapCbPtr->familyType, intIfNum,
                    intfCfg->queryInterval);
    mgmdMapInterfaceVersionSet(mgmdMapCbPtr->familyType, intIfNum,
                    intfCfg->version);
    mgmdMapInterfaceQueryMaxResponseTimeSet(mgmdMapCbPtr->familyType,intIfNum,
                  intfCfg->responseInterval);
    mgmdMapInterfaceRobustnessSet(mgmdMapCbPtr->familyType, intIfNum,
                  intfCfg->robustness);
    mgmdMapInterfaceStartupQueryIntervalSet(mgmdMapCbPtr->familyType,intIfNum,
             intfCfg->startupQueryInterval);
    mgmdMapInterfaceStartupQueryCountSet(mgmdMapCbPtr->familyType, intIfNum,
             intfCfg->startupQueryCount);
    mgmdMapInterfaceLastMembQueryIntervalSet(mgmdMapCbPtr->familyType,intIfNum,
             intfCfg->lastMemQueryInterval);
    mgmdMapInterfaceLastMembQueryCountSet(mgmdMapCbPtr->familyType, intIfNum,
             intfCfg->lastMemQueryCount);

    return L7_SUCCESS;
  }
  MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Interface is not valid for MGMD configuration\n");
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Propogate Routing Event notifications to MGMD Map task
*
* @param    intIfNum     @b{(input)} Interface number
* @param    event        @b{(input)} Event type
* @param    pData        @b{(input)} Data
* @param    pEventInfo   @b{(input)} Event Info.
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapRoutingEventCallback(L7_uint32 intIfNum,
                                    L7_uint32 event,
                                    void *pData,
                                    ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  mgmdMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  if (((event != L7_RTR_ENABLE) &&
       (event != L7_RTR_DISABLE_PENDING) &&
       (event != L7_RTR_INTF_CREATE) &&
       (event != L7_RTR_INTF_DELETE) &&
       (event != L7_RTR_INTF_ENABLE) &&
       (event != L7_RTR_STARTUP_DONE) &&
       (event != L7_RTR_INTF_DISABLE_PENDING)) ||
      ((intIfNum != 0) && (mgmdIntfIsValid(intIfNum) != L7_TRUE)))
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", event);

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MGMD;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator = pEventInfo->correlator;
      completionData.handlerId = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(mgmdMapMsg_t));

  msg.msgId = MGMDMAP_ROUTING_EVENT_MSG;

  msg.u.mgmdMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.mgmdMapRoutingEventParms.event = event;
  msg.u.mgmdMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.mgmdMapRoutingEventParms.eventInfo,
           pEventInfo, sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.mgmdMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
  }
  else
  {
    msg.u.mgmdMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
  }

  if(mgmdMessageSend(MGMD_EVENT_Q,&msg) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nMessae Send Failed"
                    "for eventType:%d.\n", event);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMDMAP: Failed to send message.\n");
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MGMD;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator = pEventInfo->correlator;
      completionData.handlerId = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  This is the callback function MGMD registers with IP6 MAP
*           for notification of routing events.
*
* @param    intIfnum    @b{(input)}  internal interface number
* @param    event       @b{(input)}  an event listed in l7_ip6_api.h
* @param    pData       @b{(input)}  unused
* @param    pEventInfo  @b{(input)}  provides parameters used to acknowledge
*                                    processing of the event
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmd6MapRoutingEventCallback (L7_uint32 intIfNum,
                                      L7_uint32 event,
                                      void *pData,
                                      ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  mgmdMapMsg_t msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  if (((event != RTR6_EVENT_ENABLE) &&
       (event != RTR6_EVENT_DISABLE_PENDING) &&
       (event != RTR6_EVENT_INTF_CREATE) &&
       (event != RTR6_EVENT_INTF_DELETE) &&
       (event != RTR6_EVENT_INTF_ENABLE) &&
       (event != RTR6_EVENT_INTF_DISABLE_PENDING) &&
       (event != RTR6_EVENT_INTF_ADDR_ADD) &&
       (event != RTR6_EVENT_STARTUP_DONE)) ||
      ((intIfNum != 0) && (mgmdIntfIsValid(intIfNum) != L7_TRUE)))
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nInput parameters are not valid for eventType:%d.\n", event);
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MGMD;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator = pEventInfo->correlator;
      completionData.handlerId = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  memset(&msg, 0, sizeof(mgmdMapMsg_t));

  msg.msgId = MGMDMAP_RTR6_EVENT_MSG;

  msg.u.mgmdMapRoutingEventParms.intIfNum = intIfNum;
  msg.u.mgmdMapRoutingEventParms.event = event;
  msg.u.mgmdMapRoutingEventParms.pData = pData;

  if (pEventInfo != L7_NULLPTR)
  {
    memcpy(&msg.u.mgmdMapRoutingEventParms.eventInfo,
      pEventInfo, sizeof(ASYNC_EVENT_NOTIFY_INFO_t));
    msg.u.mgmdMapRoutingEventParms.asyncResponseRequired = L7_TRUE;
  }
  else
  {
    msg.u.mgmdMapRoutingEventParms.asyncResponseRequired = L7_FALSE;
  }

  if(mgmdMessageSend(MGMD_EVENT_Q,&msg) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nMessae Send Failed"
                    "for eventType:%d.\n", event);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMDMAP: Failed to send message.\n");
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId = L7_IPRT_MGMD;
    if (pEventInfo != L7_NULLPTR)
    {
      completionData.correlator = pEventInfo->correlator;
      completionData.handlerId = pEventInfo->handlerId;
    }
    completionData.async_rc.rc = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  This routine is called if an interface is enabled for
* @purpose  for routing and process the ipmap related Events.
*
* @param    intIfnum    @b{(input)}  internal interface number
* @param    event       @b{(input)}  an event listed in l7_ip6_api.h
* @param    pData       @b{(input)}  unused
* @param    asyncRespReq @b{(input)} async response request
* @param    pEventInfo  @b{(input)}  provides parameters used to acknowledge
*                                    processing of the event
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmdMapIp4RoutingEventProcess(L7_uint32 intIfNum,
                                L7_uint32 event,
                                void *pData,
                                L7_BOOL asyncRespReq,
                                ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_inet_addr_t ipAddr;
  L7_uint32      glblAdminMode = L7_NULL;
  L7_RC_t        rc = L7_SUCCESS;
  mgmdIntfCfgData_t *pCfg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;
  mgmdMapCB_t    *mgmdMapCbPtr = L7_NULLPTR;

  inetAddressReset(&ipAddr);
  if (mgmdMapCtrlBlockGet(L7_AF_INET, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get control block\n");
    return;
  }

  if (mgmdMapAdminModeGet(mgmdMapCbPtr->familyType, &glblAdminMode) != L7_SUCCESS)
    glblAdminMode = L7_DISABLE;

  if (asyncRespReq == L7_TRUE)
  {
    completionData.componentId = L7_IPRT_MGMD;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc       = L7_SUCCESS;
    completionData.async_rc.reason   = ASYNC_EVENT_REASON_RC_SUCCESS;
  }

  switch (event)
  {
  case L7_RTR_ENABLE:
    if (glblAdminMode == L7_ENABLE)
    {
      rc = mgmdMapMgmdAdminModeEnable(mgmdMapCbPtr, L7_FALSE);
    }
    break;

  case L7_RTR_DISABLE_PENDING :
    if (glblAdminMode == L7_ENABLE)
    {
      rc = mgmdMapMgmdAdminModeDisable(mgmdMapCbPtr, L7_FALSE);
    }
    break;

  case L7_RTR_INTF_CREATE:
    rc = mgmdIntfCreate(mgmdMapCbPtr, intIfNum);
    break;

  case L7_RTR_INTF_DELETE:
    rc = mgmdIntfDelete(mgmdMapCbPtr, intIfNum);
    break;

  case L7_RTR_INTF_DISABLE_PENDING:
    if (glblAdminMode == L7_ENABLE)
    {
      if ((mgmdMapMgmdIntfInitialized(mgmdMapCbPtr->familyType, intIfNum) == L7_SUCCESS) ||
          (mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType, intIfNum) == L7_SUCCESS))
      {
        if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum , &pCfg) == L7_TRUE)
        {
          rc = mgmdMapIntfModeDisable(mgmdMapCbPtr, intIfNum, pCfg->interfaceMode);
        }
        else
        {
          MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nERROR\n");
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Interface not valid for MGMD\n");
          rc = L7_FAILURE;
        }
      }
      if (mgmdMapMgmdProxyIntfInitialized(mgmdMapCbPtr->familyType,
                                          intIfNum) == L7_SUCCESS)
      {
        mgmdMapMgmdProxyIntfDownMRTApply(mgmdMapCbPtr, intIfNum);
      }
    }
    break;

  case L7_RTR_INTF_ENABLE:
    if ((glblAdminMode == L7_ENABLE) ||
        ((mgmdMapProxyInterfaceModeGet(mgmdMapCbPtr->familyType, intIfNum, &glblAdminMode) ==  L7_SUCCESS) &&
         (glblAdminMode == L7_ENABLE)))
    {
      if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum , &pCfg) == L7_TRUE)
      {
        if (mcastIpMapRtrIntfIpAddressGet(mgmdMapCbPtr->familyType, intIfNum,
                                          &ipAddr) == L7_SUCCESS &&
            inetIsAddressZero(&ipAddr) != L7_TRUE)
        {
          rc = mgmdMapIntfModeEnable(mgmdMapCbPtr, intIfNum, pCfg->interfaceMode);
        }
      }
      else
      {
        MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nERROR\n");
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Interface not valid for MGMD\n");
        rc = L7_FAILURE;
      }
    }
    break;

  case L7_RTR_STARTUP_DONE:
    /* signal that all router events for the startup phase have been issued */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS, "mgmd received L7_RTR_STARTUP_DONE event");

    if (mgmdMapCbPtr->warmRestartInProgress == L7_TRUE)
    {
      if (glblAdminMode == L7_ENABLE)
      {
        /* restart was WARM and we are Enabled at config time, schedule timer to
         * measure WARM_RESTART_INTERVAL
         */

        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS,
                        "Starting MGMD Warm Restart Timer");
        osapiTimerAdd(mgmdMapStartupTimerExpireCallback, L7_AF_INET, 0,
                      MGMD_WARM_RESTART_INTERVAL_MSEC,
                      &(mgmdMapCbPtr->mgmdMapStartupTimer));
        if (mgmdMapCbPtr->mgmdMapStartupTimer == L7_NULLPTR)
        {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                          "Null timer pointer from osapiTimerAdd() on attempt to start warm restart timer.");

          /* since we could not add the timer, do the expire processing now */
          mgmdMapStartupTimerExpireCallback(L7_AF_INET, 0);
        }
      }
      else
      {
        /* restart was WARM but we are Disabled so we can declare sync complete now */
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS,
                        "MGMD Disabled, calling mgmdMapStartupTimerExpireCallback() to end warm restart interval");

        /* since we could not add the timer, do the expire processing now */
        mgmdMapStartupTimerExpireCallback(L7_AF_INET, 0);
      }
    }
    else
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS, "  mgmd restart is cold");
    }
    break;

  default:
    rc = L7_SUCCESS;
    break;
  }

  if (asyncRespReq == L7_TRUE)
  {
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return;
}

/*********************************************************************
* @purpose  Process protocol-related events
*
* @param    familyType  @b{(input)} address Family.
* @param    event       @b{(input)} Event.
* @param    pData       @b{(input)} Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdMapProtoEventChangeProcess(L7_uchar8 familyType,
                                    L7_uint32 event, void *pData)
{
  if (mgmd_event_process(familyType, event, pData) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,
                   "MGMD Event Process Failed for event %d.\n", event);
    if (event == MCAST_EVENT_IGMP_CONTROL_PKT_RECV ||
        event == MCAST_EVENT_MLD_CTRL_PKT_RECV)
    {
      mcastCtrlPktBufferPoolFree(familyType,
            ((mcastControlPkt_t *) pData)->payLoad);
    }
    return;
  }
  return;
}

/*********************************************************************
* @purpose  Determine if the interface is valid for MGMD configuration
*           and optionally output a pointer to the configuration structure
*           entry for this interface
*
* @param    mgmdMapCbPtr @b{(input)} Mapping Control Block.
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    pCfg         @b{(input)} Output pointer location,
*                                    or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid interface number to use when referencing the
*           interface config data structure.
*
* @end
*********************************************************************/

L7_BOOL mgmdMapIntfIsConfigurable(mgmdMapCB_t *mgmdMapCbPtr,
              L7_uint32 intIfNum, mgmdIntfCfgData_t **pCfg)
{
  L7_uint32 index = L7_NULL;
  nimConfigID_t configId;
  mgmdMapCB_t *mgmdMapCb = mgmdMapCbPtr;

  if (!(MGMD_IS_READY))
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD not ready\n");
    return L7_FALSE;
  }

  if (mgmdIntfIsValid(intIfNum)!= L7_TRUE)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Intf %d not valid for MGMD\n", intIfNum);
    return L7_FALSE;
  }

  index = mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum];

  if (index == 0)
  {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Config table index 0\n");
      return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches
   * the configId that NIM maps to the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(mgmdMapCbPtr->pMgmdMapCfgData->
                               mgmdIntf[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between
       * pMgmdMapCfgData and pMgmdMapCfgMapTbl or  there is synchronization
       * issue between NIM and components w.r.t. interface creation/deletion
       */
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Error accessing MGMD config data for interface\n");
      return L7_FALSE;
    }
  }
  /*MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "Index->%d\n", index);*/

  if (pCfg != L7_NULLPTR)
  {
    *pCfg = &(mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[index]);
  }
  /*MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_APIS, "*pCfg->%x pCfg->%x\n",
                 (L7_uint32)(*pCfg),(L7_uint32)pCfg);*/

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Retrieve a pointer to an empty configuration data for a given
*           interface if found.
*
* @param    mgmdMapCbPtr  @b{(input)} Mapping Control Block.
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    pCfg          @b{(input)} Output pointer location
*                         @b{(output)} Pointer to MGMD interface config structure
*
* @returns  L7_TRUE     an empty interface config structure was found
* @returns  L7_FALSE    no empty interface config structure entry is available
*
* @comments Does not check for a valid router interface, since that may
*           not get configured until later.  All that is required here
*           is a valid slot.port number to use when referencing the
*           interface config data structure.
*
* @end
*********************************************************************/
L7_BOOL mgmdMapIntfConfigEntryGet(mgmdMapCB_t *mgmdMapCbPtr,
                        L7_uint32 intIfNum, mgmdIntfCfgData_t **pCfg)
{
  L7_uint32 index = L7_NULL;
  nimConfigID_t configIdNull;
  mgmdMapCB_t *mgmdMapCb = mgmdMapCbPtr;

  if (!(MGMD_IS_READY))
  {
    *pCfg = L7_NULLPTR;
    return L7_FALSE;
  }

  memset((void *)&configIdNull, 0, sizeof(nimConfigID_t));

  for (index = 1; index < L7_IPMAP_INTF_MAX_COUNT; index++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&(mgmdMapCbPtr->pMgmdMapCfgData->
                                 mgmdIntf[index].configId), &configIdNull))
    {
      mgmdMapCbPtr->pMgmdMapCfgMapTbl[intIfNum] = index;
      *pCfg = &mgmdMapCbPtr->pMgmdMapCfgData->mgmdIntf[index];
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}
/*********************************************************************
* @purpose  Build default MGMD config data for an intf
*
* @param    mgmdMapCbPtr  @b{(input)} Mapping Control Block.
* @param    pCfg         @b{(output)} pointer to the MGMD
*                                     config data for the interface
*
* @returns  L7_SUCCESS   default configuration applied
* @returns  L7_FAILURE   could not build default config data for interface
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdIntfBuildDefaultConfigData(mgmdMapCB_t *mgmdMapCbPtr,
                                       mgmdIntfCfgData_t *pCfg)
{
  L7_RC_t rc = L7_FAILURE;

  if (pCfg != L7_NULLPTR)
  {
    pCfg->adminMode            = L7_DISABLE;
    pCfg->queryInterval        = FD_IGMP_QUERY_INTERVAL;
    pCfg->robustness           = FD_IGMP_ROBUST_VARIABLE;
    pCfg->startupQueryInterval = FD_IGMP_STARTUP_QUERY_INTERVAL;
    pCfg->startupQueryCount    = FD_IGMP_STARTUP_QUERY_COUNT;
    pCfg->lastMemQueryCount    = FD_IGMP_LAST_MEMBER_QUERY_COUNT;
    pCfg->entryStatus          = FD_IGMP_ENTRY_STATUS;
    pCfg->unsolicitedReportInterval
                               = FD_IGMP_DEFAULT_UNSOLICITED_REPORT_INTERVAL ;
    pCfg->interfaceMode        = MGMD_INTERFACE_MODE_NULL;
    if (mgmdMapCbPtr->familyType == L7_AF_INET)
    {
      pCfg->lastMemQueryInterval = FD_IGMP_LAST_MEMBER_QUERY_INTERVAL;
      pCfg->responseInterval     = FD_IGMP_QUERY_RESPONSE_INTERVAL;
      pCfg->version              = FD_IGMP_DEFAULT_VER;
    }
    else
    {
      pCfg->lastMemQueryInterval = FD_MLD_LAST_MEMBER_QUERY_INTERVAL;
      pCfg->responseInterval     = FD_MLD_QUERY_RESPONSE_INTERVAL;
      pCfg->version              = FD_MLD_DEFAULT_VERSION;
    }
    rc = L7_SUCCESS;
  }
  return rc;
}
/*********************************************************************
* @purpose  Propogate MCAST Event notifications to Mgmd Map task
*
* @param    intIfNum    @b{(input)} Interface number
* @param    event       @b{(input)} Event type
* @param    pData       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapMcastEventCallback(L7_uint32 event,
                                  void *pData,
                                  ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  mgmdMapMsg_t                msg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  if ((event != MCAST_RTR_ADMIN_MODE_ENABLED) &&
      (event != MCAST_RTR_ADMIN_MODE_DISABLE_PENDING))
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nInput parameters are not"
                    "valid for eventType:%d.\n", event);
    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId     = L7_MRP_MGMD_PROXY;
    completionData.correlator      = pEventInfo->correlator;
    completionData.handlerId       = pEventInfo->handlerId;
    completionData.async_rc.rc     = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  memset(&msg, 0, sizeof(mgmdMapMsg_t));

  msg.msgId = MGMDMAP_MCAST_EVENT_MSG;

  msg.u.mgmdMapRoutingEventParms.event = event;
  msg.u.mgmdMapRoutingEventParms.pData = pData;
  memcpy(&msg.u.mgmdMapRoutingEventParms.eventInfo,
      pEventInfo, sizeof(ASYNC_EVENT_NOTIFY_INFO_t));


  if(mgmdMessageSend(MGMD_EVENT_Q,&msg) != L7_SUCCESS)
  {
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nMessae Send Failed"
                    "for eventType:%d.\n", event);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMDMAP: Failed to send message.\n");

    memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
    completionData.componentId     = L7_MRP_MGMD_PROXY;
    completionData.correlator      = pEventInfo->correlator;
    completionData.handlerId       = pEventInfo->handlerId;
    completionData.async_rc.rc     = L7_SUCCESS;
    completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Mcast related Events.
*
* @param    mgmdMapCbPtr @b{(input)} Mapping Control Block.
* @param    intIfNum     @b{(input)} Internal Interface Number
* @param    event        @b{(input)} Event On the Interface
*
* @returns  None
*
* @comments none
*
* @end
*********************************************************************/
void mgmdMapMcastCommonEventProcess(mgmdMapCB_t *mgmdMapCbPtr,
                                    L7_uint32 event)
{
  L7_RC_t           rc = L7_SUCCESS;

  switch (event)
  {
  case MCAST_RTR_ADMIN_MODE_ENABLED:
    if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_ENABLE)
    {
      rc = mgmdMapMgmdAdminModeEnable(mgmdMapCbPtr, L7_FALSE);
      if (rc != L7_SUCCESS)
      {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "mgmdMapMgmdAdminModeEnable Failed for event %d.\n", event);
      }
    }
    break;

  case MCAST_RTR_ADMIN_MODE_ENABLE_PENDING:
    break;

  case MCAST_RTR_ADMIN_MODE_DISABLE_PENDING:
    if (mgmdMapCbPtr->pMgmdMapCfgData->adminMode == L7_ENABLE)
    {
      rc = mgmdMapMgmdAdminModeDisable(mgmdMapCbPtr, L7_FALSE);
      if (rc != L7_SUCCESS)
      {
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "mgmdMapMgmdAdminModeDisable Failed for event %d.\n", event);
      }
    }
    break;

  case MCAST_RTR_ADMIN_MODE_DISABLED:
    break;

  default:
    rc = L7_FAILURE;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid event\n");
    break;
  }
  return;
}

/**************************************************************
*
* @purpose  This routine is called when a mcast event occurs
*           that may change the operational state of the
*           multicast routing.
*
* @param    intIfNum    @b{(input)} internal interface number
* @param    event       @b{(input)} event ID
* @param    pdata       @b{(input)} Data
* @param    pEventInfo  @b{(input)} Event Info.
*
* @returns  void
*
* @comments None
*
* @end
*
***************************************************************/
void mgmdMapMcastEventProcess(L7_uint32 event,
                     void *pdata, ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_uint32                   cbIndex = L7_NULL;
  L7_uint32                   protoMax = L7_NULL;
  mgmdMapCB_t                 *mgmdMapCbPtr = L7_NULLPTR;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;

  protoMax =  mgmdMapProtocolsMaxGet();

  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0];
                       cbIndex < protoMax; cbIndex++, mgmdMapCbPtr++)
  {
    /* process Mcast Events. */
    mgmdMapMcastCommonEventProcess(mgmdMapCbPtr, event);
  }

  memset(&completionData, 0, sizeof(ASYNC_EVENT_COMPLETE_INFO_t));
  completionData.componentId     = L7_MRP_MGMD_PROXY;
  completionData.correlator      = pEventInfo->correlator;
  completionData.handlerId       = pEventInfo->handlerId;
  completionData.async_rc.rc     = L7_SUCCESS;
  completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
  /* Return event completion status  */
  asyncEventCompleteTally(&completionData);

  return;
}
/*********************************************************************
* @purpose  This routine is called if an interface is enabled for
*           for routing. This routine process Ip6Map Events.
*
* @param    intIfnum     @b{(input)} Internal interface number
* @param    event        @b{(input)} Event listed in L7_RTR_EVENT_CHANGE_t
* @param    pdata        @b{(input)} Data
* @param    asyncRespReq @b{(input)} Async Response Request
* @param    pEventInfo   @b{(input)} Event Info.
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void mgmd6MapRoutingEventProcess(L7_uint32 intIfNum,
                                L7_uint32 event,
                                void *pData,
                                L7_BOOL asyncRespReq,
                                ASYNC_EVENT_NOTIFY_INFO_t *pEventInfo)
{
  L7_inet_addr_t ipAddr;
  L7_uint32      mode = L7_NULL, rtrIfNum = 0;
  L7_RC_t        rc = L7_SUCCESS;
  mgmdIntfCfgData_t *pCfg;
  ASYNC_EVENT_COMPLETE_INFO_t completionData;
  mgmdMapCB_t    *mgmd6MapCbPtr = L7_NULLPTR;

  inetAddressReset(&ipAddr);
  if (mgmdMapCtrlBlockGet(L7_AF_INET6, &mgmd6MapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed in getting Control Block.\n");
    return;
  }
  if (asyncRespReq == L7_TRUE)
  {
    completionData.componentId = L7_IPRT_MGMD;
    completionData.correlator  = pEventInfo->correlator;
    completionData.handlerId   = pEventInfo->handlerId;
    completionData.async_rc.rc       = L7_SUCCESS;
    completionData.async_rc.reason   = ASYNC_EVENT_REASON_RC_SUCCESS;
  }
  switch (event)
  {
    case RTR6_EVENT_ENABLE:
      if (mgmdMapAdminModeGet(mgmd6MapCbPtr->familyType,
                              &mode) == L7_SUCCESS && mode == L7_ENABLE)
      {
        rc = mgmdMapMgmdAdminModeEnable(mgmd6MapCbPtr, L7_FALSE);
      }
      break;

    case RTR6_EVENT_DISABLE:
      /* Do Nothing */
      break;

    case RTR6_EVENT_DISABLE_PENDING:
      if ((mgmdMapAdminModeGet(mgmd6MapCbPtr->familyType, &mode) == L7_SUCCESS) &&
          mode == L7_ENABLE)
      {
        rc = mgmdMapMgmdAdminModeDisable(mgmd6MapCbPtr, L7_FALSE);
      }
      break;

    case RTR6_EVENT_INTF_CREATE:
      rc = mgmdIntfCreate(mgmd6MapCbPtr, intIfNum);
      break;

    case RTR6_EVENT_INTF_DELETE:
      rc = mgmdIntfDelete(mgmd6MapCbPtr, intIfNum);
      break;


  case RTR6_EVENT_INTF_DISABLE_PENDING:
    if (mgmdMapAdminModeGet(mgmd6MapCbPtr->familyType,
                            &mode) == L7_SUCCESS && mode == L7_ENABLE)
    {
        if ((mgmdMapProxyChkIsEnabled(mgmd6MapCbPtr->familyType) == L7_TRUE) &&
            (mcastIpMapIntIfNumToRtrIntf(mgmd6MapCbPtr->familyType,
                                        intIfNum, &rtrIfNum) == L7_SUCCESS))
        {
          mgmd_proxy_MfcIntfEventQueue(mgmd6MapCbPtr->familyType, rtrIfNum , L7_DISABLE);
        }
        if ((mgmdMapMgmdIntfInitialized(mgmd6MapCbPtr->familyType,
             intIfNum) == L7_SUCCESS) ||
            (mgmdMapMgmdProxyIntfInitialized(mgmd6MapCbPtr->familyType,
             intIfNum) == L7_SUCCESS))
        {
          if ((mgmdMapIntfIsConfigurable(mgmd6MapCbPtr, intIfNum,
                             &pCfg) == L7_TRUE) && (pCfg != L7_NULLPTR))
          {
            rc = mgmdMapIntfModeDisable(mgmd6MapCbPtr, intIfNum, pCfg->interfaceMode);
          }
        }
    }
      break;

  case RTR6_EVENT_INTF_ENABLE:
    if (mgmdMapAdminModeGet(mgmd6MapCbPtr->familyType,
                            &mode) == L7_SUCCESS && mode == L7_ENABLE)
    {
      if ((mgmdMapProxyChkIsEnabled(mgmd6MapCbPtr->familyType) == L7_TRUE) &&
            (mcastIpMapIntIfNumToRtrIntf(mgmd6MapCbPtr->familyType,
                                        intIfNum, &rtrIfNum) == L7_SUCCESS))
      {
        mgmd_proxy_MfcIntfEventQueue(mgmd6MapCbPtr->familyType, rtrIfNum , L7_ENABLE);
      }
      if (((mgmdMapInterfaceModeGet(mgmd6MapCbPtr->familyType, intIfNum,
          &mode) == L7_SUCCESS)
          && mode == L7_ENABLE) || ((mgmdMapProxyInterfaceModeGet(
          mgmd6MapCbPtr->familyType, intIfNum, &mode) == L7_SUCCESS) &&
          mode == L7_ENABLE))
      {
        if (mcastIpMapRtrIntfIpAddressGet(mgmd6MapCbPtr->familyType, intIfNum,
            &ipAddr) == L7_SUCCESS && inetIsAddressZero(&ipAddr) != L7_TRUE)
        {
          if (mgmdMapIntfIsConfigurable(mgmd6MapCbPtr, intIfNum , &pCfg) == L7_TRUE)
          {
            rc = mgmdMapIntfModeEnable(mgmd6MapCbPtr, intIfNum, pCfg->interfaceMode);
          }
          else
          {
            MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES, "\nERROR\n");
            MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Interface not valid for MGMD\n");
            rc = L7_FAILURE;
          }
        }
      }
    }
    break;

  case RTR6_EVENT_INTF_ADDR_ADD:
    if (mgmdMapAdminModeGet(mgmd6MapCbPtr->familyType,
                            &mode) == L7_SUCCESS && mode == L7_ENABLE)
    {
      if (mcastIpMapRtrIntfIpAddressGet(mgmd6MapCbPtr->familyType, intIfNum,
          &ipAddr) == L7_SUCCESS && inetIsAddressZero(&ipAddr) != L7_TRUE)
      {
        if ((mgmdMapInterfaceModeGet(mgmd6MapCbPtr->familyType, intIfNum,
             &mode) == L7_SUCCESS) && mode == L7_ENABLE)
        {
          rc = mgmdMapIntfModeEnable(mgmd6MapCbPtr, intIfNum, MGMD_ROUTER_INTERFACE);
        }
        else if ((mgmdMapProxyInterfaceModeGet(mgmd6MapCbPtr->familyType, intIfNum,
                  &mode) == L7_SUCCESS) && mode == L7_ENABLE)
        {
          rc = mgmdMapIntfModeEnable(mgmd6MapCbPtr, intIfNum, MGMD_PROXY_INTERFACE);
        }
      }
    }
    break;

  case RTR6_EVENT_STARTUP_DONE:
    /* signal that all router events for the startup phase have been issued */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS, "mgmd received RTR6_EVENT_STARTUP_DONE event");

    if (mgmd6MapCbPtr->warmRestartInProgress == L7_TRUE)
    {
      if ((mgmdMapAdminModeGet(mgmd6MapCbPtr->familyType, &mode) == L7_SUCCESS) &&
          mode == L7_ENABLE)
      {
        /* restart was WARM and we are Enabled at config time, schedule timer to
         * measure WARM_RESTART_INTERVAL
         */

        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS,
                        "Starting MGMD6 Warm Restart Timer");
        osapiTimerAdd(mgmdMapStartupTimerExpireCallback, L7_AF_INET6, 0,
                      MGMD_WARM_RESTART_INTERVAL_MSEC,
                      &(mgmd6MapCbPtr->mgmdMapStartupTimer));
        if (mgmd6MapCbPtr->mgmdMapStartupTimer == L7_NULLPTR)
        {
          MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                          "Null timer pointer from osapiTimerAdd() on attempt to start warm restart timer.");

          /* since we could not add the timer, do the expire processing now */
          mgmdMapStartupTimerExpireCallback(L7_AF_INET6, 0);
        }
      }
      else
      {
        /* restart was WARM but we are Disabled so we can declare sync complete now */
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS,
                        "MGMD6 Disabled, calling mgmdMapStartupTimerExpireCallback() to end warm restart interval");

        /* since we could not add the timer, do the expire processing now */
        mgmdMapStartupTimerExpireCallback(L7_AF_INET6, 0);
      }
    }
    else
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS, "  mgmd6 restart is cold");
    }
    break;



  default:
    rc = L7_SUCCESS;
    break;
  }

  if (asyncRespReq == L7_TRUE)
  {
    completionData.async_rc.rc = rc;
    if (rc == L7_SUCCESS)
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_SUCCESS;
    }
    else
    {
      completionData.async_rc.reason = ASYNC_EVENT_REASON_RC_REGISTRANT_FAILURE;
    }
    /* Return event completion status  */
    asyncEventCompleteTally(&completionData);
  }
  return;
}

/*********************************************************************
* @purpose  Process UI events
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
void mgmdMapUIEventChangeProcess (MCAST_CB_HNDL_t cbHandle,
                                  MGMD_UI_EVENT_TYPE_t event,
                                  void *pData)
{
  mgmdMapCB_t *mgmdMapCbPtr = L7_NULLPTR;
  mgmdMapUIEventParms_t *uiEvent = (mgmdMapUIEventParms_t*) pData;

  if ((mgmdMapCbPtr = (mgmdMapCB_t*) cbHandle) == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD MAP Control Block Handle is Invalid.\n");
    return;
  }

  if (uiEvent == L7_NULLPTR)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD MAP Dequeued Buffer is NULL for Event - %d.\n", event);
    return;
  }

  switch (event)
  {
    case MGMD_ADMIN_MODE_SET:
    {
      L7_uint32 adminMode = uiEvent->eventInfo;

      if (adminMode == L7_ENABLE)
      {
        mgmdMapMgmdAdminModeEnable (mgmdMapCbPtr, L7_TRUE);
      }
      else if (adminMode == L7_DISABLE)
      {
        mgmdMapMgmdAdminModeDisable (mgmdMapCbPtr, L7_TRUE);
      }
      break;
    }
    case MGMD_ROUTER_ALERT_CHECK_SET:
      {
        L7_BOOL rtrAlertCheck = uiEvent->eventInfo;
        mgmdMapMgmdRouterAlertCheckApply(mgmdMapCbPtr, rtrAlertCheck);
      }
      break;
    case MGMD_ROBUSTNESS_SET:
      {
        L7_uint32 robustness = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfRobustnessApply(mgmdMapCbPtr, intIfNum, robustness);
      }
      break;
    case MGMD_QUERY_INTERVAL_SET:
      {
        L7_BOOL queryInterval = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfQueryIntervalApply(mgmdMapCbPtr, intIfNum, queryInterval);
      }
      break;
    case MGMD_QUERY_MAX_RESPONSE_TIME_SET:
      {
        L7_BOOL queryMaxRespTime = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfQueryMaxRespTimeApply(mgmdMapCbPtr, intIfNum, queryMaxRespTime);
      }
      break;
    case MGMD_STARTUP_QUERY_INTERVAL_SET:
      {
        L7_BOOL startupQueryInterval = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfStartupQueryIntervalApply(mgmdMapCbPtr, intIfNum, startupQueryInterval);
      }
      break;
    case MGMD_STARTUP_QUERY_COUNT_SET:
      {
        L7_BOOL startupQueryCount = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfStartupQueryCountApply(mgmdMapCbPtr, intIfNum, startupQueryCount);
      }
      break;
    case MGMD_LASTMEMBER_QUERY_INTERVAL_SET:
      {
        L7_BOOL lastMemQueryInterval = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfLastMemberQueryIntervalApply(mgmdMapCbPtr, intIfNum, lastMemQueryInterval);
      }
      break;
    case MGMD_LASTMEMBER_QUERY_COUNT_SET:
      {
        L7_BOOL lastMemQueryCount = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfLastMemberQueryCountApply(mgmdMapCbPtr, intIfNum, lastMemQueryCount);
      }
      break;
    case MGMD_UNSOLICITED_REPORT_INTERVAL_SET:
      {
        L7_BOOL unsolicitedReportInterval = uiEvent->eventInfo;
        L7_uint32 intIfNum = uiEvent->intIfNum;
        mgmdMapIntfUnsolicitedReportIntervalApply(mgmdMapCbPtr, intIfNum, unsolicitedReportInterval);
      }
      break;

    case MGMD_INTF_MODE_SET:
    {
      L7_uint32 intIfNum      = uiEvent->intIfNum;
      L7_uint32 intfAdminMode = uiEvent->eventInfo;
      mgmdIntfCfgData_t *intfCfg = L7_NULLPTR;
      L7_uint32 mgmdAdminMode;

      if (mgmdMapIntfIsConfigurable(mgmdMapCbPtr, intIfNum,
                                    &intfCfg) != L7_TRUE)
      {
        /* ERROR: Operation failed. */
        MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to identify if the interface \
                 is valid for MGMD configuration\n");
        return;
      }

      if (intfAdminMode == L7_ENABLE)
      {
        if (uiEvent->intfType == MGMD_PROXY_INTERFACE)
        {
          if (mgmdCnfgrProxyInitPhase1DynamicProcess(mgmdMapCbPtr) != L7_SUCCESS)
          {
            MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                            "Proxy Vendor Layer Dynamic Init Failed for Family - %d",
                            mgmdMapCbPtr->familyType);
            return ;
          }
        }
        if ((mgmdMapAdminModeGet (mgmdMapCbPtr->familyType, &mgmdAdminMode)
                                        == L7_SUCCESS) &&
               (mgmdAdminMode == L7_ENABLE))
        {
          mgmdMapIntfModeEnable (mgmdMapCbPtr, intIfNum, uiEvent->intfType);
        }
      }
      else if (intfAdminMode == L7_DISABLE)
      {
        mgmdMapIntfModeDisable (mgmdMapCbPtr, intIfNum, uiEvent->intfType);
        if (uiEvent->intfType == MGMD_PROXY_INTERFACE)
        {
          /* De-Initialize the protocol's Memory & Control Block */
          mgmdCnfgrProxyFiniPhase1DynamicProcess (mgmdMapCbPtr);
        }
      }

      break;
    }

    case MGMD_VERSION_SET:
      mgmdMapIntfVersionApply(mgmdMapCbPtr,uiEvent->intIfNum, uiEvent->eventInfo);
      break;


    default:
      break;
  }

  return;
}

/*********************************************************************
* @purpose  Send event to MGMD Vendor
*
* @param    familyType    @b{(input)}  Address Family type
* @param    eventType     @b{(input)}  Event Type
* @param    msgLen        @b{(input)}  Message Length
* @param    eventMsg      @b{(input)}  Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapComponentCallback(L7_uchar8 familyType,
         L7_uint32  eventType, L7_uint32  msgLen, void *eventMsg)
{

  /*NOTE: Don't check for mgmd global and mgmd interface mode validation*/
  return mgmdMapMessageQueueSend(familyType,
                                 eventType, msgLen, eventMsg);
}

/*********************************************************************
* @purpose  Send AdminScope Boundary event to MGMD (Proxy ) Vendor
*
* @param    eventType    @b{(input)} Event Type
* @param    msgLen       @b{(input)} Message Length
* @param    eventMsg     @b{(input)} Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is an IPv4 specific function
*
* @end
*********************************************************************/
L7_RC_t mgmdMapAdminScopeEventChangeCallback (L7_uint32 eventType,
                                               L7_uint32 msgLen,
                                               void *eventMsg)
{
  return mgmdMapComponentCallback(L7_AF_INET,eventType,
                                  msgLen, eventMsg);

}

void mgmdMapStartupTimerExpireCallback(L7_uint32 familyType, L7_uint32 arg2)
{
  mgmdMapCB_t *mgmdMapCbPtr;
  L7_uint32    cbIndex;
  L7_uint32    protoMax;
  L7_BOOL      restartInProgress;

  MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS, "Startup timer expired for familyType = %d", familyType);

  if (mgmdMapCtrlBlockGet((L7_uchar8) familyType, &mgmdMapCbPtr) != L7_SUCCESS)
  {
    /* Failed to get control block */
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Failed to get Control Block");
    return;
  }

  mgmdMapCbPtr->warmRestartInProgress = L7_FALSE;

  if (mgmdMapCbPtr->mgmdMapStartupTimer != L7_NULLPTR)
  {
    /* free the osapiTimer used to trigger this callback */
    osapiTimerFree(mgmdMapCbPtr->mgmdMapStartupTimer);
    mgmdMapCbPtr->mgmdMapStartupTimer = L7_NULLPTR;
  }

  /*
   * scan each protocol's warmRestart status, only when all have exited warm restart period
   * should we signal system that restart is complete
   */
  protoMax = mgmdMapProtocolsMaxGet();
  restartInProgress = L7_FALSE;

  for (cbIndex = L7_NULL, mgmdMapCbPtr = &mgmdMapCB_g[0];
        ((cbIndex < protoMax) && (restartInProgress == L7_FALSE));
        cbIndex++, mgmdMapCbPtr++)
  {
    if (L7_TRUE == mgmdMapCbPtr->warmRestartInProgress)
    {
      restartInProgress = L7_TRUE;
    }
  }
  if (L7_FALSE == restartInProgress)
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_EVENTS, "Startup interval for all familyTypes, signalling Cnfgr");
    (void)cnfgrApiComponentHwUpdateDone(L7_FLEX_MGMD_MAP_COMPONENT_ID, L7_CNFGR_HW_APPLY_IPMCAST);
  }
}

/*********************************************************************
* @purpose  Post UI event to the Mapping layer
*
* @param    addrFamily  @b{(input)} Address Family Identifier
* @param    event       @b{(input)} UI Event
* @param    pData       @b{(input)} UI Event Data
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMapUIEventSend (mgmdMapCB_t *mgmdMapCbPtr,
                            MGMD_UI_EVENT_TYPE_t eventType,
                            L7_uint32 eventInfo,
                            L7_uint32 intIfNum,
                            L7_MGMD_INTF_MODE_t intfType)
{
  mgmdMapMsg_t msg;

  memset(&msg, 0, sizeof(mgmdMapMsg_t));

  if ((eventType != MGMD_ADMIN_MODE_SET) &&
      (eventType != MGMD_INTF_MODE_SET) &&
      (eventType != MGMD_ROUTER_ALERT_CHECK_SET) &&
      (eventType != MGMD_VERSION_SET) &&
      (eventType != MGMD_ROBUSTNESS_SET) &&
      (eventType != MGMD_QUERY_INTERVAL_SET) &&
      (eventType != MGMD_QUERY_MAX_RESPONSE_TIME_SET) &&
      (eventType != MGMD_STARTUP_QUERY_INTERVAL_SET) &&
      (eventType != MGMD_STARTUP_QUERY_COUNT_SET) &&
      (eventType != MGMD_LASTMEMBER_QUERY_INTERVAL_SET) &&
      (eventType != MGMD_LASTMEMBER_QUERY_COUNT_SET) &&
      (eventType != MGMD_UNSOLICITED_REPORT_INTERVAL_SET))
  {
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Invalid eventType:%d.\n",
                    eventType);
    return L7_FAILURE;
  }

  msg.msgId = MGMDMAP_UI_EVENT_MSG;

  msg.u.mgmdMapUiEventParms.event = eventType;
  msg.u.mgmdMapUiEventParms.cbHandle = (MCAST_CB_HNDL_t) mgmdMapCbPtr;
  msg.u.mgmdMapUiEventParms.intIfNum = intIfNum;
  msg.u.mgmdMapUiEventParms.eventInfo = eventInfo;

  if((eventType == MGMD_VERSION_SET) || (eventType == MGMD_INTF_MODE_SET))
  {
    msg.u.mgmdMapUiEventParms.intfType = intfType;
  }
  if(mgmdMessageSend(MGMD_EVENT_Q,&msg) != L7_SUCCESS)
  {
   MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Event Post Failed for eventType:%d.\n", eventType);
   return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To post a message to the corresponding Queue
*
* @param    QIndex  @b{(input)}   Queue Index
* @param    msg       @b{(input)} Msg pointer
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t mgmdMessageSend(MGMD_QUEUE_ID_t QIndex,void *msg)
{

  if (osapiMessageSend (mgmdMapGblVariables_g.mgmdQueue[QIndex].QPointer, (L7_VOIDPTR) msg,
                        mgmdMapGblVariables_g.mgmdQueue[QIndex].QSize, L7_NO_WAIT, L7_MSG_PRIORITY_NORM)
                     != L7_SUCCESS)
  {
    mgmdMapGblVariables_g.mgmdQueue[QIndex].QSendFailedCnt++;
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "MGMD Map Queue of queueIndex:%d  Failed while sending the message :.\n", QIndex);
    return L7_FAILURE;
  }
  else
  {
     mgmdMapGblVariables_g.mgmdQueue[QIndex].QCurrentMsgCnt++;

     if(mgmdMapGblVariables_g.mgmdQueue[QIndex].QCurrentMsgCnt >
               mgmdMapGblVariables_g.mgmdQueue[QIndex].QMaxRx)
     {
       mgmdMapGblVariables_g.mgmdQueue[QIndex].QMaxRx =
                 mgmdMapGblVariables_g.mgmdQueue[QIndex].QCurrentMsgCnt;
     }
     osapiSemaGive(mgmdMapGblVariables_g.mgmdMapMsgQSema);
  }
  return L7_SUCCESS;
}
/********************** MGMD DEBUG TRACE ROUTINES *******************/
/*********************************************************************
 * @purpose  Saves mgmd debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mgmdDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mgmdDebugSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Copy the operational states into the config file */
  mgmdDebugCfgUpdate();

  if (mgmdDebugHasDataChanged() == L7_TRUE)
  {
    mgmdDebugCfg.hdr.dataChanged = L7_FALSE;
    mgmdDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&mgmdDebugCfg,
        (L7_uint32)(sizeof(mgmdDebugCfg) - sizeof(mgmdDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_FLEX_MGMD_MAP_COMPONENT_ID, MGMD_DEBUG_CFG_FILENAME ,
            (L7_char8 *)&mgmdDebugCfg, (L7_uint32)sizeof(mgmdDebugCfg_t))) == L7_ERROR)
    {
      MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES,
                      "Error on call to osapiFsWrite routine on config file %s\n",
                      MGMD_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
 * @purpose  Restores mgmd debug configuration
 *
 * @param    void
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    mgmdDebugCfg is the overlay
 *
 * @end
 *********************************************************************/
L7_RC_t mgmdDebugRestore(void)
{
  L7_RC_t rc;

  mgmdDebugBuildDefaultConfigData(MGMD_DEBUG_CFG_VER_CURRENT);

  mgmdDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = mgmdApplyDebugConfigData();

  return rc;
}
/*********************************************************************
 * @purpose  Checks if mgmd debug config data has changed
 *
 * @param    void
 *
 * @returns  L7_TRUE or L7_FALSE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_BOOL mgmdDebugHasDataChanged(void)
{
  return mgmdDebugCfg.hdr.dataChanged;
}

/*********************************************************************
 * @purpose  Build default mgmd config data
 *
 * @param    ver   Software version of Config Data
 *
 * @returns  void
 *
 * @notes
 *
 * @end
 *********************************************************************/
void mgmdDebugBuildDefaultConfigData(L7_uint32 ver)
{
  /* setup file header */
  mgmdDebugCfg.hdr.version = ver;
  mgmdDebugCfg.hdr.componentID = L7_FLEX_MGMD_MAP_COMPONENT_ID;
  mgmdDebugCfg.hdr.type = L7_CFG_DATA;
  mgmdDebugCfg.hdr.length = (L7_uint32)sizeof(mgmdDebugCfg_t);
  strcpy((L7_char8 *)mgmdDebugCfg.hdr.filename, MGMD_DEBUG_CFG_FILENAME);
  mgmdDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&mgmdDebugCfg.cfg, 0, sizeof(mgmdDebugCfg.cfg));

}

/*********************************************************************
 * @purpose  Apply mgmd debug config data
 *
 * @param    void
 *
 * @returns  L7_SUCCESS/L7_FAILURE
 *
 * @notes    Called after a default config is built
 *
 * @end
 *********************************************************************/
L7_RC_t mgmdApplyDebugConfigData(void)
{
  L7_uchar8 familyIndex;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 flagIndex;

  for (familyIndex = 0; familyIndex < MGMD_MAP_CB_MAX;
       familyIndex++)
  {
    mgmdDebugTraceFlags_t *mgmdDebugTraceFlags =
       &(mgmdDebugCfg.cfg.mgmdDebugTraceFlag[familyIndex]);
    for(flagIndex = 0;  flagIndex < MGMD_DEBUG_LAST_TRACE ; flagIndex ++)
    {
       if(((*(mgmdDebugTraceFlags[flagIndex/MGMD_DEBUG_TRACE_FLAG_BITS_MAX])) &
          (MGMD_DEBUG_TRACE_FLAG_VALUE << (flagIndex % MGMD_DEBUG_TRACE_FLAG_BITS_MAX))) != 0)
       {
         rc = mgmdDebugTraceFlagSet(familyIndex+1, flagIndex, L7_TRUE);
       }
       else
       {
         rc = mgmdDebugTraceFlagSet(familyIndex+1, flagIndex, L7_FALSE);
       }
    }
  }
  return rc;
}



/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_cfg.c
*
* @purpose   Multiple Spanning tree configuration file
*
* @component dot1s
*
* @comments
*
* @create    10/14/2002
*
* @author    skalyanam
*
* @end
*
**********************************************************************/

#include "dot1s_include.h"
#include "dot1s_debug_api.h"
#include "support_api.h"

DOT1S_VLAN_MASK_t           dot1sVlanMask;
extern DOT1S_BRIDGE_t               *dot1sBridge;
extern DOT1S_INSTANCE_INFO_t        *dot1sInstance;
extern DOT1S_PORT_COMMON_t          *dot1sPort;
extern DOT1S_INSTANCE_MAP_t     *dot1sInstanceMap;
extern DOT1S_INST_VLAN_MAP_t        *dot1sInstVlanMap;
extern DOT1S_PORT_STATS_t           *dot1sPortStats;

extern dot1sCfg_t                  *dot1sCfg;
dot1sDebugCfg_t dot1sDebugCfg;    /* Debug Configuration File Overlay */
extern void dot1sDebugCfgUpdate(void);
extern dot1sDeregister_t dot1sDeregister;
static DOT1S_INST_VLAN_MAP_t dot1sInstVlanMapEndianSafe[L7_MAX_VLAN_ID+2];
static L7_BOOL              defaultsBuilt;
static L7_uint32            dot1sNumLagIntfInit = 0;
static const L7_uint32 dot1s_counters_port[] =
{
L7_PLATFORM_CTR_DOT1S_STP_RX,
L7_PLATFORM_CTR_DOT1S_RSTP_RX,
L7_PLATFORM_CTR_DOT1S_MSTP_RX,
L7_PLATFORM_CTR_DOT1S_STP_TX,
L7_PLATFORM_CTR_DOT1S_RSTP_TX,
L7_PLATFORM_CTR_DOT1S_MSTP_TX,
L7_PLATFORM_CTR_DOT1S_RX_DISCARDS,
L7_PLATFORM_CTR_DOT1S_TX_DISCARDS
};
const L7_uchar8 dot1sConfigDigSigKey[] =
{0x13,0xAC,0x06,0xA6,0x2E,0x47,0xFD,0x51,0xF9,0x5D,0x2B,0xA2,0x43,0xCD,0x03,0x46
};
void *dot1sTaskSyncSema = L7_NULLPTR;
void *dot1s_queue = L7_NULLPTR;  /* reference to the dot1s message queue */
void *dot1s_stateCB_queue = L7_NULLPTR;
void *dot1s_signal_queue = L7_NULLPTR;
static L7_uint32 dot1s_task_id = 0;
static L7_uint32 dot1s_timer_task_id = 0;
extern dot1sCnfgrState_t dot1sCnfgrState;
static nimConfigID_t dot1sConfigId[L7_DOT1S_MAX_INTERFACE_COUNT];

/*********************************************************************
* @purpose  Saves dot1s configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments dot1sCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1sSave(void)
{
    L7_RC_t rc;

    rc = L7_SUCCESS;

    if (dot1sDeregister.dot1sSave == L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "dot1sSave deregistered\n");
      return L7_FAILURE;
    }

    if (dot1sHasDataChanged() == L7_TRUE)
    {
      rc = dot1sCfgUpdate();
      dot1sCfg->hdr.dataChanged = L7_FALSE;
      dot1sCfg->checkSum = nvStoreCrc32((L7_uchar8 *)dot1sCfg, (L7_uint32)(sizeof(dot1sCfg_t) - sizeof(dot1sCfg->checkSum)));

      if (sysapiCfgFileWrite(L7_DOT1S_COMPONENT_ID, DOT1S_CFG_FILENAME, (L7_char8 *)dot1sCfg,
                             (L7_int32)sizeof(dot1sCfg_t)) == L7_ERROR)
      {
        rc = L7_ERROR;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
                "Error on call to sysapiCfgFileWrite routine on config file %s\n", DOT1S_CFG_FILENAME);
      }
    }
    return(rc);
}

/*********************************************************************
* @purpose  Restores dot1s configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments dot1sCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1sRestore(void)
{
  L7_RC_t rc;

  if (dot1sDeregister.dot1sRestore == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "dot1sSave deregistered\n");
    return L7_FAILURE;
  }
  /* remove current config if necessary */
  dot1sConfiguredDataRemove();

  /* build the default config */
  dot1sBuildConfigData(DOT1S_CFG_VER_CURRENT);

  /* apply the stored config */
  rc = dot1sApplyConfigData();

  dot1sCfg->hdr.dataChanged = L7_TRUE;

  return rc;
}

/*********************************************************************
* @purpose  Checks if dot1s user config data has changed
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
L7_BOOL dot1sHasDataChanged(void)
{
  if (dot1sDeregister.dot1sHasDataChanged == L7_TRUE)
  {
    return L7_FALSE;
  }
  return dot1sCfg->hdr.dataChanged;
}
void dot1sResetDataChanged(void)
{
  dot1sCfg->hdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Removes any configured data
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1sConfiguredDataRemove()
{
  L7_uint32 i;
  L7_RC_t rc;

  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    if (dot1sInstanceMap[i].inUse == L7_TRUE)
    {
      rc = dot1sInstanceDelete(dot1sInstanceMap[i].instNumber);
    }
  }
}

/*********************************************************************
* @purpose  Initialize default values for a spanning tree port.
*
* @param    portCfg - Pointer the the port configuration data.
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1sPortDefaultConfigDataBuild (DOT1S_PORT_COMMON_CFG_t *portCfg)
{
  L7_uint32 inst;
  L7_uint32 intIfNum;
  L7_INTF_TYPES_t intfType;
  DOT1S_PORTID_t defPrio = FD_DOT1S_PORT_PRIORITY;

  portCfg->adminEdge = FD_DOT1S_DEFAULT_ADMIN_EDGE;
  portCfg->portAdminMode = FD_DOT1S_PORT_MODE;
  portCfg->HelloTime = DOT1S_INVALID_HELLO_TIME;
  portCfg->autoEdge = FD_DOT1S_AUTO_EDGE;
  portCfg->restrictedRole = FD_DOT1S_ROOTGUARD;
  portCfg->loopGuard = FD_DOT1S_LOOPGUARD;
  portCfg->restrictedTcn = FD_DOT1S_TCNGUARD;

  if ((nimIntIfFromConfigIDGet(&(portCfg->configId),&intIfNum) == L7_SUCCESS) &&
      (nimGetIntfType(intIfNum, &intfType) == L7_SUCCESS) &&
      (intfType == L7_LAG_INTF))
  {
    defPrio = FD_DOT1S_LAG_PORT_PRIORITY;
  }

  for (inst = 0; inst <= L7_MAX_MULTIPLE_STP_INSTANCES; inst++)
  {
    portCfg->portInstInfo[inst].ExternalPortPathCost = FD_DOT1S_PORT_PATH_COST;
    portCfg->portInstInfo[inst].InternalPortPathCost = FD_DOT1S_PORT_PATH_COST;

    /* Update the Auto Port Path Cost setting */
    if (FD_DOT1S_PORT_PATH_COST == L7_DOT1S_AUTO_CALC_PATH_COST)
    {
      portCfg->portInstInfo[inst].autoInternalPortPathCost = L7_TRUE;
      portCfg->portInstInfo[inst].autoExternalPortPathCost = L7_TRUE;
    }
    else
    {
      portCfg->portInstInfo[inst].autoInternalPortPathCost = L7_FALSE;
      portCfg->portInstInfo[inst].autoExternalPortPathCost = L7_FALSE;
    }

    portCfg->portInstInfo[inst].portPriority = defPrio;
  }

  portCfg->bpduFilterMode = FD_DOT1S_BPDUFILTER;
  portCfg->bpduFloodMode = FD_DOT1S_BPDUFLOOD;
}


/*********************************************************************
* @purpose  Build default dot1s config data
*
* @param    ver @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1sBuildConfigData(L7_uint32 ver)
{
    /*L7_uint32 itype, nextIntf;*/
    L7_RC_t rc;
    L7_uint32 i = 0;
    L7_uchar8 mac[L7_MAC_ADDR_LEN];
    L7_uchar8 cfgName[DOT1S_MAX_CONFIG_NAME_SIZE];
    DOT1S_BRIDGE_CFG_t *cfgBridge;
    DOT1S_INSTANCE_INFO_CFG_t *cfgInstance;
    /*L7_uint32 sleepCount = 0;*/
    L7_uint32 cfgIndex;

    memset(&dot1sConfigId[0], 0, sizeof(nimConfigID_t) * L7_DOT1S_MAX_INTERFACE_COUNT);

    /* To differentiate whether this routine is called from dot1sRestore or
     * a failure to read the config file in phase 3 init copy the existing
     * config ids from the current dot1sCfg file.
     * If it has come from dot1sRestore there will be valid vaules in them
     * If it is from phase 3 init they would have been zeroed out
     */
    for (cfgIndex = 1; cfgIndex < L7_DOT1S_MAX_INTERFACE_COUNT; cfgIndex++)
      NIM_CONFIG_ID_COPY(&dot1sConfigId[cfgIndex], &dot1sCfg->cfg.dot1sCfgPort[cfgIndex].configId);

    memset((void *)dot1sCfg, 0x00, sizeof(dot1sCfg_t));

    /* setup file header */
    dot1sCfg->hdr.version = ver;
    dot1sCfg->hdr.componentID = L7_DOT1S_COMPONENT_ID;
    dot1sCfg->hdr.type = L7_CFG_DATA;
    dot1sCfg->hdr.length = (L7_uint32)sizeof(dot1sCfg_t);

    strcpy((L7_char8 *)dot1sCfg->hdr.filename, DOT1S_CFG_FILENAME);
    dot1sCfg->hdr.dataChanged = L7_FALSE;

    /* zero overlays */
    bzero((L7_char8 *)&dot1sCfg->cfg.dot1sBridge, (L7_int32)sizeof(dot1sCfg->cfg.dot1sBridge));
    bzero((L7_char8 *)&dot1sCfg->cfg.dot1sInstance, (L7_int32)sizeof(dot1sCfg->cfg.dot1sInstance));
    bzero((L7_char8 *)&dot1sCfg->cfg.dot1sCfgPort, (L7_int32)sizeof(dot1sCfg->cfg.dot1sCfgPort));
    bzero((L7_char8 *)&dot1sCfg->cfg.dot1sInstanceMap, (L7_int32)sizeof(dot1sCfg->cfg.dot1sInstanceMap));
    bzero((L7_char8 *)&dot1sCfg->cfg.dot1sInstVlanMap, (L7_int32)sizeof(dot1sCfg->cfg.dot1sInstVlanMap));

    /* default values for the dot1sBridge structure */
    cfgBridge = &dot1sCfg->cfg.dot1sBridge;

    cfgBridge->Mode = FD_DOT1S_MODE;
    cfgBridge->ForceVersion = FD_DOT1S_FORCE_VERSION;
    cfgBridge->FwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    cfgBridge->TxHoldCount = FD_DOT1S_BRIDGE_TX_HOLD_COUNT;
    cfgBridge->MigrateTime = 3 * DOT1S_TIMER_UNIT;
    cfgBridge->MaxHops = FD_DOT1S_BRIDGE_MAX_HOP;
    cfgBridge->instanceCount = 0;
    cfgBridge->MstConfigId.formatSelector = 0;
    cfgBridge->bpduGuardMode = L7_FALSE;
    cfgBridge->bpduFilterMode = L7_FALSE;
    dot1sBaseMacAddrGet(mac);
    sprintf((L7_char8 *)cfgName, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    memcpy((L7_uchar8 *)&cfgBridge->MstConfigId.configName, cfgName, sizeof(cfgName));
    cfgBridge->MstConfigId.revLevel = 0;

    /* default values for the dot1sInstVlanMap structure */
    for (i = 0; i < L7_DOT1Q_MAX_VLAN_ID + 2; i++)
    {
        dot1sCfg->cfg.dot1sInstVlanMap[i].instNumber = 0;
    }

    /* calculate the MSTP Configuration Digest after the Instance to Vlan Map is
     * initialized.
     */
    dot1sInstVlanMapEndianSafeMake(dot1sCfg->cfg.dot1sInstVlanMap);

    /* No need to store the digest key in the configuration file*/
    /*
    L7_hmac_md5((L7_uchar8 *)&dot1sInstVlanMapEndianSafe, (L7_uint32)sizeof(dot1sInstVlanMapEndianSafe),
                (L7_uchar8 *)&dot1sConfigDigSigKey, (L7_uint32)sizeof(dot1sConfigDigSigKey),
                (L7_uchar8 *)&dot1sCfg->cfg.dot1sBridge.MstConfigId.configDigest);
    */
    cfgInstance = &dot1sCfg->cfg.dot1sInstance;

    /* default values for the cist dot1sInstance structure */
    rc = dot1sCfgInstanceDefaultPopulate(cfgInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);

    /* default values for the msti dot1sInstance structures */
    for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      rc = dot1sCfgInstanceDefaultPopulate(cfgInstance, i, L7_NULL);
    }

    /* default values for the dot1sInstanceMap structure */
    dot1sCfg->cfg.dot1sInstanceMap[0].instNumber = DOT1S_CIST_ID;
    dot1sCfg->cfg.dot1sInstanceMap[0].inUse = L7_TRUE;
    for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
        dot1sCfg->cfg.dot1sInstanceMap[i].instNumber = 0;
        dot1sCfg->cfg.dot1sInstanceMap[i].inUse = L7_FALSE;
    }

    for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT; i++)
    {

      NIM_CONFIG_ID_COPY(&dot1sCfg->cfg.dot1sCfgPort[i].configId, &dot1sConfigId[i]);

    dot1sPortDefaultConfigDataBuild (&dot1sCfg->cfg.dot1sCfgPort[i]);

  }
  defaultsBuilt = L7_TRUE;
}
/*********************************************************************
* @purpose  Apply dot1s config data
*
* @param    void
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sApplyConfigData(void)
{
  L7_uint32 intIfNum, i;
  L7_uint32 instIndex, index, instId, priInstId, vlanId;
  L7_RC_t rc;
  DOT1S_PORT_COMMON_t *pPort;
  DOT1S_PORT_COMMON_CFG_t *pPortCfg;
  L7_BOOL cfgMode;
  L7_uint32 portLinkState;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));


  /* Store the current configured mode and write the opposite mode
   * in the cfg structure. The opposite mode will allow for proper
   * processing when MSTP is enabled or disabled at the completion
   * of this function.
   */
  cfgMode = dot1sCfg->cfg.dot1sBridge.Mode;
  dot1sCfg->cfg.dot1sBridge.Mode = !cfgMode;

  /* Clear the dot1sBridge structure and apply the configured values */

  bzero((L7_char8 *)dot1sBridge, (L7_int32)sizeof(DOT1S_BRIDGE_t));

  dot1sBridge->ForceVersion = dot1sCfg->cfg.dot1sBridge.ForceVersion;
  dot1sBridge->FwdDelay = dot1sCfg->cfg.dot1sBridge.FwdDelay;
  dot1sBridge->instanceCount  = dot1sCfg->cfg.dot1sBridge.instanceCount;
  dot1sBridge->MaxHops  = dot1sCfg->cfg.dot1sBridge.MaxHops;
  dot1sBridge->MigrateTime  = dot1sCfg->cfg.dot1sBridge.MigrateTime;
  dot1sBridge->Mode  = dot1sCfg->cfg.dot1sBridge.Mode;
  memcpy(&dot1sBridge->MstConfigId.configName,
         &dot1sCfg->cfg.dot1sBridge.MstConfigId.configName,
         DOT1S_MAX_CONFIG_NAME_SIZE);;
  dot1sBridge->MstConfigId.formatSelector = dot1sCfg->cfg.dot1sBridge.MstConfigId.formatSelector;
  dot1sBridge->MstConfigId.revLevel       = dot1sCfg->cfg.dot1sBridge.MstConfigId.revLevel;
  dot1sBridge->TxHoldCount                = dot1sCfg->cfg.dot1sBridge.TxHoldCount;
  dot1sBridge->bpduGuard  = dot1sCfg->cfg.dot1sBridge.bpduGuardMode;
  dot1sBridge->bpduFilter = dot1sCfg->cfg.dot1sBridge.bpduFilterMode;

  /* Force the instance count back to zero, it will be restored as
   * each configured instance is re-created below.
   */
  dot1sBridge->instanceCount = L7_NULL;

  /* Clear the dot1s operational structures and apply the configured values */
  bzero((L7_char8 *)dot1sInstance, (L7_int32)sizeof(DOT1S_INSTANCE_INFO_t));
  bzero((L7_char8 *)dot1sInstanceMap, (L7_int32)sizeof(DOT1S_INSTANCE_MAP_t) * L7_MAX_MULTIPLE_STP_INSTANCES+1);
  bzero((L7_char8 *)dot1sInstVlanMap, (L7_int32)sizeof(DOT1S_INST_VLAN_MAP_t) * L7_MAX_VLAN_ID+2);

  /* The CIST is always in use so set it here and make sure its creation
   * is propagated to the rest of the system. The call to
   * dot1sInstanceCreate is not necessary since MSTP assumes that the
   * CIST is already created.
   */
  dot1sInstanceDefaultPopulate(dot1sInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);
  /*memcpy(&dot1sInstance->cist, &dot1sCfg->cfg.dot1sInstance.cist, sizeof(DOT1S_CIST_t));*/
  dot1sInstance->cist.BridgeIdentifier.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

  dot1sInstance->cist.CistBridgePriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.CistBridgePriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.CistBridgePriority.rootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

  dot1sInstance->cist.cistRootPriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.cistRootPriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.cistRootPriority.rootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

  dot1sInstance->cist.CistBridgeTimes.bridgeFwdDelay = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay;
  dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeHelloTime;
  dot1sInstance->cist.CistBridgeTimes.bridgeMaxAge = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge;
  dot1sInstance->cist.CistBridgeTimes.maxHops = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.maxHops;
  dot1sInstance->cist.CistBridgeTimes.msgAge = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.msgAge;

  dot1sInstance->cist.cistid = dot1sCfg->cfg.dot1sInstance.cist.cistid;

  rc = dot1sInstanceMacAddrPopulate(dot1sInstance, DOT1S_CIST_INDEX);
  rc = dot1sIhDoNotifyInstanceCreate(DOT1S_CIST_ID);
  dot1sInstanceMap[DOT1S_CIST_INDEX].inUse = L7_TRUE;
  for (vlanId = 1; vlanId < L7_DOT1Q_MAX_VLAN_ID + 2; vlanId++)
  {
    if (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask,vlanId))
    {
    if (dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber == (L7_ushort16)DOT1S_CIST_ID)
    {
        rc = dot1sApplyCfgInstanceVlanAdd(DOT1S_CIST_ID, vlanId);
        break;
      }
    }
  }

  rc = dot1sApplyCfgInstanceVlanAdd(DOT1S_CIST_ID, vlanId);

  /* Loop and create all configured instances and map the vlans. */
  for (instIndex = 1; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if ((dot1sCfg->cfg.dot1sInstanceMap[instIndex].inUse == L7_TRUE) &&
        (dot1sInstanceAvailIndexFind(&index) == L7_SUCCESS))
    {
      /* Apply the stored configuration information for this instance into
       * the next available index.
       */
      /*
      memcpy(&dot1sInstance->msti[index],
             &dot1sCfg->cfg.dot1sInstance.msti[instIndex],
             sizeof(DOT1S_MSTI_t));
             */
      dot1sInstanceDefaultPopulate(dot1sInstance, index, dot1sCfg->cfg.dot1sInstance.msti[instIndex].mstid);

      dot1sInstance->msti[index].BridgeIdentifier.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

      dot1sInstance->msti[index].MstiBridgePriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;
      dot1sInstance->msti[index].MstiBridgePriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

      dot1sInstance->msti[index].mstiRootPriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;
      dot1sInstance->msti[index].mstiRootPriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

      dot1sInstance->msti[index].MstiBridgeTimes.bridgeMaxHops = dot1sCfg->cfg.dot1sInstance.msti[instIndex].MstiBridgeTimes.bridgeMaxHops;

      dot1sInstance->msti[index].mstid = dot1sCfg->cfg.dot1sInstance.msti[instIndex].mstid;



      rc = dot1sInstanceMacAddrPopulate(dot1sInstance, index);
      instId = dot1sCfg->cfg.dot1sInstanceMap[instIndex].instNumber;
      rc = dot1sApplyCfgInstanceCreate(instId);

      for (vlanId = 1; vlanId < L7_DOT1Q_MAX_VLAN_ID + 2; vlanId++)
      {
        if (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask,vlanId) &&
            dot1sCfg->cfg.dot1sInstVlanMap[vlanId].instNumber == (L7_ushort16)instId)
        {
          rc = dot1sApplyCfgInstanceVlanAdd(instId, vlanId);
        }
      }
    }
  }

  /* Request to regenerate the MSTP Configuration Identifier now that
   * the vlan map is populated with the configured values.
   */
  rc = dot1sMstConfigIdCompute();

  /* Initialize the dot1sPort structure with defaults and apply the
   * appropriate values from the config structure. This struct does
   * not have a one to one mapping like the others so it can not be
   * done with a straight memcpy.
   */
  bzero((L7_char8 *)dot1sPort, (L7_int32)sizeof(DOT1S_PORT_COMMON_t) * L7_DOT1S_MAX_INTERFACE_COUNT);

  dot1sBridge->enabledPortCount = 0;

  for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&dot1sCfg->cfg.dot1sCfgPort[i].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&dot1sCfg->cfg.dot1sCfgPort[i].configId, &intIfNum) != L7_SUCCESS)
      continue;
    if (dot1sIsValidIntf(intIfNum) != L7_TRUE)
      continue;

      pPort = dot1sIntfFind(intIfNum);

      if (pPort == L7_NULLPTR)
      {
        return L7_FAILURE;
      }
      /* Initialize with the defaults then replace the configured fields
       * with their configured values.
       */
      rc = dot1sPortDefaultPopulate(pPort, intIfNum);

    pPortCfg = &dot1sCfg->cfg.dot1sCfgPort[intIfNum];

      pPort->portAdminMode = pPortCfg->portAdminMode;
      /* notParticipating must be kept in sync with Admin Mode
       * up until the time of an IhAcquire or IhRelease
       */
      if (pPort->portAdminMode == L7_ENABLE)
      {
        pPort->notParticipating = L7_FALSE;
      }
      else
      {
        pPort->notParticipating = L7_TRUE;
      }

    pPort->portNum = intIfNum;
      pPort->adminEdge = pPortCfg->adminEdge;
      pPort->autoEdge = pPortCfg->autoEdge;
      pPort->restrictedRole = pPortCfg->restrictedRole;
      pPort->loopGuard = pPortCfg->loopGuard;
      pPort->restrictedTcn = pPortCfg->restrictedTcn;

      for (instIndex = DOT1S_CIST_INDEX; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
      {
        pPort->portInstInfo[instIndex].ExternalPortPathCost = pPortCfg->portInstInfo[instIndex].ExternalPortPathCost;
        pPort->portInstInfo[instIndex].InternalPortPathCost = pPortCfg->portInstInfo[instIndex].InternalPortPathCost;
        pPort->portInstInfo[instIndex].autoInternalPortPathCost = pPortCfg->portInstInfo[instIndex].autoInternalPortPathCost;
      pPort->portInstInfo[instIndex].autoExternalPortPathCost = pPortCfg->portInstInfo[instIndex].autoExternalPortPathCost;

      pPort->portInstInfo[instIndex].portId = ((pPortCfg->portInstInfo[instIndex].portPriority << 8) | intIfNum);
        /* msti */
        if (instIndex != DOT1S_CIST_INDEX)
        {
        priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

          pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.regRootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId.priInstId = (L7_ushort16)priInstId;
        }
        else /* cist */
        {
        priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

          pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.rootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.regRootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.rootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.regRootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.rootId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.dsgBridgeId.priInstId = (L7_ushort16)priInstId;
          pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.regRootId.priInstId = (L7_ushort16)priInstId;
        }
      }/*endfor instIndex*/

      /* Handle the current link state for this port in case it is
       * already up.
       */
      if (nimGetIntfLinkState(intIfNum, &portLinkState) == L7_SUCCESS)
      {
      rc = dot1sIhNotifySystem(intIfNum, portLinkState);
    }
  }
  /* Set mode to the saved configured value and generate all necessary events */
  if (cfgMode == L7_ENABLE)
  {
    dot1sCfg->cfg.dot1sBridge.Mode = cfgMode;
    rc = dot1sSwitchModeEnableSet(L7_ENABLE);
  }
  else
  {
    dot1sCfg->cfg.dot1sBridge.Mode = cfgMode;
    rc = dot1sSwitchModeDisableSet(L7_DISABLE);
  }

  /* Applied the configured values using routines that will set the dataChanged
   * variable to true, set it to false to ensure that a duplicate config is not
   * written out.
   */
  dot1sCfg->hdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Copies dot1s config data for the bridge and instance alone
*
* @param    void
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sCopyConfigData(void)
{
  L7_uint32 instIndex, index;
  L7_RC_t rc;
  L7_BOOL cfgMode;


  /* Store the current configured mode and write the opposite mode
   * in the cfg structure. The opposite mode will allow for proper
   * processing when MSTP is enabled or disabled at the completion
   * of this function.
   */
  cfgMode = dot1sCfg->cfg.dot1sBridge.Mode;
  dot1sCfg->cfg.dot1sBridge.Mode = !cfgMode;

  /* Clear the dot1sBridge structure and apply the configured values */

  bzero((L7_char8 *)dot1sBridge, (L7_int32)sizeof(DOT1S_BRIDGE_t));

  dot1sBridge->ForceVersion = dot1sCfg->cfg.dot1sBridge.ForceVersion;
  dot1sBridge->FwdDelay = dot1sCfg->cfg.dot1sBridge.FwdDelay;
  dot1sBridge->instanceCount  = dot1sCfg->cfg.dot1sBridge.instanceCount;
  dot1sBridge->MaxHops  = dot1sCfg->cfg.dot1sBridge.MaxHops;
  dot1sBridge->MigrateTime  = dot1sCfg->cfg.dot1sBridge.MigrateTime;
  dot1sBridge->Mode  = dot1sCfg->cfg.dot1sBridge.Mode;
  memcpy(&dot1sBridge->MstConfigId.configName,
         &dot1sCfg->cfg.dot1sBridge.MstConfigId.configName,
         DOT1S_MAX_CONFIG_NAME_SIZE);;
  dot1sBridge->MstConfigId.formatSelector = dot1sCfg->cfg.dot1sBridge.MstConfigId.formatSelector;
  dot1sBridge->MstConfigId.revLevel       = dot1sCfg->cfg.dot1sBridge.MstConfigId.revLevel;
  dot1sBridge->TxHoldCount                = dot1sCfg->cfg.dot1sBridge.TxHoldCount;
  dot1sBridge->bpduGuard  = dot1sCfg->cfg.dot1sBridge.bpduGuardMode;
  dot1sBridge->bpduFilter = dot1sCfg->cfg.dot1sBridge.bpduFilterMode;


  /* Force the instance count back to zero, it will be restored as
   * each configured instance is re-created below.
   */
  dot1sBridge->instanceCount = L7_NULL;

  /* Clear the dot1s operational structures and apply the configured values */
  bzero((L7_char8 *)dot1sInstance, (L7_int32)sizeof(DOT1S_INSTANCE_INFO_t));
  bzero((L7_char8 *)dot1sInstanceMap, (L7_int32)sizeof(DOT1S_INSTANCE_MAP_t) * L7_MAX_MULTIPLE_STP_INSTANCES+1);
  bzero((L7_char8 *)dot1sInstVlanMap, (L7_int32)sizeof(DOT1S_INST_VLAN_MAP_t) * L7_MAX_VLAN_ID+2);

  /* The CIST is always in use so set it here and make sure its creation
   * is propagated to the rest of the system. The call to
   * dot1sInstanceCreate is not necessary since MSTP assumes that the
   * CIST is already created.
   */
  dot1sInstanceDefaultPopulate(dot1sInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);
  /*memcpy(&dot1sInstance->cist, &dot1sCfg->cfg.dot1sInstance.cist, sizeof(DOT1S_CIST_t));*/
  dot1sInstance->cist.BridgeIdentifier.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

  dot1sInstance->cist.CistBridgePriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.CistBridgePriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.CistBridgePriority.rootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

  dot1sInstance->cist.cistRootPriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.cistRootPriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;
  dot1sInstance->cist.cistRootPriority.rootId.priInstId = dot1sCfg->cfg.dot1sInstance.cist.BridgeIdentifier.priInstId;

  dot1sInstance->cist.CistBridgeTimes.bridgeFwdDelay = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeFwdDelay;
  dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeHelloTime;
  dot1sInstance->cist.CistBridgeTimes.bridgeMaxAge = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.bridgeMaxAge;
  dot1sInstance->cist.CistBridgeTimes.maxHops = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.maxHops;
  dot1sInstance->cist.CistBridgeTimes.msgAge = dot1sCfg->cfg.dot1sInstance.cist.CistBridgeTimes.msgAge;

  dot1sInstance->cist.cistid = dot1sCfg->cfg.dot1sInstance.cist.cistid;

  rc = dot1sInstanceMacAddrPopulate(dot1sInstance, DOT1S_CIST_INDEX);

  /* Loop and create all configured instances and map the vlans. */
  for (instIndex = 1; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    if ((dot1sCfg->cfg.dot1sInstanceMap[instIndex].inUse == L7_TRUE) /*&&
        (dot1sInstanceAvailIndexFind(&index) == L7_SUCCESS)*/)
    {
      /* Apply the stored configuration information for this instance into
       * the next available index.
       */
      index = instIndex;
      dot1sInstanceDefaultPopulate(dot1sInstance, index, dot1sCfg->cfg.dot1sInstance.msti[instIndex].mstid);

      dot1sInstance->msti[index].BridgeIdentifier.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

      dot1sInstance->msti[index].MstiBridgePriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;
      dot1sInstance->msti[index].MstiBridgePriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

      dot1sInstance->msti[index].mstiRootPriority.dsgBridgeId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;
      dot1sInstance->msti[index].mstiRootPriority.regRootId.priInstId = dot1sCfg->cfg.dot1sInstance.msti[instIndex].BridgeIdentifier.priInstId;

      dot1sInstance->msti[index].MstiBridgeTimes.bridgeMaxHops = dot1sCfg->cfg.dot1sInstance.msti[instIndex].MstiBridgeTimes.bridgeMaxHops;

      dot1sInstance->msti[index].mstid = dot1sCfg->cfg.dot1sInstance.msti[instIndex].mstid;



      rc = dot1sInstanceMacAddrPopulate(dot1sInstance, index);
    }
  }

  /* bzero the dot1sPort Structure*/
  bzero((L7_char8 *)dot1sPort, (L7_int32)sizeof(DOT1S_PORT_COMMON_t) * L7_DOT1S_MAX_INTERFACE_COUNT);

  dot1sBridge->enabledPortCount = 0;

  /* Applied the configured values using routines that will set the dataChanged
   * variable to true, set it to false to ensure that a duplicate config is not
   * written out.
   */
  dot1sCfg->hdr.dataChanged = L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Populate the Dot1s Instance Structure with Default Values
*
* @param    Instance  @b{(input)} pointer to a dot1s Instance structure
* @param    instIndex @b{(input)} instance Index
* @param    instId    @b{(input)} MSTP instance id
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sInstanceDefaultPopulate(DOT1S_INSTANCE_INFO_t *instance,
                                     L7_uint32 instIndex,
                                     L7_uint32 instId)
{
  L7_RC_t rc;

  rc = dot1sInstanceMacAddrPopulate(instance, instIndex);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    /* default values for the cist dot1sInstance structure */
    /* cist - Bridge Identifier */
    instance->cist.cistid = instId;
    instance->cist.BridgeIdentifier.priInstId = FD_DOT1S_BRIDGE_PRIORITY;

    /* cist - Bridge Priority Vector */
    instance->cist.CistBridgePriority.rootId.priInstId = FD_DOT1S_BRIDGE_PRIORITY;
    instance->cist.CistBridgePriority.extRootPathCost = 0;
    instance->cist.CistBridgePriority.regRootId.priInstId = FD_DOT1S_BRIDGE_PRIORITY;
    instance->cist.CistBridgePriority.intRootPathCost = 0;
    instance->cist.CistBridgePriority.dsgBridgeId.priInstId = FD_DOT1S_BRIDGE_PRIORITY;
    instance->cist.CistBridgePriority.dsgPortId = 0;
    instance->cist.CistBridgePriority.rxPortId = 0;

    /* cist - Bridge Times */
    instance->cist.CistBridgeTimes.msgAge = 0;
    instance->cist.CistBridgeTimes.bridgeMaxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT;
    instance->cist.CistBridgeTimes.bridgeFwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    instance->cist.CistBridgeTimes.bridgeHelloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT;
    instance->cist.CistBridgeTimes.maxHops = FD_DOT1S_BRIDGE_MAX_HOP;

    /* cist - Root Port */
    instance->cist.cistRootPortId = 0;

    /* cist - Root Priority Vector */
    instance->cist.cistRootPriority.regRootId.priInstId = FD_DOT1S_BRIDGE_PRIORITY;
    instance->cist.cistRootPriority.intRootPathCost = 0;
    instance->cist.cistRootPriority.rootId.priInstId = FD_DOT1S_BRIDGE_PRIORITY;
    instance->cist.cistRootPriority.dsgBridgeId.priInstId = FD_DOT1S_BRIDGE_PRIORITY;
    instance->cist.cistRootPriority.dsgPortId = 0;
    instance->cist.cistRootPriority.rxPortId = 0;

    /* cist - Root Times */
    instance->cist.cistRootTimes.msgAge = 0;
    instance->cist.cistRootTimes.maxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT;
    instance->cist.cistRootTimes.fwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    instance->cist.cistRootTimes.helloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT;
    instance->cist.cistRootTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

    /* cist - Port Role Selection State */
    instance->cist.prsState = PRS_INIT;

    /* cist - tcn counters */
    instance->cist.tcWhileChange = 0;
    instance->cist.tcWhileChangeTime = 0xFFFFFFFF;
  }/*end if (instIndex == DOT1S_CIST_INDEX)*/
  else
  {
    /* msti - Bridge Identifier */
    instance->msti[instIndex].mstid = instId;
    instance->msti[instIndex].BridgeIdentifier.priInstId =
      (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);

    /* msti - Bridge Priority Vector */
    instance->msti[instIndex].MstiBridgePriority.regRootId.priInstId =
      (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);
    instance->msti[instIndex].MstiBridgePriority.intRootPathCost = 0;
    instance->msti[instIndex].MstiBridgePriority.dsgBridgeId.priInstId =
      (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);
    instance->msti[instIndex].MstiBridgePriority.dsgPortId = 0;
    instance->msti[instIndex].MstiBridgePriority.rxPortId = 0;

    /* msti - Bridge Times */
    instance->msti[instIndex].MstiBridgeTimes.bridgeMaxHops = FD_DOT1S_BRIDGE_MAX_HOP;

    /* msti - Root Port */
    instance->msti[instIndex].mstiRootPortId = 0;

    /* msti - Root Priority Vector */
    instance->msti[instIndex].mstiRootPriority.regRootId.priInstId = (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);
    instance->msti[instIndex].mstiRootPriority.intRootPathCost = 0;
    instance->msti[instIndex].mstiRootPriority.dsgBridgeId.priInstId = (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);
    instance->msti[instIndex].mstiRootPriority.dsgPortId = 0;
    instance->msti[instIndex].mstiRootPriority.rxPortId = 0;

    /* msti - Root Times */
    instance->msti[instIndex].mstiRootTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

    /* msti - Port Role Selection State */
    instance->msti[instIndex].prsState = PRS_INIT;

    /* msti - tcn counters */
    instance->msti[instIndex].tcWhileChange = 0;
    instance->msti[instIndex].tcWhileChangeTime = 0xFFFFFFFF;
  }/*end else if (instIndex == DOT1S_CIST_INDEX)*/

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Populate the Dot1s Instance Structure with Default Values
*
* @param    Instance  @b{(input)} pointer to a dot1s Instance structure
* @param    instIndex @b{(input)} instance Index
* @param    instId    @b{(input)} MSTP instance id
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sCfgInstanceDefaultPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance,
                                        L7_uint32 instIndex,
                                        L7_uint32 instId)
{
  L7_RC_t rc;

  rc = dot1sCfgInstanceMacAddrPopulate(instance, instIndex);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    /* default values for the cist dot1sInstance structure */
    /* cist - Bridge Identifier */
    instance->cist.cistid = instId;
    instance->cist.BridgeIdentifier.priInstId = FD_DOT1S_BRIDGE_PRIORITY;

    /* cist - Bridge Times */
    instance->cist.CistBridgeTimes.msgAge = 0;
    instance->cist.CistBridgeTimes.bridgeMaxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT;
    instance->cist.CistBridgeTimes.bridgeFwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    instance->cist.CistBridgeTimes.bridgeHelloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT;
    instance->cist.CistBridgeTimes.maxHops = FD_DOT1S_BRIDGE_MAX_HOP;

  }/*end if (instIndex == DOT1S_CIST_INDEX)*/
  else
  {
    /* msti - Bridge Identifier */
    instance->msti[instIndex].mstid = instId;
    instance->msti[instIndex].BridgeIdentifier.priInstId = (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);

    /* msti - Bridge Times */
    instance->msti[instIndex].MstiBridgeTimes.bridgeMaxHops = FD_DOT1S_BRIDGE_MAX_HOP;

  }/*end else if (instIndex == DOT1S_CIST_INDEX)*/

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Populate the Dot1s Instance Structure with the current MAC
* @purpose  address
*
* @param    Instance  @b{(input)} pointer to a dot1s Instance structure
* @param    instIndex @b{(input)} instance Index
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sInstanceMacAddrPopulate(DOT1S_INSTANCE_INFO_t *instance,
                                     L7_uint32 instIndex)
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];

  dot1sBaseMacAddrGet(macAddr);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    memcpy(instance->cist.BridgeIdentifier.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->cist.CistBridgePriority.rootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->cist.CistBridgePriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->cist.CistBridgePriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->cist.cistRootPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->cist.cistRootPriority.rootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->cist.cistRootPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
  }
  else
  {
    memcpy(instance->msti[instIndex].BridgeIdentifier.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->msti[instIndex].MstiBridgePriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->msti[instIndex].MstiBridgePriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->msti[instIndex].mstiRootPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(instance->msti[instIndex].mstiRootPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Populate the Dot1s Instance Structure with the current MAC
* @purpose  address
*
* @param    Instance  @b{(input)} pointer to a dot1s Instance structure
* @param    instIndex @b{(input)} instance Index
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sCfgInstanceMacAddrPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance,
                                     L7_uint32 instIndex)
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];

  dot1sBaseMacAddrGet(macAddr);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    memcpy(instance->cist.BridgeIdentifier.macAddr, &macAddr, L7_MAC_ADDR_LEN);
  }
  else
  {
    memcpy(instance->msti[instIndex].BridgeIdentifier.macAddr, &macAddr, L7_MAC_ADDR_LEN);
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Populate the Dot1s Port Structure with Default Values
*
* @param    Port     @b{(input)} a pointer to the dot1s port structure
* @param    intIfNum @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only populate the non configured fields
*
* @end
*********************************************************************/
L7_RC_t dot1sPortDefaultPopulate(DOT1S_PORT_COMMON_t *pPort, L7_uint32 intIfNum)
{
  L7_uint32 instIndex = 0;
  L7_RC_t rc;

  pPort->portUpEnabledNum = 0;
  pPort->portEnabled = L7_FALSE;
  pPort->tick = 0;
  pPort->txCount = 0;
  pPort->operEdge = 0;
  pPort->infoInternal = 0;
  pPort->newInfoCist = L7_FALSE;
  pPort->newInfoMist = L7_FALSE;
  pPort->rcvdInternal = L7_FALSE;
  pPort->prevRcvdInternal = L7_FALSE;
  pPort->initPm = 0;
  pPort->rcvdBpdu = 0;
  pPort->rcvdRSTP = 0;
  pPort->rcvdSTP = 0;
  pPort->rcvdTcAck = 0;
  pPort->rcvdTcn = 0;
  pPort->sendRSTP = 0;
  pPort->tcAck = 0;
  pPort->bdmBpduRcvd = L7_FALSE;
  pPort->mdelayWhile = 0;
  pPort->helloWhen = 0;
  pPort->edgeDelayWhile = 0;
  pPort->loopInconsistent = L7_FALSE;

  for (instIndex = 0; instIndex <= L7_MAX_MULTIPLE_STP_INSTANCES; instIndex++)
  {
    rc = dot1sPortInstInfoDefaultPopulate(pPort, instIndex, intIfNum);
  }

  pPort->prxState = PRX_DISCARD;
  pPort->ppmState = PPM_CHECKING_RSTP; /*TBD */
  pPort->ptxState = PTX_INIT;
  pPort->bdmState = (FD_DOT1S_DEFAULT_ADMIN_EDGE == L7_TRUE) ? BDM_EDGE : BDM_NOT_EDGE;
  /* This value is based on the default value of  Admin edge */
  pPort->mcheck = L7_FALSE;
  pPort->lastNotificationSent = L7_LAST_PORT_EVENT;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Populate the Dot1s Port InstInfo Structure with Default
* @purpose  values
*
* @param    Port      @b{(input)} a pointer to the dot1s port structure
* @param    instIndex @b{(input)} instance Index
* @param    intIfNum  @b{(input)} interface number
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments
* @notes    Only populate the non configured fields
*
* @end
*********************************************************************/
L7_RC_t dot1sPortInstInfoDefaultPopulate(DOT1S_PORT_COMMON_t *pPort,
                                         L7_uint32 instIndex,
                                         L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_ushort16 priInstId;
  L7_uint32 portIndex;

  portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);
  if (portIndex == L7_NULL)
    return L7_FAILURE;

  rc = dot1sPortInstInfoMacAddrPopulate(pPort, instIndex);

  pPort->portInstInfo[instIndex].stPortNum = intIfNum;
  pPort->portInstInfo[instIndex].fdWhile = 0;
  pPort->portInstInfo[instIndex].rrWhile = 0;
  pPort->portInstInfo[instIndex].rbWhile = 0;
  pPort->portInstInfo[instIndex].tcWhile = 0;
  pPort->portInstInfo[instIndex].forward = L7_FALSE;
  pPort->portInstInfo[instIndex].forwarding = L7_FALSE;
  pPort->portInstInfo[instIndex].infoIs = INFO_DISABLED;
  pPort->portInstInfo[instIndex].learn = L7_FALSE;
  pPort->portInstInfo[instIndex].learning = L7_FALSE;
  pPort->portInstInfo[instIndex].proposed = L7_FALSE;
  pPort->portInstInfo[instIndex].proposing = L7_FALSE;
  pPort->portInstInfo[instIndex].rcvdTc = L7_FALSE;
  pPort->portInstInfo[instIndex].reRoot = L7_FALSE;
  pPort->portInstInfo[instIndex].reselect = L7_FALSE;
  pPort->portInstInfo[instIndex].selected = L7_FALSE;
  pPort->portInstInfo[instIndex].tcProp = L7_FALSE;
  pPort->portInstInfo[instIndex].updtInfo = L7_FALSE;
  pPort->portInstInfo[instIndex].agreed = L7_FALSE;
  pPort->portInstInfo[instIndex].rcvdInfo = OtherInfo;
  pPort->portInstInfo[instIndex].role = ROLE_DISABLED;
  pPort->portInstInfo[instIndex].selectedRole = ROLE_DISABLED;
  pPort->portInstInfo[instIndex].sync = L7_FALSE;
  pPort->portInstInfo[instIndex].synced = L7_FALSE;
  pPort->portInstInfo[instIndex].portState = L7_DOT1S_DISABLED;

  dot1sPortStats[portIndex].transitionsIntoLoopInconsistentState = L7_NULL;
  dot1sPortStats[portIndex].transitionsOutOfLoopInconsistentState = L7_NULL;

  /* msti */
  if (instIndex != DOT1S_CIST_INDEX)
  {
    priInstId = (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | dot1sInstanceMap[instIndex].instNumber);

    pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgPortId = pPort->portInstInfo[instIndex].portId;
    pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.intRootPathCost = 0;
    pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.rxPortId = pPort->portInstInfo[instIndex].portId;

    pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

    pPort->portInstInfo[instIndex].inst.msti.mstiMaster = 0;
    pPort->portInstInfo[instIndex].inst.msti.mstiMastered = 0;

    pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.dsgBridgeId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.dsgPortId = pPort->portInstInfo[instIndex].portId;
    pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.intRootPathCost = 0;
    pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.regRootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.rxPortId = pPort->portInstInfo[instIndex].portId;

    pPort->portInstInfo[instIndex].inst.msti.mstiMsgTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

    pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgPortId = 0;
    pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.intRootPathCost = 0;
    pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.rxPortId = 0;

    pPort->portInstInfo[instIndex].inst.msti.mstiPortTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

  } /* end if msti */
  else /* cist */
  {
    priInstId = FD_DOT1S_BRIDGE_PRIORITY;

    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.rootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.dsgBridgeId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.dsgPortId = pPort->portInstInfo[instIndex].portId;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.intRootPathCost = 0;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.regRootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.rxPortId = pPort->portInstInfo[instIndex].portId;

    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedTimes.fwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedTimes.helloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedTimes.maxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedTimes.msgAge = 0;

    pPort->portInstInfo[instIndex].inst.cist.cistDesignatedTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

    pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.rootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.dsgBridgeId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.dsgPortId = pPort->portInstInfo[instIndex].portId;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.intRootPathCost = 0;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.regRootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.rxPortId = pPort->portInstInfo[instIndex].portId;

    pPort->portInstInfo[instIndex].inst.cist.cistMsgTimes.fwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgTimes.helloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgTimes.maxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgTimes.msgAge = 0;
    pPort->portInstInfo[instIndex].inst.cist.cistMsgTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;

    pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.rootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.dsgBridgeId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.dsgPortId = pPort->portInstInfo[instIndex].portId;
    pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.intRootPathCost = 0;
    pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.regRootId.priInstId = priInstId;
    pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.rxPortId = pPort->portInstInfo[instIndex].portId;

    pPort->portInstInfo[instIndex].inst.cist.cistPortTimes.fwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistPortTimes.helloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistPortTimes.maxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT;
    pPort->portInstInfo[instIndex].inst.cist.cistPortTimes.msgAge = 0;
    pPort->portInstInfo[instIndex].inst.cist.cistPortTimes.remHops = FD_DOT1S_BRIDGE_MAX_HOP;
  } /* end else cist */

  pPort->portInstInfo[instIndex].agree = 0;
  pPort->portInstInfo[instIndex].changedMaster = L7_FALSE;
  pPort->portInstInfo[instIndex].rcvdMsg = L7_FALSE;
  pPort->portInstInfo[instIndex].pimState = PIM_DISABLED;
  pPort->portInstInfo[instIndex].prtState = PRT_INIT_PORT;
  pPort->portInstInfo[instIndex].pstState = PST_DISCARDING;
  pPort->portInstInfo[instIndex].tcmState = TCM_INACTIVE;
  pPort->portInstInfo[instIndex].stateChangeInProgress = 0;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Populate the Dot1s Port InstInfo Structure with the current
* @purpose  Mac address
*
* @param    Port      @b{(input)} a pointer to the dot1s port structure
* @param    instIndex @b{(input)} instance Index
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sPortInstInfoMacAddrPopulate(DOT1S_PORT_COMMON_t *pPort,
                                         L7_uint32 instIndex)
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];

  dot1sBaseMacAddrGet(macAddr);

  /* msti */
  if (instIndex != DOT1S_CIST_INDEX)
  {
    memcpy(pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.msti.mstiDesignatedPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.msti.mstiMsgPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.msti.mstiPortPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
  }
  else /* cist */
  {
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.rootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistDesignatedPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.rootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistMsgPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.rootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.dsgBridgeId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
    memcpy(pPort->portInstInfo[instIndex].inst.cist.cistPortPriority.regRootId.macAddr, &macAddr, L7_MAC_ADDR_LEN);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update dot1s information in the Cfg structure
*
* @param    void
*
* @returns  L7_SUCCESS
* @retruns  L7_FAILURE
*
* @comments    none
*
* @end
*********************************************************************/
L7_RC_t dot1sCfgUpdate(void)
{
  dot1sCfg->cfg.dot1sBridge.instanceCount = dot1sBridge->instanceCount;

  memcpy(&dot1sCfg->cfg.dot1sInstanceMap, dot1sInstanceMap, sizeof(DOT1S_INSTANCE_MAP_t) * (L7_MAX_MULTIPLE_STP_INSTANCES+1));
  /*memcpy(&dot1sCfg->cfg.dot1sInstVlanMap, dot1sInstVlanMap, sizeof(DOT1S_INST_VLAN_MAP_t) * (L7_MAX_VLAN_ID+2));*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Registers all dot1s statistics with the stats manager
*
* @param    none
*
* @returns  L7_SUCCESS   if all stats registered
*           L7_FALIURE   if stats manager fails to accept all registrations
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sStatsCreate(L7_uint32 intIfNum)
{
  L7_RC_t rc, nim_rc;
  L7_uint32 i;
  L7_INTF_TYPES_t sysIntfType;
  statsParm_entry_t dot1sCtr;
  L7_uint32 listSize_port = (L7_uint32)(sizeof(dot1s_counters_port) / sizeof(L7_uint32));

  rc = L7_SUCCESS;
  dot1sCtr.cKey = 1;
  dot1sCtr.cSize = C32_BITS;
  dot1sCtr.cType = ExternalCtr;
  dot1sCtr.isResettable = L7_FALSE;
  dot1sCtr.pMutlingsParmList = L7_NULL;
  dot1sCtr.pSource = (L7_VOIDFUNCPTR_t) dot1sStatGet;

  nim_rc = nimGetIntfType(intIfNum, &sysIntfType);
  if ((sysIntfType == L7_PHYSICAL_INTF) && (nim_rc == L7_SUCCESS))
  {
    dot1sCtr.cKey = intIfNum;
    for (i=0; (i < listSize_port && rc == L7_SUCCESS) ;i++)
    {
      dot1sCtr.cId = dot1s_counters_port[i];
      rc = statsCreate(1, (pStatsParm_list_t)(&dot1sCtr));
    }
  }

  return rc;

}

/*********************************************************************
* @purpose  Decodes and services an incoming request from the stats
*           manager for a dot1s statistic
*
* @param    c     @b{(input)} Pointer to storage allocated by stats
                              manager to hold the results
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @comments Only called by stats manager
*
* @end
*********************************************************************/
void dot1sStatGet(void *statPtr)
{
  pCounterValue_entry_t c = (pCounterValue_entry_t) statPtr;
  c->status = L7_SUCCESS;
  c->cSize = C32_BITS;

  switch (c->cId)
  {
  case L7_PLATFORM_CTR_DOT1S_STP_RX:
    c->cValue.low = dot1sPortStats[c->cKey].stpRx;
    break;
  case L7_PLATFORM_CTR_DOT1S_RSTP_RX:
    c->cValue.low = dot1sPortStats[c->cKey].rstpRx;
    break;
  case L7_PLATFORM_CTR_DOT1S_MSTP_RX:
    c->cValue.low = dot1sPortStats[c->cKey].mstpRx;
    break;
  case L7_PLATFORM_CTR_DOT1S_STP_TX:
    c->cValue.low = dot1sPortStats[c->cKey].stpTx;
    break;
  case L7_PLATFORM_CTR_DOT1S_RSTP_TX:
    c->cValue.low = dot1sPortStats[c->cKey].rstpTx;
    break;
  case L7_PLATFORM_CTR_DOT1S_MSTP_TX:
    c->cValue.low = dot1sPortStats[c->cKey].mstpTx;
    break;
  case L7_PLATFORM_CTR_DOT1S_RX_DISCARDS:
    c->cValue.low = dot1sPortStats[c->cKey].rxDiscards;
    break;
  case L7_PLATFORM_CTR_DOT1S_TX_DISCARDS:
    c->cValue.low = dot1sPortStats[c->cKey].txDiscards;
    break;
  default:
    c->status = L7_FAILURE;
  }
}
/*********************************************************************
* @purpose  This routine calculates the mst config id digest using hmac md5 computaion
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sMstConfigIdCompute()
{
  DOT1S_BRIDGE_t *bridge;

  bridge = dot1sBridgeParmsFind();

  if (bridge != L7_NULLPTR)
  {
    /*call the hmac api to recompute the config id*/
    dot1sInstVlanMapEndianSafeMake(dot1sInstVlanMap);
    L7_hmac_md5((L7_uchar8 *)&dot1sInstVlanMapEndianSafe, (L7_uint32)sizeof(dot1sInstVlanMapEndianSafe),
                (L7_uchar8 *)dot1sConfigDigSigKey, (L7_uint32)sizeof(dot1sConfigDigSigKey),
                (L7_uchar8 *)&bridge->MstConfigId.configDigest);

  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Keeps track of LAG interface creates
*
* @param    intIfNum    @b{(input)}  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    Once all the lag interfaces are created, only then can we apply
*           any configuration. This configuration could be a saved configuration
*           in which case defaultsBuilt will be False, otherwise we build the
*           defaults once again and then apply the latest configuration.
*           In the process of building and/or applying configuration, the
*           dot1s port structure are updated for the lag interfaces that
*           were missed (as they had not yet been created, dot3ad explicitly
*           creates them later on in the cnfgrinit sequence)
*           when dot1s initialized.
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLagIntfInit(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  dot1sNumLagIntfInit++;
  if (dot1sNumLagIntfInit == L7_MAX_NUM_LAG_INTF)
  {
    if (defaultsBuilt == L7_TRUE)
    {
      dot1sBuildConfigData(dot1sCfg->hdr.version);
      /*first time around the cfg file was automatically saved
      during buildDefaultConfig BUT without lag interfaces*/
      /*so save them now*/
      dot1sCfg->hdr.dataChanged = L7_FALSE;
      dot1sCfg->checkSum = nvStoreCrc32((L7_uchar8 *)dot1sCfg, (L7_uint32)(sizeof(dot1sCfg_t) - sizeof(dot1sCfg->checkSum)));

      if (osapiFsWrite(DOT1S_CFG_FILENAME, (L7_char8 *)dot1sCfg,
                       (L7_int32)sizeof(dot1sCfg_t)) == L7_ERROR)
      {
        rc = L7_ERROR;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
                "Error on call to osapiFsWrite routine on config file %s\n", DOT1S_CFG_FILENAME);
      }
    }
    rc = dot1sApplyConfigData();
  }
  return rc;
}
/*********************************************************************
* @purpose  Takes the dot1sInstVlanMap structue and makes it Endian safe
*
*
* @param    dot1sInstVlanMapInput @b{(input)} pointer to a DOT1S_INST_VLAN_MAP_t
*
* @returns  none
*
* @notes    This routine works on the dot1sInstVlanMap structure and stores
*           endian safe result in the structure dot1sInstVlanMapEndianSafe
*           Caller of this routine should use the static dot1sInstVlanMapEndianSafe
*           for submission to the hmac md5 calculation.
*
* @end
*********************************************************************/
void dot1sInstVlanMapEndianSafeMake(DOT1S_INST_VLAN_MAP_t *dot1sInstVlanMapInput)
{
  L7_uint32 i;

  /* Bzero the dot1sInstVlanMapEndianSafe array */
  bzero((char *)&dot1sInstVlanMapEndianSafe, (L7_int32)sizeof(dot1sInstVlanMapEndianSafe));

  for (i = 0; i < L7_MAX_VLAN_ID+2; i++)
  {
    /* Check if the instNumber is 0. Make the htons call only if it is non zero.
     * Zero is the same byte order wise so we save on the unneccessary call to htons.
     * Also there will be more zeros in this array then non zero mstids.
     */
    if (dot1sInstVlanMapInput[i].instNumber != 0)
    {
      dot1sInstVlanMapEndianSafe[i].instNumber = osapiHtons(dot1sInstVlanMapInput[i].instNumber);
    }
  }
  return;
}

/*********************************************************************
*
* @purpose  System Initialization for IGMP Snooping component
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
L7_RC_t dot1sInit(L7_CNFGR_CMD_DATA_t *pCmdData)
{
  DOT1S_MSG_t msg;
  DOT1S_STATE_CHANGE_MSG_t stateChgMsg;
  L7_CNFGR_CB_DATA_t cbData;

  /*semaphore creation for task protection over the common data*/
  dot1sTaskSyncSema = osapiSemaBCreate( OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dot1sTaskSyncSema == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Unable to create dot1s task semaphore()");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  dot1s_queue = (void*)osapiMsgQueueCreate("dot1s_queue", DOT1S_MSG_COUNT,
                                            (L7_uint32)sizeof(DOT1S_MSG_t));

  dot1s_stateCB_queue = osapiMsgQueueCreate("dot1s_stateCB_queue",
                                            DOT1S_STATE_CHANGE_QUEUE_SIZE,
                                            sizeof(stateChgMsg));

  dot1s_signal_queue = osapiMsgQueueCreate ("dot1s_signal_queue",
                                            DOT1S_SIGNAL_QUEUE_SIZE,
                                            sizeof(DOT1S_SIGNAL_MSG_t));

  if ((dot1s_queue == L7_NULLPTR) ||
      (dot1s_stateCB_queue == L7_NULLPTR) ||
      (dot1s_signal_queue == L7_NULLPTR))
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "dot1sInit: msgQueue creation error.");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  /* create dot1s_timer_task - to service queue*/
  dot1s_timer_task_id = osapiTaskCreate("dot1s_timer_task",
                                        (void *)dot1s_timer_task, 0, 0,
                                        L7_DOT1S_STACK_SIZE,
                                        L7_DOT1S_DEFAULT_DOT1S_TIMER_TASK_PRI,
                                        L7_DEFAULT_TASK_SLICE);

  if (dot1s_timer_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "dot1sInit: timer task creation error.");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }
  if (osapiWaitForTaskInit (L7_DOT1S_TIMER_TASK_SYNC,
                            L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Unable to initialize dot1s_timer_task()");
    return(L7_FAILURE);
  }

  dot1s_task_id = osapiTaskCreate("dot1s_task", (void *)dot1s_task, 0, 0,
                                             L7_DOT1S_STACK_SIZE,
                                             L7_DOT1S_DEFAULT_DOT1S_TASK_PRI,
                                             L7_DEFAULT_TASK_SLICE);
  if (dot1s_task_id == L7_ERROR)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "dot1sInit: task creation error.");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }
  if (osapiWaitForTaskInit (L7_DOT1S_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_CRITICAL, L7_DOT1S_COMPONENT_ID,
            "Unable to initialize dot1s_task()");
    cbData.correlator = pCmdData->correlator;
    cbData.asyncResponse.rc = L7_FAILURE;
    cbData.asyncResponse.u.reason = L7_CNFGR_ERR_RC_LACK_OF_RESOURCES;
    cnfgrApiCallback(&cbData);
    return L7_FAILURE;
  }

  memcpy(&msg.data.CmdData, pCmdData, sizeof(L7_CNFGR_CMD_DATA_t));
  msg.event = dot1sCnfgr;

  osapiMessageSend(dot1s_queue, &msg, (L7_uint32)sizeof(DOT1S_MSG_t),
                   L7_NO_WAIT, L7_MSG_PRIORITY_NORM);
  dot1s_task_signal();


  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  System Init Undo for IGMP Snooping component
*
* @param    none
*
* @notes    none
*
* @end
*********************************************************************/
void dot1sInitUndo()
{
  if (dot1s_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(dot1s_queue);
    dot1s_queue = L7_NULLPTR;
  }

  if (dot1s_task_id != L7_ERROR)
  {
    osapiTaskDelete(dot1s_task_id);
    dot1s_task_id = L7_ERROR;
  }

  if (dot1s_stateCB_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(dot1s_stateCB_queue);
    dot1s_stateCB_queue = L7_NULLPTR;
  }

  if (dot1s_signal_queue != L7_NULLPTR)
  {
    osapiMsgQueueDelete(dot1s_signal_queue);
    dot1s_signal_queue = L7_NULLPTR;
  }

  if (dot1s_timer_task_id != L7_ERROR)
  {
    osapiTaskDelete(dot1s_timer_task_id);
    dot1s_timer_task_id = L7_ERROR;
  }

  dot1sCnfgrState = DOT1S_PHASE_INIT_0;
}

/*********************************************************************
* @purpose  Saves dot1s configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot1sDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1sDebugSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  if (dot1sDebugHasDataChanged() == L7_TRUE)
  {
    /* Copy the operational states into the config file */
    dot1sDebugCfgUpdate();

    /* Store the config file */
    dot1sDebugCfg.hdr.dataChanged = L7_FALSE;
    dot1sDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&dot1sDebugCfg,
                                      (L7_uint32)(sizeof(dot1sDebugCfg) - sizeof(dot1sDebugCfg.checkSum)));
        /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_DOT1S_COMPONENT_ID, DOT1S_DEBUG_CFG_FILENAME,
                            (L7_char8 *)&dot1sDebugCfg, (L7_uint32)sizeof(dot1sDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",DOT1S_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores dot1s debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot1sDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1sDebugRestore(void)
{
  L7_RC_t rc;

  dot1sDebugBuildDefaultConfigData(DOT1S_DEBUG_CFG_VER_CURRENT);

  dot1sDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = dot1sApplyDebugConfigData();

  return rc;
}

/*********************************************************************
* @purpose  Checks if dot1s debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1sDebugHasDataChanged(void)
{
  return dot1sDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default dot1s config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  dot1sDebugCfg.hdr.version = ver;
  dot1sDebugCfg.hdr.componentID = L7_DOT1S_COMPONENT_ID;
  dot1sDebugCfg.hdr.type = L7_CFG_DATA;
  dot1sDebugCfg.hdr.length = (L7_uint32)sizeof(dot1sDebugCfg);
  strcpy((L7_char8 *)dot1sDebugCfg.hdr.filename, DOT1S_DEBUG_CFG_FILENAME);
  dot1sDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&dot1sDebugCfg.cfg, 0, sizeof(dot1sDebugCfg.cfg));
}

/*********************************************************************
* @purpose  Apply dot1s debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t dot1sApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc = dot1sDebugPacketTraceFlagSet(dot1sDebugCfg.cfg.dot1sDebugPacketTraceTxFlag,dot1sDebugCfg.cfg.dot1sDebugPacketTraceRxFlag);

  return rc;
}







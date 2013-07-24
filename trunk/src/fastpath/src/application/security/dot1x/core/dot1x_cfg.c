/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_cfg.c
*
* @purpose   dot1x configuration file
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*
**********************************************************************/

#include "dot1x_include.h"
#include "support_api.h"

dot1xCfg_t *dot1xCfg;

dot1xInfo_t      dot1xInfo;
dot1xPortInfo_t  *dot1xPortInfo = L7_NULLPTR;
dot1xSupplicantPortInfo_t  *dot1xSupplicantPortInfo = L7_NULLPTR;
dot1xPortStats_t *dot1xPortStats = L7_NULLPTR;
L7_uint32        *dot1xMapTbl = L7_NULLPTR;

dot1xPortSessionStats_t *dot1xPortSessionStats = L7_NULLPTR;

dot1xDebugCfg_t dot1xDebugCfg;
extern void dot1xDebugCfgUpdate(void);
extern L7_VLAN_MASK_t dot1xGuestVlanMask;
extern L7_ushort16 *dot1xGuestVlanIntfCount;

/*********************************************************************
* @purpose  Saves dot1x configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments dot1xCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1xSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (dot1xHasDataChanged() == L7_TRUE)
  {
    dot1xCfg->cfgHdr.dataChanged = L7_FALSE;
    dot1xCfg->checkSum = nvStoreCrc32( (L7_uchar8 *)dot1xCfg,
                                      (L7_uint32)(sizeof(dot1xCfg_t) - sizeof(dot1xCfg->checkSum)) );

    if ( (rc = sysapiCfgFileWrite(L7_DOT1X_COMPONENT_ID, DOT1X_CFG_FILENAME, (L7_char8 *)dot1xCfg,
                                  (L7_int32)sizeof(dot1xCfg_t))) != L7_SUCCESS )
    {
      rc = L7_ERROR;
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
              "dot1xSave: Error on call to sysapiCfgFileWrite for config file %s\n", DOT1X_CFG_FILENAME);
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Checks if dot1x user config data has changed
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
L7_BOOL dot1xHasDataChanged(void)
{
  return dot1xCfg->cfgHdr.dataChanged;
}
void dot1xResetDataChanged(void)
{
  dot1xCfg->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Print the current dot1x config values to serial port
*
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xCfgDump(void)
{
  L7_char8 buf[32];
  L7_uint32 i;
  L7_uint32 intIfNum;
  dot1xPortCfg_t *pCfg;
  nimConfigID_t configIdNull;
  nimUSP_t usp;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  printf("\n");
  printf("DOT1X\n");
  printf("=====\n");

  if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
    osapiSnprintf(buf, sizeof(buf),"Enable");
  else
    osapiSnprintf(buf,sizeof(buf),"Disable");
  printf("Admin Mode - %s\n\n", buf);

  if (dot1xCfg->dot1xLogTraceMode == L7_ENABLE)
    osapiSnprintf(buf, sizeof(buf),"Enable");
  else
    osapiSnprintf(buf,sizeof(buf),"Disable");
  printf("Log Trace Mode - %s\n\n", buf);

  if(dot1xCfg->vlanAssignmentMode == L7_ENABLE)
    osapiSnprintf(buf,sizeof(buf),"Enable");
  else
    osapiSnprintf(buf, sizeof(buf), "Disable");
  printf("Vlan Assignment Mode - %s\n\n", buf);

  if(dot1xCfg->dynamicVlanMode== L7_ENABLE)
    osapiSnprintf(buf,sizeof(buf),"Enable");
  else
    osapiSnprintf(buf, sizeof(buf), "Disable");
  printf("Dynamic Vlan Creation  Mode - %s\n\n", buf);


  printf("Interface configuration:\n");
  for (i = 1; i < L7_DOT1X_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&dot1xCfg->dot1xPortCfg[i].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&(dot1xCfg->dot1xPortCfg[i].configId), &intIfNum) != L7_SUCCESS)
      continue;
    if (dot1xIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if ((nimGetUnitSlotPort(intIfNum, &usp) == L7_SUCCESS) )
      {
        printf("USP: %d.%d.%d, intIfNum: %d\n", usp.unit, usp.slot, usp.port, intIfNum);
      }
      else
      {
        printf("Failed getting USP for intIfNum %d\n", intIfNum);
        continue;
      }

      switch (pCfg->adminControlledDirections)
      {
      case L7_DOT1X_PORT_DIRECTION_BOTH:
        osapiSnprintf(buf, sizeof(buf), "Both");
        break;
      case L7_DOT1X_PORT_DIRECTION_IN:
        osapiSnprintf(buf,sizeof(buf),"In");
        break;
      default:
        osapiSnprintf(buf, sizeof(buf),"N/A");
        break;
      }
      printf("  Controlled Directions: %s(%d)\n", buf, pCfg->adminControlledDirections);

      switch (pCfg->portControlMode)
      {
      case L7_DOT1X_PORT_FORCE_UNAUTHORIZED:
        osapiSnprintf(buf,sizeof(buf), "forceUnauthorized");
        break;
      case L7_DOT1X_PORT_AUTO:
        osapiSnprintf(buf, sizeof(buf), "auto");
        break;
      case L7_DOT1X_PORT_FORCE_AUTHORIZED:
        osapiSnprintf(buf, sizeof(buf), "forceAuthorized");
        break;
      default:
        osapiSnprintf(buf,sizeof(buf), "N/A");
        break;
      }
      printf("  Port Control Mode:     %s(%d)\n", buf, pCfg->portControlMode);

      printf("  Quiet Period:          %d\n", pCfg->quietPeriod);
      printf("  Tx Period:             %d\n", pCfg->txPeriod);
      printf("  Supp Timeout:          %d\n", pCfg->suppTimeout);
      printf("  Server Timeout:        %d\n", pCfg->serverTimeout);
      printf("  Max Req:               %d\n", pCfg->maxReq);
      printf("  ReAuth Period:         %d\n", pCfg->reAuthPeriod);
      printf("  GuestVLAN Period:      %d\n", pCfg->guestVlanPeriod);
      printf("  Unauthenticated vlan:  %d\n", pCfg->unauthenticatedVlan);

      switch (pCfg->reAuthEnabled)
      {
      case L7_TRUE:
        osapiSnprintf(buf, sizeof(buf),"True");
        break;
      case L7_FALSE:
        osapiSnprintf(buf, sizeof(buf), "False");
        break;
      default:
        osapiSnprintf(buf,sizeof(buf), "N/A");
        break;
      }
      printf("  ReAuth Enabled:        %s\n", buf);

      switch (pCfg->keyTxEnabled)
      {
      case L7_TRUE:
        osapiSnprintf(buf,sizeof(buf), "True");
        break;
      case L7_FALSE:
        osapiSnprintf(buf, sizeof(buf), "False");
        break;
      default:
        osapiSnprintf(buf, sizeof(buf), "N/A");
        break;
      }
      printf("  KeyTx Enabled:         %s\n", buf);

      printf(" Guest VLAN Id :      %d\n", pCfg->guestVlanId);

       switch (pCfg->mabEnabled)
      {
      case L7_ENABLE:
        osapiSnprintf(buf, sizeof(buf),"Enabled");
        break;
      case L7_DISABLE:
        osapiSnprintf(buf, sizeof(buf), "Disabled");
        break;
      default:
        osapiSnprintf(buf,sizeof(buf), "N/A");
        break;
      }
      printf("  MAB Enabled:        %s\n", buf);
      printf("\n");
    }
  }

  printf("=============\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build default dot1x config data
*
* @param    ver @((input)) Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1xBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_DOT1X_INTF_MAX_COUNT];


  /* Save the config IDs */
  memset((void *)&configId[0], 0, sizeof(nimConfigID_t) * L7_DOT1X_INTF_MAX_COUNT);
  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &dot1xCfg->dot1xPortCfg[cfgIndex].configId);

  memset((void *)dot1xCfg, 0, sizeof(dot1xCfg_t));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT; cfgIndex++)
  {
    dot1xBuildDefaultIntfConfigData(&configId[cfgIndex], &dot1xCfg->dot1xPortCfg[cfgIndex]);

#if L7_FEAT_DOT1X_SUPPLICANT_CONTROL_ON_AUTH_PORTS
    if (cfgIndex == FD_DOT1X_WAN_PORT) /* WAN Port Info, Need to get this from dot1ad */
    {
      dot1xCfg->dot1xPortCfg[cfgIndex].paeCapabilities = L7_DOT1X_PAE_PORT_SUPP_CAPABLE;
    }
#endif
    dot1xBuildDefaultSupplicantIntfConfigData(&configId[cfgIndex], &dot1xCfg->dot1xPortCfg[cfgIndex]);
  }

  strcpy((L7_char8 *)dot1xCfg->cfgHdr.filename, DOT1X_CFG_FILENAME);

  dot1xCfg->cfgHdr.version = ver;

  dot1xCfg->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;

  dot1xCfg->cfgHdr.type = L7_CFG_DATA;

  dot1xCfg->cfgHdr.length = (L7_uint32)sizeof(dot1xCfg_t);

  dot1xCfg->cfgHdr.dataChanged = L7_FALSE;

  dot1xCfg->dot1xAdminMode    = FD_DOT1X_ADMIN_MODE;
  dot1xCfg->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;
  dot1xCfg->vlanAssignmentMode = FD_DOT1X_VLAN_ASSIGN_MODE;
  dot1xCfg->dynamicVlanMode = FD_DOT1X_DYNAMIC_VLAN_CREATION_MODE;

  return;
}

/*********************************************************************
* @purpose  Build default dot1x port config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void dot1xBuildDefaultIntfConfigData(nimConfigID_t *configId, dot1xPortCfg_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  pCfg->paeCapabilities = FD_DOT1X_PORT_PAE_CAPABILITIES;

  pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
  pCfg->portControlMode = FD_DOT1X_PORT_MODE;
  pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
  pCfg->guestVlanPeriod = FD_DOT1X_PORT_GUEST_PERIOD;
  pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;

  if (pCfg->guestVlanId != 0)
  {
    dot1xGuestVlanIntfCount[pCfg->guestVlanId]--;
    if (dot1xGuestVlanIntfCount[pCfg->guestVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(dot1xGuestVlanMask, pCfg->guestVlanId);
    }
  }

  pCfg->guestVlanId =  FD_DOT1X_PORT_GUEST_VLAN;
  pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
  pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
  pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
  pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
  pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
  pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
  pCfg->guestVlanId = FD_DOT1X_PORT_GUEST_VLAN;
  pCfg->guestVlanPeriod = FD_DOT1X_PORT_GUEST_PERIOD;
  pCfg->maxUsers = FD_DOT1X_PORT_MAX_USERS;
  pCfg->unauthenticatedVlan = FD_DOT1X_PORT_UNAUTHENTICATED_VLAN;
  pCfg->mabEnabled = FD_DOT1X_PORT_MAB_ENABLED;

  dot1xCfg->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build default dot1x Supplicant port config data
*
* @parms    config Id, the config Id to be placed into the intf config
* @parms    pCfg, a pointer to the interface structure
*
* @returns  none
*
*
* @end
*********************************************************************/
void dot1xBuildDefaultSupplicantIntfConfigData(nimConfigID_t *configId, dot1xPortCfg_t *pCfg)
{

  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  /* Supplicant Parametes configuration */
  pCfg->supplicantPortCfg.adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
  pCfg->supplicantPortCfg.portControlMode = FD_DOT1X_SUPPLICANT_PORT_MODE;
  pCfg->supplicantPortCfg.maxStart = FD_DOT1X_SUPPLICANT_PORT_MAX_START;
  pCfg->supplicantPortCfg.startPeriod = FD_DOT1X_SUPPLICANT_PORT_START_PERIOD;
  pCfg->supplicantPortCfg.heldPeriod = FD_DOT1X_SUPPLICANT_PORT_HELD_PERIOD;
  pCfg->supplicantPortCfg.authPeriod = FD_DOT1X_SUPPLICANT_PORT_AUTH_PERIOD;
  dot1xCfg->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Apply dot1x config data
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
L7_RC_t dot1xApplyConfigData(void)
{
  return (dot1xIssueCmd(dot1xMgmtApplyConfigData, L7_NULL, L7_NULLPTR));
}

/*********************************************************************
* @purpose  Apply dot1x config data to specified interface
*
* @param    intIfNum     @b{(input)) internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xApplyPortConfigData(L7_uint32 intIfNum)
{
  return (dot1xIssueCmd(dot1xMgmtApplyPortConfigData, L7_NULL, L7_NULLPTR));
}

/*********************************************************************
* @purpose  Fill in default values and set the port state
*
* @param    intIfNum  @b{(input))  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xPortReset(L7_uint32 intIfNum)
{
  L7_uint32 lIndex;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,intIfNum,"%s:Intialize Physical port-%d \n",
                      __FUNCTION__,intIfNum);

  /*dot1xPortInfoInitialize(intIfNum,L7_FALSE);*/

  lIndex = DOT1X_LOGICAL_PORT_ITERATE;
  while((logicalPortInfo=dot1xLogicalPortInfoGetNextNode(intIfNum,&lIndex))!= L7_NULLPTR)
  {
    dot1xLogicalPortReset(logicalPortInfo);
  }

  dot1xPortInfoInitialize(intIfNum,L7_FALSE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize the Dot1x Port Structure with Default Values
*
* @param    intIfNum  @b{(input))  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xPortInfoInitialize(L7_uint32 intIfNum,L7_BOOL flag)
{
  L7_uint32 linkState, adminState,lIndex;
  dot1xLogicalPortInfo_t *logicalPortInfo;

  dot1xPortInfo[intIfNum].paeProtocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  dot1xPortInfo[intIfNum].paeCapabilities    = L7_DOT1X_PAE_PORT_AUTH_CAPABLE;
  dot1xPortInfo[intIfNum].maxUsers           = DOT1X_MAX_USERS_PER_PORT;
  dot1xPortInfo[intIfNum].numUsers           = 0;

  dot1xPortInfo[intIfNum].currentId          = 0;
  dot1xPortInfo[intIfNum].initialize         = L7_FALSE;

  if ( (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS) && (linkState == L7_UP) &&
       (nimGetIntfAdminState(intIfNum, &adminState) == L7_SUCCESS) && (adminState == L7_ENABLE) )
    dot1xPortInfo[intIfNum].portEnabled = L7_TRUE;
  else
    dot1xPortInfo[intIfNum].portEnabled = L7_FALSE;

  dot1xPortInfo[intIfNum].keyAvailable = L7_FALSE;
  dot1xPortInfo[intIfNum].reAuthMax    = dot1xPortInfo[intIfNum].maxReq;

  dot1xPortInfo[intIfNum].operControlledDirections = dot1xPortInfo[intIfNum].adminControlledDirections;
  dot1xPortInfo[intIfNum].bridgeDetected           = L7_FALSE;
  dot1xPortInfo[intIfNum].authCount                = 0;
  dot1xPortInfo[intIfNum].portStatus               = L7_DOT1X_PORT_STATUS_UNAUTHORIZED;

  dot1xPortInfo[intIfNum].portMacAddrAdded         = L7_FALSE;
  dot1xPortInfo[intIfNum].mabEnabled               = L7_DISABLE;

  if(flag == L7_TRUE)
  {
    lIndex = DOT1X_LOGICAL_PORT_ITERATE;
    while((logicalPortInfo= dot1xLogicalPortInfoGetNextNode(intIfNum,&lIndex))!=L7_NULL)
    {
      dot1xLogicalPortInfoInitialize(logicalPortInfo);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Saves dot1x debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot1xDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugSave(void)
{
  L7_RC_t rc;

  rc = L7_SUCCESS;

  if (dot1xDebugHasDataChanged() == L7_TRUE)
  {
    /* Copy the operational states into the config file */
    dot1xDebugCfgUpdate();

    /* Store the config file */
    dot1xDebugCfg.hdr.dataChanged = L7_FALSE;
    dot1xDebugCfg.checkSum = nvStoreCrc32((L7_uchar8 *)&dot1xDebugCfg,
        (L7_uint32)(sizeof(dot1xDebugCfg) - sizeof(dot1xDebugCfg.checkSum)));
    /* call save NVStore routine */
    if ((rc = sysapiSupportCfgFileWrite(L7_DOT1X_COMPONENT_ID, DOT1X_DEBUG_CFG_FILENAME,
            (L7_char8 *)&dot1xDebugCfg, (L7_uint32)sizeof(dot1xDebugCfg_t))) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
              "Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",DOT1X_DEBUG_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Restores dot1x debug configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot1xDebugCfg is the overlay
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugRestore(void)
{
  L7_RC_t rc;
  dot1xDebugBuildDefaultConfigData(DOT1X_DEBUG_CFG_VER_CURRENT);

  dot1xDebugCfg.hdr.dataChanged = L7_TRUE;

  rc = dot1xApplyDebugConfigData();

  return rc;
}

/*********************************************************************
* @purpose  Checks if dot1x debug config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1xDebugHasDataChanged(void)
{
  return dot1xDebugCfg.hdr.dataChanged;
}

/*********************************************************************
* @purpose  Build default dot1x config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1xDebugBuildDefaultConfigData(L7_uint32 ver)
{

  /* setup file header */
  dot1xDebugCfg.hdr.version = ver;
  dot1xDebugCfg.hdr.componentID = L7_DOT1X_COMPONENT_ID;
  dot1xDebugCfg.hdr.type = L7_CFG_DATA;
  dot1xDebugCfg.hdr.length = (L7_uint32)sizeof(dot1xDebugCfg);
  strcpy((L7_char8 *)dot1xDebugCfg.hdr.filename, DOT1X_DEBUG_CFG_FILENAME);
  dot1xDebugCfg.hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&dot1xDebugCfg.cfg, 0, sizeof(dot1xDebugCfg.cfg));
}

/*********************************************************************
* @purpose  Apply dot1x debug config data
*
* @param    void
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @notes    Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t dot1xApplyDebugConfigData(void)
{
  L7_RC_t rc;

  rc = dot1xDebugPacketTraceFlagSet(dot1xDebugCfg.cfg.dot1xDebugPacketTraceTxFlag,dot1xDebugCfg.cfg.dot1xDebugPacketTraceRxFlag);

  return rc;
}

/*********************************************************************
* @purpose  Set values of the Logical Dot1x Port Structure
*           with Default Values of port it belongs to
*
* @param    logicalPortInfo  @b{(input))  Logical port Info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoSetPortInfo(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32 physPort;
  physPort = logicalPortInfo->physPort;

  /* Timers */
  logicalPortInfo->aWhile             = 0; /* suppTimeout or serverTimeout */
  logicalPortInfo->quietWhile         = dot1xPortInfo[physPort].quietPeriod;
  logicalPortInfo->reAuthWhen         = dot1xPortInfo[physPort].reAuthPeriod;
  logicalPortInfo->txWhenLP           = dot1xPortInfo[physPort].txPeriod;
  DOT1X_EVENT_TRACE(DOT1X_TRACE_MAB,physPort,
                        "%s:Resetting guest vlan timer for linterface = %d . \n",
                        __FUNCTION__,logicalPortInfo->logicalPortNumber);
  logicalPortInfo->guestVlanTimer     = 0;

  /* Inter state machine communication */
  logicalPortInfo->authAbort          = L7_FALSE;
  logicalPortInfo->authFail           = L7_FALSE;
  logicalPortInfo->authStart          = L7_FALSE;
  logicalPortInfo->authTimeout        = L7_FALSE;
  logicalPortInfo->authSuccess        = L7_FALSE;
  logicalPortInfo->currentIdL         = dot1xPortInfo[physPort].currentId;
  logicalPortInfo->reAuthenticate     = L7_FALSE;

  /* Authenticator PAE FSM */
  logicalPortInfo->eapLogoff   = L7_FALSE;
  logicalPortInfo->eapStart    = L7_FALSE;
  logicalPortInfo->reAuthCount = 0;
  logicalPortInfo->rxRespId    = L7_FALSE;

  /* BAM FSM */
  logicalPortInfo->reqCount     = 0;
  logicalPortInfo->rxResp       = L7_FALSE;
  logicalPortInfo->idFromServer = 0;

  /* Rx Key FSM */
  logicalPortInfo->rxKey = L7_FALSE;

  /* FSM state Holders */
  logicalPortInfo->apmState = APM_INITIALIZE;
  logicalPortInfo->ktxState = KTX_NO_KEY_TRANSMIT;
  logicalPortInfo->rtmState = RTM_INITIALIZE;
  logicalPortInfo->bamState = BAM_INITIALIZE;
  logicalPortInfo->krxState = KRX_NO_KEY_RECEIVE;
  logicalPortInfo->logicalPortStatus = 0;

  /* User Details */
  memset(logicalPortInfo->dot1xUserName,0,DOT1X_USER_NAME_LEN);
  logicalPortInfo->dot1xUserNameLength = 0;
  logicalPortInfo->dot1xUserIndex = DOT1X_USER_INDEX_INVALID;
  memset(logicalPortInfo->dot1xChallenge,0, DOT1X_CHALLENGE_LEN);
  logicalPortInfo->dot1xChallengelen =0;
  memset(logicalPortInfo->serverState,0, DOT1X_SERVER_STATE_LEN);
  logicalPortInfo->serverStateLen = 0;
  memset(logicalPortInfo->serverClass,0,DOT1X_SERVER_CLASS_LEN);
  logicalPortInfo->serverClassLen = 0;
  logicalPortInfo->sessionTimeout = 0;
  logicalPortInfo->terminationAction = RADIUS_TERMINATION_ACTION_DEFAULT;
  logicalPortInfo->cnfgrEapolMacFlag = L7_FALSE;
  logicalPortInfo->suppBufHandle = L7_NULL;
  memset(&logicalPortInfo->suppMacAddr,0,sizeof(L7_enetMacAddr_t));
  logicalPortInfo->vlanId = 0;
  logicalPortInfo->authMethod = L7_AUTH_METHOD_UNDEFINED;
  logicalPortInfo->vlanAssigned = 0;
  memset(logicalPortInfo->filterName,0,DOT1X_FILTER_NAME_LEN);
  logicalPortInfo->unauthVlan = 0;
  logicalPortInfo->reauth_auth_apply = L7_FALSE;
  logicalPortInfo->clientTimeout = 0;
  logicalPortInfo->guestVlanId = 0;
  logicalPortInfo->voiceVlanId = 0;
  logicalPortInfo->isMABClient = L7_FALSE;
  logicalPortInfo->blockVlanId = 0;
  logicalPortInfo->defaultVlanId = 0;
  logicalPortInfo->isMonitorModeClient = L7_FALSE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize the Logical Dot1x Port Structure
*           with Default Values
*
* @param    logicalPortInfo  @b{(input))  Logical Port Info node
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortInfoInitialize(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_uint32 intIfNum = logicalPortInfo->physPort;
  L7_RC_t rc= L7_FAILURE;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,intIfNum,
                    "%s intf-%d and logical - %d \n",__FUNCTION__,intIfNum,
                    logicalPortInfo->logicalPortNumber);

  /*remove supplicant mac address from Mac address Database*/
   dot1xMacAddrInfoRemove(&(logicalPortInfo->suppMacAddr));

  if (logicalPortInfo->inUse == L7_TRUE)
  {
    if(dot1xPortInfo[intIfNum].numUsers > 0)
    {
        dot1xPortInfo[intIfNum].numUsers--;
    }
    rc = dot1xLogicalPortInfoDeAlloc(logicalPortInfo);
  }

  if(rc != L7_SUCCESS)
  {
    logicalPortInfo->inUse = L7_FALSE;
    dot1xLogicalPortInfoSetPortInfo(logicalPortInfo);
    if (dot1xPortInfo[intIfNum].portControlMode == L7_DOT1X_PORT_AUTO)
    {
        dot1xPortInfo[intIfNum].portMacAddrAdded = L7_FALSE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the physical port for the logical interface
*
* @param    intIfNum  @b{(input))  internal interface number
*
* @returns  end index for specified interface in dot1x logical
*           interface list
*
* @comments
*
* @end
*********************************************************************/
L7_uint32 dot1xPhysPortGet(L7_uint32 intIfNum)
{
  L7_uint32 physPort;
  physPort = DOT1X_PHYSICAL_PORT_GET(intIfNum);
  return physPort;
}


/*********************************************************************
* @purpose  Fill in default values and set the logical port state
*
* @param    logicalPortInfo  @b{(input))  Logical port Info
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xLogicalPortReset(dot1xLogicalPortInfo_t *logicalPortInfo)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 physPort,lIntIfNum;

  physPort = logicalPortInfo->physPort;
  lIntIfNum = logicalPortInfo->logicalPortNumber;

  DOT1X_EVENT_TRACE(DOT1X_TRACE_API_CALLS,physPort,"%s:Restting Logical port-%d \n",
                      __FUNCTION__,lIntIfNum);

  if(logicalPortInfo->inUse == L7_TRUE)
  {
    if (logicalPortInfo->blockVlanId != 0)
    {
      DOT1X_EVENT_TRACE(DOT1X_TRACE_EXTERNAL,physPort,
                        "%s:%d Unblocking the Client %02x:%02x:%02x:%02x:%02x:%02x with vlan id- %d \n",
                        __FUNCTION__,__LINE__,
                        logicalPortInfo->suppMacAddr.addr[0],
                        logicalPortInfo->suppMacAddr.addr[1],
                        logicalPortInfo->suppMacAddr.addr[2],
                        logicalPortInfo->suppMacAddr.addr[3],
                        logicalPortInfo->suppMacAddr.addr[4],
                        logicalPortInfo->suppMacAddr.addr[5],
                        logicalPortInfo->blockVlanId);
      dtlDot1xIntfClientUnblock(physPort,logicalPortInfo->suppMacAddr,logicalPortInfo->blockVlanId);
      logicalPortInfo->blockVlanId = 0;
    }
    /* If admin mode is enabled, set the appropriate port status */
    if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
    {
      if (dot1xPortInfo[physPort].portControlMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)
        rc = dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_AUTHORIZED);
      else
      {
            rc = dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED);
            /*remove supplicant mac address from Mac address Database*/
           /* dot1xMacAddrInfoRemove(&(logicalPortInfo->suppMacAddr));*/
      }
    }
    else
    {
      /* Disable the Guest vlan timer */
      dot1xCtlPortGuestVlanTimerStop(lIntIfNum);

      /* If dot1x admin mode is disabled, set port to authorized */
      rc = dot1xIhPortStatusSet(lIntIfNum, L7_DOT1X_PORT_STATUS_AUTHORIZED);
    }
  }
  dot1xLogicalPortInfoInitialize(logicalPortInfo);

  return rc;
}

/* Supplicant APIs*/

/*********************************************************************
* @purpose  Initialize the Dot1x Port Structure with Default Values
*
* @param    intIfNum  @b{(input))  internal interface number
* @param    flag      @b{(input))  Flag to indicate whether to copy
*                     Default config or not
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantPortInfoInitialize(L7_uint32 intIfNum,
                                                        L7_BOOL flag)
{
  dot1xPortCfg_t *pCfg;
  L7_uint32 linkState, adminState;

  if (dot1xIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* PAE state machine parameters */
  dot1xSupplicantPortInfo[intIfNum].paeProtocolVersion = L7_DOT1X_PAE_PORT_PROTOCOL_VERSION_1;
  dot1xSupplicantPortInfo[intIfNum].startCount = 0;
  dot1xSupplicantPortInfo[intIfNum].spmState = L7_DOT1X_SPM_DISCONNECTED;
  dot1xSupplicantPortInfo[intIfNum].sbmState = L7_DOT1X_SBM_IDLE;

  if ( (nimGetIntfLinkState(intIfNum, &linkState) == L7_SUCCESS) && (linkState == L7_UP) &&
       (nimGetIntfAdminState(intIfNum, &adminState) == L7_SUCCESS) && (adminState == L7_ENABLE) )

    dot1xSupplicantPortInfo[intIfNum].portEnabled = L7_TRUE;
  else
    dot1xSupplicantPortInfo[intIfNum].portEnabled = L7_FALSE;


  /* Supplicant Timers */
  dot1xSupplicantPortInfo[intIfNum].startWhen = pCfg->supplicantPortCfg.startPeriod;
  dot1xSupplicantPortInfo[intIfNum].heldWhile = pCfg->supplicantPortCfg.heldPeriod;
  dot1xSupplicantPortInfo[intIfNum].authWhile = pCfg->supplicantPortCfg.authPeriod;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Fill in default values and set the Supplicant port state
*
* @param    intIfNum  @b{(input))  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1xSupplicantPortReset(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  if (dot1xSupplicantPortInfoInitialize(intIfNum, L7_FALSE) == L7_SUCCESS)
  {
    /* If admin mode is enabled, set the appropriate port status */
    if (dot1xCfg->dot1xAdminMode == L7_ENABLE)
    {
      if (dot1xSupplicantPortInfo[intIfNum].sPortMode == L7_DOT1X_PORT_FORCE_AUTHORIZED)
        rc = dot1xIhPhyPortStatusSet(intIfNum, L7_DOT1X_PORT_STATUS_AUTHORIZED,L7_TRUE);
      else
        rc =dot1xIhPhyPortStatusSet (intIfNum, L7_DOT1X_PORT_STATUS_UNAUTHORIZED,L7_TRUE);
    }
    else
    {
      /* If dot1x admin mode is disabled, set port to authorized */
      rc = dot1xIhPhyPortStatusSet (intIfNum, L7_DOT1X_PORT_STATUS_AUTHORIZED,L7_TRUE);
    }
  }

  return rc;
}


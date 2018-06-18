/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv.c
*
* @purpose    DiffServ Component system infrastructure
*
* @component  DiffServ
*
* @comments   This file contains DiffServ init/config functions
*
* @create     04/09/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#include "l7_diffserv_include.h"
#include "platform_config.h"
#include "usmdb_mib_diffserv_private_api.h"

/* DiffServ component globals */
L7_diffServCfg_t        *pDiffServCfgData_g = L7_NULLPTR;
diffServInfo_t          *pDiffServInfo_g = L7_NULLPTR;
L7_uint32               *diffServMapTbl_g = L7_NULLPTR;
L7_diffServCfg_t        tempSaveCfgData_g;        /* for cfg file restoration */
diffServDeregister_t    diffServDeregister_g = { L7_TRUE, L7_TRUE, L7_TRUE, L7_TRUE };
PORTEVENT_MASK_t        diffServPortEventMask_g;


/*********************************************************************
* @purpose  Check if DiffServ user config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL diffServHasDataChanged(void)
{
  if (diffServDeregister_g.diffServHasDataChanged == L7_TRUE)
    return L7_FALSE;

  return pDiffServCfgData_g->cfgHdr.dataChanged;
}
void diffservResetDataChanged(void)
{
  pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Save DiffServ user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t diffServSave(void)
{
  L7_RC_t       rc = L7_SUCCESS;

  if (diffServDeregister_g.diffServSave == L7_TRUE)
    return L7_SUCCESS;

  if (pDiffServCfgData_g->cfgHdr.dataChanged == L7_TRUE)
  {
    /* save current local config file contents in case of problem */
    memcpy(&tempSaveCfgData_g, pDiffServCfgData_g, L7_DIFFSERV_CFG_DATA_SIZE);

    do
    {
      /* build user config data from private MIB */
      if (diffServBuildUserConfigData(L7_DIFFSERV_CFG_VER_CURRENT,
                                      pDiffServCfgData_g) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s %d: %s : Error while building contents of config file %s\n",
                __FILE__, __LINE__, __FUNCTION__, L7_DIFFSERV_CFG_FILENAME);
        rc = L7_FAILURE;
        break;
      }
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;

      pDiffServCfgData_g->checkSum = nvStoreCrc32((L7_uchar8 *)pDiffServCfgData_g,
                                                  ((L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE -
                                                    (L7_uint32)sizeof(pDiffServCfgData_g->checkSum)));


      if (sysapiCfgFileWrite(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                             L7_DIFFSERV_CFG_FILENAME,
                             (L7_char8 *)pDiffServCfgData_g,
                             (L7_uint32)sizeof(L7_diffServCfg_t)) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s %d: %s : Error on call to osapiFsWrite routine for config "
                "file %s\n", __FILE__, __LINE__, __FUNCTION__,
                L7_DIFFSERV_CFG_FILENAME);
        rc = L7_FAILURE;
        break;
      }

    } while ( 0 );

    /* restore previous local config file contents if save was unsuccessful */
    if (rc != L7_SUCCESS)
      memcpy(pDiffServCfgData_g, &tempSaveCfgData_g, L7_DIFFSERV_CFG_DATA_SIZE);

  } /* endif data changed */

  return rc;
}

/*********************************************************************
* @purpose  Restores DiffServ user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t diffServRestore(void)
{
  L7_uint32     dataChanged = L7_FALSE;
  L7_RC_t       rc;

  if (diffServDeregister_g.diffServRestore == L7_TRUE)
    return L7_SUCCESS;

  /*------------------------------------------------------------------------
   * NOTE:  The local config file always represents the last saved config.
   *        Since it is used globally during the config 'apply' functions,
   *        we must save/restore its current contents here.
   *
   *        If/when the user chooses to save the factory default config
   *        that has just been created, the local config file will be
   *        updated for real at that time.
   *------------------------------------------------------------------------
   */

  /* save current local config file contents */
  memcpy(&tempSaveCfgData_g, pDiffServCfgData_g, L7_DIFFSERV_CFG_DATA_SIZE);

  rc = diffServResetCleanUp();
  if (rc == L7_SUCCESS)
  {
    diffServBuildDefaultConfigData(L7_DIFFSERV_CFG_VER_CURRENT);
    dataChanged = L7_TRUE;

    rc = diffServConfigDataApply();
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s %d: %s: Failed to reset DiffServ. Recommend resetting device."
            " While attempting to clear the running configuration an error was "
            "encountered in removing the current settings.  This may lead to an "
            "inconsistent state in the system and resetting is advised.",
            __FILE__, __LINE__, __FUNCTION__);
  }

  /* always restore saved original local config file contents */
  memcpy(pDiffServCfgData_g, &tempSaveCfgData_g, L7_DIFFSERV_CFG_DATA_SIZE);
  pDiffServCfgData_g->cfgHdr.dataChanged = dataChanged;

  return rc;
}

/*********************************************************************
* @purpose  Replaces DiffServ operational config in its entirety
*
* @param    *pCfgData   @b{(input)} Ptr to DiffServ config data
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    This function is intended for use when receiving DiffServ
*           config that was pushed from another switch in the cluster.
*           This operation is initiated by the clustering component
*           instead of the Configurator, hence no 'deregistration' flag
*           is defined/used.
*
* @end
*********************************************************************/
L7_RC_t diffServReplace(L7_diffServCfg_t *pCfgData)
{
  L7_uint32     dataChanged;
  L7_RC_t       rc;

  if (L7_NULLPTR == pCfgData)
    return L7_FAILURE;

  /*------------------------------------------------------------------------
   * NOTE:  The local config file always represents the last-saved config.
   *        Since it is used globally during the config 'apply' functions,
   *        we must save/restore its current contents here.
   *
   *        If/when the new operational config gets saved, the local
   *        config file will be updated for real at that time.
   *------------------------------------------------------------------------
   */

  /* save current local config file contents */
  memcpy(&tempSaveCfgData_g, pDiffServCfgData_g, L7_DIFFSERV_CFG_DATA_SIZE);
  dataChanged = pDiffServCfgData_g->cfgHdr.dataChanged;

  rc = diffServResetCleanUp();
  if (L7_SUCCESS == rc)
  {
    /* temporarily copy the new config into the last-saved config global struct
     * and apply it as the operational config
     */
    memcpy(pDiffServCfgData_g, pCfgData, L7_DIFFSERV_CFG_DATA_SIZE);
    dataChanged = L7_TRUE;

    rc = diffServConfigDataApply();
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Failed clearing DiffServ config. The device may need to be reset.\n",
            __FUNCTION__);
  }

  /* always restore last-saved original local config file contents */
  memcpy(pDiffServCfgData_g, &tempSaveCfgData_g, L7_DIFFSERV_CFG_DATA_SIZE);
  pDiffServCfgData_g->cfgHdr.dataChanged = dataChanged;

  return rc;
}

/*********************************************************************
* @purpose  Build default DiffServ intf config data
*
* @parms    configId  the configId to be put into the structure
* @parms    pCfg      points to the interface structure
*
* @returns  void
*
* @notes    There may be times when the user wants the configId to
*           remain the same by passing (&pCfg->configId) as the configId
*           the configId to make sure when we clear out pCfg that the
*           configId is not lost in that case.
*
* @end
*********************************************************************/
void diffServBuildDefaultIntfConfigData(nimConfigID_t *configId, L7_diffServIntfCfg_t *pCfg)
{

  /* Since the configId is the first thing in the interface structure
   * we can skip it when zeroing out the rest of the interface structure
   */
  memset(pCfg , 0, sizeof(L7_diffServIntfCfg_t));
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);
}

/*********************************************************************
* @purpose  Build default DiffServ config data
*
* @param    ver         @b{(input)} Software version of Config Data
*
* @returns  void
*
* @notes    Just builds the config file contents, but does not apply
*           it to the active configuration.
*
* @end
*********************************************************************/
void diffServBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t configId[L7_DIFFSERV_INTF_MAX_COUNT];

  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_DIFFSERV_INTF_MAX_COUNT);

  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &pDiffServCfgData_g->diffServIntfCfg[cfgIndex].configId);

  memset(pDiffServCfgData_g, 0, L7_DIFFSERV_CFG_DATA_SIZE);

  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
    diffServBuildDefaultIntfConfigData(&configId[cfgIndex], &pDiffServCfgData_g->diffServIntfCfg[cfgIndex]);

  /* build config file header */
  strcpy((char *)pDiffServCfgData_g->cfgHdr.filename, L7_DIFFSERV_CFG_FILENAME);
  pDiffServCfgData_g->cfgHdr.version       = ver;
  pDiffServCfgData_g->cfgHdr.componentID   = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  pDiffServCfgData_g->cfgHdr.type          = L7_CFG_DATA;
  pDiffServCfgData_g->cfgHdr.length        = (L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE;
  pDiffServCfgData_g->cfgHdr.dataChanged   = L7_FALSE;

  /* build default config data */
  pDiffServCfgData_g->adminMode = FD_QOS_DIFFSERV_ADMIN_MODE;
  pDiffServCfgData_g->traceMode = FD_QOS_DIFFSERV_TRACE_MODE;
  pDiffServCfgData_g->msgLvl    = FD_QOS_DIFFSERV_MSG_LVL;
}

/*********************************************************************
* @purpose  Applies DiffServ config data
*
* @param    void
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    This routine takes the current DiffServ configuration
*           file and expands it into the active configuration for
*           the system.
*
* @end
*********************************************************************/
L7_RC_t diffServConfigDataApply(void)
{
  L7_RC_t       rc = L7_FAILURE;

  if (diffServIsReady() != L7_TRUE)
    return L7_SUCCESS;

  /* enter DiffServ special config mode to relax some internal checking rules */
  dsmibSpecialConfigModeOn();

  do
  {

    /* disable admin mode in the active config before setting things up */
    if (diffServAdminModeSet(L7_DISABLE) != L7_SUCCESS)
      break;

    /* global config */
    if (diffServGlobalConfig() != L7_SUCCESS)
      break;

    /* class configuration */
    if (diffServClassConfig() != L7_SUCCESS)
      break;

    /* policy configuration */
    if (diffServPolicyConfig() != L7_SUCCESS)
      break;

    if (DIFFSERV_IS_READY)
    {
      /* service configuration */
      if (diffServServiceConfig() != L7_SUCCESS)
        break;
    }

    rc = L7_SUCCESS;

  } while (0);

  /* exit DiffServ special config mode */
  dsmibSpecialConfigModeOff();

  /* check for any class reference rule chain length violations
   * (this was bypassed during special config mode so that it
   * would only need to be run once here)
   */
  if (rc == L7_SUCCESS)
  {
    rc = dsmibClassRuleRefLimitCheck();
  }

  /* now turn off tracing if it is disabled in the configuration */
  if (DS_TRACE_IS_ENABLED() != L7_TRUE)
    dsTraceModeApply(L7_DISABLE);

  if (rc == L7_SUCCESS)
  {
    /* enable the admin mode if it was originally enabled
     *
     * NOTE:  Must do this AFTER turning off special config mode so that
     *        policy activation can take effect.
     */
    if (pDiffServCfgData_g->adminMode == L7_ENABLE)
      rc = diffServAdminModeSet(L7_ENABLE);

    /* send initial callback notifications for all configured policies */
    diffServCallbackPolicyAllSend();
  }

  return rc;
}

/*********************************************************************
* @purpose  Applies DiffServ administrative mode change
*
* @param    mode        @b{(input)} Mode value (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function triggers DiffServ operation in accordance
*           with the specified mode.  It expects the caller to
*           determine whether an actual mode change has occurred or not.
*
* @end
*********************************************************************/
L7_RC_t diffServAdminModeApply(L7_uint32 mode)
{
  /* to support pre-configuration, return w/success if component not ready */
  if (diffServIsReady() != L7_TRUE)
    return L7_SUCCESS;

  /* notify Distiller of latest admin mode */
  return dsDistillerAdminModeChange(mode);
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
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t diffServIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  L7_diffServIntfCfg_t *pCfg;
  L7_uint32 i;

  if (diffServIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;


  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_DIFFSERV_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pDiffServCfgData_g->diffServIntfCfg[i].configId, &configId))
    {
      diffServMapTbl_g[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if (diffServMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if (pCfg != L7_NULL)
    {
      diffServBuildDefaultIntfConfigData(&configId, pCfg);
      /*NIM_CONFIG_ID_COPY(&pCfg->configId, &configId); */
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To copy the interface configuration data to the MIB
*           structure
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t diffServApplyIntfConfigToMIB(L7_uint32 intIfNum)
{
  L7_RC_t               rc = L7_FAILURE;
  L7_uint32             prevDataChanged;
  L7_diffServIntfCfg_t  *pCfg;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    prevDataChanged = pDiffServCfgData_g->cfgHdr.dataChanged;

    /* enter DiffServ special config mode to relax some internal checking rules */
    dsmibSpecialConfigModeOn();

    rc = diffServServiceIntfConfig(intIfNum);

    /* exit DiffServ special config mode */
    dsmibSpecialConfigModeOff();

    /* restore dataChanged flag to previous value since service intf config
     * has not really changed yet
     */
    pDiffServCfgData_g->cfgHdr.dataChanged = prevDataChanged;

    if (rc != L7_SUCCESS)
    {
      /* remove the configuration from this interface and build defaults */
      rc = diffServIntfDetach(intIfNum, L7_TRUE);

      if (rc == L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s: Error activating saved config for intf %s -- using "
                "factory defaults instead\n", __FUNCTION__, ifName);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s: Error activating saved config for intf %s -- attempt "
                "to use factory defaults also failed\n", __FUNCTION__, ifName);
      }
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface
*
* @param    intIfNum    @{(input)} internal interface number
* @param    event       @{(input)} interface change event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is typically used to update the device when config
*           such as for redirect and mirror interfaces come and go.
*
* @comments The intIfNum parm represents the potential target interface
*           for redirect or mirroring, not the current interface(s) to
*           which a DiffServ policy containing those attributes is applied.
*
* @end
*
*********************************************************************/
L7_RC_t diffServCollateralConfigApply(L7_uint32 intIfNum, L7_uint32 event)
{
  L7_RC_t                 rc;
  L7_uint32               adminMode;
  L7_uint32               policyIndex, policyInstIndex, policyAttrIndex;
  L7_uint32               targetIntIfNum;
  dsmibPolicyAttrType_t   attrType;
  nimConfigID_t           configId;
  L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t  oid;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* only interested in valid DiffServ interfaces */
  if (diffServIsValidIntf(intIfNum) != L7_TRUE)
    return L7_SUCCESS;

  diffServAdminModeGet(&adminMode);

  /* walk the policy attr table looking for any redirect or mirror
   * attributes (only need to find first matching attribute within each policy,
   * since that's enough to have the entire policy evaluated)
   */
  policyIndex = policyInstIndex = policyAttrIndex = 0;
  while (diffServPolicyAttrGetNext(policyIndex, policyInstIndex, policyAttrIndex,
                                   &policyIndex, &policyInstIndex, &policyAttrIndex)
         == L7_SUCCESS)
  {
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE,
                                    &attrType) != L7_SUCCESS)
      continue;

    /* mirror and redirect are mutually-exclusive attributes within an instance */
    if (attrType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
      oid = L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF;
    else if (attrType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
      oid = L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF;
    else
      continue;

    /* NOTE:  Once we identify a mirror or redirect attribute, the next
     *        iteration begins with the next policy instance, or perhaps
     *        the next policy.
     */

    rc = diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                     oid, &configId);

    if (rc == L7_SUCCESS)
      rc = nimIntIfFromConfigIDGet(&configId, &targetIntIfNum);

    /* evaluate the policy if the specified interface is the mirror/redirect target */
    if ((rc == L7_SUCCESS) && (targetIntIfNum == intIfNum))
    {
      DIFFSERV_PRT(DIFFSERV_MSGLVL_LO_2,
                  "DiffServ: Found collateral policy %u,%u mirr/redir to intf %u, %s\n",
                  policyIndex, policyInstIndex, intIfNum, ifName);

      /* set 'reissueTlv' flag for this policy evaluation */
      if (dsDistillerPolicyEvaluate(policyIndex, adminMode, DSMIB_POLICY_EVAL_ALL_INTF,
                                    L7_TRUE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s: Error evaluating policy index %u (from target intf %s, event %u)\n",
                __FUNCTION__, policyIndex, ifName, event);
      }

      /* jump ahead to next policy index (regardless of evaluation outcome) */
      policyIndex++;
      policyInstIndex = 0;
      policyAttrIndex = 0;

      continue;
    }

    /* mirror/redirect attr not for target intf -- jump ahead to next instance */
    policyInstIndex++;
    policyAttrIndex = 0;

  } /* endwhile */

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_BOOL    noMsg     suppress error messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Apply default configuration on intIfNum
*
* @end
*
*********************************************************************/
L7_RC_t diffServIntfDetach(L7_uint32 intIfNum, L7_BOOL noMsg)
{
  L7_RC_t               rc;
  L7_uint32             ifDirection;
  L7_diffServIntfCfg_t  *pCfg;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    for (ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
        ifDirection <= L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
        ifDirection++)
    {
      rc = diffServServiceDelete(intIfNum, ifDirection);
      if ((rc != L7_SUCCESS) && (rc != L7_ERROR))
      {
        if (noMsg == L7_FALSE)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                  "%s: Error deleting service interface %s %s\n",
                  __FUNCTION__, ifName,
                  (char *)((ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ?
                    "inbound" : "outbound"));
        }
      }
    }

    diffServBuildDefaultIntfConfigData(&pCfg->configId, pCfg);
  }

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
* @comments Apply default configuration on intIfNum
*
* @end
*
*********************************************************************/
L7_RC_t diffServIntfDelete(L7_uint32 intIfNum)
{
  L7_RC_t               rc;
  L7_diffServIntfCfg_t  *pCfg;
  L7_uint32             ifDirection;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    for (ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
        ifDirection <= L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
        ifDirection++)
    {
      rc = diffServServiceDelete(intIfNum, ifDirection);
      if ((rc != L7_SUCCESS) && (rc != L7_ERROR))
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s: Error deleting service interface %s %s\n",
                __FUNCTION__, ifName,
                (char *)((ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ?
                  "inbound" : "outbound"));
      }
    }

    memset(&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset(&diffServMapTbl_g[intIfNum], 0, sizeof(L7_uint32));
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Callback function provided to NIM for interface change
*           notifications
*
* @param    intIfNum    @{(input)} Internal interface number
* @param    event       @{(input)} Interface change event
* @param    correlator  @{(input)} correlator for the event
*
* @returns  L7_SUCCESS
*
* @notes    NIM expects function to return an L7_uint32, not an L7_RC_t.
*
* @notes    This function hides any errors from NIM, logging a message instead.
*
* @notes    The DiffServ component assumes its callback event processing
*           can occur on the current execution thread (i.e., NIM has taken
*           proper measures to perform notifications outside of any
*           time-critical thread).
*
* @end
*********************************************************************/
L7_uint32 diffServIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator)
{
  L7_RC_t                   rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  status.intIfNum        = intIfNum;
  status.component       = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  status.event           = event;
  status.correlator      = correlator;
  status.response.rc     = rc;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (diffServDeregister_g.diffServIntfChange == L7_TRUE)
  {
    nimEventStatusCallback(status);
    return rc;
  }

  /* check configurator state readiness */
  if (DIFFSERV_IS_READY != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s called outside permitted phase (intf=%s, event=%u)\n",
            __FUNCTION__, ifName, event);
    nimEventStatusCallback(status);
    return rc;
  }

  /* check DiffServ internal ready state indicators */
  if (diffServIsReady() != L7_TRUE)
  {
    nimEventStatusCallback(status);
    return rc;
  }

  /* only interested in interfaces supported by DiffServ */
  if (diffServIsValidIntf(intIfNum) != L7_TRUE)
  {
    nimEventStatusCallback(status);
    return rc;
  }

  /* notify Distiller of all interface events */
  if (dsDistillerIntfEventNotify(intIfNum, event) != L7_SUCCESS)
  {
    /* log an error message, but continue processing the intf event */
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Error during distiller processing for intf %s, event %u\n",
            __FUNCTION__, ifName, event);
  }

  switch (event)
  {
  case L7_CREATE:
    rc = diffServIntfCreate(intIfNum);
    if (rc == L7_SUCCESS)
      rc = diffServApplyIntfConfigToMIB(intIfNum);
    break;

  case L7_ATTACH:
    DIFFSERV_PRT(DIFFSERV_MSGLVL_LO_2, "DiffServ: Attach %3u\n", intIfNum);
    rc = diffServCollateralConfigApply(intIfNum, event);
    break;

  case L7_DETACH:
    /* The policy should have been removed by the DsDistillerIntfEventNotify */
    DIFFSERV_PRT(DIFFSERV_MSGLVL_LO_2, "DiffServ: Detach %3u\n", intIfNum);
    rc = diffServCollateralConfigApply(intIfNum, event);
    break;

  case L7_DELETE:
    rc = diffServIntfDelete(intIfNum);
    break;

  case L7_LAG_ACQUIRE:
  case L7_PROBE_SETUP:
    /* NOTE:  Per agreement, the HAPI layer is responsible for retaining
     *        all DiffServ interface config data while the port is a
     *        member of a LAG.  The application continues to treat
     *        config changes normally, letting HAPI re-apply it to the
     *        port when it is released from the LAG.  LAG member ports are
     *        controlled by the DiffServ config of the LAG interface itself.
     */
    break;

  case L7_LAG_RELEASE:
  case L7_PROBE_TEARDOWN:
    /* NOTE:  Per agreement, the HAPI layer is responsible for retaining
     *        all DiffServ interface config data while the port is a
     *        member of a LAG.  The application continues to treat
     *        config changes normally, letting HAPI re-apply it to the
     *        port when it is released from the LAG.  LAG member ports are
     *        controlled by the DiffServ config of the LAG interface itself.
     */
    break;

  default:
    break;

  } /* endswitch */

  if (rc != L7_SUCCESS)
  {
    status.response.rc = rc;
    status.response.reason = NIM_ERR_RC_INTERNAL;
  }
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
* @purpose  Process NIM Startup notifications to DIFFSERV component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void diffServStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    switch (startupPhase)
    {
    case NIM_INTERFACE_CREATE_STARTUP:
      DIFFSERV_PRT(DIFFSERV_MSGLVL_LO_2, "diffServ: Create Startup %3u\n", intIfNum);
      if (diffServIntfCreate(intIfNum) != L7_SUCCESS)
      {
        DIFFSERV_PRT(DIFFSERV_MSGLVL_LO,"%s: Error during Create processing for intf %u\n",
                     __FUNCTION__, intIfNum);
      }
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      DIFFSERV_PRT(DIFFSERV_MSGLVL_LO_2, "diffServ: Activate Startup %3u\n", intIfNum);
      /* only interested in interfaces supported by DiffServ */
      if (diffServIsValidIntf(intIfNum) == L7_TRUE)
      {
        /* NOTE: The following two APIs need an event parameter,
         * these are not used in processing the call, rather just to
         * filter when to process.  Use L7_ACTIVE as it is accepted by these
         * APIs. */
        if (dsDistillerIntfEventNotify(intIfNum, L7_ACTIVE) != L7_SUCCESS)
        {
          DIFFSERV_PRT(DIFFSERV_MSGLVL_LO,"%s: Error during distiller processing for intf %u\n",
                       __FUNCTION__, intIfNum);
        }

        if (diffServCollateralConfigApply(intIfNum, L7_ACTIVE) != L7_SUCCESS)
        {
          DIFFSERV_PRT(DIFFSERV_MSGLVL_LO,"%s: Error during collateral config apply for intf %u\n",
                       __FUNCTION__, intIfNum);
        }
      }
      break;

    default:
      break;
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
  case NIM_INTERFACE_CREATE_STARTUP:
    /* Register with NIM to receive port CREATE/DELETE events */
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_CREATE);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_DELETE);
    nimRegisterIntfEvents(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, diffServPortEventMask_g);
    break;

  case NIM_INTERFACE_ACTIVATE_STARTUP:
    /* Add ATTACH/DETACH/etc. events to our NIM registration */
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_ACTIVE);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_INACTIVE);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_SPEED_CHANGE);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_ATTACH);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_DETACH);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_LAG_ACQUIRE);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_LAG_RELEASE);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_PROBE_SETUP);
    PORTEVENT_SETMASKBIT(diffServPortEventMask_g, L7_PROBE_TEARDOWN);
    nimRegisterIntfEvents(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, diffServPortEventMask_g);
    break;

  default:
    break;
  }

  nimStartupEventDone(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID);

  /* Inform cnfgr that protected port has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  }
}

/*********************************************************************
* @purpose  Determines if an interface has been acquired by another
*           component and is therefore unavailable to DiffServ.
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_TRUE    interface has been acquired
* @returns  L7_FALSE
*
* @comments The interface is only considered to be not available for
*           DiffServ purposes if it is acquired by any of these components:
*              - DOT3AD (LAGs)
*              - port mirroring
*
* @end
*********************************************************************/
L7_BOOL diffServIsIntfAcquired(L7_uint32 intIfNum)
{
  if ((mirrorIsActiveProbePort(intIfNum)) ||
      (dot3adIsLagActiveMember(intIfNum)))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to diffserv interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL diffServMapIntfIsConfigurable(L7_uint32 intIfNum, L7_diffServIntfCfg_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if (!(DIFFSERV_IS_READY))
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = diffServMapTbl_g[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(pDiffServCfgData_g->diffServIntfCfg[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between pDiffServCfgData_g and diffServMapTbl_g or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "Error accessing DIFFSERV config data for interface %s in diffServMapIntfIsConfigurable.\n", ifName);
      return L7_FALSE;
    }
  }

  if (L7_NULLPTR != pCfg)
    *pCfg = &pDiffServCfgData_g->diffServIntfCfg[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to diffserv interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL diffServMapIntfConfigEntryGet(L7_uint32 intIfNum, L7_diffServIntfCfg_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  static L7_uint32 nextIndex = 1;

  if ((DIFFSERV_IS_READY != L7_TRUE) ||
      (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS))
    return L7_FALSE;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Avoid N^2 processing when interfaces created at startup */
  if ((nextIndex < L7_DIFFSERV_INTF_MAX_COUNT) &&
      (NIM_CONFIG_ID_IS_EQUAL(&pDiffServCfgData_g->diffServIntfCfg[nextIndex].configId, &configIdNull) == L7_TRUE))
  {
    /* entry corresponding to nextIndex is free so use it, move speculative index to next entry
     * for next time
     */
    i = nextIndex++;
  }
  else
  /* Cached nextIndex is in use. Resort to search from beginning. N^2. */
  {
    for (i = 1; i < L7_DIFFSERV_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&pDiffServCfgData_g->diffServIntfCfg[i].configId, &configIdNull))
      {
        /* found a free entry, update the speculative index to next entry for next time */
        nextIndex = i+1;
        break;
      }
    }
  }

  if (i < L7_DIFFSERV_INTF_MAX_COUNT)
  {
    diffServMapTbl_g[intIfNum] = i;
    if (L7_NULLPTR != pCfg)
      *pCfg = &pDiffServCfgData_g->diffServIntfCfg[i];
    return L7_TRUE;
  }
  return L7_FALSE;
}


/*============================================================================*/
/*================= START OF CONFIG MIGRATION DEBUG CHANGES ==================*/
/*============================================================================*/

/*********************************************************************
* @purpose  Build test DiffServ intf config data
*
* @parms    configId  the configId to be put into the structure
* @parms    pCfg      points to the interface structure
*
* @returns  void
*
* @notes    There may be times when the user wants the configId to
*           remain the same by passing (&pCfg->configId) as the configId
*           to make sure when we clear out pCfg that the
*           configId is not lost in that case.
*
* @notes    The test config data consists of arbitrary non-default values.
*           This data can be checked for proper migration, but it will
*           not successfully apply to the device upon reboot since it
*           will not pass the built-in validity checking.
*
* @end
*********************************************************************/
void diffServBuildTestIntfConfigData(nimConfigID_t *configId, L7_diffServIntfCfg_t *pCfg)
{
  L7_diffServService_t  *pServ;
  dsmibServiceMib_t     *pMib;
  L7_uint32             intIfNum;

  /* Since the configId is the first thing in the interface structure
   * we can skip it when zeroing out the rest of the interface structure
   */
  memset(pCfg , 0, sizeof(L7_diffServIntfCfg_t));
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  if (nimIntIfFromConfigIDGet(configId, &intIfNum) != L7_SUCCESS)
    intIfNum = 1;

  /* set up inbound service parms */
  pServ = &pCfg->serviceIn;

  pServ->inUse = L7_TRUE;
  pServ->rowInvalidFlags = (dsmibServiceFlags_t)0x80000000; /* invalid */

  pMib = &pServ->mib;
  pMib->index = intIfNum;
  pMib->direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
  pMib->policyIndex = 2;
  pMib->ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP;
  pMib->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
  pMib->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;

  /* set up outbound service parms */
  pServ = &pCfg->serviceOut;

  pServ->inUse = L7_TRUE;
  pServ->rowInvalidFlags = (dsmibServiceFlags_t)0x80000000; /* invalid */

  pMib = &pServ->mib;
  pMib->index = intIfNum;
  pMib->direction = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
  pMib->policyIndex = 3;
  pMib->ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP;
  pMib->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
  pMib->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
}

/*********************************************************************
* @purpose  Build test DiffServ config data
*
* @param    void
*
* @returns  void
*
* @notes    Just builds the config file contents, but does not apply
*           it to the active configuration.
*
* @notes    The test config data consists of arbitrary non-default values.
*           This data can be checked for proper migration, but it will
*           not successfully apply to the device upon reboot since it
*           will not pass the built-in validity checking.
*
* @end
*********************************************************************/
void diffServBuildTestConfigData(void)
{
  L7_uint32             cfgIndex;
  nimConfigID_t         configId[L7_DIFFSERV_INTF_MAX_COUNT];
  L7_uint32             i, j;
  L7_uint32             classIndex, classRuleRowIndex;
  L7_uint32             policyIndex, policyInstIndex;
  L7_uint32             policyInstRowIndex, policyAttrRowIndex;
  L7_diffServCfg_t      *pCfg;
  dsmibClassMib_t       *pMibClass;
  dsmibClassRuleMib_t   *pMibClassRule;
  dsmibPolicyMib_t      *pMibPolicy;
  dsmibPolicyInstMib_t  *pMibPolicyInst;
  dsmibPolicyAttrMib_t  *pMibPolicyAttr;

  pCfg = pDiffServCfgData_g;

  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_DIFFSERV_INTF_MAX_COUNT);

  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &pCfg->diffServIntfCfg[cfgIndex].configId);

  memset(pCfg, 0, L7_DIFFSERV_CFG_DATA_SIZE);

  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
    diffServBuildTestIntfConfigData(&configId[cfgIndex], &pCfg->diffServIntfCfg[cfgIndex]);

  /* build config file header */
  osapiStrncpySafe(pCfg->cfgHdr.filename, L7_DIFFSERV_CFG_FILENAME, sizeof(pCfg->cfgHdr.filename));
  pCfg->cfgHdr.version       = L7_DIFFSERV_CFG_VER_CURRENT;
  pCfg->cfgHdr.componentID   = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  pCfg->cfgHdr.type          = L7_CFG_DATA;
  pCfg->cfgHdr.length        = (L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE;
  pCfg->cfgHdr.dataChanged   = L7_FALSE;

  /* build default config data */
  pCfg->adminMode = L7_DISABLE;
  pCfg->traceMode = L7_DISABLE;
  pCfg->msgLvl    = DIFFSERV_MSGLVL_OFF;

  /* build class hdr entries */
  for (i = L7_DIFFSERV_CLASS_HDR_START; i < L7_DIFFSERV_CLASS_HDR_MAX; i++)
  {
    pCfg->classHdr[i].inUse = L7_TRUE;
    pCfg->classHdr[i].chainIndex = ((i - 1) * L7_DIFFSERV_RULE_PER_CLASS_LIM) + 1;
    pCfg->classHdr[i].rowInvalidFlags = (dsmibClassFlags_t)0x80000000;  /* invalid */

    pMibClass = &pCfg->classHdr[i].mib;

    pMibClass->index = i;
    osapiSnprintf(pMibClass->name, sizeof(pMibClass->name), "testDiffServClass_%u", i);
    pMibClass->type = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL;
    pMibClass->aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC;   /* invalid */
    pMibClass->aclNum = 5;                                          /* invalid */
    pMibClass->ruleIndexNextFree = L7_DIFFSERV_RULE_PER_CLASS_LIM;  /* invalid */
    pMibClass->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    pMibClass->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
  }

  /* build class rule entries */
  classIndex = L7_DIFFSERV_CLASS_HDR_START;
  classRuleRowIndex = 1;
  for (i = L7_DIFFSERV_CLASS_RULE_START; i < L7_DIFFSERV_CLASS_RULE_MAX; i++)
  {
    pCfg->classRule[i].hdrIndexRef = classIndex;
    pCfg->classRule[i].chainIndex = i + 1;
    pCfg->classRule[i].rowInvalidFlags = (dsmibClassRuleFlags_t)0x80000000; /* invalid */
    pCfg->classRule[i].arid = 3;                                            /* invalid */

    pMibClassRule = &pCfg->classRule[i].mib;

    pMibClassRule->index = classRuleRowIndex;
    pMibClassRule->entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;
    for (j = 0; j < L7_MAC_ADDR_LEN; j++)
    {
      pMibClassRule->match.srcMac.addr[j] = (L7_uchar8)((classRuleRowIndex * 0x10) + j); /* n0:n1:n2:n3:n4:n5 */
      pMibClassRule->match.srcMac.mask[j] = (L7_uchar8)0xDF - (L7_uchar8)j;              /* DF:DE:DD:DC:DB:DA */
    }
    pMibClassRule->excludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;
    pMibClassRule->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    pMibClassRule->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;

    if (++classRuleRowIndex > L7_DIFFSERV_RULE_PER_CLASS_LIM)
    {
      /* mark end of rule chain for current class and advance to next class index */
      pCfg->classRule[i].chainIndex = 0;
      classIndex++;
      classRuleRowIndex = 1;
    }
  } /* endfor i */

  /* build policy hdr entries */
  for (i = L7_DIFFSERV_POLICY_HDR_START; i < L7_DIFFSERV_POLICY_HDR_MAX; i++)
  {
    pCfg->policyHdr[i].inUse = L7_TRUE;
    pCfg->policyHdr[i].chainIndex = ((i - 1) * L7_DIFFSERV_INST_PER_POLICY_LIM) + 1;
    pCfg->policyHdr[i].rowInvalidFlags = (dsmibPolicyFlags_t)0x80000000;  /* invalid */

    pMibPolicy = &pCfg->policyHdr[i].mib;

    pMibPolicy->index = i;
    osapiSnprintf(pMibPolicy->name, sizeof(pMibPolicy->name), "testDiffServPolicy_%u", i);
    pMibPolicy->type = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN;
    pMibPolicy->instIndexNextFree = L7_DIFFSERV_INST_PER_POLICY_LIM;  /* invalid */
    pMibPolicy->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    pMibPolicy->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
  }

  /* build policy inst entries */
  policyIndex = L7_DIFFSERV_POLICY_HDR_START;
  policyInstRowIndex = 1;
  for (i = L7_DIFFSERV_POLICY_INST_START; i < L7_DIFFSERV_POLICY_INST_MAX; i++)
  {
    pCfg->policyInst[i].hdrIndexRef = policyIndex;
    pCfg->policyInst[i].attrChainIndex = ((i - 1) * L7_DIFFSERV_ATTR_PER_INST_LIM) + 1;
    pCfg->policyInst[i].chainIndex = i + 1;
    pCfg->policyInst[i].rowInvalidFlags = (dsmibPolicyInstFlags_t)0x80000000;  /* invalid */

    pMibPolicyInst = &pCfg->policyInst[i].mib;

    pMibPolicyInst->index = policyInstRowIndex;
    pMibPolicyInst->classIndex = policyInstRowIndex;  /* use instead of separate classIndex */
    pMibPolicyInst->attrIndexNextFree = L7_DIFFSERV_ATTR_PER_INST_LIM;  /* invalid */
    pMibPolicyInst->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    pMibPolicyInst->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;

    if (++policyInstRowIndex > L7_DIFFSERV_INST_PER_POLICY_LIM)
    {
      /* mark end of inst chain for current policy and advance to next policy index */
      pCfg->policyInst[i].chainIndex = 0;
      policyIndex++;
      policyInstRowIndex = 1;
    }
  } /* endfor i */

  /* build policy attr entries */
  policyInstIndex = L7_DIFFSERV_POLICY_INST_START;
  policyAttrRowIndex = 1;
  for (i = L7_DIFFSERV_POLICY_ATTR_START; i < L7_DIFFSERV_POLICY_ATTR_MAX; i++)
  {
    pCfg->policyAttr[i].instIndexRef = policyInstIndex;
    pCfg->policyAttr[i].chainIndex = i + 1;
    pCfg->policyAttr[i].rowInvalidFlags = (dsmibPolicyAttrFlags_t)0x80000000;  /* invalid */

    pMibPolicyAttr = &pCfg->policyAttr[i].mib;

    pMibPolicyAttr->index = policyAttrRowIndex;
    pMibPolicyAttr->entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    pMibPolicyAttr->stmt.policeTwoRate.crate = 5000;
    pMibPolicyAttr->stmt.policeTwoRate.cburst = 80;
    pMibPolicyAttr->stmt.policeTwoRate.prate = 6000;
    pMibPolicyAttr->stmt.policeTwoRate.pburst = 96;
    pMibPolicyAttr->stmt.policeTwoRate.conformAct = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC;
    pMibPolicyAttr->stmt.policeTwoRate.exceedAct = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS;
    pMibPolicyAttr->stmt.policeTwoRate.nonconformAct = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND;
    pMibPolicyAttr->stmt.policeTwoRate.conformVal = 7;
    pMibPolicyAttr->stmt.policeTwoRate.exceedVal = 6;
    pMibPolicyAttr->stmt.policeTwoRate.nonconformVal = 5;       /* invalid */
    pMibPolicyAttr->stmt.policeTwoRate.colorConformIndex = 4;
    pMibPolicyAttr->stmt.policeTwoRate.colorExceedIndex = 3;
    pMibPolicyAttr->stmt.policeTwoRate.colorConformMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND;
    pMibPolicyAttr->stmt.policeTwoRate.colorExceedMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND;
    pMibPolicyAttr->stmt.policeTwoRate.colorConformVal = 46;
    pMibPolicyAttr->stmt.policeTwoRate.colorExceedVal = 2;
    pMibPolicyAttr->storageType = L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE;
    pMibPolicyAttr->rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;

    if (++policyAttrRowIndex > L7_DIFFSERV_ATTR_PER_INST_LIM)
    {
      /* mark end of attr chain for current inst and advance to next inst index */
      pCfg->policyAttr[i].chainIndex = 0;
      policyInstIndex++;
      policyAttrRowIndex = 1;
    }
  } /* endfor i */

  sysapiPrintf("\nBuilt DiffServ test config data\n\n");
}

/*********************************************************************
* @purpose  Save test DiffServ config data to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
void diffServSaveTestConfigData(void)
{
  pDiffServCfgData_g->cfgHdr.dataChanged = L7_FALSE;
  pDiffServCfgData_g->checkSum = nvStoreCrc32((L7_uchar8 *)pDiffServCfgData_g,
                                              ((L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE -
                                                (L7_uint32)sizeof(pDiffServCfgData_g->checkSum)));


  if (sysapiCfgFileWrite(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                         L7_DIFFSERV_CFG_FILENAME,
                         (L7_char8 *)pDiffServCfgData_g,
                         (L7_uint32)sizeof(L7_diffServCfg_t)) != L7_SUCCESS)
  {
    sysapiPrintf("\nAttempt to save DiffServ test config data failed!");
    return;
  }

  sysapiPrintf("\nSaved DiffServ test config data to NVStore\n\n");
}

/*********************************************************************
*
* @purpose  Dump test DiffServ config data contents
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void diffServConfigDataTestShow(void)
{
  /* config file header contents */
  sysapiCfgFileHeaderDump(&pDiffServCfgData_g->cfgHdr);
  sysapiPrintf("\n");

  /* config file parameters */
  diffServConfigDataShow();

  /* checksum */
  sysapiPrintf("pDiffServCfgData_g->checkSum : 0x%8.8x\n", pDiffServCfgData_g->checkSum);

  /* pertinent scaling constants */
  sysapiPrintf("\nScaling Constants\n");
  sysapiPrintf(  "-----------------\n");

  sysapiPrintf("L7_DIFFSERV_CLASS_HDR_MAX.................. %u\n",
               (L7_uint32)L7_DIFFSERV_CLASS_HDR_MAX);

  sysapiPrintf("L7_DIFFSERV_CLASS_RULE_MAX................. %u\n",
               (L7_uint32)L7_DIFFSERV_CLASS_RULE_MAX);

  sysapiPrintf("L7_DIFFSERV_RULE_PER_CLASS_LIM............. %u\n",
               (L7_uint32)L7_DIFFSERV_RULE_PER_CLASS_LIM);

  sysapiPrintf("L7_DIFFSERV_POLICY_HDR_MAX................. %u\n",
               (L7_uint32)L7_DIFFSERV_POLICY_HDR_MAX);

  sysapiPrintf("L7_DIFFSERV_POLICY_INST_MAX................ %u\n",
               (L7_uint32)L7_DIFFSERV_POLICY_INST_MAX);

  sysapiPrintf("L7_DIFFSERV_INST_PER_POLICY_LIM............ %u\n",
               (L7_uint32)L7_DIFFSERV_INST_PER_POLICY_LIM);

  sysapiPrintf("L7_DIFFSERV_POLICY_ATTR_MAX................ %u\n",
               (L7_uint32)L7_DIFFSERV_POLICY_ATTR_MAX);

  sysapiPrintf("L7_DIFFSERV_ATTR_PER_INST_LIM.............. %u\n",
               (L7_uint32)L7_DIFFSERV_ATTR_PER_INST_LIM);

  sysapiPrintf("L7_DIFFSERV_INTF_MAX_COUNT................. %u\n",
               (L7_uint32)L7_DIFFSERV_INTF_MAX_COUNT);

  sysapiPrintf("L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX....... %u\n",
               (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX);

  sysapiPrintf("L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX...... %u\n",
               (L7_uint32)L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX);

  sysapiPrintf("\n");
}

/*============================================================================*/
/*================== END OF CONFIG MIGRATION DEBUG CHANGES ===================*/
/*============================================================================*/


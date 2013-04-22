/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   snooping_cfg.c
*
* @purpose    Contains definitions to support the configuration read
*             save and apply routines
*
* @component  Snooping
*
* @comments   none
*
* @create     07-Dec-2006
*
* @author     drajendra
*
* @end
*
**********************************************************************/
#include "comm_mask.h"
#include "nvstoreapi.h"
#include "support_api.h"
#include "l7utils_inet_addr_api.h"

#include "snooping_outcalls.h"
#include "snooping.h"
#include "snooping_util.h"
#include "snooping_api.h"
#include "snooping_debug_api.h"
#include "snooping_debug.h"
#include "snooping_ctrl.h"
#include "snooping_db.h"

/*********************************************************************
* @purpose  Saves Snooping configuration  to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopSave(void)
{
  L7_RC_t     rc = L7_SUCCESS;
  L7_uint32   cbIndex = 0, maxInst;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  if (snoopHasDataChanged() == L7_TRUE)
  {
    maxInst = maxSnoopInstancesGet();
    pSnoopCB = snoopCBFirstGet();
    for (cbIndex = 0; cbIndex < maxInst;
         cbIndex++, pSnoopCB++)
    {
      pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_FALSE;
      pSnoopCB->snoopCfgData->checkSum =
        nvStoreCrc32((L7_char8 *)pSnoopCB->snoopCfgData,
                     (sizeof (snoopCfgData_t) -
                      sizeof (pSnoopCB->snoopCfgData->checkSum)));
      if (sysapiCfgFileWrite(L7_SNOOPING_COMPONENT_ID,
                             pSnoopCB->snoopCfgData->cfgHdr.filename,
                             (L7_char8 *)pSnoopCB->snoopCfgData,
                             sizeof(snoopCfgData_t)) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
               "snoopSave: Error on call to sysapiCfgFileWrite file %s",
                pSnoopCB->snoopCfgData->cfgHdr.filename);
        rc = L7_FAILURE;
        break;
      }
    }
  }

  return (rc);
}

/*********************************************************************
* @purpose  Restores Snooping component user configuration to factory
*           defaults
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopRestore(void)
{
  L7_uint32        vlanId;
  L7_uint32        cbIndex = 0, maxInst = 0;
  snoop_cb_t      *pSnoopCB = L7_NULLPTR;
  snoopOperData_t *pSnoopOperEntry  = L7_NULLPTR;
#ifdef L7_MCAST_PACKAGE
  snoopL3InfoData_t *pSnoopL3Entry = L7_NULLPTR;
  L7_inet_addr_t     mcastSrcAddr, mcastGrpAddr;
#endif

  maxInst = maxSnoopInstancesGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = 0; cbIndex < maxInst; cbIndex++, pSnoopCB++)
  {
    /* Delete all IGMP Snooping entries */
    (void)snoopEntriesDoFlush(pSnoopCB);
    pSnoopOperEntry = snoopOperEntryFirstGet(pSnoopCB);
    while (pSnoopOperEntry)
    {
      vlanId = pSnoopOperEntry->vlanId;

      /* Clear all the mrouter information */
      memset(&pSnoopOperEntry->mcastRtrAttached, 0x00,
             sizeof(L7_INTF_MASK_t));

      /* Clear allthe interface information */
      memset(&pSnoopOperEntry->snoopIntfMode, 0x00,
             sizeof(L7_INTF_MASK_t));

      /* Disable all querier vlans that are operational */
      if (pSnoopOperEntry->snoopQuerierInfo.snoopQuerierOperState !=
          SNOOP_QUERIER_DISABLED)
      {
        (void)snoopQuerierVlanModeApply(vlanId, L7_DISABLE, pSnoopCB);
      }
      pSnoopOperEntry = snoopOperEntryGet(vlanId, pSnoopCB,
                                          L7_MATCH_GETNEXT);
    }/* End of vlan iterations */
  } /* End of snoop instances interations */

#ifdef L7_MCAST_PACKAGE
  /* Cleanup learnt L3 mrouter databased */
  memset(&mcastSrcAddr, 0x00, sizeof(L7_inet_addr_t));
  memset(&mcastGrpAddr, 0x00, sizeof(L7_inet_addr_t));
  pSnoopL3Entry = snoopL3EntryFind(&mcastGrpAddr, &mcastSrcAddr, L7_MATCH_GETNEXT);
  while (pSnoopL3Entry)
  {
    if (snoopL3EntryDelete(&pSnoopL3Entry->snoopL3InfoDataKey.mcastGroupAddr,
                           &pSnoopL3Entry->snoopL3InfoDataKey.mcastSrcAddr)
        != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
             "snoopRestore: Error cleaning the snoopL3Table");
      break;
    }
    memset(&mcastSrcAddr, 0x00, sizeof(L7_inet_addr_t));
    memset(&mcastGrpAddr, 0x00, sizeof(L7_inet_addr_t));
    pSnoopL3Entry = snoopL3EntryFind(&mcastGrpAddr, &mcastSrcAddr,
                                     L7_MATCH_GETNEXT);
  }
#endif

  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = 0; cbIndex < maxInst; cbIndex++, pSnoopCB++)
  {
    /* Build Default configuration */
    if (pSnoopCB->family == L7_AF_INET)
    {
      snoopIGMPBuildDefaultConfigData(pSnoopCB->snoopCfgData->cfgHdr.version);
    }
    else if (pSnoopCB->family == L7_AF_INET6)
    {

      snoopMLDBuildDefaultConfigData(pSnoopCB->snoopCfgData->cfgHdr.version);
    }
    (void)snoopApplyConfigData(pSnoopCB);
    pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if Snooping user config data has changed
*
* @param    none
*
* @returns  L7_TRUE    Atleast one snoop instances' data has changed
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL snoopHasDataChanged(void)
{
  L7_BOOL     rc = L7_FALSE;
  L7_uint32   cbIndex = 0, maxInst;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  maxInst = maxSnoopInstancesGet();
  if ((pSnoopCB = snoopCBFirstGet()) == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  for (cbIndex = 0; cbIndex < maxInst;
       cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB != L7_NULLPTR &&
        pSnoopCB->snoopCfgData->cfgHdr.dataChanged == L7_TRUE)
    {
      rc = L7_TRUE;
      break;
    }
  }
  return rc;
}
void snoopResetDataChanged(void)
{
  L7_uint32   cbIndex = 0, maxInst;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  maxInst = maxSnoopInstancesGet();
  if ((pSnoopCB = snoopCBFirstGet()) == L7_NULLPTR)
  {
    return;
  }

  for (cbIndex = 0; cbIndex < maxInst;
       cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB != L7_NULLPTR)
      pSnoopCB->snoopCfgData->cfgHdr.dataChanged = L7_FALSE;
  }
  return;
}
/*********************************************************************
* @purpose  Apply Configuration Data for a specified snoop instance
*
* @param    pSnoopCB           @b{(input)}  Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopApplyConfigData(snoop_cb_t *pSnoopCB)
{
  L7_uint32       cfgIndex;
  L7_uint32       intIfNum;
  nimConfigID_t   configIdNull;
  snoopCfgData_t *pSnoopCfg;

  pSnoopCfg = pSnoopCB->snoopCfgData;
  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* Apply the global admin mode for Snooping */
  if (snoopAdminModeApply(pSnoopCfg->snoopAdminMode, L7_NULL, pSnoopCB) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
      "snoopApplyConfigData: Error enabling global admin mode for family %d",
      pSnoopCB->family);
    return L7_FAILURE;
  }

  /* Now check for interfaces that have been enabled for Snooping.  All non-zero
     values in the Cfg structure contain an external interface number that is
     configured for Snooping.  Convert this to internal interface number and
     attempt to enable it.  The internal interface number is used as an index
     into the operational interface data */
  if (snoopIsReady() == L7_TRUE)
  {
    for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&(pSnoopCfg->snoopIntfCfgData[cfgIndex].configId),
                                 &configIdNull))
      {
        continue;
      }
      if (nimIntIfFromConfigIDGet(&(pSnoopCfg->snoopIntfCfgData[cfgIndex].configId),
                                  &intIfNum) != L7_SUCCESS)
      {
        continue;
      }

      /* Apply interface configuration for snooping */
      if (snoopApplyIntfConfigData(intIfNum, pSnoopCB) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_SNOOPING_COMPONENT_ID,
          "snoopApplyConfigData: Error enabling interface admin mode for family %d",
          pSnoopCB->family);
        return L7_FAILURE;
      }
    }/* end of cfgIndex for */
  }/* CNFGR_STATE_CHECK */

  return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Build default Interface config data for a specified
*           snoop instance
*
* @param    configId       @b{(input)}  The config Id to be placed
*                                       into the intf config
* @param    pCfg           @b{(input)}  A pointer to the interface cfg
*                                       structure
* @param    pSnoopCB       @b{(input)}  Control Block
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                     snoopIntfCfgData_t *pCfg,
                                     snoop_cb_t *pSnoopCB)
{
  switch (pSnoopCB->family)
  {
    case L7_AF_INET:
      snoopIGMPBuildDefaultIntfConfigData(configId, pCfg);
      break;
    case L7_AF_INET6:
      snoopMLDBuildDefaultIntfConfigData(configId, pCfg);
      break;
    default:
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SNOOPING_COMPONENT_ID,
             "snoopBuildDefaultIntfConfigData: Invalid family passed");
  }
}

/*********************************************************************
* @purpose  Build default IGMP Snooping Intf config data
*
* @param    configId       @b{(input)}  The config Id to be placed
*                                       into the intf config
* @param    pCfg           @b{(input)}  A pointer to the interface cfg
*                                       structure
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopIGMPBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                         snoopIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  /* Interface snooping mode */
  if (FD_IGMP_SNOOPING_INTF_MODE == L7_ENABLE)
  {
    pCfg->intfMode |= SNOOP_VLAN_MODE;
  }
  else
  {
    pCfg->intfMode &= ~SNOOP_VLAN_MODE;
  }

  /* Set static Mcast router attached interface config */
  pCfg->intfMcastRtrAttached = FD_IGMP_SNOOPING_MCASTRTR_STATUS;

  if (FD_IGMP_SNOOPING_MCASTRTR_STATUS == L7_DISABLE)
  {
    memset(&pCfg->vlanStaticMcastRtr, 0, sizeof(pCfg->vlanStaticMcastRtr));
  }
  else
  {
    memset(&pCfg->vlanStaticMcastRtr, 0xff, sizeof(pCfg->vlanStaticMcastRtr));
  }

  /* interface fast leave mode */
  if (FD_IGMP_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE == L7_ENABLE)
  {
    pCfg->intfMode |= SNOOP_VLAN_FAST_LEAVE_MODE;
  }
  else
  {
    pCfg->intfMode &= ~SNOOP_VLAN_FAST_LEAVE_MODE;
  }

  /* Group membership interval */
  pCfg->groupMembershipInterval = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;

  /* Max response time */
  pCfg->responseTime = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;

  /* Mcast router attached interface expiry time */
  pCfg->mcastRtrExpiryTime = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
}

/*********************************************************************
* @purpose  Build default IGMP Snooping config data
*
* @param    ver       @b{(input)}  Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopIGMPBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32       cfgIndex, mode;
  nimConfigID_t   configId[L7_IGMP_SNOOPING_MAX_INTF];
  snoop_cb_t     *pSnoopCB;
  snoopCfgData_t *pSnoopCfg;

  /* Get a valid control block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    return;
  }

  pSnoopCfg = pSnoopCB->snoopCfgData;

  /* zero the configuration */
  memset(( void * )pSnoopCfg, 0x00, sizeof( snoopCfgData_t));
  memset(configId, 0x00, sizeof(configId));
  /**********************************
   * Building Interface config data  *
   **********************************/
  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF; cfgIndex++)
  {
    snoopIGMPBuildDefaultIntfConfigData(&configId[cfgIndex],
                                        &pSnoopCfg->snoopIntfCfgData[cfgIndex]);
  }

  /**********************************
   * Building VLAN config data  *
   **********************************/
  /* reusing the cfgIndex variable */
  mode = 0;
  if (FD_IGMP_SNOOPING_QUERIER_VLAN_ELECTION_MODE == L7_ENABLE)
  {
    mode |= SNOOP_QUERIER_ELECTION_PARTICIPATE;
  }
  else
  {
    mode &= ~SNOOP_QUERIER_ELECTION_PARTICIPATE;
  }

  if (FD_IGMP_SNOOPING_QUERIER_ADMIN_MODE == L7_ENABLE)
  {
    mode |= SNOOP_QUERIER_MODE;
  }
  else
  {
    mode &= ~SNOOP_QUERIER_MODE;
  }

  for (cfgIndex = 1; cfgIndex < (L7_DOT1Q_MAX_VLAN_ID + 1); cfgIndex++)
  {
    /* group membership interval */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].groupMembershipInterval =
      FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;

    /* max response time */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].maxResponseTime =
      FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;

    /* mcast router attached interface expiry time */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].mcastRtrExpiryTime =
      FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
    /* Snoop Querier information */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      = mode;
    inetAddressZeroSet(L7_AF_INET,
                       &pSnoopCfg->snoopVlanCfgData[cfgIndex].snoopVlanQuerierCfgData.snoopQuerierVlanAddress);
  }

  /* Snoop global admin mode */
  pSnoopCfg->snoopAdminMode = FD_IGMP_SNOOPING_ADMIN_MODE;
  /* PTin added: IGMP */
  pSnoopCfg->snoopAdminIGMPPrio = SNOOP_IGMP_DEFAULT_PRIO;
  /* Snoop Querier admin mode */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierAdminMode = FD_IGMP_SNOOPING_QUERIER_ADMIN_MODE;
  /* Version */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierVersion = FD_IGMP_SNOOPING_QUERIER_VERSION;
  /* Querier Address */
  inetAddressZeroSet(L7_AF_INET6, &pSnoopCfg->snoopQuerierCfgData.snoopQuerierAddress);
  /* Query Interval */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierQueryInterval = FD_IGMP_SNOOPING_QUERIER_QUERY_INTERVAL;
  /* Other Querier Interval */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierExpiryInterval = FD_IGMP_SNOOPING_QUERIER_EXPIRY_INTERVAL;
  /* Strict RFC validations */
  pSnoopCfg->igmpv3_tos_rtr_alert_check = FD_IGMP_SNOOPING_ROUTER_ALERT_CHECK;
  /* Snooping config file name */
  strcpy(pSnoopCfg->cfgHdr.filename, SNOOP_IGMP_CFG_FILENAME);

  pSnoopCfg->cfgHdr.version = ver;

  pSnoopCfg->cfgHdr.componentID = L7_SNOOPING_COMPONENT_ID;

  pSnoopCfg->cfgHdr.type = L7_CFG_DATA;

  pSnoopCfg->cfgHdr.length = sizeof(snoopCfgData_t);

  pSnoopCfg->cfgHdr.dataChanged = L7_FALSE;

  pSnoopCB->enabledSnoopQuerierVlans = 0;
  return;
}

/*********************************************************************
* @purpose  Build default MLD Snooping Intf config data
*
* @param    configId       @b{(input)}  The config Id to be placed
*                                       into the intf config
* @param    pCfg           @b{(input)}  A pointer to the interface cfg
*                                       structure
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopMLDBuildDefaultIntfConfigData(nimConfigID_t *configId,
                                        snoopIntfCfgData_t *pCfg)
{
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  /* Interface snooping mode */
  if (FD_MLD_SNOOPING_INTF_MODE == L7_ENABLE)
  {
    pCfg->intfMode |= SNOOP_VLAN_MODE;
  }
  else
  {
    pCfg->intfMode &= ~SNOOP_VLAN_MODE;
  }

  /* Set static Mcast router attached interface config */
  pCfg->intfMcastRtrAttached = FD_MLD_SNOOPING_MCASTRTR_STATUS;

  /* Set static Mcast router attached (interface,vlan) config */
  if (FD_MLD_SNOOPING_MCASTRTR_STATUS == L7_DISABLE)
  {
    memset(&pCfg->vlanStaticMcastRtr, 0, sizeof(pCfg->vlanStaticMcastRtr));
  }
  else
  {
    memset(&pCfg->vlanStaticMcastRtr, 0xff, sizeof(pCfg->vlanStaticMcastRtr));
  }

  /* interface fast leave mode */
  if (FD_MLD_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE == L7_ENABLE)
  {
    pCfg->intfMode |= SNOOP_VLAN_FAST_LEAVE_MODE;
  }
  else
  {
    pCfg->intfMode &= ~SNOOP_VLAN_FAST_LEAVE_MODE;
  }

  /* Group membership interval */
  pCfg->groupMembershipInterval = FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;

  /* Max response time */
  pCfg->responseTime = FD_MLD_SNOOPING_MAX_RESPONSE_TIME;

  /* Mcast router attached interface expiry time */
  pCfg->mcastRtrExpiryTime = FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME;
}

/*********************************************************************
* @purpose  Build default MLD Snooping config data
*
* @param    ver       @b{(input)}  Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopMLDBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32       cfgIndex, mode;
  nimConfigID_t   configId[L7_IGMP_SNOOPING_MAX_INTF];
  snoop_cb_t     *pSnoopCB;
  snoopCfgData_t *pSnoopCfg;

  /* Get a valid control block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET6)) == L7_NULLPTR)
  {
    return;
  }

  pSnoopCfg = pSnoopCB->snoopCfgData;

  /* zero the configuration */
  memset(( void * )pSnoopCfg, 0x00, sizeof( snoopCfgData_t));
  memset(configId, 0x00, sizeof(configId));
  /**********************************
   * Building Interface config data  *
   **********************************/
  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF; cfgIndex++)
  {
    snoopMLDBuildDefaultIntfConfigData(&configId[cfgIndex],
                                       &pSnoopCfg->snoopIntfCfgData[cfgIndex]);
  }

  /**********************************
   * Building VLAN config data  *
   **********************************/
  /* reusing the cfgIndex variable */
  mode = 0;
  if (FD_MLD_SNOOPING_QUERIER_VLAN_ELECTION_MODE == L7_ENABLE)
  {
    mode |= SNOOP_QUERIER_ELECTION_PARTICIPATE;
  }
  else
  {
    mode &= ~SNOOP_QUERIER_ELECTION_PARTICIPATE;
  }

  if (FD_MLD_SNOOPING_QUERIER_ADMIN_MODE == L7_ENABLE)
  {
    mode |= SNOOP_QUERIER_MODE;
  }
  else
  {
    mode &= ~SNOOP_QUERIER_MODE;
  }

  for (cfgIndex = 1; cfgIndex < (L7_DOT1Q_MAX_VLAN_ID + 1); cfgIndex++)
  {
    /* group membership interval */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].groupMembershipInterval =
      FD_MLD_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;

    /* max response time */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].maxResponseTime =
      FD_MLD_SNOOPING_MAX_RESPONSE_TIME;

    /* mcast router attached interface expiry time */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].mcastRtrExpiryTime =
      FD_MLD_SNOOPING_MCAST_RTR_EXPIRY_TIME;
    /* Snoop Querier information */
    pSnoopCfg->snoopVlanCfgData[cfgIndex].snoopVlanQuerierCfgData.snoopQuerierVlanAdminMode
      = mode;

    inetAddressZeroSet(L7_AF_INET6, &pSnoopCfg->snoopVlanCfgData[cfgIndex].snoopVlanQuerierCfgData.snoopQuerierVlanAddress);
  }

  /* Snoop global admin mode */
  pSnoopCfg->snoopAdminMode = FD_MLD_SNOOPING_ADMIN_MODE;

  /* Snoop Querier admin mode */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierAdminMode = FD_MLD_SNOOPING_QUERIER_ADMIN_MODE;
  /* PTin added: IGMP */
  pSnoopCfg->snoopAdminIGMPPrio = SNOOP_IGMP_DEFAULT_PRIO;
  /* Version */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierVersion = FD_MLD_SNOOPING_QUERIER_VERSION;
  /* Querier Address */
  inetAddressZeroSet(L7_AF_INET6, &pSnoopCfg->snoopQuerierCfgData.snoopQuerierAddress);
  /* Query Interval */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierQueryInterval = FD_MLD_SNOOPING_QUERIER_QUERY_INTERVAL;
  /* Other Querier Interval */
  pSnoopCfg->snoopQuerierCfgData.snoopQuerierExpiryInterval = FD_MLD_SNOOPING_QUERIER_EXPIRY_INTERVAL;

  /* Snooping config file name */
  strcpy(pSnoopCfg->cfgHdr.filename, SNOOP_MLD_CFG_FILENAME);

  pSnoopCfg->cfgHdr.version = ver;

  pSnoopCfg->cfgHdr.componentID = L7_SNOOPING_COMPONENT_ID;

  pSnoopCfg->cfgHdr.type = L7_CFG_DATA;

  pSnoopCfg->cfgHdr.length = sizeof(snoopCfgData_t);

  pSnoopCfg->cfgHdr.dataChanged = L7_FALSE;

  pSnoopCB->enabledSnoopQuerierVlans = 0;

  return;
}

/*********************************************************************
* @purpose  Get all currently configured VLANs and store it in snoop
*           oper tree
*
* @param    none
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void snoopAllVlansGet(void)
{
  L7_uint32 vlanId = 0;

  while (snoopNextVlanGet(vlanId, &vlanId) == L7_SUCCESS)
  {
    if (snoopOperEntryAdd(vlanId) == L7_SUCCESS)
    {
      SNOOP_TRACE(SNOOP_DEBUG_QUERIER, 0,
                  "snoopAllVlansGet: Added OperEntry VLAN %d, add querier", vlanId);

      snoopQuerierVlanAdd(vlanId);
    }
  }
}

/*********************************************************************
* @purpose  Saves snoop debug configuration for all snoop instances
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopDebugSave(void)
{
  L7_RC_t rc;
  snoop_cb_t   *pSnoopCB = L7_NULLPTR;
  L7_uint32     maxInst, cbIndex;

  rc = L7_SUCCESS;
  maxInst = maxSnoopInstancesGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = 0; cbIndex < maxInst;
       cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->snoopDebugCfg->hdr.dataChanged == L7_TRUE)
    {
      /* Copy the operational states into the config file */
      snoopDebugCfgUpdate(pSnoopCB);

      /* Store the config file */
      pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_FALSE;
      pSnoopCB->snoopDebugCfg->checkSum =
        nvStoreCrc32((L7_uchar8 *)pSnoopCB->snoopDebugCfg,
                     (L7_uint32)(sizeof(snoopDebugCfg_t) -
                                 sizeof(pSnoopCB->snoopDebugCfg->checkSum)));
      /* call save NVStore routine */
      if ((rc = sysapiSupportCfgFileWrite(L7_SNOOPING_COMPONENT_ID,
                                          pSnoopCB->snoopDebugCfg->hdr.filename,
                                          (L7_char8 *)pSnoopCB->snoopDebugCfg,
                                          (L7_uint32)sizeof(snoopDebugCfg_t)))
          == L7_ERROR)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
               "Error on call to sysapiSupportCfgFileWrite routine on config file %s\n",
                pSnoopCB->snoopDebugCfg->hdr.filename);
      }
    }/* config change check  */
  }

  return rc;
}

/*********************************************************************
* @purpose  Restores snoop debug configuration of all snoop instances
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t snoopDebugRestore(void)
{
  L7_RC_t rc;
  snoop_cb_t   *pSnoopCB = L7_NULLPTR;
  L7_uint32     maxInst, cbIndex;

  rc = L7_SUCCESS;
  maxInst = maxSnoopInstancesGet();

  /* Restore debug configuration of all snooping instances */
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = 0; cbIndex < maxInst;
       cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->family == L7_AF_INET)
    {
      snoopIGMPDebugBuildDefaultConfigData(SNOOP_IGMP_DEBUG_CFG_VER_CURRENT);
      pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_TRUE;
      rc = snoopApplyDebugConfigData(pSnoopCB);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
               "snoopDebugRestore: IGMP Snooping Debug config restore failed");
        break;
      }
    }
    else if (pSnoopCB->family == L7_AF_INET6)
    {
      snoopMLDDebugBuildDefaultConfigData(SNOOP_MLD_DEBUG_CFG_VER_CURRENT);
      pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_TRUE;
      rc = snoopApplyDebugConfigData(pSnoopCB);
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_SNOOPING_COMPONENT_ID,
               "snoopDebugRestore: MLD Snooping Debug config restore failed");
        break;
      }
    }
  }/* end of for to get control blocks */
  return rc;
}

/*********************************************************************
* @purpose  Checks if snoop debug config data has changed for
*           all snoop instances
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL snoopDebugHasDataChanged(void)
{
  L7_BOOL     rc = L7_FALSE;
  L7_uint32   cbIndex = 0, maxInst;
  snoop_cb_t *pSnoopCB = L7_NULLPTR;

  maxInst = maxSnoopInstancesGet();
  pSnoopCB = snoopCBFirstGet();
  for (cbIndex = 0; cbIndex < maxInst; cbIndex++, pSnoopCB++)
  {
    if (pSnoopCB->snoopDebugCfg->hdr.dataChanged == L7_TRUE)
    {
      rc = L7_TRUE;
      break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Build default IGMP Snooping Debug config data
*
* @param    ver     @b{(input)}  Software version of debug Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopIGMPDebugBuildDefaultConfigData(L7_uint32 ver)
{
  snoop_cb_t     *pSnoopCB;

  /* Get a valid control block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET)) == L7_NULLPTR)
  {
    return;
  }

  /* setup debug file header */
  pSnoopCB->snoopDebugCfg->hdr.version = ver;
  pSnoopCB->snoopDebugCfg->hdr.componentID = L7_SNOOPING_COMPONENT_ID;
  pSnoopCB->snoopDebugCfg->hdr.type = L7_CFG_DATA;
  pSnoopCB->snoopDebugCfg->hdr.length = (L7_uint32)sizeof(snoopDebugCfg_t);
  strcpy((L7_char8 *)pSnoopCB->snoopDebugCfg->hdr.filename, SNOOP_IGMP_DEBUG_CFG_FILENAME);
  pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&(pSnoopCB->snoopDebugCfg->cfg), 0, sizeof(pSnoopCB->snoopDebugCfg->cfg));
}

/*********************************************************************
* @purpose  Build default MLD Snooping Debug config data
*
* @param    ver     @b{(input)}  Software version of debug Config Data
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void snoopMLDDebugBuildDefaultConfigData(L7_uint32 ver)
{
  snoop_cb_t     *pSnoopCB;

  /* Get a valid control block */
  if ((pSnoopCB = snoopCBGet(L7_AF_INET6)) == L7_NULLPTR)
  {
    return;
  }

  /* setup debug file header */
  pSnoopCB->snoopDebugCfg->hdr.version = ver;
  pSnoopCB->snoopDebugCfg->hdr.componentID = L7_SNOOPING_COMPONENT_ID;
  pSnoopCB->snoopDebugCfg->hdr.type = L7_CFG_DATA;
  pSnoopCB->snoopDebugCfg->hdr.length = (L7_uint32)sizeof(snoopDebugCfg_t);
  strcpy((L7_char8 *)pSnoopCB->snoopDebugCfg->hdr.filename,
         SNOOP_MLD_DEBUG_CFG_FILENAME);
  pSnoopCB->snoopDebugCfg->hdr.dataChanged = L7_FALSE;

  /* set all flags to L7_FALSE */
  memset(&(pSnoopCB->snoopDebugCfg->cfg), 0, sizeof(pSnoopCB->snoopDebugCfg->cfg));
}

/*********************************************************************
* @purpose  Apply Snooping Debug config data for a specified
*           snoop instance
*
* @param    pSnoopCB    @b{(input)}  Control Block
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Called after a default config is built
*
* @end
*********************************************************************/
L7_RC_t snoopApplyDebugConfigData(snoop_cb_t *pSnoopCB)
{
  return snoopDebugPacketTraceFlagSet(pSnoopCB->snoopDebugCfg->cfg.snoopDebugPacketTraceTxFlag,
                                      pSnoopCB->snoopDebugCfg->cfg.snoopDebugPacketTraceRxFlag,
                                      pSnoopCB->family);
}

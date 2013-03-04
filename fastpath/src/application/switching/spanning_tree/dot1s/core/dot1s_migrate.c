
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dot1s_migrate.c
*
* @purpose   Multiple Spanning tree configuration migration
*
* @component dot1s
*
* @comments  none
*
* @create    08/24/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#include "dot1s_migrate.h"

static L7_RC_t dot1sMigrateConfigV1V2Convert (dot1sCfgV1_t * pCfgV1, dot1sCfgV2_t * pCfgV2);
static L7_RC_t dot1sMigrateConfigV2V3Convert (dot1sCfgV2_t * pCfgV2, dot1sCfgV3_t * pCfgV3);
static L7_RC_t dot1sMigrateConfigV3V4Convert (dot1sCfgV3_t * pCfgV3, dot1sCfgV4_t * pCfgV4);
static L7_RC_t dot1sMigrateConfigV4V5Convert (dot1sCfgV4_t * pCfgV4, dot1sCfgV5_t * pCfgV5);

static void dot1sBuildConfigDataV2 (dot1sCfgV2_t * pCfgV2);
static void dot1sBuildConfigDataV3 (dot1sCfgV3_t * pCfgV3);
static void dot1sBuildConfigDataV4 (dot1sCfgV4_t * pCfgV4);
static void dot1sBuildConfigDataV5 (dot1sCfgV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static dot1sCfg_t *pCfgCur = L7_NULLPTR;

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void
dot1sMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  dot1sCfgV1_t *pCfgV1 = L7_NULLPTR;
  dot1sCfgV2_t *pCfgV2 = L7_NULLPTR;
  dot1sCfgV3_t *pCfgV3 = L7_NULLPTR;
  dot1sCfgV4_t *pCfgV4 = L7_NULLPTR;
  dot1sCfgV5_t *pCfgV5 = L7_NULLPTR;

  pCfgCur = (dot1sCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case DOT1S_CFG_VER_1:
    pCfgV1 = (dot1sCfgV1_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1sCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (dot1sCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = DOT1S_CFG_VER_1;
    break;

  case DOT1S_CFG_VER_2:
    pCfgV2 = (dot1sCfgV2_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1sCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (dot1sCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = DOT1S_CFG_VER_2;
    break;

  case DOT1S_CFG_VER_3:
    pCfgV3 = (dot1sCfgV3_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1sCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (dot1sCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = DOT1S_CFG_VER_3;
    break;

  case DOT1S_CFG_VER_4:
    pCfgV4 = (dot1sCfgV4_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1sCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (dot1sCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = DOT1S_CFG_VER_4;
    break;

  case DOT1S_CFG_VER_5:
    pCfgV5 = (dot1sCfgV5_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1sCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (dot1sCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = DOT1S_CFG_VER_5;
    break;

  default:
    buildDefault = L7_TRUE;
    oldCfgSize = 0;
    pOldCfgCksum = 0;
    oldCfgVer = 0;
    break;

  }                             /* endswitch */

  if (buildDefault != L7_TRUE)
  {
    /* read the old config file found on this system */

    /* Note:

       The routine sysapiCfgFileGet which invokes the migrate routine will pass
       in the a pointer to the location of the configuration data read from
       nonvolatile storage.

       In cases where the older version of the configuration file may be larger
       than the current version, sysapiCfgFileGet truncates the data to the size
       of the storage location pointed to by the buffer provided for the component.
       Thus, it is important to read the configuration file again so that the entirety
       of the configuration data can be accessed.

     */

    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_DOT1S_COMPONENT_ID,
                                         DOT1S_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1S_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d."
                     " Configuration did not exist or could not be read"
                     " for the specified feature.  This message is usually"
                     " followed by a message indicating that default configuration"
                     " values will be used."
                     , (L7_int32) oldCfgSize,
                     (L7_int32) oldCfgVer);
      buildDefault = L7_TRUE;
    }
  }

  /* Convert the old config file to the latest version.
     Each version of the configuation is iteratively migrated to the
     next version until brought up to the current level.
   */

  if (buildDefault != L7_TRUE)
  {
    switch (oldVer)
    {
    case DOT1S_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (dot1sCfgV2_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1sCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) || (L7_SUCCESS != dot1sMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case DOT1S_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (dot1sCfgV3_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1sCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || (L7_SUCCESS != dot1sMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case DOT1S_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (dot1sCfgV4_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1sCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) || (L7_SUCCESS != dot1sMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case DOT1S_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (dot1sCfgV5_t *) osapiMalloc (L7_DOT1S_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1sCfgV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) || (L7_SUCCESS != dot1sMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case DOT1S_CFG_VER_5:
      if (pCfgV5 != L7_NULLPTR)
      {
        /* pCfgV5 has already been allocated and filled in */
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
        osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV5);
      }

      pCfgV5 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->hdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_DOT1S_COMPONENT_ID, pCfgV5);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dot1sBuildConfigData (DOT1S_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V1 to V2.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
dot1sMigrateConfigV1V2Convert (dot1sCfgV1_t * pCfgV1, dot1sCfgV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_uint32 j;
  L7_uint32 count;
  L7_int32 intfCfgIndex;
  nimConfigID_t configId;
  L7_INTF_TYPES_t intfType;
  DOT1S_BRIDGE_t *pBridge;
  DOT1S_BRIDGE_CFG_V1_t *pBridgeCfg;
  DOT1S_INSTANCE_INFO_V1_t *pInstInfo;
  DOT1S_INSTANCE_INFO_CFG_V2_t *pInstInfoCfg;
  DOT1S_PORT_COMMON_CFG_V1_t *pPortCfgV1;
  DOT1S_PORT_COMMON_CFG_V2_t *pPortCfgV2;
  L7_uint32 minInst = 0;

  /* verify correct version of old config file */
  if (pCfgV1->hdr.version != DOT1S_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->hdr.version, DOT1S_CFG_VER_1);
    return L7_FAILURE;
  }

  dot1sBuildConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */

  /* dot1sBridge */
  pBridge = &pCfgV1->cfg.dot1sBridge;
  pBridgeCfg = &pCfgV2->cfg.dot1sBridge;
  pBridgeCfg->Mode = pBridge->Mode;
  pBridgeCfg->ForceVersion = pBridge->ForceVersion;
  pBridgeCfg->FwdDelay = pBridge->FwdDelay;
  pBridgeCfg->TxHoldCount = pBridge->TxHoldCount;
  pBridgeCfg->MigrateTime = pBridge->MigrateTime;
  pBridgeCfg->MaxHops = pBridge->MaxHops;
  pBridgeCfg->instanceCount = pBridge->instanceCount;
  pBridgeCfg->MstConfigId.formatSelector = pBridge->MstConfigId.formatSelector;
  osapiStrncpySafe (pBridgeCfg->MstConfigId.configName, pBridge->MstConfigId.configName,
           sizeof (pBridgeCfg->MstConfigId.configName));
  pBridgeCfg->MstConfigId.revLevel = pBridge->MstConfigId.revLevel;

  /* dot1sInstance */
  pInstInfo = &pCfgV1->cfg.dot1sInstance;
  pInstInfoCfg = &pCfgV2->cfg.dot1sInstance;
  pInstInfoCfg->cist.cistid = pInstInfo->cist.cistid;
  MIGRATE_COPY_STRUCT (pInstInfoCfg->cist.BridgeIdentifier, pInstInfo->cist.BridgeIdentifier);
  MIGRATE_COPY_STRUCT (pInstInfoCfg->cist.CistBridgeTimes, pInstInfo->cist.CistBridgeTimes);

  minInst = min(L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_0,L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1);
  for (i = 1; i <= minInst; i++)
  {
    pInstInfoCfg->msti[i].mstid = pInstInfo->msti[i].mstid;
    MIGRATE_COPY_STRUCT (pInstInfoCfg->msti[i].BridgeIdentifier,
                         pInstInfo->msti[i].BridgeIdentifier);
    MIGRATE_COPY_STRUCT (pInstInfoCfg->msti[i].MstiBridgeTimes, pInstInfo->msti[i].MstiBridgeTimes);
  }

  /* dot1sCfgPort */
  intfCfgIndex = 1;
  intfType = 0;
  count = min (L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_0, L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_1);
  for (i = 1; i < count; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, &intfType);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (dot1sIsValidIntfType (intfType) != L7_TRUE)
      continue;

    if (intfCfgIndex >= L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (DOT1S_CFG_FILENAME, intfCfgIndex);
      break;
    }

    pPortCfgV1 = &pCfgV1->cfg.dot1sCfgPort[i];
    pPortCfgV2 = &pCfgV2->cfg.dot1sCfgPort[intfCfgIndex];
    (void) nimConfigIdCopy (&configId, &pPortCfgV2->configId);

    pPortCfgV2->adminEdge = pPortCfgV1->adminEdge;
    pPortCfgV2->portAdminMode = pPortCfgV1->portAdminMode;
    pPortCfgV2->HelloTime = pPortCfgV1->HelloTime;

    for (j = 0; j <= minInst; j++)
    {
      pPortCfgV2->portInstInfo[j].ExternalPortPathCost =
        pPortCfgV1->portInstInfo[j].ExternalPortPathCost;
      pPortCfgV2->portInstInfo[j].InternalPortPathCost =
        pPortCfgV1->portInstInfo[j].InternalPortPathCost;
      pPortCfgV2->portInstInfo[j].autoInternalPortPathCost =
        pPortCfgV1->portInstInfo[j].autoInternalPortPathCost;
      pPortCfgV2->portInstInfo[j].portPriority = ((pPortCfgV1->portInstInfo[j].portId & DOT1S_PORT_PRIORITY_MASK) >> 8);
    }
    intfCfgIndex++;
  }

  /* dot1sInstanceMap */
  MIGRATE_COPY_STRUCT (pCfgV2->cfg.dot1sInstanceMap, pCfgV1->cfg.dot1sInstanceMap);

  /* dot1sInstVlanMap */
  MIGRATE_COPY_STRUCT (pCfgV2->cfg.dot1sInstVlanMap, pCfgV1->cfg.dot1sInstVlanMap);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
dot1sMigrateConfigV2V3Convert (dot1sCfgV2_t * pCfgV2, dot1sCfgV3_t * pCfgV3)
{
  L7_uint32 i;
  L7_uint32 j;
  L7_uint32 count;
  L7_uint32 minInst = 0;

  /* verify correct version of old config file */
  if (pCfgV2->hdr.version != DOT1S_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, DOT1S_CFG_VER_2);
    return L7_FAILURE;
  }

  dot1sBuildConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot1sBridge, pCfgV2->cfg.dot1sBridge);
  MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot1sInstance, pCfgV2->cfg.dot1sInstance);
  count = min (L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_1, L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_3);
  minInst = min(L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1,L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3);
  for (i = 1; i < count; i++)
  {
    /* Determine if the interface is valid for participation in this feature */
    if (dot1sIsValidIntfType (pCfgV2->cfg.dot1sCfgPort[i].configId.type) != L7_TRUE)
      continue;

    MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot1sCfgPort[i].configId,
                         pCfgV2->cfg.dot1sCfgPort[i].configId);
    pCfgV3->cfg.dot1sCfgPort[i].adminEdge = pCfgV2->cfg.dot1sCfgPort[i].adminEdge;
    pCfgV3->cfg.dot1sCfgPort[i].portAdminMode = pCfgV2->cfg.dot1sCfgPort[i].portAdminMode;
    pCfgV3->cfg.dot1sCfgPort[i].HelloTime = pCfgV2->cfg.dot1sCfgPort[i].HelloTime;
    for (j = 0; j <= minInst; j++)
    {
      pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].ExternalPortPathCost =
        pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[j].ExternalPortPathCost;
      pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].InternalPortPathCost =
        pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[j].InternalPortPathCost;
      pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].autoInternalPortPathCost =
        pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[j].autoInternalPortPathCost;
      pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].portPriority =
        pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[j].portPriority;
    }
  }
  MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot1sInstanceMap, pCfgV2->cfg.dot1sInstanceMap);
  MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot1sInstVlanMap, pCfgV2->cfg.dot1sInstVlanMap);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
dot1sMigrateConfigV3V4Convert (dot1sCfgV3_t * pCfgV3, dot1sCfgV4_t * pCfgV4)
{
  L7_uint32 i;
  L7_uint32 j;
  L7_uint32 count;
  L7_uint32 minInst = 0;

  /* verify correct version of old config file */
  if (pCfgV3->hdr.version != DOT1S_CFG_VER_3 &&
      pCfgV3->hdr.version != DOT1S_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->hdr.version, DOT1S_CFG_VER_3);
    return L7_FAILURE;
  }

  dot1sBuildConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot1sBridge, pCfgV3->cfg.dot1sBridge);
  /*MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot1sInstance, pCfgV3->cfg.dot1sInstance);*/
  /* The sizes are different. V3 is smaller than V4 sp just copy the size of V3 */
  memcpy((void *)&pCfgV4->cfg.dot1sInstance,(void *)&pCfgV3->cfg.dot1sInstance, sizeof(pCfgV3->cfg.dot1sInstance));
  count = min (L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_3, L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_4);
  minInst = min(L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3,L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_4);
  for (i = 1; i < count; i++)
  {
    /* Determine if the interface is valid for participation in this feature */
    if (dot1sIsValidIntfType (pCfgV3->cfg.dot1sCfgPort[i].configId.type) != L7_TRUE)
      continue;

    MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot1sCfgPort[i].configId,
                         pCfgV3->cfg.dot1sCfgPort[i].configId);
    pCfgV4->cfg.dot1sCfgPort[i].adminEdge = pCfgV3->cfg.dot1sCfgPort[i].adminEdge;
    pCfgV4->cfg.dot1sCfgPort[i].portAdminMode = pCfgV3->cfg.dot1sCfgPort[i].portAdminMode;
    pCfgV4->cfg.dot1sCfgPort[i].HelloTime = pCfgV3->cfg.dot1sCfgPort[i].HelloTime;
    for (j = 0; j <= minInst; j++)
    {
      pCfgV4->cfg.dot1sCfgPort[i].portInstInfo[j].ExternalPortPathCost =
        pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].ExternalPortPathCost;
      pCfgV4->cfg.dot1sCfgPort[i].portInstInfo[j].InternalPortPathCost =
        pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].InternalPortPathCost;
      pCfgV4->cfg.dot1sCfgPort[i].portInstInfo[j].autoInternalPortPathCost =
        pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].autoInternalPortPathCost;
      pCfgV4->cfg.dot1sCfgPort[i].portInstInfo[j].portPriority =
        pCfgV3->cfg.dot1sCfgPort[i].portInstInfo[j].portPriority;
    }
  }
  /*MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot1sInstanceMap, pCfgV3->cfg.dot1sInstanceMap);*/
  /* The sizes are different. V3 size is smaller than V4 */
  memcpy((void *)&pCfgV4->cfg.dot1sInstanceMap, (void *)&pCfgV3->cfg.dot1sInstanceMap, sizeof(pCfgV3->cfg.dot1sInstanceMap));
  MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot1sInstVlanMap, pCfgV3->cfg.dot1sInstVlanMap);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5.
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
dot1sMigrateConfigV4V5Convert (dot1sCfgV4_t * pCfgV4, dot1sCfgV5_t * pCfgV5)
{
  L7_uint32 i;
  L7_uint32 j;
  L7_uint32 count;
  L7_uint32 minInst = 0;

  /* verify correct version of old config file */
  if (pCfgV4->hdr.version != DOT1S_CFG_VER_4 &&
      pCfgV4->hdr.version != DOT1S_CFG_VER_3 &&
      pCfgV4->hdr.version != DOT1S_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->hdr.version, DOT1S_CFG_VER_4);
    return L7_FAILURE;
  }

  dot1sBuildConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->cfg.dot1sBridge.ForceVersion  =  pCfgV4->cfg.dot1sBridge.ForceVersion;
  pCfgV5->cfg.dot1sBridge.FwdDelay      =  pCfgV4->cfg.dot1sBridge.FwdDelay;
  pCfgV5->cfg.dot1sBridge.instanceCount =  pCfgV4->cfg.dot1sBridge.instanceCount;
  pCfgV5->cfg.dot1sBridge.MaxHops       =  pCfgV4->cfg.dot1sBridge.MaxHops;
  pCfgV5->cfg.dot1sBridge.MigrateTime   =  pCfgV4->cfg.dot1sBridge.MigrateTime;
  pCfgV5->cfg.dot1sBridge.Mode          =  pCfgV4->cfg.dot1sBridge.Mode;
  pCfgV5->cfg.dot1sBridge.MstConfigId   =  pCfgV4->cfg.dot1sBridge.MstConfigId;
  pCfgV5->cfg.dot1sBridge.TxHoldCount   =  pCfgV4->cfg.dot1sBridge.TxHoldCount;

  /* The sizes are different. V4 is smaller than V5 sp just copy the size of V4 */
  memcpy((void *)&pCfgV5->cfg.dot1sInstance,(void *)&pCfgV4->cfg.dot1sInstance, sizeof(pCfgV4->cfg.dot1sInstance));
  count = min (L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_4, L7_DOT1S_MAX_INTERFACE_COUNT);

  minInst = min(L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_4,L7_MAX_MULTIPLE_STP_INSTANCES);
  for (i = 1; i < count; i++)
  {
    /* Determine if the interface is valid for participation in this feature */
    if (dot1sIsValidIntfType (pCfgV4->cfg.dot1sCfgPort[i].configId.type) != L7_TRUE)
      continue;

    MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot1sCfgPort[i].configId,pCfgV4->cfg.dot1sCfgPort[i].configId);
    pCfgV5->cfg.dot1sCfgPort[i].adminEdge = pCfgV4->cfg.dot1sCfgPort[i].adminEdge;
    pCfgV5->cfg.dot1sCfgPort[i].portAdminMode = pCfgV4->cfg.dot1sCfgPort[i].portAdminMode;
    pCfgV5->cfg.dot1sCfgPort[i].HelloTime = pCfgV4->cfg.dot1sCfgPort[i].HelloTime;
    for (j = 0; j <= minInst; j++)
    {
      MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot1sCfgPort[i].portInstInfo[j],pCfgV4->cfg.dot1sCfgPort[i].portInstInfo[j]);
    }
  }
  /*MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot1sInstanceMap, pCfgV3->cfg.dot1sInstanceMap);*/
  /* The sizes are different. V4 size is smaller than V5 */
  memcpy((void *)&pCfgV5->cfg.dot1sInstanceMap, (void *)&pCfgV4->cfg.dot1sInstanceMap, sizeof(pCfgV4->cfg.dot1sInstanceMap));
  MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot1sInstVlanMap, pCfgV4->cfg.dot1sInstVlanMap);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Build Version 2 defaults
*
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1sBuildConfigDataV2 (dot1sCfgV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i = 0, inst = 0;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uchar8 cfgName[DOT1S_MAX_CONFIG_NAME_SIZE];
  DOT1S_BRIDGE_CFG_V1_t *cfgBridge;
  DOT1S_INSTANCE_INFO_CFG_V2_t *cfgInstance;

  memset ((L7_char8 *) pCfgV2, 0, (L7_int32) sizeof (*pCfgV2));

  /* default values for the dot1sBridge structure */
  cfgBridge = &pCfgV2->cfg.dot1sBridge;

  cfgBridge->Mode = FD_DOT1S_MODE;
  cfgBridge->ForceVersion = FD_DOT1S_FORCE_VERSION;
  cfgBridge->FwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * 256;
  cfgBridge->TxHoldCount = 3;
  cfgBridge->MigrateTime = 3 * 256;
  cfgBridge->MaxHops = FD_DOT1S_BRIDGE_MAX_AGE;
  cfgBridge->instanceCount = 0;
  cfgBridge->MstConfigId.formatSelector = 0;
  dot1sBaseMacAddrGet (mac);
  sprintf ((L7_char8 *) cfgName, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3],
           mac[4], mac[5]);
  strcpy ((L7_uchar8 *) & cfgBridge->MstConfigId.configName, cfgName);
  cfgBridge->MstConfigId.revLevel = 0;

  /* default values for the dot1sInstVlanMap structure */
  for (i = 0; i < L7_DOT1Q_MAX_VLAN_ID + 2; i++)
  {
    pCfgV2->cfg.dot1sInstVlanMap[i].instNumber = 0;
  }

  /* calculate the MSTP Configuration Digest after the Instance to Vlan Map is
   * initialized.
   */
  dot1sInstVlanMapEndianSafeMake (pCfgV2->cfg.dot1sInstVlanMap);

  cfgInstance = &pCfgV2->cfg.dot1sInstance;

  /* default values for the cist dot1sInstance structure */
  rc = dot1sCfgInstanceDefaultPopulate ((DOT1S_INSTANCE_INFO_CFG_t *)cfgInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);

  /* default values for the msti dot1sInstance structures */
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1; i++)
  {
    rc = dot1sCfgInstanceDefaultPopulate ((DOT1S_INSTANCE_INFO_CFG_t *)cfgInstance, i, L7_NULL);
  }

  /* default values for the dot1sInstanceMap structure */
  pCfgV2->cfg.dot1sInstanceMap[0].instNumber = DOT1S_CIST_ID;
  pCfgV2->cfg.dot1sInstanceMap[0].inUse = L7_TRUE;
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1; i++)
  {
    pCfgV2->cfg.dot1sInstanceMap[i].instNumber = 0;
    pCfgV2->cfg.dot1sInstanceMap[i].inUse = L7_FALSE;
  }

  for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_1; i++)
  {
    pCfgV2->cfg.dot1sCfgPort[i].adminEdge = 0;
    pCfgV2->cfg.dot1sCfgPort[i].portAdminMode = FD_DOT1S_PORT_MODE;
    pCfgV2->cfg.dot1sCfgPort[i].HelloTime = FD_DOT1S_BRIDGE_HELLO_TIME * 256;
    for (inst = 0; inst <= L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_1; inst++)
    {
      pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[inst].ExternalPortPathCost = FD_DOT1S_PORT_PATH_COST;
      pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[inst].InternalPortPathCost = FD_DOT1S_PORT_PATH_COST;

      /* Update the Auto Port Path Cost setting */
      if (FD_DOT1S_PORT_PATH_COST == L7_DOT1S_AUTO_CALC_PATH_COST)
      {
        pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[inst].autoInternalPortPathCost = L7_TRUE;
      }
      else
      {
        pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[inst].autoInternalPortPathCost = L7_FALSE;
      }

      pCfgV2->cfg.dot1sCfgPort[i].portInstInfo[inst].portPriority =
        (DOT1S_PORTID_t) (FD_DOT1S_PORT_PRIORITY);
    }
  }

  /* setup file header */
  pCfgV2->hdr.version = DOT1S_CFG_VER_2;
  pCfgV2->hdr.componentID = L7_DOT1S_COMPONENT_ID;
  pCfgV2->hdr.type = L7_CFG_DATA;
  pCfgV2->hdr.length = (L7_uint32) sizeof (*pCfgV2);

  strcpy ((L7_char8 *) pCfgV2->hdr.filename, DOT1S_CFG_FILENAME);
  pCfgV2->hdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build Version 3 defaults
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1sBuildConfigDataV3 (dot1sCfgV3_t * pCfgV3)
{
  L7_RC_t rc;
  L7_uint32 i = 0;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uchar8 cfgName[DOT1S_MAX_CONFIG_NAME_SIZE];
  DOT1S_BRIDGE_CFG_V1_t *cfgBridge;
  DOT1S_INSTANCE_INFO_CFG_V3_t *cfgInstance;

  memset((void *)pCfgV3, 0, sizeof(*pCfgV3));

  /* setup file header */
  pCfgV3->hdr.version = DOT1S_CFG_VER_3;
  pCfgV3->hdr.componentID = L7_DOT1S_COMPONENT_ID;
  pCfgV3->hdr.type = L7_CFG_DATA;
  pCfgV3->hdr.length = (L7_uint32)sizeof(*pCfgV3);

  strcpy((L7_char8 *)pCfgV3->hdr.filename, DOT1S_CFG_FILENAME);
  pCfgV3->hdr.dataChanged = L7_FALSE;

  /* default values for the dot1sBridge structure */
  cfgBridge = &pCfgV3->cfg.dot1sBridge;

  cfgBridge->Mode = FD_DOT1S_MODE;
  cfgBridge->ForceVersion = FD_DOT1S_FORCE_VERSION;
  cfgBridge->FwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
  cfgBridge->TxHoldCount = 3;
  cfgBridge->MigrateTime = 3 * DOT1S_TIMER_UNIT;
  cfgBridge->MaxHops = FD_DOT1S_BRIDGE_MAX_HOP;
  cfgBridge->instanceCount = 0;
  cfgBridge->MstConfigId.formatSelector = 0;
  dot1sBaseMacAddrGet(mac);
  sprintf((L7_char8 *)cfgName, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  memcpy((L7_uchar8 *)&cfgBridge->MstConfigId.configName, cfgName, sizeof(cfgName));
  cfgBridge->MstConfigId.revLevel = 0;

  /* default values for the dot1sInstVlanMap structure */
  for (i = 0; i < L7_DOT1Q_MAX_VLAN_ID + 2; i++)
  {
    pCfgV3->cfg.dot1sInstVlanMap[i].instNumber = 0;
  }

  /* calculate the MSTP Configuration Digest after the Instance to Vlan Map is
    * initialized.
    */
  dot1sInstVlanMapEndianSafeMake(pCfgV3->cfg.dot1sInstVlanMap);

  cfgInstance = &pCfgV3->cfg.dot1sInstance;

  /* default values for the cist dot1sInstance structure */
  rc = dot1sCfgInstanceDefaultPopulate((DOT1S_INSTANCE_INFO_CFG_t *)cfgInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);

  /* default values for the msti dot1sInstance structures */
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3; i++)
  {
    rc = dot1sCfgInstanceDefaultPopulate((DOT1S_INSTANCE_INFO_CFG_t *)cfgInstance, i, L7_NULL);
  }

  /* default values for the dot1sInstanceMap structure */
  pCfgV3->cfg.dot1sInstanceMap[0].instNumber = DOT1S_CIST_ID;
  pCfgV3->cfg.dot1sInstanceMap[0].inUse = L7_TRUE;
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_3; i++)
  {
    pCfgV3->cfg.dot1sInstanceMap[i].instNumber = 0;
    pCfgV3->cfg.dot1sInstanceMap[i].inUse = L7_FALSE;
  }

  for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT_REL_4_3; i++)
  {
    dot1sPortDefaultConfigDataBuild ((DOT1S_PORT_COMMON_CFG_t *)&pCfgV3->cfg.dot1sCfgPort[i]);
  }
}

/*********************************************************************
* @purpose  Build Version 4 defaults
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1sBuildConfigDataV4 (dot1sCfgV4_t * pCfgV4)
{
  L7_RC_t rc;
  L7_uint32 i = 0;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];
  L7_uchar8 cfgName[DOT1S_MAX_CONFIG_NAME_SIZE];
  DOT1S_BRIDGE_CFG_V1_t *cfgBridge;
  DOT1S_INSTANCE_INFO_CFG_t  *cfgInstance;


  memset((void *)pCfgV4, 0, sizeof(*pCfgV4));

  /* setup file header */
  pCfgV4->hdr.version = DOT1S_CFG_VER_4;
  pCfgV4->hdr.componentID = L7_DOT1S_COMPONENT_ID;
  pCfgV4->hdr.type = L7_CFG_DATA;
  pCfgV4->hdr.length = (L7_uint32)sizeof(*pCfgV4);

  strcpy((L7_char8 *)pCfgV4->hdr.filename, DOT1S_CFG_FILENAME);
  pCfgV4->hdr.dataChanged = L7_FALSE;

  /* default values for the dot1sBridge structure */
  cfgBridge = &pCfgV4->cfg.dot1sBridge;

  cfgBridge->Mode = FD_DOT1S_MODE;
  cfgBridge->ForceVersion = FD_DOT1S_FORCE_VERSION;
  cfgBridge->FwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT;
  cfgBridge->TxHoldCount = 3;
  cfgBridge->MigrateTime = 3 * DOT1S_TIMER_UNIT;
  cfgBridge->MaxHops = FD_DOT1S_BRIDGE_MAX_HOP;
  cfgBridge->instanceCount = 0;
  cfgBridge->MstConfigId.formatSelector = 0;
  dot1sBaseMacAddrGet(mac);
  sprintf((L7_char8 *)cfgName, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  memcpy((L7_uchar8 *)&cfgBridge->MstConfigId.configName, cfgName, sizeof(cfgName));
  cfgBridge->MstConfigId.revLevel = 0;

  /* default values for the dot1sInstVlanMap structure */
  for (i = 0; i < L7_MAX_VLAN_ID + 2; i++)
  {
    pCfgV4->cfg.dot1sInstVlanMap[i].instNumber = 0;
  }

  /* calculate the MSTP Configuration Digest after the Instance to Vlan Map is
    * initialized.
    */
  dot1sInstVlanMapEndianSafeMake(pCfgV4->cfg.dot1sInstVlanMap);

  cfgInstance = &pCfgV4->cfg.dot1sInstance;

  /* default values for the cist dot1sInstance structure */
  rc = dot1sCfgInstanceDefaultPopulate((DOT1S_INSTANCE_INFO_CFG_t *)cfgInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);

  /* default values for the msti dot1sInstance structures */
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
  {
    rc = dot1sCfgInstanceDefaultPopulate((DOT1S_INSTANCE_INFO_CFG_t *)cfgInstance, i, L7_NULL);
  }

  /* default values for the dot1sInstanceMap structure */
  pCfgV4->cfg.dot1sInstanceMap[0].instNumber = DOT1S_CIST_ID;
  pCfgV4->cfg.dot1sInstanceMap[0].inUse = L7_TRUE;
  for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES_REL_4_4; i++)
  {
    pCfgV4->cfg.dot1sInstanceMap[i].instNumber = 0;
    pCfgV4->cfg.dot1sInstanceMap[i].inUse = L7_FALSE;
  }

  /*populate the port configuration */
  for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT; i++)
  {
    L7_uint32 inst;
    DOT1S_PORT_COMMON_CFG_V4_t *portCfg;

    portCfg = &pCfgV4->cfg.dot1sCfgPort[i];

    portCfg->adminEdge = 0;
    portCfg->portAdminMode = FD_DOT1S_PORT_MODE;
    portCfg->HelloTime = DOT1S_INVALID_HELLO_TIME/*dot1sInstance->cist.CistBridgeTimes.bridgeHelloTime*/;
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

      portCfg->portInstInfo[inst].portPriority = (DOT1S_PORTID_t)(FD_DOT1S_PORT_PRIORITY);
    }
  }
}

/*********************************************************************
* @purpose  Build Version 5 defaults
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1sBuildConfigDataV5 (dot1sCfgV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (dot1sCfgV5_t));
  dot1sBuildConfigData (DOT1S_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

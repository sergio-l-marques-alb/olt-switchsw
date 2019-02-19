
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename rip_map_migrate.c
*
* @purpose RIP Configuration Migration
*
* @component RIP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "rip_map_migrate.h"

static L7_RC_t ripMapMigrateConfigV3V4Convert (ripMapCfgV3_t * pCfgV3, ripMapCfgV4_t * pCfgV4);
static L7_RC_t ripMapMigrateConfigV4V5Convert (ripMapCfgV4_t * pCfgV4, ripMapCfgV5_t * pCfgV5);
static L7_RC_t ripMapMigrateConfigV5V6Convert (ripMapCfgV5_t * pCfgV5, ripMapCfgV6_t * pCfgV6);
static L7_RC_t ripMapMigrateConfigV6V7Convert (ripMapCfgV6_t * pCfgV6, ripMapCfgV7_t * pCfgV7);
static void ripMapBuildDefaultConfigDataV4 (ripMapCfgV4_t * pCfgV4);
static void ripMapBuildDefaultConfigDataV5 (ripMapCfgV5_t * pCfgV5);
static void ripMapBuildDefaultConfigDataV6 (ripMapCfgV6_t * pCfgV6);
static void ripMapBuildDefaultConfigDataV7 (ripMapCfgV7_t * pCfgV7);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static ripMapCfg_t *pCfgCur = L7_NULLPTR;

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
ripMapMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  ripMapCfgV3_t *pCfgV3 = L7_NULLPTR;
  ripMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  ripMapCfgV5_t *pCfgV5 = L7_NULLPTR;
  ripMapCfgV6_t *pCfgV6 = L7_NULLPTR;
  ripMapCfgV7_t *pCfgV7 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  ver = ver;
  pCfgCur = (ripMapCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == L7_RIP_MAP_CFG_VER_3 && pCfgHdr->length == sizeof (ripMapCfgV4_t))
  {
    oldVer = L7_RIP_MAP_CFG_VER_4;
  }
  else if (pCfgHdr->version == L7_RIP_MAP_CFG_VER_2 && pCfgHdr->length == sizeof (ripMapCfgV4_t))
  {
    oldVer = L7_RIP_MAP_CFG_VER_4;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_RIP_MAP_CFG_VER_3:
    pCfgV3 = (ripMapCfgV3_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                            (L7_uint32) sizeof (ripMapCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (ripMapCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_RIP_MAP_CFG_VER_3;
    break;

  case L7_RIP_MAP_CFG_VER_4:
    pCfgV4 = (ripMapCfgV4_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                            (L7_uint32) sizeof (ripMapCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (ripMapCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case L7_RIP_MAP_CFG_VER_5:
    pCfgV5 = (ripMapCfgV5_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                            (L7_uint32) sizeof (ripMapCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (ripMapCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case L7_RIP_MAP_CFG_VER_6:
    pCfgV6 = (ripMapCfgV6_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                            (L7_uint32) sizeof (ripMapCfgV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (ripMapCfgV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case L7_RIP_MAP_CFG_VER_7:
    pCfgV7 = (ripMapCfgV7_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                            (L7_uint32) sizeof (ripMapCfgV7_t));
    if (L7_NULLPTR == pCfgV7)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV7;
    oldCfgSize = (L7_uint32) sizeof (ripMapCfgV7_t);
    pOldCfgCksum = &pCfgV7->checkSum;
    oldCfgVer = pCfgHdr->version;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_RIP_MAP_COMPONENT_ID,
                                         L7_RIP_MAP_CFG_FILENAME, pOldCfgBuf,
                                         oldCfgSize, pOldCfgCksum, oldCfgVer,
                                         L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
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
    case L7_RIP_MAP_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (ripMapCfgV4_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (ripMapCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != ripMapMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_RIP_MAP_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (ripMapCfgV5_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (ripMapCfgV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != ripMapMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case L7_RIP_MAP_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (ripMapCfgV6_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (ripMapCfgV6_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_SUCCESS != ripMapMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case L7_RIP_MAP_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      pCfgV7 = (ripMapCfgV7_t *) osapiMalloc (L7_RIP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (ripMapCfgV7_t));
      if ((L7_NULLPTR == pCfgV7) || (L7_SUCCESS != ripMapMigrateConfigV6V7Convert (pCfgV6, pCfgV7)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;
      /*passthru */

    case L7_RIP_MAP_CFG_VER_7:
      /* pCfgV7 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV7, sizeof (*pCfgCur));
      osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV7);
      pCfgV7 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV6);
  if (pCfgV7 != L7_NULLPTR)
    osapiFree (L7_RIP_MAP_COMPONENT_ID, pCfgV7);


  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    ripMapBuildDefaultConfigData (L7_RIP_MAP_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}


/*********************************************************************
* @purpose  Converts the config data structure from version V1 to V2.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2        @b{(input)} ptr to version 2 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static void
ripMapMigrateIntfConfigV3V4Convert (ripMapCfgV3_t * pCfgV3, ripMapCfgV4_t * pCfgV4,
                                    L7_uint32 intfType, L7_uint32 * offset)
{
  L7_RC_t rc;
  L7_uint32 i, j, intCfgIndex;
  nimConfigID_t configId;
  L7_BOOL truncated;

           
  /* Note: The methodology used by the policyMigrate* routines in migrating different 
           types  of interfaces is more mature than the multiple invocations
           of the same routine implemented for this routine as fewer loop checks
           are required. However, this works, so we go with it. */ 

  /* Note: The truncated flag merely indicates if the migration was truncated within
           this invocation of this routine. */ 

  truncated = L7_FALSE;
  
  intCfgIndex = *offset;
  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    for (j = 1; j <= L7_MAX_PORTS_PER_SLOT_REL_4_0; j++)
    {
      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (i, j, &configId, L7_NULL, L7_NULL);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (i, j);
        continue;
      }

      if (configId.type != intfType)
        continue;

      if (intCfgIndex >= L7_RIP_MAP_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_INDEX_OUTOF_BOUND (intCfgIndex);
        continue;
      }

      if (intCfgIndex >= L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE (L7_RIP_MAP_CFG_FILENAME, intCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV4->intf[intCfgIndex].configId);
      pCfgV4->intf[intCfgIndex].adminMode = pCfgV3->ckt[i][j].adminMode;
      pCfgV4->intf[intCfgIndex].authType = pCfgV3->ckt[i][j].authType;
      MIGRATE_COPY_STRUCT (pCfgV4->intf[intCfgIndex].authKey, pCfgV3->ckt[i][j].authKey);
      pCfgV4->intf[intCfgIndex].authKeyLen = pCfgV3->ckt[i][j].authKeyLen;
      pCfgV4->intf[intCfgIndex].authKeyId = pCfgV3->ckt[i][j].authKeyId;
      pCfgV4->intf[intCfgIndex].verSend = pCfgV3->ckt[i][j].verSend;
      pCfgV4->intf[intCfgIndex].verRecv = pCfgV3->ckt[i][j].verRecv;
      pCfgV4->intf[intCfgIndex].intfCost = pCfgV3->ckt[i][j].intfCost;
      intCfgIndex++;
    }

    if (truncated == L7_TRUE)
        break;

  }
  *offset = intCfgIndex;
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
ripMapMigrateConfigV3V4Convert (ripMapCfgV3_t * pCfgV3, ripMapCfgV4_t * pCfgV4)
{
  L7_uint32 intCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_RIP_MAP_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_RIP_MAP_CFG_VER_3);
    return L7_FAILURE;
  }

  ripMapBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->ripAdminMode = pCfgV3->rtr.ripAdminMode;
  pCfgV4->ripSplitHorizon = pCfgV3->rtr.ripSplitHorizon;
  pCfgV4->ripAutoSummarization = pCfgV3->rtr.ripAutoSummarization;
  pCfgV4->ripHostRoutesAccept = pCfgV3->rtr.ripHostRoutesAccept;
  pCfgV4->ripDefaultMetric = pCfgV3->rtr.ripDefaultMetric;
  MIGRATE_COPY_STRUCT (pCfgV4->ripRtRedist, pCfgV3->rtr.ripRtRedist);
  MIGRATE_COPY_STRUCT (pCfgV4->ripDefRtAdv, pCfgV3->rtr.ripDefRtAdv);

  intCfgIndex = 1;
  ripMapMigrateIntfConfigV3V4Convert (pCfgV3, pCfgV4, L7_PHYSICAL_INTF, &intCfgIndex);


  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      ripMapMigrateIntfConfigV3V4Convert (pCfgV3, pCfgV4, L7_LOGICAL_VLAN_INTF, &intCfgIndex);


  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      ripMapMigrateIntfConfigV3V4Convert (pCfgV3, pCfgV4, L7_LAG_INTF, &intCfgIndex);

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
ripMapMigrateConfigV4V5Convert (ripMapCfgV4_t * pCfgV4, ripMapCfgV5_t * pCfgV5)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != L7_RIP_MAP_CFG_VER_4 &&
      pCfgV4->cfgHdr.version != L7_RIP_MAP_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_RIP_MAP_CFG_VER_4);
    return L7_FAILURE;
  }

  ripMapBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->ripAdminMode = pCfgV4->ripAdminMode;
  pCfgV5->ripSplitHorizon = pCfgV4->ripSplitHorizon;
  pCfgV5->ripAutoSummarization = pCfgV4->ripAutoSummarization;
  pCfgV5->ripHostRoutesAccept = pCfgV4->ripHostRoutesAccept;
  pCfgV5->ripDefaultMetric = pCfgV4->ripDefaultMetric;
  MIGRATE_COPY_STRUCT (pCfgV5->ripRtRedist, pCfgV4->ripRtRedist);
  MIGRATE_COPY_STRUCT (pCfgV5->ripDefRtAdv, pCfgV4->ripDefRtAdv);

  intIfCount = min (L7_RIP_MAP_INTF_MAX_COUNT_REL_4_1, L7_RIP_MAP_INTF_MAX_COUNT_REL_4_3);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->intf[i], pCfgV4->intf[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V5 to V6.
*
* @param    pCfgV5    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV6    @b{(input)} ptr to version 5 config data structure
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
ripMapMigrateConfigV5V6Convert (ripMapCfgV5_t * pCfgV5, ripMapCfgV6_t * pCfgV6)
{
  L7_uint32 i, intIfCount, redistCount;

  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != L7_RIP_MAP_CFG_VER_5 &&
      pCfgV5->cfgHdr.version != L7_RIP_MAP_CFG_VER_4 &&
      pCfgV5->cfgHdr.version != L7_RIP_MAP_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, L7_RIP_MAP_CFG_VER_5);
    return L7_FAILURE;
  }

  ripMapBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV6->ripAdminMode = pCfgV5->ripAdminMode;
  pCfgV6->ripSplitHorizon = pCfgV5->ripSplitHorizon;
  pCfgV6->ripAutoSummarization = pCfgV5->ripAutoSummarization;
  pCfgV6->ripHostRoutesAccept = pCfgV5->ripHostRoutesAccept;
  pCfgV6->ripDefaultMetric = pCfgV5->ripDefaultMetric;
  MIGRATE_COPY_STRUCT (pCfgV6->ripDefRtAdv, pCfgV5->ripDefRtAdv);

  redistCount = min(REDIST_RT_LAST_REL_4_3, REDIST_RT_LAST_REL_4_4);
  for (i = REDIST_RT_FIRST + 1; i < redistCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV6->ripRtRedist[i], pCfgV5->ripRtRedist[i]);
  }

  intIfCount = min (L7_RIP_MAP_INTF_MAX_COUNT_REL_4_3, L7_RIP_MAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV6->intf[i], pCfgV5->intf[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V6 to V7.
*
* @param    pCfgV6    @b{(input)} ptr to version 6 config data structure
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
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
ripMapMigrateConfigV6V7Convert (ripMapCfgV6_t * pCfgV6, ripMapCfgV7_t * pCfgV7)
{
  L7_uint32 i, intIfCount, redistCount;

  /* verify correct version of old config file */
  if (pCfgV6->cfgHdr.version != L7_RIP_MAP_CFG_VER_6 &&
      pCfgV6->cfgHdr.version != L7_RIP_MAP_CFG_VER_5 &&
      pCfgV6->cfgHdr.version != L7_RIP_MAP_CFG_VER_4 &&
      pCfgV6->cfgHdr.version != L7_RIP_MAP_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV6->cfgHdr.version, L7_RIP_MAP_CFG_VER_6);
    return L7_FAILURE;
  }

  ripMapBuildDefaultConfigDataV7 (pCfgV7);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV7->ripAdminMode = pCfgV6->ripAdminMode;
  pCfgV7->ripSplitHorizon = pCfgV6->ripSplitHorizon;
  pCfgV7->ripAutoSummarization = pCfgV6->ripAutoSummarization;
  pCfgV7->ripHostRoutesAccept = pCfgV6->ripHostRoutesAccept;
  pCfgV7->ripDefaultMetric = pCfgV6->ripDefaultMetric;
  MIGRATE_COPY_STRUCT (pCfgV7->ripDefRtAdv, pCfgV6->ripDefRtAdv);

  redistCount = min(REDIST_RT_LAST_REL_4_4, REDIST_RT_LAST);
  for (i = REDIST_RT_FIRST + 1; i < redistCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV7->ripRtRedist[i], pCfgV6->ripRtRedist[i]);
  }

  intIfCount = min (L7_RIP_MAP_INTF_MAX_COUNT_REL_4_4, L7_RIP_MAP_INTF_MAX_COUNT);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV7->intf[i], pCfgV6->intf[i]);
  }

  return L7_SUCCESS;
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
ripMapBuildDefaultConfigDataV4 (ripMapCfgV4_t * pCfgV4)
{
  L7_uint32 index;
  L7_uint32 cfgIndex;

  memset (pCfgV4, 0, sizeof (*pCfgV4));

  /* generic RIP cfg */
  pCfgV4->ripAdminMode = FD_RIP_MAP_DEFAULT_ADMIN_MODE;
  pCfgV4->ripSplitHorizon = FD_RIP_MAP_DEFAULT_SPLITHORIZON_MODE;
  pCfgV4->ripAutoSummarization = FD_RIP_MAP_DEFAULT_AUTOSUMMARY_MODE;
  pCfgV4->ripHostRoutesAccept = FD_RIP_MAP_DEFAULT_HOSTROUTESACCEPT_MODE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1; cfgIndex++)
  {
    L7_uint32 k;
    ripMapCfgIntf_t *pCfg = &pCfgV4->intf[cfgIndex];

    pCfg->adminMode = FD_RIP_INTF_DEFAULT_ADMIN_MODE;
    pCfg->authType = FD_RIP_INTF_DEFAULT_AUTH_TYPE;

    for (k = 0; k < L7_AUTH_MAX_KEY_RIP; k++)
      pCfg->authKey[k] = FD_RIP_INTF_DEFAULT_AUTH_KEY_CHAR;

    pCfg->authKeyLen = FD_RIP_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfg->authKeyId = FD_RIP_INTF_DEFAULT_AUTH_KEY_ID;
    pCfg->verSend = FD_RIP_INTF_DEFAULT_VER_SEND;
    pCfg->verRecv = FD_RIP_INTF_DEFAULT_VER_RECV;
    pCfg->intfCost = FD_RIP_INTF_DEFAULT_INTF_COST;

  }

  pCfgV4->ripDefaultMetric = FD_RIP_DEFAULT_DEFAULT_METRIC;
  pCfgV4->ripDefRtAdv.defRtAdv = FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE;
  for (index = REDIST_RT_FIRST + 1; index < REDIST_RT_LAST_REL_4_1; index++)
  {
    pCfgV4->ripRtRedist[index].redistribute = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
    pCfgV4->ripRtRedist[index].metric = FD_RIP_DEFAULT_REDIST_ROUTE_METRIC;
    pCfgV4->ripRtRedist[index].matchType = FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE;
    pCfgV4->ripRtRedist[index].ripRtRedistFilter.mode = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
    pCfgV4->ripRtRedist[index].ripRtRedistFilter.filter = FD_RIP_DEFAULT_REDIST_ROUTE_FILTER;
  }

  strcpy (pCfgV4->cfgHdr.filename, L7_RIP_MAP_CFG_FILENAME);
  pCfgV4->cfgHdr.version = L7_RIP_MAP_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_RIP_MAP_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = sizeof (*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;

}

/*********************************************************************
* @purpose  Build Version 5 defaults
*
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
void
ripMapBuildDefaultConfigDataV5 (ripMapCfgV5_t * pCfgV5)
{
  L7_uint32 index;
  L7_uint32 cfgIndex;

  memset (pCfgV5, 0, sizeof (*pCfgV5));

  /* generic RIP cfg */
  pCfgV5->ripAdminMode = FD_RIP_MAP_DEFAULT_ADMIN_MODE;
  pCfgV5->ripSplitHorizon = FD_RIP_MAP_DEFAULT_SPLITHORIZON_MODE;
  pCfgV5->ripAutoSummarization = FD_RIP_MAP_DEFAULT_AUTOSUMMARY_MODE;
  pCfgV5->ripHostRoutesAccept = FD_RIP_MAP_DEFAULT_HOSTROUTESACCEPT_MODE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_3; cfgIndex++)
  {
    L7_uint32 k;
    ripMapCfgIntf_t *pCfg = &pCfgV5->intf[cfgIndex];

    pCfg->adminMode = FD_RIP_INTF_DEFAULT_ADMIN_MODE;
    pCfg->authType = FD_RIP_INTF_DEFAULT_AUTH_TYPE;

    for (k = 0; k < L7_AUTH_MAX_KEY_RIP; k++)
      pCfg->authKey[k] = FD_RIP_INTF_DEFAULT_AUTH_KEY_CHAR;

    pCfg->authKeyLen = FD_RIP_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfg->authKeyId = FD_RIP_INTF_DEFAULT_AUTH_KEY_ID;
    pCfg->verSend = FD_RIP_INTF_DEFAULT_VER_SEND;
    pCfg->verRecv = FD_RIP_INTF_DEFAULT_VER_RECV;
    pCfg->intfCost = FD_RIP_INTF_DEFAULT_INTF_COST;

  }

  pCfgV5->ripDefaultMetric = FD_RIP_DEFAULT_DEFAULT_METRIC;
  pCfgV5->ripDefRtAdv.defRtAdv = FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE;
  for (index = REDIST_RT_FIRST + 1; index < REDIST_RT_LAST_REL_4_3; index++)
  {
    pCfgV5->ripRtRedist[index].redistribute = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
    pCfgV5->ripRtRedist[index].metric = FD_RIP_DEFAULT_REDIST_ROUTE_METRIC;
    pCfgV5->ripRtRedist[index].matchType = FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE;
    pCfgV5->ripRtRedist[index].ripRtRedistFilter.mode = FD_RIP_DEFAULT_REDIST_ROUTE_MODE;
    pCfgV5->ripRtRedist[index].ripRtRedistFilter.filter = FD_RIP_DEFAULT_REDIST_ROUTE_FILTER;
  }

  strcpy (pCfgV5->cfgHdr.filename, L7_RIP_MAP_CFG_FILENAME);
  pCfgV5->cfgHdr.version = L7_RIP_MAP_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_RIP_MAP_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = sizeof (*pCfgV5);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;

}

/*********************************************************************
* @purpose  Build Version 6 defaults
*
* @param    pCfgV6    @b{(input)} ptr to version 6 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
void
ripMapBuildDefaultConfigDataV6 (ripMapCfgV6_t * pCfgV6)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (ripMapCfgV6_t));
  ripMapBuildDefaultConfigData (L7_RIP_MAP_CFG_VER_6);
  memcpy ((L7_uchar8 *) pCfgV6, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

/*********************************************************************
* @purpose  Build Version 7 defaults
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
void
ripMapBuildDefaultConfigDataV7 (ripMapCfgV7_t * pCfgV7)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (ripMapCfgV7_t));
  ripMapBuildDefaultConfigData (L7_RIP_MAP_CFG_VER_7);
  memcpy ((L7_uchar8 *) pCfgV7, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

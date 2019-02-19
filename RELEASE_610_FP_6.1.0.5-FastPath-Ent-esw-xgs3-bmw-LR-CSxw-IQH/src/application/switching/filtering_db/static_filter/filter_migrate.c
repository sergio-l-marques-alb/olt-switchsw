
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    filter_migrate.c
* @purpose     Port filter Configuration Migration
* @component   filter
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @end
*             
**********************************************************************/

#include "filter_migrate.h"

static L7_RC_t filterMigrateConfigV1V2Convert (filterCfgDataV1_t * pCfgV1,
                                               filterCfgDataV2_t * pCfgV2);

static L7_RC_t filterMigrateConfigV2V3Convert (filterCfgDataV2_t * pCfgV2,
                                               filterCfgDataV3_t * pCfgV3);

static L7_RC_t filterMigrateConfigV3V4Convert (filterCfgDataV3_t * pCfgV3,
                                               filterCfgDataV4_t * pCfgV4);

static L7_RC_t filterMigrateConfigV4V5Convert (filterCfgDataV4_t * pCfgV4,
                                               filterCfgDataV5_t * pCfgV5);

static void filterBuildDefaultConfigDataV2 (filterCfgDataV2_t * pCfgV2);
static void filterBuildDefaultConfigDataV3 (filterCfgDataV3_t * pCfgV3);
static void filterBuildDefaultConfigDataV4 (filterCfgDataV4_t * pCfgV4);
static void filterBuildDefaultConfigDataV5 (filterCfgDataV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static filterCfgData_t *pCfgCur = L7_NULLPTR;

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
filterMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  filterCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  filterCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  filterCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  filterCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  filterCfgDataV5_t *pCfgV5 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  ver = ver;
  pCfgCur = (filterCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case FILTER_CFG_VER_1:
    pCfgV1 = (filterCfgDataV1_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                (L7_uint32) sizeof (filterCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (filterCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = FILTER_CFG_VER_1;
    break;

  case FILTER_CFG_VER_2:
    pCfgV2 = (filterCfgDataV2_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                (L7_uint32) sizeof (filterCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (filterCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = FILTER_CFG_VER_2;
    break;

  case FILTER_CFG_VER_3:
    pCfgV3 = (filterCfgDataV3_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                (L7_uint32) sizeof (filterCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (filterCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = FILTER_CFG_VER_3;
    break;

  case FILTER_CFG_VER_4:
    pCfgV4 = (filterCfgDataV4_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                (L7_uint32) sizeof (filterCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (filterCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = FILTER_CFG_VER_4;
    break;

  case FILTER_CFG_VER_5:
    pCfgV5 = (filterCfgDataV5_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                (L7_uint32) sizeof (filterCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (filterCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = FILTER_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_FILTER_COMPONENT_ID,
                                         FILTER_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FILTER_COMPONENT_ID,
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
    case FILTER_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (filterCfgDataV2_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                  (L7_uint32) sizeof (filterCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) || (L7_SUCCESS != filterMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FILTER_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case FILTER_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (filterCfgDataV3_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                  (L7_uint32) sizeof (filterCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || (L7_SUCCESS != filterMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FILTER_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case FILTER_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (filterCfgDataV4_t *) osapiMalloc (L7_FILTER_COMPONENT_ID,
                                                  (L7_uint32) sizeof (filterCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) || (L7_SUCCESS != filterMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FILTER_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case FILTER_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (filterCfgDataV5_t *) osapiMalloc(L7_FILTER_COMPONENT_ID,
                                                 (L7_uint32) sizeof(filterCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) || (L7_SUCCESS != filterMigrateConfigV4V5Convert (pCfgV4,pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FILTER_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /* passthru*/

    case FILTER_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      if (L7_NULLPTR == pCfgV5)
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_FILTER_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_FILTER_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_FILTER_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_FILTER_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_FILTER_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_FILTER_COMPONENT_ID, pCfgV5);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    filterBuildDefaultConfigData (FILTER_CFG_VER_CURRENT);
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
static L7_RC_t
filterMigrateConfigV1V2Convert (filterCfgDataV1_t * pCfgV1, filterCfgDataV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i, j;
  L7_uint32 filterCount;
  L7_uint32 ifIndexCount;
  L7_uint32 intfCfgIndex;
  nimConfigID_t configId;
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 configIdOffset;
  L7_uint32 configIdOffsetDelta;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != FILTER_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, FILTER_CFG_VER_1);
    return L7_FAILURE;
  }

  filterBuildDefaultConfigDataV2 (pCfgV2);

  /* Ugly: This is just to migrate "ifIndex" values from release 4.0 to 4.1 */
  configIdOffsetDelta = L7_MAX_INTERFACE_COUNT_REL_4_1 - L7_MAX_INTERFACE_COUNT_REL_4_0;
  if (configIdOffsetDelta < 0) 
  {
      LOG_MSG("May be unable to migrate src and/or dest interfaces for non-physical ports\n");
  }

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  filterCount = min (L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_0,
                     L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_1);
  for (i = 0; i < filterCount; i++)
  {
    if (pCfgV1->filterData[i].inUse == L7_FALSE)
      continue;
    MIGRATE_COPY_STRUCT (pCfgV2->filterData[i].macAddr, pCfgV1->filterData[i].macAddr);
    pCfgV2->filterData[i].vlanId = pCfgV1->filterData[i].vlanId;
    pCfgV2->filterData[i].numSrc = pCfgV1->filterData[i].numSrc;
    pCfgV2->filterData[i].numDst = pCfgV1->filterData[i].numDst;
    pCfgV2->filterData[i].inUse = pCfgV1->filterData[i].inUse;

    ifIndexCount = min (L7_FILTER_MAX_INTF_REL_4_0, L7_FILTER_MAX_INTF_REL_4_1);
    for (j = 0; j < ifIndexCount; j++)
    {

      /* In release 4.0, the extIfNum is stored in srcIfIndex and dstIfIndex.
         In release 4.1, the intIfNum is stored in srcIfIndex and dstIfIndex */
      sysapiCfgFileMigrateExtIfNumToUnitSlotPortRel4_0Get (pCfgV1->filterData[i].srcifIndex[j], 
                                                           &unit, &slot, &port);

      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId, 
                                                     &configIdOffset, L7_NULL);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (slot, port);
      }
      else
      {
          /* Post release G, the srcIfIndex field contains the
             intIfNum, which also happens to be the configIdOffset.
             On some platforms with stacking introduced, the values of
             non-physical ports for intIfNum are greater. This is the
             case with LAG interfaces. */
          pCfgV2->filterData[i].srcifIndex[j] = configIdOffset;
          if (configIdOffset > L7_MAX_PORT_COUNT_REL_4_0 ) 
          {
              if (configIdOffsetDelta > 0) 
              { 
                  pCfgV2->filterData[i].srcifIndex[j] += configIdOffsetDelta;
              }
          }
      }


      /* In release 4.0, the extIfNum is stored in srcIfIndex and dstIfIndex.
         In release 4.1, the intIfNum is stored in srcIfIndex and dstIfIndex */
      sysapiCfgFileMigrateExtIfNumToUnitSlotPortRel4_0Get (pCfgV1->filterData[i].dstifIndex[j], 
                                                           &unit, &slot, &port);

      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId, 
                                                     &configIdOffset, L7_NULL);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (slot, port);
      }
      else
      {
          /* Post release G, the srcIfIndex field contains the
             intIfNum, which also happens to be the configIdOffset.
             On some platforms with stacking introduced, the values of
             non-physical ports for intIfNum are greater. This is the
             case with LAG interfaces. */
          pCfgV2->filterData[i].dstifIndex[j] = configIdOffset;
          if (configIdOffset > L7_MAX_PORT_COUNT_REL_4_0 ) 
          {
              if (configIdOffsetDelta > 0) 
              { 
                  pCfgV2->filterData[i].dstifIndex[j] += configIdOffsetDelta;
              }
          }
      }
    }
  }

  intfCfgIndex = 1;
  for (i = 0; i < L7_MAX_INTERFACE_COUNT_REL_4_0; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (filterIsValidIntfType (configId.type) != L7_TRUE)
      continue;


    if (intfCfgIndex >= L7_FILTER_INTF_MAX_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (FILTER_CFG_FILENAME, intfCfgIndex);
      break;
    }


    (void) nimConfigIdCopy (&configId, &pCfgV2->filterIntfCfgData[intfCfgIndex].configId);
    intfCfgIndex++;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
* @param    pCfgV3        @b{(input)} ptr to version 3 config data structure
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
filterMigrateConfigV2V3Convert (filterCfgDataV2_t * pCfgV2, filterCfgDataV3_t * pCfgV3)
{
  L7_uint32 i, j;
  L7_uint32 filterCount, ifIndexCount;
  L7_uint32 configIdOffsetDelta;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != FILTER_CFG_VER_2 && pCfgV2->cfgHdr.version != FILTER_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, FILTER_CFG_VER_2);
    return L7_FAILURE;
  }

  filterBuildDefaultConfigDataV3 (pCfgV3);
  
  /* Ugly: This is just to migrate between non-stacking releases for releases H and H2 to 
     release I */
  configIdOffsetDelta = L7_MAX_INTERFACE_COUNT_REL_4_2 - L7_MAX_INTERFACE_COUNT_REL_4_3;
  if (configIdOffsetDelta < 0) 
  {
      LOG_MSG("May be unable to migrate src and/or dest interfaces for non-physical ports\n");
  }

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  filterCount = min (L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_3, L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_1);
  ifIndexCount = min (L7_FILTER_MAX_INTF_REL_4_3, L7_FILTER_MAX_INTF_REL_4_1);
  for (i = 0; i < filterCount; i++)
  {
    if (pCfgV2->filterData[i].inUse == L7_FALSE)
      continue;
    MIGRATE_COPY_STRUCT (pCfgV3->filterData[i].macAddr, pCfgV2->filterData[i].macAddr);
    pCfgV3->filterData[i].vlanId = pCfgV2->filterData[i].vlanId;
    pCfgV3->filterData[i].numSrc = pCfgV2->filterData[i].numSrc;
    pCfgV3->filterData[i].numDst = pCfgV2->filterData[i].numDst;
    pCfgV3->filterData[i].inUse = pCfgV2->filterData[i].inUse;

    for (j = 0; j < ifIndexCount; j++)
    {
      pCfgV3->filterData[i].srcifIndex[j] = pCfgV2->filterData[i].srcifIndex[j];
      pCfgV3->filterData[i].dstifIndex[j] = pCfgV2->filterData[i].dstifIndex[j];

      if (configIdOffsetDelta > 0) 
      {
          /*  Adjust the  intIfNums downward for non-physical ports since between 
              releases 4.2 and 4.3 they are smaller  */  
          if (pCfgV3->filterData[i].srcifIndex[j] > L7_MAX_PORT_COUNT_REL_4_2) 
          {
              pCfgV3->filterData[i].srcifIndex[j]  -= configIdOffsetDelta;
          }

          if (pCfgV3->filterData[i].dstifIndex[j] > L7_MAX_PORT_COUNT_REL_4_2) 
          {
              pCfgV3->filterData[i].dstifIndex[j]  -= configIdOffsetDelta;
          }
      }
    }
  }


  ifIndexCount = min (L7_FILTER_INTF_MAX_COUNT_REL_4_1, L7_FILTER_INTF_MAX_COUNT_REL_4_3);
  for (i = 1; i < ifIndexCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->filterIntfCfgData[i], pCfgV2->filterIntfCfgData[i]);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    pCfgV3    @b{(input)} ptr to version 2 config data structure
* @param    pCfgV4        @b{(input)} ptr to version 3 config data structure
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
filterMigrateConfigV3V4Convert (filterCfgDataV3_t * pCfgV3, filterCfgDataV4_t * pCfgV4)
{
  L7_uint32 i, j;
  L7_uint32 filterCount, ifIndexCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != FILTER_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != FILTER_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != FILTER_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, FILTER_CFG_VER_3);
    return L7_FAILURE;
  }

  filterBuildDefaultConfigDataV4 (pCfgV4);

  filterCount = min (L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_4, L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_3);
  ifIndexCount = min (L7_FILTER_MAX_INTF_REL_4_4, L7_FILTER_MAX_INTF_REL_4_3);
  for (i = 0; i < filterCount; i++)
  {
    if (pCfgV3->filterData[i].inUse == L7_FALSE)
      continue;
    MIGRATE_COPY_STRUCT (pCfgV4->filterData[i].macAddr, pCfgV3->filterData[i].macAddr);
    pCfgV4->filterData[i].vlanId = pCfgV3->filterData[i].vlanId;
    pCfgV4->filterData[i].numSrc = pCfgV3->filterData[i].numSrc;
    pCfgV4->filterData[i].numDst = pCfgV3->filterData[i].numDst;
    pCfgV4->filterData[i].inUse = pCfgV3->filterData[i].inUse;

    for (j = 0; j < ifIndexCount; j++)
    {
      pCfgV4->filterData[i].srcifIndex[j] = pCfgV3->filterData[i].srcifIndex[j];
      pCfgV4->filterData[i].dstifIndex[j] = pCfgV3->filterData[i].dstifIndex[j];
    }
  }


  ifIndexCount = min (L7_FILTER_INTF_MAX_COUNT_REL_4_3, L7_FILTER_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < ifIndexCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->filterIntfCfgData[i], pCfgV3->filterIntfCfgData[i]);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV5        @b{(input)} ptr to version 5 config data structure
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
filterMigrateConfigV4V5Convert (filterCfgDataV4_t * pCfgV4, filterCfgDataV5_t * pCfgV5)
{
  L7_uint32 i, j;
  L7_uint32 filterCount, ifIndexCount;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != FILTER_CFG_VER_4 &&
      pCfgV4->cfgHdr.version != FILTER_CFG_VER_3 &&
      pCfgV4->cfgHdr.version != FILTER_CFG_VER_2 &&
      pCfgV4->cfgHdr.version != FILTER_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, FILTER_CFG_VER_4);
    return L7_FAILURE;
  }

  filterBuildDefaultConfigDataV5 (pCfgV5);

  filterCount = min (L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_4, L7_MAX_STATIC_FILTER_ENTRIES_REL_5_0);
  ifIndexCount = min (L7_FILTER_MAX_INTF_REL_4_4, L7_FILTER_MAX_INTF_REL_5_0);
  for (i = 0; i < filterCount; i++)
  {
    if (pCfgV4->filterData[i].inUse == L7_FALSE)
      continue;
    MIGRATE_COPY_STRUCT (pCfgV5->filterData[i].macAddr, pCfgV4->filterData[i].macAddr);
    pCfgV5->filterData[i].vlanId = pCfgV4->filterData[i].vlanId;
    pCfgV5->filterData[i].numSrc = pCfgV4->filterData[i].numSrc;
    pCfgV5->filterData[i].numDst = pCfgV4->filterData[i].numDst;
    pCfgV5->filterData[i].inUse = pCfgV4->filterData[i].inUse;

    for (j = 0; j < ifIndexCount; j++)
    {
      pCfgV5->filterData[i].srcifIndex[j] = pCfgV4->filterData[i].srcifIndex[j];
      pCfgV5->filterData[i].dstifIndex[j] = pCfgV4->filterData[i].dstifIndex[j];
    }
  }


  ifIndexCount = min (L7_FILTER_INTF_MAX_COUNT_REL_5_0, L7_FILTER_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < ifIndexCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->filterIntfCfgData[i], pCfgV4->filterIntfCfgData[i]);
  }
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
filterBuildDefaultConfigDataV2 (filterCfgDataV2_t * pCfgV2)
{
  L7_uint32 i;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  strcpy ((char *) pCfgV2->cfgHdr.filename, FILTER_CFG_FILENAME);
  pCfgV2->cfgHdr.version = FILTER_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_FILTER_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
  pCfgV2->cfgHdr.savePointId = 0;
  pCfgV2->cfgHdr.targetDevice = 0;

  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_1; i++)
  {
    memset ((void *) pCfgV2->filterData[i].macAddr, 0x00, L7_MAC_ADDR_LEN);
    pCfgV2->filterData[i].vlanId = L7_NULL;
    memset ((void *) pCfgV2->filterData[i].srcifIndex, 0x00,
            sizeof (pCfgV2->filterData[i].srcifIndex));
    pCfgV2->filterData[i].numSrc = L7_NULL;
    memset ((void *) pCfgV2->filterData[i].dstifIndex, 0x00,
            sizeof (pCfgV2->filterData[i].dstifIndex));
    pCfgV2->filterData[i].numDst = L7_NULL;
    pCfgV2->filterData[i].inUse = L7_FALSE;
  }

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
filterBuildDefaultConfigDataV3 (filterCfgDataV3_t * pCfgV3)
{
  L7_uint32 i;

  memset (pCfgV3, 0, sizeof (*pCfgV3));

  strcpy ((char *) pCfgV3->cfgHdr.filename, FILTER_CFG_FILENAME);
  pCfgV3->cfgHdr.version = FILTER_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_FILTER_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32) sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;
  pCfgV3->cfgHdr.savePointId = 0;
  pCfgV3->cfgHdr.targetDevice = 0;

  for (i = 0; i < L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_3; i++)
  {
    memset ((void *) pCfgV3->filterData[i].macAddr, 0x00, L7_MAC_ADDR_LEN);
    pCfgV3->filterData[i].vlanId = L7_NULL;
    memset ((void *) pCfgV3->filterData[i].srcifIndex, 0x00,
            sizeof (pCfgV3->filterData[i].srcifIndex));
    pCfgV3->filterData[i].numSrc = L7_NULL;
    memset ((void *) pCfgV3->filterData[i].dstifIndex, 0x00,
            sizeof (pCfgV3->filterData[i].dstifIndex));
    pCfgV3->filterData[i].numDst = L7_NULL;
    pCfgV3->filterData[i].inUse = L7_FALSE;
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
filterBuildDefaultConfigDataV4 (filterCfgDataV4_t * pCfgV4)
{
  memset (pCfgV4, 0, sizeof (*pCfgV4));

  strcpy ((char *) pCfgV4->cfgHdr.filename, FILTER_CFG_FILENAME);
  pCfgV4->cfgHdr.version = FILTER_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_FILTER_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = (L7_uint32) sizeof (*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;
  pCfgV4->cfgHdr.savePointId = 0;
  pCfgV4->cfgHdr.targetDevice = 0;

  /* Rest of structure is zeroed out right at the begining of this routine */
  return;
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
filterBuildDefaultConfigDataV5 (filterCfgDataV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (filterCfgDataV5_t));
  filterBuildDefaultConfigData (FILTER_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}




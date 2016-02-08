
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename rtrdisc_migrate.c
*
* @purpose rtrdisc Configuration Migration
*
* @component rtrdisc
*
* @comments none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "rtrdisc_migrate.h"
#include "rtrdisc_api.h"

static L7_RC_t rtrDiscMigrateConfigV1V2Convert (rtrDiscCfgDataV1_t * pCfgV1,
                                                rtrDiscCfgDataV2_t * pCfgV2);

static L7_RC_t rtrDiscMigrateConfigV2V3Convert (rtrDiscCfgDataV2_t * pCfgV2,
                                                rtrDiscCfgDataV3_t * pCfgV3);

static L7_RC_t rtrDiscMigrateConfigV3V4Convert (rtrDiscCfgDataV3_t * pCfgV3,
                                                rtrDiscCfgDataV4_t * pCfgV4);
static L7_RC_t rtrDiscMigrateConfigV4V5Convert (rtrDiscCfgDataV4_t * pCfgV4, 
                                                rtrDiscCfgDataV5_t * pCfgV5);
static void rtrDiscBuildDefaultConfigDataV2 (rtrDiscCfgDataV2_t * pCfgV2);
static void rtrDiscBuildDefaultConfigDataV3 (rtrDiscCfgDataV3_t * pCfgV3);
static void rtrDiscBuildDefaultConfigDataV4 (rtrDiscCfgDataV4_t * pCfgV4);
static void rtrDiscBuildDefaultConfigDataV5 (rtrDiscCfgDataV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static rtrDiscCfgData_t *pCfgCur = L7_NULLPTR;

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
rtrDiscMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  rtrDiscCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  rtrDiscCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  rtrDiscCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  rtrDiscCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  rtrDiscCfgDataV5_t *pCfgV5 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (rtrDiscCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  oldCfgSize = 0;
  pOldCfgCksum = 0;
  oldCfgVer = 0;
  ver = ver;

  switch (oldVer)
  {
  case L7_RTR_DISC_CFG_VER_1:
    pCfgV1 = (rtrDiscCfgDataV1_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                 (L7_uint32) sizeof (rtrDiscCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (rtrDiscCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = L7_RTR_DISC_CFG_VER_1;
    break;

  case L7_RTR_DISC_CFG_VER_2:
    pCfgV2 = (rtrDiscCfgDataV2_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                 (L7_uint32) sizeof (rtrDiscCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (rtrDiscCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_RTR_DISC_CFG_VER_2;
    break;

  case L7_RTR_DISC_CFG_VER_3:
    pCfgV3 = (rtrDiscCfgDataV3_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                 (L7_uint32) sizeof (rtrDiscCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (rtrDiscCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_RTR_DISC_CFG_VER_3;
    break;

  case L7_RTR_DISC_CFG_VER_4:
    pCfgV4 = (rtrDiscCfgDataV4_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                 (L7_uint32) sizeof (rtrDiscCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (rtrDiscCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_RTR_DISC_CFG_VER_4;
    break;

  case L7_RTR_DISC_CFG_VER_5:
    pCfgV5 = (rtrDiscCfgDataV5_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                 (L7_uint32) sizeof (rtrDiscCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (rtrDiscCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = L7_RTR_DISC_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_RTR_DISC_COMPONENT_ID,
                                         L7_RTR_DISC_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_RTR_DISC_COMPONENT_ID,
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
    case L7_RTR_DISC_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (rtrDiscCfgDataV2_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                   (L7_uint32) sizeof (rtrDiscCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != rtrDiscMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_RTR_DISC_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (rtrDiscCfgDataV3_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                   (L7_uint32) sizeof (rtrDiscCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != rtrDiscMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_RTR_DISC_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (rtrDiscCfgDataV4_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                   (L7_uint32) sizeof (rtrDiscCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != rtrDiscMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_RTR_DISC_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (rtrDiscCfgDataV5_t *) osapiMalloc (L7_RTR_DISC_COMPONENT_ID,
                                                   (L7_uint32) sizeof (rtrDiscCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV5) ||
          (L7_SUCCESS != rtrDiscMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case L7_RTR_DISC_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV5);
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
    osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_RTR_DISC_COMPONENT_ID, pCfgV5);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    rtrDiscBuildDefaultConfigData (L7_RTR_DISC_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}



/*********************************************************************
* @purpose  Converts the interface config data structure from version V2 to V3.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
* @param    intfType  @b{(input)} interface types being migrated
* @param    offset    @b{(input)} current offset to version 2 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*          
*
* @end
*********************************************************************/
static void
rtrDiscMigrateIntfConfigV1V2Convert (rtrDiscCfgDataV1_t * pCfgV1, rtrDiscCfgDataV2_t * pCfgV2,
                                     L7_uint32 intfType, L7_uint32 * offset)
{
  L7_RC_t rc;
  L7_int32 i, j, intCfgIndex;
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

      if (intCfgIndex >= L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE (L7_RTR_DISC_FILENAME, intCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV2->rtrDiscIntfCfgData[intCfgIndex].configId);
      pCfgV2->rtrDiscIntfCfgData[intCfgIndex].advAddress = pCfgV1->rtrDiscData[i][j].advAddress;
      pCfgV2->rtrDiscIntfCfgData[intCfgIndex].maxAdvInt = pCfgV1->rtrDiscData[i][j].maxAdvInt;
      pCfgV2->rtrDiscIntfCfgData[intCfgIndex].minAdvInt = pCfgV1->rtrDiscData[i][j].minAdvInt;
      pCfgV2->rtrDiscIntfCfgData[intCfgIndex].advLifetime = pCfgV1->rtrDiscData[i][j].advLifetime;
      pCfgV2->rtrDiscIntfCfgData[intCfgIndex].advertise = pCfgV1->rtrDiscData[i][j].advertise;
      pCfgV2->rtrDiscIntfCfgData[intCfgIndex].preferenceLevel =
        pCfgV1->rtrDiscData[i][j].preferenceLevel;
      intCfgIndex++;
    }

    if (truncated == L7_TRUE)
        break;
  }

  *offset = intCfgIndex;
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
rtrDiscMigrateConfigV1V2Convert (rtrDiscCfgDataV1_t * pCfgV1, rtrDiscCfgDataV2_t * pCfgV2)
{
  L7_uint32 intCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_RTR_DISC_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_RTR_DISC_CFG_VER_1);
    return L7_FAILURE;
  }

  rtrDiscBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intCfgIndex = 1;
  rtrDiscMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_PHYSICAL_INTF, &intCfgIndex);


  if (intCfgIndex < L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1)
      rtrDiscMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_LOGICAL_VLAN_INTF, &intCfgIndex);


  if (intCfgIndex < L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1)
      rtrDiscMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_LAG_INTF, &intCfgIndex);

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
rtrDiscMigrateConfigV2V3Convert (rtrDiscCfgDataV2_t * pCfgV2, rtrDiscCfgDataV3_t * pCfgV3)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_RTR_DISC_CFG_VER_2 &&
      pCfgV2->cfgHdr.version != L7_RTR_DISC_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_RTR_DISC_CFG_VER_2);
    return L7_FAILURE;
  }

  rtrDiscBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1, L7_RTR_DISC_INTF_MAX_COUNT_REL_4_3);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->rtrDiscIntfCfgData[i], pCfgV2->rtrDiscIntfCfgData[i]);
  }

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
rtrDiscMigrateConfigV3V4Convert (rtrDiscCfgDataV3_t * pCfgV3, rtrDiscCfgDataV4_t * pCfgV4)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_RTR_DISC_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != L7_RTR_DISC_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != L7_RTR_DISC_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_RTR_DISC_CFG_VER_3);
    return L7_FAILURE;
  }

  rtrDiscBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_RTR_DISC_INTF_MAX_COUNT_REL_4_3, L7_RTR_DISC_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->rtrDiscIntfCfgData[i], pCfgV3->rtrDiscIntfCfgData[i]);
  }

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
rtrDiscMigrateConfigV4V5Convert (rtrDiscCfgDataV4_t * pCfgV4, rtrDiscCfgDataV5_t * pCfgV5)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != L7_RTR_DISC_CFG_VER_4 &&
      pCfgV4->cfgHdr.version != L7_RTR_DISC_CFG_VER_3 &&
      pCfgV4->cfgHdr.version != L7_RTR_DISC_CFG_VER_2 &&
      pCfgV4->cfgHdr.version != L7_RTR_DISC_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_RTR_DISC_CFG_VER_4);
    return L7_FAILURE;
  }

  rtrDiscBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_RTR_DISC_INTF_MAX_COUNT_REL_4_4, L7_RTR_DISC_INTF_MAX_COUNT);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->rtrDiscIntfCfgData[i], pCfgV4->rtrDiscIntfCfgData[i]);
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
rtrDiscBuildDefaultConfigDataV2 (rtrDiscCfgDataV2_t * pCfgV2)
{
  L7_uint32 i;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  /* Router Discovery parameters */
  for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT_REL_4_1; i++)
  {
    rtrDiscIntfCfgData_t *pCfg = &pCfgV2->rtrDiscIntfCfgData[i];

    pCfg->advAddress = L7_DEFAULT_ADV_ADDR;
    pCfg->maxAdvInt = MAX_ADV_INTERVAL;
    pCfg->minAdvInt = MIN_ADV_INTERVAL;
    pCfg->advLifetime = ADV_LIFETIME;
    pCfg->advertise = ADVERTISEMENT;
    pCfg->preferenceLevel = PREFERENCE_LEVEL;
  }

  strcpy (pCfgV2->cfgHdr.filename, L7_RTR_DISC_FILENAME);
  pCfgV2->cfgHdr.version = L7_RTR_DISC_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_RTR_DISC_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
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
rtrDiscBuildDefaultConfigDataV3 (rtrDiscCfgDataV3_t * pCfgV3)
{
  L7_uint32 i;

  /* Build Config File Header */
  memset((void*)&pCfgV3->cfgHdr, 0, sizeof(L7_fileHdr_t));  
  strcpy(pCfgV3->cfgHdr.filename, L7_RTR_DISC_FILENAME);
  pCfgV3->cfgHdr.version = L7_RTR_DISC_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_RTR_DISC_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof(*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  /*-------------------------------------*/
  /* Build Default Router Discovery Data */
  /*-------------------------------------*/


  /* Router Discovery parameters */
  for (i = 1; i < L7_RTR_DISC_INTF_MAX_COUNT_REL_4_3; i++)
  {
    rtrDiscIntfCfgData_t *pCfg = &pCfgV3->rtrDiscIntfCfgData[i];

    pCfg->advAddress = L7_DEFAULT_ADV_ADDR;
    pCfg->maxAdvInt = MAX_ADV_INTERVAL;
    pCfg->minAdvInt = MIN_ADV_INTERVAL;
    pCfg->advLifetime = ADV_LIFETIME;
    pCfg->advertise = ADVERTISEMENT;
    pCfg->preferenceLevel = PREFERENCE_LEVEL;
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
rtrDiscBuildDefaultConfigDataV4 (rtrDiscCfgDataV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (rtrDiscCfgDataV4_t));
  rtrDiscBuildDefaultConfigData (L7_RTR_DISC_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
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
static void
rtrDiscBuildDefaultConfigDataV5 (rtrDiscCfgDataV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (rtrDiscCfgDataV5_t));
  rtrDiscBuildDefaultConfigData (L7_RTR_DISC_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

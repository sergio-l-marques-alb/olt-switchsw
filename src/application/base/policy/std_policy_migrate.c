
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename std_policy_migrate.c
*
* @purpose Configuration Migration
*
* @component policy
*
* @comments none
*
* @create 8/23/2004
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "std_policy_migrate.h"
#include "std_policy_api.h"

/* old default values - no longer used */
#define FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_HIGH  20
#define FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_LOW   10

static L7_RC_t policyMigrateConfigV1V2Convert (policyCfgDataV1_t * pCfgV1,
                                               policyCfgDataV2_t * pCfgV2);

static L7_RC_t policyMigrateConfigV2V3Convert (policyCfgDataV2_t * pCfgV2,
                                               policyCfgDataV3_t * pCfgV3);

static L7_RC_t policyMigrateConfigV3V4Convert (policyCfgDataV3_t * pCfgV3,
                                               policyCfgDataV4_t * pCfgV4);

static L7_RC_t policyMigrateConfigV4V5Convert (policyCfgDataV4_t * pCfgV4,
                                               policyCfgDataV5_t * pCfgV5);

static void policyBuildDefaultConfigDataV2 (policyCfgDataV2_t * pCfgV2);
static void policyBuildDefaultConfigDataV3 (policyCfgDataV3_t * pCfgV3);
static void policyBuildDefaultConfigDataV4 (policyCfgDataV4_t * pCfgV4);
static void policyBuildDefaultConfigDataV5 (policyCfgDataV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static policyCfgData_t *pCfgCur = L7_NULLPTR;

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
policyMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  policyCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  policyCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  policyCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  policyCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  policyCfgDataV5_t *pCfgV5 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (policyCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case POLICY_CFG_VER_1:
    pCfgV1 = (policyCfgDataV1_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                (L7_uint32) sizeof (policyCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (policyCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = POLICY_CFG_VER_1;
    break;

  case POLICY_CFG_VER_2:
    pCfgV2 = (policyCfgDataV2_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                (L7_uint32) sizeof (policyCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (policyCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = POLICY_CFG_VER_2;
    break;

  case POLICY_CFG_VER_3:
    pCfgV3 = (policyCfgDataV3_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                (L7_uint32) sizeof (policyCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (policyCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = POLICY_CFG_VER_3;
    break;

  case POLICY_CFG_VER_4:
    pCfgV4 = (policyCfgDataV4_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                (L7_uint32) sizeof (policyCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (policyCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = POLICY_CFG_VER_4;
    break;

  case POLICY_CFG_VER_5:
    pCfgV5 = (policyCfgDataV5_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                (L7_uint32) sizeof (policyCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (policyCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = POLICY_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_POLICY_COMPONENT_ID,
                                         POLICY_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_POLICY_COMPONENT_ID,
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
    case POLICY_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (policyCfgDataV2_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                  (L7_uint32) sizeof (policyCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != policyMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_POLICY_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case POLICY_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (policyCfgDataV3_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                  (L7_uint32) sizeof (policyCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != policyMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_POLICY_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case POLICY_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (policyCfgDataV4_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                  (L7_uint32) sizeof (policyCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != policyMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_POLICY_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case POLICY_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (policyCfgDataV5_t *) osapiMalloc (L7_POLICY_COMPONENT_ID,
                                                  (L7_uint32) sizeof (policyCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != policyMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_POLICY_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case POLICY_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_POLICY_COMPONENT_ID, pCfgV5);
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
    osapiFree (L7_POLICY_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_POLICY_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_POLICY_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_POLICY_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_POLICY_COMPONENT_ID, pCfgV5);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_POLICY_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    policyBuildDefaultConfigData (POLICY_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v1 to v2
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
policyMigrateConfigV1V2Convert (policyCfgDataV1_t * pCfgV1, policyCfgDataV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_int32 i, j;
  nimConfigID_t configId;
  L7_int32 intfCfgIndex;
  L7_BOOL truncated;

  truncated = L7_FALSE;

  /* verify correct version of old config file */
  if (POLICY_CFG_VER_1 != pCfgV1->cfgHdr.version)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, POLICY_CFG_VER_1);
    return L7_FAILURE;
  }

  policyBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->systemFlowControlMode = pCfgV1->systemFlowControlMode;
  pCfgV2->systemBcastStormMode = pCfgV1->systemBcastStormMode;
  pCfgV2->systemBcastStormModeHigh = pCfgV1->systemBcastStormModeHigh;
  pCfgV2->systemBcastStormModeLow = pCfgV1->systemBcastStormModeLow;

  intfCfgIndex = 1;
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
      /* Determine if the interface is valid for participation in this feature */
      if (policyIsValidIntfType (configId.type) != L7_TRUE)
        continue;

      if (intfCfgIndex > L7_POLICY_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE (POLICY_CFG_FILENAME, intfCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV2->policyIntfCfgData[intfCfgIndex].configId);
      pCfgV2->policyIntfCfgData[intfCfgIndex].flowControlMode = pCfgV1->flowControlMode[i][j];
      pCfgV2->policyIntfCfgData[intfCfgIndex].bcastStormMode = pCfgV1->bcastStormMode[i][j];
      pCfgV2->policyIntfCfgData[intfCfgIndex].bcastStormHighLimit =
        pCfgV1->bcastStormHighLimit[i][j];
      pCfgV2->policyIntfCfgData[intfCfgIndex].bcastStormLowLimit = pCfgV1->bcastStormLowLimit[i][j];
      intfCfgIndex++;
    }


    if (truncated == L7_TRUE)
        break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v2 to v3
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
policyMigrateConfigV2V3Convert (policyCfgDataV2_t * pCfgV2, policyCfgDataV3_t * pCfgV3)
{
  L7_int32 i;
  L7_int32 intfCount;

  /* verify correct version of old config file */
  if (POLICY_CFG_VER_2 != pCfgV2->cfgHdr.version && POLICY_CFG_VER_1 != pCfgV2->cfgHdr.version)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, POLICY_CFG_VER_2);
    return L7_FAILURE;
  }

  policyBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->systemFlowControlMode = pCfgV2->systemFlowControlMode;
  pCfgV3->systemBcastStormMode = pCfgV2->systemBcastStormMode;
  pCfgV3->systemBcastStormModeHigh = pCfgV2->systemBcastStormModeHigh;
  pCfgV3->systemBcastStormModeLow = pCfgV2->systemBcastStormModeLow;

  intfCount = min (L7_POLICY_INTF_MAX_COUNT_REL_4_1, L7_POLICY_INTF_MAX_COUNT_REL_4_3);
  for (i = 0; i < intfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->policyIntfCfgData[i], pCfgV2->policyIntfCfgData[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v3 to v4
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
policyMigrateConfigV3V4Convert (policyCfgDataV3_t * pCfgV3, policyCfgDataV4_t * pCfgV4)
{
  L7_int32 i;
  L7_int32 intfCount;

  /* verify correct version of old config file */
  if (POLICY_CFG_VER_3 != pCfgV3->cfgHdr.version &&
      POLICY_CFG_VER_2 != pCfgV3->cfgHdr.version &&
      POLICY_CFG_VER_1 != pCfgV3->cfgHdr.version)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, POLICY_CFG_VER_3);
    return L7_FAILURE;
  }

  policyBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->systemFlowControlMode = pCfgV3->systemFlowControlMode;
  pCfgV4->systemBcastStormMode = pCfgV3->systemBcastStormMode;

  intfCount = min (L7_POLICY_INTF_MAX_COUNT_REL_4_3, L7_POLICY_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < intfCount; i++)
  {
    pCfgV4->policyIntfCfgData[i].configId = pCfgV3->policyIntfCfgData[i].configId;
    pCfgV4->policyIntfCfgData[i].flowControlMode = pCfgV3->policyIntfCfgData[i].flowControlMode;
    if(pCfgV4->systemBcastStormMode)
    {
      pCfgV4->policyIntfCfgData[i].bcastStormMode = pCfgV4->systemBcastStormMode;
    }
    else
    {
      pCfgV4->policyIntfCfgData[i].bcastStormMode = pCfgV3->policyIntfCfgData[i].bcastStormMode;
    }
    /*
     * leave rest at defaults and there is no mapping for
     * bcastStormLowLimit and bcastStormHighLimit as these
     * are no longer used.
     */
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v4 to v5
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
policyMigrateConfigV4V5Convert (policyCfgDataV4_t * pCfgV4, policyCfgDataV5_t * pCfgV5)
{
  L7_int32 i;
  L7_int32 intfCount;

  /* verify correct version of old config file */
  if (POLICY_CFG_VER_4 != pCfgV4->cfgHdr.version &&
      POLICY_CFG_VER_3 != pCfgV4->cfgHdr.version &&
      POLICY_CFG_VER_2 != pCfgV4->cfgHdr.version &&
      POLICY_CFG_VER_1 != pCfgV4->cfgHdr.version)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, POLICY_CFG_VER_4);
    return L7_FAILURE;
  }

  policyBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->systemFlowControlMode = pCfgV4->systemFlowControlMode;
  pCfgV5->systemBcastStormMode = pCfgV4->systemBcastStormMode;
  pCfgV5->systemBcastStormThreshold = pCfgV4->systemBcastStormThreshold;
  pCfgV5->systemMcastStormMode = pCfgV4->systemMcastStormMode;
  pCfgV5->systemMcastStormThreshold = pCfgV4->systemMcastStormThreshold;
  pCfgV5->systemUcastStormMode = pCfgV4->systemUcastStormMode;
  pCfgV5->systemUcastStormThreshold = pCfgV4->systemUcastStormThreshold;

  intfCount = L7_POLICY_INTF_MAX_COUNT_REL_4_4;
  for (i = 0; i < intfCount; i++)
  {
    pCfgV5->policyIntfCfgData[i].configId = pCfgV4->policyIntfCfgData[i].configId;
    pCfgV5->policyIntfCfgData[i].flowControlMode = pCfgV4->policyIntfCfgData[i].flowControlMode;
    pCfgV5->policyIntfCfgData[i].bcastStormMode = pCfgV4->policyIntfCfgData[i].bcastStormMode;
    pCfgV5->policyIntfCfgData[i].bcastStormThreshold = pCfgV4->policyIntfCfgData[i].bcastStormThreshold;
    pCfgV5->policyIntfCfgData[i].mcastStormMode = pCfgV4->policyIntfCfgData[i].mcastStormMode;
    pCfgV5->policyIntfCfgData[i].mcastStormThreshold = pCfgV4->policyIntfCfgData[i].mcastStormThreshold;
    pCfgV5->policyIntfCfgData[i].ucastStormMode = pCfgV4->policyIntfCfgData[i].ucastStormMode;
    pCfgV5->policyIntfCfgData[i].ucastStormThreshold = pCfgV4->policyIntfCfgData[i].ucastStormThreshold;
  }

  /*
   * leave rest at defaults
   */

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
policyBuildDefaultConfigDataV2 (policyCfgDataV2_t * pCfgV2)
{
  L7_uint32 i;

  memset ((char *) pCfgV2, 0, sizeof (*pCfgV2));

  /* Initialize all interface-related entries in the config file to default values. */
  for (i = 1; i < L7_POLICY_INTF_MAX_COUNT_REL_4_1; i++)
  {
    pCfgV2->policyIntfCfgData[i].bcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
    pCfgV2->policyIntfCfgData[i].flowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;
    pCfgV2->policyIntfCfgData[i].bcastStormHighLimit = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_HIGH;
    pCfgV2->policyIntfCfgData[i].bcastStormLowLimit = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_LOW;
  }

  pCfgV2->systemBcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
  pCfgV2->systemFlowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;
  pCfgV2->systemBcastStormModeHigh = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_HIGH;
  pCfgV2->systemBcastStormModeLow = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_LOW;

  /* Build header */
  strcpy (pCfgV2->cfgHdr.filename, POLICY_CFG_FILENAME);
  pCfgV2->cfgHdr.version = POLICY_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_POLICY_COMPONENT_ID;
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
policyBuildDefaultConfigDataV3 (policyCfgDataV3_t * pCfgV3)
{
  L7_uint32 i;

  memset ((char *) pCfgV3, 0, sizeof (*pCfgV3));

  /* Initialize all interface-related entries in the config file to default values. */
  for (i = 1; i < L7_POLICY_INTF_MAX_COUNT_REL_4_3; i++)
  {
    pCfgV3->policyIntfCfgData[i].bcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
    pCfgV3->policyIntfCfgData[i].flowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;
    pCfgV3->policyIntfCfgData[i].bcastStormHighLimit = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_HIGH;
    pCfgV3->policyIntfCfgData[i].bcastStormLowLimit = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_LOW;
  }

  pCfgV3->systemBcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
  pCfgV3->systemFlowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;
  pCfgV3->systemBcastStormModeHigh = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_HIGH;
  pCfgV3->systemBcastStormModeLow = FD_POLICY_DEFAULT_BCAST_STORM_MODE_RATE_LOW;

  /* Build header */
  strcpy (pCfgV3->cfgHdr.filename, POLICY_CFG_FILENAME);
  pCfgV3->cfgHdr.version = POLICY_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_POLICY_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

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
policyBuildDefaultConfigDataV4 (policyCfgDataV4_t * pCfgV4)
{
  L7_uint32 i;

  memset ((char *) pCfgV4, 0, sizeof (*pCfgV4));

  /* Initialize all interface-related entries in the config file to default values. */
  for (i = 1; i < L7_POLICY_INTF_MAX_COUNT_REL_4_4; i++)
  {
    pCfgV4->policyIntfCfgData[i].bcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
    pCfgV4->policyIntfCfgData[i].bcastStormThreshold = FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD;
    pCfgV4->policyIntfCfgData[i].mcastStormMode = FD_POLICY_DEFAULT_MCAST_STORM_MODE;
    pCfgV4->policyIntfCfgData[i].mcastStormThreshold = FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD;
    pCfgV4->policyIntfCfgData[i].ucastStormMode = FD_POLICY_DEFAULT_UCAST_STORM_MODE;
    pCfgV4->policyIntfCfgData[i].ucastStormThreshold = FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD;
    pCfgV4->policyIntfCfgData[i].flowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;
  }

  pCfgV4->systemBcastStormMode = FD_POLICY_DEFAULT_BCAST_STORM_MODE;
  pCfgV4->systemBcastStormThreshold = FD_POLICY_DEFAULT_BCAST_STORM_THRESHOLD;
  pCfgV4->systemMcastStormMode = FD_POLICY_DEFAULT_MCAST_STORM_MODE;
  pCfgV4->systemMcastStormThreshold = FD_POLICY_DEFAULT_MCAST_STORM_THRESHOLD;
  pCfgV4->systemUcastStormMode = FD_POLICY_DEFAULT_UCAST_STORM_MODE;
  pCfgV4->systemUcastStormThreshold = FD_POLICY_DEFAULT_UCAST_STORM_THRESHOLD;
  pCfgV4->systemFlowControlMode = FD_POLICY_DEFAULT_FLOW_CONTROL_MODE;

  /* Build header */
  strcpy (pCfgV4->cfgHdr.filename, POLICY_CFG_FILENAME);
  pCfgV4->cfgHdr.version = POLICY_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_POLICY_COMPONENT_ID;
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
static void
policyBuildDefaultConfigDataV5 (policyCfgDataV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (policyCfgDataV5_t));
  policyBuildDefaultConfigData (POLICY_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}


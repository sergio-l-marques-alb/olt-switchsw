
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename dot1x_migrate.c
*
* @purpose dot1x Configuration Migration
*
* @component dot1x
*
* @comments none
*
* @create 8/23/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#include "dot1x_migrate.h"

static L7_RC_t dot1xMigrateConfigV1V2Convert (dot1xCfgV1_t * pCfgV1, dot1xCfgV2_t * pCfgV2);
static L7_RC_t dot1xMigrateConfigV2V3Convert (dot1xCfgV2_t * pCfgV2, dot1xCfgV3_t * pCfgV3);
static L7_RC_t dot1xMigrateConfigV3V4Convert (dot1xCfgV3_t * pCfgV3, dot1xCfgV4_t * pCfgV4);
static L7_RC_t dot1xMigrateConfigV4V5Convert (dot1xCfgV4_t * pCfgV4, dot1xCfgV5_t * pCfgV5);
static L7_RC_t dot1xMigrateConfigV5V6Convert (dot1xCfgV5_t * pCfgV5, dot1xCfgV6_t * pCfgV6);
static L7_RC_t dot1xMigrateConfigV6V7Convert (dot1xCfgV6_t * pCfgV6, dot1xCfgV7_t * pCfgV7);
static L7_RC_t dot1xMigrateConfigV7V8Convert (dot1xCfgV7_t * pCfgV7, dot1xCfgV8_t * pCfgV8);
static void dot1xBuildDefaultConfigDataV2 (dot1xCfgV2_t * pCfgV2);
static void dot1xBuildDefaultConfigDataV3 (dot1xCfgV3_t * pCfgV3);
static void dot1xBuildDefaultConfigDataV4 (dot1xCfgV4_t * pCfgV4);
static void dot1xBuildDefaultConfigDataV5 (dot1xCfgV5_t * pCfgV5);
static void dot1xBuildDefaultConfigDataV6 (dot1xCfgV6_t * pCfgV6);
static void dot1xBuildDefaultConfigDataV7 (dot1xCfgV7_t * pCfgV7);
static void dot1xBuildDefaultConfigDataV8 (dot1xCfgV8_t * pCfgV8);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static dot1xCfg_t *pCfgCur = L7_NULLPTR;

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
dot1xMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  dot1xCfgV1_t *pCfgV1 = L7_NULLPTR;
  dot1xCfgV2_t *pCfgV2 = L7_NULLPTR;
  dot1xCfgV3_t *pCfgV3 = L7_NULLPTR;
  dot1xCfgV4_t *pCfgV4 = L7_NULLPTR;
  dot1xCfgV5_t *pCfgV5 = L7_NULLPTR;
  dot1xCfgV6_t *pCfgV6 = L7_NULLPTR;
  dot1xCfgV7_t *pCfgV7 = L7_NULLPTR;
  dot1xCfgV8_t *pCfgV8 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (dot1xCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case DOT1X_CFG_VER_1:
    pCfgV1 = (dot1xCfgV1_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1xCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = DOT1X_CFG_VER_1;
    break;

  case DOT1X_CFG_VER_2:
    pCfgV2 = (dot1xCfgV2_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1xCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = DOT1X_CFG_VER_2;
    break;

  case DOT1X_CFG_VER_3:
    pCfgV3 = (dot1xCfgV3_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1xCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = DOT1X_CFG_VER_3;
    break;

  case DOT1X_CFG_VER_4:
    pCfgV4 = (dot1xCfgV4_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1xCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = DOT1X_CFG_VER_4;
    break;

  case DOT1X_CFG_VER_5:
    pCfgV5 = (dot1xCfgV5_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                           (L7_uint32) sizeof (dot1xCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = DOT1X_CFG_VER_5;
    break;

  case DOT1X_CFG_VER_6:
    pCfgV6 = (dot1xCfgV6_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
        (L7_uint32) sizeof (dot1xCfgV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = DOT1X_CFG_VER_6;
    break;

  case DOT1X_CFG_VER_7:
    pCfgV7 = (dot1xCfgV7_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
        (L7_uint32) sizeof (dot1xCfgV7_t));
    if (L7_NULLPTR == pCfgV7)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV7;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV7_t);
    pOldCfgCksum = &pCfgV7->checkSum;
    oldCfgVer = DOT1X_CFG_VER_7;
    break;

  case DOT1X_CFG_VER_8:
    pCfgV8 = (dot1xCfgV8_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
        (L7_uint32) sizeof (dot1xCfgV8_t));
    if (L7_NULLPTR == pCfgV8)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV8;
    oldCfgSize = (L7_uint32) sizeof (dot1xCfgV8_t);
    pOldCfgCksum = &pCfgV8->checkSum;
    oldCfgVer = DOT1X_CFG_VER_8;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_DOT1X_COMPONENT_ID,
                                         DOT1X_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
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
    case DOT1X_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (dot1xCfgV2_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1xCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) || (L7_SUCCESS != dot1xMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case DOT1X_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (dot1xCfgV3_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1xCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || (L7_SUCCESS != dot1xMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case DOT1X_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (dot1xCfgV4_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1xCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) || (L7_SUCCESS != dot1xMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case DOT1X_CFG_VER_4:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV5 = (dot1xCfgV5_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                             (L7_uint32) sizeof (dot1xCfgV5_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV5) ||  (L7_SUCCESS != dot1xMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case DOT1X_CFG_VER_5:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV6 = (dot1xCfgV6_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                              (L7_uint32) sizeof (dot1xCfgV6_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV6) || (L7_SUCCESS != dot1xMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case DOT1X_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      pCfgV7 = (dot1xCfgV7_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                              (L7_uint32) sizeof (dot1xCfgV7_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_NULLPTR == pCfgV7) || (L7_SUCCESS != dot1xMigrateConfigV6V7Convert (pCfgV6, pCfgV7)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;
      /*passthru */

    case DOT1X_CFG_VER_7:
      /* pCfgV7 has already been allocated and filled in */
      pCfgV8 = (dot1xCfgV8_t *) osapiMalloc (L7_DOT1X_COMPONENT_ID,
                                              (L7_uint32) sizeof (dot1xCfgV8_t));
      if ((L7_NULLPTR == pCfgV7) || (L7_NULLPTR == pCfgV8) || (L7_SUCCESS != dot1xMigrateConfigV7V8Convert (pCfgV7, pCfgV8)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV7);
      pCfgV7 = L7_NULLPTR;
      /*passthru */


    case DOT1X_CFG_VER_8:
      /* pCfgV8 has already been allocated and filled in */
      if(pCfgV8 != L7_NULLPTR)
      {
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV8, sizeof (*pCfgCur));
        osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV8);
      }
      pCfgV8 = L7_NULLPTR;

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
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV5);
  if(pCfgV6 != L7_NULLPTR)
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV6);
  if(pCfgV7 != L7_NULLPTR)
    osapiFree (L7_DOT1X_COMPONENT_ID, pCfgV7);


  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dot1xBuildDefaultConfigData (DOT1X_CFG_VER_CURRENT);
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
dot1xMigrateConfigV1V2Convert (dot1xCfgV1_t * pCfgV1, dot1xCfgV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_uint32 intfCfgIndex;
  nimConfigID_t configId;
  L7_INTF_TYPES_t intfType;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, DOT1X_CFG_VER_1);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->dot1xAdminMode = pCfgV1->dot1xAdminMode;
  pCfgV2->dot1xLogTraceMode = pCfgV1->dot1xLogTraceMode;

  intfCfgIndex = 1;
  intfType = 0;
  for (i = 0; i < L7_MAX_INTERFACE_COUNT_REL_4_0; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i+1, &configId, L7_NULL, &intfType);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (dot1xIsValidIntfType (intfType) != L7_TRUE)
      continue;

    if (intfCfgIndex >= L7_DOT1X_INTF_MAX_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (DOT1X_CFG_FILENAME, intfCfgIndex);
      break;
    }

    pCfgV2->dot1xPortCfg[intfCfgIndex].adminControlledDirections =
      pCfgV1->dot1xPortCfg[i].adminControlledDirections;
    pCfgV2->dot1xPortCfg[intfCfgIndex].portControlMode = pCfgV1->dot1xPortCfg[i].portControlMode;
    pCfgV2->dot1xPortCfg[intfCfgIndex].quietPeriod = pCfgV1->dot1xPortCfg[i].quietPeriod;
    pCfgV2->dot1xPortCfg[intfCfgIndex].txPeriod = pCfgV1->dot1xPortCfg[i].txPeriod;
    pCfgV2->dot1xPortCfg[intfCfgIndex].suppTimeout = pCfgV1->dot1xPortCfg[i].suppTimeout;
    pCfgV2->dot1xPortCfg[intfCfgIndex].serverTimeout = pCfgV1->dot1xPortCfg[i].serverTimeout;
    pCfgV2->dot1xPortCfg[intfCfgIndex].maxReq = pCfgV1->dot1xPortCfg[i].maxReq;
    pCfgV2->dot1xPortCfg[intfCfgIndex].reAuthPeriod = pCfgV1->dot1xPortCfg[i].reAuthPeriod;
    pCfgV2->dot1xPortCfg[intfCfgIndex].reAuthEnabled = pCfgV1->dot1xPortCfg[i].reAuthEnabled;
    pCfgV2->dot1xPortCfg[intfCfgIndex].keyTxEnabled = pCfgV1->dot1xPortCfg[i].keyTxEnabled;

    (void) nimConfigIdCopy (&configId, &pCfgV2->dot1xPortCfg[intfCfgIndex].configId);
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
dot1xMigrateConfigV2V3Convert (dot1xCfgV2_t * pCfgV2, dot1xCfgV3_t * pCfgV3)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != DOT1X_CFG_VER_2 && pCfgV2->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, DOT1X_CFG_VER_2);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->dot1xAdminMode = pCfgV2->dot1xAdminMode;
  pCfgV3->dot1xLogTraceMode = pCfgV2->dot1xLogTraceMode;
  intIfCount = min (L7_DOT1X_INTF_MAX_COUNT_REL_4_1, L7_DOT1X_INTF_MAX_COUNT_REL_4_3);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->dot1xPortCfg[i], pCfgV2->dot1xPortCfg[i]);
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
dot1xMigrateConfigV3V4Convert (dot1xCfgV3_t * pCfgV3, dot1xCfgV4_t * pCfgV4)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != DOT1X_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != DOT1X_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, DOT1X_CFG_VER_3);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->dot1xAdminMode = pCfgV3->dot1xAdminMode;
  pCfgV4->dot1xLogTraceMode = pCfgV3->dot1xLogTraceMode;
  intIfCount = min (L7_DOT1X_INTF_MAX_COUNT_REL_4_3, L7_DOT1X_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->dot1xPortCfg[i], pCfgV3->dot1xPortCfg[i]);
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
dot1xMigrateConfigV4V5Convert (dot1xCfgV4_t * pCfgV4, dot1xCfgV5_t * pCfgV5)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != DOT1X_CFG_VER_4 &&
      pCfgV4->cfgHdr.version != DOT1X_CFG_VER_3 &&
      pCfgV4->cfgHdr.version != DOT1X_CFG_VER_2 &&
      pCfgV4->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, DOT1X_CFG_VER_4);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->dot1xAdminMode = pCfgV4->dot1xAdminMode;
  pCfgV5->dot1xLogTraceMode = pCfgV4->dot1xLogTraceMode;
  intIfCount = min (L7_DOT1X_INTF_MAX_COUNT_REL_4_4,L7_DOT1X_INTF_MAX_COUNT);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->dot1xPortCfg[i], pCfgV4->dot1xPortCfg[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V5 to V6.
*
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
* @param    pCfgV6    @b{(input)} ptr to version 6 config data structure
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
dot1xMigrateConfigV5V6Convert (dot1xCfgV5_t * pCfgV5, dot1xCfgV6_t * pCfgV6)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != DOT1X_CFG_VER_5 &&
      pCfgV5->cfgHdr.version != DOT1X_CFG_VER_4 &&
      pCfgV5->cfgHdr.version != DOT1X_CFG_VER_3 &&
      pCfgV5->cfgHdr.version != DOT1X_CFG_VER_2 &&
      pCfgV5->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, DOT1X_CFG_VER_5);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV6->dot1xAdminMode = pCfgV5->dot1xAdminMode;
  pCfgV6->dot1xLogTraceMode = pCfgV5->dot1xLogTraceMode;
  pCfgV6->vlanAssignmentMode = pCfgV5->vlanAssignmentMode;
  
  intIfCount = min (L7_DOT1X_INTF_MAX_COUNT_REL_4_4,L7_DOT1X_INTF_MAX_COUNT);
  for (i = 0; i < intIfCount; i++)
  {
    pCfgV6->dot1xPortCfg[i].adminControlledDirections = pCfgV5->dot1xPortCfg[i].adminControlledDirections;
    pCfgV6->dot1xPortCfg[i].portControlMode = pCfgV5->dot1xPortCfg[i].portControlMode;
    pCfgV6->dot1xPortCfg[i].quietPeriod = pCfgV5->dot1xPortCfg[i].quietPeriod;
    pCfgV6->dot1xPortCfg[i].txPeriod = pCfgV5->dot1xPortCfg[i].txPeriod;
    pCfgV6->dot1xPortCfg[i].suppTimeout = pCfgV5->dot1xPortCfg[i].suppTimeout;
    pCfgV6->dot1xPortCfg[i].serverTimeout = pCfgV5->dot1xPortCfg[i].serverTimeout;
    pCfgV6->dot1xPortCfg[i].maxReq = pCfgV5->dot1xPortCfg[i].maxReq;
    pCfgV6->dot1xPortCfg[i].reAuthPeriod = pCfgV5->dot1xPortCfg[i].reAuthPeriod;
    pCfgV6->dot1xPortCfg[i].reAuthEnabled = pCfgV5->dot1xPortCfg[i].reAuthEnabled;
    pCfgV6->dot1xPortCfg[i].keyTxEnabled = pCfgV5->dot1xPortCfg[i].keyTxEnabled;

    (void) nimConfigIdCopy (&pCfgV5->dot1xPortCfg[i].configId, &pCfgV6->dot1xPortCfg[i].configId);

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
dot1xMigrateConfigV6V7Convert (dot1xCfgV6_t * pCfgV6, dot1xCfgV7_t * pCfgV7)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV6->cfgHdr.version != DOT1X_CFG_VER_6 &&
      pCfgV6->cfgHdr.version != DOT1X_CFG_VER_5 &&
      pCfgV6->cfgHdr.version != DOT1X_CFG_VER_4 &&
      pCfgV6->cfgHdr.version != DOT1X_CFG_VER_3 &&
      pCfgV6->cfgHdr.version != DOT1X_CFG_VER_2 &&
      pCfgV6->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV6->cfgHdr.version, DOT1X_CFG_VER_6);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV7 (pCfgV7);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV7->dot1xAdminMode = pCfgV6->dot1xAdminMode;
  pCfgV7->dot1xLogTraceMode = pCfgV6->dot1xLogTraceMode;
  pCfgV7->vlanAssignmentMode = pCfgV6->vlanAssignmentMode;
  
  intIfCount = min (L7_DOT1X_INTF_MAX_COUNT_REL_4_4,L7_DOT1X_INTF_MAX_COUNT);
  for (i = 0; i < intIfCount; i++)
  {
    pCfgV7->dot1xPortCfg[i].adminControlledDirections = pCfgV6->dot1xPortCfg[i].adminControlledDirections;
    pCfgV7->dot1xPortCfg[i].portControlMode = pCfgV6->dot1xPortCfg[i].portControlMode;
    pCfgV7->dot1xPortCfg[i].quietPeriod = pCfgV6->dot1xPortCfg[i].quietPeriod;
    pCfgV7->dot1xPortCfg[i].txPeriod = pCfgV6->dot1xPortCfg[i].txPeriod;
    pCfgV7->dot1xPortCfg[i].suppTimeout = pCfgV6->dot1xPortCfg[i].suppTimeout;
    pCfgV7->dot1xPortCfg[i].serverTimeout = pCfgV6->dot1xPortCfg[i].serverTimeout;
    pCfgV7->dot1xPortCfg[i].maxReq = pCfgV6->dot1xPortCfg[i].maxReq;
    pCfgV7->dot1xPortCfg[i].reAuthPeriod = pCfgV6->dot1xPortCfg[i].reAuthPeriod;
    pCfgV7->dot1xPortCfg[i].reAuthEnabled = pCfgV6->dot1xPortCfg[i].reAuthEnabled;
    pCfgV7->dot1xPortCfg[i].keyTxEnabled = pCfgV6->dot1xPortCfg[i].keyTxEnabled;
    pCfgV7->dot1xPortCfg[i].guestVlanId = pCfgV6->dot1xPortCfg[i].guestVlanId;
    pCfgV7->dot1xPortCfg[i].guestVlanPeriod = pCfgV6->dot1xPortCfg[i].guestVlanPeriod;

    (void) nimConfigIdCopy (&pCfgV6->dot1xPortCfg[i].configId, &pCfgV7->dot1xPortCfg[i].configId);

  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Converts the config data structure from version V7 to V8.
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
* @param    pCfgV8    @b{(input)} ptr to version 8 config data structure
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
dot1xMigrateConfigV7V8Convert (dot1xCfgV7_t * pCfgV7, dot1xCfgV8_t * pCfgV8)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV7->cfgHdr.version != DOT1X_CFG_VER_7 &&
      pCfgV7->cfgHdr.version != DOT1X_CFG_VER_6 &&
      pCfgV7->cfgHdr.version != DOT1X_CFG_VER_5 &&
      pCfgV7->cfgHdr.version != DOT1X_CFG_VER_4 &&
      pCfgV7->cfgHdr.version != DOT1X_CFG_VER_3 &&
      pCfgV7->cfgHdr.version != DOT1X_CFG_VER_2 &&
      pCfgV7->cfgHdr.version != DOT1X_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV7->cfgHdr.version, DOT1X_CFG_VER_7);
    return L7_FAILURE;
  }

  dot1xBuildDefaultConfigDataV8 (pCfgV8);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV8->dot1xAdminMode = pCfgV7->dot1xAdminMode;
  pCfgV8->dot1xLogTraceMode = pCfgV7->dot1xLogTraceMode;
  pCfgV8->vlanAssignmentMode = pCfgV7->vlanAssignmentMode;
  
  intIfCount = L7_DOT1X_INTF_MAX_COUNT;
  for (i = 0; i < intIfCount; i++)
  {
    pCfgV8->dot1xPortCfg[i].adminControlledDirections = pCfgV7->dot1xPortCfg[i].adminControlledDirections;
    pCfgV8->dot1xPortCfg[i].portControlMode = pCfgV7->dot1xPortCfg[i].portControlMode;
    pCfgV8->dot1xPortCfg[i].quietPeriod = pCfgV7->dot1xPortCfg[i].quietPeriod;
    pCfgV8->dot1xPortCfg[i].txPeriod = pCfgV7->dot1xPortCfg[i].txPeriod;
    pCfgV8->dot1xPortCfg[i].suppTimeout = pCfgV7->dot1xPortCfg[i].suppTimeout;
    pCfgV8->dot1xPortCfg[i].serverTimeout = pCfgV7->dot1xPortCfg[i].serverTimeout;
    pCfgV8->dot1xPortCfg[i].maxReq = pCfgV7->dot1xPortCfg[i].maxReq;
    pCfgV8->dot1xPortCfg[i].reAuthPeriod = pCfgV7->dot1xPortCfg[i].reAuthPeriod;
    pCfgV8->dot1xPortCfg[i].reAuthEnabled = pCfgV7->dot1xPortCfg[i].reAuthEnabled;
    pCfgV8->dot1xPortCfg[i].keyTxEnabled = pCfgV7->dot1xPortCfg[i].keyTxEnabled;
    pCfgV8->dot1xPortCfg[i].guestVlanId = pCfgV7->dot1xPortCfg[i].guestVlanId;
    pCfgV8->dot1xPortCfg[i].guestVlanPeriod = pCfgV7->dot1xPortCfg[i].guestVlanPeriod;
    pCfgV8->dot1xPortCfg[i].maxUsers = pCfgV7->dot1xPortCfg[i].maxUsers;

    (void) nimConfigIdCopy (&pCfgV7->dot1xPortCfg[i].configId, &pCfgV8->dot1xPortCfg[i].configId);

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
dot1xBuildDefaultConfigDataV2 (dot1xCfgV2_t * pCfgV2)
{
  L7_uint32 cfgIndex;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT_REL_4_1; cfgIndex++)
  {
    dot1xPortCfgV2_t *pCfg = &pCfgV2->dot1xPortCfg[cfgIndex];

    pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
    pCfg->portControlMode = FD_DOT1X_PORT_MODE;
    pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
    pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;
    pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
    pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
    pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
    pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
    pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
    pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
  }

  strcpy ((L7_char8 *) pCfgV2->cfgHdr.filename, DOT1X_CFG_FILENAME);
  pCfgV2->cfgHdr.version = DOT1X_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;

  pCfgV2->dot1xAdminMode = FD_DOT1X_ADMIN_MODE;
  pCfgV2->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;

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
dot1xBuildDefaultConfigDataV3 (dot1xCfgV3_t * pCfgV3)
{
  L7_uint32 cfgIndex;

  memset (pCfgV3, 0, sizeof (*pCfgV3));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT_REL_4_3; cfgIndex++)
  {
    dot1xPortCfgV2_t *pCfg = &pCfgV3->dot1xPortCfg[cfgIndex];

    pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
    pCfg->portControlMode = FD_DOT1X_PORT_MODE;
    pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
    pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;
    pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
    pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
    pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
    pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
    pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
    pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
  }

  strcpy ((L7_char8 *) pCfgV3->cfgHdr.filename, DOT1X_CFG_FILENAME);
  pCfgV3->cfgHdr.version = DOT1X_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32) sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  pCfgV3->dot1xAdminMode = FD_DOT1X_ADMIN_MODE;
  pCfgV3->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;

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
dot1xBuildDefaultConfigDataV4 (dot1xCfgV4_t * pCfgV4)
{
  L7_uint32 cfgIndex;

  memset (pCfgV4, 0, sizeof (*pCfgV4));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT_REL_4_4; cfgIndex++)
  {
    dot1xPortCfgV2_t *pCfg = &pCfgV4->dot1xPortCfg[cfgIndex];

    pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
    pCfg->portControlMode = FD_DOT1X_PORT_MODE;
    pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
    pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;
    pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
    pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
    pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
    pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
    pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
    pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
  }

  strcpy ((L7_char8 *) pCfgV4->cfgHdr.filename, DOT1X_CFG_FILENAME);
  pCfgV4->cfgHdr.version = DOT1X_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = (L7_uint32) sizeof (*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;

  pCfgV4->dot1xAdminMode = FD_DOT1X_ADMIN_MODE;
  pCfgV4->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;

}


/*********************************************************************
* @purpose  Build Version 5 defaults
*
* @param    pCfgV5   @b{(input)} ptr to version 5 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1xBuildDefaultConfigDataV5 (dot1xCfgV5_t * pCfgV5)
{
  L7_uint32 cfgIndex;

  memset (pCfgV5, 0, sizeof (*pCfgV5));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT_REL_4_4; cfgIndex++)
  {
    dot1xPortCfgV2_t *pCfg = &pCfgV5->dot1xPortCfg[cfgIndex];

    pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
    pCfg->portControlMode = FD_DOT1X_PORT_MODE;
    pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
    pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;
    pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
    pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
    pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
    pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
    pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
    pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
  }

  strcpy ((L7_char8 *) pCfgV5->cfgHdr.filename, DOT1X_CFG_FILENAME);
  pCfgV5->cfgHdr.version = DOT1X_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = (L7_uint32) sizeof (*pCfgV5);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;

  pCfgV5->dot1xAdminMode = FD_DOT1X_ADMIN_MODE;
  pCfgV5->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;
  pCfgV5->vlanAssignmentMode = FD_DOT1X_VLAN_ASSIGN_MODE;

}

/*********************************************************************
* @purpose  Build Version 6 defaults
*
* @param    pCfgV6   @b{(input)} ptr to version 6 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1xBuildDefaultConfigDataV6 (dot1xCfgV6_t * pCfgV6)
{
  L7_uint32 cfgIndex;

  memset (pCfgV6, 0, sizeof (*pCfgV6));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT_REL_4_4; cfgIndex++)
  {
    dot1xPortCfgV3_t *pCfg = &pCfgV6->dot1xPortCfg[cfgIndex];

    pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
    pCfg->portControlMode = FD_DOT1X_PORT_MODE;
    pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
    pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;
    pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
    pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
    pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
    pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
    pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
    pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
    pCfg->guestVlanId = FD_DOT1X_PORT_GUEST_VLAN;
    pCfg->guestVlanPeriod = FD_DOT1X_PORT_GUEST_PERIOD;
  }

  strcpy ((L7_char8 *) pCfgV6->cfgHdr.filename, DOT1X_CFG_FILENAME);
  pCfgV6->cfgHdr.version = DOT1X_CFG_VER_6;
  pCfgV6->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;
  pCfgV6->cfgHdr.type = L7_CFG_DATA;
  pCfgV6->cfgHdr.length = (L7_uint32) sizeof (*pCfgV6);
  pCfgV6->cfgHdr.dataChanged = L7_FALSE;

  pCfgV6->dot1xAdminMode = FD_DOT1X_ADMIN_MODE;
  pCfgV6->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;
  pCfgV6->vlanAssignmentMode = FD_DOT1X_VLAN_ASSIGN_MODE;
}

/*********************************************************************
* @purpose  Build Version 7 defaults
*
* @param    pCfgV7   @b{(input)} ptr to version 7 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1xBuildDefaultConfigDataV7 (dot1xCfgV7_t * pCfgV7)
{
  L7_uint32 cfgIndex;

  memset (pCfgV7, 0, sizeof (*pCfgV7));

  for (cfgIndex = 1; cfgIndex < L7_DOT1X_INTF_MAX_COUNT; cfgIndex++)
  {
    dot1xPortCfgV7_t *pCfg = &pCfgV7->dot1xPortCfg[cfgIndex];

    pCfg->adminControlledDirections = FD_DOT1X_PORT_DIRECTION;
    pCfg->portControlMode = FD_DOT1X_PORT_MODE;
    pCfg->quietPeriod = FD_DOT1X_PORT_QUIET_PERIOD;
    pCfg->txPeriod = FD_DOT1X_PORT_TX_PERIOD;
    pCfg->suppTimeout = FD_DOT1X_PORT_SUPP_TIMEOUT;
    pCfg->serverTimeout = FD_DOT1X_PORT_SERVER_TIMEOUT;
    pCfg->maxReq = FD_DOT1X_PORT_MAX_REQ;
    pCfg->reAuthPeriod = FD_DOT1X_PORT_REAUTH_PERIOD;
    pCfg->reAuthEnabled = FD_DOT1X_PORT_REAUTH_ENABLED;
    pCfg->keyTxEnabled = FD_DOT1X_PORT_KEY_TRANSMISSION_ENABLED;
    pCfg->guestVlanId = FD_DOT1X_PORT_GUEST_VLAN;
    pCfg->guestVlanPeriod = FD_DOT1X_PORT_GUEST_PERIOD;
    pCfg->maxUsers = FD_DOT1X_PORT_MAX_USERS;
  }

  strcpy ((L7_char8 *) pCfgV7->cfgHdr.filename, DOT1X_CFG_FILENAME);
  pCfgV7->cfgHdr.version = DOT1X_CFG_VER_7;
  pCfgV7->cfgHdr.componentID = L7_DOT1X_COMPONENT_ID;
  pCfgV7->cfgHdr.type = L7_CFG_DATA;
  pCfgV7->cfgHdr.length = (L7_uint32) sizeof (*pCfgV7);
  pCfgV7->cfgHdr.dataChanged = L7_FALSE;

  pCfgV7->dot1xAdminMode = FD_DOT1X_ADMIN_MODE;
  pCfgV7->dot1xLogTraceMode = FD_DOT1X_LOG_TRACE_MODE;
  pCfgV7->vlanAssignmentMode = FD_DOT1X_VLAN_ASSIGN_MODE;
}


/*********************************************************************
* @purpose  Build Version 8 defaults
*
* @param    pCfgV8    @b{(input)} ptr to version 8 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
dot1xBuildDefaultConfigDataV8 (dot1xCfgV8_t * pCfgV8)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (dot1xCfgV8_t));
  dot1xBuildDefaultConfigData (DOT1X_CFG_VER_8);
  memcpy ((L7_uchar8 *) pCfgV8, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur)); 
}


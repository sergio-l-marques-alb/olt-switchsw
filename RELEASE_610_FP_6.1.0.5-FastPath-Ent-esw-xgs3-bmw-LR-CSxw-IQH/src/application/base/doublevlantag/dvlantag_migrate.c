
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dvlantag_migrate.c
*
* @purpose   Double Vlan Tagging configuration Migration
*
* @component DvlanTag
*
* @comments  none
*
* @create    8/23/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*             
**********************************************************************/

#include "dvlantag_migrate.h"

static L7_RC_t dvlantagMigrateConfigV1V2Convert (DVLANTAG_CFG_V1_t * pCfgV1,
                                                 DVLANTAG_CFG_V2_t * pCfgV2);

static L7_RC_t dvlantagMigrateConfigV2V3Convert (DVLANTAG_CFG_V2_t * pCfgV2,
                                                 DVLANTAG_CFG_V3_t * pCfgV3);

static L7_RC_t dvlantagMigrateConfigV3V4Convert (DVLANTAG_CFG_V3_t * pCfgV3,
                                                 DVLANTAG_CFG_V4_t * pCfgV4);

static void dvlantagBuildDefaultConfigDataV2 (DVLANTAG_CFG_V2_t * pCfgV2);
static void dvlantagBuildDefaultConfigDataV3 (DVLANTAG_CFG_V3_t * pCfgV3);
static void dvlantagBuildDefaultConfigDataV4 (DVLANTAG_CFG_V4_t * pCfgV4);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static DVLANTAG_CFG_t *pCfgCur = L7_NULLPTR;

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
dvlantagMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  DVLANTAG_CFG_V1_t *pCfgV1 = L7_NULLPTR;
  DVLANTAG_CFG_V2_t *pCfgV2 = L7_NULLPTR;
  DVLANTAG_CFG_V3_t *pCfgV3 = L7_NULLPTR;
  DVLANTAG_CFG_V4_t *pCfgV4 = L7_NULLPTR;


  
  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (DVLANTAG_CFG_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case DVLANTAG_CFG_VER_1:
    pCfgV1 = (DVLANTAG_CFG_V1_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                (L7_uint32) sizeof (DVLANTAG_CFG_V1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (DVLANTAG_CFG_V1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = DVLANTAG_CFG_VER_1;
    break;

  case DVLANTAG_CFG_VER_2:
    pCfgV2 = (DVLANTAG_CFG_V2_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                (L7_uint32) sizeof (DVLANTAG_CFG_V2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (DVLANTAG_CFG_V2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = DVLANTAG_CFG_VER_2;
    break;

  case DVLANTAG_CFG_VER_3:
    pCfgV3 = (DVLANTAG_CFG_V3_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                (L7_uint32) sizeof (DVLANTAG_CFG_V3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (DVLANTAG_CFG_V3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = DVLANTAG_CFG_VER_3;
    break;

  case DVLANTAG_CFG_VER_4:
    pCfgV4 = (DVLANTAG_CFG_V4_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                (L7_uint32) sizeof (DVLANTAG_CFG_V4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (DVLANTAG_CFG_V4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = DVLANTAG_CFG_VER_4;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_DVLANTAG_COMPONENT_ID,
                                         DVLANTAG_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DVLANTAG_COMPONENT_ID,
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
    case DVLANTAG_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (DVLANTAG_CFG_V2_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                  (L7_uint32) sizeof (DVLANTAG_CFG_V2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) ||
          (L7_SUCCESS != dvlantagMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case DVLANTAG_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (DVLANTAG_CFG_V3_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                  (L7_uint32) sizeof (DVLANTAG_CFG_V3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != dvlantagMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case DVLANTAG_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (DVLANTAG_CFG_V4_t *) osapiMalloc (L7_DVLANTAG_COMPONENT_ID,
                                                  (L7_uint32) sizeof (DVLANTAG_CFG_V4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != dvlantagMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case DVLANTAG_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      if (L7_NULLPTR == pCfgV4)
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgCur));
      osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;

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
    osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DVLANTAG_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dvlantagBuildDefaultConfigData (DVLANTAG_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
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
dvlantagMigrateConfigV1V2Convert (DVLANTAG_CFG_V1_t * pCfgV1, DVLANTAG_CFG_V2_t * pCfgV2)
{
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 intCfgIndex;
  L7_uint32 intIfCount;
  nimConfigID_t configId;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != DVLANTAG_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, DVLANTAG_CFG_VER_1);
    return L7_FAILURE;
  }

  dvlantagBuildDefaultConfigDataV2 (pCfgV2);

  if (L7_TRUE != cnfgrIsFeaturePresent (L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_FEATURE_ID))
    return L7_SUCCESS;

  if (L7_FALSE != cnfgrIsFeaturePresent (L7_DVLANTAG_COMPONENT_ID,
                                         L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID))
    return L7_SUCCESS;

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_0,
                    L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_1);
  intCfgIndex = 1;
  for (i = 1; i < intIfCount; i++)
  {
    slot = pCfgV1->intfCfg[i].usp.slot;
    port = pCfgV1->intfCfg[i].usp.port;
    rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_SLOT_PORT (slot, port);
      continue;
    }

    /* check if the interface type is valid */
    if (L7_SUCCESS != dvlantagIsValidIntfType (configId.type))
      continue;

    if (intCfgIndex >= L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (DVLANTAG_CFG_FILENAME, intCfgIndex);
      break;
    }

    (void) nimConfigIdCopy (&configId, &pCfgV2->intfCfg[intCfgIndex].configId);
    pCfgV2->intfCfg[intCfgIndex].mode = pCfgV1->intfCfg[i].mode;
    /* pCfgV2->intfCfg[intCfgIndex].etherType[0] = pCfgV1->intfCfg[i].etherType[0]; */
    pCfgV2->intfCfg[intCfgIndex].custId = pCfgV1->intfCfg[i].custId;
    intCfgIndex++;
  }

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
dvlantagMigrateConfigV2V3Convert (DVLANTAG_CFG_V2_t * pCfgV2, DVLANTAG_CFG_V3_t * pCfgV3)
{
  L7_uint32 i;
  L7_uint32 intIfCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != DVLANTAG_CFG_VER_2 && pCfgV2->cfgHdr.version != DVLANTAG_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, DVLANTAG_CFG_VER_2);
    return L7_FAILURE;
  }

  dvlantagBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_1, L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_3);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->intfCfg[i], pCfgV2->intfCfg[i]);
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
dvlantagMigrateConfigV3V4Convert (DVLANTAG_CFG_V3_t * pCfgV3, DVLANTAG_CFG_V4_t * pCfgV4)
{
  L7_uint32 i;
  L7_uint32 intIfCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != DVLANTAG_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != DVLANTAG_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != DVLANTAG_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, DVLANTAG_CFG_VER_3);
    return L7_FAILURE;
  }

  dvlantagBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate
     fields in the newer one
   */
  intIfCount = min (L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_3,
                    L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->intfCfg[i], pCfgV3->intfCfg[i]);
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
dvlantagBuildDefaultConfigDataV2 (DVLANTAG_CFG_V2_t * pCfgV2)
{
  L7_uint32 i;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  for (i = 0; i < L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_1; i++)
  {
    DVLANTAG_INTF_CFG_t *pCfg = &pCfgV2->intfCfg[i];

    pCfg->custId = L7_DVLANTAG_DEFAULT_CUSTID;
    pCfg->etherType[0] = L7_DVLANTAG_DEFAULT_ETHERTYPE;
    pCfg->mode = L7_DVLANTAG_DEFAULT_PORT_MODE;
  }

  strcpy ((L7_char8 *) pCfgV2->cfgHdr.filename, DVLANTAG_CFG_FILENAME);
  pCfgV2->cfgHdr.version = DVLANTAG_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_DVLANTAG_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
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
dvlantagBuildDefaultConfigDataV3 (DVLANTAG_CFG_V3_t * pCfgV3)
{
  L7_uint32 i;

  memset((void *)pCfgV3, 0, sizeof(*pCfgV3));

  for (i = 0; i < L7_DVLANTAG_MAX_INTERFACE_COUNT_REL_4_3; i++)
  {
    DVLANTAG_INTF_CFG_t *pCfg = &pCfgV3->intfCfg[i];

    pCfg->custId = L7_DVLANTAG_DEFAULT_CUSTID;
    pCfg->etherType[0] = L7_DVLANTAG_DEFAULT_ETHERTYPE;
    pCfg->mode = L7_DVLANTAG_DEFAULT_PORT_MODE;
  }

  strcpy((L7_char8 *)pCfgV3->cfgHdr.filename, DVLANTAG_CFG_FILENAME);
  pCfgV3->cfgHdr.version = DVLANTAG_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_DVLANTAG_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32)sizeof(*pCfgV3);
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
dvlantagBuildDefaultConfigDataV4 (DVLANTAG_CFG_V4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (DVLANTAG_CFG_V4_t));
  dvlantagBuildDefaultConfigData (DVLANTAG_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

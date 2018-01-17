/**********************************************************************
* @filename lldp_migrate.c
*
* @purpose LLDP MED configuration file migrate
*
* @component 802.1AB
*
* @comments none
*
* @create 12/19/2006
*
* @author skalyanam
*
* @end
*
**********************************************************************/
#include "lldp_migrate.h"

static L7_RC_t lldpMigrateConfigV1V2Convert(lldpCfgDataV1_t * pCfgV1, lldpCfgDataV2_t * pCfgV2);
static void lldpBuildDefaultConfigDataV2 (lldpCfgDataV2_t * pCfgV4);
/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static lldpCfgData_t *pCfgCur = L7_NULLPTR;

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
void lldpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  lldpCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  lldpCfgDataV2_t *pCfgV2 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (lldpCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case LLDP_CFG_VER_1:
    pCfgV1 = (lldpCfgDataV1_t *) osapiMalloc (L7_LLDP_COMPONENT_ID,
                                              (L7_uint32) sizeof (lldpCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (lldpCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = LLDP_CFG_VER_1;
    break;

  case LLDP_CFG_VER_2:
    pCfgV2 = (lldpCfgDataV2_t *) osapiMalloc (L7_LLDP_COMPONENT_ID,
                                              (L7_uint32) sizeof (lldpCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (lldpCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = LLDP_CFG_VER_2;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_LLDP_COMPONENT_ID,
                                         LLDP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LLDP_COMPONENT_ID,
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
    case LLDP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (lldpCfgDataV2_t *) osapiMalloc (L7_LLDP_COMPONENT_ID,
                                                (L7_uint32) sizeof (lldpCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) || (L7_SUCCESS != lldpMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_LLDP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case LLDP_CFG_VER_2:
      if (pCfgV2 != L7_NULLPTR)
      {
        /* pCfgV2 has already been allocated and filled in */
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV2, sizeof (*pCfgCur));
        osapiFree (L7_LLDP_COMPONENT_ID, pCfgV2);
      }

      pCfgV2 = L7_NULLPTR;

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
    osapiFree (L7_LLDP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_LLDP_COMPONENT_ID, pCfgV2);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_LLDP_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    lldpBuildDefaultConfigData (LLDP_CFG_VER_CURRENT);
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
lldpMigrateConfigV1V2Convert (lldpCfgDataV1_t * pCfgV1, lldpCfgDataV2_t * pCfgV2)
{
  L7_uint32 i, intfCount;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != LLDP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, LLDP_CFG_VER_1);
    return L7_FAILURE;
  }

  lldpBuildDefaultConfigDataV2 (pCfgV2);

  /*
   * Map the fields in the older structure
   * to the appropriate fields in the newer one
   */
  pCfgV2->notifyInterval = pCfgV1->notifyInterval;
  pCfgV2->reinitDelay = pCfgV1->reinitDelay;
  pCfgV2->txHoldMultiplier = pCfgV1->txHoldMultiplier;
  pCfgV2->txInterval = pCfgV1->txInterval;


  intfCount = min (L7_LLDP_INTF_MAX_COUNT_REL_4_4, L7_LLDP_INTF_MAX_COUNT);
  for (i = 0; i < intfCount; i++)
  {
    MIGRATE_COPY_STRUCT(pCfgV2->intfCfgData[i].cfgId, pCfgV1->intfCfgData[i].cfgId);
    pCfgV2->intfCfgData[i].mgmtAddrTxEnabled = pCfgV1->intfCfgData[i].mgmtAddrTxEnabled;
    pCfgV2->intfCfgData[i].notificationEnabled = pCfgV1->intfCfgData[i].notificationEnabled;
    pCfgV2->intfCfgData[i].optionalTLVsEnabled = pCfgV1->intfCfgData[i].optionalTLVsEnabled;
    pCfgV2->intfCfgData[i].rxEnabled = pCfgV1->intfCfgData[i].rxEnabled;
    pCfgV2->intfCfgData[i].txEnabled = pCfgV1->intfCfgData[i].txEnabled;
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
lldpBuildDefaultConfigDataV2 (lldpCfgDataV2_t * pCfgV2)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (lldpCfgDataV2_t));
  lldpBuildDefaultConfigData (LLDP_CFG_VER_2);
  memcpy ((L7_uchar8 *) pCfgV2, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pimsm_migrate.c
*
* @purpose pimsm Configuration Migration
*
* @component pimsm
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "pimsm_migrate.h"
#include "pimsm_map.h"
static L7_RC_t pimsmMigrateConfigV2V3Convert (L7_pimsmMapCfgV2_t * pCfgV2,
                                              L7_pimsmMapCfgV3_t * pCfgV3);

static L7_RC_t pimsmMigrateConfigV3V4Convert (L7_pimsmMapCfgV3_t * pCfgV3,
                                              L7_pimsmMapCfgV4_t * pCfgV4);

static L7_RC_t pimsmMigrateConfigV4V5Convert (L7_pimsmMapCfgV4_t * pCfgV4,
                                              L7_pimsmMapCfgV5_t * pCfgV5);

static L7_RC_t pimsmMigrateConfigV5V6Convert (L7_pimsmMapCfgV5_t * pCfgV5,
                                              L7_pimsmMapCfgV6_t * pCfgV6);

static void pimsmBuildDefaultConfigDataV3 (L7_pimsmMapCfgV3_t * pCfgV3);
static void pimsmBuildDefaultConfigDataV4 (L7_pimsmMapCfgV4_t * pCfgV4);
static void pimsmBuildDefaultConfigDataV5 (L7_pimsmMapCfgV5_t * pCfgV5);
static void pimsmBuildDefaultConfigDataV6 (L7_pimsmMapCfgV6_t * pCfgV6);

static L7_pimsmMapCfg_t *pCfgCur = L7_NULLPTR;
/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @comments This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @comments none
*
* @end
*********************************************************************/
void
pimsm6MigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;

  /* declare a pointer to each supported config structure version */
  L7_pimsmMapCfgV5_t *pCfgV5 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pCfgCur = (L7_pimsmMapCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if(pCfgHdr->version == L7_PIMSM_CFG_VER_2 && 
     pCfgHdr->length == sizeof(L7_pimsmMapCfgV3_t))
  {
    oldVer = L7_PIMSM_CFG_VER_3;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {

  case L7_PIMSM_CFG_VER_5:
    pCfgV5 = (L7_pimsmMapCfgV5_t *) osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_pimsmMapCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (L7_pimsmMapCfgV5_t);
    pOldCfgCksum = &pCfgV5->checksum;
    oldCfgVer = L7_PIMSM_CFG_VER_5;
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
    /* read the old config file found on this system 
     *
     * Note:
     *
     * The routine sysapiCfgFileGet which invokes the migrate routine will pass
     * in the a pointer to the location of the configuration data read from
     * nonvolatile storage.

     * In cases where the older version of the configuration file may be larger
     * than the current version, sysapiCfgFileGet truncates the data to the size
     * of the storage location pointed to by the buffer provided for the component.
     * Thus, it is important to read the configuration file again so that the entirety
     * of the configuration data can be accessed.
     *
     */

    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      L7_PIMSM6_CFG_FILENAME, pOldCfgBuf,
                                      oldCfgSize, pOldCfgCksum, oldCfgVer,
                                      L7_NULL /* do not build defaults */ ,
                                      L7_NULL /* do not call migrate again */
        )))
    {
      MIGRATE_DEBUG ("sysapiCfgFileGet failed size = %d version = %d\n", 
                    (L7_int32) oldCfgSize,
                     (L7_int32) oldCfgVer);
      buildDefault = L7_TRUE;
    }
  }

  /* Convert the old config file to the latest version.
   * Each version of the configuation is iteratively migrated to the
   * next version until brought up to the current level.
   */

  if (buildDefault != L7_TRUE)
  {
    switch (oldVer)
    {
    case L7_PIMSM_CFG_VER_5:
      if (L7_NULLPTR == pCfgV5)
      {
        MIGRATE_DEBUG ("\n\rpCfgV5 Null Pointer\r\n");
        break;
      }
      /* pCfgV5 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV5);
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
  if (pCfgV5 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV5);
  }

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    pimsmBuildDefaultConfigData (L7_PIMSM_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}
/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @comments This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @comments none
*
* @end
*********************************************************************/
void
pimsmMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;

  /* declare a pointer to each supported config structure version */
  L7_pimsmMapCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_pimsmMapCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_pimsmMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_pimsmMapCfgV5_t *pCfgV5 = L7_NULLPTR;
  L7_pimsmMapCfgV6_t *pCfgV6 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pCfgCur = (L7_pimsmMapCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if(pCfgHdr->version == L7_PIMSM_CFG_VER_2 && 
     pCfgHdr->length == sizeof(L7_pimsmMapCfgV3_t))
  {
    oldVer = L7_PIMSM_CFG_VER_3;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_PIMSM_CFG_VER_2:
    pCfgV2 = (L7_pimsmMapCfgV2_t *) osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_pimsmMapCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_pimsmMapCfgV2_t);
    pOldCfgCksum = &pCfgV2->checksum;
    oldCfgVer = L7_PIMSM_CFG_VER_2;
    break;

  case L7_PIMSM_CFG_VER_3:
    pCfgV3 = (L7_pimsmMapCfgV3_t *) osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_pimsmMapCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_pimsmMapCfgV3_t);
    pOldCfgCksum = &pCfgV3->checksum;
    oldCfgVer = pCfgHdr->version;
    break;

  case L7_PIMSM_CFG_VER_4:
    pCfgV4 = (L7_pimsmMapCfgV4_t *) osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_pimsmMapCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_pimsmMapCfgV4_t);
    pOldCfgCksum = &pCfgV4->checksum;
    oldCfgVer = L7_PIMSM_CFG_VER_4;
    break;

    case L7_PIMSM_CFG_VER_5:
    pCfgV5 = (L7_pimsmMapCfgV5_t *) osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_pimsmMapCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (L7_pimsmMapCfgV5_t);
    pOldCfgCksum = &pCfgV5->checksum;
    oldCfgVer = L7_PIMSM_CFG_VER_5;
    break;

    case L7_PIMSM_CFG_VER_6:
    pCfgV6 = (L7_pimsmMapCfgV6_t *) osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_pimsmMapCfgV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (L7_pimsmMapCfgV6_t);
    pOldCfgCksum = &pCfgV6->checksum;
    oldCfgVer = L7_PIMSM_CFG_VER_6;
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
    /* read the old config file found on this system 
     *
     * Note:
     *
     * The routine sysapiCfgFileGet which invokes the migrate routine will pass
     * in the a pointer to the location of the configuration data read from
     * nonvolatile storage.

     * In cases where the older version of the configuration file may be larger
     * than the current version, sysapiCfgFileGet truncates the data to the size
     * of the storage location pointed to by the buffer provided for the component.
     * Thus, it is important to read the configuration file again so that the entirety
     * of the configuration data can be accessed.
     *
     */

    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                      L7_PIMSM_CFG_FILENAME, pOldCfgBuf,
                                      oldCfgSize, pOldCfgCksum, oldCfgVer,
                                      L7_NULL /* do not build defaults */ ,
                                      L7_NULL /* do not call migrate again */
        )))
    {
      MIGRATE_DEBUG ("sysapiCfgFileGet failed size = %d version = %d\n", 
                    (L7_int32) oldCfgSize,
                     (L7_int32) oldCfgVer);
      buildDefault = L7_TRUE;
    }
  }

  /* Convert the old config file to the latest version.
   * Each version of the configuation is iteratively migrated to the
   * next version until brought up to the current level.
   */

  if (buildDefault != L7_TRUE)
  {
    switch (oldVer)
    {
    case L7_PIMSM_CFG_VER_2:
      if (L7_NULLPTR == pCfgV2)
      {
        MIGRATE_DEBUG ("\n\rpCfgV2 Null Pointer\r\n");
        break;
      }
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 =(L7_pimsmMapCfgV3_t *)osapiMalloc (L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                       (L7_uint32) sizeof (L7_pimsmMapCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || 
          (L7_SUCCESS != pimsmMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_PIMSM_CFG_VER_3:
      if (L7_NULLPTR == pCfgV3)
      {
        MIGRATE_DEBUG ("\n\rpCfgV3 Null Pointer\r\n");
        break;
      }
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (L7_pimsmMapCfgV4_t *)osapiMalloc(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                     (L7_uint32) sizeof (L7_pimsmMapCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || 
          (L7_SUCCESS != pimsmMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_PIMSM_CFG_VER_4:
      if (L7_NULLPTR == pCfgV4)
      {
        MIGRATE_DEBUG ("\n\rpCfgV4 Null Pointer\r\n");
        break;
      }
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (L7_pimsmMapCfgV5_t *)osapiMalloc(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                       (L7_uint32) sizeof (L7_pimsmMapCfgV5_t));
      if ((L7_NULLPTR == pCfgV5)||(L7_SUCCESS != 
           pimsmMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

      case L7_PIMSM_CFG_VER_5:
      if (L7_NULLPTR == pCfgV5)
      {
        MIGRATE_DEBUG ("\n\rpCfgV5 Null Pointer\r\n");
        break;
      }
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (L7_pimsmMapCfgV6_t *)osapiMalloc(L7_FLEX_PIMSM_MAP_COMPONENT_ID,
                                       (L7_uint32) sizeof (L7_pimsmMapCfgV6_t));
      if ((L7_NULLPTR == pCfgV6)||(L7_SUCCESS != 
           pimsmMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;

      /*passthru*/

      case L7_PIMSM_CFG_VER_6:
        if (L7_NULLPTR == pCfgV6)
        {
          MIGRATE_DEBUG ("\n\rpCfgV6 Null Pointer\r\n");
          break;
        }
        /* pCfgV5 has already been allocated and filled in */
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV6, sizeof (*pCfgCur));
        osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV6);
        pCfgV6 = L7_NULLPTR;

        /* done with migration - Flag unsaved configuration */
        pCfgCur->cfgHdr.dataChanged = L7_TRUE;
        break;


    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV2 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV2);
  }
  if (pCfgV3 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV3);
  }
  if (pCfgV4 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV4);
  }
  if (pCfgV5 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV5);
  }
  if (pCfgV6 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_PIMSM_MAP_COMPONENT_ID, pCfgV6);
  }

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    pimsmBuildDefaultConfigData (L7_PIMSM_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}


/*********************************************************************
* @purpose  Converts the interface config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 3 config data structure
* @param    pCfgV3    @b{(input)} ptr to version 4 config data structure
* @param    intfType  @b{(input)} interface types being migrated
* @param    offset    @b{(input)} current offset to version 4 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none   
*          
*
* @end
*********************************************************************/
static void
pimsmMigrateIntfConfigV2V3Convert (L7_pimsmMapCfgV2_t * pCfgV2, 
                                   L7_pimsmMapCfgV3_t * pCfgV3,
                                   L7_uint32 intfType, L7_uint32 * offset)
{
  L7_RC_t rc;
  L7_uint32 i, j, intCfgIndex;
  nimConfigID_t configId;
  L7_BOOL truncated;

  /*         
   * Note: The methodology used by the policyMigrate* routines in migrating 
   *       different types  of interfaces is more mature than the multiple 
   *       invocations of the same routine implemented for this routine as 
   *       fewer loop checks are required. However, this works, so we go with 
   *       it.  
   *  
   * Note: The truncated flag merely indicates if the migration was truncated 
   *        within this invocation of this routine. 
   */ 

  truncated = L7_FALSE;
  intCfgIndex = *offset;
  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    for (j = 1; j <= L7_MAX_PORTS_PER_SLOT_REL_4_0; j++)
    {
      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (i, j, &configId, L7_NULL, 
                                                     L7_NULL);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
        {
          MIGRATE_INVALID_SLOT_PORT (i, j);
        }
        continue;
      }

      if (configId.type != intfType)
      {
        continue;
      }

      if (intCfgIndex >= L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE(L7_PIMSM_CFG_FILENAME, intCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV3->intf[intCfgIndex].configId);
      pCfgV3->intf[intCfgIndex].mode = pCfgV2->intf[i][j].mode;
      pCfgV3->intf[intCfgIndex].intfHelloInterval = pCfgV2->intf[i][j].
                                                        intfHelloInterval;
      pCfgV3->intf[intCfgIndex].intfJoinPruneInterval = pCfgV2->intf[i][j].
                                                         intfJoinPruneInterval;
      pCfgV3->intf[intCfgIndex].intfCBSRPreference = pCfgV2->intf[i][j].
                                                      intfCBSRPreference;

      /* The following elements are present in base REL-G
       * pCfgV3->intf[intCfgIndex].intfCBSRHashMskLen = 
       *                   pCfgV2->intf[i][j].intfCBSRHashMskLen;
       * pCfgV3->intf[intCfgIndex].intfCRPPreference = 
       *                   pCfgV2->intf[i][j].intfCRPPreference;
       */
      intCfgIndex++;
    }

    if (truncated == L7_TRUE)
    {
      break;
    }
  }
  *offset = intCfgIndex;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 3 config data structure
* @param    pCfgV3    @b{(input)} ptr to version 4 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
pimsmMigrateConfigV2V3Convert (L7_pimsmMapCfgV2_t * pCfgV2, 
                               L7_pimsmMapCfgV3_t * pCfgV3)
{
  L7_uint32 intCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_PIMSM_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_PIMSM_CFG_VER_2);
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Incorrect version of config file");
    return L7_FAILURE;
  }

  pimsmBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the 
   * newer one 
   */
  MIGRATE_COPY_STRUCT (pCfgV3->rtr, pCfgV2->rtr);

  intCfgIndex = 1;
  pimsmMigrateIntfConfigV2V3Convert (pCfgV2, pCfgV3, L7_PHYSICAL_INTF, 
                                     &intCfgIndex);

  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      pimsmMigrateIntfConfigV2V3Convert (pCfgV2, pCfgV3, L7_LOGICAL_VLAN_INTF, 
                                         &intCfgIndex);

  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      pimsmMigrateIntfConfigV2V3Convert (pCfgV2, pCfgV3, L7_LAG_INTF, 
                                         &intCfgIndex);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    pCfgV3    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV4    @b{(input)} ptr to version 5 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
pimsmMigrateConfigV3V4Convert (L7_pimsmMapCfgV3_t * pCfgV3, 
                               L7_pimsmMapCfgV4_t * pCfgV4)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if ((pCfgV3->cfgHdr.version != L7_PIMSM_CFG_VER_3) && 
      (pCfgV3->cfgHdr.version != L7_PIMSM_CFG_VER_2)) /* b'cos REL_H2 version 
                                                       * is in-correct 
                                                       */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, 
                                    L7_PIMSM_CFG_VER_3);
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Incorrect version of config file");
    return L7_FAILURE;
  }

  pimsmBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in 
   * the newer one 
   */
  MIGRATE_COPY_STRUCT (pCfgV4->rtr, pCfgV3->rtr);

  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_1, 
                    L7_IPMAP_INTF_MAX_COUNT_REL_4_3);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->intf[i], pCfgV3->intf[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5.
*
* @param    pCfgV4    @b{(input)} ptr to version 5 config data structure
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
pimsmMigrateConfigV4V5Convert (L7_pimsmMapCfgV4_t * pCfgV4, 
                               L7_pimsmMapCfgV5_t * pCfgV5)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if ((pCfgV4->cfgHdr.version != L7_PIMSM_CFG_VER_4) &&
      (pCfgV4->cfgHdr.version != L7_PIMSM_CFG_VER_3) &&
      (pCfgV4->cfgHdr.version != L7_PIMSM_CFG_VER_2)) /* b'cos REL_H2 version 
                                                       * is in-correct 
                                                       */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_PIMSM_CFG_VER_4);
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Incorrect version of config file");
    return L7_FAILURE;
  }

  pimsmBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the 
   * newer one 
   */
  MIGRATE_COPY_STRUCT (pCfgV5->rtr, pCfgV4->rtr);

  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_3, 
                    L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->intf[i], pCfgV4->intf[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build Version 4 defaults
*
* @param    pCfgV3    @b{(input)} ptr to version 4 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
pimsmBuildDefaultConfigDataV3 (L7_pimsmMapCfgV3_t * pCfgV3)
{
  L7_uint32 i, rpIpAddr, rpGrpAddr, rpGrpMask;

  memset (pCfgV3, 0, sizeof (*pCfgV3));

  /* generic PIM-SM cfg */
  pCfgV3->rtr.pimsmAdminMode = FD_PIMSM_DEFAULT_ADMIN_MODE;
  pCfgV3->rtr.joinPruneInterval = FD_PIMSM_DEFAULT_JOIN_PRUNE_INTERVAL;
  pCfgV3->rtr.dataThresholdRate = FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE;
  pCfgV3->rtr.regThresholdRate = FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE;
  for (i = 0; i < L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_1; i++)
  {
    rpIpAddr = FD_PIMSM_DEFAULT_STATIC_RP_IP_ADDR;
    /*inetAddressSet(L7_AF_INET, &rpIpAddr, 
                               &pCfgV3->rtr.pimsmStaticRP[i].rpIpAddr);*/
    pCfgV3->rtr.pimsmStaticRP[i].rpIpAddr = rpIpAddr;

    rpGrpAddr = FD_PIMSM_DEFAULT_STATIC_RP_GRP_ADDR;
    /*inetAddressSet(L7_AF_INET, &rpGrpAddr, 
                               &pCfgV3->rtr.pimsmStaticRP[i].rpGrpAddr);*/
    pCfgV3->rtr.pimsmStaticRP[i].rpGrpAddr = rpGrpAddr;
    rpGrpMask = FD_PIMSM_DEFAULT_STATIC_RP_GRP_MASK;
    /*inetAddressSet(L7_AF_INET, &rpGrpMask, 
                               &pCfgV3->rtr.pimsmStaticRP[i].rpGrpMask);*/
    pCfgV3->rtr.pimsmStaticRP[i].rpGrpMask = rpGrpMask;
  }
  /* PIM router interface configuration parameters */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_1; i++)
  {
    pimsmCfgCktV5_t *pCfg = &pCfgV3->intf[i];

    pCfg->mode = FD_PIMSM_DEFAULT_INTERFACE_MODE;
    pCfg->intfHelloInterval = FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL;
    pCfg->intfJoinPruneInterval =FD_PIMSM_DEFAULT_INTERFACE_JOIN_PRUNE_INTERVAL;
    pCfg->intfCBSRPreference = FD_PIMSM_CBSR_PRIORITY;
    pCfg->intfCBSRHashMskLen = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
    /*pCfg->intfCRPPreference = FD_PIMSM_DEFAULT_INTERFACE_CRP_PREFERENCE;*/
  }

  strcpy (pCfgV3->cfgHdr.filename, L7_PIMSM_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_PIMSM_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
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
pimsmBuildDefaultConfigDataV4 (L7_pimsmMapCfgV4_t * pCfgV4)
{
  L7_uint32 i, rpIpAddr, rpGrpAddr, rpGrpMask;;

  memset (pCfgV4, 0, sizeof (*pCfgV4));

  /* generic PIM-SM cfg */
  pCfgV4->rtr.pimsmAdminMode = FD_PIMSM_DEFAULT_ADMIN_MODE;
  pCfgV4->rtr.joinPruneInterval = FD_PIMSM_DEFAULT_JOIN_PRUNE_INTERVAL;
  pCfgV4->rtr.dataThresholdRate = FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE;
  pCfgV4->rtr.regThresholdRate = FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE;
  for (i = 0; i < L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_3; i++)
  {
    rpIpAddr = FD_PIMSM_DEFAULT_STATIC_RP_IP_ADDR;
    /*inetAddressSet(L7_AF_INET, &rpIpAddr, 
                               &pCfgV4->rtr.pimsmStaticRP[i].rpIpAddr);*/
    pCfgV4->rtr.pimsmStaticRP[i].rpIpAddr = rpIpAddr;
    rpGrpAddr = FD_PIMSM_DEFAULT_STATIC_RP_GRP_ADDR;
    /*inetAddressSet(L7_AF_INET, &rpGrpAddr, 
                               &pCfgV4->rtr.pimsmStaticRP[i].rpGrpAddr);*/
    pCfgV4->rtr.pimsmStaticRP[i].rpGrpAddr = rpGrpAddr;
    rpGrpMask = FD_PIMSM_DEFAULT_STATIC_RP_GRP_MASK;
    /*inetAddressSet(L7_AF_INET, &rpGrpMask, 
                               &pCfgV4->rtr.pimsmStaticRP[i].rpGrpMask);*/
    pCfgV4->rtr.pimsmStaticRP[i].rpGrpMask = rpGrpMask;
  }
  /* PIM router interface configuration parameters */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_3; i++)
  {
    pimsmCfgCktV5_t *pCfg = &pCfgV4->intf[i];

    pCfg->mode = FD_PIMSM_DEFAULT_INTERFACE_MODE;
    pCfg->intfHelloInterval = FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL;
    pCfg->intfJoinPruneInterval =FD_PIMSM_DEFAULT_INTERFACE_JOIN_PRUNE_INTERVAL;
    pCfg->intfCBSRPreference = FD_PIMSM_CBSR_PRIORITY;
    pCfg->intfCBSRHashMskLen = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
    /*pCfg->intfCRPPreference = FD_PIMSM_DEFAULT_INTERFACE_CRP_PREFERENCE;*/
  }

  strcpy (pCfgV4->cfgHdr.filename, L7_PIMSM_CFG_FILENAME);
  pCfgV4->cfgHdr.version = L7_PIMSM_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = sizeof (*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;
}

/* New Migration stuff*/
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
pimsmBuildDefaultConfigDataV5 (L7_pimsmMapCfgV5_t * pCfgV5)
{
  L7_uint32 i, rpIpAddr, rpGrpAddr, rpGrpMask;;

  memset (pCfgV5, 0, sizeof (*pCfgV5));

  /* generic PIM-SM cfg */
  pCfgV5->rtr.pimsmAdminMode = FD_PIMSM_DEFAULT_ADMIN_MODE;
  pCfgV5->rtr.joinPruneInterval = FD_PIMSM_DEFAULT_JOIN_PRUNE_INTERVAL;
  pCfgV5->rtr.dataThresholdRate = FD_PIMSM_DEFAULT_DATA_THRESHOLD_RATE;
  pCfgV5->rtr.regThresholdRate = FD_PIMSM_DEFAULT_REG_THRESHOLD_RATE;
  for (i = 0; i < L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_4; i++)
  {
    rpIpAddr = FD_PIMSM_DEFAULT_STATIC_RP_IP_ADDR;
    /*inetAddressSet(L7_AF_INET, &rpIpAddr, 
                               &pCfgV5->rtr.pimsmStaticRP[i].rpIpAddr);*/
    pCfgV5->rtr.pimsmStaticRP[i].rpIpAddr = rpIpAddr;
    rpGrpAddr = FD_PIMSM_DEFAULT_STATIC_RP_GRP_ADDR;
    /*inetAddressSet(L7_AF_INET, &rpGrpAddr, 
                               &pCfgV5->rtr.pimsmStaticRP[i].rpGrpAddr);*/
    pCfgV5->rtr.pimsmStaticRP[i].rpGrpAddr = rpGrpAddr;
    rpGrpMask = FD_PIMSM_DEFAULT_STATIC_RP_GRP_MASK;
    /*inetAddressSet(L7_AF_INET, &rpGrpMask, 
                               &pCfgV5->rtr.pimsmStaticRP[i].rpGrpMask);*/
    pCfgV5->rtr.pimsmStaticRP[i].rpGrpMask = rpGrpMask;
  }
  /* PIM router interface configuration parameters */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_4; i++)
  {
    pimsmCfgCktV5_t *pCfg = &pCfgV5->intf[i];

    pCfg->mode = FD_PIMSM_DEFAULT_INTERFACE_MODE;
    pCfg->intfHelloInterval = FD_PIMSM_DEFAULT_INTERFACE_HELLO_INTERVAL;
    pCfg->intfJoinPruneInterval =FD_PIMSM_DEFAULT_INTERFACE_JOIN_PRUNE_INTERVAL;
    pCfg->intfCBSRPreference = FD_PIMSM_CBSR_PRIORITY;
    pCfg->intfCBSRHashMskLen = FD_PIMSM_CBSR_HASH_MASK_LENGTH;
    /*pCfg->intfCRPPreference = FD_PIMSM_DEFAULT_INTERFACE_CRP_PREFERENCE;*/
  }

  strcpy (pCfgV5->cfgHdr.filename, L7_PIMSM_CFG_FILENAME);
  pCfgV5->cfgHdr.version = L7_PIMSM_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_FLEX_PIMSM_MAP_COMPONENT_ID;
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
static void
pimsmBuildDefaultConfigDataV6 (L7_pimsmMapCfgV6_t * pCfgV6)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (*pCfgCur));
  pimsmBuildDefaultConfigData (L7_PIMSM_CFG_VER_6);
  memcpy (pCfgV6, pCfgCur, sizeof (*pCfgCur));
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
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
pimsmMigrateConfigV5V6Convert (L7_pimsmMapCfgV5_t * pCfgV5, 
                               L7_pimsmMapCfgV6_t * pCfgV6)
{
  L7_uint32 i, intIfCount,rpIpAddr,rpGrpAddr,rpGrpMask;


  /* verify correct version of old config file */
  if ((pCfgV5->cfgHdr.version != L7_PIMSM_CFG_VER_5) &&
      (pCfgV5->cfgHdr.version != L7_PIMSM_CFG_VER_4) &&
      (pCfgV5->cfgHdr.version != L7_PIMSM_CFG_VER_3) &&
      (pCfgV5->cfgHdr.version != L7_PIMSM_CFG_VER_2)) /* b'cos REL_H2 version 
                                                       * is in-correct 
                                                       */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, L7_PIMSM_CFG_VER_5);
    PIMSM_MAP_DEBUG(PIMSM_MAP_DEBUG_FAILURES,"Incorrect version of config file");
    return L7_FAILURE;
  }

  pimsmBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the 
   * newer one 
   */
  /*MIGRATE_COPY_STRUCT (pCfgV6->rtr, pCfgV5->rtr);*/

  pCfgV6->rtr.regThresholdRate = pCfgV5->rtr.regThresholdRate;
  pCfgV6->rtr.pimsmAdminMode = pCfgV5->rtr.pimsmAdminMode;
  pCfgV6->rtr.dataThresholdRate = pCfgV5->rtr.dataThresholdRate;

  for (i = 0; i < L7_PIMSM_MAX_STATIC_RP_NUM_REL_4_4; i++)
  {
    rpIpAddr = pCfgV5->rtr.pimsmStaticRP[i].rpIpAddr;
    inetAddressSet(L7_AF_INET, &rpIpAddr, 
                               &pCfgV6->rtr.pimsmStaticRP[i].rpIpAddr);


    rpGrpAddr = pCfgV5->rtr.pimsmStaticRP[i].rpGrpAddr;
    inetAddressSet(L7_AF_INET, &rpGrpAddr, 
                               &pCfgV6->rtr.pimsmStaticRP[i].rpGrpAddr);
   

    rpGrpMask = pCfgV5->rtr.pimsmStaticRP[i].rpGrpMask;
    inetAddressSet(L7_AF_INET, &rpGrpMask, 
                               &pCfgV6->rtr.pimsmStaticRP[i].rpGrpMask);
  }

  /*Fill in the Static RP Info */

  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_3, 
                    L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    /*MIGRATE_COPY_STRUCT (pCfgV6->intf[i], pCfgV5->intf[i]);*/
    memcpy(&pCfgV6->intf[i] ,&pCfgV5->intf[i],sizeof(pCfgV5->intf[i]));
    pCfgV6->intf[i].intfJoinPruneInterval = pCfgV5->rtr.joinPruneInterval;
    
  }

  return L7_SUCCESS;
}


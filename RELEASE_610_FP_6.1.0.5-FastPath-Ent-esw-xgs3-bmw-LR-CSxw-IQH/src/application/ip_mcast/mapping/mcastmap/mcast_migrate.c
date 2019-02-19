
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename mcast_migrate.c
*
* @purpose multicast Configuration Migration
*
* @component multicast
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/


#include "l3_defaultconfig.h"
#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"
#include "l7_mcast_api.h"
#include "mcast_map.h"
#include "mcast_migrate.h"



static L7_RC_t mcastMigrateConfigV1V2Convert (L7_mcastMapCfgV1_t * pCfgV1,
                                              L7_mcastMapCfgV2_t * pCfgV2);

static L7_RC_t mcastMigrateConfigV2V3Convert (L7_mcastMapCfgV2_t * pCfgV2,
                                              L7_mcastMapCfgV3_t * pCfgV3);

static L7_RC_t mcastMigrateConfigV3V4Convert (L7_mcastMapCfgV3_t * pCfgV3,
                                              L7_mcastMapCfgV4_t * pCfgV4);

static L7_RC_t mcastMigrateConfigV4V5Convert (L7_mcastMapCfgV4_t * pCfgV4,
                                              L7_mcastMapCfgV5_t * pCfgV5);

static void mcastBuildDefaultConfigDataV2 (L7_mcastMapCfgV2_t * pCfgV2);
static void mcastBuildDefaultConfigDataV3 (L7_mcastMapCfgV3_t * pCfgV3);
static void mcastBuildDefaultConfigDataV4 (L7_mcastMapCfgV4_t * pCfgV4);
static void mcastBuildDefaultConfigDataV5 (L7_mcastMapCfgV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when 
 * BuildDefault called in convert.
 */
static L7_mcastMapCfg_t *pMcastCfgCur = L7_NULLPTR;


/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer       @b{(input)} version of old config definition
* @param    ver          @b{(input)} version of current config definition
* @param    pCfgBuffer   @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @comments This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
*
* @end
*********************************************************************/
void
mcastMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;

  /* declare a pointer to each supported config structure version */
  L7_mcastMapCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_mcastMapCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_mcastMapCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_mcastMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_mcastMapCfgV5_t *pCfgV5 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pMcastCfgCur = (L7_mcastMapCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pMcastCfgCur));

  if (pCfgHdr->version == L7_MCAST_CFG_VER_0 && 
      pCfgHdr->length == sizeof (L7_mcastMapCfgV1_t))
  {
    oldVer = L7_MCAST_CFG_VER_1;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
    case L7_MCAST_CFG_VER_1:
      pCfgV1 = (L7_mcastMapCfgV1_t *) 
               osapiMalloc(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                           (L7_uint32) sizeof (L7_mcastMapCfgV1_t));
      if (L7_NULLPTR == pCfgV1)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV1;
      oldCfgSize = (L7_uint32) sizeof (L7_mcastMapCfgV1_t);
      pOldCfgCksum = &pCfgV1->checkSum;
      oldCfgVer = pCfgHdr->version;
      break;

    case L7_MCAST_CFG_VER_2:
      pCfgV2 = (L7_mcastMapCfgV2_t *) 
               osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                            (L7_uint32) sizeof (L7_mcastMapCfgV2_t));
      if (L7_NULLPTR == pCfgV2)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV2;
      oldCfgSize = (L7_uint32) sizeof (L7_mcastMapCfgV2_t);
      pOldCfgCksum = &pCfgV2->checkSum;
      oldCfgVer = L7_MCAST_CFG_VER_2;
      break;

    case L7_MCAST_CFG_VER_3:
      pCfgV3 = (L7_mcastMapCfgV3_t *) 
               osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                            (L7_uint32) sizeof (L7_mcastMapCfgV3_t));
      if (L7_NULLPTR == pCfgV3)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV3;
      oldCfgSize = (L7_uint32) sizeof (L7_mcastMapCfgV3_t);
      pOldCfgCksum = &pCfgV3->checkSum;
      oldCfgVer = L7_MCAST_CFG_VER_3;
      break;

    case L7_MCAST_CFG_VER_4:
      pCfgV4 = (L7_mcastMapCfgV4_t *) osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_mcastMapCfgV4_t));
      if (L7_NULLPTR == pCfgV4)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV4;
      oldCfgSize = (L7_uint32) sizeof (L7_mcastMapCfgV4_t);
      pOldCfgCksum = &pCfgV4->checkSum;
      oldCfgVer = L7_MCAST_CFG_VER_4;
      break;

    case L7_MCAST_CFG_VER_5:
      pCfgV5 = (L7_mcastMapCfgV5_t *) osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_mcastMapCfgV5_t));
      if (L7_NULLPTR == pCfgV5)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV5;
      oldCfgSize = (L7_uint32) sizeof (L7_mcastMapCfgV5_t);
      pOldCfgCksum = &pCfgV5->checkSum;
      oldCfgVer = L7_MCAST_CFG_VER_5;
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
     *
     * The routine sysapiCfgFileGet which invokes the migrate routine will pass
     * in the a pointer to the location of the configuration data read from
     * nonvolatile storage.
     * In cases where the older version of the configuration file may be larger
     * than the current version, sysapiCfgFileGet truncates the data to the size
     * of the storage location pointed to by the buffer provided for the 
     * component.
     * Thus, it is important to read the configuration file again so that the
     * entirety of the configuration data can be accessed.
     */

    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                         L7_MCAST_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, 
                                         L7_NULL /* do not build defaults */,
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
      case L7_MCAST_CFG_VER_1:
        if (L7_NULLPTR == pCfgV1)
        {
          MIGRATE_DEBUG ("\n\rpCfgV1 Null Pointer\r\n");
          break;
        }
        /* pCfgV2 has already been allocated and filled in */
        pCfgV2 = (L7_mcastMapCfgV2_t *) osapiMalloc 
                 (L7_FLEX_MCAST_MAP_COMPONENT_ID, 
                  (L7_uint32) sizeof (L7_mcastMapCfgV2_t));
        if ((L7_NULLPTR == pCfgV2) || 
            (L7_SUCCESS != mcastMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV1);
        pCfgV1 = L7_NULLPTR;
        /*passthru */

      case L7_MCAST_CFG_VER_2:
        if (L7_NULLPTR == pCfgV2)
        {
          MIGRATE_DEBUG ("\n\rpCfgV2 Null Pointer\r\n");
          break;
        }
        /* pCfgV3 has already been allocated and filled in */
        pCfgV3 = (L7_mcastMapCfgV3_t *) 
                 osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                              (L7_uint32) sizeof (L7_mcastMapCfgV3_t));
        if ((L7_NULLPTR == pCfgV3) || 
            (L7_SUCCESS != mcastMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV2);
        pCfgV2 = L7_NULLPTR;
        /*passthru */

      case L7_MCAST_CFG_VER_3:
        /* pCfgV4 has already been allocated and filled in */
        pCfgV4 = (L7_mcastMapCfgV4_t *) osapiMalloc (L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                                    (L7_uint32) sizeof (L7_mcastMapCfgV4_t));
        if ((L7_NULLPTR == pCfgV4) || ((pCfgV3 != L7_NULLPTR) && (L7_SUCCESS != mcastMigrateConfigV3V4Convert (pCfgV3, pCfgV4))))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV3);
        pCfgV3 = L7_NULLPTR;
        /*passthru */

     case L7_MCAST_CFG_VER_4:
        /* pCfgV4 has already been allocated and filled in */
        if (L7_NULLPTR == pCfgV4) 
        {
          MIGRATE_DEBUG ("\n\r pCfgV4Null Pointer\r\n");
          break;
        }
        pCfgV5 = (L7_mcastMapCfgV5_t *)osapiMalloc(L7_FLEX_MCAST_MAP_COMPONENT_ID,
                                       (L7_uint32) sizeof (L7_mcastMapCfgV5_t));
        if ((L7_NULLPTR == pCfgV5)||(L7_SUCCESS != 
             mcastMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV4);
        pCfgV4 = L7_NULLPTR;
        /*passthru*/

     case L7_MCAST_CFG_VER_5:
        if (L7_NULLPTR == pCfgV5)
        {
          MIGRATE_DEBUG ("\n\r pCfgV5Null Pointer\r\n");
          break;
        }

        memcpy ((L7_uchar8 *) pMcastCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pMcastCfgCur));
        osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV5);
        pCfgV5 = L7_NULLPTR;

        /* done with migration - Flag unsaved configuration */
        pMcastCfgCur->cfgHdr.dataChanged = L7_TRUE;
        break;

      default:
        buildDefault = L7_TRUE;
        break;
    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV4);  
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_FLEX_MCAST_MAP_COMPONENT_ID, pCfgV5); 

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pMcastCfgCur, 0, sizeof (*pMcastCfgCur));
    mcastMapBuildDefaultConfigData (L7_MCAST_CFG_VER_CURRENT);
    pMcastCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}


/*********************************************************************
* @purpose  Converts the interface config data structure from version V2 to V3.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
* @param    intfType  @b{(input)} interface types being migrated
* @param    offset    @b{(input)} current offset to version 2 
*                                 config data structure
*
* @returns  void
*
* @comments   
*
* @end
*********************************************************************/

static void
mcastMigrateIntfConfigV1V2Convert (L7_mcastMapCfgV1_t * pCfgV1, 
                                   L7_mcastMapCfgV2_t * pCfgV2,
                                   L7_uint32 intfType, L7_uint32 * offset)
{
  L7_uint32 i;
  L7_uint32 rc;
  L7_uint32 count, intCfgIndex;
  nimConfigID_t configId;


  /* Note: The methodology used by the policyMigrate* routines in 
   *       migrating different 
   *       types  of interfaces is more mature than the multiple invocations
   *       of the same routine implemented for this routine as fewer loop checks
   *       are required. However, this works, so we go with it.
   */ 

  /* Note: The truncated flag merely indicates if the migration was 
   *       truncated within
   *       this invocation of this routine.
   */ 


  if (mcastIntfIsValidType (intfType) != L7_TRUE)
    return;

  intCfgIndex = *offset;
  count = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT_REL_4_0);
  for (i = 1; i < count; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    if (configId.type != intfType)
      continue;

    if (intCfgIndex >= L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (L7_MCAST_CFG_FILENAME, intCfgIndex);
      break;
    }


    (void) nimConfigIdCopy (&configId, &pCfgV2->intf[intCfgIndex].configId);
    pCfgV2->intf[intCfgIndex].ipMRouteIfTtlThresh 
    = pCfgV1->intf[i].ipMRouteIfTtlThresh;
    intCfgIndex++;
  }
  *offset = intCfgIndex;
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
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
mcastMigrateConfigV1V2Convert (L7_mcastMapCfgV1_t * pCfgV1, L7_mcastMapCfgV2_t * pCfgV2)
{
  L7_uint32 i;
  L7_uint32 rc;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 intIfNum;
  L7_uint32 intCfgIndex;
  nimConfigID_t configId;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_MCAST_CFG_VER_1 && 
      pCfgV1->cfgHdr.version != L7_MCAST_CFG_VER_0)
  /* b'cos REL_G gives wrong version */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_MCAST_CFG_VER_1);
    return L7_FAILURE;
  }

  mcastBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate 
   * fields in the newer one.
   */
  pCfgV2->rtr.mcastAdminMode = pCfgV1->rtr.mcastAdminMode;
  pCfgV2->rtr.currentMcastProtocol = pCfgV1->rtr.currentMcastProtocol;
  pCfgV2->rtr.numStaticMRouteEntries = pCfgV1->rtr.numStaticMRouteEntries;
  pCfgV2->rtr.numAdminScopeEntries = pCfgV1->rtr.numAdminScopeEntries;

  for (i = 0; i < L7_RTR_MAX_STATIC_MROUTES_REL_4_0; i++)
  {
    intIfNum = pCfgV1->rtr.mcastStaticRtsCfgData[i].intfNum;
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (intIfNum, 
                                                &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    (void) nimConfigIdCopy (&configId, 
                            &pCfgV2->rtr.mcastStaticRtsCfgData[i].ifConfigId);
    inetAddressSet(L7_AF_INET, &(pCfgV1->rtr.mcastStaticRtsCfgData[i].source), 
                   &(pCfgV2->rtr.mcastStaticRtsCfgData[i].source));
    inetAddressSet(L7_AF_INET, &(pCfgV1->rtr.mcastStaticRtsCfgData[i].mask), 
                   &(pCfgV2->rtr.mcastStaticRtsCfgData[i].mask));
    inetAddressSet(L7_AF_INET, &(pCfgV1->rtr.mcastStaticRtsCfgData[i].rpfAddr), 
                   &(pCfgV2->rtr.mcastStaticRtsCfgData[i].rpfAddr));
    pCfgV2->rtr.mcastStaticRtsCfgData[i].preference 
    = pCfgV1->rtr.mcastStaticRtsCfgData[i].preference;

  }

  for (i = 0; i < L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_0; i++)
  {
    if (pCfgV1->rtr.mcastAdminScopeCfgData[i].inUse == L7_FALSE)
      continue;

    slot = pCfgV1->rtr.mcastAdminScopeCfgData[i].ipUSP.usp.slot;
    port = pCfgV1->rtr.mcastAdminScopeCfgData[i].ipUSP.usp.port;
    rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, 
                                                   port, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    if (mcastIntfIsValidType (configId.type) != L7_TRUE)
      continue;

    (void) nimConfigIdCopy (&configId, 
                            &pCfgV2->rtr.mcastAdminScopeCfgData[i].ifConfigId);
    pCfgV2->rtr.mcastAdminScopeCfgData[i].inUse 
    = pCfgV1->rtr.mcastAdminScopeCfgData[i].inUse;
    inetAddressSet(L7_AF_INET, 
                   &(pCfgV1->rtr.mcastAdminScopeCfgData[i].groupIpAddr),
                   &(pCfgV2->rtr.mcastAdminScopeCfgData[i].groupIpAddr));
    inetAddressSet(L7_AF_INET, 
                   &(pCfgV1->rtr.mcastAdminScopeCfgData[i].groupIpMask),
                   &(pCfgV2->rtr.mcastAdminScopeCfgData[i].groupIpMask));
  }

  intCfgIndex = 1;
  mcastMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, 
                                     L7_PHYSICAL_INTF, &intCfgIndex);

  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
    mcastMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, 
                                       L7_LOGICAL_VLAN_INTF, &intCfgIndex);

  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
    mcastMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, 
                                       L7_LAG_INTF, &intCfgIndex);

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
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
mcastMigrateConfigV2V3Convert (L7_mcastMapCfgV2_t * pCfgV2, L7_mcastMapCfgV3_t * pCfgV3)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_MCAST_CFG_VER_2 && 
      pCfgV2->cfgHdr.version != L7_MCAST_CFG_VER_0)
  /* b'cos REL_G gives wrong version */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_MCAST_CFG_VER_2);
    return L7_FAILURE;
  }

  mcastBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate 
   * fields in the newer one.
   */
  MIGRATE_COPY_STRUCT (pCfgV3->rtr, pCfgV2->rtr);

  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_1, 
                    L7_IPMAP_INTF_MAX_COUNT_REL_4_3);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->intf[i], pCfgV2->intf[i]);
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
mcastMigrateConfigV3V4Convert (L7_mcastMapCfgV3_t * pCfgV3, L7_mcastMapCfgV4_t * pCfgV4)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_MCAST_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != L7_MCAST_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != L7_MCAST_CFG_VER_0)     /* b'cos REL_G gives wrong version */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_MCAST_CFG_VER_3);
    return L7_FAILURE;
  }

  mcastBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV4->rtr, pCfgV3->rtr);

  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_3, L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->intf[i], pCfgV3->intf[i]);
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
mcastBuildDefaultConfigDataV2 (L7_mcastMapCfgV2_t * pCfgV2)
{
  L7_uint32 itr;
  L7_uint32 defSrc, defMask, defRpf;
  L7_uint32 defAdminScpIp, defAdminScpMask;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  pCfgV2->rtr.mcastAdminMode = FD_MCAST_DEFAULT_ADMIN_MODE;
  pCfgV2->rtr.currentMcastProtocol = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;
  pCfgV2->rtr.currentMcastV6Protocol = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;

  for (itr = 0; itr < L7_RTR_MAX_STATIC_MROUTES_REL_4_1; itr++)
  {
    defSrc = FD_MCAST_DEFAULT_STATIC_SRC;
    defMask = FD_MCAST_DEFAULT_STATIC_SRC_MASK;
    defRpf = FD_MCAST_DEFAULT_STATIC_RPFADDR;
    inetAddressSet(L7_AF_INET, &defSrc, 
                   &(pCfgV2->rtr.mcastStaticRtsCfgData[itr].source));
    inetAddressSet(L7_AF_INET, &defMask, 
                   &(pCfgV2->rtr.mcastStaticRtsCfgData[itr].mask));
    inetAddressSet(L7_AF_INET, &defRpf, 
                   &(pCfgV2->rtr.mcastStaticRtsCfgData[itr].rpfAddr));
    pCfgV2->rtr.mcastStaticRtsCfgData[itr].preference 
    = FD_MCAST_DEFAULT_STATIC_METRIC;
  }

  for (itr = 0; itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_1; itr++)
  {
    defAdminScpIp = FD_MCAST_DEFAULT_ADMINSCOPE_IPADDR;
    defAdminScpMask = FD_MCAST_DEFAULT_ADMINSCOPE_MASK;
    pCfgV2->rtr.mcastAdminScopeCfgData[itr].inUse 
    = FD_MCAST_DEFAULT_ADMINSCOPE_INUSE;
    inetAddressSet(L7_AF_INET, &defAdminScpIp, 
                   &(pCfgV2->rtr.mcastAdminScopeCfgData[itr].groupIpAddr));
    inetAddressSet(L7_AF_INET, &defAdminScpMask, 
                   &(pCfgV2->rtr.mcastAdminScopeCfgData[itr].groupIpMask));
  }

  pCfgV2->rtr.numStaticMRouteEntries = FD_MCAST_DEFAULT_NUM_STATIC_ENTRIES;
  pCfgV2->rtr.numAdminScopeEntries = FD_MCAST_DEFAULT_NUM_ADMIN_SCOPE_ENTRIES;

  /**********************************
   * Building interface config data *
   **********************************/
  for (itr = 1; itr < L7_IPMAP_INTF_MAX_COUNT_REL_4_1; itr++)
  {
    pCfgV2->intf[itr].ipMRouteIfTtlThresh = FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD;
  }

  strcpy(pCfgV2->cfgHdr.filename, L7_MCAST_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_MCAST_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
  pCfgV2->checkSum = 0;
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
mcastBuildDefaultConfigDataV3 (L7_mcastMapCfgV3_t * pCfgV3)
{
  L7_uint32 itr;
  L7_uint32 defSrc, defMask, defRpf;
  L7_uint32 defAdminScpIp, defAdminScpMask;

  memset (pCfgV3, 0, sizeof (*pCfgV3));

  pCfgV3->rtr.mcastAdminMode = FD_MCAST_DEFAULT_ADMIN_MODE;
  pCfgV3->rtr.currentMcastProtocol = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;

  for (itr = 0; itr < L7_RTR_MAX_STATIC_MROUTES_REL_4_3; itr++)
  {
    defSrc = FD_MCAST_DEFAULT_STATIC_SRC;
    defMask = FD_MCAST_DEFAULT_STATIC_SRC_MASK;
    defRpf = FD_MCAST_DEFAULT_STATIC_RPFADDR;
    inetAddressSet(L7_AF_INET, &defSrc, 
                   &(pCfgV3->rtr.mcastStaticRtsCfgData[itr].source));
    inetAddressSet(L7_AF_INET, &defMask, 
                   &(pCfgV3->rtr.mcastStaticRtsCfgData[itr].mask));
    inetAddressSet(L7_AF_INET, &defRpf, 
                   &(pCfgV3->rtr.mcastStaticRtsCfgData[itr].rpfAddr));
    pCfgV3->rtr.mcastStaticRtsCfgData[itr].preference = FD_MCAST_DEFAULT_STATIC_METRIC;
  }

  for (itr = 0; itr < L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_3; itr++)
  {
    defAdminScpIp = FD_MCAST_DEFAULT_ADMINSCOPE_IPADDR;
    defAdminScpMask = FD_MCAST_DEFAULT_ADMINSCOPE_MASK;
    
    pCfgV3->rtr.mcastAdminScopeCfgData[itr].inUse = FD_MCAST_DEFAULT_ADMINSCOPE_INUSE;
    inetAddressSet(L7_AF_INET, &defAdminScpIp, 
                   &(pCfgV3->rtr.mcastAdminScopeCfgData[itr].groupIpAddr));
    inetAddressSet(L7_AF_INET, &defAdminScpMask, 
                   &(pCfgV3->rtr.mcastAdminScopeCfgData[itr].groupIpMask));
  }

  pCfgV3->rtr.numStaticMRouteEntries = FD_MCAST_DEFAULT_NUM_STATIC_ENTRIES;
  pCfgV3->rtr.numAdminScopeEntries = FD_MCAST_DEFAULT_NUM_ADMIN_SCOPE_ENTRIES;

  /**********************************
   * Building interface config data *
   **********************************/
  for (itr = 1; itr < L7_IPMAP_INTF_MAX_COUNT_REL_4_3; itr++)
  {
    pCfgV3->intf[itr].ipMRouteIfTtlThresh = FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD;
  }

  strcpy (pCfgV3->cfgHdr.filename, L7_MCAST_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_MCAST_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;
  pCfgV3->checkSum = 0;
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
mcastBuildDefaultConfigDataV4 (L7_mcastMapCfgV4_t * pCfgV4)
{
  L7_uint32 i;

  memset(pCfgV4, 0, sizeof(* pCfgV4));

  pCfgV4->rtr.currentMcastProtocol = FD_MCAST_DEFAULT_ROUTING_PROTOCOL;
  pCfgV4->rtr.mcastAdminMode =  FD_MCAST_DEFAULT_ADMIN_MODE;
  pCfgV4->rtr.numAdminScopeEntries = 0;
  pCfgV4->rtr.numStaticMRouteEntries = 0;

  /* All the static and admin scope boundary entries are already reset,
     which is default values */

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_4; i++)
  {
    L7_mcastMapIfCfgData_t *intfCfg = &pCfgV4->intf[i];

    memset(&intfCfg->configId, 0, sizeof(intfCfg->configId)) ;
    intfCfg->ipMRouteIfTtlThresh = FD_MCAST_INTF_DEFAULT_TTL_THRESHOLD;
  }

  strcpy (pCfgV4->cfgHdr.filename, L7_MCAST_CFG_FILENAME);
  pCfgV4->cfgHdr.version = L7_MCAST_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_FLEX_MCAST_MAP_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = sizeof (*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;

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
mcastBuildDefaultConfigDataV5 (L7_mcastMapCfgV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pMcastCfgCur, 0, sizeof (L7_mcastMapCfgV5_t));
  mcastMapBuildDefaultConfigData (L7_MCAST_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pMcastCfgCur, sizeof (*pMcastCfgCur));
}
/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5.
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV5   @b{(input)} ptr to version 5 config data structure
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
mcastMigrateConfigV4V5Convert (L7_mcastMapCfgV4_t * pCfgV4, L7_mcastMapCfgV5_t * pCfgV5)
{
  L7_uint32 i, intfCount, numStaticRoutes, numAdminScopes;
  L7_uint32 srcAddr, srcMask, rpfAddr, grpAddr, grpMask;

  /* verify correct version of old config file */
  if ((pCfgV4->cfgHdr.version != L7_MCAST_CFG_VER_4) &&
      (pCfgV4->cfgHdr.version != L7_MCAST_CFG_VER_3) &&
      (pCfgV4->cfgHdr.version != L7_MCAST_CFG_VER_2)) 
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_MCAST_CFG_VER_4);
    return L7_FAILURE;
  }

  mcastBuildDefaultConfigDataV5 (pCfgV5);
  /* Map the fields in the older structure to the appropriate fields in the 
   * newer one 
   */

  pCfgV5->rtr.mcastAdminMode = pCfgV4->rtr.mcastAdminMode;
  pCfgV5->rtr.currentMcastProtocol = pCfgV4->rtr.currentMcastProtocol;
  pCfgV5->rtr.numAdminScopeEntries = pCfgV4->rtr.numAdminScopeEntries;
  pCfgV5->rtr.numStaticMRouteEntries = pCfgV4->rtr.numStaticMRouteEntries;

  /* Map all static route data */
  numStaticRoutes = min (L7_RTR_MAX_STATIC_MROUTES_REL_4_0,  L7_RTR_MAX_STATIC_MROUTES);

  for (i= 0; i < numStaticRoutes; i++)
  {
    srcAddr = pCfgV4->rtr.mcastStaticRtsCfgData[i].source;
    inetAddressSet(L7_AF_INET, &srcAddr, 
                               &pCfgV5->rtr.mcastStaticRtsCfgData[i].source);
    srcMask = pCfgV4->rtr.mcastStaticRtsCfgData[i].mask;
    inetAddressSet(L7_AF_INET, &srcMask, 
                               &pCfgV5->rtr.mcastStaticRtsCfgData[i].mask);
    rpfAddr = pCfgV4->rtr.mcastStaticRtsCfgData[i].rpfAddr;
    inetAddressSet(L7_AF_INET, &rpfAddr, 
                               &pCfgV5->rtr.mcastStaticRtsCfgData[i].rpfAddr);
    pCfgV5->rtr.mcastStaticRtsCfgData[i].ifConfigId = 
               pCfgV4->rtr.mcastStaticRtsCfgData[i].ifConfigId ;
    pCfgV5->rtr.mcastStaticRtsCfgData[i].preference = 
               pCfgV4->rtr.mcastStaticRtsCfgData[i].preference;
  }

  /* Map all admin scope data */
  numAdminScopes = min (L7_MCAST_MAX_ADMINSCOPE_ENTRIES_REL_4_0,  L7_MCAST_MAX_ADMINSCOPE_ENTRIES);

  for (i= 0; i < numStaticRoutes; i++)
  {
    grpAddr = pCfgV4->rtr.mcastAdminScopeCfgData[i].groupIpAddr;
    inetAddressSet(L7_AF_INET, &grpAddr, 
                               &pCfgV5->rtr.mcastAdminScopeCfgData[i].groupIpAddr);
    grpMask = pCfgV4->rtr.mcastAdminScopeCfgData[i].groupIpMask;
    inetAddressSet(L7_AF_INET, &grpMask, 
                               &pCfgV5->rtr.mcastAdminScopeCfgData[i].groupIpMask);
    pCfgV5->rtr.mcastAdminScopeCfgData[i].ifConfigId = 
               pCfgV4->rtr.mcastAdminScopeCfgData[i].ifConfigId ;
    pCfgV5->rtr.mcastAdminScopeCfgData[i].inUse = 
               pCfgV4->rtr.mcastAdminScopeCfgData[i].inUse;
  }

  /* Map all inetrface configuration */
  intfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_4, L7_IPMAP_INTF_MAX_COUNT);

  for (i = 0; i < intfCount; i++)
  {
    pCfgV5->intf[i].configId = pCfgV4->intf[i].configId;
    pCfgV5->intf[i].ipMRouteIfTtlThresh = pCfgV4->intf[i].ipMRouteIfTtlThresh;
  }

  return L7_SUCCESS;
}


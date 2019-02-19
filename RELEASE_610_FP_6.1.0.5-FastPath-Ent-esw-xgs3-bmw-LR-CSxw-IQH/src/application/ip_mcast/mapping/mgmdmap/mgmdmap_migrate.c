
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename mgmd_migrate.c
*
* @purpose mgmd Configuration Migration
*
* @component mgmd
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "l7_mgmdmap_include.h"
#include "mgmdmap_migrate.h"

static L7_RC_t mgmdMigrateConfigV1V2Convert (L7_mgmdMapCfgV1_t * pCfgV1,
                                             L7_mgmdMapCfgV2_t * pCfgV2);

static L7_RC_t mgmdMigrateConfigV2V3Convert (L7_mgmdMapCfgV2_t * pCfgV2,
                                             L7_mgmdMapCfgV3_t * pCfgV3);

static L7_RC_t mgmdMigrateConfigV3V4Convert (L7_mgmdMapCfgV3_t * pCfgV3,
                                             L7_mgmdMapCfgV4_t * pCfgV4);

static L7_RC_t mgmdMigrateConfigV4V5Convert (L7_mgmdMapCfgV4_t * pCfgV4,
                                             L7_mgmdMapCfgV5_t * pCfgV5);

static void mgmdBuildDefaultConfigDataV2 (L7_mgmdMapCfgV2_t * pCfgV2);
static void mgmdBuildDefaultConfigDataV3 (L7_mgmdMapCfgV3_t * pCfgV3);
static void mgmdBuildDefaultConfigDataV4 (L7_mgmdMapCfgV4_t * pCfgV4);
static void mgmdBuildDefaultConfigDataV5 (L7_mgmdMapCfgV5_t * pCfgV5);

static L7_mgmdMapCfg_t *pCfgCur = L7_NULLPTR;

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
mgmdMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;

  /* declare a pointer to each supported config structure version */
  L7_mgmdMapCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_mgmdMapCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_mgmdMapCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_mgmdMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_mgmdMapCfgV5_t *pCfgV5 = L7_NULLPTR;
  
  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = L7_NULL;
  oldCfgVer     = L7_NULL;
  
  pCfgCur = (L7_mgmdMapCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));


  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_MGMD_CFG_VER_1:
    pCfgV1 = (L7_mgmdMapCfgV1_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                    (L7_uint32) sizeof (L7_mgmdMapCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_mgmdMapCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = L7_MGMD_CFG_VER_1;
    break;

  case L7_MGMD_CFG_VER_2:
    pCfgV2 = (L7_mgmdMapCfgV2_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_mgmdMapCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_mgmdMapCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_MGMD_CFG_VER_2;
    break;

  case L7_MGMD_CFG_VER_3:
    pCfgV3 = (L7_mgmdMapCfgV3_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                       (L7_uint32) sizeof (L7_mgmdMapCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_mgmdMapCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_MGMD_CFG_VER_3;
    break;

  case L7_MGMD_CFG_VER_4:
    pCfgV4 = (L7_mgmdMapCfgV4_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_mgmdMapCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_mgmdMapCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_MGMD_CFG_VER_4;
    break;

  case L7_MGMD_CFG_VER_5:
    pCfgV5 = (L7_mgmdMapCfgV5_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_mgmdMapCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (L7_mgmdMapCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = L7_MGMD_CFG_VER_5;
    break;

  default:
    buildDefault = L7_TRUE;
    oldCfgSize = L7_NULL;
    pOldCfgCksum = L7_NULL;
    oldCfgVer = L7_NULL;
    break;

  }                             /* endswitch */

  if (buildDefault != L7_TRUE)
  {
    /* read the old config file found on this system */

    /* Note:
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                               L7_IGMP_CFG_FILENAME,
                               pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                               oldCfgVer, L7_NULL /* do not build defaults */,
                               L7_NULL /* do not call migrate again */)))
    {
      MIGRATE_DEBUG ("sysapiCfgFileGet failed size = %d version = %d\n", 
                     (L7_int32) oldCfgSize, (L7_int32) oldCfgVer);
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
    case L7_MGMD_CFG_VER_1:
      if (L7_NULLPTR == pCfgV1)
      {
        MIGRATE_DEBUG ("\n\rpCfgV1 Null Pointer\r\n");
        break;
      }
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_mgmdMapCfgV2_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                      (L7_uint32) sizeof (L7_mgmdMapCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || 
         (L7_SUCCESS != mgmdMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_MGMD_CFG_VER_2:
      if (L7_NULLPTR == pCfgV2)
      {
        MIGRATE_DEBUG ("\n\rpCfgV2 Null Pointer\r\n");
        break;
      }
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_mgmdMapCfgV3_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                    (L7_uint32) sizeof (L7_mgmdMapCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || 
          (L7_SUCCESS != mgmdMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_MGMD_CFG_VER_3:
      if (L7_NULLPTR == pCfgV3)
      {
        MIGRATE_DEBUG ("\n\rpCfgV3 Null Pointer\r\n");
        break;
      }
      pCfgV4 = (L7_mgmdMapCfgV4_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_mgmdMapCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || 
          (L7_SUCCESS != mgmdMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_MGMD_CFG_VER_4:
      if (L7_NULLPTR == pCfgV4)
      {
        MIGRATE_DEBUG ("\n\rpCfgV4 Null Pointer\r\n");
        break;
      }
      pCfgV5 = (L7_mgmdMapCfgV5_t *) osapiMalloc (L7_FLEX_MGMD_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_mgmdMapCfgV5_t));
      if ((L7_NULLPTR == pCfgV5) || 
          (L7_SUCCESS != mgmdMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */
      
    case L7_MGMD_CFG_VER_5:
      if (L7_NULLPTR == pCfgV5)
      {
        MIGRATE_DEBUG ("\n\rpCfgV5 Null Pointer\r\n");
        break;
      }
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV5);
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
  {
    osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV1);
  }
  if (pCfgV2 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV2);
  }
  if (pCfgV3 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV3);
  }
  if (pCfgV4 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV4);
  }
  if (pCfgV5 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_MGMD_MAP_COMPONENT_ID, pCfgV5);
  }


  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    mgmdBuildDefaultConfigData (L7_MGMD_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the interface config data structure from version V1 to V2.
*
* @param    pCfgV1     @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2     @b{(input)} ptr to version 2 config data structure
* @param    intfType   @b{(input)} interface types being migrated
* @param    offset     @b{(input)} current offset to version 2 config data 
*                      structure
*
* @returns  void
*
* @comments none
*          
*
* @end
*********************************************************************/
static void
mgmdMigrateIntfConfigV1V2Convert (L7_mgmdMapCfgV1_t * pCfgV1, 
 L7_mgmdMapCfgV2_t * pCfgV2,  L7_uint32 intfType, L7_uint32 * offset)
{
  L7_RC_t rc;
  L7_int32 i, j;
  L7_uint32 intCfgIndex;
  nimConfigID_t configId;

  L7_BOOL truncated;

           
  /* Note: The methodology used by the policyMigrate routines in migrating 
   *       different types  of interfaces is more mature than the multiple 
   *       invocations of the same routine implemented for this routine as 
   *       fewer loop checks are required. However, this works, so we go 
   *       with it.
   */ 
  /* Note: The truncated flag merely indicates if the migration was truncated 
   * within this invocation of this routine.
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
        MIGRATE_LOG_INTERFACE_TRUNCATE (L7_IGMP_CFG_FILENAME, intCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void)nimConfigIdCopy(&configId, &pCfgV2->mgmdIntf[intCfgIndex].configId);
      pCfgV2->mgmdIntf[intCfgIndex].adminMode = 
          pCfgV1->mgmdIntf[i][j].adminMode;
      pCfgV2->mgmdIntf[intCfgIndex].queryInterval = 
          pCfgV1->mgmdIntf[i][j].queryInterval;
      pCfgV2->mgmdIntf[intCfgIndex].version = pCfgV1->mgmdIntf[i][j].version;
      pCfgV2->mgmdIntf[intCfgIndex].responseInterval = 
          pCfgV1->mgmdIntf[i][j].responseInterval;
      pCfgV2->mgmdIntf[intCfgIndex].robustness = 
          pCfgV1->mgmdIntf[i][j].robustness;
      pCfgV2->mgmdIntf[intCfgIndex].startupQueryInterval =
          pCfgV1->mgmdIntf[i][j].startupQueryInterval;
      pCfgV2->mgmdIntf[intCfgIndex].startupQueryCount = 
          pCfgV1->mgmdIntf[i][j].startupQueryCount;
      pCfgV2->mgmdIntf[intCfgIndex].lastMemQueryInterval =
          pCfgV1->mgmdIntf[i][j].lastMemQueryInterval;
      pCfgV2->mgmdIntf[intCfgIndex].lastMemQueryCount = 
          pCfgV1->mgmdIntf[i][j].lastMemQueryCount;
      pCfgV2->mgmdIntf[intCfgIndex].entryStatus = 
          pCfgV1->mgmdIntf[i][j].entryStatus;
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
* @purpose  Converts the config data structure from version V1 to V2.
*
* @param    pCfgV1     @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2     @b{(input)} ptr to version 2 config data structure
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
mgmdMigrateConfigV1V2Convert (L7_mgmdMapCfgV1_t * pCfgV1, 
                              L7_mgmdMapCfgV2_t * pCfgV2)
{
  L7_uint32 intCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_MGMD_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_MGMD_CFG_VER_1);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Incorrect version of old config file\n");
    return L7_FAILURE;
  }

  mgmdBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields 
   * in the newer one 
   */
  pCfgV2->adminMode = pCfgV1->adminMode;
  pCfgV2->traceMode = pCfgV1->traceMode;

  intCfgIndex = 1;

  mgmdMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_PHYSICAL_INTF, 
                                    &intCfgIndex);

  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
  {
     mgmdMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_LOGICAL_VLAN_INTF, 
                                       &intCfgIndex);
  }

  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
  {  mgmdMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_LAG_INTF, 
                                       &intCfgIndex);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2     @b{(input)} ptr to version 2 config data structure
* @param    pCfgV3     @b{(input)} ptr to version 3 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t mgmdMigrateConfigV2V3Convert (L7_mgmdMapCfgV2_t * pCfgV2, 
                                             L7_mgmdMapCfgV3_t * pCfgV3)
{
  L7_uint32 i, intIfCount;
  nimConfigID_t nullConfigId;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_MGMD_CFG_VER_2 && 
      pCfgV2->cfgHdr.version != L7_MGMD_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_MGMD_CFG_VER_2);
    MGMD_MAP_DEBUG (MGMD_MAP_DEBUG_FAILURES, "Incorrect version of old config file\n");
    return L7_FAILURE;
  }

  mgmdBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields 
   * in the newer one 
   */
  pCfgV3->adminMode = pCfgV2->adminMode;

  memset ((void *) &nullConfigId, 0x00, (L7_uint32) sizeof (nimConfigID_t));
  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_2, 
                    L7_IPMAP_INTF_MAX_COUNT_REL_4_3);
  for (i = 0; i < intIfCount; i++)
  {
    /* Don't copy null configIDs */
    if (NIM_CONFIG_ID_IS_EQUAL (&pCfgV2->mgmdIntf[i].configId, &nullConfigId) )
    {
      continue;
    }


    MIGRATE_COPY_STRUCT (pCfgV3->mgmdIntf[i], pCfgV2->mgmdIntf[i]);
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
mgmdMigrateConfigV3V4Convert (L7_mgmdMapCfgV3_t * pCfgV3, L7_mgmdMapCfgV4_t * pCfgV4)
{
  L7_uint32 i, intIfCount;
  nimConfigID_t nullConfigId;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_MGMD_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != L7_MGMD_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != L7_MGMD_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_MGMD_CFG_VER_3);
    return L7_FAILURE;
  }

  mgmdBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->adminMode = pCfgV3->adminMode;

  memset ((void *) &nullConfigId, 0x00, (L7_uint32) sizeof (nimConfigID_t));
  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_3, L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < intIfCount; i++)
  {
    /* Don't copy null configIDs */
    if (NIM_CONFIG_ID_IS_EQUAL (&pCfgV3->mgmdIntf[i].configId, &nullConfigId) )
      continue;

    /*MIGRATE_COPY_STRUCT (pCfgV4->mgmdIntf[i], pCfgV3->mgmdIntf[i]);*/
    (void) nimConfigIdCopy (&pCfgV3->mgmdIntf[i].configId, &pCfgV4->mgmdIntf[i].configId);
    pCfgV4->mgmdIntf[i].adminMode            = pCfgV3->mgmdIntf[i].adminMode ;
    pCfgV4->mgmdIntf[i].queryInterval        = pCfgV3->mgmdIntf[i].queryInterval ;
    pCfgV4->mgmdIntf[i].version              = pCfgV3->mgmdIntf[i].version ;
    pCfgV4->mgmdIntf[i].responseInterval     = pCfgV3->mgmdIntf[i].responseInterval ;
    pCfgV4->mgmdIntf[i].robustness           = pCfgV3->mgmdIntf[i].robustness ;
    pCfgV4->mgmdIntf[i].startupQueryInterval = pCfgV3->mgmdIntf[i].startupQueryInterval ;
    pCfgV4->mgmdIntf[i].startupQueryCount    = pCfgV3->mgmdIntf[i].startupQueryCount ;
    pCfgV4->mgmdIntf[i].lastMemQueryInterval = pCfgV3->mgmdIntf[i].lastMemQueryInterval ;
    pCfgV4->mgmdIntf[i].lastMemQueryCount    = pCfgV3->mgmdIntf[i].lastMemQueryCount ;
    pCfgV4->mgmdIntf[i].entryStatus          = pCfgV3->mgmdIntf[i].entryStatus ;
    pCfgV4->mgmdIntf[i].interfaceMode        = MGMD_ROUTER_INTERFACE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Build Version 2 defaults
*
* @param    pCfgV2     @b{(input)} ptr to version 2 config data structure
*
* @returns  none
*
* @comments none
*
* @end
*********************************************************************/
static void
mgmdBuildDefaultConfigDataV2 (L7_mgmdMapCfgV2_t * pCfgV2)
{
  L7_uint32 i;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  pCfgV2->adminMode = FD_IGMP_DEFAULT_ADMIN_MODE;

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_2; i++)
  {
    mgmdIntfCfgData_t *pCfg = &pCfgV2->mgmdIntf[i];

    pCfg->adminMode = L7_DISABLE;
    pCfg->queryInterval = FD_IGMP_QUERY_INTERVAL;
    pCfg->version = FD_IGMP_DEFAULT_VER;
    pCfg->responseInterval = FD_IGMP_QUERY_RESPONSE_INTERVAL;
    pCfg->robustness = FD_IGMP_ROBUST_VARIABLE;
    pCfg->startupQueryInterval = FD_IGMP_STARTUP_QUERY_INTERVAL;
    pCfg->startupQueryCount = FD_IGMP_STARTUP_QUERY_COUNT;
    pCfg->lastMemQueryInterval = FD_IGMP_LAST_MEMBER_QUERY_INTERVAL;
    pCfg->lastMemQueryCount = FD_IGMP_LAST_MEMBER_QUERY_COUNT;
    pCfg->entryStatus = FD_IGMP_ENTRY_STATUS;
  }

  strcpy (pCfgV2->cfgHdr.filename, L7_IGMP_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_MGMD_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_FLEX_MGMD_MAP_COMPONENT_ID;
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
mgmdBuildDefaultConfigDataV3 (L7_mgmdMapCfgV3_t * pCfgV3)
{
  L7_uint32 i;

  memset (pCfgV3, 0, sizeof (*pCfgV3));

  pCfgV3->adminMode = FD_IGMP_DEFAULT_ADMIN_MODE;
  pCfgV3->traceMode = FD_IGMP_DEFAULT_TRACE_MODE;

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_3; i++)
  {
    mgmdIntfCfgDataV3_t *pCfg = &pCfgV3->mgmdIntf[i];

    pCfg->adminMode = L7_DISABLE;
    pCfg->queryInterval = FD_IGMP_QUERY_INTERVAL;
    pCfg->version = FD_IGMP_DEFAULT_VER;
    pCfg->responseInterval = FD_IGMP_QUERY_RESPONSE_INTERVAL;
    pCfg->robustness = FD_IGMP_ROBUST_VARIABLE;
    pCfg->startupQueryInterval = FD_IGMP_STARTUP_QUERY_INTERVAL;
    pCfg->startupQueryCount = FD_IGMP_STARTUP_QUERY_COUNT;
    pCfg->lastMemQueryInterval = FD_IGMP_LAST_MEMBER_QUERY_INTERVAL;
    pCfg->lastMemQueryCount = FD_IGMP_LAST_MEMBER_QUERY_COUNT;
    pCfg->entryStatus = FD_IGMP_ENTRY_STATUS;
  }

  strcpy (pCfgV3->cfgHdr.filename, L7_IGMP_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_MGMD_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_FLEX_MGMD_MAP_COMPONENT_ID;
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
mgmdBuildDefaultConfigDataV4 (L7_mgmdMapCfgV4_t * pCfgV4)
{
  L7_uint32 i;

  memset(pCfgV4, 0, sizeof(*pCfgV4));

  pCfgV4->adminMode = FD_IGMP_DEFAULT_ADMIN_MODE;

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_4; i++)
  {
    mgmdIntfCfgData_t *intfCfg = &pCfgV4->mgmdIntf[i];

    intfCfg->adminMode            = L7_DISABLE;
    intfCfg->queryInterval        = FD_IGMP_QUERY_INTERVAL;
    intfCfg->version              = FD_IGMP_DEFAULT_VER;
    intfCfg->robustness           = FD_IGMP_ROBUST_VARIABLE;
    intfCfg->startupQueryInterval = FD_IGMP_STARTUP_QUERY_INTERVAL;
    intfCfg->startupQueryCount    = FD_IGMP_STARTUP_QUERY_COUNT;
    intfCfg->lastMemQueryCount    = FD_IGMP_LAST_MEMBER_QUERY_COUNT;
    intfCfg->entryStatus          = FD_IGMP_ENTRY_STATUS;
    intfCfg->unsolicitedReportInterval = FD_IGMP_DEFAULT_UNSOLICITED_REPORT_INTERVAL ;
    intfCfg->interfaceMode        = MGMD_INTERFACE_MODE_NULL;
    intfCfg->lastMemQueryInterval = FD_IGMP_LAST_MEMBER_QUERY_INTERVAL;
    intfCfg->responseInterval     = FD_IGMP_QUERY_RESPONSE_INTERVAL;
  }

  strcpy(pCfgV4->cfgHdr.filename, L7_IGMP_CFG_FILENAME);
  pCfgV4->cfgHdr.version       = L7_MGMD_CFG_VER_4;
  pCfgV4->cfgHdr.componentID   = L7_FLEX_MGMD_MAP_COMPONENT_ID;
  pCfgV4->cfgHdr.type          = L7_CFG_DATA;
  pCfgV4->cfgHdr.length        = sizeof(L7_mgmdMapCfg_t);
  pCfgV4->cfgHdr.dataChanged   = L7_FALSE;

}
/*********************************************************************
* @purpose  Build Version 5defaults
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
mgmdBuildDefaultConfigDataV5 (L7_mgmdMapCfgV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (L7_mgmdMapCfgV5_t));
  mgmdBuildDefaultConfigData (L7_MGMD_CFG_VER_5);
  memcpy (pCfgV5, pCfgCur, sizeof (*pCfgCur));
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
mgmdMigrateConfigV4V5Convert (L7_mgmdMapCfgV4_t * pCfgV4, L7_mgmdMapCfgV5_t * pCfgV5)
{
  L7_uint32 i, intfCount;

  /* verify correct version of old config file */
  if ((pCfgV4->cfgHdr.version != L7_MGMD_CFG_VER_4) &&
      (pCfgV4->cfgHdr.version != L7_MGMD_CFG_VER_3) &&
      (pCfgV4->cfgHdr.version != L7_MGMD_CFG_VER_2)) 
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_MGMD_CFG_VER_4);
    MGMD_MAP_DEBUG(MGMD_MAP_DEBUG_FAILURES,"Incorrect version of config file");
    return L7_FAILURE;
  }
  
  mgmdBuildDefaultConfigDataV5 (pCfgV5);
  /* Map the fields in the older structure to the appropriate fields in the 
   * newer one 
   */
  pCfgV5->adminMode = pCfgV4->adminMode;

  intfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_4, 
                    L7_IPMAP_INTF_MAX_COUNT);
  for (i = 0; i < intfCount; i++)
  {
    memcpy(&pCfgV5->mgmdIntf[i], &pCfgV4->mgmdIntf[i], sizeof(pCfgV4->mgmdIntf[i]));
  }
  return L7_SUCCESS;
}


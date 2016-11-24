/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dvmrp_migrate.c
*
* @purpose dmprp Configuration Migration
*
* @component dmprp
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/
#include "l7_common_l3.h"
#include "l3_mcast_commdefs.h"
#include "l7_ip_api.h"                   
#include "l3_mcast_defaultconfig.h"
#include "l3_mcast_default_cnfgr.h"
#include "dvmrp_cnfgr.h" 
#include "dvmrp_config.h" 
#include "dvmrp_map.h" 
#include "dvmrp_migrate.h" 
#include "dvmrp_map_debug.h"

static L7_RC_t dvmrpMigrateConfigV1V2Convert (L7_dvmrpMapCfgV1_t * pCfgV1,
                                              L7_dvmrpMapCfgV2_t * pCfgV2);

static L7_RC_t dvmrpMigrateConfigV2V3Convert (L7_dvmrpMapCfgV2_t * pCfgV2,
                                              L7_dvmrpMapCfgV3_t * pCfgV3);

static L7_RC_t dvmrpMigrateConfigV3V4Convert (L7_dvmrpMapCfgV3_t * pCfgV3,
                                              L7_dvmrpMapCfgV4_t * pCfgV4);

static L7_RC_t dvmrpMigrateConfigV4V5Convert (L7_dvmrpMapCfgV4_t * pCfgV4,
                                              L7_dvmrpMapCfgV5_t * pCfgV5);

static void dvmrpBuildDefaultConfigDataV2 (L7_dvmrpMapCfgV2_t * pCfgV2);
static void dvmrpBuildDefaultConfigDataV3 (L7_dvmrpMapCfgV3_t * pCfgV3);
static void dvmrpBuildDefaultConfigDataV4 (L7_dvmrpMapCfgV4_t * pCfgV4);
static void dvmrpBuildDefaultConfigDataV5 (L7_dvmrpMapCfgV5_t * pCfgV5);

static L7_dvmrpMapCfg_t *pCfgCur = L7_NULLPTR;

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer   @b{(input)} ptr to location of read configuration
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
dvmrpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;

  /* declare a pointer to each supported config structure version */
  L7_dvmrpMapCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_dvmrpMapCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_dvmrpMapCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_dvmrpMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_dvmrpMapCfgV5_t *pCfgV5 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pCfgCur = (L7_dvmrpMapCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
    case L7_DVMRP_CFG_VER_1:
      pCfgV1 = (L7_dvmrpMapCfgV1_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dvmrpMapCfgV1_t));
      if (L7_NULLPTR == pCfgV1)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV1;
      oldCfgSize = (L7_uint32) sizeof (L7_dvmrpMapCfgV1_t);
      pOldCfgCksum = &pCfgV1->checkSum;
      oldCfgVer = L7_DVMRP_CFG_VER_1;
      break;

    case L7_DVMRP_CFG_VER_2:
      pCfgV2 = (L7_dvmrpMapCfgV2_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dvmrpMapCfgV2_t));
      if (L7_NULLPTR == pCfgV2)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV2;
      oldCfgSize = (L7_uint32) sizeof (L7_dvmrpMapCfgV2_t);
      pOldCfgCksum = &pCfgV2->checkSum;
      oldCfgVer = L7_DVMRP_CFG_VER_2;
      break;

    case L7_DVMRP_CFG_VER_3:
      pCfgV3 = (L7_dvmrpMapCfgV3_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dvmrpMapCfgV3_t));
      if (L7_NULLPTR == pCfgV3)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV3;
      oldCfgSize = (L7_uint32) sizeof (L7_dvmrpMapCfgV3_t);
      pOldCfgCksum = &pCfgV3->checkSum;
      oldCfgVer = L7_DVMRP_CFG_VER_3;
      break;

    case L7_DVMRP_CFG_VER_4:
      pCfgV4 = (L7_dvmrpMapCfgV4_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dvmrpMapCfgV4_t));
      if (L7_NULLPTR == pCfgV4)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV4;
      oldCfgSize = (L7_uint32) sizeof (L7_dvmrpMapCfgV4_t);
      pOldCfgCksum = &pCfgV4->checkSum;
      oldCfgVer = L7_DVMRP_CFG_VER_4;
      break;

    case L7_DVMRP_CFG_VER_5:
      pCfgV5 = (L7_dvmrpMapCfgV5_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dvmrpMapCfgV5_t));
      if (L7_NULLPTR == pCfgV5)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV5;
      oldCfgSize = (L7_uint32) sizeof (L7_dvmrpMapCfgV5_t);
      pOldCfgCksum = &pCfgV5->checkSum;
      oldCfgVer = L7_DVMRP_CFG_VER_5;
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
     * The routine sysapiCfgFileGet which invokes the migrate routine will pass
     * in the a pointer to the location of the configuration data read from
     * nonvolatile storage.
     * In cases where the older version of the configuration file may be larger
     * than the current version, sysapiCfgFileGet truncates the data to the size
     * of the storage location pointed to by the buffer provided for the component.
     * Thus, it is important to read the configuration file again so that the entirety
     * of the configuration data can be accessed.
     */

    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                         L7_DVMRP_CFG_FILENAME, pOldCfgBuf,
                                         oldCfgSize, pOldCfgCksum, oldCfgVer,
                                         L7_NULL /* do not build defaults */ ,
                                         L7_NULL /* do not call migrate again */)))
    {
      MIGRATE_DEBUG ("sysapiCfgFileGet failed size = %d version = %d\n", (L7_int32) oldCfgSize,
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
      case L7_DVMRP_CFG_VER_1:
        /* pCfgV1 has already been allocated and filled in */
        if (L7_NULLPTR == pCfgV1)
        {
          MIGRATE_DEBUG ("\n\rpCfgV1 Null Pointer\r\n");
          break;
        }
        pCfgV2 = (L7_dvmrpMapCfgV2_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                     (L7_uint32) sizeof (L7_dvmrpMapCfgV2_t));
        if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != dvmrpMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV1);
        pCfgV1 = L7_NULLPTR;
        /*passthru */

      case L7_DVMRP_CFG_VER_2:
        if (L7_NULLPTR == pCfgV2)
        {
          MIGRATE_DEBUG ("\n\rpCfgV2 Null Pointer\r\n");
          break;
        }
        /* pCfgV2 has already been allocated and filled in */
        pCfgV3 = (L7_dvmrpMapCfgV3_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                     (L7_uint32) sizeof (L7_dvmrpMapCfgV3_t));
        if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != dvmrpMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV2);
        pCfgV2 = L7_NULLPTR;
        /*passthru */

      case L7_DVMRP_CFG_VER_3:
        if (L7_NULLPTR == pCfgV3)
        {
          MIGRATE_DEBUG ("\n\rpCfgV3 Null Pointer\r\n");
          break;
        }
        /* pCfgV3 has already been allocated and filled in */
        pCfgV4 = (L7_dvmrpMapCfgV4_t *) osapiMalloc (L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                     (L7_uint32) sizeof (L7_dvmrpMapCfgV4_t));
        if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != dvmrpMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV3);
        pCfgV3 = L7_NULLPTR;
        /*passthru */

      case L7_DVMRP_CFG_VER_4:
        if (L7_NULLPTR == pCfgV4)
        {
          MIGRATE_DEBUG ("\n\rpCfgV4 Null Pointer\r\n");
          break;
        }
        /* pCfgV4 has already been allocated and filled in */
        pCfgV5 = (L7_dvmrpMapCfgV5_t *)osapiMalloc(L7_FLEX_DVMRP_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dvmrpMapCfgV5_t));
        if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != 
                                       dvmrpMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV4);
        pCfgV4 = L7_NULLPTR;
        /*passthru */

      case L7_DVMRP_CFG_VER_5:
        if (L7_NULLPTR == pCfgV5)
        {
          MIGRATE_DEBUG ("\n\rpCfgV5 Null Pointer\r\n");
          break;
        }

        /* pCfgV5 has already been allocated and filled in */
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
        osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV5);
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
    osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV1);
  }
  if (pCfgV2 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV2);
  }
  if (pCfgV3 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV3);
  }
  if (pCfgV4 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV4);
  }
  if (pCfgV5 != L7_NULLPTR)
  {
    osapiFree (L7_FLEX_DVMRP_MAP_COMPONENT_ID, pCfgV5);
  }

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dvmrpBuildDefaultConfigData (L7_DVMRP_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}


/*********************************************************************
* @purpose  Converts the interface config data structure from version V2 to V3.
*
* @param    pCfgV1     @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2     @b{(input)} ptr to version 2 config data structure
* @param    intfType  @b{(input)} interface types being migrated
* @param    offset     @b{(input)} current offset to version 2 config data structure
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
dvmrpMigrateCktConfigV1V2Convert (L7_dvmrpMapCfgV1_t * pCfgV1, L7_dvmrpMapCfgV2_t * pCfgV2,
                                  L7_uint32 intfType, L7_uint32 * offset)
{
  L7_RC_t rc;
  L7_uint32 i, j, intCfgIndex;
  nimConfigID_t configId;
  L7_BOOL truncated;


  /* Note: The methodology used by the policyMigrate* routines in migrating different 
   *       types  of interfaces is more mature than the multiple invocations
   *       of the same routine implemented for this routine as fewer loop checks
   *       are required. However, this works, so we go with it.
   */ 

  /* Note: The truncated flag merely indicates if the migration was truncated within
   *       this invocation of this routine.
   */

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
        MIGRATE_LOG_INTERFACE_TRUNCATE (L7_DVMRP_CFG_FILENAME, intCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV2->ckt[intCfgIndex].configId);
      pCfgV2->ckt[intCfgIndex].intfAdminMode = pCfgV1->ckt[i][j].intfAdminMode;
      pCfgV2->ckt[intCfgIndex].intfMetric = pCfgV1->ckt[i][j].intfMetric;
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
* @param    pCfgV2         @b{(input)} ptr to version 2 config data structure
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
dvmrpMigrateConfigV1V2Convert (L7_dvmrpMapCfgV1_t * pCfgV1, 
                               L7_dvmrpMapCfgV2_t * pCfgV2)
{
  L7_uint32 intCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_DVMRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_DVMRP_CFG_VER_1);
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Incorrect version of old config file\n");
    return L7_FAILURE;
  }

  dvmrpBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV2->rtr, pCfgV1->rtr);

  intCfgIndex = 1;
  dvmrpMigrateCktConfigV1V2Convert (pCfgV1, pCfgV2, L7_PHYSICAL_INTF, &intCfgIndex);
  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
  {
    dvmrpMigrateCktConfigV1V2Convert (pCfgV1, pCfgV2, L7_LOGICAL_VLAN_INTF, 
                                      &intCfgIndex);
  }
  if (intCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
  {
    dvmrpMigrateCktConfigV1V2Convert (pCfgV1, pCfgV2, L7_LAG_INTF,&intCfgIndex);
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
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t 
dvmrpMigrateConfigV2V3Convert (L7_dvmrpMapCfgV2_t * pCfgV2, 
                               L7_dvmrpMapCfgV3_t * pCfgV3)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_DVMRP_CFG_VER_2 && 
      pCfgV2->cfgHdr.version != L7_DVMRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_DVMRP_CFG_VER_2);
    DVMRP_MAP_DEBUG(DVMRP_MAP_DEBUG_FAILURES, "Incorrect version of old config file\n");
    return L7_FAILURE;
  }

  dvmrpBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV3->rtr, pCfgV2->rtr);
  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_1, 
                    L7_IPMAP_INTF_MAX_COUNT_REL_4_3);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->ckt[i], pCfgV2->ckt[i]);
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
dvmrpMigrateConfigV3V4Convert (L7_dvmrpMapCfgV3_t * pCfgV3, L7_dvmrpMapCfgV4_t * pCfgV4)
{
  L7_uint32 i, intIfCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_DVMRP_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != L7_DVMRP_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != L7_DVMRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_DVMRP_CFG_VER_3);
    return L7_FAILURE;
  }

  dvmrpBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV4->rtr, pCfgV3->rtr);
  intIfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_3, L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->ckt[i], pCfgV3->ckt[i]);
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
* @notes
*
* @end
*********************************************************************/
static void
dvmrpBuildDefaultConfigDataV2 (L7_dvmrpMapCfgV2_t * pCfgV2)
{
  L7_uint32 i;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  /* generic DVMRP cfg */
  pCfgV2->rtr.adminMode = FD_DVMRP_DEFAULT_ADMIN_MODE;

  /* DVMRP router interface configuration parameters */
  /* we are preserving any previous configId fields in the table */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_1; i++)
  {
    dvmrpCfgCkt_t *pCfg = &pCfgV2->ckt[i];

    pCfg->intfAdminMode = FD_DVMRP_INTF_DEFAULT_ADMIN_MODE;
    pCfg->intfMetric = FD_DVMRP_INTF_DEFAULT_METRIC;
  }


  osapiStrncpy(pCfgV2->cfgHdr.filename,L7_DVMRP_CFG_FILENAME,L7_MAX_FILENAME);
  pCfgV2->cfgHdr.version = L7_DVMRP_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_FLEX_DVMRP_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
  pCfgV2->checkSum = 0;
}

/*********************************************************************
* @purpose  Build Version 3 defaults
*
* @param    pCfgV3     @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
static void
dvmrpBuildDefaultConfigDataV3 (L7_dvmrpMapCfgV3_t * pCfgV3)
{
  L7_uint32 i;

  memset (pCfgV3, 0, sizeof (*pCfgV3));

  /* generic DVMRP cfg */
  pCfgV3->rtr.adminMode = FD_DVMRP_DEFAULT_ADMIN_MODE;

  /* DVMRP router interface configuration parameters */
  /* we are preserving any previous configId fields in the table */
  for (i = 1; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_3; i++)
  {
    dvmrpCfgCkt_t *pCfg = &pCfgV3->ckt[i];

    pCfg->intfAdminMode = FD_DVMRP_INTF_DEFAULT_ADMIN_MODE;
    pCfg->intfMetric = FD_DVMRP_INTF_DEFAULT_METRIC;
  }

  strcpy (pCfgV3->cfgHdr.filename, L7_DVMRP_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_DVMRP_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_FLEX_DVMRP_MAP_COMPONENT_ID;
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
dvmrpBuildDefaultConfigDataV4 (L7_dvmrpMapCfgV4_t * pCfgV4)
{
  L7_uint32 i;

  memset(pCfgV4, 0, sizeof(L7_dvmrpMapCfgV4_t));

  pCfgV4->rtr.adminMode = FD_DVMRP_DEFAULT_ADMIN_MODE;

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_4; i++)
  {
    pCfgV4->ckt[i].intfAdminMode =  FD_DVMRP_INTF_DEFAULT_ADMIN_MODE;
    pCfgV4->ckt[i].intfMetric = FD_DVMRP_INTF_DEFAULT_METRIC;
  }

  strcpy(pCfgV4->cfgHdr.filename, L7_DVMRP_CFG_FILENAME);
  pCfgV4->cfgHdr.version       = L7_DVMRP_CFG_VER_4;
  pCfgV4->cfgHdr.componentID   = L7_FLEX_DVMRP_MAP_COMPONENT_ID;
  pCfgV4->cfgHdr.type          = L7_CFG_DATA;
  pCfgV4->cfgHdr.length        = sizeof(L7_dvmrpMapCfg_t);
  pCfgV4->cfgHdr.dataChanged   = L7_FALSE;

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
dvmrpBuildDefaultConfigDataV5 (L7_dvmrpMapCfgV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (L7_dvmrpMapCfgV5_t));
  dvmrpBuildDefaultConfigData (L7_DVMRP_CFG_VER_5);
  memcpy (pCfgV5, pCfgCur, sizeof (*pCfgCur));
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
dvmrpMigrateConfigV4V5Convert (L7_dvmrpMapCfgV4_t * pCfgV4, L7_dvmrpMapCfgV5_t * pCfgV5)
{
  L7_uint32 i, intfCount;

  /* verify correct version of old config file */
  if ((pCfgV4->cfgHdr.version != L7_DVMRP_CFG_VER_4) &&
      (pCfgV4->cfgHdr.version != L7_DVMRP_CFG_VER_3) &&
      (pCfgV4->cfgHdr.version != L7_DVMRP_CFG_VER_2))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_DVMRP_CFG_VER_4);
    return L7_FAILURE;
  }

  dvmrpBuildDefaultConfigDataV5(pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the 
   * newer one 
   */

  pCfgV5->rtr.adminMode = pCfgV4->rtr.adminMode;

  intfCount = min (L7_IPMAP_INTF_MAX_COUNT_REL_4_4, 
                   L7_IPMAP_INTF_MAX_COUNT);

  for (i = 0; i < intfCount; i++)
  {
    memcpy(&pCfgV5->ckt[i], &pCfgV4->ckt[i], sizeof(pCfgV4->ckt[i]));
  }
  return L7_SUCCESS;
}

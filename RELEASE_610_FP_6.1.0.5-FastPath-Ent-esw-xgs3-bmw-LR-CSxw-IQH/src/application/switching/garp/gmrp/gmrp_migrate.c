
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    gmrp_migrate.c
* @purpose     GMRP Configuration Migration
* @component   GMRP-GARP
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @end
*             
**********************************************************************/

#include "gmrp_migrate.h"

static L7_RC_t gmrpMigrateConfigV1V2Convert (gmrpCfgV1_t * pCfgV1, gmrpCfgV2_t * pCfgV2);
static L7_RC_t gmrpMigrateConfigV2V3Convert (gmrpCfgV2_t * pCfgV2, gmrpCfgV3_t * pCfgV3);
static L7_RC_t gmrpMigrateConfigV3V4Convert (gmrpCfgV3_t * pCfgV3, gmrpCfgV4_t * pCfgV4);
static void gmrpBuildDefaultConfigDataV2 (gmrpCfgV2_t * pCfgV2);
static void gmrpBuildDefaultConfigDataV3 (gmrpCfgV3_t * pCfgV3);
static void gmrpBuildDefaultConfigDataV4 (gmrpCfgV4_t * pCfgV4);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static gmrpCfg_t *pCfgCur = L7_NULLPTR;

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
gmrpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  gmrpCfgV1_t *pCfgV1 = L7_NULLPTR;
  gmrpCfgV2_t *pCfgV2 = L7_NULLPTR;
  gmrpCfgV3_t *pCfgV3 = L7_NULLPTR;
  gmrpCfgV4_t *pCfgV4 = L7_NULLPTR;

  ver = ver;
  pCfgCur = (gmrpCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == GMRP_CFG_VER_1 && pCfgHdr->length == sizeof (gmrpCfgV2_t))
  {
    oldVer = GMRP_CFG_VER_2;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case GMRP_CFG_VER_1:
    pCfgV1 = (gmrpCfgV1_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (gmrpCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = GMRP_CFG_VER_1;
    break;

  case GMRP_CFG_VER_2:
    pCfgV2 = (gmrpCfgV2_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (gmrpCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case GMRP_CFG_VER_3:
    pCfgV3 = (gmrpCfgV3_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (gmrpCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case GMRP_CFG_VER_4:
    pCfgV4 = (gmrpCfgV4_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (gmrpCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = pCfgHdr->version;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_GMRP_COMPONENT_ID,
                                         GMRP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GMRP_COMPONENT_ID,
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
    case GMRP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (gmrpCfgV2_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != gmrpMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_GMRP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case GMRP_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (gmrpCfgV3_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != gmrpMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_GMRP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case GMRP_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (gmrpCfgV4_t *) osapiMalloc (L7_GMRP_COMPONENT_ID, (L7_uint32) sizeof (gmrpCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != gmrpMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_GMRP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case GMRP_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgCur));
      osapiFree (L7_GMRP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->hdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_GMRP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_GMRP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_GMRP_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_GMRP_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    gmrpBuildDefaultConfigData (GMRP_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
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
gmrpMigrateIntfConfigV1V2Convert (gmrpCfgV1_t * pCfgV1, gmrpCfgV2_t * pCfgV2,
                                  L7_uint32 intfType, L7_uint32 * offsetp)
{
  L7_RC_t rc;
  L7_uint32 i, intfCfgIndex;
  nimConfigID_t configId;


  /* Note: The methodology used by the policyMigrate* routines in migrating different 
           types  of interfaces is more mature than the multiple invocations
           of the same routine implemented for this routine as fewer loop checks
           are required. However, this works, so we go with it. */ 

  if (gmrpIsValidIntfType (intfType) != L7_TRUE)
    return;

  pCfgV1 = pCfgV1;
  intfCfgIndex = *offsetp;
  for (i = 1; i < L7_MAX_INTERFACE_COUNT_REL_4_0; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (configId.type != intfType)
      continue;

    if (intfCfgIndex >= L7_GARP_MAX_CFG_IDX_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (GMRP_CFG_FILENAME, intfCfgIndex);
      break;
    }

    (void) nimConfigIdCopy (&configId, &pCfgV2->intf[intfCfgIndex].configId);
    intfCfgIndex++;
  }

  *offsetp = intfCfgIndex;
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
gmrpMigrateConfigV1V2Convert (gmrpCfgV1_t * pCfgV1, gmrpCfgV2_t * pCfgV2)
{
  L7_uint32 i, count, intfCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV1->hdr.version != GMRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->hdr.version, GMRP_CFG_VER_1);
    return L7_FAILURE;
  }

  gmrpBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_1,
               L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_0);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV2->cfg.staticEntry[i].vlanIdmacAddress,
                         pCfgV1->cfg.staticEntry[i].vlanIdmacAddress);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->cfg.staticEntry[i].fixedReg,
                                pCfgV1->cfg.staticEntry[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->cfg.staticEntry[i].forbiddenReg,
                                pCfgV1->cfg.staticEntry[i].forbiddenReg);
    pCfgV2->cfg.staticEntry[i].inUse = pCfgV1->cfg.staticEntry[i].inUse;
  }

  count = min (L7_MAX_VLANS_REL_4_1, L7_MAX_VLANS_REL_4_0);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->cfg.staticForwardAll[i].fixedReg,
                                pCfgV1->cfg.staticForwardAll[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->cfg.staticForwardAll[i].forbiddenReg,
                                pCfgV1->cfg.staticForwardAll[i].forbiddenReg);
    pCfgV2->cfg.staticForwardAll[i].vlanId = pCfgV1->cfg.staticForwardAll[i].vlanId;
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->cfg.staticForwardUnreg[i].fixedReg,
                                pCfgV1->cfg.staticForwardUnreg[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->cfg.staticForwardUnreg[i].forbiddenReg,
                                pCfgV1->cfg.staticForwardUnreg[i].forbiddenReg);
    pCfgV2->cfg.staticForwardUnreg[i].vlanId = pCfgV1->cfg.staticForwardUnreg[i].vlanId;
  }

  count = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT_REL_4_0);
  for (i = 0; i <= count; i++)
  {
    pCfgV2->cfg.gmrpEnabled[i] = pCfgV1->cfg.gmrpEnabled[i];
  }

  intfCfgIndex = 1;
  gmrpMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_PHYSICAL_INTF, &intfCfgIndex);

  if (intfCfgIndex < L7_MAX_INTERFACE_COUNT_REL_4_1)
      gmrpMigrateIntfConfigV1V2Convert (pCfgV1, pCfgV2, L7_LAG_INTF, &intfCfgIndex);

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
gmrpMigrateConfigV2V3Convert (gmrpCfgV2_t * pCfgV2, gmrpCfgV3_t * pCfgV3)
{
  L7_uint32 i;
  L7_uint32 count;

  /* verify correct version of old config file */
  if (pCfgV2->hdr.version != GMRP_CFG_VER_2 && pCfgV2->hdr.version != GMRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, GMRP_CFG_VER_2);
    return L7_FAILURE;
  }

  gmrpBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_3, L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_1);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->cfg.staticEntry[i].vlanIdmacAddress,
                         pCfgV2->cfg.staticEntry[i].vlanIdmacAddress);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->cfg.staticEntry[i].fixedReg,
                                pCfgV2->cfg.staticEntry[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->cfg.staticEntry[i].forbiddenReg,
                                pCfgV2->cfg.staticEntry[i].forbiddenReg);
    pCfgV3->cfg.staticEntry[i].inUse = pCfgV2->cfg.staticEntry[i].inUse;
  }

  count = min (L7_MAX_VLANS_REL_4_3, L7_MAX_VLANS_REL_4_1);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->cfg.staticForwardAll[i].fixedReg,
                                pCfgV2->cfg.staticForwardAll[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->cfg.staticForwardAll[i].forbiddenReg,
                                pCfgV2->cfg.staticForwardAll[i].forbiddenReg);
    pCfgV3->cfg.staticForwardAll[i].vlanId = pCfgV2->cfg.staticForwardAll[i].vlanId;
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->cfg.staticForwardUnreg[i].fixedReg,
                                pCfgV2->cfg.staticForwardUnreg[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->cfg.staticForwardUnreg[i].forbiddenReg,
                                pCfgV2->cfg.staticForwardUnreg[i].forbiddenReg);
    pCfgV3->cfg.staticForwardUnreg[i].vlanId = pCfgV2->cfg.staticForwardUnreg[i].vlanId;
  }

  count = min (L7_MAX_INTERFACE_COUNT_REL_4_3, L7_MAX_INTERFACE_COUNT_REL_4_1);
  for (i = 0; i <= count; i++)
  {
    pCfgV3->cfg.gmrpEnabled[i] = pCfgV2->cfg.gmrpEnabled[i];
  }

  count = min (L7_GARP_MAX_CFG_IDX_REL_4_3, L7_GARP_MAX_CFG_IDX_REL_4_1);
  for (i = 1; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->intf[i], pCfgV2->intf[i]);
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
gmrpMigrateConfigV3V4Convert (gmrpCfgV3_t * pCfgV3, gmrpCfgV4_t * pCfgV4)
{
  L7_uint32 i;
  L7_uint32 count;

  /* verify correct version of old config file */
  if (pCfgV3->hdr.version != GMRP_CFG_VER_3 &&
      pCfgV3->hdr.version != GMRP_CFG_VER_2 &&
      pCfgV3->hdr.version != GMRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->hdr.version, GMRP_CFG_VER_3);
    return L7_FAILURE;
  }

  gmrpBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_4, L7_MAX_GROUP_REGISTRATION_ENTRIES_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->cfg.staticEntry[i].vlanIdmacAddress,
                         pCfgV3->cfg.staticEntry[i].vlanIdmacAddress);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV4->cfg.staticEntry[i].fixedReg,
                                pCfgV3->cfg.staticEntry[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV4->cfg.staticEntry[i].forbiddenReg,
                                pCfgV3->cfg.staticEntry[i].forbiddenReg);
    pCfgV4->cfg.staticEntry[i].inUse = pCfgV3->cfg.staticEntry[i].inUse;
  }

  count = min (L7_MAX_VLANS_REL_4_4, L7_MAX_VLANS_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV4->cfg.staticForwardAll[i].fixedReg,
                                pCfgV3->cfg.staticForwardAll[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV4->cfg.staticForwardAll[i].forbiddenReg,
                                pCfgV3->cfg.staticForwardAll[i].forbiddenReg);
    pCfgV4->cfg.staticForwardAll[i].vlanId = pCfgV3->cfg.staticForwardAll[i].vlanId;
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV4->cfg.staticForwardUnreg[i].fixedReg,
                                pCfgV3->cfg.staticForwardUnreg[i].fixedReg);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV4->cfg.staticForwardUnreg[i].forbiddenReg,
                                pCfgV3->cfg.staticForwardUnreg[i].forbiddenReg);
    pCfgV4->cfg.staticForwardUnreg[i].vlanId = pCfgV3->cfg.staticForwardUnreg[i].vlanId;
  }

  count = min (L7_MAX_INTERFACE_COUNT_REL_4_4, L7_MAX_INTERFACE_COUNT_REL_4_3);
  for (i = 0; i <= count; i++)
  {
    pCfgV4->cfg.gmrpEnabled[i] = pCfgV3->cfg.gmrpEnabled[i];
  }

  count = min (L7_GARP_MAX_CFG_IDX_REL_4_4, L7_GARP_MAX_CFG_IDX_REL_4_3);
  for (i = 1; i < count; i++)
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
gmrpBuildDefaultConfigDataV2 (gmrpCfgV2_t * pCfgV2)
{
  L7_uint32 i;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  pCfgV2->hdr.version = GMRP_CFG_VER_2;
  pCfgV2->hdr.componentID = L7_GMRP_COMPONENT_ID;
  pCfgV2->hdr.type = L7_CFG_DATA;
  pCfgV2->hdr.length = (L7_uint32) sizeof (*pCfgV2);
  strcpy ((char *) pCfgV2->hdr.filename, GMRP_CFG_FILENAME);
  pCfgV2->hdr.dataChanged = L7_FALSE;

  for (i = 0; i <= L7_MAX_INTERFACE_COUNT_REL_4_1; i++)
    pCfgV2->cfg.gmrpEnabled[i] = L7_FALSE;
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
gmrpBuildDefaultConfigDataV3 (gmrpCfgV3_t * pCfgV3)
{
  L7_uint32 i;
  
  memset((void*)pCfgV3, 0, sizeof(*pCfgV3));
  
  /* setup file header */
  pCfgV3->hdr.version = GMRP_CFG_VER_3;
  pCfgV3->hdr.componentID = L7_GMRP_COMPONENT_ID;
  pCfgV3->hdr.type = L7_CFG_DATA;
  pCfgV3->hdr.length = (L7_uint32)sizeof(*pCfgV3);
  strcpy((char *)pCfgV3->hdr.filename, GMRP_CFG_FILENAME);
  pCfgV3->hdr.dataChanged = L7_FALSE;

  for (i = 0; i <= L7_MAX_INTERFACE_COUNT_REL_4_3; i++)
    pCfgV3->cfg.gmrpEnabled[i] = L7_FALSE;
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
gmrpBuildDefaultConfigDataV4 (gmrpCfgV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (gmrpCfgV4_t));
  gmrpBuildDefaultConfigData (GMRP_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

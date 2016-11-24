
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename snooping_migrate.c
*
* @purpose  Snooping Configuration Migration
*
* @component  Snooping
*
* @comments none
*
* @create 08/23/2004
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "datatypes.h"
#include "comm_mask.h"

#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "snooping_migrate.h"
#include "snooping_outcalls.h"

static L7_RC_t snoopMigrateConfigV1V2Convert (snoopCfgDataV1_t * pCfgV1, snoopCfgDataV2_t * pCfgV2);
static L7_RC_t snoopMigrateConfigV2V3Convert (snoopCfgDataV2_t * pCfgV2, snoopCfgDataV3_t * pCfgV3);
static L7_RC_t snoopMigrateConfigV3V4Convert (snoopCfgDataV3_t * pCfgV3, snoopCfgDataV4_t * pCfgV4);
static L7_RC_t snoopMigrateConfigV4V5Convert (snoopCfgDataV4_t * pCfgV4, snoopCfgDataV5_t * pCfgV5);
static void snoopBuildDefaultConfigDataV2 (snoopCfgDataV2_t * pCfgV2);
static void snoopBuildDefaultConfigDataV3 (snoopCfgDataV3_t * pCfgV3);
static void snoopBuildDefaultConfigDataV4 (snoopCfgDataV4_t * pCfgV4);
static void snoopBuildDefaultConfigDataV5 (snoopCfgDataV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static snoopCfgData_t *pCfgCur = L7_NULLPTR;

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
void snoopMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  snoopCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  snoopCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  snoopCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  snoopCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  snoopCfgDataV5_t *pCfgV5 = L7_NULLPTR;

  pCfgCur = (snoopCfgData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == SNOOP_IGMP_CFG_VER_1 && pCfgHdr->length == sizeof (snoopCfgDataV2_t))
    oldVer = SNOOP_IGMP_CFG_VER_2;

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case SNOOP_IGMP_CFG_VER_1:
    pCfgV1 = (snoopCfgDataV1_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                               (L7_uint32) sizeof (snoopCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (snoopCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = SNOOP_IGMP_CFG_VER_1;
    break;

  case SNOOP_IGMP_CFG_VER_2:
    pCfgV2 = (snoopCfgDataV2_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                               (L7_uint32) sizeof (snoopCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (snoopCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = SNOOP_IGMP_CFG_VER_2;
    break;

  case SNOOP_IGMP_CFG_VER_3:
    pCfgV3 = (snoopCfgDataV3_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                               (L7_uint32) sizeof (snoopCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (snoopCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = SNOOP_IGMP_CFG_VER_3;
    break;

  case SNOOP_IGMP_CFG_VER_4:
    pCfgV4 = (snoopCfgDataV4_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                               (L7_uint32) sizeof (snoopCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (snoopCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = SNOOP_IGMP_CFG_VER_4;
    break;

  case SNOOP_IGMP_CFG_VER_5:
    pCfgV5 = (snoopCfgDataV5_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                               (L7_uint32) sizeof (snoopCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (snoopCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = SNOOP_IGMP_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_SNOOPING_COMPONENT_ID,
                                         SNOOP_IGMP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SNOOPING_COMPONENT_ID,
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
    case SNOOP_IGMP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 =
        (snoopCfgDataV2_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                          (L7_uint32) sizeof (snoopCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) ||
          (L7_SUCCESS != snoopMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case SNOOP_IGMP_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 =
        (snoopCfgDataV3_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                          (L7_uint32) sizeof (snoopCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || 
          (L7_SUCCESS != snoopMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case SNOOP_IGMP_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 =
        (snoopCfgDataV4_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                          (L7_uint32) sizeof (snoopCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) || 
          (L7_SUCCESS != snoopMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case SNOOP_IGMP_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 =
        (snoopCfgDataV5_t *) osapiMalloc (L7_SNOOPING_COMPONENT_ID,
                                          (L7_uint32) sizeof (snoopCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) || 
          (L7_SUCCESS != snoopMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case SNOOP_IGMP_CFG_VER_5:
      /* pCfgV4 has already been allocated and filled in */
      if (L7_NULLPTR == pCfgV5)
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV5);
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
    osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_SNOOPING_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    snoopIGMPBuildDefaultConfigData (SNOOP_IGMP_CFG_VER_CURRENT);
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
static L7_RC_t snoopMigrateConfigV1V2Convert(snoopCfgDataV1_t *pCfgV1, snoopCfgDataV2_t *pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_int32 intfCfgIndex;
  nimConfigID_t configId;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != SNOOP_IGMP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, SNOOP_IGMP_CFG_VER_1);
    return L7_FAILURE;
  }

  snoopBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->snoopAdminMode = pCfgV1->snoopAdminMode;
  pCfgV2->groupMembershipInterval = pCfgV1->groupMembershipInterval;
  pCfgV2->responseTime = pCfgV1->responseTime;
  pCfgV2->mcastRtrExpiryTime = pCfgV1->mcastRtrExpiryTime;

  intfCfgIndex = 1;
  for (i = 0; i < L7_IGMP_SNOOPING_MAX_INTF_REL_4_0; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (snoopIsValidIntfType (configId.type) != L7_TRUE)
      continue;

    if (intfCfgIndex >= L7_IGMP_SNOOPING_MAX_INTF_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE ("snoopCfgData.cfg", intfCfgIndex);
      break;
    }

    (void) nimConfigIdCopy (&configId, &pCfgV2->snoopIntfCfgData[intfCfgIndex].configId);
    intfCfgIndex++;
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
static L7_RC_t snoopMigrateConfigV2V3Convert(snoopCfgDataV2_t *pCfgV2, snoopCfgDataV3_t *pCfgV3)
{
  L7_uint32 i;
  L7_uint32 igmpIfCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != SNOOP_IGMP_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, SNOOP_IGMP_CFG_VER_2);
    return L7_FAILURE;
  }

  snoopBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->snoopAdminMode = pCfgV2->snoopAdminMode;

  igmpIfCount = min (L7_IGMP_SNOOPING_MAX_INTF_REL_4_1, L7_IGMP_SNOOPING_MAX_INTF_REL_4_3);
  for (i = 1; i < igmpIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->snoopIntfCfgData[i].configId,
                         pCfgV2->snoopIntfCfgData[i].configId);
    pCfgV3->snoopIntfCfgData[i].intfMode = pCfgV2->snoopIntfCfgData[i].intfMode;
    pCfgV3->snoopIntfCfgData[i].groupMembershipInterval = pCfgV2->groupMembershipInterval;
    pCfgV3->snoopIntfCfgData[i].responseTime = pCfgV2->responseTime;
    pCfgV3->snoopIntfCfgData[i].mcastRtrExpiryTime = pCfgV2->mcastRtrExpiryTime;
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
static L7_RC_t snoopMigrateConfigV3V4Convert(snoopCfgDataV3_t *pCfgV3, snoopCfgDataV4_t *pCfgV4)
{
  L7_uint32 i;
  L7_uint32 igmpIfCount;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != SNOOP_IGMP_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != SNOOP_IGMP_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, SNOOP_IGMP_CFG_VER_3);
    return L7_FAILURE;
  }

  snoopBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->snoopAdminMode = pCfgV3->snoopAdminMode;

  igmpIfCount = min (L7_IGMP_SNOOPING_MAX_INTF_REL_4_3, L7_IGMP_SNOOPING_MAX_INTF_REL_4_4);
  for (i = 1; i < igmpIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->snoopIntfCfgData[i],
                         pCfgV3->snoopIntfCfgData[i]);
  }

  MIGRATE_COPY_STRUCT (pCfgV4->vlanGroupMembershipInterval,
                       pCfgV3->vlanGroupMembershipInterval);
  MIGRATE_COPY_STRUCT (pCfgV4->vlanResponseTime, pCfgV3->vlanResponseTime);
  MIGRATE_COPY_STRUCT (pCfgV4->vlanMcastRtrExpiryTime,
                       pCfgV3->vlanMcastRtrExpiryTime);
  MIGRATE_COPY_STRUCT (pCfgV4->snoopVlanFlags, pCfgV3->snoopVlanFlags);

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
static L7_RC_t snoopMigrateConfigV4V5Convert(snoopCfgDataV4_t *pCfgV4, snoopCfgDataV5_t *pCfgV5)
{
  L7_uint32 i;
  
  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != SNOOP_IGMP_CFG_VER_4 &&
      pCfgV4->cfgHdr.version != SNOOP_IGMP_CFG_VER_3 &&
      pCfgV4->cfgHdr.version != SNOOP_IGMP_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, SNOOP_IGMP_CFG_VER_4);
    return L7_FAILURE;
  }

  snoopBuildDefaultConfigDataV5 (pCfgV5);

  pCfgV5->snoopAdminMode = pCfgV4->snoopAdminMode;
  
  for (i = 1; i < L7_IGMP_SNOOPING_MAX_INTF_REL_4_4; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->snoopIntfCfgData[i],
                         pCfgV4->snoopIntfCfgData[i]);
  }

  /* Migrate VLAN data */
  for (i = 0; i < L7_DOT1Q_MAX_VLAN_ID+1; i++)
  {
    pCfgV5->snoopVlanCfgData[i].groupMembershipInterval
                   = pCfgV4->vlanGroupMembershipInterval[i];
    pCfgV5->snoopVlanCfgData[i].maxResponseTime
                   = pCfgV4->vlanResponseTime[i];
    pCfgV5->snoopVlanCfgData[i].mcastRtrExpiryTime
                   = pCfgV4->vlanMcastRtrExpiryTime[i];
  }

  MIGRATE_COPY_STRUCT (pCfgV5->snoopVlanMode, pCfgV4->snoopVlanFlags);

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
static void snoopBuildDefaultConfigDataV2(snoopCfgDataV2_t *pCfgV2)
{
  L7_uint32 cfgIndex;

  memset ((void *) pCfgV2, 0, sizeof (*pCfgV2));

  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF_REL_4_1; cfgIndex++)
    pCfgV2->snoopIntfCfgData[cfgIndex].intfMode = FD_IGMP_SNOOPING_INTF_MODE;

  strcpy (pCfgV2->cfgHdr.filename, SNOOP_IGMP_CFG_FILENAME);
  pCfgV2->cfgHdr.version = SNOOP_IGMP_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_SNOOPING_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
  pCfgV2->snoopAdminMode = FD_IGMP_SNOOPING_ADMIN_MODE;
  pCfgV2->groupMembershipInterval = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
  pCfgV2->responseTime = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
  pCfgV2->mcastRtrExpiryTime = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;

  return;
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
static void snoopBuildDefaultConfigDataV3(snoopCfgDataV3_t *pCfgV3)
{
  L7_uint32 cfgIndex;
  L7_uint32 vIdx;

  memset(pCfgV3, 0, sizeof(*pCfgV3));

  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF_REL_4_3; cfgIndex++)
  {
    /*
     * Copied from the V3 version of snoopBuildDefaultIntfConfigData()
     */
    if (FD_IGMP_SNOOPING_INTF_MODE == L7_ENABLE)
      pCfgV3->snoopIntfCfgData[cfgIndex].intfMode |= SNOOP_VLAN_MODE;
    else
      pCfgV3->snoopIntfCfgData[cfgIndex].intfMode &= ~SNOOP_VLAN_MODE;
    pCfgV3->snoopIntfCfgData[cfgIndex].intfMcastRtrAttached = FD_IGMP_SNOOPING_MCASTRTR_STATUS;

    for(vIdx = 1 ; vIdx < (L7_DOT1Q_MAX_VLAN_ID + 1) ; vIdx++)
    {
      if (FD_IGMP_SNOOPING_MCASTRTR_STATUS == L7_DISABLE)
        L7_VLAN_CLRMASKBIT(pCfgV3->snoopIntfCfgData[cfgIndex].vlanStaticMcastRtr, vIdx);
      else
        L7_VLAN_SETMASKBIT(pCfgV3->snoopIntfCfgData[cfgIndex].vlanStaticMcastRtr, vIdx);
    }
  }

  strcpy(pCfgV3->cfgHdr.filename, SNOOP_IGMP_CFG_FILENAME);
  pCfgV3->cfgHdr.version = SNOOP_IGMP_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_SNOOPING_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof(*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  pCfgV3->snoopAdminMode = FD_IGMP_SNOOPING_ADMIN_MODE;

  /**********************************
  * Building interface config data  *
  **********************************/
  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF_REL_4_3; cfgIndex++)
  {
    if (FD_IGMP_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE == L7_ENABLE)
      pCfgV3->snoopIntfCfgData[cfgIndex].intfMode |= SNOOP_VLAN_FAST_LEAVE_MODE;
    else
      pCfgV3->snoopIntfCfgData[cfgIndex].intfMode &= ~SNOOP_VLAN_FAST_LEAVE_MODE;

    pCfgV3->snoopIntfCfgData[cfgIndex].groupMembershipInterval = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
    pCfgV3->snoopIntfCfgData[cfgIndex].responseTime = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
    pCfgV3->snoopIntfCfgData[cfgIndex].mcastRtrExpiryTime = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
  }
  
  for (cfgIndex = 1; cfgIndex < (L7_DOT1Q_MAX_VLAN_ID + 1); cfgIndex++)
  {
    pCfgV3->vlanGroupMembershipInterval[cfgIndex] = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
    pCfgV3->vlanResponseTime[cfgIndex] = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
    pCfgV3->vlanMcastRtrExpiryTime[cfgIndex] = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
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
static void snoopBuildDefaultConfigDataV4(snoopCfgDataV4_t *pCfgV4)
{
  L7_uint32 cfgIndex;
  L7_uint32 vIdx;

  memset(pCfgV4, 0, sizeof(*pCfgV4));

  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF_REL_4_4; cfgIndex++)
  {
    /*
     * Copied from the V4 version of snoopBuildDefaultIntfConfigData()
     */
    if (FD_IGMP_SNOOPING_INTF_MODE == L7_ENABLE)
      pCfgV4->snoopIntfCfgData[cfgIndex].intfMode |= SNOOP_VLAN_MODE;
    else
      pCfgV4->snoopIntfCfgData[cfgIndex].intfMode &= ~SNOOP_VLAN_MODE;
    pCfgV4->snoopIntfCfgData[cfgIndex].intfMcastRtrAttached = FD_IGMP_SNOOPING_MCASTRTR_STATUS;

    for(vIdx = 1 ; vIdx < (L7_DOT1Q_MAX_VLAN_ID + 1) ; vIdx++)
    {
      if (FD_IGMP_SNOOPING_MCASTRTR_STATUS == L7_DISABLE)
        L7_VLAN_CLRMASKBIT(pCfgV4->snoopIntfCfgData[cfgIndex].vlanStaticMcastRtr, vIdx);
      else
        L7_VLAN_SETMASKBIT(pCfgV4->snoopIntfCfgData[cfgIndex].vlanStaticMcastRtr, vIdx);
    }
  }

  strcpy(pCfgV4->cfgHdr.filename, SNOOP_IGMP_CFG_FILENAME);
  pCfgV4->cfgHdr.version = SNOOP_IGMP_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_SNOOPING_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = sizeof(*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;

  pCfgV4->snoopAdminMode = FD_IGMP_SNOOPING_ADMIN_MODE;

  /**********************************
  * Building interface config data  *
  **********************************/
  for (cfgIndex = 1; cfgIndex < L7_IGMP_SNOOPING_MAX_INTF_REL_4_4; cfgIndex++)
  {
    if (FD_IGMP_SNOOPING_INTF_FAST_LEAVE_ADMIN_MODE == L7_ENABLE)
      pCfgV4->snoopIntfCfgData[cfgIndex].intfMode |= SNOOP_VLAN_FAST_LEAVE_MODE;
    else
      pCfgV4->snoopIntfCfgData[cfgIndex].intfMode &= ~SNOOP_VLAN_FAST_LEAVE_MODE;
    pCfgV4->snoopIntfCfgData[cfgIndex].groupMembershipInterval = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
    pCfgV4->snoopIntfCfgData[cfgIndex].responseTime = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
    pCfgV4->snoopIntfCfgData[cfgIndex].mcastRtrExpiryTime = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
  }

  for (cfgIndex = 1; cfgIndex < (L7_DOT1Q_MAX_VLAN_ID + 1); cfgIndex++)
  {
    pCfgV4->vlanGroupMembershipInterval[cfgIndex] = FD_IGMP_SNOOPING_GROUP_MEMBERSHIP_INTERVAL;
    pCfgV4->vlanResponseTime[cfgIndex] = FD_IGMP_SNOOPING_MAX_RESPONSE_TIME;
    pCfgV4->vlanMcastRtrExpiryTime[cfgIndex] = FD_IGMP_SNOOPING_MCAST_RTR_EXPIRY_TIME;
  } 
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
static void snoopBuildDefaultConfigDataV5(snoopCfgDataV5_t *pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (snoopCfgDataV5_t));
  snoopIGMPBuildDefaultConfigData (SNOOP_IGMP_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

/* MLD Snooping functions */
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
void snoopMLDMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer)
{
  DUMMY_MIGRATE_FUNCTION(oldVer, ver, sizeof(snoopCfgData_t));
  snoopMLDBuildDefaultConfigData (ver);
  return;
}

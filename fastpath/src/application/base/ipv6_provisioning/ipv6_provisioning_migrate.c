
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_migrate.c
*
* @purpose Contains IPv6 Provisioning Configuration Migration
*
* @component ipv6_provisioning
*
* @comments  none
*
* @create 08/24/2004
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include <string.h>

#include "ipv6_provisioning_migrate.h"
#include "ipv6_provisioning_api.h"

static L7_RC_t ipv6ProvMigrateConfigV1V2Convert (ipv6ProvCfgDataV1_t * pCfgV1,
                                                 ipv6ProvCfgDataV2_t * pCfgV2);
static L7_RC_t ipv6ProvMigrateConfigV2V3Convert (ipv6ProvCfgDataV2_t * pCfgV2,
                                                 ipv6ProvCfgDataV3_t * pCfgV3);
static L7_RC_t ipv6ProvMigrateConfigV3V4Convert (ipv6ProvCfgDataV3_t * pCfgV3,
                                                 ipv6ProvCfgDataV4_t * pCfgV4);
static void ipv6ProvBuildDefaultConfigDataV2 (ipv6ProvCfgDataV2_t * pCfgV2);
static void ipv6ProvBuildDefaultConfigDataV3 (ipv6ProvCfgDataV3_t * pCfgV3);
static void ipv6ProvBuildDefaultConfigDataV4 (ipv6ProvCfgDataV4_t * pCfgV4);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static ipv6ProvCfgData_t *pCfgCur = L7_NULLPTR;

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
ipv6ProvMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  ipv6ProvCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  ipv6ProvCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  ipv6ProvCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  ipv6ProvCfgDataV4_t *pCfgV4 = L7_NULLPTR;

  pCfgCur = (ipv6ProvCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case IPV6_PROVISIONING_CFG_VER_1:
    pCfgV1 = (ipv6ProvCfgDataV1_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                  (L7_uint32) sizeof (ipv6ProvCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (ipv6ProvCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = IPV6_PROVISIONING_CFG_VER_1;
    break;

  case IPV6_PROVISIONING_CFG_VER_2:
    pCfgV2 = (ipv6ProvCfgDataV2_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                  (L7_uint32) sizeof (ipv6ProvCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (ipv6ProvCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = IPV6_PROVISIONING_CFG_VER_2;
    break;

  case IPV6_PROVISIONING_CFG_VER_3:
    pCfgV3 = (ipv6ProvCfgDataV3_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                  (L7_uint32) sizeof (ipv6ProvCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (ipv6ProvCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = IPV6_PROVISIONING_CFG_VER_3;
    break;

  case IPV6_PROVISIONING_CFG_VER_4:
    pCfgV4 = (ipv6ProvCfgDataV4_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                  (L7_uint32) sizeof (ipv6ProvCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (ipv6ProvCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = IPV6_PROVISIONING_CFG_VER_4;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                         IPV6_PROVISIONING_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
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
    case IPV6_PROVISIONING_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (ipv6ProvCfgDataV2_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                    (L7_uint32) sizeof (ipv6ProvCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != ipv6ProvMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case IPV6_PROVISIONING_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (ipv6ProvCfgDataV3_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                    (L7_uint32) sizeof (ipv6ProvCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != ipv6ProvMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case IPV6_PROVISIONING_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (ipv6ProvCfgDataV4_t *) osapiMalloc (L7_IPV6_PROVISIONING_COMPONENT_ID,
                                                    (L7_uint32) sizeof (ipv6ProvCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != ipv6ProvMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case IPV6_PROVISIONING_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgCur));
      osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV4);
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
    osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV3);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_IPV6_PROVISIONING_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_IPV6_PROVISIONING_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    ipv6ProvBuildDefaultConfigData (IPV6_PROVISIONING_CFG_VER_CURRENT);
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
ipv6ProvMigrateConfigV1V2Convert (ipv6ProvCfgDataV1_t * pCfgV1, ipv6ProvCfgDataV2_t * pCfgV2)
{
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 dstIfNum;
  L7_uint32 intIfNum;
  L7_uint32 intIfCount;
  L7_uint32 ifConfigIndex;
  nimConfigID_t srcConfigId;
  nimConfigID_t dstConfigId;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != IPV6_PROVISIONING_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, IPV6_PROVISIONING_CFG_VER_1);
    return L7_FAILURE;
  }

  ipv6ProvBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_1,
                    L7_MAX_INTERFACE_COUNT_REL_4_0);
  intIfNum = 0;
  ifConfigIndex = 1;
  for (i = 1; i < intIfCount; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &srcConfigId, &intIfNum, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    if (intIfNum >= L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_1)
    {
      MIGRATE_INDEX_OUTOF_BOUND (i);
      break;
    }

    if (ifConfigIndex >= L7_MAX_INTERFACE_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (IPV6_PROVISIONING_CFG_FILENAME, ifConfigIndex);
      break;
    }
    if (ipv6ProvIsValidIntfType (srcConfigId.type) != L7_TRUE)
      continue;

    dstIfNum = pCfgV1->ipv6PortMappings[i].dstIfNum;
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (dstIfNum, &dstConfigId, L7_NULL, L7_NULL);
    if (L7_SUCCESS == rc)
    {
      (void) nimConfigIdCopy (&dstConfigId, &pCfgV2->ipv6PortMappings[ifConfigIndex].ipv6DstId);
    }
    else
    {
        /* Do not log a message for default configuration of zero */
        if (dstIfNum != 0)
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
                    "Cannot migrate dstIfNum %d\n", dstIfNum);
        }
        continue;
    }
    (void) nimConfigIdCopy (&srcConfigId, &pCfgV2->ipv6PortMappings[ifConfigIndex].configId);
    pCfgV2->ipv6PortMappings[ifConfigIndex].ipv6copyToCpu = pCfgV1->ipv6PortMappings[i].ipv6copyToCpu;
    pCfgV2->ipv6PortMappings[ifConfigIndex].ipv6Mode = pCfgV1->ipv6PortMappings[i].ipv6Mode;
    ifConfigIndex++;
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
ipv6ProvMigrateConfigV2V3Convert (ipv6ProvCfgDataV2_t * pCfgV2, ipv6ProvCfgDataV3_t * pCfgV3)
{
  L7_uint32 i;
  L7_uint32 intIfCount;
  L7_uint32 ifConfigIndex;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != IPV6_PROVISIONING_CFG_VER_2 &&
      pCfgV2->cfgHdr.version != IPV6_PROVISIONING_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, IPV6_PROVISIONING_CFG_VER_2);
    return L7_FAILURE;
  }

  ipv6ProvBuildDefaultConfigDataV3 (pCfgV3);

  ifConfigIndex = 1;
  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_1,
                    L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_3);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->ipv6PortMappings[i], pCfgV2->ipv6PortMappings[i]);

    if (ifConfigIndex >= L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_3)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (IPV6_PROVISIONING_CFG_FILENAME, ifConfigIndex);
      break;
    }
    ifConfigIndex++;
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
ipv6ProvMigrateConfigV3V4Convert (ipv6ProvCfgDataV3_t * pCfgV3, ipv6ProvCfgDataV4_t * pCfgV4)
{
  L7_uint32 i;
  L7_uint32 intIfCount;
  L7_uint32 ifConfigIndex;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != IPV6_PROVISIONING_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, IPV6_PROVISIONING_CFG_VER_3);
    return L7_FAILURE;
  }

  ipv6ProvBuildDefaultConfigDataV4 (pCfgV4);

  ifConfigIndex = 1;
  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intIfCount = min (L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_1,
                    L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_4);
  for (i = 1; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->ipv6PortMappings[i], pCfgV3->ipv6PortMappings[i]);

    if (ifConfigIndex >= L7_IPV6_PROVISIONING_MAX_INTERFACE_COUNT_REL_4_4)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (IPV6_PROVISIONING_CFG_FILENAME, ifConfigIndex);
      break;
    }
    ifConfigIndex++;
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
ipv6ProvBuildDefaultConfigDataV2 (ipv6ProvCfgDataV2_t * pCfgV2)
{
  memset ((void *) pCfgV2, 0, sizeof (*pCfgV2));

  strcpy ((L7_char8 *) pCfgV2->cfgHdr.filename, IPV6_PROVISIONING_CFG_FILENAME);
  pCfgV2->cfgHdr.version = IPV6_PROVISIONING_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_IPV6_PROVISIONING_COMPONENT_ID;
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
ipv6ProvBuildDefaultConfigDataV3 (ipv6ProvCfgDataV3_t * pCfgV3)
{
  memset ((void *) pCfgV3, 0, sizeof (*pCfgV3));

  strcpy ((L7_char8 *) pCfgV3->cfgHdr.filename, IPV6_PROVISIONING_CFG_FILENAME);
  pCfgV3->cfgHdr.version = IPV6_PROVISIONING_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_IPV6_PROVISIONING_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32) sizeof (*pCfgV3);
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
ipv6ProvBuildDefaultConfigDataV4 (ipv6ProvCfgDataV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (ipv6ProvCfgDataV4_t));
  ipv6ProvBuildDefaultConfigData (IPV6_PROVISIONING_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

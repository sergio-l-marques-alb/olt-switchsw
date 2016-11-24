
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename ipmap_migrate.c
*
* @purpose IPMAP Configuration Migration
*
* @component IPMAP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "ipmap_migrate.h"

static L7_RC_t ipStaticRtsMigrateConfigV1V2Convert (L7_rtrStaticRouteCfgV1_t *
                                                    pCfgV1, L7_rtrStaticRouteCfgV2_t * pCfgV2);
static L7_RC_t ipStaticRtsMigrateConfigV2V3Convert (L7_rtrStaticRouteCfgV2_t *
                                                    pCfgV2, L7_rtrStaticRouteCfgV3_t * pCfgV3);
static L7_RC_t ipStaticRtsMigrateConfigV3V4Convert (L7_rtrStaticRouteCfgV3_t *
                                                    pCfgV3, L7_rtrStaticRouteCfgV4_t * pCfgV4);
static L7_RC_t ipStaticRtsMigrateConfigV4V5Convert (L7_rtrStaticRouteCfgV4_t *
                                                    pCfgV4, L7_rtrStaticRouteCfgV5_t * pCfgV5);
static L7_RC_t ipStaticRtsMigrateConfigV5V6Convert(L7_rtrStaticRouteCfgV5_t *pCfgV5, 
                                                   L7_rtrStaticRouteCfgV6_t *pCfgV6);
static void ipStaticRtsBuildDefaultConfigDataV2 (L7_rtrStaticRouteCfgV2_t * pCfgV2);
static void ipStaticRtsBuildDefaultConfigDataV3 (L7_rtrStaticRouteCfgV3_t * pCfgV3);
static void ipStaticRtsBuildDefaultConfigDataV4 (L7_rtrStaticRouteCfgV4_t * pCfgV4);
static void ipStaticRtsBuildDefaultConfigDataV5 (L7_rtrStaticRouteCfgV5_t * pCfgV5);
static void ipStaticRtsBuildDefaultConfigDataV6 (L7_rtrStaticRouteCfgV6_t * pCfgV6);

static L7_RC_t ipStaticArpMigrateConfigV1V2Convert (L7_rtrStaticArpCfgV1_t * pCfgV1,
                                                    L7_rtrStaticArpCfgV2_t * pCfgV2);
static L7_RC_t ipStaticArpMigrateConfigV2V3Convert (L7_rtrStaticArpCfgV2_t *pCfgV2,
                                                    L7_rtrStaticArpCfgV3_t *pCfgV3);
static void ipStaticArpBuildDefaultConfigDataV2 (L7_rtrStaticArpCfgV2_t *pCfgV2);
static void ipStaticArpBuildDefaultConfigDataV3 (L7_rtrStaticArpCfgV3_t *pCfgV3);

static L7_RC_t ipmapMigrateConfigV4V5Convert (L7_ipMapCfgV4_t * pCfgV4, L7_ipMapCfgV5_t * pCfgV5);
static L7_RC_t ipmapMigrateConfigV5V6Convert (L7_ipMapCfgV5_t * pCfgV5, L7_ipMapCfgV6_t * pCfgV6);
static L7_RC_t ipmapMigrateConfigV6V7Convert (L7_ipMapCfgV6_t * pCfgV6, L7_ipMapCfgV7_t * pCfgV7);
static L7_RC_t ipmapMigrateConfigV7V8Convert (L7_ipMapCfgV7_t * pCfgV7, L7_ipMapCfgV8_t * pCfgV8);
static L7_RC_t ipmapMigrateConfigV8V9Convert (L7_ipMapCfgV8_t * pCfgV8, L7_ipMapCfgV9_t * pCfgV9);
static void ipBuildDefaultConfigDataV5 (L7_ipMapCfgV5_t * pCfgV5);
static void ipBuildDefaultConfigDataV6 (L7_ipMapCfgV6_t * pCfgV6);
static void ipBuildDefaultConfigDataV7 (L7_ipMapCfgV7_t * pCfgV7);
static void ipBuildDefaultConfigDataV8 (L7_ipMapCfgV8_t * pCfgV8);
static void ipBuildDefaultConfigDataV9 (L7_ipMapCfgV9_t * pCfgV9);
/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static L7_ipMapCfg_t *pIpMapCfgCur = L7_NULLPTR;
static L7_rtrStaticArpCfg_t *pRtrStaticArpCfgCur = L7_NULLPTR;
static L7_rtrStaticRouteCfg_t *pRtrStaticRouteCfgCur = L7_NULLPTR;

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
ipStaticRtsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_rtrStaticRouteCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_rtrStaticRouteCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_rtrStaticRouteCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_rtrStaticRouteCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_rtrStaticRouteCfgV5_t *pCfgV5 = L7_NULLPTR;
  L7_rtrStaticRouteCfgV6_t *pCfgV6 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pRtrStaticRouteCfgCur = (L7_rtrStaticRouteCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pRtrStaticRouteCfgCur));

  if ((L7_IP_STATIC_ROUTES_CFG_VER_1 == oldVer) &&
      (sizeof (L7_rtrStaticRouteCfgV2_t) == pCfgHdr->length))
  {
    oldVer = L7_IP_STATIC_ROUTES_CFG_VER_2;
  }

  
  if ((L7_IP_STATIC_ROUTES_CFG_VER_1 == oldVer) &&
      (sizeof (L7_rtrStaticRouteCfgV3_t) == pCfgHdr->length))
  {
    oldVer = L7_IP_STATIC_ROUTES_CFG_VER_3;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case 0:
  case L7_IP_STATIC_ROUTES_CFG_VER_1:
    pCfgV1 = (L7_rtrStaticRouteCfgV1_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof
                                                       (L7_rtrStaticRouteCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticRouteCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = oldVer;
    break;

  case L7_IP_STATIC_ROUTES_CFG_VER_2:
    pCfgV2 = (L7_rtrStaticRouteCfgV2_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof
                                                       (L7_rtrStaticRouteCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticRouteCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case L7_IP_STATIC_ROUTES_CFG_VER_3:
    pCfgV3 = (L7_rtrStaticRouteCfgV3_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof
                                                       (L7_rtrStaticRouteCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticRouteCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case L7_IP_STATIC_ROUTES_CFG_VER_4:
    pCfgV4 = (L7_rtrStaticRouteCfgV4_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof
                                                       (L7_rtrStaticRouteCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticRouteCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_IP_STATIC_ROUTES_CFG_VER_4;
    break;
 
  case L7_IP_STATIC_ROUTES_CFG_VER_5:
    pCfgV5 = (L7_rtrStaticRouteCfgV5_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof
                                                       (L7_rtrStaticRouteCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticRouteCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = L7_IP_STATIC_ROUTES_CFG_VER_5;
    break;
 
  case L7_IP_STATIC_ROUTES_CFG_VER_6:
    pCfgV6 = (L7_rtrStaticRouteCfgV6_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof
                                                       (L7_rtrStaticRouteCfgV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticRouteCfgV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = L7_IP_STATIC_ROUTES_CFG_VER_6;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_IP_MAP_COMPONENT_ID,
                                         L7_IP_STATIC_ROUTES_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d\n"
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
    case 0:
    case L7_IP_STATIC_ROUTES_CFG_VER_1:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV2 = (L7_rtrStaticRouteCfgV2_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                         (L7_uint32)
                                                         sizeof (L7_rtrStaticRouteCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) ||
          (L7_SUCCESS != ipStaticRtsMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_IP_STATIC_ROUTES_CFG_VER_2:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV3 = (L7_rtrStaticRouteCfgV3_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                         (L7_uint32)
                                                         sizeof (L7_rtrStaticRouteCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != ipStaticRtsMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_IP_STATIC_ROUTES_CFG_VER_3:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV4 = (L7_rtrStaticRouteCfgV4_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                         (L7_uint32)
                                                         sizeof (L7_rtrStaticRouteCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != ipStaticRtsMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_IP_STATIC_ROUTES_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (L7_rtrStaticRouteCfgV5_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                         (L7_uint32)
                                                         sizeof (L7_rtrStaticRouteCfgV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != ipStaticRtsMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case L7_IP_STATIC_ROUTES_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (L7_rtrStaticRouteCfgV6_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                         (L7_uint32)
                                                         sizeof (L7_rtrStaticRouteCfgV6_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_NULLPTR == pCfgV5) ||
          (L7_SUCCESS != ipStaticRtsMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */
      break;

    case L7_IP_STATIC_ROUTES_CFG_VER_6:
      if (L7_NULLPTR == pCfgV6)
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pRtrStaticRouteCfgCur, (L7_uchar8 *) pCfgV6,
              sizeof (*pRtrStaticRouteCfgCur));
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pRtrStaticRouteCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV6);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("ipStaticRtsMigrateConfigData: Building Defaults\n");
    memset ((void *) pRtrStaticRouteCfgCur, 0, sizeof (pRtrStaticRouteCfgCur));
    ipStaticRtsBuildDefaultConfigData (L7_IP_STATIC_ROUTES_CFG_VER_CURRENT);
    pRtrStaticRouteCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
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
ipStaticRtsMigrateConfigV1V2Convert (L7_rtrStaticRouteCfgV1_t * pCfgV1,
                                     L7_rtrStaticRouteCfgV2_t * pCfgV2)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if ((pCfgV1->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_1) && (pCfgV1->cfgHdr.version != 0))       /* b'cos REL_G incorrect */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_IP_STATIC_ROUTES_CFG_VER_1);
    return L7_FAILURE;
  }

  ipStaticRtsBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_RTR_MAX_STATIC_ROUTES_REL_4_1, L7_RTR_MAX_STATIC_ROUTES);
  for (i = 0; i < count; i++)
  {
    pCfgV2->rtrStaticRouteCfgData[i].inUse = pCfgV1->rtrStaticRouteCfgData[i].inUse;
    pCfgV2->rtrStaticRouteCfgData[i].ipAddr = pCfgV1->rtrStaticRouteCfgData[i].ipAddr;
    pCfgV2->rtrStaticRouteCfgData[i].ipMask = pCfgV1->rtrStaticRouteCfgData[i].ipMask;
    pCfgV2->rtrStaticRouteCfgData[i].nextHopRtr = pCfgV1->rtrStaticRouteCfgData[i].nextHopRtr;
    pCfgV2->rtrStaticRouteCfgData[i].flags = pCfgV1->rtrStaticRouteCfgData[i].flags;
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
ipStaticRtsMigrateConfigV2V3Convert (L7_rtrStaticRouteCfgV2_t * pCfgV2,
                                     L7_rtrStaticRouteCfgV3_t * pCfgV3)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if ((pCfgV2->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_2) && (pCfgV2->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_1))   /* b'cos REL_H incorrect */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_IP_STATIC_ROUTES_CFG_VER_2);
    return L7_FAILURE;
  }

  ipStaticRtsBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_RTR_MAX_STATIC_ROUTES_REL_4_1, L7_RTR_MAX_STATIC_ROUTES);
  for (i = 0; i < count; i++)
  {
    pCfgV3->rtrStaticRouteCfgData[i].inUse = pCfgV2->rtrStaticRouteCfgData[i].inUse;
    pCfgV3->rtrStaticRouteCfgData[i].ipAddr = pCfgV2->rtrStaticRouteCfgData[i].ipAddr;
    pCfgV3->rtrStaticRouteCfgData[i].ipMask = pCfgV2->rtrStaticRouteCfgData[i].ipMask;
    pCfgV3->rtrStaticRouteCfgData[i].nextHopRtr[0] = pCfgV2->rtrStaticRouteCfgData[i].nextHopRtr;
    pCfgV3->rtrStaticRouteCfgData[i].preference = ipMapRouterPreferenceGet(ROUTE_PREF_STATIC);
    pCfgV3->rtrStaticRouteCfgData[i].flags = pCfgV2->rtrStaticRouteCfgData[i].flags;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
* @param    pCfgV4        @b{(input)} ptr to version 4 config data structure
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
ipStaticRtsMigrateConfigV3V4Convert (L7_rtrStaticRouteCfgV3_t * pCfgV3,
                                     L7_rtrStaticRouteCfgV4_t * pCfgV4)
{
  L7_uint32 i, j, count, countEqRosts;

  /* verify correct version of old config file */
  if ((pCfgV3->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_2) && 
      (pCfgV3->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_1) &&    /* b'cos REL_H/H2 incorrect */
      (pCfgV3->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_3))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_IP_STATIC_ROUTES_CFG_VER_3);
    return L7_FAILURE;
  }

  ipStaticRtsBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_RTR_MAX_STATIC_ROUTES_REL_4_2, L7_RTR_MAX_STATIC_ROUTES);
  countEqRosts  = min (L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_2, L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_3);
  for (i = 0; i < count; i++)
  {
    pCfgV4->rtrStaticRouteCfgData[i].inUse = pCfgV3->rtrStaticRouteCfgData[i].inUse;
    pCfgV4->rtrStaticRouteCfgData[i].ipAddr = pCfgV3->rtrStaticRouteCfgData[i].ipAddr;
    pCfgV4->rtrStaticRouteCfgData[i].ipMask = pCfgV3->rtrStaticRouteCfgData[i].ipMask;
    for (j = 0; j < countEqRosts; j++)
      pCfgV4->rtrStaticRouteCfgData[i].nextHopRtr[j] = pCfgV3->rtrStaticRouteCfgData[i].nextHopRtr[j];
    pCfgV4->rtrStaticRouteCfgData[i].preference = pCfgV3->rtrStaticRouteCfgData[i].preference;
    pCfgV4->rtrStaticRouteCfgData[i].flags = pCfgV3->rtrStaticRouteCfgData[i].flags;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5
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
ipStaticRtsMigrateConfigV4V5Convert (L7_rtrStaticRouteCfgV4_t * pCfgV4,
                                     L7_rtrStaticRouteCfgV5_t * pCfgV5)
{
  L7_uint32 i, j, count, countEqRosts;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_IP_STATIC_ROUTES_CFG_VER_4);
    return L7_FAILURE;
  }

  ipStaticRtsBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_RTR_MAX_STATIC_ROUTES_REL_4_3, L7_RTR_MAX_STATIC_ROUTES);
  countEqRosts  = min (L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_3, L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_4);
  for (i = 0; i < count; i++)
  {
    pCfgV5->rtrStaticRouteCfgData[i].inUse = pCfgV4->rtrStaticRouteCfgData[i].inUse;
    pCfgV5->rtrStaticRouteCfgData[i].ipAddr = pCfgV4->rtrStaticRouteCfgData[i].ipAddr;
    pCfgV5->rtrStaticRouteCfgData[i].ipMask = pCfgV4->rtrStaticRouteCfgData[i].ipMask;
    for (j = 0; j < countEqRosts; j++)
    {
      pCfgV5->rtrStaticRouteCfgData[i].nextHopRtr[j] = 
        pCfgV4->rtrStaticRouteCfgData[i].nextHopRtr[j];
    }
    pCfgV5->rtrStaticRouteCfgData[i].preference = pCfgV4->rtrStaticRouteCfgData[i].preference;
    pCfgV5->rtrStaticRouteCfgData[i].flags = pCfgV4->rtrStaticRouteCfgData[i].flags;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5
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
ipStaticRtsMigrateConfigV5V6Convert (L7_rtrStaticRouteCfgV5_t * pCfgV5,
                                     L7_rtrStaticRouteCfgV6_t * pCfgV6)
{
  L7_uint32 i, j, count, maxNextHops;

  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != L7_IP_STATIC_ROUTES_CFG_VER_5)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, L7_IP_STATIC_ROUTES_CFG_VER_5);
    return L7_FAILURE;
  }

  ipStaticRtsBuildDefaultConfigDataV6(pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_RTR_MAX_STATIC_ROUTES_REL_4_4, L7_RTR_MAX_STATIC_ROUTES);
  maxNextHops  = min (L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_4, L7_RT_MAX_EQUAL_COST_ROUTES);
  for (i = 0; i < count; i++)
  {
    pCfgV6->rtrStaticRouteCfgData[i].inUse = pCfgV5->rtrStaticRouteCfgData[i].inUse;
    pCfgV6->rtrStaticRouteCfgData[i].ipAddr = pCfgV5->rtrStaticRouteCfgData[i].ipAddr;
    pCfgV6->rtrStaticRouteCfgData[i].ipMask = pCfgV5->rtrStaticRouteCfgData[i].ipMask;
    for (j = 0; j < maxNextHops; j++)
    {
      pCfgV6->rtrStaticRouteCfgData[i].nextHops[j].nextHopRtr = 
        pCfgV5->rtrStaticRouteCfgData[i].nextHopRtr[j];
      /* leave next hop interface unspecified */
    }
    pCfgV6->rtrStaticRouteCfgData[i].preference = pCfgV5->rtrStaticRouteCfgData[i].preference;
    pCfgV6->rtrStaticRouteCfgData[i].flags = pCfgV5->rtrStaticRouteCfgData[i].flags;
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
ipStaticRtsBuildDefaultConfigDataV2 (L7_rtrStaticRouteCfgV2_t * pCfgV2)
{
  memset (pCfgV2, 0, sizeof (*pCfgV2));

  strcpy (pCfgV2->cfgHdr.filename, L7_IP_STATIC_ROUTES_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_IP_STATIC_ROUTES_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_IP_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (L7_rtrStaticRouteCfgV2_t);
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
ipStaticRtsBuildDefaultConfigDataV3 (L7_rtrStaticRouteCfgV3_t * pCfgV3)
{
    memset (pCfgV3, 0, sizeof (*pCfgV3));

    strcpy (pCfgV3->cfgHdr.filename, L7_IP_STATIC_ROUTES_CFG_FILENAME);
    pCfgV3->cfgHdr.version = L7_IP_STATIC_ROUTES_CFG_VER_3;
    pCfgV3->cfgHdr.componentID = L7_IP_MAP_COMPONENT_ID;
    pCfgV3->cfgHdr.type = L7_CFG_DATA;
    pCfgV3->cfgHdr.length = sizeof (L7_rtrStaticRouteCfgV3_t);
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
ipStaticRtsBuildDefaultConfigDataV4 (L7_rtrStaticRouteCfgV4_t *pCfgV4)
{
    memset (pCfgV4, 0, sizeof (*pCfgV4));

    strcpy (pCfgV4->cfgHdr.filename, L7_IP_STATIC_ROUTES_CFG_FILENAME);
    pCfgV4->cfgHdr.version = L7_IP_STATIC_ROUTES_CFG_VER_4;
    pCfgV4->cfgHdr.componentID = L7_IP_MAP_COMPONENT_ID;
    pCfgV4->cfgHdr.type = L7_CFG_DATA;
    pCfgV4->cfgHdr.length = sizeof (L7_rtrStaticRouteCfgV4_t);
    pCfgV4->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build Version 5 defaults for static routes config file
*
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
ipStaticRtsBuildDefaultConfigDataV5 (L7_rtrStaticRouteCfgV5_t *pCfgV5)
{
    memset (pCfgV5, 0, sizeof (*pCfgV5));

    strcpy (pCfgV5->cfgHdr.filename, L7_IP_STATIC_ROUTES_CFG_FILENAME);
    pCfgV5->cfgHdr.version = L7_IP_STATIC_ROUTES_CFG_VER_5;
    pCfgV5->cfgHdr.componentID = L7_IP_MAP_COMPONENT_ID;
    pCfgV5->cfgHdr.type = L7_CFG_DATA;
    pCfgV5->cfgHdr.length = sizeof (L7_rtrStaticRouteCfgV5_t);
    pCfgV5->cfgHdr.dataChanged = L7_FALSE;  
}

/*********************************************************************
* @purpose  Build Version 6 defaults for static routes config file
*
* @param    pCfgV6   @b{(input)} ptr to version 6 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
ipStaticRtsBuildDefaultConfigDataV6 (L7_rtrStaticRouteCfgV6_t *pCfgV6)
{
  memset ((L7_uchar8 *) pRtrStaticRouteCfgCur, 0, sizeof (L7_rtrStaticRouteCfgV6_t));
  ipStaticRtsBuildDefaultConfigData (L7_IP_STATIC_ROUTES_CFG_VER_6);
  memcpy ((L7_uchar8 *) pCfgV6, (L7_uchar8 *) pRtrStaticRouteCfgCur,
          sizeof (*pRtrStaticRouteCfgCur));
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
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void
ipStaticArpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_rtrStaticArpCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_rtrStaticArpCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_rtrStaticArpCfgV3_t *pCfgV3 = L7_NULLPTR;

  pRtrStaticArpCfgCur = (L7_rtrStaticArpCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pRtrStaticArpCfgCur));

  if (pCfgHdr->version == L7_IP_STATIC_ARP_CFG_VER_1 &&
      pCfgHdr->length == sizeof (L7_rtrStaticArpCfgV2_t))
  {
    oldVer = L7_IP_STATIC_ARP_CFG_VER_2;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case 0:
  case L7_IP_STATIC_ARP_CFG_VER_1:
    pCfgV1 = (L7_rtrStaticArpCfgV1_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                     (L7_uint32) sizeof (L7_rtrStaticArpCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticArpCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = oldVer;
    break;

  case L7_IP_STATIC_ARP_CFG_VER_2:
    pCfgV2 = (L7_rtrStaticArpCfgV2_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                     (L7_uint32) sizeof (L7_rtrStaticArpCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticArpCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = pCfgHdr->version;       /* b'cos REL_H2 version is not updated */
    break;

  case L7_IP_STATIC_ARP_CFG_VER_3:
    pCfgV3 = (L7_rtrStaticArpCfgV3_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                     (L7_uint32) sizeof (L7_rtrStaticArpCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_rtrStaticArpCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_IP_STATIC_ARP_CFG_VER_3; 
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_IP_MAP_COMPONENT_ID,
                                         L7_IP_STATIC_ARP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d\n"
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
    case 0:
    case L7_IP_STATIC_ARP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_rtrStaticArpCfgV2_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof (L7_rtrStaticArpCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) ||
          (L7_SUCCESS != ipStaticArpMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_IP_STATIC_ARP_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_rtrStaticArpCfgV3_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                       (L7_uint32) sizeof (L7_rtrStaticArpCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != ipStaticArpMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_IP_STATIC_ARP_CFG_VER_3:
      /* NOTE: this is the current version, so copy it to cfg struct */
      if ( L7_NULLPTR == pCfgV3 )
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pRtrStaticArpCfgCur, (L7_uchar8 *) pCfgV3,
              sizeof (*pRtrStaticArpCfgCur));
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pRtrStaticArpCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV3);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("ipStaticArpMigrateConfigData: Building Defaults\n");
    memset ((void *) pRtrStaticArpCfgCur, 0, sizeof (pRtrStaticArpCfgCur));
    ipStaticArpBuildDefaultConfigData (L7_IP_STATIC_ARP_CFG_VER_CURRENT);
    pRtrStaticArpCfgCur->cfgHdr.dataChanged = L7_TRUE;
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
ipStaticArpMigrateConfigV1V2Convert (L7_rtrStaticArpCfgV1_t * pCfgV1,
                                     L7_rtrStaticArpCfgV2_t * pCfgV2)
{
  L7_uint32 i;

  /* verify correct version of old config file */
  if ((pCfgV1->cfgHdr.version != L7_IP_STATIC_ARP_CFG_VER_1) && (pCfgV1->cfgHdr.version != 0))  /* b'cos REL_G version incorrect */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_IP_STATIC_ARP_CFG_VER_1);
    return L7_FAILURE;
  }

  ipStaticArpBuildDefaultConfigDataV2 (pCfgV2);

  for (i = 0; i < FD_RTR_MAX_STATIC_ARP_ENTRIES; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV2->rtrStaticArpCfgData[i].ipAddr,
                         pCfgV1->rtrStaticArpCfgData[i].ipAddr);
    MIGRATE_COPY_STRUCT (pCfgV2->rtrStaticArpCfgData[i].macAddr,
                         pCfgV1->rtrStaticArpCfgData[i].macAddr);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the static ARP config data structure from version V2 to V3.
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
ipStaticArpMigrateConfigV2V3Convert (L7_rtrStaticArpCfgV2_t *pCfgV2,
                                     L7_rtrStaticArpCfgV3_t *pCfgV3)
{
  L7_uint32 i;
  L7_uint32 numEntries = min(L7_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_4, 
                             L7_RTR_MAX_STATIC_ARP_ENTRIES);

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_IP_STATIC_ARP_CFG_VER_2) 
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, 
                                    L7_IP_STATIC_ARP_CFG_VER_2);
    return L7_FAILURE;
  }

  ipStaticArpBuildDefaultConfigDataV3 (pCfgV3);

  for (i = 0; i < numEntries; i++)
  {
    pCfgV3->rtrStaticArpCfgData[i].ipAddr = pCfgV2->rtrStaticArpCfgData[i].ipAddr;
    MIGRATE_COPY_STRUCT (pCfgV3->rtrStaticArpCfgData[i].macAddr,
                         pCfgV2->rtrStaticArpCfgData[i].macAddr);
    /* leave interface unspecified */
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
ipStaticArpBuildDefaultConfigDataV2 (L7_rtrStaticArpCfgV2_t * pCfgV2)
{
  memset(( void * )pCfgV2, 0, sizeof(L7_rtrStaticArpCfgV2_t));
  strcpy(pCfgV2->cfgHdr.filename, L7_IP_STATIC_ARP_CFG_FILENAME);
  pCfgV2->cfgHdr.version       = L7_IP_STATIC_ARP_CFG_VER_2;
  pCfgV2->cfgHdr.componentID   = L7_IP_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type          = L7_CFG_DATA;
  pCfgV2->cfgHdr.length        = sizeof(L7_rtrStaticArpCfgV2_t);
  pCfgV2->cfgHdr.dataChanged   = L7_FALSE;
}

/*********************************************************************
* @purpose  Build Version 3 defaults for static ARP config
*
* @param    pCfgV3    @b{(input)} ptr to version 2 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
ipStaticArpBuildDefaultConfigDataV3(L7_rtrStaticArpCfgV3_t *pCfgV3)
{
  memset ((L7_uchar8 *) pRtrStaticArpCfgCur, 0, sizeof (L7_rtrStaticArpCfgV3_t));
  ipStaticArpBuildDefaultConfigData (L7_IP_STATIC_ARP_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pRtrStaticArpCfgCur, 
          sizeof (*pRtrStaticArpCfgCur));
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
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void
ipmapMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_ipMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_ipMapCfgV5_t *pCfgV5 = L7_NULLPTR;
  L7_ipMapCfgV6_t *pCfgV6 = L7_NULLPTR;
  L7_ipMapCfgV7_t *pCfgV7 = L7_NULLPTR;
  L7_ipMapCfgV8_t *pCfgV8 = L7_NULLPTR;
  L7_ipMapCfgV9_t *pCfgV9 = L7_NULLPTR;
  pIpMapCfgCur = (L7_ipMapCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pIpMapCfgCur));

  if (pCfgHdr->version == L7_IP_CFG_VER_4 && pCfgHdr->length == sizeof (L7_ipMapCfgV5_t))
  {
    oldVer = L7_IP_CFG_VER_5;
  }
  else if (pCfgHdr->version == L7_IP_CFG_VER_4 && pCfgHdr->length == sizeof (L7_ipMapCfgV6_t))
  {
    oldVer = L7_IP_CFG_VER_6;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_IP_CFG_VER_4:
    pCfgV4 = (L7_ipMapCfgV4_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (L7_ipMapCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_IP_CFG_VER_4;
    break;

  case L7_IP_CFG_VER_5:
    pCfgV5 = (L7_ipMapCfgV5_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (L7_ipMapCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = L7_IP_CFG_VER_4;        /* b'cos: REL_H version was not updated */
    break;

  case L7_IP_CFG_VER_6:
    pCfgV6 = (L7_ipMapCfgV6_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (L7_ipMapCfgV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = L7_IP_CFG_VER_4;        /* b'cos: REL_H version was not updated */
    break;

  case L7_IP_CFG_VER_7:
    pCfgV7 = (L7_ipMapCfgV7_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (L7_ipMapCfgV7_t));
    if (L7_NULLPTR == pCfgV7)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV7;
    oldCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV7_t);
    pOldCfgCksum = &pCfgV7->checkSum;
    oldCfgVer = L7_IP_CFG_VER_7;
    break;

  case L7_IP_CFG_VER_8:
    pCfgV8 = (L7_ipMapCfgV8_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                              (L7_uint32) sizeof (L7_ipMapCfgV8_t));
    if (L7_NULLPTR == pCfgV8)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV8;
    oldCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV8_t);
    pOldCfgCksum = &pCfgV8->checkSum;
    oldCfgVer = L7_IP_CFG_VER_8;
    break;
  case L7_IP_CFG_VER_9:
    pCfgV9 = (L7_ipMapCfgV9_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
        (L7_uint32) sizeof (L7_ipMapCfgV9_t));
    if (L7_NULLPTR == pCfgV9)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV9;
    oldCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV9_t);
    pOldCfgCksum = &pCfgV9->checkSum;
    oldCfgVer = L7_IP_CFG_VER_9;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_IP_MAP_COMPONENT_ID,
                                         L7_IP_CFG_FILENAME, pOldCfgBuf,
                                         oldCfgSize, pOldCfgCksum, oldCfgVer,
                                         L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IP_MAP_COMPONENT_ID,
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
    case L7_IP_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (L7_ipMapCfgV5_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ipMapCfgV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) || (L7_SUCCESS != ipmapMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case L7_IP_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (L7_ipMapCfgV6_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ipMapCfgV6_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_NULLPTR == pCfgV5) || (L7_SUCCESS != ipmapMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case L7_IP_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      pCfgV7 = (L7_ipMapCfgV7_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ipMapCfgV7_t));
      if ((L7_NULLPTR == pCfgV7) || (L7_NULLPTR == pCfgV6) || (L7_SUCCESS != ipmapMigrateConfigV6V7Convert (pCfgV6, pCfgV7)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;
      /*passthru */

    case L7_IP_CFG_VER_7:
      /* pCfgV7 has already been allocated and filled in */
      pCfgV8 = (L7_ipMapCfgV8_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ipMapCfgV8_t));
      if ((L7_NULLPTR == pCfgV8) || (L7_NULLPTR == pCfgV7) || (L7_SUCCESS != ipmapMigrateConfigV7V8Convert (pCfgV7, pCfgV8)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV7);
      pCfgV7 = L7_NULLPTR;
      /*passthru */

    case L7_IP_CFG_VER_8:
      /* pCfgV8 has already been allocated and filled in */
      pCfgV9 = (L7_ipMapCfgV9_t *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
          (L7_uint32) sizeof (L7_ipMapCfgV9_t));
      if ((L7_NULLPTR == pCfgV9) || (L7_SUCCESS != ipmapMigrateConfigV8V9Convert (pCfgV8, pCfgV9)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV8);
      pCfgV8 = L7_NULLPTR;
      /*passthru */
    case L7_IP_CFG_VER_9:
      /* pCfgV9 has already been allocated and filled in */
      /* NOTE: this is the current version, so copy it to cfg struct */
      memcpy ((L7_uchar8 *) pIpMapCfgCur, (L7_uchar8 *) pCfgV9, sizeof (*pIpMapCfgCur));
      osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV9);
      pCfgV9 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pIpMapCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV6);
  if (pCfgV7 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV7);
  if (pCfgV8 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV8);
  if (pCfgV9 != L7_NULLPTR)
    osapiFree (L7_IP_MAP_COMPONENT_ID, pCfgV9);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pIpMapCfgCur, 0, sizeof (pIpMapCfgCur));
    ipBuildDefaultConfigData (L7_IP_CFG_VER_CURRENT);
    pIpMapCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the interface config data structure from version V4 to V5
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
ipmapMigrateConfigCktV4V5Convert (L7_ipMapCfgV4_t * pCfgV4, L7_ipMapCfgV5_t * pCfgV5,
                                  L7_uint32 intfType, L7_uint32 * offsetp)
{
  L7_RC_t rc;
  L7_uint32 i, j;
  nimConfigID_t configId;
  L7_uint32 intfCfgIndex = *offsetp;
  L7_BOOL truncated;

           
  /* Note: The methodology used by the policyMigrate* routines in migrating different 
           types  of interfaces is more mature than the multiple invocations
           of the same routine implemented for this routine as fewer loop checks
           are required. However, this works, so we go with it. */ 

  /* Note: The truncated flag merely indicates if the migration was truncated within
           this invocation of this routine. */ 

  truncated = L7_FALSE;

  intfCfgIndex = *offsetp;
  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    for (j = 1; j <= L7_MAX_PORTS_PER_SLOT_REL_4_0; j++)
    {
      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (i, j, &configId, L7_NULL, L7_NULL);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (i, j);
        continue;
      }

      /* Determine if the interface is valid for participation in this feature */
      if (configId.type != intfType)
        continue;

      if (intfCfgIndex >= L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE (L7_IP_CFG_FILENAME, intfCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV5->ckt[intfCfgIndex].configId);
      pCfgV5->ckt[intfCfgIndex].ipAddr = pCfgV4->ckt[i][j].ipAddr;
      pCfgV5->ckt[intfCfgIndex].ipMask = pCfgV4->ckt[i][j].ipMask;
      pCfgV5->ckt[intfCfgIndex].flags = pCfgV4->ckt[i][j].flags;
      intfCfgIndex++;
    }

    if (truncated == L7_TRUE)
        break;
  }

  *offsetp = intfCfgIndex;
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
ipmapMigrateConfigV4V5Convert (L7_ipMapCfgV4_t * pCfgV4, L7_ipMapCfgV5_t * pCfgV5)
{
  L7_int32 intfCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != L7_IP_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_IP_CFG_VER_4);
    return L7_FAILURE;
  }

  ipBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV5->ip, pCfgV4->ip);
  pCfgV5->rtr.rtrAdminMode = pCfgV4->rtr.rtrAdminMode;
  pCfgV5->rtr.rtrForwarding = pCfgV4->rtr.rtrForwarding;
  pCfgV5->rtr.rtrTOSForwarding = pCfgV4->rtr.rtrTOSForwarding;
  pCfgV5->rtr.rtrICMPRedirectMode = pCfgV4->rtr.rtrICMPRedirectMode;
  pCfgV5->rtr.rtrBootpDhcpRelayMode = pCfgV4->rtr.rtrBootpDhcpRelayMode;
  pCfgV5->rtr.rtrBootpDhcpRelayServer = pCfgV4->rtr.rtrBootpDhcpRelayServer;
  pCfgV5->rtr.rtrTraceMode = pCfgV4->rtr.rtrTraceMode;

  intfCfgIndex = 1;
  ipmapMigrateConfigCktV4V5Convert (pCfgV4, pCfgV5, L7_PHYSICAL_INTF, &intfCfgIndex);

  if (intfCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      ipmapMigrateConfigCktV4V5Convert (pCfgV4, pCfgV5, L7_LOGICAL_VLAN_INTF, &intfCfgIndex);

  if (intfCfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1)
      ipmapMigrateConfigCktV4V5Convert (pCfgV4, pCfgV5, L7_LAG_INTF, &intfCfgIndex);

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
ipmapMigrateConfigV5V6Convert (L7_ipMapCfgV5_t * pCfgV5, L7_ipMapCfgV6_t * pCfgV6)
{
  L7_uint32 i;

  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != L7_IP_CFG_VER_5 && pCfgV5->cfgHdr.version != L7_IP_CFG_VER_4)   /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, L7_IP_CFG_VER_5);
    return L7_FAILURE;
  }

  ipBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV6->ip, pCfgV5->ip);
  pCfgV6->rtr.rtrAdminMode = pCfgV5->rtr.rtrAdminMode;
  pCfgV6->rtr.rtrForwarding = pCfgV5->rtr.rtrForwarding;
  pCfgV6->rtr.rtrTOSForwarding = pCfgV5->rtr.rtrTOSForwarding;
  pCfgV6->rtr.rtrICMPRedirectMode = pCfgV5->rtr.rtrICMPRedirectMode;
  pCfgV6->rtr.rtrBootpDhcpRelayMode = pCfgV5->rtr.rtrBootpDhcpRelayMode;
  pCfgV6->rtr.rtrBootpDhcpRelayServer = pCfgV5->rtr.rtrBootpDhcpRelayServer;
  pCfgV6->rtr.rtrProxyArpMode = pCfgV5->rtr.rtrProxyArpMode;
  pCfgV6->rtr.rtrTraceMode = pCfgV5->rtr.rtrTraceMode;

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_2; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV6->ckt[i].configId, pCfgV5->ckt[i].configId);
    pCfgV6->ckt[i].ipAddr = pCfgV5->ckt[i].ipAddr;
    pCfgV6->ckt[i].ipMask = pCfgV5->ckt[i].ipMask;
    pCfgV6->ckt[i].flags = pCfgV5->ckt[i].flags;
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
ipmapMigrateConfigV6V7Convert (L7_ipMapCfgV6_t * pCfgV6, L7_ipMapCfgV7_t * pCfgV7)
{
  L7_uint32 i;

  /* verify correct version of old config file */
  if (pCfgV6->cfgHdr.version != L7_IP_CFG_VER_6 && pCfgV6->cfgHdr.version != L7_IP_CFG_VER_4)   /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV6->cfgHdr.version, L7_IP_CFG_VER_6);
    return L7_FAILURE;
  }

  ipBuildDefaultConfigDataV7 (pCfgV7);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV7->ip, pCfgV6->ip);
  pCfgV7->rtr.rtrAdminMode = pCfgV6->rtr.rtrAdminMode;
  pCfgV7->rtr.rtrForwarding = pCfgV6->rtr.rtrForwarding;
  pCfgV7->rtr.rtrTOSForwarding = pCfgV6->rtr.rtrTOSForwarding;
  pCfgV7->rtr.rtrICMPRedirectMode = pCfgV6->rtr.rtrICMPRedirectMode;
  pCfgV7->rtr.rtrBootpDhcpRelayMode = pCfgV6->rtr.rtrBootpDhcpRelayMode;
  pCfgV7->rtr.rtrBootpDhcpRelayServer = pCfgV6->rtr.rtrBootpDhcpRelayServer;
  pCfgV7->rtr.rtrTraceMode = pCfgV6->rtr.rtrTraceMode;

  for (i = 0; i < L7_IPMAP_INTF_MAX_COUNT_REL_4_2; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV7->ckt[i].configId, pCfgV6->ckt[i].configId);
    pCfgV7->ckt[i].addrs[0].ipAddr = pCfgV6->ckt[i].ipAddr;
    pCfgV7->ckt[i].addrs[0].ipMask = pCfgV6->ckt[i].ipMask;
    pCfgV7->ckt[i].flags = pCfgV6->ckt[i].flags;
    pCfgV7->ckt[i].ipMtu = pCfgV6->ckt[i].ipMtu;
  }

  return L7_SUCCESS;
}

typedef enum
{
  v7_RTO_RESERVED = 0,      /* Not a valid protocol */
  v7_RTO_LOCAL,
  v7_RTO_STATIC,
  v7_RTO_MPLS,
  v7_RTO_OSPF_INTRA_AREA,
  v7_RTO_OSPF_INTER_AREA,
  v7_RTO_OSPF_TYPE1_EXT,
  v7_RTO_OSPF_TYPE2_EXT,
  v7_RTO_OSPF_NSSA_TYPE1_EXT,
  v7_RTO_OSPF_NSSA_TYPE2_EXT,
  v7_RTO_RIP,
  v7_RTO_DEFAULT,
  v7_RTO_IBGP,  
  v7_RTO_EBGP,
  v7_RTO_RSVD1,       /* Reserved to minimize config file upgrades */
  v7_RTO_RSVD2,       /* Reserved to minimize config file upgrades */
  v7_RTO_RSVD3,       /* Reserved to minimize config file upgrades */
  v7_RTO_LAST_PROTOCOL  /* Always keep this last */

} L7_RTO_PROTOCOL_INDICES_t_v7;


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
ipmapMigrateConfigV7V8Convert (L7_ipMapCfgV7_t * pCfgV7, L7_ipMapCfgV8_t * pCfgV8)
{
  L7_uint32 i;
  L7_uint32 count;

  /* verify correct version of old config file */
  if (pCfgV7->cfgHdr.version != L7_IP_CFG_VER_7 &&
      pCfgV7->cfgHdr.version != L7_IP_CFG_VER_6 &&
      pCfgV7->cfgHdr.version != L7_IP_CFG_VER_4)   /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV7->cfgHdr.version, L7_IP_CFG_VER_7);
    return L7_FAILURE;
  }

  ipBuildDefaultConfigDataV8 (pCfgV8);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV8->ip.ipSourceAddrCheck = pCfgV7->ip.ipSourceAddrCheck;
  pCfgV8->ip.ipSpoofingCheck = pCfgV7->ip.ipSpoofingCheck;
  pCfgV8->ip.ipDefaultTTL = pCfgV7->ip.ipDefaultTTL;
  pCfgV8->ip.ipArpAgeTime = pCfgV7->ip.ipArpAgeTime;
  pCfgV8->ip.ipArpRespTime = pCfgV7->ip.ipArpRespTime;
  pCfgV8->ip.ipArpRetries = pCfgV7->ip.ipArpRetries;
  pCfgV8->ip.ipArpCacheSize = pCfgV7->ip.ipArpCacheSize;
  pCfgV8->ip.ipArpDynamicRenew = pCfgV7->ip.ipArpDynamicRenew;

  pCfgV8->ip.route_preferences[RTO_STATIC] = pCfgV7->ip.route_preferences[v7_RTO_STATIC];
  pCfgV8->ip.route_preferences[RTO_OSPF_INTRA_AREA] = pCfgV7->ip.route_preferences[v7_RTO_OSPF_INTRA_AREA];
  pCfgV8->ip.route_preferences[RTO_OSPF_INTER_AREA] = pCfgV7->ip.route_preferences[v7_RTO_OSPF_INTER_AREA];
  pCfgV8->ip.route_preferences[RTO_OSPF_TYPE1_EXT] = pCfgV7->ip.route_preferences[v7_RTO_OSPF_TYPE1_EXT];
  pCfgV8->ip.route_preferences[RTO_OSPF_TYPE2_EXT] = pCfgV7->ip.route_preferences[v7_RTO_OSPF_TYPE2_EXT];
  pCfgV8->ip.route_preferences[RTO_OSPF_NSSA_TYPE1_EXT] = pCfgV7->ip.route_preferences[v7_RTO_OSPF_NSSA_TYPE1_EXT];
  pCfgV8->ip.route_preferences[RTO_OSPF_NSSA_TYPE2_EXT] = pCfgV7->ip.route_preferences[v7_RTO_OSPF_NSSA_TYPE2_EXT];
  pCfgV8->ip.route_preferences[RTO_RIP] = pCfgV7->ip.route_preferences[v7_RTO_RIP];
  pCfgV8->ip.route_preferences[RTO_DEFAULT] = pCfgV7->ip.route_preferences[v7_RTO_DEFAULT];
  pCfgV8->ip.route_preferences[RTO_IBGP] = pCfgV7->ip.route_preferences[v7_RTO_IBGP];
  pCfgV8->ip.route_preferences[RTO_EBGP] = pCfgV7->ip.route_preferences[v7_RTO_EBGP];

  pCfgV8->rtr.rtrAdminMode = pCfgV7->rtr.rtrAdminMode;
  pCfgV8->rtr.rtrForwarding = pCfgV7->rtr.rtrForwarding;
  pCfgV8->rtr.rtrTOSForwarding = pCfgV7->rtr.rtrTOSForwarding;
  pCfgV8->rtr.rtrICMPRedirectMode = pCfgV7->rtr.rtrICMPRedirectMode;
  pCfgV8->rtr.rtrBootpDhcpRelayMode = pCfgV7->rtr.rtrBootpDhcpRelayMode;
  pCfgV8->rtr.rtrBootpDhcpRelayServer = pCfgV7->rtr.rtrBootpDhcpRelayServer;
  pCfgV8->rtr.rtrTraceMode = pCfgV7->rtr.rtrTraceMode;

  count = min(L7_IPMAP_INTF_MAX_COUNT_REL_4_3, L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV8->ckt[i], pCfgV7->ckt[i]);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V8 to V9.
*
* @param    pCfgV8    @b{(input)} ptr to version 8 config data structure
* @param    pCfgV9    @b{(input)} ptr to version 9 config data structure
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
ipmapMigrateConfigV8V9Convert (L7_ipMapCfgV8_t * pCfgV8, L7_ipMapCfgV9_t * pCfgV9)
{
  L7_uint32 i,j;
  L7_uint32 count;

  /* verify correct version of old config file */
  if (pCfgV8->cfgHdr.version != L7_IP_CFG_VER_8 &&
      pCfgV8->cfgHdr.version != L7_IP_CFG_VER_7 &&
      pCfgV8->cfgHdr.version != L7_IP_CFG_VER_6 &&
      pCfgV8->cfgHdr.version != L7_IP_CFG_VER_4)   /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV8->cfgHdr.version, L7_IP_CFG_VER_8);
    return L7_FAILURE;
  }
  ipBuildDefaultConfigDataV9 (pCfgV9);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV9->ip.ipSourceAddrCheck = pCfgV8->ip.ipSourceAddrCheck;
  pCfgV9->ip.ipSpoofingCheck = pCfgV8->ip.ipSpoofingCheck;
  pCfgV9->ip.ipDefaultTTL = pCfgV8->ip.ipDefaultTTL;
  pCfgV9->ip.ipArpAgeTime = pCfgV8->ip.ipArpAgeTime;
  pCfgV9->ip.ipArpRespTime = pCfgV8->ip.ipArpRespTime;
  pCfgV9->ip.ipArpRetries = pCfgV8->ip.ipArpRetries;
  pCfgV9->ip.ipArpCacheSize = pCfgV8->ip.ipArpCacheSize;
  pCfgV9->ip.ipArpDynamicRenew = pCfgV8->ip.ipArpDynamicRenew;

  pCfgV9->ip.route_preferences[ROUTE_PREF_STATIC] = pCfgV8->ip.route_preferences[RTO_STATIC];
  pCfgV9->ip.route_preferences[ROUTE_PREF_OSPF_INTRA_AREA] = pCfgV8->ip.route_preferences[RTO_OSPF_INTRA_AREA];
  pCfgV9->ip.route_preferences[ROUTE_PREF_OSPF_INTER_AREA] = pCfgV8->ip.route_preferences[RTO_OSPF_INTER_AREA];

  /* From release L onwards, we shall be treating the route preference values to be the same
   * for all ospf external routes
   * We get the type1 route's preference value in earlier config set to the external route
   * preference value */

  pCfgV9->ip.route_preferences[ROUTE_PREF_OSPF_EXTERNAL] = pCfgV8->ip.route_preferences[RTO_OSPF_TYPE1_EXT];
  pCfgV9->ip.route_preferences[ROUTE_PREF_RIP] = pCfgV8->ip.route_preferences[RTO_RIP];
  pCfgV9->ip.route_preferences[ROUTE_PREF_IBGP] = pCfgV8->ip.route_preferences[RTO_IBGP];
  pCfgV9->ip.route_preferences[ROUTE_PREF_EBGP] = pCfgV8->ip.route_preferences[RTO_EBGP];
  
  pCfgV9->rtr.rtrAdminMode = pCfgV8->rtr.rtrAdminMode;
  pCfgV9->rtr.rtrTOSForwarding = pCfgV8->rtr.rtrTOSForwarding;
  pCfgV9->rtr.rtrICMPRedirectMode = pCfgV8->rtr.rtrICMPRedirectMode;
  pCfgV9->rtr.rtrTraceMode = pCfgV8->rtr.rtrTraceMode;

  count = min(L7_IPMAP_INTF_MAX_COUNT_REL_4_3, L7_IPMAP_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < count; i++)
  {
    pCfgV9->ckt[i].icmpUnreachablesMode = pCfgV8->ckt[i].icmpUnreachablesMode;
    pCfgV9->ckt[i].ipMtu = pCfgV8->ckt[i].ipMtu ;
    pCfgV9->ckt[i].flags =  pCfgV8->ckt[i].flags;    
    MIGRATE_COPY_STRUCT (pCfgV9->ckt[i].configId, pCfgV8->ckt[i].configId);
      
    for(j=0; j< L7_L3_NUM_IP_ADDRS;j++)
    MIGRATE_COPY_STRUCT (pCfgV9->ckt[i].addrs[j],pCfgV8->ckt[i].addrs[j]);
  }

  return L7_SUCCESS;
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
ipBuildDefaultConfigDataV5 (L7_ipMapCfgV5_t * pCfgV5)
{
  L7_uint32 cfgIndex;
  L7_rtrCfgCktV5_t *pCfg;

  memset (pCfgV5, 0, sizeof (pCfgV5));

  /* Generic IP Stack Cfg */
  pCfgV5->ip.ipSourceAddrCheck = FD_IP_DEFAULT_IP_SRC_ADDR_CHECKING;
  pCfgV5->ip.ipSpoofingCheck = FD_IP_DEFAULT_IP_SPOOFING;
  pCfgV5->ip.ipDefaultTTL = FD_IP_DEFAULT_TTL;

#ifdef  L7_MAP_ROUTE_PREFERENCES
  memcpy ((L7_uint32 *) & pCfgV5->ip.route_preferences, (L7_uint32 *) & FD_rtr_rto_preferences,
          (sizeof (L7_uint32) * L7_RTO_MAX_PROTOCOLS));
#endif

  /* ARP Configuration parameters */
  pCfgV5->ip.ipArpAgeTime = FD_IP_DEFAULT_ARP_AGE_TIME;
  pCfgV5->ip.ipArpRespTime = FD_IP_DEFAULT_ARP_RESP_TIME;
  pCfgV5->ip.ipArpRetries = FD_IP_DEFAULT_ARP_RETRIES;
  pCfgV5->ip.ipArpCacheSize = FD_IP_DEFAULT_ARP_CACHE_SIZE;
  pCfgV5->ip.ipArpDynamicRenew = FD_IP_DEFAULT_ARP_DYNAMIC_RENEW;

  /* Router Configuration parameters */
  pCfgV5->rtr.rtrAdminMode = FD_RTR_DEFAULT_ADMIN_MODE;
  pCfgV5->rtr.rtrForwarding = FD_RTR_DEFAULT_FORWARDING;
  pCfgV5->rtr.rtrTOSForwarding = FD_RTR_DEFAULT_TOS_FORWARDING;
  pCfgV5->rtr.rtrEcmpMode = FD_RTR_DEFAULT_ECMP_MODE;
  pCfgV5->rtr.rtrICMPRedirectMode = FD_RTR_DEFAULT_ICMP_REDIRECT_MODE;
  pCfgV5->rtr.rtrBootpDhcpRelayMode = FD_RTR_DEFAULT_UDP_RELAY_ADMIN_MODE;
  pCfgV5->rtr.rtrBootpDhcpRelayServer = 0;
  /* pCfgV5->rtr.rtrProxyArpMode          = FD_RTR_DEFAULT_PROXY_ARP_MODE; */
  pCfgV5->rtr.rtrTraceMode = FD_RTR_DEFAULT_TRACE_MODE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_1; cfgIndex++)
  {
    pCfg = &pCfgV5->ckt[cfgIndex];
    pCfg->ipAddr = L7_NULL_IP_ADDR;
    pCfg->ipMask = L7_NULL_IP_MASK;
    if (FD_RTR_DEFAULT_INTF_ADMIN_MODE == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_ADMIN_MODE_ENABLE;
    if (FD_RTR_DEFAULT_FWD_MULTICASTS == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_MCAST_FWD_MODE_ENABLE;
    if (FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE;
    if (FD_RTR_DEFAULT_GRAT_ARP == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_GRAT_ARP;
  }

  strcpy (pCfgV5->cfgHdr.filename, L7_IP_CFG_FILENAME);
  pCfgV5->cfgHdr.version = L7_IP_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_IP_MAP_COMPONENT_ID;
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
ipBuildDefaultConfigDataV6 (L7_ipMapCfgV6_t * pCfgV6)
{
  L7_uint32 cfgIndex;
  L7_rtrCfgCktV6_t *pCfg;

  memset (pCfgV6, 0, sizeof (pCfgV6));

  /* Generic IP Stack Cfg */
  pCfgV6->ip.ipSourceAddrCheck = FD_IP_DEFAULT_IP_SRC_ADDR_CHECKING;
  pCfgV6->ip.ipSpoofingCheck = FD_IP_DEFAULT_IP_SPOOFING;
  pCfgV6->ip.ipDefaultTTL = FD_IP_DEFAULT_TTL;

  /* ARP Configuration parameters */
  pCfgV6->ip.ipArpAgeTime = FD_IP_DEFAULT_ARP_AGE_TIME;
  pCfgV6->ip.ipArpRespTime = FD_IP_DEFAULT_ARP_RESP_TIME;
  pCfgV6->ip.ipArpRetries = FD_IP_DEFAULT_ARP_RETRIES;
  pCfgV6->ip.ipArpCacheSize = FD_IP_DEFAULT_ARP_CACHE_SIZE;
  pCfgV6->ip.ipArpDynamicRenew = FD_IP_DEFAULT_ARP_DYNAMIC_RENEW;

#ifdef  L7_MAP_ROUTE_PREFERENCES
  memcpy ((L7_uint32 *) & pCfgV6->ip.route_preferences, (L7_uint32 *) & FD_rtr_rto_preferences,
          (sizeof (L7_uint32) * L7_RTO_MAX_PROTOCOLS));
#endif

  /* Router Configuration parameters */
  pCfgV6->rtr.rtrAdminMode = FD_RTR_DEFAULT_ADMIN_MODE;
  pCfgV6->rtr.rtrForwarding = FD_RTR_DEFAULT_FORWARDING;
  pCfgV6->rtr.rtrTOSForwarding = FD_RTR_DEFAULT_TOS_FORWARDING;
  pCfgV6->rtr.rtrICMPRedirectMode = FD_RTR_DEFAULT_ICMP_REDIRECT_MODE;
  pCfgV6->rtr.rtrBootpDhcpRelayMode = FD_RTR_DEFAULT_UDP_RELAY_ADMIN_MODE;
  pCfgV6->rtr.rtrBootpDhcpRelayServer = 0;
  /* pCfgV6->rtr.rtrProxyArpMode          = FD_RTR_DEFAULT_PROXY_ARP_MODE; */
  pCfgV6->rtr.rtrTraceMode = FD_RTR_DEFAULT_TRACE_MODE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_2; cfgIndex++)
  {
    pCfg = &pCfgV6->ckt[cfgIndex];
    pCfg->ipAddr = L7_NULL_IP_ADDR;
    pCfg->ipMask = L7_NULL_IP_MASK;
    pCfg->ipMtu = FD_IP_DEFAULT_IP_MTU;
    if (FD_RTR_DEFAULT_INTF_ADMIN_MODE == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_ADMIN_MODE_ENABLE;
    if (FD_RTR_DEFAULT_FWD_MULTICASTS == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_MCAST_FWD_MODE_ENABLE;
    if (FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE;
    if (FD_RTR_DEFAULT_GRAT_ARP == L7_ENABLE)
      pCfg->flags |= L7_RTR_INTF_GRAT_ARP;
  }

  strcpy (pCfgV6->cfgHdr.filename, L7_IP_CFG_FILENAME);
  pCfgV6->cfgHdr.version = L7_IP_CFG_VER_6;
  pCfgV6->cfgHdr.componentID = L7_IP_MAP_COMPONENT_ID;
  pCfgV6->cfgHdr.type = L7_CFG_DATA;
  pCfgV6->cfgHdr.length = sizeof (*pCfgV6);
  pCfgV6->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build Version 7 defaults
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
ipBuildDefaultConfigDataV7 (L7_ipMapCfgV7_t * pCfgV7)
{
  L7_uint32 cfgIndex;
  L7_rtrCfgCktV7_t *pCfg;

  memset((void *)pCfgV7, 0, sizeof(*pCfgV7));

  strcpy(pCfgV7->cfgHdr.filename, L7_IP_CFG_FILENAME);
  pCfgV7->cfgHdr.version       = L7_IP_CFG_VER_7;
  pCfgV7->cfgHdr.componentID   = L7_IP_MAP_COMPONENT_ID;
  pCfgV7->cfgHdr.type          = L7_CFG_DATA;
  pCfgV7->cfgHdr.length        = sizeof(*pCfgV7);
  pCfgV7->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* Build Default Config Data */
  /*---------------------------*/

  /* Generic IP Stack Cfg */
  pCfgV7->ip.ipSourceAddrCheck     =  FD_IP_DEFAULT_IP_SRC_ADDR_CHECKING;
  pCfgV7->ip.ipSpoofingCheck       =  FD_IP_DEFAULT_IP_SPOOFING;
  pCfgV7->ip.ipDefaultTTL          =  FD_IP_DEFAULT_TTL;

  /* ARP Configuration parameters */
  pCfgV7->ip.ipArpAgeTime          =  FD_IP_DEFAULT_ARP_AGE_TIME;
  pCfgV7->ip.ipArpRespTime         =  FD_IP_DEFAULT_ARP_RESP_TIME;
  pCfgV7->ip.ipArpRetries          =  FD_IP_DEFAULT_ARP_RETRIES;
  pCfgV7->ip.ipArpCacheSize        =  FD_IP_DEFAULT_ARP_CACHE_SIZE;
  pCfgV7->ip.ipArpDynamicRenew     =  FD_IP_DEFAULT_ARP_DYNAMIC_RENEW;

#ifdef  L7_MAP_ROUTE_PREFERENCES
  memcpy((L7_uint32 *)&pCfgV7->ip.route_preferences,
         (L7_uint32 *)&FD_rtr_rto_preferences,
         (sizeof (L7_uint32) * L7_RTO_MAX_PROTOCOLS));
#endif

  /* Router Configuration parameters */
  pCfgV7->rtr.rtrAdminMode             = FD_RTR_DEFAULT_ADMIN_MODE;
  pCfgV7->rtr.rtrForwarding            = FD_RTR_DEFAULT_FORWARDING;
  pCfgV7->rtr.rtrTOSForwarding         = FD_RTR_DEFAULT_TOS_FORWARDING;
  pCfgV7->rtr.rtrICMPRedirectMode      = FD_RTR_DEFAULT_ICMP_REDIRECT_MODE;
  pCfgV7->rtr.rtrBootpDhcpRelayMode    = FD_RTR_DEFAULT_UDP_RELAY_ADMIN_MODE;
  pCfgV7->rtr.rtrBootpDhcpRelayServer  = 0;
  pCfgV7->rtr.rtrTraceMode             = FD_RTR_DEFAULT_TRACE_MODE;

  /* Router Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_IPMAP_INTF_MAX_COUNT_REL_4_3; cfgIndex++)
  {
    pCfg = &pCfgV7->ckt[cfgIndex];

    pCfg->icmpUnreachablesMode = FD_RTR_DEFAULT_ICMP_UNREACHABLES_MODE;
    pCfg->ipMtu = FD_IP_DEFAULT_IP_MTU;
    if (FD_RTR_DEFAULT_INTF_ADMIN_MODE == L7_ENABLE)
      pCfg->flags  |=  L7_RTR_INTF_ADMIN_MODE_ENABLE;
    if (FD_RTR_DEFAULT_FWD_MULTICASTS == L7_ENABLE)
      pCfg->flags  |=  L7_RTR_INTF_MCAST_FWD_MODE_ENABLE;
    if (FD_RTR_DEFAULT_FWD_NET_DIRECT_BCAST == L7_ENABLE)
      pCfg->flags  |=  L7_RTR_INTF_NETDIR_BCAST_MODE_ENABLE;
    if (FD_PROXY_ARP_ADMIN_MODE == L7_ENABLE)
      pCfg->flags  |=  L7_RTR_INTF_PROXY_ARP;
    if (FD_LOCAL_PROXY_ARP_ADMIN_MODE == L7_ENABLE)
      pCfg->flags  |=  L7_RTR_INTF_LOCAL_PROXY_ARP;
    if (FD_RTR_DEFAULT_GRAT_ARP == L7_ENABLE)
      pCfg->flags  |=  L7_RTR_INTF_GRAT_ARP;
  }
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
ipBuildDefaultConfigDataV8 (L7_ipMapCfgV8_t * pCfgV8)
{
  memset((void *)pCfgV8, 0, sizeof(*pCfgV8));

  strcpy(pCfgV8->cfgHdr.filename, L7_IP_CFG_FILENAME);
  pCfgV8->cfgHdr.version       = L7_IP_CFG_VER_8;
  pCfgV8->cfgHdr.componentID   = L7_IP_MAP_COMPONENT_ID;
  pCfgV8->cfgHdr.type          = L7_CFG_DATA;
  pCfgV8->cfgHdr.length        = sizeof(*pCfgV8);
  pCfgV8->cfgHdr.dataChanged   = L7_FALSE;

  /*---------------------------*/
  /* Build Default Config Data */
  /*---------------------------*/
  /* Rather than fix coverity defect 94197, just delete this code. Being 
   * deleted in 6.2 anyway. Binary config migration to 6.0 not supported for 
   * this component. */
}

/*********************************************************************
* @purpose  Build Version 9 defaults
*
* @param    pCfgV9    @b{(input)} ptr to version 9 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
 static void
ipBuildDefaultConfigDataV9 (L7_ipMapCfgV9_t * pCfgV9)
{
  memset ((L7_uchar8 *) pIpMapCfgCur, 0, sizeof (L7_ipMapCfgV9_t));
  ipBuildDefaultConfigData (L7_IP_CFG_VER_9);
  memcpy ((L7_uchar8 *) pCfgV9, (L7_uchar8 *) pIpMapCfgCur, sizeof (*pIpMapCfgCur));
}



/*********************************************************************
* @purpose  Migrate VLAN interfaces 
*
* @param    pIpMap	Old IP Map
* @param    pIpMap	Old IP Map length
* @param    min_vlan_intIfNum Minimum VLAN internal interface number
*
* @returns  L7_SUCCESS Migration Success
* @returns  L7_ERROR Migration failed
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t ipMapMigrateRel4_0VlanInterfaces (L7_uchar8 * pIpMap, L7_uint32 ipMapLen, 
                                          L7_uint32 min_vlan_intIfNum)
{
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 vlanId;
  L7_uchar8 *bufPtr;
  L7_uint32 mappingIndex;
  L7_uint32 expectedLength;
  L7_ipMapCfgV4_t ipMapCfg;

  expectedLength = sizeof (L7_ipMapCfgV4_t);
  expectedLength -= sizeof (L7_fileHdr_t);
  expectedLength += sizeof (L7_fileHdrV1_t);
  if (ipMapLen != expectedLength)
  {
    LOG_MSG ("IPMAP length mismatch expecting %d got %d", 
             expectedLength, ipMapLen);
    return L7_ERROR;
  }

  bufPtr = (L7_uchar8 *) & ipMapCfg;
  bufPtr += sizeof (L7_fileHdr_t);
  memcpy (bufPtr, &pIpMap[sizeof (L7_fileHdrV1_t)],
        ipMapLen - sizeof (L7_fileHdrV1_t));

  for (slot = 0; slot < L7_MAX_SLOTS_PER_BOX_REL_4_0; slot++)
  {
    for (port = 0; port <= L7_MAX_PORTS_PER_SLOT_REL_4_0; port++)
    {
      vlanId = ipMapCfg.ckt[slot][port].circuitData.vlanID;
      if (vlanId == L7_NULL)
        continue;

      LOG_MSG ("Found VLAN Slot %d port = %d vlanID = %d", slot, port, vlanId);

      /* Port number is offset from port structure */
      mappingIndex = min_vlan_intIfNum + port - 1;
      sysapiCfgFileMapVlanInterfaces (mappingIndex, vlanId, slot, port);
    }
  }
  return L7_SUCCESS;
}



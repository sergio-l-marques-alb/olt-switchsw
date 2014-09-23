
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename radius_migrate.c
*
* @purpose Radius Configuration Migration
*
* @component Radius
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "radius_migrate.h"

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static radiusCfg_t *pCfgCur = L7_NULLPTR;


static L7_RC_t radiusMigrateConfigV1V3Convert (radiusCfgV1_t * pCfgV1, radiusCfgV3_t * pCfgV3);
static void radiusBuildDefaultConfigDataV3 (radiusCfgV3_t *pCfgV3);


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
radiusMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  radiusCfgV1_t *pCfgV1 = L7_NULLPTR;
  radiusCfgV3_t *pCfgV3 = L7_NULLPTR;

  pCfgCur = (radiusCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  switch (oldVer)
  {
  case RADIUS_CFG_VER_1:
  case RADIUS_CFG_VER_2:
    pCfgV1 = (radiusCfgV1_t *) osapiMalloc (L7_RADIUS_COMPONENT_ID,
                                           (L7_uint32) sizeof (radiusCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (radiusCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = oldVer;
    break;

  case RADIUS_CFG_VER_3:
    pCfgV3 = (radiusCfgV3_t *) osapiMalloc (L7_RADIUS_COMPONENT_ID,
                                           (L7_uint32) sizeof (radiusCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (radiusCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = RADIUS_CFG_VER_3;
    break;

  default:
    buildDefault = L7_TRUE;
    oldCfgSize = 0;
    pOldCfgCksum = 0;
    oldCfgVer = 0;
    break;
  }

  if (buildDefault != L7_TRUE)
  {
    /* read the old config file found on this system */
    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_RADIUS_COMPONENT_ID,
                                         RADIUS_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_RADIUS_COMPONENT_ID,
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
    case RADIUS_CFG_VER_1:
    case RADIUS_CFG_VER_2:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV3 = (radiusCfgV3_t *) osapiMalloc (L7_RADIUS_COMPONENT_ID,
                                             (L7_uint32) sizeof (radiusCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != radiusMigrateConfigV1V3Convert (pCfgV1, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_RADIUS_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case RADIUS_CFG_VER_3:
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV3, sizeof (*pCfgCur));
      osapiFree (L7_RADIUS_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->hdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;
    }
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_RADIUS_COMPONENT_ID, pCfgV1);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_RADIUS_COMPONENT_ID, pCfgV3);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_RADIUS_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    radiusBuildConfigData (RADIUS_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
  }
  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v1 to v3
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV3   @b{(input)} ptr to version 3 config data structure
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
radiusMigrateConfigV1V3Convert (radiusCfgV1_t * pCfgV1, radiusCfgV3_t * pCfgV3)
{
  L7_uint32 i;

  /* verify correct version of old config file */
  if (pCfgV1->hdr.version != RADIUS_CFG_VER_1 &&
      pCfgV1->hdr.version != RADIUS_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->hdr.version, RADIUS_CFG_VER_2);
    return L7_FAILURE;
  }

  radiusBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->cfg.radiusClient.authOperMode = pCfgV1->cfg.radiusClient.authOperMode;
  pCfgV3->cfg.radiusClient.acctAdminMode = pCfgV1->cfg.radiusClient.acctAdminMode;
  pCfgV3->cfg.radiusClient.maxNumRetrans = pCfgV1->cfg.radiusClient.maxNumRetrans;
  pCfgV3->cfg.radiusClient.timeOutDuration = pCfgV1->cfg.radiusClient.timeOutDuration;
  pCfgV3->cfg.radiusClient.retryPrimaryTime = pCfgV1->cfg.radiusClient.retryPrimaryTime;
  for (i = 0; i < L7_RADIUS_MAX_SERVERS; i++)
  {
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigIpAddress =
     pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigIpAddress;
    if(pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigIpAddress != 0)
    {
      pCfgV3->cfg.radiusClient.serverConfigEntries[i].
                               radiusServerDnsHostName.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
    }
    else
    {
      pCfgV3->cfg.radiusClient.serverConfigEntries[i].
                               radiusServerDnsHostName.hostAddrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
    }
    memset(pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerDnsHostName.
                                                           host.hostName, 0, L7_DNS_HOST_NAME_LEN_MAX);
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigUdpPort
        = pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigUdpPort;
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigServerType
        = pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigServerType;
    strcpy(pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigSecret,
        pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigSecret);
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerIsSecretConfigured
        = pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerIsSecretConfigured;
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigRowStatus
        = pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigRowStatus;
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigPriority
        = pCfgV1->cfg.radiusClient.serverConfigEntries[i].radiusServerConfigPriority;
    pCfgV3->cfg.radiusClient.serverConfigEntries[i].incMsgAuthMode
        = pCfgV1->cfg.radiusClient.serverConfigEntries[i].incMsgAuthMode;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Build Version 3 defaults
*
* @param    pCfgV1    @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
radiusBuildDefaultConfigDataV3 (radiusCfgV3_t * pCfgV3)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (radiusCfgV3_t));
  radiusBuildConfigData (RADIUS_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

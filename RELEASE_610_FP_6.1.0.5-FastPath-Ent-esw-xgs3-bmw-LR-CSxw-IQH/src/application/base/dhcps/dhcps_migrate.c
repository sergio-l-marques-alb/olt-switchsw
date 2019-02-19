
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   dhcps_migrate.c
*
* @purpose    DHCP Server Configuration Migration
*
* @component  DHCP Server Component
*
* @comments   none
*
* @create     08/24/2004
*
* @author     Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "dhcps_migrate.h"

static L7_RC_t dhcpsMigrateConfigV1V2Convert (L7_dhcpsMapCfgV1_t * pCfgV1,
                                              L7_dhcpsMapCfgV2_t * pCfgV2);

static L7_RC_t dhcpsMigrateConfigV2V3Convert (L7_dhcpsMapCfgV2_t * pCfgV2,
                                              L7_dhcpsMapCfgV3_t * pCfgV3);

static L7_RC_t dhcpsMigrateConfigV3V4Convert (L7_dhcpsMapCfgV3_t * pCfgV3,
                                              L7_dhcpsMapCfgV4_t * pCfgV4);

static L7_RC_t dhcpsMigrateLeaseConfigV1V2Convert (L7_dhcpsMapLeaseCfgV1_t * pCfgV1,
                                                   L7_dhcpsMapLeaseCfgV2_t * pCfgV2);

static L7_RC_t dhcpsMigrateLeaseConfigV2V3Convert (L7_dhcpsMapLeaseCfgV2_t * pCfgV2,
                                                   L7_dhcpsMapLeaseCfgV3_t * pCfgV3);

static void dhcpsBuildDefaultConfigDataV2 (L7_dhcpsMapCfgV2_t * pCfgV2);
static void dhcpsBuildDefaultConfigDataV3 (L7_dhcpsMapCfgV3_t * pCfgV3);
static void dhcpsBuildDefaultConfigDataV4 (L7_dhcpsMapCfgV4_t * pCfgV4);
static void dhcpsBuildDefaultLeaseConfigDataV2 (L7_dhcpsMapLeaseCfgV2_t * pCfgV2);
static void dhcpsBuildDefaultLeaseConfigDataV3 (L7_dhcpsMapLeaseCfgV3_t * pCfgV3);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static L7_dhcpsMapCfg_t *pCfgDataCur = L7_NULLPTR;
static L7_dhcpsMapLeaseCfg_t *pLeaseCfgDataCur = L7_NULLPTR;
static L7_dhcpsMapLeaseCfgV1_t *pLeaseCfgDataV1 = L7_NULLPTR;

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
dhcpsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_dhcpsMapCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_dhcpsMapCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_dhcpsMapCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_dhcpsMapCfgV4_t *pCfgV4 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgDataCur = (L7_dhcpsMapCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgDataCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_DHCPS_CFG_VER_1:
    pCfgV1 = (L7_dhcpsMapCfgV1_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_dhcpsMapCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = L7_DHCPS_CFG_VER_1;
    break;

  case L7_DHCPS_CFG_VER_2:
    pCfgV2 = (L7_dhcpsMapCfgV2_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_dhcpsMapCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_DHCPS_CFG_VER_2;
    break;

  case L7_DHCPS_CFG_VER_3:
    pCfgV3 = (L7_dhcpsMapCfgV3_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_dhcpsMapCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_DHCPS_CFG_VER_3;
    break;

  case L7_DHCPS_CFG_VER_4:
    pCfgV4 = (L7_dhcpsMapCfgV4_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_dhcpsMapCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_DHCPS_CFG_VER_4;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_DHCPS_MAP_COMPONENT_ID,
                                         L7_DHCPS_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCPS_MAP_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d."
                     " Configuration did not exist or could not be read"
                     " for the specified feature.  This message is usually"
                     " followed by a message indicating that default configuration"
                     " values will be used."
                     ,(L7_int32) oldCfgSize,
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
    case L7_DHCPS_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_dhcpsMapCfgV2_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dhcpsMapCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) ||
          (L7_SUCCESS != dhcpsMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_DHCPS_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_dhcpsMapCfgV3_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dhcpsMapCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || 
          (L7_SUCCESS != dhcpsMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_DHCPS_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (L7_dhcpsMapCfgV4_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_dhcpsMapCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) || 
          (L7_SUCCESS != dhcpsMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_DHCPS_CFG_VER_4:
      if (L7_NULLPTR == pCfgV4)
      {
        buildDefault = L7_TRUE;
        break;
      }
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgDataCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgDataCur));
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgDataCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgDataCur, 0, sizeof (*pCfgDataCur));
    dhcpsBuildDefaultConfigData (L7_DHCPS_CFG_VER_CURRENT);
    pCfgDataCur->cfgHdr.dataChanged = L7_TRUE;
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
dhcpsMigrateConfigV1V2Convert (L7_dhcpsMapCfgV1_t * pCfgV1, L7_dhcpsMapCfgV2_t * pCfgV2)
{
  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_DHCPS_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_DHCPS_CFG_VER_1);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV2->dhcps, pCfgV1->dhcps);
  MIGRATE_COPY_STRUCT (pCfgV2->pool, pCfgV1->pool);
  MIGRATE_COPY_STRUCT (pCfgV2->excludedAddrs, pCfgV1->excludedAddrs);
  pLeaseCfgDataV1 = (L7_dhcpsMapLeaseCfgV1_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                      (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV1_t));
  MIGRATE_COPY_STRUCT (pLeaseCfgDataV1->lease, pCfgV1->lease);  /* make a local copy of leases */

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
dhcpsMigrateConfigV2V3Convert (L7_dhcpsMapCfgV2_t * pCfgV2, L7_dhcpsMapCfgV3_t * pCfgV3)
{
  L7_uint32 i;
  L7_uint32 count;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_DHCPS_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_DHCPS_CFG_VER_2);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->dhcps.dhcpsAdminMode = pCfgV2->dhcps.dhcpsAdminMode;
  pCfgV3->dhcps.dhcpsPingPktNo = pCfgV2->dhcps.dhcpsPingPktNo;

  count = min (L7_DHCPS_MAX_POOL_NUMBER_REL_4_0, L7_DHCPS_MAX_POOL_NUMBER_REL_4_2);
  for (i = 0; i < count; i++)
  {
    strcpy (pCfgV3->pool[i].poolName, pCfgV2->pool[i].poolName);
    pCfgV3->pool[i].ipAddr = pCfgV2->pool[i].ipAddr;
    pCfgV3->pool[i].ipMask = pCfgV2->pool[i].ipMask;
    pCfgV3->pool[i].poolType = pCfgV2->pool[i].poolType;
    MIGRATE_COPY_STRUCT (pCfgV3->pool[i].defaultrouterAddrs, pCfgV2->pool[i].defaultrouterAddrs);
    MIGRATE_COPY_STRUCT (pCfgV3->pool[i].DNSServerAddrs, pCfgV2->pool[i].DNSServerAddrs);
    pCfgV3->pool[i].leaseTime = pCfgV2->pool[i].leaseTime;
    strcpy (pCfgV3->pool[i].hostIdentifier, pCfgV2->pool[i].hostIdentifier);
    pCfgV3->pool[i].hostIdLength = pCfgV2->pool[i].hostIdLength;
    strcpy (pCfgV3->pool[i].hostAddr, pCfgV2->pool[i].hostAddr);
    pCfgV3->pool[i].hostAddrtype = pCfgV2->pool[i].hostAddrtype;
    pCfgV3->pool[i].hostAddrLength = pCfgV2->pool[i].hostAddrLength;
    strcpy (pCfgV3->pool[i].hostName, pCfgV2->pool[i].hostName);
  }

  count = min (L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_0, L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_2);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->excludedAddrs[i], pCfgV2->excludedAddrs[i]);
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
dhcpsMigrateConfigV3V4Convert (L7_dhcpsMapCfgV3_t * pCfgV3, L7_dhcpsMapCfgV4_t * pCfgV4)
{
  L7_uint32 i;
  L7_uint32 count;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_DHCPS_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_DHCPS_CFG_VER_3);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV4->dhcps, pCfgV3->dhcps);

  count = min (L7_DHCPS_MAX_POOL_NUMBER_REL_4_3, L7_DHCPS_MAX_POOL_NUMBER_REL_4_2);
  for (i = 0; i < count; i++)
  {
    (void)osapiStrncpySafe(pCfgV4->pool[i].poolName, pCfgV3->pool[i].poolName, L7_DHCPS_POOL_NAME_MAXLEN);
    pCfgV4->pool[i].ipAddr = pCfgV3->pool[i].ipAddr;
    pCfgV4->pool[i].ipMask = pCfgV3->pool[i].ipMask;
    pCfgV4->pool[i].poolType = pCfgV3->pool[i].poolType;
    MIGRATE_COPY_STRUCT (pCfgV4->pool[i].defaultrouterAddrs, pCfgV3->pool[i].defaultrouterAddrs);
    MIGRATE_COPY_STRUCT (pCfgV4->pool[i].DNSServerAddrs, pCfgV3->pool[i].DNSServerAddrs);
    pCfgV4->pool[i].leaseTime = pCfgV3->pool[i].leaseTime;
    memcpy (pCfgV4->pool[i].hostIdentifier, pCfgV3->pool[i].hostIdentifier, L7_DHCPS_CLIENT_ID_MAXLEN);

    /* for v3 (H2/H) to v4 (I) migration: recalculate hostIdLength */
    if(pCfgV3->pool[i].hostIdLength != 0)
    {
      L7_uint32 hostLen = L7_DHCPS_CLIENT_ID_MAXLEN; /* trim upto 7 bytes */
      while(pCfgV4->pool[i].hostIdentifier[hostLen-1] == 0 && hostLen > 7)
        hostLen--;

      pCfgV4->pool[i].hostIdLength = hostLen;
    }
    else
    {
      pCfgV4->pool[i].hostIdLength = 0;
    }

    memcpy (pCfgV4->pool[i].hostAddr, pCfgV3->pool[i].hostAddr, L7_DHCPS_HARDWARE_ADDR_MAXLEN);
    pCfgV4->pool[i].hostAddrtype = pCfgV3->pool[i].hostAddrtype;
    pCfgV4->pool[i].hostAddrLength = pCfgV3->pool[i].hostAddrLength;
    (void)osapiStrncpySafe(pCfgV4->pool[i].hostName, pCfgV3->pool[i].hostName, L7_DHCPS_HOST_NAME_MAXLEN);

    MIGRATE_COPY_STRUCT (pCfgV4->pool[i].netBiosNameServer, pCfgV3->pool[i].netBiosNameServer);
    pCfgV4->pool[i].netBiosNodeType = pCfgV3->pool[i].netBiosNodeType;
    pCfgV4->pool[i].nextServer = pCfgV3->pool[i].nextServer;
    (void)osapiStrncpySafe(pCfgV4->pool[i].domainName, pCfgV3->pool[i].domainName, L7_DHCPS_DOMAIN_NAME_MAXLEN);
    (void)osapiStrncpySafe(pCfgV4->pool[i].clientBootFile, pCfgV3->pool[i].clientBootFile, L7_DHCPS_BOOT_FILE_NAME_MAXLEN);
    MIGRATE_COPY_STRUCT (pCfgV4->pool[i].options, pCfgV3->pool[i].options);
  }

  count = min (L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_3, L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_2);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->excludedAddrs[i], pCfgV3->excludedAddrs[i]);
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
dhcpsBuildDefaultConfigDataV2 (L7_dhcpsMapCfgV2_t * pCfgV2)
{
  L7_uint32 index;

  memset ((L7_uchar8 *) pCfgV2, 0, sizeof (*pCfgV2));
  strcpy (pCfgV2->cfgHdr.filename, L7_DHCPS_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_DHCPS_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_DHCPS_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;

  /* generic DHCP Server cfg */
  pCfgV2->dhcps.dhcpsAdminMode = FD_DHCPS_DEFAULT_ADMIN_MODE;
  pCfgV2->dhcps.dhcpsPingPktNo = FD_DHCPS_DEFAULT_PINGPKTNO;

  for (index = 0; index < L7_DHCPS_MAX_POOL_NUMBER_REL_4_0; index++)
  {
    pCfgV2->pool[index].leaseTime = FD_DHCPS_DEFAULT_POOLCFG_LEASETIME * 60;    /* convert into seconds */
    pCfgV2->pool[index].poolType = FD_DHCPS_DEFAULT_POOLCFG_TYPEOFBINDING;

    pCfgV2->pool[index].DNSServerAddrs[0] = FD_DHCPS_DEFAULT_DNS_SERVER;
    pCfgV2->pool[index].defaultrouterAddrs[0] = FD_DHCPS_DEFAULT_ROUTER;

    strcpy (pCfgV2->pool[index].hostName, FD_DHCPS_DEFAULT_CLIENT_NAME);

    strcpy (pCfgV2->pool[index].hostIdentifier, FD_DHCPS_DEFAULT_CLIENT_ID);
    pCfgV2->pool[index].hostIdLength = strlen (FD_DHCPS_DEFAULT_CLIENT_ID);

    strcpy (pCfgV2->pool[index].hostAddr, FD_DHCPS_DEFAULT_HARDWARE_ADDRESS);
    pCfgV2->pool[index].hostAddrLength = strlen (FD_DHCPS_DEFAULT_HARDWARE_ADDRESS);
    pCfgV2->pool[index].hostAddrtype = FD_DHCPS_DEFAULT_HARDWARE_TYPE;
  }
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
dhcpsBuildDefaultConfigDataV3 (L7_dhcpsMapCfgV3_t * pCfgV3)
{
  L7_uint32 index;

  memset ((L7_uchar8 *) pCfgV3, 0, sizeof (*pCfgV3));
  strcpy (pCfgV3->cfgHdr.filename, L7_DHCPS_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_DHCPS_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_DHCPS_MAP_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  /* generic DHCP Server cfg */
  pCfgV3->dhcps.dhcpsAdminMode = FD_DHCPS_DEFAULT_ADMIN_MODE;
  pCfgV3->dhcps.dhcpsPingPktNo = FD_DHCPS_DEFAULT_PINGPKTNO;
  pCfgV3->dhcps.dhcpConflictLogging = FD_DHCPS_DEFAULT_CONFLICTLOGGING;
  pCfgV3->dhcps.dhcpBootpAuto = FD_DHCPS_DEFAULT_BOOTPAUTO;

  for (index = 0; index < L7_DHCPS_MAX_POOL_NUMBER_REL_4_2; index++)
  {
    pCfgV3->pool[index].leaseTime = FD_DHCPS_DEFAULT_POOLCFG_LEASETIME * 60;    /* convert into seconds */
    pCfgV3->pool[index].poolType = FD_DHCPS_DEFAULT_POOLCFG_TYPEOFBINDING;

    pCfgV3->pool[index].DNSServerAddrs[0] = FD_DHCPS_DEFAULT_DNS_SERVER;
    pCfgV3->pool[index].defaultrouterAddrs[0] = FD_DHCPS_DEFAULT_ROUTER;

    strcpy (pCfgV3->pool[index].hostName, FD_DHCPS_DEFAULT_CLIENT_NAME);

    strcpy (pCfgV3->pool[index].hostIdentifier, FD_DHCPS_DEFAULT_CLIENT_ID);
    pCfgV3->pool[index].hostIdLength = strlen (FD_DHCPS_DEFAULT_CLIENT_ID);

    strcpy (pCfgV3->pool[index].hostAddr, FD_DHCPS_DEFAULT_HARDWARE_ADDRESS);
    pCfgV3->pool[index].hostAddrLength = strlen (FD_DHCPS_DEFAULT_HARDWARE_ADDRESS);
    pCfgV3->pool[index].hostAddrtype = FD_DHCPS_DEFAULT_HARDWARE_TYPE;

    strcpy (pCfgV3->pool[index].domainName, FD_DHCPS_DEFAULT_DOMAIN_NAME);
    strcpy (pCfgV3->pool[index].clientBootFile, FD_DHCPS_DEFAULT_BOOTFILE_NAME);
    pCfgV3->pool[index].nextServer = FD_DHCPS_DEFAULT_NEXTSERVERIP_ADDR;
    pCfgV3->pool[index].netBiosNameServer[0] = FD_DHCPS_DEFAULT_NBNAMESERVERIP_ADDR;
    pCfgV3->pool[index].netBiosNodeType = FD_DHCPS_DEFAULT_NBNODETYPE;
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
static void
dhcpsBuildDefaultConfigDataV4 (L7_dhcpsMapCfgV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgDataCur, 0, sizeof (L7_dhcpsMapCfgV4_t));
  dhcpsBuildDefaultConfigData (L7_DHCPS_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgDataCur, sizeof (*pCfgDataCur));
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
dhcpsMigrateLeaseConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_dhcpsMapLeaseCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_dhcpsMapLeaseCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_dhcpsMapLeaseCfgV3_t *pCfgV3 = L7_NULLPTR;

  SYSAPI_MIGRATE_UNUSED (ver);
  pLeaseCfgDataCur = (L7_dhcpsMapLeaseCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pLeaseCfgDataCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_DHCPS_LEASE_CFG_VER_1:
    pCfgV1 = (L7_dhcpsMapLeaseCfgV1_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                      (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = L7_DHCPS_LEASE_CFG_VER_1;
    break;

  case L7_DHCPS_LEASE_CFG_VER_2:
    pCfgV2 = (L7_dhcpsMapLeaseCfgV2_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                      (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_DHCPS_LEASE_CFG_VER_2;
    break;

  case L7_DHCPS_LEASE_CFG_VER_3:
    pCfgV3 = (L7_dhcpsMapLeaseCfgV3_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                      (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_DHCPS_LEASE_CFG_VER_3;
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

    if ((L7_NULL != pLeaseCfgDataV1) && (sizeof (*pLeaseCfgDataV1) == oldCfgSize))
    {
      memset (&pLeaseCfgDataV1->cfgHdr, 0, sizeof (pLeaseCfgDataV1->cfgHdr));
      strcpy (pLeaseCfgDataV1->cfgHdr.filename, L7_DHCPS_LEASE_CFG_FILENAME);
      pLeaseCfgDataV1->cfgHdr.version = L7_DHCPS_LEASE_CFG_VER_1;
      pLeaseCfgDataV1->cfgHdr.componentID = L7_DHCPS_MAP_COMPONENT_ID;
      pLeaseCfgDataV1->cfgHdr.type = L7_CFG_DATA;
      pLeaseCfgDataV1->cfgHdr.fileHdrVersion = L7_FILE_HDR_VER_CURRENT;
      memcpy (pOldCfgBuf, pLeaseCfgDataV1, oldCfgSize);
    }
    else if ((L7_NULLPTR == pOldCfgBuf) ||
             (L7_SUCCESS != nvStoreGetFile (L7_DHCPS_LEASE_CFG_FILENAME, pOldCfgBuf, oldCfgSize,
                                            pOldCfgCksum, oldCfgVer, L7_NULL)))
    {
      MIGRATE_DEBUG ("nvStoreGetFile failed size = %d version = %d\n", (L7_int32) oldCfgSize,
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
    case L7_DHCPS_LEASE_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_dhcpsMapLeaseCfgV2_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                        (L7_uint32)
                                                        sizeof (L7_dhcpsMapLeaseCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) ||
          (L7_SUCCESS != dhcpsMigrateLeaseConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_DHCPS_LEASE_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_dhcpsMapLeaseCfgV3_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                        (L7_uint32)
                                                        sizeof (L7_dhcpsMapLeaseCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != dhcpsMigrateLeaseConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_DHCPS_LEASE_CFG_VER_3:
      if (L7_NULLPTR == pCfgV3)
      {
        buildDefault = L7_TRUE;
        break;
      }
      /* pCfgV3 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pLeaseCfgDataCur, (L7_uchar8 *) pCfgV3, sizeof (*pLeaseCfgDataCur));
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pLeaseCfgDataCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV3);

  if (pLeaseCfgDataV1 != L7_NULLPTR)
  {
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pLeaseCfgDataV1);
  }

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pLeaseCfgDataCur, 0, sizeof (*pLeaseCfgDataCur));
    dhcpsBuildDefaultLeaseConfigData (L7_DHCPS_LEASE_CFG_VER_CURRENT);
    pLeaseCfgDataCur->cfgHdr.dataChanged = L7_TRUE;
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
dhcpsMigrateLeaseConfigV1V2Convert (L7_dhcpsMapLeaseCfgV1_t * pCfgV1,
                                    L7_dhcpsMapLeaseCfgV2_t * pCfgV2)
{
  L7_int32 i, count;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_DHCPS_LEASE_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_DHCPS_LEASE_CFG_VER_1);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultLeaseConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_DHCPS_MAX_LEASE_NUMBER_REL_4_1, L7_DHCPS_MAX_LEASE_NUMBER_REL_4_0);
  for (i = 0; i < count; i++)
  {
    pCfgV2->lease[i].hAddrtype = pCfgV2->lease[i].hAddrtype;
    pCfgV2->lease[i].hwAddrLength = pCfgV2->lease[i].hwAddrLength;
    pCfgV2->lease[i].ipAddr = pCfgV2->lease[i].ipAddr;
    pCfgV2->lease[i].ipMask = pCfgV2->lease[i].ipMask;
    pCfgV2->lease[i].leaseTime = pCfgV2->lease[i].leaseTime;
    pCfgV2->lease[i].state = pCfgV2->lease[i].state;
    pCfgV2->lease[i].IsBootp = pCfgV2->lease[i].IsBootp;
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
dhcpsMigrateLeaseConfigV2V3Convert (L7_dhcpsMapLeaseCfgV2_t * pCfgV2,
                                    L7_dhcpsMapLeaseCfgV3_t * pCfgV3)
{
  L7_int32 i;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_DHCPS_LEASE_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_DHCPS_LEASE_CFG_VER_2);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultLeaseConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  for (i = 0; i < L7_DHCPS_MAX_LEASE_NUMBER_REL_4_1; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->lease[i], pCfgV2->lease[i]);
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
dhcpsBuildDefaultLeaseConfigDataV2 (L7_dhcpsMapLeaseCfgV2_t * pCfgV2)
{
  memset ((L7_uchar8 *) pCfgV2, 0, sizeof (*pCfgV2));
  strcpy (pCfgV2->cfgHdr.filename, L7_DHCPS_LEASE_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_DHCPS_LEASE_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_DHCPS_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
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
dhcpsBuildDefaultLeaseConfigDataV3 (L7_dhcpsMapLeaseCfgV3_t * pCfgV3)
{
  memset ((L7_uchar8 *) pLeaseCfgDataCur, 0, sizeof (L7_dhcpsMapLeaseCfgV3_t));
  dhcpsBuildDefaultLeaseConfigData (L7_DHCPS_LEASE_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pLeaseCfgDataCur, sizeof (*pLeaseCfgDataCur));
}

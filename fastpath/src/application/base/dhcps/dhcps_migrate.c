
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

static L7_RC_t dhcpsMigrateLeaseConfigV1V2Convert (L7_dhcpsMapLeaseCfgV1_t * pCfgV1,
                                                   L7_dhcpsMapLeaseCfgV2_t * pCfgV2);

static L7_RC_t dhcpsMigrateLeaseConfigV2V3Convert (L7_dhcpsMapLeaseCfgV2_t * pCfgV2,
                                                   L7_dhcpsMapLeaseCfgV3_t * pCfgV3);

static L7_RC_t dhcpsMigrateLeaseConfigV3V4Convert (L7_dhcpsMapLeaseCfgV3_t * pCfgV3,
                                                   L7_dhcpsMapLeaseCfgV4_t * pCfgV4);

static void dhcpsBuildDefaultLeaseConfigDataV2 (L7_dhcpsMapLeaseCfgV2_t * pCfgV2);
static void dhcpsBuildDefaultLeaseConfigDataV3 (L7_dhcpsMapLeaseCfgV3_t * pCfgV3);
static void dhcpsBuildDefaultLeaseConfigDataV4 (L7_dhcpsMapLeaseCfgV4_t * pCfgV4);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
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
  L7_dhcpsMapLeaseCfgV4_t *pCfgV4 = L7_NULLPTR;

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

  case L7_DHCPS_LEASE_CFG_VER_4:
    pCfgV4 = (L7_dhcpsMapLeaseCfgV4_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                      (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_dhcpsMapLeaseCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_DHCPS_LEASE_CFG_VER_4;
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
        (L7_SUCCESS != sysapiCfgFileGet(L7_DHCPS_MAP_COMPONENT_ID,
                                        L7_DHCPS_LEASE_CFG_FILENAME,
                                        pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                        oldCfgVer, L7_NULL /* do not build defaults */ ,
                                        L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCPS_MAP_COMPONENT_ID,
              "sysapiCfgFileGet failed old cfg size = %d old cfg version = %d\n",
              (L7_int32) oldCfgSize, (L7_int32) oldCfgVer);
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
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (L7_dhcpsMapLeaseCfgV4_t *) osapiMalloc (L7_DHCPS_MAP_COMPONENT_ID,
                                                        (L7_uint32)
                                                        sizeof (L7_dhcpsMapLeaseCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != dhcpsMigrateLeaseConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_DHCPS_LEASE_CFG_VER_4:
      if (L7_NULLPTR == pCfgV4)
      {
        buildDefault = L7_TRUE;
        break;
      }
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pLeaseCfgDataCur, (L7_uchar8 *) pCfgV4, sizeof (*pLeaseCfgDataCur));
      osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;

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
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pCfgV4);

  if (pLeaseCfgDataV1 != L7_NULLPTR)
  {
    osapiFree (L7_DHCPS_MAP_COMPONENT_ID, pLeaseCfgDataV1);
  }

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DHCPS_MAP_COMPONENT_ID, "Building Defaults\n");
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
    /*
    Note that the previous version of this file copied the version
    2 lease to the version 2 lease (bug) rather than copying the
    version 1 lease to the version 2 lease
    */

    pCfgV2->lease[i].hAddrtype = pCfgV1->lease[i].hAddrtype;
    pCfgV2->lease[i].hwAddrLength = pCfgV1->lease[i].hwAddrLength;
    pCfgV2->lease[i].ipAddr = pCfgV1->lease[i].ipAddr;
    pCfgV2->lease[i].ipMask = pCfgV1->lease[i].ipMask;
    pCfgV2->lease[i].leaseTime = pCfgV1->lease[i].leaseTime;
    pCfgV2->lease[i].state = pCfgV1->lease[i].state;
    pCfgV2->lease[i].IsBootp = pCfgV1->lease[i].IsBootp;
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
  L7_int32 i, count;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_DHCPS_LEASE_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_DHCPS_LEASE_CFG_VER_2);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultLeaseConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */

  /*
  The current version 3 only allows 256 versus the 1024 allowed
  by version 2 -- therefore migrate only the first 256 leases
  */

  count = min (L7_DHCPS_MAX_LEASE_NUMBER_REL_4_1, L7_DHCPS_MAX_LEASE_NUMBER_REL_4_3);
  for (i = 0; i < count; i++)
  {
    /*
    Note that version 3 of the lease config structure did not match the
    (at the time) current version of the lease structure, because of the
    differences in the max number of leases and the client id lengths

    The clientIdLength element can only as large as 255, but the
    clientIdentifier field from a version 2 is 256 in length -- only the
    first 255 bytes will be migrated; the entry could fail if the identifier
    is actually 256 bytes long, but that could never have been represented
    in version 2 properly anyway -- had the entry been 256 bytes long, the
    length would have been 0 (256 wrapped) and the entry would not have
    been used as a key
    */

    osapiStrncpySafe(pCfgV3->lease[i].poolName, pCfgV2->lease[i].poolName, L7_DHCPS_POOL_NAME_MAXLEN);
    pCfgV3->lease[i].clientIdLength = pCfgV2->lease[i].clientIdLength;
    memcpy(pCfgV3->lease[i].clientIdentifier, pCfgV2->lease[i].clientIdentifier, L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_3);
    memcpy(pCfgV3->lease[i].hwAddr, pCfgV2->lease[i].hwAddr, L7_DHCPS_HARDWARE_ADDR_MAXLEN);
    pCfgV3->lease[i].hAddrtype = pCfgV2->lease[i].hAddrtype;
    pCfgV3->lease[i].hwAddrLength = pCfgV2->lease[i].hwAddrLength;
    pCfgV3->lease[i].ipAddr = pCfgV2->lease[i].ipAddr;
    pCfgV3->lease[i].ipMask = pCfgV2->lease[i].ipMask;

    pCfgV3->lease[i].leaseTime = pCfgV2->lease[i].leaseTime;
    pCfgV3->lease[i].state = pCfgV2->lease[i].state;
    pCfgV3->lease[i].IsBootp = pCfgV2->lease[i].IsBootp;
    pCfgV3->lease[i].conflictDetectionMethod = pCfgV2->lease[i].conflictDetectionMethod;
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
dhcpsMigrateLeaseConfigV3V4Convert (L7_dhcpsMapLeaseCfgV3_t * pCfgV3,
                                    L7_dhcpsMapLeaseCfgV4_t * pCfgV4)
{
  L7_int32 i;
  L7_uint32 upTime = simSystemUpTimeGet();

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_DHCPS_LEASE_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_DHCPS_LEASE_CFG_VER_3);
    return L7_FAILURE;
  }

  dhcpsBuildDefaultLeaseConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */

  for (i = 0; i < L7_DHCPS_MAX_LEASE_NUMBER_REL_4_3; i++)
  {
    /*
    Can't just migrate_copy the struct here, because version 4 has an
    additional entry -- the lease expiration time
    */

    osapiStrncpySafe(pCfgV4->lease[i].poolName, pCfgV3->lease[i].poolName, L7_DHCPS_POOL_NAME_MAXLEN);
    pCfgV4->lease[i].clientIdLength = pCfgV3->lease[i].clientIdLength;
    memcpy(pCfgV4->lease[i].clientIdentifier, pCfgV3->lease[i].clientIdentifier, L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_3);
    memcpy(pCfgV4->lease[i].hwAddr, pCfgV3->lease[i].hwAddr, L7_DHCPS_HARDWARE_ADDR_MAXLEN);
    pCfgV4->lease[i].hAddrtype = pCfgV3->lease[i].hAddrtype;
    pCfgV4->lease[i].hwAddrLength = pCfgV3->lease[i].hwAddrLength;
    pCfgV4->lease[i].ipAddr = pCfgV3->lease[i].ipAddr;
    pCfgV4->lease[i].ipMask = pCfgV3->lease[i].ipMask;

    pCfgV4->lease[i].leaseTime = pCfgV3->lease[i].leaseTime;
    pCfgV4->lease[i].leaseExpiration = pCfgV3->lease[i].leaseTime + upTime;
    pCfgV4->lease[i].state = pCfgV3->lease[i].state;
    pCfgV4->lease[i].IsBootp = pCfgV3->lease[i].IsBootp;
    pCfgV4->lease[i].conflictDetectionMethod = pCfgV3->lease[i].conflictDetectionMethod;
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
  memset ((L7_uchar8 *) pCfgV3, 0, sizeof (*pCfgV3));
  strcpy (pCfgV3->cfgHdr.filename, L7_DHCPS_LEASE_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_DHCPS_LEASE_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_DHCPS_MAP_COMPONENT_ID;
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
dhcpsBuildDefaultLeaseConfigDataV4 (L7_dhcpsMapLeaseCfgV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pLeaseCfgDataCur, 0, sizeof (L7_dhcpsMapLeaseCfgV4_t));
  dhcpsBuildDefaultLeaseConfigData (L7_DHCPS_LEASE_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pLeaseCfgDataCur, sizeof (*pLeaseCfgDataCur));
}

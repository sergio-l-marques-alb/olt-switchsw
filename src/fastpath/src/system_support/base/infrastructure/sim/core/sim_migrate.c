/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename sim_migrate.c
*
* @purpose System Interface Manager Configuration Migration
*
* @component sim
*
* @comments none
*
* @create 8/23/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#include "sim_migrate.h"

static L7_RC_t simMigrateConfigV2V3Convert(simCfgData_ver2_t *pCfgV2,
                                           simCfgData_ver3_t *pCfgV3);
static L7_RC_t simMigrateConfigV3V4Convert(simCfgData_ver3_t *pCfgV3,
                                           simCfgData_ver4_t *pCfgV4);
static L7_RC_t simMigrateConfigV4V5Convert(simCfgData_ver4_t *pCfgV4,
                                           simCfgData_ver5_t *pCfgV5);
static L7_RC_t simMigrateConfigV5V6Convert(simCfgData_ver5_t *pCfgV5,
                                           simCfgData_ver6_t *pCfgV6);
static void simBuildDefaultConfigDataV3(simCfgData_ver3_t *pCfgV3);
static void simBuildDefaultConfigDataV4(simCfgData_ver4_t *pCfgV4);
static void simBuildDefaultConfigDataV5(simCfgData_ver5_t *pCfgV5);
static void simBuildDefaultConfigDataV6(simCfgData_ver6_t *pCfgV6);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static simCfgData_t *pCfgCur = L7_NULLPTR;
extern simTransferInfo_t simTransferInfo;

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
simMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;

  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  simCfgData_ver2_t *pCfgV2 = L7_NULLPTR;
  simCfgData_ver3_t *pCfgV3 = L7_NULLPTR;
  simCfgData_ver4_t *pCfgV4 = L7_NULLPTR;
  simCfgData_ver5_t *pCfgV5 = L7_NULLPTR;
  simCfgData_ver6_t *pCfgV6 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pCfgCur = (simCfgData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == SIM_CFG_VER_2 && pCfgHdr->length == sizeof (simCfgData_ver3_t))
  {
    oldVer = SIM_CFG_VER_3;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case SIM_CFG_VER_2:
    pCfgV2 = (simCfgData_ver2_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (simCfgData_ver2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (simCfgData_ver2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = SIM_CFG_VER_2;
    break;

  case SIM_CFG_VER_3:
    pCfgV3 = (simCfgData_ver3_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (simCfgData_ver3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (simCfgData_ver3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = SIM_CFG_VER_3;
    break;

  case SIM_CFG_VER_4:
    pCfgV4 = (simCfgData_ver4_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (simCfgData_ver4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (simCfgData_ver4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = SIM_CFG_VER_4;
    break;

  case SIM_CFG_VER_5:
    pCfgV5 = (simCfgData_ver5_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (simCfgData_ver5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (simCfgData_ver5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = SIM_CFG_VER_5;
    break;

  case SIM_CFG_VER_6:
    pCfgV6 = (simCfgData_ver6_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (simCfgData_ver6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (simCfgData_ver6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = SIM_CFG_VER_6;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_SIM_COMPONENT_ID,
                                         SIM_CFG_FILENAME, pOldCfgBuf,
                                         oldCfgSize, pOldCfgCksum, oldCfgVer,
                                         L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_SIM_COMPONENT_ID,
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
    case SIM_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (simCfgData_ver3_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                  (L7_uint32) sizeof (simCfgData_ver3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != simMigrateConfigV2V3Convert(pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SIM_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case SIM_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (simCfgData_ver4_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                  (L7_uint32) sizeof (simCfgData_ver4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != simMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SIM_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case SIM_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (simCfgData_ver5_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                  (L7_uint32) sizeof (simCfgData_ver5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != simMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SIM_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case SIM_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (simCfgData_ver6_t *) osapiMalloc (L7_SIM_COMPONENT_ID,
                                                  (L7_uint32) sizeof (simCfgData_ver6_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_SUCCESS != simMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_SIM_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case SIM_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV6, sizeof (*pCfgCur));
      osapiFree (L7_SIM_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;
    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_SIM_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_SIM_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_SIM_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_SIM_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_SIM_COMPONENT_ID, pCfgV6);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_SIM_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    simBuildDefaultConfigData (SIM_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
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
simMigrateConfigV2V3Convert (simCfgData_ver2_t * pCfgV2, simCfgData_ver3_t * pCfgV3)
{
  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != SIM_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, SIM_CFG_VER_2);
    return L7_FAILURE;
  }

  simBuildDefaultConfigDataV3(pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV3->systemName, pCfgV2->systemName);
  MIGRATE_COPY_STRUCT (pCfgV3->systemLocation, pCfgV2->systemLocation);
  MIGRATE_COPY_STRUCT (pCfgV3->systemContact, pCfgV2->systemContact);
  MIGRATE_COPY_STRUCT (pCfgV3->servPortIpAddress, pCfgV2->servPortIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV3->servPortNetMask, pCfgV2->servPortNetMask);
  MIGRATE_COPY_STRUCT (pCfgV3->servPortGateway, pCfgV2->servPortGateway);
  MIGRATE_COPY_STRUCT (pCfgV3->systemIpAddress, pCfgV2->systemIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV3->systemNetMask, pCfgV2->systemNetMask);
  MIGRATE_COPY_STRUCT (pCfgV3->systemGateway, pCfgV2->systemGateway);
  MIGRATE_COPY_STRUCT (pCfgV3->systemBIA, pCfgV2->systemBIA);
  MIGRATE_COPY_STRUCT (pCfgV3->systemLAA, pCfgV2->systemLAA);
  MIGRATE_COPY_STRUCT (pCfgV3->systemMacType, pCfgV2->systemMacType);
  MIGRATE_COPY_STRUCT (pCfgV3->systemConfigMode, pCfgV2->systemConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV3->servPortConfigMode, pCfgV2->servPortConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV3->systemBaudRate, pCfgV2->systemBaudRate);
  MIGRATE_COPY_STRUCT (pCfgV3->serialPortTimeOut, pCfgV2->serialPortTimeOut);
  MIGRATE_COPY_STRUCT (pCfgV3->serialPortStopBits, pCfgV2->serialPortStopBits);
  MIGRATE_COPY_STRUCT (pCfgV3->serialPortParityType, pCfgV2->serialPortParityType);
  MIGRATE_COPY_STRUCT (pCfgV3->serialPortFlowControl, pCfgV2->serialPortFlowControl);
  MIGRATE_COPY_STRUCT (pCfgV3->serialPortCharSize, pCfgV2->serialPortCharSize);
  MIGRATE_COPY_STRUCT (pCfgV3->systemTransferMode, pCfgV2->systemTransferMode);
  MIGRATE_COPY_STRUCT (pCfgV3->systemTransferServerIp, pCfgV2->systemTransferServerIp);
  MIGRATE_COPY_STRUCT (pCfgV3->systemTransferFilePath, pCfgV2->systemTransferFilePath);
  MIGRATE_COPY_STRUCT (pCfgV3->systemTransferFileName, pCfgV2->systemTransferFileName);
  MIGRATE_COPY_STRUCT (pCfgV3->systemUploadFileType, pCfgV2->systemUploadFileType);
  MIGRATE_COPY_STRUCT (pCfgV3->systemDownloadFileType, pCfgV2->systemDownloadFileType);
  MIGRATE_COPY_STRUCT (pCfgV3->systemUnitID, pCfgV2->systemUnitID);

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
simMigrateConfigV3V4Convert (simCfgData_ver3_t * pCfgV3, simCfgData_ver4_t * pCfgV4)
{
  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != SIM_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, SIM_CFG_VER_3);
    return L7_FAILURE;
  }

  simBuildDefaultConfigDataV4(pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV4->systemName, pCfgV3->systemName);
  MIGRATE_COPY_STRUCT (pCfgV4->systemLocation, pCfgV3->systemLocation);
  MIGRATE_COPY_STRUCT (pCfgV4->systemContact, pCfgV3->systemContact);
  MIGRATE_COPY_STRUCT (pCfgV4->servPortIpAddress, pCfgV3->servPortIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV4->servPortNetMask, pCfgV3->servPortNetMask);
  MIGRATE_COPY_STRUCT (pCfgV4->servPortGateway, pCfgV3->servPortGateway);
  MIGRATE_COPY_STRUCT (pCfgV4->systemIpAddress, pCfgV3->systemIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV4->systemNetMask, pCfgV3->systemNetMask);
  MIGRATE_COPY_STRUCT (pCfgV4->systemGateway, pCfgV3->systemGateway);
  MIGRATE_COPY_STRUCT (pCfgV4->systemBIA, pCfgV3->systemBIA);
  MIGRATE_COPY_STRUCT (pCfgV4->systemLAA, pCfgV3->systemLAA);
  MIGRATE_COPY_STRUCT (pCfgV4->systemMacType, pCfgV3->systemMacType);
  MIGRATE_COPY_STRUCT (pCfgV4->systemConfigMode, pCfgV3->systemConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV4->servPortConfigMode, pCfgV3->servPortConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV4->systemBaudRate, pCfgV3->systemBaudRate);
  MIGRATE_COPY_STRUCT (pCfgV4->serialPortTimeOut, pCfgV3->serialPortTimeOut);
  MIGRATE_COPY_STRUCT (pCfgV4->serialPortStopBits, pCfgV3->serialPortStopBits);
  MIGRATE_COPY_STRUCT (pCfgV4->serialPortParityType, pCfgV3->serialPortParityType);
  MIGRATE_COPY_STRUCT (pCfgV4->serialPortFlowControl, pCfgV3->serialPortFlowControl);
  MIGRATE_COPY_STRUCT (pCfgV4->serialPortCharSize, pCfgV3->serialPortCharSize);
  MIGRATE_COPY_STRUCT (pCfgV4->systemMgmtVlanId, pCfgV3->systemMgmtVlanId);
  MIGRATE_COPY_STRUCT (pCfgV4->systemUnitID, pCfgV3->systemUnitID);

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
static L7_RC_t
simMigrateConfigV4V5Convert (simCfgData_ver4_t * pCfgV4, simCfgData_ver5_t * pCfgV5)
{
  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != SIM_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, SIM_CFG_VER_4);
    return L7_FAILURE;
  }

  simBuildDefaultConfigDataV5(pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV5->systemName, pCfgV4->systemName);
  MIGRATE_COPY_STRUCT (pCfgV5->systemLocation, pCfgV4->systemLocation);
  MIGRATE_COPY_STRUCT (pCfgV5->systemContact, pCfgV4->systemContact);
  MIGRATE_COPY_STRUCT (pCfgV5->servPortIpAddress, pCfgV4->servPortIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV5->servPortNetMask, pCfgV4->servPortNetMask);
  MIGRATE_COPY_STRUCT (pCfgV5->servPortGateway, pCfgV4->servPortGateway);
  MIGRATE_COPY_STRUCT (pCfgV5->systemIpAddress, pCfgV4->systemIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV5->systemNetMask, pCfgV4->systemNetMask);
  MIGRATE_COPY_STRUCT (pCfgV5->systemGateway, pCfgV4->systemGateway);
  MIGRATE_COPY_STRUCT (pCfgV5->systemBIA, pCfgV4->systemBIA);
  MIGRATE_COPY_STRUCT (pCfgV5->systemLAA, pCfgV4->systemLAA);
  MIGRATE_COPY_STRUCT (pCfgV5->systemMacType, pCfgV4->systemMacType);
  MIGRATE_COPY_STRUCT (pCfgV5->systemConfigMode, pCfgV4->systemConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV5->servPortConfigMode, pCfgV4->servPortConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV5->systemBaudRate, pCfgV4->systemBaudRate);
  MIGRATE_COPY_STRUCT (pCfgV5->serialPortTimeOut, pCfgV4->serialPortTimeOut);
  MIGRATE_COPY_STRUCT (pCfgV5->serialPortStopBits, pCfgV4->serialPortStopBits);
  MIGRATE_COPY_STRUCT (pCfgV5->serialPortParityType, pCfgV4->serialPortParityType);
  MIGRATE_COPY_STRUCT (pCfgV5->serialPortFlowControl, pCfgV4->serialPortFlowControl);
  MIGRATE_COPY_STRUCT (pCfgV5->serialPortCharSize, pCfgV4->serialPortCharSize);
  MIGRATE_COPY_STRUCT (pCfgV5->systemMgmtVlanId, pCfgV4->systemMgmtVlanId);
  MIGRATE_COPY_STRUCT (pCfgV5->systemUnitID, pCfgV4->systemUnitID);

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
simMigrateConfigV5V6Convert (simCfgData_ver5_t * pCfgV5, simCfgData_ver6_t * pCfgV6)
{
  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != SIM_CFG_VER_5)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, SIM_CFG_VER_5);
    return L7_FAILURE;
  }

  simBuildDefaultConfigDataV6(pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  osapiStrncpySafe (pCfgV6->systemName, pCfgV5->systemName, L7_SYS_SIZE_32);
  osapiStrncpySafe (pCfgV6->systemLocation, pCfgV5->systemLocation, L7_SYS_SIZE_32);
  osapiStrncpySafe (pCfgV6->systemContact, pCfgV5->systemContact, L7_SYS_SIZE_32);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortIpAddress, pCfgV5->servPortIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortNetMask, pCfgV5->servPortNetMask);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortGateway, pCfgV5->servPortGateway);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortIpv6Intfs, pCfgV5->servPortIpv6Intfs);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortIpv6Gateway, pCfgV5->servPortIpv6Gateway);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortIpv6AdminMode, pCfgV5->servPortIpv6AdminMode);
  MIGRATE_COPY_STRUCT (pCfgV6->systemIpAddress, pCfgV5->systemIpAddress);
  MIGRATE_COPY_STRUCT (pCfgV6->systemNetMask, pCfgV5->systemNetMask);
  MIGRATE_COPY_STRUCT (pCfgV6->systemGateway, pCfgV5->systemGateway);
  MIGRATE_COPY_STRUCT (pCfgV6->systemIpv6Intfs, pCfgV5->systemIpv6Intfs);
  MIGRATE_COPY_STRUCT (pCfgV6->systemIpv6Gateway, pCfgV5->systemIpv6Gateway);
  MIGRATE_COPY_STRUCT (pCfgV6->systemIpv6AdminMode, pCfgV5->systemIpv6AdminMode);
  MIGRATE_COPY_STRUCT (pCfgV6->systemLAA, pCfgV5->systemLAA);
  MIGRATE_COPY_STRUCT (pCfgV6->systemMacType, pCfgV5->systemMacType);
  MIGRATE_COPY_STRUCT (pCfgV6->systemConfigMode, pCfgV5->systemConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV6->servPortConfigMode, pCfgV5->servPortConfigMode);
  MIGRATE_COPY_STRUCT (pCfgV6->systemBaudRate, pCfgV5->systemBaudRate);
  MIGRATE_COPY_STRUCT (pCfgV6->serialPortTimeOut, pCfgV5->serialPortTimeOut);
  MIGRATE_COPY_STRUCT (pCfgV6->serialPortStopBits, pCfgV5->serialPortStopBits);
  MIGRATE_COPY_STRUCT (pCfgV6->serialPortParityType, pCfgV5->serialPortParityType);
  MIGRATE_COPY_STRUCT (pCfgV6->serialPortFlowControl, pCfgV5->serialPortFlowControl);
  MIGRATE_COPY_STRUCT (pCfgV6->serialPortCharSize, pCfgV5->serialPortCharSize);
  MIGRATE_COPY_STRUCT (pCfgV6->systemTransferUnitNumber, pCfgV5->systemTransferUnitNumber);
  MIGRATE_COPY_STRUCT (pCfgV6->systemMgmtVlanId, pCfgV5->systemMgmtVlanId);
  MIGRATE_COPY_STRUCT (pCfgV6->systemUnitID, pCfgV5->systemUnitID);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build version 3 default system config data
*
* @param    pCfgV3   @{(input)}  Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void simBuildDefaultConfigDataV3(simCfgData_ver3_t *pCfgV3)
{
  memset(( void * )pCfgV3, 0, sizeof(simCfgData_ver3_t));

  strcpy(pCfgV3->cfgHdr.filename, SIM_CFG_FILENAME);
  pCfgV3->cfgHdr.version = SIM_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_SIM_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof(simCfgData_ver3_t);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  strcpy(pCfgV3->systemName, "");
  strcpy(pCfgV3->systemLocation, "");
  strcpy(pCfgV3->systemContact, "");

  pCfgV3->servPortIpAddress = 0;
  pCfgV3->servPortNetMask = 0;
  pCfgV3->servPortGateway = 0;
  pCfgV3->systemIpAddress = 0;
  pCfgV3->systemNetMask = 0;
  pCfgV3->systemGateway = 0;

  strcpy(pCfgV3->systemLAA, "");

  pCfgV3->systemBaudRate        = FD_SIM_DEFAULT_SYSTEM_BAUD_RATE;
  pCfgV3->serialPortStopBits    = FD_SIM_DEFAULT_SERIAL_PORT_STOPBITS;
  pCfgV3->serialPortParityType  = FD_SIM_DEFAULT_SERIAL_PORT_PARITY_TYPE;
  pCfgV3->serialPortFlowControl = FD_SIM_DEFAULT_SERIAL_PORT_FLOW_CONTROL;
  pCfgV3->serialPortCharSize    = FD_SIM_DEFAULT_SERIAL_PORT_CHAR_SIZE;
  pCfgV3->serialPortTimeOut = FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT;
  pCfgV3->systemMacType = FD_SIM_MACTYPE;
  pCfgV3->systemConfigMode = FD_SIM_NETWORK_CONFIG_MODE;

  if (SERVICE_PORT_PRESENT)
  {
    pCfgV3->servPortConfigMode = FD_SIM_SERVPORT_CONFIG_MODE;
  }
  else
    pCfgV3->servPortConfigMode = L7_SYSCONFIG_MODE_NONE;

  pCfgV3->systemUnitID        = FD_SIM_DEFAULT_STACK_ID;

  pCfgV3->systemTransferMode     =  FD_SIM_DEFAULT_TRANSFER_MODE;
  pCfgV3->systemTransferServerIp =  FD_SIM_DEFAULT_TRANSFER_SERVER_IP;
  strcpy(pCfgV3->systemTransferFilePath, "");
  strcpy(pCfgV3->systemTransferFileName, "");
  pCfgV3->systemUploadFileType   =  FD_SIM_DEFAULT_UPLOAD_FILE_TYPE;
  pCfgV3->systemDownloadFileType =  FD_SIM_DEFAULT_DOWNLOAD_FILE_TYPE;

  strcpy(simTransferInfo.systemTransferFilePathLocal, "");
  strcpy(simTransferInfo.systemTransferFileNameLocal, "");
  strcpy(simTransferInfo.systemTransferFilePathRemote, "");
  strcpy(simTransferInfo.systemTransferFileNameRemote, "");

  pCfgV3->systemMgmtVlanId = FD_SIM_DEFAULT_MGMT_VLAN_ID;
}

/*********************************************************************
* @purpose  Build version 4 default system config data
*
* @param    pCfgV4   @{(input)}  Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void simBuildDefaultConfigDataV4(simCfgData_ver4_t *pCfgV4)
{
  memset(( void * )pCfgV4, 0, sizeof(simCfgData_ver4_t));

  strcpy(pCfgV4->cfgHdr.filename, SIM_CFG_FILENAME);
  pCfgV4->cfgHdr.version = SIM_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_SIM_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = sizeof(simCfgData_ver4_t);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;

  strcpy(pCfgV4->systemName, "");
  strcpy(pCfgV4->systemLocation, "");
  strcpy(pCfgV4->systemContact, "");

  pCfgV4->servPortIpAddress = 0;
  pCfgV4->servPortNetMask = 0;
  pCfgV4->servPortGateway = 0;
  pCfgV4->systemIpAddress = 0;
  pCfgV4->systemNetMask = 0;
  pCfgV4->systemGateway = 0;

  strcpy(pCfgV4->systemLAA, "");

  pCfgV4->systemBaudRate        = FD_SIM_DEFAULT_SYSTEM_BAUD_RATE;
  pCfgV4->serialPortStopBits    = FD_SIM_DEFAULT_SERIAL_PORT_STOPBITS;
  pCfgV4->serialPortParityType  = FD_SIM_DEFAULT_SERIAL_PORT_PARITY_TYPE;
  pCfgV4->serialPortFlowControl = FD_SIM_DEFAULT_SERIAL_PORT_FLOW_CONTROL;
  pCfgV4->serialPortCharSize    = FD_SIM_DEFAULT_SERIAL_PORT_CHAR_SIZE;
  pCfgV4->serialPortTimeOut = FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT;
  pCfgV4->systemMacType = FD_SIM_MACTYPE;
  pCfgV4->systemConfigMode = FD_SIM_NETWORK_CONFIG_MODE;

  if (SERVICE_PORT_PRESENT)
  {
    pCfgV4->servPortConfigMode = FD_SIM_SERVPORT_CONFIG_MODE;
  }
  else
    pCfgV4->servPortConfigMode = L7_SYSCONFIG_MODE_NONE;

  pCfgV4->systemUnitID        = FD_SIM_DEFAULT_STACK_ID;

  /* setting default values for the non-configuration transfer attributes */
  simTransferInfo.systemTransferMode            =  FD_SIM_DEFAULT_TRANSFER_MODE;
  simTransferInfo.systemUploadFileType          =  FD_SIM_DEFAULT_UPLOAD_FILE_TYPE;
  simTransferInfo.systemDownloadFileType        =  FD_SIM_DEFAULT_DOWNLOAD_FILE_TYPE;
  simTransferInfo.systemTransferServerIp.family =  FD_SIM_DEFAULT_TRANSFER_SERVER_ADDR_TYPE;
  memset((L7_uchar8 *)&(simTransferInfo.systemTransferServerIp.addr), 0,
         sizeof(simTransferInfo.systemTransferServerIp.addr));

  strcpy(simTransferInfo.systemTransferFilePathLocal, "");
  strcpy(simTransferInfo.systemTransferFileNameLocal, "");
  strcpy(simTransferInfo.systemTransferFilePathRemote, "");
  strcpy(simTransferInfo.systemTransferFileNameRemote, "");

  pCfgV4->systemMgmtVlanId = FD_SIM_DEFAULT_MGMT_VLAN_ID;
}

/*********************************************************************
* @purpose  Build version 5 default system config data
*
* @param    pCfgV5   @{(input)}  Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void simBuildDefaultConfigDataV5(simCfgData_ver5_t *pCfgV5)
{
  memset(( void * )pCfgV5,
         0,
         sizeof( simCfgData_ver5_t));

  strcpy(pCfgV5->cfgHdr.filename, SIM_CFG_FILENAME);
  pCfgV5->cfgHdr.version = SIM_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_SIM_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = sizeof(simCfgData_ver5_t);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;
  strcpy(pCfgV5->systemName, "");
  strcpy(pCfgV5->systemLocation, "");
  strcpy(pCfgV5->systemContact, "");
  pCfgV5->servPortIpAddress = 0;
  pCfgV5->servPortNetMask = 0;
  pCfgV5->servPortGateway = 0;
  pCfgV5->servPortIpv6AdminMode = FD_SIM_DEFAULT_SERVPORT_IPV6_ADMIN_MODE;
  pCfgV5->systemIpAddress = 0;
  pCfgV5->systemNetMask = 0;
  pCfgV5->systemGateway = 0;
  pCfgV5->systemIpv6AdminMode = FD_SIM_DEFAULT_SYSTEM_IPV6_ADMIN_MODE;

  strcpy(pCfgV5->systemLAA, "");

  pCfgV5->systemBaudRate        = FD_SIM_DEFAULT_SYSTEM_BAUD_RATE;
  pCfgV5->serialPortStopBits    = FD_SIM_DEFAULT_SERIAL_PORT_STOPBITS;
  pCfgV5->serialPortParityType  = FD_SIM_DEFAULT_SERIAL_PORT_PARITY_TYPE;
  pCfgV5->serialPortFlowControl = FD_SIM_DEFAULT_SERIAL_PORT_FLOW_CONTROL;
  pCfgV5->serialPortCharSize    = FD_SIM_DEFAULT_SERIAL_PORT_CHAR_SIZE;

  pCfgV5->serialPortTimeOut = FD_SIM_DEFAULT_SERIAL_PORT_TIMEOUT;
  pCfgV5->systemMacType = FD_SIM_MACTYPE;
  pCfgV5->systemConfigMode = FD_SIM_NETWORK_CONFIG_MODE;
  if (SERVICE_PORT_PRESENT)
  {
    pCfgV5->servPortConfigMode = FD_SIM_SERVPORT_CONFIG_MODE;
  }
  else
    pCfgV5->servPortConfigMode = L7_SYSCONFIG_MODE_NONE;

  pCfgV5->systemUnitID        = FD_SIM_DEFAULT_STACK_ID;

  /* setting default values for the non-configuration transfer attributes */
  simTransferInfo.systemTransferMode            =  FD_SIM_DEFAULT_TRANSFER_MODE;
  simTransferInfo.systemUploadFileType          =  FD_SIM_DEFAULT_UPLOAD_FILE_TYPE;
  simTransferInfo.systemDownloadFileType        =  FD_SIM_DEFAULT_DOWNLOAD_FILE_TYPE;
  simTransferInfo.systemTransferServerIp.family =  FD_SIM_DEFAULT_TRANSFER_SERVER_ADDR_TYPE;
  memset((L7_uchar8 *)&(simTransferInfo.systemTransferServerIp.addr), 0,
         sizeof(simTransferInfo.systemTransferServerIp.addr));

  strcpy(simTransferInfo.systemTransferFilePathLocal, "");
  strcpy(simTransferInfo.systemTransferFileNameLocal, "");
  strcpy(simTransferInfo.systemTransferFilePathRemote, "");
  strcpy(simTransferInfo.systemTransferFileNameRemote, "");

  pCfgV5->systemMgmtVlanId = FD_SIM_DEFAULT_MGMT_VLAN_ID;
}

/*********************************************************************
* @purpose  Build version 6 default system config data
*
* @param    pCfgV6   @{(input)}  Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void simBuildDefaultConfigDataV6(simCfgData_ver6_t *pCfgV6)
{
  memset(( void * )pCfgCur, 0, sizeof(simCfgData_ver6_t));
  simBuildDefaultConfigData(SIM_CFG_VER_6);
  memcpy((L7_uchar8 *)pCfgV6, (L7_uchar8 *)pCfgCur, sizeof(simCfgData_ver6_t));
}

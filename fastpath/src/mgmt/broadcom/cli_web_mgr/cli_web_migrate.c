
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2004-2007
 *
 **********************************************************************
 * @filename cli_web_migrate.c
 *
 * @purpose Cli Web Manager Configuration Migration
 *
 * @component cliWeb
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

#include "cli_web_migrate.h"
#include "osapi.h"

static L7_RC_t cliWebMigrateConfigV2V3Convert (cliWebCfgDataV2_t * pCfgV2,
    cliWebCfgDataV3_t * pCfgV3);
static L7_RC_t cliWebMigrateConfigV3V4Convert (cliWebCfgDataV3_t * pCfgV3,
    cliWebCfgDataV4_t * pCfgV4);
static L7_RC_t cliWebMigrateConfigV4V5Convert (cliWebCfgDataV4_t * pCfgV4,
    cliWebCfgDataV5_t * pCfgV5);
static L7_RC_t cliWebMigrateConfigV5V6Convert (cliWebCfgDataV5_t * pCfgV5,
    cliWebCfgDataV6_t * pCfgV6);
static void cliWebBuildDefaultConfigDataV3 (cliWebCfgDataV3_t * pCfgV3);
static void cliWebBuildDefaultConfigDataV4 (cliWebCfgDataV4_t * pCfgV4);
static void cliWebBuildDefaultConfigDataV5 (cliWebCfgDataV5_t * pCfgV5);
static void cliWebBuildDefaultConfigDataV6 (cliWebCfgDataV6_t * pCfgV6);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static cliWebCfgData_t *pCfgCur = L7_NULLPTR;

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
cliWebMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  cliWebCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  cliWebCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  cliWebCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  cliWebCfgDataV5_t *pCfgV5 = L7_NULLPTR;
  cliWebCfgDataV6_t *pCfgV6 = L7_NULLPTR;

  pCfgCur = (cliWebCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
    case CLI_WEB_CFG_VER_2:
      pCfgV2 = (cliWebCfgDataV2_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
      (L7_uint32) sizeof (cliWebCfgDataV2_t));
      if (L7_NULLPTR == pCfgV2)
      {
    buildDefault = L7_TRUE;
    break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV2;
      oldCfgSize = (L7_uint32) sizeof (cliWebCfgDataV2_t);
      pOldCfgCksum = &pCfgV2->checkSum;
      oldCfgVer = CLI_WEB_CFG_VER_2;
      break;

    case CLI_WEB_CFG_VER_3:
      pCfgV3 = (cliWebCfgDataV3_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
      (L7_uint32) sizeof (cliWebCfgDataV3_t));
      if (L7_NULLPTR == pCfgV3)
      {
    buildDefault = L7_TRUE;
    break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV3;
      oldCfgSize = (L7_uint32) sizeof (cliWebCfgDataV3_t);
      pOldCfgCksum = &pCfgV3->checkSum;
      oldCfgVer = CLI_WEB_CFG_VER_3;
      break;

    case CLI_WEB_CFG_VER_4:
      pCfgV4 = (cliWebCfgDataV4_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
      (L7_uint32) sizeof (cliWebCfgDataV4_t));
      if (L7_NULLPTR == pCfgV4)
      {
    buildDefault = L7_TRUE;
    break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV4;
      oldCfgSize = (L7_uint32) sizeof (cliWebCfgDataV4_t);
      pOldCfgCksum = &pCfgV4->checkSum;
      oldCfgVer = CLI_WEB_CFG_VER_4;
      break;

    case CLI_WEB_CFG_VER_5:
      pCfgV5 = (cliWebCfgDataV5_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
      (L7_uint32) sizeof (cliWebCfgDataV5_t));
      if (L7_NULLPTR == pCfgV5)
      {
    buildDefault = L7_TRUE;
    break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV5;
      oldCfgSize = (L7_uint32) sizeof (cliWebCfgDataV5_t);
      pOldCfgCksum = &pCfgV5->checkSum;
      oldCfgVer = CLI_WEB_CFG_VER_5;
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
    (L7_SUCCESS != sysapiCfgFileGet (L7_CLI_WEB_COMPONENT_ID,
                     CLI_WEB_CFG_FILENAME,
                     pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                     oldCfgVer, L7_NULL /* do not build defaults */ ,
                     L7_NULL        /* do not call migrate again */
                    )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID,
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
      case CLI_WEB_CFG_VER_2:
    /* pCfgV2 has already been allocated and filled in */
    pCfgV3 = (cliWebCfgDataV3_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
        (L7_uint32) sizeof (cliWebCfgDataV3_t));
    if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != cliWebMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
    {
      buildDefault = L7_TRUE;
      break;
    }
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV2);
    pCfgV2 = L7_NULLPTR;
    /*passthru */

      case CLI_WEB_CFG_VER_3:
    /* pCfgV3 has already been allocated and filled in */
    pCfgV4 = (cliWebCfgDataV4_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
        (L7_uint32) sizeof (cliWebCfgDataV4_t));
    if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != cliWebMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
    {
      buildDefault = L7_TRUE;
      break;
    }
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV3);
    pCfgV3 = L7_NULLPTR;
    /*passthru */

      case CLI_WEB_CFG_VER_4:
    /* pCfgV4 has already been allocated and filled in */
    pCfgV5 = (cliWebCfgDataV5_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
        (L7_uint32) sizeof (cliWebCfgDataV5_t));
    if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != cliWebMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
    {
      buildDefault = L7_TRUE;
      break;
    }
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV4);
    pCfgV4 = L7_NULLPTR;
    /*passthru */

      case CLI_WEB_CFG_VER_5:
    /* pCfgV5 has already been allocated and filled in */
    pCfgV6 = (cliWebCfgDataV6_t *) osapiMalloc (L7_CLI_WEB_COMPONENT_ID,
        (L7_uint32) sizeof (cliWebCfgDataV6_t));
    if ((L7_NULLPTR == pCfgV6) || (L7_SUCCESS != cliWebMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
    {
      buildDefault = L7_TRUE;
      break;
    }
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV5);
    pCfgV5 = L7_NULLPTR;
    /*passthru */

      case CLI_WEB_CFG_VER_6:
    /* pCfgV6 has already been allocated and filled in */
    memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV6, sizeof (*pCfgCur));
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV6);
    pCfgV6 = L7_NULLPTR;

    /* done with migration - Flag unsaved configuration */
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
    break;
    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_CLI_WEB_COMPONENT_ID, pCfgV6);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CLI_WEB_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    cliWebBuildDefaultConfigData (CLI_WEB_CFG_VER_CURRENT);
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
cliWebMigrateConfigV2V3Convert (cliWebCfgDataV2_t * pCfgV2, cliWebCfgDataV3_t * pCfgV3)
{
  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != CLI_WEB_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, CLI_WEB_CFG_VER_2);
    return L7_FAILURE;
  }

  cliWebBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->systemWebMode = pCfgV2->systemWebMode;
  pCfgV3->systemJavaMode = pCfgV2->systemJavaMode;
  strcpy (pCfgV3->systemCommandPrompt, pCfgV2->systemCommandPrompt);
  pCfgV3->systemTelnetNewSessions = pCfgV2->systemTelnetNewSessions;
  pCfgV3->systemTelnetNumSessions = pCfgV2->systemTelnetNumSessions;

  if (pCfgV2->systemTelnetTimeout == 0) /* We now require a finite timeout */
    pCfgV3->systemTelnetTimeout = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  else
    pCfgV3->systemTelnetTimeout = pCfgV2->systemTelnetTimeout;

  pCfgV3->userPrompt = pCfgV2->userPrompt;

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
cliWebMigrateConfigV3V4Convert (cliWebCfgDataV3_t * pCfgV3, cliWebCfgDataV4_t * pCfgV4)
{
  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != CLI_WEB_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, CLI_WEB_CFG_VER_3);
    return L7_FAILURE;
  }

  cliWebBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->systemWebMode = pCfgV3->systemWebMode;
  pCfgV4->systemJavaMode = pCfgV3->systemJavaMode;
  pCfgV4->systemTelnetAdminMode = FD_CLI_WEB_TELNET_DEFAULT_ADMIN_MODE;
  strcpy (pCfgV4->systemCommandPrompt, pCfgV3->systemCommandPrompt);
  pCfgV4->systemTelnetNewSessions = pCfgV3->systemTelnetNewSessions;
  pCfgV4->systemTelnetNumSessions = pCfgV3->systemTelnetNumSessions;
  pCfgV4->systemTelnetTimeout = pCfgV3->systemTelnetTimeout;
  pCfgV4->userPrompt = pCfgV3->userPrompt;
  strcpy (pCfgV4->cliBanner, pCfgV3->cliBanner);

  return L7_SUCCESS;
}

/*********************************************************************
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
cliWebMigrateConfigV4V5Convert (cliWebCfgDataV4_t * pCfgV4, cliWebCfgDataV5_t * pCfgV5)
{
  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != CLI_WEB_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, CLI_WEB_CFG_VER_4);
    return L7_FAILURE;
  }

  cliWebBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->systemWebMode = pCfgV4->systemWebMode;
  pCfgV5->systemJavaMode = pCfgV4->systemJavaMode;
  pCfgV5->systemTelnetAdminMode = pCfgV4->systemTelnetAdminMode;
  strcpy (pCfgV5->systemCommandPrompt, pCfgV4->systemCommandPrompt);
  pCfgV5->systemTelnetNewSessions = pCfgV4->systemTelnetNewSessions;
  pCfgV5->systemTelnetNumSessions = pCfgV4->systemTelnetNumSessions;
  pCfgV5->systemTelnetTimeout = pCfgV4->systemTelnetTimeout;
  pCfgV5->userPrompt = pCfgV4->userPrompt;
  strcpy (pCfgV5->cliBanner, pCfgV4->cliBanner);
  return L7_SUCCESS;
}

/*********************************************************************
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
cliWebMigrateConfigV5V6Convert (cliWebCfgDataV5_t * pCfgV5, cliWebCfgDataV6_t * pCfgV6)
{
  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != CLI_WEB_CFG_VER_5)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, CLI_WEB_CFG_VER_5);
    return L7_FAILURE;
  }

  cliWebBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV6->systemWebMode = pCfgV5->systemWebMode;
  pCfgV6->systemJavaMode = pCfgV5->systemJavaMode;
  pCfgV6->systemTelnetAdminMode = pCfgV5->systemTelnetAdminMode;
  strcpy (pCfgV6->systemCommandPrompt, pCfgV5->systemCommandPrompt);
  pCfgV6->systemTelnetNewSessions = pCfgV5->systemTelnetNewSessions;
  pCfgV6->systemTelnetNumSessions = pCfgV5->systemTelnetNumSessions;
  pCfgV6->systemTelnetTimeout = pCfgV5->systemTelnetTimeout;
  pCfgV6->userPrompt = pCfgV5->userPrompt;
  strcpy (pCfgV6->cliBanner, pCfgV5->cliBanner);
  pCfgV6->webSessionHardTimeOut = pCfgV5->webSessionHardTimeOut;
  pCfgV6->webSessionSoftTimeOut = pCfgV5->webSessionSoftTimeOut;
  pCfgV6->webNumSessions = pCfgV5->webNumSessions;
  pCfgV6->linesForPagination = FD_CLI_LINES_FOR_PAGINATION;
  return L7_SUCCESS;
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
cliWebBuildDefaultConfigDataV3 (cliWebCfgDataV3_t * pCfgV3)
{
  memset(pCfgV3, 0, sizeof( *pCfgV3));
  strcpy(pCfgV3->cfgHdr.filename, CLI_WEB_CFG_FILENAME);
  pCfgV3->cfgHdr.version = CLI_WEB_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_CLI_WEB_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = sizeof(*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  pCfgV3->systemWebMode = FD_CLI_WEB_MODE;
  pCfgV3->userPrompt = L7_FALSE;
  strcpy(pCfgV3->systemCommandPrompt, FD_CLI_WEB_COMMAND_PROMPT);
  pCfgV3->systemTelnetNewSessions = FD_CLI_WEB_TELNET_NEW_SESSIONS;
  pCfgV3->systemTelnetNumSessions = FD_CLI_WEB_DEFAULT_NUM_SESSIONS;
  pCfgV3->systemTelnetTimeout = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  pCfgV3->systemJavaMode = FD_CLI_WEB_JAVA_MODE;
  memset(pCfgV3->cliBanner, 0, sizeof(pCfgV3->cliBanner));
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
cliWebBuildDefaultConfigDataV4 (cliWebCfgDataV4_t * pCfgV4)
{
  memset(pCfgV4, 0, sizeof( *pCfgV4));
  strcpy(pCfgV4->cfgHdr.filename, CLI_WEB_CFG_FILENAME);
  pCfgV4->cfgHdr.version = CLI_WEB_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_CLI_WEB_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = sizeof(*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;

  pCfgV4->systemWebMode = FD_CLI_WEB_MODE;
  pCfgV4->userPrompt = L7_FALSE;
  strcpy(pCfgV4->systemCommandPrompt, FD_CLI_WEB_COMMAND_PROMPT);
  pCfgV4->systemTelnetNewSessions = FD_CLI_WEB_TELNET_NEW_SESSIONS;
  pCfgV4->systemTelnetNumSessions = FD_CLI_WEB_DEFAULT_NUM_SESSIONS;
  pCfgV4->systemTelnetTimeout = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  pCfgV4->systemTelnetAdminMode = FD_CLI_WEB_TELNET_DEFAULT_ADMIN_MODE;
  pCfgV4->systemJavaMode = FD_CLI_WEB_JAVA_MODE;
  memset(pCfgV4->cliBanner, 0, sizeof(pCfgV4->cliBanner));
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
cliWebBuildDefaultConfigDataV5 (cliWebCfgDataV5_t * pCfgV5)
{
  memset(pCfgV5, 0, sizeof( *pCfgV5));
  strcpy(pCfgV5->cfgHdr.filename, CLI_WEB_CFG_FILENAME);
  pCfgV5->cfgHdr.version = CLI_WEB_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_CLI_WEB_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = sizeof(*pCfgV5);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;

  pCfgV5->systemWebMode = FD_CLI_WEB_MODE;
  pCfgV5->userPrompt = L7_FALSE;
  strcpy(pCfgV5->systemCommandPrompt, FD_CLI_WEB_COMMAND_PROMPT);
  pCfgV5->systemTelnetNewSessions = FD_CLI_WEB_TELNET_NEW_SESSIONS;
  pCfgV5->systemTelnetNumSessions = FD_CLI_WEB_DEFAULT_NUM_SESSIONS;
  pCfgV5->systemTelnetTimeout = FD_CLI_WEB_DEFAULT_REMOTECON_TIMEOUT;
  pCfgV5->systemTelnetAdminMode = FD_CLI_WEB_TELNET_DEFAULT_ADMIN_MODE;
  pCfgV5->systemJavaMode = FD_CLI_WEB_JAVA_MODE;
  memset(pCfgV5->cliBanner, 0, sizeof(pCfgV5->cliBanner));

  pCfgV5->webSessionHardTimeOut = FD_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
  pCfgV5->webSessionSoftTimeOut = FD_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
  pCfgV5->webNumSessions        = FD_HTTP_DEFAULT_MAX_CONNECTIONS;
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
cliWebBuildDefaultConfigDataV6 (cliWebCfgDataV6_t * pCfgV6)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (cliWebCfgDataV6_t));
  cliWebBuildDefaultConfigData (CLI_WEB_CFG_VER_6);
  memcpy ((L7_uchar8 *) pCfgV6, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}


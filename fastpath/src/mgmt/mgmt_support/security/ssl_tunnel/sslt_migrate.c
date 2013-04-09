
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  sslt_migrate.c
*
* @purpose   SSL Tunnel configuration Migration
*
* @component sslt
*
* @comments none
*
* @create    8/23/2004
*
* @author    Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#include "sslt_migrate.h"
#include "osapi.h"
#include "sslt_exports.h"

static L7_RC_t ssltMigrateConfigV2V3Convert (ssltCfgVer2_t * pCfgV2, ssltCfgVer3_t * pCfgV3);

static void ssltBuildDefaultConfigDataV3 (ssltCfgVer3_t * pCfgV3);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static ssltCfg_t *pCfgCur = L7_NULLPTR;

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
ssltMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL    buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32  oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32  oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  ssltCfgVer2_t *pCfgV2 = L7_NULLPTR;
  ssltCfgVer3_t *pCfgV3 = L7_NULLPTR;

  pCfgCur = (ssltCfg_t *) pCfgBuffer;

  ver = ver;

  switch (oldVer)
  {
    case SSLT_CFG_VER_1:
      pCfgV2 = (ssltCfgVer2_t *) osapiMalloc (L7_FLEX_SSLT_COMPONENT_ID,
                                                (L7_uint32) sizeof (ssltCfgVer2_t));
      if (L7_NULLPTR == pCfgV2)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV2;
      oldCfgSize = (L7_uint32) sizeof (ssltCfgVer2_t);
      pOldCfgCksum = &pCfgV2->checkSum;
      oldCfgVer = SSLT_CFG_VER_2;

      /* passthrough */
    case SSLT_CFG_VER_2:
      break;

    case SSLT_CFG_VER_3:
      pCfgV3 = (ssltCfgVer3_t *) osapiMalloc (L7_FLEX_SSLT_COMPONENT_ID,
                                                (L7_uint32) sizeof (ssltCfgVer3_t));
      if (L7_NULLPTR == pCfgV3)
      {
        buildDefault = L7_TRUE;
        break;
      }

      pOldCfgBuf = (L7_uchar8 *) pCfgV3;
      oldCfgSize = (L7_uint32) sizeof (ssltCfgVer3_t);
      pOldCfgCksum = &pCfgV3->checkSum;
      oldCfgVer = SSLT_CFG_VER_3;
      break;

    default:
      memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
      ssltBuildDefaultConfigData (SSLT_CFG_VER_CURRENT);
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_SSLT_COMPONENT_ID,
                                         SSLT_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
                                        )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSLT_COMPONENT_ID,
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
      case SSLT_CFG_VER_1:
      case SSLT_CFG_VER_2:
        /* pCfgV2 has already been allocated and filled in */
        pCfgV3 = (ssltCfgVer3_t *) osapiMalloc (L7_FLEX_SSLT_COMPONENT_ID, sizeof (ssltCfgVer3_t));
        if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != ssltMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_FLEX_SSLT_COMPONENT_ID, pCfgV2);
        pCfgV2 = L7_NULLPTR;
        /*passthru */

      case SSLT_CFG_VER_3:
        /* pCfgV3 has already been allocated and filled in */
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV3, sizeof (*pCfgCur));
        osapiFree (L7_FLEX_SSLT_COMPONENT_ID, pCfgV3);
        pCfgV3 = L7_NULLPTR;

        /* done with migration - Flag unsaved configuration */
        pCfgCur->hdr.dataChanged = L7_TRUE;
        break;

      default:
        buildDefault = L7_TRUE;
        break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_FLEX_SSLT_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_FLEX_SSLT_COMPONENT_ID, pCfgV3);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    ssltBuildDefaultConfigData (SSLT_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
  }

  return;
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
ssltMigrateConfigV2V3Convert (ssltCfgVer2_t * pCfgV2, ssltCfgVer3_t * pCfgV3)
{
  /* verify correct version of old config file */
  if (pCfgV2->hdr.version != SSLT_CFG_VER_2 &&
      pCfgV2->hdr.version != SSLT_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, SSLT_CFG_VER_2);
    return L7_FAILURE;
  }

  ssltBuildDefaultConfigDataV3(pCfgV3);

  /*---------------------------*/
  /* Migrate Data              */
  /*---------------------------*/

  pCfgV3->cfg.ssltAdminMode = pCfgV2->cfg.ssltAdminMode;
  pCfgV3->cfg.ssltSecurePort = pCfgV2->cfg.ssltSecurePort;
  pCfgV3->cfg.ssltUnSecurePort = pCfgV2->cfg.ssltUnSecurePort;
  pCfgV3->cfg.ssltUnSecureServerAddr = pCfgV2->cfg.ssltUnSecureServerAddr;
  strcpy(pCfgV3->cfg.ssltPemPassPhrase, pCfgV2->cfg.ssltPemPassPhrase);
  pCfgV3->cfg.ssltProtocolSSL30 = pCfgV2->cfg.ssltProtocolSSL30;
  pCfgV3->cfg.ssltProtocolTLS10 = pCfgV2->cfg.ssltProtocolTLS10;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Build Version 2 defaults.
*
* @param ver @b{(input)} version of configuration file for SSLT
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltBuildDefaultConfigDataV2 (ssltCfgVer2_t * pCfgV2)
{
  /* Setup file header */
  pCfgV2->hdr.version = SSLT_CFG_VER_2;
  pCfgV2->hdr.componentID = L7_FLEX_SSLT_COMPONENT_ID;
  pCfgV2->hdr.type = L7_CFG_DATA;
  pCfgV2->hdr.length = (L7_uint32)sizeof(*pCfgV2);

  strcpy((L7_char8 *)pCfgV2->hdr.filename, SSLT_CFG_FILENAME);
  pCfgV2->hdr.dataChanged = L7_FALSE;

  /* Zero overlay */
  bzero((L7_char8 *)&pCfgV2->cfg, sizeof(pCfgV2->cfg));

  /*
  ** Default values for the sslt configuration structure
  */
  pCfgV2->cfg.ssltAdminMode = L7_SSLT_ADMIN_MODE;

  pCfgV2->cfg.ssltSecurePort = L7_SSLT_SECURE_PORT;
  strcpy(pCfgV2->cfg.ssltPemPassPhrase, L7_SSLT_PEM_PASS_PHRASE);

  pCfgV2->cfg.ssltUnSecurePort = L7_SSLT_UNSECURE_PORT;
  pCfgV2->cfg.ssltUnSecureServerAddr = L7_SSLT_UNSECURE_SERVER_ADDR;

  pCfgV2->cfg.ssltProtocolSSL30 = L7_SSLT_SSL30_MODE;
  pCfgV2->cfg.ssltProtocolTLS10 = L7_SSLT_TLS10_MODE;

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
static void
ssltBuildDefaultConfigDataV3 (ssltCfgVer3_t * pCfgV3)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (ssltCfgVer3_t));
  ssltBuildDefaultConfigData (SSLT_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

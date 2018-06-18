
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename sshd_migrate.c
*
* @purpose SSHD Configuration Migration
*
* @component SSHD
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include <stdio.h>

#include "commdefs.h"
#include "datatypes.h"
#include "comm_structs.h"
#include "sysapi.h"
#include "osapi.h"
#include "log.h"
#include "sysapi_cfg_migrate.h"

#include "sshd_cfg.h"
#include "sshd_migrate.h"
#include "dtlapi.h"

static L7_RC_t sshdMigrateConfigV2V3Convert (sshdCfgV2_t * pCfgV2,
                                             sshdCfgV3_t * pCfgV3);
static void sshdBuildDefaultConfigDataV3 (sshdCfgV3_t * pCfgV3);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static sshdCfg_t *pCfgCur = L7_NULLPTR;

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
sshdMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  sshdCfgV2_t *pCfgV2 = L7_NULLPTR;
  sshdCfgV3_t *pCfgV3 = L7_NULLPTR;

  pCfgCur = (sshdCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */
  switch (oldVer)
  {
  case SSHD_CFG_VER_2:
    pCfgV2 = (sshdCfgV2_t *) osapiMalloc (L7_FLEX_SSHD_COMPONENT_ID,
                                                (L7_uint32) sizeof (sshdCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (sshdCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = SSHD_CFG_VER_2;
    break;

  case SSHD_CFG_VER_3:
    pCfgV3 = (sshdCfgV3_t *) osapiMalloc (L7_FLEX_SSHD_COMPONENT_ID,
                                                (L7_uint32) sizeof (sshdCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (sshdCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = SSHD_CFG_VER_3;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_SSHD_COMPONENT_ID,
                                         SSHD_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID,
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
    case SSHD_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (sshdCfgV3_t *) osapiMalloc (L7_FLEX_SSHD_COMPONENT_ID,
                                                  (L7_uint32) sizeof (sshdCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) ||
          ((pCfgV2 != NULL) && (L7_SUCCESS != sshdMigrateConfigV2V3Convert (pCfgV2, pCfgV3))))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_SSHD_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case SSHD_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      if (pCfgV3 != NULL)
      {
          memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV3, sizeof (*pCfgCur));
          osapiFree (L7_FLEX_SSHD_COMPONENT_ID, pCfgV3);
          pCfgV3 = L7_NULLPTR;

          /* done with migration - Flag unsaved configuration */
          pCfgCur->hdr.dataChanged = L7_TRUE;
      }
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_FLEX_SSHD_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_FLEX_SSHD_COMPONENT_ID, pCfgV3);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    sshdBuildDefaultConfigData (SSHD_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV3        @b{(input)} ptr to version 2 config data structure
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
sshdMigrateConfigV2V3Convert (sshdCfgV2_t * pCfgV2, sshdCfgV3_t * pCfgV3)
{
  /* verify correct version of old config file */
  if (pCfgV2->hdr.version != SSHD_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, SSHD_CFG_VER_2);
    return L7_FAILURE;
  }

  sshdBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */

  pCfgV3->cfg.sshdAdminMode = pCfgV2->cfg.sshdAdminMode;
  pCfgV3->cfg.sshdProtoLevel = pCfgV2->cfg.sshdProtoLevel;

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
sshdBuildDefaultConfigDataV3 (sshdCfgV3_t * pCfgV3)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (sshdCfgV3_t));
  sshdBuildDefaultConfigData (SSHD_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}


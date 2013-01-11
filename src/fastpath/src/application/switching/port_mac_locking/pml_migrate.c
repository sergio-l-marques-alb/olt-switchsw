
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pml_migrate.c
*
* @purpose PML Configuration Migration
*
* @component PML
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "pml_migrate.h"

static L7_RC_t pmlMigrateConfigV1V2Convert (pmlCfgDataV1_t * pCfgV1,
                                            pmlCfgDataV2_t * pCfgV2);

static void pmlBuildDefaultConfigDataV2 (pmlCfgDataV2_t * pCfgV2);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static pmlCfgData_t *pCfgCur = L7_NULLPTR;

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
void pmlMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  pmlCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  pmlCfgDataV2_t *pCfgV2 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (pmlCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case PML_CFG_VER_1:
    pCfgV1 = (pmlCfgDataV1_t *) osapiMalloc (L7_PORT_MACLOCK_COMPONENT_ID,
                                           (L7_uint32) sizeof (pmlCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (pmlCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = PML_CFG_VER_1;
    break;

  case PML_CFG_VER_2:
    pCfgV2 = (pmlCfgDataV2_t *) osapiMalloc (L7_PORT_MACLOCK_COMPONENT_ID,
                                           (L7_uint32) sizeof (pmlCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (pmlCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = PML_CFG_VER_2;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_PORT_MACLOCK_COMPONENT_ID,
                                         PML_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PORT_MACLOCK_COMPONENT_ID,
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
    case PML_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (pmlCfgDataV2_t *) osapiMalloc (L7_PORT_MACLOCK_COMPONENT_ID,
                                             (L7_uint32) sizeof (pmlCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) || (L7_SUCCESS != pmlMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PORT_MACLOCK_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case PML_CFG_VER_2:
      if (pCfgV2 != L7_NULLPTR)
      {
        /* pCfgV2 has already been allocated and filled in */
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV2, sizeof (*pCfgCur));
        osapiFree (L7_PORT_MACLOCK_COMPONENT_ID, pCfgV2);
      }

      pCfgV2 = L7_NULLPTR;

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
    osapiFree (L7_PORT_MACLOCK_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_PORT_MACLOCK_COMPONENT_ID, pCfgV2);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MACLOCK_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    pmlBuildDefaultConfigData (PML_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v1 to v2
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
pmlMigrateConfigV1V2Convert (pmlCfgDataV1_t * pCfgV1, pmlCfgDataV2_t * pCfgV2)
{
  L7_uint32 i, intfCount;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != PML_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, PML_CFG_VER_1);
    return L7_FAILURE;
  }

  pmlBuildDefaultConfigDataV2 (pCfgV2);

  /*
   * Map the fields in the older structure
   * to the appropriate fields in the newer one
   */

  pCfgV2->globalLockEnabled = pCfgV1->globalLockEnabled;

  intfCount = min (L7_PML_MAX_INTF_REL_4_3, L7_PML_MAX_INTF_REL_4_4);
  for (i = 0; i < intfCount; i++)
  {
    MIGRATE_COPY_STRUCT(pCfgV2->pmlIntfCfgData[i], pCfgV1->pmlIntfCfgData[i]);
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
pmlBuildDefaultConfigDataV2 (pmlCfgDataV2_t * pCfgV2)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (pmlCfgDataV2_t));
  pmlBuildDefaultConfigData (PML_CFG_VER_2);
  memcpy ((L7_uchar8 *) pCfgV2, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}


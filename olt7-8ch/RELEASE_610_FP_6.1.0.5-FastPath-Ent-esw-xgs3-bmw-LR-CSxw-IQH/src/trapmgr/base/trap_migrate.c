
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename trap_migrate.c
*
* @purpose TRAP Configuration Migration
*
* @component TRAP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "trap_migrate.h"

static L7_RC_t trapMigrateConfigV1V2Convert (trapMgrCfgDataV1_t * pCfgV1,
                                             trapMgrCfgDataV2_t * pCfgV2);

static L7_RC_t trapMigrateConfigV2V3Convert (trapMgrCfgDataV2_t * pCfgV2,
                                             trapMgrCfgDataV3_t * pCfgV3);

static L7_RC_t trapMigrateConfigV3V4Convert (trapMgrCfgDataV3_t * pCfgV3,
                                             trapMgrCfgDataV4_t * pCfgV4);

static L7_RC_t trapMigrateConfigV4V5Convert (trapMgrCfgDataV4_t * pCfgV4,
                                             trapMgrCfgDataV5_t * pCfgV5);

static L7_RC_t trapMigrateConfigV5V6Convert (trapMgrCfgDataV5_t * pCfgV5,
                                             trapMgrCfgDataV6_t * pCfgV6);

static L7_RC_t trapMigrateConfigV6V7Convert (trapMgrCfgDataV6_t * pCfgV6,
                                             trapMgrCfgDataV7_t * pCfgV7);

static void trapBuildDefaultConfigDataV2 (trapMgrCfgDataV2_t * pCfgV2);
static void trapBuildDefaultConfigDataV3 (trapMgrCfgDataV3_t * pCfgV3);
static void trapBuildDefaultConfigDataV4 (trapMgrCfgDataV4_t * pCfgV4);
static void trapBuildDefaultConfigDataV5 (trapMgrCfgDataV5_t * pCfgV5);
static void trapBuildDefaultConfigDataV6 (trapMgrCfgDataV6_t * pCfgV6);
static void trapBuildDefaultConfigDataV7 (trapMgrCfgDataV7_t * pCfgV7);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static trapMgrCfgData_t *pCfgCur = L7_NULLPTR;

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
trapMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  trapMgrCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  trapMgrCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  trapMgrCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  trapMgrCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  trapMgrCfgDataV5_t *pCfgV5 = L7_NULLPTR;
  trapMgrCfgDataV6_t *pCfgV6 = L7_NULLPTR;
  trapMgrCfgDataV7_t *pCfgV7 = L7_NULLPTR;

  pCfgCur = (trapMgrCfgData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, (L7_uint32) sizeof (*pCfgCur));

  if (pCfgHdr->version == TRAP_CFG_VER_1 &&
      pCfgHdr->length == (L7_uint32) sizeof (trapMgrCfgDataV2_t))
    oldVer = TRAP_CFG_VER_2;

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case TRAP_CFG_VER_1:
    pCfgV1 = (trapMgrCfgDataV1_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (trapMgrCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = TRAP_CFG_VER_1;
    break;

  case TRAP_CFG_VER_2:
    pCfgV2 = (trapMgrCfgDataV2_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (trapMgrCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = TRAP_CFG_VER_1; /* b'cos: REL_H version was not updated */
    break;

  case TRAP_CFG_VER_3:
    pCfgV3 = (trapMgrCfgDataV3_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (trapMgrCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = TRAP_CFG_VER_3;
    break;

  case TRAP_CFG_VER_4:
    pCfgV4 = (trapMgrCfgDataV4_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (trapMgrCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = TRAP_CFG_VER_4;
    break;


  case TRAP_CFG_VER_5:
    pCfgV5 = (trapMgrCfgDataV5_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (trapMgrCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = TRAP_CFG_VER_5;
    break;

  case TRAP_CFG_VER_6:
    pCfgV6 = (trapMgrCfgDataV6_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
        (L7_uint32) sizeof (trapMgrCfgDataV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = TRAP_CFG_VER_6;
    break;

  case TRAP_CFG_VER_7:
    pCfgV7 = (trapMgrCfgDataV7_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
        (L7_uint32) sizeof (trapMgrCfgDataV7_t));
    if (L7_NULLPTR == pCfgV7)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV7;
    oldCfgSize = (L7_uint32) sizeof (trapMgrCfgDataV7_t);
    pOldCfgCksum = &pCfgV7->checkSum;
    oldCfgVer = TRAP_CFG_VER_7;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_TRAPMGR_COMPONENT_ID,
                                         TRAP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_TRAPMGR_COMPONENT_ID,
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
    case TRAP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (trapMgrCfgDataV2_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (trapMgrCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != trapMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case TRAP_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (trapMgrCfgDataV3_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (trapMgrCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != trapMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case TRAP_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (trapMgrCfgDataV4_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (trapMgrCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != trapMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case TRAP_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (trapMgrCfgDataV5_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (trapMgrCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != trapMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case TRAP_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (trapMgrCfgDataV6_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
          (L7_uint32) sizeof (trapMgrCfgDataV6_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != trapMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case TRAP_CFG_VER_6:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV7 = (trapMgrCfgDataV7_t *) osapiMalloc (L7_TRAPMGR_COMPONENT_ID,
          (L7_uint32) sizeof (trapMgrCfgDataV7_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_SUCCESS != trapMigrateConfigV6V7Convert (pCfgV6, pCfgV7)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;
      /*passthru */

    case TRAP_CFG_VER_7:
      /* pCfgV7 has already been allocated and filled in */
      memcpy ((L7_char8 *) pCfgCur, (L7_char8 *) pCfgV7, sizeof (*pCfgCur));
      osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV7);
      pCfgV7 = L7_NULLPTR;

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
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV6);
  if (pCfgV7 != L7_NULLPTR)
    osapiFree (L7_TRAPMGR_COMPONENT_ID, pCfgV7);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    trapBuildDefaultConfigData (TRAP_CFG_VER_CURRENT);
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
trapMigrateConfigV1V2Convert (trapMgrCfgDataV1_t * pCfgV1, trapMgrCfgDataV2_t * pCfgV2)
{
  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != TRAP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, TRAP_CFG_VER_1);
    return L7_FAILURE;
  }

  trapBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->trapAuth = pCfgV1->trapAuth;
  pCfgV2->trapLink = pCfgV1->trapLink;
  pCfgV2->trapMultiUsers = pCfgV1->trapMultiUsers;
  pCfgV2->trapSpanningTree = pCfgV1->trapSpanningTree;
  pCfgV2->trapBcastStorm = pCfgV1->trapBcastStorm;
  pCfgV2->trapGeneralSystem = pCfgV1->trapGeneralSystem;
  pCfgV2->trapDot1q = pCfgV1->trapDot1q;
  pCfgV2->trapOspf = pCfgV1->trapOspf;
  pCfgV2->trapVrrp = pCfgV1->trapVrrp;
  pCfgV2->trapBgp = pCfgV1->trapBgp;
  pCfgV2->trapPim = pCfgV1->trapPim;
  pCfgV2->trapDvmrp = pCfgV1->trapDvmrp;

  /* This parameter is only present in base REL_G
     pCfgV2->trapPoe = pCfgV1->trapPoe;
   */

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
trapMigrateConfigV2V3Convert (trapMgrCfgDataV2_t * pCfgV2, trapMgrCfgDataV3_t * pCfgV3)
{
  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != TRAP_CFG_VER_2 && pCfgV2->cfgHdr.version != TRAP_CFG_VER_1)     /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, TRAP_CFG_VER_2);
    return L7_FAILURE;
  }

  trapBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->trapAuth = pCfgV2->trapAuth;
  pCfgV3->trapLink = pCfgV2->trapLink;
  pCfgV3->trapMultiUsers = pCfgV2->trapMultiUsers;
  pCfgV3->trapSpanningTree = pCfgV2->trapSpanningTree;
  pCfgV3->trapGeneralSystem = pCfgV2->trapGeneralSystem;
  pCfgV3->trapDot1q = pCfgV2->trapDot1q;
  pCfgV3->trapOspf = pCfgV2->trapOspf;
  pCfgV3->trapVrrp = pCfgV2->trapVrrp;
  pCfgV3->trapBgp = pCfgV2->trapBgp;
  pCfgV3->trapPim = pCfgV2->trapPim;
  pCfgV3->trapDvmrp = pCfgV2->trapDvmrp;
  pCfgV3->trapInventory = pCfgV2->trapInventory;
  pCfgV3->trapPoe = pCfgV2->trapPoe;

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
trapMigrateConfigV3V4Convert (trapMgrCfgDataV3_t * pCfgV3, trapMgrCfgDataV4_t * pCfgV4)
{
  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != TRAP_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, TRAP_CFG_VER_3);
    return L7_FAILURE;
  }

  trapBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->trapAuth = pCfgV3->trapAuth;
  pCfgV4->trapLink = pCfgV3->trapLink;
  pCfgV4->trapMultiUsers = pCfgV3->trapMultiUsers;
  pCfgV4->trapSpanningTree = pCfgV3->trapSpanningTree;
  pCfgV4->trapGeneralSystem = pCfgV3->trapGeneralSystem;
  pCfgV4->trapDot1q = pCfgV3->trapDot1q;
  pCfgV4->trapOspf = pCfgV3->trapOspf;
  pCfgV4->trapVrrp = pCfgV3->trapVrrp;
  pCfgV4->trapBgp = pCfgV3->trapBgp;
  pCfgV4->trapPim = pCfgV3->trapPim;
  pCfgV4->trapDvmrp = pCfgV3->trapDvmrp;
  pCfgV4->trapInventory = pCfgV3->trapInventory;
  pCfgV4->trapPoe = pCfgV3->trapPoe;
  pCfgV4->trapMacLockViolation = pCfgV3->trapMacLockViolation;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5.
*
* @param    pCfgV4    @b{(input)} ptr to version 3 config data structure
* @param    pCfgV5    @b{(input)} ptr to version 4 config data structure
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
trapMigrateConfigV4V5Convert (trapMgrCfgDataV4_t * pCfgV4, trapMgrCfgDataV5_t * pCfgV5)
{
  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != TRAP_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, TRAP_CFG_VER_4);
    return L7_FAILURE;
  }

  trapBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->trapAuth = pCfgV4->trapAuth;
  pCfgV5->trapLink = pCfgV4->trapLink;
  pCfgV5->trapMultiUsers = pCfgV4->trapMultiUsers;
  pCfgV5->trapSpanningTree = pCfgV4->trapSpanningTree;
  pCfgV5->trapGeneralSystem = pCfgV4->trapGeneralSystem;
  pCfgV5->trapDot1q = pCfgV4->trapDot1q;
  pCfgV5->trapOspf = pCfgV4->trapOspf;
  pCfgV5->trapVrrp = pCfgV4->trapVrrp;
  pCfgV5->trapBgp = pCfgV4->trapBgp;
  pCfgV5->trapPim = pCfgV4->trapPim;
  pCfgV5->trapDvmrp = pCfgV4->trapDvmrp;
  pCfgV5->trapInventory = pCfgV4->trapInventory;
  pCfgV5->trapPoe = pCfgV4->trapPoe;
  pCfgV5->trapMacLockViolation = pCfgV4->trapMacLockViolation;
  pCfgV5->trapAcl = pCfgV4->trapAcl;

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
trapMigrateConfigV5V6Convert (trapMgrCfgDataV5_t * pCfgV5, trapMgrCfgDataV6_t * pCfgV6)
{
  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != TRAP_CFG_VER_5)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, TRAP_CFG_VER_5);
    return L7_FAILURE;
  }

  trapBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV6->trapAuth = pCfgV5->trapAuth;
  pCfgV6->trapLink = pCfgV5->trapLink;
  pCfgV6->trapMultiUsers = pCfgV5->trapMultiUsers;
  pCfgV6->trapSpanningTree = pCfgV5->trapSpanningTree;
  pCfgV6->trapGeneralSystem = pCfgV5->trapGeneralSystem;
  pCfgV6->trapDot1q = pCfgV5->trapDot1q;
  pCfgV6->trapOspf = pCfgV5->trapOspf;
  pCfgV6->trapVrrp = pCfgV5->trapVrrp;
  pCfgV6->trapBgp = pCfgV5->trapBgp;
  pCfgV6->trapPim = pCfgV5->trapPim;
  pCfgV6->trapDvmrp = pCfgV5->trapDvmrp;
  pCfgV6->trapInventory = pCfgV5->trapInventory;
  pCfgV6->trapPoe = pCfgV5->trapPoe;
  pCfgV6->trapMacLockViolation = pCfgV5->trapMacLockViolation;
  pCfgV6->trapAcl = pCfgV5->trapAcl;
  pCfgV6->trapWireless = pCfgV5->trapWireless;

  pCfgV6->trapOspfv3 = FD_TRAP_OSPFV3_TRAP;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V6 to V7.
*
* @param    pCfgV6    @b{(input)} ptr to version 5 config data structure
* @param    pCfgV7    @b{(input)} ptr to version 6 config data structure
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
trapMigrateConfigV6V7Convert (trapMgrCfgDataV6_t * pCfgV6, trapMgrCfgDataV7_t * pCfgV7)
{
  /* verify correct version of old config file */
  if (pCfgV6->cfgHdr.version != TRAP_CFG_VER_6)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV6->cfgHdr.version, TRAP_CFG_VER_6);
    return L7_FAILURE;
  }

  trapBuildDefaultConfigDataV7 (pCfgV7);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV7->trapAuth = pCfgV6->trapAuth;
  pCfgV7->trapLink = pCfgV6->trapLink;
  pCfgV7->trapMultiUsers = pCfgV6->trapMultiUsers;
  pCfgV7->trapSpanningTree = pCfgV6->trapSpanningTree;
  pCfgV7->trapGeneralSystem = pCfgV6->trapGeneralSystem;
  pCfgV7->trapDot1q = pCfgV6->trapDot1q;
  pCfgV7->trapOspf = pCfgV6->trapOspf;
  pCfgV7->trapVrrp = pCfgV6->trapVrrp;
  pCfgV7->trapBgp = pCfgV6->trapBgp;
  pCfgV7->trapPim = pCfgV6->trapPim;
  pCfgV7->trapDvmrp = pCfgV6->trapDvmrp;
  pCfgV7->trapInventory = pCfgV6->trapInventory;
  pCfgV7->trapPoe = pCfgV6->trapPoe;
  pCfgV7->trapMacLockViolation = pCfgV6->trapMacLockViolation;
  pCfgV7->trapAcl = pCfgV6->trapAcl;
  pCfgV7->trapWireless = pCfgV6->trapWireless;
  pCfgV7->trapOspfv3 = pCfgV6->trapOspfv3;

  pCfgV7->trapCaptivePortal = FD_TRAP_CAPTIVE_PORTAL;
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
trapBuildDefaultConfigDataV2 (trapMgrCfgDataV2_t * pCfgV2)
{
  memset (pCfgV2, 0, sizeof (*pCfgV2));
  strcpy ((char *) pCfgV2->cfgHdr.filename, TRAP_CFG_FILENAME);
  pCfgV2->cfgHdr.version = TRAP_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_TRAPMGR_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
  pCfgV2->trapAuth = FD_TRAP_AUTH;
  pCfgV2->trapLink = FD_TRAP_LINK_STATUS;
  pCfgV2->trapMultiUsers = FD_TRAP_MULTI_USERS;
  pCfgV2->trapSpanningTree = FD_TRAP_SPAN_TREE;
  pCfgV2->trapBcastStorm = FD_TRAP_BCAST_STORM;
  pCfgV2->trapDot1q = FD_TRAP_DOT1Q;
  pCfgV2->trapOspf = FD_TRAP_OSPF_TRAP;
  pCfgV2->trapVrrp = FD_TRAP_VRRP;
  pCfgV2->trapPim = FD_TRAP_PIM;
  pCfgV2->trapDvmrp = FD_TRAP_DVMRP;
  pCfgV2->trapPoe = FD_TRAP_POE;
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
trapBuildDefaultConfigDataV3 (trapMgrCfgDataV3_t * pCfgV3)
{
  memset (pCfgV3, 0, sizeof (*pCfgV3));
  strcpy ((char *) pCfgV3->cfgHdr.filename, TRAP_CFG_FILENAME);
  pCfgV3->cfgHdr.version = TRAP_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_TRAPMGR_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32) sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;
  pCfgV3->trapAuth = FD_TRAP_AUTH;
  pCfgV3->trapLink = FD_TRAP_LINK_STATUS;
  pCfgV3->trapMultiUsers = FD_TRAP_MULTI_USERS;
  pCfgV3->trapSpanningTree = FD_TRAP_SPAN_TREE;
  pCfgV3->trapBcastStorm = FD_TRAP_BCAST_STORM;
  pCfgV3->trapDot1q = FD_TRAP_DOT1Q;
  pCfgV3->trapOspf = FD_TRAP_OSPF_TRAP;
  pCfgV3->trapVrrp = FD_TRAP_VRRP;
  pCfgV3->trapPim = FD_TRAP_PIM;
  pCfgV3->trapDvmrp = FD_TRAP_DVMRP;
  pCfgV3->trapPoe = FD_TRAP_POE;
  pCfgV3->trapMacLockViolation = FD_TRAP_MACLOCK_VIOLATION;
}

/*********************************************************************
* @purpose  Build default trap config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapBuildDefaultConfigDataV4(trapMgrCfgDataV4_t * pCfgV4)
{
  memset(pCfgV4, 0, sizeof(*pCfgV4));
  strcpy ((char *) pCfgV4->cfgHdr.filename, TRAP_CFG_FILENAME);
  pCfgV4->cfgHdr.version = TRAP_CFG_VER_4;
  pCfgV4->cfgHdr.componentID = L7_TRAPMGR_COMPONENT_ID;
  pCfgV4->cfgHdr.type = L7_CFG_DATA;
  pCfgV4->cfgHdr.length = (L7_uint32) sizeof (*pCfgV4);
  pCfgV4->cfgHdr.dataChanged = L7_FALSE;
  pCfgV4->trapAuth = FD_TRAP_AUTH;
  pCfgV4->trapLink = FD_TRAP_LINK_STATUS;
  pCfgV4->trapMultiUsers = FD_TRAP_MULTI_USERS;
  pCfgV4->trapSpanningTree = FD_TRAP_SPAN_TREE;
  pCfgV4->trapDot1q = FD_TRAP_DOT1Q;
  pCfgV4->trapOspf = FD_TRAP_OSPF_TRAP;
  pCfgV4->trapVrrp = FD_TRAP_VRRP;
  pCfgV4->trapPim = FD_TRAP_PIM;
  pCfgV4->trapDvmrp = FD_TRAP_DVMRP;
  pCfgV4->trapPoe = FD_TRAP_POE;
  pCfgV4->trapMacLockViolation = FD_TRAP_MACLOCK_VIOLATION;
  pCfgV4->trapAcl = FD_TRAP_ACL;
}

/*********************************************************************
* @purpose  Build default trap config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapBuildDefaultConfigDataV5(trapMgrCfgDataV5_t * pCfgV5)
{
  memset(pCfgV5, 0, sizeof(*pCfgV5));
  strcpy ((char *) pCfgV5->cfgHdr.filename, TRAP_CFG_FILENAME);
  pCfgV5->cfgHdr.version = TRAP_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_TRAPMGR_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = (L7_uint32) sizeof (*pCfgV5);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;
  pCfgV5->trapAuth = FD_TRAP_AUTH;
  pCfgV5->trapLink = FD_TRAP_LINK_STATUS;
  pCfgV5->trapMultiUsers = FD_TRAP_MULTI_USERS;
  pCfgV5->trapSpanningTree = FD_TRAP_SPAN_TREE;
  pCfgV5->trapDot1q = FD_TRAP_DOT1Q;
  pCfgV5->trapOspf = FD_TRAP_OSPF_TRAP;
  pCfgV5->trapVrrp = FD_TRAP_VRRP;
  pCfgV5->trapPim = FD_TRAP_PIM;
  pCfgV5->trapDvmrp = FD_TRAP_DVMRP;
  pCfgV5->trapPoe = FD_TRAP_POE;
  pCfgV5->trapMacLockViolation = FD_TRAP_MACLOCK_VIOLATION;
  pCfgV5->trapAcl = FD_TRAP_ACL;
  pCfgV5->trapWireless = FD_TRAP_WIRELESS;
}


/*********************************************************************
* @purpose  Build default trap config data
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void trapBuildDefaultConfigDataV6(trapMgrCfgDataV6_t * pCfgV6)
{
  memset(pCfgV6, 0, sizeof(*pCfgV6));
  strcpy ((char *) pCfgV6->cfgHdr.filename, TRAP_CFG_FILENAME);
  pCfgV6->cfgHdr.version = TRAP_CFG_VER_5;
  pCfgV6->cfgHdr.componentID = L7_TRAPMGR_COMPONENT_ID;
  pCfgV6->cfgHdr.type = L7_CFG_DATA;
  pCfgV6->cfgHdr.length = (L7_uint32) sizeof (*pCfgV6);
  pCfgV6->cfgHdr.dataChanged = L7_FALSE;
  pCfgV6->trapAuth = FD_TRAP_AUTH;
  pCfgV6->trapLink = FD_TRAP_LINK_STATUS;
  pCfgV6->trapMultiUsers = FD_TRAP_MULTI_USERS;
  pCfgV6->trapSpanningTree = FD_TRAP_SPAN_TREE;
  pCfgV6->trapDot1q = FD_TRAP_DOT1Q;
  pCfgV6->trapOspf = FD_TRAP_OSPF_TRAP;
  pCfgV6->trapVrrp = FD_TRAP_VRRP;
  pCfgV6->trapPim = FD_TRAP_PIM;
  pCfgV6->trapDvmrp = FD_TRAP_DVMRP;
  pCfgV6->trapPoe = FD_TRAP_POE;
  pCfgV6->trapMacLockViolation = FD_TRAP_MACLOCK_VIOLATION;
  pCfgV6->trapAcl = FD_TRAP_ACL;
  pCfgV6->trapWireless = FD_TRAP_WIRELESS;
  pCfgV6->trapOspfv3 = FD_TRAP_OSPFV3_TRAP;
}

/*********************************************************************
* @purpose  Build Version 6 defaults
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
trapBuildDefaultConfigDataV7 (trapMgrCfgDataV7_t * pCfgV7)
{
  memset ((L7_char8 *) pCfgCur, 0, sizeof (trapMgrCfgDataV7_t));
  trapBuildDefaultConfigData (TRAP_CFG_VER_7);
  memcpy ((L7_char8 *) pCfgV7, (L7_char8 *) pCfgCur, sizeof (*pCfgCur));
}

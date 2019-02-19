
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename user_mgr_migrate.c
*
* @purpose usermgr Configuration Migration
*
* @component usermgr
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "commdefs.h"
#define USER_MGR_FACTORY_DEFAULT  FACTORY_DEFAULT_REFER

#include "l7_common.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "user_manager_exports.h"
#include "user_mgr.h"
#include "defaultconfig.h"
#include "user_mgr.h"
#include "user_mgr_migrate.h"
#include "user_mgr_api.h"
#include "user_mgr_apl.h"
#include "user_mgr_ports.h"
#include "default_cnfgr.h"
#include "pw_scramble_api.h"

#define USER_MGR_PORT_ALL_ACCESS 0xFFFFFFFF

static L7_RC_t userMgrMigrateConfigV4V5Convert (userMgrCfgDataV4_t * pCfgV4,
                                                userMgrCfgDataV5_t * pCfgV5);

static L7_RC_t userMgrMigrateConfigV5V6Convert (userMgrCfgDataV5_t * pCfgV5,
                                                userMgrCfgDataV6_t * pCfgV6);

static L7_RC_t userMgrMigrateConfigV6V7Convert (userMgrCfgDataV6_t * pCfgV6,
                                                userMgrCfgDataV7_t * pCfgV7);

static L7_RC_t userMgrMigrateConfigV7V8Convert (userMgrCfgDataV7_t * pCfgV7,
                                                userMgrCfgDataV8_t * pCfgV8);

static void userMgrBuildDefaultConfigDataV5 (userMgrCfgDataV5_t * pCfgV5);
static void userMgrBuildDefaultConfigDataV6 (userMgrCfgDataV6_t * pCfgV6);
static void userMgrBuildDefaultConfigDataV7 (userMgrCfgDataV7_t * pCfgV7);
static void userMgrBuildDefaultConfigDataV8 (userMgrCfgDataV8_t * pCfgV8);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static userMgrCfgData_t *pCfgCur = L7_NULLPTR;
extern userMgrCfgData_t userMgrCfgData;

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
userMgrMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  userMgrCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  userMgrCfgDataV5_t *pCfgV5 = L7_NULLPTR;
  userMgrCfgDataV6_t *pCfgV6 = L7_NULLPTR;
  userMgrCfgDataV7_t *pCfgV7 = L7_NULLPTR;
  userMgrCfgDataV8_t *pCfgV8 = L7_NULLPTR;

  pCfgCur = (userMgrCfgData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == USER_MGR_CFG_VER_4 && pCfgHdr->length == sizeof (userMgrCfgDataV5_t))
  {
    oldVer = USER_MGR_CFG_VER_5;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case USER_MGR_CFG_VER_4:
    pCfgV4 = (userMgrCfgDataV4_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (userMgrCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (userMgrCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = USER_MGR_CFG_VER_4;
    break;

  case USER_MGR_CFG_VER_5:
    pCfgV5 = (userMgrCfgDataV5_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (userMgrCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (userMgrCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case USER_MGR_CFG_VER_6:
    pCfgV6 = (userMgrCfgDataV6_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (userMgrCfgDataV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (userMgrCfgDataV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case USER_MGR_CFG_VER_7:
    pCfgV7 = (userMgrCfgDataV7_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (userMgrCfgDataV7_t));
    if (L7_NULLPTR == pCfgV7)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV7;
    oldCfgSize = (L7_uint32) sizeof (userMgrCfgDataV7_t);
    pOldCfgCksum = &pCfgV7->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case USER_MGR_CFG_VER_8:
    pCfgV8 = (userMgrCfgDataV8_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                 (L7_uint32) sizeof (userMgrCfgDataV8_t));
    if (L7_NULLPTR == pCfgV8)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV8;
    oldCfgSize = (L7_uint32) sizeof (userMgrCfgDataV8_t);
    pOldCfgCksum = &pCfgV8->checkSum;
    oldCfgVer = pCfgHdr->version;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_USER_MGR_COMPONENT_ID,
                                         USER_MGR_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_USER_MGR_COMPONENT_ID,
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
    case USER_MGR_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (userMgrCfgDataV5_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (userMgrCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != userMgrMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case USER_MGR_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (userMgrCfgDataV6_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (userMgrCfgDataV6_t));
      if ((L7_NULLPTR == pCfgV6) || (L7_NULLPTR == pCfgV5) ||
          (L7_SUCCESS != userMgrMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case USER_MGR_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      pCfgV7 = (userMgrCfgDataV7_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (userMgrCfgDataV7_t));
      if ((L7_NULLPTR == pCfgV7) || (L7_NULLPTR == pCfgV6) ||
          (L7_SUCCESS != userMgrMigrateConfigV6V7Convert (pCfgV6, pCfgV7)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;
      /*passthru */

    case USER_MGR_CFG_VER_7:
      /* pCfgV7 has already been allocated and filled in */
      pCfgV8 = (userMgrCfgDataV8_t *) osapiMalloc (L7_USER_MGR_COMPONENT_ID,
                                                   (L7_uint32) sizeof (userMgrCfgDataV8_t));
      if ((L7_NULLPTR == pCfgV8) || (L7_NULLPTR == pCfgV7) ||
          (L7_SUCCESS != userMgrMigrateConfigV7V8Convert (pCfgV7, pCfgV8)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV7);
      pCfgV7 = L7_NULLPTR;
      /*passthru */

    case USER_MGR_CFG_VER_8:
      /* pCfgV7 has already been allocated and filled in */
      if (pCfgV8 != L7_NULLPTR)
      {
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV8, sizeof (*pCfgCur));
        osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV8);
        pCfgV8 = L7_NULLPTR;
        /* done with migration - Flag unsaved configuration */
        pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      }
      else
      {
        buildDefault = L7_TRUE;
      }
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV6);
  if (pCfgV7 != L7_NULLPTR)
    osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV7);
  if (pCfgV8 != L7_NULLPTR)
    osapiFree (L7_USER_MGR_COMPONENT_ID, pCfgV8);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    userMgrBuildDefaultConfigData (USER_MGR_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5
*
* @param    pCfgV4    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV5        @b{(input)} ptr to version 2 config data structure
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
userMgrMigrateConfigV4V5Convert (userMgrCfgDataV4_t * pCfgV4, userMgrCfgDataV5_t * pCfgV5)
{
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 intIfNum;
  L7_uint32 intIfCount;
  nimConfigID_t configId;
  L7_INTF_TYPES_t intfType;
  L7_uint32 ifConfigIndex;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != USER_MGR_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, USER_MGR_CFG_VER_4);
    return L7_FAILURE;
  }

  userMgrBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  for (i = 0; i < L7_MAX_USERS_LOGINS; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->systemLogins[i], pCfgV4->systemLogins[i]);
    pCfgV5->systemLogins[i].userAuth[USER_MANAGER_COMPONENT_OFFSET].componentId =
      L7_USER_MGR_COMPONENT_ID;
    pCfgV5->systemLogins[i].userAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;
  }

  for (i = 0; i < L7_APL_COMPONENT_COUNT; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->nonConfiguredUserAuth[i], pCfgV4->nonConfiguredUserAuth[i]);
  }
  pCfgV5->nonConfiguredUserAuth[USER_MANAGER_COMPONENT_OFFSET].componentId =
    L7_USER_MGR_COMPONENT_ID;
  pCfgV5->nonConfiguredUserAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;
  MIGRATE_COPY_STRUCT (pCfgV5->APLs, pCfgV4->APLs);

  intIfCount = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT_REL_4_0);
  intIfNum = 0;
  intfType = 0;
  ifConfigIndex = 0;
  for (i = 0; i < intIfCount; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, &intIfNum, &intfType);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }
    if (userMgrPortIsValidIntfType (intfType) == L7_FALSE)
      continue;

    if (ifConfigIndex >= L7_MAX_INTERFACE_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (USER_MGR_CFG_FILENAME, ifConfigIndex);
      break;
    }

    (void) nimConfigIdCopy (&configId, &pCfgV5->portData[ifConfigIndex].configID);
    MIGRATE_COPY_STRUCT (pCfgV5->portData[ifConfigIndex].portUsers, pCfgV4->portUsers[i]);
    ifConfigIndex++;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V5 to V6
*
* @param    pCfgV5    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV6        @b{(input)} ptr to version 2 config data structure
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
userMgrMigrateConfigV5V6Convert (userMgrCfgDataV5_t * pCfgV5, userMgrCfgDataV6_t * pCfgV6)
{
  L7_uint32 i, intIfCount;
  L7_INTF_TYPES_t intfType;
  L7_uint32 ifConfigIndex;


  /* verify correct version of old config file */
  if ((pCfgV5->cfgHdr.version != USER_MGR_CFG_VER_5) &&
      (pCfgV5->cfgHdr.version != USER_MGR_CFG_VER_4))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, USER_MGR_CFG_VER_5);
    return L7_FAILURE;
  }

  userMgrBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  for (i = 0; i < L7_MAX_USERS_LOGINS; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV6->systemLogins[i], pCfgV5->systemLogins[i]);
    pCfgV6->systemLogins[i].userAuth[USER_MANAGER_COMPONENT_OFFSET].componentId =
      L7_USER_MGR_COMPONENT_ID;
    pCfgV6->systemLogins[i].userAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;
  }
  MIGRATE_COPY_STRUCT (pCfgV6->nonConfiguredUserAuth, pCfgV5->nonConfiguredUserAuth);
  pCfgV6->nonConfiguredUserAuth[USER_MANAGER_COMPONENT_OFFSET].componentId =
    L7_USER_MGR_COMPONENT_ID;
  pCfgV6->nonConfiguredUserAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;
  MIGRATE_COPY_STRUCT (pCfgV6->APLs, pCfgV5->APLs);

  ifConfigIndex = 0;
  intIfCount = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT_REL_4_3);

  for (i = 0; i < intIfCount; i++)
  {
    intfType = pCfgV5->portData[i].configID.type;

    if (userMgrPortIsValidIntfType (intfType) == L7_FALSE)
      continue;

    if (ifConfigIndex >= L7_MAX_INTERFACE_COUNT_REL_4_3)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (USER_MGR_CFG_FILENAME, ifConfigIndex);
      break;
    }

    MIGRATE_COPY_STRUCT (pCfgV6->portData[ifConfigIndex], pCfgV5->portData[i]);
    ifConfigIndex++;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V6 to V7
*
* @param    pCfgV6    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV7    @b{(input)} ptr to version 2 config data structure
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
userMgrMigrateConfigV6V7Convert (userMgrCfgDataV6_t * pCfgV6, userMgrCfgDataV7_t * pCfgV7)
{
  L7_uint32 i, intIfCount;
  L7_INTF_TYPES_t intfType;
  L7_uint32 ifConfigIndex;


  /* verify correct version of old config file */
  if ((pCfgV6->cfgHdr.version != USER_MGR_CFG_VER_6) &&
      (pCfgV6->cfgHdr.version != USER_MGR_CFG_VER_5) &&
      (pCfgV6->cfgHdr.version != USER_MGR_CFG_VER_4))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV6->cfgHdr.version, USER_MGR_CFG_VER_6);
    return L7_FAILURE;
  }

  userMgrBuildDefaultConfigDataV7 (pCfgV7);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  for (i = 0; i < L7_MAX_USERS_LOGINS; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV7->systemLogins[i], pCfgV6->systemLogins[i]);
    pCfgV7->systemLogins[i].userAuth[USER_MANAGER_COMPONENT_OFFSET].componentId =
      L7_USER_MGR_COMPONENT_ID;
    pCfgV7->systemLogins[i].userAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;
  }
  MIGRATE_COPY_STRUCT (pCfgV7->nonConfiguredUserAuth, pCfgV6->nonConfiguredUserAuth);
  pCfgV7->nonConfiguredUserAuth[USER_MANAGER_COMPONENT_OFFSET].componentId =
    L7_USER_MGR_COMPONENT_ID;
  pCfgV7->nonConfiguredUserAuth[DOT1X_COMPONENT_OFFSET].componentId = L7_DOT1X_COMPONENT_ID;
  MIGRATE_COPY_STRUCT (pCfgV7->APLs, pCfgV6->APLs);

  ifConfigIndex = 0;
  intIfCount = min (L7_MAX_INTERFACE_COUNT_REL_4_3, L7_MAX_INTERFACE_COUNT);

  for (i = 0; i < intIfCount; i++)
  {
    intfType = pCfgV6->portData[i].configID.type;

    if (userMgrPortIsValidIntfType (intfType) == L7_FALSE)
      continue;

    if (ifConfigIndex >= L7_MAX_INTERFACE_COUNT)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (USER_MGR_CFG_FILENAME, ifConfigIndex);
      break;
    }

    MIGRATE_COPY_STRUCT (pCfgV7->portData[ifConfigIndex], pCfgV6->portData[i]);
    ifConfigIndex++;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Converts the config data structure from version V7 to V8
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
userMgrMigrateConfigV7V8Convert (userMgrCfgDataV7_t * pCfgV7, userMgrCfgDataV8_t * pCfgV8)
{
  L7_uint32 i, intIfCount;
  L7_INTF_TYPES_t intfType;
  L7_uint32 ifConfigIndex;
  L7_char8 passUnscrambled[L7_PASSWORD_SIZE];

  /* verify correct version of old config file */
  if ((pCfgV7->cfgHdr.version != USER_MGR_CFG_VER_7) &&
      (pCfgV7->cfgHdr.version != USER_MGR_CFG_VER_6) &&
      (pCfgV7->cfgHdr.version != USER_MGR_CFG_VER_5) &&
      (pCfgV7->cfgHdr.version != USER_MGR_CFG_VER_4))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV7->cfgHdr.version, USER_MGR_CFG_VER_7);
    return L7_FAILURE;
  }

  userMgrBuildDefaultConfigDataV8 (pCfgV8);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  for (i = 0; i < L7_MAX_USERS_LOGINS; i++)
  {
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].loginName,        pCfgV7->systemLogins[i].loginName);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].accessMode,       pCfgV7->systemLogins[i].accessMode);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].loginStatus,      pCfgV7->systemLogins[i].loginStatus);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].authProt,         pCfgV7->systemLogins[i].authProt);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].encryptProt,      pCfgV7->systemLogins[i].encryptProt);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].encryptKey,       pCfgV7->systemLogins[i].encryptKey);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].snmpv3AccessMode, pCfgV7->systemLogins[i].snmpv3AccessMode);
    MIGRATE_COPY_STRUCT(pCfgV8->systemLogins[i].userAuth,         pCfgV7->systemLogins[i].userAuth);

    memset(passUnscrambled, 0, sizeof(passUnscrambled));
    memset(pCfgV8->systemLogins[i].password, 0, sizeof(pCfgV8->systemLogins[i].password));
    pwUnscrambleLength(pCfgV7->systemLogins[i].password, passUnscrambled, L7_PASSWORD_SIZE_REL_4_4);
    pwEncrypt(passUnscrambled, pCfgV8->systemLogins[i].password, L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
  }


  MIGRATE_COPY_STRUCT (pCfgV8->nonConfiguredUserAuth, pCfgV7->nonConfiguredUserAuth);
  /* MIGRATE_COPY_STRUCT (pCfgV8->APLs, pCfgV7->APLs); */

  ifConfigIndex = 0;
  intIfCount = min (L7_MAX_INTERFACE_COUNT_REL_4_4, L7_MAX_INTERFACE_COUNT);

  for (i = 0; i < intIfCount; i++)
  {
    intfType = pCfgV7->portData[i].configID.type;

    if (userMgrPortIsValidIntfType (intfType) == L7_FALSE)
      continue;

    if (ifConfigIndex >= L7_MAX_INTERFACE_COUNT)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (USER_MGR_CFG_FILENAME, ifConfigIndex);
      break;
    }

    MIGRATE_COPY_STRUCT (pCfgV8->portData[ifConfigIndex], pCfgV7->portData[i]);
    ifConfigIndex++;
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
userMgrBuildDefaultConfigDataV5 (userMgrCfgDataV5_t * pCfgV5)
{

  L7_uint32 i;
  L7_BOOL allow;

  memset (pCfgV5, 0, sizeof (*pCfgV5));
  strcpy (pCfgV5->cfgHdr.filename, USER_MGR_CFG_FILENAME);
  pCfgV5->cfgHdr.version = USER_MGR_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_USER_MGR_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = sizeof (*pCfgV5);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;


  /*----------------------------------------------------------------*/
  /* Logic copied from userMgrConfigLoginsFactoryDefault for vers 5 */
  /*----------------------------------------------------------------*/

  /* build checking on FD_logins structure */
#if (FD_USER_MGR_LOGINS_SIZE != L7_MAX_USERS_LOGINS)
#error USER_MGR Build Error
#endif

  memset((void *)pCfgV5->systemLogins, 0, sizeof(pCfgV5->systemLogins));

  /* build checking on FD_nonConfiguredUserAuth structure */
#if (FD_NONCONFIGUREDAUTH_SIZE != L7_APL_COMPONENT_COUNT)
#error USER_MGR NonConfigured User Build Error
#endif

  memcpy(pCfgV5->nonConfiguredUserAuth,
         FD_nonConfiguredUserAuth,
         sizeof(FD_nonConfiguredUserAuth));


  /*----------------------------------------------------------------*/
  /* Logic copied from userMgrConfigAPLsFactoryDefault for vers 5 */
  /*----------------------------------------------------------------*/

  /* build checking on FD_APLs structure */
#if (FD_APL_SIZE != FD_MAX_APL_COUNT)
#error USER_MGR APL List Build Error  FD_APL_SIZE != FD_MAX_APL_COUNT
#endif

  /* memcpy(pCfgV5->APLs, FD_APLs, sizeof(FD_APLs)); */


  /*----------------------------------------------------------------*/
  /* Logic copied from userMgrPortInitializeAll for vers 5 */
  /*----------------------------------------------------------------*/

  allow = L7_DOT1X_DEFAULT_USER_PORT_ACCESS;

  for (i=0; i<L7_MAX_INTERFACE_COUNT_REL_4_1; i++)
  {

      if (L7_TRUE == allow)
          pCfgV5->portData[i].portUsers = USER_MGR_PORT_ALL_ACCESS;
      else
          pCfgV5->portData[i].portUsers = 0;
  }

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
userMgrBuildDefaultConfigDataV6 (userMgrCfgDataV6_t * pCfgV6)
{

  L7_uint32 i;
  L7_BOOL allow;

  memset (pCfgV6, 0, sizeof (*pCfgV6));
  strcpy (pCfgV6->cfgHdr.filename, USER_MGR_CFG_FILENAME);
  pCfgV6->cfgHdr.version = USER_MGR_CFG_VER_6;
  pCfgV6->cfgHdr.componentID = L7_USER_MGR_COMPONENT_ID;
  pCfgV6->cfgHdr.type = L7_CFG_DATA;
  pCfgV6->cfgHdr.length = sizeof (*pCfgV6);
  pCfgV6->cfgHdr.dataChanged = L7_FALSE;


  /*----------------------------------------------------------------*/
  /* Logic copied from userMgrConfigLoginsFactoryDefault for vers 6 */
  /*----------------------------------------------------------------*/

  /* build checking on FD_logins structure */
#if (FD_USER_MGR_LOGINS_SIZE != L7_MAX_USERS_LOGINS)
#error USER_MGR Build Error
#endif

     memset((void *)pCfgV6->systemLogins, 0, (sizeof(logins_V3_t)*(L7_MAX_USERS_LOGINS)));

  /* build checking on FD_nonConfiguredUserAuth structure */
#if (FD_NONCONFIGUREDAUTH_SIZE != L7_APL_COMPONENT_COUNT)
#error USER_MGR NonConfigured User Build Error
#endif

  memcpy(pCfgV6->nonConfiguredUserAuth,
         FD_nonConfiguredUserAuth,
         sizeof(FD_nonConfiguredUserAuth));


  /*----------------------------------------------------------------*/
  /* Logic copied from userMgrConfigAPLsFactoryDefault for vers 6 */
  /*----------------------------------------------------------------*/

  /* build checking on FD_APLs structure */
#if (FD_APL_SIZE != FD_MAX_APL_COUNT)
#error USER_MGR APL List Build Error  FD_APL_SIZE != FD_MAX_APL_COUNT
#endif

  /* memcpy(pCfgV6->APLs, FD_APLs, sizeof(FD_APLs)); */


  /*----------------------------------------------------------------*/
  /* Logic copied from userMgrPortInitializeAll for vers 6 */
  /*----------------------------------------------------------------*/

  allow = L7_DOT1X_DEFAULT_USER_PORT_ACCESS;

  for (i=0; i<L7_MAX_INTERFACE_COUNT_REL_4_3; i++)
  {
    if (L7_TRUE == allow)
      pCfgV6->portData[i].portUsers = USER_MGR_PORT_ALL_ACCESS;
    else
      pCfgV6->portData[i].portUsers = 0;
  }

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
userMgrBuildDefaultConfigDataV7 (userMgrCfgDataV7_t * pCfgV7)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (userMgrCfgDataV7_t));
  userMgrBuildDefaultConfigData (USER_MGR_CFG_VER_7);
  memcpy ((L7_uchar8 *) pCfgV7, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

/*********************************************************************
* @purpose  Build Version 8 defaults
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
userMgrBuildDefaultConfigDataV8 (userMgrCfgDataV8_t * pCfgV8)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (userMgrCfgDataV8_t));
  userMgrBuildDefaultConfigData (USER_MGR_CFG_VER_8);
  memcpy ((L7_uchar8 *) pCfgV8, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}




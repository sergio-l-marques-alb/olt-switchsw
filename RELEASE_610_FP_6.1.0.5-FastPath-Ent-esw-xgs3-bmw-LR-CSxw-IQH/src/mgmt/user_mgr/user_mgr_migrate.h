
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename user_mgr_migrate.h
*
* @purpose User Manager Configuration Migration
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

#include "cli_web_exports.h"
#include "snmp_exports.h"
#include "user_manager_exports.h"


#ifndef USER_MGR_MIGRATE_H
#define USER_MGR_MIGRATE_H

#include "platform_config_migrate.h"
#include "sysapi_cfg_migrate.h"

#define L7_PASSWORD_SIZE_REL_4_4    9

typedef struct
{
  L7_char8       loginName[L7_LOGIN_SIZE];
  L7_char8       password[L7_PASSWORD_SIZE_REL_4_4];
  L7_uint32      accessMode;
  L7_uint32      loginStatus;
  L7_uint32      authProt;
  L7_uint32      encryptProt;
  L7_char8       encryptKey[L7_ENCRYPTION_KEY_SIZE];
  L7_uint32      snmpv3AccessMode;

} logins_ver2_t;

typedef struct
{
  L7_char8       loginName[L7_LOGIN_SIZE];
  L7_char8       password[L7_PASSWORD_SIZE_REL_4_4];
  L7_uint32      accessMode;
  L7_uint32      loginStatus;
     /* SNMPv3 Fields to be moved to separate structure */
  L7_uint32      authProt;
  L7_uint32      encryptProt;
  L7_char8       encryptKey[L7_ENCRYPTION_KEY_SIZE];
  L7_uint32      snmpv3AccessMode;
     /* end of SNMPv3 Fields */
  user_component_auth_t userAuth[ L7_APL_COMPONENT_COUNT ];
} logins_V3_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  logins_ver2_t  systemLogins[L7_MAX_LOGINS];
  L7_uint32      checkSum;

} userMgrCfgData_ver1_t;

typedef struct
{
  L7_fileHdr_t   cfgHdr;
  logins_V3_t    systemLogins[L7_MAX_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[ L7_APL_COMPONENT_COUNT ];
  L7_uint32      checkSum;                     /* keep this as last 4 bytes */

} userMgrCfgData_ver2_t;

typedef struct
{
  L7_fileHdr_t          cfgHdr;
  logins_V3_t           systemLogins[L7_MAX_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[ L7_APL_COMPONENT_COUNT ];
  userMgrPortUserCfg_t  portData[L7_MAX_PORT_COUNT];
  APL_t                 APLs[ L7_MAX_APL_COUNT ];
  L7_uint32             checkSum;         /* keep this as last 4 bytes */

} userMgrCfgData_ver3_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  logins_V3_t  systemLogins[L7_MAX_USERS_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[L7_APL_COMPONENT_COUNT];
  userMgrPortUser_t portUsers[L7_MAX_INTERFACE_COUNT_REL_4_0];
  APL_t APLs[L7_MAX_APL_COUNT];
  L7_uint32 checkSum;           /* keep this as last 4 bytes */
}
userMgrCfgDataV4_t;

typedef struct
{
  L7_fileHdr_t cfgHdr;
  logins_V3_t  systemLogins[L7_MAX_USERS_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[L7_APL_COMPONENT_COUNT];
  userMgrPortUserCfg_t portData[L7_MAX_INTERFACE_COUNT_REL_4_1];
  APL_t APLs[L7_MAX_APL_COUNT];
  L7_uint32 checkSum;           /* keep this as last 4 bytes */

} userMgrCfgDataV5_t;

typedef struct
{
  L7_fileHdr_t          cfgHdr;
  logins_V3_t           systemLogins[L7_MAX_USERS_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[ L7_APL_COMPONENT_COUNT ];
  userMgrPortUserCfg_t     portData[L7_MAX_INTERFACE_COUNT_REL_4_3];
  APL_t                 APLs[ L7_MAX_APL_COUNT ];
  L7_uint32             checkSum;         /* keep this as last 4 bytes */

} userMgrCfgDataV6_t;

typedef struct
{
  L7_fileHdr_t          cfgHdr;
  logins_V3_t           systemLogins[L7_MAX_USERS_LOGINS];
  user_component_auth_t nonConfiguredUserAuth[ L7_APL_COMPONENT_COUNT ];
  userMgrPortUserCfg_t  portData[L7_MAX_INTERFACE_COUNT_REL_4_4];
  APL_t                 APLs[ L7_MAX_APL_COUNT ];
  L7_uint32             checkSum;         /* keep this as last 4 bytes */

} userMgrCfgDataV7_t;

typedef userMgrCfgData_t userMgrCfgDataV8_t;

void userMgrMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer);

#endif /* USER_MGR_MIGRATE_H */

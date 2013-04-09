/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   dot1x_auth_serv.h
*
* @purpose    Dot1x Authentication Server header file
*
* @component  Dot1x Authentication Server component
*
* @comments   none
*
* @create     11/17/2009
*
* @author     pradeepk
*
* @end
*
**********************************************************************/

#ifndef DOT1X_AUTH_SERV_H
#define DOT1X_AUTH_SERV_H

#include "l7_common.h"
#include "cli_web_exports.h"
#include "dot1x_auth_serv_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "nvstoreapi.h"
#include "cnfgr.h"

/* User Configuration Structure*/
typedef struct
{
  /*  User Name */
  L7_char8       name[L7_LOGIN_SIZE];

  /* User Password */
  L7_char8       password[L7_ENCRYPTED_PASSWORD_SIZE];

  /* Status of this user entry */
  L7_uint32      userStatus;

} dot1xUserData_t;

  /* Dot1x User Database */
  typedef struct
  {
    L7_fileHdr_t          cfgHdr;
    dot1xUserData_t             dot1xUsers[L7_MAX_IAS_USERS];
    L7_uint32             checkSum;         /* keep this as last 4 bytes */
  } dot1xAuthServCfgData_t;

extern dot1xAuthServCfgData_t dot1xAuthServCfgData;
extern osapiRWLock_t     dot1xAuthServUserDBRWLock;

#endif


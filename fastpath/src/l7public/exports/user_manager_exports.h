/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename user_manager_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application
*
* @component
*
* @comments
*
*
* @Notes
*
* @created 03/26/2008
*
* @author akulkarn
* @end
*
**********************************************************************/

#ifndef __USR_MGR_EXPORTS_H_
#define __USR_MGR_EXPORTS_H_

#include "cli_web_exports.h"

/******************************************************************/
/*************       Start User Manager types and defines *********/
/******************************************************************/

#define L7_MAX_AUTH_METHODS     6 /* max number of methods per APL */

typedef enum
{
  L7_AUTH_METHOD_UNDEFINED = 0,
  L7_AUTH_METHOD_ENABLE,
  L7_AUTH_METHOD_IAS,
  L7_AUTH_METHOD_LINE,
  L7_AUTH_METHOD_LOCAL,
  L7_AUTH_METHOD_NONE,
  L7_AUTH_METHOD_RADIUS,
  L7_AUTH_METHOD_TACACS,
  L7_AUTH_METHOD_REJECT
} L7_USER_MGR_AUTH_METHOD_t;

/* Password encryption algorithms */
typedef enum
{
  L7_PASSWORD_ENCRYPT_NONE = 0,
  L7_PASSWORD_ENCRYPT_AES,
  L7_PASSWORD_ENCRYPT_MD5

} L7_PASSWORD_ENCRYPT_ALG_t;

/* FASTPATH uses AES password encryption. */
#define L7_PASSWORD_ENCRYPT_ALG  L7_PASSWORD_ENCRYPT_AES

/* Captive Portal also uses AES password encryption */
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#define L7_CAPTIVE_PORTAL_PASSWORD_ENCRYPT_ALG  L7_PASSWORD_ENCRYPT_AES
#endif /* #ifdef L7_CAPTIVE_PORTAL_PACKAGE */

#define L7_MAX_APL_NAME_SIZE_MIN    1  /* Minimum name size */
#define L7_MAX_APL_NAME_SIZE    15 /* not including trailing null */

#define L7_APL_COMPONENT_COUNT   2 /* number of components requiring APLs */
                                   /* currently dot1x and user login */

#define  L7_MAX_LOGINAUTH_METHODS  6 /* max number of methods per APL */
#define  L7_MAX_ENABLEAUTH_METHODS 5

#define L7_LINE_PASSWORD_ARGUMENTS_MAX     1

#define L7_MAX_AUTHENTICATIONLIST_NAME_SIZE    12 /* not including trailing null */

#define L7_USER_MGR_MAX_CHALLENGE_PHRASE_LENGTH  253
#define L7_USER_MGR_DEFAULT_USER_STRING "default"
#define L7_DEFAULT_APL_NAME       "defaultList\0\0\0\0"
#define L7_ENABLE_APL_NAME        "enableList\0\0\0\0\0"
#define L7_DEFAULT_HTTP_APL_NAME  "httpList\0\0\0\0\0\0\0"
#define L7_DEFAULT_NETWORK_APL_NAME  "networkList\0\0\0\0"

#define CLI_HTTPSLISTNAME_AUTH "HttpsListName"
#define CLI_HTTPLISTNAME_AUTH "HttpListName"
#define CLI_DOT1XLISTNAME_AUTH "Dot1xListName"

#define L7_DOT1X_DEFAULT_USER_PORT_ACCESS  L7_TRUE

#define L7_MAX_APL_COUNT         FD_MAX_APL_COUNT /* max number of configured APLs */

#define L7_USER_MGR_STATE_FIELD_SIZE  253
#define L7_MAX_LOGIN_AUTHLIST_COUNT         5 /* max number of configured login authentication lists */
#define L7_MAX_ENABLE_AUTHLIST_COUNT        5 /* max number of configured enable authentication lists */
#define L7_MAX_HTTP_AUTHLIST_COUNT          1 /* max number of http authentication lists */
#define L7_MAX_HTTPS_AUTHLIST_COUNT         1 /* max number of https authentication lists */
#define L7_MAX_DOT1X_AUTHLIST_COUNT         1 /* max number of dot1x authentication lists */
#define L7_AUTH_LIST_NONE            3

#define L7_HTTP_MAX_AUTH_METHODS       4 /* max number of methods per APL */
#define L7_HTTP_MIN_AUTH_METHODS       1 /* min number of methods */
#define L7_HTTPS_MAX_AUTH_METHODS      4 /* max number of methods per APL */
#define L7_HTTPS_MIN_AUTH_METHODS      1 /* min number of methods */
#define L7_DOT1X_MAX_AUTH_METHODS      3 /* max number of methods per APL */
#define L7_DOT1X_MIN_AUTH_METHODS      1 /* min number of methods */
#define L7_LOGIN_MAX_AUTH_METHODS      6 /* max number of methods per APL */
#define L7_LOGIN_MIN_AUTH_METHODS      1 /* min number of methods */
#define L7_ENABLE_MAX_AUTH_METHODS     5 /* max number of methods per APL */
#define L7_ENABLE_MIN_AUTH_METHODS     1 /* min number of methods */

#define L7_USER_MGR_MIN_ACCESSLEVEL 1
#define L7_USER_MGR_MAX_ACCESSLEVEL 15

#define L7_LINEPASSWORD_INDEX           1
#define L7_USE_MGR_DEFAULT_ACCESSLEVEL  1

#define L7_USER_MGR_DEFAULT_USER_STRING "default"
#define L7_USER_MGR_ENABLE_USER_STRING "$enab15$"
#define L7_ACCESS_LEVEL_NUM 3
#define L7_ACCESS_LINE_NUM 8
#define L7_ENABLE_LEVEL_NUM 16

#define L7_MAX_LOGIN_HISTORY_SIZE   50

typedef enum
{
  ACCESS_LEVEL_UNKNOWN = 0,
  ACCESS_LEVEL_LOGIN,
  ACCESS_LEVEL_ENABLE
} L7_ACCESS_LEVEL_t;

typedef enum
{
  L7_SERVICE_UNKNOWN = 0,
  L7_SERVICE_AUTHEN  = 1,
  L7_SERVICE_AUTHOR  = 2
}L7_SERVICE_TYPE;

typedef enum
{
  ACCESS_LINE_UNKNOWN = 0,
  ACCESS_LINE_CONSOLE,
  ACCESS_LINE_TELNET,
  ACCESS_LINE_SSH,
  ACCESS_LINE_HTTPS,
  ACCESS_LINE_HTTP,
  ACCESS_LINE_DOT1X,
  ACCESS_LINE_CTS  /* corresponding to the combination of Console, Telnet, and SSH */
} L7_ACCESS_LINE_t;

typedef
enum
{
  usmUserAuthProtocol_none = 0,
  usmUserAuthProtocol_sha = 1,
  usmUserAuthProtocol_md5 = 2,
  usmUserAuthProtocol_last                  /* do not remove */
} l7_usmUserAuthProtocol_t;

typedef
enum
{
  usmUserPrivProtocol_none = 0,
  usmUserPrivProtocol_des = 1,
  usmUserPrivProtocol_3des = 2,             /* not supported */
  usmUserPrivProtocol_aes128 = 3,           /* not supported */
  usmUserPrivProtocol_aes192 = 4,           /* not supported */
  usmUserPrivProtocol_aes256 = 5,           /* not supported */
  usmUserPrivProtocol_last                  /* do not remove */
} l7_usmUserPrivProtocol_t;

typedef struct userMgrAuthRequest_s
{
  L7_char8         *pUserName;              /* Username.  May be NULL for certain TACACS+ operations */
  L7_char8         *pPwd;                   /* Password */
  L7_uchar8        *port;                   /* Name of port -- may be NULL */
  L7_uchar8        *rem_addr;               /* remote address of client -- may be NULL */
  L7_uint32         component;              /* Component requesting access */
  L7_uint32         accessLevel;            /* Desired access level (0-15) */
  L7_LOGIN_TYPE_t   line;                   /* The line on which the user is connected (serial, telnet, etc.) */
  L7_ACCESS_LEVEL_t mode;                   /* The mode for which to authenticate (login/enable) */
  L7_BOOL           mayChallenge;           /* L7_TRUE if may be challenged -- input */
  L7_BOOL           isChallenged;           /* L7_TRUE if challenged -- output */
  L7_uint32         challengeFlags;         /* flags returned by authentication method
                                               currently only 0x01 -- NOECHO is supported */
  L7_SERVICE_TYPE   servType;               /* AUTHENTICATION/AUTHORIZATION */
  L7_uchar8        *pChallengePhrase;       /* phrase to display to user -- output */
  L7_uchar8        *pState;                 /* challenge state -- I/O */
} userMgrAuthRequest_t;

/******************************************************************/
/*************       End User Manager types and defines   *********/
/******************************************************************/

/******************** conditional Override *****************************/

#ifdef INCLUDE_USER_MANAGER_EXPORTS_OVERRIDES
#include "user_manager_exports_overrides.h"
#endif

#endif /* __USR_MGR_EXPORTS_H_*/

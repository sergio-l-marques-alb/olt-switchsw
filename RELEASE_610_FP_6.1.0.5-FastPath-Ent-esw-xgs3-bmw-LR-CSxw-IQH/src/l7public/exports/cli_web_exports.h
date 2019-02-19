/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snooping_exports.h
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

#ifndef __CLI_WEB_EXPORTS_H_
#define __CLI_WEB_EXPORTS_H_

/*--------------------------------------*/
/*  CLI and WEB Constants               */
/*--------------------------------------*/

#define L7_MAX_LEN_1024  1024

#define L7_SYS_SIZE       256
#define L7_SYS_SIZE_32    32
#define L7_SYS_DISPLAY_SIZE  40
#define L7_PROMPT_SIZE    65
#define L7_SYSNAME         1
#define L7_ALIASNAME       2

#define L7_CLI_MAX_RANGE_DISPLAY_STRING_LENGTH L7_CLI_MAX_LARGE_STRING_LENGTH*10
#define L7_CLI_STRING_LENGTH         16
#define L7_CLI_PORT_MIN              1
#define L7_CLI_PORT_MAX              65535

typedef enum
{
  L7_LOGIN_TYPE_UNKNWN = 0,
  L7_LOGIN_TYPE_SERIAL,
  L7_LOGIN_TYPE_TELNET,
  L7_LOGIN_TYPE_SSH,
  L7_LOGIN_TYPE_HTTP,
  L7_LOGIN_TYPE_HTTPS,
  L7_LOGIN_TYPE_SNMP
} L7_LOGIN_TYPE_t;

#define L7_LOGIN_SIZE                65 /* 64 + \0 */
#define L7_PASSWORD_SIZE             65 /* 64 + \0 */
#define L7_ENCRYPTED_PASSWORD_SIZE   129 /* hex characters + /0 */
#define L7_MIN_PASSWORD_SIZE         0
#define L7_MAX_PASSWORD_SIZE         L7_PASSWORD_SIZE-1
#define L7_MIN_HIST_LENGTH           0
#define L7_MAX_HIST_LENGTH           10
#define L7_MAX_HISTORY_SIZE          10
#define L7_MIN_LOCKOUT_COUNT         1
#define L7_MAX_LOCKOUT_COUNT         5
#define L7_MIN_PASSWD_AGING          1
#define L7_MAX_PASSWD_AGING          365

#define CLI_WEB_LOGIN_SERIAL_USERNAME "----"

#define L7_MAX_LOGINS                6
#define L7_MAX_USERS_LOGINS          L7_MAX_LOGINS+1
#define L7_MAX_TELNET_LOGINS         6
#define L7_MAX_SSH_LOGINS            6
#define L7_MAX_WEB_LOGINS            FD_WEB_DEFAULT_MAX_CONNECTIONS

#define EXEC_TIMEOUT_MINUTES_MIN    0
#define EXEC_TIMEOUT_MINUTES_MAX    65535
#define EXEC_TIMEOUT_SECONDS_MIN    0
#define EXEC_TIMEOUT_SECONDS_MAX    59

#define L7_WEB_SESSION_SOFT_TIMEOUT_MIN 1       /* 1 minute */
#define L7_WEB_SESSION_SOFT_TIMEOUT_MAX 60      /* 1 hour */
#define L7_WEB_SESSION_HARD_TIMEOUT_MIN 1       /* 1 hour */
#define L7_WEB_SESSION_HARD_TIMEOUT_MAX (24*7)  /* 1 day */

#define FD_WEB_SESSION_DEFAULT_SOFT_TIMEOUT  2 /* minutes */
#define FD_WEB_SESSION_DEFAULT_HARD_TIMEOUT  3 /* hours */

#define L7_CLI_WEB_TIME_CONVERSION_FACTOR    60 

typedef enum
{
  L7_LOGIN_ACCESS_NONE = 0,
  L7_LOGIN_ACCESS_READ_ONLY = 1,
  L7_LOGIN_ACCESS_READ_WRITE = 15
} lvl7SecurityLevels_t;

#define L7_DISCOVERY_IP_LIST_MAX        64
#define L7_DISCOVERY_VLAN_LIST_MAX      16
#define L7_CLI_MAX_STRING_LENGTH     256
#define L7_CLI_MAX_LARGE_STRING_LENGTH L7_CLI_MAX_STRING_LENGTH*2
#define L7_LAG_NAME_SIZE             16
#define L7_MACADDR_SIZE              20
#define L7_CLI_MAX_ERROR_MSG_LENGTH  125
#define L7_CLI_MAX_LINES_PER_PAGE    15



/* WEB Component Feature List */
typedef enum
{
  L7_WEB_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_WEB_APPLET_CUSTOMIZATON_FEATURE_ID,   /* Customization support    */
  L7_TXT_CFG_FEATURE_ID,                   /* Customization support    */
  L7_HTTP_FILE_DOWNLOAD_FEATURE,           /* HTTP file download feature support */
  L7_TXT_RUN_CFG_FEATURE_ID,
  L7_WEB_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_WEB_FEATURE_IDS_t;

/*--------------------------*/
/* Outbound Telnet constants*/
/*--------------------------*/

#define L7_TELNET_MAX_SESSIONS                   5
#define L7_TELNET_MIN_SESSIONS                   0
#define L7_TELNET_MAX_SESSION_TIMEOUT          160
#define L7_TELNET_MIN_SESSION_TIMEOUT            1
#define L7_TELNET_DEFAULT_MAX_SESSIONS           5
#define L7_TELNET_DEFAULT_MAX_SESSION_TIMEOUT    FD_TELNET_DEFAULT_TIMEOUT
#define L7_TELNET_DEFAULT_PORT                  23
#define L7_TELNET_MAX_PORT_NO                65535
#define L7_TELNET_MAX_HOST_LENGTH              256

#define L7_TELNET_CFG_SAVE_INTERVAL_SECS       600 /* interval to save config if dirty */


/*-------------------------------------*/
/* Configurable Login Banner constants */
/*-------------------------------------*/
#define BANNER_MAX_FILE_SIZE                2048
#define BANNER_MAX_ROW_WIDTH                80
#define BANNER_MAX_ROW_FILE_COUNT           20
#define BANNER_MAX_WORD_SIZE                80
#define BANNER_MAX_ROW_DISPLAY_COUNT        50
#define BUF_BANNER_DISPLAY_SIZE             4096

#define TEMP_CLI_BANNER_FILE_NAME           "temp_cli.bnr"
#define CLI_BANNER_FILE_NAME                "cli.bnr"

/*----------------------------------------------------------------------*/
/*  Constants added for Configurable Pagination for Show running-config */
/*----------------------------------------------------------------------*/
#define L7_MAX_TERMINAL_LENGTH       48
#define L7_MIN_TERMINAL_LENGTH       5

/* This is a type-definition for the output functions like "ewsTelnetWrite()" in CLI */

typedef void (outputFunc)(void *, char *);
typedef outputFunc *outputFuncPtr;

/******************** conditional Override *****************************/

#ifdef INCLUDE_CLI_WEB_EXPORTS_OVERRIDES
#include "cli_web_exports_overrides.h"
#endif

#endif /* __CLI_WEB_EXPORTS_H_*/

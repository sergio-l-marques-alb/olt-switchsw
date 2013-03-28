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

#include "datatypes.h"
#include "cli_web_mgr_api.h"

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
#define L7_SYSNAME_LONG    3

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

#define L7_LOGIN_SIZE                33 /* 32 + \0 */
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

/******** STRONG PASSWORD ******/
#define L7_PASSWD_UPPERCASE_CHARS_MIN_LEN   0
#define L7_PASSWD_UPPERCASE_CHARS_MAX_LEN   16

#define L7_PASSWD_LOWERCASE_CHARS_MIN_LEN   0
#define L7_PASSWD_LOWERCASE_CHARS_MAX_LEN   16

#define L7_PASSWD_NUMERIC_CHARS_MIN_LEN   0
#define L7_PASSWD_NUMERIC_CHARS_MAX_LEN   16

#define L7_PASSWD_SPECIAL_CHARS_MIN_LEN   0
#define L7_PASSWD_SPECIAL_CHARS_MAX_LEN   16

#define L7_PASSWD_MAX_CONSECUTIVE_CHARS_MIN_LEN  0
#define L7_PASSWD_MAX_CONSECUTIVE_CHARS_MAX_LEN  16

#define L7_PASSWD_MAX_REPEATED_CHARS_MIN_LEN  0
#define L7_PASSWD_MAX_REPEATED_CHARS_MAX_LEN  16

#define L7_PASSWD_CHARACTER_CLASSES_MIN   0
#define L7_PASSWD_CHARACTER_CLASSES_MAX   4

#define L7_PASSWD_EXCLUDE_KEYWORDS_MAX     2
#define L7_PASSWD_EXCLUDE_KEYWORD_MIN_LEN  2 
#define L7_PASSWD_EXCLUDE_KEYWORD_MAX_LEN  65 /* 64 + '\0' */

typedef enum
{
  L7_USER_MGR_PASSWD_INVALID_PRINTABLE_CHARACTERS = 3,
  L7_USER_MGR_PASSWD_INVALID_MIN_LENGTH, 
  L7_USER_MGR_PASSWD_INVALID_MAX_LENGTH, 
  L7_USER_MGR_PASSWD_INVALID_UPPERCASE_LEN,
  L7_USER_MGR_PASSWD_INVALID_LOWERCASE_LEN,
  L7_USER_MGR_PASSWD_INVALID_NUMERIC_CHAR_LEN,
  L7_USER_MGR_PASSWD_INVALID_SPECIAL_CHAR_LEN,
  L7_USER_MGR_PASSWD_INVALID_CONSECUTIVE_CHAR_LEN,
  L7_USER_MGR_PASSWD_INVALID_REPEATED_CHAR_LEN,
  L7_USER_MGR_PASSWD_INVALID_CHARACTER_CLASSES,
  L7_USER_MGR_PASSWD_EXCLUDE_KEYWORDS_EXCEEDS_MIN_LEN,
  L7_USER_MGR_PASSWD_EXCLUDE_LOGIN_NAME_ERROR,
  L7_USER_MGR_PASSWD_EXCLUDE_KEYWORDS_ERROR,
  L7_USER_MGR_PASSWD_EXCLUDE_KEYWORD_ALREADY_EXIST,
  L7_USER_MGR_PASSWD_EXCEEDS_MIN_LEN,
  L7_USER_MGR_PASSWD_INVALID_QUOTATION_CHAR,
  L7_USER_MGR_PASSWD_INVALID_QUESTIONMARK_CHAR,
  L7_USER_MGR_PASSWD_UPPERCASE_CHARS_EXCEEDS_MAX_LIMIT,
  L7_USER_MGR_PASSWD_LOWERCASE_CHARS_EXCEEDS_MAX_LIMIT,
  L7_USER_MGR_PASSWD_NUMERIC_CHARS_EXCEEDS_MAX_LIMIT,
  L7_USER_MGR_PASSWD_SPECIAL_CHARS_EXCEEDS_MAX_LIMIT
} L7_USER_MGR_PASSWD_ERROR_TYPES_t;

/* Password Character Classes */
typedef enum
{
  L7_USER_MGR_PASSWD_MIN_UPPERCASE_LETTERS_MODE = 0,
  L7_USER_MGR_PASSWD_MIN_LOWERCASE_LETTERS_MODE,
  L7_USER_MGR_PASSWD_MIN_NUMERIC_CHARS_MODE,
  L7_USER_MGR_PASSWD_MIN_SPECIAL_CHARS_MODE,
  L7_USER_MGR_PASSWD_MIN_CHARACTER_CLASSES_MODE,
  L7_USER_MGR_PASSWD_MAX_REPEATED_CHARS_MODE,
  L7_USER_MGR_PASSWD_MAX_CONSECUTIVE_CHARS_MODE,
  L7_USER_MGR_PASSWD_EXCLUDE_KEYWORDS_MODE
}userMgrPasswdCharacterClassesMode_t;


/******************************/

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

#define L7_CPU_FREE_MEM_MAX_VALUE     cliWebCpuFreeMemMaxGet()

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
#define L7_CLI_MIN_TERMINAL_LENGTH   0
#define L7_CLI_MAX_TERMINAL_LENGTH   512
#define L7_CLI_NO_PAGINATION         0
#define L7_CLI_MAX_BANNER_MESSAGE_SIZE    2000

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
#define BANNER_MAX_CHAR_SIZE                1500
#define BANNER_MAX_CONVERT_SIZE             9001
#define BANNER_MAX_ROW_WIDTH                80
#define BANNER_MAX_ROW_FILE_COUNT           20
#define BANNER_MAX_WORD_SIZE                80
#define BANNER_MAX_ROW_DISPLAY_COUNT        50
#define BUF_BANNER_DISPLAY_SIZE             4096

#define TEMP_CLI_BANNER_FILE_NAME           "temp_cli.bnr"
#define CLI_BANNER_FILE_NAME                "cli.bnr"

/*L7_FEAT_BANNER_MOTD -- BEGIN*/
#define CLI_BANNER_CFG_FILE_NAME          "cliBannerCfg.txt"

/*Structure for saving cli banner configuration(exec, login or MOTD)*/
typedef struct cliBanner_s
{
  L7_BOOL telnetState;
  L7_BOOL consoleState;
  L7_BOOL SSHState;
  L7_char8 bannerStr[L7_CLI_MAX_BANNER_MESSAGE_SIZE];
}cliBanner_t;

/*Structure for saving all cli banners configuration(exec, login and MOTD)*/
typedef struct cliBannerCfg_s
{
  cliBanner_t execCfg;
  cliBanner_t loginCfg;
  cliBanner_t motdCfg;
}cliBannerCfg_t;

/*L7_FEAT_BANNER_MOTD -- END*/

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

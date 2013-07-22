/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename captive_portal_commdefs.h
*
* @purpose The purpose of this file is to have a central location for
*          common constants to be used by the captive portal package.
*
* @component cp
*
* @comments none
*
* @create 06/26/2007
*
* @author darsenault
* @end
*
**********************************************************************/

#ifndef INCLUDE_CP_COMMDEFS
#define INCLUDE_CP_COMMDEFS

#include <time.h>
#include "datatypes.h"
#include "commdefs.h"
#include "comm_structs.h"
#include "l3_addrdefs.h"
#include "nimapi.h"
#include "defaultconfig.h"

/* DANC: This will be supplied by Haixing; platIntfwirelessNetIntfMaxCountGet(void), in platform_config.h */
#ifdef L7_CP_WIO_PACKAGE
#define L7_MAX_CP_PORT_COUNT        L7_MAX_PORT_COUNT
#else
#define L7_MAX_CP_PORT_COUNT        0
#endif

#define CP_INTERFACE_MAX            (L7_MAX_CP_PORT_COUNT + L7_MAX_NUM_WIRELESS_INTF +1) /* total number of CP interfaces */

#ifdef _L7_OS_VXWORKS_
#define RAM_CP_PATH	""
#define RAM_CP_NAME "RamCP:"
#else
#define RAM_CP_PATH	"/usr/local/ptin/log/fastpath/"     /* PTin modified: paths */
#define RAM_CP_NAME "ramcp"
#endif  

#define CP_URL_FILE_SYSTEM_PATH "/filesystem/ramcp" /* same as EMWEB_FILE_LOCAL_RAM_CP_PREFIX */
#ifdef L7_XWEB_PACKAGE
#define CP_URL_BASE_IMAGE_PATH  "/images"
#else
#define CP_URL_BASE_IMAGE_PATH  "/base/images"
#endif
#ifndef L7_XWEB_PACKAGE
#define CP_URL_PATH             "/security/captive_portal/captive_portal.html"
#define CP_URL_ERROR_PATH       "/security/captive_portal/captive_portal_error.html"
#define CP_URL_DISABLED_PATH    "/security/captive_portal/captive_portal_disabled.html"
#define CP_URL_DISPATCH_PATH    "/security/captive_portal/cp_dispatch.html"
#define CP_URL_WELCOME_PATH     "/security/captive_portal/cp_welcome.html"
#define CP_URL_LOGOUT_PATH      "/security/captive_portal/cp_logout.html"
#define CP_URL_SUCCESS_PATH     "/security/captive_portal/cp_success.html"
#else
#define CP_URL_PATH             "/captive_portal.html"
#define CP_URL_ERROR_PATH       "/captive_portal_error.html"
#define CP_URL_DISABLED_PATH    "/captive_portal_disabled.html"
#define CP_URL_DISPATCH_PATH    "/cp_dispatch.html"
#define CP_URL_WELCOME_PATH     "/cp_welcome.html"
#define CP_URL_LOGOUT_PATH      "/cp_logout.html"
#define CP_URL_SUCCESS_PATH     "/cp_success.html"
#endif

#define CP_URL_PROTOCOL_HOST	"%s://%s"

#define HTTP_STR "HTTP"
#define HTTPS_STR "HTTPS"

#define CP_STANDARD_HTTP_PORT   80  /* Used by CP UIs, this should equate to emweb standard/well_known HTTP port */

#define CP_FLAG_P		        "?p1=%d"
#define CP_INTF_P		        "&p2=%d"
#define CP_IP_P		            "&p3=%u"
#define CP_LANG_P		        "&p4=%s"
#define CP_UID_P		        "&p5=%s"
#define CP_PWD_P		        "&p6=%s"

/* Used by user to change locale preference */
#define CP_LINK_ENTRY           " | <a href=" CP_URL_PROTOCOL_HOST CP_URL_PATH CP_FLAG_P CP_INTF_P CP_IP_P CP_LANG_P ">%s</a>" 

/* Used to perform initial redirect/serve */
#define CP_SERVE_URL		    CP_URL_PROTOCOL_HOST CP_URL_PATH CP_FLAG_P CP_INTF_P CP_IP_P
#define CP_SERVE_URI		    CP_URL_PATH CP_FLAG_P CP_INTF_P CP_IP_P

/* Used during WIP for browser refresh */
#define CP_REFRESH_META "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d;url=" CP_URL_PROTOCOL_HOST CP_URL_PATH  CP_FLAG_P CP_INTF_P CP_IP_P CP_LANG_P CP_UID_P CP_PWD_P ";\">"

/* Fully qualified welcome url */
#define CP_WELCOME_URL "\"" CP_URL_PROTOCOL_HOST "%s" CP_FLAG_P CP_INTF_P CP_IP_P CP_LANG_P "\""
#define CP_WELCOME_PARAMS "\"%s\""

/* User logout popup url including window parameters */
#define CP_LOGOUT_URL CP_URL_PROTOCOL_HOST CP_URL_LOGOUT_PATH CP_FLAG_P CP_INTF_P CP_IP_P CP_LANG_P
#define CP_LOGOUT_POPUP_PARAMS "\"%s\",\"logoutWindow\",\"resizable=1,width=320,height=230\""

#define CP_PARAM_NAME_ACCOUNT_IMAGE         "account-image"
#define CP_PARAM_NAME_ACCOUNT_LABEL         "account-label"
#define CP_PARAM_NAME_ACCEPT_MSG            "accept-msg"
#define CP_PARAM_NAME_ACCEPT_TEXT           "accept-text"
#define CP_PARAM_NAME_AUP_TEXT              "aup-text"
#define CP_PARAM_NAME_BACKGROUND_IMAGE      "background-image"
#define CP_PARAM_NAME_BUTTON_LABEL          "button-label"
#define CP_PARAM_NAME_BRANDING_IMAGE        "branding-image"
#define CP_PARAM_NAME_BROWSER_TITLE         "browser-title"
#define CP_PARAM_NAME_CODE                  "code"
#define CP_PARAM_NAME_DENIED_MSG            "denied-msg"
#define CP_PARAM_NAME_FONT_LIST             "font-list"
#define CP_PARAM_NAME_INSTRUCTIONAL_TEXT    "instructional-text"
#define CP_PARAM_NAME_LINK                  "link"
#define CP_PARAM_NAME_PASSWORD_LABEL        "password-label"
#define CP_PARAM_NAME_RESOURCE_MSG          "resource-msg"
#define CP_PARAM_NAME_TITLE_TEXT            "title-text"
#define CP_PARAM_NAME_TIMEOUT_MSG           "timeout-msg"
#define CP_PARAM_NAME_USER_LABEL            "user-label"
#define CP_PARAM_NAME_WELCOME_TITLE         "welcome-title"
#define CP_PARAM_NAME_WELCOME_TEXT          "welcome-text"
#define CP_PARAM_NAME_WIP_MSG               "wip-msg"
#define CP_PARAM_NAME_SCRIPT_TEXT           "script-text"
#define CP_PARAM_NAME_POPUP_TEXT            "popup-text"
#define CP_PARAM_NAME_LOGOUT_BROWSER_TITLE  "logout-browser-title"
#define CP_PARAM_NAME_LOGOUT_TITLE          "logout-title"
#define CP_PARAM_NAME_LOGOUT_CONTENT        "logout-text"
#define CP_PARAM_NAME_LOGOUT_BUTTON_LABEL   "logout-button-label"
#define CP_PARAM_NAME_LOGOUT_CONFIRM_TEXT   "logout-confirmation-text"
#define CP_PARAM_NAME_LOGOUT_SUCCESS_BROWSER_TITLE "logout-success-browser-title"
#define CP_PARAM_NAME_LOGOUT_SUCCESS_TITLE_TEXT "logout-success-title"
#define CP_PARAM_NAME_LOGOUT_SUCCESS_CONTENT_TEXT "logout-success-text"
#define CP_PARAM_NAME_LOGOUT_SUCCESS_BACKGROUND_IMAGE "logout-success-background-image"

#define CP_FILE_NAME_MAX                    32

#define CP_ID_MIN                           1
#define CP_ID_MAX                           FD_CP_CONFIG_MAX
#define GP_ID_MIN                           1
#define GP_ID_MAX                           FD_CP_USER_GROUP_MAX
#define GP_DEFAULT_NAME                     "Default"
#define CP_WEB_ID_MIN                       1
#define CP_WEB_ID_MAX                       FD_CP_CUSTOM_LOCALE_MAX
#define CP_DEFAULT_LOCALE_MAX               7   /* number of preferred defaults */
#define CP_NAME_MAX                         32  /* CP configuration name */
#define CP_RADIUS_AUTH_SERVER_MAX           L7_RADIUS_SERVER_NAME_LENGTH
#define CP_LANG_CODE_MAX                    32
#define CP_LOCALE_MAX                       32
#define CP_FOREGROUND_COLOR_MAX             32
#define CP_BACKGROUND_COLOR_MAX             32
#define CP_SEPARATOR_COLOR_MAX              32
#define CP_FONT_LIST_MAX                    512
#define CP_BROWSER_TITLE_TEXT_MAX           512   /* UTF-16 format length */
#define CP_WELCOME_URL_MAX                  512   /* UTF-16 format length */
#define CP_TITLE_TEXT_MAX                   512   /* UTF-16 format length */
#define CP_LOCALE_LINK_MAX                  512   /* UTF-16 format length */
#define CP_ACCOUNT_LABEL_MAX                256   /* UTF-16 format length */
#define CP_USER_LABEL_MAX                   128   /* UTF-16 format length */
#define CP_PASSWORD_LABEL_MAX               128   /* UTF-16 format length */
#define CP_BUTTON_LABEL_MAX                 128   /* UTF-16 format length */
#define CP_INSTRUCTIONAL_TEXT_MAX           1024  /* UTF-16 format length */
#define CP_AUP_TEXT_MAX                     32768 /* UTF-16 format length */
#define CP_ACCEPT_TEXT_MAX                  512   /* UTF-16 format length */
#define CP_SCRIPT_TEXT_MAX                  512   /* UTF-16 format length */
#define CP_POPUP_TEXT_MAX                   512   /* UTF-16 format length */
#define CP_MSG_TEXT_MAX                     512   /* UTF-16 format length */
#define CP_WELCOME_TITLE_TEXT_MAX           512   /* UTF-16 format length */
#define CP_WELCOME_TEXT_MAX                 1024  /* UTF-16 format length */
#define CP_HEX_NCR_MAX                      65536 /* Max numeric character reference */
#define CP_LOGOUT_BROWSER_TITLE_TEXT_MAX    512   /* UTF-16 format length */
#define CP_LOGOUT_TITLE_TEXT_MAX            512   /* UTF-16 format length */
#define CP_LOGOUT_CONTENT_TEXT_MAX          1024  /* UTF-16 format length */
#define CP_LOGOUT_BUTTON_LABEL_MAX          128   /* UTF-16 format length */
#define CP_LOGOUT_CONFIRM_TEXT_MAX          512   /* UTF-16 format length */
#define CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX 512 /* UTF-16 format length */
#define CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX    512   /* UTF-16 format length */
#define CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX  1024  /* UTF-16 format length */

/* WEB UI input field max */
#define CP_UI_CP_BROWSER_TITLE_TEXT_MAX     CP_BROWSER_TITLE_TEXT_MAX/4
#define CP_UI_WELCOME_URL_MAX               CP_WELCOME_URL_MAX/4
#define CP_UI_TITLE_TEXT_MAX                CP_TITLE_TEXT_MAX/4
#define CP_UI_LOCALE_LINK_MAX               CP_LOCALE_LINK_MAX/4
#define CP_UI_ACCOUNT_LABEL_MAX             CP_ACCOUNT_LABEL_MAX/4
#define CP_UI_USER_LABEL_MAX                CP_USER_LABEL_MAX/4
#define CP_UI_PASSWORD_LABEL_MAX            CP_PASSWORD_LABEL_MAX/4
#define CP_UI_BUTTON_LABEL_MAX              CP_BUTTON_LABEL_MAX/4
#define CP_UI_INSTRUCTIONAL_TEXT_MAX        CP_INSTRUCTIONAL_TEXT_MAX/4
#define CP_UI_AUP_TEXT_MAX                  CP_AUP_TEXT_MAX/4
#define CP_UI_ACCEPT_TEXT_MAX               CP_ACCEPT_TEXT_MAX/4
#define CP_UI_SCRIPT_TEXT_MAX               CP_SCRIPT_TEXT_MAX/4
#define CP_UI_POPUP_TEXT_MAX                CP_POPUP_TEXT_MAX/4
#define CP_UI_MSG_TEXT_MAX                  CP_MSG_TEXT_MAX/4
#define CP_UI_WELCOME_TITLE_TEXT_MAX        CP_WELCOME_TITLE_TEXT_MAX/4
#define CP_UI_WELCOME_TEXT_MAX              CP_WELCOME_TEXT_MAX/4
#define CP_UI_LOGOUT_BROWSER_TITLE_TEXT_MAX CP_LOGOUT_BROWSER_TITLE_TEXT_MAX/4
#define CP_UI_LOGOUT_TITLE_TEXT_MAX         CP_LOGOUT_TITLE_TEXT_MAX/4
#define CP_UI_LOGOUT_CONTENT_TEXT_MAX       CP_LOGOUT_CONTENT_TEXT_MAX/4
#define CP_UI_LOGOUT_BUTTON_LABEL_MAX       CP_LOGOUT_BUTTON_LABEL_MAX/4
#define CP_UI_LOGOUT_CONFIRM_TEXT_MAX       CP_LOGOUT_CONFIRM_TEXT_MAX/4
#define CP_UI_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX/4
#define CP_UI_LOGOUT_SUCCESS_TITLE_TEXT_MAX  CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX/4
#define CP_UI_LOGOUT_SUCCESS_CONTENT_TEXT_MAX CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX/4

#define CP_HTTP_PORT_MIN                    0
#define CP_HTTP_PORT_MAX                    65535
#define CP_HTTP_SECURE_PORT_MIN             0
#define CP_HTTP_SECURE_PORT_MAX             65535
#define CP_PS_STATS_REPORT_INTERVAL_MIN     15
#define CP_PS_STATS_REPORT_INTERVAL_MAX     3600
#define CP_AUTH_SESSION_TIMEOUT_MIN         60
#define CP_AUTH_SESSION_TIMEOUT_MAX         600

#define CP_USER_LOCAL_USERNAME_MIN          1
#define CP_USER_LOCAL_USERNAME_MAX          32
#define CP_USER_LOCAL_PASSWORD_MIN          8
#define CP_USER_LOCAL_PASSWORD_MAX          64
#define CP_USER_LOCAL_USERGROUP_MIN         1
#define CP_USER_LOCAL_USERGROUP_MAX         32
#define CP_USER_LOCAL_SESSION_TIMEOUT_MIN   0
#define CP_USER_LOCAL_SESSION_TIMEOUT_MAX   86400   /* 24 hrs */
#define CP_USER_LOCAL_IDLE_TIMEOUT_MIN      0
#define CP_USER_LOCAL_IDLE_TIMEOUT_MAX      900     /* 15 mins */
#define CP_USER_LOCAL_MAX_BW_UP_MIN         0
#define CP_USER_LOCAL_MAX_BW_UP_MAX         4294967295U/8
#define CP_USER_LOCAL_MAX_BW_DOWN_MIN       0
#define CP_USER_LOCAL_MAX_BW_DOWN_MAX       4294967295U/8
#define CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN  0
#define CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX  4294967295U
#define CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN 0
#define CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX 4294967295U
#define CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN  0
#define CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX  4294967295U

#define CP_USER_RADIUS_USERGROUPS_MAX       (CP_USER_LOCAL_USERGROUP_MAX+1)*FD_CP_USER_GROUP_MAX

#define CP_AUTH_IN_PROGRESS_MAX             1024
#define CP_CLIENT_CONN_STATUS_MAX           1024

#define CP_INTF_DESCRIPTION_MAX_LEN         L7_NIM_INTF_DESCR_SIZE

/* Shared btw ews and captive portal web pages (could probably live elsewhere  */
#define CP_APPINFO_FLAG_IDX         0
#define CP_APPINFO_INTF_IDX         1
#define CP_APPINFO_IP_IDX           2

/* Shared btw ews and captive portal web pages (could probably live elsewhere  */
typedef struct
{
  L7_uint32 data[32];
  L7_char8 lang[64];
  L7_char8 uid[L7_PASSWORD_SIZE];
  L7_char8 pwd[L7_PASSWORD_SIZE];
} cpAppInfo_t;

typedef enum
{
  CP_VERIFY_MODE_GUEST = 1,
  CP_VERIFY_MODE_LOCAL,
  CP_VERIFY_MODE_RADIUS
} CP_VERIFY_MODE_t;

typedef enum
{
  CP_GLOBAL_PAGE = 1,
  CP_AUTHENTICATION_PAGE,
  CP_WELCOME_PAGE,
  CP_LOGOUT_PAGE,
  CP_LOGOUT_SUCCESS_PAGE,
  CP_ALL
} CP_PAGE_TYPE_t;

typedef enum
{
  CP_DEFAULT = 0,   /* initial request from client */
  CP_SERVE,         /* default serve */
  CP_VALIDATE,      /* time to verify */
  CP_NOACCEPT,      /* resp: user did not check AUP accept */
  CP_WIP,           /* resp: work in progress */
  CP_WIP_PENDING,   /* interim status pending RADIUS callback */
  CP_RADIUS_WIP,    /* work in progress, ready for AUTH command */
  CP_DENIED,        /* resp: invalid credentials */
  CP_RESOURCE,      /* resp: limit exceeded */
  CP_TIMEOUT,       /* resp: ran out of time */
  CP_SUCCESS,       /* resp: authenticated */
  CP_PREVIEW,       /* special administrator flag */
  CP_LOGOUT         /* user logout de-authenticate feature */
} CP_AUTH_STATUS_FLAG_t;

typedef enum
{
  L7_CP_IP_STATUS_NOT_POLLED = 0,
  L7_CP_IP_STATUS_POLLED,
  L7_CP_IP_STATUS_DISCOVERED,
  L7_CP_IP_STATUS_DISCOVERED_FAILED
} L7_CP_IP_STATUS_t;

typedef enum
{
  L7_CP_MODE_ENABLE_PENDING = 0,
  L7_CP_MODE_ENABLED,
  L7_CP_MODE_DISABLE_PENDING,
  L7_CP_MODE_DISABLED
} L7_CP_MODE_STATUS_t;

typedef enum
{
  L7_CP_MODE_REASON_NONE = 0,
  L7_CP_MODE_REASON_ADMIN,
  L7_CP_MODE_REASON_NO_IP_ADDR,
  L7_CP_MODE_REASON_ROUTING_ENABLED_NO_IP_RT_INTF,
  L7_CP_MODE_REASON_ROUTING_DISABLED,
} L7_CP_MODE_REASON_t;

typedef enum
{
  L7_INTF_WHY_DISABLED_NONE = 0,
  L7_INTF_WHY_DISABLED_UNATTACHED,
  L7_INTF_WHY_DISABLED_ADMIN
} L7_INTF_WHY_DISABLED_t;

typedef enum
{
  L7_CP_USER_LOGOUT_CLEAR = 0,
  L7_CP_USER_LOGOUT
} L7_CP_USER_LOGOUT_FLAG_t;

/* TBD - these L7_INTF enums need to renamed for captive portal */
typedef enum
{
  L7_INTF_PARM_CP_ALL            = 0x1,     /* Captive Portal All Capabilities */
  L7_INTF_PARM_CP_MODE           = 0x2,     /* Captive Portal Mode */
  L7_INTF_PARM_CP_SESTIMEOUT     = 0x4,     /* Captive Portal Session Time Out */
  L7_INTF_PARM_CP_IDLETIMEOUT    = 0x8,     /* Captive Portal Session Idle Time Out */
  L7_INTF_PARM_CP_BWUPRATECTRL   = 0x10,    /* Captive Portal Bandwidth Up Rate Control */
  L7_INTF_PARM_CP_BWDNRATECTRL   = 0x20,    /* Captive Portal Bandwidth Down Rate Control */
  L7_INTF_PARM_CP_MAXINOCTMON    = 0x40,    /* Captive Portal Maximum Input Octets Monitor */
  L7_INTF_PARM_CP_MAXOUTOCTMON   = 0x80,    /* Captive Portal Maximum Output Octets Monitor */
  L7_INTF_PARM_CP_BYTESRECVD     = 0x100,   /* Captive Portal Bytes Received Counter */
  L7_INTF_PARM_CP_BYTESTXD       = 0x200,   /* Captive Portal Bytes Transmitted Counter */
  L7_INTF_PARM_CP_PKTRECVD       = 0x400,   /* Captive Portal Packets Received Counter */
  L7_INTF_PARM_CP_PKTTXD         = 0x800,   /* Captive Portal Packets Transmitted Counter */
  L7_INTF_PARM_CP_ROAMING        = 0x1000,  /* Captive Portal Roaming Support */
} L7_INTF_PARM_CP_TYPES_t;

typedef enum
{
  L7_CP_INST_OPER_STATUS_DISABLED = 0,
  L7_CP_INST_OPER_STATUS_ENABLED
} L7_CP_INST_OPER_STATUS_t;

typedef enum
{
  L7_CP_INST_DISABLE_REASON_NONE = 0,
  L7_CP_INST_DISABLE_REASON_ADMIN,
  L7_CP_INST_DISABLE_REASON_NO_RADIUS_SERVER,
  L7_CP_INST_DISABLE_REASON_NO_ACCT_SERVER,
  L7_CP_INST_DISABLE_REASON_NOT_ASSOC_INTF,
  L7_CP_INST_DISABLE_REASON_NO_ACTIVE_INTF,
  L7_CP_INST_DISABLE_REASON_NO_VALID_CERT
} L7_CP_INST_DISABLE_REASON_t;

typedef enum
{
  L7_CP_INST_BLOCK_STATUS_BLOCKED = 0,
  L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING,
  L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED,
  L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING
} L7_CP_INST_BLOCK_STATUS_t;


typedef L7_ushort16 cpId_t;
typedef L7_ushort16 webId_t;
typedef L7_ushort16 gpId_t;
typedef L7_ushort16 uId_t;

typedef enum
{
  CP_AUTH_ON_PEER_SWITCH = 0,
  CP_AUTH_ON_LOCAL_SWITCH
} cpdmAuthWhichSwitch_t;

typedef struct /* user struct for cpcmUserAuthRequest */
{
  L7_IP_ADDR_t            ipAddr;       
  L7_char8                uid[L7_PASSWORD_SIZE];
  L7_char8                pwd[L7_PASSWORD_SIZE];
  CP_AUTH_STATUS_FLAG_t   flag;
} cpUserAuth_t;


typedef struct CPIfNumPair_s
{
  cpId_t    cpId;
  L7_uint32 intIfNum;
} ifNumCPPair_t;

typedef struct ifNumMACPair_s
{
  L7_uint32         intIfNum;
  L7_enetMacAddr_t  macAddr;
} ifNumMACPair_t;

typedef struct CPMACPair_s
{
  cpId_t            cpId;
  L7_enetMacAddr_t  macAddr;
} CPMACPair_t;


/* Trap flags */

typedef enum 
{
  CP_TRAP_NONE                   = 0x00,
  CP_TRAP_CLIENT_CONNECTED       = 0x01,
  CP_TRAP_CLIENT_DISCONNECTED    = 0x02,
  CP_TRAP_AUTH_FAILURE           = 0x04,
  CP_TRAP_CONNECTION_DB_FULL     = 0x08,
  CP_TRAP_AUTH_FAILURE_LOG_WRAP  = 0x10,
  CP_TRAP_ACTIVITY_LOG_WRAP      = 0x20,
  CP_TRAP_ALL                    = 0x3F
} CP_TRAP_FLAGS_t;


/* type values for CP connection transactions */

typedef enum {
  CP_CONN_NEW = 0,
  CP_CONN_DELETE,
  CP_CONN_DEAUTH,
  MAX_CP_CONN_TRANS_TYPES /* always last, but never used as an actual type */
} cpConnTransactionType_t;

/* Types for authentications in progress */

typedef struct cpcmAuthInProgressStatusDesc_s
{
  CP_AUTH_STATUS_FLAG_t   flag;
  L7_enetMacAddr_t        macAddr;
  L7_uint32               intfId;
  L7_uint32               port;
  time_t                  connTime;
  L7_uchar8               uid[CP_USER_LOCAL_USERNAME_MAX + 1];
  L7_uchar8               pwd[L7_PASSWORD_SIZE];
} cpcmAuthInProgressStatusDesc_t;

typedef struct cpConnectionLimits_s
{
  L7_uint32               sessionTimeout;
  L7_uint32               idleTimeout;
  L7_uint32               maxBandwidthUp;
  L7_uint32               maxBandwidthDown;
  L7_uint32               maxInputOctets;
  L7_uint32               maxOutputOctets;
  L7_uint32               maxTotalOctets;
} cpConnectionLimits_t;

/*----------------------------------------*/
/*  Start Captive Portal Logging Options */
/*----------------------------------------*/

#define  CP_DLOG(cp_log_level, __fmt__, __args__...) \
if (captivePortalDebugLevelCheck(cp_log_level) == L7_TRUE) \
{ \
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, __fmt__, ## __args__); \
} \

L7_BOOL  captivePortalDebugLevelCheck(L7_uint32 level);

typedef enum
{
  CPD_LEVEL_FIRST = 0,    /* Place holder to mark the start of the logging options */

  CPD_LEVEL_ERROR = 1,    /* Non-fatal errors */
  CPD_LEVEL_LOG = 2,      /* This trace flag is used for messages that were previsouly logged with LOG_MSG */
  CPD_LEVEL_DEFAULT = 3,  /* This log level is enabled by default */



 /* This must be the last element in the list.
  */
  CPD_LEVEL_LAST
} cp_log_level_t;

#endif /* INCLUDE_CP_COMMDEFS */

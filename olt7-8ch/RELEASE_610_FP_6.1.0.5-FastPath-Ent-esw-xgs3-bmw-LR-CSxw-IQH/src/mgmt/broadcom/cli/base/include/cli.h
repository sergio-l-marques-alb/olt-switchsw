/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli.h
 *
 * @purpose cli #defines and structures to be used outside of the cli as well
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   06/19/2000
 *
 * @author  Forrest Samuels
 * @end
 *
 **********************************************************************/
#ifndef CLI_H
#define CLI_H

#include "util_enumstr.h"
#include "l7_product.h"       

/*  CLI defines */
#define U_IDX 1
#define MAX_ARPS    64

/*for printout format */
#define FORMAT_NUM                      48  /* text description length + dots  (35->50)  */
#define FORMAT_IND                      45  /* text description length        (30->45)  */

#define FORMAT_SHORT_NUM                35
#define FORMAT_SHORT_IND                32

#define MAX_STRING_LEN                  80
#define CHARS_LESS_MAX_STRING_LEN       2

#define CLI_INPUT_EMPTY                 0
#define CLI_INPUT_NOECHO                5

#define CLI_INDEX_NOT_FOUND             -1

#define MAX_COMMAND_LENGTH              128 /*find emweb #defines */

#define CLI_MAX_SCROLL_LINES            24  /* chng 25->24 b/se impact on large output */

#define SCROLL_BUF_LINE_LENGTH          128

#define CLI_SERIAL_HANDLE_NUM           0

#define ESC                             0x1b
#define ARROWESC                        0x5b
#define FUNCTIONESC                     0x4f
#define TAB                             0x09
#define CR                              0x0d
#define LF                              0x0a
#define FF                              0x0c
#define SPC                             0x20
#define UPARROW                         0x1b5b41
#define DOWNARROW                       0x1b5b42
#define RIGHTARROW                      0x1b5b43
#define LEFTARROW                       0x1b5b44
#define F1KEY                           0x1b4f50
#define F2KEY                           0x1b4f51
#define F3KEY                           0x1b4f52
#define F4KEY                           0x1b4f53

#define CLI_IPADDR_SIZE   16    /* max ipaddr: "xxx.xxx.xxx.xxx\0" */

typedef struct
{
  L7_BOOL telnetConnection;                                           /* true = telnet connection, false = HTTP connection */
  L7_BOOL scroll;                                                     /* do we need to scroll or not? */
  L7_BOOL haveUser;                                                   /* has a username been stored? */
  L7_BOOL allowAccess;                                                /* users access */
  L7_int32 charInputID;                                                /* ID associated with current character input (0 for none */
  L7_int32 stringInputID;                                              /* ID associated with current string input (0 for none) */
  L7_int32 stringIndex;                                                /* index of next position in string buffer */
  L7_BOOL stringPassword;                                             /* specifies string input is a password */
  L7_int32 prevDepth;                                                  /* saves the depth for 'root' execution of commands */
  L7_int32 functionArguments;                                          /* number of arguments passed into function */
  L7_int32 userAccess;                                                 /* the access level of the user */
  L7_BOOL adminUserFlag;                                              /* if the current user is 'admin'*/
  L7_uint32 userLoginSessionIndex;                                      /* index into loginsession information, -1 is invalid default */
  L7_int32 scrollLine;                                                 /* current scroll line number (0 based) */
  L7_int32 scrollLineMax;                                              /* max number of lines */
  L7_int32 socket;                                                     /* the socket */
  L7_char8 prompt[L7_PROMPT_SIZE+MAX_COMMAND_LENGTH];                  /* Global Prompt */
  L7_char8 command[MAX_COMMAND_LENGTH];                                /* command to be executed */
  L7_char8 scrollBuffer[SCROLL_BUF_LINES][SCROLL_BUF_LINE_LENGTH];     /* scroll buffer to hold all output when over x number of lines */
  L7_char8 charInput;                                                  /* input L7_char8 */
  L7_char8 stringInput[MAX_STRING_LEN];                                /* input string */
  L7_char8 password[L7_PASSWORD_SIZE];                                 /* used for confirm in password change */
  L7_BOOL devshellActive;                                             /* flag indicates command input is routed to the DevShell */
  L7_BOOL isScroll;                            /* whether line is to be scroll*/
  L7_BOOL loginStatus;                        /* for password aging */
  L7_BOOL passwdExpStatus;                    /* for password aging */
  L7_uint32 callCount;

  struct
  {
    L7_BOOL consoleTraceDisplayEnabled;         /* Display trace and debug printfs on this telnet when flag is enabled */
    /* This flag accounts for multiple factors,
       e.g. the supportConsole is enabled,
       a "debug" trace command in effect
     */

    L7_BOOL debugConsoleDisplayEnabled;          /* Debug display  traces on this telnet when flag is enabled */
    L7_BOOL supportConsoleDisplayEnabled;        /* Debug display  trace and debug printfs on this telnet when flag is enabled */

  } debugDisplayCtl;    /* Control for session display of debug output (traces, devshell) */
} cliCommon_t;

typedef struct
{
  L7_int32 handleNum;
  EwsContext transferContext;
  L7_int32 devshellHandleNum;
  L7_char8 systemPrompt[L7_PROMPT_SIZE];
  L7_char8 loginUserPrompt[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 trapMsgString[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL trapMsgDisplay;
  L7_BOOL transferInProgress;
  L7_BOOL resetSerialConnection;                                      /* flag to reset connection on next input */
  L7_BOOL ioRedirectedToCLI;
  L7_BOOL doNotApplyGlobalConfig;
} cliUtil_t;

typedef enum
{
  L7_FIRST_MODE=100,
  L7_USER_EXEC_MODE,
  L7_PRIVILEGE_USER_MODE,
  L7_VLAN_MODE,
  L7_GLOBAL_CONFIG_MODE,
  L7_INTERFACE_CONFIG_MODE,
  L7_GLOBAL_SUPPORT_MODE,
  L7_LINE_CONFIG_CONSOLE_MODE,
  L7_LINE_CONFIG_SSH_MODE,
  L7_LINE_CONFIG_TELNET_MODE,
  L7_ACL_MODE,
  L7_ACL_RULE_MODE,
  L7_POLICY_MAP_MODE,
  L7_POLICY_CLASS_MODE,
  L7_CLASS_MAP_MODE,
  L7_CLASS_MAP_IPV6_MODE,
  L7_ROUTER_CONFIG_OSPF_MODE,
  L7_ROUTER_CONFIG_OSPFV3_MODE,
  L7_ROUTER_CONFIG_RIP_MODE,
  L7_ROUTER_CONFIG_BGP4_MODE,
  L7_HIDDEN_COMMAND_MODE,
  L7_DHCP_POOL_CONFIG_MODE,
  L7_DHCP6S_POOL_CONFIG_MODE,
  L7_MAC_ACCESS_LIST_CONFIG_MODE,
  L7_IPV6_ACCESS_LIST_CONFIG_MODE,
  L7_IPV4_ACCESS_LIST_CONFIG_MODE,
  L7_STACK_MODE,
  L7_TUNNEL_CONFIG_MODE,
  L7_LOOPBACK_CONFIG_MODE,
  L7_TACACS_MODE,
  L7_WIRELESS_CONFIG_MODE,
  L7_WIRELESS_AP_CONFIG_MODE,
  L7_WIRELESS_NETWORK_CONFIG_MODE,
  L7_WIRELESS_AP_PROFILE_CONFIG_MODE,
  L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE,
  L7_WIRELESS_AP_PROFILE_VAP_CONFIG_MODE,
  L7_MAINTENANCE_MODE,
  L7_CAPTIVEPORTAL_MODE,
  L7_CPINSTANCE_MODE,
  L7_CPENCODED_IMAGE_MODE,
  L7_CPINSTANCE_LOCALE_MODE,
  L7_ARP_ACL_CONFIG_MODE,
  L7_DOT1AG_MAINTAINANCE_DOMAIN_MODE,
  L7_LAST_MODE
} cliModeID_t;

#define L7_CLIMODE_COUNT (L7_LAST_MODE - L7_FIRST_MODE)

#define CLI_MODE_INDEX(x) (x - L7_FIRST_MODE)

typedef struct
{
  EwsCliCommandP cliModeNode[L7_CLIMODE_COUNT];
} cliModes_t;

extern cliCommon_t * cliCommon;
extern cliUtil_t cliUtil;
extern cliModes_t cliModeRW;
extern cliModes_t cliModeRO;
extern EwaStatus ewsFlushAll ( EwsContext context );

EwsCliCommandP cliGetMode (L7_uint32 mode);
void cliChangeMode(L7_uint32 mode);
EwsCliCommandP cliGetThisMode (L7_uint32 mode, cliModes_t * cliModep);
void cliSetMode (L7_uint32 mode, EwsCliCommandP objMode);
#define  READWRITE    L7_LOGIN_ACCESS_READ_WRITE

typedef struct /* for handling error messages in range modes */
{
  L7_uint32 Count;
  L7_uint32 Interface[max(L7_PLATFORM_MAX_VLAN_ID, L7_MAX_INTERFACE_COUNT)];
  const L7_char8 *String[max(L7_PLATFORM_MAX_VLAN_ID, L7_MAX_INTERFACE_COUNT)];
} cliFailureData_t;

extern void cliDisplayVlanFailureInfo(EwsContext ewsContext, cliFailureData_t *failureData);

#endif

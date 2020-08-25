/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cliapi.h
*
* @purpose api header for the private cli structures
*
* @component user interface
*
* @comments this is to allow for easy access to the cliCommon
* @comments and the cliUtil structures as defined in cli.h
*
* @create  04/03/2001
*
* @author  fsamuels
* @end
*
**********************************************************************/

#ifndef CLI_API_H
#define CLI_API_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "l7_common.h"
#include "osapi.h"

#include "sysapi.h"
#include "defaultconfig.h"

#include "ews.h"
#include "ews_telnet.h"
#include "ews_api.h"
#include "ew_config.h"
#include "ews_send.h"             /* for ewsFlushAll() */

#include "cli.h"
#include "cli_macro.h"
#include "util_pstring.h"

#ifdef L7_ROUTING_PACKAGE
#include "clicommands_l3.h"
#endif

#include "clicommands.h"

#include "cliutil.h"
#include "clidevshell.h"

#include "usmdb_util_api.h"

#ifdef L7_WIRELESS_PACKAGE
  #include "default_cnfgr.h"
  #include "usmdb_wdm_api.h"
  #include "usmdb_wdm_ap_profile_api.h"
  #include "usmdb_wdm_ap_api.h"
  #include "usmdb_wdm_ap_statistics_api.h"
  #include "wdm_ap_api.h"
  #include "wdm_ap_statistics_api.h"
  #include "usmdb_wdm_ch_pwr_api.h"
#endif

extern L7_int32 tolower(L7_int32 __c);
extern L7_int32 toupper(L7_int32 __c);
extern L7_int32 isalnum(L7_int32 c);
extern L7_int32 isdigit(L7_int32 __c);

/* function used in cli_assorted.c in copy command*/
void dispProcessingIndicator();

/* cliUtil */
L7_uint32 cliCurrentHandleGet();
void cliCurrentHandleSet(L7_uint32 handle);
L7_uint32 cliDevShellHandleGet();
void cliDevShellHandleSet(L7_uint32 handle);
L7_BOOL cliIoRedirectToCliGet();
void cliIoRedirectToCliSet(L7_BOOL mode);
L7_char8 *cliSystemPromptGet();
void cliSystemPromptSet(L7_char8 * sysPrompt);
L7_char8 *cliTrapMsgStringGet();
void cliTrapMsgStringSet(L7_char8 * trapString);
L7_BOOL cliResetSerialConnectionGet();
void cliResetSerialConnectionSet(L7_BOOL value);

/* cli_web_util.c */
void cliWebIORedirectIoToCLI(void);
void cliWebIORedirectIoToStdIO(void);
int cliWebIORedirectStdInGet(void);
int cliWebIORedirectStdOutGet(void);
int cliWebIORedirectStdErrGet(void);

/* cliCommon */
L7_BOOL cliTelnetConnectionGet();
void cliTelnetConnectionSet(L7_BOOL value);
L7_BOOL cliScrollGet();
void cliScrollSet(L7_BOOL value);
L7_BOOL cliHaveUserGet();
void cliAllowAccessSet(L7_BOOL value);
L7_uint32 cliPrevDepthGet();
void cliPrevDepthSet(L7_uint32 depth);
L7_uint32 cliNumFunctionArgsGet();
void cliNumFunctionArgsSet(L7_uint32 numberOfArgs);
L7_uint32 cliUserNumberGet();
void cliUserNumberSet(L7_uint32 index);
L7_uint32 cliUserAccessGet();
void cliUserAccessSet(L7_uint32 access);
L7_BOOL cliAdminUserFlagGet();
void cliAdminUserFlagSet(L7_BOOL flag);
L7_uint32 cliLoginSessionIndexGet();
void cliLoginSessionIndexSet(L7_uint32 index);
L7_uint32 cliScrollLineGet();
void cliScrollLineSet(L7_uint32 line);
L7_uint32 cliScrollLineMaxGet();
void cliScrollLineMaxSet(L7_uint32 scrollMax);
L7_uint32 cliCurrentSocketGet();
void cliCurrentSocketSet(L7_uint32 socket);
L7_char8 *cliCurrentPromptGet();
void cliCurrentPromptSet(L7_char8 * prompt);
L7_char8 *cliAlternateCommandGet();
void cliAlternateCommandSet(L7_char8 * cmd);
L7_char8 *cliPasswordGet();
void cliPasswordSet(L7_char8 * pass);
L7_BOOL cliDevShellActiveGet();
void cliDevShellActiveset(L7_BOOL value);
L7_BOOL cliIsCurrUnitMgmtUnit();

L7_BOOL cliTrapMsgStatusGet(void);
void cliTrapMsgStatusSet(L7_BOOL val);

#endif /* CLI_API_H */

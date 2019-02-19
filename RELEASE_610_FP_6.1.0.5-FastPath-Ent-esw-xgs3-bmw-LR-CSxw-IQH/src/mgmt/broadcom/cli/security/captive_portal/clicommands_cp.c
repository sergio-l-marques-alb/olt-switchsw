/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   src/mgmt/cli/security/captive_portal/clicommands_cp.c
*
* @purpose    CLI commands for Captive Portal
*
* @component  user interface
*
* @create     07/10/2007
*
* @author     rjain, rjindal
*
* @end
*
*********************************************************************/

#include "strlib_security_cli.h"
#include "cliapi.h"
#include "clicommands_cp.h"
#include "commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "usmdb_cpdm_connstatus_api.h"

/*********************************************************************
* @purpose  Build the Captive Portal Mode
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*********************************************************************/
void buildTreeGlobalCaptivePortal(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;

  depth2 = ewsCliAddNode(depth1, "captive-portal ", pStrInfo_security_CPCfgHelp, cliCaptivePortalMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
* @purpose  To build the Captive portal Global config tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*********************************************************************/
void buildTreeCaptivePortalConfig()
{
  EwsCliCommandP depth1;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliCaptivePortalMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_CAPTIVEPORTAL_MODE, depth1);

  buildTreeCaptivePortalGlobalConfig(depth1);
}

/*********************************************************************
* @purpose  Build the Captive Portal Global commands tree
*
* @param  void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*********************************************************************/
void buildTreeCaptivePortalGlobalConfig(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* depth1 = "Global Config" */

  /* encoded-image */
  /* Note: encoded-image is a hidden node and is initialized using buildTreeHiddenCommandMode */

  /* authentication timeout */
  depth2 = ewsCliAddNode(depth1, "authentication ", pStrInfo_security_GlobalAuthHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "timeout ", pStrInfo_security_GlobalAuthTimeoutHelp, commandCPAuthTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_AUTH_SESSION_TIMEOUT_MIN, CP_AUTH_SESSION_TIMEOUT_MAX);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_CPTime,  NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* encoded-image mode */
  depth2 = ewsCliAddNode(depth1, "encoded-image", pStrInfo_security_CPEncodedImageMode, cliCaptivePortalEncodedImageMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* configuration <cp-id> */
  depth2 = ewsCliAddNode(depth1, "configuration ", pStrInfo_security_CPCfgMode, cliCaptivePortalInstanceMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_ID_MIN, CP_ID_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPCfgId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* enable */
  depth2 = ewsCliAddNode(depth1, "enable ", pStrInfo_security_CPEnableHelp, commandCPEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* exit */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* help */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* http port <port> */
  depth2 = ewsCliAddNode(depth1, "http ", pStrInfo_security_GlobalHttpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "port ", pStrInfo_security_GlobalHttpPortHelp, commandCPHTTPPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_HTTP_PORT_MIN, CP_HTTP_PORT_MAX);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_GlobalPort, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_MGMT_SECURITY_PACKAGE
  /* https port <port> */
  depth2 = ewsCliAddNode(depth1, "https ", pStrInfo_security_GlobalHttpSecureHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "port ", pStrInfo_security_GlobalHttpSecurePortHelp, commandCPHTTPSecurePort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_HTTP_SECURE_PORT_MIN, CP_HTTP_SECURE_PORT_MAX);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_GlobalSecurePort, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  /* statistics interval <interval> */
  if (usmDbCpdmClusterSupportGet() == L7_SUCCESS)
  {
    depth2 = ewsCliAddNode(depth1, "statistics ", pStrInfo_security_GlobalStatsHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, "interval ", pStrInfo_security_GlobalStatsIntervalHelp, commandCPStatisticsInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, "<0> ", pStrInfo_security_GlobalIntervalZero, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_PS_STATS_REPORT_INTERVAL_MIN, CP_PS_STATS_REPORT_INTERVAL_MAX);
    depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_GlobalInterval, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* trapflags */
  depth2 = ewsCliAddNode(depth1, "trapflags ", pStrInfo_security_CPTrapflagHelp, commandCPTrapFlags, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* trapflags client-auth-failure */
  depth3 = ewsCliAddNode(depth2, "client-auth-failure ", pStrInfo_security_CPTrapClientAuthFailure, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* trapflags client-connected */
  depth3 = ewsCliAddNode(depth2, "client-connect ", pStrInfo_security_CPTrapClientConnection, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* trapflags client-db-full */
  depth3 = ewsCliAddNode(depth2, "client-db-full ", pStrInfo_security_CPTrapClientDBFull, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* trapflags client-disconnected */
  depth3 = ewsCliAddNode(depth2, "client-disconnect ", pStrInfo_security_CPTrapClientDisconnection, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> */
  depth2 = ewsCliAddNode(depth1, "user ", pStrInfo_security_CPUserIDCfgHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", 1, FD_CP_LOCAL_USERS_MAX);                                                 
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPUserID, commandCPLocalUserDelete, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /* user <user-id> group <group-id> */
  depth4 = ewsCliAddNode(depth3, "group ", pStrInfo_security_CPUserGroupIDHelp, commandCPLocalUserGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", GP_ID_MIN, GP_ID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPUserGroupID, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* user <user-id> idle-timeout <time> */
  depth4 = ewsCliAddNode(depth3, "idle-timeout ", pStrInfo_security_CPUserIdleTimeoutHelp, commandCPLocalUserIdleTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_USER_LOCAL_IDLE_TIMEOUT_MIN, CP_USER_LOCAL_IDLE_TIMEOUT_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> name <name> */
  depth4 = ewsCliAddNode(depth3, "name ", pStrInfo_security_CPUserNameHelp, commandCPLocalUserName, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "<name> ", pStrInfo_security_CPUserName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> password */
  depth4 = ewsCliAddNode(depth3, "password ", pStrInfo_security_CPUserPwdHelp, commandCPLocalUserAddUpdate, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> password encrypted <encrypted-password> */
  depth5 = ewsCliAddNode(depth4, "encrypted ", pStrInfo_security_CPUserEncryHelp, commandCPLocalUserPasswordEncrypted, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "<encrypted-password> ", pStrInfo_security_CPUserEncryPwd, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> session-timeout <time> */
  depth4 = ewsCliAddNode(depth3, "session-timeout ", pStrInfo_security_CPUserSessionTimeoutHelp, commandCPLocalUserSessionTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_USER_LOCAL_SESSION_TIMEOUT_MIN, CP_USER_LOCAL_SESSION_TIMEOUT_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> max-bandwidth-up <bytes/sec> */
  depth4 = ewsCliAddNode(depth3, "max-bandwidth-up ", pStrInfo_security_CPMaxBandwidthUpHelp, commandCPLocalUserMaxBandwidthUp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_BW_UP_MIN, CP_USER_LOCAL_MAX_BW_UP_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPMaxBandwidthUp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> max-bandwidth-down <bytes/sec> */
  depth4 = ewsCliAddNode(depth3, "max-bandwidth-down ", pStrInfo_security_CPMaxBandwidthDownHelp, commandCPLocalUserMaxBandwidthDown, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_BW_DOWN_MIN, CP_USER_LOCAL_MAX_BW_DOWN_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPMaxBandwidthDown, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> max-input-octets <bytes> */
  depth4 = ewsCliAddNode(depth3, "max-input-octets ", pStrInfo_security_CPMaxInputOctetsHelp, commandCPLocalUserMaxInputOctets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPMaxInputOctets, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> max-output-octets <bytes> */
  depth4 = ewsCliAddNode(depth3, "max-output-octets ", pStrInfo_security_CPMaxOutputOctetsHelp, commandCPLocalUserMaxOutputOctets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPMaxOutputOctets, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user <user-id> max-total-octets <bytes> */
  depth4 = ewsCliAddNode(depth3, "max-total-octets ", pStrInfo_security_CPMaxTotalOctetsHelp, commandCPLocalUserMaxTotalOctets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPMaxTotalOctets, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user group <group-id>... */
  depth3 = ewsCliAddNode(depth2, "group ", pStrInfo_security_GroupIDHelp, commandCPUserGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", GP_ID_MIN, GP_ID_MAX);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_CPGroupID, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user group <group-id> name <name> */
  depth5 = ewsCliAddNode(depth4, "name ", pStrInfo_security_GroupNameHelp, commandCPUserGroupName, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "<name> ", pStrInfo_security_GroupName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user group <group-id> moveusers <new-group-id> */
  depth5 = ewsCliAddNode(depth4, "moveusers ", pStrInfo_security_GroupMoveUsersHelp, commandCPUserGroupMoveUsers, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "<destination-group-id> ", pStrInfo_security_GroupMoveUsersNewID, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the Captive Portal Instance configuration commands tree
*
* @param  void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*********************************************************************/
void buildTreeCaptivePortalInstanceConfig()
{
  EwsCliCommandP depth1, depth2, depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliCaptivePortalInstanceMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_CPINSTANCE_MODE, depth1);

  /* block */
  depth2 = ewsCliAddNode(depth1, "block ", pStrInfo_security_BlockHelp, commandCPConfigurationBlock, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* clear */
  depth2 = ewsCliAddNode(depth1, "clear ", pStrInfo_security_CPClearHelp, commandCPConfigurationClear, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* enable */
  depth2 = ewsCliAddNode(depth1, "enable ", pStrInfo_security_CPEnableInstance, commandCPConfigurationEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* exit */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* group */
  depth2 = ewsCliAddNode(depth1, "group ", pStrInfo_security_CPGroupIDHelp, commandCPConfigurationGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", GP_ID_MIN, GP_ID_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPGroupID, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* help */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Help_2, pStrInfo_common_DispHelpForVariousSpecialKeys, commandError, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* idle-timeout */
  depth2 = ewsCliAddNode(depth1, "idle-timeout ", pStrInfo_security_CPIdleTimeout, commandCPConfigurationIdleTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_USER_LOCAL_IDLE_TIMEOUT_MIN, CP_USER_LOCAL_IDLE_TIMEOUT_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* interface */
  depth2 = ewsCliAddNode(depth1, "interface ", pStrInfo_security_CPInterfaceCfg, commandCPConfigurationInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = buildTreeInterfaceHelp(depth2, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* locale <web-id> */
  depth2 = ewsCliAddNode(depth1, "locale ", pStrInfo_security_CPCfgLocaleMode, cliCaptivePortalInstanceLocaleMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_WEB_ID_MIN, CP_WEB_ID_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPLocaleWebId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* name */
  depth2 = ewsCliAddNode(depth1, "name ", pStrInfo_security_CPNameHelp, commandCPConfigurationName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<cp-name> ", pStrInfo_security_CPIDName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* protocol */
  depth2 = ewsCliAddNode(depth1, "protocol ", pStrInfo_security_CPProtocol, commandCPConfigurationProtocol, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "http ", pStrInfo_security_CPProtocolHTTPHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "https ", pStrInfo_security_CPProtocolHTTPSHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* radius authentication server */
  depth2 = ewsCliAddNode(depth1, "radius-auth-server ", pStrInfo_security_CPRadiusAuthServerHelp, commandCPConfigurationRadiusAuthServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<server-name> ", pStrInfo_security_CPRadiusAuthServer, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user-logout */
  depth2 = ewsCliAddNode(depth1, "user-logout ", pStrInfo_security_CPUserLogoutHelp, commandCPConfigurationUserLogout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* redirect */
  depth2 = ewsCliAddNode(depth1, "redirect ", pStrInfo_security_CPRedirectHelp, commandCPConfigurationRedirect, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* redirect url */
  depth2 = ewsCliAddNode(depth1, "redirect-url ", pStrInfo_security_CPRedirectUrlHelp, commandCPConfigurationRedirectUrl, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "<url> ", pStrInfo_security_CPRedirectUrl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* session-timeout */
  depth2 = ewsCliAddNode(depth1, "session-timeout ", pStrInfo_security_CPSessionTimeout, commandCPConfigurationSessionTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_USER_LOCAL_SESSION_TIMEOUT_MIN, CP_USER_LOCAL_SESSION_TIMEOUT_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* max-bandwidth-up <bytes/sec> */
  depth2 = ewsCliAddNode(depth1, "max-bandwidth-up ", pStrInfo_security_CPMaxBandwidthUpHelp, commandCPConfigurationMaxBandwidthUp, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_BW_UP_MIN, CP_USER_LOCAL_MAX_BW_UP_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPMaxBandwidthUp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* max-bandwidth-down <bytes/sec> */
  depth2 = ewsCliAddNode(depth1, "max-bandwidth-down ", pStrInfo_security_CPMaxBandwidthDownHelp, commandCPConfigurationMaxBandwidthDown, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_BW_DOWN_MIN, CP_USER_LOCAL_MAX_BW_DOWN_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPMaxBandwidthDown, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* max-input-octets <bytes> */
  depth2 = ewsCliAddNode(depth1, "max-input-octets ", pStrInfo_security_CPMaxInputOctetsHelp, commandCPConfigurationMaxInputOctets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPMaxInputOctets, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* max-output-octets <bytes> */
  depth2 = ewsCliAddNode(depth1, "max-output-octets ", pStrInfo_security_CPMaxOutputOctetsHelp, commandCPConfigurationMaxOutputOctets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPMaxOutputOctets, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* max-total-octets <bytes> */
  depth2 = ewsCliAddNode(depth1, "max-total-octets ", pStrInfo_security_CPMaxTotalOctetsHelp, commandCPConfigurationMaxTotalOctets, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  osapiSnprintf(buf, sizeof(buf), "<%d-%u> ", CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_security_CPMaxTotalOctets, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* verification */
  depth2 = ewsCliAddNode(depth1, "verification ", pStrInfo_security_CPVerifyMode, commandCPConfigurationVerifyMode, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "guest ", pStrInfo_security_CPVerifyModeGuest, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "local ", pStrInfo_security_CPVerifyModeLocal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, "radius ", pStrInfo_security_CPVerifyModeRadius, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* foreground-color */
  depth2 = ewsCliAddNode(depth1, "foreground-color ", pStrInfo_security_CPCfgForegroundColorHelp, commandCPConfigurationForegroundColor, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<color-code> ", pStrInfo_security_CPCfgColor, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* separator-color */
  depth2 = ewsCliAddNode(depth1, "background-color ", pStrInfo_security_CPCfgBackgroundColorHelp, commandCPConfigurationBackgroundColor, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<color-code> ", pStrInfo_security_CPCfgColor, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* separator-color */
  depth2 = ewsCliAddNode(depth1, "separator-color ", pStrInfo_security_CPCfgSeparatorColorHelp, commandCPConfigurationSeparatorColor, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<color-code> ", pStrInfo_security_CPCfgColor, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
* @purpose  Build the Captive Portal Instance Locale commands tree
*
* @param  void
*
* @returntype struct EwsCliCommandP
*
* @notes This entire node is hidden under buildTreeHiddenCommandMode
*
* @end
*********************************************************************/
void buildTreeCaptivePortalInstanceLocaleConfig()
{
  EwsCliCommandP depth1, depth2, depth3, depth4;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliCaptivePortalInstanceLocaleMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_CPINSTANCE_LOCALE_MODE, depth1);

  /* account-image */
  depth2 = ewsCliAddNode(depth1, "account-image ", pStrInfo_security_CPLocaleAccountImageHelp, commandCPLocaleAccountImage, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<image-name> ", pStrInfo_security_CPLocaleAccountImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* account-label */
  depth2 = ewsCliAddNode(depth1, "account-label ", pStrInfo_security_CPLocaleAccountLabelHelp, commandCPLocaleAccountLabel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleAccountLabel, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* accept-msg */
  depth2 = ewsCliAddNode(depth1, "accept-msg ", pStrInfo_security_CPLocaleAcceptMsgHelp, commandCPLocaleAcceptMsg, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleAcceptMsg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* accept-text */
  depth2 = ewsCliAddNode(depth1, "accept-text ", pStrInfo_security_CPLocaleAcceptTextHelp, commandCPLocaleAcceptText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleAcceptText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* aup-text */
  depth2 = ewsCliAddNode(depth1, "aup-text ", pStrInfo_security_CPLocaleAUPTextHelp, commandCPLocaleAUPText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleAUPText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* background-image */
  depth2 = ewsCliAddNode(depth1, "background-image ", pStrInfo_security_CPLocaleBackgroundImageHelp, commandCPLocaleBackgroundImage, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<image-name> ", pStrInfo_security_CPLocaleBackgroundImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* button-label */
  depth2 = ewsCliAddNode(depth1, "button-label ", pStrInfo_security_CPLocaleButtonLabelHelp, commandCPLocaleButtonLabel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleButtonLabel, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* branding-image */
  depth2 = ewsCliAddNode(depth1, "branding-image ", pStrInfo_security_CPLocaleBrandingImageHelp, commandCPLocaleBrandingImage, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<image-name> ", pStrInfo_security_CPLocaleBrandingImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* browser-title */
  depth2 = ewsCliAddNode(depth1, "browser-title ", pStrInfo_security_CPLocaleBrowserTitleHelp, commandCPLocaleBrowserTitle, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleBrowserTitle, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* code */
  depth2 = ewsCliAddNode(depth1, "code ", pStrInfo_security_CPLocaleCodeHelp, commandCPLocaleCode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<locale-code> ", pStrInfo_security_CPLocaleCode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* denied-msg */
  depth2 = ewsCliAddNode(depth1, "denied-msg ", pStrInfo_security_CPLocaleDeniedMsgHelp, commandCPLocaleDeniedMsg, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleDeniedMsg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* exit */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* font-list */
  depth2 = ewsCliAddNode(depth1, "font-list ", pStrInfo_security_CPLocaleFontListHelp, commandCPLocaleFontList, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<font,font> ", pStrInfo_security_CPLocaleFontList, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* instructional-text */
  depth2 = ewsCliAddNode(depth1, "instructional-text ", pStrInfo_security_CPLocaleInstructionalTextHelp, commandCPLocaleInstructionalText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleInstructionalText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* link */
  depth2 = ewsCliAddNode(depth1, "link ", pStrInfo_security_CPLocaleLinkHelp, commandCPLocaleLink, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLink, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-success-background-image */
  depth2 = ewsCliAddNode(depth1, "logout-success-background-image ", pStrInfo_security_CPLocaleLogoutSuccessBackgroundImageHelp, commandCPLocaleLogoutSuccessBackgroundImage, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<image-name> ", pStrInfo_security_CPLocaleLogoutSuccessBackgroundImage, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* password-label */
  depth2 = ewsCliAddNode(depth1, "password-label ", pStrInfo_security_CPLocalePasswordLabelHelp, commandCPLocalePasswordLabel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocalePasswordLabel, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* resource-msg */
  depth2 = ewsCliAddNode(depth1, "resource-msg ", pStrInfo_security_CPLocaleResourceMsgHelp, commandCPLocaleResourceMsg, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleResourceMsg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* title-text */
  depth2 = ewsCliAddNode(depth1, "title-text ", pStrInfo_security_CPLocaleTitleTextHelp, commandCPLocaleTitleText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleTitleText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* timeout-msg */
  depth2 = ewsCliAddNode(depth1, "timeout-msg ", pStrInfo_security_CPLocaleTimeoutMsgHelp, commandCPLocaleTimeoutMsg, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleTimeoutMsg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* user-label */
  depth2 = ewsCliAddNode(depth1, "user-label ", pStrInfo_security_CPLocaleUserLabelHelp, commandCPLocaleUserLabel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleUserLabel, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* welcome-title */
  depth2 = ewsCliAddNode(depth1, "welcome-title ", pStrInfo_security_CPLocaleWelcomeTitleHelp, commandCPLocaleWelcomeTitle, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleWelcomeTitle, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* welcome-text */
  depth2 = ewsCliAddNode(depth1, "welcome-text ", pStrInfo_security_CPLocaleWelcomeTextHelp, commandCPLocaleWelcomeText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleWelcomeText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* wip-msg */
  depth2 = ewsCliAddNode(depth1, "wip-msg ", pStrInfo_security_CPLocaleWIPMsgHelp, commandCPLocaleWIPMsg, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleWIPMsg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* script-text */
  depth2 = ewsCliAddNode(depth1, "script-text ", pStrInfo_security_CPLocaleScriptTextHelp, commandCPLocaleScriptText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleScriptText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* popup-text */
  depth2 = ewsCliAddNode(depth1, "popup-text ", pStrInfo_security_CPLocalePopupTextHelp, commandCPLocalePopupText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocalePopupText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-browser-title */
  depth2 = ewsCliAddNode(depth1, "logout-browser-title ", pStrInfo_security_CPLocaleLogoutBrowserTitleHelp, commandCPLocaleLogoutBrowserTitle, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutBrowserTitle, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-title */
  depth2 = ewsCliAddNode(depth1, "logout-title ", pStrInfo_security_CPLocaleLogoutTitleHelp, commandCPLocaleLogoutTitle, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutTitle, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-text */
  depth2 = ewsCliAddNode(depth1, "logout-text ", pStrInfo_security_CPLocaleLogoutContentTextHelp, commandCPLocaleLogoutContentText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutContentText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-button-label */
  depth2 = ewsCliAddNode(depth1, "logout-button-label ", pStrInfo_security_CPLocaleLogoutButtonLabelHelp, commandCPLocaleLogoutButtonLabel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutButtonLabel, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-confirmation-text */
  depth2 = ewsCliAddNode(depth1, "logout-confirmation-text ", pStrInfo_security_CPLocaleLogoutConfirmTextHelp, commandCPLocaleLogoutConfirmText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutConfirmText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-success-browser-title */
  depth2 = ewsCliAddNode(depth1, "logout-success-browser-title ", pStrInfo_security_CPLocaleLogoutSuccessBrowserTitleHelp, commandCPLocaleLogoutSuccessBrowserTitle, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutSuccessBrowserTitle, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-success-title */
  depth2 = ewsCliAddNode(depth1, "logout-success-title ", pStrInfo_security_CPLocaleLogoutSuccessTitleHelp, commandCPLocaleLogoutSuccessTitle, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutSuccessTitle, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* logout-success-text */
  depth2 = ewsCliAddNode(depth1, "logout-success-text ", pStrInfo_security_CPLocaleLogoutSuccessTextHelp, commandCPLocaleLogoutSuccessContentText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<UTF-16> ", pStrInfo_security_CPLocaleLogoutSuccessText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the Captive Portal Encoded image commands tree
*
* @param  void
*
* @returntype struct EwsCliCommandP
*
* @notes This entire node is hidden under buildTreeHiddenCommandMode
*
* @end
*********************************************************************/
void buildTreeCaptivePortalEncodedImageConfig()
{
  EwsCliCommandP depth1, depth2, depth3, depth4;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliCaptivePortalEncodedImageMode, L7_NO_OPTIONAL_PARAMS);
  cliSetMode(L7_CPENCODED_IMAGE_MODE, depth1);

  /* decoded-image-size */
  depth2 = ewsCliAddNode(depth1, "decoded-image-size ", pStrInfo_security_CPDecodedImageSizeHelp, commandCPDecodedImageSize, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<bytes> ", pStrInfo_security_CPDecodedImageSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* encoded-image-text */
  depth2 = ewsCliAddNode(depth1, "encoded-image-text ", pStrInfo_security_CPEncodedImageTextHelp, commandCPEncodedImageText, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "<text> ", pStrInfo_security_CPEncodedImageText, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* exit */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the Captive Portal action commands in the Priv EXEC mode
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*********************************************************************/
void buildTreePrivCaptivePortal(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* captive-portal client deauthenticate */
  depth2 = ewsCliAddNode(depth1, "captive-portal ", pStrInfo_security_CaptivePortalHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth3 = ewsCliAddNode(depth2, "client ", pStrInfo_security_CPClientDeauth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, "deauthenticate ", pStrInfo_security_CPClientDeauthHelp, commandCPClientDeauthenticate, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "<cr> ", pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* captive-portal client deauthenticate <macaddr> */
  depth5 = ewsCliAddNode(depth4, "<macaddr> ", pStrInfo_security_CPClientMACHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  /* captive-portal client deauthenticate <cp-id> */
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_ID_MIN, CP_ID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPCfgId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the Captive Portal clear commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*********************************************************************/
void buildTreePrivCaptivePortalClear(EwsCliCommandP depth2)
{
  /* depth2 = clear */
  EwsCliCommandP depth3, depth4, depth5;

  /* clear captive-portal users */
  depth3 = ewsCliAddNode(depth2, "captive-portal ", pStrInfo_security_CPClear, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, "users ", pStrInfo_security_CPClearUsersHelp, commandCPLocalUserDeleteAll, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the Show Captive Portal Switch tree.
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*********************************************************************/
void buildTreePrivShowCaptivePortal(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* show captive-portal */ 
  depth2 = ewsCliAddNode(depth1, "captive-portal ", pStrInfo_security_CPShowInfo, commandShowCP, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal status */ 
  depth3 = ewsCliAddNode(depth2, "status ", pStrInfo_security_CPShowStatus, commandShowCPStatus, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


  /* show captive-portal configuration... */
  depth3 = ewsCliAddNode(depth2, "configuration ", pStrInfo_security_CPConfigurationHelp, commandShowCPConfiguration, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration <cp-id> */
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_ID_MIN, CP_ID_MAX);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_CPCfgId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration <cp-id> client status */
  depth5 = ewsCliAddNode(depth4, "client ", pStrInfo_security_CPClient, commandShowCPConfigurationClientStatus, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "status ", pStrInfo_security_CPClientStatus, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration <cp-id> interface */
  depth5 = ewsCliAddNode(depth4, "interface ", pStrInfo_security_CPConfigurationIntfHelp, commandShowCPConfigurationIntf, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration <cp-id> interface <intf> */
  depth6 = buildTreeInterfaceHelp(depth5, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration <cp-id> status */
  depth5 = ewsCliAddNode(depth4, "status ", pStrInfo_security_CPConfigurationStatus, commandShowCPConfigurationStatus, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration <cp-id> locales */
  depth5 = ewsCliAddNode(depth4, "locales ", pStrInfo_security_LocalesHelp, commandShowCPConfigurationLocales, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration status */
  depth4 = ewsCliAddNode(depth3, "status ", pStrInfo_security_CPConfigurationStatus, commandShowCPConfigurationStatus, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal configuration client status */
  depth4 = ewsCliAddNode(depth3, "client ", pStrInfo_security_CPClient, commandShowCPConfigurationClientStatus, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "status ", pStrInfo_security_CPClientStatus, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal client... */
  depth3 = ewsCliAddNode(depth2, "client ", pStrInfo_security_CPClient, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal client <macaddr> status */
  depth4 = ewsCliAddNode(depth3, "<macaddr> ", pStrInfo_security_CPClientMACHelp, NULL, 2, L7_NODE_DATA_TYPE, L7_MAC_ADDRESS_SVL_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, "status ", pStrInfo_security_CPClientStatus, commandShowCPClientStatus, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal client <macaddr> statistics */
  depth5 = ewsCliAddNode(depth4, "statistics ", pStrInfo_security_StatisticsHelp, commandShowCPClientStatistics, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal client status */
  depth4 = ewsCliAddNode(depth3, "status ", pStrInfo_security_CPClientStatus, commandShowCPClientStatus, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


  /* show captive portal interface... */
  depth3 = ewsCliAddNode(depth2, "interface ", pStrInfo_security_CPInterfaceHelp, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface <intf> client status */
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, "client ", pStrInfo_security_CPClient, commandShowCPIntfClientStatus, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "status ", pStrInfo_security_CPClientStatus, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface capability */
  depth4 = ewsCliAddNode(depth3, "capability ", pStrInfo_security_CapabilityStatus, commandShowCPInterfaceCapability, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface capability <intf> */
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface client status */
  depth4 = ewsCliAddNode(depth3, "client ", pStrInfo_security_CPClient, commandShowCPIntfClientStatus, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "status ", pStrInfo_security_CPClientStatus, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface configuration... */
  depth4 = ewsCliAddNode(depth3, "configuration ", pStrInfo_security_CPConfigurationHelp, commandShowCPIntfConfigurationStatus, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface configuration <cp-id> status */
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", CP_ID_MIN, CP_ID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPIdHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "status ", pStrInfo_security_CPIntfConfigurationStatus, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal interface configuration status */
  depth5 = ewsCliAddNode(depth4, "status ", pStrInfo_security_CPIntfConfigurationStatus, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal trapflags */
  depth3 = ewsCliAddNode(depth2, "trapflags ", pStrInfo_security_CPShowTrapflags, commandShowCPTrapFlags, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal user */
  depth3 = ewsCliAddNode(depth2, "user ", pStrInfo_security_CPShowUserLocalInfo, commandShowCPLocalUser, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal user <user-id> */
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", 1, FD_CP_LOCAL_USERS_MAX);                                                 
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_security_CPUserID, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


  /* show captive-portal user group */
  depth4 = ewsCliAddNode(depth3, "group ", pStrInfo_security_CPUserGroupHelp, commandShowCPUserGroup, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* show captive-portal user group <group-id> */
  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", GP_ID_MIN, GP_ID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_security_CPUserGroupID, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


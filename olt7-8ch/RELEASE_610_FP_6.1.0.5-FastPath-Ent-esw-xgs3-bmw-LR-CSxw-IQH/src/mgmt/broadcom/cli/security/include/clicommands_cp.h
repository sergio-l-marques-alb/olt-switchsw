/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   src/mgmt/cli/security/include/clicommands_cp.h
*
* @purpose    Header for Captive portal Command Line Interface
*
* @component  Command Line Interface
*
* @comments   None
*
* @create     07/10/2007
*
* @author     rjain, rjindal
*
* @end
*
*********************************************************************/

#ifndef CLICOMMANDSCAPTIVEPORTAL_H
#define CLICOMMANDSCAPTIVEPORTAL_H

#include "strlib_security_cli.h"
#include "l7_common.h"

/********************************************************************/
/*       Start Captive Portal Tree                                  */
/********************************************************************/
void buildTreeGlobalCaptivePortal(EwsCliCommandP depth1);
void buildTreeCaptivePortalConfig();
void buildTreeCaptivePortalGlobalConfig(EwsCliCommandP depth1);
void buildTreeCaptivePortalInstanceConfig();
void buildTreeCaptivePortalInstanceLocaleConfig();
void buildTreeCaptivePortalEncodedImageConfig();
void buildTreePrivCaptivePortal(EwsCliCommandP depth1);
void buildTreePrivCaptivePortalClear(EwsCliCommandP depth2);
void buildTreePrivShowCaptivePortal(EwsCliCommandP depth1);

/********************************************************************/
/*       Start Captive Portal Commands                              */
/********************************************************************/
/* global */
const L7_char8 *commandCPAuthTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPEnable(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPHTTPPort(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPHTTPSecurePort(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPStatisticsInterval(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPTrapFlags(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandSnmpServerEnableTrapsCP(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPDecodedImageSize(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPEncodedImageText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* cp configuration */
const L7_char8 *commandCPConfigurationBlock(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationClear(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationEnable(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationIdleTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationInterface(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationName(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationProtocol(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationUserLogout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationRedirect(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationRedirectUrl(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationSessionTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationMaxBandwidthUp(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationMaxBandwidthDown(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationMaxInputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationMaxOutputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationMaxTotalOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationVerifyMode(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationRadiusAuthServer(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationForegroundColor(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationBackgroundColor(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPConfigurationSeparatorColor(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* cp locales */
const L7_char8 *commandCPLocaleAccountImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleAccountLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleAcceptMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleAcceptText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleAUPText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleBackgroundImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleButtonLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleBrandingImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleBrowserTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleCode(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleDeniedMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleFontList(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleInstructionalText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutSuccessBackgroundImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLink(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalePasswordLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleResourceMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleTitleText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleTimeoutMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleUserLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleWelcomeTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleWelcomeText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleWIPMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleScriptText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalePopupText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutBrowserTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutContentText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutButtonLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutConfirmText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutSuccessBrowserTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutSuccessTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocaleLogoutSuccessContentText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* client deauthenticate */
const L7_char8 *commandCPClientDeauthenticate(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* clear */
const L7_char8 *commandCPLocalUserDeleteAll(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* local user */
const L7_char8 *commandCPLocalUserDelete(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserIdleTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserName(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserAddUpdate(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserPasswordEncrypted(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserSessionTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserMaxBandwidthUp(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserMaxBandwidthDown(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserMaxInputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserMaxOutputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPLocalUserMaxTotalOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* user group */
const L7_char8 *commandCPUserGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPUserGroupName(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandCPUserGroupMoveUsers(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

/* show */
const L7_char8 *commandShowCP(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPStatus(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPConfiguration(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPConfigurationClientStatus(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPConfigurationIntf(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPConfigurationStatus(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPConfigurationLocales(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPClientStatus(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPClientStatistics(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPIntfClientStatus(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPInterfaceCapability(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPIntfConfigurationStatus(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPTrapFlags(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPLocalUser(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandShowCPUserGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);

#endif /* CLICOMMANDSCAPTIVEPORTAL_H */


/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_mode.h
 *
 * @purpose header for cli_mode.h
 *
 * @component user interface
 *
 * @comments none
 *
 * @create   18/06/2003
 *
 * @author Jagdish
 * @end
 *
 **********************************************************************/

#ifndef CLIMODE_H
#define CLIMODE_H

const L7_char8 *cliPromptSet(L7_char8 * tmpPrompt,const L7_char8 * * argv, EwsContext ewsContext,EwsCliCommandP menuTree);

const L7_char8 *cliCheckCommonConditions(L7_uint32 depth, L7_char8 * tmpUpPrompt, L7_uint32 argc,const L7_char8 * * argv, EwsContext ewsContext, EwsCliCommandP menuTree);

const L7_char8 *cliCheckNoForm(EwsContext ewsContext);

const L7_char8 *cliCommonPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,EwsContext ewsContext, EwsCliCommandP upMode, EwsCliCommandP currMode);

const L7_char8 *cliConsolePrompt(L7_uint32 depth, L7_char8 * tmpPrompt,L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,EwsContext ewsContext, EwsCliCommandP upMode, EwsCliCommandP currMode);

const L7_char8 *cliSshPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,EwsContext ewsContext, EwsCliCommandP upMode, EwsCliCommandP currMode);

const L7_char8 *cliTelnetPrompt(L7_uint32 depth, L7_char8 * tmpPrompt,L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv,EwsContext ewsContext, EwsCliCommandP upMode, EwsCliCommandP currMode);

const L7_char8 *cliInterfacePrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

const L7_char8 *cliPriviledgeUserPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

const L7_char8 *cliVlanPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

const L7_char8 *cliBGPRouterPrompt (L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

const L7_char8 *cliDhcpsPoolConfigPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt,L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
const L7_char8 *cliDhcp6sPoolConfigPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt,L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
const L7_char8 *cliWirelessAPPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliWirelessNetworkPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliAPProfilePrompt (L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliAPProfileRadioPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliAPProfileVAPPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliWirelessWdsApGroupPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliBuildingPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliFloorPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
const L7_char8 *cliCaptivePortalEncodedImagePrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliCaptivePortalInstancePrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
const L7_char8 *cliCaptivePortalInstanceLocalePrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);
#endif

const L7_char8 *cliIntAuthServUserPrompt(L7_uint32 depth, L7_char8 * tmpPrompt, L7_char8 * tmpUpPrompt, L7_uint32 argc, const L7_char8 * * argv, EwsContext ewsContext);

#endif

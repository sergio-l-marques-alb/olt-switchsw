/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_prompt.c
 *
 * @purpose Command mode functions
 *
 * @component user interface
 *
 * @comments
 *
 * @create   18/06/2003
 *
 * @author   jagdishc
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "cli_mode.h"

#ifdef L7_QOS_FLEX_PACKAGE_ACL
#include "clicommands_acl.h"
#endif

#include "clicommands_tacacs.h"

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "strlib_security_cli.h"
#include "cli_web_exports.h"
#include "captive_portal_commdefs.h"
#endif

/*********************************************************************
*
* @purpose Function to set or exit from the user priviledge mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliUserPriviledgeMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliPriviledgeUserPrompt(index+1, pStrInfo_common_PriviledgeUsrExecModePrompt,pStrInfo_base_RootPrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliCommonPrompt(index+1, pStrInfo_common_GlobalCfgModePrompt,pStrInfo_common_PriviledgeUsrExecModePrompt, argc, argv, ewsContext, cliGetMode(L7_PRIVILEGE_USER_MODE), cliGetMode(L7_GLOBAL_CONFIG_MODE));
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliSupportMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliCommonPrompt(index+1, pStrInfo_common_GlobalSuppModePrompt,pStrInfo_common_PriviledgeUsrExecModePrompt, 
                         argc, argv, ewsContext, cliGetMode(L7_PRIVILEGE_USER_MODE), cliGetMode(L7_GLOBAL_SUPPORT_MODE));
}
/* The following support modes are required in non wireless builds 
   to support text based configuration */

#ifndef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose Function to set or exit from the cliWirelessConfigSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  if( (argc == 1) && (strcmp(argv[0], pStrInfo_common_Exit) == 0))
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    cliSetTree(cliGetMode(L7_GLOBAL_CONFIG_MODE));
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  }
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose Function to set or exit from the cliWirelessAPConfigSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessAPModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 * * argv, L7_uint32 index)
{
  if( (argc == 1) && (strcmp(argv[0], pStrInfo_common_Exit) == 0))
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    cliSetTree((cliGetMode(L7_WIRELESS_CONFIG_MODE)));
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  }
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileConfigSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessNetworkModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  if( (argc == 1) && (strcmp(argv[0], pStrInfo_common_Exit) == 0))
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    cliSetTree((cliGetMode(L7_WIRELESS_CONFIG_MODE)));
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  }
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileConfigSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 * * argv, L7_uint32 index)
{
  if( (argc == 1) && (strcmp(argv[0], pStrInfo_common_Exit) == 0))
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    cliSetTree((cliGetMode(L7_WIRELESS_CONFIG_MODE)));
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  }
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileRadioConfigSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileRadioModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  if( (argc == 1) && (strcmp(argv[0], pStrInfo_common_Exit) == 0))
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    cliSetTree(cliGetMode(L7_WIRELESS_AP_PROFILE_CONFIG_MODE));
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  }
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileVAPConfigSupport mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileVAPModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{
  if( (argc == 1) && (strcmp(argv[0], pStrInfo_common_Exit) == 0))
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
    cliSetTree(cliGetMode(L7_WIRELESS_AP_PROFILE_RADIO_CONFIG_MODE));
  }
  else
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  }
  return cliPrompt (ewsContext);
}
#endif

#ifdef L7_WIRELESS_PACKAGE
/*********************************************************************
*
* @purpose Function to set or exit from the cliWirelessConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessMode(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  return cliCommonPrompt (index+1, pStrInfo_base_WsCfgModePrompt,
                          pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext,
                          cliGetMode(L7_GLOBAL_CONFIG_MODE),
                          cliGetMode(L7_WIRELESS_CONFIG_MODE));
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliWirelessAPConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessAPMode(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 * * argv, L7_uint32 index)
{
  return cliWirelessAPPrompt (index+1, pStrInfo_base_WsApCfgModePrompt,
                              pStrInfo_base_WsCfgModePrompt,
                              argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessNetworkMode(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  return cliWirelessNetworkPrompt(index+1, pStrInfo_base_WsNwCfgModePrompt,
                                  pStrInfo_base_WsCfgModePrompt,
                                  argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileMode(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 * * argv, L7_uint32 index)
{
  return cliAPProfilePrompt (index+1, pStrInfo_base_WsApProfileCfgModePrompt,
                             pStrInfo_base_WsCfgModePrompt,
                             argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileRadioConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileRadioMode(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  return cliAPProfileRadioPrompt (index+1,
                                  pStrInfo_base_WsApProfileRadioCfgModePrompt,
                                  pStrInfo_base_WsApProfileCfgModePrompt,
                                  argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileVAPConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliAPProfileVAPMode(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{
  return cliAPProfileVAPPrompt (index+1,
                                pStrInfo_base_WsApProfileVapCfgModePrompt,
                                pStrInfo_base_WsApProfileRadioCfgModePrompt,
                                argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliAPProfileConfig mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliWirelessWdsApGroupConfigMode(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  return cliWirelessWdsApGroupPrompt(index+1, pStrInfo_base_WsWdsGrpCfgModePrompt,
                                     pStrInfo_base_WsCfgModePrompt,
                                     argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliDevLocBldngMode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliDevLocBldngMode(EwsContext ewsContext, 
                                   L7_uint32 argc,
                                   const L7_char8 * * argv, 
                                   L7_uint32 index)
{

  return cliBuildingPrompt (index+1,
                            pStrInfo_base_WsDevLocBldngCfgModePrompt,
                            pStrInfo_base_WsCfgModePrompt,
                            argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the cliDevLocBldngFlrMode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliDevLocBldngFlrMode(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  return cliFloorPrompt  (index+1, pStrInfo_base_WsDevLocBldngFlrCfgModePrompt,
                          pStrInfo_base_WsDevLocBldngCfgModePrompt,
                          argc, argv, ewsContext);
}
#endif /* L7_WIRELESS_PACKAGE */


/*********************************************************************
*
* @purpose Function to set or exit from the cli interface mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliInterfaceMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliInterfacePrompt(index+1, pStrInfo_base_IntfModePrompt,
                            pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Function to set or exit from the Line config mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliLineConfigConsoleMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliConsolePrompt(index+1, pStrInfo_base_LineConsoleModePrompt,pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE), cliGetMode(L7_LINE_CONFIG_CONSOLE_MODE));
}

const L7_char8 *cliLineConfigSshMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliSshPrompt(index+1, pStrInfo_base_LineSshModePrompt,pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE), cliGetMode(L7_LINE_CONFIG_SSH_MODE));
}

const L7_char8 *cliLineConfigTelnetMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliTelnetPrompt(index+1, pStrInfo_base_LineTelnetModePrompt,pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext, cliGetMode(L7_GLOBAL_CONFIG_MODE), cliGetMode(L7_LINE_CONFIG_TELNET_MODE));
}

#ifdef L7_DHCPS_PACKAGE
/*********************************************************************
*
* @purpose Function to set or exit from the DHCP pool config mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliDhcpsPoolConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDhcpsPoolConfigPrompt(index+1, pStrInfo_base_DhcpPoolCfgModePrompt, pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}
#endif

/*********************************************************************
*
* @purpose Function to set or exit from the cli TACACS mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliTacacsMode(EwsContext ewsContext,
                              L7_uint32 argc,
                              const L7_char8 * * argv,
                              L7_uint32 index)
{
  return cliTacacsPrompt(index+1, pStrInfo_base_TacacsModePrompt,
                         pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext,
                         index);
}
/*********************************************************************
*
* @purpose Function to set or exit from the cli Mail Server mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliMailServerMode(EwsContext ewsContext,
                              L7_uint32 argc,
                              const L7_char8 * * argv,
                              L7_uint32 index)
{
  return cliMailServerPrompt(index+1, pStrInfo_base_MailServerModePrompt,
                         pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext,
                         index);
}

#ifdef L7_ROUTING_PACKAGE
#ifdef L7_IPV6_PACKAGE
/*********************************************************************
*
* @purpose Function to set or exit from the IPv6 DHCP pool config mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliDhcp6sPoolConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  return cliDhcp6sPoolConfigPrompt(index+1, pStrInfo_base_Dhcp6sPoolCfgModePrompt, pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext);
}
#endif
#endif

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
/*********************************************************************
*
* @purpose Function to set or exit from the captive portal mode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  return cliCommonPrompt (index+1, "(Config-CP)#",
                          pStrInfo_common_GlobalCfgModePrompt, argc, argv, ewsContext,
                          cliGetMode(L7_GLOBAL_CONFIG_MODE),
                          cliGetMode(L7_CAPTIVEPORTAL_MODE));
}

/*********************************************************************
*
* @purpose Action function for the Captive Portal instance mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalInstanceMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  return cliCaptivePortalInstancePrompt(index+1, "(Config-CP ", "(Config-CP)#", 
                                        argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Action function for the Captive Portal instance locale mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalInstanceLocaleMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_char8 prompt[L7_PROMPT_SIZE];
  L7_char8 upPrompt[L7_PROMPT_SIZE];
  cpId_t cpId = 0;

  cpId = EWSCPID(ewsContext);
  memset(prompt,0,sizeof(prompt));
  memset(upPrompt,0,sizeof(upPrompt));
  osapiSnprintfcat(prompt,sizeof(prompt),"(Config-CP %d ",cpId);
  osapiSnprintfcat(upPrompt,sizeof(upPrompt),"(Config-CP %d)#",cpId);

  return cliCaptivePortalInstanceLocalePrompt(index+1, prompt, upPrompt, 
                                        argc, argv, ewsContext);
}

/*********************************************************************
*
* @purpose Action function for the Captive Portal encoded image mode
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8*
*
* @returns cliPrompt(ewsContext)
* @returns NULL - for error cases
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliCaptivePortalEncodedImageMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  return cliCaptivePortalEncodedImagePrompt(index+1, "(Config-CP-EI", "(Config-CP)#",
                                        argc, argv, ewsContext);
}


#endif

/*********************************************************************
*
* @purpose Function to set or exit from the cliDot1xAuthServUserConfigMode.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
*
* @notes
*
* @end
*
*********************************************************************/
const L7_char8 *cliIntAuthServUserConfigMode(EwsContext ewsContext, 
                                   L7_uint32 argc,
                                   const L7_char8 * * argv, 
                                   L7_uint32 index)
{

  return cliIntAuthServUserPrompt (index+1,
                            pStrInfo_base_IASUserCfgModePrompt,
                            pStrInfo_common_GlobalCfgModePrompt,
                            argc, argv, ewsContext);
}


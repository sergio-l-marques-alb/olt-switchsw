/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config.h
 *
 * @purpose header for clicommands.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  19/08/2003
 *
 * @author  Samip
 * @end
 *
 **********************************************************************/

#ifndef CLI_SHOW_RUNNING_CONFIG_H
#define CLI_SHOW_RUNNING_CONFIG_H

L7_RC_t cliRunningConfigUdpRelayInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigArpInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigBootpDhcpRelayInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInternalVlan(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigAclInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigIpv6AclInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigMacAclInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningInterfaceAclInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningVlanAclInfo(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliRunningConfigDiffservInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigBgpInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigMcastInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigMiscInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigOspfInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigOspfv3Info(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigRIPInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigVrrpInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningVisibleRoutingInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigStackingInfo(EwsContext ewsContext);
L7_RC_t cliRunningConfigStackPortInfo(EwsContext ewsContext);
L7_RC_t cliRunningConfigGlobalDVlanInfo(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliRunningConfigSlotInfo(EwsContext ewsContext);
L7_RC_t cliRunningSNTPInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningSyslogInfo(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliShowCmdEnable (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdBoolean (EwsContext context, L7_uint32 val, L7_char8 * cmd);
L7_RC_t cliShowCmdTrue (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdInt (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdIntAddInt (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd, L7_uint32 add);
L7_RC_t cliShowCmdIpAddr (EwsContext context, L7_uchar8 family, void *val, void *def, L7_char8 * cmd);
L7_RC_t cliShowCmdStr (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd,L7_char8 * str);
L7_RC_t cliShowCmdQuotedStrcmp (EwsContext context, L7_char8 * val, L7_char8 * def, L7_char8 * cmd);
L7_RC_t cliShowCmdDisable (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdIntAddValue (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd, L7_uint32 val2);
L7_RC_t cliRunningInterfaceConfigLACPInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigDot3Info(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningPbVlanPhysicalInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigProtocolInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigProtocolVlanInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigIpSubnetVlanInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigMacVlanInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigDhcps(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliRunningConfigMacfilterPhyIntfInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigMacfilterInfo(EwsContext ewsContext, L7_uint32 unit);

#ifdef L7_IPV6_PACKAGE
L7_RC_t cliRunningConfigIPv6Global(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigIPv6DhcpGlobal(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceIpv6(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigIpv6RouteInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigIpv6Info(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
#endif


L7_RC_t cliRunningDoSInfo(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliRunningConfigGlobalCosQueue(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceCosQueue(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);

L7_RC_t cliRunningConfigGlobalClassofService(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceClassofService(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigClassofserviceInfo(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliRunningPhysicalInterfaceDiffservInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);

L7_RC_t cliRunningConfigAutoVoIPInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceAutoVoIPInfo(EwsContext ewsContext, L7_uint32 interface);
#ifdef L7_QOS_FLEX_PACKAGE_ISCSI
L7_RC_t cliRunningGlobalConfigIscsiInfo(EwsContext ewsContext);
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
L7_RC_t cliRunningConfigHttpSecureServerInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigSshInfo(EwsContext ewsContext, L7_uint32 unit);
#endif

L7_RC_t cliRunningTelnetEnableInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigEnable(EwsContext ewsContext);


#ifdef L7_WIRELESS_PACKAGE
L7_RC_t cliRunningConfigWirelessInfo(EwsContext ewsContext);
L7_BOOL cliRunningConfigWirelessIsCmdAPPwd(L7_char8 *buf);
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
L7_RC_t cliRunningConfigCPInfo(EwsContext ewsContext);
L7_BOOL cliRunningConfigCPIsCmdUserPwd(L7_char8 *buf);
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */

L7_RC_t cliRunningConfigGlobalRandomDetect(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceRandomDetect(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigGlobalTailDrop(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceTailDrop(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigInterfaceClassofServiceDot1p(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);

L7_RC_t cliRunningConfigClassofserviceInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigPmlGlobalInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigPmlInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningConfigMcastMrouteBoundaryInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);

L7_RC_t cliRunningInterfaceConfigLACPInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningInterfaceConfigLLDPInfo(EwsContext ewsContext, L7_uint32 interface);
L7_RC_t cliRunningConfigLLDPInfo(EwsContext ewsContext);
L7_RC_t cliRunningConfigDebuggingInfo(EwsContext ewsContext);
L7_RC_t cliRunningTelnetInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t  cliRunningConfigChassisInfo(EwsContext ewsContext);
L7_RC_t  cliRunningConfigChassisSlotInfo(EwsContext ewsContext);
#ifdef L7_DHCP_FILTER_PACKAGE
L7_RC_t cliRunningConfigDhcpFilteringInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigDhcpFilteringInterfaceInfo(EwsContext ewsContext, L7_uint32 interface);
#endif
#ifdef L7_DHCP_SNOOPING_PACKAGE
L7_RC_t cliRunningConfigDhcpSnoopingInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigIpsgInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigDhcpSnoopingInterfaceInfo(EwsContext ewsContext, L7_uint32 interface);
L7_RC_t cliRunningConfigIpsgInterfaceInfo(EwsContext ewsContext, L7_uint32 intIfNum);
#endif
#ifdef L7_DAI_PACKAGE
L7_RC_t cliRunningConfigDaiInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigDaiInterfaceInfo(EwsContext ewsContext, L7_uint32 intIfNum);
L7_RC_t cliRunningConfigArpAclInfo(EwsContext ewsContext);
#endif
L7_RC_t cliRunningGlobalConfigTACACSInfo(EwsContext ewsContext, L7_uint32 unit);

L7_RC_t cliShowCmdEnable (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdBoolean (EwsContext context, L7_uint32 val, L7_char8 * cmd);
L7_RC_t cliShowCmdTrue (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdInt (EwsContext context, L7_uint32 val, L7_uint32 def, L7_char8 * cmd);
L7_RC_t cliShowCmdIpAddr (EwsContext context, L7_uchar8 family, void *val, void *def, L7_char8 * cmd);
L7_RC_t cliRunningConfigVoiceVlanInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigInterfaceVoiceVlanInfo(EwsContext ewsContext, L7_uint32 interface);
L7_RC_t cliRunningConfigDnsClientInfo(EwsContext ewsContext,
                                                     L7_uint32 unitIndex);

L7_RC_t cliRunningConfigGlobalPoeInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningInterfacePoeInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);

#ifdef L7_ISDP_PACKAGE
L7_RC_t cliRunningInterfaceConfigIsdpInfo(EwsContext ewsContext, L7_uint32 interface);
L7_RC_t cliRunningConfigIsdpInfo(EwsContext ewsContext);
#endif /* L7_ISDP_PACKAGE */

L7_RC_t cliRunningConfigsFlowInterfaceInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
void cliRunningConfigdot1adServiceSubscInfo(EwsContext ewsContext, 
                                              L7_uint32 intIfNum);
void cliRunningConfigdot1adService(EwsContext ewsContext);
void cliRunningConfigDot1adMode(EwsContext ewsContext);
void cliRunningConfigdot1adL2ptInfo(EwsContext ewsContext);
#endif
#ifdef L7_DOT3AH_PACKAGE
L7_RC_t cliRunningConfigdot3ahInfo(EwsContext ewsContext, L7_uint32 unit,
                L7_uint32 intf);
#endif
#endif

#define displayModeCommand( flag, command )       do                                         \
                                                  {                                          \
                                                    if ( ( L7_FALSE == flag ) &&             \
                                                         ( strlen( command ) > 0 ) )         \
                                                    {                                        \
                                                      EWSWRITEBUFFER( ewsContext, command ); \
                                                      flag = L7_TRUE;                        \
                                                    }                                        \
                                                  } while ( 0 )

#endif

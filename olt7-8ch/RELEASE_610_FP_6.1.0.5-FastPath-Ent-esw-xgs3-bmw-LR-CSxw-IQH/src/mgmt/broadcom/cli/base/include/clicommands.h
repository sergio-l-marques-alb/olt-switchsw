/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/clicommands.h
*
* @purpose header for clicommands.c
*
* @component user interface
*
* @comments none
*
* @create  05/29/2000
*
* @author  Forrest Samuels and Qun He
* @end
*
**********************************************************************/

#ifndef CLICOMMANDS_H
#define CLICOMMANDS_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l3_bgp_commdefs.h"

#include "ews.h"
#include "cli.h"

#define L7_CLI_MAX_COMMAND_LENGTH EWS_CLI_MAX_LINE        /* All command and parameter strings must be less than this length */

#define BUF_BANNER_DISPLAY_SIZE             4096
#define CLI_CONSOLE_WIDTH                   80

#define CLI_MAXSIZE_LOGIN_CHALLENGE_PROMPT  32

#define CLIROOT_DEPTH                       0

#define CLI_SHOW_MACADDRTABLE_IGMP_CMD(x) (x == L7_AF_INET) ? pStrInfo_base_ShowMacAddrTblIgmpsnooping \
  : pStrInfo_base_ShowMacAddrTblMldsnooping

#ifndef L7_CHASSIS
#define  CLISTACKPORT_CMD          "show stack-port"
#define  CLI_STACKPORT_COUNTERS_CMD    "show stack-port counters"
#else
#define  CLISTACKPORT_CMD          "show backplane"
#define  CLI_STACKPORT_COUNTERS_CMD     "show backplane counters"
#endif

#ifdef LVL7_UNDO_PSTRING_CHANGES
#define CLISHOWARP_HELP                     "Display Address Resolution Protocol cache."
#endif

#define CLISHOWIGMPSNOOPING_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_DispIgmpSnoopingInfo \
  : pStrInfo_base_DispMldSnoopingInfo

#define CLISHOWIGMPSNOOPING_VLANID_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_DispIgmpSnoopingValidVlanIdInfo \
  : pStrInfo_base_DispMldSnoopingValidVlanIdInfo

#define CLISHOWIGMPSNOOPING_FASTLEAVE_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_DispIgmpSnoopingFastLeaveInfo \
  : pStrInfo_base_DispMldSnoopingFastLeaveInfo

#define CLISHOWIGMPSNOOPING_MROUTER_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_DispIgmpSnoopingMcastRtrInfo \
  : pStrInfo_base_DispMldSnoopingMcastRtrInfo

/* Denial of Service CLI Commands */
void buildTreeGlobalDenialOfService(EwsCliCommandP depth1);
void buildTreeShowDenialOfService(EwsCliCommandP depth2);

const L7_char8 *commandConfigDoSAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSSIPDIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSSMACDMAC(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSFirstFrag(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPFrag(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPOffset(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPFlag(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPFlagSeq(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSUDPPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPSyn(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPSynFin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSTCPFinUrgPsh(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSL4Port(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSICMP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSICMPv6(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigDoSICMPFrag(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowDenialOfService(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/* End - Denial of Service CLI COmmands */

/* some common help strings */
#define CLISYNTAX_HELPER_ADDRESS   "\r\nIncorrect input! Use 'ip helper-address <address> [<udp-port>]'."
#define CLISYNTAX_NO_HELPER_ADDRESS   "\r\nIncorrect input! Use 'no ip helper-address [<address>] [<udp-port>]'."
#define CLIERR_HELPER_ADDRESS_UDP_PORT_RANGE   "\r\nUDP Relay Port must be in range %d-%d."
#define CLISYNTAX_SHOW_IP_HELPER_STATS "\r\nUse 'show ip helper statistics'."
#define CLISYNTAX_INVALIDROUTERINTF         "\r\nERROR: Routing is not enabled on this interface."

#ifdef LVL7_UNDO_PSTRING_CHANGES
#define CLICONFIGPHYINTERFACE_HELP          "Enter interface."
#endif

#define CLICONFIGIGMPSNOOPINGGROUPMEMBERSHIP_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpGrpMbrshipIntvlSecs \
  : pStrInfo_base_CfgureMldGrpMbrshipIntvlSecs
#define CLISETIGMPSNOOPINGGROUPMEMBERSHIPALL_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpGrpMbrshipIntvlSecsForAllIntfs \
  : pStrInfo_base_CfgureMldGrpMbrshipIntvlSecsForAllIntfs
#define CLICONFIGIGMPSNOOPINGMAXRSP_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpMaxRespTimeSecs \
  : pStrInfo_base_CfgureMldMaxRespTimeSecs
#define CLICONFIGIGMPSNOOPINGMCRTRPRESENT_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpMcastRtrPresentExpirationSecs \
  : pStrInfo_base_CfgureMldMcastRtrPresentExpirationSecs

#define CLICONFIGIGMPSNOOPINGMAXRESPONSEPARM_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_Enter1To25Seconds \
  : pStrInfo_base_Enter1To65Seconds

#define CLICLEARIGMPSNOOPING_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_ClrAllIgmpSnoopingEntries \
  : pStrInfo_base_ClrAllMldSnoopingEntries

#ifndef L7_CHASSIS
#define CLISYNTAX_SHOWSTACKPORT_COUNTERS    "\r\nIncorrect input! Use 'show stack-port counters'."
#define CLISYNTAX_SHOWSTACKPORT             "\r\nIncorrect input! Use 'show stack-port'."
#define CLISYNTAX_SHOWSTACKPORTDIAG          "\r\nIncorrect input! Use 'show stack-port diag'."
#else
#define CLISYNTAX_SHOWSTACKPORT_COUNTERS    "\r\nIncorrect input! Use 'show backplane counters'."
#define CLISYNTAX_SHOWSTACKPORT             "\r\nIncorrect input! Use 'show backplane'."
#define CLISYNTAX_SHOWSTACKPORTDIAG          "\r\nIncorrect input! Use 'show backplane diag'."
#endif

#define CLISYNTAX_SHOWIGMPSNOOPING(x)        (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseShowIgmpsnooping, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseShowMldsnooping, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SHOWIGMPSNOOPING_MROUTERINTERFACE(x)   (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseShowIgmpsnoopingMrouterIntfVlan, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseShowMldsnoopingMrouterIntfVlan, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SHOWMFDBIGMP(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseShowMacAddrTblIgmpsnooping, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseShowMacAddrTblMldsnooping, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMrouterIntf, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  :  pStringAddCR(pStrInfo_base_UseSetMldMrouterIntf, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_SETIGMPSNOOPINGMROUTERINTERFACE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpMrouterIntf, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMrouterIntf, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmp, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMld, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGADMINMODE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmp, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMld, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGFASTLEAVEADMINMODE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpFastLeave, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldFastLeave, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGFASTLEAVEADMINMODE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpFastLeave, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldFastLeave, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpGrpMbrshipIntvlRange2to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldGrpMbrshipIntvlRange2to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpGrpMbrshipIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldGrpMbrshipIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL_ALL(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpGrpMbrshipIntvlRange2to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldGrpMbrshipIntvlRange2to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGGROUPMEMBERSHIPINTERVAL_ALL_NO(x) (x == L7_AF_INET) ?  pStringAddCR(pStrInfo_base_UseNoSetIgmpGrpMbrshipIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldGrpMbrshipIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMaxresponseRange1to3599, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldMaxresponseRange1to3599, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMcrtrexpiretimeRange0to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldMcrtrexpiretimeRange0to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE_ALL(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMaxresponseRange1to3599, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldMaxresponseRange1to3599, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE_ALL_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpMaxresponse, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMaxresponse, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME_ALL(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMcrtrexpiretimeRange0to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldMcrtrexpiretimeRange0to3600, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME_ALL_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpMcrtrexpiretime, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMcrtrexpiretime, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGINTERFACEMODE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpIntfmode, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldIntfmode, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGINTERFACEMODE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpIntfmode, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldIntfmode, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMAXRESPONSE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpMaxresponse, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMaxresponse, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGIGMPSNOOPINGMCRTREXPIRETIME_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpMcrtrexpiretime, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMcrtrexpiretime, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_CLEARIGMPSNOOPING_COMMAND(x) (x==L7_AF_INET) ? pStrInfo_base_ClrIgmpsnooping : pStrInfo_base_ClrMldsnooping

#define CLISYNTAX_CONFIGMACFILTERDELDEST       "\r\nIncorrect input! Use 'no macfilter adddest <macaddr> <vlanid>'."
#define CLISYNTAX_CONFIGMACFILTERDELDESTALL       "\r\nIncorrect input! Use 'no macfilter adddest all <macaddr> <vlanid>'."

#define CLISYNTAX_CLEARIGMPSNOOPING(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseClrIgmpsnooping, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseClrMldsnooping, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
#define CLISYNTAX_CONFIGLOGGINGHOST   "\r\nIncorrect input! Use 'logging host <ipaddress/domain-name> <addresstype> [<portid> [<severitylevel> [<component>]]]'."
#else
#define CLISYNTAX_CONFIGLOGGINGHOST   "\r\nIncorrect input! Use 'logging host <ipaddress|hostname> [<portid> [<severitylevel>]]'."
#endif

#ifdef L7_INCLUDE_COMPONENT_FACILITY_ADDRTYPE
  #define CLISYNTAX_CONFIGLOGGINGSYSLOG   "\r\nIncorrect input! Use 'logging syslog [facility <facility>]'."
#else
  #define CLISYNTAX_CONFIGLOGGINGSYSLOG   "\r\nIncorrect input! Use 'logging syslog'."
#endif

/********************NEW_HELPSTRINGS_FOR SWITCH_SYSTEM_UTILITIES_START************************/
#define L7_USMDB_COPY_UPLOAD 1
#define L7_USMDB_COPY_DOWNLOAD 2

#ifndef L7_CHASSIS
#define CLIDIM_UNIT_HELP        "Unit to be used for this operation. If absent, command executes on all nodes. "
#else
#define CLIDIM_UNIT_HELP        "Slot to be used for this operation. If absent, command executes on all slots. "
#endif

#ifndef L7_CHASSIS
#define CLIUPDATEBOOTCODE_HELP      "Updates the bootloader on the node from the activated image. "
#endif

#ifdef L7_STACKING_PACKAGE
#ifndef L7_CHASSIS
#define CLISYNTAX_BOOT      "Use 'boot system [unit] <image1 | image2>' "
#define CLISYNTAX_SHOWBOOT  "\r\nIncorrect input! Use 'show bootvar [unit]' "
#define CLISYNTAX_DELETE    "\r\nIncorrect input! Use 'delete [unit] <image1 | image2>' "
#define CLISYNTAX_UPDATEBOOTCODE    "\r\nIncorrect input! Use 'update bootcode [unit]' "
#endif  /*L7_CHASSIS*/
#else
#define CLISYNTAX_BOOT      "Use 'boot system <image1 | image2>' "
#define CLISYNTAX_SHOWBOOT  "\r\nIncorrect input! Use 'show bootvar' "
#define CLISYNTAX_DELETE    "\r\nIncorrect input! Use 'delete <image1 | image2>' "
#define CLISYNTAX_UPDATEBOOTCODE    "\r\nIncorrect input! Use 'update bootcode' "
#endif

/********************NEW_HELPSTRINGS_FOR SWITCH_SYSTEM_UTILITIES_END**************************/
/********************NEW_TREE_FUNCTIONS_FOR SWITCH_SYSTEM_UTILITIES_START*********************/

void buildTreeUserPrivilegedCounterClear(EwsCliCommandP depth1);
void cliTreeVlanClear(EwsCliCommandP depth1);
void buildTreeClearServport(depth2);
void buildTreeClearNetwork(depth2);
void cliTreePrivilegedConfigPingReload(EwsCliCommandP depth1);
void cliTreePrivilegedConfigLogout(EwsCliCommandP depth1);
void cliTreePrivilegedConfigClear(EwsCliCommandP depth1);
void cliTreePrivilegedConfigCopy(EwsCliCommandP depth1);
void cliTreePrivilegeCableStatus(EwsCliCommandP depth1);
L7_RC_t cliRunningConfigRtrVlanInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 nextvid);
void cliTreePrivilegedConfigConfigScript(EwsCliCommandP depth1);
void buildTreePrivTraceRoute(EwsCliCommandP depth1);
void buildTreeShowBackPlane(EwsCliCommandP depth1);

void buildTreePrivTraceRouteSize(EwsCliCommandP depth3);
void buildTreePrivTraceRoutePort(EwsCliCommandP depth3);
void buildTreePrivTraceRouteCount(EwsCliCommandP depth3);
void buildTreePrivTraceRouteInitTtl(EwsCliCommandP depth3);
void buildTreePrivTraceRouteMaxTtl(EwsCliCommandP depth3);
void buildTreePrivTraceRouteMaxFail(EwsCliCommandP depth3);
void buildTreePrivTraceRouteInterval(EwsCliCommandP depth3);
void buildTreePrivSwInfoShowVersion(EwsCliCommandP depth1);
/* ============= commands for routing(binds) ============== */
void buildTreeShowArp(EwsCliCommandP depth1);
/* ============= commands for image/file manipulation ============== */

void cliTreePrivilegedImage(EwsCliCommandP depth1);

void cliTreePrivilegedFile(EwsCliCommandP depth1);

/* ============= commands for support tree ============== */
void cliTreePrivilegedSupport(EwsCliCommandP depth1);

/*******Running_Config_Functions_Start**************************/
L7_RC_t cliRunningConfigVlanInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigDVlanInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface);
L7_RC_t cliRunningStormControlInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningSpanningTreeInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigUsersSnmpv3Info(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigAuthenticationInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigUsersLoginInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigDot1xInfo(EwsContext ewsContext, L7_uint32 unit);
L7_RC_t cliRunningConfigdot1adInfo(EwsContext ewsContext, L7_uint32 unit,L7_uint32 interface);
L7_RC_t cliRunningConfigdot3ahInfo(EwsContext ewsContext, L7_uint32 unit,L7_uint32 interface);
#ifdef L7_DOT1AG_PACKAGE
L7_RC_t cliRunningConfigDot1agGlobalInfo(EwsContext ewsContext);
L7_RC_t cliRunningConfigDot1agInterfaceInfo(EwsContext ewsContext, L7_uint32 unit,L7_uint32 interface);
#endif
/*******Running_Config_Functions_End**************************/

/********************NEW_TREE_FUNCTIONS_FOR SWITCH_SYSTEM_UTILITIES_END***********************/

/********************NEW_HELPSTRINGS_FOR SWITCH_DEVICES_START*********************************/

#define CLISHOWMAC_ADDRESS_TABLEIGMPSNOOPING_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_ShowMfdbIgmp \
  : pStrInfo_base_DispMldSnoopingEntriesInMfdbTbl

#define CLISETIGMP_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_EnblDsblIgmpSnoopingOnASelectedIntf \
  : pStrInfo_base_EnblDsblMldSnoopingOnASelectedIntf
#define CLISETIGMPGLOBAL_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_EnblDsblIgmpSnoopingParams \
  : pStrInfo_base_EnblDsblMldSnoopingParams

#define CLIINTERFACEMODE_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgIgmpSnoopingAdmin \
  : pStrInfo_base_EnblDsblMldSnooping
#define CLISETIGMPINTERFACEMODEALL_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_EnblDsblIgmpSnoopingForAllIntfs \
  : pStrInfo_base_EnblDsblIgmpSnoopingForAllIntfs
#define CLISETIGMPMAXRESPONSE_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpMaxRespTimeSecs \
  : pStrInfo_base_CfgureMldMaxRespTimeSecs
#define CLISETIGMPMAXRESPONSEALL_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpMaxRespTimeSecsForAllIntfs \
  : pStrInfo_base_CfgureMldMaxRespTimeSecsForAllIntfs

/********************NEW_HELPSTRINGS_FOR IGMP SNOOPING PER VLAN - START *********************/
#define CLISETIGMPVLAN_HELP(x) (x == L7_AF_INET) ? pStrInfo_base_CfgureIgmpSnoopingParamsForVlan \
  : pStrInfo_base_CfgureMldSnoopingParamsForVlan

extern L7_uint32 vrid_context;

/********************New Function Declarations for Switch Device - Start ****************/

void buildTreePrivSwDevStormControl(EwsCliCommandP depth2);       /* depth2 = "show" */
void buildTreePrivSwDevPort(EwsCliCommandP depth2);             /* depth2 = "show" */
void buildTreePrivSwDevMacAddrTable(EwsCliCommandP depth2);     /* depth2 = "show" */
void buildTreePrivSwDevSetGmrpGvrpAdmin(EwsCliCommandP depth1);   /* depth1 = "Privileged EXEC mode" */
void buildTreePrivSwDevShowIgmpInterface(EwsCliCommandP depth2, L7_uchar8 family);  /* depth2 = "show" */
void buildTreePrivSwDevShowMonitor(EwsCliCommandP depth2);    /* depth2 = "show" */
void buildTreeUserSwDevShowGarpConfiguration(EwsCliCommandP depth2);    /* depth2 = "show" */
void buildTreeUserSwDevShowVlan(EwsCliCommandP depth2);     /* depth2 = "show" */
void buildTreeGlobalDeletePortAll(EwsCliCommandP depth1);   /* depth1 = "Global Config" */
void buildTreeGlobalSwDevMacFilter(EwsCliCommandP depth1);      /* depth2 = "Global Config" */
void buildTreeGlobalSwDevInterfaceLinkTrap(EwsCliCommandP depth1);  /* depth1 = "Interface Config" */
void buildTreeGlobalSwDevInterfaceName(EwsCliCommandP depth1);  /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevSetGarp(EwsCliCommandP depth1);     /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevMacFilter(EwsCliCommandP depth1);     /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevSpeed(EwsCliCommandP depth1);   /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevVlan(EwsCliCommandP depth1);   /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevDuplex(EwsCliCommandP depth1);    /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevPortVlanGroup(EwsCliCommandP depth2);   /* depth1 = "Interface Config" */
void buildTreeInterfaceSwDevPort(EwsCliCommandP depth2);   /* depth1 = "Interface Config" */
void buildTreeInterfaceStormControl(EwsCliCommandP depth1);
void buildTreeGlobalSwDevGarpTimer(EwsCliCommandP depth1);    /* depth1 = "Global Config" */
void buildTreeGlobalSwDevVlan(EwsCliCommandP depth1);       /* depth1 = "Global Config" */
void buildTreeGlobalSwWireless(EwsCliCommandP depth1);  /* depth1 = "Global Config" */
void buildTreeGlobalSwDevSpeed(EwsCliCommandP depth1);  /* depth1 = "Global Config" */
void buildTreeGlobalSwDevStormControl(EwsCliCommandP depth1);  /* depth1 = "Global Config" */
void buildTreeGlobalSwDevLacpModeAllVlanGroupAll(EwsCliCommandP depth1);/* depth1 = "Global Config" */
void buildTreeGlobalSwDevPort(EwsCliCommandP depth1);/* depth1 = "Global Config" */
void buildTreeGlobalSwDevDuplexPortChannel(EwsCliCommandP depth1);      /* depth1 = "Global Config" */
void buildTreeGlobalSwDevCreateMirroring(EwsCliCommandP depth1);        /* depth1 = "Global Config" */
void buildTreeVlanDBSwDevVlan(EwsCliCommandP depth1);       /* depth1 = "Vlan Database" */

void buildTreeGlobalSwDevDuplexAll(EwsCliCommandP depth1);
void buildTreeUserSwDevShowGarp(EwsCliCommandP depth2);
void buildTreeInterfaceMaxFrameSize(EwsCliCommandP depth1); /* rhelbaoui */
void buildTreeGlobalMaxFrameSize(EwsCliCommandP depth1);
void buildTreeInterfaceConfigIpMtu(EwsCliCommandP depth2);
void buildTreeInterfaceIpRedirects(EwsCliCommandP depth2);
void buildTreeInterfaceIpUnreachables(EwsCliCommandP depth2);

void buildTreeGlobalLoadBalance(EwsCliCommandP depth2);
/********************New Function Declarations for Switch Device - End ****************/

/*************New Function Declarations for Switch User Acct Mgmt - Start **************/
void buildTreeGlobalSwUamUsers(EwsCliCommandP depth1);
void buildTreeUserExSwUamShowLogin(EwsCliCommandP depth2);
void buildTreePrivSwUamShowUsers(EwsCliCommandP depth2);
void buildTreePrivSwUamDisconnect(EwsCliCommandP depth1);
/*************New Function Declarations for Switch User Acct Mgmt - End ****************/

/*************New Function Declarations for Password Mgmt - Start **************/
void buildTreeGlobalConfigPasswordMgmt(EwsCliCommandP depth1);
void buildTreePrivShowPasswordsConfiguration(EwsCliCommandP depth1);
/*************New Function Declarations for Password Mgmt - End ****************/

/*************New Function Declarations for Switch System Info - Start ****************/
void buildTreePrivSwInfoEventLog(EwsCliCommandP depth2);
void buildTreePrivSwInfoHardware(EwsCliCommandP depth2);
void buildTreePrivSwInfoLogging(EwsCliCommandP depth2);
void buildTreePrivSwInfoMacAddrTable(EwsCliCommandP depth2);
void buildTreePrivSwInfoSysInfo(EwsCliCommandP depth2);
void buildTreeUserArp(EwsCliCommandP depth2);
void buildTreePrivSwInfoShowInterfaceEthernet(EwsCliCommandP depth3);
void buildTreePrivSwInfoShowInterfaceSwitchport(EwsCliCommandP depth3);
void buildTreePrivSwInfoSntp(EwsCliCommandP depth2);
void buildTreePrivWriteMemory(EwsCliCommandP depth1);
/*************New Function Declarations for Switch Systen Info - End ****************/

/***********************Authentication*********************************/
void buildTreeGlobalHTTPConfig(EwsCliCommandP depth1);
void buildTreeGlobalAuthenticationConfig(EwsCliCommandP depth1);
void buildTreeGlobalAuthenticationUsersConfig(EwsCliCommandP depth2);
void buildTreePrivShowAuthentication(EwsCliCommandP depth2);
void buildTreePrivShowAuthenticationUsers(EwsCliCommandP depth3);
/**********************************************************************/

/****************New Commands for TACACS+ - Start *********************************/
void buildTreeTacacsConfigMode();
/****************New Commands for TACACS+ - End   *********************************/

/********************New Function Declarations for Multicast start ****************/

void cliTreeUserShowDvmrpIgmp(EwsCliCommandP depth2);
void cliTreeUserShowMulticast(EwsCliCommandP depth2);
void cliTreeUserShowPimdmPimsm(EwsCliCommandP depth2);

/********************New Function Declarations for Multicast - End ****************/

/* function called by EMweb code that calls functions to set up trees*/
void cliCommandsInit();
void cliDevshellInit();
void cliDevshellAssign();
void cliAddUserTree();

/* to setup cli maintanence tree */
void buildTreeMaintenanceMode();
void initMaintanenceMode();
void cliSetMaintanenceTree(EwsCliCommandP objMode);

/* functions to set up cli tree */
void buildTreeShow();
void buildTreeConfig();
void buildTreePing(EwsCliCommandP depth1);
void buildTreePingCount(EwsCliCommandP depth6);
void buildTreePingInterval(EwsCliCommandP depth6);
void buildTreePingSize(EwsCliCommandP depth6);
void buildTreeClear();
void buildTreeTransfer();
void buildTreeOther();
void buildTreeSave();
void buildTreeTest();
void buildTreeReset();

void buildTreeUserShow(EwsCliCommandP depth1);
void buildTreeUserPriviledge();
void buildTreeVlanDatabase();
void buildTreeLoopbackInterface();
void buildTreeTunnelInterface();
void buildTreeGlobalConfig();
#ifndef L7_WIRELESS_PACKAGE
void buildTreeNonWirelessConfigSupport();
#endif
void buildTreeSupport();
void buildTreeInterfaceConfig();
void buildTreeLineConsoleConfig();
void buildTreeLineSshConfig();
void buildTreeLineTelnetConfig();
void buildTreeACL();
void buildTreeACLRule();
void buildTreeIpv6AccessListConfig();
void cliTreeIpAccessListConfig();
void buildTreeAccessListIpv6(EwsCliCommandP depth2);
void buildTreeMacAccessListConfig();
void buildTreePolicyMap();
void buildTreePolicyClassMap();
void buildTreeClassMap();
void buildTreeRouterConfigOspf();
void buildTreeRouterConfigOspfv3();
void buildTreeRouterConfigBGP();
void buildTreeRouterConfigRIP();
void buildTreeHiddenCommandMode();
EwsCliCommandP cliGetHiddenNode(void);
EwsCliCommandP cliGetMaintenanceNode(void);
void addBgpTree(EwsCliCommandP depth1);

void buildTreeSntp();
/* chassis commands Tree declaration */

void buildTreeConfigGlobalAlarmModes(EwsCliCommandP depth1);

void buildTreePrivilegedClearAlarms(EwsCliCommandP depth1);

void buildTreeShowAlarms(EwsCliCommandP depth1);

void buildTreeGlobalConfigAlarms(EwsCliCommandP depth1);

void  buildTreeShowPowerSupply(EwsCliCommandP depth2);
void buildTreePrivilegeExecChassisReload(EwsCliCommandP depth1);
#ifdef L7_IPV6_PACKAGE
void buildTreeDhcp6sPool();
#endif

/*****************************************************************/
/* SWMGMT */
void buildTreeGlobSWMgmtBridgeTime(EwsCliCommandP depth1);
void buildTreeGlobSWMgmtSnmpserver(EwsCliCommandP depth2);
void buildTreeGlobSWMgmtSnmpTrap(EwsCliCommandP depth1);

void buildTreePrivSWMgmtNetworkConf(EwsCliCommandP depth1);
void buildTreePrivSWMgmtServicePortConfig(EwsCliCommandP depth1);
void buildTreePrivSWMgmtSetPrompt(EwsCliCommandP depth2);
void buildTreePrivSWMgmtTelnetConf(EwsCliCommandP depth1);

void buildTreePrivSWMgmtRemoteConConf(EwsCliCommandP depth1);
void buildTreePrivSWMgmtTelnetConConf(EwsCliCommandP depth1);

void buildTreePrivSWMgmtShowForwardingDB(EwsCliCommandP depth2);
void buildTreePrivSWMgmtShowSnmpData(EwsCliCommandP depth2);
void buildTreePrivSWMgmtShowTrapFlags(EwsCliCommandP depth2);
void buildTreePrivSwInfoShowTechSupport(EwsCliCommandP depth2);

void buildTreeLineConfigSWMgmtSetEIAParm(EwsCliCommandP depth1);
void buildTreeLineConfigSWMgmtOutboundTelnetConf(EwsCliCommandP depth2);
void buildTreeLineConfigSWMgmtTelnetConf(EwsCliCommandP depth1);

void buildTreeUserExecSWMgmtOutboundTelnet(EwsCliCommandP depth1);
void buildTreeUserExecSWMgmtShowNetwork(EwsCliCommandP depth2);
void buildTreeUserExecSWMgmtShowSerial(EwsCliCommandP depth2);
void buildTreeUserExecSWMgmtShowServicePort(EwsCliCommandP depth2);
void buildTreeUserExecSWMgmtShowOutboundTelnet(EwsCliCommandP depth2);
void buildTreeUserExecSWMgmtShowPortChannel(EwsCliCommandP depth2);
void buildTreeUserExecSWMgmtShowTelnetCon(EwsCliCommandP depth2);

/*****************************************************************/

void buildTreeInterfaceVlanPortDot1pPriorityAll(EwsCliCommandP depth1);
void buildTreeInterfaceVlanPortDot1pPriority(EwsCliCommandP depth1);
void buildTreePrivHostname(EwsCliCommandP depth1);
void buildTreePrivShowIpHttp(EwsCliCommandP depth1);
void buildTreePrivIpHttp(EwsCliCommandP depth3);
void buildTreePrivIpTelnet(EwsCliCommandP depth2);

/*****************************************************************
 *Start : DNS CLIENT Component Command Tree declarations
 ******************************************************************/
void buildTreeIpDnsClient(EwsCliCommandP depth2);
void buildTreeIpNameServer(EwsCliCommandP depth2);
void buildTreeIpHostNameToAddressMapping(EwsCliCommandP depth2);
void buildTreeClearHost(EwsCliCommandP depth2);
void buildTreeShowHosts(EwsCliCommandP depth2);
void buildTreeIpHostnameToAddressMapping(EwsCliCommandP depth2);

 /**********END: DNS CLIENT Component command Trees */

/**********************************************************************************************/

/**********************************************************************************************************/

/*error */
const L7_char8 *commandError(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* sub command movement functions */
const L7_char8 *cliRoot(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliShowProtocol(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowArpSwitch(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliConfigSysName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* show */
const L7_char8 *commandShowInventory(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSysinfo(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowStatsPortDetailed(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowStatsPortSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowStatsSwitchDetailed(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowStatsSwitchSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowArpPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowNetwork(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowServicePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSerial(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSnmptrap(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSwitchconfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortDescription(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowFDBAgetime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowFDBLearned(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowFDBTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowVlanDetailed(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowVlanSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortChannel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortChannelSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowPortChannelSysPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowTelnetCon(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowOutboundTelnet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowTrapflags(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacFilterSourcePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacFilterNoSourcePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacFilterNoDestPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMonitor(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowTraplog(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowGarp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowMfdbTable(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowMfdbStaticFiltering(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowMfdbGmrp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowMfdbIgmpSnooping(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowMfdbStats(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowRunningConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliShowRunningConfig(EwsContext ewsContext, const L7_char8 * * argv, L7_uint32 numArg, L7_BOOL chkFile);
L7_RC_t cliShowRunningConfigExecute(EwsContext ewsContext);
const L7_char8 *commandShowVersion (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

void cliSetTree( EwsCliCommandP newTree);
L7_char8 *cliSetRootMode( );
L7_char8 *cliSetPriviledgeUserMode();

/* clear */
const L7_char8 *commandClearConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearPass(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearStatsPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearStatsSwitch(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearTransfer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearTraplog(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearIgmpsnooping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearPortChannel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandClearVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* clear counters for switch or port */
const L7_char8 *commandClearCounters(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* reset */
const L7_char8 *commandResetSystem(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* transfer */
const L7_char8 *commandTransferUploadMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferUploadServerIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferUploadPath(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferUploadFilename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferUploadDatatype(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferUploadStart(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandTransferDownloadMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferDownloadServerIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferDownloadPath(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferDownloadFilename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferDownloadDatatype(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTransferDownloadStart(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandCopy(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandBoot(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDelete(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandRename(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandDir(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandUpdate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowBoot(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandFileDescr(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandEraseStartupConfig(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv,L7_uint32 index);

/* save */
const L7_char8 *commandCopySystemRunningConfigNvramStartupConfig(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* reload*/
const L7_char8 *commandReload(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* show ip http */
const char *commandShowIpHttp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index);

/* ip http java */
const L7_char8 *commandIpHttpJava(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* ip http server */
const L7_char8 *commandIpHttpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* ip http session maxsessions */
const L7_char8 *commandIpHttpMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* ip http session soft-timeout */
const L7_char8 *commandIpHttpSoftTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* ip http session hard-timeout */
const L7_char8 *commandIpHttpHardTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* ping*/
const L7_char8 *commandPing(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* traceroute */
const L7_char8 *commandTraceRoute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* test*/
const L7_char8 *commandCablestatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* DevShell */
const L7_char8 *commandDevShell(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* hidden techsupport  */
const L7_char8 *commandTechSupport(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* BCM interpreter */
const L7_char8  *commandDrvShell(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

/* Logout */
const L7_char8 *commandLogout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* Quit */
const L7_char8 *commandQuit(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/*no clibanner*/
const L7_char8 *commandNoCliBanner(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/*extern EwsCliCommandP mainMenu[FD_CLI_WEB_DEFAULT_MAX_CONNECTIONS];*/
extern EwsCliCommandP mainMenu[FD_CLI_DEFAULT_MAX_CONNECTIONS];

const L7_char8 *cliUserPriviledgeMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *cliConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliSupportMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliInterfaceMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliLineConfigConsoleMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliLineConfigSshMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *cliLineConfigTelnetMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *cliAclMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *cliAclRuleMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliPolicyMapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliPolicyClassMapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliClassMapMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliOSPRouterConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliIpv6OspfConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliRIPRouterConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliRouterConfigBgp4Mode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *cliDhcpsPoolConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

#ifdef L7_IPV6_PACKAGE
const L7_char8 *cliDhcp6sPoolConfigMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
#endif

#ifndef L7_WIRELESS_PACKAGE
const L7_char8 *cliWirelessModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliWirelessAPModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                  const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliWirelessNetworkModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliAPProfileModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                 const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliAPProfileRadioModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *cliAPProfileVAPModeSupport(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index);
#endif


/*****************************************************************/
/* SWMGMT */
const L7_char8 *commandBridgeAgingTime (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetworkMACAddress (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetworkMACType (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerCommunityName (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerCommunityRo (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerCommunityRw (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerCommunityIpAddress (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerCommunityIPMask (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpserverCommunityMode (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerEnableTraps (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandEnableTrapsLinkMode (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerEnableTrapsMultiusers (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpServerEnableTrapsSTPMode (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpTrap (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpTrapIPAddr (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpTrapMode (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpTrapVersion(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index );

const L7_char8 *commandNetworkJavaMode (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetworkParams (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetworkProtocol (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandNetworkMgmtVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetServiceportProtocol (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetServiceportIp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetPrompt (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTelnetCon (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpTelnetServerEnable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandRemoteConMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandRemoteConTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowForwardingDbAgetime (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSnmp (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSnmptrap (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowTrapflags (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandSshConMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTelnetConTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTelnetConMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSshConTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandSetSerialBaudrate (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSerialTimeout (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowNetwork (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSerial (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowServicePort (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandRemoteTelnet (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandOutboundTelnet (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTelnetMaxSessions(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandTelnetTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/*****************************************************************/

/***************************NEWCOMMANDPROTOTYPEDECLARATION_CONFIG_SWITCHINGDEVICES************/

const L7_char8 *commandSetGMRPAdminMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGVRPAdminMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandAutoNegotiate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceAddPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceAdminMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortChannelAdminMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortChannelStaticLAG(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortChannelLoadBalanceLAG(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceDeletePort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandGlobalDeletePortAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortChannelLoadBalanceGlobalLAG(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfaceLinktrap(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortChannelName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfacePortChannel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandInterfacePortChannelStaticCapability(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMaxFrameSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index); /* rhelbaoui */
const L7_char8 *commandGlobalMaxFrameSize(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacFilterAddDest(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacFilterAddSrc(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortLacpMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortLacpTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortLacpTimeoutAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortLacpModeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortChannelSystemPriority(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGarpTimerJoin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGarpTimerLeave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGarpTimerLeaveAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGMRPInterfaceMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGVRPInterfaceMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShutdown(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpTrapLinkStatus(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpeed(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandAutoNegotiateAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacFilter(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacFilterAddDestAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandMacFilterAddSrcAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortMirroring(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandPortLacpModeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);


const L7_char8 *commandSetGarpTimerJoinAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGarpTimerLeaveAllTimeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGarpTimerLeaveAllTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGMRPInterfaceModeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetGVRPInterfaceModeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPGlobal(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceModeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceGroupMembershipInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceMaxResponse(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceMcRtrExpireTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceGroupMembershipIntervalAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceMaxResponseAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceMcRtrExpireTimeAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPInterfaceFastLeave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPMrouterInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlFlowControl(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShutdownAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSnmpTrapLinkStatusAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSpeedAll(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlBcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlMcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlUcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlIntfBcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlIntfMcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandStormControlIntfUcast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpClientMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpClientPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpClientUnicastPollInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpClientUnicastPollTimeout(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpClientUnicastPollRetry(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpClientBroadcastPollInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSntpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/***************************NEWCOMMANDPROTOTYPEDECLARATION_SHOW_SWITCHINGDEVICES************/

const L7_char8 *commandShowGmrpConfiguration(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowGvrpConfiguration(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIGMPSnooping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowIGMPSnoopingMrouterInterface(EwsContext ewsContext,uintf argc, const char * * argv, uintf index);
const L7_char8 *commandShowMacAddressTableGmrp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddressTableIgmpSnooping(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddressTableMulticast(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddressTableStatic(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddressTableStaticFiltering(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddressTableStats(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddrTableCount(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowStormControl(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowDVlanTunnelInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowDVlanTunnel(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/****************New Commands for Switching User Acct Mgmt - Start***********************/
const L7_char8 *commandUsersSnmpv3AccessMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersSnmpv3Authentication(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersSnmpv3Encryption(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandUsersSnmpv3EcryptdKeyEncryption(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);


/****************New Commands for Switching User Acct Mgmt - End ***********************/

/****************New Commands for Switching System Info - Start ***********************/
const L7_char8 *commandSnmpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowEventLog(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowHardware(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddrTable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowInterfaceSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowInterfacesEthernet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSntp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSntpClient(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSntpServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandShowLogging(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowLoggingPersistent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowLoggingBuffered(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowLoggingHosts(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowLoggingTraplogs(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandLoggingBuffered(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingBufferedWrap(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandLoggingCliCommand(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

const L7_char8 *commandLoggingBufferedComponent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingPersistent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingSyslogPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingSyslog(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingSyslogFacility(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingConsole(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingConsoleComponent(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingHost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingHostRemove(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandLoggingHostReconfigure(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

L7_RC_t cliLogConvertSeverityLevelToVal(L7_char8 * severityLevelString, L7_uint32 * severityLevel);
L7_RC_t cliLogConvertSeverityLevelToString(L7_uint32 severityLevel, L7_char8 * severityLevelString, L7_uint32 bufSiz);

L7_RC_t cliLogConvertFacilityToVal(L7_char8 * facilityString, L7_LOG_FACILITY_t * facility);
L7_RC_t cliLogConvertFacilityToString(L7_uint32 facility, L7_char8 * facilityString, L7_uint32 bufSiz);
/****************New Commands for Switching System Info - End  ***********************/

/****************New Commands for Configuration Scripting - Start ***********************/
const L7_char8 *commandConfigScriptApply(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigScriptDelete(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigScriptList(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigScriptShow(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandConfigScriptValidate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/****************New Commands for Configuration Scripting - End ***********************/

const L7_char8 *commandLicenseAdvanced(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowKeyFeatures(EwsContext ewsContext,L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
void buildTreeShowKeyingStatus(EwsCliCommandP depth1);
EwsCliCommandP cliGetMode(cliModeID_t mode);
void cliSetMode(cliModeID_t mode, EwsCliCommandP objMode);
EwsCliCommandP cliGetThisMode(cliModeID_t modeId, cliModes_t * modesp);

/****************New Commands for IGMP Snooping Per Vlan - Start *********************************/
const L7_char8 *commandSetIGMPVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPVlanFastLeave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPVlanGroupMembershipInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPVlanMaxResponseTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetIGMPVlanMulticastRouterExpiryTime(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/****************New Commands for IGMP Snooping Per Vlan - End   *********************************/
const L7_char8 *commandShowMacAddrTableInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowMacAddrTableVlanId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowTechSupport(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandHostname(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandWriteMemory(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/****************New Commands for Support Functionality - Start   *********************************/
const L7_char8 *commandSupportClear(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSupportConsole(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);

/****************New Commands for Configurable Pagination feature - Start *********************************/
const L7_char8 *commandSetTerminalLength(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
void  buildTreeUserExecTerminalLength(EwsCliCommandP depth1);
const L7_char8 *commandShowTerminalLength (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
void buildTreeShowTerminalLength(EwsCliCommandP depth2);
const L7_char8 *commandShowPaginationMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetPaginationMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
L7_RC_t cliRunningConfigPaginationInfo(EwsContext ewsContext, L7_uint32 unit);
/****************New Commands for Configurable Pagination feature - End *********************************/

/****************New Commands for PORT Naming  - Start   *********************************/
const L7_char8 *commandIntfDescription(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/****************New Commands for PORT Naming  - End   *********************************/
/**** Command functions for DNS Client - Start ******************************/
const L7_char8 *commandIpDomainLookup(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDomainName(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpNameServer(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpHostNameAddress(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDomainRetry(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDomainTimeout(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpDomainList(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandClearHostname(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandShowHosts(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandIpv6HostNameAddress(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);

/******Command functions for DNS Client - End ********************************/


/*****************command added for updating LM boot code through CDA*****************/
const L7_char8 *commandUpdateBootcode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
/*******************CPU Utilization**************************************************/
void buildTreePrivCpuInfo(EwsCliCommandP depth2);
const L7_char8 *commandShowProcessCpu(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

/***********************Console Break-in*********************************************/
void buildTreePrivSupportConsoleBreakIn(EwsCliCommandP depth1);
const L7_char8 *commandSupportConsoleBreakinString(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandSupportShowConsoleBreakin(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index);
const L7_char8 *commandSupportConsoleBreakin(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
const L7_char8 *cliCaptivePortalMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *cliCaptivePortalInstanceMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *cliCaptivePortalInstanceLocaleMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *cliCaptivePortalEncodedImageMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
#endif

#ifdef L7_DOT1AG_PACKAGE
const L7_char8 *cliDot1agMaintainanceDomainMode(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
#endif

/**************User Manager*************/
void buildTreeLineConfigSWMgmtLogin(EwsCliCommandP depth2);
void buildTreeLineConfigSWMgmtEnable(EwsCliCommandP depth2);
void buildTreeLinePasswordConfig(EwsCliCommandP depth2);

#ifdef L7_AUTO_INSTALL_PACKAGE 
/****************   Commands for AutoInstall   *********************************/
const L7_char8 * commandShowAutoinstall(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandBootAutoinstall(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8 *commandBootAutoinstallAutoSave(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandBootAutoinstallRetrycount(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
void cliTreePrivilegedAutoinstall(EwsCliCommandP depth2);
#endif 

#ifdef L7_ROUTING_PACKAGE
void cliTreeGlobalIphelper(EwsCliCommandP depth2);
#endif

#endif

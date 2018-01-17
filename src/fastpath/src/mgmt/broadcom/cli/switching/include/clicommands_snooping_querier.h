/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/clicommands_snooping_querier.h
 *
 * @purpose header for snooping querier commands
 *
 * @component cli
 *
 * @comments none
 *
 * @create  14-Dec-2006
 *
 * @author  drajendra
 * @end
 *
 **********************************************************************/

#ifndef CLICOMMANDS_SNOOP_QUERIER_H
#define CLICOMMANDS_SNOOP_QUERIER_H

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"

#define CLISETSNOOP_QUERIER_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_CfgureIgmpSnoopingQuerierParams \
  : pStrInfo_switching_CfgureMldSnoopingQuerierParams

#define CLISETSNOOP_QUERIERMODE_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_CfgureIgmpSnoopingQuerierMode \
  : pStrInfo_switching_CfgureMldSnoopingQuerierMode

#define CLISETSNOOP_QUERIERVLAN_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_CfgureIgmpSnoopingQuerierParamsForVlan \
  : pStrInfo_switching_CfgureMldSnoopingQuerierParamsForVlan

#define CLISETSNOOP_QUERIER_VERSION_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_CfgureIgmpVer : pStrInfo_switching_CfgureMldVer

#define CLISNOOP_QUERIER_VERSION_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_EnterIgmpVer : pStrInfo_switching_EnterMldVer

#define CLISETSNOOP_QUERIERVLANMODE_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_CfgureIgmpSnoopingQuerierModeForVlan \
  : pStrInfo_switching_CfgureMldSnoopingQuerierModeForVlan

#define CLISETSNOOP_ADDR_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_EnterIpv4Addr \
  : pStrInfo_switching_EnterIpv6Addr

#define CLISHOWIGMPSNOOPING_QUERIER_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_DispIgmpSnoopingQuerierInfo \
  : pStrInfo_switching_DispMldSnoopingQuerierInfo
#define CLISHOWIGMPSNOOPING_QUERIER_DETAIL_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_DispIgmpSnoopingQuerierDetailedInfo \
  : pStrInfo_switching_DispMldSnoopingQuerierDetailedInfo

#define CLISHOWIGMPSNOOPING_QUERIER_VLANID_HELP(x) (x == L7_AF_INET) ? pStrInfo_switching_DispIgmpSnoopingQuerierVlanInfo \
  : pStrInfo_switching_DispMldSnoopingQuerierVlanInfo

#define CLISYNTAX_SNOOPQUERIER_SETQUERYTIME(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierQueryIntvlIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierQueryIntvlIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPQUERIER_SETQUERYTIME_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierQueryIntvlIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierQueryIntvlIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPQUERIER_SETQUERIERTIME(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierTimerExpiryIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierTimerExpiryIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPQUERIER_SETQUERIERTIME_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierTimerExpiryIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierTimerExpiryIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPQUERIER_SETQUERIERVERSION(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierVerVer, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierVerVer, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPQUERIER_SETQUERIERVERSION_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierVerVer, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierVerVer, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLIERROR_SNOOPQUERIER_QUERIERVERSIONSET(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UnableToSetIgmpVer, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_switching_UnableToSetMldVer, 1, 0, 0, 0, L7_NULLPTR)

#define CLISYNTAX_CONFIGSNOOPQUERIER_ADDRESS(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierAddrIpv4addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierAddrIpv6addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGSNOOPQUERIER_VLANADDRESS(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierVidAddrIpv4addr_1, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierVidAddrIpv6addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_CONFIGSNOOPQUERIER_VLANADDRESS_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierVidAddrIpv4addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierVidAddrIpv6addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_CONFIGSNOOPQUERIER_ADDRESS_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseNoSetIgmpQuerierAddr, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseNoSetMldQuerierAddr, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_CONFIGSNOOPQUERIER_ADMINMODE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerier, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerier, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGSNOOPQUERIER_VLANADMINMODE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGSNOOPQUERIER_VLANADMINMODE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseNoSetIgmpQuerierVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseNoSetMldQuerierVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGSNOOPQUERIER_VLANELECTMODE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseSetIgmpQuerierElectionParticipateVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseSetMldQuerierElectionParticipateVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGSNOOPQUERIER_VLANELECTMODE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseNoSetIgmpQuerierElectionParticipateVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseNoSetMldQuerierElectionParticipateVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_CONFIGSNOOPQUERIER_ADMINMODE_NO(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_UseNoSetIgmpQuerier, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseNoSetMldQuerier, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLIERROR_SET_SNOOPING_QUERIER_ADMIN_MODE(x) (x == L7_AF_INET) ? pStrInfo_switching_IgmpSnoopingQuerierAdminMode \
  : pStrInfo_switching_MldSnoopingQuerierAdminMode
#define CLIERROR_SET_SNOOPING_QUERIER_VLANADMIN_MODE(x) (x == L7_AF_INET) ? pStrInfo_switching_IgmpSnoopingQuerierVlanMode_1 \
  : pStrInfo_switching_MldSnoopingQuerierVlanMode_1
#define CLIERROR_SET_SNOOPING_QUERIER_VLANELECT_MODE(x) (x == L7_AF_INET) ? pStrInfo_switching_IgmpSnoopingQuerierElectionParticipateVlanMode \
  : pStrInfo_switching_MldSnoopingQuerierElectionParticipateVlanMode

#define CLISYNTAX_CONFIGQUERIERIP(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_switching_Ipv4addrMustBeAValidIpv4Addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_Ipv6addrMustBeAValidIpv6Addr, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SHOW_SNOOPQUERIER_HELP(x) \
  (x) == L7_AF_INET \
  ? pStringAddCR(pStrInfo_switching_UseShowIgmpsnoopingQuerierDetailVlanVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_switching_UseShowMldsnoopingQuerierDetailVlanVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISNOOP_QUERIER_MODE(x) \
  (x) == L7_AF_INET \
  ? pStrInfo_switching_IgmpSnoopingQuerierMode :  pStrInfo_switching_MldSnoopingQuerierMode
#define CLISNOOP_QUERIER_VLAN_MODE(x) \
  (x) == L7_AF_INET \
  ? pStrInfo_switching_IgmpSnoopingQuerierVlanMode :  pStrInfo_switching_MldSnoopingQuerierVlanMode

#define CLISNOOP_QUERIER_VERSION(x) \
  (x) == L7_AF_INET ? pStrInfo_common_IgmpVer : pStrInfo_common_MldVer

#define CLISNOOP_SHOW_SNOOPING_QUERIER_VLAN_DETAIL(x) \
  (x) == L7_AF_INET ? pStrInfo_switching_ShowIgmpSnoopingQuerierDetail : pStrInfo_switching_ShowMldSnoopingQuerierDetail
const L7_char8 *commandSetSnoopingQuerier(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierAddress(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierQueryInterval(EwsContext ewsContext, L7_uint32 argc,
                                                       const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierExpiryInterval(EwsContext ewsContext, L7_uint32 argc,
                                                        const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierVersion(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierVlanMode(EwsContext ewsContext, L7_uint32 argc,
                                                  const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierVlanAddress(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSnoopingQuerierElectionVlanMode(EwsContext ewsContext, L7_uint32 argc,
                                                          const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSnoopingQuerier(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv,
                                           L7_uint32 index);

#endif

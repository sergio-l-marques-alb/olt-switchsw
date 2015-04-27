/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/cli_macro.h
*
* @purpose Macros for cli
*
* @component user interface
*
* @comments none
*
* @create 01/07/2003
*
* @author jagdishc
* @end

**********************************************************************/

#ifndef _CLI_MACRO_
#define _CLI_MACRO_

#define L7_NO_OPTIONAL_PARAMS  0
#define CLI_OPTION68             68

#define CLIERROR_SET_IGMP_SNOOPING_ADMIN_MODE(x) (x == L7_AF_INET) ? pStrInfo_base_IgmpSnoopingAdminMode_1 \
  : pStrInfo_base_MldSnoopingAdminMode_1

#define CLIFAILED_PROTECTEDPORT_ILLEGAL_INTERFACE "\r\nSelected interface cannot be configured as a protected port."
#define CLIERROR_IPSUBNET_VLAN_IP_NETMASK_CONFLICT   "\r\nError: IP Address/Netmask entered conflicts with an \
                                                  already existing entry."

#define CLIIGMP_SNOOPING_ADMINMODE(x) (x == L7_AF_INET) ? pStrInfo_base_IgmpSnoopingAdminMode \
  : pStrInfo_base_MldSnoopingAdminMode

#define CLIIGMP_SNOOPING_ENTRIES_CLEARED(x) (x == L7_AF_INET) ? pStrInfo_base_IgmpSnoopingEntriesClred \
  : pStrInfo_base_MldSnoopingEntriesClred
#define CLIIGMP_SNOOPING_ENTRIES_NOT_CLEARED(x) (x == L7_AF_INET) ? pStrErr_base_IgmpSnoopingEntriesNotClred \
  : pStrErr_base_MldSnoopingEntriesNotClred
#define CLIIGMP_SNOOPING_INTERFACE_CONFIGURATION(x) (x == L7_AF_INET) ? pStrInfo_common_IgmpSnoopingIntfCfg \
  : pStrInfo_common_MldSnoopingIntfCfg

#define CLIINTERFACES_ENABLED_FOR_IGMP_SNOOPING(x)  (x == L7_AF_INET) ? pStrInfo_common_IntfsEnbldForIgmpSnooping \
  : pStrInfo_common_IntfsEnbldForMldSnooping

#define CLIVERIFY_CLEAR_ALL_IGMP_SNOOPING_ENTRIES_YN(x) (x==L7_AF_INET) ? pStringAddCR(pStrInfo_base_ConfirmToClrAllIgmpSnoopingEntriesYN, 1, 0, 0, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_ConfirmToClrAllMldSnoopingEntriesYN, 1, 0, 0, 0, L7_NULLPTR)
#define NSSA_TYPE1     "nssa-type1"
#define NSSA_TYPE2     "nssa-type2"  
#define CLILVL7_HOME    "Broadcom Home"
#define CLILVL7_HOME_PAGE    "Broadcom Home Page"
#define CLILVL7_HOME_PAGE_1    "Broadcom Home Page"
#define CLILVL7_NETWORKING    "Broadcom Networking"
#define CLILVL7_SYSTEMS_FASTPATH_SWITCH_APPLET    "Broadcom FASTPATH Switch Applet"
#define CLILVL7_SYSTEMS_HOME    "Broadcom HOME"
#define CLIROUTING_IPV6_LIFETIME_INTERVAL "Router Advertisement Lifetime"

/* dot1x default values */
#define L7_DOT1S_DEFAULT_MST_PORTPRIORITY            128

/* telnet fd values */
#define FD_CLI_REMOTECON_MIN_SESSIONS                0
#define FD_CLI_REMOTECON_MAX_SESSIONS                5
#define FD_CLI_SSHCON_MIN_SESSIONS                   0
#define FD_CLI_SSHCON_MAX_SESSIONS                   5

/* outbound telnet fd values */
#define FD_CLI_TELNET_MIN_SESSIONS                   0
#define FD_CLI_TELNET_MAX_SESSIONS                   5
#define FD_CLI_TELNET_MIN_TIMEOUT                    0
#define FD_CLI_TELNET_MAX_TIMEOUT                    160

/* dot1x default values */
#define L7_DOT1X_PORT_NO_QUIET_PERIOD                60
#define L7_DOT1X_PORT_NO_TX_PERIOD                   30
#define L7_DOT1X_PORT_NO_SUPP_TIMEOUT                30
#define L7_DOT1X_PORT_NO_SERV_TIMEOUT                30
#define L7_DOT1X_PORT_NO_REAUTH_PERIOD               3600
#define L7_DOT1X_PORT_NO_MAX_REQ                     2

/* RADIUS default values */
#define L7_RADIUS_SERVER_NO_TIMEOUT                  6
#define L7_RADIUS_SERVER_NO_RETRANSMIT               10

#ifndef L7_CHASSIS
#define CLIUNIT_X_DOES_NOT_EXIST            "\r\nSwitch %d does not exist!"
#else
#define CLIUNIT_X_DOES_NOT_EXIST            "\r\nSlot %d is empty!"
#endif

#ifndef L7_CHASSIS
#define CLIUNIT_TYPE                        "Switch Type"
#else
#define CLIUNIT_TYPE                        "Module Type"
#endif

#ifndef L7_CHASSIS
#define CLIERROR_INVALID_SWITCH_IDX         "\r\nERROR: Invalid switch index %d!"
#define CLIERROR_INVALID_UNIT               "\r\nERROR: Invalid switch ID!"
#else
#define CLIERROR_INVALID_SWITCH_IDX         "\r\nERROR: Invalid module index %d!"
#define CLIERROR_INVALID_UNIT               "\r\nERROR: Invalid slotID!"
#endif

#ifndef L7_CHASSIS
#define CLIERROR_SHOW_SWITCH_IDX            "\r\nType command `show supported switchtype' to view switch indexes."
#else
#define CLIERROR_SHOW_SWITCH_IDX            "\r\nType command `show supported moduletype' to view module indexes."
#endif

#ifndef L7_CHASSIS
#define CLISWITCH_NO_SUPPORTED_TYPE         "No supported switch types."
#else
#define CLISWITCH_NO_SUPPORTED_TYPE         "No supported module types."
#endif

#ifndef L7_CHASSIS
#define CLIUNIT_DESCRIPTION                 "Switch Description"
#define CLIUNIT_STATUS                      "Switch Status"
#else
#define CLIUNIT_DESCRIPTION                 "Module Description"
#define CLIUNIT_STATUS                      "Module Status"
#endif

#define CLIERROR_SNOOPIGMP_VLANMODESET(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UnableToSetAdminModeForIgmpSnoopingOnVlan, 1, 0, 1, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_UnableToSetAdminModeForMldSnoopingOnVlan, 1, 0, 1, 0, L7_NULLPTR)

#define CLISYNTAX_SNOOPIGMP_SETIGMP(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_SNOOPIGMP_NOSETIGMP(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLIERROR_SNOOPIGMP_VLANMODEGET(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UnableToObtainAdminModeForIgmpSnoopingOnVlan, 1, 0, 1, 0, L7_NULLPTR) \
  : pStringAddCR(pStrInfo_base_UnableToObtainAdminModeForMldSnoopingOnVlan, 1, 0, 1, 0, L7_NULLPTR)

#define CLISYNTAX_SNOOPIGMP_SETFASTLEAVE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpFastLeaveVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldFastLeaveVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPIGMP_NOSETFASTLEAVE(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpFastLeaveVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldFastLeaveVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPIGMP_SETGMI(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpGrpMbrshipIntvlVidGrpMbrshipIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldGrpMbrshipIntvlVidGrpMbrshipIntvl, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_SNOOPIGMP_NOSETGMI(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpGrpMbrshipIntvlVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldGrpMbrshipIntvlVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPIGMP_SETMAXRESPONSETIME(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMaxresponseVidMaxRespTime, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldMaxresponseVidMaxRespTime, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_SNOOPIGMP_NOSETMAXRESPONSETIME(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseNoSetIgmpMaxresponseVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMaxresponseVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#define CLISYNTAX_SNOOPIGMP_SETMCASTRTREXPIRYTIME(x) (x == L7_AF_INET) ? pStringAddCR(pStrInfo_base_UseSetIgmpMcrtrexpiretimeVidMcrouterExpiryTime, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseSetMldMcrtrexpiretimeVidMcrouterExpiryTime, 1, 0, 0, 0, pStrErr_common_IncorrectInput)
#define CLISYNTAX_SNOOPIGMP_NOSETMCASTRTREXPIRYTIME(x) (x == L7_AF_INET) ?  pStringAddCR(pStrInfo_base_UseNoSetIgmpMcrtrexpiretimeVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput) \
  : pStringAddCR(pStrInfo_base_UseNoSetMldMcrtrexpiretimeVid, 1, 0, 0, 0, pStrErr_common_IncorrectInput)

#endif          /* _CLI_MACRO_ */

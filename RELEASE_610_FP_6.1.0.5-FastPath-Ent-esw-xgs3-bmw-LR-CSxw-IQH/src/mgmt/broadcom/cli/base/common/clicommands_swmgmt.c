/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/base/clicommands_swmgmt.c
*
* @purpose create the tree for CLI Switch Management Commands Tree
*
* @component user interface
*
* @comments none
*
* @create  07/07/2003
*
* @author  TCS
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands.h"
#include "dot1q_exports.h"

#ifdef L7_WIRELESS_PACKAGE
 #include "clicommands_wireless.h"
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
 #include "clicommands_cp.h"
#endif

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
#include "clicommands_base_ipv6.h"
#include "usmdb_util_api.h"
#include "fdb_exports.h"
#include "cli_web_exports.h"
#endif
#include "sim_exports.h"

/*********************************************************************
*
* @purpose  Build the Switch Management bridge aging-time commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobSWMgmtBridgeTime(EwsCliCommandP depth1)
{
  /* depth1 = Global Config Mode */
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Bridge_1, pStrInfo_base_CfgFdb, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_AgingTime, pStrInfo_base_CfgFdbAgeTimeout, commandBridgeAgingTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_FDB_MIN_AGING_TIMEOUT, L7_FDB_MAX_AGING_TIMEOUT);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_CfgFdbSeconds, NULL, L7_NO_OPTIONAL_PARAMS);

  if ( usmDbFeaturePresentCheck(unit, L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID) == L7_TRUE  )
  {
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_BridgeAgingFdbId, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_BridgeAgingFdbAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  else
  {
    depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Management snmp-server commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreeGlobSWMgmtSnmpserver(EwsCliCommandP depth2)
{
  /* depth2 = "snmp-server" */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Community_1, pStrInfo_base_CfgSnmpCommunity, commandSnmpServerCommunityName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_SwmgmtSnmpCommAdd, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ipaddr_2, pStrInfo_base_SnmpSrvrCommunityIpAddr, commandSnmpServerCommunityIpAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_base_CfgSnmpCommunityName, NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_base_SwmgmtSnmpCommIp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Name, pStrInfo_base_CfgSnmpCommunityName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ipmask_1, pStrInfo_base_CfgSnmpCommunityIpMask, commandSnmpServerCommunityIPMask, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_base_CfgSnmpCommunityName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipmask, pStrInfo_base_CfgSnmpCommIpMask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Name, pStrInfo_base_CfgSnmpCommunityName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApShowRunningMode, pStrInfo_base_SnmpSrvrCommunityMode, commandSnmpserverCommunityMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_base_CfgSnmpCommunityName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ro, pStrInfo_base_SwmgmtSnmpCommModeReadOnly, commandSnmpServerCommunityRo, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_base_SwmgmtSnmpCommName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Rw, pStrInfo_base_SwmgmtSnmpCommModeReadWrite, commandSnmpServerCommunityRw, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_base_SwmgmtSnmpCommName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Contact, pStrInfo_base_CfgStatsContact, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Con, pStrInfo_base_CfgSysContact, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Enbl_2, pStrInfo_base_SnmpTrapEnbl, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Traps, pStrInfo_base_SnmpTrapEnbl, commandSnmpServerEnableTraps, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_base_SnmpTrapEnblTrapsAuth, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

   #ifdef L7_CAPTIVE_PORTAL_PACKAGE
    depth5 = ewsCliAddNode(depth4, "captive-portal ", "Enable/Disable sending Captive Portal traps.", commandSnmpServerEnableTrapsCP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   #endif

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Linkmode, pStrInfo_base_CfgTrapFlagsLinksTatus, commandEnableTrapsLinkMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Multiusers, pStrInfo_base_CfgTrapFlagsMultiUsr, commandSnmpServerEnableTrapsMultiusers, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Stpmode, pStrInfo_base_CfgTrapFlagsStp, commandSnmpServerEnableTrapsSTPMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_WIRELESS_PACKAGE
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ws_3,
                           pStrInfo_base_CfgTrapFlagsWireless,
                           commandSnmpServerEnableTrapsWirelessMode, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif

    depth3 = ewsCliAddNode(depth2, pStrInfo_common_ApShowRunningLocation, pStrInfo_base_CfgStatsLocation, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Loc, pStrInfo_base_CfgSysLocation, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Sysname, pStrInfo_base_CfgStatsName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_CfgSysname, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Management snmptrap commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobSWMgmtSnmpTrap(EwsCliCommandP depth1)
{
  /* depth1 = Global Config */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Snmptrap, pStrInfo_base_CfgTrapFlags, commandSnmpTrap, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_base_SnmpTrapCreateName, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ipaddr_2, pStrInfo_base_SnmpTrapCreateIpAddr, NULL, 2, 
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapCreateIpAddr, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Snmpversion,pStrInfo_base_SnmpTrapVer, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_base_SnmpVer1, pStrInfo_base_UseSnmpV1Traps, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_base_SnmpVer2, pStrInfo_base_UseSnmpV2Traps, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_NORMAL_ONLY);

    /*depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapCreateIpAddr, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NO_ONLY);*/

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ip6addr_1,  pStrInfo_base_SnmpTrapCreateIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Ip6addr,  pStrInfo_base_SnmpTrapCreateIp, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Snmpversion,pStrInfo_base_SnmpTrapVer, NULL, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_base_SnmpVer1, pStrInfo_base_UseSnmpV1Traps, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_base_SnmpVer2, pStrInfo_base_UseSnmpV2Traps, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NORMAL_ONLY);

    /*depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ip6addr,  pStrInfo_base_SnmpTrapCreateIp, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, 
                           L7_STATUS_NO_ONLY);*/
#endif


    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Ipaddr_2, pStrInfo_base_SnmpTrapIpAddr, commandSnmpTrapIPAddr, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_SnmpTrapIpAddrName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapIpAddrIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Ipaddrnew, pStrInfo_base_SnmpTrapIpAddrIpAddrNew, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    /*#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
       depth3 = ewsCliAddNode(depth2, "ip6addr ", CLISNMPTRAPIP6ADDR_HELP,commandSnmpTrapIPAddr,L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, "<name> ", CLISNMPTRAPIPADDRNAME_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
       depth5 = ewsCliAddNode(depth4, "<ip6addr> ", CLISNMPTRAPIP6ADDR_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
       depth6 = ewsCliAddNode(depth5, "<ipaddrnew> ", CLISNMPTRAPIPADDRIP6ADDRNEW_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, "<cr> ", CLINEWLINE_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
     #endif
     */
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_ApShowRunningMode, pStrInfo_base_SnmpTrapMode, commandSnmpTrapMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_SnmpTrapIpAddrName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapModeIpAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Ip6addr, pStrInfo_base_SnmpTrapModeIpAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Snmpversion, pStrInfo_base_SnmpVerChg, commandSnmpTrapVersion, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_base_SnmpTrapIpAddrName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapModeIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_SnmpVer1, pStrInfo_base_UseSnmpV1Traps, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_SnmpVer2, pStrInfo_base_UseSnmpV2Traps, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Ip6addr, pStrInfo_base_SnmpTrapModeIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_SnmpVer1, pStrInfo_base_UseSnmpV1Traps, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_SnmpVer2, pStrInfo_base_UseSnmpV2Traps, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

  }
}

/*********************************************************************
*
* @purpose  Build the Switch Management network configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivSWMgmtNetworkConf(EwsCliCommandP depth1)
{
  /* depth1 = Privileged Exec Mode */

  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_WsShowRunningNwMode, pStrInfo_base_SwmgmtNw, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_Ipv6Nw, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* ipv6 address */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Addr_4, pStrInfo_base_Ipv6NwAddr, commandNetworkIpv6Prefix, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_PrefixPrefixLen, pStrInfo_common_Ipv6Prefix, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IPV6_PREFIX_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode (depth5, pStrInfo_common_Ipv6Eui, pStrInfo_base_Ipv6PrefixEui, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_AutoConfig, pStrInfo_base_AutoConfigHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Dhcp_1, pStrInfo_base_IPv6ProtoDhcp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


  /* ipv6 admin mode */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Enbl_2, pStrInfo_base_Ipv6NwAdminMode, commandNetworkIpv6Enable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* ipv6 gateway */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Gateway_2, pStrInfo_base_Ipv6NwGateway, commandNetworkIpv6Gateway, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Ipv6Addr, pStrInfo_base_Ipv6GatewayAddr, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Javamode, pStrInfo_base_CfgNwJavaMode, commandNetworkJavaMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

 if(usmDbFeaturePresentCheck(unit, L7_SIM_COMPONENT_ID, L7_SIM_ALLOW_LOCAL_MAC_USAGE_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_MacAddr_3, pStrInfo_base_NwMacAddr, commandNetworkMACAddress, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_MacAddr, pStrInfo_base_NwMacAddrVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_MacType, pStrInfo_base_CfgNetMacType, commandNetworkMACType, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Burnedin, pStrInfo_base_CfgNetMacAddrType, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_LocalAuth, pStrInfo_base_CfgNetMacAddrType, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MgmtVlan, pStrInfo_base_NwMgmtVlan, commandNetworkMgmtVlan,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_NO_ONLY);
  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth4 = ewsCliAddNode(depth3, buf,  pStrInfo_base_NwMgmtVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Parms, pStrInfo_base_NwParms, commandNetworkParams, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapModeIpAddr, NULL, 2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Netmask, pStrInfo_base_NwParmsNmask, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Gateway, pStrInfo_base_NwParmsGway, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Proto_1, pStrInfo_base_CfgNetProto, commandNetworkProtocol, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Bootp_1, pStrInfo_base_NwProtoBoot, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dhcp_1, pStrInfo_base_NwProtoDhcp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_None_3, pStrInfo_base_NwProtoNone, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management set ip commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivSWMgmtServicePortConfig(EwsCliCommandP depth1)
{
  /* depth1 = Privileged Exec Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_CfgServiceportKeyword, pStrInfo_base_SetServiceport, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpOption, pStrInfo_base_SetIpRoute, commandSetServiceportIp, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapModeIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Netmask, pStrInfo_base_NwParmsNmask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Gateway, pStrInfo_base_NwParmsGway, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Diffserv_5, pStrInfo_base_Ipv6ServPort, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    /* ipv6 address */
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Addr_4, pStrInfo_base_Ipv6ServPortAddr, commandServicePortIpv6Prefix, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode (depth4, pStrInfo_common_PrefixPrefixLen, pStrInfo_common_Ipv6Prefix, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_NODE_DATA_TYPE, L7_IPV6_PREFIX_DATA_TYPE);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode (depth5, pStrInfo_common_Ipv6Eui, pStrInfo_base_Ipv6PrefixEui, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode (depth4, pStrInfo_common_AutoConfig, pStrInfo_base_AutoConfigHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode (depth4, pStrInfo_common_Dhcp_1, pStrInfo_base_IPv6ProtoDhcp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);


    /* ipv6 admin mode */
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Enbl_2, pStrInfo_base_Ipv6ServPortAdminMode, commandServicePortIpv6Enable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /* ipv6 gateway */
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Gateway_2, pStrInfo_base_Ipv6ServPortGateway, commandServicePortIpv6Gateway, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depth5 = ewsCliAddNode (depth4, pStrInfo_common_Ipv6Addr, pStrInfo_base_Ipv6GatewayAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Proto_1, pStrInfo_base_SetIpProto, commandSetServiceportProtocol, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Bootp_1, pStrInfo_base_SetIpProtoBoot, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dhcp_1, pStrInfo_base_SetIpProtoDhcp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_None_3, pStrInfo_base_SetIpProtoNone, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Management set prompt commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivSWMgmtSetPrompt(EwsCliCommandP depth2)
{
  /* depth2 = "set" */
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Prompt, pStrInfo_base_SetPrompt, commandSetPrompt, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_PromptString, pStrInfo_base_CfgPromptName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management telnetcon configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreePrivSWMgmtTelnetConConf(EwsCliCommandP depth1)
{
  /* depth1 = Privileged Exec Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Telnetcon, pStrInfo_base_TelnetCon, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Maxsessions, pStrInfo_base_TelnetConMaxSessions, commandTelnetConMaxSessions, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  sprintf(buf, "<%d-%d> ", 0, FD_CLI_WEB_DEFAULT_NUM_SESSIONS);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_RemoteConMaxSessions, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Timeout_2, pStrInfo_base_TelnetConTimeout, commandTelnetConTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to160, pStrInfo_base_CfgTelnetTime,NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management telnet configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeLineConfigSWMgmtTelnetConf(EwsCliCommandP depth2)
{
  /* depth1 = Lineconfig Mode */
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Transport, pStrInfo_base_TransPortTelnet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Input_5, pStrInfo_base_InputTelnet, commandTelnetCon, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Telnet, pStrInfo_base_CfgTelnetStatus, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#ifdef L7_OUTBOUND_TELNET_PACKAGE
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Output, pStrInfo_base_OutPutTelnet, commandOutboundTelnet, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Telnet, pStrInfo_base_CfgTelnetStatus, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
}

/*********************************************************************
*
* @purpose  Build the Switch Management show forwarding database commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivSWMgmtShowForwardingDB(EwsCliCommandP depth2)
{
  /* depth2 = Show */
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Fwddb, pStrInfo_base_ShowFwdDb, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Agetime, pStrInfo_base_ShowFwdDbAgeTime, commandShowForwardingDbAgetime, L7_NO_OPTIONAL_PARAMS);
  if ( usmDbFeaturePresentCheck(unit, L7_FDB_COMPONENT_ID, L7_FDB_AGETIME_PER_VLAN_FEATURE_ID) == L7_TRUE  )
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Fdbid, pStrInfo_base_ShowFwdDbFdbId, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  else
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Management show snmp commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivSWMgmtShowSnmpData(EwsCliCommandP depth2)
{
  /* depth2 = Show */
  EwsCliCommandP depth3, depth4;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE )
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Snmpcommunity, pStrInfo_base_ShowSnmp, commandShowSnmp, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Snmptrap, pStrInfo_base_ShowSnmpTrap, commandShowSnmptrap, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Management show trapflags commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivSWMgmtShowTrapFlags(EwsCliCommandP depth2)
{
  /* depth2 = Show */
  EwsCliCommandP depth3, depth4;
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Trapflags, pStrInfo_base_ShowTrapFlags, commandShowTrapflags, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management set EIA parameters commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeLineConfigSWMgmtSetEIAParm(EwsCliCommandP depth2)
{
  /* depth1 = Line Config Mode */

  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Serial, pStrInfo_base_SetSerial, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Baudrate, pStrInfo_base_SetSerialBaudRate, commandSetSerialBaudrate, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate1200, pStrInfo_base_SetSerialBaudSpeed1200, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate2400, pStrInfo_base_SetSerialBaudSpeed2400, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate4800, pStrInfo_base_SetSerialBaudSpeed4800, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate9600, pStrInfo_base_SetSerialBaudSpeed9600, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate19200, pStrInfo_base_SetSerialBaudSpeed19200, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate38400, pStrInfo_base_SetSerialBaudSpeed38400, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate57600, pStrInfo_base_SetSerialBaudSpeed57600, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SerialBaudRate115200, pStrInfo_base_SetSerialBaudSpeed115200, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Timeout_2, pStrInfo_base_SetSerialTimeout, commandSetSerialTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Range0to160, pStrInfo_base_CfgTelnetTime, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management show network commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtShowNetwork(EwsCliCommandP depth2)
{
  /* depth2 = "show" */

  EwsCliCommandP depth3, depth4;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  EwsCliCommandP depth5, depth6, depth7;
#endif

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_WsShowRunningNwMode, pStrInfo_base_ShowNw, commandShowNetwork, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
   depth4 = ewsCliAddNode(depth3, pStrInfo_common_Diffserv_5, pStrInfo_base_ShowDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dhcp_1, pStrInfo_base_ShowDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowStats, pStrInfo_base_ShowDhcpv6Stats, commandShowNwIpv6DhcpStats, L7_NO_OPTIONAL_PARAMS);
   depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
 
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ndp, pStrInfo_base_ShowIpv6NwNdp, commandShowNetworkNdp, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

/*********************************************************************
*
* @purpose  Build the Switch Management show EIA parameters commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtShowSerial(EwsCliCommandP depth2)
{
  /* depth2 = "show" */

  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Serial, pStrInfo_base_ShowSerial, commandShowSerial, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management show EIA parameters commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtShowServicePort(EwsCliCommandP depth2)
{
  /* depth2 = "show" */

  EwsCliCommandP depth3, depth4;
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  EwsCliCommandP depth5, depth6, depth7;
#endif
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if ( usmDbComponentPresentCheck(unit, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE )
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_CfgServiceportKeyword, pStrInfo_base_ShowServiceport, commandShowServicePort, L7_NO_OPTIONAL_PARAMS);
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Diffserv_5, pStrInfo_base_ShowDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dhcp_1, pStrInfo_base_ShowDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowStats, pStrInfo_base_ShowDhcpv6Stats, commandShowServPortIpv6DhcpStats, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Ndp, pStrInfo_base_ShowIpv6ServiceportNdp, commandShowServicePortNdp, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    #endif
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

 /*********************************************************************
*
* @purpose  Build the Switch Management show port-channel command tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtShowPortChannel (EwsCliCommandP depth2)
{
	/* depth2 = "show" */

	EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lag_4, pStrInfo_base_ShowLag, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_base_ShowLagSummary, commandShowPortChannelSummary, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch Management bridge show inbound telnet
*           commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtShowTelnetCon(EwsCliCommandP depth2)
{
  /* depth2 = "show" */

  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Telnetcon, pStrInfo_base_ShowTelnet, commandShowTelnetCon, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
#ifdef L7_OUTBOUND_TELNET_PACKAGE
/*********************************************************************
*
* @purpose  Build the Switch Management outbound telnet configuration
*           commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeLineConfigSWMgmtOutboundTelnetConf(EwsCliCommandP depth2)
{
  /* depth1 = Lineconfig Mode */
  EwsCliCommandP  depth3, depth4, depth5;
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_SessionLimit, pStrInfo_base_TelnetNumMaxSessions, commandTelnetMaxSessions, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", L7_TELNET_MIN_SESSIONS, L7_TELNET_MAX_SESSIONS);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_TelnetNumMaxSessions, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_SessionTimeout_1, pStrInfo_base_TelnetTimeout, commandTelnetTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", L7_TELNET_MIN_SESSION_TIMEOUT, L7_TELNET_MAX_SESSION_TIMEOUT);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_CfgTelnetTime,NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management bridge show outbound telnet
*           commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtShowOutboundTelnet(EwsCliCommandP depth2)
{
  /* depth2 = "show" */

  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Telnet, pStrInfo_base_ShowOutboundTelnet, commandShowOutboundTelnet, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch Management bridge Outbound Telnet
*           commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeUserExecSWMgmtOutboundTelnet(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Telnet, pStrInfo_base_SwmgmtTelnet, commandRemoteTelnet, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_HostAddr, pStrInfo_base_SwmgmtTelnetHost, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Port, pStrInfo_base_SwmgmtTelnetPort, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_LogSeverityDebug, pStrInfo_base_SwmgmtTelnetDebug, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_base_Line_2, pStrInfo_base_SwmgmtTelnetLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode ( depth6, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode ( depth5, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Line_2, pStrInfo_base_SwmgmtTelnetLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_LogSeverityDebug, pStrInfo_base_SwmgmtTelnetDebug, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Line_2, pStrInfo_base_SwmgmtTelnetLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Line_2, pStrInfo_base_SwmgmtTelnetLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode ( depth3, pStrInfo_base_Localecho, pStrInfo_base_SwmgmtTelnetLocalEcho, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
#endif /* L7_OUTBOUND_TELNET_PACKAGE */
/*********************************************************************
*
* @purpose  Build the Switch Management telnet configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivIpTelnet(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  /* Router Config mode*/
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Telnet, pStrInfo_base_IpTelnet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Srvr_1, pStrInfo_base_IpTelnet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH );
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Enbl_2, pStrInfo_base_IpTelnet, commandIpTelnetServerEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/*********************************************************************
*
* @purpose  Build the IP HTTP Secure-Server tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreePrivIpHttp(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7;
  L7_char8 buf[32];

  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Srvr_1,pStrInfo_base_IpHttpSrvrCfg, commandIpHttpServer, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode (depth3, pStrInfo_common_Session_1,pStrInfo_base_IpHttpSession, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_HardTimeout,pStrInfo_base_IpHttpHardTimeoutCfg, commandIpHttpHardTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", FD_HTTP_SESSION_HARD_TIMEOUT_MIN, FD_HTTP_SESSION_HARD_TIMEOUT_MAX);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_base_IpHttpHardTimeoutCfg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Maxsessions,pStrInfo_common_IpHttpMaxSessionCfg, commandIpHttpMaxSessions, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", 0, FD_HTTP_DEFAULT_MAX_CONNECTIONS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_IpHttpMaxSessionCfg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode (depth4, pStrInfo_common_SoftTimeout,pStrInfo_base_IpHttpSoftTimeoutCfg, commandIpHttpSoftTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", FD_HTTP_SESSION_SOFT_TIMEOUT_MIN, FD_HTTP_SESSION_SOFT_TIMEOUT_MAX);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_base_IpHttpSoftTimeoutCfg, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Show IP HTTP Secure-Server tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreePrivShowIpHttp(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5;

/* DEPTH3 = "ip " */
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Http_1, pStrInfo_base_ShowIpHttp, commandShowIpHttp, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
/*******************************************************************
*
* @purpose  To build the ip domain-lookup tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/
void buildTreeIpDnsClient(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[32];

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_domain, 
                         pStrInfo_base_IpDomainLookup,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_lookup, 
                         pStrInfo_base_IpDomainLookup,
                         commandIpDomainLookup, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_name, pStrInfo_base_IpDomainName,
                         commandIpDomainName, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_name_parm, 
                         pStrInfo_base_IpDomainNameName, 
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_retry, 
                         pStrInfo_base_IpDomainNameRetry,
                         commandIpDomainRetry, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", L7_DNS_DOMAIN_RETRY_NUMBER_MIN, 
                           L7_DNS_DOMAIN_RETRY_NUMBER_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_IpDomainRetryName, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_timeout, 
                         pStrInfo_base_IpDomainTimeout,
                         commandIpDomainTimeout, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  sprintf(buf, "<%d-%d> ", L7_DNS_DOMAIN_TIMEOUT_MIN, 
                           L7_DNS_DOMAIN_TIMEOUT_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_IpDomainTimeoutName, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_list, 
                         pStrInfo_base_IpDomainList,
                         commandIpDomainList, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_name_parm, 
                         pStrInfo_base_IpDomainListName, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

/*******************************************************************
*
* @purpose  To build the ip name-server tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/
void buildTreeIpNameServer(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9, depth10;
  EwsCliCommandP depth11, depth12, depth13;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_name, pStrInfo_base_IpNameServer,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Srvr_1, 
                         pStrInfo_base_IpNameServer,
                         commandIpNameServer, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL,2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_SrvIpAddr1, 
                         pStrInfo_base_IpNameServerAddress,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_SrvIpAddr2, 
                         pStrInfo_base_IpNameServerAddress,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_base_SrvIpAddr3, 
                         pStrInfo_base_IpNameServerAddress,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth8 = ewsCliAddNode(depth7, pStrInfo_base_SrvIpAddr4, 
                         pStrInfo_base_IpNameServerAddress,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
                         NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth9 = ewsCliAddNode(depth8, pStrInfo_base_SrvIpAddr5, 
                         pStrInfo_base_IpNameServerAddress,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth10 = ewsCliAddNode(depth9, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth10 = ewsCliAddNode(depth9, pStrInfo_base_SrvIpAddr6, 
                         pStrInfo_base_IpNameServerAddress,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth11 = ewsCliAddNode(depth10, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth11 = ewsCliAddNode(depth10, pStrInfo_base_SrvIpAddr7, 
                          pStrInfo_base_IpNameServerAddress,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth12 = ewsCliAddNode(depth11, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth12 = ewsCliAddNode(depth11, pStrInfo_base_SrvIpAddr8, 
                          pStrInfo_base_IpNameServerAddress,
                          NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth13 = ewsCliAddNode(depth12, pStrInfo_common_Cr, 
                          pStrInfo_common_NewLine, NULL, 2,
                          L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*******************************************************************
*
* @purpose  To build the ip host name to address mapping tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/
void buildTreeIpHostnameToAddressMapping(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Host_1, pStrInfo_common_IpHost,
                         commandIpHostNameAddress, 2, L7_NO_COMMAND_SUPPORTED,
                         L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_name, 
                         pStrInfo_common_DnsHostName, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddress, 
                         pStrInfo_base_HostIpAddress, NULL,
                         2, L7_NODE_DATA_TYPE, L7_IP_ADDRESS_DATA_TYPE);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

/*******************************************************************
*
* @purpose To build the tree for clear host name
*          from host name-to-address cache
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/
void buildTreeClearHost(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Host_4, 
                         pStrInfo_base_IpClearHost,
                         commandClearHostname, L7_NO_OPTIONAL_PARAMS);

  /* Change Made for FASTPATH : Instead of <hostname|*>, we use <hostname | all>     in FASTPATH
   */

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_HostAll, 
                         pStrInfo_base_IpClearHostName,
                         NULL , L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
}

/*******************************************************************
*
* @purpose To build the tree for show hosts
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/
void buildTreeShowHosts(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Hosts, pStrInfo_base_ShowHost, 
                         commandShowHosts,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Hostname, 
                         pStrInfo_base_DnsHostName, NULL ,
                         L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, 
                         pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

}
/*******************************************************************
*
* @purpose To build the tree for clear network ipv6 dhcp statistics
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/
void buildTreeClearNetwork(EwsCliCommandP depth2)
{
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;

   depth3 = ewsCliAddNode(depth2, pStrInfo_common_WsShowRunningNwMode, pStrInfo_base_ClearDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth4 = ewsCliAddNode(depth3, pStrInfo_common_Diffserv_5, pStrInfo_base_ClearDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dhcp_1, pStrInfo_base_ClearDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowStats, pStrInfo_base_ClearDhcpv6Stats, commandClearNwIpv6DhcpStats, L7_NO_OPTIONAL_PARAMS);
   depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}
/*******************************************************************
*
* @purpose To build the tree for clear network ipv6 dhcp statistics
*
* @param void
*
* @returntype void
*
* @note
*
* @end
**********************************************************************/

void buildTreeClearServport(EwsCliCommandP depth2)
{
#if defined (L7_IPV6_PACKAGE) || defined (L7_IPV6_MGMT_PACKAGE)
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

   unit = cliGetUnitId();
  if ( usmDbComponentPresentCheck(unit, L7_SERVICE_PORT_PRESENT_ID) == L7_TRUE )
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_CfgServiceportKeyword, pStrInfo_base_ClearDhcpv6Stats, commandClearServPortIpv6DhcpStats, L7_NO_OPTIONAL_PARAMS);

   depth4 = ewsCliAddNode(depth3, pStrInfo_common_Diffserv_5, pStrInfo_base_ClearDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dhcp_1, pStrInfo_base_ClearDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowStats, pStrInfo_base_ClearDhcpv6Stats, NULL, L7_NO_OPTIONAL_PARAMS);
   depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
#endif
}


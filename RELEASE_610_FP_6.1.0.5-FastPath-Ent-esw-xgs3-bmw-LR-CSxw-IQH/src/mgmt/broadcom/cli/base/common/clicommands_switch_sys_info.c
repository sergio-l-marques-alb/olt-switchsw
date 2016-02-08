/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_switch_sys_info.c
 *
 * @purpose create the cli for Switching System Info and Stats
 *
 * @component user interface
 *
 * @comments contains the code to build the tree for Switching Device
 * @comments also contains functions that allow tree navigation
 *
 * @create  07/10/20003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_card.h"
#include "unitmgr_api.h"
#ifdef L7_ROUTING_PACKAGE
#include "clicommands_loopback.h"
#endif
#ifdef L7_CHASSIS
#include "clicommands_chassis.h"
#endif
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "log_exports.h"

/*********************************************************************
*
* @purpose  Build the Switch System Info show eventlog commands tree
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
void buildTreePrivSwInfoEventLog(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Eventlog, pStrInfo_base_ShowEventLog, commandShowEventLog, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (cliIsStackingSupported() == L7_TRUE)
  {
    #ifndef L7_CHASSIS
    sprintf(buf, pStrInfo_common_StackingUnit, L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Unit_1, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    #else
    sprintf(buf, pStrInfo_common_ChassisSlot, L7_CHASSIS_MIN_SLOT_NUMBER, L7_CHASSIS_MAX_SLOT_NUMBER);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_SlotTree, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    #endif
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch System Info show hardware commands tree
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
void buildTreePrivSwInfoHardware(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Hardware, pStrInfo_base_ShowHardware, commandShowHardware, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch System Info show logging commands tree
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

void buildTreePrivSwInfoLogging(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5;
  #ifndef L7_CHASSIS
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  EwsCliCommandP depth6;
  #endif

  L7_uint32 unit;

  unit = cliGetUnitId ();

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Logging_3, pStrInfo_base_ShowLogging, commandShowLogging, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Bufed, pStrInfo_base_ShowLoggingBufed, commandShowLoggingBuffered, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Hosts, pStrInfo_base_ShowLoggingHosts, commandShowLoggingHosts, L7_NO_OPTIONAL_PARAMS);
 #ifndef L7_CHASSIS
  if (cliIsStackingSupported() == L7_TRUE)
  {
    sprintf(buf, pStrInfo_common_StackingUnit, L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Unit_1, buf, NULL, 3, L7_NODE_UINT_RANGE,
                           L7_UNITMGR_MIN_UNIT_NUMBER, L7_UNITMGR_MAX_UNIT_NUMBER);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  else
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  #else
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  #endif
  if (usmDbFeaturePresentCheck(unit, L7_LOG_COMPONENT_ID, L7_LOG_PERSISTENT_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Persistent, pStrInfo_base_ShowLoggingPersistent, commandShowLoggingPersistent, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Traplogs, pStrInfo_base_ShowLoggingTrapLog, commandShowLoggingTraplogs, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch System Info show mac-addr-table commands tree
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

void buildTreePrivSwInfoMacAddrTable(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MacAddrTbl, pStrInfo_base_ShowFdb, commandShowMacAddrTable, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Macaddr, pStrInfo_base_Mac, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, NULL, pStrInfo_common_DiffservVlanId, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Count_1, pStrInfo_base_ShowMacAddrTblCount, commandShowMacAddrTableCount, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_ShowMacAddrTblIntf, commandShowMacAddrTableInterface, L7_NO_OPTIONAL_PARAMS);

  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_base_ShowMacAddrTblVlanId, commandShowMacAddrTableVlanId, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, NULL, pStrInfo_common_DiffservVlanId, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
*
*
* @returntype struct EwsCliCommandP
*
* @notes none
*
* @end
*
*********************************************************************/

void buildTreePrivSwInfoSysInfo(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Sysinfo, pStrInfo_base_ShowSysinfo, commandShowSysinfo, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch System Info show interface commands tree
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

void buildTreePrivSwInfoShowInterfaceEthernet(EwsCliCommandP depth3)
{
  /* depth2 = "show" */
  EwsCliCommandP depth4, depth5, depth6;

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_DhcpEtherNet, pStrInfo_base_ShowIntfEtherNet, commandShowInterfacesEthernet, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Switchport, pStrInfo_base_SwitchPort, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch System Info show interface commands tree
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

void buildTreePrivSwInfoShowInterfaceSwitchport(EwsCliCommandP depth3)
{
  /* depth2 = "show" */
  EwsCliCommandP depth4, depth5;

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Switchport, pStrInfo_base_SwitchPort, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch System Info show logging commands tree
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

void buildTreePrivSwInfoSntp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Sntp_1, pStrInfo_base_ShowSntp, commandShowSntp, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Client_1, pStrInfo_base_ShowSntpClient, commandShowSntpClient, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Srvr_1, pStrInfo_base_ShowSntpSrvr, commandShowSntpServer, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch System Info show version command tree
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

void buildTreePrivSwInfoShowVersion(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ver_2, pStrInfo_base_ShowVer, commandShowVersion, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the Switch System Info show tech-support command tree
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

void buildTreePrivSwInfoShowTechSupport(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_TechSupport, pStrInfo_base_ShowTechSupport, commandShowTechSupport, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

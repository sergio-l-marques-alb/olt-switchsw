/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_pml.c
*
* @purpose Port mac-locking commands
*
* @component Port mac-locking
*
* @comments contains the code to build the  Port mac-locking tree
* @comments gloabl mode and interface mode
*
* @create  03/5/2007
*
* @author  nshrivastav
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_pml.h"
#include "usmdb_pml_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"

#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif

/*********************************************************************
*
* @purpose  Build the Switch Device port-security global config commands tree
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

void buildTreeGlobalSwDevPortSecurity(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3;
  L7_uint32 unit = cliGetUnitId();

  if (usmDbComponentPresentCheck(unit, L7_PORT_MACLOCK_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_PortSecurity_1, pStrInfo_base_PortSecurity, commandPortSecurity, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

}

/*********************************************************************
*
* @purpose  Build the Switch Device port-security interface config commands tree
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

void buildTreeInterfaceSwDevPortSecurity(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_char8 validVlanRange[L7_CLI_MAX_STRING_LENGTH];

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_PortSecurity_1, pStrInfo_base_IntfPortSecurity, commandPortSecurity, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MacAddr_3, pStrInfo_base_IntfPortSecurityMacAddr, commandPortSecurityMacAddress, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Move, pStrInfo_base_IntfPortSecurityMacAddrMov, commandPortSecurityMacAddressMove, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_MacAddr, pStrInfo_base_IntfPortSecurityMacAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintf(validVlanRange, "<%d-%d> ", (L7_DOT1Q_MIN_VLAN_ID), L7_DOT1Q_MAX_VLAN_ID);
  depth5 = ewsCliAddNode(depth4, validVlanRange, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MaxDyn, pStrInfo_base_IntfPortSecurityDyn, commandPortSecurityMaxDynamic, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to600, pStrInfo_base_IntfPortSecurityDyn, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MaxStatic, pStrInfo_base_IntfPortSecurityStatic, commandPortSecurityMaxStatic, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to20, pStrInfo_base_IntfPortSecurityStatic, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
}

/*********************************************************************
*
* @purpose  Build the Switch Device port-security interface snmp trap commands tree
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

void buildTreeInterfaceSwDevPortSecuritySnmp(EwsCliCommandP depth1)
{

  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  L7_uint32 unit; /*get switch ID based on presence/absence of STACKING package */

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }
  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_SnmpSrvr_1, pStrInfo_base_IntfPortSecuritySnmpViolationTrap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Enbl_2, pStrInfo_base_IntfPortSecuritySnmpViolationTrap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Traps, pStrInfo_base_IntfPortSecuritySnmpViolationTrap, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Violation, pStrInfo_base_IntfPortSecuritySnmpViolationTrap, commandSnmpServerEnableTrapsViolationMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device show port-security (Port MAC Locking) command tree
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
void buildTreePrivSwDevShowPortSecurity(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_uint32 unit = cliGetUnitId();

  if (usmDbComponentPresentCheck(unit, L7_PORT_MACLOCK_COMPONENT_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_PortSecurity_1, pStrInfo_base_ShowPortSecurity, commandShowPortSecurity, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_ShowPortSecurityAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

#ifdef L7_STACKING_PACKAGE
    depth4 = ewsCliAddNode (depth3, pStrErr_common_AclIntfsStacking, pStrInfo_base_PortSecurityInfo, NULL, L7_NO_OPTIONAL_PARAMS);
#else
    depth4 = ewsCliAddNode (depth3, pStrErr_common_AclIntfs, pStrInfo_base_PortSecurityInfo, NULL, L7_NO_OPTIONAL_PARAMS);
#endif

    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Dyn_2, pStrInfo_base_ShowPortSecurityDyn, commandShowPortSecurityDynamic, L7_NO_OPTIONAL_PARAMS);
    depth5 = buildTreeInterfaceHelp( depth4,  L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Static2, pStrInfo_base_ShowPortSecurityStatic, commandShowPortSecurityStatic, L7_NO_OPTIONAL_PARAMS);
    depth5    = buildTreeInterfaceHelp( depth4,  L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Violation, pStrInfo_base_ShowPortSecurityViolation, commandShowPortSecurityViolation, L7_NO_OPTIONAL_PARAMS);
    depth5 = buildTreeInterfaceHelp( depth4,  L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}



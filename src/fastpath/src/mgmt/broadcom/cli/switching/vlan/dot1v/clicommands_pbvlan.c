/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename clicommands_pbvlan.c
*
* @purpose Protocol-based vlan commands
*
* @component Protocol-based vlan
*
* @comments Build Protocol-based vlan tree
*
* @create  04/09/2007
*
* @author  nshrivastav
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "clicommands_pbvlan.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"

#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif


/*********************************************************************
*
* @purpose  Build the tree for show commands
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
void buildTreePrivSwPbvlan(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6;
  L7_uint32 unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Proto_1, pStrInfo_base_ShowProto, commandShowPortProtocol, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Grpid, pStrInfo_base_ShowProtoDetailedRange, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the tree for Interface commands
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
void buildTreeInterfaceSwDevPortVlanGroup(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  L7_uint32 unit;

  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  sprintf(buf, "Enter Group Ids in the range <%d-%d> ",L7_PBVLAN_MIN_NUM_GROUPS, L7_PBVLAN_MAX_NUM_GROUPS);


  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_Proto_1, pStrInfo_base_CfgProto, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_MacAclVlan_1, pStrInfo_base_CfgProtoIntfAdd, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Grp_2,pStrInfo_base_CfgProtoIntfAdd,commandProtocolVlanGroupAdd, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_base_ShowProtoDetailedRange, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  Build the tree for Global mode
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
void buildTreeGlobalSwDevVlanPbvlan(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9, depth10;
  L7_uint32 unit;

  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  sprintf(buf, "<%d-%d> ",L7_PBVLAN_MIN_NUM_GROUPS, L7_PBVLAN_MAX_NUM_GROUPS);

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }


  if (usmDbComponentPresentCheck(unit, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Proto_1, pStrInfo_base_CfgProto, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Grp_2, pStrInfo_base_VlanProtoGrp, commandVlanProtocolGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Add_1, pStrInfo_base_VlanProtoGrpAdd_protocols, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Proto_1, pStrInfo_base_VlanProtoGrpAdd_protocols, commandVlanProtocolGroupAdd, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, buf, pStrInfo_base_ShowProtoDetailedRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth8 = ewsCliAddNode(depth7, "ethertype ", pStrInfo_base_VlanProtoGrpAdd_protocols, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth9 = ewsCliAddNode(depth8, "<protocol-list|arp|ip|ipx> ", pStrInfo_base_CfgProtoProtoRange_string, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth10 = ewsCliAddNode(depth9, "<cr> ", pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_base_ShowProtoDetailedRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    /*depth8 = ewsCliAddNode(depth7, pStrInfo_common_IpOption,pStrInfo_base_CfgProtoProtoRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Arp_2,pStrInfo_base_CfgProtoProtoRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth8 = ewsCliAddNode(depth7, pStrInfo_base_Ipx_1,pStrInfo_base_CfgProtoProtoRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth5 = ewsCliAddNode(depth4, pStrInfo_base_Grpname, pStrInfo_base_CfgProtoCreateRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);*/

    depth5 = ewsCliAddNode(depth4, "name ", pStrInfo_base_VlanProtoGrp_name, commandVlanProtocolGroupName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_base_ShowProtoDetailedRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, "<groupName> ", pStrInfo_base_CfgProtoCreateRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);


    /*depth5 = ewsCliAddNode(depth4, pStrInfo_common_Remove_1, pStrInfo_base_VlanProtoGrpRemove, commandVlanProtocolGroupRemove, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_base_ShowProtoDetailedRange, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);*/

  }
}
/*********************************************************************
*
* @purpose  Build the Global mode all commands.
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
void buildTreeGlobalSwDevModeAllVlanGroupAll(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;

  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_Proto_1, pStrInfo_base_CfgProto, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_MacAclVlan_1, pStrInfo_base_CfgProtoIntfAdd, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Grp_2, pStrInfo_base_Grp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_ProtoIntfAddAll, commandProtocolVlanGroupAddAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Grpid, pStrInfo_base_ShowProtoDetailedRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  Build the vlan mode commands.
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
void buildTreeVlanDBSwDevVlanPbvlan(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf1[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  sprintf(buf1, "<%d-%d> ",L7_PBVLAN_MIN_NUM_GROUPS, L7_PBVLAN_MAX_NUM_GROUPS);
  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  if (usmDbComponentPresentCheck(unit, L7_PBVLAN_COMPONENT_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_Proto_1, pStrInfo_base_CfgProtoProto, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Grp_2, pStrInfo_base_CfgProtoVlanAdd,commandProtocolGroupAdd, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, buf1, pStrInfo_base_ShowProtoDetailedRange, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}


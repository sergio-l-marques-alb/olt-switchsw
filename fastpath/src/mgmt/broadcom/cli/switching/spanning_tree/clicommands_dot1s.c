/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/dot1s/clicommands_dot1s.c
 *
 * @purpose create the tree for CLI Switch Spanning Tree
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/07/2003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "clicommands_dot1s.h"
#include "clicommands_card.h"
#include "usmdb_common.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "dot1s_exports.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Build the Switch Spanning Tree configuration commands tree
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
void buildTreeGlobalSWSpanSpanningTreeConf(EwsCliCommandP depth1)
{
  /* depth1 = Global Config Mode */
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_SpanTree_2, pStrInfo_switching_SpanTree, commandSpanningTree, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPPDUFILTER) == L7_TRUE)
  {
      /* Command : spanning-tree bpdufilter */
      depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Bpdufilter, pStrInfo_switching_SpanTreeBpduFilter, 
                             NULL, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_PassDefl,pStrInfo_switching_SpanTreeBpduFilter, 
                             commandSpanningTreeBpduFilterDefault,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                             2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  }


  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPDUGUARD) == L7_TRUE)
  {
      /* Command : spanning-tree bpdu-protection */
      depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Bpduguard,pStrInfo_switching_SpanTreeBpduGuard, 
                             commandSpanningTreeBpduGuard,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,
                             L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Bpdumigrationcheck, pStrInfo_switching_SpanTreeBpduMigration, commandSpanningTreeBpdumigrationcheck, L7_NO_OPTIONAL_PARAMS);
  if (cliIsStackingSupported() == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrErr_common_AclIntfsStacking, pStrInfo_common_SlotPortWithUnit, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  else
  {
    depth4 = ewsCliAddNode(depth3, pStrErr_common_AclIntfs, pStrInfo_common_SlotPortWithoutUnit, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_switching_SpanTreeBpduMigrationCheckAll, commandSpanningTreeBpdumigrationcheckAll, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cfg_8, pStrInfo_switching_SpanTreeCfg, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApProfileShowRunningName, pStrInfo_switching_SpanTreeCfgName, commandSpanningTreeConfigurationName, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_switching_SpanTreeCfgNameVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Revision, pStrInfo_switching_SpanTreeCfgRevision, commandSpanningTreeConfigurationRevision, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_switching_SpanTreeCfgRevisionVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Forceversion, pStrInfo_switching_SpanTreeForceVer, commandSpanningTreeForceVersion, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Dot1d_1, pStrInfo_switching_SpanTreeForceVer802Dot1D, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Dot1s, pStrInfo_switching_SpanTreeForceVer802Dot1S, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Dot1w, pStrInfo_switching_SpanTreeForceVer802Dot1X, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_ForwardTime, pStrInfo_switching_SpanTreeForwardTime, commandSpanningTreeForwardTime, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Range4to30, pStrInfo_switching_SpanTreeForwardTimeVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_HoldCount, pStrInfo_switching_SpanTreeHoldCount, commandSpanningTreeHoldCount, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to10, pStrInfo_switching_SpanTreeHoldCountVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_MaxAge, pStrInfo_switching_SpanTreeMaxAge, commandSpanningTreeMaxAge, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Range6to40, pStrInfo_switching_SpanTreeMaxAgeVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_MaxHops, pStrInfo_switching_SpanTreeMaxHop, commandSpanningTreeMaxHops, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Range6to40, pStrInfo_switching_SpanTreeMaxHopVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Mst_1, pStrInfo_switching_CfgDot1sMst, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Inst, pStrInfo_switching_CfgDot1sMstCreate, commandSpanningTreeMstInstance, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_switching_Dot1sMstIdParm, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_SwitchPri_1, pStrInfo_switching_CfgDot1sMstPortPri, commandSpanningTreeMstPriority, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_switching_Dot1sMstIdParm, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_switching_Range0to61440, pStrInfo_switching_SpanTreeDot1sMstIdParmVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_switching_CfgDot1sMstVlan, commandSpanningTreeMstVlan, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_switching_Dot1sMstIdParm, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  osapiSnprintf(buf, (L7_int32)sizeof(buf), pStrInfo_base_VlanRange_1, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_VlanList_1, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Port_4, pStrInfo_switching_CfgDot1sPort, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApShowRunningMode, pStrInfo_switching_CfgDot1sPortMode, commandSpanningTreePortModeAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_switching_SpanTreeDot1sSlotPortAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/*********************************************************************
*
* @purpose  Build the Switch Spanning Tree configuration commands tree
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

void buildTreeInterfaceSWSpanSpanningTreeConf(EwsCliCommandP depth1)
{
  /* depth1 = Interface Config Mode */
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_SpanTree_2, pStrInfo_switching_SpanTree, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPPDUFILTER) == L7_TRUE)
  {

     /*BPDU Filter*/
      depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Bpdufilter,pStrInfo_switching_SpanTreeBpduFilter,commandSpanningTreeIntfBpduFilter,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr,pStrInfo_common_NewLine, NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_BPPDUFLOOD) == L7_TRUE)
  {
    /*BPDU Flood*/
     depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Bpduflood,pStrInfo_switching_SpanTreeBpdu,commandSpanningTreeIntfBpduFlood,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
     depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr,pStrInfo_common_NewLine, NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  if ((usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ROOTGUARD) == L7_TRUE) &&
     (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_LOOPGUARD) == L7_TRUE))
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Portguard, pStrInfo_switching_SpanTreePortGuard, commandSpanningTreePortGuard, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth4 = ewsCliAddNode(depth3, pStrInfo_switching_PortguardLoop, pStrInfo_switching_SpanTreeLoopGuard, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_None_3, pStrInfo_switching_SpanTreeNoneGuard, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    
    depth4 = ewsCliAddNode(depth3, pStrInfo_switching_PortguardRoot, pStrInfo_switching_SpanTreeRootGuard, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);    
  }
  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1S_COMPONENT_ID, L7_DOT1S_FEATURE_ROOTGUARD) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Tcnguard, pStrInfo_switching_SpanTreeTcnGuard, commandSpanningTreeTcnGuard, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Edgeport, pStrInfo_switching_CfgDot1sPortEdgePort, commandSpanningTreeCstPortEdgePort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_AutoEdge_1, pStrInfo_switching_SpanTreeDot1sAutoEdge, commandSpanningTreeAutoEdge, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Port_4, pStrInfo_switching_CfgDot1sPort, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_ApShowRunningMode, pStrInfo_switching_CfgDot1sPortMode, commandSpanningTreePortModeInterface, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Mst_1, pStrInfo_switching_CfgDot1sMst, commandSpanningTreeMstPortPathCostPriority, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_0_3, pStrInfo_switching_Dot1sMstIdParm, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cost2, pStrInfo_switching_CfgDot1sMstPortPathCost, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_switching_Range1to200000000, pStrInfo_switching_SpanTreeCstPortPathCostVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Auto2, pStrInfo_switching_SpanTreeCstPortPathCostAutoVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_switching_ExternalCost, pStrInfo_switching_SpanTreeDot1sMstPortPathExtCost, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_switching_Range1to200000000, pStrInfo_switching_SpanTreeCstPortPathCostVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Auto2, pStrInfo_switching_SpanTreeDot1sMstPortPathExtCostAutoVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_switching_PortPri_1, pStrInfo_switching_CfgDot1sMstPortPri, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_switching_Range0to240, pStrInfo_switching_SpanTreeCstPortPriVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX);

  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_switching_Dot1sMstIdParm, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cost2, pStrInfo_switching_CfgDot1sMstPortPathCost, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_switching_Range1to200000000, pStrInfo_switching_SpanTreeCstPortPathCostVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Auto2, pStrInfo_switching_SpanTreeCstPortPathCostAutoVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_switching_PortPri_1, pStrInfo_switching_CfgDot1sMstPortPri, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_switching_Range0to240, pStrInfo_switching_SpanTreeCstPortPriVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the Switch Spanning Tree show commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes creates the tree for all the show commands for spanning-tree
*
* @end
*
*********************************************************************/

void buildTreeUserExecSWSpanShowSpanningTree(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_SpanTree_2, pStrInfo_switching_ShowDot1s, commandShowSpanningTree, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_switching_ShowDot1sCst, commandShowSpanningTreeBrief, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_switching_ShowSpanTreeCstPort, commandShowSpanningTreeInterface, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_switching_Mst_1, pStrInfo_switching_ShowDot1sMst, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Detailed, pStrInfo_switching_ShowDot1sMst, commandShowSpanningTreeMstDetailed, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, NULL, pStrInfo_switching_Dot1sMstIdParm, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Port_4, pStrInfo_switching_ShowDot1sMstPort, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Detailed, pStrInfo_switching_ShowDot1sMstPortDetailed, commandShowSpanningTreeMstPortDetailed, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, NULL, pStrInfo_switching_Dot1sMstIdParm, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);

  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowSummary, pStrInfo_switching_ShowDot1sMstPortSummary, commandShowSpanningTreeMstPortSummary, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, NULL, pStrInfo_switching_Dot1sMstIdParm, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1S_CIST_INSTANCE, L7_DOT1S_MSTID_MAX);

  depth8 = buildTreeInterfaceHelp(depth7, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Active, pStrInfo_common_ShowSpanPortActive, NULL, L7_NO_OPTIONAL_PARAMS);
  depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Dot1xShowSummary, pStrInfo_switching_ShowDot1sMst, commandShowSpanningTreeMstSummary, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowSummary, pStrInfo_switching_ShowDot1sSummary, commandShowSpanningTreeSummary, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_switching_ShowDot1sVlan, commandShowSpanningTreeVlan, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, NULL, pStrInfo_switching_Dot1sVlanParm, NULL, 3, L7_NODE_UINT_RANGE, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

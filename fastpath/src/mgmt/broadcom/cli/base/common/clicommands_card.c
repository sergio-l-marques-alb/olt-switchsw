/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_card.c
*
* @purpose Card commands
*
* @component User Interface
*
* @comments contains the code to build the root of the tree
* @comments also contains functions that allow tree navigation
*
* @create  07/31/2003
*
* @author  rjindal
* @end
*
*********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "clicommands_card.h"
#include "usmdb_cardmgr_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_user_mgmt_api.h"
#ifdef L7_CHASSIS
#include "clicommands_chassis.h"
#endif
#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif

/*********************************************************************
* @purpose  Build the tree for slot user-exec commands
*
* @param  depth1  @b{(input)) the command structure
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void buildTreeUserExecShowSlot(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  /* depth1 = show */

  /* show slot [[unit/] slot] */
  #ifndef L7_CHASSIS
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Slot_2, pStrInfo_base_ShowSlot, commandShowSlot, L7_NO_OPTIONAL_PARAMS);
  #else
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Slot_2, pStrInfo_base_ShowSlot, commandShowChassisSlot, L7_NO_OPTIONAL_PARAMS);
  #endif
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  #ifndef L7_CHASSIS
  depth3 = buildTreeSlotHelper(depth2, L7_STATUS_NORMAL_ONLY);
  #else
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_SlotTree, pStrInfo_base_ChassisSlot_1, NULL, L7_NO_OPTIONAL_PARAMS);
  #endif
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the tree for show supported user-exec commands
*
* @param  depth1  @b{(input)) the command structure
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void buildTreeUserExecShowSupported(EwsCliCommandP depth1)
{
#ifndef FEAT_METRO_CPE_V1_0
  EwsCliCommandP depth2, depth3, depth4, depth5;


  /* depth1 = show */

  /* show supported */
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_RatesUpPort, pStrInfo_base_ShowSupported, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
#ifndef L7_CHASSIS
#ifndef FEAT_METRO_CPE_V1_0
  /* show supported cardtype [cardindex] */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Cardtype, pStrInfo_base_ShowSupportedCardType, commandShowSupportedCardType, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_CardIdx, pStrInfo_base_GiveCardIdx, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
#endif

#ifdef L7_STACKING_PACKAGE
  /* show supported switchtype [switchindex] */
#ifdef L7_CHASSIS
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Moduletype, CLISHOWSUPPORTEDSWITCHTYPE_HELP, commandShowSupportedSwitchType, L7_NO_OPTIONAL_PARAMS);
#else
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Switchtype, CLISHOWSUPPORTEDSWITCHTYPE_HELP, commandShowSupportedSwitchType, L7_NO_OPTIONAL_PARAMS);
#endif
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_CHASSIS
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ModuleIdx, CLISTACKINGUNITTYPE_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
#else
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_SwitchIdx, CLISTACKINGUNITTYPE_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
#endif
}

/*********************************************************************
* @purpose  Build the tree for slot-card global config commands
*
* @param  depth1  @b{(input)) the command structure
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void buildTreeGlobalSlotCard(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* depth1 = Switch(config)# (i.e., Global Config mode) */

  /* card <[unit/] slot> <cardindex> */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Slot_2, pStrInfo_base_CfgSlots, commandSlot, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = buildTreeSlotHelper(depth2, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_CardIdx, pStrInfo_base_GiveCardIdx, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
* @purpose  Build the tree for slot-set global config commands
*
* @param  depth1  @b{(input)) the command structure
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void buildTreeGlobalSlotSet(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  /* depth1 = Switch(config)# (i.e., Global Config mode) */

  /* set slot ... */
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Slot_2, pStrInfo_base_SlotCfgSet, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* set slot disable <[unit/] slot | all> */
  #ifndef L7_CHASSIS
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dsbl2, pStrInfo_base_CfgSlotAdminMode, commandSetSlotDisable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  #else
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dsbl2, pStrInfo_base_CfgSlotAdminMode, commandSetChassisSlotDisable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  #endif
  depth5 = buildTreeSlotAllHelper(depth4, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* set slot power <[unit/] slot | all> */
  #ifndef L7_CHASSIS
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_RadioPowerOption, pStrInfo_base_CfgSlotPowerMode, commandSetSlotPower, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  #else
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_RadioPowerOption, pStrInfo_base_CfgSlotPowerMode, commandSetChassisSlotPower, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  #endif
  depth5 = buildTreeSlotAllHelper(depth4, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
}

/*********************************************************************
* @purpose  Build tree helper routine
*
* @param  depth  @b{(input)) the command structure
*
* @returns  EwsCliCommandP  the command structure
*
* @comments
*
* @end
*********************************************************************/
EwsCliCommandP buildTreeSlotHelper(EwsCliCommandP depth, EwsCliNodeNoFormStatus noFormStatus)
{
  EwsCliCommandP nextDepth;

#ifdef L7_STACKING_PACKAGE
  nextDepth = ewsCliAddNode (depth, pStrInfo_base_UnitSlotTree, pStrInfo_base_StackingSlot, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
#else
  nextDepth = ewsCliAddNode (depth, pStrInfo_common_SlotTree, pStrInfo_base_Slot, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
#endif

  return nextDepth;
}

/*********************************************************************
* @purpose  Build tree helper routine
*
* @param  depth  @b{(input)) the command structure
*
* @returns  EwsCliCommandP  the command structure
*
* @comments
*
* @end
*********************************************************************/
EwsCliCommandP buildTreeSlotAllHelper(EwsCliCommandP depth, EwsCliNodeNoFormStatus noFormStatus)
{
  EwsCliCommandP nextDepth;

#ifdef L7_STACKING_PACKAGE
 #ifndef L7_CHASSIS
  nextDepth = ewsCliAddNode (depth, pStrInfo_base_UnitSlotOrAllTree, pStrInfo_base_StackingSlotOrAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
 #else
  nextDepth = ewsCliAddNode (depth, pStrInfo_base_SlotOrAllTree, pStrInfo_base_SlotOrAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
 #endif
#else
  nextDepth = ewsCliAddNode (depth, pStrInfo_base_SlotOrAllTree, pStrInfo_base_SlotOrAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, noFormStatus);
#endif

  return nextDepth;
}

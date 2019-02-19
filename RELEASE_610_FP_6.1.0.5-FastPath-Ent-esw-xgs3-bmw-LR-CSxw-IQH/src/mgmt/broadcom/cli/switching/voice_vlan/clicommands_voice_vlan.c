/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename clicommands_voice_vlan.c
*
* @purpose create the tree for CLI VOICE VLAN
*
* @component user interface
*
* @comments none
*
* @create  
*
* @author  
* @end
*
**********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "cliapi.h"
#include "clicommands_voice_vlan.h"
#include "cli_web_exports.h"

/*********************************************************************
* @purpose  To Build the Voice VLAN tree in Global configuration mode
*
* @param    EwsCliCommandP depth1   @b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeGlobalConfigVoiceVlan(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, "voice ", CLI_VOICE_VLAN_MODE_HELP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, "vlan ", CLI_VOICE_VLAN_MODE_HELP, commandVoiceVlan, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


/*********************************************************************
* @purpose  To Build the Voice VLAN tree in interface configuration mode
*
* @param    EwsCliCommandP depth1   @b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeInterfaceConfigVoiceVlan(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6,depth7;
  L7_uint32 unit = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* Initializing unit */
  memset(buf, 0x00, sizeof(buf));
  unit = cliGetUnitId();

  /* Building the Voice VLAN Command Tree */
  depth2 = ewsCliAddNode(depth1, "voice ", CLI_VOICE_VLAN_HELP, NULL,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2 , "vlan ", CLI_VOICE_VLAN_HELP, commandVoiceVlanId,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
    
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine,NULL,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode(depth3, "<id> ",CLI_VOICE_VLAN_ID_HELP,NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth4 = ewsCliAddNode(depth3, "dot1p ", CLI_VOICE_VLAN_PRIORITY_HELP,commandVoiceVlanPriority,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CosIntRangeParam, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  depth5 = ewsCliAddNode(depth4,buf,CLI_VOICE_VLAN_PRIORITY_HELP,NULL,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth4 = ewsCliAddNode(depth3, "none " ,CLI_VOICE_VLAN_NONE_HELP,commandVoiceVlanNone,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth4 = ewsCliAddNode(depth3, "untagged ",CLI_VOICE_VLAN_UNTAG_HELP,commandVoiceVlanUntagged,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4,pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth4 = ewsCliAddNode(depth3, "data ", CLI_VOICE_VLAN_DATA_PRIORITY_HELP, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, "priority ",CLI_VOICE_VLAN_DATA_PRIORITY_HELP,commandVoiceVlanDataPriority,L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "untrust ",CLI_VOICE_VLAN_DATA_PRIORITY_UNTRUST,NULL,L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, "trust ",CLI_VOICE_VLAN_DATA_PRIORITY_TRUST,NULL,L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
}


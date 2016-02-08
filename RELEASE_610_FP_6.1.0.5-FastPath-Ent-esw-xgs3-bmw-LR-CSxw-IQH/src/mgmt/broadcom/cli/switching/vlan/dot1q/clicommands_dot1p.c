/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/clicommands_dot1p.c
 *
 * @purpose create the tree for CLI Authentication commands
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  09/15/2003
 *
 * @author  jshaw
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "clicommands_dot1p.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"

/*********************************************************************
*
* @purpose  Build the dot1p privileged exec config commands tree
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
void buildTreeCosDot1pConfig(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* Initializing unit */
  memset(buf, 0, sizeof(buf));
  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Dot1pMapping, pStrInfo_switching_Dot1pPriv, commandClassofserviceDot1pmapping, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CosIntRangeParam, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_switching_Dot1pPri_1, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_CosIntRangeParam, L7_DOT1P_MIN_TRAFFIC_CLASS, L7_DOT1P_MAX_TRAFFIC_CLASS);

  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_switching_Dot1pPriQueue, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the dot1p privileged exec config commands tree
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
void buildTreeInterfaceVlanPortDot1pPriority(EwsCliCommandP depth2)
{
  /* depth1 = clear */
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* Initializing unit */
  unit = cliGetUnitId();
  memset(buf, 0, sizeof(buf));
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_SwitchPri_1, pStrInfo_switching_CfgVlanPortPri, commandVlanPortPriority, L7_NO_OPTIONAL_PARAMS);
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CosIntRangeParam, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_switching_VlanPortPri, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the dot1p privileged exec config commands tree
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
void buildTreeInterfaceVlanPortDot1pPriorityAll(EwsCliCommandP depth3)
{
  /* depth1 = clear */
  L7_uint32 unit;
  EwsCliCommandP depth4, depth5, depth6, depth7;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* Initializing unit */
  unit = cliGetUnitId();
  memset(buf, 0, sizeof(buf));
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_SwitchPri_1, pStrInfo_switching_CfgVlanPortPri, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_switching_VlanPortPriAll, commandVlanPortPriorityAll, L7_NO_OPTIONAL_PARAMS);
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CosIntRangeParam, L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_switching_VlanPortPri, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the dot1p privileged exec show commands tree
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
void buildTreePrivDot1pShow(EwsCliCommandP depth2)
{
  /* depth2 = show */
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5;

  /* Initializing unit */
  unit = cliGetUnitId();

  depth3 = ewsCliAddNode(depth2, pStrInfo_switching_Dot1pMapping, pStrInfo_switching_ShowDot1pMapPing, commandShowClassofServiceDot1pmapping, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
 *
 * (C) Copyright Broadcom 2007
 *
 **********************************************************************
 *
 * @filename clicommands_isdp.c
 *
 * @purpose cli commands for the isdp
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  21/11/2007
 *
 * @author  Rostyslav Ivasiv
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_base_cli.h"
#include "ews.h"
#include "cliutil.h"
#include "clicommands_isdp.h"
#include "compdefs.h"
#include "cli_web_exports.h"


/*********************************************************************
*
* @purpose build the tree isdp config mode
* @param EwsCliCommandP depth1
*
* @returntype  void
*
* @notes none
*
* @end
*************************************************************************/
void buildTreeConfigIsdp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Isdp, pStrInfo_base_IsdpConfigureHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IsdpAdvertise, pStrInfo_base_IsdpAdvertiseHelp, commandIsdpAdvertiseV2, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_ISDP_HOLDTIME_MIN, L7_ISDP_HOLDTIME_MAX);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IsdpHoldtime, pStrInfo_base_IsdpHoldtimeHelp, commandIsdpHoldtime, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_IsdpHoldtimeRangeHelp, NULL, 3, L7_NODE_UINT_RANGE, L7_ISDP_HOLDTIME_MIN, L7_ISDP_HOLDTIME_MAX);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IsdpRun, pStrInfo_base_IsdpRunHelp, commandIsdpRun, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_ISDP_TIMER_MIN, L7_ISDP_TIMER_MAX);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_IsdpTimer, pStrInfo_base_IsdpTimerHelp, commandIsdpTimer, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_base_IsdpTimerRangeHelp, NULL, 3, L7_NODE_UINT_RANGE, L7_ISDP_TIMER_MIN, L7_ISDP_TIMER_MAX);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose build the tree for 'clear isdp *'
* @param EwsCliCommandP depth2
*
* @returntype  void
*
* @notes none
*
* @end
*************************************************************************/
void buildTreePrivClearIsdp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Isdp, pStrInfo_base_ClearIsdpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ClearIsdpCounters, pStrInfo_base_ClearIsdpCountersHelp, commandClearIsdpCounters, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_ClearIsdpTable, pStrInfo_base_ClearIsdpTableHelp, commandClearIsdpTable, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*****************************************************************************
*
* @purpose build the tree for 'show isdp *'
* @param EwsCliCommandP depth2
*
* @returntype  void
*
* @notes none
*
* @end
******************************************************************************/
void buildTreePrivIsdp(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Isdp, pStrInfo_base_IsdpHelp, commandShowIsdp, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IsdpEntry, pStrInfo_base_IsdpEntryHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_IsdpEntryAllHelp, commandShowIsdpEntry, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IsdpEntryName, pStrInfo_base_IsdpEntryNameHelp, commandShowIsdpEntry, 3, L7_NODE_STRING_RANGE, 1, 30);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, commandShowIsdpEntry, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IsdpInterface, pStrInfo_base_IsdpInterfaceHelp, commandShowIsdpInterface, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_IsdpInterfaceAllHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IsdpNeighbors, pStrInfo_base_IsdpNeighborsHelp, commandShowIsdpNeighbors, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_base_IsdpNeighborsDetail, pStrInfo_base_IsdpNeighborsDetailHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_IsdpNeighborsDetail, pStrInfo_base_IsdpNeighborsDetailHelp, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_IsdpTraffic, pStrInfo_base_IsdpTrafficHelp, commandShowIsdpTraffic, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/******************************************************************************
*
* @purpose build the tree for configure isdp per port
* @param EwsCliCommandP depth2
*
* @returntype  void
*
* @notes none
*
* @end
******************************************************************************/
void buildTreeInterfaceConfigIsdp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Isdp, pStrInfo_base_IsdpEnblHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Enbl_2, pStrInfo_base_IsdpIntfEnblHelp, commandIntfIsdpEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

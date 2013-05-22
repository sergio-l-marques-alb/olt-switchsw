/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/clicommands_voip.c
 *
 * @purpose create auto voip commands
 *
 * @component user interface
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  11/23/07
 *
 * @author Amitabha sen
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "clicommands_voip.h"
#include "clicommands_card.h"
#include "dot1q_api.h"
#include "l7_cos_api.h"
#include "usmdb_util_api.h"
#include "cos_exports.h"

/*********************************************************************
*
* @purpose  Build the tree nodes for Global Auto VoIP commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalAutoVoIP(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_uint32 unit;
  
  unit = cliGetUnitId();

  /* auto voip */
  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AutoVoIP_1, pStrInfo_qos_AutoVoIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3=  ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_qos_AutoVoIPAll, commandGlobalAutoVoIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_BUCKET_FEATURE_ID) == L7_TRUE)
  {
    /* Minimum Bandwidth */
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MinBandwidth_2 ,pStrInfo_qos_AutoVoIPMinBw,commandGlobalAutoVoIPMinBandwidth,2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AutoVoIPMinBw0, pStrInfo_qos_AutoVoIPSetMinBw0, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AutoVoIPMinBw64, pStrInfo_qos_AutoVoIPSetMinBw64, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AutoVoIPMinBw128, pStrInfo_qos_AutoVoIPSetMinBw128, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AutoVoIPMinBw256, pStrInfo_qos_AutoVoIPSetMinBw256, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AutoVoIPMinBw512, pStrInfo_qos_AutoVoIPSetMinBw512, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AutoVoIPMinBw1024, pStrInfo_qos_AutoVoIPSetMinBw1024, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Interface Auto VoIP commands
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeInterfaceAutoVoIP(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  L7_uint32 unit;

  unit = cliGetUnitId();

  /* auto voip */
  depth2 = ewsCliAddNode (depth1, pStrInfo_qos_AutoVoIP_1, pStrInfo_qos_AutoVoIP, commandInterfaceAutoVoIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree to display Auto VoIP config
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
void  buildTreePrivShowAutoVoIP(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2,depth3,depth4,depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AutoVoIP_1, pStrInfo_qos_ShowAutoVoIP, commandShowAutoVoIP, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_interface, pStrInfo_switching_ShowAutoVoIPInterface, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_switching_AutoVoIPAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  return;
}


/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_filter.c
 *
 * @purpose create the tree for CLI static Filter
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  04/20/2008
 *
 * @author  akulkarn
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "clicommands_filter.h"
#include "snooping_exports.h"
#include "usmdb_util_api.h"
#include "filter_exports.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"

/*********************************************************************
*
* @purpose  Build the Switch Device macfilter commands tree
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

void buildTreeGlobalSwDevMacFilter(EwsCliCommandP depth1)
{
  /* depth2 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* Configuring static filters for multicast addresses with destination ports is supported
   * on all platforms */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Macfilter, pStrInfo_base_CfgMacFilter, commandMacFilter, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Adddest, pStrInfo_base_CfgMacFilterAddDest, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_MacFilterAddDestAll, commandMacFilterAddDestAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Addsrc, pStrInfo_base_CfgMacFilterAddSrc, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_MacFilterAddSrcAll, commandMacFilterAddSrcAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

}

/*********************************************************************
*
* @purpose  Build the Switch Device macfilter commands tree
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

void buildTreeInterfaceSwDevMacFilter(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2 = NULL, depth3, depth4, depth5, depth6;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* Configuring static filters for multicast addresses with destination ports is supported
   * on all platforms */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Macfilter, pStrInfo_base_CfgMacFilter, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Adddest, pStrInfo_base_CfgMacFilterAddDest, commandMacFilterAddDest, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Addsrc, pStrInfo_base_CfgMacFilterAddSrc, commandMacFilterAddSrc, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Macaddr, pStrInfo_common_CfgNetMacAddr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device show mac-address-table commands tree
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

void buildTreePrivSwDevMacAddrTable(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_MacAddrTbl_1, pStrInfo_base_ShowMac, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Gmrp_1, pStrInfo_base_ShowMacAddrTblGmrp, commandShowMacAddressTableGmrp, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                               L7_IGMP_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Igmpsnooping, CLISHOWMAC_ADDRESS_TABLEIGMPSNOOPING_HELP(L7_AF_INET),
                           commandShowMacAddressTableIgmpSnooping, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_SNOOPING_COMPONENT_ID,
                               L7_MLD_SNOOPING_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mldsnooping, CLISHOWMAC_ADDRESS_TABLEIGMPSNOOPING_HELP(L7_AF_INET6),
                           commandShowMacAddressTableIgmpSnooping, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mcast_3, pStrInfo_base_ShowMfdbTbl, commandShowMacAddressTableMulticast, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Macaddr, pStrInfo_base_Mac, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Configuring static filters for multicast addresses with destionation ports is supported
   * on all platforms */

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Static2, pStrInfo_base_ShowMacAddrTblStatic, commandShowMacAddressTableStatic, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Macaddr, pStrInfo_base_ShowFdbLearnedParm, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_base_FilterEntriesAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Configuring static filters for multicast addresses with destionation ports is supported
   * on all platforms */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Staticfiltering, pStrInfo_base_ShowMacAddrTblStaticFiltering, commandShowMacAddressTableStaticFiltering, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Stats, pStrInfo_base_ShowMfdbStats, commandShowMacAddressTableStats, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}




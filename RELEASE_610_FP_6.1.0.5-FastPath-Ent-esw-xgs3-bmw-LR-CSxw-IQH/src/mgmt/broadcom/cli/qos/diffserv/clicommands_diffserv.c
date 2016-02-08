/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/diffserv/clicommands_diffserv.c
 *
 * @purpose create the entire cli
 *
 * @component user interface
 *
 *
 * @create  07/20/2002
 *
 * @author  Kathy McDowell
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cliapi.h"
#include "clicommands_diffserv.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "diffserv_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"

void cliTreeGlobalConfigDiffserv(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Diffserv_3, pStrInfo_qos_Diffserv_1, commandDiffserv, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  return;
}

void cliTreePrivilegedExecShowDiffserv(EwsCliCommandP depth2)
{
  L7_uint32 unit;
  L7_BOOL showServIn = L7_FALSE;
  L7_BOOL showServOut = L7_FALSE;
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;

  /* depth2 = show */

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  /* set up service interface directional values based on feature support */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {
    showServIn = L7_TRUE;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {
    showServOut = L7_TRUE;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Diffserv_3, pStrInfo_qos_ShowDiffserv, commandShowDiffserv, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Service, pStrInfo_qos_ShowDiffservService, commandShowDiffservService, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  if (showServIn == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclInStr, pStrInfo_qos_DiffservDirectionIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (showServOut == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclOutStr, pStrInfo_qos_DiffservDirectionOut, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Brief, pStrInfo_qos_ShowDiffservServiceBrief, commandShowDiffservServiceBrief, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (showServIn == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclInStr, pStrInfo_qos_DiffservDirectionIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (showServOut == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclOutStr, pStrInfo_qos_DiffservDirectionOut, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  return;
}

void cliTreePrivilegedExecShowServicePolicy(EwsCliCommandP depth2)
{
  L7_uint32 unit;
  L7_BOOL showServIn = L7_FALSE;
  L7_BOOL showServOut = L7_FALSE;
  EwsCliCommandP depth3, depth4, depth5;

  /* depth2 = show */

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  /* set up service interface directional values based on feature support */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {
    showServIn = L7_TRUE;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {
    showServOut = L7_TRUE;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_ServicePolicy_1, pStrInfo_qos_ShowServicePolicy, commandShowServicePolicy, L7_NO_OPTIONAL_PARAMS);

  if (showServIn == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclInStr, pStrInfo_qos_DirectionIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (showServOut == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclOutStr, pStrInfo_qos_DiffservDirectionOut, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  return;
}

void cliTreeInterfaceConfigServicePolicy(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE)
  {

    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_ServicePolicy_1, pStrInfo_qos_ServicePolicy, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
        usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_AclInStr, pStrInfo_qos_CfgDiffservServiceIn, commandServicePolicyIn, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PolicyMapName, pStrInfo_qos_DiffservPolicyName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
        usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgDiffservServiceOut, commandServicePolicyOut, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PolicyMapName, pStrInfo_qos_DiffservPolicyName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
  }

  return;
}

void cliTreeGlobalConfigServicePolicy(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {

    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_ServicePolicy_1, pStrInfo_qos_ServicePolicy, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
        usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_AclInStr, pStrInfo_qos_CfgDiffservServiceIn, commandServicePolicyInAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PolicyMapName, pStrInfo_qos_DiffservPolicyName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
        usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgDiffservServiceOut, commandServicePolicyOutAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PolicyMapName, pStrInfo_qos_DiffservPolicyName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  return;
}

void cliTreeUserExecClassMap(EwsCliCommandP depth2)
{
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* depth2 = show */

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_ClassMap, pStrInfo_qos_ShowClassMap, commandShowClassMap, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ClassName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  return;
}

void cliTreeClassMapConfig(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Match_1, pStrInfo_qos_DiffservClassMatch, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AclAnyStr, pStrInfo_qos_DiffservMatchAny, commandMatchAny, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_ClassMap, pStrInfo_qos_DiffservMatchClassMap, commandMatchClassMap, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Refclassname, pStrInfo_qos_DiffservRefClassName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclCos, pStrInfo_qos_MacAccessListMatchCos, commandMatchCos, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_DstAddr, pStrInfo_qos_DiffservClassMatchDstMac, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchDstMac, commandMatchDestinationAddressMac, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_MacMask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Dstip_1, pStrInfo_qos_DiffservMatchDstIp, commandMatchDstip, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipmask, pStrInfo_common_DiffservIpMask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Dstl4port, pStrInfo_qos_DiffservMatchDstL4Port, commandMatchDstl4port, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ethertype, pStrInfo_qos_DiffservMatchEtherType, commandMatchEtherType, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Keyword, pStrInfo_qos_MacAccessListEtherTypeKeyword, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_0x06000xffff, pStrInfo_qos_MacAccessListEtherTypeCustomVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpOption, pStrInfo_qos_DiffservClassMatchIp, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AclDscpStr, pStrInfo_qos_DiffservMatchIpDscp, commandMatchIpDscp, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPPRECEDENCE_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AclPrecedence, pStrInfo_qos_DiffservMatchIpPrecedence, commandMatchIpPrecedence, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AclTosStr, pStrInfo_qos_DiffservMatchIpTos, commandMatchIpTos, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Tosbits, pStrInfo_qos_AclCfgAclIpTosBits, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrErr_qos_AclTosBitsSyntax, pStrInfo_qos_DiffservTosMask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* not form of commands  -- start */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Not_2, pStrInfo_qos_DiffservExcl, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AclAnyStr, pStrInfo_qos_DiffservMatchAny, commandMatchNotAny, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_MacAclCos, pStrInfo_qos_MacAccessListMatchCos, commandMatchNotCos, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_DstAddr, pStrInfo_qos_DiffservClassMatchDstMac, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchDstMac, commandMatchNotDestinationAddressMac, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_qos_MacMask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Dstip_1, pStrInfo_qos_DiffservMatchDstIp, commandMatchNotDstip, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipmask, pStrInfo_common_DiffservIpMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Dstl4port, pStrInfo_qos_DiffservMatchDstL4Port, commandMatchNotDstl4port, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
      {
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ethertype, pStrInfo_qos_DiffservMatchEtherType, commandMatchNotEtherType, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Keyword, pStrInfo_qos_MacAccessListEtherTypeKeyword, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_0x06000xffff, pStrInfo_qos_MacAccessListEtherTypeCustomVal, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_IpOption, pStrInfo_qos_DiffservClassMatchIp, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_AclDscpStr, pStrInfo_qos_DiffservMatchIpDscp, commandMatchNotIpDscp, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_AclPrecedence, pStrInfo_qos_DiffservMatchIpPrecedence, commandMatchNotIpPrecedence, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_AclTosStr, pStrInfo_qos_DiffservMatchIpTos, commandMatchNotIpTos, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Tosbits, pStrInfo_qos_AclCfgAclIpTosBits, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrErr_qos_AclTosBitsSyntax, pStrInfo_qos_DiffservTosMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Proto_1, pStrInfo_qos_DiffservMatchProto, commandMatchNotProtocol, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_ProtoName, pStrInfo_qos_DiffservClassMatchProto, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to255, pStrInfo_qos_DiffservProtoNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_MacAclSecondaryCos, pStrInfo_qos_MacAccessListMatchSecondaryCos, commandMatchNotSecondaryCos, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_MacAclSecondaryVlan, pStrInfo_qos_MacAccessListMatchSecondaryVlan, commandMatchNotSecondaryVlan, L7_NO_OPTIONAL_PARAMS);
      osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
      {
        depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
      else
      {
        depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
        depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_SrcAddr_2, pStrInfo_qos_DiffservClassMatchSrcMac, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchSrcMac, commandMatchNotSourceAddressMac, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Macmask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Srcip_1, pStrInfo_qos_DiffservMatchSrcIp, commandMatchNotSrcip, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipmask, pStrInfo_common_DiffservIpMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Srcl4port, pStrInfo_qos_DiffservMatchSrcL4Port, commandMatchNotSrcl4port, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
      {
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_MacAccessListMatchVlan, commandMatchNotVlan, L7_NO_OPTIONAL_PARAMS);
      osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
      {
        depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
      else
      {
        depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
        depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }

    }
  }  /***Not Commands end***/

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Proto_1, pStrInfo_qos_DiffservMatchProto, commandMatchProtocol, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ProtoName, pStrInfo_qos_DiffservClassMatchProto, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to255, pStrInfo_qos_DiffservProtoNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclSecondaryCos, pStrInfo_qos_MacAccessListMatchSecondaryCos, commandMatchSecondaryCos, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclSecondaryVlan, pStrInfo_qos_MacAccessListMatchSecondaryVlan, commandMatchSecondaryVlan, L7_NO_OPTIONAL_PARAMS);
    osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    else
    {
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SrcAddr_2, pStrInfo_qos_DiffservClassMatchSrcMac, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchSrcMac, commandMatchSourceAddressMac, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Macmask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Srcip_1, pStrInfo_qos_DiffservMatchSrcIp, commandMatchSrcip, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipaddr, pStrInfo_common_DiffservIpAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipmask, pStrInfo_common_DiffservIpMask, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Srcl4port, pStrInfo_qos_DiffservMatchSrcL4Port, commandMatchSrcl4port, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_MacAccessListMatchVlan, commandMatchVlan, L7_NO_OPTIONAL_PARAMS);
    osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    else
    {
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
      depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

  }
}

void cliTreeClassMapIpv6Config(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Match_1, pStrInfo_qos_DiffservClassMatch, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AclAnyStr, pStrInfo_qos_DiffservMatchAny, commandMatchAny, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_ClassMap, pStrInfo_qos_DiffservMatchClassMap, commandMatchClassMap, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Refclassname, pStrInfo_qos_DiffservRefClassName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclCos, pStrInfo_qos_MacAccessListMatchCos, commandMatchCos, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_DstAddr, pStrInfo_qos_DiffservClassMatchDstMac, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchDstMac, commandMatchDestinationAddressMac, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_qos_MacMask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Dstip6, pStrInfo_qos_DiffservMatchDstIp_1, commandMatchDstip6, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_DstIpv6PrefixPrefixLen, pStrInfo_qos_DiffservIp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Dstl4port, pStrInfo_qos_DiffservMatchDstL4Port, commandMatchDstl4port, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ethertype, pStrInfo_qos_DiffservMatchEtherType, commandMatchEtherType, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Keyword, pStrInfo_qos_MacAccessListEtherTypeKeyword, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_0x06000xffff, pStrInfo_qos_MacAccessListEtherTypeCustomVal, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_IPDSCP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_IpOption, pStrInfo_qos_DiffservClassMatchIp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AclDscpStr, pStrInfo_qos_DiffservMatchIpDscp, commandMatchIpDscp, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Ip6flowlbl, pStrInfo_qos_DiffservIp_1, commandMatchIp6FlowLbl, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Label, pStrInfo_qos_AclMatchIpFlowLabelVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* not form of commands  -- start */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Not_2, pStrInfo_qos_DiffservExcl, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_AclAnyStr, pStrInfo_qos_DiffservMatchAny, commandMatchNotAny, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_qos_MacAclCos, pStrInfo_qos_MacAccessListMatchCos, commandMatchNotCos, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_qos_DstAddr, pStrInfo_qos_DiffservClassMatchDstMac, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchDstMac, commandMatchNotDestinationAddressMac, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_MacMask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
        depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Dstip6, pStrInfo_qos_DiffservMatchDstIp_1, commandMatchNotDstip6, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_DstIpv6PrefixPrefixLen, pStrInfo_qos_DiffservIp, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Dstl4port, pStrInfo_qos_DiffservMatchDstL4Port, commandMatchNotDstl4port, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ethertype, pStrInfo_qos_DiffservMatchEtherType, commandMatchNotEtherType, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Keyword, pStrInfo_qos_MacAccessListEtherTypeKeyword, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_qos_0x06000xffff, pStrInfo_qos_MacAccessListEtherTypeCustomVal, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_IpOption, pStrInfo_qos_DiffservClassMatchIp, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_IPDSCP_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_AclDscpStr, pStrInfo_qos_DiffservMatchIpDscp, commandMatchNotIpDscp, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Ip6flowlbl, pStrInfo_qos_DiffservIp_1, commandMatchNotIp6FlowLbl, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Label, pStrInfo_qos_AclMatchIpFlowLabelVal, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Proto_1, pStrInfo_qos_DiffservMatchProto, commandMatchNotProtocol, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_ProtoName, pStrInfo_qos_DiffservClassMatchProto, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to255, pStrInfo_qos_DiffservProtoNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_qos_MacAclSecondaryCos, pStrInfo_qos_MacAccessListMatchSecondaryCos, commandMatchNotSecondaryCos, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_qos_MacAclSecondaryVlan, pStrInfo_qos_MacAccessListMatchSecondaryVlan, commandMatchNotSecondaryVlan, L7_NO_OPTIONAL_PARAMS);
        osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);

        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                     L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
        {
          depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
          depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        }
        else
        {
          depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
          depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
          osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
          depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
          depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        }
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_qos_SrcAddr_2, pStrInfo_qos_DiffservClassMatchSrcMac, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchSrcMac, commandMatchNotSourceAddressMac, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Macmask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
        depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Srcip6, pStrInfo_qos_DiffservMatchSrcIp_1, commandMatchNotSrcip6, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_SrcIpv6PrefixPrefixLen, pStrInfo_qos_DiffservIp, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_FEATURE_ID) == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Srcl4port, pStrInfo_qos_DiffservMatchSrcL4Port, commandMatchNotSrcl4port, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_MacAccessListMatchVlan, commandMatchNotVlan, L7_NO_OPTIONAL_PARAMS);
        osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                     L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
        {
          depth5 = ewsCliAddNode(depth4, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
          depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        }
        else
        {
          depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
          depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
          osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
          depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
          depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        }
      }
    }
  }  /***Not Commands end***/

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Proto_1, pStrInfo_qos_DiffservMatchProto, commandMatchProtocol, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ProtoName, pStrInfo_qos_DiffservClassMatchProto, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to255, pStrInfo_qos_DiffservProtoNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclSecondaryCos, pStrInfo_qos_MacAccessListMatchSecondaryCos, commandMatchSecondaryCos, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclSecondaryVlan, pStrInfo_qos_MacAccessListMatchSecondaryVlan, commandMatchSecondaryVlan, L7_NO_OPTIONAL_PARAMS);
      osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
      else
      {
        depth4 = ewsCliAddNode(depth3, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
        depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SrcAddr_2, pStrInfo_qos_DiffservClassMatchSrcMac, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mac_2, pStrInfo_qos_DiffservClassMatchSrcMac, commandMatchSourceAddressMac, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Addr_1, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Macmask, pStrInfo_qos_DiffservMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Srcip6, pStrInfo_qos_DiffservMatchSrcIp_1, commandMatchSrcip6, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_SrcIpv6PrefixPrefixLen, pStrInfo_qos_DiffservIp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Srcl4port, pStrInfo_qos_DiffservMatchSrcL4Port, commandMatchSrcl4port, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PortKey, pStrInfo_qos_DiffservPortKey, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservPortNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_qos_DiffservEndPort, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_MacAccessListMatchVlan, commandMatchVlan, L7_NO_OPTIONAL_PARAMS);
      osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_MATCH_SUPPORTS_RANGES_FEATURE_ID) != L7_TRUE)
      {
        depth4 = ewsCliAddNode(depth3, buf, pStrInfo_common_DiffservVlanId, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
      else
      {
        depth4 = ewsCliAddNode(depth3, buf, pStrInfo_qos_DiffservVlanIdMin, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN, L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX);
        depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_DiffservVlanIdMax, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }
  }
}

void cliTreePrivilegedExecPolicyMap(EwsCliCommandP depth2)
{
  L7_uint32 unit;
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_BOOL showServIn = L7_FALSE;
  L7_BOOL showServOut = L7_FALSE;

  /* depth2 = show */

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  /* set up service interface directional values based on feature support */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_IN_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {
    showServIn = L7_TRUE;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE)
  {
    showServOut = L7_TRUE;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_PolicyMap_1, pStrInfo_qos_ShowDiffservPolicy, commandShowPolicyMap, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_PolicyMapName, pStrInfo_qos_DiffservPolicyName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if ((showServIn == L7_TRUE) || (showServOut == L7_TRUE))
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_qos_ShowPolicyMapIntf, commandShowPolicyMapInterface, L7_NO_OPTIONAL_PARAMS);
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

    if (showServIn == L7_TRUE)
    {
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclInStr, pStrInfo_qos_ShowPolicyMapIntfIn, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (showServOut == L7_TRUE)
    {
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclOutStr, pStrInfo_qos_ShowPolicyMapIntfOut, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  return;
}

void cliTreePolicyClassMapViolateAction(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_ViolateAction_1, pStrInfo_qos_DiffservPolicyPoliceNonConform, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Drop_2, pStrInfo_qos_DiffservPolicyNonConformDrop, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetCosTx, pStrInfo_qos_DiffservPolicyNonConformMarkCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetCosAsCos2Tx, pStrInfo_qos_DiffservPolicyNonConformMarkCosAsCos2, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetDscpTx, pStrInfo_qos_DiffservPolicyNonConformMarkDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetPrecTx, pStrInfo_qos_DiffservPolicyNonConformMarkPrec, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetSecCosTx, pStrInfo_qos_DiffservPolicyNonConformMarkSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Tx_1, pStrInfo_qos_DiffservPolicyNonConformSend, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  return;
}

void cliTreePolicyClassMapPoliceSimple(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_PoliceSimple_2, pStrInfo_qos_DiffservPolicyPoliceSimple, commandPoliceSimple, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Range1to2147483647_1, pStrInfo_qos_DiffservDataRateKbps, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range1to128, pStrInfo_qos_DiffservBurstSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_ConformAction_1, pStrInfo_qos_CfgDiffservPolicyPoliceConform, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Drop_2, pStrInfo_qos_DiffservPolicyConformDrop, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth6);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_SetCosTx, pStrInfo_qos_DiffservPolicyConformMarkCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth7);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_SetCosAsCos2Tx, pStrInfo_qos_DiffservPolicyConformMarkCosAsCos2, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth6);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_SetDscpTx, pStrInfo_qos_DiffservPolicyConformMarkDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth7);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_SetPrecTx, pStrInfo_qos_DiffservPolicyConformMarkPrec, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth7);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_qos_SetSecCosTx, pStrInfo_qos_DiffservPolicyConformMarkSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth7);
  }

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Tx_1, pStrInfo_qos_DiffservPolicyConformSend, NULL, L7_NO_OPTIONAL_PARAMS);

  cliTreePolicyClassMapViolateAction(depth6);

  return;
}

void cliTreePolicyClassMapPoliceSingleRate(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;

  unit = cliGetUnitId();

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_PoliceSingleRate_2, pStrInfo_qos_DiffservPolicyPoliceSingleRate, commandPoliceSingleRate, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Range1to2147483647_1, pStrInfo_qos_DiffservDataRateKbps, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range1to128, pStrInfo_qos_DiffservBurstSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range1to128, pStrInfo_qos_DiffservExcessBurstSize, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_ConformAction_1, pStrInfo_qos_CfgDiffservPolicyPoliceConform, NULL, L7_NO_OPTIONAL_PARAMS);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Drop_2, pStrInfo_qos_DiffservPolicyConformDrop, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth7);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
  {
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_SetCosTx, pStrInfo_qos_DiffservPolicyConformMarkCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth8);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
  {
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_SetCosAsCos2Tx, pStrInfo_qos_DiffservPolicyConformMarkCosAsCos2, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth7);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
  {
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_SetDscpTx, pStrInfo_qos_DiffservPolicyConformMarkDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth8);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
  {
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_SetPrecTx, pStrInfo_qos_DiffservPolicyConformMarkPrec, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth8);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
  {
    depth7 = ewsCliAddNode(depth6, pStrInfo_qos_SetSecCosTx, pStrInfo_qos_DiffservPolicyConformMarkSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth8);
  }
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Tx_1, pStrInfo_qos_DiffservPolicyConformSend, NULL, L7_NO_OPTIONAL_PARAMS);

  cliTreePolicyClassMapPoliceExceedAction(depth7);

  return;
}

void cliTreePolicyClassMapPoliceExceedAction(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_ExceedAction_1, pStrInfo_qos_CfgDiffservPolicyPoliceExceed, NULL, L7_NO_OPTIONAL_PARAMS);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Drop_2, pStrInfo_qos_DiffservPolicyExceedDrop, NULL, L7_NO_OPTIONAL_PARAMS);
    cliTreePolicyClassMapViolateAction(depth3);

  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetCosTx, pStrInfo_qos_DiffservPolicyExceedMarkCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth4);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetCosAsCos2Tx, pStrInfo_qos_DiffservPolicyExceedMarkCosAsCos2, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth3);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetDscpTx, pStrInfo_qos_DiffservPolicyExceedMarkDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth4);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetPrecTx, pStrInfo_qos_DiffservPolicyExceedMarkPrec, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth4);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_SetSecCosTx, pStrInfo_qos_DiffservPolicyExceedMarkSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapViolateAction(depth4);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Tx_1, pStrInfo_qos_DiffservPolicyExceedsEnd, NULL, L7_NO_OPTIONAL_PARAMS);

  cliTreePolicyClassMapViolateAction(depth3);

  return;
}

void cliTreePolicyClassMapPoliceTwoRate(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9;

  unit = cliGetUnitId();

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_PoliceTwoRate_2, pStrInfo_qos_DiffservPolicyPoliceTwoRate, commandPoliceTwoRate, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Range1to2147483647_1, pStrInfo_qos_DiffservDataRateKbps, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range1to128, pStrInfo_qos_DiffservBurstSize, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Range1to2147483647_1, pStrInfo_qos_DiffservPeakDataRateKbps, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Range1to128, pStrInfo_qos_DiffservExcessBurstSize, NULL, L7_NO_OPTIONAL_PARAMS);

  depth7 = ewsCliAddNode(depth6, pStrInfo_qos_ConformAction_1, pStrInfo_qos_CfgDiffservPolicyPoliceConform, NULL, L7_NO_OPTIONAL_PARAMS);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_Drop_2, pStrInfo_qos_DiffservPolicyConformDrop, NULL, L7_NO_OPTIONAL_PARAMS);
    cliTreePolicyClassMapPoliceExceedAction(depth8);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_SetCosTx, pStrInfo_qos_DiffservPolicyConformMarkCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth9);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_SetCosAsCos2Tx, pStrInfo_qos_DiffservPolicyConformMarkCosAsCos2, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth8);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_SetDscpTx, pStrInfo_qos_DiffservPolicyConformMarkDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth9);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_SetPrecTx, pStrInfo_qos_DiffservPolicyConformMarkPrec, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth9);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
  {
    depth8 = ewsCliAddNode(depth7, pStrInfo_qos_SetSecCosTx, pStrInfo_qos_DiffservPolicyConformMarkSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);
    depth9 = ewsCliAddNode(depth8, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreePolicyClassMapPoliceExceedAction(depth9);
  }

  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Tx_1, pStrInfo_qos_DiffservPolicyConformSend, NULL, L7_NO_OPTIONAL_PARAMS);

  cliTreePolicyClassMapPoliceExceedAction(depth8);

  return;
}

void cliTreePolicyClassMapConformColor(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_ConformColor_1, pStrInfo_qos_DiffservConformColor, commandConformColor, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID) == L7_TRUE)
  {

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ExceedColor, pStrInfo_qos_DiffservExceedColor, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  return;

}

void cliTreePolicyClassMapConfig(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclAsSignQueueStr, pStrInfo_qos_DiffservAsSignQueue, commandAssignQueue, L7_NO_OPTIONAL_PARAMS);

    osapiSnprintf(buf, sizeof(buf),  "<%d-%d> ",
                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX);
    depth3 = ewsCliAddNode(depth2, buf, pStrInfo_qos_DiffservQueueId, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_FALSE)
  {
    depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID) == L7_TRUE)
  {
    cliTreePolicyClassMapConformColor(depth1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Drop_2, pStrInfo_qos_DiffservDrop, commandDrop, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE))
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Mark_1, pStrInfo_qos_DiffservMark, commandMark, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclCos, pStrInfo_qos_DiffservPolicyMarkCos, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrErr_qos_CfgCosTrustIpDscp, pStrInfo_qos_DiffservPolicyMarkIpDscp, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Val, dscpHelp, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrErr_qos_CfgCosTrustIpPrecedence, pStrInfo_qos_DiffservPolicyMarkIpPrecedence, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservPrecedenceVal, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclSecondaryCos, pStrInfo_qos_DiffservPolicyMarkSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Range0to7, pStrInfo_qos_DiffservSecondaryCosNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
    
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_MacAclCosAsCos2, pStrInfo_qos_DiffservPolicyMarkCosAsCos2, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclMirrorStr, pStrInfo_qos_DiffservMirror, commandMirror, L7_NO_OPTIONAL_PARAMS);
    depth3 = buildTreeInterfaceHelp(depth2, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  cliTreePolicyClassMapPoliceSimple(depth1);

  cliTreePolicyClassMapPoliceSingleRate(depth1);

  cliTreePolicyClassMapPoliceTwoRate(depth1);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_DiffservRedirect, commandRedirect, L7_NO_OPTIONAL_PARAMS);
    depth3 = buildTreeInterfaceHelp(depth2, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  return;
}

void cliTreeGlobalConfigClassMap(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
  {

    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_ClassMap, pStrInfo_qos_CfgDiffservClass, cliClassMapMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservModeClassName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_DiffservMatchAccess, pStrInfo_qos_DiffservClassAcl, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Aclid, pStrInfo_qos_DiffservAclNum, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, "<ip-acl-name>", "Specify an existing IP ACL name of up to 31 alphanumeric characters.", 
                             NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Ipv6AclName, pStrInfo_qos_DiffservIpv6AclName, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_TYPE_ALL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_DiffservMatchAllType, pStrInfo_qos_DiffservClassAll, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Diffserv_4, pStrInfo_qos_DiffservClassSsl, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE)
      {
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Diffserv_5, pStrInfo_qos_DiffservClassSsl_1, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_TYPE_ANY_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_DiffservMatchAnyType, pStrInfo_qos_DiffservClassAny, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE)
      {
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Diffserv_4, pStrInfo_qos_DiffservClassSsl, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_Diffserv_5, pStrInfo_qos_DiffservClassSsl_1, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_DiffservMatchMacAccess, pStrInfo_qos_DiffservClassAclMac, NULL, L7_NO_OPTIONAL_PARAMS);
      depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_qos_MacAclName, pStrInfo_qos_DiffservMacAclName, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                   L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE)
      {
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Diffserv_4, pStrInfo_qos_DiffservClassSsl, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                     L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) == L7_TRUE)
        {
          depth6 = ewsCliAddNode(depth5, pStrInfo_common_Diffserv_5, pStrInfo_qos_DiffservClassSsl_1, NULL, L7_NO_OPTIONAL_PARAMS);
          depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
        }
      }
    }

    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Rename_1, pStrInfo_qos_ClassMapRename, commandClassMapRename, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_ClassMapName, pStrInfo_qos_DiffservClassName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_NewClassMapName, pStrInfo_qos_DiffservNewClassName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

void cliTreeGlobalConfigPolicyMap(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_PolicyMap_1, pStrInfo_qos_DiffservPolicyCreate, cliPolicyMapMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_PolicyName, pStrInfo_qos_DiffservPolicyName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclInStr, pStrInfo_qos_DiffservDirectionIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_SERVICE_OUT_SLOTPORT_FEATURE_ID) == L7_TRUE) ||
        (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_SERVICE_OUT_ALLPORTS_FEATURE_ID) == L7_TRUE ))
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclOutStr, pStrInfo_qos_DiffservDirectionOut, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    /* rename command */
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Rename_1, pStrInfo_qos_DiffservPolicyRename, commandPolicyMapRename, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Policyname, pStrInfo_qos_DiffservPolicyName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Newpolicyname, pStrInfo_qos_DiffservNewPolicyName, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  }
}

void cliTreePolicyMapClass(EwsCliCommandP depth1)
{
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no diffserv features are present, don't display any diffserv menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_FEATURE_SUPPORTED) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Class_2, pStrInfo_qos_DiffservClass, cliPolicyClassMapMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Classname, pStrInfo_qos_DiffservClassName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  To build the Policy Map tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreePolicyMap()
{
  EwsCliCommandP depth1,depth2,depth3;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliPolicyMapMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_POLICY_MAP_MODE, depth1);

  cliTreePolicyMapClass(depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}
/*********************************************************************
*
* @purpose  To build the Policy class Map  Mode tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreePolicyClassMap()
{
  EwsCliCommandP depth1;
  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliPolicyClassMapMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_POLICY_CLASS_MODE, depth1);

  cliTreePolicyClassMapConfig(depth1);
}

/*********************************************************************
*
* @purpose  To build the class Map  Mode tree
*
* @param void
*
* @returntype void
*
* @note
*
* @end
*
*********************************************************************/
void buildTreeClassMap()
{

  EwsCliCommandP depth1,depth2,depth3;
  L7_uint32 unit;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliClassMapMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_CLASS_MAP_MODE, depth1);

  depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  cliTreeClassMapConfig(depth1);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                               L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID) == L7_TRUE)
  {
    depth1 = ewsCliAddNode(NULL, NULL, NULL, cliClassMapMode, L7_NO_OPTIONAL_PARAMS);

    cliSetMode(L7_CLASS_MAP_IPV6_MODE, depth1);

    depth2 = ewsCliAddNode ( depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode ( depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    cliTreeClassMapIpv6Config(depth1);
  }
}

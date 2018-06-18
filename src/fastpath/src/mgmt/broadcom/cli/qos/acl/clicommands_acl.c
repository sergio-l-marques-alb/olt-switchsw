/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/acl/clicommands_acl.c
 *
 * @purpose create acl commands
 *
 * @component user interface
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  12/07/2003
 *
 * @author Jagdish
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "cli_mode.h"
#include "clicommands_acl.h"
#include "cli_mode.h"
#include "cliutil_acl.h"
#include "usmdb_util_api.h"
#include "acl_exports.h"
#include "cli_web_exports.h"

static EwsCliCommandP treeIpAclExtendedDeny = L7_NULLPTR;
static EwsCliCommandP treeIpAclExtendedPermit = L7_NULLPTR;

/*********************************************************************
*
* @purpose  Build the access-list command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'access-list' command tree
*
* @end
*
*********************************************************************/
void cliTreeAccessList(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }
  depth2 =  ewsCliAddNode(depth1, pStrInfo_qos_AccessList_1, pStrInfo_qos_AclAccessList, commandAccessList, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Range1to99, pStrInfo_qos_AclAccessListNormalNum, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  cliTreeAccessListNormalAction(depth3);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Range100to199, pStrInfo_qos_AclAccessListExtendedNum, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  cliTreeAccessListExtendedAction(depth3);
}


void cliTreeIpAccessList(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessList_1, pStrInfo_qos_AclAccessList, cliIpAccessListMode,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH );
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessListName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Rename_1, pStrInfo_qos_AccessListRename, commandIpAccessListRename, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Oldname, pStrInfo_qos_AccessListRenameOldName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Newname, pStrInfo_qos_AccessListRenameNewName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
}


void cliTreeIpAccessListConfig()
{
  EwsCliCommandP depth1, depth2, depth3;

  if ((treeIpAclExtendedDeny == L7_NULLPTR) ||
      (treeIpAclExtendedPermit == L7_NULLPTR))
  {
    return;
  }

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliIpAccessListMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_IPV4_ACCESS_LIST_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Deny_2, pStrInfo_qos_AclActionDeny, commandAccessList, 2,
                         L7_OPTIONS_NODE, treeIpAclExtendedDeny);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, cliIpAccessListMode, L7_NO_OPTIONAL_PARAMS );
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Permit_1, pStrInfo_qos_AclActionPermit, commandAccessList, 2,
                         L7_OPTIONS_NODE, treeIpAclExtendedPermit);
}
/*********************************************************************
*
* @purpose  Build the acl command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'acl-trapflags' command tree
*
* @end
*
*********************************************************************/
void cliTreeAclTrapflags(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclTrapflags, pStrInfo_qos_AclTrapFlags, commandAclTrapflags, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 4, L7_KEY_RESTRICTED_NODE, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}

/*********************************************************************
*
* @purpose  Build the access-list Normal command action tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'access-list' command tree
*
* @end
*
*********************************************************************/
void cliTreeAccessListNormalAction(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  EwsCliCommandP treeDeny, treeEvery, treeMirror = L7_NULLPTR;
  L7_uint32 unit;
  L7_BOOL srcIpFeatureSupported, srcMaskFeatureSupported;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  treeDeny = ewsCliAddNode(depth1, pStrInfo_common_Deny_2, pStrInfo_qos_AclActionDeny, NULL, L7_NO_OPTIONAL_PARAMS);
  treeEvery = ewsCliAddNode(treeDeny, pStrInfo_qos_AclEveryStr, pStrInfo_qos_AclActionEvery, NULL, L7_NO_OPTIONAL_PARAMS);

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclLogsStr, pStrInfo_qos_MacAccessListLog, NULL,2, L7_OPTIONS_NODE, treeEvery);
  }
    /* if feature TIME RANGE Feature present - configure time range*/
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    depth4 = ewsCliAddNode(treeEvery,pStrInfo_qos_AclTimeRangeStr, pStrInfo_qos_MacAccessListTimeRange, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_name, pStrInfo_qos_TimeRangePara, NULL, 2, L7_OPTIONS_NODE, treeEvery);

  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

    sprintf(buf, "<%d-%d> ", L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclAsSignQueueStr, pStrInfo_qos_MacAccessListAsSignQueue, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_CosQueuePara, NULL, 2, L7_OPTIONS_NODE, treeEvery);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    treeMirror = ewsCliAddNode(treeEvery, pStrInfo_qos_AclMirrorStr, pStrInfo_qos_MacAccessListMirror, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_STACKING_PACKAGE
    depth5 = ewsCliAddNode (treeMirror, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2, L7_OPTIONS_NODE, treeEvery);
#else
    depth5 = ewsCliAddNode (treeMirror, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2, L7_OPTIONS_NODE, treeEvery);
#endif
  }

  /* Note: mirror and redirect attributes are mutually-exclusive */

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    if (treeMirror != L7_NULLPTR)
    {
      depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, 2, L7_SAME_AS_NODE, treeMirror);
    }
    else
    {
      depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, L7_NO_OPTIONAL_PARAMS);
    }
#ifdef L7_STACKING_PACKAGE
    depth5 = ewsCliAddNode (depth4, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2, L7_OPTIONS_NODE, treeEvery);
#else
    depth5 = ewsCliAddNode (depth4, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2, L7_OPTIONS_NODE, treeEvery);
#endif
  }

  depth4 = ewsCliAddNode(treeEvery, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  srcIpFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                   L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID);
  srcMaskFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                     L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID);

  if ((L7_TRUE == srcIpFeatureSupported) && (L7_FALSE == srcMaskFeatureSupported))
  {
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_qos_Srcip, pStrInfo_qos_AclCfgAclSrcIp, NULL, 2, L7_OPTIONS_NODE, treeEvery);
  }
  if ((L7_TRUE == srcIpFeatureSupported) && (L7_TRUE == srcMaskFeatureSupported))
  {
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_qos_Srcip, pStrInfo_qos_AclCfgAclSrcIp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrErr_qos_AclSrcMask, pStrInfo_qos_AclCfgAclSrcMask, NULL, 2, L7_OPTIONS_NODE, treeEvery);
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Permit_1, pStrInfo_qos_AclActionPermit, NULL, 2, L7_OPTIONS_NODE, treeDeny);
}

/*********************************************************************
*
* @purpose  Build the access-list extended command  action tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'access-list' command tree
*
* @end
*
*********************************************************************/
void cliTreeAccessListExtendedAction(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  EwsCliCommandP treeDeny, treeEvery, treeMirror = L7_NULLPTR, treeSrcAny, treeDstAny, treeProtocols, treeSrcPortEq, treeDscp, treeDstPortEq;
  L7_uint32 unit;
  L7_BOOL srcIpFeatureSupported, dstIpFeatureSupported, maskFeatureSupported;
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  treeDeny = ewsCliAddNode(depth1, pStrInfo_common_Deny_2, pStrInfo_qos_AclActionDeny, NULL, L7_NO_OPTIONAL_PARAMS);
  treeEvery = ewsCliAddNode(treeDeny, pStrInfo_qos_AclEveryStr, pStrInfo_qos_AclActionEvery, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(treeEvery, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclLogsStr, pStrInfo_qos_MacAccessListLog, NULL, 2,
                           L7_OPTIONS_NODE, treeEvery);
  }
  
  /* if feature TIME RANGE Feature present - configure time range*/
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    depth4 = ewsCliAddNode(treeEvery,pStrInfo_qos_AclTimeRangeStr, pStrInfo_qos_MacAccessListTimeRange, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_name, pStrInfo_qos_TimeRangePara, NULL, 2, L7_OPTIONS_NODE, treeEvery);

  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    sprintf(buf, "<%d-%d> ", L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclAsSignQueueStr, pStrInfo_qos_MacAccessListAsSignQueue, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_CosQueuePara, NULL, 2,
                           L7_OPTIONS_NODE, treeEvery);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    treeMirror = ewsCliAddNode(treeEvery, pStrInfo_qos_AclMirrorStr, pStrInfo_qos_MacAccessListMirror, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_STACKING_PACKAGE
    depth5 = ewsCliAddNode (treeMirror, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#else
    depth5 = ewsCliAddNode (treeMirror, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#endif
  }

  /* Note: mirror and redirect attributes are mutually-exclusive */

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    if (treeMirror != L7_NULLPTR)
    {
      depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, 2,
                             L7_SAME_AS_NODE, treeMirror);
    }
    else
    {
      depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, L7_NO_OPTIONAL_PARAMS);
    }
#ifdef L7_STACKING_PACKAGE
    depth5 = ewsCliAddNode (depth4, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#else
    depth5 = ewsCliAddNode (depth4, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#endif
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
  {
    treeProtocols = ewsCliAddNode(treeDeny, pStrInfo_common_Icmp_1, pStrInfo_qos_AclMatchIcmpProto, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_common_Igmp_2, pStrInfo_qos_AclMatchIgmpProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_common_IpOption, pStrInfo_qos_AclMatchIpProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_qos_Tcp_1, pStrInfo_qos_AclMatchTcpProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_qos_Udp_1, pStrInfo_qos_AclMatchUdpProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_common_Range1to255, pStrInfo_qos_AclMatchProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);

    srcIpFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                     L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID);
    dstIpFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                     L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID);
    maskFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                    L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID);

    if (L7_TRUE == srcIpFeatureSupported)
    {
      treeSrcAny = ewsCliAddNode(treeProtocols, pStrInfo_qos_AclAnyStr, pStrInfo_qos_AccessListMatchAnySrc, NULL, L7_NO_OPTIONAL_PARAMS);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID) == L7_TRUE)
      {
        treeSrcPortEq = ewsCliAddNode(treeSrcAny, pStrInfo_qos_AclEqstr, pStrInfo_qos_AclEqualPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(treeSrcPortEq, pStrInfo_common_Range0to65535, pStrInfo_qos_CfgL4PortNum, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
        depth6 = ewsCliAddNode(treeSrcPortEq, pStrInfo_qos_Portkey, pStrInfo_qos_CfgAclPortKeyword, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
      }
      else
      {
        treeSrcPortEq = treeSrcAny;
      }
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth5 = ewsCliAddNode(treeSrcAny, pStrInfo_common_AclRangeStr, pStrInfo_qos_AclPortRange, NULL, 2,
                               L7_SAME_AS_NODE, treeSrcPortEq);
        depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Startport, pStrInfo_qos_AclSrcStartPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Endport, pStrInfo_qos_AclSrcEndPort, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
      }
      if (L7_TRUE == maskFeatureSupported)
      {
        depth4 = ewsCliAddNode(treeProtocols, pStrInfo_qos_Srcip, pStrInfo_qos_AclCfgAclSrcIp, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4, pStrErr_qos_AclSrcMask, pStrInfo_qos_AclCfgAclSrcMask, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
      }
      else
      {
        depth4 = ewsCliAddNode(treeProtocols, pStrInfo_qos_Srcip, pStrInfo_qos_AclCfgAclSrcIp, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
      }
    }
    else
    {
      /* in case source IP matching is NOT supported but destination IP matching IS supported, make sure treeSrcAny is initialized */
      treeSrcAny = treeProtocols;
    }
    if (L7_TRUE == dstIpFeatureSupported)
    {
      treeDstAny = ewsCliAddNode(treeSrcAny, pStrInfo_qos_AclAnyStr, pStrInfo_qos_AccessListMatchAnyDst, NULL, 2,
                                 L7_OPTIONS_NODE, treeEvery);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID) == L7_TRUE)
      {
        treeDstPortEq = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclEqstr, pStrInfo_qos_AclEqualPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(treeDstPortEq, pStrInfo_common_Range0to65535, pStrInfo_qos_CfgL4PortNum, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
        depth5 = ewsCliAddNode(treeDstPortEq, pStrInfo_qos_Portkey, pStrInfo_qos_CfgAclPortKeyword, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
      }
      else
      {
        treeDstPortEq = treeDstAny;
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth5 = ewsCliAddNode(treeDstAny, pStrInfo_common_AclRangeStr, pStrInfo_qos_AclPortRange, NULL, 2,
                               L7_SAME_AS_NODE, treeDstPortEq);
        depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Startport, pStrInfo_qos_AclDestStartPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Endport, pStrInfo_qos_AclDestEndPort, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
      }
      if (L7_TRUE == maskFeatureSupported)
      {
        depth5 = ewsCliAddNode(treeSrcAny, pStrInfo_qos_Dstip, pStrInfo_qos_AclCfgDstIp, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(depth5, pStrErr_qos_AclDstMask, pStrInfo_qos_AclCfgDstMask, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
      }
      else
      {
        depth4 = ewsCliAddNode(treeSrcAny, pStrInfo_qos_Dstip, pStrInfo_qos_AclCfgDstIp, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
      }
    }
    else
    {
      treeDstAny = treeSrcAny;
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID) == L7_TRUE)
    {
      treeDscp = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclDscpStr, pStrInfo_qos_AclMatchIpDscp, NULL, L7_NO_OPTIONAL_PARAMS);
      osapiStrncpySafe(buf, cliTreeAccessListDscpNodeHelp(), sizeof(buf));
      depth7 = ewsCliAddNode(treeDscp, pStrInfo_common_Val, buf, NULL, 2,
                             L7_OPTIONS_NODE, treeEvery);
    }
    else
    {
      treeDscp = treeDstAny;
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID) == L7_TRUE)
    {
      depth6 = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclPrecedence, pStrInfo_qos_CfgAclRuleMatchIpPrec, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Range0to7,  pStrInfo_qos_CfgPrecedence, NULL, 2,
                             L7_OPTIONS_NODE, treeEvery);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID) == L7_TRUE)
    {
      depth6 = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclTosStr, pStrInfo_qos_AclMatchIpTos, NULL, L7_NO_OPTIONAL_PARAMS);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
      {
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Tos, pStrInfo_qos_AclCfgAclIpTosBits, NULL, L7_NO_OPTIONAL_PARAMS);
        depth8 = ewsCliAddNode(depth7, pStrErr_qos_AclTosBitsSyntax, pStrInfo_qos_AclCfgAclIpTosMask, NULL, 2,
                               L7_OPTIONS_NODE, treeEvery);
      }
      else
      {
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Tos, pStrInfo_qos_AclCfgAclIpTosBits, NULL, 2,
                               L7_OPTIONS_NODE, treeEvery);
      }
    }
  }
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Permit_1, pStrInfo_qos_AclActionPermit, NULL, 2,
                         L7_OPTIONS_NODE, treeDeny);
  /* save context command tree pointer for re-use in named ipv4 ACL mode */
  treeIpAclExtendedPermit = depth2;
  treeIpAclExtendedDeny = treeDeny;
}

/*********************************************************************
*
* @purpose  Creates the 'show access-lists interface/vlan' commands
*
* @param void
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void cliTreeShowAccesslist(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no ACL features are present, don't display any ACL menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AccessLists, pStrInfo_qos_ShowAcl, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_qos_ShowAclIntf, commandShowAccessListInterface, L7_NO_OPTIONAL_PARAMS);
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_qos_ShowAclIntfIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclOutStr, pStrInfo_qos_ShowAclIntfOut, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* build 'show access-lists vlan' part of tree */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_ShowAclVlan, commandShowAccessListVlan, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_VlanId, pStrInfo_qos_ShowAclVlanId, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_qos_ShowAclVlanIdIn, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclOutStr, pStrInfo_qos_ShowAclVlanIdOut, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

  return;
}

/*********************************************************************
*
* @purpose  Creates the 'show ip access-lists' commands
*
* @param void
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void cliTreeShowIpAccesslist(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no ACL features are present, don't display any ACL menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AccessLists, pStrInfo_qos_ShowIpAcl, commandShowIpAccessList, L7_NO_OPTIONAL_PARAMS); 
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Range1to199, pStrInfo_qos_CfgAclId, NULL, L7_NO_OPTIONAL_PARAMS);  
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_qos_AccessListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  return;
}

/*********************************************************************
*
* @purpose  Creates the 'show ip access-lists' commands
*
* @param void
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void cliTreeShowMacAccessList(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no ACL features are present, don't display any ACL menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Mac_2, pStrInfo_qos_ShowMacAcl, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessLists, pStrInfo_qos_ShowMacAclSummary, commandShowMacAccessLists, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  return;
}

/*********************************************************************
*
* @purpose  Creates the 'show ipv6 access-lists' commands
*
* @param void
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void cliTreeShowIpv6Accesslist(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* If no ACL features are present, don't display any ACL menus */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_FALSE)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessLists, pStrInfo_qos_ShowAcl, commandShowIpv6AccessLists, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  return;
}

/*********************************************************************
*
* @purpose  Build the access-group command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'ip access-group' command tree
*
* @end
*
*********************************************************************/
void cliTreeIpAccessGroup( EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  EwsCliCommandP treeAclId;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) != L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) != L7_TRUE))
  {
    return;
  }
  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AccessGrp, pStrInfo_qos_CfgAclGrp, commandIPAccessGroup, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
  treeAclId = ewsCliAddNode(depth2, pStrInfo_common_Range1to199, pStrInfo_qos_CfgAclId, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_INTF_SEQ_NUM);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(treeAclId, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(treeAclId, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_qos_AccessGrpName, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH, L7_OPTIONS_NODE, treeAclId);
}

/*********************************************************************
*
* @purpose  Build the access-group command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'ip access-group all' command tree
*
* @end
*
*********************************************************************/
void cliTreeIpAccessGroupAll( EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8;
  EwsCliCommandP treeAclId;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit= cliGetUnitId();
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_FEATURE_SUPPORTED) == L7_FALSE)
  {
    return;
  }
  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) != L7_TRUE) &&
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) != L7_TRUE))
  {
    return;
  }
  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AccessGrp, pStrInfo_qos_CfgAclGrp, commandIPAccessGroupAll, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
  treeAclId = ewsCliAddNode(depth2, pStrInfo_common_Range1to199, pStrInfo_qos_CfgAclId, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_INTF_SEQ_NUM);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(treeAclId, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(treeAclId, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(treeAclId, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_CfgAclVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_VlanId, pStrInfo_qos_CfgAclVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_VLAN_SEQ_NUM);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth7 = ewsCliAddNode(depth6, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
    }
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth7 = ewsCliAddNode(depth6, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
    }
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_qos_AccessGrpName, NULL, 4, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH,  L7_OPTIONS_NODE,  treeAclId);
}

/*********************************************************************
*
* @purpose  To build the Bandwidth Allocation mode
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
void buildTreeGlobalMacAccessList(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Mac_2, pStrInfo_qos_MacAccessList, cliMacAccessListMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessList_1, pStrInfo_qos_MacAccessList, NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH );
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Extended, pStrInfo_qos_MacAccessListExtended, NULL,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Name, pStrInfo_qos_AccessListName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Rename_1, pStrInfo_qos_AccessListRename, commandMacAccessListRename, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Oldname, pStrInfo_qos_AccessListRenameOldName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Newname, pStrInfo_qos_AccessListRenameNewName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessGrp, pStrInfo_qos_MacAccessGrp, commandMacAccessGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessGrpName, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_INTF_SEQ_NUM);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2, L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_CfgAclVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_VlanId, pStrInfo_qos_CfgAclVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_VLAN_SEQ_NUM);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
      depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
    }
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
      depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
    }
  }
}

/*********************************************************************
*
* @purpose  To build the Bandwidth Allocation mode
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
void buildTreeInterfaceMacAccessList(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Mac_2,pStrInfo_qos_MacAccessListGrp, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH );

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessGrp, pStrInfo_qos_MacAccessGrpIntf, commandMacAccessGroup, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessGrpName, NULL, 2, L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_INTF_SEQ_NUM);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH );
    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH );
    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

}

/*********************************************************************
*
* @purpose  To build the Mac Access List Menu
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
void buildTreeMacAccessListConfig()
{
  EwsCliCommandP depth1, depth2, depth2a, depth3;
  L7_BOOL permitMenu = L7_TRUE;

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliMacAccessListMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_MAC_ACCESS_LIST_CONFIG_MODE, depth1);

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Deny_2, pStrInfo_qos_AclActionDeny, commandMacAccessListDenyPermit, L7_NO_OPTIONAL_PARAMS);
  buildTreeMacAccessListSource(depth2, permitMenu);

  depth2a = ewsCliAddNode (depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, NULL, L7_NO_OPTIONAL_PARAMS );
  depth3 = ewsCliAddNode (depth2a, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );

  depth2a = ewsCliAddNode(depth1, pStrInfo_common_Permit_1, pStrInfo_qos_AclActionPermit, commandMacAccessListDenyPermit, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddTree(depth2a, depth2);
}

void buildTreeMacAccessListSource(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth2a, depth3;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Srcmac, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Srcmacmask, pStrInfo_qos_MacAccessListMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
      buildTreeMacAccessListDest(depth3, permitMenu);

      depth2a = ewsCliAddNode(depth1, pStrInfo_qos_AclAnyStr, pStrInfo_qos_MacAccessListMatchAnySrc, NULL, L7_NO_OPTIONAL_PARAMS);
      ewsCliAddTree(depth2a, depth3);
    }
    else
    {
      buildTreeMacAccessListDest(depth2, permitMenu);

      depth2a = ewsCliAddNode(depth1, pStrInfo_qos_AclAnyStr, pStrInfo_qos_MacAccessListMatchAnySrc, NULL, L7_NO_OPTIONAL_PARAMS);
      ewsCliAddTree(depth2a, depth2);
    }

  }

  else
  {
    buildTreeMacAccessListDest(depth1, permitMenu);
  }
}

void buildTreeMacAccessListDest(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth2a, depth3, depth4;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
  {

    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Dstmac, pStrInfo_qos_MacAccessListMacAddr, NULL, L7_NO_OPTIONAL_PARAMS);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Dstmacmask, pStrInfo_qos_MacAccessListMacMask, NULL, L7_NO_OPTIONAL_PARAMS);
      buildTreeMacAccessListOptional(depth3, permitMenu);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth2a = ewsCliAddNode(depth1, pStrInfo_qos_AclAnyStr, pStrInfo_qos_MacAccessListMatchAnyDst, NULL,2,L7_OPTIONS_NODE, depth3);
    }
    else
    {
      buildTreeMacAccessListOptional(depth2, permitMenu);
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      depth2a = ewsCliAddNode(depth1, pStrInfo_qos_AclAnyStr, pStrInfo_qos_MacAccessListMatchAnyDst, NULL,L7_NO_OPTIONAL_PARAMS);
      depth3 = ewsCliAddNode(depth2a, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      ewsCliAddTree(depth2a, depth2);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID) == L7_TRUE)
    {
      depth2a = ewsCliAddNode(depth1, pStrInfo_common_MacAclBpdusStr, pStrInfo_qos_MacAccessListBpdu, NULL, L7_NO_OPTIONAL_PARAMS);
      depth3 = ewsCliAddNode(depth2a, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
      buildTreeMacAccessListOptional(depth2a, permitMenu);
      /* ewsCliAddTree(depth2a, depth2);*/
    }
  }
  else
  {
    buildTreeMacAccessListOptional(depth1, permitMenu);
  }
}

void buildTreeMacAccessListOptional(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = depth1;

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListEtherType(depth1, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListVlan(depth1, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListCos(depth1, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListSecondaryVlan(depth1, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListSecondaryCos(depth1, permitMenu);
  }

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    buildTreeMacAccessListLog(depth1);
  }
  
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    buildTreeMacAccessListTimeRange(depth1);
  }
 
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListAssignQueue(depth1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth1);
  }

}

void buildTreeMacAccessListEtherType(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth3, depth2a;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_Ethertypekey, pStrInfo_qos_MacAccessListEtherTypeKeyword, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListVlan(depth2, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListCos(depth2, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListSecondaryVlan(depth2, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListSecondaryCos(depth2, permitMenu);
  }

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    buildTreeMacAccessListLog(depth2);
  }

  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    buildTreeMacAccessListTimeRange(depth2);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListAssignQueue(depth2);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth2);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth2);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth2a = ewsCliAddNode(depth1, pStrInfo_qos_0x06000xffff, pStrInfo_qos_MacAccessListEtherTypeCustomVal, NULL, 2,L7_OPTIONS_NODE, depth2);
  /*   ewsCliAddTree(depth2a, depth2);*/

}

void buildTreeMacAccessListVlan(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth3, depth4 = 0, depth4a, depth5;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_MacAccessListMatchVlan, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AclEqstr, pStrInfo_qos_MacAccessListVlan, NULL, L7_NO_OPTIONAL_PARAMS);

      osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", (L7_ACL_MIN_VLAN_ID), L7_ACL_MAX_VLAN_ID);
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_qos_MacAclVlan, NULL, L7_NO_OPTIONAL_PARAMS);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListCos(depth4, permitMenu);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListSecondaryVlan(depth4, permitMenu);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListSecondaryCos(depth4, permitMenu);
      }

      if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
          (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
      {
        buildTreeMacAccessListLog(depth4);
      }
      
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
      {
         buildTreeMacAccessListTimeRange(depth4);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListAssignQueue(depth4);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListMirror(depth4);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListRedirect(depth4);
      }

      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      /* This logic means if L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID is supported, L7_ACL_RULE_MATCH_VLANID_FEATURE_ID
       * must be supported also or this CLI subtree will not appear.
       */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_AclRangeStr, pStrInfo_qos_MacAccessListVlanRange, NULL, L7_NO_OPTIONAL_PARAMS);

        osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", (L7_ACL_MIN_VLAN_ID), L7_ACL_MAX_VLAN_ID);
        depth4a = ewsCliAddNode(depth3, buf, pStrInfo_qos_MacAclVlansStart, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4a, buf, pStrInfo_qos_MacAclVlanEnd, NULL, L7_NO_OPTIONAL_PARAMS);
        ewsCliAddTree(depth5, depth4);
      }
    }
  }
}

void buildTreeMacAccessListCos(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_ACL_MIN_COS, L7_ACL_MAX_COS);

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_MacAclCos, pStrInfo_qos_MacAccessListMatchCos, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_qos_CosQueuePara, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListSecondaryVlan(depth3, permitMenu);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListSecondaryCos(depth3, permitMenu);
  }

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    buildTreeMacAccessListLog(depth3);
  }

  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    buildTreeMacAccessListTimeRange(depth3);
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListAssignQueue(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth3);
  }

  /* depth4 = ewsCliAddNode(depth3, "<cr> ", CLINEWLINE_HELP , NULL, L7_NO_OPTIONAL_PARAMS);*/
}

void buildTreeMacAccessListSecondaryVlan(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth3, depth4 = 0, depth4a, depth5;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE ||
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) == L7_TRUE)
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_qos_MacAclSecondaryVlan, pStrInfo_qos_MacAccessListMatchSecondaryVlan, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AclEqstr, pStrInfo_qos_MacAccessListVlan, NULL, L7_NO_OPTIONAL_PARAMS);

      osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", (L7_ACL_MIN_VLAN_ID), L7_ACL_MAX_VLAN_ID);
      depth4 = ewsCliAddNode(depth3, buf, pStrInfo_qos_MacAclVlan, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_COS2_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListSecondaryCos(depth4, permitMenu);
      }

      if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
          (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
      {
        buildTreeMacAccessListLog(depth4);
      }
 
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
      {
        buildTreeMacAccessListTimeRange(depth4);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListAssignQueue(depth4);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListMirror(depth4);
      }

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
      {
        buildTreeMacAccessListRedirect(depth4);
      }

      /* This logic means if L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID is supported, L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID
       * must be supported also or this CLI subtree will not appear.
       */
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth3 = ewsCliAddNode(depth2, pStrInfo_common_AclRangeStr, pStrInfo_qos_MacAccessListVlanRange, NULL, L7_NO_OPTIONAL_PARAMS);

        osapiSnprintf(buf, (L7_int32)sizeof(buf), "<%d-%d> ", (L7_ACL_MIN_VLAN_ID), L7_ACL_MAX_VLAN_ID);
        depth4a = ewsCliAddNode(depth3, buf, pStrInfo_qos_MacAclVlansStart, NULL, L7_NO_OPTIONAL_PARAMS);
        depth5 = ewsCliAddNode(depth4a, buf, pStrInfo_qos_MacAclVlanEnd, NULL, L7_NO_OPTIONAL_PARAMS);
        ewsCliAddTree(depth5, depth4);
      }
    }

  }
}

void buildTreeMacAccessListSecondaryCos(EwsCliCommandP depth1, L7_BOOL permitMenu)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_MacAclSecondaryCos, pStrInfo_qos_MacAccessListMatchSecondaryCos, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_ACL_MIN_COS, L7_ACL_MAX_COS);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_qos_CosQueuePara, NULL, L7_NO_OPTIONAL_PARAMS);

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    buildTreeMacAccessListLog(depth3);
  }

  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    buildTreeMacAccessListTimeRange(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListAssignQueue(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth3);
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

void buildTreeMacAccessListLog(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclLogsStr, pStrInfo_qos_MacAccessListLog, NULL, L7_NO_OPTIONAL_PARAMS);
  
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    buildTreeMacAccessListTimeRange(depth2);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListAssignQueue(depth2);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth2);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth2);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
void buildTreeMacAccessListTimeRange(EwsCliCommandP depth1)
{  
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if(unit ==0)
  {
   return;
  }
  /* if feature TIME RANGE Feature present - configure time range*/
  depth2 = ewsCliAddNode(depth1,pStrInfo_qos_AclTimeRangeStr, pStrInfo_qos_MacAccessListTimeRange, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_name, pStrInfo_qos_TimeRangePara, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListAssignQueue(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth3);
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

void buildTreeMacAccessListAssignQueue(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  osapiSnprintf(buf, sizeof(buf), "<%d-%d> ", L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclAsSignQueueStr, pStrInfo_qos_MacAccessListAsSignQueue, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, buf, pStrInfo_qos_CosQueuePara, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListMirror(depth3);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeMacAccessListRedirect(depth3);
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

void buildTreeMacAccessListMirror(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* Note: mirror and redirect attributes are mutually-exclusive */

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclMirrorStr, pStrInfo_qos_MacAccessListMirror, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

void buildTreeMacAccessListRedirect(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  /* Note: mirror and redirect attributes are mutually-exclusive */

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  To build the IPv6 Access List command tree
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
void buildTreeAccessListIpv6(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_AccessList_1, pStrInfo_qos_Ipv6AclAccessList, cliIpv6AccessListMode,  2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH );
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessListName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Rename_1, pStrInfo_qos_AccessListRename, commandIpAccessListRename, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Oldname, pStrInfo_qos_AccessListRenameOldName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Newname, pStrInfo_qos_AccessListRenameNewName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_TrafficFilter, pStrInfo_qos_CfgAclGrp, commandIpv6TrafficFilter, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Name, pStrInfo_qos_AccessGrpName, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_INTF_SEQ_NUM);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2, L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_VLAN_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_MacAclVlan_1, pStrInfo_qos_CfgAclVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_VlanId, pStrInfo_qos_CfgAclVlan, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_VLAN_SEQ_NUM);
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
      depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
    }
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
    {
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
      depth8 = ewsCliAddNode(depth7, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
      depth9 = ewsCliAddNode(depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
    }
  }
}

/*********************************************************************
*
* @purpose  To build the IPv6 Access List Menu
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
void buildTreeIpv6AccessListConfig()
{
  EwsCliCommandP depth1, depth2, depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  EwsCliCommandP treeDeny, treeEvery, treeMirror = L7_NULLPTR, treeSrcAny, treeDstAny, treeProtocols, treeSrcPortEq, treeDstPortEq;
  L7_uint32 unit;
  L7_BOOL srcIpFeatureSupported, dstIpFeatureSupported;
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  depth1 = ewsCliAddNode(NULL, NULL, NULL, cliIpv6AccessListMode, L7_NO_OPTIONAL_PARAMS);

  cliSetMode(L7_IPV6_ACCESS_LIST_CONFIG_MODE, depth1);

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  treeDeny = ewsCliAddNode(depth1, pStrInfo_common_Deny_2, pStrInfo_qos_AclActionDeny, commandIpv6AccessList, L7_NO_OPTIONAL_PARAMS);
  treeEvery = ewsCliAddNode(treeDeny, pStrInfo_qos_AclEveryStr, pStrInfo_qos_AclActionEvery, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(treeEvery, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if ((usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_DENY_FEATURE_ID) == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_LOG_PERMIT_FEATURE_ID) == L7_TRUE))
  {
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclLogsStr, pStrInfo_qos_MacAccessListLog, NULL, 2,
                           L7_OPTIONS_NODE, treeEvery);
  }
  if ((usmDbComponentPresentCheck(unit,L7_TIMERANGES_COMPONENT_ID) == L7_TRUE ) &&(usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_PER_RULE_ACTIVATION_ID) == L7_TRUE))
  {
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclTimeRangeStr, pStrInfo_qos_MacAccessListTimeRange, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_name, pStrInfo_qos_TimeRangePara, NULL, 2,
                           L7_OPTIONS_NODE, treeEvery);
  }  

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_ASSIGN_QUEUE_FEATURE_ID) == L7_TRUE)
  {
    sprintf(buf, "<%d-%d> ", L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclAsSignQueueStr, pStrInfo_qos_MacAccessListAsSignQueue, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_CosQueuePara, NULL, 2,
                           L7_OPTIONS_NODE, treeEvery);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_MIRROR_FEATURE_ID) == L7_TRUE)
  {
    treeMirror = ewsCliAddNode(treeEvery, pStrInfo_qos_AclMirrorStr, pStrInfo_qos_MacAccessListMirror, NULL, L7_NO_OPTIONAL_PARAMS);
#ifdef L7_STACKING_PACKAGE
    depth5 = ewsCliAddNode (treeMirror, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#else
    depth5 = ewsCliAddNode (treeMirror, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#endif
  }

  /* Note: mirror and redirect attributes are mutually-exclusive */

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_REDIRECT_FEATURE_ID) == L7_TRUE)
  {
    if (treeMirror != L7_NULLPTR)
    {
      depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, 2,
                             L7_SAME_AS_NODE, treeMirror);
    }
    else
    {
      depth4 = ewsCliAddNode(treeEvery, pStrInfo_qos_AclRedirectsStr, pStrInfo_qos_MacAccessListRedirect, NULL, L7_NO_OPTIONAL_PARAMS);
    }
#ifdef L7_STACKING_PACKAGE
    depth5 = ewsCliAddNode (depth4, pStrErr_common_AclIntfsStacking, pStrInfo_common_CfgLogIntfWithUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#else
    depth5 = ewsCliAddNode (depth4, pStrErr_common_AclIntfs, pStrInfo_common_CfgLogIntfWithoutUnit, NULL, 2,
                            L7_OPTIONS_NODE, treeEvery);
#endif
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                               L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE)
  {
    treeProtocols = ewsCliAddNode(treeDeny, pStrInfo_common_Icmpv6_1, pStrInfo_qos_AclMatchIcmpv6Proto, NULL, L7_NO_OPTIONAL_PARAMS);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_common_Diffserv_5, pStrInfo_qos_AclMatchIpv6Proto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_qos_Tcp_1, pStrInfo_qos_AclMatchTcpProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_qos_Udp_1, pStrInfo_qos_AclMatchUdpProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);
    depth3 = ewsCliAddNode(treeDeny, pStrInfo_common_Range1to255, pStrInfo_qos_AclMatchProto, NULL, 2,
                           L7_OPTIONS_NODE, treeProtocols);

    srcIpFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                     L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID);
    dstIpFeatureSupported = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                     L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID);

    if (L7_TRUE == srcIpFeatureSupported)
    {
      treeSrcAny = ewsCliAddNode(treeProtocols, pStrInfo_qos_AclAnyStr, pStrInfo_qos_AccessListMatchAnySrc, NULL, L7_NO_OPTIONAL_PARAMS);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID) == L7_TRUE)
      {
        treeSrcPortEq = ewsCliAddNode(treeSrcAny, pStrInfo_qos_AclEqstr, pStrInfo_qos_AclEqualPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(treeSrcPortEq, pStrInfo_common_Range0to65535, pStrInfo_qos_CfgL4PortNum, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
        depth6 = ewsCliAddNode(treeSrcPortEq, pStrInfo_qos_Portkey, pStrInfo_qos_CfgAclPortKeyword, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
      }
      else
      {
        treeSrcPortEq = treeSrcAny;
      }
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth5 = ewsCliAddNode(treeSrcAny, pStrInfo_common_AclRangeStr, pStrInfo_qos_AclPortRange, NULL, 2,
                               L7_SAME_AS_NODE, treeSrcPortEq);
        depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Startport, pStrInfo_qos_AclSrcStartPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Endport, pStrInfo_qos_AclSrcEndPort, NULL, 2,
                               L7_OPTIONS_NODE, treeSrcAny);
      }
      depth4 = ewsCliAddNode(treeProtocols, pStrInfo_qos_SrcIpv6PrefixPrefixLen_1, pStrInfo_qos_AclCfgAclSrcIpv6, NULL, 2,
                             L7_OPTIONS_NODE, treeSrcAny);
    }
    else
    {
      /* in case source IP matching is NOT supported but destination IP matching IS supported, make sure treeSrcAny is initialized */
      treeSrcAny = treeProtocols;
    }
    if (L7_TRUE == dstIpFeatureSupported)
    {
      treeDstAny = ewsCliAddNode(treeSrcAny, pStrInfo_qos_AclAnyStr, pStrInfo_qos_AccessListMatchAnyDst, NULL, 2,
                                 L7_OPTIONS_NODE, treeEvery);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID) == L7_TRUE)
      {
        treeDstPortEq = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclEqstr, pStrInfo_qos_AclEqualPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth6 = ewsCliAddNode(treeDstPortEq, pStrInfo_common_Range0to65535, pStrInfo_qos_CfgL4PortNum, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
        depth6 = ewsCliAddNode(treeDstPortEq, pStrInfo_qos_Portkey, pStrInfo_qos_CfgAclPortKeyword, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
      }
      else
      {
        treeDstPortEq = treeDstAny;
      }
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID) == L7_TRUE)
      {
        depth5 = ewsCliAddNode(treeDstAny, pStrInfo_common_AclRangeStr, pStrInfo_qos_AclPortRange, NULL, 2,
                               L7_SAME_AS_NODE, treeDstPortEq);
        depth6 = ewsCliAddNode(depth5, pStrInfo_qos_Startport, pStrInfo_qos_AclDestStartPort, NULL, L7_NO_OPTIONAL_PARAMS);
        depth7 = ewsCliAddNode(depth6, pStrInfo_qos_Endport, pStrInfo_qos_AclDestEndPort, NULL, 2,
                               L7_OPTIONS_NODE, treeDstAny);
      }
      depth5 = ewsCliAddNode(treeSrcAny, pStrInfo_qos_DstIpv6PrefixPrefixLen, pStrInfo_qos_AclCfgAclDstIpv6, NULL, 2,
                             L7_OPTIONS_NODE, treeDstAny);
    }
    else
    {
      treeDstAny = treeSrcAny;
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID) == L7_TRUE)
    {
      depth6 = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclDscpStr, pStrInfo_qos_AclMatchIpDscp, NULL, L7_NO_OPTIONAL_PARAMS);
      osapiStrncpySafe(buf, cliTreeAccessListDscpNodeHelp(), sizeof(buf));
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Val, buf, NULL, 2,
                             L7_OPTIONS_NODE, treeEvery);

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID) == L7_TRUE)
      {
        depth8 = ewsCliAddNode(depth7, pStrInfo_qos_AclFlowLabelsStr, pStrInfo_qos_AclMatchIpFlowLabel, NULL, L7_NO_OPTIONAL_PARAMS);
        depth9 = ewsCliAddNode(depth8, pStrInfo_common_Val, pStrInfo_qos_AclMatchIpFlowLabelVal, NULL, 2,
                               L7_OPTIONS_NODE, treeEvery);
      }
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                 L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID) == L7_TRUE)
    {
      depth6 = ewsCliAddNode(treeDstAny, pStrInfo_qos_AclFlowLabelsStr, pStrInfo_qos_AclMatchIpFlowLabel, NULL, L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Val, pStrInfo_qos_AclMatchIpFlowLabelVal, NULL, 2,
                             L7_OPTIONS_NODE, treeEvery);
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID,
                                   L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID) == L7_TRUE)
      {
        depth8 = ewsCliAddNode(depth7, pStrInfo_qos_AclDscpStr, pStrInfo_qos_AclMatchIpDscp, NULL, L7_NO_OPTIONAL_PARAMS);
        osapiStrncpySafe(buf, cliTreeAccessListDscpNodeHelp(), sizeof(buf));
        depth9 = ewsCliAddNode(depth8, pStrInfo_common_Val, buf, NULL, 2,
                               L7_OPTIONS_NODE, treeEvery);
      }
    }
  }
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Exit, pStrInfo_common_ToExitMode, cliIpv6AccessListMode, L7_NO_OPTIONAL_PARAMS );
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Permit_1, pStrInfo_qos_AclActionPermit, commandIpv6AccessList, 2,
                         L7_OPTIONS_NODE, treeDeny);
}

/*********************************************************************
*
* @purpose
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
void buildTreeInterfaceIpv6TrafficFilter(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_TrafficFilter, pStrInfo_qos_Ipv6TrafficFilter, commandIpv6TrafficFilter, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH );
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_qos_AccessGrpName, NULL, 2, L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_qos_1_3, (unsigned long) L7_ACL_MAX_INTF_SEQ_NUM);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclInStr, pStrInfo_qos_CfgAclIntfDirectionIn, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH );
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_AclOutStr, pStrInfo_qos_CfgAclIntfDirectionOut, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,  L7_NO_COMMAND_SUPPORTED,  L7_STATUS_BOTH );
    depth5 = ewsCliAddNode(depth4, buf, pStrInfo_qos_AccessListsSeq, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS );
  }
}

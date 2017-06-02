/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/clicommands_cos.c
 *
 * @purpose create acl commands
 *
 * @component user interface
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  04/13/04
 *
 * @author kmans
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "clicommands_cos.h"
#include "clicommands_card.h"
#include "dot1q_api.h"
#include "l7_cos_api.h"
#include "usmdb_util_api.h"
#include "cli_web_exports.h"
#include "cos_exports.h"

/*********************************************************************
*
* @purpose  Build the QOS Queue command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'cos-queue' command tree
*
* @end
*
*********************************************************************/
void buildTreePrivSwInfoShowInterfacesCosQueue(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_CosQueue_1, pStrInfo_qos_ShowCosQueue, commandShowInterfacesCosQueue, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_RandomDetect_2, pStrInfo_qos_ShowIntfsRandomDetect, commandShowInterfacesRandomDetect, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
    {
      depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

}
/*********************************************************************
*
* @purpose  Build the QOS Queue command tree
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @notes Creates the 'cos-queue' command tree
*
* @end
*
*********************************************************************/
void buildTreePrivSwInfoShowInterfacesTailDropThreshold(EwsCliCommandP depth3)
{

  EwsCliCommandP depth4, depth5, depth6;
  L7_uint32 unit;

  unit = cliGetUnitId();

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_TailDropThresh_2, pStrInfo_qos_ShowIntfsTailDropThresh, commandShowInterfacesTailDropThreshold, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) == L7_TRUE)
    {
      depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }
  }

}
/*********************************************************************
*
* @purpose  Build the tree nodes for Global COS QUEUE command
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
void buildTreeGlobalCosQueue(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depthn, depthnplus1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 help[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode (depth1, pStrInfo_qos_CosQueue_1, pStrInfo_qos_CosQueue, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* Maximum Bandwidth */
  buildTreeCosQueueMaxBandwidth (depth2);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE)
  {

    depthn = ewsCliAddNode (depth2, pStrInfo_qos_MinBandwidth_2, pStrInfo_qos_CosQueueMinBw, commandGlobalCosQueueMinBandwidth, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Bw, i);

      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, help, sizeof(help), pStrInfo_qos_CosQueueMinBwPara, i);

      depthnplus1 = ewsCliAddNode(depthn, buf, help, NULL, L7_NO_OPTIONAL_PARAMS);
      depthn = depthnplus1;
    }
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* Random Detect */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeCosQueueRandomDetect(depth2);
  }

  /* Scheduler Type (Strict) */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID) == L7_FALSE)
  {
    osapiSnprintf(help, sizeof(help), pStrInfo_qos_CosQueueStrictPara, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

    depthn = ewsCliAddNode (depth2, pStrInfo_qos_Strict_2, pStrInfo_qos_CosQueueStrict, commandGlobalCosQueueStrict, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_QueueId, help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    for (i = 1; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, help, sizeof(help), pStrInfo_qos_CosQueueStrictParaAdditional, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

      depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_QueueId, help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depthn = depthnplus1;
      depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
  }
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Interface COS QUEUE command
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
void buildTreeInterfaceCosQueue(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depthn, depthnplus1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 help[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode (depth1, pStrInfo_qos_CosQueue_1, pStrInfo_qos_CosQueue, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* Maximum Bandwidth */
  buildTreeCosQueueMaxBandwidth (depth2);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MIN_BW_FEATURE_ID) == L7_TRUE)
  {

    depthn = ewsCliAddNode (depth2, pStrInfo_qos_MinBandwidth_2, pStrInfo_qos_CosQueueMinBw, commandInterfaceCosQueueMinBandwidth, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Bw, i);

      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, help, sizeof(help), pStrInfo_qos_CosQueueMinBwPara, i);

      depthnplus1 = ewsCliAddNode(depthn, buf, help, NULL, L7_NO_OPTIONAL_PARAMS);
      depthn = depthnplus1;
    }
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  /* Random Detect */
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_FALSE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID) == L7_TRUE)
  {
    buildTreeCosQueueRandomDetect(depth2);
  }

  /* Scheduler Type (Strict) */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID) == L7_FALSE)
  {
    osapiSnprintf(help, sizeof(help), pStrInfo_qos_CosQueueStrictPara, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

    depthn = ewsCliAddNode (depth2, pStrInfo_qos_Strict_2, pStrInfo_qos_CosQueueStrict, commandInterfaceCosQueueStrict, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_QueueId, help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    for (i = 1; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, help, sizeof(help), pStrInfo_qos_CosQueueStrictParaAdditional, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

      depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_QueueId, help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depthn = depthnplus1;
      depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
  }
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Maximum Bandwidth (Interface and Global Config)
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
void buildTreeCosQueueMaxBandwidth(EwsCliCommandP depth2)
{
  EwsCliCommandP depthn, depthnplus1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 help[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i, unit;

  unit = cliGetUnitId();
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MAX_BW_FEATURE_ID) == L7_TRUE)
  {
    depthn = ewsCliAddNode (depth2, pStrInfo_qos_MaxBandwidth_2, pStrInfo_qos_CosQueueMaxBw, commandCosQueueMaxBandwidth, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    for (i = 0; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_Bw, i);

      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, help, sizeof(help), pStrInfo_qos_CosQueueMaxBwPara, i);

      depthnplus1 = ewsCliAddNode(depthn, buf, help, NULL, L7_NO_OPTIONAL_PARAMS);
      depthn = depthnplus1;
    }
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Random Detect (Interface and Global Config)
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

void buildTreeCosQueueRandomDetect(EwsCliCommandP depth2)
{
  EwsCliCommandP depthn, depthnplus1;
  L7_char8 help[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i, unit;

  unit = cliGetUnitId();
  osapiSnprintf(help, sizeof(help), pStrInfo_qos_CosQueueStrictPara, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

  depthn = ewsCliAddNode (depth2, pStrInfo_qos_RandomDetect_2, pStrInfo_qos_CosQueueQueueParms, commandCosQueueRandomDetect, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_QueueId, help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depthn = depthnplus1;
  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  for (i = 1; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, help, sizeof(help), pStrInfo_qos_CosQueueStrictParaAdditional, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);

    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_QueueId, help, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depthn = depthnplus1;
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

}

/*********************************************************************
*
* @purpose  Build the tree nodes for Interface Traffic Shape Command
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
void buildTreeInterfaceTrafficShape(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  depth2 = ewsCliAddNode (depth1, pStrInfo_qos_TrafficShape_2, pStrInfo_qos_TrafficShape, commandInterfaceTrafficShape, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if(L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_TrafficShapeParaKbps, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX);
  }
  else
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_TrafficShapePara, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Bw_1, buf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Global Traffic Shape Command
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
void buildTreeGlobalTrafficShape(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  depth2 = ewsCliAddNode (depth1, pStrInfo_qos_TrafficShape_2, pStrInfo_qos_TrafficShape, commandGlobalTrafficShape, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if(L7_QOS_COS_INTF_SHAPING_RATE_UNITS == L7_RATE_UNIT_KBPS)
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_TrafficShapeParaKbps, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN, L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX);
  }
  else
  {
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_TrafficShapePara, L7_QOS_COS_INTF_SHAPING_RATE_MIN, L7_QOS_COS_INTF_SHAPING_RATE_MAX, L7_QOS_COS_INTF_SHAPING_STEP_SIZE);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_Bw_1, buf, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the tree nodes for Global Traffic Shape Command
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
void buildTreeTailDrop(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depthnCont, depthnplus1Cont;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_TailDrop_1, pStrInfo_qos_TailDropQueueParms, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* Tail Drop Queue Parms */

  osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_CosQueueStrictPara, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_QueueParms, pStrInfo_qos_TailDropThresh, commandTailDropQueueParms, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_QueueId, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_qos_Thresh_2, pStrInfo_qos_TailDropThresh, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeTailDropQueueParmsContinued(depth5);

  depthnCont = depth4;
  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_CosQueueStrictParaAdditional, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
  for (i = 1; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
  {
    depthnplus1Cont = ewsCliAddNode(depthnCont, pStrInfo_qos_QueueId, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    (void) ewsCliAddNode(depthnCont, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
    depthnCont = depthnplus1Cont;
    depthnplus1Cont = ewsCliAddNode(depthnCont, pStrInfo_qos_Thresh_2, pStrInfo_qos_TailDropThresh, NULL, L7_NO_OPTIONAL_PARAMS);
    ewsCliAddTree(depthnplus1Cont, depth5);   /* buildTreeTailDropQueueParmsContinued(depthnplus1Cont); */
  }
}

void buildTreeCosIPDscpMapping(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  osapiSnprintf(buf, sizeof(buf), dscpHelp, L7_QOS_COS_MAP_IPDSCP_MIN, L7_QOS_COS_MAP_IPDSCP_MAX);

  depth3 = ewsCliAddNode(depth2, pStrInfo_qos_IpDscpMapping, pStrInfo_qos_IpDscpMapPing, commandClassofserviceIpDscpMapping, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Ipdscp, buf, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_CosIntRangeParam, L7_QOS_COS_MAP_TRAFFIC_CLASS_MIN, L7_QOS_COS_MAP_TRAFFIC_CLASS_MAX);

  depth5 = ewsCliAddNode (depth4, buf, pStrInfo_qos_CosTrafficClassPara, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

void buildTreeCosIPPrecedenceMapping(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_CosIntRangeParam, L7_QOS_COS_MAP_IPPREC_MIN, L7_QOS_COS_MAP_IPPREC_MAX);

  depth3 = ewsCliAddNode (depth2, pStrInfo_qos_IpPrecedenceMapping, pStrInfo_qos_IpPrecedenceMapPing, commandClassofserviceIpPrecedenceMapping, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode (depth3, buf, pStrInfo_qos_IpPrecedenceMapPingPara, NULL, L7_NO_OPTIONAL_PARAMS);

  osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_CosIntRangeParam, L7_QOS_COS_MAP_TRAFFIC_CLASS_MIN, L7_QOS_COS_MAP_TRAFFIC_CLASS_MAX);

  depth5 = ewsCliAddNode (depth4, buf, pStrInfo_qos_CosTrafficClassPara, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

void buildTreeInterfaceCosTrust(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Trust, pStrInfo_qos_CosTrust, commandClassofserviceTrust, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE &&
      usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustDot1p, pStrInfo_qos_CosTrustDot1p, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustIpDscp, pStrInfo_qos_CosTrustIpDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustIpPrecedence, pStrInfo_qos_CosTrustIpPrecedence, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustUntrusted, pStrInfo_qos_CosTrustUntrusted, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

void buildTreeGlobalCosTrust(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth3 = ewsCliAddNode (depth2, pStrInfo_common_Trust, pStrInfo_qos_CosTrust, commandClassofserviceTrust, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustDot1p, pStrInfo_qos_CosTrustGlobalDot1p, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustIpDscp, pStrInfo_qos_CosTrustGlobalIpDscp, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_FEATURE_ID) == L7_TRUE)
  {
    depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustIpPrecedence, pStrInfo_qos_CosTrustGlobalIpPrecedence, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  depth4 = ewsCliAddNode (depth3, pStrErr_qos_CfgCosTrustUntrusted, pStrInfo_qos_CosTrustGlobalUntrusted, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

void buildTreeRandomDetect(EwsCliCommandP depth1, L7_BOOL isGlobalCfg)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depthnCont, depthnplus1Cont;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  L7_uint32 unit;

  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_qos_RandomDetect_2, pStrInfo_qos_RandomDetect, commandRandomDetect, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_qos_RandomDetectIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

  /* Random Detect Exponential Weighting Constant */
  if ((isGlobalCfg == L7_TRUE) ||
      (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID) == L7_FALSE))
  {
    if (isGlobalCfg == L7_TRUE)
    {
      depth3 = ewsCliAddNode (depth2, pStrInfo_qos_ExponentialWeightingConstant, pStrInfo_qos_RandomDetectExponentialWeightingConSta, commandRandomDetectExponentialWeightingConstant, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
    else
    {
      depth3 = ewsCliAddNode (depth2, pStrInfo_qos_ExponentialWeightingConstant, pStrInfo_qos_RandomDetectExponentialWeightingConSta_1, commandRandomDetectExponentialWeightingConstant, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }

    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_RandomDetectExponentialWeightingConSta_2, L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN, L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX);

    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_Exponent, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  if (isGlobalCfg == L7_TRUE || (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID) == L7_TRUE))
  {
    /* Random Detect Queue Parms */
    osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_CosQueueStrictPara, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    depth3 = ewsCliAddNode(depth2, pStrInfo_qos_QueueParms, pStrInfo_qos_RandomDetectQueueParms, commandRandomDetectQueueParms, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_qos_QueueId, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_qos_MinThresh_3, pStrInfo_qos_RandomDetectMinThresh, NULL, L7_NO_OPTIONAL_PARAMS);
    buildTreeRandomDetectQueueParmsContinued(depth5);

    depthnCont = depth4;
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_CosQueueStrictParaAdditional, L7_QOS_COS_QUEUE_ID_MIN, L7_QOS_COS_QUEUE_ID_MAX);
    for (i = 1; i < L7_MAX_CFG_QUEUES_PER_PORT; i++)
    {
      depthnplus1Cont = ewsCliAddNode(depthnCont, pStrInfo_qos_QueueId, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      (void) ewsCliAddNode(depthnCont, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
      depthnCont = depthnplus1Cont;
      depthnplus1Cont = ewsCliAddNode(depthnCont, pStrInfo_qos_MinThresh_3, pStrInfo_qos_RandomDetectMinThresh, NULL, L7_NO_OPTIONAL_PARAMS);
      ewsCliAddTree(depthnplus1Cont, depth5);    /* buildTreeRandomDetectQueueParmsContinued(depth5); */
    }
  }
}

void buildTreeRandomDetectQueueParmsContinued(EwsCliCommandP depthn)
{
  EwsCliCommandP depthnplus1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 j;
  L7_uint32 unit;

  unit = cliGetUnitId();

  for (j = L7_QOS_COS_DROP_PREC_LEVEL_MIN; j <= L7_QOS_COS_DROP_PREC_LEVEL_MAX; j++)
  {
    memset ( buf, 0, sizeof(buf));
    if (L7_QOS_COS_DROP_PREC_LEVEL_MIN == L7_QOS_COS_DROP_PREC_LEVEL_MAX)
    {
      sprintf(buf, pStrInfo_qos_RandomDetectMinThreshPara, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX);
    }
    else
    {
      sprintf(buf, pStrInfo_qos_RandomDetectMinThreshMultiPara, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX, j);
    }
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_Minthresh, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    depthn = depthnplus1;
  }
  sprintf(buf, pStrInfo_qos_RandomDetectMinThreshNonTcp, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX);
  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_Minthresh, buf, NULL, L7_NO_OPTIONAL_PARAMS);
  depthn = depthnplus1;

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID) == L7_TRUE)
  {
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_MaxThresh_2, pStrInfo_qos_RandomDetectMaxThresh, NULL, L7_NO_OPTIONAL_PARAMS);
    depthn = depthnplus1;

    for (j = L7_QOS_COS_DROP_PREC_LEVEL_MIN; j <= L7_QOS_COS_DROP_PREC_LEVEL_MAX; j++)
    {
      memset ( buf, 0, sizeof(buf));
      if (L7_QOS_COS_DROP_PREC_LEVEL_MIN == L7_QOS_COS_DROP_PREC_LEVEL_MAX)
      {
        sprintf(buf, pStrInfo_qos_RandomDetectMaxThreshPara, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX);
      }
      else
      {
        sprintf(buf, pStrInfo_qos_RandomDetectMaxThreshMultiPara, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX, j);
      }
      depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_Maxthresh, buf, NULL, L7_NO_OPTIONAL_PARAMS);
      depthn = depthnplus1;
    }
    sprintf(buf, pStrInfo_qos_RandomDetectMaxThreshNonTcp, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN, L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX);
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_Maxthresh, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    depthn = depthnplus1;
  }

  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_DropProb_1, pStrInfo_qos_RandomDetectDropProbs, NULL, L7_NO_OPTIONAL_PARAMS);
  depthn = depthnplus1;

  for (j = L7_QOS_COS_DROP_PREC_LEVEL_MIN; j <= L7_QOS_COS_DROP_PREC_LEVEL_MAX; j++)
  {
    memset ( buf, 0, sizeof(buf));
    if (L7_QOS_COS_DROP_PREC_LEVEL_MIN == L7_QOS_COS_DROP_PREC_LEVEL_MAX)
    {
      sprintf(buf, pStrInfo_qos_RandomDetectDropProbs_2, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX);
    }
    else
    {
      sprintf(buf, pStrInfo_qos_RandomDetectDropProbs_1, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX, j);
    }
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_DropProb, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    depthn = depthnplus1;
  }
  sprintf(buf, pStrInfo_qos_RandomDetectDropProbs_NonTcp, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN, L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX);
  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_qos_DropProb, buf, NULL, L7_NO_OPTIONAL_PARAMS);
  depthn = depthnplus1;

  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

void buildTreeTailDropQueueParmsContinued(EwsCliCommandP depthn)
{
  EwsCliCommandP depthnplus1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 j;

  for (j = L7_QOS_COS_DROP_PREC_LEVEL_MIN; j <= (L7_QOS_COS_DROP_PREC_LEVEL_MAX+1); j++)
  {
    memset ( buf, 0, sizeof(buf));
    if (L7_QOS_COS_DROP_PREC_LEVEL_MIN == L7_QOS_COS_DROP_PREC_LEVEL_MAX)
    {
      sprintf(buf, pStrInfo_qos_TailDropThreshPara, L7_QOS_COS_QUEUE_TDROP_THRESH_MIN, L7_QOS_COS_QUEUE_TDROP_THRESH_MAX);
    }
    else
    {
      if (j == (L7_QOS_COS_DROP_PREC_LEVEL_MAX+1)) 
      {
        sprintf(buf, pStrInfo_qos_TailDropThreshNonTcp, L7_QOS_COS_QUEUE_TDROP_THRESH_MIN, L7_QOS_COS_QUEUE_TDROP_THRESH_MAX, j);
      }
      else
      {
        sprintf(buf, pStrInfo_qos_TailDropThreshMultiPara, L7_QOS_COS_QUEUE_TDROP_THRESH_MIN, L7_QOS_COS_QUEUE_TDROP_THRESH_MAX, j);
      }
    }
    depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Thresh_1, buf, NULL, L7_NO_OPTIONAL_PARAMS);
    depthn = depthnplus1;
  }

  depthnplus1 = ewsCliAddNode(depthn, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the cos ip-dscp-mapping privileged exec show commands tree
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

void buildTreePrivShowClassOfServiceIPDscpMapping(EwsCliCommandP depth3)
{
  /* depth2 = show */
  L7_uint32 unit;
  EwsCliCommandP depth4, depth5, depth6;

  /* Initializing unit */
  unit = cliGetUnitId();

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IpDscpMapping, pStrInfo_qos_ShowIpDscpMapPing, commandShowClassofServiceIpDscpMapping, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the cos ip-precedence-mapping privileged exec show commands tree
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

void buildTreePrivShowClassOfServiceIPPrecedenceMapping(EwsCliCommandP depth3)
{
  /* depth2 = show */
  L7_uint32 unit;
  EwsCliCommandP depth4, depth5, depth6;

  /* Initializing unit */
  unit = cliGetUnitId();

  depth4 = ewsCliAddNode(depth3, pStrInfo_qos_IpPrecedenceMapping, pStrInfo_qos_ShowIpPrecedenceMapPing, commandShowClassofServiceIpPrecedenceMapping, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the cos trust privileged exec show commands tree
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

void buildTreePrivShowClassOfServiceTrust(EwsCliCommandP depth3)
{
  /* depth2 = show */
  L7_uint32 unit;
  EwsCliCommandP depth4, depth5, depth6;

  /* Initializing unit */
  unit = cliGetUnitId();

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Trust, pStrInfo_qos_ShowClassOfServiceTrust, commandShowClassofServiceTrust, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID) == L7_TRUE)
  {
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

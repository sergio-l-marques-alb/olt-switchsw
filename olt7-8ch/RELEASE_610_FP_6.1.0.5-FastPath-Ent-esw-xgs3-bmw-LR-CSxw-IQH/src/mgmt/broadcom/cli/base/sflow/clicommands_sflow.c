/* Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/common/clicommands_sflow.c
 *
 * @purpose create the commands for sflow
 *
 * @component user interface
 *
 * @comments contains the code to build the root of the tree
 * @comments also contains functions that allow tree navigation
 *
 * @create  10/16/2007
 *
 * @author  Rajesh G
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "clicommands_sflow.h"
#include "sflow_exports.h"
#include "cli_web_exports.h"

/*********************************************************************
*
* @purpose build the tree for sFlow 
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @notes none
*
* @end
*********************************************************************/
void buildTreeGlobalsFlowConfig(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8, depth9;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* command sflow receiver <rcvr_idx> owner <owner_string> ip/ipv6 <ip> port <port> timeout <rcvr_timeout> maxdatagram <size> */

  /* command sflow receiver <rcvr_idx> owner <owner_string> timeout timeout <value> */

  depth3 = ewsCliAddNode(depth2, pStrinfo_common_Sflow_Receiver, pStrInfo_common_CfgSflowRcvr, commandsFlowReceiver, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_Sflow_RcvrOption, pStrInfo_common_CfgSflowRcvrOption, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH); 
  
  depth5 =  ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_Rcvr_Owner, pStrInfo_common_CfgSflowRcvrOwner, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    
  depth6 =  ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);


  depth6 = ewsCliAddNode(depth5, pStrinfo_common_Sflow_Rcvr_OwnerOption, pStrInfo_common_CfgSflowOwnerOption, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrinfo_common_Sflow_Rcvr_Timeout, pStrInfo_common_CfgSflowRcvrTimeout, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CfgSflowRcvrTimeoutOption, L7_SFLOW_MIN_TIMEOUT, L7_SFLOW_MAX_TIMEOUT);
  depth8 =  ewsCliAddNode ( depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth8 = ewsCliAddNode(depth7, pStrinfo_common_Sflow_Rcvr_TimeoutOption, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth9 =  ewsCliAddNode ( depth8, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* command sflow receiver <index> ip <address> */

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_IpOption, pStrInfo_common_CfgSflowRcvrIP, commandsFlowReceiverIP, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 =  ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrinfo_common_Sflow_Rcvr_IP, pStrInfo_common_CfgSflowRcvrIPOption, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 =  ewsCliAddNode (depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* command sflow receiver <index> port <value> */
  depth5 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_Rcvr_Port, pStrInfo_common_CfgSflowRcvrPort, commandsFlowReceiverPort, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 =  ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrinfo_common_Sflow_Rcvr_PortOption, pStrInfo_common_CfgSflowRcvrPortOption, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 =  ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

 /* command sflow receiver <index> maxdatagram <size> */
  depth5 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_Rcvr_MaxDatagram, pStrInfo_common_CfgSflowRcvrMaxDatagram, commandsFlowReceiverMaxData, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
 
  depth6 =  ewsCliAddNode (depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  
  memset(buf, L7_EOS, sizeof(buf));
  
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CfgSflowSizeOption, L7_SFLOW_MIN_DATAGRAM_SIZE, L7_SFLOW_MAX_DATAGRAM_SIZE); 

  depth6 = ewsCliAddNode(depth5, pStrinfo_common_Sflow_SizeOption, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth7 =  ewsCliAddNode ( depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

}

/*********************************************************************
* @purpose Build Interface config commands for sflow
* 
* 
*  
* @param      EwsCliCommandP    depth1 ( sflow ) 
* 
*
* @returntype void  
*
* @notes
*
* @end

*
**********************************************************************/
void buildTreeInterfacesFlow( EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  L7_uint32 unit;  
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  unit = cliGetUnitId();
  if(unit == 0)
  {
    return;
  }
  depth2 = ewsCliAddNode(depth1, pStrinfo_common_SflowOption, pStrInfo_common_CfgSflow, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   /* command sflow poller <rcvr_idx> */
  depth3 = ewsCliAddNode(depth2, pStrinfo_common_SflowPoller, pStrInfo_common_CfgSflowPoller, commandInterfacesFlowPoller, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_Sflow_RcvrOption, pStrInfo_common_CfgSflowRcvrOption, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth5 =  ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

 /* command sflow poller interval <interval>*/

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_Sflow_CpInterval, pStrInfo_common_CfgSflowCpInterval, commandInterfacesFlowCpInterval, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  memset(buf, L7_EOS, sizeof(buf));
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CfgSflowCpIntervalOption, L7_SFLOW_MIN_POLLING_INTERVAL, L7_SFLOW_MAX_POLLING_INTERVAL);

  depth5 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_CpIntervalOption, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 =  ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);



  /* command sflow sampler <rcvr_idx> */
  

  depth3 = ewsCliAddNode(depth2, pStrinfo_common_SflowSampler, pStrInfo_common_CfgSflowSampler, commandInterfacesFlowSampler, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode ( depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrinfo_common_Sflow_RcvrOption, pStrInfo_common_CfgSflowRcvrOption, NULL, 2, L7_NODE_TYPE_MODE, L7_STATUS_BOTH);
  depth5 =  ewsCliAddNode ( depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* command sflow sampler max_header_size*/

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_Sflow_SpMaxHSize, pStrInfo_common_CfgSflowSpMaxHSize, commandInterfacesFlowSpMaxHSize, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  memset(buf, L7_EOS, sizeof(buf));
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CfgSflowSpSizeOption, L7_SFLOW_MIN_HEADER_SIZE, L7_SFLOW_MAX_HEADER_SIZE);

  depth5 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_SizeOption, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 =  ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  
  /* command sflow sampler rate <rate> */

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_Sflow_SpRate, pStrInfo_common_CfgSflowSpRate, commandInterfacesFlowSpRate, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode (depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  
  memset(buf, L7_EOS, sizeof(buf));
  sprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_CfgSflowSpRateOption, L7_SFLOW_MIN_SAMPLING_RATE, L7_SFLOW_MAX_SAMPLING_RATE);
 
  depth5 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_SpRateOption, buf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 =  ewsCliAddNode ( depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);


}


/*********************************************************************
*
* @purpose build the tree for show sFlow
* @param EwsCliCommandP depth2
*
* @returntype void
*
* @notes none
*
* @end
*********************************************************************/

void buildTreeUserExecSWMgmtShowSflow(EwsCliCommandP depth2)
{
  /* depth2 = Show */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }
  
  depth3 = ewsCliAddNode(depth2, pStrinfo_common_SflowOption, pStrInfo_common_ShowSflow, NULL, L7_NO_OPTIONAL_PARAMS);
  
  /* command show sflow agent */

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_SflowAgent, pStrInfo_common_ShowSflowAgent, commandShowsFlowAgent, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* command show sflow receivers */

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_SflowReceivers, pStrInfo_common_ShowSflowReceivers, commandShowsFlowReceivers, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth4, pStrinfo_common_Sflow_RcvrOption, pStrInfo_common_CfgSflowRcvrOption, 
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
   
 
  /* command show sflow samplers */

  depth4 = ewsCliAddNode(depth3, pStrinfo_common_SflowSamplers, pStrInfo_common_ShowSflowSamplers, commandShowsFlowSamplers, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

 /* command show sflow pollers */
  depth4 = ewsCliAddNode(depth3, pStrinfo_common_SflowPollers, pStrInfo_common_ShowSflowPollers, commandShowsFlowPollers, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}









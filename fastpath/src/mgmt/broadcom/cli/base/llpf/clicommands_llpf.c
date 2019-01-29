/*********************************************************************
 *
 * (C) Copyright Broadcom 2001-2009
 *
 **********************************************************************
 *
 * @filename clicommands_llpf.c
 *
 * @purpose cli commands for LLPF
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/02/2009
 *
 * @author   Vijayanand K (kvijayan)
 *
 * @end
 *
 **********************************************************************/
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "ews.h"
#include "cliutil.h"
#include "clicommands_llpf.h"
#include "compdefs.h"
#include "cli_web_exports.h"
#include "strlib_base_cli.h"

/*****************************************************************************
*
* @purpose build the tree for 'show llpf *'
* @param  depth2 @b{(input)} EwsCliCommand
*
* @returntype  void
*
* @notes none
*
* @end
******************************************************************************/
void buildTreePrivLlpf(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Llpf, 
                 pStrInfo_base_LlpfEnableHelp, commandShowLlpfInterface, L7_NO_OPTIONAL_PARAMS);

  /* Interface */
  depth4 = ewsCliAddNode(depth3, pStrInfo_base_interface,
                         pStrInfo_base_LlpfShowIntf,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  /* All*/
  depth5 = ewsCliAddNode(depth4, pStrInfo_base_all_1,
                         pStrInfo_base_LlpfShowAll,
                         L7_NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  /* Interface <u/s/p>*/
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,
                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine,
                         NULL, L7_NO_OPTIONAL_PARAMS);

}

/******************************************************************************
*
* @purpose build the tree for configure llpf per port
* @param  depth2 @b{(input)} EwsCliCommand
*
* @returntype  void
*
* @notes none
*
* @end
******************************************************************************/
void buildTreeInterfaceConfigLlpf(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Llpf, pStrInfo_base_LlpfEnableHelp, commandIntfLlpfEnable, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfAll,  pStrInfo_base_LlpfIntfBlockAllHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfDtp,  pStrInfo_base_LlpfIntfBlockDtpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfIsdp, pStrInfo_base_LlpfIntfBlockIsdpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfPagp, pStrInfo_base_LlpfIntfBlockPagpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfSstp, pStrInfo_base_LlpfIntfBlockSstpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfUdld, pStrInfo_base_LlpfIntfBlockUdldHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LlpfVtp,  pStrInfo_base_LlpfIntfBlockVtpHelp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

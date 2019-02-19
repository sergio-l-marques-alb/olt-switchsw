/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/dvlan/clicommands_dvlan.c
 *
 * @purpose cli config commands for the dvlan tagging
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  Amit Kulkarni
 *
 * @end
 *
 **********************************************************************/

#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_dvlantag_api.h"
#include "defaultconfig.h"

#include "cliapi.h"
#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"
#include "clicommands_dvlan.h"





/*********************************************************************
*
* @purpose  Build the Switch Device DVlan commands tree
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

void buildTreeInterfaceSwDVlan(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  L7_uint32 unit = cliGetUnitId();

  if ((usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE )
      && ((usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_CUSTID_FEATURE_ID) == L7_TRUE)
          || (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID) == L7_TRUE)))
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_DvlanTunnel, pStrInfo_base_CfgDvlanParams, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_CUSTID_FEATURE_ID) == L7_TRUE)
    {

      depth3 = ewsCliAddNode(depth2, pStrInfo_base_CustomerId_1, pStrInfo_base_CfgDvlanCustomer, commandDVlanCustomerId, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_base_Range0to4095, pStrInfo_base_CfgDvlanCustomerId, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    }

    if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID) == L7_TRUE)
    {
      depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ethertype, pStrInfo_base_CfgDvlanEtherType, commandDVlanIntfEtherType, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_DvlanEtherType802, pStrInfo_base_CfgDvlanEtherType802, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_DvlanEtherTypeCustom_1, pStrInfo_base_CfgDvlanEtherTypeCustom, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_base_CfgDvlanCustom, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

      depth4 = ewsCliAddNode(depth3, pStrInfo_common_DvlanEtherTypeVman_1, pStrInfo_base_CfgDvlanEtherTypeVman, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device DVlan commands tree
*
* @param void
*
* @returntype struct EwsCliCommandP
*
* @notes These are the same functions as invoked in buildTreeInterfaceSwDVlan
*        however the below different versions of the same command must exist
*        as well.
*
* @end
*
*********************************************************************/

void buildTreeInterfaceSwDot1qTunnel(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4;

  L7_uint32 unit = cliGetUnitId();

  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE )
  {
    depth2 = ewsCliAddNode(depth1, pStrInfo_common_ApShowRunningMode, pStrInfo_base_CfgDvlanMode, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_DvlanTunnel, pStrInfo_base_CfgDvlanPortTagging, commandDVlanTagging, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Dot1qTunnel, pStrInfo_base_CfgDot1qPortTagging, commandDVlanTagging, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }

}


/*********************************************************************
*
* @purpose  Build the Switch Device show dvlan commands tree
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

void buildTreeUserSwDevShowDot1qTunnel(EwsCliCommandP depth2)
{
  /* depth2 = "show" */
  EwsCliCommandP depth3, depth4, depth5, depth6;

  L7_uint32 unit = cliGetUnitId();

  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE )
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Dot1qTunnel, pStrInfo_base_ShowDot1q, commandShowDVlanTunnel, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_CfgIntf, commandShowDVlanTunnelInterface, L7_NO_OPTIONAL_PARAMS);
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device show dvlan commands tree
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

void buildTreeUserSwDevShowDVlanTunnel(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  L7_uint32 unit = cliGetUnitId();

  if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_FEATURE_SUPPORTED) == L7_TRUE )
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_DvlanTunnel, pStrInfo_base_ShowDvlan, commandShowDVlanTunnel, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Ipv6DhcpRelayIntf_1, pStrInfo_base_CfgIntf, commandShowDVlanTunnelInterface, L7_NO_OPTIONAL_PARAMS);
    depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

void buildTreeDVlanGlobalConfig(EwsCliCommandP depth1)
{
    EwsCliCommandP depth2,depth3,depth4, depth5, depth6, depth7;
    L7_uint32 unit = cliGetUnitId();

    depth2 = ewsCliAddNode(depth1, pStrInfo_base_DvlanTunnel, pStrInfo_base_CfgDvlanParams, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Ethertype, pStrInfo_base_CfgDvlanEtherType, commandDVlanEtherType, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_DvlanEtherType802, pStrInfo_base_CfgDvlanEtherType802, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) == L7_TRUE) {
        depth5 = ewsCliAddNode(depth4, pStrInfo_common_DefTpid, pStrInfo_base_CfgDvlanDefaultTpid, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);    
    }
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_DvlanEtherTypeCustom_1, pStrInfo_base_CfgDvlanEtherTypeCustom, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_base_CfgDvlanCustom, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    if (usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) == L7_TRUE) {
        depth6 = ewsCliAddNode(depth5, pStrInfo_common_DefTpid, pStrInfo_base_CfgDvlanDefaultTpid, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);    
        depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);        
    }
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_DvlanEtherTypeVman_1, pStrInfo_base_CfgDvlanEtherTypeVman, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    if(usmDbFeaturePresentCheck(unit, L7_DVLANTAG_COMPONENT_ID, L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID) == L7_TRUE)
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_DefTpid, pStrInfo_base_CfgDvlanDefaultTpid, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);    
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);        
    }
}

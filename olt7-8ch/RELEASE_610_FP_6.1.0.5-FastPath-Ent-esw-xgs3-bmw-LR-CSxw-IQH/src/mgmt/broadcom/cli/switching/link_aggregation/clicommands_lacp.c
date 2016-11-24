/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_lacp.c
 *
 * @purpose create the tree for CLI LACP
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  180/10/2005
 *
 * @author  Sunil Babu
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_lacp.h"
#include "dot3ad_exports.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
* @purpose  To Build the LLDP tree in interface configuration mode
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeInterfaceConfigLacp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6, depth7;
  L7_uint32 unit = 0;

  unit = cliGetUnitId();

  /* Building the LACP Command Tree */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lacp_1, pStrInfo_base_Lacp, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /*Begin of lacp actor .... */

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Actor, pStrInfo_base_LacpActor, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LoginUsrId, pStrInfo_base_LacpActorAdmin, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Key_3, pStrInfo_base_LacpActorAdminKey, commandLACPActorAdminKey,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_base_LacpActorAdminKeyInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /*lacp actor admin state {longtimeout|passive|individual} */

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_State_1, pStrInfo_base_LacpActorAdminState, commandLACPActorAdminState,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LacpIn_1, pStrInfo_base_LacpActorIn, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LacpLongTimeoutString, pStrInfo_base_LacpActorLongTimeoutInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Pass_3, pStrInfo_base_LacpActorPass, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /*lacp actor port priority */

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Port_4, pStrInfo_base_LacpActorPortPri, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SwitchPri_1, pStrInfo_base_LacpActorPortPri, commandLACPActorPortPriority,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to255, pStrInfo_base_LacpActorPortPriInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /*lacp actor  system priority */

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_BootSys, pStrInfo_base_LacpActorSysPri, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SwitchPri_1, pStrInfo_base_LacpActorSysPri, commandLACPActorSysPriority,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_base_LacpActorSysPri, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /*End of lacp actor .... */

  /*Begin of lacp admin key <key> */

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_LoginUsrId, pStrInfo_base_LacpAdmin, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Key_3, pStrInfo_base_LacpAdmin, commandLACPAdminKey,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_base_LacpAdminKey, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  /*end of lacp admin key <key> */

  /*Begin of lacp collector max-delay <max-delay> */

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Collector, pStrInfo_base_LacpCollectorMaxDelay, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_MaxDelay, pStrInfo_base_LacpCollectorMaxDelay, commandLACPCollectorMaxDelay,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_base_LacpMaxDelayTime, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /*End of lacp collector max-delay <max-delay> */

  /*Begin of lacp partner .... */

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Partner, pStrInfo_base_LacpPartner, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_LoginUsrId, pStrInfo_base_LacpPartnerAdmin, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Key_3, pStrInfo_base_LacpPartnerAdminKey, commandLACPPartnerAdminKey,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_base_LacpPartnerAdminKeyInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /* llacp partner admin state TBD */

  /*lacp partner admin state {longtimeout|passive|individual} */

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_State_1, pStrInfo_base_LacpPartnerAdminState, commandLACPPartnerAdminState,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LacpIn_1, pStrInfo_base_LacpPartnerIn, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_LacpLongTimeoutString, pStrInfo_base_LacpPartnerLongTimeoutInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Pass_3, pStrInfo_base_LacpPartnerPass, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /*lacp partner port */

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Port_4, pStrInfo_base_LacpPartnerPort, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Id_1, pStrInfo_base_LacpPartnerPortId, commandLACPPartnerPortID,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to65535, pStrInfo_base_LacpPartnerPortIdInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SwitchPri_1, pStrInfo_base_LacpPartnerPortPri, commandLACPPartnerPortPriority,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to255, pStrInfo_base_LacpPartnerPortPriInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /*lacp partner system id */

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_BootSys, pStrInfo_base_LacpPartnerSys, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_base_Id_1, pStrInfo_base_LacpPartnerSysId, commandLACPPartnerSysID,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_base_Sysid, pStrInfo_base_LacpPartnerSysIdInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SwitchPri_1, pStrInfo_base_LacpPartnerSysPri, commandLACPPartnerSysPriority,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Range0to255, pStrInfo_base_LacpPartnerSysPriInput, NULL,
                         2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,
                         L7_NO_OPTIONAL_PARAMS);

  /*End of llacp partner .... */
}

/*********************************************************************
* @purpose  To Build the LACP tree for show commands
*
* @param    EwsCliCommandP depth1		@b((input)) The parent node
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void buildTreeShowConfigLacp(EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* Building the LACP Show Command Tree */

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lacp_1, pStrInfo_base_ShowLacpShow, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Actor, pStrInfo_base_ShowLacpShowActor, commandLACPActorShow, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,commandLACPPartnerShow, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Partner, pStrInfo_base_ShowLacpShowPartner, commandLACPPartnerShow, L7_NO_OPTIONAL_PARAMS);

  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr,  pStrInfo_common_NewLine,commandLACPPartnerShow, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the tree for port-channel  static and load-balance commands
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
void buildTreeInterfaceLAG(EwsCliCommandP depth1)
{
  L7_BOOL   rc;
  L7_uint32 unit;

  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5;

  /* add Static */
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lag_4, pStrInfo_base_Lag_1, NULL, 2,
                         L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Static2, pStrInfo_base_StaticLag, commandPortChannelStaticLAG, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* add Load-Balance*/
  unit = usmDbThisUnitGet();
  rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_INTF_FEATURE_ID);
  if (rc == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_LoadBalance, pStrInfo_base_LoadBalanceLag, commandPortChannelLoadBalanceLAG, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSA, pStrInfo_base_HashModeSA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDA, pStrInfo_base_HashModeDA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSADA, pStrInfo_base_HashModeSADA, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSAEthIntf, pStrInfo_base_HashModeSAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDAEthIntf, pStrInfo_base_HashModeDAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSADAEthIntf, pStrInfo_base_HashModeSADAEthIntf, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSrcIP, pStrInfo_base_HashModeSrcIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeDestIP, pStrInfo_base_HashModeDestIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }
    rc = usmDbFeaturePresentCheck(unit, L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID);
    if (rc == L7_TRUE)
    {
      depth4 = ewsCliAddNode(depth3, pStrInfo_common_HashModeSrcDestIP, pStrInfo_base_HashModeSrcDestIP, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    }

    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device show port-channel command tree
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
void buildTreePrivSwDevShowPortChannel(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Lag_4, pStrInfo_base_ShowLag, commandShowPortChannel, L7_NO_OPTIONAL_PARAMS);
  depth4 = buildTreeLogInterfaceHelp(depth3, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Brief, pStrInfo_base_ShowLagSummary, commandShowPortChannelSummary, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_BootSys, pStrInfo_base_ShowLagSysPri, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_SwitchPri_1, pStrInfo_base_ShowLagSysPri, commandShowPortChannelSysPriority, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}
/*********************************************************************
*
* @purpose  Build the Switch Device portchannel commands tree
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

void buildTreeInterfaceSwDevInterfacePortChannel(EwsCliCommandP depth1)
{
  /* depth1 = "Interface Config" */
  EwsCliCommandP depth2, depth3, depth4;
  L7_uint32 unit;
  unit = cliGetUnitId();

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Addport, pStrInfo_base_IntfAddPort, commandInterfaceAddPort, L7_NO_OPTIONAL_PARAMS);
  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NORMAL_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Adminmode, pStrInfo_base_CfgLagAdminMode, commandInterfaceAdminMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /***Adding the AutoNegiotiate Command***/
  buildTreeInterfaceSwDevDuplex(depth1);
}

/*********************************************************************
*
* @purpose  Build the Switch Device port-channel commands tree
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

void buildTreeGlobalSwDevPortChannel(EwsCliCommandP depth1)
{
  /* depth1 = "Global Config" */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_base_Lag_4, pStrInfo_base_IntfLag, commandInterfacePortChannel, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Name, pStrInfo_base_CfgLagNameString, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = buildTreeLogInterfaceHelp(depth2, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_All, pStrInfo_common_ShowRtrMcastBoundaryAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_Adminmode, pStrInfo_base_CfgLagAdminMode, commandPortChannelAdminMode, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_base_CfgLogIntfAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  buildTreeGlobalSwDevInterfaceLinkTrap(depth2);
  buildTreeGlobalLoadBalance(depth2);
  buildTreeGlobalSwDevInterfaceName(depth2);

  depth3 = ewsCliAddNode(depth2, pStrInfo_base_BootSys, pStrInfo_base_CfgLagsSysPri, commandPortChannelSystemPriority, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_SwitchPri_1, pStrInfo_base_CfgLagsSysPri, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NO_ONLY);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range0to65535, pStrInfo_base_CfgLagsSysPriInput, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);

}


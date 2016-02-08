/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_protectedport.c
*
* @purpose Protected port commands
*
* @component Protected ports
*
* @comments contains the code to build the protected port tree
* @comments gloabl mode and interface mode
*
* @create  03/5/2007
*
* @author  nshrivastav
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "cliapi.h"
#include "clicommands_protectedport.h"
#include "usmdb_protected_port_api.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"

#ifdef L7_STACKING_PACKAGE
#include "clicommands_stacking.h"
#endif

void buildTreeUserProtectedPort(EwsCliCommandP depth2)
{
  EwsCliCommandP depth3,depth4,depth5, depth6;
  L7_char8 protectedPortGroupRange[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }
  if (usmDbComponentPresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID) == L7_TRUE)
  {

    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Switchport,pStrInfo_base_ShowSwitchPort,
                           NULL,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_base_Protected,pStrInfo_base_ShowProtectedPort,
                           commandShowSwitchPortProtected, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                                 L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
    {
      sprintf(protectedPortGroupRange, "<%d-%d> ", 0, L7_PROTECTED_PORT_MAX_GROUPS-1);
      depth5 = ewsCliAddNode(depth4, protectedPortGroupRange,
                             pStrInfo_base_ProtectedPortGrp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                             L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
    else
    {
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                             L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }

  }
}

void buildTreeShowInterfacesSwitchPort(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4,depth5, depth6,depth7;
  L7_char8 protectedPortGroupRange[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth4 = ewsCliAddNode(depth3, pStrInfo_base_Switchport,pStrInfo_base_ShowProtectedPort,
                         commandShowInterfacesSwitchPort, 2,L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION,

                                  L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);

  if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                               L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE)
  {
    sprintf(protectedPortGroupRange, "<%d-%d> ", 0, L7_PROTECTED_PORT_MAX_GROUPS-1);
    depth6 = ewsCliAddNode(depth5, protectedPortGroupRange, pStrInfo_base_ProtectedPortGrp, NULL,
                           2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
  else
  {
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  }
}
/*********************************************************************
*
* @purpose  Build the switchport protected commands tree.
*
*
* @param EwsCliCommandP depth1
*
* @returntype void
*
* @end
*
*********************************************************************/
void buildTreeGlobalSwDevProtectedPort( EwsCliCommandP depth1)
{
  EwsCliCommandP depth2, depth3;
  EwsCliCommandP depth4=L7_NULLPTR,depth5=L7_NULLPTR, depth6=L7_NULLPTR, depth7;
  L7_char8 protectedPortGroupRange[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID) == L7_TRUE)
  {
    /* Switch Port Protected command in global config mode */
    depth2 = ewsCliAddNode(depth1, pStrInfo_base_Switchport, 
						 pStrInfo_base_ProtectedPortParams, NULL, 2, 
						 L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth3 = ewsCliAddNode(depth2, pStrInfo_base_Protected, 
						 pStrInfo_base_CfgProtectedPort, commandSwitchPortProtectedGroupName, 
						 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

    if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                                 L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
    {
      sprintf(protectedPortGroupRange, "<%d-%d> ", 0, L7_PROTECTED_PORT_MAX_GROUPS-1);
      depth4 = ewsCliAddNode(depth3, protectedPortGroupRange, 
							 pStrInfo_base_ProtectedPortGrp, NULL, 2, 
							 L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_ApProfileShowRunningName, 
							 pStrInfo_base_CfgProtectedPortGrpName, NULL, 2, 
							 L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Name, 
							 pStrInfo_common_ApProfileNameString, NULL, 
							 L7_NO_OPTIONAL_PARAMS);
      depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, 
							 NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    }
  }
  
}

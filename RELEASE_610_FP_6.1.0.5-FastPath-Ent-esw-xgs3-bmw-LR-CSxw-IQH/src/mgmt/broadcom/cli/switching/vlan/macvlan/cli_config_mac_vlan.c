/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_config_mac_vlan.c
 *
 * @purpose MAC association to VLAN commands for cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/30/2005
 *
 * @author  rjain
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "ews.h"
#include "clicommands.h"
#include "clicommands_card.h"
#include "clicommands_macvlan.h"
#include "usmdb_vlan_mac_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  MAC association to  VLAN.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  vlan association mac <macaddr> <vlanId >
*             no vlan association mac <macaddr>
*             (vlan database)
*
* @cmdhelp
*
* @cmddescript  Add/Remove MAC association to  VLAN.
*
* @end
*
*********************************************************************/
const L7_char8 *
commandVlanAssociationMac (EwsContext ewsContext, L7_uint32 argc,
                           const L7_char8 * * argv, L7_uint32 index)
{
  L7_int32 vlanId = 0;
  L7_uint32 argVlanId = 2;
  L7_uint32 argMacAddress = 1;
  L7_char8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit= 0, i= 0;
  L7_uint32 numArg= 0;
  L7_enetMacAddr_t tempMac;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop (ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId ();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet ();

  if ((ewsContext->commType == CLI_NORMAL_CMD)&&(numArg != 2))     /* macaddr, vlanId */
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_VlanAssociationMacVlan);
  }
  else
  {
    if((ewsContext->commType == CLI_NO_CMD) && (numArg != 1))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_NoVlanAssociationMacVlan);
    }
  }

  if (strlen(argv[index + argMacAddress]) >= sizeof(buf))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_CfgMacOutOfRange);
  }

  memset(strMacAddr, 0, sizeof (strMacAddr));
  OSAPI_STRNCPY_SAFE(buf, argv[index + argMacAddress]);

  if ( cliConvertMac(buf, strMacAddr) != L7_TRUE )
  {
    return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_UsrInputInvalidClientMacAddr);
  }

  if (cliIsRestrictedFilterMac(strMacAddr) == L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (0, 2, 0, 1, pStrErr_common_UserInput,  ewsContext, pStrErr_common_MacAddr_1);
  }

  for(i = 0; i < L7_ENET_MAC_ADDR_LEN; i++)
  {
    tempMac.addr[i] = strMacAddr[i];
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {

    if (( cliConvertTo32BitUnsignedInteger(argv[index+argVlanId], &vlanId) != L7_SUCCESS) ||
        ( vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
        (vlanId > L7_DOT1Q_MAX_VLAN_ID ))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_InvalidVlanId);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbVlanIDGet(unit, vlanId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_VlanIdNotExisting);
      }
      if ((rc=usmDbVlanMacAdd(unit, tempMac, vlanId)) != L7_SUCCESS)
      {
        if(rc == L7_NOT_SUPPORTED)
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_InvalidSrcMacAddr);
        else
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_FailedToAddMacVlan);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if((usmDbVlanMacGet(unit, tempMac, &vlanId)) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_DuringVlanLookup);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if ((usmDbVlanMacDelete(unit, tempMac, vlanId)) != L7_SUCCESS)
      {
        /* cannot delete a dynamic VLAN without converting it to static */
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_FailedToDelMacVlan);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Build the Switch Device show Mac vlan commands tree
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
void buildTreeUserSwDevShowVlanMacVlan(EwsCliCommandP depth4)
{
  EwsCliCommandP depth5, depth6, depth7;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  if (usmDbComponentPresentCheck(unit, L7_VLAN_MAC_COMPONENT_ID) ==  L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Mac_2, pStrInfo_base_ShowVlanMacAssociationToVlan,
                           commandShowVlanAssociationMac, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Macaddr, pStrInfo_base_ShowVlanMacToVlanRange,
                           NULL, L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the Switch Device vlan commands tree
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
void buildTreeVlanDBSwDevVlanMacVlan(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }


  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);


  if (usmDbComponentPresentCheck(unit, L7_VLAN_MAC_COMPONENT_ID) ==  L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Mac_2, pStrInfo_base_CfgMacAssociationToVlan,
                           commandVlanAssociationMac, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH );
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Macaddr, pStrInfo_base_ShowVlanMacToVlanRange, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, buf, pStrInfo_common_DiffservVlanId, NULL, 2,
                           L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,L7_NO_OPTIONAL_PARAMS);
  }
}

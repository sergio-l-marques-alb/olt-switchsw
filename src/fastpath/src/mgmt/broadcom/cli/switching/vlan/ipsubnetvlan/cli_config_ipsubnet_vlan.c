/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_config_ipsubnet_vlan.c
 *
 * @purpose ip subnet VLAN commands for cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/29/2005
 *
 * @author  rjain
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "clicommands_card.h"
#include "clicommands_ipsubnetvlan.h"
#include "ews.h"
#include "usmdb_vlan_ipsubnet_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose Add VLAN to Ip-Subnet
* @        no form  - used to remove vlan from  from database.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  vlan association subnet <ipaddr> <netmask> <vlanId>
*             no vlan association subnet <ipaddr> <netmask>
*             (in VLAN database)
*
* @cmdhelp
*
* @cmddescript Create/Delete a IP Subnet VLAN.
*
* @end
*
*********************************************************************/
const L7_char8 *
commandVlanAssociationSubnet (EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argIPAddr     = 1;
  L7_uint32 argSubnetMask = 2;
  L7_uint32 argVlanID     = 3;
  L7_int32 vlanId = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIPAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSubnetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit= 0;
  L7_uint32 numArg= 0;
  L7_uint32 IPaddr = 0;
  L7_uint32 subnetMask = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  /* ip-address, subnet mask, VlanId */
  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg != 3))
  {
    memset (buf, 0, sizeof (buf));
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_VlanAssociationSubnet);
  }
  else
  {
    if ((ewsContext->commType == CLI_NO_CMD) && (numArg != 2))
    {
      memset (buf, 0, sizeof (buf));
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_NoVlanAssociationSubnet);
    }
  }

  memset (strIPAddr, 0,sizeof(strIPAddr));
  memset (strSubnetMask, 0,sizeof(strSubnetMask));
  if (strlen(argv[index + argIPAddr]) >= sizeof(strIPAddr) ||
      strlen(argv[index + argSubnetMask]) >= sizeof(strSubnetMask))
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_VlanAssociationSubnet);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_NoVlanAssociationSubnet);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Verify if the specified IP Address is valid */
  OSAPI_STRNCPY_SAFE(strIPAddr, argv[index + argIPAddr]);

  if (usmDbInetAton(strIPAddr, &IPaddr) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgNwIp);
  }

  /* Verify if the specified subnet mask is valid */
  OSAPI_STRNCPY_SAFE(strSubnetMask, argv[index + argSubnetMask]);

  if (usmDbInetAton(strSubnetMask, &subnetMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwMask);
  }
  if (usmDbNetmaskValidate32(subnetMask) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSubnetMask32);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (( cliConvertTo32BitUnsignedInteger(argv[index+argVlanID], &vlanId) != L7_SUCCESS) ||
        ( vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
        (vlanId > L7_DOT1Q_MAX_VLAN_ID ))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_InvalidVlanId);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Adding a VLAN to IP -Subnet */
      rc = usmDbVlanIpSubnetSubnetAdd(unit, IPaddr, subnetMask, vlanId);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_FailedToAssociateIpSubnetVlan);
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* Deleting a VLAN from IP-Subnet */
      if(usmDbVlanIpSubnetSubnetGet(unit, IPaddr, subnetMask, &vlanId) == L7_SUCCESS)
      {
        rc = usmDbVlanIpSubnetSubnetDelete(unit, IPaddr, subnetMask, vlanId);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_FailedToDelIpSubnetVlan);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_switching_FailedToDelIpSubnetVlan);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt (ewsContext);
}
/*********************************************************************
*
* @purpose  Build the Switch Device show IP Subnet vlan commands tree
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
void buildTreeUserSwDevShowVlanIpVlan(EwsCliCommandP depth4)
{
  EwsCliCommandP depth5, depth6, depth7, depth8;
  L7_uint32 unit;

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }


  if (usmDbComponentPresentCheck(unit, L7_VLAN_IPSUBNET_COMPONENT_ID) ==  L7_TRUE)
  {
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Subnet, pStrInfo_base_ShowVlanSubnetAssociationToVlan,
                            commandShowVlanAssociationSubnet, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapModeIpAddr, NULL,
                           L7_NO_OPTIONAL_PARAMS);
    depth7 = ewsCliAddNode(depth6, pStrInfo_base_Netmask, pStrInfo_base_NwParmsNmask, NULL,
                           L7_NO_OPTIONAL_PARAMS);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
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
void buildTreeVlanDBSwDevVlanIpVlan(EwsCliCommandP depth3)
{
  EwsCliCommandP depth4, depth5, depth6, depth7, depth8;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }


  sprintf(buf, "<%d-%d> ", L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);

  if (usmDbComponentPresentCheck(unit, L7_VLAN_IPSUBNET_COMPONENT_ID) ==  L7_TRUE)
  {
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Subnet, pStrInfo_base_CfgSubnetAssociationToVlan,
                           commandVlanAssociationSubnet, 2, L7_NO_COMMAND_SUPPORTED,
                           L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Ipaddr, pStrInfo_base_SnmpTrapModeIpAddr,
                           NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth6 = ewsCliAddNode(depth5, pStrInfo_base_Netmask, pStrInfo_base_NwParmsNmask,
                           NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth7 = ewsCliAddNode(depth6, buf, pStrInfo_common_DiffservVlanId,
                           NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_NORMAL_ONLY);
    depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

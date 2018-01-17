/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_ipsubnet_vlan.c
 *
 * @purpose show commands for the cli
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
#include "ews.h"
#include "usmdb_vlan_ipsubnet_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  display IP Subnet association to VLAN Database
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show vlan association subnet [<ipaddr> <subnet>]
*
* @cmdhelp
*
* @cmddescript  Display information about association between IP Subnet to VLAN
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowVlanAssociationSubnet(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv,
                                                 L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIPAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSubnetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit= 0;
  L7_uint32 numArg= 0;
  L7_uint32 count= 0;
  L7_uint32 argIPAddr = 1;
  L7_uint32 argSubnetMask = 2;
  L7_uint32 IPAddr = 0, subnetMask= 0;
  static L7_uint32 nextIpMask = 0, nextIpVal = 0, nextVid = 0;
  L7_uint32 ipVal = 0, ipMask = 0;
  L7_uint32 vlanId = 0;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArg = cliNumFunctionArgsGet();

  if (numArg > 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowVlanAssociationToVlan_1);
  }

  if((numArg != 0) &&(numArg != 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowVlanAssociationToVlan_1);
  }

  if (numArg == 2)
  {
    memset(strIPAddr, 0, sizeof(strIPAddr));
    memset(strSubnetMask, 0, sizeof(strSubnetMask));
    OSAPI_STRNCPY_SAFE(strIPAddr, argv[index + argIPAddr]);
    OSAPI_STRNCPY_SAFE(strSubnetMask, argv[index + argSubnetMask]);

    if (usmDbInetAton(strIPAddr, &IPAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgNwIp);
    }

    if (usmDbInetAton(strSubnetMask, &subnetMask) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgNwMask);
    }
    if (usmDbNetmaskValidate32(subnetMask) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSubnetMask32);
    }

    rc = usmDbVlanIpSubnetSubnetGet(unit, IPAddr, subnetMask, &vlanId);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_switching_NoIpSubnetVlanAssociation);
    }

    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_IpSubnetIpMaskVlanId);
    ewsTelnetWrite(ewsContext,"\r\n----------------   ----------------   -------");
    cliSyntaxBottom(ewsContext);

    memset(buf, 0, sizeof(buf));
    rc = usmDbInetNtoa(IPAddr, buf);
    if(rc == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat),"%-19s",buf);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),"----------------");
    }
    ewsTelnetWrite(ewsContext, stat);

    memset(buf, 0, sizeof(buf));
    rc = usmDbInetNtoa(subnetMask, buf);
    if(rc == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat),"%-19s",buf);
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),"----------------");
    }
    ewsTelnetWrite(ewsContext, stat);

    return cliSyntaxReturnPrompt (ewsContext," %-7d", vlanId);
  }
  else
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {                                                                     /* if our question has been answered */
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_IpSubnetIpMaskVlanId);
      ewsTelnetWrite(ewsContext,"\r\n----------------   ----------------   -------");
      cliSyntaxBottom(ewsContext);

      rc = usmDbVlanIpSubnetSubnetGetNext (unit, ipVal,ipMask,&nextIpVal, &nextIpMask, &nextVid);
      if (rc != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_switching_NoIpSubnetVlanAssociation);
      }

    }

    for ( count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
    {
      memset(buf, 0, sizeof(buf));
      rc = usmDbInetNtoa(nextIpVal, buf);
      if(rc == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"\r\n%-19s",buf);
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat),"\r\n----------------");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(buf, 0, sizeof(buf));
      rc = usmDbInetNtoa(nextIpMask, buf);
      if(rc == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"%-19s",buf);
      }
      else
      {
        osapiSnprintf(stat,sizeof(stat),"----------------");
      }
      ewsTelnetWrite(ewsContext, stat);

      ewsTelnetPrintf (ewsContext, "%-7d", nextVid);

      ipVal = nextIpVal;
      ipMask = nextIpMask;
      if ((usmDbVlanIpSubnetSubnetGetNext (unit, ipVal,ipMask,&nextIpVal, &nextIpMask, &nextVid)) != L7_SUCCESS)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }

    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliAlternateCommandSet(pStrInfo_switching_ShowVlanAssociationSubnet);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
}

/*********************************************************************
 * @purpose  To print the running configuration of IP Subnet vlan Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningConfigIpSubnetVlanInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 vid;
  L7_uint32 ipVal = 0, nextIpVal = 0;
  L7_uint32 ipMask = 0, nextIpMask = 0;
  L7_char8 strIPAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 netMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t  rc,rc1;

  vid = 0;
  rc = usmDbVlanIpSubnetSubnetGetNext (unit, ipVal,ipMask,&nextIpVal, &nextIpMask, &vid);

  if (rc == L7_SUCCESS)
  {
    do
    {
      memset(strIPAddr, 0, sizeof(strIPAddr));
      memset(netMask, 0, sizeof(netMask));
      rc = usmDbInetNtoa(nextIpVal, strIPAddr);
      rc1 = usmDbInetNtoa(nextIpMask, netMask);
      if (rc == L7_SUCCESS && rc1 == L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_switching_VlanAssociationSubnet_1, strIPAddr, netMask, vid);
        EWSWRITEBUFFER(ewsContext, buf);
      }
      ipVal = nextIpVal;
      ipMask = nextIpMask;
    } while( usmDbVlanIpSubnetSubnetGetNext (unit, ipVal,ipMask,&nextIpVal, &nextIpMask, &vid) == L7_SUCCESS);
  }

  return rc;
}



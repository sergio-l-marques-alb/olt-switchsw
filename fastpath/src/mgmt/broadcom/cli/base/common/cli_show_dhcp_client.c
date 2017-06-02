/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2002-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_show_dhcp_client.c
*
* @purpose DHCP Client show commands for the cli
*
* @component DHCP Client
*
* @comments none
*
* @create  04/11/2008
*
* @author  ddevi
*
* @end
*
*********************************************************************/

#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "strlib_switching_cli.h"
#include "datatypes.h"
#include "usmdb_dhcp_client.h"
#include "cli_web_exports.h"
#include "clicommands_card.h"
#include "usmdb_ip_api.h"

#ifdef L7_ROUTING_PACKAGE
/* Purpose to print the State of DHCP client .
 * This has to be in sync with L7_DHCP_CLIENT_STATE_t.
 */
static L7_uchar8 *dhcpClientState[] =
{
  "...",
  "Init",
  "Selecting",
  "Requesting",
  "Request Receive",
  "Bound",
  "Renewing",
  "Renew Receive",
  "Rebinding",
  "Rebing Receive",
  "BootP Fallback",
  "Not Bound",
  "Failed",
  "Release",
  "Inform Request",
  "Inform Ack Wait",
  "Inform Failed",
  "Inform Bound"
};
#endif /* L7_ROUTING_PACKAGE */

/*********************************************************************
*
* @purpose Displays the DHCP Client Vendor identifier information.
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const char **argv
* @param uintf index
*
* @returntype const char  *
* @returns cliPrompt(ewsContext)
*
* @notes      All options of this show command are included in this function
*
* @cmdsyntax  show dhcp client vendor-id-option
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDhcpClientVendorIdOption(EwsContext ewsContext,
                                                L7_uint32 argc,
                                                const L7_char8 **argv, 
                                                L7_uint32 index)
{
  L7_uint32 gblMode = L7_DISABLE, numArg = 0;
  L7_uchar8 eosStr = L7_EOS;
  L7_uchar8 remoteIdStr[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != L7_NULL)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_switching_DhcpL2RelayNumArgs);
  }

  cliCmdScrollSet( L7_FALSE);


  /* Vendor Identifier admin Mode */
  cliFormat(ewsContext, pStrInfo_base_DhcpClientVendorIdMode);
  if (usmdbDhcpVendorClassOptionAdminModeGet(&gblMode) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, strUtilEnabledDisabledGet(gblMode, 
                                                         pStrInfo_common_NotApplicable));
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrInfo_common_NotApplicable);
  }

  /* Vendor Identifier Option string.*/
  memset(remoteIdStr,0x00,sizeof(remoteIdStr));
  cliFormat(ewsContext, pStrInfo_base_DhcpClientVendorIdStrMode);
  if ((usmdbDhcpVendorClassOptionStringGet(remoteIdStr) == L7_SUCCESS) &&
      (osapiStrncmp(remoteIdStr, &eosStr, sizeof(L7_uchar8)) != L7_NULL))
  {
    ewsTelnetWrite(ewsContext, remoteIdStr);
  }
  else
  {
    ewsTelnetWrite(ewsContext, "Not configured");
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

#ifdef L7_ROUTING_PACKAGE
/*******************************************************
*
* @purpose  To display lease parameters on the interface.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes To show DHCP lease parameters on interfaces 
*
* @cmdsyntax show  dhcp lease [interface <ifIndex>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
********************************************************/
const L7_char8 *commandShowDhcpLease(EwsContext ewsContext,
                                     L7_uint32 argc,
                                     const L7_char8 ** argv,
                                     L7_uint32 index)
{

  L7_RC_t rc = L7_FAILURE;
  L7_uint32 val,string = 1;
  L7_uint32 s, p,numArg,argSlotPort = 2;
  static L7_uint32 intIfNum;
  L7_uint32 nextInterface;
  L7_uint32 unit;
  L7_uint32 id = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_IP_MASK_t mask;
  L7_IP_ADDR_t ipAddr;
  L7_uint32 count = 0;
  L7_DHCP_CLIENT_STATE_t state;
  L7_uint32 ltime = 0;
  L7_uint32 rntime = 0;
  L7_uint32 rtcount = 0;
  L7_uint32 rbtime = 0;
  L7_uint32 srvaddr = 0;
  L7_INTF_IP_ADDR_METHOD_t  method;
  L7_timespec timeSpec;

  /*  validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();
  numArg = cliNumFunctionArgsGet();

  if ((numArg >= 3) || (numArg < 0))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                             pStrErr_routing_ShowDhcpLease);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, 
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  if (strcmp(argv[index+string], pStrInfo_common_Ipv6DhcpRelayIntf_1) == 0)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+argSlotPort],
                                   &unit, &s, &p) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                               pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                               pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      if (cliSlotPortToInterface(argv[index + argSlotPort],
                                 &unit, &s, &p, &intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, 
                                               pStrErr_common_InvalidSlotPort_1);
      }
    }
  }
  else
  {
    if (usmDbValidIntIfNumFirstGet(&intIfNum) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, 
                                             pStrErr_common_NoValidPorts);
    }
  }

  while (intIfNum)
  {
    if (usmDbIpIntfExists(unit, intIfNum) == L7_TRUE) 
    {
      if ((usmDbIpRtrIntfIpAddressMethodGet(intIfNum, &method) == L7_SUCCESS) &&
          (method == L7_INTF_IP_ADDR_METHOD_DHCP) &&
          (usmDbIpDhcpClientIPAddressGet(intIfNum, &ipAddr) == L7_SUCCESS) &&
          (ipAddr != 0))
      {
         /*print the ip address and the ipmask */
         if (count != 0)
         {
           cliSyntaxTop(ewsContext);
         }
         memset (stat, 0, sizeof(stat));

         if (usmDbIpDhcpClientIPAddressGet(intIfNum, &ipAddr) == L7_SUCCESS)
         {
           rc = usmDbInetNtoa(ipAddr, stat);
         }
         ewsTelnetPrintf (ewsContext, pStrInfo_common_InterfaceIpaddr, stat);
         if (usmDbUnitSlotPortGet(intIfNum, &val, &s, &p) == L7_SUCCESS)
         {
           osapiSnprintf(buf, sizeof(buf), cliGetIntfName(intIfNum, val, s, p));
         }
         ewsTelnetPrintf (ewsContext, "%-13s",buf);

         memset (stat, 0, sizeof(stat));

         if (usmDbIpDhcpClientNetworkMaskGet(intIfNum, &mask) == L7_SUCCESS)
         {
           rc = usmDbInetNtoa(mask, stat);
         }
         ewsTelnetPrintf (ewsContext, pStrInfo_common_InterfaceMask, stat);

         memset (stat, 0, sizeof(stat));

         ewsTelnetPrintf (ewsContext, pStrInfo_routing_DhcpServeraddr);
         if (usmDbIpDhcpClientDhcpServerIPAddressGet(intIfNum, &srvaddr) == L7_SUCCESS)
         {
           rc = usmDbInetNtoa(srvaddr, stat);
           ewsTelnetPrintf (ewsContext, "%.15s,", stat);
         }

         ewsTelnetPrintf(ewsContext, pStrInfo_routing_DhcpClientState);
         if (usmDbIpDhcpClientStateGet(intIfNum, &state) == L7_SUCCESS)
         {
           ewsTelnetPrintf(ewsContext, "%d %s", state, dhcpClientState[state]);
         }
         memset (stat, 0, sizeof(stat));

         ewsTelnetPrintf(ewsContext,pStrInfo_routing_DhcpTransactionID);
         if (usmDbIpDhcpClientTransactionIdGet(intIfNum, &id) == L7_SUCCESS)
         {
           ewsTelnetPrintf(ewsContext,"0x%x", id);
         }
         memset (stat, 0, sizeof(stat));

         if (usmDbIpDhcpClientLeaseTimeInfoGet(intIfNum, &ltime, &rntime, &rbtime) 
             == L7_SUCCESS)
         {
           ewsTelnetPrintf(ewsContext, pStrInfo_routing_DhcpClientLeaseTime);
           osapiConvertRawUpTime(ltime,(L7_timespec *)&timeSpec);
           osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DaysHrsMinsSecs, timeSpec.days,
                               timeSpec.hours,timeSpec.minutes,timeSpec.seconds);
           ewsTelnetWrite(ewsContext, stat);
           
           ewsTelnetPrintf(ewsContext, pStrInfo_routing_DhcpClientRenewTime);
           memset (stat, 0, sizeof(stat));
           osapiConvertRawUpTime(rntime,(L7_timespec *)&timeSpec);
           osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DaysHrsMinsSecs, timeSpec.days,
                               timeSpec.hours,timeSpec.minutes,timeSpec.seconds);
           ewsTelnetWrite(ewsContext, stat);

           ewsTelnetPrintf(ewsContext, pStrInfo_routing_DhcpClientRebindTime);
           memset (stat, 0, sizeof(stat));
           osapiConvertRawUpTime(rbtime,(L7_timespec *)&timeSpec);
           osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DaysHrsMinsSecs, timeSpec.days,
                               timeSpec.hours,timeSpec.minutes,timeSpec.seconds);
           ewsTelnetWrite(ewsContext, stat);
           
         }

         ewsTelnetPrintf (ewsContext, pStrInfo_routing_DhcpRetryCount);
         if (usmDbIpDhcpClientRetryCountGet(intIfNum, &rtcount) == L7_SUCCESS)
         {
           ewsTelnetPrintf (ewsContext, "%d ", rtcount);
         }

         cliSyntaxBottom(ewsContext);
         count++;
      }
    }

    if (strcmp(argv[index+string], pStrInfo_common_Ipv6DhcpRelayIntf_1) == 0) 
    {
      if (method != L7_INTF_IP_ADDR_METHOD_DHCP)
      {
        ewsTelnetPrintf (ewsContext, pStrErr_routing_Dhcp_notEnabled);
      }
      break;
    }

    /* Get the next interface */
    if ((rc = usmDbValidIntIfNumNext(intIfNum, &nextInterface)) == L7_SUCCESS)
    {
      intIfNum = nextInterface;
      if (count >= (CLI_MAX_SCROLL_LINES - 6))
      {
        break;
      }
    }
    else
    {
      intIfNum = 0;
    }     
  }     /* end of while loop */

  return cliPrompt(ewsContext);
}
#endif /* L7_ROUTING_PACKAGE */


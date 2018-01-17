/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/cli_config_tunnel.c
 *
 * @purpose cli configuration of IPV6 Tunneling.
 *
 * @component user interface
 *
 *
 * @create  07/14/2005
 *
 * @author  Ravi Saladi
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "defaultconfig.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"

#include "clicommands_ipv6.h"
#include "usmdb_rlim_api.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"
/* IPV6 Tunneling Function declarations */

/*********************************************************************
*
* @purpose This functions configure the tunnel Source with address or
*          interface.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command: tunnel source { <ipv4-address> |
*              <interface-type> {<unit/slot/port> | <slot/port>}}
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIPV6TunnelSource(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intfNum, s, p;
  L7_uint32 unit, lUnit;
  L7_uint32 lIntfNum, lSlot, lPort;
  L7_uint32 tunnelID;
  L7_uint32 argIPaddr = 1;
  L7_uint32 argInfNum = 2;
  L7_uchar8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_in6_addr_t ip6Addr;
  L7_IP_ADDR_t ipAddr;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArgs=0;

  memset(stat, 0, sizeof(stat));
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  tunnelID = EWSTUNNELID(ewsContext);
  if(cliGetUnitSlotPortIntf(ewsContext, tunnelID, &intfNum, &unit, &s,
                            &p, L7_TUNNEL_INTF) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  /*******Check  whether normal or no command Executed*************/
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {

      if(argc == 3)
      {
        /* check if valid IP addrs */
        if (strlen(argv[index+argIPaddr]) > sizeof(strIPaddr))
        {

#ifdef L7_STACKING_PACKAGE
          osapiSnprintfAddBlanks(1, 0, 0, 1, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                                 pStrErr_routing_CfgTunnelSrc, pStrErr_common_AclIntfsStacking);
#else
          osapiSnprintfAddBlanks(1, 0, 0, 1, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                                 pStrErr_routing_CfgTunnelSrc, pStrErr_common_AclIntfs);
#endif
          return cliSyntaxReturnPrompt (ewsContext, stat);
        }
        OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);

        /* This check is required if we support IPV6 Address in future release */
        if(osapiInetPton(L7_AF_INET6, strIPaddr, (L7_uchar8 *)&ip6Addr)
           != L7_SUCCESS)
        {
          if(usmDbInetAton(strIPaddr, &ipAddr) == L7_SUCCESS)
          {
            if(usmDbNetworkAddressValidate(ipAddr) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
            }

            if (usmDbRlimTunnelLocalIp4AddrSet(unit, intfNum, ipAddr) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_TunnelSourceAddrSetFailed);
            }
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
          }
        }
        else
        {
#ifdef L7_STACKING_PACKAGE
          osapiSnprintfAddBlanks(1, 0, 0, 1, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                                 pStrErr_routing_CfgTunnelSrc, pStrErr_common_AclIntfsStacking);
#else
          osapiSnprintfAddBlanks(1, 0, 0, 1, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                                 pStrErr_routing_CfgTunnelSrc, pStrErr_common_AclIntfs);
#endif
          return cliSyntaxReturnPrompt (ewsContext, stat);
        }
      }
      else if(argc == 4)
      {
        /* Getting the user given unit, slot and port */
        if (cliIsStackingSupported() == L7_TRUE)
        {
          /* Get interface and check its validity */
          if ((cliValidSpecificUSPCheck(argv[index+argInfNum], &lUnit, &lSlot, &lPort) != L7_SUCCESS) ||
              (usmDbIntIfNumFromUSPGet(lUnit, lSlot, lPort, &lIntfNum) != L7_SUCCESS))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
          }
        }
        else
        {
          if (cliSlotPortToInterface(argv[index + argInfNum], &lUnit, &lSlot,
                                     &lPort, &lIntfNum) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
          }
        }

        if (usmDbRlimTunnelLocalIntfSet(unit, intfNum, lIntfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_FailureSetIntf);
        }
      }
      else
      {
#ifdef L7_STACKING_PACKAGE
        osapiSnprintfAddBlanks(1, 0, 0, 1, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                               pStrErr_routing_CfgTunnelSrc, pStrErr_common_AclIntfsStacking);
#else
        osapiSnprintfAddBlanks(1, 0, 0, 1, pStrErr_common_IncorrectInput, stat, sizeof(stat),
                               pStrErr_routing_CfgTunnelSrc, pStrErr_common_AclIntfs);
#endif
        return cliSyntaxReturnPrompt (ewsContext, stat);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgTunnelSrcNo);
    }
    else
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(usmDbRlimTunnelLocalAddrClear(unit, intfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_FailureDelIntf);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command: tunnel destination {<ip-address>}
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandTunnelDestination(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  /* L7_uint32  rc;*/
  L7_uint32 intfNum, s, p;
  L7_uint32 unit;
  L7_uint32 tunnelID;
  L7_uint32 argIPaddr = 1;
  L7_uchar8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_in6_addr_t ip6Addr;
  L7_IP_ADDR_t ipAddr;
  L7_uint32 numArgs=0;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArgs = cliNumFunctionArgsGet();
  tunnelID = EWSTUNNELID(ewsContext);
  if(cliGetUnitSlotPortIntf(ewsContext, tunnelID, &intfNum, &unit, &s, &p,
                            L7_TUNNEL_INTF) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(argc == 3)
      {
        /* check if valid IP addrs */
        if (strlen(argv[index+argIPaddr]) > sizeof(strIPaddr))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
        }
        OSAPI_STRNCPY_SAFE(strIPaddr, argv[index + argIPaddr]);

        /* This check is required if we support IPV6 Address in future release */
        if(osapiInetPton(L7_AF_INET6, strIPaddr, (L7_uchar8 *)&ip6Addr)
           != L7_SUCCESS)
        {
          if(usmDbInetAton(strIPaddr, &ipAddr) == L7_SUCCESS)
          {
            if(usmDbNetworkAddressValidate(ipAddr) != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
            }

            if(usmDbRlimTunnelRemoteIp4AddrSet(unit, intfNum, ipAddr)
               != L7_SUCCESS)
            {
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_TunnelDestAddrSetFailed);
            }
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgTunnelDst);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if(numArgs != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgTunnelSrcNo);
    }
    else
    {
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if(usmDbRlimTunnelRemoteAddrClear(unit, intfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_FailureDelIntf);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command: tunnel mode ipv6ip [6to4]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandTunnelModeIpv6Ip(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  /* L7_uint32  rc; */
  L7_uint32 intfNum, s, p;
  L7_uint32 unit;
  L7_uint32 argTemp = 1;
  L7_uint32 tunnelID;
  L7_uint32 tunnelMode, ret = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  /* get switch ID based on presence/absence of STACKING package */
  tunnelID = EWSTUNNELID(ewsContext);
  if(cliGetUnitSlotPortIntf(ewsContext, tunnelID, &intfNum, &unit, &s, &p,
                            L7_TUNNEL_INTF) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      /* To support future release other nodes */
      if(strcmp(argv[index+argTemp], pStrInfo_routing_Ipv6IpMode_1) == 0)
      {
        tunnelMode = L7_TUNNEL_MODE_6OVER4;
        if(argc == 4)
        {
          if(strcmp(argv[index+argTemp+1], pStrInfo_routing_Ipv6Ip6To4) == 0)
          {
            tunnelMode = L7_TUNNEL_MODE_6TO4;
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgTunnelMode);
          }
        }

        ret = usmDbRlimTunnelModeSet(unit, intfNum, tunnelMode);
        if(ret == L7_NOT_SUPPORTED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_Ospfv3NotAppliedOn6To4Tunnel);
        }
        else if(ret == L7_ALREADY_CONFIGURED)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_6To4_NumberCheck);
        }
        else if(ret != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_TunnelModeSetFailed);
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgTunnelMode);
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(usmDbRlimTunnelModeSet(unit, intfNum, L7_TUNNEL_MODE_UNDEFINED)
         != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot,  ewsContext, pStrErr_routing_TunnelModeSetFailed);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

#endif

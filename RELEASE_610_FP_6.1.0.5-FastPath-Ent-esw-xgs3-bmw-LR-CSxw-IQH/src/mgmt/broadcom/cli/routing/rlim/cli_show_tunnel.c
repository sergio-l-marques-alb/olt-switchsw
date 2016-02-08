/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/cli_show_tunnel.c
 *
 * @purpose Tunnel show commands implementation
 *
 * @component user interface
 *
 *
 * @create  07/14/2005
 *
 * @author  Ravi Saladi
 *
 *
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "defaultconfig.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"

#include "clicommands_ipv6.h"
#include "usmdb_ip6_api.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "usmdb_rlim_api.h"
#include "usmdb_util_api.h"

/* IPV6 Tunneling Function declarations */
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
* @Mode  Global Config
*
* @cmdsyntax for normal command: show interface tunnel
*                  show interface tunnel <tunnel-id>
*
* @cmdhelp
*
* @cmddescript  check for valid IP addr and valid subnet mask.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowTunnelInfo(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argInterface = 1;
  L7_uint32 numArgs;
  L7_uint32 interface, slot, port;
  L7_uint32 nextInterface, val;
  L7_uint32 unit;
  L7_uint32 numAddr;
  L7_uint32 i;
  L7_uint32 tunnelId, nextTunnelId;
  L7_uint32 mtu;
  L7_TUNNEL_MODE_t tunnelMode;
  rlimAddrType_t addrType;
  L7_in6_addr_t ipv6Addr;
  L7_IP_ADDR_t ipv4Addr;
  L7_uchar8 mIpv6addr[40];
  L7_ipv6IntfAddr_t ipAddrList[L7_RTR6_MAX_INTF_ADDRS];

  memset(stat, 0, sizeof(stat));
  memset(buf, 0, sizeof(buf));
  memset(mIpv6addr,0, sizeof(mIpv6addr));
  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if (numArgs > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowTunnel);
  }
  else if (numArgs == 1)
  {
    if(cliValidateTunnelId(ewsContext, (L7_char8 *)argv[index+argInterface],
                           &tunnelId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidTunnel);
    }

    /* GET INTERFACE */
    if(usmDbRlimTunnelIntIfNumGet(tunnelId, &interface) == L7_SUCCESS)
    {
      if(usmDbUnitSlotPortGet(interface, &unit, &slot, &port) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_TunnelNotFound);
    }

    if (cliSlotPortCpuCheck(slot, port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* print the link status */
    cliFormat(ewsContext, pStrInfo_routing_IntfLinkStatus);
    usmDbIfOperStatusGet(unit, interface, &val);
    switch (val)
    {
    case L7_DOWN:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
      break;
    case L7_UP:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Up);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
    }
    ewsTelnetWrite(ewsContext,stat);

    numAddr = L7_RTR6_MAX_INTF_ADDRS;
    if(usmDbIp6RtrIntfAddressesGet( interface, &numAddr, ipAddrList)
       == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "\n%s", pStrInfo_common_Ipv6Enbld);
      if(numAddr > 0)
      {
        ipv6Addr = ipAddrList[0].ip6Addr;
        if(memcmp(&ipv6Addr, " ", sizeof(L7_in6_addr_t)) != 0)
        {
          cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Ipv6Addres);
          osapiInetNtop(L7_AF_INET6, (char *)&ipv6Addr, mIpv6addr,
                        sizeof(mIpv6addr));
          osapiSnprintf(stat, sizeof(stat),  "%s/%u", mIpv6addr,
                        ipAddrList[0].ip6PrefixLen);
          if(ipAddrList[0].ip6AddrState != L7_IPV6_INTFADDR_ACTIVE)
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, stat,pStrInfo_common_Tent);
          }
          ewsTelnetWrite(ewsContext, stat);
        }

        for(i = 1; i <=numAddr - 1; i++)
        {
          cliSyntaxBottom(ewsContext);
          ipv6Addr = ipAddrList[i].ip6Addr;
          if(memcmp(&ipv6Addr, " ", sizeof(L7_in6_addr_t)) != 0)
          {
            osapiInetNtop(L7_AF_INET6, (char *)&ipv6Addr, mIpv6addr,
                          sizeof(mIpv6addr)),
            osapiSnprintf(stat, sizeof(stat), "%48s", " ");
            osapiSnprintf(buf, sizeof(buf),  "%s%s/%u", stat, mIpv6addr,
                          ipAddrList[i].ip6PrefixLen);
            if(ipAddrList[i].ip6AddrState != L7_IPV6_INTFADDR_ACTIVE)
            {
              OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, buf,pStrInfo_common_Tent);
            }
            ewsTelnetWrite(ewsContext, buf);
          }
        }
      }
      else
      {
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_Ipv6Addres);
        ewsTelnetWrite(ewsContext, pStrErr_common_Err);
      }
    }

    /* print the MTU Size */
    cliFormat(ewsContext, pStrInfo_routing_MtuSize);
    if(usmDbIfMtuSizeGet(interface, &mtu) != L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf),  pStrInfo_common_UnSpecified);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%d %s",mtu, pStrInfo_common_Bytes);
    }
    ewsTelnetWrite(ewsContext, buf);

    if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_FRAMES, interface, &val)
        == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_PktsRcvdWithout);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_RX_TOTAL_ERROR_FRAMES, interface, &val)
        == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_PktsRcvdWith);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_RX_BCAST_FRAMES, interface, &val)
        == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_BcastPktsRcvd);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_FRAMES, interface, &val)
        == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_PktsTxedWithoutErrs);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_ERROR_FRAMES, interface,
                     &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrErr_common_TxPktErrs);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    if (usmDbStatGet(unit, L7_CTR_TX_TOTAL_COLLISION_FRAMES, interface,
                     &val) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_common_CollisionFrames);  /*Collision Frames*/
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_TunnelidIntfTunnelmodeSrcaddressDstaddress);
    ewsTelnetWrite(ewsContext,"\r\n--------    --------- ----------        -------------       ------------------");

    if(usmDbRlimTunnelIdFirstGet(&tunnelId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if(usmDbRlimTunnelIntIfNumGet(tunnelId, &interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    while(interface)
    {
      ewsTelnetPrintf (ewsContext, "\r\n%-12d", tunnelId);

      if (usmDbUnitSlotPortGet(interface, &unit, &slot, &port) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliGetIntfName(interface, unit,slot,port));
        ewsTelnetPrintf (ewsContext, "%-10s", buf);
      }

      if (usmDbRlimTunnelModeGet(unit, interface, &tunnelMode) == L7_SUCCESS)
      {
        switch (tunnelMode)
        {
        case L7_TUNNEL_MODE_6OVER4:
          osapiSnprintf(stat, sizeof(stat), "%-18s", pStrInfo_routing_TunnelMode);
          break;
        case L7_TUNNEL_MODE_6TO4:
          osapiSnprintf(stat, sizeof(stat), "%-18s", pStrInfo_routing_Ipv6Ip6To4);
          break;
        case L7_TUNNEL_MODE_IP6:
          osapiSnprintf(stat, sizeof(stat), "%-18s", pStrInfo_common_Ipv6_3);
          break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%-18s", pStrInfo_common_UnDefined);
        }
        ewsTelnetWrite(ewsContext,stat);
      }

      /* Print Local Address */
      if(usmDbRlimTunnelLocalAddrTypeGet(unit, interface, &addrType)
         == L7_SUCCESS)
      {
        switch (addrType)
        {
        case RLIM_ADDRTYPE_IP4:
          if(usmDbRlimTunnelLocalIp4AddrGet(unit, interface, &ipv4Addr)
             == L7_SUCCESS)
          {
            /* ip addr */
            rc = usmDbInetNtoa(ipv4Addr, stat);
            ewsTelnetPrintf (ewsContext, "%-20s", stat);
          }
          break;
        case RLIM_ADDRTYPE_IP6:
          if(usmDbRlimTunnelLocalIp6AddrGet(unit, interface, &ipv6Addr)
             == L7_SUCCESS)
          {
            osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&ipv6Addr, mIpv6addr, 40);
            ewsTelnetPrintf (ewsContext, "%-20s", stat);
          }
          break;
        default:
          ewsTelnetPrintf (ewsContext, "%-20s", " ");
        }
      }

      /* Print Remote Address */
      if(usmDbRlimTunnelRemoteAddrTypeGet(unit, interface, &addrType)
         == L7_SUCCESS)
      {

        switch (addrType)
        {
        case RLIM_ADDRTYPE_IP4:
          if(usmDbRlimTunnelRemoteIp4AddrGet(unit, interface, &ipv4Addr)
             == L7_SUCCESS)
          {
            /* ip addr */
            rc = usmDbInetNtoa(ipv4Addr, stat);
            ewsTelnetPrintf (ewsContext, "%-20s", stat);
          }
          break;
        case RLIM_ADDRTYPE_IP6:
          if(usmDbRlimTunnelRemoteIp6AddrGet(unit, interface, &ipv6Addr)
             == L7_SUCCESS)
          {
            osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&ipv6Addr, mIpv6addr, 40);
            ewsTelnetPrintf (ewsContext, "%-20s", stat);
          }
          break;
        default:
          ewsTelnetPrintf (ewsContext, "%-20s", " ");
        }
      }

      if(usmDbRlimTunnelIdNextGet(tunnelId, &nextTunnelId) == L7_SUCCESS)
      {
        if(usmDbRlimTunnelIntIfNumGet(nextTunnelId, &nextInterface)
           == L7_SUCCESS)
        {
          interface = nextInterface;
          tunnelId = nextTunnelId;
        }
        else
        {
          interface = 0;
        }
      }
      else
      {
        break;
      }
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

#endif

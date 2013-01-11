/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/common/cli_show_l3.c
 *
 * @purpose show commands for the cli
 *
 * @component user interface
 *
 * @comments
 *           An output value of Err means that the usmDb returned != SUCCESS   english num 1312
 *           An output value of ------ means that the value type was incompatible,
 *           and even though the usmDb return SUCCESS; the value is garbage.
 *
 * @create  08/09/2001
 *
 * @author  Diogenes De Los Santos
 * @author  Samip Garg
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "datatypes.h"

/* layer 3 includes           */
/* #include <osapi.h>
#include "nimapi.h" */
#include "usmdb_mib_ospf_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_1213_api.h"
#include "usmdb_2233_stats_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_ospf_api.h"
#include "usmdb_mib_ospfv3_api.h"
#include "usmdb_ospfv3_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_relay_api.h"
#include "usmdb_rtrdisc_api.h"
#include "l3end_api.h"
#include "clicommands_l3.h"
#include "l3_commdefs.h"
#include "l3_defaultconfig.h"
#include "default_cnfgr.h"
#include "osapi_support.h"
#include "usmdb_l3.h"
#include "cli.h"
#include "ews.h"
#include "clicommands_card.h"
#include "usmdb_rlim_api.h"
#include "clicommands_loopback.h"
#include "rlim_api.h"
#include "clicommands_time.h"
#include "cliutil_l3.h"
#include "util_enumstr.h"
#include "usmdb_dot1q_api.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif

#ifdef L7_BGP_PACKAGE
#include "clicommands_bgp_l3.h"
#endif
#ifdef L7_MCAST_PACKAGE
#include "clicommands_mcast.h"
#endif

/*********************************************************************
 *  Structure to hold the indications of the options parsed in
 *  "show ip route" command
 *********************************************************************/
typedef enum
{
  PROTOCOL_ONLY_OPTION = 0, /* It means "show ip route <protocol> [all]" */
  NO_OPTION,              /* It means just "show ip route [all]" without any option than "all" */
  IP_ADDR_OPTION,         /* It means "show ip route <ip-address> [<protocol>]" */
  IP_MASK_OPTION,         /* It means "show ip route <ip-address> <mask> [<protocol>] [all]" */
  LONGER_PREFIXES_OPTION, /* It means "show ip route <ip-address> <mask> longer-prefixes [<protocol>] [all]" */
  SUMMARY_OPTION          /* It means "show ip route summary [all]" */

} Option_e;

typedef struct showIpRouteOptions_s
{
  L7_BOOL    protocolOption;       /* if <protocol> option is given */
  L7_BOOL    allOption;            /* if "all" option is given */
  Option_e   showRouteOption;      /* one of the exclusive flags */

#define PROTOCOL_OPTION_MAX_LEN  20

  L7_char8   protocol[PROTOCOL_OPTION_MAX_LEN];
  L7_uint32  ipAddress;
  L7_uint32  ipMask;

} showIpRouteOptions_t;

/*********************************************************************
 *
 * @purpose          displays ip interface information.
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
 * @notes
 *
 * @cmdsyntax   show ip interface <slot/port>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 val, i;
  L7_RC_t rc;
  L7_uint32 intIface = 0, s, p;
  L7_uint32 encapType;
  L7_IP_MASK_t mask;
  L7_IP_ADDR_t ipAddr;
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackID = 0;
#endif
  L7_uchar8 strMacAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit = 0;
  L7_BOOL isUnnumbered;
  L7_uint32 numberedIfc;
  L7_uint32 numbUnit, numbSlot, numbPort;
  L7_uint32 ifState;
  L7_INTF_IP_ADDR_METHOD_t method;
  L7_BOOL isLoopback = L7_FALSE;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_ShowIpIntf_1, cliSyntaxInterfaceHelp());
  }

#ifdef L7_RLIM_PACKAGE
  if(argc > 4)
  {
    if((strcmp(argv[index], RLIM_INTF_LOOPBACK_NAME_PREFIX) == 0))
    {
      if(cliValidateLoopbackId(ewsContext, (L7_char8 *)argv[argc-1],
            &loopbackID) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_common_LoopBackIdOutOfRange, L7_CLIMIN_LOOPBACKID,
            L7_CLIMAX_LOOPBACKID);
      }
      if(usmDbRlimLoopbackIntIfNumGet(loopbackID, &intIface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
      else
      {
        if(usmDbUnitSlotPortGet(intIface, &unit, &s, &p) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      isLoopback = L7_TRUE;
    }
  }
  else
#endif
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &s, &p) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, s, p, &intIface) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &s, &p, &intIface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }

    if (cliSlotPortCpuCheck(s,p) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    if(cliIntfIsNamedByUSP(intIface) == L7_FALSE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
    {
      return cliPrompt(ewsContext);
    }

  }
  cliFormat(ewsContext, pStrInfo_routing_IpInterfaceStatus);
  if (usmDbIpRtrIntfOperModeGet(intIface, &ifState) == L7_SUCCESS)
  {
    (ifState == L7_ENABLE) ? sprintf(stat, "%s", pStrInfo_routing_Up) :
      sprintf(stat, "%s", pStrInfo_routing_Down);
  }
  else
  {
    sprintf(stat, "%s", pStrInfo_common_Blank);
  }
  ewsTelnetWrite(ewsContext, stat);
  
  if ((usmDbIpUnnumberedGet(intIface, &isUnnumbered, &numberedIfc) == L7_SUCCESS) &&
      (isUnnumbered == L7_TRUE))
  {
    cliFormat(ewsContext, pStrInfo_routing_Unnumbered);
    if (usmDbUnitSlotPortGet(numberedIfc, &numbUnit, &numbSlot, &numbPort) == L7_SUCCESS)
      sprintf(stat, "%s", cliGetIntfName(numberedIfc, numbUnit, numbSlot, numbPort));
    else
      ewsTelnetPrintf (ewsContext, "%13.12s", pStrInfo_common_Blank);
  }
  else if (usmDbIpRtrIntfCfgIpAddrListGet(unit, intIface, ipAddrList) == L7_SUCCESS)
  {
    L7_uint32 secondaryAddrCount = 0;

    ipAddr = ipAddrList[0].ipAddr;
    mask = ipAddrList[0].ipMask;

    if (ipAddr != L7_NULL_IP_ADDR)
    {
      cliFormat(ewsContext,pStrInfo_routing_IpAddrPrimary);                          /* ip addr */
      rc = usmDbInetNtoa(ipAddr, stat);
      ewsTelnetWrite(ewsContext, stat);

      if (mask != L7_NULL_IP_MASK)
      {
        rc = usmDbInetNtoa(mask, stat);
        ewsTelnetPrintf (ewsContext, "/%s", stat);
      }
    }

    if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_MULTINETTING) == L7_TRUE)
    {
      for (i = 1; i <= L7_L3_NUM_IP_ADDRS - 1; i++)
      {
        ipAddr = ipAddrList[i].ipAddr;
        mask = ipAddrList[i].ipMask;
  
        if (ipAddr != L7_NULL_IP_ADDR)
        {
          secondaryAddrCount++;
  
          if (secondaryAddrCount == 1)
          {
            cliFormat(ewsContext,pStrInfo_routing_IpAddresSecondary);
          }                                                                           /* ip addr */
  
          /* pad secondary addresses */
          if (secondaryAddrCount > 1)
          {
            cliFormat(ewsContext,pStrInfo_common_EmptyString);
          }                                                                /* ip addr */
  
          rc = usmDbInetNtoa(ipAddr, stat);
          ewsTelnetWrite(ewsContext, stat);
  
          if (mask != L7_NULL_IP_MASK)
          {
            rc = usmDbInetNtoa(mask, stat);
            ewsTelnetPrintf (ewsContext, "/%s", stat);
          }
        }
      }
    }
  }
  else
  {
    cliFormat(ewsContext,pStrInfo_routing_IpAddrPrimary);
    ewsTelnetWrite(ewsContext, pStrErr_common_Err);                    /* Err */
  }

  if (isLoopback == L7_FALSE)
  {
    cliFormat(ewsContext, pStrInfo_common_Method);
    if (usmDbIpRtrIntfIpAddressMethodGet(intIface, &method) == L7_SUCCESS)  /* Method */
    {
      switch(method)
      {
      case L7_INTF_IP_ADDR_METHOD_DHCP:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DHCP);
        break;
      case L7_INTF_IP_ADDR_METHOD_CONFIG:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Manual);
        break;
      case L7_INTF_IP_ADDR_METHOD_NONE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_None_1);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  }  
  cliFormat(ewsContext, pStrInfo_common_RoutingMode);                           /* Routing mode */
  if (usmDbIpRtrIntfModeGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, pStrInfo_common_AdministrativeMode);                   /* Administrative Mode */
  if (usmDbIfAdminStateGet(unit, intIface, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_DISABLE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dsbl_1);
      break;
	case L7_DIAG_DISABLE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DiagDsbl_1);
	  break;
    case L7_ENABLE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Enbl_1);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  if (isLoopback == L7_FALSE)
  {
    cliFormat(ewsContext, pStrInfo_routing_ForwardNetDirectedBcasts);
    if (usmDbIpNetDirectBcastsGet(unit, intIface, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_PROXY_ARP) == L7_TRUE)
    {
      /* Proxy ARP */
      cliFormat(ewsContext, pStrInfo_routing_IpProxyArp_1);
      if (usmDbProxyArpGet(unit, intIface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);
  
      /* Local Proxy ARP */
      val = 0;
      cliFormat(ewsContext, pStrInfo_routing_IpLocalProxyArp_1);
      if (usmDbLocalProxyArpGet(unit, intIface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);
    }
  
    cliFormat(ewsContext,pStrInfo_routing_ActiveState);                           /* Active State */
    if (usmDbIfActiveStateGet(unit, intIface, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_ACTIVE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_WsActive);                           /* Active */
        break;
      case L7_INACTIVE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_WsInactive);                           /* Inactive */
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    cliFormat(ewsContext, pStrInfo_routing_LinkSpeedDataRate);
    if (usmDbIfSpeedSimplexDataRateGet(unit, intIface, &val) == L7_SUCCESS)
    {
      if (usmDbIsIntfSpeedFullDuplex(unit, intIface) == L7_TRUE)
      {
        osapiSnprintf(stat, sizeof(stat),  "%u %s", val, pStrInfo_common_Full);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%u %s", val, pStrInfo_routing_Half);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_WsInactive);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    cliFormat(ewsContext, pStrInfo_common_MacAddr_2);
    if (usmDbIfIntfL3MacAddressGet(unit, intIface, strMacAddr) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%02X:%02X:%02X:%02X:%02X:%02X",
                    strMacAddr[0],strMacAddr[1],strMacAddr[2],strMacAddr[3],strMacAddr[4],strMacAddr[5]);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    cliFormat(ewsContext,pStrInfo_routing_EncapType);
    if (usmDbIfEncapsulationTypeGet(intIface, &encapType) == L7_SUCCESS)
    {
      switch (encapType)
      {
      case  L7_ENCAP_ETHERNET:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_EtherNet);
        break;
      case L7_ENCAP_802:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Snap);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    /* Displaying IP Mtu */
    cliFormat(ewsContext, pStrInfo_routing_IpMtu);
  
    if ((usmDbIntfEffectiveIpMtuGet(intIface, &val) == L7_SUCCESS))
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    /* Displaying Bandwidth */
    cliFormat(ewsContext, pStrInfo_common_Bandwidth);
  
    if ((usmDbIfBandwidthGet(intIface, &val) == L7_SUCCESS))
    {
      sprintf(stat, "%u kbps", val);
    }
    else
    {
      osapiStrncpy(stat, pStrErr_common_Err, sizeof(stat));
    }
    ewsTelnetWrite(ewsContext, stat);
    
    /* Displaying ICMP Unreachable Mode */
    val = 0;
    cliFormat(ewsContext, pStrInfo_common_DestUnreachables);
    if ((usmDbIpMapICMPUnreachablesModeGet(unit,intIface, &val) == L7_SUCCESS))
    {
      osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    /* Displaying ICMP Redirects Mode */
    val = 0;
    cliFormat(ewsContext, pStrInfo_common_ICMPRedirects);
    if ((usmDbIpMapIfICMPRedirectsModeGet(unit,intIface, &val) == L7_SUCCESS))
    {
      osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
    }
    else
    {
      osapiSnprintf(stat,sizeof(stat),pStrErr_common_Err);
    }

    ewsTelnetWrite(ewsContext, stat);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose  Displays Ip summary information
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
 * @notes
 *
 * @cmdsyntax     show ip brief
 *
 * @cmdhelp
 *
 * @cmddescript This commands takes no options
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpBrief(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 intDefaultTTL, val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 maxhops;
  L7_uint32 maxRoutes;
  L7_uint32 burstsize;
  L7_uint32 interval;
  L7_RC_t rc = L7_FAILURE;
  /* validity check and number of arguments */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowIpSummary_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext,pStrInfo_routing_DeflTimeToLive);
  memset (stat, 0, sizeof(stat));
  if (usmDbIpDefaultTTLGet(unit, &intDefaultTTL) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", intDefaultTTL);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext,pStrInfo_common_RoutingMode);
  memset (stat, 0, sizeof(stat));
  if (usmDbIpRtrAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, pStrInfo_routing_IpMaxNextHops);
  memset (stat, 0,sizeof(stat));
  maxhops = platRtrRouteMaxEqualCostEntriesGet();
  ewsTelnetPrintf (ewsContext, "%u", maxhops);
 /* Maximum number of routes*/
   maxRoutes=platRtrRouteMaxEntriesGet();
  cliFormat(ewsContext, pStrInfo_common_IpMaxRoutes);
  ewsTelnetPrintf (ewsContext, "%u", maxRoutes);
#if 0
  memset (strSourceCheckingMode, 0, sizeof(strSourceCheckingMode));
  cliFormat(ewsContext, pStrInfo_routing_IpSrcCheckingMode);
  if (usmDbIpSourceCheckingGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(strSourceCheckingMode,sizeof(strSourceCheckingMode),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(strSourceCheckingMode, sizeof(strSourceCheckingMode), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, strSourceCheckingMode);

  /*
     rc = usmDbIpNumOfActiveRoutesGet(unit, &val);
     bzero(activeRoutes, sizeof(activeRoutes));
     cliFormat(ewsContext,CLINUMBER_OF_ACTIVE_HOST_ENTRIES);
     ewsTelnetPrintf (ewsContext, "%u", val);
   */

  cliFormat(ewsContext,pStrInfo_routing_ForwardNetDirectedBcasts);
  if (usmDbIpNetDirectBcastsGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(strDirectedBcasts,sizeof(strDirectedBcasts),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(strDirectedBcasts, sizeof(strDirectedBcasts), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, strDirectedBcasts);
#endif
  cliFormat(ewsContext, pStrInfo_routing_IpIcmpRateLimitInterval);
  rc = usmDbIpMapRtrICMPRatelimitGet (&burstsize, &interval);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat), "%u msec", interval);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);
 
  cliFormat(ewsContext, pStrInfo_routing_IpIcmpRateLimitBurstSize);

  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat), "%u messages", burstsize);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, pStrInfo_routing_IpIcmpEchoReplyMode);
  val = 0;
  rc = usmDbIpMapRtrICMPEchoReplyModeGet (&val);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat), strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat); 

   /* Displaying ICMP Redirects Mode */
  val = 0;
  cliFormat(ewsContext, pStrInfo_common_ICMPRedirects);
  if ((usmDbIpMapRtrICMPRedirectsModeGet(&val) == L7_SUCCESS))
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat,sizeof(stat),pStrErr_common_Err);
  }
 
  ewsTelnetWrite(ewsContext, stat);
  
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Show IP helper configuration
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show ip helper-address [u/s/p]
*
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpHelperAddress(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = L7_CLI_ZERO;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 udpPort = 0;
  L7_ushort16 prevPort = 0;
  L7_uint32 serverAddr;
  L7_BOOL discard;
  L7_uint32 hitCount;
  L7_RC_t rc;
  L7_uint32 argSlotPort=1;
  L7_uint32 intIfNum, unit, slot, port;
  L7_uint32 reqIntIfNum = IH_INTF_ANY;    /* user requested entries for just this interface */
  L7_uint32 prevIntIfNum = 0xFFFFFFFF;
  L7_uchar8 udpPortName[IH_UDP_PORT_NAME_LEN];
  L7_uchar8 serverAddrStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uint32 enableStatus;

  /* validate arguments */
  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ((numArg != 0) && (numArg != 1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, 
                                           pStrErr_routing_ShowHelperIpIntf_1, cliSyntaxInterfaceHelp());
  }

  /* Show global enable/disable status */
  if (usmDbIpHelperAdminModeGet(&enableStatus) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "Failed to get the IP helper enable status.");
  }
  if (enableStatus == L7_ENABLE)
    ewsTelnetWrite(ewsContext, "\nIP helper is enabled\n");
  else if (enableStatus == L7_DISABLE)
    ewsTelnetWrite(ewsContext, "\nIP helper is disabled\n");
  else
    ewsTelnetWrite(ewsContext, "\nIP helper enable status is invalid\n");

  osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, "\r\n%-20s %11s %10s %10s %18s",
                 "Interface", "UDP Port", "Discard", "Hit Count", "Server Address");
  ewsTelnetWrite(ewsContext, buf);
  ewsTelnetWrite(ewsContext,
                 "\r\n-------------------- ----------- ---------- ---------- ------------------");

  if (numArg == 1)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
      return cliPrompt(ewsContext);
    }
    if (usmDbIntIfNumFromUSPGet(unit, slot, port, &intIfNum) != L7_SUCCESS)
    {  
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetIntIfNum);
    }
    if (cliSlotPortCpuCheck(slot,port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if(cliIntfIsNamedByUSP(intIfNum) == L7_FALSE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if ( cliValidateRtrIntf(ewsContext, intIfNum) != L7_SUCCESS )
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_INVALIDROUTERINTF);
    }
    /* Set reqIntIfNum = intIfNum. As we want output only for the user requested interface. */
    reqIntIfNum = intIfNum;
  }


  rc = usmDbIpHelperAddressFirst(&intIfNum, &udpPort, &serverAddr, &discard, &hitCount);
  if (rc != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "\r\nNo IP helper addresses are configured");
  }

  while (rc == L7_SUCCESS)
  {
    if ((reqIntIfNum == IH_INTF_ANY) || (intIfNum == reqIntIfNum))
    {
      bzero(buf, sizeof(buf));

      if (discard)
        serverAddrStr[0] = '\0';
      else
        usmDbInetNtoa(serverAddr, serverAddrStr);

      if ((intIfNum == prevIntIfNum) && (udpPort == prevPort))
      {
        /* Just print next server address */
        osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, "\r\n%73s", serverAddrStr);
      }
      else
      {
        /* New relay entry. Print all attributes. */
        strUtilUdpPortToString(udpPort, udpPortName);
        rc = usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port);
        osapiSnprintf(buf, L7_CLI_MAX_STRING_LENGTH, "\r\n%-20s %11s %10s %10u %18s",
                      intIfNum ? cliGetIntfName(intIfNum, unit, slot, port) : "Any",
                      udpPortName, discard ? "Yes" : "No", hitCount, serverAddrStr);
      }
      ewsTelnetWrite(ewsContext, buf);
    }
    prevIntIfNum = intIfNum;
    prevPort = udpPort;
    rc = usmDbIpHelperAddressNext(&intIfNum, &udpPort, &serverAddr, &discard,
                                  &hitCount);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Show IP helper statistics
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  show ip helper-address statistics
*
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpHelperStats(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = L7_CLI_ZERO;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  ipHelperStats_t helperStats;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* Error Checking for command Type */
  if (numArg > 0)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SHOW_IP_HELPER_STATS);
  }

  /* Show global enable/disable status */
  if (usmDbIpHelperStatisticsGet(&helperStats) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, "Failed to get the IP helper enable status.");
  }

  cliFormat(ewsContext, "DHCP client messages received");
  sprintf(stat, "%u", helperStats.dhcpClientMsgsReceived);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "DHCP client messages relayed");
  sprintf(stat, "%u", helperStats.dhcpClientMsgsRelayed);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "DHCP server messages received");
  sprintf(stat, "%u", helperStats.dhcpServerMsgsReceived);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "DHCP server messages relayed");
  sprintf(stat, "%u", helperStats.dhcpServerMsgsRelayed);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "UDP client messages received");
  sprintf(stat, "%u", helperStats.udpClientMsgsReceived);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "UDP client messages relayed");
  sprintf(stat, "%u", helperStats.udpClientMsgsRelayed);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "DHCP message hop count exceeded max");
  sprintf(stat, "%u", helperStats.tooManyHops);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "DHCP message with secs field below min");
  sprintf(stat, "%u", helperStats.tooEarly);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "DHCP message with giaddr set to local address");
  sprintf(stat, "%u", helperStats.spoofedGiaddr);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "Packets with expired TTL");
  sprintf(stat, "%u", helperStats.ttlExpired);
  ewsTelnetWrite(ewsContext, stat);

  cliFormat(ewsContext, "Packets that matched a discard entry");
  sprintf(stat, "%u", helperStats.matchDiscardEntry);
  ewsTelnetWrite(ewsContext, stat);

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose   Displays ip statistics information
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
 * @notes
 *
 * @cmdsyntax   show ip stats
 *
 * @cmdhelp
 *
 * @cmddescript This commands takes no options
 *
 * @end
 *
 *********************************************************************/
const char  *commandShowIpStats(EwsContext ewsContext, uintf argc,
                               const char * * argv, uintf index)
{
  L7_uint32 high, low;
  L7_uchar8 buf[1024];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowIpStats_1);
  }

  /* IP Statistics */

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbIpInReceivesGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpInReceives);  /* "datagrams received from interfaces"); */
    ewsTelnetPrintf (ewsContext, "%s ", strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpInHdrErrorsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrErr_routing_IpInHdrErrs);  /* "input datagrams discarded due to headers"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpInAddrErrorsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrErr_routing_IpInAddrErrs);  /* "input datagrams discarded due to invalid IP address");  */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpForwDatagramsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpForwDatagrams);  /* ,"input datagrams sent to forwarding"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpInUnknownProtosGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpInUnknownProtos);  /* ,"locally-addressed datagrams received and discarded"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpInDiscards64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpInDiscards);  /* ,"discarded input IP datagrams with no problems"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpInDelivers64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpInDelivers);  /* ,"input datagrams delivered to IP user-protocols"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpOutRequests64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpOutReqs);  /* ,"IP datagrams in request for transmission"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpOutDiscardsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpOutDiscards);  /* ,"discarded ouput IP datagrams with no problems"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpNoRoutesGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpOutNoRoutes);  /* ,"datagrams discarded with no route"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpReasmTimeoutGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpReasmTimeout);  /* ,"Max seconds to received fragments before reassembly"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpReasmReqdsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpReasmReq);  /* ,"IP fragments received to be reassembled"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpReasmOKsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpReasmOks);  /* ,"IP datagrams successfully re-assembled"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpReasmFailsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpReasmFails);  /* ,"failures detected by the IP re-assembyl agorithm"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpFragOKsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpFragOks);  /* ,"IP datagrams that have been fragmented"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpFragFailsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpFragFails);  /* ,"IP datagrams not fragmented and discarded"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpFragCreatesGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpFragCreates);  /* ,"IP datagrams fragments"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIpRoutingDiscardsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IpRoutingDiscards);  /* ,"routing entities to be discarded"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }


  /*ICMP Statistics*/

  if (usmDbIcmpInMsgsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInMsgs);  /* ,"ICMP messages which the entity received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInErrorsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrErr_routing_IcmpInErrs);  /* ,"ICMP messages received with errors"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInDestUnreachsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInDestUnreach);  /* ,"ICMP Destination Unreachable messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  cliSyntaxTop(ewsContext);
  if (usmDbIcmpInTimeExcdsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInTimeExceeds);  /* ,"ICMP Time Exceeded messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInParmProbsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInParmProbs);  /* ,"ICMP Parameter Problem messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInSrcQuenchsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInSrcQuenchs);  /* ,"ICMP Source Quench messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInRedirectsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInRedirects);  /* ,"ICMP Redirect messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInEchosGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInEchos);  /* ,"ICMP Echo messages"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInEchoRepsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInEchoReps);  /* ,"ICMP Echo Reply messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInTimestampsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInTimeStamps);  /* ,"ICMP Timestamp messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInTimestampRepsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInTimeStampReps);  /* ,"ICMP Timestamp reply messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInAddrMasksGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInAddrMasks);  /* ,"ICMP Address Mask Request messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpInAddrMaskRepsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpInAddrMaskReps);  /* ,"ICMP Address Mask Reply messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutMsgsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutMsgs);  /* ,"ICMP messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutErrorsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrErr_routing_IcmpOutErrs);  /* ,"ICMP messages not sent due to problems"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutDestUnreachsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutDestUnreach);  /* ,"ICMP Destination Unreachable messages received"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutTimeExcdsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutTimeExceeds);  /* ,"ICMP Time Exceeded messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutParmProbsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutParmProbs);  /* ,"ICMP Parameter Problem messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutSrcQuenchsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutSrcQuenchs);  /* ,"number of ICMP Source Quench messages"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutRedirectsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutRedirects);  /* ,"ICMP redirect messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }


  if (usmDbIcmpOutEchosGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutEchos);  /* ,"ICMP Echo Request messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutEchoRepsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutEchoReps);  /* ,"ICMP Echo Reply messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutTimestampsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutTimeStamps);  /* ,"ICMP Timestamp messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutTimestampRepsGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutTimeStampReps);  /* ,"ICMP Timestamp reply messages sent"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }

  if (usmDbIcmpOutAddrMasksGet64(unit, &high, &low) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_routing_IcmpOutAddrMasks);  /* ,"ICMP Address Mask Request messages"); */
    ewsTelnetPrintf (ewsContext, strUtil64toa (high, low, buf, sizeof (buf)));
  }


  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose Checks if valid protocol option is given
 *
 * @param EwsContext ewsContext
 * @param L7_char8 *option
 *
 * @returntype L7_RC_t
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t protocolOptionCheck(L7_char8 * option)
{
  if( (strcmp(option, pStrInfo_common_Connected_1) ==0) ||
     (strcmp(option, pStrInfo_common_Ospf_1) == 0) ||
     (strcmp(option, pStrInfo_common_Rip3) == 0) ||
     (strcmp(option, pStrInfo_common_Static2) == 0) ||
     (strcmp(option, pStrInfo_common_Bgp_1) == 0) )
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
 *
 * @purpose Checks if protocol option matches the protocol in the
 *          routing table entry
 *
 * @param EwsContext ewsContext
 * @param L7_char8 *option
 *
 * @returntype L7_BOOL
 * @returns L7_FALSE
 * @returns L7_TRUE
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_BOOL protocolMatchedWithOption(L7_char8 * protocol, L7_uint32 entryProtocol)
{
  if( (strcmp(protocol, pStrInfo_common_Connected_1) == 0) &&
      (entryProtocol == RTO_LOCAL) )
  {
    return L7_TRUE;
  }

  if( (strcmp(protocol, pStrInfo_common_Ospf_1) == 0) &&
      ((entryProtocol == RTO_OSPF_INTRA_AREA) ||
       (entryProtocol == RTO_OSPF_INTER_AREA) ||
       (entryProtocol == RTO_OSPF_TYPE1_EXT) ||
       (entryProtocol == RTO_OSPF_TYPE2_EXT) ||
       (entryProtocol == RTO_OSPF_NSSA_TYPE1_EXT) ||
       (entryProtocol == RTO_OSPF_NSSA_TYPE2_EXT)) )
  {
    return L7_TRUE;
  }

  if( (strcmp(protocol, pStrInfo_common_Rip3) == 0) &&
      (entryProtocol == RTO_RIP) )
  {
    return L7_TRUE;
  }

  if( (strcmp(protocol, pStrInfo_common_Static2) == 0) &&
      ((entryProtocol == RTO_STATIC) ||
       (entryProtocol == RTO_DEFAULT)) )
  {
    return L7_TRUE;
  }

  if( (strcmp(protocol, pStrInfo_common_Bgp_1) == 0) &&
      ((entryProtocol == RTO_IBGP) ||
       (entryProtocol == RTO_EBGP)) )
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
 *
 * @purpose Parses the "show ip route" command's options
 *
 * @param EwsContext ewsContext
 * @param uintf argc
 * @param const char **argv
 * @param {output} showIpRouteOptions_t *options
 *
 * @returntype L7_RC_t
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t parseShowIpRouteCommandOptions(EwsContext ewsContext, uintf argc,
                                       const char * * argv, uintf index,
                                       showIpRouteOptions_t * options)
{
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;

  numArg = cliNumFunctionArgsGet();

  options->protocolOption        = L7_FALSE;
  options->showRouteOption       = PROTOCOL_ONLY_OPTION;
  options->allOption             = L7_FALSE;

  memset(options->protocol, 0, sizeof(options->protocol));
  options->ipAddress = 0;
  options->ipMask = 0;

  /*****************************************************************
   * The following commands are to be accepted
   ******************************************************************
   *  show ip route [all]
   *  show ip route summary [all]
   *  show ip route <protocol> [all]
   *  show ip route <ip-address>
   *  show ip route <ip-address> <mask> [all]
   *  show ip route <ip-address> <protocol> [all]
   *  show ip route <ip-address> <mask> <protocol> [all]
   *  show ip route <ip-address> <mask> longer-prefixes [all]
   *  show ip route <ip-address> <mask> longer-prefixes <protocol> [all]
   *
   *****************************************************************/

  if(argc < 3)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
    return L7_FAILURE;
  }

  if (numArg == 0)
  {
    options->showRouteOption = NO_OPTION;
  }

  else if (numArg > 0)
  {
     OSAPI_STRNCPY_SAFE(buf, argv[index+1]);
     cliConvertToLowerCase(buf);
  }

  if ((numArg == 1) && (strcmp(buf, pStrInfo_common_All) == 0))
  {
    options->showRouteOption = NO_OPTION;
  }

  if (numArg > 0)
  {
    OSAPI_STRNCPY_SAFE(buf, argv[index+1]);
    cliConvertToLowerCase(buf);
  }
  if (numArg >= 1)
  {
    if(strcmp(buf, pStrInfo_common_Dot1xShowSummary) == 0)
    {
      options->showRouteOption = SUMMARY_OPTION;
    }
    else if(strcmp(buf, pStrInfo_common_All) == 0)
    {
      options->allOption = L7_TRUE;
    }
    /* Get the protocol type */
    else if(protocolOptionCheck(buf) == L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(options->protocol, buf);
      options->protocolOption = L7_TRUE;
    }
    else
    {
      /* Get the ip address */
      if (usmDbInetAton(buf, &(options->ipAddress)) == L7_SUCCESS)
      {
        options->showRouteOption = IP_ADDR_OPTION;
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
        return L7_FAILURE;
      }
    }
  }
  if (numArg >= 2)
  {
    OSAPI_STRNCPY_SAFE(buf, argv[index+2]);
    cliConvertToLowerCase(buf);

    if((options->showRouteOption != IP_ADDR_OPTION) &&
       (strcmp(buf, pStrInfo_common_All) == 0))
    {
      options->allOption = L7_TRUE;
    }
    /* Get the protocol type */
    else if(protocolOptionCheck(buf) == L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(options->protocol, buf);
      options->protocolOption = L7_TRUE;
    }
    else
    {
      /* Get the ip Mask*/
      if (usmDbInetAton(buf, &(options->ipMask)) == L7_SUCCESS)
      {
        options->showRouteOption = IP_MASK_OPTION;
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
        return L7_FAILURE;
      }
    }
  }
  if (numArg >= 3)
  {
    OSAPI_STRNCPY_SAFE(buf, argv[index+3]);
    cliConvertToLowerCase(buf);

    if(strcmp(buf, pStrInfo_common_All) == 0)
    {
      options->allOption = L7_TRUE;
    }
    else if(protocolOptionCheck(buf) == L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(options->protocol, buf);
      options->protocolOption = L7_TRUE;
    }
    else if(strcmp(buf, pStrInfo_routing_LongerPrefixesOption) == L7_SUCCESS)
    {
      options->showRouteOption = LONGER_PREFIXES_OPTION;
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
      return L7_FAILURE;
    }
  }
  if (numArg >= 4)
  {
    OSAPI_STRNCPY_SAFE(buf, argv[index+4]);
    cliConvertToLowerCase(buf);

    if(strcmp(buf, pStrInfo_common_All) == 0)
    {
      options->allOption = L7_TRUE;
    }
    else if(protocolOptionCheck(buf) == L7_SUCCESS)
    {
      OSAPI_STRNCPY_SAFE(options->protocol, buf);
      options->protocolOption = L7_TRUE;
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
      return L7_FAILURE;
    }
  }
  if (numArg == 5)
  {
    OSAPI_STRNCPY_SAFE(buf, argv[index+5]);
    cliConvertToLowerCase(buf);

    if(strcmp(buf, pStrInfo_common_All) == 0)
    {
      options->allOption = L7_TRUE;
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  Causes the entire route table to be displayed
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
 * @notes
 *
 * @cmdsyntax    show ip route [{<ip-address> [<protocol>] | {<ip-address> <mask> [longer-prefixes]
 *                              [<protocol>] | <protocol> | summary} [all] | all}]
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char  *commandShowIpRoute(EwsContext ewsContext, uintf argc,
                               const char * * argv, uintf index)

{
  L7_RC_t      rc=L7_SUCCESS;
  L7_uint32    i;
  L7_char8     stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8     buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8     strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32    unit, slot, port;
  L7_uint32    numArg;
  L7_char8     strFinal[2*L7_CLI_MAX_STRING_LENGTH];
  L7_uint32    numLines;
  const L7_char8 * codesArr[]= { pStrInfo_common_Blank, pStrInfo_common_Ipv6TypeConnected, pStrInfo_common_Ipv6TypeStatic, pStrInfo_common_Mpls,
                                 pStrInfo_common_Ipv6TypeOspfIntra, pStrInfo_routing_TypeOspfInter,
                                 pStrInfo_routing_TypeOspfType, pStrInfo_routing_TypeOspfType2,
                                 pStrInfo_routing_TypeOspfNssaType, pStrInfo_routing_TypeOspfNssaType2,
                                 pStrInfo_common_Ipv6TypeOspfIntra, pStrInfo_routing_TypeOspfInter,
                                 pStrInfo_routing_TypeOspfType, pStrInfo_routing_TypeOspfType2,
                                 pStrInfo_routing_TypeOspfNssaType, pStrInfo_routing_TypeOspfNssaType2,
                                 pStrInfo_common_Ipv6TypeRip, pStrInfo_common_Ipv6TypeStatic, pStrInfo_common_B, pStrInfo_common_B };
  L7_BOOL        flagPrint  = L7_FALSE;
  static L7_BOOL bestRoutes = L7_FALSE;
  L7_uint32    cntLocal=0, cntIntra=0, cntInter=0, cntExtOne=0,
               cntExtTwo=0, cntRip=0, cntStatic=0, cntBgp = 0, cntReject = 0,
               maskLength=0, spacePadding = 0;

  static L7_routeEntry_t        routeTable;
  static showIpRouteOptions_t   options;
  L7_uint32 globalDefGwAddr;
  L7_timespec routeUpdateTime;
  cliCmdScrollSet(L7_FALSE);

  numArg = cliNumFunctionArgsGet();

  if(numArg > 5)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_ShowRtrRouteTbl);
    return cliPrompt(ewsContext);
  }

  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    return cliPrompt(ewsContext);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    numLines = 0;
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    memset((L7_char8 *)&routeTable, 0, sizeof(L7_routeEntry_t));
    numLines = 6; /* 6 lines for the header */

    if(parseShowIpRouteCommandOptions(ewsContext, argc, argv, index,
          &options) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    /* best routes will be displayed only if the "all" option is not given */
    bestRoutes = ((options.allOption) ? L7_FALSE : L7_TRUE);

#ifdef L7_PRODUCT_SMB
    {
      OSAPI_STRNCPY_SAFE_ADD_BLANKS (2, 2, 0, 0, L7_NULLPTR, stat, pStrInfo_routing_ShowIpRouteConnectedStatic);
      ewsTelnetWrite(ewsContext,stat);
    }
#else
    if(options.showRouteOption != SUMMARY_OPTION)
    {
      OSAPI_STRNCPY_SAFE_ADD_BLANKS (2, 0, 0, 0, L7_NULLPTR, stat, pStrInfo_routing_ShowIpRouteCodes_2);
      ewsTelnetWrite(ewsContext,stat);

      OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 7, 0, L7_NULLPTR, stat, pStrInfo_routing_ShowIpRouteCodes2);
      ewsTelnetWrite(ewsContext,stat);

      OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 7, 0, L7_NULLPTR, stat, pStrInfo_routing_ShowIpRouteCodes);
      ewsTelnetWrite(ewsContext,stat);

      OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 2, 7, 0, L7_NULLPTR, stat, pStrInfo_routing_ShowIpRouteCodes_1);
      ewsTelnetWrite(ewsContext,stat);
    }
#endif

    if (usmDbIpGlobalDefaultGatewayAddressGet (&globalDefGwAddr, L7_TRUE)
                                            == L7_SUCCESS)
    {
      if (globalDefGwAddr != L7_NULL)
      {
        memset(buf, 0, sizeof(buf));
        rc = usmDbInetNtoa(globalDefGwAddr, buf);
        osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_DefGatway, buf);
        ewsTelnetWrite(ewsContext,stat);
      }
    }
    
    if(options.showRouteOption == IP_ADDR_OPTION)
    {
      /* Get the best matching route (longest prefix match) */
      rc = usmDbBestMatchRouteLookup(unit, options.ipAddress, &routeTable);
    }
    else
    {
      /* best and non-best matching routes are iterated in the else */

      /* to get the matching entry with ip-address and ip-mask */
      if(options.showRouteOption == IP_MASK_OPTION)
      {
        routeTable.ipAddr = options.ipAddress;
      }

      rc = usmDbNextRouteEntryGet(unit, &routeTable, bestRoutes);

      /* When the "longer-prefixes" keyword is included,
         the address and mask pair becomes the prefix,
         and any address that matches that prefix is displayed.
         Therefore, multiple addresses are displayed. */
      if(options.showRouteOption == LONGER_PREFIXES_OPTION)
      {
        while((rc == L7_SUCCESS) &&
            ((routeTable.ipAddr & options.ipMask) !=
             (options.ipAddress & options.ipMask)))
        {
          rc = usmDbNextRouteEntryGet(unit, &routeTable, bestRoutes);
        }
      }
    }
    if (rc != L7_SUCCESS && options.showRouteOption != SUMMARY_OPTION)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  do
  {
    /* Forming a displayable row */
    flagPrint = L7_FALSE;

    if (routeTable.protocol >= RTO_LOCAL && routeTable.protocol <= RTO_EBGP)
    {
      OSAPI_STRNCPY_SAFE(buf, codesArr[routeTable.protocol]);
    }
    else
    {
      OSAPI_STRNCPY_SAFE(buf, codesArr[0]);
    }

    osapiSnprintf(stat, sizeof(stat), "%-7s",buf);
    osapiSnprintf(strFinal, sizeof(strFinal), stat);

    memset(buf, 0, sizeof(buf));
    rc = usmDbInetNtoa(routeTable.ipAddr, buf);
    osapiSnprintf(stat, sizeof(stat),  "%s/", buf);
    OSAPI_STRNCAT(strFinal, stat);

    memset(buf, 0, sizeof(buf));
    rc = usmDbInetNtoa(routeTable.subnetMask, buf);
    maskLength = usmCliMaskLength (routeTable.subnetMask);
    osapiSnprintf(stat, sizeof(stat),  "%d ", maskLength);
    OSAPI_STRNCAT(strFinal, stat);

    /* "Route Preference" and "Route Metric" */
    osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_ShowIpv6RoutePrefAndMetric,
        routeTable.pref, routeTable.metric);
    OSAPI_STRNCAT(strFinal, stat);

    /* All the "OPTION" checks will be handled in the below blocks */
    switch(options.showRouteOption)
    {
    case IP_ADDR_OPTION:
      flagPrint = L7_TRUE;
      rc = L7_FAILURE; /* we will return after we get
                          a best-route with best match */
      break;
    case IP_MASK_OPTION:
      if((routeTable.ipAddr == options.ipAddress) &&
          (routeTable.subnetMask == options.ipMask))
      {
        flagPrint = L7_TRUE;
      }
      break;
    case LONGER_PREFIXES_OPTION:
      /* check for longer-prefixes was done as part of iteration */
    case NO_OPTION:
      flagPrint = L7_TRUE;
      break;
    case SUMMARY_OPTION:
      switch (routeTable.protocol)
      {
      case RTO_LOCAL:
        cntLocal++;
        break;
      case RTO_STATIC:
      case RTO_DEFAULT:
        cntStatic++;
        break;
      case RTO_OSPF_INTRA_AREA:
        cntIntra++;
        break;
      case RTO_OSPF_INTER_AREA:
        cntInter++;
        break;
      case RTO_OSPF_TYPE1_EXT:
      case RTO_OSPF_NSSA_TYPE1_EXT:
        cntExtOne++;
        break;
      case RTO_OSPF_TYPE2_EXT:
      case RTO_OSPF_NSSA_TYPE2_EXT:
        cntExtTwo++;
        break;
      case RTO_RIP:
        cntRip++;
        break;
      case RTO_IBGP:
      case RTO_EBGP:
        cntBgp++;
        break;
      default:
        break;
      }
      if(routeTable.flags & L7_RTF_REJECT)
      {
        cntReject++;
      }
      break;
    default:
      break;
    }
    if(options.protocolOption == L7_TRUE)
    {
      if(protocolMatchedWithOption(options.protocol, routeTable.protocol) == L7_TRUE)
      {
        /* Setting true for the case "show ip route <protocol> [all]" */
        if(options.showRouteOption == PROTOCOL_ONLY_OPTION)
        {
          flagPrint = L7_TRUE;
        }
      }
      else
      {
        flagPrint = L7_FALSE;
      }
    }

    /* Print the Route entries(including ECMP) if the checks above have passed */
    if(flagPrint == L7_TRUE)
    {
      if (routeTable.flags & L7_RTF_REJECT)
      {
        if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_REJECTROUTE) == L7_TRUE)
        {
          numLines++;
          /* Reject route */
          osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Rejected);
          OSAPI_STRNCAT(strFinal,stat);
          ewsTelnetWrite(ewsContext,strFinal);
          cliSyntaxNewLine(ewsContext);
        }
      }
      else
      {
        for (i = 0; i < routeTable.ecmpRoutes.numOfRoutes; i++)
        {
          numLines++;

          spacePadding = strlen(strFinal);
          if ( routeTable.protocol != RTO_LOCAL )
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 1, L7_NULLPTR, strFinal,pStrInfo_common_Via);
            usmDbInetNtoa(routeTable.ecmpRoutes.equalCostPath[i].arpEntry.ipAddr,
                          strIpAddr);
            osapiSnprintf(stat, sizeof(stat), "%s,   ", strIpAddr);
            OSAPI_STRNCAT(strFinal, stat);

	    if(routeTable.protocol !=RTO_STATIC )
	    {
               /*Specifies the last time the route was updated (in hours:minutes:seconds)*/
               osapiConvertRawUpTime((simSystemUpTimeGet()-routeTable.updateTime),&routeUpdateTime);
		
               if(routeUpdateTime.days>=1)
               {
                     osapiSnprintf(stat,sizeof(stat),"%02dd:%02dh:%02dm,  ",routeUpdateTime.days,
                                   routeUpdateTime.hours,routeUpdateTime.minutes);

                     OSAPI_STRNCAT(strFinal,stat);
               }
               else
               {
		     osapiSnprintf(stat,sizeof(stat),"%02dh:%02dm:%02ds,  ",routeUpdateTime.hours, 
                                   routeUpdateTime.minutes,routeUpdateTime.seconds);

                     OSAPI_STRNCAT(strFinal,stat);
               }

             }
          }
          else
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 3, L7_NULLPTR, strFinal,pStrInfo_routing_DirectlyConnected);
          }
          if (usmDbUnitSlotPortGet(
                routeTable.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum,
                &unit, &slot, &port) == L7_SUCCESS)
          {
            osapiSnprintf(stat, sizeof(stat), cliGetIntfName(routeTable.ecmpRoutes.equalCostPath[i].arpEntry.intIfNum, unit, slot, port));
            OSAPI_STRNCAT(strFinal, stat);
          }
          else
          {
            OSAPI_STRNCPY_SAFE(stat, pStrInfo_common_Invalid_1);
            OSAPI_STRNCAT(strFinal,stat);
          }
          ewsTelnetWrite(ewsContext,strFinal);
          cliSyntaxNewLine(ewsContext);
          memset(strFinal, 0, sizeof(strFinal));
          memset(strFinal, ' ', spacePadding);
        }
      }
    }

    /* Get the next entry */
    if(rc == L7_SUCCESS)
    {
      rc = usmDbNextRouteEntryGet(unit, &routeTable, bestRoutes);
      if(options.showRouteOption == LONGER_PREFIXES_OPTION)
      {
        while((rc == L7_SUCCESS) &&
            ((routeTable.ipAddr & options.ipMask) !=
             (options.ipAddress & options.ipMask)))
        {
          rc = usmDbNextRouteEntryGet(unit, &routeTable, bestRoutes);
        }
      }
    }
  }
  while ((rc == L7_SUCCESS ) && (numLines <= 15));
  /* end of while loop */

  if (rc != L7_SUCCESS)
  {
    if (options.showRouteOption == SUMMARY_OPTION)
    {
      cliSyntaxTop(ewsContext);

      cliFormat(ewsContext, pStrInfo_common_RouteConnectedRoutes);
      ewsTelnetPrintf (ewsContext, "%d", cntLocal);

      cliFormat(ewsContext, pStrInfo_common_RoutesTaticRoutes);
      ewsTelnetPrintf (ewsContext, "%d", cntStatic);
#ifdef L7_RIP_PACKAGE
      cliFormat(ewsContext, pStrInfo_routing_RtrIpRoutes);
      ewsTelnetPrintf (ewsContext, "%d", cntRip);
#endif

#ifdef L7_BGP_PACKAGE
      cliFormat(ewsContext, pStrInfo_routing_RtrBgpRoutes);
      ewsTelnetPrintf (ewsContext, "%d", cntBgp);
#endif

#ifdef L7_OSPF_PACKAGE
      cliFormat(ewsContext, pStrInfo_common_RouteOspfRoutes);
      ewsTelnetPrintf (ewsContext, "%d", (cntIntra+cntInter+cntExtOne+cntExtTwo));

      cliFormatAddBlanks (0, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_RouteIntraAreaRoutes);
      ewsTelnetPrintf (ewsContext, "%d", cntIntra);

      cliFormatAddBlanks (0, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_RouteInterAreaRoutes);
      ewsTelnetPrintf (ewsContext, "%d", cntInter);

      cliFormatAddBlanks (0, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_RouteExternal);
      ewsTelnetPrintf (ewsContext, "%d", cntExtOne);

      cliFormatAddBlanks (0, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_RouteExternal2Routes);
      ewsTelnetPrintf (ewsContext, "%d", cntExtTwo);
#endif

      if(usmDbFeaturePresentCheck(unit, L7_IP_MAP_COMPONENT_ID, L7_IPMAP_FEATURE_REJECTROUTE) == L7_TRUE)
      {
        cliFormat(ewsContext, pStrInfo_common_Rtr_1);
        ewsTelnetPrintf (ewsContext, "%d", cntReject);
      }
      cliFormat(ewsContext, pStrInfo_common_RouteTotalRoutes);
      ewsTelnetPrintf (ewsContext, "%d", (cntIntra+cntInter+cntExtOne+
            cntExtTwo+cntLocal+cntStatic+
            cntRip+cntBgp));

    }
    /* done creating output */
    return cliPrompt(ewsContext);
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_routing_ShowIpRoute);
  }
  return pStrInfo_common_Name_2;
}

/*********************************************************************
 *
 * @purpose  Displays preferences values for all routing protocols
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
 * @notes
 *
 * @cmdsyntax    show ip route preferences
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpRoutePreferences(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_RC_t rc;
  L7_uint32 pref;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_ShowRtrRoutePrefs_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  cliFormat(ewsContext, pStrInfo_common_WsNwLocal);
  memset (stat, 0, sizeof(stat));
  rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_LOCAL, &pref);
  ewsTelnetPrintf (ewsContext, "%u", pref);

  cliFormat(ewsContext, pStrInfo_common_PimSmGrpRpMapStatic);
  memset (stat, 0, sizeof(stat));
  rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_STATIC, &pref);
  ewsTelnetPrintf (ewsContext, "%u", pref);

  if (usmDbComponentPresentCheck(unit, L7_FLEX_MPLS_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliFormat(ewsContext, pStrInfo_common_Mpls);
    memset (stat, 0, sizeof(stat));
    rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_MPLS, &pref);
    ewsTelnetPrintf (ewsContext, "%u", pref);
  }

  if (usmDbComponentPresentCheck(unit, L7_OSPF_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliFormat(ewsContext, pStrInfo_common_OspfIntra);
    memset (stat, 0, sizeof(stat));
    rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_OSPF_INTRA_AREA, &pref);
    ewsTelnetPrintf (ewsContext, "%u", pref);

    cliFormat(ewsContext, pStrInfo_common_OspfInter);
    memset (stat, 0, sizeof(stat));
    rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_OSPF_INTER_AREA, &pref);
    ewsTelnetPrintf (ewsContext, "%u", pref);

    cliFormat(ewsContext, pStrInfo_common_OspfExternal);
    memset (stat, 0, sizeof(stat));
    rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_OSPF_EXTERNAL, &pref);
    osapiSnprintf(stat, sizeof(stat),  "%u", pref);
    ewsTelnetPrintf (ewsContext, "%u", pref);
  }

  if (usmDbComponentPresentCheck(unit, L7_RIP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    cliFormat(ewsContext, pStrInfo_common_Rip);
    memset (stat, 0, sizeof(stat));
    rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_RIP, &pref);
    ewsTelnetPrintf (ewsContext, "%u", pref);
  }

  if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    /* For release G, the preference of iBGP routes and eBGP routes is
       always the same. */
    cliFormat(ewsContext, pStrInfo_common_Bgp4);
    memset (stat, 0, sizeof(stat));
    rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_IBGP, &pref);
    ewsTelnetPrintf (ewsContext, "%u", pref);
  }

  cliFormat(ewsContext, "Configured Default Gateway");
  memset(stat, 0, sizeof(stat));
  rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_GLOBAL_DEFAULT_GATEWAY, &pref);
  ewsTelnetPrintf(ewsContext, "%u", pref);

  cliFormat(ewsContext, "DHCP Default Gateway");
  memset(stat, 0, sizeof(stat));
  rc = usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_DEFAULT_ROUTE_DHCP, &pref);
  ewsTelnetPrintf(ewsContext, "%u", pref);

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose    displays the ip interface summary.
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
 * @notes
 *
 * @cmdsyntax    show ip interface brief
 *
 * @cmdhelp
 *
 * @cmddescript  This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpInterfaceBrief(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 val;
  L7_uint32 intSlot, intPort;
static L7_uint32 interface;
  L7_uint32 nextInterface;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_IP_MASK_t mask;
  L7_IP_ADDR_t ipAddr;
  L7_INTF_IP_ADDR_METHOD_t method;
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackID = 0;
#endif
  L7_uint32 ifState;
  L7_uint32 count = 0;

  /*  validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  cliCmdScrollSet (L7_FALSE);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRtrRipIntfSummary_1);
  }
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  { /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      interface = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    /* get the first interface */
    if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NoValidPorts);
    }
  }
  /* Heading for the router rip interface summary */
  ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_routing_IntfIpAddrIpMaskBcastCastfwd);
  ewsTelnetWrite(ewsContext,"\r\n----------   -----  --------------- --------------- ------- \r\n");

  while (interface)
  {
    if (usmDbIpIntfExists(unit, interface) == L7_TRUE
#ifdef L7_RLIM_PACKAGE
        || (usmDbRlimLoopbackIdGet(interface, &loopbackID) == L7_SUCCESS)
#endif
       )
    {
      if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliGetIntfName(interface, val, intSlot, intPort));
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%13.12s", pStrErr_common_Err);
      }
      ewsTelnetPrintf (ewsContext, "%-13s",buf);

      if (usmDbIpRtrIntfOperModeGet(interface, &ifState) == L7_SUCCESS)
      {
        if (ifState == L7_ENABLE)
          sprintf(buf, "%-7.6s", pStrInfo_routing_Up);
        else if (ifState == L7_DISABLE)
          sprintf(buf, "%-7.6s", pStrInfo_routing_Down);
        else
          sprintf(buf, "%-7.6s", pStrInfo_common_Blank);
      }
      else
      {
        sprintf(buf, "%-7.6s", pStrInfo_common_Blank);
      }
      ewsTelnetWrite(ewsContext, buf);

      /* If interface is unnumbered, say so. */
      if (usmDbIntfIsUnnumbered(interface))
      {
        ewsTelnetPrintf (ewsContext, "%-32.15s", "unnumbered");
      }

      /*print the ip address and the ipmask */
      else if (usmDbIpRtrIntfIpAddressGet(unit, interface, &ipAddr, &mask) == L7_SUCCESS)
      {
        /* ip addr */
        rc = usmDbInetNtoa(ipAddr, stat);
        ewsTelnetPrintf (ewsContext, "%-16.15s", stat);
        /* subnet mask */
        rc = usmDbInetNtoa(mask, stat);
        ewsTelnetPrintf (ewsContext, "%-16.15s", stat);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-16.15s","-----");                   /* Err */
      }

      memset (stat, 0, sizeof(stat));
      if(usmDbIpRtrIntfIpAddressMethodGet(interface, &method)==L7_SUCCESS)
      {
        if(method == L7_INTF_IP_ADDR_METHOD_CONFIG)
        {
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s", pStrInfo_common_Manual);
        }
        else if(method == L7_INTF_IP_ADDR_METHOD_DHCP)
        {
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s", pStrInfo_common_DHCP);
        }
        else  if(method == L7_INTF_IP_ADDR_METHOD_NONE)
        {
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s",pStrInfo_common_None_1);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s",pStrErr_common_Err);
        }
      }
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      count++;
    }
    /* Get the next interface */
    if ((rc = usmDbValidIntIfNumNext(interface, &nextInterface)) == L7_SUCCESS)
    {
      interface = nextInterface;
      if ( count >= CLI_MAX_SCROLL_LINES - 6)
      {
        break;
      }
    }
    else
    {
      interface = 0;
    }
  }     /* end of while loop */

  if ((rc == L7_SUCCESS) && (count >= CLI_MAX_SCROLL_LINES - 6))
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_routing_ShowIpInterfaceBrief);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
  
  return cliPrompt(ewsContext);

}

/*********************************************************************
 *
 * @purpose Displays Arp table information
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
 * @notes
 *
 * @cmdsyntax    show arp
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowArp(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 count, val, loopCount;
  L7_uint32 intSlot, intPort, intU_IDX;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_IP_ADDR_t ipAddr;
  static L7_uint32 intIfNum;
  L7_uint32 i;
  L7_uint32 menuDisplayed = L7_FALSE;
  L7_char8 * pBuf;
  L7_arpEntry_t arp, * pArp;
  L7_uint32 unit;
  L7_arpCacheStats_t pStats;
  L7_uint32 ageSecs, ageMins, ageHrs;
  L7_uint32 numArg;

  pArp = &arp;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  numArg =  cliNumFunctionArgsGet();
  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowArpTbl);
  }

  unit = cliGetUnitId();
  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_routing_AgeTimeSeconds);
    if (usmDbIpArpAgeTimeGet(unit, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_routing_RespTimeSeconds);
    if (usmDbIpArpRespTimeGet(unit, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_routing_Retries_1);
    if (usmDbIpArpRetriesGet(unit, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_routing_CacheSize);
    if (usmDbIpArpCacheSizeGet(unit, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* Kavleen: Added for ARP Enhancements  - Begin */
    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_routing_DynRenewMode);
    if (usmDbIpArpDynamicRenewGet(unit, &val) == L7_SUCCESS)
    {
      osapiSnprintf(buf,sizeof(buf),strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
      osapiSnprintf(stat, sizeof(stat), buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    if (usmDbIpArpCacheStatsGet(unit, &pStats) == L7_SUCCESS)
    {
      cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_routing_ArpTotalEntryCount);
      ewsTelnetPrintf (ewsContext, "%d / %d",pStats.cacheCurrent,pStats.cachePeak);

      cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_routing_ArpStaticEntryCount);
      ewsTelnetPrintf (ewsContext, "%u / %d / %d", usmDbIpMapStaticArpCount(unit),
          pStats.staticCurrent, pStats.staticMax);
    }
    else
    {
      ewsTelnetPrintf (ewsContext, pStrErr_common_Err);
    }

    /*
       Total Entry Count
       Static Entry Count
       Maximum Static Entry Count

       If brief not given, then
       display each ARP entry
     */
    /* Kavleen: Added for ARP Enhancements  - End */

    ipAddr = 0;
    intIfNum = 0;
    loopCount = 9;

    ewsTelnetWrite(ewsContext,    pStrInfo_common_CrLf);

    if (numArg == 0)
    {
      ewsTelnetWriteAddBlanks (1, 0, 2, 6, L7_NULLPTR, ewsContext,    pStrInfo_routing_IpAddrMacAddrIntfTypeAge);
      ewsTelnetWrite(ewsContext,    "\r\n---------------  -----------------  --------------  --------  -----------  ");

      menuDisplayed = L7_TRUE;
    }
    else
    {
      /* 'brief' format does not display any of the ARP entries */
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    loopCount = 15;
    menuDisplayed = L7_FALSE;
  }

  /* Heading for the arp table */

  for (count = 0; count < loopCount; count++)
  {

    if (usmDbIpArpEntryNext(unit, ipAddr, intIfNum, pArp) != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if ((count == 0) && (menuDisplayed == L7_FALSE))
    {
      ewsTelnetWrite(ewsContext,    pStrInfo_common_CrLf);
      ewsTelnetWriteAddBlanks (1, 0, 2, 6, L7_NULLPTR, ewsContext,    pStrInfo_routing_IpAddrMacAddrIntfTypeAge);
      ewsTelnetWrite(ewsContext,    "\r\n---------------  -----------------  --------------  --------  -----------  ");
    }

    ipAddr = pArp->ipAddr;                                                     /* update to current IP address */
    intIfNum = pArp->intIfNum;

    cliSyntaxTop(ewsContext);

    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    if (usmDbInetNtoa(pArp->ipAddr, buf) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-17s", buf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-17s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    for (i = 0, pBuf = buf; i < pArp->macAddr.len - 1; i++, pBuf+=3)
    {
      osapiSnprintf(pBuf, sizeof(buf), "%2.2X:", pArp->macAddr.addr.enetAddr.addr[i]);
    }
    osapiSnprintf(pBuf, sizeof(buf), "%2.2X", pArp->macAddr.addr.enetAddr.addr[i]);
    ewsTelnetPrintf (ewsContext, "%-19.19s", buf);

    memset (stat, 0, sizeof(stat));
    if (usmDbUnitSlotPortGet(pArp->intIfNum, &intU_IDX, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-16s", cliGetIntfName(pArp->intIfNum, intU_IDX, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-16.14s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    if ((pArp->flags & L7_ARP_LOCAL) != 0)
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_WsNwLocal);
    }
    else if ((pArp->flags & L7_ARP_GATEWAY) != 0)
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_routing_Gateway_1);
    }
    else if ((pArp->flags & L7_ARP_STATIC) != 0)
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_PimSmGrpRpMapStatic);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dyn_1);
    }
    ewsTelnetPrintf (ewsContext, "%-10.8s", buf);

    /* Kavleen : Added for ARP Enhancements */
    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    if ((pArp->flags & (L7_ARP_LOCAL | L7_ARP_STATIC)) == 0)
    {
      ageSecs = pArp->ageSecs;
      ageHrs = ageSecs / SECONDS_PER_HOUR;
      ageSecs %= SECONDS_PER_HOUR;
      ageMins = ageSecs / SECONDS_PER_MINUTE;
      ageSecs %= SECONDS_PER_MINUTE;
      osapiSnprintf(buf, sizeof(buf), pStrInfo_routing_HMS,ageHrs,ageMins,ageSecs);
      osapiSnprintf(stat, sizeof(stat), "%-13.11s", buf);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), "    %s", pStrInfo_routing_NotApplicable_1);
      osapiSnprintf(stat, sizeof(stat), "%-13.11s", buf);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  if (usmDbIpArpEntryNext(unit, ipAddr, intIfNum, pArp) != L7_SUCCESS)
  {
    ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_routing_ShowArp_1);                            /*hard coded in command name for now, #define later :-) */
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }
}

/*********************************************************************
 *
 * @purpose  Displays router discovery information for all interfaces
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
 * @cmdsyntax in stacking env: show ip irdp <unit/slot/port | all>
 * @cmdsyntax in non-stacking env: show ip irdp <slot/port | all>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpIrdp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 interface;
  static L7_BOOL   all;
  static L7_RC_t   rc;
  L7_char8  stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 i;
  L7_uint32 u, s, p, slot, port;
  L7_uint32 nextInterface, mode, ipAddr;
  L7_uint32 argSlotPort=1;
  L7_int32  time, integer;
  L7_BOOL   flag;
  L7_uint32 unit = 0;
  L7_int32  count;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_ShowRtrIrdp, cliSyntaxInterfaceHelp());
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  { /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    rc = L7_FAILURE;
    if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
    {
      /* get switch ID based on presence/absence of STACKING package */
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }

      all = L7_TRUE;
      if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS)
      {
        rc = usmDbIpRtrIntfModeGet(unit, interface, &mode);
      }
    }
    else
    {
      all = L7_FALSE;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      else
      {
        /* NOTE: No need to check the value of `unit` as
         *       ID of a standalone switch is always `U_IDX` (=> 1).
         */
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port, &interface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      if ( cliValidateRtrIntf(ewsContext, interface) != L7_SUCCESS )
      {
        return cliPrompt(ewsContext);
      }

      if (usmDbRtrDiscIsValidIntf(unit, interface) != L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
      }

      rc = usmDbIpRtrIntfModeGet(unit, interface, &mode);
    }
  }

  /* Heading for the router discovery */
  ewsTelnetWrite(ewsContext,    pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,    pStrInfo_routing_IntfAdModeAdvertiseAddrMaxIntMinIntHoldTimePref);
  ewsTelnetWrite(ewsContext,    "\r\n------------  -------  -----------------  -------  -------  ---------  -----------\r\n");

  count = 0;
  while ((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
  {
    if (( usmDbValidateRtrIntf(unit, interface) == L7_SUCCESS ) &&
        (usmDbRtrDiscIsValidIntf(unit, interface) == L7_TRUE))
    {
      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      if ( usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliGetIntfName(interface, u, s, p));
        osapiSnprintf(stat, sizeof(stat), "%-14.12s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-14.12s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      if ((usmDbRtrDiscAdvertiseGet(unit, interface, &flag) == L7_SUCCESS))
      {
        osapiSnprintf(stat,sizeof(stat),"%-9.7s",strUtilEnableDisableGet(flag,pStrInfo_common_Dsbl_1));
      }
        else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9.7s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      if ((usmDbRtrDiscAdvAddrGet(unit, interface, &ipAddr) == L7_SUCCESS))
      {
        if (usmDbInetNtoa(ipAddr, buf) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-19.17s", buf);
      }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-19.17s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      if ((usmDbRtrDiscMaxAdvIntervalGet(unit, interface, &time) == L7_SUCCESS))
      {
        osapiSnprintf(buf, sizeof(buf), "%u", time);
        osapiSnprintf(stat, sizeof(stat), "%-9.7s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9.7s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      if ((usmDbRtrDiscMinAdvIntervalGet(unit, interface, &time) == L7_SUCCESS))
      {
        osapiSnprintf(buf, sizeof(buf), "%u", time);
        osapiSnprintf(stat, sizeof(stat), "%-9.7s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-9.7s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      if (usmDbRtrDiscAdvLifetimeGet(unit, interface, &time) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%u", time);
        osapiSnprintf(stat, sizeof(stat), "%-11.8s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-11.8s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      if ((usmDbRtrDiscPreferenceLevelGet(unit, interface, &integer) == L7_SUCCESS))
      {
        osapiSnprintf(buf, sizeof(buf), "%d", integer);
        osapiSnprintf(stat, sizeof(stat), "%-11.11s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%-4.4s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);
      ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    }

    if (!all)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else
    {
      do   /*continue to get new interfaces until there are no more
             available or a routing interface is found. Interfaces valid
             for routing only get displayed for the 'all' parameter.*/
      {
        if (usmDbValidIntIfNumNext(interface, &nextInterface) != L7_SUCCESS)
        {
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
          return cliSyntaxReturnPrompt (ewsContext, "");
        }

        interface = nextInterface;
        rc = usmDbIpRtrIntfModeGet(unit, interface, &mode);
      } while (rc != L7_SUCCESS);
    }
    count++;
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  cliSyntaxBottom(ewsContext);

  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);

  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}

/*********************************************************************
 *
 * @purpose  Displays BOOTP/DHCP relay information
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
 * @cmdsyntax  show bootpdhcprelay
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowBootpDhcpRelay(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRelay);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_routing_MaxHopCount);
  if (usmDbBootpDhcpRelayMaxHopCountGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf),  "%u", val);
  }
  ewsTelnetWrite(ewsContext,buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_routing_MinWaitTimeSeconds);
  if (usmDbBootpDhcpRelayMinWaitTimeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf),  "%u", val);
  }
  ewsTelnetWrite(ewsContext,buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_common_IgmpProxyAdminMode);
  if (usmDbIpHelperAdminModeGet(&val) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
  }
  ewsTelnetWrite(ewsContext,buf);

  memset (buf, 0, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_routing_CircuitIdOptionMode_1);
  if (usmDbBootpDhcpRelayCircuitIdOptionModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(buf,sizeof(buf),strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
  }
  ewsTelnetWrite(ewsContext,buf);

  return cliSyntaxReturnPrompt (ewsContext,buf);
}

/*********************************************************************
 *
 * @purpose  display vlan routing entries
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
 * @notes in progress
 *
 * @cmdsyntax  show ip vlan
 *
 * @cmdhelp Display summary information about all VLANs with routing enabled
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandShowIpVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nextvid = 0;
  L7_uchar8 macAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_IP_MASK_t mask;
  L7_IP_ADDR_t ipAddr;
  L7_uint32 intIfNum, unitId, slot, port;
  L7_uint32 unit;

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowIpVlanRouting_1);
  }
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  rc = usmDbIpVlanRtrVlanIdGetNext(unit, &nextvid);
  if (rc != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_NoCfguredVlansOnWhichRoutingIsEnbld);
  }

  /* display MAC address */
  ewsTelnetWriteAddBlanks (1, 0, 0, 3, L7_NULLPTR, ewsContext, pStrInfo_common_MacAddrUsedByRoutingVlans);
  if (usmDbIpVlanRtrVlanIdToIntIfNum(unit, nextvid, &intIfNum) == L7_SUCCESS)
  {
    if (usmDbIfIntfL3MacAddressGet(unit, intIfNum, macAddr) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%02X:%02X:%02X:%02X:%02X:%02X \r\n", macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
  }
  else
  {
      osapiSnprintf(stat, sizeof(stat),  pStrErr_common_NotAvailable);
  }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat),  pStrErr_common_NotAvailable);
  }
  ewsTelnetWrite(ewsContext, stat);

  ewsTelnetWriteAddBlanks (1, 0, 11, 36, L7_NULLPTR, ewsContext,pStrInfo_common_Logical);
  ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_routing_VlanIdIntfIpAddrSubnetMask);
  ewsTelnetWrite(ewsContext,"\r\n-------  --------------  ---------------  ---------------");

  do
  {
    /* print VLAN ID */
    ewsTelnetPrintf (ewsContext, "\r\n%-9d", nextvid);

    /* print Logical Interface */
    if (usmDbIpVlanRtrVlanIdToIntIfNum(unit, nextvid, &intIfNum) == L7_SUCCESS)
    {
      if (usmDbUnitSlotPortGet(intIfNum, &unitId, &slot, &port) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "%-16.14s", cliGetIntfName(intIfNum, unitId, slot, port));
      }
    }

    /* print IP Address */
    if (usmDbIpRtrIntfIpAddressGet(unit, intIfNum, &ipAddr, &mask) == L7_SUCCESS)
    {
      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      rc = usmDbInetNtoa(ipAddr, stat);
      ewsTelnetPrintf (ewsContext, "%-17.15s", stat);

      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      rc = usmDbInetNtoa(mask, stat);
      ewsTelnetPrintf (ewsContext, "%-15.15s", stat);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrErr_common_Err);                    /* Err */
    }
  } while (usmDbIpVlanRtrVlanIdGetNext(unit, &nextvid) == L7_SUCCESS);

  return cliSyntaxReturnPrompt (ewsContext, "");

}

/*********************************************************************
*
* @purpose  Show VLANs internally assigned to port based routing interfaces.
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
* @cmdsyntax  show vlan internal usage
*
* @cmdhelp Show VLANs assigned to port-based routing interfaces
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVlanInternal(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 vlanId;           /* next VLAN ID to report */
  static L7_RC_t rc;
  L7_uint32 i;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        
  L7_int32  count;
  L7_uchar8 usage[L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR + 1];         


  cliSyntaxTop(ewsContext);

  cliCmdScrollSet(L7_FALSE);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, "Use show vlan internal usage");
  }

  osapiSnprintf(stat, L7_CLI_MAX_STRING_LENGTH, "\r\nBase VLAN ID:  %u", usmDbDot1qBaseInternalVlanIdGet());
  ewsTelnetWrite(ewsContext, stat);

  osapiSnprintf(stat, L7_CLI_MAX_STRING_LENGTH, "\r\nAllocation policy:  %s\r\n", 
                (usmDbDot1qInternalVlanPolicyGet() == DOT1Q_INT_VLAN_POLICY_ASCENDING) ? "Ascending" : "Descending");
  ewsTelnetWrite(ewsContext, stat);
  

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  { /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
     {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
     }
  }
  else
  {
    rc = usmDbDot1qInternalVlanFindFirst(&vlanId);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, "VLAN   Usage");
  ewsTelnetWrite(ewsContext,"\r\n-----  --------------------------------");

  count = 0;
  while ((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
  {
    /* print VLAN ID */
    ewsTelnetPrintf (ewsContext, "\r\n%-5d  ", vlanId);

    /* print usage description */
    rc = usmDbDot1qInternalVlanUsageDescrGet(vlanId, usage);
    if (rc == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%-33s ", usage);
    }
    else 
    {
     sprintf(stat,"%-33s ", "");
     ewsTelnetWrite(ewsContext,stat);
    }

    rc = usmDbDot1qInternalVlanFindNext(vlanId, &vlanId);
    if (rc != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    count++;
  }

  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);

  cliSyntaxBottom(ewsContext);

  osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
  for (i=1; i<argc; i++)
  {
    OSAPI_STRNCAT(cmdBuf, " ");
    OSAPI_STRNCAT(cmdBuf, argv[i]);
  }
  cliAlternateCommandSet(cmdBuf);

  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}



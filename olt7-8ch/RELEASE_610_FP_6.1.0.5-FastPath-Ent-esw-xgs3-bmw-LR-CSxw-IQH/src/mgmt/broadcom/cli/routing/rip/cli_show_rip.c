/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/rip/cli_show_rip.c
 *
 * @purpose show commands for the RIP
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

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "cliapi.h"
#include "datatypes.h"

/* layer 3 includes           */
/* #include <osapi.h>
#include "nimapi.h" */
#include "usmdb_ip_api.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_relay_api.h"
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

/*********************************************************************
 *
 * @purpose Displays RIP (Routing Information Protocol) information
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
 * @cmdsyntax    show ip rip
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowRIP(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, rrdMode;
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 metric, matchType, filterValue, defaultRoute;
  L7_RC_t rc;
  L7_char8 match[L7_CLI_MAX_STRING_LENGTH];

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRtrRipInfo_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_RipAdminMode);                           /* RIP Mode */
  if (usmDbRip2AdminModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Split Horizon Mode*/
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_SplitHorizonMode);
  if (usmDbRip2SplitHorizonModeGet(unit, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_RIP_SPLITHORIZON_NONE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_None_1);    /* None */
      break;
    case L7_RIP_SPLITHORIZON_SIMPLE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Simple_1);   /* Simple */
      break;
    case L7_RIP_SPLITHORIZON_POISONREVERSE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_PoisonReverse);   /* Poison Reverse*/
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);   /* Err */
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Auto summary mode*/
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_AutoSummaryMode);
  if (usmDbRip2AutoSummarizationModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);   /* Err */
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Host Routes Accept Mode*/
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_HostRoutesAcceptMode);
  if (usmDbRip2HostRoutesAcceptModeGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);   /* Err */
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_GlobalRouteChgs_1);
  if (usmDbRip2GlobalRouteChangesGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_GlobalQueries_1);
  if (usmDbRip2GlobalQueriesGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Added to show RIP Route Redistribution Details */
  memset (stat, 0, sizeof(stat));
  cliSyntaxTop(ewsContext);
  cliFormat(ewsContext, pStrInfo_common_DeflMetric);

  rc = usmDbRip2DefaultMetricGet(unit,&val);

  if (rc == L7_ERROR)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_NotCfgured);
  }
  else
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }

  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_DeflRouteAdvertise);
  if (usmDbRip2DefaultRouteAdvertiseModeGet(unit,&defaultRoute) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%d", defaultRoute);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  for (protocol = 1; protocol < 6; protocol++)
  {
    if (usmDbRip2RouteRedistributeModeGet(unit,protocol, &rrdMode) == L7_SUCCESS)
    {
      if (rrdMode == L7_ENABLE)
      {
        cliSyntaxTop(ewsContext);
        cliFormat(ewsContext, pStrInfo_common_Redistribute);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_Src);

        switch (protocol)
        {
        case REDIST_RT_OSPF:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Ospf_1);
          break;
        case REDIST_RT_BGP:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Bgp_1);
          break;
        case REDIST_RT_STATIC:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Static2);
          break;
        case REDIST_RT_LOCAL:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Connected);
          break;
        default:
          continue;
        }
        ewsTelnetWrite(ewsContext, stat);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_Metric);

        rc = usmDbRip2RouteRedistributeMetricGet(unit,protocol,&metric);
        if (rc == L7_ERROR)
        {
          osapiSnprintf(stat, sizeof(stat),pStrErr_common_ApProfileNameNotCfgured);
        }
        else
        if (rc == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u", metric);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext,stat);

        if (protocol == REDIST_RT_OSPF)
        {
          memset (stat, 0, sizeof(stat));
          cliFormat(ewsContext, pStrInfo_common_MatchVal);

          rc = usmDbRip2RouteRedistributeMatchTypeGet(unit,protocol,&matchType);
          if (rc == L7_ERROR )
          {
            ewsTelnetPrintf (ewsContext, pStrInfo_common_None_1);
          }
          else
          if (rc == L7_SUCCESS)
          {
            memset (match, 0, sizeof(match));
              if ((matchType & L7_OSPF_METRIC_TYPE_INTERNAL) == L7_OSPF_METRIC_TYPE_INTERNAL)
            {
              OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, match,pStrInfo_common_Internal);
            }

              if ((matchType & L7_OSPF_METRIC_TYPE_EXT1) == L7_OSPF_METRIC_TYPE_EXT1)
            {
              OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, match,pStrInfo_common_External1);
            }
              if ((matchType & L7_OSPF_METRIC_TYPE_EXT2) == L7_OSPF_METRIC_TYPE_EXT2)
            {
              OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, match,pStrInfo_common_External2);
            }
              if ((matchType & L7_OSPF_METRIC_TYPE_NSSA_EXT1) == L7_OSPF_METRIC_TYPE_NSSA_EXT1)
            {
              OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, match,pStrInfo_common_NssaExternal1);
            }
              if ((matchType & L7_OSPF_METRIC_TYPE_NSSA_EXT2) == L7_OSPF_METRIC_TYPE_NSSA_EXT2)
            {
              OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, match,pStrInfo_common_NssaExternal2);
            }
            ewsTelnetPrintf (ewsContext, match);
        }
        }

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_DistributeList);

        rc = usmDbRip2RouteRedistributeFilterGet(unit,protocol,&filterValue);

        if (rc == L7_ERROR)
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_NotCfgured);
        }
        else
        if (rc == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u", filterValue);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        }

        ewsTelnetWrite(ewsContext, stat);

      }
      }
  }
  /* Added to show the RIP Route Redistribution Details - End */

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose Displays RIP (Routing Information Protocol) information
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
 * @cmdsyntax    show ip rip interface <slot/port)
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpRIPInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 val, authType;
  L7_uint32 linkState = L7_DOWN;
  L7_uint32 intSlot, intPort, interface;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_RIP+1];

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_ShowRtrRipIntfDetailed, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
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
    if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  if ( cliValidateRtrIntf(ewsContext, interface) != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }
  

  cliFormat(ewsContext, pStrInfo_common_Intf);
  ewsTelnetPrintf (ewsContext, cliGetIntfName(interface, unit,intSlot,intPort));

  memset (buf, 0, sizeof(buf));
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ApIpAddr);
  if (( usmDbRip2IfConfSrcIpAddrGet(unit, interface, &val) == L7_SUCCESS ) &&
      ( usmDbInetNtoa(val, buf) == L7_SUCCESS ))
  {
    osapiSnprintf(stat, sizeof(stat), buf);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_SendVer_1);
  if (usmDbRip2IfConfSendGet(unit, interface, &val) == L7_SUCCESS)
  {
    switch (val)                                                               /* None, RIP-1, RIP-2, compat  */
    {
    case L7_RIP_CONF_SEND_DO_NOT_SEND:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_None_1);
      break;
    case L7_RIP_CONF_SEND_RIPV1:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Rip_1);
      break;
    case L7_RIP_CONF_SEND_RIPV1_COMPAT:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Rip_2);
      break;
    case L7_RIP_CONF_SEND_RIPV2:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Rip2);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_ReceiveVer_1);
  if (usmDbRip2IfConfReceiveGet(unit, interface, &val) == L7_SUCCESS)
  {
    switch (val)                                                               /* None, RIP-1, RIP-2, compat  */
    {
    case L7_RIP_CONF_RECV_DO_NOT_RECEIVE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_None_1);
      break;
    case L7_RIP_CONF_RECV_RIPV1:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Rip_1);
      break;
    case L7_RIP_CONF_RECV_RIPV2:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Rip2);
      break;
    case L7_RIP_CONF_RECV_RIPV1_RIPV2:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Both);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_RipAdminMode);                           /* RIP Mode */
  if (usmDbRip2IfConfStatusGet(unit, interface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val,"-----"));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_LinkState);
  if (usmDbRip2IntfStateGet(unit, interface, &linkState) == L7_SUCCESS)
  {
    switch (linkState)
    {
    case L7_DOWN:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_1);
      break;
    case L7_UP:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Up);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AuthType_1);
  if (usmDbRip2IfConfAuthTypeGet(unit, interface, &authType) == L7_SUCCESS)
  {
    switch (authType)
    {
    case L7_AUTH_TYPE_NONE:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_None_1);
      break;
    case L7_AUTH_TYPE_SIMPLE_PASSWORD:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Simple_1);
      break;
    case L7_AUTH_TYPE_MD5:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Md5);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Auth Key */
  if (authType == L7_AUTH_TYPE_SIMPLE_PASSWORD || authType == L7_AUTH_TYPE_MD5)
  {
    cliFormat(ewsContext, pStrInfo_common_AuthKey);
    if (usmDbRip2IfConfAuthKeyActualGet(unit, interface, authKey) == L7_SUCCESS)
    {
      if ((cliUserNumberGet()==0) && (mainMenu[cliCurrentHandleGet()] == cliGetMode(L7_PRIVILEGE_USER_MODE)))   /* admin user in privileged mode */
      {
        ewsTelnetPrintf (ewsContext, "\"%s\"", authKey);
      }
      else
      {
        ewsTelnetWrite(ewsContext, "********");
      }
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }
  }

  /* Auth Key ID */
  if (authType == L7_AUTH_TYPE_MD5)
  {
    cliFormat(ewsContext, pStrInfo_common_AuthKeyId);
    if (usmDbRip2IfConfAuthKeyIdGet(unit, interface, &val) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_BadPktsRcvd);
  if (linkState == L7_DOWN)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }                                                    /* invalid since link is down */
  else if (usmDbRip2IfStatRcvBadPacketsGet(unit, interface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    val = 0;
    osapiSnprintf(stat, sizeof(stat), "%u", val);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_BadRoutesRcvd);
  if (linkState == L7_DOWN)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }                                                    /* invalid since link is down */
  else if (usmDbRip2IfStatRcvBadRoutesGet(unit, interface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    val = 0;
    osapiSnprintf(stat, sizeof(stat), "%u", val);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_UpdatesSent);
  if (linkState == L7_DOWN)
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }                                                    /* invalid since link is down */
  else if (usmDbRip2IfStatSentUpdatesGet(unit, interface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    val = 0;
    osapiSnprintf(stat, sizeof(stat), "%u", val);
  }
  return cliSyntaxReturnPrompt (ewsContext, stat);
}

/*********************************************************************
 *
 * @purpose Displays RIP (Routing Information Protocol) information
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
 * @notes     Here we only display the visible interfaces.
 *
 * @cmdsyntax    show ip rip interface brief
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpRIPInterfaceBrief(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val;
  L7_uint32 intSlot, intPort, interface;
  L7_uint32 nextInterface;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRtrRipIntfSummary_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* get the first interface */
  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NoValidPorts);
  }

  /* Heading for the router rip interface summary */
  ewsTelnetWriteAddBlanks (1, 0, 31, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_SendReceiveRipLink);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_IntfIpAddrVerVerModeState);
  ewsTelnetWrite(ewsContext,"\r\n------------  ---------------  -----------  -----------  ---------  ----------\r\n");

  while (interface)
  {
    /* display only the visible interfaces - and non loop/tnnl for now */
    if ((usmDbRip2IsValidIntf(unit, interface) == L7_TRUE)  &&
        (usmDbIpIntfExists(unit, interface) == L7_TRUE))
    {
      memset (buf, 0,sizeof(buf));
      memset (strSlotPort, 0,sizeof(strSlotPort));
      if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliGetIntfName(interface, val, intSlot, intPort));
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
      }
      ewsTelnetPrintf (ewsContext, "%-14.12s",buf);

      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      if (( usmDbRip2IfConfSrcIpAddrGet(unit, interface, &val) == L7_SUCCESS ) &&
          ( usmDbInetNtoa(val, buf) == L7_SUCCESS ))
      {
        osapiSnprintf(stat, sizeof(stat),  "%-17.16s", buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-17.16s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      if (usmDbRip2IfConfSendGet(unit, interface, &val) == L7_SUCCESS)
      {
        switch (val)                                                           /* None, RIP-1, RIP-2, RIP-1c  */
        {
        case L7_RIP_CONF_SEND_DO_NOT_SEND:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_None_1);
          break;
        case L7_RIP_CONF_SEND_RIPV1:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Rip_1);
          break;
        case L7_RIP_CONF_SEND_RIPV1_COMPAT:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Rip_2);
          break;
        case L7_RIP_CONF_SEND_RIPV2:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Rip2);
          break;
        default:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Blank);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-13.11s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      if (usmDbRip2IfConfReceiveGet(unit, interface, &val) == L7_SUCCESS)
      {
        switch (val)                                                           /* None, RIP-1, RIP-2, compat  */
        {
        case L7_RIP_CONF_RECV_DO_NOT_RECEIVE:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_None_1);
          break;
        case L7_RIP_CONF_RECV_RIPV1:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Rip_1);
          break;
        case L7_RIP_CONF_RECV_RIPV2:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Rip2);
          break;
        case L7_RIP_CONF_RECV_RIPV1_RIPV2:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Both);
          break;
        default:
          osapiSnprintf(stat, sizeof(stat),  "%-13.11s", pStrInfo_common_Blank);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-13.11s", "-----");
      }

      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      memset (buf, 0, sizeof(buf));
      if (usmDbRip2IfConfStatusGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"%-11.9s",strUtilEnableDisableGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-11.9s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset (stat, 0, sizeof(stat));
      if (usmDbRip2IntfStateGet(unit, interface, &val) == L7_SUCCESS)
      {
        switch (val)
        {
        case L7_DOWN:
          osapiSnprintf( stat, sizeof(stat),   "%-11.9s", pStrInfo_common_Down_1);
          break;
        case L7_UP:
          osapiSnprintf(stat, sizeof(stat),  "%-11.9s", pStrInfo_common_Up);
          break;
        default:
          osapiSnprintf(stat, sizeof(stat),  "%-11.9s", pStrInfo_common_Blank);
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-11.9s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      cliSyntaxBottom(ewsContext);
      /*end if interface exists*/
    }                                                                            /* end if visible interface */

    /* Get the next interface */
    if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
    }
    else
    {
      interface = 0;
    }
  }                                                                            /* end of while loop */

  return cliPrompt(ewsContext);
}


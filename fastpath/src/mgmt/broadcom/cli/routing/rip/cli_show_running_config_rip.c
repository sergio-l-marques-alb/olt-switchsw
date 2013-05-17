/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/rip/cli_show_running_config_rip.c
 *
 * @purpose show running config commands for RIP
 *
 * @component user interface
 *
 * @comments
 *
 * @create  18/08/2003
 *
 * @author  Samip
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_ip_api.h"
#include "usmdb_mib_ripv2_api.h"
#include "comm_mask.h"
#include "config_script_api.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"

#include "l3_defaultconfig.h"

#include "cli_show_running_config.h"
#include "util_enumstr.h"

/*********************************************************************
 * @purpose  To print the running configuration of rip Info
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

L7_RC_t cliRunningConfigRIPInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val ;

  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_routing_RtrRip);

  /* For RIP admin mode we want to display the config whether or not it is at the default value */
  if (usmDbRip2AdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RIP_MAP_DEFAULT_ADMIN_MODE,pStrInfo_common_Enbl_2);
  }
  if (usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_RIP, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val, FD_RTR_RTO_PREF_RIP, pStrInfo_routing_DistanceRip);
  }
  /* Split Horizon Mode*/
  if (usmDbRip2SplitHorizonModeGet(unit, &val) == L7_SUCCESS)
  {
    if (( val != FD_RIP_MAP_DEFAULT_SPLITHORIZON_MODE )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      switch (val)
      {
      case L7_RIP_SPLITHORIZON_NONE:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_SplitHorizonNone);
        break;
      case L7_RIP_SPLITHORIZON_SIMPLE:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_SplitHorizonSimple);
        break;
      case L7_RIP_SPLITHORIZON_POISONREVERSE:
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_routing_SplitHorizonPoison);
        break;
      default:
        /* Do nothing */
        break;
      }
      }
    }

  /* Auto summary mode*/
  if (usmDbRip2AutoSummarizationModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RIP_MAP_DEFAULT_AUTOSUMMARY_MODE,pStrInfo_routing_AutoSummary);
    }

  /* Host Routes Accept Mode*/
  if (usmDbRip2HostRoutesAcceptModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RIP_MAP_DEFAULT_HOSTROUTESACCEPT_MODE,pStrInfo_routing_Hostroutesaccept);
  }

  cliRunningConfigRIPRedistribute(ewsContext, unit);
  EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_Exit); /*exit from router rip mode */
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of RIP Redistribute Operation
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
L7_RC_t cliRunningConfigRIPRedistribute(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 protocolString[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 matchString[L7_CLI_MAX_STRING_LENGTH];
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 val, metric, match;
  L7_BOOL metricFlag = L7_DISABLE, matchFlag= L7_DISABLE, mode= L7_DISABLE;

  if (usmDbRip2DefaultMetricGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_RIP_DEFAULT_REDIST_ROUTE_METRIC,pStrInfo_common_DeflMetric_1);
  }

  if (usmDbRip2DefaultRouteAdvertiseModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_RIP_DEFAULT_DEFAULT_ROUTE_ADV_MODE,pStrInfo_common_DispOspfDeflInfo);
  }

  for (protocol = 1; protocol<=5; protocol++)
  {
    metricFlag = L7_DISABLE, matchFlag= L7_DISABLE, mode= L7_DISABLE;

    if (protocol != REDIST_RT_RIP)
    {
      memset(protocolString, 0,sizeof(protocolString));
      switch (protocol)
      {
      case REDIST_RT_LOCAL:
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Connected_1);
        break;

      case REDIST_RT_STATIC:
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Static2);
        break;

      case REDIST_RT_OSPF:
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Ospf_1);
        break;

      case REDIST_RT_BGP:
        if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) != L7_TRUE)
        {
          continue;
        }

        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Bgp_1);
        break;
        /* include these to keep the compiler happy */
      case REDIST_RT_FIRST:
      case REDIST_RT_LAST:
      case REDIST_RT_RIP:
      case REDIST_RT_OSPFV3:
        break;
      }

      memset(stat, 0,sizeof(stat));
      if (usmDbRip2RouteRedistributeModeGet(unit, protocol, &val) == L7_SUCCESS)
      {
        if ((val != FD_RIP_DEFAULT_REDIST_ROUTE_MODE) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
        {
          if (val!=FD_RIP_DEFAULT_REDIST_ROUTE_MODE)
          {
            mode = L7_ENABLE;

            if (usmDbRip2RouteRedistributeMetricGet(unit, protocol, &metric) == L7_SUCCESS)
            {
              if ((metric != FD_RIP_DEFAULT_REDIST_ROUTE_METRIC)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {

                metricFlag = L7_ENABLE;
              }
            }

            if ((protocol == REDIST_RT_OSPF)
                && (usmDbRip2RouteRedistributeMatchTypeGet(unit, protocol, &match) == L7_SUCCESS))
            {
              if ((match != FD_RIP_DEFAULT_REDIST_OSPF_MATCH_TYPE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
                matchFlag= L7_ENABLE;
                memset(matchString, 0, sizeof(matchString));
                if ((match & L7_OSPF_METRIC_TYPE_INTERNAL) == L7_OSPF_METRIC_TYPE_INTERNAL)
                {
                  OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, matchString, pStrInfo_common_Internal_2);
                }
                if ((match & L7_OSPF_METRIC_TYPE_EXT1) == L7_OSPF_METRIC_TYPE_EXT1)
                {
                  OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, matchString, pStrInfo_common_External1_1);
                }
                if ((match & L7_OSPF_METRIC_TYPE_EXT2) == L7_OSPF_METRIC_TYPE_EXT2)
                {
                  OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, matchString, pStrInfo_common_External2_1);
                }
                if ((match & L7_OSPF_METRIC_TYPE_NSSA_EXT1) == L7_OSPF_METRIC_TYPE_NSSA_EXT1)
                {
                  OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, matchString, pStrInfo_common_NssaExternal1_1);
                }
                if ((match & L7_OSPF_METRIC_TYPE_NSSA_EXT2) == L7_OSPF_METRIC_TYPE_NSSA_EXT2)
                {
                  OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, matchString, pStrInfo_common_NssaExternal2_1);
                }
              }
              }

            if (metricFlag == L7_ENABLE)
            {
              if (matchFlag == L7_ENABLE)
              {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_RedistributeMetricMatch,protocolString, metric, matchString);
                EWSWRITEBUFFER(ewsContext, stat);
              }
              else
              {
                osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_RedistributeMetric,protocolString, metric);
                EWSWRITEBUFFER(ewsContext, stat);
              }
            }
            else if (matchFlag == L7_ENABLE)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_RedistributeMatch_1,protocolString, matchString);
              EWSWRITEBUFFER(ewsContext, stat);
            }

            if ((metricFlag != L7_ENABLE) && (matchFlag != L7_ENABLE))
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Redistribute_2,protocolString);
              EWSWRITEBUFFER(ewsContext, stat);
            }

          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_NoRedistribute,protocolString);
            EWSWRITEBUFFER(ewsContext, stat);
          }
        }
      }

      if (usmDbRip2RouteRedistributeFilterGet(unit, protocol, &val) == L7_SUCCESS)
      {
        if ((val != FD_RIP_DEFAULT_REDIST_ROUTE_FILTER)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_DistributeListOut, val, protocolString);
          EWSWRITEBUFFER(ewsContext, stat);
        }
        }
    }  /* End of if for source = ospf|bgp|static|connected     */
  }     /* End of For loop for protocol */

  return L7_SUCCESS;
}


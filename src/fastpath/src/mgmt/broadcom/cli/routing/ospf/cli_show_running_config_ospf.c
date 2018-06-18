/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/ospf/cli_show_running_config_ospf.c
 *
 * @purpose show running config commands for OSPF
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
#include "usmdb_l3.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"
#include "comm_mask.h"
#include "config_script_api.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"

#include "usmdb_rtrdisc_api.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_defaultconfig.h"
#include "clicommands_loopback.h"
#include "usmdb_ip6_api.h"

#ifdef L7_IPV6_PACKAGE
#include "clicommands_tunnel.h"
#endif

#include "cli_show_running_config.h"
#include "util_enumstr.h"

/*********************************************************************
 * @purpose  To print the running configuration of ospf Info
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

L7_RC_t cliRunningConfigOspfInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val, val1, authType;
  L7_int32 ival;
  L7_int32 adminVal = L7_DISABLE;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf2[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 maxMetricCmd[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 areaId, aggrAreaId, neighbor, wildcardMask;
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_RC_t virt_flag;
  L7_RC_t rc, rc1;
  L7_char8 areaIdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 defInfoOrg, metric;
  L7_OSPF_STUB_METRIC_TYPE_t metricType;
  L7_uint32 lsdbType = 0, ipAddr = 0, subnet = 0, advertisement = 0;
  L7_char8  ipAddrBuf[L7_CLI_MAX_STRING_LENGTH], subnetBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 interface, nextInterface, retVal, u, s, p;
  L7_BOOL defaultPassiveMode, intfPassiveMode;
  OSPF_STUB_ROUTER_CFG_t stubRtrMode;
  L7_uint32 startupDuration;
  L7_uint32 overrideSummaryMetric;
  L7_uint32 summaryMetric;
#ifdef L7_NSF_PACKAGE
  OSPF_NSF_SUPPORT_t nsfSupport;
#endif
  OSPF_HELPER_SUPPORT_t helperSupport;
  L7_BOOL strictLsaChecking;

  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_routing_RtrOspf);

  /* For OSPF admin mode we want to display the config whether or not it is at the default value */
  if (usmDbOspfRouterIdGet(unit, &val) == L7_SUCCESS)
  {
    val1 = FD_OSPF_DEFAULT_ROUTER_ID;
    cliShowCmdIpAddr(ewsContext, L7_AF_INET, &val, &val1, pStrInfo_common_RtrId_2);
  }
  areaId = 0;
  neighbor = 0;
  if (usmDbOspfAdminModeGet (unit, &adminVal) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext, adminVal, FD_OSPF_DEFAULT_ADMIN_MODE,
                     pStrInfo_common_Enbl_2);
  }

  if (usmDbOspfRfc1583CompatibilityGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY,
                     pStrInfo_routing_1583compatibility);
  }

  if (usmDbOspfMaxPathsGet(&val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext, val, FD_OSPF_MAXIMUM_PATHS, pStrInfo_common_MaxPaths_1);
  }

  if (usmDbOspfStubRtrModeGet(&stubRtrMode, &startupDuration, 
                              &overrideSummaryMetric, &summaryMetric) == L7_SUCCESS)
  {
    if ((stubRtrMode != FD_OSPF_STUB_ROUTER) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      if (stubRtrMode == OSPF_STUB_RTR_CFG_DISABLED)
      {
        EWSWRITEBUFFER(ewsContext, "no max-metric router-lsa");
      }
      else 
      {
        memset(maxMetricCmd, 0, sizeof(maxMetricCmd));
        if (stubRtrMode == OSPF_STUB_RTR_CFG_ALWAYS)
        {
          osapiSnprintf(maxMetricCmd, sizeof(maxMetricCmd), "max-metric router-lsa");
        }
        else if (stubRtrMode == OSPF_STUB_RTR_CFG_STARTUP)
        {
          osapiSnprintf(maxMetricCmd, sizeof(maxMetricCmd), 
                        "max-metric router-lsa on-startup %u", startupDuration);
        }
        if (overrideSummaryMetric == L7_ENABLE)
        {
          strncat(maxMetricCmd, " summary-lsa", 12);
          if ((summaryMetric != FD_OSPF_STUB_ROUTER_SUMMARY_METRIC) || 
              (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
          {
            L7_uchar8 metricStr[16];
            osapiSnprintf(metricStr, 16, " %u", summaryMetric);
            strncat(maxMetricCmd, metricStr, 16);
          }
        }
        EWSWRITEBUFFER(ewsContext, maxMetricCmd);
      }
    }
  }
  
  if (usmDbOspfOpaqueLsaSupportGet(&val) == L7_SUCCESS)
  {
    memset(stat, 0, sizeof(stat));
    osapiSnprintf(stat, sizeof(stat), pStrInfo_routing_Capability);
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, stat, pStrInfo_routing_Opaque);
    cliShowCmdEnable(ewsContext, val, FD_OSPF_DEFAULT_OPAQUE_LSA_MODE, stat);
  }

  if (usmDbOspfExitOverFlowIntervalGet(unit, &val ) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL,
                  pStrInfo_common_ExitOverflowIntvl_1);
  }
  if (usmDbOspfTrapModeGet(unit, &val, L7_OSPF_TRAP_ALL) == L7_SUCCESS)
  {
   if (adminVal == L7_ENABLE)
    {
      if (( val != FD_OSPF_DEFAULT_TRACE_MODE) ||
          (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
         if (val == L7_ENABLE)
         {
            if(usmDbOspfTrapFlagsGet(unit, &val) == L7_SUCCESS)
            {
               if (val == L7_OSPF_TRAP_ALL)
               {
              EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsAll);
               }
               else
               {
                  if (val & L7_OSPF_TRAP_ERRORS_ALL)
                  {
                     if ((val &  L7_OSPF_TRAP_ERRORS_ALL) == L7_OSPF_TRAP_ERRORS_ALL)
                     {
                       EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext,
                                                  pStrInfo_common_TrapflagsErrsAll);
                     }
                     else
                     {
                        if (val &  L7_OSPF_TRAP_IF_AUTH_FAILURE)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsErrsAuthFailure);
                  }
                        if (val &  L7_OSPF_TRAP_RX_BAD_PACKET)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsErrsBadPkt);
                  }
                        if (val &  L7_OSPF_TRAP_IF_CONFIG_ERROR)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsErrsCfgError);
                  }
                        if (val &  L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsErrsVirtAuthFailure);
                  }
                        if (val &  L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsErrsVirtBadPkt);
                  }
                        if (val &  L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR )
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsErrsVirtCfgError);
                  }
                     }
                  }
                  if (val &  L7_OSPF_TRAP_LSA_ALL)
                  {
                     if ((val &  L7_OSPF_TRAP_LSA_ALL)== L7_OSPF_TRAP_LSA_ALL)
                     {
                  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsLsaAll);
                     }
                     else
                     {
                        if (val &  L7_OSPF_TRAP_MAX_AGE_LSA)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsLsaLsaMaxage);
                  }
                        if (val &  L7_OSPF_TRAP_ORIGINATE_LSA)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsLsaLsaOriginate);
                  }
                     }
                 }
                 if (val & L7_OSPF_TRAP_OVERFLOW_ALL)
                 {
                    if ((val & L7_OSPF_TRAP_OVERFLOW_ALL) ==  L7_OSPF_TRAP_OVERFLOW_ALL)
                    {
                  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsOverflowAll);
                    }
                    else
                    {
                       if (val &  L7_OSPF_TRAP_LS_DB_OVERFLOW)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsOverflowLsdbOverflow);
                  }
                       if (val &  L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsOverflowLsdbApproachingOverflow);
                  }
                    }
                 }
                 if (val & L7_OSPF_TRAP_RETRANSMIT_ALL)
                 {
                    if ((val & L7_OSPF_TRAP_RETRANSMIT_ALL) ==  L7_OSPF_TRAP_RETRANSMIT_ALL)
                    {
                  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsReTxAll);
                    }
                    else
                    {
                       if (val &  L7_OSPF_TRAP_TX_RETRANSMIT)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsReTxPkts);
                  }
                       if (val &  L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsReTxVirtPkts);
                  }
                    }
                }
                if (val & L7_OSPF_TRAP_STATE_CHANGE_ALL)
                {
                   if ((val & L7_OSPF_TRAP_STATE_CHANGE_ALL) == L7_OSPF_TRAP_STATE_CHANGE_ALL)
                   {
                  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsStateChgAll);
                   }
                   else
                   {
                      if (val &  L7_OSPF_TRAP_IF_STATE_CHANGE)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsStateChgIfStateChg);
                  }
                      if (val &  L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsStateChgVirtifStateChg);
                  }
                      if (val &  L7_OSPF_TRAP_NBR_STATE_CHANGE)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsStateChgNeighborStateChg);
                  }
                      if (val &  L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE)
                  {
                    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_TrapflagsStateChgVirtneighborStateChg);
                  }
                   }
                }
             }
          } /*endif usmDbTrapFlagsGet*/
       } /* endif val=L7_ENABLE */
       else
       {
          EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_NoTrapflagsAll);
        }
       }
    }
 }

  rc = usmDbOspfSpfDelayGet(unit, &val );
  rc1 = usmDbOspfSpfHoldtimeGet(unit, &val1);
  if ((rc == L7_SUCCESS) && (rc1 == L7_SUCCESS) &&
      ((val != FD_OSPF_DEFAULT_SPF_DELAY) ||
       (val1 != FD_OSPF_DEFAULT_SPF_HOLDTIME) || (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), 
                            pStrInfo_routing_TimersSpf, val, val1);
    EWSWRITEBUFFER(ewsContext, stat);
  }
  
  /* auto-cost reference-bandwidth */
  rc = usmDbOspfAutoCostRefBwGet(&val);
  if (rc == L7_SUCCESS)
  {
    memset(stat, 0, sizeof(stat));
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_AutoCost);
    OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, stat, pStrInfo_common_RefBandwidth);
    cliShowCmdInt (ewsContext, val, FD_OSPF_DEFAULT_REFER_BANDWIDTH, stat); 

  }

  /* Display OSPF area commands */
  rc = usmDbOspfAreaCfgGetFirst(unit, &areaId);
  while (rc == L7_SUCCESS)
  {
    memset(areaIdBuf, 0, sizeof(areaIdBuf));
    usmDbInetNtoa(areaId, areaIdBuf);
    if ((usmDbOspfStubMetricGet(unit, areaId, L7_TOS_NORMAL_SERVICE, &val) == L7_SUCCESS))
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaDeflCost, areaIdBuf);
      cliShowCmdInt (ewsContext, val, FD_OSPF_AREA_DEFAULT_STUB_METRIC, stat);
    }
    if (usmDbOspfNSSADefaultInfoGet (unit, areaId, &defInfoOrg, &metricType, &metric) == L7_SUCCESS)
    {
      if (defInfoOrg)
      {
        if ((defInfoOrg != FD_OSPF_NSSA_DEFAULT_DEFAULT_INFO_ORIGINATE) ||
            (metricType != FD_OSPF_NSSA_DEFAULT_METRIC_TYPE)||
            (metric != FD_OSPF_NSSA_DEFAULT_METRIC ) ||
            (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
        {
          memset(stat, 0, sizeof(stat));
          if (metricType == L7_OSPF_AREA_STUB_COMPARABLE_COST)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                    pStrInfo_common_DispOspfAreaNssaInfoOr,
                    areaIdBuf, metric);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),
                                    pStrInfo_common_DispOspfAreaNssaInfoOr_1,
                    areaIdBuf, metric);
          }
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
      else if ((defInfoOrg != FD_OSPF_NSSA_DEFAULT_DEFAULT_INFO_ORIGINATE) ||
               (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_DispOspfAreaNssaInfoOr_2, areaIdBuf);
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }

    if (usmDbOspfNSSARedistributeGet(unit, areaId, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaNssaNoRedistribute, areaIdBuf );
      cliShowCmdDisable(ewsContext,val,FD_OSPF_NSSA_DEFAULT_REDISTRIBUTE,stat);
    }
    if (usmDbOspfNSSAImportSummariesGet(unit, areaId, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_DispOspfAreaNssaSummary, areaIdBuf );
      cliShowCmdDisable(ewsContext,val,FD_OSPF_NSSA_DEFAULT_IMPORT_SUMMARIES,stat);
    }

    if ((usmDbOspfNSSATranslatorRoleGet(unit, areaId, &val) == L7_SUCCESS) &&
        ((val != FD_OSPF_NSSA_DEFAULT_TRANSLATOR_ROLE) ||
         (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      memset(stat, 0,sizeof(stat));
      switch (val)
      {
      case L7_OSPF_NSSA_TRANSLATOR_ALWAYS:
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_DispOspfAreaNssaTransRoleAlways, areaIdBuf );
        break;
      case L7_OSPF_NSSA_TRANSLATOR_CANDIDATE:
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_DispOspfAreaNssaTransRole, areaIdBuf );
        break;
      }
      EWSWRITEBUFFER(ewsContext, stat);
    }

    if ((usmDbOspfNSSATranslatorStabilityIntervalGet(unit, areaId, &val) == L7_SUCCESS))
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaNssaTranslatorStabIntv, areaIdBuf);
      cliShowCmdInt (ewsContext, val, FD_OSPF_NSSA_DEFAULT_TRANSLATOR_STABILITY_INTERVAL,stat);
    }
    aggrAreaId = areaId;
    ipAddr = 0;
    subnet = 0;
    lsdbType = 0;
    while (usmDbOspfAreaAggregateEntryNext(unit, &aggrAreaId, &lsdbType, &ipAddr,
          &subnet) == L7_SUCCESS)
    {
      usmDbOspfAreaAggregateEffectGet(unit, areaId, lsdbType, ipAddr, subnet,
          &advertisement);
      memset(ipAddrBuf, 0, sizeof(ipAddrBuf));
      usmDbInetNtoa(ipAddr,ipAddrBuf);
      memset(subnetBuf, 0, sizeof(subnetBuf));
      usmDbInetNtoa(subnet, subnetBuf);
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_AreaRange, areaIdBuf, ipAddrBuf, subnetBuf );
      switch (lsdbType)
      {
      case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK:
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_SummaryLink_1);
        break;
      case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK:
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_NssaExternalLink);
        break;
      }
      if (advertisement == L7_NULL)
      {
        advertisement = USMDB_OSPF_AREA_DEFAULT_ADVERTISE_AREA;
      }

      if (advertisement == L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING)
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Advertise_1);
      }
      else
      {
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_NoAdvertise);
      }

      EWSWRITEBUFFER(ewsContext, stat);

    } /* end of 'while' loop */

    if ((strcmp(areaIdBuf, pStrInfo_common_0000) != 0) &&
        (usmDbOspfImportAsExternGet(unit, areaId, &val) == L7_SUCCESS) &&
        ((val != FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY) ||
         (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
    {
      memset(stat, 0,sizeof(stat));
      switch (val)
      {
      case L7_OSPF_AREA_IMPORT_NSSA:
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_DispOspfAreaNssa, areaIdBuf );
        break;
      case L7_OSPF_AREA_IMPORT_NO_EXT:
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_DispOspfAreaStub, areaIdBuf );
        break;
      }
      EWSWRITEBUFFER(ewsContext, stat);
    }

    if (usmDbOspfMapAreaSummaryGet(unit,areaId, &ival) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_DispOspfAreaSummaryLsa, areaIdBuf );
      cliShowCmdDisable(ewsContext,ival,FD_OSPF_AREA_DEFAULT_STUB_SUMMARY_LSA,stat);
    }

    rc = usmDbOspfAreaCfgGetNext(unit, areaId, &areaId);
  }

  /* Display OSPF virtual link commands */
  areaId = 0;
  neighbor = 0;
  virt_flag = usmDbOspfVirtIfEntryNext(unit, &areaId, &neighbor);
  while (virt_flag == L7_SUCCESS)
  {
    memset(stat, 0,sizeof(stat));
    memset(buf1, 0,sizeof(buf1));
    if (usmDbInetNtoa(areaId, buf1) == L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_Area_3, buf1);
    }

    memset(buf2, 0,sizeof(buf2));
    if (usmDbInetNtoa(neighbor, buf2) == L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_common_VirtualLink_2, buf2);
      OSAPI_STRNCAT(stat, buf);
    }
    EWSWRITEBUFFER(ewsContext, stat);

    if (usmDbOspfVirtIfHelloIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaVirtualLinkHelloIntvl, buf1, buf2);
      cliShowCmdInt (ewsContext, val, FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL, stat);
    }

    if (usmDbOspfVirtIfRetransIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaVirtualLinkReTxIntvl, buf1, buf2);
      cliShowCmdInt (ewsContext, val, FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL, stat);
      }

    if (usmDbOspfVirtIfTransitDelayGet (unit, areaId, neighbor, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaVirtualLinkTxDelay, buf1, buf2);
      cliShowCmdInt (ewsContext, val, FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY, stat);
    }

    if (usmDbOspfVirtIfRtrDeadIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_AreaVirtualLinkDeadIntvl, buf1, buf2);
      cliShowCmdInt (ewsContext, val, FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL, stat);
      }

    if ((usmDbOspfVirtIfAuthTypeGet (unit, areaId, neighbor, &authType) == L7_SUCCESS) &&
        (authType != FD_OSPF_INTF_DEFAULT_AUTH_TYPE))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_AreaVirtualLinkAuth, buf1, buf2,
                              strUtilAuthTypeGet(authType, ""));
    }
    else
    {
      stat[0] = 0;
    }

    /* Auth Key */
    if (authType == L7_AUTH_TYPE_SIMPLE_PASSWORD || authType == L7_AUTH_TYPE_MD5)
    {
      if (usmDbOspfVirtIfAuthKeyActualGet(unit, areaId, neighbor, authKey) == L7_SUCCESS)
      {
        if (cliUserNumberGet()==0)
        {
          osapiSnprintf(buf, sizeof(buf),  " %s ", authKey);
          OSAPI_STRNCAT(stat, buf);
        }
      }
      /* Auth Key ID */
      if (authType == L7_AUTH_TYPE_MD5)
      {
        if (usmDbOspfVirtIfAuthKeyIdGet(unit, areaId, neighbor, &val) == L7_SUCCESS)
        {
          osapiSnprintf(buf, sizeof(buf),  " %u ", val);
          OSAPI_STRNCAT(stat, buf);
        }
      }
    }
    EWSWRITEBUFFER(ewsContext, stat);

    virt_flag = usmDbOspfVirtIfEntryNext(unit, &areaId, &neighbor);
  }

  /* Display OSPF Network Area commands */
  ipAddr = wildcardMask = areaId = 0;
  rc = usmDbOspfNetworkAreaEntryNext(unit, &ipAddr, &wildcardMask, &areaId);
  while (rc == L7_SUCCESS)
  {
    memset(buf, 0, sizeof(buf));
    usmDbInetNtoa(ipAddr, buf);

    memset(buf1, 0, sizeof(buf1));
    usmDbInetNtoa(wildcardMask, buf1);

    memset(buf2, 0, sizeof(buf2));
    usmDbInetNtoa(areaId, buf2);

    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_NwArea, buf, buf1, buf2);
    EWSWRITEBUFFER(ewsContext, stat);

    rc = usmDbOspfNetworkAreaEntryNext(unit, &ipAddr, &wildcardMask, &areaId);
  }

  if (usmDbOspfExtLsdbLimitGet (unit, &ival ) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,ival,FD_OSPF_DEFAULT_EXT_LSDB_LIMIT,pStrInfo_common_ExternalLsdbLimit);
  }

#ifdef L7_NSF_PACKAGE
  if (usmDbOspfNsfSupportGet(&nsfSupport) == L7_SUCCESS)
  {
    if (nsfSupport != FD_OSPF_NSF_SUPPORT)
    {
      if (nsfSupport == OSPF_NSF_NONE)
        EWSWRITEBUFFER(ewsContext, "no nsf");
      if (nsfSupport == OSPF_NSF_PLANNED_ONLY)
        EWSWRITEBUFFER(ewsContext, "nsf planned-only");
      else if (nsfSupport == OSPF_NSF_ALWAYS)
        EWSWRITEBUFFER(ewsContext, "nsf");
    }
  }

  if (usmDbOspfRestartIntervalGet(&val) == L7_SUCCESS)
  {
    if (val != FD_OSPF_RESTART_INTERVAL)
    {
      osapiSnprintf(stat, sizeof(stat), "nsf restart-interval %u", val);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }
#endif

  if (usmDbOspfHelperSupportGet(&helperSupport) == L7_SUCCESS)
  {
    if (helperSupport != FD_OSPF_HELPER_SUPPORT)
    {
      if (helperSupport == OSPF_HELPER_NONE)
        EWSWRITEBUFFER(ewsContext, "no nsf helper");
      else if (helperSupport == OSPF_HELPER_PLANNED_ONLY)
        EWSWRITEBUFFER(ewsContext, "nsf helper planned-only");
      else if (helperSupport == OSPF_HELPER_ALWAYS)
        EWSWRITEBUFFER(ewsContext, "nsf helper");
    }
  }

  if (usmDbOspfStrictLsaCheckingGet(&strictLsaChecking) == L7_SUCCESS)
  {
    if (strictLsaChecking != FD_OSPF_STRICT_LSA_CHECKING)
    {
    if (strictLsaChecking)
      EWSWRITEBUFFER(ewsContext, "nsf helper strict-lsa-checking");
    else 
      EWSWRITEBUFFER(ewsContext, "no nsf helper strict-lsa-checking");
    }
  }

  if (usmDbOspfPassiveModeGet (unit, &defaultPassiveMode) == L7_SUCCESS)
  {
    if (defaultPassiveMode == L7_TRUE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_PassiveIntfDefl);
    }
  }

  if (usmDbValidIntIfNumFirstGet(&interface) == L7_SUCCESS)
  {
    while (interface != 0)
    {
      if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) == L7_SUCCESS)
      {
        if((usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS) &&
            (cliSlotPortCpuCheck(s, p) == L7_SUCCESS))
        {
          /* get Interface routing status */
          if (usmDbIpRtrIntfModeConfigurable(unit, interface) == L7_TRUE)
          {
            if(usmDbOspfIntfPassiveModeGet(unit, interface, &intfPassiveMode) == L7_SUCCESS)
            {
               if((defaultPassiveMode == L7_TRUE) && (intfPassiveMode == L7_FALSE))
               {
                osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_NoPassiveIntf,cliDisplayInterfaceHelp(u,s,p));
                  EWSWRITEBUFFER(ewsContext, buf);
               }
               else if((defaultPassiveMode == L7_FALSE) && (intfPassiveMode == L7_TRUE))
               {
                osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_PassiveIntf_1,cliDisplayInterfaceHelp(u,s,p));
                  EWSWRITEBUFFER(ewsContext, buf);
               }
            }
          }
        }
      }
      if ( usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS )
      {
         interface = nextInterface;
      }
      else
      {
         interface = 0;
    }
  }
  }

  cliRunningConfigOspfRedistribute(ewsContext, unit);
  cliRunningConfigOspfDistance(ewsContext, unit);
  EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_Exit); /*exit from router ospf */
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of OSPF Redistribute Operation
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t cliRunningConfigOspfRedistribute(EwsContext ewsContext, L7_uint32 unit)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 protocolString[L7_CLI_MAX_STRING_LENGTH];
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 val, metric, metricType, tag, subnets, always;
  L7_BOOL mode = L7_DISABLE, metricFlag = L7_DISABLE, mtypeFlag = L7_DISABLE,
          tagFlag = L7_DISABLE, subnetFlag = L7_DISABLE;

  if (usmDbOspfDefaultMetricGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt (ewsContext, val, FD_OSPF_DEFAULT_METRIC, pStrInfo_common_DeflMetric_1);
    }

  memset(stat, 0,sizeof(stat));
  if ((usmDbOspfDefaultRouteOrigGet(unit, &val) == L7_SUCCESS))
  {
    if (val != FD_OSPF_ORIG_DEFAULT_ROUTE)
    {
      if ((usmDbOspfDefaultRouteAlwaysGet(unit, &always) == L7_SUCCESS))
      {
        if ((always != FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          OSAPI_STRNCAT_ADD_BLANKS (0, 0, 1, 0, L7_NULLPTR, stat, pStrInfo_common_Always_1);
        }
      }
      if ((usmDbOspfDefaultRouteMetricGet(unit, &metric) == L7_SUCCESS))
      {
        if ((metric != FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_common_DispOspfMetric, metric);
          OSAPI_STRNCAT(stat, buf);
        }
      }
      if ((usmDbOspfDefaultRouteMetricTypeGet(unit, &metricType) == L7_SUCCESS))
      {
        if ((metricType != FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_common_DispOspfMetricType, metricType);
          OSAPI_STRNCAT(stat, buf);
        }
      }
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_DispOspfDeflInfo);
      OSAPI_STRNCAT(buf, stat);
      EWSWRITEBUFFER(ewsContext, buf);
    }

  }

  for (protocol = 1; protocol<=5; protocol++)
  {
    memset(stat, 0,sizeof(stat));
    memset(protocolString, 0,sizeof(protocolString));
    if (protocol != REDIST_RT_OSPF)
    {
      memset(stat, 0,sizeof(stat));
      memset(protocolString, 0,sizeof(protocolString));
      switch (protocol)
      {
      case REDIST_RT_LOCAL:
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_DispRedistributeConnected);
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Connected_1);
        break;

      case REDIST_RT_STATIC:
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_DispOspfRedistributeStat);
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Static2);
        break;

      case REDIST_RT_RIP:
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_DispOspfRedistributeRip);
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Rip3);
        break;

      case REDIST_RT_BGP:
        if (usmDbComponentPresentCheck(unit, L7_FLEX_BGP_MAP_COMPONENT_ID) != L7_TRUE)
        {
          continue;
        }

        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_routing_DispOspfRedistributeBgp);
        osapiSnprintf(protocolString, sizeof(protocolString), pStrInfo_common_Bgp_1);
        break;
        /* include these to keep the compiler happy */
      case REDIST_RT_FIRST:
      case REDIST_RT_LAST:
      case REDIST_RT_OSPF:
      case REDIST_RT_OSPFV3:
        break;
      }

      if (usmDbOspfRedistributeGet(unit, protocol, &val) == L7_SUCCESS)
      {
        if ((val != FD_OSPF_REDISTRIBUTE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          if (val != FD_OSPF_REDISTRIBUTE)
          {
            mode = L7_ENABLE;
            if (usmDbOspfRedistMetricGet(unit, protocol, &metric) == L7_SUCCESS)
            {
              if ((metric != FD_OSPF_REDIST_METRIC)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_DispOspfMetric, metric);
                OSAPI_STRNCAT(stat, buf);
                metricFlag = L7_ENABLE;
              }
              }

            if (usmDbOspfRedistMetricTypeGet(unit, protocol, &metricType) == L7_SUCCESS)
            {
              if ((metricType != FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_DispOspfMetricType, metricType);
                OSAPI_STRNCAT(stat, buf);
                mtypeFlag = L7_ENABLE;
              }
            }

            if (usmDbOspfTagGet(unit, protocol, &tag)  == L7_SUCCESS)
            {
              if ((tag != FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_ROUTE_TAG)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
                osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_Tag_2, tag);
                OSAPI_STRNCAT(stat, buf);
                tagFlag = L7_ENABLE;
              }
            }

            if (usmDbOspfRedistSubnetsGet(unit, protocol, &subnets) == L7_SUCCESS)
            {
              if ((subnets != FD_OSPF_REDIST_SUBNETS)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
              {
                osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Subnets_1);
                OSAPI_STRNCAT(stat, buf);
                subnetFlag = L7_ENABLE;
              }
            }
            EWSWRITEBUFFER(ewsContext, stat);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_routing_NoRedistribute, protocolString);
            EWSWRITEBUFFER(ewsContext, buf);
          }
          }
        }

      if (usmDbOspfDistListGet(unit, protocol, &val) == L7_SUCCESS)
      {
        if ((val != FD_OSPF_DIST_LIST_OUT)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintf(buf, sizeof(buf), "%d",val);
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_common_DispOspfDistributeList, buf, protocolString);
          EWSWRITEBUFFER(ewsContext, stat);
        }
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of distance ospf Info
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

L7_RC_t cliRunningConfigOspfDistance(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 pref;

  if (usmDbComponentPresentCheck(unit, L7_OSPF_MAP_COMPONENT_ID) == L7_TRUE)
  {
    if (usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_OSPF_EXTERNAL, &pref) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,pref,FD_RTR_RTO_PREF_OSPF_EXTERNAL,pStrInfo_common_DistanceOspfExternal);
      }

    if (usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_OSPF_INTER_AREA, &pref) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,pref,FD_RTR_RTO_PREF_OSPF_INTER,pStrInfo_common_DistanceOspfInterArea);
      }

    if (usmDbIpRouterPreferenceGet(unit, ROUTE_PREF_OSPF_INTRA_AREA, &pref) == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext,pref,FD_RTR_RTO_PREF_OSPF_INTRA,pStrInfo_common_DistanceOspfIntraArea);
    }

  }
  return L7_SUCCESS;
}


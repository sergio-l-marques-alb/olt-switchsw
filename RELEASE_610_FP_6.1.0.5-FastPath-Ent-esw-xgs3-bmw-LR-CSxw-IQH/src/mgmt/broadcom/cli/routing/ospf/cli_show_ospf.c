/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/ospf/cli_show_ospf.c
 *
 * @purpose show commands for the OSPF
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
#include "usmdb_mib_ospf_api.h"
#include "cli_web_exports.h"
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

/*********************************************************************
 *
 * @purpose   displays a list of router configuration options
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
 * @cmdsyntax   show ip ospf
 *
 * @cmdhelp
 *
 * @cmddescript  This commands takes no options, contains all info for routerid, tosrouting, ospf:::
 *               admin-mode <enable/disable>, abr mode <true/false>(AreaBdrRtrStatus),
 *               asbr mode <enable/disable>; external LSA count, external LSA checksum,
 *               new LSAs originated, LSAs recived, external LSA limit (all int) for this see virata.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspf(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)

{
  L7_uint32 val, adminMode;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, metricType, tag;
  L7_uint32 metric, defaultRoute, filterValue, value;
  L7_REDIST_RT_INDICES_t protocol;
  L7_BOOL present, subnets, passiveMode;
  L7_ospfStatus_t ospfStatus;
  L7_RC_t rc;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_ShowRtrOspfInfo_1);
  }

  /*----------------------------------------------------------*/
  /*  Start of OSPF Config information                        */
  /*----------------------------------------------------------*/

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* Router ID */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_RtrId_1);
  if ((usmDbOspfRouterIdGet(unit, &val) == L7_SUCCESS) &&
      (usmDbInetNtoa(val, stat) == L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_common_Err);
  }                                                          /* Err */

  /* OSPF Admin Mode */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_OspfAdminMode);
  if (usmDbOspfAdminModeGet (unit, &adminMode) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(adminMode, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* 1583 Compat */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_Rfc1583Compatibility);    /* RFC 1583 Compatibility */
  if (usmDbOspfRfc1583CompatibilityGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Ext LSDB Limit */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ExternalLinkStateDbaseLimit);
  if (usmDbOspfExternalLSALimitGet(unit, &val)== L7_SUCCESS)                  /* External LSA Limit */
  {
    if (val == -1)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_NoLimit_1);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat),  pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Ext LSDB Exit Overflow Interval */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ExitOverflowIntvl);
  if (usmDbOspfExitOverFlowIntervalGet(unit, &val ) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat),  pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* SPF Delay */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_SpfDelayIntvl);
  if (usmDbOspfSpfDelayGet(unit, &val ) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat),  pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* SPF Hold */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_SpfHoldTime);
  if (usmDbOspfSpfHoldtimeGet(unit, &val ) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat),  pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Opaque LSA support */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_OspfOpaqueLsaCapability);    /* Opaque Capability */
  if (usmDbOspfOpaqueLsaSupportGet(&val) == L7_SUCCESS)
  {
    sprintf(stat,strUtilEnableDisableGet(val, pStrInfo_common_Blank));
  }
  else
  {
    sprintf(stat, pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Displaying Auto-cost Reference-Bandwidth */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext,  pStrInfo_common_AutoCostRef);

  if ((usmDbOspfAutoCostRefBwGet(&value) == L7_SUCCESS))
  {
    sprintf(stat, "%u Mbps", value);
  }
  else
  {
    sprintf(stat, pStrErr_common_Err);
  }

  ewsTelnetWrite(ewsContext, stat);

  /* Interfaces passive by default or not */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext,  pStrInfo_common_Pass);

  if (usmDbOspfPassiveModeGet(unit, &passiveMode) == L7_SUCCESS)
  {
    sprintf(stat,strUtilEnabledDisabledGet(passiveMode,pStrInfo_common_Dsbld));

  }
  else
  {
    sprintf(stat, pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Max paths */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_MaxPaths);
  rc = usmDbOspfMaxPathsGet(&value);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", value);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Default Metric */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_DeflMetric);

  rc =  usmDbOspfDefaultMetricGet(unit,&metric);

  if (rc == L7_NOT_EXIST)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_NotCfgured);
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
  ewsTelnetWrite(ewsContext, stat);

  /*Default Route Advertise*/
  cliSyntaxTop(ewsContext);
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_DeflRouteAdvertise);
  if (usmDbOspfDefaultRouteOrigGet(unit,&defaultRoute) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(defaultRoute,pStrInfo_common_Dsbld));
  }
    else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /*Always*/
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_Always);
  if (usmDbOspfDefaultRouteAlwaysGet(unit,&val) == L7_SUCCESS)
  {
    if (val == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_True);
    }
    else if (val == L7_FALSE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_False);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /*Metric*/
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_Metric);
  rc = usmDbOspfDefaultRouteMetricGet(unit, &val);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat), "%u",val);
  }

  else if (rc == L7_NOT_EXIST)
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_NotCfgured);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /*Metric Type*/
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_MetricType);
  if (usmDbOspfDefaultRouteMetricTypeGet(unit,&val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_OSPF_METRIC_EXT_TYPE1:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_OspfMetricExtType1);
      break;
    case L7_OSPF_METRIC_EXT_TYPE2:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_OspfMetricExtType2);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
      break;
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);


  /* Redistribution Config */
  cliSyntaxTop(ewsContext);
  for (protocol = 1; protocol < 6; protocol++)
  {
    if (usmDbOspfRedistributeGet(unit,protocol,&present) == L7_SUCCESS)
    {
      if (present == L7_TRUE)
      {
        cliSyntaxTop(ewsContext);
        cliFormat(ewsContext, pStrInfo_common_Redistribute);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_Src);

        switch (protocol)
        {
        case REDIST_RT_BGP:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Bgp_1);
          break;
        case REDIST_RT_RIP:
          osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Rip3);
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
        rc = usmDbOspfRedistMetricGet(unit,protocol,&metric);
        if (rc == L7_NOT_EXIST)
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_ApProfileNameNotCfgured);
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
        ewsTelnetWrite(ewsContext, stat);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_MetricType);
        if (usmDbOspfRedistMetricTypeGet(unit,protocol,&metricType) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u", metricType);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_Tag);
        if (usmDbOspfTagGet(unit,protocol,&tag) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat),  "%u", tag);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_routing_Subnets);
        if (usmDbOspfRedistSubnetsGet(unit,protocol,&subnets) == L7_SUCCESS)
        {
          if (subnets == L7_TRUE)
          {
            osapiSnprintf(stat, sizeof(stat),pStrInfo_common_Yes);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), pStrInfo_common_No);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);

        memset (stat, 0, sizeof(stat));
        cliFormat(ewsContext, pStrInfo_common_DistributeList);
        rc = usmDbOspfDistListGet(unit,protocol,&filterValue);

        if (rc == L7_NOT_EXIST)
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
 
  /* check if ospf is enabled. If not, don't attempt to show status information. */
  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    if (adminMode == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgRtrId_1);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspf);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  
  /* Status Information */

  if (usmDbOspfStatusGet(&ospfStatus) == L7_SUCCESS)
  {
    /* Report number of active areas of each type */
    cliSyntaxTop(ewsContext);
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, "Number of Active Areas");   
    osapiSnprintf(stat, sizeof(stat), "%u (%u normal, %u stub, %u nssa)",
                  ospfStatus.normalAreas + ospfStatus.stubAreas + ospfStatus.nssaAreas,
                  ospfStatus.normalAreas, ospfStatus.stubAreas, ospfStatus.nssaAreas);
    ewsTelnetWrite(ewsContext, stat);

    /* ABR Status */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_AbrStatus);   
    val = ospfStatus.abr ? L7_ENABLE : L7_DISABLE;
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
    ewsTelnetWrite(ewsContext, stat);
  
    /* ASBR Status */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_AsbrMode);      
    val = ospfStatus.asbr ? L7_ENABLE : L7_DISABLE;
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
    ewsTelnetWrite(ewsContext, stat);
  
    /* Stub Router */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_OspfStubRouter);                           
    if (ospfStatus.stubRouter == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_True);
    }
    else 
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_False);
    }
    ewsTelnetWrite(ewsContext, stat);
  
  
    /* Ext LSDB Overflow */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_OspfExtLsdbOverflow);                           
    if (ospfStatus.extLsdbOverflow == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_True);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_False);
    }
    ewsTelnetWrite(ewsContext, stat);
  
    /* Ext LSA Count */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_ExternalLsaCount);
    osapiSnprintf(stat, sizeof(stat),  "%u", ospfStatus.extLsaCount);
    ewsTelnetWrite(ewsContext, stat);
  
    /* Ext LSA Checksum */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_ExternalLsaChecksum);
    osapiSnprintf(stat, sizeof(stat),  "%#x", ospfStatus.extLsaChecksum);
    ewsTelnetWrite(ewsContext, stat);
  
    /* AS Opaque LSA Count */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_routing_AsOpaqueLsaCount);
    sprintf(stat, "%u", ospfStatus.asOpaqueLsaCount);
    ewsTelnetWrite(ewsContext, stat);
  
    /* AS Opaque Checksum */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_routing_AsOpaqueLsaChecksum);
    sprintf(stat, "%u", ospfStatus.asOpaqueLsaChecksum);
    ewsTelnetWrite(ewsContext, stat);
  
    /* Number of originated LSAs */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_NewLsasOriginated);
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.lsasOrig);
    ewsTelnetWrite(ewsContext, stat);
  
    /* Number of received LSAs */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_LsasRcvd);
    osapiSnprintf(stat, sizeof(stat),  "%u", ospfStatus.lsasReceived);
    ewsTelnetWrite(ewsContext, stat);

    /* Current number of LSAs in LSDB */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_LsaCount);
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.lsaCount);
    ewsTelnetWrite(ewsContext, stat);

    /* Max number of LSAs in LSDB */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_LsaCapacity);
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.lsaCapacity);
    ewsTelnetWrite(ewsContext, stat);

    /* LSA high water mark */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_LsaHighWater);
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.lsaHighWater);
    ewsTelnetWrite(ewsContext, stat);

    /* Current number of retransmit entries */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, "Retransmit List Entries");
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.retxEntries);
    ewsTelnetWrite(ewsContext, stat);

    /* Max number of retransmit entries */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, "Maximum Number of Retransmit Entries");
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.retxCapacity);
    ewsTelnetWrite(ewsContext, stat);

    /* Retransmit entries high water mark */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, "Retransmit Entries High Water Mark");
    osapiSnprintf(stat, sizeof(stat), "%u", ospfStatus.retxHighWater);
    ewsTelnetWrite(ewsContext, stat);
  }
  
    return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose    Display IFO information
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
 * @cmdsyntax    show ip ospf interface <slot/port>
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfInterface(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 intAreaId;
  L7_uint32 val;
  L7_uint32 adminMode;
  L7_uint32 mtuIgnore;
  L7_uint32 s, i, p;
  L7_IP_MASK_t primaryMask = 0;
  L7_IP_ADDR_t primaryAddr = 0;
  L7_IP_MASK_t mask = 0;
  L7_IP_ADDR_t ipAddr = 0;
  L7_uint32 intIface;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_RC_t   rc;
  L7_uint32 unit;
  L7_uint32 authType;
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackID;
#endif
  L7_rtrIntfIpAddr_t ipAddrList[L7_L3_NUM_IP_ADDRS];
  L7_uint32 networkType;      /* OSPF network type for this interface */
  L7_BOOL isUnnumbered;
  L7_uint32 numberedIfc;
  L7_uint32 numbUnit, numbSlot, numbPort;
  L7_BOOL passiveMode;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  if(cliNumFunctionArgsGet() > 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_ShowRtrOspfIntfInfo, cliSyntaxInterfaceHelp());
  }
  else
  {
    if((strcmp(argv[index], RLIM_INTF_LOOPBACK_NAME_PREFIX) == 0))
    {
#ifdef L7_RLIM_PACKAGE
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
#endif
    }
    else
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
      { /* NOTE: No need to check the value of `unit` as
         *       ID of a standalone switch is always `U_IDX` (=> 1).  */
        unit = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &s, &p, &intIface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }

      if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
      {
        return cliPrompt(ewsContext);
      }

    }
  }

  /*-----------------------------------------------------*/
  /*  START OF OSPF INTERFACE CONFIGURATION INFORMATION  */
  /*-----------------------------------------------------*/

  memset (stat, 0, sizeof(stat));
  /* check if ospf is enable and if it is in addition enable per interface */
  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    cliSyntaxTop(ewsContext);
    rc = usmDbOspfAdminModeGet (unit, &val);
    if (val != L7_ENABLE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                             pStrErr_common_MustInitOspf);
    }
  }

  if ((usmDbIpUnnumberedGet(intIface, &isUnnumbered, &numberedIfc) == L7_SUCCESS) &&
      (isUnnumbered == L7_TRUE))
  {
    cliFormat(ewsContext, pStrInfo_routing_Unnumbered);
    if (usmDbUnitSlotPortGet(numberedIfc, &numbUnit, &numbSlot, &numbPort) == L7_SUCCESS)
      sprintf(stat, "%s", cliGetIntfName(numberedIfc, numbUnit, numbSlot, numbPort));
    else
      ewsTelnetPrintf (ewsContext, "%13.12s", pStrInfo_common_Blank);
  }
  else
  {
    if (usmDbIpRtrIntfIpAddressGet(unit, intIface, &primaryAddr,
                                   &primaryMask) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_common_ApIpAddr);                          /* ip addr */
      memset (stat, 0, sizeof(stat));
        rc = usmDbInetNtoa(primaryAddr, stat);

      ewsTelnetWrite(ewsContext, stat);
      cliFormat(ewsContext, pStrInfo_common_SubnetMask_3);                         /* subnet mask */
      memset (stat, 0, sizeof(stat));
        rc = usmDbInetNtoa(primaryMask, stat);
      ewsTelnetWrite(ewsContext, stat);
    }
    else
    {
      cliFormat(ewsContext,pStrInfo_common_ApIpAddr);
      ewsTelnetWrite(ewsContext, pStrErr_common_Err);                    /* Err */

      cliFormat(ewsContext, pStrInfo_common_SubnetMask_3);
      ewsTelnetWrite(ewsContext, pStrErr_common_Err);
    }
  }

  cliFormat(ewsContext, pStrInfo_routing_IpAddresSecondary);
  /* Secondary Ip Address */
  if (usmDbIpRtrIntfCfgIpAddrListGet(unit, intIface, ipAddrList) == L7_SUCCESS)
  {
    for (i = 1; i <= L7_L3_NUM_IP_ADDRS - 1; i++)
    {
      ipAddr = ipAddrList[i].ipAddr;
      mask = ipAddrList[i].ipMask;

      memset(stat, 0, sizeof(stat));
      memset(buf, 0, sizeof(buf));
      if ((ipAddr != L7_NULL_IP_ADDR) && (usmDbInetNtoa(ipAddr, buf) == L7_SUCCESS))
      {
        if(i > 1)
        {
          OSAPI_STRNCAT(stat, "\r\n                                                ");
        }

        OSAPI_STRNCAT(stat, buf);
        memset(buf, 0, sizeof(buf));
        if ((mask != L7_NULL_IP_MASK) && (usmDbInetNtoa(mask, buf) == L7_SUCCESS))
        {
          OSAPI_STRNCAT(stat, "  ");
          OSAPI_STRNCAT(stat, buf);
        }
      }

      if (strlen(stat) > 0)
      {
        ewsTelnetWrite(ewsContext, stat);
      }
    }
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_OspfAdminMode);
  if (usmDbOspfIntfAdminModeGet(unit, intIface, &adminMode) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(adminMode, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_OspfAreaId);                           /* get the AREA ID */
  if ((usmDbOspfIntfAreaIdGet(unit, intIface, &intAreaId) == L7_SUCCESS) &&
      (usmDbInetNtoa(intAreaId, stat) == L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_common_Err);
  }
  /* Network type */
  bzero(stat, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_OspfNetworkType);
  if (usmDbOspfIfTypeGet(intIface, &networkType) == L7_SUCCESS)
  {
    switch (networkType)
    {
    case L7_OSPF_INTF_BROADCAST:
      sprintf(stat, "%s", pStrInfo_routing_Broadcast);
      break;
    case L7_OSPF_INTF_NBMA:
      sprintf(stat, "%s", pStrInfo_routing_Nbma);
      break;
    case L7_OSPF_INTF_PTP:
      sprintf(stat, "%s", pStrInfo_routing_P2P);
      break;
    case L7_OSPF_INTF_VIRTUAL:
      sprintf(stat, "%s", pStrInfo_routing_Virt);
      break;
    case L7_OSPF_INTF_POINTTOMULTIPOINT:
      sprintf(stat, "%s", pStrInfo_routing_P2MP);
      break;
    case L7_OSPF_INTF_TYPE_LOOPBACK:
      sprintf(stat, "%s", pStrInfo_routing_Loopback);
      break;
    default:
      sprintf(stat, "%s", pStrInfo_common_Blank);
    }
  }
  else
  {
    sprintf(stat, "%s", pStrInfo_common_Blank);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_RtrPri);
  if (usmDbOspfIntfPriorityGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ReTxIntvl);
  if (usmDbOspfIntfRxmtIntervalGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_HelloIntvl);
  if (usmDbOspfIntfHelloIntervalGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_DeadIntvl);
  if (usmDbOspfIntfDeadIntervalGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

#if L7_OSPF_NBMA_SUPPORTED

  /*
     NBMA is not a part of LVL7's tested reference architecture.
   */

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_NbmaPollIntvl);
  if (usmDbOspfIntfNbmaPollIntervalGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

#endif

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_LsaAckIntvl);
  if (usmDbOspfIntfLSAAckIntervalGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_IfTransitDelayIntvl);
  if (usmDbOspfIntfTransitDelayGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AuthType_1);
  if (usmDbOspfIntfAuthTypeGet(unit, intIface, &authType) == L7_SUCCESS)
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
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Auth Key */
  if (authType == L7_AUTH_TYPE_SIMPLE_PASSWORD || authType == L7_AUTH_TYPE_MD5)
  {
    cliFormat(ewsContext, pStrInfo_common_AuthKey);
    if (usmDbOspfIntfAuthKeyActualGet(unit, intIface, authKey) == L7_SUCCESS)
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
    if (usmDbOspfIntfAuthKeyIdGet(unit, intIface, &val) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
    }
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_MetricCost);
  if (usmDbOspfIfMetricValueGet(unit, intIface, 0, &val) == L7_SUCCESS)
  {
    if(usmDbOspfIsIfMetricConfigured(unit, primaryAddr, intIface, 0) == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_Cfgured, val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  pStrInfo_common_Computed, val);
    }

    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }

  /* If the interface status is passive, display the passive status */
  if (usmDbOspfIntfPassiveModeGet(unit, intIface, &passiveMode) == L7_SUCCESS)
  {
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_OspfPass_2);
    if (passiveMode == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_PassIntf);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_NonPass);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  /* Displaying OSPF Mtu-ignore */
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_OspfMtuIgnore);

  if (usmDbOspfIntfMtuIgnoreGet(unit, intIface, &mtuIgnore) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(mtuIgnore, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }

  ewsTelnetWrite(ewsContext, stat);

  /*-----------------------------------------------------*/
  /*   END  OF OSPF INTERFACE CONFIGURATION INFORMATION  */
  /*-----------------------------------------------------*/

  /*----------------------------------------------------------*/
  /*  TEST FOR OSPF INTERFACE HAVING VALID STATUS INFORMATION */
  /*----------------------------------------------------------*/

  /* check if ospf is enable and if it is in addition enable per interface */
  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    cliSyntaxTop(ewsContext);
    rc = usmDbOspfAdminModeGet (unit, &val);
    if (val == L7_ENABLE)
    {
      ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgRtrId_1);
    }
    else
    {ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspf);}

    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if (usmDbOspfIntfExists(unit, intIface) != L7_TRUE)
  {
    cliSyntaxTop(ewsContext);
    /* check for global Ospf Admin Mode */
    if(usmDbOspfAdminModeGet (unit, &val) != L7_SUCCESS || val != L7_ENABLE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                             pStrErr_routing_MustEnblOspf);
    }
    /* check for intf Ospf Admin Mode */
    if (adminMode == L7_ENABLE)
    {
      if ((usmDbIfAdminStateGet(unit, intIface, &val) == L7_SUCCESS) &&
          (val == L7_DISABLE))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                               pStrErr_routing_InterfaceIsDisabled);
      }
      if ((usmDbIfActiveStateGet(unit, intIface, &val) == L7_SUCCESS) &&
          (val != L7_ACTIVE))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                               pStrErr_routing_InterfaceIsDown);
      }
      if ((usmDbIpRtrIntfModeGet(unit, intIface, &val) == L7_SUCCESS) &&
          (val == L7_DISABLE))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                               pStrErr_routing_RoutingInterfaceIsDown);
      }
      if ((primaryAddr == L7_NULL_IP_ADDR) || (primaryMask == L7_NULL_IP_ADDR))
      {
        /* If an unnumbered interface, OSPF must treat interface as p2p. */
        if (usmDbIntfIsUnnumbered(intIface))
        {
          if (networkType != L7_OSPF_INTF_PTP)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                                   pStrErr_routing_MustBeP2p);
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                                 pStrErr_routing_NoIpAddress);
        }
      }
      if ((usmDbIfActiveStateGet(unit, intIface, &val) == L7_SUCCESS) && (val != L7_ACTIVE))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                                 pStrInfo_routing_OspfIpIntfIsInactive);
      }
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                             pStrErr_common_OspfCantBeInitializedOnIntf);
    }
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext,
                                             pStrErr_common_MustInitOspfIntf);
  }

  /*----------------------------------------------*/
  /*  START OF OSPF INTERFACE STATUS INFORMATION  */
  /*----------------------------------------------*/

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_State);
  if (usmDbOspfIntfStateGet(unit, intIface, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_OSPF_INTF_DOWN:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
      break;
    case L7_OSPF_INTF_LOOPBACK:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_LoopBack);
      break;
    case L7_OSPF_INTF_WAITING:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Waiting_2);
      break;
    case L7_OSPF_INTF_POINTTOPOINT:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_PointToPoint_2);
      break;
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DesignatedRtr_2);
      break;
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_BackupDesignatedRtr_2);
      break;
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_OtherDesignatedRtr_1);
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

  if ((networkType == L7_OSPF_INTF_BROADCAST) ||
      (networkType == L7_OSPF_INTF_NBMA))
  {
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_DesignatedRtr);
  if ((usmDbOspfIntfDrIdGet(unit, intIface, &val) == L7_SUCCESS) &&
      (usmDbInetNtoa(val, stat) == L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_BackupDesignatedRtr);
  if ((usmDbOspfIntfBackupDrIdGet(unit, intIface, &val) == L7_SUCCESS) &&
      (usmDbInetNtoa(val, stat) == L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  }
  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_NumOfLinkEvents);

  if (usmDbOspfIntfLinkEventsCounterGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_LocalLinkLsas);
     if (usmDbOspfIntfLocalLinkOpaqueLSAsGet(unit, intIface, &val) == L7_SUCCESS)
     {
     osapiSnprintf(stat, sizeof(stat),  "%u", val);
     }
     else
     {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
     }
     ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_routing_LocalLinkLsaChecksum);
     if (usmDbOspfIntfLocalLinkOpaqueLSACksumGet(unit, intIface, &val) == L7_SUCCESS)
     {
     osapiSnprintf(stat, sizeof(stat),  "%u", val);
     }
     else
     {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  return cliSyntaxReturnPrompt (ewsContext, stat);
}

/*********************************************************************
 *
 * @purpose Displays statistics for the specified ospf interface
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
 * @cmdsyntax    show ip ospf interface stats <slot/port>
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfInterfaceStats(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 intIface, intSlot, intPort;
  L7_uint32 intIPaddr;
  L7_uint32 intAreaId;
  L7_uint32 val;
  L7_uint32 mask;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_OspfIntfStats_t intfStats;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_ShowRtrOspfIntfStats_1, cliSyntaxInterfaceHelp());
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &intIface) != L7_SUCCESS)
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
    if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &intSlot, &intPort, &intIface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
  }
  }
  if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliPrompt(ewsContext);
  }

  /*----------------------------------------------------------*/
  /*  TEST FOR OSPF INTERFACE HAVING VALID STATUS INFORMATION */
  /*----------------------------------------------------------*/

  /* check if ospf is enable and if it is in addition enable per interface */
  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }
  if (usmDbOspfIntfExists(unit, intIface) != L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfIntf ); /* "OSPF must have been enabled on this interface.\r\nUse Command: config router ospf interface mode <slot/port> enable"*/
    return cliPrompt(ewsContext);
  }

  /*----------------------------------------------------------*/
  /*  Begin VALID STATUS INFORMATION                          */
  /*----------------------------------------------------------*/

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_OspfAreaId);                           /* get the areaid */
  if ((usmDbOspfIntfAreaIdGet(unit, intIface, &intAreaId) == L7_SUCCESS) &&
      (usmDbInetNtoa(intAreaId, stat) == L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_common_Err);
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AreaBorderRtrCount);
  if (usmDbOspfAreaBorderRtrCountGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AsBorderRtrCount);
  if (usmDbOspfAsBdrRtrCountGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AreaLsaCount);
  if (usmDbOspfAreaLSACountGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ApIpAddr);
  if ((usmDbIpRtrIntfIpAddressGet(unit, intIface, &intIPaddr, &mask) == L7_SUCCESS) &&
      (usmDbInetNtoa(intIPaddr, stat) == L7_SUCCESS))
  {
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_common_Err);
  }

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_OspfIntfEvents);
  if (usmDbOspfIfEventsGet(unit, intIPaddr, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_VirtualEvents);
  if (usmDbOspfRtrIntfAllVirtIfEventsGet(unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_NeighborEvents);
  if (usmDbOspfRtrIntfAllNbrEventsCounterGet (unit, intIface, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ExternalLsaCount);
  if (usmDbOspfExternalLSACountGet(unit, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* Get block of interface statistics */
  if (usmDbOspfIntfStatsGet(intIface, &intfStats) == L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    cliFormat(ewsContext, pStrInfo_common_OspfTxPkts);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.TxPackets);

    cliFormat(ewsContext, pStrInfo_common_OspfRxPkts);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.RxPackets);

    cliFormat(ewsContext, pStrInfo_common_OspfRxDiscard);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.DiscardPackets);

    cliFormat(ewsContext, pStrInfo_common_OspfRxBadVer);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadVersion);

    cliFormat(ewsContext, pStrErr_routing_OspfRxBadNw);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadNetwork);

    cliFormat(ewsContext, pStrErr_common_OspfRxBadVirtLink);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadVirtualLink);

    cliFormat(ewsContext, pStrInfo_common_OspfRxBadArea);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadArea);

    cliFormat(ewsContext, pStrErr_common_OspfRxBadDestAddr);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadDstAdr);

    cliFormat(ewsContext, pStrInfo_common_OspfRxBadAuthType);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadAuType);

    cliFormat(ewsContext, pStrErr_common_AuthFailure);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadAuthentication);

    cliFormat(ewsContext, pStrInfo_common_OspfRxBadNbr);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadNeighbor);

    cliFormat(ewsContext, pStrErr_common_OspfRxBadPktType);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.BadPckType);

    cliFormat(ewsContext, pStrInfo_common_OspfHellosIgnored);
    ewsTelnetPrintf (ewsContext, "%u", intfStats.HellosIgnored);

    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_PktTypeSentRcvd);
    ewsTelnetWrite(ewsContext, "\r\n--------------------  ----------  ---------- ");

    ewsTelnetPrintf (ewsContext, "\r\n%-20s  %10u  %10u",
                  pStrInfo_common_Hello, intfStats.TxHellos, intfStats.RxHellos);

    ewsTelnetPrintf (ewsContext, "\r\n%-20s  %10u  %10u",
                  pStrInfo_common_DbaseDesc, intfStats.TxDbDescr, intfStats.RxDbDescr);

    ewsTelnetPrintf (ewsContext, "\r\n%-20s  %10u  %10u",
                  pStrInfo_common_LsReq, intfStats.TxLsReq, intfStats.RxLsReq);

    ewsTelnetPrintf (ewsContext, "\r\n%-20s  %10u  %10u",
                  pStrInfo_common_LsUpdate, intfStats.TxLsUpdate, intfStats.RxLsUpdate);

    ewsTelnetPrintf (ewsContext, "\r\n%-20s  %10u  %10u\r\n",
                  pStrInfo_common_LsAckment, intfStats.TxLsAck, intfStats.RxLsAck);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose   Display OSPF area information
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
 * @cmdsyntax    show ip ospf areas <area id>
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfArea(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 argAreaId = 1;
  L7_uint32 intAreaId;
  L7_uint32 val;
  L7_uint32 externalRouting;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRtrOspfAreaInfo);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  if (strlen(argv[index+argAreaId]) >= sizeof(strAreaId) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }
  OSAPI_STRNCPY_SAFE(strAreaId, argv[index + argAreaId]);
  if (usmDbInetAton(strAreaId, &intAreaId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }
  else if (usmDbOspfAreaEntryExists(intAreaId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_NoAreaCfgured, strAreaId);
  }

  cliSyntaxTop(ewsContext);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AreaId_2);  /* AreaID */
  if (usmDbInetNtoa( intAreaId, stat) != L7_SUCCESS )
  {
    osapiSnprintf(stat, sizeof(stat), strAreaId);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_ExternalRouting);
  if (usmDbOspfImportAsExternGet(unit,intAreaId, &externalRouting) == L7_SUCCESS)
  {
    switch (externalRouting)
    {
    case L7_OSPF_AREA_IMPORT_EXT:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_ImportExternalLsas);
      break;
    case L7_OSPF_AREA_IMPORT_NO_EXT:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_ImportNoLsas);
      break;
    case L7_OSPF_AREA_IMPORT_NSSA:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_ImportNssas);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Unknown_1);
    }
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat );

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_SpfRuns_1);
  if (usmDbOspfNumSPFRunsGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat );

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AreaBorderRtrCount);
  if (usmDbOspfAreaBorderRtrCountGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat );

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AreaLsaCount);
  if (usmDbOspfAreaLSACountGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%u", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat );

  memset (stat, 0, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_common_AreaLsaChecksum);
  if (usmDbOspfAreaLSACksumSumGet(unit, intAreaId, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat, sizeof(stat),  "%#x", val);
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  /* For NSSA, we report this later in the NSSA section */
  if (externalRouting != L7_OSPF_AREA_IMPORT_NSSA)
  {
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_ImportSummaryLsas);
    if (usmDbOspfAreaSummaryGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_AREA_SEND_SUMMARY:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Enbl_1);   /* enable */
        break;
      case L7_OSPF_AREA_NO_SUMMARY:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dsbl_1);   /* disable */
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);   /* ------- */
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
  }

  if (externalRouting == L7_OSPF_AREA_IMPORT_NO_EXT)
  {
    cliSyntaxTop(ewsContext);
    cliFormat(ewsContext, pStrInfo_common_StubAreaMetricVal);
    if (usmDbOspfStubMetricGet(unit, intAreaId, L7_TOS_NORMAL_SERVICE, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "  %-14d", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %-14s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliSyntaxBottom(ewsContext);
  }

  if (externalRouting == L7_OSPF_AREA_IMPORT_NSSA)
  {
    cliSyntaxTop(ewsContext);
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_ShowRtrOspfNssaInfo);
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
    cliFormat( ewsContext, pStrInfo_common_ImportSummaryLsas);
    if (usmDbOspfNSSAImportSummariesGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliFormat( ewsContext, pStrInfo_common_NssaRedist);
    if (usmDbOspfNSSARedistributeGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat,sizeof(stat),strUtilEnableDisableGet(val, pStrInfo_common_Blank));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliFormat( ewsContext, pStrInfo_common_DeflInfoOriginate);
    if (usmDbOspfNSSADefaultInfoOriginateGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      memset (stat, 0,sizeof(stat));
      switch (val)
      {
      case L7_TRUE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_True);
        break;
      case L7_FALSE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_False);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliFormat( ewsContext, pStrInfo_common_DeflMetric);
    if (usmDbOspfNSSADefaultMetricGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u",val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliFormat( ewsContext, pStrInfo_common_DeflMetricType);
    if (usmDbOspfNSSADefaultMetricTypeGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      memset (stat, 0,sizeof(stat));
      switch (val)
      {
      case L7_OSPF_AREA_STUB_COMPARABLE_COST:
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Comparable);
        break;
      case L7_OSPF_AREA_STUB_NON_COMPARABLE_COST:
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_NonComparable);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    if (usmDbOspfNSSATranslatorRoleGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      memset (stat, 0,sizeof(stat));
      cliFormat( ewsContext, pStrInfo_common_TranslatorRole);
      switch (val)
      {
      case L7_OSPF_NSSA_TRANSLATOR_ALWAYS:
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Always);
        break;
      case L7_OSPF_NSSA_TRANSLATOR_CANDIDATE:
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Candidate);
        break;
      default:
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Candidate);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    if (usmDbOspfNSSATranslatorStabilityIntervalGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      cliFormat( ewsContext, pStrInfo_common_TranslatorStabilityIntvl);
      osapiSnprintf(stat, sizeof(stat), "%u",val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    if(usmDbOspfNSSATranslatorStateGet(unit, intAreaId, &val) == L7_SUCCESS)
    {
      memset (stat, 0,sizeof(stat));
      cliFormat( ewsContext, pStrInfo_common_TranslatorState);
      switch (val)
      {
      case L7_OSPF_NSSA_TRANS_STATE_ENABLED:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Enbld);
        break;
      case L7_OSPF_NSSA_TRANS_STATE_ELECTED:
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_Elected);
        break;
      case L7_OSPF_NSSA_TRANS_STATE_DISABLED:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dsbld);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dsbld);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "  %s", pStrErr_common_Err);
    }
    ewsTelnetWrite( ewsContext, stat);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose   Display OSPF area information
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
 * @cmdsyntax    show ip ospf area range <area id>
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfAreaRange(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_RC_t rc;
  L7_uint32 argAreaId = 1;
  L7_uint32 intAreaId;
  L7_uint32 val;
  L7_uint32 areaId = 0;
  L7_uint32 lsdbType = 0;
  L7_uint32 ipAddr = 0;
  L7_uint32 subnet = 0;
  L7_uint32 advertisement = 0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_ShowRtrOspfAreaRange_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  if (strlen(argv[index+argAreaId]) >= sizeof(strAreaId) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }
  OSAPI_STRNCPY_SAFE(strAreaId, argv[index + argAreaId]);
  if ((usmDbInetAton(strAreaId, &intAreaId) != L7_SUCCESS) ||
      (usmDbOspfNumSPFRunsGet(unit, intAreaId, &val) != L7_SUCCESS))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }

  ewsTelnetWriteAddBlanks (1, 0, 4, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_AreaIdIpAddrSubnetMaskLsdbTypeAdvertisement);
  ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- --------------- --------------- -------------\r\n");

  areaId = intAreaId;

  while (usmDbOspfAreaAggregateEntryNext(unit, &areaId, &lsdbType, &ipAddr, &subnet) == L7_SUCCESS  &&
      intAreaId == areaId)
  {
    rc = usmDbOspfAreaAggregateEffectGet(unit, areaId, lsdbType, ipAddr, subnet, &advertisement);

    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    usmDbInetNtoa(areaId, buf);
    ewsTelnetPrintf (ewsContext, "%-16s", buf);

    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    usmDbInetNtoa(ipAddr, buf);
    ewsTelnetPrintf (ewsContext, "%-16s", buf);

    memset (stat, 0, sizeof(stat));
    memset (buf, 0, sizeof(buf));
    usmDbInetNtoa(subnet, buf);
    ewsTelnetPrintf (ewsContext, "%-16s", buf);

    memset (stat, 0, sizeof(stat));
    switch (lsdbType)
    {
    case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK:
      osapiSnprintf (stat, sizeof(stat),  "%-16s", pStrInfo_common_SummaryLink);
      break;
    case L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK:
      osapiSnprintf (stat, sizeof(stat),  "%-16s", pStrInfo_common_ExternalLink);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat),  "%-16s", pStrInfo_common_Blank);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    if (advertisement == L7_NULL)
    {
      advertisement = USMDB_OSPF_AREA_DEFAULT_ADVERTISE_AREA;
    }

    if (advertisement == L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING)
    {
      osapiSnprintf(stat, sizeof(stat),  "%-13s", pStrInfo_common_Enbld);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "%-13s", pStrInfo_common_Dsbld);
    }
    ewsTelnetWrite(ewsContext, stat);

    cliSyntaxBottom(ewsContext);

  } /* end of 'while' loop */

  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose displays the OSPF Interface summary
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
 * @cmdsyntax    show ip ospf Interface brief
 *
 * @cmddescript  This command has no options.
 *
 * @end
 *
 **********************************************************************/
const char *commandShowIpOspfInterfaceBrief(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val, adminMode;
  L7_uint32 intAreaId;
  L7_uint32 intSlot, intPort, interface;
  L7_uint32 nextInterface;
  L7_uint32 unit;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  ospfInterfaceAttrib attrib[OSPF_MAX_ATTRIBUTES];

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_ShowRtrOspfIntfSummary);
  }

  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NoValidPorts);
  }

  /* Heading for the router OSPF interface summary */
  ewsTelnetWriteAddBlanks (2, 0, 47, 3, L7_NULLPTR, ewsContext,pStrInfo_routing_HelloDeadRetraxLsa);
  ewsTelnetWriteAddBlanks (1, 0, 13, 3, L7_NULLPTR, ewsContext,pStrInfo_routing_AdminRtrIntIntIntRetraxAck);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_IntfModeAreaIdPriorCostValValValDelayIntval);
  ewsTelnetWrite(ewsContext,"\r\n------------ -------- ----------- ------ ----- ----- ----- ------ ------ ------\r\n");

  memset(attrib, 0, sizeof(attrib));
  attrib[0].width = 13;
  attrib[0].precision = 12;
  attrib[1].width = 9;
  attrib[1].precision = 8;
  attrib[2].width = 12;
  attrib[2].precision = 11;
  attrib[3].width = 7;
  attrib[3].precision = 6;
  attrib[4].width = 6;
  attrib[4].precision = 5;
  attrib[5].width = 6;
  attrib[5].precision = 5;
  attrib[6].width = 6;
  attrib[6].precision = 5;
  attrib[7].width = 7;
  attrib[7].precision = 6;
  attrib[8].width = 7;
  attrib[8].precision = 6;
  attrib[9].width = 6;
  attrib[9].precision = 5;

  while (interface)
  {
    /* display only the visible interfaces */
    if ((usmDbOspfIsValidIntf(unit, interface) == L7_TRUE)  &&
        (usmDbIpIntfExists(unit, interface) == L7_TRUE) &&
        (usmDbOspfAdminModeGet (unit, &adminMode) == L7_SUCCESS) &&
        (adminMode == L7_ENABLE))
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
      ewsTelnetPrintf (ewsContext, "%-13.12s",buf);

      if (usmDbOspfIntfAdminModeGet (unit, interface, &adminMode) == L7_SUCCESS)
      {
        switch (adminMode)
        {
        case L7_DISABLE:
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s", pStrInfo_common_Dsbl_1);
          break;
        case L7_ENABLE:
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s", pStrInfo_common_Enbl_1);
          break;
        default:
          osapiSnprintf(stat, sizeof(stat),  "%-9.8s", "-----");
        }
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-9.8s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext, stat);

      /* display area ID */
      memset(stat, 0, sizeof(stat));
      memset(buf, 0, sizeof(buf));
      if ((usmDbOspfIntfAreaIdGet(unit, interface, &intAreaId) == L7_SUCCESS) &&
          (usmDbInetNtoa(intAreaId, stat) == L7_SUCCESS))
      {
        memset(attrib[2].remainStr, 0, sizeof(attrib[2].remainStr));
         sprintf(buf, "%*.*s",-attrib[2].width, attrib[2].precision, stat);
         if(strlen(stat) >= attrib[2].width)
         {
           strncpy(attrib[2].remainStr, &stat[attrib[2].precision], (strlen(stat) - attrib[2].precision));
           attrib[2].flagRemain = L7_TRUE;
         }
         else
         {
           attrib[2].flagRemain = L7_FALSE;
         }
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%-12.11s", "-----");
      }
      ewsTelnetWrite(ewsContext, buf);

      /*display priority */
      memset (buf, 0, sizeof(buf));
      if (usmDbOspfIntfPriorityGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf),  "%-7d", val);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%-7.6s", "-----");
      }
      ewsTelnetWrite(ewsContext, buf);

      /* display ospf cost */
      if (usmDbOspfIfMetricValueGet(unit, interface, L7_TOS_NORMAL_SERVICE, &val) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%-6d", val);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "%-6.5s", "-----");
      }
      ewsTelnetWrite(ewsContext, buf);

      /* display hello interval*/
      memset (stat, 0, sizeof(stat));
      if (usmDbOspfIntfHelloIntervalGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat),  "%-6d", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-6.5s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      /*display dead interval */
      memset (stat, 0, sizeof(stat));
      if (usmDbOspfIntfDeadIntervalGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat),  "%-6d", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-6.5s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      /*Display Rxmt Interval*/
      memset (stat, 0, sizeof(stat));
      if (usmDbOspfIntfRxmtIntervalGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat),  "%-7d", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-7.6s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      /*Display rxmt Delay*/
      memset (stat, 0, sizeof(stat));
      if (usmDbOspfIntfTransitDelayGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat),  "%-7d", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-7.6s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      /* LSA Ack Interval */
      memset (stat, 0, sizeof(stat));
      if (usmDbOspfIntfLSAAckIntervalGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat),  "%-6d", val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-6s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      while(checkPrintRemain(9, attrib))
      {
        printRemain(ewsContext, 9, attrib);
      }

      cliSyntaxBottom(ewsContext);
    }                                                                         /* end of if */

    /* Get the next interface */
    if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
    }
    else
    {
      interface = 0;
    }

  }                                                                           /* end of while loop */
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  Display the OSPF neighbor table list
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
 * @cmdsyntax in stacking env: show ip ospf neighbor brief <unit/slot/port | all}
 * @cmdsyntax in non-stacking env: show ip ospf neighbor brief <slot/port | all}
 *
 * @cmdhelp  Display the brief OSPF neighbor table list.
 *
 * @cmddescript  This command has no options. When a particular neighbor ID
 *               is specified, detailed information about a neighbor is given.
 *
 * @end
 *
 *********************************************************************/
const char  *commandShowIpOspfNeighborBrief(EwsContext ewsContext, L7_uint32 unit, L7_BOOL isIntfCfg, L7_uint32 intIfNum, uintf argc, const char * * argv)
{
  L7_RC_t rc;
  L7_uint32 intSlot;
  L7_uint32 intPort;
  static  L7_uint32 nbrIpAddr = 0, nbrIntIfNum = 0;
  L7_uint32 intU_IDX;
  L7_uint32 intRouterId;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strRouterId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNeigborIface[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;
  L7_uint32 nbrState;
  L7_uint32 count = 0;
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  
  /* turn auto-scroll off */
  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      nbrIpAddr = 0;
      nbrIntIfNum = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    nbrIpAddr = 0;
    nbrIntIfNum = 0;
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_RtrIdPriIpAddrNeighborStateDead);
  ewsTelnetWriteAddBlanks (1, 0, 41, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_IntfTime);
  ewsTelnetWrite(ewsContext,"\r\n--------------- -------- --------------- ----------- ------------------ ----\r\n");

  count = 0;
  rc = usmDbOspfNbrEntryGet(unit, nbrIpAddr, nbrIntIfNum);
  if (rc != L7_SUCCESS)
  {
    rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddr, &nbrIntIfNum);
  }
  while ((rc == L7_SUCCESS) &&  (count < CLI_MAX_SCROLL_LINES-6))
  {
     if (isIntfCfg)
    {
      if (intIfNum != nbrIntIfNum)
      {

        rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddr, &nbrIntIfNum);
        continue;
      }
    }

    if ((usmDbOspfNeighbourRouterIdGet(unit, nbrIntIfNum, nbrIpAddr, &intRouterId)) == L7_SUCCESS)
    {
      memset (stat, 0, sizeof(stat));
      memset (strRouterId, 0, sizeof(strRouterId));
      if (usmDbInetNtoa(intRouterId, strRouterId) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-16.15s", strRouterId);
     }
    else
    {
        osapiSnprintf(stat, sizeof(stat), "%-16.15s", pStrErr_common_Err);
    }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    if (usmDbOspfNeighbourPriorityGet(unit, nbrIntIfNum, nbrIpAddr, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-9u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-9.8s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    memset (strIpAddr, 0, sizeof(strIpAddr));
    if ((usmDbInetNtoa(nbrIpAddr, strIpAddr) == L7_SUCCESS))
    {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", strIpAddr);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-16.15s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    if ((usmDbUnitSlotPortGet(nbrIntIfNum, &intU_IDX, &intSlot, &intPort) == L7_SUCCESS))
    {
      osapiSnprintf(strNeigborIface, sizeof(strNeigborIface), cliGetIntfName(nbrIntIfNum, intU_IDX, intSlot, intPort));
      osapiSnprintf(stat, sizeof(stat), "%-12.11s", strNeigborIface);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-12.11s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    if (usmDbOspfNeighbourStateGet(unit, nbrIntIfNum, nbrIpAddr, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_DOWN:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Down_1);
        break;
      case L7_OSPF_ATTEMPT:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Attempt);
        break;
      case L7_OSPF_INIT:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Init);
        break;
      case L7_OSPF_TWOWAY:
        osapiSnprintf(stat, sizeof(stat),  "%s/",  pStrInfo_common_TwoWay);
        break;
      case L7_OSPF_EXCHANGESTART:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_ExStart);
        break;
      case L7_OSPF_EXCHANGE:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Exchange);
        break;
      case L7_OSPF_LOADING:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Loading);
        break;
      case L7_OSPF_FULL:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Full);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat),  "%s/", "---");
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s/", pStrErr_common_Err);
    }

    if (usmDbOspfNeighbourIntfStateGet(unit, nbrIntIfNum, intRouterId, &nbrState) == L7_SUCCESS)
    {
      switch (nbrState)
      {
      case L7_OSPF_INTF_DOWN:
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Down);
        break;
      case L7_OSPF_INTF_LOOPBACK:
        OSAPI_STRNCAT(stat, pStrInfo_common_LoopBack);
        break;
      case L7_OSPF_INTF_WAITING:
        OSAPI_STRNCAT(stat, pStrInfo_common_Waiting);
        break;
      case L7_OSPF_INTF_POINTTOPOINT:
        OSAPI_STRNCAT(stat, pStrInfo_common_Ptp);
        break;
      case L7_OSPF_INTF_DESIGNATEDROUTER:
        OSAPI_STRNCAT(stat, pStrInfo_common_Dr);
        break;
      case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
        OSAPI_STRNCAT(stat, pStrInfo_common_BackupDr);
        break;
      case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
        OSAPI_STRNCAT(stat, pStrInfo_common_DrOther);
        break;
      default:
        OSAPI_STRNCAT(stat, "---");
      }
    }
    else
    {
      OSAPI_STRNCAT(stat, pStrErr_common_Err);
    }
    ewsTelnetPrintf (ewsContext, "%-19.18s", stat);

    memset (stat, 0, sizeof(stat));
    if (usmDbOspfNeighbourDeadTimerRemainingGet(unit, nbrIntIfNum, nbrIpAddr, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%-4u", (val/1000));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-4.4s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
    count = count+1;
    rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddr, &nbrIntIfNum);
    cliSyntaxBottom(ewsContext);
  }
  if (rc != L7_SUCCESS)
  {  
    nbrIpAddr = 0;
    nbrIntIfNum = 0;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  else
  {
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (val=1; val<argc; val++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[val]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }
}

/*********************************************************************
 *
 * @purpose    displays the OSPF neighbor detailed list.
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
 * @cmdsyntax    show ip ospf neighbor [interface <slot/port>] [<nbr-id>]
 *
 * @cmdhelp
 *
 * @cmddescript  This command has no options. When a particular neighbor ID
 *               is specified, detailed information about a neighbor is given.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfNeighbor(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 numArgs, pos;
  L7_uint32 val;
  L7_uint32 uptime; 
  L7_timespec tsUptime;
  static L7_uint32 nbrIpAddress = 0, intIface = 0, nbrCount = 0;
  L7_uint32 nbrIface = 0;
  L7_uint32 intSlot, intPort;
  L7_uint32 intRouterId, nbrRouterId;
  L7_uint32 areaId;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strRouterId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNbrIpAddress[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strNbrAreaAddress[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit = 0, i;
  L7_BOOL isBrief = L7_FALSE, isIntfCfg = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;

  cliCmdScrollSet(L7_FALSE);
  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  if(numArgs <= 0)
  {
    isBrief = L7_TRUE;
    isIntfCfg = L7_FALSE;
  }
  else
  {
    pos = 1;

    if(strcmp(argv[index+pos], pStrInfo_common_Ipv6DhcpRelayIntf_1) == 0)
    {
      pos++;

      /* need at least one more arg for slot/port */
      if(numArgs < pos)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, stat, sizeof(stat),  pStrErr_routing_ShowRtrOspfNeighborDetailed, cliSyntaxInterfaceHelp());
        ewsTelnetWrite( ewsContext, stat);
        return cliPrompt(ewsContext);
      }

      isIntfCfg = L7_TRUE;
      if (cliIsStackingSupported() == L7_TRUE)
      {
        if (cliValidSpecificUSPCheck(argv[index+pos], &unit, &intSlot, &intPort) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &nbrIface) != L7_SUCCESS)
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
        if (cliSlotPortToIntNum(ewsContext, argv[index+pos], &intSlot, &intPort, &nbrIface) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
      
      if ( cliValidateRtrIntf(ewsContext, nbrIface) != L7_SUCCESS )
      {
        return cliPrompt(ewsContext);
      }

      pos++;
    }

    /* have a neighbor ipaddr? */
    if (numArgs >= pos)
    {
      if (strlen(argv[index+pos]) >= sizeof(strRouterId) )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }

      OSAPI_STRNCPY_SAFE(strRouterId, argv[index+pos]);

      isBrief = L7_FALSE;
    }
    else
    {
      isBrief = L7_TRUE;
    }
  }

  /* check if all the nescessary values have been initialized */
  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }
  else if (isIntfCfg && usmDbOspfIntfExists(unit, nbrIface) != L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfIntf ); /* "OSPF must have been enabled on this interface.\r\nUse Command: config router ospf interface mode <slot/port> enable"*/
    return cliPrompt(ewsContext);
  }

  if(isBrief)
  {
    return commandShowIpOspfNeighborBrief(ewsContext, unit, isIntfCfg, nbrIface, argc, argv);
  }

  if (usmDbInetAton(strRouterId, &nbrRouterId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  intIface = isIntfCfg ? nbrIface : 0;
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      nbrIpAddress = 0;
      intIface = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  if((rc = usmDbOspfNbrEntryGet(unit, nbrIpAddress, intIface)) != L7_SUCCESS)
  {
    rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddress, &intIface);
  }

  while( rc == L7_SUCCESS )
  {
    if(usmDbOspfNbrRtrIdGet(unit, nbrIpAddress, intIface, &intRouterId) != L7_SUCCESS)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_Err);
      rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddress, &intIface);
      continue;
    }

    if(intRouterId != nbrRouterId)
    {
      rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddress, &intIface);
      continue;
    }

    if (isIntfCfg && (intIface != nbrIface))
    {
      rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddress, &intIface);
      continue;
    }

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_Intf);
    if (usmDbUnitSlotPortGet(intIface, &unit, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), cliGetIntfName(intIface, unit, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
    memset (strNbrIpAddress, 0, sizeof(strNbrIpAddress));
    cliFormat(ewsContext, pStrInfo_common_NeighborIpAddr);
    if (usmDbInetNtoa(nbrIpAddress, strNbrIpAddress) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, strNbrIpAddress);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrErr_common_Err);
    }

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_IntfIdx);
    if (usmDbOspfNeighbourIfIndexGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);
    memset (strNbrAreaAddress, 0, sizeof(strNbrAreaAddress));
    cliFormat(ewsContext, pStrInfo_common_AreaId_1);
    if (usmDbOspfNeighbourAreaGet(unit, intIface, nbrIpAddress, &areaId) == L7_SUCCESS)
    {
      usmDbInetNtoa(areaId, strNbrAreaAddress);
      ewsTelnetWrite(ewsContext, strNbrAreaAddress);
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrErr_common_Err);
    }

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_Options);
    if (usmDbOspfNeighbourOptionsGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "0x%x", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_RtrPri);
    if (usmDbOspfNeighbourPriorityGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_DeadTimer);
    if (usmDbOspfNeighbourDeadTimerRemainingGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", (val/1000));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* How long adjacency has been in Full state */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_UpTime);
    if (usmDbOspfNbrUptimeGet(intIface, nbrIpAddress, &uptime) == L7_SUCCESS)
    {
      osapiConvertRawUpTime(uptime, &tsUptime);
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DaysHrsMinsSecs, 
                    tsUptime.days, tsUptime.hours, tsUptime.minutes, tsUptime.seconds);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_State);                           /* State */
    if (usmDbOspfNeighbourStateGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_DOWN:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Down_1);
        break;
      case L7_OSPF_ATTEMPT:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Attempt);
        break;
      case L7_OSPF_INIT:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Init);
        break;
      case L7_OSPF_TWOWAY:
        osapiSnprintf(stat, sizeof(stat),  "%s/",  pStrInfo_common_TwoWay);
        break;
      case L7_OSPF_EXCHANGESTART:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_ExchangeStart);
        break;
      case L7_OSPF_EXCHANGE:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Exchange);
        break;
      case L7_OSPF_LOADING:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Loading);
        break;
      case L7_OSPF_FULL:
        osapiSnprintf(stat, sizeof(stat),  "%s/", pStrInfo_common_Full);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat),  "%s/", "---");
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%s/", pStrErr_common_Err);
    }

    if (usmDbOspfNeighbourIntfStateGet(unit, intIface, intRouterId, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_INTF_DOWN:
        OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_common_Down);
        break;
      case L7_OSPF_INTF_LOOPBACK:
        OSAPI_STRNCAT(stat, pStrInfo_common_LoopBack);
        break;
      case L7_OSPF_INTF_WAITING:
        OSAPI_STRNCAT(stat, pStrInfo_common_Waiting);
        break;
      case L7_OSPF_INTF_POINTTOPOINT:
        OSAPI_STRNCAT(stat, pStrInfo_common_Ptp);
        break;
      case L7_OSPF_INTF_DESIGNATEDROUTER:
        OSAPI_STRNCAT(stat, pStrInfo_common_Dr);
        break;
      case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
        OSAPI_STRNCAT(stat, pStrInfo_common_BackupDr);
        break;
      case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
        OSAPI_STRNCAT(stat, pStrInfo_common_DrOther);
        break;
      default:
        OSAPI_STRNCAT(stat, "---");
      }
    }
    else
    {
      OSAPI_STRNCAT(stat, pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_Events);                          /* "Events"); */
    if (usmDbOspfNeighbourEventsCounterGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_RetransmissionQueueLen);
    if (usmDbOspfNeighbourLSRetransQLenGet(unit, intIface, nbrIpAddress, &val) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat), "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
    nbrCount++;
    rc = usmDbOspfNbrEntryNext(unit, &nbrIpAddress, &intIface);
    break;/* To provide pagination per interface */ 
  }
  if(rc != L7_SUCCESS)
  {
    nbrIpAddress = 0;
    intIface = 0;
  }

  if(nbrIpAddress != 0)
  {
    cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);

    for (i=1; i<argc; i++)
    {
      strcat(cmdBuf, " ");
      strcat(cmdBuf, argv[i]);
    }
    cliAlternateCommandSet(cmdBuf);
    return pStrInfo_common_Name_2;
  }


  if (nbrCount == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_NoNeighbors );
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose Display statistics on recent SPF runs.
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
 * @cmdsyntax    show ip ospf statistics
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char  *commandShowIpOspfStatistics(EwsContext ewsContext, uintf argc,
                                        const char * * argv, uintf index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 areaIdStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 areaId;
  L7_uint32 numSpfRuns;
  L7_uint32 maxStats = 15;
  L7_uint32 numRows;          /* number of SPF runs reported */
  L7_OspfSpfStats_t spfStats[maxStats];
  L7_uint32 deltaTime;        /* number of seconds since an SPF run */
  L7_uint32 hours, minutes, seconds;
  L7_uint32 curTime;          /* number of seconds since boot */

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  /* print the number of times the SPF has run for each area */
  rc = usmDbOspfAreaIdGet(unit, &areaId);
  while (rc == L7_SUCCESS)
  {
    memset (stat, 0, sizeof(stat));
    if (usmDbOspfNumSPFRunsGet(unit, areaId, &numSpfRuns) == L7_SUCCESS)
    {
      usmDbInetNtoa(areaId, areaIdStr);
      osapiSnprintfAddBlanks (1, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat),  pStrInfo_routing_AreaSpfAlgorithmExecutedTimes, areaIdStr,
          numSpfRuns);
      ewsTelnetWrite(ewsContext, stat);
    }
    rc = usmDbOspfAreaIdGetNext(unit, areaId, &areaId);
  }

  /* print table of recent SPF runs */
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_DeltaTSpfDurationMsecReason);
  ewsTelnetWrite(ewsContext,"\r\n--------  -------------------    ---------------");
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

  numRows = usmDbOspfSpfStatsGet(unit, maxStats, spfStats);
  curTime = osapiTimeMillisecondsGet()/1000;
  for (i = 0; i < numRows; i++)
  {
    memset (stat, 0, sizeof(stat));
    /* Print how long ago each SPF ran. So subtract SPF time from current time
     * and convert time of SPF to hh:mm:ss. */
    //deltaTime = curTime - spfStats[i].spfTime;
    deltaTime = osapiTimeMillisecondsDiff(curTime,spfStats[i].spfTime);
    hours = deltaTime / 3600;
    minutes = (deltaTime - (3600 * hours)) / 60;
    seconds = deltaTime - (3600 * hours) - (60 * minutes);
    ewsTelnetPrintf (ewsContext, "\r\n%02u:%02u:%02u  %19u    %-15s", hours, minutes, seconds,
        spfStats[i].spfDuration, spfStats[i].reason);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose displays the OSPF routes to ABRs
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
 * @cmdsyntax    show ip ospf abr
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfABRs(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_RC_t rc;
  L7_uint32 destIp;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RtbEntryInfo_t rtbEntry;
  L7_uint32 i, slot, port, unitIndex;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  /* print column headings */
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_TypeRtrIdCostAreaIdNextHopNextHop_1);
  ewsTelnetWriteAddBlanks (1, 0, 62, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Intf_3);
  ewsTelnetWrite(ewsContext,"\r\n----- --------------- ----- --------------- --------------- -----------");
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

  destIp = 0;
  rc = usmDbOspfAbrEntryGet(unit, destIp, &rtbEntry);
  if (rc != L7_SUCCESS)
  {
    rc = usmDbOspfAbrEntryNext(unit, &destIp, &rtbEntry);
  }

  while (rc == L7_SUCCESS)
  {
    /* Route Type ( intra (or) inter ) */
    memset (stat, 0, sizeof(stat));
    switch (rtbEntry.pathType)
    {
    case L7_OSPF_INTRA_AREA:
      osapiSnprintf(stat, sizeof(stat), "%-6.5s", pStrInfo_common_IntraArea);
      break;
    case L7_OSPF_INTER_AREA:
      osapiSnprintf(stat, sizeof(stat), "%-6.5s", pStrInfo_common_InterArea);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), "%-6.5s", pStrInfo_common_UnDefined);
      break;
    }
    ewsTelnetWrite(ewsContext, stat);

    /* Destination ID */
    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    rc = usmDbInetNtoa(rtbEntry.destinationIp, buf);
    ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

    /* Cost of using this route */
    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    osapiSnprintf(buf, sizeof(buf), "%u", rtbEntry.cost);
    ewsTelnetPrintf (ewsContext, "%-6.5s", buf);

    /* Area Id of the area from which this routing entry is learned */
    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    rc = usmDbInetNtoa(rtbEntry.areaId, buf);
    ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

    for( i = 0 ; i < rtbEntry.no_of_paths; i++ )
    {
      /* Next Hop Ip address */
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      rc = usmDbInetNtoa(rtbEntry.path[i].nextHopIpAdr, buf);
      ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

      /* Next Hop interface name */
      if(usmDbUnitSlotPortGet(rtbEntry.path[i].nextHopIfIndex,
                              &unitIndex, &slot, &port) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliGetIntfName(rtbEntry.path[i].nextHopIfIndex, unitIndex, slot, port));
        ewsTelnetPrintf (ewsContext, "%-12.11s", buf);
      }

      if((i+1) == rtbEntry.no_of_paths)
      {
        /* There are no more equal cost multiple paths left
         *   * in this routing entry */
        break;
      }
      else
      {
        cliSyntaxBottom(ewsContext);

        /* Padding the columns for the next hops */
        ewsTelnetPrintf (ewsContext, "%44s", pStrInfo_common_EmptyString);
      }
    }

    rc = usmDbOspfAbrEntryNext(unit, &destIp, &rtbEntry);
    cliSyntaxBottom(ewsContext);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose displays the OSPF routes to ASBRs
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
 * @cmdsyntax    show ip ospf asbr
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfASBRs(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_RC_t rc;
  L7_uint32 destIp;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RtbEntryInfo_t rtbEntry;
  L7_uint32 i, slot, port, unitIndex;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  /* print column headings */
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_TypeRtrIdCostAreaIdNextHopNextHop_1);
  ewsTelnetWriteAddBlanks (1, 0, 62, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Intf_3);
  ewsTelnetWrite(ewsContext,"\r\n----- --------------- ----- --------------- --------------- -----------");
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

  destIp = 0;
  rc = usmDbOspfAsbrEntryGet(unit, destIp, &rtbEntry);
  if (rc != L7_SUCCESS)
  {
    rc = usmDbOspfAsbrEntryNext(unit, &destIp, &rtbEntry);
  }

  while (rc == L7_SUCCESS)
  {
    /* Route Type ( intra (or) inter ) */
    memset (stat, 0, sizeof(stat));
    switch (rtbEntry.pathType)
    {
    case L7_OSPF_INTRA_AREA:
      osapiSnprintf(stat, sizeof(stat), "%-6.5s", pStrInfo_common_IntraArea);
      break;
    case L7_OSPF_INTER_AREA:
      osapiSnprintf(stat, sizeof(stat), "%-6.5s", pStrInfo_common_InterArea);
      break;
    default:
      osapiSnprintf(stat, sizeof(stat), "%-6.5s", pStrInfo_common_UnDefined);
      break;
    }
    ewsTelnetWrite(ewsContext, stat);

    /* Destination ID */
    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    rc = usmDbInetNtoa(rtbEntry.destinationIp, buf);
    ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

    /* Cost of using this route */
    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    osapiSnprintf(buf, sizeof(buf), "%u", rtbEntry.cost);
    ewsTelnetPrintf (ewsContext, "%-6.5s", buf);

    /* Area Id of the area from which this routing entry is learned */
    memset (buf, 0, sizeof(buf));
    memset (stat, 0, sizeof(stat));
    rc = usmDbInetNtoa(rtbEntry.areaId, buf);
    ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

    for( i = 0 ; i < rtbEntry.no_of_paths; i++ )
    {
      /* Next Hop Ip address */
      memset (buf, 0, sizeof(buf));
      memset (stat, 0, sizeof(stat));
      rc = usmDbInetNtoa(rtbEntry.path[i].nextHopIpAdr, buf);
      ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

      /* Next Hop interface name */
      if(usmDbUnitSlotPortGet(rtbEntry.path[i].nextHopIfIndex,
                              &unitIndex, &slot, &port) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliGetIntfName(rtbEntry.path[i].nextHopIfIndex, unitIndex, slot, port));
        ewsTelnetPrintf (ewsContext, "%-12.11s", buf);
      }

      if((i+1) == rtbEntry.no_of_paths)
      {
        /* There are no more equal cost multiple paths left
         *   * in this routing entry */
        break;
      }
      else
      {
        cliSyntaxBottom(ewsContext);

        /* Padding the columns for the next hops */
        ewsTelnetPrintf (ewsContext, "%44s", pStrInfo_common_EmptyString);
      }
    }

    rc = usmDbOspfAsbrEntryNext(unit, &destIp, &rtbEntry);
    cliSyntaxBottom(ewsContext);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose displays the OSPF link state database summary in columns
 *
 *
 * @param EwsContext ewsContext
 * @param L7_BOOL first
 * @param L7_BOOL isUserAreaIdCfg
 * @param L7_uint32 userAreaId
 *
 * @returntype L7_BOOL
 * @returns L7_TRUE  has more data to show
 * @returns L7_FALSE finished processing (no more)
 *
 * @notes
 *
 * @cmdsyntax    show ip ospf database
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
L7_BOOL showIpOspfDatabaseSummary(EwsContext ewsContext, L7_BOOL first, L7_BOOL isUserAreaIdCfg, L7_uint32 userAreaId)
{
  L7_uint32 intRouterId;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 line[L7_CLI_MAX_STRING_LENGTH];
  static L7_RC_t rc = L7_FAILURE;
  static L7_uint32 areaId = 0;
  L7_uint32 areaStatus;
  L7_uint32 unit = cliGetUnitId();
  L7_uint32 subtotal = 0, lsaCount = 0;
  L7_OspfAreaDbStats_t areaStats;
  static L7_OspfAreaDbStats_t rtrStats;
  L7_BOOL displayArea = L7_TRUE;

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
    return L7_FALSE;
  }

  if(usmDbOspfRouterIdGet(unit, &intRouterId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DispOspfRtr);
    return L7_FALSE;
  }

  if(first)
  {
    /* init, print headers the first time round */
    usmDbInetNtoa(intRouterId, buf);
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, line, sizeof(line),  pStrInfo_common_CfgOspfRtrId, buf);
    ewsTelnetWrite(ewsContext, line);
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    areaId = 0;
    memset ((void *)&rtrStats, 0, sizeof(rtrStats));
    rc = usmDbOspfAreaIdGet(unit, &areaId);
  }

  while (rc == L7_SUCCESS)
  {
    if(usmDbOspfAreaStatusGet(unit, areaId, &areaStatus) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
      usmDbInetNtoa(areaId, buf);
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, line, sizeof(line),  pStrInfo_routing_AreaDbaseSummary, buf);
      ewsTelnetWrite(ewsContext, line);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrErr_common_Err);
      areaStatus = L7_FAILURE;
    }
    if(areaStatus != L7_OSPF_ROW_ACTIVE)
    {
      rc = usmDbOspfAreaIdGetNext(unit, areaId, &areaId);
      continue;
    }

    /* filter display based on user prefs */
    displayArea = L7_TRUE;
    if(isUserAreaIdCfg && (areaId != userAreaId))
    {
      displayArea = L7_FALSE;
    }

    if(displayArea)
    {
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
      usmDbInetNtoa(areaId, buf);
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, line, sizeof(line),  pStrInfo_routing_AreaDbaseSummary, buf);
      ewsTelnetWrite(ewsContext, line);
    }

    subtotal = 0;
    memset ((void *)&areaStats, 0, sizeof(areaStats));
    rc = usmDbOspfAreaLsaStatsGet(unit, areaId, &areaStats);

    if(rc != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_common_Err);
      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
      return L7_FALSE;
    }

    rtrStats.RouterLsaCount += areaStats.RouterLsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_common_Rtr_2);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.RouterLsaCount);
      subtotal += areaStats.RouterLsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    rtrStats.NetworkLsaCount += areaStats.NetworkLsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_common_Nw);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.NetworkLsaCount);
      subtotal += areaStats.NetworkLsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    rtrStats.NetSumLsaCount += areaStats.NetSumLsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_routing_SummaryNet);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.NetSumLsaCount);
      subtotal += areaStats.NetSumLsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    rtrStats.RtrSumLsaCount += areaStats.RtrSumLsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_common_SummaryAsbr);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.RtrSumLsaCount);
      subtotal += areaStats.RtrSumLsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    rtrStats.NSSALsaCount += areaStats.NSSALsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_common_TypeExt);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.NSSALsaCount);
      subtotal += areaStats.NSSALsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    if(displayArea)
    {
      /* not added to totals as its incl. in type-7 */
      cliFormatAddBlanks (0, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SelfOriginatedType7);
      ewsTelnetPrintf (ewsContext, "%d", areaStats.SelfOrigNSSALsaCount);
    }

    rtrStats.OpaqueLinkLsaCount += areaStats.OpaqueLinkLsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_routing_OpaqueLink);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.OpaqueLinkLsaCount);
      subtotal += areaStats.OpaqueLinkLsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    rtrStats.OpaqueAreaLsaCount += areaStats.OpaqueAreaLsaCount;
    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_routing_OpaqueArea);
      osapiSnprintf(line, sizeof(line),  "%d", areaStats.OpaqueAreaLsaCount);
      subtotal += areaStats.OpaqueAreaLsaCount;
      ewsTelnetWrite(ewsContext, line);
    }

    if(displayArea)
    {
      cliFormat(ewsContext, pStrInfo_common_SubTotal);
      ewsTelnetPrintf (ewsContext, "%d", subtotal);
    }

    /* get next area */
    rc = usmDbOspfAreaIdGetNext(unit, areaId, &areaId);

    /* if we displayed a section,
     * pause to allow user to more/quit
     */
    if(displayArea)
    {
      return L7_TRUE;
    }
  }

  /* print router totals */
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWrite(ewsContext, pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_RtrDbaseSummary);

  subtotal = 0;

  cliFormat(ewsContext, pStrInfo_common_Rtr_2);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.RouterLsaCount);
  subtotal += rtrStats.RouterLsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_common_Nw);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.NetworkLsaCount);
  subtotal += rtrStats.NetworkLsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_routing_SummaryNet);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.NetSumLsaCount);
  subtotal += rtrStats.NetSumLsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_common_SummaryAsbr);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.RtrSumLsaCount);
  subtotal += rtrStats.RtrSumLsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_common_TypeExt);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.NSSALsaCount);
  subtotal += rtrStats.NSSALsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_routing_OpaqueLink);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.OpaqueLinkLsaCount);
  subtotal += rtrStats.OpaqueLinkLsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_routing_OpaqueArea);
  osapiSnprintf(line, sizeof(line),  "%d", rtrStats.OpaqueAreaLsaCount);
  subtotal += rtrStats.OpaqueAreaLsaCount;
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_common_Type5Ext);
  if(usmDbOspfExternalLSACountGet(unit, &lsaCount) == L7_SUCCESS)
  {
    osapiSnprintf(line, sizeof(line),  "%d", lsaCount);
    subtotal += lsaCount;
  }
  else
  {
    osapiSnprintf(line, sizeof(line), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, line);

  cliFormatAddBlanks (0, 0, 2, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SelfOriginatedType5Ext);
  if(usmDbOspfSelfOrigExternalLSACountGet(unit, &lsaCount) == L7_SUCCESS)
  {
    osapiSnprintf(line, sizeof(line),  "%d", lsaCount);
  }
  else
  {
    osapiSnprintf(line, sizeof(line), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_routing_OpaqueAs);
  if(usmDbOspfOpaqueASLSACountGet(unit, &lsaCount) == L7_SUCCESS)
  {
    osapiSnprintf(line, sizeof(line),  "%d", lsaCount);
    subtotal += lsaCount;
  }
  else
  {
    osapiSnprintf(line, sizeof(line), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, line);

  cliFormat(ewsContext, pStrInfo_common_Total);
  ewsTelnetPrintf (ewsContext, "%d", subtotal);

  return L7_FALSE;
}

/*********************************************************************
 *
 * @purpose Parses the OSPF database command
 *
 *
 * @param EwsContext ewsContext
 * @param uintf argc
 * @param const char **argv
 * @param uintf index
 * @param {output} L7_BOOL * isSummary
 * @param {output} L7_BOOL * isUserAreaIdCfg
 * @param {output} L7_uint32 * userAreaId
 * @param {output} L7_uint32 * userLsType
 * @param {output} L7_uint32 intLsId
 * @param {output} L7_BOOL * isUserLsIdCfg
 * @param {output} L7_uint32 * userLsId
 * @param {output} L7_BOOL * isUserAdvRouterCfg
 * @param {output} L7_uint32 * userAdvRouter
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
L7_RC_t parseOspfDbCmdLine(EwsContext ewsContext, uintf argc, const char * * argv, uintf index,
    L7_BOOL * isSummary,
    L7_BOOL * isUserAreaIdCfg, L7_uint32 * userAreaId,
    L7_BOOL * isUserLsTypeCfg, L7_uint32 * userLsType,
    L7_BOOL * isUserLsIdCfg, L7_uint32 * userLsId,
    L7_BOOL * isUserAdvRouterCfg, L7_uint32 * userAdvRouter)
{
  L7_RC_t retval = L7_FAILURE;
  L7_uint32 pos = 0;
  L7_char8 strAddr[L7_CLI_MAX_STRING_LENGTH];

  do
  {
    /* must be at least: "show ip ospf database" */
    if(argc < 4)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
      break;
    }

    pos = 3; /* show ip ospf XXX */
    if(strncmp(argv[pos], pStrInfo_common_Dbase, strlen(pStrInfo_common_Dbase)) == 0)
    {
      *isUserAreaIdCfg = L7_FALSE;
      pos++; /* show ip ospf database XXX */
    }
    else
    {
      /* must have at least 1 token after areaid
       * "show ip ospf <areaid> database"
       * (note that pos is zero based)
       */
      if(argc < (pos+2))
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
        break;
      }

      OSAPI_STRNCPY_SAFE(strAddr, argv[pos]);
      if(usmDbInetAton(strAddr, userAreaId) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
        break;
      }

      *isUserAreaIdCfg = L7_TRUE;
      pos++; /* show ip ospf <areaid> XXX */

      if(strncmp(argv[pos], pStrInfo_common_Dbase, strlen(pStrInfo_common_Dbase)) != 0)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
        break;
      }

      pos++; /* show ip ospf <areaid> database XXX */
    }

    /* done? */
    if(argc == pos)
    {
      retval = L7_SUCCESS;
      break;
    }

    /* check for "show ip ospf database-summary" */
    *isSummary = L7_FALSE;
    if(strncmp(argv[pos], pStrInfo_common_DbaseSummary, strlen(pStrInfo_common_DbaseSummary)) == 0)
    {
      *isSummary = L7_TRUE;
      pos++;

      /* cant have args after database-summary */
      if(argc != pos)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
        break;
      }

      retval = L7_SUCCESS;
      break;
    }

    /* at "show ip ospf [areaid] database XXX"
     * check for type parameter
     */
    if(strncmp(argv[pos], pStrInfo_routing_AsbrSummary, strlen(pStrInfo_routing_AsbrSummary)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_ASBR_SUMMARY_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos], pStrInfo_common_External, strlen(pStrInfo_common_External)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_AS_EXTERNAL_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos], pStrInfo_common_WsShowRunningNwMode, strlen(pStrInfo_common_WsShowRunningNwMode)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_NETWORK_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos], pStrInfo_common_NssaExternal_1, strlen(pStrInfo_common_NssaExternal_1)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_NSSA_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos],pStrInfo_common_OpaqueLink , strlen(pStrInfo_common_OpaqueLink)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_LINK_OPAQUE_LSA ;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos], pStrInfo_common_OpaqueArea, strlen(pStrInfo_common_OpaqueArea)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_AREA_OPAQUE_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos], pStrInfo_common_OpaqueAs, strlen(pStrInfo_common_OpaqueAs)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_AS_OPAQUE_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }

    else if(strncmp(argv[pos], pStrInfo_common_Rtr_3, strlen(pStrInfo_common_Rtr_3)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_ROUTER_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else if(strncmp(argv[pos], pStrInfo_common_Dot1xShowSummary, strlen(pStrInfo_common_Dot1xShowSummary)) == 0)
    {
      *isUserLsTypeCfg = L7_TRUE;
      *userLsType      = L7_S_IPNET_SUMMARY_LSA;
      pos++; /* "show ip ospf [areaid] database TYPE XXX" */
    }
    else
    {
      *isUserLsTypeCfg = L7_FALSE;
      *userLsType      = 0;
    }

    /* done? */
    if(argc == pos)
    {
      retval = L7_SUCCESS;
      break;
    }

    /* at "show ip ospf [areaid] database [TYPE] XXX"
     * check for <lsid> (see if it matches a possible next token)
     */
    if((strncmp(argv[pos], pStrInfo_common_AdvRtr_1, strlen(pStrInfo_common_AdvRtr_1)) != 0) &&
       (strncmp(argv[pos], pStrInfo_common_SelfOriginate, strlen(pStrInfo_common_SelfOriginate)) != 0))
    {
      OSAPI_STRNCPY_SAFE(strAddr, argv[pos]);
      if(usmDbInetAton(strAddr, userLsId) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
        break;
      }

      *isUserLsIdCfg = L7_TRUE;
      pos++; /* at "show ip ospf [areaid] database [TYPE] <lsid> XXX" */
    }

    /* done? */
    if(argc == pos)
    {
      retval = L7_SUCCESS;
      break;
    }

    /* at "show ip ospf [areaid] database [TYPE] <lsid> XXX"
     * check for adv-router or self-originate
     */
    if(strncmp(argv[pos], pStrInfo_common_AdvRtr_1, strlen(pStrInfo_common_AdvRtr_1)) == 0)
    {
      /* if no token after adv-router, use our own router-id
       * "... adv-router [<rtrid>] ..."
       * (note that pos is zero based)
       */
      if(argc < (pos+2))
      {
        pos++; /* at "show ip ospf [areaid] database [TYPE] <lsid> adv-router XXX" */
        if(usmDbOspfRouterIdGet(cliGetUnitId(), userAdvRouter) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DispOspfRtr);
          break;
        }
        *isUserAdvRouterCfg = L7_TRUE;
      }
      else
      {
        pos++; /* at "show ip ospf [areaid] database [TYPE] <lsid> adv-router XXX" */

        OSAPI_STRNCPY_SAFE(strAddr, argv[pos]);
        if(usmDbInetAton(strAddr, userAdvRouter) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
          break;
        }

        *isUserAdvRouterCfg = L7_TRUE;
        pos++; /* at "show ip ospf [areaid] database [TYPE] <lsid> adv-router <rtr> XXX" */
      }
    }
    else if(strncmp(argv[pos], pStrInfo_common_SelfOriginate, strlen(pStrInfo_common_SelfOriginate)) == 0)
    {
      if(usmDbOspfRouterIdGet(cliGetUnitId(), userAdvRouter) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_DispOspfRtr);
        break;
      }
      *isUserAdvRouterCfg = L7_TRUE;
      pos++; /* at "show ip ospf [areaid] database [TYPE] <lsid> self-originate XXX" */
    }

    /* done! argc must match pos */
    if(argc != pos)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_ShowRtrOspfDbase);
      break;
    }

    retval = L7_SUCCESS;

  } while (0);

  return retval;
}

/*********************************************************************
 *
 * @purpose Displays one row of OSPF database information
 *
 *
 * @param EwsContext ewsContext
 * @param L7_BOOL sectionHeader
 * @param L7_uint32 intRouterId
 * @param L7_uint32 intAreaId
 * @param L7_uint32 intType
 * @param L7_uint32 intLsId
 * @param L7_ospfLsdbEntry_t * p_Lsa
 *
 * @returntype void
 * @returns
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void showIpOspfDatabaseRow(EwsContext ewsContext,
    L7_BOOL sectionHeader,
    L7_uint32 intRouterId,
    L7_uint32 intAreaId,
    L7_uint32 intType,
    L7_uint32 intLsId,
    L7_ospfLsdbEntry_t * p_Lsa)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strLsaType[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  /* print section heading */
  if(sectionHeader == L7_TRUE)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    memset (strLsaType, 0, sizeof(strLsaType));
    switch (intType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_IllegalLsa);
      break;

    case L7_S_ROUTER_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_RtrLinks);
      break;

    case L7_S_NETWORK_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_NwLinks);
      break;

    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_NwSummary);
      break;

    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_SummaryAsbr);
      break;

    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_AsExternal);
      break;

    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_McastGrp);
      break;

    case L7_S_NSSA_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_NssaLsa);
      break;

    case L7_S_TMP2_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_TempLsa2);
      break;

    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_LinkOpaque);
      break;

    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_AreaOpaque);
      break;

    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_AsOpaque);
      break;

    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_LastLsa);
      break;

    default:
      osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_Blank);
    }

    osapiSnprintfAddBlanks (1, 0, 16, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_States, strLsaType);
    ewsTelnetWrite(ewsContext, stat);

    /* Area (not for external LSAs) */
    if ( (intType != L7_S_AS_EXTERNAL_LSA) && (intType != L7_S_AS_OPAQUE_LSA))
    {
      memset (buf, 0, sizeof(buf));
      rc = usmDbInetNtoa(intAreaId, buf);
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Area, buf);
    }

    /* print column headings */
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_routing_LinkIdAdvRtrAgeSeqChksmOptionsRtrOpt);
    ewsTelnetWrite(ewsContext,"\r\n--------------- --------------- ----- -------- ------ ------- -------");
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    sectionHeader = L7_FALSE;
  }

  /* Display this entry */

  /* Link State ID */
  memset (buf, 0, sizeof(buf));
  rc = usmDbInetNtoa(intLsId, buf);
  ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

  /* Advertising Router ID */
  memset (buf, 0, sizeof(buf));
  rc = usmDbInetNtoa(intRouterId, buf);
  ewsTelnetPrintf (ewsContext, "%-16.15s", buf);

  /* Age */
  osapiSnprintf(buf, sizeof(buf), "%u", p_Lsa->LsdbAge);
  ewsTelnetPrintf (ewsContext, "%-6.5s", buf);

  /* Sequence Number */
  osapiSnprintf(buf, sizeof(buf), "%8.8x", p_Lsa->LsdbSequence);
  ewsTelnetPrintf (ewsContext, "%-9.8s", buf);

  /* Checksum */
  osapiSnprintf(buf, sizeof(buf), "%4.4x", p_Lsa->LsdbChecksum);
  ewsTelnetPrintf (ewsContext, "%-7.7s", buf);

  if ((intType != L7_S_AS_EXTERNAL_LSA) && (intType != L7_S_AS_OPAQUE_LSA))
  {
    /* LSA Options */
    memset (buf, 0, sizeof(buf));
    if ((L7_OSPF_OPT_Q_BIT & p_Lsa->LsdbOptions) != 0)
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Q);
    }
    else
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
    }

    if ((L7_OSPF_OPT_E_BIT & p_Lsa->LsdbOptions) != 0)
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_E);
    }
    else
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
    }

    if ((L7_OSPF_OPT_NP_BIT & p_Lsa->LsdbOptions) != 0)
    {
      /* This bit is the "P" bit in LSAs. It's the "N" bit
       * only in Hello packets. */
      OSAPI_STRNCAT(buf, pStrInfo_common_P_1);
    }
    else
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
    }

    if ((L7_OSPF_OPT_MC_BIT & p_Lsa->LsdbOptions) != 0)
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Mc);
    }
    else
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
    }

    if ((L7_OSPF_OPT_O_BIT & p_Lsa->LsdbOptions) != 0)
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Ipv6TypeOspfIntra);
    }
    else
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
    }

    if ((L7_VPN_OPTION_BIT & p_Lsa->LsdbOptions) != 0)
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_V_2);
    }
    else
    {
      OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
    }
    ewsTelnetPrintf (ewsContext, "%-8.7s", buf);

    /* Router Options (Router LSA only) */
    if (p_Lsa->LsdbType == L7_S_ROUTER_LSA)
    {
      L7_uchar8 rtrLsaFlags = p_Lsa->rtrLsaFlags;

      memset (buf, 0, sizeof(buf));
      if (rtrLsaFlags & L7_OSPFRTRLSAFLAGS_Nt)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Nt);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
      }

      if (rtrLsaFlags & L7_OSPFRTRLSAFLAGS_W)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_W);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
      }

      if (rtrLsaFlags & L7_OSPFRTRLSAFLAGS_V)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_V_2);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
      }

      if (rtrLsaFlags & L7_OSPFRTRLSAFLAGS_E)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_E);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
      }

      if (rtrLsaFlags & L7_OSPFRTRLSAFLAGS_B)
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_B);
      }
      else
      {
        OSAPI_STRNCAT(buf, pStrInfo_common_Dash);
      }
      ewsTelnetPrintf (ewsContext, "%-7.6s", buf);
    }
  }

  cliSyntaxBottom(ewsContext);
}

/*********************************************************************
 *
 * @purpose Displays an LSA within the OSPF database in
 *          detailed format.
 *
 * @param EwsContext ewsContext
 * @param L7_BOOL sectionHeader
 * @param L7_uint32 intRouterId
 * @param L7_uint32 intAreaId
 * @param L7_uint32 intType
 * @param L7_uint32 intLsId
 * @param L7_ospfLsdbEntry_t * p_Lsa
 *
 * @returntype void
 * @returns
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
void showIpOspfDatabaseRowDetailed(EwsContext ewsContext,
    L7_BOOL sectionHeader,
    L7_uint32 intRouterId,
    L7_uint32 intAreaId,
    L7_uint32 intType,
    L7_uint32 intLsId,
    L7_ospfLsdbEntry_t * p_Lsa)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strLsaType[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 *p_LsaInfo = NULL;
  L7_uint32 unit = 0, lsaLen = 0;
  L7_RC_t rc;
  L7_uint32 areaId = intAreaId;

  /* print section heading */
  if(sectionHeader == L7_TRUE)
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);

    memset (strLsaType, 0, sizeof(strLsaType));
    switch (intType)
    {
      case L7_S_ILLEGAL_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_IllegalLsa);
        break;

      case L7_S_ROUTER_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_RtrLinks);
        break;

      case L7_S_NETWORK_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_NwLinks);
        break;

      case L7_S_IPNET_SUMMARY_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_NwSummary);
        break;

      case L7_S_ASBR_SUMMARY_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_SummaryAsbr);
        break;

      case L7_S_AS_EXTERNAL_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_AsExternal);
        break;

      case L7_S_GROUP_MEMBER_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_McastGrp);
        break;

      case L7_S_NSSA_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_NssaLsa);
        break;

      case L7_S_TMP2_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_TempLsa2);
        break;

      case L7_S_LINK_OPAQUE_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_LinkOpaque);
        break;

      case L7_S_AREA_OPAQUE_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_AreaOpaque);
        break;

      case L7_S_AS_OPAQUE_LSA:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_AsOpaque);
        break;

      case L7_S_LAST_LSA_TYPE:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_LastLsa);
        break;

      default:
        osapiSnprintf(strLsaType, sizeof(strLsaType), pStrInfo_common_Blank);
    }
    osapiSnprintfAddBlanks (1, 0, 16, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_States, strLsaType);
    ewsTelnetWrite(ewsContext, stat);

    /* Area (not for external LSAs) */
    if ((intType != L7_S_AS_EXTERNAL_LSA) && (intType != L7_S_AS_OPAQUE_LSA))
    {
      bzero(buf, sizeof(buf));
      rc = usmDbInetNtoa(intAreaId, buf);
      ewsTelnetPrintf (ewsContext, pStrInfo_common_Area, buf);
    }

    if ((intType == L7_S_AS_EXTERNAL_LSA) || (intType  == L7_S_AS_OPAQUE_LSA))
    {
      /* Make sure we set Area ID to zero for AS External LSAs */
      areaId = 0;

    }
  }
  /* Display this entry */

  switch (intType)
  {
    case L7_S_AS_EXTERNAL_LSA:
      rc = usmDbOspfExtLsdbAdvertisementGet(unit, intType, intLsId,
                                            intRouterId, &p_LsaInfo, &lsaLen);
      break;
    case L7_S_AS_OPAQUE_LSA:
      rc = usmDbOspfAsOpaqueLsdbAdvertisementGet(intType, intLsId,
                                                 intRouterId, &p_LsaInfo, &lsaLen);
      break;
    default:
      rc = usmDbOspfLsdbAdvertisementGet(unit, areaId, intType, intLsId,
                                         intRouterId, &p_LsaInfo, &lsaLen);
    break;
  }

  if (rc == L7_SUCCESS)
  {
    (void)usmDbOspfLsaDecode(p_LsaInfo, (outputFuncPtr)ewsTelnetWrite, ewsContext);

    /* We need to free the LSA information that was allocated by OSPF */
    osapiFree(L7_OSPF_MAP_COMPONENT_ID, p_LsaInfo);
  }

  cliSyntaxBottom(ewsContext);
}

/*********************************************************************
 *
 * @purpose displays the OSPF link state database summary in columns
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
 * @cmdsyntax    show ip ospf database
 *
 * @cmdhelp
 *
 * @cmddescript This command has no options.
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfDatabase(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  /* user params */
  static L7_BOOL isSummary = L7_FALSE;
  static L7_BOOL isUserAreaIdCfg = L7_FALSE;
  static L7_uint32 userAreaId = 0;
  static L7_BOOL isUserLsTypeCfg = L7_FALSE;
  static L7_uint32 userLsType = 0;
  static L7_BOOL isUserLsIdCfg = L7_FALSE;
  static L7_uint32 userLsId = 0;
  static L7_BOOL isUserAdvRouterCfg = L7_FALSE;
  static L7_uint32 userAdvRouter = 0;
  L7_BOOL lsaMatches;

  /* current position across calls */
  static L7_uint32 intRouterId;
  static L7_uint32 intAreaId;
  static L7_uint32 intType;
  static L7_uint32 intLsId;
  static L7_BOOL callExtFunc = L7_FALSE;
  L7_uint32 prevAreaId = 0;
  L7_uint32 prevType = 0;

  L7_uint32 count = 0;
  static L7_BOOL first = L7_TRUE;
  static L7_BOOL empty = L7_FALSE;
  static L7_uint32 foundEntry;
  static L7_BOOL sectionHeader = L7_TRUE;
  L7_uint32 unit;
  L7_uint32 numRows = 15;
  L7_uint32 numRowsInHeader = 5;

  static L7_ospfLsdbEntry_t Lsa;
  static L7_ospfOpaqueLsdbEntry_t opaqueLsa;

  /* turn auto-scroll off */
  cliCmdScrollSet(L7_FALSE);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    callExtFunc = L7_FALSE;
    empty = L7_FALSE;
    first = L7_TRUE;
    sectionHeader = L7_TRUE;

    /* init user options */
    isUserAreaIdCfg     = L7_FALSE;
    isUserLsTypeCfg     = L7_FALSE;
    isUserLsIdCfg       = L7_FALSE;
    isUserAdvRouterCfg  = L7_FALSE;
    isSummary           = L7_FALSE;
    userAreaId    = 0;
    userLsType    = 0;
    userLsId      = 0;
    userAdvRouter = 0;

    intRouterId   = 0;
    intAreaId     = 0;
    intType       = 0;
    intLsId       = 0;

    /* parse user options */
    if(parseOspfDbCmdLine(ewsContext, argc, argv, index,
          &isSummary,
          &isUserAreaIdCfg, &userAreaId,
          &isUserLsTypeCfg, &userLsType,
          &isUserLsIdCfg, &userLsId,
          &isUserAdvRouterCfg, &userAdvRouter) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* if the user asks for an area that's not configured, return an error. */
    if (isUserAreaIdCfg && (usmDbOspfAreaEntryGet(unit, userAreaId) != L7_SUCCESS))
    {
      L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
      L7_char8 areaIdStr[L7_CLI_MAX_STRING_LENGTH];
      memset (areaIdStr, 0, sizeof(buf));
      usmDbInetNtoa(userAreaId, areaIdStr);
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_OspfNoArea, areaIdStr);
    }

    if(isUserLsTypeCfg)
    {
      /* If user asks for specific area and T5 externals, give error */
      if (((L7_S_AS_EXTERNAL_LSA == userLsType) || (L7_S_AS_OPAQUE_LSA == userLsType)) && (isUserAreaIdCfg))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_OspfExtDbNoArea_1);
      }
      callExtFunc = ((L7_S_AS_EXTERNAL_LSA == userLsType) || (L7_S_AS_OPAQUE_LSA == userLsType)) ? L7_TRUE : L7_FALSE;
      if (callExtFunc == L7_TRUE)
      {
        empty = L7_TRUE;
      }
    }
  }

  if(isSummary)
  {
    if(showIpOspfDatabaseSummary(ewsContext, first, isUserAreaIdCfg, userAreaId) == L7_TRUE)
    {
      /* show more y/n and continue */
      first = L7_FALSE;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      cliAlternateCommandSet(pStrInfo_routing_ShowIpOspfDbase);
      return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
    }
    else
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  if (first == L7_TRUE)
  {
    sectionHeader = L7_TRUE;
    first = L7_FALSE;

    intAreaId   = (isUserAreaIdCfg) ? userAreaId : 0;
    intType     = (isUserLsTypeCfg) ? userLsType : 0;
    intLsId     = 0;
    intRouterId = 0;

    if (callExtFunc == L7_FALSE)
    {
      foundEntry = usmDbOspfLsdbEntryGet(unit, intAreaId, intType, intLsId, intRouterId, &Lsa);
      if (foundEntry != L7_SUCCESS)
      {
        foundEntry = usmDbOspfLsdbEntryNext(unit, &intAreaId, &intType, &intLsId, &intRouterId, &Lsa);
      }

      /* found anything yet? */
      if (foundEntry != L7_SUCCESS)
      {
        empty = L7_TRUE;
        callExtFunc = L7_TRUE;
      }
      else
      {
        empty = L7_FALSE;
      }
    }

    /*Ext LSDB data if not restricting by type or restricting to externals */
    if (callExtFunc == L7_TRUE && (!isUserLsTypeCfg || userLsType == L7_S_AS_EXTERNAL_LSA || userLsType == L7_S_AS_OPAQUE_LSA))
    {
      intAreaId   = (isUserAreaIdCfg) ? userAreaId : 0;
      intType     = (isUserLsTypeCfg) ? userLsType : 0;
      intLsId     = 0;
      intRouterId = 0;
      if ( !isUserLsTypeCfg)
      {
      foundEntry = usmDbOspfExtLsdbEntryGet(unit, intType, intLsId, intRouterId, &Lsa);
      if (foundEntry != L7_SUCCESS)
      {
        foundEntry = usmDbOspfExtLsdbEntryNext(unit, &intType, &intLsId, &intRouterId, &Lsa);
      }
      if (foundEntry != L7_SUCCESS)
      {  
        foundEntry = usmDbOspfAsOpaqueLsdbEntryGet(intType, intLsId, intRouterId, &opaqueLsa);
        if (foundEntry != L7_SUCCESS)
        {
          foundEntry = usmDbOspfAsOpaqueLsdbEntryNext( &intType, &intLsId, &intRouterId, &opaqueLsa);
        }
        if (foundEntry == L7_SUCCESS)
        {
          memcpy (&Lsa, &opaqueLsa, sizeof(Lsa));
        }
      }
      }
      else
      {
        if (userLsType == L7_S_AS_EXTERNAL_LSA)
        {
          foundEntry = usmDbOspfExtLsdbEntryGet(unit, intType, intLsId, intRouterId, &Lsa);
          if (foundEntry != L7_SUCCESS)
          {
            foundEntry = usmDbOspfExtLsdbEntryNext(unit, &intType, &intLsId, &intRouterId, &Lsa);
          }
        }
        if (userLsType == L7_S_AS_OPAQUE_LSA)
        {
          foundEntry = usmDbOspfAsOpaqueLsdbEntryGet(intType, intLsId, intRouterId, &opaqueLsa);
          if (foundEntry != L7_SUCCESS)
          {
            foundEntry = usmDbOspfAsOpaqueLsdbEntryNext( &intType, &intLsId, &intRouterId, &opaqueLsa);
          }
          if (foundEntry == L7_SUCCESS)
          {
            memcpy (&Lsa, &opaqueLsa, sizeof(Lsa));
          }
        }
      }
    }

    /* no entires? */
    if (foundEntry != L7_SUCCESS)
    {
      if (empty)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_LinkStateDbaseEmpty);
      }

      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  while (count < numRows)
  {
    lsaMatches = L7_TRUE;

    if(foundEntry != L7_SUCCESS)
    {
      lsaMatches = L7_FALSE;
    }
    else if(isUserAreaIdCfg && intAreaId != userAreaId)
    {
      /* we're done */
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    else if(isUserLsTypeCfg && userLsType != intType)
    {
      intLsId     = 0;  /* reset lsid, will check on iteration */
      intRouterId = 0;  /* reset routerid will check on iteration */

      /* have we gone past the type we want? */
      if(intType > userLsType)
      {
        /* get first match in next area */
        intAreaId++;
      }

      /* set type to user requested type */
      intType = userLsType;

      lsaMatches = L7_FALSE;
    }
    else if(isUserLsIdCfg && userLsId != intLsId)
    {
      /* have we gone past the lsid we want? */
      if(intLsId > userLsId)
      {
        /* get first match in next type (dont touch area) */
        intLsId     = 0xffffffff;
        intRouterId = 0xffffffff;
      }

      lsaMatches = L7_FALSE;
    }
    else if(isUserAdvRouterCfg && userAdvRouter != intRouterId)
    {
      lsaMatches = L7_FALSE;
    }

    /* display if LSA matches user criteria */
    if(lsaMatches == L7_TRUE)
    {
      if (userLsType)
      {
        showIpOspfDatabaseRowDetailed(ewsContext, sectionHeader, intRouterId, intAreaId,
            intType, intLsId, &Lsa);
        count +=9;  /* We'll just guess an average number of rows for detailed output */
      }
      else
      {

      showIpOspfDatabaseRow(ewsContext, sectionHeader, intRouterId, intAreaId,
          intType, intLsId, &Lsa);
        count ++;
      }

      if(sectionHeader == L7_TRUE)
      {
        count += numRowsInHeader;
        sectionHeader = L7_FALSE;
      }

      prevAreaId = intAreaId;
      prevType   = intType;
    }

    /* reset first */
    first = L7_FALSE;

    /* get next item */
    if (callExtFunc == L7_FALSE)
    {
      foundEntry = usmDbOspfLsdbEntryNext(unit, &intAreaId, &intType, &intLsId, &intRouterId, &Lsa);

      if(foundEntry == L7_SUCCESS)
      {
        if(isUserLsIdCfg && (intLsId < userLsId))
        {
          intLsId     = (isUserLsIdCfg) ? userLsId : intLsId;
          intRouterId = (isUserAdvRouterCfg) ? userAdvRouter : intRouterId;
          foundEntry = usmDbOspfLsdbEntryGet(unit, intAreaId, intType, intLsId, intRouterId, &Lsa);
        }
      }
      else
      {
        /* try externals if the GetNext failed */
        if (isUserLsTypeCfg || isUserAreaIdCfg)
        {
          /* if we're restricting on type or area ID, we're done */
          return cliSyntaxReturnPrompt (ewsContext, "");
        }

        if (count < numRows)
        {
          intAreaId   = 0;
          intType     = 0;
          intLsId = 0;
          intRouterId = 0;

          foundEntry = usmDbOspfExtLsdbEntryGet(unit, intType, intLsId, intRouterId, &Lsa);
          if (foundEntry != L7_SUCCESS)
          {
            foundEntry = usmDbOspfExtLsdbEntryNext(unit, &intType, &intLsId, &intRouterId, &Lsa);

            if (foundEntry != L7_SUCCESS)
            {
              foundEntry = usmDbOspfAsOpaqueLsdbEntryGet(intType, intLsId, intRouterId, &opaqueLsa);
              if (foundEntry != L7_SUCCESS)
              {
                foundEntry = usmDbOspfAsOpaqueLsdbEntryNext(&intType, &intLsId, &intRouterId, &opaqueLsa);
              }
              if (foundEntry == L7_SUCCESS)
              {
                memcpy(&Lsa, &opaqueLsa, sizeof(Lsa));
              }
            }

            if (foundEntry != L7_SUCCESS)
            {

              return cliSyntaxReturnPrompt (ewsContext, "");
            }
          }
        }

        first = L7_TRUE;
        callExtFunc = L7_TRUE;
      }
    }

    /* Ext LSDB data */
    if (callExtFunc == L7_TRUE)
    {
      /* not the first time? */
      if(first == L7_FALSE)
      {
        /* The usmDbOspfExtLsdbEntryNext function only searches T5 LSAs so if
         * the intType is greater that means an LSId or AdvRouter mismatch is
         * trying to skip forward.
         */
        if(intType > L7_S_AS_OPAQUE_LSA)
        {
          /* if the getNext failed, we're done */
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
       if (intType == L7_S_AS_EXTERNAL_LSA)
       {
        foundEntry = usmDbOspfExtLsdbEntryNext(unit, &intType, &intLsId, &intRouterId, &Lsa);
        if ((foundEntry != L7_SUCCESS) && (!isUserLsTypeCfg))
        {
          intAreaId   = 0;
          intType     = 0;
          intLsId = 0;
          intRouterId = 0;
          foundEntry = usmDbOspfAsOpaqueLsdbEntryNext(&intType, &intLsId, &intRouterId, &opaqueLsa);
          if (foundEntry == L7_SUCCESS)
          {
            memcpy (&Lsa, &opaqueLsa, sizeof(Lsa));
          }

        }
       }        
       else 
       {
         if (intType == L7_S_AS_OPAQUE_LSA)
         {
           foundEntry = usmDbOspfAsOpaqueLsdbEntryNext(&intType, &intLsId, &intRouterId, &opaqueLsa);
           if (foundEntry == L7_SUCCESS)
           {
             memcpy (&Lsa, &opaqueLsa, sizeof(Lsa));
           }
         }
       }
         if(foundEntry != L7_SUCCESS)
         {
           /* if the getNext failed, we're done */
           return cliSyntaxReturnPrompt (ewsContext, "");
         }
      }

      if(foundEntry == L7_SUCCESS)
      {
        /* skip ahead to user specified lsid if needed */
        if(isUserLsIdCfg && (intLsId < userLsId))
        {
          intLsId     = (isUserLsIdCfg) ? userLsId : intLsId;
          intRouterId = (isUserAdvRouterCfg) ? userAdvRouter : intRouterId;

          if (intType == L7_S_AS_EXTERNAL_LSA)
          {
            foundEntry = usmDbOspfExtLsdbEntryGet(unit, intType, intLsId, intRouterId, &Lsa);
          }
          else
          {
            foundEntry = usmDbOspfAsOpaqueLsdbEntryGet(intType, intLsId, intRouterId, &opaqueLsa);
            if (foundEntry == L7_SUCCESS)
            {
              memcpy (&Lsa, &opaqueLsa, sizeof(Lsa));
            }
          }

        }
      }
    }

    /* new section? */
    if(intType != prevType || intAreaId != prevAreaId)
    {
      sectionHeader = L7_TRUE;

      /* prompt user at section break */
      if( (numRows - count) < numRowsInHeader)
      {
        break;
      }
    }

  }   /* end of 'while count < numRows' loop */

  /* show more y/n and continue */
  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  cliAlternateCommandSet(pStrInfo_routing_ShowIpOspfDbase);
  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}

/*********************************************************************
 *
 * @purpose Displays Ospf Virtual Link summary information
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
 * @cmdsyntax    show ip ospf virtual-links brief
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfVirtualLinksBrief(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val;
  L7_uint32 areaId, neighbor;
  L7_char8 areaId_str[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 nbr_str[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRtrOspfVirtualIntfSummary);
  }

  areaId = 0;
  neighbor = 0;

  /* Heading for the router rip interface summary */
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  ewsTelnetWriteAddBlanks (1, 0, 36, 1, L7_NULLPTR, ewsContext,pStrInfo_common_HelloDeadReTxTransit);
  ewsTelnetWriteAddBlanks (1, 0, 4, 2, L7_NULLPTR, ewsContext,pStrInfo_common_AreaIdNeighborIntvlIntvlIntvlDelay);
  ewsTelnetWrite(ewsContext,"\r\n---------------  ---------------  ----------  ----------  ----------  --------\r\n");

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  while (usmDbOspfVirtIfEntryNext(unit, &areaId, &neighbor) == L7_SUCCESS )
  {

    memset (buf, 0,sizeof(buf));
    if (usmDbInetNtoa(areaId, areaId_str) == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "%-17.16s", areaId_str);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-17.16s", pStrInfo_common_Blank);
    }
    ewsTelnetWrite(ewsContext, stat);
    ewsTelnetWrite(ewsContext, buf);
    memset (buf, 0,sizeof(buf));
    if (usmDbInetNtoa(neighbor, nbr_str) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf),  "%-17.16s", nbr_str);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-17.16s", pStrInfo_common_Blank);
    }
    ewsTelnetWrite(ewsContext, buf);

    memset (buf, 0, sizeof(buf));
    if (( usmDbOspfVirtIfHelloIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12d", val);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12s", "-----");
    }
    ewsTelnetWrite(ewsContext, buf);

    memset (buf, 0, sizeof(buf));
    if (( usmDbOspfVirtIfRtrDeadIntervalGet  (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12d", val);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12s", "-----");
    }
    ewsTelnetWrite(ewsContext, buf);

    memset (buf, 0, sizeof(buf));
    if (( usmDbOspfVirtIfRetransIntervalGet   (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12d", val);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12s", "-----");
    }
    ewsTelnetWrite(ewsContext, buf);

    memset (buf, 0, sizeof(buf));
    if (( usmDbOspfVirtIfTransitDelayGet    (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(buf, sizeof(buf),  "%-9d", val);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-9s", "-----");
    }
    ewsTelnetWrite(ewsContext, buf);

    cliSyntaxBottom(ewsContext);
  }                                                                            /* end of while loop */
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Displays Ospf Virtual Link detailed information
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
 * @cmdsyntax    show ip ospf virtual-links
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const char *commandShowIpOspfVirtualLinks(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 val, authType;
  L7_uint32 areaId, neighbor;
  L7_uint32 argAreaId = 1;
  L7_uint32 argNbr = 2;
  L7_char8 areaId_str[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 nbr_str[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uchar8 authKey[L7_AUTH_MAX_KEY_OSPF+1];

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  if (cliNumFunctionArgsGet() != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_ShowRtrOspfVirtualIntfDetailed);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  if (strlen(argv[index+argAreaId]) >= sizeof(areaId_str) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_IsNotAValidAreaId);
  }
  if (strlen(argv[index+argNbr]) >= sizeof(nbr_str) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
  OSAPI_STRNCPY_SAFE(areaId_str,argv[index + argAreaId]);
  OSAPI_STRNCPY_SAFE(nbr_str,argv[index + argNbr]);

  if (usmDbInetAton(areaId_str, &areaId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_IsNotAValidAreaId);
  }

  if (usmDbInetAton(nbr_str, &neighbor) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }

  if ( usmDbOspfVirtIfEntryGet(unit, areaId, neighbor) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_AreaId);
    ewsTelnetPrintf (ewsContext, areaId_str);

    cliFormat(ewsContext, pStrInfo_common_NeighborIpAddr_1);
    ewsTelnetPrintf (ewsContext, nbr_str);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_HelloIntvl);
    if (( usmDbOspfVirtIfHelloIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "-----");
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_DeadIntvl);
    if (( usmDbOspfVirtIfRtrDeadIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "-----");
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_IfTransitDelayIntvl);
    memset (stat, 0, sizeof(stat));
    if (( usmDbOspfVirtIfTransitDelayGet (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "-----");
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_ReTxIntvl);
    if (( usmDbOspfVirtIfRetransIntervalGet (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "-----");
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_State);
    if (usmDbOspfVirtIfStateGet(unit, areaId, neighbor, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_INTF_DOWN:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
        break;
      case L7_OSPF_INTF_LOOPBACK:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_LoopBack);
        break;
      case L7_OSPF_INTF_WAITING:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Waiting_2);
        break;
      case L7_OSPF_INTF_POINTTOPOINT:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_PointToPoint_2);
        break;
      case L7_OSPF_INTF_DESIGNATEDROUTER:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_DesignatedRtr_2);
        break;
      case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_BackupDesignatedRtr_2);
        break;
      case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_OtherDesignatedRtr_1);
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

    /* Virtual link metric */
    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_Metric);
    if (( usmDbOspfVirtIfMetricGet (unit, areaId, neighbor, &val) == L7_SUCCESS ))
    {
      osapiSnprintf(stat, sizeof(stat),  "%u", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "-----");
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_NeighborState);
    if (usmDbOspfVirtNbrStateGet(unit, areaId, neighbor, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_VIRT_NBR_STATE_DOWN:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
        break;
      case L7_OSPF_VIRT_NBR_STATE_ATTEMPT:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Attempt);
        break;
      case L7_OSPF_VIRT_NBR_STATE_INIT:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Init);
        break;
      case L7_OSPF_VIRT_NBR_STATE_TWOWAY:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_TwoWay);
        break;
      case L7_OSPF_VIRT_NBR_STATE_EXCHANGE_START:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_ExchangeStart);
        break;
      case L7_OSPF_VIRT_NBR_STATE_EXCHANGE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Exchange);
        break;
      case L7_OSPF_VIRT_NBR_STATE_LOADING:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Loading);
        break;
      case L7_OSPF_VIRT_NBR_STATE_FULL:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Full);
        break;
      default:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Down_2);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    cliFormat(ewsContext, pStrInfo_common_AuthType_1);
    if (usmDbOspfVirtIfAuthTypeGet (unit, areaId, neighbor, &authType) == L7_SUCCESS)
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
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Blank);
      }
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    /* Auth Key */
    if (authType == L7_AUTH_TYPE_SIMPLE_PASSWORD || authType == L7_AUTH_TYPE_MD5)
    {
      cliFormat(ewsContext, pStrInfo_common_AuthKey);
      if (usmDbOspfVirtIfAuthKeyActualGet(unit, areaId, neighbor, authKey) == L7_SUCCESS)
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
      if (usmDbOspfVirtIfAuthKeyIdGet(unit, areaId, neighbor, &val) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, "%u", val);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_common_Line);
      }
    }

  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_VirtualIntfIsNotActive);  /* "This virtual interface is not active"*/
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
 *
 * @purpose  Displays trap flag info for OSPF
 *
 *
 * @param EwsContext ewsContext
 *
 * @returntype   void
 * @returns      nothing
 *
 * @notes        This is a helper function called from
 *               commandShowTrapFlags for show the OSPF Traps
 *
 * @cmdsyntax     show trapflags
 *
 * @cmdhelp
 *
 * @end
 *
 *********************************************************************/
void commandShowOspfTrapflags(EwsContext ewsContext)
{
  L7_uint32 val;
  L7_uint32 unit;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  if ( (usmDbOspfTrapFlagsGet(unit, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if((val & L7_OSPF_TRAP_ALL) == 0)
    {
      cliFormat(ewsContext, pStrInfo_routing_OspfTraps_1);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Dsbl_1);
    }
    else
    {
      ewsTelnetPrintf (ewsContext, "\n\r\n%s:", pStrInfo_routing_OspfTraps_1);
    }
  }
  if(val & L7_OSPF_TRAP_ERRORS_ALL)
  {
    ewsTelnetPrintf (ewsContext, "\r\n  %s:", pStrErr_common_OspfTrapErrs);

     if(val & L7_OSPF_TRAP_IF_AUTH_FAILURE)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrErr_common_OspfTrapAuthFailure);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_RX_BAD_PACKET)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapBadPkt);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_IF_CONFIG_ERROR)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrErr_common_OspfTrapCfg);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrErr_common_OspfTrapVirtAuthFailure);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapVirtBadPkt);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrErr_common_OspfTrapVirtCfg);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }

  } /* endif checking enabled error flags */

  if(val & L7_OSPF_TRAP_LSA_ALL )
  {
    ewsTelnetPrintf (ewsContext, "\r\n  %s:", pStrInfo_common_OspfTrapLsa);

    if(val & L7_OSPF_TRAP_MAX_AGE_LSA)
    {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapMaxAgeLsa);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_ORIGINATE_LSA)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s",pStrInfo_common_OspfTrapOriginateLsa);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
  } /* endif checking enabled lsa flags */

  if(val & L7_OSPF_TRAP_OVERFLOW_ALL)
  {
    ewsTelnetPrintf (ewsContext, "\r\n  %s:", pStrInfo_common_OspfTrapOverflow);

    if(val & L7_OSPF_TRAP_LS_DB_OVERFLOW)
    {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrap);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
    }
    if(val & L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
    {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapDbAp);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
    }
  } /* endif checking enabled overflow flags */

  if(val & L7_OSPF_TRAP_RETRANSMIT_ALL)
  {
    ewsTelnetPrintf (ewsContext, "\r\n  %s:", pStrInfo_common_OspfTrapReTx);

    if(val & L7_OSPF_TRAP_TX_RETRANSMIT)
    {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapTxReTx);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
    }
    if(val & L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
    {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapVirtIfTxReTx);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
    }
  } /* endif checking enabled retransmit flags */

  if(val & L7_OSPF_TRAP_STATE_CHANGE_ALL )
  {
    ewsTelnetPrintf (ewsContext, "\r\n  %s:", pStrInfo_common_OspfTrapStateChg);

     if(val & L7_OSPF_TRAP_IF_STATE_CHANGE)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s",pStrInfo_common_OspfTrapIfStateChg);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_NBR_STATE_CHANGE)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapNbrStateChg);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s",  pStrInfo_common_OspfTrapVirtIfStateChg);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
     if(val & L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE)
     {
      osapiSnprintf(buf, sizeof(buf), "     %s", pStrInfo_common_OspfTrapVirtNbrStateChg);
      cliFormat(ewsContext, buf);
      ewsTelnetWrite(ewsContext, pStrInfo_common_Enbl_1);
     }
  } /* endif checking enabled state-change flags */

  return;
}

/*********************************************************************
 *
 * @purpose    to display the stub table
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
 * @notes     none
 *
 * @cmdsyntax    show ip ospf stub table
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/

const char *commandShowIpOspfStubTable(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_RC_t rc;
  L7_uint32 areaId = 0;
  L7_int32 TOS = 0;
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;
  L7_uint32 unit = 0;
  L7_uint32 argTable=1;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_ShowRtrOspfStubTbl_1);
  }

  if (strcmp(argv[index+argTable], pStrInfo_common_Tbl_2) != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_ShowRtrOspfStubTbl_1);
  }

  if (usmDbOspfInitialized(unit) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_MustInitOspfSwitch);
  }

  if ( ( usmDbOspfStubAreaEntryGet(unit, areaId, TOS ) == L7_SUCCESS) ||
      ( usmDbOspfStubAreaEntryNext(unit, &areaId, &TOS ) == L7_SUCCESS) )
  {
    ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_AreaidTypeofserviceMetricValImportSummarylsa);
    ewsTelnetWrite(ewsContext,"\r\n---------------- ------------- ---------- -----------------");
    cliSyntaxBottom(ewsContext);
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_NoStubAreaCfgured);
  }

  do
  {
    /*areaId */
    memset (buf, 0,sizeof(buf));
    memset (stat, 0, sizeof(stat));
    rc = usmDbInetNtoa(areaId, stat);
    ewsTelnetPrintf (ewsContext, "%-17.16s", stat);

    ewsTelnetPrintf (ewsContext, "%-14.13s", pStrInfo_common_Normal);

    /*Stub Area Default Metric for TOS Normal*/
    memset (stat, 0, sizeof(stat));
    rc = usmDbOspfStubMetricGet(unit, areaId, L7_TOS_NORMAL_SERVICE, &val);
    if(rc == L7_SUCCESS)
    {
      osapiSnprintf(stat, sizeof(stat),  "%-11d", val);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat),  "%-11.10s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, stat);

    memset (stat, 0, sizeof(stat));
    if (usmDbOspfAreaSummaryGet(unit, areaId, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_OSPF_AREA_SEND_SUMMARY:
        osapiSnprintf(stat, sizeof(stat),  "%-17s", pStrInfo_common_Enbl_1);  /* enable */
        break;
      case L7_OSPF_AREA_NO_SUMMARY:
        osapiSnprintf(stat, sizeof(stat),  "%-17s", pStrInfo_common_Dsbl_1);  /* disable */
        break;
      default:
        osapiSnprintf(stat, sizeof(stat),  "%-17s", pStrInfo_common_Blank);  /* ------- */
      }
    }
    ewsTelnetWrite(ewsContext, stat);
    cliSyntaxBottom(ewsContext);
  }
  while (usmDbOspfStubAreaEntryNext(unit, &areaId, &TOS ) == L7_SUCCESS);

  return cliSyntaxReturnPrompt (ewsContext, "");
}

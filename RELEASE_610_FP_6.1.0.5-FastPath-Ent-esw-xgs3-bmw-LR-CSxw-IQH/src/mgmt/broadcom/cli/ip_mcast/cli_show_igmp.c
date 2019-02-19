/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_show_igmp.c
 *
 * @purpose show commands for the igmp cli functionality
 *
 * @component user interface
 *
 * @comments
 *           An output value of Err means that the usmDb returned != SUCCESS   english num 1312
 *           An output value of ------ means that the value type was incompatible,
 *           and even though the usmDb return SUCCESS; the value is garbage.
 *
 * @create
 *
 * @author  srikrishnas
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include <cliapi.h>
#include <datatypes.h>
#include <usmdb_ip_api.h>
#include <usmdb_util_api.h>
#include <clicommands_l3.h>
#include <clicommands_mcast.h>
#include <l3_commdefs.h>
#include <usmdb_mib_igmp_api.h>
#include <usmdb_igmp_api.h>
#include "usmdb_l3.h"
#include "cli_web_exports.h"
#include "usmdb_common.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"
#endif

#include "l3_mcast_defaultconfig.h"
#include "l7_mgmd_api.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  Displays igmp information in the router
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
* @cmdsyntax    show ip igmp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpIgmp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intSlot, intPort, interface;
  L7_uint32 nextInterface;
  L7_uint32 val = L7_DISABLE;
  L7_uint32 unit, numArgs;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpIgmp);
  }

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_ipmcast_IgmpAdminMode);
  if (usmDbMgmdAdminModeGet (unit, L7_AF_INET, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, stat);

  memset(stat, L7_NULL, sizeof(stat));
  cliFormat(ewsContext, pStrInfo_ipmcast_IgmpRouterAlertCheck);
  if (usmDbMgmdRouterAlertMandatoryGet (unit, L7_AF_INET, &val) == L7_SUCCESS)
  {
    osapiSnprintf(stat,sizeof(stat),strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
  }
  else
  {
    osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
  }

  ewsTelnetWrite(ewsContext, stat);
  /* Heading for the router IGMP interface summary */
  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox);
  }

  /* Display the IGMP interface info */
  ewsTelnetWriteAddBlanks (2, 0, 4, 12, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IgmpIntfStatus);
  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_IntfIntfModeProtoState);
  ewsTelnetWrite(ewsContext,"\r\n---------  --------------  ----------------\r");

/* start for loop for all the interfaces */
  while (interface)
  {
    /* display only the visible interfaces - and non loop/tunnel for now */
    if ((usmDbMgmdIsValidIntf(unit, interface) == L7_TRUE)  &&
        (usmDbIpIntfExists(unit, interface) == L7_TRUE))
    {
      memset(buf, L7_NULL, sizeof(buf));
      memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
      if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
      }
      ewsTelnetPrintf (ewsContext, "\r\n%-11.9s",buf);

      memset(stat, L7_NULL, sizeof(stat));
      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbMgmdInterfaceModeGet(unit, L7_AF_INET,
                                    interface, &val) == L7_SUCCESS)
      {
        switch(val)
        {
          case L7_ENABLE :
            osapiSnprintf(buf, sizeof(buf),"%-16s",pStrInfo_common_Enbld);
            break;
          case L7_DISABLE:
            osapiSnprintf(buf, sizeof(buf),"%-16s",pStrInfo_common_Dsbld);
            break;
          default:
            osapiSnprintf(buf, sizeof(buf),"%-16s",pStrErr_common_Err);
            break;
        }
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", "-----");
      }
      ewsTelnetWrite(ewsContext, buf);

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbMgmdInterfaceOperationalStateGet(unit, L7_AF_INET,
                                                interface) == L7_TRUE)
      {
        osapiSnprintf(buf, sizeof(buf),  " %-16s", pStrInfo_common_Operational);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  " %-16s", pStrInfo_common_NonOperational);
      }
      ewsTelnetWrite(ewsContext, buf);
      /*end if interface exists*/
    }  /* end if visible interface */

    /* Get the next interface */
    if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
    {
      interface = nextInterface;
    }
    else
    {
      interface   = 0;
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays igmp information for the specified interface
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
* @cmdsyntax    show ip igmp interface <slot/port>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const char *commandShowIpIgmpInterface(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 val = L7_DISABLE;
  L7_int32 u;
  L7_uint32 intIface, s, p, slot, port;
  L7_uint32 unit, numArgs;
  L7_IP_ADDR_t ipAddr;
  L7_IP_MASK_t mask;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpIgmpIntf_1, cliSyntaxInterfaceHelp());
  }

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
    if (cliSlotPortToInterface(argv[index + argSlotPort], &unit, &s, &p, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  memset(buf, L7_NULL, sizeof(buf));
  cliFormat(ewsContext, pStrInfo_common_Intf);
  if ( usmDbUnitSlotPortGet(intIface, &u, &slot, &port) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(u, slot, port));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
  }
  ewsTelnetWrite(ewsContext, buf);

  if (usmDbIpRtrIntfIpAddressGet(u, intIface, &ipAddr, &mask) == L7_SUCCESS)
  {
    /* ip addr */
    memset(buf, L7_NULL, sizeof(buf));
    usmDbInetNtoa(ipAddr, buf);
    cliFormat(ewsContext, pStrInfo_ipmcast_IpAddress);
    ewsTelnetWrite(ewsContext, buf);

    /* subnet mask */
    memset(buf, L7_NULL, sizeof(buf));
    usmDbInetNtoa(mask, buf);
    cliFormat(ewsContext, pStrInfo_ipmcast_SubnetMask);
    ewsTelnetWrite(ewsContext, buf);
  }
  else
  {
    cliFormat(ewsContext, pStrInfo_ipmcast_IpAddress);
    osapiSnprintf(buf, sizeof(buf), "-------");
    ewsTelnetWrite(ewsContext, buf);

    cliFormat(ewsContext, pStrInfo_ipmcast_SubnetMask);
    osapiSnprintf(buf, sizeof(buf), "-------");
    ewsTelnetWrite(ewsContext, buf);

  }



  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbMgmdAdminModeGet (unit, L7_AF_INET, &val) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_ipmcast_IgmpAdminMode);
    switch (val)
    {
      case L7_ENABLE :
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld);
        break;
      case L7_DISABLE:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld);
        break;
      default:
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        break;
    }
    ewsTelnetPrintf (ewsContext, buf);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbMgmdInterfaceModeGet(unit, L7_AF_INET, intIface, &val) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_IntfMode);
    switch (val)
    {
      case L7_ENABLE :
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Enbld);
        break;
      case L7_DISABLE:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld);
        break;
      default:
        osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
        break;
    }
    ewsTelnetPrintf (ewsContext, buf);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbMgmdInterfaceVersionGet(unit, L7_AF_INET,
                                   intIface, &val) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_IgmpVer);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdInterfaceQueryIntervalGet(unit, L7_AF_INET,
                                          intIface, &val) == L7_SUCCESS))
  {
    cliFormat(ewsContext, pStrInfo_common_QueryIntvlSecs);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdInterfaceQueryMaxResponseTimeGet(unit, L7_AF_INET,
                                                 intIface, &val) == L7_SUCCESS))
  {
    cliFormat(ewsContext, pStrInfo_common_QueryMaxRespTime110OfASecond);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdInterfaceRobustnessGet(unit, L7_AF_INET,
                                       intIface, &val) == L7_SUCCESS))
  {
    cliFormat(ewsContext, pStrInfo_common_Robustness);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdInterfaceStartupQueryIntervalGet(unit, L7_AF_INET,
                                                 intIface, &val) == L7_SUCCESS))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_StartupQueryIntvlSecs);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdInterfaceStartupQueryCountGet(unit, L7_AF_INET,
                                              intIface, &val) == L7_SUCCESS))
  {
    cliFormat(ewsContext, pStrInfo_common_StartupQueryCount);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdInterfaceLastMembQueryIntervalGet(unit, L7_AF_INET,
                                                  intIface, &val) == L7_SUCCESS))
  {
    cliFormat(ewsContext, pStrInfo_common_LastMbrQueryIntvl110OfASecond);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((usmDbMgmdLastMembQueryCountGet(unit, L7_AF_INET,
                                      intIface, &val) == L7_SUCCESS))
  {
    cliFormat(ewsContext, pStrInfo_common_LastMbrQueryCount);
    ewsTelnetPrintf (ewsContext, "%u", val);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays igmp interface statistics
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
* @cmdsyntax    show ip igmp interface stats <slot/port>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const char *commandShowIpIgmpInterfaceStats(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 querierMode, val = L7_DISABLE;
  L7_uint32 intIface, s, p;
  L7_IP_ADDR_t ipAddr;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetIpAddr;
  usmDbTimeSpec_t timeSpec;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpIgmpIntfStats_1, cliSyntaxInterfaceHelp());
  }

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
    if (cliSlotPortToInterface(argv[index + argSlotPort], &unit, &s, &p, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if(usmDbMgmdInterfaceOperationalStateGet(unit,
                                           L7_AF_INET, intIface) == L7_TRUE)
  {
    cliFormat(ewsContext, pStrInfo_common_QuerierStatus);
    if (usmDbMgmdInterfaceQuerierStatusGet(unit, L7_AF_INET, intIface,
                                           &querierMode) == L7_SUCCESS)
    {
      switch (querierMode)
      {
      case L7_FALSE:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NonQuerier);
        break;
      case L7_TRUE:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Querier);
        break;
      default:
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NonQuerier);
        break;
      }
      ewsTelnetWrite(ewsContext,buf);
    }

    memset(stat, L7_NULL, sizeof(stat));
    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbMgmdInterfaceQuerierGet(unit, L7_AF_INET, intIface,
                                      &inetIpAddr) == L7_SUCCESS))
    {
      cliFormat(ewsContext, pStrInfo_common_QuerierIpAddr);
      inetAddressGet(L7_AF_INET, &inetIpAddr, &ipAddr);
      if (usmDbInetNtoa(ipAddr, buf) == L7_SUCCESS)
      {
        ewsTelnetPrintf (ewsContext, buf);
      }
    }

    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbMgmdInterfaceQuerierUpTimeGet(unit, L7_AF_INET,
                                            intIface, &val) == L7_SUCCESS))
    {
      cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_QuerierUpTimeSecs);
      memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
      osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
      ewsTelnetPrintf (ewsContext, "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                       timeSpec.minutes,timeSpec.seconds);
    }

    memset(buf, L7_NULL, sizeof(buf));
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_QuerierExpiryTimeSecs);
    if (querierMode == L7_TRUE)
    /*if it is a querier - dont show expiry time*/
    {
      val = 0;
      memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
      osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
      osapiSnprintf(buf, sizeof(buf), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                         timeSpec.minutes,timeSpec.seconds);
    }
    else
    {
      if ((usmDbMgmdInterfaceQuerierExpiryTimeGet(unit, L7_AF_INET, intIface,
                                                  &val) == L7_SUCCESS))
      {
        memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
        osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
        osapiSnprintf(buf, sizeof(buf), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                         timeSpec.minutes,timeSpec.seconds);
      }
    }
    ewsTelnetWrite(ewsContext,buf);

    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbMgmdInterfaceWrongVersionQueriesGet(unit, L7_AF_INET, intIface,
                                                  &val) == L7_SUCCESS))
    {
      cliFormat(ewsContext, pStrInfo_common_WrongVerQueries);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbMgmdInterfaceJoinsGet(unit, L7_AF_INET, intIface,
                                    &val) == L7_SUCCESS))
    {
      cliFormat(ewsContext, pStrInfo_common_NumOfJoins);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }

    memset(buf, L7_NULL, sizeof(buf));
    if ((usmDbMgmdInterfaceGroupsGet(unit, L7_AF_INET, intIface,
                                     &val) == L7_SUCCESS))
    {
      cliFormat(ewsContext, pStrInfo_common_NumOfGrps);
      ewsTelnetPrintf (ewsContext, "%u", val);
    }
  }
  else
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_IntfIsNotOperational);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays the registered multicast groups on the interface
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
* @cmdsyntax    show ip igmp groups <slot/port> [detail]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpIgmpGroups(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{

  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort = 1;
  L7_uint32 argDetail = 2;
  L7_uint32 val = L7_DISABLE;
  L7_uint32 intIface, s, p;
  static L7_uint32 indexIntIface;
  static L7_IP_ADDR_t ipAddr;
  L7_IP_ADDR_t ipmask;
  L7_RC_t rc;
  L7_uint32 unit, numArgs;
  L7_uint32 compatMode = 0;
  L7_uint32 sfMode = 0;
  L7_uint32 iterator;
  L7_BOOL ISv3 = L7_FALSE;
  L7_uint32 lineCount = 0;
  static L7_BOOL firstIteration = L7_TRUE;
  L7_inet_addr_t inetAddr, inetIpAddr;
  usmDbTimeSpec_t timeSpec;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if ((numArgs != 1) && (numArgs != 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpIgmpGrps_1, cliSyntaxInterfaceHelp());
  }

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
    if (cliSlotPortToInterface(argv[index + argSlotPort], &unit, &s, &p, &intIface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }

  if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    ipAddr =0;
    inetAddressSet(L7_AF_INET, &ipAddr, &inetIpAddr);
    indexIntIface = 0;
    firstIteration = L7_TRUE;
  }
  else
  {
    inetAddressSet(L7_AF_INET, &ipAddr, &inetIpAddr);
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  if ((numArgs == 2) && (strcmp(argv[index+argDetail], pStrInfo_common_IgmpProxyGrpsDetail) == 0))
  {

    if (usmDbIpRtrIntfModeGet(unit, intIface, &val) == L7_SUCCESS && val != L7_ENABLE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_common_RoutingIsNotEnbldOnPort, argv[index + argSlotPort]);
    }

    if (usmDbMgmdInterfaceOperationalStateGet(unit, L7_AF_INET, intIface) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_IntfIsNotOperational);
    }

    ewsTelnetWriteAddBlanks (1, 0, 18, 16, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_RegisteredMcastGrpDetails);
    ewsTelnetWriteAddBlanks (1, 0, 51, 1, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_Ver1Ver2Grp);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_McastLastUpExpiryHostHostCompat);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IpAddrReporterTimeTimeTimerTimerMode);
    ewsTelnetWrite(ewsContext,"\r\n--------------- ------------ ---------- ---------- ---------- ---------- ------");

    for (iterator = 0; iterator < 15;)
    {
      if (usmDbMgmdCacheEntryNextGet(unit, L7_AF_INET,
                                     &inetIpAddr, &indexIntIface) !=  L7_SUCCESS)
      {
        return cliSyntaxReturnPrompt (ewsContext, "");
      }

      ISv3 = L7_FALSE;

      if (indexIntIface == intIface)
      {
        /* Increase the iterator value only if we are displaying a row */
        iterator++;

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpAddr, &ipAddr);
        rc = usmDbInetNtoa(ipAddr, buf);
        ewsTelnetPrintf (ewsContext, "\r\n%-16s",buf);

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        if (usmDbMgmdCacheLastReporterGet(unit, L7_AF_INET, &inetIpAddr,
                                          intIface, &inetAddr) == L7_SUCCESS)
        {
          inetAddressGet(L7_AF_INET, &inetAddr, &val);
          rc = usmDbInetNtoa(val, buf);
          osapiSnprintf(stat, sizeof(stat), "%-13s",buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat),  "%-13s", pStrErr_common_Err);
        }

        /* Last Reporter */
        ewsTelnetWrite(ewsContext, stat);

        if ((usmDbMgmdCacheGroupCompatModeGet(unit, L7_AF_INET, &inetIpAddr,
                                              intIface, &compatMode) == L7_SUCCESS) && (compatMode == 3))
        {
          ISv3 = L7_TRUE;
          if (usmDbMgmdCacheGroupSourceFilterModeGet(unit, L7_AF_INET,
                                                     &inetIpAddr, intIface, &sfMode) != L7_SUCCESS)
          {
            sfMode = MGMD_FILTER_MODE_EXCLUDE;
          }              /* Exclude */
        }
        val = 0;
        if (usmDbMgmdCacheUpTimeGet(unit, L7_AF_INET, &inetIpAddr, intIface,
                                    &val) == L7_SUCCESS)
        {
          memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
          osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d   ", timeSpec.hours,
                        timeSpec.minutes,timeSpec.seconds);
        } else
        {
          osapiSnprintf(stat, sizeof(stat),  "%-11s", pStrErr_common_Err);
        }
        ewsTelnetWrite(ewsContext, stat);  /* Up Time */


        if ((ISv3 == L7_TRUE) && (sfMode == 1))   /* v3 and Exclude */
        {
          ewsTelnetPrintf (ewsContext, "%-11s", "-----");  /* Expiry Time */
        }
        else
        {
          val = 0;
          if (usmDbMgmdCacheExpiryTimeGet(unit, L7_AF_INET, &inetIpAddr,
                                          intIface, &val) == L7_SUCCESS)
          {
            memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
            osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
            osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d   ", timeSpec.hours,
                                             timeSpec.minutes,timeSpec.seconds);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-11s", pStrErr_common_Err);
          }
          ewsTelnetWrite(ewsContext, stat);  /* Expiry Time */
        }

        if (compatMode != 1)
        {
          osapiSnprintf(stat, sizeof(stat),  "%-11s", "-----");
        }
        else
        {
          if (usmDbMgmdCacheVersion1HostTimerGet(unit, L7_AF_INET, &inetIpAddr,
                                                 intIface, &val) == L7_SUCCESS)
          {
            memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
            osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
            osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d   ", timeSpec.hours,
                                             timeSpec.minutes,timeSpec.seconds);

          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%-11s", "-----");
          }
        }
        ewsTelnetWrite(ewsContext, stat);  /* Version1 Host Timer */

        if (compatMode != 2)
        {
          osapiSnprintf(stat, sizeof(stat),  "%-11s", "-----");
        }
        else
        {
          if (usmDbMgmdCacheVersion2HostTimerGet(unit, L7_AF_INET, &inetIpAddr,
                                                 intIface, &val) == L7_SUCCESS)
          {
            memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
            osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
            osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d   ", timeSpec.hours,
                                             timeSpec.minutes,timeSpec.seconds);

          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-11s", "-----");
          }
        }
        ewsTelnetWrite(ewsContext, stat);  /* Version2 Host Timer */

        if (usmDbMgmdCacheGroupCompatModeGet(unit, L7_AF_INET, &inetIpAddr,
                                             intIface, &compatMode) == L7_SUCCESS)
        {
          if (compatMode == 1)
          {
            osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_common_V1);
          }
          else if (compatMode == 2)
          {
            osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_common_V2);
          }
          else if (compatMode == 3)
          {
            osapiSnprintf(stat, sizeof(stat), "%-6s", pStrInfo_common_V3);
          }
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat),  " %-6s", "-----");
        }
        ewsTelnetWrite(ewsContext, stat);   /* Group compat mode */
      }
    }

    cliSetCharInputID(1, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_ShowIpIgmpGrpsDetail, argv[index + argSlotPort]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */

  }
  else if(numArgs == 1)
  {
    memset(buf, L7_NULL, sizeof(buf));
    memset(stat, L7_NULL, sizeof(stat));
    if (firstIteration == L7_TRUE)
    {

      if (usmDbIpRtrIntfIpAddressGet(unit, intIface, &ipAddr,
                                     &ipmask) == L7_SUCCESS)
      {
        rc = usmDbInetNtoa(ipAddr, buf);
        osapiSnprintf(stat, sizeof(stat), buf);
        cliFormat(ewsContext, pStrInfo_common_ApIpAddr);
        ewsTelnetWrite(ewsContext, stat);

        memset(buf, L7_NULL, sizeof(buf));
        memset(stat, L7_NULL, sizeof(stat));
        rc = usmDbInetNtoa(ipmask, buf);
        osapiSnprintf(stat, sizeof(stat), buf);
        cliFormat(ewsContext, pStrInfo_common_SubnetMask_3);
        ewsTelnetWrite(ewsContext, stat);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        cliFormat(ewsContext, pStrInfo_common_ApIpAddr);
        ewsTelnetWrite(ewsContext, stat);

        osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err);
        cliFormat(ewsContext, pStrInfo_common_SubnetMask_3);
        ewsTelnetWrite(ewsContext, stat);
      }

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbMgmdInterfaceModeGet(unit, L7_AF_INET, intIface,
                                    &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_IntfMode);
        ewsTelnetPrintf (ewsContext, strUtilEnabledDisabledGet(val,pStrInfo_common_Dsbl_1));
      }

      memset(buf, L7_NULL, sizeof(buf));
      if (usmDbMgmdInterfaceQuerierStatusGet(unit, L7_AF_INET, intIface,
                                             &val) == L7_SUCCESS)
      {
        cliFormat(ewsContext, pStrInfo_common_QuerierStatus);
        switch (val)
        {
        case L7_DISABLE:
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NonQuerier);
          break;
        case L7_ENABLE:
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Querier);
          break;
        default:
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NonQuerier);
          break;
        }
      }
      ewsTelnetWrite(ewsContext,buf);
      firstIteration = L7_FALSE;
      lineCount = lineCount + 5;
    }
    /* Listing the Multicast Ip addresses*/

    while ((lineCount < CLI_MAX_SCROLL_LINES-6) &&
           (usmDbMgmdCacheEntryNextGet(unit, L7_AF_INET, &inetIpAddr,
                                       &indexIntIface) ==  L7_SUCCESS))
    {
      if (indexIntIface == intIface)
      {
        lineCount++;
        memset(buf, L7_NULL, sizeof(buf));
        memset(buf, L7_NULL, sizeof(stat));
        inetAddressGet(L7_AF_INET, &inetIpAddr, &ipAddr);
        rc = usmDbInetNtoa(ipAddr, buf);
        osapiSnprintf(stat, sizeof(stat),buf);
        cliFormat(ewsContext, pStrInfo_ipmcast_McastGrpIp);
        ewsTelnetWrite(ewsContext, stat);
      }
    }
    if (lineCount >= CLI_MAX_SCROLL_LINES-6)
    {
      cliSetCharInputID(1, ewsContext, argv);
      cliSyntaxBottom(ewsContext);
      osapiSnprintf(buf, sizeof(buf), argv[0]);
      for (lineCount=1; lineCount<argc; lineCount++)
      {
        OSAPI_STRNCAT(buf, " ");
        OSAPI_STRNCAT(buf, argv[lineCount]);
      }
      cliAlternateCommandSet(buf);
      return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }
  firstIteration = L7_TRUE;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays the list of interfaces subscribed with the specified multicast ipaddr
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
* @cmdsyntax    show ip igmp interface membership <multiipaddr> [detail]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const char *commandShowIpIgmpInterfaceMembership(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIPaddr = 1;
  L7_uint32 argDetail = 2;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intSlot, intPort, interface;
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nextInterface;
  L7_uint32 val = L7_DISABLE;
  L7_uint32 expTime = 0;
  L7_uint32 compatMode = 0;
  L7_uint32 ipVal;
  L7_IP_ADDR_t ipAddress, ipmask;
  static L7_uint32 indexIntIface;
  L7_RC_t rc;
  L7_uint32 iterator;
  L7_BOOL ISv3 = L7_FALSE;
  L7_uint32 unit, numArgs;
  L7_uint32 groupAddr,hostAddr;
  L7_inet_addr_t inetIpVal;
  L7_inet_addr_t inetGroupAddr, inetHostAddr;
  L7_uint32 intIfNum;
  L7_inet_addr_t inetIpValNext;
  usmDbTimeSpec_t timeSpec;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if ((numArgs != 1) && (numArgs != 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpIgmpIntfMbrship_1);
  }

  if (usmDbMgmdAdminModeGet(unit, L7_AF_INET, &val) == L7_SUCCESS)
  {
    if (val != L7_ENABLE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_ipmcast_MustInitIgmp);
    }
  }

  OSAPI_STRNCPY_SAFE(strIPaddr,argv[index + argIPaddr]);

  if (usmDbInetAton(strIPaddr, (L7_uint32 *)&ipVal) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInvalidIpAddr);
  }
  inetAddressSet(L7_AF_INET, &ipVal, &inetIpVal);

  if (inetIsInMulticast(&inetIpVal) != L7_TRUE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInvalidMcastAddr);
  }
  /* Heading for iterating through all the interfaces */
  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox);
  }

  if ((numArgs == 2) && (strcmp(argv[index+argDetail], pStrInfo_common_IgmpProxyGrpsDetail) == 0))
  {
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      if(L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }

    ewsTelnetWriteAddBlanks (1, 0, 13, 14, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IgmpIntfDetailedMbrshipInfo);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IntfGrpCompatSrcFilterSrcHostsExpiryTime);
    ewsTelnetWriteAddBlanks (1, 0, 16, 36, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_ModeMode);
    ewsTelnetWrite(ewsContext,"\r\n----------   -------------   --------------   --------------  ------------");

    /* start for loop for all the interfaces */
    while (interface)
    {
      /* display only the visible interfaces - and non loop/tnnl for now */
      if ((usmDbMgmdIsValidIntf(unit, interface) == L7_TRUE) &&
          (usmDbIpIntfExists(unit, interface) == L7_TRUE))
      {
        if (usmDbMgmdCacheEntryGet(unit, L7_AF_INET, &inetIpVal,
                                   interface) == L7_SUCCESS)
        {
          ISv3 = L7_FALSE;

          memset(buf, L7_NULL, sizeof(buf));
          memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
          if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "\r\n%-13s",buf);

          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbMgmdCacheGroupCompatModeGet(unit, L7_AF_INET,
                                               &inetIpVal, interface, &compatMode) == L7_SUCCESS)
          {
            if (compatMode == 1)
            {
              osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_V1);
            }
            else if (compatMode == 2)
            {
              osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_V2);
            }
            else if (compatMode == 3)
            {
              ISv3 = L7_TRUE;
              osapiSnprintf(stat, sizeof(stat), "%-16s", pStrInfo_common_V3);
            }
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-16s", pStrInfo_common_Line);
          }
          ewsTelnetWrite(ewsContext, stat);

          memset(stat, L7_NULL, sizeof(stat));
          if (ISv3 == L7_TRUE)
          {
            if (usmDbMgmdCacheGroupSourceFilterModeGet(unit, L7_AF_INET,
                                                       &inetIpVal, interface, &val) == L7_SUCCESS)
            {
              if (val == MGMD_FILTER_MODE_INCLUDE)
              {                             /* FILTER_MODE_INCLUDE  =1 */
                osapiSnprintf(stat, sizeof(stat), "%-16s ",pStrInfo_common_Incl);
              }
              else if (val == MGMD_FILTER_MODE_EXCLUDE)
              {                              /* FILTER_MODE_EXCLUDE  =2 */
                osapiSnprintf(stat, sizeof(stat), "%-16s ",pStrInfo_common_Excl);
              }
              else
              {
                osapiSnprintf(stat, sizeof(stat), "%-16s ",pStrInfo_common_None_1);
              }
            }
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-17s", pStrInfo_common_Line);
          }
          ewsTelnetWrite(ewsContext, stat);

          if (ISv3 == L7_FALSE)
          {
            ewsTelnetPrintf (ewsContext, "%-16s", pStrInfo_common_Line);

            ewsTelnetPrintf (ewsContext, "%-12s", pStrInfo_common_Line);
          }
          else
          {
            /* Listing the Source Addresses */
            groupAddr = 0;
            hostAddr = 0;
            inetAddressSet(L7_AF_INET, &groupAddr, &inetGroupAddr);
            inetAddressSet(L7_AF_INET, &hostAddr, &inetHostAddr);
            indexIntIface = 0;
            iterator = 0;
            intIfNum = interface;
            inetCopy (&inetIpValNext, &inetIpVal);  
            while(usmDbMgmdSrcListEntryNextGet(unit, L7_AF_INET, &inetIpValNext,
                                               &intIfNum, &inetHostAddr) == L7_SUCCESS )
            {
                memset(buf, L7_NULL, sizeof(buf));
                memset(stat, L7_NULL, sizeof(stat));
                inetAddressGet(L7_AF_INET, &inetHostAddr, &hostAddr);
                rc = usmDbInetNtoa(hostAddr, buf);
                if ((L7_INET_ADDR_COMPARE (&inetIpValNext, &inetIpVal)!= 0 ) || 
                    (intIfNum != interface))
                   continue;
                if (iterator > 0)
                {
                  ewsTelnetPrintf (ewsContext, "\r\n%-46s", "");
                  memset(stat, L7_NULL, sizeof(stat));
                }
                ewsTelnetPrintf (ewsContext, "%-16s",buf);

                if (usmDbMgmdCacheGroupSourceFilterModeGet(unit, L7_AF_INET,
                                                           &inetIpVal, interface, &val) == L7_SUCCESS)
                {
                  if (val == MGMD_FILTER_MODE_INCLUDE)
                  {
                    memset(stat, L7_NULL, sizeof(stat));
                    expTime = 0;
                    if (usmDbMgmdSrcExpiryTimeGet(unit, L7_AF_INET, &inetIpVal,
                                                  interface, &inetHostAddr, &expTime) == L7_SUCCESS)
                    {
                      memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
                      osapiConvertRawUpTime(expTime,(L7_timespec *)&timeSpec);
                      osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                                       timeSpec.minutes,timeSpec.seconds);
                    }
                    else
                    {
                      osapiSnprintf(stat, sizeof(stat),  "%-8s", pStrErr_common_Err);
                    }
                    ewsTelnetWrite(ewsContext, stat);
                  }
                  else if (val == MGMD_FILTER_MODE_EXCLUDE)
                  {
                    memset(stat, L7_NULL, sizeof(stat));
                    expTime = 0;
                    if (usmDbMgmdSrcExpiryTimeGet(unit, L7_AF_INET, &inetIpVal,
                                                  interface, &inetHostAddr, &expTime) == L7_SUCCESS)
                    {

                    memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
                    osapiConvertRawUpTime(expTime,(L7_timespec *)&timeSpec);
                    osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                  timeSpec.minutes,timeSpec.seconds);

                    }
                    else
                    {
                      osapiSnprintf(stat, sizeof(stat),  "%-8s", pStrErr_common_Err);
                    }
                    ewsTelnetWrite(ewsContext, stat);
                  }
                }
                iterator++;
            }
          }
        } /*end if interface exists*/

      } /* end if visible interface */

      /* Get the next interface */
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    } /* end of while loop */
  }
  else if(numArgs == 1)
  {

    /* Display the IGMP interface membership info */
    ewsTelnetWriteAddBlanks (1, 0, 16, 5, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IgmpIntfMbrshipInfo);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_IntfIntfIpStateGrpCompatSrcFilter);
    ewsTelnetWriteAddBlanks (1, 0, 42, 4, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_ModeMode_1);
    ewsTelnetWrite(ewsContext,"\r\n---------  ---------------  ------------  ------------  -------------");

    /* start for loop for all the interfaces */
    while (interface)
    {
      /* display only the visible interfaces */
      if ((usmDbMgmdIsValidIntf(unit, interface) == L7_TRUE) &&
          (usmDbIpIntfExists(unit, interface) == L7_TRUE))
      {
        if (usmDbMgmdCacheEntryGet(unit, L7_AF_INET, &inetIpVal, interface) == L7_SUCCESS)
        {
          ISv3 = L7_FALSE;

          memset(buf, L7_NULL, sizeof(buf));
          memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
          if (usmDbUnitSlotPortGet(interface, &val, &intSlot, &intPort) == L7_SUCCESS)
          {
            osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
          }
          ewsTelnetPrintf (ewsContext, "\r\n%-11s",buf);

          memset(buf, L7_NULL, sizeof(buf));
          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbIpRtrIntfIpAddressGet(unit, interface, &ipAddress, &ipmask) == L7_SUCCESS)
          {
            rc = usmDbInetNtoa(ipAddress, buf);
            osapiSnprintf(stat, sizeof(stat), "%-17s", buf);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-17s", pStrErr_common_Err);
          }
          ewsTelnetWrite(ewsContext, stat);

          memset(buf, L7_NULL, sizeof(buf));
          if (usmDbMgmdInterfaceQuerierStatusGet(unit, L7_AF_INET, interface,
                                                 &val) == L7_SUCCESS)
          {
            switch (val)
            {
            case L7_DISABLE:
              osapiSnprintf(buf, sizeof(buf),  "%-14s", pStrInfo_common_NonQuerier);
              break;
            case L7_ENABLE:
              osapiSnprintf(buf, sizeof(buf),  "%-14s", pStrInfo_common_Querier);
              break;
            default:
              osapiSnprintf(buf, sizeof(buf),  "%-14s", pStrInfo_common_NonQuerier);
              break;
            }
          }
          else
          {
            osapiSnprintf(buf, sizeof(buf),  "%-14s", pStrErr_common_Err);
          }
          ewsTelnetWrite(ewsContext,buf);

          memset(stat, L7_NULL, sizeof(stat));
          if (usmDbMgmdCacheGroupCompatModeGet(unit, L7_AF_INET, &inetIpVal,
                                               interface, &compatMode) == L7_SUCCESS)
          {
            if (compatMode == 1)
            {
              osapiSnprintf(stat, sizeof(stat), "%-14s", pStrInfo_common_V1);
            }
            else if (compatMode == 2)
            {
              osapiSnprintf(stat, sizeof(stat), "%-14s", pStrInfo_common_V2);
            }
            else if (compatMode == 3)
            {
              ISv3 = L7_TRUE;
              osapiSnprintf(stat, sizeof(stat), "%-14s", pStrInfo_common_V3);
            }
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-14s", pStrInfo_common_Line);
          }
          ewsTelnetWrite(ewsContext, stat);

          memset(stat, L7_NULL, sizeof(stat));
          if (ISv3 == L7_TRUE)
          {
            if (usmDbMgmdCacheGroupSourceFilterModeGet(unit, L7_AF_INET,
                                                       &inetIpVal, interface, &val) == L7_SUCCESS)
            {
              if (val == MGMD_FILTER_MODE_INCLUDE)
              {                            /* FILTER_MODE_INCLUDE = 1 */
                osapiSnprintf(stat, sizeof(stat), "%s ",pStrInfo_common_Incl);
              }
              else if (val == MGMD_FILTER_MODE_EXCLUDE)
              {                             /* FILTER_MODE_EXCLUDE = 2 */
                osapiSnprintf(stat, sizeof(stat), "%s ",pStrInfo_common_Excl);
              }
              else
              {
                osapiSnprintf(stat, sizeof(stat), "%s ",pStrInfo_common_None_1);
              }
            }
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat),  "%-8s", pStrInfo_common_Line);
          }
          ewsTelnetWrite(ewsContext, stat);

        } /*end if interface exists*/

      } /* end if visible interface */

      /* Get the next interface */
      if (usmDbValidIntIfNumNext(interface, &nextInterface) == L7_SUCCESS)
      {
        interface = nextInterface;
      }
      else
      {
        interface = 0;
      }
    } /* end of while loop */
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 1, 0, L7_NULLPTR, ewsContext, pStrErr_common_McastInvalidIpAddr);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays igmp Proxy information
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip igmp-proxy
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpIgmpProxy(EwsContext ewsContext, L7_uint32 argc,
                                   const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intIfNum;
  L7_uint32 temp_val = L7_DISABLE;
  L7_uint32 val, intSlot, intPort;
  L7_uint32 unit, numArgs;
  L7_IP_ADDR_t ipAddr;
  L7_inet_addr_t inetIpAddr;
  usmDbTimeSpec_t timeSpec;


  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpIgmpProxy );
  }

  /* Display the IGMP Proxy IfNum info */

  if (usmDbMgmdProxyInterfaceGet(unit, L7_AF_INET, &intIfNum) == L7_SUCCESS)
  {

    if (usmDbUnitSlotPortGet(intIfNum, &val, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
    }
    osapiSnprintf(strSlotPort, sizeof(strSlotPort), "%-10s",buf);
    cliFormat(ewsContext,pStrInfo_common_IntfIdx);
    ewsTelnetWrite(ewsContext, strSlotPort);

    if (usmDbMgmdProxyInterfaceModeGet(unit, L7_AF_INET, intIfNum,
                                       &temp_val) == L7_SUCCESS)
    {
      switch (temp_val)
      {
      case L7_DISABLE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dsbld );
        cliFormat(ewsContext,pStrInfo_ipmcast_AdminMode);
        break;
      case L7_ENABLE:
        osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Enbld );
        cliFormat(ewsContext,pStrInfo_ipmcast_AdminMode);
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

    /* Check if IGMP Proxy is operational. */
    if (usmDbMgmdProxyInterfaceOperationalStateGet(unit, L7_AF_INET,
                                                   intIfNum) != L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dsbld );
      cliFormat(ewsContext,pStrInfo_ipmcast_IgmpProxyOprMode);
      return cliSyntaxReturnPrompt (ewsContext, stat);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Enbld );
      cliFormat(ewsContext,pStrInfo_ipmcast_IgmpProxyOprMode);
      ewsTelnetWrite(ewsContext, stat);
    }

    if (usmDbMgmdHostInterfaceVersionGet(unit, L7_AF_INET, intIfNum,
                                         &temp_val) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_common_IgmpProxyVer);
      ewsTelnetPrintf (ewsContext, "%u", temp_val);
    }

    if (usmDbMgmdProxyInterfaceNumMcastGroupsGet (unit, L7_AF_INET,
                                                  intIfNum, &temp_val) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_ipmcast_MgmdProxyGrps);
      ewsTelnetPrintf (ewsContext, "%u", temp_val);
    }

    if (usmDbMgmdProxyInterfaceUnsolicitedIntervalGet (unit, L7_AF_INET,
                                                       intIfNum, &temp_val) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_ipmcast_MgmdProxyReportIntvl);
      ewsTelnetPrintf (ewsContext, "%u", temp_val);
    }

    if (usmDbMgmdHostInterfaceQuerierGet(unit, L7_AF_INET, intIfNum,
                                         &inetIpAddr) == L7_SUCCESS)
    {
      cliFormat(ewsContext, pStrInfo_ipmcast_MgmdProxyUpstreamQuerierIp);
      inetAddressGet(L7_AF_INET, &inetIpAddr, &ipAddr);
      if (usmDbInetNtoa(ipAddr, buf) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), buf);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), pStrErr_common_Err );
      }

      ewsTelnetWrite(ewsContext,stat);
    }

    if (usmDbMgmdHostInterfaceVer1QuerierTimerGet (unit, L7_AF_INET,
                                                   intIfNum, &temp_val) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_ipmcast_MgmdProxyQuerier1Timeout);
      memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
      osapiConvertRawUpTime(temp_val,(L7_timespec *)&timeSpec);
      ewsTelnetPrintf (ewsContext, "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                       timeSpec.minutes,timeSpec.seconds);
    }

    if (usmDbMgmdHostInterfaceVer2QuerierTimerGet (unit, L7_AF_INET,
                                                   intIfNum, &temp_val) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_ipmcast_MgmdProxyQuerier2Timeout);
      memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
      osapiConvertRawUpTime(temp_val,(L7_timespec *)&timeSpec);
      ewsTelnetPrintf (ewsContext, "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                       timeSpec.minutes,timeSpec.seconds);
    }

    if (usmDbMgmdProxyInterfaceRestartCountGet (unit, L7_AF_INET, intIfNum,
                                                &temp_val) == L7_SUCCESS)
    {
      cliFormat(ewsContext,pStrInfo_ipmcast_MgmdProxyStartFrequency);
      ewsTelnetPrintf (ewsContext, "%u", temp_val);
    }

  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld );
    cliFormat(ewsContext,pStrInfo_ipmcast_AdminMode);
    ewsTelnetWrite(ewsContext, buf);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");

}
/*********************************************************************
*
* @purpose  Displays igmp-proxy information for the specified interface
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip igmp-proxy interface
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const char *commandShowIpIgmpProxyInterface(EwsContext ewsContext,
                                            L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 i;
  L7_uint32 unit, numArgs;
  L7_uint32 intIface;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 temp_val = L7_DISABLE;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val, intSlot, intPort;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpIgmpProxyIntf);
  }

  /* Get the IGMP Proxy interface. */
  if (usmDbMgmdProxyInterfaceGet(unit, L7_AF_INET, &intIface) == L7_SUCCESS)
  {
    /* Check if IGMP Proxy is operational. */
    if (usmDbMgmdProxyInterfaceOperationalStateGet(unit, L7_AF_INET,
                                                   intIface) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_ipmcast_ShowIgmpProxyIntfOprError);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrInfo_ipmcast_ShowIgmpProxyNotEnbld);
  }

  /* Display IGMP-Proxy Interface Index */
  if (usmDbUnitSlotPortGet(intIface, &val, &intSlot, &intPort) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
  }
  osapiSnprintf(strSlotPort, sizeof(strSlotPort), "%-10s",buf);
  cliFormat(ewsContext,pStrInfo_common_IntfIdx);
  ewsTelnetWrite(ewsContext, strSlotPort);

  /* Display the IGMP-Proxy Interface Statistics */
  ewsTelnetWriteAddBlanks (2, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_VerQueryRcvdReportRcvdReportSentLeaveRcvdLeaveSent);
  ewsTelnetWrite(ewsContext,"\r\n-----------------------------------------------------------------\n");

  for(i = 1; i <= L7_MGMD_VERSION_3; i++)
  {
    cliSyntaxBottom(ewsContext);
    ewsTelnetPrintf (ewsContext, "%-10u", i);

    if (i == L7_MGMD_VERSION_1)
    {
      if (usmDbMgmdProxyV1QueriesRecvdGet (unit, L7_AF_INET, intIface,
                                           &temp_val) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%-12u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext, stat);

      if ((usmDbMgmdProxyV1ReportsRecvdGet (unit, L7_AF_INET, intIface,
                                            &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat), " %-12u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext, stat);

      if ((usmDbMgmdProxyV1ReportsSentGet (unit, L7_AF_INET, intIface, &temp_val)
           == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat), " %-9u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  " %-9s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext, stat);

      ewsTelnetPrintf (ewsContext, "%-12s", pStrInfo_common_Line );
      ewsTelnetPrintf (ewsContext, "%-10s", pStrInfo_common_Line );
    }
    else if (i == L7_MGMD_VERSION_2)
    {
      if ((usmDbMgmdProxyV2QueriesRecvdGet (unit, L7_AF_INET,
                                            intIface, &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat), "%-13u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

      if ((usmDbMgmdProxyV2ReportsRecvdGet (unit, L7_AF_INET, intIface,
                                            &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%-13u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

      if ((usmDbMgmdProxyV2ReportsSentGet (unit, L7_AF_INET, intIface,
                                           &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%-12u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

      if ((usmDbMgmdProxyV2LeavesRecvdGet (unit, L7_AF_INET, intIface,
                                           &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%-11u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

      if ((usmDbMgmdProxyV2LeavesSentGet (unit, L7_AF_INET, intIface,
                                          &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

    }
    else
    {
      if ((usmDbMgmdProxyV3QueriesRecvdGet (unit, L7_AF_INET, intIface,
                                            &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%-13u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

      if ((usmDbMgmdProxyV3ReportsRecvdGet (unit, L7_AF_INET, intIface,
                                            &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%-13u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-12s", pStrErr_common_Err );
      }
      ewsTelnetWrite(ewsContext,stat);

      if ((usmDbMgmdProxyV3ReportsSentGet (unit, L7_AF_INET, intIface,
                                           &temp_val) == L7_SUCCESS))
      {
        osapiSnprintf(stat, sizeof(stat),"%-9u", temp_val);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), " %-9s", pStrErr_common_Err);
      }
      ewsTelnetWrite(ewsContext,stat);

      ewsTelnetPrintf (ewsContext, "%-12s", pStrInfo_common_Line );
      ewsTelnetPrintf (ewsContext, "%-10s", pStrInfo_common_Line );
    }
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays the registered multicast groups on the interface
*
* @param  ewsContext  @b{(input)) the context structure
* @param  argc        @b{(input)) the argument count
* @param  *argv       @b{(input)) pointer to argument
* @param  index       @b{(input)) the index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax    show ip igmp-proxy groups
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowIpIgmpProxyGroups(EwsContext ewsContext,
                                         L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argDetail = 1;
  L7_uint32 intIface, rc1;
  L7_RC_t rc;
  L7_uint32 unit, numArgs;
  L7_int32 j;
  L7_char8 checkChar;
  static L7_uint32 indexIntIface = 0;
  L7_uint32 indexIntIface2 = 0;
  static L7_IP_ADDR_t groupAddr = 0;
  static L7_IP_ADDR_t hostAddr = 0;
  static L7_IP_ADDR_t newIpAddr = 0;
  static L7_BOOL srcPagenated = L7_FALSE;
  L7_uint32 ret_val = L7_DISABLE;
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val, intSlot, intPort;
  L7_char8 delay_member_string[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 idle_member_string[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cli_error_member_string[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cli_include_string[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cli_exclude_string[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cli_none_string[L7_CLI_MAX_STRING_LENGTH];
  L7_inet_addr_t inetGroupAddr, inetHostAddr, inetNewIpAddr;
  L7_inet_addr_t inet_ret_val;
  usmDbTimeSpec_t timeSpec;

  j = CLI_MAX_SCROLL_LINES - 6;

  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 0 && numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpIgmpProxyGrps);
  }

  /* Get the IGMP Proxy interface. */
  if (usmDbMgmdProxyInterfaceGet(unit, L7_AF_INET, &intIface) == L7_SUCCESS)
  {
    /* Check if IGMP Proxy is operational. */
    if (usmDbMgmdProxyInterfaceOperationalStateGet(unit, L7_AF_INET, intIface) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrErr_ipmcast_ShowIgmpProxyIntfOprError);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext,  pStrInfo_ipmcast_ShowIgmpProxyNotEnbld);
  }

  if ((numArgs == 0) ||
      ((numArgs == 1) && (strcmp(argv[index+argDetail], pStrInfo_common_IgmpProxyGrpsDetail) == 0)))

  {
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      newIpAddr = L7_NULL;
      inetAddressSet(L7_AF_INET, &newIpAddr, &inetNewIpAddr);
      indexIntIface = intIface;
    }
    else
    {
      inetAddressSet(L7_AF_INET, &newIpAddr, &inetNewIpAddr);
      checkChar = tolower(cliGetCharInput());
      if (( checkChar == 'q' ) ||
          ( checkChar == 'Q' ))
      {
        srcPagenated = L7_FALSE;
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }

    /* Initialize the Display strings */
    osapiSnprintf(delay_member_string, sizeof(delay_member_string), "%-12s", pStrInfo_ipmcast_IgmpProxyDelayMbr); /* DELAY_MEMBER */
    osapiSnprintf(idle_member_string, sizeof(idle_member_string),  "%-12s", pStrInfo_ipmcast_IgmpProxyIdleMbr);  /* IDLE_MEMBER */
    osapiSnprintf(cli_error_member_string, sizeof(cli_error_member_string), "%-12s", pStrErr_common_Err);                /* CLI_ERROR */
    osapiSnprintf(cli_include_string, sizeof(cli_include_string), " %-11s ",pStrInfo_common_Incl);  /* FILTER_MODE_INCLUDE */
    osapiSnprintf(cli_exclude_string, sizeof(cli_exclude_string), " %-11s ",pStrInfo_common_Excl);  /* FILTER_MODE_EXCLUDE */
    osapiSnprintf(cli_none_string, sizeof(cli_none_string),    " %-11s ",pStrInfo_common_None_1);

    if (srcPagenated == L7_FALSE)
    {
    /* Display IGMP-Proxy Interface Index */
    if (usmDbUnitSlotPortGet(intIface, &val, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), pStrErr_common_Err);
    }
    osapiSnprintf(strSlotPort, sizeof(strSlotPort), "%-10s",buf);
    cliFormat(ewsContext,pStrInfo_common_IntfIdx);
    ewsTelnetWrite(ewsContext, strSlotPort);

    /* Display IGMP-Proxy Groups Report */
    ewsTelnetWriteAddBlanks (2, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_GrpAddrLastReporterUpTimeMbrStateFilterModeSrcsIgmp);
    ewsTelnetWrite(ewsContext,    "\r\n---------------  ---------------  --------  ------------ ----------- -------");
    }
    while(j > 0)
    {
      if (srcPagenated == L7_FALSE)
      {
        rc1 = usmDbMgmdHostCacheEntryNextGet(unit, L7_AF_INET, &inetNewIpAddr, &indexIntIface);
        if ( rc1 != L7_SUCCESS )
        {
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
        inetAddressGet(L7_AF_INET, &inetNewIpAddr, &newIpAddr);
        rc = usmDbInetNtoa(newIpAddr, buf);
        ewsTelnetPrintf (ewsContext, "\r\n%-16s",buf);

        if (usmDbMgmdHostCacheLastReporterGet(unit, L7_AF_INET,
                                              &inetNewIpAddr, intIface, &inet_ret_val) == L7_SUCCESS)
        {
          inetAddressGet(L7_AF_INET, &inet_ret_val, &ret_val);
          rc = usmDbInetNtoa(ret_val, buf);
          osapiSnprintf(stat, sizeof(stat), " %-17s",buf);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat),  " %-17s", pStrErr_common_Err );
        }
        ewsTelnetWrite(ewsContext, stat);

        if (usmDbMgmdHostCacheUpTimeGet(unit, L7_AF_INET, &inetNewIpAddr,
                                        intIface, &ret_val) == L7_SUCCESS)
        {
          memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
          osapiConvertRawUpTime(ret_val,(L7_timespec *)&timeSpec);
          osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d  ", timeSpec.hours,
                                       timeSpec.minutes,timeSpec.seconds);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat),  "%-10s ", pStrErr_common_Err );
        }

        ewsTelnetWrite(ewsContext, stat);

        if (usmDbMgmdHostGroupStatusGet(unit, L7_AF_INET, &inetNewIpAddr,
                                        intIface, &ret_val) == L7_SUCCESS)
        {
          switch (ret_val)
          {
          case MGMD_HOST_DELAY_MEMBER:    /* 2 */
            ewsTelnetWrite(ewsContext, delay_member_string);
            break;
          case MGMD_HOST_IDLE_MEMBER:     /* 3 */
            ewsTelnetWrite(ewsContext, idle_member_string);
            break;
          default:
            ewsTelnetWrite(ewsContext, cli_error_member_string);
            break;
          }/*End-of-Switch*/
        }
        else
        {
          ewsTelnetWrite(ewsContext, cli_error_member_string);
        }

        if (usmDbMgmdHostCacheGroupSourceFilterModeGet(unit, L7_AF_INET,
                                                       &inetNewIpAddr, intIface, &ret_val) == L7_SUCCESS)
        {
          switch (ret_val)
          {
          case MGMD_FILTER_MODE_INCLUDE:   /* FILTER_MODE_INCLUDE = 1 */
            ewsTelnetWrite(ewsContext, cli_include_string);
            break;
          case MGMD_FILTER_MODE_EXCLUDE:   /* FILTER_MODE_EXCLUDE = 2 */
            ewsTelnetWrite(ewsContext, cli_exclude_string);
            break;
          default:
            ewsTelnetWrite(ewsContext, cli_none_string);
            break;
          }/*End-of-Switch*/
        }
        else
        {
          ewsTelnetWrite(ewsContext, cli_none_string);
        }

        if (usmDbMgmdHostInterfaceSrcsGet(unit, L7_AF_INET, &inetNewIpAddr,
                                          intIface, &ret_val) == L7_SUCCESS)
        {
          osapiSnprintf(stat, sizeof(stat), "%-4u", ret_val);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat),  "%-6s", pStrErr_common_Err);
          ret_val = 0;
        }

        ewsTelnetWrite(ewsContext, stat);
        /*cliSyntaxBottom(ewsContext);*/

        if( (ret_val != 0)&& (numArgs == 1))
        {
          if(j < 3)       /* to print the src list at least there should be 3 lines gap */
          {
            break;
          }
        }
        j--;
      }
        /* If ((display Detail true) and (src count/ret_val) == 0) then DISPLAY_SRC_LIST. */
        if ((srcPagenated == L7_TRUE) || (((numArgs == 1) && (strcmp(argv[index+argDetail], pStrInfo_common_IgmpProxyGrpsDetail) == 0))
            && (ret_val != 0)) )
        {
          groupAddr = newIpAddr;
          if (srcPagenated == L7_FALSE)
          {
            hostAddr = 0;
          }
          indexIntIface2 = intIface;

          j--;      /* you need to count the heading also. other wise buffer problem */
          j--;

          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_GrpSrcListExpiryTime);
          ewsTelnetWrite(ewsContext,"\r\n-----------------  -----------");
          if (j <= 0)
          {
            srcPagenated = L7_TRUE;
            cliSetCharInputID(1, ewsContext, argv);
            cliSyntaxBottom(ewsContext);
            osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_ipmcast_ShowIgmpProxyGrpsDetail,
                                    argv[index+argDetail]);
            cliAlternateCommandSet(buf);
            return pStrInfo_common_Name_2; 
          }
          while(j > 0)
          {
            /*  Listing the Source Addresses */

            inetAddressSet(L7_AF_INET, &groupAddr, &inetGroupAddr);
            inetAddressSet(L7_AF_INET, &hostAddr, &inetHostAddr);
            rc = usmDbMgmdHostSrcListEntryGetNext(unit, L7_AF_INET,
                                                  &inetGroupAddr, &indexIntIface2,&inetHostAddr);
            inetAddressGet(L7_AF_INET, &inetGroupAddr, &groupAddr);
            inetAddressGet(L7_AF_INET, &inetHostAddr, &hostAddr);
            if ((rc == L7_SUCCESS) && (indexIntIface2 == intIface) && (groupAddr == newIpAddr))
            {
              cliSyntaxBottom(ewsContext);

              rc = usmDbInetNtoa(hostAddr, buf);
              ewsTelnetPrintf (ewsContext, "%-17s  ",buf);
              ret_val = 0;  
              if (usmDbMgmdHostSrcListExpiryTimeGet(unit, L7_AF_INET,
                                                    intIface, &inetGroupAddr, &inetHostAddr,
                                                    &ret_val) == L7_SUCCESS)
              {
                memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
                osapiConvertRawUpTime(ret_val,(L7_timespec *)&timeSpec);
                osapiSnprintf(stat, sizeof(stat), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                                 timeSpec.minutes,timeSpec.seconds);
              }
              else
              {
                osapiSnprintf(stat, sizeof(stat), "    %-6s", pStrErr_common_Err );
              }
              ewsTelnetWrite(ewsContext, stat);

              j--;
              if (j <= 0)
              {
              srcPagenated = L7_TRUE;
              cliSetCharInputID(1, ewsContext, argv);
              cliSyntaxBottom(ewsContext);
              osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_ipmcast_ShowIgmpProxyGrpsDetail,
                                      argv[index+argDetail]);
              cliAlternateCommandSet(buf);
              return pStrInfo_common_Name_2; 
              }
            }
            else
            {
              srcPagenated = L7_FALSE;
              cliSyntaxBottom(ewsContext);
              break;
            }
          }/*End-of-While*/
        }/*End-If-Block-For-displaying-Source-List*/
      
    }
    srcPagenated = L7_FALSE;
    cliSetCharInputID(1, ewsContext, argv);
    cliSyntaxBottom(ewsContext);
    osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),  pStrInfo_ipmcast_ShowIgmpProxyGrpsDetail,
                            argv[index+argDetail]);
    cliAlternateCommandSet(buf);
    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*end of cli_show_igmp_l3.c*/

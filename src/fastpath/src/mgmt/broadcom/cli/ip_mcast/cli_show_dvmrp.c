/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/ip_mcast/cli_show_dvmrp.c
 *
 * @purpose show commands for the dvmrp cli functionality
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
#include <cliapi.h>
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_ip_mcast_common.h"
#include "strlib_ip_mcast_cli.h"
#include "l3_mcast_commdefs.h"
#include "l3_mcast_defaultconfig.h"
#include <datatypes.h>
#include <usmdb_util_api.h>
/*#include <clicommands_l3.h>*/
#include <l3_commdefs.h>
#include <usmdb_mib_dvmrp_api.h>
#include "usmdb_l3.h"
#include "cli_web_exports.h"
#include "usmdb_ip_api.h"
#include "usmdb_util_api.h"

#ifdef L7_IP_MCAST_PACKAGE
#include "usmdb_mib_dvmrp_api.h"
#endif


#include <clicommands_mcast.h>
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose  Displays dvmrp information in the router
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
* @cmdsyntax    show ip dvmrp
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandShowIpDvmrp(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 versionStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 nextInterface;
  L7_uint32 intSlot, intPort;
  L7_uint32 numberOfRoutes, reachableRoutes, mode, interface, val;
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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpDvmrp_1);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbDvmrpAdminModeGet(unit, &mode) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_ipmcast_AdminMode);
    ewsTelnetPrintf (ewsContext, strUtilEnabledDisabledGet(mode,pStrInfo_common_Dsbl_1));
  }

  memset(versionStr, L7_NULL, sizeof(versionStr));
  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbDvmrpVersionStringGet(unit, versionStr) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_IgmpProxyVer);
    ewsTelnetPrintf (ewsContext, versionStr);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbDvmrpNumRoutesGet(unit, &numberOfRoutes) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_TotalNumOfRoutes);
    ewsTelnetPrintf (ewsContext, "%d", numberOfRoutes);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbDvmrpReachableRoutesGet(unit, &reachableRoutes) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_ReachableRoutes);
    ewsTelnetPrintf (ewsContext, "%d", reachableRoutes);
  }

  /* Heading for the router DVMRP interface summary */
  if (usmDbValidIntIfNumFirstGet(&interface) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_NoValidPortsInBox);
  }
  /* Display the DVMRP interface info */
  ewsTelnetWriteAddBlanks (2, 0, 4, 10, L7_NULLPTR, ewsContext,pStrInfo_ipmcast_DvmrpIntfStatus);
  ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_common_IntfIntfModeProtoState);
  ewsTelnetWrite(ewsContext,"\r\n---------  --------------  ----------------\r");

  /* start for loop for all the interfaces */
  while (interface)
  {
    if ((usmDbDvmrpIsValidIntf(unit, interface) == L7_TRUE) &&
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
      if (usmDbDvmrpIntfAdminModeGet(unit, interface, &val) == L7_SUCCESS)
      {
        osapiSnprintf(stat,sizeof(stat),"%-16s",strUtilEnabledDisabledGet(val, pStrInfo_common_Blank));
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat),  "%-16s", "-----");
      }
      ewsTelnetWrite(ewsContext, stat);

      memset(buf, L7_NULL, sizeof(buf));
      if(usmDbDvmrpInterfaceOperationalStateGet(unit, interface) == L7_TRUE)
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_Operational);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_NonOperational);
      }
      ewsTelnetWrite(ewsContext, buf);

      /*end if interface exists*/
    }                                                                                                                                                            /* end if visible interface */
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
* @purpose  Displays dvmrp interface specific information in the router
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
* @cmdsyntax    show ip dvmrp interface <slot/port>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpDvmrpInterface(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 mode, localAddr, intIface, s, p;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipStr[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 metric;
  L7_ulong32 rcvBadPkts, rcvBadRoutes, sentRoutes, genId;
  L7_RC_t rc;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetLocalAddr;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();
  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_ipmcast_ShowIpDvmrpIntf_1, cliSyntaxInterfaceHelp());
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
  if (usmDbDvmrpIntfAdminModeGet(unit, intIface, &mode) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_IntfMode);
    ewsTelnetPrintf (ewsContext, strUtilEnabledDisabledGet(mode,pStrInfo_common_Dsbl_1));
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbDvmrpIntfMetricGet( unit, intIface, &metric ) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_IntfMetric);
    ewsTelnetPrintf (ewsContext, "%d", metric);
  }

  memset(buf, L7_NULL, sizeof(buf));
  memset(buf, L7_NULL, sizeof(ipStr));
  if (usmDbDvmrpIntfLocalAddressGet(unit, intIface, &inetLocalAddr) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_LocalAddr);
    inetAddressGet(L7_AF_INET, &inetLocalAddr, &localAddr);
    rc = usmDbInetNtoa(localAddr, ipStr);
    ewsTelnetPrintf (ewsContext, ipStr);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if (usmDbDvmrpIntfGenerationIdGet(unit, intIface, &genId) == L7_SUCCESS)
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_GenerationId);
    ewsTelnetPrintf (ewsContext, "%ld", genId);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((mode == L7_ENABLE) && (usmDbDvmrpIntfRcvBadPktsGet(unit, intIface, &rcvBadPkts) == L7_SUCCESS))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_RcvdBadPkts);
    ewsTelnetPrintf (ewsContext, "%ld", rcvBadPkts);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((mode == L7_ENABLE) && (usmDbDvmrpIntfRcvBadRoutesGet(unit, intIface, &rcvBadRoutes) == L7_SUCCESS))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_RcvdBadRoutes);
    ewsTelnetPrintf (ewsContext, "%ld", rcvBadRoutes);
  }

  memset(buf, L7_NULL, sizeof(buf));
  if ((mode == L7_ENABLE) && (usmDbDvmrpIntfSentRoutesGet(unit, intIface, &sentRoutes) == L7_SUCCESS))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_SentRoutes);
    ewsTelnetPrintf (ewsContext, "%ld", sentRoutes);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Displays neighbor information of the DVMRP router
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
* @cmdsyntax    show ip dvmrp neighbor
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpDvmrpNeighbor(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stateStr[L7_CLI_MAX_STRING_LENGTH];
  static L7_BOOL flag = L7_FALSE;
  L7_uint32 intSlot, intPort, val, count;
  static L7_uint32 intIfNum, nbrIpAddr, nextCount;
  L7_ulong32 nbrUpTime, nbrExpTime, rcvRts, rcvBadPkts, state, val1;
  L7_ushort16 version,capability;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetNbrIpAddr;
  L7_timespec timeSpec;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpDvmrpNeighbor_1);
  }

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    intIfNum = 0;
    nbrIpAddr = 0;
    inetAddressSet(L7_AF_INET, &nbrIpAddr, &inetNbrIpAddr);
    flag = L7_FALSE;
    nextCount = 1;

    if (usmDbDvmrpNeighborEntryNextGet(unit, &intIfNum, &inetNbrIpAddr) != L7_SUCCESS)
    {
      if(flag == L7_FALSE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_ipmcast_NoNeighborsAvailable);
      }
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    if (tolower(cliGetCharInput()) == 'q')
    {
      intIfNum = 1; nbrIpAddr = 0;
      inetAddressSet(L7_AF_INET, &nbrIpAddr, &inetNbrIpAddr);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    else
    {
      inetAddressSet(L7_AF_INET, &nbrIpAddr, &inetNbrIpAddr);
    }
  }

  flag = L7_TRUE;
  memset(buf, L7_NULL, sizeof(buf));
  memset(ipStr, L7_NULL, sizeof(ipStr));
  memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
  val = 0;
  val1 = 0;
  count = 1;

  while(count <= (CLI_MAX_SCROLL_LINES-6))
  {

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 1) )
    {
      if (usmDbUnitSlotPortGet(intIfNum, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_Intf);
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
        ewsTelnetPrintf (ewsContext, buf);
        count++;
      }
      nextCount = 2;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 2) )
    {
      cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_NeighborIpAddr);
      memset(ipStr, L7_NULL, sizeof(ipStr));
      inetAddressGet(L7_AF_INET, &inetNbrIpAddr, &nbrIpAddr);
      usmDbInetNtoa(nbrIpAddr, ipStr);
      ewsTelnetWrite(ewsContext, ipStr);

      if (usmDbDvmrpNeighborStateGet( unit, intIfNum, &inetNbrIpAddr, &state ) == L7_SUCCESS)
      {
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_State);
        switch(state)
        {
        case L7_DVMRP_NEIGHBOR_STATE_ONEWAY:
          osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stateStr, sizeof(stateStr), pStrInfo_ipmcast_Oneway);
          break;
        case L7_DVMRP_NEIGHBOR_STATE_ACTIVE:
          osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stateStr, sizeof(stateStr), pStrInfo_common_WsActive);
          break;
        case L7_DVMRP_NEIGHBOR_STATE_IGNORING:
          osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stateStr, sizeof(stateStr), pStrInfo_ipmcast_Ignoring);
          break;
        case L7_DVMRP_NEIGHBOR_STATE_DOWN:
          osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stateStr, sizeof(stateStr), pStrInfo_common_Down_1);
          break;
        default:
          break;
        }
        ewsTelnetWrite(ewsContext, stateStr);

        count++;
      }
      nextCount = 4;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 4) )
    {
      if (usmDbDvmrpNeighborUpTimeGet( unit, intIfNum, &inetNbrIpAddr, &nbrUpTime ) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_UpTimeHhMmSs);
        osapiConvertRawUpTime(nbrUpTime,(L7_timespec *)&timeSpec);
        ewsTelnetPrintf (ewsContext, "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                                 timeSpec.minutes,timeSpec.seconds);
        count++;

      }
      nextCount = 5;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 5) )
    {
      if (usmDbDvmrpNeighborExpiryTimeGet( unit, intIfNum, &inetNbrIpAddr, &nbrExpTime ) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_common_ExpiryTimeHhMmSs);
        if(state == L7_DVMRP_NEIGHBOR_STATE_DOWN)
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NotApplicable);
        }
        else
        {
          osapiConvertRawUpTime(nbrExpTime,(L7_timespec *)&timeSpec);
          osapiSnprintf(buf, sizeof(buf), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                                 timeSpec.minutes,timeSpec.seconds);
        }
        ewsTelnetWrite(ewsContext, buf);

        count++;
      }
      nextCount = 6;
    }
    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 6) )
    {
      if (usmDbDvmrpNeighborGenIdGet(unit, intIfNum, &inetNbrIpAddr, &val1) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_GenerationId);
        ewsTelnetPrintf (ewsContext, "%lu", val1);

        count++;
      }
      nextCount = 7;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 7) )
    {
      if (usmDbDvmrpNeighborMajorVersionGet(unit, intIfNum, &inetNbrIpAddr, &version) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_MajorVer);
        ewsTelnetPrintf (ewsContext, "%d", version);

        count++;
      }
      nextCount = 8;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 8) )
    {
      if (usmDbDvmrpNeighborMinorVersionGet(unit, intIfNum, &inetNbrIpAddr, &version) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_MinorVer);
        ewsTelnetPrintf (ewsContext, "%d", version);

        count++;
      }
      nextCount = 9;
    }

    /* Neighbor Capabilities */
    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 9) )
    {
      if (usmDbDvmrpNeighborCapabilitiesGet(unit, intIfNum, &inetNbrIpAddr, &capability) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_Capabilities);
        if(capability != L7_NULL)
        {
          if (capability & (0x01 << L7_DVMRP_CAPABILITIES_LEAF))
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_ipmcast_Leaf);
          }
          if (capability & (0x01 << L7_DVMRP_CAPABILITIES_PRUNE))
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_ipmcast_Prune);
          }
          if (capability & (0x01 << L7_DVMRP_CAPABILITIES_GENID))
          {
            OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_ipmcast_Genid);
          }
          if (capability & (0x01 << L7_DVMRP_CAPABILITIES_MTRACE))
          {
            OSAPI_STRNCAT(buf, pStrInfo_ipmcast_Missing11441);
          }
          ewsTelnetWrite(ewsContext, buf);
        }
        else
        {
          ewsTelnetWrite(ewsContext, pStrInfo_common_None_1);
        }

        count++;
      }
      nextCount = 10;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 10) )
    {
      if (usmDbDvmrpNeighborRcvRoutesGet(unit, intIfNum, &inetNbrIpAddr, &val1) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_RcvdRoutes);
        ewsTelnetPrintf (ewsContext, "%lu", val1);

        count++;
      }
      nextCount = 11;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 11) )
    {
      if (usmDbDvmrpNeighborRcvBadPktsGet( unit, intIfNum, &inetNbrIpAddr, &rcvBadPkts ) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_RcvdBadPkts);
        ewsTelnetPrintf (ewsContext, "%lu", rcvBadPkts);

        count++;
      }
      nextCount = 12;
    }

    if( (count <= (CLI_MAX_SCROLL_LINES-6)) && (nextCount == 12) )
    {
      if (usmDbDvmrpNeighborRcvBadRoutesGet( unit, intIfNum, &inetNbrIpAddr, &rcvRts ) == L7_SUCCESS)
      {
        memset(buf, L7_NULL, sizeof(buf));
        cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_RcvdBadRoutes);
        ewsTelnetPrintf (ewsContext, "%lu", rcvRts);

        count++;
      }
      nextCount = 1;
    }

    if((count > (CLI_MAX_SCROLL_LINES-6)) && (nextCount != 1))
    {
      break;
    }
    else
    {
      if (usmDbDvmrpNeighborEntryNextGet(unit, &intIfNum, &inetNbrIpAddr) != L7_SUCCESS)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");

      }
    }

  }

  cliSetCharInputID(1, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpDvmrpNeighbor_2);
  return pStrInfo_ipmcast_MoreEntriesOrQuitQ;
}
/*********************************************************************
*
* @purpose  Displays next hop information on outgoing interfaces
*           for routing multicast datagrams
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
* @cmdsyntax    show ip dvmrp nexthop
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpDvmrpNexthop(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 nextHopSrc, nextHopSrcMask, nextHopIfIndex;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH], strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipMaskStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 count;
  L7_uint32 intSlot, intPort, val;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetNextHopSrc, inetNextHopSrcMask;
  L7_BOOL printHeaderFlag = L7_TRUE;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpDvmrpNextHop_1);
  }

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    nextHopSrc = 0;
    nextHopSrcMask = 0;
    nextHopIfIndex = 0;
    inetAddressSet(L7_AF_INET, &nextHopSrc, &inetNextHopSrc);
    inetAddressSet(L7_AF_INET, &nextHopSrcMask, &inetNextHopSrcMask);

  }
  else
  {
    if (tolower(cliGetCharInput()) == 'q')
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  for (count = 0; count < 15; count++)
  {
    if (usmDbDvmrpRouteNextHopEntryNextGet( unit, &inetNextHopSrc, &inetNextHopSrcMask, &nextHopIfIndex) != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if(printHeaderFlag == L7_TRUE)
    {
      /*header for DVMRP NextHop*/
      ewsTelnetWriteAddBlanks (1, 0, 32, 9, L7_NULLPTR, ewsContext, pStrInfo_common_NextHop);
      ewsTelnetWriteAddBlanks (1, 0, 2, 3, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_SrcIpSrcMaskIntfType);
      ewsTelnetWrite(ewsContext, "\r\n--------------- --------------- --------- -------");
      printHeaderFlag = L7_FALSE;
    }

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    inetAddressGet(L7_AF_INET, &inetNextHopSrc, &nextHopSrc);
    usmDbInetNtoa(nextHopSrc, ipStr);
    ewsTelnetPrintf (ewsContext, "\r\n%-16s", ipStr);

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipMaskStr, L7_NULL, sizeof(ipMaskStr));
    inetAddressGet(L7_AF_INET, &inetNextHopSrcMask, &nextHopSrcMask);
    usmDbInetNtoa(nextHopSrcMask, ipMaskStr);
    ewsTelnetPrintf (ewsContext, "%-16s", ipMaskStr);

    memset(buf, L7_NULL, sizeof(buf));
    memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
    val = 0;
    if (usmDbUnitSlotPortGet(nextHopIfIndex, &val, &intSlot, &intPort) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NotApplicable);
    }
    ewsTelnetPrintf (ewsContext, "%-10s",buf);

    memset(buf,L7_NULL, sizeof(buf));
    if (usmDbDvmrpNextHopTypeGet( unit, &inetNextHopSrc, &inetNextHopSrcMask, nextHopIfIndex, &val) == L7_SUCCESS)
    {
      if(val == L7_DVMRP_NEXTHOP_TYPE_LEAF)
      {
        osapiSnprintf (buf, sizeof(buf),  "%-7s", pStrInfo_ipmcast_Leaf);
      }
      else if(val == L7_DVMRP_NEXTHOP_TYPE_BRANCH)
      {
        osapiSnprintf (buf, sizeof(buf),  "%-7s", pStrInfo_ipmcast_Branch);
      }
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-7s", pStrInfo_common_NotApplicable);
    }
    ewsTelnetWrite(ewsContext, buf);
  }

  cliSetCharInputID(1, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpDvmrpNexthop);
  return pStrInfo_ipmcast_MoreEntriesOrQuitQ;
}

/*********************************************************************
*
* @purpose  Displays the multicast routing information of Dvmrp
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
* @cmdsyntax    show ip dvmrp route
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpDvmrpRoute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 srcIpAddr, srcMask;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipStr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 count, ipAddr, ifIndex;
  L7_uint32 intSlot, intPort, val;
  L7_ushort16 metric;
  L7_ulong32 rtTime;
  L7_uint32 unit, numArgs;
  L7_inet_addr_t inetSrcIpAddr, inetSrcMask, inetIpAddr;
  L7_timespec timeSpec;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpDvmrpRoute_1);
  }

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    srcIpAddr = 0;
    srcMask = 0;
    inetAddressSet(L7_AF_INET, &srcIpAddr, &inetSrcIpAddr);
    inetAddressSet(L7_AF_INET, &srcMask, &inetSrcMask);
  }
  else
  {
    if (tolower(cliGetCharInput()) == 'q')
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
    inetAddressSet(L7_AF_INET, &srcIpAddr, &inetSrcIpAddr);
    inetAddressSet(L7_AF_INET, &srcMask, &inetSrcMask);

  }

  /*header for DVMRP Route Table*/
  ewsTelnetWriteAddBlanks (1, 0, 32, 1, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_UpstreamExpiryUpTime);
  ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_SrcAddrSrcMaskNeighborIntfMetricTimeSecs);
  ewsTelnetWrite(ewsContext, "\r\n--------------- --------------- ------------ -------- ------ ---------- ----------");

  for (count = 0; count < 15; count++)
  {
    if (usmDbDvmrpRouteEntryNextGet(unit, &inetSrcIpAddr, &inetSrcMask) != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    inetAddressGet(L7_AF_INET, &inetSrcIpAddr, &srcIpAddr);
    usmDbInetNtoa(srcIpAddr, ipStr);
    ewsTelnetPrintf (ewsContext, "\r\n%-16s", ipStr);

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    inetAddressGet(L7_AF_INET, &inetSrcMask, &srcMask);
    usmDbInetNtoa(srcMask, ipStr);
    ewsTelnetPrintf (ewsContext, "%-16s", ipStr);

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    if (usmDbDvmrpRouteUpstreamNeighborGet( unit, &inetSrcIpAddr, &inetSrcMask, &inetIpAddr ) == L7_SUCCESS)
    {
      inetAddressGet(L7_AF_INET, &inetIpAddr, &ipAddr);
      usmDbInetNtoa(ipAddr, ipStr);
      osapiSnprintf(buf, sizeof(buf),  "%-16s", ipStr);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-16s", pStrInfo_common_NotApplicable);
    }
    ewsTelnetWrite(ewsContext, buf);

    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbDvmrpRouteIfIndexGet(unit, &inetSrcIpAddr,
                                  &inetSrcMask, &ifIndex) == L7_SUCCESS)
    {
      memset(strSlotPort, L7_NULL, sizeof(strSlotPort));
      val = 0;
      if (usmDbUnitSlotPortGet(ifIndex, &val, &intSlot, &intPort) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), cliDisplayInterfaceHelp(val, intSlot, intPort));
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NotApplicable);
      }
      ewsTelnetPrintf (ewsContext, "%-7s",buf);
    }
    else
    {
      ewsTelnetPrintf (ewsContext, "%-10s", pStrInfo_common_NotApplicable);
    }

    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbDvmrpRouteMetricGet( unit, &inetSrcIpAddr, &inetSrcMask, &metric ) == L7_SUCCESS)
    {
      osapiSnprintf(buf, sizeof(buf),  "%-6d",metric);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-6s", pStrInfo_common_NotApplicable);
    }
    ewsTelnetWrite(ewsContext, buf);

    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbDvmrpRouteExpiryTimeGet( unit, &inetSrcIpAddr, &inetSrcMask, &rtTime ) == L7_SUCCESS)
    {
      osapiConvertRawUpTime(rtTime,(L7_timespec *)&timeSpec);
      osapiSnprintf(buf, sizeof(buf), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12s", pStrInfo_common_NotApplicable);
    }
    ewsTelnetWrite(ewsContext, buf);

    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbDvmrpRouteUpTimeGet( unit, &inetSrcIpAddr, &inetSrcMask, &rtTime ) == L7_SUCCESS)
    {
      osapiConvertRawUpTime(rtTime,(L7_timespec *)&timeSpec);
      osapiSnprintf(buf, sizeof(buf), "    %2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-8s", pStrInfo_common_NotApplicable);
    }
    ewsTelnetWrite(ewsContext, buf);
  }

  cliSetCharInputID(1, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpDvmrpRoute_2);
  return pStrInfo_ipmcast_MoreEntriesOrQuitQ;
}

/*********************************************************************
*
* @purpose  Displays the table listing routers upstream prune info.

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
* @cmdsyntax    show ip dvmrp prune
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowIpDvmrpPrune(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 ipStr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 count;
  L7_uint32 unit, numArgs;
  L7_ulong32 pruneExpTime;
  static L7_uint32 pruneGrp, pruneSrc, pruneSrcMask;
  L7_inet_addr_t inetPruneGrp, inetPruneSrc, inetPruneSrcMask;
  L7_timespec timeSpec;

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
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_ipmcast_ShowIpDvmrpPrune_1);
  }

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    pruneGrp = 0;
    pruneSrc = 0;
    pruneSrcMask = 0;
    inetAddressSet(L7_AF_INET, &pruneGrp, &inetPruneGrp);
    inetAddressSet(L7_AF_INET, &pruneSrc, &inetPruneSrc);
    inetAddressSet(L7_AF_INET, &pruneSrcMask, &inetPruneSrcMask);
  }
  else
  {
    if (tolower(cliGetCharInput()) == 'q')
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  /*header for DVMRP Prune Table*/
  ewsTelnetWriteAddBlanks (1, 0, 4, 4, L7_NULLPTR, ewsContext, pStrInfo_ipmcast_GrpIpSrcIpSrcMaskTimeSecs);
  ewsTelnetWrite(ewsContext, "\r\n--------------- --------------- --------------- --------------");
  for (count = 0; count < 15; count++)
  {

    if (usmDbDvmrpPruneEntryNextGet(unit, &inetPruneGrp, &inetPruneSrc, &inetPruneSrcMask) != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    inetAddressGet(L7_AF_INET, &inetPruneGrp, &pruneGrp);
    usmDbInetNtoa(pruneGrp, ipStr);
    ewsTelnetPrintf (ewsContext, "\r\n%-16s", ipStr);

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    inetAddressGet(L7_AF_INET, &inetPruneSrc, &pruneSrc);
    usmDbInetNtoa(pruneSrc, ipStr);
    ewsTelnetPrintf (ewsContext, "%-16s", ipStr);

    memset(buf, L7_NULL, sizeof(buf));
    memset(ipStr, L7_NULL, sizeof(ipStr));
    inetAddressGet(L7_AF_INET, &inetPruneSrcMask, &pruneSrcMask);
    usmDbInetNtoa(pruneSrcMask, ipStr);
    ewsTelnetPrintf (ewsContext, "%-16s", ipStr);

    memset(buf, L7_NULL, sizeof(buf));
    if (usmDbDvmrpPruneExpiryTimeGet(unit, &inetPruneGrp, &inetPruneSrc, &inetPruneSrcMask, &pruneExpTime) == L7_SUCCESS)
    {
      osapiConvertRawUpTime(pruneExpTime,(L7_timespec *)&timeSpec);
      osapiSnprintf(buf, sizeof(buf), "%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                               timeSpec.minutes,timeSpec.seconds);   
    }
    else
    {
      osapiSnprintf(buf, sizeof(buf),  "%-12s", pStrErr_common_Err);
    }
    ewsTelnetWrite(ewsContext, buf);
  }
  cliSetCharInputID(1, ewsContext, argv);
  cliSyntaxBottom(ewsContext);
  cliAlternateCommandSet(pStrInfo_ipmcast_ShowIpDvmrpPrune_2);
  return pStrInfo_ipmcast_MoreEntriesOrQuitQ;
}

/*********************************************************************
*
* @purpose  Displays trap flag info for DVMRP
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
* @cmdsyntax     show trapflags
*
* @cmdhelp
*
* @cmddescript This commands takes no options
*
* @end
*
*********************************************************************/
void commandShowDvmrpTrapflags(EwsContext ewsContext)
{
  L7_uint32 val;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return;
  }

  if (usmDbDvmrpTrapModeGet(unit, L7_DVMRP_TRAP_ALL, &val) == L7_SUCCESS)
  {
    cliFormat(ewsContext, pStrInfo_common_DvmrpTraps);
    ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val,pStrInfo_common_Dsbl_1));
  }
  return;
}

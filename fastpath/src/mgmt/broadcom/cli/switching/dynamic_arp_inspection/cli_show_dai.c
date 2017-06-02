
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/cli_show_dai.c
*
* @purpose Dynamic ARP Inspection show commands for the cli
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create  09/27/2007
*
* @author  Kiran Kumar Kella
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
#include "clicommands_card.h"
#include "clicommands_dai.h"
#include "usmdb_dai_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "osapi.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose Displays the Dynamic ARP Inspection information including
*          global config and vlan config for DAI enabled vlans.
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
* @notes     Here we  display the global and vlan specific DAI info.
*
* @cmdsyntax    show ip arp inspection
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDai(EwsContext ewsContext, L7_uint32 argc,
                           const L7_char8 **argv, L7_uint32 index)
{
  return commandShowDaiVlan(ewsContext, argc, argv, index);
}

/*********************************************************************
*
* @purpose Displays the Dynamic ARP Inspection information of vlan(s).
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
* @notes     Here we  display the vlan specific DAI info.
*
* @cmdsyntax    show ip arp inspection [vlan <vlan-list>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDaiVlan(EwsContext ewsContext, L7_uint32 argc,
                               const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL verifySMac = L7_FALSE, verifyDMac = L7_FALSE, verifyIP = L7_FALSE;
  L7_BOOL configuredEntry = L7_FALSE;
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 arpAclName[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 vlanListIndex;
  L7_uint32 numArg=0, val, count = 0, temp = 0, i;
  L7_uint32 vlanList[L7_MAX_VLANS], vlanCount = 0;
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_ShowDai);
  }

  temp = CLI_MAX_SCROLL_LINES-6;
  cliCmdScrollSet( L7_FALSE);

  if(numArg == 1)
  {
    /* show ip arp inspection vlan <vlan-list> */
    /* Get list of VLAN IDs */
    strVlanList = (L7_char8 *) argv[index + 1];

    if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                              L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, tempBuf);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }
  else
  {
    /* show ip arp inspection */
    for(i = 0; i < L7_MAX_VLANS; i++)
    {
      vlanList[i] = i+1;
    }
    vlanCount = L7_MAX_VLANS;
  }

  /* Check if any vlan in the list is invalid */
  for(i = 0; i < vlanCount; i++)
  {
    if((vlanList[i] > L7_MAX_VLANS) || (vlanList[i] <= 0))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                              L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, tempBuf);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    temp = CLI_MAX_SCROLL_LINES-9;
    /* source mac validation */
    cliFormat(ewsContext, pStrInfo_base_DaiVerifySMac);
    if (usmDbDaiVerifySMacGet(&verifySMac) == L7_SUCCESS)
    {
      if (verifySMac)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_Enabled); 
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_Disabled);
      }
    }

    /* destination mac validation */
    cliFormat(ewsContext, pStrInfo_base_DaiVerifyDMac);
    if (usmDbDaiVerifyDMacGet(&verifyDMac) == L7_SUCCESS)
    {
      if (verifyDMac)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_Enabled);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_Disabled);
      }
    }

    /* ip address validation */
    cliFormat(ewsContext, pStrInfo_base_DaiVerifyIp);
    if (usmDbDaiVerifyIPGet(&verifyIP) == L7_SUCCESS)
    {
      if (verifyIP)
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_Enabled);
      }
      else
      {
        ewsTelnetWrite(ewsContext, pStrInfo_base_Disabled);
      }
    }

    vlanListIndex = 0;

    /* List DAI Configuration on the VLANs specified in the vlan list */

    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowVlanHeader);
    ewsTelnetWrite(ewsContext, pStrInfo_DaiShowVlanHeaderLine);
  }
  else
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  while(vlanListIndex < vlanCount)
  {
    configuredEntry = L7_FALSE;

    /* Find if the VLAN has a non-default DAI configuration */
    do
    {
      if(usmDbDaiVlanEnableGet(vlanList[vlanListIndex], &val) == L7_SUCCESS)
      {
        if(val != FD_DAI_VLAN)
        {
          configuredEntry = L7_TRUE;
          break;
        }
      }
      if(usmDbDaiVlanLoggingEnableGet(vlanList[vlanListIndex], &val) == L7_SUCCESS)
      {
        if(val != FD_DAI_LOG_INVALID)
        {
          configuredEntry = L7_TRUE;
          break;
        }
      }
      usmDbDaiVlanArpAclGet(vlanList[vlanListIndex], arpAclName);
      if(strcmp(arpAclName, "") != 0)
      {
        configuredEntry = L7_TRUE;
        break;
      }
    }
    while(0);

    /* Iterate through all the VLANs in the range and display those rows that have
     * a non-default DAI configuration.
     * If single VLAN is given as an argument, display the row irrespective of
     * the configuration on that VLAN */
    if((L7_TRUE == configuredEntry) || (1 == vlanCount))
    {
      ewsTelnetPrintf(ewsContext, "\r\n%4d  ", vlanList[vlanListIndex]);

      if(usmDbDaiVlanEnableGet(vlanList[vlanListIndex], &val) == L7_SUCCESS)
      {
        if(val == L7_ENABLE)
        {
          ewsTelnetPrintf(ewsContext, "%13s ", pStrInfo_base_Enabled);
        }
        else
        {
          ewsTelnetPrintf(ewsContext, "%13s ", pStrInfo_base_Disabled);
        }
      }
      else
      {
        ewsTelnetPrintf(ewsContext, "%13s ", "");
      }
      if(usmDbDaiVlanLoggingEnableGet(vlanList[vlanListIndex], &val) == L7_SUCCESS)
      {
        if(val == L7_ENABLE)
        {
          ewsTelnetPrintf(ewsContext, "%11s ", pStrInfo_base_Enabled);
        }
        else
        {
          ewsTelnetPrintf(ewsContext, "%11s ", pStrInfo_base_Disabled);
        }
      }
      else
      {
        ewsTelnetPrintf(ewsContext, "%11s ", "");
      }
      usmDbDaiVlanArpAclGet(vlanList[vlanListIndex], arpAclName);
      ewsTelnetPrintf(ewsContext, "%32s ",arpAclName);

      if(strcmp(arpAclName, "") != 0)
      {
        if(usmDbDaiVlanArpAclStaticFlagGet(vlanList[vlanListIndex], &val) == L7_SUCCESS)
        {
          if(val == L7_ENABLE)
          {
            ewsTelnetPrintf(ewsContext, "%11s", pStrInfo_base_Enabled);
          }
          else
          {
            ewsTelnetPrintf(ewsContext, "%11s", pStrInfo_base_Disabled);
          }
        }
        else
        {
          ewsTelnetPrintf(ewsContext, "%11s", "");
        }
      }

      count++;
      vlanListIndex++;

      if (count >= temp)
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
        return pStrInfo_common_Name_2;               /*--More-- or (q)uit */
      }
    }
    else
    {
      vlanListIndex++;
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Displays the Dynamic ARP Inspection information of the interface(s).
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
* @notes     Here we  display the interface specific DAI info.
*
* @cmdsyntax    show ip arp inspection interfaces [<u/s/p>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDaiIf(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 **argv, L7_uint32 index)
{
  L7_BOOL trust;
  static L7_uint32 intIfNum;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg=0, unit, intSlot, intPort;
  L7_uint32 count = 0, temp = 0, cfgIntf = 0;
  L7_uint32 rate, burst, val;
  L7_BOOL portEnabled = L7_FALSE;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_ShowDaiIf);
  }

  temp = CLI_MAX_SCROLL_LINES-6;
  cliCmdScrollSet( L7_FALSE);

  if (numArg == 1)
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if (cliValidSpecificUSPCheck(argv[index+1], &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              pStrErr_common_InvalidSlotPort_1);
      }

      /* Get interface and check its validity */
      if (usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,  
                                              ewsContext, pStrErr_common_InvalidSlotPort_1);
      }
    }
    else
    {
      unit = cliGetUnitId();
      if (cliSlotPortToIntNum(ewsContext, argv[index+1], &intSlot, 
                              &intPort, &cfgIntf) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowIfHeader);
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowIfUnits);
    ewsTelnetWrite(ewsContext, pStrInfo_DaiShowIfHeaderLine);
  }
  else
  {
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowIfHeader);
      ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowIfUnits);
      ewsTelnetWrite(ewsContext, pStrInfo_DaiShowIfHeaderLine);
      intIfNum = 0;
    }
    else
    {
      if (L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }
    }
  }

  while((cfgIntf != 0) || (usmDbValidIntIfNumNext(intIfNum, &intIfNum) == L7_SUCCESS))
  {
    if(cfgIntf == 0)
    {
      /* get unit slot and port for this interface*/
      if (usmDbUnitSlotPortGet(intIfNum, &unit, &intSlot, &intPort) != L7_SUCCESS)
      {
        continue;
      }
    }
    else
    {
      /* show ip arp inspection interfaces <u/s/p> */
      intIfNum = cfgIntf;
    }

    /* In the display of all interfaces information, we shall show only
     * those interfaces enabled for DAI (i.e., with atleast one 
     * participating vlan enabled for DAI)
     * In the display of specific interface information, we shall show its
     * information irrespective of DAI enabled for it */
    if (((cfgIntf != 0) ||
         ((usmDbDaiPortEnabledGet(intIfNum, &portEnabled) == L7_SUCCESS) &&
          (portEnabled))) &&
         (usmDbDaiIntfTrustGet(intIfNum, &trust) == L7_SUCCESS))
    {
      osapiSnprintf(stat, sizeof(stat), cliDisplayInterfaceHelp(unit, intSlot, intPort));
      memset (buf, 0,sizeof(buf));
      if (trust)
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%10s   %13s", stat, pStrInfo_common_Yes);
      }
      else
      {
        osapiSnprintf(buf, sizeof(buf), "\r\n%10s   %13s", stat, pStrInfo_common_No);
      }

      if (usmDbDaiIntfRateLimitGet(intIfNum, &rate) == L7_SUCCESS)
      {
        if(rate == L7_DAI_RATE_LIMIT_NONE)
        {
          osapiSnprintf(stat, sizeof(stat), "   %13s", pStrInfo_base_LimitNone_1);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "   %13d", rate);
        }
        osapiStrncat(buf, stat, sizeof(buf));
      }
      if (usmDbDaiIntfBurstIntervalGet(intIfNum, &burst) == L7_SUCCESS)
      {
        if(rate == L7_DAI_RATE_LIMIT_NONE)
        {
          osapiSnprintf(stat, sizeof(stat), "   %15s", pStrInfo_base_NA);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "   %15d", burst);
        }
        osapiStrncat(buf, stat, sizeof(buf));
      }
      count++;
      ewsTelnetWrite(ewsContext, buf);
    }

    if(cfgIntf != 0)
      break;

    if (count >= temp)
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
      return pStrInfo_common_Name_2;               /*--More-- or (q)uit */
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Displays the statistics in brief for all the DAI enabled VLANs
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
* @cmdsyntax    show ip arp inspection statistics
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDaiStatsBrief(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index)
{
  static L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 vlanCurrent;
  L7_uint32 numArg=0, count = 0, temp = 0, val;
  daiVlanStats_t stats;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_ShowDaiStatsBrief);
  }

  temp = CLI_MAX_SCROLL_LINES-6;
  cliCmdScrollSet( L7_FALSE);

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    vlanCurrent = L7_DOT1Q_DEFAULT_VLAN;
    rc          = L7_SUCCESS;

    /* List DAI Statistics in brief on the DAI enabled VLANs */

    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowStatsBriefHeader);
    ewsTelnetWrite(ewsContext, pStrInfo_DaiShowStatsBriefHeaderLine);
  }
  else
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  while(rc == L7_SUCCESS)
  {
    if((usmDbDaiVlanEnableGet(vlanCurrent, &val) == L7_SUCCESS) &&
       (val == L7_ENABLE))
    {
      if(usmDbDaiVlanStatsGet(vlanCurrent, &stats) == L7_SUCCESS)
      {
        ewsTelnetPrintf(ewsContext, "\r\n%4d", vlanCurrent);
        ewsTelnetPrintf(ewsContext, "   %12d", stats.forwarded);
        ewsTelnetPrintf(ewsContext, "  %9d", stats.dropped);
        count++;
      }
    }
    rc = usmDbDaiVlanNextGet(vlanCurrent, &vlanCurrent);
    if (count >= temp)
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
      return pStrInfo_common_Name_2;               /*--More-- or (q)uit */
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Displays the DAI statistics in detail for a list of VLANs
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
* @cmdsyntax    show ip arp inspection statistics vlan <vlan-list>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowDaiStatsVlan(EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 **argv, L7_uint32 index)
{
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 vlanListIndex;
  L7_uint32 numArg=0, count = 0, temp = 0, val;
  L7_BOOL singleVlan = L7_FALSE;
  daiVlanStats_t stats;
  L7_uint32 vlanList[L7_MAX_VLANS], vlanCount = 0, i = 0;
  L7_char8 *strVlanList;
  L7_uchar8 tempBuf[L7_CLI_MAX_LARGE_STRING_LENGTH];

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_ShowDai);
  }

  temp = CLI_MAX_SCROLL_LINES-6;
  cliCmdScrollSet( L7_FALSE);

  /* Get list of VLAN IDs */
  strVlanList = (L7_char8 *) argv[index + 1];

  if (L7_SUCCESS != cliParseRangeInput(strVlanList, &vlanCount, vlanList, L7_MAX_VLANS))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                            L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
    ewsTelnetWrite(ewsContext, tempBuf);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(vlanCount == 1)
  {
    singleVlan = L7_TRUE;
  }

  /* Check if any vlan in the list is invalid */
  for(i = 0; i < vlanCount; i++)
  {
    if((vlanList[i] > L7_MAX_VLANS) || (vlanList[i] <= 0))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, NULL, tempBuf, sizeof(tempBuf), pStrInfo_switching_VlanIdOutOfRange_1,
                              L7_MAX_VLANS, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite(ewsContext, tempBuf);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    vlanListIndex = 0;

    /* List DAI statistics in detail on the VLANs specified in the vlan list */

    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowStatsDetailHeader);
    ewsTelnetWriteAddBlanks(1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_DaiShowStatsDetailHeader_2);
    ewsTelnetWrite(ewsContext, pStrInfo_DaiShowStatsDetailHeaderLine);
  }
  else
  {
    if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  while(vlanListIndex < vlanCount)
  {
    if(((usmDbDaiVlanEnableGet(vlanList[vlanListIndex], &val) == L7_SUCCESS) &&
        (val == L7_ENABLE)) || (singleVlan == L7_TRUE))
    {
      if(usmDbDaiVlanStatsGet(vlanList[vlanListIndex], &stats) == L7_SUCCESS)
      {
        ewsTelnetPrintf(ewsContext, "\r\n%4d", vlanList[vlanListIndex]);
        ewsTelnetPrintf(ewsContext, " %10d", stats.dhcpDrops);
        ewsTelnetPrintf(ewsContext, " %10d", stats.aclDrops);
        ewsTelnetPrintf(ewsContext, " %10d", stats.dhcpPermits);
        ewsTelnetPrintf(ewsContext, " %10d", stats.aclPermits);
        ewsTelnetPrintf(ewsContext, " %10d", stats.sMacFailures);
        ewsTelnetPrintf(ewsContext, " %10d", stats.dMacFailures);
        ewsTelnetPrintf(ewsContext, " %9d", stats.ipValidFailures);
        count++;
      }
    }
    vlanListIndex++;
    if (count >= temp)
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
      return pStrInfo_common_Name_2;               /*--More-- or (q)uit */
    }
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose Displays the ARP ACL configuration
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
* @cmdsyntax    show arp access-list [<acl-name>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowArpAcl(EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 **argv, L7_uint32 index)
{
  static L7_RC_t rc = L7_SUCCESS;
  static L7_uchar8 arpAclCurrent[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 ipAddrCurrent;
  static L7_enetMacAddr_t macAddrCurrent;
  L7_uchar8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg=0, count = 0, temp = 0, val;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                           ewsContext, pStrErr_base_ShowArpAcl);
  }

  if (numArg == 1)
  {
    if(usmDbArpAclGet((L7_char8 *)argv[index+1]) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, pStrErr_ArpAccessListDoesntExist);
    }
  }

  temp = CLI_MAX_SCROLL_LINES-6;

  if (numArg == 1)
  {
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      OSAPI_STRNCPY_SAFE(arpAclCurrent, argv[index+1]);
      memset(&macAddrCurrent, 0, sizeof(macAddrCurrent));
      ipAddrCurrent = 0;
      ewsTelnetPrintf(ewsContext, "\r\n%s %s",pStrInfo_base_ArpAclEntry, arpAclCurrent);
      count++;
      rc = usmDbArpAclRuleInAclNextGet(arpAclCurrent, ipAddrCurrent, &macAddrCurrent,
                                       &ipAddrCurrent, &macAddrCurrent);
    }
    else if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      memset(arpAclCurrent, 0, sizeof(arpAclCurrent));
      ipAddrCurrent = 0;
      memset(&macAddrCurrent, 0, sizeof(macAddrCurrent));
      rc = L7_FAILURE;
    }
    else if (L7_TRUE == cliIsPromptRespQuit())
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }

  for(;;)
  {
    if(rc != L7_SUCCESS)
    {
      if(numArg == 1)
        break;

      if((rc = usmDbArpAclNextGet(arpAclCurrent, arpAclCurrent)) != L7_SUCCESS)
        break;
      memset(&macAddrCurrent, 0, sizeof(macAddrCurrent));
      ipAddrCurrent = 0;
      ewsTelnetPrintf(ewsContext, "\r\n%s %s",pStrInfo_base_ArpAclEntry, arpAclCurrent);
      count++;
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      usmDbInetNtoa(ipAddrCurrent, buf);
      ewsTelnetPrintf(ewsContext, "\r\n    permit ip host %s mac host %02x:%02x:%02x:%02x:%02x:%02x",
                      buf, macAddrCurrent.addr[0], macAddrCurrent.addr[1], macAddrCurrent.addr[2],
                      macAddrCurrent.addr[3], macAddrCurrent.addr[4], macAddrCurrent.addr[5]);
      count++;
    }
    rc = usmDbArpAclRuleInAclNextGet(arpAclCurrent, ipAddrCurrent, &macAddrCurrent,
                                     &ipAddrCurrent, &macAddrCurrent);
    if (count >= temp)
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
      return pStrInfo_common_Name_2;               /*--More-- or (q)uit */
    }
  }

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  return cliSyntaxReturnPrompt (ewsContext, "");
}

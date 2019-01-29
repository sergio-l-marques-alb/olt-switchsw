/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_snooping_querier.c
 *
 * @purpose Snooping Querier show commands for cli
 *
 * @component CLI
 *
 * @comments none
 *
 * @create  14-Dec-2006
 *
 * @author  drajendra
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "clicommands.h"
#include "clicommands_snooping_querier.h"
#include "ews.h"
#include "usmdb_snooping_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "osapi_support.h"
#include "snooping_exports.h"

/*********************************************************************
*
* @purpose  display igmp/mld snooping querier information
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
* @cmdsyntax  show igmpsnooping querier [{detail | vlan <vlanid>}]
* @cmdsyntax  show mldsnooping querier [{detail | vlan <vlanid>}]
*
* @cmdhelp
*
* @cmddescript   display summary information about snooping querier
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowSnoopingQuerier(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 numArg = 0, val, vlanId, argVlanId = 2;
  static L7_uint32 tempVid, recallCode = 0;
  L7_uchar8 family = L7_AF_INET;
  L7_BOOL globalShow = L7_FALSE, vlanStatusShow = L7_FALSE;
  L7_BOOL vlanTableShow = L7_FALSE, loopFlag = L7_TRUE;
  L7_uchar8 ipAddr[L7_IP6_ADDR_LEN];

  L7_uint32 lineCount = 0;

  cliCmdScrollSet( L7_FALSE);

  memset(stat, 0, sizeof(stat));
  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmpsnooping) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mldsnooping) == 0)
  {
    family = L7_AF_INET6;
  }
  else
  {
    family = L7_AF_INET;
  }

  if (numArg > 2)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SHOW_SNOOPQUERIER_HELP(family));
  }

  if (numArg == 0) /* Global configuration */
  {
    globalShow = L7_TRUE;
    recallCode = 0;
  }
  else if (numArg == 1) /* detail */
  {
    globalShow = L7_TRUE;
    vlanStatusShow = L7_TRUE;
    vlanTableShow  = L7_TRUE;
    if (cliGetCharInputID() != CLI_INPUT_EMPTY)
    {
      if (L7_TRUE == cliIsPromptRespQuit())
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
      }

      /* Figure out where the command was stopped */
      if (recallCode == 2)
      {
        vlanTableShow = L7_FALSE;
      }
      else if (recallCode == 3)
      {
        vlanTableShow = L7_FALSE;
        globalShow    = L7_FALSE;
      }
    }
    else
    {
      recallCode = 0; 
      tempVid = L7_DOT1Q_MIN_VLAN_ID;
    }
  }
  else if (numArg == 2) /* vlan <vid> */
  {
    vlanStatusShow = L7_TRUE;
    loopFlag       = L7_FALSE;
    recallCode = 0;
    if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argVlanId], &vlanId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (vlanId < L7_DOT1Q_MIN_VLAN_ID ||
        vlanId> L7_DOT1Q_MAX_VLAN_ID)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidVlanIdSpecified);
    }
  }

  if (vlanTableShow == L7_TRUE)
  {
    if (family == L7_AF_INET)
    {
      ewsTelnetWriteAddBlanks (1, 0, 8, 17, L7_NULLPTR, ewsContext,pStrInfo_switching_LastQuerier);
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanIdAddrIgmpVer);
      ewsTelnetWrite(ewsContext,"\r\n------- ---------------- ------------");
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanIdLastQuerierAddrMldVer);
      ewsTelnetWrite(ewsContext,"\r\n------- ---------------------------------------- -----------");
    }

    if (recallCode == 0)
    {
      lineCount += 3;
    }

    tempVid = L7_NULL;
    while (usmDbSnoopVlanNextGet(tempVid, &tempVid) == L7_SUCCESS)
    {
      if (usmDbSnoopQuerierLastQuerierAddressGet(tempVid, ipAddr, family) == L7_SUCCESS)
      {
        if (lineCount == CLI_MAX_SCROLL_LINES-4)
        {
          recallCode = 1;
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          memset(buf,0,sizeof(buf));
          osapiSnprintf( buf, sizeof(buf),CLISNOOP_SHOW_SNOOPING_QUERIER_VLAN_DETAIL(family),argv[index+1]);
          cliAlternateCommandSet(buf);
          return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
        }
        lineCount++;
        osapiInetNtop(family, ipAddr, stat, L7_CLI_MAX_STRING_LENGTH);
        /* VLAN ID */
        /* Oper Version */
        rc = usmDbSnoopQuerierLastQuerierVersionGet(tempVid, &val, family);
        if (rc == L7_SUCCESS)
        {
          if (family == L7_AF_INET)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_switching_V, tempVid, stat, val);
          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_switching_V_1, tempVid, stat, val);
          }
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), "\r\n%-7d %s", tempVid, stat);
        }
        ewsTelnetWrite(ewsContext, buf);
      }/* Check for last querier address */
    }
    cliSyntaxBottom(ewsContext);
  }

  if (globalShow == L7_TRUE)
  {
    if (lineCount == CLI_MAX_SCROLL_LINES-4-8)
    {
      recallCode = 2;
      cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
      memset(buf,0,sizeof(buf));
      osapiSnprintf( buf, sizeof(buf),CLISNOOP_SHOW_SNOOPING_QUERIER_VLAN_DETAIL(family),argv[index+1]);
      cliAlternateCommandSet(buf);
      return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
    }

    lineCount += 8; /* Number of lines taken by global section */

    if (family == L7_AF_INET)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_GlobalIgmpSnoopingQuerierStatus);
      ewsTelnetWrite(ewsContext,"\r\n-----------------------------------");
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_GlobalMldSnoopingQuerierStatus);
      ewsTelnetWrite(ewsContext,"\r\n----------------------------------");
    }

    /* Global Admin Mode */
    cliFormat(ewsContext, CLISNOOP_QUERIER_MODE(family));
    if (usmDbSnoopQuerierAdminModeGet(&val,family) == L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val , pStrInfo_common_Dsbl_1));
    }

    /* Global Address */
    cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierAddr);
    if (usmDbSnoopQuerierAddressGet(ipAddr,family) == L7_SUCCESS)
    {
      osapiInetNtop(family, ipAddr, stat, L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetWrite(ewsContext, stat);
    }

    /* Querier Version */
    cliFormat(ewsContext, CLISNOOP_QUERIER_VERSION(family));
    if (usmDbSnoopQuerierVersionGet(&val,family) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%d", val);
    }

    /* Query Interval */
    cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierQueryInt);
    if (usmDbSnoopQuerierQueryIntervalGet(&val,family) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%d", val);
    }

    /* Querier Interval */
    cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierQuerierInt);
    if (usmDbSnoopQuerierExpiryIntervalGet(&val,family) == L7_SUCCESS)
    {
      ewsTelnetPrintf (ewsContext, "%d", val);
    }
    cliSyntaxBottom(ewsContext);
  }

  if (vlanStatusShow == L7_TRUE)
  {
    if (loopFlag == L7_FALSE)
    {
      tempVid = vlanId;
    }
    else if (recallCode != 3)
    {
      tempVid = L7_DOT1Q_MIN_VLAN_ID;
    }

    tempVid = L7_NULL;
    while (usmDbSnoopVlanNextGet(tempVid, &tempVid) == L7_SUCCESS)
    {
      rc = usmDbSnoopQuerierVlanModeGet(tempVid,&val,family);
      if ((rc == L7_SUCCESS && val == L7_ENABLE) || loopFlag == L7_FALSE)
      {
        if (lineCount >= CLI_MAX_SCROLL_LINES-12)
        {
          recallCode = 3;
          cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
          memset(buf,0,sizeof(buf));
          osapiSnprintf( buf, sizeof(buf),CLISNOOP_SHOW_SNOOPING_QUERIER_VLAN_DETAIL(family),argv[index+1]);
          cliAlternateCommandSet(buf);
          return pStrInfo_common_Name_2;    /* --More-- or (q)uit */
        }

        /* VLAN ID */
        if (family == L7_AF_INET)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_VlanIgmpSnoopingQuerierStatus, tempVid);
          ewsTelnetWrite(ewsContext, stat);
        }
        else
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_VlanMldSnoopingQuerierStatus, tempVid);
          ewsTelnetWrite(ewsContext, stat);
        }

        ewsTelnetWrite(ewsContext, "\r\n----------------------------------------------");

        /* VLAN Admin Mode */
        cliFormat(ewsContext, CLISNOOP_QUERIER_VLAN_MODE(family));
        ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val , pStrInfo_common_Dsbl_1));

        /* Querier Election Admin Mode */
        cliFormat(ewsContext, pStrInfo_switching_SnoopQuerier_1);
        if (usmDbSnoopQuerierVlanElectionModeGet(tempVid, &val, family) == L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext,strUtilEnableDisableGet(val , pStrInfo_common_Dsbl_1));
        }

        /* Querier IP Address */
        cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierVlanAddr);
        if (usmDbSnoopQuerierVlanAddressGet(tempVid, ipAddr, family) == L7_SUCCESS)
        {
          osapiInetNtop(family, ipAddr, stat, L7_CLI_MAX_STRING_LENGTH);
          ewsTelnetWrite(ewsContext, stat);
        }

        /* Operational State */
        if (usmDbSnoopQuerierOperStateGet(tempVid, &val, family)
            == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierOperState);
          switch(val)
          {
          case L7_SNOOP_QUERIER_QUERIER:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Querier);
            break;
          case L7_SNOOP_QUERIER_NON_QUERIER:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NonQuerier);
            break;
          case L7_SNOOP_QUERIER_DISABLED:
            osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Dsbld);
            break;
          default:
            memset(buf, 0, sizeof(buf));
            break;
          }
          ewsTelnetPrintf (ewsContext, buf);
        }

        /* Last Querier */
        if (usmDbSnoopQuerierLastQuerierAddressGet(tempVid, ipAddr, family)
            == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_switching_SnoopLastQuerierAddr);
          osapiInetNtop(family, ipAddr, stat, L7_CLI_MAX_STRING_LENGTH);
          ewsTelnetWrite(ewsContext, stat);
        }

        /* Operational Version */
        if (usmDbSnoopQuerierOperVersionGet(tempVid, &val, family)
            == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierOperVer);
          ewsTelnetPrintf (ewsContext, "%d", val);
        }

        /* Operational Max Response Time */
        if (usmDbSnoopQuerierOperMaxRespTimeGet(tempVid, &val, family)
            == L7_SUCCESS)
        {
          cliFormat(ewsContext, pStrInfo_switching_SnoopQuerierOperMaxResp);
          if (val == 0)
          {
            osapiSnprintf(stat, sizeof(stat), pStrInfo_switching_1_1);
          }
          else
          {
            osapiSnprintf(stat, sizeof(stat), "%d", val);
          }
          ewsTelnetWrite(ewsContext, stat);
        }
        cliSyntaxBottom(ewsContext);
        lineCount += 10;
      }
      if (loopFlag == L7_FALSE)
      {
        break;
      }
    }/* VLAN Status loop */

  }

  return cliPrompt(ewsContext);
}

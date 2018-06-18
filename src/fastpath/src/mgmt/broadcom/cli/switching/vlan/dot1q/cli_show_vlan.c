/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/switching/cli_show_vlan.c
*
* @purpose vlan show commands for the cli
*
* @component user interface
*
* @comments none
*
* @create  10/29/2000
*
* @author  djohnson/Forrest Samuels
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "clicommands_card.h"
#include "clicommands_vlan.h"
#include "ews.h"
#include "usmdb_2233_stats_api.h"   /* for usmDbIfNameGet(), oddly enough */
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_protected_port_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Display vlan information based on the vlan id
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking, replace help string
*
* @cmdsyntax  show vlan <1-4094>
*
* @cmdhelp Display detailed information about a specific VLAN
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVlan(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 nextInterface;
  static L7_uint32 vlanID;
  L7_uchar8 buf[81], stat[81];
  L7_RC_t rc;
  L7_uint32 val;
  L7_uint32 unitNum, slot, port;
  L7_uint32 sysIntfType;
  L7_uint32 i = 0;
  L7_uint32 argVlanId = 1;
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  vlanBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_int32  count;
  static L7_uint32 vlanType;
  L7_BOOL headerPrinted = L7_FALSE;

  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);
  memset(vlanBuf, 0x0, sizeof(vlanBuf));


  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg == 0)
  {
    return commandShowVlanBrief(ewsContext, argc, argv, index);
  }

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowVlanDetailed, L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
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
  if (cliCheckIfInteger((L7_char8 *)argv[index+argVlanId]) != L7_SUCCESS)
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }
  sscanf(argv[index+argVlanId], "%d", &vlanID);

  rc = usmDbVlanIDGet(unit, vlanID);
  if (rc != L7_SUCCESS)
  {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_VlanDoesntExist);
    }

    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanId_2);
    ewsTelnetPrintf (ewsContext, "%-6d", vlanID);

    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanName_2);
    memset(buf, 0, sizeof(buf));
  rc = usmDbVlanNameGet(unit, vlanID, buf);
    ewsTelnetPrintf (ewsContext, buf);

    ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanType_1);
  rc = usmDbVlanTypeGet(unit, vlanID, &val);
  vlanType=val;
  if (val == L7_DOT1Q_DEFAULT)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Defl);
    }
    else if (val == L7_DOT1Q_ADMIN)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_PimSmGrpRpMapStatic);
    }
    else if (val == L7_DOT1Q_DYNAMIC || val == L7_DOT1Q_WS_DYNAMIC || val == L7_DOT1Q_IP_INTERNAL_VLAN || val == L7_DOT1Q_DOT1X_REGISTERED)
    {
      if(val == L7_DOT1Q_DYNAMIC)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynGVRP_1);
      }
      else if(val == L7_DOT1Q_WS_DYNAMIC)
      { 
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynWLAN_1);
      } 
      else if(val == L7_DOT1Q_IP_INTERNAL_VLAN)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynIP_VLAN_1);
      }
      else if(val == L7_DOT1Q_DOT1X_REGISTERED)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynDOT1X_1);
      } 

      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dyn_1);
      osapiStrncat(stat, vlanBuf, strlen(vlanBuf));
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "------");
    }

    ewsTelnetWrite(ewsContext,stat);

    if (usmDbValidIntIfNumFirstGet(&nextInterface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    i=nextInterface;

    if ((usmDbDot1qIsValidIntf(unit, i) == L7_TRUE) &&
        (nimGetIntfType(i, &sysIntfType) == L7_SUCCESS) &&
        (usmDbUnitSlotPortGet(i, &unitNum, &slot, &port) == L7_SUCCESS))
    {
      if (!(sysIntfType == L7_LAG_INTF  &&  usmDbDot3adIsConfigured(unit,i) == L7_FALSE))
      {
        if(headerPrinted == L7_FALSE)
        {
          ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfCurrentCfguredTagging);
          ewsTelnetWrite(ewsContext,"\r\n----------  --------  -----------  --------");
          headerPrinted = L7_TRUE;
          count = 3;
        }

        ewsTelnetPrintf (ewsContext, "\r\n%-12s", cliDisplayInterfaceHelp(unitNum,slot,port));

        val = 0;
        rc = usmDbOperVlanMemberSetGet(unit, vlanID, i, &val);
        switch (val)
        {
        case L7_DOT1Q_FIXED:
          osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_common_Incl);
            break;
        case L7_DOT1Q_FORBIDDEN:
          osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_common_Excl);
            break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_common_Blank);
            break;
        }
        ewsTelnetWrite(ewsContext,stat);

        val = 0;
        rc = usmDbVlanMemberSetGet(unit, vlanID, i, &val);
        switch (val)
        {
        case L7_DOT1Q_FIXED:
          osapiSnprintf(stat, sizeof(stat), "%-12s ", pStrInfo_common_Incl);
            break;
        case L7_DOT1Q_FORBIDDEN:
          osapiSnprintf(stat, sizeof(stat), "%-12s ", pStrInfo_common_Excl);
            break;
        case L7_DOT1Q_NORMAL_REGISTRATION:
          osapiSnprintf(stat, sizeof(stat), "%-12s ", pStrInfo_switching_AutoDetect);
            break;
        default:
          osapiSnprintf(stat, sizeof(stat), "%-12s ", pStrInfo_common_Blank);
            break;
        }
        ewsTelnetWrite(ewsContext,stat);

        val=0;
        if(vlanType == L7_DOT1Q_DYNAMIC || vlanType == L7_DOT1Q_WS_DYNAMIC || L7_DOT1Q_IP_INTERNAL_VLAN)
        {
          rc= usmDbDot1qOperVlanTaggedMemberGet(unit,vlanID,i,&val);
        }
        else
        {
          rc = usmDbVlanTaggedSetGet(unit, vlanID, i, &val);
        }
        if(rc == L7_SUCCESS)
        {
          switch (val)
          {
            case L7_DOT1Q_UNTAGGED:
            osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_switching_UnTagged);
                break;
            case L7_DOT1Q_TAGGED:
            osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_switching_Tagged);
                break;
            default:
            osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_common_Blank);
                break;
          }
        }
        else
        {
          /* error getting tagged staate for port*/
          osapiSnprintf(stat, sizeof(stat), "%-9s ", pStrInfo_common_Blank);
        }

        ewsTelnetWrite(ewsContext,stat);
      }
    }

    if (usmDbValidIntIfNumNext(i, &nextInterface) != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
}
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
* @purpose  display vlan entries
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
* @cmdsyntax  show vlan brief
*
* @cmdhelp Display summary information about all VLANs
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVlanBrief(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 nextvid;
  static L7_RC_t rc;
  L7_uint32 val, i;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  vlanBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  vlanBuf1[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;        /* New variable Added */
  L7_int32  count;

  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);
  memset(vlanBuf, 0x0, sizeof(vlanBuf));
  memset(vlanBuf1, 0x0, sizeof(vlanBuf1));

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowVlanSummary_1);
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
    nextvid = L7_NULL;
    /* default VLAN may not exist if the system is on its way up */
    rc = usmDbNextVlanGet(unit, L7_NULL, &nextvid);
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanIdVlanNameVlanType);
  ewsTelnetWrite(ewsContext,"\r\n------- -------------------------------- -------------------");

  count = 0;
  i = 0;
  while ((rc == L7_SUCCESS) && (count < CLI_MAX_SCROLL_LINES-6))
  {
    /* print VLAN ID */
    i = nextvid;
    ewsTelnetPrintf (ewsContext, "\r\n%-7d ", i);

    /* print optional VLAN name */
    rc = usmDbVlanNameGet(unit, i, buf);
    if ( rc == L7_SUCCESS)
    {
    ewsTelnetPrintf (ewsContext, "%-33s ", buf);
    }
    else 
    {
     sprintf(stat,"%-33s ", "");
     ewsTelnetWrite(ewsContext,stat);
    }

    /* print VLAN type */
    rc = usmDbVlanTypeGet(unit, i, &val);
    if (val == L7_DOT1Q_DEFAULT)
    {
      osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_Defl);
    }
    else if (val == L7_DOT1Q_ADMIN)
    {
      osapiSnprintf(stat, sizeof(stat), "%-9s", pStrInfo_common_PimSmGrpRpMapStatic);
    }
    else if (val == L7_DOT1Q_DYNAMIC || val == L7_DOT1Q_WS_DYNAMIC || val == L7_DOT1Q_IP_INTERNAL_VLAN || val == L7_DOT1Q_DOT1X_REGISTERED)
    {

      if(val == L7_DOT1Q_DYNAMIC)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynGVRP_1);
      }
      else if(val == L7_DOT1Q_WS_DYNAMIC)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynWLAN_1);
      }
      else if(val == L7_DOT1Q_IP_INTERNAL_VLAN)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynIP_VLAN_1);
      }
      else if(val == L7_DOT1Q_DOT1X_REGISTERED)
      {
        osapiSnprintf(vlanBuf, sizeof(vlanBuf), pStrInfo_common_DynDOT1X_1);
      }

      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Dyn_1);
      osapiStrncat(stat, vlanBuf, strlen(vlanBuf));
 
      osapiSnprintf(vlanBuf, sizeof(vlanBuf1), "%-19s", stat);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), "%-19s", "------");
    }
    ewsTelnetWrite(ewsContext,stat);

    rc = usmDbNextVlanGet(unit, i, &nextvid);
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

/*********************************************************************
*
* @purpose  display 802.1Q port parameters
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add argument checking
*
* @cmdsyntax  show vlan port {<slot/port> | all}
*
* @cmdhelp Display 802.1Q port parameters.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandShowVlanPort(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  static L7_uint32 nextInterface;
  static L7_BOOL all;
  L7_uint32 val;
  L7_RC_t   rc;
  L7_uint32 i, groupId = 0;
  L7_uint32 unitNum, slot, port;
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 interface;
  L7_uint32 sysIntfType;
  L7_uint32 argSlotPort=1;
  L7_uint32 unit;
  L7_int32  retVal;
  L7_uint32 numArg;        /* New variable Added */
  static L7_uint32 lagSupported = L7_FALSE;
  L7_int32  count;
  L7_BOOL headerPrinted = L7_FALSE;

  cliSyntaxTop(ewsContext);

  cliCmdScrollSet( L7_FALSE);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowVlanPort_1, cliSyntaxInterfaceHelp());
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
    if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
    {
      all = L7_TRUE;

      if (usmDbValidIntIfNumFirstGet(&nextInterface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }

      /* get switch ID based on presence/absence of STACKING package */
      unit = cliGetUnitId();
      if (unit == 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
      }
    }
    else
    {
      all = L7_FALSE;

      if (cliIsStackingSupported() == L7_TRUE)
      {
        if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
        {
          if (rc == L7_ERROR)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
            ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit, slot, port);
          }
          else if (rc == L7_NOT_EXIST)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
          }
          else if (rc == L7_NOT_SUPPORTED)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
          }

          return cliSyntaxReturnPrompt (ewsContext, "");
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(unit, slot, port, &interface) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, slot, port);
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

      if (usmDbVisibleInterfaceCheck(unit, interface, &retVal) != L7_SUCCESS)
      {
        if (retVal == -1)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }

      nextInterface = interface;
    }

    if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
    {
         lagSupported = L7_TRUE;
    }

    if( usmDbIntfTypeGet(nextInterface, &sysIntfType)== L7_SUCCESS)
    {
      /* Only show physical interfaces or lag interfaces */
      /* Not a physical interface and not a lag interface */
      if ((sysIntfType != L7_PHYSICAL_INTF) &&
          (!((lagSupported == L7_TRUE) && (sysIntfType == L7_LAG_INTF))))
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        memset (stat, 0, sizeof(stat));
        if (cliIsStackingSupported() == L7_TRUE)
        {
          osapiSnprintf(stat, sizeof(stat), "%u/%u/%u", unit, slot, port);
        }
        else
        {
          osapiSnprintf(stat, sizeof(stat), "%u/%u", slot, port);
        }
        return cliSyntaxReturnPrompt (ewsContext, stat);
      }
    }
  }

  for (count = 0; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    i=nextInterface;

    if ((usmDbIntfTypeGet(i, &sysIntfType) == L7_SUCCESS) &&
        (usmDbUnitSlotPortGet(i, &unitNum, &slot, &port) == L7_SUCCESS) &&
        (sysIntfType != L7_CPU_INTF))
    {
      if ((sysIntfType == L7_PHYSICAL_INTF) || ((lagSupported == L7_TRUE) && (sysIntfType == L7_LAG_INTF)))
      {
        if(headerPrinted == L7_FALSE)
        {
          if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
          {
            ewsTelnetWriteAddBlanks (1, 0, 10, 19, L7_NULLPTR, ewsContext,pStrInfo_switching_PortPortIngressIngress);
            ewsTelnetWriteAddBlanks (1, 0, 10, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanIdVlanIdAcceptableFilteringFilteringDefl);
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfCfguredCurrentFrameTypesCfguredCurrentGvrpPri);
            ewsTelnetWrite(ewsContext,"\r\n--------- ---------- -------- ----------- ---------- --------- ------- --------");
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 10, 10, L7_NULLPTR, ewsContext,pStrInfo_switching_PortPortIngressIngress);
            ewsTelnetWriteAddBlanks (1, 0, 10, 8, L7_NULLPTR, ewsContext,pStrInfo_switching_VlanIdVlanIdAcceptableFilteringFiltering);
            ewsTelnetWriteAddBlanks (1, 0, 0, 2, L7_NULLPTR, ewsContext,pStrInfo_switching_IntfCfguredCurrentFrameTypesCfguredCurrentGvrp);
            ewsTelnetWrite(ewsContext,"\r\n--------- ---------- -------- ----------- ---------- --------- -------");
          }

          headerPrinted = L7_TRUE;
          count = 4;
        }

        ewsTelnetPrintf (ewsContext, "\r\n%-10s", cliDisplayInterfaceHelp(unitNum,slot,port));

        memset (stat, 0, sizeof(stat));
        rc = usmDbQportsPVIDGet(unit, i, &val);
        ewsTelnetPrintf (ewsContext, "%-10d ",val);

        memset (stat, 0, sizeof(stat));
        rc = usmDbQportsCurrentPVIDGet(unit, i, &val);
        ewsTelnetPrintf (ewsContext, "%-8d ",val);

        memset (stat, 0, sizeof(stat));
        rc = usmDbQportsAcceptFrameTypeGet(unit, i, &val);
        switch (val)
        {
        case L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED:
          osapiSnprintf(stat, sizeof(stat),"%-11s ",pStrInfo_switching_VlanOnly);
            break;
        case L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED:
          osapiSnprintf(stat, sizeof(stat),"%-11s ",pStrInfo_switching_UntaggedOnly);
            break;
        default:
          osapiSnprintf(stat, sizeof(stat),"%-11s ",pStrInfo_switching_AdmitAll);
            break;
        }
        ewsTelnetWrite(ewsContext,stat);

        memset (stat, 0, sizeof(stat));
        rc = usmDbQportsEnableIngressFilteringGet(unit, i, &val);
        ewsTelnetPrintf (ewsContext, "%-10s ",strUtilEnableDisableGet(val, pStrInfo_common_Blank));

        memset (stat, 0, sizeof(stat));
        rc = usmDbQportsCurrentIngressFilteringGet(unit, i, &val);
        ewsTelnetPrintf (ewsContext, "%-9s ",strUtilEnableDisableGet(val, pStrInfo_common_Blank));

        memset (stat, 0, sizeof(stat));
        rc = usmDbQportsEnableGVRPGet(unit, i, &val);
        ewsTelnetPrintf (ewsContext, "%-5s",strUtilEnableDisableGet(val, pStrInfo_common_Blank));

        if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_FEATURE_SUPPORTED) == L7_TRUE)
        {
          memset (stat, 0, sizeof(stat));
          rc = usmDbDot1dPortDefaultUserPriorityGet(unit, i, &val);
          ewsTelnetPrintf (ewsContext, "%6d ", val);
        }
      }
    }

    if(all == L7_FALSE)
    {
      cliFormatAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_ProtectedPort);
      memset(stat, 0, L7_CLI_MAX_STRING_LENGTH);
      if ((usmdbProtectedPortGroupIntfGet(unit, port, &groupId)) == L7_SUCCESS)
      {
        osapiSnprintf(stat, sizeof(stat), "%s ", pStrInfo_common_True2);
      }
      else
      {
        osapiSnprintf(stat, sizeof(stat), "%s ", pStrInfo_common_False2);
      }

      ewsTelnetWrite(ewsContext,stat);

      if(groupId > 0)
      {
        osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_InGrp_1, groupId);
        ewsTelnetWrite(ewsContext,stat);
      }
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    if ((all != L7_TRUE) || (usmDbValidIntIfNumNext(i, &nextInterface) != L7_SUCCESS))
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
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




/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2006-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_protectedport.c
 *
 * @purpose  protected port show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create 06/23/2005
 *
 * @author ryadagiri
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "cliapi.h"
#include "clicommands_card.h"
#include "ews.h"
#include "usmdb_nim_api.h"
#include "cli_web_exports.h"
#include "usmdb_protected_port_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Display switch port protection information for configured interfaces
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
* @cmdsyntax  show switchport protected <groupid>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *
commandShowSwitchPortProtected(EwsContext ewsContext, L7_uint32 argc,
                               const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 name[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc, rc1;
  L7_uint32 unitNum;
  L7_uint32 unit, slot, port;
  L7_uint32 nextintIfNum;
  L7_uint32 nextgroupId;
  L7_uint32 numArgs;
  L7_uint32 Linecount = 0;
  L7_uint32 argGroupId = 1;
  L7_BOOL flag = L7_FALSE;
  L7_BOOL commaflag = L7_FALSE;
  static L7_BOOL all = L7_TRUE;
  static L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  static L7_uint32 i = 0;
  static L7_uint32 groupId;
  static L7_uint32 count = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop (ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId ();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }
  numArgs = cliNumFunctionArgsGet ();
  cliCmdScrollSet( L7_FALSE);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {               /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
    {
      i = 0;
      all = L7_TRUE;
      groupId = 0;
      count = 0;
      memset(buf, 0, L7_CLI_MAX_STRING_LENGTH);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliPrompt(ewsContext);
    }
  }
  else
  {

    if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                                 L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
    {
      if (numArgs == 1)
      {
        OSAPI_STRNCPY_SAFE(strGroupId, argv[index + argGroupId]);
        if (cliCheckIfInteger (strGroupId) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
        }
        groupId = atoi (strGroupId);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowProtectedPortGrp);
      }
    }
    else
    {
      if (numArgs == 0)
      {
        groupId = 0;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_switching_ShowProtectedPortGrpNoArgs);
      }
    }
  }

  if ((groupId < 0) || (groupId >= L7_PROTECTED_PORT_MAX_GROUPS))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_CfgureValIsOutOfRange);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
  if(all == L7_TRUE)
  {
    cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Name_1);
    rc = usmdbProtectedPortGroupNameGet(unit,groupId,name);
    ewsTelnetPrintf (ewsContext, name);
    ewsTelnetPrintf (ewsContext, "\r\n%s :", pStrInfo_switching_MbrPorts);
    all = L7_FALSE;
  }

  cliSyntaxBottom (ewsContext);
  ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
  memset(stat, 0, L7_CLI_MAX_STRING_LENGTH);
  memset(buf, 0, L7_CLI_MAX_STRING_LENGTH);
  Linecount = 0;

  while(Linecount < CLI_MAX_SCROLL_LINES-6)
  {
    rc = usmdbProtectedPortGroupIntfGetNext(unit,groupId, i,
                                            &nextgroupId, &nextintIfNum);
    if(rc != L7_SUCCESS)
    {
      i = 0;
      all = L7_TRUE;
      groupId = 0;
      count = 0;
      ewsTelnetWrite(ewsContext,buf);
      memset(buf, 0, L7_CLI_MAX_STRING_LENGTH);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
    rc1 = usmDbUnitSlotPortGet(nextintIfNum, &unitNum, &slot, &port);
    if (rc1 == L7_SUCCESS)
    {
      if (groupId == nextgroupId)
      {
        count++;
        if(count > 1)
        {
          commaflag = L7_TRUE;
        }
        flag = L7_FALSE;
        osapiSnprintf(stat, sizeof(stat),cliDisplayInterfaceHelp(unitNum, slot, port));
        if(commaflag == L7_TRUE)
        {
          OSAPI_STRNCAT(buf, ", ");
        }

        if((strlen(buf) + strlen(stat)) < 60)
        {
          OSAPI_STRNCAT(buf, stat);
          memset(stat, 0, L7_CLI_MAX_STRING_LENGTH);
        }
        else
        {
          flag = L7_TRUE;
        }
      }
      if(flag == L7_TRUE)
      {
        ewsTelnetWrite(ewsContext,buf);
        memset(buf, 0, L7_CLI_MAX_STRING_LENGTH);
        OSAPI_STRNCAT(buf, stat);
        memset(stat, 0, L7_CLI_MAX_STRING_LENGTH);
        ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
        flag = L7_FALSE;
        Linecount++;
      }
    }
    i = nextintIfNum;
  }
  cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
  memset(cmdBuf, 0, L7_CLI_MAX_STRING_LENGTH);
  if (cliNumFunctionArgsGet() == 0)
  {
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_switching_ShowSwitchportProtected);
  }
  else
  {
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), pStrInfo_switching_ShowSwitchportProtected_1, groupId);
  }

  cliAlternateCommandSet(cmdBuf);
  return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
}

/*********************************************************************
*
* @purpose  Display switch port protection information for configured interfaces
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  show interfaces switchport <slot/port> <groupid>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *
commandShowInterfacesSwitchPort(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uchar8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strGroupId[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_uint32 unit, slot, port,interface;
  L7_uint32 argSlotPort = 1;
  L7_uint32 argGroupId = 2;
  L7_uint32 numArg;
  L7_uint32 nextgroupId;
  L7_uint32 groupId = 0;
  L7_int32 retVal ;
  L7_uint32 itype;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop (ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId ();

  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet ();
  if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                               L7_PROTECTED_PORT_MAX_GROUPS) == L7_TRUE )
  {
    if (numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowProtectedPortIntf, cliSyntaxInterfaceHelp());
    }
  }
  else
  {
    if ((numArg != 1))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowProtectedPortIntfNoArgs, cliSyntaxInterfaceHelp());
    }
  }

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported () == L7_TRUE)
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
    unit = cliGetUnitId ();
    if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &slot, &port,
                            &interface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  unit = cliGetUnitId ();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ((rc = usmDbVisibleInterfaceCheck(unit, interface, &retVal)) != L7_SUCCESS)
  {
    if (retVal == -2)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrErr_common_InvalidLag,
                              cliDisplayInterfaceHelp(unit, slot, port));
    }
    else if (retVal == -1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else if (usmDbIntfTypeGet(interface, &itype) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
  }

  if(numArg == 2)
  {
    OSAPI_STRNCPY_SAFE(strGroupId, argv[index + argGroupId]);
    if (cliCheckIfInteger (strGroupId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }
    groupId = atoi (strGroupId);
  }
  else
  {
    groupId = 0;
  }

  if ((groupId < 0) || (groupId >= L7_PROTECTED_PORT_MAX_GROUPS))
  {
    cliFormatAddBlanks (0, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_CfgureValIsOutOfRange);
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  rc = usmdbProtectedPortGroupIntfGet(unit,interface, &nextgroupId);
  if(rc != L7_SUCCESS)
  {
    cliFormatAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_ProtectedPort);
    return cliSyntaxReturnPrompt (ewsContext,"%5s ", pStrInfo_common_False2);
  }

  cliFormatAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Name_1);
  rc = usmdbProtectedPortGroupNameGet(unit,groupId,buf);
  ewsTelnetPrintf (ewsContext, buf);
  cliFormatAddBlanks (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_switching_ProtectedPort);
  if(groupId == nextgroupId)
  {
    if (usmDbFeaturePresentCheck(unit, L7_PROTECTED_PORT_COMPONENT_ID,
                                 L7_PROTECTED_PORT_MAX_GROUPS) != L7_TRUE )
    {
      ewsTelnetPrintf (ewsContext, "%5s ", pStrInfo_common_True2);
    }
    else
    {
      ewsTelnetPrintf (ewsContext, "%5s ", pStrInfo_common_True2);
      memset(stat, 0, L7_CLI_MAX_STRING_LENGTH);
      ewsTelnetWrite(ewsContext,stat);
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_InGrp, groupId);
      ewsTelnetWrite(ewsContext,stat);
    }
  }
  else
  {
    ewsTelnetPrintf (ewsContext, "%5s ", pStrInfo_common_False2);
  }
  return cliSyntaxReturnPrompt (ewsContext, "");
}

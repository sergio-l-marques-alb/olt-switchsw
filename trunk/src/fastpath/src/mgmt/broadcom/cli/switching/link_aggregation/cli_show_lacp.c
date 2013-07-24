/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_lacp.c
 *
 * @purpose LACP show commands for CLI
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  10/10/2005
 *
 * @author  Sunil Babu
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>
#include "clicommands_card.h"
#include "clicommands_lacp.h"

/*********************************************************************
*
* @purpose  Displaying LACP actor Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lacp actor {u/s/p | all }
*
* @cmdhelp    Display the current LACP actor configuration summary
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandLACPActorShow(EwsContext ewsContext,
                                     L7_uint32 argc,
                                     const L7_char8 * * argv,
                                     L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 currentIntIfNum;
  static L7_uint32 currentExtIfNum;
  L7_uint32 nextInterface;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uchar8 strLogicalSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argSlotPort=1;
  L7_BOOL all = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 itype;

  L7_uint32 priorityValue;
  L7_uint32 keyValue;
  L7_uchar8 adminState;

  L7_uint32 count;

  L7_BOOL showHead;
  showHead = L7_TRUE;

  /* validity checking */
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLacpActor,
                            cliSyntaxLogInterfaceHelp());
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      currentExtIfNum = 0;
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    all = L7_TRUE;
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      rc = usmDbValidIntIfNumFirstGet(&currentExtIfNum);
      if (rc != L7_SUCCESS)
      {
        currentExtIfNum = 0;
        return cliPrompt(ewsContext);
      }
    }
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit,
                                         &logicalSlot, &logicalPort)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit,
                        logicalSlot, logicalPort);
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
      rc = usmDbIntIfNumFromUSPGet(unit, logicalSlot,
                                   logicalPort, &currentIntIfNum);
      if (rc != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u",
                      unit, logicalSlot, logicalPort);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      rc = cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort],
                               &logicalSlot, &logicalPort, &currentIntIfNum);
      if (rc != L7_SUCCESS)
      {
        currentExtIfNum = 0;
        return cliPrompt(ewsContext);
      }
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {

    if(L7_TRUE == cliIsPromptRespQuit())
    {
      currentExtIfNum = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(),argv);
      return cliPrompt(ewsContext);
    }
  }

  if (!all)
  {
    if ( usmDbIntfTypeGet(currentIntIfNum, &itype) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if(itype != L7_PHYSICAL_INTF )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
    }
  }

  /* count is set 3 to count the heading of 3 lines */
  for (count=3; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    if(all)
    {
      rc = usmDbIntIfNumFromExtIfNum(currentExtIfNum, &currentIntIfNum);
      if (rc != L7_SUCCESS)
      {
        currentExtIfNum = 0;
        return cliPrompt(ewsContext);
      }
      if (usmDbIntfTypeGet(currentIntIfNum, &itype) != L7_SUCCESS)
      {

        currentExtIfNum = 0;
        return cliPrompt(ewsContext);
      }
    }

    if (itype == L7_PHYSICAL_INTF )
    {
      if(showHead == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 9, 0, L7_NULLPTR, ewsContext,pStrInfo_base_SysAdminPortAdmin);
        ewsTelnetWriteAddBlanks (1, 0, 1, 2, L7_NULLPTR, ewsContext,pStrInfo_base_IntfPriKeyPriState);
        ewsTelnetWrite(ewsContext,"\r\n------ -------- ----- -------- ----------- ");
        showHead = L7_FALSE;
      }

      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
      if (usmDbUnitSlotPortGet(currentIntIfNum, &unit,
                               &logicalSlot, &logicalPort) == L7_SUCCESS)
      {
        osapiSnprintf(strLogicalSlotPort,sizeof(strLogicalSlotPort),
                      cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
        ewsTelnetPrintf (ewsContext, "%-7.6s", strLogicalSlotPort);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-7.6s", pStrInfo_common_EmptyString);
      }

      /* system priority */
      priorityValue = 0;

      if( usmDbDot3adAggPortActorSystemPriorityGet(
           unit, currentIntIfNum, &priorityValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",priorityValue);
        ewsTelnetPrintf (ewsContext, "%-9.8s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-9s", pStrInfo_common_EmptyString);
      }

      /* admin key  */
      keyValue = 0;
      if( usmDbDot3adAggPortActorAdminKeyGet(
           unit, currentIntIfNum, &keyValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",keyValue);
        ewsTelnetPrintf (ewsContext, "%-6.5s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-10s", pStrInfo_common_EmptyString);
      }

      /* port priority  */
      priorityValue = 0;

      if( usmDbDot3adAggPortActorPortPriorityGet(
           unit, currentIntIfNum, &priorityValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",priorityValue);
        ewsTelnetPrintf (ewsContext, "%-9.8s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-9s", pStrInfo_common_EmptyString);
      }

      /* admin state */
      adminState = 0;
      memset(buf, 0, sizeof(buf));
      if( usmDbDot3adAggPortActorAdminStateGet(
           unit, currentIntIfNum, &adminState) == L7_SUCCESS)
      {
        if((adminState & DOT3AD_STATE_LACP_ACTIVITY)
           == DOT3AD_STATE_LACP_ACTIVITY )
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_base_LacpActiveShort);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf),pStrInfo_base_LacpPass);
        }

        if((adminState & DOT3AD_STATE_AGGREGATION)
           == DOT3AD_STATE_AGGREGATION )
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpAggregateShort);
        }
        else
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpIn);
        }

        if((adminState & DOT3AD_STATE_LACP_TIMEOUT)
           == DOT3AD_STATE_LACP_TIMEOUT )
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpShortTimeoutShort);
        }
        else
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpLongTimeoutShort);
        }
        ewsTelnetPrintf (ewsContext, "%-12.11s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-12s", pStrInfo_common_EmptyString);
      }

      if (!all)
      {
        currentExtIfNum = 0;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      else
      {
        if (usmDbValidIntIfNumNext(currentExtIfNum, &nextInterface)
            == L7_SUCCESS)
        {
          currentExtIfNum = nextInterface;
        }
        else
        {
          currentExtIfNum = 0;
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
    else
    {
      if (usmDbValidIntIfNumNext(currentExtIfNum, &nextInterface)
          == L7_SUCCESS)
      {
        currentExtIfNum = nextInterface;
      }
      else
      {
        currentExtIfNum = 0;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      count--;
    }
  } /* end of for  */

  if (currentExtIfNum != 0 )
  {
    cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowLacpActorAll);
    return pStrInfo_common_Name_2;
  }
  else
  {
    currentExtIfNum = 0;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

}

/*********************************************************************
*
* @purpose  Displaying LACP partner Information
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8 *
* @returns    cliPrompt(ewsContext)
*
* @notes    None
*
* @cmdsyntax  show lacp partner {u/s/p | all }
*
* @cmdhelp    Display the current LACP partner configuration summary
*
* @cmddescript
*
* @end
*********************************************************************/

const L7_char8 *commandLACPPartnerShow(EwsContext ewsContext,
                                       L7_uint32 argc,
                                       const L7_char8 * * argv,
                                       L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 currentIntIfNum;
  static L7_uint32 currentExtIntfNum;
  L7_uint32 nextInterface;
  L7_uchar8 buf[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 logicalSlot;
  L7_uint32 logicalPort;
  L7_uchar8 strLogicalSlotPort[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 argSlotPort=1;
  L7_BOOL all = L7_FALSE;
  L7_uint32 unit;
  L7_uint32 numArg;

  L7_uint32 itype;

  L7_uint32 keyValue;
  L7_uint32 priorityValue;
  L7_uchar8 adminState;
  L7_uchar8 sysId[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 count;

  L7_BOOL showHead;
  showHead = L7_TRUE;

  /* validity checking */
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet( L7_FALSE);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_ShowLacpPartner,
                            cliSyntaxLogInterfaceHelp());
  }

  if (strcmp(argv[index+argSlotPort], pStrInfo_common_All) == 0)
  {
    /* get switch ID based on presence/absence of STACKING package */
    unit = cliGetUnitId();
    if (unit == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    }

    all = L7_TRUE;
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      rc = usmDbValidIntIfNumFirstGet(&currentExtIntfNum);
      if (rc != L7_SUCCESS)
      {
        currentExtIntfNum = 0;
        return cliPrompt(ewsContext);
      }
    }
  }
  else
  {
    if (cliIsStackingSupported() == L7_TRUE)
    {
      if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit,
                                         &logicalSlot, &logicalPort)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
          ewsTelnetPrintf (ewsContext, "%u/%u/%u", unit,
                        logicalSlot, logicalPort);
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
      rc = usmDbIntIfNumFromUSPGet(unit, logicalSlot,
                                   logicalPort, &currentIntIfNum);
      if (rc != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit,
                      logicalSlot, logicalPort);
      }
    }
    else
    {
      /* NOTE: No need to check the value of `unit` as
       *       ID of a standalone switch is always `U_IDX` (=> 1).
       */
      unit = cliGetUnitId();
      rc = cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort],
                               &logicalSlot, &logicalPort, &currentIntIfNum);
      if (rc != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    if(L7_TRUE == cliIsPromptRespQuit())
    {

      currentExtIntfNum = 0;
      ewsCliDepth(ewsContext, cliPrevDepthGet(),argv);
      return cliPrompt(ewsContext);
    }
  }

  if (!all)
  {
    if( usmDbIntfTypeGet(currentIntIfNum, &itype) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
    if(itype != L7_PHYSICAL_INTF )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR, ewsContext,pStrErr_base_PhyIntfNotValidIntf);
    }
  }

  /*count is initializex to 3 to include the 3 lines for header*/
  for (count=3; count < CLI_MAX_SCROLL_LINES-6; count++)
  {
    if(all)
    {
      if(usmDbIntIfNumFromExtIfNum(currentExtIntfNum,
                                   &currentIntIfNum) != L7_SUCCESS)
      {
        currentExtIntfNum = 0;
        return cliPrompt(ewsContext);
      }
      if(usmDbIntfTypeGet(currentIntIfNum, &itype) != L7_SUCCESS)
      {
        currentExtIntfNum = 0;
        return cliPrompt(ewsContext);
      }
    }

    if (itype == L7_PHYSICAL_INTF )
    {
      if(showHead == L7_TRUE)
      {
        ewsTelnetWriteAddBlanks (1, 0, 7, 0, L7_NULLPTR, ewsContext,pStrInfo_base_SysSysAdminPrtPrtAdmin);
        ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext,pStrInfo_base_IntfPriIdKeyPriIdState);
        ewsTelnetWrite(ewsContext,"\r\n------ ----- ----------------- ----- ----- ----- ----------- ");
        showHead = L7_FALSE;
      }

      ewsTelnetWrite(ewsContext,pStrInfo_common_CrLf);
      if (usmDbUnitSlotPortGet(currentIntIfNum, &unit, &logicalSlot,
                               &logicalPort) == L7_SUCCESS)
      {
        osapiSnprintf(strLogicalSlotPort,sizeof(strLogicalSlotPort),
                      cliDisplayInterfaceHelp(unit, logicalSlot, logicalPort));
        ewsTelnetPrintf (ewsContext, "%-7.6s", strLogicalSlotPort);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-7s", pStrInfo_common_EmptyString);

      }

      /* system priority  */
      priorityValue = 0;
      if( usmDbDot3adAggPortPartnerAdminSystemPriorityGet(unit,
                                                          currentIntIfNum, &priorityValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",priorityValue);
        ewsTelnetPrintf (ewsContext, "%-6.5s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-6s", pStrInfo_common_EmptyString);
      }

      /*sys-ID  */
      memset(sysId, 0, sizeof(sysId));
      if( usmDbDot3adAggPortPartnerAdminSystemIDGet(unit,
                                                    currentIntIfNum, sysId) == L7_SUCCESS)
      {
        /* Convert sysId to string format and then display */
        osapiSnprintf(buf, sizeof(buf),"%02X:%02X:%02X:%02X:%02X:%02X",
                      sysId[0], sysId[1], sysId[2], sysId[3], sysId[4], sysId[5]);
        ewsTelnetPrintf (ewsContext, "%-18.17s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-18s", pStrInfo_common_EmptyString);
      }

      /* admin key   */
      keyValue = 0;
      if( usmDbDot3adAggPortPartnerAdminKeyGet(unit, currentIntIfNum,
                                               &keyValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",keyValue);
        ewsTelnetPrintf (ewsContext, "%-6.5s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-6s", pStrInfo_common_EmptyString);
      }

      /* port priority */
      priorityValue = 0;
      if( usmDbDot3adAggPortPartnerAdminPortPriorityGet(unit,
                                                        currentIntIfNum, &priorityValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",priorityValue);
        ewsTelnetPrintf (ewsContext, "%-6.5s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-6s", pStrInfo_common_EmptyString);
      }

      /* port-id  */
      priorityValue = 0;
      if( usmDbDot3adAggPortPartnerAdminPortGet(unit,
                                                currentIntIfNum, &priorityValue) == L7_SUCCESS)
      {
        osapiSnprintf(buf, sizeof(buf), "%d",priorityValue);
        ewsTelnetPrintf (ewsContext, "%-6.5s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-6s", pStrInfo_common_EmptyString);
      }

      /* Admin-State */
      adminState = 0;
      memset(buf, 0, sizeof(buf));
      if( usmDbDot3adAggPortPartnerAdminStateGet(unit,
                                                 currentIntIfNum, &adminState) == L7_SUCCESS)
      {
        if((adminState & DOT3AD_STATE_LACP_ACTIVITY)
           == DOT3AD_STATE_LACP_ACTIVITY )
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_base_LacpActiveShort);
        }
        else
        {
          osapiSnprintf(buf, sizeof(buf), pStrInfo_base_LacpPass);
        }

        if((adminState & DOT3AD_STATE_AGGREGATION)
           == DOT3AD_STATE_AGGREGATION )
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpAggregateShort);
        }
        else
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpIn);
        }

        if((adminState & DOT3AD_STATE_LACP_TIMEOUT)
           == DOT3AD_STATE_LACP_TIMEOUT )
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpShortTimeoutShort);
        }
        else
        {
          OSAPI_STRNCAT(buf,"|");
          OSAPI_STRNCAT(buf, pStrInfo_base_LacpLongTimeoutShort);
        }
        ewsTelnetPrintf (ewsContext, "%-12.11s", buf);
      }
      else
      {
        ewsTelnetPrintf (ewsContext, "%-12s", pStrInfo_common_EmptyString);
      }

      if (!all)
      {
        currentExtIntfNum = 0;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      else
      {
        if (usmDbValidIntIfNumNext(currentExtIntfNum,
                                   &nextInterface) == L7_SUCCESS)
        {
          currentExtIntfNum = nextInterface;
        }
        else
        {
          currentExtIntfNum = 0;
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
      }
    }
    else
    {
      if (usmDbValidIntIfNumNext(currentExtIntfNum,
                                 &nextInterface) == L7_SUCCESS)
      {
        currentExtIntfNum = nextInterface;
      }
      else
      {
        currentExtIntfNum = 0;
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      count--; /*since its not a valid physical interface */
    }
  } /* end of for  */

  if (currentExtIntfNum != 0 )
  {
    cliSetCharInputID(CLI_INPUT_NOECHO,ewsContext,argv);
    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(pStrInfo_base_ShowLacpPartnerAll);
    return pStrInfo_common_Name_2;
  }
  else
  {
    currentExtIntfNum = 0;
    return cliSyntaxReturnPrompt (ewsContext, "");
  }
}

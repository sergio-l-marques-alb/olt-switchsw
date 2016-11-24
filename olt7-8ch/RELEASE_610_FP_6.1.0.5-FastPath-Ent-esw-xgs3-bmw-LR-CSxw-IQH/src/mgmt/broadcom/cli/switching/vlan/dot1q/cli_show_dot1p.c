/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_dot1p.c
 *
 * @purpose Dot1p show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  09/16/2003
 *
 * @author  Jason Shaw
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_dot1p.h"
#include "usmdb_counters_api.h"
#include "clicommands_card.h"

/*********************************************************************
*
* @purpose Displays the 802.1p Priority Mapping
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
* @cmdsyntax in stacking env: show classofservice dot1p-mapping [unit/slot/port]
* @cmdsyntax in non-stacking env: show classofservice dot1p-mapping [slot/port]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const char *commandShowClassofServiceDot1pmapping(EwsContext ewsContext, uintf argc, const char * * argv, uintf index)
{
  L7_uint32 priority;
  L7_uint32 queue;
  L7_uint32 unit, interface, rc;
  L7_uint32 argSlotPort=1;
  L7_uint32 intSlot;
  L7_uint32 intPort;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /* determine if displaying interface config or global config parms */
  if (cliNumFunctionArgsGet() == 0)
  {
    /* displaying global config values */
    interface = L7_ALL_INTERFACES;      /* use this for all dot1p USMDB calls */
  }
  else
  {
    /* displaying interface config values (but only if intf feature is supported) */
    if (usmDbFeaturePresentCheck(unit, L7_DOT1P_COMPONENT_ID, L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID) != L7_TRUE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_switching_ShowCosDot1pMapPing);
    }

    if (cliNumFunctionArgsGet() != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_switching_ShowCosDot1pMapPingPort, cliSyntaxInterfaceHelp());
    }

    if (cliIsStackingSupported() == L7_TRUE)
    {
      rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &intSlot, &intPort);

      if (rc == L7_FAILURE)
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 1, pStrErr_common_Error,ewsContext, pStrErr_common_InvalidIntfNonSpecific, cliSyntaxInterfaceHelp());
      }
      else if (rc == L7_ERROR || usmDbIntIfNumFromUSPGet(unit, intSlot, intPort, &interface) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        return cliSyntaxReturnPrompt (ewsContext,"%u/%u/%u", unit, intSlot, intPort);
      }
    }
    else
    {
      if (cliSlotPortToIntNum(ewsContext, argv[index+argSlotPort], &intSlot, &intPort, &interface) != L7_SUCCESS)
      {
        return cliPrompt(ewsContext);
      }
    }
  }

  ewsTelnetWriteAddBlanks (1, 0, 1, 0, L7_NULLPTR, ewsContext, pStrInfo_switching_UsrPriTrafficClass);
  ewsTelnetWrite(ewsContext, "\r\n -------------    -------------");

  priority = L7_DOT1P_MIN_PRIORITY;
  while ((usmDbDot1dTrafficClassGet(unit, interface, priority, &queue) == L7_SUCCESS) && (priority <= L7_DOT1P_MAX_PRIORITY) )
  {

    ewsTelnetPrintf (ewsContext, "\r\n%8d %16d", priority, queue);
    priority++;
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

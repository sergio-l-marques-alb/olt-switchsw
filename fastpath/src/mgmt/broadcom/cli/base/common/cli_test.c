/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_test.c
 *
 * @purpose cli test commands
 *
 * @component user interface
 *
 * @create  09/19/2002
 *
 * @author  Jill Flanagan
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "clicommands_card.h"
#include "usmdb_status.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"

/*********************************************************************
*
* @purpose  Test the status of a copper cable
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8 *
* @returns NULL
*
* @notes Fails (L7_ERROR) if requested for a non-copper cable.
*
* @cmdsyntax in stacking env:  cablestatus <unit/slot/port>
* @cmdsyntax in non-stacking env:  cablestatus <slot/port>
*
* @cmdhelp  Test the cable attached to an interface.
*
* @cmddescript
*   This command will test the cable connected to the specified
*   interface.  The cable status, estimated cable length, and
*   estimated failure location will be displayed.
*
* @end
*
*********************************************************************/
const L7_char8 *commandCablestatus(EwsContext ewsContext, L7_uint32 argc,
                                   const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSlotPort = 1;
  L7_uint32 iface, slot, port;
  L7_uint32 rc;
  L7_uint32 cableStatus;
  L7_BOOL lengthKnown;
  L7_uint32 shortestLength;
  L7_uint32 longestLength;
  L7_uint32 cableFailureLength;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 connType;
  L7_uint32 unit;
  L7_clocktime timestamp;

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);
  if ( cliNumFunctionArgsGet() != 1)       /* parameter check */
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_base_TestCableStatus_1, cliSyntaxInterfaceHelp());
  }

  if ((rc = cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &slot, &port)) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }
  if (usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /* Verify the cable is copper */
  if( ( usmDbIntfConnectorTypeGet(iface, &connType) != L7_SUCCESS ) ||
     ( connType != L7_RJ45 ) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidCableType);
  }

  rc = usmDbStatusCableGet(iface, &cableStatus,&lengthKnown, &shortestLength,
                           &longestLength, &cableFailureLength, &timestamp );

  if( rc == L7_NOT_SUPPORTED )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_InvalidCableType);
  }

  if ((rc == L7_FAILURE)  || (rc == L7_ERROR ) || (cableStatus == L7_CABLE_TEST_FAIL) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_base_TestCableStatusFail);
  }

  /* Format cable status */

  memset ( stat, 0, sizeof(stat));
  switch (cableStatus)
  {
  case L7_CABLE_NORMAL:
    osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Normal);
    break;
  case L7_CABLE_OPEN:
    osapiSnprintf(stat, sizeof(stat), pStrInfo_base_Open);
    break;
  case L7_CABLE_SHORT:
    osapiSnprintf(stat, sizeof(stat), pStrInfo_base_Short);
    break;
  case L7_CABLE_NOCABLE:
    osapiSnprintf(stat, sizeof(stat), pStrInfo_base_NoCable);
    break;        
  default:
    /* Should never reach this point. */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_TestCableStatusUnknown);
    /*PASSTHRU*/
  }

  cliFormat(ewsContext,pStrInfo_base_CableStatus);          /*   Cable Status */
  ewsTelnetWrite(ewsContext, stat);

  /* Format cable length */

  memset ( stat, 0, sizeof(stat));
  if (cableStatus == L7_CABLE_NORMAL)
  {
    if (lengthKnown == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_base_MM_1, shortestLength, longestLength);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Unknown_1);
    }

    cliFormat(ewsContext, pStrInfo_base_CableLen);          /*   Cable Length */
    ewsTelnetWrite(ewsContext, stat);
  }

  /* Format failure location */

  if ( (cableStatus == L7_CABLE_OPEN ) || (cableStatus == L7_CABLE_SHORT))
  {
    memset ( stat, 0, sizeof(stat) );
    if (lengthKnown == L7_TRUE)
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_base_M_1, cableFailureLength);
    }
    else
    {
      osapiSnprintf(stat, sizeof(stat), pStrInfo_common_Unknown_1);
    }

    cliFormat(ewsContext, pStrErr_base_FailureLocation);          /*   Failure Location */
    ewsTelnetWrite(ewsContext, stat);
  }

  return cliSyntaxReturnPrompt (ewsContext, "");
}

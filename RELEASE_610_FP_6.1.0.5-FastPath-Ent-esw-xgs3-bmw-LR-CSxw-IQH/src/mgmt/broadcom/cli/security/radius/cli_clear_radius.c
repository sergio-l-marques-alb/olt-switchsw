/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/radius/cli_clear_radius.c
 *
 * @purpose Radius clear commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  5/9/2003
 *
 * @author  kmans
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"
#include "usmdb_radius_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_radius.h"


/*********************************************************************
*
* @purpose  clear radius statistics
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes add range checking
*
* @cmdsyntax  clear radius statistic
*
* @cmdhelp
*
* @cmddescript
*
*********************************************************************/

const L7_char8 *commandClearRadiusStatistics(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  if ( cliNumFunctionArgsGet() != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_ClrRadiusStatsCheck);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if ( cliGetCharInputID() != CLI_INPUT_EMPTY || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
  {
    if ( tolower(cliGetCharInput()) == 'y' || ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbRadiusStatsClear(unit) == L7_SUCCESS )
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,  pStrInfo_security_RadiusStatsClred);
        }                                                                           /* Statistics Cleared!  */
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_ClrRadiusStats);
        }                                                               /* Error */
      }
    }
    else   /* no */
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext,  pStrErr_security_RadiusStatsNotClred);  /* Statistics Not Cleared!" */
    }

    ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

    return cliPrompt(ewsContext);
  }
  else
  {
    cliSetCharInputID(1, ewsContext, argv);

    cliAlternateCommandSet(pStrInfo_security_ClrRadiusStats_1);   /*  clear radius stats */
    return pStrErr_security_ConfirmClrAllRadiusStats;    /* Are you sure you want to clear the radius statistics? (y/n) */
  }
}

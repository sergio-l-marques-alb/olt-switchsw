/*********************************************************************
 * LL   VV  VV LL   7   77   All Rights Reserved.
 * LL   VV  VV LL      77
 * LL    VVVV  LL     77
 * LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
 * </pre>
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/ospf/cli_clear_ospf.c
 *
 * @purpose config commands for the OSPF
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/5/2007
 *
 * @author  Rob Rice
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"

#include "cliapi.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "usmdb_ospf_api.h"
#include "usmdb_util_api.h"

#include "ews.h"
#include "clicommands_card.h"



/*********************************************************************
*
* @purpose    Reset various things in OSPF.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax for normal command: clear ip ospf { configuration | redistribution | counters | neighbor [ interface <usp> ] [ <neighbor-id> ] }
*
* @cmdsyntax for no command: no command does not exist
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIpOspf(EwsContext ewsContext, L7_uint32 argc, 
                                   const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uchar8 *syntax = "Use 'clear ip ospf { configuration | redistribution | counters | neighbor [ interface <usp> ] [ <neighbor-id> ] }'.";

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, 
                                           ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();
  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, syntax);
  }

  /* clear ip ospf. Disable and reenable. */
  usmDbOspfAdminModeSet(unit, L7_DISABLE);
  usmDbOspfAdminModeSet(unit, L7_ENABLE);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}



/*********************************************************************
*
* @purpose    Restore OSPF configuration to factory defaults.
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax for normal command: clear ip ospf configuration
*
* @cmdsyntax for no command: no command does not exist
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIpOspfConfiguration(EwsContext ewsContext, L7_uint32 argc, 
                                                const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uchar8 *syntax = "Use 'clear ip ospf configuration'.";

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, syntax);
  }

  usmDbOspfRestore(cliGetUnitId());
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose    Flush self originated external LSAs and re-apply 
*             route redistribution.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax for normal command: clear ip ospf configuration
*
* @cmdsyntax for no command: no command does not exist
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandClearIpOspfRedistribution(EwsContext ewsContext, L7_uint32 argc, 
                                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uchar8 *syntax = "Use 'clear ip ospf redistribution'.";
  L7_RC_t rc;

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, syntax);
  }
  rc = usmDbOspfMapSelfOrigExtLsaFlush();
  if (rc != L7_SUCCESS)
  {
    if (rc == L7_NOT_EXIST)
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, NULL, ewsContext, "OSPF is not running");
    else
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, NULL, ewsContext, "Failed to clear external routes");
  }
  return cliPrompt(ewsContext);
}


/*********************************************************************
* @purpose    Reset OSPF statistics.
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax for normal command: clear ip ospf counters
*
* @cmdsyntax for no command: no command does not exist
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandClearIpOspfCounters(EwsContext ewsContext, L7_uint32 argc, 
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg = cliNumFunctionArgsGet();
  L7_uchar8 *syntax = "Use 'clear ip ospf counters'.";

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, syntax);
  }

  if (usmDbOspfCountersClear() != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, NULL, ewsContext, "Failed to clear OSPF counters");
  }

  return cliPrompt(ewsContext);
}


/*********************************************************************
* @purpose    Bounce OSPF neighbors
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax for normal command: clear ip ospf neighbor [ interface <usp> ] [ <neighbor-id> ]
*
* @cmdsyntax for no command: no command does not exist
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandClearIpOspfNeighbor(EwsContext ewsContext, L7_uint32 argc, 
                                           const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArgs = cliNumFunctionArgsGet();
  L7_uchar8 *syntax = "Use 'clear ip ospf neighbor [ interface <usp> ] [ <neighbor-id> ]'.";
  L7_uint32 intIfNum = 0;      /* 0 indicates all interfaces */
  L7_char8 strRouterId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 routerId = 0;      /* 0 indicates all neighbors */
  L7_uint32 u, s, p;
  L7_uint32 pos = 1;
  L7_RC_t rc;

  if (numArgs > 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                           ewsContext, syntax);
  }

  if (numArgs > 0)
  {
    if (strcmp(argv[index + pos], "interface") == 0)
    {
      /* User has restricted clear to an interface */
      pos++;
      if (numArgs < pos)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, 
                                               ewsContext, syntax);
      }
      if (cliIsStackingSupported())
      {
        if (cliValidSpecificUSPCheck(argv[index + pos], &u, &s, &p) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }

        /* Get interface and check its validity */
        if (usmDbIntIfNumFromUSPGet(u, s, p, &intIfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }
      else
      {
        /* non-stacking */
        u = cliGetUnitId();
        if (cliSlotPortToIntNum(ewsContext, argv[index + pos], &s, &p, &intIfNum) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
        }
      }

      if (cliValidateRtrIntf(ewsContext, intIfNum) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                               pStrErr_common_InvalidSlotPort_1);
      }
      pos++;
    }

    /* clear restricted to a specific router ID? */
    if (numArgs == pos)
    {
      /* User has restricted clear to a specific neighbor */
      if (strlen(argv[index + pos]) >= sizeof(strRouterId) )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                               pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
      }
      OSAPI_STRNCPY_SAFE(strRouterId, argv[index + pos]);
      if (usmDbInetAton(strRouterId, &routerId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              "Invalid router ID");
      }
    }
  }

  rc = usmDbOspfMapNeighborClear(intIfNum, routerId);
  if (rc != L7_SUCCESS)
  {
    if (rc == L7_NOT_EXIST)
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, NULL, ewsContext, "OSPF is not running");
    else
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, NULL, ewsContext, "Failed to clear neighbors");
  }
  return cliPrompt(ewsContext);
}

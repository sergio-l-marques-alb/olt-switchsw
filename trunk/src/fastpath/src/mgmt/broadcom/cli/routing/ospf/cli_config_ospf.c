/*********************************************************************
 * LL   VV  VV LL   7   77   All Rights Reserved.
 * LL   VV  VV LL      77
 * LL    VVVV  LL     77
 * LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
 * </pre>
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/ospf/cli_config_ospf.c
 *
 * @purpose config commands for the OSPF
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/15/2001
 *
 * @author  Diogenes DeLosSantos
 * @author  Samip Garg
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include <errno.h>

/* For internet addr translation routines */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "l7_common.h"
#include "acl_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
#include "l7_relay_api.h"

#include "ews.h"

/* layer 3 includes           */
#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ospf_api.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospfv3_api.h"
#include "usmdb_mib_ospfv3_api.h"
#include "usmdb_ip6_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_rtrdisc_api.h"
#include "clicommands_l3.h"
#ifdef L7_IPV6_PACKAGE
#include "clicommands_ipv6.h"
#endif
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_relay_api.h"
#include "l3_commdefs.h"
#include "clicommands_card.h"
#include  "cli_config_script.h"

/*********************************************************************
*
* @purpose  Sets a 32-bit integer uniquely identifying the router
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
* @notes routerid is a valid ip address.
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: router-id <ipaddress>
*
* @cmdhelp
*
* @cmddescript  To ensure uniqueness, it defaults to the value of the
*               switch's management IP address.  If this value is
*               unconfigured, then the value of any active router
*               interface IP address is used.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRouterId (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argRouterId = 1;
  L7_uint32 intRouterId;
  L7_char8 strRouterId[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_uint32 ospfMode;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 currentRouterID;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !(numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgureRtrId);
  }

  /* convert the routerid ip addrs into a decimal number intIPaddr */
  if (strlen(argv[index+argRouterId]) >= sizeof(strRouterId))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidIpRtrId);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  OSAPI_STRNCPY_SAFE(strRouterId, argv[index + argRouterId]);
  if (  (usmDbInetAton(strRouterId, &intRouterId) == L7_SUCCESS) )
  {
    if (intRouterId == 0)
    {
      /* Setting router id to 0 is always invalid, regardless of prior setting. */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Invalid);
    }
    /* Perform OSPF admin check. First get current Router ID. */
    if (usmDbOspfRouterIdGet(unit, &currentRouterID) == L7_SUCCESS)
    {
      /* If non-zero, check admin state of OSPF. */
      if (currentRouterID && (currentRouterID != intRouterId))
      {
        rc = usmDbOspfAdminModeGet(unit, &ospfMode);
        if ((rc == L7_SUCCESS) && (ospfMode == L7_ENABLE))
        {
          /* Require OSPF admin state to be disabled.  */
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, 
                                                 "To change the OSPF router ID, you must first "
                                                 "globally disable OSPF using the 'no enable' command.");
        }
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfRouterIdSet( unit,  intRouterId) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetRtrId);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidIpRtrId);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable or disable the OSPF traps.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: trapflags ospf
*
* @cmdsyntax for no command: no trapflags ospf
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandTrapflagsOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 val;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 flagType=0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if( !(numArg>=1))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgTrapFlagsOspf_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbOspfAdminModeGet (unit, &val) == L7_SUCCESS)
  {
    if (val != L7_ENABLE)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_OspfMustBeEnbld);
    }
  }

  if ( numArg == 1 )
  {
    if(strcmp(argv[index+1],pStrInfo_common_All)== 0)
    {
      flagType=L7_OSPF_TRAP_ALL;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgTrapFlagsOspf);
    }
  }
  else if ( numArg == 2 && strcmp(argv[index+2],pStrInfo_common_All) == 0)
  {
    if ( strcmp(argv[index+1], pStrErr_common_OspfTrapErrs) == 0)
    {
      flagType=L7_OSPF_TRAP_ERRORS_ALL;
    }

    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapLsa) == 0)
    {
      flagType=L7_OSPF_TRAP_LSA_ALL;
    }

    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapReTx) == 0)
    {
      flagType=L7_OSPF_TRAP_RETRANSMIT_ALL;
    }

    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapOverflow) == 0)
    {
      flagType=L7_OSPF_TRAP_OVERFLOW_ALL;
    }

    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapStateChg) == 0)
    {
      flagType=L7_OSPF_TRAP_STATE_CHANGE_ALL;
    }
  }
  else if ( numArg == 2 )
  {
    if ( strcmp(argv[index+1], pStrErr_common_OspfTrapErrs) == 0)
    {
      if ( strcmp(argv[index+2], pStrErr_common_OspfTrapAuthFailure) == 0)
      {
        flagType=L7_OSPF_TRAP_IF_AUTH_FAILURE;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapBadPkt) == 0)
      {
        flagType=L7_OSPF_TRAP_RX_BAD_PACKET;
      }

      else if ( strcmp(argv[index+2], pStrErr_common_OspfTrapCfg) == 0)
      {
        flagType=L7_OSPF_TRAP_IF_CONFIG_ERROR;
      }

      else if ( strcmp(argv[index+2], pStrErr_common_OspfTrapVirtAuthFailure) == 0)
      {
        flagType=L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapVirtBadPkt) == 0)
      {
        flagType=L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET;
      }

      else if ( strcmp(argv[index+2], pStrErr_common_OspfTrapVirtCfg) == 0)
      {
        flagType=L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR;
      }
    }
    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapLsa) == 0)
    {
      if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapMaxAgeLsa) == 0)
      {
        flagType=L7_OSPF_TRAP_MAX_AGE_LSA;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapOriginateLsa) == 0)
      {
        flagType=L7_OSPF_TRAP_ORIGINATE_LSA;
      }
    }
    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapReTx) == 0)
    {
      if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapTxReTx) == 0)
      {
        flagType=L7_OSPF_TRAP_TX_RETRANSMIT;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapVirtIfTxReTx) == 0)
      {
        flagType=L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT;
      }
    }
    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapOverflow) == 0)
    {
      if ( strcmp(argv[index+2], pStrInfo_common_OspfTrap) == 0)
      {
        flagType=L7_OSPF_TRAP_LS_DB_OVERFLOW;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapDbAp) == 0)
      {
        flagType=L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW;
      }
    }
    else if ( strcmp(argv[index+1], pStrInfo_common_OspfTrapStateChg) == 0)
    {
      if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapIfStateChg) == 0)
      {
        flagType=L7_OSPF_TRAP_IF_STATE_CHANGE;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapNbrStateChg) == 0)
      {
        flagType=L7_OSPF_TRAP_NBR_STATE_CHANGE;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapVirtIfStateChg) == 0)
      {
        flagType=L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE;
      }

      else if ( strcmp(argv[index+2], pStrInfo_common_OspfTrapVirtNbrStateChg) == 0)
      {
        flagType=L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE;
      }
    }
  } /* endif numArg==2 */

  if( ewsContext->commType == CLI_NORMAL_CMD)

  {
    /*******Check if the Flag is Set for Execution*************/
    if ( ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfTrapModeSet(unit, L7_ENABLE, flagType) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantEnblTraps);
      }
    }
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfTrapModeSet(unit, L7_DISABLE, flagType) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDsblTraps);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgTrapFlagsOspf_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable or disable OSPF 1583 compatibility
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: 1583Compatibility
*
* @cmdsyntax for no command: no 1583Compatibility
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *command1583Compatibility (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 mode;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !(numArg == 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspf1583Compat);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if(  ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspf1583Compat);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbOspfRfc1583CompatibilitySet(unit, mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_Ospf1583Compat);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the administrative mode of OSPF to active or inactive
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: enable
*
* @cmdsyntax for no command: no enable
*
* @cmdhelp
*
* @cmddescript  enable = TRUE: OSPF++ is active on at least one interface;
*               disable = FALSE: OSPF++ is inactive on all interfaces;
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandEnableOspf (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];

  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if((numArg > 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg != 0 && (ewsContext->commType == CLI_NO_CMD)) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgWsEnbl_1);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( (ewsContext->commType == CLI_NORMAL_CMD) && (numArg == 0))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfAdminModeSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                              "Failed to enable OSPF");
        /*
           don't need to say anything here because it will always set to disable in our config files & the
           only reason this call would return failure is if the router id isn't set,  and
           we don't want to tell the user they could not disable it b/c of an invalid routerid.

         */
      }
    }
  }
  else if( (ewsContext->commType == CLI_NORMAL_CMD) && (numArg == 1))
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgWsEnbl_1);
    }

    OSAPI_STRNCPY_SAFE(strMode,argv[index + argMode]);
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_common_Lvl7Clr) == 0)
    {
      /*  lvl7clear is a method of clearing all ospf */
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbOspfRestore(unit) != L7_SUCCESS )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantClrOspf);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_Successful);
        }
      }
    }
  }

  else if(( ewsContext->commType == CLI_NO_CMD) && (numArg == 0))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfAdminModeSet(unit, L7_DISABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDsblOspf);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgWsEnbl_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Delete the specified area.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: cannot be used as a normal command
*
* @cmdsyntax for no command: no area <areaid>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaAreaId(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if(ewsContext->commType == CLI_NORMAL_CMD && argc == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfCantCreateArea);
  }
  else if(ewsContext->commType == CLI_NORMAL_CMD && argc > 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfQuestion);
  }

  if(argc != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoArea_1);
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CfgRtrOspfNoArea);
  }
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);

  unit = cliGetUnitId();
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
    {
      if( ewsContext->commType == CLI_NO_CMD)
      {
        rc = usmDbOspfAreaDelete(unit, intAreaid);
        switch(rc)
        {
        case L7_ERROR:
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CantDelAreaNotCfgured);

        case L7_FAILURE:
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CantDelAreaActiveIntf);

        default:
          break;
        }

      }
    }
    else
    {
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CfgRtrOspfNoArea);
    }
  }

  cliSyntaxBottom(ewsContext);
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> stub
*
* @cmdsyntax for no command: no area <areaid> stub
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaAreaIdStub(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 extRoutingCapability = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if( !(argc == 3 && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfStub);
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfStub);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(usmDbOspfVirtIfCheck(unit, intAreaid) == L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CantCreatesStubWithVlink);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfImportAsExternSet(unit, intAreaid, L7_OSPF_AREA_IMPORT_NO_EXT);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
                                                   ewsContext, pStrErr_common_BackboneCantBeStub);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,
                                                   ewsContext, pStrErr_common_CantCreatesStubArea);
          }
        }
      }
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfImportAsExternGet(unit, intAreaid, &extRoutingCapability);
        if (extRoutingCapability == L7_OSPF_AREA_IMPORT_NSSA)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDelsStubNssaId,intAreaid);
        }

        rc = usmDbOspfImportAsExternSet(unit, intAreaid, L7_OSPF_AREA_IMPORT_EXT);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDelsStubArea);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfStub);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose    Configures the preferences of OSPF Routes
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
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: distance ospf {intra-area<1-255> | inter-area<1-255> | external<1-255>}
*
* @cmdsyntax for no command: no distance ospf {intra-area | inter-area | external}
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDistanceOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argType = 1;
  L7_uint32 argPref = 2;
  L7_uint32 intPref, defintPref;
  L7_uint32 intType;
  L7_char8 strType[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_RC_t rc;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( !((numArg == 2 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 1 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfDistance);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoDistance);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (strlen(argv[index+argType]) >= sizeof(strType))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfDistance);
  }
  OSAPI_STRNCPY_SAFE(strType, argv[index + argType]);
  cliConvertToLowerCase(strType);
  if (strcmp(strType, pStrInfo_common_Intra) == 0)
  {
    intType = ROUTE_PREF_OSPF_INTRA_AREA;
    defintPref = L7_RTO_PREFERENCE_INTRA;
  }
  else if (strcmp(strType, pStrInfo_common_Inter) == 0)
  {
    intType = ROUTE_PREF_OSPF_INTER_AREA;
    defintPref = L7_RTO_PREFERENCE_INTER;
  }
  else if (strcmp(strType, pStrInfo_common_External) == 0)
  {
    intType = ROUTE_PREF_OSPF_EXTERNAL;
    defintPref = L7_RTO_PREFERENCE_EXTERNAL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfDistance);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argPref], &intPref) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfDistance);
    }

    if (intPref > L7_RTO_PREFERENCE_MAX || intPref < (L7_RTO_PREFERENCE_MIN))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfDistance);
    }

    else
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ((rc = usmDbIpRouterPreferenceSet(unit, intType, intPref)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetOspfPref);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ((rc = usmDbIpRouterPreferenceSet(unit, intType, defintPref)) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetOspfPref);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF Exit Overflow Interval
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  exit-overflow-interval <0-2147483647>
*
* @cmdsyntax for no command: no exit-overflow-interval
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandExitOverflowInterval (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeconds = 1;
  L7_uint32 seconds;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfExitOverflowIntvl_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoExitOverflowIntvl);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &seconds) == L7_SUCCESS)
    {
      if ( ( seconds >= L7_OSPF_EXIT_OVERFLOW_INT_MIN ) && ( seconds <= L7_OSPF_EXIT_OVERFLOW_INT_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbOspfExitOverflowIntervalSet(unit, seconds) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetExit);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_common_RangeExitOverflowIntvlValMustBeInRangeOfTo,
                                L7_OSPF_EXIT_OVERFLOW_INT_MIN, L7_OSPF_EXIT_OVERFLOW_INT_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfExitOverflowIntvl_1);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfExitOverflowIntervalSet(unit, L7_OSPF_EXIT_OVERFLOW_INT_DEF ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefExit);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF SPF delay inteval and OSPF SPF hold interval
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  timers spf <0-65535> <0-65535>
*
* @cmdsyntax for no command: no timers spf
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandOspfSpfTimers (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argDelay = 2;
  L7_uint32 argHoldTime = 3;
  L7_uint32 delayTime;
  L7_uint32 holdTime;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if( !((numArg == 3 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 1 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfTimersSpf);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfNotimersSpf);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argDelay],&delayTime) == L7_SUCCESS)
    {
      if ( ( delayTime >=L7_OSPF_SPF_DELAY_TIME_MIN ) && ( delayTime <=L7_OSPF_SPF_DELAY_TIME_MAX) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbOspfSpfDelaySet(unit, delayTime) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetSpfDelayInt);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeSpfDelayTimeValMustBeInRangeOfTo,
                                L7_OSPF_SPF_DELAY_TIME_MIN,L7_OSPF_SPF_DELAY_TIME_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfTimersSpf);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argHoldTime],&holdTime) == L7_SUCCESS)
    {
      if ( ( holdTime >=L7_OSPF_SPF_HOLD_TIME_MIN ) && ( holdTime<=L7_OSPF_SPF_HOLD_TIME_MAX) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbOspfSpfHoldtimeSet(unit, holdTime) != L7_SUCCESS )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetSpfHoldTimeInt);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeSpfHoldTimeValMustBeInRangeOfTo,
                                L7_OSPF_SPF_HOLD_TIME_MIN,L7_OSPF_SPF_HOLD_TIME_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfTimersSpf);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfSpfDelaySet(unit, L7_OSPF_SPF_DELAY_TIME_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefSpfDelayInt);
      }
    }
    if ( usmDbOspfSpfHoldtimeSet(unit, L7_OSPF_SPF_HOLD_TIME_DEF) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefSpfHoldTimeInt);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF default passive mode for interfaces
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  passive-interface { default | <%s> }
*
* @cmdsyntax for no command: no passive-interface { default | <%s> }
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandOspfPassive (EwsContext ewsContext, L7_uint32 argc,
                                    const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg, intfIndex = 1;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrOspfPass);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (strcmp(argv[index+intfIndex], pStrInfo_common_PassDefl) != 0)
  {
    return commandOspfPassiveInterface(ewsContext, argc, argv, index);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfPassiveModeSet(unit, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetPass);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    if((cliGetCharInputID() == CLI_INPUT_EMPTY) &&
       ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      cliSyntaxTop(ewsContext);
      cliSetCharInputID(1, ewsContext, argv);

      osapiSnprintf(buf, sizeof(buf), pStrInfo_common_NoPassiveIntfDefl);
      cliAlternateCommandSet(buf);

      return pStrInfo_common_VerifyChangingToDeflNonPass;
    }
    if((tolower(cliGetCharInput()) != 'y') &&
       ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPrompt(ewsContext, pStrInfo_common_PassNotChgd);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfPassiveModeSet(unit, L7_FALSE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetPass);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF passive mode for an interface
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  passive-interface <%s>
*
* @cmdsyntax for no command: no passive-interface <%s>
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandOspfPassiveInterface (EwsContext ewsContext, L7_uint32 argc,
                                             const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 iface, s, p;
  L7_uint32 argSlotPort = 1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  if (cliIsStackingSupported() == L7_TRUE)
  {
    if (cliValidSpecificUSPCheck(argv[index+argSlotPort], &unit, &s, &p) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidSlotPort_1);
    }

    /* Get interface and check its validity */
    if (usmDbIntIfNumFromUSPGet(unit, s, p, &iface) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  { /* NOTE: No need to check the value of `unit` as
     *       ID of a standalone switch is always `U_IDX` (=> 1).  */
    unit = cliGetUnitId();
    if (cliSlotPortToIntNum(ewsContext, argv[index + argSlotPort], &s, &p, &iface) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfIntfPassiveModeSet(unit, iface, L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIntfPass);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfIntfPassiveModeSet(unit, iface, L7_FALSE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIntfPass);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF Ext Lsdb Limit
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  external-lsdb-limit <-1-2147483647>
*
* @cmdsyntax for no command: no external-lsdb-limit
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandExternalLsdbLimit (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argLimit = 1;
  L7_int32 limit;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfExtLinkStateDbaseLimit_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoExtLinkStateDbaseLimit);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitSignedInteger(argv[index+argLimit], &limit) == L7_SUCCESS)
    {

      if ( ( limit >= L7_OSPF_EXT_LSDB_LIMIT_MIN ) && ( limit <= L7_OSPF_EXT_LSDB_LIMIT_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbOspfExtLsdbLimitSet(unit, limit) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetExtLinkStateDbase);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForExtLsdbLimitValMustBeInRangeOfTo,
                                L7_OSPF_EXT_LSDB_LIMIT_MIN, L7_OSPF_EXT_LSDB_LIMIT_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfExtLinkStateDbaseLimit_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfExtLsdbLimitSet(unit, L7_OSPF_EXT_LSDB_LIMIT_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefExtLinkStateDbase);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the maximum number of ECMP paths for OSPF.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  maximum-paths <1-<L7_RT_MAX_EQUAL_COST_ROUTES>>
*
* @cmdsyntax for no command: no maximum-paths
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandMaximumPaths (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 1;
  L7_int32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (!((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),  pStrErr_common_CfgRtrOspfMaxPaths,
                              platRtrRouteMaxEqualCostEntriesGet());
      ewsTelnetWrite(ewsContext, buf);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoMaxPaths);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitSignedInteger(argv[index+argVal], &val) == L7_SUCCESS)
    {

      if (( val >= 1 ) && ( val <= platRtrRouteMaxEqualCostEntriesGet() ))
      {
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbOspfMaxPathsSet(unit, val) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetMaxPaths);
            cliSyntaxBottom(ewsContext);
          }
        }
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),  pStrErr_common_CantSetOspfMaxPathsRange,
                                platRtrRouteMaxEqualCostEntriesGet());
        ewsTelnetWrite( ewsContext, stat);
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),  pStrErr_common_CfgRtrOspfMaxPaths,
                              platRtrRouteMaxEqualCostEntriesGet());
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfMaxPathsSet(unit, FD_OSPF_MAXIMUM_PATHS) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetMaxPaths);
        cliSyntaxBottom(ewsContext);
      }
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configure OSPF to advertise maximum metrics in its router LSAs.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  max-metric router-lsa [on-startup <seconds>] [summary-lsa {<metric>}]
*
* @cmdsyntax for no command:      no max-metric router-lsa [on-startup] [summary-lsa]
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandMaxMetric(EwsContext ewsContext, L7_uint32 argc, 
                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 pos = 1;     /* index of next argument to process */
  L7_uint32 duration = 0;
  L7_uint32 sumLsaMetric = FD_OSPF_STUB_ROUTER_SUMMARY_METRIC;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 numArg;
  L7_BOOL summaryOption = L7_FALSE;  /* L7_TRUE if normal cmd includes summary-lsa option */
  L7_BOOL onStartOption = L7_FALSE;  /* L7_TRUE if normal cmd includes on-startup option */
  OSPF_STUB_ROUTER_CFG_t currentCfg;
  OSPF_STUB_ROUTER_CFG_t stubRtrMode = OSPF_STUB_RTR_CFG_DISABLED;
  L7_uchar8 *syntax = "Use 'max-metric router-lsa [on-startup <seconds>] [summary-lsa {<metric>}]'.";
  L7_uchar8 *noSyntax = "Use 'no max-metric router-lsa [on-startup] [summary-lsa]'.";
  L7_uchar8 *failureMsg = "Failed to configure OSPF to advertise maximum metrics.";

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /* Validate number of arguments */
  if ((ewsContext->commType == CLI_NORMAL_CMD) && (numArg > 4))
  {
    osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), syntax);
    ewsTelnetWrite(ewsContext, buf);
    return cliPrompt(ewsContext);
  }

  if ((ewsContext->commType == CLI_NO_CMD) && (numArg > 2))
  {
    osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), noSyntax);
    ewsTelnetWrite(ewsContext, buf);
    return cliPrompt(ewsContext);
  }

  /* Get current stub router mode. */
  if (usmDbOspfStubRtrModeGet(&currentCfg, NULL, NULL, NULL) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, failureMsg);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg == 0)
    {
      if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
          (currentCfg == OSPF_STUB_RTR_CFG_STARTUP))
      {
        /* if current mode is max metrics on startup, "max-metric router-lsa" 
         * has no effect. Same as cisco. */
        return cliPrompt(ewsContext);
      }
      if (usmDbOspfStubRtrModeSet(OSPF_STUB_RTR_CFG_ALWAYS, 0) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, failureMsg);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      /* parse options*/
      while (pos <= numArg)
      {
        if (strcmp(argv[index + pos], "summary-lsa") == 0)
        {
          pos++;
          summaryOption = L7_TRUE;
        
          /* See if a metric is specified */
          if ((pos <= numArg) &&
              (cliConvertTo32BitUnsignedInteger(argv[index + pos], &sumLsaMetric) == L7_SUCCESS))
          {
            if ((sumLsaMetric < L7_OSPF_STUB_ROUTER_SUMLSA_MET_MIN) ||
                (sumLsaMetric > L7_OSPF_STUB_ROUTER_SUMLSA_MET_MAX))
            {
              ewsTelnetWrite(ewsContext, "Summary LSA metric is out of range.");
              cliSyntaxBottom(ewsContext);
              return cliPrompt(ewsContext);
            }
            pos++;
          }
          if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
              (usmDbOspfStubRtrSumLsaMetricOverride(L7_ENABLE, sumLsaMetric) != L7_SUCCESS))
          {
            ewsTelnetWrite(ewsContext, "Failed to configure OSPF to override summary LSA metric.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
        }
        else if (strcmp(argv[index + pos], "on-startup") == 0)
        {
          pos++;
          onStartOption = L7_TRUE;

          /* Startup duration is required */
          if (cliConvertTo32BitUnsignedInteger(argv[index + pos], &duration) != L7_SUCCESS)
          {
            osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, 
                                   buf, sizeof(buf), syntax);
            ewsTelnetWrite(ewsContext, buf);
            return cliPrompt(ewsContext);
          } 
          if ((duration < L7_OSPF_STUB_ROUTER_DURATION_MIN) || 
              (duration > L7_OSPF_STUB_ROUTER_DURATION_MAX))
          {
            ewsTelnetWrite(ewsContext, "Startup duration is out of range.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
          pos++;
        }
        else
        {
          osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, 
                                 buf, sizeof(buf), syntax);
          ewsTelnetWrite(ewsContext, buf);
          return cliPrompt(ewsContext);
        }
      }   /* while pos <= numArg */

      /* Now, based on current mode and options given, figure out new mode and set it. */
      if (currentCfg == OSPF_STUB_RTR_CFG_DISABLED)
      {
        if (onStartOption)
        {
          stubRtrMode = OSPF_STUB_RTR_CFG_STARTUP;
        }
        else
        {
          stubRtrMode = OSPF_STUB_RTR_CFG_ALWAYS;
        }
      }
      else if (((currentCfg == OSPF_STUB_RTR_CFG_ALWAYS) || 
                (currentCfg == OSPF_STUB_RTR_CFG_STARTUP)) && onStartOption)
      {
        /* if currently on start, then may need to update startup duration */
        stubRtrMode = OSPF_STUB_RTR_CFG_STARTUP;
      }   

      if ((stubRtrMode != OSPF_STUB_RTR_CFG_DISABLED) &&
          (usmDbOspfStubRtrModeSet(stubRtrMode, duration) != L7_SUCCESS))
      {
        ewsTelnetWrite(ewsContext, failureMsg);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }     /* end non-zero number of arguments */
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg == 0)
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbOspfStubRtrModeSet(OSPF_STUB_RTR_CFG_DISABLED, 0) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, "Failed to configure OSPF to advertise normal metrics");
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        /* Clear summary-lsa option */
        if (usmDbOspfStubRtrSumLsaMetricOverride(L7_DISABLE, 
                                                 FD_OSPF_STUB_ROUTER_SUMMARY_METRIC) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, 
                         "Failed to configure OSPF to advertise normal "
                         "metrics in summary LSAs.");
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
      }
    }
    else 
    {
      /* parse options */
      while (pos <= numArg)
      {
        if (strcmp(argv[index + pos], "on-startup") == 0)
        {
          pos++;
          if (currentCfg == OSPF_STUB_RTR_CFG_STARTUP)
          {
            /* Removes optional on-startup option. */
            if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
                (usmDbOspfStubRtrModeSet(OSPF_STUB_RTR_CFG_ALWAYS, 0) != L7_SUCCESS))
            {
              ewsTelnetWrite(ewsContext, failureMsg);
              cliSyntaxBottom(ewsContext);
              return cliPrompt(ewsContext);
            }
          }
        }
        else if (strcmp(argv[index + pos], "summary-lsa") == 0)
        {
          pos++;
          if ((ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT) &&
              (usmDbOspfStubRtrSumLsaMetricOverride(L7_DISABLE, 
                                                    FD_OSPF_STUB_ROUTER_SUMMARY_METRIC) != L7_SUCCESS))
          {
            ewsTelnetWrite(ewsContext, 
                           "Failed to configure OSPF to advertise normal "
                           "metrics in summary LSAs.");
            cliSyntaxBottom(ewsContext);
            return cliPrompt(ewsContext);
          }
        }
        else 
        {
          osapiSnprintfAddBlanks(1, 0, 0, 0, pStrErr_common_IncorrectInput, 
                                 buf, sizeof(buf), noSyntax);
          ewsTelnetWrite(ewsContext, buf);
          return cliPrompt(ewsContext);
        }
      }
    }
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF OpaqueCapability enable/disable
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  capability opaque
*
* @cmdsyntax for no command: no capability opaque
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandCapabilityOpaque (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_int32 val;
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (!( numArg == 1 ))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrOspfOpaqueCapability_1);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfNoOpaqueCapability);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    val = L7_ENABLE;
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfOpaqueLsaSupportSet(val) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetOpaqueCapability);
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrOspfOpaqueCapability_1);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    val = L7_DISABLE;
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfOpaqueLsaSupportSet(val) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetOpaqueCapability);
        cliSyntaxBottom(ewsContext);
      }
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF area to where the specified router interface
*             belongs.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf area <areaid> [secondaries none]
*
* @cmdsyntax for no command: no ip ospf area [secondaries none]
*
* @cmdhelp
*
* @cmddescript      The id denotes a 32-bit integer that uniquely identifies
*                   the area to which the interface connects.
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfAreaid (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argId = 1;
  L7_uint32 iface, s, p;
  L7_char8 strId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 temp[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg, secondaryIndex = 1, secondaryNoneIndex = 2;
  L7_BOOL flag = L7_FALSE;
  L7_uint32 areaID, i,areaIDLength;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if((ewsContext->commType == CLI_NORMAL_CMD) && ((numArg < 1) || (numArg > 3)))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfIntfAreaId_1);
  }
  else if((ewsContext->commType == CLI_NO_CMD) && (numArg > 2))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfIntfAreaIdNo);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argId]) >= sizeof(strId))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidAreaId);
    }
    OSAPI_STRNCPY_SAFE(strId, argv[index+argId]);

    /* This loop finds whether given router ID is in IP format or
     *     * decimal format */

    areaIDLength = strlen(strId);
    for(i=0; i < areaIDLength; i++)
    {
      if(strId[i] == '.')
      {
        flag = L7_TRUE;
      }

      if(flag == L7_TRUE)
      {
        break;
      }
    }

    /* If the router ID is in decimal format check boundry value */
    if(flag == L7_FALSE)
    {
      if (cliCheckIfInteger((L7_char8 *)argv[index+argId]) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidAreaId);
      }
      sscanf(argv[index+argId], "%u", &areaID);

      osapiSnprintf(temp, sizeof(temp), "%u",areaID);
      if(strcmp(temp,argv[index+argId])!= 0)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidAreaId);
      }

      if( (areaID < CLIOSPFAREAID_MIN) || (areaID > CLIOSPFAREAID_MAX) )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidAreaId);
      }
    }

    if ((flag == L7_FALSE) || (usmDbInetAton(strId, &areaID) == L7_SUCCESS))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
        {
          if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
              usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
            continue;
          }

          if ((usmDbOspfIntfAreaIdSet(unit, iface, areaID) != L7_SUCCESS) ||
              (usmDbOspfIntfAdminModeSet(unit, iface, L7_ENABLE) != L7_SUCCESS))
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfAreaId);
            continue;
          }
          /* "secondaries none" ==> don't advertise any secondary address on this
           * interface in ospf, so setting the flag to disable
           */
          if ((numArg > 1) &&
              (strcmp(argv[index+secondaryNoneIndex], pStrInfo_routing_Second) == 0) &&
              (strcmp(argv[index+secondaryNoneIndex+1], pStrInfo_common_None_3) == 0))
          {
            if(usmDbOspfIntfSecondariesFlagSet(unit, iface, L7_DISABLE) != L7_SUCCESS)
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantDsblSecond);
            }
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidAreaId);
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {/*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        /* If the secondaries keyword is entered in the no form of
         * this command, the secondary IP addresses will be advertised.
         * If the secondaries keyword is not present, OSPFv2 will be disabled.
         */
        if ((numArg == 0) &&
            ((usmDbOspfIntfAreaIdSet(unit, iface, 0)!= L7_SUCCESS) ||
             (usmDbOspfIntfAdminModeSet(unit, iface, L7_DISABLE) != L7_SUCCESS) ))
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDelIfAreaId);
          continue;
        }
        else if ((numArg > 0) &&
                 (strcmp(argv[index+secondaryIndex], pStrInfo_routing_Second) == 0) &&
                 (strcmp(argv[index+secondaryIndex+1], pStrInfo_common_None_3) == 0))
        {
          if(usmDbOspfIntfSecondariesFlagSet(unit, iface, L7_ENABLE) != L7_SUCCESS)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantEnblSecond);
          }
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Enables the OSPF on a router interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf mode
*
* @cmdsyntax for no command: no ip ospf
*
* @cmdhelp
* @Open Issue TO BE DISCUSSED WITH TEAM
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(!(numArg == 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfMode);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if ( usmDbOspfIntfAdminModeSet(unit, iface, L7_ENABLE) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfMode);
        }
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if ( usmDbOspfIntfAdminModeSet(unit, iface, L7_DISABLE) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfMode);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfMode);
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Sets the OSPF priority for the specified router interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf priority <0-255>
*
* @cmdsyntax for no command: no ip ospf priority
*
* @cmdhelp
*
* @cmddescript    The priority default is 1 which is the highest router priority.
*                 A value of '0' indicated that the router is not elegible
*                 to become the designated router on this network.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfPriority (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 argPriority = 1;
  L7_uint32 iface, s, p;
  L7_uint32 priority;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfPri_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoIntfPri);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argPriority], &priority) == L7_SUCCESS)
    {
      if (( priority >= L7_OSPF_INTF_PRIORITY_MIN ) && ( priority <= L7_OSPF_INTF_PRIORITY_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
          {
            if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
                usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
              continue;
            }

            if (usmDbOspfIntfPrioritySet(unit, iface, priority) != L7_SUCCESS)
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfPri);
            }
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_common_CfgOspfIntfPri, L7_OSPF_INTF_PRIORITY_MIN, L7_OSPF_INTF_PRIORITY_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfPri_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if (usmDbOspfIntfPrioritySet(unit, iface, L7_OSPF_INTF_PRIORITY_DEF) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfDefPri);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF hello interval for the specified interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf hello-interval <1-65535>
*             <L7_OSPF_INTF_SECONDS_MIN, L7_OSPF_INTF_SECONDS_MAX>
*
* @cmdsyntax for no command: no ip ospf hello-interval
*
* @cmdhelp
*
* @cmddescript   between the Hello packets that the router send on the interface.
*                The value for the length of time must be the same
*                for all routers attached to a network.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfHelloInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeconds = 1;
  L7_uint32 iface, s, p;
  L7_uint32 seconds;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfIntvlHello_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfNoIntvlHello);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &seconds) == L7_SUCCESS)
    {
      if ( ( seconds >= L7_OSPF_INTF_SECONDS_MIN ) && ( seconds <= L7_OSPF_INTF_SECONDS_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
          {
            if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
                usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
              continue;
            }

            if ( usmDbOspfIntfHelloIntervalSet(unit, iface, seconds) != L7_SUCCESS)
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetHelloIntv);
            }
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForOspfHelloIntvlMustBeInRangeOfTo,
                                L7_OSPF_INTF_SECONDS_MIN, L7_OSPF_INTF_SECONDS_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfIntvlHello_1);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if ( usmDbOspfIntfHelloIntervalSet(unit, iface, L7_OSPF_INTF_SECONDS_DEF) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefHelloIntv);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF dead interval for the specified interface
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf dead-interval <1 to 65535>
*
* @cmdsyntax for no command: no ip ospf dead-interval
*
* @cmdhelp
*
* @cmddescript      The number of seconds that a router's Hello packets
*                   have not been seen before it's neighbor routers declare
*                   that the router is down.  This should be some multiple of HelloInterval.
*                   This value must be the same for all routers attached
*                   to a common network.
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfDeadInterval (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 argSeconds = 1;
  L7_uint32 iface, s, p;
  L7_uint32 seconds;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfIntfIntvlDead_1);
    }
    if(ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfIntfNoIntvlDead);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &seconds) == L7_SUCCESS)
    {
      if ( ( seconds >= L7_OSPF_INTF_DEAD_MIN ) && ( seconds <= L7_OSPF_INTF_DEAD_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
          {
            if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
                usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
              continue;
            }

            if ( usmDbOspfIntfDeadIntervalSet(unit, iface, seconds) != L7_SUCCESS)
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfDeadInt);
            }
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForOspfDeadIntvlMustBeInRangeOfTo,
                                L7_OSPF_INTF_DEAD_MIN, L7_OSPF_INTF_DEAD_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfIntfIntvlDead_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if ( usmDbOspfIntfDeadIntervalSet(unit, iface, L7_OSPF_INTF_DEAD_DEF) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefIfDeadInt);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF retransmit interval for the specified interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf retransmit-interval <1 to 3600>
*
* @cmdsyntax for no command: no ip ospf retransmit-interval
*
* @cmdhelp
*
* @cmddescript  <seconds> is the number of seconds between link-state
*               advertisement retransmissions for adjacencies belonging
*               to this router interface.  This value is also used when
*               retransmitting database descriptions and link-state
*               request packets.
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfRetransmitInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeconds = 1;
  L7_uint32 iface, s, p;
  L7_uint32 seconds;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfIntvlReTx_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoIntfIntvlReTx);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &seconds) == L7_SUCCESS)
    {
      if (( seconds >= L7_OSPF_INTF_RETRANSMIT_MIN ) && ( seconds <= L7_OSPF_INTF_RETRANSMIT_MAX ))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
          {
            if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
                usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
              continue;
            }

            if ( usmDbOspfIntfRxmtIntervalSet(unit, iface, seconds) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetIfReTxInt);
            }
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForOspfReTxIntvlMustBeInRangeOfTo,
                                L7_OSPF_INTF_RETRANSMIT_MIN, L7_OSPF_INTF_RETRANSMIT_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfIntvlReTx_1);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if ( usmDbOspfIntfRxmtIntervalSet(unit, iface, L7_OSPF_INTF_RETRANSMIT_DEF ) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefIfReTxInt);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  sets the OSPF Transit Delay for the specified interface
*           In addition, it sets the estimated number of seconds it takes
*           to transmit a link state update packet over this interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf transmit-delay <1 to 3600 >
*
* @cmdsyntax for no command: no ip ospf transmit-delay
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfTransmitDelay (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argSeconds = 1;
  L7_uint32 iface, s, p;
  L7_uint32 seconds;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfIfTransitDelay_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoIntfIfTransitDelay);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argSeconds], &seconds) == L7_SUCCESS)
    {
      if ( ( seconds >= L7_OSPF_INTF_TRANSMITDELAY_MIN ) && ( seconds <= L7_OSPF_INTF_TRANSMITDELAY_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
          {
            if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
                usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
            {
              continue;
            }

            if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
              continue;
            }

            if ( usmDbOspfIntfTransitDelaySet(unit, iface, seconds) != L7_SUCCESS )
            {
              status = L7_FAILURE;
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
              ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetTxInt);
            }
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForOspfTransitIntvlMustBeInRangeOfTo,
                                L7_OSPF_INTF_TRANSMITDELAY_MIN, L7_OSPF_INTF_TRANSMITDELAY_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfIfTransitDelay_1);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if ( usmDbOspfIntfTransitDelaySet(unit, iface, FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefTxInt);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  sets the OSPF Authentication Type to None for the specified interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf authentication none
*
* @cmdsyntax for no command: no ip ospf authentication
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfAuthenticationNone(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if (!((argc == 3 && (ewsContext->commType == CLI_NO_CMD)) || (argc == 4 && (ewsContext->commType == CLI_NORMAL_CMD))))
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfAuthNone_1);
    }
    if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrNoOspfIntfAuthNone);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Callee will display error messages */
  if(commandIpOspfAuthentication( ewsContext, argv, index, L7_AUTH_TYPE_NONE, L7_NULL, 0 ) != L7_SUCCESS)
  {
    return cliPrompt( ewsContext );
  }

  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt( ewsContext );
}

/*********************************************************************
*
* @purpose  sets the OSPF Authentication Type to Simple for the specified interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf authentication simple <key>
*
* @cmdsyntax for no command: no ip ospf authentication
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfAuthenticationSimple(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argKey = 1;
  L7_uchar8 strKey[L7_AUTH_MAX_KEY_OSPF_SIMPLE+1];
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfAuthSimple_1);
  }

  if (strlen(argv[index+argKey]) > (sizeof(strKey)-1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRipAuthKey, L7_AUTH_MAX_KEY_OSPF_SIMPLE);
  }

  OSAPI_STRNCPY_SAFE(strKey, argv[index + argKey]);

  /* Callee will display error messages */
  if(commandIpOspfAuthentication( ewsContext, argv, index, L7_AUTH_TYPE_SIMPLE_PASSWORD, strKey, 0 ) != L7_SUCCESS)
  {
    return cliPrompt( ewsContext );
  }

  /*************Set Flag for Script Success******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt( ewsContext );
}

/*********************************************************************
*
* @purpose  sets the OSPF Authentication Type to Encrypt for the specified interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf authentication encrypt <key> <keyid>
*
* @cmdsyntax for no command: no ip ospf authentication
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfAuthenticationEncrypt(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argKey = 1;
  L7_uint32 argKeyId = 2;
  L7_uchar8 strKey[L7_AUTH_MAX_KEY_OSPF_MD5+1];
  L7_uint32 keyId;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (numArg != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfAuthEncrypt_1);
  }

  if (strlen(argv[index+argKey]) > (sizeof(strKey)-1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRipAuthKey, L7_AUTH_MAX_KEY_OSPF_MD5);
  }

  OSAPI_STRNCPY_SAFE(strKey, argv[index + argKey]);

  if (cliConvertTo32BitUnsignedInteger(argv[index+argKeyId], &keyId) == L7_SUCCESS &&
      keyId <= L7_OSPF_AUTH_KEY_ID_MAX )
  {
    /* Callee will display error messages */
    if(commandIpOspfAuthentication( ewsContext, argv, index, L7_AUTH_TYPE_MD5, strKey, keyId ) != L7_SUCCESS)
    {
      return cliPrompt( ewsContext );
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgOspfAuthKeyId, L7_OSPF_AUTH_KEY_ID_MAX);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt( ewsContext );
}

/*********************************************************************
*
* @purpose  sets the OSPF Authentication Type based on input
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype L7_RC_t (L7_SUCCESS or L7_FAILURE)
*
* @returns cliPrompt(ewsContext)
*
* @notes Assumes the caller has verified the parameter count is appropriate
*        for the authentication type specified.
*
*
* @end
*
*********************************************************************/
L7_RC_t commandIpOspfAuthentication(EwsContext ewsContext,
                                    const L7_char8 * * argv,
                                    L7_uint32 index,
                                    L7_uint32 intType,
                                    L7_uchar8 * strKey,
                                    L7_uint32 keyId)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 prevType, prevKeyId;
  L7_uchar8 prevKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_BOOL restore = L7_TRUE;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
  {
    if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
        usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
    {
      continue;
    }

    if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
    {
      status = L7_FAILURE;
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
      continue;
    }
  
    /* Get previous values to enable restore if necessary. */
    memset ( prevKey, 0, sizeof(prevKey));
    if (usmDbOspfIntfAuthenticationGet(unit, iface, &prevType, prevKey, &prevKeyId) != L7_SUCCESS)
    {
      restore = L7_FALSE;
    }
  
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfIntfAuthenticationSet(unit, iface, intType, strKey, keyId) != L7_SUCCESS)
      {
        if (restore == L7_TRUE)
        {
          (void)usmDbOspfIntfAuthenticationSet(unit, iface, prevType, prevKey, prevKeyId);
        }
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_CfgOspfAuthFail);
        continue;
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return status;
}

/*********************************************************************
*
* @purpose  sets the OSPF Cost for the specified interface.
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf cost <1-65535>
*
* @cmdsyntax for no command: no ip ospf cost
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 argCost = 1;
  L7_uint32 cost;
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 ipaddr;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  L7_char8 ipbuf[CLI_IPADDR_SIZE];
  OSAPI_STRNCPY_SAFE(ipbuf, pStrInfo_common_0000);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  rc = usmDbInetAton(ipbuf, &ipaddr);

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfCost_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoIntfCost);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argCost], &cost) == L7_SUCCESS)
    {
      if (cost < L7_OSPF_INTF_COST_MIN || cost > L7_OSPF_INTF_COST_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfCost_1);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
        {
          if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
              usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
          {
            continue;
          }

          if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
            continue;
          }

          if (usmDbOspfIfMetricValueSet (unit, ipaddr, iface, L7_TOS_NORMAL_SERVICE, cost) != L7_SUCCESS)
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetOspfCost);
          }
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfIntfCost_1);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
            usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        if (usmDbOspfIfMetricValueSet (unit, ipaddr, iface, L7_TOS_NORMAL_SERVICE, L7_OSPF_INTF_METRIC_NOT_CONFIGURED) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefOspfCost);
        }
      }
    }
  }
  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  sets the specified metric default for the route advertised into the stub area.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  area <areaid> default-cost <1-16777215>
*
* @The no option is not valid for this command.
*
* @cmdhelp
*
* @cmddescript     sets the metric value applied at the indicated
*                  type of service (set and get operations)
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaAreaIdDefaultCost (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaId = 1, tempInd = 0;

  L7_uint32 argMetricValue = 3;
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 tos;
  L7_uint32 areaid;
  L7_uint32 metricValue;
  L7_uint32 unit;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);

  if((ewsContext->commType == CLI_NORMAL_CMD) && (argc != 4))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaStubMetricVal);
  }
  else if((ewsContext->commType == CLI_NO_CMD ) && (argc != 3))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaStubMetricValNo);
  }

  tos = L7_TOS_NORMAL_SERVICE;

  if (strlen(argv[tempInd+argAreaId]) >= sizeof(strAreaId))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidAreaId);
  }
  OSAPI_STRNCPY_SAFE(strAreaId,argv[tempInd + argAreaId]);

  if (usmDbInetAton(strAreaId, &areaid) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidAreaId);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[tempInd+argMetricValue], &metricValue) == L7_SUCCESS)
    {
      if ( ( metricValue >= L7_OSPF_AREA_STUBMETRIC_MIN ) && ( metricValue <= L7_OSPF_AREA_STUBMETRIC_MAX ) )
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if ( usmDbOspfStubMetricSet(unit, areaid, tos, metricValue) != L7_SUCCESS )
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetAreaMetricVal);
          }
        }
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_MetricValRangeMustBeInRangeOfTo,
                                L7_OSPF_AREA_STUBMETRIC_MIN, L7_OSPF_AREA_STUBMETRIC_MAX);
      }
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidMetricVal);
      cliSyntaxBottom(ewsContext);
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaStubMetricVal);
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD )
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(usmDbOspfStubMetricSet(unit, areaid, tos, FD_OSPF_AREA_DEFAULT_STUB_METRIC) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetAreaMetricValNo);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  sets the specified metric type for the route advertised into the stub area.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  area <areaid> stub [no-summary]
*
* @cmdsyntax for no command: no area <areaid> stub [no-summary]
*
* @cmdhelp
*
* @cmddescript     sets the metric value applied at the indicated
*                  type of service (set and get operations)
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaAreaIdStubSummaryLSA (EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaId = 1, tempInd = 0;
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 areaid;
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 unit;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if( !(argc == 4 && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaStubSummaryLsa_1);
  }

  if (strlen(argv[tempInd+argAreaId]) >= sizeof(strAreaId))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidAreaId);
  }
  OSAPI_STRNCPY_SAFE(strAreaId,argv[tempInd + argAreaId]);
  if (usmDbInetAton(strAreaId, &areaid) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidAreaId);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_DISABLE;
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_ENABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRtrOspfAreaStubSummaryLsa_1);
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    rc = usmDbOspfMapAreaSummarySet(unit, areaid, mode);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_REQUEST_DENIED)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                              ewsContext, pStrErr_common_BackboneCantBeStub);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot,
                                              ewsContext, pStrErr_common_CantSetAreaStubSummLsa);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the range for an area
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
* @notes  The user is advised to disable OSPF before using this command.
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  area <areaId> range <ip addr>
*                                 <subnet mask> <lsdbtype> [advertise | not-advertise]
*
* @cmdsyntax for no command: no area <areaid> range <ip addr> <subnet mask> <lsdbtype>
*
* @cmdhelp
*
* @cmddescript  An area ID of 0.0.0.0 is reserved for the backbone.
*               Area IDs have the same form as IP addresses, but
*               are distinct from IP addresses.  The IP network number
*               of the sub-netted network may be used for the area ID.
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaAreaIdRange(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaId = 1, tempInd = 0;

  L7_uint32 argIPaddr = 3;
  L7_uint32 argSubnetMask = 4;
  L7_uint32 argLsdb = 5;
  L7_uint32 argOpt1 = 6;
  L7_uint32 longLsdb = 0;
  L7_uint32 longAdvertise = 0;
  L7_uint32 longAreaId;
  L7_uint32 longIPaddr;
  L7_uint32 longSubnetMask;
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIPaddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strSubnetMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strOpt1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strLsdb[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_RC_t status;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if ( (argc < 6) || (argc > 7) ) /* for NO command argc will be 6 */
  {

    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaRange);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfAreaRangeDel);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if ((argc == 7) && ( ewsContext->commType == CLI_NORMAL_CMD))
  {
    if ((strlen(argv[tempInd+argOpt1]) >= sizeof(strOpt1)) || (strlen(argv[tempInd+argLsdb]) >= sizeof(strLsdb)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaRange);
    }
    OSAPI_STRNCPY_SAFE(strOpt1, argv[tempInd + argOpt1]);
    cliConvertToLowerCase(strOpt1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (argc == 7)
    {
      if (strcmp(strOpt1, pStrInfo_common_Advertise_1) == 0)
      {
        longAdvertise = L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING;
      }
      else if (strcmp(strOpt1, pStrInfo_common_NoAdvertise) == 0)
      {
        longAdvertise = L7_OSPF_AREA_AGGREGATE_EFFECT_DONOT_ADVERTISEMATCHING;
      }
    }
    else
    {
      longAdvertise = L7_OSPF_AREA_AGGREGATE_EFFECT_ADVERTISEMATCHING;
    }
  }
  else /*CLI_NO_CMD*/
  {
    if (argc != 6)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrOspfAreaRangeDel);
    }
  }

  if (strlen(argv[tempInd+argAreaId]) >= sizeof(strAreaId) ||
      strlen(argv[tempInd+argIPaddr]) >= sizeof(strIPaddr) ||
      strlen(argv[tempInd+argSubnetMask]) >= sizeof(strSubnetMask) ||
      strlen(argv[tempInd+argLsdb]) >= sizeof(strLsdb))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaRange);
  }
  OSAPI_STRNCPY_SAFE(strAreaId, argv[tempInd + argAreaId]);

  OSAPI_STRNCPY_SAFE(strIPaddr, argv[tempInd + argIPaddr]);

  OSAPI_STRNCPY_SAFE(strSubnetMask, argv[tempInd + argSubnetMask]);

  OSAPI_STRNCPY_SAFE(strLsdb, argv[tempInd + argLsdb]);
  cliConvertToLowerCase(strLsdb);
  if (strcmp(strLsdb, pStrInfo_common_SummaryLink_1) == 0)
  {
    longLsdb = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_SUMMARYLINK;
  }
  else if (strcmp(strLsdb, pStrInfo_common_NssaExternalLink) == 0)
  {
    longLsdb = L7_OSPF_AREA_AGGREGATE_LSDBTYPE_NSSAEXTERNALLINK;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_InvalidAreaRangeType);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (  (usmDbInetAton(strAreaId, &longAreaId) == L7_SUCCESS) &&
      (usmDbInetAton(strIPaddr, &longIPaddr) == L7_SUCCESS) &&
      (usmDbInetAton(strSubnetMask, &longSubnetMask) == L7_SUCCESS) )
  {

    if (usmDbNetmaskValidate(longSubnetMask) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSubnetMask);
    }
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        status = usmDbOspfAreaAggregateStatusSet(unit, longAreaId, longLsdb, longIPaddr,
                                                 longSubnetMask, L7_CREATE);
        if (status == L7_SUCCESS)
        {
          if (usmDbOspfAreaAggregateEffectSet(unit, longAreaId, longLsdb, longIPaddr,
                                              longSubnetMask, longAdvertise) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_common_CreateAreaRange);
          }
        }
        else if(status == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_common_CreateAreaRangeConflict);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrInfo_common_CreateAreaRange);
        }
      }
    }
    else if ( ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        status = usmDbOspfAreaAggregateStatusSet(unit, longAreaId, longLsdb, longIPaddr,
                                                 longSubnetMask, L7_DELETE);
        if (status != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_common_CantDelAreaRange);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaRange);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose  Configures a network with OSPF
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
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  network <ipaddr> <wildcard-mask> area <areaid>
*
* @cmdsyntax for no command: no network <ipaddr> <wildcard-mask> area <areaid>
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandNetworkArea(EwsContext ewsContext, L7_uint32 argc,
                                   const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argIpAddr=1, argMask=2, argAreaId=4;
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMask[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit, ipaddress, wildcardmask, areaId, numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg > 4)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfNwArea);
  }

  if (strlen(argv[index+argIpAddr]) >= sizeof(strIpAddr) ||
      strlen(argv[index+argMask]) >= sizeof(strMask) ||
      strlen(argv[index+argAreaId]) >= sizeof(strAreaId))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrOspfNwArea);
  }

  osapiStrncpySafe(strIpAddr, argv[index + argIpAddr], sizeof(strIpAddr));

  osapiStrncpySafe(strMask, argv[index + argMask], sizeof(strMask));

  osapiStrncpySafe(strAreaId, argv[index + argAreaId], sizeof(strAreaId));

  if ( (usmDbInetAton(strIpAddr, &ipaddress) != L7_SUCCESS) ||
      (usmDbInetAton(strMask, &wildcardmask) != L7_SUCCESS) ||
      (usmDbInetAton(strAreaId, &areaId) != L7_SUCCESS) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_routing_CfgRtrOspfNwArea);
  }

  if(ipaddress == 0)
  {
    /* "0" network is an invalid input */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_routing_CfgRtrOspfNwZero);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(usmDbOspfNetworkAreaEntryCreate(unit, ipaddress, wildcardmask,
                                         areaId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_routing_CantCreateNwArea);
      }
    }
  }
  else if ( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(usmDbOspfNetworkAreaEntryDelete(unit, ipaddress, wildcardmask,
                                         areaId) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, pStrErr_routing_CantDelNwArea);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the OSPF auto cost reference bandwidth
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  auto-cost reference-bandwidth <L7_OSPF_MIN_REF_BW - L7_OSPF_MAX_REF_BW>
*
* @cmdsyntax for no command: no auto-cost reference-bandwidth
*
* @cmdhelp
*
* @end
*
*********************************************************************/
const L7_char8 *commandAutoCost(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argVal = 2;
  L7_uint32 val;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_uint32 numArg;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (!((numArg == 2 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 1 && (ewsContext->commType == CLI_NO_CMD))))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_common_CfgRtrOspfAutoCostRef, L7_OSPF_MIN_REF_BW, L7_OSPF_MAX_REF_BW);
      ewsTelnetWrite(ewsContext, buf);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoAutoCostRef);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliConvertTo32BitSignedInteger(argv[index+argVal], &val) == L7_SUCCESS)
    {
      if (( val >= L7_OSPF_MIN_REF_BW ) && ( val <= L7_OSPF_MAX_REF_BW ))
      {
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
        {
          if (usmDbOspfAutoCostRefBwSet(unit, val) != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetAutoCostRef);
            cliSyntaxBottom(ewsContext);
          }
        }
      }
      else
      {
        sprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, pStrErr_common_CantSetOspfAutoCostRange, L7_OSPF_MIN_REF_BW, L7_OSPF_MAX_REF_BW);
        ewsTelnetWrite( ewsContext, stat);
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf), pStrErr_common_CfgRtrOspfAutoCostRef, L7_OSPF_MIN_REF_BW, L7_OSPF_MAX_REF_BW);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfAutoCostRefBwSet(unit, FD_OSPF_DEFAULT_REFER_BANDWIDTH ) != L7_SUCCESS)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetAutoCostRef);
        cliSyntaxBottom(ewsContext);
      }
    }
  }

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the default route information that is to be used for advertisement.
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
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  default-information originate [always] [metric <1-16777215>] [metric-type <value>]
*
* @cmdsyntax for no command: no default-information originate [always] [metric][metric-type]
*
* @cmdhelp
*
* @cmddescript  Note that a metric value is a 24 bit integer value
*
*
* @end
*
*********************************************************************/

const L7_char8 *commandDefaultInformationOriginateOspf(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 nextIndex = 1;
  L7_BOOL originateDefault = L7_FALSE;
  L7_BOOL Isalways = L7_FALSE;
  L7_int32 defRouteMetric;
  L7_OSPF_EXT_METRIC_TYPES_t defRouteMetType;

  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();
  memset (buf, 0,sizeof(buf));


  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    originateDefault = L7_TRUE;

    if (numArg > 5)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, buf, sizeof(buf),  pStrErr_common_CfgRtrOspfDeflRouteInfo,
          L7_OSPF_DEFAULT_METRIC_MIN,L7_OSPF_DEFAULT_METRIC_MAX,
          L7_OSPF_METRIC_EXT_TYPE1,L7_OSPF_METRIC_EXT_TYPE2);
      ewsTelnetWrite( ewsContext, buf);
    }
  }
  if(  ewsContext->commType == CLI_NO_CMD)
  {
    originateDefault = L7_FALSE;
    if (numArg > 3)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoDeflInfoOr);
      return cliSyntaxReturnPrompt (ewsContext, "");
    }
  }



  if ( usmDbOspfDefaultRouteMetricGet(unit, &defRouteMetric) == L7_NOT_EXIST)
  {
    defRouteMetric = FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC;
  }
  if ( usmDbOspfDefaultRouteMetricTypeGet(unit, &defRouteMetType) != L7_SUCCESS)
  {
    defRouteMetType = FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE;
  }

  if ( usmDbOspfDefaultRouteAlwaysGet(unit, &Isalways) != L7_SUCCESS)
  {
    Isalways = FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS;
  }

  if(numArg == 0 && ewsContext->commType == CLI_NO_CMD)
  {
    defRouteMetric = FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC;
    defRouteMetType = FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE;
    Isalways = FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS;
  }
  else
  {
    if( numArg >= nextIndex)
    {
      memset(stat, 0, sizeof(stat));
      OSAPI_STRNCPY_SAFE(stat,argv[index+ nextIndex]);
      cliConvertToLowerCase(stat);
      if(strcmp(stat, pStrInfo_common_Always_1) == 0)
      {
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {
          Isalways = L7_TRUE;
        }
        else if(ewsContext->commType == CLI_NO_CMD)
        {
          Isalways = FD_OSPF_ORIG_DEFAULT_ROUTE_ALWAYS;
        }

        nextIndex++;
      }
    }
    if( numArg >= nextIndex)
    {
      memset(stat, 0, sizeof(stat));
      OSAPI_STRNCPY_SAFE(stat,argv[index+ nextIndex]);
      cliConvertToLowerCase(stat);
      if(strcmp(stat, pStrInfo_common_Metric_1) == 0)
      {
        nextIndex++;
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {

          if( numArg >= nextIndex)
          {
            rc =  cliConvertTo32BitUnsignedInteger(argv[index+nextIndex], &defRouteMetric);
            nextIndex++;

            if ( (rc != L7_SUCCESS) || (defRouteMetric < L7_OSPF_DEFAULT_METRIC_MIN ) || (defRouteMetric > L7_OSPF_DEFAULT_METRIC_MAX))
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_common_InvalidMetric,
                                                      L7_OSPF_DEFAULT_METRIC_MIN,L7_OSPF_DEFAULT_METRIC_MAX);
            }
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRtrOspfDeflRouteInfo,
                L7_OSPF_DEFAULT_METRIC_MIN,L7_OSPF_DEFAULT_METRIC_MAX,
                L7_OSPF_METRIC_EXT_TYPE1,L7_OSPF_METRIC_EXT_TYPE2);
          }
        }
        else if (ewsContext->commType == CLI_NO_CMD)
        {
          defRouteMetric = FD_OSPF_ORIG_DEFAULT_ROUTE_METRIC;
        }

      }
    }

    if( numArg >=nextIndex)
    {
      memset(stat, 0, sizeof(stat));
      OSAPI_STRNCPY_SAFE(stat,argv[index+ nextIndex]);
      cliConvertToLowerCase(stat);
      if(strcmp(stat, pStrInfo_common_MetricType_1) == 0)
      {
        nextIndex++;
        if (ewsContext->commType == CLI_NORMAL_CMD)
        {

          if( numArg >=nextIndex)
          {
            rc = cliConvertTo32BitUnsignedInteger(argv[index+nextIndex], &defRouteMetType);

            if ( rc != L7_SUCCESS ||((defRouteMetType != L7_OSPF_METRIC_EXT_TYPE1 ) && ( defRouteMetType != L7_OSPF_METRIC_EXT_TYPE2 )))
            {
              return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_common_InvalidMetricType,
                                                      L7_OSPF_METRIC_EXT_TYPE1, L7_OSPF_METRIC_EXT_TYPE2);
            }

          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRtrOspfDeflRouteInfo,
                                                  L7_OSPF_DEFAULT_METRIC_MIN,L7_OSPF_DEFAULT_METRIC_MAX,
                                                                                    L7_OSPF_METRIC_EXT_TYPE1,L7_OSPF_METRIC_EXT_TYPE2);
          }
        }
        else if (ewsContext->commType == CLI_NO_CMD)
        {
          defRouteMetType = FD_OSPF_ORIG_DEFAULT_ROUTE_TYPE;
        }
      }
    }
  }


    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfDefaultRouteSet(unit, originateDefault, Isalways, defRouteMetric, defRouteMetType) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDeflRouteInfo);
      }
    }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures a virtual link on the specified interface.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command:  area <areaid > virtual-link <neighborid>
* @      [ authentication [ none|simple|encrypt} [key] [keyid]] ] [ hello-interval
* @      <1-65535> ] [ retransmit-interval <0-3600> ] [ transmit-delay <0-3600> ]
* @      [dead-interval <1-65535> ] ]
*
* @cmdsyntax for no command: no area <areaid> virtual-link <neighborid>
* @      [{authentication|hello-interval|retransmit-interval|transmit-delay|dead-interval}]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaVirtualLink(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc;
  L7_uint32 neighbor, areaId, extRouteCap;
  L7_uint32 unit;
  L7_uint32 numArg;

  L7_uint32 argAreaId = 1;
  L7_uint32 argNeighbor = 3;
  L7_uint32 cmdType = 4;
  L7_uint32 cmdLen = 0;

  L7_char8 strNeighbor[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strCmd[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  numArg = cliNumFunctionArgsGet();

  if (argc < 4)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfCreate);
    }
    if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoVirtualIntfCreate);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (strlen(argv[argNeighbor]) >= sizeof(strNeighbor) ||
      strlen(argv[argAreaId]) >= sizeof(strAreaId) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }
  memset (strNeighbor, 0,sizeof(strNeighbor));
  memset (strAreaId, 0,sizeof(strAreaId));
  OSAPI_STRNCPY_SAFE(strNeighbor,argv[argNeighbor]);
  OSAPI_STRNCPY_SAFE(strAreaId,argv[argAreaId]);

  if ( usmDbInetAton(strNeighbor, &neighbor) != L7_SUCCESS ||
      usmDbInetAton(strAreaId, &areaId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
  }

  if((numArg == 1) && (ewsContext->commType == CLI_NO_CMD))
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if(usmDbOspfVirtIfDelete (unit, areaId, neighbor) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDelVirtLink);
      }
    }
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ((rc = usmDbOspfVirtIfCreate(unit, areaId, neighbor)) != L7_SUCCESS )
      {
        if(rc == L7_ERROR)
        {
          /* Neighbor router-id can't be same as local router-id */
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CreateVirtualLinkRtrId);
        }
        else if(rc == L7_REQUEST_DENIED)
        {
          /* Transit area can't be a stub area or nssa area */
          if(usmDbOspfAreaExternalRoutingCapabilityGet(unit, areaId, &extRouteCap)
             == L7_SUCCESS)
          {
            switch(extRouteCap)
            {
            case L7_OSPF_AREA_IMPORT_NSSA:
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_CreateVirtualLinkNssa);
              break;
            case L7_OSPF_AREA_IMPORT_NO_EXT:
              ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_CreateVirtualLinkStubArea);
              break;
            default:
              ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_CreateVirtualLink);
              break;
            }
          }
        }
        else
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CreateVirtualLink_1);
        }

        return cliSyntaxReturnPrompt (ewsContext, "");
      }
    }
  }

  /* If there were only four arguments we are done */
  if(numArg == 1)
  {
    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  OSAPI_STRNCPY_SAFE(strCmd,argv[cmdType]);
  cliConvertToLowerCase(strCmd);

  cmdLen=strlen(strCmd);
  if (strncmp(strCmd, pStrInfo_common_Auth_1, min(cmdLen, strlen(pStrInfo_common_Auth_1))) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      VirtifHandleAuthentication(ewsContext, numArg, argv, index);
    }

  }

  else if (strncmp(strCmd, pStrInfo_common_HelloIntvl_1, min(cmdLen, strlen(pStrInfo_common_HelloIntvl_1))) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      VirtifHandleHelloInterval(ewsContext, numArg, argv, index, neighbor, areaId);
    }

  }

  else if (strncmp(strCmd, pStrInfo_common_DeadIntvl_1, min(cmdLen, strlen(pStrInfo_common_DeadIntvl_1))) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      VirtifHandleDeadInterval(ewsContext, numArg, argv, index, neighbor, areaId);
    }

  }

  else if (strncmp(strCmd, pStrInfo_common_ReTxIntvl_1, min(cmdLen, strlen(pStrInfo_common_ReTxIntvl_1))) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      VirtifHandleRxmtInterval(ewsContext, numArg, argv, index, neighbor, areaId);
    }

  }

  else if (strncmp(strCmd, pStrInfo_common_TxDelay, min(cmdLen, strlen(pStrInfo_common_TxDelay))) == 0)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      VirtifHandleTransmitDelay(ewsContext, numArg, argv, index, neighbor, areaId);
    }

  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to set the ospf auth type and key
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype L7_RC_t
*
* @notes Called by the three ospf virtif authentication commands
*
*
* @end
*
*********************************************************************/
L7_RC_t commandIpOspfVirtifAuthentication(EwsContext ewsContext,
                                          const L7_char8 * * argv,
                                          L7_uint32 index,
                                          L7_uint32 intType,
                                          L7_uchar8 * strKey,
                                          L7_uint32 keyId )
{
  L7_uint32 argAreaId = 1;
  L7_uint32 argNeighbor = 3;
  L7_char8 strNeighbor[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAreaId[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 neighbor, areaId;
  L7_uint32 prevType, prevKeyId;
  L7_uchar8 prevKey[L7_AUTH_MAX_KEY_OSPF+1];
  L7_BOOL restore = L7_TRUE;
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  if (strlen(argv[argNeighbor]) >= sizeof(strNeighbor) ||
      strlen(argv[argAreaId]) >= sizeof(strAreaId))
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  memset (strNeighbor, 0,sizeof(strNeighbor));
  memset (strAreaId, 0,sizeof(strAreaId));
  OSAPI_STRNCPY_SAFE(strNeighbor,argv[argNeighbor]);
  OSAPI_STRNCPY_SAFE(strAreaId,argv[argAreaId]);
  if ( usmDbInetAton(strNeighbor, &neighbor) != L7_SUCCESS ||
      usmDbInetAton(strAreaId, &areaId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgBgpRtrPolicyIpAddrInvalid);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /* Get previous values to enable restore */
  memset ( prevKey, 0, sizeof(prevKey));
  if ( usmDbOspfVirtIfAuthenticationGet(unit, areaId, neighbor, &prevType, prevKey, &prevKeyId) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_CfgOspfAuthFail);
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_routing_CfgOspfAuthIntfAreaMismatchFail);
    cliSyntaxBottom(ewsContext);
    return L7_FAILURE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbOspfVirtIfAuthenticationSet(unit, areaId, neighbor, intType, strKey, keyId) != L7_SUCCESS)
    {
      if (restore == L7_TRUE)
      {
        (void)usmDbOspfVirtIfAuthenticationSet(unit, areaId, neighbor, prevType, prevKey, prevKeyId);
      }

      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_common_CfgOspfAuthFail);
      cliSyntaxBottom(ewsContext);
      return L7_FAILURE;
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Utility routine to handle authentication request for virtual interface
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandAreaVirtualLink function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *VirtifHandleAuthentication( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv,L7_uint32 index)
{
  L7_uint32 argAuthType = 5;
  L7_uint32 keyParam = 6;
  L7_uint32 keyidParam = 7;
  L7_uint32 keyId;

  L7_char8 strAuthType[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strAuthKey[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg < 3 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfVirtualIntfAuth);
    }
    OSAPI_STRNCPY_SAFE(strAuthType,argv[argAuthType]);
    cliConvertToLowerCase(strAuthType);

    if (strcmp(strAuthType, pStrInfo_common_None_3) == 0)
    {
      if ( numArg != 3 )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfVirtualIntfAuthNone);
      }

      else
      {
        if(commandIpOspfVirtifAuthentication( ewsContext, argv, index,
                                             L7_AUTH_TYPE_NONE, L7_NULL, 0 ) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }

    else if (strcmp(strAuthType, pStrInfo_common_Simple_2) == 0)
    {
      if ( numArg != 4 )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfVirtualIntfAuthSimple);
      }
      else
      {
        /* Get the Key */
        if (strlen(argv[keyParam]) > L7_AUTH_MAX_KEY_OSPF_SIMPLE )
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRipAuthKey, L7_AUTH_MAX_KEY_OSPF_SIMPLE);
        }

        OSAPI_STRNCPY_SAFE(strAuthKey, argv[keyParam]);

        if(commandIpOspfVirtifAuthentication ( ewsContext, argv, index,
                                              L7_AUTH_TYPE_SIMPLE_PASSWORD, strAuthKey, 0 ) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }

      }
    }

    else if (strcmp(strAuthType, pStrInfo_common_Encrypt_2) == 0)
    {
      if ( numArg != 5)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfVirtualIntfAuthEncrypt);
      }
      /* Get the Key */
      else
      {
        if (strlen(argv[keyParam]) > L7_AUTH_MAX_KEY_OSPF_MD5 )
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRipAuthKey, L7_AUTH_MAX_KEY_OSPF_SIMPLE);
        }

        OSAPI_STRNCPY_SAFE(strAuthKey, argv[keyParam]);

        /* Get the Key id */

        if ( cliConvertTo32BitUnsignedInteger(argv[keyidParam], &keyId) != L7_SUCCESS ||
            keyId > L7_OSPF_AUTH_KEY_ID_MAX )
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgOspfAuthKeyId, L7_OSPF_AUTH_KEY_ID_MAX);
        }
        if(commandIpOspfVirtifAuthentication(ewsContext, argv, index,
                                             L7_AUTH_TYPE_MD5, strAuthKey, keyId ) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if( numArg != 2 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfNoVirtualIntfAuth);
    }
    if(commandIpOspfVirtifAuthentication( ewsContext, argv, index,
                                         L7_AUTH_TYPE_NONE, L7_NULL, 0 ) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle the Hello Interval for virtual interface
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandAreaVirtualLink function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *VirtifHandleHelloInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv,L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId)
{
  L7_uint32 unit;
  L7_uint32 seconds;
  L7_uint32 argSeconds = 5;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( (numArg != 3) || ( cliConvertTo32BitUnsignedInteger(argv[argSeconds], &seconds) != L7_SUCCESS ))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfIntvlHello);
    }

    else if ( ( seconds >= L7_OSPF_INTF_SECONDS_MIN ) && ( seconds <= L7_OSPF_INTF_SECONDS_MAX))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbOspfVirtIfHelloIntervalSet(unit, areaId, neighbor, seconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetVirtIntHello);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfIntvlHello);
    }
  }

  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoVirtualIntfIntvlHello);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfVirtIfHelloIntervalSet(unit, areaId, neighbor, L7_OSPF_INTF_SECONDS_DEF) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefVirtIntHello);
      }
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle the dead Interval for virtual interface
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandAreaVirtualLink function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *VirtifHandleDeadInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv,L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId)
{
  L7_uint32 unit;
  L7_uint32 seconds;
  L7_uint32 argSeconds = 5;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if((numArg != 3) || (cliConvertTo32BitUnsignedInteger(argv[argSeconds], &seconds) != L7_SUCCESS ))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfVirtualIntfIntvlDead);
    }

    else if ( ( seconds >= L7_OSPF_INTF_DEAD_MIN ) && ( seconds <= L7_OSPF_INTF_DEAD_MAX ) )
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbOspfVirtIfRtrDeadIntervalSet(unit, areaId, neighbor, seconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetVirtIntDead);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrOspfVirtualIntfIntvlDead);
    }
  }

  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoVirtualIntfIntvlDead);
    }

    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if( usmDbOspfVirtIfRtrDeadIntervalSet(unit, areaId, neighbor, L7_OSPF_INTF_DEAD_DEF) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefVirtIntDead);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle the retransmit Interval for virtual interface
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandAreaVirtualLink function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *VirtifHandleRxmtInterval( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv,L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId)
{
  L7_uint32 unit;
  L7_uint32 seconds;
  L7_uint32 argSeconds = 5;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if((numArg != 3) || ( cliConvertTo32BitUnsignedInteger(argv[argSeconds], &seconds) != L7_SUCCESS ))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfIntvlRx);
    }
    else if ( ( seconds >= L7_OSPF_VIRTIF_RETRANSMIT_MIN ) && ( seconds <= L7_OSPF_VIRTIF_RETRANSMIT_MAX ) )
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbOspfVirtIfRetransIntervalSet (unit, areaId, neighbor, seconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetVirtIntRx);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfIntvlRx);
    }
  }

  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoVirtualIntfIntvlRx);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfVirtIfRetransIntervalSet (unit, areaId, neighbor, L7_OSPF_VIRTIF_RETRANSMIT_DEF ) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetDefVirtIntRx);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine to handle the transmit delay for virtual interface
*
*
* @param EwsContext ewsContext
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype cliPrompt(ewsContext)
*
* @notes Called by the commandAreaVirtualLink function.
*
*
* @end
*
*********************************************************************/
const L7_char8 *VirtifHandleTransmitDelay( EwsContext ewsContext, L7_uint32 numArg, const L7_char8 * * argv,L7_uint32 index, L7_uint32 neighbor, L7_uint32 areaId)
{
  L7_uint32 unit;
  L7_uint32 seconds;
  L7_uint32 argSeconds = 5;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if( (numArg != 3 ) || ( cliConvertTo32BitUnsignedInteger(argv[argSeconds], &seconds) != L7_SUCCESS ))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfTransDelay);
    }
    else if ( ( seconds >= L7_OSPF_VIRTIF_DELAY_MIN ) && ( seconds <= L7_OSPF_VIRTIF_DELAY_MAX) )
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ( usmDbOspfVirtIfTransitDelaySet  (unit, areaId, neighbor, seconds) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetVirtIntra);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfVirtualIntfTransDelay);
    }
  }
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 2)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoVirtualIntfTransDelay);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbOspfVirtIfTransitDelaySet  (unit, areaId, neighbor, L7_OSPF_VIRTIF_DELAY_DEF) !=L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetVirtIntra);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures OSPF Route-Redistribution
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
* @notes none
*
* @Mode  Vlan database Config
*
* @cmdsyntax  redistribute <rip | bgp | static | connected> >
   [metric <0 - 16777215 >] [metric-type <1 | 2>] [tag <0-4294967295 >] [subnets]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRedistributeOSPF(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,numArg;
  L7_uint32 argProtocol = 1;
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 mode, argMetricType = 0, argTagValue = 0, distList = 0;
  L7_int32 argMetricValue = 0;
  L7_BOOL argSubnetValue = L7_FALSE;
  L7_uint32 metricFlag, metricTypeFlag, tagFlag, subnetFlag;
  L7_uint32 position;
  L7_char8 strTok[L7_CLI_MAX_STRING_LENGTH];

  L7_char8 * param1 = pStrInfo_common_Metric_1;
  L7_char8 * param2 = pStrInfo_common_MetricType_1;
  L7_char8 * param3 = pStrInfo_common_Tag_1;
  L7_char8 * param4 = pStrInfo_common_Subnets_1;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg =  cliNumFunctionArgsGet();

  if (numArg < 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfRedistribute_1);
  }

  OSAPI_STRNCPY_SAFE(strTok, argv[index+argProtocol]);
  cliConvertToLowerCase(strTok);
  if (strcmp(strTok,pStrInfo_common_Rip3) == 0)
  {
    protocol = REDIST_RT_RIP;
  }
  else if (strcmp(strTok,pStrInfo_common_Bgp_1) == 0)
  {
    protocol = REDIST_RT_BGP;
  }
  else if (strcmp(strTok,pStrInfo_common_Static2) == 0)
  {
    protocol = REDIST_RT_STATIC;
  }
  else if (strcmp(strTok,pStrInfo_common_Connected_1) == 0)
  {
    protocol = REDIST_RT_LOCAL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_OspfRedistributeProto);
  }

  metricFlag = metricTypeFlag = tagFlag = subnetFlag = L7_DISABLE;

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;

    if (numArg == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Redistribute this protocol with default values for metric,
         * metric type etc */
        if (usmDbOspfRedistributeSet(unit, protocol, mode) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_EnblRouteReDistributionFailure);
        }
        else
        {
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        }
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* The user has supplied one or more optional arguments. Get the current
     * value for all options, so that all options specified in the command
     * can be set in one usmdb call. Making a usmdb call to set each option
     * individually would unnecessarily cause OSPF to originate LSAs multiple
     * times. */
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfRedistMetricGet(unit, protocol, &argMetricValue) == L7_NOT_EXIST)
      {
        /* No redistribution metric currently configured. */
        argMetricValue = FD_OSPF_REDIST_METRIC;
      }
      (void) usmDbOspfRedistMetricTypeGet(unit, protocol, &argMetricType);
      (void) usmDbOspfTagGet(unit, protocol, &argTagValue);
      (void) usmDbOspfRedistSubnetsGet(unit, protocol, &argSubnetValue);
      (void) usmDbOspfDistListGet(unit, protocol, &distList);
    }

    /* require options to be in this order:  metric, metric-type, tag, subnets */
    position = 2;

    if (position < numArg)
    {
      OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
      cliConvertToLowerCase(strTok);
      if((strcmp(strTok, param1)!=0) && (strcmp(strTok, param2)!=0) &&
         (strcmp(strTok, param3)!=0) && (strcmp(strTok, param4)!=0))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfRedistribute_1);
      }

      if (strcmp(strTok, param1) == 0)
      {
        position = position + 1;   /* go to metric value */
        if((cliConvertTo32BitUnsignedInteger(argv[index+position], &argMetricValue) != L7_SUCCESS) ||
           ((argMetricValue < L7_OSPF_REDIST_METRIC_MIN) || (argMetricValue > L7_OSPF_REDIST_METRIC_MAX)))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfMetricVal);
        }
        metricFlag = L7_ENABLE;       /* command sets metric */
        position = position + 1;      /* go to next keyword */
      }
    }

    if (position < numArg)
    {
      OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
      cliConvertToLowerCase(strTok);
      if((strcmp(strTok, param2)!=0) && (strcmp(strTok, param3)!=0) && (strcmp(strTok, param4)!=0))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfRedistribute_1);
      }

      if (strcmp(strTok, param2) == 0)
      {
        position = position+1;

        if((cliConvertTo32BitUnsignedInteger(argv[index+position], &argMetricType) != L7_SUCCESS) ||
           ((argMetricType != L7_OSPF_METRIC_EXT_TYPE1) && (argMetricType != L7_OSPF_METRIC_EXT_TYPE2)))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfMetricTypeVal);
        }
        metricTypeFlag = L7_ENABLE;
        position = position+1;
      }
    }

    if (position < numArg)
    {
      OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
      cliConvertToLowerCase(strTok);
      if((strcmp(strTok, param3)!=0) && (strcmp(strTok, param4)!=0))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfRedistribute_1);
      }

      if (strcmp(strTok, param3) == 0)
      {
        position = position + 1;

        if((cliConvertTo32BitUnsignedInteger(argv[index+position], &argTagValue) != L7_SUCCESS) ||
           ((argTagValue < L7_OSPF_REDIST_TAG_MIN) || (argTagValue > L7_OSPF_REDIST_TAG_MAX)))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfTagVal);
        }
      }
      tagFlag = L7_ENABLE;
      position = position + 1;
    }

    if (position == numArg)
    {
      OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
      cliConvertToLowerCase(strTok);
      if (strcmp(strTok, param4) == 0)
      {
        subnetFlag = L7_ENABLE;
        argSubnetValue = L7_TRUE;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfRedistribute_1);
      }
    }
  }   /* End of the "Normal" Command   */

  if (ewsContext->commType ==  CLI_NO_CMD)
  {
    mode = L7_DISABLE;

    if (numArg == 1)
    {
      /* Only the protocol field is specified. Setting the RR mode to ENABLE. */
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        /* Only the protocol field is specified. Revert redistribution configuration
         * for this protocol to the defaults.   */
        if (usmDbOspfRedistributeRevert(unit, protocol) != L7_SUCCESS)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_RevertRedistributionFailure);
        }
        else
        {
          ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        }
      }
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    else if (numArg <= 5)
    {
      position = 2;
      if (position <= numArg)
      {
        OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
        cliConvertToLowerCase(strTok);
        if((strcmp(strTok, param1)!=0) && (strcmp(strTok, param2)!=0) &&
           (strcmp(strTok, param3)!=0) && (strcmp(strTok, param4)!=0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfNoRedistribute);
        }

        if (strcmp(strTok, param1) == 0)
        {
          position = position + 1;
          metricFlag = L7_ENABLE;
        }
      }

      if (position <= numArg)
      {
        OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
        cliConvertToLowerCase(strTok);
        if((strcmp(strTok, param2)!=0) && (strcmp(strTok, param3)!=0) && (strcmp(strTok, param4)!=0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfNoRedistribute);
        }
        if (strcmp(strTok, param2) == 0)
        {
          position = position + 1;
          metricTypeFlag = L7_ENABLE;
        }
      }

      if (position <= numArg)
      {
        OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
        cliConvertToLowerCase(strTok);
        if((strcmp(strTok, param3)!=0) && (strcmp(strTok, param4)!=0))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfNoRedistribute);
        }
        if (strcmp(strTok, param3) == 0)
        {
          position = position + 1;
          tagFlag = L7_ENABLE;
        }
      }

      if (position == numArg)
      {
        OSAPI_STRNCPY_SAFE(strTok, argv[index+position]);
        cliConvertToLowerCase(strTok);
        if (strcmp(strTok, param4) == 0)
        {
          position = position + 1;
          subnetFlag = L7_ENABLE;
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfNoRedistribute);
        }
      }
    }
    else   /* too many args for no redistribute */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_OspfNoRedistribute);
    }
  } /*   End of the "No" Command */
    /*   End of validity Checks for Arguments   */

  /*   Setting the value of the parameters */
  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfRedistributionSet(unit, protocol, mode, distList, argMetricValue,
                                     argMetricType,argTagValue, argSubnetValue) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_EnblRouteReDistributionFailure);
      }
    }
  }

  if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (metricFlag == L7_ENABLE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbOspfRedistMetricClear(unit,protocol) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_ClrRedistributionFailure);
        }
      }
    }
    if (metricTypeFlag == L7_ENABLE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbOspfRedistMetricTypeRevert(unit,protocol) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_RevertMetricTypeFailure);
        }
      }
    }

    if (tagFlag == L7_ENABLE)
    {
      argTagValue = 0;
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbOspfTagSet(unit, protocol,
                            FD_OSPF_ASBR_EXT_ROUTE_DEFAULT_ROUTE_TAG) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_RevertTagToDeflFailure);
        }
      }
    }

    if (subnetFlag == L7_ENABLE)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbOspfRedistSubnetsSet(unit,protocol,FD_OSPF_REDIST_SUBNETS) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgFailedToRevert);
        }
      }
    }
  }

  cliSyntaxBottom(ewsContext);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the Default metric for OSPF Route-Redistribution
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
* @notes none
*
* @Mode  Vlan database Config
*
* @cmdsyntax  default-metric <metric-value>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDefaultMetricOSPF(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,numArg;
  L7_uint32 argMetricValue = 1;
  L7_uint32 metricValue = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg =  cliNumFunctionArgsGet();

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfDeflMetric);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argMetricValue], &metricValue) == L7_SUCCESS)
    {
      if ( (metricValue < L7_OSPF_DEFAULT_METRIC_MIN ) || (metricValue > L7_OSPF_DEFAULT_METRIC_MAX))
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_OspfDeflMetricVal);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbOspfDefaultMetricSet(unit,metricValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrErr_common_CfgDeflMetric);
        }
      }
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_RipNoDeflMetric);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfDefaultMetricClear(unit) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrErr_routing_CantResetDeflOspfMetricVal);
      }
    }
  }
  cliSyntaxBottom(ewsContext);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the Filter used for OSPF Route Redistribution
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
* @notes none
*
* @Mode  Vlan database Config
*
* @cmdsyntax  distribute-list <100-199> out  < rip | bgp | static | connected >
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDistributeListOSPF(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,numArg;
  L7_uint32 argFilter = 1, argOut = 2, argProtocol = 3;
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 argFilterValue = 0, filterValue;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg =  cliNumFunctionArgsGet();
  if (numArg != 3)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfDistributeList);
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+argFilter], &argFilterValue) == L7_SUCCESS)
  {
    if ((argFilterValue < L7_ACL_MIN_STD1_ID) || (argFilterValue > L7_ACL_MAX_EXT1_ID))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfDistributeListFilter);
    }
  }

  if (strcmp(argv[index+argOut],pStrInfo_common_AclOutStr) != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfDistributeList);
  }

  if (strcmp(argv[index+argProtocol],pStrInfo_common_Rip3) == 0)
  {
    protocol = REDIST_RT_RIP;
  }
  else if (strcmp(argv[index+argProtocol],pStrInfo_common_Bgp_1) == 0)
  {
    protocol = REDIST_RT_BGP;
  }
  else if (strcmp(argv[index+argProtocol],pStrInfo_common_Static2) == 0)
  {
    protocol = REDIST_RT_STATIC;
  }
  else if (strcmp(argv[index+argProtocol],pStrInfo_common_Connected_1) == 0)
  {
    protocol = REDIST_RT_LOCAL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_OspfDistributeList);
  }

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfDistListSet(unit, protocol, argFilterValue) == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgFilter);
      }
    }
  }
  else
  {
    rc = usmDbOspfDistListGet(unit,protocol,&filterValue);

    if (rc == L7_ERROR)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgNoFilter);
    }

    if (rc == L7_SUCCESS)
    {
      if (filterValue != argFilterValue)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, L7_NULLPTR,  ewsContext, pStrErr_routing_WrongFilterVal);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfDistListClear(unit, protocol) == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfgResetDistribute);
      }
    }
  }
  cliSyntaxBottom(ewsContext);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> nssa
*
* @cmdsyntax for no command: no area <areaid> nssa
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaNSSA(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 extRoutingCapability = 0;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if( !(argc == 3 && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssa);
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssa);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }
  unit = cliGetUnitId();

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      if(usmDbOspfVirtIfCheck(unit, intAreaid) == L7_TRUE)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                               pStrErr_common_CantCreatesStubNssaWithVlink);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfImportAsExternSet(unit, intAreaid, L7_OSPF_AREA_IMPORT_NSSA);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
                                                   ewsContext, pStrErr_common_BackboneCantBeNssa);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,
                                                   ewsContext, pStrErr_common_CantCreatesStubNssa);
          }
        }
      }
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfImportAsExternGet(unit, intAreaid, &extRoutingCapability);
        if (extRoutingCapability == L7_OSPF_AREA_IMPORT_NO_EXT)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,
                                                 ewsContext,
                                                 pStrErr_common_CantDelsStubAreaId,
                                                 intAreaid);
        }

        if ((rc != L7_SUCCESS) || (extRoutingCapability != L7_OSPF_AREA_IMPORT_NSSA))
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDelsStubArea);
        }

        rc = usmDbOspfImportAsExternSet(unit, intAreaid, L7_OSPF_AREA_IMPORT_EXT);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDelsStubNssa);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssa);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose Set ospfv2 interface network type
*
* @param  EwsContext ewsContext
* @param  L7_uint32 argc
* @param  const L7_char8 **argv
* @param  None
*
* @Mode   Interface Config
*
* @cmdsyntax for normal command:    ip ospf network { broadcast | point-to-point }
*
* @cmdsyntax for no command:        no ip ospf network
*
* @cmdhelp
* @cmddescript
* @end
*
***********************************************************************/
const L7_char8 *commandIpOspfNetwork(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32   intIfNum;
  L7_uint32   unit, s, p;
  L7_uint32   numArgs;
  L7_uint32   argNetworkOption=1;
  L7_uint32   ospfInterfaceType = L7_OSPF_INTF_DEFAULT;
  L7_RC_t     rc;
  L7_RC_t status = L7_SUCCESS;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  numArgs = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArgs != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                             pStrErr_routing_IpOspfNetwork);
    }
    else
    {
      if (strcmp(argv[index+argNetworkOption], "broadcast") == 0)
      {
        ospfInterfaceType = L7_OSPF_INTF_BROADCAST;
      }
      else if (strcmp(argv[index+argNetworkOption], "point-to-point") == 0)
      {
        ospfInterfaceType = L7_OSPF_INTF_PTP;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext,
                                                 pStrErr_routing_IpOspfNetwork);
      }
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum) ||
            usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, intIfNum) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        rc = usmDbOspfIfTypeSet(intIfNum, ospfInterfaceType);
        if (rc == L7_FAILURE)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet, ewsContext,
                                                 pStrErr_routing_FailedToSetIpOspfNetwork);
        }
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      for (intIfNum=1; intIfNum < L7_MAX_INTERFACE_COUNT; intIfNum++)
      {
        if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIfNum) ||
            usmDbUnitSlotPortGet(intIfNum, &unit, &s, &p) != L7_SUCCESS)
        {
          continue;
        }

        if ( cliValidateRtrIntf(ewsContext, intIfNum) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
          continue;
        }

        rc = usmDbOspfIfTypeSet(intIfNum, L7_OSPF_INTF_DEFAULT);
        if (rc == L7_FAILURE)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext,
                                                 pStrErr_routing_FailedToRevertIpOspfNetwork);
        }
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> nssa no-summary
*
* @cmdsyntax for no command: no area <areaid> nssa no-summary
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaNSSANoSummary(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit, numArg;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if( !(argc == 4 && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaNoSummary);
  }
  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaNoSummary);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }
  unit = cliGetUnitId();

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfNSSAImportSummariesSet(unit, intAreaid, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_common_BackboneCantBeNssa);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot,
                                                  ewsContext, pStrErr_common_CantSetImportSummaries);
          }
        }
      }

    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfNSSAImportSummariesSet(unit, intAreaid, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetImportSummaries);
        }

      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaNoSummary);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> nssa no-redistribute
*
* @cmdsyntax for no command: no area <areaid> nssa no-redistribute
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaNSSANoRedistribute(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit, numArg;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if( !(argc == 4 && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))) )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaNoRedist);
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaNoRedist);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }
  unit = cliGetUnitId();

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfNSSARedistributeSet(unit, intAreaid, L7_DISABLE);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_common_BackboneCantBeNssa);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot,
                                                  ewsContext, pStrErr_common_CantSetNoRedistribution);
          }
        }
      }
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfNSSARedistributeSet(unit, intAreaid, L7_ENABLE);
        if (rc != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantSetNoRedistribution);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaNoRedist);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> nssa default-info-originate [metric] [metric-type]
*
* @cmdsyntax for no command: no area <areaid> nssa default-info-originate [metric] [metric-type]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaNSSADefaultInfoOriginate(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1,  tempInd = 0;
  L7_uint32 argMetric=4;
  L7_uint32 argMetricType=5;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strMetricType[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit, numArg;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 metric;
  L7_OSPF_STUB_METRIC_TYPE_t metricType = FD_OSPF_NSSA_DEFAULT_METRIC_TYPE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if ( (argc < 4) || (argc > 6) ) /* for NO command argc will be 4 */
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaDeflInfoOr);
  }

  if ((argc == 6) && ( ewsContext->commType == CLI_NORMAL_CMD))
  {
    if ((strlen(argv[tempInd+argMetricType]) >= sizeof(strMetricType)))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaDeflInfoOr);
    }
    OSAPI_STRNCPY_SAFE(strMetricType, argv[tempInd + argMetricType]);
    cliConvertToLowerCase(strMetricType);
  }

  if (argc > 4 )
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[tempInd+argMetric], &metric) == L7_SUCCESS)
    {
      if ( ( metric < L7_OSPF_AREA_NSSA_DEFAULT_METRIC_MIN ) || ( metric > L7_OSPF_AREA_NSSA_DEFAULT_METRIC_MAX ) )
      {
        return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForMetricValMustBeInRangeOfTo,
                                L7_OSPF_AREA_NSSA_DEFAULT_METRIC_MIN, L7_OSPF_AREA_NSSA_DEFAULT_METRIC_MAX);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaDeflInfoOr);
    }

  }
  else
  {
    metric = FD_OSPF_NSSA_DEFAULT_METRIC;
    metricType = FD_OSPF_NSSA_DEFAULT_METRIC_TYPE;
  }

  if (argc == 5)
  {
    metricType = FD_OSPF_NSSA_DEFAULT_METRIC_TYPE;
  }

  if (argc == 6)
  {
    if (strcmp(strMetricType, pStrInfo_common_Comparable_1) == 0)
    {
      metricType = L7_OSPF_AREA_STUB_COMPARABLE_COST;
    }
    else if (strcmp(strMetricType, pStrInfo_common_NonComparable_1))
    {
      metricType = L7_OSPF_AREA_STUB_NON_COMPARABLE_COST;
    }
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaDeflInfoOr);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfNSSADefaultInfoSet(unit, intAreaid, L7_TRUE, metricType, metric);
        if (rc != L7_SUCCESS)
        {
          if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_common_BackboneCantBeNssa);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_common_FailedDefOrigNssa);
          }
        }
      }
    }
    else if( ewsContext->commType == CLI_NO_CMD)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        rc = usmDbOspfNSSADefaultInfoSet(unit, intAreaid, L7_FALSE, metricType, metric);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaDeflInfoOr);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> nssa translator-role <always | candidate>
*
* @cmdsyntax for no command: no area <areaid> nssa translator-role <always | candidate>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaNSSATranslatorRole(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_uint32 argTransRole=4;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strTransRole[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 rc;
  L7_uint32 unit, numArg;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 transRole=0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if(argc != 5 && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans);
  }
  else if(argc != 4 && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoAreaNssaTrans);
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }

  if (strlen(argv[tempInd+argTransRole]) >= sizeof(strTransRole))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans);
  }

  memset (strTransRole, 0,sizeof(strTransRole));
  if(ewsContext->commType == CLI_NO_CMD)
  {
    OSAPI_STRNCPY_SAFE(strTransRole,pStrInfo_common_Candidate_1);
  }
  else
  {
    OSAPI_STRNCPY_SAFE(strTransRole,argv[tempInd+ argTransRole]);
  }

  cliConvertToLowerCase(strTransRole);
  if (strcmp(strTransRole, pStrInfo_common_Always_1) == 0)
  {
    transRole = L7_OSPF_NSSA_TRANSLATOR_ALWAYS;
  }
  else if (strcmp(strTransRole, pStrInfo_common_Candidate_1) == 0)
  {
    transRole = L7_OSPF_NSSA_TRANSLATOR_CANDIDATE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,
                                           ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans);
  }

  unit = cliGetUnitId();

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbOspfNSSATranslatorRoleSet(unit, intAreaid, transRole);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_REQUEST_DENIED)
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_common_BackboneCantBeNssa);
          }
          else
          {
            return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot,
                                                  ewsContext, pStrErr_common_CantSetTrans);
          }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure the area's support for importing AS external link-state advertisements.
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
* @notes none
*
* @Mode  Router (OSPF) Config
*
* @cmdsyntax for normal command: area <areaid> nssa translator-stab-intv <0-3600>
*
* @cmdsyntax for no command: no area <areaid> nssa translator-stab-intv <0-3600>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandAreaNSSATranslatorStabilityInterval(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argAreaid=1, tempInd = 0;
  L7_uint32 argTransStabIntv=4;
  L7_char8 strAreaid[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 intAreaid;
  L7_uint32 intStabIntv;
  L7_uint32 rc;
  L7_uint32 unit, numArg;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /* The command has tokens and parameters randomly in the string. We would need to do some parsing of the command line input rather than using index */

  if(argc != 5 && (ewsContext->commType == CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans_1);
  }
  else if(argc != 4 && (ewsContext->commType == CLI_NO_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfNoAreaNssaTrans_1);
  }

  if (strlen(argv[tempInd+argAreaid]) >= sizeof(strAreaid))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans_1);
  }
  memset (strAreaid, 0, sizeof(strAreaid));
  memset (buf, 0,sizeof(buf));
  OSAPI_STRNCPY_SAFE(strAreaid, argv[tempInd + argAreaid]);
  OSAPI_STRNCPY_SAFE(buf, pStrInfo_common_0000);
  if(strcmp(strAreaid, buf) == 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_InvalidIpAddr_2);
  }

  if(ewsContext->commType == CLI_NO_CMD)
  {
    intStabIntv = L7_OSPF_NSSA_TRANS_STABILITY_INT_DEF;
  }
  else if ( cliConvertTo32BitUnsignedInteger(argv[tempInd+argTransStabIntv], &intStabIntv) == L7_SUCCESS)
  {
    if ( ( intStabIntv < L7_OSPF_NSSA_TRANS_STABILITY_INT_MIN ) || ( intStabIntv > L7_OSPF_NSSA_TRANS_STABILITY_INT_MAX ) )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,  pStrErr_routing_RangeForStabilityIntvlValMustBeInRangeOfTo,
                              L7_OSPF_NSSA_TRANS_STABILITY_INT_MIN, L7_OSPF_NSSA_TRANS_STABILITY_INT_MAX);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans_1);
  }

  unit = cliGetUnitId();

  if (usmDbInetAton(strAreaid, &intAreaid) == L7_SUCCESS)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbOspfNSSATranslatorStabilityIntervalSet(unit, intAreaid, intStabIntv);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_REQUEST_DENIED)
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR,
                                                  ewsContext, pStrErr_common_BackboneCantBeNssa);
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot,
                                                ewsContext, pStrErr_common_CantSetTrans_1);
        }
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_common_CfgRtrOspfAreaNssaTrans_1);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Enable/Disable OSPF MTU mismatch detection on receiving
*             Database Descriptor(DBD) packets
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
* @notes none
*
* @Mode  Interface Config
*
* @cmdsyntax for normal command:  ip ospf mtu-ignore
*
* @cmdsyntax for no command: no ip ospf mtu-ignore
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpOspfMtuIgnore(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 iface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if(!(numArg == 0))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgIpOspfMtuIgnore);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
        continue;
      }

      if (usmDbOspfIntfMtuIgnoreSet(unit, iface, L7_TRUE) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantDsblIpOspfMtuIgnore);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    for (iface=1; iface < L7_MAX_INTERFACE_COUNT; iface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), iface) ||
          usmDbUnitSlotPortGet(iface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( cliValidateRtrIntf(ewsContext, iface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
        continue;
      }

      if (usmDbOspfIntfMtuIgnoreSet(unit, iface, L7_FALSE) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantEnblIpOspfMtuIgnore);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgIpOspfMtuIgnore);
  }

  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}

#ifdef L7_NSF_PACKAGE

/*********************************************************************
*
* @purpose    Enable/Disable OSPFv2 graceful restart
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
* @notes none
*
* @Mode  OSPFv2 Router Config
*
* @cmdsyntax for normal command:  nsf [ietf] [ planned-only ]
*
* @cmdsyntax for no command: no nsf [ietf]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandOspfNsf(EwsContext ewsContext, L7_uint32 argc, 
                               const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 argOption = 1;
  OSPF_NSF_SUPPORT_t nsfSupport = OSPF_NSF_NONE;
  L7_uchar8 *syntax = "Use 'nsf [ planned-only ]'.";
  L7_uchar8 *noSyntax = "Use 'no nsf'.";

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, syntax);
    }
    nsfSupport = OSPF_NSF_ALWAYS;
    if (numArg && (strcmp(argv[index + argOption], "planned-only") == 0))
    {
      nsfSupport = OSPF_NSF_PLANNED_ONLY;
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfNsfSupportSet(nsfSupport) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "configure graceful restart");
      }
    }
  }

  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, noSyntax);
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfNsfSupportSet(nsfSupport) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "disable graceful restart");
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, syntax);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configure OSPFv2 graceful restart grace period
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
* @notes none
*
* @Mode  OSPFv2 Router Config
*
* @cmdsyntax for normal command:  nsf [ietf] restart-interval <seconds>
*
* @cmdsyntax for no command: no nsf [ietf] restart-interval 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandNsfRestartInterval(EwsContext ewsContext, L7_uint32 argc, 
                                          const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 argInterval = 1;
  L7_uint32 seconds;
  L7_uint32 numArg;
  L7_uchar8 syntax[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 *noSyntax = "Use 'no nsf restart-interval'.";

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    osapiSnprintf(syntax, L7_CLI_MAX_STRING_LENGTH, 
                  "Use 'nsf restart-interval <%u-%u>'.", 
                  L7_OSPF_RESTART_INTERVAL_MIN, L7_OSPF_RESTART_INTERVAL_MAX);
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, syntax);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index + argInterval], &seconds) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, syntax);
    }
    if ((seconds < L7_OSPF_RESTART_INTERVAL_MIN ) || (seconds > L7_OSPF_RESTART_INTERVAL_MAX ))
    { 
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, L7_NULLPTR, ewsContext, 
                                              "Value out of range [%u, %u]",
                                              L7_OSPF_RESTART_INTERVAL_MIN, 
                                              L7_OSPF_RESTART_INTERVAL_MAX);
    }

    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfRestartIntervalSet(seconds) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, 
                                              ewsContext, "set the restart interval");
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, noSyntax);
    }
    /*******Check if the Flag is Set for Execution*************/
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfRestartIntervalSet(FD_OSPF_RESTART_INTERVAL) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, 
                                              ewsContext, 
                                              "set the restart interval");
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

#else
/* Make compiler happy */
const L7_char8 *commandOspfNsf(EwsContext ewsContext, L7_uint32 argc, 
                               const L7_char8 **argv, L7_uint32 index)
{
  return cliPrompt(ewsContext);
}
#endif

/*********************************************************************
*
* @purpose    Configure OSPFv2 graceful restart helper status
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
* @notes none
*
* @Mode  OSPFv2 Router Config
*
* @cmdsyntax for normal command:  nsf [ietf] helper [ planned-only ]
*
* @cmdsyntax for no command: no nsf [ietf] helper 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandNsfHelper(EwsContext ewsContext, L7_uint32 argc, 
                                 const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 argOption = 1;
  OSPF_NSF_SUPPORT_t nsfSupport = OSPF_NSF_NONE;
  L7_uchar8 *syntax = "Use 'nsf helper [ planned-only ]'.";
  L7_uchar8 *noSyntax = "Use 'no nsf helper'.";

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 1)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, syntax);
    }

    nsfSupport = OSPF_NSF_ALWAYS;
    if ((numArg == 1) && (strcmp(argv[index + argOption], "planned-only") == 0))
    {
      nsfSupport = OSPF_NSF_PLANNED_ONLY;
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfHelperSupportSet(nsfSupport) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "configure graceful restart helper");
      }
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, noSyntax);
    }
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfHelperSupportSet(nsfSupport) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "disable graceful restart helper");
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, syntax);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Disable OSPFv2 graceful restart helper 
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
* @notes none
*
* @Mode  OSPFv2 Router Config
*
* @cmdsyntax for normal command:  nsf ietf helper disable
*
* @cmdsyntax for no command:      N/A
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandNsfHelperDisable(EwsContext ewsContext, L7_uint32 argc, 
                                        const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uchar8 *syntax = "Use 'nsf ietf helper disable'.";

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg)
    {
      return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                            ewsContext, syntax);
    }

    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfHelperSupportSet(OSPF_NSF_NONE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "disable graceful restart helper");
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, syntax);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configure OSPFv2 graceful restart option to terminate helper status
*             when the helper detects a topology change.
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
* @notes none
*
* @Mode  OSPFv2 Router Config
*
* @cmdsyntax for normal command:  nsf [ietf] helper strict-lsa-checking
*
* @cmdsyntax for no command: no nsf [ietf] helper strict-lsa-checking 
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandNsfHelperStrictLsaChecking(EwsContext ewsContext, L7_uint32 argc, 
                                                  const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uchar8 *syntax = "Use 'nsf helper strict-lsa-checking'.";

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg != 0)
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, syntax);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfStrictLsaCheckingSet(L7_TRUE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "configure graceful restart strict LSA checking");
      }
    }
  }

  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbOspfStrictLsaCheckingSet(L7_FALSE) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_CouldNot, ewsContext, 
                                              "disable graceful restart strict LSA checking");
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks(1, 1, 0, 0, pStrErr_common_IncorrectInput,  
                                          ewsContext, syntax);
  }

  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}



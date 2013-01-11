/*********************************************************************
 * LL   VV  VV LL   7   77   All Rights Reserved.
 * LL   VV  VV LL      77
 * LL    VVVV  LL     77
 * LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
 * </pre>
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/rip/cli_config_rip.c
 *
 * @purpose config commands for the RIP
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
#include "usmdb_ip6_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_iputil_api.h"
#include "usmdb_rtrdisc_api.h"
#include "clicommands_l3.h"
#include "usmdb_mib_ripv2_api.h"
#include "usmdb_mib_relay_api.h"
#include "l3_commdefs.h"
#include "clicommands_card.h"
#include  "cli_config_script.h"

L7_RC_t compareMatchOptions(const L7_char8 * *, L7_uint32, L7_uint32, L7_uint32 *, L7_uint32 *);

/*********************************************************************
*
* @purpose    Configures the administrative mode of RIP in the router to active or inactive
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
* @Mode  Router (RIP) Config
*
* @cmdsyntax for normal command:  enable
*
* @cmdsyntax for no command: no enable
*
* @cmdhelp
*
* @cmddescript   usmDbRip2AdminModeSet
*
*@However, by default, Cisco OSPF is enabled and FP OSPF is disabled. LVL7
*@will change the default OSPF to be enabled. So, since the default will be
*@enabled, the following command makes the most sense:
*@    router rip
*@    router# disable
*@    router# no disable
*
* @end
*
*********************************************************************/
const L7_char8 *commandEnableRIP(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;

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

  if ( numArg!= 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipAdminMode);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbRip2AdminModeSet(unit,  L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRipAdminMode);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (  usmDbRip2AdminModeSet(unit,  L7_DISABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRipAdminMode);
      }
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipAdminMode);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Enables or disables RIP autosummary
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
* @Mode  Router (RIP) Config
*
* @cmdsyntax for normal command:  auto-summary
*
* @cmdsyntax for no command: no auto-summary
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRIPAutoSummary(EwsContext ewsContext, L7_uint32 argc,
                                      const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 mode;
  L7_uint32 unit;

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

  if ( numArg!= 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipAutoSummary_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipAutoSummary_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbRip2AutoSummarizationModeSet(unit,  mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgRipAutoSummary);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Enables or disables RIP host routes accept
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
* @Mode  Router (RIP) Config
*
* @cmdsyntax for normal command:  hostroutesaccept
*
* @cmdsyntax for no command: no hostroutesaccept
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRIPHostRoutesAccept(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 mode;
  L7_uint32 unit;
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

  if ( numArg!= 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipHostRoutesAccept_1);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipHostRoutesAccept_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbRip2HostRoutesAcceptModeSet(unit,  mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgRipHostRoutesAccept);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures RIP split horizon mode
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
* @Mode  Router (RIP) Config
*
* @cmdsyntax for normal command:  split-horizon <none/simple/poison>
*
* @cmdsyntax for no command: no split-horizon
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRIPSplitHorizon(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 1;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 mode = 0;
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if ( ((numArg != 1) && (ewsContext->commType == CLI_NORMAL_CMD)) ||
      ((numArg != 0) && (ewsContext->commType == CLI_NO_CMD)) )
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipSplitHorizon);
    }
    if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipNoSplitHorizon);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(strlen(argv[index+argMode]) < sizeof(strMode))
    {
      OSAPI_STRNCPY_SAFE(strMode,argv[index+ argMode]);
      cliConvertToLowerCase(strMode);
      if (strcmp(strMode, pStrInfo_common_None_3) == 0)
      {                                     /* none */
        mode = L7_RIP_SPLITHORIZON_NONE;
      }
      else if (strcmp(strMode, pStrInfo_common_Simple_2) == 0)
      {                                            /* simple */
        mode = L7_RIP_SPLITHORIZON_SIMPLE;
      }
      else if (strcmp(strMode, pStrInfo_routing_Poison) == 0)
      {                                          /* poison */
        mode = L7_RIP_SPLITHORIZON_POISONREVERSE;
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipSplitHorizon);
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipSplitHorizon);
    }
  }

  else
  {
    mode = L7_RIP_SPLITHORIZON_SIMPLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbRip2SplitHorizonModeSet(unit,  mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_CfgRipSplitHorizon);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the RIP Version 2 Authentication Type to none for the specified interface
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
* @cmdsyntax for normal command:  ip rip authentication none
*
* @cmdsyntax for no command: no ip rip authentication
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRIPAuthenticationNone(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 1;
  L7_uint32 tmpInd = 2;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if( !((argc == 4 && (ewsContext->commType == CLI_NORMAL_CMD)) || (argc == 3 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if( ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfAuthNone);
    }
    if( ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfAuth);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if( ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(strlen(argv[tmpInd+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfAuthNone);
    }
    OSAPI_STRNCPY_SAFE(strMode,argv[tmpInd+ argMode]);
    cliConvertToLowerCase(strMode);

    if (strcmp(strMode, pStrInfo_common_None_3) != 0)  /* none */
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfAuthNone);
    }
  }

  if(commandRIPAuthentication( ewsContext, argv, tmpInd, L7_AUTH_TYPE_NONE, L7_NULL, 0 ) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the RIP Version 2 Authentication Type to simple for the specified interface
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
* @cmdsyntax for normal command:  ip rip authentication simple <key>
*
* @cmdsyntax for no command: no ip rip authentication
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRIPAuthenticationSimple(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argKey = 1;
  L7_uchar8 strKey[L7_AUTH_MAX_KEY_RIP+1];

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if((numArg != 1) || (ewsContext->commType != CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfAuthSimple);
  }

  if (strlen(argv[index+argKey]) > (sizeof(strKey)-1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRipAuthKey, L7_AUTH_MAX_KEY_RIP);

  }

  OSAPI_STRNCPY_SAFE(strKey, argv[index + argKey]);

  if(commandRIPAuthentication( ewsContext, argv, index,L7_AUTH_TYPE_SIMPLE_PASSWORD, strKey, 0 ) != L7_SUCCESS)
  {
    return cliPrompt(ewsContext);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Configures the RIP Version 2 Authentication Type to encrypt for the specified interface
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
* @cmdsyntax for normal command:  ip rip authentication encrypt <key> <keyid>
*
* @cmdsyntax for no command: no ip rip authentication
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRIPAuthenticationEncrypt(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argKey = 1;
  L7_uint32 argKeyId = 2;
  L7_uchar8 strKey[L7_AUTH_MAX_KEY_RIP+1];
  L7_uint32 keyId;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if ((numArg != 2 ) || (ewsContext->commType != CLI_NORMAL_CMD))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfAuthEncrypt);
  }

  if (strlen(argv[index+argKey]) > (sizeof(strKey)-1))
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_common_CfgRipAuthKey, L7_AUTH_MAX_KEY_RIP);
  }

  OSAPI_STRNCPY_SAFE(strKey, argv[index + argKey]);

  if (cliConvertTo32BitUnsignedInteger(argv[index+argKeyId], &keyId) == L7_SUCCESS &&
      keyId <= L7_RIP_CONF_AUTH_KEY_ID_MAX && keyId >= L7_RIP_CONF_AUTH_KEY_ID_MIN)
  {
    /* Callee will display error messages */
    if(commandRIPAuthentication( ewsContext, argv, index,L7_AUTH_TYPE_MD5, strKey, keyId ) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,  pStrErr_routing_CfgRipAuthKeyId, L7_RIP_CONF_AUTH_KEY_ID_MIN, L7_RIP_CONF_AUTH_KEY_ID_MAX);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Utility routine for configuring RIP authentication
*
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype L7_RC_t
*
* @returns cliPrompt(ewsContext)
*
* @notes Called by 3 RIP authentication commands
*
* @end
*
*********************************************************************/
L7_RC_t commandRIPAuthentication(EwsContext ewsContext,
                                 const L7_char8 * * argv,
                                 L7_uint32 index,
                                 L7_uint32 intType,
                                 L7_uchar8 * strKey,
                                 L7_uint32 keyId )
{
  L7_uint32 intOldType, oldKeyId;
  L7_uchar8 oldKey[L7_AUTH_MAX_KEY_RIP+1];
  L7_uint32 iface, s, p;
  L7_uint32 rc = L7_FAILURE, rc2 = L7_FAILURE;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_RC_t status = L7_SUCCESS;

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

      /* must set auth type before key */
      rc2 = usmDbRip2IfConfAuthenticationGet(unit, iface, &intOldType, oldKey, &oldKeyId);   /* For restoration purposes */
  
      if ( (rc = usmDbRip2IfConfAuthTypeSet(unit, iface, intType)) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrInfo_routing_CfgAuthTypeFail);
      }
      else
      {
        if ( (rc = usmDbRip2IfConfAuthKeySet(unit, iface, strKey)) != L7_SUCCESS )
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, buf, sizeof(buf),  pStrInfo_routing_CfgAuthKeyFail, L7_AUTH_MAX_KEY_RIP);
          ewsTelnetWrite( ewsContext, buf);
        }
        else
        {
          if ( (rc = usmDbRip2IfConfAuthKeyIdSet(unit, iface, keyId)) != L7_SUCCESS )
          {
            status = L7_FAILURE;
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
            osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, buf, sizeof(buf),  pStrInfo_routing_CfgAuthKeyIdFail, L7_RIP_CONF_AUTH_KEY_ID_MIN, L7_RIP_CONF_AUTH_KEY_ID_MAX);
            ewsTelnetWrite( ewsContext, buf);
          }
        }
      }
  
      if ((rc != L7_SUCCESS) && (rc2 == L7_SUCCESS))
      {
        /* restore original authentication information */
        if ( usmDbRip2IfConfAuthenticationSet(unit, iface, intOldType, oldKey, oldKeyId) != L7_SUCCESS)
        {
          status = L7_FAILURE;
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_routing_CfgAuthRestoreFail);
        }
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
* @cmdsyntax for normal command:  default-information originate
*
* @cmdsyntax for no command: no default-information originate
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDefaultInformationOriginateRip(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  unit = cliGetUnitId();

  if( !(numArg == 0 ))
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfDeflMetric);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipNoIntfDeflMetric);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbRip2DefaultRouteAdvertiseModeSet(unit, L7_ENABLE) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefMetric);
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbRip2DefaultRouteAdvertiseModeClear(unit) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefMetric);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);

}

/*********************************************************************
*
* @purpose    Enables or disables RIP on a router's interface
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
* @cmdsyntax for normal command:  ip rip
*
* @cmdsyntax for no command: no ip rip
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRIP(EwsContext ewsContext, L7_uint32 argc,
                             const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 intMode;
  L7_uint32 intIface, s, p;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !(numArg == 0 && ((ewsContext->commType == CLI_NORMAL_CMD) || (ewsContext->commType == CLI_NO_CMD))))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfMode);
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    intMode = L7_ENABLE;
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    intMode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfMode);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIface=1; intIface < L7_MAX_INTERFACE_COUNT; intIface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIface) ||
          usmDbUnitSlotPortGet(intIface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
        continue;
      }
      if (usmDbRip2IfStatStatusSet(unit, intIface, intMode) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRtrRipIfMode);
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
* @purpose    Configures the interface to allow RIP control packets of the specified version(s) to be received
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
* @cmdsyntax for normal command:  ip rip receive version  <rip1|rip2|rip1or2|none>
*
* @cmdsyntax for no command: no ip rip version receive
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRIPReceiveVersion(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 1;
  L7_uint32 intMode = 0;
  L7_uint32 intIface, s, p;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
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
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfVerReceive);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipNoIntfVerReceive);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfVerReceive);
    }

    OSAPI_STRNCPY_SAFE(strMode,argv[index+ argMode]);
    cliConvertToLowerCase(strMode);
    if (strcmp(strMode, pStrInfo_routing_Rip12) == 0)
    {
      intMode = L7_RIP_CONF_RECV_RIPV1;
    }
    else if (strcmp(strMode, pStrInfo_routing_Rip2_1) == 0)
    {
      intMode = L7_RIP_CONF_RECV_RIPV2;
    }
    else if (strcmp(strMode, pStrInfo_common_Both_1) == 0)
    {
      intMode = L7_RIP_CONF_RECV_RIPV1_RIPV2;
    }
    else if (strcmp(strMode, pStrInfo_common_None_3) == 0)
    {
      intMode = L7_RIP_CONF_RECV_DO_NOT_RECEIVE;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfVerReceive);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    intMode = L7_RIP_CONF_RECV_RIPV1_RIPV2;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIface=1; intIface < L7_MAX_INTERFACE_COUNT; intIface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIface) ||
          usmDbUnitSlotPortGet(intIface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
        continue;
      }
      if ( usmDbRip2IfConfReceiveSet(unit, intIface, intMode) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRipIf);
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
* @purpose    Configures the interface to allow RIP control packets of the specified version to be sent
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
* @cmdsyntax for normal command:  ip rip send version  <rip1|rip1c|rip2|none>
*
* @cmdsyntax for no command: no ip rip send version
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandIpRIPSendVersion(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argMode = 1;
  L7_uint32 intMode = 0;
  L7_uint32 intIface, s, p;
  L7_char8 strMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  L7_RC_t status = L7_SUCCESS;

  L7_uint32 numArg;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if( !((numArg == 1 && (ewsContext->commType == CLI_NORMAL_CMD)) || (numArg == 0 && (ewsContext->commType == CLI_NO_CMD))) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfVerSend_1);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipNoIntfVerSend);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (strlen(argv[index+argMode]) >= sizeof(strMode))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfVerSend_1);
    }

    OSAPI_STRNCPY_SAFE(strMode,argv[index+ argMode]);
    cliConvertToLowerCase(strMode);
    if (strcmp(strMode, pStrInfo_routing_Rip12) == 0)
    {
      intMode =  L7_RIP_CONF_SEND_RIPV1;
    }
    else if (strcmp(strMode, pStrInfo_routing_Rip1c2) == 0)
    {
      intMode =  L7_RIP_CONF_SEND_RIPV1_COMPAT;
    }
    else if (strcmp(strMode, pStrInfo_routing_Rip2_1) == 0)
    {
      intMode = L7_RIP_CONF_SEND_RIPV2;
    }
    else if (strcmp(strMode, pStrInfo_common_None_3) == 0)
    {
      intMode = L7_RIP_CONF_SEND_DO_NOT_SEND;
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrRipIntfVerSend_1);
    }
  }

  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /* Changing to default value */
    intMode =  FD_RIP_INTF_DEFAULT_VER_SEND;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    for (intIface=1; intIface < L7_MAX_INTERFACE_COUNT; intIface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), intIface) ||
          usmDbUnitSlotPortGet(intIface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }

      if ( cliValidateRtrIntf(ewsContext, intIface) != L7_SUCCESS )
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_common_CantGetValidIntf);
        continue;
      }
      if ( usmDbRip2IfConfSendSet(unit, intIface, intMode) != L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWriteAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRipIf_1);
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
* @purpose    Configures the preferences of RIP Routes
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
* @Mode  Router (RIP) Config
*
* @cmdsyntax for normal command:  distance rip <0-255>
*
* @cmdsyntax for no command: no distance rip
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandDistanceRIP(EwsContext ewsContext, L7_uint32 argc,
                                   const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argPref = 1;
  L7_uint32 intPref;
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_char8 cliNormalComSyntax[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;

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

  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, cliNormalComSyntax, sizeof(cliNormalComSyntax),  pStrErr_routing_CfgRtrRipDistance,
                          L7_RTO_PREFERENCE_MIN, L7_RTO_PREFERENCE_MAX);

  if ( ((numArg != 1) && (ewsContext->commType == CLI_NORMAL_CMD)) ||
      ((numArg != 0) && (ewsContext->commType == CLI_NO_CMD)) )
  {
    if(  ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite( ewsContext, cliNormalComSyntax);
    }
    if(  ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_CfgRtrNoRipDistance);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if(  ewsContext->commType == CLI_NORMAL_CMD)
  {
    if ( cliConvertTo32BitUnsignedInteger(argv[index+argPref], &intPref) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, cliNormalComSyntax);
    }

    if (intPref > L7_RTO_PREFERENCE_MAX || intPref < L7_RTO_PREFERENCE_MIN)
    {
      ewsTelnetWrite(ewsContext, cliNormalComSyntax);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if ((rc = usmDbIpRouterPreferenceSet(unit, ROUTE_PREF_RIP, intPref)) != L7_SUCCESS)
        {
          if (rc == L7_ALREADY_CONFIGURED)
          {
            ewsTelnetPrintf (ewsContext, "\r\n%s %s\r\n%s", pStrErr_common_CouldNot,
                          pStrErr_routing_CantSetRipPref, pStrInfo_common_PrefValAlreadyInUse);
            return cliPrompt(ewsContext);
          }
          else
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetRipPref);
            return cliPrompt(ewsContext);
          }
        }
      }
    }
  }
  else if( ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if ( usmDbIpRouterPreferenceSet(unit, ROUTE_PREF_RIP, L7_RTO_RIP_PREFERENCE_DEF) != L7_SUCCESS )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_CouldNot,  ewsContext, pStrErr_routing_CantSetDefRipPref);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures RIP Route-Redistribution
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
* @cmdsyntax  redistribute <ospf> [metric <metric-value>]
*        [match <internal> | <external 1> | <external 2> | <nssa-external 1> | <nssa-external 2>
*        or
*           redistribute < bgp | static | connected> [metric <metric-value>]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRedistributeRIP(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,numArg;
  L7_uint32 argProtocol = 1;
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 mode, argMetricValue = 0, argMatchType = 0;
  L7_uint32 metricFlag = L7_DISABLE, matchFlag = L7_DISABLE;
  L7_uint32 position, newPosition;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg =  cliNumFunctionArgsGet();

  if (numArg < 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipRedistribute_1);
  }

  if (strcmp(argv[index+argProtocol], pStrInfo_common_Ospf_1) == 0)
  {
    protocol = REDIST_RT_OSPF;
  }
  else if (strcmp(argv[index+argProtocol], pStrInfo_common_Bgp_1) == 0)
  {
    protocol = REDIST_RT_BGP;
  }
  else if (strcmp(argv[index+argProtocol], pStrInfo_common_Static2) == 0)
  {
    protocol = REDIST_RT_STATIC;
  }
  else if (strcmp(argv[index+argProtocol], pStrInfo_common_Connected_1) == 0)
  {
    protocol = REDIST_RT_LOCAL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_RipProto);
  }

  /* Parse the metric and the match options. */
  position = 2;
  while (position <= numArg)
  {
    if ((strcmp(argv[index+position], pStrInfo_common_Metric_1) == 0) && (metricFlag == L7_DISABLE))
    {
      position = position+1;
      if (ewsContext->commType ==  CLI_NORMAL_CMD)
      {

        if (cliConvertTo32BitUnsignedInteger(argv[index+position], &argMetricValue) == L7_SUCCESS)
        {
          if ((argMetricValue < L7_RIP_CONF_METRIC_MIN) || (argMetricValue > L7_RIP_CONF_METRIC_MAX))
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipMetricVal);
          }
          else
          {
            metricFlag = L7_ENABLE;
            position = position+1;
          }
        }
        else
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipMetricVal);
        }
      }
      else if (ewsContext->commType ==  CLI_NO_CMD)
      {
        metricFlag = L7_ENABLE;
      }
    }
    else if ((protocol == REDIST_RT_OSPF) && (strcmp(argv[index+position], pStrInfo_common_Match_1) == 0) &&
             (matchFlag == L7_DISABLE))
    {
      /* Check for Validity of Match options */
      newPosition = 0;
      if (compareMatchOptions(argv, numArg, position, &argMatchType, &newPosition) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_WrongMatchOptions);
      }
      else
      {
        matchFlag = L7_ENABLE;
        position = newPosition;
      }
    }
    else
    {
      /* If it ever comes here, it means, the argument is something other than
       * metric and match, or, we already found a metric and match and this is a
       * repeat of the same, which is wrong
       */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipRedistribute_1);
    }
  }

  /* Now we have all the options parsed. */
  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    if ((metricFlag == L7_ENABLE) && (matchFlag != L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeMetricSet(unit,protocol,L7_ENABLE,argMetricValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_MetricsSetError);
        }
      }
    }
    else if ((metricFlag != L7_ENABLE) && (matchFlag == L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeMatchTypeSet(unit,protocol,L7_ENABLE,argMatchType) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_MatchOptionsSetError);
        }
      }
    }
    else if ((metricFlag == L7_ENABLE) && (matchFlag == L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeParmsSet(unit,protocol,L7_ENABLE,argMetricValue,argMatchType) != L7_SUCCESS)
        {

          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_MatchAndMetricsSetError);
        }
      }
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRip2RouteRedistributeModeSet(unit, protocol, L7_ENABLE) == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_RedistributeModeSetError);
      }
    }
  }   /* End of the "Normal" Command   */

  if (ewsContext->commType ==  CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    if ((metricFlag != L7_ENABLE) && (matchFlag != L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeModeSet(unit, protocol, L7_DISABLE) == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_RedistributeModeDsblError);
        }
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeParmsClear(unit, protocol) == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_routing_RedistributeMatchResetError);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RRouteRedistributeFilterClear(unit, protocol) == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfgResetDistribute);
        }
      }
    }
    else if ((metricFlag != L7_ENABLE) && (matchFlag == L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeMatchTypeSet(unit,protocol,L7_DISABLE,argMatchType) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_DsblMatchOptionError);
        }
      }
    }
    else if ((metricFlag == L7_ENABLE) && (matchFlag != L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeMetricSet(unit,protocol,L7_DISABLE,argMetricValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_DsblMetricOptionError);
        }
      }
    }
    else if ((metricFlag == L7_ENABLE) && (matchFlag == L7_ENABLE))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2RouteRedistributeParmsSet(unit,protocol,L7_DISABLE,argMetricValue,argMatchType) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_routing_MatchAndMetricDsblError);
        }
      }
    }
    else
    {
      /* It should never come here. Just in case to catch all other errors.
       */
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipRedistribute_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the Default metric for RIP Route-Redistribution
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
const L7_char8 *commandDefaultMetricRIP(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,numArg;
  L7_uint32 argMetricValue = 1;
  L7_uint32 mode, metricValue = 0;
  L7_char8 cliNormalComSyntax[L7_CLI_MAX_STRING_LENGTH];

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg =  cliNumFunctionArgsGet();

  osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput, cliNormalComSyntax, sizeof(cliNormalComSyntax),  pStrErr_routing_RipDeflMetric_1,
                          L7_RIP_CONF_METRIC_MIN, L7_RIP_CONF_METRIC_MAX);

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, cliNormalComSyntax);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+argMetricValue], &metricValue) == L7_SUCCESS)
    {
      if ( (metricValue < L7_RIP_CONF_METRIC_MIN ) || (metricValue > L7_RIP_CONF_METRIC_MAX))
      {
        return cliSyntaxReturnPrompt (ewsContext, cliNormalComSyntax);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRip2DefaultMetricSet(unit,mode,metricValue) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrErr_routing_CantSetDeflMetricValForRip);
        }
      }
    }
  }
  else if (ewsContext->commType ==  CLI_NO_CMD)
  {
    mode = L7_DISABLE;
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_RipNoDeflMetric);
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRip2DefaultMetricSet(unit,mode,metricValue) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Cant,  ewsContext, pStrErr_routing_CantResetDeflMetricValForRip);
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
* @purpose  Configures the Filter used for RIP Route Redistribution
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
* @cmdsyntax  distribute-list <100-199> out  < ospf | bgp | static | connected >
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandDistributeListRIP(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 unit,numArg;
  L7_uint32 argFilter = 1, argOut = 2, argProtocol = 3;
  L7_REDIST_RT_INDICES_t protocol;
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 argFilterValue = 0, filterValue;
  L7_RC_t rc;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* validity check (arguments, Access */
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  numArg =  cliNumFunctionArgsGet();
  if (numArg != 3)
  {
    if (ewsContext->commType ==  CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipDistributeList);
    }
    else
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipNoDistributeList);
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cliConvertTo32BitUnsignedInteger(argv[index+argFilter], &argFilterValue) == L7_SUCCESS)
  {
    if ((argFilterValue < L7_ACL_MIN_STD1_ID) || (argFilterValue > L7_ACL_MAX_EXT1_ID))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipDistributeListFilter);
    }
  }

  if (strcmp(argv[index+argOut],pStrInfo_common_AclOutStr) != 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipDistributeList);
  }

  if (strcmp(argv[index+argProtocol],pStrInfo_common_Ospf_1) == 0)
  {
    protocol = REDIST_RT_OSPF;
  }
  else if (strcmp(argv[index+argProtocol],pStrInfo_common_Bgp_1) == 0)
  {
    protocol = REDIST_RT_BGP;
  }
  else if (strcmp(argv[index+argProtocol],pStrInfo_common_Static2) == 0)
  {
    protocol = REDIST_RT_STATIC;
  }
  else if (strcmp(argv[index+argProtocol], pStrInfo_common_Connected_1) == 0)
  {
    protocol = REDIST_RT_LOCAL;
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_routing_RipDistributeList);
  }

  if (ewsContext->commType ==  CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRip2RouteRedistributeFilterSet(unit, protocol, mode, argFilterValue) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_CfgFilter);
      }
    }
  }
  else
  {
    mode = L7_DISABLE;

    rc = usmDbRip2RouteRedistributeFilterGet(unit,protocol,&filterValue);
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
      if (usmDbRip2RRouteRedistributeFilterClear(unit, protocol) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_common_CfgResetDistribute);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}



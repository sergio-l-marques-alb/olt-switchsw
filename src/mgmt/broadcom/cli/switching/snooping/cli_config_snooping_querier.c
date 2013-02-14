/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_config_snooping_querier.c
 *
 * @purpose Snooping Querier Commands
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
#include "ews.h"
#include "osapi_support.h"
#include "clicommands_snooping_querier.h"
#include "usmdb_snooping_api.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"

/*********************************************************************
*
* @purpose  Configures the Snooping Querier Administrative Mode,
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
* @cmdsyntax    [no] set igmp querier
* @cmdsyntax    [no] set mld querier
*
* @cmdhelp
*
* @cmddescript  This command enables and disables Snooping Querier
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerier(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 numArg;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADMINMODE(family));
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierAdminModeSet(L7_ENABLE, family);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADMINMODE_NO(family));
    }
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierAdminModeSet(L7_DISABLE, family);
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADMINMODE(family));
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  
                                             ewsContext, CLIERROR_SET_SNOOPING_QUERIER_ADMIN_MODE(family));
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose  Configures the Snooping Querier address
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
* @cmdsyntax    [no] set igmp querier address <ip-address>
* @cmdsyntax    [no] set mld querier address <ip-address>
*
* @cmdhelp
*
* @cmddescript  This command sets and clears the snooping querier address.
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierAddress(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 numArg, argIpaddr = 1;
  L7_uchar8 family = L7_AF_INET;
  L7_uchar8 ipAddr[L7_IP6_ADDR_LEN];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ip4addr = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADDRESS(family));
    }

    /* Extract the ip address */
    if (strlen(argv[index+argIpaddr]) >= sizeof(strIpAddr))
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADDRESS(family));
    }

    osapiStrncpySafe(strIpAddr, argv[index + argIpaddr], sizeof(strIpAddr));
    if (osapiInetPton(family, strIpAddr, ipAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGQUERIERIP(family));
    }

    if(family == L7_AF_INET && 
       (usmDbInetAton(strIpAddr, &ip4addr) != L7_SUCCESS))
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGQUERIERIP(family));
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierAddressSet(ipAddr, family);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADDRESS_NO(family));
    }
    memset(ipAddr, 0, L7_IP6_ADDR_LEN);
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierAddressSet(ipAddr, family);
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_ADDRESS(family));
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    { 
      if (rc == L7_NOT_SUPPORTED || rc == L7_ERROR)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                                               pStrErr_common_IncorrectInput,  
                                               ewsContext, 
                                               (family == L7_AF_INET) ?
                                                pStrErr_switching_UseSnoopingQuerierVlanAddr :
                                                pStrErr_switching_UseSnoopingQuerierVlanAddr6);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                                               L7_NULLPTR, 
                                               ewsContext, 
                                               pStrErr_switching_SetSnoopingQuerierAddr);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the Snooping Querier periodic query interval
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
* @cmdsyntax    [no] set igmp querier query-interval
* @cmdsyntax    [no] set mld querier query-interval
*
* @cmdhelp
*
* @cmddescript  This command modifies the querier's periodic query
*               interval.
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierQueryInterval(EwsContext ewsContext, L7_uint32 argc,
                                                       const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 interval, argQry = 1;
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERYTIME(family));
    }

    if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argQry],
                                         &interval) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierQueryIntervalSet(interval, family);
    }
  }/* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERYTIME_NO(family));
    }

    interval = (family == L7_AF_INET) ? FD_IGMP_SNOOPING_QUERIER_QUERY_INTERVAL
               : FD_MLD_SNOOPING_QUERIER_QUERY_INTERVAL;
    /*******Check if the Flag is Set for Execution*************/
    rc = usmDbSnoopQuerierQueryIntervalSet(interval, family);
  }/* END OF NO COMMAND */
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERYTIME(family));
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_SnoopQuerierQueryTimeSet);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Configures the Snooping Querier other querier expiry time
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
* @cmdsyntax   [no] set igmp querier timer expiry
* @cmdsyntax   [no] set mld querier timer expiry
*
* @cmdhelp
*
* @cmddescript  This command modifies the querier's last querier expiry
*               interval.
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierExpiryInterval(EwsContext ewsContext, L7_uint32 argc,
                                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 interval, argQry = 1;
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERIERTIME(family));
    }

    if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argQry],
                                         &interval) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierExpiryIntervalSet(interval, family);
    }
  }/* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERIERTIME_NO(family));
    }

    interval = (family == L7_AF_INET) ? FD_IGMP_SNOOPING_QUERIER_EXPIRY_INTERVAL
               : FD_MLD_SNOOPING_QUERIER_EXPIRY_INTERVAL;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierExpiryIntervalSet(interval, family);
    }
  }/* END OF NO COMMAND */
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERIERTIME(family));
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_switching_SnoopQuerierQuerierTimeSet);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Configures the Snooping Querier mutlicast protocol version
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
* @cmdsyntax    [no] set igmp querier version
*
* @cmdhelp
*
* @cmddescript  This command configures igmp version to be used by
*               the snooping querier
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierVersion(EwsContext ewsContext, L7_uint32 argc,
                                                 const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 version, argVer = 1;
  L7_uchar8 family = L7_AF_INET;
  L7_RC_t rc = L7_FAILURE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERIERVERSION(family));
    }

    if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argVer],
                                         &version) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVersionSet(version, family);
    }
  }/* END OF NORMAL COMMAND */
  else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERIERVERSION_NO(family));
    }

    version = (family == L7_AF_INET) ? FD_IGMP_SNOOPING_QUERIER_VERSION
              : FD_MLD_SNOOPING_QUERIER_VERSION;
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVersionSet(version, family);
    }
  }/* END OF NO COMMAND */
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_SNOOPQUERIER_SETQUERIERVERSION(family));
  }

  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_SNOOPQUERIER_QUERIERVERSIONSET(family));
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Configures the Snooping Querier Administrative Mode for a
*           specified vlan.
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
* @cmdsyntax    [no] set igmp querier <vlan-id>
* @cmdsyntax    [no] set mld querier <vlan-id>
*
* @cmdhelp
*
* @cmddescript  This command enables and disables Snooping Querier
*               on a vlan.
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierVlanMode(EwsContext ewsContext, L7_uint32 argc,
                                                  const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 numArg, vlanId, argVlanid = 1;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* Error Checking for Number of Arguments */
  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADMINMODE(family));
    }
    else
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADMINMODE_NO(family));
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argVlanid], &vlanId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (vlanId < L7_DOT1Q_MIN_VLAN_ID ||
      vlanId> L7_DOT1Q_MAX_VLAN_ID)
  {
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidVlanIdSpecified);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVlanModeSet(vlanId, L7_ENABLE, family);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVlanModeSet(vlanId, L7_DISABLE, family);
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADMINMODE_NO(family));
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  
                                             ewsContext, CLIERROR_SET_SNOOPING_QUERIER_VLANADMIN_MODE(family));
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the Snooping Querier vlan address
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
* @cmdsyntax    [no] set igmp querier <vlan-id> address <ip-address>
* @cmdsyntax    [no] set mld querier <vlan-id> adress <ip-address>
*
* @cmdhelp
*
* @cmddescript  This command sets and clears the snooping querier address.
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierVlanAddress(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 numArg, argIpaddr = 1, vlanId;
  L7_int32 argVlanid = -1;
  L7_uchar8 family = L7_AF_INET;
  L7_uchar8 ipAddr[L7_IP6_ADDR_LEN];
  L7_uchar8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 ip4addr = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADDRESS(family));
    }
    /* Extract the vlan id */
    if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argVlanid],
                                         &vlanId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (vlanId < L7_DOT1Q_MIN_VLAN_ID ||
        vlanId> L7_DOT1Q_MAX_VLAN_ID)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidVlanIdSpecified);
    }
    /* Extract the ip address */
    if (strlen(argv[index+argIpaddr]) >= sizeof(strIpAddr))
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADDRESS(family));
    }

    osapiStrncpySafe(strIpAddr, argv[index + argIpaddr], sizeof(strIpAddr));
    if (osapiInetPton(family, strIpAddr, ipAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGQUERIERIP(family));
    }

    if(family == L7_AF_INET && 
       (usmDbInetAton(strIpAddr, &ip4addr) != L7_SUCCESS))
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGQUERIERIP(family));
    }

    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVlanAddressSet(vlanId, ipAddr, family);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* Error Checking for Number of Arguments */
    if (numArg != 0)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADDRESS_NO(family));
    }

    /* Extract the vlan id */
    if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argVlanid],
                                         &vlanId) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (vlanId < L7_DOT1Q_MIN_VLAN_ID ||
        vlanId> L7_DOT1Q_MAX_VLAN_ID)
    {
      return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidVlanIdSpecified);
    }

    memset(ipAddr, 0, L7_IP6_ADDR_LEN);
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVlanAddressSet(vlanId, ipAddr, family);
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANADDRESS(family));
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_NOT_SUPPORTED)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                                               pStrErr_common_IncorrectInput,  
                                               ewsContext, 
                                               (family == L7_AF_INET) ?
                                                pStrErr_switching_UseSnoopingQuerierVlanAddr :
                                                pStrErr_switching_UseSnoopingQuerierVlanAddr6);
      }
      else
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                                             L7_NULLPTR, 
                                             ewsContext, 
                                             pStrErr_switching_SetSnoopingQuerierVlanAddr);
      } 
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configures the Snooping Querier election participate
*           administrative Mode.
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
* @cmdsyntax    [no] set igmp querier election participate <vlan-id>
* @cmdsyntax    [no] set mld querier election participate <vlan-id>
*
* @cmdhelp
*
* @cmddescript  This command enables and disables Snooping Querier's
*               election participate mode on a vlan.
*
* @end
*
*********************************************************************/
const L7_char8 *commandSetSnoopingQuerierElectionVlanMode(EwsContext ewsContext, L7_uint32 argc,
                                                          const L7_char8 * * argv, L7_uint32 index)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 numArg, vlanId, argVlanid = 1;        /* New variable Added */
  L7_uchar8 family = L7_AF_INET;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  if (strcmp(argv[1], pStrInfo_common_Igmp_2) == 0)
  {
    family = L7_AF_INET;
  }
  else if (strcmp(argv[1], pStrInfo_common_Mld_1) == 0)
  {
    family = L7_AF_INET6;
  }

  /* Error Checking for Number of Arguments */
  if (numArg != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext,
                     CLISYNTAX_CONFIGSNOOPQUERIER_VLANELECTMODE(family));
    }
    else
    {
      ewsTelnetWrite(ewsContext,
                     CLISYNTAX_CONFIGSNOOPQUERIER_VLANELECTMODE_NO(family));
    }

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (cliConvertTo32BitUnsignedInteger((L7_char8 *)argv[index+argVlanid], &vlanId) != L7_SUCCESS)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
  }

  if (vlanId < L7_DOT1Q_MIN_VLAN_ID ||
      vlanId> L7_DOT1Q_MAX_VLAN_ID)
  {
    return cliSyntaxReturnPromptAddBlanks (0, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidVlanIdSpecified);
  }

  /* If the command is of type 'normal' the 'if' condition is executed
     otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVlanElectionModeSet(vlanId, L7_ENABLE, family);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      rc = usmDbSnoopQuerierVlanElectionModeSet(vlanId, L7_DISABLE, family);
    }
  }
  else
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_CONFIGSNOOPQUERIER_VLANELECTMODE(family));
  }
  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (rc != L7_SUCCESS)
    { 
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_FailedToSet,  
                                             ewsContext, 
                                             CLIERROR_SET_SNOOPING_QUERIER_VLANELECT_MODE(family));
      
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

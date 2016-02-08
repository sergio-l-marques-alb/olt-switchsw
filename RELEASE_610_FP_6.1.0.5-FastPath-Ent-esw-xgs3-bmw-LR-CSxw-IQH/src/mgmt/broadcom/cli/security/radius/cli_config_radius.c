/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/radius/cli_config_radius.c
 *
 * @purpose Radius config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  4/08/2003
 *
 * @author  jflanagan
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"
#include "radius_exports.h"
#include "cli_web_exports.h"
#include "usmdb_radius_api.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_radius.h"

#include "pw_scramble_api.h"

/* For the definitions of CLI_NORMAL_CMD and CLI_NO_CMD */
#include "ews.h"

#include "commdefs.h"

/* For configuring secrets */
#define SECRET_FIRST_ENTRY 1
#define SECRET_SECOND_ENTRY 2

/*********************************************************************
*
* @purpose  Configure the maxretransmit value
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
* @notes
*
* @cmdsyntax  radius server retransmit <retries>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandRadiusServerRetransmit(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index)
{
  /*
   * Normal form : radius server retransmit <1-15>
   * No form     : no radius server retransmit
   * In the no form, the  retransmit value is set to default.
   */

  L7_uint32 numArg;
  L7_uint32 argMaxRetransmit = 1;
  L7_uint32 maxRetransmit = 0;
  L7_uint32 unit;
  L7_uchar8 strIpAddr[20];

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* For the normal command */
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusMaxReTx);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argMaxRetransmit], &maxRetransmit) != L7_SUCCESS ||
        maxRetransmit < L7_RADIUS_MIN_RETRANS || maxRetransmit > L7_RADIUS_MAX_RETRANS )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_security_RadiusMaxReTx, L7_RADIUS_MIN_RETRANS, L7_RADIUS_MAX_RETRANS );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* For the no form */
    if ( numArg != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusNoMaxReTx);
    }

    /* For the no form, the max retransmit value is set to the default. */
    maxRetransmit = FD_RADIUS_MAX_RETRANS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);
    if ( usmDbRadiusServerRetransSet(strIpAddr, maxRetransmit, L7_RADIUSGLOBAL) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_CfgRadiusMaxReTxs);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure the timeout value
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
* @notes
*
* @cmdsyntax  radius server timeout <seconds>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandRadiusServerTimeout(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  /**
   * Normal Form: radius server timeout <1-30>
   * No Form    : no radius server timeout
   * In the no form, the timeout value is set to the default.
   */

  L7_uint32 numArg;
  L7_uint32 argTimeout = 1;
  L7_uint32 timeout = 0;
  L7_uint32 unit;
  L7_uchar8 strIpAddr[20];

  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* For the normal command */
    if ( numArg != 1 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusTimeout_1);
    }

    if ( cliConvertTo32BitUnsignedInteger(argv[index+argTimeout], &timeout) != L7_SUCCESS ||
        timeout < L7_RADIUS_MIN_TIMEOUTVAL || timeout > L7_RADIUS_MAX_TIMEOUTVAL )
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_security_RadiusTimeout, L7_RADIUS_MIN_TIMEOUTVAL, L7_RADIUS_MAX_TIMEOUTVAL );
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    /* For the no form */
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusNoTimeout);
    }

    /* For the no form the default value is set (i.e. 6) */
    timeout = FD_RADIUS_TIMEOUT_DURATION;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    usmDbInetNtoa(ALL_RADIUS_SERVERS, strIpAddr);
    if ( usmDbRadiusServerTimeOutSet(strIpAddr, timeout, L7_RADIUSGLOBAL) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_CfgRadiusTimeout);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Enable/Disable RADIUS accounting support
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
* @notes
*
* @cmdsyntax  radius accounting mode
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandRadiusAccountingMode(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 * * argv,
                                            L7_uint32 index)
{
  /**
   * Normal Form: radius accounting mode
   * No Form    : no radius accounting mode
   * In the no form, the radius accounting mode is disabled.
   */

  L7_uint32 numArgs;      /* New variable added */
  L7_uint32 mode = 0;
  L7_uint32 unit;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if ( numArgs != 0 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusAcctMode);
  }

  if ( ewsContext->commType == CLI_NORMAL_CMD) /* enable */
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD) /* disable */
  {
    mode = L7_DISABLE;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbRadiusAccountingModeSet(unit, mode) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_CfgAcctMode);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure the IP address and port of the accounting server
* and radius server.
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
* @cmdsyntax  radius server host {auth | acct} <ipaddr|hostname> 
*                                  [ name <servername> ] [<port>]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRadiusServerHost(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argToken  = 1; /* auth | acct */
  L7_uint32 argHost = 2; /* ipaddr | hostname */
  L7_uint32 numArgs;
  L7_uint32 ipAddr, port;
  L7_uchar8 strHost[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uint32 unit=0;    
  L7_RC_t rc = L7_SUCCESS;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_char8 serverName[L7_RADIUS_SERVER_NAME_LENGTH+1];
  /* Indicate the presence of 'name' parameter in this command */
  L7_BOOL argNamePresent = L7_FALSE; 
  /* Indicate the presence of 'port' parameter in this command */
  L7_BOOL argPortPresent = L7_FALSE; 
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  /**
   * There can be a minimum of two arguments and a maximum
   * of six arguments to this command
   */
  if ((cliNumFunctionArgsGet() < 2)||(cliNumFunctionArgsGet() > 6))
  {    
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_security_CfgRadiusAuthAcctSrvrAdd);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_security_CfgRadiusNoAuthAcctSrvrAdd);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /* Verify the IP Address first. This is common to both modes */
  if (strlen(argv[index+argHost]) >= sizeof(strHost))
  {    
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_base_InvalidHostName);
  }

  osapiStrncpySafe(strHost, argv[index + argHost], sizeof(strHost));
  if (cliIPHostAddressValidate(ewsContext, strHost, &ipAddr, 
                     &addrType) != L7_SUCCESS)
  {
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }

  memset(serverName,0,L7_RADIUS_SERVER_NAME_LENGTH+1);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_ErrCouldNot, ewsContext, 
                     pStrErr_common_UnitId_1);
  }

  /* Obtain the first argument - token {auth|acct} */
  if (strcmp(argv[index+argToken],pStrInfo_common_LogFacilityAuth) == 0)
  {
    /* auth mode */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (numArgs == 4 )
      {
        /* radius host  ( auth <ip/dns> port <port> )*/        
        if( strcmp(argv[index+3],pStrInfo_common_Port_4) == 0 )
        {
          if ( cliConvertTo32BitUnsignedInteger(argv[index+4], 
                 &port) != L7_SUCCESS ||          
               port < L7_RADIUS_MIN_PORT || port > L7_RADIUS_MAX_PORT ) 
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     L7_NULLPTR,  ewsContext, 
                     pStrErr_security_InvalidPortNum );
          }

          argPortPresent = L7_TRUE;
          osapiStrncpySafe(serverName,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);          
        } 
        else if(strcmp(argv[index+3],pStrInfo_secuirty_serverName) == 0 )
        {
          /* radius host  ( auth <ip/dns> name <servername> )*/                  

          /*verify if the specified string contains all the alpha-numeric characters*/
          if (cliIsAlphaNum((L7_char8 *)argv[index+4]) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
          }

          if (strlen(argv[index+4]) > L7_RADIUS_SERVER_NAME_LENGTH)
          {      
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext,
                              pStrErr_security_SrvrNameMustBeCharsOrLess,
                              L7_RADIUS_SERVER_NAME_LENGTH);
          }

          argNamePresent = L7_TRUE;
          osapiStrncpySafe(serverName,argv[index+4],L7_RADIUS_SERVER_NAME_LENGTH+1);          
        }
      }   
      else if(numArgs == 6 ) 
      {
        /* radius host  ( auth <ip/dns> name <servername> port <port> )*/  
        if( ( strcmp(argv[index+3],pStrInfo_secuirty_serverName) != 0 )  || 
            ( strcmp(argv[index+5],pStrInfo_common_Port_4) != 0 )  
          )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_Error,  
                     ewsContext, 
                     pStrErr_security_CfgRadiusAuthAcctSrvrAdd1);
        }        
        /*verify if the specified string contains all the alpha-numeric characters*/
        if (cliIsAlphaNum((L7_char8 *)argv[index+4]) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
        }

        if (strlen(argv[index+4]) > L7_RADIUS_SERVER_NAME_LENGTH)
        {      
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,
                     ewsContext,
                     pStrErr_security_SrvrNameMustBeCharsOrLess,
                     L7_RADIUS_SERVER_NAME_LENGTH);
        }
        if( cliConvertTo32BitUnsignedInteger(argv[index+6], 
                     &port) != L7_SUCCESS ||          
                     port < L7_RADIUS_MIN_PORT || port > L7_RADIUS_MAX_PORT )           
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     L7_NULLPTR,  
                     ewsContext, 
                     pStrErr_security_InvalidPortNum );
        }        

        argPortPresent = L7_TRUE;
        argNamePresent = L7_TRUE;
        osapiStrncpySafe(serverName,argv[index+4],L7_RADIUS_SERVER_NAME_LENGTH+1);        
      }
      else if (2 != numArgs)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  
                     ewsContext, 
                     pStrErr_security_CfgRadiusAuthAcctSrvrAdd);
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {                      
        if(L7_FALSE == argNamePresent)
        {
          osapiStrncpySafe(serverName,L7_RADIUS_SERVER_DEFAULT_NAME_AUTH,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);                           
        }
        rc = usmDbRadiusIPHostNameServerAdd(unit, strHost,
                     addrType,serverName);
        if (rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks  (1, 0, 0, 0, 
                     pStrErr_common_Error,ewsContext, 
                     pStrErr_security_CfgSrvrAdd);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          if( L7_TRUE == argNamePresent )
          {
            if( usmDbRadiusServerNameSet(unit, strHost, addrType, serverName) 
                     != L7_SUCCESS )
            {         
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                     pStrErr_common_Error,  ewsContext,
                     pStrErr_security_CfgSrvrName);
            }
          }
        }
        else if(rc == L7_ERROR)
        {
          return cliSyntaxReturnPromptAddBlanks  (1, 0, 0, 0, 
                     pStrErr_common_Error,
                     ewsContext, pStrErr_security_CfgSrvrAddMaxReached);
        }

        if( L7_TRUE == argPortPresent )
        {
          if (usmDbRadiusHostNameServerPortNumSet(unit, strHost, addrType,
                                                    port) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                     pStrErr_common_Error,  ewsContext,
                     pStrErr_security_CfgSrvrPort);
          }
        }
      }/*if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)*/

    }/*if (ewsContext->commType == CLI_NORMAL_CMD)*/

    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /* no form */
      if (numArgs != 2)
      {
         return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusNoAuthAcctSrvrAdd);
      }

      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRadiusAuthHostNameServerRemove(unit, strHost, 
                     addrType) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
               pStrErr_common_Error,  ewsContext,
               pStrErr_security_CfgSrvrRemove);
        }
      }
    }
  }/* first argument is auth from - token {auth|acct} */

  else if (strcmp(argv[index+argToken],pStrInfo_security_Acct_1) == 0)
  {
    /* acct mode */
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (numArgs == 4 )
      {
        /* radius host  ( acct <ip/dns> port <port> )*/        
        if( strcmp(argv[index+3],pStrInfo_common_Port_4) == 0 )
        {
          if ( cliConvertTo32BitUnsignedInteger(argv[index+4], 
                    &port) != L7_SUCCESS ||          
                    port < L7_RADIUS_MIN_PORT || port > L7_RADIUS_MAX_PORT ) 
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                    L7_NULLPTR,  ewsContext, 
                    pStrErr_security_InvalidPortNum );
          }

          argPortPresent = L7_TRUE;
        } 
        else if(strcmp(argv[index+3],pStrInfo_secuirty_serverName) == 0 )
        {
          /* radius host  ( acct <ip/dns> name <servername> )*/                  

          /*verify if the specified string contains all the alpha-numeric characters*/
          if (cliIsAlphaNum((L7_char8 *)argv[index+4]) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,
                     ewsContext, pStrErr_security_SrvrName);
          }

          if (strlen(argv[index+4]) > L7_RADIUS_SERVER_NAME_LENGTH)
          {      
            return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,
                              ewsContext,
                              pStrErr_security_SrvrNameMustBeCharsOrLess,
                              L7_RADIUS_SERVER_NAME_LENGTH);
          }

          argNamePresent = L7_TRUE;
          osapiStrncpySafe(serverName,argv[index+4],L7_RADIUS_SERVER_NAME_LENGTH+1);          
        }
      }   
      else if(numArgs == 6 ) 
      {
        /* radius host  ( acct <ip/dns> name <servername> port <port> )*/  
        if( ( strcmp(argv[index+3],pStrInfo_secuirty_serverName) != 0 )  || 
            ( strcmp(argv[index+5],pStrInfo_common_Port_4) != 0 )  
          )
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_Error,  
                     ewsContext, 
                     pStrErr_security_CfgRadiusAuthAcctSrvrAdd1);
        }        
        /*verify if the specified string contains all the alpha-numeric characters*/
        if (cliIsAlphaNum((L7_char8 *)argv[index+4]) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, 
                     ewsContext, pStrErr_security_SrvrName);
        }

        if (strlen(argv[index+4]) > L7_RADIUS_SERVER_NAME_LENGTH)
        {      
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,
                     ewsContext,
                     pStrErr_security_SrvrNameMustBeCharsOrLess,
                     L7_RADIUS_SERVER_NAME_LENGTH);
        }

        if( cliConvertTo32BitUnsignedInteger(argv[index+6], 
                     &port) != L7_SUCCESS ||          
                     port < L7_RADIUS_MIN_PORT || port > L7_RADIUS_MAX_PORT )           
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     L7_NULLPTR,  
                     ewsContext, pStrErr_security_InvalidPortNum );
        }        

        argNamePresent = L7_TRUE;
        argPortPresent = L7_TRUE;
        osapiStrncpySafe(serverName,argv[index+4],L7_RADIUS_SERVER_NAME_LENGTH+1);                
      }
      else if (2 != numArgs) /* radius host acct <ipaddr|dnsname>*/
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  
                     ewsContext, pStrErr_security_CfgRadiusAuthAcctSrvrAdd);
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {                      
        /* case: radius host  ( acct <ip/dns> port <port> ) 
             which doesn't specify server name */                  
        if(L7_FALSE == argNamePresent)
        {
          osapiStrncpySafe(serverName,L7_RADIUS_SERVER_DEFAULT_NAME_ACCT,
                     L7_RADIUS_SERVER_NAME_LENGTH+1);                           
        }

        rc = usmDbRadiusAccountingHostNameServerAdd(unit, strHost,
                     addrType, serverName);
        if (rc == L7_FAILURE)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 0, 0, 0, 
                     pStrErr_common_Error,
                     ewsContext, pStrErr_security_CfgAcctSrvrAdd);

        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          if( L7_TRUE == argNamePresent )
          {
            rc = usmDbRadiusAccountingServerNameSet(unit, strHost, addrType, 
                     serverName);
            if( L7_FAILURE == rc)
            {  
              return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                     pStrErr_common_Error,  ewsContext,
                     pStrErr_security_CfgAcctSrvrName);
            }
            /* setting the same name is not allowed for Acct. servers*/
            if( L7_ALREADY_CONFIGURED == rc)
            {
              memset(buf,0,sizeof(buf));
              sprintf(buf,"%s %s %s",
                     pStrErr_security_CfgAcctSrvrAddNameExists,
                     serverName,pStrErr_security_CfgAcctSrvrAddNameExists1);
              return cliSyntaxReturnPromptAddBlanks  (1, 0, 0, 0, 
                     pStrErr_common_Error,
                     ewsContext, buf);
            }
          }
        }
        /* request denied as 'serverName' already used by another server*/
        else if (rc == L7_REQUEST_DENIED)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,
                     ewsContext,
                     pStrErr_security_CfgAcctSrvrAddNameExists2,
                     serverName);
        }
        else if(rc == L7_ERROR) /* Max entries reached*/
        {
          return cliSyntaxReturnPromptAddBlanks  (1, 0, 0, 0, 
                     pStrErr_common_Error,
                     ewsContext,pStrErr_security_CfgSrvrAddMaxReached);

        }
        

      /* If the port no. is spcecified in the command, it is being set.
       * By now, the server entry is there to set. Either it might h'v 
       * been added or had already been existing there.
       */

        if( L7_TRUE == argPortPresent )
        {
          if (usmDbRadiusAccountingHostNameServerPortNumSet(unit, strHost,
                     addrType,  port) != L7_SUCCESS)
          {            
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                     pStrErr_common_Error,  ewsContext,
                     pStrErr_security_CfgAcctSrvrPort);
          }    
        }
      }
    }/* if (ewsContext->commType == CLI_NORMAL_CMD)*/

    else if (ewsContext->commType == CLI_NO_CMD)
    {
      /* no form */
      if (numArgs != 2)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusNoAcctSrvrAdd);
      }

      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRadiusAccountingHostNameServerRemove(unit, strHost,
                     addrType) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
               pStrErr_common_Error,  ewsContext,
               pStrErr_security_CfgAcctSrvrRemove);
        }
      }
    }
  }
  else
  {
    /* unidentified token ... error condition */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, 
                     pStrErr_common_IncorrectInput,  ewsContext, 
                     pStrErr_security_CfgRadiusAcctSrvrAdd);
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Configure the secret for an accounting server or radius server.
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
* @notes Secret is handled as a password and is prompted
*
* @cmdsyntax  radius server key {auth | acct} <ipaddr|hostname>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRadiusServerKey(EwsContext ewsContext, L7_uint32 argc,
                                       const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArgs;
  L7_uint32 argToken   = 1;
  L7_uint32 argHost  = 2;
  L7_uint32 modeAction = 0;
  L7_char8 buf[ L7_DNS_HOST_NAME_LEN_MAX ];
  static L7_char8 strHost[ L7_DNS_HOST_NAME_LEN_MAX ];
  static L7_char8 strSecret[ L7_CLI_MAX_STRING_LENGTH ];
  L7_uint32 unit, ipAddr = 0;
  boolean valid;
  static L7_IP_ADDRESS_TYPE_t addrType;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (numArgs != 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusAcctSrvrSecret);
  }

  if (strcmp(argv[index+argToken],pStrInfo_common_LogFacilityAuth) == 0)
  {
    /* radius server case */
    modeAction = 1;
  }
  else if (strcmp(argv[index+argToken],pStrInfo_security_Acct_1) == 0)
  {
    /* accounting server case */
    modeAction = 2;
  }
  else
  {
    /* unidentified token ... error condition */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusAcctSrvrSecret);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if (cliGetStringInputID() == CLI_INPUT_EMPTY)
  {
    /* Verify if the specified ip address is valid */
    osapiStrncpySafe(strHost, argv[index + argHost], sizeof(strHost));
    if (cliIPHostAddressValidate(ewsContext, strHost, &ipAddr, &addrType) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }
    if (modeAction == 2)
    {
      /* Verify the IP address is a configurd authentication server */
      if (usmDbRadiusAccountingServerIPHostNameVerify(unit, strHost, addrType,
                                                &valid) != L7_SUCCESS ||
          valid == L7_FALSE )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
             pStrErr_common_IncorrectInput,  ewsContext,
             pStrErr_security_CfgRadiusAcctSrvrInvalid);
      }
    }
    else if (modeAction == 1)
    {
      /* Verify the IP address is a configurd server */
      if ( usmDbRadiusHostNameServerAddrVerify(unit, strHost, addrType,
                                       &valid) != L7_SUCCESS ||
          valid == L7_FALSE )
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
               pStrErr_common_IncorrectInput,  ewsContext,
               pStrErr_security_CfgRadiusSrvrInvalid);
      }

    }

    /* First pass through.  Set the alternate command to get the secret.*/
    memset (buf, 0, sizeof(buf));
    if (modeAction == 2)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s",
            pStrInfo_security_RadiusCfgAcctServKey, argv[index+argHost]);
    }
    else if (modeAction == 1)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s",
           pStrInfo_security_RadiusCfgAuthServKey, argv[index+argHost]);
    }
    cliAlternateCommandSet(buf);
    cliSetStringInputID(SECRET_FIRST_ENTRY, ewsContext, argv);
    cliSetStringPassword();

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
	
    return pStrInfo_security_RadiusEnterSecretFirstPrompt;
  }
  else if (cliGetStringInputID() == SECRET_FIRST_ENTRY)
  {
    memset(strSecret,0,sizeof(strSecret));
    osapiStrncpySafe(strSecret, cliGetStringInput(), sizeof(strSecret));
	
    if (strlen(strSecret) > L7_RADIUS_MAX_SECRET)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_Error,ewsContext, pStrErr_security_RadiusSecret_1, L7_RADIUS_MAX_SECRET );
    }

    if (usmDbStringPrintableCheck(strSecret) != L7_SUCCESS)
    {
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      return cliSyntaxReturnPromptAddBlanks(1,1, 0, 0, pStrErr_common_IncorrectInput, ewsContext, pStrErr_security_WsVapRadiusSecretInvalid);
    }

    /* Second pass through.  Set the alternate command to confirm the secret.*/
    memset (buf, 0, sizeof(buf));
    if (modeAction == 2)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_security_RadiusCfgAcctServKey, argv[index+argHost]);
    }
    else if (modeAction == 1)
    {
      osapiSnprintf(buf, sizeof(buf), "%s %s", pStrInfo_security_RadiusCfgAuthServKey, argv[index+argHost]);
    }
    cliAlternateCommandSet(buf);
    cliSetStringInputID(SECRET_SECOND_ENTRY, ewsContext, argv);
    cliSetStringPassword();

    /*************Set Flag for Script Successful******/
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

    return pStrInfo_common_ApProfileEnterSecretSecondPrompt;
  }
  else /* (cliGetStringInputID() == SECRET_SECOND_ENTRY) */
  {
    if (strcmp(strSecret, cliGetStringInput()) != 0 )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgApProfileRadiusSecretMismatch);
    }

    if (modeAction == 1)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRadiusHostNameServerSharedSecretSet(strHost,
                                          strSecret, L7_RADIUSLOCAL) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                 pStrErr_common_Error,  ewsContext,
                 pStrErr_security_CfgSrvrSecret);
        }
      }
    }
    else if (modeAction == 2)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbRadiusAccountingHostNameServerSharedSecretSet(unit, strHost,
                                           addrType, strSecret) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
                 pStrErr_common_Error,  ewsContext,
                 pStrErr_security_CfgAcctSrvrSecret);
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
* @purpose  Configure the secret for an accounting server or radius server
*           using a pre-encrypted key.
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
* @cmdsyntax  radius server key {auth | acct} <ipaddr|hostname> encrypted [key]
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRadiusServerKeyEncrypted(EwsContext ewsContext, L7_uint32 argc,
                                                const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArgs;
  L7_int32 argToken   = -2;
  L7_int32 argHost    = -1;
  L7_int32 argKey     = 1;
  L7_uint32 modeAction = 0;
  L7_char8 strHost[ L7_DNS_HOST_NAME_LEN_MAX ];
  L7_char8 strSecret[ L7_CLI_MAX_STRING_LENGTH ];
  L7_char8 strEncrypted[ L7_ENCRYPTED_PASSWORD_SIZE ];
  L7_uint32 unit, ipAddr = 0;
  boolean valid;
  L7_IP_ADDRESS_TYPE_t addrType;

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  if (numArgs != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusAcctSrvrSecretEncrypted);
  }

  if (strcmp(argv[index+argToken],pStrInfo_common_LogFacilityAuth) == 0)
  {
    /* radius server case */
    modeAction = 1;
  }
  else if (strcmp(argv[index+argToken],pStrInfo_security_Acct_1) == 0)
  {
    /* accounting server case */
    modeAction = 2;
  }
  else
  {
    /* unidentified token ... error condition */
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusAcctSrvrSecret);
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /* Verify if the specified ip address is valid */
  osapiStrncpySafe(strHost, argv[index + argHost], sizeof(strHost));
  if (cliIPHostAddressValidate(ewsContext, strHost, &ipAddr, &addrType) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  if (modeAction == 2)
  {
    /* Verify the IP address is a configurd authentication server */
    if (usmDbRadiusAccountingServerIPHostNameVerify(unit, strHost, addrType,
                                              &valid) != L7_SUCCESS ||
        valid == L7_FALSE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
           pStrErr_common_IncorrectInput,  ewsContext,
           pStrErr_security_CfgRadiusAcctSrvrInvalid);
    }
  }
  else if (modeAction == 1)
  {
    /* Verify the IP address is a configurd server */
    if ( usmDbRadiusHostNameServerAddrVerify(unit, strHost, addrType,
                                     &valid) != L7_SUCCESS ||
        valid == L7_FALSE )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
             pStrErr_common_IncorrectInput,  ewsContext,
             pStrErr_security_CfgRadiusSrvrInvalid);
    }
  }

  osapiStrncpySafe(strEncrypted, argv[index + argKey], sizeof(strEncrypted));

  /* decrypt key prior to configuration */
  if (pwDecrypt(strSecret, strEncrypted, L7_PASSWORD_SIZE-1) != L7_SUCCESS) {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
             pStrErr_common_Error,  ewsContext,
             pStrErr_security_CfgSrvrSecretDecrypt);
  }

  if (modeAction == 1)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRadiusHostNameServerSharedSecretSet(strHost,
                                        strSecret, L7_RADIUSLOCAL) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
               pStrErr_common_Error,  ewsContext,
               pStrErr_security_CfgSrvrSecret);
      }
    }
  }
  else if (modeAction == 2)
  {
    /*******Check if the Flag is Set for Execution*************/
    if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbRadiusAccountingHostNameServerSharedSecretSet(unit, strHost,
                                         addrType, strSecret) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
               pStrErr_common_Error,  ewsContext,
               pStrErr_security_CfgAcctSrvrSecret);
      }
    }
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Specify which server is the primary RADIUS server.
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
* @cmdsyntax  radius server primary <ipaddr|hostname>
*
* @cmdhelp The specified IP address must match that of a configured RADIUS server.
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandRadiusServerPrimary(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 argHost = 1;
  L7_uint32 ipAddr;
  L7_char8 strHost[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uint32 unit;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusSrvrPrimary_1);
  }

  if (strlen(argv[index+argHost]) >= sizeof(strHost))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
  }

  /* Verify if the specified ip address is valid */
  osapiStrncpySafe(strHost, argv[index + argHost], sizeof(strHost));
  if (cliIPHostAddressValidate(ewsContext, strHost, &ipAddr, &addrType) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbRadiusHostNameServerEntryTypeSet(unit,  strHost, addrType,
                L7_RADIUS_SERVER_ENTRY_TYPE_PRIMARY ) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
              ewsContext, pStrErr_security_CfgSrvrPrimary);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose Enable or disable the message authenticator attribute for this server.
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
 * @cmdsyntax  [no] radius server msgauth <ipaddr|hostname>
 *
 * @cmdhelp The specified IP address must match that of a configured RADIUS server.
 *
 * @cmddescript
 *
 *
 * @end
 * *********************************************************************/
const L7_char8 *commandRadiusServerMsgAuth(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 * * argv, L7_uint32 index) {
  L7_uint32 argHost = 1;
  L7_uint32 ipAddr;
  L7_char8 strHost[L7_DNS_HOST_NAME_LEN_MAX];
  L7_uint32 mode = L7_DISABLE;
  L7_uint32 unit;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  if (cliNumFunctionArgsGet() != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusSrvrMsgAuth);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgNoRadiusSrvrMsgAuth);
    }
    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  if (strlen(argv[index+argHost]) >= sizeof(strHost))
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
  }

  /* Verify if the specified ip address is valid */
  osapiStrncpySafe(strHost, argv[index + argHost], sizeof(strHost));
  if (cliIPHostAddressValidate(ewsContext, strHost, &ipAddr, &addrType) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0,
           pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbRadiusHostNameServerIncMsgAuthModeSet(unit, strHost, addrType,
                                                   mode) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,
             ewsContext, pStrErr_security_CfgSrvrMsgAuth);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the NAS-IP addresss for the Radius Client.
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
* @notes
*
* @cmdsyntax  radius server attribute 4 [<ip_address>]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/

const L7_char8 *commandRadiusAttribute4Set(EwsContext ewsContext,
                                           L7_uint32 argc,
                                           const L7_char8 * * argv,
                                           L7_uint32 index)
{
  L7_uint32 numArgs;      /* New variable added */
  L7_BOOL mode = 0;
  L7_uint32 unit;
  L7_uint32 ipAddr;
  L7_char8 strIpAddr[L7_CLI_MAX_STRING_LENGTH];

  cliSyntaxTop(ewsContext);
  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if ( numArgs > 1 )
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgRadiusAttr_1);
  }

  if ( ewsContext->commType == CLI_NORMAL_CMD) /* enable */
  {
    mode = L7_TRUE;
  }
  else if (ewsContext->commType == CLI_NO_CMD) /* disable */
  {
    mode = L7_FALSE;
  }

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
  }

  if(numArgs != 0)
  {
    /* Verify the IP Address first. This is common to both modes */
    if (strlen(argv[index+1]) >= sizeof(strIpAddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }
    osapiStrncpySafe(strIpAddr, argv[index + 1], sizeof(strIpAddr));
    if (usmDbInetAton(strIpAddr, &ipAddr) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_common_CfgNwIp);
    }

  }
  else
  {
    ipAddr = 0;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ( usmDbRadiusAttribute4Set(unit,mode,ipAddr) != L7_SUCCESS )
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_security_CfgRadiusAttr);
    }
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

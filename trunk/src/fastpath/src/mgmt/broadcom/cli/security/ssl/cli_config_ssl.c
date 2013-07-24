/*********************************************************************
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssl/cli_config_ssl.c
*
* @purpose config commands for the cli
*
* @component user interface
*
* @comments none
*
* @create  09/16/2003
*
* @author  Kim Mans
* @end
*
**********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"

#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_sslt_api.h"
#include "l7_relay_api.h"

#include "ews.h"

#include "osapi.h"
#include "nimapi.h"
#include "usmdb_ip_api.h"
#include "clicommands_ssl.h"
#include "sslt_exports.h"

/*********************************************************************
*
* @purpose    Enables or disables IP http secure-server
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  config ip http secure-server
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8  *commandIpHttpSecureServer(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 * * argv, L7_uint32 index)
{

   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 val;
   L7_int32 rc;

   /* check argument validity and # of parameter */
   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   numArg = cliNumFunctionArgsGet();

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   if (ewsContext->commType == CLI_NORMAL_CMD)
   {
      if (numArg != 0)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecureSrvr);
      }

      val = L7_ENABLE;
   }
   else if (ewsContext->commType == CLI_NO_CMD)
   {
      if (numArg != 0)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecureSrvrNo);
      }
      val = L7_DISABLE;
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecureSrvr);
   }

   /*******Check if the Flag is Set for Execution*************/
   if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
   {
     rc =  usmDbssltAdminModeSet(unit, val);

     if (rc != L7_SUCCESS && ewsContext->commType == CLI_NORMAL_CMD)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecureSrvr);
     }
     else if (rc != L7_SUCCESS && ewsContext->commType == CLI_NO_CMD)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecureSrvrNo);
     }
   }

   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Enables or disables IP http secure-protocol
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  ip http secure-protocol
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpSecureProtocol(EwsContext ewsContext, L7_uint32 argc,
                                            const L7_char8 * * argv, L7_uint32 index)
{

  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 argProtocol = 1;
  L7_uchar8 strProtocol[L7_CLI_MAX_STRING_LENGTH],strProtocol1[L7_CLI_MAX_STRING_LENGTH];

  /* check argument validity and # of parameter */
  cliSyntaxTop(ewsContext);
  unit = cliGetUnitId();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  numArg = cliNumFunctionArgsGet();

  if (numArg < 1 || numArg > 2)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecureProto);
  }

  if (numArg < 2)
  {
    osapiStrncpySafe(strProtocol,argv[index + argProtocol], sizeof(strProtocol));
    cliConvertToLowerCase(strProtocol);

    if (strcmp(pStrInfo_security_Tls1_1, strProtocol) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
      if (usmDbssltProtocolLevelSet(unit, L7_SSLT_PROTOCOL_TLS10, L7_ENABLE) != L7_SUCCESS)
      {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProto_1, strProtocol);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
      if (usmDbssltProtocolLevelSet(unit, L7_SSLT_PROTOCOL_SSL30, L7_DISABLE) !=L7_SUCCESS)
      {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProtoDefl, strProtocol);
        }
      }
    }
    else if (strcmp(pStrInfo_security_Ssl3_1, strProtocol) == 0)
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbssltProtocolLevelSet(unit, L7_SSLT_PROTOCOL_SSL30, L7_ENABLE) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProto_1, strProtocol);
        }
      }

      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
      if (usmDbssltProtocolLevelSet(unit, L7_SSLT_PROTOCOL_TLS10, L7_DISABLE) != L7_SUCCESS)
      {

          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProtoDefl, strProtocol);
        }
      }
    }
    else if ((strcmp(pStrInfo_security_Tls1_1, strProtocol) != 0) || (strcmp(pStrInfo_security_Ssl3_1, strProtocol) != 0))
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProto_1, strProtocol);
    }
  }

  else if (numArg == 2)
  {
    osapiStrncpySafe(strProtocol,argv[index + 1], sizeof(strProtocol));
    cliConvertToLowerCase(strProtocol);
    osapiStrncpySafe(strProtocol1,argv[index + 2], sizeof(strProtocol1));
    cliConvertToLowerCase(strProtocol1);

    if (((strcmp(pStrInfo_security_Tls1_1, strProtocol) == 0) && (strcmp(pStrInfo_security_Ssl3_1, strProtocol1) == 0)) || ((strcmp(pStrInfo_security_Ssl3_1, strProtocol) == 0) && (strcmp(pStrInfo_security_Tls1_1, strProtocol1) == 0)))
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
      if (usmDbssltProtocolLevelSet(unit, L7_SSLT_PROTOCOL_TLS10, L7_ENABLE) != L7_SUCCESS)
      {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProto_1, pStrInfo_security_Tls1);
        }
      }
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbssltProtocolLevelSet(unit, L7_SSLT_PROTOCOL_SSL30, L7_ENABLE) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProto_1, pStrInfo_security_Ssl3);
        }
      }
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_HttpSecureProtoBoth, strProtocol, strProtocol1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose    Enables or disables IP http secure-port
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @notes none
*
* @cmdsyntax  config ip http secure-protocol
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8  *commandIpHttpSecurePort(EwsContext ewsContext, L7_uint32 argc,
                                        const L7_char8 * * argv, L7_uint32 index)
{

 L7_uint32 unit;
 L7_uint32 numArg;
 L7_uint32 port;

 /* check argument validity and # of parameter */
 cliSyntaxTop(ewsContext);
 unit = cliGetUnitId();

 numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecurePort);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecurePortNo);
    }
  }
  else
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecurePort);
  }

   if (ewsContext->commType == CLI_NORMAL_CMD)
   {
      if (cliConvertTo32BitUnsignedInteger(argv[index+1], &port) != L7_SUCCESS)
      {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpHttpSecurePort);
      }
      else if (port < L7_SSLT_SECURE_PORT_MIN ||
               port > L7_SSLT_SECURE_PORT_MAX)
      {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_HttpSecurePortRange, L7_SSLT_SECURE_PORT_MIN, L7_SSLT_SECURE_PORT_MAX);
    }
    else
    {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
        if (usmDbssltSecurePortSet(unit, port) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecurePort);
        }
        }
      }
   }
   else if (ewsContext->commType == CLI_NO_CMD)
   {
      /*******Check if the Flag is Set for Execution*************/
      if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbssltSecurePortSet(unit, L7_SSLT_SECURE_PORT) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecurePortDefl);
      }
      }
   }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set the maximum number of allowable Secure http sessions
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
* @cmdsyntax  ip http secure-session maxsessions <0-16>
*
* @cmdhelp Set the maximum number of allowable secure http sessions.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpSecureMaxSessions(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 max;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext, pStrErr_security_PrintCfgHttpSecureMaxSessions, 0, FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS);
    }
    else
    {
      (void) cliConvertTo32BitUnsignedInteger(argv[index+1], &max);
    }
  }
  else /* ewsContext->commType == CLI_NO_CMD) */
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgHttpSecureMaxSessionsNo);
    }
    else
    {
      max = FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS;
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbssltNumSessionsSet(max) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecureMaxSessions);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Set the secure http session soft timeout
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
* @cmdsyntax  ip http secure-session soft-timeout <0-60>
*
* @cmdhelp Set the secure http session soft timeout in minutes.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpSecureSoftTimeout(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 val;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                              pStrErr_security_PrintCfgHttpSecureSoftTimeout,
                    FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MIN,
                    FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_MAX);
    }
    else
    {
      (void) cliConvertTo32BitUnsignedInteger(argv[index+1], &val);
    }
  }
  else /* ewsContext->commType == CLI_NO_CMD) */
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgHttpSecureSoftTimeoutNo);
    }
    else
    {
      val = FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbssltSessionSoftTimeOutSet(val) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecureSoftTimeout);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose  Set the secure http session hard timeout
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
* @cmdsyntax  ip http secure-session hard-timeout <0-168>
*
* @cmdhelp Set the secure http session hard timeout in hours.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpHttpSecureHardTimeout(EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 numArg;
  L7_uint32 val;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg != 1)
    {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, pStrErr_common_IncorrectInput,ewsContext,
                              pStrErr_security_PrintCfgHttpSecureHardTimeout,
                    FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MIN,
                    FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_MAX);
    }
    else
    {
      (void) cliConvertTo32BitUnsignedInteger(argv[index+1], &val);
    }
  }
  else /* ewsContext->commType == CLI_NO_CMD) */
  {
    if (numArg != 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgHttpSecureHardTimeoutNo);
    }
    else
    {
      val = FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
    }
  }

  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbssltSessionHardTimeOutSet(val) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_HttpSecureHardTimeout);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliSyntaxReturnPrompt (ewsContext, "");
}

/*********************************************************************
*
* @purpose    Enables or disables IP SSH
*
*
* @param EwsContext ewsContext
* @param uintf argc
* @param const L7_char8 **argv
* @param uintf index
*
* @returntype const L7_char8
*
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  crypto certificate generate
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandCryptoCertGenerate(EwsContext ewsContext, L7_uint32 argc,
                              const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 tmp32;

  unit = cliGetUnitId();

  if (usmDbssltAdminModeGet(unit, &tmp32) != L7_SUCCESS)
  {
    tmp32 = L7_DISABLE;
  }

  if (tmp32 == L7_ENABLE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_HttpCertHttpsEnabled);
  }

  osapiFsDeleteFile(L7_SSLT_SERVER_CERT_PEM_ONE);
  osapiFsDeleteFile(L7_SSLT_SERVER_KEY_PEM_ONE);


  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (usmDbssltCertificateGenerate(1, 1024, 0, 0, 0, 0, 0, 0, 0, 0) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_HttpCertGenerate);
    }
  }

  return cliPrompt(ewsContext);
}

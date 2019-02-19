/*********************************************************************
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssh/cli_config_ssh.c
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

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include <errno.h>

#include "cliapi.h"
#include "l7_common.h"
#include "l7_relay_api.h"

#include "ews.h"

#include "osapi.h"
#include "nimapi.h"
#include "clicommands_ssh.h"
#include "sshd_api.h"
#include "usmdb_sshd_api.h"
#include "sshd_exports.h"

/*********************************************************************
*
* @purpose    Enables or disables IP SSH Protocol
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
* @cmdsyntax  config ip ssh
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpSshProtocol(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{

   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 protoLevel=0;

   /* check argument validity and # of parameter */
   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   numArg = cliNumFunctionArgsGet();

   if (numArg < 1 || numArg > 2)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpSshProto);
   }
   else if (numArg == 2)
   {
    if ((strcmp(pStrInfo_common_RateMbps, argv[index+1]) == 0 && strcmp(pStrInfo_common_Rate2Mbps_1, argv[index+2]) == 0) ||
        (strcmp(pStrInfo_common_Rate2Mbps_1, argv[index+1]) == 0 && strcmp(pStrInfo_common_RateMbps, argv[index+2]) == 0))
    {

      protoLevel = L7_SSHD_PROTO_LEVEL;
    }                                       /* Both 1 and 2 */

    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpSshProtoInvalid);
    }

  }
  else if (strcmp(pStrInfo_common_RateMbps, argv[index+1]) == 0)
   {
      protoLevel = L7_SSHD_PROTO_LEVEL_V1_ONLY;
   }
  else if (strcmp(pStrInfo_common_Rate2Mbps_1, argv[index+1]) == 0)
   {
      protoLevel = L7_SSHD_PROTO_LEVEL_V2_ONLY;
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpSshProtoInvalid);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
   if (usmDbsshdProtoLevelSet(unit, protoLevel) != L7_SUCCESS)
   {
      return cliSyntaxReturnPromptAddBlanks (1,1, 0, 0, L7_NULLPTR,ewsContext, pStrInfo_security_IpSshProto_1, argv[index+1] );
    }
   }

   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Sets the maximum number of ssh connections allowed into the switch.
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
* @cmdsyntax  sshcon maxsessions <value>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8  *commandSshConMaxSessions(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 tempInt = 0;
   L7_uint32 arg1 = 1;

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   numArg = cliNumFunctionArgsGet();

   if (ewsContext->commType == CLI_NORMAL_CMD)
   {
       if (numArg != 1)
       {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgSshConMaxSession);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+arg1],  &tempInt)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgSshConMaxSession);
       }

      if ((tempInt >= FD_CLI_SSHCON_MIN_SESSIONS) && (tempInt <=L7_SSHD_DEFAULT_MAX_CONNECTIONS))
      {
        /*******Check if the Flag is Set for Execution*************/
        if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
           rc = usmDbSshdMaxNumSessionsSet(unit, &tempInt);
       }
    }
       else
       {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgSshConMaxSession);
       }

   }
   else
       if (ewsContext->commType == CLI_NO_CMD)
       {
           if (numArg != 0)
           {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgNoSshConMaxSession);
           }
           else
           {
               tempInt = FD_SSHD_MAX_SESSIONS;
               /*******Check if the Flag is Set for Execution*************/
               if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
                 rc = usmDbSshdMaxNumSessionsSet(unit, &tempInt);
           }
    }

       }

   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

   return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose    Sets the idle timeout value for ssh sessions into the switch
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
* @cmdsyntax  sshcon timeout <value>
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/

const L7_char8  *commandSshConTimeout(EwsContext ewsContext, L7_uint32 argc,
                                     const L7_char8 * * argv, L7_uint32 index)
{
   L7_RC_t rc;
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 tempInt = 0;
   L7_uint32 arg1 = 1;

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   if (unit == 0)
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
   }

   numArg = cliNumFunctionArgsGet();

   if (ewsContext->commType == CLI_NORMAL_CMD)
   {
       if (numArg != 1)
       {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgSshConTimeout);
    }
    if (cliConvertTo32BitUnsignedInteger(argv[index+arg1],  &tempInt)!=L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgSshConTimeout);
       }
       if ((tempInt >= 1 ) && (tempInt <=160))
       {
       /*******Check if the Flag is Set for Execution*************/
       if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
          rc = usmDbsshdTimeoutSet(unit, &tempInt);
       }
    }
       else
       {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgSshConTimeout);
       }

   }
   else
       if (ewsContext->commType == CLI_NO_CMD)
       {
           if (numArg != 0)
           {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgNoSshConTimeout);
           }
           else
           {
               tempInt = FD_SSHD_TIMEOUT;
               /*******Check if the Flag is Set for Execution*************/
               if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
      {
                 rc = usmDbsshdTimeoutSet(unit, &tempInt);
      }
           }
       }
   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   return cliPrompt(ewsContext);
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
* @notes none
*
* @cmdsyntax  config ip ssh
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8  *commandIpSshServerEnable(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 * * argv, L7_uint32 index)
{
   L7_uint32 unit;
   L7_uint32 numArg;
   L7_uint32 val;
   L7_int32 rc;

   /* check argument validity and # of parameter */
   cliSyntaxTop(ewsContext);
   unit = cliGetUnitId();

   /*************Set Flag for Script Failed******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

   numArg = cliNumFunctionArgsGet();

   if (ewsContext->commType == CLI_NORMAL_CMD)
   {
     if ((numArg != 0) && (numArg != 2))
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpSsh);
     }
     val = L7_ENABLE;
   }
   else if (ewsContext->commType == CLI_NO_CMD)
   {
     if ((numArg != 0) && (numArg != 2))
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpSshNo);
     }
     val = L7_DISABLE;
   }
   else
   {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 1, pStrErr_common_IncorrectInput,  ewsContext, pStrErr_security_CfgIpSsh);
   }

   /*******Check if the Flag is Set for Execution*************/
   if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
   {
     rc = usmDbsshdAdminModeSet(unit, val);
     if (rc != L7_SUCCESS && ewsContext->commType == CLI_NORMAL_CMD)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_IpSsh);
     }
     else if (rc != L7_SUCCESS && ewsContext->commType == CLI_NO_CMD)
     {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_security_IpSshNo);
     }
   }

   /*************Set Flag for Script Successful******/
   ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
   return cliPrompt(ewsContext);
}
/*********************************************************************
*
* @purpose    Generates RSA Key
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
* @cmdsyntax  crypto key generate rsa
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandCryptoGenerateSshRSAKey(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 **argv,
                                               L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 tmp32;

  unit = cliGetUnitId();

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (usmDbsshdAdminModeGet(unit, &tmp32) != L7_SUCCESS)
  {
      tmp32 = L7_DISABLE;
  }

  if (tmp32 == L7_ENABLE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_KeyGenSshEnabled);
  }

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* if key exists, prompt user to overwrite existing key */
    if (usmDbsshdKeyExists(SSHD_KEY_TYPE_RSA) == L7_SUCCESS)
    {
      if (cliGetCharInputID() == CLI_INPUT_EMPTY)
      {
        cliSetCharInputID(1, ewsContext, argv);
        cliAlternateCommandSet(pStrErr_security_CryptoKeyGenerateRsa);
        return pStrInfo_security_ConfirmRsaKeyOverwrite;
      }
      else if ((cliGetCharInputID() == 1) &&
               (tolower(cliGetCharInput()) == 'n'))
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_RsaKeysNotGenerated);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        cliSyntaxBottom(ewsContext);
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      cliSyntaxTop(ewsContext);
    }

    /*******Check if the Flag is Set for Execution*************/
    /* this check is probably redundant based on checks above, but doesn't hurt */
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* start asynchronous key generation */
      if (usmDbsshdKeyGenerate(SSHD_KEY_TYPE_RSA) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_RsaKeyGenerate);
      }
      if (cliGetCharInputID() == 1)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      }
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  { /* 'no' command */

    osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY);
    osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY_RSA);
  }
  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose    Generates DSA Key
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
* @cmdsyntax  crypto key generate dsa
*
* @cmdhelp
*
* @cmddescript
*
*
* @end
*
*********************************************************************/
const L7_char8 *commandCryptoGenerateSshDSAKey(EwsContext ewsContext,
                                               L7_uint32 argc,
                                               const L7_char8 **argv,
                                               L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 tmp32;

  unit = cliGetUnitId();
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (usmDbsshdAdminModeGet(unit, &tmp32) != L7_SUCCESS)
  {
      tmp32 = L7_DISABLE;
  }

  if (tmp32 == L7_ENABLE)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_KeyGenSshEnabled);
  }

  /* this command is not valid in a configuration script,
     just fail if we are validating a script or running a script */
  if ((ewsContext->scriptActionFlag == L7_VALIDATE_SCRIPT) ||
      (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
  {
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    /* if key exists, prompt user to overwrite existing key */
    if (usmDbsshdKeyExists(SSHD_KEY_TYPE_DSA) == L7_SUCCESS)
    {
      if (cliGetCharInputID() == CLI_INPUT_EMPTY)
      {
        cliSetCharInputID(1, ewsContext, argv);
        cliAlternateCommandSet(pStrErr_security_CryptoKeyGenerateDsa);
        return pStrInfo_security_ConfirmDsaKeyOverwrite;
      }
      else if ((cliGetCharInputID() == 1) &&
               (tolower(cliGetCharInput()) == 'n'))
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_DsaKeysNotGenerated);
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        cliSyntaxBottom(ewsContext);
        ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      cliSyntaxTop(ewsContext);
    }
    /*******Check if the Flag is Set for Execution*************/
    /* this check is probably redundant based on checks above, but doesn't hurt */
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      /* start asynchronous key generation */
      if (usmDbsshdKeyGenerate(SSHD_KEY_TYPE_DSA) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_security_DsaKeyGenerate);
      }
      if (cliGetCharInputID() == 1)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      }
      cliSyntaxBottom(ewsContext);
    }
  }
  else
  { /* 'no' command */

    osapiFsDeleteFile(L7_SSHD_PATH_SERVER_PRIVKEY_DSA);
  }


  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_config_tacacs.c
 *
 * @purpose Action command support to CLI TACACS+
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  15/03/2005
 *
 * @authors  Rama Krishna Hazari, Navin Kumar Rungta
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "osapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "usmdb_tacacs_api.h"
#include "clicommands_tacacs.h"
#include "clicommands_card.h"
#include "pw_scramble_api.h"

/*********************************************************************
*
* @purpose  To set the TACACS server port
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  port <port-number>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSPort(EwsContext ewsContext,
                                  L7_uint32 argc,
                                  const L7_char8 * * argv,
                                  L7_uint32 index)
{
  L7_uint32 port = 0;
  L7_ushort16 port1 = 0;
  L7_uint32 numArg = 0, unit = 0, ipAddr = 0;
  L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_PORT_HELP);
  }
  else if (numArg == 0)
  {
    port = CLITACACS_DEFAULT_PORT_NUM;
  }
  else
  {

    if (cliConvertTo32BitUnsignedInteger(argv[index + 1], &port) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPortRange_1);
    }

    /* check for the range of the port. */
    if (port < CLITACACS_PORT_RANGE_MIN || port > CLITACACS_PORT_RANGE_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPortRange_1);
    }
  }
  unit = cliGetUnitId();
  /* get the current Host Address */
  OSAPI_STRNCPY_SAFE(strHostAddr, EWSTACACSSRVHOST(ewsContext));
  /*Validate & Get the address type for Host Address */
  if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &type) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  port1 = (L7_ushort16) port;
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbTacacsHostNamePortNumberSet(type, strHostAddr, port1) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPort_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the TACACS server timeout
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  timeout <timeout>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSTimeout(EwsContext ewsContext,
                                     L7_uint32 argc,
                                     const L7_char8 * * argv,
                                     L7_uint32 index)
{
  L7_uchar8 timeout = 0;
  L7_uint32 intTimeout = 0, ipAddr = 0;
  L7_uint32 numArg = 0, unit = 0;
  L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_TIMEOUT_HELP);
  }

  if (numArg == 0)
  {
    timeout = 0; /* timeout parameter of 0 specifies to use global configuration */
  }
  else
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index + 1], &intTimeout)
        != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_TacacsTimeoutRange);
    }

    /* check for the range of timeout value. */
    if (intTimeout < CLITACACS_TIMEOUT_RANGE_MIN
        || intTimeout > CLITACACS_TIMEOUT_RANGE_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_TacacsTimeoutRange);
    }
    timeout = (L7_char8) intTimeout;
  }
  unit = cliGetUnitId();
  /* get the current Host Address */
  OSAPI_STRNCPY_SAFE(strHostAddr, EWSTACACSSRVHOST(ewsContext));
  /* Validate & Get the address type for Host Address */
  if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &type) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbTacacsHostNameTimeOutSet(type, strHostAddr, timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsTimeout_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the TACACS server key string
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  key <key-string>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSKey(EwsContext ewsContext,
                                 L7_uint32 argc,
                                 const L7_char8 * * argv,
                                 L7_uint32 index)
{
  L7_char8 keyString[CLITACACS_KEY_STRING_LEN+1];
  L7_uint32 numArg = 0, unit = 0, ipAddr = 0;
  L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  memset (keyString, 0, sizeof(keyString));
  if (numArg > 1)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_KEY_HELP);
  }
  /* keyString of "", or L7_NULLPTR specifies to use global configuration,
     we already set keyString to "" with bzero, only set if argument specified */
  if ((numArg == 1) &&
      (strcmp(pStrInfo_common_Space, argv[index + numArg]) != 0))
  {
    /* check for the size of the key string. */
    if (strlen(argv[index + numArg]) > CLITACACS_KEY_STRING_LEN)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsKeyRange);
    }
    else
    {
      OSAPI_STRNCPY_SAFE(keyString, argv[index + numArg]);
    }
  }
  unit = cliGetUnitId();
  /* get the current Host Address */
  OSAPI_STRNCPY_SAFE(strHostAddr, EWSTACACSSRVHOST(ewsContext));
  /* Validate & Get the address type for Host Address */
  if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &type) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbTacacsHostNameKeySet(type, strHostAddr, keyString) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsKey);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the TACACS server encrypted key string
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  key <key-string>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSKeyEncrypted(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 * * argv,
                                          L7_uint32 index)
{
  L7_char8 keyString[CLITACACS_KEY_STRING_LEN+1];
  L7_char8 keyEncrypted[(CLITACACS_KEY_STRING_LEN*2)+1];
  L7_uint32 numArg = 0, unit = 0, ipAddr = 0;
  L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  memset (keyEncrypted, 0, sizeof(keyEncrypted));
  OSAPI_STRNCPY_SAFE(keyEncrypted, argv[index + numArg]);

  unit = cliGetUnitId();
  /* get the current Host Address */
  OSAPI_STRNCPY_SAFE(strHostAddr, EWSTACACSSRVHOST(ewsContext));

  /* Validate & Get the address type for Host Address */
  if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &type) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    memset (keyString, 0, sizeof(keyString));
    if (pwDecrypt(keyString, keyEncrypted, CLITACACS_KEY_STRING_LEN) != L7_SUCCESS ||
        usmDbTacacsHostNameKeySet(type, strHostAddr, keyString) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsKey);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the TACACS server priority
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  priority <priority>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSPriority(EwsContext ewsContext,
                                      L7_uint32 argc,
                                      const L7_char8 * * argv,
                                      L7_uint32 index)
{
  L7_uint32 numArg = 0, unit = 0;
  L7_uint32 priority = 0, ipAddr = 0;
  L7_ushort16 priority1 = 0;
  L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (numArg > 1)
  {
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPri_2);
  }
  else if (numArg != 0)
  {
    if (cliConvertTo32BitUnsignedInteger(argv[index + numArg], &priority) !=
        L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPortRange_1);
    }

    /* check for the range of the priority. */
    if (priority < CLITACACS_PRIORITY_RANGE_MIN
        || priority > CLITACACS_PRIORITY_RANGE_MAX)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPortRange_1);
    }
  }
  unit = cliGetUnitId();
  /* get the current Host Address */
  OSAPI_STRNCPY_SAFE(strHostAddr, EWSTACACSSRVHOST(ewsContext));
  /* Validate & Get the address type for Host Address */
  if (cliIPHostAddressValidate(ewsContext, strHostAddr, &ipAddr, &type) != L7_SUCCESS)
  {
    /* Invalid Host Address*/
    return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
  }
  priority1 = (L7_ushort16) priority;
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbTacacsHostNamePrioritySet(type, strHostAddr, priority1) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_base_TacacsPri_1);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the TACACS global key string
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  tacacs-server key <key string>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSGblKey(EwsContext ewsContext,
                                    L7_uint32 argc,
                                    const L7_char8 * * argv,
                                    L7_uint32 index)
{
  L7_char8 keyString[CLITACACS_KEY_STRING_LEN];
  L7_uint32 numArg = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  memset (keyString, 0, sizeof(keyString));
  /* If the command is of type 'normal' the 'if' condition is executed
      otherwise 'else-if' condition is excuted.
   */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_GLOBAL_KEY_HELP);
    }
    else if (numArg == 0)
    {
      OSAPI_STRNCPY_SAFE(keyString, pStrInfo_common_EmptyString);
    }
    else
    {
      /* check for the size of the key string. */
      if (strlen(argv[index + numArg]) > CLITACACS_KEY_STRING_LEN)
      {
        return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_GLOBAL_KEY_RANGE_HELP);
      }
      else
      {
        OSAPI_STRNCPY_SAFE(keyString, argv[index + numArg]);
      }
    }
  } /* end if CLI_NORMAL_CMD */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    OSAPI_STRNCPY_SAFE(keyString, pStrInfo_common_EmptyString);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbTacacsGblKeySet(keyString) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_TACACS_GLOBAL_KEY_HELP);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  To set the TACACS global key string
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  tacacs-server key <key string>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSGblKeyEncrypted(EwsContext ewsContext,
                                             L7_uint32 argc,
                                             const L7_char8 * * argv,
                                             L7_uint32 index)
{
  L7_char8 keyString[CLITACACS_KEY_STRING_LEN];
  L7_char8 keyEncrypted[(CLITACACS_KEY_STRING_LEN*2)+1];
  L7_uint32 numArg = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  memset (keyString, 0, sizeof(keyString));
  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(keyEncrypted, argv[index + numArg]);
    if (pwDecrypt(keyString, keyEncrypted, CLITACACS_KEY_STRING_LEN) != L7_SUCCESS ||
        usmDbTacacsGblKeySet(keyString) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_TACACS_GLOBAL_KEY_HELP);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To set the TACACS server global timeout
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_uint32  argc         @b((input))  number of arguments
* @param    const L7_uint8  **argv  @b((input))  argument pointers
* @param    L7_uint32  index        @b((input))  index
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax  tacacs-server timeout <timeout>
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSGblTimeout(EwsContext ewsContext,
                                        L7_uint32 argc,
                                        const L7_char8 * * argv,
                                        L7_uint32 index)
{
  L7_uchar8 timeout = 0;
  L7_uint32 value = 0;
  L7_uint32 numArg = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  numArg = cliNumFunctionArgsGet();

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (numArg > 1)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_GLOBAL_TIMEOUT_HELP);
    }

    if (numArg == 0)
    {
      value = CLITACACS_DEFAULT_TIMEOUT_VAL;
    }
    else
    {
      if (cliConvertTo32BitUnsignedInteger(argv[index + numArg], &value) !=
          L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_TacacsTimeoutRange);
      }

      /* check for the range of the timeout value. */
      if (value < CLITACACS_TIMEOUT_RANGE_MIN
          || value > CLITACACS_TIMEOUT_RANGE_MAX)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrInfo_base_TacacsTimeoutRange);
      }
    }
  } /* end if CLI_NORMAL_CMD */
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    value = CLITACACS_DEFAULT_TIMEOUT_VAL;
  }

  timeout = (L7_uchar8) value;

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    if (usmDbTacacsGblTimeOutSet(timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPrompt (ewsContext, CLIERROR_TACACS_GLOBAL_TIMEOUT_HELP);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

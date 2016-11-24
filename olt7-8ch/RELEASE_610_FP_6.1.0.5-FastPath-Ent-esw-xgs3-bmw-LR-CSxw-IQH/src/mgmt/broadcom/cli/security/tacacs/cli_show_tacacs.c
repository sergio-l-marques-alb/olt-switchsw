/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/radius/cli_show_tacacs.c
 *
 * @purpose Tacacs show commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  3/5/2007
 *
 * @author  nshrivastav
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "osapi.h"
#include "user_manager_exports.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "clicommands_tacacs.h"

#include "usmdb_tacacs_api.h"
#include "pw_scramble_api.h"
#include "tacacs_exports.h"

/*********************************************************************
*
* @purpose  To display the TACACS server deatils
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
* @cmdsyntax  show tacacs [ip-address|hostname]
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8 *commandTACACSShow(EwsContext ewsContext,
                                  L7_uint32 argc,
                                  const L7_char8 * * argv,
                                  L7_uint32 index)
{
  L7_uint32 numArg = 0;
  L7_uint32 flag = 0, ipAddr = 0,  cmdIndex = 0;
  static L7_char8  strHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8  strNextHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8  strHostParam[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  cmdBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  tempBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 gblTimeout = 0;
  static L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_IP_ADDRESS_TYPE_t typeParam = L7_IP_ADDRESS_TYPE_UNKNOWN;
  static L7_uint32 firstDisplay = L7_TRUE;
  L7_uint32 lineCount = 0;
  L7_uint32 hostLen = L7_NULL;
  L7_uint32 hostNoOfLines = L7_NULL;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  cliCmdScrollSet(L7_FALSE);
  numArg = cliNumFunctionArgsGet();

  /* initialise all the strings */
  memset (buf, 0, sizeof(buf));
  memset (tempBuf, 0, sizeof(tempBuf));
  if (numArg > 1)
  {
    return cliSyntaxReturnPrompt (ewsContext, CLISYNTAX_TACACS_SHOW_HELP);
  }
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  { /* if our question has been answered */
    if(L7_TRUE == cliIsPromptRespQuit())
     {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliPrompt(ewsContext);
     }
  }
  else
  {
    firstDisplay = L7_TRUE;
    type = L7_IP_ADDRESS_TYPE_UNKNOWN;
    memset(strHostAddr, 0, sizeof(strHostAddr));
  }

  if (numArg == 0)
  {
    if (firstDisplay == L7_TRUE)
    {
      /* get the first ip-address information. */
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbTacacsHostNameServerFirstGet(&type, strHostAddr) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
        }
      }

      /* get the global timeout information. */
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbTacacsGblTimeOutGet(&gblTimeout) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
        }
        osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_TacacsShowTimeout);
        osapiSnprintf(tempBuf, sizeof(tempBuf), "%d \r\n", gblTimeout);
        OSAPI_STRNCAT(buf, tempBuf);
        ewsTelnetWrite(ewsContext, buf);
        lineCount++;
      }
      /* print the information header for TACACS server */
      ewsTelnetWrite(ewsContext, CLITACACS_SHOW_HEADING);
      ewsTelnetWrite(ewsContext, CLITACACS_SHOW_HEADING_LINE);
    }
    memset (tempBuf, 0, sizeof(tempBuf));
    memset (buf, 0, sizeof(buf));
    for (;;)
    {
      serverInfoGet(ewsContext, type, strHostAddr, &lineCount);

      /* get the next ip-address information. */
      if (usmDbTacacsHostNameServerNextGet(strHostAddr, &type, strNextHostAddr)
          != L7_SUCCESS)
      {
        ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        return cliSyntaxReturnPrompt (ewsContext, "");
      }
      else
      {
        OSAPI_STRNCPY_SAFE(strHostAddr, strNextHostAddr);
      }
      hostLen = strlen(strHostAddr); 
      hostNoOfLines = hostLen / CLI_MAX_CHARACTERS_PER_ROW;
      if (lineCount + hostNoOfLines >= (CLI_MAX_SCROLL_LINES-6))
      {
        /* Print in next display, if number of lines in next host name
         * exceeds maximum limit.
         */
        firstDisplay = L7_FALSE;
        break;
      }
    }
    cliSetCharInputID(CLI_INPUT_NOECHO, ewsContext, argv);
    
    cliSyntaxBottom(ewsContext);
    
    osapiSnprintf(cmdBuf, sizeof(cmdBuf), argv[0]);
    for (cmdIndex=1; cmdIndex<argc; cmdIndex++)
    {
      OSAPI_STRNCAT(cmdBuf, " ");
      OSAPI_STRNCAT(cmdBuf, argv[cmdIndex]);
    }
    cliAlternateCommandSet(cmdBuf);

    return pStrInfo_common_Name_2;   /* --More-- or (q)uit */
  } /* end else numArg == 0 */
  else if (numArg == 1)
  {
    /* get the first ip-address information. */
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbTacacsHostNameServerFirstGet(&type, strHostAddr) != L7_SUCCESS)
      {
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
      }
    }
    OSAPI_STRNCPY_SAFE(strHostParam, argv[numArg + index]);
    /* check if the ip address entered is existing or not. If NO then
       display error message.
     */
    if (cliIPHostAddressValidate(ewsContext, strHostParam, &ipAddr, &typeParam) != L7_SUCCESS)
    {
      /* Invalid Host Address*/
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
    }

    for (;;)
    {
      if (strcmp(strHostParam, strHostAddr) == 0)
      {
        flag = 1;
        break;
      }
      else
      {
        flag = 0;
        if (usmDbTacacsHostNameServerNextGet(strHostAddr, &type, strNextHostAddr)
            != L7_SUCCESS)
        {
          break;
        }
        OSAPI_STRNCPY_SAFE(strHostAddr, strNextHostAddr);
      }
    }

    if (flag == 1)
    {
      ewsTelnetWrite(ewsContext, CLITACACS_SHOW_HEADING);
      ewsTelnetWrite(ewsContext, CLITACACS_SHOW_HEADING_LINE);
      serverInfoGet(ewsContext, type, strHostParam, &lineCount);
    }
    else
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_TacacsInValHost);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  To display the TACACS server deatils
*
* @param    EwsContext ewsContext   @b((input))  EwsContext
* @param    L7_IP_ADDRESS_TYPE_t type@b((input)) type
* @param    L7_char8 *serverAddress@b((input))  server Address
* @param    L7_uint32        *count@b((output))  server Address
*
* @returntype const L7_char8  *
* @return cliPrompt(ewsContext)
*
* @note
*
* @cmdsyntax
*
* @cmdhelp
*
* @cmddescript
*
* @end
*
*********************************************************************/
L7_char8 * serverInfoGet(EwsContext ewsContext, L7_IP_ADDRESS_TYPE_t type,
                         L7_uchar8 *serverAddress, L7_uint32 *count)
{
  L7_char8 tempBuf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 splitLabel[L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 tempVal = 0;
  L7_ushort16 priority = 0;
  L7_uchar8 timeOut = 0;
  L7_uchar8 *hostAddrTraverse = L7_NULLPTR;
  L7_BOOL firstRow = L7_TRUE;
  L7_uint32 lineCount = 0;

  memset (splitLabel, 0, sizeof(splitLabel));
  lineCount = *count;

  hostAddrTraverse = serverAddress;

  while (*hostAddrTraverse  != L7_EOS)
  {
    if (firstRow == L7_TRUE)
    {
      /* server Host address information. */
      memset (buf, 0, sizeof(buf));
      cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
      osapiSnprintf(buf, sizeof(buf), "\r\n%-27s", splitLabel);

      /* getting the port information. */
      memset (tempBuf, 0, sizeof(tempBuf));
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbTacacsHostNamePortNumberGet(type, serverAddress, &tempVal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
        }
        osapiSnprintf(tempBuf, sizeof(tempBuf), "%-8d", tempVal);
        OSAPI_STRNCAT(buf, tempBuf);
      }

      /* getting the timeout information. */
      memset (tempBuf, 0, sizeof(tempBuf));
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        L7_BOOL useGlobal = L7_FALSE;
        if (usmDbTacacsHostNameServerUseGlobalTimeoutGet(type, serverAddress, &useGlobal) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
        }
        if (useGlobal == L7_TRUE)
        {
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%-10s", pStrInfo_common_TacacsShowGlobal);
        }
        else
        {
          if (usmDbTacacsHostNameTimeOutGet(type, serverAddress, &timeOut) != L7_SUCCESS)
          {
            return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
          }
          osapiSnprintf(tempBuf, sizeof(tempBuf), "%-10d", timeOut);
        }
        OSAPI_STRNCAT(buf, tempBuf);
      }

      /* getting the priority information. */
      memset (tempBuf, 0, sizeof(tempBuf));
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbTacacsHostNamePriorityGet(type, serverAddress, &priority) != L7_SUCCESS)
        {
          return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_base_TacacsShow_1);
        }
        osapiSnprintf(tempBuf, sizeof(tempBuf), "%-10d", priority);
        OSAPI_STRNCAT(buf, tempBuf);
      }

      lineCount++;
      ewsTelnetWrite(ewsContext, buf);
      firstRow = L7_FALSE;
    } /* end of if (firstRow == L7_TRUE)*/
    else
    {
      /* Displaying remaining hostname in second Row */
      memset (buf, 0, sizeof(buf));
      memset (splitLabel, 0, sizeof(splitLabel));
      /* Get the split hostname which fits in available space 
       * (24 characters) in a row .
       */
      cliSplitLongNameGet(&hostAddrTraverse, splitLabel);
      osapiSnprintf(buf, sizeof(buf), "\r\n%-27s", splitLabel);
      lineCount++;
      ewsTelnetWrite(ewsContext, buf);
    }
  }
  *count = lineCount;
  memset (buf, 0, sizeof(buf));
  memset (tempBuf, 0, sizeof(tempBuf));
  return cliPrompt(ewsContext);
}

/*********************************************************************
 * @purpose  To print the running configuration of TACACS
 *
 * @param    EwsContext  ewsContext    @b((input))
 * @param    L7_uint32   unit          @b((input))   unit number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t cliRunningGlobalConfigTACACSInfo(EwsContext ewsContext,
    L7_uint32 unit)
{
  L7_uint32 tempIp = 0;
  L7_char8  hostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8  nextHostAddr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 buf [L7_CLI_MAX_STRING_LENGTH], temp[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 bufEncrypted [L7_CLI_MAX_STRING_LENGTH + (L7_TACACS_KEY_LEN_MAX*2)];
  L7_char8 tempEncrypted[L7_TACACS_KEY_LEN_MAX*2];
  L7_char8 bufTemp [L7_CLI_MAX_STRING_LENGTH];
  L7_ushort16 priority = 0;
  L7_ushort16 port = 0;
  L7_BOOL useGlobal = L7_FALSE;  /* flag for global vs. server configurations */
  L7_uchar8 timeout = 0;
  L7_IP_ADDRESS_TYPE_t type = L7_IP_ADDRESS_TYPE_UNKNOWN;

  memset(temp, 0, sizeof(temp));
  if (usmDbTacacsHostNameServerFirstGet(&type, hostAddr) == L7_SUCCESS)
  {
    for(;;)
    {
      OSAPI_STRNCPY_SAFE(temp, hostAddr);
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufTemp, sizeof(bufTemp), pStrInfo_base_TacacsSrvrHost, temp);
      EWSWRITEBUFFER(ewsContext, bufTemp);

      if (usmDbTacacsHostNamePortNumberGet(type, hostAddr, &port) == L7_SUCCESS)
      {
        if (port != FD_TACACS_PLUS_PORT_NUM)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, temp, sizeof(temp), pStrInfo_base_Port_5, port);
          EWSWRITEBUFFER(ewsContext, temp);
        }
      }

      if ((usmDbTacacsHostNameServerUseGlobalTimeoutGet(type, hostAddr, &useGlobal) == L7_SUCCESS) &&
          (useGlobal == L7_FALSE))
      {
        if (usmDbTacacsHostNameTimeOutGet(type, hostAddr, &timeout) == L7_SUCCESS)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, temp, sizeof(temp), pStrInfo_base_Timeout_3, timeout);
          EWSWRITEBUFFER(ewsContext, temp);
        }
      }

      if ((usmDbTacacsHostNameServerUseGlobalKeyGet(type, hostAddr, &useGlobal) == L7_SUCCESS) &&
          (useGlobal == L7_FALSE))
      {
        memset(temp, 0, sizeof(temp));
        if (usmDbTacacsHostNameKeyGet(type, hostAddr, temp) == L7_SUCCESS)
        {
          memset(tempEncrypted, 0, sizeof(tempEncrypted));
          if (pwEncrypt(temp, tempEncrypted, L7_TACACS_KEY_LEN_MAX, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufEncrypted, sizeof(bufEncrypted), pStrInfo_base_Key_4, tempEncrypted);
            EWSWRITEBUFFER(ewsContext, bufEncrypted);
          }
        }
      }

      memset(temp, 0, sizeof(temp));
      if (usmDbTacacsHostNamePriorityGet(type, hostAddr, &priority) == L7_SUCCESS)
      {
        if (priority != FD_TACACS_PLUS_PRIORITY)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_Pri_4, priority);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }

      EWSWRITEBUFFER_ADD_BLANKS (2, 1, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_Exit);
      if (usmDbTacacsHostNameServerNextGet(hostAddr, &type, nextHostAddr) == L7_SUCCESS)
      {
        OSAPI_STRNCPY_SAFE(hostAddr, nextHostAddr);
      }
      else
      {
        break;
      }
    }
  }

  if (usmDbTacacsGblTimeOutGet(&timeout) == L7_SUCCESS)
  {
    if (timeout != FD_TACACS_PLUS_TIMEOUT_VALUE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_TacacsSrvrTimeout, timeout);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  memset(bufTemp, 0, sizeof(bufTemp));
  if (usmDbTacacsGblKeyGet(bufTemp) == L7_SUCCESS)
  {
    if (strcmp(bufTemp, "\0") != 0)
    {
      memset(tempEncrypted, 0, sizeof(tempEncrypted));
      if (pwEncrypt(bufTemp, tempEncrypted, L7_TACACS_KEY_LEN_MAX, L7_PASSWORD_ENCRYPT_ALG) == L7_SUCCESS)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, bufEncrypted, sizeof(bufEncrypted), pStrInfo_base_TacacsSrvrKey, tempEncrypted);
        EWSWRITEBUFFER(ewsContext, bufEncrypted);
      }
    }
  }

  memset (bufTemp, 0, sizeof(bufTemp));
  if (usmDbTacacsGblSrcIpAddrGet(&tempIp) == L7_SUCCESS)
  {
    if (tempIp != FD_TACACS_PLUS_SRC_IP_ADDR)
    {
      usmDbInetNtoa(tempIp, bufTemp);
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_TacacsSrvrSrcIp, bufTemp);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }
  EWSWRITEBUFFER(ewsContext,pStrInfo_common_CrLf);
  return L7_SUCCESS;
}


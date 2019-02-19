/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/security/captive_portal/cli_config_cp.c
*
* @purpose config commands for Captive portal
*
* @component user interface
*
* @comments none
*
* @create  07/19/2007
*
* @author  rjain, rjindal
*
* @end
*
**********************************************************************/

#include "strlib_security_cli.h"
#include "cliapi.h"
#include "l7_common.h"
#include "usmdb_radius_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_util_api.h"
#include "ews.h"
#include "osapi.h"
#include "clicommands_cp.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpim_api.h"
#include "usmdb_cpdm_user_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "captive_portal_defaultconfig.h"
#include "usmdb_cpdm_web_api.h"
#include "clicommands_card.h"
#include "sslt_exports.h"

#ifdef L7_WIRELESS_PACKAGE
#include "usmdb_wdm_network_api.h"
#endif

#ifdef L7_MGMT_SECURITY_PACKAGE
#include "usmdb_sslt_api.h"
#endif

/**********************************************************************
* @purpose  Configure Captive Portal decoded image size
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

* @cmdsyntax  decoded-image-size <bytes>
*
* @cmdhelp  Configure Captive Portal decoded image size.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPDecodedImageSize(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 eoi=0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPDecodedImageSize);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &eoi) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((eoi < 0) || (eoi > L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPDecodedImageSizeOutOfRange, 0, L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPNoDecodedImageSize);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    eoi = 0;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmDecodedImageSizeSet(eoi) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPDecodedImageSizeSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal encoded image text
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

* @cmdsyntax  encoded-image-text <text>
*
* @cmdhelp  Configure Captive Portal image text.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPEncodedImageText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_int32 MAX_LEN = (L7_CLI_MAX_LARGE_STRING_LENGTH*2-1); /* current CLI max */
  L7_char8 buf[MAX_LEN]; /* currently the max */

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPEncodedImageText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPNoEncodedImageText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    {
      if (usmDbCpdmDecodeTextAndImageCreate(L7_NULLPTR) != L7_SUCCESS)
      {
        ewsTelnetWrite (ewsContext, pStrErr_security_CPEncodedImageInitFailure);
        cliSyntaxBottom(ewsContext);
        return NULL;
      }
      ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
      return cliPrompt(ewsContext);
    }
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (strlen(argv[index+1]) > MAX_LEN)
    {
      osapiSnprintf(buf, sizeof(buf), pStrErr_security_CPEncodedImageTextLength, MAX_LEN);
      ewsTelnetWrite(ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(buf, argv[index+1], MAX_LEN);
    buf[MAX_LEN] = '\0';

    if (usmDbCpdmDecodeTextAndImageCreate(buf) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPEncodedImageTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure captive portal authentication timeout
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  authentication timeout <time>
* @cmdsyntax  no authentication timeout
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPAuthTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 timeout = FD_CP_AUTH_SESSION_TIMEOUT;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalAuthTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((timeout < CP_AUTH_SESSION_TIMEOUT_MIN) || (timeout > CP_AUTH_SESSION_TIMEOUT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPGlobalAuthTimeoutOutOfRange, CP_AUTH_SESSION_TIMEOUT_MIN, CP_AUTH_SESSION_TIMEOUT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalNoAuthTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    timeout = FD_CP_AUTH_SESSION_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {

    if (usmDbCpdmGlobalStatusSessionTimeoutSet(timeout) != L7_SUCCESS)
    {

      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalAuthTimeoutSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Enable/Disable Captive portal
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  enable
* @cmdsyntax  no enable
*
* @cmdhelp  Enable/Disable Captive portal.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPEnable(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 mode;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPModeEnable);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPModeDisable);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    mode = L7_DISABLE;
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPModeEnable);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmGlobalModeSet(mode) != L7_SUCCESS)
    {
      if (ewsContext->commType == CLI_NORMAL_CMD)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPModeNotEnabled);
      }
      else if (ewsContext->commType == CLI_NO_CMD)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPModeNotDisabled);
      }
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure additional HTTP port
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  http port <port>
* @cmdsyntax  no http port
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPHTTPPort(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 port=CP_HTTP_PORT_MIN;
  L7_uint32 currentPort;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
#ifdef L7_MGMT_SECURITY_PACKAGE
  L7_uint32 currentDefaultSecurePort;
  L7_uint32 currentAdditionalSecurePort;
  L7_uint32 currentAdminSecurePort;
  L7_uint32 unit = usmDbThisUnitGet();
#endif
  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalHTTPPort);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((port < CP_HTTP_PORT_MIN) || (port > CP_HTTP_PORT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPGlobalHTTPPortOutOfRange, CP_HTTP_PORT_MIN, CP_HTTP_PORT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (L7_SUCCESS != usmDbCpdmGlobalHTTPPortGet(&currentPort))
    {
      currentPort = CP_HTTP_PORT_MIN;
    }

#ifdef L7_MGMT_SECURITY_PACKAGE
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&currentDefaultSecurePort))
    {
      currentDefaultSecurePort = L7_SSLT_SECURE_PORT;
    }
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&currentAdditionalSecurePort))
    {
      currentAdditionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
    }
    if (L7_SUCCESS != usmDbssltSecurePortGet(unit,&currentAdminSecurePort))
    {
      currentAdminSecurePort = FD_SSLT_SECURE_PORT;
    }
#endif
    if ((port != 0) && (port != currentPort) &&
        ((port ==  CP_STANDARD_HTTP_PORT)
#ifdef L7_MGMT_SECURITY_PACKAGE
         || (port == currentDefaultSecurePort)
         || (port == currentAdditionalSecurePort)
         || (port == currentAdminSecurePort)
#endif
         ))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPGlobalHTTPPortAlreadyInUse, port);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalNoHTTPPort);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    port = FD_CP_HTTP_PORT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmGlobalHTTPPortSet(port) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalHTTPPortSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure additional HTTP Secure port
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  https port <port>
* @cmdsyntax  no https port
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPHTTPSecurePort(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
#ifdef L7_MGMT_SECURITY_PACKAGE
  L7_uint32 port=CP_HTTP_SECURE_PORT_MIN;
  L7_uint32 currentPort;
  L7_uint32 currentDefaultSecurePort;
  L7_uint32 currentAdditionalSecurePort;
  L7_uint32 currentAdminSecurePort;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit = usmDbThisUnitGet();

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalHTTPSecurePort);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &port) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((port < CP_HTTP_SECURE_PORT_MIN) || (port > CP_HTTP_SECURE_PORT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPGlobalHTTPPortOutOfRange, CP_HTTP_SECURE_PORT_MIN, CP_HTTP_SECURE_PORT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (L7_SUCCESS != usmDbCpdmGlobalHTTPPortGet(&currentPort))
    {
      currentPort = CP_HTTP_PORT_MIN;
    }
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&currentDefaultSecurePort))
    {
      currentDefaultSecurePort = L7_SSLT_SECURE_PORT;
    }
    if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&currentAdditionalSecurePort))
    {
      currentAdditionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
    }
    if (L7_SUCCESS != usmDbssltSecurePortGet(unit,&currentAdminSecurePort))
    {
      currentAdminSecurePort = FD_SSLT_SECURE_PORT;
    }

    if ((port != 0) && (port != currentAdditionalSecurePort) &&
        ((port ==  CP_STANDARD_HTTP_PORT) ||
         (port == currentPort) ||
         (port == currentDefaultSecurePort) ||
         (port == currentAdminSecurePort)
         ))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPGlobalHTTPPortAlreadyInUse, port);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalNoHTTPSecurePort);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    port = FD_CP_HTTP_AUX_SECURE_PORT2;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmGlobalHTTPSecurePort2Set(port) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalHTTPPortSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
#endif
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure peer switch statistics reporting interval
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  statistics interval <interval>
* @cmdsyntax  no statistics interval
*
* @cmdhelp
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPStatisticsInterval(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 val=FD_CP_PS_STATS_REPORT_INTERVAL;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalStatsInterval);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &val) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks(1,1,0,0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if (((val < CP_PS_STATS_REPORT_INTERVAL_MIN) && (val != 0)) ||
        (val > CP_PS_STATS_REPORT_INTERVAL_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPGlobalStatsIntervalOutOfRange,
                    CP_PS_STATS_REPORT_INTERVAL_MIN, CP_PS_STATS_REPORT_INTERVAL_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalNoStatsInterval);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    val = FD_CP_PS_STATS_REPORT_INTERVAL;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmGlobalStatusPeerSwStatsReportIntervalSet(val) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalStatsIntervalSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Configure Captive Portal traps
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  trapflags {client-auth-failure | client-connect | client-disconnect | client-db-full}
* @cmdsyntax  no trapflags {client-auth-failure | client-connect | client-disconnect | client-db-full}
*
* @cmdhelp  Enable/Disable SNMP traps for CP related events.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPTrapFlags(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 mode = L7_ENABLE;
  CP_TRAP_FLAGS_t flag;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if ((cliNumFunctionArgsGet() != 0) && (cliNumFunctionArgsGet() != 1))
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPTrapflags);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWrite( ewsContext, pStrErr_security_CPNoTrapflags);
    }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  if (cliNumFunctionArgsGet() == 0)
  {
    flag = CP_TRAP_AUTH_FAILURE | CP_TRAP_CLIENT_CONNECTED | CP_TRAP_CLIENT_DISCONNECTED | CP_TRAP_CONNECTION_DB_FULL;
  }
  else if (cliNoCaseCompare((L7_char8 *)argv[index+1], pStrInfo_security_CPTrapflagClientFailure) == L7_TRUE)
  {
    flag = CP_TRAP_AUTH_FAILURE;
  }
  else if (cliNoCaseCompare((L7_char8 *)argv[index+1], pStrInfo_security_CPTrapflagClientConnect) == L7_TRUE)
  {
    flag = CP_TRAP_CLIENT_CONNECTED;
  }
  else if (cliNoCaseCompare((L7_char8 *)argv[index+1], pStrInfo_security_CPTrapflagClientDisconnect) == L7_TRUE)
  {
    flag = CP_TRAP_CLIENT_DISCONNECTED;
  }
  else if (cliNoCaseCompare((L7_char8 *)argv[index+1], pStrInfo_security_CPTrapflagClientDBFull) == L7_TRUE)
  {
    flag = CP_TRAP_CONNECTION_DB_FULL;
  }
  else
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPTrapflags);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPNoTrapflags);
    }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmGlobalTrapModeSet(flag, mode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPTrapflagsSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Enable/Disable sending of Captive Portal traps
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
* @cmdsyntax  snmp-server enable traps captive-portal
* @cmdsyntax  [no] snmp-server enable traps captive-portal
*
* @cmdhelp  Enable/Disable sending of CP traps.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandSnmpServerEnableTrapsCP(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_uint32 mode = L7_ENABLE;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 0)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalTrap);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPNoGlobalTrap);
    }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbTrapCaptivePortalSet(mode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPGlobalTrapSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Block/Unblock traffic on a Captive Portal Configuration
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  block
* @cmdsyntax  no block
*
* @cmdhelp  Block/Unblock traffic on a Captive Portal Configuration.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationBlock(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_CP_INST_BLOCK_STATUS_t blkStatus=L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgBlock);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    blkStatus = L7_CP_INST_BLOCK_STATUS_BLOCKED_PENDING;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoBlock);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    blkStatus = L7_CP_INST_BLOCK_STATUS_NOT_BLOCKED_PENDING;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigBlockedStatusSet(cpId, blkStatus) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgBlockStatusSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Clear the Captive Portal Configuration
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  clear
*
* @cmdhelp  Clear the Captive Portal Configuration.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationClear(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;

  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliGetCharInputID() == CLI_INPUT_EMPTY)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgClear);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      cliSetCharInputID(1, ewsContext, argv);
      cliAlternateCommandSet(pStrErr_security_CPCfgClearCmd);
      return pStrErr_security_CPCfgClearCmdConfirm;
    }
  }
  else if (cliGetCharInputID() == 1)
  {
    if (tolower(cliGetCharInput()) == 'y')
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbCpdmCPConfigReset(cpId) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgClearError);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgClearSuccess);
        cliSyntaxBottom(ewsContext);
      }
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNotCleared);
      ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Enable/Disable Captive Portal configuration
*
* @param EwsContext ewsContext
* @paam L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index
*
* @returntype const L7_char8
* @returns cliPrompt(ewsContext)
*
* @cmdsyntax  enable
* @cmdsyntax  no enable
*
* @cmdhelp  Enable/Disable Captive Portal configuration.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationEnable(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_CP_MODE_STATUS_t mode;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPModeEnable);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    mode = L7_CP_MODE_ENABLED;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPModeDisable);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    mode = L7_CP_MODE_DISABLED;
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPModeEnable);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigModeSet(cpId, mode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgModeSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure captive Portal configuration group ID
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
* @cmdsyntax  group <group-id>
* @cmdsyntax  no group
*
* @cmdhelp  Configure captive Portal configuration group ID.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationGroup(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 gId32;
  CP_VERIFY_MODE_t verifyMode;
  gpId_t gId=0;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgGroup);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &gId32) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((gId32 < GP_ID_MIN) || (gId32 > GP_ID_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgGroupOutOfRange, GP_ID_MIN, GP_ID_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    gId = (L7_ushort16)gId32;

    if (usmDbCpdmUserGroupEntryGet(gId) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_GroupIdDoesNotExist);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoGroup);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    gId = GP_ID_MIN;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigVerifyModeGet(cpId, &verifyMode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgGroupSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (verifyMode != CP_VERIFY_MODE_GUEST)
    {
      if (usmDbCpdmCPConfigGpIdSet(cpId, gId) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgGroupSetError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgGroupUnableToSet);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration idle timeout
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
* @cmdsyntax  idle-timeout <time>
* @cmdsyntax  no idle-timeout
*
* @cmdhelp  Configure Captive Portal configuration idle timeout.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationIdleTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 timeout = FD_CP_AUTH_SESSION_TIMEOUT;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgIdleTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((timeout < CP_USER_LOCAL_IDLE_TIMEOUT_MIN) || (timeout > CP_USER_LOCAL_IDLE_TIMEOUT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgIdleTimeoutOutOfRange, CP_USER_LOCAL_IDLE_TIMEOUT_MIN, CP_USER_LOCAL_IDLE_TIMEOUT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoIdleTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    timeout = CP_DEF_IDLE_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigIdleTimeoutSet(cpId, timeout) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgIdleTimeoutSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
* @purpose  Associate an interface to a Captive Portal configuration
*
* @param EwsContext ewsContext
* @param L7_uint32 argc
* @param const L7_char8 **argv
* @param L7_uint32 index*
*
* @returntype const L7_char8  *
* @returns cliPrompt(ewsContext)
*
* @notes
*
* @cmdsyntax  interface <intf>
* @cmdsyntax  no interface <intf>
*
* @cmdhelp  Associate an interface to a Captive Portal configuration.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationInterface(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0, tmpCpId;
  L7_uint32 intf, u, s, p;
  L7_RC_t rc;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgInterface);
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoInterface);
    }
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliIsStackingSupported() == L7_TRUE)
  {
    if ((rc = cliValidSpecificUSPCheck(argv[index+1], &u, &s, &p)) != L7_SUCCESS)
    {
      if (rc == L7_ERROR)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
        ewsTelnetPrintf (ewsContext, "%u/%u/%u", u, s, p);
      }
      else if (rc == L7_NOT_EXIST)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_UnitDoesntExist);
      }
      else if (rc == L7_NOT_SUPPORTED)
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_common_SlotDoesntExist);
      }
      else
      {
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidInput);
      }
    }

    if (usmDbIntIfNumFromUSPGet(u, s, p, &intf) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrErr_common_InvalidSlotPort_1);
    }
  }
  else
  {
    if (cliSlotPortToIntNum(ewsContext, argv[index+1], &s, &p, &intf) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
  }

  if (usmDbCpimIntfGet(intf) != L7_SUCCESS)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPIntfDoesNotExist);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (ewsContext->commType == CLI_NORMAL_CMD)
    {
      if (usmDbCpdmCPConfigIntIfNumFind(intf, &tmpCpId) == L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgInterfaceAlreadyAssoc);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      if (usmDbCpdmCPConfigIntIfNumAdd(cpId, intf) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgInterfaceSetError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else if (ewsContext->commType == CLI_NO_CMD)
    {
      if (usmDbCpdmCPConfigIntIfNumGet(cpId, intf) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgInterfaceNotAssoc);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }


      if (usmDbCpdmCPConfigIntIfNumDelete(cpId, intf) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgInterfaceDeleteError);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration name
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

* @cmdsyntax  name <cp-name>
* @cmdsyntax  no name
*
* @cmdhelp  Configure Captive Portal configuration name.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationName(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_char8 name[CP_NAME_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgName);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliIsAlphaNum((L7_char8 *)argv[index+1]) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNameAlphaNumeric);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (strlen(argv[index+1]) > CP_NAME_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgNameLength, CP_NAME_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(name, argv[index+1], CP_NAME_MAX);
    name[CP_NAME_MAX] = '\0';
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoName);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(name, CP_DEF_NAME, CP_NAME_MAX);
    name[CP_NAME_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigNameSet(cpId, name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNameSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration protocol
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
* @cmdsyntax  protocol <http | https>
*
* @cmdhelp  Configure Captive Portal configuration protocol.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationProtocol(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_LOGIN_TYPE_t protocol=L7_LOGIN_TYPE_HTTP;

  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgProtocol);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strcmp((L7_char8 *)argv[index+1], pStrInfo_common_Http_1) == 0)
  {
    protocol = L7_LOGIN_TYPE_HTTP;
  }
  else if (strcmp((L7_char8 *)argv[index+1], "https") == 0)
  {
    protocol = L7_LOGIN_TYPE_HTTPS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigProtocolModeSet(cpId, protocol) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgProtocolSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Enable/Disable Captive Portal configuration user logout mode
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
* @cmdsyntax  user-logout
* @cmdsyntax  no user-logout
*
* @cmdhelp  Enable/Disable Captive Portal configuration user logout mode.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationUserLogout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uchar8 userLogoutMode;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgUserLogout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    userLogoutMode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoUserLogout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    userLogoutMode = L7_DISABLE;
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgUserLogout);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigUserLogoutModeSet(cpId, userLogoutMode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgUserLogoutSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Enable/Disable Captive Portal configuration redirect mode
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
* @cmdsyntax  redirect
* @cmdsyntax  no redirect
*
* @cmdhelp  Enable/Disable Captive Portal configuration redirect mode.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationRedirect(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uchar8 redirectMode;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRedirect);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    redirectMode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoRedirect);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    redirectMode = L7_DISABLE;
  }
  else
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRedirect);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigRedirectModeSet(cpId, redirectMode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRedirectSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration redirect URL
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
* @cmdsyntax  redirect-url <url>
*
* @cmdhelp  Configure Captive Portal configuration redirect URL.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationRedirectUrl(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uchar8 redirectMode;
  L7_char8 redirectUrl[CP_WELCOME_URL_MAX+1];

  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRedirectURL);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strlen(argv[index+1]) > CP_WELCOME_URL_MAX)
  {
    osapiSnprintf(redirectUrl, sizeof(redirectUrl), pStrErr_security_CPCfgRedirectURLLength, CP_WELCOME_URL_MAX);
    ewsTelnetWrite(ewsContext, redirectUrl);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  osapiStrncpy(redirectUrl, argv[index+1], CP_WELCOME_URL_MAX);
  redirectUrl[CP_WELCOME_URL_MAX] = '\0';

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if ((usmDbCpdmCPConfigRedirectModeGet(cpId, &redirectMode) != L7_SUCCESS) || 
        (redirectMode != L7_ENABLE))
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRedirectModeNotEnabled);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (usmDbCpdmCPConfigRedirectURLSet(cpId, redirectUrl) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRedirectURLSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration session timeout
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
* @cmdsyntax  session-timeout <time>
* @cmdsyntax  no session-timeout
*
* @cmdhelp  Configure Captive Portal configuration session timeout.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationSessionTimeout(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 timeout = FD_CP_AUTH_SESSION_TIMEOUT;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgSessionTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &timeout) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((timeout < CP_USER_LOCAL_SESSION_TIMEOUT_MIN) || (timeout > CP_USER_LOCAL_SESSION_TIMEOUT_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgSessionTimeoutOutOfRange, CP_USER_LOCAL_SESSION_TIMEOUT_MIN, CP_USER_LOCAL_SESSION_TIMEOUT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoSessionTimeout);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    timeout = CP_DEF_SESSION_TIMEOUT;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigSessionTimeoutSet(cpId, timeout) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgSessionTimeoutSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration max bandwidth up
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
* @cmdsyntax  max-bandwidth-up <bytes/sec>
* @cmdsyntax  no max-bandwidth-up
*
* @cmdhelp  Configure Captive Portal configuration max bandwidth up.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationMaxBandwidthUp(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 bytes=CP_USER_LOCAL_MAX_BW_UP_MIN;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgMaxBandwidthUp);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_BW_UP_MIN) || (bytes > CP_USER_LOCAL_MAX_BW_UP_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_BW_UP_MIN, CP_USER_LOCAL_MAX_BW_UP_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoMaxBandwidthUp);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_UP_RATE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* convert from bytes to bits */
    if (usmDbCpdmCPConfigUserUpRateSet(cpId, bytes*8) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration max bandwidth down
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
* @cmdsyntax  max-bandwidth-down <bytes/sec>
* @cmdsyntax  no max-bandwidth-down
*
* @cmdhelp  Configure Captive Portal configuration max bandwidth down.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationMaxBandwidthDown(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 bytes = CP_USER_LOCAL_MAX_BW_DOWN_MIN;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgMaxBandwidthDown);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_BW_DOWN_MIN) || (bytes > CP_USER_LOCAL_MAX_BW_DOWN_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_BW_DOWN_MIN, CP_USER_LOCAL_MAX_BW_DOWN_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoMaxBandwidthDown);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_DOWN_RATE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    /* convert from bytes to bits */
    if (usmDbCpdmCPConfigUserDownRateSet(cpId, bytes*8) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration max input octets
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
* @cmdsyntax  max-input-octets <bytes>
* @cmdsyntax  no max-input-octers
*
* @cmdhelp  Configure Captive Portal configuration max input octets.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationMaxInputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 bytes = CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgMaxInputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN) || (bytes > CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_INPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_INPUT_OCTETS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoMaxInputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_INPUT_OCTETS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigMaxInputOctetsSet(cpId, bytes) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration max output octets
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
* @cmdsyntax  max-output-octets <bytes>
* @cmdsyntax  no max-output-octets
*
* @cmdhelp  Configure Captive Portal configuration max output octets.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationMaxOutputOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 bytes = CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgMaxOutputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN) || (bytes > CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MIN, CP_USER_LOCAL_MAX_OUTPUT_OCTETS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoMaxOutputOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_USER_OUTPUT_OCTETS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigMaxOutputOctetsSet(cpId, bytes) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration max total octets
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
* @cmdsyntax  max-total-octets <bytes>
* @cmdsyntax  no max-total-octets
*
* @cmdhelp  Configure Captive Portal configuration max total octets.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationMaxTotalOctets(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_uint32 bytes = CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgMaxTotalOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (cliConvertTo32BitUnsignedInteger(argv[index+1], &bytes) != L7_SUCCESS)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_common_InvalidInteger);
    }

    if ((bytes < CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN) || (bytes > CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPBytesOutOfRange, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MIN, CP_USER_LOCAL_MAX_TOTAL_OCTETS_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoMaxTotalOctets);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    bytes = CP_DEF_TOTAL_OCTETS;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigMaxTotalOctetsSet(cpId, bytes) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPBytesSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration verification mode
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
* @cmdsyntax  verification <guest | local | radius>
*
* @cmdhelp  Configure Captive Portal configuration verification mode.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationVerifyMode(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  CP_VERIFY_MODE_t verifyMode;

  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() != 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgVerification);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (strcmp((L7_char8 *)argv[index+1], pStrInfo_common_LocalAuth) == 0)
  {
    verifyMode = CP_VERIFY_MODE_LOCAL;
  }
  else if (strcmp((L7_char8 *)argv[index+1], pStrInfo_common_RadiusAuth) == 0)
  {
    verifyMode = CP_VERIFY_MODE_RADIUS;
  }
  else
  {
    verifyMode = CP_VERIFY_MODE_GUEST;
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigVerifyModeSet(cpId, verifyMode) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgVerificationSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal RADIUS authentication server
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

* @cmdsyntax  radius-auth-server <server-name>
* @cmdsyntax  no radius-auth-server
*
* @cmdhelp  Configure Captive Portal RADIUS authentication server
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationRadiusAuthServer(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 name[CP_RADIUS_AUTH_SERVER_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 unit;
  cpId_t cpId=0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRadiusAuthServer);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (strlen(argv[index+1]) > CP_RADIUS_AUTH_SERVER_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgNameLength, CP_RADIUS_AUTH_SERVER_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(name, argv[index+1], CP_RADIUS_AUTH_SERVER_MAX);
    name[CP_RADIUS_AUTH_SERVER_MAX] = '\0';

    /* Verify that the server name has the correct format */
    if (L7_SUCCESS != usmDbStringAlphaNumericPlusCheck(name, "_- ", L7_NULLPTR))
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRadiusSrvrMustBeAlphaNumericPlus);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgNoRadiusAuthServer);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /* Get the primary RADIUS server */
    unit = usmDbThisUnitGet();
    memset(name,0,sizeof(name));
    usmDbRadiusCurrentAuthHostNameServerGet(unit,name);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigRadiusAuthServerSet(cpId, name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgRadiusAuthServerSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal foreground (hex or named) color
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

* @cmdsyntax  foreground-color <color-code> 
* @cmdsyntax  no foreground-color
*
* @cmdhelp  Configure Captive Portal foreground color
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationForegroundColor(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_char8 color[CP_FOREGROUND_COLOR_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgForegroundColor);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (strlen(argv[index+1]) > CP_FOREGROUND_COLOR_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgForegroundColorLength, CP_FOREGROUND_COLOR_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(color, argv[index+1], CP_FOREGROUND_COLOR_MAX);
    color[CP_FOREGROUND_COLOR_MAX] = '\0';
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgForegroundColorNoForegroundColor);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(color, CP_DEF_FOREGROUND_COLOR, CP_FOREGROUND_COLOR_MAX);
    color[CP_FOREGROUND_COLOR_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigForegroundColorSet(cpId, color) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgForegroundColorSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal background (hex or named) color
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

* @cmdsyntax  background-color <color-code> 
* @cmdsyntax  no background-color
*
* @cmdhelp  Configure Captive Portal background color
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationBackgroundColor(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_char8 color[CP_BACKGROUND_COLOR_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgBackgroundColor);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (strlen(argv[index+1]) > CP_BACKGROUND_COLOR_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgBackgroundColorLength, CP_BACKGROUND_COLOR_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(color, argv[index+1], CP_BACKGROUND_COLOR_MAX);
    color[CP_BACKGROUND_COLOR_MAX] = '\0';
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgBackgroundColorNoBackgroundColor);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(color, CP_DEF_BACKGROUND_COLOR, CP_BACKGROUND_COLOR_MAX);
    color[CP_BACKGROUND_COLOR_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigBackgroundColorSet(cpId, color) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgBackgroundColorSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal separator (hex or named) color
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

* @cmdsyntax  separator-color <color-code> 
* @cmdsyntax  no separator-color
*
* @cmdhelp  Configure Captive Portal separator color
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPConfigurationSeparatorColor(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  cpId_t cpId=0;
  L7_char8 color[CP_SEPARATOR_COLOR_MAX+1];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgSeparatorColor);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    if (strlen(argv[index+1]) > CP_SEPARATOR_COLOR_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPCfgSeparatorColorLength, CP_SEPARATOR_COLOR_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(color, argv[index+1], CP_SEPARATOR_COLOR_MAX);
    color[CP_SEPARATOR_COLOR_MAX] = '\0';
  }

  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgSeparatorColorNoSeparatorColor);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    osapiStrncpy(color, CP_DEF_SEPARATOR_COLOR, CP_SEPARATOR_COLOR_MAX);
    color[CP_SEPARATOR_COLOR_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigSeparatorColorSet(cpId, color) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPCfgSeparatorColorSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Deauthenticate a Captive Portal client
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
* @cmdsyntax  captive-portal client deauthenticate <macaddr>
*
* @cmdhelp  Deauthenticate a Captive Portal client.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPClientDeauthenticate(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  static L7_enetMacAddr_t macAddr;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 cpId32 = 0;
  L7_ushort16 cpId = 0;
  L7_BOOL fIsCpId = L7_FALSE;
  L7_BOOL fIsMacAddr = L7_FALSE;

  cliSyntaxTop(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (cliNumFunctionArgsGet() > 1)
  {
    ewsTelnetWrite(ewsContext, pStrErr_security_CPClientDeauthenticate);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if (cliNumFunctionArgsGet() != 0)
  {
    cpId32 = atoi((L7_char8 *)argv[index+1]);
    cpId = (L7_ushort16)cpId32;
    fIsCpId = (CP_ID_MIN<=cpId && CP_ID_MAX>=cpId)?L7_TRUE:L7_FALSE;

    if (L7_TRUE != fIsCpId)
    {
      memset(&macAddr, 0x00, sizeof(macAddr));
      fIsMacAddr = (cliConvertMac((L7_uchar8 *)argv[index+1], macAddr.addr))?L7_TRUE:L7_FALSE;
    }

    if ((L7_TRUE != fIsCpId) && (L7_TRUE != fIsMacAddr))
    {
      return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR, ewsContext, pStrErr_security_CPClientDeauthenticateEither);
    }
  }

  if (L7_TRUE == fIsMacAddr)
  {
    if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
    {
      if (usmDbCpdmClientConnStatusGet(&macAddr) != L7_SUCCESS)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPClientNotFound);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }

    if ((cliGetCharInputID() == CLI_INPUT_EMPTY) && (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING))
    {
      cliSetCharInputID(U_IDX, ewsContext, argv);
      osapiSnprintf(stat, sizeof(stat), pStrInfo_security_CPClientDeauthCmd, argv[index+1]);
      cliAlternateCommandSet(stat);
      return pStrInfo_security_CPClientDeauthVerify;
    }
    else if ((tolower(cliGetCharInput()) == 'y') || (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_RUNNING))
    {
      if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
      {
        if (usmDbCpdmClientConnStatusDelete(&macAddr) != L7_SUCCESS)
        {
          ewsTelnetWrite(ewsContext, pStrErr_security_CPClientDeleteFailed);
          cliSyntaxBottom(ewsContext);
          return cliPrompt(ewsContext);
        }
        ewsTelnetWrite(ewsContext, pStrErr_security_CPClientDeleteSuccessful);
      }
    }
    else
    {
      if (ewsContext->configScriptStateFlag == L7_CONFIG_SCRIPT_NOT_RUNNING)
      {
        ewsTelnetWrite(ewsContext, pStrErr_security_CPClientNotDeleted);
      }
    }
  }
  else if (L7_TRUE == fIsCpId)
  {
    if (L7_SUCCESS != usmDbCpdmClientConnStatusByCPIdDeleteAll(cpId))
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPClientsDeauthFailed);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(stat,0,sizeof(stat));
    osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPClientsDeauthSuccessful, cpId);
    ewsTelnetWrite(ewsContext, stat);
  }
  else
  {
    if (L7_SUCCESS != usmDbCpdmClientConnStatusDeleteAll())
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPClientsDeauthFailed);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    ewsTelnetWrite(ewsContext, pStrErr_security_CPClientsDeauthAllSuccessful);
  }

  ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
  cliSyntaxBottom(ewsContext);

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Account image
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

* @cmdsyntax  account-image <image-name>
* @cmdsyntax  no account-image
*
* @cmdhelp  Configure Captive Portal account image.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleAccountImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 name[CP_FILE_NAME_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAccountImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_FILE_NAME_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleAccountImageLength, CP_FILE_NAME_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(name, argv[index+1], CP_FILE_NAME_MAX);
    name[CP_FILE_NAME_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleNoAccountImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(name,0,sizeof(name));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebAccountImageNameSet(cpId,webId,name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAccountImageSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration account label
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

* @cmdsyntax  account-label <UTF-16>
* @cmdsyntax  no account-label
*
* @cmdhelp  Configure Captive Portal account label
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleAccountLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 label[CP_ACCOUNT_LABEL_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAccountLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_ACCOUNT_LABEL_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleAccountLabelLength, CP_ACCOUNT_LABEL_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, argv[index+1], CP_ACCOUNT_LABEL_MAX);
    label[CP_ACCOUNT_LABEL_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAccountLabelNoAccountLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(label,0,sizeof(label));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebAccountLabelSet(cpId,webId,label) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAccountLabelSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration accept text
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

* @cmdsyntax  accept-text <UTF-16>
* @cmdsyntax  no accept-text
*
* @cmdhelp  Configure Captive Portal accept text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleAcceptText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_ACCEPT_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAcceptText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_ACCEPT_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleAcceptTextNoAcceptText, CP_ACCEPT_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_ACCEPT_TEXT_MAX);
    text[CP_ACCEPT_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAcceptTextNoAcceptText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebAcceptTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAcceptTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration AUP text
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

* @cmdsyntax  aup-text <UTF-16>
* @cmdsyntax  no aup-text
*
* @cmdhelp  Configure Captive Portal AUP text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleAUPText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[(L7_CLI_MAX_LARGE_STRING_LENGTH*2)-1]; /* currently the max */
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAUPText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > sizeof(text))
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleAUPTextLength, CP_AUP_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
    osapiStrncpy(text, argv[index+1], sizeof(text));
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAUPTextNoAUPText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
    usmDbCpdmCPConfigWebAUPTextSet(cpId,webId,text);
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebAUPTextAppend(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAUPTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Button Label
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

* @cmdsyntax  button-label <UTF-16>
* @cmdsyntax  no button-label
*
* @cmdhelp  Configure Captive Portal Button label
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleButtonLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 label[CP_BUTTON_LABEL_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleButtonLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_BUTTON_LABEL_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleButtonLabelLength, CP_BUTTON_LABEL_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, argv[index+1], CP_BUTTON_LABEL_MAX);
    label[CP_BUTTON_LABEL_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleButtonLabelNoButtonLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, WEB_DEF1_BUTTON_LABEL, CP_BUTTON_LABEL_MAX);
    label[CP_BUTTON_LABEL_MAX] = '\0';  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebButtonLabelSet(cpId,webId,label) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleButtonLabelSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Branding image
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

* @cmdsyntax  branding-image <image-name>
* @cmdsyntax  no branding-image
*
* @cmdhelp  Configure Captive Portal Branding image
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleBrandingImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 name[CP_FILE_NAME_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBrandingImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_FILE_NAME_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleBrandingImageLength, CP_FILE_NAME_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(name, argv[index+1], CP_FILE_NAME_MAX);
    name[CP_FILE_NAME_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleNoBrandingImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(name,0,sizeof(name));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebBrandingImageNameSet(cpId,webId,name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBrandingImageSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Background image
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

* @cmdsyntax  background-image <image-name>
* @cmdsyntax  no background-image
*
* @cmdhelp  Configure Captive Portal Background image
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleBackgroundImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 name[CP_FILE_NAME_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBackgroundImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_FILE_NAME_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleBackgroundImageLength, CP_FILE_NAME_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(name, argv[index+1], CP_FILE_NAME_MAX);
    name[CP_FILE_NAME_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleNoBackgroundImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(name,0,sizeof(name));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebBackgroundImageNameSet(cpId,webId,name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBackgroundImageSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Success Background image
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

* @cmdsyntax  logout-success-background-image <image-name>
* @cmdsyntax  no logout-success-background-image
*
* @cmdhelp  Configure Captive Portal Logout Success Background image
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutSuccessBackgroundImage(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 name[CP_FILE_NAME_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessBackgroundImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_FILE_NAME_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutSuccessBackgroundImageLength, CP_FILE_NAME_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(name, argv[index+1], CP_FILE_NAME_MAX);
    name[CP_FILE_NAME_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleNoLogoutSuccessBackgroundImage);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(name,0,sizeof(name));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameSet(cpId,webId,name) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessBackgroundImageSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Browser title
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

* @cmdsyntax  browser-title <UTF-16>
* @cmdsyntax  no browser-title
*
* @cmdhelp  Configure Captive Portal browser title.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleBrowserTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 title[CP_BROWSER_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBrowserTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_BROWSER_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleBrowserTitleLength, CP_BROWSER_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(title, argv[index+1], CP_BROWSER_TITLE_TEXT_MAX);
    title[CP_BROWSER_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBrowserTitleNoBrowserTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(title,0,sizeof(title));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebBrowserTitleTextSet(cpId,webId,title) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleBrowserTitleSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration locale code
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

* @cmdsyntax  code <locale-code>
* @cmdsyntax  no code
*
* @cmdhelp  Configure Captive Portal locale code.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleCode(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 langCode[CP_LANG_CODE_MAX+1];
  L7_char8 code[CP_LANG_CODE_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;
  L7_RC_t rc = L7_FAILURE;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleCode);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_LANG_CODE_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleCodeLength, CP_LANG_CODE_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(code, argv[index+1], CP_LANG_CODE_MAX);
    code[CP_LANG_CODE_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleCodeNoCode);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(code, WEB_DEF1_LANG_CODE, CP_LANG_CODE_MAX);
    code[CP_LANG_CODE_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    memset(langCode,0,sizeof(langCode));
    rc = usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,langCode);
    if (osapiStrncmp(langCode,code,CP_LANG_CODE_MAX) != 0)
    {
      rc = usmDbCpdmCPConfigWebAddDefaults(cpId,webId,code,CP_ALL);
    }
    if (L7_SUCCESS != rc)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleCodeSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Denied message
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

* @cmdsyntax  denied-msg <UTF-16>
* @cmdsyntax  no denied-msg
*
* @cmdhelp  Configure Captive Portal browser title.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleDeniedMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 msg[CP_MSG_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleDeniedMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_MSG_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleDeniedMsgLength, CP_MSG_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, argv[index+1], CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleDeniedMsgNoDeniedMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, WEB_DEF1_DENIED_MSG, CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebDeniedMsgTextSet(cpId,webId,msg) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleDeniedMsgSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Font List
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

* @cmdsyntax  font-list comma-separated list
* @cmdsyntax  no font-list
*
* @cmdhelp  Configure Captive Portal Font List
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleFontList(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 fontList[CP_FONT_LIST_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() > 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleFontList);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    memset(fontList,0,sizeof(fontList));
    if (cliNumFunctionArgsGet() == 0)
    {
      if (strlen(argv[index+1]) > CP_FONT_LIST_MAX)
      {
        osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleFontListLength, CP_FONT_LIST_MAX);
        ewsTelnetWrite(ewsContext, stat);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
      osapiStrncpy(fontList, argv[index+1], CP_FONT_LIST_MAX);
      fontList[CP_FONT_LIST_MAX] = '\0';
    }
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleFontListNoFontList);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(fontList, WEB_DEF1_FONT_LIST, CP_FONT_LIST_MAX);
    fontList[CP_FONT_LIST_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebFontListSet(cpId,webId,fontList) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleFontListSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Instructional text
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

* @cmdsyntax  instructional-text <UTF-16>
* @cmdsyntax  no instructional-text
*
* @cmdhelp  Configure Captive Portal Instructional text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleInstructionalText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_INSTRUCTIONAL_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleInstructionalText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_INSTRUCTIONAL_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleInstructionalTextLength, CP_INSTRUCTIONAL_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text, 0, sizeof(text));
    osapiStrncpy(text, argv[index+1], CP_INSTRUCTIONAL_TEXT_MAX);

  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleInstructionalTextNoInstructionalText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
    usmDbCpdmCPConfigWebInstructionalTextSet(cpId,webId,text);
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebInstructionalTextAppend(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleInstructionalTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration locale link
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

* @cmdsyntax  link <locale-link>
* @cmdsyntax  no link
*
* @cmdhelp  Configure Captive Portal locale link identifier.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLink(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 link[CP_LOCALE_LINK_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLink);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_LOCALE_LINK_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLinkLength, CP_LOCALE_LINK_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(link, argv[index+1], CP_LOCALE_LINK_MAX);
    link[CP_LOCALE_LINK_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLinkNoCode);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(link, WEB_DEF1_LOCALE_LINK, CP_LOCALE_LINK_MAX);
    link[CP_LOCALE_LINK_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,link) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLinkSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Accept message
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

* @cmdsyntax  noaccept-msg <UTF-16>
* @cmdsyntax  no accept-msg
*
* @cmdhelp  Configure Captive Portal noaccept message.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleAcceptMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 msg[CP_MSG_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAcceptMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_MSG_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleAcceptMsgLength, CP_MSG_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, argv[index+1], CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAcceptMsgNoAcceptMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(msg,0,sizeof(msg));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
      
    if (usmDbCpdmCPConfigWebNoAcceptMsgTextSet(cpId,webId,msg) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleAcceptMsgSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Password label
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

* @cmdsyntax  password-label <UTF-16>
* @cmdsyntax  no password-label
*
* @cmdhelp  Configure Captive Portal Password label
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalePasswordLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 label[CP_PASSWORD_LABEL_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocalePasswordLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_PASSWORD_LABEL_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocalePasswordLabelLength, CP_PASSWORD_LABEL_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, argv[index+1], CP_PASSWORD_LABEL_MAX);
    label[CP_PASSWORD_LABEL_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocalePasswordLabelNoPasswordLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(label,0,sizeof(label));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  { 
    if (usmDbCpdmCPConfigWebPasswordLabelSet(cpId,webId,label) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocalePasswordLabelSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Resource message
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

* @cmdsyntax  resource-msg <UTF-16>
* @cmdsyntax  no resource-msg
*
* @cmdhelp  Configure Captive Portal resource message.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleResourceMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 msg[CP_MSG_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleResourceMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_MSG_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleResourceMsgLength, CP_MSG_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, argv[index+1], CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleResourceMsgNoResourceMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, WEB_DEF1_RESOURCE_MSG, CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
      
    if (usmDbCpdmCPConfigWebResourceMsgTextSet(cpId,webId,msg) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleResourceMsgSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Title text
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

* @cmdsyntax  title-text <UTF-16>
* @cmdsyntax  no title-text
*
* @cmdhelp  Configure Captive Portal Title text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleTitleText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleTitleText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleTitleTextLength, CP_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_TITLE_TEXT_MAX);
    text[CP_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleTitleTextNoTitleText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebTitleTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleTitleTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Timeout message
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

* @cmdsyntax  timeout-msg <UTF-16>
* @cmdsyntax  no timeout-msg
*
* @cmdhelp  Configure Captive Portal timeout message.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleTimeoutMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 msg[CP_MSG_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleTimeoutMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_MSG_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleTimeoutMsgLength, CP_MSG_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, argv[index+1], CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleTimeoutMsgNoTimeoutMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, WEB_DEF1_TIMEOUT_MSG, CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebTimeoutMsgTextSet(cpId,webId,msg) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleTimeoutMsgSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration User label
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

* @cmdsyntax  user-label <UTF-16>
* @cmdsyntax  no user-label
*
* @cmdhelp  Configure Captive Portal User label
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleUserLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 label[CP_USER_LABEL_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleUserLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_USER_LABEL_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleUserLabelLength, CP_USER_LABEL_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, argv[index+1], CP_USER_LABEL_MAX);
    label[CP_USER_LABEL_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleUserLabelNoUserLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(label,0,sizeof(label));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebUserLabelSet(cpId,webId,label) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleUserLabelSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Welcome title
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

* @cmdsyntax  welcome-title <UTF-16>
* @cmdsyntax  no welcome-title
*
* @cmdhelp  Configure Captive Portal Welcome title.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleWelcomeTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 title[CP_WELCOME_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWelcomeTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_WELCOME_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleWelcomeTitleLength, CP_WELCOME_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(title, argv[index+1], CP_WELCOME_TITLE_TEXT_MAX);
    title[CP_WELCOME_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWelcomeTitleNoWelcomeTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(title,0,sizeof(title));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebWelcomeTitleTextSet(cpId,webId,title) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWelcomeTitleSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Welcome text
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

* @cmdsyntax  welcome-text <UTF-16>
* @cmdsyntax  no welcome-text
*
* @cmdhelp  Configure Captive Portal Welcome text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleWelcomeText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_WELCOME_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWelcomeText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_WELCOME_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleWelcomeTextLength, CP_WELCOME_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }   
    memset(text,0,sizeof(text));
    osapiStrncpy(text, argv[index+1], CP_WELCOME_TEXT_MAX);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWelcomeTextNoWelcomeText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
    usmDbCpdmCPConfigWebWelcomeTextSet(cpId,webId,text);
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebWelcomeTextAppend(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWelcomeTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration WIP message
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

* @cmdsyntax  wip-msg <UTF-16>
* @cmdsyntax  no wip-msg
*
* @cmdhelp  Configure Captive Portal WIP message.
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleWIPMsg(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 msg[CP_MSG_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWIPMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_MSG_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleWIPMsgLength, CP_MSG_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, argv[index+1], CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWIPMsgNoWIPMsg);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(msg, WEB_DEF1_WIP_MSG, CP_MSG_TEXT_MAX);
    msg[CP_MSG_TEXT_MAX] = '\0';
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebWipMsgTextSet(cpId,webId,msg) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleWIPMsgSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Script text
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

* @cmdsyntax  script-text <UTF-16>
* @cmdsyntax  no script-text
*
* @cmdhelp  Configure Captive Portal Script text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleScriptText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleScriptText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_SCRIPT_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleScriptTextLength, CP_SCRIPT_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_SCRIPT_TEXT_MAX);
    text[CP_SCRIPT_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleScriptTextNoScriptText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebScriptTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleScriptTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Popup text
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

* @cmdsyntax  popup-text <UTF-16>
* @cmdsyntax  no popup-text
*
* @cmdhelp  Configure Captive Portal Popup text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocalePopupText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocalePopupText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_POPUP_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocalePopupTextLength, CP_POPUP_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_POPUP_TEXT_MAX);
    text[CP_POPUP_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocalePopupTextNoPopupText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebPopupTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocalePopupTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Browser title
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

* @cmdsyntax  logout-browser-title <UTF-16>
* @cmdsyntax  no logout-browser-title
*
* @cmdhelp  Configure Captive Portal Logout Browser title
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutBrowserTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 title[CP_LOGOUT_BROWSER_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutBrowserTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_BROWSER_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutBrowserTitleLength, CP_LOGOUT_BROWSER_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(title, argv[index+1], CP_LOGOUT_BROWSER_TITLE_TEXT_MAX);
    title[CP_LOGOUT_BROWSER_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutBrowserTitleNoLogoutBrowserTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(title,0,sizeof(title));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutBrowserTitleTextSet(cpId,webId,title) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutBrowserTitleSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout title
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

* @cmdsyntax  logout-title <UTF-16>
* @cmdsyntax  no logout-title
*
* @cmdhelp  Configure Captive Portal Logout title text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_LOGOUT_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutTitleText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutTitleTextLength, CP_LOGOUT_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_LOGOUT_TITLE_TEXT_MAX);
    text[CP_LOGOUT_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutTitleTextNoLogoutTitleText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutTitleTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutTitleTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Content text
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

* @cmdsyntax  logout-content-text <UTF-16>
* @cmdsyntax  no logout-content-text
*
* @cmdhelp  Configure Captive Portal Logout Content text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutContentText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_LOGOUT_CONTENT_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutContentText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_CONTENT_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutContentTextLength, CP_LOGOUT_CONTENT_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }   
    memset(text,0,sizeof(text));
    osapiStrncpy(text, argv[index+1], CP_LOGOUT_CONTENT_TEXT_MAX);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutContentTextNoLogoutContentText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
    usmDbCpdmCPConfigWebLogoutContentTextSet(cpId,webId,text);
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutContentTextAppend(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutContentTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Button label
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

* @cmdsyntax  logout-button-label <UTF-16>
* @cmdsyntax  no logout-button-label
*
* @cmdhelp  Configure Captive Portal Logout Button label
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutButtonLabel(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 label[CP_LOGOUT_BUTTON_LABEL_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutButtonLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_BUTTON_LABEL_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutButtonLabelLength, CP_LOGOUT_BUTTON_LABEL_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, argv[index+1], CP_LOGOUT_BUTTON_LABEL_MAX);
    label[CP_LOGOUT_BUTTON_LABEL_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutButtonLabelNoLogoutButtonLabel);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(label, WEB_DEF1_LOGOUT_BUTTON_LABEL, CP_LOGOUT_BUTTON_LABEL_MAX);
    label[CP_LOGOUT_BUTTON_LABEL_MAX] = '\0';  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutButtonLabelSet(cpId,webId,label) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutButtonLabelSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Confirmation
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

* @cmdsyntax  logout-confirmation-text <UTF-16>
* @cmdsyntax  no logout-confirmation-text
*
* @cmdhelp  Configure Captive Portal Logout Confirmation text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutConfirmText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_LOGOUT_CONFIRM_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutConfirmText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_CONFIRM_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutConfirmTextLength, CP_LOGOUT_CONFIRM_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_LOGOUT_CONFIRM_TEXT_MAX);
    text[CP_LOGOUT_CONFIRM_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutConfirmTextNoLogoutConfirmText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, WEB_DEF1_LOGOUT_CONFIRM_TEXT, CP_LOGOUT_CONFIRM_TEXT_MAX);
    text[CP_LOGOUT_CONFIRM_TEXT_MAX] = '\0';  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutConfirmTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutConfirmTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Success Browser title text
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

* @cmdsyntax  logout-success-browser-title <UTF-16>
* @cmdsyntax  no logout-success-browser-title
*
* @cmdhelp  Configure Captive Portal Logout Success Browser Title text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutSuccessBrowserTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 title[CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessBrowserTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutSuccessBrowserTitleLength, CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(title, argv[index+1], CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX);
    title[CP_LOGOUT_SUCCESS_BROWSER_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessBrowserTitleNoLogoutSuccessBrowserTitle);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(title,0,sizeof(title));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextSet(cpId,webId,title) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessBrowserTitleSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Success Title text
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

* @cmdsyntax  logout-success-title <UTF-16>
* @cmdsyntax  no logout-success-title
*
* @cmdhelp  Configure Captive Portal Logout Success Title
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutSuccessTitle(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessTitleText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutSuccessTitleTextLength, CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    osapiStrncpy(text, argv[index+1], CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX);
    text[CP_LOGOUT_SUCCESS_TITLE_TEXT_MAX] = '\0';
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessTitleTextNoLogoutSuccessTitleText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutSuccessTitleTextSet(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessTitleTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/**********************************************************************
* @purpose  Configure Captive Portal configuration Logout Success Content text
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

* @cmdsyntax  logout-success-content-text <UTF-16>
* @cmdsyntax  no logout-success-content-text
*
* @cmdhelp  Configure Captive Portal Logout Success Content text
*
* @cmddescript
*
* @end
*********************************************************************/
const L7_char8 *commandCPLocaleLogoutSuccessContentText(EwsContext ewsContext,L7_uint32 argc,const L7_char8 **argv,L7_uint32 index)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 text[CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX+1];
  cpId_t cpId=0;
  webId_t webId = 0;

  cliExamine(ewsContext, argc, argv, index);
  cliSyntaxTop(ewsContext);

  cpId = EWSCPID(ewsContext);
  webId = EWSWEBID(ewsContext);

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    if (cliNumFunctionArgsGet() != 1)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessContentText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext); 
    }
    if (strlen(argv[index+1]) > CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX)
    {
      osapiSnprintf(stat, sizeof(stat), pStrErr_security_CPLocaleLogoutSuccessContentTextLength, CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX);
      ewsTelnetWrite(ewsContext, stat);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }   
    memset(text,0,sizeof(text));
    osapiStrncpy(text, argv[index+1], CP_LOGOUT_SUCCESS_CONTENT_TEXT_MAX);
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    if (cliNumFunctionArgsGet() != 0)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessContentTextNoLogoutSuccessContentText);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
    memset(text,0,sizeof(text));
    usmDbCpdmCPConfigWebLogoutSuccessContentTextSet(cpId,webId,text);
    ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
    return cliPrompt(ewsContext);
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if (usmDbCpdmCPConfigWebLogoutSuccessContentTextAppend(cpId,webId,text) != L7_SUCCESS)
    {
      ewsTelnetWrite(ewsContext, pStrErr_security_CPLocaleLogoutSuccessContentTextSetError);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


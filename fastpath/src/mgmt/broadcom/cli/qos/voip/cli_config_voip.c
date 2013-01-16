/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/cli_config_voip.c
 *
 * @purpose auto voip config commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  11/23/2007
 *
 * @author  Amitabha sen
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "dot1q_api.h"
#include "l7_cos_api.h"
#include "ews.h"
#include "clicommands_cos.h"
#include "usmdb_qos_cos_api.h"
#include "cli_web_exports.h"
#include "voip_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_util_diffserv_api.h"
#include "clicommands_card.h"
#include "usmdb_qos_voip_api.h"
/*********************************************************************
*
* @purpose  enable/disable auto voip mode globally.
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
* @cmdsyntax  auto-voip all
*
* @cmdhelp Enable/Disable Auto VoIP Mode.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows you to enable or disable Auto VoIP
*   on global mode. The default value of this
*   parameter is disabled.
*
* @end
*
*********************************************************************/
const L7_char8  *commandGlobalAutoVoIP( EwsContext ewsContext, 
                                        L7_uint32 argc, 
                                        const L7_char8 **argv, 
                                        L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 mode = 0;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if(unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  numArg = cliNumFunctionArgsGet();

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIP);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPNo);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(usmDbQosVoIPIntfModeSet(unit, L7_ALL_INTERFACES, mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPModeSet);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }
    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}    

/*********************************************************************
*
* @purpose  Set the auto voip profile minimum bandwidth.
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
* @cmdsyntax  auto-voip
*
* @cmdhelp Set the auto voip profile minimum bandwidth.
*
* @cmddescript
*   This is a configurable value and can be set to certain values. It
*   allows you to configure the Minimum guaranteed Cosq bandwidth to VoIP
*   traffice on an interface. The default value of this
*   parameter is 0 which means Unlimited.
*
* @end
*
*********************************************************************/
const L7_char8  *commandGlobalAutoVoIPMinBandwidth( EwsContext ewsContext, 
                                                    L7_uint32 argc, 
                                                    const L7_char8 **argv, 
                                                    L7_uint32 index)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argValue = 1;
  L7_uint32 value=0;
  L7_uint32 unit;
  L7_uint32 numArgs;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArgs != 1)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPBW);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if(unit == 0)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_ErrCouldNot, ewsContext, pStrErr_common_UnitId_1);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                        &value) != L7_SUCCESS ||
      (value != VOIPBANDWIDTH_0   &&
       value != VOIPBANDWIDTH_64  &&
       value != VOIPBANDWIDTH_128 &&
       value != VOIPBANDWIDTH_256 &&
       value != VOIPBANDWIDTH_512 &&
       value != VOIPBANDWIDTH_1024))
    {
      bzero(buf, sizeof(buf));
      osapiStrncpySafe(buf, pStrErr_qos_AutoVoIPBW, sizeof(buf));
      ewsTelnetWrite( ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    if(usmDbQosVoIPIntfMinBandwidthSet(unit, L7_ALL_INTERFACES,value)!= L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPBWSet);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  enable/disable auto voip mode on an interface.
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
* @cmdsyntax  auto-voip
*
* @cmdhelp Enable/Disable Auto VoIP Mode.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows you to enable or disable Auto VoIP
*   on an interface. The default value of this
*   parameter is disabled.
*
* @end
*
*********************************************************************/
const L7_char8 *commandInterfaceAutoVoIP( EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 **argv,
                                          L7_uint32 index)
{
  
  L7_uint32 mode = 0;
  L7_uint32 s = 0, p = 0;
  L7_uint32 interface = 0;
  L7_uint32 unit = 0;
  L7_uint32 numArg;
  L7_RC_t status = L7_SUCCESS;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_FAILED;
  cliSyntaxTop(ewsContext);
  
  numArg = cliNumFunctionArgsGet();
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIP);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPNo);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  /* If the command is of type 'normal' the 'if' condition is executed
  otherwise 'else-if' condition is excuted */
  if (ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_ENABLE;
  }
  else if (ewsContext->commType == CLI_NO_CMD)
  {
    mode = L7_DISABLE;
  }
  else
  {
    return cliSyntaxReturnPrompt(ewsContext, "");
  }

  /*******Check if the Flag is Set for Execution*************/
  if (ewsContext->scriptActionFlag == L7_EXECUTE_SCRIPT)
  {
    for (interface=1; interface < L7_MAX_INTERFACE_COUNT; interface++)
    {
      if (!L7_INTF_ISMASKBITSET(EWSINTFMASK(ewsContext), interface) ||
          usmDbUnitSlotPortGet(interface, &unit, &s, &p) != L7_SUCCESS)
      {
        continue;
      }
      if (usmDbQosVoIPIntfModeSet(unit, interface, mode) != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPModeSet);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }

  /*************Set Flag for Script Successful******/
  if (status == L7_SUCCESS)
  {
    ewsContext->actionFunctionStatusFlag = L7_ACTION_FUNC_SUCCESS;
  }
  return cliPrompt(ewsContext);
}
#if 0
/*********************************************************************
*
* @purpose  Set the auto voip profile minimum bandwidth on an interface.
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
* @cmdsyntax  auto-voip
*
* @cmdhelp Set the auto voip profile minimum bandwidth.
*
* @cmddescript
*   This is a configurable value and can be set to certain values. It
*   allows you to configure the Minimum guaranteed Cosq bandwidth to VoIP
*   traffice on an interface. The default value of this
*   parameter is 0 which means Unlimited.
*
* @end
*
*********************************************************************/
const L7_char8  *commandInterfaceAutoVoIPMinBandwidth(EwsContext ewsContext, 
                                                      L7_uint32 argc, 
                                                      const L7_char8 **argv, 
                                                      L7_uint32 index)
{
  L7_uint32 s, p, iface;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argValue = 1;
  L7_uint32 value=0;
  L7_uint32 unit;
  L7_uint32 numArgs;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArgs != 1)
    {
      ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPBW);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                        &value) != L7_SUCCESS ||
      (value != VOIPBANDWIDTH_0   &&
       value != VOIPBANDWIDTH_64  &&
       value != VOIPBANDWIDTH_128 &&
       value != VOIPBANDWIDTH_256 &&
       value != VOIPBANDWIDTH_512 &&
       value != VOIPBANDWIDTH_1024))
    {
      bzero(buf, sizeof(buf));
      sprintf( buf, pStrErr_qos_AutoVoIPBW);
      ewsTelnetWrite( ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
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
      if(usmDbQosVoIPIntfMinBandwidthSet(unit, iface,value)!= L7_SUCCESS)
      {
        status = L7_FAILURE;
        ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, cliDisplayInterfaceHelp(unit, s, p));
        ewsTelnetWrite( ewsContext, pStrErr_qos_AutoVoIPBWSet);
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
#endif



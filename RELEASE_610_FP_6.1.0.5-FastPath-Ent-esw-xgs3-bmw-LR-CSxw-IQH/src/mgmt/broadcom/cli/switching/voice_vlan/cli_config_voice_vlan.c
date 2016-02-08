/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename cli_config_voice_vlan.c
*
* @purpose config commands for the Voice Vlan Cli.
*
* @component user interface
*
* @comments none
*
* @create  
*
* @update 
*
* @author  
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "l7_common.h"
#include "cli_web_exports.h"
#include "dot1q_exports.h"
#include "usmdb_util_api.h"
#include "cliapi.h"
#include "datatypes.h"
#include "cli_macro.h"
#include "ews.h"
#include <stdio.h>

#include "clicommands_card.h"


#include "clicommands_voice_vlan.h"
#include "usmdb_voice_vlan_api.h"





/*********************************************************************
*
* @purpose  enable/disable the voice vlan on admin mode.
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
* @cmdsyntax  voice vlan
*
* @cmdhelp Enable/Disable Voice Vlan Mode.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows you to enable or disable the Voice Vlan Feature
*   on global mode. The default value of this
*   parameter is disabled.
*
* @end
*
*********************************************************************/
const L7_char8  *commandVoiceVlan(EwsContext ewsContext, 
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
      ewsTelnetWrite( ewsContext, CLISYNTAX_VOICEVLANMODE);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_VOICEVLANMODE_NO);
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
    if(usmdbVoiceVlanAdminModeSet(unit, mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLIVOICEVLANADMINMODE_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}    

/*********************************************************************
*
* @purpose  Set the Voice Vlan Id for the specified port
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
* @cmdsyntax  [no] voice vlan <vlan-id>
*
* @cmdhelp Set the voice vlan id on a per-port basis.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandVoiceVlanId(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 **argv,
                                         L7_uint32 index)
{
  L7_uint32 slot, port, iface;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 argValue = 1;
  L7_uint32 value=0;
  L7_uint32 unit;
  L7_uint32 numArgs,rc;

  cliSyntaxTop(ewsContext);

  numArgs = cliNumFunctionArgsGet();

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArgs != 1)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGPORTVOICEVLAN_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArgs != 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGVOICEVLANNO_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if(cliIsStackingSupported() == L7_TRUE)
    unit = EWSUNIT(ewsContext);
  else
    unit = cliGetUnitId();

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if(usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }


  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                        &value) != L7_SUCCESS ||
       value < L7_DOT1Q_MIN_VLAN_ID||
       value > L7_DOT1Q_MAX_VLAN_ID)
    {
      bzero(buf, sizeof(buf));
      sprintf( buf, CLISYNTAX_CONFIGPORTVOICEVLAN,
               L7_DOT1Q_MIN_VLAN_ID, L7_DOT1Q_MAX_VLAN_ID);
      ewsTelnetWrite( ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    value = 0;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
  {
    if(value !=0)
    {
      if((rc=usmdbVoiceVlanPortVlanIdSet(unit, iface,value))!= L7_SUCCESS)
      {
        if(rc == L7_FAILURE)
          ewsTelnetWrite( ewsContext, CLIPORTVOICEVLAN_FAILURE);
        else
          ewsTelnetWrite( ewsContext, CLIPORTVOICEVLAN_ERR);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
    else
    {
      if((rc=usmdbvoiceVlanPortAdminModeSet(unit,iface,VOICE_VLAN_CONFIG_DISABLE,0))!=L7_SUCCESS)
      {
        if(rc == L7_FAILURE)
          ewsTelnetWrite( ewsContext, CLIPORTVOICEVLAN_FAILURE);
        else
          ewsTelnetWrite( ewsContext, CLIPORTVOICEVLAN_ERR);
        cliSyntaxBottom(ewsContext);
        return cliPrompt(ewsContext);
      }
    }
  }
    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}

/*********************************************************************
*
* @purpose  Set 802.1p priority tagging for voice traffic on the specified port 
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
* @cmdsyntax  [no] voice vlan dot1p <priority>
*
* @cmdhelp Set 802.1p priority for the voice traffic.
*
* @cmddescript
*
* @end
*
*********************************************************************/
const L7_char8  *commandVoiceVlanPriority(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 **argv,
                                         L7_uint32 index)
{
  L7_uint32 slot, port, iface;
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
      ewsTelnetWrite( ewsContext, CLISYNTAX_CONFIGPORTVOICEVLANPRIORITY_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  if(cliIsStackingSupported() == L7_TRUE)
    unit = EWSUNIT(ewsContext);
  else
    unit = cliGetUnitId();

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if(usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }


  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(cliConvertTo32BitUnsignedInteger(argv[index+argValue],
                                        &value) != L7_SUCCESS ||
       value < L7_DOT1P_MIN_PRIORITY||
       value > L7_DOT1P_MAX_PRIORITY)
    {
      bzero(buf, sizeof(buf));
      sprintf( buf, CLISYNTAX_CONFIGPORTVOICEVLAN,
               L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY);
      ewsTelnetWrite( ewsContext, buf);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    if(usmdbVoiceVlanPortVlanPrioritySet(unit, iface,value)!= L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLIPORTVOICEVLANPRIORITY_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;
  return cliPrompt(ewsContext);
}


/*********************************************************************
*
* @purpose  Enable/Disable default configuration for Voice Vlan
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
* @cmdsyntax  voice vlan
*
* @cmdhelp Enable/Disable Voice Vlan none.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows the IP phone to use its own configuration to send untagged 
*   voice traffic.The default value of this
*   parameter is disabled.
*
* @end
*
*********************************************************************/
const L7_char8  *commandVoiceVlanNone(EwsContext ewsContext, 
                                      L7_uint32 argc, 
                                      const L7_char8 **argv, 
                                      L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 slot, port, iface;
  L7_BOOL mode = L7_FALSE;

  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_VOICEVLANNONE_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  if(cliIsStackingSupported() == L7_TRUE)
    unit = EWSUNIT(ewsContext);
  else
    unit = cliGetUnitId();

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if(usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }


  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  } 

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    if(usmdbVoiceVlanPortNoneSet(unit,iface,mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLIVOICEVLANNONE_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}   

/*********************************************************************
*
* @purpose  Enable/Disable tagging for voice traffic
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
* @cmdsyntax  voice vlan
*
* @cmdhelp Enable/Disable Voice Vlan tagging.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows you to configure the phone to send untagged voice traffic.
*   The default value of this parameter is disabled.
*
* @end
*
*********************************************************************/
const L7_char8  *commandVoiceVlanUntagged(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 slot, port, iface;
  L7_BOOL mode = 0;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_VOICEVLANUNTAG_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } 

  if(cliIsStackingSupported() == L7_TRUE)
    unit = EWSUNIT(ewsContext);
  else
    unit = cliGetUnitId();

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if(usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }


  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    mode = L7_TRUE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    if(usmdbVoiceVlanPortUntagSet(unit,iface, mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLIVOICEVLANUNTAG_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}    


/*********************************************************************
*
* @purpose  Trust or untrust the data traffic arriving on the voice vlan port.
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
* @cmdsyntax  voice vlan data priority
*
* @cmdhelp Enable/Disable Default Cos Override for voice traffic.
*
* @cmddescript
*   This is a configurable value and can be Enabled or Disabled. It
*   allows you to trust or untrust the data traffic arriving on 
*   the voice vlan port.The default value of this parameter is disabled.
*
* @end
*
*********************************************************************/

const L7_char8  *commandVoiceVlanDataPriority(EwsContext ewsContext, 
                                              L7_uint32 argc, 
                                              const L7_char8 **argv, 
                                              L7_uint32 index)
{
  L7_uint32 unit;
  L7_uint32 numArg;
  L7_uint32 slot, port, iface;
  L7_uint32 mode = 0;


  /*************Set Flag for Script Failed******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_FAILED;

  cliSyntaxTop(ewsContext);

  numArg = cliNumFunctionArgsGet();
  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(numArg != 1)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_VOICEVLANCOS_OVERRIDE_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  } else if(ewsContext->commType == CLI_NO_CMD)
  {
    if(numArg != 0)
    {
      ewsTelnetWrite( ewsContext, CLISYNTAX_VOICEVLANCOS_OVERRIDENO_SYNTAX);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }
  }

  if(cliIsStackingSupported() == L7_TRUE)
    unit = EWSUNIT(ewsContext);
  else
    unit = cliGetUnitId();

  slot = EWSSLOT(ewsContext);
  port = EWSPORT(ewsContext);

  if(usmDbIntIfNumFromUSPGet(unit, slot, port, &iface) != L7_SUCCESS)
  {
    ewsTelnetWriteAddBlanks (1, 0, 0, 1, pStrErr_common_Error, ewsContext, pStrErr_common_InvalidIntf);
    cliSyntaxBottom(ewsContext);
    return cliPrompt(ewsContext);
  }


  if(ewsContext->commType == CLI_NORMAL_CMD)
  {
    if(strcmp(argv[index+1],"trust")==0)
      mode = L7_FALSE;
    else if(strcmp(argv[index+1],"untrust")==0)
      mode = L7_TRUE;
  }

  /*******Check if the Flag is Set for Execution*************/
  if(ewsContext->scriptActionFlag==L7_EXECUTE_SCRIPT)
    if(usmdbVoiceVlanPortCosOverrideSet(unit,iface, mode) != L7_SUCCESS)
    {
      ewsTelnetWrite( ewsContext, CLIVOICEVLANCOSOVERRIDE_ERR);
      cliSyntaxBottom(ewsContext);
      return cliPrompt(ewsContext);
    }

    /*************Set Flag for Script Successful******/
  ewsContext->actionFunctionStatusFlag=L7_ACTION_FUNC_SUCCESS;

  return cliPrompt(ewsContext);
}    



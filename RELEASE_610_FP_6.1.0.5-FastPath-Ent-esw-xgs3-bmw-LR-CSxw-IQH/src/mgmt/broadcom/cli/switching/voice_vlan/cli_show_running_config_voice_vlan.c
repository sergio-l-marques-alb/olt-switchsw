/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_running_config_voice_vlan.c
 *
 * @purpose show running config commands for voice vlan
 *
 * @component user interface
 *
 * @comments
 *
 * @create  03/05/2007
 *
 * @author  nshrivastav
 * @end
 *
 **********************************************************************/
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_switching_common.h"
#include "strlib_switching_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "config_script_api.h"
#include "usmdb_voice_vlan_api.h"
#include "osapi_support.h"
#include "cli_show_running_config.h"

#ifndef _L7_OS_LINUX_
#include <inetLib.h>        /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "datatypes.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "util_enumstr.h"

/*********************************************************************
* @purpose  To print the running configuration of Voice Vlan info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 unit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigVoiceVlanInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 mode;

  /* Administrative Mode */
  if (usmdbVoiceVlanAdminModeGet(unit,&mode) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,mode,FD_VOICE_VLAN_ADMIN_MODE,pStrInfo_switching_VoiceVlanAdminMode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of Voice Vlan info
*
* @param    EwsContext ewsContext
* @param    L7_uint32 interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
   @end
*********************************************************************/
L7_RC_t cliRunningConfigInterfaceVoiceVlanInfo(EwsContext ewsContext, L7_uint32 interface)
{
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 vlanId;
  L7_BOOL   val1;
  L7_uint32 val2;
  L7_uint32 unit =cliGetUnitId();

  VOICE_VLAN_CONFIG_TYPE_t  mode;

  if (usmdbVoiceVlanPortAdminModeGet(unit,interface,&mode) == L7_SUCCESS)
  {
    switch(mode)
    {
    case VOICE_VLAN_CONFIG_VLAN_ID:
    
      if (usmdbVoiceVlanPortVlanIdGet(unit,interface,&vlanId) == L7_SUCCESS)
      {
        if (vlanId != FD_VOICE_VLAN_VLAN_ID || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),pStrInfo_switching_VoiceVlanPortvlanId, vlanId);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    break;
    case VOICE_VLAN_CONFIG_DOT1P:

      if(usmdbVoiceVlanPortVlanPriorityGet(unit,interface,&val2) == L7_SUCCESS)
      {
        if (val2 != FD_VOICE_VLAN_PRIORITY || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),pStrInfo_switching_VoiceVlanPortPriority, val2);
          EWSWRITEBUFFER(ewsContext, buf);
        }
      }
    break;

    case VOICE_VLAN_CONFIG_NONE:
      if(usmdbVoiceVlanPortNoneGet(unit,interface,&val1) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext, val1,FD_VOICE_VLAN_NONE_MODE,pStrInfo_switching_VoiceVlanPortNone);
      }
   break;
    case VOICE_VLAN_CONFIG_UNTAGGED:
      if(usmdbVoiceVlanPortUntagGet(unit,interface,&val1) == L7_SUCCESS)
      {
        cliShowCmdEnable(ewsContext, val1,FD_VOICE_VLAN_TAGGING,pStrInfo_switching_VoiceVlanPortTagging);
      }
    break;  
    default:
    if(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),"%s %s ", pStrInfo_common_No_1,pStrInfo_switching_VoiceVlanAdminMode);
      EWSWRITEBUFFER(ewsContext, buf);
    }
    break;
    }
  }
  if(usmdbVoiceVlanPortCosOverrideGet(unit, interface, &val1) == L7_SUCCESS)
  {
    if (val1 != FD_VOICE_VLAN_OVERRIDE)
    {
       osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),pStrInfo_switching_VoiceVlanPortCosOverrideUntrust);
       EWSWRITEBUFFER(ewsContext, buf);
    }
    else if(EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),pStrInfo_switching_VoiceVlanPortCosOverrideTrust);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }
  return L7_SUCCESS;
}

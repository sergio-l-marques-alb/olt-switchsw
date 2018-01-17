/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/qos/cos/cli_show_running_config_cos.c
 *
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  04/16/2004
 *
 * @author  kmans
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_qos_common.h"
#include "strlib_qos_cli.h"
#include "l7_common.h"
#include "cos_exports.h"
#include "cli_web_exports.h"
#include "usmdb_util_api.h"
/*#include "comm_mask.h"*/
#ifdef L7_QOS_FLEX_PACKAGE_VOIP
#include "config_script_api.h"

#include "datatypes.h"
/*#include "usmdb_counters_api.h" 
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"*/
#include "default_cnfgr.h"
#include "defaultconfig.h"
/*#include "cli_web_user_mgmt.h"
#include "l7_cos_api.h"*/
#include "cli_show_running_config.h"
#include "usmdb_qos_voip_api.h"

/*********************************************************************
* @purpose  To print the running configuration of Auto VoIP info
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
L7_RC_t cliRunningConfigAutoVoIPInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 minBandwidth, mode;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* Administrative Mode */
  if (usmDbQosVoIPIntfModeGet(unit,L7_ALL_INTERFACES, &mode) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,mode,FD_VOIP_INTF_PROFILE_MODE,pStrInfo_qos_AutoVoIP_1);
  }

  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_COS_COMPONENT_ID, L7_COS_QUEUE_BUCKET_FEATURE_ID) == L7_TRUE)
  {
    /* Min bandwidth */
    if (usmDbQosVoIPIntfMinBandwidthGet(unit,L7_ALL_INTERFACES, &minBandwidth) == L7_SUCCESS )
    {
      if (minBandwidth != FD_VOIP_INTF_PROFILE_BANDWIDTH || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf),pStrInfo_qos_AutoVoIP_MinBw, minBandwidth);
        EWSWRITEBUFFER(ewsContext, buf);
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the running configuration of Auto VoIP 
*           Interface info
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
L7_RC_t cliRunningConfigInterfaceAutoVoIPInfo(EwsContext ewsContext, L7_uint32 interface)
{
  L7_uint32 mode;
  L7_uint32 unit =cliGetUnitId();

  /* Administrative Mode */
  if (usmDbQosVoIPIntfModeGet(unit,interface, &mode) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,mode,FD_VOIP_INTF_PROFILE_MODE,pStrInfo_qos_AutoVoIP_1);
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }
}



#endif /* End if VoIP package include */

/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/routing/vrrp/cli_show_running_config_vrrp_l3.c
 *
 * @purpose show running config commands for VRRP
 *
 * @component user interface
 *
 * @comments
 *
 * @create  18/08/2003
 *
 * @author  Samip
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_routing_common.h"
#include "strlib_routing_cli.h"
#include "l7_common.h"
#include "usmdb_mib_vrrp_api.h"
#include "comm_mask.h"
#include "config_script_api.h"

#include "datatypes.h"
#include "usmdb_counters_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "default_cnfgr.h"
#include "defaultconfig.h"
#include "cli_web_user_mgmt.h"

#include "usmdb_rtrdisc_api.h"
#include "l3_mcast_defaultconfig.h"
#include "l3_defaultconfig.h"
#include "clicommands_loopback.h"
#include "usmdb_ip6_api.h"

#include "cli_show_running_config.h"
#include "util_enumstr.h"

L7_RC_t cliRunningVrrpModeInfo(EwsContext ewsContext, L7_uint32 unit)
{
  return L7_SUCCESS;
}
/*********************************************************************
 * @purpose  To print the running configuration of VRRP
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

L7_RC_t cliRunningConfigVrrpInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbVrrpOperAdminStateGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_VRRP_DEFAULT_ADMIN_MODE,pStrInfo_routing_IpVrrp);
  }

  return L7_SUCCESS;
}


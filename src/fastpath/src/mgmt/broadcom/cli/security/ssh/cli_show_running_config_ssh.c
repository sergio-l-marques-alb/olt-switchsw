/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/ssh/cli_show_running_config_ssh.c
 *
 * @purpose show running config commands for the ssh cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  10/22/2003
 *
 * @author  dcbii
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
#include "usmdb_sshd_api.h"
#include "comm_mask.h"
#include "cli_show_running_config.h"
#include "config_script_api.h"

#include "datatypes.h"

#include "clicommands_ssh.h"
#include "sshd_exports.h"

/*********************************************************************
* @purpose  To print the running configuration of ssh Info
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

L7_RC_t cliRunningConfigSshInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbsshdAdminModeGet(unit, &val ) == L7_SUCCESS )
  {
    cliShowCmdEnable(ewsContext,val,FD_SSHD_ADMIN_MODE,pStrInfo_security_IpSshSrvrEnbl);
  }

  if ( usmDbsshdProtoLevelGet(unit, &val) == L7_SUCCESS)
  {
    if((val != FD_SSHD_PROTO_LEVEL)|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      memset ( buf, 0, sizeof(buf) );
      if (val == L7_SSHD_PROTO_LEVEL_V1_ONLY)
      {
        strcpyAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_RateMbps);
      }
      else if (val == L7_SSHD_PROTO_LEVEL_V2_ONLY)
      {
        strcpyAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_common_Rate2Mbps_1);
      }

      if ((val == L7_SSHD_PROTO_LEVEL_V1_ONLY) ||
          (val == L7_SSHD_PROTO_LEVEL_V2_ONLY))
      {
        sprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, stat, pStrInfo_security_ShowRunningIpSshProto, buf);
        EWSWRITEBUFFER(ewsContext, stat);
      }
    }
  }

  if( usmDbSshdMaxNumSessionsGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_SSHD_MAX_SESSIONS,pStrInfo_security_SshconMaxsessions);
  }

  if( usmDbsshdTimeoutGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,FD_SSHD_TIMEOUT,pStrInfo_security_SshconTimeout);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_isdp.c
 *
 * @purpose show running config commands for isdp
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  14/01/2008
 *
 * @author  dgaryachy
 *
 * @end
 *
 **********************************************************************/

#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_isdp_api.h"
#include "defaultconfig.h"
#include "cliapi.h"
#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"


/*********************************************************************
 * @purpose  To print the running configuration of isdp
 *
 * @param    EwsContext ewsContext@b((input))
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t cliRunningConfigIsdpInfo(EwsContext ewsContext)
{
  L7_uint32 val;
  L7_RC_t rc;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];

  /* isdp mode */
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpModeGet(&val);
  if (val != FD_ISDP_DEFAULT_MODE)
  {
    if(val != L7_ENABLE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_NoIsdpRun);
      EWSWRITEBUFFER(ewsContext, buf);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_CfgIsdpRun);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /* isdp v2 mode */
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpV2ModeGet(&val);
  if (val != FD_ISDP_DEFAULT_V2MODE)
  {
    if(val != L7_ENABLE)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_NoIsdpAdv);
      EWSWRITEBUFFER(ewsContext, buf);
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_IsdpAdv);
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  /*holdtime interval*/
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpHoldTimeGet(&val);
  if (val != FD_ISDP_DEFAULT_HOLDTIME)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_IsdpHoldtimeCmd, val);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  /*message interval*/
  memset (buf, 0, sizeof(buf));
  rc = usmdbIsdpTimerGet(&val);
  if (val != FD_ISDP_DEFAULT_TIMER)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_IsdpTimerCmd, val);
    EWSWRITEBUFFER(ewsContext, buf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of isdp in Interface mode
 *
 * @param    EwsContext ewsContext    @b((input))
 * @param    L7_uint32    interface     @b((input))   interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningInterfaceConfigIsdpInfo(EwsContext ewsContext, L7_uint32 interface)
{
  L7_uint32 mode;
  L7_RC_t rc;

  if (usmdbIsdpIsValidIntf(interface) != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  rc = usmdbIsdpIntfModeGet(interface, &mode);
  
  if (mode != L7_ENABLE) 
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoInterfaceEnable);
  }

  return L7_SUCCESS;
}

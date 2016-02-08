/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/switching/cli_show_running_config_pml.c
 *
 * @purpose show running config commands for pml
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
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "cliapi.h"
#include "config_script_api.h"
#include "usmdb_pml_api.h"
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
* @purpose  To print the global running configuration for port mac locking
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
L7_RC_t cliRunningConfigPmlGlobalInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if (usmDbPmlAdminModeGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_PML_ADMIN_MODE,pStrInfo_common_PortSecurity_1);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To print the interface running configuration for port mac locking
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

L7_RC_t cliRunningConfigPmlInterfaceInfo(EwsContext ewsContext, L7_uint32 unit,
                                         L7_uint32 interface)
{
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_RC_t rc;
  L7_uint32 val;
  L7_ushort16 vlanId;
  L7_uchar8 mac[L7_CLI_MAX_STRING_LENGTH];

  if (usmDbPmlIntfModeGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_PML_ADMIN_MODE,pStrInfo_common_PortSecurity_1);
  }

  if (usmDbPmlIntfDynamicLimitGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,L7_MACLOCKING_MAX_DYNAMIC_ADDRESSES,pStrInfo_switching_PortSecurityMaxDyn);
  }

  if (usmDbPmlIntfStaticLimitGet(unit, interface, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,val,L7_MACLOCKING_MAX_STATIC_ADDRESSES,pStrInfo_switching_PortSecurityMaxStatic);
  }

  if (usmDbComponentPresentCheck(unit, L7_SNMP_COMPONENT_ID) == L7_TRUE)
  {
    if (usmDbPmlIntfViolationTrapModeGet(unit, interface, &val) == L7_SUCCESS)
    {
      cliShowCmdEnable(ewsContext,val,FD_PML_INTF_VIOLATION_TRAP_MODE,pStrInfo_switching_SnmpSrvrEnblTrapsViolation);
    }
  }

  rc = usmDbPmlIntfStaticEntryGetFirst(unit, interface, (L7_enetMacAddr_t *)mac, &vlanId);
  while(rc == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), " %02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_switching_PortSecurityMacAddr, buf, vlanId);
    EWSWRITEBUFFER(ewsContext, stat);
    rc = usmDbPmlIntfStaticEntryGetNext(unit, interface, (L7_enetMacAddr_t *)mac, &vlanId);
  }
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/ssl/cli_show_running_config_ssl.c
 *
 * @purpose show running config commands for the ssl cli
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
#include "usmdb_sslt_api.h"
#include "comm_mask.h"
#include "cli_show_running_config.h"
#include "config_script_api.h"

#include "datatypes.h"

#include "clicommands_ssl.h"
#include "sslt_exports.h"

/*********************************************************************
* @purpose  To print the running configuration of ssl Info
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

L7_RC_t cliRunningConfigHttpSecureServerInfo(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 val;

  if ( usmDbssltAdminModeGet(unit, &val ) == L7_SUCCESS)
  {
    cliShowCmdEnable(ewsContext,val,FD_SSLT_ADMIN_MODE,pStrInfo_security_IpHttpSecureSrvr);
  }

  if (( usmDbssltProtocolLevelGet(unit, L7_SSLT_PROTOCOL_TLS10, &val) == L7_SUCCESS) &&
      ((val != FD_SSLT_TLS10_MODE)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
  {
    if (val == FD_SSLT_TLS10_MODE)
    {
      if ((usmDbssltProtocolLevelGet(unit, L7_SSLT_PROTOCOL_SSL30, &val) == L7_SUCCESS) &&
          ((val == FD_SSLT_SSL30_MODE)&& (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
      {
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_security_IpHttpSecureProtoTls1Ssl3);
      }
    }
    else
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_security_IpHttpSecureProtoSsl3);
    }
  }

  if ((usmDbssltProtocolLevelGet(unit, L7_SSLT_PROTOCOL_SSL30, &val) == L7_SUCCESS) &&
      ((val != FD_SSLT_SSL30_MODE)))
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, ewsContext,pStrInfo_security_IpHttpSecureProtoTls1);
  }

  if ( usmDbssltSecurePortGet(unit, &val) == L7_SUCCESS)
  {
    cliShowCmdInt(ewsContext,((L7_int32)val),((L7_int32)FD_SSLT_SECURE_PORT),pStrInfo_security_IpHttpSecurePort_1);
  }

  /* ----------------------------------------------------------------------- */
  /* HTTP Secure Session Config                                              */
  /* ----------------------------------------------------------------------- */

  if ((usmDbssltNumSessionsGet(&val) == L7_SUCCESS))
  {
    cliShowCmdInt(ewsContext,val,FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS,pStrInfo_security_IpHttpSecureSessionMaxsessions);
  }

  if ((usmDbssltSessionHardTimeOutGet(&val) == L7_SUCCESS))
  {
    cliShowCmdInt(ewsContext,val,FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_DEFAULT,pStrInfo_security_IpHttpSecureSessionHardTimeout);
  }
  if ((usmDbssltSessionSoftTimeOutGet(&val) == L7_SUCCESS))
  {
    cliShowCmdInt(ewsContext,val,FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT,pStrInfo_security_IpHttpSecureSessionSoftTimeout);
  }

  return L7_SUCCESS;
}

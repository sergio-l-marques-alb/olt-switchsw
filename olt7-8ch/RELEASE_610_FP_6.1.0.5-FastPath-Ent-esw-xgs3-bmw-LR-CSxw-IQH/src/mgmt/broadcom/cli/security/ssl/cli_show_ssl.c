/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
 * @filename src/mgmt/cli/security/ssl/cli_show_ssl.c
*
* @purpose show commands for the cli
*
* @component user interface
*
* @comments
*
* @create  09/16/2003
*
* @author  Kim Mans
* @end
*
**********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "cliapi.h"
#include "datatypes.h"

#include "clicommands_ssl.h"
#include "default_cnfgr.h"
#include "usmdb_sslt_api.h"
#include "cli_web_exports.h"
#include "cli.h"
#include "ews.h"
#include "sslt_exports.h"

/*********************************************************************
*
* @purpose Displays secure http configuration   
*
* @param EwsContext ewsContext
*
* @returntype const char  *
* @returns nothing
*
* @notes  called by commandShowIpHttp() 
*
* @end
*
*********************************************************************/
void cliShowIpHttpSecure(EwsContext ewsContext)
{

  L7_uint32 unit;
  L7_uint32 ssltAdminMode = L7_DISABLE;
  L7_uint32 ssltProtoMode = L7_DISABLE;
  L7_uint32 ssltSecurePort;
  L7_uchar8 strSsltAdminMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strSsltProtoMode[L7_CLI_MAX_STRING_LENGTH];
  L7_uchar8 strSsltSecurePort[L7_CLI_MAX_STRING_LENGTH];
  L7_char8  buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;

  unit = cliGetUnitId();
  cliSyntaxTop(ewsContext);

  cliFormat(ewsContext, pStrInfo_security_HttpModeSecure);
  if (usmDbssltAdminModeGet(unit, &ssltAdminMode) == L7_SUCCESS)
  {
    sprintf(strSsltAdminMode,"%-8s",strUtilEnabledDisabledGet(ssltAdminMode ,"-----"));
  }
  else
  {
    sprintf(strSsltAdminMode,"%-8s",pStrInfo_common_Line);
  }

  ewsTelnetWrite(ewsContext,strSsltAdminMode);

  cliFormat(ewsContext, pStrInfo_security_SecurePort);
  if (usmDbssltSecurePortGet(unit, &ssltSecurePort) == L7_SUCCESS && ssltSecurePort > 0)
  {
    sprintf(strSsltSecurePort,"%-5d",ssltSecurePort);
  }
  else
  {
    sprintf(strSsltSecurePort,"%-5s",pStrInfo_common_Line);
  }
  ewsTelnetWrite(ewsContext,strSsltSecurePort);

  cliFormat(ewsContext, pStrInfo_security_SecureProtoLvlS);
  if (usmDbssltProtocolLevelGet(unit, L7_SSLT_PROTOCOL_TLS10, &ssltProtoMode) == L7_SUCCESS)
  {
    if (ssltProtoMode == L7_ENABLE)
    {
      sprintf (strSsltProtoMode,"%-5s",pStrInfo_security_Tls1);
    }
    else
    {
      sprintf(strSsltProtoMode,"%-5s","-----");
    }

    ewsTelnetWrite(ewsContext,strSsltProtoMode);
  }
  else
  {
    sprintf(strSsltProtoMode,"%-5s","-----");
  }

  if (usmDbssltProtocolLevelGet(unit, L7_SSLT_PROTOCOL_SSL30, &ssltProtoMode) == L7_SUCCESS)
  {
    if (ssltProtoMode == L7_ENABLE)
    {
      sprintf(strSsltProtoMode,"%-5s",pStrInfo_security_Ssl3);
    }
    else
    {
      sprintf(strSsltProtoMode,"%-5s","-----");
    }
    ewsTelnetWrite(ewsContext,strSsltProtoMode);
  }
  else
  {
    sprintf(strSsltProtoMode,"%-5s","-----");
  }

  cliFormat(ewsContext, pStrInfo_security_MaxAllowableHttpsSessions);
  if (usmDbssltNumSessionsGet(&val) == L7_SUCCESS)
  {
    sprintf(buf, "%-5d", val);
  }
  else
  {
    strcpy(buf, "-----");
  }
  ewsTelnetWrite(ewsContext, buf);

  cliFormat(ewsContext, pStrInfo_security_HttpsSessionHardTimeout);
  if (usmDbssltSessionHardTimeOutGet(&val) == L7_SUCCESS)
  {
    if (val > 0)
    {
      sprintf(buf, pStrInfo_common_Hours, val);
    }
    else
    {
      strcpy(buf, pStrInfo_common_0Infinite);
    }
  }
  else
  {
    strcpy(buf, "-----");
  }
  ewsTelnetWrite(ewsContext, buf);


  cliFormat(ewsContext, pStrInfo_security_HttpsSessionSoftTimeout);
  if (usmDbssltSessionSoftTimeOutGet(&val) == L7_SUCCESS)
  {
    if (val > 0)
    {
      sprintf(buf, pStrInfo_common_Minutes, val);
    }
    else
    {
      strcpy(buf, pStrInfo_common_0Infinite);
    }
  }
  else
  {
    strcpy(buf, "-----");
  }
  ewsTelnetWrite(ewsContext, buf);


  cliFormat(ewsContext, pStrInfo_certificate_Present);
  if (usmDbssltCertificateExists(1) == L7_SUCCESS)
  {
    osapiSnprintf(buf, sizeof(buf), "%-15s", "True");
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-15s", "False");
  }
  ewsTelnetWrite(ewsContext,buf);


  cliFormat(ewsContext, pStrInfo_certification_Inprogress);
  if (usmDbssltCertificateExists(1) == L7_FAILURE)
  {
    osapiSnprintf(buf, sizeof(buf), "%-15s", "True");
  }
  else
  {
    osapiSnprintf(buf, sizeof(buf), "%-15s", "False");
  }
  ewsTelnetWrite(ewsContext,buf);
}

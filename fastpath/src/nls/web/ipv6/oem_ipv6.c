/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename src/nls/web/ipv6/oem_ipv6.c
*
* @purpose    OEM String Helper Functions
*
* @component  WEB
*
* @comments   none
*
* @create     01/10/2007
*
* @author     Rama Sasthri, Kristipati
*
* @end
*
*********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_ipv6_common.h"
#include "strlib_ipv6_web.h"
#include "datatypes.h"

extern L7_char8 *usmWebPageHeader1stGet(L7_char8 *);
extern L7_char8 *usmWebPageHeader2ndGet(L7_char8 *);
L7_char8 *usmWebIpV6GetPageHdr1(L7_int32 token);
static L7_BOOL local_call=L7_FALSE;

L7_char8 *usmWebIpV6GetNLS(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1097:
    errMsg = pStrInfo_common_Ping;
    break;
  case 1639:
    errMsg = pStrInfo_common_Intf;
    break;
  case 22222:
    errMsg = pStrInfo_common_Ipv6Addr2;
    break;
  case 23111:
    errMsg = pStrInfo_common_TacacsShowGlobal;
    break;
  case 23112:
    errMsg = pStrInfo_ipv6_LinkLocal;
    break;
  case 23113:
    errMsg = pStrInfo_ipv6_DatagramSize_1;
    break;
  case 23114:
    errMsg = pStrInfo_ipv6_PingOutput;
    break;
  case 23115:
    errMsg = pStrInfo_ipv6_LinkLocalAddr_1;
    break;
  case 22269:
    errMsg = pStrInfo_common_HostIpv6Addr2;
    break;

  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebIpV6GetOEM(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  default:
    local_call=L7_TRUE;
    errMsg = usmWebIpV6GetPageHdr1(token);
    local_call=L7_FALSE;
    break;
  }
  return errMsg;
}

L7_char8 *usmWebIpV6GetPageHdr1(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 1071:
    errMsg = pStrInfo_ipv6_PingIpv6;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  if( local_call == L7_TRUE )
     return errMsg;
  else
     return usmWebPageHeader1stGet(errMsg);
}

L7_char8 *usmWebIpV6GetPageHdr2(L7_int32 token)
{
  L7_char8 * errMsg;

  switch(token)
  {
  case 2071:
    errMsg = pStrInfo_ipv6_HtmlLinkBaseSysHelpSwitchCfgPingipv6;
    break;
  default:
    errMsg = pStrInfo_common_NlsError;
    break;
  }
  return usmWebPageHeader2ndGet(errMsg);
}

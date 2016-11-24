/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_client_intf_status.c
*
* @purpose    Code in support of the cp_client_intf_status.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     7/9/2007
*
* @author     rjindal
*
* @end
*
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_buffer.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpdm_api.h"
#include "captive_portal_commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "util_pstring.h"


/*********************************************************************
* @purpose  Get the client entry as HTML data
*
* @param    macAddr  @b{(input)} Client mac address
* @param    idx      @b{(input)} Entry index
*
* @returns  Display buffer
*
* @end
*********************************************************************/
L7_char8 *usmWebCPClientIntfEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 idx)
{
  cpdmAuthWhichSwitch_t swType;
  L7_IP_ADDR_t ipAddr;
  L7_char8 ipBuf[USMWEB_IPADDR_SIZE];
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  L7_char8 name[USMWEB_APP_DATA_SIZE+1];
  L7_char8 tmpBuf[USMWEB_APP_DATA_SIZE+1];
  cpId_t val;
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;

  memset (webStaticContentBuffer, 0x00, sizeof(webStaticContentBuffer));

  /* Peer Authenticated */
  memset(buf, 0, sizeof(buf));
  usmDbCpdmClientConnStatusWhichSwitchGet(&macAddr, &swType);
  if (CP_AUTH_ON_LOCAL_SWITCH == swType)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                            pStrInfo_security_JsdataPeerClient, idx,
                            pStrInfo_common_Space);
  }
  else
  {
    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf),
                            pStrInfo_security_JsdataPeerClient, idx,
                            pStrInfo_common_Asterix);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer) - strlen(webStaticContentBuffer) - 1));

  /* client mac address */
  memset(buf, 0, sizeof(buf));
  osapiSnprintfAddBlanks(1,0,0,1, L7_NULLPTR, buf, sizeof(buf), 
                         pStrInfo_security_JsdataClientMAC2, 
                         macAddr.addr[0], macAddr.addr[1], macAddr.addr[2], 
                         macAddr.addr[3], macAddr.addr[4], macAddr.addr[5]);
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer) - strlen(webStaticContentBuffer) - 1));

  /* client ip address */
  memset(&ipAddr, 0, sizeof(L7_IP_ADDR_t));
  memset(ipBuf, 0, sizeof(ipBuf));
  if ((usmDbCpdmClientConnStatusIpGet(&macAddr, &ipAddr) == L7_SUCCESS) && 
      (usmDbInetNtoa(ipAddr, ipBuf) == L7_SUCCESS))
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_ClientIP, ipBuf);
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_ClientIPError);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  /* cpid */
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmClientConnStatusCpIdGet(&macAddr, &val) == L7_SUCCESS)
  {
    memset(name, 0, sizeof(name));
    if (usmDbCpdmCPConfigNameGet(val, name) == L7_SUCCESS)
    {
      osapiSnprintf(tmpBuf, sizeof(tmpBuf), "%u-%s", val, name);
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_ClientCpid, tmpBuf);
    }
    else
    {
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_ClientCpidError);
    }
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_ClientCpidError);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  /* protocol */
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmClientConnStatusProtocolModeGet(&macAddr, &protocol) == L7_SUCCESS)
  {
    if (protocol == L7_LOGIN_TYPE_HTTP)
    {
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_Protocol_1, pStrInfo_security_ProtocolHTTP);
    }
    else
    {
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_Protocol_1, pStrInfo_security_ProtocolHTTPS);
    }
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_ProtocolError);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  /* verification */
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmClientConnStatusVerifyModeGet(&macAddr, &verifyMode) == L7_SUCCESS)
  {
    if (verifyMode == CP_VERIFY_MODE_LOCAL)
    {
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), 
                             pStrInfo_security_Verification_1, pStrInfo_security_VerificationModeLocal);
    }
    else if (verifyMode == CP_VERIFY_MODE_RADIUS)
    {
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), 
                             pStrInfo_security_Verification_1, pStrInfo_security_VerificationModeRadius);
    }
    else
    {
      osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), 
                             pStrInfo_security_Verification_1, pStrInfo_security_VerificationModeGuest);
    }
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_VerificationError);
  }
  osapiStrncat(webStaticContentBuffer, buf, (sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer)-1));

  return webStaticContentBuffer;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext                  context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpClientIntfStatusP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpClientIntfStatus(EwsContext context, EwaForm_cpClientIntfStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  form->value.err_msg = L7_NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag == L7_TRUE)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
  }

  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext                  context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpClientIntfStatusP form    @b{(input)} pointer to the form
*
* @returns  cp_client_intf_status.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpClientIntfStatus(EwsContext context, EwaForm_cpClientIntfStatusP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  appInfo.data[0] = form->value.assoced_if_id;
  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPClientIntfStatus);
}


/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_clients.c
*
* @purpose    Code in support of the cp_clients.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     9/9/2007
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
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "l7utils_api.h"
#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "util_pstring.h"


/*********************************************************************
* @purpose  Get the client entry as HTML data for status parameters
*
* @param    macAddr  @b{(input)} Client mac address
* @param    idx      @b{(input)} Entry index
*
* @returns  Display buffer
*
* @end
*********************************************************************/
L7_char8 *usmWebCPClientEntryGet(L7_enetMacAddr_t macAddr, L7_uint32 idx)
{
  cpdmAuthWhichSwitch_t swType;
  L7_IP_ADDR_t ipAddr;
  L7_char8 ipBuf[USMWEB_IPADDR_SIZE];
  L7_char8 buf[USMWEB_BUFFER_SIZE_256];
  L7_LOGIN_TYPE_t protocol;
  CP_VERIFY_MODE_t verifyMode;
  L7_uchar8 userName[CP_USER_LOCAL_USERNAME_MAX+1];

  memset(webStaticContentBuffer, 0, sizeof(webStaticContentBuffer));

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

  /* user */
  memset(userName, 0, sizeof(userName));
  memset(buf, 0, sizeof(buf));
  if (usmDbCpdmClientConnStatusUserNameGet(&macAddr, userName) == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_User, userName);
  }
  else
  {
    osapiSnprintfAddBlanks(0,0,0,1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_security_UserError);
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
* @param    EwsContext         context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpClientsP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpClients(EwsContext context, EwaForm_cpClientsP form)
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

  form->value.Deauth = (char *)osapiStrDup(pStrInfo_common_Del);
  form->status.Deauth |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Deauth_all = (char *)osapiStrDup(pStrInfo_common_DelAll);
  form->status.Deauth_all |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->value.Refresh = (char *) osapiStrDup(pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
  form->status.selectedMACs |= (EW_FORM_INITIALIZED);
  form->value.selectedMACs = NULL;
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext         context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpClientsP form    @b{(input)} pointer to the form
*
* @returns  cp_clients.html
*
* @end
*********************************************************************/
char *ewaFormSubmit_cpClients(EwsContext context, EwaForm_cpClientsP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 *ptok;
  L7_enetMacAddr_t macAddr;
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_char8 errBuf[USMWEB_BUFFER_SIZE_512];
  L7_char8 tmpBuf[USMWEB_APP_DATA_SIZE+1];
  L7_uint32 count=0, deleteCount=0;

  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  if (net->app_pointer == NULL)
  {
    ewsContextSendReply(context, pStrInfo_security_HtmlFileCPUserLocal);
    return L7_NULL;
  }
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;
  memset(errBuf, 0x00, sizeof(errBuf));
  memset(errMsg, 0x00, sizeof(errMsg));

  if (form->status.Deauth & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 1)
    {
      ptok = strtok(form->value.selectedMACs, ";");
      while (ptok != L7_NULL)
      {
        count++;
        memset(&macAddr, 0x00, sizeof(macAddr));
        (void)l7utilsMacAddrStringToHex(ptok, macAddr.addr);
        if (usmDbCpdmClientConnStatusGet(&macAddr) == L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          if (usmDbCpdmClientConnStatusDelete(&macAddr) != L7_SUCCESS)
          {
            osapiSnprintf(errMsg, sizeof(errMsg), pStrInfo_security_DeauthClientFailed);
            osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
          }
          else
          {
            if (deleteCount == 0)
            {
              osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_security_DeauthClients);
            }
            osapiSnprintf(tmpBuf, sizeof(tmpBuf), "\r\n%s", ptok);
            osapiStrncat(errMsg, tmpBuf, (sizeof(errMsg)-strlen(errMsg)-1));
            deleteCount++;
          }
        }
        ptok = strtok(L7_NULL, ";");
      }

      osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
      if (count == 0)
      {
        usmWebAppInfoCatErrMsg(&appInfo, pStrErr_common_Error, pStrInfo_security_SelectCheckBox);
      }
    }
  }

  else if (form->status.Deauth_all & EW_FORM_RETURNED)
  {
    if (form->value.refresh_key == 2)
    {
      appInfo.err.err_flag = L7_TRUE;
      if (usmDbCpdmClientConnStatusDeleteAll() != L7_SUCCESS)
      {
        osapiSnprintfAddBlanks(0,1,0,0, pStrErr_common_Error, errMsg, sizeof(errMsg), 
                               pStrErr_common_FailedToDel, pStrInfo_security_Clients);
      }
      else
      {
        osapiSnprintfAddBlanks(1,0,0,0, L7_NULLPTR, errMsg, sizeof(errMsg), 
                               pStrInfo_security_ClientsAllDeauth);
      }
      osapiStrncat(appInfo.err.msg, errMsg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1));
    }
  }

  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPClients);
}


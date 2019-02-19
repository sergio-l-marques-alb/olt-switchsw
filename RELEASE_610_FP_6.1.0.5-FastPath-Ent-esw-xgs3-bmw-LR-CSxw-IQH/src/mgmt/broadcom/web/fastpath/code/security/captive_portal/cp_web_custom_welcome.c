/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
*
* @filename cp_web_customer_welcome.html
*
* @purpose  web customization iframe content
*
* @comments 
*
* @create 01-05-2009
*
* @author darsen
* @end
*
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "l7_common.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "web.h"
#include "web_buffer.h"
#include "web_oem.h"
#include "strlib_security_web.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "ews_form.h" /* for EwsFormInputFileState */
#include "strlib_common_common.h"
#include "strlib_common_web.h"


#define CLEAR       1
#define CP_APPINFO_CPID_IDX     0
#define CP_APPINFO_WEBID_IDX    1

/*****************************************************************************
 ****************************************************************************/
void ewaFormServe_cpWebCustomWelcome ( EwsContext context, EwaForm_cpWebCustomWelcomeP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  cpId_t cpId = CP_ID_MIN;
  webId_t webId = CP_WEB_ID_MIN;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 tmp[CP_AUP_TEXT_MAX+1]; /* max UTF-16 format */
  L7_char8 buf[CP_HEX_NCR_MAX+1]; /* max Hexadecimal NCR */
  L7_char8  *pStr;

  form->value.err_msg = NULL;
  form->status.err_msg |= (EW_FORM_INITIALIZED);
  form->value.err_flag = L7_FALSE;
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  memset(data, 0, sizeof(data));

  if (ewsCGIQueryString(context, data, sizeof(data)) > 0)
  {
    pStr = strstr(data, "cp=");
    if (pStr != L7_NULLPTR)
    {
      pStr += strlen("cp=");
      cpId = (cpId_t) atoi(pStr);
    }
    pStr = strstr(data, "web=");
    if (pStr != L7_NULLPTR)
    {
      pStr += strlen("web=");
      webId = (webId_t) atoi(pStr);
    }
  }
  else /* No Query String */
  {
    net = ewsContextNetHandle(context);
    if (net->app_pointer != L7_NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      cpId = (cpId_t) appInfo.data[CP_APPINFO_CPID_IDX];
      webId = (webId_t) appInfo.data[CP_APPINFO_WEBID_IDX];
      form->value.err_flag = appInfo.err.err_flag;
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_DYNAMIC);
    }
  }

  form->value.cpId = (L7_uint32) cpId;
  form->value.webId = (L7_uint32) webId;

  /* authentication page branding image */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebBrandingImageNameGet(cpId,webId,buf))
  {
    form->value.cp_welcome_branding_image = osapiStrDup(buf);
    form->status.cp_welcome_branding_image |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* authentication page browser title */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebBrowserTitleTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_welcome_browser_title = osapiStrDup(buf);
    form->status.cp_welcome_browser_title |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* welcome title */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebWelcomeTitleTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_welcome_title = osapiStrDup(buf);
    form->status.cp_welcome_title |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* welcome text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebWelcomeTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_welcome_text = osapiStrDup(buf);
    form->status.cp_welcome_text |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/******************************************************************************
******************************************************************************/
char *ewaFormSubmit_cpWebCustomWelcome ( EwsContext context, EwaForm_cpWebCustomWelcomeP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  cpId_t cpId;
  webId_t webId;
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
  L7_char8 buf[CP_AUP_TEXT_MAX+1]; /* max UTF-16 string */
  L7_char8 langCode[CP_LANG_CODE_MAX+1];
  L7_char8 langLink[CP_LOCALE_LINK_MAX+1];
  L7_char8  *pStr;

  cpId = (cpId_t) form->value.cpId;
  webId = (webId_t) form->value.webId;

  if ((0==cpId) || (0==webId))
  {
    memset(data, 0, sizeof(data));
    if (ewsCGIQueryString(context, data, sizeof(data)) > 0)
    {
      pStr = strstr(data, "cp=");
      if (pStr != L7_NULLPTR)
      {
        pStr += strlen("cp=");
        cpId = (cpId_t) atoi(pStr);
      }
      pStr = strstr(data, "web=");
      if (pStr != L7_NULLPTR)
      {
        pStr += strlen("web=");
        webId = (webId_t) atoi(pStr);
      }
    }
    else /* No Query String */
    {
      net = ewsContextNetHandle(context);
      if (net->app_pointer != L7_NULL)
      {
        memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
        cpId = (cpId_t) appInfo.data[CP_APPINFO_CPID_IDX];
        webId = (webId_t) appInfo.data[CP_APPINFO_WEBID_IDX];
        form->value.err_flag = appInfo.err.err_flag;
        form->value.err_msg = osapiStrDup(appInfo.err.msg);
        form->status.err_msg |= (EW_FORM_DYNAMIC);
      }
    }
  }

  memset(errMsg, 0x00, sizeof(errMsg));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;
  appInfo.data[CP_APPINFO_CPID_IDX] = (L7_uint32) cpId;
  appInfo.data[CP_APPINFO_WEBID_IDX] = (L7_uint32) webId;
  memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));

  /* SUBMIT */
  if ((form->status.Submit & EW_FORM_RETURNED))
  {

    /* welcome title */
    if (CP_WELCOME_TITLE_TEXT_MAX < strlen(form->value.cp_welcome_title_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_WelcomeTitle);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_WELCOME_TITLE_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_welcome_title_hex, strlen(form->value.cp_welcome_title_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebWelcomeTitleTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_WelcomeTitle);
      }
    }

    /* welcome text */
    if (CP_WELCOME_TEXT_MAX < strlen(form->value.cp_welcome_text_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_WelcomeText);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_WELCOME_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_welcome_text_hex, strlen(form->value.cp_welcome_text_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebWelcomeTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_WelcomeText);
      }
    }
  }

  /* CLEAR/RESET WEB ID */
  if (form->value.refresh_key == CLEAR)
  {
    memset(langCode,0,sizeof(langCode));
    memset(langLink,0,sizeof(langLink));
    usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,langCode);
    usmDbCpdmCPConfigWebLocaleLinkGet(cpId,webId,langLink);
    if (usmDbCpdmCPConfigWebAddDefaults(cpId,webId,langCode,CP_WELCOME_PAGE) != L7_SUCCESS)
    {
      usmDbCpdmCPConfigWebLangCodeSet(cpId,webId,langCode);
      usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,langLink);
    }
  }

  memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
  ewsContextSendReply(context,"cp_web_custom_welcome.html");
  return NULL;
}




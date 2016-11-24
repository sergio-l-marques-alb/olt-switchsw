/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
*
* @filename cp_web_customer_auth.html
*
* @purpose  web customization iframe content
*
* @comments 
*
* @create 12-16-2008
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
#define DELETE      2
#define CP_APPINFO_CPID_IDX     0
#define CP_APPINFO_WEBID_IDX    1

/*****************************************************************************
 ****************************************************************************/
void ewaFormServe_cpWebCustomAuth ( EwsContext context, EwaForm_cpWebCustomAuthP form )
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

  /* authentication page background image */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebBackgroundImageNameGet(cpId,webId,buf))
  {
    form->value.cp_auth_background_image = osapiStrDup(buf);
    form->status.cp_auth_background_image |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* authentication page branding image */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebBrandingImageNameGet(cpId,webId,buf))
  {
    form->value.cp_auth_branding_image = osapiStrDup(buf);
    form->status.cp_auth_branding_image |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* browser title */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebBrowserTitleTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_browser_title = osapiStrDup(buf);
    form->status.cp_browser_title |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* page title */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebTitleTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_page_title = osapiStrDup(buf);
    form->status.cp_page_title |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* account label */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebAccountLabelGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_account_label = osapiStrDup(buf);
    form->status.cp_account_label |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* user label */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebUserLabelGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_user_label = osapiStrDup(buf);
    form->status.cp_user_label |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* password label */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebPasswordLabelGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_password_label = osapiStrDup(buf);
    form->status.cp_password_label |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* button label */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebButtonLabelGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_button_label = osapiStrDup(buf);
    form->status.cp_button_label |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* AUP text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebAUPTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_aup_text = osapiStrDup(buf);
    form->status.cp_aup_text |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* accept text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebAcceptTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_accept_text = osapiStrDup(buf);
    form->status.cp_accept_text |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* instructional text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebInstructionalTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_instruct_text = osapiStrDup(buf);
    form->status.cp_instruct_text |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* denied msg */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebDeniedMsgTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_denied_msg = osapiStrDup(buf);
    form->status.cp_denied_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* resource msg */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebResourceMsgTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_resource_msg = osapiStrDup(buf);
    form->status.cp_resource_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* timeout msg */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebTimeoutMsgTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_timeout_msg = osapiStrDup(buf);
    form->status.cp_timeout_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* WIP msg */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebWipMsgTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_wip_msg = osapiStrDup(buf);
    form->status.cp_wip_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* noaccept msg */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebNoAcceptMsgTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_noaccept_msg = osapiStrDup(buf);
    form->status.cp_noaccept_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* separator color */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigSeparatorColorGet(cpId,buf))
  {
    form->value.cp_sp = osapiStrDup(buf);
    form->status.cp_sp |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* foreground color */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigForegroundColorGet(cpId,buf))
  {
    form->value.cp_fg = osapiStrDup(buf);
    form->status.cp_fg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* background color */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigBackgroundColorGet(cpId,buf))
  {
    form->value.cp_bg = osapiStrDup(buf);
    form->status.cp_bg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }
  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/******************************************************************************
******************************************************************************/
char *ewaFormSubmit_cpWebCustomAuth ( EwsContext context, EwaForm_cpWebCustomAuthP form )
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
  L7_BOOL fError = L7_FALSE;
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
  if ((form->status.Submit & EW_FORM_RETURNED) &&
      (form->value.refresh_key != DELETE))
  {

   /* account image */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_account_image,CP_FILE_NAME_MAX);
    if (osapiStrncmp(buf,WEB_DEF_NOSEL_IMAGE_NAME,CP_FILE_NAME_MAX) == 0)
    {
      memset(buf, 0, sizeof(buf));
    }   
    if (L7_SUCCESS != usmDbCpdmCPConfigWebAccountImageNameSet(cpId,webId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Account Image Name");
    }

    /* browser title */
    if (CP_BROWSER_TITLE_TEXT_MAX < strlen(form->value.cp_browser_title_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_BrowserTitle);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_CP_BROWSER_TITLE_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_browser_title_hex, strlen(form->value.cp_browser_title_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebBrowserTitleTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;


        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_BrowserTitle);
      }
    }

    /* page title */
    if (CP_TITLE_TEXT_MAX < strlen(form->value.cp_page_title_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_PageTitle);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_TITLE_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_page_title_hex, strlen(form->value.cp_page_title_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebTitleTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_PageTitle);
      }
    }

    /* account label */
    if (CP_ACCOUNT_LABEL_MAX < strlen(form->value.cp_account_label_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_AccountLabel);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_ACCOUNT_LABEL_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_account_label_hex, strlen(form->value.cp_account_label_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebAccountLabelSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_AccountLabel);
      }
    }

    /* user label */
    if (CP_USER_LABEL_MAX < strlen(form->value.cp_user_label_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_UserLabel);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_USER_LABEL_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_user_label_hex, strlen(form->value.cp_user_label_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebUserLabelSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_UserLabel);
      }
    }

    /* password label */
    if (CP_PASSWORD_LABEL_MAX < strlen(form->value.cp_password_label_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_PasswordLabel);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_PASSWORD_LABEL_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_password_label_hex, strlen(form->value.cp_password_label_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebPasswordLabelSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_PasswordLabel);
      }
    }

    /* button label */
    if (CP_BUTTON_LABEL_MAX < strlen(form->value.cp_button_label_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_ButtonLabel);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_BUTTON_LABEL_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_button_label_hex, strlen(form->value.cp_button_label_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebButtonLabelSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_ButtonLabel);
      }
    }

    /* AUP text */
    if (CP_AUP_TEXT_MAX < strlen(form->value.cp_aup_text_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_AUPText);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_AUP_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_aup_text_hex, strlen(form->value.cp_aup_text_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebAUPTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_AUPText);
      }
    }

    /* accept text */
    if (CP_ACCEPT_TEXT_MAX < strlen(form->value.cp_accept_text_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_AcceptText);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_ACCEPT_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_accept_text_hex, strlen(form->value.cp_accept_text_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebAcceptTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_AcceptText);
      }
    }

    /* instructional text */
    if (CP_INSTRUCTIONAL_TEXT_MAX < strlen(form->value.cp_instruct_text_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_InstructionalText);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_INSTRUCTIONAL_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_instruct_text_hex, strlen(form->value.cp_instruct_text_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebInstructionalTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_InstructionalText);
      }
    }

    /* denied msg */
    if (CP_MSG_TEXT_MAX < strlen(form->value.cp_denied_msg_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_DeniedMsg);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_MSG_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_denied_msg_hex, strlen(form->value.cp_denied_msg_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebDeniedMsgTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_DeniedMsg);
      }
    }

    /* resource msg */
    if (CP_MSG_TEXT_MAX < strlen(form->value.cp_resource_msg_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_ResourceMsg);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_MSG_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_resource_msg_hex, strlen(form->value.cp_resource_msg_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebResourceMsgTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_ResourceMsg);
      }
    }

    /* timeout msg */
    if (CP_MSG_TEXT_MAX < strlen(form->value.cp_timeout_msg_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_TimeoutMsg);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_MSG_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_timeout_msg_hex, strlen(form->value.cp_timeout_msg_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebTimeoutMsgTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_TimeoutMsg);
      }
    }

    /* WIP msg */
    if (CP_MSG_TEXT_MAX < strlen(form->value.cp_wip_msg_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_WIPMsg);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_MSG_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_wip_msg_hex, strlen(form->value.cp_wip_msg_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebWipMsgTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_WIPMsg);
      }
    }

    /* noaccept msg */
    if (CP_MSG_TEXT_MAX < strlen(form->value.cp_noaccept_msg_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_NoAcceptMsg);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_MSG_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_noaccept_msg_hex, strlen(form->value.cp_noaccept_msg_hex));
      fError = ((0==strlen(buf)) && (0!=strlen(form->value.cp_accept_text_hex)))?L7_TRUE:L7_FALSE;
      if ((L7_TRUE == fError) || (L7_SUCCESS != usmDbCpdmCPConfigWebNoAcceptMsgTextSet(cpId,webId,buf)))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_NoAcceptMsg);
      }
    }

    /* Separator Color */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_sp, strlen(form->value.cp_sp));
    if (L7_SUCCESS != usmDbCpdmCPConfigSeparatorColorSet(cpId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Separator Color");
    }

    /* Foreground Color */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_fg, strlen(form->value.cp_fg));
    if (L7_SUCCESS != usmDbCpdmCPConfigForegroundColorSet(cpId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Foreground Color");
    }

    /* Background Color */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_bg, strlen(form->value.cp_bg));
    if (L7_SUCCESS != usmDbCpdmCPConfigBackgroundColorSet(cpId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Background Color");
    }
  }

  /* CLEAR/RESET WEB ID */
  if (form->value.refresh_key == CLEAR)
  {
    memset(langCode,0,sizeof(langCode));
    memset(langLink,0,sizeof(langLink));
    usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,langCode);
    usmDbCpdmCPConfigWebLocaleLinkGet(cpId,webId,langLink);
    if (usmDbCpdmCPConfigWebAddDefaults(cpId,webId,langCode,CP_AUTHENTICATION_PAGE) != L7_SUCCESS)
    {
      usmDbCpdmCPConfigWebLangCodeSet(cpId,webId,langCode);
      usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,langLink);
    }
    usmDbCpdmCPConfigForegroundColorSet(cpId,CP_DEF_FOREGROUND_COLOR);
    usmDbCpdmCPConfigBackgroundColorSet(cpId,CP_DEF_BACKGROUND_COLOR);
    usmDbCpdmCPConfigSeparatorColorSet(cpId,CP_DEF_SEPARATOR_COLOR);
  }

  memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
  ewsContextSendReply(context,"cp_web_custom_auth.html");
  return NULL;
}




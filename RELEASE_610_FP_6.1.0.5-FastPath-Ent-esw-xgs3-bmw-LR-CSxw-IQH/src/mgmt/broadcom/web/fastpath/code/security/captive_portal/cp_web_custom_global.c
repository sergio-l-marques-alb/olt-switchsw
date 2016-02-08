/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
*
* @filename cp_web_customer_global.html
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

extern L7_RC_t deleteImage(L7_char8 *imageName);

/*****************************************************************************
 ****************************************************************************/
void ewaFormServe_cpWebCustomGlobal ( EwsContext context, EwaForm_cpWebCustomGlobalP form )
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

  /* font list */
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebFontListGet(cpId,webId,buf))
  {
    form->value.cp_font_list = osapiStrDup(buf);
    form->status.cp_font_list |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* script text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebScriptTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_logout_script_text = osapiStrDup(buf);
    form->status.cp_logout_script_text |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  /* popup text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebPopupTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.cp_logout_popup_text = osapiStrDup(buf);
    form->status.cp_logout_popup_text |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  form->value.refresh_in_progress = 0;
  form->status.refresh_in_progress |= (EW_FORM_INITIALIZED);
  form->value.refresh_key = 0;
  form->status.refresh_key |= (EW_FORM_INITIALIZED);
}

/******************************************************************************
******************************************************************************/
char *ewaFormSubmit_cpWebCustomGlobal ( EwsContext context, EwaForm_cpWebCustomGlobalP form )
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
  L7_uint32 fileSize = 0;
  EwsFormInputFileState *file = context->inputFile;

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

  /* IMAGE DOWNLOAD */
  if (form->status.download_btn & EW_FORM_RETURNED)
  {

    memset(buf, 0, sizeof(buf));
    if (form->value.filename_handle == NULL)
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       "Invalid file transfer! Check file name and type.\nPlease note that the file name is limited to %d characters.",
                       CP_FILE_NAME_MAX);
    }
    else
    {
      /* by emweb design, we must close the file descriptor here */
      osapiFsClose(form->value.filename_handle->fd);

      /* Best attempt to detect device full (as determined by ewsParseFormData) */
      if (file->formFile == EWA_FILE_HANDLE_NULL)
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         "Invalid file transfer! Check free space.\nPlease note that RAM Disk space is limited to %d bytes.",
                         L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE);
      }
      /* Have yet to see this failure */
      else if (form->status.filename & EW_FORM_FILE_ERROR)
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         pStrErr_common_CfgAclsStringFmt,
                         pStrErr_security_FileTypeInvalidOrErrorProcessingFile);
      }
      else
      {
        /* Check target file size as a final verification */
        if (L7_SUCCESS == osapiFsFileSizeGet(file->formFile->localPath,&fileSize))
        {
          if (fileSize <= 0)
          {
            if (L7_SUCCESS == osapiFsDeleteFile(file->formFile->localPath))
            {
              appInfo.err.err_flag = L7_TRUE;
              osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                               pStrErr_common_CfgAclsStringFmt,
                               pStrErr_security_FileTransferPleaseVerifyInputFile);
            }
          }
        }
      }
    }
  }

  /* SUBMIT */
  if ((form->status.Submit & EW_FORM_RETURNED) &&
      (form->value.refresh_key != DELETE))
  {

   /* background image */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_background_image,CP_FILE_NAME_MAX);
    if (osapiStrncmp(buf,WEB_DEF_NOSEL_IMAGE_NAME,CP_FILE_NAME_MAX) == 0)
    {
      memset(buf, 0, sizeof(buf));
    }
    if (L7_SUCCESS != usmDbCpdmCPConfigWebBackgroundImageNameSet(cpId,webId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Background Image Name");
    }

   /* branding image */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_branding_image,CP_FILE_NAME_MAX);
    if (osapiStrncmp(buf,WEB_DEF_NOSEL_IMAGE_NAME,CP_FILE_NAME_MAX) == 0)
    {
      memset(buf, 0, sizeof(buf));
    }
    if (L7_SUCCESS != usmDbCpdmCPConfigWebBrandingImageNameSet(cpId,webId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Branding Image Name");
    }

    /* font list */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, form->value.cp_font_list, strlen(form->value.cp_font_list));
    if (L7_SUCCESS != usmDbCpdmCPConfigWebFontListSet(cpId,webId,buf))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, "Browser Font List");
    }

    /* script text */
    if (CP_SCRIPT_TEXT_MAX < strlen(form->value.cp_logout_script_text_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_ScriptText);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_SCRIPT_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_logout_script_text_hex, strlen(form->value.cp_logout_script_text_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebScriptTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_ScriptText);
      }
    }

    /* popup text */
    if (CP_POPUP_TEXT_MAX < strlen(form->value.cp_logout_popup_text_hex))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                       pStrErr_common_FailedToSet, pStrInfo_security_PopupText);
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       pStrInfo_security_MaxNumOfChars, 
                       CP_UI_POPUP_TEXT_MAX);
    }
    else
    {
      memset(buf, 0, sizeof(buf));
      memcpy(buf, form->value.cp_logout_popup_text_hex, strlen(form->value.cp_logout_popup_text_hex));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebPopupTextSet(cpId,webId,buf))
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1), "%s%s.\r\n", 
                         pStrErr_common_FailedToSet, pStrInfo_security_PopupText);
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
    if (usmDbCpdmCPConfigWebAddDefaults(cpId,webId,langCode,CP_GLOBAL_PAGE) != L7_SUCCESS)
    {
      usmDbCpdmCPConfigWebLangCodeSet(cpId,webId,langCode);
      usmDbCpdmCPConfigWebLocaleLinkSet(cpId,webId,langLink);
    }
  }

  /* IMAGE DELETE */
  if (form->value.refresh_key == DELETE)
  {
    if ((strlen(form->value.cp_image)==0) ||
        (osapiStrncmp(form->value.cp_image,WEB_DEF_NOSEL_IMAGE_NAME,CP_FILE_NAME_MAX)== 0) ||
        (osapiStrncmp(form->value.cp_image,WEB_DEF_ACCOUNT_IMAGE_NAME,CP_FILE_NAME_MAX)== 0) ||
        (osapiStrncmp(form->value.cp_image,WEB_DEF_BACKGROUND_IMAGE_NAME,CP_FILE_NAME_MAX)== 0) ||
        (osapiStrncmp(form->value.cp_image,WEB_DEF_BRANDING_IMAGE_NAME,CP_FILE_NAME_MAX)== 0) ||
        (osapiStrncmp(form->value.cp_image,WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME,CP_FILE_NAME_MAX)== 0))
    {
      appInfo.err.err_flag = L7_TRUE;
      osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                       "Can not delete, invalid file selection!");
    }

    if (L7_TRUE != appInfo.err.err_flag)
    {
      if (L7_SUCCESS == deleteImage(form->value.cp_image))
      {
        usmDbCpdmCPConfigWebImageResetAll(form->value.cp_image);
      }
      else
      {
        appInfo.err.err_flag = L7_TRUE;
        osapiSnprintfcat(appInfo.err.msg, (sizeof(appInfo.err.msg)-strlen(appInfo.err.msg)-1),
                         "Failed to delete image file.");
      }
    }
  }

  memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
  ewsContextSendReply(context,"cp_web_custom_global.html");
  return NULL;
}




/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename   src/mgmt/emweb/web/security/captive_portal/cp_web_custom.c
*
* @purpose    Code in support of the cp_web_custom.html page
*
* @component  Captive Portal
*
* @comments
*
* @create     1/20/2008
*
* @author     darsen
*
* @end
*
*********************************************************************/

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

/*********************************************************************
*
* @purpose  Delete locale image file from CP ram directory
*
*
* @param    L7_char8 *imageName @b{(input)} file name to delete
*
* @notes    none
* @returns  L7_FAILURE or L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t deleteImage(L7_char8 *imageName)
{
  L7_char8 searchName[CP_FILE_NAME_MAX+1];
  L7_char8 fileName[CP_FILE_NAME_MAX+1];
  L7_char8 tempName[CP_FILE_NAME_MAX+1];
  struct dirent *entry;
  L7_int32 FQF_MAX = 256;
  L7_char8 fqf[FQF_MAX];
  DIR *dir = NULL;

  if ((imageName == L7_NULLPTR) ||
      (strlen(imageName) > CP_FILE_NAME_MAX) ||
      ((dir = opendir(RAM_CP_PATH RAM_CP_NAME)) == NULL))
  {
    return L7_FAILURE;
  }

  memset(searchName,0,sizeof(searchName));
  osapiSnprintfcat(searchName,sizeof(searchName),"%s",imageName);
  osapiConvertToLowerCase(searchName);

  /* Search target file name for deletion */
  while ((NULL!=dir) && (entry = readdir(dir)) != NULL)
  {
    memset(fileName,0,sizeof(fileName));
    memset(tempName,0,sizeof(tempName));

    osapiSnprintfcat(fileName,sizeof(fileName),"%s",entry->d_name);
    osapiSnprintfcat(tempName,sizeof(tempName),"%s",fileName);
    osapiConvertToLowerCase(tempName);

    if (osapiStrncmp(searchName,tempName,sizeof(searchName)) == 0)
    {
      memset(fqf,0,FQF_MAX);
      osapiSnprintfcat(fqf,FQF_MAX,"%s%s/%s",RAM_CP_PATH,RAM_CP_NAME,fileName);

      if (0==remove(fqf))
      {
        usmDbCpdmSetImageFileChanged();
        if (NULL!=dir)
        {
          closedir(dir);
        }
        return L7_SUCCESS;
      }
    }
  }
  if (NULL!=dir)
  {
    closedir(dir);
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose  Determine if the cooresponding form object (id) should be
*           enabled
*
* @param    EwsContext context @b{(input)} web context for param extraction
* @param    usmWeb_cpFormObjectId_t id @b{(input)} form object identifier
*
* @returns  L7_BOOL L7_TRUE or L7_FALSE
*
* @end
*
*********************************************************************/
L7_BOOL usmWebCPWebFormObjectEnabled(EwsContext context, usmWeb_cpFormObjectId_t id)
{
  EwaNetHandle net = ewsContextNetHandle(context);
  cpAppInfo_t appInfo;
  CP_VERIFY_MODE_t verifyMode;
  L7_uchar8 userLogout;
  cpId_t cpId;

  if (net->app_pointer != L7_NULL)      
  {
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));
    cpId  = (cpId_t) appInfo.data[CP_APPINFO_CPID_IDX];

    switch (id)
    {
      case CP_FORM_OBJ_PWD:
      {
        if (usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode) == L7_SUCCESS)
        {
          return ((verifyMode==CP_VERIFY_MODE_LOCAL) ||
                  (verifyMode==CP_VERIFY_MODE_RADIUS))?L7_TRUE:L7_FALSE;
        }
        break;
      }
      case CP_FORM_OBJ_USER_LOGOUT:
      {
        if (usmDbCpdmCPConfigUserLogoutModeGet(cpId,&userLogout) == L7_SUCCESS)
        {
          return (L7_ENABLE==userLogout)?L7_TRUE:L7_FALSE;
        }
        break;
      }
      default: /* usmWeb_cpGenericGetId_t is shared */
      {
        break;
      }
    }
  }
  return L7_FALSE;
}


/*********************************************************************
*
* @purpose  Retrieve the generic string as indicated by the ID.
*
* @param    EwsContext context @b{(input)} web context for param extraction
* @param    L7_char8 *id @b{(input)} string id, must match caller
*
* @returns  L7_char8 string
*
* @end
*
*********************************************************************/
L7_char8 *usmWebCPGenericWebGet(EwsContext context, usmWeb_cpGenericGetId_t id)
{
  memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));
  switch (id)
  {
    case CP_GET_ID_NOSEL_IMAGE:
    {
      osapiSnprintf(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",WEB_DEF_NOSEL_IMAGE_NAME);
      break;
    }
    case CP_GET_ID_DEFAULT_ACCOUNT_IMAGE:
    {
      osapiSnprintf(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",WEB_DEF_ACCOUNT_IMAGE_NAME);
      break;
    }
    case CP_GET_ID_DEFAULT_BACKGROUND_IMAGE:
    {
      osapiSnprintf(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",WEB_DEF_BACKGROUND_IMAGE_NAME);
      break;
    }
    case CP_GET_ID_DEFAULT_BRANDING_IMAGE:
    {
      osapiSnprintf(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",WEB_DEF_BRANDING_IMAGE_NAME);
      break;
    }
    case CP_GET_ID_DEFAULT_LOGOUT_SUCCESS_BACKGROUND_IMAGE:
    {
      osapiSnprintf(webStaticContentBuffer,sizeof(webStaticContentBuffer),"%s",WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME);
      break;
    }
    default: /* usmWeb_cpGenericGetId_t is shared */
    {
      break;
    }
  }
  return webStaticContentBuffer;
}

/*********************************************************************
* @purpose  Get the list of available image names
*
* @param    context @b{(input)} EmWeb/Server request context handle
* @param    id      @b{(input)} desired image
*
* @returns
*
* @end
*********************************************************************/
L7_char8 *usmWebCPImageFileSelect(EwsContext context, usmWeb_cpGenericGetId_t id)
{
  L7_char8 SEL1[] = "<SELECT CLASS='buttonattributes' name='%s' id='%s' SIZE='1' onChange='imageSelect(this.options[this.selectedIndex].text);'>";
  L7_char8 SEL2[] = "<SELECT CLASS='buttonattributes' name='%s' id='%s' SIZE='1' onChange='setDirty(AUTH_PAGE,true);'>";
  L7_char8 SEL3[] = "<SELECT CLASS='buttonattributes' name='%s' id='%s' SIZE='1' onChange='setDirty(SUCCESS_PAGE,true);'>";
  L7_char8 OPTE[]  = "<OPTION VALUE=''>< Not Available >";
  L7_char8 OPTS[]  = "<OPTION VALUE='%s' SELECTED>%s";
  L7_char8 OPTN[]  = "<OPTION VALUE='%s'>%s";
  L7_char8 END[]   = "</SELECT>";
  EwaNetHandle net = ewsContextNetHandle(context);
  L7_char8 fileName[CP_FILE_NAME_MAX+1];
  L7_char8 selName[CP_FILE_NAME_MAX+1];
  L7_BOOL fError = L7_FALSE;
  L7_uint32 count = 0;
  cpAppInfo_t appInfo;
  cpId_t cpId;
  L7_short16 webId;
  L7_char8 *ext;
  struct dirent *entry;
  DIR *dir = NULL;

  if (net->app_pointer != L7_NULL)      
  {
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));
    cpId  = (cpId_t) appInfo.data[CP_APPINFO_CPID_IDX];
    webId = (webId_t) appInfo.data[CP_APPINFO_WEBID_IDX];

    if ((dir = opendir(RAM_CP_PATH RAM_CP_NAME)) == NULL)
    {
      fError = L7_TRUE;
    }

    memset(selName,0,sizeof(selName));
    switch (id)
    {
      case CP_GET_ID_IMAGES:
      {
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),SEL1,"sel_images","sel_images");
        if (L7_TRUE == fError)
        {
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",OPTE);
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return webStaticContentBuffer;          
        }
        break;
      }
      case CP_GET_ID_ACCOUNT_IMAGE:
      {
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),SEL2,"sel_account_image","sel_account_image");
        if (L7_TRUE == fError)
        {
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",OPTE);
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return webStaticContentBuffer;          
        }
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                         OPTN,WEB_DEF_NOSEL_IMAGE_NAME,WEB_DEF_NOSEL_IMAGE_NAME);
        if (usmDbCpdmCPConfigWebAccountImageNameGet(cpId,webId,selName) != L7_SUCCESS)
        {
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return L7_NULL;
        }
        break;
      }
      case CP_GET_ID_BACKGROUND_IMAGE:
      {
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),SEL2,"sel_background_image","sel_background_image");
        if (L7_TRUE == fError)
        {
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",OPTE);
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return webStaticContentBuffer;          
        }
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                         OPTN,WEB_DEF_NOSEL_IMAGE_NAME,WEB_DEF_NOSEL_IMAGE_NAME);
        if (usmDbCpdmCPConfigWebBackgroundImageNameGet(cpId,webId,selName) != L7_SUCCESS)
        {
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return L7_NULL;
        }
        break;
      }
      case CP_GET_ID_BRANDING_IMAGE:
      {
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),SEL2,"sel_branding_image","sel_branding_image");
        if (L7_TRUE == fError)
        {
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",OPTE);
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return webStaticContentBuffer;          
        }
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                         OPTN,WEB_DEF_NOSEL_IMAGE_NAME,WEB_DEF_NOSEL_IMAGE_NAME);
        if (usmDbCpdmCPConfigWebBrandingImageNameGet(cpId,webId,selName) != L7_SUCCESS)
        {
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return L7_NULL;
        }
        break;
      }
      case CP_GET_ID_LOGOUT_SUCCESS_BACKGROUND_IMAGE:
      {
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer),SEL3,"sel_logout_success_image","sel_logout_success_image");
        if (L7_TRUE == fError)
        {
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",OPTE);
          osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return webStaticContentBuffer;          
        }
        osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                         OPTN,WEB_DEF_NOSEL_IMAGE_NAME,WEB_DEF_NOSEL_IMAGE_NAME);
        if (usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId,webId,selName) != L7_SUCCESS)
        {
          if (NULL!=dir)
          {
            closedir(dir);
          }
          return L7_NULL;
        }
        break;
      }
      default: /* usmWeb_cpGenericGetId_t is shared */
      {
        if (NULL!=dir)
        {
          closedir(dir);
        }
        return L7_NULL;
      }
    }

    /* Add default images to the list */
    if (osapiStrncmp(WEB_DEF_BACKGROUND_IMAGE_NAME,selName,CP_FILE_NAME_MAX) == 0)
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTS,WEB_DEF_BACKGROUND_IMAGE_NAME,WEB_DEF_BACKGROUND_IMAGE_NAME);
    }
    else
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTN,WEB_DEF_BACKGROUND_IMAGE_NAME,WEB_DEF_BACKGROUND_IMAGE_NAME);
    }

    if (osapiStrncmp(WEB_DEF_BRANDING_IMAGE_NAME,selName,CP_FILE_NAME_MAX) == 0)
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTS,WEB_DEF_BRANDING_IMAGE_NAME,WEB_DEF_BRANDING_IMAGE_NAME);
    }
    else
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTN,WEB_DEF_BRANDING_IMAGE_NAME,WEB_DEF_BRANDING_IMAGE_NAME);
    }

    if (osapiStrncmp(WEB_DEF_ACCOUNT_IMAGE_NAME,selName,CP_FILE_NAME_MAX) == 0)
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTS,WEB_DEF_ACCOUNT_IMAGE_NAME,WEB_DEF_ACCOUNT_IMAGE_NAME);
    }
    else
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTN,WEB_DEF_ACCOUNT_IMAGE_NAME,WEB_DEF_ACCOUNT_IMAGE_NAME);
    }
    /*
    The default logout success background image currently shares the background image

    if (osapiStrncmp(WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME,selName,CP_FILE_NAME_MAX) == 0)
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTS,WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME,WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME);
    }
    else
    {
      osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),
                       OPTN,WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME,WEB_DEF_LOGOUT_SUCCESS_BACKGROUND_IMAGE_NAME);
    }
    */

    /* Retrieve file names */
    while ((NULL!=dir) && (entry = readdir(dir)) != NULL)
    {
      memset(&fileName,0x00,sizeof(fileName));
      strncpy(fileName, entry->d_name, CP_FILE_NAME_MAX);
      ext = strrchr(fileName,'.');
      if ((NULL!=ext) && (4==strlen(ext)))
      {
        if ((strcmp(ext,".gif")==0) || (strcmp(ext,".GIF")==0) ||
            (strcmp(ext,".jpg")==0) || (strcmp(ext,".JPG")==0))
        {
          count++;
          if (osapiStrncmp(fileName,selName,CP_FILE_NAME_MAX) == 0)
          {
            osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),OPTS,fileName,fileName);
          }
          else
          {
            osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),OPTN,fileName,fileName);
          }
        }
      }
    }

    osapiSnprintfcat(webStaticContentBuffer,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer),"%s",END);
    if (NULL!=dir)
    {
      closedir(dir);
    }
    return webStaticContentBuffer;
  }
  if (NULL!=dir)
  {
    closedir(dir);
  }
  return L7_NULL;
}

/*********************************************************************
* @purpose  Initialize the strings on the form
*
* @param    EwsContext           context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpWebCustomP form    @b{(input)} pointer to the form
*
* @returns  none
*
* @end
*********************************************************************/
void ewaFormServe_cpWebCustom(EwsContext context, EwaForm_cpWebCustomP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  cpId_t cpId = CP_ID_MIN;
  webId_t webId = CP_WEB_ID_MIN;
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
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
}

/*********************************************************************
* @purpose  Save the current values in the form
*
* @param    EwsContext           context @b{(input)} EmWeb/Server request context handle
* @param    EwaForm_cpWebCustomP form    @b{(input)} pointer to the form
*
* @returns  cp_web_custom.html
*
* @end
*********************************************************************/
L7_char8 *ewaFormSubmit_cpWebCustom(EwsContext context, EwaForm_cpWebCustomP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  cpId_t cpId = 0;
  webId_t webId = 0;
  L7_char8 errMsg[USMWEB_ERROR_MSG_SIZE];
  L7_char8 data[USMWEB_APP_SMALL_DATA_SIZE];
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


  return usmWebEwsContextSendReply(context, L7_FALSE, &appInfo, pStrInfo_security_HtmlFileCPWebCustom);
}


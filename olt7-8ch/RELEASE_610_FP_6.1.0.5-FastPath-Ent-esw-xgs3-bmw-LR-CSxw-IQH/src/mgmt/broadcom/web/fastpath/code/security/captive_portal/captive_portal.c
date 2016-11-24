/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
 *
 * @filename  captive_portal.c
 *
 * @purpose   Web interface for user validation
 *
 * @component captive portal
 *
 * @comments
 *
 * @create    8/04/2007
 *
 * @author    darsenault
 *
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "transfer_exports.h"
#include "user_manager_exports.h"
#include "ews_ctxt.h"
#include "ew_proto.h"
#include "ewnet.h"
#include "ews.h"
#include "ews_ctxt.h"
#include "web.h"
#include "web_buffer.h"
#include "web_oem.h"
#include "intf_cb_api.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_web_api.h"
#include "usmdb_cpcm_api.h"
#include "osapi_support.h"
#include "user_mgr_api.h"
#include "cli_web_user_mgmt.h"
#include "commdefs.h"
#include "captive_portal_commdefs.h"
#include "captive_portal_defaultconfig.h"
#include "usmdb_cpdm_connstatus_api.h"

#define CP_BROWSER_REFRESH          5 /* seconds */
#define CP_PROTOCOL_STR_SIZE        6
#define CP_HOST_STR_SIZE            256

static L7_char8 tmp[CP_AUP_TEXT_MAX+1]; /* max UTF-16 format */
static L7_char8 buf[CP_HEX_NCR_MAX+1]; /* max Hexadecimal NCR */

/*********************************************************************
*
* @purpose  ewsContextSendReply wrapper to provide support for cpAppInfo_t
*
* @param    EwsContext context @b{(input)} web context
* @param    L7_BOOL create @b{(input)} flag for appinfo creation
* @param    cpAppInfo_t * appInfop @b{(input)} CP data structure to pass
* @param    L7_char8 * file @b{(input)} file to serve
*
* @end
*
*********************************************************************/
L7_char8 *sendReply (EwsContext context, L7_BOOL create,
                                     cpAppInfo_t * appInfop, L7_char8 * file)
{
  EwaNetHandle net = ewsContextNetHandle (context);

  if (create == L7_TRUE)
  {
    net->app_pointer = ewaAlloc(sizeof(cpAppInfo_t));
    memset(net->app_pointer,0,sizeof(cpAppInfo_t));
  }

  if (appInfop != L7_NULLPTR)
  {
    memcpy(net->app_pointer,appInfop,sizeof(cpAppInfo_t));
  }
  ewsContextSendReply (context, file);
  return L7_NULL;
}

/*********************************************************************
*
* @purpose  Return URL or PATH redirection based on cpId
*
* @param    cpId_t cpId @b{(input)} Captive Portal configuration  
*
* @returns  L7_char8 *redirection @b{(output)} URL or path specification
*
* @end
*
*********************************************************************/
L7_uchar8 *getAuthSuccessPath(cpId_t cpId)
{
  L7_char8 PROTOCOL[] = "http://";
  L7_char8 tmp[CP_WELCOME_URL_MAX+1];
  L7_uchar8 redirectMode;

  memset(tmp,0,sizeof(tmp));
  memset(webStaticContentBuffer,0,sizeof(webStaticContentBuffer));
  if (L7_SUCCESS != usmDbCpdmCPConfigRedirectModeGet(cpId,&redirectMode))
  {
    return webStaticContentBuffer;
  }

  if (L7_ENABLE==redirectMode)
  {
    if (L7_SUCCESS != usmDbCpdmCPConfigRedirectURLGet(cpId,tmp))
    {
      return webStaticContentBuffer;
    }
    osapiConvertToLowerCase(tmp);
    /* Search for http only (https could be used) */
    if (0!=osapiStrncmp(tmp,PROTOCOL,4))
    {
      strcpy(webStaticContentBuffer,PROTOCOL);
    }
    if (L7_SUCCESS != usmDbCpdmCPConfigRedirectURLGet(cpId,tmp))
    {
      return webStaticContentBuffer;
    }
    osapiStrncat(webStaticContentBuffer,tmp,sizeof(webStaticContentBuffer)-strlen(webStaticContentBuffer));
  }
  else
  {
    strcpy(webStaticContentBuffer,CP_URL_WELCOME_PATH);
  }
  return webStaticContentBuffer;
}

/*********************************************************************
*
* @purpose  Loop through the available web configurations in search
*           for a language match. If not found, return first webId.
*
* @param    cpId_t cpId @b{(input)} Captive Portal configuration
* @param    L7_char8 *lang @b{(input)} interested language 
* @params   L7_short16 *webId @b{(output)} associated web configuration
*
* @returns  L7_RC_t L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t getLangWebId(cpId_t cpId, L7_char8 *lang, L7_short16 *webId)
{
  L7_char8 tmpLang[USMWEB_BUFFER_SIZE_64]; /* tmp for lang comparision */
  L7_short16 nextWebId = 0;
  L7_short16 tmpWebId = 0;
  L7_RC_t rc = L7_SUCCESS;

  memset(tmpLang,0,sizeof(tmpLang));
  if (L7_SUCCESS != usmDbCpdmCPConfigWebIdNextGet(cpId,tmpWebId,&tmpWebId))
  {
    return L7_FAILURE;
  }
  if (L7_SUCCESS != usmDbCpdmCPConfigWebLangCodeGet(cpId,tmpWebId,tmpLang))
  {
    return L7_FAILURE;
  }
  /* Search for a match if we didn't get one the first time */
  if ((0!=strlen(lang)) && ((strcmp(lang,tmpLang)!=0)))
  {
    /* Loop in search for an exact match */
    while ((rc = usmDbCpdmCPConfigWebIdNextGet(cpId,tmpWebId,&nextWebId))==L7_SUCCESS)
    {
      memset(tmpLang,0,sizeof(tmpLang));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebLangCodeGet(cpId,nextWebId,tmpLang))
      {
        rc = L7_FAILURE;
        break;
      }
      if ((strcmp(lang,tmpLang)==0))
      {
        tmpWebId = nextWebId;
        break;
      }
      tmpWebId = nextWebId;
    }
    if (L7_SUCCESS!=rc)
    {
      tmpWebId = 0;
      nextWebId = 0;
      /* Loop in search for any match */
      while ((rc = usmDbCpdmCPConfigWebIdNextGet(cpId,tmpWebId,&nextWebId))==L7_SUCCESS)
      {
        memset(tmpLang,0,sizeof(tmpLang));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLangCodeGet(cpId,nextWebId,tmpLang))
        {
          rc = L7_FAILURE;
          break;
        }
        if (((strstr(lang,tmpLang)!=NULL)) || ((strstr(tmpLang,lang)!=NULL)))
        {
          tmpWebId = nextWebId;
          break;
        }
        tmpWebId = nextWebId;
      }
    }
  }
  *webId = (L7_SUCCESS==rc)?tmpWebId:1;
  return rc;
}

/*********************************************************************
*
* @purpose  For now, simply return the first browser language if the
*           lang is empty. We could get slick and parse through the
*           entire browser preference list.
*
* @param    EwsContext context @b{(input)} web context
* @param    cpId_t cpId @b{(input)} Captive Portal configuration
* @param    L7_char8 *lang @b{(input)} interested language 
* @param    L7_char8 *prefLang @b{(output)} interested language 
*
* @returns  L7_RC_t L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t getPrefLang(EwsContext context, cpId_t cpId, L7_char8 *lang, L7_char8 *prefLang)
{
  L7_char8 *pbuf;                               /* tmp for lang comma parse */
  L7_char8 userLang[USMWEB_BUFFER_SIZE_64];     /* safe copy for lang comparision */
  L7_char8 acceptLangs[USMWEB_BUFFER_SIZE_256]; /* from browser */

  osapiStrncpy(userLang,lang,sizeof(userLang));

  /* Get preferred browser lang if user didn't specify (via a link) */
  if (strlen(userLang)==0)
  {
    ewsContextAcceptLanguage(context,acceptLangs,sizeof(acceptLangs));
    osapiStrncpy(userLang,acceptLangs,sizeof(userLang));
    pbuf = strchr(userLang,',');
    if (pbuf != L7_NULLPTR)  /* comma separator found */
    {
      userLang[strlen(userLang)-strlen(pbuf)]='\0';
    }
  }

  osapiStrncpy(prefLang,userLang,sizeof(userLang));

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Get the associated message text
*
* @param    cpId_t cpId @b{(input)} Captive Portal configuration
* @param    L7_short16 webId @b{(input)} Web configuration
* @param    CP_AUTH_STATUS_FLAG_t flag @b{(input)} status flag
* @param    L7_char8 *msgText @b{(output)} 
*
* @returns  L7_RC_t L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t getMsgText(cpId_t cpId, L7_short16 webId, CP_AUTH_STATUS_FLAG_t flag, L7_char8 *msgText)
{
  L7_RC_t rc = L7_FAILURE;

  if (L7_NULL == msgText)
  {
    return rc;
  }

  switch (flag)
  {
    case CP_NOACCEPT:
    {
      rc = usmDbCpdmCPConfigWebNoAcceptMsgTextGet(cpId,webId,msgText);
      break;
    }
    case CP_WIP:
    case CP_RADIUS_WIP:
    {
      rc = usmDbCpdmCPConfigWebWipMsgTextGet(cpId,webId,msgText);
      break;
    }
    case CP_DENIED:
    {
      rc = usmDbCpdmCPConfigWebDeniedMsgTextGet(cpId,webId,msgText);
      break;
    }
    case CP_RESOURCE:
    {
      rc = usmDbCpdmCPConfigWebResourceMsgTextGet(cpId,webId,msgText);
      break;
    }
    case CP_TIMEOUT:
    {
      rc = usmDbCpdmCPConfigWebTimeoutMsgTextGet(cpId,webId,msgText);
      break;
    }
    default:
    {
      strcpy(msgText,"");
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Get the associated protocol text
*
* @param    L7_LOGIN_TYPE_t protocolMode @b{(input)}
* @param    L7_char8 *protocol @b{(output)} 
*
* @returns  L7_RC_t L7_SUCCESS or L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t getProtocolText(L7_LOGIN_TYPE_t protocolMode, L7_char8 *protocol)
{
  if (L7_LOGIN_TYPE_HTTPS==protocolMode)
  {
    strcpy(protocol,HTTPS_STR);
  }
  else
  {
    strcpy(protocol,HTTP_STR);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Create language preference links.
*
* @param    EwsContext context @b{(input)} web context for param extraction
*
* @comments This function constructs and returns HTML code that provides
*           the user with language link refs.
*
* @returns  L7_char8 HTML code provides language links wrapped in a table
*
* @end
*
*********************************************************************/
L7_char8 *usmWebCPLangLinksGet(EwsContext context)
{
  EwaNetHandle net = ewsContextNetHandle(context);
  cpAppInfo_t appInfo;
  L7_char8 host[CP_HOST_STR_SIZE];
  L7_char8 lang[USMWEB_BUFFER_SIZE_64];         /* user or browser */
  cpId_t cpId, webId, nextWebId;
  L7_char8 link[CP_LOCALE_LINK_MAX];
  L7_uint32 intf;
  L7_IP_ADDR_t ip;
  L7_short16 fWebId = 0;                        /* lang found flag */
  L7_char8 LINK_HEADER[] = "<div><font size=\"2\">";
  L7_char8 LINK_FOOTER[] = "</div>";
  L7_LOGIN_TYPE_t protocolMode = L7_LOGIN_TYPE_UNKNWN;
  L7_char8 protocol[CP_PROTOCOL_STR_SIZE];
  L7_BOOL fMultiLang = L7_FALSE;
  if (net->app_pointer != L7_NULL)      
  {
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    memset(tmp, 0, sizeof(tmp));
    memset(buf, 0, sizeof(buf));

    memset(host,0,sizeof(host));
    ewsContextHost(context,host,sizeof(host));
    intf = (L7_uint32) appInfo.data[CP_APPINFO_INTF_IDX];
    ip   = (L7_IP_ADDR_t) appInfo.data[CP_APPINFO_IP_IDX];

    /* Get the cp config and web id based on preferred language */
    memset(link,0,sizeof(link));
    memset(lang,0,sizeof(lang));
    memset(protocol,0,sizeof(protocol));
    if ( (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intf,&cpId)) ||
         (L7_SUCCESS != usmDbCpdmCPConfigProtocolModeGet(cpId,&protocolMode)))
    {
      return NULL;
    }
    getProtocolText(protocolMode,protocol);
    osapiStrncpy(lang,appInfo.lang,USMWEB_BUFFER_SIZE_64);
    getLangWebId(cpId,lang,&fWebId);
    if (L7_SUCCESS != usmDbCpdmCPConfigWebLocaleLinkGet(cpId,fWebId,link))
    {
      return NULL;
    }

    /* Output first (non-href) lang */
    osapiStrncpy(buf,LINK_HEADER,sizeof(LINK_HEADER));
    usmWebConvertHexToUnicode(link,tmp);
    osapiSnprintfcat(buf,sizeof(buf)-strlen(buf),"%s",tmp);

    /* Loop through the rest of the web id's */
    if (L7_SUCCESS != usmDbCpdmCPConfigWebIdNextGet(cpId,0,&webId))
    {
      return NULL;
    }
    if (fWebId != webId)
    {
      memset(link,0,sizeof(link));
      memset(lang,0,sizeof(lang));
      if (L7_SUCCESS != usmDbCpdmCPConfigWebLangCodeGet(cpId,webId,lang))
      {
        return NULL;
      }
      if (L7_SUCCESS != usmDbCpdmCPConfigWebLocaleLinkGet(cpId,webId,link))
      {
        return NULL;
      }
      if ((0!=strlen(lang)) && (0!=strlen(link)))
      {
        usmWebConvertHexToUnicode(link,tmp);
        osapiSnprintfcat(buf,sizeof(buf)-strlen(buf),
                         CP_LINK_ENTRY,protocol,host,CP_SERVE,intf,ip,lang,tmp);
        fMultiLang = L7_TRUE;
      }
    }
    while (usmDbCpdmCPConfigWebIdNextGet(cpId,webId,&nextWebId)==L7_SUCCESS)
    {
      if (fWebId != nextWebId)
      {
        memset(lang,0,sizeof(lang));
        memset(link,0,sizeof(link));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLangCodeGet(cpId,nextWebId,lang))
        {
          return NULL;
        }
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLocaleLinkGet(cpId,nextWebId,link))
        {
          return NULL;
        }
        if ((0!=strlen(lang)) && (0!=strlen(link)))
        {
          usmWebConvertHexToUnicode(link,tmp);
          osapiSnprintfcat(buf,sizeof(buf)-strlen(buf),
                           CP_LINK_ENTRY,protocol,host,CP_SERVE,intf,ip,lang,tmp);
          fMultiLang = L7_TRUE;
        }
      }
      webId = nextWebId;
    }
    osapiStrncat(buf,LINK_FOOTER,sizeof(buf)-strlen(buf));
    return (fMultiLang)?buf:L7_NULL;
  }

  return L7_NULL;
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
L7_char8 *usmWebCPGenericGet(EwsContext context, usmWeb_cpGenericGetId_t id)
{
  EwaNetHandle net = ewsContextNetHandle(context);
  cpAppInfo_t appInfo;
  CP_AUTH_STATUS_FLAG_t flag;
  cpId_t cpId = 0;
  webId_t webId = 0;
  L7_uint32 intf;
  L7_char8 imageName[L7_MAX_FILENAME];
  L7_char8 host[CP_HOST_STR_SIZE];
  L7_char8 lang[USMWEB_BUFFER_SIZE_64];
  L7_char8 uid[L7_ENCRYPTED_PASSWORD_SIZE];
  L7_char8 pwd[L7_ENCRYPTED_PASSWORD_SIZE];
  if (net->app_pointer != L7_NULL)      
  {
    memset(host,0,sizeof(host));
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    memset(tmp, 0, sizeof(tmp));
    memset(buf, 0, sizeof(buf));
    memset(lang,0,sizeof(lang));
    flag = (CP_AUTH_STATUS_FLAG_t) appInfo.data[CP_APPINFO_FLAG_IDX];
    intf = (L7_uint32) appInfo.data[CP_APPINFO_INTF_IDX];
    osapiStrncpy(lang,appInfo.lang,USMWEB_BUFFER_SIZE_64);
    ewsContextHost(context,host,sizeof(host));

    if (CP_PREVIEW == flag)
    {
      cpId  = (cpId_t) appInfo.data[CP_APPINFO_INTF_IDX];
      webId = (webId_t) appInfo.data[CP_APPINFO_IP_IDX];
    }
    else
    {
      if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intf, &cpId))
      {
        return NULL;
      }
      getLangWebId(cpId,lang,&webId);   
    }

    switch (id)
    {
      case CP_GET_ID_ACCOUNT_IMAGE:
      {
        memset(imageName,0,sizeof(imageName));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebAccountImageNameGet(cpId,webId,imageName))
        {
          return NULL;
        }
        if (strlen(imageName)>0)
        {
          if ((strcmp(imageName,WEB_DEF1_BRANDING_IMAGE_NAME)==0) ||
              (strcmp(imageName,WEB_DEF1_ACCOUNT_IMAGE_NAME)==0)  ||
              (strcmp(imageName,WEB_DEF1_BACKGROUND_IMAGE_NAME)==0))
          {
            osapiSnprintf(buf,sizeof(buf),
                          "<img src='%s/%s' alt='' height='55' width='310'>",
                          CP_URL_BASE_IMAGE_PATH,imageName);
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf),
                          "<img src='%s/%s' alt='' height='55' width='310'>",
                          CP_URL_FILE_SYSTEM_PATH,imageName);
          }
        }
        break;
      }
      case CP_GET_ID_BACKGROUND_IMAGE:
      {
        memset(imageName,0,sizeof(imageName));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebBackgroundImageNameGet(cpId,webId,imageName))
        {
          return NULL;
        }
        if (strlen(imageName)>0)
        {
          if ((strcmp(imageName,WEB_DEF1_BRANDING_IMAGE_NAME)==0) ||
              (strcmp(imageName,WEB_DEF1_ACCOUNT_IMAGE_NAME)==0)  ||
              (strcmp(imageName,WEB_DEF1_BACKGROUND_IMAGE_NAME)==0))
          {
            osapiSnprintf(buf,sizeof(buf),
                          "url('%s/%s')",
                          CP_URL_BASE_IMAGE_PATH,imageName);
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf),
                          "url('%s/%s')",
                          CP_URL_FILE_SYSTEM_PATH,imageName);
          }
        }
        break;
      }
      case CP_GET_ID_BRANDING_IMAGE:
      {
        memset(imageName,0,sizeof(imageName));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebBrandingImageNameGet(cpId,webId,imageName))
        {
          return NULL;
        }
        if (strlen(imageName)>0)
        {
          if ((strcmp(imageName,WEB_DEF1_BRANDING_IMAGE_NAME)==0) ||
              (strcmp(imageName,WEB_DEF1_ACCOUNT_IMAGE_NAME)==0)  ||
              (strcmp(imageName,WEB_DEF1_BACKGROUND_IMAGE_NAME)==0))
          {
            osapiSnprintf(buf,sizeof(buf),
                          "<img src='%s/%s' alt=''>",
                          CP_URL_BASE_IMAGE_PATH,imageName);
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf),
                          "<img src='%s/%s' alt=''>",
                          CP_URL_FILE_SYSTEM_PATH,imageName);
          }
        }
        break;
      }
      case CP_GET_ID_LOGOUT_SUCCESS_BACKGROUND_IMAGE:
      {
        memset(imageName,0,sizeof(imageName));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutSuccessBackgroundImageNameGet(cpId,webId,imageName))
        {
          return NULL;
        }
        if (strlen(imageName)>0)
        {
          if ((strcmp(imageName,WEB_DEF1_BRANDING_IMAGE_NAME)==0) ||
              (strcmp(imageName,WEB_DEF1_ACCOUNT_IMAGE_NAME)==0)  ||
              (strcmp(imageName,WEB_DEF1_BACKGROUND_IMAGE_NAME)==0))
          {
            osapiSnprintf(buf,sizeof(buf),
                          "url('%s/%s')",
                          CP_URL_BASE_IMAGE_PATH,imageName);
          }
          else
          {
            osapiSnprintf(buf,sizeof(buf),
                          "url('%s/%s')",
                          CP_URL_FILE_SYSTEM_PATH,imageName);
          }
        }
        break;
      }
      case CP_GET_ID_FONT_LIST:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebFontListGet(cpId,webId,buf))
        {
          return NULL;
        }
        break;
      }
      case CP_GET_ID_SEPARATOR_COLOR:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigSeparatorColorGet(cpId,buf))
        {
          return NULL;
        }
        break;
      }
      case CP_GET_ID_FOREGROUND_COLOR:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigForegroundColorGet(cpId,buf))
        {
          return NULL;
        }
        break;
      }
      case CP_GET_ID_BACKGROUND_COLOR:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigBackgroundColorGet(cpId,buf))
        {
          return NULL;
        }
        break;
      }
      case CP_GET_ID_ACCEPT:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebAcceptTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_ACCOUNT:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebAccountLabelGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_BTITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebBrowserTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
       case CP_GET_ID_TITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_USER:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebUserLabelGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_PASSWORD:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebPasswordLabelGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_INSTRUCT:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebInstructionalTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_MSG:
      {
        if (L7_SUCCESS != getMsgText(cpId,webId,flag,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_WTITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebWelcomeTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_WELCOME:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebWelcomeTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_REFRESH:
      {
        L7_short16 timeout = CP_BROWSER_REFRESH;
        L7_IP_ADDR_t ip;
        L7_LOGIN_TYPE_t protocolMode = L7_LOGIN_TYPE_UNKNWN;
        L7_char8 protocol[CP_PROTOCOL_STR_SIZE];
        char host[CP_HOST_STR_SIZE];
        if ((CP_WIP==flag) || (CP_RADIUS_WIP==flag))
        {
          memset(uid,0,sizeof(uid));
          memset(pwd,0,sizeof(pwd));
          memset(host,0,sizeof(host));
          pwEncrypt(appInfo.uid,uid,L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
          pwEncrypt(appInfo.pwd,pwd,L7_PASSWORD_SIZE-1, L7_PASSWORD_ENCRYPT_ALG);
          ip = (L7_IP_ADDR_t) appInfo.data[CP_APPINFO_IP_IDX];
          ewsContextHost(context,host,sizeof(host));
          if (L7_SUCCESS != usmDbCpdmCPConfigProtocolModeGet(cpId,&protocolMode))
          {
            return NULL;
          }
          getProtocolText(protocolMode,protocol);
          osapiSnprintf(buf,sizeof(buf),CP_REFRESH_META,
            timeout,protocol,host,flag,intf,ip,lang,uid,pwd);
        }
        break;
      }
      case CP_GET_ID_SCRIPT:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebScriptTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_POPUP:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebPopupTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_LOGOUT_URL:
      {
        L7_IP_ADDR_t ip;
        L7_LOGIN_TYPE_t protocolMode = L7_LOGIN_TYPE_UNKNWN;
        L7_char8 protocol[CP_PROTOCOL_STR_SIZE];
        char host[CP_HOST_STR_SIZE];

        if (L7_SUCCESS != usmDbCpdmCPConfigProtocolModeGet(cpId,&protocolMode))
        {
          return NULL;
        }
        memset(host,0,sizeof(host));
        ewsContextHost(context,host,sizeof(host));
        getProtocolText(protocolMode,protocol);
        ip = (L7_IP_ADDR_t) appInfo.data[CP_APPINFO_IP_IDX];
        osapiSnprintf(tmp,sizeof(tmp),CP_LOGOUT_URL,protocol,host,flag,intf,ip,lang);
        osapiSnprintf(buf,sizeof(buf),CP_LOGOUT_POPUP_PARAMS,tmp);
        break;
      }
      case CP_GET_ID_LOGOUT_BTITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutBrowserTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_LOGOUT_TITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_LOGOUT_CONTENT:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutContentTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_LOGOUT_BUTTON:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutButtonLabelGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_WELCOME_URL:
      {
        L7_IP_ADDR_t ip;
        L7_uchar8 redirectMode;
        L7_LOGIN_TYPE_t protocolMode = L7_LOGIN_TYPE_UNKNWN;
        L7_char8 protocol[CP_PROTOCOL_STR_SIZE];
        char host[CP_HOST_STR_SIZE];

        if (L7_SUCCESS != usmDbCpdmCPConfigRedirectModeGet(cpId,&redirectMode))
        {
          return NULL;
        }
        osapiStrncpySafe(tmp,getAuthSuccessPath(cpId),sizeof(tmp));
        if (L7_ENABLE==redirectMode)
        {
          osapiSnprintf(buf,sizeof(buf),CP_WELCOME_PARAMS,tmp);
        }
        else
        {
          if (L7_SUCCESS != usmDbCpdmCPConfigProtocolModeGet(cpId,&protocolMode))
          {
            return NULL;
          }
          memset(host,0,sizeof(host));
          ewsContextHost(context,host,sizeof(host));
          getProtocolText(protocolMode,protocol);
          ip = (L7_IP_ADDR_t) appInfo.data[CP_APPINFO_IP_IDX];
          osapiSnprintf(buf,sizeof(buf),CP_WELCOME_URL,protocol,host,tmp,flag,intf,ip,lang);
        }
        break;
      }
      case CP_GET_ID_LOGOUT_CONFIRM:
      {
        L7_char8 confirm[CP_LOGOUT_CONFIRM_TEXT_MAX+1];
        memset(confirm, 0, sizeof(confirm));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutConfirmTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,confirm);
        osapiSnprintf(buf,sizeof(buf),"\"%s\"",confirm);
        break;
      }
      case CP_GET_ID_LOGOUT_SUCCESS_URL: /* may not need this... */
      {
        break;
      }
      case CP_GET_ID_LOGOUT_SUCCESS_BTITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutSuccessBrowserTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_LOGOUT_SUCCESS_TITLE:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutSuccessTitleTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      case CP_GET_ID_LOGOUT_SUCCESS_CONTENT:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigWebLogoutSuccessContentTextGet(cpId,webId,tmp))
        {
          return NULL;
        }
        usmWebConvertHexToUnicode(tmp,buf);
        break;
      }
      default: /* usmWeb_cpGenericGetId_t is shared */
      {
        break;
      }
    }
    if (strlen(buf)>0)
    {
      return buf;
    }
  }

  return L7_NULL;
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
L7_BOOL usmWebCPFormObjectEnabled(EwsContext context, usmWeb_cpFormObjectId_t id)
{
  EwaNetHandle net = ewsContextNetHandle(context);
  cpAppInfo_t appInfo;
  L7_char8 lang[USMWEB_BUFFER_SIZE_64];
  L7_char8 userLabel[CP_USER_LABEL_MAX+1];
  L7_char8 aupText[CP_AUP_TEXT_MAX+1];
  L7_char8 acceptText[CP_ACCEPT_TEXT_MAX+1]; 
  CP_VERIFY_MODE_t verifyMode;
  CP_AUTH_STATUS_FLAG_t flag;
  cpId_t cpId = 0;
  webId_t webId = 0;
  L7_uint32 intf;

  if (net->app_pointer != L7_NULL)      
  {
    memset(lang,0,sizeof(lang));
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    intf = (L7_uint32) appInfo.data[CP_APPINFO_INTF_IDX];
    flag = (CP_AUTH_STATUS_FLAG_t) appInfo.data[CP_APPINFO_FLAG_IDX];
    osapiStrncpy(lang,appInfo.lang,USMWEB_BUFFER_SIZE_64);

    if (CP_PREVIEW == flag)
    {
      cpId  = (cpId_t) appInfo.data[CP_APPINFO_INTF_IDX];
      webId = (webId_t) appInfo.data[CP_APPINFO_IP_IDX];
    }
    else
    {
      if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intf,&cpId))
      {
        return L7_FALSE;
      }
      getLangWebId(cpId,lang,&webId);
    }

    switch (id)
    {
      case CP_FORM_OBJ_UID:
      {
        memset(userLabel,0,sizeof(userLabel));
        if (L7_SUCCESS != usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode))
        {
          return L7_FALSE;
        }
        if (L7_SUCCESS != usmDbCpdmCPConfigWebUserLabelGet(cpId,webId,userLabel))
        {
          return L7_FALSE;
        }
        return ((verifyMode==CP_VERIFY_MODE_LOCAL) ||
                (verifyMode==CP_VERIFY_MODE_RADIUS) ||
                (strlen(userLabel)>0))?L7_TRUE:L7_FALSE;
      }
      case CP_FORM_OBJ_PWD:
      {
        if (L7_SUCCESS != usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode))
        {
          return L7_FALSE;
        }
        return ((verifyMode==CP_VERIFY_MODE_LOCAL) ||
                (verifyMode==CP_VERIFY_MODE_RADIUS))?L7_TRUE:L7_FALSE;
      }
      case CP_FORM_OBJ_MSG:
      {
        return ((flag!=CP_VALIDATE) && (flag!=CP_SERVE))?L7_TRUE:L7_FALSE;
      }
      case CP_FORM_OBJ_AUP:
      {
        memset(aupText,0,sizeof(aupText));
        memset(acceptText,0,sizeof(acceptText));
        if (L7_SUCCESS != usmDbCpdmCPConfigWebAUPTextGet(cpId,webId,aupText))
        {
          return L7_FALSE;
        }
        if (L7_SUCCESS != usmDbCpdmCPConfigWebAcceptTextGet(cpId,webId,acceptText))
        {
          return L7_FALSE;
        }
        return ((strlen(aupText)>0) && (strlen(acceptText)>0))
                ?L7_TRUE:L7_FALSE;
      }
      case CP_FORM_OBJ_USER_LOGOUT:
      {
        L7_uchar8 val;
        if (usmDbCpdmCPConfigUserLogoutModeGet(cpId, &val) == L7_SUCCESS)
        {
          return (val==L7_ENABLE)?L7_TRUE:L7_FALSE;
        }
        break;
      }
    }
  }

  return L7_FALSE;
}

/*****************************************************************************
 ****************************************************************************/
void ewaFormServe_captive_portal ( EwsContext context, EwaForm_captive_portalP form )
{
  EwaNetHandle net;
  cpAppInfo_t appInfo;
  L7_char8 lang[USMWEB_BUFFER_SIZE_64];     /* from user or browser accept */
  L7_char8 tmp[CP_AUP_TEXT_MAX+1];          /* max UTF-16 format */
  L7_char8 buf[CP_HEX_NCR_MAX+1];           /* max Hexadecimal NCR */
  L7_char8 uid[L7_PASSWORD_SIZE];
  L7_char8 pwd[L7_PASSWORD_SIZE];
  CP_AUTH_STATUS_FLAG_t flag;
  cpId_t cpId = 0;
  webId_t webId = 0;
  L7_IP_ADDR_t ip;
  L7_uint32 intf;

  net = ewsContextNetHandle(context);
  if (net->app_pointer != NULL)
  {
    memset(uid,0,sizeof(uid));
    memset(pwd,0,sizeof(pwd));
    memset(lang,0,sizeof(lang));
    memset(&appInfo,0,sizeof(cpAppInfo_t));
    memcpy(&appInfo,net->app_pointer,sizeof(cpAppInfo_t));
    flag = (CP_AUTH_STATUS_FLAG_t) appInfo.data[CP_APPINFO_FLAG_IDX];
    intf = (L7_uint32) appInfo.data[CP_APPINFO_INTF_IDX];
    ip   = (L7_IP_ADDR_t) appInfo.data[CP_APPINFO_IP_IDX];
    osapiStrncpy(lang,appInfo.lang,USMWEB_BUFFER_SIZE_64);
    osapiStrncpy(uid,appInfo.uid,L7_PASSWORD_SIZE-1);
    osapiStrncpy(pwd,appInfo.pwd,L7_PASSWORD_SIZE-1);
  }
  else
  {
    return; /* will always have a context */
  }

  if (CP_PREVIEW == flag)
  {
    cpId  = (cpId_t) appInfo.data[CP_APPINFO_INTF_IDX];
    webId = (webId_t) appInfo.data[CP_APPINFO_IP_IDX];
  }
  else
  {
    /* Get associated configuration and web id */
    if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intf,&cpId))
    {
      return; /* Can't go on.. */
    }
    getLangWebId(cpId,lang,&webId);
  }

  /* AUP text */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebAUPTextGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.aupText = osapiStrDup(buf);
    form->status.aupText |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  if ((CP_WIP==flag) || (CP_RADIUS_WIP==flag))
  {
    form->value.aupCheck = L7_TRUE;
  }

  /* button label */
  memset(tmp, 0, sizeof(tmp));
  memset(buf, 0, sizeof(buf));
  if (L7_SUCCESS == usmDbCpdmCPConfigWebButtonLabelGet(cpId,webId,tmp))
  {
    usmWebConvertHexToUnicode(tmp,buf);
    form->value.Submit = osapiStrDup(buf);
    form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  if (CP_DENIED==flag)
  {
    memset(appInfo.pwd,0,sizeof(appInfo.pwd));
    osapiStrncpy(pwd,appInfo.pwd,USMWEB_BUFFER_SIZE_64);
  }

  if (CP_SERVE==flag)
  {
    flag = CP_VALIDATE;
  }

  /* Initialize uid and pwd */
  form->value.p5 = (char *) osapiStrDup(uid);
  form->value.p5_maxlength = 128;
  form->value.p6 = (char *) osapiStrDup(pwd);
  form->value.p6_maxlength = 128;
  form->status.p5 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p6 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.aupCheck |= (EW_FORM_INITIALIZED);

  /* Save hidden parms */
  form->value.p1 = flag;
  form->value.p2 = intf;
  form->value.p3 = ip;
  form->value.p4 = (char *) osapiStrDup(lang);
  form->status.p1 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p2 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p3 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  form->status.p4 |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
}

/******************************************************************************
*
* @purpose  Submit is used to authenticate a captive portal user. The parameters
*           are received via standard form components and the URI. The URI is
*           used during a browser meta content refresh. During this WIP refresh,
*           the uid and pwd parameters are encrypted. Although low-tech, the
*           browser refresh was selected in an effort to minimize browser
*           requirements (no scripting, no cookies, etc.). There *may* be URL
*           size restrictions for older browsers. We'll need to keep a watch out
*           for this.
*
* @param    EwsContext context @b{(input)} web context for param extraction
* @param    EwaForm_captive_portalP form @b{(input)} form object
*
* @returns  char *url serve redirect
*
* @end
*
******************************************************************************/
char *ewaFormSubmit_captive_portal ( EwsContext context, EwaForm_captive_portalP form )
{
  EwaNetHandle net;
  cpAppInfo_t appInfo;
  L7_char8 prefLang[USMWEB_BUFFER_SIZE_64];
  L7_char8 uid[L7_PASSWORD_SIZE];
  L7_char8 pwd[L7_PASSWORD_SIZE];
  CP_VERIFY_MODE_t verifyMode;
  CP_AUTH_STATUS_FLAG_t flag;
  cpId_t cpId, webId;
  cpUserAuth_t user;

  memset(pwd,0,sizeof(pwd));
  memset(uid,0,sizeof(uid));
  memset(&user,0,sizeof(cpUserAuth_t));

  memset(&appInfo,0,sizeof(cpAppInfo_t));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(cpAppInfo_t));

  /* Copy params from the form */
  appInfo.data[CP_APPINFO_FLAG_IDX] = (L7_uint32) form->value.p1;
  appInfo.data[CP_APPINFO_INTF_IDX] = (L7_uint32) form->value.p2;
  appInfo.data[CP_APPINFO_IP_IDX]   = (L7_uint32) form->value.p3;

  /* Special administrator preview command */
  if (CP_PREVIEW == form->value.p1)
  {
    memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));
    return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
  }

  if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(form->value.p2,&cpId))
  {
    memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));
    return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
  }

  /* Get and save the default language */
  if (NULL!=form->value.p4)
  {
    osapiStrncpy(appInfo.lang,form->value.p4,USMWEB_BUFFER_SIZE_64);
  }
  else
  {
    memset(prefLang,0,sizeof(prefLang));
    getPrefLang(context,cpId,appInfo.lang,prefLang);
    osapiStrncpy(appInfo.lang,prefLang,USMWEB_BUFFER_SIZE_64);
  }

  if (CP_SERVE==form->value.p1)
  {
    memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));
    return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
  }

  /* Fudge the aup check if admin didn't configure acceptance */
  memcpy(net->app_pointer,&appInfo,sizeof(cpAppInfo_t));
  if ((CP_WIP==form->value.p1) ||
      (CP_RADIUS_WIP==form->value.p1) ||
      (L7_TRUE!=usmWebCPFormObjectEnabled(context,CP_FORM_OBJ_AUP)))
  {
    form->value.aupCheck = L7_TRUE;
  }

  if (L7_TRUE!=form->value.aupCheck)
  {
    appInfo.data[CP_APPINFO_FLAG_IDX] = (L7_uint32) CP_NOACCEPT;
    memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));
    return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
  }

  if ((CP_WIP==form->value.p1) ||
      (CP_RADIUS_WIP==form->value.p1)) /* Get UID & PWD from the form and decrypt */
  {
    pwDecrypt(uid,form->value.p5,L7_PASSWORD_SIZE-1);
    pwDecrypt(pwd,form->value.p6,L7_PASSWORD_SIZE-1);
    osapiStrncpy(appInfo.uid,uid,L7_PASSWORD_SIZE-1);
    osapiStrncpy(appInfo.pwd,pwd,L7_PASSWORD_SIZE-1);
    form->value.p5 = (char *) osapiStrDup(uid);
    form->value.p6 = (char *) osapiStrDup(pwd);
    form->value.aupCheck = L7_TRUE;
  }
  else /* Get UID & PWD from the form */
  {
    if (L7_NULL!=form->value.p5)
    {
      osapiStrncpy(appInfo.uid,form->value.p5,L7_PASSWORD_SIZE-1);
    }
    if (L7_NULL!=form->value.p6)
    {
      osapiStrncpy(appInfo.pwd,form->value.p6,L7_PASSWORD_SIZE-1);
    }
  }

  getLangWebId(cpId,appInfo.lang,&webId);
  if (L7_SUCCESS != usmDbCpdmCPConfigVerifyModeGet(cpId,&verifyMode))
  {
    return NULL;
  }

  /* Perform some preliminary/obvious checks before requesting authentication */
  switch (verifyMode)
  {
    case CP_VERIFY_MODE_GUEST:
    {
      if (L7_TRUE==usmWebCPFormObjectEnabled(context,CP_FORM_OBJ_UID))
      {
        /* expect a user id */
        if (strlen(appInfo.uid)==0)
        {
          appInfo.data[CP_APPINFO_FLAG_IDX] = (L7_uint32) CP_DENIED;
          return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
        }
      }
      else
      {
        strcpy(appInfo.uid,CP_DEF_NO_GUEST_NAME);
      }
      break;
    }
    case CP_VERIFY_MODE_LOCAL:
    case CP_VERIFY_MODE_RADIUS:
    {
      if ((strlen(appInfo.uid)==0) || (strlen(appInfo.pwd)==0))
      {
        appInfo.data[CP_APPINFO_FLAG_IDX] = (L7_uint32) CP_DENIED;
        memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));
        return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
      }
      break;
    }
  }

  /* Made it through preliminary validation, time to authenticate */
  user.ipAddr = form->value.p3;
  user.flag = ((CP_WIP==form->value.p1)||(CP_RADIUS_WIP==form->value.p1))?CP_WIP:CP_VALIDATE;
  osapiStrncpy(user.uid,appInfo.uid,L7_PASSWORD_SIZE-1);
  osapiStrncpy(user.pwd,appInfo.pwd,L7_PASSWORD_SIZE-1);
  if (L7_SUCCESS != usmDbCpcmUserAuthRequest(&user,&flag))
  {
    return NULL;
  }
  appInfo.data[CP_APPINFO_FLAG_IDX] = (L7_uint32) flag;

  if (CP_SUCCESS!=flag)
  {
    memcpy(net->app_pointer, &appInfo, sizeof(cpAppInfo_t));
    return sendReply(context,L7_FALSE,&appInfo,CP_URL_PATH);
  }

  /* user authentication success */
  ewsContextSendReply(context,CP_URL_DISPATCH_PATH);
  return NULL;
}




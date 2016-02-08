/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2009
 *
 **********************************************************************
 *
 * @filename captive_portal_util.c 
 *
 * @purpose
 *
 * @component captive portal handlers
 *
 * @comments
 *
 * @create  3 Mar  2009
 *
 * @author  Vamshi krishna Madap
 * @end
 *
 **********************************************************************/

#include "emweb_if.h"
#include "web.h"
#include "cli_web_exports.h"
#include "commdefs.h"
#include "usmdb_cpdm_api.h"
#include "captive_portal_commdefs.h"
#include "log.h"
#include "sslt_exports.h"
#include "usmdb_cpcm_api.h"
#include "captive_portal_defaultconfig.h"
#include "ews_send.h"
#include "captive_portal_util.h"

extern L7_RC_t ewaCpConnListAnd(L7_inet_addr_t remote);

L7_RC_t ewaUrlCpHook(void* context, char *url,char* dsturl)
{
  CP_AUTH_STATUS_FLAG_t flag = CP_SERVE;
  L7_LOGIN_TYPE_t protocolMode = L7_LOGIN_TYPE_UNKNWN;
  L7_char8 redirectUrl[512];
  L7_char8 localName[LOCAL_NAME_MAX];
  L7_uint32 mode = L7_DISABLE;
  L7_char8 protocol[6];
  L7_inet_addr_t remote;
  const char *net_loc;
  L7_uint32 listeningPort = L7_SSLT_SECURE_PORT;
  L7_uint32 intfId = 0;
  cpId_t cpId = 0;

    /* Make sure CP is enabled before we mess with the stream */
  if (usmDbCpdmGlobalModeGet(&mode) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  if (L7_ENABLE == mode)
  {
    /* Get the client IP address */
    memset(&remote,0,sizeof(L7_inet_addr_t));
    emwebContextRemoteIpAddressGet(context,&remote);
    emwebContextListenPortGet(context,&listeningPort);
	  
    /* Check if client is a captive portal candidate. */
    if (L7_SUCCESS==usmDbCpcmAIPStatusIntfIdGet(remote.addr.ipv4.s_addr,&intfId))
    {
      if ((strstr(url,CP_URL_PATH)!=NULL) ||             /* core CP URLs */
#ifndef L7_XWEB_PACKAGE
          (strstr(url,"/base/")!=NULL) ||                /* base images and style sheets */
#else
          (strstr(url,"/images/")!=NULL) ||                /* base images and style sheets */
#endif
         (strstr(url,CP_URL_FILE_SYSTEM_PATH)!=NULL))   /* user defined images */
     {
        /* Allow CP related requests through */
     }
     else /* All others requests trigger a CP redirect */
     {

      /* Get the associated CP configuration */
      if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intfId,&cpId))
      {
        dsturl = CP_URL_ERROR_PATH;
        return L7_FAILURE;
      }

     /* Check if we can continue to authenticate */
     if (L7_SUCCESS!=ewaCpConnListAnd(remote))
     {
       flag = CP_RESOURCE;
       /* TBD: write message to activity log */
     }

     /* Get protocol */
     (void)usmDbCpdmCPConfigProtocolModeGet(cpId,&protocolMode);
     if (protocolMode==L7_LOGIN_TYPE_HTTPS)
     {
#ifdef L7_MGMT_SECURITY_PACKAGE
       L7_uint32 defaultSecurePort;
       L7_uint32 additionalSecurePort;
       if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort1Get(&defaultSecurePort))
       {
         defaultSecurePort = L7_SSLT_SECURE_PORT;
       }
       if (L7_SUCCESS != usmDbCpdmGlobalHTTPSecurePort2Get(&additionalSecurePort))
       {
         additionalSecurePort = FD_CP_HTTP_AUX_SECURE_PORT2;
       }            
       if (L7_SUCCESS!=usmDbCpcmAIPStatusPortGet(remote.addr.ipv4.s_addr,&listeningPort))
       {
         listeningPort = L7_SSLT_SECURE_PORT;
       }
       if ((listeningPort!=defaultSecurePort) &&
          (listeningPort!=additionalSecurePort))
       {
         listeningPort = defaultSecurePort;
       }
#endif
       strcpy(protocol,HTTPS_STR);
     }
     else
     {
       strcpy(protocol,HTTP_STR);
     }

    /* Replace 0 with real default */
     if (0==listeningPort)
     {
       listeningPort = (protocolMode==L7_LOGIN_TYPE_HTTPS)?L7_SSLT_SECURE_PORT:CP_STANDARD_HTTP_PORT;
     }

    /* Get host */
     memset(localName,0,sizeof(localName));
     net_loc = ewaNetLocalHostName(context);
     osapiSnprintfcat(localName,sizeof(localName),"%s:%d",net_loc,listeningPort);

     /* Construct CP redirect */
     osapiSnprintf(redirectUrl,sizeof(redirectUrl),
                   CP_SERVE_URL,protocol,localName,flag,intfId,remote.addr.ipv4.s_addr);

     L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,"%s - redirectUrl:%s",__FUNCTION__,redirectUrl);

     ewsSendRedirect(context,redirectUrl,ewsRedirectTemporary);
     dsturl = NULL;
     return L7_FAILURE;
   }
  }
 }
 return L7_SUCCESS;
}

xLibRC_t cpGetNextDefaultImage(char* current,char* next)
{
  int isSame;

  if(current == NULL || next == NULL)
  {
    return XLIBRC_FAILURE;
  }
  
  /*Return END OF TABLE if the current file name is not equal to WEB_DEF_BRANDING_IMAGE_NAME, WEB_DEF_ACCOUNT_IMAGE_NAME*/
  isSame = strncmp(current,WEB_DEF_BRANDING_IMAGE_NAME,strlen(WEB_DEF_BRANDING_IMAGE_NAME)) && 
           strncmp(current,WEB_DEF_ACCOUNT_IMAGE_NAME,strlen(WEB_DEF_ACCOUNT_IMAGE_NAME));
 
  if(isSame)
  {
    return XLIBRC_ENDOF_TABLE;
  }
 
  isSame = strncmp(current,WEB_DEF_BRANDING_IMAGE_NAME,strlen(WEB_DEF_BRANDING_IMAGE_NAME));
  if(!isSame) 
  {
    memset(next,0x0,sizeof(*next));
    strncpy(next,WEB_DEF_ACCOUNT_IMAGE_NAME,strlen(WEB_DEF_ACCOUNT_IMAGE_NAME));
    return XLIBRC_SUCCESS;       
  }

  if(strncmp(current,WEB_DEF_ACCOUNT_IMAGE_NAME,strlen(WEB_DEF_ACCOUNT_IMAGE_NAME))== 0)
  {
    return XLIBRC_ENDOF_TABLE;
  } 
  return XLIBRC_SUCCESS;
}

xLibRC_t cpGetNexFSImage(char* current,char* next)
{
  struct dirent *entry; 
  DIR *dir = NULL; 
  xLibU32_t begin = 0;
  xLibU32_t found = 0;
  char* ext;
  
  if ((dir = opendir(RAM_CP_PATH RAM_CP_NAME)) == NULL)   
  {
    return XLIBRC_FAILURE;   
  }
  
  while ((entry = readdir(dir)) != NULL)
  {
    if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0)
    {
      continue;
    }
    ext = strrchr(entry->d_name,'.');
    if ((NULL != ext) && (4 == strlen(ext)))
    {
      if ((strcmp(ext,".gif")==0) || (strcmp(ext,".GIF")==0) ||
          (strcmp(ext,".jpg")==0) || (strcmp(ext,".JPG")==0))
      {
        if(strncmp(current,WEB_DEF_ACCOUNT_IMAGE_NAME,strlen(WEB_DEF_ACCOUNT_IMAGE_NAME)) == 0)
        {
          found = 1;
          memset(next,0x0,sizeof(*next));
          strncpy(next,entry->d_name,strlen(entry->d_name));
          break;
        }
        else if(begin == 1)
        {
          found = 1;
          memset(next,0x0,sizeof(*next));
          strncpy(next,entry->d_name,strlen(entry->d_name));
          break;  
        }
        else if(strncmp(current,entry->d_name,strlen(entry->d_name)) == 0)
        {
          begin = 1;
        }
      }
    }
  }  
  
  closedir(dir);	 
  
  if(found == 0)
  {
    return XLIBRC_ENDOF_TABLE;
  }
  return XLIBRC_SUCCESS;
}

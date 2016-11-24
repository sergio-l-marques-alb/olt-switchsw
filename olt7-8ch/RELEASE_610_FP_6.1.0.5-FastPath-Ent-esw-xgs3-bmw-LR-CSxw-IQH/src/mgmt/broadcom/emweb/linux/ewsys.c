/*
 *
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 *
 * Notice to Users of this Software Product:
 *
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in
 * source and object code format embodies valuable intellectual property
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and
 * copyright laws of the United States and foreign countries, as well as
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 *
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 *
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 *
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 *
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb reference port to linux - system interfaces
 *
 */

#include "posix_config.h"
#include "osapi_support.h"
#include "ew_config.h"
#include "ews_api.h"
#include "ewnet.h"

#include "usmdb_dim_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_sslt_api.h"
#include "session.h"
#include "sslt_exports.h"

#ifdef L7_WIRELESS_PACKAGE
#include "wnv_api.h"
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#include "captive_portal_commdefs.h"
#include "usmdb_cpdm_api.h"
#include "usmdb_cpcm_api.h"
extern L7_RC_t ewaCpConnListAnd(L7_inet_addr_t remote);
#endif

#ifdef L7_ORIGINAL_VENDOR_CODE
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* WIN32 */
#endif
#if 1 /*JWL*/
#include <time.h>
#endif
#include "cliapi.h"

#ifndef EMWEB_CLEANUP_OVERRIDE

/*
 * Clean up any state in the net handle after each request
 */
void ewaNetHTTPCleanup( EwaNetHandle handle )
{
  handle->app_data = 0;
  if (handle->app_pointer != NULL) /* LVL7_P0006 */
  {                                /* LVL7_P0006 */
    ewaFree(handle->app_pointer);  /* LVL7_P0006 */
    handle->app_pointer = NULL;
  }                                /* LVL7_P0006 */
  handle->buffer[0] = '\000';

# ifdef EW_CONFIG_OPTION_FILE_GET
  if (handle->params)
    {
      ewaFree( handle->params );
      handle->params = NULL;
    }
# endif /* EW_CONFIG_OPTION_FILE_GET */

# ifdef EW_CONFIG_OPTION_SMTP
  {
    EwaSMTPHandle smtp;

    if (   NULL != handle->context
        && NULL != ( smtp = ewsContextSMTPHandle(handle->context) ) )
    {
      ewaFree( smtp );
    }
  }
# endif /* EW_CONFIG_OPTION_SMTP */

# ifdef EW_CONFIG_OPTION_CLIENT
  {
#  if 0 /* TBD -- this doesn't belong here and isn't available on WinCE */
    if (handle->tmpFile != NULL)
      {
        unlink(handle->tmpFile);
        handle->tmpFile = NULL;
      }
    if (handle->tmpFile2 != NULL)
      {
        unlink(handle->tmpFile2);
        handle->tmpFile2 = NULL;
      }
#  endif /* TBD */
    if (handle->clientHandle != NULL)
      {
        ewaFree(handle->clientHandle);
        handle->clientHandle = NULL;
      }
  }
# endif /* EW_CONFIG_OPTION_CLIENT */
# ifdef EW_CONFIG_OPTION_UPNP
  /* free up the udp_xmit_bufp */
  if (handle->udp_xmit_buffer)
    {
      ewaFree(handle->udp_xmit_buffer);
      handle->udp_xmit_buffer = NULL;
      handle->udp_xmit_buflen = 0;
    }
#endif /* EW_CONFIG_OPTION_UPNP */
}

#endif /* EMWEB_CLEANUP_OVERRIDE */

#ifdef EW_CONFIG_OPTION_DATE

static const char *month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const char *day[] = {
  "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *day1036[] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
   "Saturday"
};

/*
 * ewaDate
 * Return date string in RFC1123 format - MUST be in GMT
 * If your system does not have a clock that can give you
 * accurate time in GMT, then undef EW_CONFIG_OPTION_DATE
 * and EmWeb/Server will send responses that don't need dates.
 */
const char *
ewaDate2 ( time_t *tp, char *date )
{
  struct tm *tmp = gmtime(tp);

  sprintf( date
          , "%s, %02d %s %d %02d:%02d:%02d GMT"
          ,day[tmp->tm_wday]
          ,tmp->tm_mday
          ,month[tmp->tm_mon]
          ,(1900 + tmp->tm_year)
          ,tmp->tm_hour
          ,tmp->tm_min
          ,tmp->tm_sec
          );
  return date;
}

const char *
ewaDate ( void )
{
  static char date[32];
  time_t t = time(NULL);
  struct tm *tmp = gmtime(&t);

  sprintf( date
          , "%s, %02d %s %d %02d:%02d:%02d GMT"
          ,day[tmp->tm_wday]
          ,tmp->tm_mday
          ,month[tmp->tm_mon]
          ,(1900 + tmp->tm_year)
          ,tmp->tm_hour
          ,tmp->tm_min
          ,tmp->tm_sec
          );
  return date;
}

const char *
ewaDate1036(time_t *tp, char *date1036)
{
  struct tm *tmp = gmtime(tp);

  sprintf (date1036, "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
           day1036[tmp->tm_wday], tmp->tm_mday, month[tmp->tm_mon],
            tmp->tm_year % 100, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
  return date1036;
}


/*
 * ewaTime
 * Return number of seconds since 00:00:00 GMT 01-Jan-1970.  Currently
 * only used by EmWeb/Client with Cacheing.
 */
uint32
ewaTime ( void )
{
  return (uint32) time ( NULL );
}

#endif /* EW_CONFIG_OPTION_DATE */

#ifdef EWA_LOG_HOOK
/*
 * ewaLogHook
 * Application logging hook
 *
 * context - context of request
 * status  - request status
 */
void
ewaLogHook ( EwsContext context, EwsLogStatus status )
{
  EwaNetHandle net_handle;
  char string[256];
  char *sourceAddr;
  const char *s;
  uintf bytes;
  const char *url_name;

    /*
     * Convert status to string
     */
    switch(status)
    {
      case EWS_LOG_STATUS_OK: s="OK";break;
      case EWS_LOG_STATUS_MOVED_TEMPORARILY: s="MOVED_TEMPORARILY";break;
      case EWS_LOG_STATUS_MOVED_PERMANENTLY: s="MOVED_PERMANENTLY";break;
      case EWS_LOG_STATUS_SEE_OTHER: s="SEE_OTHER";break;
      case EWS_LOG_STATUS_NOT_MODIFIED: s="NOT_MODIFIED";break;
      case EWS_LOG_STATUS_NO_CONTENT: s="NO_CONTENT";break;
      case EWS_LOG_STATUS_AUTH_FAILED: s="AUTH_FAILED";break;
      case EWS_LOG_STATUS_AUTH_FORGERY: s="AUTH_FORGERY";break;
      case EWS_LOG_STATUS_AUTH_STALE: s="AUTH_STALE";break;
      case EWS_LOG_STATUS_AUTH_REQUIRED: s="AUTH_REQUIRED";break;
      case EWS_LOG_STATUS_AUTH_DIGEST_REQUIRED: s="AUTH_DIGEST_REQUIRED";break;
      case EWS_LOG_STATUS_BAD_REQUEST: s="BAD_REQUEST";break;
      case EWS_LOG_STATUS_BAD_FORM: s="BAD_FORM";break;
      case EWS_LOG_STATUS_BAD_IMAGEMAP: s="BAD_IMAGEMAP";break;
      case EWS_LOG_STATUS_NOT_FOUND: s="NOT_FOUND";break;
      case EWS_LOG_STATUS_UNAVAILABLE: s="UNAVAILABLE";break;
      case EWS_LOG_STATUS_NOT_IMPLEMENTED: s="NOT_IMPLEMENTED";break;
      case EWS_LOG_STATUS_NO_RESOURCES: s="NO_RESOURCES";break;
      case EWS_LOG_STATUS_INTERNAL_ERROR: s="INTERNAL_ERROR";break;
      case EWS_LOG_STATUS_METHOD_NOT_ALLOWED: s="METHOD_NOT_ALLOWED";break;
      case EWS_LOG_STATUS_LENGTH_REQUIRED: s="LENGTH_REQUIRED";break;
      case EWS_LOG_STATUS_PRECONDITION_FAILED: s="PRECONDITION_FAILED";break;
      default:s="?";break;
    }

    /*
     * If disposition code, we already printed a status line.  So, just print
     * the disposition and return.
     */
    switch(status)
    {
      case EWS_LOG_STATUS_MOVED_TEMPORARILY:
      case EWS_LOG_STATUS_MOVED_PERMANENTLY:
      case EWS_LOG_STATUS_NOT_MODIFIED:
      case EWS_LOG_STATUS_NO_CONTENT:
      case EWS_LOG_STATUS_SEE_OTHER:
        printf("\tDisposition: %s\n", s);

#ifndef EMWEB_OSE
        fflush(stdout);
#endif
        return;
      default:
        break;
    }

    /*
     * Print status line
     */
#   ifdef EW_CONFIG_OPTION_DATE
    printf("%s ", ewaDate());
#   else
    printf("LOG ");
#   endif

    net_handle = ewsContextNetHandle(context);

    url_name = ewsCGIScriptName(context);
    if (url_name == NULL)
      {
        url_name = "NULL";
      }

    if (NULL != net_handle->sourceAddrStr &&
        '\0' != net_handle->sourceAddrStr[0])
      {
        /* A useable net address has been stored, so use it. */
        sourceAddr = net_handle->sourceAddrStr;
      }
    else
      {
#ifdef EW_CONFIG_OPTION_UPNP
        /* Have to build our own net address string. */
        sourceAddr = inet_ntoa(net_handle->fromAddr.sin_addr);
#else /* EW_CONFIG_OPTION_UPNP */
        /* No way to determine source address. */
        sourceAddr = (char *) "[unknown]";
#endif /* EW_CONFIG_OPTION_UPNP */
      }

    printf("%s %s %s\n",
           sourceAddr
#          ifdef EW_CONFIG_OPTION_CGI_SCRIPT_NAME
           ,url_name
#          else /* EW_CONFIG_OPTION_CGI_SCRIPT_NAME */
           ,""
#          endif /* EW_CONFIG_OPTION_CGI_SCRIPT_NAME */
           ,s
           );

#   ifdef EW_CONFIG_OPTION_CGI
    printf("\t");
#   ifdef EW_CONFIG_OPTION_CGI_SERVER_PROTOCOL
    bytes = ewsCGIServerProtocol(context, string, sizeof(string));
    if (bytes != 0) printf("Protocol = %s ", string);
#   endif
#   ifdef EW_CONFIG_OPTION_CGI_REQUEST_METHOD
    bytes = ewsCGIRequestMethod(context, string, sizeof(string));
    if (bytes != 0) printf("Method = %s ", string);
#   endif
#   if defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
    || defined( EW_CONFIG_OPTION_URL_PATH_INFO )
    s = ewsCGIPathInfo(context);
    if (s != NULL) printf("Info = %s ", s);
#   endif
#   ifdef EW_CONFIG_OPTION_CGI_QUERY_STRING
    bytes = ewsCGIQueryString(context, string, sizeof(string));
    if (bytes != 0) printf("Query = %s ", string);
#   endif
    printf("\n");
#   ifdef EW_CONFIG_OPTION_CGI_CONTENT_ENCODING
    bytes = ewsCGIContentEncoding(context, string, sizeof(string));
    if (bytes != 0) printf("\tContent-Encoding = %s\n", string);
#   endif
#   ifdef EW_CONFIG_OPTION_CGI_CONTENT_TYPE
    bytes = ewsCGIContentType(context, string, sizeof(string));
    if (bytes != 0) printf("\tContent-Type = %s\n", string);
#   endif
#   ifdef EW_CONFIG_OPTION_CGI_CONTENT_LENGTH
    bytes = ewsCGIContentLength(context);
    if (bytes != 0) printf("\tContent-Length = %d\n", bytes);
#   endif

#   endif /* EW_CONFIG_OPTION_CGI */

#   ifdef EW_CONFIG_OPTION_CONTEXT_HOST
    bytes = ewsContextHost(context, string, sizeof(string));
    if (bytes != 0) printf("\tHost = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_DATE
    bytes = ewsContextDate(context, string, sizeof(string));
    if (bytes != 0) printf("\tDate = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_PRAGMA
    bytes = ewsContextPragma(context, string, sizeof(string));
    if (bytes != 0) printf("\tPragma = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_FROM
    bytes = ewsContextFrom(context, string, sizeof(string));
    if (bytes != 0) printf("\tFrom = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE
    bytes = ewsContextIfModifiedSince(context, string, sizeof(string));
    if (bytes != 0) printf("\tIf-Modified-Since = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_REFERER
    bytes = ewsContextReferer(context, string, sizeof(string));
    if (bytes != 0) printf("\tReferer = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_USER_AGENT
    bytes = ewsContextUserAgent(context, string, sizeof(string));
    if (bytes != 0) printf("\tUser-Agent = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_IF_MATCH
    bytes = ewsContextIfMatch(context, string, sizeof(string));
    if (bytes != 0) printf("\tIf-Match = %s\n", string);
#   endif

#   ifdef EW_CONFIG_OPTION_CONTEXT_IF_NONE_MATCH
    bytes = ewsContextIfNoneMatch(context, string, sizeof(string));
    if (bytes != 0) printf("\tIf-None-Match = %s\n", string);
#   endif

#ifndef EMWEB_OSE
    fflush(stdout);
#endif
}
#endif /* EWA_LOG_HOOK */

#ifdef EW_CONFIG_OPTION_DEMAND_LOADING
/*
 * ewaDocumentFault
 *    see websrc/include/ews_doc.h for description
 */
EwaStatus ewaDocumentFault(EwsContext context, EwaDocumentHandle handle)
{
  EW_UNUSED(context);
  EW_UNUSED(handle);
  return EWA_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_DEMAND_LOADING */

#ifdef EW_CONFIG_OPTION_URL_HOOK
#ifndef EMWEB_URL_HOOK_OVERRIDE
/*
 * ewaURLHook
 *
 *   If included, this is called by the EmWeb/Server after processing the
 *   HTTP request headers but before looking up the request in the archive.
 *   The examples given here show how to use this to access local file
 *   system objects.
 */
char * ewaURLHook (EwsContext context, char *url)
{
  char *session_id = NULL;
  L7_inet_addr_t inetAddr;
  L7_uint32 sessionType = L7_LOGIN_TYPE_UNKNWN;
  EwaNetHandle handle = ewsContextNetHandle(context);

  /* Conditionalize on the filesystem methods, not on EW_CONFIG_OPTION_FILE
   * itself, because the only filesystem operation available might be POST.
   */
# if defined( EW_CONFIG_OPTION_FILE_GET ) \
  || defined( EW_CONFIG_OPTION_FILE_PUT ) \
  || defined( EW_CONFIG_OPTION_FILE_DELETE )

  char *ptr;

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
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
    usmDbCpdmGlobalModeGet(&mode);
    if (L7_ENABLE == mode)
    {
#if 0
      L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
              "%s: CP is enabled, requested url = %s",__FUNCTION__,url);
#endif

      /* Get the client IP address */
      memset(&remote,0,sizeof(L7_inet_addr_t));
      if (handle->peer.u.sa.sa_family == AF_INET6)
      {
        if(L7_IP6_IS_ADDR_V4MAPPED(&handle->peer.u.sa6.sin6_addr))
        {
          remote.family = L7_AF_INET;
          remote.addr.ipv4.s_addr = handle->peer.u.sa6.sin6_addr.in6_u.u6_addr32[3];
          listeningPort = handle->listeningPort;
        }
        else
        {
          remote.family = L7_AF_INET6;
          remote.addr.ipv6 = *(L7_in6_addr_t *)&handle->peer.u.sa6.sin6_addr;
          L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                  "%s IPV6 is not currently supported",__FUNCTION__);
        }
      }
      else
      {
        remote.family = L7_AF_INET;
        remote.addr.ipv4.s_addr = osapiNtohl(handle->peer.u.sa4.sin_addr.s_addr);
        listeningPort = handle->listeningPort;
      }

      /* Check if client is a captive portal candidate */
      if (L7_SUCCESS==usmDbCpcmAIPStatusIntfIdGet(remote.addr.ipv4.s_addr,&intfId))
      {
        if ((strstr(url,CP_URL_PATH)!=NULL) ||             /* core CP URLs */
            (strstr(url,"/base/images/")!=NULL) ||         /* base images */
            (strstr(url,CP_URL_FILE_SYSTEM_PATH)!=NULL))   /* user defined images */
        {
          /* Allow CP related requests through */
        }
        else /* All others requests trigger a CP redirect */
        {
#if 0
          L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                  "%s: CP redirect candidate",__FUNCTION__);
#endif

          /* Get the associated CP configuration */
          if (L7_SUCCESS != usmDbCpdmCPConfigIntIfNumFind(intfId,&cpId))
          {
            return CP_URL_ERROR_PATH;
          }

          /* Check if we can continue to authenticate */
          if (L7_SUCCESS!=ewaCpConnListAnd(remote))
          {
            flag = CP_RESOURCE;
            /* TBD: write message to activity log */
          }

          /* Get protocol */
          usmDbCpdmCPConfigProtocolModeGet(cpId,&protocolMode);
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
            listeningPort = (protocolMode==L7_LOGIN_TYPE_HTTPS)?L7_SSLT_SECURE_PORT:HTTP_WELL_KNOWN_PORT;
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
          return NULL;
        }
      }
    }
#endif /* #ifdef L7_CAPTIVE_PORTAL_PACKAGE */

  /*
   * If URL begins with URL prefix, then map URL to local filesystem
   * instead of installed EmWeb archives.
   *
   * For example, map '/filesystem/x/y/z.html' => '/mnt/filesystem/x/y/z.html'
   *
   * We don't allow ".." to appear in the path for security.
   */
  if ((ptr = strstr( url, EMWEB_FILE_LOCAL_PREFIX )) == url)
    {
      static char prefix[] = EMWEB_FILE_LOCAL_ROOT;
      static char toc[]    = EMWEB_FILE_LOCAL_INDEX;
      STATBUF statbuf;
      struct tm  *tmp;
      char *path;
      char *date;
      char *date1036;
      char *cp;

      /*
       * Disallow ".." in path for security
       */
      if (strstr(url, "..") != NULL)
        {
          /* EMWEB_WARN(("ewaURLHook: Attempted .. in local file path\n")); */
          return NULL;
        }

      /*
       * setup the file information structure
       */
      ptr += strlen( EMWEB_FILE_LOCAL_PREFIX );  /* skip prefix */
      if (handle->params != NULL)
        {
          ewaFree(handle->params); /* may be same connection, so free prev */
        }

      if ((handle->params = (EwsFileParamsP)
                            ewaAlloc(  sizeof( EwsFileParams )
                                     + strlen( ptr )
                                     + strlen( prefix )
                                     + strlen( toc )
                                     + 30 /* RFC1123 date */
                                     + 34 /* RFC1036 date */
                                     + 1)
          ) == NULL)
        {
          EMWEB_ERROR(("ewaURLHook: unable to allocate file parameters\n"));
          return NULL;
        }

      memset( &handle->params->fileInfo
             ,0
             ,sizeof( handle->params->fileInfo )
             );

      /* build pathname under local root */
      date = (char *)(handle->params + 1);
      date1036 = date + 30;
      path = date1036 + 34;
      strcpy( path, prefix );
      strcat( path, ptr );

      /*
       * Read file information.
       */
      if (STAT(path, &statbuf) < 0)
        {

#         ifdef EW_CONFIG_OPTION_FILE_PUT
          /*
           * If file is not found, and method is not PUT, then return URL
           * (without file information) to cause a "not found" error.
           */
          if (ewsContextRequestMethod(context) != ewsRequestMethodPut)
            {
              ewaFree(handle->params);
              handle->params = NULL;
              return url;
            }

          /*
           * If method is PUT and file is not found, then set file with
           * PUT method allowed.
           */
          handle->params->fileInfo.allow = ewsRequestMethodPut;
          handle->params->fileInfo.realm = "File Write";
          handle->params->fileInfo.fileName = path;
          if (ewsContextSetFile( context, handle->params ) != EWS_STATUS_OK)
            {
              EMWEB_ERROR(( "ewaURLHook: ewsContextSetFile failed\n" ));
              ewaFree( handle->params );
              handle->params = NULL;
              return NULL;
            }
          return url;

#         else /* !EW_CONFIG_OPTION_FILE_PUT */

          ewaFree(handle->params);
          handle->params = NULL;
          return url;

#         endif /* EW_CONFIG_OPTION_FILE_PUT */

        }

      /*
       * If accessing a directory, append the index file to URL and
       * send redirect
       */
      if (S_ISDIR(statbuf.st_mode))
        {
          strcpy(path, url);
          strcat(path, toc);
          ewsContextSendRedirect(context, path);
          return NULL;
        }

      /*
       * Set mime type according to path suffix.  For now, we assume
       * text/html unless .gif
       */
      cp = strrchr(path, '.');
      if (!strcmp(".txt", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "text/plain";
        }
      else if (!strcmp(".jpg", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "image/jpeg";
        }
      else if (!strcmp(".png", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "image/png";
        }
      else if (!strcmp(".gif", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "image/gif";
        }
      else if (!strcmp(".html", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "text/html";
        }
      else if (!strcmp(".htm", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "text/html";
        }
      else if (!strcmp(".xml", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "text/xml";
        }
      else if (!strcmp(".js", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "text/javascript";
        }
      else if (!strcmp(".css", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "text/css";
        }
      else if (!strcmp(".pdf", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "applications/pdf";
        }
      else if (!strcmp(".ps", cp == NULL ? "" : cp))
        {
          handle->params->fileInfo.contentType = "applications/postscript";
        }
      else
        {
          handle->params->fileInfo.contentType = "application/octet-stream";
        }

      /*
       * If '/filesystem/public/...' or '/filesystem/ramnv' or '/filesystem/ramcp'
       * then disable authentication
       */
      if ((strstr( url, EMWEB_FILE_LOCAL_PUBLIC_PREFIX ) == url) ||
          (strstr( url, EMWEB_FILE_LOCAL_RAM_NV_PREFIX ) == url) ||
          (strstr( url, EMWEB_FILE_LOCAL_RAM_CP_PREFIX ) == url))
        {
          handle->params->fileInfo.realm = NULL;
        }

      /*
       * Otherwise, assign realm for access control
       */
      else
        {
          handle->params->fileInfo.realm = "File Read";
        }

#      ifdef EW_CONFIG_OPTION_DATE
      /* !TBD! pSOS gmtime doesn't work */
       tmp = gmtime(&statbuf.st_mtime);
       sprintf( date
              , "%s, %02d %s %04d %02d:%02d:%02d GMT"
              ,day[tmp->tm_wday]
              ,tmp->tm_mday
              ,month[tmp->tm_mon]
              ,tmp->tm_year + 1900
              ,tmp->tm_hour
              ,tmp->tm_min
              ,tmp->tm_sec
              );
       {
         /* Put this array here because the only reference to date1036 is
          * in the following sprintf(), and this code is heavily
          * conditionalized. */

         static const char *day1036[] = {
           "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
           "Saturday"
         };

         sprintf( date1036
                  , "%s, %02d-%s-%02d %02d:%02d:%02d GMT"
                  , day1036[tmp->tm_wday]
                  , tmp->tm_mday
                  , month[tmp->tm_mon]
                  ,tmp->tm_year % 100
                  ,tmp->tm_hour
                  ,tmp->tm_min
                  ,tmp->tm_sec
                  );
       }
#      endif

      /*
       * Set up remaining file information
       */
      handle->params->fileInfo.fileName = path;
      handle->params->fileInfo.contentLength = statbuf.st_size;
      handle->params->fileInfo.allow = ( ewsRequestMethodGet
#                                       ifdef EW_CONFIG_OPTION_FILE_PUT
                                        | ewsRequestMethodPut
#                                       endif /* EW_CONFIG_OPTION_FILE_PUT */
#                                       ifdef EW_CONFIG_OPTION_FILE_DELETE
                                        | ewsRequestMethodDelete
#                                       endif /* EW_CONFIG_OPTION_FILE_DELETE */
                                        );
      handle->params->fileInfo.lastModified = date;
      handle->params->fileInfo.lastModified1036 = date1036;
      handle->params->fileInfo.eTag = "";

#     ifdef EW_CONFIG_OPTION_FILE_DELETE
      /*
       * If DELETE method, set access controls
       */
      if (ewsContextRequestMethod(context) == ewsRequestMethodDelete)
        {
          handle->params->fileInfo.realm = "File Write";
        }
#     endif /* EW_CONFIG_OPTION_FILE_DELETE */

#     ifdef EW_CONFIG_OPTION_BYTE_RANGES
      /*
       * If byte range, adjust params
       */
      {
        int32 firstBytePos;
        int32 lastBytePos;
          handle->params->fileInfo.acceptRanges = TRUE;
          handle->params->fileInfo.instanceLength = statbuf.st_size;
          if (ewsContextRange(context, &firstBytePos, &lastBytePos)
              == EWS_STATUS_OK)
            {
              if (firstBytePos < 0)
                {
                  firstBytePos += statbuf.st_size;
                  lastBytePos = statbuf.st_size - 1;
                }
              if (lastBytePos == EWS_CONTENT_LENGTH_UNKNOWN)
                {
                  lastBytePos = statbuf.st_size - 1;
                }
              if (firstBytePos >= statbuf.st_size ||
                  lastBytePos >= statbuf.st_size
                 )
                {
                  ewsContextSendRangeError(context, statbuf.st_size);
                  return NULL;
                }

              handle->params->fileInfo.isRange = TRUE;
              handle->params->fileInfo.firstBytePos = firstBytePos;
              handle->params->fileInfo.lastBytePos = lastBytePos;
              handle->params->fileInfo.contentLength
                = 1 + lastBytePos - firstBytePos;
            }
      }
#     endif /* EW_CONFIG_OPTION_BYTE_RANGES */

      /*
       * Notify EmWeb/Server to serve file from local filesystem instead
       * of installed archives.
       */
      if (ewsContextSetFile( context, handle->params ) != EWS_STATUS_OK)
        {
          EMWEB_ERROR(( "ewaURLHook: ewsContextSetFile failed\n" ));
          ewaFree( handle->params );
          handle->params = NULL;
          return NULL;
        }
    }
# endif /* EW_CONFIG_OPTION_FILE_* */

  /* lookup session ID and query internal database */
  if (handle != NULL)
  {
    if (handle->connection_type == EW_CONNECTION_HTTPS)
    {
      sessionType = L7_LOGIN_TYPE_HTTPS;
      session_id = ewsContextCookieValue(context, "SIDSSL");
    }
    else
    {
      sessionType = L7_LOGIN_TYPE_HTTP;
      session_id = ewsContextCookieValue(context, "SID");
    }
    memset(&inetAddr, 0x00, sizeof(inetAddr)); /* This is not used in the following call */
    context->session = EwaSessionLookup(session_id, sessionType, inetAddr, L7_FALSE);
  }

  return url;
}
#endif /* EMWEB_URL_HOOK_OVERRIDE */
#endif /* EW_CONFIG_OPTION_URL_HOOK */

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
/*
 * ewaAuthDigestSecret
 * Provide server-specific secret to be used in nonce generation
 */
void
ewaAuthDigestSecret( char **secret )
{
  static char nonce_secret[12];

  sprintf( nonce_secret, "%0d", rand() );
  *secret = nonce_secret;
}
/*
 * ewaAuthNonceCreate
 * Initialize application-specific seed parameters used to generate unique
 * nonce authentication challenges.
 */
void
ewaAuthNonceCreate
( EwsContext context, const char *realm, EwaAuthNonce *noncep )
{
  static uint32 up_counter;
  EwaNetHandle handle = ewsContextNetHandle(context);
  EW_UNUSED(realm);

  noncep->client_ip = osapiNtohl(handle->peer.sin_addr.s_addr);
  noncep->timestamp = (uint32) time(NULL);
  noncep->up_counter = up_counter++;
  strncpy(noncep->secret, "PrivKey", EWA_AUTH_SECRET_SIZE);
}

/*
 * ewaAuthNonceCheck
 * Check validity of nonce challenge for current context
 */
EwaAuthNonceStatus
ewaAuthNonceCheck
( EwsContext context, const char * realm, EwaAuthNonce *noncep, uintf count )
{
  EwaNetHandle handle = ewsContextNetHandle(context);
  uint32 seconds;
  EW_UNUSED(realm);
  EW_UNUSED(count);

  /*
   * Verify nonce generated for this client's IP address
   */
  if (noncep->client_ip != osapiNtohl(handle->peer.sin_addr.s_addr))
    {
      return ewaAuthNonceDenied;
    }

  /*
   * Check timestamp and use count (for now, we'll just print a message
   * to track usage.  If the nonce hasn't been used for an hour, expire it).
   */
  seconds = ((uint32) time(NULL)) - noncep->timestamp;
  EMWEB_TRACE(("ewaAuthNonceCheck: count = %lu age = %lu\n",
         (unsigned long) count, (unsigned long) seconds));
  if (seconds > 3600) /* an hour for now, nice for testing */
    {
      return ewaAuthNonceStale;
    }

  return ewaAuthNonceOK;
}
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#ifdef EW_CONFIG_OPTION_AUTH_MBASIC
/* See websrc/include/ews_auth.h */
#ifndef EMWEB_MBASIC_OVERRIDE
boolean ewaAuthCheckBasic( EwsContext context
                           ,const char *realm
                           ,const char *basicCookie
                           ,const char *userName
                           ,const char *password )
{
  L7_uint32    accessLevel;
  L7_uint32    realmID = 0;
  L7_uint32    web_mode;
  L7_BOOL      bAllowAccess = L7_FALSE;
  L7_char8     uname[L7_LOGIN_SIZE];
  L7_char8     pwd[L7_PASSWORD_SIZE];
  EwaNetHandle handle = ewsContextNetHandle(context);
  char        *session_id = NULL;


  EW_UNUSED(realm);
  EW_UNUSED(basicCookie);
  EW_UNUSED(userName);
  EW_UNUSED(password);

  memset(uname, 0x00, sizeof(uname));
  memset(pwd, 0x00, sizeof(pwd));

  strncpy(uname, userName, sizeof(uname));
  strncpy(pwd, password, sizeof(pwd));


  context->allow_http = TRUE;
  if (usmDbSwDevCtrlWebMgmtModeGet(0, &web_mode) == L7_SUCCESS)
  {
    if (web_mode != L7_ENABLE)
    {
      if ((handle == NULL) || (handle->connection_type != EW_CONNECTION_HTTPS))
        context->allow_http = FALSE;
    }
  }

  if (cliIsCurrUnitMgmtUnit() != L7_TRUE)
  {
    return FALSE;
  }

  if (handle != NULL)
  {
    if (handle->connection_type == EW_CONNECTION_HTTPS)
    {
      session_id = ewsContextCookieValue(context, "SIDSSL");
    }
    else
    {
      session_id = ewsContextCookieValue(context, "SID");
    }
  }

  if (EwaSessionAccessLevelGet(session_id, &accessLevel) != L7_SUCCESS)
  {
    bAllowAccess = L7_FALSE;
  }
  else
  {
    if (strcmp(realm, "READWRITE") == 0)
      realmID = L7_LOGIN_ACCESS_READ_WRITE;
    else if (strcmp(realm, "READONLY") == 0)
      realmID = L7_LOGIN_ACCESS_READ_ONLY;
    else
      realmID = L7_LOGIN_ACCESS_NONE;

    switch (accessLevel)
    {
      case L7_LOGIN_ACCESS_READ_WRITE:  /* READWRITE users get access to it all */
        if ((realmID == L7_LOGIN_ACCESS_READ_WRITE) || (realmID == L7_LOGIN_ACCESS_READ_ONLY))
        {
          bAllowAccess = L7_TRUE;
        }
        else
        {
          bAllowAccess = L7_FALSE;
        }
        break;
      case L7_LOGIN_ACCESS_READ_ONLY:
        if (realmID == L7_LOGIN_ACCESS_READ_ONLY)
        {
          bAllowAccess = L7_TRUE;
        }
        else
        {
          bAllowAccess = L7_FALSE;
        }
        break;
      case L7_LOGIN_ACCESS_NONE:
        bAllowAccess = L7_FALSE;
        break;
      default:
        bAllowAccess = L7_FALSE;
        break;
    }
  }

  if (context->session == NULL)
  {
    if (realmID != L7_LOGIN_ACCESS_NONE)
    {
      bAllowAccess = L7_FALSE;
    }
  }

  if (bAllowAccess == L7_TRUE)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

}
#endif /* !EMWEB_MBASIC_OVERRIDE */
#endif /* EW_CONFIG_OPTION_AUTH_MBASIC */

#ifdef EW_CONFIG_OPTION_AUTH_MDIGEST
/* See websrc/include/ews_auth.h */

#define EW_MD5_SZ       (16)                    /* MD5 digest bytes */
#define EW_MD5_HEX_SZ   (2 * EW_MD5_SZ)         /* ASCII hexadecimal chars */

void authMD5String( char *dst, MD5_CTX *md5_ctx )
{
  uint8 md5[EW_MD5_SZ];
  uintf i;
  uintf nybble;

    MD5Final(md5, md5_ctx);
    for (i = 0; i < EW_MD5_SZ; i++)
      {
        nybble = (md5[i] >> 4) & 0x0f;
        *dst++ = nybble > 9? nybble - 10 + 'a' : nybble + '0';
        nybble = md5[i] & 0x0f;
        *dst++ = nybble > 9? nybble - 10 + 'a' : nybble + '0';
      }
    *dst = '\0';
}


boolean ewaAuthDigestHash( EwsContext context
                                  ,const char *realm
                                  ,const char *user
                                  ,const char *nonce
                                  ,const char *cnonce
                                  ,char **digest )
{
  MD5_CTX md5_ctxt;
  char    md5[EW_MD5_HEX_SZ + 1];
  MD5_CTX A1_md5_ctxt;
  char    *A1_md5;

  A1_md5 = *digest;
  /*
   * and generate MD5(user:realm:password):nonce:cnonce = A1
   */
  MD5Init(&md5_ctxt);
  MD5Update(&md5_ctxt, (uint8 *)user, strlen(user));
  MD5Update(&md5_ctxt, (uint8 *) ":", 1);
  MD5Update(&md5_ctxt, (uint8 *)realm, strlen(realm));
  /*
   *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   * now GET THE PASSWORD and add here
   *!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   *
  MD5Update(&md5_ctxt, (uint8 *) ":", 1);
  MD5Update(&md5_ctxt,(uint8 *)password, strlen(password));
   */
  authMD5String(md5, &md5_ctxt);
  /*
   *  hash above with nonce and cnonce
   */
  MD5Init(&A1_md5_ctxt);
  MD5Update(&A1_md5_ctxt, (uint8 *)md5 ,EW_MD5_HEX_SZ);
  MD5Update(&A1_md5_ctxt, (uint8 *) ":", 1);
  MD5Update(&A1_md5_ctxt, (uint8 *)nonce, strlen(nonce));
  MD5Update(&A1_md5_ctxt, (uint8 *) ":", 1);
  MD5Update(&A1_md5_ctxt, (uint8 *)cnonce, strlen(cnonce));
  authMD5String(A1_md5, &A1_md5_ctxt);

  *digest = A1_md5;

  return(TRUE);
}
#endif /* EW_CONFIG_OPTION_AUTH_MDIGEST */

#ifdef EW_CONFIG_OPTION_AUTH_VERIFY
/* See websrc/include/ews_auth.h */
boolean ewaAuthVerifySecurity( EwsContext context
                               ,const char *realm )
{
  EW_UNUSED(context);
  EW_UNUSED(realm);
  return TRUE;
}
#endif /* EW_CONFIG_OPTION_AUTH_VERIFY */

#if !defined(EMWEB_FILE_OVERRIDE) && !defined(EW_CONFIG_OPTION_UPLOAD_ARCHIVE)

#ifdef EW_CONFIG_OPTION_FILE
#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
/*
 * File upload interface:
 *

 */
EwaFileHandle ewaFilePost ( EwsContext context
                            ,const EwsFileParams *params
                            )
{
  L7_BOOL fIsNVImageFile = L7_FALSE;
  L7_BOOL fIsCPImageFile = L7_FALSE;
  EwaFileHandle fp;
  L7_uint32 unit; 
  int PMAX = 80;
  char path[PMAX];
  char *filename;
  L7_char8 download_path[256];
#if defined (L7_WIRELESS_PACKAGE) || defined (L7_CAPTIVE_PORTAL_PACKAGE)
  char *GIF = ".GIF";
  char *JPG = ".JPG";
  char *gif = ".gif";
  char *jpg = ".jpg";
#endif /* L7_WIRELESS_PACKAGE */

  unit = usmDbThisUnitGet(); 

  /* Perform some simple validation */
  if (params->fileField.fileName == NULL) 
  {
    /* EMWEB_WARN( ("ewaFilePost: Invalid file name\n") ); */
    return (EWA_FILE_HANDLE_NULL);
  }


  if (strstr(context->url, "http_file_download.html") == NULL)
  {
      if ( (strstr(params->fileField.fileName,".") == NULL) ||
     (strlen(params->fileField.fileName)<4))
     {
         /* EMWEB_WARN( ("ewaFilePost: Invalid file name\n") ); */
         return (EWA_FILE_HANDLE_NULL);
     }
  }

  filename = ewaAlloc(sizeof(params->fileField.fileName)+1);

  /* Strip out filename if in WinOs name format */
  if (strrchr(params->fileField.fileName,'\\') != NULL)
  {
    filename = strrchr(params->fileField.fileName,'\\');
    filename++; /* skip slash */
  }
  else
  {
    filename = (char *)params->fileField.fileName;
  }

  /*
   * if file length is known, and is too big, then fail
   */
# ifdef FILE_MAX_LENGTH
  if (   params->fileField.contentLength != EWS_CONTENT_LENGTH_UNKNOWN
      && params->fileField.contentLength > FILE_MAX_LENGTH
     )
  {
    /* EMWEB_WARN(("ewaFilePost: file too long\n")); */
    return (EWA_FILE_HANDLE_NULL);
  }
#endif /* FILE_MAX_LENGTH */

#ifdef L7_WIRELESS_PACKAGE
  /* Define the target path by parsing on the URL */
  if (strstr(context->url, "nv_image_mgmt.html") != NULL)
  {
    if (strlen(filename)>L7_NV_MAX_FILE_NAME)
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file name.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }

    if ((strstr(filename,GIF)==NULL) &&
        (strstr(filename,JPG)==NULL) &&
        (strstr(filename,gif)==NULL) &&
        (strstr(filename,jpg)==NULL))
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file type.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }

    fIsNVImageFile = L7_TRUE;
    strncpy(path,RAM_NV_PATH,PMAX);
    strcat(path,RAM_NV_NAME);
  }
  else
  {
    /* could assign a different path based on url */
  }
#endif /* L7_WIRELESS_PACKAGE */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  /* Define the target path by parsing on the URL */
  if (strstr(context->url, "cp_web_custom_global.html") != NULL)
  {
    if (strlen(filename)>CP_FILE_NAME_MAX)
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file name, too big.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }

    if ((strstr(filename,GIF)==NULL) &&
        (strstr(filename,JPG)==NULL) &&
        (strstr(filename,gif)==NULL) &&
        (strstr(filename,jpg)==NULL))
    {
      /* EMWEB_WARN(("ewaFilePost: invalid file type.\n")); */
      return (EWA_FILE_HANDLE_NULL);
    }

    fIsCPImageFile = L7_TRUE;
    strncpy(path,RAM_CP_PATH,PMAX);
    strcat(path,RAM_CP_NAME);
  }
  else
  {
    /* could assign a different path based on url */
  }
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */


  if (strstr(context->url, "http_file_download.html") != NULL)
  {
    if (usmDbTransferFileNameLocalSet(unit, filename) != L7_SUCCESS)
	{
	  return(EWA_FILE_HANDLE_NULL);
	}

	/* Prepare for file transfer */
	if (usmDbHttpTransferFilePrepare(unit) != L7_SUCCESS)
	{
      return(EWA_FILE_HANDLE_NULL);
	}

	memset(download_path, 0, sizeof(download_path));
    strcpy(download_path, DOWNLOAD_PATH);


#ifdef L7_CODE_UPDATE_TO_FLASH
    /* There is no other way of finding out here if the user has initiated 
	** a code download or not.
	*/
    if(strstr(params->fileField.fileName, ".stk") != NULL)
	{
      strcpy(download_path, CONFIG_PATH);
	}
#endif

    if ((fp = (EwaFileHandle)ewaAlloc(  sizeof( EwaFileHandle_t )
                                      + strlen(download_path)
                                      + 1 /* path delimiter */
                                      + strlen(filename)
                                      + 10 /* EOL */))
        != EWA_FILE_HANDLE_NULL)
     {
       /* construct the path to the local filesystem and open it. */
       fp->localPath = (char *)(fp + 1);
       strcpy(fp->localPath,download_path);
       strcat(fp->localPath,filename);
     }
   }
   else
   {

  if ((fp = (EwaFileHandle)ewaAlloc(  sizeof( EwaFileHandle_t )
                                      + strlen(path)
                                      + 1 /* path delimiter */
                                      + strlen(filename)
                                      + 1 /* EOL */))
      != EWA_FILE_HANDLE_NULL)
  {
    /* construct the path to the local filesystem and open it. */
    fp->localPath = (char *)(fp + 1);
    strcpy(fp->localPath,path);
    strcat(fp->localPath,"/");
    strcat(fp->localPath,filename);
  }
   }

  if (osapiFsFileCreate((L7_char8 *)fp->localPath, &fp->fd) == L7_ERROR)
  {
    /* EMWEB_WARN(("ewaFilePost: unable to create file\n")); */
    fp = EWA_FILE_HANDLE_NULL;
  }
  else
  {
    if (fIsNVImageFile == TRUE)
    {
#ifdef L7_WIRELESS_PACKAGE
      wnvSetImageCfgChanged();
#endif /* L7_WIRELESS_PACKAGE */
    }
    if (fIsCPImageFile == TRUE)
    {
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
      usmDbCpdmSetImageFileChanged();
#endif /* L7_CAPTIVE_PORTAL_PACKAGE */
    }
  }
  return fp;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#ifndef EMWEB_FILE_WRITE_OVERRIDE
/*
 * File write interface:
 *
 * This would typically write data to a file (using POSIX write() or the
 * equivalent) and return the number of bytes written.
 *
 * However, continuing the MD5() calculation example above, we simply
 * update the MD5() checksum from the data.
 */
sintf ewaFileWrite ( EwsContext context
                     ,EwaFileHandle handle
                     ,const uint8  *datap
                     ,uintf         length
                     )
{
  if (handle != EWA_FILE_HANDLE_NULL)
  {
    if (osapiFsWriteNoClose(handle->fd,(char *)datap,length) == L7_ERROR)
    {
      /* EMWEB_WARN(("ewaFileWrite: Error writing to file.\n")); */
      return -1;
    }
  }
  return length;
}
#endif /* EMWEB_FILE_WRITE_OVERRIDE */

#ifndef EMWEB_FILE_READ_OVERRIDE
sintf ewaFileRead ( EwsContext context
                   , EwaFileHandle handle      /* handle from ewaFileOpen */
                   , uint8   *datap           /* pointer to data buffer */
                   , uintf         length     /* length of buffer */
                  )
{
  sintf bytes;
  EW_UNUSED(context);

  if (handle != EWA_FILE_HANDLE_NULL)
    {
      switch( handle->type )
        {
        case fileHandleMD5Example:
          return -1;
        case fileHandleStream:

#         ifdef EW_CONFIG_OPTION_BYTE_RANGES
          /* If sending range, adjust length, check for EOF */
          if (handle->isRange)
            {
              if (length > handle->bytesRemaining)
                {
                  length = handle->bytesRemaining;
                }
              if (length == 0)
                {
                  return length;
                }
            }

#         endif /* EW_CONFIG_OPTION_BYTE_RANGES */

          if (osapiFileRead(handle->fd,datap,length) == L7_ERROR)
          {
           /*  EMWEB_WARN(("ewaFileRead: Error reading from file.\n")); */
            return -1;
          }
          bytes = length; // osapi() takes care of writes. this may be a problem for emweb????

#         ifdef EW_CONFIG_OPTION_BYTE_RANGES
          handle->bytesRemaining -= bytes;
#         endif /* EW_CONFIG_OPTION_BYTE_RANGES */

          return bytes;

        default:
          break;
        }
    }
  return -1;
}
#endif /* EMWEB_FILE_READ_OVERRIDE */

/*
 * File close interface:
 *
 * This would typically close a file descriptor (using POSIX close() or the
 * equivalent.
 */

#ifndef EMWEB_FILE_CLOSE_OVERRIDE
EwaFileStatus ewaFileClose ( EwaFileHandle handle, EwsStatus status )
{
  if (handle != EWA_FILE_HANDLE_NULL)
  {
    (void)osapiFsClose (handle->fd);
  }
  return EWA_FILE_STATUS_OK;
}
#endif /* EMWEB_FILE_CLOSE_OVERRIDE */


#ifdef EW_CONFIG_OPTION_FILE_GET
#ifndef EMWEB_FILE_GET_OVERRIDE
EwaFileHandle ewaFileGet ( EwsContext context
                          ,const char *url
                          /* from ewsContextSetFile() */
                          ,const EwsFileParams *params
                         )
{
  /* just attempt to open it.  If the open fails, then the
   * server should set back a "not found" error
   */
  EwaFileHandle efa = &ewsContextNetHandle(context)->file_handle;
  EW_UNUSED(url);

  efa->context = context;
  efa->type = fileHandleStream;

  efa->fd = osapiFsOpen((L7_char8 *)params->fileInfo.fileName);
  if (efa->fd == L7_ERROR)
  {
    /* EMWEB_WARN(("ewaFileGet: unable to open file\n")); */
    return EWA_FILE_HANDLE_NULL;
  }

# ifdef EW_CONFIG_OPTION_BYTE_RANGES
  efa->isRange = params->fileInfo.isRange;
  efa->bytesRemaining = params->fileInfo.contentLength;
  if (efa->isRange)
  {
    osapiFileSeek(efa->fd,params->fileInfo.firstBytePos,SEEK_SET);
  }
# endif /* EW_CONFIG_OPTION_BYTE_RANGES */
  return efa;
}
/* returns EWA_FILE_HANDLE_NULL on error */
#endif /* EMWEB_FILE_GET_OVERRIDE */
#endif /* EW_CONFIG_OPTION_FILE_GET */

#ifdef EW_CONFIG_OPTION_FILE_PUT
#ifndef EMWEB_FILE_PUT_OVERRIDE
EwaFileHandle ewaFilePut ( EwsContext context
                          ,const EwsFileParams *params
                        )
{
  /* just attempt to open it.  If the open fails, then the
   * server should set back a "not found" error
   */
  EwaFileHandle efa = &ewsContextNetHandle(context)->file_handle;
  efa->context = context;
  efa->type = fileHandleStream;
  efa->fd = osapiFsOpen((L7_char8 *)params->fileInfo.fileName);
  if (efa->fd == L7_ERROR)
  {
    /* EMWEB_WARN("ewaFilePut: unable to open file\n"); */
    return EWA_FILE_HANDLE_NULL;
  }
  return efa;
}
#endif /* EMWEB_FILE_PUT_OVERRIDE */
#endif /* EW_CONFIG_OPTION_FILE_PUT */

#ifdef EW_CONFIG_OPTION_FILE_DELETE
EwaStatus ewaFileDelete ( EwsContext context
                         ,const EwsFileParams *params
                        )
{
  EW_UNUSED(context);
  osapiFsDeleteFile((L7_char8*)params->fileInfo.fileName);
  return EWA_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FILE_DELETE */


#endif /* EW_CONFIG_OPTION_FILE */

#endif /* !EMWEB_FILE_OVERRIDE && !EW_CONFIG_OPTION_UPLOAD_ARCHIVE */

/* If using the EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE option,
fill in these functions
*/
#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
#ifdef EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE

/* ewaAuthDigestChange_serve:
 * Application-provided function to initialize the form before serving.
 */
void ewaAuthDigestChange_serve(EwsContext context, void *formp)
{
}


/* FUNCTION ewaAuthDigestChange:
 * Application-provided function called by the server after the password
 * change form has successfully been submitted (called from
 * ewaFormServer_passwordChange()).
 .
 * NOTE:  This is a shell for the function - the application developer
 * must CUSTOMIZE it.
 *
 * The application should use this function to do the following:
 *  1. Decide whether or not the user submitting this request (available via
 *     ewsContextAuthHandle() using the 'context' arg) is permitted to
 *     change the password for the user.
 *  2. Set the reply page by calling ewsContextSendReply (the built in
 *     submit routine for the form returns NULL, so if this does not
 *     happen, the user will get a 'no data' response; probably not what the
 *     application wants).
 *  3. Arrange for the stable storage of the new values so that when the
 *     server is initialized next time they are registered.
 *  4. Return an EwaAuthHandle value to indicate whether or not the application
 *     wishes the password change to be effective immediately.
 *     Note:
 *     A non-NULL handle returned tells the server that the password
 *     change should take effect immediately.
 *      A NULL handle indicates that the application does NOT want the
 *     password change to take effect.
 *
 */

EwaAuthHandle ewaAuthDigestChange(EwsContext context,
                                  const EwaAuthHandle oldHandle,
                                  const char *realm,
                                  const char *user,
                                  const char *newhash)
{
  EwaAuthHandle handle;

  EMWEB_TRACE(("ewaAuthDigestChange: realm = %s,user=%s\n",
               realm, user));

  handle = ewsContextAuthHandle(context);

   /* THIS IS JUST AN EXAMPLE!
   * Change the check to be  what you want here!
   * Ex: only allow certain users (get from 'oldHandle' arg) to set the password
   *     or only allow certain users (in 'user' arg) to have their passwds changed.
   */

  /* for this example, if the user who password is being changed is "user2",
    then we do not allow the password change to take place
    */

  if(0 == strcmp(user,"user2"))
    {
      /* return NULL if you do NOT want the password change to take effect*/
      return EWA_NET_HANDLE_NULL;
    }
  else
    {
      /* SET THE REPLY PAGE - FILL IN THE REPLY PAGE YOU WANT HERE */
      ewsContextSendReply(context, "/password_change/reply.html");
    }
  /*  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   *  At this point you MUST arrange for stable storage of the
   *  password (newhash) !
   *  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   */

  /* Return a NON-NULL handle to tell the server to use the new password!! */
  /* Fill in the handle appropriately.*/
  return((EwaAuthHandle)0xbeeff0bd);
}
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

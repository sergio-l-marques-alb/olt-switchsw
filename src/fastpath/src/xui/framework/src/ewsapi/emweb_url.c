/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename emweb_url.c
*
* @purpose
*
* @component EMWEB
*
* @comments
*
* @create 09/04/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "posix_config.h"
#include "ew_config.h"
#include "ews_api.h"
#include "ewnet.h"

#include "ews.h"
#include "session.h"

#include <time.h>

#include "fastpath_inc.h"
#include "emweb_url.h"
#include "datatypes.h"
#include "emweb_if.h"
#include "ew_config.h"

static emwebUrlHook_t* emwebUrlHookList;

extern char *ewaURLHookForRPC (EwsContext context, char *url);
extern int ewaFileDownloadGetProgressGlobal(void);
extern int ewaFileDownloadGetProgress(EwsContext context);

static int (*emwebTransferProgressCheckCallback)(void *) = NULL;
static char *emwebTransferProgressCheckDefaultUrl = "/http_download_status.html";
static void *emwebTransferProgressCheckCallbackArg = NULL;
static char *emwebTransferProgressCheckCallbackUrl = NULL;
static char *emwebTransferProgressCheckCompletitionUrl = NULL;

void emwebTransferProgressCheckSet(int (*callback)(void *), void *arg, char *url1, char *url2)
{
  emwebTransferProgressCheckCallback = callback;
  emwebTransferProgressCheckCallbackArg = arg;
  emwebTransferProgressCheckCallbackUrl = url1;
  emwebTransferProgressCheckCompletitionUrl = url2;
}

void *emwebTransferProgressCheckGet(void)
{
  return emwebTransferProgressCheckCallback;
}

void emwebDownloadUrlSet (EwsContext context, char *url)
{
  EwaNetHandle handle = ewsContextNetHandle (context);

  handle->downloadUrl[0] = 0;

  if (url != NULL)
  {
    sprintf (handle->downloadUrl, "%s%s", EMWEB_FILE_LOCAL_PREFIX, url);
  }
}

static char *ewaURLTransferHook(EwsContext context, char *url)
{

  /* For a new web session url is "/". Don't allow new session if transfer is in progress*/
  if ((strlen(url) == 1) && (osapiStrncmp(url, "/", 1) == 0) &&
      (usmDbTransferInProgressGet(1) == L7_TRUE) &&
      (ewaFileDownloadGetProgress(context) == FALSE))
  {
    return emwebTransferProgressCheckDefaultUrl;
  }

  /* If http download is in progress in current context then serve the page.
   * js, css and gif files should always be processed.
   */
  if (((ewaFileDownloadGetProgressGlobal() == TRUE) && (ewaFileDownloadGetProgress(context) == TRUE)) ||
     (strstr (url, ".html") == NULL))
  {
        return url;
  }

  if(emwebTransferProgressCheckCallback != NULL)
  {
    if(emwebTransferProgressCheckCallback(emwebTransferProgressCheckCallbackArg))
    {
      return emwebTransferProgressCheckCallbackUrl;
    }

    if ((osapiStrncmp(url, emwebTransferProgressCheckCallbackUrl, strlen(emwebTransferProgressCheckCallbackUrl)) == 0) &&
        (emwebTransferProgressCheckCompletitionUrl != NULL))
    {
      return emwebTransferProgressCheckCompletitionUrl;
    }
  }

  /* If http download is in progress in another context then serve the default page.
   */
  if ((ewaFileDownloadGetProgressGlobal() == TRUE) && (ewaFileDownloadGetProgress(context) != TRUE)&& 
     (strstr (url, ".html") != NULL))
  {
    return emwebTransferProgressCheckDefaultUrl;
  }

  return url;
}

/*
 * ewaURLHook
 *
 *   If included, this is called by the EmWeb/Server after processing the
 *   HTTP request headers but before looking up the request in the archive.
 */

char *ewaURLHook (EwsContext context, char *url)
{
  EW_NSC char *session_id = NULL;
  L7_inet_addr_t inetAddr;
  L7_uint32 sessionType = L7_LOGIN_TYPE_UNKNWN;
  EwaNetHandle handle = ewsContextNetHandle (context);

  url = ewaURLTransferHook(context, url);
  
  /*
   * If URL begins with URL prefix, then map URL to local filesystem
   * instead of installed EmWeb archives.
   *
   * For example, map '/filesystem/x/y/z.html' => '/mnt/filesystem/x/y/z.html'
   *
   * We don't allow ".." to appear in the path for security.
   */
  ewaNewUrl(context,url,url);
  url = emwebFilePathHandler(context,url);
   
  /* lookup session ID and query internal database */
  if (handle != NULL)
  {
    if (handle->connection_type == EW_CONNECTION_HTTPS)
    {
      sessionType = L7_LOGIN_TYPE_HTTPS;
      session_id = ewsContextCookieValue (context, "SIDSSL");
    }
    else
    {
      sessionType = L7_LOGIN_TYPE_HTTP;
      session_id = ewsContextCookieValue (context, "SID");
    }
    memset (&inetAddr, 0x00, sizeof (inetAddr));        /* This is not used in the following call */
    context->session = EwaSessionLookup ((char *)session_id, sessionType, inetAddr, L7_FALSE);
  }

#ifdef L7_XWEB_RPCC
    url = ewaURLHookForRPC(context, url);
#endif  

  return url;
}

/*********************************************************************
* @purpose  Register a url hook function
*
* @param    srcurl  source url on which the function needs to be called
* @param     func    function to be called if the url requested matches with srcurl
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments  When the requested url matches the srcurl, func is invoked which will return the new url 
*
* @end
*
*********************************************************************/
L7_RC_t emwebUrlRegisterHook(char* srcurl,void* func)
{
 emwebUrlHook_t* temp = L7_NULL;
 
 if(func == L7_NULL)
 {
  return 1; 
 }
 
 if(emwebUrlHookList == L7_NULL)
 {
  /*
    first registration
  */
  emwebUrlHookList = (emwebUrlHook_t*)osapiMalloc(L7_CLI_WEB_COMPONENT_ID,sizeof(emwebUrlHook_t));
  if(emwebUrlHookList == L7_NULL)
  {
   EMWEB_ERROR(("\r\nFailed to allocate memory for emwebUrlHookList"));
   return L7_FAILURE;
  }
 }
 else
 {
  temp = (emwebUrlHook_t*)osapiMalloc(L7_CLI_WEB_COMPONENT_ID,sizeof(emwebUrlHook_t));
  if(temp == L7_NULL)
  {
   EMWEB_ERROR(("\r\nFailed to allocate memory for emwebUrlHookList"));
   return L7_FAILURE;
  }
  
  temp->next = emwebUrlHookList;
  emwebUrlHookList = temp;  
  /*
     NOTE:We are not checking for duplicates.
  */
 }

  if(srcurl != L7_NULL)
  {
  
   emwebUrlHookList->urlhook.srcurl = osapiStrDup(srcurl);
  }
 else
 {
  /*if srcurl is NULL then the function will be called for all the urls*/
   emwebUrlHookList->urlhook.srcurl = L7_NULL;
 }
 emwebUrlHookList->urlhook.func = (URLHOOK)func;
 
 return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Deregister a url hook function
*
* @param    srcurl  source url on which the function needs to be called

* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t emwebUrlDeregisterHook(void* func)
{
   emwebUrlHook_t* temp = emwebUrlHookList;
  emwebUrlHook_t* deleteNode = L7_NULL;
  
  if(func == L7_NULL)
  {
   EMWEB_ERROR(("\r\n srcurl is null during hook deregistration"));
   return L7_FAILURE;
  }
  while(temp != L7_NULL)
  {
    if(func == temp->urlhook.func)
    {
     if(temp->next == L7_NULL)
     {
     /*
       it means temp is the last node in the list. free it directly
     */
      if(temp->urlhook.srcurl != L7_NULL)
      {
        osapiFree(L7_CLI_WEB_COMPONENT_ID,temp->urlhook.srcurl);
      temp->urlhook.srcurl = L7_NULL;
      }  
     osapiFree(L7_CLI_WEB_COMPONENT_ID,temp); 
     }
    else
    {
       /*it means temp is not the last node.
         so copy the contents of temp->next to temp and delete temp->next
         Before doing it there is a good chance that temp->srcurl and temp->next->srcurl
         may not be of same size.so free temp->srcurl,malloc temp->next->srcurl and copy
         temp->next->srcurl
     */
     deleteNode = temp->next;
     if(temp->urlhook.srcurl != L7_NULL)
     {
       osapiFree(L7_CLI_WEB_COMPONENT_ID,temp->urlhook.srcurl);
      temp->urlhook.srcurl = L7_NULL;
     } 
     if(temp->next->urlhook.srcurl != L7_NULL)
     {
       temp->urlhook.srcurl = osapiStrDup(temp->next->urlhook.srcurl);
     } 
     temp->next = temp->next->next;
     temp->urlhook.func = temp->next->urlhook.func;
     if(deleteNode->urlhook.srcurl != L7_NULL)
     {
       osapiFree(L7_CLI_WEB_COMPONENT_ID,deleteNode->urlhook.srcurl);
     } 
     osapiFree(L7_CLI_WEB_COMPONENT_ID,deleteNode);
    }
    return L7_SUCCESS;
    }
   temp = temp->next;
  }
  /*
     if we reach here, it means there is no matching entry.return failure.
     it is upto the caller to act accordingly
  */
  EMWEB_ERROR(("\r\n No matching entry during deregistration"));
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  call the application specific function to get the new url 
            if the requested url has an associated function
*
*@param    context structure
*@param    source url on which the function needs to be called if matched
*@param    new url returned by the function call

*@returns  L7_SUCCESS or L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t ewaNewUrl(EwsContext context, char *url,char* dsturl)
{
  emwebUrlHook_t* temp = emwebUrlHookList;
  if(url == L7_NULL || context == L7_NULL || dsturl == L7_NULL)
  {
    return L7_FAILURE;
  }
 
  while(temp != L7_NULL)
  {
    /*First look for exact match then call the function handlers for null url*/
    if(temp->urlhook.srcurl != L7_NULL) 
    {
      if(strstr(url,temp->urlhook.srcurl) != NULL )
      {
        /*
         call the function and return the new url in dsturl
         */ 

        dsturl = temp->urlhook.func(context,url);
        return L7_SUCCESS;
      }
    }  
    temp = temp->next;
  }

  /*Now call handlers who have registered for null(all) urls*/
  temp = emwebUrlHookList;
  while(temp != L7_NULL)
  {
    /*First look for exact match then call the function handlers for null url*/
    if(temp->urlhook.srcurl == L7_NULL)
    {
      /*
        call the function and return the new url in dsturl
      */

      dsturl = temp->urlhook.func(context,url);
      return L7_SUCCESS;
    }

    temp = temp->next;
  }

  return L7_FAILURE;
}

void dumpRegisteredHooks()
{
 emwebUrlHook_t* temp = emwebUrlHookList;
 while(temp != NULL)
 {
   if(temp->urlhook.srcurl != NULL)
   {
     printf("\nurl is %s",temp->urlhook.srcurl);
   }
   printf("\r\n Function is %p",temp->urlhook.func);
   temp = temp->next;
 }
 printf("\r\nDone\r\n");
}

char *ewaURLMimeTypeGet (EwsContext context, char *path)
{
  emwebWa_t *ewap = emwebWorkAreaGetByConnection (context);
  char *cp = NULL;

  if ((ewap != NULL) && (ewap->downloadUrl[0] != 0))
  {
    return "application/unknown";
  }

  cp = strrchr (path, '.');
  if (!strcmp (".txt", cp == NULL ? "" : cp))
  {
    return "text/plain";
  }
  else if (!strcmp (".jpg", cp == NULL ? "" : cp))
  {
    return "image/jpeg";
  }
  else if (!strcmp (".png", cp == NULL ? "" : cp))
  {
    return "image/png";
  }
  else if (!strcmp (".gif", cp == NULL ? "" : cp))
  {
    return "image/gif";
  }
  else if (!strcmp (".html", cp == NULL ? "" : cp))
  {
    return "text/html";
  }
  else if (!strcmp (".htm", cp == NULL ? "" : cp))
  {
    return "text/html";
  }
  else if (!strcmp (".xml", cp == NULL ? "" : cp))
  {
    return "text/xml";
  }
  else if (!strcmp (".js", cp == NULL ? "" : cp))
  {
    return "text/javascript";
  }
  else if (!strcmp (".css", cp == NULL ? "" : cp))
  {
    return "text/css";
  }
  else if (!strcmp (".pdf", cp == NULL ? "" : cp))
  {
    return "applications/pdf";
  }
  else if (!strcmp (".ps", cp == NULL ? "" : cp))
  {
    return "applications/postscript";
  }
  return "application/octet-stream";
}

/*
 * ews_ctxt.c
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
 * EmWeb/Server Context access functions
 *
 */

#include <stdio.h>
#include "ews.h"
#include "ews_serv.h"
#include "ews_send.h"
#include "ews_sys.h"
#include "ews_form.h"
#include "ews_xml.h"
#include "ews_str.h"
#include "ews_ctxt.h"
#include "l7_common.h"
#include "osapi.h"    /* Delay 1sec - Hack to fix vrrp, dot1x, radius and dot1s */
#include <stdlib.h>



/*
 * REQUEST CONTEXT
 *
 * Each HTTP request received by EmWeb/Server is assigned a unique context.
 * The context structure contains all the information pertaining to the
 * request, as well as internal state information regarding the processing
 * state of the request.
 *
 * API functions are provided to give the application some visibility into
 * the current context state.
 */

/*
 * ewsContextNetHandle
 * Return the application-specific network handle provided to EmWeb/Server
 * by the application in ewsNetHTTPStart().
 */
EwaNetHandle
ewsContextNetHandle ( EwsContext context )
{
    return context->net_handle;
}

#if defined (EW_CONFIG_OPTION_DEMAND_LOADING) \
    || defined (EW_CONFIG_OPTION_CLONING)
/*
 * ewsContextDocumentHandle
 * Return the application-specific document handle provided to EmWeb/Server
 * by the application in ewsDocumentRegister() or ewsDocumentClone().
 */
EwaDocumentHandle
ewsContextDocumentHandle ( EwsContext context )
{
    if (context->doc_stack_tail != (EwsDocumentContextP) NULL)
      {
        return context->doc_stack_tail->document->doc_handle;
      }
    else
      {
        return EWA_DOCUMENT_HANDLE_NULL;
      }
}
#endif /* EW_CONFIG_OPTION_DEMAND_LOADING || EW_CONFIG_OPTION_CLONING */

#ifdef EW_CONFIG_OPTION_AUTH
/*
 * ewsContextAuthHandle
 * Return the application-specific authorization handle provided to
 * EmWeb/Server by the application in ewsAuthRegister().
 */
EwaAuthHandle
ewsContextAuthHandle ( EwsContext context )
{
    return context->auth_handle;
}
#endif /* EW_CONFIG_OPTION_AUTH */

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
/*
 * ewsContextAuthState
 *
 * Return the state of uthorization process
 *
 */
EwsAuthState
ewsContextAuthState ( EwsContext context )
{
    return context->auth_state;
}
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#ifdef EW_CONFIG_OPTION_AUTH
/*
 * ewsContextAuthUsername
 *
 * Puts user name (NULL terminate), as received in authentication header,
 * in application provided space.
 *
 * Returns:
 *    EWS_STATUS_OK - provided space contains user name
 *    EWS_STATUS_AUTH_INCOMPLETE - req not authenticated yet,
 *                                but provided space contains user name
 *    EWS_STATUS_BAD_STATE - user name not available yet, application
 *                          can check authentication state to see why
 *    EWS_STATUS_NOT_FOUND - user name not available
 *    EWS_STATUS_NO_RESOURCES - not enough space for user name
 *
 */
EwsStatus
ewsContextAuthUsername(EwsContext context, char *userp, uintf len)
{
  EwsStatus status;
# if defined (EW_CONFIG_OPTION_AUTH_DIGEST) \
  || defined (EW_CONFIG_OPTION_AUTH_MDIGEST)
  uintf name_len;
#else
# if defined (EW_CONFIG_OPTION_AUTH_MBASIC) \
  && defined (EW_CONFIG_OPTION_AUTH_MBASIC_DECODE)
  uintf name_len;
#endif
#endif

  switch (context->auth_state)
    {

# if defined (EW_CONFIG_OPTION_AUTH_DIGEST) \
  || defined (EW_CONFIG_OPTION_AUTH_MDIGEST)
  case ewsAuthStateOKDigest:
  case ewsAuthStateAuthenticateDigest:
    /*
     * get user
     */
    name_len = ewsStringLength(&context->authStateParams.digest.username);
    if ((name_len + 1) > len)
      {
         status = EWS_STATUS_NO_RESOURCES;
      }
    else
      {
        ewsStringCopyOut( userp
                         ,&context->authStateParams.digest.username
                         ,name_len );
        userp[name_len] = '\0';
        if (context->auth_state == ewsAuthStateOKDigest)
          {
            status = EWS_STATUS_OK;
          }
        else
          {
            status = EWS_STATUS_AUTH_INCOMPLETE;
          }
      }
    break;
# endif

# if defined (EW_CONFIG_OPTION_AUTH_MBASIC)
# if defined (EW_CONFIG_OPTION_AUTH_MBASIC_DECODE)
  case ewsAuthStateOKMBasic:
  case ewsAuthStateAuthenticateMBasic:
    EMWEB_STRLEN(name_len, context->authStateParams.basic.user);
    if ((name_len + 1) > len)
      {
        status = EWS_STATUS_NO_RESOURCES;
      }
    else
      {
        EMWEB_STRCPY(userp, context->authStateParams.basic.user);
        if (context->auth_state == ewsAuthStateOKMBasic)
          {
            status = EWS_STATUS_OK;
          }
        else
          {
            status = EWS_STATUS_AUTH_INCOMPLETE;
          }
      }
    break;
# else /* !EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */
  case ewsAuthStateOKMBasic:
  case ewsAuthStateAuthenticateMBasic:
#   ifdef EMWEB_SANITY
    EMWEB_ERROR(("ewsContextAuthUsername(): mbasic auth w/o decode - request for user name\n"));
#   endif /* EMWEB_SANITY */
    userp[0] = '\0';
    status = EWS_STATUS_BAD_STATE;
    break;
# endif
# endif

# if defined (EW_CONFIG_OPTION_AUTH_BASIC)
  case ewsAuthStateOKBasic:
#   ifdef EMWEB_SANITY
    EMWEB_ERROR(("ewsContextAuthUsername(): basic auth - request for user name\n"));
#   endif /* EMWEB_SANITY */
# endif
  case ewsAuthStateUnauthenticated:
  case ewsAuthStateUninitialized:
  default:
    userp[0] = '\0';
    status = EWS_STATUS_BAD_STATE;
    (void)len;
    break;
  };

  return(status);

}

/* Start LVL7_P07826 */

/*
 * ewsContextAuthPassword
 *
 * Puts password (NULL terminate), as received in authentication header,
 * in application provided space.
 *
 * Returns:
 *    EWS_STATUS_OK - provided space contains password
 *    EWS_STATUS_AUTH_INCOMPLETE - req not authenticated yet,
 *                                but provided space contains password
 *    EWS_STATUS_BAD_STATE - user name not available yet, application
 *                          can check authentication state to see why
 *    EWS_STATUS_NOT_FOUND - password not available
 *    EWS_STATUS_NO_RESOURCES - not enough space for password
 *
 */
EwsStatus
ewsContextAuthPassword(EwsContext context, char *pwd, uintf len)
{
  EwsStatus status;
# if defined (EW_CONFIG_OPTION_AUTH_MBASIC) \
  && defined (EW_CONFIG_OPTION_AUTH_MBASIC_DECODE)
  uintf pwd_len;
#endif

  switch (context->auth_state)
    {

# if defined (EW_CONFIG_OPTION_AUTH_MBASIC)
# if defined (EW_CONFIG_OPTION_AUTH_MBASIC_DECODE)
  case ewsAuthStateOKMBasic:
  case ewsAuthStateAuthenticateMBasic:
    EMWEB_STRLEN(pwd_len, context->authStateParams.basic.password);
    if ((pwd_len + 1) > len)
      {
        status = EWS_STATUS_NO_RESOURCES;
      }
    else
      {
        EMWEB_STRCPY(pwd, context->authStateParams.basic.password);
        if (context->auth_state == ewsAuthStateOKMBasic)
          {
            status = EWS_STATUS_OK;
          }
        else
          {
            status = EWS_STATUS_AUTH_INCOMPLETE;
          }
      }
    break;
# else /* !EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */
  case ewsAuthStateOKMBasic:
  case ewsAuthStateAuthenticateMBasic:
#   ifdef EMWEB_SANITY
    EMWEB_ERROR(("ewsContextAuthUsername(): mbasic auth w/o decode - request for password\n"));
#   endif /* EMWEB_SANITY */
    pwd[0] = '\0';
    status = EWS_STATUS_BAD_STATE;
    break;
# endif
# endif

# if defined (EW_CONFIG_OPTION_AUTH_BASIC)
  case ewsAuthStateOKBasic:
#   ifdef EMWEB_SANITY
    EMWEB_ERROR(("ewsContextAuthUsername(): basic auth - request for password\n"));
#   endif /* EMWEB_SANITY */
# endif
  case ewsAuthStateUnauthenticated:
  case ewsAuthStateUninitialized:
  default:
    pwd[0] = '\0';
    status = EWS_STATUS_BAD_STATE;
    (void)len;
    break;
  };

  return(status);

}

/* End LVL7_P07826 */


#endif /* EW_CONFIG_OPTION_AUTH */

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
/*
 * ewsContextIsResuming
 * Return TRUE if EmWeb/Server is resuming (as a result of ewsResume()) after
 * the context was suspended (as a result of ewsSuspend()).
 */
boolean
ewsContextIsResuming ( EwsContext context )
{
    return (context->schedulingState == ewsContextResuming);
}
#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

#if defined (EW_CONFIG_OPTION_STRING) || defined (EW_CONFIG_OPTION_INCLUDE)\
   || defined (EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC)
/*
 * ewsContextIterations
 * Returns the iteration count corresponding for EMWEB_ITERATE
 */
uint32
ewsContextIterations ( EwsContext context )
{
#   ifdef EMWEB_SANITY
    if (context->doc_stack_tail == (EwsDocumentContextP) NULL)
      {
        EMWEB_WARN(("ewsContextIterations: not in string/include callout\n"));
        return 0;
      }
#   endif /* EMWEB_SANITY */

    return (context->doc_stack_tail->iterations);
}
#endif /*  EW_CONFIG_OPTION_STRING || EW_CONFIG_OPTION_INCLUDE
         || EW_CONFIG_OPTION_FIELDTYP_SELECT_DYNAMIC */

#if defined (EW_CONFIG_OPTION_FORM_REPEAT)
/*
 * ewsContextRepetition
 * Returns the Repetition count corresponding to EMWEB_REPEAT
 */
uint32
ewsContextRepetition ( EwsContext context )
{
#   ifdef EMWEB_SANITY
    if (context->doc_stack_tail == (EwsDocumentContextP) NULL)
      {
        EMWEB_WARN(("ewsContextRepetition: not in string/include callout\n"));
        return 0;
      }
#   endif /* EMWEB_SANITY */

    return (context->doc_stack_tail->rpt_body_iteration);
}
char *ewsContextPrefixGet(EwsContext context)
{
  sprintf(context->prefixBuf, "%lu.%lu.%u%s",
          context->doc_stack_tail->rpt_body_number,
          context->doc_stack_tail->rpt_body_iteration,
          context->doc_stack_tail->rpt_body_total_repeat,
          ewsContextGetValue(context));
  return context->prefixBuf;
}

char *ewsContextRepeatIndexModGet(EwsContext context)
{
  char *modstr = (char *)ewsContextGetValue(context);
  int tmpint = atoi(modstr);

  /* memset((void*)context->indexModBuf, 0x0, sizeof(context->indexModBuf)); */
  if(tmpint != 0)
  {
    sprintf(context->indexModBuf, "%d", (int)(context->doc_stack_tail->rpt_body_iteration % tmpint));
  }
  return context->indexModBuf;
}

#endif /*  EW_CONFIG_OPTION_FORM_REPEAT */

#ifdef EW_CONFIG_OPTION_ITERATE
/*
 * ewsContextIterator
 * Returns the current iterator for the current <EMWEB_ITERATE> tag
 *
 * NOW IMPLEMENTED AS A MACRO IN ../include/ews_ctxt.c
 */

/*
 * ewsContextIteratorN
 * Returns the current iterator for the Nth <EMWEB_ITERATE> tag,
 *   the innermost tag is 0, the next out is 1, etc
 * NULL if no iterator at the specified level.
 */
const void *
ewsContextIteratorN( EwsContext context, uintf nth )
{
  EwDocIterateContext *itag;

# ifdef EMWEB_SANITY
  if (context->doc_stack_tail == (EwsDocumentContextP) NULL)
    {
      EMWEB_WARN(("ewsContextIterator: not in <emweb_iterate>\n"));
      return NULL;
    }
# endif /* EMWEB_SANITY */

  /* Walk out the iteration stack */
  for ( itag = context->doc_stack_tail->iterate;
        ( itag != NULL ) && ( nth > 0 );
        nth--, itag = itag->stack
       )
    {
    }

  return ( itag != NULL ) ? itag->iterator : NULL;
}
#endif /* EW_CONFIG_OPTION_ITERATE */

/*
 * The following functions extract the corresponding fields from the HTTP/1.0
 * request header, if present, into an application-provided buffer.  Each of
 * these functions return the number of bytes in the actual header value, even
 * if this is larger than the application-provided buffer (though EmWeb/Server
 * will not overwrite the applicatoin buffer by honoring the buffer length).
 * If the header is not present, zero is returned.  The application may
 * specify a zero-length buffer in order to determine the size of the header
 * value.  The returned value begins with the first non-whitespace following
 * the HTTP header and ends just before the terminating end-of-line
 * character(s).
 */
#ifdef EW_CONFIG_OPTION_CONTEXT_DATE
uintf
ewsContextDate ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderDate]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderDate]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_DATE */
#ifdef EW_CONFIG_OPTION_CONTEXT_PRAGMA
uintf
ewsContextPragma ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderPragma]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderPragma]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_PRAGMA */
#ifdef EW_CONFIG_OPTION_CONTEXT_FROM
uintf
ewsContextFrom ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderFrom]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderFrom]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_FROM */
#ifdef EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE
uintf
ewsContextIfModifiedSince
  ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

  bytes = ewsStringLength
    (&context->req_headerp[ewsRequestHeaderIfModifiedSince]);
  if (length > bytes + 1)
    {
      length = bytes + 1;
    }
  if (length >= 1)
    {
      ewsStringCopyOut
        (datap
         ,&context->req_headerp[ewsRequestHeaderIfModifiedSince]
         ,length - 1);
    }
  return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE */
#ifdef EW_CONFIG_OPTION_CONTEXT_REFERER
uintf
ewsContextReferer ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderReferer]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderReferer]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_REFERER */
#ifdef EW_CONFIG_OPTION_CONTEXT_USER_AGENT
uintf
ewsContextUserAgent ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderUserAgent]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderUserAgent]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_USER_AGENT */

#ifdef EW_CONFIG_OPTION_CONTEXT_VIA
uintf
ewsContextVia ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

  bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderVia]);
  if (length > bytes + 1)
    {
      length = bytes + 1;
    }
  if (length >= 1)
    {
      ewsStringCopyOut
        (datap
         ,&context->req_headerp[ewsRequestHeaderVia]
         ,length - 1);
    }
  return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_VIA */

#ifdef EW_CONFIG_OPTION_CONTEXT_HOST
uintf
ewsContextHost ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_host);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_host
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_HOST */

#ifdef EW_CONFIG_OPTION_CONTEXT_SEND_REPLY
/*
 * ewsContextSendReply
 * This function serves a local URL from an unscheduled request context.
 * May be used in form submission or raw CGI script.
 *
 * context      - Request context
 * url          - URL redirect
 *
 * Returns EWS_STATUS_OK on success, else error code
 */
EwsStatus
ewsContextSendReply ( EwsContext context, const char * url )
{
  uintf hash;
  EwsDocument document;
  EwsStatus status;
  char *new_url = NULL;
  uintf base_bytes = 0;

  osapiSleepMSec(1000); /* Delay 1sec - Hack to fix vrrp, dot1x, radius and dot1s */

    if (url == NULL) url = ""; /* NULL URL for current directory */

#   ifdef EMWEB_SANITY
    if((TRUE

#       ifdef EW_CONFIG_OPTION_FORM
        && context->state != ewsContextStateSubmittingForm
#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_CGI
        && context->state != ewsContextStateStartingCGI
        && context->state != ewsContextStateServingCGI
#       endif /* EW_CONFIG_OPTION_CGI */

#       ifdef EW_CONFIG_OPTION_FILE_PUT
        && context->state != ewsContextStateParsingPut
#       endif /* EW_CONFIG_OPTION_FILE_PUT */

#       ifdef EW_CONFIG_OPTION_TELNET
        && context->state != ewsContextStateTelnet
#       endif /* EW_CONFIG_OPTION_TELNET */

#       ifdef EW_CONFIG_OPTION_SOAP
        && context->state != ewsContextStateServingSOAPResponse
#       endif /* EW_CONFIG_OPTION_SOAP */

#       ifdef EMWEB_XMLP
        && context->state != ewsContextStateXMLP
#       endif /* EMWEB_XMLP */

       )
#      ifdef EW_CONFIG_OPTION_CLIENT
       || context->client != NULL
#      endif /* EW_CONFIG_OPTION_CLIENT */
     )
      {
        return EWS_STATUS_BAD_STATE;
      }

    if ( EMWEB_STRCHR( url, '?' ) != NULL )
      {
        EMWEB_WARN(("ewsContextSendReply: query strings not supported\n"));
      }
#   endif /* EMWEB_SANITY */

    /*
     * Convert relative to absolute url
     */
    if (*url != '/')
      {
        char * base_url = context->url;
        uintf url_bytes;

          EMWEB_STRLEN(base_bytes, base_url);
          EMWEB_STRLEN(url_bytes, url);
          while (base_url[base_bytes - 1] != '/')
            {
              base_bytes--;
            }

          /*
           * If ACTION URL, strip ACTION
           */
          if ((context->flags & EWS_CONTEXT_FLAGS_ACTION_URL) != 0)
            {
              base_bytes--;
              while (base_url[base_bytes - 1] != '/')
                {
                  base_bytes--;
                }
            }

          new_url = (char *) ewaAlloc(base_bytes + url_bytes + 1);
          if (new_url == (char *)NULL)
            {
              EMWEB_WARN(("ewsContextSendReply: no resources for new url\n"));
              (void) ewsNetHTTPAbort(context);
              return(EWS_STATUS_NO_RESOURCES);
            }
          EMWEB_MEMCPY(new_url, base_url, base_bytes);
          EMWEB_STRCPY(new_url + base_bytes, url);
          new_url[base_bytes + url_bytes] = '\0';
          url = new_url;
      }

#   ifdef EW_CONFIG_FILE_METHODS
    if (context->fileParams != NULL)
      {
        document = ewsFileSetupFilesystemDoc( context, url );
      }
    else
#   endif /* EW_CONFIG_FILE_METHODS */
      {
        /*
         * Lookup URL in filesystem
         */
        hash = ewsFileHash(url);
        document = ewsFileHashLookup(hash, url);
      }

    if (document == EWS_DOCUMENT_NULL ||
        (document->flags & EWS_FILE_FLAG_LOADED) == 0)
      {
        EMWEB_ERROR(("ewsContextSendReply: document not found\n"));
        (void) ewsNetHTTPAbort(context);
        if (new_url != NULL)
          ewaFree(new_url);
        return EWS_STATUS_NOT_FOUND;
      }

    /*
     * Push document serving context onto request context state
     */
    status = ewsServeInclude(context, document);
    if (status != EWS_STATUS_OK)
      {
        EMWEB_WARN(("ewsContextSendReply: couldn't ServeInclude\n"));
        if (new_url != NULL)
          ewaFree(new_url);
        (void) ewsNetHTTPAbort(context);
        return status;
      }

#   ifdef EW_CONFIG_OPTION_TELNET
    if (context->state == ewsContextStateTelnet)
      {
        context->state = ewsContextStateServingDocument;
#       ifdef EW_CONFIG_OPTION_PERSISTENT
        context->keep_alive = TRUE;
#       endif /* EW_CONFIG_OPTION_PERSISTENT */
        return EWS_STATUS_OK;
      }
#   endif /* EW_CONFIG_OPTION_TELNET */

    (void) ewsSendHeader( context
                         ,document
#   ifdef EW_CONFIG_OPTION_CLIENT
                         ,context->override_status != NULL ?
                          context->override_status         : "200 OK"
#   else /* EW_CONFIG_OPTION_CLIENT */
                         ,"200 OK"
#   endif /* EW_CONFIG_OPTION_CLIENT */
                         , "\r\n" );
    if (new_url != NULL)        /* cannot access "url" now, either */
      ewaFree(new_url);

    /***********************************************************************
     *                Start serving document data                          *
     *  Transition to document serving state from CGI or form submit state *
     ***********************************************************************/

    context->state = ewsContextStateServingDocument;
    context->schedulingState = ewsContextScheduled;

#   ifdef EW_CONFIG_OPTION_CONVERT_XML
    /*
     * If this document is an XML conversion of an HTML
     * document, write out the declaration tag...
     */
    ewsServeXMLBeginConvert( context );
#   endif /* EW_CONFIG_OPTION_CONVERT_XML */


#   ifdef EW_CONFIG_OPTION_SCHED

    /*
     * Remove context from wait list and move to serve list
     */
    EWA_TASK_LOCK();
    EWS_LINK_DELETE(&context->link);
    EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
    EWA_TASK_UNLOCK();

#   else

#   ifdef EW_CONFIG_OPTION_FORM
    /*
     * If no scheduler and serving from submit function, free form
     */
    if (context->form != NULL)
      {
        ewsFormFree(context);
      }
#   endif

    /*
     * Serve the document and terminate request when done
     */
    while (ewsServeDocument(context) == EWA_STATUS_OK)
      {
        if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
          {
            break;
          }
      }

    ewsInternalFinish(context);

#   endif /* EW_CONFIG_OPTION_SCHED */

    return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REPLY */


#ifdef EW_CONFIG_FILE_METHODS
/*
 * ewsContextSetFile
 * This function is called by the application from either
 * ewaURLHook() if the requested URL should be mapped to the
 * local filesystem, or prior to calling ewsContextSendReply()
 * from a CGI or form submit function.  The server will now
 * map to the local filesystem instead of its internal archives.
 *
 * This may be used for HEAD, GET, PUT, and DELETE operations if
 * called from ewaURLHook() as follows:
 *
 * HEAD, GET:
 *   The application must initialize all fields.  The fileName is
 *   an application-defined opaque type which is passed back to the
 *   application in a possible subsequent call to ewaFileGet.
 *
 *   For HEAD method requests, the EmWeb/Server will simply use the
 *   parameters to generate a suitable response.
 *
 *   For GET method requests, the EmWeb/Server will pass the parameters
 *   back to the application in ewaFileGet() to open the file.  The
 *   file will then be read by one or more calls to ewaFileRead().
 *   Finally, ewaFileClose() will be called to terminate processing.
 *
 * PUT, POST:
 *   The application need only initialize the realm, allow, eTag, and
 *   lastModified fields.  However, they may all be NULL unless the
 *   file already exists.  If the file exists and no eTag is specified,
 *   eTag should be set to an empty string "" rather than NULL for
 *   proper operation of HTTP/1.1 conditions.
 *
 *   The EmWeb/Server will pass the parameters back to the application
 *   in ewaFilePut() to open the file for writing.  The file will then
 *   be written by one or more calls to ewaFileWrite().  Finally,
 *   ewaFileClose() will be called to terminate processing.
 *
 * DELETE:
 *   The application need only initialize the realm, allow, eTag, and
 *   lastModified fields.  However, they may all be NULL.  The
 *   EmWeb/Servver will pass the parameters back to the application
 *   by calling ewaFileDelete().
 *
 * context - input/output: the current context.  Setup the context to use an
 *              external file for the data source/sink rather than the archive
 * params - output: setup by the application, this structure contains the
 *          information necessary to correctly operate on the external file.
 *          The contents of this structure are copied by this routine and
 *          params is no longer referenced by the server after the call
 *          completes.
 *
 * Returns EWS_STATUS_OK on success, else an error code.
 */
EwsStatus
ewsContextSetFile ( EwsContext context
                   ,EwsFileParamsP params
#ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                   ,EwsFileSystemHandle fileSystemHandle
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                  )
{
# ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsFileSystemHandle handle;
# endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */


  EMWEB_TRACE(("ewsContextSetFile( %p, %p )\n", context, params));

# ifdef EMWEB_SANITY
  if(( context->state != ewsContextStateFindDocument
#     ifdef EW_CONFIG_OPTION_FORM
      && context->state != ewsContextStateSubmittingForm
#     endif /* EW_CONFIG_OPTION_FORM */

#     ifdef EW_CONFIG_OPTION_CGI
      && context->state != ewsContextStateStartingCGI
      && context->state != ewsContextStateServingCGI
#     endif /* EW_CONFIG_OPTION_CGI */

#     ifdef EW_CONFIG_OPTION_FILE_PUT
      && context->state != ewsContextStateParsingPut
#     endif /* EW_CONFIG_OPTION_FILE_PUT */
#     ifdef EW_CONFIG_OPTION_FILE_GET
      && context->state != ewsContextStateServingDocument
#     endif /* EW_CONFIG_OPTION_FILE_GET */
#     ifdef EW_CONFIG_OPTION_INCLUDE
      && context->state != ewsContextStateInclude
#     endif /* EW_CONFIG_OPTION_INCLUDE */
#     ifdef EMWEB_XMLP
      && context->state != ewsContextStateXMLP
#     endif /* EMWEB_XMLP */
     )
#    ifdef EW_CONFIG_OPTION_CLIENT
     || context->client != NULL
#    endif /* EW_CONFIG_OPTION_CLIENT */
    )
      {
        EMWEB_TRACE(("ewsContextSetFile: bad context state: 0x%X\n"
                     ,context->state)
                   );
        (void) ewsNetHTTPAbort(context);
        return EWS_STATUS_BAD_STATE;
      }
# endif /* EMWEB_SANITY */

  /* Hook the param structure temporarily into the context block,
   * move it to document structure later when we create one
   */

# ifdef EMWEB_SANITY
  if (context->fileParams != NULL)
    {
      EMWEB_ERROR(("ewsContextSetFile: file param context field in use\n"));
      (void) ewsNetHTTPAbort(context);
      return EWS_STATUS_BAD_STATE;
    }
# endif /* EMWEB_SANITY */

  context->fileParams = params;
# ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST

# ifdef EMWEB_SANITY
  /*
   * verify file system handle
   */
  if (EWS_LINK_IS_EMPTY(&(ews_state->file_system_list)))
    {
       EMWEB_ERROR(("ewsContextSetFile: no registered file system handles\n"));
       (void) ewsNetHTTPAbort(context);
       return EWS_STATUS_BAD_STATE;
    }
# endif /* EMWEB_SANITY */

  handle = (EwsFileSystemHandle)EWS_LINK_HEAD(ews_state->file_system_list);
  while (handle != fileSystemHandle)
    {
      /* keep going */
      handle = (EwsFileSystemHandle)(EWS_LINK_NEXT(handle->link));
      if (handle == (EwsFileSystemHandle)EWS_LINK_HEAD(ews_state->file_system_list))
         {
            /* we wrapped around */
            EMWEB_ERROR(("ewsContextSetFile: unregistered file system handle\n"));
            (void) ewsNetHTTPAbort(context);
            return EWS_STATUS_BAD_STATE;
          }
    }
  context->fileSystem = fileSystemHandle;
  context->fs_inuse = TRUE;
  (fileSystemHandle->use_cnt)++;
# endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
  return EWS_STATUS_OK;
}
#endif  /* EW_CONFIG_FILE_METHODS */

#ifdef EW_CONFIG_OPTION_XMLP
/*
 * ewsContextSetXMLP
 *
 * This function is called by the application's ewaURLHook() to redirect
 * the HTTP input stream to the XML parser.
 *
 * Params:
 *         context  - Request context
 *         consumer - Consumer handle from xmlpRegisterConsumer()
 *
 * Returned:
 *         EWS_STATUS_OK on success, else error code
 */
EwsStatus
ewsContextSetXMLP ( EwsContext context, XmlpConsumer consumer )
{
  context->state = ewsContextStateXMLP;
  context->xmlp_handle = ewxmlStart( consumer, context, NULL );
  context->req_str_begin = context->req_str_end;
  context->req_str_begin.length = 0; /* initialize to zero before parsing */

  if (context->xmlp_handle != NULL)
    {
      return EWS_STATUS_OK;
    }
  else
    {
      return EWS_STATUS_NO_RESOURCES;
    }
}
#endif /* EW_CONFIG_OPTION_XMLP */

#ifdef EW_CONFIG_OPTION_STRING_VALUE

/*
 * ewsContextGetValue
 *
 * This function is called by the application to retrive the string value
 * from the request header.
 *
 * Params:
 *         context - Request context
 *
 * Returned:
 *         pointer to the value string or NULL
 *
 */
const char *ewsContextGetValue(EwsContext context)
{
  return context->doc_stack_tail->string_value;
}
#endif  /* EW_CONFIG_OPTION_STRING_VALUE */

#ifdef EW_CONFIG_OPTION_COOKIES

/*
 * ewsContextCookieValue
 *
 * This function is called by the application to retrive specified cookie's value
 * from the request header.
 *
 * Params:
 *         name - pointer to the cookie name
 *
 * Returned:
 *         pointer to the value string or NULL
 *
 */

EW_NSC char *ewsContextCookieValue(EwsContext context, EW_NSC char *name)
{
  EwsCookieP cP;
  uintf len;
  boolean bvalue;

  /* if no cookies with this req, return        */
  if (context->cookie_list_first == NULL) return(NULL);
  EMWEB_STRLEN(len, name);
  if (len == 0) return(NULL);/* app supplied empty name         */

  cP = context->cookie_list_first;
  while (cP != (EwsCookieP)NULL)
    {
      EMWEB_STRCMP(bvalue, name, cP->nameP);
      if (!bvalue)
        return(cP->valueP);
      else
        cP = cP->next;
    }
  return(NULL);
}
#endif  /* EW_CONFIG_OPTION_COOKIES */


#ifdef EW_CONFIG_OPTION_CONTEXT_IF_MATCH
uintf
ewsContextIfMatch
( EwsContext context, char *datap, uintf length )
{
  uintf bytes=0;
  bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderIfMatch]);
  if (length > bytes + 1)
    {
      length = bytes + 1;
    }
  if (length >= 1)
    {
      ewsStringCopyOut
        (datap
         ,&context->req_headerp[ewsRequestHeaderIfMatch]
         ,length - 1);
    }
  return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_IF_MATCH */

#ifdef EW_CONFIG_OPTION_CONTEXT_IF_NONE_MATCH
uintf
ewsContextIfNoneMatch
( EwsContext context, char *datap, uintf length )
{
  uintf bytes=0;
  bytes = ewsStringLength
    (&context->req_headerp[ewsRequestHeaderIfNoneMatch]);
  if (length > bytes + 1)
    {
      length = bytes + 1;
    }
  if (length >= 1)
    {
      ewsStringCopyOut
        (datap
         ,&context->req_headerp[ewsRequestHeaderIfNoneMatch]
         ,length - 1);
    }
  return bytes;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_IF_NONE_MATCH */

#ifdef EW_CONFIG_OPTION_ACCEPT_LANG
uintf
ewsContextAcceptLanguage( EwsContext context, char *datap, uintf length )
{
  uintf bytes=0;
  bytes = ewsStringLength
    (&context->req_headerp[ewsRequestHeaderAcceptLanguage]);
  if (length > bytes + 1)
    {
      length = bytes + 1;
    }
  if (length >= 1)
    {
      ewsStringCopyOut
        (datap
         ,&context->req_headerp[ewsRequestHeaderAcceptLanguage]
         ,length - 1);
    }
  return bytes;
}
#endif /* EW_CONFIG_OPTION_ACCEPT_LANG */

#ifdef EW_CONFIG_OPTION_CONTEXT_REQUEST_METHOD
EwsRequestMethod
ewsContextRequestMethod( EwsContext context )
{
  return context->req_method;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_REQUEST_METHOD */

#ifdef EW_CONFIG_OPTION_SNMP_AGENT
EwaSNMPAgentHandle
ewsContextGetSNMPAgentHandle( EwsContext context )
{
  return context->snmpHandle;
}

void
ewsContextSetSNMPAgentHandle( EwsContext context
                             ,EwaSNMPAgentHandle handle )
{
  context->snmpHandle = handle;
}

#ifdef EW_CONFIG_OPTION_AUTH
EwaSNMPAgentAuthHandle
ewsContextGetSNMPAgentAuthHandle( EwsContext context )
{
  return context->snmpAuthHandle;
}
#endif /* EW_CONFIG_OPTION_AUTH */
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#if defined( EW_CONFIG_SCHEMA_ARCHIVE )
boolean
ewsContextSchemaInfo( EwsContext context
                      ,EwsSchemaOperation op
                      ,EwsSchemaInfo  *arg )
{
  const EwsDocumentNode *formatNode;
  uint32 indx;
  const EwsDocumentNode *schemaElements;
  const EwsDocumentNode *current;
  const uint8           *archiveData;

  EMWEB_TRACE(("ewsContextSchemaInfo: op=%d\n", op));

  current = context->currentElement;
  archiveData = context->archiveData;
  schemaElements = context->schemaNodes;

  if (current == NULL || schemaElements == NULL)  /* no element, no schema */
    {
      EMWEB_WARN(("ewsContextSchemaInfo: no schema data in archive.\n"));
      return FALSE;
    }

  /* the "translated data" is the object_id parameter passed to the ns get
   * function.  It is pointed to by the schema node's index field
   */
  if (op == ewsSchemaGetTranslation)
    {
      indx = EW_BYTES_TO_UINT32(current->index);
      arg->translation = (const char *)&archiveData[indx];
      return TRUE;
    }
  else if (op == ewsSchemaGetName)  /* name of object (eg: sysName) */
    {
      /* build up the index from the attributes/extend fields */

      indx = EW_DOCNODE_GET_3BYTE_ATTR( current );

      if (indx)        /* if name present (compiler option) */
        {
          arg->name = (const char *)&archiveData[indx];
          return TRUE;
        }
      return FALSE;
    }

  /*
   * all other schema attributes are based off of the "format" node that
   * is pointed to by the schema node's offset field.  If a namespace
   * does not have a schema (no mapping file), then the offset to the
   * format node is zero, and no schema attributes are available.
   */
  indx = EW_BYTES_TO_UINT32(current->data_offset) & 0x00ffffff;
  if (indx == 0)
    {
      EMWEB_WARN(("ewsContextSchemaInfo: no schema information available.\n"));
      return FALSE;         /* no schema */
    }

  formatNode = (const EwsDocumentNode *)(  ((const uint8 *)schemaElements)
                                           + (SIZEOF_EWS_DOCUMENT_NODE * (indx - 1))
                                           );

  switch (op)
    {
    case ewsSchemaGetAppTypeCode:
      arg->appTypeCode = EW_BYTES_TO_UINT32(formatNode->data_offset);
      break;
    case ewsSchemaGetRenderingCode:
      arg->renderingCode = (EwsSchemaRenderingCode) ewsField2Rendering
                             ((EwFieldType)formatNode->attributes);
      if (arg->renderingCode == (EwsSchemaRenderingCode) -1)
        {
          arg->renderingCode = ewsSchemaRenderHexString;
        }
      break;
    case ewsSchemaGetSyntax:
      indx = EW_BYTES_TO_UINT32(formatNode->index);
      arg->syntax = (const char *) &archiveData[indx];
      break;
    default:
      EMWEB_WARN(("ewsContextSchemaInfo: bad operation.\n"));
      return FALSE;
    }
  return TRUE;
}
#endif /* EW_CONFIG_SCHEMA_ARCHIVE */

#ifdef EW_CONFIG_OPTION_CONTEXT_SEND_ERROR
/*
 * ewsContextSendError
 * Abort a request with HTTP error code
 *
 *   context     - context of request
 *   status      - error message
 *
 * Returns EWS_STATUS_OK on success, or EWS_STATUS_BAD_STATE if too late to
 * send error (e.g. headers already sent).  The connection will automatically
 * be aborted.
 */
EwsStatus
ewsContextSendError ( EwsContext context
                     ,const char *status
                    )
{
  EwsStatus s = EWS_STATUS_OK;

  /*
   * If we already sent headers, all we can do now is abort
   */
  if (context->flags & EWS_CONTEXT_FLAGS_SENT_HEADER)
    {
      s = EWS_STATUS_BAD_STATE;
    }

  /*
   * Otherwise send headers and terminate request
   */
  else
    {
      ewsSendHeaderAndEnd(context
                         ,status
                         ,ews_no_body);
    }

  /*
   * Abort connection forcing close
   */
  ewsNetHTTPAbort(context);
  return s;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_ERROR */

#ifdef EW_CONFIG_OPTION_CONTEXT_SEND_REDIRECT
/*
 * ewsContextSendRedirect
 * Called from URL Hook function to redirect request to a different resource
 * using "Moved Permanently"
 *
 *   context     - context of request
 *   location    - Absolute URI (e.g. "/...") or absolute URL (e.g. "http://...)
 *
 * Returns EWS_STATUS_OK on success, or EWS_STATUS_BAD_STATE if not in URL hook
 */
EwsStatus
ewsContextSendRedirect ( EwsContext context
                        ,const char *location
                       )
{
  /*
   * Only allowed from URL Hook
   */
  if (context->state != ewsContextStateFindDocument)
    {
      return EWS_STATUS_BAD_STATE;
    }
  ewsSendRedirect(context
                 ,location
                 ,ewsRedirectPermanent);
  return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REDIRECT */

#ifdef EW_CONFIG_OPTION_CLIENT
/*
 * ewsContextStatusOverride
 *
 *   context     - context of request
 *   status_string - new status string (e.g. "200 OK")
 *
 * This function is provided to facilitate the development of an HTTP
 * gateway using EmWeb/Server together with EmWeb/Client.  This overrides
 * the value of the generated status code when sending response data back
 * to the server if and only if the response would have been "200".
 */
void
ewsContextStatusOverride ( EwsContext  context
                         , const char *status_string
                        )
{
  context->override_status = status_string;
}
#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
/*
 * ewsContextRange
 *
 *   context - context of request
 *   firstBytePosP - output: >=0: first byte pos.  <0: sufix (from end) pos.
 *   lastBytePosP  - output: last byte pos or EWS_CONTENT_LENGTH_UNKNOWN
 *
 * This function is provided in support of content byte ranges so that the
 * application can detect if an HTTP request was for a content range.
 *
 * Returns EWS_STATUS_OK if this is a request with a requested range, or
 * EWS_STATUS_NOT_FOUND if no range header was present.
 */
EwsStatus
ewsContextRange ( EwsContext context
                 ,int32 *firstBytePosP
                 ,int32 *lastBytePosP
                )
{
    if (context->isRange)
      {
        *firstBytePosP = context->firstBytePos;
        *lastBytePosP = context->lastBytePos;
        return EWS_STATUS_OK;
      }
    else
      {
        return EWS_STATUS_NOT_FOUND;
      }
}

/*
 * ewsContextSendRangeError
 *
 *   context        - context of request
 *   instanceLength - size of entity
 *
 * This function generates a "416 Requested range not satisfiable" error
 * including a "Content-Range: * / <instanceLength>" header.
 */
void
ewsContextSendRangeError(EwsContext context, uint32 instanceLength)
{
  context->instanceLength = instanceLength;
  ewsSendHeaderAndEnd(context, ews_http_status_416, ews_no_body);
}

#endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
/*
 * ewsContextCacheControl
 *
 *   May be used only in the code specified in EMWEB_ETAG
 *   to set the Cache-Control: max-age value
 */
EwsStatus
ewsContextCacheControl( EwsContext context, uintf maxage )
{
  EwsStatus status;

# ifdef EMWEB_SANITY
  if (  ( ewsContextStateServeSetup   != context->state )
      ||( ewsContextServeSetupDynEtag != context->substate.setup )
      )
    {
      EMWEB_ERROR(("ewsContextCacheControl called other than from EMWEB_ETAG code\n"));
      status = EWS_STATUS_BAD_STATE;
    }
  else
# endif /* EMWEB_SANITY */
    {
      context->max_age = maxage;
      status = EWS_STATUS_OK;
    }
  return status;
}
#endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */

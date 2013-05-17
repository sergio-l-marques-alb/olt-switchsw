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
 * EmWeb/Server request processing
 *
 */

#include "ewnet.h"
#include "ew_config.h"
#include "ews.h"
#include "ews_serv.h"
#include "ews_send.h"
#include "ews_sys.h"
#include "ews_secu.h"
#include "ews_form.h"
#include "ews_gway.h"
#include "ew_lib.h"
#include "ews_str.h"
#include "ews_urlhooks.h"
#include "ews_xml.h"
#include "ews_skel.h"
#include "ews_tah.h"
#include "ews_net.h"
#include "ews_ctxt.h"

#ifdef EW_CONFIG_OPTION_COMPRESS
#include "ew_cmp.h"
#endif /* EW_CONFIG_OPTION_COMPRESS */

#ifdef EW_CONFIG_OPTION_UPNP
#include "ews_upnp.h"
#endif /* EW_CONFIG_OPTION_UPNP */

#include "l7_common.h"
#include "usmdb_sim_api.h"
#include "usmdb_unitmgr_api.h"

#include "session.h"

/* ================================================================
 * Static Function Declarations
 * ================================================================ */

# ifdef EW_CONFIG_FILE_METHODS
EwaStatus
ewsServeLocalFile( EwsContext context, EwsDocumentContextP doc_context );
#endif /* EW_CONFIG_FILE_METHODS */

#ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
static EwsMatchStatus ewsSearchMatchHdr( const char*     eTag
                                        ,EwsString       string
                                        ,EwsContext      context
                                        );
static boolean ewsHandleMatchHdr( EwsContext   context
                                 ,EwsDocument  document
                                 ,EwsMatchType matchType
                                 );
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */

#ifdef EW_CONFIG_OPTION_METHOD_TRACE
static void ewsServeTrace ( EwsContext context );
#endif /* EW_CONFIG_OPTION_METHOD_TRACE */

boolean ewsServeConditionalCheck(EwsContext context, EwsDocument document);

#ifdef EW_CONFIG_OPTION_STRING_VALUE
static EwaStatus copyIntoStringValue(EwsDocumentContextP, const char *, int);
#endif

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
static void serveDynamicSelectHTML( EwsContext
                                   ,EwsDocumentContextP
                                   ,const EwFormField *
                                   ,const EwFormEntry *
                                   ,EwsFormSelectOption *
                                   ,EwFieldType
                                   );
#endif

#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
static boolean
ewsSelectVariant ( EwsContext context,
                   EwsDocument document,
                   const EwsDocumentHeader *doc_header);

typedef enum
{
  ewsQualTypeNone,
  ewsQualTypeDefinite,
  ewsQualTypeSpeculative
} ewsQualType;

#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA

static uint32
ewsCalcQual( char *attr,
             EwsAcceptHdrData *acc_hdr,
             ewsQualType *qual_type,
             EwsAttr attr_field);

static uint32
ewsQStrToInt ( char * q_str);

boolean
ewsCkWildcard(char *attr, EwsAcceptHdrData *acc_hdr, EwsAttr attr_field);

#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA */
#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */


static void
ewsServeDocumentData( EwsContext context
                      ,EwsDocumentContextP doc_context
                      ,uint32 chunk );

static EwaStatus
ewsServeDocumentNode( EwsContext context
                     ,EwsDocumentContextP doc_context);

#ifdef EW_CONFIG_OPTION_ITERATE
static EwaStatus
ewsServeEmWebIterateStart ( EwsContext context
                            ,EwsDocumentContextP doc_context );

static EwaStatus
ewsServeEmWebIterateEnd ( EwsContext context
                          ,EwsDocumentContextP doc_context );
#endif /* EW_CONFIG_OPTION_ITERATE */

#ifdef EW_CONFIG_OPTION_IF
static EwaStatus
ewsServeEmWebIf ( EwsContext context
                       ,EwsDocumentContextP doc_context );

static EwaStatus
ewsServeEmWebEndIf ( EwsContext context
                     ,EwsDocumentContextP doc_context );
#endif /* EW_CONFIG_OPTION_IF */

#ifdef    EW_CONFIG_OPTION_GROUP_DYNAMIC

#  define EWS_GROUP_DYNAMIC_CHECK(context, vp)        \
          {                                           \
            if (context->group_dynamic_count != 0)    \
              {                                       \
                ewaGroupDynamicFlush(context);        \
                vp = context->group_dynamic_ptr[0];   \
              }                                       \
          }

#  define EWS_GROUP_DYNAMIC_CLEANUP(context, vp)     \
          {                                          \
            if (context->group_dynamic_count != 0)   \
              {                                      \
                if (context->group_dynamic_flag[0])  \
                  {                                  \
                    ewaFree((void *)vp);             \
                  }                                  \
                context->group_dynamic_count = 0;    \
              }                                      \
          }

#else  /* EW_CONFIG_OPTION_GROUP_DYNAMIC */

#  define EWS_GROUP_DYNAMIC_CHECK(context, vp)
#  define EWS_GROUP_DYNAMIC_CLEANUP(context, vp)

#endif /* EW_CONFIG_OPTION_GROUP_DYNAMIC */

static EwaStatus
ewsServeEmWebConstString( EwsContext, EwsDocumentContextP );


/* ================================================================
 * External Interfaces
 * ================================================================ */


/*
 * ewsServeStart
 * This function takes a context that has successfully completed header
 * parsing and starts processing of the request.
 *
 * context      - Context of request
 *
 * Returns TRUE if ready to crank the state machine again, FALSE if more
 * data is needed (e.g. the body of a request for form submission or raw
 * CGI interfaces).
 */
boolean
ewsServeStart( EwsContext context )
{
  char *url;
  uintf bytes;
  EwsString temp_string;

  EW_UNUSED(temp_string);

#   ifdef EW_CONFIG_OPTION_CLIENT
    /*
     * If this is a client context, we are here because the EmWeb HTTP
     * parser has reached the end of the response headers.  We must now
     * pass control back to the EmWeb/Client module for processing.
     */
    if (context->client != NULL)
      {
        context->state = ewsContextStateClient;
        return TRUE;
      }
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   ifdef EW_CONFIG_OPTION_UPNP
#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
    /*
     * If finished parsing headers of M-SEARCH response, pass control back
     * to UPnP to parse out information from HTTP headers and clean up.
     */
    if (context->isMsearchResponse)
      {
        ewuSsdpProcessMsearchResponse(context);
        ewsNetHTTPAbort(context);
        return FALSE;
      }
#   endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */
#   endif /* EW_CONFIG_OPTION_UPNP */

    EMWEB_TRACE(("ewsServeStart(%p)\n", context));

    /*
     * HTTP Request Major Version > 1 should be rejected
     * because the major version bump allows incompatible changes
     */
    if ( context->req_protocol_maj > 1 )
      {
        ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
        ewsSendHeaderAndEnd(context
                            ,ews_http_status_505
                            ,ews_http_string_505);
        return FALSE;
      }

#   ifdef EW_CONFIG_OPTION_PRIMARY
    /*
     * Special case:
     * If we are parsing the HTTP response from a secondary server...
     */
    if (context->is_secondary_context)
      {
        /*
         * If HTTP/0.9 request, skip ahead of header processing.  We
         * adjust the buffer chain so that there is no data up to
         * the message body.  (If 100 Continue response and downgrading,
         * we also have to skip ahead of header processing).
         */
        if (context->secondary->context_current->req_protocol_maj == 0 ||
            (context->secondary->context_current->downgrade &&
             context->res_status == 100))
          {
            EwaNetBuffer buffer;

              for (buffer = context->req_buf_first;
                   buffer != ewsStringBuffer(&context->req_str_end);
                   buffer = ewaNetBufferNextGet(buffer))
                {
                  ewaNetBufferLengthSet(buffer, 0);
                }
              bytes = context->req_str_end.offset;
              ewaNetBufferLengthSet(buffer
                                   ,ewaNetBufferLengthGet(buffer) - bytes);
              ewaNetBufferDataSet(buffer,ewsStringData(&context->req_str_end));
              context->req_str_end.offset -= bytes;
          }

        /*
         * Otherwise, rewrite protocol version with our own, followed by
         * space, followed by the three-digit status code, followed by spaces
         * to pad any difference between the response protocol version length
         * and ours.  Then forward the status line and headers to the agent.
         */
        else
          {
            bytes = ewsStringLength(&context->req_protocol) - 3;
            ewsStringRewriteChar(&context->req_protocol, '1');
            ewsStringRewriteChar(&context->req_protocol, '.');
#           if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
            ewsStringRewriteChar(&context->req_protocol, '1');
#           else
            ewsStringRewriteChar(&context->req_protocol, '0');
#           endif
            ewsStringRewriteChar(&context->req_protocol, ' ');
            ewsStringRewriteChar(&context->req_protocol
                                ,'0' + ((context->res_status / 100) % 10));
            ewsStringRewriteChar(&context->req_protocol
                                ,'0' + ((context->res_status / 10) % 10));
            ewsStringRewriteChar(&context->req_protocol
                                ,'0' + ((context->res_status) % 10));
            while (bytes > 0)
              {
                ewsStringRewriteChar(&context->req_protocol, ' ');
                bytes--;
              }

            /*
             * Forward status line and headers to user agent
             */
            ewsDistForwardAgent(context, TRUE);

         }

#      if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
       /*
        * If 100 Continue response, then reset parser for next status line
        */
       if (context->res_status == 100)
         {
           context->state = ewsContextStateForwarding;
           context->substate.forwarding = ewsContextSubstateResponse;
           ewsStringLengthSet(&context->req_str_end, 0);
           context->req_str_begin = context->req_str_end;
           return FALSE;
         }
#      endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

       /*
        * If request was HEAD method, don't expect any body
        * If request was HEAD method or
        * had 1xx status (supported for HTTP 1.1 only) or
        * had 304 status or
        * had 204 staus (HTTP 1.1),
        * don't expect any body
        */
       if ((context->secondary->context_current->req_method
           == ewsRequestMethodHead) ||
           (context->res_status == 304)
#          if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
           || (context->res_status == 204)
           || ((context->res_status - (context->res_status%100))
              == 100)
#          endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
           )
         {
           context->content_remaining = 0;
#          if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
           context->chunked_encoding = FALSE;
#          endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
           ewsDistFinish(context->secondary, EWS_STATUS_OK);
           return FALSE;
         }

#      if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && \
          defined(EW_CONFIG_OPTION_PERSISTENT)
        /*
         * If downgrading response to HTTP/1.0 and request
         * used chunked encoding, then we must force connection close
         */
        if (context->secondary->context_current->downgrade &&
            context->chunked_encoding)
          {
            context->secondary->context_current->keep_alive = FALSE;
          }
#       endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && PERSISTENT */

        /*
         * If no Content-Length or Transfer-Encoding specified, set
         * expected content to (nearly) infinity.
         */
        if (ewsStringLength(
              &context->req_headerp[ewsRequestHeaderContentLength]) == 0
#             if EW_CONFIG_OPTION_HTTP_PROTOCOL >= HTTP_1_1
              && !context->chunked_encoding
#             endif
           )
          {
            context->content_remaining = (uint32) -1;
          }

        /*
         * Begin parsing and forwarding message body
         */
        context->state = ewsContextStateForwarding;
        context->substate.forwarding = ewsContextSubstateForwardBody;
        context->req_str_begin = context->req_str_end;
        return FALSE;
      }
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1

    /*
     * Requests from HTTP/1.1 clients without Host: field must be rejected.
     * (We will accept if Host: header isn't present, but specified in
     * the request URI with an absoluteURI, unless request == 1.1).
     */
    if ( ( (context->req_protocol_maj == 1 && context->req_protocol_min == 1)
         &&( ewsStringLength(&context->req_headerp[ewsRequestHeaderHost]) == 0)
         )
       ||( (context->req_protocol_maj > 1 ||
            (context->req_protocol_maj == 1 && context->req_protocol_min >= 1)
           )
           && ewsStringLength(&context->req_host) == 0
         )
       )
      {
            ewaLogHook(context, EWS_LOG_STATUS_BAD_REQUEST);
            ewsSendHeaderAndEnd(context
                               ,ews_http_status_400
                               ,ews_http_string_400);
            return FALSE;
      }
    /*
     * Unknown transfer encodings must be rejected
     */
    if (ewsStringLength(
          &context->req_headerp[ewsRequestHeaderTransferEncoding]) != 0
        && !context->chunked_encoding)
      {
        ewaLogHook(context, EWS_LOG_STATUS_NOT_IMPLEMENTED);
        ewsSendHeaderAndEnd(context
                           ,ews_http_status_501
                           ,ews_http_string_501);
        return FALSE;
      }

#   ifdef EW_CONFIG_OPTION_EXPECT
    /*
     * Unknown expectations must be rejected
     */
    if (context->expect_failed)
      {
        ewaLogHook(context, EWS_LOG_STATUS_NOT_IMPLEMENTED);
        ewsSendHeaderAndEnd(context
                           ,ews_http_status_417
                           ,ews_http_string_417);
        return FALSE;
      }
#   endif /* EW_CONFIG_OPTION_EXPECT */

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES
    /*
     * If unparsable byte ranges header, ignore it.
     */
    if (    context->isRange
         && context->lastBytePos != EWS_CONTENT_LENGTH_UNKNOWN
         && context->firstBytePos > context->lastBytePos
       )
      {
        context->isRange = FALSE;
      }
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */

    /*
     * Process "*" URL here
     */
    /* Make temporary copy of URL string, since a string match will cause
       the location pointer in temp_string to be advanced. */
    temp_string = context->req_url;
    if (   (    1 == ewsStringLength(&temp_string)
            && ewsStringCompare(&temp_string
                                ,"*"
                                ,ewsStringCompareCaseInsensitive
                                ,NULL)
            )
        /* We treat the slash-asterisk URL as equivalent to asterisk.
         * This is not required by the standards, but was done for
         * interoperability reasons with early HTTP/1.1 implementations.
         * Since it is harmless, we allow it to remain. */
        || (   2 == ewsStringLength(&temp_string)
            && ewsStringCompare(&temp_string
                                ,"/*"
                                ,ewsStringCompareCaseInsensitive
                                ,NULL)
            ))
      {
        switch (context->req_method)
          {
#           ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
            /*
             * For OPTIONS method:
             */
            case ewsRequestMethodOptions:
              ewaLogHook(context, EWS_LOG_STATUS_OK);
              ewsSendHeader(context
                           ,EWS_DOCUMENT_NULL
                           ,"200 OK"
                           ,"Public: HEAD, GET, "
#                           ifdef EW_CONFIG_OPTION_FORM
                            "POST, "
#                           endif
#                           ifdef EW_CONFIG_OPTION_FILE_PUT
                            "PUT, "
#                           endif
#                           ifdef EW_CONFIG_OPTION_FILE_DELETE
                            "DELETE, "
#                           endif
#                           ifdef EW_CONFIG_OPTION_METHOD_TRACE
                            "TRACE, "
#                           endif
#                           ifdef EW_CONFIG_OPTION_UPNP
                            "NOTIFY, M-SEARCH, SUBSCRIBE, UNSUBSCRIBE, "
#                           endif
                            "OPTIONS\r\n\r\n");
              ewsInternalFinish(context);
              return FALSE;
#           endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */

#           ifdef EW_CONFIG_OPTION_METHOD_TRACE
            case ewsRequestMethodTrace:
              ewsServeTrace(context);
              return FALSE;
#           endif /* EW_CONFIG_OPTION_METHOD_TRACE */

#           ifdef EW_CONFIG_OPTION_UPNP
            case ewsRequestMethodMSearch:
              return (ewuSsdpMSearch(context));
            case ewsRequestMethodNotify:
              /*
               * NOTIFYs with a '*' URI are SSDP announcements, and
               * are to be ignored unless SSDP_CP is configured
               */
#             ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
              return(ewuSsdpGatewayNotify(context));
#             else
              ewsInternalFinish(context);
              return FALSE;
#             endif
#           endif /* EW_CONFIG_OPTION_UPNP */

            /*
             * Unsupported method on "*" URI
             */
            default:
              ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
#             ifdef EW_CONFIG_OPTION_PERSISTENT
              context->keep_alive = FALSE;
#             endif /* EW_CONFIG_OPTION_PERSISTENT */
              ewsSendHeaderAndEnd(context
                                 ,ews_http_status_405
                                 ,ews_http_string_405);
              return FALSE;
          }
      }
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

#   ifdef EW_CONFIG_OPTION_UPNP
    /*    temp_string = context->req_url; */
    if ( context->req_method == ewsRequestMethodSubscribe )
      {
        ewuGenaSubscribe(context);
        return(FALSE);
      }
    if ( context->req_method == ewsRequestMethodUnsubscribe )
      {
        ewuGenaUnSubscribe(context);
        return(FALSE);
      }

#   endif /* EW_CONFIG_OPTION_UPNP */
    /*
     * Unescape URL and convert to contiguous character string
     */
    ewsStringUnescape(&context->req_url, ewsStringUnescapeNoPlus2Space);
    bytes = ewsStringLength(&context->req_url);
    if (ewsStringIsContiguous(&context->req_url))
      {
        context->url = url = (char *) ewsStringData(&context->req_url);
        url[bytes] = '\0';
      }
    else
      {
        context->url = url = (char *) ewaAlloc(bytes + 1);
        if (url == (char *) NULL)
          {
            ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
            EMWEB_WARN(("ewsServeStart: no resources for url\n"));
            ewsNetHTTPAbort(context);
            return FALSE;
          }
        context->url_dynamic = TRUE;
        ewsStringCopyOut(url, &context->req_url, bytes );
      }

    context->state = ewsContextStateFindDocument;
#   ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
    context->next_url_hook = NULL;
#   endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

#   ifndef EW_CONFIG_OPTION_SCHED

    /*
     * Without EmWeb scheduler, ewsServeFindDocument will run to completion
     * serving the entire response.  Normally, we would be done at this
     * point.  However, if HTTP/1.1 pipelining and persistent connections
     * are in use, it is possible that additional HTTP request data has been
     * received and not yet processed.  Therefore, return TRUE to force
     * the ewsParse() loop in ewsNetHTTPReceive() to continue processing any
     * remaining data, unless the context was aborted.
     */
    (void) ewsServeFindDocument( context );
    return (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)? FALSE : TRUE;

#   else /* !EW_CONFIG_OPTION_SCHED */

    /*
     * With EmWeb scheduler, return continuation flag to caller and let
     * ewsRun() reschedule as necessary
     */
    return ewsServeFindDocument( context );

#   endif /* !EW_CONFIG_OPTION_SCHED */

}


/*
 * ewsServeFindDocument
 * This function is the continuation of the ewsServeStart function.  It
 * is used to allow the the ewaURLHook call to suspend processing of
 * the context.  This function looks up the document associated with
 * the given URL
 *
 * context      - Context of request
 *
 * Returns TRUE if ready to crank the state machine again, FALSE if more
 * data is needed (e.g. the body of a request for form submission or raw
 * CGI interfaces).
 */
boolean
ewsServeFindDocument( EwsContext context )
{
  char *url;
  uintf bytes;
  EwsDocument document;
  uintf hash;
  boolean setupOk;



  /* If the HTTP Web Mode is disabled throw back a 404 */
  if (context->allow_http == FALSE)
  {
    #ifdef EW_CONFIG_OPTION_PERSISTENT
    context->keep_alive = FALSE;
    #endif /* EW_CONFIG_OPTION_PERSISTENT */
    ewaLogHook(context, EWS_LOG_STATUS_NOT_FOUND);
    ewsSendHeaderAndEnd(context ,ews_http_status_404 ,ews_http_string_404);
    return FALSE;
  }


  /*
   * Mark context for application cleanup since we are about to invoke
   * the external URL hook API.  This function is called from various EmWeb
   * modules (e.g. Client) which may not go through normal processing.
   */
  context->flags |= EWS_CONTEXT_FLAGS_CLEANUP;

  url = context->url;

# ifdef EW_OPTION_URL_HOOK
  {
    char *url2;
    /*
     * Invoke application URL call-out now that HTTP request headers have been
     * fully parsed.  Note that the call-out may alter the URL string in-place
     * (without increasing its length) before we perform a lookup in the file
     * system.  This gives the application an opportunity to change leading
     * characters into a sub-tree prefix based on request context.
     * (The application can also allocate memory, store a new URL into it, and
     * return a pointer to that.  Of course, it has to arrange to free the
     * memory during end-of-request cleanup.  But the server will immediately
     * copy the URL into memory it owns, because later processing may write
     * over the new URL, and the server doesn't have the right to write over
     * such application-allocated memory.)
     * The application returns NULL to request that we abort the request.  The
     * application can also suspend the context from this call.  In this case,
     * as in all suspends, the return value is ignored.
     */
#   ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
    /* ewsUrlHook will execute the URL hooks that remain to be executed. */
    url2 = ewsUrlHook(context, url);
#   else /* EW_CONFIG_OPTION_URL_HOOK */
    /* ewaURLHook is the application URL hook function. */
    url2 = ewaURLHook(context, url);
#   endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return FALSE;
      }
    /* reset state to scheduled (may be resuming) */
    context->schedulingState = ewsContextScheduled;
#   endif/* EW_CONFIG_OPTION_SCHED_SUSP_RES */

    /*
     * Check for abort or redirect case.  Since context->state is already set,
     * we need only return to the scheduler, and it will do everything
     * necessary.
     */
    if (   context->abort_flags & EWS_CONTEXT_FLAGS_ABORT
#       ifdef EW_CONFIG_OPTION_PERSISTENT
        || context->state == ewsContextStateSkipContent
#       endif /* EW_CONFIG_OPTION_PERSISTENT */
       )
      {
        return FALSE;
      }

    if (url2 == NULL)
      {
        /* The URL hook as requested an abort.  In this case, we need to call
         * ewsNetHTTPAbort to set context->state to indicate aborting.
         */
        ewsNetHTTPAbort(context);
        return FALSE;
      }

    /*
     * If application used different memory than the memory that stored the
     * original URL, then we must copy it for private server use.
     */
    url = ewsRelocateURL( context, url, url2 );
    if (url == NULL)
      {
        /* Failed to allocate memory needed to relocate URL.
         * (Context has already been aborted by ewsRelocateURL.) */
        return FALSE;
      }
  }
# endif /* EW_OPTION_URL_HOOK */
  /* Beyond this point, ewsServeFindDocument cannot suspened or reschedule
   * processing without setting a new context state.  Thus, there is no danger
   * that ewaURLHook will be executed twice for the same request.
   * (ewsURLHook uses the context substate to prevent multiple executions
   * of the application URL hook routines.)
   */

# ifdef EW_CONFIG_OPTION_METHOD_TRACE
  /*
   * Handle trace method
   */
  if (context->req_method == ewsRequestMethodTrace
#     ifdef EW_CONFIG_OPTION_PRIMARY
      &&(  context->secondary == EWS_SECONDARY_HANDLE_NULL
         ||context->max_forwards == 0
        )
#     endif
     )
    {
      ewsServeTrace(context);
      return FALSE;
    }
# endif /* EW_CONFIG_OPTION_METHOD_TRACE */

# ifdef EW_CONFIG_OPTION_PRIMARY
  /*
   * If the request is to be forwarded to a secondary
   */
  if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
    {
      EwsSecondaryHandle secondary = context->secondary;

      /*
       * Initialize state for forwarding current request to secondary
       */
      context->state = ewsContextStateForwarding;
      context->substate.forwarding = ewsContextSubstateForwardMethod;
      ewsStringExpand(&context->req_method_str, 1); /* include space */

      /*
       * If secondary is down or already processing a context, then
       * remove context from scheduler list and queue to the secondary
       * for future processing.
       */
      if (!secondary->opened || secondary->context_current != NULL)
        {
          context->state = ewsContextStateForwardingQueued;
          EWA_TASK_LOCK();
          EWS_LINK_DELETE(&context->link);
          EWS_LINK_INSERT(&secondary->context_list, &context->link);
          EWA_TASK_UNLOCK();
        }

      /*
       * Otherwise, return to scheduler to begin secondary processing
       */
      else
        {
          secondary->context_current = context;
        }
      return FALSE;
    }
# endif /* EW_CONFIG_OPTION_PRIMARY */

  EMWEB_STRLEN(bytes, url);

  /*
   * Remove terminating '/'
   * We do this after the above, so that if we are redirecting to
   * a secondary or responding to a trace, we don't modify the request.
   */
  if (bytes > 2 && url[bytes - 1] == '/')
    {
      bytes--;
      url[bytes] = '\0';
    }
# ifdef EW_CONFIG_OPTION_DIRECTORY_REDIRECT
  /*
   * Otherwise, remember that we didn't have a trailing '/'.  If this
   * turns out to be a directory URL without the trailing slash, we may
   * decide to force a redirect to the URL with a trailing slash.
   */
  else if (bytes > 2)
    {
      context->flags |= EWS_CONTEXT_FLAGS_URL_NO_SLASH;
    }
# endif /* EW_CONFIG_OPTION_DIRECTORY_REDIRECT */

#ifdef EW_CONFIG_OPTION_XMLP
  /*
   * If redirecting to the XML Parser, return to parser!
   */
  if (context->state == ewsContextStateXMLP)
    {
      return EWA_STATUS_OK;
    }
#endif /* EW_CONFIG_OPTION_XMLP */

  /*
   * If the URL corresponds to a file managed by the application, rather than
   * a document from the archive, then the ewaURLHook would have set the
   * context->fileParams field (via ewsContextSetFile()).  If this is the
   * case, then allocate a dummy document structure and dummy document
   * header and continue directly into document processing.
   */
# ifdef EW_CONFIG_FILE_METHODS
  if (context->fileParams != NULL)
    {
      if ((document = ewsFileSetupFilesystemDoc( context, context->url ))
          == NULL
         )
        {
          ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
          EMWEB_WARN(("ewsServeFindDocument: unable to create document\n"));
          ewsNetHTTPAbort(context);
          return EWA_STATUS_ERROR;
        }
      context->state = ewsContextStateServeSetup;
      context->substate.setup = ewsContextServeSetupBegin;
#     ifdef EW_CONFIG_OPTION_SCHED
      context->document = document;
#     endif /* EW_CONFIG_OPTION_SCHED */

#ifdef EW_CONFIG_OPTION_AUTH
      context->auth_state = ewsAuthStateUninitialized;
#endif
      return ewsServeSetup(context, document);
    }
# endif  /* EW_CONFIG_FILE_METHODS */

# ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION

  while (1)
    {
#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

      /*
       * Lookup URL in filesystem
       */
      hash = ewsFileHash(url);
      document = ewsFileHashLookup(hash, url);

#     if defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
      || defined( EW_CONFIG_OPTION_URL_PATH_INFO )
      /*
       * If document not found using the exact URL, it may be that
       * the browser is sending a URL that contains extra path information
       * past the document's path.  This extra path info can be used by
       * the application as a "parameter".  For example, assume /dir/doc
       * exists and accepts extra path info.  If the URL /dir/doc/info1/info2
       * is submitted, the lookup on this entire URL will fail.  We must
       * then "backup" the URL to check for a possible document.
       *
       * Work from the end of the URL string backward towards the beginning
       * looking for '/' directory seperators.  If found, test the URL path up
       * to that point. A match on a document that allows extra path info
       * indicates that anything after the '/' is "extra" path information.
       */
      if (document == EWS_DOCUMENT_NULL)
        {
          /*
           * Scan backward up to but not including the leading '/' character
           */
          for (bytes--; bytes > 0; bytes--)
            {
              /*
               * If we find a directory component separator
               */
              if (url[bytes] == '/')
                {
                  /*
                   * Temporarily terminate the string at this point and perform
                   * a lookup on the truncated URL.  Then, restore the '/' and
                   * keep searching until a URL is found.
                   */
                  url[bytes] = '\0';
                  hash = ewsFileHash(url);
                  document = ewsFileHashLookup(hash, url);
                  url[bytes] = '/';
                  if (document != EWS_DOCUMENT_NULL)
                    {
                      break;
                    }
                }
            }

          /*
           * If a URL was found from the truncated string, and this document
           * permits extra path info, then save the path information and truncate
           * the url name to the document.
           */
          if (document != EWS_DOCUMENT_NULL)
            {
              if (  document->doc_header->document_flags
                  & EW_ARCHIVE_DOC_FLAG_PATH_INFO
                 )
                {
                  url[bytes] = '\0';
                  context->path_info = url + bytes + 1;
                }
              else      /* bad! pathinfo not supported: bad URL */
                {
                  document = EWS_DOCUMENT_NULL;
                }
            }
        }
#     endif /* EW_CONFIG_OPTION_CGI/URL_PATH_INFO */

      /*
       * Document not found
       */
      if (document == EWS_DOCUMENT_NULL)
        {
#           ifdef EW_CONFIG_OPTION_PERSISTENT
          context->keep_alive = FALSE;
#           endif /* EW_CONFIG_OPTION_PERSISTENT */
          ewaLogHook(context, EWS_LOG_STATUS_NOT_FOUND);
          ewsSendHeaderAndEnd(context ,ews_http_status_404 ,ews_http_string_404);
          return FALSE;
        }

#       ifdef EW_CONFIG_OPTION_DEMAND_LOADING
      {
        EwsDocument basedoc;

#       ifdef EW_CONFIG_OPTION_CLONING
        /*
         * Find base document if cloned
         */
        if ((document->flags & EWS_FILE_FLAG_CLONE) != 0)
          {
            basedoc = document->clone_base;
          }
        else
#       endif /* EW_CONFIG_OPTION_CLONING */

          {
            basedoc = document;
          }

        /*
         * If document not loaded, generate a fault and link
         */
        if ((basedoc->flags & EWS_FILE_FLAG_LOADED) == 0)
          {
            EwaStatus status;
            EMWEB_TRACE(("ewsServeStart: document fault %p\n",
                         basedoc->doc_handle));

            /*
             * Remove context from wait list and move to fault list
             */
#           ifdef EW_CONFIG_OPTION_SCHED
            EWS_LINK_DELETE(&context->link);
#           endif /* EW_CONFIG_OPTION_SCHED */
            EWS_LINK_INSERT(&basedoc->fault_list, &context->link);
            context->state = ewsContextStateDocumentFault;
            status = ewaDocumentFault(context, basedoc->doc_handle);
            if (status != EWA_STATUS_OK)
              {
                ewaLogHook(context, EWS_LOG_STATUS_UNAVAILABLE);
#                   ifdef EW_CONFIG_OPTION_PERSISTENT
                context->keep_alive = FALSE;
#                   endif /* EW_CONFIG_OPTION_PERSISTENT */
                ewsSendHeaderAndEnd(context
                                    ,ews_http_status_503
                                    ,ews_http_string_503);
              }
            return FALSE;
          }
      }
#       endif /* EW_CONFIG_OPTION_DEMAND_LOADING */

      /*
       * If document hidden (and not a clone), return not found
       */
      if ((document->doc_header->document_type & EW_ARCHIVE_DOC_FLAG_HIDDEN) != 0

#           ifdef EW_CONFIG_OPTION_CLONING
          && (document->flags & EWS_FILE_FLAG_CLONE) == 0
#           endif /* EW_CONFIG_OPTION_CLONING */
#           ifdef EW_CONFIG_OPTION_SMTP
          && context->smtp == NULL /* and not SMTP mail */
#           endif /* EW_CONFIG_OPTION_SMTP */
#           ifdef EW_CONFIG_OPTION_TELNET
          && context->telnet == NULL /* and not telnet */
#           endif /* EW_CONFIG_OPTION_TELNET */
#           ifdef EW_CONFIG_OPTION_CLIENT
          && context->client == NULL /* and not client */
#           endif /* EW_CONFIG_OPTION_CLIENT */

         )
        {
#           ifdef EW_CONFIG_OPTION_PERSISTENT
          context->keep_alive = FALSE;
#           endif /* EW_CONFIG_OPTION_PERSISTENT */
          ewaLogHook(context, EWS_LOG_STATUS_NOT_FOUND);
          ewsSendHeaderAndEnd(context ,ews_http_status_404 ,ews_http_string_404);
          return FALSE;
        }

      /*
       * We have a valid loaded document, so set up an appropriate response.
       * Move to the serve setup state.
       */
      context->state = ewsContextStateServeSetup;
      context->substate.setup = ewsContextServeSetupBegin;
#     ifdef EW_CONFIG_OPTION_SCHED
      context->document = document;       /* save in case we're resumed */
#     endif /* EW_CONFIG_OPTION_SCHED */

#ifdef EW_CONFIG_OPTION_AUTH
      context->auth_state = ewsAuthStateUninitialized;
#endif
      setupOk = ewsServeSetup(context, document);

#     ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
      /*
       * if content negotiation changed the response document,
       * go back to look it up in the filesystem.
       */
      if (context->select_state == ewsDocSelectStateFindDocument)
        {
          context->select_state = ewsDocSelectStateFinish;
          url = context->url;
        }
      else
        {
          /* break out to continue serving doc */
          break;
        }
    } /* end while (1) */
# endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */
  return setupOk;
}

/*
 * ewsServeSetup
 * Setup state for processing request after document identified and loaded
 *
 * context      - context of request
 * document     - requested document
 *
 * Returns TRUE if ready to crank the state machine again, FALSE if more
 * data is needed (e.g. the body of a request for form submission or raw
 * CGI interfaces).
 */
boolean
ewsServeSetup ( EwsContext context, EwsDocument document )
{
  const EwsDocumentHeader *doc_header = document->doc_header;
  uint32 doc_type = doc_header->document_type & EW_ARCHIVE_DOC_TYPE_MASK;
  EwsStatus     send_status;
#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
	boolean 	  send_300_MC = FALSE;
#endif
  L7_uint32   realmID;
  L7_BOOL     bAllowAccess = L7_FALSE;
  L7_uint32   unit;
  L7_uint32   web_mode;
  EwaNetHandle net=NULL;

    EMWEB_TRACE(("ewsServeSetup(%p, %p)\n", context, document));

    /*
     * Mark context for application cleanup since we are about to invoke
     * the external authentication APIs.  Note that we do this here because
     * this function is called from various EmWeb modules (e.g. Client)
     * which may not go through normal URL hook processing.
     */
    context->flags |= EWS_CONTEXT_FLAGS_CLEANUP;

    /*
     * If URL is directory with index, lookup the index document.
     */
    if (doc_type == EW_ARCHIVE_DOC_TYPE_INDEX)
      {
        char *url;
        uintf hash;

          /*
           * If not root URL '/', set flag indicating an index URL.
           */
          if (document->url[1] != '\0')
            {
              context->flags |= EWS_CONTEXT_FLAGS_INDEX_URL;
            }

          /*
           * Lookup index file
           */
          url = (char *)
            &document->archive_data[EW_BYTES_TO_UINT32(doc_header->data_offset)];

#         ifdef EW_CONFIG_OPTION_DIRECTORY_REDIRECT
          /*
           * If the URL did not have a trailing '/', then force a redirect to
           * a new URL with the trailing '/'.  Not doing this causes relative
           * links to be interpreted with the wrong base.
           */
          if (context->flags & EWS_CONTEXT_FLAGS_URL_NO_SLASH)
            {
              ewaLogHook(context, EWS_LOG_STATUS_OK);
              ewsSendRedirect
                ( context
                 ,url
                 ,ewsRedirectPermanent
                );
              return FALSE;
            }
#         endif /* EW_CONFIG_OPTION_DIRECTORY_REDIRECT */

          hash = ewsFileHash(url);
          document = ewsFileHashLookup(hash, url);

#         ifdef EMWEB_SANITY
          if (document == EWS_DOCUMENT_NULL)
            {
              ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
              EMWEB_WARN(("ewsServeSetup: index not found\n"));
              ewsSendHeaderAndEnd(context
                                  ,ews_http_status_500
                                  ,ews_http_string_500);
              return FALSE;
            }
#         endif /* EMWEB_SANITY */

          doc_header = document->doc_header;
          doc_type = doc_header->document_type & EW_ARCHIVE_DOC_TYPE_MASK;

#         ifdef EMWEB_SANITY
          if (doc_type != EW_ARCHIVE_DOC_TYPE_MIME)
            {
              ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
              EMWEB_WARN(("ewsServeSetup: index wrong type\n"));
              ewsSendHeaderAndEnd(context
                                 ,ews_http_status_500
                                 ,ews_http_string_500);
              return FALSE;
            }
#         endif /* EMWEB_SANITY */
      }

    net = ewsContextNetHandle(context);

    context->allow_http = TRUE;
    if (usmDbUnitMgrMgrNumberGet(&unit) == L7_SUCCESS)
    {
      if (usmDbSwDevCtrlWebMgmtModeGet(unit, &web_mode) == L7_SUCCESS)
      {
        if (web_mode != L7_ENABLE)
        {
          if ((net == NULL) || (net->connection_type != EW_CONNECTION_HTTPS))
            context->allow_http = FALSE;
        }
      }
    }

    /* if (cliIsCurrUnitMgmtUnit() != L7_TRUE)
    {
      return FALSE;
    }*/

    if (document->realmp != NULL)
    {
      if (strcmp(document->realmp->realm, "READWRITE") == 0)
        realmID = L7_LOGIN_ACCESS_READ_WRITE;
      else if (strcmp(document->realmp->realm, "READONLY") == 0)
        realmID = L7_LOGIN_ACCESS_READ_ONLY;
      else
        realmID = L7_LOGIN_ACCESS_NONE;

      if (context->session == NULL)
      {
        bAllowAccess = L7_FALSE;
      }
      else
      {
        switch (context->session->access_level)
        {
          case L7_LOGIN_ACCESS_READ_WRITE:  /* READWRITE users get access to it all */
            if ((realmID == L7_LOGIN_ACCESS_READ_WRITE) || (realmID == L7_LOGIN_ACCESS_READ_ONLY))
              bAllowAccess = L7_TRUE;
            else
              bAllowAccess = L7_FALSE;
            break;
          case L7_LOGIN_ACCESS_READ_ONLY:
            if (realmID == L7_LOGIN_ACCESS_READ_ONLY)
              bAllowAccess = L7_TRUE;
            else
              bAllowAccess = L7_FALSE;
            break;
          case L7_LOGIN_ACCESS_NONE:
            bAllowAccess = L7_FALSE;
            break;
          default:
            bAllowAccess = L7_FALSE;
            break;
        }
      }

      if (bAllowAccess == L7_FALSE)
      {
        /* redirect to login page */

        ewsSendRedirect(context,"/", ewsRedirectTemporary);
        return FALSE;
      }

      context->auth_state = ewsAuthStateUnauthenticated;
      context->substate.setup = ewsContextServeSetupAuthDone;
    }


#   ifdef EW_CONFIG_OPTION_AUTH
    /*
     * Authorization: Get document's security realm
     */
    if (   context->substate.setup < ewsContextServeSetupAuthDone
        && document->realmp != (EwsRealmP) NULL
#       ifdef EW_CONFIG_OPTION_SMTP
        && context->smtp == NULL /* unless sending URL through mailer */
#       endif /* EW_CONFIG_OPTION_SMTP */
#       ifdef EW_CONFIG_OPTION_TELNET
        && context->telnet == NULL /* unless sending URL through telnet */
#       endif /* EW_CONFIG_OPTION_TELNET */
#       ifdef EW_CONFIG_OPTION_CLIENT
        && context->client == NULL /* unless sending URL through client */
#       endif /* EW_CONFIG_OPTION_CLIENT */
       )
      {
        EwsStatus status;
        /*
         * If request context does not contain proper credentials to access
         * this realm, then generate an Unauthorized header with a challenge.
         *
         * If the context is aborted or suspended, handle it here.
         */
        status = ewsAuthAuthenticate(context, document->realmp);

        if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
          return FALSE;

#       ifdef EW_CONFIG_AUTH_SCHED                     /* context suspended */
        if (context->schedulingState == ewsContextSuspended)
          return FALSE;
#       endif

        if (status == EWS_STATUS_BAD_REQUEST)
          {
            /*
             * client did not provide enough data to authorize, bad request
             */
            ewsSendHeaderAndEnd(context
                                ,ews_http_status_400
                                ,ews_http_string_400);
            return FALSE;
          }
        if (status == EWS_STATUS_NO_RESOURCES)
          {
            ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
            EMWEB_WARN(("ewsServeSetup: unable to authenticate\n"));
            ewsNetHTTPAbort(context);
            return FALSE;
          }
        if ((status == EWS_STATUS_AUTH_CHALLENGE) ||
            (status == EWS_STATUS_AUTH_FAILED))
          {
            if (ewsSendHeader(context
                             ,EWS_DOCUMENT_NULL
                             ,ews_http_status_401
                             ,ews_http_string_401) != EWS_STATUS_OK)
               {
                 return FALSE;
               }
            /*
             * user authorized for realm, challenge user for digest
             */
            status = ewsAuthChallenge(document->realmp, context);

            if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
              return FALSE;

#           ifdef EW_CONFIG_AUTH_SCHED                   /* context suspended */
            if (context->schedulingState == ewsContextSuspended)
              return FALSE;
#           endif

            if (status == EWS_STATUS_OK)
              {
                ewsSendStatusString(context, ews_http_string_401, NULL);
                return FALSE;
              }
            else
              {
                (void) ewsStringCopyIn( &context->res_str_end
                                       ,"\r\ndigest auth requires HTTP 1.1 and up\r\n"
                                      );
              }
            ewsInternalFinish(context);
            return FALSE;
          }
      }
#   endif /* EW_CONFIG_OPTION_AUTH */

    /*
     * Check for unimplemented method
     */
    if (   context->req_method == ewsRequestMethodUnknown
        && doc_type != EW_ARCHIVE_DOC_TYPE_CGI
        && doc_type != EW_ARCHIVE_DOC_TYPE_XMLP)
      {
        ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
        ewsSendHeaderAndEnd(context
                           ,ews_http_status_405
                           ,ews_http_string_405);
        return FALSE;
      }

#   ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
    /*
     * Check if document is a negotiable resource.
     * context->select_state is set to ewsDocSelectStateFinish when negotiation
     * is finished. Need this when result of negotiation is the original url
     * requested.
     */
    if (context->select_state == ewsDocSelectStateStart)
      {
        /* if doc has variants flag set, it's a negotiable resource */
        if (doc_header->hdr_flags & EW_ARCHIVE_DOC_HDR_VARIANTS_FLG)
          {
            /* start negotiation process */
            if ( ewsSelectVariant(context, document, doc_header) ==
                                              ewsDocSelectStateFindDocument)
              {
                return FALSE;   /* go back to look up new document */
              }
          }
      }
#   endif  /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

#   if defined(EW_CONFIG_OPTION_CONTEXT_SEND_REPLY) &&\
       defined(EW_CONFIG_OPTION_FORM)

    /*
     * Special case handling:  We want application developers to be able
     * to call ewsContextSendReply() from an ewaFormSubmit_*() function
     * specifying a relative URL.  If the <FORM> tag specified an ACTION
     * attribute, then the URL of the submission is one level below the
     * form's document in the URL hierarchy.  This causes relative URLs
     * to fail.
     *
     * To solve this problem, we explicitely set a flag for FORM ACTION
     * URLs.  In ewsContextSendReply(), we check the flag and strip off
     * the ACTION part of the URL if present.
     */
    if (doc_type == EW_ARCHIVE_DOC_TYPE_FORM)
      {
        context->flags |= EWS_CONTEXT_FLAGS_ACTION_URL;
      }
    else
      {
        context->flags &= ~EWS_CONTEXT_FLAGS_ACTION_URL;
      }
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REPLY | FORM */

    /*
     * Handle by document type.  Special cases are handled here.  The
     * general case of serving a document with or without nodes is handled
     * below this switch().
     */
    switch (doc_type)
      {
#       ifdef EW_CONFIG_OPTION_LINK
        /*********************************************************************/
        /* LINK:
         * Generate a "Moved Temporarily" response specifying the new link and
         * terminate request processing.
         */
        case EW_ARCHIVE_DOC_TYPE_LINK:
          ewaLogHook(context, EWS_LOG_STATUS_OK);
          ewsSendRedirect
            ( context
             ,(char *)&document->archive_data
                         [EW_BYTES_TO_UINT32 (doc_header->data_offset)]
             ,ewsRedirectPermanent
            );
          return FALSE;
#       endif /* EW_CONFIG_OPTION_LINK */

        /*********************************************************************/
        /* MIME:
         * Only method "GET" is supported (unless a form node is present).
         * If other method requested, generate a "Not Implemented" response.
         * Otherwise, break out of switch to serve document.  Note that we
         * ignore any query string for MIME documents.
         */
        case EW_ARCHIVE_DOC_TYPE_MIME:

          /*
           * If HEAD method, break out of switch and serve headers
           */
          if (context->req_method == ewsRequestMethodHead
#             ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
              || context->req_method == ewsRequestMethodOptions
#             endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */
             )
            {
              break;
            }

          /*
           * If GET method and no form node present or no query, break out
           * of switch and serve document.
           */
          if (context->req_method == ewsRequestMethodGet &&
              ((doc_header->document_type & EW_ARCHIVE_DOC_FLAG_FORM) == 0 ||
               ewsStringLength(&context->req_query) == 0))
            {
              break;
            }

#         ifdef EW_CONFIG_OPTION_FORM
          /*
           * If no form node present, then method not implemented, unless
           * GET with query string -- in which case we allow the GET to
           * serve the document, and the query string is available to
           * any application-provided dynamic content.
           */
          if ((doc_header->document_type & EW_ARCHIVE_DOC_FLAG_FORM) == 0)
#         endif /* EW_CONFIG_OPTION_FORM */

            {
              if (context->req_method == ewsRequestMethodGet)
                {
                  break;
                }
              ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
              ewsSendHeaderAndEnd(context
                                 ,ews_http_status_405
                                 ,ews_http_string_405);
              return FALSE;
            }

          /* fall through to next case */

#       ifdef EW_CONFIG_OPTION_FORM

        /*********************************************************************/
        /* FORM:
         * Handle form submission.  Method must be GET with query string,
         * or POST.  If HEAD method, break out.
         */
        case EW_ARCHIVE_DOC_TYPE_FORM:
          if (context->req_method == ewsRequestMethodHead
#             ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
              || context->req_method == ewsRequestMethodOptions
#             endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */
             )
            {
              break;
            }
          return ewsServeSetupForm(context, document);

#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_CGI

        /*********************************************************************/
        /* CGI:
         * Raw CGI interface.
         */
        case EW_ARCHIVE_DOC_TYPE_CGI:
#         ifdef EW_CONFIG_OPTION_PERSISTENT
          /*
           * Since CGI interface may generate arbitrary content, we can't
           * handle persistent connections in this case.
           */
          context->keep_alive = FALSE;
#         endif /* EW_CONFIG_OPTION_PERSISTENT */
          ewaLogHook(context, EWS_LOG_STATUS_OK);
          ewsServeSetupCGI(context, document);
          return FALSE;

#       endif /* EW_CONFIG_OPTION_CGI */

#       ifdef EMWEB_XMLP
        /*********************************************************************/
        /* XMLP:
         * XML Parser interface
         */
        case EW_ARCHIVE_DOC_TYPE_XMLP:
          /*
           * Check methods.  We handle HEAD and OPTIONS generically,
           * GET is not allowed.  Any other (including unknown) is okay.
           */
          if (context->req_method == ewsRequestMethodHead
#             ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
              || context->req_method == ewsRequestMethodOptions
#             endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */
             )
            {
              break;
            }
          if (context->req_method == ewsRequestMethodGet)
            {
              ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
              ewsSendHeaderAndEnd(context
                                 ,ews_http_status_405
                                 ,ews_http_string_405);
              return FALSE;
            }

          /*
           * Start XMLP
           */
          ewaLogHook(context, EWS_LOG_STATUS_OK);
          ewsServeSetupXMLP(context, document);
          return FALSE;

#       endif /* EMWEB_XMLP */


#       ifdef EW_CONFIG_OPTION_IMAGEMAP
        /*********************************************************************/
        /* IMAGEMAP:
         * Handle image map
         */
        case EW_ARCHIVE_DOC_TYPE_IMAGEMAP:
          if (context->req_method == ewsRequestMethodHead
#             ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
              || context->req_method == ewsRequestMethodOptions
#             endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */
             )
            {
              break;
            }
          ewaLogHook (context, EWS_LOG_STATUS_OK);
          ewsServeImageMap(context, document);
          return FALSE;
#       endif /* EW_CONFIG_OPTION_IMAGEMAP */


#       ifdef EW_CONFIG_FILE_METHODS
        /*********************************************************************/
        /* LOCAL FILESYSTEM:
         * This document is not from the archive.  Rather, it corresponds to
         * an entry in the local file system.   For now we only deal with
         * the GET method, so drop through to the serving code.
         */
        case EW_ARCHIVE_DOC_TYPE_FILE:
          /*
           * Check method
           */
          if ((  (  document->fileParams->fileInfo.allow
                  | ((document->fileParams->fileInfo.allow & ewsRequestMethodGet)
                     ? ewsRequestMethodHead : 0 )
#                 ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
                  | ewsRequestMethodOptions
#                 endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */
                 )
               & context->req_method) == 0)
            {
              ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
              ewsSendHeaderAndEnd(context
                                 ,ews_http_status_405
                                 ,ews_http_string_405);
              return FALSE;
            }
#         ifdef EW_CONFIG_OPTION_FILE_PUT
          /*
           * Handle PUT method
           */
          if (   context->req_method == ewsRequestMethodPut
              || context->req_method == ewsRequestMethodPost
#             ifdef EW_CONFIG_OPTION_UPNP /* let notify use filesystem PUT */
              || context->req_method == ewsRequestMethodNotify
#             endif /* EW_CONFIG_OPTION_UPNP */
             )
            {
#             if defined (EW_CONFIG_OPTION_CONDITIONAL_GET) ||\
                 defined (EW_CONFIG_OPTION_CONDITIONAL_MATCH)
              /*
               * Check conditionals
               */
              if (!ewsServeConditionalCheck(context, document))
                {
                  return FALSE;
                }
#             endif /* EW_CONFIG_OPTION_CONDITIONAL_GET | MATCH */

              /*
               * Open file
               */
#             ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
              if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
                {
                  if (context->fileSystem->fs.filePut == NULL)
                    {
                      EMWEB_ERROR(("ewsServSetup: NULL fs filePut function\n"));
                      context->fileHandle = EWA_FILE_HANDLE_NULL;
                    }
                  else
                    {
                      context->fileHandle = context->fileSystem->fs.filePut(
                                                         context
                                                        ,document->fileParams);
                    }
                }
              else
                {
                  context->fileHandle = EWA_FILE_HANDLE_NULL;
                }
#             else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
              context->fileHandle = ewaFilePut( context ,document->fileParams);
#             endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

              /*
               * Handle aborted or suspended state
               */
              if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                {
#                 ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                  (context->fileSystem->use_cnt--);
                  context->fs_inuse = FALSE;
#                 endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                  context->fileHandle = EWA_FILE_HANDLE_NULL;
                  return FALSE;
                }
#             ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
              if (context->schedulingState == ewsContextSuspended)
                {
#                 ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                  (context->fileSystem->use_cnt--);
                  context->fs_inuse = FALSE;
#                 endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                  context->fileHandle = EWA_FILE_HANDLE_NULL;
                  return FALSE;
                }
              /* reset state to scheduled (may be resuming) */
              context->schedulingState = ewsContextScheduled;
#             endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

              /*
               * If NULL handle was returned, generate internal error response
               */
              if (context->fileHandle == EWA_FILE_HANDLE_NULL)
                {
#                 ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                  if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
                    {
                      (context->fileSystem->use_cnt--);
                      context->fs_inuse = FALSE;
                    }
#                 endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                  ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
                  ewsSendHeaderAndEnd(context
                                     ,ews_http_status_500
                                     ,ews_http_string_500);
                  return FALSE;
                }

              /*
               * Parse message body as PUT data
               */
#             if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
              /*
               * PUT expects data.  Let HTTP/1.1 clients know that it is
               * okay to continue sending the request.
               */
              EMWEB_TRACE(("ewsServeSetup: calling ewsSendContinue from "
                           "%s:%d\n",
                           __FILE__, __LINE__));
              ewsSendContinue(context);
#             endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
              context->req_str_begin = context->req_str_end;
              context->state = ewsContextStateParsingPut;
              return FALSE; /* this state is done */
            }
          else

#         endif /* EW_CONFIG_OPTION_FILE_PUT */
#         ifdef EW_CONFIG_OPTION_FILE_DELETE

          /*
           * Handle DELETE method
           */
          if (context->req_method == ewsRequestMethodDelete)
            {
              EwaStatus status;

#             if defined (EW_CONFIG_OPTION_CONDITIONAL_GET) ||\
                 defined (EW_CONFIG_OPTION_CONDITIONAL_MATCH)
              /*
               * Check conditionals
               */
              if (!ewsServeConditionalCheck(context, document))
                {
                  return FALSE;
                }
#             endif /* EW_CONFIG_OPTION_CONDITIONAL_GET | MATCH */

              /*
               * Delete file
               */
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
              if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
                {
                  if (context->fileSystem->fs.fileDelete == NULL)
                    {
                      EMWEB_ERROR(("ewsServeSetup: NULL fs fileDelete function\n"));
                      status = EWA_STATUS_ERROR;
                    }
                  else
                    {
                      status = context->fileSystem->fs.fileDelete(context, document->fileParams);
                    }
                  (context->fileSystem->use_cnt)--;
                  context->fs_inuse = FALSE;
                }
              else
                {
                  status = EWA_STATUS_ERROR;
                }
#   else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
              status = ewaFileDelete(context, document->fileParams);
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

              /*
               * Handle aborted or suspended state
               */
              if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                {
                  return FALSE;
                }
#             ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
              if (context->schedulingState == ewsContextSuspended)
                {
                  return FALSE;
                }
              /* reset state to scheduled (may be resuming) */
              context->schedulingState = ewsContextScheduled;
#             endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

              if (status != EWA_STATUS_OK)
                {
                  ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
                  ewsSendHeaderAndEnd(context
                                     ,ews_http_status_500
                                     ,ews_http_string_500);
                }
              else
                {
                  ewaLogHook(context, EWS_LOG_STATUS_OK);
                  ewsSendHeaderAndEnd(context, "200 OK", ews_no_body);
                }
              return FALSE;
            }
#         endif /* EW_CONFIG_OPTION_FILE_DELETE */
          break;
#       endif  /* EW_CONFIG_FILE_METHODS */


#       ifdef EMWEB_SANITY
        /*********************************************************************/
        /* Unknown type */
        default:
          EMWEB_ERROR(("ewsServeSetup: unknown document type\n"));
          ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
          ewsSendHeaderAndEnd(context
                             ,ews_http_status_500
                             ,ews_http_string_500);
          return FALSE;
#       endif /* EMWEB_SANITY */

      }

    /*
     * Only GET, OPTIONS and HEAD methods are supported for serving documents
     */
    if (context->req_method != ewsRequestMethodGet
        && context->req_method != ewsRequestMethodHead
#       ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
        && context->req_method != ewsRequestMethodOptions
#       endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */
       )
      {
        ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
        ewsSendHeaderAndEnd(context
                           ,ews_http_status_405
                           ,ews_http_string_405);
        return FALSE;
      }

    ewaLogHook (context, EWS_LOG_STATUS_OK);

#   ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
    /*
     * Handle OPTIONS method
     */
    if (context->req_method == ewsRequestMethodOptions)
      {
        char    *allow;

#       ifdef EW_CONFIG_FILE_METHODS
        /*
         * If the document is actually from the local file system,
         * then use the information from the file parameters to
         * determine the allowed methods
         */
        if (doc_type == EW_ARCHIVE_DOC_TYPE_FILE)
          {
            char    buff[80], *ptr;
            uintf   len;

            allow = ptr = &buff[0];

#           ifdef EW_CONFIG_OPTION_METHOD_TRACE
            EMWEB_STRCPY( ptr, "Allow: OPTIONS, TRACE" );
#           else
            EMWEB_STRCPY( ptr, "Allow: OPTIONS" );
#           endif

            if (document->fileParams->fileInfo.allow & ewsRequestMethodGet)
              {
                EMWEB_STRLEN( len, ptr );
                ptr += len;
                EMWEB_STRCPY( ptr, ", HEAD, GET" );
              }

            /* POST may be used to file systems when they are really pipes... */
            if (document->fileParams->fileInfo.allow & ewsRequestMethodPost)
              {
                EMWEB_STRLEN( len, ptr );
                ptr += len;
                EMWEB_STRCPY( ptr, ", POST" );
              }

#           ifdef EW_CONFIG_OPTION_FILE_PUT
            if (document->fileParams->fileInfo.allow & ewsRequestMethodPut)
              {
                EMWEB_STRLEN( len, ptr );
                ptr += len;
                EMWEB_STRCPY( ptr, ", PUT" );
              }
#           endif /* EW_CONFIG_OPTION_FILE_PUT */

#           ifdef EW_CONFIG_OPTION_FILE_DELETE
            if (document->fileParams->fileInfo.allow & ewsRequestMethodDelete)
              {
                EMWEB_STRLEN( len, ptr );
                ptr += len;
                EMWEB_STRCPY( ptr, ", DELETE" );
              }
#           endif /* EW_CONFIG_OPTION_FILE_DELETE */

            EMWEB_STRLEN( len, ptr );
            ptr += len;
            EMWEB_STRCPY( ptr, "\r\n\r\n" );
          }
        else
#       endif /* EW_CONFIG_FILE_METHODS */
          {
            /*
             * else document is from archive, use document_type
             * to determine the allow header to use
             */
            allow = (char *)((doc_header->document_type & EW_ARCHIVE_DOC_FLAG_FORM)
                     ? "Allow: HEAD, GET, POST, OPTIONS\r\n\r\n"
                     : "Allow: HEAD, GET, OPTIONS\r\n\r\n");

          }
        ewsSendHeader( context, EWS_DOCUMENT_NULL, "200 OK", allow );
        ewsInternalFinish(context);
        return FALSE;
      }
#   endif /* EW_CONFIG_OPTION_METHOD_OPTIONS */


#   if defined (EW_CONFIG_OPTION_CONDITIONAL_GET) ||\
       defined (EW_CONFIG_OPTION_CONDITIONAL_MATCH)

    /*
     * If If-Modified-Since: header present and document contains no nodes
     * for dynamic content, then compare (we use strict equals which works on
     * most browsers - not the end of the world if it doesn't) the archive
     * creation date with the If-Modified-Since: field value.  If they are
     * equal, then so indicate by sending a Not Modified response.
     * GET method: Check precondition and serve
     */
    if (!ewsServeConditionalCheck(context, document))
      {
        return FALSE;
      }
#   endif /* EW_CONFIG_OPTION_CONDITIONAL_GET | MATCH */

    /*
     * Push document onto request context processing stack
     */
    if (ewsServeInclude(context, document) != EWS_STATUS_OK)
      {
        ewsNetHTTPAbort(context);
        return FALSE;
      }

    if (context->req_method == ewsRequestMethodHead)
      {
        /*
         * If "HEAD" method, just return header and close
         */
        ewsSendHeader(context, document, "200 OK ", ews_no_body);
        ewsInternalFinish(context);
        return FALSE;
      }
    /*
     * Send OK status and start serving document
     */

    /* send 300 Multiple Choices response status if we're config'd for
     * content negotiation _and_ we're sending a list-response _and_ the
     * requesting user-agent protocol is greater than HTTP/1.0. Some HTTP/1.0
     * user-agents have a problem with 300 Multiple Choices status.
     */
#   ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
		
	  if (context->req_protocol_maj == 1 && context->req_protocol_min >= 1)
	  {
		if (context->tcn_hdr == ewsTcnHdrList)
		  {
			send_300_MC = TRUE;
		  }
	  }
	  if (send_300_MC)
	  {
		send_status = ewsSendHeader(context, document, "300 Multiple Choices", "\r\n");
	  }
	  else
	  {
		send_status = ewsSendHeader(context
								   ,document
#                                  ifdef EW_CONFIG_OPTION_CLIENT
								   ,context->override_status != NULL ?
									context->override_status		 : "200 OK"
#                                  else /* EW_CONFIG_OPTION_CLIENT */
								   ,"200 OK"
#                                  endif /* EW_CONFIG_OPTION_CLIENT */

								   ,"\r\n");
	  }
#   else
		send_status = ewsSendHeader(context
								   ,document
#                                  ifdef EW_CONFIG_OPTION_CLIENT
								   ,context->override_status != NULL ?
									context->override_status		 : "200 OK"
#                                  else /* EW_CONFIG_OPTION_CLIENT */
								   ,"200 OK"
#                                  endif /* EW_CONFIG_OPTION_CLIENT */

								   ,"\r\n");
		
#   endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */


    if (send_status != EWS_STATUS_OK)
      {
        return FALSE;
      }


    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return FALSE;
      }
    /***********************************************************************
     *                Start serving document data                          *
     ***********************************************************************/

    context->state = ewsContextStateServingDocument;

#   ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
    /*
     * If this document is a TEXTAREA XML conversion of an HTML
     * document, write out the initial boilerplate...
     * This call must be before the XML and skeleton calls, because
     * the TEXTAREA envelops them.
     */
    ewsServeTextareaBeginConvert( context, document );
#   endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

#   ifdef EW_CONFIG_OPTION_CONVERT_XML
    /*
     * If this document is an XML conversion of an HTML
     * document, write out the declaration tag...
     */
    ewsServeXMLBeginConvert( context );
#   endif /* EW_CONFIG_OPTION_CONVERT_XML */

#   ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
    /*
     * If this document is a skeleton of an HTML
     * document, write out the declaration tag...
     */
    if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
      {
        ewsServeSkeletonBeginConvert( context );
      }
#   endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */

#   ifndef EW_CONFIG_OPTION_SCHED
    /*
     * Crank request processing state machine until done.  Then terminate
     * request.
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

    return FALSE;
}


/*
 * ewsServeInclude
 * This function pushes a new document onto the context processing stack.
 * The document must have already been determined to be loaded, of type
 * MIME or TEXT, etc.
 *
 * context      - request context
 * document     - document to serve
 */
EwsStatus
ewsServeInclude ( EwsContext context, EwsDocument document )
{
  const EwsDocumentHeader *doc_header = document->doc_header;
  EwsDocumentContextP doc_context;
  uint32 offset;

    /*
     * Mark context for application cleanup since we are about to invoke
     * the external dynamic content generation APIs.  Note that we do this
     * here because this function is called from various EmWeb modules
     * (e.g. Client) which may not go through normal URL hook processing.
     */
    context->flags |= EWS_CONTEXT_FLAGS_CLEANUP;

    /*
     * Allocate context for document
     */
    doc_context = (EwsDocumentContextP) ewaAlloc(sizeof(EwsDocumentContext));
    if (doc_context == (EwsDocumentContextP) NULL)
      {
        ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
        EMWEB_WARN(("ewsServeStart: no resources for doc context\n"));
        return EWS_STATUS_NO_RESOURCES;
      }

    /*
     * Initialize document context, based on whether or not the
     * document is in the archive, or in an application-managed
     * local file system
     */

    doc_context->context = context;
    doc_context->doc_headerp = doc_header;
    doc_context->offset = 0;
    doc_context->iterations = 0;

#   ifdef EW_CONFIG_OPTION_IF
    doc_context->if_true = FALSE;
    doc_context->if_depth = 0;
#   endif /* EW_CONFIG_OPTION_IF */

#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    doc_context->string_value = NULL;
    doc_context->string_buffer = NULL;
#   endif /* EW_CONFIG_OPTION_STRING_VALUE */

#   ifdef EW_CONFIG_OPTION_ITERATE
    doc_context->iterate = NULL;
#   endif /* EW_CONFIG_OPTION_ITERATE */

#   ifdef EW_CONFIG_FILE_METHODS
    doc_context->fileHandle = EWA_FILE_HANDLE_NULL;
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
    doc_context->fileSystem = EWS_FILE_SYSTEM_HANDLE_NULL;
    doc_context->fs_inuse = FALSE;
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

    if (document->flags & EWS_FILE_FLAG_FILESYSTEM)
      {
        doc_context->node_count = 0;
        doc_context->remaining = document->fileParams->fileInfo.contentLength;

        /* store the document into the doc-context.  The document structure
         * memory will now "belong" to the doc_context, and will be freed
         * when it is freed
         */
        EWA_TASK_LOCK();
        doc_context->document = document;
        context->dummy = NULL;
        EWA_TASK_UNLOCK();
      }
    else                /* document is from archive */
#   endif /* EW_CONFIG_FILE_METHODS */
      {
        doc_context->document = document;
        EWA_TASK_LOCK();
        document->archive->reference_count++;
        EWA_TASK_UNLOCK();
        offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
        doc_context->nodep = (const EwsDocumentNode *) &document->archive_data[offset];
        doc_context->node_count = EW_BYTES_TO_UINT16(doc_header->node_count);
        offset = EW_BYTES_TO_UINT32(doc_header->data_offset);
        doc_context->datap = (const uint8 *) &document->archive_data[offset];
        doc_context->remaining = EW_BYTES_TO_UINT32(doc_header->orig_length);

#       ifdef EW_CONFIG_OPTION_FORM
#       ifdef EW_CONFIG_OPTION_FORM_REPEAT
        doc_context->rpt_body_skip = FALSE;
#       endif /* EW_CONFIG_OPTION_FORM_REPEAT */
#       if defined(EW_CONFIG_OPTION_FIELDTYPE_RADIO) ||\
           defined(EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE)
        /*
         * Save archive form information in context in case EMWEB_STRING or
         * EMWEB_INCLUDE function wishes to access ewsFormEnumToString().
         * (We assume that the document is in the same archive!)
         */
        context->form_enum_table = document->archive->emweb_form_enum_table;
        context->form_enum_table_size=document->archive->emweb_form_enum_table_size;
#       endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO | SELECT_SINGLE */
#       endif /* EW_CONFIG_OPTION_FORM */
      }

#    ifdef EW_CONFIG_OPTION_COMPRESS
     if (doc_header->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
       {
         EwsArchiveHeaderP head = (EwsArchiveHeaderP) document->archive_data;
         offset = EW_BYTES_TO_UINT32(head->dict_offset);
         doc_context->compress_context = (EwDecompressContext *)
           ewDecompressBegin( &document->archive_data[offset]
                             , (void *) doc_context);
         if (doc_context->compress_context == (void *) NULL)
           {
             ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
             ewsFreeDocContext(doc_context);
             return EWS_STATUS_NO_RESOURCES;
           }
       }
     else
#    endif /* EW_CONFIG_OPTION_COMPRESS */
       {
         doc_context->compress_context = NULL;
       }

    /*
     * Push document context onto request context stack
     */
    doc_context->next = (EwsDocumentContextP) NULL;
    if (context->doc_stack_tail != (EwsDocumentContextP) NULL)
      {
        context->doc_stack_tail->next = doc_context;
      }
    else
      {
        context->doc_stack_head = doc_context;
        /*
         * First document on stack:
         *
         * If serving document with dynamic content, don't use persistent
         * connection unless chunked encoding allowed.
         */
        if (context->req_protocol_maj > 1 ||
            (context->req_protocol_maj == 1 && context->req_protocol_min >= 1))
          {
#           ifdef EW_CONFIG_OPTION_CHUNKED_OUT
            /*
             * If unknown length or digest, use chunked encoding
             */
            if (      ewsGetDocLength(context, document)
                   == EWS_CONTENT_LENGTH_UNKNOWN
#               ifdef EW_CONFIG_OPTION_AUTH_DIGEST
                || context->auth_state == ewsAuthStateOKDigest
#               ifdef EW_CONFIG_OPTION_AUTH_DIGEST_INT
                || context->auth_state == ewsAuthStateOKDigestIntegrity
#               endif /* EW_CONFIG_OPTION_AUTH_DIGEST_INT */
#               endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
               )
              {
                context->chunk_out_state = ewsContextChunkOutNewChunk;
              }
#           endif /* EW_CONFIG_OPTION_CHUNKED_OUT */
          }
#          ifdef EW_CONFIG_OPTION_PERSISTENT
           /*
            * If HEAD, or chunking, we can keep persistent
            */
           if (   context->req_method != ewsRequestMethodHead
               &&    ewsGetDocLength(context, document)
                  == EWS_CONTENT_LENGTH_UNKNOWN
#              ifdef EW_CONFIG_OPTION_CHUNKED_OUT
               && context->chunk_out_state != ewsContextChunkOutNewChunk
#              endif /* EW_CONFIG_OPTION_CHUNKED_OUT */
              )
             {
               context->keep_alive = FALSE;
             }
#         endif /* EW_CONFIG_OPTION_PERSISTENT */
      }

    doc_context->prev = context->doc_stack_tail;
    context->doc_stack_tail = doc_context;
    context->doc_stack_depth++;

    return EWS_STATUS_OK;
}

/*
 * ewsServeDocument
 * This function takes a context for which request processing has started and
 * cranks the state machine.
 *
 * context      - Context of request
 *
 * Returns EWA_STATUS_OK if state machine should continue to crank,
 * EWA_STATUS_OK_YIELD if either (a) processing is complete or (b) the
 * application requested that we yield the CPU.
 */
EwaStatus
ewsServeDocument ( EwsContext context )
{
  EwaNetBuffer buffer;          /* current outgoing buffer */
  EwsDocumentContextP doc_context;      /* context of document being served */
  uint32 node_offset;           /* data offset corresponding to node */
  uint32 chunk;                 /* bytes remaining in current chunk */

    EMWEB_TRACE(("ewsServeDocument(%p)\n", context));

#   ifdef EW_CONFIG_OPTION_FLUSH_DATA
    /*
     * If the context is marked to flush all currently pending
     * data, do so now.  This flag is set by ewsContextFlushData(),
     * called by the application during emweb_include/string
     * emweb_interate C-code.
     */
    if (context->flags & EWS_CONTEXT_FLAGS_FLUSH_DATA)
      {
        context->flags &= ~EWS_CONTEXT_FLAGS_FLUSH_DATA;
        if (ewsFlushAll( context ) != EWA_STATUS_OK)
          {
            ewsNetHTTPAbort(context);
            return EWA_STATUS_ERROR;
          }
      }
#   endif

    /*
     * Make sure we have prefetched a buffer to load the next part of the
     * document if there is data to send.  This is necessary so that we know
     * how much data we can collect and insert in a single network buffer.
     * We use this to throttle EmWeb/Server's use of the CPU by "cranking"
     * through one buffer at a time, more or less.
     */
    buffer = ewsStringAttachBuffer(&context->res_str_end);
    if (buffer == EWA_NET_BUFFER_NULL)
      {
        ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
        EMWEB_WARN(("ewsServeDocument: no buffers\n"));
        ewsNetHTTPAbort(context);
        return EWA_STATUS_ERROR;
      }

    /*
     * Start building outgoing buffer chain if not already started
     */
    if (context->res_buf_next == EWA_NET_BUFFER_NULL)
      {
        context->res_buf_next = buffer;
      }

    /*
     * Get current document state from top of stack
     */
    doc_context = context->doc_stack_tail;

    /*
     * if the document is not from our emweb archive, but is from the
     * application's local file system, then serve the document by
     * reading from the filesystem.  Else, source the document based
     * on the archive information
     */

#   ifdef EW_CONFIG_OPTION_FILE_GET
    if (doc_context->document->flags & EWS_FILE_FLAG_FILESYSTEM)
      {
        if (doc_context->remaining != 0)        /* if there MAY be data to read */
          {
            return ewsServeLocalFile( context, doc_context );
          }
      }
    else
#   endif /* EW_CONFIG_OPTION_FILE_GET */
      {
        /*********************************************************************
         * First, check if we are at a document node.  If so, process
         * it and return
         *********************************************************************/

        if (doc_context->node_count != 0)
          {
            /*
             * Read offset
             */
            node_offset = EW_BYTES_TO_UINT32(doc_context->nodep->data_offset);

            /*
             * If original data index at node offset, then process node
             */
            if (doc_context->offset == node_offset)
              {
                return ewsServeDocumentNode( context, doc_context );
              }

            /*
             * Otherwise, adjust available room so we get up to node offset
             */
            else
              {
                chunk = node_offset - doc_context->offset;
              }
          }
        /*
         * Otherwise, current chunk is remaining bytes in document
         */
        else
          {
            chunk = doc_context->remaining;
          }

        /*********************************************************************
         * At this point, we have "chunk" worth of static content to write
         * out.
         *********************************************************************/

        ewsServeDocumentData( context, doc_context, chunk );

      }  /* endif document is from the archive, not filesystem */


    /*************************************************************************
     * If document completed
     *************************************************************************/

    if (doc_context->remaining == 0 && doc_context->node_count == 0)
      {
        /*
         * Pop document stack
         */
        context->doc_stack_tail = doc_context->prev;
        if (doc_context->prev == (EwsDocumentContextP) NULL)
          {
            context->doc_stack_head = (EwsDocumentContextP) NULL;
          }
        else
          {
            context->doc_stack_tail->next = (EwsDocumentContextP) NULL;
          }
        context->doc_stack_depth--;

        /* NOTE: no archive if document is a local file */

#       ifdef EW_CONFIG_FILE_METHODS
        if (!(doc_context->document->flags & EWS_FILE_FLAG_FILESYSTEM))
#       endif /* EW_CONFIG_FILE_METHODS */
          {
            EWA_TASK_LOCK();
            doc_context->document->archive->reference_count--;
            EWA_TASK_UNLOCK();
          }

        /*
         * Release document context
         */
        ewsFreeDocContext(doc_context);

        /*
         * If no more documents, terminate request
         */
        if (context->doc_stack_tail == (EwsDocumentContextP) NULL)
          {
#           ifdef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND
            /*
             * Check to see if we have been pre-generating the NOTIFY
             * request body so that we can send it to Windows Me without
             * using chunked encoding.
             */
            if (context->flags & EWS_CONTEXT_FLAGS_BUFFER_BODY)
              {
                /* Send the request headers.
                 * Here we depend on knowing a priori that this is a Client
                 * context in order to find the original request document. */
                ewsSendHeader(context, context->client->document, NULL,
                              "\r\n");
                /* Turn off the flag so ewsBodyNSend works normally.
                 * This must be done after ewsSendHeader (so that when it
                 * calls ewsGetDocLength it will return scratch_length_used),
                 * but before ewsBodyNSend. */
                context->flags &= ~EWS_CONTEXT_FLAGS_BUFFER_BODY;
                /* Send the request body. */
                ewsBodyNSend(context, (uint8 *) context->scratch,
                             context->scratch_length_used);
                /* Deleting the scratch area will be done automatically at
                 * context end. */
              }
#           endif /* EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND */

#           ifdef EW_CONFIG_OPTION_CONVERT_XML
            /*
             * finish up XML Conversion
             */
            ewsServeXMLEndConvert( context );
#           endif /* EW_CONFIG_OPTION_COVERT_XML */

#           ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
            /*
             * finish up skeleton Conversion
             */
            if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
              {
                ewsServeSkeletonEndConvert( context );
              }
#           endif /* EW_CONFIG_OPTION_COVERT_SKELETON */

#           ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
            /*
             * If this document is a TEXTAREA XML conversion of an HTML
             * document, write out the final boilerplate...
             * Note this must be done after finishing up XML conversion,
             * since XML conversion is "inside" of TEXTAREA conversion.
             */
            ewsServeTextareaEndConvert( context );
#           endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

#           ifdef EW_CONFIG_OPTION_CHUNKED_OUT
            /*
             * If chunked, write trailer
             */
            switch ( context->chunk_out_state )
              {
              case ewsContextChunkOutNotChunked:
                break;

              case ewsContextChunkOutPartial:
                ewsBodyUpdateChunkHeader( context );
                /* deliberately fall through to put on last-chunk... */

              case ewsContextChunkOutNewChunk:
                ewsStringCopyIn(&context->res_str_end, "0\r\n");
#               if defined EW_CONFIG_OPTION_AUTH_DIGEST
                if (   context->auth_state == ewsAuthStateOKDigest
#                   ifdef _CONFIG_OPTION_AUTH_DIGEST_INT
                    || context->auth_state == ewsAuthStateOKDigestIntegrity
#                   endif /* _CONFIG_OPTION_AUTH_DIGEST_INT */
                   )
                  {
                    ewsAuthenticationInfoHeadOrTrail(context);
                  }
#               endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
                ewsStringCopyIn(&context->res_str_end, "\r\n");
              }
#           endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

#           ifdef EW_CONFIG_OPTION_SCHED

            /*
             * If using EmWeb's scheduler, then finish context and return
             * flush status to ewsRun()
             */
            ewsInternalFinish(context);
            return EWA_STATUS_OK;       /* may have yielded, but handle later */

#           else /* EW_CONFIG_OPTION_SCHED */

            /*
             * Otherwise, end request and return non-okay status to notify
             * caller that request has been terminated.  Caller must
             * abort the request.
             */
            (void) ewsFlushAll(context);
            return EWA_STATUS_ERROR;    /* not really error, but end request */

#           endif /* EW_CONFIG_OPTION_SCHED */
          }
      }

    /*
     * Flush out completed buffer and return application's yield state
     */

    return ewsFlush(context);
}


/*
 * ewsServeDocumentNode
 *
 * This function processes a node that has been encountered in the
 * outgoing document.
 *
 * context      - Context of request
 * doc_context  - Current context of the document.
 *
 * Determines the current node type, processes that node
 * and returns EWA_STATUS_OK if state machine should continue to crank,
 * EWA_STATUS_OK_YIELD if either (a) processing is complete or (b) the
 * application requested that we yield the CPU.  The document's node
 * count and next-node-ptr are updated.
 */
static EwaStatus
ewsServeDocumentNode( EwsContext context
                     ,EwsDocumentContextP doc_context)
{
# ifdef EMWEB_SANITY
  if (   doc_context->node_count == 0
      || doc_context->offset != EW_BYTES_TO_UINT32(doc_context->nodep->data_offset))
    {
      EMWEB_ERROR(("ewsServeDocumentNode: current offset MUST be at a node!!\n"));
    }
# endif /* EMWEB_SANITY */

  /*
   * Get node index and handle by type
   */
  switch (doc_context->nodep->type)
    {

#   ifdef EW_CONFIG_OPTION_STRING
    case EW_DOCUMENT_NODE_TYPE_STRING:
      return ewsServeEmWebString(context, doc_context);
#   endif /* EW_CONFIG_OPTION_STRING */

#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    case EW_DOCUMENT_NODE_TYPE_STRING_VALUE:
      return ewsServeEmWebStringValue(context, doc_context);
#   endif /* EW_CONFIG_OPTION_STRING_VALUE */

#   ifdef EW_CONFIG_OPTION_INCLUDE
    case EW_DOCUMENT_NODE_TYPE_INCLUDE:
      return ewsServeEmWebInclude(context, doc_context);
#   endif /* EW_CONFIG_OPTION_INCLUDE */

#   ifdef EW_CONFIG_OPTION_FORM
    case EW_DOCUMENT_NODE_TYPE_FORM:
      return ewsServeEmWebForm(context, doc_context);
#   endif /* EW_CONFIG_OPTION_FORM */

#   ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
    case EW_DOCUMENT_NODE_TYPE_DYNAMIC_SELECT:
      return ewsServeEmWebDynamicSelect(context, doc_context);
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#   ifdef EW_CONFIG_OPTION_FORM_REPEAT
    case EW_DOCUMENT_NODE_TYPE_START_RPT_BODY:
      return ewsServeEmWebRptBodyStart(context, doc_context);
    case EW_DOCUMENT_NODE_TYPE_END_RPT_BODY:
      return ewsServeEmWebRptBodyEnd(context, doc_context);
    case EW_DOCUMENT_NODE_TYPE_NAME_PREFIX:
    case EW_DOCUMENT_NODE_TYPE_VALUE_PREFIX:
      return ewsServeEmWebPrefix(context, doc_context);
#   endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#   ifdef EW_CONFIG_OPTION_ITERATE
    case EW_DOCUMENT_NODE_TYPE_START_ITERATE:
      return ewsServeEmWebIterateStart( context, doc_context );
    case EW_DOCUMENT_NODE_TYPE_END_ITERATE:
      return ewsServeEmWebIterateEnd( context, doc_context );
#   endif /* EW_CONFIG_OPTION_ITERATE */

#   ifdef EW_CONFIG_OPTION_IF
    case EW_DOCUMENT_NODE_TYPE_IF:
      return ewsServeEmWebIf( context, doc_context );
    case EW_DOCUMENT_NODE_TYPE_ENDIF:
      return ewsServeEmWebEndIf( context, doc_context );
#   endif /* EW_CONFIG_OPTION_IF */

#   ifdef EW_CONFIG_OPTION_NS_GET
    case EW_DOCUMENT_NODE_TYPE_NS_GET:
      return ewsServeEmWebNamespaceGet(context, doc_context);
#   endif /* EW_CONFIG_OPTION_NS_GET */

    case EW_DOCUMENT_NODE_TYPE_CONST_STRING:
      return ewsServeEmWebConstString( context, doc_context );

      /*
       * Skip unsupported node type
       */
    default:
      EMWEB_ERROR(("ewsServeDocument: Unknown node type %d seen in %s at "
                   "node_count %d, data offset %d.\n"
                   ,doc_context->nodep->type
                   ,doc_context->document->url
                   ,doc_context->node_count
                   ,(int) doc_context->offset));     /*LVL7_P0006*/
      EMWEB_ERROR(("ewsServeDocument: Do you have the right options "
                   "turned on?\n"));
      doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
      doc_context->node_count--;
      return EWA_STATUS_OK;
    }
}


/*
 * ewsServeDocumentData
 *
 * This function sends out a chunk of non-dynamic document data.
 *
 * context      - Context of request
 * doc_context  - Current context of the document.
 * count        - number of bytes of content to send.
 *
 * Sends count bytes of data from the current point in the
 * document, handling compression if needed.  Update the
 * count and position of the remaining data document's
 * doc_context.
 */
static void
ewsServeDocumentData( EwsContext context
                      ,EwsDocumentContextP doc_context
                      ,uint32 count )
{
  int32 available;             /* bytes available in current buffer */

  /*
   * Determine amount we can send in this buffer (may be less than
   * the desired "count").  We limit the amount of data to about
   * one buffer in order to allow other non-emweb processing to
   * take control easily.
   */
  available = ewsStringAvailable(&context->res_str_end);
# ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
  /* If serving skeleton XML, wrap the constant string in a CDATA section. */
  /* Adjust the count of the number of bytes we are willing to output at this
   * time. */
  if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
    {
      available = ewsServeSkeletonCDATABegin( context, doc_context, available);
    }
  /* A returned value of -1 means that the remainder of this buffer is too
   * small to contain the entire CDATA, so we should finish the buffer off
   * and return to the caller to supply another buffer. */
  if (available < 0) {
    ewaNetBufferLengthSet(context->res_str_end.first,
                          context->res_str_end.offset);
    return;
  }
# endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */

  /* At this point, 'available' is the number of bytes of data that the buffer
   * will hold, after accounting for any CDATA wrapper.
   * Now convert it to the count of bytes of data remaining to be written
   * that we have room for by limiting it by the number of bytes of data
   * available.
   * (All of this may be academic if EWS_CONTEXT_FLAGS_OUTPUT_QUOTE is set,
   * because then data characters may be written as more than one byte.
   * But ewsBodyNSend will allocate additional buffers as necessary.)
   */
  if ((unsigned int )available > count)
    {
      available = count;
    }

# ifdef EW_CONFIG_OPTION_COMPRESS
  /*
   * If decompression, decompress available bytes into output buffer
   */
  if (  doc_context->doc_headerp->document_flags
        & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS
        )
    {
      while (available != 0)
        {
          uintf cacheCount;
          uint8 cache[64];

          cacheCount = ((available > (int32) sizeof(cache))
                        ? (uintf) sizeof(cache)
                        : (uintf) available);

          (void) ewDecompressRead( (EwDecompressContext)doc_context->compress_context
                                   ,cache
                                   ,cacheCount
                                 );
          ewsBodyNSend( context, cache, cacheCount );

          doc_context->offset += cacheCount;
          doc_context->remaining -= cacheCount;
          available -= cacheCount;
        }
    }
  else
# endif /* EW_CONFIG_OPTION_COMPRESS */
    /*
     * No compression
     */
    {
      ewsBodyNSend(context, doc_context->datap, available);
      doc_context->datap += available;
      doc_context->offset += available;
      doc_context->remaining -= available;
    }

# ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
  if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
    {
      ewsServeSkeletonCDATAEnd(context, doc_context);
    }
# endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */
}

#ifdef EW_CONFIG_OPTION_FORM

/*
 * ewsServeSetupForm
 * Set up state to process FORM submission
 *
 * context      - context of request
 * document     - document containing FORM node for submission
 *
 * Returns TRUE if parser needs to be cranked again, FALSE otherwise.
 */
boolean
ewsServeSetupForm ( EwsContext context, EwsDocument document )
{
  const EwsDocumentHeader *doc_header = document->doc_header;
  EwsDocumentNodeP nodep;
  uint32 offset;
  uintf node_count;

    /*
     * If GET+Query, setup GET method form submission by adjusting
     * request parser state and form length to refer to query
     * string.
     */
    if (context->req_method == ewsRequestMethodGet &&
        ewsStringLength(&context->req_query) != 0)
      {
        context->form_parsev = context->req_query;
        context->form_parsep = &context->form_parsev;
        context->form_expectv = ewsStringLength(&context->req_query);
        context->form_expectp = &context->form_expectv;
#       if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
        context->form_chunkedv = FALSE;
        context->form_chunkedp = &context->form_chunkedv;
#       endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
      }

    /*
     * Otherwise, set form length to expected length as defined in
     * the Content-Length: request header
     */
    else if ((context->req_method == ewsRequestMethodPost)
#   ifdef EW_CONFIG_OPTION_SOAP

             || (context->req_method == ewsRequestMethodMPost)
#   endif /* EW_CONFIG_OPTION_SOAP */
            )
      {
#       if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
        /*
         * Form expects data.  Let HTTP/1.1 clients know that it is
         * okay to continue sending the request.
         */
#       ifdef EW_CONFIG_OPTION_SOAP
        if (!(context->soap_flags & EW_SOAP_REQUEST))
#       endif
          {
            EMWEB_TRACE(("ewsServeSetupForm: calling ewsSendContinue from "
                         "%s:%d\n",
                         __FILE__, __LINE__));
            ewsSendContinue(context);
            context->form_chunkedp = &context->chunked_encoding;
#           endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
            context->form_expectp = &context->content_remaining;
            context->form_parsep = &context->req_str_end;
          }
      }

    /*
     * If not GET or POST, unsupported method.  Generate "Not
     * Implemented" response and terminate request processing.
     */
    else
      {
        ewaLogHook(context, EWS_LOG_STATUS_METHOD_NOT_ALLOWED);
        ewsSendHeaderAndEnd(context
                           ,ews_http_status_405
                           ,ews_http_string_405);
        return FALSE;
      }

    /*    Check if request header is conditional IF-MATCH */
#ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
    {
      if (ewsStringLength(&context->req_headerp[ewsRequestHeaderIfMatch])
          != 0)
        {
          boolean status;
          status = ewsHandleMatchHdr(context, document,
                                     IfMatch_Conditional);
          if (status == FALSE)
            {
              return(FALSE);
            }

        }
    }
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */

    /*    Check if request header is conditional IF-NONE-MATCH */
#ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
    {
      if (ewsStringLength(&context->req_headerp[ewsRequestHeaderIfNoneMatch])
          != 0)
        {
          boolean status;
          status = ewsHandleMatchHdr(context, document,
                                     IfNoneMatch_Conditional);
          if (status == FALSE)
            {
              return(FALSE);
            }
        }
    }
#endif /*#ifdef EW_CONFIG_OPTION_CACHE_CONDITIONAL_MATCH*/


    /*
     * Get document node list
     */
    offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
    nodep = (EwsDocumentNodeP) &document->archive_data[offset];
    node_count = EW_BYTES_TO_UINT16(doc_header->node_count);

#   ifdef EMWEB_SANITY
    if (offset == 0 || node_count == 0)
      {
        EMWEB_ERROR(("ewsServeSetupForm: no form nodes\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return (FALSE);
      }
#   endif /* EMWEB_SANITY */

    /*
     * Search list for first FORM node
     */
    while (node_count > 0)
      {
        if (nodep->type == EW_DOCUMENT_NODE_TYPE_FORM)
          {
            break;
          }
        node_count--;
        nodep = NEXT_DOCNODE( nodep );
      }

#   ifdef EMWEB_SANITY
    if (node_count == 0)
      {
        EMWEB_ERROR(("ewsServeSetupForm: node not found\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return (FALSE);
      }
#   endif /* EMWEB_SANITY */

    /*
     * Save form node information in context
     */
    context->form_index = EW_FORM_NODE_INDEX_TO_FORM_INDEX
      (EW_BYTES_TO_UINT32(nodep->index));

#   ifdef EW_CONFIG_OPTION_NS_SET
    /*
     * If this is an EmWeb-style form (e.g. parse to C structures)
     */
    if (context->form_index != EW_FORM_INDEX_NAMESPACE)
#   endif /* EW_CONFIG_OPTION_NS_SET */
      {

#       ifdef EMWEB_SANITY
        if (context->form_index >= document->archive->emweb_form_table_size ||
            document->archive->emweb_form_table == NULL)
          {
            EMWEB_ERROR(("ewsServeSetupForm: bad form index\n"));
            ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
            ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
            return (FALSE);
          }
#       endif /* EMWEB_SANITY */

        context->form_entry =
          &document->archive->emweb_form_table[context->form_index];
        context->form_enum_table = document->archive->emweb_form_enum_table;
        context->form_enum_table_size=document->archive->emweb_form_enum_table_size;
      }
#   ifdef EW_CONFIG_OPTION_NS_SET
    /*
     * If namespace form selected, get namespace index
     */
    else
      {
        uintf indx;
        indx = EW_FORM_NODE_INDEX_TO_NS_INDEX
          (EW_BYTES_TO_UINT32(nodep->index));
        context->form_ns = &document->archive->emweb_namespace_table[indx];
      }
#   endif /* EW_CONFIG_OPTION_NS_SET */

#   ifdef EW_CONFIG_OPTION_SOAP
    if (!(context->soap_flags & EW_SOAP_REQUEST))
#   endif
      {
        context->req_str_begin = *context->form_parsep;
        context->state = ewsContextStateParsingForm;
        context->substate.form = ewsContextSubstateParseName;
      }

#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
    /*
     * Check for "multipart/form-data,boundary=..." content type
     */
    if (context->req_method == ewsRequestMethodPost)
      {
        EwsString es1, es2;
        uint32 bytes;

          es1 = context->req_headerp[ewsRequestHeaderContentType];
          bytes = ewsStringLength(&es1);

          if (ewsStringCompare(&es1
                               ,"multipart/form-data"
                               ,ewsStringCompareCaseInsensitive
                               ,&bytes))
            {
              /* We need to find the value of the "boundary" parameter.
               *
               * First, the state structures we will need to manage
               * a multipart/form-data submission
               */
              if (NULL == (context->inputFile = (EwsFormInputFileState *)
                           ewaAlloc( sizeof( EwsFormInputFileState )
                                     + (sizeof(EwsString)
                                        * ewsFormDataHeaderCount)
                                     )
                           )
                  )
                {
                  ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
                  ewsSendHeaderAndEnd(context
                                      ,ews_http_status_500
                                      ,ews_http_string_500);
                  return (FALSE);
                }

              EMWEB_MEMSET( context->inputFile
                           ,0
                           ,sizeof(EwsFormInputFileState)
                            + (sizeof(EwsString)
                               * ewsFormDataHeaderCount)
                            );

              context->inputFile->formDataHeaders =
                (EwsString*) &context->inputFile[1];

#             ifdef EW_CONFIG_OPTION_CHUNKED_IN
              ewsStringInit( &context->inputFile->leftOverChunk
                            ,EWA_NET_BUFFER_NULL );
#             endif /* EW_CONFIG_OPTION_CHUNKED_IN */


              /*
               * Parse multipart parameters until boundary found
               * es1 and bytes are now the remaining part of the
               * header past "multipart/form-data".  If found,
               * es2 will contain the boundary value, and it
               * should not be of zero length!!
               */

              if (   ewsParseAttributeFromHeader( &es1
                                                 ,(char *)"boundary"
                                                 ,&es2
                                                 ,ewsParseValue
                                                 ,';'
                                                 ,ewsHdrUpdateNo )
                  && (bytes = ewsStringLength(&es2)) != 0
                  )
                {
                  if ((context->inputFile->formDataBoundary =
                       (char *)ewaAlloc( bytes + 5 ))
                      != NULL)
                    {
                      char *cptr = context->inputFile->formDataBoundary;

                      cptr[0] = '\r';
                      cptr[1] = '\n';
                      cptr[2] = cptr[3] = '-';
                      ewsStringCopyOut( &cptr[4]
                                       ,&es2
                                       ,bytes
                                      );

                    }
                }

              if (context->inputFile->formDataBoundary == NULL)
                {
                  ewaLogHook(context, EWS_LOG_STATUS_BAD_FORM);
                  ewsSendHeaderAndEnd(context
                                      ,ews_http_status_400
                                      ,ews_http_string_400);
                  return FALSE;

                }

              context->state = ewsContextStateParsingMPForm;
              context->substate.form = ewsContextSubstateMPPreamble;
            }       /* end if multipart/form-data */
      }     /* end if req_method == post */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */



  /*
   * Start form - allocate form structure and attach to context
   */
  ewsStringLengthSet(&context->req_str_begin, 0);
  if (ewsFormSubmitStart(context) != EWS_STATUS_OK)
    {
      ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
      ewsNetHTTPAbort(context);
      return FALSE;
    }
# ifdef EW_CONFIG_OPTION_SOAP
  if (context->soap_flags & EW_SOAP_REQUEST)
    {
      /*
       * this is it.
       * We just needed to setup document structures
       * to collect data parsed from the request body
       */
      return(TRUE);
    }
# endif /* EW_CONFIG_OPTION_SOAP */

  /*
   * If no content is expected, then invoke submission now
   */
  if (*context->form_expectp == 0
#     if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
      && !*context->form_chunkedp
#     endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
      )
    {
      /* note well: content-length may not be given when parsing
       * a form-data POST.  If this is the case, then set form_expectp
       * to NULL, and parse the data until we hit the "end boundary"
       */
#     ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
      if (   context->state == ewsContextStateParsingMPForm
          && (ewsStringLength(&context->req_headerp[ewsRequestHeaderContentLength])
              == 0)
         )
        {
          context->form_expectp = NULL;     /* ignore content length */
        }
      else
#     endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */
        {
          context->state = ewsContextStateSubmittingForm;
          ewsServeSubmitForm(context);
          return FALSE;
        }
    }

  /*
   * Run the parser
   */
  return ewsParse(context);
}

/*
 * ewsServeSubmitForm
 * This function is called by the parser after parsing of the received
 * form data is complete.  At this point, the form is submitted to the
 * application and a response is generated.  The application returns a
 * URL to serve, or NULL for a default response.
 *
 * context      - context of request
 *
 * No return value
 */
void
ewsServeSubmitForm( EwsContext context )
{
  const char *url;

# if defined EW_CONFIG_OPTION_AUTH_DIGEST
    /*
     * Verify message digest
     */
  EwsStatus status;
  /* First check if this is a BASIC form (we can have both basic & digest forms).
     if it is, then don't do the digest handling below. (LLB)
  */

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
 if (context->schedulingState != ewsContextResuming)
#endif
 {
  if ((ewsAuthStateOKBasic==context->auth_state) ||
      (context->auth_state == ewsAuthStateOKMBasic))
    {
      EMWEB_TRACE(("ewsServeSubmitForm: auth_state indicates serving BASIC form\n"));
    }
  else
    {
      if (   context->auth_state != ewsAuthStateUninitialized
          && context->auth_state != ewsAuthStateUnauthenticated)
        {
          status = ewsAuthDigestFinish(context);
          if ((status == EWS_STATUS_AUTH_FAILED) ||
              (status == EWS_STATUS_AUTH_CHALLENGE))
            {
              ewaLogHook(context, EWS_LOG_STATUS_AUTH_FORGERY);

              if (ewsSendHeader(context
                                ,EWS_DOCUMENT_NULL
                                ,ews_http_status_401
                                ,ews_http_string_401) == EWS_STATUS_OK)
                {
                  status = ewsAuthChallenge(context->realmp, context);

                  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                    return;

#             ifdef EW_CONFIG_AUTH_SCHED                   /* context suspended */
                  if (context->schedulingState == ewsContextSuspended)
                    return;
#             endif

                  if (status == EWS_STATUS_OK)
                    {
                      ewsSendStatusString(context, ews_http_string_401, NULL);
                    }
                  else
                    {
                      ewsSendStatusString(context, ews_http_string_400, NULL);
                    }
                  return;
                }
            }
          if (status == EWS_STATUS_BAD_REQUEST)
            {
              /*
               * This should not happen, by now authorization process is
               * finishing integrity state, so data provided by client
               * has been verified long ago
               */
              ewsSendHeaderAndEnd(context ,ews_http_status_400 ,ews_http_string_400);
              return;
            }
          else if (status == EWS_STATUS_NO_RESOURCES)
            {
              ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
              EMWEB_WARN(("ewsServeSubmitSetup: unable to authenticate\n"));
              ewsNetHTTPAbort(context);
              return;
            }
        }
    }
 }
# endif

    /*
     * Invoke application submission
     */
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState != ewsContextResuming)
#   endif
      {
        ewaLogHook(context, EWS_LOG_STATUS_OK);
      }
    url = ewsFormSubmitAction (context);

    /*
     * Handle special case for abort and suspend
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return;
      }

#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return;
      }
    /* reset state to scheduled (may be resuming) */
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

#   ifdef EW_CONFIG_OPTION_CLIENT
    /*
     * If we are an HTTP Client request, then we are here because
     * the response from the remote server was sent to the EmWeb
     * internal form processing.  This is a submit-only operation
     * for API convenience, but the return value is actually
     * irrelevent as there is no where to send a response.  So,
     * complete processing by returning back to client.
     */
    if (context->client != NULL)
      {
        ewsFormFree(context);
        context->state = ewsContextStateClient;
        return;
      }
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   ifdef EW_CONFIG_OPTION_CONTEXT_SEND_REPLY

    /*
     * Handle case where application's form submission function invokes
     * ewsContextSendReply() to serve document as response
     */

#   ifdef EW_CONFIG_OPTION_SCHED
    /*
     * If EmWeb scheduler is used, we will be in the ServingDocument state.
     * We need to free the submitted form in case the served document contains
     * a form, and return to the scheduler to continue processing.
     *
     * If EmWeb scheduler is not used and connections are not persistent,
     * then the context would have been aborted and caught above.
     */
    if (context->state == ewsContextStateServingDocument)
      {
        ewsFormFree(context);
        return;
      }
#   else /* EW_CONFIG_OPTION_SCHED */

#   ifdef EW_CONFIG_OPTION_PERSISTENT
    /*
     * If EmWeb scheduler is not used and connections are persistent, then
     * we will be in the SkipContent state.  Just return and await further
     * data from the network for parsing.
     */
    if (context->state == ewsContextStateSkipContent)
      {
        return;
      }
#   endif /* EW_CONFIG_OPTION_PERSISTENT */

#   endif /* EW_CONFIG_OPTION_SCHED */

#   endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REPLY */

    /*
     * If URL specified, send redirection response
     */
    if (url != NULL)
      {
        ewsSendRedirect(context, url, ewsRedirectSeeOther);
      }

    /*
     * Otherwise, send default response
     */
    else
      {
        ewaLogHook(context, EWS_LOG_STATUS_NO_CONTENT);
        ewsSendHeaderAndEnd(context, ews_http_status_204, ews_no_body );
      }
}
#endif /* EW_CONFIG_OPTION_FORM */

#ifdef EW_CONFIG_OPTION_CGI
/*
 * ewsServeSetupCGI
 * Set up state to process CGI request
 *
 * context      - context of request
 * document     - CGI document
 *
 * No return value
 */
void
ewsServeSetupCGI ( EwsContext context, EwsDocument document )
{
  const EwsDocumentHeader *doc_header = document->doc_header;
  EwsDocumentNodeP node;
  uint32 offset;

    /*
     * Get CGI document node
     */
    offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
    node = (EwsDocumentNodeP) &document->archive_data[offset];

#   ifdef EMWEB_SANITY
    /*
     * Check node and abort if there is a problem (database corrupt?)
     */
    if (offset == 0 || node->type != EW_DOCUMENT_NODE_TYPE_CGI)
      {
        EMWEB_ERROR(("ewsServeSetup: bad CGI node\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return;
      }
#   endif /* EMWEB_SANITY */

    context->cgi_index = EW_BYTES_TO_UINT32(node->index);

#   ifdef EMWEB_SANITY
    if (context->cgi_index == 0 ||
        context->cgi_index > document->archive->emweb_cgi_table_size)
      {
        EMWEB_ERROR(("ewsServeSetupCGI: bad cgi index\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Save data function in context to handle additional inbound request
     * data
     */
    context->cgi_data_f =
      document->archive->emweb_cgi_table[context->cgi_index].data_f;

    /*
     * Invoke CGI start function
     */
    context->state = ewsContextStateStartingCGI;
    context->cgi_handle =
      document->archive->emweb_cgi_table[context->cgi_index].start_f( context );

    /*
     * If aborted before return, really abort now
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return;
      }

    /*
     * If state changed to anything else, return now.  (This may
     * be due to ewsCGIRedirect() changing state to ServingDocument).
     */
    if (context->state != ewsContextStateStartingCGI)
      {
        return;
      }

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    /*
     * CGI appears to expect data.  Let HTTP/1.1 clients know that it is
     * okay to continue sending the request.
     */
    EMWEB_TRACE(("ewsServeSetupCGI: calling ewsSendContinue from %s:%d\n",
                 __FILE__, __LINE__));
    ewsSendContinue(context);
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

    /*
     * If there is no data to send to CGI script, return now
     */
    context->state = ewsContextStateServingCGI;
    if (ewsStringAvailable(&context->req_str_end)  == 0 &&
        ewaNetBufferNextGet(ewsStringBuffer(&context->req_str_end)) ==
          EWA_NET_BUFFER_NULL)
      {
        context->schedulingState = ewsContextDataWait;
        return;
      }

    /*
     * If request data is present in the same buffer as HTTP
     * headers.  We must copy the data portion of that buffer into a
     * new buffer before attempting delivery to the application.
     */
    if (ewsStringAvailable(&context->req_str_end) != 0)
      {
        EwaNetBuffer buf;
        EwsString estring;

          /*
           * Copy data portion of last buffer containing headers into
           * fresh buffer(s).  Abort context if resources unavailable.
           */
          ewsStringInit(&estring, EWA_NET_BUFFER_NULL);
          buf = ewsStringAttachBuffer(&estring);
          if (buf == EWA_NET_BUFFER_NULL ||
              ewsStringNCopyIn(&estring
                              ,ewsStringData(&context->req_str_end)
                              ,ewsStringAvailable(&context->req_str_end))
                == EWA_NET_BUFFER_NULL)
            {
              ewsNetHTTPAbort(context);
              return;
            }
          ewaNetBufferLengthSet(ewsStringBuffer(&estring), estring.offset);

          /*
           * Append any received buffers after header to copy
           */
          ewaNetBufferNextSet(buf,
            ewaNetBufferNextGet(ewsStringBuffer(&context->req_str_end)));

          /*
           * Link copy to last buffer containing headers.
           */
          ewaNetBufferNextSet(ewsStringBuffer(&context->req_str_end), buf);
      }

    /*
     * Process CGI data received so far
     */
    ewsServeCGIData( context );
}

#ifdef EMWEB_XMLP
/*
 * ewsServeSetupXMLP
 * Set up state to process XMLP request
 *
 * context      - context of request
 * document     - XMLP document
 *
 * No return value
 */
void
ewsServeSetupXMLP ( EwsContext context, EwsDocument document )
{
  const EwsDocumentHeader *doc_header = document->doc_header;
  uint32 offset;
  uint16 xmlp_index;

    /*
     * Get XMLP document node
     */
    offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
    xmlp_index = EW_BYTES_TO_UINT16(doc_header->extend);

#   ifdef EMWEB_SANITY
    if (   xmlp_index == 0
        || (   xmlp_index > document->archive->emweb_xmlp_table_size
#   ifdef EW_CONFIG_OPTION_SOAP
            && xmlp_index != EW_XMLP_INDEX_INTERNAL_SOAP
#   endif
#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
            && xmlp_index != EW_XMLP_INDEX_INTERNAL_SSDP_RSP
#   endif
           )
       )
      {
        EMWEB_ERROR(("ewsServeSetupXMLP: bad xmlp index\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return;
      }
#   endif /* EMWEB_SANITY */

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    /*
     * XMLP expects data.  Let HTTP/1.1 clients know that it is
     * okay to continue sending the request.
     */
    EMWEB_TRACE(("ewsServeSetupXMLP: calling ewsSendContinue from %s:%d\n",
                 __FILE__, __LINE__));
    ewsSendContinue(context);
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

#   ifdef EW_CONFIG_OPTION_UPNP
#   ifdef EW_CONFIG_OPTION_SOAP
    if (xmlp_index == EW_XMLP_INDEX_INTERNAL_SOAP)
      {
        context->soap_flags |= EW_SOAP_REQUEST;
        /*
         * register SOAP consumer
         */
         if (ewuSoapIntroXmlpCustomer(context) != EWS_STATUS_OK)
           {
             EMWEB_WARN(("ewsServeSetupXMLP: SOAP consumer registration failed\n"));
             ewsNetHTTPAbort(context);
           }
      }
#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
    else if (xmlp_index == EW_XMLP_INDEX_INTERNAL_SSDP_RSP)
          {
            /*
             * register SSDP consumer
             */
            if (ewuSsdpIntroXmlpCustomer(context) != EWS_STATUS_OK)
             {
               EMWEB_WARN(("ewsServeSetupXMLP: SSDP consumer registration failed\n"));
               ewsNetHTTPAbort(context);
             }
          }
#   endif /* EW_CONFIG_OPTION_UPNP_GATEWY */
    /*
     * Otherwise, invoke XMLP start
     */
    else
#   endif /* EW_CONFIG_OPTION_SOAP */
#   endif /* EW_CONFIG_OPTION_UPNP */
      {
#       ifdef EW_CONFIG_OPTION_XMLP
        EmWebXmlpTableP tp = &document->archive->emweb_xmlp_table[xmlp_index];
        if ( ewsContextSetXMLP(context
                              ,(XmlpConsumer)tp->consumer) != EWS_STATUS_OK )
          {
            EMWEB_WARN(("ewsServeSetupXMLP: ewsContextSetXMLP failed\n"));
            ewsNetHTTPAbort(context);
          }
#       else /* EW_CONFIG_OPTION_XMLP */
        EMWEB_ERROR(("ewsServeSetupXMLP: EW_CONFIG_OPTION_XMLP not defined\n"));
#       endif /* EW_CONFIG_OPTION_XMLP */
      }
}
#endif /* EMWEB_XMLP */

/*
 * ewsServeCGIData
 * Continue a CGI request by passing data to it.
 *
 * context      - context of request
 *
 * No return value
 */
void
ewsServeCGIData( EwsContext context )
{
  EwaNetBuffer buffer;
  EwaNetBuffer buffer_prev;

  /*
   * req_str_end marks the end of the saved request headers.  New CGI
   * data will be linked to subsequent buffers.  First, we must unlink
   * the data.
   */
  buffer_prev = ewsStringBuffer(&context->req_str_end);
  buffer = ewaNetBufferNextGet(buffer_prev);
  ewaNetBufferNextSet(buffer_prev, EWA_NET_BUFFER_NULL);
  /* reset last buf ptr - bugfix: ASI #683 */
  context->req_buf_last = buffer_prev;

  /*
   * Send data to CGI script
   */
  context->cgi_data_f(context->cgi_handle, buffer);

  /* if ewsContextSendReply() is called, then we are
   * done, and the state is "send data".  Otherwise,
   * wait for more incoming stuff... [ASI Support #683]
   */
  if (context->state == ewsContextStateServingCGI)
    {
      /*
       * Wait for more incoming data.  It's up to the
       * application to abort this context when its finished.
       */
      context->schedulingState = ewsContextDataWait;
    }
}
#endif /* EW_CONFIG_OPTION_CGI */

#ifdef EW_CONFIG_OPTION_IMAGEMAP
/*
 * ewsServeImageMap
 * Process imagemap
 *
 * context      - context of request
 * document     - document containing FORM node for submission
 *
 * No return value
 */
void
ewsServeImageMap ( EwsContext context, EwsDocument document )
{
  const EwsDocumentHeader *doc_header = document->doc_header;
  EwsDocumentNodeP nodep;
  uint32 offset;
  const EwImageMapTable *imap_table;
  const EwImageMap *imap_entryp;
  uintf i;
  uint32 x;
  uint32 y;
  const char *url;

    /*
     * Get imagemap document node
     */
    offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
    nodep = (EwsDocumentNodeP) &document->archive_data[offset];

#   ifdef EMWEB_SANITY
    /*
     * Check node and abort if there is a problem (database corrupt?)
     */
    if (offset == 0 || nodep->type != EW_DOCUMENT_NODE_TYPE_IMAGEMAP)
      {
        EMWEB_ERROR(("ewsServeImageMap: bad imagemap node\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Parse x,y from query string
     */
    if (!ewsStringGetInteger(&context->req_query, 10, &x) ||
        ewsStringGetChar(&context->req_query) != ',' ||
        !ewsStringGetInteger(&context->req_query, 10, &y))
      {
        EMWEB_WARN(("ewsServeImageMap: bad query string\n"));
        ewaLogHook(context, EWS_LOG_STATUS_BAD_IMAGEMAP);
        ewsSendHeaderAndEnd(context, ews_http_status_400, ews_http_string_400);
        return;
      }

    /*
     * Lookup image map table entry
     */
    offset = EW_BYTES_TO_UINT32(nodep->index);
    imap_table = &document->archive->emweb_image_table[offset];

#   ifdef EMWEB_SANITY
    if (offset >= document->archive->emweb_image_table_size ||
        document->archive->emweb_image_table == NULL)
      {
        EMWEB_ERROR(("ewsServeImageMap: bad imagemap node index\n"));
        ewaLogHook(context, EWS_LOG_STATUS_INTERNAL_ERROR);
        ewsSendHeaderAndEnd(context, ews_http_status_500, ews_http_string_500);
        return;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Start with default URL, then scan entries in order for encapsulating
     * rectangle.  If found, update url pointer accordingly.
     */
    url = imap_table->default_url;
    for (imap_entryp = imap_table->map_table, i = imap_table->map_entries;
         i > 0;
         imap_entryp++, i--)
      {
        if (x >= imap_entryp->ul_x && x <= imap_entryp->lr_x &&
            y >= imap_entryp->ul_y && y <= imap_entryp->lr_y)
          {
            url = imap_entryp->url;
            break;
          }
      }

    /*
     * If URL specified, send redirection response
     */
    if (url != NULL)
      {
        ewsSendRedirect(context, url, ewsRedirectSeeOther);
      }

    /*
     * Otherwise, send default response
     */
    else
      {
        ewaLogHook(context, EWS_LOG_STATUS_NO_CONTENT);
        ewsSendHeaderAndEnd(context, ews_http_status_204, ews_no_body );
      }
}
#endif /* EW_CONFIG_OPTION_IMAGEMAP */

#ifdef EW_CONFIG_OPTION_STRING
/*
 * ewsServeEmWebString
 * Handle <EMWEB_STRING> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebString ( EwsContext context, EwsDocumentContextP doc_context )
{
  uint32 node_index;
  const void *vp;
  uintf type;

    /*
     * Adjust state for proper handling of ewsNetHTTPAbort
     */
    context->state = ewsContextStateString;


#  ifdef EW_CONFIG_OPTION_STRING_VALUE
    /*
      This happens if a macro is invoked without an
      argument. Note that it is perfectly possible to have
      EW_DOCUMENT_NODE_CLEAR_VALUE and EW_DOCUMENT_NODE_SET_VALUE
      flagged for the same node. The first clears the string_value
      field before serving the node. The second sets the string_value
      field after serving the node. example: emweb:/foo1?emweb:/foo2;
      */
    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_CLEAR_VALUE)
      {
        doc_context->string_value = NULL;
      }
#   endif

    /*
     * Determine if the index field of the emweb string node is the
     * index of the C-code case, or a pointer to the schema node
     * for this emweb string
     */
    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_STR_NAME_INFO)
      {
#       if defined( EW_CONFIG_SCHEMA_ARCHIVE )
        EwsDocumentNode *schema;

        node_index = 0x00FFFFFF & EW_BYTES_TO_UINT32( doc_context->nodep->index );

        schema = (EwsDocumentNode *)
          (  ((const char *)doc_context->document->schema_nodes)
           + (SIZEOF_EWS_DOCUMENT_NODE * (node_index - 1))
          );

        node_index = EW_BYTES_TO_UINT32( schema->data_offset );
#       else        /* unable to process this node, no schema stuff */
        EMWEB_ERROR(("ews_serv.c: this archive was compiled with the --archive-symbols option.\n"
                     " The EmWeb/Server will be unable to process most <EMWEB_STRINGS> in this archive.\n"
                     " You must #define EW_CONFIG_OPTION_CONVERT_XML in ew_options.h to use this archive.\n" ));
        doc_context->node_count--;
        doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
        return EWA_STATUS_OK;       /* skip this node */
#       endif
      }
    else
      {
        node_index = EW_BYTES_TO_UINT32(doc_context->nodep->index);
      }
#   ifdef EW_CONFIG_OPTION_NS_LOOKUP
    context->node_index = node_index; /* save for NS_LOOKUP support */
#   endif /* EW_CONFIG_OPTION_NS_LOOKUP */

    /*
     * Invoke <EMWEB_STRING> C-code fragment
     */
    vp = doc_context->document->archive->emweb_string( context
          ,node_index & 0x00ffffff);

#   ifdef EW_CONFIG_OPTION_NS_LOOKUP
    node_index = context->node_index; /* restore for NS_LOOKUP support */
#   endif /* EW_CONFIG_OPTION_NS_LOOKUP */

    /*
     * Handle special cases for abort or suspend, otherwise
     * restore state
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return EWA_STATUS_ERROR;
      }
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return ewsFlushAll(context);
      }
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

    EWS_GROUP_DYNAMIC_CHECK(context, vp);

    context->state = ewsContextStateServingDocument;

    type = node_index >> 24;

    /*
     * If valid string returned, copy it into outgoing
     * network buffers (unless value is to be stored in context as parameter
     * to subsequent macros)
     */
    if (   vp != NULL
        && 0 == (doc_context->nodep->attributes & EW_DOCUMENT_NODE_SET_VALUE)
       )
      {
#       ifdef EW_CONFIG_OPTION_CONVERT_XML
        /*
         * if converting to XML, send the string out via
         * the "XML preprocessor"
         */
        ewsServeXMLEmWebStringBegin( context, doc_context );
#       endif /* EW_CONFIG_OPTION_CONVERT_XML */

#       ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
        /*
         * If outputting skeleton, output only the DYNAMIC tag.
         */
        if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
          {
            if (ewsServeSkeletonEmWebString(context, doc_context))
              goto done;
          }
#       endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */

#       ifdef EW_CONFIG_OPTION_STRING_TYPED
        /*
         * If typed, use form conversion functions to extract value from
         * pointer and insert in output stream.  We fake out a value
         * offset that, referenced from the current form (or NULL),
         * results in the pointer returned by the EMWEB_STRING code
         * fragment.
         */
        if (type != 0)
          {
            EwFormField field;

#           if defined(EMWEB_SANITY) && defined(EW_CONFIG_OPTION_SECONDARY_ACTION)
            /*
             * KAG 02/08/99: can eventually remove this processing of
             * EW_DOCUMENT_NODE_FORM_ACTION, since as of release 5.0, the compiler
             * no longer generates emweb_string nodes with this attribute.
             * See EW_DOCUMENT_NODE_TYPE_CONST_STRING
             */
            if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_FORM_ACTION)
              {
                EMWEB_ERROR(("ews_serv.c: Internal error: typed form action!\n"));
              }
#           endif

            /* The handling of an EMWEB_STRING with EMWEB_TYPE=ENTITY_STRING
             * (the convert-special-characters variety of character string)
             * is a special case -- vp is the pointer to the char string,
             * not the pointer to a field containing the pointer to the chars.
             * This case has to be handled specially here because EMWEB_STRING
             * processing does not match FORM processing for text fields.
             * (The EMWEB_TYPE-missing case is also a special case, but it
             * is handled above (look for "if (type != 0)").)
             */
            field.value_offset = (ewFieldTypeText == type ?
                                  (uint8 *) &vp :
                                  (uint8 *) vp
                                  ) - ((uint8 *) context->form);
            if (ews_state->form_field_table[type].to_f != NULL)
              {
                ews_state->form_field_table[type].to_f(context, &field);
              }
#           ifdef EMWEB_SANITY
            else
              {
                EMWEB_WARN(("EMWEB_STRING: EMWEB_TYPE unsupported\n"));
                ewsBodySend(context, "EMWEB_UNSUPPORTED");
              }
#           endif /* EMWEB_SANITY */
          }
        /*
         * Otherwise, handle as untyped by treating returned pointer as
         * character string.
         */
        else
#       else /* EW_CONFIG_OPTION_STRING_TYPED */
#       ifdef EMWEB_SANITY
          if (type != 0)
            {
              EMWEB_WARN(("ewsServeEmWebString: typed strings unsupported\n"));
            }
          else
#       endif /* EMWEB_SANITY */
#       endif /* EW_CONFIG_OPTION_STRING_TYPED */
            {
#             ifdef EW_CONFIG_OPTION_SECONDARY_ACTION
              /*
               * KAG 02/08/99: can eventually remove this processing of
               * EW_DOCUMENT_NODE_FORM_ACTION, since as of release 5.0, the compiler
               * no longer generates emweb_string nodes with this attribute.
               * See EW_DOCUMENT_NODE_TYPE_CONST_STRING
               */
              if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_FORM_ACTION)
                {
                  const char *prefix;
                  /*
                   * this string is really the contents of a form's ACTION=
                   * attribute (these are always emweb strings).
                   * Give the user a chance to prefix a special path
                   * to this url in case we are a secondary emweb server
                   * in a distributed system.
                   */
                  prefix = ewaSecondaryPrefixAction( context
                                                    ,(const char *)vp );
                  if (prefix != NULL)
                    {
                      ewsBodySend( context, prefix );
                    }
                }
#             endif /* EW_CONFIG_OPTION_SECONDARY_ACTION */

              ewsBodySend(context, (char *)vp);
            }

#       ifdef EW_CONFIG_OPTION_CONVERT_XML
        /*
         * if converting to XML, send any tag info that trails the emweb
         * string output....
         */
        ewsServeXMLEmWebStringEnd( context, doc_context );
#       endif /* EW_CONFIG_OPTION_CONVERT_XML */

        doc_context->iterations++;
      }


#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    /*
     * If EmWeb String value is to be used to set the parameter for a
     * subsequent macro (instead of being written to the user agent)
     */
    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_SET_VALUE)
      {
        EwaStatus s;

        s = copyIntoStringValue(doc_context, (char *) vp, type);

        if (EWA_STATUS_OK != s)
          {
            EWS_GROUP_DYNAMIC_CLEANUP(context, vp);
            return(s);
          }
      }
#   endif
#   ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
done:;
#   endif
    /*
     * If non-repeating or NULL string, complete node
     */
    if (   vp == NULL
        || (doc_context->nodep->attributes & EW_DOCUMENT_NODE_REPEAT) == 0
       )
       {
         doc_context->iterations = 0;
         doc_context->node_count--;
         doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
       }

   /*
    * Flush output and return
    */
   EWS_GROUP_DYNAMIC_CLEANUP(context, vp);
   return ewsFlush(context);
}
#endif /* EW_CONFIG_OPTION_STRING */

#ifdef EW_CONFIG_OPTION_STRING_VALUE
/*
 * ewsServeEmWebStringValue
 * Handle <EMWEB_STRING value=''> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebStringValue ( EwsContext context, EwsDocumentContextP doc_context )
{
  EwsDocument   document = doc_context->document;
  uint32 offset;
  EW_UNUSED(context);

  offset = EW_BYTES_TO_UINT32(doc_context->nodep->index);
  doc_context->string_value = (char *)&document->archive_data[offset];
  doc_context->node_count--;
  doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
  return EWA_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_STRING_VALUE */

#ifdef EW_CONFIG_OPTION_INCLUDE
/*
 * ewsServeEmWebInclude
 * Handle <EMWEB_INCLUDE> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebInclude ( EwsContext context , EwsDocumentContextP doc_context )
{
  uint32 node_index;
  const char *cp;
  uintf hash;
  EwsDocument document;
  EwsStatus status;
  char *new_url = NULL;

    /*
     * Adjust state for proper handling of ewsNetHTTPAbort
     */
    context->state = ewsContextStateInclude;

#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    /*
      This happens if a namespace macro is invoked without an
      argument. Note that it is perfectly possible to have
      EW_DOCUMENT_NODE_CLEAR_VALUE and EW_DOCUMENT_NODE_SET_VALUE
      flagged for the same node. The first clears the string_value
      field before serving the node. The second sets the string_value
      field after serving the node.
      */
    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_CLEAR_VALUE)
      {
        doc_context->string_value = NULL;
      }

#   endif

    /*
     * Invoke <EMWEB_INCLUDE> C-code fragment
     */
    node_index = EW_BYTES_TO_UINT32(doc_context->nodep->index);
    cp = doc_context->document->archive->emweb_include( context, node_index );

    /*
     * Handle special cases for abort or suspend, otherwise
     * restore state
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return EWA_STATUS_ERROR;
      }
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return ewsFlushAll(context);
      }
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
    context->state = ewsContextStateServingDocument;

#   ifdef EW_CONFIG_FILE_METHODS
    if (context->fileParams != NULL)
      {
        if ((document = ewsFileSetupFilesystemDoc( context, context->url ))
              == EWS_DOCUMENT_NULL)
          {
            EMWEB_WARN(("ewsServeEmWebInclude: no resources\n"));
            return EWA_STATUS_ERROR;
          }
        doc_context->iterations++;
        status = ewsServeInclude(context, document);
        if (status != EWS_STATUS_OK)
          {
            return EWA_STATUS_ERROR;
          }
        cp = context->url;
      }
    else
#   endif /* EW_CONFIG_FILE_METHODS */
    {

    /*
     * If valid string returned
     */
    if (cp != NULL
        && 0 == (doc_context->nodep->attributes & EW_DOCUMENT_NODE_SET_VALUE)
       )
      {
        /*
         * If relative path, convert to absolute path
         */
        if (*cp != '/')
          {
            const char * base_url = doc_context->document->url;
            uintf base_bytes;
            uintf url_bytes;

              EMWEB_STRLEN(base_bytes, base_url);
              EMWEB_STRLEN(url_bytes, cp);
              for (;;)
                {
                /* First, remove the final component off the base. */
                /* (This loop depends on base_url[0] == '/'.) */
                while (base_url[base_bytes - 1] != '/')
                  {
                    base_bytes--;
                  }
                /* At this point, base_url[base_bytes-1] == '/'.
                 * Check to see if the new URL has a leading ".." that should
                 * be removed:
                 * URL must start with "../" (or ".." if its length is 2).
                 * base_url must have a component after the initial "/".
                 */
                if (!(url_bytes >=2 &&
                      cp[0] == '.' && cp[1] == '.' &&
                      (url_bytes == 2 || cp[2] == '/') &&
                      base_bytes > 1))
                  break;
                /* We now remove the ".." or "../". */
                cp += 2;        /* 2 == strlen("..") */
                url_bytes -= 2; /* 2 == strlen("..") */
                if (url_bytes > 0)
                  {
                    /* url_bytes was >= 3, so remove the "/" as well. */
                    cp += 1;            /* 1 == strlen("/") */
                    url_bytes -= 1;     /* 1 == strlen("/") */
                  }
                base_bytes -= 1;
                /* Having removed the final "/" from the base URL, return
                 * to the top of this loop to remove the final component
                 * of the base URL, then check again for "../" at the beginning
                 * of the new URL.
                 */
                }
              new_url = (char *) ewaAlloc(base_bytes + url_bytes + 1);
              if (new_url == (char *)NULL)
                {
                EMWEB_ERROR(("ewsServeEmWebInclude: no memory for new url"));
                return(EWA_STATUS_ERROR);
                }
              EMWEB_MEMCPY(new_url, base_url, base_bytes);
              EMWEB_STRCPY(new_url + base_bytes, cp);
              new_url[base_bytes + url_bytes] = '\0';
              cp = new_url;
          }

        /*
         * Lookup URL
         */
        hash = ewsFileHash(cp);
        document = ewsFileHashLookup(hash, cp);

#       ifdef EMWEB_SANITY
        /*
         * If URL not found, skip node
         */
        if (document == EWS_DOCUMENT_NULL ||
            ((document->doc_header->document_type & EW_ARCHIVE_DOC_TYPE_MASK)
              != EW_ARCHIVE_DOC_TYPE_MIME)
            )
          {
            EMWEB_WARN(("ewsServeEmWebInclude: '%s' not found or wrong type\n"
                        ,cp));
            cp = NULL;
          }
#       endif /* EMWEB_SANITY */

        /*
         * Release dynamically allocated URL if any
         *
         * Make sure this code is below the EMWEB_WARN above, since EMWEB_WARN
         * needs acces to cp, which may point to the dynamically allocated
         * memory pointed to by new_url.
         */
        if (new_url != NULL)
          {
            ewaFree(new_url);
          }

        /*
         * Push new URL onto processing stack
         */
        /* cp may have been set to NULL above if the URL does not identify
         * a valid document.
         */
        if (cp != NULL)
          {
            doc_context->iterations++;
            status = ewsServeInclude(context, document);
            if (status != EWS_STATUS_OK)
              {
                return EWA_STATUS_ERROR;
              }
          }
      }
    }

#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    /*
     * If EmWeb String value is to be used to set the parameter for a
     * subsequent macro (instead of being written to the user agent)
     */

    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_SET_VALUE)
      {
        EwaStatus        s;

        s = copyIntoStringValue(doc_context, cp, 0);

        if (EWA_STATUS_OK != s)
          return(s);
      }
#   endif /* EW_CONFIG_OPTION_STRING_VALUE */

     /*
     * If non-repeating or NULL string, complete node
     */
    if (   cp == NULL
        || (doc_context->nodep->attributes & EW_DOCUMENT_NODE_REPEAT) == 0
       )
       {
         doc_context->iterations = 0;
         doc_context->node_count--;
         doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
       }

   /*
    * Return and handle next state
    */
   return EWA_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_INCLUDE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
/*
 * ewsServeEmWebDynamicSelect
 * Handle <SELECT ... EMWEB_ITERATE...> node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebDynamicSelect ( EwsContext context, EwsDocumentContextP doc_context )
{
  uint32 node_index = EW_BYTES_TO_UINT32(doc_context->nodep->index);
  uintf form = EW_FORM_NODE_INDEX_TO_FORM_INDEX(node_index);
  uintf element = EW_FORM_NODE_INDEX_TO_ELEMENT_INDEX(node_index);
  const EwFormEntry *entryp = &doc_context->document->archive->emweb_form_table[form];
  const EwFormField *fieldp = &entryp->field_list[element-1];
  EwFieldType sub_type;
  EwsFormSelectOption option;
  EwFormFieldDynamicMultiSelect *multi_fieldP;
  EwFormFieldDynamicSingleSelect *single_fieldP;

    /*
     * Adjust state for proper handling of ewsNetHTTPAbort
     */
    context->state = ewsContextStateDynamicSelect;

    /*
     * Invoke iterator function
     */
    if (fieldp->field_type == ewFieldTypeDynamicSingleSelect)
      {
        single_fieldP =( EwFormFieldDynamicSingleSelect *)fieldp->default_value;
        /*
         * call iterator function
         */
        if (doc_context->iterations == 0)
          doc_context->iterationHandle =
            single_fieldP->iteration_f( context, &option, NULL );
        else
          doc_context->iterationHandle =
            single_fieldP->iteration_f
              (context, &option, doc_context->iterationHandle);
        sub_type =  single_fieldP->field_type;
      }
    else if (fieldp->field_type == ewFieldTypeDynamicMultiSelect)
      {
        multi_fieldP =( EwFormFieldDynamicMultiSelect *)fieldp->default_value;
        /*
         * call iterator function
         */
        if (doc_context->iterations == 0)
          doc_context->iterationHandle =
            multi_fieldP->iteration_f( context, &option, NULL );
        else
          doc_context->iterationHandle =
            multi_fieldP->iteration_f
              (context, &option, doc_context->iterationHandle);
        sub_type =  multi_fieldP->field_type;
      }
    else
      {
        return EWA_STATUS_ERROR;
      }

    /*
     * Handle special cases for abort or suspend, otherwise
     * restore state
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return EWA_STATUS_ERROR;
      }
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return ewsFlushAll(context);
      }
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
    context->state = ewsContextStateServingDocument;


    /*
     * Render the result, try XML first, if that is desired skip
     * the HTML version.
     */
    serveDynamicSelectHTML( context, doc_context, fieldp, entryp, &option, sub_type );

    /*
     * If non-repeating or NULL string, complete node
     */
    if (doc_context->iterationHandle == NULL)
       {
         doc_context->iterations = 0;
         doc_context->node_count--;
         doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
       }

   /*
    * Flush output and return
    */
   return ewsFlush(context);
}

static void
serveDynamicSelectHTML( EwsContext context, EwsDocumentContextP doc_context
                       ,const EwFormField *fieldp
                       ,const EwFormEntry *entryp
                       ,EwsFormSelectOption *option
                       ,EwFieldType sub_type
                      )
{
  /*
   * If valid string returned, copy it into outgoing
   * network buffers
   */

  EW_UNUSED(fieldp);
  EW_UNUSED(entryp);
  if (doc_context->iterationHandle != NULL)
    {
      (void) ewsBodySend(context, "\n<OPTION class=\"selectfield\" value=");    /*TRGTRG 10-apr-2003*/
      /*
       * Convert application value to HTML
       */
      if (option->valuep != NULL)
        {
          EwFormField field;
          uint8 *base;

          /* Text fields in R3_03 required valuep to be a char**,
           * following releases changed this to char * for consistency.
           * For backward compatibility with R3_03's char **, define the
           * symbol below.
           */
#         ifndef EW_CONFIG_OPTION_DYNSEL_PPCHAR
          if (sub_type == ewFieldTypeText)
            {
              base = (uint8 *) &option->valuep;
            }
          else
#         endif
            {
              base = (uint8 *) option->valuep;
            }

          field.value_offset = base - ((uint8 *) context->form);

          if (ews_state->form_field_table[sub_type].to_f != NULL)
            {
              ewsBodySend(context, "\"");
              ews_state->form_field_table[sub_type].to_f(context, &field);
              ewsBodySend(context, "\"");
            }
#           ifdef EMWEB_SANITY
          else
            {
              EMWEB_WARN(("ewFieldToUnsupported: EMWEB_TYPE unsupported\n"));
              ewsBodySend(context, "EMWEB_UNSUPPORTED");
            }
#           endif /* EMWEB_SANITY */
        }

      ewsBodySend(context, option->selected ? " SELECTED>" : ">");

      /* display "visual" text in OPTION tag.  Use text of VALUE= if
       * "choice" string is not supplied
       */
      if (option->choice != NULL)
        {
          ewsBodySend(context, option->choice);
        }
      else if (option->valuep != NULL)
        {
          EwFormField field;
          uint8 *base;

#           ifndef EW_CONFIG_OPTION_DYNSEL_PPCHAR
          if (sub_type == ewFieldTypeText)
            {
              base = (uint8 *) &option->valuep;
            }
          else
#           endif
            {
              base = (uint8 *) option->valuep;
            }

          field.value_offset = base - ((uint8 *) context->form);

          /*
           * we already made sure ..to_f exists
           */
          if (ews_state->form_field_table[sub_type].to_f != NULL)
            {
              ews_state->form_field_table[sub_type].to_f( context, &field);
            }
#           ifdef EMWEB_SANITY
          else
            {
              EMWEB_WARN(("ewFieldToUnsupported: EMWEB_TYPE unsupported\n"));
              ewsBodySend(context, "EMWEB_UNSUPPORTED");
            }
#           endif /* EMWEB_SANITY */
        }
      doc_context->iterations++;
    }
}



#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
/*
 * ewsServeEmWebPrefix()
 *
 * This function processes .._NAME(or VALUE)_PREFIX node.
 *
 * It inserts proper combination of numbers in to the outgoing string
 */
EwaStatus
ewsServeEmWebPrefix ( EwsContext context, EwsDocumentContextP doc_context )
{
  char          ltoa_string[32];
  char          *ltoa_ptr;

  /*
   * copy prefix it into outgoing
   * network buffers
   */
   EMWEB_LTOA( ltoa_ptr
              ,(uint32)(doc_context->rpt_body_number)
              ,ltoa_string ,sizeof(ltoa_string));
   ewsBodySend(context, ltoa_ptr);
   ewsBodySend(context, ".");
   EMWEB_LTOA( ltoa_ptr
              ,(uint32)(doc_context->rpt_body_iteration)
              ,ltoa_string ,sizeof(ltoa_string));
   ewsBodySend(context, ltoa_ptr);
   ewsBodySend(context, ".");
   EMWEB_LTOA( ltoa_ptr
              ,(uint32)(doc_context->rpt_body_total_repeat)
              ,ltoa_string ,sizeof(ltoa_string));
   ewsBodySend(context, ltoa_ptr);
   ewsBodySend(context, ".");

   doc_context->node_count--;
   doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );

   /*
    * Flush output and return
    */
   return ewsFlush(context);
}

/*
 * ewsServeEmWebRptBodyStart()
 *
 * This function processes .._START_RPT_BODY node.
 * There is nothing written to the outgoing buffer, but
 * informations are saved in the document context to
 * help processing repeated body properly.
 *
 * Upon termination of processing, we are going to advance to the next
 * node (part of repeat body).
 */


EwaStatus
ewsServeEmWebRptBodyStart ( EwsContext context, EwsDocumentContextP doc_context )
{
  uint32 node_index = EW_BYTES_TO_UINT32(doc_context->nodep->index);
  uintf element = EW_FORM_NODE_INDEX_TO_ELEMENT_INDEX(node_index);
  uintf form = EW_FORM_NODE_INDEX_TO_FORM_INDEX(node_index);
  const EwFormEntry *entryp = &doc_context->document->archive->emweb_form_table[form];
  const EwFormField *fieldp = &entryp->field_list[element-1];

   /*
    * save start info
    */
   if (doc_context->rpt_body_iteration == 0)
     {
       /*
        * first iteration
        */
       doc_context->rpt_body_number++;
       doc_context->rpt_fieldp =
         (EwFormRepeatField *)(fieldp->default_value);
       if (*(uintf *)((uint8 *)context->form+doc_context->rpt_fieldp->repeat_offset)
          == 0)
         {
           uint32    skipCount;  /* non-dynamic data bytes to skip */

           /*
            * The repeat count is zero, this means that nothing is output.
            * So, we need to skip all "nodes" related to this repeat block
            * AND we need to skip any non-dynamic data that appears
            * in the repeat block.
            */
           doc_context->rpt_body_skip = TRUE;
           skipCount = EW_BYTES_TO_UINT32(doc_context->nodep->data_offset);
           for ( (doc_context->node_count--,
                  doc_context->nodep = NEXT_DOCNODE( doc_context->nodep )
                 );
                 doc_context->nodep->type != EW_DOCUMENT_NODE_TYPE_END_RPT_BODY;
                 (doc_context->node_count--,
                 doc_context->nodep = NEXT_DOCNODE( doc_context->nodep )
                 )
               )
             ;

           skipCount = (EW_BYTES_TO_UINT32(doc_context->nodep->data_offset)
                        - skipCount
                       );

#          ifdef EW_CONFIG_OPTION_COMPRESS
           if (doc_context->doc_headerp->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
             {
               (void) ewDecompressRead( (EwDecompressContext) doc_context->compress_context
                                        ,NULL       /* discard data */
                                        ,skipCount
                                        );
             }
           else
#          endif /* EW_CONFIG_OPTION_COMPRESS */
             {
               doc_context->datap += skipCount;
             }
           doc_context->offset += skipCount;
           doc_context->remaining -= skipCount;

           return(EWA_STATUS_OK);
         }
       doc_context->rpt_body_start_node_count = doc_context->node_count;
       doc_context->rpt_body_start_nodep = doc_context->nodep;
       doc_context->rpt_body_start_offset = doc_context->offset;
       doc_context->rpt_body_start_remaining = doc_context->remaining;
       doc_context->rpt_body_start_datap = doc_context->datap;
#      ifdef EW_CONFIG_OPTION_COMPRESS
       if (doc_context->doc_headerp->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
         {
           ewDecompressSaveContext( (EwDecompressContext) doc_context->compress_context
                                    ,&doc_context->rpt_compress);
         }
#      endif /* EW_CONFIG_OPTION_COMPRESS */

       doc_context->rpt_body_total_repeat =
         *(uintf *)((uint8 *) context->form
           + doc_context->rpt_fieldp->repeat_offset);
       /*
        * do form pointer magic.
        * Until end of repeat body, the pointer point to
        * the substructure.
        */
       context->base_form = context->form;
       context->form_status = *((uintf **)((uint8 *)context->base_form +
                                fieldp->status_offset));
       context->form = *((uintf **)((uint8 *)context->form +
                         fieldp->value_offset));
     }
   else
     {
       context->form_status = (uint8 *)context->form_status +
                            doc_context->rpt_fieldp->status_substruct_size;
       context->form = (uint8 *)context->form +
                     doc_context->rpt_fieldp->value_substruct_size;
     }
   doc_context->node_count--;
   doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
   return(EWA_STATUS_OK);
}

/*
 * ewsServeEmWebRptBodyEnd()
 *
 *
 * This function processes .._END_RPT_BODY node.
 * There is nothing written to outgoing string
 * but decision is made about next iteration of
 * body processing.
 *
 * If there are more iterations, we have to go back
 * to the start node.
 */
EwaStatus
ewsServeEmWebRptBodyEnd ( EwsContext context, EwsDocumentContextP doc_context )
{
   if (doc_context->rpt_body_skip == TRUE)
     {
       doc_context->rpt_body_skip = FALSE;
       doc_context->node_count--;
       doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
       return(EWA_STATUS_OK);
     }
   doc_context->rpt_body_iteration++;
   if (doc_context->rpt_body_iteration < doc_context->rpt_body_total_repeat)
     {
       /*
        * more iterations to do, go back to repeat start node
        */
       doc_context->node_count = doc_context->rpt_body_start_node_count ;
       doc_context->nodep  = doc_context->rpt_body_start_nodep;
       doc_context->offset  = doc_context->rpt_body_start_offset;
       doc_context->remaining  = doc_context->rpt_body_start_remaining;
       doc_context->datap  = doc_context->rpt_body_start_datap;
#      ifdef EW_CONFIG_OPTION_COMPRESS
       if (doc_context->doc_headerp->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
         {
           ewDecompressRestoreContext( (EwDecompressContext) doc_context->compress_context
                                      ,&doc_context->rpt_compress);
         }
#      endif /* EW_CONFIG_OPTION_COMPRESS */
     }
   else
     {
       /*
        * cleanup
        */
       doc_context->rpt_body_iteration = 0;
       doc_context->rpt_body_total_repeat = 0;
       doc_context->rpt_body_start_node_count = 0;
       doc_context->rpt_body_start_nodep = NULL;
       doc_context->rpt_body_start_offset = 0;
       /*
        * restore base form pointer and move on
        */
       context->form = context->base_form;
       context->form_status = context->base_form;

       doc_context->node_count--;
       doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
     }
   return(EWA_STATUS_OK);
}
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#ifdef EW_CONFIG_OPTION_NS_GET
EwaStatus
ewsServeEmWebNamespaceGet ( EwsContext context, EwsDocumentContextP doc_context )
{
  uint32 indx;
  uint32 offset;
  uintf  ns_index;
  const EmWebNamespaceTable *ns;
  const char *cp;
  const char *nsData;
  const EwsDocumentNode *schemaNode;

    /*
     * Adjust state for proper handling of ewsNetHTTPAbort
     */
    context->state = ewsContextStateString;

    /*
     * Find and validate namespace table entry
     */
    indx = EW_BYTES_TO_UINT32(doc_context->nodep->index);
    ns_index = (indx >> 24);
    offset = (indx & 0x00ffffff);

#   ifdef EMWEB_SANITY
    if (ns_index >= doc_context->document->archive->emweb_namespace_table_size)
      {
        EMWEB_WARN(("ewsServeEmWebNamespaceGet: index out of bounds\n"));
        return EWA_STATUS_ERROR;
      }
#   endif /* EMWEB_SANITY */

    ns = &doc_context->document->archive->emweb_namespace_table[ns_index];

#   ifdef EMWEB_SANITY
    if (ns->get_f == NULL)
      {
        EMWEB_WARN(("ewsServeEmWebNamespaceGet: null get function\n"));
        return EWA_STATUS_ERROR;
      }
#   endif /* EMWEB_SANITY */

#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    /*
      This happens if a namespace macro is invoked without an
      argument. Note that it is perfectly possible to have
      EW_DOCUMENT_NODE_CLEAR_VALUE and EW_DOCUMENT_NODE_SET_VALUE
      flagged for the same node. The first clears the string_value
      field before serving the node. The second sets the string_value
      field after serving the node.
      */
    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_CLEAR_VALUE)
      {
        doc_context->string_value = NULL;
      }
#   endif

    /*
     * offset is the offset (base 1) of this element's schema node
     * in the list of schema nodes for the document.
     * lookup schema node for this element, and pull OID string out of
     * the data archive from the offset field given in the schema
     * node's index field
     */

#   ifdef EMWEB_SANITY
    if (doc_context->document->schema_nodes == NULL)
      {
        EMWEB_WARN(("ewsServeEmWebNamespaceGet: no schema nodes!\n"));
        return EWA_STATUS_ERROR;
      }
    if (offset == 0)
      {
        EMWEB_WARN(("ewsServeEmWebNamespaceGet: invalid schema node index!\n"));
        return EWA_STATUS_ERROR;
      }
#   endif

    schemaNode = (const EwsDocumentNode *)
      (  ((const char *)doc_context->document->schema_nodes)
       + (SIZEOF_EWS_DOCUMENT_NODE * (offset - 1))
      );
    offset = EW_BYTES_TO_UINT32(schemaNode->index);

    nsData = (const char *) &doc_context->document->archive_data[offset];

    /* setup context in case ewsContextSchemaInfo called.... */

    context->archiveData = doc_context->document->archive_data;
    context->schemaNodes = doc_context->document->schema_nodes;
    context->currentElement = schemaNode;

    /*
     * Invoke namespace get function
     */
    cp = ns->get_f( context
                   ,nsData
                   ,doc_context->string_value);

    /*
     * immediately clear current schema element so ewsContextSchemaInfo
     * returns failure if called not within ns get call
     */
    context->currentElement = NULL;

    /*
     * Handle special cases for abort or suspend, otherwise
     * restore state
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return EWA_STATUS_ERROR;
      }
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return ewsFlushAll(context);
      }
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

    EWS_GROUP_DYNAMIC_CHECK(context, cp);

    context->state = ewsContextStateServingDocument;

    /*
     * If valid string returned (and writing to buffer)
     */
    if (cp != NULL
        && 0 == (doc_context->nodep->attributes & EW_DOCUMENT_NODE_SET_VALUE)
       )
      {
#       ifdef EW_CONFIG_OPTION_CONVERT_XML
        /*
         * if converting to XML, send the string out via
         * the "XML preprocessor"
         */
        ewsServeXMLNamespaceBegin( context, doc_context );
#       endif /* EW_CONFIG_OPTION_CONVERT_XML */

#       ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
        /*
         * If outputting skeleton, output only the REFRESH tag.
         */
        if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
          {
            if (ewsServeSkeletonEmWebNamespace(context, doc_context))
              goto done;
          }
#       endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */

        ewsBodySend(context, (char *) cp);

#       ifdef EW_CONFIG_OPTION_CONVERT_XML
        /*
         * if converting to XML, send any tag info that trails
         * the namespace string output....
         */
        ewsServeXMLNamespaceEnd( context, doc_context );
#       endif /* EW_CONFIG_OPTION_CONVERT_XML */
      }

#   ifdef EW_CONFIG_OPTION_STRING_VALUE
    /*
     * If EmWeb String value is to be used to set the parameter for a
     * subsequent macro (instead of being written to the user agent)
     */
    if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_SET_VALUE)
      {
        EwaStatus s;

        s = copyIntoStringValue(doc_context, cp, 0);

        if (EWA_STATUS_OK != s)
          {
            EWS_GROUP_DYNAMIC_CLEANUP(context, cp);
            return(s);
          }
      }
#   endif
#   ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
done:;
#   endif
    /*
     * Complete node
     */
    doc_context->iterations = 0;
    doc_context->node_count--;
    doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );

   /*
    * Return and handle next state
    */
   EWS_GROUP_DYNAMIC_CLEANUP(context, cp);
   return EWA_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_NS_GET */

#ifdef EW_CONFIG_OPTION_FORM
/*
 * ewsServeEmWebForm
 * Handle FORM serve node
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Returns EwaStatus from application send function, or appropriate
 * status generated internally.
 */
EwaStatus
ewsServeEmWebForm ( EwsContext context, EwsDocumentContextP doc_context )
{
  uint32 node_index;

    /*
     * Adjust state for proper handling of ewsNetHTTPAbort
     */
    context->state = ewsContextStateServingForm;

    /*
     * Invoke compiler-generated C-code fragment
     */
    node_index = EW_BYTES_TO_UINT32(doc_context->nodep->index);

    if (EW_FORM_NODE_INDEX_TO_FORM_INDEX(node_index) !=  EW_FORM_INDEX_NAMESPACE)
      {
        /* only Serve NON-NAMESPACE forms */
        ewsFormServe(context, doc_context, node_index);
      }

    /*
     * Handle special cases for abort or suspend, otherwise
     * restore state
     */
    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        return EWA_STATUS_ERROR;
      }
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return ewsFlushAll(context);
      }
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
    context->state = ewsContextStateServingDocument;

    doc_context->node_count--;
    doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );

    /*
     * Flush output and return
     */
    return ewsFlush(context);
}
#endif /* EW_CONFIG_OPTION_FORM */

#ifdef EW_CONFIG_OPTION_COMPRESS
sintf
ewDecompressInput( void * handle )
{
  EwsDocumentContextP doc_context = (EwsDocumentContextP) handle;

    return *(doc_context->datap++);
}
#endif /* EW_CONFIG_OPTION_COMPRESS */


/*
 * ewsServeEmWebConstString
 * Handle node for serving a constant string from the data archive.
 *
 * context      - context of request
 * doc_context  - context of current document being served
 *
 * Writes the constant string out, and returns EWA_STATUS_OK;
 */
static EwaStatus
ewsServeEmWebConstString( EwsContext context, EwsDocumentContextP doc_context )
{
  EwsDocument   document;
  uint32        offset;
  char          *data;

  /* get data string (null terminated) from data archive */

  document = doc_context->document;
  offset = EW_BYTES_TO_UINT32(doc_context->nodep->index);
  data = (char *)&document->archive_data[offset];

# ifdef EW_CONFIG_OPTION_SECONDARY_ACTION
  if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_FORM_ACTION)
    {
      const char *prefix;
      /*
       * this string is really the contents of a form's ACTION=
       * attribute (these are always emweb strings).
       * Give the user a chance to prefix a special path
       * to this url in case we are a secondary emweb server
       * in a distributed system.
       */
      prefix = ewaSecondaryPrefixAction( context
                                        ,data
                                       );
      if (prefix)
        {
          ewsBodySend( context, prefix );
        }
    }
# endif /* EW_CONFIG_OPTION_SECONDARY_ACTION */

# ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
  /* If serving skeleton XML, wrap the constant string in a CDATA section. */
  if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
    {
      (void) ewsServeSkeletonCDATABegin(context, doc_context, 10000);
    }
# endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */
  ewsBodySend(context, data);           /* write out data */
# ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
  if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
    {
      ewsServeSkeletonCDATAEnd(context, doc_context);
    }
# endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */

  doc_context->node_count--;
  doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );

  return EWA_STATUS_OK;
}


#ifdef EW_CONFIG_OPTION_FILE_GET

EwaStatus
ewsServeLocalFile( EwsContext context, EwsDocumentContextP doc_context )
{
  EwsDocument   document = doc_context->document;
  sintf         status;
  uintf         length;
  uintf         totalWritten;
  EwaFileHandle handle;

  /*
   * need to open the file first if it hasn't been opened already
   */

  if (doc_context->fileHandle == EWA_FILE_HANDLE_NULL)
    {
#     ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
        {
          if (context->fileSystem->fs.fileGet == NULL)
            {
              EMWEB_ERROR(("ewsServeLocalFile: NULL fs fileGet function\n"));
              handle = EWA_FILE_HANDLE_NULL;
            }
          else
            {
              handle = context->fileSystem->fs.fileGet( context
                                                       ,document->url
                                                       ,document->fileParams
                                                  );
            }
         }
      else
         {
           handle = EWA_FILE_HANDLE_NULL;
         }
#     else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
        handle = ewaFileGet( context
                            ,document->url
                            ,document->fileParams
                           );
#     endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

#     ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
      if (context->schedulingState == ewsContextSuspended)
        {
          return ewsFlushAll(context);
        }
      context->schedulingState = ewsContextScheduled;
#     endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

      /* check for abort/error */

      if (handle == EWA_FILE_HANDLE_NULL)
        {
#         ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
          if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
            {
              /* we will abort without closing the file so */
              (context->fileSystem->use_cnt)--;
              context->fs_inuse = FALSE;
            }
#         endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
          EMWEB_TRACE(("ewsServeLocalFile: ewaFileGet return null handle\n"));
          ewsNetHTTPAbort( context );
          return EWA_STATUS_ERROR;
        }

      doc_context->fileHandle = handle;
#     ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      doc_context->fileSystem = context->fileSystem;
      doc_context->fs_inuse = context->fs_inuse;
      context->fs_inuse = FALSE;
#     endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

      /*
       * If connection aborted, return to scheduler
       */
      if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
        {
          return EWA_STATUS_ERROR;
        }
    }

# ifdef EW_CONFIG_OPTION_CHUNKED_OUT
  /*
   * Must manually add the chunk header, since
   * we are not going through the standard output routines
   * (that would require an extra data copy)
   */
  if ( ewsContextChunkOutNewChunk == context->chunk_out_state )
    {
      ewsBodyNewChunkHeader(context);
    }
# endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

  /* attempt to attach a buffer - will do nothing if there is already
   * some free space left in the buffer - guarantees length will not
   * be zero.  If we are chunked, *try* to reserve two bytes at the
   * end of the buffer so we don't have to allocate a whole new
   * buffer for the \r\n end of chunk (if we can't, cest la vie)
   */
  if (ewsStringAttachBuffer( &context->res_str_end ) == EWA_NET_BUFFER_NULL)
    {
      EMWEB_TRACE(("ewsServeLocalFile: no net buffers\n"));
      ewsNetHTTPAbort( context );
      return EWA_STATUS_ERROR;
    }

  /* get # free bytes in buffer */
  length = ewsStringAvailable( &context->res_str_end );

# ifdef EW_CONFIG_OPTION_CHUNKED_OUT
  /* try to reserve space for /r/n for end of chunk, if not
   * enuff room, we'll just allocate another buffer later
   */
  if (   ( ewsContextChunkOutNotChunked != context->chunk_out_state )
      && ( length > 2 )
      )
    {
      length -= 2;
    }
# endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

  totalWritten = 0;

  /*
   * now try to fill the entire buffer.  Keep reading until:
   * 1) buffer is filled
   * 2) EOF is reached
   * 3) context is suspended
   * 4) Error
   */
  do
    {
      uint8*     buffer_space;

      /* start of free space to write to */
      buffer_space = (  ewsStringData(&context->res_str_end)
                      + totalWritten
                      );

#     ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      if (doc_context->fileSystem == EWS_FILE_SYSTEM_HANDLE_NULL)
        {
          EMWEB_TRACE(("ewsServeLocalFile: file system handle NULL\n"));
          return EWA_STATUS_ERROR;
        }
      if (context->fileSystem->fs.fileRead == NULL)
        {
          EMWEB_ERROR(("ewsServeLocalFile: NULL fs fileRead function\n"));
          return EWA_STATUS_ERROR;
        }
      if ((status = doc_context->fileSystem->fs.fileRead(
                                 context
                                ,doc_context->fileHandle
                                ,buffer_space
                                ,length
                                )
           )
          < 0)      /* ERROR!! */
#     else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
      if ((status = ewaFileRead(
                                 context
                                ,doc_context->fileHandle
                                ,buffer_space
                                ,length
                                )
           )
          < 0)      /* ERROR!! */
#     endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
        {
          /*
           * abort will close the file...
           */
          EMWEB_TRACE(("ewsServeLocalFile: error returned from "
                       "ewaFileRead\n"));
          return EWA_STATUS_ERROR;
        }
#     ifdef EMWEB_SANITY
      if ((uintf)status > length )
        {
          EMWEB_ERROR(("ewsServeLocalFile: file buffer overflow\n"));
        }
#     endif
      if ( status > 0 )
        {
          length -= status;     /* adjust length by the #bytes written */
          totalWritten += status;
        }

      /*
       * Check for abort state
       */
      if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
        {
          return EWA_STATUS_ERROR;
        }
    } while (   length != 0     /* buffer not filled */
             && status != 0     /* EOF not reached */
#            ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
             && context->schedulingState != ewsContextSuspended
#            endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
            );

  /*
   * if bytes of data written, then adjust res_str_end to
   * reflect the amount of data written
   */

  if (totalWritten)
    {
      /* modify res_str_end to appear as if we used the
       * ewsString[N]CopyIn routines to get data into the buffer
       * like everyone else.
       */
#     ifdef EW_CONFIG_BODY_DIGEST
      /*
       * run the digest algorithm over the generated body
       */
      ewsBodyDigest( context
                    ,ewsStringData(&context->res_str_end)
                    ,totalWritten
                    );
#     endif /* EW_CONFIG_BODY_DIGEST */

      length = ewsStringLength(&context->res_str_end) + totalWritten;
      ewsStringLengthSet(&context->res_str_end, length);
      ewsStringTrim(&context->res_str_end, totalWritten);  /* undoes length */
      ewsStringLengthSet(&context->res_str_end, length);
    }

  /*
   * deal with suspension
   */

# ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
  if (context->schedulingState == ewsContextSuspended)
    {
      /*
       * if suspended, flush finished buffers and scram
       */
      return ewsFlushAll( context );
    }

  /*
   * reset the state in case it was "resumed"
   */
  context->schedulingState = ewsContextScheduled;
# endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */


  /*
   * if EOF...
   */
  if (status == 0)
    {
      /*
       * EOF reached, reset remaining byte count to zero (so this
       * routine will not be called again), close the file.
       */
      doc_context->remaining = 0;
#     ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      if (doc_context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
        {
          (doc_context->fileSystem->use_cnt)--;
          doc_context->fs_inuse = FALSE;
          if (doc_context->fileSystem->fs.fileClose == NULL)
            {
              EMWEB_ERROR(("ewsServeLocalFile: NULL fs fileClose function\n"));
            }
          else
            {
              doc_context->fileSystem->fs.fileClose( doc_context->fileHandle
                                                    ,EWS_STATUS_OK );
            }
         }
#     else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
      ewaFileClose( doc_context->fileHandle, EWS_STATUS_OK );
#     endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
      doc_context->fileHandle = EWA_FILE_HANDLE_NULL;
    }

  return ewsFlushAll(context);
}
#endif /* EW_CONFIG_OPTION_FILE_GET */


/*
 * ewsFreeDocContext
 * Free the memory allocated for the document context.  Be sure to
 * correctly free up substructures and other fields
 */
void ewsFreeDocContext( EwsDocumentContextP doc_context )
{
  if (doc_context->compress_context != NULL)
    {
      ewaFree( doc_context->compress_context );
    }

  /* if a filesystem document, then free the dummy
   * document header
   */
# ifdef EW_CONFIG_FILE_METHODS
  if (   doc_context->document
      && (doc_context->document->flags & EWS_FILE_FLAG_FILESYSTEM)
     )
    {
      ewaFree( doc_context->document );
    }

  if (doc_context->fileHandle != EWA_FILE_HANDLE_NULL)
    {
#     ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      if (doc_context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
        {
          (doc_context->fileSystem->use_cnt)--;
          doc_context->fs_inuse = FALSE;
          if (doc_context->fileSystem->fs.fileClose == NULL)
            {
              EMWEB_ERROR(("ewsFreeDocContext: NULL fs fileClose function\n"));
            }
          else
            {
              doc_context->fileSystem->fs.fileClose( doc_context->fileHandle
                                                    ,EWS_STATUS_ABORTED );
            }
        }
#     else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
      ewaFileClose( doc_context->fileHandle, EWS_STATUS_ABORTED );
#     endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    }
# endif /* EW_CONFIG_FILE_METHODS */

# ifdef EW_CONFIG_OPTION_ITERATE
  /*
   * free up any nested iterate context structure
   */
  while (doc_context->iterate != NULL)
    {
      EwDocIterateContext *tmp;

      tmp = doc_context->iterate;
      doc_context->iterate = doc_context->iterate->stack;
      ewaFree( tmp );
    }
# endif /* EW_CONFIG_OPTION_ITERATE */

# ifdef EW_CONFIG_OPTION_STRING_VALUE
  if (doc_context->string_buffer)
    {
      ewaFree(doc_context->string_buffer);
    }
# endif /* EW_CONFIG_OPTION_STRING_VALUE */

  ewaFree( doc_context );
}

#ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
/**********************************************************************
 *
 * ewsSearchMatchHdr
 *
 *  Purpose: Search a if-match or if-none-match header for a matching ETAG
 *           or a * character.
 *
 *  Inputs: eTag - the eTag to search for.
 *          inputString - emweb string containing the if-match or
 *                  if-not-match request header that will be searched.
 *          context - EmWeb context
 *
 *  Returns:   status - indicates whether found:
 *                 a matching Etag
 *                 a  * (wildcard) but no exact match
 *                 no match
 *
 * Examples of  If-Match: or If-None-Match headers :
 *   If-Match : "abcde"
 *   If-Match : "12345", "abcdef", "xxyyzz"
 *   If-Match :  W/"abcde"
 *   If-Match : *
 *   If-None-Match: W/          "TESTSPACESBEFORE"
 */

static EwsMatchStatus
ewsSearchMatchHdr( const char*     eTag
                  ,EwsString       inputString
                  ,EwsContext      context
                  )
{
  EwsString string;
  EwsString end;
  uintf match_c;  /* matching character for parsing entries */
  uint32 len;
  EwsMatchStatus matchStatus;
  boolean stringIsToken;

  EW_UNUSED( context );

  matchStatus = EtagNoMatch;

  string = end = inputString;
  len = ewsStringLength(&string);

  while ( len && ( EtagMatched != matchStatus ) )
    {
      /*
       * The goal now is to scan forward in 'string' for the next etag
       * value or wildcard.  By the bottom of this loop
       *  len => remaining bytes in header value
       *  end => the header past the current value, if any
       * If 'stringIsToken == TRUE', then
       *  string => the next etag value from the header
       */
      stringIsToken = FALSE;

      /* Search up to first non white space character or , */
      match_c = ewsStringSearch( &string
                                ,&end
                                ,(char *)" \t\r\n,"
                                ,ewsStringSearchExclude
                                ,&len
                                );

      if ( EWS_STRING_EOF == match_c )
        {
          /*
           * End of header value - everything left was whitespace
           * set remaining len to zero so we get out of the loop
           */
          ewsStringLengthSet(&end, 0);
        }
      else if ( '"' == (char)match_c )
        {
          /* nice proper quoted string */
          ewsStringGetChar(&end); /* gobble open quote */
          len--;

          /* reset string for etag token and read etag up to terminal quote */
          string = end;
          ewsStringLengthSet(&string, 0);
          match_c = ewsStringSearch( &string
                                    ,&end
                                    ,(char *)"\""
                                    ,ewsStringSearchInclude
                                    ,&len
                                    );
          if ( EWS_STRING_EOF != match_c )
            {
              stringIsToken = TRUE;
              ewsStringGetChar(&end); /* gobble close quote */
            }
        }
      else if ( '*'  == (char)match_c ) /* check for wildcard match */

        {
          matchStatus = EtagWildCard;
          ewsStringGetChar(&end); /* gobble wildcard */
        }
      else
        {
          /*
           * This is not either a wildcard or a quoted string,
           * so it's either a weak etag or a syntax error
           */
          /* A Weak etag looks like W/"<string>".*/
          if ( ewsStringCompare( &string, "W/\""
                                ,ewsStringCompareCaseSensitive
                                ,NULL
                                )
              )
            {
              /*
               * Since we never generate these, they cannot ever match,
               * so we skip over and ignore this value.
               * advance past the close quote
               */
              match_c = ewsStringSearch( &string
                                        ,&end
                                        ,(char *)"\""
                                        ,ewsStringSearchInclude
                                        ,&len
                                        );
            }
          else
            {
              /*
               * RFC2616 specifies that Etag values are contained in quoted
               * strings.  However, it's easy enough to implement the parser
               * to be more permissive.  This token did not start in one of
               * the legal ways, so skip to whitespace, EOF, or comma.
               */
              string = end;
              ewsStringLengthSet(&string, 0);
              match_c = ewsStringSearch( &string
                                        ,&end
                                        ,(char *)" \t\r\n,"
                                        ,ewsStringSearchInclude
                                        ,&len
                                        );
              stringIsToken = TRUE; /* be liberal... */
            }
        }

      /* If a token was found, compare it to the target etag value */
      if (   eTag && stringIsToken
          && ewsStringCompare( &string, eTag
                              ,ewsStringCompareCaseSensitive, NULL
                              )
          && ( ! ewsStringLength(&string) /* not a substring match */ )
          )
        {
          matchStatus = EtagMatched;
        }

      /* Reset to past what we just saw for next iteration */
      string = end;
      len = ewsStringLength(&string);
    } /* END WHILE */

  return matchStatus;
}
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */

#ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
/**********************************************************************
 *
 * ewsHandleMatchHdr
 *
 *  Purpose: Handle an if-match, if-none-match header. Search for an
 *           ETAG in the header, and based upon what you find (*, etag match,
 *           or no match),the type of request and the
 *           and the type of conditional, return TRUE if should continue
 *           processing, or setup the appropriate header and return FALSE
 *           to indicate no more processing should take place.
 *
 *  Inputs: context - the document context to determine the type of
 *              header and to get the header estring to parse.
 *          document - the document we are processing.  Get the
 *              eTag from the document.
 *          matchType - the type of conditional we are processing -
 *              either it is an if-match or if-none-match.
 *
 *  Outputs:   none.
 *
 *  Return:  TRUE indicates continue processing the document.
 *           FALSE indicates some condition found and processing must stop.
 *
 *  Note:  See RFC2068 chapters 13,14 for rules on how if-match/if-none-match
 *       conditionals are to be handled.
 *
 */
static boolean  ewsHandleMatchHdr( EwsContext context
                                  ,EwsDocument document
                                  ,EwsMatchType matchType
                                  )
{
  boolean return_code=FALSE;
  EwsString matchValues;
  const char *eTag = NULL;
  EwsMatchStatus matchStatus;

  switch(matchType)
    {
    case IfMatch_Conditional:
      matchValues = context->req_headerp[ewsRequestHeaderIfMatch];
      break;

    case IfNoneMatch_Conditional:
      matchValues = context->req_headerp[ewsRequestHeaderIfNoneMatch];
      break;

#   ifdef EMWEB_SANITY
    default:
      EMWEB_ERROR(("ewsHandleMatchHdr: invalid type of conditional\n"));
      return FALSE;
      break;
#   endif /* EMWEB_SANITY */

    }

  /* now get the etag */
# ifdef EW_CONFIG_FILE_METHODS
  if (document->flags & EWS_FILE_FLAG_FILESYSTEM)
    {
      /*
       * For file system documents, we use the eTag value in the file params
       * provided by the application.  It is possible that the corresponding
       * file doesn't actually exist.  This is indicated by a NULL eTag
       * value. Otherwise, an empty "" eTag or an actual eTag value would have
       * been provided.  Set the document handle to NULL if the eTag is NULL,
       * otherwise replace the eTag with NULL if it was empty "".
       */
      eTag=document->fileParams->fileInfo.eTag;
      if (eTag == NULL)
        {
          document = NULL;
        }
      else if (eTag[0] == '\0')
        {
          eTag = NULL;
        }
    }
  else
# endif /*EW_CONFIG_FILE_METHODS */
    {
      eTag=ewsRetrieveEtag(context, document);
    }

  /*
   * Search for eTag in matchValues
   */
  matchStatus = ewsSearchMatchHdr( eTag, matchValues, context );

  /*
   * For "readability", separate handling of  the Ifmatch and Ifnonematch
   *  status.
   *
   *  if-match: if you get a match, or a * , and the representation
   *  exists, perform the requested method.  Otherwise, server MUST NOT
   *  perform the method and must return 412 response.
   *  HTTP 1.1 requires that "A server MUST use the strong
   *  comparison function to compare entity tags in If-Match."
   */
  if (IfMatch_Conditional==matchType)
    {
      switch(matchStatus)
        {
        case EtagMatched:
          return_code = TRUE;
          break;

        case EtagNoMatch:
          ewaLogHook(context, EWS_LOG_STATUS_PRECONDITION_FAILED);
          ewsSendHeaderAndEnd( context
                              ,ews_http_status_412
                              ,ews_http_string_412);
          return_code = FALSE;
          break;

        case EtagWildCard:
          /* Perform method if representation exists.
           * If * and no current entity exist, the server MUST NOT perform the
           * requested method and MUST return 412(Precondition Failed) response.
           */
          if (document == EWS_DOCUMENT_NULL)
            {
              ewaLogHook(context,
                         EWS_LOG_STATUS_PRECONDITION_FAILED);
              ewsSendHeaderAndEnd(context
                                  ,ews_http_status_412
                                  ,ews_http_string_412);
              return_code = FALSE;
            }
          else
            {
              /* If document exists, and get *, perform the method */
              return_code = TRUE;
            }
          break;

#       ifdef EMWEB_SANITY
        default:
          /* This should never happen ! */
          EMWEB_ERROR(("ewsHandleMatchHdr: ewsSearchMatchHdr INVALID status!\n"));
          return_code = FALSE;
          break;
#       endif /* EMWEB_SANITY */
        } /* END switch */
    }  /*END  if matchType == IfMatch_Conditional*/
  else
    {
      /* handle status for matchType == IfNoneMatch_Conditional
       *
       *  if-none-match: If you get match, or a "*" AND the entity exists, then
       *  do NOT perform the requested method.
       *
       * Http1.1:
       *   if any of the etags match the etag of the entity that would bave
       *   been returned in the response to a similar GET request (w/o the
       *   if-none-match header) on that resource, or if "*" is given and
       *   any current entity exists for that resource, then the server
       *   must NOT perform the requested method.
       *   Instead, if the response was a get or head, the server should
       *   respond w/ a 304 (Not Modified) respnse.
       *   For all other request methods, the server MUST
       *   respond w/ a status of 412 (precondition failed).
       */
      switch(matchStatus)
        {
        case EtagNoMatch:
          /* if none of the entity tags match, or if * is given and no
             current entity exists, then the server MAY perform the
             requested method as if the If-None-Match header field did
             not exist.
             */
          return_code = TRUE;
          break;

        case EtagWildCard:
          /* if none of the entity tags match, or if * is given and no
             current entity exists, then the server MAY perform the
             requested method as if the If-None-Match header field did not
             exist.
             */
          if (NULL  == document)
            {
              return_code = TRUE;
              break;
            }
          /* else FALL THROUGH TO ETAG_MATCH_FOUND */

        case EtagMatched:
          /* For ifNoneMatch: if GET or HEAD, return 304,else return 412.
             Do not continue (return FALSE).
             */
          return_code = FALSE;
          /* should only get weak etag match with GET or HEAD */
          if (   (context->req_method == ewsRequestMethodHead)
              || (context->req_method == ewsRequestMethodGet)
              )
            {
              ewaLogHook(context, EWS_LOG_STATUS_NOT_MODIFIED);
              ewsSendHeaderAndEnd(context, ews_http_status_304, ews_no_body);
            }
          else
            {
              /* should never happen if we get to this function, since the
                 document must exist if we get here !
                 */
              ewaLogHook(context, EWS_LOG_STATUS_PRECONDITION_FAILED);
              ewsSendHeaderAndEnd(context
                                  ,ews_http_status_412
                                  ,ews_http_string_412);
            }
          break;

#       ifdef EMWEB_SANITY
        default:
          /* This should never happen ! */
          EMWEB_ERROR(("ewsHandleMatchHdr: ewsSearchMatchHdr INVALID status!\n"));
          return_code = FALSE;
          break;
#       endif /* EMWEB_SANITY */
        } /* end of switch */

    } /* END else IFNONEMATCH */
  return(return_code);
} /* END FUNCTION ewsHandleMatchHdr */
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */

#   if defined (EW_CONFIG_OPTION_CONDITIONAL_GET) ||\
       defined (EW_CONFIG_OPTION_CONDITIONAL_MATCH)
/*
 * ewsServeConditionalCheck
 *
 * Check preconditions (i.e. if-modified-since, if-match, if-none-match)
 * and return TRUE if preconditions pass, else FALSE.
 */
boolean
ewsServeConditionalCheck(EwsContext context, EwsDocument document)
{
  const EwsDocumentHeader *doc_header = document->doc_header;
# ifdef EW_CONFIG_FILE_METHODS
  uint32 doc_type = doc_header->document_type & EW_ARCHIVE_DOC_TYPE_MASK;
  EW_UNUSED(doc_type); /* may be unused depending on file options configured */
# endif /* EW_CONFIG_FILE_METHODS */

# ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
  /* If-Match:
   * If-Match can be one of the following formats
   * If-Match : "abcde"
   * If-Match : "12345", "abcdef", "xxyyzz"
   * If-Match : *
   *
   * NOTE for first implementation the ETAGs we generate will be 6chars
   * 5 chars+null, where chars are base64 bit encoded value.
   * However, do not assume anything about the ETAGS in the "if-match"
   * header other than that they should be in quotes.
   *
   * HTTP 1.1 requires that "A server MUST use the strong
   * comparison function to compare entity tags in If-Match."
   * we never want to compare against weak etags for if-match!
   *
   * perform method if representation exists.
   * Otherwise, server MUST NOT perform the method
   * and must return 412 response.
   */
  if (ewsStringLength(&context->req_headerp[ewsRequestHeaderIfMatch]) != 0)
    {
      boolean status;
      status = ewsHandleMatchHdr(context,document, IfMatch_Conditional);
      if (status == FALSE)
        {
          return(FALSE);
        }
    }
  else
# endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */
    {
# ifdef EW_CONFIG_OPTION_CONDITIONAL_MATCH
      /*
       * Check if-none-match header
       */
      if (ewsStringLength(&context->req_headerp[ewsRequestHeaderIfNoneMatch])
          != 0
          )
        {
          boolean status;
          status=ewsHandleMatchHdr(context, document, IfNoneMatch_Conditional);
          if (status == FALSE)
            {
              return(FALSE);
            }
        }
      else
# endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH */
        /*
         * The If-Match and If-None-Match conditionals are 'strong' validators
         * and the If-Modified-Since is weak, so if the strong ones are present
         * they are the only ones that should be used; only if they were
         * not in the request do we check for a date as validator.
         */
        {

#         ifdef EW_CONFIG_OPTION_CONDITIONAL_GET
          {
            boolean   notModified = FALSE;
            /*
             * If If-Modified-Since: header present and document contains no
             * nodes for dynamic content, then compare (we use strict equals
             * which works on most browsers - not the end of the world if it
             * doesn't) the archive creation date with the If-Modified-Since:
             * field value.  If they are equal, then so indicate by sending a
             * Not Modified response.
             */
            if (ewsStringLength( &context->req_headerp[ewsRequestHeaderIfModifiedSince])
                != 0)
              {
                EwsString string;

                string = context->req_headerp[ewsRequestHeaderIfModifiedSince];

#               ifdef EW_CONFIG_FILE_METHODS
                if (doc_type == EW_ARCHIVE_DOC_TYPE_FILE)
                  {
                    if (document->fileParams->fileInfo.lastModified != NULL)
                      {
                        notModified
                          = ewsStringCompare( &string
                                              ,document->fileParams->fileInfo.lastModified
                                              ,ewsStringCompareCaseInsensitive
                                              ,NULL);
                      }
                    if (!notModified &&
                        document->fileParams->fileInfo.lastModified1036 != NULL)
                      {
                        notModified
                          = ewsStringCompare( &string
                                              ,document->fileParams->fileInfo.lastModified1036
                                              ,ewsStringCompareCaseInsensitive
                                              ,NULL);
                      }
                  }
                else
#               endif /* EW_CONFIG_FILE_METHODS */
                  {
                    if (   EW_BYTES_TO_UINT16(doc_header->node_count) == 0
                           || ((  doc_header->document_type
                                  & EW_ARCHIVE_DOC_FLAG_STATIC)
                               != 0)
                           )
                      {
                        notModified =
                          (   ewsStringCompare( &string
                                                ,ewsDocumentArchiveDate(document->archive_data)
                                                ,ewsStringCompareCaseInsensitive
                                                ,NULL)
                              || ewsStringCompare( &string
                                                   ,ewsDocumentArchiveDate1036(document->
                                                                               archive_data)
                                                   ,ewsStringCompareCaseInsensitive
                                                   ,NULL)
                              );
#                      ifndef EW_CONFIG_OPTION_BROKEN_IMS_EXTRA_DATA
                        /*
                         * Many browsers violate the HTTP specifications by
                         * appending extra data after the date specified in
                         * If-Modified-Since: headers (usually '; length=nnn').
                         * If we are being strict, make sure there is nothing
                         * following the date.
                         */
                        if ( notModified && ewsStringLength( &string ) != 0)
                          {
                            notModified = FALSE;
                          }
#                       endif /* ! EW_CONFIG_OPTION_BROKEN_IMS_EXTRA_DATA */
                      }
                  }
              }

            if (notModified)      /* send Not Modified response */
              {
                ewaLogHook(context, EWS_LOG_STATUS_NOT_MODIFIED);
                ewsSendHeaderAndEnd(context, ews_http_status_304, ews_no_body);
                return FALSE;
              }
          }
#         endif /* EW_CONFIG_OPTION_CONDITIONAL_GET */

        }
    }

  return TRUE;
}
#endif /* EW_CONFIG_OPTION_CONDITIONAL_GET | MATCH */


#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
/*
 * ewsSelectVariant
 *
 * Because the requested document is a negotiable resource, this function
 * will choose the appropriate response based on the combination of
 * Negotiate and Accept headers and the list of variants.
 *
 * Inputs: context - the request context
 *         document - ptr to doc loaded in filesystem
 *         doc_header - ptr to doc info in data archive
 *
 * Outputs: if the function is not allowed to select a response or isn't able
 *          to select a variant with a quality rating greater than 0, returns
 *          ewsDocSelectStateFinish and sets the TCN, Alternates, and Vary
 *          headers to respond with the requested document as the
 *          list-response.
 *
 *          if function is able to select a variant with a quality rating
 *          greater then 0, returns ewsDocSelectStateFindDocument, sets the
 *          TCN, Vary and Alternates headers, and serves the selected document
 *          as the choice-response.
 *
 */
static boolean
ewsSelectVariant( EwsContext context,
                  EwsDocument document,
                  const EwsDocumentHeader *doc_header)
{
  EwsDocumentNode      *nodePtr;
  EwsNegotiateHdrData  *neg_hdr;
  char                 *alt_hdr = NULL;   /* Alternates header */
  char                 *vary_hdr;
  ewsQualType          choice_qual_type = ewsQualTypeNone;
  uint32               offset;
  uint32               variant_block_offset;
  uint32               alt_str_offset;
  uint32               choice_attr_block_offset = 0;
  uint32               choice_quality = 0;
  uintf                nodes = EW_BYTES_TO_UINT16(doc_header->node_count);
  uintf                i;
  uintf                status;
  boolean              sel = FALSE;

# ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA

  uint32               attr_block_offset;
  uint32               num_vars;
  uint32               var_quality;
  uint8                vary_hdr_flags = 0;   /* to create the Vary header */
  ewsQualType          qual_type;   /* speculative or definite */

# endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA */

  /* vary_hdr holds at most '\r\nVary: negotiate, accept, accept-charset,
     accept-language' */
  vary_hdr = (char *)ewaAlloc(60);   /* free'd in ewsInternalCleanup() */

  if (vary_hdr == NULL)
    {
      EMWEB_WARN(("ewsSelectVariant: no resources for Vary header\n"));
      return (context->select_state = ewsDocSelectStateFinish);
    }

  vary_hdr[0] = '\0';

  EMWEB_TRACE(("ewsSelectVariant(%p), (%s)\n", context, document->url));

  /* select variant if there are Accept hdr's and no Negotiate hdr */
  if ( (!context->negotiate_hdr_first) &&
       ( (context->accept_hdr_first) ||
         (context->accept_charset_hdr_first) ||
         (context->accept_language_hdr_first) ))
    {
      sel = TRUE;
    }
  else
    {
      /* loop thru the Negotiate directives */
      for (neg_hdr = context->negotiate_hdr_first;
           neg_hdr != NULL;
           neg_hdr = neg_hdr->next)
        {
          /* either 1.0 or * allows us to select */
          if ( ( EMWEB_STRSTR (neg_hdr->directive, "1.0") )   ||
               ( EMWEB_STRSTR (neg_hdr->directive, "*") ) )
            {
              sel = TRUE;
              break;
            }
        }
    }

  /*
   * fetch pointers to variant and attribute data blocks in data archive
   */

  /* Calculate the start of the header nodes */
  offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
  offset += SIZEOF_EWS_DOCUMENT_NODE * nodes;

  /* search for the variants node */
  for (nodePtr = (EwsDocumentNode *)&document->archive_data[offset], i=0;
        i < doc_header->hdr_node_count;
        nodePtr = NEXT_DOCNODE(nodePtr),i++)
    {
      if (nodePtr->type == EW_DOCUMENT_NODE_TYPE_VARIANT)
        {
          /* get offset into the archive for the variant data block */
          variant_block_offset = EW_BYTES_TO_UINT32(nodePtr->data_offset);
          alt_str_offset = EW_BYTES_TO_UINT32(&document->archive_data[variant_block_offset]);
          alt_hdr = (char *)&document->archive_data[alt_str_offset];
          EMWEB_TRACE(("ews_tcn: alternates header: %s\n",
                          (char *)&document->archive_data[alt_str_offset]));

#         ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA

          if (sel)
            {
              /* 2nd location of variant data block holds number of variants */
              num_vars =  EW_BYTES_TO_UINT32(&document->archive_data[variant_block_offset + sizeof(uint32)]);
              EMWEB_TRACE(("ews_tcn: number of variants: %ld\n", num_vars));
              /* for each variant in variant data block match the variants
               * attributes with the Accept header data to calculate total
               * variant quality. Variants start in the third location in the
               * variant data block (sizeof(uint32) * 2)
               */
              for (i = sizeof(uint32) * 2;
                   num_vars > 0;
                   i += sizeof(uint32), num_vars--)
                {
                  qual_type = ewsQualTypeNone;
                  attr_block_offset = EW_BYTES_TO_UINT32(&document->archive_data[variant_block_offset + i]);
                  EMWEB_TRACE(("ews_tcn: variant: %s\n", (char *)ewsGetAttr(document, attr_block_offset, ewAttrName, NULL)));

                  var_quality = ewsGetAttr(document, attr_block_offset, ewAttrQuality, &vary_hdr_flags);
                  var_quality *= ewsCalcQual( (char *)ewsGetAttr(document, attr_block_offset, ewAttrType, &vary_hdr_flags),
                                              context->accept_hdr_first,
                                              &qual_type,
                                              ewAttrType);

                  var_quality *= ewsCalcQual( (char *)ewsGetAttr(document, attr_block_offset, ewAttrCharset, &vary_hdr_flags),
                                              context->accept_charset_hdr_first,
                                              &qual_type,
                                              ewAttrCharset);

                  var_quality *= ewsCalcQual( (char *)ewsGetAttr(document, attr_block_offset, ewAttrLanguage, &vary_hdr_flags),
                                              context->accept_language_hdr_first,
                                              &qual_type,
                                              ewAttrLanguage);

                  if (var_quality > choice_quality)
                    {
                      choice_quality = var_quality;
                      choice_attr_block_offset = attr_block_offset;
                      choice_qual_type = qual_type;
                    }
                }

              /* build elaborate or simple Vary header */
#             ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_EL_VARY
              /* create the Vary header */
              EMWEB_STRCPY (vary_hdr, "\r\nVary: negotiate");
              if (vary_hdr_flags & ewAttrType)
                EMWEB_STRCAT (vary_hdr, ", accept");
              if (vary_hdr_flags & ewAttrCharset)
                EMWEB_STRCAT (vary_hdr, ", accept-charset");
              if (vary_hdr_flags & ewAttrLanguage)
                EMWEB_STRCAT (vary_hdr, ", accept-language");
#             else /* !EW_CONFIG_OPTION_CONTENT_NEGOTIATION_EL_VARY */
                EMWEB_STRCAT (vary_hdr, "\r\nVary: *");
#             endif /* !EW_CONFIG_OPTION_CONTENT_NEGOTIATION_EL_VARY */

              break;
            } /* end if select */

#         endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA */
        }
    }

  /* send choice-response if (made a choice with quality > 0 AND rcvd no Negotiate hdr) OR
   *                         (made a choice with quality > 0 AND quality type is definite AND rcvd the Negotiate hdr)
   */
  if  ( ((choice_quality > 0) && (context->negotiate_hdr_first == NULL)) ||
        ((choice_quality > 0) && (choice_qual_type == ewsQualTypeDefinite) && (context->negotiate_hdr_first != NULL)) )
    {
      /* send the choice response */
      if (context->url_dynamic)
        {
          ewaFree(context->url);
          context->url_dynamic = FALSE;
        }
      context->url = (char *)ewsGetAttr(document, choice_attr_block_offset, ewAttrName, NULL);
      context->tcn_hdr = ewsTcnHdrChoice;   /* set up the TCN header */
      context->select_state = ewsDocSelectStateFindDocument;
    }
  else
    {
      /* send the original doc requested as the list response */
      context->tcn_hdr = ewsTcnHdrList;   /* set up the TCN header */
      context->select_state = ewsDocSelectStateFinish;
    }

  context->alt_hdr = alt_hdr;   /* set up the Alternates header */

  /* add the Vary header */
  EMWEB_STRLEN(status, vary_hdr);
  if (status > 0)
    {
      context->vary_hdr = vary_hdr;
    }
  else
    {
      EMWEB_STRCPY(vary_hdr, "\r\nVary: *");
      context->vary_hdr = vary_hdr;
    }

  return (context->select_state);
}
#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA

/* search list of accept header data looking for match on
 * 'attr' attribute parameter. If find match, convert accept
 * data quality value from string in the form "q=n.nn" to
 * integer format in the range of 0 to 100.
 */
static uint32
ewsCalcQual( char *attr, EwsAcceptHdrData *acc_hdr, ewsQualType *qual_type, EwsAttr attr_field)
{
  ewsQualType type = ewsQualTypeNone;
  ewsQualType tmp_type;
  uint32      q = 0;  /* quality */
  uint32      tmp_q;
  uintf       qa;     /* quality adjustment */
  boolean     status;

  /* if either the accept header data or the doc attribute is missing,
   * assign a default quality and return
   */
  if (!acc_hdr || !attr)
    {
      q = 100;
    }
  else
    {
      /* loop thru the accept header data looking for an attribute match */
      for (qa = 100;
           acc_hdr != NULL;
           acc_hdr = acc_hdr->next)
        {
          tmp_q = 0;
          tmp_type = ewsQualTypeNone;

          if (acc_hdr->extn == 0)   /* if didn't rcv an extension */
            {
              /* check for direct match */
              EMWEB_STRCMP( status, attr, acc_hdr->data);

              if (status) /* no direct match */
                {
                  /* check for wildcards in accept header */
                  status = !ewsCkWildcard(attr, acc_hdr, attr_field);

                  if (!status)   /* wildcard match */
                    {
                      tmp_type = ewsQualTypeSpeculative;
                    }
                }
              else
                {
                  tmp_type = ewsQualTypeDefinite;
                }

              if (!status) /* match */
                {
                  if (acc_hdr->qual)     /* if rcvd quality value */
                    {
                      /* convert the quality string to an integer */
                      tmp_q = ewsQStrToInt ( acc_hdr->qual);
                    }
                  else
                    {
                      /* for each accept data without a quality value, assign
                       * the first a default quality value of 100 (1.0) and
                       * decrement each subsequent entry by 1 (0.01) to imply a
                       * preference from its position in the accept header. If no
                       * quality value was specified, the first in the list is the
                       * most preferred.
                       */
                      tmp_q = qa;
                    }
                }

              /* save the best so far.
               * - replace a definite with a higher quality definite.
               * - replace a speculative with a higher quality speculative if not replacing a definite.
               * - replace a speculative with a definite.
               */
              if ( ((tmp_q > q) && (tmp_type == ewsQualTypeDefinite) && (type == ewsQualTypeDefinite)) ||
                   ((tmp_q > q) && (tmp_type == ewsQualTypeSpeculative) && (type != ewsQualTypeDefinite)) ||
                   ((tmp_type == ewsQualTypeDefinite)  && (type != ewsQualTypeDefinite)) )
                {
                  q = tmp_q;
                  type = tmp_type;
                }
            }

          /* decrease quality adjustment. Don't go negative on very long accept headers */
          if (qa > 0)
            {
              qa--;
            }
        }

      /* if a previous accept header resulted in a speculative type, don't over-ride it */
      if (!(*qual_type == ewsQualTypeSpeculative))
        {
          *qual_type = type;
        }
    }
  return q;
}

/*
 * check for wildcards in the accept header.
 */
boolean
ewsCkWildcard( char *attr, EwsAcceptHdrData *acc_hdr, EwsAttr attr_field)
{
  char    *attr_ptr = attr;
  char    *data_ptr = acc_hdr->data;
  uintf   i;
  boolean status = FALSE;

  if (EMWEB_STRCHR(acc_hdr->data, '*'))   /* have wildcard in accept header */
    {
      if ( (acc_hdr->data[0] == '*') &&
           (acc_hdr->data[1] == '/') &&
           (acc_hdr->data[2] == '*') )
        {
          status = TRUE;
        }

      else if ( (acc_hdr->data[0] == '*') &&   /* "*" */
                (acc_hdr->data[1] == '\0') )
        {
          status = TRUE;
        }

      /* check for type with matching primary and wildcard secondary */
      else if (attr_field == ewAttrType)
        {
          for(i = 0; ((*attr_ptr != '\0') && (*data_ptr != '\0')); i++, attr_ptr++, data_ptr++)
            {
              if (*attr_ptr == *data_ptr)
                {
                  if ( (i > 0) &&
                       (data_ptr[0] == '/') &&
                       (data_ptr[1] == '*') &&
                       (data_ptr[2] == '\0') )
                    {
                      status = TRUE;
                      break;
                    }
                }
              else
                {
                  break;
                }
            }
        }
    }
  return status;
}

/*
 * ewsQStrToInt
 *
 * takes a quality string in the format "q=x.xx" in the range
 * 0.00 to 9.99 and converts it to an integer in the range
 * 0 - 999. Ignores characters before the first digit
 * and after the second digit after the '.'.
 */
static uint32
ewsQStrToInt (char *q_str)
{
  uint32 q = 0;   /* quality */
  uintf  i;   /* loop control */
  char   c;   /* character */

  /* find the first digit */
  for (i = 0, c = q_str[i];
       c != '\0';
       c = q_str[++i])
    {
      if (c >= '0' && c <= '1')
        {
          q = (c - '0') * 100;

          if ( ( c = q_str[++i] ) == '.' )
            {
              c = q_str[++i];   /* skip the '.' */

              if (c >= '0' && c <= '9')
                {
                  q += (c - '0') * 10;
                }
              else
                {
                  break;
                }

              c = q_str[++i];
              if (c >= '0' && c <= '9')
                q += (c - '0') * 1;

              break;
            }
          else
            {
              EMWEB_WARN(("ewsQStrToInt: invalid quality format\n"));
              q = 0;
            }
          break;
        }
    }
  return q;
}
#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA */

/* fetch the requested attribute from the attribute block in the
 * data archive and set the vary header flag.
 */
uint32
ewsGetAttr(EwsDocument document, uint32 attr_block_offset, EwsAttr attr_type, uint8 *vary_hdr_flags)
{

  uint32 offset;
  uint32 attr = 0;
  uint32 indx;

  /* attr_block_offset will never be 0 */
  if (attr_block_offset)
    {
      /* point to the location in the attribute block */
      indx = attr_type * sizeof(uint32);

      /* get the offset to the attribute */
      offset = EW_BYTES_TO_UINT32(&document->archive_data[attr_block_offset + indx]);

      if (!offset)
        {
          attr = 0;
        }
      else if (attr_type == ewAttrQuality)
        {
          /* for quality, the offset is the value, not the offset to the value */
          attr = offset;
        }
      else
        {
          attr = (uint32)&document->archive_data[offset];
          if (vary_hdr_flags)
            {
              *vary_hdr_flags |= attr_type;   /* set the vary header flag for this type */
            }
        }
    }
  return attr;
}

/* return the attribute data block offset for this document.
 * A zero attr_block_offset is invalid.
 */
uint32
ewsGetAttrBlkOffset( EwsDocument document)
{
  const EwsDocumentHeader   *doc_header;
  EwsDocumentNode           *nodePtr;
  uint32                    offset;
  uint32                    attr_block_offset = 0;
  uintf                     nodes;
  uintf                     i;

  doc_header = document->doc_header;
  nodes = EW_BYTES_TO_UINT16(doc_header->node_count);

  /* Calculate the start of the header nodes */
  offset = EW_BYTES_TO_UINT32(doc_header->node_offset);
  offset += SIZEOF_EWS_DOCUMENT_NODE * nodes;

  /* search for the attributes node */
  for (nodePtr = (EwsDocumentNode *)&document->archive_data[offset], i=0;
        i < doc_header->hdr_node_count;
        nodePtr = NEXT_DOCNODE(nodePtr),i++)
    {
      if (nodePtr->type == EW_DOCUMENT_NODE_TYPE_ATTRIBUTES)
        {
          /* get offset into the archive for the attribute data block */
          attr_block_offset = EW_BYTES_TO_UINT32(nodePtr->data_offset);
          break;
        }
    }
  return attr_block_offset;
}

#ifdef EW_CONFIG_OPTION_METHOD_TRACE
static void
ewsServeTrace(EwsContext context)
{
  EwaNetBuffer first = ewsStringBuffer(&context->req_method_str);
  uintf bytes = context->req_method_str.offset;

    ewaLogHook(context, EWS_LOG_STATUS_OK);
    context->flags &= ~EWS_CONTEXT_FLAGS_CLEANUP;
#   ifdef EW_CONFIG_OPTION_PERSISTENT
    context->keep_alive = FALSE;
#   endif /* EW_CONFIG_OPTION_PERSISTENT */
    /* put back space -- was clobbered to '\0' if url contiguous */
    if (ewsStringIsContiguous(&context->req_url))
      {
        ewsStringData(&context->req_url)[ewsStringLength(&context->req_url)]
          = ' ';
      }
    ewsSendHeader(context
                 ,NULL
                 ,"200 OK"
                 ,"Content-Type: message/http\r\n\r\n");
    ewaNetBufferDataSet(first, ewaNetBufferDataGet(first) + bytes);
    ewaNetBufferLengthSet(first, ewaNetBufferLengthGet(first) - bytes);
    if (ewsStringBuffer(&context->req_str_end) == first)
      {
        context->req_str_end.offset -= bytes;
      }
    ewaNetBufferNextSet(ewsStringBuffer(&context->res_str_end), first);
    ewaNetBufferLengthSet(ewsStringBuffer(&context->res_str_end)
                         ,context->res_str_end.offset);
    context->req_buf_first = EWA_NET_BUFFER_NULL;
    context->res_str_end = context->req_str_end;
    ewsInternalFinish(context);
}
#endif /* EW_CONFIG_OPTION_METHOD_TRACE */

#ifdef EW_CONFIG_OPTION_ITERATE
/**********************************************************************
 *
 *  ewsServeEmWebIterateStart
 *
 *  Purpose: Processes the node assocated with an <EMWEB_ITERATE> tag.
 *
 *  Inputs:  context - of HTTP transaction
 *           doc_context - of current document
 *
 *  Outputs: none
 *
 *  Returns: EWA_STATUS_OK, or EWA_STATUS_ERROR on abort.
 *
 *  Notes:
 *      when an NODE_TYPE_START_ITERATE node is first encountered,
 *      an iterate context is allocated and the current data state
 *      is stored. The "restart" node is also stored: this is where
 *      the iterate will restart from.  This node could be any nodes
 *      that set value prior to the START_ITERATE node.
 *      The associated C="" code is invoked.
 *      If the return value is NULL, then all nodes and data between
 *      the START_ITERATE node and its END_ITERATE are skipped.
 *      Otherwise, the data is advanced.
 *
 */
static EwaStatus
ewsServeEmWebIterateStart ( EwsContext context, EwsDocumentContextP doc_context )
{
  EwDocIterateContext   *iterate;

  iterate = NULL;

  /*
   * if no context yet, or the context at the top of the stack does not
   * correspond to this node, then this is the first time we have entered
   * this ITERATE
   */
  if (   doc_context->iterate == NULL
      || (doc_context->iterate->iterateNodeId != doc_context->node_count)
     )
    {
      iterate = (EwDocIterateContext *)ewaAlloc( sizeof( *iterate ) );
      if (iterate != NULL)
        {
          const EwsDocumentNode  *startNode;
          const EwsDocumentNode  *prevNode;
          const EwsDocumentNode  *firstNode;
          uint32 nodeOffset;
          uintf nodeCount;

          /* link it to the top of the stack, and store the node_count
           * as an identifier, initialize iterator handle to null
           */
          iterate->stack = doc_context->iterate;
          doc_context->iterate = iterate;
          iterate->iterateNodeId = doc_context->node_count;
          iterate->iterator = NULL;

          /*
           * now we must determine exactly where we will restart from.
           * If a value="" attribute is included, then this restart point will
           * be -prior- to the current node (so the value can be reset again
           * for the next loop).  Search back until no more "set value" nodes
           * are found for this offset.
           */
          startNode = doc_context->nodep;
          nodeOffset = EW_BYTES_TO_UINT32(doc_context->nodep->data_offset);
          /* firstNode == first node in document's node list */
          firstNode = (const EwsDocumentNode *)&doc_context->document->archive_data
                        [EW_BYTES_TO_UINT32(doc_context->doc_headerp->node_offset)];


          for (
               (prevNode = PREV_DOCNODE( startNode ),
                nodeCount = doc_context->node_count
               );
               (   prevNode >= firstNode    /* don't backup too far */
                && (prevNode->attributes & EW_DOCUMENT_NODE_SET_VALUE)
                && EW_BYTES_TO_UINT32(prevNode->data_offset) == nodeOffset
               );
               (prevNode = PREV_DOCNODE( prevNode ),
                nodeCount++
               )
              )
            {
              startNode = prevNode;
            }

          /*
           * save point in document were repeat block starts from, and compression
           * if necessary
           */

          iterate->nodep = startNode;
          iterate->nodeCount = nodeCount;
          iterate->offset = doc_context->offset;
          iterate->remaining = doc_context->remaining;
          iterate->datap = doc_context->datap;
#         ifdef EW_CONFIG_OPTION_COMPRESS
          if (doc_context->doc_headerp->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
            {
              ewDecompressSaveContext( (EwDecompressContext) doc_context->compress_context
                                       ,&iterate->save_cmp);
            }
#          endif /* EW_CONFIG_OPTION_COMPRESS */

        } /* endif new iterate context allocated */
    }
  else      /* repeating again */
    {
      iterate = doc_context->iterate;       /* use top most context */
    }

  /*
   * invoke iterator Code, deal with suspension and abort.
   */
  if (iterate != NULL)
    {
      iterate->iterator = doc_context->document->archive->emweb_string
                             ( context
                              ,EW_BYTES_TO_UINT32(doc_context->nodep->index) );

      if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
        {
          return EWA_STATUS_ERROR;
        }
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (context->schedulingState == ewsContextSuspended)
      {
        return ewsFlushAll(context);
      }
    context->schedulingState = ewsContextScheduled;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
    }

  /*
   * if unable to iterate (due to no free memory for context), or done
   * iterating, skip to the matching END node
   */
  if (iterate == NULL || iterate->iterator == NULL)
    {
      ewsSkipBlockContent( context
                          ,doc_context
                          ,EW_DOCUMENT_NODE_TYPE_END_ITERATE );
      /*
       * free context and adjust stack (if not out of memory)
       */
      if (iterate != NULL)
        {
          doc_context->iterate = iterate->stack;
          ewaFree( iterate );
        }
    }

  /*
   * NOTE: we're either skipping the END_ITERATE node here (no more iterations),
   * or the START_ITERATE node.
   */
  doc_context->node_count--;
  doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
  return EWA_STATUS_OK;
}


/**********************************************************************
 *
 *  ewsServeEmWebIterateEnd
 *
 *  Purpose: Loops back up to the beginning of the iterated output.
 *
 *  Inputs: context - current http transaction context.
 *          doc_context - current document context.
 *
 *  Outputs: none
 *
 *  Returns: EWA_STATUS_OK
 *
 *  Node: the "end iteration" case is not dealt with here.  It is done
 *      in the  IterateStart routine (skipping this node in the process).
 *
 */
static EwaStatus
ewsServeEmWebIterateEnd ( EwsContext context, EwsDocumentContextP doc_context )
{
  EW_UNUSED(context);
# ifdef EMWEB_SANITY
  if (doc_context->iterate == NULL)
    {
      EMWEB_ERROR( ("ewsServeEmWebIterateEnd: internal error - no context\n") );
    }
# endif

  doc_context->nodep  = doc_context->iterate->nodep;
  doc_context->node_count = doc_context->iterate->nodeCount;
  doc_context->offset  = doc_context->iterate->offset;
  doc_context->remaining  = doc_context->iterate->remaining;
  doc_context->datap  = doc_context->iterate->datap;
# ifdef EW_CONFIG_OPTION_COMPRESS
  if (doc_context->doc_headerp->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
    {
      ewDecompressRestoreContext( (EwDecompressContext) doc_context->compress_context
                                  ,&doc_context->iterate->save_cmp);
    }
# endif /* EW_CONFIG_OPTION_COMPRESS */

  return(EWA_STATUS_OK);
}
#endif /* EW_CONFIG_OPTION_ITERATE */

#ifdef EW_CONFIG_OPTION_IF
/**********************************************************************
 *
 *  ewsServeEmWebIf
 *
 *  Purpose: Processes the nodes assocated with an <EMWEB_IF>,
 *  <EMWEB_IF_ELSE>, <EMWEB_ELSE> tags.
 *
 *  Inputs:  context - of HTTP transaction
 *           doc_context - of current document
 *
 *  Outputs: none
 *
 *  Returns: EWA_STATUS_OK, or EWA_STATUS_ERROR on abort.
 *
 *  Notes:
 *
 */
static EwaStatus
ewsServeEmWebIf ( EwsContext context, EwsDocumentContextP doc_context )
{

  const  void            *condition;    /* returned by application code */
  const EwsDocumentNode  *startNode;
  const EwsDocumentNode  *prevNode;
  const EwsDocumentNode  *firstNode;
  uint32                 nodeOffset;
  uintf                  nodeCount;
  uint32                 nodeIndex;


  /* if processing an <EMWEB_IF> */
  if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_IF_START)
    {
      doc_context->if_depth++;   /* the nesting level of if stmts */
      doc_context->if_true = FALSE; /* have we hit a true condition yet */
    } /* endif IF */

  if (doc_context->if_true == TRUE)
    { /* just processed true condition, so skip to endif */
      ewsSkipBlockContent( context
                          ,doc_context
                          ,EW_DOCUMENT_NODE_TYPE_ENDIF );
        return EWA_STATUS_OK;
    }

  /* haven't found a true condition yet, keep looking */

  /*
   * invoke conditional code, deal with suspension and abort.
   */

  nodeIndex = EW_BYTES_TO_UINT32(doc_context->nodep->index);

  /* check for <EMWEB_ELSE> */
  if(0 == nodeIndex)
    {
      condition = (const void *)"T";  /* always true */
    }
  else
    {
      condition = doc_context->document->archive->emweb_string
        ( context
         ,nodeIndex );

      if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
        {
          return EWA_STATUS_ERROR;
        }
#     ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
      if (context->schedulingState == ewsContextSuspended)
        {
          return EWA_STATUS_OK;
        }
      context->schedulingState = ewsContextScheduled;
#     endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
    }

  if (condition == NULL)
    {
      /* still looking for true, so skip to else, elseif, or end */
      ewsSkipBlockContent( context
                           ,doc_context
                           ,EW_DOCUMENT_NODE_TYPE_ENDIF );

      /* check what we found */
      /* if we found an endif, done */
      if (doc_context->nodep->type == EW_DOCUMENT_NODE_TYPE_ENDIF)
        return EWA_STATUS_OK;

      nodeIndex = EW_BYTES_TO_UINT32(doc_context->nodep->index);

      /* if we found an else skip it and continue processing */
      if(nodeIndex == 0)
        {
          doc_context->if_true = TRUE;

          /* advance to next node */
          doc_context->node_count--;
          doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
          return EWA_STATUS_OK;
        }
      else
        {
          /*
           * now we must backup and reset the values because
           * we might be going to process an else/if.
           * If a value="" attribute is included, then this value node
           * will be -prior- to the current node.  Search back
           * until no more "set value" nodes are found for this offset.
           */
          startNode = doc_context->nodep;
          nodeOffset = EW_BYTES_TO_UINT32(doc_context->nodep->data_offset);

          /* firstNode == first node in document's node list */
          firstNode = (const EwsDocumentNode *)&doc_context->document->archive_data
            [EW_BYTES_TO_UINT32(doc_context->doc_headerp->node_offset)];

          for ( (prevNode = PREV_DOCNODE( startNode ),
                 nodeCount = doc_context->node_count );
                ( prevNode >= firstNode    /* don't backup too far */
                 && (prevNode->attributes & EW_DOCUMENT_NODE_SET_VALUE)
                 && EW_BYTES_TO_UINT32(prevNode->data_offset)
                 == nodeOffset );
                (prevNode = PREV_DOCNODE( prevNode ), nodeCount++))
            {
              startNode = prevNode;
              doc_context->nodep = prevNode;
            }
          doc_context->node_count = nodeCount;
        } /* end else */
    } /* if condition is NULL */

  if (condition != NULL)
    { /* condition is TRUE */
      doc_context->if_true = TRUE;

      /* advance to next node */
      doc_context->node_count--;
      doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );
      return EWA_STATUS_OK;
    }

  return EWA_STATUS_OK;
}

/**********************************************************************
 *
 *  ewsServeEmWebEndIf
 *
 *  Purpose: Processes the EndIf and advances to next node.
 *
 *  Inputs: context - current http transaction context.
 *          doc_context - current document context.
 *
 *  Outputs: none
 *
 *  Returns: EWA_STATUS_OK
 *
 *
 */
static EwaStatus
ewsServeEmWebEndIf ( EwsContext context, EwsDocumentContextP doc_context )
{
  EW_UNUSED(context);

  /* decrement nesting level */
  doc_context->if_depth--;

  /* we are done with this level, so we have processed the true condition */
  doc_context->if_true = TRUE;

  doc_context->node_count--;
  doc_context->nodep = NEXT_DOCNODE( doc_context->nodep );

  return(EWA_STATUS_OK);
}
#endif /* EW_CONFIG_OPTION_IF */

#ifdef EW_CONFIG_OPTION_STRING_VALUE
/*
 *  If the EmWeb_String or namespace result is to be used to set the
 * parameter for a subsequent macro (instead of being written to the
 * user agent).
 *
 *  The order of allocating memory, copying memory, and
 * deallocating memory is important because vp could be
 * pointing to doc_context->string_buffer.
 *
 */

static EwaStatus copyIntoStringValue(EwsDocumentContextP  doc_context,
                                     const char           *vp,
                                     int                  type)
{
  EwaStatus    status       = EWA_STATUS_OK;
  void  *ptr;
  EW_UNUSED(type); /* may be unused if string types not configured */

  ptr = doc_context->string_buffer;
  doc_context->string_value = doc_context->string_buffer = NULL;

# ifdef EMWEB_SANITY    /* Sanity Check first */
  if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_REPEAT)

    {
      EMWEB_WARN(("copyIntoStringValue: embedded macro with repeat set.\n"));
      if (ptr) ewaFree( ptr );
      return EWA_STATUS_ERROR;
    }
# endif
  /* If NULL, value already cleared */

  if (!vp)
    {
      /* nuttin' */
    }

  /*
   *  Otherwise, copy returned value into dynamic memory and save
   * in document context.
   */

# ifdef EW_CONFIG_OPTION_STRING_TYPED
  /*
   * This mode supports the DECIMAL_UINT type only.
   * Convert parameter to text and save in document context.
   */
  else if (type == ewFieldTypeDecimalUint)
    {
      doc_context->string_buffer = (char *) ewaAlloc(12);

      if (!doc_context->string_buffer)
        {
          EMWEB_WARN(("copyIntoStringValue: no memory to copy parameter\n"));
          status = EWA_STATUS_ERROR;
        }
      else
        {
          EMWEB_LTOA(doc_context->string_value
                     ,*((uint32 *)vp)
                     ,doc_context->string_buffer
                     ,12);
        }
    }
  else if (type != 0)
    {
      EMWEB_WARN(("copyIntoStringValue: unsupported parameter type\n"));
      status = EWA_STATUS_ERROR;
    }
# endif /* EW_CONFIG_OPTION_STRING_TYPED */
  else        /* Copy text parameter into document context */
    {
      uintf len;

      EMWEB_STRLEN(len, vp);

      doc_context->string_buffer = (char *) ewaAlloc(len + 1);

      if (!doc_context->string_buffer)
        {
          EMWEB_WARN(("copyIntoStringValue: no memory to copy parameter\n"));
          status = EWA_STATUS_ERROR;
        }
      else
        {
          EMWEB_STRCPY(doc_context->string_buffer, vp);
          doc_context->string_value = doc_context->string_buffer;
        }
    }

  if (ptr) ewaFree( ptr );      /* free old memory - note: this may be vp */

  return(status);
}
#endif /* EW_CONFIG_OPTION_STRING_VALUE */

#ifdef EW_CONFIG_OPTION_GROUP_DYNAMIC
/*
 * ewsGroupDynamic
 * The following API may be available in the future to group dynamic
 * content into a "bulk get" operation.  This function is called from
 * <EMWEB_STRING> or ewaNamespaceGet_* in order to request that a dynamic
 * value be deferred until later so as to group many gets into a single
 * operation.
 *
 * This function returns a pointer to a pointer to be saved by the application.
 * The application will eventually save a pointer to the value here.  The
 * EmWeb/Server invokes ewaGroupDynamicFlush() to signal that the application
 * must get all oustanding values (if it has not done so already).
 *
 * Note: This functionality has not actually been implemented.  At this time,
 * we provide a working API.  We always flush after ewsGroupDynamic thus
 * creating groups of one.  The actual grouping mechanism will likely be
 * added in a future release.
 */
void * *
ewsGroupDynamic ( EwsContext context /* request context */
                 ,boolean    dynamic /* TRUE if value will be ewaAlloc'ed */
                )
{

#   ifdef EMWEB_SANITY
    if (context->state != ewsContextStateString)
      {
        EMWEB_WARN(("ewsGroupDynamic: bad state\n"));
        return NULL;
      }
    if (context->group_dynamic_count != 0)
      {
        EMWEB_WARN(("ewsGroupDynamic: internal error\n"));
        return NULL;
      }
#   endif /* EMWEB_SANITY */

    context->group_dynamic_flag[0] = dynamic;
    context->group_dynamic_ptr[0] = NULL;
    context->group_dynamic_count = 1;
    return (void **) &context->group_dynamic_ptr[0];
}
#endif /* EW_CONFIG_OPTION_GROUP_DYNAMIC */

#if defined(EW_CONFIG_OPTION_CONVERT_XML) || defined(EW_CONFIG_OPTION_ITERATE) || defined(EW_CONFIG_OPTION_IF)
/**********************************************************************
 *
 *  ewsSkipBlockContent
 *
 *  Purpose: Skips over the document content contained between
 *          two tags.  For example, this routine is used
 *          to skip the contents of an <EMWEB_ITERATE>...</EMWEB_ITERATE>
 *          block that is not being served.
 *          It is also used by <EMWEB_IF>...</EMWEB_IF>.
 *          The <EMWEB_IF>,<EMWEB_ELSE>,<EMWEB_ELSE_IF> tags are all the
 *          same node type.
 *          If a true condition has been found while processing an <EMWEB_IF>
 *          statement, then we want to skip to the </EMWEB_IF>.
 *          If we are still looking for a true condition, then we want to
 *          skip to the next else, else if, or end.
 *
 *  Inputs:  context - current context
 *           doc_context - current document context
 *           endNode - node type to terminate skip at.
 *
 *  Outputs: Advances doc_context current node ptr and data offset.
 *           Current node is set to be the node with the endNode
 *           type.
 *
 *  Returns: void
 *
 */
void
ewsSkipBlockContent( EwsContext context
                    ,EwsDocumentContextP doc_context
                    ,uintf endTag
                   )
{
  uint32 skipCount;     /* # bytes static data */
  uintf nestCount;      /* skip containing blocks */
  uintf startTag;

  EW_UNUSED(context);

  /* remember starting data offset */

  skipCount = EW_BYTES_TO_UINT32(doc_context->nodep->data_offset);

  /* remember starting node type (for nesting) */

  startTag = doc_context->nodep->type;

  /* skip nodes */

  for ( (doc_context->node_count--,
         nestCount = 0,
         doc_context->nodep = NEXT_DOCNODE( doc_context->nodep )
         );
        (nestCount !=0 ||
         ewsSkipBlockCheck(doc_context, startTag, endTag)
         );
        (doc_context->node_count--,
         doc_context->nodep = NEXT_DOCNODE( doc_context->nodep )
         )
       )
    {
      uint8 currentTag = doc_context->nodep->type;
      if (currentTag == startTag)
        {
          /* for an else, and elseif don't bump up nestCount */
          if (currentTag == EW_DOCUMENT_NODE_TYPE_IF &&
              (doc_context->nodep->index == 0 ||
               doc_context->nodep->attributes
               != EW_DOCUMENT_NODE_IF_START))
            {
            }
          else
            {
              nestCount++;
            }
        }
      else if (currentTag == endTag)
        {
          nestCount--;      /* exit a nest level */
        }
    }

  /* now determine delta of data */

  skipCount = (EW_BYTES_TO_UINT32(doc_context->nodep->data_offset)
               - skipCount
               );

  /*
   * advance past data
   */

# ifdef EW_CONFIG_OPTION_COMPRESS
  if (doc_context->doc_headerp->document_flags & EW_ARCHIVE_DOC_FLAG_EW_COMPRESS)
    {
      ewDecompressRead( (EwDecompressContext) doc_context->compress_context
                        ,NULL       /* discard data */
                        ,skipCount
                        );
    }
  else
# endif /* EW_CONFIG_OPTION_COMPRESS */
    {
      doc_context->datap += skipCount;
    }
  doc_context->offset += skipCount;
  doc_context->remaining -= skipCount;

  /* on exit, doc_context->nodep points to the endTag node */
}
#endif /* EW_CONFIG_OPTION_CONVERT_XML || EW_CONFIG_OPTION_ITERATE || EW_CONFIG_OPTION_IF */

#if defined(EW_CONFIG_OPTION_CONVERT_XML) || defined(EW_CONFIG_OPTION_ITERATE) || defined(EW_CONFIG_OPTION_IF)
/**********************************************************************
 *
 *  ewsSkipBlockCheck
 *
 *  Purpose: To determine if we are done skipping or not
 *
 *  Inputs:  context - current context
 *           startTag - start tag for which we are looking for the
 *                      corresponding end tag, ex. START_ITERATE, IF
 *           endTag - end tag for iterate
 *
 *  Outputs: None
 *
 *  Returns: TRUE to keep processing (keep skippping)
 *           FALSE to end processing (stop skipping)
 *
 */
boolean ewsSkipBlockCheck(EwsDocumentContextP doc_context,
                           uintf startTag,
                           uintf endTag)
{
# ifdef EW_CONFIG_OPTION_IF
  uint8 currentTag = doc_context->nodep->type;
# else /* EW_CONFIG_OPTION_IF */
  EW_UNUSED(startTag);
  EW_UNUSED(endTag);
# endif /* EW_CONFIG_OPTION_IF */

#ifdef EW_CONFIG_OPTION_IF

  /* if we started with an if statement, and haven't found a true condition
   * yet
   */
  if (startTag == EW_DOCUMENT_NODE_TYPE_IF)
  {
     uint32 nodeIndex = EW_BYTES_TO_UINT32(doc_context->nodep->index);


    if (doc_context->if_true == FALSE)
    { /* we want to stop on else, elseif, and endif on the same level */
      if(currentTag == EW_DOCUMENT_NODE_TYPE_ENDIF || /* endif */
         (currentTag == EW_DOCUMENT_NODE_TYPE_IF &&
          nodeIndex == 0) ||       /* else */
         (currentTag == EW_DOCUMENT_NODE_TYPE_IF &&
          doc_context->nodep->attributes
         != EW_DOCUMENT_NODE_IF_START))           /* else if */
        {
          return FALSE;  /* stop skipping */
        }
    }
    /* if we are now pointing to an </emweb_if> stop */
    if (doc_context->nodep->type == endTag)
      return FALSE;

    return TRUE; /* keep skipping */
  }
  else /* startTag != NODE_TYPE_IF */
#endif
  {
    if (doc_context->nodep->type != endTag)
      {
        return TRUE;
      }
    else
      {
        return FALSE;
      }
  }

} /* end if (startTag == EW_DOCUMENT_NODE_TYPE_IF) */

#endif  /* EW_CONFIG_OPTION_CONVERT_XML || EW_CONFIG_OPTION_ITERATE || EW_CONFIG_OPTION_IF */


#ifdef EW_OPTION_URL_HOOK
/*
 * ewsRelocateURL
 * Given a returned URL pointer (new_url) from a URL hook function,
 * and the URL pointer passed to the function (old_url), check to see if
 * we need to allocate memory and copy the URL into it (and then do so).
 * Stores the new URL pointer into the context URL pointer.
 * Aborts the context and returns NULL on memory allocation failure.
 */
extern char * ewsRelocateURL( EwsContext context
                             ,char *old_url
                             ,char *new_url)
{
  uintf old_url_length, new_url_length;

  EMWEB_STRLEN(old_url_length, old_url);

  /* An annoying task.  There are four cases:
   * * new_url == old_url: Nothing to do.
   * * new_url points inside of old_url buffer: Copy new URL down to bottom
   *    of the buffer.
   * * new_url points outside of old_url buffer, but is shorter.
   *    Copy the new URL into the old_url buffer.
   * * new_url points to a longer URL:  Reallocate the buffer, copy the URL.
   *    Change the context->url pointer.
   */
  if (new_url == old_url)
    {
      /* Do nothing. */
    }
  else if (new_url >= old_url && new_url <= old_url + old_url_length)
    {
      /* Must use a loop to do the copying, since EMWEB_STRCPY may not be
       * able to handle overlapping strings as arguments. */
      char *p = old_url;
      do {
        *p++ = *new_url++;
      } while (p[-1] != '\0');
    }
  else
    {
      /* new_url is not inside the old URL buffer. */
      EMWEB_STRLEN(new_url_length, new_url);
      if (new_url_length > old_url_length)
        {
          /* The complicated case -- Reallocate the URL buffer.
           * Free the old URL buffer if it was dynamically allocated.
           */
          if (context->url_dynamic)
            {
              ewaFree(old_url);
            }
          else
            {
              /* Remember that the new URL buffer is dynamically allocated. */
              context->url_dynamic = TRUE;
            }
          /* Allocate the new URL buffer, checking for problems. */
          context->url = old_url = (char *) ewaAlloc(new_url_length + 1);
          if (old_url == NULL)
            {
              EMWEB_ERROR(("ewsServeFindDocument: no resources for url"));
              ewsNetHTTPAbort(context);
              return(NULL);
            }
        }
      /* Now copy the new URL into the buffer at old_url (which may have
       * been reallocated, or may be the old URL buffer). */
      EMWEB_STRCPY(old_url, new_url);
    }
  /* At this point, old_url points to the current URL buffer. */
  return old_url;
}
#endif /* EW_OPTION_URL_HOOK */

void
ewsServeForceCloseLength(EwsContext context)
{
  /*
   * The *ugly* point of this routine is to force the response
   * to _not_ use chunked encoding and _not_ generate a
   * content-length header.
   * This is to work around ugly bugs in Microsoft UPnP
   */
  context->req_protocol_min = 0;
# ifdef EW_CONFIG_OPTION_PERSISTENT /* not recommended */
  context->keep_alive = FALSE;
# endif /* EW_CONFIG_OPTION_PERSISTENT */
}

boolean
ewsDocumentIsDynamic( EwsDocument document )
{
  uintf nodes;
  boolean dynamic;

  nodes = EW_BYTES_TO_UINT16(document->doc_header->node_count);
  dynamic = (   (   ( nodes != 0 )
                 && ! (  document->doc_header->document_type
                       & EW_ARCHIVE_DOC_FLAG_STATIC
                       )
                 )
#            ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
             || (  document->doc_header->hdr_flags
                 & EW_ARCHIVE_DOC_HDR_DYN_ETAG_FLG
                 )
#            endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */
             );

  return dynamic;
}

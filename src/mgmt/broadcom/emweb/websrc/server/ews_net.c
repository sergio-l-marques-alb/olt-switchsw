/*
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
 * EmWeb/Server application interface to network transport layer
 */

#include "ews.h"
#include "ews_sys.h"
#include "ews_form.h"
#include "ews_send.h"
#include "ews_serv.h"
#include "ews_gway.h"
#include "ews_ctxt.h"
#include "l7_common.h"                  /* LVL7_P0006 */
#include "cli_web_mgr_api.h"            /* LVL7_P0006 */
#include "log.h"

extern uintf ConnectionsActiveTelnet;   /* LVL7_P0006 */
extern uintf ConnectionsActiveSSH;   /* LVL7_P0006 */
/* LVL7_P0006 */
/******************************************************************************
 *
 * In a typical TCP/IP implementation, the application is responsible for
 * listening to the HTTP TCP port (80) for connection requests.  When a
 * connection request is received, the application accepts the connection
 * on behalf of EmWeb/Server and invoke ewsNetHTTPStart() to inform the
 * EmWeb/Server of the new request.
 *
 * EmWeb/Server assumes that the application maintains data buffers for
 * the reception and transmission of TCP data.  The only requirements that
 * EmWeb/Server imposes on the buffer implementation is as follows:
 *
 *   1. Buffers can be uniquely identified by a buffer descriptor.  No
 *      assumptions are made about the actual structure of the buffer
 *      descriptors or their relationship to data.  For example, a buffer
 *      descriptor could be an index into a table, a pointer to a
 *      structure (either contiguous or seperate from the data represented),
 *      etc.  The application is responsible for defining the appropriate
 *      type for EwaNetBuffer and value for EWA_NET_BUFFER_NULL.
 *
 *   2. Buffers can be chained together.  Given a buffer descriptor,
 *      EmWeb/Server must be able to get or set the "next buffer in the chain"
 *      field.  This is done by ewaNetBufferNextSet() and
 *      ewaNetBufferNextGet().  Note that the buffer chain is terminated when
 *      the next buffer value is EWA_NET_BUFFER_NULL.
 *
 *   3. Given a buffer descriptor, EmWeb/Server can determine the start of
 *      data in the buffer.  This is done by ewaNetBufferDataGet().
 *
 *   4. Given a buffer descriptor, EmWeb/Server can determine the size of
 *      contiguous data in the buffer.  Additionally, EmWeb/Server may
 *      change the size of the buffer (EmWeb/Server only changes the
 *      size of the buffer downward).  This is done by ewaNetBufferLengthGet()
 *      and ewaNetBufferLengthSet().
 *
 *   5. EmWeb/Server may allocate a buffer by invoking ewaNetBufferAlloc().
 *      If no buffers are available, this function returns EWA_NET_BUFFER_NULL.
 *      Additionally, EmWeb/Server may release a buffer by invoking
 *      ewaNetBufferFree().
 *
 * As the application receives TCP data on an HTTP connection, it passes
 * this data to the EmWeb/Server by invoking ewsNetHTTPReceive().
 *
 * The EmWeb/Server transmits TCP data on an HTTP connection by invoking
 * ewaNetHTTPTransmit().  The application may throttle EmWeb/Server by
 * returning EWA_STATUS_OK_YIELD.  This causes the EmWeb/Server to
 * save state and return control to the application.  The application must
 * invoke ewsRun() to give the EmWeb/Server an opportunity to continue
 * processing the request.
 *
 * When the EmWeb/Server completes a request, it invokes ewaNetHTTPEnd().
 *
 * The application may abort a request at any time by invoking
 * ewsNetHTTPAbort().
 *
 *****************************************************************************/

/*
 * ewsNetHTTPStart
 * Start a new HTTP request
 *
 * net_handle   - application-specific handle representing request
 *
 * Returns context for the request, or EWS_CONTEXT_NULL on failure.
 */
  EwsContext
ewsNetHTTPStart ( EwaNetHandle net_handle )
{
  EwsContext    context;

  EMWEB_TRACE(("ewsNetHTTPStart(%p)\n", net_handle));

  /*
   * Allocate, initialize, and return a new context block
   */
  context = (EwsContext) ewaAlloc(sizeof(EwsContext_t) +
      (ewsRequestHeaderCount + 1) * sizeof(EwsString));
  if (context == EWS_CONTEXT_NULL)
  {
    EMWEB_WARN(("ewsNetHTTPStart: no resources for context\n"));
    return context;
  }
  context->req_headerp = (EwsString *) &context[1];
  context->net_handle = net_handle;

  /*
   * SETUP INITIAL CONTEXT
   */

  context->allow_http = TRUE;  /* Allow HTTP requests by default */
  context->flags = 0;
  context->abort_flags = 0;

  /*
    Show running config output control variables
       intial values. These are specific to context */

  context->runningConfig.hideMode = L7_TRUE;
  context->runningConfig.count = 0;
  context->runningConfig.current = 0;
  context->runningConfig.init = L7_TRUE;

  /* End of show running config intial values */



#   ifdef EW_CONFIG_OPTION_SSL_RSA
  context->ssl_connection = FALSE;
#   endif /* EW_CONFIG_OPTION_SSL_RSA */
#   ifdef EW_CONFIG_OPTION_UPNP
  context->ssdp_connection = FALSE;
  context->upnp_have_headers = UPNP_NO_HEADERS;
  context->ssdp_mx = 0;
#   endif /* EW_CONFIG_OPTION_UPNP */
  ewsInternalSetup ( context );

  /*
   * STATE
   *
   * Initial state: created & waiting for data
   */
  context->state = ewsContextStateParseRequest;
  context->substate.request = ewsContextSubstateParseMethod;

  /*
   * REQUEST PROCESSING STATE
   */
  context->req_buf_first = EWA_NET_BUFFER_NULL;
  context->req_buf_last = EWA_NET_BUFFER_NULL;
  ewsStringInit(&context->req_str_begin, EWA_NET_BUFFER_NULL);
  ewsStringInit(&context->req_str_end, EWA_NET_BUFFER_NULL);
  context->content_remaining = 0;

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
  context->chunked_encoding = FALSE;

#   ifdef EW_CONFIG_OPTION_EXPECT
  context->expect_100_continue = FALSE;
  context->expect_failed = FALSE;
#   endif /* EW_CONFIG_OPTION_EXPECT */

#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

  /*
   * SCHEDULING
   */
  EWA_TASK_LOCK();

#   ifdef EW_CONFIG_OPTION_SCHED
  EWS_LINK_INSERT(&ews_state->context_wait_list, &context->link);
#   else  /* no EW_CONFIG_OPTION_SCHED */
  context->current = FALSE;
#   endif /* EW_CONFIG_OPTION_SCHED */

  context->schedulingState = ewsContextDataWait;
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
  context->resumePending = FALSE;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

#   ifdef EW_CONFIG_OPTION_SCHED_FC
  context->flow_control = ewsContextFlowControlOff;
#   endif /* EW_CONFIG_OPTION_SCHED_FC */

  EWA_TASK_UNLOCK();

  return context;
}

extern void ewaFileAbortConnection(EwsContext context);
/*
 * ewsNetHTTPAbort
 * Abort a previously started HTTP request.
 *
 * context      - context of request to be aborted
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD)
 */
  EwsStatus
ewsNetHTTPAbort ( EwsContext context )
{
  EMWEB_TRACE(("ewsNetHTTPAbort(%p)\n", context));
  ewaFileAbortConnection(context);
  /*
   * Take lock.  If state already aborted, then avoid recursion.  This
   * could happen in situations in which different signals (internal
   * and external) try to abort an unscheduled context (e.g. from
   * an idle EmWeb/Client connection).
   */
  EWA_TASK_LOCK();
  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
  {
    EWA_TASK_UNLOCK();
    return EWS_STATUS_OK;
  }

  /*
   * Always mark aborting context state, even if we are about to
   * destroy the context, for reason above.
   */
  context->abort_flags |= EWS_CONTEXT_FLAGS_ABORT;

  /*
   * If this context is currently being run by the server, then
   * we cannot abort immediately - server code will be touching the
   * context.  Otherwise, its safe to destroy the context
   * immediately.
   */
  if (IS_CONTEXT_CURRENT( context )
#       ifdef EW_CONFIG_OPTION_PRIMARY
      || (   context->secondary != EWS_SECONDARY_HANDLE_NULL
    && context->secondary->context_current == context
    && IS_CONTEXT_CURRENT( context->secondary->response_context )
    )
#       endif /* EW_CONFIG_OPTION_PRIMARY */
     )
  {
    EWA_TASK_UNLOCK();
    return EWS_STATUS_OK;
  }
  EWA_TASK_UNLOCK();
  ewsInternalAbort(context);
  return EWS_STATUS_OK;
}

/*
 * ewsNetHTTPReceive
 * Receive request data from the network.
 *
 * context      - context of request to which received data applies
 * buffer       - buffer containing received data
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD)
 */
  EwsStatus
ewsNetHTTPReceive ( EwsContext context, EwaNetBuffer buffer )
{
  EwaNetBuffer buf_last;
  EwaNetBuffer buf_last_prev;
#ifdef EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS
  int recvBufCount = 1; /* we have at least 1 buffer */
  EwaNetBuffer buf_ptr;
#endif  /* EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS */

  EMWEB_TRACE(("ewsNetHTTPReceive(%p, %p)\n", context, buffer));

  /*
   * Link buffer chain into context.
   * Follow buffer chain to find last buffer
   */

  buf_last_prev = context->req_buf_first;
  for (buf_last = buffer;
      ewaNetBufferNextGet(buf_last) != EWA_NET_BUFFER_NULL;
      buf_last = ewaNetBufferNextGet(buf_last))
  {
    buf_last_prev = buf_last;
#ifdef EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS
    recvBufCount++;
#endif /* EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS */
  }
#ifdef EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS

  /* If req_buf_first==NULL, then we have already counted all the
   * buffers queued up in the previous for() loop.
   * If req_buf_first !=NULL, then count through req_buf_first->next pointers
   * till get next pointer== NULL in order to get a total count of the
   * buffers.
   */
  if (context->req_buf_first != EWA_NET_BUFFER_NULL)
  {
    for(buf_ptr= context->req_buf_first;
    ewaNetBufferNextGet(buf_ptr) != NULL;
    buf_ptr = ewaNetBufferNextGet(buf_ptr))
    {
      recvBufCount++;
    }
  }
  EMWEB_TRACE(("ewsNetHTTPReceive recvBufCount=%d\n",recvBufCount));
#endif /* EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS */

  /*
   * Add buffer chain to queue
   */
  if (context->req_buf_first == EWA_NET_BUFFER_NULL)
  {
    context->req_buf_first = buffer;
    ewsStringInit(&context->req_str_begin, buffer);
    ewsStringInit(&context->req_str_end, buffer);
  }
  else
  {
    ewaNetBufferNextSet(context->req_buf_last, buffer);
  }
  context->req_buf_last = buf_last;
#ifdef EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS
  /* If we exceed EW_CONFIG_MAX_QUEUED_RECV_BUFS, then cleanup
   * and free the context (which will free up the buffer chain for the context).
   */
  if (recvBufCount > EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS)
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "ewsNetHTTPReceive:recvBufCnt exceeds "
                        "MAX_QUEUED_RECV_BUFS! The receive buffer limit has been reached.  "
                        "Bad request or DoS attack.\n");
    return EWS_STATUS_NO_RESOURCES;
  }
#endif /* EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS */

  /*
   * If the context was waiting for more data, move
   * it from the wait_list onto the serv_list
   */
  EWA_TASK_LOCK();
  if (context->schedulingState == ewsContextDataWait)
  {
    context->schedulingState = ewsContextScheduled;
#     ifdef EW_CONFIG_OPTION_SCHED
    /*
     * Only move to serv_list if not flow controlled
     */
#     ifdef EW_CONFIG_OPTION_SCHED_FC
    if (context->flow_control == ewsContextFlowControlOff)
#     endif
    {
      EWS_LINK_DELETE(&context->link);
      EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
    }
#     endif /* EW_CONFIG_OPTION_SCHED */
  }

  /*
   * If the scheduler is enabled, then this function is done now that
   * the context is on the serve_list - it will be processed next
   * time ewsRun is called. Otherwise, process the context
   * based on its state
   */

# ifndef EW_CONFIG_OPTION_SCHED
  /*
   * mark context as running, must reset this before returning
   */
  context->current = TRUE;

  switch (context->state)
  {
#   ifdef EW_CONFIG_OPTION_CGI
    /*
     * Raw CGI: pass buffers directly to application
     */
    case ewsContextStateServingCGI:
      EMWEB_TRACE(("calling ewsServeCGIData()...\n"));
      EWA_TASK_UNLOCK();
      ewsServeCGIData( context );
      EWA_TASK_LOCK();
      break;
#   endif /* EW_CONFIG_OPTION_CGI */

      /*
       * Serving a Document/Submitting a Form:
       * We're not expecting any more data now,
       * don't do anything - buffers will be freed
       * when the context is aborted
       */

    case ewsContextStateServingDocument:
#   ifdef EW_CONFIG_OPTION_SOAP
    case ewsContextStateServingSOAPResponse:
#   endif /* EW_CONFIG_OPTION_SOAP */
#   ifdef EW_CONFIG_OPTION_FORM
    case ewsContextStateSubmittingForm:
#   endif /* EW_CONFIG_OPTION_FORM */
      EMWEB_TRACE(("Unexpected data received in state: 0x%X \n"
        ,context->state
        ));
      break;

      /*
       * These states are used for parsing HTTP headers.  Append
       * the received buffer chain to the request buffer queue and run the
       * HTTP parser until it is finished processing the new data.
       */
    case ewsContextStateParseRequest:
    case ewsContextStateParseHeaders:
#   ifdef EW_CONFIG_OPTION_FORM
    case ewsContextStateParsingForm:
#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
    case ewsContextStateParsingMPForm:
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */
#   endif /* EW_CONFIG_OPTION_FORM */
#   ifdef EW_CONFIG_OPTION_FILE_PUT
    case ewsContextStateParsingPut:
#   endif /* EW_CONFIG_OPTION_FILE_PUT */
#   ifdef EW_CONFIG_OPTION_PERSISTENT
    case ewsContextStateSkipContent:
#   endif /* EW_CONFIG_OPTION_PERSISTENT */
      /*
       * Run the state machine
       * on the new data until finished
       */
      EWA_TASK_UNLOCK();
      while ( ewsParse(context) )
    ;
      EWA_TASK_LOCK();
      break;


    default:
#   ifdef EMWEB_SANITY
      /*
       * I don't expect any other states now.  Just in case, put some debug
       * stuff here
       */
      EMWEB_WARN(("ewsNetHTTPReceive: bad context state: 0x%X\n"
        ,context->state
        ));
#   endif
      context->current = FALSE;
      EWA_TASK_UNLOCK();
      return( EWS_STATUS_BAD_STATE );
  }

  /* check to see if this context
   * was aborted during the run.  If so, clean up and free the
   * context
   */
  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
  {
    EWA_TASK_UNLOCK();
    ewsInternalAbort(context);
    return EWS_STATUS_OK;
  }
  context->current = FALSE;

# endif /* no EW_CONFIG_OPTION_SCHED */

  EWA_TASK_UNLOCK();

  return EWS_STATUS_OK;
}
#ifdef EW_CONFIG_OPTION_UPNP
/*
 * ewsNetEOF
 * Indicate the end of the connection's input stream without aborting the
 * connection.
 *
 * context      - context of request to end received data
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD)
 */
  EwsStatus
ewsNetEOF ( EwsContext context )
{
  EMWEB_TRACE(("ewsNetEOF(%p)\n", context));

  /* Set the EOF flag in the context. */
  context->flags |= EWS_CONTEXT_FLAGS_INPUT_EOF;


  /*
   * If the context was waiting for more data, move
   * it from the wait_list onto the serv_list
   */
  EWA_TASK_LOCK();
  if (context->schedulingState == ewsContextDataWait)
  {
    context->schedulingState = ewsContextScheduled;
#     ifdef EW_CONFIG_OPTION_SCHED
    /*
     * Only move to serv_list if not flow controlled
     */
#     ifdef EW_CONFIG_OPTION_SCHED_FC
    if (context->flow_control == ewsContextFlowControlOff)
#     endif
    {
      EWS_LINK_DELETE(&context->link);
      EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
    }
#     endif /* EW_CONFIG_OPTION_SCHED */
  }

  /*
   * If the scheduler is enabled, then this function is done now that
   * the context is on the serve_list - it will be processed next
   * time ewsRun is called. Otherwise, we haven't written the code yet.
   */

# ifndef EW_CONFIG_OPTION_SCHED

# error "ewaNetEOF not implemented for use without scheduler."

# endif /* no EW_CONFIG_OPTION_SCHED */

  EWA_TASK_UNLOCK();

  return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_UPNP */

/*
 * ewsInternalSetup
 * Setup a context for a new request.  This is either called from
 * ewsNetHTTPStart to set up a new connection, or from ewsInternalFinish
 * to reset context state to process new requests over an existing connection.
 *
 * context      - context of request to be reset
 *
 * No return value
 */
  void
ewsInternalSetup ( EwsContext context )
{
  uintf i;

  /*
   * HANDLES
   */
#   if defined (EW_CONFIG_OPTION_DEMAND_LOADING) \
  || defined (EW_CONFIG_OPTION_CLONING)
  context->doc_handle = EWA_DOCUMENT_HANDLE_NULL;
#   endif

#   ifdef EW_CONFIG_OPTION_AUTH
  context->auth_handle = EWA_AUTH_HANDLE_NULL;
#   if defined EW_CONFIG_OPTION_AUTH
  context->auth_state = ewsAuthStateUnauthenticated;
#   endif
#   if defined EW_CONFIG_OPTION_AUTH_DIGEST
  context->md5_remaining = 0;
  context->nonce_stale = FALSE;
  context->rcved_url = NULL;
  context->send_auth_info_hdr = FALSE;
#   endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
#   ifdef EW_CONFIG_AUTH_SCHED
#   if defined EW_CONFIG_OPTION_AUTH_MBASIC
  context->authStateParams.basic.freeCookie = FALSE;
#   if defined EW_CONFIG_OPTION_AUTH_MBASIC_DECODE
  context->authStateParams.basic.user = NULL;
#   endif /* EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */
#   endif /* EW_CONFIG_OPTION_AUTH_MBASIC */
#   endif /* EW_CONFIG_AUTH_SCHED */
#   endif /* EW_CONFIG_OPTION_AUTH */

  /*
   * REQUEST PROCESSING STATE
   */
  context->req_method = ewsRequestMethodUnknown;
  context->req_buf_used = EWA_NET_BUFFER_NULL;
  ewsStringInit(&context->req_method_str, EWA_NET_BUFFER_NULL);
  ewsStringInit(&context->req_protocol, EWA_NET_BUFFER_NULL);
  ewsStringInit(&context->req_url, EWA_NET_BUFFER_NULL);
  ewsStringInit(&context->req_query, EWA_NET_BUFFER_NULL);
#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 ||\
  defined(EW_CONFIG_OPTION_CONTEXT_HOST)
  ewsStringInit(&context->req_host, EWA_NET_BUFFER_NULL);
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */
  context->content_length = 0;

#   if defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
  || defined( EW_CONFIG_OPTION_URL_PATH_INFO )
  context->path_info = NULL;
#   endif /* EW_CONFIG_OPTION_CGI/URL_PATH_INFO */

#   ifdef EW_CONFIG_OPTION_PRIMARY
  context->secondary = EWS_SECONDARY_HANDLE_NULL;
  context->max_forwards = 0;
  context->is_secondary_context = FALSE;
  context->clen = 0;
  ewsStringInit(&context->hdr_str_begin, EWA_NET_BUFFER_NULL);
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   ifdef EW_CONFIG_OPTION_FORM
  context->form = (void *) NULL;
  context->form_enum_table_size = 0;

#   ifdef EW_CONFIG_OPTION_NS_SET
  context->form_handle = NULL;
#   endif /* EW_CONFIG_OPTION_NS_SET */

#   endif /* EW_CONFIG_OPTION_FORM */

#   ifdef EW_CONFIG_OPTION_FORM_REPEAT
  context->base_form = (void *) NULL;
  context->form_status = (void *) NULL;

#   endif /* EW_CONFIG_OPTION_FORM_REPEAT */

  for (i = 0; i < ewsRequestHeaderCount; i++)
  {
    ewsStringInit(&context->req_headerp[i], EWA_NET_BUFFER_NULL);
  }
  context->url_dynamic = FALSE;
  context->url = NULL;

#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  context->inputFile = NULL;
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EWS_LINK_INIT(&(context->inputFile_system_list));
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

  /*
   * RESPONSE PROCESSING STATE
   */
  context->doc_stack_head = (EwsDocumentContextP) NULL;
  context->doc_stack_tail = (EwsDocumentContextP) NULL;
  context->doc_stack_depth = 0;
  context->res_buf_next = EWA_NET_BUFFER_NULL;
  ewsStringInit(&context->res_str_end, EWA_NET_BUFFER_NULL);

#   ifdef EW_CONFIG_OPTION_PERSISTENT
  context->keep_alive = FALSE;
  context->have_connection_header = FALSE;
#   ifdef EW_CONFIG_OPTION_SCHED
  context->finish_yield = FALSE;
#   endif /* EW_CONFIG_OPTION_SCHED */
#   endif /* EW_CONFIG_OPTION_PERSISTENT */

#   ifdef EW_CONFIG_OPTION_CHUNKED_OUT
  context->chunk_out_state = ewsContextChunkOutNotChunked;
  ewsStringInit(&context->res_chunk, EWA_NET_BUFFER_NULL);
#   endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

  /*
   * local filesystem parameters
   */
#   ifdef EW_CONFIG_FILE_METHODS
  context->fileParams = NULL;
  context->dummy = NULL;
  context->fileHandle = EWA_FILE_HANDLE_NULL;
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  context->fileSystem = EWS_FILE_SYSTEM_HANDLE_NULL;
  context->fs_inuse = FALSE;
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#   endif /* EW_CONFIG_FILE_METHODS */

  /*
   * cookies
   */
#   ifdef EW_CONFIG_OPTION_COOKIES
  context->cookie_list_first = (EwsCookieP)NULL;
  context->cookie_list_last = (EwsCookieP)NULL;

#   endif /* EW_CONFIG_OPTION_COOKIES */

  /*
   * Content Negotiation
   */
#   ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
  context->select_state = ewsDocSelectStateStart;
  context->negotiate_hdr_first = (EwsNegotiateHdrDataP)NULL;
  context->negotiate_hdr_last = (EwsNegotiateHdrDataP)NULL;
  context->accept_hdr_first = (EwsAcceptHdrDataP)NULL;
  context->accept_hdr_last = (EwsAcceptHdrDataP)NULL;
  context->accept_charset_hdr_first = (EwsAcceptHdrDataP)NULL;
  context->accept_charset_hdr_last = (EwsAcceptHdrDataP)NULL;
  context->accept_language_hdr_first = (EwsAcceptHdrDataP)NULL;
  context->accept_language_hdr_last = (EwsAcceptHdrDataP)NULL;
  context->tcn_hdr = ewsTcnHdrNone;
  context->alt_hdr = NULL;
  context->vary_hdr = NULL;
  context->accept_encoding_hdr_first = (EwsAcceptHdrDataP)NULL;
  context->accept_encoding_hdr_last = (EwsAcceptHdrDataP)NULL;
  context->ck_accept_encoding = TRUE;
#   endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */
  context->content_encoding = NULL;

#   ifdef EW_CONFIG_OPTION_SMTP
  context->smtp = NULL;
#   endif /* EW_CONFIG_OPTION_SMTP */

#   ifdef EW_CONFIG_OPTION_TELNET
  context->telnet = NULL;
#   endif /* EW_CONFIG_OPTION_TELNET */

#   ifdef EW_CONFIG_OPTION_CLIENT
  context->client = NULL;
  context->cmp = NULL;
  context->override_status = NULL;
#   ifdef EW_CONFIG_OPTION_CLIENT_CACHE
  context->parse_from_file = FALSE;
#   endif /* EW_CONFIG_OPTION_CLIENT_CACHE */
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   ifdef EW_CONFIG_OPTION_SNMP_AGENT
  context->snmpHandle = EWA_SNMP_AGENT_HANDLE_NULL;
#   ifdef EW_CONFIG_OPTION_AUTH
  context->snmpAuthHandle = EWA_SNMP_AGENT_AUTH_HANDLE_NULL;
#   endif /* EW_CONFIG_OPTION_AUTH */
#   endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#   if defined( EW_CONFIG_OPTION_NS_GET ) || defined( EW_CONFIG_OPTION_NS_SET )
  context->currentElement = NULL;
  context->schemaNodes = NULL;
  context->archiveData = NULL;
#   endif /* EW_CONFIG_OPTION_NS_* */

#   ifdef EW_CONFIG_OPTION_GROUP_DYNAMIC
  context->group_dynamic_count = 0;
#   endif /* EW_CONFIG_OPTION_GROUP_DYNAMIC */

#   ifdef EW_CONFIG_OPTION_SOAP
  ewuSoapInitContext(context);
#   endif /* EW_CONFIG_OPTION_SOAP */

  /*
   * clear flags that need to be reset between requests on the same connection
   */
  context->flags &= ~ EWS_CONTEXT_FLAGS_RESET_BETWEEN_REQUESTS;

#   ifdef EW_CONFIG_OPTION_CONVERT_XML
  context->xmlCurrentNamespace = NULL;
#   endif /* EW_CONFIG_OPTION_CONVERT_XML */

  /*
   * UPnP support
   */
#   ifdef EW_CONFIG_OPTION_UPNP
  context->upnp_parse_ddd = FALSE;
  context->upnp_parse_root_device = NULL;
  context->upnp_parse_device = NULL;
  context->upnp_parse_service = NULL;
  context->scratch_length_allocated = 0;
  context->scratch_length_used = 0;
  context->scratch = NULL;
  context->isMsearchResponse = FALSE;
#   endif /* EW_CONFIG_OPTION_UPNP */

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES
  /*
   * Byte range support
   */
  context->isRange = FALSE;
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#   ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
  /*
   * Application Etag generation
   */
  context->etag = NULL;
  context->max_age = EWS_CACHE_REVALIDATE;
#   endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */
  context->unbufferedWrite = FALSE;

}

/*
 * ewsInternalCleanup
 * Clean up resources used by current request in preparation for either
 * subsequent request (if called by ewsInternalFinish) or connection abort
 * (if called by ewsInternalAbort).
 *
 * context      - context of request to be finished
 *
 * No return value
 */
  void
ewsInternalCleanup ( EwsContext context )
{
  EwsDocumentContextP   doc_context;

  /*
   * Release buffers queued for transmission
   */
  if (   context->res_buf_next != EWA_NET_BUFFER_NULL
#       ifdef EW_CONFIG_OPTION_TELNET
      && context->telnet == NULL
#       endif /* EW_CONFIG_OPTION_TELNET */
     )
  {
    ewaNetBufferFree(context->res_buf_next);
  }

  /*
   * Release document context stack
   */
  while (context->doc_stack_head != (EwsDocumentContextP) NULL)
  {
    doc_context = context->doc_stack_head;
    context->doc_stack_head = doc_context->next;
#       ifdef EW_CONFIG_FILE_METHODS
    if (!(doc_context->document->flags & EWS_FILE_FLAG_FILESYSTEM))
#       endif /* EW_CONFIG_FILE_METHODS */
    {
      doc_context->document->archive->reference_count--;
    }
    ewsFreeDocContext( doc_context );
  }

#   ifdef EW_CONFIG_OPTION_FORM
  /*
   * Release form if any
   */
  if (context->form != NULL)
  {
    ewsFormFree(context);
  }

#   ifdef EW_CONFIG_OPTION_NS_SET
  if (context->form_handle != NULL)
  {
    (*context->form_ns->set_finish_f)(context
                      ,context->form_handle
                      ,EWS_STATUS_ABORTED);
  }
#   endif /* EW_CONFIG_OPTION_NS_SET */

#   endif /* EW_CONFIG_OPTION_FORM */

#   ifdef EW_CONFIG_AUTH_SCHED
  /*
   * Release any authentication state
   */
#   if defined EW_CONFIG_OPTION_AUTH_MBASIC
  if (context->authStateParams.basic.freeCookie)
    ewaFree( context->authStateParams.basic.cookie );
#   if defined EW_CONFIG_OPTION_AUTH_MBASIC_DECODE
  if (context->authStateParams.basic.user)
    ewaFree( context->authStateParams.basic.user );
#   endif /* EW_CONFIG_OPTION_AUTH_MBASIC_DECODE */
#   endif /* EW_CONFIG_OPTION_AUTH_MBASIC */
#   endif /* EW_CONFIG_AUTH_SCHED */

  /*
   * Release dynamically created contiguous URL
   */
  if (context->url_dynamic)
  {
    ewaFree(context->url);
  }

#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  /*
   * release form-data support
   */
  if (context->inputFile != NULL)
  {
    if (context->inputFile->formDataBoundary != NULL)
    {
      ewaFree( context->inputFile->formDataBoundary );
    }
    ewaFree( context->inputFile );
  }
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  if (!(EWS_LINK_IS_EMPTY(&(context->inputFile_system_list))))
  {
    /*
     * free the list of file systems
     */
    EwsInputFileSystemEntry entry, next;
    next = (EwsInputFileSystemEntry)EWS_LINK_HEAD(context->inputFile_system_list);
    do
    {
      entry = next;
      next = (EwsInputFileSystemEntry)(EWS_LINK_NEXT(next->link));
      EWS_LINK_DELETE(&(entry->link));
      ewaFree(entry);
    } while (next != (EwsInputFileSystemEntry)EWS_LINK_HEAD(context->inputFile_system_list));
  }
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

  /*
   * reset the local filesystem's param block pointer,
   * free any associated document if it's for a local file system
   * object.
   */
#   ifdef EW_CONFIG_FILE_METHODS
  context->fileParams = NULL;
  if (context->dummy != NULL)
  {
    ewaFree( context->dummy );
    context->dummy = NULL;
  }
  if (context->fileHandle != EWA_FILE_HANDLE_NULL)
  {
#       ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
    if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
    {
      (context->fileSystem->use_cnt)--;
      context->fs_inuse = FALSE;
      if (context->fileSystem->fs.fileClose == NULL)
      {
    EMWEB_ERROR(("ewsInternalCleanup: NULL fs fileClose function\n"));
      }
      context->fileSystem->fs.fileClose(context->fileHandle, EWS_STATUS_ABORTED);
    }
#       else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    ewaFileClose(context->fileHandle, EWS_STATUS_ABORTED);
#       endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    context->fileHandle = EWA_FILE_HANDLE_NULL;
  }
#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  else
  {
    if (context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
    {
      /* we may have some cleanup to do on file system */
      if (context->fs_inuse == TRUE)
      {
    (context->fileSystem->use_cnt)--;
    context->fs_inuse = FALSE;
      }
    }
  }
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

#   ifdef EW_CONFIG_OPTION_CLIENT_CACHE
  /*
   * If parsing from file, clean up file system resources
   */
  if (context->parse_from_file)
  {
#       ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
    (context->source_fs->use_cnt)--;
    context->source_fs->fs.fileClose ( context->source_file
    ,EWS_STATUS_ABORTED );
#       else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    ewaFileClose(context->source_file, EWS_STATUS_ABORTED);
#       endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    context->parse_from_file = FALSE;
  }
#   endif /* EW_CONFIG_OPTION_CLIENT_CACHE */

#   endif /* EW_CONFIG_FILE_METHODS */

# ifdef EW_CONFIG_OPTION_COOKIES

  if (context->cookie_list_first != (EwsCookieP)NULL)   /* free cookies' values         */
  {
    EwsCookieP vp, vp_next=NULL;

    vp = context->cookie_list_first;
    while (vp != NULL)
    {
      vp_next = vp->next;
      ewaFree((uint8 *)vp);
      vp = vp_next;
    }
  }
# endif /* EW_CONFIG_OPTION_COOKIES */

# ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION

  /* free content negotiation header info */

  if (context->vary_hdr)
  {
    ewaFree (context->vary_hdr);
  }

  while (context->negotiate_hdr_first != (EwsNegotiateHdrDataP) NULL)
  {
    EwsNegotiateHdrDataP tmp = context->negotiate_hdr_first;
    if (tmp->directive != NULL)
    {
      ewaFree(tmp->directive);
    }
    context->negotiate_hdr_first = tmp->next;
    ewaFree(tmp);
  }
  context->negotiate_hdr_last = (EwsNegotiateHdrDataP) NULL;

  while (context->accept_hdr_first != (EwsAcceptHdrDataP) NULL)
  {
    EwsAcceptHdrDataP tmp = context->accept_hdr_first;
    if (tmp->data != NULL)
    {
      ewaFree(tmp->data);
    }

    if (tmp->qual != NULL)
    {
      ewaFree(tmp->qual);
    }
    context->accept_hdr_first = tmp->next;
    ewaFree(tmp);
  }
  context->accept_hdr_last = (EwsAcceptHdrDataP) NULL;

  while (context->accept_charset_hdr_first != (EwsAcceptHdrDataP) NULL)
  {
    EwsAcceptHdrDataP tmp = context->accept_charset_hdr_first;
    if (tmp->data != NULL)
    {
      ewaFree(tmp->data);
    }

    if (tmp->qual != NULL)
    {
      ewaFree(tmp->qual);
    }

    context->accept_charset_hdr_first = tmp->next;
    ewaFree(tmp);
  }
  context->accept_charset_hdr_last = (EwsAcceptHdrDataP) NULL;

  while (context->accept_language_hdr_first != (EwsAcceptHdrDataP) NULL)
  {
    EwsAcceptHdrDataP tmp = context->accept_language_hdr_first;
    if (tmp->data != NULL)
    {
      ewaFree(tmp->data);
    }

    if (tmp->qual != NULL)
    {
      ewaFree(tmp->qual);
    }

    context->accept_language_hdr_first = tmp->next;
    ewaFree(tmp);
  }
  context->accept_language_hdr_last = (EwsAcceptHdrDataP) NULL;

  while (context->accept_encoding_hdr_first != (EwsAcceptHdrDataP) NULL)
  {
    EwsAcceptHdrDataP tmp = context->accept_encoding_hdr_first;
    if (tmp->data != NULL)
    {
      ewaFree(tmp->data);
    }

    if (tmp->qual != NULL)
    {
      ewaFree(tmp->qual);
    }

    context->accept_encoding_hdr_first = tmp->next;
    ewaFree(tmp);
  }
  context->accept_encoding_hdr_last = (EwsAcceptHdrDataP) NULL;
# endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

# ifdef EW_CONFIG_OPTION_SNMP_AGENT
  /*
   * note the order of this call: it MUST be called
   * BEFORE ewaNetHTTPCleanup().  Note I also call this
   * routine regardless of whether snmpHandle is NULL or
   * not - the very act of calling it gives the SNMP
   * interface code a chance to clean up internal state,
   * even if the handle is unused.
   */
  ewaSNMPAgentHandleCleanup( context, context->snmpHandle );
  context->snmpHandle = EWA_SNMP_AGENT_HANDLE_NULL;
# endif /* EW_CONFIG_OPTION_SNMP_AGENT */

  /*
   * call the application's cleanup routine to allow it
   * to reset any state stored in the net handle.  Call only
   * if a request was seen since the last time we called
   * cleanup (or context init)
   */
  if (context->flags & EWS_CONTEXT_FLAGS_CLEANUP)
  {
    context->flags &= ~EWS_CONTEXT_FLAGS_CLEANUP;
    ewaNetHTTPCleanup( context->net_handle );
  }

# ifdef EW_CONFIG_OPTION_SMTP
  if (context->smtp != NULL)
  {
    ewaFree(context->smtp);
    context->smtp = NULL;
  }
# endif /* EW_CONFIG_OPTION_SMTP */

# ifdef EW_CONFIG_OPTION_AUTH_DIGEST
  if (context->rcved_url != NULL)
  {
    ewaFree(context->rcved_url);
    context->rcved_url = NULL;
  }
# endif

# ifdef EW_CONFIG_OPTION_UPNP

  /*
   * Clean up UPnP resources.
   */
  /* Free the scratch space. */
  if (NULL != context->scratch)
  {
    ewaFree(context->scratch);
    context->scratch_length_allocated = 0;
    context->scratch_length_used = 0;
    context->scratch = NULL;
  }

# ifdef EW_CONFIG_OPTION_SOAP
  ewuSoapCleanContext(context);
# endif /* EW_CONFIG_OPTION_SOAP */

# endif /* EW_CONFIG_OPTION_UPNP */

#   ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
  /*
   * Application Etag generation
   */
  if ( NULL != context->etag )
  {
    ewaFree( context->etag );
  }
#   endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */

}

/*
 * ewsInternalFinish
 * Finish processing current request.  If a persistent connection is to be
 * maintained, then the connection state is reset to handle the next request.
 * Otherwise, the connection is aborted.
 *
 * context      - context of request to be finished
 *
 * No return value
 *
 * Note that this function calls ewsInternalCleanup, so any processing that
 * should also be done at the end of aborted transactions should be
 * coded in ewsInternalCleanup, not here.
 */
  void
ewsInternalFinish ( EwsContext context )
{
# ifdef EW_CONFIG_OPTION_PERSISTENT
  EwaStatus status;
# endif /* EW_CONFIG_OPTION_PERSISTENT */

# ifdef EW_CONFIG_OPTION_SMTP
  if (context->smtp != NULL)
  {
    ewsSmtpFinish(context);
    return;
  }
# endif /* EW_CONFIG_OPTION_SMTP */

# ifdef EW_CONFIG_OPTION_TELNET
  if (context->telnet != NULL)
  {
    ewsTelnetFinish(context);
    return;
  }
# endif /* EW_CONFIG_OPTION_TELNET */

#ifdef EW_CONFIG_OPTION_CLIENT
  if (context->client != NULL)
  {
    ewsClientFinish(context);
    return;
  }
#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EW_CONFIG_OPTION_UPNP
  if (context->upnp_parse_ddd)
  {
    ewuParseFinish(context);
    /* Continue executing to shut down pseudo-connection. */
  }
#endif /* EW_CONFIG_OPTION_UPNP */

  /*
   * Flush data to network
   */
#ifdef EW_CONFIG_OPTION_PERSISTENT
  status = ewsFlushAll(context);
#else /* EW_CONFIG_OPTION_PERSISTENT */
  (void) ewsFlushAll(context);
#endif /* EW_CONFIG_OPTION_PERSISTENT */

#ifdef EW_CONFIG_OPTION_PERSISTENT
  /*
   * If connection marked for keep alive, then release resources and reset
   * state to process next request.  However, if flush status was error,
   * we must abort the connection now.
   */
  if (context->keep_alive && status != EWA_STATUS_ERROR)
  {
    EwaNetBuffer currbuf, prevbuf, lastbuf;

#       ifdef EW_CONFIG_OPTION_SCHED
    /*
     * If status was yield, set flag for scheduler
     */
    context->finish_yield = (status == EWA_STATUS_OK_YIELD);
#       endif /* EW_CONFIG_OPTION_SCHED */

    /*
     * Update parser to end of previous request
     */
    context->req_str_begin = context->req_str_end;
    ewsStringLengthSet(&context->req_str_begin, 0);

    /*
     * Release previous context resources (except for request buffers)
     * and initialize state for next connection.
     */
    ewsInternalCleanup (context);
    ewsInternalSetup   (context);
    context->state = ewsContextStateSkipContent;

    /*
     * Release request buffers held over from previous request up to
     * received data for next request (if any).
     */
    for ( currbuf = context->req_buf_first
    ,lastbuf = ewsStringBuffer(&context->req_str_begin)
    ,prevbuf = EWA_NET_BUFFER_NULL
    ; currbuf != lastbuf
    ; currbuf = ewaNetBufferNextGet(currbuf))
    {
      prevbuf = currbuf;
    }
    if (prevbuf != EWA_NET_BUFFER_NULL)
    {
      ewaNetBufferNextSet(prevbuf, EWA_NET_BUFFER_NULL);
      ewaNetBufferFree(context->req_buf_first);
      context->req_buf_first = currbuf;
    }
  }
  else

#endif /* EW_CONFIG_OPTION_PERSISTENT */
  {
    /*
     * If not a persistent connection, simply abort connection
     */
    ewsNetHTTPAbort ( context );
  }
}

/*
 * ewsInternalAbort
 * Abort a previously started HTTP request.
 *
 * context      - context of request to be aborted
 *
 * Returns EWA_STATUS_OK or EWA_STATUS_OK_YIELD from ewaNetHTTPEnd function.
 *
 * Note that this function calls ewsInternalCleanup, so any processing that
 * should also be done at the end of non-aborted transactions should be
 * coded in ewsInternalCleanup, not here.
 */
  EwaStatus
ewsInternalAbort ( EwsContext context )
{
  EwaStatus             status;

  EMWEB_TRACE(("ewsInternalAbort(%p)\n", context));

  EWA_TASK_LOCK();
  context->abort_flags |= EWS_CONTEXT_FLAGS_INTERNAL_ABORT;
  EWA_TASK_UNLOCK();

#   ifdef EW_CONFIG_OPTION_CLIENT
  /*
   * If this is an EmWeb/Client context, signal abort and return.
   */
  if (context->cmp != NULL)
  {
    ewsClientFinish(context);
    return EWA_STATUS_OK;
  }
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   ifdef EW_CONFIG_OPTION_PRIMARY
  if (   context->secondary != EWS_SECONDARY_HANDLE_NULL
      && context->secondary->context_current == context
     )
  {
    if (context->secondary->opened)
    {
      context->secondary->opened = FALSE;
      ewaSecondaryClose(context->secondary->handle);
      ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
      return EWA_STATUS_OK;
    }
    ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
    if (context->schedulingState != ewsContextDataWait)
    {
      return EWA_STATUS_OK;
    }
  }
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   ifdef EW_CONFIG_OPTION_SCHED
  EWA_TASK_LOCK();
  /*
   * Special case: If the aborting context is the next context to be
   * scheduled by the scheduler, then we must advance the scheduler's
   * next context pointer before we remove the context from the list!
   */
  if (context == (EwsContext) ews_state->context_serve_next)
  {
    ews_state->context_serve_next = context->link.next;
  }

  /*
   * Remove context from serve, wait, or fault list
   */
  EWS_LINK_DELETE(&context->link);
  EWA_TASK_UNLOCK();
#   endif /* EW_CONFIG_OPTION_SCHED */

#   ifdef EW_CONFIG_OPTION_FILE_GET
  {
    EwsDocumentContextP   doc_context;
    for (doc_context = context->doc_stack_head;
    doc_context != (EwsDocumentContextP) NULL;
    doc_context = doc_context->next)
    {
      if (doc_context->fileHandle != EWA_FILE_HANDLE_NULL)
      {
#             ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
    if (doc_context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
    {
      (doc_context->fileSystem->use_cnt)--;
      doc_context->fs_inuse = FALSE;
      if (context->fileSystem->fs.fileClose == NULL)
      {
        EMWEB_ERROR(("ewsInternalAbort: NULL fs fileClose function\n"));
      }
      doc_context->fileSystem->fs.fileClose(
          doc_context->fileHandle
          , EWS_STATUS_ABORTED);
    }
#             else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    ewaFileClose( doc_context->fileHandle, EWS_STATUS_ABORTED );
#             endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    doc_context->fileHandle = EWA_FILE_HANDLE_NULL;
      }
#         ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      else
      {
    if (doc_context->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
    {
      /* we may have some cleanup to do on file system */
      if (doc_context->fs_inuse == TRUE)
      {
        (doc_context->fileSystem->use_cnt)--;
        doc_context->fs_inuse = FALSE;
      }
    }
      }
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
    }
  }
#   endif /* EW_CONFIG_OPTION_FILE_GET */

  /*
   * Release buffers retained for request headers
   */
  if (context->req_buf_first != EWA_NET_BUFFER_NULL)
  {
    ewaNetBufferFree(context->req_buf_first);
    context->req_buf_first = context->req_buf_last = EWA_NET_BUFFER_NULL;
  }

#   ifdef EW_CONFIG_OPTION_TELNET
  if (context->telnet != NULL)
  {
    if (context->telnet->ssh)
    {
      ConnectionsActiveSSH--;
    }
    else
      ConnectionsActiveTelnet--;    /* LVL7_P0006 */

    ewaFree(context->telnet);
    context->telnet = NULL;
  }
#   endif /* EW_CONFIG_OPTION_TELNET */

  /*
   * Clean up other resources associated with context,
   */
  ewsInternalCleanup(context);

  /*
   * Inform application of terminated request (unless special secondary
   * context)
   */
  if (context->net_handle != EWA_NET_HANDLE_NULL)
  {
#       ifdef EW_CONFIG_OPTION_PRIMARY
    if (!context->is_secondary_context)
    {
      status = ewaNetHTTPEnd(context->net_handle);
    }
    else
    {
      status = EWA_STATUS_OK;
      return status;
    }
#       else /* EW_CONFIG_OPTION_PRIMARY */
    status = ewaNetHTTPEnd(context->net_handle);
#       endif /* EW_CONFIG_OPTION_PRIMARY */
  }
  else
  {
    status = EWA_STATUS_OK;
  }

  /*
   * Release context and return
   */
  ewaFree(context);
  return status;
}

#ifdef EW_CONFIG_OPTION_SCHED_FC
/*
 * ewsNetFlowControl
 * Mark context for flow control to avoid predicted congestion.  ewsRun() will
 * place the context on the suspended list at the next opportunity and
 * continue by processing additional requests.
 *
 * context      - context of request to be flow controlled
 *
 * Returns EWS_STATUS_OK.
 */
  EwsStatus
ewsNetFlowControl ( EwsContext context )
{
  /*
   * If not yet flow controlled, set flag.  ewsRun() will suspend the
   * request at the next opportunity.
   */
  EWA_TASK_LOCK();
  if (context->flow_control == ewsContextFlowControlOff)
  {
    context->flow_control = ewsContextFlowControlPending;
#       ifdef EW_CONFIG_OPTION_PRIMARY
    if (context->secondary != EWS_SECONDARY_HANDLE_NULL &&
    context->secondary->context_current == context)
    {
      ewaSecondaryFlowControl(context->secondary->handle);
    }
#       endif /* EW_CONFIG_OPTION_PRIMARY */
  }
  EWA_TASK_UNLOCK();
  return EWS_STATUS_OK;
}

/*
 * ewsNetUnFlowControl
 * Resume previously flow controlled context
 *
 * context      - context of request to be flow controlled
 *
 * Returns EWS_STATUS_OK.
 */
  EwsStatus
ewsNetUnFlowControl ( EwsContext context )
{
  /*
   * Handle by state
   */
  EWA_TASK_LOCK();
  switch ( context->flow_control )
  {
    /*
     * If not already flow controlled, do nothing
     */
    case ewsContextFlowControlOff:
      break;

      /*
       * If suspended and for flow control, and not suspended for any other
       * reason, then reschedule the context.
       */
    case ewsContextFlowControlOn:

      if (   context->schedulingState != ewsContextDataWait
#                ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
      && context->schedulingState != ewsContextSuspended
#                endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
     )
      {
    EWS_LINK_DELETE(&context->link);
    EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
      }

      /* No break -- fall through! */

    case ewsContextFlowControlPending: /* just clear flag */
      context->flow_control = ewsContextFlowControlOff;
#         ifdef EW_CONFIG_OPTION_PRIMARY
      if (context->secondary != EWS_SECONDARY_HANDLE_NULL &&
      context->secondary->context_current == context)
      {
    ewaSecondaryUnFlowControl(context->secondary->handle);
      }
#         endif /* EW_CONFIG_OPTION_PRIMARY */
      break;

#       ifdef EMWEB_SANITY
    default:
      EMWEB_ERROR(("ewsNetFlowControl: bad state\n"));
      break;
#       endif /* EMWEB_SANITY */
  }

  EWA_TASK_UNLOCK();
  return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_SCHED_FC */

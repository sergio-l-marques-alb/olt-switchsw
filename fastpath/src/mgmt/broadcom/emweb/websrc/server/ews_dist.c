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
 * EmWeb/Server distributed primary/secondary interface support
 *
 */

#include "ews_gway.h"
#include "ews_send.h"
#include "ews_str.h"

#ifdef EW_CONFIG_OPTION_PRIMARY
/* START-STRIP-EMWEB-LIGHT */
/******************************************************************************
 * DISTRIBUTED PRIMARY/SECONDARY INTERFACE SUPPORT
 *
 * PRIMARY
 *   Refers to an EmWeb/Server that can act as an HTTP gateway to forward
 *   requests to one or more secondary HTTP servers.
 *
 * SECONDARY
 *   Refers to an HTTP server that receives forwarded requests from the
 *   primary.  This may not be an EmWeb/Server, but should implement
 *   HTTP/1.1 protocols.  If secondaries are HTTP/1.0 servers, then the
 *   primary should also be configured as an HTTP/1.0 server.
 *
 * When acting as a gateway, the primary forwards HTTP requests to the
 * secondary as follows:
 *
 *   1) if (Max-Forwards == 0 && method == TRACE)
 *      then don't forward.  Handle trace locally.
 *
 *   2) rewrite request line for HTTP/1.1 and rewritten URL.
 *
 *   3) decrement Max-Forwards if present
 *
 *   4) remove (by clobbering) recognized hop-to-hop headers (i.e. Connection).
 *
 *   5) append a Via: header indicating the received protocol and local host
 *
 *   6) append a Host: header if not otherwise present
 *
 * The response from the secondary is forwarded back to the user agent as
 * follows:
 *
 *   1) For HTTP/0.9 requests, the status line and headers are discarded.
 *
 *   2) Otherwise, the status line is rewritten to reflect the primary's
 *      protocol version.
 *
 *   3) remove (by clobbering) recognized hop-to-hop headers (i.e. Connection).
 *
 *   4) If the request was from an HTTP/1.0 or earlier client, and the response
 *      uses chunked encoding, then eliminate chunk headers and trailers and
 *      remove (by clobbering) the Transfer-Encoding header.
 */

/*****************************************************************************/

/*
 * ewsSecondaryRegister
 * Register a secondary with the primary -- exchange handles.   This connection
 * will not be used by EmWeb/Server until the application indicates that it is
 * open by calling ewsSecondaryOpened().
 */
EwsSecondaryHandle
ewsSecondaryRegister ( EwaSecondaryHandle handle )
{
  EwsSecondaryHandle secondary;

    EMWEB_TRACE (( "ewsSecondaryRegister(%p)\n", (void *) handle ));

    /*
     * Allocate per-secondary structure and initialize
     */
    secondary = (EwsSecondaryHandle) ewaAlloc(sizeof(EwsSecondaryHandle_t));
    if (secondary != (EwsSecondaryHandle) NULL)
      {
        secondary->context_current = EWS_CONTEXT_NULL;
        EWS_LINK_INIT(&secondary->context_list);
        secondary->handle = handle;
        secondary->opened = FALSE;
        secondary->eof = FALSE;

        /*
         * We allocate a special context block to handle parsing of the
         * response from the secondary and forwarding data back to the
         * user agent.  This context is different from those created
         * by requests from user agents, and is marked by setting the
         * is_secondary_context flag.
         *
         * NOTE: ewsNetHTTPStart() puts context on context_wait_list
         * where this context stays 'forever' (until unregister call
         * is issued).
         */
        secondary->response_context = ewsNetHTTPStart(0);
        if (secondary->response_context == EWS_CONTEXT_NULL)
          {
            return EWS_SECONDARY_HANDLE_NULL;
          }
        secondary->response_context->is_secondary_context = TRUE;
        secondary->response_context->state = ewsContextStateForwarding;
        secondary->response_context->substate.forwarding = ewsContextSubstateResponse;
        secondary->response_context->secondary = secondary;

        /*
         * Link secondaries on server-wide list of registered secondaries
         */
        EWA_TASK_LOCK();
        EWS_LINK_INSERT(&ews_state->secondary_list, &secondary->link);
        EWA_TASK_UNLOCK();
      }
    return secondary;
}

/*
 * ewsSecondaryUnregister
 * Unregister a previously registered secondary.  If any requests are waiting
 * for the secondary (because the connection is not open), then the
 * ewaURLHook() will be reinvoked for these requests.  This gives the
 * application a second chance to redirect to a different secondary, or to
 * handle the error locally.
 *
 * Following this call, the given EwsSecondaryHandle value is no longer
 * valid and all resources associated with it will have been released.
 */
EwsStatus
ewsSecondaryUnregister ( EwsSecondaryHandle secondary )
{
    EMWEB_TRACE (( "ewsSecondaryUnregister(%p)\n", (void *) secondary ));
#   ifdef EMWEB_SANITY
    /*
     * Check state
     */
    if (secondary->opened)
      {
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Remove from list of secondaries
     */
    EWA_TASK_LOCK();
    EWS_LINK_DELETE(&secondary->link);
    EWA_TASK_UNLOCK();

    /*
     * Walk list of waiting contexts.  Change state of each context to
     * ewsContextStateFindDocument and reschedule so that the ewaURLHook()
     * may redirect requests.
     */
    while (!EWS_LINK_IS_EMPTY(&secondary->context_list))
      {
        EwsContext context = (EwsContext) secondary->context_list.next;
        context->state = ewsContextStateFindDocument;
#       ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
        context->next_url_hook = NULL;
#       endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */
        context->secondary = EWS_SECONDARY_HANDLE_NULL;
        EWS_LINK_DELETE(&context->link);
        EWA_TASK_LOCK();
        EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
        EWA_TASK_UNLOCK();
      }

    /*
     * Release resources and return:
     *  remove special response context from the context_wait_list
     *  and free it.
     */
    EWS_LINK_DELETE(&secondary->response_context->link);
    ewaFree(secondary->response_context);
    ewaFree(secondary);
    return EWS_STATUS_OK;
}

/*
 * ewsSecondaryOpened
 * Notify EmWeb that a secondary connection has opened.  If requests are
 * waiting for the secondary, move the first request to the scheduler table
 * for further processing.
 */
EwsStatus
ewsSecondaryOpened ( EwsSecondaryHandle secondary )
{
    EMWEB_TRACE (( "ewsSecondaryOpened(%p)\n", (void *) secondary ));
#   ifdef EMWEB_SANITY
    /*
     * If already opened, bad state
     */
    if (secondary->opened)
      {
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    EWA_TASK_LOCK();
    secondary->opened = TRUE;

    /*
     * If no running context and context waiting, make current and move to
     * scheduler list for processing.
     */
    if (secondary->context_current == EWS_CONTEXT_NULL &&
        !EWS_LINK_IS_EMPTY(&secondary->context_list))
      {
        EwsContext context = (EwsContext) secondary->context_list.next;
        secondary->context_current = context;
        EWS_LINK_DELETE(&context->link);
        EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
      }
    EWA_TASK_UNLOCK();
    return EWS_STATUS_OK;
}

/*
 * ewsSecondaryClosed
 * Notify EmWeb that a secondary connection has closed.  If a request is in
 * progress, it must be terminated.
 */
EwsStatus
ewsSecondaryClosed ( EwsSecondaryHandle secondary )
{
    EMWEB_TRACE (( "ewsSecondaryClosed(%p)\n", (void *) secondary ));
#   ifdef EMWEB_SANITY
    /*
     * Check state
     */
    if (!secondary->opened)
      {
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    secondary->opened = FALSE;

    /*
     * If request in progress, it must be terminated.
     */
    if (secondary->context_current != EWS_CONTEXT_NULL)
      {
        /*
         * If no buffers have been received from the secondary before closing,
         * then abort the connection.
         */
        if (secondary->response_context->req_buf_first == EWA_NET_BUFFER_NULL)
          {
            ewsDistFinish(secondary, EWS_STATUS_ABORTED);
          }

        /*
         * Otherwise, set EOF flag.  The data must be forwarded back to the
         * user agent before termination.  If parser is waiting for data,
         * reschedule.  Force connection close.
         */
        else
          {
            secondary->eof = TRUE;
#           ifdef EW_CONFIG_OPTION_PERSISTENT
            secondary->context_current->keep_alive = FALSE;
#           endif /* EW_CONFIG_OPTION_PERSISTENT */
            if (secondary->response_context->schedulingState ==
                ewsContextDataWait)
              {
                EWA_TASK_LOCK();
                EWS_LINK_DELETE(&secondary->response_context->link);
                EWS_LINK_INSERT(&ews_state->context_serve_list
                               ,&secondary->response_context->link);
                secondary->response_context->schedulingState =
                  ewsContextScheduled;
                EWA_TASK_UNLOCK();
              }
          }
      }
    return EWS_STATUS_OK;
}

/*
 * ewsSecondaryRedirect
 * Called by application's ewaURLHook() to redirect a request to a secondary
 * for processing.  In addition to this redirection, the application may
 * rewrite the URL as usual.
 */
EwsStatus
ewsSecondaryRedirect ( EwsContext context, EwsSecondaryHandle secondary )
{
    EMWEB_TRACE (( "ewsSecondaryRedirect(%p, %p)\n"
                 , (void *) context
                 , (void *) secondary ));
#   ifdef EMWEB_SANITY
    /*
     * Check state - only allowed from ewaURLHook()
     */
    if (context->state != ewsContextStateFindDocument)
      {
        EMWEB_WARN(("ewsSecondaryRedirect: bad state\n"));
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * If max-forwards specified, decrement hop count by rewriting header
     */
    if (context->max_forwards != 0)
      {
        char *ltoa_ptr;
        char ltoa_string[12];
        EwsString *ep = &context->req_headerp[ewsRequestHeaderMaxForwards];
        uintf bytes = ewsStringLength(ep);
        uintf len;

          EMWEB_LTOA(ltoa_ptr
                    ,context->max_forwards - 1
                    ,ltoa_string
                    ,sizeof(ltoa_string));
          EMWEB_STRLEN(len, ltoa_ptr);
          while (len < bytes)
            {
              ewsStringRewriteChar(ep, ' ');
              len++;
            }
          while(*ltoa_ptr != '\0')
            {
              ewsStringRewriteChar(ep, *ltoa_ptr);
              ltoa_ptr++;
            }
      }

    /*
     * If HTTP/1.0 or HTTP/0.9 request, set downgrade flag.  This will be used
     * later in translating the response.
     */
    if (context->req_protocol_maj == 0 ||
        (context->req_protocol_maj == 1 && context->req_protocol_min == 0))
      {
        context->downgrade = TRUE;
      }
    else
      {
        context->downgrade = FALSE;
      }

    /*
     * Associate context with secondary and return.  Processing will
     * continue after checking for TRACE method with non-zero hop count
     * from ewsServeFindDocument.
     */
    context->secondary = secondary;
    return EWS_STATUS_OK;
}

/*
 * ewsSecondaryData
 * Pass data received from secondary connection to primary.  This data is
 * forwarded to the corresponding user agent.
 */
EwsStatus
ewsSecondaryData ( EwsSecondaryHandle secondary /* secondary handle */
                  ,const uint8        *datap    /* pointer to buffer */
                  ,uintf              bytes     /* size of buffer */
                 )
{
  EwsContext context = secondary->response_context;
  EwaNetBuffer first, last;
  EwsString estring;

    /*
     * Check state and disregard if no current request
     */
    if (secondary->context_current == EWS_CONTEXT_NULL)
      {
        EMWEB_TRACE(("ewsSecondaryData: unexpected data\n"));
        return EWS_STATUS_BAD_STATE;
      }

    /*
     * Append data to response chain
     */
    ewsStringInit(&estring, EWA_NET_BUFFER_NULL);
    first = ewsStringAttachBuffer(&estring);
    if (first == EWA_NET_BUFFER_NULL)
      {
        EMWEB_WARN(("ewsSecondaryData: no resources\n"));
        ewsDistFinish(secondary, EWS_STATUS_ABORTED);
        return EWS_STATUS_NO_RESOURCES;
      }
    last = ewsStringNCopyIn(&estring, datap, bytes);
    if (last == EWA_NET_BUFFER_NULL)
      {
        EMWEB_WARN(("ewsSecondaryData: no resources\n"));
        ewsDistFinish(secondary, EWS_STATUS_ABORTED);
        return EWS_STATUS_NO_RESOURCES;
      }
    ewaNetBufferLengthSet(last, estring.offset);

    /*
     * Pass data buffers to secondary context parser
     */
    return ewsNetHTTPReceive(context, first);
}

/*
 * ewsDistRun
 * Internal:  Called by ewsRun() to handle forwarding states
 */
void
ewsDistRun(EwsContext context)
{
  uintf bytes;
  sintf bytes_sent;
  uintf match_c;

    /*
     * If writing fixed data to secondary
     */
    if (context->clen > 0)
      {
        bytes_sent = ewaSecondaryData(context->secondary->handle
                                 ,context
                                 ,(uint8*) context->cptr
                                 ,context->clen);
        if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
          {
            return;
          }
#       ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
        if (context->schedulingState != ewsContextSuspended)
          {
            context->schedulingState = ewsContextScheduled;
          }
#       endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

        if (bytes_sent < 0)
          {
            if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
              {
                ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
              }
            return;
          }
        if (context->clen != 0)
          {
            context->clen -= bytes_sent;
            context->cptr += bytes_sent;
          }
        return;
      }

    /*
     * Handle by substate
     */
    switch(context->substate.forwarding)
      {
        /**********************************************************************
         * Send method.  Note that we have already expanding the method
         * string one character to pick up the space.
         */
        case ewsContextSubstateForwardMethod:
          bytes_sent = ewsStringWrite(context
                                 ,context->secondary->handle
                                 ,ewsStringWriteSecondary
                                 ,&context->req_method_str);
          if (bytes_sent < 0)
            {
              if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                {
                  ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                }
              return;
            }
          if (ewsStringLength(&context->req_method_str) != 0)
            {
              return;
            }

          /*
           * Send the rewritten URL, then transition to write the protocol
           */
          context->cptr = context->url;
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardProtocol;
          return;

        /**********************************************************************
         * Send the protocol
         */
        case ewsContextSubstateForwardProtocol:
#         if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
          context->cptr = (char*)" HTTP/1.1\r\n";
#         else
          context->cptr = (char*)" HTTP/1.0\r\n";
#         endif
          EMWEB_STRLEN(context->clen, context->cptr);

          /*
           * If HTTP/0.9 request, there are no headers to forward.  So, skip
           * to generating our own headers.  Otherwise, forward headers
           * unless there are no headers.
           */
          if (context->req_protocol_maj == 0 ||
              ewsStringBuffer(&context->hdr_str_begin) == EWA_NET_BUFFER_NULL)
            {
              context->substate.forwarding = ewsContextSubstateForwardGenerate;
            }
          else
            {
              context->substate.forwarding = ewsContextSubstateForwardHeaders;
            }
          return;

        /**********************************************************************
         * Forward request headers to secondary
         */
        case ewsContextSubstateForwardHeaders:
          /*
           * If end of current buffer, advance to next buffer
           */
          if (ewsStringAvailable(&context->hdr_str_begin) == 0)
            {
              ewsStringInit(&context->hdr_str_begin
                           ,ewaNetBufferNextGet(
                              ewsStringBuffer(&context->hdr_str_begin)));
            }

          /*
           * Prepare to copy contiguous data up to end of current buffer,
           * or if last buffer, up to end of data
           */
          if (ewsStringBuffer(&context->hdr_str_begin) ==
              ewsStringBuffer(&context->hdr_str_end))
            {
              bytes = context->hdr_str_end.offset -
                      context->hdr_str_begin.offset;
            }
          else
            {
              bytes = ewsStringAvailable(&context->hdr_str_begin);
            }

          /*
           * If at end of current buffer, continue processing with next buffer.
           */
          if (bytes == 0)
            {
              return;
            }

          /*
           * Transmit headers
           */
          bytes_sent = ewaSecondaryData(context->secondary->handle
                                  ,context
                                  ,ewsStringData(&context->hdr_str_begin)
                                  ,bytes);
          if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
            {
              return;
            }
#         ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
          if (context->schedulingState != ewsContextSuspended)
            {
              context->schedulingState = ewsContextScheduled;
            }
#         endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

          if (bytes_sent< 0)
            {
              if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                {
                  ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                }
              return;
            }

          context->hdr_str_begin.offset += bytes_sent;

          /*
           * If more headers to go, return to scheduler for rescheduling
           */
          if (ewsStringData(&context->hdr_str_begin) !=
              ewsStringData(&context->hdr_str_end))
            {
              return;
            }

          /*
           * Generate our own extra headers here...
           */
          context->substate.forwarding = ewsContextSubstateForwardGenerate;

          /* fall through */

        /**********************************************************************
         * Generate our own headers
         *   Via: maj.min primaryhost (EmWeb/R6_2_0)\r\n
         *   {Host: primaryhost\r\n}, if not otherwise specified
         *   \r\n
         */
        case ewsContextSubstateForwardGenerate:
          context->cptr = (char*)"Via: "; context->clen = 5;
          context->substate.forwarding = ewsContextSubstateForwardVia0;
          return;

        case ewsContextSubstateForwardVia0:
          EMWEB_LTOA(context->cptr
                    ,context->req_protocol_maj
                    ,context->cbuf
                    ,sizeof(context->cbuf));
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardVia1;
          return;

        case ewsContextSubstateForwardVia1:
          context->cptr = (char*)"."; context->clen = 1;
          context->substate.forwarding = ewsContextSubstateForwardVia2;
          return;

        case ewsContextSubstateForwardVia2:
          EMWEB_LTOA(context->cptr
                    ,context->req_protocol_min
                    ,context->cbuf
                    ,sizeof(context->cbuf));
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardVia3;
          return;

        case ewsContextSubstateForwardVia3:
          context->cptr = (char*)" ";
          context->clen = 1;
          context->substate.forwarding = ewsContextSubstateForwardVia4;
          return;

        case ewsContextSubstateForwardVia4:
          context->cptr = (char *) ewaNetLocalHostName(context);
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardVia5;
          return;

        case ewsContextSubstateForwardVia5:
          context->cptr = (char*)" (";
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardVia6;
          return;

        case ewsContextSubstateForwardVia6:
          context->cptr = (char*)EmWebId;
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardVia7;
          return;

        case ewsContextSubstateForwardVia7:
          context->cptr = (char*)")";
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardHost0;
          return;

        case ewsContextSubstateForwardHost0:
          /*
           * If Host: header is not present, we must generate one
           */
          if (ewsStringLength(&context->req_host) == 0)
            {
              context->cptr = (char*)"\r\nHost: "; context->clen = 8;
              context->substate.forwarding = ewsContextSubstateForwardHost1;
            }
          /*
           * Otherwise, send header termination
           */
          else
            {
              context->substate.forwarding = ewsContextSubstateForwardCRNL;
            }
          return;

        case ewsContextSubstateForwardHost1:
          context->cptr = (char *) ewaNetLocalHostName(context);
          EMWEB_STRLEN(context->clen, context->cptr);
          context->substate.forwarding = ewsContextSubstateForwardCRNL;
          return;

        case ewsContextSubstateForwardCRNL:
          /*
           * Terminate headers
           */
          context->cptr = (char*)"\r\n\r\n"; context->clen = 4;
          context->req_str_begin = context->req_str_end;
          context->substate.forwarding = ewsContextSubstateForwardBody;
          return;

        /**********************************************************************
         * Forward message body (either primary to secondary or, if
         * is_secondary_context, secondary to primary).
         */
        case ewsContextSubstateForwardBody:
        case ewsContextSubstateForwardResponse:
#         if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
          /*
           * If chunked encoding, handle chunk state if not parsing data
           */
          if (context->chunked_encoding &&
              context->chunk_in_state != ewsContextChunkInStateParseData)
            {
              ewsParseChunk(context);
              return;
            }

          /*
           * Write chunk header/trailer.  If forwarding request body from
           * primary to secondary, we must write data from req_str_begin
           * through req_str_end.  Otherwise, if forwarding response body
           * from secondary to primary, we can skip this processing and
           * simply advance req_str_end over the chunk body.
           */
          if (!context->is_secondary_context
              && ewsStringData(&context->req_str_begin) !=
                 ewsStringData(&context->req_str_end))
            {
              bytes = ewsStringAvailable(&context->req_str_begin);
              if (bytes == 0)
                {
                  ewsStringInit(
                    &context->req_str_begin
                   ,ewaNetBufferNextGet(
                      ewsStringBuffer(&context->req_str_begin)));
                  return;
                }
              if (ewsStringBuffer(&context->req_str_begin) ==
                  ewsStringBuffer(&context->req_str_end))
                {
                  bytes = context->req_str_end.offset -
                          context->req_str_begin.offset;
                }
              bytes_sent = ewaSecondaryData(context->secondary->handle
                                      ,context
                                      ,ewsStringData(&context->req_str_begin)
                                      ,bytes);
              if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                {
                  return;
                }
#             ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
              if (context->schedulingState != ewsContextSuspended)
                {
                  context->schedulingState = ewsContextScheduled;
                }
#             endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

              if (bytes_sent < 0)
                {
                  if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                    {
                      ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                    }
                }
              context->req_str_begin.offset += bytes_sent;
              return;
            }
#         endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

          /*
           * Write remaining data
           */
          if (context->content_remaining != 0)
            {
              /*
               * Determine portion of remaining content available in current
               * buffer.  If end of current buffer, advance to next buffer.
               */
              bytes = ewsStringAvailable(&context->req_str_end);
              if (bytes == 0)
                {
                  EwaNetBuffer nxt = ewaNetBufferNextGet(
                                       ewsStringBuffer(&context->req_str_end));
                  if (nxt == EWA_NET_BUFFER_NULL)
                    {
                      /*
                       * If secondary closed connection, flush data to agent
                       */
                      if (context->secondary->eof)
                        {
                          if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                            {
                              ewsDistFinish(context->secondary, EWS_STATUS_OK);
                            }
                        }

                      /*
                       * Otherwise, wait for more data
                       */
                      else
                        {
                          context->schedulingState = ewsContextDataWait;
                        }
                      return;
                    }
                  ewsStringInit(&context->req_str_end, nxt);
                  context->req_str_begin = context->req_str_end;
                  return;
                }
              if (bytes > context->content_remaining)
                {
                  bytes = context->content_remaining;
                }

              /*
               * If forwarding request body from primary to secondary,
               * transmit data.
               */
              if (!context->is_secondary_context)
                {
                  bytes_sent = ewaSecondaryData(context->secondary->handle
                                          ,context
                                          ,ewsStringData(&context->req_str_end)
                                          ,bytes);
                  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                    {
                      return;
                    }
#                 ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
                  if (context->schedulingState != ewsContextSuspended)
                    {
                      context->schedulingState = ewsContextScheduled;
                    }
#                 endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

                  if (bytes_sent < 0)
                    {
                      if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                        {
                          ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                        }
                      return;
                    }
                  context->req_str_end.offset += bytes_sent;
                  context->req_str_begin = context->req_str_end;
                  context->content_remaining -= bytes_sent;
                  ewsParseReleaseUnusedBuffers(context, FALSE);
                }

              /*
               * If forwarding response chain from secondary to user agent,
               * forward buffers received so far
               */
              else
                {
                  context->req_str_end.offset += bytes;
                  context->req_str_begin = context->req_str_end;
                  context->content_remaining -= bytes;
                  ewsDistForwardAgent(context, TRUE);
                }
              return;
            }

#         if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
          /*
           * If end of chunk, parse next chunk
           */
          if (context->chunked_encoding)
            {
              ewsParseChunk(context);
              return;
            }
#         endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

          /*
           * If forwarding request to secondary, we have completed forwarding
           * of the request.  We must temporarily suspend the request
           * until the response has been processed.
           */
          if (!context->is_secondary_context)
            {
              context->schedulingState = ewsContextSuspended;
            }

          /*
           * If forwarding response from secondary to user agent, complete
           * processing of request and response.
           */
          else
            {
              /*
               * If downgrading to HTTP/1.0 or 0.9, strip chunk trailer
               */
              if (context->secondary->context_current->downgrade)
                {
                  EwaNetBuffer buffer;

                    /*
                     * Set end-of-data to beginning of zero chunk and free
                     * any extra buffers if chunk trailer crossed boundary
                     */

                  context->req_str_end = context->req_str_begin;
                  buffer = (EwaNetBuffer)ewaNetBufferNextGet(ewsStringBuffer(&context->req_str_end));
                  if (buffer != EWA_NET_BUFFER_NULL)
                    {
                      ewaNetBufferFree(buffer);
                      ewaNetBufferNextSet(ewsStringBuffer(&context->req_str_end),
                                                          EWA_NET_BUFFER_NULL);
                      }
                }

              if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                {
                  ewsDistFinish(context->secondary, EWS_STATUS_OK);
                }
            }
          return;

        /*=====================================================================
         * RESPONSE PARSING (per-secondary response_context)
         */

        /**********************************************************************
         * Parse status line from secondary: read protocol version
         */
        case ewsContextSubstateResponse:
          /*
           * Parse secondary's protocol version.
           */
          /*
           * so now we know primary context is forwarding response
           * to the client
           */
          context->secondary->context_current->substate.forwarding =
                    ewsContextSubstateForwardResponse;
          match_c = ewsStringSearch(&context->req_str_begin
                                   ,&context->req_str_end
                                   ," \t\n\r"
                                   ,ewsStringSearchInclude
                                   ,NULL);
          if (match_c == EWS_STRING_EOF)
            {
              if (context->secondary->eof)
                {
                  if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                    {
                      ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                    }
                }
              else
                {
                  context->schedulingState = ewsContextDataWait;
                }
              return;
            }
          /*
           * If premature end-of-line, restart parser.
           */
          if (match_c == '\n' || match_c == '\r')
            {
              match_c = ewsStringGetChar(&context->req_str_end);
              context->req_str_begin.length++;
              (void)ewsStringTrim(&context->req_str_begin, 1);
              return;
            }
          if (!ewsStringCompare(&context->req_str_begin
                               ,"HTTP/"
                               ,ewsStringCompareCaseInsensitive
                               ,NULL)
              || (context->req_protocol = context->req_str_begin, FALSE)
              || !ewsStringGetInteger(&context->req_str_begin
                                     ,10
                                     ,&context->req_protocol_maj)
              || ewsStringGetChar(&context->req_str_begin) != '.'
              || !ewsStringGetInteger(&context->req_str_begin
                                     ,10
                                     ,&context->req_protocol_min))
            {
              if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                {
                  ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                }
              return;
            }
          context->substate.forwarding = ewsContextSubstateResponse1;

          /* fall through */

        /**********************************************************************
         * Parse status line from secondary: Skip LWS to status code
         */
        case ewsContextSubstateResponse1:
          match_c = ewsStringSearch(&context->req_str_begin
                                   ,&context->req_str_end
                                   ," \t\r\n"
                                   ,ewsStringSearchExclude
                                   ,NULL);
          if (match_c == EWS_STRING_EOF)
            {
              if (context->secondary->eof)
                {
                  if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                    {
                      ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                    }
                }
              else
                {
                  context->schedulingState = ewsContextDataWait;
                }
              return;
            }
          context->substate.forwarding = ewsContextSubstateResponse2;
          context->req_str_begin = context->req_str_end;

          /* fall through */

        /**********************************************************************
         * Parse status line from secondary: Read to end of line, get code
         */
        case ewsContextSubstateResponse2:
          match_c = ewsStringSearch(&context->req_str_begin
                                   ,&context->req_str_end
                                   ,"\n"
                                   ,ewsStringSearchInclude
                                   ,NULL);
          if (match_c == EWS_STRING_EOF)
            {
              if (context->secondary->eof)
                {
                  if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                    {
                      ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                    }
                }
              else
                {
                  context->schedulingState = ewsContextDataWait;
                }
              return;
            }
          if (!ewsStringGetInteger(&context->req_str_begin
                                  ,10
                                  ,&context->res_status))
            {
              if (context->secondary != EWS_SECONDARY_HANDLE_NULL)
                {
                  ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
                }
              return;
            }

          /*
           * Enter parser to parse headers
           */
          (void) ewsStringGetChar(&context->req_str_end); /* read NL */
          (void) ewsStringExpand(&context->req_str_end, 1);
          context->req_str_begin = context->req_str_end;
          context->state = ewsContextStateParseHeaders;
          context->substate.header = ewsContextSubstateHaveNL;
          context->req_header = ewsRequestHeaderUnknown;
          return;

        /**********************************************************************
         * Bad state
         */
        default:
#         ifdef EMWEB_SANITY
          EMWEB_ERROR(("ewsDistRun: bad state %d\n"
                      ,context->substate.forwarding));
#         endif /* EMWEB_SANITY */
          break;
      }
}

/*
 * ewsDistForwardAgent
 * Internal: Forward response buffers from secondary context to user agent via
 * primary.  We effectively move network buffers from the secondary context's
 * request list to the primary context's response list.  The more flag
 * indicates that there is more data to be forwarded later.
 */
void
ewsDistForwardAgent ( EwsContext scontext, boolean more )
{
  EwsSecondaryHandle secondary = scontext->secondary;
  EwsContext pcontext = secondary->context_current;
  EwaNetBuffer last;
  EwaNetBuffer buffer;

    /*
     * If end of data, attach remaining response buffers to primary
     * context response queue and flush.
     */
    if (!more)
      {
        if (pcontext->res_buf_next == EWA_NET_BUFFER_NULL)
          {
            pcontext->res_buf_next = scontext->req_buf_first;
          }
        else
          {
            ewaNetBufferNextSet(ewsStringBuffer(&pcontext->res_str_end)
                               ,scontext->req_buf_first);
          }
        pcontext->res_str_end = scontext->req_str_end;
        scontext->req_buf_first = scontext->req_buf_last = EWA_NET_BUFFER_NULL;
        if (ewsFlushAll(pcontext) != EWA_STATUS_OK)
          {
            ewsNetHTTPAbort(pcontext);
          }
        return;
      }

    /*
     * If there are no buffers before the current buffer, we can't forward
     * anything yet.
     */
    last = ewsStringBuffer(&scontext->req_str_end);
    if (last == scontext->req_buf_first)
      {
        return;
      }

    /*
     * Find previous buffer
     */
    for (buffer = scontext->req_buf_first;
         ewaNetBufferNextGet(buffer) != last;
         buffer = ewaNetBufferNextGet(buffer))
      ;

    /*
     * Unlink previous buffers from current buffers, append previous
     * buffers to response chain for forwarding.
     */
    ewaNetBufferNextSet(buffer, EWA_NET_BUFFER_NULL);
    if (pcontext->res_buf_next == EWA_NET_BUFFER_NULL)
      {
        pcontext->res_buf_next = scontext->req_buf_first;
      }
    else
      {
        ewaNetBufferNextSet(ewsStringBuffer(&pcontext->res_str_end),
                            scontext->req_buf_first);
      }
    scontext->req_buf_first = last;
    ewsStringInit(&pcontext->res_str_end, buffer);
    pcontext->res_str_end.offset = ewaNetBufferLengthGet(buffer);
    if (ewsFlushAll(pcontext) != EWA_STATUS_OK)
      {
        ewsNetHTTPAbort(pcontext);
      }
}

/*
 * ewsDistFinish
 * Internal: Terminate processing of current request on secondary.  Status
 * indicates EWS_STATUS_OK for normal termination, EWS_STATUS_ABORTED for
 * abort status
 */
void
ewsDistFinish(EwsSecondaryHandle secondary, EwsStatus status)
{
  EwsContext scontext;
  EwsContext pcontext;

    pcontext = secondary->context_current;
    scontext = secondary->response_context;

    /*
     * If successful completion of forwarded request
     */
    if (status == EWS_STATUS_OK)
      {
        /*
         * Flush remaining response data to the user agent
         */

        ewsDistForwardAgent(scontext, FALSE);
      }
    /*
     * error
     */
    else
      {
        /*
         * Release buffers retained for request headers
         */
        if (    (scontext->is_secondary_context)
             && (scontext->req_buf_first != EWA_NET_BUFFER_NULL)
           )
          {
            ewaNetBufferFree(scontext->req_buf_first);

            scontext->req_buf_first = scontext->req_buf_last =
                                                     EWA_NET_BUFFER_NULL;
          }
      }


    /*
     * if EWS_STATUS_ABORTED:
     *       send internal error response to the client if we still can
     *    or abort primary context if we are already forwarding response
     *       to our client
     */
    if (status == EWS_STATUS_ABORTED)
      {
        if (pcontext->substate.forwarding == ewsContextSubstateForwardResponse)
          {
            /*
             * in this case, abort response
             */
            ewsNetHTTPAbort(pcontext);
          }
        else
           {
             ewsSendHeaderAndEnd(pcontext
                                ,ews_http_status_500
                                ,ews_http_string_500);
           }
        pcontext->secondary = EWS_SECONDARY_HANDLE_NULL;
      }

    /*
     * If we had previously internally suspended the primary context
     * while waiting for the secondary response to complete, we must
     * now resume the primary context so that the next request on this
     * connection may be processed.
     */
    if (pcontext->schedulingState == ewsContextSuspended)
      {
        EWS_LINK_DELETE(&pcontext->link)
        pcontext->schedulingState = ewsContextScheduled;
        if ( pcontext->flow_control == ewsContextFlowControlPending )
          {
            /*
             * Primary is flow controlled, so this secondary has been too.
             * Before we disconnect it, release the flow control on the
             * seconary side - it no longer cares what the primary flow
             * control state is.
             */
            ewaSecondaryUnFlowControl( scontext->net_handle );
          }
#       ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
        pcontext->resumePending = FALSE;
#       endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

        EWA_TASK_LOCK();
        EWS_LINK_INSERT(&ews_state->context_serve_list, &pcontext->link)
        EWA_TASK_UNLOCK();

        /*
         * Complete processing of current request
         */
        ewsInternalFinish(pcontext);
      }

    /*
     * If no more requests waiting for this secondary, idle
     */
    if (EWS_LINK_IS_EMPTY(&secondary->context_list) || !secondary->opened)
      {
        secondary->context_current = EWS_CONTEXT_NULL;
      }

    /*
     * Otherwise, dequeue next request and schedule
     */
    else
      {
        pcontext = (EwsContext) secondary->context_list.next;
        secondary->context_current = pcontext;
        EWS_LINK_DELETE(&pcontext->link);
        EWA_TASK_LOCK();
        EWS_LINK_INSERT(&ews_state->context_serve_list, &pcontext->link);
        EWA_TASK_UNLOCK();
      }

    /*
     * Reset secondary response context for next request
     */

    ewsInternalCleanup(scontext);
    ewsInternalSetup(scontext);
    scontext->is_secondary_context = TRUE;
    scontext->state = ewsContextStateForwarding;
    scontext->substate.forwarding = ewsContextSubstateResponse;
    scontext->schedulingState = ewsContextDataWait;
    scontext->secondary = secondary;
    if (status == EWS_STATUS_ABORTED)
      {
        /*
         * if response context is on server's serv list, get it off it
         * BUT ONLY if it is not currently processed and
         * make sure to put it on wait list
         */
        if (!IS_CONTEXT_CURRENT(scontext))
          {
            EWA_TASK_LOCK();
            EWS_LINK_DELETE(&scontext->link);
            EWS_LINK_INSERT(&ews_state->context_wait_list, &scontext->link);
            EWA_TASK_UNLOCK();
          }
      }
    secondary->eof = FALSE;
}

/*
 * ewsDistShutdown
 * Called by ewsShutdown() to cleanup resources allocated for primary/secondary
 * processing
 */
void
ewsDistShutdown()
{
  EwsSecondaryHandle secondary;
  EwsContext context;

    while (!EWS_LINK_IS_EMPTY(&ews_state->secondary_list))
      {
        secondary = (EwsSecondaryHandle) ews_state->secondary_list.next;
        while (!EWS_LINK_IS_EMPTY(&secondary->context_list))
          {
            context = (EwsContext) secondary->context_list.next;
            ewsInternalAbort(context);
          }
        if (secondary->opened)
          {
            ewsSecondaryClosed(secondary);
          }
        ewsSecondaryUnregister(secondary);
      }
}
/* END-STRIP-EMWEB-LIGHT */
#endif /* EW_CONFIG_OPTION_PRIMARY */

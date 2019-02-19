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
 * EmWeb/Server System Interfaces
 */

#include "ews.h"
#include "ews_sys.h"
#include "ews_serv.h"
#include "ews_secu.h"
#include "ews_send.h"
#include "ews_gway.h"
#include "ews_xml.h"
#include "ews_pars.h"

#ifdef EW_CONFIG_OPTION_UPNP
#include "../upnp/ews_upnp_i.h"
#endif /* EW_CONFIG_OPTION_UPNP */

#ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
#include "ews_urlhooks.h"
#endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

#include "session.h"
/* ================================================================
 * Static Function Declarations
 * ================================================================ */

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
static void ewsResumeReadyContexts( void );     /* KAGTODO */
#endif


/* ================================================================
 * External Interfaces
 * ================================================================ */

#ifndef ews_state
EwsStateP ews_state;        /* the one global context manager */
#endif

const char *EmWebId =
#ifdef EW_CONFIG_OPTION_UPNP
        EMWEB_OS " UPnP/1.0 "
#endif /* EW_CONFIG_OPTION_UPNP */
        EMWEB_ID;

/* Used as constant pointers to compare against default value in field
 * table to determine if field is used as input or output in UPnP Soap action
 * form
 */
uint32 ew_upnp_field_output;
uint32 ew_upnp_field_input;

/******************************************************************************
 *
 * INITIALIZATION AND SHUTDOWN
 *
 * The EmWeb/Server must be initialized at start-up before accepting requests,
 * installing document archives, or registering authentication information.
 *
 *****************************************************************************/

/*
 * ewsInit
 * Initialize EmWeb/Server
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
EwsStatus
ewsInit ( void )
{
  EwsStateP *espp = &ews_state; /* indirection for macro override */
    *espp = (EwsStateP) ewaAlloc(sizeof(EwsState));

    if (ews_state == (EwsStateP) NULL)
      {
        return EWS_STATUS_NO_RESOURCES;
      }

    ewsFileHashInit();

#   ifdef EW_CONFIG_OPTION_SCHED
    EWS_LINK_INIT(&ews_state->context_wait_list);
    EWS_LINK_INIT(&ews_state->context_serve_list);
    ews_state->context_serve_next = &ews_state->context_serve_list;
    ews_state->context_current = EWS_CONTEXT_NULL;
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    ews_state->contextResumed = FALSE;         /* true if a context is resumed KAGTODO */
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
#   endif /* EW_CONFIG_OPTION_SCHED */

    ews_state->req_header_table = ews_request_header_table;
#   if defined(EW_CONFIG_OPTION_FORM) || defined(EW_CONFIG_OPTION_STRING_TYPED)
    ews_state->form_field_table = ewFormFieldTable;
#   endif /* EW_CONFIG_OPTION_FORM | STRING_TYPED */
#   ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
    ews_state->form_data_header_table = ews_form_data_header_table;
#   endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#   ifdef EW_CONFIG_OPTION_PRIMARY
    EWS_LINK_INIT(&ews_state->secondary_list);
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   ifdef EW_CONFIG_OPTION_AUTH
    if (!ewsAuthInit())
      {
        return EWS_STATUS_NO_RESOURCES;
      }
#   endif /* EW_CONFIG_OPTION_AUTH */

#   ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
    EWS_LINK_INIT(&ews_state->url_hook_list);
#   endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

#   ifdef EW_CONFIG_OPTION_SNMP_AGENT
    ews_state->snmpAgent = ewaSNMPAgentInit();
#   endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
    EWS_LINK_INIT(&ews_state->file_system_list);
#   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

#   ifdef EW_CONFIG_OPTION_NS_LOOKUP
    ewsSchemaHashInit();
#   endif /* EW_CONFIG_OPTION_NS_LOOKUP */

#   ifdef EW_CONFIG_OPTION_CLIENT
    ewsClientInit();
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   if defined(EW_CONFIG_OPTION_CLI) 
    ewsCliInit();
#   endif /* EW_CONFIG_OPTION_CLI */

#   ifdef EW_CONFIG_OPTION_UPNP
    ewuUPnPInit();
#   endif /* EW_CONFIG_OPTION_UPNP */

    EwaSessionInit();

    return EWS_STATUS_OK;
}

#ifdef EW_CONFIG_OPTION_CLEANUP
/*
 * ewsShutdown
 * Graceful shutdown of EmWeb/Server terminating all requests in progress
 * and releasing all memory and buffers.
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
EwsStatus
ewsShutdown ( void )
{
#   ifdef EW_CONFIG_OPTION_UPNP
    ewuUPnPShutdown();
#   endif /* EW_CONFIG_OPTION_UPNP */

#   ifdef EW_CONFIG_OPTION_PRIMARY
    ewsDistShutdown();
#   endif /* EW_CONFIG_OPTION_PRIMARY */

#   ifdef EW_CONFIG_OPTION_SCHED
    while (&ews_state->context_wait_list != ews_state->context_wait_list.next)
      {
        ewsInternalAbort((EwsContext) ews_state->context_wait_list.next);
      }
    while (&ews_state->context_serve_list != ews_state->context_serve_list.next)
      {
        ewsInternalAbort((EwsContext) ews_state->context_serve_list.next);
      }
#   endif /* EW_CONFIG_OPTION_SCHED */

    ewsFileHashShutdown();

#   ifdef EW_CONFIG_OPTION_AUTH
    ewsAuthShutdown();
#   endif /* EW_CONFIG_OPTION_AUTH */

#   ifdef EW_CONFIG_OPTION_SNMP_AGENT
    ewaSNMPAgentShutdown( ews_state->snmpAgent );
#   endif /* EW_CONFIG_OPTION_SNMP_AGENT */


#   ifdef EW_CONFIG_OPTION_NS_LOOKUP
    ewsSchemaShutdown( NULL );
#   endif /* EW_CONFIG_OPTION_NS_LOOKUP */

#   ifdef EW_CONFIG_OPTION_CLIENT
    ewsClientShutdown( );
#   endif /* EW_CONFIG_OPTION_CLIENT */

#   if defined(EW_CONFIG_OPTION_CLI) 
    ewsCliShutdown( );
#   endif /* EW_CONFIG_OPTION_CLI */

#   ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
    ewsUrlHookShutdown( );
#   endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

    ewaFree(ews_state);

    return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_CLEANUP */

#ifdef EW_CONFIG_OPTION_SCHED
/******************************************************************************
 *
 * SCHEDULING
 *
 * The EmWeb/Server is capable of both multi-threading HTTP requests and
 * yielding execution control to the CPU.  The following functions are provided
 * to give the application control over how EmWeb/Server schedules  pending
 * requests.
 *
 *****************************************************************************/

/*
 * ewsRun
 * Reschedule request processing after control returned to application as a
 * result of returning EWA_STATUS_OK_YIELD from ewaNetHTTPSend(),
 * ewaNetHTTPEnd(), or ewaDocumentFault(), or as the value to the status
 * parameter in ewsResume().
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
EwsStatus
ewsRun ( void )
{
  EwaStatus status;
  EwsContext current;

    /*
     * While there are requests scheduled to run
     */
    EWA_TASK_LOCK();

/*
 * Loop if contexts resumed by other contexts
 */
do
  {
#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    if (ews_state->contextResumed) /* if context(s) were resumed */
      {
        EWA_TASK_UNLOCK();
        ewsResumeReadyContexts();   /* put them on the ready list */
        EWA_TASK_LOCK();
      }
#endif

#   ifdef EW_CONFIG_OPTION_UPNP
    EWA_TASK_UNLOCK();
    ewsGenaRun(); /* give GENA a chance to start notices */
    EWA_TASK_LOCK();
#   endif /* EW_CONFIG_OPTION_UPNP */

    /*
     * Schedule requests
     */
    while (&ews_state->context_serve_list != ews_state->context_serve_list.next)
      {

        /*
         * If next context wrapped beyond list (or wasn't yet initialized),
         * set it to the beginning of the list.
         */
        if (ews_state->context_serve_next == &ews_state->context_serve_list)
          {
            ews_state->context_serve_next = ews_state->context_serve_list.next;
          }

        /*
         * Schedule the next context
         */
        ews_state->context_current = (EwsContext) ews_state->context_serve_next;
        current = ews_state->context_current;

        /*
         * Schedule the request
         */

#       ifdef EW_CONFIG_OPTION_SCHED_FC
        /*
         * If flow control requested for this context, move to wait list
         * and mark as flow controlled.  Then, move on to next request if any.
         */
        if (current->flow_control == ewsContextFlowControlPending)
          {
            ews_state->context_serve_next = current->link.next;
            EWS_LINK_DELETE(&current->link);
            EWS_LINK_INSERT(&ews_state->context_wait_list, &current->link);
            current->flow_control = ewsContextFlowControlOn;
            continue;
          }
#       endif /* EW_CONFIG_OPTION_SCHED_FC */

        EWA_TASK_UNLOCK();

#       if defined(EW_CONFIG_OPTION_CLIENT) && \
           defined(EW_CONFIG_OPTION_CLIENT_CACHE)
        /*
         * Check to see if data sourced from file instead of network.
         * If so, allocate a network buffer, read from file, and pass
         * it along.  The parser will either be PUTing to another file,
         * or otherwise processing data such as form submissions, etc.
         * If end of file is reached, set content_remaining to zero to
         * signal end-of-data.  In any case, continue processing with
         * state machine as normal.
         */
        if (current->parse_from_file)
          {
            EwaNetBuffer buf = EWA_NET_BUFFER_NULL;
            sintf len;

            /*
             * Only read from file if at end of chain
             */
            if (   current->req_buf_first == EWA_NET_BUFFER_NULL
                || (   ewsStringAvailable(&current->req_str_end) == 0
                    && ewsStringBuffer(&current->req_str_end) == current->req_buf_last
                   )
               )
              {


                buf = ewaNetBufferAlloc();
                if (buf != EWA_NET_BUFFER_NULL)
                  {
#                   ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                    len = current->source_fs->fs.fileRead
#                   else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                    len = ewaFileRead
#                   endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                      (
                        current
                       ,current->source_file
                       ,ewaNetBufferDataGet(buf)
                       ,ewaNetBufferLengthGet(buf)
                      );
                    if (len > 0)
                      {
                        ewaNetBufferLengthSet(buf, len);
                        ewsNetHTTPReceive(current, buf);
                      }
                    else if (len == 0)
                      {
                        ewaNetBufferFree(buf);
                        current->content_remaining = 0;
#                       ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                        current->source_fs->fs.fileClose
                          ( current->source_file, EWS_STATUS_OK );
                        (current->source_fs->use_cnt)--;
#                       else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                        ewaFileClose
                          ( current->source_file, EWS_STATUS_OK );
#                       endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                        current->parse_from_file = FALSE;
                      }
                    else
                      {
                        ewaNetBufferFree(buf);
                        ewsNetHTTPAbort(current);
                      }
                  }
                else
                  {
                    ewsNetHTTPAbort(current);
                  }
              }
          }
#       endif /* EW_CONFIG_OPTION_CLIENT && CLIENT_CACHE */

        /*
         * now run the state machine for the context's given state
         */
        status = EWA_STATUS_OK;     /* different return types!! */
        switch (current->state)
          {
          case ewsContextStateParseRequest:
          case ewsContextStateParseHeaders:
#         ifdef EW_CONFIG_OPTION_FORM
          case ewsContextStateParsingForm:
#         ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
          case ewsContextStateParsingMPForm:
#         endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */
#         endif /* EW_CONFIG_OPTION_FORM */
#         ifdef EW_CONFIG_OPTION_FILE_PUT
          case ewsContextStateParsingPut:
#         endif /* EW_CONFIG_OPTION_FILE_PUT */
#         ifdef EW_CONFIG_OPTION_PERSISTENT
          case ewsContextStateSkipContent:
#         endif /* EW_CONFIG_OPTION_PERSISTENT */
#         ifdef EW_CONFIG_OPTION_SMTP
          case ewsContextStateSmtp:
#         endif /* EW_CONFIG_OPTION_SMTP */
#         ifdef EW_CONFIG_OPTION_TELNET
          case ewsContextStateTelnet:
#         endif /* EW_CONFIG_OPTION_TELNET */
#         ifdef EW_CONFIG_OPTION_CLIENT
          case ewsContextStateClient:
#         endif /* EW_CONFIG_OPTION_CLIENT */
#         ifdef EMWEB_XMLP
          case ewsContextStateXMLP:
#         endif /* EMWEB_XMLP */
#         ifdef EW_CONFIG_OPTION_UPNP
          case ewsContextStateSSDPMsearchResponse:
#         endif /* EW_CONFIG_OPTION_UPNP */

            while (ewsParse( current ))
              ;

            /* Here is where we test to see if we have reached the known
             * end of the input, and handle the situation appropriately.
             * This ensures that the context doesn't hang indefinitely
             * waiting for input that will never come.
             * Currently, this feature is not used with ordinary HTTP,
             * only HTTP/UDP, so it is conditionalized on the UPnP feature.
             * Also, do not invoke any of these actions if the context has
             * already been marked to abort.
             */
#ifdef EW_CONFIG_OPTION_UPNP
            if (   0 != (current->flags & EWS_CONTEXT_FLAGS_INPUT_EOF)
                && 0 == (current->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
                && ewsContextDataWait == current->schedulingState
                )
              {
                /* Parser wants more input but the source has signalled
                 * EOF, so no more input will ever arrive. */
                if (!ewsParseAwaitingRequest(current))
                  {
                    /* The parser is in the middle of a request.
                     * So we should force the request to deliver an
                     * appropriate error response, "400 Bad Request".
                     *
                     * This test might not work in some tricky cases where
                     * we are parsing the body of the request, and the code
                     * to perform the method has already generated some or all
                     * of the response.  In that case, the request processing
                     * code has to itself check for EWS_CONTEXT_FLAGS_INPUT_EOF
                     * and do the right thing (which will be method-specific).
                     */
                    EMWEB_TRACE(("ewsRun: Parsing reached EOF with request "
                                 "partially complete, context = %p\n",
                                 current));
                    /* Send response "400 Bad Request". */
                    ewsSendHeaderAndEnd(current
                                        ,ews_http_status_400
                                        ,ews_http_string_400);
                    /* After sending the response, the server state machine
                     * will return to the parse-request-line state, and the
                     * following case(s) will abort the context.
                     */
                  }
                else
                  {
                    /* The parser is awaiting a new request.
                     * We can safely order the context to be aborted.
                     */
                    EMWEB_TRACE(("ewsRun: Parsing reached EOF while awaiting "
                                 "request, context = %p\n",
                                 current));
                    /* ewsNetHTTPAbort does not flush output that hasn't
                     * been sent to the application. */
                    ewsFlushAll(current);
                    ewsNetHTTPAbort(current);
                  }
              }
#endif /* EW_CONFIG_OPTION_UPNP */
            break;

          case ewsContextStateFindDocument:
            ewsServeFindDocument( current );
            status = EWA_STATUS_OK;
            break;

          case ewsContextStateServeSetup:
            while (ewsServeSetup( current
                                  ,current->document))
              /* empty */ ;
            break;

#         ifdef EW_CONFIG_OPTION_CGI
          case ewsContextStateServingCGI:
            ewsServeCGIData( current );
            break;
#         endif

          case ewsContextStateServingDocument:
#         ifdef EW_CONFIG_OPTION_FORM
          case ewsContextStateServingForm:
#         endif
#         ifdef EW_CONFIG_OPTION_STRING
          case ewsContextStateString:
#         endif
#         ifdef EW_CONFIG_OPTION_INCLUDE
          case ewsContextStateInclude:
#         endif
#         ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
          case ewsContextStateDynamicSelect:
#         endif
#         ifdef EW_CONFIG_OPTION_SOAP
          case ewsContextStateServingSOAPResponse:
#         endif
            status = ewsServeDocument( current );
            break;

#         ifdef EW_CONFIG_OPTION_FORM
          /*
           * note: ewsServeSubmitForm aborts the context
           * before returning!!
           */
          case ewsContextStateSubmittingForm:
            ewsServeSubmitForm(current);
            break;
#         endif /* EW_CONFIG_OPTION_FORM */

#         ifdef EW_CONFIG_OPTION_PRIMARY
          case ewsContextStateForwarding:
            ewsDistRun(current);
            break;
#         endif /* EW_CONFIG_OPTION_PRIMARY */

#         ifdef EW_CONFIG_OPTION_UPNP
          case ewsContextStateDoMSearch:
            ewuSsdpSearch(current);
            break;
          case ewsContextStateNotify:
          case ewsContextStateReNotify:
            ewuSsdpNotify(current);
            break;
#         ifdef EW_CONFIG_OPTION_SOAP
          case  ewsContextStateSOAPAction:
          case  ewsContextStateSOAPQuery:
            ewuSoapRequestProcessing(current);
            break;
#         endif /* EW_CONFIG_OPTION_SOAP */
#         endif /* EW_CONFIG_OPTION_UPNP */

#         ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
          case ewsContextStateSsdpCPSearchResourceDB:
            ewuSsdpCPSearchResourceDB(current);
            break;
#         endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */

          default:
#         ifdef EMWEB_SANITY
            status = EWA_STATUS_ERROR;
            EMWEB_WARN(("ewsRun: bad context state: 0x%X\n"
                        ,current->state
                        )
                       );
#         endif
            break;
        }

#       ifdef EW_CONFIG_OPTION_PERSISTENT
        /*
         * Special case:  A persistent connection may have just finished
         * one request and is about to begin a new request.  It is possible
         * that the last flush resulted in the application requesting that
         * EmWeb/Server yield the CPU.  However, in most cases, the state
         * will have been lost (for non-persistent connections, the context
         * would have been aborted by now).  So, check yield flag set by
         * ewsInternalFinish() for possible yield and update status
         * accordingly.
         */
        if (status == EWA_STATUS_OK && current->finish_yield)
          {
            status = EWA_STATUS_OK_YIELD;
          }
#       endif /* EW_CONFIG_OPTION_PERSISTENT */

        EWA_TASK_LOCK();

#       ifdef EW_CONFIG_OPTION_PRIMARY
        /*
         * We have a special case here:  If the context was moved to
         * the secondary list, then we have to reset the next
         * context to the start of list and re-run the scheduler.
         */
        if (current->state == ewsContextStateForwardingQueued)
          {
            current->state = ewsContextStateForwarding;
            ews_state->context_serve_next = &ews_state->context_serve_list;
            if (current->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
              {
                EWA_TASK_UNLOCK();
                ewsInternalAbort(current);
                EWA_TASK_LOCK();
              }
            continue;
          }
#       endif /* EW_CONFIG_OPTION_PRIMARY */

#       ifdef EW_CONFIG_OPTION_DEMAND_LOADING
        /*
         * We have a special case here:  If the context was moved to
         * the document fault list, then we have to reset the next
         * context to the start of list and re-run the scheduler.
         */
        if (current->state == ewsContextStateDocumentFault)
          {
            ews_state->context_serve_next = &ews_state->context_serve_list;
            if (current->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
              {
                EWA_TASK_UNLOCK();
                ewsInternalAbort(current);
                EWA_TASK_LOCK();
              }
            continue;
          }
        else
#       endif /* EW_CONFIG_OPTION_DEMAND_LOADING */
#       ifdef EW_CONFIG_OPTION_CLIENT
        /*
         * We have a special case here:  If the context was moved to
         * EmWeb/Client's per-host idle list, then we have to reset the next
         * context to the start of list and re-run the scheduler.
         */
        if (current->state == ewsContextStateClientIdle)
          {
            ews_state->context_serve_next = &ews_state->context_serve_list;
            if (current->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
              {
                EWA_TASK_UNLOCK();
                ewsInternalAbort(current);
                EWA_TASK_LOCK();
              }
            continue;
          }
        else
#       endif /* EW_CONFIG_OPTION_CLIENT */
          ews_state->context_serve_next = current->link.next;

#       if defined(EW_CONFIG_OPTION_CLIENT) && \
           defined(EW_CONFIG_OPTION_CLIENT_CACHE)
        /*
         * If we are parsing from file and entered data wait state,
         * exit from data wait because more data needs to be read.
         */
        if (   current->parse_from_file
            && current->schedulingState == ewsContextDataWait
           )
          {
            current->schedulingState = ewsContextScheduled;
          }
#       endif /* EW_CONFIG_OPTION_CLIENT && CACHE */

        /*
         * If current context was aborted by application, abort it now.
         *
         * NOTE: Abortion takes higher precedence than scheduling state!
         * In other words, abortion must be checked and acted upon
         * before processing scheduling state.
         *
         * NOTE: We used to check for ewsContextStateAbort, but this
         * doesn't really work because other processing can overwrite the
         * state before we discover it.  So, we now use flags.
         */
        if (   status == EWA_STATUS_ERROR
            || (current->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
            )
          {
            EwsContext  tmp;

            tmp = current;

            /*
             * bugfix: cannot hold task lock when calling internal
             * abort. Note that ews_state->context_current is
             * undefined after this call  (don't reference it again).
             */
            EWA_TASK_UNLOCK();
            if (ewsInternalAbort(tmp) == EWA_STATUS_OK_YIELD)
              {
                status = EWA_STATUS_OK_YIELD;
              }
            EWA_TASK_LOCK();
          }

        /*
         * If current context was suspended by application,
         * or it needs to receive more data before it can proceed,
         * then move to wait list.
         */
        else if (  (current->schedulingState == ewsContextDataWait)
#                ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
                 ||
                   (   (   current->schedulingState == ewsContextSuspended )
                    && !(current->resumePending)
                   )
#                endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
                )
          {
            EWS_LINK_DELETE(&current->link);
            EWS_LINK_INSERT(&ews_state->context_wait_list, &current->link);

#           ifdef EW_CONFIG_OPTION_SCHED_FC
            /*
             * If marked for pending flow control and we just moved to the
             * wait list, advance flow control state to on.
             */
            if (current->flow_control == ewsContextFlowControlPending)
              {
                current->flow_control = ewsContextFlowControlOn;
              }
#           endif /* EW_CONFIG_OPTION_SCHED_FC */
          }
#       ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
        else if (current->resumePending)
          {
#           ifdef EMWEB_SANITY
            if (   current->schedulingState != ewsContextSuspended )
              {
                EMWEB_ERROR(("ewsRun: Bad State - should be suspended: 0x%X\n"
                             ,current->schedulingState
                           ));
              }
#           endif
            /*
             * The context was suspended and immediately resumed.
             * Just set state to "resuming" and process
             * it during this call to ewsRun.
             */
            current->resumePending = FALSE;
            current->schedulingState = ewsContextResuming;
          }
#       endif       /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

        /*
         * If application requested that we yield the CPU, then return
         * to the application
         */
        if (status == EWA_STATUS_OK_YIELD)
          {
            break;
          }
#       ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
        /*
         * Check for resumed tasks one more time
         */
        if (ews_state->contextResumed)  /* if context(s) were ewsResumed, */
          {
            EWA_TASK_UNLOCK();
            ewsResumeReadyContexts();   /* put them on the ready list */
            EWA_TASK_LOCK();
          }
#       endif
      }

#   ifdef EW_CONFIG_OPTION_UPNP
    EWA_TASK_UNLOCK();
    ewsGenaRun(); /* give GENA a chance to start notices */
    EWA_TASK_LOCK();
#   endif /* EW_CONFIG_OPTION_UPNP */

  } while (
#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
      ews_state->contextResumed /* Reschedule if resumed tasks */
#else
      0
#endif
      );

    /*
     * Indicate nothing currently running to avoid race condition with
     * ewsNetHTTPAbort().
     */
    ews_state->context_current = EWS_CONTEXT_NULL;

    EWA_TASK_UNLOCK();
    return EWS_STATUS_OK;
}

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES

/*
 * ewsSuspend
 * Suspend processing of the current request during an EmWeb callout function
 * (e.g. EmWebString, EmWebFormGet, or EmWebFormSubmit).  This function is
 * used in the implementation of proxies.
 *
 * context      - context of request to be suspended
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
EwsStatus
ewsSuspend ( EwsContext context )
{

    EMWEB_TRACE(("ewsSuspend(%p)\n", context));

#   ifdef EMWEB_SANITY
    /*
     * Check state
     */
    if (context->schedulingState == ewsContextSuspended)
      {
       EMWEB_WARN(("ewsSuspend: bad scheduling state\n"));
        return EWS_STATUS_BAD_STATE;
      }
    if ( 1
#       ifdef EW_CONFIG_OPTION_INCLUDE
        && context->state != ewsContextStateInclude
#       endif /* EW_CONFIG_OPTION_INCLUDE */

#       ifdef EW_CONFIG_OPTION_STRING
        && context->state != ewsContextStateString
#       endif /* EW_CONFIG_OPTION_STRING */

#       ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
        && context->state != ewsContextStateDynamicSelect
#       endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC  */

        && (   context->state != ewsContextStateServeSetup
#           ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
            || ( context->substate.setup == ewsContextServeSetupDynEtag )
#           endif
            )
        && context->state != ewsContextStateServingDocument
#       ifdef EW_CONFIG_OPTION_SOAP
        && context->state != ewsContextStateServingSOAPResponse
        && context->state != ewsContextStateSOAPAction
        && context->state != ewsContextStateSOAPQuery
#       endif /* EW_CONFIG_OPTION_SOAP */

#       ifdef EW_CONFIG_OPTION_URL_HOOK
        && context->state != ewsContextStateFindDocument
#       endif /* EW_CONFIG_OPTION_URL_HOOK */

#       ifdef EW_CONFIG_OPTION_FORM
        && context->state != ewsContextStateSubmittingForm
        && context->state != ewsContextStateServingForm
#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
        && context->state != ewsContextStateParsingMPForm
#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_FILE_PUT
        && context->state != ewsContextStateParsingPut
#       endif /* EW_CONFIG_OPTION_FILE_PUT */

#       ifdef EW_CONFIG_OPTION_PRIMARY
        && context->state != ewsContextStateForwarding
#       endif /* EW_CONFIG_OPTION_PRIMARY */

#       ifdef EW_CONFIG_OPTION_TELNET
        && context->state != ewsContextStateTelnet
#       endif /* EW_CONFIG_OPTION_TELNET */

#       ifdef EW_CONFIG_OPTION_CLIENT
        && context->state != ewsContextStateClient
#       endif /* EW_CONFIG_OPTION_CLIENT */

#       ifdef EW_CONFIG_OPTION_UPNP
        && !((  (context->state == ewsContextStateDoMSearch)
              &&(context->ssdp_connection == TRUE)))
        && !((  (context->state == ewsContextStateNotify)
              &&(context->ssdp_connection == TRUE)))
        && !((  (context->state == ewsContextStateReNotify)
              &&(context->ssdp_connection == TRUE)))
#       endif /* EW_CONFIG_OPTION_UPNP */

#       ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
        && context->state != ewsContextStateSsdpCPSearchResourceDB
#       endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */

#       ifdef EMWEB_XMLP
        && context->state != ewsContextStateXMLP
#       endif /* EMWEB_XMLP */
       )
     {
       EMWEB_WARN(("ewsSuspend: bad state\n"));
       return EWS_STATUS_BAD_STATE;
     }

#   endif /* EMWEB_SANITY */

    /*
     * Mark context as suspended.  Scheduler will take care of the
     * rest.
     */
    context->schedulingState = ewsContextSuspended;
#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    context->resumePending = FALSE;
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

    return EWS_STATUS_OK;
}

/*
 * ewsResume
 * Resume processing of a request previously suspended by ewsSuspend().  This
 * causes the request to be rescheduled, and the callout from which ewsSuspend
 * had been called will be reinvoked.
 *
 * context      - Context of suspended request to be resumed
 * status       - EWA_STATUS_OK or EWA_STATUS_OK_YIELD.
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
EwsStatus
ewsResume ( EwsContext context, EwaStatus status )
{

    EMWEB_TRACE(("ewsResume(%p, %d)\n", context, status));
    EW_UNUSED(status);

#   ifdef EMWEB_SANITY
    /*
     * Check state
     */
    if (context->schedulingState != ewsContextSuspended)
      {
       EMWEB_WARN(("ewsResume: bad scheduling state\n"));
        return EWS_STATUS_BAD_STATE;
      }
    if (1
#       ifdef EW_CONFIG_OPTION_INCLUDE
        && context->state != ewsContextStateInclude
#       endif /* EW_CONFIG_OPTION_INCLUDE */

#       ifdef EW_CONFIG_OPTION_STRING
        && context->state != ewsContextStateString
#       endif /* EW_CONFIG_OPTION_STRING */

#       ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
        && context->state != ewsContextStateDynamicSelect
#       endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

        && context->state != ewsContextStateServeSetup
        && context->state != ewsContextStateServingDocument
#       ifdef EW_CONFIG_OPTION_SOAP
        && context->state != ewsContextStateServingSOAPResponse
        && context->state != ewsContextStateSOAPAction
        && context->state != ewsContextStateSOAPQuery
#       endif /* EW_CONFIG_OPTION_SOAP */


#       ifdef EW_CONFIG_OPTION_URL_HOOK
        && context->state != ewsContextStateFindDocument
#       endif /* EW_CONFIG_OPTION_URL_HOOK */

#       ifdef EW_CONFIG_OPTION_FORM
        && context->state != ewsContextStateSubmittingForm
        && context->state != ewsContextStateServingForm
#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
        && context->state != ewsContextStateParsingMPForm
#       endif /* EW_CONFIG_OPTION_FORM */

#       ifdef EW_CONFIG_OPTION_FILE_PUT
        && context->state != ewsContextStateParsingPut
#       endif /* EW_CONFIG_OPTION_FILE_PUT */

#       ifdef EW_CONFIG_OPTION_PRIMARY
        && context->state != ewsContextStateForwarding
#       endif /* EW_CONFIG_OPTION_PRIMARY */

#       ifdef EW_CONFIG_OPTION_TELNET
        && context->state != ewsContextStateTelnet
#       endif /* EW_CONFIG_OPTION_TELNET */

#       ifdef EW_CONFIG_OPTION_CLIENT
        && context->state != ewsContextStateClient
#       endif /* EW_CONFIG_OPTION_CLIENT */

#       ifdef EW_CONFIG_OPTION_UPNP
        && !((  (context->state == ewsContextStateDoMSearch)
              &&(context->ssdp_connection == TRUE)))
        && !((  (context->state == ewsContextStateNotify)
              &&(context->ssdp_connection == TRUE)))
        && !((  (context->state == ewsContextStateReNotify)
              &&(context->ssdp_connection == TRUE)))
#       endif /* EW_CONFIG_OPTION_UPNP */

#       ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
        && context->state != ewsContextStateSsdpCPSearchResourceDB
#       endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */

#       ifdef EMWEB_XMLP
        && context->state != ewsContextStateXMLP
#       endif /* EMWEB_XMLP */
       )
     {
       EMWEB_WARN(("ewsResume: bad state\n"));
       return EWS_STATUS_BAD_STATE;
     }

#   endif /* EMWEB_SANITY */

#   ifdef EMWEB_SANITY
    /*
     * BUGFIX: old implementation of ewsResume used
     * to call ewsRun directly.  This is NOT RECOMMENDED
     * in multitasking implementations of EmWeb (where
     * different tasks call ewsResume and ewsRun), as it
     * can cause race conditions/deadlocks.
     *
     * Instead, ewsResume will call the new interface
     * ewaResumeWakeup() [see ew_config.h].  Multitasking
     * implementations should use this routine to "wake up" the
     * emweb task so it will call ewsRun.
     */
    if (status == EWA_STATUS_OK)
      {
        EMWEB_ERROR(("ewsResume: No longer supports directly calling ewsRun.\n"
                     "Please refer to the release notes for more information.\n"
                   ));
      }
#   endif

    /*
     * note: order is important here in order to
     * prevent race conditions!  (Must be reverse of
     * flag-setting order of ewsResumeReadyContexts())
     */
    context->resumePending = TRUE;
    ews_state->contextResumed = TRUE;

    ewaResumeWakeup( context );

    return EWS_STATUS_OK;
}

#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
#endif /* EW_CONFIG_OPTION_SCHED */

#ifdef EW_CONFIG_OPTION_SNMP_AGENT
/*
 * ewsSNMPAgentGetState
 *
 * return the SNMP agent state on demand
 */
EwaSNMPAgentState
ewsSNMPAgentGetState()
{
  return ews_state->snmpAgent;
}

/*
 * ewsSNMPAgentSetState
 *
 * set the SNMP agent state on demand, return
 * old state.
 */
EwaSNMPAgentState
ewsSNMPAgentSetState( EwaSNMPAgentState newState )
{
  EwaSNMPAgentState old;

  old = ews_state->snmpAgent;
  ews_state->snmpAgent = newState;

  return old;
}
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */


/* ================================================================
 * Static Functions
 * ================================================================ */


#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
/*
 * move all contexts that have been ewsResumed from the
 * wait list to the ready list.
 *
 * !TBD! Modify to put suspended requests on a separate list
 * (better performance).
 */
static void
ewsResumeReadyContexts( void )
{
  EwsContext context;
  EwsLinkP   next;


  /* EWA_TASK_LOCK MUST BE TAKEN !! */
  /* KAGTODO # warning "ensure this!!" */

  EWA_TASK_LOCK();

  ews_state->contextResumed = FALSE;    /* do this first avoid race condition */
  for ( context = (EwsContext)ews_state->context_wait_list.next;
        context != (EwsContext)&ews_state->context_wait_list;
        context = (EwsContext)next
       )
    {
      next = context->link.next;
      if (   ewsContextSuspended == context->schedulingState
             && context->resumePending)
        {
#         ifdef EW_CONFIG_OPTION_SCHED_FC
          if (context->flow_control != ewsContextFlowControlOn)
#         endif /* EW_CONFIG_OPTION_SCHED_FC */
            {
              context->resumePending = FALSE;
              context->schedulingState = ewsContextResuming;
              EWS_LINK_DELETE(&context->link); 
              EWS_LINK_INSERT(&ews_state->context_serve_list, &context->link);
            }
        }
    }
    EWA_TASK_UNLOCK();

}
#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

/*
 * Product: EmWeb/Telnet
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
 * EmWeb/Server Telnet interfaces
 */
#include <string.h>
#include "ews.h"
#include "ews_sys.h"
#include "ews_def.h"
#include "ews_serv.h"
#include "ew_conneg.h"
#include "ews_tel.h"
#include "ews_telnet.h"
#include "ews_str.h"
#include "cliutil.h"
#include "cliapi.h"

#ifdef EW_CONFIG_OPTION_TELNET

#if  1 != _EWS_TELNET_H_ || \
     1 != _EWS_H_TELNET || \
     1 != _EWS_TEL_H_
#error "EmWeb/Telnet: Install error - file version mismatch"
#endif

/* Trace macro, helpful for watching telnet protocol traffic */
#define TELNET_TRACE(x)  /* printf x */

unsigned char telnet_raw_mode[] = TELNET_RAW_MODE;
unsigned char telnet_ga[] = { TELNET_IAC, TELNET_GA };

/*
 * ewsNetTelnetStart
 * Use instead of ewsNetHTTPStart to signal a telnet connection.
 *
 *   handle - network handle
 *   issue  - issue message
 *   prompt - current prompt to send after send reply document and after issue
 *   flags  - additional flags (e.g. EWS_TELNET_SERIAL_PORT)
 *
 * Returns context, or NULL on error
 */
EwsContext ewsNetTelnetStart ( EwaNetHandle handle
    ,const char   *issue
    ,const char   *prompt
    ,uintf        flags )
{
  EwsContext context;
  EwsStringP estring;
  EwaNetBuffer buffer;
  /*
   * Call ewsNetHTTPStart to set up an EmWeb context
   */
  context = ewsNetHTTPStart(handle);
  if (context == NULL)
  {
    return context;
  }

  /*
   * Initialize context for telnet mode
   */
  context->telnet = (EwsTelnetP) ewaAlloc(sizeof(EwsTelnet));
  if (context->telnet == NULL)
  {
    ewsNetHTTPAbort(context);
    return NULL;
  }

  context->state = ewsContextStateTelnet;
  context->url = (char*)"TELNET";
  context->req_protocol_maj = HTTP_NOT; /* prevent chunking */
  context->telnet->state = ewsTelnetStart;
  context->telnet->prompt = prompt;
  context->telnet->flags = flags;
  context->telnet->supress_ga = TRUE;
  context->telnet->need_flush = FALSE;

  /*
   * Prepare to write
   */
  if (context->res_buf_next == EWA_NET_BUFFER_NULL)
  {
    estring = &context->res_str_end;
    buffer = ewsStringAttachBuffer(estring);
    if (buffer == EWA_NET_BUFFER_NULL)
    {
      ewsNetHTTPAbort(context);
      return NULL;
    }
    context->res_buf_next = buffer;
  }

  /*
   * Set telnet session in raw mode, unless we are talking directly
   * to a terminal over a serial port.  In the latter case, there is no
   * need to send a telnet command, and we don't expect to receive
   * telnet commands.
   */
  if (0 == (flags & EWS_TELNET_SERIAL_PORT))
  {
    ewsStringNCopyIn(&context->res_str_end
    ,telnet_raw_mode
    ,sizeof(telnet_raw_mode));
  }

  /*
   * Send issue and prompt
   */
  if (issue != NULL)
  {
    ewsStringCopyIn(&context->res_str_end, issue);
  }
  if (prompt != NULL)
  {
    ewsStringCopyIn(&context->res_str_end, prompt);
  }
  ewsFlushAll(context);

#   ifdef EW_CONFIG_OPTION_CLI
  ewsCliStart(context);
#   endif /* EW_CONFIG_OPTION_CLI */

  return context;
}

/*
 * ewsNetTelnetPrompt
 * Change prompt to use after send reply
 */
  void
ewsSetTelnetPrompt(EwsContext context, const char * prompt)
{
  context->telnet->prompt = prompt;
}

/*
 * ewsSetTelnetStackingTrapMsg
 * Set trap string message of context
 */
  void
ewsSetTelnetStackingTrapMsg(EwsContext context, const char *trapString)
{
  context->telnet->trapString = trapString;
}

/*
 * ewsGetTelnetPrompt
 * Read the current prompt
 */
  const char *
ewsGetTelnetPrompt(EwsContext context)
{
  return context->telnet->prompt;
}

/*
 * Send headers (nothing to do)
 */
EwsStatus
ewsTelnetSendHeader( EwsContext context
    , EwsDocument document
    , const char *status
    , const char *string
    )
{
  (void)document;
  (void)status;
  (void)string;
# ifdef EMWEB_SANITY
  /*
   * Only valid for telnet context
   */
  if (context->telnet == NULL)
  {
    EMWEB_WARN(("ewsTelnetSendHeader: bad state\n"));
    return EWS_STATUS_BAD_STATE;
  }
# endif /* EMWEB_SANITY */

  return EWS_STATUS_OK;
}

/*
 * Finish sending message body to terminal (flush and reset state)
 */
  void
ewsTelnetFinish( EwsContext context )
{
  EwsTelnetP etp = context->telnet;

# ifdef EMWEB_SANITY
  /*
   * Only valid for telnet context
   */
  if (etp == NULL)
  {
    EMWEB_WARN(("ewsTelnetFinish: bad state\n"));
    return;
  }
# endif /* EMWEB_SANITY */

  /*
   * Send prompt
   */
  if (etp->prompt)
  {
    ewsStringCopyIn(&context->res_str_end, etp->prompt);
  }

  /*
   * If not supressing go-ahead, signal NVT
   */
  if (!etp->supress_ga)
  {
    ewsStringNCopyIn(&context->res_str_end
    ,telnet_ga
    ,sizeof(telnet_ga));
  }

  /*
   * Really finish
   */
  context->telnet = NULL;
  ewsInternalFinish(context);

  /*
   * Restore state and continue processing
   */
  context->telnet = etp;
  context->state = ewsContextStateTelnet;
}

/*
 * Telnet protocol wont/dont strings.  We write to the third byte to
 * return negative acknowledgements to the client for unsupported options.
 * Note that we only support LINEMODE and ECHO options.
 */
char ews_telnet_wont[] = {TELNET_IAC,TELNET_WONT, 0, 0};
char ews_telnet_dont[] = {TELNET_IAC,TELNET_DONT, 0, 0};

/*
 * IAC or CR search string
 */
char ews_telnet_iac_or_cr[] = {TELNET_IAC, '\r', 0};

/*
 * ewsTelnetParse
 * Called by ewsParse to handle incoming telnet data.
 *
 *   context - request context
 *
 * Returns TRUE to continue looping through parser states, FALSE to wait
 * for more data or change scheduler states.
 */
boolean
ewsTelnetParse ( EwsContext context )
{
  uintf match_c;
  EwaNetBuffer buffer;
  EwaStatus status = EWA_STATUS_OK;
  char *cp = NULL;
  EwsStringP estring;
  uintf bytes;
  uintf bytes_remaining;

# ifdef EMWEB_SANITY
  /*
   * Only valid for telnet context
   */
  if (context->telnet == NULL)
  {
    EMWEB_ERROR(("ewsTelnetParse: bad state\n"));
    ewsNetHTTPAbort(context);
    return FALSE;
  }
# endif /* EMWEB_SANITY */


  /*
   * Prepare response buffer for outgoing data
   */
  if (context->res_buf_next == EWA_NET_BUFFER_NULL)
  {
    estring = &context->res_str_end;
    buffer = ewsStringAttachBuffer(estring);
    if (buffer == EWA_NET_BUFFER_NULL)
    {
      ewsNetHTTPAbort(context);
      return FALSE;
    }
    context->res_buf_next = buffer;
  }

  /*
   * Handle by state
   */
  switch(context->telnet->state)
  {
    /*********************************************************************
     * Initial telnet state.  Waiting for character, special processing
     * if IAC or CR is encountered
     */
    case ewsTelnetStart:
      /*
       * Remember where we are, and search available buffers for
       * IAC or CR.
       */
      context->req_str_begin = context->req_str_end;
      /*
       * Advance begin pointer to beginning of next buffer if at end
       * of current buffer.
       */
      if (0 == ewsStringAvailable(&context->req_str_begin))
      {
        context->req_str_begin.first =
          ewaNetBufferNextGet(context->req_str_begin.first);
        context->req_str_begin.offset = 0;
      }
      ewsStringLengthSet(&context->req_str_begin, 0);
      match_c = ewsStringSearch(
        &context->req_str_begin
        , &context->req_str_end
        , ews_telnet_iac_or_cr
        , ewsStringSearchInclude
        , NULL );

      /*
       * If IAC or CR not found, then all data in this buffer is available
       * to write to application.
       */
      if (match_c == EWS_STRING_EOF)
      {
        bytes = ewsStringAvailable(&context->req_str_begin);
      }

      /*
       * Else IAC or CR found
       */
      else
      {
        /*
         * Determine number of bytes of data before IAC or CR
         */
        bytes = ewsStringLength(&context->req_str_begin);

        /*
         * If no data before IAC, then process IAC
         */
        if (bytes == 0 && match_c == TELNET_IAC)
        {
          ewsStringGetChar(&context->req_str_end);
          context->telnet->state = ewsTelnetHaveIAC;
          return TRUE;
        }

        /*
         * If CR found and present in this buffer, then include in data
         * to client and change state to skip expected NUL or LF following
         * CR, making sure they exist first.
         */
        if (   match_c == '\r'
               && bytes < ewsStringAvailable(&context->req_str_begin)
           )
        {
          bytes++;
          ewsStringExpand(&context->req_str_begin, 1);
          ewsStringGetChar(&context->req_str_end);
          context->telnet->state = ewsTelnetHaveCR;
        }

        /*
         * Adjust bytes downward to available contiguous bytes
         */
        if (bytes > ewsStringAvailable(&context->req_str_begin))
        {
          bytes = ewsStringAvailable(&context->req_str_begin);
        }
      }

      /*
       * Write data to application, if any
       */
      bytes_remaining = bytes;
      if (bytes > 0)
      {
        cp = ewaNetTelnetData(
          context->net_handle
          ,(char *) ewsStringData(&context->req_str_begin)
          ,& bytes_remaining );
        context->req_str_end = context->req_str_begin;
        ewsStringTrim(&context->req_str_end, bytes - bytes_remaining);
      }

      /*
       * Handle master state changes
       */
      if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT
#            ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
          || context->schedulingState == ewsContextSuspended
#            endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
#            ifdef EW_CONFIG_OPTION_CONTEXT_SEND_REPLY
          || context->state == ewsContextStateServingDocument
#            endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REPLY */
         )
      {
        return FALSE;
      }

#       ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
      /*
       * Return to scheduling state (may have been resuming)
       */
      context->schedulingState = ewsContextScheduled;
#       endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

      /*
       * Send return data to terminal
       */
      if (cp != NULL)
      {
        ewsStringCopyIn(&context->res_str_end, cp);

        /*
         * If not suppressing go-ahead, signal NVT
         */
        if (!context->telnet->supress_ga)
        {
          ewsStringNCopyIn(&context->res_str_end
                           ,telnet_ga
                           ,sizeof(telnet_ga) );
        }
      }
      if (cp != NULL || context->telnet->need_flush)
      {
        status = ewsFlushAll(context);
        context->telnet->need_flush = FALSE;
        if (status != EWA_STATUS_OK)
        {
          EMWEB_TRACE(("ewsTelnetParse: aborting context 0x%X\n", context));
          ewsNetHTTPAbort( context );
          return FALSE;
        }
      }

      if ((TRUE == cliLogoutDeferredGet(context)) &&
          (cliIoRedirectToCliGet() != L7_TRUE))
      {
        cliContextLogout(context);
      }

      /*
       * We now wish to free all unused buffers.
       *
       * First, check to see if we have processed all data in received
       * buffers.  If so, free the entire chain and wait for more data.
       */
      if (   0
             == ewsStringAvailable(&context->req_str_end)
             &&    EWA_NET_BUFFER_NULL
             == ewaNetBufferNextGet(context->req_str_end.first)
         )
      {
        ewaNetBufferFree(context->req_buf_first);
        context->req_buf_first = EWA_NET_BUFFER_NULL;
        break;
      }

      /*
       * Otherwise, release all buffers up to the current buffer.
       */
      ewsParseReleaseUnusedBuffers(context, FALSE);

      return TRUE;

      /*********************************************************************
       * Last character was a CR.  We expect the next character to be a
       * NULL or NL (e.g. CRNUL or CRLF).  This should be ignored and not
       * transmitted to application, according to the RFC.
       */
    case ewsTelnetHaveCR:
      context->req_str_begin = context->req_str_end;
      match_c = ewsStringGetChar(&context->req_str_end);
      if (match_c == EWS_STRING_EOF)
      {
        break;
      }
      if (match_c != '\0' && match_c != '\n')
      {
        context->req_str_end = context->req_str_begin;
      }
      context->telnet->state = ewsTelnetStart;
      return TRUE;

      /*********************************************************************
       * Last character was IAC. We expect the next character to be an escaped
       * telnet command.  For WILL/WONT/DO/DONT, we enter the appropriate
       * state and prepare to read the next character describing a particular
       * Telnet option.  We also support the AYT signal, in which case we
       * identify ourself.
       */
    case ewsTelnetHaveIAC:
      match_c = ewsStringGetChar(&context->req_str_end);
      switch(match_c)
      {
        case EWS_STRING_EOF:
          {
            break;
          }
        case TELNET_WILL:
          {
            TELNET_TRACE(("WILL "));
            context->telnet->state = ewsTelnetHaveWill;
            return TRUE;
          }
        case TELNET_WONT:
          {
            TELNET_TRACE(("WONT "));
            context->telnet->state = ewsTelnetHaveWont;
            return TRUE;
          }
        case TELNET_DO:
          {
            TELNET_TRACE(("DO "));
            context->telnet->state = ewsTelnetHaveDo;
            return TRUE;
          }
        case TELNET_DONT:
          {
            TELNET_TRACE(("DONT "));
            context->telnet->state = ewsTelnetHaveDont;
            return TRUE;
          }
        case TELNET_SB:
          {
            TELNET_TRACE(("SB "));
            context->telnet->state = ewsTelnetHaveSB;
            return TRUE;
          }
        case TELNET_AYT:
          {
            TELNET_TRACE(("AYT"));
            ewsStringCopyIn(&context->res_str_end
                            ,EMWEB_ID"\r\n");
            ewsFlushAll(context);
            context->telnet->state = ewsTelnetStart;
            return TRUE;
          }
        default:
          {
            context->telnet->state = ewsTelnetStart;
            return TRUE;
          }
      }
      break;

      /*********************************************************************
       * WILL/DO: We expect to see these for the options we support
       * (e.g. ECHO and SUPRESS_GO_AHEAD).  For anything else, we
       * send a negative acknowledgement indicating our unwillingness to
       * support unimplemented Telnet options.
       */
    case ewsTelnetHaveWill:
    case ewsTelnetHaveDo:
      match_c = ewsStringGetChar(&context->req_str_end);
      if (match_c == EWS_STRING_EOF)
      {
        break;
      }
      TELNET_TRACE(("%d\n", match_c));

      /*
       * Send negative acknowledgement of any option requested by client
       * except those that we request
       */
      if (match_c != TELNET_ECHO &&
          match_c != TELNET_SUPRESS_GO_AHEAD
         )
      {
        ews_telnet_dont[2] = ews_telnet_wont[2] = match_c;
        ewsStringCopyIn(&context->res_str_end
                        ,context->telnet->state == ewsTelnetHaveWill?
                        ews_telnet_dont : ews_telnet_wont);
        ewsFlushAll(context);
      }
      context->telnet->state = ewsTelnetStart;
      return TRUE;

      /*********************************************************************
       * DONT/WONT: We silently ignore the NVT's unwillingness to support
       * options, with the exception of SUPRESS_GO_AHEAD.  If the NVT
       * won't let us supress go aheads, we set a flag.  When we complete
       * each response to the NVT from now on, we must send a go ahead signal.
       */
    case ewsTelnetHaveDont:
    case ewsTelnetHaveWont:
      match_c = ewsStringGetChar(&context->req_str_end);
      if (match_c == EWS_STRING_EOF)
      {
        break;
      }

      /*
       * If NVT won't accept our supression of go ahead, then we must
       * send go ahead each time we send data to the NVT to force
       * character-at-a-time mode.
       */
      if (match_c == TELNET_SUPRESS_GO_AHEAD)
      {
        context->telnet->supress_ga = FALSE;
      }
      TELNET_TRACE(("%d\n", match_c));
      context->telnet->state = ewsTelnetStart;
      return TRUE;

      /*********************************************************************
       * SB: Telnet subnegotiation.  Wait until IAC-SE to return to
       * normal state.
       */
    case ewsTelnetHaveSB:
      match_c = ewsStringGetChar(&context->req_str_end);
      if (match_c == EWS_STRING_EOF)
      {
        break;
      }
      TELNET_TRACE(("%d\n", match_c));
      if (match_c == TELNET_IAC)
      {
        context->telnet->state = ewsTelnetHaveSBIAC;
      }
      return TRUE;

    case ewsTelnetHaveSBIAC:
      match_c = ewsStringGetChar(&context->req_str_end);
      if (match_c == EWS_STRING_EOF)
      {
        break;
      }
      TELNET_TRACE(("%d ", match_c));
      if (match_c == TELNET_SE)
      {
        TELNET_TRACE(("\n"));
        context->telnet->state = ewsTelnetStart;
      }
      else
      {
        context->telnet->state = ewsTelnetHaveSB;
      }
      return TRUE;

      /*********************************************************************
       * Default: unexpected state
       */
    default:
#       ifdef EMWEB_SANITY
      EMWEB_ERROR(("ewsTelnetParse: Unexpected state %d\n"
                   , context->telnet->state));
#       endif /* EMWEB_SANITY */
      break;
  }

  /*
   * Wait for more data
   */
  context->schedulingState = ewsContextDataWait;
  return FALSE;
}

/*
 * ewsTelnetWrite
 * Write text to client
 *
 * context - request context
 * text    - null terminated string of content to write to port
 */
  void
ewsTelnetWrite ( EwsContext context, const char * text )
{
  if (context->unbufferedWrite == TRUE)
    ewsTelnetWriteDataInsert( context, text);
  else
  {
    ewsStringCopyIn(&context->res_str_end, text);
    context->telnet->need_flush = TRUE;
  }
}

void ewsTelnetPrintf (EwsContext context, L7_char8 const *fmt, ...)
{
  va_list ap;
  L7_char8 str[1024];

  memset (&ap, 0, sizeof (ap));
  va_start (ap, fmt);
  (void) osapiVsnprintf (str, sizeof (str), fmt, ap);
  va_end (ap);

  ewsTelnetWrite (context, (const char *) str);
}

/*
 * ewsTelnetWriteDataInsert
 * Write third party data to client.
 *
 * context      - Context of request
 * text    - null terminated string of content to write to port.
 */
  void
ewsTelnetWriteDataInsert ( EwsContext context, const char * text )
{
  EwaNetBuffer buffer;
  EwsStringP   estring;

  estring = &context->res_str_end;
  if (context->res_buf_next == EWA_NET_BUFFER_NULL)
  {
    buffer = ewsStringAttachBuffer(estring);
    if (buffer == EWA_NET_BUFFER_NULL)
    {
      ewsNetHTTPAbort(context);
    }
    EWA_TASK_LOCK();
    context->res_buf_next = buffer;
    EWA_TASK_UNLOCK();
  }
  ewsStringCopyIn(&context->res_str_end, text);
  context->telnet->need_flush = TRUE;
  ewsFlushAll (context);
  buffer = ewsStringAttachBuffer(estring);
  if (buffer == EWA_NET_BUFFER_NULL)
  {
    ewsNetHTTPAbort(context);
  }
  EWA_TASK_LOCK();
  context->res_buf_next = buffer;
  EWA_TASK_UNLOCK();
}

/*
 * ewsTelnetSessionValidate
 * Determine if the telnet session pointer is valid.
 *
 * context      - Context of request
 */
  boolean
ewsTelnetSessionValidate ( EwsContext context )
{
  if (context->telnet == 0)
    return FALSE;
  else
    return TRUE;

  return FALSE;
}
#endif /* EW_CONFIG_OPTION_TELNET */

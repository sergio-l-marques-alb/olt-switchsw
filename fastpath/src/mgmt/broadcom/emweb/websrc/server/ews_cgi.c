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
 *      EmWeb/Server application interface to raw CGI
 *
 */

#include "ews.h"
#include "ews_cgi.h"
#include "ews_send.h"
#include "ews_serv.h"

#ifdef EW_CONFIG_OPTION_CGI

/******************************************************************************
 * CGI INTERFACE
 *
 * EmWeb supports a raw CGI interface for imagemap and various special
 * application-defined CGI capabilities.
 *
 * The EmWeb/Compiler creates CGI URLs from specifications provided in
 * _ACCESS files.  The application provides two functions for each CGI URL.
 * First, there is the CGI start function.  This is invoked by the
 * EmWeb/Server to mark the start of a new CGI request.  Second, there is the
 * CGI data function.  This is invoked by the EmWeb/Server one or more times
 * to pass network buffers containing raw CGI request data to the application.
 * The end of request data is indicated by an EWA_NET_BUFFER_NULL EwaNetBuffer.
 *
 * The CGI application uses the ewsCGIData() function to transmit response data
 * to the network.
 *
 *****************************************************************************/

/*
 * ewsCGISendStatus
 * This optional call causes the EmWeb/Server to construct and send an
 * appropriate  HTTP status header (and possibly other server-generated
 * headers).  It must be followed by one or more calls to ewsCGIData() to send
 * additional header information and optional body content.  (Note that a call
 * to ewsCGIData() must not precede the invocation of this function).
 *
 * Note: If the application desires complete control of the response, it may
 * simply send all header information itself using ewsCGIData() instead of
 * invoking this function.
 *
 * context      - context of request
 * status       - 3-digit status code followed by reason string
 * string       - string containing additional headers and/or body, or NULL
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD).
 */
EwsStatus
ewsCGISendStatus
  ( EwsContext context, const char * status, const char * string )
{
  EwsStatus estatus;

    EMWEB_TRACE(("ewsCGISendStatus(%p, %s)\n", context, status));

#   ifdef EMWEB_SANITY
    if (context->state != ewsContextStateServingCGI &&
        context->state != ewsContextStateStartingCGI)
      {
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Send header and (possibly) flush to network
     */
    estatus = ewsSendHeader(context, EWS_DOCUMENT_NULL, status, string);
    if (estatus != EWS_STATUS_OK)
      {
        return estatus;
      }
    if (ewsFlushAll(context) != EWA_STATUS_OK)
      {
        ewsNetHTTPAbort(context);
        return EWS_STATUS_ABORTED;
      }
    return EWS_STATUS_OK;
}

/*
 * ewsCGIData
 *
 * This function is called by a raw CGI application one or more times to send
 * raw CGI response data to the browser client.  The application is responsible
 * for throttling its use of the CPU.  Note that response data may include raw
 * HTTP headers as well as data.  Therefore, a CR-NL sequence must be
 * transmitted to properly delimit the header portion of the response from
 * any returned data.
 *
 * Note that ewsCGISendStatus() may be called once before any calls to
 * ewsCGIData() to generate standard HTTP headers.  Otherwise, the application
 * must generate all standard headers itself.
 *
 * Note that sending a EWA_NET_BUFFER_NULL terminates the request.
 *
 * context      - context of request
 * buffer       - buffer containing data to send, or EWA_NET_BUFFER_NULL
 *                if this is the last buffer in the response.
 *
 * No return value
 */
EwsStatus
ewsCGIData ( EwsContext context, EwaNetBuffer buffer )
{

    EMWEB_TRACE(("ewsCGIData(%p, %p)\n", context, buffer));

#   ifdef EMWEB_SANITY
    if (context->state != ewsContextStateServingCGI &&
        context->state != ewsContextStateStartingCGI)
      {
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * If application is closing CGI connection
     */
    if (buffer == EWA_NET_BUFFER_NULL)
      {
        ewsFlushAll(context);
        ewsNetHTTPAbort(context);
      }

    /*
     * Otherwise, pass data to network and flush
     */
    else
      {
        if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
          {
            return(EWS_STATUS_BAD_STATE);
          }
        else
          {
            (void) ewaNetHTTPSend(context->net_handle, buffer);
            if (ewsFlush(context) != EWA_STATUS_OK)
              {
                ewsNetHTTPAbort(context);
                return EWS_STATUS_ABORTED;
              }
          }
      }

    return EWS_STATUS_OK;
}

/*
 * ewsCGIRedirect
 * This call causes the EmWeb/Server to respond to the browser with the
 * document specified by the URL (if local), or with a redirect (if not).
 * This function will terminate the request.  Note that this feature can not
 * be used in conjunction with ewsCGIData() or ewsCGIStatus().
 *
 * context      - context of request
 * url          - URL for redirect
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD).
 */
EwsStatus
ewsCGIRedirect ( EwsContext context, const char *url )
{
    EMWEB_TRACE(("ewsCGIRedirect(%p, %s)\n", context, url));

#   ifdef EMWEB_SANITY
    if (context->state != ewsContextStateServingCGI &&
        context->state != ewsContextStateStartingCGI)
      {
        return EWS_STATUS_BAD_STATE;
      }
#   endif /* EMWEB_SANITY */

    ewsSendRedirect(context, url, ewsRedirectSeeOther);
    return EWS_STATUS_OK;
}

#ifdef EW_CONFIG_OPTION_CGI_SERVER_SOFTWARE
/*
 * ewsCGIServerSoftware
 *
 * Returns a string corresponding to the server software such as
 * "EmWeb/x.y"
 */
const char *
ewsCGIServerSoftware ( void )
{
    return EmWebId;
}
#endif /* EW_CONFIG_OPTION_CGI_SERVER_SOFTWARE */

#ifdef EW_CONFIG_OPTION_CGI_GATEWAY_INTERFACE
/*
 * ewsCGIGatewayInterface
 *
 * Returns a string corresponding to the CGI version "CGI/1.0"
 */
const char *
ewsCGIGatewayInterface ( void )
{
    return (const char *) ("CGI/1.0");
}
#endif /* EW_CONFIG_OPTION_CGI_GATEWAY_INTERFACE */

#ifdef EW_CONFIG_OPTION_CGI_SERVER_PROTOCOL
/*
 * ewsCGIServerProtocol
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Returns number of bytes in actual protocol string, or zero if not present.
 */
uintf ewsCGIServerProtocol ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_protocol);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut(datap, &context->req_protocol, length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CGI_SERVER_PROTOCOL */

#ifdef EW_CONFIG_OPTION_CGI_REQUEST_METHOD
/*
 * ewsCGIRequestMethod
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Returns number of bytes in actual request method, or zero if not present.
 */
uintf
ewsCGIRequestMethod ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_method_str);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut(datap, &context->req_method_str, length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CGI_REQUEST_METHOD */


#ifdef EW_CONFIG_OPTION_CGI_SCRIPT_NAME
/*
 * ewsCGIScriptName
 *
 * context      - context of request
 *
 * Returns pointer to script name string
 */
const char *
ewsCGIScriptName ( EwsContext context )
{
    return (const char *) context->url;
}
#endif /* EW_CONFIG_OPTION_CGI_SCRIPT_NAME */

#ifdef EW_CONFIG_OPTION_CGI_QUERY_STRING
/*
 * ewsCGIQueryString
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Unescapes query string
 *
 * Returns number of bytes in actual query string, or zero if not present.
 */
uintf
ewsCGIQueryString ( EwsContext context, char *datap, uintf length )
{
  uintf c;
  uintf bytes = 0;
  EwsString estring = context->req_query;
  boolean have_pct = FALSE;
  boolean have_hex1 = FALSE;
  uintf hex;
  uint8 acc = 0;                /* Not necessary, since any initial value will
                                 * be shifted beyond the low-order 8 bits which
                                 * will be used, but it stops gcc from
                                 * complaining that the variable is
                                 * uninitialized. */

    /*
     * Read characters in escaped query string and unescape them.  We process
     * the whole query string to get the actual number of bytes.
     */
    while (ewsStringLength(&estring) > 0)
      {
        c = ewsStringGetChar(&estring);
        if (c == EWS_STRING_EOF)
          {
            break;
          }

        /*
         * Check for %<HEX><HEX>.  If we have '%':
         */
        if (have_pct)
          {
            /*
             * Read hex digit
             */
            if (c >= '0' && c <= '9')
              {
                hex = c - '0';
              }
            else if (c >= 'a' && c <= 'z')
              {
                hex = 10 + c - 'a';
              }
            else
              {
                hex = 10 + c - 'A';
              }
            acc = (acc << 4) | hex;

            /*
             * If second hex digit, return unescaped character
             */
            if (have_hex1)
              {
                have_pct = FALSE;
                have_hex1 = FALSE;
                c = acc;
              }

            /*
             * Otherwise, wait for second hex digit
             */
            else
              {
                have_hex1 = TRUE;
                continue;
              }
          }

        /*
         * Unescape + to <SP>
         */
        else if (c == '+')
          {
            c = ' ';
          }

        /*
         * Look for '%' for %<HEX><HEX>
         */
        else if (c == '%')
          {
            have_pct = TRUE;
            continue;
          }

        /*
         * Write character to application buffer, and continue
         */
        if (length > 1)
          {
            *datap++ = c;
            length--;
          }
        bytes++;
      }

    /*
     * Terminate string
     */
    if (length > 0 && bytes > 0)
      {
        *datap = '\0';
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CGI_QUERY_STRING */

#ifdef EW_CONFIG_OPTION_CGI_QUERY_STRING_ESCAPED
/*
 * ewsCGIQueryStringEscaped
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Returns number of bytes in actual unescaped query string, or zero if not
 * present.
 */
uintf
ewsCGIQueryStringEscaped ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_query);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_query
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CGI_QUERY_STRING_ESCAPED */

#ifdef EW_CONFIG_OPTION_CGI_CONTENT_TYPE
/*
 * ewsCGIContentType
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Returns number of bytes in actual content type, or zero if not present.
 */
uintf
ewsCGIContentType ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength(&context->req_headerp[ewsRequestHeaderContentType]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderContentType]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CGI_CONTENT_TYPE */

#ifdef EW_CONFIG_OPTION_CGI_CONTENT_LENGTH
/*
 * ewsCGIContentLength
 *
 * context      - context of request
 *
 * Returns content length as specified by client, or zero if not present
 */
uint32
ewsCGIContentLength ( EwsContext context )
{
#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    if (context->chunked_encoding)
      {
        return EWS_CGI_CONTENT_LENGTH_CHUNKED;
      }
    else
#   endif
      {
        return(context->content_length);
      }
}
#endif /* EW_CONFIG_OPTION_CGI_CONTENT_LENGTH */

#ifdef EW_CONFIG_OPTION_CGI_CONTENT_ENCODING
/*
 * ewsCGIContentEncoding
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Returns number of bytes in actual content encoding, or zero if not present.
 */
uintf
ewsCGIContentEncoding ( EwsContext context, char *datap, uintf length )
{
  uintf bytes;

    bytes = ewsStringLength
      (&context->req_headerp[ewsRequestHeaderContentEncoding]);
    if (length > bytes + 1)
      {
        length = bytes + 1;
      }
    if (length >= 1)
      {
        ewsStringCopyOut
          (datap
          ,&context->req_headerp[ewsRequestHeaderContentEncoding]
          ,length - 1);
      }
    return bytes;
}
#endif /* EW_CONFIG_OPTION_CGI_CONTENT_ENCODING */

#endif /* EW_CONFIG_OPTION_CGI */


#if defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
 || defined( EW_CONFIG_OPTION_URL_PATH_INFO )
/*
 * ewsCGIPathInfo
 *
 * context      - context of request
 *
 * Returns pointer to path information string, or NULL if not present.
 * This function can be useful even if CGI is not included in
 * the build, hence the separate EW_CONFIG_OPTION_URL_PATH_INFO
 */
const char *
ewsCGIPathInfo ( EwsContext context )
{
    return (const char *)context->path_info;
}
#endif /* EW_CONFIG_OPTION_CGI/URL_PATH_INFO */


#if !defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
 && !defined( EW_CONFIG_OPTION_URL_PATH_INFO ) \
 && !defined( EW_CONFIG_OPTION_CGI )
     /* ANSI C forbids empty source file, so just stub this */
int ews_no_cgi;
#endif

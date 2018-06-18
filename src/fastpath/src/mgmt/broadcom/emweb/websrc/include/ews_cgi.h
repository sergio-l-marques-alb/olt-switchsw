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
 * EmWeb/Server application interface to raw CGI
 *
 */
#ifndef _EWS_CGI_H_
#define _EWS_CGI_H_

#include "ew_types.h"

/******************************************************************************
 * CGI INTERFACE
 *
 * NOTICE: THIS API HAS BEEN DEPRICATED.  USE THE FILESYSTEM API INSTEAD
 *         FOR RAW CGI APPLICATIONS.  SOME CONTEXT ACCESS FUNCTIONS 
 *         DEFINED HERE MAY STILL BE USED.  OBSOLETED FUNCTIONS ARE 
 *         MARKED "(DEPRICATED)".
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
 * ewaCGIStart (DEPRICATED)
 *
 * This per-CGI function is provided by the application.  It is invoked by
 * EmWeb/Server at the start of a new request to the corresponding CGI URL.
 * The application may return an application-specific handle.
 *
 * context      - context of current request
 *
 * Returns application-specific handle to be used by EmWeb/Server in
 * subsequent calls to ewaCGIData.
 */
typedef EwaCGIHandle EwaCGIStart_f ( EwsContext context );

/*
 * ewaCGIData (DEPRICATED)
 *
 * This per-CGI function is provided by the application.  It is invoked by
 * EmWeb/Server zero or more times to pass raw CGI request data to the
 * application.
 *
 * cgi_handle   - application-specific handle as defined by ewaCGIStart
 * buffer       - application buffer containing all or part of the raw CGI
 *                request data
 *
 * No return value
 */
typedef void EwaCGIData_f ( EwaCGIHandle cgi_handle, EwaNetBuffer buffer );

#if defined( EW_CONFIG_OPTION_CGI_PATH_INFO ) \
 || defined( EW_CONFIG_OPTION_URL_PATH_INFO )
/*
 * ewsCGIPathInfo
 *
 * context      - context of request
 *
 * Returns pointer to PathInfo string, or NULL if not present.
 * This function may be used without the rest of the CGI support
 * by defining EW_CONFIG_OPTION_URL_PATH_INFO
 */
extern const char * ewsCGIPathInfo ( EwsContext context );
#endif /* EW_CONFIG_OPTION_CGI/URL_PATH_INFO */


#ifdef EW_CONFIG_OPTION_CGI

/*
 * ewsCGISendStatus (DEPRICATED)
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
 * string       - string containing additional headers and/or data, or NULL
 *
 * Returns EWS_STATUS_OK on success, else error code (TBD).
 */
extern EwsStatus ewsCGISendStatus
  ( EwsContext context, const char * status, const char * string );

/*
 * ewsCGIData (DEPRICATED)
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
extern EwsStatus ewsCGIData ( EwsContext context, EwaNetBuffer buffer );

/*
 * ewsCGIRedirect (DEPRICATED)
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
extern EwsStatus ewsCGIRedirect ( EwsContext context, const char *url );

#ifdef EW_CONFIG_OPTION_CGI_SERVER_SOFTWARE
/*
 * ewsCGIServerSoftware
 *
 * Returns a string corresponding to the server software such as
 * "EmWeb/R6_2_0"
 */
extern const char *ewsCGIServerSoftware ( void );
#endif /* EW_CONFIG_OPTION_CGI_SERVER_SOFTWARE */

#ifdef EW_CONFIG_OPTION_CGI_GATEWAY_INTERFACE
/*
 * ewsCGIGatewayInterface
 *
 * Returns a string corresponding to the CGI version "CGI/1.0"
 */
extern const char *ewsCGIGatewayInterface ( void );
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
extern uintf ewsCGIServerProtocol
  ( EwsContext context, char *datap, uintf length );
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
extern uintf ewsCGIRequestMethod
  ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CGI_REQUEST_METHOD */

#ifdef EW_CONFIG_OPTION_CGI_SCRIPT_NAME
/*
 * ewsCGIScriptName
 *
 * context      - context of request
 *
 * Returns pointer to script name string
 */
extern const char * ewsCGIScriptName ( EwsContext context );
#endif /* EW_CONFIG_OPTION_CGI_SCRIPT_NAME */

#ifdef EW_CONFIG_OPTION_CGI_QUERY_STRING
/*
 * ewsCGIQueryString
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Copies unescaped (canonical) query string.
 *
 * Returns number of bytes in actual query string, or zero if not present.
 */
extern uintf ewsCGIQueryString
  ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CGI_QUERY_STRING */

#ifdef EW_CONFIG_OPTION_CGI_QUERY_STRING_ESCAPED
/*
 * ewsCGIQueryStringEscaped
 *
 * context      - context of request
 * datap        - pointer to data buffer
 * length       - size of data buffer (may be zero)
 *
 * Copies original query string, possibly including escape characters
 *
 * Returns number of bytes in actual query string, or zero if not present.
 */
extern uintf ewsCGIQueryStringEscaped
  ( EwsContext context, char *datap, uintf length );
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
extern uintf ewsCGIContentType
  ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CGI_CONTENT_TYPE */

#ifdef EW_CONFIG_OPTION_CGI_CONTENT_LENGTH
/*
 * ewsCGIContentLength
 *
 * context      - context of request
 *
 * Returns content length as specified by client, or zero if not present,
 * or EWS_CGI_CONTENT_LENGTH_CHUNKED if chunked.
 */
#define EWS_CGI_CONTENT_LENGTH_CHUNKED  0xffffffff
extern uint32 ewsCGIContentLength ( EwsContext context );
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
extern uintf ewsCGIContentEncoding
  ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CGI_CONTENT_ENCODING */

#endif /* EW_CONFIG_OPTION_CGI */

#endif /* _EWS_CGI_H_ */

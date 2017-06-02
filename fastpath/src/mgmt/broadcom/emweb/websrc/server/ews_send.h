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
 * EmWeb/Server transmission utilities
 *
 */

#ifndef EWS_SEND_H_
#define EWS_SEND_H_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_def.h"

extern const EwsConstCharP ews_no_body; /* may not be modified */

/*
 * Global strings for standard status and headers.  These may be overridden
 * in ew_config.h.
 *
 * NOTE: The EWS_HTTP_STRING_XXX definitions may now take two forms.
 *       If the first character is '/', then this is the URL of an installed
 *       archive document that will be served as the response body.  This
 *       is especially useful for "404 Not Found" and "401 Unauthorized"
 *       to allow more sophisticated and customizable look-and-feel responses.
 */
#ifdef EW_CONFIG_OPTION_UPNP
#ifndef EWS_HTTP_STATUS_200     /* OK */
#define EWS_HTTP_STATUS_200     "OK"
#endif
#ifndef EWS_HTTP_STRING_200
#define EWS_HTTP_STRING_200     "\r\n200 OK\r\n"
#endif
extern const EwsConstCharP ews_http_status_200;
extern EwsConstCharP ews_http_string_200;
#endif /* EW_CONFIG_OPTION_UPNP */

#ifdef EW_CONFIG_OPTION_FILE_PUT
#ifndef EWS_HTTP_STATUS_201     /* Created */
#define EWS_HTTP_STATUS_201     "Created"
#endif
#ifndef EWS_HTTP_STRING_201
#define EWS_HTTP_STRING_201     "\r\n201 Created\r\n"
#endif
extern const EwsConstCharP ews_http_status_201;
extern EwsConstCharP ews_http_string_201;
#endif /* EW_CONFIG_OPTION_FILE_PUT */

#if defined(EW_CONFIG_OPTION_FORM) || defined(EW_CONFIG_OPTION_IMAGEMAP) ||\
    defined(EW_CONFIG_OPTION_FILE_PUT) || defined(EW_CONFIG_OPTION_UPNP)
#ifndef EWS_HTTP_STATUS_204     /* No Content */
#define EWS_HTTP_STATUS_204     "No Content"
#endif
#ifdef EWS_HTTP_STRING_204
#  error "EWS_HTTP_STRING_204 may not be redefined"
#endif
extern const EwsConstCharP ews_http_status_204;
extern EwsConstCharP ews_http_string_204;
#endif /* EW_CONFIG_OPTION_FORM | IMAGEMAP */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
#ifndef EWS_HTTP_STATUS_206
#define EWS_HTTP_STATUS_206     "Partial Content"
#endif
extern const EwsConstCharP ews_http_status_206;
#endif /* EW_CONFIG_OPTION_BYTE_RANGES */


#if defined(EW_CONFIG_OPTION_CONDITIONAL_GET) \
 || defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH)
#ifndef EWS_HTTP_STATUS_304     /* Not Modified */
#define EWS_HTTP_STATUS_304     "Not Modified"
#endif
#ifdef EWS_HTTP_STRING_304
#  error "EWS_HTTP_STRING_304 may not be redefined"
#endif
extern const EwsConstCharP ews_http_status_304;
#endif /* EW_CONFIG_OPTION_CONDITIONAL_GET/MATCH */

#ifndef EWS_HTTP_STATUS_400     /* Bad Request */
#define EWS_HTTP_STATUS_400     "Bad Request"
#endif
#ifndef EWS_HTTP_STRING_400
#define EWS_HTTP_STRING_400     "\r\n400 Bad Request\r\n"
#endif
extern const EwsConstCharP ews_http_status_400;
extern EwsConstCharP ews_http_string_400;

#ifdef EW_CONFIG_OPTION_AUTH
#ifndef EWS_HTTP_STATUS_401     /* Unauthorized */
#define EWS_HTTP_STATUS_401     "Unauthorized"
#endif
#ifndef EWS_HTTP_STRING_401
#define EWS_HTTP_STRING_401     "\r\n401 Unauthorized\r\n"
#endif
extern const EwsConstCharP ews_http_status_401;
extern EwsConstCharP ews_http_string_401;
#endif /* EW_CONFIG_OPTION_AUTH */

#ifndef EWS_HTTP_STATUS_404     /* Not Found */
#define EWS_HTTP_STATUS_404     "Not Found"
#endif
#ifndef EWS_HTTP_STRING_404
#define EWS_HTTP_STRING_404     "\r\n404 Not Found\r\n"
#endif
extern const EwsConstCharP ews_http_status_404;
extern EwsConstCharP ews_http_string_404;

#ifndef EWS_HTTP_STATUS_405     /* Method Not Allowed */
#define EWS_HTTP_STATUS_405     "Method Not Allowed"
#endif
#ifndef EWS_HTTP_STRING_405
#define EWS_HTTP_STRING_405     "\r\n405 Method Not Allowed\r\n"
#endif
extern const EwsConstCharP ews_http_status_405;
extern EwsConstCharP ews_http_string_405;

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && \
    !defined(EW_CONFIG_OPTION_CHUNKED_IN)
#ifndef EWS_HTTP_STATUS_411     /* Length Required */
#define EWS_HTTP_STATUS_411     "Length Required"
#endif
#ifndef EWS_HTTP_STRING_411
#define EWS_HTTP_STRING_411     "\r\n411 Length Required\r\n"
#endif
extern const EwsConstCharP ews_http_status_411;
extern EwsConstCharP ews_http_string_411;
#endif /* HTTP_PROTOCOL >= HTTP_1_1 && !CHUNKED_IN */

#if defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) || \
    defined(EW_CONFIG_OPTION_UPNP)
#ifndef EWS_HTTP_STATUS_412     /* Precondition Failed */
#define EWS_HTTP_STATUS_412     "Precondition Failed"
#endif
#ifndef EWS_HTTP_STRING_412
#define EWS_HTTP_STRING_412     "\r\n412 Precondition Failed\r\n"
#endif

extern const EwsConstCharP ews_http_status_412;
extern EwsConstCharP ews_http_string_412;
#endif /* defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) || \
          defined(EW_CONFIG_OPTION_UPNP) */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
#ifndef EWS_HTTP_STATUS_416     /* Length Required */
#define EWS_HTTP_STATUS_416     "Requested range not satisfiable"
#endif
extern const EwsConstCharP ews_http_status_416;
#endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && defined(EW_CONFIG_OPTION_EXPECT)
#ifndef EWS_HTTP_STATUS_417     /* Expectation Failed */
#define EWS_HTTP_STATUS_417     "Expectation Failed"
#endif
#ifndef EWS_HTTP_STRING_417
#define EWS_HTTP_STRING_417     "\r\n417 Expectation Failed\r\n"
#endif
extern const EwsConstCharP ews_http_status_417;
extern EwsConstCharP ews_http_string_417;
#endif /* HTTP_PROTOCOL >= HTTP_1_1 && EXPECT */

#ifndef EWS_HTTP_STATUS_500     /* Internal Error */
#define EWS_HTTP_STATUS_500     "Internal Error"
#endif
#ifndef EWS_HTTP_STRING_500
#define EWS_HTTP_STRING_500     "\r\n500 Internal Error\r\n"
#endif
extern const EwsConstCharP ews_http_status_500;
extern EwsConstCharP ews_http_string_500;

#ifndef EWS_HTTP_STATUS_501     /* Not Implemented */
#define EWS_HTTP_STATUS_501     "Not Implemented"
#endif
#ifndef EWS_HTTP_STRING_501
#define EWS_HTTP_STRING_501     "\r\n501 Not Implemented\r\n"
#endif
extern const EwsConstCharP ews_http_status_501;
extern EwsConstCharP ews_http_string_501;


#ifdef EW_CONFIG_OPTION_DEMAND_LOADING
#ifndef EWS_HTTP_STATUS_503     /* Service Unavailable */
#define EWS_HTTP_STATUS_503     "Service Unavailable"
#endif
#ifndef EWS_HTTP_STRING_503
#define EWS_HTTP_STRING_503     "\r\n503 Service Unavailable\r\n"
#endif
extern const EwsConstCharP ews_http_status_503;
extern EwsConstCharP ews_http_string_503;
#endif /* EW_CONFIG_OPTION_DEMAND_LOADING */

#ifndef EWS_HTTP_STATUS_505     /* HTTP Version Not Supported */
#define EWS_HTTP_STATUS_505     "HTTP Version Not Supported"
#endif
#ifndef EWS_HTTP_STRING_505
#define EWS_HTTP_STRING_505     "\r\n505 HTTP Version Not Supported\r\n"
#endif
extern const EwsConstCharP ews_http_status_505;
extern EwsConstCharP ews_http_string_505;

#ifdef  EW_CONFIG_OPTION_CONTENT_NEGOTIATION
typedef enum EwsTcnHdrType_e
{
  ewsTcnHdrNone,
  ewsTcnHdrChoice,
  ewsTcnHdrList
} EwsTcnHdrType;

#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */


/*
 * ewsSendHeader
 * This function generates a standardized HTTP response header
 *
 * context      - Context of request
 * document     - Document for header generation
 *                EWS_DOCUMENT_NULL means not to send document-specific headers
 * status       - Status string
 *                NULL indicates that this is a request, rather than a response
 * string       - Text to append to headers, or NULL (may include headers)
 *
 * Returns EWS_STATUS_OK on success, EWS_STATUS_NO_RESOURCES if buffers
 * unavailable.
 */
extern EwsStatus ewsSendHeader
  ( EwsContext context, EwsDocument document, const char * status,
    const char *string );

/*
 * ewsSendHeaderAndEnd
 * This function generates an error HTTP response header and ends the request
 *
 * context      - Context of request
 * status       - Status string
 * string       - Text to append to headers, or NULL (may include headers)
 *
 * Returns EWA_STATUS_OK or EWA_STATUS_OK_YIELD
 */
extern void ewsSendHeaderAndEnd
  ( EwsContext context, const char * status, const char *string );

/*
 * ewsSendStatusString
 * This function processes the HTTP status string, usually passed to
 * ewsSendHeaderAndEnd, but sometimes (for authentication) after ewsSendHeader
 * has already been invoked.  If the first character of the string is '/',
 * then set up a response body from the archive using string as the URL.
 * Otherwise, the string contains text (including the blank line after headers)
 * to be transmitted.
 *
 * context     - Context of request
 * string      - status string or URL
 * status      - status if sending headers, else NULL
 */
void
ewsSendStatusString( EwsContext context
                   , const char *string
                   , const char *status );

/*
 * ewsSendRedirect
 * Send a redirection header.  (The caller is responsible for aborting the
 * context on return).
 *
 * context      - context of request
 * url          - redirection URL (may be absolute or relative)
 * type         - ewsRedirectTemporary, Permanent, SeeOther
 *
 * No return value
 */
typedef enum
  {
    ewsRedirectTemporary,
    ewsRedirectPermanent,
    ewsRedirectSeeOther
  } EwsRedirectType;

extern void ewsSendRedirect
  ( EwsContext context, const char *url, EwsRedirectType type );

/*
 * ewsSendContinue
 * This function generates an HTTP/1.1 or later 100 Continue status
 */
extern void ewsSendContinue ( EwsContext context );

/*
 * ewsBodySend
 *     Append a null-terminated string to the body of a response;
 *
 * ewsBodyNSend
 *     Append known length content to the body of a response
 *
 * These routines handle all chunking and digest generation for body output,
 * updating the context as appropriate.  ALL body content MUST go through
 * these interfaces in order to be written out the network connection!
 */

EwsStatus ewsBodySend( EwsContext context
                       ,const char * str ); /* null terminated str */
EwsStatus ewsBodyNSend( EwsContext context
                        ,const uint8 *val
                        ,const uintf len );

#ifdef EW_CONFIG_OPTION_CONVERT_XML
/*
 * Conversion of HTML to XML requires that some content be "dropped"
 * rather than sent out the network.  These routines control the above
 * body routines to allow dropping of network data.
 */
extern void ewsBodyOutputOff( EwsContext ); /* disable output */
extern void ewsBodyOutputOn( EwsContext );  /* enable output */
#endif /* !EW_CONFIG_OPTION_CONVERT_XML */

/*
 * Chunk Header primitives
 *   These are mostly to be used by the above routines, but are available
 *   for direct use by the CGI/File interfaces that need direct access to
 *   the buffer strings.
 *   These manage all context->chunk_out_state changes.
 */
/*
 * ewsBodyNewChunkHeader
 *   Insert a chunk header of 8 zeros
 */
extern void ewsBodyNewChunkHeader( EwsContext context );
/*
 * ewsBodyUpdateChunkHeader
 *   Correct the length of the most recently added chunk header
 */
extern void ewsBodyUpdateChunkHeader( EwsContext context );


/*
 * ewsFlush
 * Flush whole buffers queued for transmission and return application's
 * status from ewaHTTPSend().
 *
 * context      - Context of request
 *
 * Returns application's status from ewaHTTPSend, or EWA_STATUS_OK if
 * nothing sent.
 */
extern EwaStatus ewsFlush ( EwsContext context );

/*
 * ewsFlushAll
 * Flush all buffers queued for transmission and return application's
 * status from ewaHTTPSend().
 *
 * context      - Context of request
 *
 * Returns application's status from ewaHTTPSend, or EWA_STATUS_OK if
 * nothing sent.
 */
extern EwaStatus ewsFlushAll ( EwsContext context );

/*
 * ewsRetrieveEtag
 * Retrieve the ETAG for the given document.
 *
 * document   - document to find the ETAG
 *
 * Returns an ETAG if one exists or NULL if it does not exist.
 */
extern const char * ewsRetrieveEtag(EwsContext context, EwsDocument document);

/*
 * ewsGetDocLength
 * utility routine to determine a document's length.  Returns the
 * length if known (no dynamic content), or EWS_CONTENT_LENGTH_UNKNOWN.
 */
extern int32 ewsGetDocLength( EwsContext context, EwsDocument document );
#endif /* EWS_SEND_H_ */

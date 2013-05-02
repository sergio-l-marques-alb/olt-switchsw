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
 * EmWeb/Server response transmission utilities
 *
 */

#include "ews.h"
#include "ewnet.h"
#include "ews_send.h"
#include "ews_serv.h"
#include "ews_sys.h"
#include "ew_lib.h"
#include "ew_conneg.h"
#include "ews_gway.h"
#include "ews_str.h"
#include "ews_ctxt.h"

extern uint32 ewsGetAttrBlkOffset(EwsDocument);
extern uint32 ewsGetAttr(EwsDocument, uint32, EwsAttr, uint8 *);

/*
 * Global strings for standard status and headers.
 * The constant ews_no_body has a special meaning in ewsSendHeaderAndEnd;
 * explicitly using it for the 'string' parameter (which includes any
 * additional headers and body) signals that routine that it may be all right
 * to keep a persistent connection open.  With any string parameter other
 * than 'ews_no_body' that routine will close the connection for any method
 * other than HEAD.
 */

const EwsConstCharP ews_no_body = "\r\n";

/*
 * Global strings for standard status and headers.  These may be overridden
 * in ew_config.h.
 */

#ifdef EW_CONFIG_OPTION_UPNP
const EwsConstCharP ews_http_status_200 = "200 " EWS_HTTP_STATUS_200;
EwsConstCharP ews_http_string_200 = EWS_HTTP_STRING_200;
#endif /* EW_CONFIG_OPTION_UPNP */

#ifdef EW_CONFIG_OPTION_FILE_PUT
const EwsConstCharP ews_http_status_201 = "201 " EWS_HTTP_STATUS_201;
EwsConstCharP ews_http_string_201 = EWS_HTTP_STRING_201;
#endif /* EW_CONFIG_OPTION_FILE_PUT */

#if defined(EW_CONFIG_OPTION_FORM) || defined(EW_CONFIG_OPTION_IMAGEMAP) ||\
    defined(EW_CONFIG_OPTION_FILE_PUT)
const EwsConstCharP ews_http_status_204 = "204 " EWS_HTTP_STATUS_204;
#endif /* EW_CONFIG_OPTION_FORM | IMAGEMAP */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
const EwsConstCharP ews_http_status_206 = "206 " EWS_HTTP_STATUS_206;
#endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#if defined(EW_CONFIG_OPTION_CONDITIONAL_GET) \
 || defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH)
const EwsConstCharP ews_http_status_304 = "304 " EWS_HTTP_STATUS_304;
#endif /* EW_CONFIG_OPTION_CONDITIONAL_GET/MATCH */

const EwsConstCharP ews_http_status_400 = "400 " EWS_HTTP_STATUS_400;
EwsConstCharP ews_http_string_400 = EWS_HTTP_STRING_400;

#ifdef EW_CONFIG_OPTION_AUTH
const EwsConstCharP ews_http_status_401 = "401 " EWS_HTTP_STATUS_401;
EwsConstCharP ews_http_string_401 = EWS_HTTP_STRING_401;
#endif /* EW_CONFIG_OPTION_AUTH */

const EwsConstCharP ews_http_status_404 = "404 " EWS_HTTP_STATUS_404;
EwsConstCharP ews_http_string_404 = EWS_HTTP_STRING_404;
const EwsConstCharP ews_http_status_405 = "405 " EWS_HTTP_STATUS_405;
EwsConstCharP ews_http_string_405 = EWS_HTTP_STRING_405;

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && !defined(EW_CONFIG_OPTION_CHUNKED_IN)
const EwsConstCharP ews_http_status_411 = "411 " EWS_HTTP_STATUS_411;
EwsConstCharP ews_http_string_411 = EWS_HTTP_STRING_411;
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && !EW_CONFIG_OPTION_CHUNKED_IN */

#if defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) || \
    defined(EW_CONFIG_OPTION_UPNP)
const EwsConstCharP ews_http_status_412 = "412 " EWS_HTTP_STATUS_412;
EwsConstCharP ews_http_string_412 = EWS_HTTP_STRING_412;
#endif /* defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) || \
          defined(EW_CONFIG_OPTION_UPNP) */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
const EwsConstCharP ews_http_status_416 = "416 " EWS_HTTP_STATUS_416;
#endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && defined(EW_CONFIG_OPTION_EXPECT)
const EwsConstCharP ews_http_status_417 = "417 " EWS_HTTP_STATUS_417;
EwsConstCharP ews_http_string_417 = EWS_HTTP_STRING_417;
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && EXPECT */

const EwsConstCharP ews_http_status_500 = "500 " EWS_HTTP_STATUS_500;
EwsConstCharP ews_http_string_500 = EWS_HTTP_STRING_500;
const EwsConstCharP ews_http_status_501 = "501 " EWS_HTTP_STATUS_501;
EwsConstCharP ews_http_string_501 = EWS_HTTP_STRING_501;


#ifdef EW_CONFIG_OPTION_DEMAND_LOADING
const EwsConstCharP ews_http_status_503 = "503 " EWS_HTTP_STATUS_503;
EwsConstCharP ews_http_string_503 = EWS_HTTP_STRING_503;
#endif /* EW_CONFIG_OPTION_DEMAND_LOADING */

const EwsConstCharP ews_http_status_505 = "505 " EWS_HTTP_STATUS_505;
EwsConstCharP ews_http_string_505 = EWS_HTTP_STRING_505;

/*
 * ewsSendHeader
 * This function builds standard headers to answer the request.
 *
 * context      - request context
 * document     - Document for header generation
 *                EWS_DOCUMENT_NULL means not to send document-specific headers
 * status       - status code and descriptive text
 *                NULL indicates that this is a request, rather than a response
 * string       - Text to append to headers, or NULL (may include headers)
 *
 * Return EWS_STATUS_OK on success, else error code.
 * If EWS_STATUS_NO_RESOURCES is returned, the context has been aborted.
 */
EwsStatus
ewsSendHeader( EwsContext context
             , EwsDocument document
             , const char *status
             , const char *string
             )
{
  EwaNetBuffer  buffer = EWA_NET_BUFFER_NULL;
  EwsStringP    estring;
  char          ltoa_string[12];
  char          *ltoa_ptr;

# ifdef EW_CONFIG_OPTION_BYTE_RANGES
  boolean       acceptRanges = FALSE;
  boolean       isRange = FALSE;
  int32         firstBytePos = 0;
  int32         lastBytePos = EWS_CONTENT_LENGTH_UNKNOWN;
  uint32        instanceLength = EWS_CONTENT_LENGTH_UNKNOWN;
# endif /* EW_CONFIG_OPTION_BYTE_RANGES */

/*
 * Define some convenient macros for noting if we are running as a
 * client or server.  We want to share as much code as possible
 * between EmWeb/Server and EmWeb/Client.  Many of the generated
 * headers are common, some are different.
 */
#ifdef EW_CONFIG_OPTION_CLIENT
#define IF_CLIENT if (context->client != NULL)
#define IF_NOT_CLIENT if (context->client == NULL)
#else /* EW_CONFIG_OPTION_CLIENT */
#define IF_CLIENT if (FALSE)
#define IF_NOT_CLIENT if (TRUE)
#endif /* EW_CONFIG_OPTION_CLIENT */

    EMWEB_TRACE(("ewsSendHeader(%p, %p, %s)\n", context, document, status));

#   ifdef EW_CONFIG_OPTION_CONTEXT_SEND_ERROR
    context->flags |= EWS_CONTEXT_FLAGS_SENT_HEADER;
#   endif /* EW_CONFIG_OPTION_CONTEXT_SEND_ERROR */

    estring = &context->res_str_end;

    /*
     * If we don't already have a buffer chain to be sent...
     */
    if (context->res_buf_next == NULL)
      {
        /*
         * ... set-up estring so that ewsStringCopyIn appends strings to
         * outgoing buffer list
         */
        buffer = ewsStringAttachBuffer(estring);
        if (buffer == EWA_NET_BUFFER_NULL)
          {
            ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
            ewsNetHTTPAbort(context);
            return EWS_STATUS_NO_RESOURCES;
          }
        context->res_buf_next = buffer;
      }

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES

    /*
     * If error case
     */
    if (status == ews_http_status_416)
      {
        acceptRanges = TRUE;
        instanceLength = context->instanceLength;
      }

    /*
     * Check to see if response is for byte range and fill in locals
     * for header generation.
     */
#   ifdef EW_CONFIG_FILE_METHODS
    /*
     * If filesystem document...
     */
    if (   document != EWS_DOCUMENT_NULL
        && (document->flags & EWS_FILE_FLAG_FILESYSTEM))
      {
        /*
         * Get range parameters from file info
         */
        acceptRanges = document->fileParams->fileInfo.acceptRanges;
        isRange = document->fileParams->fileInfo.isRange;

        /*
         * If document configured for range
         */
        if (isRange)
          {

            /*
             * If status good, change status to partial content and set
             * locals for correct generation of Content-Range: header.
             * By default, we assume error and copy only instance length
             * if known.
             */
            if (   status[0] == '2'
                && status[1] == '0'
                && status[2] == '0')
              {
                status = ews_http_status_206;
                firstBytePos = document->fileParams->fileInfo.firstBytePos;
                lastBytePos = document->fileParams->fileInfo.lastBytePos;
              }
          }
        if (isRange || acceptRanges)
          {
            instanceLength = document->fileParams->fileInfo.instanceLength;
          }
      }
#   endif /* EW_CONFIG_FILE_METHODS */
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#   ifdef EW_CONFIG_OPTION_SMTP
    if (context->smtp != NULL)
      {
        return ewsSmtpSendHeader(context, document, status, string);
      }
#   endif /* EW_CONFIG_OPTION_SMTP */

#   ifdef EW_CONFIG_OPTION_TELNET
    if (context->telnet != NULL)
      {
        return ewsTelnetSendHeader(context, document, status, string);
      }
#   endif /* EW_CONFIG_OPTION_TELNET */

    /*
     * If the protocol wasn't specified, it must be HTTP/0.9.  In this case,
     * don't send any headers and just return.
     */
    if (context->req_protocol_maj == 0)
      {
        return EWS_STATUS_OK;
      }

IF_NOT_CLIENT
  {
#   ifdef EW_CONFIG_OPTION_AUTH
    /*
     * Special case:
     *
     * Authentication error status 401 is returned by invoking ewsSendHeader()
     * instead of ewsSendHeaderAndEnd() so that WWW-Authenticate: headers can
     * be added to the list of headers before sending the body of the message.
     * In this case, we pass ews_http_string_401 to ewsSendHeader so that we
     * can determine if it leads with a '/', and if so, set up the message
     * body to come from the archive.  In this case, we set document to the
     * 401 status response document to generate headers appropriately.  In any
     * case, we reset string to NULL because we don't actually want to emit
     * the string yet (the caller will add additional authentication headers
     * first).
     */
    if (string == ews_http_string_401)
      {
        if (*string == '/')
          {
            uintf i = ewsFileHash(string);
            document = ewsFileHashLookup(i, string);
            if (document != EWS_DOCUMENT_NULL)
              {
            ewsServeInclude( context, document );
              }
#           ifdef EMWEB_SANITY
            else
              {
                EMWEB_WARN(("ewsSendHeader: 401 url %s not found\n", string));
              }
#           endif /* EMWEB_SANITY */
          }

#       ifdef EW_CONFIG_OPTION_PERSISTENT
        else
          {
            /*
             * Because the 'string' may have a body of unknown length
             * we must use the connection close to signal the end.
             */
            context->keep_alive = FALSE;
          }
#       endif /* EW_CONFIG_OPTION_PERSISTENT */

        string = NULL;
      }
#  endif /* EW_CONFIG_OPTION_AUTH */

    /*
     * Send protocol version and status line
     */
#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    (void) ewsStringCopyIn(estring, "HTTP/1.1");
#   else
    (void) ewsStringCopyIn(estring, "HTTP/1.0");
#   endif

    /*
     * If 'status' is NULL, this is a response, and the space after the
     * HTTP version and the status are omitted, since the HTTP version is
     * the end of the request line.
     */
    if (NULL != status)
      {
        (void) ewsStringCopyIn(estring, " ");
        (void) ewsStringCopyIn(estring, status);
      }
  }

#   ifdef EW_CONFIG_OPTION_DATE
    /*
     * Send Date: header
     */
    (void) ewsStringCopyIn(estring, "\r\nDate: ");
    (void) ewsStringCopyIn(estring, ewaDate());
#   endif /* EW_CONFIG_OPTION_DATE */

IF_NOT_CLIENT
  {
    /*
     * Send Server: header
     */
    (void) ewsStringCopyIn(estring, "\r\nServer: ");
    (void) ewsStringCopyIn(estring, EmWebId);
  }

#   ifdef EW_CONFIG_OPTION_PERSISTENT

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    /*
     * If HTTP/1.1 or later client
     */
    if (context->req_protocol_maj > 1 ||
        (context->req_protocol_maj == 1 && context->req_protocol_min >= 1))
      {
        /*
         * Signal connection close if close needed
         */
        if (!context->keep_alive)
          {
            (void) ewsStringCopyIn(estring, "\r\nConnection: close");
          }
#ifdef EW_CONFIG_OPTION_CLIENT
IF_CLIENT
  {
       if (context->keep_alive)
         {
            (void) ewsStringCopyIn(estring, "\r\nConnection: Keep-Alive");
         }
  }
#endif /* EW_CONFIG_OPTION_CLIENT */
      }

    /*
     * Otherwise, HTTP/1.0 or 0.9
     */
    else
#   endif
      {
        /*
         * Send connection header from keep-alive flag for persistent
         * connections
         */
        if (context->keep_alive)
          {
            (void) ewsStringCopyIn(estring, "\r\nConnection: Keep-Alive");
          }
        else if (context->have_connection_header)
          {
            (void) ewsStringCopyIn(estring, "\r\nConnection: close");
          }
      }
#   else /* EW_CONFIG_OPTION_PERSISTENT */

#   if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    /*
     * If HTTP/1.1 without persistent connection support, generate close
     * on each response for conformance with specification
     */
    (void) ewsStringCopyIn(estring, "\r\nConnection: close");
#   endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

#   endif /* EW_CONFIG_OPTION_PERSISTENT */

#   ifdef EW_CONFIG_OPTION_CHUNKED_OUT
    if (ewsContextChunkOutNewChunk == context->chunk_out_state)
      {
        (void) ewsStringCopyIn(estring, "\r\nTransfer-Encoding: chunked");
      }
#   endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

    /*
     * If document is present, send document-specific headers
     */
    if (document != EWS_DOCUMENT_NULL)
      {
        boolean dynamic;
        int32  contentLength;
        const char    *contentType;
#       ifdef EW_CONFIG_OPTION_LAST_MODIFIED
        const char    *lastModified;
#       endif /* EW_CONFIG_OPTION_LAST_MODIFIED */
#       if !defined(EW_CONFIG_OPTION_DATE) && defined(EW_CONFIG_OPTION_EXPIRE)
        const char    *zeroDate = "Thu, 01 Jan 1970 00:00:00 GMT";
#       endif /* EW_CONFIG_OPTION_DATE */
        const char    *contentLanguage = NULL;
        const char    *contentEncoding = NULL;
#       ifdef EW_CONFIG_OPTION_DISPOSITION
        const char    *contentDisposition = NULL;
#       endif /* EW_CONFIG_OPTION_DISPOSITION */
        uint32        attr_block_offset;
        const char    *str;
#       if defined(EW_CONFIG_OPTION_COOKIES)\
        && defined(EW_CONFIG_OPTION_SUPPORT_ARCH1_0)
        EwsArchiveHeader *ahp = (EwsArchiveHeader *)document->archive_data;
#       endif
#       ifdef EW_CONFIG_OPTION_CACHE_ETAG
        const char *eTag = NULL;
#       endif

        /*
         * Determine content length (will be EWS_CONTENT_LENGTH_UNKNOWN
         * if dynamic).
         */
        contentLength = ewsGetDocLength( context, document );

        /*
         * If the document is an entry in the archive, get its info
         * from the archive structures.  Otherwise, it's a file in the
         * local application's filesystem.  Get the info from the
         * fileParameter structure.
         */
#       ifdef EW_CONFIG_FILE_METHODS
        if (document->flags & EWS_FILE_FLAG_FILESYSTEM)
          {
            dynamic = document->fileParams->fileInfo.nocache;
            contentType = document->fileParams->fileInfo.contentType;
#           ifdef EW_CONFIG_OPTION_LAST_MODIFIED
            lastModified = document->fileParams->fileInfo.lastModified;
#           endif /* EW_CONFIG_OPTION_LAST_MODIFIED */
            contentLanguage = document->fileParams->fileInfo.contentLanguage;
#           ifdef EW_CONFIG_OPTION_DISPOSITION
            contentDisposition = document->fileParams->fileInfo.contentDisposition;
#           endif /* EW_CONFIG_OPTION_DISPOSITION */
            contentEncoding = document->fileParams->fileInfo.contentEncoding;
          }
        else
#       endif /* EW_CONFIG_FILE_METHODS */
          {
            /*
             * Dynamic documents have nodes
             * and no <EMWEB_STATIC> tags
             */
            dynamic = ewsDocumentIsDynamic( document );

            contentType = (const char *) &document->
              archive_data[EW_BYTES_TO_UINT32(document->
                                              doc_header->mime_offset)];

#           ifdef EW_CONFIG_OPTION_LAST_MODIFIED
            lastModified = ewsDocumentArchiveDate(document->archive_data);
#           endif /* EW_CONFIG_OPTION_LAST_MODIFIED */

            contentEncoding = context->content_encoding;
#           ifdef EW_CONFIG_OPTION_CONVERT_XML
            /*
             * If we are converting an html document to XML, then
             * set the contentType to be text/xml.  This is used
             * as the mime type which is sent in the Content-Type: header
             * of the response.
             */
            if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_XML)
              {
                contentType = EWA_CONVERT_XML_CONTENT_TYPE;
              }
#           endif /* EW_CONFIG_OPTION_CONVERT_XML */
#           ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
            /*
             * If we are converting an html document to its skeleton, then
             * set the contentType to be text/xml.  This is used
             * as the mime type which is sent in the Content-Type: header
             * of the response.
             */
            if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_SKELETON)
              {
                contentType = EWA_CONVERT_SKELETON_CONTENT_TYPE;
              }
#           endif /* EW_CONFIG_OPTION_CONVERT_SKELETON */
#           ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
            /*
             * If we are embedding a document in a textarea, then
             * set the contentType to be HTML.  This is used
             * as the mime type which is sent in the Content-Type: header
             * of the response.
             * Note this test must be after the CONVERT_XML and SKELETON tests,
             * since we might be sending one of those inside a TEXTAREA,
             * and in that case, Content-Type: should be text/html.
             */
            if (context->flags & EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA)
              {
                contentType = EWA_CONVERT_TEXTAREA_CONTENT_TYPE;
              }
#           endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */
          }

        /*
         * If fixed document and not chunked, send Content-Length:
         */
        if (   contentLength != EWS_CONTENT_LENGTH_UNKNOWN
#           ifdef EW_CONFIG_OPTION_CHUNKED_OUT
            && ewsContextChunkOutNewChunk != context->chunk_out_state
#           endif /* EW_CONFIG_OPTION_CHUNKED_OUT */
           )
          {
            (void) ewsStringCopyIn(estring, "\r\nContent-Length: ");
            EMWEB_LTOA(ltoa_ptr
                       ,(uint32) contentLength
                       ,ltoa_string
                       ,sizeof(ltoa_string));
            (void) ewsStringCopyIn(estring, ltoa_ptr);
          }

        /*
         * Send Content-Type: header
         */
        if (contentType != NULL)
          {
            (void) ewsStringCopyIn(estring, "\r\nContent-Type: ");
            (void) ewsStringCopyIn(estring, contentType );

            /* get charset if available */
            attr_block_offset = ewsGetAttrBlkOffset(document);

            str = (char *)ewsGetAttr(document, attr_block_offset,
                                     ewAttrCharset, NULL);

            if (str != NULL)
              {
                (void) ewsStringCopyIn(estring, "; charset=");
                (void) ewsStringCopyIn(estring, str);
              }
          }

        /*
         * send content language if language is specified
         */
        if (contentLanguage == NULL)
          {
            /* if not specified because of local filesystem
             * check if language specified in attribute data block
             */
            attr_block_offset = ewsGetAttrBlkOffset(document);

            contentLanguage = (char *)ewsGetAttr(document, attr_block_offset, ewAttrLanguage, NULL);
          }

        if (contentLanguage != NULL)
          {
            (void) ewsStringCopyIn( estring, "\r\nContent-Language: ");
            (void) ewsStringCopyIn( estring, contentLanguage );
          }

#       ifdef EW_CONFIG_OPTION_DISPOSITION
        /*
         * send content disposition if specified
         */
        if (contentDisposition != NULL)
          {
            (void) ewsStringCopyIn( estring, "\r\nContent-Disposition: attachment; filename=\"");
            (void) ewsStringCopyIn( estring, contentDisposition );
            (void) ewsStringCopyIn( estring, "\"");
          }
#       endif /* EW_CONFIG_OPTION_DISPOSITION */

        /*
         * send content encoding if encoding is specified
         */
        if (contentEncoding == NULL)
          {
            /* if not specified because of local filesystem
             * check if encoding specified in attribute data block
             */
            attr_block_offset = ewsGetAttrBlkOffset(document);

            contentEncoding = (char *)ewsGetAttr(document, attr_block_offset, ewAttrEncoding, NULL);
          }

        if (contentEncoding != NULL)
          {
            (void) ewsStringCopyIn( estring, "\r\nContent-Encoding: ");
            (void) ewsStringCopyIn( estring, contentEncoding );
          }

#       ifdef EW_CONFIG_OPTION_EXPIRE
IF_NOT_CLIENT
  {
        /*
         * If dynamic, send Expires: header with current time
         */
        if (dynamic)
          {
#           ifdef   EW_CONFIG_OPTION_DYNAMIC_ETAG
            /* do not send Expires header for docs with explicit etag */
            if ( ! ( document->doc_header->hdr_flags
                    & EW_ARCHIVE_DOC_HDR_DYN_ETAG_FLG
                    )
                )
#           endif
              {
                (void) ewsStringCopyIn(estring, "\r\nExpires: ");
#               ifdef EW_CONFIG_OPTION_DATE
                (void) ewsStringCopyIn(estring, ewaDate());
#               else /* !EW_CONFIG_OPTION_DATE */
                (void) ewsStringCopyIn(estring, zeroDate);
#               endif /* EW_CONFIG_OPTION_DATE */
              }
          }
  }
#       endif /* EW_CONFIG_OPTION_EXPIRE */

#       ifdef EW_CONFIG_OPTION_CACHE_ETAG
IF_NOT_CLIENT
  {
        eTag = ewsRetrieveEtag(context, document); /*  NULL => send no etag */
        if (eTag!=NULL && eTag[0] != '\0')
          {
            /* Copy in as ETag: \"<etag value>\" - quoted-string*/
            (void)ewsStringCopyIn(estring, "\r\nETag: \"");
            (void)ewsStringCopyIn(estring, eTag);
            (void)ewsStringCopyIn(estring, "\"");
          }
  }
#       endif /* EW_CONFIG_OPTION_CACHE_ETAG */

#       ifdef EW_CONFIG_OPTION_LAST_MODIFIED
IF_NOT_CLIENT
  {
        /*
         * Send Last-Modified: header.  Dynamic documents use current time,
         * static documents use archive creation time
         */
#       ifdef EW_CONFIG_OPTION_DATE
        (void) ewsStringCopyIn(estring, "\r\nLast-Modified: ");
        (void) ewsStringCopyIn(estring
                              ,(dynamic || lastModified == NULL)
                               ? ewaDate()
                               : lastModified
                               );
#       else /* EW_CONFIG_OPTION_DATE */
        /*
         * If no date support, only generate last modified for static docs
         */
        if (!dynamic && lastModified != NULL)
        {
          (void) ewsStringCopyIn(estring, "\r\nLast-Modified: ");
          (void) ewsStringCopyIn(estring, lastModified);
        }
#       endif /* EW_CONFIG_OPTION_DATE */
  }
#       endif /* EW_CONFIG_OPTION_LAST_MODIFIED */

#       ifdef EW_CONFIG_OPTION_CACHE_CONTROL
IF_NOT_CLIENT
  {
        boolean cache_auth = TRUE;
        boolean comma = TRUE;
        /*
         * Send Cache-Control: header.
         * Indicate "public" for static documents in NULL realm,
         * "private" for static documents protected by non-NULL realm.
         * We define defaults for
         * EW_CONFIG_OPTION_ CACHE_CONTROL_DYNAMIC to be "no-cache"
         * for EW_CONFIG_OPTION_CACHE_CONTROL_STATIC to be "max-age=3600"
         * These may be changed at the app. developer's discretion.
         */

        (void) ewsStringCopyIn(estring, "\r\nCache-Control: ");
        if (dynamic)
          {
#           ifdef   EW_CONFIG_OPTION_DYNAMIC_ETAG
            if (   (  document->doc_header->hdr_flags
                    & EW_ARCHIVE_DOC_HDR_DYN_ETAG_FLG
                    )
                )
              {
                switch ( context->max_age )
                  {
                  case EWS_CACHE_UNLIMITED:
                    comma = FALSE;
                    break;
                  case EWS_CACHE_REVALIDATE:
                    (void) ewsStringCopyIn(estring, "must-revalidate");
                    cache_auth = FALSE;
                    break;
                  default:
                    (void) ewsStringCopyIn(estring, "max-age=");
                    EMWEB_LTOA(ltoa_ptr
                               ,(uint32) context->max_age
                               ,ltoa_string
                               ,sizeof(ltoa_string));
                    (void) ewsStringCopyIn(estring, ltoa_ptr);
                    break;
                  }
              }
            else
#           endif  /* EW_CONFIG_OPTION_DYNAMIC_ETAG */
              {
#               ifdef   EW_CONFIG_OPTION_CACHE_CONTROL_DYNAMIC
                (void) ewsStringCopyIn(estring,
                                       EW_CONFIG_OPTION_CACHE_CONTROL_DYNAMIC);
#               else  /* EW_CONFIG_OPTION_CACHE_CONTROL_DYNAMIC */
                (void) ewsStringCopyIn(estring, "no-cache");
#               endif /* EW_CONFIG_OPTION_CACHE_CONTROL_DYNAMIC */
                cache_auth = FALSE;
              }
          }
        else
          {
#           ifdef   EW_CONFIG_OPTION_CACHE_CONTROL_STATIC
            (void) ewsStringCopyIn(estring,
                                   EW_CONFIG_OPTION_CACHE_CONTROL_STATIC);
#           endif /* EW_CONFIG_OPTION_CACHE_CONTROL_STATIC */
          }

        if ( cache_auth )
          {
            if ( comma )
              {
                (void) ewsStringCopyIn(estring,", ");
              }
#           ifdef EW_CONFIG_OPTION_AUTH
            (void) ewsStringCopyIn( estring
                                   ,document->realmp == NULL
                                   ? "public" : "private");
#           else /* EW_CONFIG_OPTION_AUTH */
            (void) ewsStringCopyIn(estring, "public");
#           endif /* EW_CONFIG_OPTION_AUTH */
          }
  }
#       endif /* EW_CONFIG_OPTION_CACHE_CONTROL */

#       ifdef EW_CONFIG_OPTION_PRAGMA_NOCACHE
IF_NOT_CLIENT
  {
        if (   dynamic
                                /* !TBD!
                                   only send Pragma no-cache to 1.0 clients */
            && (   context->req_protocol_maj == 1
                && context->req_protocol_min == 0
                )
            )
          {
            (void) ewsStringCopyIn(estring, "\r\nPragma: no-cache");
          }
  }
#       endif /* EW_CONFIG_OPTION_PRAGMA_NOCACHE */

#       ifdef EW_CONFIG_OPTION_COOKIES
IF_NOT_CLIENT
  {
        if ((context->session != NULL) && (context->session->sendCookie == L7_TRUE))
        {
          L7_BOOL validCookie = L7_FALSE;
          if (context->session->type == L7_LOGIN_TYPE_HTTPS)
          {
            (void) ewsStringCopyIn(estring, "\r\nSet-Cookie: SIDSSL=");
            validCookie = L7_TRUE;
          }
          else if (context->session->type == L7_LOGIN_TYPE_HTTP)
          {
            (void) ewsStringCopyIn(estring, "\r\nSet-Cookie: SID=");
            validCookie = L7_TRUE;
          }
          else
          {
            validCookie = L7_FALSE;
          }
          if (validCookie == L7_TRUE)
          {
            (void) ewsStringCopyIn(estring, context->session->sid);  
            (void) ewsStringCopyIn(estring, ";PATH=/");
          }
          context->session->sendCookie = L7_FALSE;
        }

        /*
         * If there are any cookie header nodes, add 'Set-Cookie' header(s).
         * Do it only if archive revision is >= 1.1
         */
#       if defined(EW_CONFIG_OPTION_SUPPORT_ARCH1_0)
        if (ahp != NULL && ((ahp->version_maj > 1) ||
            (ahp->version_min >= 1)))
#       endif /* EW_CONFIG_OPTION_SUPPORT_ARCH1_0 */
          {
            EwsDocumentNode *np;
            const EwCookieAttributes *cap;
            uint32 offset;
            uint16 node_cnt;
            const void *vp;

            if (document->doc_header->hdr_flags & EW_ARCHIVE_DOC_HDR_COOKIE_FLG)
              {                         /* there are cookies to set             */
                offset = EW_BYTES_TO_UINT32(document->doc_header->node_offset);
                node_cnt = EW_BYTES_TO_UINT16(document->doc_header->node_count);
                for (np = (EwsDocumentNode *)
                     ((uint8 *)(&document->archive_data[offset]) +
                                node_cnt * SIZEOF_EWS_DOCUMENT_NODE);
                     np->type != EW_DOCUMENT_NODE_TYPE_COOKIE;
                     np = NEXT_DOCNODE(np));
                while (np->type == EW_DOCUMENT_NODE_TYPE_COOKIE)
                  {                             /* process all cookie hdr nodes */
                    vp = (*document->archive->emweb_string) (context, EW_BYTES_TO_UINT32(np->index));
                    if (vp != NULL)
                      {
                        cap = &(document->archive->emweb_cookie_tbl[EW_BYTES_TO_UINT32(np->data_offset)]);
                        (void) ewsStringCopyIn(estring, "\r\nSet-Cookie: ");
                        (void) ewsStringCopyIn(estring, cap->name);
                        (void) ewsStringCopyIn(estring, "=");
                        (void) ewsStringCopyIn(estring, (char *)vp);
                        if (cap->path != NULL)
                          {
                            (void) ewsStringCopyIn(estring, ";PATH=");
                            (void) ewsStringCopyIn(estring, cap->path);
                          }
                        if (np->attributes & EW_DOCUMENT_HDR_SECURE)
                            (void) ewsStringCopyIn(estring, ";SECURE");
                      }
                    np = NEXT_DOCNODE(np);
                  }
              }
          }
  }
#       endif /* EW_CONFIG_OPTION_COOKIES */

#     ifdef EW_CONFIG_OPTION_AUTH_DIGEST
IF_NOT_CLIENT
  {
      /*
       * Generate AuthenticationInfo: header
      if (context->noncep != NULL)
        {
          (void) ewsStringCopyIn(estring, "\r\nAuthentication-info: nextnonce=\"");
          (void) ewsStringCopyIn(estring, context->noncep->nonce_value);
          (void) ewsStringCopyIn(estring, "\"");
        }
       */
  }
#     endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
      }
#   ifdef EW_CONFIG_OPTION_METHOD_OPTIONS
    else if (context->req_method == ewsRequestMethodOptions)
      {
        ewsStringCopyIn(estring, "\r\nContent-Length: 0");
      }
#   endif

#   ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
IF_NOT_CLIENT
  {

    /*
     * Generate Content-Location: header for negotiated responses
     */
    if (context->tcn_hdr != ewsTcnHdrNone)
      {
        (void) ewsStringCopyIn(estring, "\r\nContent-Location: ");
        (void) ewsStringCopyIn(estring, context->url);
      }

    /*
     * Generate TCN: header
     */
    if (context->tcn_hdr == ewsTcnHdrChoice)
      {
        (void) ewsStringCopyIn(estring, "\r\nTCN: choice ");
      }
    else if (context->tcn_hdr == ewsTcnHdrList)
      {
        (void) ewsStringCopyIn(estring, "\r\nTCN: list ");
      }

    /*
     * send the Alternates header if there is one
     */
    if (context->alt_hdr)
      {
        (void) ewsStringCopyIn(estring, "\r\n");
        (void) ewsStringCopyIn(estring, context->alt_hdr);
      }

    /*
     * send the Vary header if there is one
     */
    if (context->vary_hdr != 0)
      {
        (void) ewsStringCopyIn(estring, context->vary_hdr);
      }

  }
#   endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

#   if defined EW_CONFIG_OPTION_AUTH_DIGEST
IF_NOT_CLIENT
  {
    if (context->send_auth_info_hdr == TRUE)
      {
        /*
         * we have to add Authentication-info header
         */
        (void) ewsStringCopyIn(estring, "\r\n");
        ewsAuthenticationInfoHeadOrTrail(context);
        context->send_auth_info_hdr = FALSE;
      }
  }
#   endif

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES
IF_NOT_CLIENT
  {
    if (acceptRanges)
      {
        (void) ewsStringCopyIn(estring, "\r\nAccept-Ranges: bytes");
      }
    else
      {
        (void) ewsStringCopyIn(estring, "\r\nAccept-Ranges: none");
      }
    if (isRange || acceptRanges)
      {
        (void) ewsStringCopyIn(estring, "\r\nContent-Range: bytes ");
        if (lastBytePos == EWS_CONTENT_LENGTH_UNKNOWN)
          {
            (void) ewsStringCopyIn(estring, "*");
          }
        else
          {
            EMWEB_LTOA(ltoa_ptr
                      ,firstBytePos
                      ,ltoa_string
                      ,sizeof(ltoa_string));
            (void) ewsStringCopyIn(estring, ltoa_ptr);
            (void) ewsStringCopyIn(estring,"-");
            EMWEB_LTOA(ltoa_ptr
                      ,lastBytePos
                      ,ltoa_string
                      ,sizeof(ltoa_string));
            (void) ewsStringCopyIn(estring, ltoa_ptr);
          }
        if (instanceLength == (uint32) EWS_CONTENT_LENGTH_UNKNOWN)
          {
            (void) ewsStringCopyIn(estring, "/*");
          }
        else
          {
            (void) ewsStringCopyIn(estring, "/");
            EMWEB_LTOA(ltoa_ptr
                      ,instanceLength
                      ,ltoa_string
                      ,sizeof(ltoa_string));
            (void) ewsStringCopyIn(estring, ltoa_ptr);
          }
      }
  }
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */


#   ifdef  EW_CONFIG_OPTION_UPNP
#   ifdef  EW_CONFIG_OPTION_SOAP
IF_NOT_CLIENT
  {
    if (context->upnp_have_headers & UPNP_MAN_HEADER)
      {
        (void) ewsStringCopyIn(estring, "\r\nEXT:");
      }
  }
# endif  /* EW_CONFIG_OPTION_SOAP */
# endif  /* EW_CONFIG_OPTION_UPNP */

    /*
     * Terminate last header line.  (This is not the break between headers
     * and data)
     *
     * NOTE: We set 'buffer' to the return of ewsStringCopyIn to
     * detect if we ran out of buffers while building the headers.  We
     * only test the value of 'buffer' returned by the final call of
     * ewsStringCopyIn to see if an error occurred.  It is possible
     * that an error occurred earlier when writing other headers, but
     * due to its implementation, if ewsStringCopyIn fails for any one
     * string, it will fail for all later strings until some buffers
     * have been returned to the buffer pool.  And since these
     * portions of EmWeb run as a single thread, no buffers can be
     * freed while this function is running.
     */
    buffer = ewsStringCopyIn(estring, "\r\n");

    /*
     * Append extra information if present
     */
    if (string != NULL)
      {
        buffer = ewsStringCopyIn(estring, string);
      }
    /*
     * Test to see if we ran out of buffers while writing headers.
     */
    if (buffer == EWA_NET_BUFFER_NULL)
      {
        ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
        ewsNetHTTPAbort(context);
        return EWS_STATUS_NO_RESOURCES;
      }

    return EWS_STATUS_OK;
}

/*
 * ewsSendStatusString
 * This function processes the HTTP status string, usually passed to
 * ewsSendHeaderAndEnd, but sometimes (for authentication) after ewsSendHeader
 * has already been invoked.  If the first character of the string is '/',
 * then set up a response body from the archive using string as the URL.
 * Otherwise, the string contains text (including the blank line after headers)
 * to be transmitted.  (Note that in the authentication case, we've already
 * pushed the response document on the stack).
 *
 * context     - Context of request
 * string      - status string or URL
 * status      - status string if sending header, or NULL if header already sent
 */
void
ewsSendStatusString( EwsContext context
                   , const char *string
                   , const char *status )
{
  uintf i;
  EwsDocument document;

  /*
   * If string[0] == '/', then treat string as a URL of an internal
   * archive document containing the body of the response.
   */
  if (*string == '/')
    {
      if (status != NULL)
        {
          i = ewsFileHash(string);
          document = ewsFileHashLookup(i, string);
          if (document != EWS_DOCUMENT_NULL)
            {
              ewsServeInclude( context, document );
              ewsSendHeader( context, document, status, "\r\n" );
            }
#         ifdef EMWEB_SANITY
          else
            {
              EMWEB_WARN(("ewsSendStatusString: %s not found\n", string));
              ewsSendHeader( context, document, status, "\r\n" );
            }
#         endif /* EMWEB_SANITY */
        }
      else
        {
          ewsStringCopyIn( &context->res_str_end, "\r\n" );
        }
      context->state = ewsContextStateServingDocument;
      return;
    }

# ifdef EW_CONFIG_OPTION_PERSISTENT
  /*
   * Because the 'string' may have a body of unknown length
   * we must use the connection close to signal the end.
   */
  context->keep_alive = FALSE;
# endif /* EW_CONFIG_OPTION_PERSISTENT */
  if (status != NULL)
    {
      ewsSendHeader( context, EWS_DOCUMENT_NULL, status ,string);
    }
  else
    {
      ewsStringCopyIn(&context->res_str_end, string);
    }
  ewsInternalFinish(context);
}

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
void
ewsSendHeaderAndEnd
  ( EwsContext context, const char * status, const char *string )
{

# ifdef EW_CONFIG_OPTION_PRIMARY
  /*
   * If this is secondary context, abort processing of secondary request,
   * abort transaction with user agent error.
   */

  if (context->is_secondary_context)
    {
      ewsDistFinish(context->secondary, EWS_STATUS_ABORTED);
      return;
    }
# endif /* EW_CONFIG_OPTION_PRIMARY */

# if defined EW_CONFIG_OPTION_AUTH_DIGEST

  /*
   * the following MUST be done BEFORE ewsSendHeader() is called,
   * so the Authentication-info header will be generated
   */
  if (context->auth_state == ewsAuthStateOKDigest)
    {
      context->send_auth_info_hdr = TRUE;
    }
# endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

  if (   ( context->req_method == ewsRequestMethodHead )
      || ( string == ews_no_body )
      )
    {
      /*
       * There is no body, so see if we can keep the connection
       */
      ewsSendHeader( context, EWS_DOCUMENT_NULL, status
                    /* Prevent any body
                     * this also has the possibly unfortunate
                     * effect of removing any additional headers
                     * that the customer specified response would
                     * have if this were a GET, but hey...
                     */
                    ,ews_no_body
                    );
      ewsInternalFinish(context);
    }
  else
    {
      ewsSendStatusString(context, string, status);
    }
}

#if defined(EW_CONFIG_OPTION_FORM) \
    || defined(EW_CONFIG_OPTION_CGI) \
    || defined(EW_CONFIG_OPTION_LINK) \
    || defined(EW_CONFIG_OPTION_CONTEXT_SEND_REDIRECT)
/*
 * ewsSendRedirect
 * Send a redirection header.
 *
 * context      - context of request
 * url          - redirection URL (may be absolute or relative)
 * type         - ewsRedirectTemporary | Permanent | SeeOther
 *
 * No return value
 */
void
ewsSendRedirect ( EwsContext context, const char *url, EwsRedirectType type )
{
  const char *ccp;
  char *cp = NULL;
  const char *net_loc;
  char *rel_url;
  boolean need_scheme;
  uint32 bytes;
  char cl_buf[11];
  char *cl_ptr;

    /*
     * Send redirection status
     */
    switch (type)
      {
        case ewsRedirectPermanent:
          ewaLogHook(context, EWS_LOG_STATUS_MOVED_PERMANENTLY);
          cp = (char *)"301 Moved Permanently";
          break;
        case ewsRedirectSeeOther:
#         if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
          if (context->req_protocol_maj > 1 ||
              (context->req_protocol_maj == 1 &&
               context->req_protocol_min >= 1))
            {
              ewaLogHook(context, EWS_LOG_STATUS_SEE_OTHER);
              cp = (char *)"303 See Other";
              break;
            }
#         endif /* HTTP/1.1 */

          /* if HTTP/1.0, fall through to RedirectTemporary case */

        case ewsRedirectTemporary:
          ewaLogHook(context, EWS_LOG_STATUS_MOVED_TEMPORARILY);
          cp = (char *)"302 Moved Temporarily";
          break;

#       ifdef EMWEB_SANITY
        default:
          EMWEB_ERROR(("ewsSendRedirect: bad type %d\n", type));
          return;
#       endif /* EMWEB_SANITY */
      }

    /*
     * Send headers
     */
    if (ewsSendHeader(context, EWS_DOCUMENT_NULL, cp, "Location: ") !=
        EWS_STATUS_OK)
      {
        return;
      }

    /*
     * Build components of absolute URL.  The final URL will be:
     *
     *   scheme + net_loc + rel_url + url
     *
     * If relative path URL
     */
    if (*url == '/')
      {
        need_scheme = TRUE;
        net_loc = ewaNetLocalHostName(context);
        rel_url = NULL;
      }

    /*
     * Otherwise, see if this is an absolute or relative URL
     */
    else
      {

        /*
         * Look for first ':' or '/' character to help determine if url is
         * absolute or relative.
         */
        for (ccp = url; *ccp != '\0'; ccp++)
          {
            if (*ccp == ':' || *ccp == '/')
              {
                break;
              }
          }

        /*
         * If we found a ':' before a '/' or end of string, then this is an
         * absolute URL (the ':' must be a delimiter between the scheme and the
         * network path, otherwise this URL is invalid, and we don't want to
         * spend cycles checking for syntax).
         */
        if (*ccp == ':')
          {
            need_scheme = FALSE;
            net_loc = NULL;
            rel_url = NULL;
          }

        /*
         * Otherwise, this is a relative URL.  We need to find the base path
         * of the requested URL.
         */
        else
          {
            /*
             * Get the requested url
             */
            rel_url = context->url;
            EMWEB_STRLEN(bytes, rel_url);

            /*
             * Scan backwards until we find a '/'.  We add a '\0' immediately
             * afterward.  This becomes the new path name to which we are
             * relative.  We are guaranteed to find a '/' as this is an
             * absolute path URL that begins with at least a '/'.
             */
            for (cp = rel_url + bytes - 1; *cp != '/'; cp--)
              ;

            /*
             * If ACTION URL, strip ACTION
             */
            if ((context->flags & EWS_CONTEXT_FLAGS_ACTION_URL) != 0)
              {
                for (cp--; *cp != '/'; cp--)
                  ;
              }

            cp[1] = '\0';
            net_loc = ewaNetLocalHostName(context);
            need_scheme = TRUE;
          }
      }

    /*
     * Generate Location: header (scheme + net_loc + rel_url + url)
     */
    ewsStringLengthSet(&context->res_str_end, 0);
    if (need_scheme)
      {
#       ifdef EW_CONFIG_OPTION_SSL_RSA
        if (context->ssl_connection == TRUE)
          {
            (void) ewsStringCopyIn(&context->res_str_end, "https://");
          }
        else
          {
            (void) ewsStringCopyIn(&context->res_str_end, "http://");
          }
#       else
        if (context->net_handle->connection_type == EW_CONNECTION_HTTPS)
        {
          (void) ewsStringCopyIn(&context->res_str_end, "https://");
        }
        else
        {
          (void) ewsStringCopyIn(&context->res_str_end, "http://");
        }

#       endif
      }
    if (net_loc != NULL)
      {
        (void) ewsStringCopyIn(&context->res_str_end, net_loc);
      }
    if (rel_url != NULL)
      {
        (void) ewsStringCopyIn(&context->res_str_end, rel_url);
      }
    (void) ewsStringCopyIn(&context->res_str_end, url);
    bytes = ewsStringLength(&context->res_str_end);

    /*
     * If HEAD method, we're done.
     */
    if (context->req_method == ewsRequestMethodHead)
      {
#       if defined EW_CONFIG_OPTION_AUTH_DIGEST
        if (context->auth_state == ewsAuthStateOKDigest)
          {
            (void) ewsStringCopyIn(&context->res_str_end, "\r\n");
            ewsAuthenticationInfoHeadOrTrail(context);
          }
#       endif /* EW_CONFIG_OPTION_AUTH_DIGEST */
        (void) ewsStringCopyIn(&context->res_str_end, "\r\n\r\n");
        ewsInternalFinish(context);
        return;
      }

    /*
     * Generate a message body
     */
    (void) ewsStringCopyIn(&context->res_str_end,
      "\r\nContent-Type: text/html\r\nContent-Length: ");
    bytes += 68;        /* 48 + 20 from body components below */
    EMWEB_LTOA(cl_ptr, bytes, cl_buf, sizeof(cl_buf));
    (void) ewsStringCopyIn(&context->res_str_end, cl_ptr);
    (void) ewsStringCopyIn(&context->res_str_end, "\r\n\r\n");
    (void) ewsBodySend( context,/* 48 */
                       "<HEAD><TITLE>Moved</TITLE></HEAD><BODY><A HREF=\"");
    if (need_scheme)
      {
#       ifdef EW_CONFIG_OPTION_SSL_RSA
        if (context->ssl_connection == TRUE)
          {
            (void) ewsBodySend( context, "https://");
          }
        else
          {
            (void) ewsBodySend( context, "http://");
          }
#       else
        if (context->net_handle->connection_type == EW_CONNECTION_HTTPS)
        {
          (void) ewsBodySend( context, "https://");
        }
        else
        {
          (void) ewsBodySend( context, "http://");
        }
#       endif
      }
    if (net_loc != NULL)
      {
        (void) ewsBodySend(context, net_loc);
      }
    if (rel_url != NULL)
      {
        (void) ewsBodySend(context, rel_url);
      }
    (void) ewsBodySend(context, url);
    (void) ewsBodySend(context
                       ,"\">Moved</A></BODY>\r\n"); /*20*/
    ewsInternalFinish(context);
}
#endif /* EW_CONFIG_OPTION_FORM | CGI | LINK */

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 &&\
    ( defined(EW_CONFIG_OPTION_FORM) ||\
      defined(EW_CONFIG_OPTION_CGI) )
/*
 * ewsSendContinue
 * This function generates an HTTP/1.1 or later 100 Continue status
 */
void
ewsSendContinue ( EwsContext context )
{
IF_NOT_CLIENT
  {
    /*
     * If HTTP/1.1 request and 100-Continue expected
     */
    if ((context->req_protocol_maj > 1 ||
          (context->req_protocol_maj == 1 && context->req_protocol_min >= 1))

#ifdef EW_CONFIG_OPTION_EXPECT
        && context->expect_100_continue
#endif

       )
      {
        /*
         * Set-up estring so that ewsStringCopyIn appends strings to outgoing
         * buffer list
         */
        context->res_buf_next = ewsStringAttachBuffer(&context->res_str_end);
        if (context->res_buf_next == EWA_NET_BUFFER_NULL)
          {
            ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
            ewsNetHTTPAbort(context);
          }
        else
          {
            ewsStringCopyIn(&context->res_str_end
                           ,"HTTP/1.1 100 Continue\r\n\r\n");
            if (ewsFlushAll(context) != EWA_STATUS_OK)
              {
                ewsNetHTTPAbort(context);
              }
          }
      }
  }
}
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && (FORM | CGI) */

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
EwaStatus
ewsFlush ( EwsContext context )
{
  EwaNetBuffer buffer;
  EwaNetBuffer buffer2;

    EMWEB_TRACE(("ewsFlush(%p)\n", context));

    /*
     * If there are no buffers or we haven't filled the first buffer yet,
     * do nothing and return good status.
     */
    buffer = context->res_buf_next;
    if (buffer == EWA_NET_BUFFER_NULL ||
        (buffer == ewsStringBuffer(&context->res_str_end) &&
         ewsStringAvailable(&context->res_str_end) != 0))
      {
        return EWA_STATUS_OK;
      }

#   ifdef EW_CONFIG_OPTION_CHUNKED_OUT
    if ( ewsContextChunkOutPartial == context->chunk_out_state )
      {
        ewsBodyUpdateChunkHeader(context);
      }
#   endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

    if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
      {
        EMWEB_TRACE(("ewsFlush - aborted\n"));
        return(EWA_STATUS_ERROR);
      }

    /*
     * If the last buffer on the buffer chain is completely filled, send
     * everything to the network.
     */
    if (buffer == ewsStringBuffer(&context->res_str_end))
      {
        EWA_TASK_LOCK();
        context->res_buf_next = EWA_NET_BUFFER_NULL;
        EWA_TASK_UNLOCK();
        ewsStringInit(&context->res_str_end, EWA_NET_BUFFER_NULL);
        return ewaNetHTTPSend(context->net_handle, buffer);
      }

    /*
     * Otherwise, follow chain of buffers until second-to-last found.  Hold on
     * to the last partial buffer until it is filled or ewsFlushAll() is
     * invoked.
     */
    for (;;)
      {
        buffer2 = ewaNetBufferNextGet(buffer);
        if (buffer2 == ewsStringBuffer(&context->res_str_end))
          {
            break;
          }
        buffer = buffer2;
      }

    /*
     * Break buffer chain and send completed buffers to network
     */
    ewaNetBufferNextSet(buffer, EWA_NET_BUFFER_NULL);
    buffer = context->res_buf_next;
    EWA_TASK_LOCK();
    context->res_buf_next = buffer2;
    EWA_TASK_UNLOCK();
    return ewaNetHTTPSend(context->net_handle, buffer);
}

#ifdef EW_CONFIG_OPTION_FLUSH_DATA
/*
 * ewsContextFlushData
 *
 * Call this function from within a emweb_string or emweb_include
 * emweb_iterate code block to cause all information to be sent
 * to the network.  This is useful if small bits of data are being
 * written out over a long time, and the app needs the screen to
 * be updated frequently.  A better approach is to suspend the
 * context between iterations.
 */
void ewsContextFlushData( EwsContext context )
{
  context->flags |= EWS_CONTEXT_FLAGS_FLUSH_DATA;
}
#endif

/*
 * ewsFlushAll
 * Flush all buffers queued for transmission and return application's
 * status from ewaHTTPSend().
 * In addition, if the context is for a UDP 'connection'
 * (context->ssdp_connection == TRUE), then the application is told that
 * this is the end of a UDP packet (via ewaNetUdpSendMsg).
 *
 * context      - Context of request
 *
 * Returns EWA_STATUS_OK, or the failure return status of ewaNetHTTPSend or
 * ewaNetUdpSendMsg, if they were called and returned a failure status.
 */
EwaStatus
ewsFlushAll ( EwsContext context )
{
  EwaNetBuffer buffer;
  EwaStatus status = EWA_STATUS_OK;

  EMWEB_TRACE(("ewsFlushAll(%p)\n", context));

  /*
   * If the context has been marked to abort, return immediately.
   */
  if (context->abort_flags & EWS_CONTEXT_FLAGS_ABORT)
    {
      EMWEB_TRACE(("ewsFlushAll - aborted\n"));
      return(EWA_STATUS_ERROR);
    }

  /*
   * If there are no buffers, most of the actions of this routine can be
   * skipped.
   */
  buffer = context->res_buf_next;
  if (EWA_NET_BUFFER_NULL != buffer)
    {
#     ifdef EW_CONFIG_OPTION_CHUNKED_OUT
      if ( ewsContextChunkOutPartial == context->chunk_out_state )
        {
          ewsBodyUpdateChunkHeader(context);
        }
#     endif /* EW_CONFIG_OPTION_CHUNKED_OUT */

      /*
       * Adjust the size of last buffer to reflect the number of characters
       * that we know are in it.
       */
      ewaNetBufferLengthSet(ewsStringBuffer(&context->res_str_end),
                            context->res_str_end.offset);

      /*
       * Clear the output saved in the context.
       */
      ewsStringInit(&context->res_str_end, EWA_NET_BUFFER_NULL);
      EWA_TASK_LOCK();
      context->res_buf_next = EWA_NET_BUFFER_NULL;
      EWA_TASK_UNLOCK();

      /*
       * Free empty buffers until a nonempty buffer found.
       * This prevents us from sending a list of empty buffers.
       */
      while (   EWA_NET_BUFFER_NULL != buffer
             && 0 == ewaNetBufferLengthGet( buffer )
                )
        {
          EwaNetBuffer freeBuf;

          freeBuf = buffer;
          buffer = ewaNetBufferNextGet( buffer );
          ewaNetBufferNextSet( freeBuf, EWA_NET_BUFFER_NULL );
          ewaNetBufferFree( freeBuf );
        }

      /*
       * Send any output to the application.
       */
      if (EWA_NET_BUFFER_NULL != buffer)
        {
          status = ewaNetHTTPSend(context->net_handle, buffer);
        }
    }

  /*
   * If context is for UDP output, we have to mark the end of the
   * response packet.
   */
# ifdef EW_CONFIG_OPTION_UPNP
  if (context->ssdp_connection && EWA_STATUS_OK == status)
    {
      /*
       * context MUST be in one of the SSDP states:
       *   ewsContextStateDoMSearch
       *   ewsContextStateNotify
       *   ewsContextStateReNotify
       */
      status = ewaNetUdpSendMsg(context->net_handle);
    }
# endif /* defined EW_CONFIG_OPTION_UPNP */

  return (status);
}

#if defined(EW_CONFIG_OPTION_CACHE_ETAG) ||\
    defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH)
/*
 * ewsRetrieveEtag
 * Retrieve an ETAG for a given document
 *
 * document     - document to get ETAG for.
 *
 * Returns the ETAG or NULL if ETAG does not exist.
 */
const char * ewsRetrieveEtag(EwsContext context, EwsDocument document)
{
  const char *eTag;  /* initialize the ETAG */
# ifdef EW_CONFIG_OPTION_SUPPORT_ARCH1_0
  EwsArchiveHeader *ahp;
# endif /* EW_CONFIG_OPTION_SUPPORT_ARCH1_0 */
# ifndef EW_CONFIG_OPTION_DYNAMIC_ETAG
  EW_UNUSED(context);
# endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */

  eTag = NULL;

  /* if the document is NULL, then it doesn't exist so just return NULL ETAG */

  if (EWS_DOCUMENT_NULL==document)
    {
      return( eTag );
    }

# ifdef EW_CONFIG_OPTION_SUPPORT_ARCH1_0
  ahp = (EwsArchiveHeader *)document->archive_data;
#endif /* EW_CONFIG_OPTION_SUPPORT_ARCH1_0 */

#   ifdef EW_CONFIG_FILE_METHODS
  if (document->flags & EWS_FILE_FLAG_FILESYSTEM)
    {
      eTag= document->fileParams->fileInfo.eTag;
      return(eTag);
    }
#   endif /* EW_CONFIG_FILE_METHODS */

  /* Now we know that we have an archive document */

# ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
  if ( NULL != context->etag )
    {
      /*
       * We already calculated the etag for this once
       * (this happens for conditional requests - this routine is called
       *  for the etag to compare against, and then again if the document
       *  is served.  This ensures that both values are the same and that
       *  the application etag code is called only once)
       */
      return context->etag;
    }
# endif

    /*
     * If there are any ETAG cache header nodes, send the ETAG with
     * the STATIC document if the archive is version 1.1.
     * Check if ETAG exists before send it.
     */
#     ifdef EW_CONFIG_OPTION_SUPPORT_ARCH1_0
  if (   (ahp->version_maj > 1)
      ||(ahp->version_min >= 1)
      )
#     endif /* EW_CONFIG_OPTION_SUPPORT_ARCH1_0 */
      {
        EwsDocumentNode *np;
        uint32 offset;
        uint32 etag_offset;
        uintf i;
        boolean foundEtag;
#       ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
        uint32 node_index;
#       endif

        /* Get the etag for this document from the ARCHIVE */
        if (  document->doc_header->hdr_flags
            & (EW_ARCHIVE_DOC_HDR_SCACHE_FLG | EW_ARCHIVE_DOC_HDR_DYN_ETAG_FLG)
            )
          {
            /* Calculate the start of the header nodes which follow
               IMMEDIATELY after the document nodes.
               */
            offset= EW_BYTES_TO_UINT32(document->doc_header->node_offset);
            offset+= (  SIZEOF_EWS_DOCUMENT_NODE
                      * EW_BYTES_TO_UINT16(document->doc_header->node_count)
                      );

            /* loop thru the header nodes and look for a cache node*/
            for (( np = (EwsDocumentNode *)&document->archive_data[offset]
                  ,i = 0
                  ,foundEtag = FALSE
                  );
                 (   ! foundEtag /* there is only one, so quit at the first */
                  && ( i < document->doc_header->hdr_node_count )
                  );
                 ( np = NEXT_DOCNODE(np)
                  ,i++
                  )
                 )
            {
                switch (np->type)
                  {
                  case EW_DOCUMENT_NODE_TYPE_STATIC_ETAG:
                    /* now get the offset into the archive to get
                       the ETAG string */
                    etag_offset = EW_BYTES_TO_UINT32(np->data_offset);
                    eTag=( const char *) &document->archive_data[etag_offset];
                    foundEtag=TRUE;
                    break;

#                 ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
                  case EW_DOCUMENT_NODE_TYPE_DYN_ETAG:
                    {
                      EwsServeSetupSubstate savedState;

                      /*
                       * Save the state so that we can check it
                       * in ewsContextCacheControl
                       */
                      savedState = context->substate.setup;
                      context->substate.setup = ewsContextServeSetupDynEtag;

                      node_index = EW_BYTES_TO_UINT32(np->index);
                      eTag = document->archive->emweb_string( context
                                                             ,( node_index
                                                               &0x00ffffff
                                                               )
                                                             );
                      /* restore the real substate */
                      context->substate.setup = savedState;

                      EMWEB_STRDUP( context->etag, eTag );
                      /* the strdup above allocates memory, so save it in the
                       * context to be freed in ewsInternalCleanup */
                      eTag = context->etag;
                      foundEtag=TRUE;
                    }
                    break;
#                 endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */

                  default:
                    /* not an etag-relevant header node */
                    break;
                  }
              }
          }
      }
  return(eTag);
}
/* END ewsRetrieveEtag  */
#endif /* EW_CONFIG_OPTION_CACHE_ETAG || CONDITIONAL_MATCH */


/*
 * ewsBodyUpdateChunkHeader
 *
 * If data (length of res_str_end) is not zero, then rewrite the "00000000"
 * to contain the actual data length (not including chunk headers) and
 * add terminating "\r\n".
 * If no data, reset res_str_end to position prior to writing "00000000"
 * and set its length to zero. Clean up any buffers that may have
 * been added to hold this chunk header.
 */
#ifdef EW_CONFIG_OPTION_CHUNKED_OUT
void ewsBodyUpdateChunkHeader( EwsContext context )
{
  uint32     chunk_len;
  char       chunk_buf[9];
  char*      chunk_ptr = NULL;
  uint32     chunk_strlen;
  EwsStringP this_chunk;

# ifdef EMWEB_SANITY
  if ( ewsContextChunkOutPartial != context->chunk_out_state )
    {
      EMWEB_ERROR(("ewsBodyUpdateChunkHeader: wrong state\n"));
    }
# endif /* EMWEB_SANITY */

  this_chunk = &context->res_str_end;

  chunk_len = ewsStringLength(this_chunk);
  if (chunk_len > 0)
    {
#     if defined EW_CONFIG_OPTION_AUTH_DIGEST
#     ifdef _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT
      /*
       *
       * update response digest
       *
       */
      ewsAuthMD5Update(&context->res_md5_ctxt, &context->res_str_end);
      context->res_entity_body = TRUE;
#     endif /* _CONFIG_OPTION_AUTH_DIGEST_AUTH_INT */
#     endif
      /*
       *  write chunk header and trailer
       */
      EMWEB_LTOAX(chunk_ptr, chunk_len, chunk_buf, sizeof(chunk_buf));
      EMWEB_STRLEN(chunk_strlen, chunk_ptr);
      while (chunk_strlen < 8)
        {
          ewsStringRewriteChar(&context->res_chunk, '0'); /* leading 0's */
          chunk_strlen++;
        }
      while (*chunk_ptr != '\0')
        {
          ewsStringRewriteChar(&context->res_chunk, *chunk_ptr++);
        }
      ewsStringCopyIn(&context->res_str_end, "\r\n");   /* end chunk */
    }
  else
    {
      EwaNetBuffer end_buffer;
      EwaNetBuffer penultimate_buffer;

      end_buffer = ewsStringBuffer( &context->res_str_end );
      /*
       * The chunk was empty, so strip its header by reseting the string
       * to where it was before we added the header
       */
      context->res_str_end = context->res_chunk;

      /* If we added a buffer to write that chunk header, take it back off */
      penultimate_buffer = ewsStringBuffer( &context->res_str_end );
      if ( end_buffer != penultimate_buffer )
        {
          ewaNetBufferNextSet(penultimate_buffer, EWA_NET_BUFFER_NULL);
          ewaNetBufferFree( end_buffer );
        }
    }

  context->chunk_out_state = ewsContextChunkOutNewChunk;
}


/*
 * ewsBodyNewChunkHeader
 *
 * Adds a chunk header at res_str_end.  Remembers old position of
 * res_str_end *before* the new chunk header string "00000000"
 * in context->res_chunk.  Advances res_str_end past "00000000"
 * sets chunk state to "partial"
 */
void ewsBodyNewChunkHeader( EwsContext context )
{
  EwsStringP this_chunk;

# ifdef EMWEB_SANITY
  if ( ewsContextChunkOutNewChunk != context->chunk_out_state )
    {
      EMWEB_ERROR(("ewsBodyNewChunkHeader: wrong state\n"));
    }
# endif /* EMWEB_SANITY */

  this_chunk = &context->res_str_end;
  ewsStringLengthSet(this_chunk, 0); /* reset res_str_end len */

  /*
   * Write blank header containing 8 zeros (enough for 32-bit hex integer).
   * We will write the actual length here after we know how big it is.
   */
  context->res_chunk = *this_chunk; /* for later update */
  ewsStringCopyIn(this_chunk, "00000000\r\n");
  ewsStringLengthSet(this_chunk, 0); /* becomes the chunk length */
  context->chunk_out_state = ewsContextChunkOutPartial;
}
#endif /* EW_CONFIG_OPTION_CHUNKED_OUT */


/**********************************************************************
 *
 *  ewsBodySend
 *
 *  Purpose: Write document data (body) out the network
 *
 *  Inputs:  context - current request context
 *           str - NULL terminated C-string to send
 *
 *  Outputs: Adds data to outgoing document datastream
 *
 *  Returns: EWS_STATUS_OK
 *
 *  Notes:
 *
 *      When writing body data, this routine or ewsBodyNSend()
 *      MUST be called.  You cannot write directly to the outgoing
 *      data stream without dealing with Digest or XML conversion.
 *
 *      Tests for whether the flag EWS_CONTEXT_FLAGS_BODY_QUOTE is set,
 *      and if so, turns on EWS_CONTEXT_FLAGS_OUTPUT_QUOTE while calling
 *      ewsStringCopyIn.  This two-level flagging is needed because
 *      ewsBodySend needs to be told by higher-level code whether body
 *      quoting is in effect (it is only done during part of a TEXTAREA
 *      send), and ewsStringCopyIn can be called asynchronously by other code
 *      to output headers (e.g., chunk headers).
 */
EwsStatus
ewsBodySend( EwsContext  context
             ,const char *str /* null terminated string */
             )
{
  EwsStatus    status;

  status = EWS_STATUS_OK;

#ifdef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND
  /*
   * If body is to be buffered as it is generated, divert processing.
   */
  if (context->flags & EWS_CONTEXT_FLAGS_BUFFER_BODY)
    {
      int i;

      EMWEB_STRLEN(i, str);
      EwuScratchAppend(context, str, i);
      return status;
    }
#endif /* EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND */

# ifdef EW_CONFIG_OPTION_CONVERT_XML
  /*
   * if output is disabled, then just return
   */
  if (context->flags & EWS_CONTEXT_FLAGS_OUTPUT_OFF)
    {
      return status;
    }
# endif /* EW_CONFIG_OPTION_CONVERT_XML */

# ifdef EW_CONFIG_OPTION_CHUNKED_OUT
  if ( ewsContextChunkOutNewChunk == context->chunk_out_state )
  {
    ewsBodyNewChunkHeader(context);
  }
# endif

# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  if (context->flags & EWS_CONTEXT_FLAGS_BODY_QUOTE)
    {
      context->flags |= EWS_CONTEXT_FLAGS_OUTPUT_QUOTE;
    }
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */
  /* copy the new string into the response */
  (void) ewsStringCopyIn( &context->res_str_end, str );
# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  if (context->flags & EWS_CONTEXT_FLAGS_BODY_QUOTE)
    {
      context->flags &= ~EWS_CONTEXT_FLAGS_OUTPUT_QUOTE;
    }
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

  return status;
}


/**********************************************************************
 *
 *  ewsBodyNSend
 *
 *  Purpose: Write document data (body) out the network
 *
 *  Inputs:  context - current request context
 *           val - pointer to data to send
 *           len - # bytes of data in val
 *
 *  Outputs: Adds data to outgoing document datastream
 *
 *  Returns: EWS_STATUS_OK
 *
 *  Notes:
 *
 *      When writing body data, this routine or ewsBodySend()
 *      MUST be called.  You cannot write directly to the outgoing
 *      data stream without dealing with Digest or XML conversion.
 *
 *      Tests for whether the flag EWS_CONTEXT_FLAGS_BODY_QUOTE is set,
 *      and if so, turns on EWS_CONTEXT_FLAGS_OUTPUT_QUOTE while calling
 *      ewsStringNCopyIn.  This two-level flagging is needed because
 *      ewsBodyNSend needs to be told by higher-level code whether body
 *      quoting is in effect (it is only done during part of a TEXTAREA
 *      send), and ewsStringNCopyIn can be called asynchronously by other code
 *      to output headers (e.g., chunk headers).
 */
EwsStatus
ewsBodyNSend( EwsContext   context
              ,const uint8 *val
              ,const uintf len
              )
{
  EwsStatus    status;

  status = EWS_STATUS_OK;

#ifdef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND
  /*
   * If body is to be buffered as it is generated, divert processing.
   */
  if (context->flags & EWS_CONTEXT_FLAGS_BUFFER_BODY)
    {
      EwuScratchAppend(context, (char *) val, len);
      return status;
    }
#endif /* EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND */

# ifdef EW_CONFIG_OPTION_CONVERT_XML
  /*
   * if output is disabled, then just return
   */
  if (context->flags & EWS_CONTEXT_FLAGS_OUTPUT_OFF)
    {
      return status;
    }
# endif /* EW_CONFIG_OPTION_CONVERT_XML */

# ifdef EW_CONFIG_OPTION_CHUNKED_OUT
  if ( ewsContextChunkOutNewChunk == context->chunk_out_state )
    {
      ewsBodyNewChunkHeader(context);
    }
# endif

# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  if (context->flags & EWS_CONTEXT_FLAGS_BODY_QUOTE)
    {
      context->flags |= EWS_CONTEXT_FLAGS_OUTPUT_QUOTE;
    }
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */
  /* copy the new string into the response */
  (void) ewsStringNCopyIn( &context->res_str_end, val, len );
# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  if (context->flags & EWS_CONTEXT_FLAGS_BODY_QUOTE)
    {
      context->flags &= ~EWS_CONTEXT_FLAGS_OUTPUT_QUOTE;
    }
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

  return status;
}


/**********************************************************************
 *
 *  ewsGetDocLength
 *
 *  Purpose: Returns the length of the given document.  Several places
 *      throughout the code did this independently - make the code size
 *      smaller by doing it once here.
 *
 *  Inputs:  EwsDocument describing the target document
 *
 *  Outputs: None
 *
 *  Returns: length of document, or EWS_CONTENT_LENGTH_UNKNOWN if
 *          the length cannot be determined (dynamic content).
 *
 */
int32 ewsGetDocLength( EwsContext context, EwsDocument document )
{
  EW_UNUSED(context);

  return /* Calculate return value below. */

#ifdef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND
    /*
     * If we are buffering the body, return the length of the scratch area
     * holding the body.
     */
    (context->flags & EWS_CONTEXT_FLAGS_BUFFER_BODY) ?
    context->scratch_length_used :
#endif /* EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND */

# if defined(EW_CONFIG_OPTION_CONVERT_XML) || \
  defined(EW_CONFIG_OPTION_CONVERT_TEXTAREA) || \
    defined(EW_CONFIG_OPTION_CONVERT_SKELETON)
    /*
     * If we are converting an html document to XML, then
     * force the contentLength to be "unknown" (since most of
     * the static html content will be stripped out).
     */
    (context->flags & (EWS_CONTEXT_FLAGS_CONVERT_XML |
                       EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA |
                       EWS_CONTEXT_FLAGS_CONVERT_SKELETON)) ?
    EWS_CONTENT_LENGTH_UNKNOWN :
# endif /* defined(EW_CONFIG_OPTION_CONVERT_XML) || etc. */

# ifdef EW_CONFIG_FILE_METHODS
    /*
     * If this document is actually a file external to the emweb
     * archive, get the contentLength from the fileInfo structure
     * assocated with the file params.
     */
    (document->flags & EWS_FILE_FLAG_FILESYSTEM) ?
    document->fileParams->fileInfo.contentLength :
# endif /* EW_CONFIG_FILE_METHODS */

    /*
     * If there are no dynamic nodes, return the original length.
     */
    (EW_BYTES_TO_UINT16(document->doc_header->node_count) == 0) ?
    (int32) EW_BYTES_TO_UINT32(document->doc_header->orig_length) :

    /*
     * Else the length is unknown.
     */
    EWS_CONTENT_LENGTH_UNKNOWN;
}

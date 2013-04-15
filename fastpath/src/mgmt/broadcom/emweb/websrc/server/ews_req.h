/*
 * File to generate the enum EwsRequestHeaderIndex in ews_pars.h and
 * the table ews_request_header_table[] in ews_pars.c.
 * By generating them both from the same file, we reduce the chance that
 * entries in the two will differ, which reduces the number of (hard to
 * detect) errors that result from mismatches between them.
 */

#ifdef EW_REQ_ENUM
  #ifdef EW_REQ_TABLE
    #error "Both EW_REQ_ENUM and EW_REQ_TABLE are defined.  ews_req.h is being used incorrectly."
  #else
    #define EW_REQ_ENTRY(string, enum)    enum
  #endif
#else
  #ifdef EW_REQ_TABLE
    #define EW_REQ_ENTRY(string, enum)    string
  #else
    #error "Neither EW_REQ_ENUM nor EW_REQ_TABLE are defined.  ews_req.h is being used incorrectly."
  #endif
#endif

/*
 * Following is a series of calls of the macro EW_REQ_ENTRY.
 * Each call must have two arguments.  The first one is the string constant
 * which is the relevant header name (case-insensitive).  The second one is
 * the enum constant name.
 * Each call (including the last) must be followed by a comma.
 */

#if defined(EW_CONFIG_OPTION_CONTEXT_DATE)
    EW_REQ_ENTRY("Date", ewsRequestHeaderDate),
#endif

#ifdef EW_CONFIG_OPTION_CONTEXT_PRAGMA
    EW_REQ_ENTRY("Pragma", ewsRequestHeaderPragma),
#endif

#ifdef EW_CONFIG_OPTION_AUTH
    EW_REQ_ENTRY("Authorization", ewsRequestHeaderAuthorization),
#endif

#ifdef EW_CONFIG_OPTION_CONTEXT_FROM
    EW_REQ_ENTRY("From", ewsRequestHeaderFrom),
#endif

#if defined(EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE) ||\
    defined(EW_CONFIG_OPTION_CONDITIONAL_GET)
    EW_REQ_ENTRY("If-Modified-Since", ewsRequestHeaderIfModifiedSince),
#endif

#ifdef EW_CONFIG_OPTION_CONTEXT_REFERER
    EW_REQ_ENTRY("Referer", ewsRequestHeaderReferer),
#endif

#ifdef EW_CONFIG_OPTION_CONTEXT_USER_AGENT
    EW_REQ_ENTRY("User-Agent", ewsRequestHeaderUserAgent),
#endif

#ifdef EW_CONFIG_OPTION_CONTEXT_VIA
    EW_REQ_ENTRY("Via", ewsRequestHeaderVia),
#endif

#if defined (EW_CONFIG_OPTION_FORM) || defined (EW_CONFIG_OPTION_CGI) ||\
       defined (EW_CONFIG_OPTION_CLIENT) || \
       (defined (EW_CONFIG_OPTION_UPNP) && defined(EW_CONFIG_OPTION_SOAP))
    EW_REQ_ENTRY("Content-Encoding", ewsRequestHeaderContentEncoding),
    EW_REQ_ENTRY("Content-Length", ewsRequestHeaderContentLength),
    EW_REQ_ENTRY("Content-Type", ewsRequestHeaderContentType),
#endif

    EW_REQ_ENTRY("Expires", ewsRequestHeaderExpires),
    EW_REQ_ENTRY("Last-Modified", ewsRequestHeaderLastModified),

#ifdef EW_CONFIG_OPTION_PERSISTENT
    EW_REQ_ENTRY("Connection", ewsRequestHeaderConnection),
#endif /* EW_CONFIG_OPTION_PERSISTENT */

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 ||\
       defined(EW_CONFIG_OPTION_CONTEXT_HOST)
    EW_REQ_ENTRY("Host", ewsRequestHeaderHost),
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1
    EW_REQ_ENTRY("Transfer-Encoding", ewsRequestHeaderTransferEncoding),
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 */

#ifdef EW_CONFIG_OPTION_COOKIES
    EW_REQ_ENTRY("Cookie", ewsRequestHeaderCookie),
#endif /* EW_CONFIG_OPTION_COOKIES */

#if defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) ||\
    defined(EW_CONFIG_OPTION_CONTEXT_IF_MATCH)
    EW_REQ_ENTRY("If-Match", ewsRequestHeaderIfMatch),
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH || CONTEXT_IF_MATCH*/

#if defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) ||\
    defined(EW_CONFIG_OPTION_CONTEXT_IF_NONE_MATCH)
    EW_REQ_ENTRY("If-None-Match", ewsRequestHeaderIfNoneMatch),
#endif /* EW_CONFIG_OPTION_CONDITIONAL_MATCH || CONTEXT_IF_NONE_MATCH*/

#if defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION)\
 || defined(EW_CONFIG_OPTION_ACCEPT_LANG)
    EW_REQ_ENTRY("Accept-Language", ewsRequestHeaderAcceptLanguage),
#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION || ACCEPT_LANG */

#ifdef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
    EW_REQ_ENTRY("Negotiate", ewsRequestHeaderNegotiate),
    EW_REQ_ENTRY("Accept-Encoding", ewsRequestHeaderAcceptEncoding),
    EW_REQ_ENTRY("Accept-Charset", ewsRequestHeaderAcceptCharset),
    EW_REQ_ENTRY("Accept", ewsRequestHeaderAccept),
#endif /* EW_CONFIG_OPTION_CONTENT_NEGOTIATION */

#if EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && defined(EW_CONFIG_OPTION_EXPECT)
    EW_REQ_ENTRY("Expect", ewsRequestHeaderExpect),
#endif /* EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1 && EXPECT */

#ifdef EW_CONFIG_OPTION_PRIMARY
    EW_REQ_ENTRY("Max-Forwards", ewsRequestHeaderMaxForwards),
#endif /* EW_CONFIG_OPTION_PRIMARY */

#ifdef EW_CONFIG_OPTION_SECONDARY
    EW_REQ_ENTRY("X-EmWeb-Primary", ewsRequestHeaderXEmWebPrimary),
#endif /* EW_CONFIG_OPTION_SECONDARY */

#ifdef EW_CONFIG_OPTION_CLIENT
    EW_REQ_ENTRY("Location", ewsRequestHeaderLocation),
    EW_REQ_ENTRY("ETag", ewsRequestHeaderEtag),
    EW_REQ_ENTRY("Server", ewsRequestHeaderServer),
    EW_REQ_ENTRY("Cache-Control", ewsRequestHeaderCacheControl),
    EW_REQ_ENTRY("Age", ewsRequestHeaderAge),

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
    EW_REQ_ENTRY("WWW-Authenticate", ewsRequestDigestAuthenticate),
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#ifdef EW_CONFIG_OPTION_CLIENT_COOKIES
    EW_REQ_ENTRY("Set-Cookie", ewsRequestHeaderSetCookie),
#endif /* EW_CONFIG_OPTION_CLIENT_COOKIES */

#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EW_CONFIG_OPTION_UPNP
#   ifdef EW_CONFIG_OPTION_UPNP_SSDP_CP
    EW_REQ_ENTRY("NTS", ewsRequestHeaderNTS),
    EW_REQ_ENTRY("USN", ewsRequestHeaderUSN),
#   endif /* EW_CONFIG_OPTION_UPNP_SSDP_CP */
    EW_REQ_ENTRY("MAN", ewsRequestHeaderMAN),
    EW_REQ_ENTRY("ST", ewsRequestHeaderST),
    EW_REQ_ENTRY("MX", ewsRequestHeaderMX),
    EW_REQ_ENTRY("Timeout", ewsRequestHeaderTimeout),
    EW_REQ_ENTRY("Callback", ewsRequestHeaderCallback),
    EW_REQ_ENTRY("NT", ewsRequestHeaderNT),
    EW_REQ_ENTRY("SID", ewsRequestHeaderSID),
    /* Must appear after SID and ST until REQ
     * #3286 is resolved. */
    EW_REQ_ENTRY("S", ewsRequestHeaderS),
#endif /* EW_CONFIG_OPTION_UPNP */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
    EW_REQ_ENTRY("Range", ewsRequestHeaderRange),
    EW_REQ_ENTRY("Accept-Ranges", ewsRequestHeaderAcceptRanges),
#ifdef EW_CONFIG_OPTION_CLIENT
    EW_REQ_ENTRY("Content-Range", ewsRequestHeaderContentRange),
#endif /* EW_CONFIG_OPTION_CLIENT */
#endif /* EW_CONFIG_OPTION_BYTE_RANGES */


/*
 * Leave EW_REQ_ENTRY undefined to prevent unpleasant interactions if
 * this file is accidentally included in the wrong location.
 */
#undef EW_REQ_ENTRY

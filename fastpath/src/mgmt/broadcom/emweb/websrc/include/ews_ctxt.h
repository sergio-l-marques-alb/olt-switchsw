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
 * EmWeb/Server public definitions
 *
 */
#ifndef _EWS_CONTEXT_H_
#define _EWS_CONTEXT_H_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_def.h"
#include "ews_sys.h"

#ifdef EW_CONFIG_OPTION_XMLP
#include "ews_xmlp.h"
#endif

/*
 * REQUEST CONTEXT
 *
 * Each HTTP request received by EmWeb/Server is assigned a unique context.
 * The context structure contains all the information pertaining to the
 * request, as well as internal state information regarding the processing
 * state of the request.
 *
 * API functions are provided to give the application some visibility into
 * the current context state.
 */

/*
 * ewsContextNetHandle
 * Return the application-specific network handle provided to EmWeb/Server
 * by the application in ewsNetHTTPStart().
 */
EwaNetHandle ewsContextNetHandle ( EwsContext context );

#if defined (EW_CONFIG_OPTION_DEMAND_LOADING) \
    || defined (EW_CONFIG_OPTION_CLONING)
/*
 * ewsContextDocumentHandle
 * Return the application-specific document handle provided to EmWeb/Server
 * by the application in ewsDocumentRegister() or ewsDocumentClone().
 */
EwaDocumentHandle ewsContextDocumentHandle ( EwsContext context );
#endif /* EW_CONFIG_OPTION_DEMAND_LOADING || EW_CONFIG_OPTION_CLONING */

#ifdef EW_CONFIG_OPTION_AUTH
/*
 * ewsContextAuthHandle
 * Return the application-specific authorization handle provided to
 * EmWeb/Server by the application in ewsAuthRegister().
 */
EwaAuthHandle ewsContextAuthHandle ( EwsContext context );
#endif /* EW_CONFIG_OPTION_AUTH */

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
/*
 * Return state of authentication process
 */

extern EwsAuthState ewsContextAuthState( EwsContext context );
#endif

#ifdef EW_CONFIG_OPTION_AUTH
/*
 * Return user name
 */
extern EwsStatus ewsContextAuthUsername( EwsContext context
                                        ,char       *userp
                                        ,uintf      len);
/*
 * Return password
 */
extern EwsStatus ewsContextAuthPassword( EwsContext context
                                        ,char *pwd
                                        ,uintf len);
#endif

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
/*
 * ewsContextIsResuming
 * Return TRUE if EmWeb/Server is resuming (as a result of ewsResume()) after
 * the context was suspended (as a result of ewsSuspend()).
 */
boolean ewsContextIsResuming ( EwsContext context );
#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

#if defined (EW_CONFIG_OPTION_STRING) || defined (EW_CONFIG_OPTION_INCLUDE)\
   || defined (EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC)
/*
 * ewsContextIterations
 * Returns the iteration count corresponding for EMWEB_ITERATE
 */
uint32 ewsContextIterations ( EwsContext context );
#endif /* EW_CONFIG_OPTION_STRING || EW_CONFIG_OPTION_INCLUDE
         || EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#if defined (EW_CONFIG_OPTION_FORM_REPEAT)
/*
 * ewsContextRepetition
 * Returns the iteration count corresponding to EMWEB_REPEAT
 */
uint32 ewsContextRepetition ( EwsContext context );
char *ewsContextPrefixGet(EwsContext context);
char *ewsContextRepeatIndexModGet(EwsContext context);
#endif /* EW_CONFIG_OPTION_STRING */

#ifdef EW_CONFIG_OPTION_ITERATE
/*
 * ewsContextIterator
 * Returns the current iterator for the innermost <EMWEB_ITERATE> tag,
 * NULL if no iterator.
 *
 * Note that this does not cross document boundaries - in an EMWEB_INCLUDED
 * document, it will return only an iterator within the included document,
 * not one from the document that contained the EMWEB_INCLUDE tag.
 */
#define ewsContextIterator( context ) ewsContextIteratorN( context, 0 )

/*
 * ewsContextIteratorN
 * Returns the current iterator for the Nth <EMWEB_ITERATE> tag,
 *   the innermost tag is 0, the next out is 1, etc
 * NULL if no iterator at the specified level.
 *
 * Note that this does not cross document boundaries - in an EMWEB_INCLUDED
 * document, it will return only the iterators within the included document,
 * not any from the document that contained the EMWEB_INCLUDE tag.
 */
const void *ewsContextIteratorN( EwsContext context, uintf nth );
#endif

#ifdef EW_CONFIG_OPTION_CONTEXT_SEND_REPLY
/*
 * ewsContextSendReply
 * Serves specified local path URL to browser in response to form submission
 * or raw CGI.
 */
EwsStatus ewsContextSendReply ( EwsContext context, const char * url );
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REPLY */

#if defined( EW_CONFIG_OPTION_FILE_GET ) \
 || defined( EW_CONFIG_OPTION_FILE_PUT ) \
 || defined( EW_CONFIG_OPTION_FILE_DELETE )
EwsStatus ewsContextSetFile( EwsContext context
                            ,EwsFileParamsP params
#ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                            ,EwsFileSystemHandle fileSystemHandle
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                           );
#endif /* EW_CONFIG_OPTION_FILE_xxx */

#ifdef EW_CONFIG_OPTION_XMLP
/*
 * ewsContextSetXMLP
 *
 * This function is called by the application's ewaURLHook() to redirect
 * the HTTP input stream to the XML parser.
 *
 * Params:
 *         context  - Request context
 *         consumer - Consumer handle from xmlpRegisterConsumer()
 *
 * Returned:
 *         EWS_STATUS_OK on success, else error code
 */
EwsStatus ewsContextSetXMLP( EwsContext context
                            ,XmlpConsumer consumer
                            );
#endif /* EW_CONFIG_OPTION_XMLP */

#ifdef EW_CONFIG_OPTION_COOKIES
/*
 * ewsContextCookieValue
 * Returnes cookie's value
 */
EW_NSC char *ewsContextCookieValue(EwsContext context, EW_NSC char *name);
#endif /* EW_CONFIG_OPTION_COOKIES */

#ifdef EW_CONFIG_OPTION_STRING_VALUE
/*
 * ewsContextGetValue
 * Returns the contents of the value parameter
 *  e.g. <EMWEB_STRING value='a value'>
 * would return 'a value'
 */
const char *ewsContextGetValue(EwsContext context);
#endif /* EW_CONFIG_OPTION_STRING_VALUE */

#ifdef EW_CONFIG_OPTION_FLUSH_DATA
void ewsContextFlushData( EwsContext context );
#endif

#ifdef EW_CONFIG_OPTION_CONVERT_XML
/*
 * ewsContextServeXML
 * Calling this routine in ewaURLHook() or prior to ewsContextSendReply()
 * will cause only the XML content of the next page to be served.
 */
boolean ewsContextServeXML( EwsContext context );

/*
 * ewsContextServingXML
 * returns TRUE if current document being served by the context is being
 * served in "XML out" mode
 */
boolean ewsContextServingXML( EwsContext context );
#endif

#ifdef EW_CONFIG_OPTION_CONVERT_SKELETON
/*
 * ewsContextServeSkeleton
 * Calling this routine in ewaURLHook() or prior to ewsContextSendReply()
 * will cause the skeleton of the content of the next page to be served.
 */
boolean ewsContextServeSkeleton( EwsContext context );

/*
 * ewsContextServingSkeleton
 * returns TRUE if current document being served by the context is being
 * served in skeleton mode
 */
boolean ewsContextServingSkeleton( EwsContext context );
#endif

#ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
/*
 * ewsContextServeTextarea
 * Calling this routine in ewaURLHook() or prior to ewsContextSendReply()
 * will cause the content of the next page to be served wrapped in a
 * "TEXTAREA hack".
 */
boolean ewsContextServeTextarea( EwsContext context );

/*
 * ewsContextServingTextarea
 * returns TRUE if current document being served by the context is being
 * served in TEXTAREA mode
 */
boolean ewsContextServingTextarea( EwsContext context );
#endif


/*
 * The following functions extract the corresponding fields from the HTTP/1.0
 * request header, if present, into an application-provided buffer.  Each of
 * these functions return the number of bytes in the actual header value, even
 * if this is larger than the application-provided buffer (though EmWeb/Server
 * will not overwrite the applicatoin buffer by honoring the buffer length).
 * If the header is not present, zero is returned.  The application may
 * specify a zero-length buffer in order to determine the size of the header
 * value.  The returned value begins with the first non-whitespace following
 * the HTTP header and ends just before the terminating end-of-line
 * character(s).
 */
#ifdef EW_CONFIG_OPTION_CONTEXT_DATE
uintf ewsContextDate ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_DATE */
#ifdef EW_CONFIG_OPTION_CONTEXT_PRAGMA
uintf ewsContextPragma ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_PRAGMA */
#ifdef EW_CONFIG_OPTION_CONTEXT_FROM
uintf ewsContextFrom ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_FROM */
#ifdef EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE
uintf ewsContextIfModifiedSince
  ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE */
#ifdef EW_CONFIG_OPTION_CONTEXT_REFERER
uintf ewsContextReferer ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_REFERER */
#ifdef EW_CONFIG_OPTION_CONTEXT_USER_AGENT
uintf ewsContextUserAgent ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_USER_AGENT */
#ifdef EW_CONFIG_OPTION_CONTEXT_VIA
uintf ewsContextVia ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_VIA */
#ifdef EW_CONFIG_OPTION_CONTEXT_HOST
uintf ewsContextHost ( EwsContext context, char *datap, uintf length );
#endif /* EW_CONFIG_OPTION_CONTEXT_HOST */
#ifdef EW_CONFIG_OPTION_CONTEXT_IF_MATCH
uintf ewsContextIfMatch ( EwsContext context, char *datap, uintf length );
#endif
#ifdef EW_CONFIG_OPTION_CONTEXT_IF_NONE_MATCH
uintf ewsContextIfNoneMatch ( EwsContext context, char *datap, uintf length );
#endif
#ifdef EW_CONFIG_OPTION_CONTEXT_REQUEST_METHOD
EwsRequestMethod ewsContextRequestMethod ( EwsContext context );
#endif /* EW_CONFIG_OPTION_CONTEXT_REQUEST_METHOD */
#ifdef EW_CONFIG_OPTION_ACCEPT_LANG
uintf ewsContextAcceptLanguage( EwsContext context, char *datap, uintf length );
#endif

#ifdef EW_CONFIG_OPTION_SNMP_AGENT
extern EwaSNMPAgentHandle ewsContextGetSNMPAgentHandle( EwsContext );
extern void ewsContextSetSNMPAgentHandle( EwsContext, EwaSNMPAgentHandle );
#ifdef EW_CONFIG_OPTION_AUTH
extern EwaSNMPAgentAuthHandle ewsContextGetSNMPAgentAuthHandle( EwsContext context );
#endif /* EW_CONFIG_OPTION_AUTH */
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#if defined( EW_CONFIG_OPTION_NS_GET ) \
 || defined( EW_CONFIG_OPTION_NS_SET ) \
 || defined( EW_CONFIG_OPTION_CONVERT_XML )
extern boolean ewsContextSchemaInfo( EwsContext
                                    ,EwsSchemaOperation op
                                    ,EwsSchemaInfo  *arg );
#endif /* schema */

#ifdef EW_CONFIG_OPTION_NS_LOOKUP

/*
 * ewsNamespaceGet
 * Called from EMWEB_STRING code fragment.  The code fragment should return
 * the value returned by this function.  This function is used to indirectly
 * access namespace schema nodes by name.
 *
 *   context - context pointer
 *   namespace - name of namespace, or EWS_NAMESPACE_EMWEB for EmWeb namespace
 *   name      - name of schema object
 *   parameters - parameters to pass to macro
 *
 * Returns result to be returned from EMWEB_STRING code fragment, or
 * EWS_NAMESPACE_GET_ERROR if the lookup failed.
 */
#define      EWS_NAMESPACE_EMWEB      NULL
#ifdef EW_CONFIG_OPTION_NS_GET
extern
#ifndef _cplusplus
const
#endif
char *EWS_NAMESPACE_GET_ERROR;

char *
ewsNamespaceGet( EwsContext context
                ,const char *nameSpace
                ,const char *name
                ,const char *parameters );
#endif /* EW_CONFIG_OPTION_NS_GET */

#ifdef EW_CONFIG_OPTION_NS_SET
/*
 * The following functions give the user a mechanism to set the value of
 * namespace objects indirectly by name.  These functions mirror the three
 * per-namespace API functions used to implement sets.  These functions
 * can only be called from a valid context.  Note that if the per-namespace
 * set_finish function invokes ewsContextSendReply, then it should only be
 * invoked from a state in which this would be allowed.
 *
 * Note that namespace sets may not be nested.  Only one per context
 * is allowed at any given time.
 */

 /*
  * ewsNamespaceSetStart
  * Returns EWS_STATUS_OK or error
  *
  *   context    - context of request
  *   namespace  - name of namespace
  */
EwsStatus  ewsNamespaceSetStart ( EwsContext context
                                 ,const char *nameSpace );

/*
 * ewsNamespaceSet
 * Set object value
 *
 *   context    - context of request
 *   name       - name of object to set
 *   parameters - qualifying parameters
 *   value      - new value
 *
 * Returns EWS_STATUS_OK or EWS_STATUS_NOT_FOUND
 */
EwsStatus ewsNamespaceSet ( EwsContext context
                           ,const char *name
                           ,const char *parameters
                           ,const char *value );

/*
 * ewsNamespaceSetFinish
 * Complete atomic set operation.  This should be called from a state in
 * which ewsContextSendReply() is permitted, and should not be called from
 * a namespace form (since nesting is not allowed).
 *
 *   context - context of request
 *   status  - EWS_STATUS_OK|ABORT
 *
 * Note that abort case is handled by server.
 */
EW_NSC
char * ewsNamespaceSetFinish ( EwsContext context, EwsStatus status );

#endif /* EW_CONFIG_OPTION_NS_SET */

#ifdef EW_CONFIG_OPTION_NS_GETNEXT
/*
 * ewsGetNextObject
 * Read object names from namespace
 *
 *   ns     - name of namespace
 *   object - name of object
 *
 * Returns next name of object or NULL
 */
const char * ewsGetNextObject ( const char *ns, const char *obj );
#endif /* EW_CONFIG_OPTION_NS_GETNEXT */

#endif /* EW_CONFIG_OPTION_NS_LOOKUP */

#ifdef EW_CONFIG_OPTION_CONTEXT_SEND_ERROR
/*
 * ewsContextSendError
 * Abort a request with HTTP error code
 *
 *   context     - context of request
 *   status      - error message
 *
 * Returns EWS_STATUS_OK on success, or EWS_STATUS_BAD_STATE if too late to
 * send error (e.g. headers already sent).  The connection will automatically
 * be aborted.
 */
EwsStatus
ewsContextSendError ( EwsContext context
                     ,const char *status
                    );
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_ERROR */

#ifdef EW_CONFIG_OPTION_CONTEXT_SEND_REDIRECT
/*
 * ewsContextSendRedirect
 * Called from URL Hook function to redirect request to a different resource
 * using "Moved Permanently"
 *
 *   context     - context of request
 *   location    - Absolute URI (e.g. "/...") or absolute URL (e.g. "http://...)
 *
 * Returns EWS_STATUS_OK on success, or EWS_STATUS_BAD_STATE if not in URL hook
 */
EwsStatus
ewsContextSendRedirect ( EwsContext context
                        ,const char *location
                       );
#endif /* EW_CONFIG_OPTION_CONTEXT_SEND_REDIRECT */

#ifdef EW_CONFIG_OPTION_CLIENT
/*
 * ewsContextStatusOverride
 *
 *   context     - context of request
 *   status_string - new status string (e.g. "200 OK")
 *
 * This function is provided to facilitate the development of an HTTP
 * gateway using EmWeb/Server together with EmWeb/Client.  This overrides
 * the value of the generated status code when sending response data back
 * to the server if and only if the response would have been "200".
 */
void
ewsContextStatusOverride ( EwsContext  context
                         , const char *status_string
                        );
#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EW_CONFIG_OPTION_SSL_RSA
/*************************************************************************
 *
 * ewsContextSecurityHandle()
 *
 ************************************************************************
EwaSecurityHandle ewsContextSecurityHandle(EwsContext context);
*/
#endif /* EW_CONFIG_OPTION_SSL_RSA */

#ifdef EW_CONFIG_OPTION_BYTE_RANGES
/*
 * ewsContextRange
 *
 *   context - context of request
 *   firstBytePosP - output: >=0: first byte pos.  <0: sufix (from end) pos.
 *   lastBytePosP  - output: last byte pos or EWS_CONTENT_LENGTH_UNKNOWN
 *
 * This function is provided in support of content byte ranges so that the
 * application can detect if an HTTP request was for a content range.
 *
 * Returns EWS_STATUS_OK if this is a request with a requested range, or
 * EWS_STATUS_NOT_FOUND if no range header was present.
 */
EwsStatus
ewsContextRange ( EwsContext context
                 ,int32 *firstBytePosP
                 ,int32 *lastBytePosP
                );

/*
 * ewsContextSendRangeError
 *
 *   context        - context of request
 *   instanceLength - size of entity
 *
 * This function generates a "416 Requested range not satisfiable" error
 * including a "Content-Range: * / <instanceLength>" header.
 */
void
ewsContextSendRangeError(EwsContext context, uint32 instanceLength);

#endif /* EW_CONFIG_OPTION_BYTE_RANGES */

#ifdef EW_CONFIG_OPTION_DYNAMIC_ETAG
/*
 * ewsContextCacheControl
 *
 *   May be used only in the code specified in EMWEB_ETAG
 *   to control the Cache-Control header as follows:
 *   = EWS_CACHE_REVALIDATE
 *   Document may be cached, and no maximum cache time is
 *   specified for it, but the special cache control directive
 *   'must-revalidate' is sent.  This means that the client may
 *   cache the response but before returning it from the cache,
 *   must send a conditional request for it to check that it is
 *   still valid.
 *
 *   = EWS_CACHE_UNLIMITED
 *   Document is indefinitly cachable.
 *   No maximum cache time is set for the response.  This
 *   probably should not be used for web pages intended for
 *   browsers, since it may produce unpredictable browser and
 *   proxy cache behaviour, but it may be of use in other HTTP
 *   applications.
 *
 *   = Any other value is sent as the value of the max-age attribute,
 *   which specifies the number of seconds for which the cached copy
 *   is valid.
 *
 *   If this routine is not called, then the default is
 *     EWS_CACHE_REVALIDATE
 */
EwsStatus
#define EWS_CACHE_UNLIMITED ((uintf)-1)
#define EWS_CACHE_REVALIDATE 0
ewsContextCacheControl( EwsContext context, uintf maxage );
#endif /* EW_CONFIG_OPTION_DYNAMIC_ETAG */

#endif /* _EWS_CONTEXT_H_ */

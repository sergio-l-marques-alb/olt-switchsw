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
 * EmWeb Server Options
 */

#ifndef _EW_OPTIONS_H_
#define _EW_OPTIONS_H_

/*
 * HTTP PROTOCOL VERSION
 */
#       define HTTP_1_0 1000    /* DO NOT MODIFY */
#       define HTTP_1_1 1001    /* DO NOT MODIFY */
/*
 * Select desired conformance level
 *   We strongly recommend HTTP_1_1; the performance is much better,
 *   and at lower cost to your system because it uses fewer TCP
 *   connections.
 */
#define EW_CONFIG_HTTP_PROTOCOL HTTP_1_1

/*
 * PROTOCOL OPTIONS (>= HTTP/1.0)
 *   (R) Required, but some embedded systems don't have time-of-day
 *       If your system cannot produce an accurate GMT time stamp
 *       (see the ewaDate routine in ewsys.c for an example of the
 *       correct format), you should undef EW_CONFIG_OPTION_DATE;
 *       EmWeb will then do the cache control correctly without the
 *       timestamps.
 *   (O) Optional, but very strongly recommended
 */
#define EW_CONFIG_OPTION_DATE           /* (R) Generate Date: */

#define EW_CONFIG_OPTION_EXPIRE         /* (O) Generate Expire: */
#define EW_CONFIG_OPTION_LAST_MODIFIED  /* (O) Generate Last-modified: */
#define EW_CONFIG_OPTION_CONDITIONAL_GET/* (O) Parse If-Modified-Since: */
#define EW_CONFIG_OPTION_PRAGMA_NOCACHE /* (O) Generate Pragma: no-cache */

/*
 * PROTOCOL OPTIONS (>= HTTP/1.1)
 *   (O) Optional, but strongly recommended
 *   (*) May be used with HTTP/1.0
 */
#define EW_CONFIG_OPTION_PERSISTENT     /* (O*) persistent connections */
#define EW_CONFIG_OPTION_CHUNKED_OUT    /* (O) respond with chunked encoding */
#define EW_CONFIG_OPTION_CHUNKED_IN     /* (O) parse chunked encoding */
#define EW_CONFIG_OPTION_METHOD_OPTIONS /* (O*) support OPTIONS method */
/* #define EW_CONFIG_OPTION_METHOD_TRACE   *** (O*) support TRACE method */
#define EW_CONFIG_OPTION_CACHE_CONTROL  /* (O*) Generate Cache-Control: */
#define EW_CONFIG_OPTION_EXPECT         /* (O) Use Expect: */
#undef EW_CONFIG_OPTION_BYTE_RANGES    /* (O) support content byte ragnes */

/* Cache control directives, for HTTP 1.1

   Cache-Control = "Cache-Control" ":" 1#cache-directive

   For EW_CONFIG_OPTION_CACHE_CONTROL_STATIC, please note that these
   values apply to ALL static documents in the archive.  If you want to
   change the values for static docs, please change this definition,
   and separate the directives with ",".
   The default is "max-age=3600".

   For EW_CONFIG_OPTION_CACHE_CONTROL_DYNAMIC, please note that these
   values apply to ALL dynamic documents in the archive.  If you want to
   change the values for dynamic docs, please change this definition, and
   separate the directives with ",".
   The default is "no-cache".

   The following are cache-response-directives available:

   no-cache -- must not be cached anywhere.
      Note this has the same semantics as the "Pragma: no-cache"
      directive, but "Pragma: no-cache" is  defined for backwards
      compatibility for HTTP/1.0.
      Note that the "no-cache" "=" <"> 1#field-name <"> is
      currently not supported.  This directive applies to
      the entire request, and not to a particular field.

   no-store -- may be cached, but not stored on secondary storage.  Good
      for keeping sensitive information from getting backed up
      on tape, for example.

   no-transform -- don't let caches reformat data.

   must-revalidate -- force revalidation with server if cache entry aged

   proxy-revalidate -- proxies must revalidate, but end users don't.
      (better for authenticated pages than must).

   max-age=nnn -- cache is "stale" after nnn seconds.
      Note from RFC2068: "...If the response includes both an
      Expires header and a  max-age directive, the max-age directive
      overrides the Expires header, even if the Expires header is
      more restrictive.  This rule allows an origin server to provide,
      for a given response, a longer expiration time to an HTTP/1.1
      (or later) cache than to an HTTP/1.0 cache."

   *public -- response is cachable by any cache, even if it would normally
      be non-cachable or cachable only within a non-shared cache.

   *private -- indicates that all or part of the response message is intended
      for a single user and MUST NOT be cached by a shared cache.
      Note that the "private" "=" <"> 1#field-name <"> is
      currently not supported.  This directive applies to
      the entire request, and not to a particular field.

   * Note that the EMWEB server determines whether to set public or private
   for a static document, based upon the realm and presence of the
   EW_CONFIG_OPTION_AUTH definition.

   */

#define EW_CONFIG_OPTION_CACHE_CONTROL_DYNAMIC "no-cache"
#define EW_CONFIG_OPTION_CACHE_CONTROL_STATIC  "max-age=3600"

#define EW_CONFIG_OPTION_CACHE_ETAG           /* (O) Generate ETAG */
#define EW_CONFIG_OPTION_CONDITIONAL_MATCH    /* (O) Parse if-match,if-none-match: header */

/* #define EW_CONFIG_OPTION_DYNAMIC_ETAG  // support for application etags using EMWEB_ETAG tag */

/*
 * CONFIGURATION OPTIONS
 *
 * The EmWeb/Server may be customized by the application developer to balance
 * between functionality v.s. memory and CPU requirements.  Each of the options
 * below may be selected (#define) or unselected (#undef).
 */
#define EW_CONFIG_OPTION_NO_STRICT_CONST /* LVL7_P0006 'char*' instead of 'const char*' */
#define EW_CONFIG_OPTION_STRING         /* emweb_string support */
#define EW_CONFIG_OPTION_EWS_EWS_STRING_COMPARE /* compare two strings from chains of network bufs*/
#define EW_CONFIG_OPTION_STRING_TYPED   /* typed emweb_string support */
#define EW_CONFIG_OPTION_STRING_VALUE   /* values in strings and macros */
#define EW_CONFIG_OPTION_INCLUDE        /* emweb_include support */
#define EW_CONFIG_OPTION_NS_GET         /* namespace get support */
#define EW_CONFIG_OPTION_NS_SET         /* namespace set support */
#undef EW_CONFIG_OPTION_NS_LOOKUP      /* namespace lookup support */
#undef EW_CONFIG_OPTION_NS_GETNEXT     /* namespace getnext support */
#undef EW_CONFIG_OPTION_URL_GETNEXT    /* URL getnext support */
#define EW_CONFIG_OPTION_FORM           /* form support */
#define EW_CONFIG_OPTION_FORM_REPEAT    /* support <EMWEB_REPEAT> */
#define EW_CONFIG_OPTION_IMAGEMAP       /* support for imagemaps */
#define EW_CONFIG_OPTION_CGI            /* raw CGI support */
#define EW_CONFIG_OPTION_LINK           /* link node support */
#define EW_CONFIG_OPTION_CLONING        /* URL cloning support */
#define EW_CONFIG_OPTION_DEMAND_LOADING /* demand document loading support */
#define EW_CONFIG_OPTION_DOCUMENT_DATA  /* direct archive data access support */
#define EW_CONFIG_OPTION_DOCUMENT_SET_REALM /* run-time realm assignment sup. */
#define EW_CONFIG_OPTION_CLEANUP        /* graceful cleanup support */
#define EW_CONFIG_OPTION_SCHED          /* MUST BE DEFINED! call ASI if undef */
#define EW_CONFIG_OPTION_SCHED_SUSP_RES /* suspend/resume request support */

#define EW_CONFIG_OPTION_SCHED_FC       /* flow control support */
#define EW_CONFIG_OPTION_URL_HOOK       /* | Single URL rewriting hook; */
/* #undef  EW_CONFIG_OPTION_URL_HOOK_LIST  // | OR - list of URL hooks */
#define EW_CONFIG_OPTION_DIRECTORY_REDIRECT /* Redirect if index w/o slash */
#define EW_CONFIG_OPTION_AUTH           /* authorization support */
#define EW_CONFIG_OPTION_AUTH_BASIC     /* basic authorization support */

/* START-STRIP-EMWEB-LIGHT */

#define EW_CONFIG_OPTION_AUTH_MBASIC    /* "manual" basic authentication */
#define EW_CONFIG_OPTION_AUTH_MBASIC_DECODE /* do base64 decode for MBASIC */
/* #define EW_CONFIG_OPTION_AUTH_DIGEST    * digest authentication support */
/* #define EW_CONFIG_OPTION_AUTH_MDIGEST * "manual" digest authentication */

/* END-STRIP-EMWEB-LIGHT */

#define EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER /* no realm qualifier */
/* #define EW_CONFIG_OPTION_AUTH_DOMAIN  * challenge domain */
/* #define EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE * password change feature */
/* #define EW_CONFIG_OPTION_AUTH_VERIFY    * manual verification of client */
#define EW_CONFIG_OPTION_COMPRESS       /* archive decompression support */
#define EW_CONFIG_OPTION_RELEASE_UNUSED /* Early release of unused buffers */
#define EW_CONFIG_OPTION_FILE           /* local filesystem support */
#define EW_CONFIG_OPTION_FILE_GET       /* local filesystem GET method */
#define EW_CONFIG_OPTION_FILE_DELETE    /* local filesystem DELETE method */
#define EW_CONFIG_OPTION_FILE_PUT       /* local filesystem PUT method */
#define EW_CONFIG_OPTION_COOKIES        /* support state management */
/* #undef EW_CONFIG_OPTION_PRIMARY         // support distributed as primary */
#define EW_CONFIG_OPTION_URL_PATH_INFO  /* allow extra path info @ URL end */
/* #undef EW_CONFIG_OPTION_FLUSH_DATA      // provide ewsContextFlushData() */
#define EW_CONFIG_OPTION_ITERATE        /* allow <EMWEB_ITERATE> */
#define EW_CONFIG_OPTION_IF             /* allow <EMWEB_IF> */
/* #undef EW_CONFIG_OPTION_UPLOAD_ARCHIVE // derived archive uploading */
/* #undef EW_CONFIG_OPTION_SECONDARY_ACTION  // secondary prefix form action url */
/* #undef  EW_CONFIG_OPTION_FILE_SYSTEM_LIST // multiple local file systems */
#define EW_CONFIG_OPTION_LOAD           /* allow <EMWEB_LOAD> tags */
#define EW_CONFIG_OPTION_UNLOAD         /* allow <EMWEB_UNLOAD> tags */
/* #undef EW_CONFIG_OPTION_CONVERT_XML     // serve page as XML data */
/* #undef EW_CONFIG_OPTION_CONVERT_TEXTAREA // ability embed page in TEXTAREA hack */
/* #undef EW_CONFIG_OPTION_CONVERT_SKELETON // ability to serve page as a skeleton */
/* #undef EW_CONFIG_OPTION_SSL_RSA */

/*
 * Options to include support for retrieving HTTP request headers
 */
#define EW_CONFIG_OPTION_CONTEXT_REQUEST_METHOD
#define EW_CONFIG_OPTION_CONTEXT_DATE
#define EW_CONFIG_OPTION_CONTEXT_PRAGMA
#define EW_CONFIG_OPTION_CONTEXT_FROM
#define EW_CONFIG_OPTION_CONTEXT_IF_MODIFIED_SINCE
#define EW_CONFIG_OPTION_CONTEXT_REFERER
#define EW_CONFIG_OPTION_CONTEXT_USER_AGENT
/* #define EW_CONFIG_OPTION_CONTEXT_VIA */
#define EW_CONFIG_OPTION_CONTEXT_HOST
#define EW_CONFIG_OPTION_CONTEXT_IF_MATCH
#define EW_CONFIG_OPTION_CONTEXT_IF_NONE_MATCH

/*
 * Option for serving document to browser in response to form or CGI
 */
#define EW_CONFIG_OPTION_CONTEXT_SEND_REPLY

/*
 * Options to send special headers to browser
 */
#define EW_CONFIG_OPTION_CONTEXT_SEND_REDIRECT
#define EW_CONFIG_OPTION_CONTEXT_SEND_ERROR

/*
 * Options to include support for CGI access functions
 */
#define EW_CONFIG_OPTION_CGI_SERVER_SOFTWARE
#define EW_CONFIG_OPTION_CGI_GATEWAY_INTERFACE
#define EW_CONFIG_OPTION_CGI_SERVER_PROTOCOL
#define EW_CONFIG_OPTION_CGI_REQUEST_METHOD
#define EW_CONFIG_OPTION_CGI_PATH_INFO
#define EW_CONFIG_OPTION_CGI_SCRIPT_NAME
#define EW_CONFIG_OPTION_CGI_QUERY_STRING
#define EW_CONFIG_OPTION_CGI_QUERY_STRING_ESCAPED
#define EW_CONFIG_OPTION_CGI_CONTENT_TYPE
#define EW_CONFIG_OPTION_CGI_CONTENT_LENGTH
#define EW_CONFIG_OPTION_CGI_CONTENT_ENCODING

/*
 * Options to handle certain form field types (EW_CONFIG_OPTION_FORM must be
 * defined for these to take effect).
 */
#define EW_CONFIG_OPTION_FIELDTYPE_RADIO
#define EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE
#define EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE
#define EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC /* support dynamic SELECT
                                                     (single or multi) */
#define EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX
#define EW_CONFIG_OPTION_FIELDTYPE_TEXT
#define EW_CONFIG_OPTION_FIELDTYPE_IMAGE
#define EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT
#define EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT
#define EW_CONFIG_OPTION_FIELDTYPE_HEX_INT
#define EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING
#define EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP
#define EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV
#define EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC
#define EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC
#define EW_CONFIG_OPTION_FIELDTYPE_STD_MAC
#define EW_CONFIG_OPTION_FIELDTYPE_OID
#define EW_CONFIG_OPTION_FIELDTYPE_FILE

/*
 * Options to handle non-conformant browsers
 *
 * BROKEN_IMS_EXTRA_DATA - Several recent versions of Netscape incorrectly
 * add extra information at the end of an If-Modified-Since: header in the
 * form "; (size = n)".  This is in violation of the HTTP specification and
 * causes EmWeb/Server to effectively ignore the header.  Since so many
 * browsers exhibit this behaviour, this work-around is provided to ignore
 * extra characters after the date string.
 */
#define EW_CONFIG_OPTION_BROKEN_IMS_EXTRA_DATA

/*
 * BROKEN_NEED_OPAQUE - NCSA_Mosaic/2.7b5 and Spyglass_Mosaic/2.11
 * incorrectly require the server to generate an opaque parameter
 * in WWW-Authenticate: headers using Digest authentication.
 */
#define EW_CONFIG_OPTION_BROKEN_NEED_OPAQUE

/*
 * BACKWARD COMPATIBILITY with EmWeb-R2_x and EmWeb-R1_x ARCHIVES
 */
#define EW_CONFIG_OPTION_SUPPORT_ARCH1_0 /* server to handle old archives */

/*
 * SANITY CHECKING
 *
 * Define EMWEB_SANITY to include extra sanity checking code during initial
 * integration and debugging.  Code size may be reduced by not defining this.
 */
#define EMWEB_SANITY

/*
 * LOGGING
 */
/* #define EWA_LOG_HOOK                    * include ewaLogHook() interface */

/*
 * ACCEPT LANGUAGE CONTEXT
 * This feature has been enabled for Captive Portal. This allows the server
 * to obtain the accept-language preferences from the client browser. Please note
 * that we can no longer use TRANSPARENT CONTENT NEGOTIATION. FASTPATH however
 * never used this feature. This modification was approved by tgaunce :)
 */
#define EW_CONFIG_OPTION_ACCEPT_LANG

/*
 * TRANSPARENT CONTENT NEGOTIATION
 *
 * CONTENT_NEGOTIATION enables list-responses with TCN, Alternates and
 * Vary headers.
 *
 * CONTENT_NEGOTIATION_RVSA enables the Remote Variant Selection Algorithm
 * which can result in list or choice responses along with TCN, Alternates
 * and Vary headers.
 *
 * CONTENT_NEGOTIATION_EL_VARY enables the elaborate vary header. When defined
 * the Vary header will be in the verbose form (Vary: accept, accept-language).
 * Wen not defined, it will be Vary: *.
 */
#undef EW_CONFIG_OPTION_CONTENT_NEGOTIATION
#undef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA
#undef EW_CONFIG_OPTION_CONTENT_NEGOTIATION_EL_VARY


/*
 * Force server to do NO escaping of value field special characters, if defined
 */
#define EW_CONFIG_OPTION_NO_ENTITY_TRANSLATION /* LVL7_P2974 */

/******************************************************************************
 *
 * OPTIONAL PACKAGES
 *
 * The following configuration options enable optional functionality
 * licensed separately from the EmWeb core product.  These options should
 * only be defined if the corresponding package is installed.
 *
 * NOTE: When enabling and add-on package, you must edit the file
 * 'options.mak' to un-comment the Makefile rules for the package.
 * Those rules will be included in the master Makefile to build the
 * appropriate functionality.
 *****************************************************************************/

/* Options to activate the EmWeb/Client package. */
#undef EW_CONFIG_OPTION_CLIENT      /* EmWeb/Client Package */

/* #undef EW_CONFIG_OPTION_CLIENT_CACHE      // with Cacheing */
#undef EW_CONFIG_OPTION_CLIENT_COMPLETE_SPECIFIC      /* Client Request specific completion */


/* Options to activate the EmWeb/Mail package. */
/* #undef EW_CONFIG_OPTION_SMTP        // EmWeb/Mail Package */

/* Options to activate the SNMP namespace feature.
 * Activate SNMP_AGENT option, and the appropriate SNMP_*_AGENT
 * option for the SNMP agent you are using.
 */
/* #undef EW_CONFIG_OPTION_SNMP_AGENT  // interface hooks to SNMP agents */
/* #  undef EW_CONFIG_OPTION_SNMP_EPIC_AGENT   // SNMP Research */
/* #  undef EW_CONFIG_OPTION_SNMP_ENVOY_AGENT  // Epilogue */
/* #  undef EW_CONFIG_OPTION_SNMP_ATI_AGENT  // ATI Nucleus  */

/* Options to activate the EmWeb/CLI package. */
#define EW_CONFIG_OPTION_TELNET      /* EmWeb/Telnet Package */
#define EW_CONFIG_OPTION_CLI               /* EmWeb/CLI Package */

/* Options to activate the EmWeb/SSL package. */
/* #undef EW_CONFIG_OPTION_SSL_RSA     // RSA SSL-C integration package */

/* Options to activate the EmWeb/XMLP package. */
#undef EW_CONFIG_OPTION_XMLP     /* EmWeb/XMLP package */

/* Options to activate the EmWeb/UPnP package. */
#undef EW_CONFIG_OPTION_UPNP      /* UPNP option */

/* #undef EW_CONFIG_OPTION_UPNP_SSDP_CP // UPNP option with control point */
#undef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND  /* Support early Windows Me/XP versions */

/* #undef EW_CONFIG_OPTION_UPNP_NO_BYEBYE // do not send byebye messages upon loss of IP address */

/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
#include "flex.h"
#ifdef L7_XWEB_PACKAGE
#undef EW_CONFIG_OPTION_METHOD_TRACE
#define EMWEB_FILE_GET_OVERRIDE
#define EMWEB_FILE_PUT_OVERRIDE
#define EMWEB_FILE_READ_OVERRIDE
#define EMWEB_FILE_WRITE_OVERRIDE
#define EMWEB_FILE_CLOSE_OVERRIDE
#define EMWEB_FILE_OVERRIDE
#define EMWEB_URL_HOOK_OVERRIDE
#endif
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */
/* XUI */

#endif /* _EW_OPTIONS_H_ */

/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */

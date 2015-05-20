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
 * Check Option Selections and Dependancies
 *
 */

#ifndef _EWS_OPTCK_H_
#define _EWS_OPTCK_H_

/*
 * Protocol Version Dependant Options
 */
#if   EW_CONFIG_HTTP_PROTOCOL == HTTP_1_0

#  ifdef EW_CONFIG_OPTION_CHUNKED_OUT
#    error "EW_CONFIG_OPTION_CHUNKED_OUT invalid for HTTP_1_0"
/*
 * Chunked output is an HTTP/1.1 feature; setting this option with
 * support only for HTTP/1.0 may confuse some browsers because they
 * don't expect to see it in a response labeled 1.0
 * Yes, you may have gotten this combination in an earlier EmWeb release.
 */
#    undef EW_CONFIG_OPTION_CHUNKED_OUT
#  endif

#  ifdef EW_CONFIG_OPTION_AUTH_DIGEST
#    error "EW_CONFIG_OPTION_AUTH_DIGEST invalid for HTTP_1_0"
     /*
      * this option requires chunked encoding which is not supported
      * by this version of HTTP
      */
#    undef EW_CONFIG_OPTION_AUTH_DIGEST
#  endif

#elif EW_CONFIG_HTTP_PROTOCOL == HTTP_1_1

#  ifndef EW_CONFIG_OPTION_CHUNKED_IN
#    error "EW_CONFIG_OPTION_CHUNKED_IN required for HTTP_1_1"
/*
 * Technically, any 1.1 server MUST support chunk-encoded input.
 * In practice, we have not seen this yet from any browser.  If you do
 * not use any forms that send large amounts of data, it may be safe
 * to leave this support out, but could cause problems in the future.
 */
#  endif

#  ifndef EW_CONFIG_OPTION_CHUNKED_OUT
#    error "EW_CONFIG_OPTION_CHUNKED_OUT should be defined for HTTP_1_1"
/*
 * Without support for chunked output, the server will be forced to close
 * the connection to the browser for any page that contains dynamic
 * content.  This will negate much of the performance advantage of HTTP/1.1.
 */
#    ifdef EW_CONFIG_OPTION_AUTH_DIGEST
#    error "EW_CONFIG_OPTION_AUTH_DIGEST invalid without chunked encoding (output)"
     /*
      * this option requires chunked encoding on output
      * which provides support for HTTP message trailers
      */
#    undef EW_CONFIG_OPTION_AUTH_DIGEST
#    endif

#  endif

#else

#  error "EW_CONFIG_HTTP_PROTOCOL must be either HTTP_1_0 or HTTP_1_1"

#endif

#if (   defined(EW_CONFIG_OPTION_CACHE_ETAG) ||   defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) ) \
 && ( ! defined(EW_CONFIG_OPTION_CACHE_ETAG) || ! defined(EW_CONFIG_OPTION_CONDITIONAL_MATCH) )
#  error "EW_CONFIG_OPTION_CACHE_ETAG and EW_CONFIG_OPTION_CONDITIONAL_MATCH need each other"
#  define EW_CONFIG_OPTION_CACHE_ETAG
#  define EW_CONFIG_OPTION_CONDITIONAL_MATCH
#endif

#if defined(EW_CONFIG_OPTION_FILE_GET) \
 || defined(EW_CONFIG_OPTION_FILE_PUT) \
 || defined(EW_CONFIG_OPTION_FILE_DELETE)

#define EW_CONFIG_FILE_METHODS  /* shortcut for condition */

#endif /* EW_CONFIG_OPTION_FILE_GET | PUT | DELETE */

#ifdef  EW_CONFIG_OPTION_FIELDTYPE_FILE
#  ifndef EW_CONFIG_OPTION_RELEASE_UNUSED
#    error "EW_CONFIG_OPTION_RELEASE_UNUSED required with EW_CONFIG_OPTION_FIELDTYPE_FILE"
#    define EW_CONFIG_OPTION_RELEASE_UNUSED
#  endif /* EW_CONFIG_OPTION_RELEASE_UNUSED */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


/* if any of the FILE options are turned on, make sure the base option
 * EW_CONFIG_OPTION_FILE is turned on as well.
 */
#if ( defined(EW_CONFIG_OPTION_FILE_GET) || defined(EW_CONFIG_OPTION_FILE_DELETE)  \
|| defined(EW_CONFIG_OPTION_FILE_PUT) || defined(EW_CONFIG_OPTION_FIELDTYPE_FILE) )
#ifndef EW_CONFIG_OPTION_FILE
#    error "EW_CONFIG_OPTION_FILE required with EW_CONFIG_OPTION_FIELDTYPE_FILE or EW_CONFIG_OPTION_FILE_GET or EW_CONFIG_OPTION_FILE_DELETE or EW_CONFIG_OPTION_FILE_PUT"
#define EW_CONFIG_OPTION_FILE
#endif /* EW_CONFIG_OPTION_FILE */
#endif /* EW_CONFIG_OPTION_FILE_* */

/* EW_CONFIG_OPTION_FIELDTYPE_FILE outputs its data to the filesystem
 * using ewaFilePost, not ewaFilePut.  And since there is no other use
 * for ewaFilePost, there is no separate EW_CONFIG_OPTION_FILE_POST
 * option; its use is configured in by EW_CONFIG_OPTION_FIELDTYPE_FILE.
 */


#ifdef EW_CONFIG_OPTION_PRIMARY
#  ifndef EW_CONFIG_OPTION_RELEASE_UNUSED
#    error "EW_CONFIG_OPTION_RELEASE_UNUSED required with EW_CONFIG_OPTION_PRIMARY"
#    define EW_CONFIG_OPTION_RELEASE_UNUSED
#  endif /* EW_CONFIG_OPTION_RELEASE_UNUSED */
#  ifndef EW_CONFIG_OPTION_SCHED
#    error "EW_CONFIG_OPTION_SCHED required with EW_CONFIG_OPTION_PRIMARY"
#    define EW_CONFIG_OPTION_SCHED
#  endif /* EW_CONFIG_OPTION_RELEASE_UNUSED */
#endif /* EW_CONFIG_OPTION_PRIMARY */

#ifdef EW_CONFIG_OPTION_NS_SET
#  ifndef EW_CONFIG_OPTION_FORM
#    error "EW_CONFIG_OPTION_FORM required with EW_CONFIG_OPTION_NS_SET"
#    define EW_CONFIG_OPTION_FORM
#  endif /* EW_CONFIG_OPTION_FORM */
#  ifndef EW_CONFIG_OPTION_SCHED
#    error "EW_CONFIG_OPTION_SCHED required with EW_CONFIG_OPTION_NS_SET"
#    define EW_CONFIG_OPTION_SCHED
#  endif /* EW_CONFIG_OPTION_SCHED */
#endif /* EW_CONFIG_OPTION_NS_SET */

#ifdef EW_CONFIG_OPTION_NS_GET
#  ifndef EW_CONFIG_OPTION_STRING_VALUE
#    error "EW_CONFIG_OPTION_STRING_VALUE required with EW_CONFIG_OPTION_NS_GET"
#    define EW_CONFIG_OPTION_STRING_VALUE
#  endif /* EW_CONFIG_OPTION_STRING_VALUE */
#  ifndef EW_CONFIG_OPTION_SCHED
#    error "EW_CONFIG_OPTION_SCHED required with EW_CONFIG_OPTION_NS_GET"
#    define EW_CONFIG_OPTION_SCHED
#  endif /* EW_CONFIG_OPTION_SCHED */
#endif /* EW_CONFIG_OPTION_NS_GET */

#ifdef EW_CONFIG_OPTION_SNMP_AGENT
#  if !defined( EW_CONFIG_OPTION_NS_GET ) || !defined( EW_CONFIG_OPTION_NS_SET )
#    error "EW_CONFIG_OPTION_NS_GET and EW_CONFIG_OPTION_NS_SET required with EW_CONFIG_OPTION_SNMP_AGENT"
#  endif /* namespace support */
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#if defined(EW_CONFIG_OPTION_URL_HOOK)
#  define EW_OPTION_URL_HOOK
#  ifdef EW_CONFIG_OPTION_URL_HOOK_LIST
#    error "Can't define both EW_CONFIG_OPTION_URL_HOOK and EW_CONFIG_OPTION_URL_HOOK_LIST"
#  endif
#elif defined(EW_CONFIG_OPTION_URL_HOOK_LIST)
#  define EW_OPTION_URL_HOOK
#endif

#ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
#  if !defined( EW_CONFIG_OPTION_FIELDTYPE_FILE ) && !defined( EW_CONFIG_FILE_METHODS )
#    error "EW_CONFIG_OPTION_FIELDTYPE_FILE or EW_CONFIG_OPTION_FILE_GET or EW_CONFIG_OPTION_FILE_PUT or EW_CONFIG_OPTION_FILE_DELETE required with EW_CONFIG_OPTION_FILE_SYSTEM_LIST"
#  endif
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
#ifndef EW_CONFIG_OPTION_AUTH
#    error "EW_CONFIG_OPTION_AUTH_DIGEST requires EW_CONFIG_OPTION_AUTH option"
#endif
#endif
#ifdef EW_CONFIG_OPTION_AUTH_MDIGEST
#ifndef EW_CONFIG_OPTION_AUTH_DIGEST
#    error "EW_CONFIG_OPTION_AUTH_MDIGEST requires EW_CONFIG_OPTION_AUTH_DIGEST option"
#endif
#endif
#ifndef EW_CONFIG_OPTION_AUTH
#if defined(EW_CONFIG_OPTION_AUTH_DOMAIN) || defined(EW_CONFIG_OPTION_AUTH_NO_REALM_QUALIFIER)
#    error "EW_CONFIG_OPTION_AUTH_DOMAIN and _NO_REALM_QUALIFIER require EW_CONFIG_OPTION_AUTH option"
#endif
#endif

#if defined(EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE)
#ifndef EW_CONFIG_OPTION_AUTH_DIGEST
#error "EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE requires EW_CONFIG_OPTION_AUTH_DIGEST option"
#endif
#endif /* EW_CONFIG_OPTION_AUTH_DIGEST_CHANGE */

#if defined(EW_CONFIG_OPTION_ACCEPT_LANG)
# if defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION) \
  || defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA) \
  || defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION_EL_VARY)
# error "EW_CONFIG_OPTION_CONTENT_NEGOTIATION* may not be used with EW_CONFIG_OPTION_ACCEPT_LANG"
# endif
#elif defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION) \
   || defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION_RVSA) \
   || defined(EW_CONFIG_OPTION_CONTENT_NEGOTIATION_EL_VARY)
# if defined(EW_CONFIG_OPTION_ACCEPT_LANG)
# error "EW_CONFIG_OPTION_ACCEPT_LANG may not be used with EW_CONFIG_OPTION_CONTENT_NEGOTIATION*"
# endif
#endif

#if defined(EW_CONFIG_OPTION_UPNP)

#ifndef EW_CONFIG_OPTION_SCHED_SUSP_RES
#error "EW_CONFIG_OPTION_UPNP requires EW_CONFIG_OPTION_SCHED_SUSP_RES option"
#endif
#ifndef EW_CONFIG_OPTION_CLIENT
#error "EW_CONFIG_OPTION_UPNP requires EW_CONFIG_OPTION_CLIENT option"
#endif
#ifndef EW_CONFIG_OPTION_CLIENT_COMPLETE_SPECIFIC
#error "EW_CONFIG_OPTION_UPNP requires EW_CONFIG_OPTION_CLIENT_COMPLETE_SPECIFIC option"
#endif
#ifdef EW_CONFIG_OPTION_NO_ENTITY_TRANSLATION
#error "EW_CONFIG_OPTION_UPNP is incompatible with EW_CONFIG_OPTION_NO_ENTITY_TRANSLATION"
#endif
#if EW_CONFIG_HTTP_PROTOCOL == HTTP_1_0
#error "EW_CONFIG_OPTION_UPNP requires EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1"
#endif

/* If SOAP not defined, force it on.
 * (We set it directly because SOAP is an internal option, not a user option.)
 */
#ifndef EW_CONFIG_OPTION_SOAP
#define EW_CONFIG_OPTION_SOAP /* set by UPnP, requires SOAP */
#  if defined(EW_CONFIG_OPTION_SOAP)
#    ifndef EMWEB_EWS_STRING_COMPARE
#      define EMWEB_EWS_STRING_COMPARE
#    endif
#    ifndef EW_CONFIG_OPTION_FORM
#      error "EW_CONFIG_OPTION_SOAP requires EW_CONFIG_OPTION_FORM option"
#    endif /* EW_CONFIG_OPTION_FORM */
#    ifndef EMWEB_XMLP
#      define EMWEB_XMLP
#    endif
#    ifndef  EW_CONFIG_OPTION_XMLP
#if 0
#      error "EW_CONFIG_OPTION_SOAP requires EW_CONFIG_OPTION_XMLP option"
#endif
#      define EW_CONFIG_OPTION_XMLP
#    endif
#  endif /* EW_CONFIG_OPTION_SOAP */
#endif /* EW_CONFIG_OPTION_SOAP */

#else /* EW_CONFIG_OPTION_UPNP */

#  ifdef EW_CONFIG_OPTION_SOAP
#  error"EW_CONFIG_OPTION_SOAP may not be used without EW_CONFIG_OPTION_UPNP"
#  endif /* EW_CONFIG_OPTION_SOAP */
#undef EW_CONFIG_OPTION_UPNP_WINDOWS_WORKAROUND

#endif /* EW_CONFIG_OPTION_UPNP */

/* UPNP does not require that XMLP be defined -- XMLP activates the
 * application-visible XMLP interface.  UPNP automatically builds the XMLP
 * processing machinery, and accesses it through an internal interface.
 *
 * Note:  EMWEB_XMLP indicates that the XML parser is built with EmWeb server
 * contexts.  This may or may not indicate that the external "public"
 * application interfaces are specified.  EW_CONFIG_OPTION_XMLP refers to the
 * external public APIs which implies EMWEB_XMLP.  But, EMWEB_XMLP may also
 * be defined when internal APIs are used, e.g. for SOAP.
 */

#if   defined(EW_CONFIG_OPTION_XMLP) \
   || defined(EW_CONFIG_OPTION_SOAP)
#  define EMWEB_XMLP
#  define EMWEB_EWS_STRING_COMPARE
#  ifndef EW_CONFIG_OPTION_XMLP
#    define EMWEB_XMLP_ONLY_VIRATA_USERS
#  endif
#endif

#if defined(EW_CONFIG_OPTION_CLIENT)
#if EW_CONFIG_HTTP_PROTOCOL == HTTP_1_0
#error "EW_CONFIG_OPTION_CLIENT requires EW_CONFIG_HTTP_PROTOCOL >= HTTP_1_1"
#endif
#endif /* EW_CONFIG_OPTION_CLIENT */

/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EWS_OPTCK_H_ */

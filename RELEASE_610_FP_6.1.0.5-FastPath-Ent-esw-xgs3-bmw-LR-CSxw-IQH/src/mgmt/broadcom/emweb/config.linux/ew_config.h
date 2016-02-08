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
 * EmWeb common configuration
 *
 */
#ifndef _EW_CONFIG_H
#define _EW_CONFIG_H

/*
 * Set up basic EmWeb types.
 */
#include "ew_types.h"
/*
 * Server options moved to a separate file
 */
#include "ew_options.h"

#include "../../../l7public/common/commdefs.h"

/*
 * INCLUDES
 * Any system includes needed by application environment go here
 */
#if defined(WIN32) && ! defined(EMWEB_OSE)
#include <WINDOWS.H>
#endif /* defined(WIN32) && ! defined(EMWEB_OSE) */

#include <stdio.h> /* because the error and warning macros use printf */
/* ################################################################
 * Network Buffer Definitions
 *   We use the generic buffer library for EmStack (lib/ew_buffer.c)
 *   Redefine these interfaces to match our requirements.
 *################################################################ */
#include "ew_buffer.h"

#define EwaNetBuffer EbNetBuffer
#define EWA_NET_BUFFER_NULL EB_NET_BUFFER_NULL

#define ewaNetBufferAlloc     ebNetBufferAlloc
#define ewaNetBufferReset     ebNetBufferReset
#define ewaNetBufferFreeCheck ebNetBufferFreeCheck  
#define ewaNetBufferFree      ebNetBufferFree
#define ewaNetBufferLengthGet ebNetBufferLengthGet
#define ewaNetBufferLengthSet ebNetBufferLengthSet
#define ewaNetBufferDataGet   ebNetBufferDataGet
#define ewaNetBufferDataSet   ebNetBufferDataSet
#define ewaNetBufferNextGet   ebNetBufferNextGet
#define ewaNetBufferNextSet   ebNetBufferNextSet
#define ewaNetBufferPrepend   ebNetBufferPrepend
#define ewaNetBufferCopy      ebNetBufferCopy
#define ewaNetBufferShutdown  ebNetBufferShutdown

/*
 * PORT-SPECIFIC DEFINITIONS
 */
#include "osapi.h"
#define ewaAlloc(nbytes) osapiMalloc(L7_CLI_WEB_COMPONENT_ID, nbytes) /*lvl7_P0006*/
#define ewaFree(_x) osapiFree(L7_CLI_WEB_COMPONENT_ID, (_x))          /*lvl7_P0006*/

/*
 * TRACING AND LOGGING
 *
 * The following macros provide tracing and logging facilities using ANSI
 * printf() style arguments.
 *
 * They are called in statements of the following form:
 *
 *      EMWEB_XXX((format, arugment, argument, ...));
 *
 * the "list" is the same as a list of arguments to printf().  Note the
 * double parentheses.  This makes it possible for these macros to take
 * a variable number of arguments, if they are #define'd as:
 *
 *      #define EMWEB_XXX(x)  printf x
 *
 * They may not work correctly if you attempt to use them in other ways.
 * E.g., do not try to conditionalize using a conditional expression:
 *      failure ? EMWEB_ERROR((...)) : 0;       // <- does not work
 * Instead, use an 'if' statement:
 *      if (failure) { EMWEB_ERROR((...)); }    // <- works
 *
 * The normal definition of one of these macros so that it produces output is:
 *      #define EMWEB_ERROR(x)  printf x
 * The correct definition of one of these macros so that it has no effect is:
 *      #define EMWEB_ERROR(x)
 * (It is good practice when defining a macro to have a null expansion to
 * give the expansion as a comment, but I cannot write that inside this
 * comment.)
 *
 * There are other ways to "comment out" a macro, but making it null combines
 * several necessary properties:
 * 1) Followed by a semicolon, it is always syntactically valid where a
 *    "statement" is expected.
 * 2) It generates no code.
 * 3) It generates no warnings in gcc.
 * 4) It is strictly ANSI C.
 */
#define EMWEB_ERROR(x)  printf x
#define EMWEB_WARN(x)   printf x
/* Define EMWEB_TRACE to be no operation.  Replace calls with "{}", so
 * uses of EMWEB_TRACE() behave syntactically like a function call.
 * (Previously, it was replaced with the null string, which does not behave
 * correctly in some contexts.) */
#define EMWEB_TRACE(x)  /* printf x */ {}
#define EMWEB_TRACE_SOCKET(__fmt__, __args__... )  if (0) {}     /*            \
           {                                                                   \
            L7_char8  __buf__[128];                                            \
            (void)sprintf(__buf__, __fmt__, ## __args__);                      \
            printf(__buf__);                                                   \
          }   */                                                     
/* #undef EMWEB_TRACE_PARSE             // trace parser in detail */

/*
 * Application-specific Network handle
 */
typedef struct EwaNetHandle_s EwaNetHandle_t, *EwaNetHandle;
#define EWA_NET_HANDLE_NULL     ((EwaNetHandle) NULL)

#ifdef EW_CONFIG_OPTION_UPNP
/* A special NetHandle value.  Output sent to this NetHandle is discarded. */
#define  EWA_NET_HANDLE_DISCARD  &ewaNetHandleSpecialDiscardObject
extern struct EwaNetHandle_s ewaNetHandleSpecialDiscardObject;
#else
#define EWA_NET_HANDLE_DISCARD EWA_NET_HANDLE_NULL
#endif /* EW_CONFIG_OPTION_UPNP */

/*
 * Application-specific network handle cleanup function (or macro)
 * Invoked by EmWeb/Server after each processing each HTTP request
 * before invoking ewaNetHTTPEnd().
 */
extern void ewaNetHTTPCleanup( EwaNetHandle handle );

/*
 * Application-specific Document handle
 */
typedef void * EwaDocumentHandle;
#define EWA_DOCUMENT_HANDLE_NULL        ((EwaDocumentHandle) NULL)

#ifdef EW_CONFIG_OPTION_AUTH
/*
 * AUTHORIZATION
 *
 * The application may attach an application-specific handle to an entry
 * in the authentication database.  The C-type for this handle is defined
 * by the application below.
 */

/*lvl7_P0006 Start*/
typedef struct           /* user handle to ID entry */
{
    char        uname[32];
    char        access[32];
} appAuthHandle;
/*lvl7_P0006 End*/

typedef void * EwaAuthHandle;           /* user handle to ID entry */
#define EWA_AUTH_HANDLE_NULL    ((EwaAuthHandle) NULL)

#ifdef EW_CONFIG_OPTION_AUTH_DIGEST
/*
 * Application-specific nonce parameters - these are used for Digest
 * authentication to generate one-time challenges.  Recommended values
 * include the client's IP address (derived from the network handle),
 * a time-stamp, a server-side secret value, and other random bits and
 * pieces.  The EmWeb/Server generates an MD5 hash on these values (hiding
 * their semantics) to create the nonce value sent as challenges.
 */
typedef struct EwaAuthNonce_s
  {
    uint32      client_ip;
    uint32      timestamp;
    uint32      up_counter;
#   define      EWA_AUTH_SECRET_SIZE 8
    char        secret[EWA_AUTH_SECRET_SIZE];
  } EwaAuthNonce;

#endif /* EW_CONFIG_OPTION_AUTH_DIGEST */

#endif /* EW_CONFIG_OPTION_AUTH */

/*
 * RAW CGI INTERFACE
 *
 * The application may attach an application-specific handle to a raw CGI
 * request.  The C-type for this handle is defined by the application below.
 */
typedef void * EwaCGIHandle;
#define EWA_CGI_HANDLE_NULL     ((EwaCGIHandle) NULL)

/*
 * DISTRIBUTED EMWEB INTERFACE
 *
 * The application may configure one or more "secondary" EmWeb servers
 * through this "primary" EmWeb gateway.
 */
typedef void *EwaSecondaryHandle;
#define EWA_SECONDARY_HANDLE_NULL ((EwaSecondaryHandle) NULL)

/*
 * LOCKING
 *
 * The following macros must be defined to disable and enable task preemption
 * to protect critical regions if more than one pre-emptive task may access
 * EmWeb/Server API functions at the same time.
 */
#define EWA_TASK_LOCK()
#define EWA_TASK_UNLOCK()

/*
 * GLOBAL STATE
 */
typedef struct EwsState_s *EwsStateP;
/*
 * The ews_state is a pointer to global state information used internally
 * by EmWeb/Server.  In some environements, it may be desirable to maintain
 * multiple threads of EmWeb/Server within a single memory.  To support this
 * functionality, ews_state may be overridden by a macro that expands at
 * run-time to different values.  The only requirement is that the macro
 * returns type (EwsStateP), and that the address of the pointer (&ews_state)
 * can be determined.  For example:
 *
 * #define ews_state (*((EwsStateP*)someFunction(someArgument)))
 */

/*
 * LIBRARIES
 *
 * The EmWeb/Server implementation uses the following standard C library
 * functions:
 *
 *      sprintf - for converting decimal integers to strings, needed to
 *                generate Content-Length: HTTP headers.
 *      strcpy  - for copying null-terminated strings between the application
 *                and the EmWeb/Server (usually URL names).
 *      strcmp  - for comparing null-terminated strings (comparing URL names).
 *      strncmp - for comparing first 'n' characters of null-terminated
 *                strings.
 *      strlen  - get length of string in bytes
 *      memcpy  - copy an area of memory to another location
 *      memset  - set all bytes in an area of memory to a certain value
 *      strchr  -  returns  a  pointer  to the first
 *                 occurrence of the character c in the string s.
 *      strrchr - returns  a  pointer  to  the  last occurrence of the
 *                character c in the string s.
 *      strstr  - finds the first occurrence of a substring in a string.
 *      strcat  - concatenate two strings.
 *
 * Most embedded environments have these functions available in libraries.
 * However, EmWeb/Server can be configured to provide its own internal
 * implementations for this purpose.  (Note that a general sprintf
 * implementation is not provided.  Instead, only a conversion routine from
 * long integers to strings is needed.
 *
 * The following macros should be defined (#define) if the corresponding
 * function is provided by the application environment.  Otherwise,
 * the sample implementation given in lib/ew_lib.h can be used.
 */
/* #define EMWEB_HAVE_SPRINTF */
/* #define EMWEB_HAVE_STRCPY */
/* #define EMWEB_HAVE_STRCMP */
/* #define EMWEB_HAVE_STRNCMP */
/* #define EMWEB_HAVE_STRLEN */
/* #define EMWEB_HAVE_MEMCPY */
/* #define EMWEB_HAVE_MEMSET */
/* #define EMWEB_HAVE_MEMCMP */
/* #define EMWEB_HAVE_STRCHR */
/* #define EMWEB_HAVE_STRSTR */
/* #define EMWEB_HAVE_STRCAT */
/* If you have 'index' but not 'strchr', define EMWEB_HAVE_STRCHR as:
 *   #define EMWEB_STRCHR(s,c) index(s,c)
 */

/*
 * SERVER FILE SUPPORT
 *
 * For support of a server side file system.
 */

typedef struct EwaFileHandle_s *EwaFileHandle;
#define EWA_FILE_HANDLE_NULL ((EwaFileHandle) NULL)
typedef const char *EwaFileName;

/*
 * MAX_QUEUED_RECV_BUFS - Configure this with a value to limit the number
 * of network buffers used at a time per request. (Note, set this value in
 * order to prevent a request from using up all the resources).
 * If a request exceeds this limit, then the request is aborted and
 * resources are freed. The value should be chosen by the developer to
 * tune the system and is dependent upon the size of the buffers used, the
 * number of buffers used etc.
 * It is recommended that this value be chosen such that a request with 1500
 * bytes can be supported.
 *
 * Note: The old buffer limit was 16 which never came close to the real
 * buffer count of 256 (as defined in cliWebPhaseOneInit which it passes
 * 256 into the L7_ewsPhaseOneInit function - cli_web_util.c). The largest
 * implementation to date is a captive portal textarea that is capable of
 * receiving 32768 characters, this roughly equates to 67 buffers. 128
 * should be sufficient.
 */
#define EW_CONFIG_OPTION_MAX_QUEUED_RECV_BUFS 128 /* 16 */


#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
 /*
  * RESUME WAKEUP CALL
  *
  * If you suspend processing of a request using ewsSuspend, and then resume
  * it from another task using ewsResume, you may need to do something to
  * cause the EmWeb/Server task to wake up and invoke ewsRun() to process the
  * resumed request.
  * This application function will be called from within ewsResume to allow
  * you to do whatever must be done to wake up the server task.  If you don't
  * need it, use the preprocessor to stub it out.
  */
#define ewaResumeWakeup( context ) /* stub definition */
#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */


#ifdef EW_CONFIG_OPTION_CLIENT
/* ****************************************************************
 * Application handle for EmWeb/Client
 *   EmWeb/Client is an add-on product; contact sales@agranat.com
 *   for information
 * **************************************************************** */
typedef struct EwaClientHandle_s *EwaClientHandle;
#define EWA_CLIENT_HANDLE_NULL ((EwaClientHandle) NULL)
#endif /* EW_CONFIG_OPTION_CLIENT */

#ifdef EW_CONFIG_OPTION_SMTP
/* ****************************************************************
 * Application handle for SMTP Mail
 *   EmWeb/Mail is an add-on product; contact sales@agranat.com
 *   for information
 * **************************************************************** */
#  define EW_CONFIG_SMTP_DOMAIN_MAXLEN 64
#  define SMTP_ADDR_LEN 64
#  define SMTP_SUBJECT_LEN 128
#  define SMTP_MAX_RECIPIENTS 8

typedef struct EwaSMTPparams* EwaSMTPHandle;
#else
#  define SMTP_MAX_RECIPIENTS 8
#endif /* EW_CONFIG_OPTION_SMTP */


#ifdef EW_CONFIG_OPTION_SNMP_AGENT
/* ****************************************************************
 * Application-specific types for SNMP Agent support
 *   Some commercial snmp agents are supported by EmWeb;
 *   contact sales@agranat.com for more information.
 * **************************************************************** */
typedef struct EwaSNMPAgentHandle_s *EwaSNMPAgentHandle;
#define EWA_SNMP_AGENT_HANDLE_NULL  ((EwaSNMPAgentHandle) NULL)

#if defined( EW_CONFIG_OPTION_SNMP_EPIC_AGENT )   /* SNMP Research's EPIC interface */
  typedef struct _EpicAuthHandle *EwaSNMPAgentAuthHandle;
#elif defined( EW_CONFIG_OPTION_SNMP_ENVOY_AGENT )   /* ISI's Envoy */
  typedef struct _EnvoyAuthHandle *EwaSNMPAgentAuthHandle;
#else   /* agent unknown */
  typedef void *EwaSNMPAgentAuthHandle;
#endif  /* snmp agent type */

#define EWA_SNMP_AGENT_AUTH_HANDLE_NULL ((EwaSNMPAgentAuthHandle)NULL)
typedef EwaSNMPAgentAuthHandle EwaSNMPAgentState;

#endif /* EW_CONFIG_OPTION_SNMP_AGENT */

/* ****************************************************************
 * XML from HTML Conversion (EW_CONFIG_OPTION_CONVERT_XML)
 *   define the default "mime type" to send when serving the dynamic
 *   content of an HTML file as XML ("XMLout" mode).
 * **************************************************************** */
#define EWA_CONVERT_XML_CONTENT_TYPE   "text/xml"

/* ****************************************************************
 * Skeleton XML (EW_CONFIG_OPTION_CONVERT_SKELETON)
 *   define the default "mime type" to send when serving a page in
 *   "skeleton" mode, which shows where the dynamic content goes in
 *   the static content.
 * **************************************************************** */
#define EWA_CONVERT_SKELETON_CONTENT_TYPE   "text/xml"

/* ****************************************************************
 * "TEXTAREA" conversion (EW_CONFIG_OPTION_CONVERT_TEXTAREA)
 *   define the default "mime type" to send when encapsulating a
 *   document in a "TEXTAREA hack".
 * **************************************************************** */
#define EWA_CONVERT_TEXTAREA_CONTENT_TYPE   "text/html"

#ifdef EW_CONFIG_OPTION_UPNP

/* ****************************************************************
 * Application's handle for each service instance for EmWeb/UPnP
 *   EmWeb/UPnP is an add-on product; contact sales@agranat.com
 *   for information.
 * **************************************************************** */
typedef struct EwaUPnPServiceHandle_s *EwaUPnPServiceHandle;
#define EWA_UPNP_SERVICE_HANDLE_NULL ((EwaUPnPServiceHandle) NULL)

/* To get access to the system time type. */
#include <time.h>

/*
 * Definition of the time value field to be used in the timer event structure.
 * To be defined for the convenience of the code that implements the timer
 * system.
 */
typedef time_t EwaTime;

#endif /* EW_CONFIG_OPTION_UPNP */

#endif /* _EW_CONFIG_H */

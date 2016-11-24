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
 * EmWeb/Server system interfaces
 *
 */

#ifndef _EWS_SYS_H_
#define _EWS_SYS_H_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_def.h"

/******************************************************************************
 *
 * INITIALIZATION AND SHUTDOWN
 *
 * The EmWeb/Server must be initialized at start-up before accepting requests,
 * installing document archives, or registering authentication information.
 *
 *****************************************************************************/

/*
 * ewsInit
 * Initialize EmWeb/Server
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
extern EwsStatus ewsInit ( void );

#ifdef EW_CONFIG_OPTION_CLEANUP
/*
 * ewsShutdown
 * Graceful shutdown of EmWeb/Server terminating all requests in progress
 * and releasing all memory and buffers.
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
extern EwsStatus ewsShutdown ( void );
#endif /* EW_CONFIG_OPTION_CLEANUP */

#ifdef EW_CONFIG_OPTION_SCHED
/******************************************************************************
 *
 * SCHEDULING
 *
 * The EmWeb/Server is capable of both multi-threading HTTP requests and
 * yielding execution control to the CPU.  The following functions are provided
 * to give the application control over how EmWeb/Server schedules  pending
 * requests.
 *
 *****************************************************************************/

/*
 * ewsRun
 * Reschedule request processing after control returned to application as a
 * result of returning EWA_STATUS_OK_YIELD from ewaNetHTTPSend(),
 * ewaNetHTTPEnd(), or ewaDocumentFault(), or as the value to the status
 * parameter in ewsResume().
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
extern EwsStatus ewsRun ( void );

#ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
/*
 * ewsSuspend
 * Suspend processing of the current request during an EmWeb callout function
 * (e.g. EmWebString, EmWebFormServe, or EmWebFormSubmit).  This function is
 * used in the implementation of proxies.
 *
 * Note that this does _not_ cause the server to stop processing requests; in
 * fact, it forces a flush of any partially filled buffer for the current
 * request and may process other pending requests.  This just causes the
 * current callout to be marked as 'suspended'.  When the current callout
 * returns, the result will be ignored - after ewsResume (below) is called
 * for this same context, the callout will be repeated.
 *
 * context      - context of request to be suspended
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
extern EwsStatus ewsSuspend ( EwsContext context );

/*
 * ewsResume
 * Resume processing of a request previously suspended by ewsSuspend().  This
 * causes the request to be rescheduled, and the callout from which ewsSuspend
 * had been called will be reinvoked.
 *
 * context      - Context of suspended request to be resumed
 * status       - EWA_STATUS_OK or EWA_STATUS_OK_YIELD.
 *
 * Returns EWS_STATUS_OK on success, else failure code (TBD).
 */
EwsStatus ewsResume ( EwsContext context, EwaStatus status );

#ifndef ewaResumeWakeup
/*
 * ewaResumeWakeup
 * Optional user callout.  Called by ewsResume() to allow application
 * to schedule the emweb task to wake up.  The implementation of this
 * routine is application-specific.
 */
extern void ewaResumeWakeup( EwsContext context );
#endif /*ewaResumeWakeup*/

#endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

#endif /* EW_CONFIG_OPTION_SCHED */

/******************************************************************************
 *
 * MEMORY MANAGEMENT
 *
 * The EmWeb/Server presumes that the target O/S has a dynamic memory
 * management capability roughly equivalent to the POSIX malloc() and free()
 * functions.
 *
 * Alternatively, the application could create a static linked-list of memory
 * blocks of different sizes.  Refer to the product documentation for
 * information about run-time memory requirements.
 *
 * The application is responsible for providing the following memory management
 * functions to the EmWeb/Server.
 *
 *****************************************************************************/

#if !defined( ewaAlloc ) && !defined( _ewaAlloc )
/*
 * ewaAlloc
 * Allocate a block of memory of at least the requested number of bytes.
 *
 * bytes        - requested size of the memory block in bytes
 *
 * Returns a pointer to the first byte of memory, or NULL if no memory of the
 * requested size is available.
 */
extern void * ewaAlloc ( uintf bytes );
#endif

#ifndef ewaFree
/*
 * ewaFree
 * Free a block of memory previously allocated by ewaAlloc()
 *
 * pointer      - pointer to beginning of memory block (from ewaAlloc())
 *
 * No return value
 */
extern void ewaFree ( void * pointer );
#endif

#ifdef EW_CONFIG_OPTION_DATE

/******************************************************************************
 *
 * TIME-OF-DAY MANAGEMENT
 *
 * If DATE support is enabled, the EmWeb/Server invokes the following
 * application-provided function to retrieve the current date for use in
 * Date: HTTP headers.  Furthermore, the current date is also used in
 * Expire: and Last-modified: headers of dynamically generated documents
 * (i.e. HTML documents containing EMWEB extensions), if configured.
 * Note that the Last-modified: header of static documents use the archive
 * creation date stored in the archive by the EmWeb/Compiler.
 *
 *****************************************************************************/

#ifndef ewaDate
/*
 * ewaDate
 *
 * Return a string containing the current time-of-day in one of the HTTP
 * standard representations as follows:
 *
 * RFC1123: (Preferred standard)
 *   Fri, 28 Jun 1996 15:57:28 GMT
 *
 * RFC850: (Used, but not preferred)
 *   Friday, 28-Jun-96 15:57:28 GMT
 *
 * asctime: (Allowed, but discouraged)
 *   Jun  6 15:57:28 1996
 */
extern const char * ewaDate ( void );
#endif

const char *ewaDate2(time_t *tp, char *date);
const char *ewaDate1036(time_t *tp, char *date1036);

#endif /* EW_CONFIG_OPTION_DATE */

#ifndef ewaTime
/*
 * ewaTime
 *
 * Return number of seconds since 00:00:00 GMT 01-Jan-1970.  This is
 * currently used only for EmWeb/Client with cacheing.
 */
extern uint32 ewaTime ( void );
#endif

#ifdef EW_CONFIG_OPTION_URL_HOOK

/******************************************************************************
 *
 * URL REWRITING
 *
 * If URL rewriting support is enabled, the EmWeb/Server will invoke the
 * following function when each request has been successfully parsed, but
 * before the URL is looked up in the filesystem.  The application can then
 * return a new (or the same) URL to actually serve.
 *
 *****************************************************************************/

#ifndef ewaURLHook
/*
 * ewaURLHook
 *
 * context      - context of request
 * url          - requested URL
 *
 * Returns new URL, or NULL to cause abort.  (For no-op, simply return url).
 *
 * Note: the application MUST NOT invoke ewsNetHTTPAbort from here!  Return
 * a NULL pointer instead!
 *
 * The URL may be rewritten in place if the length is not increased;
 * if the returned pointer is not within the url parameter, the value is
 * copied by the server.
 */
extern char * ewaURLHook ( EwsContext context, char *url );
#endif

#endif /* EW_CONFIG_OPTION_LOG_HOOK */

/*
 * Since content length of zero is valid, we cannot use zero to
 * indicate when the content length is unknown (eg. dynamic data).
 * Assuming nobody will ever try to send/receive a 4gig document is
 * probably safe for now...
 */
#define EWS_CONTENT_LENGTH_UNKNOWN ((int32) -1)

#ifdef EW_CONFIG_OPTION_FILE
/***********************************************************************
 *
 * LOCAL FILE SYSTEM SUPPORT
 *
 * The following routines must be supplied to support the server's
 * local file system.
 */

typedef union EwsFileParams_s
{
  /*
   * fileField - for support of the form INPUT TYPE=FILE field.
   * The server fills out this structure, and passes it to
   * the application when the file is submitted
   */
# ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  struct
  {
    const char *fileName;        /* file name or NULL */
    const char *contentType;     /* MIME type */
    const char *contentEncoding; /* content encoding or NULL */
    const char *contentDisposition; /* Content-disposition: */
    int32       contentLength;   /* length or EWS_CONTENT_LENGTH_UNKNOWN */
  } fileField;
# endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


  /*
   * fileInfo - for support for local file operations (GET,HEAD,OPTIONS,
   * PUT,DELETE).  This structure is set up by the application when a URL
   * that corresponds to a local file is received.  This structure
   * gives the server all it needs to know to handle the file operation.
   */
# if defined( EW_CONFIG_OPTION_FILE_GET ) \
  || defined( EW_CONFIG_OPTION_FILE_PUT ) \
  || defined( EW_CONFIG_OPTION_FILE_DELETE )
  struct
  {
    EwaFileName fileName;        /* file name (opaque) */
    const char *contentType;     /* MIME type */
    const char *contentEncoding; /* content encoding or NULL */
#   ifdef EW_CONFIG_OPTION_DISPOSITION
    const char *contentDisposition; /* set to file name to force save */
#   endif /* EW_CONFIG_OPTION_DISPOSITION */
    const char *contentLanguage; /* content language or NULL */
    const char *eTag;            /* ETag value or "" for file present or NULL */
    const char *lastModified;    /* modification time (RFC1123) or NULL */
    const char *lastModified1036;/* modification time (RFC1036) or NULL */
    const char *realm;           /* auth realm or NULL */
    int32      contentLength;    /* length or EWS_CONTENT_LENGTH_UNKNOWN */
    EwsRequestMethod   allow;    /* methods allowed: ewsRequestMethodGet |... */
    /* HEAD & OPTION _always_ allowed by default */
    boolean    nocache;          /* TRUE if not cachable */

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES
    boolean acceptRanges; /* TRUE if ranges allowed, FALSE otherwise */
    boolean isRange;      /* TRUE if this response is a range, else FALSE */
    int32  firstBytePos; /* first byte returned, or 0 */
    int32  lastBytePos;  /* last byte, or EWS_CONTENT_LENGTH_UNKNOWN */
    int32  instanceLength; /* document length, or EWS_CONTENT_LENGTH_UNKNOWN */
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */

  } fileInfo;
# endif /* local file system GET/PUT/DELETE */

  char reserved1;               /* prevent NULL union */

} EwsFileParams, *EwsFileParamsP;

typedef enum
  {
    EWA_FILE_STATUS_OK          /* Normal file close */
   ,EWA_FILE_STATUS_CREATED     /* For PUT method, resource created */
   ,EWA_FILE_STATUS_ERROR       /* Error file close */
  } EwaFileStatus;

#ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
typedef EwaFileStatus EwaFileClose_f(EwaFileHandle handle, EwsStatus status);
typedef EwaFileClose_f *EwaFileClose;
typedef sintf EwaFileWrite_f(EwsContext context
                            , EwaFileHandle handle
                            , const uint8  *datap
                            , uintf         length
                            );
typedef EwaFileWrite_f *EwaFileWrite;
typedef sintf EwaFileRead_f (EwsContext context
                           , EwaFileHandle handle
                           , uint8 *datap
                           , uintf length
                            );
typedef EwaFileRead_f *EwaFileRead;
#ifdef EW_CONFIG_OPTION_FILE_GET
typedef EwaFileHandle EwaFileGet_f( EwsContext context
                                  ,const char *url
                                  ,const EwsFileParams *params
                                 );
typedef EwaFileGet_f *EwaFileGet;
#endif /* EW_CONFIG_OPTION_FILE_GET */
#ifdef EW_CONFIG_OPTION_FILE_PUT
typedef EwaFileHandle EwaFilePut_f( EwsContext context
                                   , const EwsFileParams *params
                                   );
typedef EwaFilePut_f *EwaFilePut;
#endif /* EW_CONFIG_OPTION_FILE_PUT */
#ifdef EW_CONFIG_OPTION_FILE_DELETE
typedef EwaStatus EwaFileDelete_f( EwsContext context
                                 ,const EwsFileParams *params
                                );
typedef EwaFileDelete_f *EwaFileDelete;
#endif /* EW_CONFIG_OPTION_FILE_DELETE */

typedef struct EwsFileSystem_s
{
    EwaFileWrite   fileWrite;
    EwaFileRead    fileRead;
    EwaFileClose   fileClose;
#ifdef EW_CONFIG_OPTION_FILE_GET
    EwaFileGet     fileGet;
#endif
#ifdef EW_CONFIG_OPTION_FILE_PUT
    EwaFilePut     filePut;
#endif
#ifdef EW_CONFIG_OPTION_FILE_DELETE
    EwaFileDelete  fileDelete;
#endif
} EwsFileSystem, *EwsFileSystemP;

/*
 * The following are the support functions for multiple
 * file system functions.
 *
 */

typedef struct EwsFileSystemEntry_s *EwsFileSystemHandle;
#define EWS_FILE_SYSTEM_HANDLE_NULL NULL

/*
 * ewsFileSystemRegister
 *   Adds a set of file system functions to the file_system_list.
 *
 */
extern EwsFileSystemHandle  ewsFileSystemRegister( EwsFileSystemP fileSystemP );

/*
 * ewsFileSystemUnregister
 *   Removes a set of file system functions from the File System List.
 *
 */
extern EwsStatus ewsFileSystemUnregister( EwsFileSystemHandle handle );
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

/*
 * ewaFileClose
 * Closes the file and indicates success or failure
 */

extern EwaFileStatus
ewaFileClose ( EwaFileHandle handle, EwsStatus status );

/*
 * ewaFileWrite
 * Write data to a file
 */
extern sintf
ewaFileWrite ( EwsContext context
               , EwaFileHandle handle     /* handle from ewaFileOpen */
               , const uint8  *datap      /* pointer to data */
               , uintf         length     /* length of data */
               );
/* returns bytes written, <0 on error */


/*
 * ewaFileRead
 * Read data from a  file
 */
extern sintf
ewaFileRead ( EwsContext context
             , EwaFileHandle handle    /* handle from ewaFileOpen */
             , uint8 *datap          /* pointer to data buffer */
             , uintf length          /* length of buffer */
              );
/* returns bytes read, 0 on EOF, <0 on error */


#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
/*
 * ewaFilePost
 * For multipart/form-data file submissions.  Before the application's
 * submit function is called, the server may invoke this function for
 * each received file as part of a <INPUT TYPE=FILE> element in the
 * form.  If successful, the file handle will be passed to the
 * application's submit function at which point it is the application's
 * responsibility to close the file.
 */
extern EwaFileHandle
ewaFilePost ( EwsContext context
             ,const EwsFileParams *params
#ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
             ,EwsFileSystemHandle *fileSystemHandle
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
            );
/* returns EWA_FILE_HANDLE_NULL on error */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#ifdef EW_CONFIG_OPTION_FILE_GET
/*
 * ewaFileGet
 * When the server determines that the file set by ewsContextSetFile
 * should be served to the browser, this function is invoked to open
 * the file for reading.
 *
 * To support Conditional GET, the request message may include
 * If-Modified-Since, If-Unmodified-Since, If-Match,
 * If-None-Match, or If-Range headers.  In this case, the
 * server will use the information given in the returned
 * params block to determine if the file should be retrieved.
 */
extern EwaFileHandle
ewaFileGet ( EwsContext context
            ,const char *url
            ,const EwsFileParams *params /* from ewsContextSetFile() */
           );
/* returns EWA_FILE_HANDLE_NULL on error */

#endif /* EW_CONFIG_OPTION_FILE_GET */

#ifdef EW_CONFIG_OPTION_FILE_PUT
/*
 * ewaFilePut
 * For PUT and POST methods where the URL is a local file as set by
 * ewsContextSetFile, the server will invoke this function to
 * open a file for writing in response to a PUT or POST method request
 */
extern EwaFileHandle
ewaFilePut ( EwsContext context
           , const EwsFileParams *params /* from ewsContextSetFile() */
           );
/* returns EWA_FILE_HANDLE_NULL on error */
#endif /* EW_CONFIG_OPTION_FILE_PUT */

#ifdef EW_CONFIG_OPTION_FILE_DELETE
/*
 * ewaFileDelete
 * For DELETE methods where the URL is a local file as set by
 * ewsContextSetFile, the server will invoke this function to
 * delete the file.
 */
EwaStatus
ewaFileDelete ( EwsContext context
               ,const EwsFileParams *params /* from ewsContextSetFile() */
              );
/* returns EWA_STATUS_OK on success, else EWA_STATUS_ERROR */
#endif /* EW_CONFIG_OPTION_FILE_DELETE */

#endif /* EW_CONFIG_OPTION_FILE */

#ifdef EW_CONFIG_OPTION_GROUP_DYNAMIC
/*
 * ewaGroupDynamicFlush
 * Application must perform "group get" operation to fill in all deferred
 * values.
 */
void
ewaGroupDynamicFlush ( EwsContext context );

/*
 * ewsGroupDynamic
 * Request server defer get operation for later grouping.  Returns pointer
 * to pointer in which to store result of get, and flag indicating if this
 * memory is static or dynamic.
 */
void * *
ewsGroupDynamic ( EwsContext context, boolean dynamic );

#endif /* EW_CONFIG_OPTION_GROUP_DYNAMICS */

#ifdef EW_CONFIG_OPTION_SECONDARY_ACTION
/*
 * ewaSecondaryPrefixAction
 * Allows the application implemented as a Secondary EmWeb/Server to
 * prefix an identifier onto the value of the ACTION= field
 * in a form before it is transferred to the primary (and out to the
 * browser).
 * It is up to the Primary EmWeb/Server to recognize this special
 * URL when the form is eventually submitted for its secondary,
 * and strip off the special prefix before forwarding the request
 * (in ewaURLHook).
 * For example, this routine can be used to add a special prefix
 * identifying the target secondary, which the primary strips off
 * before forwarding.
 *
 * context - current context (on secondary).
 * action - value of ACTION from document.
 * returns: a pointer to a null terminated string containing
 *      the special prefix that will be prepended to action
 *      or NULL if no prefix.  Note that the prefix should
 *      not end in a / character if action starts with a /
 *
 * NOTE WELL: this interface is ONLY supported for the use stated
 * above - it is explicitly NOT SUPPORTED as a general means of
 * changing the Action url.
 */
#ifndef ewaSecondaryPrefixAction
extern const char *ewaSecondaryPrefixAction( EwsContext context
                                            ,const char *action );

#endif  /* ewaSecondaryPrefixAction */
#endif /* EW_CONFIG_OPTION_SECONDARY_ACTION */

#ifdef EW_CONFIG_OPTION_SNMP_AGENT
/*
 * Interfaces required by snmp agent interface code
 */

extern EwaSNMPAgentState ewsSNMPAgentGetState( void );
extern EwaSNMPAgentState ewsSNMPAgentSetState( EwaSNMPAgentState );

#ifndef ewaSNMPAgentHandleCleanup
extern void ewaSNMPAgentHandleCleanup( EwsContext, EwaSNMPAgentHandle );
#endif

#ifndef ewaSNMPAgentInit
extern EwaSNMPAgentState ewaSNMPAgentInit( void );
#endif

#ifndef ewaSNMPAgentShutdown
extern void ewaSNMPAgentShutdown( EwaSNMPAgentState );
#endif

#ifdef EW_CONFIG_OPTION_AUTH
#ifndef ewaSNMPAgentAuthHandleFree
extern void ewaSNMPAgentAuthHandleFree( EwaSNMPAgentAuthHandle );
#endif
#endif /* EW_CONFIG_OPTION_AUTH */
#endif /* EW_CONFIG_OPTION_SNMP_AGENT */

#ifdef EW_CONFIG_OPTION_ETAG_EXTENSION
#ifndef ewaEtagExtension
extern const char * ewaEtagExtension( EwsContext context );
#endif
#endif /* EW_CONFIG_OPTION_ETAG_EXTENSION */

#endif /* _EWS_SYS_H_ */

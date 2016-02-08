/*
 * Release: R6_2_0
 *
 * EmWeb POSIX Reference Port - Network Handle definition
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
 */

#ifndef _EWNET_H_
#define _EWNET_H_

#include "osapi_sockdefs.h"

#ifndef _EWNET_C_
#   define _EXTERN_EWNET extern
#else
#   define _EXTERN_EWNET
#endif

#include "posix_config.h"
#include "ews_sys.h"

#ifdef EW_CONFIG_OPTION_SSL_RSA
#include "sslc.h"
#endif

/* ================================================================
 * Types & Constants
 * ================================================================ */

#ifdef EMWEB_POSIX_OPTION_SERVER_PUSH
typedef enum
  {
    SSP_STATE_OFF, /* disabled */
    SSP_STATE_ON,  /* selected */
    SSP_STATE_WAIT /* suspended */
  } SSPState;      /* server-side-push state */
#endif /* EMWEB_POSIX_OPTION_SERVER_PUSH */

#ifdef EW_CONFIG_OPTION_FILE
#include "ew_md5.h"

typedef enum FileHandleType_e
  {
    fileHandleUnix
    ,fileHandleMD5Example
    ,fileHandleStream
    ,fileHandleArchive
  } FileHandleType;

typedef struct EwaFileHandle_s
  {
    FileHandleType type;
    int            fd;
    FILE          *file;
    MD5_CTX        md5_ctx;
    void          *params;
    EwsContext    context;
    char          *localPath; /* local file pathname */
    /* XUI */
    /* XUI */
    /* XUI */
    /* XUI */
    void          *appWap;
    void          *appFunc;
    /* XUI */
    /* XUI */
    /* XUI */
    /* XUI */

#   ifdef EW_CONFIG_OPTION_BYTE_RANGES
    boolean       isRange;
    uint32        bytesRemaining;
#   endif /* EW_CONFIG_OPTION_BYTE_RANGES */

  } EwaFileHandle_t;
#endif /* EW_CONFIG_OPTION_FILE */

# if defined (EW_CONFIG_OPTION_SSL_RSA)
typedef enum
{
   sslStateHandshake,
   sslStateUp
} SSLState;
#   endif /* EW_CONFIG_OPTION_SSL_RSA */

typedef struct ew_sockaddr_union_s
{
  union{
	struct sockaddr      sa;
	struct sockaddr_in   sa4;
	struct sockaddr_in6  sa6;
  }u;
}ew_sockaddr_union_t;

struct EwaNetHandle_s
{

/************************************* XUI *********************************/
  /* Repeat Variables */
  int repeatCount;
  int repeatMax;
  int repeatCounter;
  int repeatSplitCount;
  char repeatRetVal[32];
  char numberBuf[32];
  char lvlCheckBuf[32];
  boolean repeatMore;
  /* Repeat Variables */
  int fileType;     /* BRCM: for http download */
  unsigned int submitType;
  boolean serveAfterSubmit;
  void *app_pointer2;   /* BRCM: xlib object cache */
  char *filterDumpRead;
  char *filterDumpWrite;
  void *filters;        /* BRCM: xlib filter cache */
  void *workarea;
  char downloadUrl[128];
#define EW_ERROR_BUF_SIZE 1024
  char errMsg[EW_ERROR_BUF_SIZE];
# define              APP_POINTER_C2(context)  \
                          (ewsContextNetHandle(context)->app_pointer2)
/************************************* XUI *********************************/

  boolean discard;
  /*
   * Used in POSIX port of EmWeb/Server
   */
  int                 handleNum;      /* lvl7_P0006 */ /*JWL*/
  SOCKET              socket;         /* BSD socket number for request */
  int                 listeningPort;  /* HTTP/S server listening port */
/* Always define peer, as this field is used by Lvl7 code */
/* # ifdef EW_CONFIG_OPTION_AUTH_DIGEST */
  ew_sockaddr_union_t  peer;           /* peer addr for nonces */
/* # endif */

# define EW_CONNECTION_SERIAL    1
# define EW_CONNECTION_TELNET    2
# define EW_CONNECTION_SSH       3
# define EW_CONNECTION_HTTP      4
# define EW_CONNECTION_HTTPS     5
  int    connection_type;
  time_t lastTime;
  struct EwsContext_s *context;       /* pointer back to EmWeb context */
  boolean             idle;           /* activity since last tick? */
# ifdef EWA_LOG_HOOK
# ifdef EMWEB_LOOKUP_HOST
  char   sourceAddrStr[256];
# else /* EMWEB_LOOKUP_HOST */
  char   sourceAddrStr[16];
# endif /* EMWEB_LOOKUP_HOST */
# endif /* EWA_LOG_HOOK */
  EwaNetBuffer        pending_xmit; /* because the socket would have blocked */
  boolean             close_pending;
# ifdef EW_CONFIG_OPTION_IN_FLOW
  boolean             in_flow_control;
# endif /* EW_CONFIG_OPTION_IN_FLOW */
# ifdef EW_CONFIG_OPTION_CLIENT
  boolean             connect_pending; /* TRUE if connection pending */
  EwaClientHandle     clientHandle;    /* handle for client transaction */
  char                *tmpFile;        /* temporary file for cleanup */
  char                *tmpFile2;       /* temporary file for cleanup */
# ifdef EW_CONFIG_OPTION_CLIENT_CACHE
  EwaFileHandle_t     cache_handle;    /* file handle for cache file */
# endif /* EW_CONFIG_OPTION_CLIENT_CACHE */
# endif /* EW_CONFIG_OPTION_CLIENT */

  /*
   * Data areas used by application to support server-side dynamic HTML
   *
   * The macros with the "_C" suffixes take one argument, which is the
   * EmWeb context which points to the network handle that contains the
   * data area.  The macros without the "_C" suffix implicitly use ewsContext
   * as the EmWeb context pointer.
   */
  int                 app_data;       /* application-specific data */
# define              APP_DATA (ewsContextNetHandle(ewsContext)->app_data)
# define              APP_DATA_C(context) \
                             (ewsContextNetHandle(context)->app_data)
  /* Note that app_pointer/APP_POINTER is a place to store a pointer, but
   * memory pointed to by it WILL NOT be automatically freed at the end
   * of the request by the standard ewaNetHTTPCleanup. */
  void                *app_pointer;   /* application-specific pointer */
# define              APP_POINTER (ewsContextNetHandle(ewsContext)->app_pointer)
# define              APP_POINTER_C(context)  \
                          (ewsContextNetHandle(context)->app_pointer)
# define              APP_BUFFER_SIZE 512
  char                buffer[APP_BUFFER_SIZE];
# define              APP_BUFFER (ewsContextNetHandle(ewsContext)->buffer)
# define              APP_BUFFER_C(context)  \
                          (ewsContextNetHandle(context)->buffer)
  union
  {
    int32               decimalInt;
    uint32              decimalUint;
    uint32              hexInt;
    uint32              dottedIp;
    unsigned char       macAddr[6];
  } app_result;     /* for typed emweb_string return data */


  /*
   * Application buffer for temporary, per-request, strings generated
   * from EMWEB_STRING, EMWEB_INCLUDE, etc.
   */
# ifdef EW_CONFIG_OPTION_SNMP_AGENT
  #ifndef EMWEB_SNMP_OID_MAXLEN
  #define EMWEB_SNMP_OID_MAXLEN 256
  #endif
  char                appOIDStr[EMWEB_SNMP_OID_MAXLEN];    /* used by getnext example */
  int                 appOIDRootLen;
# endif /* EW_CONFIG_OPTION_SNMP_AGENT */
# ifdef EW_CONFIG_OPTION_FILE
  EwaFileHandle_t     file_handle; /* application-specific file handle */
  EwsFileParamsP      params;      /* tracks _ONE_ local file per request */
# endif

# ifdef EMWEB_POSIX_OPTION_SERVER_PUSH
  /*
   * Used by Server-Side-Push logic
   */
  SSPState            ssp_state;      /* server-side-push state */
  int                 ssp_delay;      /* seconds between updates */
  int                 ssp_timeout;    /* seconds remaining before resume */
  char                ssp_url[64];    /* URL of SSP reload */
# endif

  /* Begin LVL7 2478 */
#define LOCAL_NAME_MAX 48
  char          localName[LOCAL_NAME_MAX]; /*  ipv6 string */
  /* End LVL7 2478 */

# ifdef EMWEB_POSIX_TUTORIAL
  /*
   * Used by EmWeb Tutorial archive
   */
#              define PAGE_NAME (ewsContextNetHandle(ewsContext)->PageName)
#              define PAGE_NAME_SIZE 64
  char                PageName[ PAGE_NAME_SIZE ];
# endif

#ifdef EW_CONFIG_OPTION_TELNET
  boolean             logged_in;     /* TRUE after successful login */
#endif /* EW_CONFIG_OPTION_TELNET */

# if defined (EW_CONFIG_OPTION_SSL_RSA)
  BIO *bio;                         /* i/o structure */
  SSL *ssl;                         /* SSL context */
  SSLState ssl_state;               /* SSL state */
  boolean  ssl_connection_flg;      /* True if data coming over SSL port */
# endif /* EW_CONFIG_OPTION_SSL_RSA */

#ifdef EW_CONFIG_OPTION_UPNP
  boolean  upnp_udp_connection_flg;  /* True if data coming over udp port */
  struct sockaddr_in fromAddr;       /* the address from which the packet was received */
  int fromAddrLen;                   /* size of fromAddr field */
  char *udp_xmit_buffer;             /* pointer to dynamically allocated buffer for udp
                                       transmission of data */
  int  udp_xmit_buflen;             /* current size of udp_xmit_buffer */
  boolean isMulticastMsg;           /* TRUE if msg is to be sent via sendto() to the
                                       UPNP multicast address*/
  int bytes;                        /* actual bytes received in recvfrom() call. */
#endif /* EW_CONFIG_OPTION_UPNP */
};

#ifdef EW_CONFIG_OPTION_SSL_RSA
/*
#define EwaSecurityHandle (SSL *)
*/
#endif /* EW_CONFIG_OPTION_SSL_RSA */

# ifdef EW_CONFIG_OPTION_SMTP
struct EwaSMTPparams
{
  char url[ 64 ];
  char subject[ SMTP_SUBJECT_LEN ];
  char from[ SMTP_ADDR_LEN ];
  char addrs[ SMTP_MAX_RECIPIENTS ][ SMTP_ADDR_LEN ];
  char *to[ SMTP_MAX_RECIPIENTS + 1 ];
};
#endif


/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EWNET_H_ */

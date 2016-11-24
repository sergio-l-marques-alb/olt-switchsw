/*
 * Release: R6_2_0
 *
 * Includes for posix definitions
 *
 * This file is used in all POSIX-like, and Windows ports;
 * it assumes that one of the following symbols is defined on the
 * command line (or in config.mak):
 *
 *   HAVE_CONFIG_H  - provided for unix by autoconf
 *   EMWEB_CHORUS
 *   EMWEB_VXWORKS
 *   EMWEB_PSOS
 *   WIN32
 *
 * Also may use one or more of these to detect particular features:
 *
 *   _HPUX_SOURCE
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

#ifndef _POSIX_CONFIG_H_
#define _POSIX_CONFIG_H_

/**** POSIX port options ****/

/* 
** Add 32 to EMWEB_MAX_CONNECTIONS so that the value does not become too low on platforms where
** FD_WEB_DEFAULT_MAX_CONNECTIONS is small (SMB).  
*/
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
#define EMWEB_MAX_CONNECTIONS       (FD_CLI_DEFAULT_MAX_CONNECTIONS + \
                                     FD_WEB_DEFAULT_MAX_CONNECTIONS + \
                                     FD_CP_WEB_DEFAULT_MAX_CONNECTIONS + 32) /* maximum simultaneous connections */
#else
#define EMWEB_MAX_CONNECTIONS       (FD_CLI_DEFAULT_MAX_CONNECTIONS + \
                                     FD_WEB_DEFAULT_MAX_CONNECTIONS + 32)  /* maximum simultaneous connections */
#if defined(FEAT_METRO_CPE_V1_0)
#undef EMWEB_MAX_CONNECTIONS
#define EMWEB_MAX_CONNECTIONS       (FD_CLI_DEFAULT_MAX_CONNECTIONS + \
                                     FD_WEB_DEFAULT_MAX_CONNECTIONS + 10)  /* maximum simultaneous connections
                                                                              for merlion */
#endif
#endif

/* We need to keep the default multiplier in EMWEB_MAX_BUFFERS low, to save
 * memory for embedded systems.
 * The default of 2 is adequate for ordinary Web server use, but some
 * add-ons consume extra buffers because they keep a lot of data around before
 * processing it.
 * In particular the UPnP implementation should use a multiplier of 20 or more.
 * However, if we develop an incremental XML parser, this can be reduced.
 */
#define EMWEB_MAX_BUFFERS      (EMWEB_MAX_CONNECTIONS * 20)

#define EMWEB_BUFFER_DATA_SIZE 512 /* bytes */

#define EMWEB_MAX_IDLE 10   /* close TCP after MAX_IDLE inactivity */

#define HTTP_WELL_KNOWN_PORT   80 /* HTTP   Default Port - do not change! */
#define EMWEB_LISTEN_IPADDR INADDR_ANY
#define EMWEB_LISTEN_PORT   HTTP_WELL_KNOWN_PORT

/*
 * EMWEB_POSIX_OPTION_SERVER_PUSH  Include timer for server side push
 */
/* #define EMWEB_POSIX_OPTION_SERVER_PUSH */
#define APPLICATION_TIMER_INTERVAL 1 /* second */

/*
 * This controls how the server maps some requests to local files.
 * See the ewaURLHook function in ewsys.c
 */
#ifndef EMWEB_FILE_LOCAL_ROOT
#define EMWEB_FILE_LOCAL_ROOT   "" /* points to /usr/local/ptin/log/fastpath/ by default */
#endif
#define EMWEB_FILE_LOCAL_PREFIX "/filesystem/"

/* IMPORTANT --  Do not allow access to /usr/local/ptin/log/fastpath/.  This is where the SSL/SSH key files are stored, hence
it would create a major security problem.  Instead, create the directory called /usr/local/ptin/log/fastpath/public/ and place
web accessible files there. ***/
#define EMWEB_FILE_LOCAL_PUBLIC_PREFIX "/filesystem/public/"  /* avoid realm auth checks on each file */
#define EMWEB_FILE_LOCAL_RAM_NV_PREFIX "/filesystem/ramnv/"  /* avoid realm auth checks on each file */
#define EMWEB_FILE_LOCAL_RAM_CP_PREFIX "/filesystem/ramcp/"  /* avoid realm auth checks on each file */
#define EMWEB_FILE_LOCAL_INDEX  "/index.html"

/* Here, define a symbol to specify how to find the MAC address of the
 * network interface.  Possible choices are:
 *      WIN32                   MS-Windows.  This symbol will be defined
 *                              in the main configuration files, so it needn't
 *                              be defined here.
 *      POSIX_MAC_IFCONFIG      Run ifconfig using popen().  Inefficient but
 *                              reasonably portable.
 *      POSIX_MAC_IOCTL         Use the SIOCGIFHWADDR ioctl() call.
 * In the latter two cases, EMWEB_INTERFACE_NAME must be defined to give
 * the name of the interface, usually "eth0".  It is usually defined in
 * posix_config.h.
 */

#if !defined(WIN32) && !defined(_WIN32_WCE)
#define POSIX_MAC_IOCTL
#else
#undef POSIX_MAC_IOCTL
#endif

/*
 * The name of the primary network interface.
 * For Unix-like systems, this is used to look up the MAC address, which
 * is used to generate UPnP device unique IDs.  In the future, it will
 * probably be used to look up the IP address that the Server declares
 * for itself.
 * For Windows systems, this option is currently unused, but may later be
 * defined to be the identifier of the primary interface for systems
 * with multiple interfaces.
 */
#define EMWEB_INTERFACE_NAME "eth0"

/*
 * Define EMWEB_GETTIMEOFDAY if your system supports the gettimeofday() call.
 * If gettimeofday is available, it provides time with microsecond
 * resolution, and should be used in ewaTimerGetClock, which wants the
 * highest-precision clock value available.
 * If EMWEB_GETTIMEOFDAY is not defined, ewaTimerGetClock uses the Posix time()
 * call to get a clock value with second resolution.
 */
#if !defined(WIN32) && !defined(_WIN32_WCE)
#define EMWEB_GETTIMEOFDAY
#endif


/**** End of POSIX options ****/

/*#define EMWEB_USE_SIGNALS * may be undefed below */

#if defined(EMWEB_CHORUS)

#  define HAVE_ERRNO_H
#  define HAVE_SIGNAL_H
#  define HAVE_FCNTL_H
#  define HAVE_SYS_STAT_H
#  define HAVE_SYS_SOCKET_H
#  define HAVE_SYS_IOCTL_H
#  define HAVE_UNISTD_H
#  define HAVE_NETINET_IN_H
#  define HAVE_NETDB_H
#  define HAVE_STRING_H
#  define STDC_HEADERS

#  undef  EMWEB_USE_SIGNALS

#elif defined(EMWEB_VXWORKS)

#  define HAVE_ERRNO_H
#  define HAVE_SIGNAL_H
#  define HAVE_FCNTL_H
#  define HAVE_SYS_STAT_H
#  define HAVE_SYS_SOCKET_H
#  define HAVE_SYS_IOCTL_H
#  define HAVE_UNISTD_H
#  define HAVE_NETINET_IN_H
#  define HAVE_STRING_H
#  define STDC_HEADERS

#  undef  EMWEB_USE_SIGNALS

#  include <hostLib.h>
#  include <sockLib.h>
#  include <selectLib.h>
#  include <taskLib.h>
#  include <ioLib.h>
#  include <inetLib.h>

#elif defined(EMWEB_PSOS)

#  define HAVE_ERRNO_H
#  define STDC_HEADERS

#  undef  EMWEB_USE_SIGNALS

#  include <psos.h>
#  include <pna.h>
#  include <phile.h>
#  define _TIME_T_DEFINED 1
#elif ! defined(EMWEB_OSE) && ( defined(WIN32) || defined(_WIN32_WCE) )

#  undef  EMWEB_USE_SIGNALS
  /* Now add in definitions needed for WIN32 platforms.  include wincfg.h
 includes from old file config.winnt/config.h to build server and move here */
#include "wincfg.h"
#ifdef _WIN32_WCE
#  include <winsock.h>
#  undef HAVE_FCNTL_H
#  undef HAVE_ERRNO_H
#  undef HAVE_SYS_STAT_H
#endif
#  ifndef EWOULDBLOCK
#    define EWOULDBLOCK WSAEWOULDBLOCK
#  endif /* EWOULDBLOCK */
#  ifndef EINPROGRESS
#    define EINPROGRESS WSAEINPROGRESS
#  endif /* EINPROGRESS */
#  ifndef ENETUNREACH
#    define ENETUNREACH WSAENETUNREACH
#  endif /* ENETUNREACH */
#  define SOCKET_IS_INVALID(x) ((x) == INVALID_SOCKET)
#  define htons(v) htons((u_short)v)
    /* ADDED DEFINS - from notes in winsock.h, winsock2.h  */
#  define ETIMEDOUT WSAETIMEDOUT

#elif defined(EMWEB_OSE)

#  define HAVE_STRING_H
#  define HAVE_STRERROR
#  undef HAVE_ERRNO_H
#  undef HAVE_SIGNAL_H
#  undef HAVE_FCNTL_H
#  undef HAVE_SYS_STAT_H
#  undef HAVE_SOCKET_H
#  undef HAVE_SYS_IOCTL_H
#  undef HAVE_NETINET_IN_H
#  undef STDC_HEADERS
#  undef EMWEB_USE_SIGNALS

#  if (EMWEB_ENDIAN == EMWEB_ENDIAN_LITTLE && ! defined(LITTLE_ENDIAN))
#    define LITTLE_ENDIAN
#  elif ! defined(BIG_ENDIAN)
#    define BIG_ENDIAN
#  endif

#  include "ose.h"
#  include "ew_types.h"

   /* including socket.h specifically avoids this declaration */
   int *inet_errno(void);

#  define SOCKET    int

#elif    defined(HAVE_CONFIG_H) /* Unix - autoconf configuration results */

/*
The following include needs the path because there are many files
called config.h in the possible include paths
*/
#  include "../config.linux/emweb_config.h"

#else
#error "One of (EMWEB_VXWORKS|EMWEB_PSOS|WIN32|HAVE_CONFIG_H) must be defined"
#endif

#if ! defined(WIN32) || defined(EMWEB_OSE)
/*
 * Workarounds for winsock names for socket routines
 */
#ifndef EW_CONFIG_OPTION_SSL_RSA
#  define closesocket( x ) (close( (x) ))
#  define ioctlsocket( x, y, z ) (ioctl( (x), (y), (z) ))
#endif /* EW_CONFIG_OPTION_SSL_RSA */

#  define SOCKET_IS_INVALID(x) ((x) < 0)
#else
#  define ioctlsocket( x, y, z ) (ioctlsocket( (x), (y), (u_long FAR *)(z) ))
#endif

#ifdef STDC_HEADERS
#  include <stdio.h>
#  include <stdlib.h>
#endif

#ifdef HAVE_NETDB_H
#  include <netdb.h>
#endif

#if defined(HAVE_SIGNAL_H) && defined(EMWEB_USE_SIGNALS)
#  include <signal.h>
#endif

#ifdef HAVE_FCNTL_H
#  include <fcntl.h>
#endif

#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#elif   defined(EMWEB_CHORUS) || \
    defined(EMWEB_VXWORKS) || \
    defined(EMWEB_PSOS) || \
    defined(EMWEB_OSE)

/*
 * softOSE in time.h causes time() and gmtime() to be redefined to zztime()
 * and zzgmtime() which don't exist.  Prevent that from occuring while
 * including time.h
 */
#  ifdef softOSE
#    define LOCAL_SOFTOSE   softOSE
#  endif
#  undef softOSE
#  include <time.h>
#  ifdef LOCAL_SOFTOSE
#    define softOSE     LOCAL_SOFTOSE
#  endif
#  undef LOCAL_SOFTOSE


#  ifdef EMWEB_OSE
     /* errno got defined by stddef.h included by time.h */
#    undef errno

   /* This defines errno (thus the need to have it undefined */
#    include "inet.h"

/*
 * OSE uses send() for signals; they use inet_send() for socket data
 * transmission.  Also, OSE "sometimes" (depending on what include file is
 * include before what other include file) defines the socket calls to zz*()
 * rather than to inet_*().  We'll just do the macro definitions here to
 * ensure that we get what we expect. */
#    undef socket
#    undef bind
#    undef connect
#    undef listen
#    undef accept
#    undef shutdown
#    undef close
#    undef select
#    undef setsockopt
#    undef getsockopt
#    undef getsockname
#    undef getpeername
#    undef ioctl
#    undef fcntl
#    undef inet_ntoa
#    undef inet_addr
#    undef write
#    undef send
#    undef sendto
#    undef read
#    undef recv
#    undef recvfrom

#    define socket(a,b,c)         inet_socket(a,b,c)
#    define bind(a,b,c)           inet_bind(a,b,c)
#    define connect(a,b,c)        inet_connect(a,b,c)
#    define listen(a,b)           inet_listen(a,b)
#    define accept(a,b,c)         inet_accept(a,b,c)
#    define shutdown(a,b)         inet_shutdown(a,b)
#    define close(a)              inet_close(a)
#    define select(a,b,c,d,e)     inet_select(a,b,c,d,e)
#    define setsockopt(a,b,c,d,e) inet_setsockopt(a,b,c,d,e)
#    define getsockopt(a,b,c,d,e) inet_getsockopt(a,b,c,d,e)
#    define getsockname(a,b,c)    inet_getsockname(a,b,c)
#    define getpeername(a,b,c)    inet_getpeername(a,b,c)
#    define ioctl(a,b,c)          inet_ioctl(a,b,(char *)(c))
#    define fcntl(a,b,c)          inet_fcntl(a,b,c)
#    define inet_ntoa(a)          inet_inet_ntoa(a)
#    define inet_addr(a)          inet_inet_addr(a)
#    define write(a,b,c)          inet_write(a,b,c)
#    define send(a,b,c,d)     inet_send(a,b,c,d)
#    define sendto(a,b,c,d,e,f)   inet_sendto(a,b,c,d,e,f)
#    define read(a,b,c)           inet_read(a,b,c)
#    define recv(a,b,c,d)         inet_recv(a,b,c,d)
#    define recvfrom(a,b,c,d,e,f) inet_recvfrom(a,b,c,d,e,f)
#  endif /* EMWEB_OSE */

#endif

#ifdef HAVE_ERRNO_H
#  include <errno.h>
#endif

#if (defined(WIN32) || defined(_WIN32_WCE)) && ! defined(EMWEB_OSE)
#ifndef errno
#  define errno (WSAGetLastError())
#  define ClearErrno() (WSASetLastError(0))
#else
#  define ClearErrno() (errno = 0)
#endif
#else
#  define ClearErrno() (errno = 0)
#endif

#ifdef HAVE_SOCKET_H
#  include <socket.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#  include <sys/socket.h>

#  ifndef WIN32
#ifndef SOCKET
    typedef int SOCKET;   /* Windows defines SOCKET type */
#endif
#  endif /* __EMWEB_WIN32__ */

#endif

#ifndef L7_ORIGINAL_VENDOR_CODE
#include "l7_socket.h"
#ifndef SOCKET
    typedef int SOCKET;   /* Windows defines SOCKET type */
#endif
#endif

#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>
#  ifdef WIN32
#    define STATBUF struct _stat
#    define STAT(file,result) _stat(file,result)
#  else
#    define STATBUF struct stat
#    define STAT(file,result) stat(file,result)
#  endif
#endif

#ifdef HAVE_SYS_IOCTL_H
#  ifdef IOCTL_FIONBIO_NEEDS_BSD_COMP
#    define BSD_COMP
#  endif
#  include <sys/ioctl.h>
#else
#  undef HAVE_IOCTL_FIONBIO
#endif

#ifdef HAVE_NETINET_IN_H
#  include <netinet/in.h>
#endif

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif

#ifdef HAVE_STRING_H
#  include <string.h>
#endif

#ifndef HAVE_STRERROR
extern
char *strerror( const int errnum );
#endif

#ifdef _HPUX_SOURCE             /* HP's select needs cast */
#  define FDSET_CAST( x ) ((int *)(x))
#else
#  define FDSET_CAST( x ) (x)
#endif

#ifndef EW_SIZE_T
#define EW_SIZE_T unsigned
#endif /* EW_SIZE_T */

#ifdef FD_SETSIZE
#  define MAX_FD  FD_SETSIZE           /* maximum simultaneous requests */
#else
#  define MAX_FD  256                  /* (may be limited further by O/S) */
#endif

/*
 * Any unexpected error here will call this routine, so it is
 * a convenient place for a debugger breakpoint.
 */
#define ERROR_EXIT    fatal_trap( __FILE__, __LINE__ )
extern void fatal_trap( const char * file, int line );


/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _POSIX_CONFIG_H_ */

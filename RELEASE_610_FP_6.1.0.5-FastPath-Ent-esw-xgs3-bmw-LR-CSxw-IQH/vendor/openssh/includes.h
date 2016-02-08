/*  $OpenBSD: includes.h,v 1.22 2006/01/01 08:59:27 stevesk Exp $   */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 * This file includes most of the needed system headers.
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef INCLUDES_H
#define INCLUDES_H

#define RCSID(msg) \
static /**/const char *const rcsid[] = { (const char *)rcsid, "\100(#)" msg }

#include "config.h"

#ifndef L7_SSHD
#define _GNU_SOURCE /* activate extra prototypes for glibc */
#endif /* L7_SSHD */

#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h> /* For O_NONBLOCK */
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifndef L7_SSHD
#include <pwd.h>
#include <grp.h>
#endif /* L7_SSHD */
#include <time.h>
#include <dirent.h>
#include <stddef.h>

#ifdef HAVE_LIMITS_H
# include <limits.h> /* For PATH_MAX */
#endif
#ifdef HAVE_GETOPT_H
#ifndef L7_SSHD
# include <getopt.h>
#endif /* L7_SSHD */
#endif
#ifdef HAVE_BSTRING_H
# include <bstring.h>
#endif
#if defined(HAVE_GLOB_H) && defined(GLOB_HAS_ALTDIRFUNC) && \
    defined(GLOB_HAS_GL_MATCHC)
# include <glob.h>
#endif
#ifdef HAVE_NETGROUP_H
# include <netgroup.h>
#endif
#ifdef HAVE_ENDIAN_H
# ifdef _L7_OS_LINUX_
# include <endian.h>
# endif /* _L7_OS_LINUX_ */
#endif
#ifdef HAVE_TTYENT_H
#ifndef L7_SSHD
# include <ttyent.h>
#endif /* L7_SSHD */
#endif
#ifdef HAVE_UTIME_H
# include <utime.h>
#endif
#ifdef HAVE_MAILLOCK_H
# include <maillock.h> /* For _PATH_MAILDIR */
#endif
#ifdef HAVE_NEXT
#  include <libc.h>
#endif
#include <unistd.h> /* For STDIN_FILENO, etc */
#ifndef L7_SSHD
#include <termios.h> /* Struct winsize */
#endif /* L7_SSHD */

/*
 *-*-nto-qnx needs these headers for strcasecmp and LASTLOG_FILE respectively
 */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_LOGIN_H
# include <login.h>
#endif

#ifdef HAVE_UTMP_H
#  include <utmp.h>
#endif
#ifdef HAVE_UTMPX_H
#  ifdef HAVE_TV_IN_UTMPX
#    include <sys/time.h>
#  endif
#  include <utmpx.h>
#endif
#ifdef HAVE_LASTLOG_H
#  include <lastlog.h>
#endif
#ifdef HAVE_PATHS_H
#  include <paths.h> /* For _PATH_XXX */
#endif

#include <sys/types.h>
#ifndef L7_SSHD
#include <sys/socket.h>
#include <sys/ioctl.h>
#else
#include "l7_socket.h"
#endif /* L7_SSHD */
#include <sys/wait.h>
#ifdef HAVE_SYS_TIME_H
# ifdef _L7_OS_LINUX_
# include <sys/time.h> /* For timersub */
# else
# include <sys/times.h>
# endif /* _L7_OS_LINUX_ */
#endif
# ifdef _L7_OS_LINUX_
#include <sys/resource.h>
# endif /* _L7_OS_LINUX_ */
#ifdef HAVE_SYS_SELECT_H
# ifdef _L7_OS_LINUX_
# include <sys/select.h>
# else
# include <selectLib.h>
# endif /* _L7_OS_LINUX_ */
#endif
#ifdef HAVE_SYS_BSDTTY_H
# include <sys/bsdtty.h>
#endif
# ifdef _L7_OS_LINUX_
#include <sys/param.h> /* For MAXPATHLEN and roundup() */
# endif /* _L7_OS_LINUX_ */
#ifdef HAVE_SYS_UN_H
# include <sys/un.h> /* For sockaddr_un */
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_SYS_BITYPES_H
# include <sys/bitypes.h> /* For u_intXX_t */
#endif
#ifdef HAVE_SYS_CDEFS_H
# include <sys/cdefs.h> /* For __P() */
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h> /* For S_* constants and macros */
#endif
#ifdef HAVE_SYS_SYSMACROS_H
# include <sys/sysmacros.h> /* For MIN, MAX, etc */
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h> /* for MAP_ANONYMOUS */
#endif
#ifdef HAVE_SYS_STRTIO_H
#include <sys/strtio.h> /* for TIOCCBRK on HP-UX */
#endif
#if defined(HAVE_SYS_PTMS_H) && defined(HAVE_DEV_PTMX)
# if defined(HAVE_SYS_STREAM_H)
#  include <sys/stream.h>   /* reqd for queue_t on Solaris 2.5.1 */
# endif
#include <sys/ptms.h>   /* for grantpt() and friends */
#endif

#include <netinet/in_systm.h> /* For typedefs */
#ifndef L7_SSHD
#include <netinet/in.h> /* For IPv6 macros */
#endif /* L7_SSHD */

/* #ifndef L7_SSHD */
/* With this change I get this error:
../../../../vendor/openssh/openbsd-compat/port-tun.c: In function `sys_tun_infilter':
../../../../vendor/openssh/openbsd-compat/port-tun.c:227: error: dereferencing pointer to incomplete type
need struct ip definition.
*/
#include <netinet/ip.h> /* For IPTOS macros */
/* #endif */

#ifndef L7_SSHD
#include <netinet/tcp.h>
#include <arpa/inet.h>
#endif /* L7_SSHD */
#if defined(HAVE_NETDB_H)
# include <netdb.h>
#endif
#ifdef HAVE_RPC_TYPES_H
# include <rpc/types.h> /* For INADDR_LOOPBACK */
#endif
#ifdef USE_PAM
#if defined(HAVE_SECURITY_PAM_APPL_H)
# include <security/pam_appl.h>
#elif defined (HAVE_PAM_PAM_APPL_H)
# include <pam/pam_appl.h>
#endif
#endif
#ifdef HAVE_READPASSPHRASE_H
# include <readpassphrase.h>
#endif

#ifdef HAVE_IA_H
# include <ia.h>
#endif

#ifdef HAVE_IAF_H
# include <iaf.h>
#endif

#ifdef HAVE_TMPDIR_H
# include <tmpdir.h>
#endif

#ifdef HAVE_LIBUTIL_H
# include <libutil.h> /* Openpty on FreeBSD at least */
#endif

#if defined(KRB5) && defined(USE_AFS)
# include <krb5.h>
# include <kafs.h>
#endif

#if defined(HAVE_SYS_SYSLOG_H)
# include <sys/syslog.h>
#endif

/*
 * On HP-UX 11.11, shadow.h and prot.h provide conflicting declarations
 * of getspnam when _INCLUDE__STDC__ is defined, so we unset it here.
 */
#ifdef GETSPNAM_CONFLICTING_DEFS
# ifdef _INCLUDE__STDC__
#  undef _INCLUDE__STDC__
# endif
#endif

#include <openssl/opensslv.h> /* For OPENSSL_VERSION_NUMBER */

#include "defines.h"

#include "version.h"
#include "openbsd-compat/openbsd-compat.h"
#include "openbsd-compat/bsd-nextstep.h"

#include "entropy.h"

#ifdef L7_SSHD
#include "l7defines.h"
#include "osapi.h"
#include "osapi_support.h"

#ifndef IPPORT_RESERVED
#define IPPORT_RESERVED 1024
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN  64
#endif
#ifdef _L7_OS_VXWORKS_
    #define LITTLE_ENDIAN   1234    /* LSB first: i386, vax */
    #define BIG_ENDIAN  4321    /* MSB first: 68000, ibm, net */
    #define PDP_ENDIAN  3412    /* LSB first in word, MSW first in long */
    #define BYTE_ORDER  BIG_ENDIAN
#endif
#ifndef LLONG_MAX
#define LLONG_MAX   9223372036854775807LL
#endif
#ifndef LLONG_MIN
#   define LLONG_MIN    (-LLONG_MAX - 1LL)
#endif

#endif /* L7_SSHD */

#endif /* INCLUDES_H */

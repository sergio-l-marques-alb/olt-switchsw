/*
 *
 * Copyright (C) 1996-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef SR_LINUX_H
#define SR_LINUX_H

#ifdef  __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* MANUALLY ADDED */
/*
 * see /usr/include/features.h for the ramifications of setting this
 * ifndef checks avoid redefinition error messages.
 */
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif /* _BSD_SOURCE */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif /* _POSIX_SOURCE */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
/* #undef _ALL_SOURCE */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to the type of elements in the array set by `getgroups'.
   Usually this is either `int' or `gid_t'.  */
#define GETGROUPS_T gid_t

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have atoi()  */
#define HAVE_ATOI 1

/* Define if you have memmove()  */
#define HAVE_MEMMOVE 1

/* Define if you have close()  */
#define HAVE_CLOSE 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if you have the gethostbyname function.  */
#define HAVE_GETHOSTBYNAME 1

/* Define if you have the gethostname function.  */
#define HAVE_GETHOSTNAME 1

/* Define if you have the getmntent function.  */
#define HAVE_GETMNTENT 1

/* Define if you have the getpagesize function */
#define HAVE_GETPAGESIZE 1

/* Define if you have getpeername() */
#define HAVE_GETPEERNAME 1

/* Define if you have the getservbyname function */
#define HAVE_GETSERVBYNAME 1

/* Define if you have the inet_addr function */
#define HAVE_INET_ADDR 1

/* Define if you have the inet_ntoa function */
#define HAVE_INET_NTOA 1

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define if you have a the rand().  */
#define HAVE_RAND 1

/* Define if you have a the random().  */
#define HAVE_RANDOM 1

/* Define if you have a the `recvfrom' system call.  */
#define HAVE_RECVFROM 1

/* Define if you have a the `rename' system call.  */
#define HAVE_RENAME 1

/* Define if you have a the `sendto' system call.  */
#define HAVE_SENDTO 1

/* Define if you have a the srand().  */
#define HAVE_SRAND 1

/* Define if you have a the srandom().  */
#define HAVE_SRANDOM 1

/* Define if your struct stat has st_blksize.  */
#define HAVE_ST_BLKSIZE 1

/* Define if your struct stat has st_blocks.  */
#define HAVE_ST_BLOCKS 1

/* Define if your struct stat has st_rdev.  */
#define HAVE_ST_RDEV 1

/* Define if you have sys_errlist defined in a header file.  */
/* Note that you only have this 'cause _BSD_SOURCE is defined */
#define HAVE_SYS_ERRLIST 1

/* Define if you have sys_nerr defined in a header file.  */
/* Note that you only have this 'cause _BSD_SOURCE is defined */
#define HAVE_SYS_NERR 1

/* Define if you have <sys/wait.h> that is POSIX.1 compatible.  */
#define HAVE_SYS_WAIT_H 1

/* Define if your struct tm has tm_gmtoff.  */
/* #undef HAVE_TM_GMTOFF */

/* Define if your struct tm has tm_zone.  */
/* #undef HAVE_TM_ZONE */

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
#define HAVE_TZNAME 1

/* Define if you don't have tm_gmtoff but do have the external long
   altzone.  */
/* #undef HAVE_ALTZONE */

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
/* #undef HAVE_UTIME_NULL */

/* Define if you have <vfork.h>.  */
/* #undef HAVE_VFORK_H */

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define if you have the vsprintf function.  */
#define HAVE_VSPRINTF 1

/* Define if you have the wait3 system call.  */
#define HAVE_WAIT3 1

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if the `S_IS*' macros in <sys/stat.h> do not work properly.  */
/* #undef STAT_MACROS_BROKEN */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define vfork as fork if vfork does not work.  */
/* #undef vfork */

#if (! ( defined(__x86_64__) ) && ! ( defined(__ia64__) ))
/* Settings for 32-bit Systems */

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a char *.  */
#define SIZEOF_CHAR_P 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a int *.  */
#define SIZEOF_INT_P 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* Define this is a long is 8 octets  */
/* #undef HAVE_EIGHT_BYTE_LONGS */

#else	/* (! ( defined(__x86_64__) ) && ! ( defined(__ia64__) )) */
/* Settings for 64-bit Systems */

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a char *.  */
#define SIZEOF_CHAR_P 8

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a int *.  */
#define SIZEOF_INT_P 8

/* The number of bytes in a long.  */
#define SIZEOF_LONG 8

/* Define this is a long is 8 octets  */
#define HAVE_EIGHT_BYTE_LONGS

#endif	/* (! ( defined(__x86_64__) ) && ! ( defined(__ia64__) )) */

/* Define if you have the alarm function.  */
#define HAVE_ALARM 1

/* Define if you have the ftime function.  */
#define HAVE_FTIME 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getenv function.  */
#define HAVE_GETENV 1

/* Define if you have the geteuid function.  */
/* #undef HAVE_GETEUID */

/* Define if you have the getpid function.  */
/* #undef HAVE_GETPID */

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the localtime function.  */
#define HAVE_LOCALTIME 1

/* Define if you have the mkdir function.  */
#define HAVE_MKDIR 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the sc_delay function.  */
/* #undef HAVE_SC_DELAY */

/* Define if you have the select function.  */
#define HAVE_SELECT 1

/* Define if you have the setsid function.  */
#define HAVE_SETSID 1

/* Define if you have the sigaddset function.  */
#define HAVE_SIGADDSET 1

/* Define if you have the sigdelset function.  */
#define HAVE_SIGDELSET 1

/* Define if you have the sigemptyset function.  */
#define HAVE_SIGEMPTYSET 1

/* Define if you have the siginterrupt function.  */
#define HAVE_SIGINTERRUPT 1

/* Define if you have the sigismember function.  */
#define HAVE_SIGISMEMBER 1

/* Define if you have the signal function.  */
#define HAVE_SIGNAL 1

/* Define if you have the sigprocmask function.  */
#define HAVE_SIGPROCMASK 1

/* Define if you have the sleep function.  */
#define HAVE_SLEEP 1

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strcspn function.  */
#define HAVE_STRCSPN 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strftime function.  */
#define HAVE_STRFTIME 1

/* Define if you have the strspn function.  */
#define HAVE_STRSPN 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtok_r function.  */
#define HAVE_STRTOK_R 1

/* Define if you have the strtol function.  */
#define HAVE_STRTOL 1

/* Define if you have the sysconf function.  */
#define HAVE_SYSCONF 1

/* Define if you have the snprintf function */
#define HAVE_SNPRINTF 1

/* Define if you have the vsnprintf function */
#define HAVE_VSNPRINTF 1

/* Define if you have the taskDelay function.  */
/* #undef HAVE_TASKDELAY */

/* Define if you have the tm_wkafter function.  */
/* #undef HAVE_TM_WKAFTER */

/* Define if you have the uname function.  */
#define HAVE_UNAME 1

/* Define if you have the valloc function.  */
#define HAVE_VALLOC 1

/* Define if you have the <a.out.h> header file.  */
#define HAVE_A_OUT_H 1

/* Define if you have the <arpa/inet.h> header file.  */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <bsd.h> header file.  */
/* #undef HAVE_BSD_H */

/* Define if you have the <common.h> header file.  */
/* #undef HAVE_COMMON_H */

/* Define if you have the <ctype.h> header file.  */
#define HAVE_CTYPE_H 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <dl.h> header file.  */
/* #undef HAVE_DL_H */

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <hostLib.h> header file.  */
/* #undef HAVE_HOSTLIB_H */

/* Define if you have the <inetLib.h> header file.  */
/* #undef HAVE_INETLIB_H */

/* Define if you have the <io.h> header file.  */
/* #undef HAVE_IO_H */

/* Define if you have the <ioLib.h> header file.  */
/* #undef HAVE_IOLIB_H */

/* Define if you have the <kvm.h> header file.  */
/* #undef HAVE_KVM_H */

/* Define if you have the <kstat.h> header file.  */
/* #undef HAVE_KSTAT_H */

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file.  */
/* #define HAVE_MEMORY_H 1 */

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <pcap.h> header file .  */
#define HAVE_PCAP_H 1

/* Define if you have the <net/inet.h> header file.  */
/* #undef HAVE_NET_INET_H */

/* Define if you have the <net/route.h> header file */
#define HAVE_NET_ROUTE_H 1

/* Define if you have the <net/if.h> header file */
#define HAVE_NET_IF_H 1

/* Define if you have the <net/if_arp.h> header file */
#define HAVE_NET_IF_ARP_H 1

/* Define if you have the <netdb.h> header file.  */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file.  */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <netinet/in_systm.h> header file.  */
#define HAVE_NETINET_IN_SYSTM_H 1

/* Define if you have the <netinet/ip.h> header file.  */
#define HAVE_NETINET_IP_H 1

/* Define if you have the <netinet/ip_icmp.h> header file.  */
#define HAVE_NETINET_IP_ICMP_H 1

/* Define if you have the <netinet/tcp.h> header file.  */
#define HAVE_NETINET_TCP_H 1

/* Define if you have the <netns/idp.h> header file.  */
/* #undef HAVE_NETNS_IDP_H */

/* Define if you have the <netns/ns.h> header file.  */
/* #undef HAVE_NETNS_NS_H */

/* Define if you have the <nlist.h> header file.  */
/* #undef HAVE_NLIST_H */

/* Define if you have the <os2.h> header file.  */
/* #undef HAVE_OS2_H */

/* Define if you have the <paths.h> header file.  */
#define HAVE_PATHS_H 1

/* Define if you have the <pctcp.h> header file.  */
/* #undef HAVE_PCTCP_H */

/* Define if you have the <psos.h> header file.  */
/* #undef HAVE_PSOS_H */

/* Define if you have the <setjmp.h> header file.  */
#define HAVE_SETJMP_H 1

/* Define if you have the <sgtty.h> header file.  */
#define HAVE_SGTTY_H 1

/* Define if you have the <signal.h> header file.  */
#define HAVE_SIGNAL_H 1

/* Define if you have the <socket.h> header file.  */
/* #undef HAVE_SOCKET_H */

/* Define if you have the <stab.h> header file.  */
#define HAVE_STAB_H 1

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdio.h> header file */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <synch.h> header file.  */
/* #undef HAVE_SYNCH_H */

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/fcntl.h> header file.  */
#define HAVE_SYS_FCNTL_H 1

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/filio.h> header file.  */
/* #undef HAVE_SYS_FILIO_H */

/* Define if you have the <sys/fstyp.h> header file.  */
/* #undef HAVE_SYS_FSTYP_H */

/* Define if you have the <sys/fsid.h> header file.  */
/* #undef HAVE_SYS_FSID_H */

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/select.h> header file.  */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/statvfs.h> header file.  */
/* #undef HAVE_SYS_STATVFS_H */

/* Define if you have the <sys/mnttab.h> header file.  */
/* #undef HAVE_SYS_MNTTAB_H */

/* Define if you have the <sys/proc.h> header file.  */
/* #undef HAVE_SYS_PROC_H */

/* Define if you have the <sys/processor.h> header file.  */
/* #undef HAVE_SYS_PROCESSOR_H */

/* Define if you have the <sys/procfs.h> header file.  */
#define HAVE_SYS_PROCFS_H 1

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/swap.h> header file.  */
#define HAVE_SYS_SWAP_H 1

/* Define if you have the <sys/sysinfo.h> header file.  */
#define HAVE_SYS_SYSINFO_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/timeb.h> header file.  */
#define HAVE_SYS_TIMEB_H 1

/* Define if you have the <sys/timers.h> header file.  */
/* #undef HAVE_SYS_TIMERS_H */

/* Define if you have the <sys/times.h> header file.  */
#define HAVE_SYS_TIMES_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/uio.h> header file.  */
#define HAVE_SYS_UIO_H 1

/* Define if you have the <sys/un.h> header file.  */
#define HAVE_SYS_UN_H 1

/* Define if you have the <sys/utsname.h> header file.  */
#define HAVE_SYS_UTSNAME_H 1

/* Define if you have the <sys/var.h> header file.  */
/* #undef HAVE_SYS_VAR_H */

/* Define if you have the <sys/vfs.h> header file.  */
#define HAVE_SYS_VFS_H 1

/* Define if you have the <taskLib.h> header file.  */
/* #undef HAVE_TASKLIB_H */

/* Define if you have the <termio.h> header file.  */
#define HAVE_TERMIO_H 1

/* Define if you have the <thread.h> header file.  */
/* #undef HAVE_THREAD_H */

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <timers.h> header file.  */
/* #undef HAVE_TIMERS_H */

/* Define if you have the <types.h> header file.  */
/* #undef HAVE_TYPES_H */

/* Define if you have the <types/vxTypes.h> header file.  */
/* #undef HAVE_TYPES_VXTYPES_H */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <utils.h> header file.  */
/* #undef HAVE_UTILS_H */

/* Define if you have the <utmp.h> header file.  */
#define HAVE_UTMP_H 1

/* Define if you have the <utmpx.h> header file.  */
/* #undef HAVE_UTMPX_H */

/* Define if you have the <varargs.h> header file.  */
#define HAVE_VARARGS_H 1

/* Define if you have the <vm/page.h> header file.  */
/* #undef HAVE_VM_PAGE_H */

/* Define if you have the <vrtxil.h> header file.  */
/* #undef HAVE_VRTXIL_H */


/* Define if you have the <linux/version.h> header file */
#define HAVE_LINUX_VERSION_H 1

/*
 * Define if you want to call SetUpEnviron()
 * This will be true on most UNIX-like systems.
 */
#if (defined(__x86_64__) || defined(__ia64__))
#define WANT_SETUPENVIRON 1
#endif	/* (defined(__x86_64__) || defined(__ia64__)) */

/* Don't define this if you are not on a UNIX-like system */

/*
 * Don't define this if you are not on system with BSD-like
 * networking
 */
#ifndef SR_BSD
#define SR_BSD
#endif /* SR_BSD */

/*
 * Define this if you are on System V UNIX machine
 */
#ifndef SYSV
/* define SYSV */
#endif	/* SYSV */

/*
 * Define this if you are on a System V R 4 machine
 */
/* #define VR4 */

/*
 * Use fcntl with F_SETFL, O_NDELAY to set a file descriptor for
 * non-blocking io.  The alternative - to use ioctl instead - is the
 * default, so set this only to override the default.
 */
/* #define SR_USE_FCNTL 1 */

/*
 * Use ioctlsocket with FIONBIO to set a file descriptor for
 * non-blocking io.  This is only used on winsock-based systems.
 */
/* #define SR_USE_IOCTLSOCKET 1 */

/*
 * Define this if the OS supports a "local" Subagent mechanism as
 * Windows/VxWorks/pSOS/etc. do.  This is NOT the same as "shared
 * library" subagents supported on most UNIX systems.
 */
/* #undef SR_SUPPORT_LOCAL_SUBS */

/*
 * Define this if the OS supports a shared library subagents using
 * dlopen and family.  MAKE SURE YOU SET SR_DLOPEN_FLAGS either to
 * 1 or 2 if you define SR_SUPPORT_SHLIB_SUBS.
 */
#define SR_SUPPORT_SHLIB_SUBS 1

/*
 * Set one of these depending on whether the flags sent to dlopen are
 * RTLD_LAZY or RTLD_NOW.  Only necessary if SR_SUPPORT_SHLIB_SUBS is
 * defined.
 */
#define SR_USE_RTLD_LAZY 1
/* #undef SR_USE_RTLD_NOW */

/*
 * Set this if you need to have Master Agent symbols (such as do_trap())
 * put into the shlib library in order to avoid conflict with what's
 * in the EMANATE libraries.  This will only affect those platforms
 * which define SR_SUPPORT_SHLIB_SUBS.
 */
/* #undef SR_NEED_MASTER_SYMS */

/*
 * Some operating systems croak if you pass into select a timeout
 * parameter (struct) with values greater than expected.  Set those
 * values here.
 */
#define SR_SELECT_MAX_TIMEOUT_SEC 1000000
#define SR_SELECT_MAX_TIMEOUT_USEC 1000000

/*
 * Hackery to allow code to build on older systems (w/out IPv6)
 */

#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_LINUX_H */

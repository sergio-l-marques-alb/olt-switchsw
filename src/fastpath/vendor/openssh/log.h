/*  $OpenBSD: log.h,v 1.11 2004/06/21 22:02:58 djm Exp $    */

/*
 * Author: Tatu Ylonen <ylo@cs.hut.fi>
 * Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
 *                    All rights reserved
 *
 * As far as I am concerned, the code I have written for this software
 * can be used freely for any purpose.  Any derived versions of this
 * software must be clearly marked as such, and if the derived work is
 * incompatible with the protocol description in the RFC file, it must be
 * called by a name other than "ssh" or "Secure Shell".
 */

#ifndef SSH_LOG_H
#define SSH_LOG_H

#ifdef LOG_AUTHPRIV
#include <syslog.h> /* Needed for LOG_AUTHPRIV (if present) */
#endif

/* Supported syslog facilities and levels. */
typedef enum {
    SYSLOG_FACILITY_DAEMON,
    SYSLOG_FACILITY_USER,
    SYSLOG_FACILITY_AUTH,
#ifdef LOG_AUTHPRIV
    SYSLOG_FACILITY_AUTHPRIV,
#endif
    SYSLOG_FACILITY_LOCAL0,
    SYSLOG_FACILITY_LOCAL1,
    SYSLOG_FACILITY_LOCAL2,
    SYSLOG_FACILITY_LOCAL3,
    SYSLOG_FACILITY_LOCAL4,
    SYSLOG_FACILITY_LOCAL5,
    SYSLOG_FACILITY_LOCAL6,
    SYSLOG_FACILITY_LOCAL7,
    SYSLOG_FACILITY_NOT_SET = -1
}       SyslogFacility;

#ifndef L7_SSHD
typedef enum {
    SYSLOG_LEVEL_QUIET,
    SYSLOG_LEVEL_FATAL,
    SYSLOG_LEVEL_ERROR,
    SYSLOG_LEVEL_INFO,
    SYSLOG_LEVEL_VERBOSE,
    SYSLOG_LEVEL_DEBUG1,
    SYSLOG_LEVEL_DEBUG2,
    SYSLOG_LEVEL_DEBUG3,
    SYSLOG_LEVEL_NOT_SET = -1
}       LogLevel;
#else
typedef enum {
    SYSLOG_LEVEL_QUIET   = 0x00000000,
    SYSLOG_LEVEL_FATAL   = 0x00000001,
    SYSLOG_LEVEL_ERROR   = 0x00000002,
    SYSLOG_LEVEL_INFO    = 0x00000004,
    SYSLOG_LEVEL_VERBOSE = 0x00000008,
    SYSLOG_LEVEL_DEBUG1  = 0x00000010,
    SYSLOG_LEVEL_DEBUG2  = 0x00000020,
    SYSLOG_LEVEL_DEBUG3  = 0x00000040,
    SYSLOG_LEVEL_OPEN    = 0x00000080,
    SYSLOG_LEVEL_TIMER   = 0x00000100,
    SYSLOG_LEVEL_MALLOC  = 0x00000200,
    SYSLOG_LEVEL_PACKET  = 0x00000400,
    SYSLOG_LEVEL_SIGNAL  = 0x00000800,
    SYSLOG_LEVEL_READ    = 0x00001000,
    SYSLOG_LEVEL_WRITE   = 0x00002000,
    SYSLOG_LEVEL_CRYPTO  = 0x00004000,
    SYSLOG_LEVEL_ALL     = 0x7FFFFFFF,
    SYSLOG_LEVEL_NOT_SET = -1
}       LogLevel;
#endif /* L7_SSHD */

void     log_init(char *, LogLevel, SyslogFacility, int);

SyslogFacility  log_facility_number(char *);
LogLevel log_level_number(char *);

#ifndef L7_SSHD
void     fatal(const char *, ...) __dead __attribute__((format(printf, 1, 2)));
#else
void     fatal(const char *, ...) __attribute__((format(printf, 1, 2)));
void     fatal_cn(int, const char *, ...) __attribute__((format(printf, 2, 3)));
#endif /* L7_SSHD */
void     error(const char *, ...) __attribute__((format(printf, 1, 2)));
void     logit(const char *, ...) __attribute__((format(printf, 1, 2)));
#ifndef L7_SSHD
void     verbose(const char *, ...) __attribute__((format(printf, 1, 2)));
#else
#define  verbose ssh_verbose
void     ssh_verbose(const char *, ...) __attribute__((format(printf, 1, 2)));
void     debugl7(LogLevel level, const char *fmt,...);
void     sshdDebugLevel(LogLevel level);
int      sshdLogLevelCheck(LogLevel level);
#endif /* L7_SSHD */
void     debug(const char *, ...) __attribute__((format(printf, 1, 2)));
void     debug2(const char *, ...) __attribute__((format(printf, 1, 2)));
void     debug3(const char *, ...) __attribute__((format(printf, 1, 2)));

void     do_log(LogLevel, const char *, va_list);
#ifndef L7_SSHD
void     cleanup_exit(int) __dead;
#else
void     cleanup_exit(int);
void     cleanup_exit_cn(int, int);
void     sshd_cleanup_exit(int, int);
void     sshc_cleanup_exit(int, int);
#endif /* L7_SSHD */
#endif

/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
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

#ifndef SR_DIAG_H
#define SR_DIAG_H

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

#if (defined(SR_DEBUG) || defined(WANT_SR_FILENAME))
#define SR_FILENAME static const char sr_filename[] = __FILE__;
#else	/* (defined(SR_DEBUG) || defined(WANT_SR_FILENAME)) */
#define SR_FILENAME
#endif	/* (defined(SR_DEBUG) || defined(WANT_SR_FILENAME)) */

typedef struct _LogEntry {
    SR_INT32    LogLevel;
    SR_UINT32   timestamp;
    char       *progname;
    char       *filename;
    SR_INT32    linenum;
    char       *message;
    void       *UserData;
} LogEntry;


#ifdef SR_DEBUG
typedef void (*LogFunc)(LogEntry *log);

#ifndef SR_LOGBOOK_BUF_SIZE
#define SR_LOGBOOK_BUF_SIZE 1024
#endif	/* SR_LOGBOOK_BUF_SIZE */

#ifndef SR_PROGNAME_BUF_SIZE
#define SR_PROGNAME_BUF_SIZE 80
#endif /* SR_PROGNAME_BUF_SIZE */

#ifndef SR_LOG_MSG_PADDING_SIZE
#define SR_LOG_MSG_PADDING_SIZE 5
#endif /* SR_LOG_MSG_PADDING_SIZE */

#ifdef SR_MAX_LOG_MESSAGE_SIZE
#undef SR_MAX_LOG_MESSAGE_SIZE
#endif /* SR_MAX_LOG_MESSAGE_SIZE */
#define SR_MAX_LOG_MESSAGE_SIZE (((SR_LOGBOOK_BUF_SIZE) - (SR_PROGNAME_BUF_SIZE)) - (SR_LOG_MSG_PADDING_SIZE))

typedef struct LogBook_s {
    SR_INT32    LogLevel;
    char        progname_buf[SR_PROGNAME_BUF_SIZE];
    char        message_buf[SR_LOGBOOK_BUF_SIZE];
    LogEntry   *log_global;
    LogFunc     log_func;
    void       *log_data;
} LogBook;


#define SRLOG_MAX_FORMAT   1  /* highest legal value for format field */

typedef struct _LogAppData {
    SR_INT32    operFlags;
    SR_INT32    format;       /* 0=origianl format, SRLOG_MAX_FORMAT=newest */
    FILE       *logfile_fp;
} LogAppData;


#define SR_LOG( X )  dlfLog(dLog X, __LINE__, sr_filename)

extern  void dlfLog(LogEntry *log, int line, const char *file);
#endif /* SR_DEBUG */

#ifdef SR_DEBUG
extern  LogEntry *dLog(LogBook *log_book, SR_INT32 log_level, ...);
#endif /* SR_DEBUG */
extern  int sr_dprintf(SR_INT32 log_level,...);
extern  void dlfprint(int line,const char *file);



#ifdef SR_DEBUG
LogFunc BkSetLogFunction(LogBook *log_book, LogFunc new_func);

LogFunc SetLogFunction(LogFunc new_func);

LogFunc BkGetLogFunction(LogBook *log_book);

LogFunc GetLogFunction(void);


/* LogFunc functions */

void	dsender(LogBook *log_book, LogEntry *log);

void	FreeLogMessage(LogEntry *log);

void	PrintLogMessage(LogEntry *log);

void	PrintShortLogMessage(LogEntry *log);

void	PrintLogMessageToCloseFile(LogEntry *log);

void	PrintLogMessageToOpenFile(LogEntry *log);

void	PrintSetLogMessage(LogEntry *log);



/* miscellaneous functions */

LogBook *GetLogBook(void);

LogBook *OpenLog(void);

void    CloseLog(LogBook *log_book);

LogEntry *MakeLogMessage(
	SR_INT32    LogLevel,
	SR_UINT32   timestamp,
	const char *progname,
	const char *filename,
	SR_INT32    linenum,
	const char *message,
#if ! ( (defined(lint) && (defined(SRM_AIX) || defined(SRM_AIX64))) )
	void       *UserData
#else	/* ! ( (defined(lint) && (defined(SRM_AIX) || defined(SRM_AIX64))) ) */
        void       *Userdata
#endif	/* ! ( (defined(lint) && (defined(SRM_AIX) || defined(SRM_AIX64))) ) */
);

LogEntry *CopyLogMessage(LogEntry *log);

void	BkSetLogProgramName(LogBook *log_book, char *s);

void	SetProgname(char *s);

void	TestLogMessages(void);

char   *GetLogFullPath(const char *file_name);

int PrintLogAppData(FILE *fp,
		    const LogAppData *data);




#define FNAME(x) static const char Fname[] = x;
#define NULL_CHECK(p,d,l) if ((p) == NULL) { DPRINTF(d); goto l; }


#define DPRINTF( X )  sr_dprintf X, dlfprint(__LINE__, sr_filename)

SR_INT32 BkSetLogLevel(LogBook *log_book, SR_INT32 log_level);

SR_INT32 SetLogLevel(SR_INT32 log_level);

SR_INT32 BkGetLogLevel(LogBook *log_book);

SR_INT32 GetLogLevel(void);

void   *BkSetLogUserData(LogBook *log_book, void *new_data);

void   *SetLogUserData(void *new_data);

void   *BkGetLogUserData(LogBook *log_book);

void   *GetLogUserData(void);

int     init_extensions(void);


#define SRLOG_FILE         0x00000001  /* print log messages to a log file? */
#define SRLOG_TRACEFILE    0x00000002  /* allow trace messages in log file? */
#define SRLOG_APPEND       0x00000004  /* append to existing log file? */
#define SRLOG_STDOUT       0x00000008  /* print log messages to stdout? */
#define SRLOG_STDERR       0x00000010  /* print log messages to stderr? */
#define SRLOG_CLOSEFILE    0x00000100  /* close file after message */
#define SRLOG_RESERVED_D   0x08000000
#define SRLOG_RESERVED_C   0x10000000
#define SRLOG_RESERVED_B   0x20000000
#define SRLOG_RESERVED_A   0x40000000
#define SRLOG_EXTEND       0x80000000  /* reserved for extensions */

#else				/* SR_DEBUG */



#define DPRINTF( X )
#define FNAME(x)
#define NULL_CHECK(p,d,l) if ((p) == NULL) goto l

#endif				/* SR_DEBUG */


/*
 * If you add any additional log levels, be sure to update
 * the function TestLogMessages() in debug.c!
 */

#define APWARN   0x1000
#define APERROR  0x2000
#define APTHREAD 0x4000   /* same as TASKTRACE */
#define APTIMER  0x8000   /* same as TIMERTRACE */


#ifndef APTRACE
#define APTRACE		0x0080	/* Generic Trace */
#endif /* APTRACE */


#define APCONFIG        0x00010000  /* Trace config file i/o */
#define APPACKET        0x00020000  /* Trace SNMP packet parse/builds */
#define APTRAP          0x00040000  /* Trace trap/inform messages */
#define APACCESS        0x00080000  /* Trace access control processing */
#define APVERBOSE       0x00200000  /* Extra debugging tracing */
#define APUSER          0x00400000  /* User debug trace - customers */
#define APAUDIT         0x01000000  /* Audit SNMP set requests */
                    /*  0x00800000     Reserved */
                    /*  0x10000000     Reserved */
                    /*  0x20000000     Reserved */
                    /*  0x40000000     Reserved */
                    /*  0x80000000     Reserved */

#define APALL		0x7FFF7FFF /* OR of all the above DO NOT PASS TO DPRINTF */

#define APALWAYS	0x00000000 /* always print this message PASS TO DPRINTF */


#ifdef SR_DEBUG
char * sys_errname(int i);
#endif	/* SR_DEBUG */

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* ! SR_DIAG_H */

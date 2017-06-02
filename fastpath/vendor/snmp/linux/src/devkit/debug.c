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

#define WANT_ALL_ERRORS

#include "sr_conf.h"


#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>


#include <ctype.h>

#include <string.h>


#include <stdarg.h>




#include "sr_snmp.h"


#include "diag.h"



#ifdef SR_DEBUG

SR_FILENAME


char *
sys_errname(errnum)
  int errnum;
{
    char *msg = strerror(errnum);
    return(msg);
}



/*
 * OpenLog:
 *
 * This function allocates a LogBook structure which contains
 * information about which messages to log (loglevel) and how to
 * process the messages (log_func).
 */

LogBook *
OpenLog()
{
    LogBook  *log_book;

    if((log_book = (LogBook *)malloc(sizeof(LogBook))) == NULL) {
        return NULL;
    }
#ifdef SR_CLEAR_MALLOC
    memset(log_book, 0, sizeof(LogBook));
#endif	/* SR_CLEAR_MALLOC */

    log_book->LogLevel = 0;
    log_book->log_global = NULL;
    log_book->log_func = PrintShortLogMessage;
    log_book->log_data = NULL;
    BkSetLogProgramName(log_book, NULL);

    return log_book;
}


/*
 * CloseLog:
 *
 * This function frees a LogBook structure.
 */

void
CloseLog(log_book)
    LogBook  *log_book;
{
    if (log_book != NULL) {
	free(log_book);
    }
}


/*
 * BkSetLogLevel:
 *
 * This function is an interface to the LogLevel variable to allow
 * developers without source code to change the value inside the
 * method routines.
 */

SR_INT32
BkSetLogLevel(log_book, log_level)
    LogBook  *log_book;
    SR_INT32  log_level;
{
    SR_INT32 previous_log_level;

    if (log_book != NULL) {
	previous_log_level = log_book->LogLevel;
	log_book->LogLevel = log_level;
	return (previous_log_level);
    }
    else {
	return -1;
    }
}


/*
 * BkGetLogLevel:
 *
 * This function is an interface to the LogLevel variable to allow
 * developers without source code to obtain the value inside the
 * method routines.
 */

SR_INT32
BkGetLogLevel(log_book)
    LogBook  *log_book;
{
    if (log_book != NULL) {
	return (log_book->LogLevel);
    }
    else {
	return -1;
    }
}


/*
 * BkSetLogFunction:
 *
 * This function is an interface to the variable which stores the
 * function pointer for the handler() for DPRINTF.  This function
 * allows developers without source code to change how DPRINTF
 * behaves at runtime (via command-line arguments, etc).
 */

LogFunc
BkSetLogFunction(log_book, new_func)
    LogBook  *log_book;
    LogFunc    new_func;
{
    LogFunc previous_func;

    if (log_book != NULL) {
	previous_func = log_book->log_func;
	log_book->log_func = new_func;
	return (previous_func);
    }
    else {
	return NULL;
    }
}


/*
 * BkGetLogFunction:
 *
 * This function is an interface to the variable which stores the
 * function pointer for the handler() for DPRINTF.  This function allows
 * developers without source code to obtain the value inside the
 * method routines.
 */

LogFunc
BkGetLogFunction(log_book)
    LogBook  *log_book;
{
    if (log_book != NULL) {
	return (log_book->log_func);
    }
    else {
	return NULL;
    }
}


/*
 * BkSetLogUserData:
 *
 * This function is an interface to the variable which stores the
 * value for UserData passed to the handler() for DPRINTF.  This function
 * allows developers without source code to change how DPRINTF
 * behaves at runtime (via command-line arguments, etc).
 */

void *
BkSetLogUserData(log_book, new_data)
    LogBook  *log_book;
    void     *new_data;
{
    void *previous_data;

    if (log_book != NULL) {
	previous_data = log_book->log_data;
	log_book->log_data = new_data;
	return (previous_data);
    }
    else {
	return NULL;
    }
}


/*
 * BkGetLogFunction:
 *
 * This function is an interface to the variable which stores the
 * value for UserData passed to the handler() for DPRINTF.  This
 * function allows developers without source code to obtain the value
 * inside the method routines.
 */

void *
BkGetLogUserData(log_book)
    LogBook  *log_book;
{
    if (log_book != NULL) {
	return (log_book->log_data);
    }
    else {
	return NULL;
    }
}


/*
 * BkSetLogProgramName:
 */
void
BkSetLogProgramName(log_book, s)
    LogBook  *log_book;
    char *s;
{
    char     *p, *tmp;
    int len;

    if (log_book != NULL) {
	if (s == NULL || strlen(s) == 0) {
            log_book->progname_buf[0] = '\0';
	} else {
	    p = s;
            if ((tmp = strrchr(s, '/')) != NULL) {
                p = tmp + 1;
            }
            if ((tmp = strrchr(s, '\\')) != NULL) {
                p = tmp + 1;
            }
            len = strlen(p);
            if (!strcmp(p+len-4, ".exe")) {
                len -= 4;
            }
            strncpy(log_book->progname_buf, p, len);
            log_book->progname_buf[len+1] = '\0';
        }
    }
}


void
FreeLogMessage(log)
    LogEntry *log;
{
    if (log != NULL) {
	if (log->message != NULL) {
	    free(log->message);
	}
	if (log->filename != NULL) {
	    free(log->filename);
	}
	if (log->progname != NULL) {
	    free(log->progname);
	}
	free(log);
    }
}


LogEntry *
MakeLogMessage(LogLevel, timestamp, progname, filename, linenum, message, UserData)
    SR_INT32    LogLevel;
    SR_UINT32   timestamp;
    const char *progname;
    const char *filename;
    SR_INT32    linenum;
    const char *message;
    void       *UserData;
{
    LogEntry   *new_log;
    int         len;

    if ((new_log = (LogEntry *) malloc((size_t) sizeof(LogEntry))) == NULL) {
	goto fail;
    }
    else {
	new_log->LogLevel = LogLevel;
	new_log->timestamp = timestamp;
	new_log->progname = NULL;
	new_log->filename = NULL;
	new_log->linenum = linenum;
	new_log->message = NULL;
	new_log->UserData = UserData;
    }
    if (progname != NULL && (len = strlen(progname)) > 0) {
	if ((new_log->progname = malloc((size_t)len+1)) == NULL) {
	    goto fail;
	}
        else {
	    strcpy(new_log->progname, progname);
        }
    }
    if (filename != NULL && (len = strlen(filename)) > 0) {
	if ((new_log->filename = malloc((size_t)len+1)) == NULL) {
	    goto fail;
	}
	else {
	    strcpy(new_log->filename, filename);
	}
    }
    if (message != NULL && (len = strlen(message)) > 0) {
        if ((new_log->message = malloc((size_t)len+1)) == NULL) {
	    goto fail;
	}
	else {
	    strcpy(new_log->message, message);
	}
    }
    return new_log;

fail:
    FreeLogMessage(new_log);
    return NULL;

}


LogEntry *
CopyLogMessage(log)
    LogEntry *log;
{
    if (log == NULL) {
	return NULL;
    }
    else {
	return MakeLogMessage(log->LogLevel,
			      log->timestamp,
			      log->progname,
			      log->filename,
			      log->linenum,
			      log->message,
			      log->UserData);
    }
}


/*
 * dLog:
 *
 * This function is part one of a two-step process to construct and send
 * a log message using the LOG() macro.  This function must be used ONLY
 * in conjunction with dlfLog(), since the return value is a pointer to
 * an overloaded LogEntry structure (containing a LogBook * in UserData).
 */

/*
 * The following version of dLog is more in line with ANSI C than the
 * other version, but the other version is portable to a wider variety of
 * platforms.
 */

LogEntry *
dLog(LogBook *log_book, SR_INT32 log_level, ...)
{
    va_list    arg;
    char      *format;
    LogEntry  *log = NULL;

    va_start(arg, log_level);

    format    = va_arg(arg, char *);

    if (log_book != NULL
	&& log_book->log_func != NULL
	&& log_book->log_func != FreeLogMessage
	&& (log_level == 0 || (log_level & log_book->LogLevel) != 0)) {

	if (log_book->progname_buf[0] != '\0') {
	    /*
	     * if you change the format string in the next line, you
	     * must also change SR_LOG_MSG_PADDING_SIZE
	     */
	    sprintf(log_book->message_buf, "%s: ", log_book->progname_buf);
	}
	(void) vsprintf(log_book->message_buf, format, arg);


	log = MakeLogMessage(log_level,
			     GetTimeNow(),
			     log_book->progname_buf,
	/*
	 * filename is provided by dlfLog()
	 */
			     NULL,
	/*
	 * linenum is provided by dlfLog()
	 */
			     0,
			     log_book->message_buf,
	/*
	 * UserData is overloaded with a pointer to the LogBook structure
	 * so dlfLog() can access it.
	 */
			     (void *) log_book);
    }

    va_end(arg);
    return log;
}



/*
 * dlfLog:
 *
 * This function is part two of a two-step process to construct and send
 * a log message using the LOG() macro.  This function must be used ONLY
 * in conjunction with dLog(), since it accesses the LogBook structure
 * in an unusual way.  Instead of accepting a pointer to the LogBook as
 * a parameter, this function expects the LogEntry structure to be
 * overloaded, and it recasts log->UserData as a (LogBook *).
 */

void
dlfLog(log, line, file)
    LogEntry *log;
    int       line;
    const char *file;
{
    LogBook  *log_book;
    int       len;

    if (log == NULL || log->UserData == NULL) {
	return;
    }
    else {
	log_book = (LogBook *) log->UserData;
	log->UserData = NULL;
    }

    log->linenum = line;

    if (file != NULL && (len = strlen(file)) > 0) {
	if ((log->filename = malloc((size_t)len+1)) == NULL) {
	    free(log->progname);
	    free(log->message);
	    free(log);
	    return;
	}
	else {
	    strcpy(log->filename, file);
	}
    }

    dsender(log_book, log);
    FreeLogMessage(log);
}


void
dsender(log_book, log)
    LogBook  *log_book;
    LogEntry *log;
{
    if (log_book != NULL
	&& log_book->log_func != NULL
	&& log_book->log_func != FreeLogMessage) {

	log->UserData = log_book->log_data;
	(*log_book->log_func)(log);
    }
}


void
TestLogMessages()
{
    DPRINTF((APTRACE,   "Testing log message facility for APTRACE.\n"));

    DPRINTF((APWARN,    "Testing log message facility for APWARN.\n"));
    DPRINTF((APERROR,   "Testing log message facility for APERROR.\n"));
    DPRINTF((APTHREAD,  "Testing log message facility for APTHREAD.\n"));
    DPRINTF((APTIMER,   "Testing log message facility for APTIMER.\n"));

    DPRINTF((APCONFIG,  "Testing log message facility for APCONFIG.\n"));
    DPRINTF((APPACKET,  "Testing log message facility for APPACKET.\n"));
    DPRINTF((APTRAP,    "Testing log message facility for APTRAP.\n"));
    DPRINTF((APACCESS,  "Testing log message facility for APACCESS.\n"));

    DPRINTF((APVERBOSE, "Testing log message facility for APVERBOSE.\n"));
    DPRINTF((APUSER,    "Testing log message facility for APUSER.\n"));
    DPRINTF((APAUDIT,   "Testing log message facility for APAUDIT.\n"));
}


void
PrintShortLogMessage(log)
    LogEntry *log;
{
    FILE *fp = stderr;
    if (log) {
	if (log->UserData) {
	    fp = (FILE *) log->UserData;
	}
	(void)fputs(log->message, fp);
	(void)fprintf(fp,
		"                              at line %d in file %s\n",
		(int)log->linenum, log->filename);
	(void)fflush(fp);
    }
}

void
PrintSetLogMessage(log)
    LogEntry *log;
{
    FILE *fp = stderr;
    if (log) {
	if (log->UserData) {
	    fp = (FILE *) log->UserData;
	}
	(void)fprintf(fp, "%s\n", log->message);
	(void)fflush(fp);
    }
}

void
PrintLogMessage(log)
    LogEntry *log;
{
    FILE *fp = stderr;
    char buf[24];
    SrTOD tod;

    if (log) {
	if (log->UserData) {
	    fp = (FILE *) log->UserData;
	}
        sysUpToTOD((unsigned int)log->timestamp, &tod);

	(void)fprintf(fp, "LogLevel:   ");
        if (log->LogLevel & APTRACE)	fprintf(fp, " APTRACE");
        if (log->LogLevel & APWARN)	fprintf(fp, " APWARN");
        if (log->LogLevel & APERROR)	fprintf(fp, " APERROR");
        if (log->LogLevel & APTHREAD)	fprintf(fp, " APTHREAD");
        if (log->LogLevel & APCONFIG)	fprintf(fp, " APCONFIG");
        if (log->LogLevel & APPACKET)	fprintf(fp, " APPACKET");
        if (log->LogLevel & APTRAP)	fprintf(fp, " APTRAP");
        if (log->LogLevel & APACCESS)	fprintf(fp, " APACCESS");
        if (log->LogLevel & APVERBOSE)	fprintf(fp, " APVERBOSE");
        if (log->LogLevel & APUSER)	fprintf(fp, " APUSER");
        if (log->LogLevel == 0)		fprintf(fp, " APALWAYS");
	(void)fprintf(fp, "\n");
	(void)fprintf(fp, "timestamp:   %u (%s)\n",   
                      (unsigned int)log->timestamp,
                      PrintTOD(&tod, buf, sizeof(buf)));
	if (log->progname == NULL) {
	    (void)fprintf(fp, "progname:    \n");
	}
	else {
	    (void)fprintf(fp, "progname:    %s\n",   log->progname);
	}
	(void)fprintf(fp, "filename:    %s\n",   log->filename);
	(void)fprintf(fp, "linenum:     %d\n",   (int)log->linenum);
	(void)fprintf(fp, "message:     %s",     log->message);
	if (log->UserData == NULL) {
	    (void)fprintf(fp, "UserData:    \n");
	}
	else {
          (void)fprintf(fp, "UserData:    0x%lx\n", (unsigned long) log->UserData);
	}
	(void)fprintf(fp, "\n");
	(void)fflush(fp);
    }
}


int
PrintLogAppData(FILE *fp,
		const LogAppData *data)
{
    if (fp == NULL || data == NULL) {
	return -1;
    }
    (void)fprintf(fp, "operFlags:  ");
    if (data->operFlags & SRLOG_FILE)        fprintf(fp, " SRLOG_FILE");
    if (data->operFlags & SRLOG_TRACEFILE)   fprintf(fp, " SRLOG_TRACEFILE");
    if (data->operFlags & SRLOG_APPEND)      fprintf(fp, " SRLOG_APPEND");
    if (data->operFlags & SRLOG_STDOUT)      fprintf(fp, " SRLOG_STDOUT");
    if (data->operFlags & SRLOG_STDERR)      fprintf(fp, " SRLOG_STDERR");
    if (data->operFlags & SRLOG_RESERVED_D)  fprintf(fp, " SRLOG_RESERVED_D");
    if (data->operFlags & SRLOG_RESERVED_C)  fprintf(fp, " SRLOG_RESERVED_C");
    if (data->operFlags & SRLOG_RESERVED_B)  fprintf(fp, " SRLOG_RESERVED_B");
    if (data->operFlags & SRLOG_RESERVED_A)  fprintf(fp, " SRLOG_RESERVED_A");
    if (data->operFlags & SRLOG_EXTEND)      fprintf(fp, " SRLOG_EXTEND");
    (void)fprintf(fp, "\n");
    (void)fprintf(fp, "format:      %d\n",   data->format);
    (void)fprintf(fp, "logfile_fp:  ");
    if (data->logfile_fp == stdout) {
	fprintf(fp, "stdout\n");
    }
    else if (data->logfile_fp == stderr) {
	fprintf(fp, "stderr\n");
    }
    else {
      fprintf(fp, "0x%lx\n", (unsigned long) data->logfile_fp);
    }
    return 1;
}

#endif /* SR_DEBUG */


/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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


#include <ctype.h>

#include <string.h>

#include <stdarg.h>


#include "sr_snmp.h"


#include "diag.h"




#ifdef SR_DEBUG
SR_INT32
SetLogLevel(log_level)
    SR_INT32 log_level;
{
    return BkSetLogLevel(GetLogBook(), log_level);
}

SR_INT32
GetLogLevel()
{
    return BkGetLogLevel(GetLogBook());
}

LogFunc
SetLogFunction(new_func)
    LogFunc new_func;
{
    return BkSetLogFunction(GetLogBook(), new_func);
}

LogFunc
GetLogFunction()
{
    return BkGetLogFunction(GetLogBook());
}

void *
SetLogUserData(new_data)
    void *new_data;
{
    return BkSetLogUserData(GetLogBook(), new_data);
}

void *
GetLogUserData()
{
    return BkGetLogUserData(GetLogBook());
}

void
SetProgname(s)
    char *s;
{
    BkSetLogProgramName(GetLogBook(), s);
}
#endif /* SR_DEBUG */


/*
 *    sr_dprintf() is a wrapper around the Log interface and is provided
 *    for backwards-compatability purposes.  It is intended that
 *    sr_dprintf() be used in conjunction with the DPRINTF macro, which
 *    must be defined as follows:
 *
 *    #ifdef SR_DEBUG
#ifndef PKG
 *    #define DPRINTF( X ) sr_dprintf X, dlfprint(__LINE__, __FILE__)
#else
 *    #define DPRINTF( X ) sr_dprintf X, dlfprint(0, NULL)
#endif
 *    #else
 *    #define DPRINTF( X )
 *    #endif
 *
 *    The call would be:    DPRINTF(( log_level, format_str, argv ));
 *
 *    DPRINTF can also be defined as nothing so that in a system
 *    which is pressed for text space it can be totally eliminated.
 */

/*
 * The following version of sr_dprintf is more in line with ANSI C than the
 * other version, but the other version is portable to a wider variety of
 * platforms.
 */

/*
 * sr_dprintf:
 */

#include "sysapi.h"

int sr_dprintf_flag = 0;

void
snmp_debugFlag(int flag)
{
  sr_dprintf_flag = flag;
}

int
sr_dprintf(SR_INT32 log_level, ...)
{
/* LVL7 */
    int        return_value = 0;
    va_list    arg;
    char      *format;
    char       output_str[180];

    if (sr_dprintf_flag)
    {
      va_start(arg, log_level);
  
      format = va_arg(arg, char *);
  
      (void) vsnprintf(output_str, 180, format, arg);
      output_str[179] = 0;
  
      sysapiPrintf(output_str);

      va_end(arg);
    }

#ifdef OLD_CODE /* LVL7 */
#ifdef SR_DEBUG
    va_list    arg;
    char      *format;
    LogBook   *log_book;

    va_start(arg, log_level);

    format = va_arg(arg, char *);
    log_book = GetLogBook();

    if (log_book != NULL
	&& log_book->log_global == NULL
	&& log_book->log_func != NULL
	&& log_book->log_func != FreeLogMessage
	&& (log_level == 0 || (log_level & log_book->LogLevel) != 0)) {

        int len = 0;
        if (log_book->progname_buf != NULL) {
            if ((len = strlen(log_book->progname_buf)) > 0)  {
                strncpy(log_book->message_buf, log_book->progname_buf, 
                             MIN(len, SR_LOGBOOK_BUF_SIZE - 1));
                if (len < SR_LOGBOOK_BUF_SIZE - 3) {
                    strcat(log_book->message_buf, ": ");
                    len += 2;
                }
            }
        }

	(void) vsnprintf(log_book->message_buf + len,
                         (SR_LOGBOOK_BUF_SIZE - 1) - len,
                          format, arg);
        log_book->message_buf[SR_LOGBOOK_BUF_SIZE - 1] = '\0';

	log_book->log_global = MakeLogMessage(log_level,
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
#endif /* SR_DEBUG */
#endif /* LVL7 */
/* LVL7 */
/*
    return 0;
*/
    return return_value;
/* LVL7 end */
}     /* sr_dprintf() */



void
dlfprint(line, file)
    int       line;
    const char *file;
{
#ifdef SR_DEBUG
    LogBook  *log_book;

    log_book = GetLogBook();

    if (log_book != NULL
	&& log_book->log_global != NULL
	&& log_book->log_func != NULL
	&& log_book->log_func != FreeLogMessage
	&& (log_book->log_global->LogLevel == 0
	|| (log_book->log_global->LogLevel & log_book->LogLevel) != 0)) {

	dlfLog(log_book->log_global, line, file);
	log_book->log_global = NULL;
    }

#endif /* SR_DEBUG */
}     /* dlfprint() */

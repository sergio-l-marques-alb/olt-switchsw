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

/*
 *  This is used as the logging function (in the call to 
 *  SetLogFunction() in many subagents.  Split out of debug.c
 *  to avoid missing symbol problems in non-subagent applications
 *  that require debug.c but not this function.
 */
void
PrintLogMessageToCloseFile(log)
    LogEntry *log;
{
    FILE *fp=NULL;
    char *logpath=NULL;

    if (log) {
       logpath = GetLogFullPath(log->UserData);
       if (logpath != NULL) {
    /* sr_daemonize should open the first time and print banner */
    /* this function should be used with daemonized process */
          fp = fopen(logpath, "a");
          if (fp != NULL) {
               (void)fputs(log->message, fp);
               (void)fprintf(fp,
                    "                              at line %d in file %s\n",
               (int)log->linenum, log->filename);
               (void)fflush(fp);
               fclose(fp);
               free(logpath);
          }
       }
       else {
         if(log->UserData != NULL){
            fprintf(stderr, "Unable to open path of %s\n",
                    (char *)log->UserData);
         }
       }
    }
   
    return;
}
#endif /* SR_DEBUG */

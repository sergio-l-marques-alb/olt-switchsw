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


#include "sr_conf.h"

#include <stdio.h>

#include <malloc.h>

#include <stdlib.h>

#include <limits.h>

#include <unistd.h>

#include <fcntl.h>

#include <string.h>

#include <sys/types.h>

#include <sys/stat.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_proto.h"
#include "sr_dmn.h"
#include "diag.h"

#ifdef SR_DEBUG
int stderr_closed = 0;
int logfile_open = 0;
#endif /* SR_DEBUG */

/* ----------------------------------------------------------------------
 *  sr_daemon() - daemonize this process
 *
 *  Arguments:
 *    int nochdir
 *      If zero, change current workding directory to "/".
 *    int noclose
 *      If zero, redirect stdin and stdout to /dev/null, and redirect
 *      stderr to either a log file or /dev/null.  Otherwise, make no changes.
 *    char *logfilename
 *      contains the name of a logfile to which stderr should be redirected.
 *      if NULL, stderr is redirected to /dev/null
 *
 *  Returns:
 *    int
 *      -1 if the fork fails, otherwise 0.
 *
 *  Commentary:
 *      Function renamed sr_daemon() from daemon() to avoid confusion
 *      with freebsd (bsd4.4) daemon() call.  Furthermore, the freebsd call
 *      actions vary slightly with ours.
 *
 *      If this is the calling process, this call does not return (exit(0)
 *      is called).  If this is the child process, control return to the
 *      call point, and the log file is opened.
 *
 *      Special case Solaris code is to cheaply avoid a feature where 
 *      a signal is asserted if the file being closed is not open.
 */
int
sr_daemon(nochdir, noclose, logfilename)
    int nochdir;
    int noclose;
    const char *logfilename;
{
    int pid;
    int i;
    char *fullpath;

    if(!noclose) {
      for(i = 0; i < 3; i++)
	{
	  close(i);
	}
#ifdef SR_DEBUG
      stderr_closed = 1;
#endif /* SR_DEBUG */
    }

    if((pid = fork()) < 0)
      return -1;
    else if(pid != 0)
      exit(0);

    setsid();
    if(!nochdir) {
      chdir("/");
    }

    umask(0);

    if (!noclose) {
       open("/dev/null", O_RDWR, 0755);
       open("/dev/null", O_RDWR, 0755);
       if (!(fullpath=GetLogFullPath(logfilename))) {
           open("/dev/null", O_RDWR, 0755);
       } else {
	   (void) unlink(fullpath);
           if (open(fullpath, O_WRONLY | O_CREAT, 0444) != -1) {
#ifdef SR_DEBUG
		logfile_open = 1;
#endif /* SR_DEBUG */
	   }
	   free(fullpath);
       }
    }

    return 0;
}


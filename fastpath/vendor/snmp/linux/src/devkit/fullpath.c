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

#include <string.h>

#include <malloc.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_proto.h"
#include "diag.h"


#ifndef SR_DEFAULT_LOG_DIR
 #define SR_DEFAULT_LOG_DIR "/tmp"
#endif /* SR_DEFAULT_LOG_DIR */

static char *getprefix
    SR_PROTOTYPE((void));

static char *
getprefix()
{
    char           *prefix = NULL;
    if ((prefix = getenv("SR_LOG_DIR")) == NULL) {
	prefix = SR_DEFAULT_LOG_DIR;
    }
    if (prefix[strlen(prefix) - 1] == '/') {
	prefix[strlen(prefix) - 1] = '\0';
    }
    return prefix;
}

char *
GetLogFullPath(const char *file_name)
{
    char            *sr_log_dir;
    char            *log_path=NULL;


    if (file_name == NULL) {
	return (char *) NULL;
    }

    /* Return a string containing the prefix directory.
       Respect SR_LOG_DIR if set. */

    sr_log_dir = getprefix();

    if (!(log_path=malloc(strlen(sr_log_dir)+1+strlen(file_name)+1))) {
	return (char *) NULL;
    }
    sprintf(log_path, "%s/%s", sr_log_dir, file_name);

    return log_path;
}

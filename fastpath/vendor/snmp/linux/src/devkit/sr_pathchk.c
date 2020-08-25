/*
 * Copyright (C) 2006 by SNMP Research, Incorporated.
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

/* --- Needed for free() prototype --- */
#include <stdlib.h>

/* --- Needed prerequisite for sys/stat.h --- */
#include <sys/types.h>

/* --- Needed for stat() prototype --- */
#include <sys/stat.h>


/* --- Needed for strdup() prototype --- */
#include <string.h>


/* --- Needed for SNMP Research common constants, prototypes, etc. --- */
#include "sr_snmp.h"

/* --- Needed for DPRINTF --- */
#include "diag.h"
SR_FILENAME

/* --- Needed for SrPathCheck() prototype --- */
#include "sr_pathchk.h"


/*
 *  SrPathCheck()
 *
 *  Function:  Verifies a file is writable only by root (or its owner).
 *
 *  Inputs:    filepath - Specifies the complete file path from the file
 *                        system root and including the file name.
 *             flags    - flags that modify the function's behavior:
 *
 *                        SR_PC_ANY_OWNER - skip the "root" user check
 *
 *  Outputs:    0:  The file exists, is a regular file, and is writable only
 *                  by root (or its owner).
 *              1:  The file exists and is a regular file, but may be
 *                  writable by a non-root (or non-owner) user.
 *             -1:  The file does not exist, is not a regular file, or 
 *                  an internal error occurred.
 *
 *  Notes:     This function also checks all subdirectories in the path
 *             from the file system root.
 *
 *             This function is useless on MS-Windows because the MS-Windows
 *             implementation of stat():
 *             
 *             -  *always* returns a st_uid value of 0 (root).
 *             -  does not provide information about whether a file
 *                can be read or written by group or other.
 */
int
SrPathCheck(const char *filepath, const SR_UINT32 flags)
{
    int    i, slen;
    char   *lfilepath = NULL;
    struct stat buf;
    FNAME("SrPathCheck");

    /* --- Return success if no file --- */
    if (filepath == NULL) {
        DPRINTF((APTRACE, "%s:  No file path was specified\n", Fname));
        goto fail;
    }

    /* --- Copy the file path so it can be modified --- */
    lfilepath = strdup(filepath);
    if (lfilepath == NULL) {
        DPRINTF((APERROR|APWARN|APTRACE, "%s:  Malloc failure\n", Fname));
        goto fail;
    }
    DPRINTF((APTRACE, "%s:  Checking file %s\n", Fname, lfilepath));

    /* --- Get the status of the complete file --- */
    if (stat(lfilepath, &buf) == -1) {
        DPRINTF((APERROR|APWARN|APTRACE, 
            "%s:  Unable to stat %s\n", Fname, lfilepath));
        goto fail;
    }

    /* --- Verify the file is a regular file --- */
    if (!(buf.st_mode & S_IFREG)) {
        DPRINTF((APWARN|APTRACE, 
            "%s:  File %s is not a regular file\n", Fname, lfilepath));
        goto fail;
    }

    /* --- Skip the root user check if specified --- */
    if ((flags & SR_PC_ANY_OWNER) != SR_PC_ANY_OWNER) {

        /* --- Verify the file is owned by root --- */
        if (buf.st_uid != 0) {
            DPRINTF((APWARN|APTRACE, 
                "%s:  File %s is not owned by root\n", Fname, lfilepath));
            goto warning;
        }
    }

    /* --- Verify the file is not writable by group or other --- */
    if ((buf.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
        DPRINTF((APWARN|APTRACE, 
            "%s:  File %s is writable by group or other\n", Fname, lfilepath));
        goto warning;
    }

    /*
     *  Loop through the file path, from the tail to the start.  Change
     *  path separator characters to NULL, then check the path.
     */
    slen = strlen(lfilepath);
    for (i = slen; i >= 0; i--) {

        /* --- Skip characters that are not path separators --- */
        if (lfilepath[i] != '/') {
            continue;
        }

        /* --- Change the path separator to a NULL --- */
        lfilepath[i] = '\0';

        /* --- Check the file system root --- */
        if (i == 0) {
            lfilepath[0] = '/';
            lfilepath[1] = '\0';
        }

        /* --- Get the status of the directory --- */
        if (stat(lfilepath, &buf) == -1) {
            DPRINTF((APERROR|APWARN|APTRACE, 
                "%s:  Unable to stat %s\n", Fname, lfilepath));
            goto fail;
        }

        /* --- Verify the path specifies a directory --- */
        if (!(buf.st_mode & S_IFDIR)) {
            DPRINTF((APWARN|APTRACE, 
                "%s:  %s is not a directory\n", Fname, lfilepath));
            goto fail;
        }

        /* --- Skip the root user check if specified --- */
        if ((flags & SR_PC_ANY_OWNER) != SR_PC_ANY_OWNER) {

            /* --- Verify the directory is owned by root --- */
            if (buf.st_uid != 0) {
                DPRINTF((APWARN|APTRACE, 
                "%s:  Directory %s is not owned by root\n", Fname, lfilepath));
                goto warning;
            }
        }

        /* --- Verify the directory is not writable by group or other --- */
        if ((buf.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
            DPRINTF((APWARN|APTRACE, 
                "%s:  Directory %s is writable by group or other\n", 
                Fname, lfilepath));
            goto warning;
        }
    }

    /* --- Successful return --- */
    if (lfilepath != NULL) {
        free(lfilepath);
    }

    DPRINTF((APTRACE, "%s:  File %s is writable only by file owner\n",
             Fname, filepath));
    return 0;


    /* --- Warning return --- */
warning:
    if (lfilepath != NULL) {
        free(lfilepath);
    }

    return 1;


    /* --- Failure return --- */
fail:
    if (lfilepath != NULL) {
        free(lfilepath);
    }

    return -1;
}    /* SrPathCheck() */

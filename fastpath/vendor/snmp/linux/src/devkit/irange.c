/*
 *
 * Copyright (C) 2005-2006 by SNMP Research, Incorporated.
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

/*
 *  This file implements APIs that parse strings containing integer ranges.
 */

#include "sr_conf.h"


/* --- Needed for printf() and sscanf() prototypes --- */
#include <stdio.h>


/* --- Needed for strlen() and strdup() prototypes on some platforms --- */
#include <string.h>

/* --- Needed for isdigit() --- */
#include <ctype.h>

/* --- Needed for malloc() and free() prototypes --- */
#include <stdlib.h>

#include "sr_type.h"
#include "sr_snmp.h"
#include "irange.h"




/*
 *  ParseIntegerRange()
 *
 *  Function:  Parses a string containing a single range of integers
 *             into a single iRange structure.
 *
 *  Inputs:   singleRangeString - specifies a string containing a
 *            single integer range, like so:   "161-169"
 *
 *  Outputs:  None returned explicitly.  If successful, this function
 *            will populate the passed-in iRange structure.
 *
 *  Notes:    If a range specifies a single integer, the first and last 
 *            elements of the returned iRange will have the same value.
 *
 *            If an invalid argument is specified, like "163-169-182",
 *            all elements of the returned iRange structure will be zero.
 */
static void
ParseIntegerRange(char *singleRangeString, iRange *r)
{
    int  hypenCount, i, rlen;
    SR_INT32  end, start;

    /* --- Return if no iRange argument --- */
    if (r == NULL) {
        return;
    }

    /* --- Reset the elements of the iRange argument --- */
    r->count = 0;
    r->first = 0;
    r->last  = 0;

    /* --- Return if no range string argument --- */
    if (singleRangeString == NULL) {
        return;
    }

    rlen = strlen(singleRangeString);
    if (rlen < 1) {
        return;
    }

    /* --- Count the number of hyphens in the range string argument --- */
    hypenCount = 0;
    for (i = 0; i < rlen; i++) {
        if (singleRangeString[i] == '-') {
            hypenCount++;
        }
    }

    /* --- Process argument strings with 0, 1, or more hyphens --- */
    switch (hypenCount) {
        /* --- The range string has a single integer value --- */
        case 0:
            /* --- Parse the value --- */
            i = sscanf(singleRangeString, "%d", &start);
            if (i != 1) {
                return;
            }

            /* --- Save the value --- */
            r->count = 1;
            r->first = start;
            r->last  = start;

            break;

        /* --- The range string has a start value and an end value --- */
        case 1:
            /* --- Parse the value --- */
            i = sscanf(singleRangeString, "%d-%d", &start, &end);
            if (i != 2) {
                return;
            }

            /* --- Save the values --- */
            r->first = start;
            r->last  = end;
            if (end >= start) {
                r->count = end - start + 1;  /* Add 1 to include start/end */
            }
            else {
                r->count = (start-end) + 1;  /* Add 1 to include start/end */
            }

            break;

        /* --- Range strings with other numbers of hyphens are errors --- */
        default:
            break;
    }

    return;
}   /* ParseIntegerRange() */


/*
 *  ParseIntegerRanges()
 *
 *  Function:  Parses a string containing one or more integer ranges into
 *             an array of integer ranges.
 *
 *  Inputs:   rangeString - specifies a string of integer ranges.
 *
 *  Outputs:  On failure, returns NULL.  On success, returns an array of
 *            portRange structures.  The last element of the array has the
 *            count value set to -1.  Note this array must be freed by the
 *            caller with the standard free() function.
 */
iRange *
ParseIntegerRanges(const char *rangeString)
{
    int    currentRangeIndex, rlen, i, rangeCount, singleRangeStartChar;
    char   *lrs = NULL;
    iRange  *result = NULL;

    /* --- Return if no range string --- */
    if (rangeString == NULL) {
        goto done;
    }

    rlen = strlen(rangeString);
    if (rlen < 1) {
        goto done;
    }

    /* --- Verify the range string has only numbers, commas and hyphens --- */
    for (i = 0; i < rlen; i++) {

        if (isdigit(rangeString[i])) {
            continue;
        }
        if ((rangeString[i] != ',') && (rangeString[i] != '-')) {
            goto done;
        }
    }

    /* --- Copy the input range string for strtok() --- */
    lrs = strdup(rangeString);

    /* --- Count the number of ranges and change commas to NULLs --- */
    rangeCount = 1;   /* Have a minimum of 1 range */
    for (i = 0; i < rlen; i++) {
        if (lrs[i] == ',') {
            lrs[i] = '\0';
            rangeCount++;
        }
    }
    rangeCount++;   /* Need an element for terminator */

    /* --- Allocate the array of ranges --- */
    result = (iRange *) malloc(rangeCount * sizeof(iRange));
    if (result == NULL) {
        goto done;
    }

    /* --- Initialize the array of ranges --- */
    for (i = 0; i < rangeCount; i++) {
        result[i].count = -1;
        result[i].first = 0;
        result[i].last  = 0;
    }

    /* --- Initialize for populating ranges --- */
    currentRangeIndex    = 0;
    singleRangeStartChar = 0;

    /*
     *  Iterate through the range tokens to populate the ranges.  Note this
     *  is done after the (duplicate of the) argument range string has had
     *  the commas changed to NULLs like so:
     *
     *       a-b,c,d,e-f,g
     *
     *  is changed to:
     *
     *       a-b\0c\0d\0e-f\0g
     */
    for (i = 0; i < rlen; i++) {

        /* --- Parse the next range token --- */
        if ((lrs[i] == '\0') || (i == (rlen-1))) {

            ParseIntegerRange(&lrs[singleRangeStartChar],
                              &result[currentRangeIndex]);
            singleRangeStartChar = i + 1;
            currentRangeIndex++;
        }
    }

done:
    if (lrs != NULL) {
        free(lrs);
    }

    return result;
}   /* ParseIntegerRanges() */



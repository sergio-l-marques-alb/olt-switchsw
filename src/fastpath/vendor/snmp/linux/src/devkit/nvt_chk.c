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


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME



/*
 * IsDisplayString
 *
 * Checks if the contents of an OctetString is a valid NVT string.  If it
 * is, returns 1, otherwise returns 0.
 */
int
IsDisplayString(os)
    const OctetString *os;
{
    int i;

    if (os == NULL) {
        return(0);
    }
    if (os->octet_ptr == NULL) {
        return(0);
    }

    for (i=0; i<os->length; i++) {
        if (os->octet_ptr[i] == '\r') {
            i++;
            if (i >= os->length) {
                return(0);
            }
            if ((os->octet_ptr[i] != '\n') && (os->octet_ptr[i] != '\0')) {
                return(0);
            }
        } else if (os->octet_ptr[i] == '\n') {
            continue;
        } else
        if ((os->octet_ptr[i] < ' ') || (os->octet_ptr[i] > '~')) {
            return(0);
        }
    }
    return(1);
}

/*
 * ConvertToDisplayString
 *
 * Converts an OctetString to an NVT string.  This consists of converting
 * all newline characters (\n) to a carriage-return/linefeed pair (\r\n),
 * and converting any single carriage-return characters which are not
 * followed by a NULL character into a carriage-return/NULL pair (\r\0).
 * If there are any non-printable characters, the function returns 0,
 * otherwise it returns 1.
 *
 * Re-coded to return a new OctetString instead of modifying the parameter
 * in place.  Parameter is now freed, except on failure.  Returns NULL
 * on failure.
 */
OctetString *
ConvertToDisplayString(os)
    OctetString *os;
{
    int i, j;
    unsigned char *buf;
    OctetString *octet_str;

    /* if zero length do nothing, just return original OctetString * */
    if(!os->length) return os;

    buf = (unsigned char *)malloc(os->length * 2);
    if (buf == NULL) {
        DPRINTF((APTRACE, "ConvertToDisplayString: malloc buf failed.\n"));
	return NULL;
    }

    for (i=0,j=0; i<os->length; i++,j++) {
        if (os->octet_ptr[i] == '\n') {
            buf[j] = '\r';
            buf[++j] = '\n';
        } else
        if (os->octet_ptr[i] == '\r') {
            buf[j] = '\r';
            if ((i + 1) >= os->length) {
                buf[++j] = '\0';
                goto done;
            }
            if ((os->octet_ptr[i+1] == '\n') ||
                (os->octet_ptr[i+1] == '\0')) {
                buf[++j] = os->octet_ptr[++i];
            } else {
                buf[++j] = '\0';
            }
        } else
        if ((os->octet_ptr[i] < ' ') ||
            (os->octet_ptr[i] > '~')) {
            free(buf);
            return NULL;
        } else {
            buf[j] = os->octet_ptr[i];
        }
    }

  done:
    if (j > 255) {
        j = 255;
        while ((j > 0) && (buf[j-1] == '\r')) {
            j--;
        }
    }
    octet_str = MakeOctetString(buf, j);
    free(buf);
    if (octet_str == NULL) {
        return NULL;
    }
    FreeOctetString(os);
    return octet_str;
}

/*
 * ConvertFromDisplayString
 *
 * Converts an OctetString from an NVT string to a normal string.  This
 * consists of converting carriage-return/linefeed pairs (\r\n) into a
 * single newline character (\n), and converting carriage-return/NULL
 * pairs (\r\0) into a single carriage return (\r).  If there are any
 * non-printable characters, the function returns 0, otherwise it returns 1.
 */
int
ConvertFromDisplayString(os)
    OctetString *os;
{
    int i, j;

    if (!IsDisplayString(os)) {
        return(0);
    }

    for (i=0,j=0; i<os->length; i++,j++) {
        if (os->octet_ptr[i] == '\r') {
            i++;
            if (os->octet_ptr[i] == '\n') {
                os->octet_ptr[j] = '\n';
            } else {
                os->octet_ptr[j] = '\r';
            }
        } else {
            os->octet_ptr[j] = os->octet_ptr[i];
        }
    }
    os->length = j;
    return(1);
}


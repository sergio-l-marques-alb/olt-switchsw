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

#include <string.h>

#include <malloc.h>

#include <stdlib.h>

#include <unistd.h>


#include <sys/types.h>



#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

/*
 *  OctetStringToOID()
  *
 *  Function:  Converts the specified OctetString to an OID.
 *
 *  Inputs:   os               - specifies the OctetString to convert.
 *            firstSIDIsLength - if non-zero, specifies the OctetString's
 *                               length should be the first SID of the OID.
 *                               If zero, specifies the OctetString's length
 *                               should be omitted.
 *
 *  Outputs:  On failure, returns NULL.  On success, returns an OID.
 *            The returned OID must be freed by the caller.
 */
OID *
OctetStringToOID(OctetString *os, int firstSIDIsLength)
{
    int  i, length, offset;
    OID  *result = NULL;

    /* --- Return NULL if no argument --- */
    if ((os == NULL) || (os->octet_ptr == NULL)) {
        return NULL;
    }

    /* --- Return NULL if the OctetString is too long --- */
    if (firstSIDIsLength != 0) {
        /* --- The first SID must be the length SID --- */
        if (os->length > (MAX_OID_SIZE - 1)) {
            return NULL;
        }
        length = os->length + 1;
        offset = 1;  /* Copy octets after the length SID */
    }
    else {
        /* --- The first SID is not the length SID --- */
        if (os->length > MAX_OID_SIZE) {
            return NULL;
        }
        length = os->length;
        offset = 0;  /* Copy octets in first SID */
    }
    /* --- Allocate a result OID --- */
    result = (OID *) malloc(sizeof(OID) + length * sizeof(SR_UINT32));
    if (result == NULL) {
        return NULL;
    }

    /* --- Populate the OID's length and oid pointer --- */
    result->length  = length;
    result->oid_ptr = (SR_UINT32 *) (((char *) result) + sizeof(OID));

    /* --- Populate the length SID --- */
    if (firstSIDIsLength != 0) {
        result->oid_ptr[0] = (SR_UINT32) length - 1;
    }

    /* --- Populate the remaining SIDs --- */
    for (i = 0; i < os->length; i++) {
        result->oid_ptr[i+offset] = (SR_UINT32) os->octet_ptr[i];
    }

    return result;
}   /* OctetStringToOID() */

/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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

#include "sr_snmp.h"

#ifndef SR_UNSECURABLE

#include "sr_keych.h"
#include "global.h"

#ifdef SR_MD5_HASH_ALGORITHM
#include "md5.h"
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
#include "sha.h"
#endif /* SR_SHA_HASH_ALGORITHM */

/*
 * SrGetKeyChange - Calculate the value of a KeyChange object.
 *
 * IMPORTANT NOTE:
 *
 * This function only supports certain combinations of protocol P and
 * hash algorithm H.  In particular, the key length K (defined by
 * protocol P) must be a fixed length, and that length is determined
 * by the length of the newKey parameter.
 *
 */

OctetString *
SrGetKeyChange(
    SR_INT32 algorithm,
    const OctetString *oldKey,
    const OctetString *newKey,
    SrRandomByteFunction sr_random_byte_function)
{
    unsigned char *nval, *rval;
    OctetString *result;
    SR_INT32 i, j, k, iter;

#ifdef SR_MD5_HASH_ALGORITHM
    if (algorithm == SR_KEYCHANGE_ALGORITHM_MD5) {
        MD5_CTX MD;
        unsigned char tval[MD5_DIGEST_SIZE];

        result = MakeOctetString(NULL, newKey->length * 2);
        if (result == NULL) {
            return NULL;
        }
        rval = result->octet_ptr;
        nval = &(result->octet_ptr[newKey->length]);

        for (i = 0; i < newKey->length; i++) {
            rval[i] = (*sr_random_byte_function)();
        }

        iter = (newKey->length - 1) / MD5_DIGEST_SIZE;

        for (i = 0; i < iter ; i++) {
            SrMD5Init(&MD);
            if (i == 0) {
                SrMD5Update(&MD, oldKey->octet_ptr,
                            (unsigned int)oldKey->length);
            } else {
                SrMD5Update(&MD, tval, MD5_DIGEST_SIZE);
            }
            SrMD5Update(&MD, rval, (unsigned int)newKey->length);
            SrMD5Final(tval, &MD);
            k = i * MD5_DIGEST_SIZE;
            for (j = 0; j < MD5_DIGEST_SIZE; j++) {
                nval[k+j] = tval[j] ^ newKey->octet_ptr[k+j];
            }
        }
        SrMD5Init(&MD);
        if (i == 0) {
            SrMD5Update(&MD, oldKey->octet_ptr, (unsigned int)oldKey->length);
        } else {
            SrMD5Update(&MD, tval, MD5_DIGEST_SIZE);
        }
        SrMD5Update(&MD, rval, (unsigned int)newKey->length);
        SrMD5Final(tval, &MD);
        k = i * MD5_DIGEST_SIZE;
        for (j = 0; j < (newKey->length - k); j++) {
            nval[k+j]  = tval[j] ^ newKey->octet_ptr[k+j];
        }

        return result;
    }
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
    if (algorithm == SR_KEYCHANGE_ALGORITHM_SHA) {
        SHA_CTX MD;
        unsigned char tval[SHA_DIGEST_SIZE];

        result = MakeOctetString(NULL, newKey->length * 2);
        if (result == NULL) {
            return NULL;
        }
        rval = result->octet_ptr;
        nval = &(result->octet_ptr[newKey->length]);

        for (i = 0; i < newKey->length; i++) {
            rval[i] = (*sr_random_byte_function)();
        }

        iter = (newKey->length - 1) / SHA_DIGEST_SIZE;

        for (i = 0; i < iter ; i++) {
            SHAInit(&MD);
            if (i == 0) {
                SHAUpdate(&MD, oldKey->octet_ptr, (unsigned int)oldKey->length);
            } else {
                SHAUpdate(&MD, tval, SHA_DIGEST_SIZE);
            }
            SHAUpdate(&MD, rval, (unsigned int)newKey->length);
            SHAFinal(tval, &MD);
            k = i * SHA_DIGEST_SIZE;
            for (j = 0; j < SHA_DIGEST_SIZE; j++) {
                nval[k+j] = tval[j] ^ newKey->octet_ptr[k+j];
            }
        }
        SHAInit(&MD);
        if (i == 0) {
            SHAUpdate(&MD, oldKey->octet_ptr, (unsigned int)oldKey->length);
        } else {
            SHAUpdate(&MD, tval, SHA_DIGEST_SIZE);
        }
        SHAUpdate(&MD, rval, (unsigned int)newKey->length);
        SHAFinal(tval, &MD);
        k = i * SHA_DIGEST_SIZE;
        for (j = 0; j < (newKey->length - k); j++) {
            nval[k+j]  = tval[j] ^ newKey->octet_ptr[k+j];
        }

        return result;
    }
#endif /* SR_SHA_HASH_ALGORITHM */

    return NULL;
}

#endif /* SR_UNSECURABLE */

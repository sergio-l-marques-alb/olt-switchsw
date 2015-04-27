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


#include <ctype.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>

#include "sr_type.h"
#include "sr_snmp.h"
#include "sr_auth.h"
#include "global.h"


#ifdef SR_MD5_HASH_ALGORITHM
#include "md5.h"
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
#include "sha.h"
#endif /* SR_SHA_HASH_ALGORITHM */

#define HMAC_B 64

void HmacHash(
    SR_INT32 algorithm,
    const unsigned char *secret,
    unsigned char *digest,
    const unsigned char *buf,
    SR_UINT32 len)
{
    unsigned char k1[HMAC_B];
    unsigned char k2[HMAC_B];
    int i;

#ifdef SR_MD5_HASH_ALGORITHM
    if (algorithm == SR_USM_HMAC_MD5_AUTH_PROTOCOL) {
        MD5_CTX MD;
        unsigned char tmp[MD5_DIGEST_SIZE];

        memset(k1 + MD5_DIGEST_SIZE, 0x36, HMAC_B - MD5_DIGEST_SIZE);
        memset(k2 + MD5_DIGEST_SIZE, 0x5c, HMAC_B - MD5_DIGEST_SIZE);

        for (i = 0; i < MD5_DIGEST_SIZE; i++) {
            k1[i] = secret[i] ^ (unsigned char)0x36;
            k2[i] = secret[i] ^ (unsigned char)0x5c;
        }

        /* Calculate inner digest */
        SrMD5Init(&MD);
        SrMD5Update(&MD, k1, HMAC_B);
        SrMD5Update(&MD, (unsigned char *)buf, (unsigned int)len);
        SrMD5Final(tmp, &MD);

        /* Calculate outer digest */
        SrMD5Init(&MD);
        SrMD5Update(&MD, k2, HMAC_B);
        SrMD5Update(&MD, tmp, MD5_DIGEST_SIZE);
        SrMD5Final(tmp, &MD);
        memcpy(digest, tmp, 12);
    }
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
    if (algorithm == SR_USM_HMAC_SHA_AUTH_PROTOCOL) {
        SHA_CTX MD;
        unsigned char tmp[SHA_DIGEST_SIZE];

        memset(k1 + SHA_DIGEST_SIZE, 0x36, HMAC_B - SHA_DIGEST_SIZE);
        memset(k2 + SHA_DIGEST_SIZE, 0x5c, HMAC_B - SHA_DIGEST_SIZE);

        for (i = 0; i < SHA_DIGEST_SIZE; i++) {
            k1[i] = secret[i] ^ (unsigned char)0x36;
            k2[i] = secret[i] ^ (unsigned char)0x5c;
        }

        /* Calculate inner digest */
        SHAInit(&MD);
        SHAUpdate(&MD, k1, HMAC_B);
        SHAUpdate(&MD, (unsigned char *)buf, len);
        SHAFinal(tmp, &MD);

        /* Calculate outer digest */
        SHAInit(&MD);
        SHAUpdate(&MD, k2, HMAC_B);
        SHAUpdate(&MD, tmp, SHA_DIGEST_SIZE);
        SHAFinal(tmp, &MD);
        memcpy(digest, tmp, 12);
    }
#endif /* SR_SHA_HASH_ALGORITHM */
}

#ifdef SR_MD5_HASH_ALGORITHM
#include "md5.c"
#endif /* SR_MD5_HASH_ALGORITHM */

#ifdef SR_SHA_HASH_ALGORITHM
#include "sha.c"
#endif /* SR_SHA_HASH_ALGORITHM */

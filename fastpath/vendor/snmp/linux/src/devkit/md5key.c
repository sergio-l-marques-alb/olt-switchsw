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
#include "sr_snmp.h"

#ifndef SR_UNSECURABLE

#include "global.h"
#include "md5.h"
#include "md5key.h"

#define MAINTKEY_BUF_SIZE 64
#define MAINTKEY_NUM_BUFFERS 16384

/* this routine produces a key using the MD5 hash algorithm on a password
 * which is repeated and extended to 1 Mbyte in size */
OctetString *
MakeMD5PasswordKey(password)
const OctetString *password;
{
    MD5_CTX         MD;
    unsigned char pw_buf[MAINTKEY_BUF_SIZE], *cp, *ce, *pe, *pp;
    long count;
    OctetString *retval;
    unsigned char buf[MD5_DIGEST_SIZE];
    SR_UINT32 length = password->length;
    unsigned char *octet_ptr = password->octet_ptr;

    /* set up to do MD5 */
    SrMD5Init(&MD);

    /* repeat for a 1Mbyte block */
    pp = octet_ptr;
    pe = pp + length;
    ce = pw_buf + sizeof(pw_buf);
    for (count = 0; count < MAINTKEY_NUM_BUFFERS; count++) {
	/* keep repeating the password */
	for(cp = pw_buf; cp < ce;) {
	    *cp++ = *pp++;
            if (pp >= pe) {
                pp = octet_ptr;
            }
	}

	/* update the digest */
        SrMD5Update(&MD, pw_buf, sizeof(pw_buf));
    }

    /* finish calculating */
    SrMD5Update(&MD, pw_buf, 0);
    SrMD5Final(buf, &MD);

    /* return a pointer to the key */
    retval = MakeOctetString(buf, (long) sizeof(buf));
    return(retval);
}

OctetString *
MD5LocalizeKey(
    const OctetString *snmpEngineID,
    const OctetString *key)
{
    MD5_CTX         MD;
    OctetString *retval;
    unsigned char buf[MD5_DIGEST_SIZE];

    SrMD5Init(&MD);
    SrMD5Update(&MD, key->octet_ptr, (unsigned int)key->length);
    SrMD5Update(&MD, snmpEngineID->octet_ptr, (unsigned int)snmpEngineID->length);
    SrMD5Update(&MD, key->octet_ptr, (unsigned int)key->length);
    SrMD5Final(buf, &MD);

    /* return a pointer to the key */
    retval = MakeOctetString(buf, (long) sizeof(buf));
    return(retval);
}
#endif /* SR_UNSECURABLE */

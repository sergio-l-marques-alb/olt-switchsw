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

#ifdef HAVE_SYS_TYPES
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES */


#include <string.h>


#include <stdlib.h>

#include "sr_snmp.h"

#ifndef SR_UNSECURABLE
#if (defined(SR_MD5_HASH_ALGORITHM) || defined(SR_SHA_HASH_ALGORITHM))

#include "sr_keych.h"

#ifdef SR_MD5_HASH_ALGORITHM
#ifdef SR_SHA_HASH_ALGORITHM
#define HASH_OPTIONS "md5|sha"
#else /* SR_SHA_HASH_ALGORITHM */
#define HASH_OPTIONS "md5"
#endif /* SR_SHA_HASH_ALGORITHM */
#else /* SR_MD5_HASH_ALGORITHM */
#define HASH_OPTIONS "sha"
#endif /* SR_MD5_HASH_ALGORITHM */


unsigned char
get_random_byte(void)
{
    static int      initialized = 0;
    static time_t   last_used, next_val;

    if (!initialized) {
        last_used = time(NULL);
        next_val = last_used * time(NULL) * 30 / 17;
        initialized = 1;
    }

    next_val = (next_val + time(NULL)) * time(NULL) * 30 / 17;
    if (next_val == last_used) {
        last_used = time(NULL);
        next_val = last_used * time(NULL) * 30 / 17;
    }
    return ((unsigned char)(next_val % 0xff));
}



int
main(
    int argc,
    char *argv[])
{
    OctetString *os1, *os2, *result;
    char  keybuf[128];
    int i, algorithm, keysize;

    if (argc != 2) {
        goto usage;
    }

#ifdef SR_MD5_HASH_ALGORITHM
    if (strcmp(argv[1], "md5") == 0) {
        algorithm = SR_KEYCHANGE_ALGORITHM_MD5;
    } else
    if (strcmp(argv[1], "MD5") == 0) {
        algorithm = SR_KEYCHANGE_ALGORITHM_MD5;
    } else
#endif /* SR_MD5_HASH_ALGORITHM */
#ifdef SR_SHA_HASH_ALGORITHM
    if (strcmp(argv[1], "sha") == 0) {
        algorithm = SR_KEYCHANGE_ALGORITHM_SHA;
    } else
    if (strcmp(argv[1], "SHA") == 0) {
        algorithm = SR_KEYCHANGE_ALGORITHM_SHA;
    } else
#endif /* SR_SHA_HASH_ALGORITHM */
    {
        goto usage;
    }

    printf("Enter the old localized key.\n");
    fgets(keybuf, sizeof(keybuf), stdin);
    /*
     * Remove newline character
     */
    keysize = strlen( keybuf );
    keysize--;
    if( keybuf[keysize] == '\n' ) {
      keybuf[keysize] = '\0';
    }
    os1 = MakeOctetStringFromHex(keybuf);
    if (os1 == NULL) {
        printf("error converting <oldkey> to an octet string.\n");
        return 0;
    }

    printf("Enter the new localized key.\n");
    fgets(keybuf, sizeof(keybuf), stdin);
    /*
     * Remove newline character
     */
    keysize = strlen( keybuf );
    keysize--;
    if( keybuf[keysize] == '\n' ) {
      keybuf[keysize] = '\0';
    }
    os2 = MakeOctetStringFromHex(keybuf);
    if (os2 == NULL) {
        FreeOctetString(os1);
        printf("error converting <newkey> to an octet string.\n");
        return 0;
    }

    result = SrGetKeyChange(algorithm, os1, os2, get_random_byte);
    FreeOctetString(os1);
    FreeOctetString(os2);
    if (result == NULL) {
        printf("malloc error\n");
    }

    printf("\n\nThe key is:\n\n     ");
    for (i = 0; i < result->length - 1; i++) {
        printf("%02x ", (unsigned int)result->octet_ptr[i]);
    }
    printf("%02x", (unsigned int)result->octet_ptr[i]);
    printf("\n\n");

    return 0;

  usage:
    printf("usage:  %s " HASH_OPTIONS "\n", argv[0]);
    return 0;
}

#endif	/* (defined(SR_MD5_HASH_ALGORITHM) || defined(SR_SHA_HASH_ALGORITHM)) */
#endif /* SR_UNSECURABLE */

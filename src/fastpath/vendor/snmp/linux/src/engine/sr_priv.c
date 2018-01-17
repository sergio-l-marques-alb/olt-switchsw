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


#include <stddef.h>

#include "sr_snmp.h"
#include "sr_type.h"
#include "sr_priv.h"
#include "des.h"
#ifdef SR_AES
#include "aes.h"
#include "sr_aes.h"
#endif /* SR_AES */
#include "diag.h"
SR_FILENAME

#ifdef SR_AES
void 
AES_cfb_Encrypt( 
    const unsigned char *key,
    const unsigned char *iv,
    unsigned char *buf,
    SR_UINT32 len,
    SR_INT32 protocol)
{
    SR_UINT32 rk[RK_MAXSIZE]; /* = 4*(MAX_Nr+1) */
    unsigned char IV[AES_BLOCK_SIZE];
    unsigned char block[AES_BLOCK_SIZE];
    unsigned char *desptr;
    SR_UINT32 remainder;
    int i, round, key_bit;

    if (protocol == SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
	round = 10;
	key_bit = 128;
    }
    else if (protocol == SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
	round = 12;
	key_bit = 192;
    }
    else if (protocol == SR_USM_AES_CFB_256_PRIV_PROTOCOL) {
	round = 14;
	key_bit = 256;
    }
    else {
	DPRINTF((APWARN, "Unsupported AES protocol\n"));
	round = 10;
	key_bit = 128;
    }

    memset(&rk, 0, sizeof(rk));
    memset(&IV, 0, sizeof(IV));
    memset(&block, 0, sizeof(block));
    memcpy(IV, iv, AES_BLOCK_SIZE);
    desptr = buf;

    AES_KeySetupEnc(rk, key, key_bit);
    AES_Encrypt(rk, round, IV, block); 
    for (i = 0; i < len; i++) {
	IV[i%AES_BLOCK_SIZE] = buf[i] ^ block[i%AES_BLOCK_SIZE];
	if ((i+1)%AES_BLOCK_SIZE == 0) {
	    memcpy(desptr, IV, AES_BLOCK_SIZE);
	    desptr += AES_BLOCK_SIZE;
	    AES_Encrypt(rk, round, IV, block);
	}
    }
    remainder = len%AES_BLOCK_SIZE;
    if (remainder) {
       memcpy(desptr, IV, remainder);
    }

    return;
}
  
void
AES_cfb_Decrypt(
    const unsigned char *key,
    const unsigned char *iv,
    unsigned char *buf,
    SR_UINT32 len,
    SR_INT32 protocol)
{
    SR_UINT32 rk[RK_MAXSIZE]; /* = 4*(MAX_Nr+1) */
    unsigned char IV[AES_BLOCK_SIZE];
    unsigned char block[AES_BLOCK_SIZE];
    int i, round, key_bit; 

    if (protocol == SR_USM_AES_CFB_128_PRIV_PROTOCOL) {
	round = 10;
	key_bit = 128;
    }
    else if (protocol == SR_USM_AES_CFB_192_PRIV_PROTOCOL) {
	round = 12;
	key_bit = 192;
    }
    else if (protocol == SR_USM_AES_CFB_256_PRIV_PROTOCOL) {
	round = 14;
	key_bit = 256;
    }
    else {
	DPRINTF((APWARN, "Unsupported AES protocol\n"));
	round = 10;
	key_bit = 128;
    }

    memset(&rk, 0, sizeof(rk));
    memset(&IV, 0, sizeof(IV));
    memset(&block, 0, sizeof(block));
    memcpy(IV, iv, AES_BLOCK_SIZE);
    AES_KeySetupEnc(rk, key, key_bit);
    AES_Encrypt(rk, round, IV, block); 

    for (i = 0; i < len; i++) {
	IV[i%AES_BLOCK_SIZE] = buf[i];
	buf[i] = buf[i] ^ block[i%AES_BLOCK_SIZE];
	if ((i+1)%AES_BLOCK_SIZE == 0) {
	    AES_Encrypt(rk, round, IV, block);
	}
    }
}
#endif /* SR_AES */
  
void DesDecrypt(
    const unsigned char *secret,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len)
{
    unsigned char key[8];
    unsigned char iv[8];
    unsigned char *desptr, *cp, *cp1; 
    int           cnt, i;
    unsigned char            ivtmp[8];

    memcpy(key, (char *) secret, 8);
    memcpy(iv, (char *) secret + 8, 8);
    if (salt != NULL) {
        iv[0] ^= salt[0];
        iv[1] ^= salt[1];
        iv[2] ^= salt[2];
        iv[3] ^= salt[3];
        iv[4] ^= salt[4];
        iv[5] ^= salt[5];
        iv[6] ^= salt[6];
        iv[7] ^= salt[7];
    }
    dessetkey((char *) key);

    desptr = buf;
    for (cnt = len; cnt > 0; cnt -= 8) {
        (void) memcpy(ivtmp, desptr, 8);
        dedes((char *) desptr);
        cp = desptr;
        cp1 = iv;
        for (i = 8; i != 0; i--) {
            *cp++ ^= *cp1++;
        } /* for i */
        (void) memcpy(iv, ivtmp, 8);
        desptr += 8;
    }

    return;
}

#ifdef SR_3DES
void DesEDE_Decrypt(
    const unsigned char *secret,
    const unsigned char *secret1,
    const unsigned char *secret2,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len)
{
    unsigned char key1[8], key2[8], key3[8];
    unsigned char iv[8];
    unsigned char *desptr, *cp, *cp1; 
    int           cnt, i;
    unsigned char            ivtmp[8];

    memcpy(key1, (char *) secret, 8);
    memcpy(key2, (char *) secret1, 8);
    memcpy(key3, (char *) secret2, 8);
    memcpy(iv, (char *) secret + 8, 8);
    if (salt != NULL) {
        iv[0] ^= salt[0];
        iv[1] ^= salt[1];
        iv[2] ^= salt[2];
        iv[3] ^= salt[3];
        iv[4] ^= salt[4];
        iv[5] ^= salt[5];
        iv[6] ^= salt[6];
        iv[7] ^= salt[7];
    }

    desptr = buf;
    for (cnt = len; cnt > 0; cnt -= 8) {
        (void) memcpy(ivtmp, desptr, 8);
        dessetkey((char *) key3);
        dedes((char *) desptr);
        dessetkey((char *) key2);
        endes((char *) desptr);
        dessetkey((char *) key1);
        dedes((char *) desptr);
        cp = desptr;
        cp1 = iv;
        for (i = 8; i != 0; i--) {
            *cp++ ^= *cp1++;
        } /* for i */
        (void) memcpy(iv, ivtmp, 8);
        desptr += 8;
    }
}
#endif /* SR_3DES */

void DesEncrypt(
    const unsigned char *secret,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len)
{
    unsigned char key[8];
    unsigned char iv[8];
    unsigned char *desptr, *cp, *cp1; 
    int           cnt, i;

    memcpy(key, (char *) secret, 8);
    memcpy(iv, (char *) secret + 8, 8);
    if (salt != NULL) {
        iv[0] ^= salt[0];
        iv[1] ^= salt[1];
        iv[2] ^= salt[2];
        iv[3] ^= salt[3];
        iv[4] ^= salt[4];
        iv[5] ^= salt[5];
        iv[6] ^= salt[6];
        iv[7] ^= salt[7];
    }
    dessetkey((char *) key);

    desptr = buf;
    for (cnt = len; cnt > 0; cnt -= 8) {
        cp = desptr;
        cp1 = iv;
        for (i = 8; i != 0; i--) {
            *cp++ ^= *cp1++;
        }
        endes((char *) desptr);
        memcpy(iv, desptr, 8);
        desptr += 8;
    }
}

#ifdef SR_3DES
void DesEDE_Encrypt(
    const unsigned char *secret,
    const unsigned char *secret1,
    const unsigned char *secret2,
    const unsigned char *salt,
    unsigned char *buf,
    SR_UINT32 len)
{
    unsigned char key1[8], key2[8], key3[8];
    unsigned char iv[8];
    unsigned char *desptr, *cp, *cp1; 
    int           cnt, i;

    memcpy(key1, (char *) secret, 8);
    memcpy(key2, (char *) secret1, 8);
    memcpy(key3, (char *) secret2, 8);
    memcpy(iv, (char *) secret + 8, 8);
    if (salt != NULL) {
        iv[0] ^= salt[0];
        iv[1] ^= salt[1];
        iv[2] ^= salt[2];
        iv[3] ^= salt[3];
        iv[4] ^= salt[4];
        iv[5] ^= salt[5];
        iv[6] ^= salt[6];
        iv[7] ^= salt[7];
    }
    desptr = buf;
    for (cnt = len; cnt > 0; cnt -= 8) {
        cp = desptr;
        cp1 = iv;
        for (i = 8; i != 0; i--) {
            *cp++ ^= *cp1++;
        }
        dessetkey((char *) key1);
        endes((char *) desptr);
        dessetkey((char *) key2);
        dedes((char *) desptr);
        dessetkey((char *) key3);
        endes((char *) desptr);
        memcpy(iv, desptr, 8);
        desptr += 8;
    }
}
#endif /* SR_3DES */

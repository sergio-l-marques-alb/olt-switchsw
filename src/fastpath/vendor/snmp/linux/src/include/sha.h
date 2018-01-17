/*
 * SHA.H - header file for SHAC.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#ifndef SR_SHA_H
#define SR_SHA_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SHA_DIGEST_SIZE
#define SHA_DIGEST_SIZE 20
#endif /* SHA_DIGEST_SIZE */

/* SHA context. */
typedef struct {
    UINT4           state[5];	/* state (ABCD) */
    UINT4           count[2];	/* number of bits, modulo 2^64 (lsb first) */
    unsigned char   buffer[80];	/* input buffer */
}               SHA_CTX;

void SHAInit(SHA_CTX *);

void SHAUpdate(
    SHA_CTX *,
    unsigned char *,
    unsigned int);

void SHAFinal(
    unsigned char *,
    SHA_CTX *);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif /* SR_SHA_H */

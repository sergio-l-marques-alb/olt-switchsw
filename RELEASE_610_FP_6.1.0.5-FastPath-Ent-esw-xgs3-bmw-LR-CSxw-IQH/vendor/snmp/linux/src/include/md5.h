/*
 * MD5.H - header file for MD5C.C
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

#ifndef SR_MD5_H
#define SR_MD5_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef MD5_DIGEST_SIZE
#define MD5_DIGEST_SIZE 16
#endif /* MD5_DIGEST_SIZE */

/* MD5 context. */
typedef struct {
    UINT4           state[4];	/* state (ABCD) */
    UINT4           count[2];	/* number of bits, modulo 2^64 (lsb first) */
    unsigned char   buffer[64];	/* input buffer */
}               MD5_CTX;

/* formerly MD5Init */
void SrMD5Init(MD5_CTX *);

/* formerly MD5Update */
void SrMD5Update(
    MD5_CTX *,
    unsigned char *,
    unsigned int);

/* formerly MD5Final */
void SrMD5Final(
    unsigned char[16],
    MD5_CTX *);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif /* SR_MD5_H */

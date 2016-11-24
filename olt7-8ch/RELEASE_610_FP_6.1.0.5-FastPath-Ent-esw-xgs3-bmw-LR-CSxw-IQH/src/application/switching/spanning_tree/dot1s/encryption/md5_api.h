/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  md5_api.h
*
* @purpose   MD5 Implementation API file
*
* @component Encryption
*
* @comments  from RFC 1321
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/

/* MD5.H - header file for MD5C.C
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.
These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#ifndef MD5_API_H
#define MD5_API_H

#include "l7_common.h"

/* MD5 context. */
typedef struct {
  L7_uint32 state[4];                                   /* state (ABCD) */
  L7_uint32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  L7_uchar8 buffer[64];                         /* input buffer */
} L7_MD5_CTX_t;

extern void L7_MD5Update (L7_MD5_CTX_t *context, L7_uchar8 *input, L7_uint32 inputLen);
extern void L7_MD5Final (L7_uchar8 *digest, L7_MD5_CTX_t *context);
extern void L7_MD5Init (L7_MD5_CTX_t *context);

#endif /*MD5_API_H*/

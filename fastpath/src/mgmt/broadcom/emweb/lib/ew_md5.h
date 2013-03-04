/*
 * Product:  EmWeb
 * Release:  R6_2_0
 *
 * Agranat Systems, Inc.
 * 1345 Main Street
 * Waltham, MA  02154
 *
 * (617) 893-7868
 * sales@agranat.com, support@agranat.com
 *
 * http://www.agranat.com/
 *
 * EmWeb MD5
 *
 */

#ifndef _EW_MD5_H
#define _EW_MD5_H

#include "ew_types.h"

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

/* MD5 context. */
typedef struct {
  uint32 state[4];                                   /* state (ABCD) */
  uint32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8 buffer[64];                         /* input buffer */
} MD5_CTX;

void MD5Init (MD5_CTX *);
void MD5Update (MD5_CTX *, uint8 *, uintf);
void MD5Final (uint8 [16], MD5_CTX *);

#endif /* _EW_MD5_H */

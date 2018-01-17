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

/* This file is derived from MD5C.C: */
/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
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

#include "sr_conf.h"

#include "sr_type.h"
#include "sr_proto.h"
#include "sri/sr_endia.h"

#ifndef SR_UNSECURABLE
#include "global.h"
#include "sha.h"



#include <string.h>

/* Constants for SHATransform routine.
 */

#define SL01(X) (((X) << 1) | ((X) >> 31))
#define SL05(X) (((X) << 5) | ((X) >> 27))
#define SL30(X) (((X) << 30) | ((X) >> 2))

#define SW(W,t) \
    ((W)[t] = (SL01((W)[(t)-3]^(W)[(t)-8]^(W)[(t)-14]^(W)[(t)-16])))

#define K00 0x5a827999
#define K20 0x6ed9eba1
#define K40 0x8f1bbcdc
#define K60 0xca62c1d6

static unsigned char SHA_PADDING[64] = {
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F1, F2, F3 and F4 are basic SHA functions.
 */
#define F1(B,C,D) (((B)&(C))|((~(B))&(D)))
#define F2(B,C,D) ((B)^(C)^(D))
#define F3(B,C,D) (((B)&(C))|((B)&(D))|((C)&(D)))
#define F4(B,C,D) ((B)^(C)^(D))

/* FF1, FF2, FF3, and FF4 transformations for rounds 1, 2, 3, and 4.
 */
#define FF1(A,B,C,D,E,W,TEMP) \
    (TEMP) = (SL05(A)) + (F1((B),(C),(D))) + (E) + (W) + (K00); \
    (E) = (D); \
    (D) = (C); \
    (C) = (SL30(B)); \
    (B) = (A); \
    (A) = (TEMP)
#define FF2(A,B,C,D,E,W,TEMP) \
    (TEMP) = (SL05(A)) + (F2((B),(C),(D))) + (E) + (W) + (K20); \
    (E) = (D); \
    (D) = (C); \
    (C) = (SL30(B)); \
    (B) = (A); \
    (A) = (TEMP)
#define FF3(A,B,C,D,E,W,TEMP) \
    (TEMP) = (SL05(A)) + (F3((B),(C),(D))) + (E) + (W) + (K40); \
    (E) = (D); \
    (D) = (C); \
    (C) = (SL30(B)); \
    (B) = (A); \
    (A) = (TEMP)
#define FF4(A,B,C,D,E,W,TEMP) \
    (TEMP) = (SL05(A)) + (F4((B),(C),(D))) + (E) + (W) + (K60); \
    (E) = (D); \
    (D) = (C); \
    (C) = (SL30(B)); \
    (B) = (A); \
    (A) = (TEMP)

static void SHATransform
    SR_PROTOTYPE((UINT4[5],
		  unsigned char[64]));

/* SHA initialization. Begins an SHA operation, writing a new context.
 */
void 
SHAInit(context)
    SHA_CTX        *context;	/* context */
{
    context->count[0] = context->count[1] = 0;
    /*
     * Load magic initialization constants.
     */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
    context->state[4] = 0xc3d2e1f0;
}

/* SHA block update operation. Continues an SHA message-digest
   operation, processing another message block, and updating the
   context.
   */
void 
SHAUpdate(context, input, inputLen)
    SHA_CTX        *context;	/* context */
    unsigned char  *input;	/* input block */
    unsigned int    inputLen;	/* length of input block */
{
    unsigned int    i, index, partLen;

    /* Compute number of bytes mod 64 */
    index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((context->count[0] += ((UINT4) inputLen << 3))
	< ((UINT4) inputLen << 3))
	context->count[1]++;
    context->count[1] += ((UINT4) inputLen >> 29);

    partLen = 64 - index;

    /*
     * Transform as many times as possible.
     */
    if (inputLen >= partLen) {
	memcpy((POINTER) & context->buffer[index], (POINTER) input, partLen);
	SHATransform(context->state, context->buffer);

	for (i = partLen; i + 63 < inputLen; i += 64)
	    SHATransform(context->state, &input[i]);

	index = 0;
    }
    else
	i = 0;

    /* Buffer remaining input */
    memcpy((POINTER) &context->buffer[index], (POINTER) &input[i],
	   inputLen - i);
}

/* SHA finalization. Ends an SHA message-digest operation, writing the
   the message digest and zeroizing the context.
   */
void 
SHAFinal(digest, context)
    unsigned char  *digest;	/* message digest */
    SHA_CTX        *context;	/* context */
{
    unsigned char   bits[8];
    unsigned int    index, padLen;

    /* Save number of bits */
    bits[7] = (unsigned char) (context->count[0] & 0xff);
    bits[6] = (unsigned char) ((context->count[0] >> 8) & 0xff);
    bits[5] = (unsigned char) ((context->count[0] >> 16) & 0xff);
    bits[4] = (unsigned char) ((context->count[0] >> 24) & 0xff);
    bits[3] = (unsigned char) (context->count[1] & 0xff);
    bits[2] = (unsigned char) ((context->count[1] >> 8) & 0xff);
    bits[1] = (unsigned char) ((context->count[1] >> 16) & 0xff);
    bits[0] = (unsigned char) ((context->count[1] >> 24) & 0xff);

    /*
     * Pad out to 56 mod 64.
     */
    index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    SHAUpdate(context, SHA_PADDING, padLen);

    /* Append length (before padding) */
    SHAUpdate(context, bits, 8);

    /* Store state in digest */
    digest[0] = (unsigned char) ((context->state[0] >> 24) & 0xff);
    digest[1] = (unsigned char) ((context->state[0] >> 16) & 0xff);
    digest[2] = (unsigned char) ((context->state[0] >> 8) & 0xff);
    digest[3] = (unsigned char) (context->state[0] & 0xff);
    digest[4] = (unsigned char) ((context->state[1] >> 24) & 0xff);
    digest[5] = (unsigned char) ((context->state[1] >> 16) & 0xff);
    digest[6] = (unsigned char) ((context->state[1] >> 8) & 0xff);
    digest[7] = (unsigned char) (context->state[1] & 0xff);
    digest[8] = (unsigned char) ((context->state[2] >> 24) & 0xff);
    digest[9] = (unsigned char) ((context->state[2] >> 16) & 0xff);
    digest[10] = (unsigned char) ((context->state[2] >> 8) & 0xff);
    digest[11] = (unsigned char) (context->state[2] & 0xff);
    digest[12] = (unsigned char) ((context->state[3] >> 24) & 0xff);
    digest[13] = (unsigned char) ((context->state[3] >> 16) & 0xff);
    digest[14] = (unsigned char) ((context->state[3] >> 8) & 0xff);
    digest[15] = (unsigned char) (context->state[3] & 0xff);
    digest[16] = (unsigned char) ((context->state[4] >> 24) & 0xff);
    digest[17] = (unsigned char) ((context->state[4] >> 16) & 0xff);
    digest[18] = (unsigned char) ((context->state[4] >> 8) & 0xff);
    digest[19] = (unsigned char) (context->state[4] & 0xff);

    /*
     * Zeroize sensitive information.
     */
    memset((POINTER) context, 0, sizeof(*context));
}

/* SHA basic transformation. Transforms state based on block.
 */
static void 
SHATransform(state, block)
    UINT4           state[5];
    unsigned char   block[64];
{
    UINT4 a, b, c, d, e;
    UINT4 W[80];
    UINT4 tmp;

    if (srEndian() == SR_BIG_ENDIAN) {
        memcpy(W, block, 64);
    }
    else {
        int i;
        unsigned char *o = (unsigned char *)W;
        for (i = 0; i < 64; i += 4) {
            o[i]     = block[i + 3];
            o[i + 1] = block[i + 2];
            o[i + 2] = block[i + 1];
            o[i + 3] = block[i];
        }
    }

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    FF1(a, b, c, d, e, W[0], tmp);
    FF1(a, b, c, d, e, W[1], tmp);
    FF1(a, b, c, d, e, W[2], tmp);
    FF1(a, b, c, d, e, W[3], tmp);
    FF1(a, b, c, d, e, W[4], tmp);
    FF1(a, b, c, d, e, W[5], tmp);
    FF1(a, b, c, d, e, W[6], tmp);
    FF1(a, b, c, d, e, W[7], tmp);
    FF1(a, b, c, d, e, W[8], tmp);
    FF1(a, b, c, d, e, W[9], tmp);
    FF1(a, b, c, d, e, W[10], tmp);
    FF1(a, b, c, d, e, W[11], tmp);
    FF1(a, b, c, d, e, W[12], tmp);
    FF1(a, b, c, d, e, W[13], tmp);
    FF1(a, b, c, d, e, W[14], tmp);
    FF1(a, b, c, d, e, W[15], tmp);
    FF1(a, b, c, d, e, SW(W,16), tmp);
    FF1(a, b, c, d, e, SW(W,17), tmp);
    FF1(a, b, c, d, e, SW(W,18), tmp);
    FF1(a, b, c, d, e, SW(W,19), tmp);

    FF2(a, b, c, d, e, SW(W,20), tmp);
    FF2(a, b, c, d, e, SW(W,21), tmp);
    FF2(a, b, c, d, e, SW(W,22), tmp);
    FF2(a, b, c, d, e, SW(W,23), tmp);
    FF2(a, b, c, d, e, SW(W,24), tmp);
    FF2(a, b, c, d, e, SW(W,25), tmp);
    FF2(a, b, c, d, e, SW(W,26), tmp);
    FF2(a, b, c, d, e, SW(W,27), tmp);
    FF2(a, b, c, d, e, SW(W,28), tmp);
    FF2(a, b, c, d, e, SW(W,29), tmp);
    FF2(a, b, c, d, e, SW(W,30), tmp);
    FF2(a, b, c, d, e, SW(W,31), tmp);
    FF2(a, b, c, d, e, SW(W,32), tmp);
    FF2(a, b, c, d, e, SW(W,33), tmp);
    FF2(a, b, c, d, e, SW(W,34), tmp);
    FF2(a, b, c, d, e, SW(W,35), tmp);
    FF2(a, b, c, d, e, SW(W,36), tmp);
    FF2(a, b, c, d, e, SW(W,37), tmp);
    FF2(a, b, c, d, e, SW(W,38), tmp);
    FF2(a, b, c, d, e, SW(W,39), tmp);

    FF3(a, b, c, d, e, SW(W,40), tmp);
    FF3(a, b, c, d, e, SW(W,41), tmp);
    FF3(a, b, c, d, e, SW(W,42), tmp);
    FF3(a, b, c, d, e, SW(W,43), tmp);
    FF3(a, b, c, d, e, SW(W,44), tmp);
    FF3(a, b, c, d, e, SW(W,45), tmp);
    FF3(a, b, c, d, e, SW(W,46), tmp);
    FF3(a, b, c, d, e, SW(W,47), tmp);
    FF3(a, b, c, d, e, SW(W,48), tmp);
    FF3(a, b, c, d, e, SW(W,49), tmp);
    FF3(a, b, c, d, e, SW(W,50), tmp);
    FF3(a, b, c, d, e, SW(W,51), tmp);
    FF3(a, b, c, d, e, SW(W,52), tmp);
    FF3(a, b, c, d, e, SW(W,53), tmp);
    FF3(a, b, c, d, e, SW(W,54), tmp);
    FF3(a, b, c, d, e, SW(W,55), tmp);
    FF3(a, b, c, d, e, SW(W,56), tmp);
    FF3(a, b, c, d, e, SW(W,57), tmp);
    FF3(a, b, c, d, e, SW(W,58), tmp);
    FF3(a, b, c, d, e, SW(W,59), tmp);

    FF4(a, b, c, d, e, SW(W,60), tmp);
    FF4(a, b, c, d, e, SW(W,61), tmp);
    FF4(a, b, c, d, e, SW(W,62), tmp);
    FF4(a, b, c, d, e, SW(W,63), tmp);
    FF4(a, b, c, d, e, SW(W,64), tmp);
    FF4(a, b, c, d, e, SW(W,65), tmp);
    FF4(a, b, c, d, e, SW(W,66), tmp);
    FF4(a, b, c, d, e, SW(W,67), tmp);
    FF4(a, b, c, d, e, SW(W,68), tmp);
    FF4(a, b, c, d, e, SW(W,69), tmp);
    FF4(a, b, c, d, e, SW(W,70), tmp);
    FF4(a, b, c, d, e, SW(W,71), tmp);
    FF4(a, b, c, d, e, SW(W,72), tmp);
    FF4(a, b, c, d, e, SW(W,73), tmp);
    FF4(a, b, c, d, e, SW(W,74), tmp);
    FF4(a, b, c, d, e, SW(W,75), tmp);
    FF4(a, b, c, d, e, SW(W,76), tmp);
    FF4(a, b, c, d, e, SW(W,77), tmp);
    FF4(a, b, c, d, e, SW(W,78), tmp);
    FF4(a, b, c, d, e, SW(W,79), tmp);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    /*
     * Zeroize sensitive information.
     */
    memset((POINTER) W, 0, sizeof(W));
}

#endif /* SR_UNSECURABLE */

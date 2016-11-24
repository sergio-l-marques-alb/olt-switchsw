/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename md5_api.h
*
* @purpose MD5 Implementation API file
*
* @component MD5 (utility)
*
* @comments Code taken from RFC 1321
*
* @create 9/19/2002
*
* @author skalyanam
* @author gpaussa
*
* @end
*             
**********************************************************************/


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
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#ifndef MD5_API_H
#define MD5_API_H

#include "l7_common.h"

/* MD5 context. */
typedef struct {
  L7_uint32 state[4];         /* state (ABCD) */
  L7_uint32 count[2];         /* number of bits, modulo 2^64 (lsb first) */
  L7_uchar8 buffer[64];       /* input buffer */
} L7_MD5_CTX_t;

/* NOTE: In the preceding, 'lsb first' actually means low-order word first.
 *       Each count[] word is stored in host byte order.
 */

/*********************************************************************
* @purpose  Begin a new message digest operation by initializing the context.
*
* @param    *context    @{(input)} Pointer to standard MD5 operating context
*
* @returns  void
*
* @comments Must be called once at the beginning of each new digest 
*           computation.
*
* @end
*********************************************************************/
void l7_md5_init(L7_MD5_CTX_t *context);

/*********************************************************************
* @purpose  Process a message block and update context with latest digest.
*
* @param    *context    @{(input)} Pointer to standard MD5 operating context
* @param    *input      @{(input)} Pointer to message block character array
* @param    inputLen    @{(input)} Message block length, in bytes
*
* @returns  void
*
* @comments May be called repeatedly to process additional parts of the
*           overall message.  The computed message digest is updated 
*           with each invocation of this function.
*
* @end
*********************************************************************/
void l7_md5_update(L7_MD5_CTX_t *context, L7_uchar8 *input, L7_uint32 inputLen);

/*********************************************************************
* @purpose  Finish the MD5 message-digest calculation and output the result.
*
* @param    *digest     @{(output)} Pointer to message digest output location
* @param    *context    @{(input)}  Pointer to standard MD5 operating context
*
* @returns  void
*
* @comments Must be called after the last MD5 Update operation.
*
* @comments The digest output parameter is intentionally listed first to 
*           match the order specified by RFC 1321.
*
* @end
*********************************************************************/
void l7_md5_final(L7_uchar8 *digest, L7_MD5_CTX_t *context);

#endif /*MD5_API_H*/


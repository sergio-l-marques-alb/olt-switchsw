/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  hmac.c
*
* @purpose 	 HMAC with MD5
*
* @component Encryption
*
* @comments  from RFC 2104
*
* @create    9/19/2002
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#include "md5_api.h"
#include "hmac_api.h"
#include "string.h"

void L7_hmac_md5(L7_uchar8	*text, 
			  L7_uint32 text_len, 
			  L7_uchar8* key, 
			  L7_uint32 key_len, 
			  L7_uchar8 *digest)
{
        L7_MD5_CTX_t context;
        L7_uchar8 k_ipad[65];    /* inner padding -
                                  * key XORd with ipad
                                  */
        L7_uchar8 k_opad[65];    /* outer padding -
                                  * key XORd with opad
                                  */
        L7_uchar8 tk[16];
        L7_uint32 i;
        /* if key is longer than 64 bytes reset it to key=MD5(key) */
        if (key_len > 64) {

                L7_MD5_CTX_t      tctx;

                L7_MD5Init(&tctx);
                L7_MD5Update(&tctx, key, key_len);
                L7_MD5Final(tk, &tctx);

                key = tk;
                key_len = 16;
        }

        /*
         * the HMAC_MD5 transform looks like:
         *
         * MD5(K XOR opad, MD5(K XOR ipad, text))
         *
         * where K is an n byte key
         * ipad is the byte 0x36 repeated 64 times
         * opad is the byte 0x5c repeated 64 times
         * and text is the data being protected
         */

        /* start out by storing key in pads */
        bzero( (L7_char8 *)k_ipad, (L7_int32)sizeof k_ipad);
        bzero( (L7_char8 *)k_opad, (L7_int32)sizeof k_opad);
        bcopy( (L7_char8 *)key, (L7_char8 *)k_ipad, (L7_int32)key_len);
        bcopy( (L7_char8 *)key, (L7_char8 *)k_opad, (L7_int32)key_len);

        /* XOR key with ipad and opad values */
        for (i=0; i<64; i++) {
                k_ipad[i] ^= 0x36;
                k_opad[i] ^= 0x5c;
        }
        /*
         * perform inner MD5
         */
        L7_MD5Init(&context);                   /* init context for 1st
                                                 * pass 
												 */
        L7_MD5Update(&context, k_ipad, 64);     /* start with inner pad */
        L7_MD5Update(&context, text, text_len); /* then text of datagram */
        L7_MD5Final(digest, &context);          /* finish up 1st pass */
        /*
         * perform outer MD5
         */
        L7_MD5Init(&context);                   /* init context for 2nd
                                                 * pass 
												 */
        L7_MD5Update(&context, k_opad, 64);     /* start with outer pad */
        L7_MD5Update(&context, digest, 16);     /* then results of 1st
                                                 * hash 
												 */
        L7_MD5Final(digest, &context);          /* finish up 2nd pass */
}


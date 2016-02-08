/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename        md5.c
 *
 * @purpose         MD5 implementation as per RFC 1321
 *                  The code in this file is used by all portable
 *                  systems requiring MD5
 *
 * @component       Routing Utils Component
 *
 * @comments
 *
 * @create          02/24/1999
 *
 * @author          Dan Dovolsky
 *
 * @end                                                                        
 *
 * ********************************************************************/        
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/md5.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif

/* --- standard include files --- */
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "std.h"
#include "local.h"

/* --- external object interfaces --- */

#include "xx.ext"
#include "md5.ext"

/* --- specific include files --- */
#include "md5map_api.h"                         /* LVL7 public API header */

/* --- internal prototypes --- */
/* --- internal (static) data  --- */
#if BIG_ENDIAN
static void MdReverse(ulng *X);
#endif
static void MdBlock(t_Md *p_Md, ulng *X);

/* Compile-time declarations of MD5 ``magic constants''.*/

#define I0  0x67452301L      /* Initial values for MD buffer */
#define I1  0xefcdab89L
#define I2  0x98badcfeL
#define I3  0x10325476L
#define fs1  7               /* round 1 shift amounts */
#define fs2 12   
#define fs3 17  
#define fs4 22  
#define gs1  5               /* round 2 shift amounts */
#define gs2  9   
#define gs3 14   
#define gs4 20  
#define hs1  4               /* round 3 shift amounts */
#define hs2 11 
#define hs3 16 
#define hs4 23
#define is1  6               /* round 4 shift amounts */
#define is2 10
#define is3 15
#define is4 21



/**********************************************************************
Compile-time macro declarations for MD5.
Note: The ``rot'' operator uses the variable ``tmp''.
It assumes tmp is declared as unsigned int, so that the >>
operator will shift in zeros rather than extending the sign bit.
**********************************************************************/
#define f(X,Y,Z)             ((X&Y) | ((~X)&Z))
#define g(X,Y,Z)             ((X&Z) | (Y&(~Z)))
#define h(X,Y,Z)             (X^Y^Z)
#define i_(X,Y,Z)            (Y ^ ((X) | (~Z)))
#define rot(X,S)             (tmp=X,(tmp<<S) | (tmp>>(32-S)))
#define ff(A,B,C,D,i,s,lp)   A = rot((A + f(B,C,D) + X[i] + lp),s) + B
#define gg(A,B,C,D,i,s,lp)   A = rot((A + g(B,C,D) + X[i] + lp),s) + B
#define hh(A,B,C,D,i,s,lp)   A = rot((A + h(B,C,D) + X[i] + lp),s) + B
#define ii(A,B,C,D,i,s,lp)   A = rot((A + i_(B,C,D) + X[i] + lp),s) + B
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 


/**********************************************************************
MdInit 

Initialize message digest buffer. 
**********************************************************************/
void MdInit(t_Md *p_Md)
{
    int i;

    p_Md->buffer[0] = I0;
    p_Md->buffer[1] = I1;
    p_Md->buffer[2] = I2;
    p_Md->buffer[3] = I3;
    for (i = 0; i < 8; i++)
        p_Md->count[i] = 0;
    p_Md->done = 0;
}


#if BIG_ENDIAN
/**********************************************************************
MdReverse

Reverse the byte-ordering of every ulng in X.
Assumes X is an array of 16 ints.
**********************************************************************/
static void MdReverse(ulng *X)
{
    ulng   t;
    int     i;

    for (i = 0; i < 16; ++i)
    {
        t = (*X << 16) | (*X >> 16);
        *X++ = ((t & 0xFF00FF00) >> 8) | ((t & 0x00FF00FF) << 8);
    }
}
#endif /* BIG_ENDIAN */


/**********************************************************************
MdBlock

Update message digest buffer using 16-word data block X.
**********************************************************************/
static void MdBlock(t_Md *p_Md, ulng *X)
{ 
    register ulng  tmp, A, B, C, D;
#if BIG_ENDIAN
    MdReverse(X);
#endif

    A = p_Md->buffer[0];
    B = p_Md->buffer[1];
    C = p_Md->buffer[2];
    D = p_Md->buffer[3];

    /* Update the message digest buffer */
    ff(A , B , C , D ,  0 , fs1 , 0xD76AA478L /* 3614090360 */);
    ff(D , A , B , C ,  1 , fs2 , 0xE8C7B756L /* 3905402710 */);
    ff(C , D , A , B ,  2 , fs3 , 0x60610581L /* 606105819  */);
    ff(B , C , D , A ,  3 , fs4 , 0xC1BDCEEEL /* 3250441966 */);
    ff(A , B , C , D ,  4 , fs1 , 0xF57C0FAFL /* 4118548399 */);
    ff(D , A , B , C ,  5 , fs2 , 0x4787C62AL /* 1200080426 */);
    ff(C , D , A , B ,  6 , fs3 , 0xA8304613L /* 2821735955 */);
    ff(B , C , D , A ,  7 , fs4 , 0xFD469501L /* 4249261313 */);
    ff(A , B , C , D ,  8 , fs1 , 0x698098D8L /* 1770035416 */);
    ff(D , A , B , C ,  9 , fs2 , 0x8B44F7AFL /* 2336552879 */);
    ff(C , D , A , B , 10 , fs3 , 0xFFFF5BB1L /* 4294925233 */);
    ff(B , C , D , A , 11 , fs4 , 0x895CD7BEL /* 2304563134 */);
    ff(A , B , C , D , 12 , fs1 , 0x6B901122L /* 1804603682 */);
    ff(D , A , B , C , 13 , fs2 , 0xFD987193L /* 4254626195 */);
    ff(C , D , A , B , 14 , fs3 , 0xA679438EL /* 2792965006 */);
    ff(B , C , D , A , 15 , fs4 , 0x49B40821L /* 1236535329 */);
    gg(A , B , C , D ,  1 , gs1 , 0xF61E2562L /* 4129170786 */);
    gg(D , A , B , C ,  6 , gs2 , 0xC040B340L /* 3225465664 */);
    gg(C , D , A , B , 11 , gs3 , 0x265E5A51L /* 643717713  */);
    gg(B , C , D , A ,  0 , gs4 , 0xE9B6C7AAL /* 3921069994 */);
    gg(A , B , C , D ,  5 , gs1 , 0xD62F105DL /* 3593408605 */);
    gg(D , A , B , C , 10 , gs2 , 0x2441453L  /* 38016083   */);
    gg(C , D , A , B , 15 , gs3 , 0xD8A1E681L /* 3634488961 */);
    gg(B , C , D , A ,  4 , gs4 , 0xE7D3FBC8L /* 3889429448 */);
    gg(A , B , C , D ,  9 , gs1 , 0x21E1CDE6L /* 568446438  */);
    gg(D , A , B , C , 14 , gs2 , 0xC33707D6L /* 3275163606 */);
    gg(C , D , A , B ,  3 , gs3 , 0xF4D50D87L /* 4107603335 */);
    gg(B , C , D , A ,  8 , gs4 , 0x455A14EDL /* 1163531501 */);
    gg(A , B , C , D , 13 , gs1 , 0xA9E3E905L /* 2850285829 */);
    gg(D , A , B , C ,  2 , gs2 , 0xFCEFA3F8L /* 4243563512 */);
    gg(C , D , A , B ,  7 , gs3 , 0x676F02D9L /* 1735328473 */);
    gg(B , C , D , A , 12 , gs4 , 0x8D2A4C8AL /* 2368359562 */);
    hh(A , B , C , D ,  5 , hs1 , 0xFFFA3942L /* 4294588738 */);
    hh(D , A , B , C ,  8 , hs2 , 0x8771F681L /* 2272392833 */);
    hh(C , D , A , B , 11 , hs3 , 0x6D9D6122L /* 1839030562 */);
    hh(B , C , D , A , 14 , hs4 , 0xFDE5380CL /* 4259657740 */);
    hh(A , B , C , D ,  1 , hs1 , 0xA4BEEA44L /* 2763975236 */);
    hh(D , A , B , C ,  4 , hs2 , 0x4BDECFA9L /* 1272893353 */);
    hh(C , D , A , B ,  7 , hs3 , 0xF6BB4B60L /* 4139469664 */);
    hh(B , C , D , A , 10 , hs4 , 0xBEBFBC70L /* 3200236656 */);
    hh(A , B , C , D , 13 , hs1 , 0x289B7EC6L /* 681279174  */);
    hh(D , A , B , C ,  0 , hs2 , 0xEAA127FAL /* 3936430074 */);
    hh(C , D , A , B ,  3 , hs3 , 0xD4EF3085L /* 3572445317 */);
    hh(B , C , D , A ,  6 , hs4 , 0x4881D05L  /* 76029189   */);
    hh(A , B , C , D ,  9 , hs1 , 0xD9D4D039L /* 3654602809 */);
    hh(D , A , B , C , 12 , hs2 , 0xE6DB99E5L /* 3873151461 */);
    hh(C , D , A , B , 15 , hs3 , 0x1FA27CF8L /* 530742520  */);
    hh(B , C , D , A ,  2 , hs4 , 0xC4AC5665L /* 3299628645 */);
    ii(A , B , C , D ,  0 , is1 , 0xF4292244L /* 4096336452 */);
    ii(D , A , B , C ,  7 , is2 , 0x432AFF97L /* 1126891415 */);
    ii(C , D , A , B , 14 , is3 , 0xAB9423A7L /* 2878612391 */);
    ii(B , C , D , A ,  5 , is4 , 0xFC93A039L /* 4237533241 */);
    ii(A , B , C , D , 12 , is1 , 0x655B59C3L /* 1700485571 */);
    ii(D , A , B , C ,  3 , is2 , 0x8F0CCC92L /* 2399980690 */);
    ii(C , D , A , B , 10 , is3 , 0xFFEFF47DL /* 4293915773 */);
    ii(B , C , D , A ,  1 , is4 , 0x85845DD1L /* 2240044497 */);
    ii(A , B , C , D ,  8 , is1 , 0x6FA87E4FL /* 1873313359 */);
    ii(D , A , B , C , 15 , is2 , 0xFE2CE6E0L /* 4264355552 */);
    ii(C , D , A , B ,  6 , is3 , 0xA3014314L /* 2734768916 */);
    ii(B , C , D , A , 13 , is4 , 0x4E0811A1L /* 1309151649 */);
    ii(A , B , C , D ,  4 , is1 , 0xF7537E82L /* 4149444226 */);
    ii(D , A , B , C , 11 , is2 , 0xBD3AF235L /* 3174756917 */);
    ii(C , D , A , B ,  2 , is3 , 0x2AD7D2BBL /* 718787259  */);
    ii(B , C , D , A ,  9 , is4 , 0xEB86D391L /* 3951481745 */);

    p_Md->buffer[0] += A; 
    p_Md->buffer[1] += B;
    p_Md->buffer[2] += C;
    p_Md->buffer[3] += D; 
}




/**********************************************************************
 MdUpdate
 Input: p_Md -- pointer to the MD structure
        X -- a pointer to an array of unsigned characters.
        count -- the number of bits of X to use.
                 (if not a multiple of 8, uses high bits of last byte.)
Update p_Md using the number of bits of X given by count
**********************************************************************/
void MdUpdate(t_Md *p_Md, byte *X, word count)
{
    byte           XX[64];
    byte           *p;
    word           i, tmp, bit, md_byte, mask;

    /* return with no error if this is a courtesy close with count
    ** zero and MDp->done is true.
    */
    if (count == 0 && p_Md->done)
        return;

    /* check to see if MD is already done and report error
     */
    if (p_Md->done)
    {
      XX_Print("MD5: Error: MDupdate MD already done.");
      return;
    }

    /* Add count to p_Md->count
     */
    tmp = count;
    p = p_Md->count;
    while (tmp)
    {
        tmp += *p;
        *p++ = (byte) tmp;
        tmp = tmp >> 8;
    }

    /* Process data */
    if (count == 512) 
    {
        /* Full block of data to handle */
        MdBlock(p_Md, (ulng *)X);
    }
    else if (count > 512) /* Check for count too large */
    {
      XX_Print("MD5 Error: MDupdate called with illegal count value");
      return;
    }
    else /* partial block -- must be last block so finish up */
    {
        /* Find out how many bytes and residual bits there are
         */
        md_byte = count >> 3;
        bit = count & 7;

        /* Copy X into XX since we need to modify it
         */
        for (i = 0; i <= md_byte; i++)
            XX[i] = X[i];

        for (i = md_byte + 1; i < 64; i++)
            XX[i] = 0;

        /* Add padding '1' bit and low-order zeros in last byte
         */
        mask = 1 << (7 - bit);
        XX[md_byte] = (byte)((XX[md_byte] | mask) & ~( mask - 1));

        /* If room for bit count, finish up with this block
         */
        if (md_byte <= 55)
        {
            for (i=0;i<8;i++)
                XX[56+i] = p_Md->count[i];
            MdBlock(p_Md, (ulng *)XX);
        }
        else /* need to do two blocks to finish up */
        {
            MdBlock(p_Md, (ulng *)XX);
            for (i = 0; i < 56; i++)
                XX[i] = 0;

            for (i = 0; i < 8; i++)
                XX[56+i] = p_Md->count[i];
            MdBlock(p_Md, (ulng *)XX);
        }

        /* Set flag saying we're done with MD computation
         */
        p_Md->done = 1;
    }
}


/**********************************************************************
Generates a digest for data. The digest is copied into *digest, which
must be of size 16.
**********************************************************************/
void MdGetDigest(byte *data, int length, byte *digest)
{
    /* use mapping layer to invoke LVL7 MD5 utility functions */
    md5MapDigestGet((unsigned char *)data, (unsigned int)length, 
                    (unsigned char *)digest);
}

/* --- end of file md5.c --- */

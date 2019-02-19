/*
 * $Id: nlmcmcast.h,v 1.2.8.1 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */


 
#ifndef INCLUDED_NLMCMCAST_H
#define INCLUDED_NLMCMCAST_H

#ifndef __doxygen__

/* check for macro arg side effects */
#define NlmCmAssertStable_(name) NlmCmAssert_((name) == (name), "stable expression")
#define NlmCmAssertStable(name) NlmCmAssert((name) == (name), "stable expression")

/* detect unsafe size_t casts */
#define NlmCmSize_t__castUint32(val) \
    (NlmCmAssertStable_(val) \
     NlmCmAssert_((val) <= UINT32_MAX, "Illegal size_t cast") \
     (nlm_u32)val)

/* Define type-safe and value checking casting macros for common integral types */

/* nlm_8 => nlm_16, nlm_32, nlm_u8, nlm_u16, nlm_u32 */
#define NlmCmInt8__castInt16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt8__Identity(val)) \
     (nlm_16)(val))

#define NlmCmInt8__castInt32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt8__Identity(val)) \
     (nlm_32)(val))

#define NlmCmInt8__castUint8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt8__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     (nlm_u8)(val))

#define NlmCmInt8__castUint16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt8__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     (nlm_u16)(val))

#define NlmCmInt8__castUint32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt8__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     (nlm_u32)(val))

/* nlm_16 => nlm_8, nlm_32, nlm_u8, nlm_u16, nlm_u32 */

/* N.B., the nlm_16 cast is to work around a quirk in a number
   of compilers. When passed an unsigned integer, some compilers
   silently zero-extend the signed constant resulting in erroneous
   checks against INT8_MIN.
*/
#define NlmCmInt16__castInt8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt16__Identity(val)) \
     NlmCmAssert_((val) <= INT8_MAX, "value out of range") \
     NlmCmAssert_((nlm_16)(val) >= INT8_MIN, "value out of range") \
     (nlm_8)(val))

#define NlmCmInt16__castInt32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt16__Identity(val)) \
     (nlm_32)(val))

#define NlmCmInt16__castUint8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt16__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     NlmCmAssert_((nlm_u16)(val) <= UINT8_MAX, "value out of range") \
     (nlm_u8)(val))

#define NlmCmInt16__castUint16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt16__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     (nlm_u16)(val))

#define NlmCmInt16__castUint32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt16__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     (nlm_u32)(val))

/* nlm_32 => nlm_8, nlm_16, nlm_u8, nlm_u16, nlm_u32 */

/* N.B., the nlm_16 cast is to work around a quirk in a number
   of compilers. When passed an unsigned integer, some compilers
   silently zero-extend the signed constant resulting in erroneous
   checks against INT8_MIN.
*/
#define NlmCmInt32__castInt8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt32__Identity(val)) \
     NlmCmAssert_((val) <= INT8_MAX, "value out of range") \
     NlmCmAssert_((nlm_32)(val) >= INT8_MIN, "value out of range") \
     (nlm_8)(val))
/* N.B., the nlm_16 cast is to work around a quirk in a number
   of compilers. When passed an unsigned integer, some compilers
   silently zero-extend the signed constant resulting in erroneous
   checks against INT16_MIN.
*/
#define NlmCmInt32__castInt16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt32__Identity(val)) \
     NlmCmAssert_((val) <= INT16_MAX, "value out of range") \
     NlmCmAssert_((nlm_32)(val) >= INT16_MIN, "value out of range") \
     (nlm_16)(val))

#define NlmCmInt32__castUint8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt32__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     NlmCmAssert_((nlm_u32)(val) <= UINT8_MAX, "value out of range") \
     (nlm_u8)(val))

#define NlmCmInt32__castUint16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt32__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     NlmCmAssert_((nlm_u32)(val) <= UINT16_MAX, "value out of range") \
     (nlm_u16)(val))

#define NlmCmInt32__castUint32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmInt32__Identity(val)) \
     NlmCmAssert_((val) >= 0, "value out of range") \
     (nlm_u32)(val))

/* nlm_u8 => nlm_8, nlm_16, nlm_32, nlm_u16, nlm_u32 */
#define NlmCmUint8__castInt8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint8__Identity(val)) \
     NlmCmAssert_((val) <= (nlm_u8)INT8_MAX, "value out of range") \
     (nlm_8)(val))

#define NlmCmUint8__castInt16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint8__Identity(val)) \
     (nlm_16)(val))

#define NlmCmUint8__castInt32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint8__Identity(val)) \
     (nlm_32)(val))

#define NlmCmUint8__castUint16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint8__Identity(val)) \
     (nlm_u16)(val))

#define NlmCmUint8__castUint32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint8__Identity(val)) \
     (nlm_u32)(val))

/* nlm_u16 => nlm_8, nlm_16, nlm_32, nlm_u8, nlm_u32 */
#define NlmCmUint16__castInt8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint16__Identity(val)) \
     NlmCmAssert_((val) <= (nlm_u16)INT8_MAX, "value out of range") \
     (nlm_8)(val))

#define NlmCmUint16__castInt16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint16__Identity(val)) \
     NlmCmAssert_((val) <= (nlm_u16)INT16_MAX, "value out of range") \
     (nlm_16)(val))

#define NlmCmUint16__castInt32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint16__Identity(val)) \
     (nlm_32)(val))

#define NlmCmUint16__castUint8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint16__Identity(val)) \
     NlmCmAssert_((val) <= UINT8_MAX, "value out of range") \
     (nlm_u8)(val))

#define NlmCmUint16__castUint32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint16__Identity(val)) \
     (nlm_u32)(val))

/* nlm_u32 => nlm_8, nlm_16, nlm_32, int, nlm_u8, nlm_u16, unsigned int */
#define NlmCmUint32__castInt8(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint32__Identity(val)) \
     NlmCmAssert_((val) <= (nlm_u8)INT8_MAX, "value out of range") \
     (nlm_8)(val))

#define NlmCmUint32__castInt16(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint32__Identity(val)) \
     NlmCmAssert_((val) <= (nlm_u16)INT16_MAX, "value out of range") \
     (nlm_16)(val))

#define NlmCmUint32__castInt32(val) \
    (NlmCmAssertStable_(val) NlmCmDbgOnly_(NlmCmUint32__Identity(val)) \
     NlmCmAssert_((val) <= (nlm_u32)INT32_MAX, "value out of range") \
     (nlm_32)(val))

#define NlmCmUint32__castUint8(val) \
    (NlmCmAssertStable_(val) NlmCmAssert_( \
     NlmCmUint32__Identity(val) <= UINT8_MAX, "value out of range") \
     (nlm_u8)(val))

#define NlmCmUint32__castUint16(val) \
    (NlmCmAssertStable_(val) NlmCmAssert_( \
     NlmCmUint32__Identity(val) <= UINT16_MAX, "value out of range") \
     (nlm_u16)(val))

#define NlmCmVoidStar__castUintVS(val) \
    ((uintvs_t)NlmCmVoidStar__Identity(val))

#define NlmCmUintVS__castVoidStar(val) \
    ((void*)(NlmCmUintVS__Identity(val)))

#define NlmCmVoidStar__castUint8Star(val) \
    ((nlm_u8*)NlmCmVoidStar__Identity(val))


#define NlmCmDouble__castUint32(val) \
    (NlmCmAssertStable_(val) \
    NlmCmAssert_((val) >= 0 && (val) <=UINT32_MAX,"value out of range.") \
    (nlm_u32)(val))

#define NlmCmUint32__castDouble(val) \
    (NlmCmAssertStable_(val) \
    NlmCmAssert_(sizeof(nlm_u32)<=sizeof(double),"possible value out of range.") \
    (double)(val))

/*================================================================*/

#define NlmCmUint8Star__castCharStar(val) \
    ((char*)NlmCmUint8Star__Identity(val))

#define NlmCmCharStar__castUint8Star(val) \
    ((nlm_u8*)NlmCmCharStar__Identity(val))

#define NlmCmCharStar__constcast(val) \
    ( NlmCmDbgOnly_(NlmCmConstCharStar__Identity(val)) (char*)(val) )

/*================================================================*/

#endif /*__doxygen__*/


#endif

/*[]*/













/*
 **************************************************************************************
 Copyright 2009-2019 Broadcom Corporation

 This program is the proprietary software of Broadcom Corporation and/or its licensors,
 and may only be used, duplicated, modified or distributed pursuant to the terms and
 conditions of a separate, written license agreement executed between you and
 Broadcom (an "Authorized License").Except as set forth in an Authorized License,
 Broadcom grants no license (express or implied),right to use, or waiver of any kind
 with respect to the Software, and Broadcom expressly reserves all rights in and to
 the Software and all intellectual property rights therein.
 IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 WAY,AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization, constitutes the
    valuable trade secrets of Broadcom, and you shall use all reasonable efforts to
    protect the confidentiality thereof,and to use this information only in connection
    with your use of Broadcom integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND WITH
    ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER
    EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM
    SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
    NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS LICENSORS
    BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES
    WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE
    THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
    OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
    ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 **************************************************************************************
 */



#ifndef INCLUDED_KAPS_PREFIX_H
#define INCLUDED_KAPS_PREFIX_H

#include "kaps_portable.h"
#include "kaps_legacy.h"
#include "kaps_math.h"
#include <kaps_externcstart.h>  /* this should be the final include */

/* Summary
   The kaps_prefix data structure.

   Description
   The m_data member is over-indexed.

   See Also
   kaps_prefix_create
*/
typedef struct kaps_prefix
{

    uint16_t m_avail;           /* Number of allocated bits available for storage. Always a multiple of 8. */

    uint16_t m_inuse;           /* Number of bits actually in use. */

    uint8_t m_data[4];          /* Storage for the bits. This is <B>over-indexed!</B> Also, it is sized such that the
                                 * structure is 4-byte aligned, and should remain this way for portability. */

} kaps_prefix;

/* Given a bit location determine the byte number (starting at 0) that contains that bit. */
#define     KAPS_PREFIX_GET_BIT_BYTE(n)      ((n) >> 3)

/* Given a bit location determine the byte number (starting at 0) that contains that bit. */
#define     KAPS_PREFIX_GET_BIT_MASK(n)      (0x80 >> ((n) & 7))

/* Given a bit location determine the byte number (starting at 0) that contains that bit. */
#define     KAPS_PREFIX_GET_NUM_BYTES(numBits)   (((numBits) + 7) >> 3)

extern kaps_prefix *kaps_prefix_create(
    kaps_nlm_allocator * allocator,
    unsigned maxBitLength,
    unsigned initialDataBitLength,
    const uint8_t * initialData);

extern void kaps_prefix_destroy(
    kaps_prefix * self,
    kaps_nlm_allocator * alloc);

extern void kaps_prefix_append(
    kaps_prefix * self,
    unsigned dataLength,
    uint8_t * data);

#define KAPS_PREFIX_PVT_GET_BIT_RAW(data,len,IX) (uint8_t)(data[(IX)>>3] & (0x80>>((IX)&7)))

#define KAPS_PREFIX_PVT_GET_BIT(data, len, location) (uint8_t)(KAPS_PREFIX_PVT_GET_BIT_RAW(data, len, location) != 0)

#define KAPS_PREFIX_GET_BIT(self,ix)        (KAPS_PREFIX_PVT_GET_BIT(self->m_data, self->m_inuse, ix))

#define KAPS_PREFIX_SET_BIT(self,ix,v)      { uint32_t __x = (ix) ; uint8_t __m = \
                          (uint8_t)KAPS_PREFIX_GET_BIT_MASK(__x), \
                          *ch = (self)->m_data+KAPS_PREFIX_GET_BIT_BYTE(__x) ;\
                          *ch = (uint8_t)((v) ? (*ch | __m) : (*ch & ~__m)) ; }

#define kaps_prefix_is_more_specific(prefix1, prefix2) ((prefix1->m_inuse <= prefix2->m_inuse ? 0 : \
    kaps_prefix_pvt_is_more_specific_equal((prefix1)->m_data, \
    (prefix1)->m_inuse, (prefix2)->m_data, (prefix2)->m_inuse)))

#define kaps_prefix_is_more_specific_equal(prefix1, prefix2) ((prefix1->m_inuse < prefix2->m_inuse) ? 0 : \
    kaps_prefix_pvt_is_more_specific_equal((prefix1)->m_data, \
    (prefix1)->m_inuse, (prefix2)->m_data, (prefix2)->m_inuse))

#define kaps_prefix_pvt_is_more_specific(pfx1data, pfx1len, pfx2data, pfx2len) ( (pfx1len <= pfx2len) ? 0 : \
    kaps_prefix_pvt_is_more_specific_equal(pfx1data, pfx1len, pfx2data, pfx2len))

/* IsMoreSpecific that also supports the case when the prefixes are of the same length */
int32_t kaps_prefix_pvt_is_more_specific_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len);

int32_t kaps_prefix_pvt_is_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len);

#define NlmCmPrefix__AssertTrailingBits(data, inuse, avail) \
    kaps_sassert(kaps_prefix_check_trailing_bits(data, inuse, avail))

extern int32_t kaps_prefix_check_trailing_bits(
    const uint8_t * pfxdata,
    uint32_t inuse_bits,
    uint32_t avail_bits);

#define NlmCmPrefix__AssertValid(self) \
    kaps_sassert(kaps_prefix_check_trailing_bits(self->m_data, self->m_inuse, self->m_avail))

#define NlmCmPrefix__IsValid(self)  \
    kaps_prefix_check_trailing_bits(self->m_data, self->m_inuse, self->m_avail)

/* Summary
   Print a prefix in IP notation (XXX.XXX.XXX.XXX/LENGTH)

   Description
   Print the given prefix to the given FILE * in IP notation.

   Parameters
   @param self The prefix to print
   @param fp The FILE * to print to
*/
extern void kaps_prefix_print_as_ip(
    kaps_prefix * self,
    FILE * fp);

#define KAPS_PREFIX_GET_STORAGE_SIZE(nBits) (KAPS_MAX((sizeof(kaps_prefix) + KAPS_PREFIX_GET_NUM_BYTES(nBits) - sizeof(((kaps_prefix*)0)->m_data)), \
                                                    (sizeof(kaps_prefix))))

#include <kaps_externcend.h>

#endif

/*[]*/

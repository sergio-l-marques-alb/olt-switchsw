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

#include <kaps_fib_cmn_pfxbundle.h>
#include <kaps_math.h>
#include "kaps_hw_limits.h"

/*
 * pfxsz is the number of bytes required to store prefix data
 * and assocSz is the number of bytes for the assocData
 */
#define KAPSCMROUND2_(x)     (((uint32_t)x+1)&~(uint32_t)1)
#define KAPSCMBUNDLESIZE(s1,s2)  (KAPSCMROUND2_(s1) + KAPSCMROUND2_(s2) - sizeof(uint8_t) * 2 + sizeof(kaps_pfx_bundle))

kaps_pfx_bundle *
kaps_pfx_bundle_create(
    kaps_nlm_allocator * alloc,
    kaps_prefix * prefix,
    uint32_t ix,
    uint32_t assocSize,
    uint32_t seqNum)
{
    uint32_t tmp = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(prefix->m_inuse);
    uint32_t sz = KAPSCMBUNDLESIZE(tmp, assocSize);
    kaps_pfx_bundle *ret = (kaps_pfx_bundle *) kaps_nlm_allocator_calloc(alloc, 1, sz);

    /*
     * We do not initialize the associated data 
     */
    ret->m_nIndex = ix;
    ret->m_nPfxSize = prefix->m_inuse;
#ifdef KAPS_LPM_DEBUG
    ret->m_nSeqNum = seqNum;
#endif
    kaps_memcpy(ret->m_data, prefix->m_data, tmp);
    return ret;
}

kaps_pfx_bundle *
kaps_pfx_bundle_create_from_string(
    kaps_nlm_allocator * alloc,
    const uint8_t * prefix,
    uint32_t numbits,
    uint32_t ix,
    uint32_t assocSize,
    uint8_t addExtraFirstByte)
{
    uint32_t tmp = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(numbits);
    uint32_t sz;
    kaps_pfx_bundle *ret;

    if (addExtraFirstByte)
        sz = KAPSCMBUNDLESIZE(tmp + KAPS_BITS_IN_BYTE, assocSize);
    else
        sz = KAPSCMBUNDLESIZE(tmp, assocSize);

    ret = (kaps_pfx_bundle *) kaps_nlm_allocator_calloc(alloc, 1, sz);

    /*
     * We do not initialize the associated data 
     */
    ret->m_nIndex = ix;

    ret->m_nPfxSize = (uint16_t) numbits;
    if (addExtraFirstByte)
        ret->m_nPfxSize += KAPS_BITS_IN_BYTE;

    if (prefix)
    {
        uint32_t startByte = 0;
        uint32_t endByte = tmp - 1;

        if (addExtraFirstByte)
        {
            startByte = 1;
            endByte = tmp;
        }

        kaps_memcpy(&ret->m_data[startByte], prefix, tmp);
        if (numbits & 7)
        {
            uint32_t mask = (KAPS_PREFIX_GET_BIT_MASK(numbits - 1) - 1);
            ret->m_data[endByte] &= ~mask;
        }

    }

    kaps_pfx_bundle_assert_valid(ret);

    return ret;
}

kaps_pfx_bundle *
kaps_pfx_bundle_create_from_pfx_bundle(
    kaps_nlm_allocator * alloc,
    kaps_pfx_bundle * oldPfxBundle,
    uint32_t assocSize,
    uint8_t copyAssoc)
{
    uint32_t tmp = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(oldPfxBundle->m_nPfxSize);
    uint32_t sz = KAPSCMBUNDLESIZE(tmp, assocSize);
    kaps_pfx_bundle *ret = (kaps_pfx_bundle *) kaps_nlm_allocator_calloc(alloc, 1, sz);

    ret->m_backPtr = oldPfxBundle->m_backPtr;
    ret->m_isAptLmpsofarPfx = oldPfxBundle->m_isAptLmpsofarPfx;
    ret->m_isPfxCopy = oldPfxBundle->m_isPfxCopy;
    ret->m_nIndex = oldPfxBundle->m_nIndex;
    ret->m_nPfxSize = oldPfxBundle->m_nPfxSize;

#ifdef KAPS_LPM_DEBUG
    ret->m_nSeqNum = oldPfxBundle->m_nSeqNum;
#endif

    ret->m_status = oldPfxBundle->m_status;

    kaps_memcpy(ret->m_data, oldPfxBundle->m_data, tmp);

    if (copyAssoc)
    {
        kaps_memcpy(KAPS_PFX_BUNDLE_GET_ASSOC_PTR(ret), KAPS_PFX_BUNDLE_GET_ASSOC_PTR(oldPfxBundle), assocSize);
    }

    kaps_pfx_bundle_assert_valid(ret);
    return ret;
}

void
kaps_pfx_bundle_set_bit(
    kaps_pfx_bundle * self,
    uint32_t ix,
    int32_t bit)
{
    if (ix < self->m_nPfxSize)
    {
        if (bit)
            self->m_data[ix >> 3] |= (0x80 >> (ix & 7));
        else
            self->m_data[ix >> 3] &= ~(0x80 >> (ix & 7));
    }
    else
        kaps_sassert(0);
}

int
kaps_pfx_bundle_compare(
    const kaps_pfx_bundle * prefix1,
    const kaps_pfx_bundle * prefix2)
{
    /*
     * NOTE: Borrowed from kaps_prefix 
     */
    uint32_t numBytes;
    int ret;

    kaps_pfx_bundle_assert_valid(prefix1);
    kaps_pfx_bundle_assert_valid(prefix2);

    numBytes = KAPS_MIN(prefix1->m_nPfxSize, prefix2->m_nPfxSize);
    numBytes = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(numBytes);

    /*
     * Huzzah. Due to the internal bit ordering, we can do a simple kaps_memcmp to compare prefixes. 
     */
    ret = kaps_memcmp(prefix1->m_data, prefix2->m_data, numBytes);

    if (!ret)
    {
        ret = (int) (prefix1->m_nPfxSize - prefix2->m_nPfxSize);
    }

    return ret;
}

void
kaps_pfx_bundle_print(
    kaps_pfx_bundle * self,
    FILE * fp)
{
    uint32_t byteCounter;       /* Current byte we're filling up */
    uint32_t bitCounter;        /* Current bit in current byte */
    uint32_t currentByte;
    uint32_t sz = KAPS_PFX_BUNDLE_GET_PFX_SIZE(self);

    /*
     * Start the counters at 0 then increment and set the bit 
     */
    byteCounter = 0;
    bitCounter = 0;
    currentByte = self->m_data[0];

    while (bitCounter < sz)
    {

        if (currentByte & 0x80)
            kaps_fprintf(fp, "1");
        else
            kaps_fprintf(fp, "0");

        currentByte <<= 1;
        bitCounter++;
        if ((bitCounter % 8) == 0)
        {
            byteCounter++;
            currentByte = self->m_data[byteCounter];
        }
    }
    kaps_fprintf(fp, "\n");
}

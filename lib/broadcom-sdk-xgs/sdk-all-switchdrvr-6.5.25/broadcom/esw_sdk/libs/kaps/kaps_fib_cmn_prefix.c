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

#include <kaps_fib_cmn_prefix.h>
#include "kaps_utility.h"

#define CONSTCASTINUSE(self) (*(uint16_t*)(&self->m_inuse))
#define MAX_SIZE_IN_BITS    0xffffU
#define ASSIGN_AS_U16(y, x) kaps_sassert (x <= MAX_SIZE_IN_BITS) ; y = (uint16_t) x

kaps_prefix *kaps_prefix_pvt_ctor(
    kaps_prefix * self,
    uint16_t initialAvail,
    uint16_t dataLength,
    const uint8_t * initialData);

int32_t
kaps_prefix_check_trailing_bits(
    const uint8_t * data,
    uint32_t pos,
    uint32_t end)
{
    if (pos > end)
    {
        kaps_sassert(0);        /* Huh? prefix length greater than allocated length? */
        return 0;
    }

    for (; pos < end; pos++)
    {
        if ((pos & 7) == 0)
        {
            for (; pos < end; pos += 8)
            {
                if (data[KAPS_PREFIX_GET_BIT_BYTE(pos)] != 0)
                {
                    kaps_sassert(data[KAPS_PREFIX_GET_BIT_BYTE(pos)] == 0);
                    return 0;
                }
            }
        }
        else
        {
            if (0 != (data[KAPS_PREFIX_GET_BIT_BYTE(pos)] & KAPS_PREFIX_GET_BIT_MASK(pos)))
            {
                kaps_sassert(0 == (data[KAPS_PREFIX_GET_BIT_BYTE(pos)] & KAPS_PREFIX_GET_BIT_MASK(pos)));
                return 0;
            }
        }
    }
    return 1;
}

/*---------------------------------------------------------------------------*/
/* Public method implementations */
/*---------------------------------------------------------------------------*/

/*
   Don't repeat the parameter descriptions from the header
   ==== USE ONE VERSION OF THE TRUTH ====
   But DO USE proper argument names, since this is to where tags comes.
*/

kaps_prefix *
kaps_prefix_create(
    kaps_nlm_allocator * allocator,
    unsigned maxBitLength,
    unsigned initialDataBitLength,
    const uint8_t * initialData)
{
    size_t prefixSize;
    uint16_t initialAvail;
    uint16_t dataLength;
    kaps_prefix *self;
    uint32_t avail32;
    uint16_t avail;

    /*
     * Convert the incoming unsigned int parameters to uint16_t 
     */
    ASSIGN_AS_U16(initialAvail, maxBitLength);
    ASSIGN_AS_U16(dataLength, initialDataBitLength);

    /*
     * round avail to a multiple of 8 
     */
    avail32 = initialAvail;
    avail32 = (avail32 + 7) & (~0x7);
    avail = avail32;
    initialAvail = avail;

    /*
     * The size to allocate is the size of the prefix data structure plus the initialAvail size.  Subtract 1 for the
     * m_data[1] byte that is already in the structure 
     */

    prefixSize = KAPS_PREFIX_GET_STORAGE_SIZE(avail);

    self = (kaps_prefix *) kaps_nlm_allocator_calloc(allocator, 1, prefixSize);

    if (self)
        kaps_prefix_pvt_ctor(self, initialAvail, dataLength, initialData);
    return (self);

}

void
kaps_prefix_destroy(
    kaps_prefix * self,
    kaps_nlm_allocator * alloc)
{
    if (self)
    {

        kaps_nlm_allocator_free(alloc, self);
    }
}

void
kaps_prefix_append(
    kaps_prefix * self,
    unsigned dataLength_u,      /* Length of incoming data */
    uint8_t * data)
{       /* Data stream to use */
    uint16_t byteCounter;       /* Current byte we're working with */
    uint16_t bitCounter;        /* Current bit in current byte */
    uint32_t currentByte;
    uint16_t dataLength;
    uint32_t pos;
    uint32_t newLength = self->m_inuse + dataLength_u;

    NlmCmPrefix__AssertValid(self);

    kaps_sassert(self->m_avail >= newLength);

    /*
     * convert the incoming unsigned to uint16_t 
     */
    ASSIGN_AS_U16(dataLength, dataLength_u);

    /*
     * Start the counters at 0 then increment and set the bit 
     */
    byteCounter = 0;
    bitCounter = 0;
    currentByte = data[byteCounter];

    pos = self->m_inuse;
    CONSTCASTINUSE(self) = newLength;

    while (bitCounter < dataLength)
    {

        /*
         * We dont set things to zero because they are already supposed to be zero beyond the end of the self's data 
         */
        if (currentByte & 0x80)
            self->m_data[KAPS_PREFIX_GET_BIT_BYTE(pos)] |= KAPS_PREFIX_GET_BIT_MASK(pos);
        else
            kaps_sassert(0 == (self->m_data[KAPS_PREFIX_GET_BIT_BYTE(pos)] & KAPS_PREFIX_GET_BIT_MASK(pos)));

        pos++;
        currentByte <<= 1;
        bitCounter++;
        if (((bitCounter % 8) == 0) && (bitCounter < dataLength))
        {
            byteCounter++;
            currentByte = data[byteCounter];
        }
    }

    NlmCmPrefix__AssertValid(self);
}

/* Returns True if the first prefix is more specific than the second or the two prefixes match */

int32_t
kaps_prefix_pvt_is_more_specific_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len)
{
    uint32_t nBytes, nBits;

    NlmCmPrefix__AssertTrailingBits(pfx1data, pfx1len, pfx1len);
    NlmCmPrefix__AssertTrailingBits(pfx2data, pfx2len, pfx2len);

    if (pfx1len < pfx2len)
        return 0;

    nBytes = pfx2len / 8;
    nBits = pfx2len % 8;
    if (0 != kaps_memcmp(pfx1data, pfx2data, nBytes))
        return 0;

    if (nBits)
    {
        uint32_t ch1 = pfx1data[nBytes];
        uint32_t ch2 = pfx2data[nBytes];
        uint32_t ix;
        if (ch1 == ch2)
            return 1;
        else
        {
            for (ix = 0; ix < nBits; ix++)
                if ((ch1 & KAPS_PREFIX_GET_BIT_MASK(ix)) != (ch2 & KAPS_PREFIX_GET_BIT_MASK(ix)))
                    return 0;
        }
    }
    return 1;
}

int32_t
kaps_prefix_pvt_is_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len)
{
    uint32_t nBytes, nBits;

    if (pfx1len != pfx2len)
        return 0;

    nBytes = pfx2len / 8;
    nBits = pfx2len % 8;
    if (0 != kaps_memcmp(pfx1data, pfx2data, nBytes))
        return 0;

    if (nBits)
    {
        uint32_t ch1 = pfx1data[nBytes];
        uint32_t ch2 = pfx2data[nBytes];
        uint32_t ix;
        if (ch1 == ch2)
            return 1;
        else
        {
            for (ix = 0; ix < nBits; ix++)
                if ((ch1 & KAPS_PREFIX_GET_BIT_MASK(ix)) != (ch2 & KAPS_PREFIX_GET_BIT_MASK(ix)))
                    return 0;
        }
    }
    return 1;
}

void
kaps_prefix_print_as_ip(
    kaps_prefix * self,
    FILE * fp)
{
    uint32_t bit = 0;
    uint32_t thisbyte;
    uint32_t i;
    uint32_t first = 1;

    while (bit < self->m_inuse)
    {
        i = 0;
        thisbyte = 0;
        while (i < 8 && bit < self->m_inuse)
        {
            thisbyte |= KAPS_PREFIX_GET_BIT(self, bit) << (7 - i);
            i++;
            bit++;
        }

        if (!first)
            kaps_fprintf(fp, ".");
        kaps_fprintf(fp, "%d", thisbyte);

        first = 0;
    }

    kaps_fprintf(fp, "/%d", self->m_inuse);
}

/*---------------------------------------------------------------------------*/
/* Private method implementations */
/*---------------------------------------------------------------------------*/

kaps_prefix *
kaps_prefix_pvt_ctor(
    kaps_prefix * self,         /* Already allocated space. */
    uint16_t initialAvail,      /* size in bits of allocated space */
    uint16_t dataLength,        /* Length of initial data */
    const uint8_t * initialData)
{       /* pointer to initial data */
    uint8_t *data = self->m_data;

    initialAvail = (uint16_t) ((initialAvail + 7) & ~0x7);

    kaps_sassert(dataLength <= initialAvail);
    kaps_sassert(dataLength == 0 || initialData);

    self->m_avail = initialAvail;
    CONSTCASTINUSE(self) = dataLength;
    if (dataLength)
    {
        uint32_t n = KAPS_PREFIX_GET_NUM_BYTES(dataLength);
        uint32_t m = KAPS_PREFIX_GET_NUM_BYTES(initialAvail);

        kaps_memcpy(data, initialData, n);
        if (dataLength & 7)
        {
            uint32_t mask = (KAPS_PREFIX_GET_BIT_MASK(dataLength - 1) - 1);
            data[n - 1] &= ~mask;
        }

        if (m > n)
            kaps_memset(data + n, 0, m - n);

        NlmCmPrefix__AssertValid(self);
    }
    else
        kaps_memset(data, 0, KAPS_PREFIX_GET_NUM_BYTES(initialAvail));

    return (self);
}

/*[]*/

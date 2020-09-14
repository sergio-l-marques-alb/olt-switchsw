/*******************************************************************************
 *
 * Copyright 2011-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "kaps_bitmap.h"

/** Must do weird voodoo arithmetic because (-5)/32 is 0, not -1. */
#define BMP_WORD(pos) ((32 + (pos)) / 32 - 1)
#define SUMMARY_WORD(pos) ((1024 + (pos)) / 1024 -1)
#define COUNT_BYTE(pos) ((256 + (pos)) / 256 -1)
#define CHECK_SUMMARY(fbmp, w)    \
    kaps_get_bit_of_word(fbmp->summary_bitmap[(w) / 32], (w) % 32)
#define BIT_COUNT(fbmp, b) (fbmp->counts[b] == 0 && CHECK_SUMMARY(fbmp, (b) * 8) \
                            ?256 :fbmp->counts[b])

#define debug_assert(expression) kaps_sassert(expression)

static KAPS_INLINE int
kaps_get_bit_of_word(
    uint32_t word,
    int bit_pos)
{
    debug_assert(bit_pos >= 0 && bit_pos <= 31);
    return (word >> bit_pos) & 1;
}

static KAPS_INLINE void
kaps_set_bit_of_word(
    uint32_t * pword,
    int bit_pos)
{
    debug_assert(bit_pos >= 0 && bit_pos <= 31);
    *pword |= 1 << bit_pos;
}

static KAPS_INLINE void
kaps_reset_bit_of_word(
    uint32_t * pword,
    int bit_pos)
{
    debug_assert(bit_pos >= 0 && bit_pos <= 31);
    *pword &= ~(1 << bit_pos);
}

#define get_byte_of_word(word, byte_pos) ((word >> (byte_pos * 8)) & 0xFF)

static const int kaps_first_bit_set[256] = {
    /*
     * 0,1,2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 
     */
    -25, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

static KAPS_INLINE int
kaps_first_set_in_word(
    uint32_t word,
    int start_bit_pos)
{
    debug_assert(start_bit_pos >= 0 && start_bit_pos <= 31);
    word = word & (0xFFFFFFFF << start_bit_pos);
    if (get_byte_of_word(word, 0) != 0)
        return kaps_first_bit_set[get_byte_of_word(word, 0)];
    if (get_byte_of_word(word, 1) != 0)
        return kaps_first_bit_set[get_byte_of_word(word, 1)] + 8;
    if (get_byte_of_word(word, 2) != 0)
        return kaps_first_bit_set[get_byte_of_word(word, 2)] + 16;
    return kaps_first_bit_set[get_byte_of_word(word, 3)] + 24;
}

static const int last_bit_set[256] = {
    /*
     * 0,1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 
     */
    -1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

static KAPS_INLINE int
kaps_last_set_in_word(
    uint32_t word,
    int end_bit_pos)
{
    debug_assert(end_bit_pos >= 0 && end_bit_pos <= 31);
    word = word & (0xFFFFFFFF >> (31 - end_bit_pos));
    if (get_byte_of_word(word, 3) != 0)
        return last_bit_set[get_byte_of_word(word, 3)] + 24;
    if (get_byte_of_word(word, 2) != 0)
        return last_bit_set[get_byte_of_word(word, 2)] + 16;
    if (get_byte_of_word(word, 1) != 0)
        return last_bit_set[get_byte_of_word(word, 1)] + 8;
    return last_bit_set[get_byte_of_word(word, 0)];
}

static const int kaps_bitcount[256] = {
    /*
     * 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15 
     */
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

static KAPS_INLINE int
kaps_count_set_in_word(
    uint32_t word,
    int start_bit_pos,
    int end_bit_pos)
{
    debug_assert(end_bit_pos >= 0 && end_bit_pos <= 31);
    word = word & (0xFFFFFFFF << start_bit_pos);
    word = word & (0xFFFFFFFF >> (31 - end_bit_pos));
    return kaps_bitcount[get_byte_of_word(word, 0)]
        + kaps_bitcount[get_byte_of_word(word, 1)]
        + kaps_bitcount[get_byte_of_word(word, 2)] + kaps_bitcount[get_byte_of_word(word, 3)];
}

static KAPS_INLINE int
kaps_find_nth_bit_set_in_byte(
    int byte,
    int n)
{
    int pos;

    for (pos = 0; pos <= 7; pos++)
        if (byte & (1 << pos))
        {
            n--;
            if (n == 0)
                return pos;
        }
    debug_assert(0);
    return -1;
}

int
kaps_get_bit(
    struct kaps_fast_bitmap *fbmp,
    int pos)
{
    kaps_sassert(pos < fbmp->num_access_bits);
    return kaps_get_bit_of_word(fbmp->bitmap[BMP_WORD(pos)], pos % 32);
}

void
kaps_set_bit(
    struct kaps_fast_bitmap *fbmp,
    int pos)
{
    kaps_sassert(pos < fbmp->num_access_bits);
    kaps_set_bit_of_word(fbmp->bitmap + BMP_WORD(pos), pos % 32);
    kaps_set_bit_of_word(fbmp->summary_bitmap + SUMMARY_WORD(pos), BMP_WORD(pos) % 32);
    fbmp->counts[COUNT_BYTE(pos)]++;
}

void
kaps_reset_bit(
    struct kaps_fast_bitmap *fbmp,
    int pos)
{
    kaps_sassert(pos < fbmp->num_access_bits);
    kaps_reset_bit_of_word(fbmp->bitmap + BMP_WORD(pos), pos % 32);
    if (fbmp->bitmap[BMP_WORD(pos)] == 0)
        kaps_reset_bit_of_word(fbmp->summary_bitmap + SUMMARY_WORD(pos), BMP_WORD(pos) % 32);
    fbmp->counts[COUNT_BYTE(pos)]--;
}

/** Finds first bit set assuming that end of range aligns with end of
    summary word */
static KAPS_INLINE int
kaps_find_first_bit_set_worker(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_summary)
{
    int s, start_word = BMP_WORD(start_bit_pos);
    int start_summary = SUMMARY_WORD(start_bit_pos);

    if (start_bit_pos % 1024 != 0)
    {
                /** First summary word has some start bits outside the range */
        if (start_bit_pos % 32 != 0 && CHECK_SUMMARY(fbmp, start_word))
        {
                        /** First bitmap word has some bits outside the range */
            int first_bit_set = kaps_first_set_in_word(fbmp->bitmap[start_word],
                                                       start_bit_pos % 32);
            if (first_bit_set != -1)
                return start_word * 32 + first_bit_set;
        }
        if (SUMMARY_WORD(start_bit_pos + 31) == start_summary)
        {
                        /** Some bits in first summary word fully covered by range */
            int first_summary_bit_set = kaps_first_set_in_word(fbmp->summary_bitmap[start_summary],
                                                               BMP_WORD(start_bit_pos + 31) % 32);
            if (first_summary_bit_set != -1)
                return start_summary * 1024 + first_summary_bit_set * 32 +
                    kaps_first_set_in_word(fbmp->bitmap[start_summary * 32 + first_summary_bit_set], 0);
        }
    }
    for (s = SUMMARY_WORD(start_bit_pos + 1023); s <= end_summary; s++)
    {
                /** Summary words entirely within range */
        int first_summary_bit_set = kaps_first_set_in_word(fbmp->summary_bitmap[s], 0);

        if (first_summary_bit_set != -1)
            return s * 1024 + first_summary_bit_set * 32 +
                kaps_first_set_in_word(fbmp->bitmap[s * 32 + first_summary_bit_set], 0);
    }
    return -1;
}

int
kaps_find_first_bit_set(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos)
{
    int result;

    kaps_sassert(end_bit_pos < fbmp->num_access_bits);
    result = kaps_find_first_bit_set_worker(fbmp, start_bit_pos, SUMMARY_WORD(end_bit_pos));
    return (result > end_bit_pos ? -1 : result);
}

/**
 * Finds last bit set assuming that start of range aligns with start of
 * summary word
 */

static KAPS_INLINE int
kaps_find_last_bit_set_worker(
    struct kaps_fast_bitmap *fbmp,
    int start_summary,
    int end_bit_pos)
{
    int s, end_word = BMP_WORD(end_bit_pos);
    int end_summary = SUMMARY_WORD(end_bit_pos);

    if (end_bit_pos % 1024 != 1023)
    {
                /** Last summary word has some end bits outside the range */
        if (end_bit_pos % 32 != 31 && CHECK_SUMMARY(fbmp, end_word))
        {
                        /** Last bitmap word has some bits outside the range */
            int last_bit_set = kaps_last_set_in_word(fbmp->bitmap[end_word],
                                                     end_bit_pos % 32);
            if (last_bit_set != -1)
                return end_word * 32 + last_bit_set;
        }
        if (SUMMARY_WORD(end_bit_pos - 31) == end_summary)
        {
                        /** Some bits in last summary word fully covered by range */
            int last_summary_bit_set = kaps_last_set_in_word(fbmp->summary_bitmap[end_summary],
                                                             BMP_WORD(end_bit_pos - 31) % 32);
            if (last_summary_bit_set != -1)
                return end_summary * 1024 + last_summary_bit_set * 32 +
                    kaps_last_set_in_word(fbmp->bitmap[end_summary * 32 + last_summary_bit_set], 31);
        }
    }
    for (s = SUMMARY_WORD(end_bit_pos - 1023); s >= start_summary; s--)
    {
                /** Summary words entirely within range */
        int last_summary_bit_set = kaps_last_set_in_word(fbmp->summary_bitmap[s], 31);

        if (last_summary_bit_set != -1)
            return s * 1024 + last_summary_bit_set * 32 +
                kaps_last_set_in_word(fbmp->bitmap[s * 32 + last_summary_bit_set], 31);
    }
    return -1;
}

int
kaps_find_last_bit_set(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos)
{
    int result;

    kaps_sassert(end_bit_pos < fbmp->num_access_bits);
    result = kaps_find_last_bit_set_worker(fbmp, SUMMARY_WORD(start_bit_pos), end_bit_pos);
    return (result < start_bit_pos ? -1 : result);
}

static KAPS_INLINE int
kaps_count_bits_in_bmp_word(
    struct kaps_fast_bitmap *fbmp,
    int w,
    int start_bit_pos,
    int end_bit_pos)
{
    debug_assert(start_bit_pos >= 0 && end_bit_pos <= 31 && start_bit_pos <= end_bit_pos);
    if (!CHECK_SUMMARY(fbmp, w))
        return 0;
    return kaps_count_set_in_word(fbmp->bitmap[w], start_bit_pos, end_bit_pos);
}

int
kaps_count_bits(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos)
{
    int w, b, last_full_word_before_counters;
    uint32_t count = 0;

    kaps_sassert(end_bit_pos < fbmp->num_access_bits);
    if (end_bit_pos < start_bit_pos)
        return 0;

    if (BMP_WORD(start_bit_pos) == BMP_WORD(end_bit_pos))
        return kaps_count_bits_in_bmp_word(fbmp, BMP_WORD(start_bit_pos), start_bit_pos % 32, end_bit_pos % 32);

    if (start_bit_pos % 32 != 0)
        count += kaps_count_bits_in_bmp_word(fbmp, BMP_WORD(start_bit_pos), start_bit_pos % 32, 31);

    if (COUNT_BYTE(start_bit_pos + 255) * 8 - 1 > BMP_WORD(end_bit_pos - 31))
        last_full_word_before_counters = BMP_WORD(end_bit_pos - 31);
    else
        last_full_word_before_counters = COUNT_BYTE(start_bit_pos + 255) * 8 - 1;
    for (w = BMP_WORD(start_bit_pos + 31); w <= last_full_word_before_counters; w++)
        count += kaps_count_bits_in_bmp_word(fbmp, w, 0, 31);

    for (b = COUNT_BYTE(start_bit_pos + 255); b <= COUNT_BYTE(end_bit_pos - 255); b++)
        count += BIT_COUNT(fbmp, b);

    if (COUNT_BYTE(end_bit_pos - 255) + 1 > COUNT_BYTE(start_bit_pos + 255) - 1)
        for (w = (COUNT_BYTE(end_bit_pos - 255) + 1) * 8; w <= BMP_WORD(end_bit_pos - 31); w++)
            count += kaps_count_bits_in_bmp_word(fbmp, w, 0, 31);

    if (end_bit_pos % 32 != 31)
        count += kaps_count_bits_in_bmp_word(fbmp, BMP_WORD(end_bit_pos), 0, end_bit_pos % 32);
    return count;
}

/**
 * If word has no more than *np bits set, function returns position
 * of *np-th, otherwise decrements *np by the number of bits set.
 */
static KAPS_INLINE int
kaps_find_nth_bit_set_in_word(
    struct kaps_fast_bitmap *fbmp,
    int w,
    int *np,
    int start_bit_pos)
{
    uint32_t word;
    int32_t total_bits_set;

    if (!CHECK_SUMMARY(fbmp, w))
        return -1;

    word = fbmp->bitmap[w] & (0xFFFFFFFF << start_bit_pos);
    total_bits_set = kaps_count_set_in_word(word, start_bit_pos, 31);
    if (*np > total_bits_set)
    {
        *np -= total_bits_set;
        return -1;
    }

    if (*np > kaps_bitcount[get_byte_of_word(word, 0)])
        *np -= kaps_bitcount[get_byte_of_word(word, 0)];
    else
        return kaps_find_nth_bit_set_in_byte(get_byte_of_word(word, 0), *np);

    if (*np > kaps_bitcount[get_byte_of_word(word, 1)])
        *np -= kaps_bitcount[get_byte_of_word(word, 1)];
    else
        return 8 + kaps_find_nth_bit_set_in_byte(get_byte_of_word(word, 1), *np);

    if (*np > kaps_bitcount[get_byte_of_word(word, 2)])
        *np -= kaps_bitcount[get_byte_of_word(word, 2)];
    else
        return 16 + kaps_find_nth_bit_set_in_byte(get_byte_of_word(word, 2), *np);

    return 24 + kaps_find_nth_bit_set_in_byte(get_byte_of_word(word, 3), *np);
}

static KAPS_INLINE int
kaps_find_nth_bit_worker(
    struct kaps_fast_bitmap *fbmp,
    int n,
    int start_bit_pos,
    int end_count_byte)
{
    int w, b;
    int result;

    if (start_bit_pos % 32 != 0)
    {
        result = kaps_find_nth_bit_set_in_word(fbmp, BMP_WORD(start_bit_pos), &n, start_bit_pos % 32);
        if (result != -1)
            return 32 * BMP_WORD(start_bit_pos) + result;
    }

    for (w = BMP_WORD(start_bit_pos + 31); w < COUNT_BYTE(start_bit_pos + 255) * 8; w++)
    {
        result = kaps_find_nth_bit_set_in_word(fbmp, w, &n, 0);
        if (result != -1)
            return 32 * w + result;
    }

    for (b = COUNT_BYTE(start_bit_pos + 255); b <= end_count_byte; b++)
        if (n > BIT_COUNT(fbmp, b))
            n -= BIT_COUNT(fbmp, b);
        else
            for (w = b * 8; w < b * 8 + 8; w++)
            {
                result = kaps_find_nth_bit_set_in_word(fbmp, w, &n, 0);
                if (result != -1)
                    return 32 * w + result;
            }
    kaps_sassert(0);
    return -1;
}

int
kaps_find_nth_bit(
    struct kaps_fast_bitmap *fbmp,
    int n,
    int start_bit_pos,
    int end_bit_pos)
{
    int result;

    kaps_sassert(end_bit_pos < fbmp->num_access_bits);
    debug_assert(n > 0 && n <= end_bit_pos - start_bit_pos + 1 && start_bit_pos <= end_bit_pos);
    result = kaps_find_nth_bit_worker(fbmp, n, start_bit_pos, COUNT_BYTE(end_bit_pos));
    debug_assert(result <= end_bit_pos);
    return result;
}

kaps_status
kaps_init_bmp(
    struct kaps_fast_bitmap * fbmp,
    struct kaps_allocator * alloc,
    int size,
    int are_bits_set)
{
    uint32_t num_access_bits = size;

    size = ((size + 1023) / 1024) * 1024;
    fbmp->bitmap = (uint32_t *) alloc->xcalloc(alloc->cookie, 1, (size / 8));
    if (!fbmp->bitmap)
        return KAPS_OUT_OF_MEMORY;

    fbmp->summary_bitmap = (uint32_t *) alloc->xcalloc(alloc->cookie, 1, (size / 256));
    if (!fbmp->summary_bitmap)
    {
        alloc->xfree(alloc->cookie, fbmp->bitmap);
        return KAPS_OUT_OF_MEMORY;
    }

    fbmp->counts = (uint8_t *) alloc->xcalloc(alloc->cookie, 1, (size / 256));
    if (!fbmp->counts)
    {
        alloc->xfree(alloc->cookie, fbmp->bitmap);
        alloc->xfree(alloc->cookie, fbmp->summary_bitmap);
        return KAPS_OUT_OF_MEMORY;
    }

    kaps_sassert(fbmp->bitmap != NULL && fbmp->summary_bitmap != NULL && fbmp->counts != NULL);
    /* kaps_memset(fbmp->counts, 0, size / 256);*/
    if (are_bits_set)
    {
        kaps_memset(fbmp->bitmap, 0xFF, size / 8);
        kaps_memset(fbmp->summary_bitmap, 0xFF, size / 256);
    }
    else
    {
        /* kaps_memset(fbmp->bitmap, 0, size / 8);*/
        /* kaps_memset(fbmp->summary_bitmap, 0, size / 256);*/
    }
    fbmp->num_access_bits = num_access_bits;

    return KAPS_OK;
}

void
kaps_change_bytes_for_endian(
    uint32_t * array,
    uint32_t size)
{
    uint32_t i, val;
    int endianval = 1;

    /*
     * Little Endian : don't change the bytes
     */
    if (*(char *) &endianval == 1)
        return;

    for (i = 0; i < size; i++)
    {
        val = array[i];
        array[i] = (((val >> 24) & 0xff) |
                    ((val << 8) & 0xff0000) | ((val >> 8) & 0xff00) | ((val << 24) & 0xff000000));
    }
}

uint32_t
kaps_is_subset(
    const uint8_t * set_data,
    const uint8_t * set_mask,
    const uint8_t * subset_data,
    const uint8_t * subset_mask,
    uint32_t min_pos,
    uint32_t max_pos)
{
    uint32_t next_pos, bit_pos, ind;
    uint8_t mask;
    uint8_t set_byte_mask, subset_byte_mask;
    uint8_t set_byte_data, subset_byte_data;

    kaps_sassert(max_pos >= min_pos);
    next_pos = bit_pos = min_pos;

    do
    {
        mask = ~((1 << (8 - (bit_pos & 0x7))) - 1);
        next_pos = bit_pos + (7 - (bit_pos & 0x7));
        next_pos = (next_pos > max_pos) ? max_pos : next_pos;
        mask |= (1 << (7 - (next_pos & 0x7))) - 1;
        ind = bit_pos >> 3;

        set_byte_mask = set_mask[ind] | mask;
        subset_byte_mask = subset_mask[ind] | mask;

        if ((subset_byte_mask & set_byte_mask) != subset_byte_mask)
        {
            return 0;
        }
        set_byte_data = set_data[ind] & ~set_byte_mask;
        subset_byte_data = subset_data[ind] & ~set_byte_mask;   /* the subset is masked against the larger mask */
        if (set_byte_data != subset_byte_data)
        {
            return 0;
        }
        bit_pos = next_pos + 1;
    }
    while (next_pos < max_pos);

    return 1;
}

kaps_status
kaps_update_bmp(
    struct kaps_fast_bitmap * fbmp,
    uint16_t * num_ab_slots,
    struct kaps_allocator * alloc,
    uint32_t num_ab,
    uint32_t per_ab_slots,
    uint32_t insertion_point,
    uint32_t are_bits_set)
{
    uint32_t i;
    uint32_t *bmp, *summary;
    uint8_t *counts, *t_bmp, *t_summary, *t_count;
    uint8_t *ot_bmp, *ot_summary, *ot_count;
    uint32_t new_size = num_ab * per_ab_slots;
    uint32_t num_access_bits;
    uint32_t per_ab_bits = per_ab_slots / 8;
    uint32_t per_summary_bits = per_ab_slots / 256;
    uint32_t o_ab_iter = 0, n_ab_iter = 0, o_sum_iter = 0, n_sum_iter = 0;
    uint32_t recompute_summary_bitmap = 0;

    num_access_bits = new_size;
    new_size = ((new_size + 1023) / 1024) * 1024;

    bmp = (uint32_t *) alloc->xcalloc(alloc->cookie, 1, (new_size / 8));
    if (!bmp)
        return KAPS_OUT_OF_MEMORY;

    summary = (uint32_t *) alloc->xcalloc(alloc->cookie, 1, (new_size / 256));
    if (!summary)
    {
        alloc->xfree(alloc->cookie, bmp);
        return KAPS_OUT_OF_MEMORY;
    }

    counts = (uint8_t *) alloc->xcalloc(alloc->cookie, 1, (new_size / 256));
    if (!counts)
    {
        alloc->xfree(alloc->cookie, bmp);
        alloc->xfree(alloc->cookie, summary);
        return KAPS_OUT_OF_MEMORY;
    }

    o_ab_iter = (fbmp->num_access_bits + 31) / 32;
    o_sum_iter = (fbmp->num_access_bits + 1023) / 1024;
    n_ab_iter = (new_size + 31) / 32;
    n_sum_iter = (new_size + 1023) / 1024;

    /*
     * Set bits to defaults 
     */
    kaps_memset(counts, 0, new_size / 256);
    if (are_bits_set)
    {
        kaps_memset(bmp, 0xFF, new_size / 8);
        kaps_memset(summary, 0xFF, new_size / 256);
    }
    else
    {
        kaps_memset(bmp, 0, new_size / 8);
        kaps_memset(summary, 0, new_size / 256);
    }

    /*
     * copy all the bits from old bitmaps, to new bitmaps, and create
     * default initialized bitmap hole at the insertion point
     */

    t_bmp = (uint8_t *) bmp;
    kaps_change_bytes_for_endian(fbmp->bitmap, o_ab_iter);
    ot_bmp = (uint8_t *) fbmp->bitmap;

    t_summary = (uint8_t *) summary;
    kaps_change_bytes_for_endian(fbmp->summary_bitmap, o_sum_iter);
    ot_summary = (uint8_t *) fbmp->summary_bitmap;

    t_count = counts;
    ot_count = fbmp->counts;

    if (per_summary_bits == 0)
        recompute_summary_bitmap = 1;

    if ((per_ab_slots / 2) >= 8)
    {
        for (i = 0; i < insertion_point; i++)
        {
            if (num_ab_slots)
            {
                per_ab_bits = num_ab_slots[i] / 8;
                per_summary_bits = num_ab_slots[i] / 256;
                if (per_summary_bits == 0)
                    recompute_summary_bitmap = 1;
            }
            kaps_memcpy(t_bmp, ot_bmp, per_ab_bits);
            kaps_memcpy(t_summary, ot_summary, per_summary_bits);
            kaps_memcpy(t_count, ot_count, per_summary_bits);
            t_bmp += per_ab_bits;
            ot_bmp += per_ab_bits;
            t_summary += per_summary_bits;
            ot_summary += per_summary_bits;
            t_count += per_summary_bits;
            ot_count += per_summary_bits;
        }

        /*
         * Skip the insertion point 
         */
        if (num_ab_slots)
        {
            per_ab_bits = num_ab_slots[insertion_point] / 8;
            per_summary_bits = num_ab_slots[insertion_point] / 256;
            if (per_summary_bits == 0)
                recompute_summary_bitmap = 1;
        }
        t_bmp += per_ab_bits;
        t_summary += per_summary_bits;
        t_count += per_summary_bits;

        /*
         * Add the remaining bits after the insertion point 
         */
        for (i = insertion_point + 1; i < num_ab; i++)
        {
            if (num_ab_slots)
            {
                per_ab_bits = num_ab_slots[i] / 8;
                per_summary_bits = num_ab_slots[i] / 256;
                if (per_summary_bits == 0)
                    recompute_summary_bitmap = 1;
            }
            kaps_memcpy(t_bmp, ot_bmp, per_ab_bits);
            kaps_memcpy(t_summary, ot_summary, per_summary_bits);
            kaps_memcpy(t_count, ot_count, per_summary_bits);
            t_bmp += per_ab_bits;
            ot_bmp += per_ab_bits;
            t_summary += per_summary_bits;
            ot_summary += per_summary_bits;
            t_count += per_summary_bits;
            ot_count += per_summary_bits;
        }
    }
    else
    {
        uint32_t num_slots = 0;
        int32_t src_pos = 0;
        int32_t dst_pos = 0;

        recompute_summary_bitmap = 1;
        for (i = 0; i < num_ab; i++)
        {
            if (num_ab_slots)
                num_slots = num_ab_slots[i];
            else
                num_slots = per_ab_slots;

            if (i == insertion_point)
            {
                dst_pos += num_slots;
                continue;
            }

            while (num_slots)
            {
                if (kaps_get_bit_of_word(((uint32_t *) ot_bmp)[BMP_WORD(src_pos)], src_pos % 32))
                    kaps_set_bit_of_word((uint32_t *) t_bmp + BMP_WORD(dst_pos), dst_pos % 32);
                else
                    kaps_reset_bit_of_word((uint32_t *) t_bmp + BMP_WORD(dst_pos), dst_pos % 32);
                src_pos++;
                dst_pos++;
                num_slots--;
            }
        }
    }

    alloc->xfree(alloc->cookie, fbmp->bitmap);
    alloc->xfree(alloc->cookie, fbmp->summary_bitmap);
    alloc->xfree(alloc->cookie, fbmp->counts);
    fbmp->bitmap = bmp;
    kaps_change_bytes_for_endian(fbmp->bitmap, n_ab_iter);
    fbmp->summary_bitmap = summary;
    kaps_change_bytes_for_endian(fbmp->summary_bitmap, n_sum_iter);

    fbmp->counts = counts;
    fbmp->num_access_bits = num_access_bits;

    /*
     * When the number of rows in AB is less than 256 and if we insert any AB in between we need to shift the summary
     * bit-map manually, since it requires less than 8-bits to represent one AB 
     */
    if (recompute_summary_bitmap)
    {
        int pos;

        /*
         * Resetting summary_bitmap & counts 
         */
        kaps_memset(fbmp->counts, 0, new_size / 256);
        if (are_bits_set)
            kaps_memset(fbmp->summary_bitmap, 0xFF, new_size / 256);
        else
            kaps_memset(fbmp->summary_bitmap, 0, new_size / 256);

        /*
         * Computing summary_bitmap & counts 
         */
        for (pos = 0; pos < fbmp->num_access_bits; pos++)
        {
            if (fbmp->bitmap[BMP_WORD(pos)] == 0)
                kaps_reset_bit_of_word(fbmp->summary_bitmap + SUMMARY_WORD(pos), BMP_WORD(pos) % 32);
            if (kaps_get_bit(fbmp, pos))
                fbmp->counts[COUNT_BYTE(pos)]++;
        }
    }

    return KAPS_OK;
}

uint32_t
kaps_fbmp_get_first_n_bits_set(
    struct kaps_fast_bitmap * fbmp,
    int32_t n_bits,
    uint32_t * result_bit_pos)
{
    uint32_t num_words = fbmp->num_access_bits / 32;
    uint32_t word_index;
    uint32_t i;
    int32_t num_bits_found;
    uint32_t start_bit_pos;

    start_bit_pos = 0;
    num_bits_found = 0;
    word_index = 0;
    while (word_index < num_words)
    {
        if (fbmp->bitmap[word_index] == 0)
        {
            ++word_index;
            num_bits_found = 0;
            start_bit_pos = word_index * 32;
            continue;
        }

        for (i = 0; i < 32; ++i)
        {
            if ((fbmp->bitmap[word_index] >> i) & 1)
            {
                num_bits_found++;
                if (num_bits_found == 1)
                {
                    start_bit_pos = (word_index * 32) + i;
                }

                if (num_bits_found == n_bits)
                {
                    *result_bit_pos = start_bit_pos;
                    return 1;
                }

            }
            else
            {
                num_bits_found = 0;
            }
        }

        ++word_index;
    }

    return 0;
}

void
kaps_free_bmp(
    struct kaps_fast_bitmap *fbmp,
    struct kaps_allocator *alloc)
{
    if (fbmp->bitmap)
        alloc->xfree(alloc->cookie, fbmp->bitmap);

    if (fbmp->summary_bitmap)
        alloc->xfree(alloc->cookie, fbmp->summary_bitmap);

    if (fbmp->counts)
        alloc->xfree(alloc->cookie, fbmp->counts);
}

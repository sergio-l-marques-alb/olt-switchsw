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

#ifndef __KAPS_BITMAP_H
#define __KAPS_BITMAP_H

#include "kaps_allocator.h"
#include "kaps_hw_limits.h"
#include "kaps_portable.h"
#include "kaps_errors.h"
#include "kaps_utility.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @cond INTERNAL
 *
 * @file bitmap.h
 *
 * Fast bitmap implementation
 */

/**
 * @brief Ternary bit values
 */

    enum kaps_bit_type
    {
        KAPS_ZERO = 0,              /**< Represents zero in HW */
        KAPS_ONE = 1,               /**< Represents one in HW */
        KAPS_DONT_CARE = 2          /**< Represents dont care in HW */
    };

/**
 * Structure representing fast bitmap management.
 * If the number of bits specified is not a multiple of 1024, then internally
 * the space is allocated for the nearest multiple of 1024 bits. However only
 * the user should access only those bits specified during the bitmap initialization
 */

    struct kaps_fast_bitmap
    {
        uint32_t *bitmap;      /**< Array to store bits */
        uint32_t *summary_bitmap;
                               /**< Bit n in summary bitmap is set if
                                  and only if bitmap[n] != 0 . */
        uint8_t *counts;       /**< count[n] has the number of bits set
                                  in the n+1th group of 256 bits. We use
                                  the relevant byte of summary_bitmap to
                                  disambiguate between 0 and 256. */
        int32_t num_access_bits;
                              /**< Number of bits that can be accessed
                                  in the bitmap */
    };

    int kaps_get_bit(
    struct kaps_fast_bitmap *fbmp,
    int pos);

    void kaps_set_bit(
    struct kaps_fast_bitmap *fbmp,
    int pos);

    void kaps_reset_bit(
    struct kaps_fast_bitmap *fbmp,
    int pos);

    int kaps_find_first_bit_set(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos);

    int kaps_find_last_bit_set(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos);

/*Returns 1 if found and the result_bit_pos will contain the first bit that is set (smaller numerical value)*/
    uint32_t kaps_fbmp_get_first_n_bits_set(
    struct kaps_fast_bitmap *fbmp,
    int32_t n_bits,
    uint32_t * result_bit_pos);

    int kaps_count_bits(
    struct kaps_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos);

    int kaps_find_nth_bit(
    struct kaps_fast_bitmap *fbmp,
    int n,
    int start_bit_pos,
    int end_bit_pos);

    kaps_status kaps_init_bmp(
    struct kaps_fast_bitmap *fbmp,
    struct kaps_allocator *alloc,
    int size,
    int are_bits_set);

    kaps_status kaps_update_bmp(
    struct kaps_fast_bitmap *fbmp,
    uint16_t * num_ab_slots,
    struct kaps_allocator *alloc,
    uint32_t num_ab,
    uint32_t per_ab_slots,
    uint32_t insertion_point,
    uint32_t are_bits_set);

    void kaps_free_bmp(
    struct kaps_fast_bitmap *fbmp,
    struct kaps_allocator *alloc);

/**
 * utility to retrieve bit as an abstracted enumeration value
 *
 * @param data data array with ones and zeros
 * @param mask corresponding data mask with 1 == dont care
 * @param bitno the bit to examine
 *
 * @return ::kaps_bit_type
 */

    static KAPS_INLINE enum kaps_bit_type kaps_array_get_bit(
    const uint8_t * data,
    const uint8_t * mask,
    uint32_t bitno)
    {
        uint32_t byte_index = bitno / KAPS_BITS_IN_BYTE;
        uint32_t bit_index = 7 - (bitno - (byte_index * KAPS_BITS_IN_BYTE));

        if  (
    mask[byte_index] & (1 << bit_index))
                return KAPS_DONT_CARE;
        if  (
    data[byte_index] & (1 << bit_index))
                return KAPS_ONE;
            return KAPS_ZERO;
    }

/**
 * utility to check whether a given data-mask pair is a
 * subset of another data-mask pair
 * Example *11001* is a subset of       **1*01*
 *         *11001* isn't a subset of    *01*01*
 *
 * @param set_data the less specified data (superset).
 * @param set_mask the less specified mask (superset).
 * @param subset_data the more specified data.
 * @param subset_mask the more specified mask.
 * @param min_pos the bit position to start compare from
 * @param max_pos the bit position to compare until (inclusive)
 *
 * @return 1 when bit is set, 0 when it is not
 */
    uint32_t kaps_is_subset(
    const uint8_t * set_data,
    const uint8_t * set_mask,
    const uint8_t * subset_data,
    const uint8_t * subset_mask,
    uint32_t min_pos,
    uint32_t max_pos);

/**
 * utility to check whether bit is set in array of bytes
 *
 * @param array the data array
 * @param bitno position of bit to examine
 *
 * @return 1 when bit is set, 0 when it is not
 */
    static KAPS_INLINE int kaps_array_check_bit(
    const uint8_t * array,
    uint32_t bitno)
    {
        uint32_t byte_index, bit_index;

        kaps_sassert(array != NULL);

        byte_index = bitno / KAPS_BITS_IN_BYTE;
        if (array[byte_index] == 0)
            return 0;

        bit_index = 7 - (bitno - (byte_index * KAPS_BITS_IN_BYTE));

        if (array[byte_index] & (1 << bit_index))
            return 1;
        else
            return 0;
    }

/**
 * Set the specific bit in the data array with the specified value
 *
 * @param array the data array
 * @param bitno the bit position to set
 * @param value one or zero to set in the data array
 */

    static KAPS_INLINE void kaps_array_set_bit(
    uint8_t * array,
    uint32_t bitno,
    uint8_t value)
    {
        uint32_t byte_index = bitno / KAPS_BITS_IN_BYTE;
        uint32_t bit_index = 7 - (bitno - (byte_index * KAPS_BITS_IN_BYTE));

        if (value == 1)
            array[byte_index] |= (1 << bit_index);
        else
            array[byte_index] &= ~(1 << bit_index);
    }

/**
 * Get the index of the first bit which has the specified value
 *
 * @param array the data array
 * @param value one or zero which is to be searched
 * @param bitmap_size_in_bits size of the data array in bits
 */

    static KAPS_INLINE int32_t kaps_array_get_first_bit_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        uint32_t byte_index;
        int8_t bit_index;

        for (byte_index = 0; byte_index < bitmap_size_in_bytes; byte_index++)
        {
            if (((value == 0) && (array[byte_index] != 0xFF)) || ((value == 1) && (array[byte_index] != 0)))
            {
                break;
            }
        }

        if (byte_index == bitmap_size_in_bytes)
        {
            return bitmap_size_in_bits;
        }

        for (bit_index = KAPS_BITS_IN_BYTE - 1; bit_index >= 0; bit_index--)
        {
            if (((array[byte_index] >> bit_index) & 1) == value)
            {
                return (byte_index * KAPS_BITS_IN_BYTE + 7 - bit_index);
            }
        }
        return bitmap_size_in_bits;
    }

/**
 * Get the index of the first bit which has the specified value
 * in consecutive n bits
 *
 * @param array the data array
 * @param value one or zero which is to be searched
 * @param bitmap_size_in_bits size of the data array in bits
 */
    static KAPS_INLINE int32_t kaps_array_get_first_n_bits_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t n_bits,
    int32_t n_bit_align,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        uint32_t byte_index;
        int8_t bit_index;
        int32_t found_num_bit_count;
        int32_t bit_align_count = 0;

        found_num_bit_count = 0;
        for (byte_index = 0; byte_index < bitmap_size_in_bytes; byte_index++)
        {
            if (((value == 0) && (array[byte_index] == 0xFF)) || ((value == 1) && (array[byte_index] == 0)))
            {
                found_num_bit_count = 0;
                continue;
            }

            bit_align_count = 0;
            if (n_bit_align > 1)
                found_num_bit_count = 0;
            for (bit_index = KAPS_BITS_IN_BYTE - 1; bit_index >= 0; bit_index--)
            {
                if (n_bit_align > 1)
                {
                    if (bit_align_count == n_bit_align)
                    {
                        found_num_bit_count = 0;
                        bit_align_count = 0;
                    }
                    else
                    {
                        kaps_sassert(bit_align_count < n_bit_align);
                    }
                }
                if (((array[byte_index] >> bit_index) & 1) == value)
                {
                    found_num_bit_count++;
                    if (found_num_bit_count == n_bits)
                        return (byte_index * KAPS_BITS_IN_BYTE + 7 - bit_index) - (n_bits - 1);
                }
                else
                {
                    found_num_bit_count = 0;
                }

                bit_align_count++;
            }
        }

        return bitmap_size_in_bits;
    }

/**
 * Get the index of the last bit which has the specified value
 *
 * @param array the data array
 * @param value one or zero which is to be searched
 * @param bitmap_size_in_bits size of the data array in bits
 */

    static KAPS_INLINE int32_t kaps_array_get_last_bit_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        int32_t byte_index;
        int8_t bit_index;

        for (byte_index = bitmap_size_in_bytes - 1; byte_index >= 0; byte_index--)
        {
            if (((value == 0) && (array[byte_index] != 0xFF)) || ((value == 1) && (array[byte_index] != 0)))
            {
                break;
            }
        }

        if (byte_index < 0)
        {
            return bitmap_size_in_bits;
        }

        for (bit_index = 0; bit_index < KAPS_BITS_IN_BYTE; bit_index++)
        {
            if (((array[byte_index] >> bit_index) & 1) == value)
            {
                return (byte_index * KAPS_BITS_IN_BYTE + 7 - bit_index);
            }
        }
        return bitmap_size_in_bits;
    }

/**
 * Get the index of the last bit which has the specified value
 * in consecutive n bits
 *
 * @param array the data array
 * @param value one or zero which is to be searched
 * @param bitmap_size_in_bits size of the data array in bits
 */
    static KAPS_INLINE int32_t kaps_array_get_last_n_bits_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t n_bits,
    int32_t n_bit_align,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        int32_t byte_index;
        int8_t bit_index;
        int32_t found_num_bit_count;
        int32_t bit_align_count = 0;

        found_num_bit_count = 0;
        for (byte_index = bitmap_size_in_bytes - 1; byte_index >= 0; byte_index--)
        {
            if (((value == 0) && (array[byte_index] == 0xFF)) || ((value == 1) && (array[byte_index] == 0)))
            {
                found_num_bit_count = 0;
                continue;
            }

            bit_align_count = 0;
            for (bit_index = 0; bit_index < KAPS_BITS_IN_BYTE; bit_index++)
            {
                if (n_bit_align > 1)
                {
                    if (bit_align_count == n_bit_align)
                    {
                        found_num_bit_count = 0;
                        bit_align_count = 0;
                    }
                    else
                    {
                        kaps_sassert(bit_align_count < n_bit_align);
                    }
                }
                if (((array[byte_index] >> bit_index) & 1) == value)
                {
                    found_num_bit_count++;
                    if (found_num_bit_count == n_bits)
                        return (byte_index * KAPS_BITS_IN_BYTE + 7 - bit_index);
                }
                else
                {
                    found_num_bit_count = 0;
                }
                bit_align_count++;
            }
        }

        return bitmap_size_in_bits;
    }

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif

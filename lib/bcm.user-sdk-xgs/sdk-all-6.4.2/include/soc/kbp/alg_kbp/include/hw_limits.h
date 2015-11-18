/*******************************************************************************
 *
 * Copyright 2012-2014 Broadcom Corporation
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

#ifndef __HW_LIMITS_H
#define __HW_LIMITS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @cond INTERNAL
 *
 * @file hw_limits.h
 *
 * The limits of the various HW pieces. These defines must only
 * be used in places that want to pre-define arrays in
 * data structures. The sizes here generally depict the largest
 * possible sizes across all devices. Additionally, some
 * constants of the device that have never changed across any
 * class of of our devices are pre-defined here.
 *
 * All device specific information must be derived from the device handle
 *
 */

/**
 * Macro for adjusting value to the smallest multiple of align that is greater than value
 * Align should be a power of 2
 */
#define ALIGN_UP(value, align) (((value) + ((align) - 1)) & ~((align) - 1))

/* ==================================================
 * These values are fixed across all chip lines, hence
 * can be used as constants
 */

/**
 * Number of bits in byte
 */

#define KBP_BITS_IN_BYTE 8

/**
 * Width of LPU in bits. This value is
 * fixed across all chip lines
 */

#define KBP_HW_LPU_WIDTH_1 1024

/**
 * Width of LPU in bytes
 */

#define KBP_HW_LPU_WIDTH_8 (KBP_HW_LPU_WIDTH_1 / KBP_BITS_IN_BYTE)

/**
 * Maximum number of key processing units
 */

#define KBP_HW_MAX_KPUS 4

/**
 * Maximum number of databases that can be searched by
 * single instruction. (compare3/op)
 */

#define KBP_HW_MAX_SEARCH_DB 8

/**
 * Maximum number of cycles an instruction takes (compare3)
 */

#define KBP_MAX_INSTRUCTION_CYCLES 2

/**
 * Maximum number of cascaded devices
 */
#define KBP_MAX_CASCADED_DEVICES 4


/**
 *Number of Index range bits to shift 
 */

#define KBP_NUMBER_OF_INDEX_BITS_TO_SHIFT     512


/* ==================================================
 * These values represent the max limits for pre-defining
 * data structure arrays etc.
 */

/**
 * Maximum number of LSN cycles
 */

#define KBP_HW_MAX_NUM_LSN_CYCLES 2

/**
 * Max possible range comparisons per database
 */

#define KBP_HW_MAX_RANGE_COMPARES 4

/**
 * Maximum decision trees for any database
 */

#define KBP_HW_MAX_DT_PER_DB 2

/**
 * Max DBA entry width in bits
 */

#define KBP_HW_MAX_DBA_WIDTH_1 640

/**
 * Max DBA entry width in bytes
 */

#define KBP_HW_MAX_DBA_WIDTH_8 (KBP_HW_MAX_DBA_WIDTH_1 / KBP_BITS_IN_BYTE)


/**
 * Min DBA entry width in bits
 */

#define KBP_HW_MIN_DBA_WIDTH_1 80

/**
 * Min DBA entry width in bytes
 */

#define KBP_HW_MIN_DBA_WIDTH_8 (KBP_HW_MIN_DBA_WIDTH_1 / KBP_BITS_IN_BYTE)


/**
 * Max AB Depth
 */

#define KBP_HW_MAX_AB_DEPTH 4096

/**
 * Max UDA entry width in bits
 */
#define KBP_HW_MAX_UDA_WIDTH_1 256

/**
 * Max UDA entry width in bytes
 */
#define KBP_HW_MAX_UDA_WIDTH_8 (KBP_HW_MAX_UDA_WIDTH_1 / KBP_BITS_IN_BYTE)

/**
 * Min UDA entry width in bits
 */
#define KBP_HW_MIN_UDA_WIDTH_1 32

/**
 * Min UDA entry width in bytes
 */
#define KBP_HW_MIN_UDA_WIDTH_8 (KBP_HW_MIN_UDA_WIDTH_1 / KBP_BITS_IN_BYTE)

/**
 * Maximum indirection pointers
 */

#define KBP_HW_MAX_INDIRECTION_PTR 2

/**
 * Max. number of metadata records per LPU
 */

#define KBP_HW_MAX_METADATA_RECORDS_PER_LPU 16

/**
 * Max. number of comparators in LPU
 */

#define KBP_HW_MAX_COMPARATORS_PER_LPU 43

/**
 * Max number of LPUs per database
 */

#define KBP_HW_MAX_LPUS_PER_DB 16

/**
 * Max number of LPUs per ACL database
 */

#define KBP_HW_MAX_LPUS_PER_ACL_DB 8

/**
 * Max number of LPUs per LPM database
 */
#define KBP_HW_MAX_LPUS_PER_LPM_DB 16


/**
 *  Maximum number of LPU granularity values possible
 */
#define KBP_HW_MAX_NUM_LPU_GRAN 21


/**
 * Max DBA blocks
 */

#define KBP_HW_MAX_DBA_BLOCKS 256

/**
 * Number of Super blocks per LPU
 */

#define KBP_HW_NUM_SB_PER_LPU 4

/**
 * Minimum priority (maximum value) supported by the device
 */
#define KBP_HW_MINIMUM_PRIORITY (0x3FFFFF)

/**
 * Length of the priority supported by the device in bits
 */
#define KBP_HW_PRIORITY_SIZE_1 (24)

/**
 * Length of the priority supported by the device in bytes
 */
#define KBP_HW_PRIORITY_SIZE_8 (KBP_HW_PRIORITY_SIZE_1 / KBP_BITS_IN_BYTE)

/**
 * Maximum number AB's ever
 */

#define KBP_HW_MAX_AB 256

/**
 * Maximum number Super Blocks's ever
 */

#define KBP_HW_MAX_DBA_SB 64

/**
 * Maximum number of LTR in single port mode
 */

#define KBP_HW_MAX_LTR_SINGLE_PORT 128

/**
 * Maximum number of LTR in dual port mode
 */
#define KBP_HW_MAX_LTR_DUAL_PORT 64

/**
 * Maximum number of PCM blocks
 */
#define HW_MAX_PCM_BLOCKS 16

/**
 * Maximum number of PC entries
 */
#define KBP_HW_MAX_PCM_ENTRIES 1024

/**
 * Number of bits reserved for DB ID
 */
#define KBP_HW_MAX_PCM_DB_ID_BITS 4

/**
 * Max PC database ID limit
 */
#define KBP_HW_MAX_PCM_DB_ID 15

/**
 * Massively parallel database ID
 */
#define KBP_HW_MP_PCM_DB_ID 0

/**
 * Usable database ID
 */
#define KBP_HW_START_PCM_DB_ID (KBP_HW_MP_PCM_DB_ID + 1)

/**
 * Maximum possible UDA super blocks
 */
#define KBP_HW_MAX_UDA_SB 256

/**
 * Max Width in bits of the LPM Key
 */
#define KBP_LPM_KEY_MAX_WIDTH_1 (160)


/**
 * Max Width in bytes of the LPM Key
 */
#define KBP_LPM_KEY_MAX_WIDTH_8 (20)


/**
 * Minimum LPM Granularity
 */
#define KBP_HW_LPM_SMALLEST_GRAN    (8)


/**
 * Maximum LPM Granularity
 */
#define KBP_HW_LPM_MAX_GRAN (168)

/**
 * Maximum width of hit index returned as part of
 * result.
 */
#define KBP_HW_HIT_INDEX_WIDTH_1 24

/**
 * Maximum number of range units
 */
#define HW_MAX_RANGE_UNITS 16

/**
 * UDM size in Mb
 */
#define KBP_HW_MAX_UDM_SIZE_MB 8

/**
 * UDA SB size in Mb
 */
#define KBP_HW_MAX_UDA_SB_SIZE_MB 2

/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif                          /* __HW_LIMITS_H */

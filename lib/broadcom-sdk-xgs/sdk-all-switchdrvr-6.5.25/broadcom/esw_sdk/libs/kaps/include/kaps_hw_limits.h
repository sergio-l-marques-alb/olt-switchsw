/*******************************************************************************
 *
 * Copyright 2012-2019 Broadcom Corporation
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

#ifndef __KAPS_HW_LIMITS_H
#define __KAPS_HW_LIMITS_H

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
 * class of our devices are pre-defined here.
 *
 * All device specific information must be derived from the device handle
 *
 */

/**
 * Macro for adjusting value to the smallest multiple of align that is greater than value
 * align should be a power of 2
 */
#define ALIGN_UP(value, align) (((value) + ((align) - 1)) & ~((align) - 1))

/* ==================================================
 * These values are fixed across all chip lines, hence
 * can be used as constants
 */

/**
 * Number of bits in byte
 */

#define KAPS_BITS_IN_BYTE (8)

/**
 * Maximum number of key processing units
 */

#define KAPS_HW_MAX_KPUS (4)

/**
 * Maximum number of databases that can be searched by
 * single instruction. (compare3/op)
 */

#define KAPS_HW_MAX_SEARCH_DB (8)

/**
 * Maximum number of cycles an instruction takes (compare3)
 */

#define KAPS_MAX_INSTRUCTION_CYCLES (2)

/**
 * Maximum number of cascaded devices
 */
#define KAPS_MAX_CASCADED_DEVICES (4)

/**
 * Maximum number of broadcast devices
 */
#define KAPS_MAX_BROADCAST_DEVICES (8)


/**
 * Maximum number of soft parity error fifo depth : O3S
 */
#define KAPS_MAX_PARITY_FIFO_DEPTH (16)

/**
 * Maximum number of soft parity error fifo depth : OPrime
 */
#define KAPS_OP_MAX_PARITY_FIFO_DEPTH (32)

/**
 * Maximum number of parity error fifo place holder depth
 */
#define KAPS_MAX_PLACE_HOLDER_PARITY_FIFO_DEPTH (512)

/**
 * Maximum number of interface errors fifo depth
 */
#define KAPS_MAX_INTF_ERROR_FIFO_DEPTH (485)

/**
 *Number of Index range bits to shift
 */

#define KAPS_NUMBER_OF_INDEX_BITS_TO_SHIFT (512)


/* ==================================================
 * These values represent the max limits for pre-defining
 * data structure arrays etc.
 */

/**
 * Max possible range comparisons per database
 */

#define KAPS_HW_MAX_RANGE_COMPARES (4)


/**
 * Max number of Root Pivot Blocks
 */
#define KAPS_HW_MAX_NUM_RPB_BLOCKS (4)

/**
 * Maximum number of ADS2 blocks
 */
#define KAPS_HW_MAX_NUM_ADS2_BLOCKS (2)




/**
 * Max DBA entry width in bits
 */

#define KAPS_HW_MAX_DBA_WIDTH_1 (640)

/**
 * Max DBA entry width in bytes
 */

#define KAPS_HW_MAX_DBA_WIDTH_8 (KAPS_HW_MAX_DBA_WIDTH_1 / KAPS_BITS_IN_BYTE)


/**
 * Min DBA entry width in bits
 */

#define KAPS_HW_MIN_DBA_WIDTH_1 (80)

/**
 * Min DBA entry width in bytes
 */

#define KAPS_HW_MIN_DBA_WIDTH_8 (KAPS_HW_MIN_DBA_WIDTH_1 / KAPS_BITS_IN_BYTE)


/**
 * Max AB Depth
 */

#define KAPS_HW_MAX_AB_DEPTH (4096)

/**
 * Max UDA entry width in bits
 */
#define KAPS_HW_MAX_UDA_WIDTH_1 (256)

/**
 * Max UDA entry width in bytes
 */
#define KAPS_HW_MAX_UDA_WIDTH_8 (KAPS_HW_MAX_UDA_WIDTH_1 / KAPS_BITS_IN_BYTE)

/**
 * Min UDA entry width in bits
 */
#define KAPS_HW_MIN_UDA_WIDTH_1 (32)

/**
 * Min UDA entry width in bytes
 */
#define KAPS_HW_MIN_UDA_WIDTH_8 (KAPS_HW_MIN_UDA_WIDTH_1 / KAPS_BITS_IN_BYTE)

/**
 * Max DBA blocks
 */

#define KAPS_HW_MAX_DBA_BLOCKS (256)

/**
 * Number of Super blocks per UDM
 */

#define KAPS_HW_NUM_SB_PER_UDM (4)

/**
 * Minimum priority (maximum value) supported by the device
 */
#define KAPS_HW_MINIMUM_PRIORITY (0x3FFFFF)

/**
 * Length of the priority supported by the device in bits
 */
#define KAPS_HW_PRIORITY_SIZE_1 (24)

/**
 * Length of the priority supported by the device in bytes
 */
#define KAPS_HW_PRIORITY_SIZE_8 (KAPS_HW_PRIORITY_SIZE_1 / KAPS_BITS_IN_BYTE)

/**
 * Maximum number of ABs ever
 */

#define KAPS_HW_MAX_AB (256)

/**
 * Maximum number of Super Blocks ever
 */

#define KAPS_HW_MAX_DBA_SB (64)

/**
 * Maximum number of LTR in single port mode
 */

#define KAPS_HW_MAX_LTRS (128)

/**
 * Maximum number of LTR in dual port mode
 */
#define KAPS_HW_MAX_LTR_DUAL_PORT (64)

/**
 * Maximum possible UDA super blocks
 */
#define KAPS_HW_MAX_UDA_SB (256)


/**
 * Max Width in bits of the LPM Key
 */
#define KAPS_LPM_KEY_MAX_WIDTH_1 (160)

/**
 * Max Width in bytes of the LPM Key
 */
#define KAPS_LPM_KEY_MAX_WIDTH_8 (20)


#define KAPS_HW_MAX_SEARCH_KEY_WIDTH_1 (1280)

#define KAPS_HW_MAX_SEARCH_KEY_WIDTH_8 (160)


/**
 * Maximum width of hit index returned as part of
 * result.
 */
#define KAPS_HW_HIT_INDEX_WIDTH_1 (24)

/**
 * Maximum number of range units
 */
#define HW_MAX_RANGE_UNITS (16)

/**
 * UDM size in Mb
 */
#define KAPS_HW_MAX_UDM_SIZE_MB (8)



/**
 * maximum BMRs used for larger widths (greater than 320b)
 */
 #define KAPS_MAX_BMR_USED_FOR_LARGER_WIDTH (3)

 /**
  * maximum meta priority value
  */
 #define KAPS_MAX_META_PRIORITY_VALUE (3)

 

 /**
  * Maximum number of segments per KPU
  */

 #define KAPS_MAX_NUM_OF_SEGMENTS_PER_KEY   (15)


/**
 * Maximum number of Hit Bit Blocks
 */
 #define KAPS_MAX_NUM_HB_BLOCKS              (28)


/**
 * Maximum Number of Cores
 */
#define KAPS_MAX_NUM_CORES                   (2)



#define KAPS_UDM_PER_UDC                     (4)


#define KAPS_NUM_DB_PER_DEVICE               (16)




#define KAPS_MAX_UDCS                        (16)


#define KAPS_MAX_NUM_DEVICES                 (4)


#define KAPS_FIB_MAX_INDEX_RANGE             (0xFFFFFF)


#define KAPS_ALLOC_UDM_MAX_COLS             (28)


/**
 * Maximum decision trees for any database
 */
#define KAPS_HW_MAX_DT_PER_DB (2)



/**
 * Maximum number of PCM blocks
 */
#define HW_MAX_PCM_BLOCKS (16)


/**
 * Width of LPU in bits. This value is
 * fixed across all chip lines
 */
#define KAPS_HW_LPU_WIDTH_1 (1024)

/**
 * Width of LPU in bytes
 */
#define KAPS_HW_LPU_WIDTH_8 (KAPS_HW_LPU_WIDTH_1 / KAPS_BITS_IN_BYTE)

/* ==================================================
 * These values represent the max limits for pre-defining
 * data structure arrays etc.
 */

/**
 * Maximum number of LSN cycles
 */
#define KAPS_HW_MAX_NUM_LSN_CYCLES (2)



/**
 * Maximum DBA decision trees for any database
 */
#define KAPS_HW_MAX_DBA_DT_PER_DB (2)

/**
 * Max. number of metadata records per LPU
 */
#define KAPS_HW_MAX_METADATA_RECORDS_PER_LPU (16)

/**
 * Max. number of comparators in LPU
 */
#define KAPS_HW_MAX_COMPARATORS_PER_LPU (43)

/**
 * Max number of LPUs per database
 */
#define KAPS_HW_MAX_LPUS_PER_DB (16)

/**
 * Max number of LPUs per LPM database
 */
#define KAPS_HW_MAX_LPUS_PER_LPM_DB (16)



/**
 * Max number of database groups
 */

#define KAPS_HW_MAX_NUM_DB_GROUPS (1)


#define KAPS_MAX_NUM_DAISY_CHAINS (1)


/**
 *  Maximum number of LPU granularity values possible
 */
#define KAPS_HW_MAX_NUM_LPU_GRAN (21)



/**
 * Maximum number of PCM entries
 */
#define KAPS_HW_MAX_PCM_ENTRIES (1024)


/**
 * Minimum LPM Granularity
 */
#define KAPS_HW_LPM_SMALLEST_GRAN    (8)

/**
 * Maximum LPM Granularity
 */
#define KAPS_HW_LPM_MAX_GRAN (168)

/**
 * Maximum LPUs per LSN
 */
#define KAPS_HW_MAX_LPUS_PER_LSN (8)




/**
 * Maximum Small BB per database group in the hardware
 */
#define KAPS_HW_MAX_NUM_SMALL_BB_PER_DB_GROUP (16)


/**
 * Maximum number of rows in Small BB 
 */
#define KAPS_HW_MAX_NUM_ROWS_IN_SMALL_BB (512)



#define MAX_RPT_LOPOFF_IN_BYTES (16)


/**
 * @endcond
 */

#ifdef __cplusplus
}
#endif
#endif                          /* __HW_LIMITS_H */

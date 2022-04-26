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

#ifndef __KAPS_AD_INTERNAL_H
#define __KAPS_AD_INTERNAL_H

#include "kaps_errors.h"
#include "kaps_bitmap.h"
#include "kaps_hw_limits.h"
#include "kaps_device_internal.h"
#include "kaps_list.h"
#include "kaps_ad.h"
#include "kaps_pool.h"
#include "kaps_db_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** Maximum indirection to support  : as of the bit field uint32_t ref_count:15 kaps_ad */
#define KAPS_AD_MAX_INDIRECTION 0x3FFF

#define KAPS_NUM_BITS_IN_USB_AD_ADDRESS (16)
#define KAPS_NUM_AD_ADDRESS_IN_USB (1 << KAPS_NUM_BITS_IN_USB_AD_ADDRESS)

    struct kaps_entry;

/**
 * AD chunk created by AD manager, represents one block
 * of memory that can accommodate the requested number of
 * records for alg node or DBA based management. Used for
 * index based AD management
 */
    struct kaps_ad_chunk
    {
        uint32_t ad_blk_no;               /**< AD block number to which this chunk belongs to */
        uint32_t offset_in_ad_blk;        /**< offset of the first entry within the UDA super block */
        struct kaps_device *device;        /**< The device on which this chunk lives */
        struct kaps_ad_chunk *next_chunk;  /**< Pointer to next chunk */
        struct kaps_ad_db *ad_db;          /**< The AD database this chunk belongs to */
    };

#define KAPS_GET_AD_SEQ_NUM(e) ((e)->ad_db_seq_num_0 | ((e)->ad_db_seq_num_1 << 5))

/**
 * @brief Generic Associated Data handle for any KBP entry.
 *
 * The actual handle is specialized based on the size of AD
 *
 * ad_node      List of AD entries in the database
 * entries      Array to store ACL/LPM/EM entries pointing to this AD
 * user_handle  User handle for this entry
 * ad_db_seq_num_0 The database seq_num to which this AD belongs
 * ad_db_seq_num_1 The database seq_num to which this AD belongs
 * arr_size     Size of the array for entries
 * ref_count    Reference count to maintain for AD
 * is_ms_side   If set, valid AD is at MS side else LS side
 * value        Associated Data value
 *
 */

#define KAPS_AD_STRUCT_MEMBERS   \
    struct kaps_list_node ad_node;   \
    struct kaps_entry **entries; \
    uint32_t user_handle:27;       \
    uint32_t ad_db_seq_num_0:5;       \
    uint32_t ad_db_seq_num_1:3;    \
    uint32_t arr_size:16;       \
    uint32_t ref_count:14;      \
    uint32_t is_ms_side:1;      \
    uint32_t is_dangling_ad:1;

    struct kaps_ad
    {
        KAPS_AD_STRUCT_MEMBERS uint8_t value[];
                                /**< Associated Data value */
    };

/**
 * @brief Specialized 32b AD handle
 */
    struct kaps_ad_32b
    {
        KAPS_AD_STRUCT_MEMBERS uint8_t value[4];
                                /**< 32b Associated Data value */
    };

/**
 * @brief Specialized 64b AD handle
 */
    struct kaps_ad_64b
    {
        KAPS_AD_STRUCT_MEMBERS uint8_t value[8];
                                /**< 64b Associated Data value */
    };

/**
 * @brief Specialized 128b AD handle
 */
    struct kaps_ad_128b
    {
        KAPS_AD_STRUCT_MEMBERS uint8_t value[16];
                                /**< 128b Associated Data value */
    };

#define DBLIST_TO_AD_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_ad, ad_node)

/**
 * @brief Specialized 256b AD handle
 */
    struct kaps_ad_256b
    {
        KAPS_AD_STRUCT_MEMBERS uint8_t value[32];
                                /**< 256b Associated Data value */
    };

/**
 * @brief Associated Data Database
 *
 */
    struct kaps_ad_db
    {
        struct kaps_db db_info;      /**< Common database information */
        struct kaps_fast_bitmap *fbmap[KAPS_HW_MAX_UDA_SB];
                                                          /** bitmap to mange AD UDA space */
        uint32_t num_0b_entries;    /**< Number of 0b entries */
        uint16_t user_width_1;      /**< Width of the user Associated Data in bits */
        uint8_t pool_init_done;     /**< has AD entry pool been initialized */
        uint8_t user_bytes;         /**< User AD width normalized to bytes */
        uint8_t device_bytes;       /**< Normalized width of AD as stored on device */
        uint8_t byte_offset;        /**< The offset inside normalized AD where the data starts */
        uint32_t num_writes;        /**< Number of writes issued for this db */
        uint32_t num_entries[KAPS_MAX_NUM_CORES];         /**< num of entries of each ad width */
        uint32_t acquire_extra_ix_for_lmpsofar; /**< acquire extra index for lmpsofar prefixes*/

        struct kaps_allocator *alloc;/**< Allocator handle provided by user */
        struct kaps_c_list ad_list;      /**< List of AD entries added to the database*/
        struct kaps_ad_db *next;     /**< Linked List of databases */
        struct kaps_ad_db *hb_ad;    /**< Points to the corresponding internal ad db */
        struct kaps_ad_db *parent_ad;/**< Points to the corresponding parent ad db if the current is internal*/
        struct kaps_ix_mgr *mgr[KAPS_MAX_NUM_CORES][KAPS_MAX_NUM_DAISY_CHAINS];         /**< Points to the Ix Mgr */

        union
        {
            POOL_DEF(
    kaps_ad_32b) ad_entry_pool_32;                /**< Memory pool for 32b AD */
            POOL_DEF(
    kaps_ad_64b) ad_entry_pool_64;                /**< Memory pool for 64b AD */
            POOL_DEF(
    kaps_ad_128b) ad_entry_pool_128;              /**< Memory pool for 128b AD */
            POOL_DEF(
    kaps_ad_256b) ad_entry_pool_256;              /**< Memory pool for 256b AD */
        } pool;
            POOL_DEF(
    kaps_ad_chunk) ad_chunk_pool;          /**< Memory pool for chunks created in AD */
    };

/**
 * Adds the AD entry. The value array must point to valid memory of the width of the AD database.
 * Internal API.
 * @param db Valid database handle.
 * @param value Associated data content.
 * @param ad Associated data handle returned on success. Can be used to associate with ACL, LPM and EM database entries.
 * @param Actual user handle
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

    kaps_status kaps_ad_db_add_entry_internal(
    struct kaps_ad_db *ad_db,
    uint8_t * value,
    struct kaps_ad **ad,
    intptr_t user_handle);

/**
 * common utility function that updates the AD handle to
 * cross reference the entries pointing to it
 * @param  device    valid device handle
 * @param  ad        valid AD entry handle
 * @param  entry     valid ACL/LPM/EM entry handle
 * @return       KAPS_OK on success or an error code
 */

    kaps_status kaps_ad_db_associate_entry_and_ad(
    struct kaps_device *device,
    struct kaps_ad *ad,
    struct kaps_entry *entry);

/**
 * common utility function that updates the AD handle to
 * cross remove reference of the entry pointing to it
 * @param  device    valid device handle
 * @param  ad    valid AD entry handle
 * @param  entry valid ACL/LPM/EM entry handle
 * @return       KAPS_OK on success or an error code
 */

    kaps_status kaps_ad_db_deassociate_entry_and_ad(
    struct kaps_device *device,
    struct kaps_ad *ad,
    struct kaps_entry *entry);

/**
 * Internal API to create an AD database
 *
 * @param device Valid device handle.
 * @param id Database ID. This is a control-plane identifier only.
 * @param capacity Expected maximum capacity for this database. Can be zero, which means unlimited.
 * @param width_1 Width of AD entries in bits, which must be a multiple of eight.
 * @param db Database handle, initialized and returned on success.
 * @param is_internal Whether it is an internal AD Database
 *
 * @returns KAPS_OK on success or an error code otherwise.
 */

    kaps_status kaps_ad_db_init_internal(
    struct kaps_device *device,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    struct kaps_ad_db **db,
    uint32_t is_internal);

/**
 * Internal API to change the width of the ad. This call is valid only before commit.
 *
 * @param db points to the db to be updated
 * @param new_width New width to be set
 *
 * @returns KAPS_OK on success or an error code otherwise.
 */

    kaps_status kaps_ad_db_update_width(
    struct kaps_db *db,
    uint32_t new_width);

/**
 * Internal API to find the seq num of AD Database.
 *
 * @param db Valid DB pointer
 * @param ad_db AD Database pointer
 *
 * @returns KAPS_OK on success or an error code otherwise.
 */

    uint32_t kaps_ad_db_get_seq_num(
    struct kaps_db *db,
    struct kaps_ad_db *ad_db);

/**
 * Internal API to find the AD Database with a seq num
 *
 * @param db Valid DB pointer
 * @param expected_seq_num Seq Num of AD Database
 *
 * @returns AD Database pointer
 */

    struct kaps_ad_db *kaps_ad_get_ad_db_ptr(
    struct kaps_db *db,
    uint32_t expected_seq_num);

#ifdef __cplusplus
}
#endif

#endif

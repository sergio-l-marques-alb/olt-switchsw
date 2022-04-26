/*******************************************************************************
 *
 * Copyright 2015-2019 Broadcom Corporation
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

#ifndef __KAPS_HB_INTERNAL_H
#define __KAPS_HB_INTERNAL_H

#include "kaps_errors.h"
#include "kaps_bitmap.h"
#include "kaps_hw_limits.h"
#include "kaps_list.h"
#include "kaps_hb.h"
#include "kaps_pool.h"
#include "kaps_db_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define KAPS_HB_SPECIAL_VALUE (0xFFFFFFFF)

    enum kaps2_hb_config_type
    {
        KAPS2_HB_CONFIG_A000_B000 = 0x1,
        KAPS2_HB_CONFIG_A000_B480 = 0x2,
        KAPS2_HB_CONFIG_A000_B960 = 0x4,
        KAPS2_HB_CONFIG_A480_B000 = 0x8,
        KAPS2_HB_CONFIG_A480_B480 = 0x10,
        KAPS2_HB_CONFIG_A960_B000 = 0x20
    };

    struct kaps_entry;

#define KAPS_DBLIST_TO_HB_ENTRY(ptr)  CONTAINER_OF((ptr), struct kaps_hb, hb_node)

/**
 * @brief Aging Entry Structure
 *
 */
    struct kaps_aging_entry
    {
        struct kaps_entry *entry;
                                /**< Entry pointer for Aging */
        uint32_t num_idles;    /**< Number of idles for this entry */
        uint8_t intermediate_hb_val;
                                 /**<Was Hit Bit  set during hb_dump of the other db*/
    };

/**
 * @brief Generic Hit Bit handle for any KBP entry.
 */

/*
Store the num idles here
Bit no is needed only for indirection
We will be needing hit bit value which is 0/1. Needed during LSN splits*/
    struct kaps_hb
    {
        struct kaps_list_node hb_node;
                                     /**< List of HB entries in the database*/
        struct kaps_entry *entry;/**< Entry associated to this HB */
        uint32_t user_handle:24;/**< User handle for this entry*/
        uint32_t hb_db_seq_num:8;
                                /**< seq_num of this database */
        uint32_t bit_no:30;     /**< Bit no of this HB */
        uint32_t bit_allocated:1;
                                /**< this hb no is allocated */
        uint32_t value:1;       /**< Indicates the hit bit value, either 0/1*/
    };

/**
 * @brief Hit Bit Database
 *
 */
    struct kaps_hb_db
    {
        struct kaps_db db_info;      /**< Common database information */
        struct kaps_allocator *alloc;/**< Allocator handle provided by user */
        struct kaps_c_list hb_list;      /**< List of HB entries added to the database*/
        uint32_t age_count;         /**< Number of idle cycles after entries are aged */
        uint32_t bit_iter;          /**< Iterator bit */
        uint32_t capacity;          /**< user provided database capacity */

            POOL_DEF(
    kaps_hb) kaps_hb_pool;            /**< Memory Pool for Hit Bit entries */
    };

/**
 * Adds the HB entry.
 * Internal API.
 * @param hb_db Valid database handle.
 * @param hb Hit Bit handle returned on success. Can be used to associate with LPM database entries.
 * @param user_handle Actual user handle
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
    kaps_status kaps_hb_db_add_entry_internal(
    struct kaps_hb_db *hb_db,
    struct kaps_hb **hb,
    intptr_t user_handle);

    typedef kaps_status(
    *hb_ix_changed_callback) (
    void *client,
    struct kaps_entry * entry,
    uint32_t old_index,
    uint32_t new_index);

    kaps_status kaps_hb_cb(
    void *client,
    struct kaps_entry *entry,
    uint32_t old_index,
    uint32_t new_index);



    

    kaps_status kaps_hb_wb_save_state(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

    kaps_status kaps_hb_wb_restore_state(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

    kaps_status kaps_hb_db_wb_add_entry(
    struct kaps_hb_db *hb_db,
    uint32_t bit_no,
    uintptr_t user_handle_p);

    kaps_status kaps_kaps_hb_cr_init(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *cb_fun);

#ifdef __cplusplus
}
#endif

#endif

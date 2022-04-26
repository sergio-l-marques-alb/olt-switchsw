/*******************************************************************************
 *
 * Copyright 2014-2019 Broadcom Corporation
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

#ifndef __KAPS_DB_WB_H
#define __KAPS_DB_WB_H

#include "kaps_portable.h"
#include "kaps_list.h"
#include "kaps_db.h"
#include "kaps_resource.h"
#include "kaps_bitmap.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define INVALID_PAIRED_AB_NUM (0xFFFF)

    struct kaps_ab_wb_info
    {
        uint16_t ab_num; /**< DBA Block number */
        uint16_t num_slots;
                         /**< Number of entries in DBA block */
        uint8_t conf;    /**<< AB type */
        uint8_t blk_cleared;
                         /**< Has AB been cleared */
        uint8_t device_no;
                         /**< Device number in which this block is present */
        uint8_t translate;
                         /**< Should address be translated based on AB config */
        uint16_t paired_ab_num;    /**< DBA block number that is paired with this ab*/
        uint32_t base_addr;     /**< Base address for the DBA block to point */
        uint32_t base_idx;     /**< Base index for the DBA block to point */
        struct kaps_db *db;
                          /**< Database pointer to which this AB belongs to*/
        uint32_t is_dup_ab:1;
                          /**< Indicates if its a duplicate AB */
    };

    struct kaps_ad_db_wb_info
    {
        uintptr_t stale_ptr;          /**< Database ptr*/
        uintptr_t hb_ad_stale_ptr;    /**< HB AD Database ptr*/
        uint32_t id;                  /**< Control plane ID */
        uint32_t ad_width;            /**< User specified AD width */
        uint32_t capacity;            /**< User specified AD DB capacity */
        uint8_t user_specified;   /**< User specified UDA MB*/
        int32_t uda_mb;             /**< User specified UDA MB */
        struct kaps_ad_db_hw_resource ad_hw_res;
                                             /**< Assigned HW resources for AD databases */
    };

    struct kaps_db_wb_pio_stats
    {
        uint32_t num_of_piowrs;
                            /**< Number of PIOWRs */
        uint32_t num_of_piords;
                            /**< Number of PIORDs */
        uint32_t num_of_shuffles;
                              /**< Number of Shuffles */
        uint32_t num_of_worst_case_shuffles;
                                         /**< Number of Worst case shuffles taken for entry */
        uint32_t num_ix_cbs;
                         /**< Number of index change callbacks */
        uint32_t num_blk_ops;
                          /**< Number of block operations */
        uint32_t num_ad_writes;
                            /**< Number of AD writes */
    };

    struct kaps_db_wb_info
    {
        uintptr_t stale_ptr;          /**< Database ptr*/
        uintptr_t stale_left_db_ptr;  /**< Paired left database stale pointer */
        uint32_t tid;                 /**< Control plane database identifier (database ID) */
        uint32_t capacity;            /**< Minimum required capacity */
        int32_t max_capacity;      /**< Maximum capacity limit*/
        uintptr_t clone_of;           /**< If this database is a clone, this points to the database/table of which this is a clone.*/
        enum kaps_db_type type;        /**< Database type */
        uint16_t desc;                /**< User provided description length */
        uint8_t meta_priority;       /** < User defined data */
        uint32_t enable_dynamic_allocation:1;
                                          /**< enables dynamic allocation of DBA and UDA*/
        uint32_t enable_db_compaction:1;
                                     /**< enables DB compaction*/
        uint32_t is_table:1;          /**< This database is a table */
        uint32_t is_clone:1;          /**< This database is a clone */
        uint32_t has_clones:1;        /**< This database has clones */
        uint32_t has_tables:1;        /**< This database has sub-tables */
        uint32_t user_specified:1;    /**< Resources have been user specified */
        uint32_t user_specified_range:1;
                                      /**< Range units have been specified by user */
        uint32_t cascade_override:1;  /**< User specified cascade device */
        uint32_t custom_ix_range:1;   /**< User specified IX Range */
        uint32_t callback_data:1;     /**< User specified callback function */
        uint32_t cascade_device:2;    /**< Cascade device number if cascade override is specified */
        uint32_t is_algorithmic:1;    /**< Database is algorithmic */
        uint32_t is_user_specified_algo_type:1;
                                            /**< is user specified the algo mode */
        uint32_t smt_no:1;            /**< SMT device no */
        uint32_t calc_shuffle_stats:1;/**<Enable shuffle stats calculations */
        uint32_t has_dup_ad:1;        /**< AD had been duplicated */
        uint32_t is_algo:1;           /**< user specified algo type */
        uint32_t priority_pattern:2;    /* < Indicates the priority pattern of this database */
        uint32_t is_main_bc_db:1;       /**< Database is main_db, not broadcasted */
        uint32_t is_bc_required:1;    /**< is broadcast feature required for this DB, used by Resource manager */
        uint32_t is_replicated_internally:1;
                                         /**< op2 specific, set only when the DB is replicated to other core internally by SDK */
        uint32_t mapped_to_acl:1;     /**< LPM / EM database mapped to ACL */
        uint32_t is_em:1;             /**< The database is EM */
        uint32_t has_hb:1;            /**< database has HB */
        uint32_t is_cascaded:1;       /**< cascaded database */
        uint32_t locality:2;          /**< user set locality to indicate resource sharing  */
        uint32_t db_cascade_alternate_addition:1;      /**< Add prefixes in ping and pong order (OP2 Specific)  */
        uint32_t hb_id;               /**<HB DB id*/
        uint32_t hb_capacity;         /**< HB Capacity */
        uint32_t hb_age_count;        /**< HB Stale pointer */
        uintptr_t hb_stale_ptr;       /**< HB Stale pointer */
        uint32_t defer_deletes;       /**< If defer deletes is enabled */
        int32_t prev_entry_priority;    /* < Holds the priority of the last added entry */
        uint32_t num_ad_databases;    /**< Number of AD Databases associated with this DB */
        uint32_t alg_type;            /**< ALG type */
        struct kaps_print_key *key;    /** < Key Structure for printing key */
        struct kaps_db_wb_pio_stats pio_stats;
                                           /**< PIOWR/RD and Shuffle stats */
        uint32_t bc_bitmap;           /**< Broadcast bitmap value for this DB */
        uint32_t user_num_ab;         /** < User defined data */
        uint32_t user_uda_mb;         /** < User defined data */
        uint32_t range_units;         /** < User defined data */
        uint32_t index_range_min;     /** < User defined data */
        uint32_t index_range_max;     /** < User defined data */
        uint16_t highest_wide_ab_index;
                                      /** < Highest indexed wide AB currently allocated to this db */
        uint32_t num_algo_levels_in_db; /**< Number of algorithmic levels in the database*/
        kaps_db_index_callback callback_fn;
                                       /** < User defined data */
        void *callback_handle;        /** < User defined data */
        enum kaps_db_insertion_mode insertion_mode;   /**< 0: none, 1: normal, 2: relative insertion */
        uint32_t relative_entry_wb_handle;
                                       /** <insert relative to this entry */
        enum kaps_entry_position relative_entry_position;
                                                      /**< relative position before/after */
        char description[];          /** < Description of the database */
    };

    struct kaps_wb_cb_functions
    {
        kaps_device_issu_read_fn read_fn;
                                        /**< Read function for WB */
        kaps_device_issu_write_fn write_fn;
                                        /**< Write function for WB */
        void *handle;                  /**< Handle for WB */
        uint32_t *nv_offset;           /**< Current offset for WB */
        uint8_t *nv_ptr;               /**< current location of NV memory pointer for crash recovery */
    };

    struct kaps_cr_table_entry
    {
        uint32_t offset:24;          /**< Entry offset in NV memory */
        uint32_t is_valid:8;         /**< Valid bit */
    };

    enum kaps_cr_entry_status
    {
        KAPS_CR_ENTRY_UNHANDLED = 0,
        KAPS_CR_ENTRY_COMMITED = 1,
        KAPS_CR_ENTRY_DELETE = 2,
        KAPS_CR_DANGLING_AD = 3,
    };

/**
 * Saves the DB information such as DB id, Capacity, IX range
 * to the NV memory using the read/write callbacks provided.
 *
 * @param db Valid database pointer
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
    kaps_status kaps_db_wb_save_info(
    struct kaps_db *db,
    struct kaps_wb_cb_functions *wb_fn);

/**
 * Restores the DB saved to NV memory.
 * This API is responsible for initializing the DB and performing other operations
 * such as setting various properties, setting the key on DB while in the ISSU restore phase.
 *
 * @param device Valid device pointer
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
    kaps_status kaps_db_wb_restore_info(
    struct kaps_device *device,
    struct kaps_db *db,
    struct kaps_wb_cb_functions *wb_fn);

/**
 * Starts reconcile of the DB
 * by marking all the entries as unused.
 *
 * @param db Valid database pointer
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

    kaps_status kaps_db_reconcile_start(
    struct kaps_db *db);

/**
 * Ends reconcile on the DB
 * by deleting all the unused entries.
 *
 * @param db Valid database pointer
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

    kaps_status kaps_db_reconcile_end(
    struct kaps_db *db);

/**
 * Entry point in function that saves all LPM WB information in one
 * shot
 *
 * @param device valid device handle
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_wb_save_state(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fn);

/**
 * Entry point in function that restores all the common LPM WB information
 *
 * @param device valid device handle
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_wb_restore_state(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fn);

/**
 * API to do the pre processing required before proceeding with Warmboot Save
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_wb_pre_processing(
    struct kaps_device *device);

/**
 * API to do the post processing required after Warmboot Restore is done
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_wb_post_processing(
    struct kaps_device *device);

/**
 * Entry point in function that saves all LPM CR information in one
 * shot
 *
 * @param device valid device handle
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_lpm_cr_save_state(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fn);

/**
 * Entry point in function that restores all the common LPM CR information
 *
 * @param device valid device handle
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_lpm_cr_restore_state(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fn);

/**
 * Calculate memory required for LPM advanced Shadow and advance the CR NV ptr
 *
 * @param device valid device handle
 * @param wb_fn The warm boot/ crash recovery state callbacks
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_lpm_cr_get_adv_ptr(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Return the offset in nv where the data resides
 *
 * @param db valid db handle
 * @param user_handle Entry's user_handle
 *
 * @return offset in which this user_handle corresponding data
 */

    uint32_t kaps_db_entry_get_nv_offset(
    struct kaps_db *db,
    uint32_t user_handle);

#ifdef __cplusplus
}
#endif
#endif

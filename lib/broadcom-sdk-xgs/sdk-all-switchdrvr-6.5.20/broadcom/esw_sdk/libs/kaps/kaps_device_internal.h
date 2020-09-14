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

#ifndef __KAPS_DEVICE_INTERNAL_H
#define __KAPS_DEVICE_INTERNAL_H

#include <stdint.h>
#include "kaps_device.h"
#include "kaps_xpt.h"
#include "kaps_list.h"
#include "kaps_legacy.h"
#include "kaps_hb_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define KAPS_NV_MAGIC_NUMBER                         (0x12121212)

/**
 * Build KBP SDK Version String
 */
#define BUILD_VERSION_STR_(a, b, c) a b c
#define BUILD_VERSION_STR(a, b, c) BUILD_VERSION_STR_(a, b, c)

#ifndef KAPS_DEBUG_SUFFIX
#define KAPS_DEBUG_SUFFIX ""
#endif

#ifndef KAPS_BUILD_DATE
#define KAPS_BUILD_DATE ""
#endif

#define KAPS_SDK_VERSION_FULL BUILD_VERSION_STR(KAPS_SDK_VERSION, KAPS_BUILD_DATE, KAPS_DEBUG_SUFFIX)

/**
 * @cond INTERNAL
 *
 * @file kaps_device_internal.h
 *
 * Structural details of device, key and other structures
 */

    struct kaps_shadow_device;
    struct kaps_ad_db;
    struct kaps_ad_chunk;
    struct kaps_ad;

/**
 * Return the database type in printable format
 *
 * @param type database type information
 *
 * @retval printable string on success
 * @retval NULL on error
 */

    char *kaps_device_db_name(
    struct kaps_db *db);

    enum kaps_issu_status
    {
        KAPS_ISSU_INIT = 0,         /**< Device Initialized with ISSU flag*/
        KAPS_ISSU_SAVE_COMPLETED = 1,
                                    /**< ISSU Save state has been completed*/
        KAPS_ISSU_RESTORE_START = 2,/**< ISSU Restore has been invoked */
        KAPS_ISSU_RESTORE_END = 3,  /**< ISSU Restore has completed */
        KAPS_ISSU_RECONCILE_START = 4,
                                    /**< ISSU Reconcile started */
        KAPS_ISSU_RECONCILE_END = 5 /**< ISSU Reconcile done */
    };

/**
 * Magic DB number used to mark that AB is already occupied with this magic database
 */
#define MAGIC_DB ((struct kaps_db *)0x1)

/* Note:
   for OP the correctable fifo entries are not populated to this DS.
   in case of DCR autofix bit is 0 and 1b parity will go to Uncorrectable fifo
 */

/**
 * NV Memory Manager
 */
    struct kaps_nv_memory_manager
    {
        uint32_t num_db;

        uint32_t offset_device_info_start;

        uint32_t offset_device_pending_list;

        uint32_t offset_device_lpm_info;

        uint32_t offset_device_lpm_shadow_info;

        uint32_t offset_device_hb_info;

        uint32_t *offset_db_trie_has_been_built;

        uint32_t *offset_db_trie_sb_bitmap;

        uint32_t *offset_db_info_start;
        uint32_t *offset_db_info_end;

        struct kaps_cr_pool_mgr *kaps_cr_pool_mgr;
    };

    struct kaps_device_property
    {
        uint32_t lpt_mode:1;                /**< 1: LPT encoding enabled, 0: LPT encoding disabled. Default is 1 */
        uint32_t instruction_latency:4;     /**< Can control plane issue long latency instructions */
        uint32_t early_block_clear:1;       /**< Can clear blocks at the time of device lock */
        uint32_t enable_adv_uda_write:1;    /**< Enable/Disable advanced UDA write */
        uint32_t read_type:2;               /**< read type 0: read from DS/SM, 1: read from device/model and 2: read from device and compare against SM data */
        uint32_t is_broadcast_at_xpt:1;     /**< 1: broadcast to be done at xpt level*/
        uint32_t return_error_on_asserts:1; /**< Return error code / abort on assert hit */
        uint32_t min_rxc:1;                 /**< internal use : minimal resources */
    };

    struct kaps_seq_num_to_ptr
    {
        struct kaps_db **db_ptr;       /**< Array of database pointers */
        struct kaps_ad_db **ad_db_ptr; /**< Array of ad database pointers */
        struct kaps_hb_db **hb_db_ptr; /**< Array of hb database pointers */
        uint32_t is_memory_allocated;    /**< Memory allocated for array of databases */
        uint32_t num_db;              /**< Number of databases */
        uint32_t num_ad_db;           /**< Number of ad databases */
        uint32_t num_hb_db;           /**< Number of hb databases */
    };

#define KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(d, e, ad)                                              \
    {                                                                                           \
        uint32_t ad_db_seq_num_merged = ((e)->ad_db_seq_num_0 | ((e)->ad_db_seq_num_1 << 5));   \
        kaps_sassert (ad_db_seq_num_merged);                                                          \
        kaps_sassert (ad_db_seq_num_merged <= d->seq_num_to_ptr->num_ad_db);                          \
        kaps_sassert (d->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num_merged]);                            \
        ad = d->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num_merged];                                \
    }

#define KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(d, e, db)                          \
    {                                                                    \
        kaps_sassert ((e)->db_seq_num);                                        \
        kaps_sassert ((e)->db_seq_num <= d->seq_num_to_ptr->num_db);           \
        kaps_sassert (d->seq_num_to_ptr->db_ptr[(e)->db_seq_num]);             \
        db = d->seq_num_to_ptr->db_ptr[(e)->db_seq_num];                 \
    }

/**
 * Device Specification
 */
    struct kaps_device
    {
        struct kaps_allocator *alloc;        /**< Pointer to the memory allocator */
        enum kaps_device_type type;          /**< Normalized Type of device */
        struct kaps_device_resource *hw_res; /**< HW resource information */

        struct kaps_dev_12k_devmgr *devmgr_12k;
                                             /**< Pointer to the 12K device manager */
        struct kaps_dev_12k_devinfo *dev_p;  /**< The specific device for dual port/cascade */
        struct kaps_shadow_device *kaps_shadow;
                                            /**< Kaps specific shadow device */
        struct kaps_dev_12k_adv_shadow *adv_12k_shadow;
                                                    /** Advanced shadow for 12K device */
        struct kaps_dev_op_adv_shadow *adv_op_shadow;
                                                   /** Advanced Shadow for OP device */
        struct kaps_device *main_dev;        /**< Primary device handle, when cascade/SMT is enabled, this is
                                                 device0/thread0. All databases and instructions are
                                                 chained as part of this device handle */
        struct kaps_device *next_dev;        /**< The next cascaded device */
        struct kaps_device *smt;             /**< Pointer to the other SMT device, each device will have
                                                 its pointer to SMT */

        struct kaps_device *other_core;      /**< Pointer to the other core device, each core will have
                                                 the pointer to other core */
        struct memory_map *map;             /**< Memory map of device */
        struct kaps_c_list db_list;              /**< List of databases */
        struct kaps_c_list inst_list;            /**< List of instructions */
        struct kaps_lpm_mgr *lpm_mgr;        /**< Pointer to lpm manager */
        struct kaps_device_config config;    /**< Interface configuration if specified by user */
        struct kaps_nlm_allocator nlm_alloc;    /**< Allocator used for the device manager, xpt and model */
        void *xpt;                          /**< Pointer to the transport layer (FPGA, simxpt or blackhole) */
        FILE *log_device_state;             /**< File Pointer to Log the Device State */
        FILE *dynamic_alloc_fp;             /**< File pointer to log dynamic allocation info */
        enum kaps_issu_status issu_status;   /**< Maintains the status of ISSU*/
        struct kaps_device_property prop;    /**< Device properties */
        char *description;                  /**< The XML description field */
        const char *lpm_color;              /**< 12K LPM related color */
        uint8_t num_ab_per_sb;              /**< Number of ABs in one SB */
        uint8_t num_active_ab_per_sb;       /**< Number of active ABs in one DBA Super Block */
        uint8_t num_dba_sb;                 /**< Number of SB on chip */
        uint8_t num_mcor;                   /**< Number of MCOR units */
        uint8_t port_num;                   /**< HW port number */
        uint8_t low_high;                   /**< To indicate power up lower or higher UDMs */
        uint8_t max_num_clone_parallel_lookups;
                                            /**< Maximum number of parallel lookups that can be done on a database or its clones*/
        uint16_t num_ab;                    /**< Number of ABs on chip */
        uint16_t num_udc;                   /**< UDA memory on chip 16:512Mb or 8:256Mb */
        uint8_t num_uda_per_sb;             /**< per super block */
        int16_t num_ab_available;           /**< Number of ABs available (val) */
        int16_t num_uda_available;          /**< How much UDA available (Mb) */
        int16_t num_normal_ab_allocated;    /**< Number of normal ABs currently allocated on chip */
        int16_t num_big_ab_allocated;       /**< Number of big ABs currently allocated on chip */
        int16_t num_small_ab_allocated;     /**< Number of small ABs currently allocated on chip */
        uint16_t num_80b_entries_ab;        /**< Number of 80b entries in AB */
        uint16_t max_ad_width_1;            /**< Max width of AD read */
        uint16_t max_interface_width_1;     /**< Maximum interface transmit bandwidth */
        uint16_t num_hb_blocks;             /**< Number of hit bit blocks */
        uint16_t num_rows_in_hb_block;      /**< Number of rows in hit bit block*/
        uint16_t num_small_hb_blocks;       /**< Number of hit bit blocks for small BBs*/
        uint16_t num_rows_in_small_hb_block; /**< Number of rows in hit bit blocks for small BBs*/
        uint32_t max_search_key_width_1;    /**< Maximum search key width */
        uint32_t num_reserved_rpt_for_non_xor_lpm;
                                               /**< Number of RPTs which should not be assigned to LPM XOR DBs, they are reserved for non-xor DBs */
        uint32_t is_config_locked:1;        /**< Has configuration been locked */
        uint32_t is_tap_config_locked:1;    /**< Used in Stats sources. needed for thread-safety */
        uint32_t smt_no:1;                  /**< SMT zero or one */
        uint32_t smt_init_done:1;           /**< Thread init already called */
        uint32_t device_no:3;               /**< The cascade device number */
        uint32_t is_ltr_sm_update_needed:1; /**< LTR SM update */
        uint32_t issu_in_progress:1;        /**< Indicates if ISSU is in progress or not */
        uint32_t lpm_lock_done:1;           /**< If LPM config has been locked */
        uint32_t is_range_enabled:1;        /**< Are range encoding engines enabled */
        uint32_t is_b_silicon:1;            /**< 12K B silicon */
        uint32_t silicon_sub_type:4;        /**< version of silicon */
        uint32_t profile:4;                 /**< Profile in which the device will be used*/
        uint32_t dba_err_threshold:6;       /**< DBA soft error threshold */
        uint32_t debug_level:3;             /**< debug level, default 0, any positive values > 0 can dump the debug details */
        uint32_t interrupt_enable:1;        /**< Are interrupts enabled for DBA soft errors */
        uint32_t adv_pscan:1;               /**< if 1: disable the adv parity scan, 0: enable the scan (default if dcr b'6 set, scan enabled) */
        uint32_t is_rop_mode:1;             /**< Is device on ROP mode */
        uint32_t dump_xml_data_with_entries:1;
                                            /**< When XML dump is invoked, should entries be written. */
        uint32_t is_alg_db_present:1;       /**< Indicates if at least one algorithmic database is present in the device*/
        uint32_t is_tap_db_present:1;       /**< Indicates if at least one TAP database is present in the device*/
        uint32_t is_counter_db_present:1;   /**< Indicates if at least one Counter database is present in the device*/
        uint32_t is_wb_continue:1;          /**< Is Warmboot save and continue being used*/
        uint32_t reduced_the_resources:1;   /**< Reduces the Number of Resources */
        uint32_t inject_adv_parity_err:1;   /**< if 0 disabled: 1 enabled (Netl debug regs used to flip parity bits in ADV locations: only for 12K) */
        uint32_t inject_parity_err;         /**< if 0 disabled: non zero value refer enum kaps_dev_12k_parity_inject_type, kaps_dev_op_parity_inject_type (Netl debug regs used to flip parity bits in DBA locations) */
        uint32_t is_generic_bc_device:1;    /**< 1: this device is a generic broadcast device, 0: otherwise*/
        uint32_t is_selective_bc_enabled:1;                 /**< 1: selective broadcast has been enabled in this device, 0: otherwise*/
        uint32_t txn_in_progress:1;                         /**< 1: A transaction is in progress, related to Crash Recovery*/
        uint32_t pack_dbs_in_ab:1;                          /**< When set pack 2 narrow dbs vertically within an AB */
        uint32_t defer_ltr_writes:1;                        /**< If set, defer LTR writes to flush API */
        uint32_t defer_all_writes:1;                        /**< If set, defer all writes to flush API */
        uint32_t nv_shadow:1;                               /**< If set, Shadow allocated from NV */
        uint32_t check_fatal_dbg_0:1;                       /**< Memory dbg check 0 */
        uint32_t fatal_transport_error:1;                   /**< Will be set to 1 if there is a transport error */
        uint32_t check_fatal_dbg_1:1;                       /**< Memory dbg check 1 */
        uint32_t is_fw_loaded:1;                            /**< Indicates if the FW is loaded or not */
        uint32_t op2_mode:2;                                /**< Operation mode for OP2 device */
        uint32_t op2_clock_rate:1;                          /**< 0 means full rate, 1 means half rate */
        uint32_t core_id:1;                                 /**< OP2 specific, core id of the device */
        uint32_t num_cores:2;                               /**< Number of cores in the KBP chip */
        uint32_t instruction_cascading_present:1;           /**< OP2 specific, 1 indicates instructions has been cascaded */
        uint32_t clear_ESR_err_bits:3;                      /**< If set 1: clear device errors/parity, 0 none (default 0) */
        uint32_t bc_needed:1;                               /**< Need to Broadcast to all device(s) ? : default 1 will broadcast, 0 means only particular device */
        uint32_t inplace_ad_present:1;                      /**< OP2 specific, indicates if there is any DB with inplace AD */
        uint32_t mp_db_present:1;                           /**< OP2 specific, indicates if there is any MP DB */
        uint32_t mp_db_ix_zone:2;      /**< OP2 specific, common index bits */
        uint32_t is_small_bb_on_second_half:1;              /**< KAPS specific, indicates if the second half of the small Bbs are also used or not*/
        uint32_t is_counter_scrub_thread:1;                 /**< Set to 1 if there is a separate thread for Counter/Stats scrubbing */
        uint32_t num_user_threads:3;                        /**< Number of threads created by the user*/
        uint32_t map_print:1;                               /**< Allows device map print in binary format */
        uint32_t do_lut_write:1;                            /**< Do LUT write (OP/OP2 specific)*/
        uint32_t disable_instruction_cascading:1;           /**< Disable instruction cascading */
        uint32_t error_detect_type:2;                       /**< 0: HW detected, 1: injection, 2: emulation (for internal ref) */
        uint32_t handle_interface_errors:1;                 /**< 0: Disable interface error handling functionality in SDK [applicable to OP/OP2 devices only], default 1 */
        uint32_t magic_db_entry_invalidate:1;               /**< 1: user can force the parity location to be invalidated
                                                                 (parity errors occured from block used by customers/reserved) */
        uint32_t execute_the_next_loop:1;                   /**< used for broadcast device lock, indicates whether to execute a loop or not based
                                                                 on the previous loop completion */
        uint32_t flag_pcm_sharing_level:2;                  /**< indicates the level of sharing for RPT allocation
                                                                 0: default, 1: share ACls as much as possible, 2: share everything as much as possible */
        uint32_t flag_estimation_phase:1;                   /**< this flag is used only for resource estimation */
        uint32_t is_fw_dba_scan_en:1;                       /**< Indicates if the FW DBA Scan is enabled or not */
        uint32_t dont_use_rp_conflict_bits;                 /**< If enable, then no need to populate conflict bits corresponding to result ports.
                                                                 There is no conflict there. */
        uint32_t id;                                        /**< Identifier */
        uint16_t max_num_searches;                          /**< Max number of searches possible in one instruction*/
        uint16_t max_num_kpu_key_segments;                  /**< Max number of KPU Key segments per KPU */
        uint32_t flags;                                     /**< Flags passed in by the user */
        uint32_t dba_offset;                                /**< DBA Block offset, relevant to KAPS only */
        uint32_t uda_offset;                                /**< UDA Block offset, relevant to KAPS only */
        uint32_t small_bb_offset;                           /**< Middle stage offset, relevant only for KAPS*/
        uint32_t large_bb_mb;                               /**< Relevant only for KAPS-2*/
        uint32_t num_of_piowrs;                             /**< PIOWR stats */
        uint32_t num_of_piords;                             /**< PIORD stats */
        uint32_t num_blk_ops;                               /**< Number of block operations */
        uint32_t uda_power_budget;                          /**< Max no of UDMs allowed to turn on at any cycle */
        uint32_t dba_power_budget;                          /**< Max no of ABs allowed to turn on at any cycle by non-mp DBs */
        uint32_t next_algo_color;                           /**< Store the next available color */
        uint32_t next_stats_color;                          /**< Store the next available color */
        uint64_t scp_power_up;                              /**< Device SCP power up information */
        uint64_t poll_data[2];                              /**< Data required for poll the status registers */
        uint8_t hitbit_bank_bmp[2];                         /**< Hitbit bank bitmap*/
        struct kaps_aging_entry *aging_table;               /**< Aging table for the Hitbits */
        struct kaps_aging_entry *small_aging_table;         /**< Aging table for the hit bits of small BBs in two level mode */
        uint32_t aging_table_size;                          /**< Number of entries in the aging table */
        uint32_t small_aging_table_size;                    /**< Size of the small aging table */
        uint8_t *aging_valid_memory;                        /**< Valid Memory for Aging */
        uint8_t *hb_buffer;                                 /**< Buffer to read/write hbs */
        uint8_t *hb_vbuffer;                                /**< Buffer to read/write hbs valid memory */
        struct kaps_device *main_bc_device;                  /**< Main Broadcast device */
        struct kaps_device *next_bc_device;                  /**< Chained broadcast device */
        struct kaps_device *next_op2_device;                  /**< Chained broadcast device */
        uint32_t bc_id;                                     /**< Broadcast ID for this device */
        uint32_t db_bc_bitmap;                              /**< DB broadcast bitmap for Device */
        uint32_t generic_bc_bitmap;                         /**< Generic broadcast bitmap for Device */
        int32_t ltr_db_id;                                  /**< DB ID for LTR. */
        kaps_device_issu_read_fn nv_read_fn;                 /**< NV read callback function required for Crash Recovery */
        kaps_device_issu_write_fn nv_write_fn;               /**< NV write callback function required for Crash Recovery */
        void *nv_handle;                                    /**< Cookie for NV read/write callback functions */
        void *nv_ptr;                                       /**< NV memory base pointer for Crash Recovery */
        uint32_t is_fresh_nv;                               /**< Is the NV memory being used for the first time and not for Recovery */
        uint32_t nv_size;                                   /**< NV memory size provided for Crash Recovery */
        enum kaps_restore_status *cr_status;                 /**< Crash Recovery Pending Status flag */
        struct kaps_nv_memory_manager *nv_mem_mgr;               /**< Memory manager for Crash Recovery NV memory */
        uint32_t nv_udc_config_offset;                      /**< udc config register offset in nv_memory */
        char verbose_error_string[1024];                    /**< Array of characters to store the verbose error details */
        uint32_t is_core_reg_dump;                          /**< Dump core registers */
        uint32_t log_backdoor_api;                          /**< Log back door APIs */
        uint32_t dump_on_assert;                            /**< Warmboot dump on assertion */
        char fname_dump[256];                               /**< User specified filename for dump on assertion */
        uint64_t udm_sel_table_indices_in_use;              /**< Valid region bitmap in udm_select_table */
        uint64_t lpt_map;                                   /**< Used by flush API, ISSU specific */
        uint64_t reg_DCR;                                   /**< Use of dba parity scan enabled (DCR bit 6), if set use if for IFSR (oprime) */
        struct dba_mgr *dba_mgr_ptrs[KAPS_HW_MAX_DBA_BLOCKS];/**< used by flush API, ISSU specific */
        struct kaps_seq_num_to_ptr *seq_num_to_ptr;          /**< Sequence number information */
        int32_t num_bulkread_errors;                        /**< Number of bulkread errors */
        int32_t num_dyn_eviction_errors;                    /**< Number of dynamic eviction errors */
        uint8_t tap_next_free_sltr_num_pipe0_ingress;       /**< Next available SLTR number for Pipe-0 Ingress side */
        uint8_t tap_next_free_sltr_num_pipe0_egress;        /**< Next available SLTR number for Pipe-0 Egress side */
        uint8_t tap_next_free_sltr_num_pipe1_ingress;       /**< Next available SLTR number for Pipe-1 Ingress side */
        uint8_t tap_next_free_sltr_num_pipe1_egress;        /**< Next available SLTR number for Pipe-1 Egress side */
        uint8_t tap_ingress_opcode_ext_bmp[32];             /**< Opcode extension value bitmap for Ingress side */
        uint8_t tap_egress_opcode_ext_bmp[32];              /**< Opcode extension value bitmap for Egress side */
        uint64_t tap_32s_dyn_eviction_count;                /**< Dynamic eviction count for 32b single compressed counters */
        uint64_t tap_64p_dyn_eviction_count;                /**< Dynamic eviction count for 64b pair counters */
        uint64_t tap_zero_dyn_eviction_count;               /**< Zero-value dynmaic evicted counters. */
        uint64_t entry_cscan_count;                         /**< Number of Single Entry Counter Scan messages received */
        int32_t cs_sweep_size;                              /**< Counter Scan Sweep Size for OP2 debug */
        int32_t cs_read_size;                               /**< Counter Scan Read Size for OP2 debug */
        int32_t map_print_offset;                           /**< current device map print offset */
        uint32_t dba_error_cnt;
        uint32_t uda_error_cnt;
        uint32_t rpt_error_cnt;
        uint32_t uit_error_cnt;
        uint32_t rpt_uit_init_done;

        /* parity errors: op/op2*/
        uint32_t total_mem_errors_fixed;
        uint64_t mem_errors_locs[KAPS_MAX_PLACE_HOLDER_PARITY_FIFO_DEPTH * 2];
        uint64_t interface_errors[KAPS_MAX_INTF_ERROR_FIFO_DEPTH];

        struct kaps_db *bkp_db_list[64];        /* Stores the user added order of DB db list */
        uint32_t bkp_db_count;  /* Counts the number of DB in the above list */

        uint32_t max_possible_c0_to_c1_access;  /* Calculates how many DBs that can be accessed from core0 to C1 */
        uint32_t max_possible_c1_to_c0_access;  /* Calculates how many DBs that can be accessed from core1 to C0 */

        uint32_t num_rows_in_each_hb_block[KAPS_MAX_NUM_HB_BLOCKS];

        enum kaps2_hb_config_type kaps2_hb_config[KAPS_MAX_NUM_HB_BLOCKS];
    };

/**
 * Converts a ISSU dump file to a Human Readable Format
 *
 * @param bin_fp File pointer of the ISSU dump file
 * @param txt_fp File pointer of the Output file
 *
 * @retval KAPS_OK on success
 */
    kaps_status kaps_device_parse_state(
    FILE * bin_fp,
    FILE * txt_fp);

/**
 * Advanced initialization of device
 *
 * @param device the device for which to perform advanced initialization
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_init(
    struct kaps_device *device);

/**
 * Advanced properties for device
 *
 * @param device valid device handle
 * @param property the property to set
 * @param vl the variable arguments
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_set_property(
    struct kaps_device *device,
    uint32_t property,
    va_list vl);

/**
 * Advanced properties for device
 *
 * @param device valid device handle
 * @param property the property to set
 * @param vl the variable arguments
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_get_property(
    struct kaps_device *device,
    uint32_t property,
    va_list vl);

/**
 * Destroy any advanced device information
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_destroy(
    struct kaps_device *device);

/**
 * Advanced device lock operations
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_advanced_lock(
    struct kaps_device *device);

/**
 * Check LPM constraints
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_device_check_lpm_constraints(
    struct kaps_device *device);

/**
 * Performs advanced parts of the warmboot/ISSU Save
 *
 * @param device Valid device handle.
 * @param  wb_fun WB state
 *
 * @return KAPS_OK on success, or an error code otherwise.
 */
    kaps_status kaps_device_save_two_level_bb_info(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Performs advanced parts of the warmboot/ISSU Restore
 *
 * @param device Valid device handle.
 * @param wb_fun WB state
 *
 * @return KAPS_OK on success, or an error code otherwise.
 */
    kaps_status kaps_device_restore_two_level_bb_info(
    struct kaps_device *device,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Routine to enable/disable dynamic evictions during warmboot phase
 *
 * @param device Valid device handle
 * @param enable 1 to enable 0 to disable
 *
 * @returns KAPS_OK on success or en error code otherwise.
 */
    kaps_status kaps_device_advanced_enable_dynamic_evictions(
    struct kaps_device *device,
    uint32_t enable);

/**
 * Checks and marks a flag if database is either searched in parallel
 * with itself or with any of its clones/tables.
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_device_check_for_dup_db(
    struct kaps_device *device);

/**
 * Update Config Registers
 *
 * @param device valid device handle
 * @param enable_udc 1: Turn on LPU, 0: Turn off LPU
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_device_advance_update_config_registers(
    struct kaps_device *device,
    uint8_t enable_udc);

/**
 * KAPS Shadow initialization for algorithmic resources
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */
    kaps_status kaps_initialize_advanced_shadow(
    struct kaps_device *device);

/**
 * Logs the Error verbose string
 *
 * @param device valid device handle
 * @param return_status return status of this function
 * @param fmt Printf format strings
 *
 * @return return_status
 */

    kaps_status kaps_device_log_error(
    struct kaps_device *device,
    kaps_status return_status,
    char *fmt,
    ...);

/**
 * @endcond
 */

/*Used for broadcast cases.
Returns main broadcast device handle.
If broadcast is not enabled then it
returns the input device*/
    struct kaps_device *kaps_get_main_bc_device(
    struct kaps_device *device);

/**
 * Checks if minimum one instruction is needed in this device
 *
 * @param device valid device handle
 *
 * @returns 1 if minimum one instruction is needed and 0 otherwise
 *
 */
    int32_t kaps_check_if_inst_needed(
    struct kaps_device *device);


/**
* Function to obtain the starting block number of the last level in the device
*
* @param device Valid device handle.
* @param db valid database handle
*
* @return the starting block number of the last level
*/

uint32_t
kaps_device_get_final_level_offset(
    struct kaps_device *device,
    struct kaps_db *db);


/**
* Function that returns the number of BBs in the DEVICE (not database) in the final level of the BBs
*
* @param device Valid device handle.
* @param db valid database handle
*
* @return the number of BBs in the DEVICE (not database) in the final level of the BBs
*/

uint32_t
kaps_device_get_num_final_level_bbs(
    struct kaps_device *device,
    struct kaps_db *db);


/**
* Function that returns the active aging table for the database
*
* @param device Valid device handle.
* @param db valid database handle
*
* @return the active aging table for the database
*/
struct kaps_aging_entry*
kaps_device_get_active_aging_table(
    struct kaps_device *device,
    struct kaps_db *db);


/**
* Function that returns the number of hit bit blocks in final level in whole device
*
* @param device Valid device handle.
* @param db valid database handle
*
* @return the number of hit bit blocks in final level in whole device
*/

uint32_t 
kaps_device_get_active_num_hb_blocks(
    struct kaps_device *device, 
    struct kaps_db *db);


/**
* Function that returns the number of hit bit rows in final level in whole device
*
* @param device Valid device handle.
* @param db valid database handle
*
* @return he number of hit bit blocks in final level 
*/
uint32_t 
kaps_device_get_active_num_hb_rows(
    struct kaps_device *device,
    struct kaps_db *db);



/**
 * Initializes the KAPS device handling by reading the KAPS Revision Register
 *
 * @param device Valid device handle.
 *
 * @return KAPS_OK on success or an error code otherwise.
 */

    kaps_status kaps_dm_kaps_init(
    struct kaps_device *device);

/**
 * Writes data/mask of specified length(nbytes) to the specified row(entry_nr) and block(blk_num).
 * Data can be written in either DM or XY mode.
 * valid_bit is set 0 to invalidate, 3 to validate(1 and 2 are reserved) entries via this API
 *
 * @param device Valid device handle.
 * @param blk_num DBA block number
 * @param entry_nr Location of the entry in the DBA block
 * @param nbytes Number of Bytes of data to be written
 * @param data Valid data pointer to write
 * @param mask Valid mask pointer to write
 * @param is_xy Whether data and mask are specified in XY format or not
 * @param valid_bit 0 to invalidate the entry, 3 to validate (1 and 2 are reserved)
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
    kaps_status kaps_dm_kaps_dba_write(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t entry_nr,
    uint32_t nbytes,
    uint8_t * data,
    uint8_t * mask,
    uint8_t is_xy,
    uint8_t valid_bit);

/**
 * Reads data/mask of specified length(nbytes) from the specified row(row_num) and block(blk_num).
 *
 * @param device Valid device handle.
 * @param blk_num DBA block number
 * @param row_num Location of the entry in the DBA block
 * @param nbytes Number of bytes of data to read
 * @param o_entry_x entry structure to read x
 * @param o_entry_y entry structure to ready
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
    kaps_status kaps_dm_kaps_dba_read(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t entry_nr,
    struct kaps_dba_entry *o_entry_x,
    struct kaps_dba_entry *o_entry_y);

/**
* Function to write value to the specified register(reg_num)
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param reg_num valid reg_num to write
* @param nbytes the number of bytes to write
* @param data stream of data to write
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_reg_write(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * data);
/**
* Function to read value of the specified register(reg_num)
*
* @param device Valid device handle.
* @param blk_num the block number (used by KAPS)
* @param reg_num valid reg_num to read
* @param nbytes the number of bytes to be read
* @param o_data stream of read data
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_reg_read(
    struct kaps_device *device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * o_data);

 /**
 * Function to write ad to the specified UDA address(uda_addr)
 *
 * @param device Valid device handle.
 * @param ab_num the AB number (used by KAPS)
 * @param uda_addr the physical UDA address where the entry has to be written
 * @param nbytes the number of bytes to write
 * @param data stream of data to write
 *
 * @return KAPS_OK on success or an error code otherwise.
 */
    kaps_status kaps_dm_kaps_ad_write(
    struct kaps_device *device,
    uint32_t ab_num,
    uint32_t uda_addr,
    uint32_t nbytes,
    uint8_t * data);

/**
* Function to read ad from the specified UDA address(uda_addr)
*
* @param device Valid device handle.
* @param ab_num the AB number (used by KAPS)
* @param uda_addr the physical UDA address where the entry is written to is returned
* @param nbytes the number of bytes to be read
* @param o_data stream of read data
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_ad_read(
    struct kaps_device *device,
    uint32_t ab_num,
    uint32_t uda_addr,
    uint32_t nbytes,
    uint8_t * o_data);

/**
* Devmgr kaps Search function.
*
* @param device Valid device handle.
* @param key the data to be searched in the KAPS device. For example, if five bytes of data have to be searched,
*            these bytes will be stored in key[0] to key[4].
* @param search_interface the search interface (s0a...s1b) to be used to search the key.
* @param kaps_result the result of the KAPS search is returned here.
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_search(
    struct kaps_device *device,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result *kaps_result);

/**
* Devmgr kaps Hit Bit Read function
*
* @param device Valid device handle.
* @param db valid database handle
* @param block_num hit bit block number from where to read (starts from 0)
* @param row_num row in the hit bit block from where to read
* @param data  the read hit bit data will be stored here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_hb_read(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data);

/**
* Devmgr kaps Hit Bit Write function
*
* @param device Valid device handle.
* @param db valid database handle
* @param block_num hit bit block number where to write (starts from 0)
* @param row_num row in the hit bit block where to write
* @param data  the data to be written
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_hb_write(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data);

/**
* Devmgr kaps Hit Bit dump function
*
* @param device Valid device handle.
* @param db valid database handle
* @param start_blk_num starting hit bit block from where the hit bits should be collected. (starts from 0)
* @param start_row_num starting row number in the start hit bit block from where the hit bits should be collected
* @param end_blk_num ending hit bit block up to where the hit bits should be collected. (starts from 0)
* @param end_row_num ending row number in the end hit bit block up to where the hit bits should be collected
* @param clear_on_read clears the hit bits after reading them. Currently always set to 1
* @param data  the collected hit bits will be stored here
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_hb_dump(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t start_blk_num,
    uint32_t start_row_num,
    uint32_t end_blk_num,
    uint32_t end_row_num,
    uint8_t clear_on_read,
    uint8_t * data);

/**
* Devmgr kaps Hit Bit copy function
*
* @param device Valid device handle.
* @param db valid database handle
* @param src_blk_num source hit bit block from where the hit bits should be copied. (starts from 0)
* @param src_row_num row number in the source hit bit block from where the hit bits should be copied
* @param dest_blk_num destination hit bit block to where the hit bits should be copied. (starts from 0)
* @param dest_row_num row number in the destination  hit bit block to where the hit bits should be copied
* @param source_mask mask bits applied after reading hit bits before the rotate operation
* @param rotate_right number of bits by which the result should be rotated right
* @param perform_clear indicates if the hit bits at the source be cleared after the read
*
* @return KAPS_OK on success or an error code otherwise.
*/
    kaps_status kaps_dm_kaps_hb_copy(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t src_blk_num,
    uint32_t src_row_num,
    uint32_t dest_blk_num,
    uint32_t dest_row_num,
    uint16_t source_mask,
    uint8_t rotate_right,
    uint8_t perform_clear);

#ifdef __cplusplus
}
#endif

#endif                          /* __DEVICE_KAPS_INTERNAL_H */

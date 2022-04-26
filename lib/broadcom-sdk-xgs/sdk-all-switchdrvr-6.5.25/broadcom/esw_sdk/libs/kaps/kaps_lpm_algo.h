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

#ifndef __KAPS_LPM_ALGO_H
#define __KAPS_LPM_ALGO_H

#include "kaps_errors.h"
#include "kaps_device.h"
#include "kaps_resource.h"
#include "kaps_db_internal.h"
#include "kaps_pool.h"
#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_algo_common.h"
#include "kaps_hw_limits.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 *
 * @file lpm_algo.h
 *
 * API's to manage Algorithmic LPM's in KBP
 * @addtogroup NETLPM
 * @{
 */

    struct kaps_pfx_bundle;

#define MAX_COPIES_PER_LPM_DB   (2)

/**
 * Pointer to LPM entry from database list node
 */

#define DBLIST_TO_LPM_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_lpm_entry, db_node)

/**
 * @brief Specifies if the LPM entry is in pending list or committed to device
 */

    enum lpm_entry_status
    {
        PENDING = 0,     /**< No decisions made yet for the entry */
        COMMITTED = 1    /**< Entry is in device now */
    };

/**
 * @brief Represents LPM entry added by user
 *
 * Fields arranged by most frequently used entries first
 */

    struct kaps_lpm_entry
    {
        KAPS_ENTRY_STRUCT_MEMBERS uint32_t meta_priority:2;
                              /**< Meta priority of the entry */
        struct kaps_pfx_bundle *pfx_bundle;
                                        /**< prefix data */
    };

    struct kaps_lpm_cr_entry_info
    {
        volatile uint8_t marked_for_delete;  /**< Marked for delete */
        volatile uint8_t status;  /**< Status of entry, declared in kaps_cr_entry_status */
        volatile uint8_t seq_num; /**< Table seq num */
        volatile uint32_t new_index;
                                  /**< New Index */
        uint32_t entry_user_handle;  /**< Actual Entry user handle */
        uint32_t ad_user_handle;  /**< Actual AD user handle */
        uint32_t hb_user_handle;  /**< Actual HB user handle */
        uint32_t hb_bit_no;       /**< HB bit no */
    };

/**
 * @brief LPM database handle.
 *
 * Its possible that LPM
 * databases get mapped to ACL databases, in which
 * case we want to transparently map one to another.
 * For this we maintain the look of the LPM database
 * identical to that of ACL database. The LPM
 * database structure may be slightly larger, however
 * the basic fields match
 */
    struct kaps_lpm_db
    {

        /*
         * ACL/LPM and EM common information 
         */
        struct kaps_db db_info;        /**< Common database information */
        struct kaps_acl_db_common *common;
                                       /**< All tables point to same information */
        struct range_mgr *range_mgr;  /**< Range manager per table */

        /*
         * LPM specific information 
         */
        struct kaps_fib_tbl *fib_tbl;/**< Fib table handle */
        struct kaps_fib_tbl *core0_fib_tbl;/**< Fib table handle */
        struct kaps_fib_tbl *core1_fib_tbl;/**< Fib table handle */
        struct kaps_db **resolution_dbs;
                                     /**< Array of the databases which are paricipating in resolution along with this current db */
        struct kaps_c_list pending;     /**< Entries that have not yet
                                        been handled by the algorithm */
        struct kaps_c_list tmp_list;     /**< Internal usage*/

        uint16_t core0_is_full;      /**< Internal statistics */
        uint16_t core1_is_full;      /**< Internal statistics */
        uint16_t current_core;                    /**< on which core operation is performed (OP2 Specific)  */
        uint16_t num_used_ab;      /**< Internal statistics */
        uint16_t num_released_ab;  /**< Internal statistics */
        uint16_t num_used_ait;     /**< Internal statistics */
        uint32_t used_uda_kb;      /**< Internal statistics */

        uint16_t num_ab_no_compression;      /**< Internal statistics */
        uint16_t num_ab_compression_1;       /**< Internal statistics */
        uint16_t num_ab_compression_2;       /**< Internal statistics */
        uint8_t num_resolution_db;           /**< Size / Number of databases of resolution_db array */
        uint8_t meta_priority_map;           /**< it is 4 bit map
                                                                                        if bit 0 set then priority is 0
                                                                                        if bit 1 set then priority is 1
                                                                                        if bit 2 set then priority is 2
                                                                                        if bit 3 set then priority is 3  */
        uint8_t is_entry_delete_in_progress; /**< Is entry delete in progress*/
        uint8_t multiple_meta_priority;      /**< If set to 1, then user has added entries with more than 1 meta priority */
            POOL_DEF(
    kaps_lpm_entry) lpm_entry_pool;             /**< Memory pool for LPM entry structures */
    };

    struct kaps_small_bb_stats
    {
        uint32_t numBricksForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];/**< Number of bricks for each granularity*/
        uint32_t numEmptyBricksForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];
                                                                  /**< Number of empty bricks for each granularity */
        uint32_t numPfxForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];/**< Number of prefixes for each granularity*/
        uint32_t numHolesForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];
                                                             /**< Number of holes in each granularity*/
        uint32_t numLsnForEachSize[KAPS_HW_MAX_LPUS_PER_LPM_DB];
                                                              /**< Number of LSN's  of each size */
        uint32_t numHolesForEachLsnSize[KAPS_HW_MAX_LPUS_PER_LPM_DB];
                                                                   /**< Number of Holes of each LSN size */
        uint32_t totalNumEntriesInLsns;
                                      /**< Total number of holes across all LSNs */
        uint32_t totalNumHolesInLsns;
                                    /**< Total number of holes across all LSNs */
        uint32_t maxLsnWidth;       /**< Maximum width of an LSN*/
        uint32_t maxBricks;
                        /**< Maxl Number of bricks*/
        uint32_t numUsedBricks;
                            /**< Total Number of used bricks*/
        uint32_t numUsedButEmptyBricks;
                                    /**< Total Number of used bricks that have no prefixes */
        uint32_t numFreeBricks;
                            /**< Total Number of bricks that are free */
    };

/**
 * @brief Statistics for all the tables under an LPM Manager
 */
    struct NlmFibStats
    {
        uint32_t numRPTWrites;   /**< Number of RPT write operations performed*/
        uint32_t numRITWrites;   /**< Number of RIT write operations performed*/
        uint32_t numIITWrites;   /**< Number of IIT (ADS for KAPS) write operations performed*/
        uint32_t numIPTWrites;   /**< Number of IPT (RPB for KAPS) write operations performed*/
        uint32_t numAPTWrites;   /**< Number of APT write operations performed*/
        uint32_t numAITWrites;   /**< Number of AIT write operations performed*/
        uint32_t numHitBitReads; /**< Number of hit bit read operations performed */
        uint32_t numHitBitWrites;/**< Number of hit bit write operations performed */
        uint32_t numHitBitCopy;  /**< Number of hit bit copy operations performed */
        uint32_t numRptSplits;   /**< Number of RPT Splits Performed */
        uint32_t numRptPushes;   /**< Number of RPT Push Operations performed */
        uint32_t numRptMoves;    /**< Number of RPT Moves Performed */
        uint32_t numLsnGiveOuts; /**< Number of LSN Giveouts */
        uint32_t numLsnPush;     /**< Number of LSN Push Operations */
        uint32_t numLsnMerges;   /**< Number of LSN Merge operations with ancestor nodes*/
        uint32_t numRelatedLsnMerges;
                                 /**< Number of LSN Merge Operations with related nodes */
        uint32_t numLsnShrinks;  /**< Number of LSN shrink operations */
        uint32_t numIXCompactionCalls;
                                   /**< Number of IX compaction calls */
        uint32_t numUDACompactionCalls;
                                    /**< Number of UDA compaction calls */
        uint32_t numUDADynamicAllocCalls;
                                      /**< Number of UDA compaction calls */
        uint32_t numUDAExpansionCalls;
                                   /**< Number of UDA compaction calls */
        uint32_t numLSNSbc;     /**< Number of LSN Sub Block Copy operations performed*/
        uint32_t numLSNRmw;     /**< Number of LSN Read Modify Write (Bucket Write for KAPS) operations performed*/
        uint32_t numLSNClr;     /**< Number of LSN Clear operations performed*/
        uint32_t numMetaDataWrite;
                                /**< Number of writes of the LSN Metadata */
        uint32_t numPIOWrites;  /**< Number of DBA/IT write operations performed*/
        uint32_t numADWrites;   /**< Number of AD writes*/
        uint32_t numOfIPTShuffle;
                                /**< Number of IPT (RPB for KAPS) shuffles*/
        uint32_t numOfAPTShuffle;
                                /**< Number of APT shuffles*/
        uint32_t numAptEntriesAdded;/**< Number of APT entries added*/
        uint32_t numAptEntriesDeleted;
                                    /**< Number of APT entries deleted*/
        uint32_t numLmpsofarPfx;    /**< Number of user added Lmpsofar prefixes present*/
        uint32_t numAptLongPfx;     /**< Number of long APT prefixes present*/
        uint32_t numLmpsofarHitInSearch;
                                       /**< Number of Lmpsofars searched */
        uint32_t numMissesInSearch;     /**< Number of Misses while performing searches */
        uint32_t totalRptBytesLoppedOff;
                                       /**< Total number of RPT bytes lopped off in the whole trie*/
        uint32_t maxNumRptSplitsInOneOperation;
                                            /**< Maximum number of RPT splits performed in a single operation*/
        uint32_t maxNumIptSplitsInOneOperation;
                                            /**< Maximum number of IPT splits performed in a single operation*/
        uint32_t numRecursiveRptSplits;     /**< If more than one RPT is split in an insert/delete, it is a recursive RPT split*/
        uint32_t numRecursiveIptSplits;     /**< If more than one IPT entry is split in an insert/delete, then it is a recursive IPT split*/

        uint32_t numBricksForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];/**< Number of bricks for each granularity*/
        uint32_t numEmptyBricksForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];
                                                                  /**< Number of empty bricks for each granularity */
        uint32_t numPfxForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];/**< Number of prefixes for each granularity*/
        uint32_t numHolesForEachGran[KAPS_HW_MAX_NUM_LPU_GRAN];
                                                             /**< Number of holes in each granularity*/
        uint32_t numLsnForEachSize[KAPS_HW_MAX_LPUS_PER_LPM_DB];
                                                              /**< Number of LSN's  of each size */
        uint32_t numHolesForEachLsnSize[KAPS_HW_MAX_LPUS_PER_LPM_DB];
                                                                   /**< Number of Holes of each LSN size */
        uint32_t totalNumHolesInLsns;
                                    /**< Total number of holes across all LSNs */

        uint32_t numHolesInLastBrick; /**< Number of holes in the last brick*/
        uint32_t numHolesInNonLastBricks; /**< Number of holes in the non-last bricks*/

        uint32_t numLsnsWithPrefixMatchingDepth; /**< Number of LSNs where the prefix which exactly matches the LSN depth is present */
        uint32_t numLsnsThatCanbePushedDeeper; /**< Number of LSNs that can be pushed deeper */

        uint32_t maxLsnWidth;       /**< Maximum width of an LSN*/
        uint32_t initialLsnWidth;

        uint32_t numRegions;/**< Total number of regions*/
        uint32_t numUsedBricks;
                            /**< Total Number of used bricks*/
        uint32_t numUsedButEmptyBricks;
                                    /**< Total Number of used bricks that have no prefixes */
        uint32_t numPartiallyOccupiedBricks;
                                         /**< Total Number of bricks that are partially occupied */
        uint32_t numLastBricksThatAreEmpty; /**< Total number of last bricks that are empty*/
        uint32_t numNonLastBricksThatAreEmpty; /**< Total number of non-last bricks that are empty*/
                                         
        uint32_t numFreeBricks;
                            /**< Total Number of bricks that are free */

        uint32_t numUsedUitBanks;
                              /**< Number of used UIT banks*/

        uint32_t numTrieNodes;
                            /**< Number of trie nodes present */
        uint32_t numRPTEntries;
                            /**< Number of RPT entries present */
        uint32_t numIPTEntries;
                            /**< Number of IPT entries present */
        uint32_t numAPTLmpsofarEntries;
                                    /**< Number of APT Lmpsofar entries */

        uint32_t numUsedAds2Slots; /**< Number of used ADS-2 slots*/

        uint32_t numFreeAds2Slots; /**< Number of free ADS-2 slots*/

        uint32_t rmw_lpus[KAPS_HW_MAX_LPUS_PER_LSN * 8];
                                                     /**< LSNNW  number of LPUs used */
        uint32_t sbc_lpus[KAPS_HW_MAX_LPUS_PER_LSN]; /**< LSNSBC number of LPUs used */
        uint32_t sbc_clr_lpus[KAPS_HW_MAX_LPUS_PER_LSN]; /**< LSNCLR number of LPUs used */
        uint32_t clr_lpus[KAPS_HW_MAX_LPUS_PER_LSN]; /**< LSNCLR number of LPUs used */

        uint32_t rpt_lopoff_info[KAPS_LPM_KEY_MAX_WIDTH_8 + 1];
                                                          /**< Number of  RPT entries for different RPT lengths lopped off*/
        struct kaps_small_bb_stats kaps_small_bb_stats;

        uint32_t rxcTotalNumABAssigned;

        uint32_t numTotalIptAB;
        uint32_t numTotal80bIptAB;
        uint32_t numTotal160bIptAB;
        uint32_t numTotal80bSmallIptAB;
        uint32_t numTotal160bSmallIptAB;
        uint32_t numDupIptAB;

        uint32_t numTotalAptAB;
        uint32_t numTotal80bAptAB;
        uint32_t numTotal160bAptAB;
        uint32_t numTotal80bSmallAptAB;
        uint32_t numTotal160bSmallAptAB;
        uint32_t numDupAptAB;

        uint32_t num40bTriggers;
        uint32_t num80bTriggers;
        uint32_t num160bTriggers;
        uint32_t avgTrigLen;

        uint32_t maxNumRptSplitsInANode;
        uint32_t depthofNodeWithMaxRptSplits;
        uint32_t idOfNodeWithMaxRptSplits;

        uint32_t numTriggersInAB[KAPS_MAX_NUM_POOLS];
        uint32_t widthOfAB[KAPS_MAX_NUM_POOLS];
        uint32_t isABUsed[KAPS_MAX_NUM_POOLS];
        uint32_t isIPTAB[KAPS_MAX_NUM_POOLS];
        uint32_t numColsInAB[KAPS_MAX_NUM_POOLS];
    };

/**
 * @brief Represents LPM Manager
 *
 */
    struct kaps_lpm_mgr
    {
        struct kaps_fib_tbl_mgr *fib_tbl_mgr;
                                         /**< Pointer to fib tbl mgr */
        struct kaps_db *resource_db;         /**< The db in lpm_mgr which has resources for all dbs in lpm_mgr*/
        struct kaps_db *curr_db;        /**< Current database handle */
        kaps_db_index_callback callback_fn;
                                        /**< Index changed callback function */
        void *callback_handle;         /**< User provided handle */
        uint32_t is_config_locked;     /**< Has configuration been locked */
    };

/**
 *  Get the statistics specific to the LPM database. The function kaps_lpm_db_stats should be called prior to calling this function
 *
 * @param lpm_mgr valid LPM Manager handle
 * @param fib_stats_pp the fib stats are returned here
 *
 * @return KAPS_OK on success or an error code
*/
    kaps_status kaps_lpm_db_get_fib_stats(
    struct kaps_lpm_db *db,
    struct NlmFibStats *fib_stats_pp);

    kaps_status kaps_lpm_db_print_fib_stats(
    struct kaps_db *db,
    struct NlmFibStats *fib_stats_p,
    struct kaps_db_stats *user_stats_p);

    struct kaps_wb_prefix_info
    {
        uint8_t pfx_len;
        uint8_t pfx_copy;
        uint8_t ad_seq_num;
        uint8_t meta_priority;
        uint8_t core_id;
        uint32_t index;
        uintptr_t user_handle;
    };

    struct kaps_wb_prefix_data
    {
        struct kaps_wb_prefix_info info;
        uint32_t ad_user_handle;
        uint32_t hb_no;
        uint32_t hb_user_handle;
        uint8_t *data;
        uint8_t *ad;
        struct kaps_list_node kaps_list_node;
    };

#define WBPFXLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_wb_prefix_data, kaps_list_node);


    struct large_kaps2_stats
    {
        uint32_t total_rpb_rows;
        uint32_t num_rpb_rows_used;

        uint32_t total_small_bb;
        uint32_t num_small_bb_used;

        uint32_t total_ad2_rows;
        uint32_t num_ad2_rows_used;

        uint32_t total_large_bb_rows;
        uint32_t num_large_bb_rows_used;
    };




/**
 *  Re-Maps a prefix bundle from LSN to the correct Entry and Associated Data during Crash Recovery
 *
 * @param fib_tbl valid kaps_fib_tbl handle
 * @param newPfxBundle the prefix Bundle to be remapped
 * @param is_pending Indicates whether the prefix was pending before Crash
 *
 * @return KAPS_OK on success or an error code
*/
    kaps_status kaps_lpm_cr_remap_prefix(
    struct kaps_fib_tbl *fib_tbl,
    kaps_pfx_bundle * newPfxBundle,
    uint32_t * is_pending);



    void kaps_ftm_large_kaps2_calc_stats(
    struct kaps_db *db,
    struct large_kaps2_stats *stats);

    void kaps_kaps_print_detailed_stats_html(
    FILE * f,
    struct kaps_device *dev,
    struct kaps_db *db);

    void kaps_lpm_replace_device(
    struct kaps_db *db,
    int32_t core);


    kaps_status kaps_db_is_reduced_algo_levels(
    struct kaps_db *db);

/**
 *@}
 */

#ifdef __cplusplus
}
#endif
#endif                          /* __LPM_ALGO_H */

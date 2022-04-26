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

#ifndef INCLUDED_KAPS_POOLMGR_H
#define INCLUDED_KAPS_POOLMGR_H

#include "kaps_fib_trienode.h"
#include "kaps_fib_ipm.h"
#include "kaps_device.h"
#include "kaps_hw_limits.h"
#include "kaps_it_mgr.h"
#include "kaps_algo_common.h"
#include "kaps_lpm_algo.h"

/**
 * @cond INTERNAL
 */

/**
 * @addtogroup POOL_MGR
 * @{
 */

/**
 * A pool is a collection of one or more Array Blocks that are used for the particular purpose. The pool is assigned
 * a dba manager which manages the shuffles in the Array Block.
 *
 * Currently we have two types of pools:
 * - the IPT pool for storing the Intermediate Prefix Table entries
 * - the APT pool for storing the Auxillary Prefix Table entries.
 *
 * In the current code, a pool is restricted to a single Array Block. All the IPT entries of an RPT entry are stored in an
 * IPT pool and all the APT entries of an RPT entry are stored in an APT pool. An IPT pool can consist of IPT entries
 * of more than one RPT entry. Similarly the APT pool can consist of APT entries of more than one RPT entry. Since
 * a pool can have the IPT/APT entries of multiple RPT entries, an 8 bit RPT ID is added to MS byte of the IPT/APT entries.
 * An exception to this is KAPS. KAPS doesn't have RPT entries and so doesn't require the RPT ID to be added.
 *
 * The IPT and APT pool selection has now been decoupled and each can be chosn independantly provided that there
 * is a common RPT ID between the two
 *
 * IMPORTANT TERMINOLOGY
 * pool id: a single identifier that indicates the IPT pool index and the APT pool index the entries are present for the RPT trie node
 *              the pool id is constructed by using the IPT pool index and the APT pool index
 *
 * pool index: indicates which IPT pool or APT pool.
 */

/**
 * Maximum number of IPT entries in any Pool. Since max width of an IPT entry is 160b, max num entries is 2K
 */
#define KAPS_MAX_NUM_IPT_ENTRIES_PER_POOL    (2*1024)


/**
 * Invalid pool index
 */
#define KAPS_INVALID_POOL_INDEX  (0xFFFF)

/**
 * Invalid Array Block number
 */
#define KAPS_INVALID_BLK_NUM     (0xFFFF)

/**
 * Constructs the pool id from the IPT pool index
 */
#define KAPS_CONSTRUCT_POOL_ID(iptPoolIndex) (iptPoolIndex)

/**
 * Extracts the IPT pool index from the pool id
 */
#define KAPS_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId) (poolId & 0xFFFF)

/**
 * @brief Stores the RPT trie nodes in the pool
 */
typedef struct kaps_rpts_in_pool
{
    kaps_trie_node *m_rptTrieNode_p; /**< Pointer to the RPT trie node whose IPT or APT entries are in the pool*/
    struct kaps_rpts_in_pool *m_next_p; /**< Pointer that indicates the next RPT trie node */
} kaps_rpts_in_pool;

/**
 * @brief Indicates the pool type
 */
typedef enum kaps_pool_type
{
    KAPS_IPT_POOL,   /**< Pool Intermediate Prefix Table entries*/
} kaps_pool_type;

/**
 * @brief Information for a single pool
 */
typedef struct kaps_pool_info
{
    struct kaps_fib_dba *m_dba_mgr;  /**< The DBA Manager that manages the shuffling of entries in the pool*/
    kaps_pool_type m_poolType;     /**< Indicates whether IPT or APT Block*/
    kaps_rpts_in_pool *m_rptsInPool_p;  /**< List of RPT trie nodes whose IPT or APT entries are in the pool*/
    uint8_t m_rptIdsFree[MAX_NUM_RPT_ENTRIES_IN_POOL / KAPS_BITS_IN_BYTE]; /**< Bitmap where 1 indicates RPT ID is free*/
    uint8_t m_isInUse;                /**< Indicates if the pool is being used or not. If isInUse is 0, then we should not
                                                                        access the remaining fields in the pool info*/
    uint32_t m_poolIndex;
    uint32_t m_curNumRptEntries;        /**< Number of RPT entries in the pool*/
    uint32_t m_curNumDbaEntries;        /**< Number of entries that are currently present in the pool*/
    uint32_t m_maxNumDbaEntries;        /**< Maximum number of entries that the pool can store*/
    uint32_t m_maxAllowedNumDbaEntries; /**< Number of entries that pool can store after deducting the always empty slots*/
    uint32_t m_width_1;                 /**< Width of the array block assigned to the pool*/
    uint32_t m_isReservedPool;          /**< This is a reserved pool for storing 160b triggers */
    uint32_t m_isWidthLocked;           /**< Since all the ABs in a super block have same width, if we use the first 2 ABs as reserved 160b ABs, then the
                                                                        next 2 ABs should also be 160b and so their width is locked*/
} kaps_pool_info;

/**
 * @brief Status of the DBA blocks
 */
typedef enum kaps_pool_blk_status
{
    BLK_UNAVAILABLE,    /**< DBA Block is not available to be used by the pool manager */
    BLK_FREE,           /**< DBA Block is free */
    BLK_ALLOCATED       /**< DBA Block has been allocated*/
} kaps_pool_blk_status;

/**
 *@brief Manager for the Pools
 */
typedef struct kaps_pool_mgr
{
    kaps_pool_info m_ipmPoolInfo[KAPS_MAX_NUM_POOLS];   /**< Info about all the IPT pools */
    kaps_pool_info m_apmPoolInfo[KAPS_MAX_NUM_POOLS];   /**< Info about all the APT pools*/
    kaps_nlm_allocator *m_alloc_p;                      /**< Memory allocator*/
    kaps_fib_tbl_mgr *fibTblMgr;                        /**< Handle to the FIB table manager*/
    struct it_mgr *m_itMgr;                         /**< Pointer to the Information Table (IT) Manger */
    struct kaps_ab_info *abs_allocated[KAPS_MAX_NUM_POOLS];   /**< DBA blocks assigned to the pool */
    kaps_pool_blk_status m_dbaBlkStatus[KAPS_MAX_NUM_POOLS]; /**< Status of the DBA Array Blocks on the device */
    uint32_t m_numActiveIptPools;                   /**< Number of IPT pools that have been created*/
    uint32_t m_numActiveAptPools;                   /**< Number of APT pools that have been created*/
    uint32_t m_curNumRptEntriesInAllPools;          /**< Current num of RPT nodes whose IPT/APT entries are in the pools*/
    uint32_t m_numFreeAB;                           /**< Current number of unassigned Array Blocks*/
    uint32_t m_totalNumAB;                          /**< Number of Array Blocks (free + active) assigned to Pool Manager*/
    uint32_t m_numABsReleased;                      /**< Number of ABs that have been used and then been freed completely */
    uint32_t m_force_trigger_width;                 /**< Set to 1 when we wanted to force the trigger length */
    uint32_t m_cur_max_trigger_length_1;            /**< Current Maximum Trigger Length to configure the AB */
    uint32_t m_are_pools_updated;                   /**< Are pools has updated in the nv */
} kaps_pool_mgr;

/**
 *@brief Warmboot place holder for pool information
 */
struct kaps_wb_pool_info
{
    kaps_pool_type pool_type;      /**< Indicates whether IPT or APT Pool */
    uint32_t pool_ix;           /**< Pool index assigned to the pool */
    uint32_t blk_num;           /**< DBA block number assigned to this pool */
    uint32_t m_width_1;         /**< Width of the array block assigned to the pool*/
    uint32_t m_start_offset_1;  /**< Start byte in AB used for Trigger Compression */
    uint32_t m_isReservedPool;  /**< Indicates if it is a pool for reserved 160b triggers */
    uint32_t m_isWidthLocked;   /**< The width of the pool is locked and can't be changed. This is needed since all ABs in a Super Block have same width
                                                        and we now have support for 160b Reserved ABs */
};

/**
 * Initialize the pool manager
 *
 * @param fibTblMgr_p valid FIB Table Manager
 * @param alloc_p valid allocator to use for memory allocations
 * @param abs_list linked list specifying ABs allocated to the pool
 *
 * @return handle to the pool manager that was created
 */
kaps_pool_mgr *kaps_pool_mgr_create(
    kaps_fib_tbl_mgr * fibTblMgr_p,
    kaps_nlm_allocator * alloc_p,
    uint32_t is_cascaded,
    struct kaps_c_list *ab_list);

/**
 * Assign one or two pools to an RPT trie node. It can be an already allocated pool or a new pool. For KAPS only one
 * pool should be requested. For NL12K and Oprime two pools should be requested
 *
 * @param poolMgr valid pool manager handle
 * @param rptTrieNode_p the RPT entry for which the pools are being requested
 * @param numIptEntriesRequired number of IPT entries under the RPT entry
 * @param numAptEntriesRequired number of APT entries under the RPT entry
 * @param numPools number of pools requested (should be either 1 or 2)
 * @param ab_width_1 width of the pool in bits
 * @param was_dba_dynamic_alloc returns 1 if a new set of DBA ABs have been dynamically allocated for the db
 * @param o_reason returns the reason code
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_assign_pool_to_rpt_entry(
    kaps_pool_mgr * poolMgr,
    kaps_trie_node * rptTrieNode_p,
    uint32_t numIptEntriesRequired,
    uint32_t numAptEntriesRequired,
    uint32_t numPools,
    uint32_t ab_width_1,
    int32_t * was_dba_dynamic_allocated,
    NlmReasonCode * o_reason);

/**
 * Releases the pools for the RPT entry. This is typically called when the RPT entry has to be moved to a new location.
 *
 * @param poolMgr valid pool manager handle
 * @param rptTrieNode_p the RPT entry which is being removed from the pool
 * @param poolId the pool from which the RPT entry should be removed
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_release_pool_for_rpt_entry(
    kaps_pool_mgr * poolMgr,
    kaps_trie_node * rptTrieNode_p,
    uint32_t oldPoolId,
    uint32_t oldRptId);

/**
 * Increments the number of entries in the IPT or APT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool for which the increment should be done
 * @param incr the number of entries to be incremented
 * @param poolType whether IPT or APT pool
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_incr_num_entries(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    uint32_t incr,
    kaps_pool_type poolType);

/**
 * Decrements the number of entries in the IPT or APT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool for which the increment should be done
 * @param decr the number of entries to be decremented
 * @param poolType whether IPT or APT pool
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_decr_num_entries(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    uint32_t decr,
    kaps_pool_type poolType);

/**
 * Returns the number of user added entries currently in the IPT or APT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool identifier
 * @param poolType whether IPT or APT pool
 *
 * @return number of entries present in the IPT or APT pool
 */
uint32_t kaps_pool_mgr_get_num_entries(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_pool_type poolType);

/**
 * Returns the number of allowed entries (max capacity - number of always empty slots) in the IPT or APT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool identifier
 * @param poolType whether IPT or APT pool
 *
 * @return number of allowed entries in the IPT or APT pool
 */
uint32_t kaps_pool_mgr_get_max_allowed_entries(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_pool_type poolType);

/**
 * Returns the maximum capacity of the IPT or APT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool identifier
 * @param poolType whether IPT or APT pool
 *
 * @return the maximum capacity of the IPT or APT pool
 */
uint32_t kaps_pool_mgr_get_max_entries(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_pool_type poolType);

/**
 * Returns the RPT trie node which has the most IPT or APT entries
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool identifier
 * @param poolType whether IPT or APT pool
 *
 * @return the RPT trie node with the most IPT or APT entries
 */
kaps_trie_node *kaps_pool_mgr_get_rpt_with_most_entries(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_pool_type poolType);

/**
 * Returns the dba manager for the IPT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool identifier
 *
 * @return the dba manager for the IPT pool
 */
kaps_ipm *kaps_pool_mgr_get_ipm_for_pool(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId);

/**
 * Returns the number of active IPT or APT pools
 *
 * @param poolMgr valid pool manager handle
 * @param poolType whether IPT or APT pool
 *
 * @return the number of active IPT or APT pools
 */
uint32_t kaps_pool_mgr_get_num_active_pools(
    kaps_pool_mgr * poolMgr,
    kaps_pool_type poolType);

/**
 * Returns the maximum capacity of the IPT or APT pool
 *
 * @param poolMgr valid pool manager handle
 * @param poolId the pool identifier
 * @param poolType whether IPT or APT pool
 *
 * @return the maximum capacity of the IPT or APT pool
 */
uint32_t kaps_pool_mgr_get_num_rpt_entries_in_pool(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_pool_type poolType);

/**
 * Destroys the pool manager
 *
 * @param poolMgr handle to the pool manager to be destroyed
 * @param alloc_p valid allocator to use for memory allocations
 *
 * @return None
 */
void kaps_pool_mgr_destroy(
    kaps_pool_mgr * poolMgr,
    kaps_nlm_allocator * alloc_p);

/**
 * Creates data to be written for AB which is using trigger compression
 *
 * @param poolMgr handle to the pool manager
 * @param ab_num the array block in which the entry is present
 * @param row_nr the row in the array block in which the entry is present
 * @param data Data array
 * @param mask Mask array
 * @param is_xy Data is XY mode or not
 * @param reason code for the operation
 *
 * @return NLMERR_OK on sucess, error otherwise
 */
NlmErrNum_t kaps_pool_mgr_createEntryData(
    kaps_pool_mgr * poolMgr,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t * is_xy,
    NlmReasonCode * o_reason);

/**
 * Verifies the pool manager
 *
 * @param poolMgr handle to the pool manager to be verified
 *
 * @return None
 */
void kaps_pool_mgr_verify(
    kaps_pool_mgr * poolMgr);

/**
 * Returns the DBA usage stats of the pool manager
 *
 * @param poolMgr handle to the pool manager
 * @param cur_num_entries_in_db current number of entries in the entire database (includes all tables inside the db)
 * @param numOfABsUsed returns number of Array Blocks used for all the allocated pools
 * @param numOfAitUsed returns the number of AIT locations used
 * @param num_ab_no_compression returns the number of ABs in no compression mode
 * @param num_ab_compression_1 returns the number of ABs in compression mode 1
 * @param num_ab_compression_2 returns the number of ABs in compression mode 2
 * @param is_dba_brimmed returns if DBA resources are nearly full
 * @param num_abs_released number of ABs which have been used and then freed up
 *
 * @return None
 */
void kaps_pool_mgr_get_dba_stats(
    kaps_pool_mgr * poolMgr,
    struct kaps_db *parent_db,
    uint32_t avg_num_pfx_per_brick,
    uint32_t cur_num_entries_in_db,
    uint32_t * numOfABsUsed,
    uint32_t * numOfAitUsed,
    uint32_t * num_ab_no_compression,
    uint32_t * num_ab_compression_1,
    uint32_t * num_ab_compression_2,
    uint32_t * is_dba_brimmed,
    uint32_t * num_abs_released,
    uint32_t * extra_entry_estimate,
    uint32_t log_info);

/**
 * Saves active IPT and APT pools
 *
 * @param pool_mgr Handle to the pool manager
 * @param write_fn Callback function to write data to non-volatile memory
 * @param handle User handle which will be passed back through read and write functions
 * @param nv_offset The offset in the non-volatile memory to write to
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_wb_save_pools(
    kaps_pool_mgr * pool_mgr,
    kaps_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset);

/**
 * Restores active IPT and APT pools
 *
 * @param pool_mgr Handle to the pool manager
 * @param read_fn Callback function to read data from non-volatile memory
 * @param handle User handle which will be passed back through read and write functions
 * @param nv_offset The offset in the non-volatile memory to read from
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_wb_restore_pools(
    kaps_pool_mgr * pool_mgr,
    kaps_device_issu_read_fn read_fn,
    void *handle,
    uint32_t * nv_offset);

/**
 * Adds RPT node to the pool. This node is added to IPT and/or APT pools. RPT node's pool id
 * decides whether this RPT node belongs to IPT or APT or to both
 *
 * @param pool_mgr Handle to the pool manager
 * @param rpt_node RPT node to add to the pool
 *
 * @return NLMERR_OK on success or error code
 */
NlmErrNum_t kaps_pool_mgr_wb_add_rpt_entry_to_pool(
    kaps_pool_mgr * pool_mgr,
    kaps_trie_node * rpt_node);



/**
 * Saves active IPT and APT pools During Crash Recovery
 *
 * @param pool_mgr Handle to the pool manager
 * @param wb_fun Callback function to read/write data to non-volatile memory
 *
 */
void kaps_pool_mgr_cr_save_pools(
    kaps_pool_mgr * pool_mgr,
    struct kaps_wb_cb_functions *wb_fun);

/**
 * Restores IPT and APT pools During Crash Recovery
 *
 * @param pool_mgr Handle to the pool manager
 * @param wb_fun Callback function to read/write data to non-volatile memory
 *
 */
void kaps_pool_mgr_cr_restore_pools(
    kaps_pool_mgr * pool_mgr,
    struct kaps_wb_cb_functions *wb_fun);

int kaps_pool_mgr_is_pool_in_use(
    kaps_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_pool_type poolType);

void kaps_pool_mgr_get_pool_stats(
    kaps_pool_mgr * poolMgr,
    struct NlmFibStats *fibStats);

/**
 * @}
 */

/**
 * @endcond
 */

#endif

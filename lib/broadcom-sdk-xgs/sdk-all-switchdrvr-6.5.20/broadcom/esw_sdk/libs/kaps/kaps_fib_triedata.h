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

/* This file contains definition of the following objects:
 *  kaps_lpm_trie:  Object to store the trie.
 */

#ifndef INCLUDED_KAPS_TRIEDATA_H
#define INCLUDED_KAPS_TRIEDATA_H

#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_portable.h"
#include "kaps_fib_trienode.h"
#include "kaps_fib_rpm.h"
#include "kaps_uda_mgr.h"
#include "kaps_fibmgr_xk.h"
#include "kaps_fib_hw.h"
#include "kaps_pfx_hash_table.h"

#include "kaps_externcstart.h"

#define KAPS_DEFAULT_RPT_DEPTH       (8)

#define KAPS_JERICHO_PLUS_FM4_MAX_KEY_WIDTH_1 (148)

#define KAPS_MAX_TMAX_VAL  ((uint16_t)(KAPS_HW_LPU_WIDTH_1 / KAPS_HW_LPM_SMALLEST_GRAN ) * KAPS_HW_MAX_LPUS_PER_DB)

#define KAPS_MIN_ENTRIES_NEEDED_FOR_SPLIT (3)

#define KAPS_TRIE_FIX_POOLS_THRESHOLD (1000)

#define KAPS_MAX_NUM_RPT_UUID (KAPS_HW_MAX_PCM_ENTRIES)

#define KAPS_MAX_NUM_TRIE_NODES_FOR_RELATED_MERGE (10)

#define KAPS_TRIE_NUMSTACK   3  /* stacks used for iterative recursion */

#define KAPS_MAX_SIZE_OF_TRIE_ARRAY (256)

#define KAPS_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY ((32 * 1024) + 1024)

#define KAPS_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY (32 * 1024)

typedef struct kaps_ipt_brick_scratchpad
{
    uint32_t m_numOccupiedEntries[KAPS_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_maxNumEntries[KAPS_HW_MAX_LPUS_PER_LPM_DB];
    uint32_t m_gran[KAPS_HW_MAX_LPUS_PER_LPM_DB];

    uint32_t m_numActiveBricks;

    uint32_t m_prefixLens[KAPS_HW_MAX_LPUS_PER_LPM_DB * KAPS_MAX_PFX_PER_BKT_ROW];
    uint32_t m_numPfx;

} kaps_ipt_brick_scratchpad;

enum KAPS_RPT_OPERATION
{
    KAPS_RPT_MOVE,
    KAPS_RPT_SPLIT,
    KAPS_RPT_PUSH
};

/* Data structure used for global trie */
typedef struct kaps_trie_global
{
    kaps_nlm_allocator *m_alloc_p;

    struct kaps_fib_tbl_mgr *fibtblmgr;
    struct kaps_uda_mgr *m_mlpmemmgr[KAPS_MAX_NUM_DEVICES];
    int8_t is_common_uda_mgr_used[KAPS_MAX_NUM_DEVICES];

    kaps_pfx_bundle *m_tempBrickArray[1024];
    kaps_pfx_bundle *m_bricksPfxBuffer[4 * 1024];
    kaps_rpm *m_rpm_p;

    uint32_t m_oldPoolId;
    uint32_t m_oldRptId;
    uint32_t m_old_rpt_uuid;

    uint32_t m_newPoolId;
    uint32_t m_newRptId;
    uint32_t m_new_rpt_uuid;

    uint32_t m_oldRptDepth;
    uint32_t m_newRptDepth;

    enum KAPS_RPT_OPERATION m_rptOp;

    uint32_t m_indexBufferIter;
    uint32_t *m_indexBuffer_p;
    uint8_t *m_isReserved160bTrigBuffer;

    uint32_t m_deleteRptAptLmpsofar;

    uint32_t m_maxIptEntriesForRptMove;
    uint32_t m_maxAptEntriesForRptMove;

    uint32_t m_iptEntriesMoveDelta;

    uint32_t m_numDeletes;

    struct kaps_pool_mgr *poolMgr;

    kaps_trie_node *m_rptNodesToSplitOrMove[KAPS_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_rpt_nodes_to_split_or_move;

    struct kaps_trie_node *m_curRptNodeToSplit_p;
    struct kaps_trie_node *m_curRptNodeToPush_p;

    kaps_trie_node *m_newRptNode_p;

    uint32_t m_areAllPoolsHealthy;

    uint32_t m_isNearestIptAncestorSplit:1;
    uint32_t m_isAptPresent:1;
    uint32_t m_isIITLmpsofar:1;
    uint32_t m_isLsnLmpsofar:1;
    uint32_t m_pfxLocationsToMove[KAPS_MAX_TMAX_VAL];

    kaps_pfx_bundle *m_extraPfxArray[KAPS_MAX_TMAX_VAL];        /* < Array for storing the prefixes from the IPT
                                                                 * ancestor when we merge related IPT entries */

    kaps_trie_node *m_trieNodesForRelatedMerge[KAPS_MAX_NUM_TRIE_NODES_FOR_RELATED_MERGE];

    uint32_t m_numTrieNodesForMerge;

    /*
     * Array used for AB enable in the PCT for the main database
     */
    uint8_t ab_enable[KAPS_HW_MAX_AB / KAPS_BITS_IN_BYTE];

    /*
     * Array used for AB enable in the PCT for the cloned database
     */
    uint8_t ab_enable2[KAPS_HW_MAX_AB / KAPS_BITS_IN_BYTE];

    /*
     * warmboot save/restore callback functions
     */
    struct kaps_wb_cb_functions *wb_fun;

    struct kaps_ad_db *cur_ad_db;
    struct kaps_lsn_mc *curLsnToSplit;  /* LSN Currently we are preforming Giveout */

    kaps_trie_node *m_rptNodeForCalcIptApt;

    uint32_t useTrigLenForCalcIptApt;

    uint32_t use_reserved_abs_for_160b_trig;

    uint32_t m_are_reserved_160b_abs_initialized;

    uint32_t m_max_reserved_160b_trig_per_rpt;

    uint32_t m_move_reserved_160b_trig_to_regular_pool;

    uint32_t poolIdsForReserved160bTrig[2];

    int32_t m_rpt_uuid_table[KAPS_MAX_NUM_RPT_UUID];    /* 1 indicates the uuid is available, 0 indicates that the uuid
                                                         * is taken */

    uint32_t m_numReserved160bThatBecameRegular;

    uint32_t m_numRegularThatBecameReserved160b;

} kaps_trie_global;

/* Data structure used for trie */
typedef struct kaps_lpm_trie
{
    kaps_trie_global *m_trie_global;
    kaps_fib_tbl *m_tbl_ptr;
    struct kaps_lsn_mc_settings *m_lsn_settings_p;
    struct kaps_trie_node *m_roots_trienode_p;

    struct kaps_trie_node *m_cache_trienode_p;

    kaps_pfx_bundle *m_toadd_ses[KAPS_MAX_SIZE_OF_TRIE_ARRAY];  /* List of Ses Entries to be added */
    uint32_t m_num_items_in_to_add_ses;

    kaps_pfx_bundle *m_todelete_ses[KAPS_MAX_SIZE_OF_TRIE_ARRAY];       /* list of Ses entries to be deleted */
    uint32_t m_num_items_in_to_delete_ses;

    kaps_trie_node *m_toupdate_sit[KAPS_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY];       /* List of SIT Entries to be added */
    uint32_t m_num_items_in_to_update_sit;

    struct kaps_lsn_mc *m_toupdate_lsns[KAPS_MAX_SIZE_OF_TRIE_ARRAY];   /* list of LSNs to be commited to h/w */
    uint32_t m_num_items_in_to_update_lsns;

    struct kaps_lsn_mc *m_todelete_lsns[KAPS_MAX_SIZE_OF_TRIE_ARRAY];   /* list of LSNs to be deleted */
    uint32_t m_num_items_in_to_delete_lsns;

    kaps_trie_node *m_todelete_nodes[KAPS_MAX_SIZE_OF_TRIE_ARRAY];      /* Trie nodes to be deleted */
    uint32_t m_num_items_in_to_delete_nodes;

    kaps_trie_node *m_toupdate_rit[KAPS_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY];
    uint32_t m_num_items_in_to_update_rit;

    void *m_pendingGiveouts[KAPS_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_pending_giveouts;

    void *m_completedGiveouts[KAPS_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_completed_giveouts;

    kaps_trie_node *m_delayedTrieNode[KAPS_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_delayed_trie_node;

    struct kaps_lsn_mc *m_delayedLsn[KAPS_MAX_SIZE_OF_TRIE_ARRAY];
    uint32_t m_num_items_in_delayed_lsn;

    struct kaps_trie_node **m_recurse_stack[KAPS_TRIE_NUMSTACK];
    NlmNsTrieNode__TraverseArgs *m_recurse_args_stack[KAPS_TRIE_NUMSTACK];
    uint8_t m_recurse_stack_inuse[KAPS_TRIE_NUMSTACK];
    uint8_t m_recurse_args_stack_inuse[KAPS_TRIE_NUMSTACK];
    struct kaps_trie_node **m_ancestors_space;  /* Space used for calculating ancestors */
    struct kaps_ix_mgr *m_ixMgr;
    struct pfx_hash_table *m_hashtable_p;
    kaps_pfx_bundle *m_uwcLmpsofarPfx;  /* User prefix bundle with copies that acts as lmpsofar */
    uint8_t m_doverify;         /* flag indicates verification is turned on */
    uint8_t m_alloc_failed;     /* the most recent AllocResoruces() failed */
    uint8_t m_min_lopoff;       /* Minimum lopoff that should be present in the trie */
    uint8_t m_expansion_depth;  /* depth to which the trie gets expanded to. It should be >= m_min_lopoff */
    uint8_t m_isRptSplitDueToIpt;
    uint8_t m_isRptSplitDueToApt;
    uint8_t m_ancestors_space_inuse;
    uint32_t m_numOfMCHoles;
    uint32_t m_maxDepth;
    uint32_t m_index_in_inst;
    uint32_t m_maxAllowedIptEntriesForRpt;
    uint32_t m_maxAllowedAptEntriesForRpt;
    uint32_t m_uwcLmpsofarIndex;        /* Index of the user prefix with copies that acts as lmpsofar */
    uint32_t m_isRecursiveSplit:1;
    uint32_t m_isCompactionRequired:1;
    uint32_t m_isCompactionEnabled:1;
    uint32_t m_isOverAllocEnabled:1;
    uint32_t m_isCopyIptAndAptToNewPoolInProgress:1;

    /*
     * Suppose we are doing an IPT split. The new IPT entry may be long and result in a granularity change. In this
     * case we try to move all the IPT entries to a new pool which can satisfy the new granularity. However if we can't
     * move to a new pool because the granularity is too high, then we try to do an RPT split/move. Now we should not
     * do a move and instead force a split. This is because move failed just earlier. Also the new granularity
     * calculated during IPT split is no longer stored. So a move should not be done. For this reason, we are making
     * use of m_rpt_move_failed_during_ipt_gran_change
     */
    uint32_t m_rpt_move_failed_during_ipt_gran_change;

        /**/ uint32_t m_numExtraIptEntriesNeededDuringGiveout;

    struct kaps_instruction *m_instruction;

    uint8_t *m_iptHitBitBuffer;
    uint8_t *m_rptTcamHitBitBuffer;

    NlmReasonCode m_rptSplitReasonCode;

    kaps_pfx_bundle *m_defaultRptPfxBundle_p[KAPS_MAX_CASCADED_DEVICES];

    kaps_ipt_brick_scratchpad m_iptBrickScratchpad;

} kaps_lpm_trie;

/**
 *@brief Warmboot place holder for RPT entry
 */
struct kaps_wb_rpt_entry_info
{
    uint32_t rpt_entry_len_1;                                         /**< Entry length in bits */
    uint8_t rpt_entry[KAPS_RPB_WIDTH_8];                              /**< RPT entry */
    uint32_t rpt_entry_location;                                      /**< Where this rpt entry is stored */
    uint16_t rpt_id;                                                  /**< RPT id of this entry */
    uint16_t rpt_uuid;                                                /**< Universally unique RPT ID*/
    uint32_t pool_id;                                                 /**< To which pool this rpt entry belongs to */
    uint32_t db_id;                                                   /**< to which database this rpt entry belongs to */
    uint32_t num_ipt_entries;                                         /**< Number of ipt entries under this rpt entry */
    uint32_t num_apt_entries;                                         /**< Number of apt entries under this rpt entry */
    uint32_t num_reserved_160b_trig;                                  /**< Number of reserved 160b triggers in this rpt entry */
};

/**
 *@brief Warmboot place holder for LPU brick
 */

struct kaps_wb_lpu_brick_info
{
    uint16_t gran;                 /**< LPU Brick granularity */
    uint16_t gran_ix;              /**< LPU Brick granularity  index*/
    uint32_t base_address;         /**<Base address of the LPU Brick */
    uint8_t meta_priority;      /* Meta priority associated with brick */
    uint8_t m_hasReservedSlot;  /* If is has reserved slot */
    uint8_t m_underAllocatedIx; /* Have we under allocated the Ix */
    uint16_t brick_ix_allocated;   /**<IX space allocated to the brick */
    struct kaps_ad_db *ad_db;       /**< AD DB associated to the brick */
};

/**
 *@brief Warmboot place holder for LSN
 */

struct kaps_wb_lsn_info
{
    uint32_t lsn_id;
    uint8_t num_lpus;                                                    /**< Number of LPUs in the LSN */
    uint8_t region_id;                                                   /**< Region id of the LSN uda chunk */
    uint32_t offset;                                                     /**<Start offset of the LSN uda chunk*/
    uint32_t base_address;                                               /**<Base address of the LSN */
    uint16_t ix_allocated;                                               /**<IX space allocated to the LSN */
    struct kaps_wb_lpu_brick_info brick_info[KAPS_HW_MAX_LPUS_PER_LPM_DB];     /**< LPU bricks info */
    struct kaps_wb_lpu_brick_info special_entry_info;
};

/**
 *@brief Warmboot place holder for IPT entry
 */
struct kaps_wb_ipt_entry_info
{
    uint8_t ipt_entry_len_1;                                   /**< Entry length in bits */
    uint8_t is_reserved_160b_trig;                            /**< Indicates if the IPT entry is a reserved 160b trigger*/
    uint8_t ipt_entry[KAPS_LPM_KEY_MAX_WIDTH_8];  /**< IPT entry */
    uint16_t blk_num;                                          /**< This ipt entry resides in this block */
    uint16_t addr;                                             /**< Takes values between 0 to (4k -1) */
    struct kaps_wb_lsn_info lsn_info;                               /**< LSN information */
};

struct kaps_trie_node **kaps_trie_get_stack_space(
    kaps_lpm_trie * self);

void kaps_trie_return_stack_space(
    kaps_lpm_trie * self,
    struct kaps_trie_node **stk);

void kaps_trie_node_return_stack_space_args(
    kaps_lpm_trie * self,
    NlmNsTrieNode__TraverseArgs * stk);

NlmNsTrieNode__TraverseArgs *kaps_trie_node_get_stack_space_args(
    kaps_lpm_trie * self);

void kaps_trie_incr_num_ipt_entries_in_subtrie(
    kaps_trie_node * node,
    uint32_t value);

void kaps_trie_decr_num_ipt_entries_in_subtrie(
    kaps_trie_node * node,
    uint32_t value);

uint16_t kaps_trie_get_num_rpt_bits_lopped_off(
    struct kaps_device *device,
    uint32_t depth);

NlmErrNum_t kaps_trie_compute_rpt_parent(
    kaps_trie_node * trienode,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_trie_commit_rpt_and_rit(
    kaps_trie_node * rptNode_p,
    NlmReasonCode * o_reason);

void kaps_trie_propagate_rpt_attributes(
    kaps_trie_node * curNode_p,
    kaps_trie_node * rptNode_p);

NlmErrNum_t kaps_trie_process_rpt_split_or_move(
    kaps_lpm_trie * self_p,
    NlmReasonCode * o_reason_p,
    uint32_t force_rpt_split);

NlmErrNum_t kaps_trie_obtain_rpt_uuid(
    kaps_trie_node * rptTrieNode);

NlmErrNum_t kaps_trie_release_rpt_uuid(
    kaps_trie_node * rptTrieNode);

NlmErrNum_t kaps_trie_get_algo_hit_bit_value(
    kaps_lpm_trie * trie,
    struct kaps_lpm_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value);

NlmErrNum_t kaps_trie_convert_kbp_status_to_err_num(
    kaps_status status,
    NlmReasonCode * o_reason_p);

#include <kaps_externcend.h>

#endif /* INCLUDED_NLMNSTRIEDATA_H */

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

#include "kaps_fib_trie.h"
#include "kaps_fib_cmn_prefix.h"
#include "kaps_fib_cmn_nstblrqt.h"
#include "kaps_fibmgr.h"
#include "kaps_fibmgr_xk.h"
#include "kaps_fib_cmn_seqgen.h"
#include "kaps_ix_mgr.h"
#include "kaps_fib_poolmgr.h"
#include "kaps_fib_rpm.h"
#include "kaps_arena.h"
#include "kaps_device_internal.h"
#include "kaps_device_internal.h"
#include "kaps_fib_lsnmc_hw.h"
#include "kaps_fib_hw.h"
#include "kaps_fib_cmn_wb.h"
#include "kaps_algo_hw.h"
#include "kaps_ad_internal.h"
#include "kaps_key_internal.h"
#include "kaps_handle.h"
#include "kaps_ix_mgr.h"
#include "kaps_fib_lsnmc_recover.h"

uint32_t g_submitRqtHitCnt;

#define NLMNS_TRIE_DEFAULT_LOPOFF (8)

#define NLMNS_MAX_INDEX_BUFFER_SIZE (2 * KAPS_MAX_NUM_IPT_ENTRIES_PER_POOL)

#define NLMNSTRIE_MAX_PFX_BUNDLE_WORD (KAPS_LPM_KEY_MAX_WIDTH_1 / 32 + sizeof(kaps_pfx_bundle) / 4 + 4)

static NlmErrNum_t kaps_trie_pvt_in_place_delete(
    kaps_lpm_trie * self,
    kaps_trie_node * node,
    uint32_t isMoveToNewPoolAllowed,
    NlmReasonCode * o_reason);

static NlmErrNum_t kaps_trie_ctor(
    kaps_lpm_trie * trie_p,
    kaps_trie_global * global,
    void *fibTbl_p,
    NlmReasonCode * o_reason);

static kaps_trie_node *kaps_trie_find_dest_lp_node(
    kaps_lpm_trie * self,
    kaps_trie_node * start_node,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    kaps_trie_node ** last_seen_node,
    int32_t isPfxInsertion);

static NlmErrNum_t kaps_trie_update_hardware(
    kaps_lpm_trie * self,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_trie_split_rpt_node(
    kaps_lpm_trie * trie_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_trie_add_ses(
    kaps_lpm_trie * self,
    kaps_trie_node * trienode,
    kaps_pfx_bundle * pfxBundle,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_trie_giveout_pending_lsns(
    kaps_lpm_trie * trie_p,
    kaps_lsn_mc * originalLsn,
    struct kaps_lpm_entry *entryToInsert,
    uint32_t tryOnlyLsnPush,
    NlmReasonCode * o_reason);

static void kaps_trie_pvt_cleanup_del_nodes(
    kaps_lpm_trie * trie_p);

NlmErrNum_t kaps_trie_pvt_merge_ancestor_lsns(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * destnode,
    NlmReasonCode * o_reason);

void kaps_trie_verify_iit_lmpsofar(
    kaps_trie_node * node_p,
    kaps_trie_node * ancestorIptNode_p);

void kaps_trie_verify_rpt_apt_lmpsofar(
    kaps_trie_node * cur_node,
    kaps_trie_node * highestPriorityAptTrieNode);

void kaps_trie_verify_down_stream_rpt(
    kaps_trie_node * curTrieNode,
    kaps_trie_node * nearestRptAncestor);

void kaps_trie_verify_dba_reduction(
    kaps_trie_node * node_p);

void kaps_trie_verify_kaps_joined_udcs(
    kaps_trie_node * node_p);

void kaps_trie_verify_num_ipt_and_apt_in_rpt_node(
    kaps_trie_node * curNode);

void kaps_trie_verify_kaps_ab_and_pool_mapping(
    kaps_lpm_trie * trie_p);

void kaps_trie_verify_lsn(
    kaps_trie_node * curNode);

NlmErrNum_t kaps_trie_check_rpt_resources_for_new_lsn(
    kaps_trie_node * trienode,
    NlmReasonCode * o_reason);

void kaps_trie_process_merge_after_giveout(
    kaps_lpm_trie * self);

uint32_t kaps_trie_check_if_related_ipt_nodes_can_be_merged(
    kaps_trie_node * originalIptNode,
    kaps_trie_node ** trieNode2_pp,
    kaps_trie_node ** newTrieNode_pp);

NlmErrNum_t kaps_trie_merge_related_lsns(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * trieNodeToMerge1,
    kaps_trie_node * trieNodeToMerge2,
    kaps_trie_node * newIptTrieNode,
    NlmReasonCode * o_reason);

void kaps_trie_wb_verify(
    kaps_trie_node * trie_node);

void
kaps_trie_decr_num_ipt_entries_in_subtrie(
    kaps_trie_node * node,
    uint32_t value)
{
    kaps_assert(node->m_numIptEntriesInSubtrie >= value, "Num IPT entries in subtrie is becoming negative");
    node->m_numIptEntriesInSubtrie -= value;
}

void
kaps_trie_incr_num_ipt_entries_in_subtrie(
    kaps_trie_node * node,
    uint32_t value)
{
    node->m_numIptEntriesInSubtrie += value;
}

uint16_t
kaps_trie_get_num_rpt_bits_lopped_off(
    struct kaps_device *device,
    uint32_t depth)
{
    uint16_t numRptBitsLoppedOff = 0;

    (void) device;

    /*
     * if (device->type == KAPS_DEVICE_KAPS) { numRptBitsLoppedOff = depth; } else
     */
    {

        numRptBitsLoppedOff = (uint16_t) (depth / 8);

        numRptBitsLoppedOff = numRptBitsLoppedOff * 8;

        if (numRptBitsLoppedOff > MAX_RPT_LOPOFF_IN_BYTES * 8)
            numRptBitsLoppedOff = MAX_RPT_LOPOFF_IN_BYTES * 8;
    }

    return numRptBitsLoppedOff;
}

NlmErrNum_t
kaps_trie_obtain_rpt_uuid(
    kaps_trie_node * rptTrieNode)
{
    kaps_trie_global *trie_global = rptTrieNode->m_trie_p->m_trie_global;
    uint32_t i;

    if (rptTrieNode->m_rpt_uuid == NLM_INVALID_RPT_UUID)
    {
        for (i = 0; i < KAPS_MAX_NUM_RPT_UUID; ++i)
        {
            if (trie_global->m_rpt_uuid_table[i])
            {
                rptTrieNode->m_rpt_uuid = i;
                trie_global->m_rpt_uuid_table[i] = 0;
                break;
            }
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_release_rpt_uuid(
    kaps_trie_node * rptTrieNode)
{
    kaps_trie_global *trie_global = rptTrieNode->m_trie_p->m_trie_global;
    uint32_t i;

    if (rptTrieNode->m_rpt_uuid != NLM_INVALID_RPT_UUID)
    {
        i = rptTrieNode->m_rpt_uuid;
        trie_global->m_rpt_uuid_table[i] = 1;
    }

    rptTrieNode->m_rpt_uuid = NLM_INVALID_RPT_UUID;

    return NLMERR_OK;
}

uint32_t
kaps_trie_is_big_kaps_it_full(
    kaps_lpm_trie * trie_p)
{
    uint32_t isITFull = 0;
    struct kaps_device *device = trie_p->m_trie_global->fibtblmgr->m_devMgr_p;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;
    uint32_t ads2_num;

    if (db->parent)
        db = db->parent;

    ads2_num = 1;
    if (db->is_type_a)
    {
        ads2_num = 0;
    }

    if (device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots == 0)
    {
        isITFull = 1;
    }

    return isITFull;
}

NlmErrNum_t
kaps_trie_big_kaps_get_trigger_lengths_into_scratchpad(
    kaps_lpm_trie * trie_p,
    uint32_t numRptBitsLoppedoff,
    uint32_t originalPoolId,
    uint32_t addTriggerForOriginalLsn,
    uint32_t newTriggerPfxLen)
{
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    kaps_ipm *ipm = kaps_pool_mgr_get_ipm_for_pool(poolMgr, originalPoolId);
    uint32_t maxNumDbaEntries = kaps_pool_mgr_get_max_entries(poolMgr, originalPoolId, KAPS_IPT_POOL);
    int32_t i;
    uint32_t trigger_length_1;
    kaps_flat_lsn_data *curGiveoutData_p;
    kaps_pfx_bundle *pfxBundle;
    uint32_t numPfx;

    /*
     * Re initialize the IPT Brick Scratchpad
     */
    kaps_memset(&trie_p->m_iptBrickScratchpad, 0, sizeof(kaps_ipt_brick_scratchpad));

    /*
     * Copy the prefix lengths of the triggers in Simple DBA into the Ipt Brick Scratchpad
     */
    numPfx = 0;
    for (i = 0; i < maxNumDbaEntries; ++i)
    {
        pfxBundle = ipm->kaps_simple_dba.pfx_bundles[i];

        if (pfxBundle)
        {
            /*
             * Do not add 1 bit MPE length here
             */
            trie_p->m_iptBrickScratchpad.m_prefixLens[numPfx] = pfxBundle->m_nPfxSize - numRptBitsLoppedoff;
            numPfx++;
        }
    }

    /*
     * Copy the lengths of the new triggers formed during giveout into the Brick Scratchpad
     */
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        /*
         * Do not add 1 bit MPE length here
         */
        trigger_length_1 = curGiveoutData_p->m_commonPfx->m_inuse - numRptBitsLoppedoff;

        trie_p->m_iptBrickScratchpad.m_prefixLens[numPfx] = trigger_length_1;
        numPfx++;
    }

    if (addTriggerForOriginalLsn)
    {
        trigger_length_1 = newTriggerPfxLen - numRptBitsLoppedoff;
        trie_p->m_iptBrickScratchpad.m_prefixLens[numPfx] = trigger_length_1;
        numPfx++;
    }

    trie_p->m_iptBrickScratchpad.m_numPfx = numPfx;

    return NLMERR_OK;
}

/*Calculate how many bricks we need to store the triggers based on information in the Ipt Brick Scratchpad*/
NlmErrNum_t
kaps_trie_find_num_bricks_for_big_kaps_triggers(
    kaps_lpm_trie * trie_p)
{
    uint32_t cur_brick_nr;
    int32_t i, j;
    uint32_t numPfx, numEntriesPerBrick;
    uint32_t found;
    uint32_t cur_trigger_length, cur_trigger_gran;

    numPfx = trie_p->m_iptBrickScratchpad.m_numPfx;

    if (numPfx == 0)
        return NLMERR_OK;

    /*
     * Sort the trigger prefix bundles in ascending order
     */
    for (i = 0; i < numPfx - 1; ++i)
    {
        for (j = i + 1; j < numPfx; ++j)
        {
            if (trie_p->m_iptBrickScratchpad.m_prefixLens[i] > trie_p->m_iptBrickScratchpad.m_prefixLens[j])
            {
                uint32_t temp = trie_p->m_iptBrickScratchpad.m_prefixLens[i];
                trie_p->m_iptBrickScratchpad.m_prefixLens[i] = trie_p->m_iptBrickScratchpad.m_prefixLens[j];
                trie_p->m_iptBrickScratchpad.m_prefixLens[j] = temp;
            }
        }
    }

    /*
     * Add the entries into bricks and figure out the brick granularities
     */
    for (i = 0; i < numPfx; ++i)
    {
        /*
         * First find a slot of the prefix in the bricks whose granularities have already been configured
         */
        found = 0;
        for (cur_brick_nr = 0; cur_brick_nr < trie_p->m_iptBrickScratchpad.m_numActiveBricks; ++cur_brick_nr)
        {

            if (trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr] <
                trie_p->m_iptBrickScratchpad.m_maxNumEntries[cur_brick_nr])
            {

                if (trie_p->m_iptBrickScratchpad.m_prefixLens[i] < trie_p->m_iptBrickScratchpad.m_gran[cur_brick_nr])
                {
                    /*
                     * The prefix is smaller than gran of brick and it will fit inside the current brick 
                     */
                    trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr]++;
                    found = 1;
                    break;
                }

                /*
                 * Check if we can re-configure the granularity of the current brick to the granularity of the current
                 * trigger
                 */
                cur_trigger_length = trie_p->m_iptBrickScratchpad.m_prefixLens[i] + 1;  /* Add 1 for MPE */
                cur_trigger_gran = trie_p->m_lsn_settings_p->m_middleLevelLengthToGran[cur_trigger_length];

                numEntriesPerBrick = (KAPS_BKT_WIDTH_1 / (cur_trigger_gran + KAPS_AD_WIDTH_1));

                if (numEntriesPerBrick > KAPS_MAX_PFX_PER_BKT_ROW)
                    numEntriesPerBrick = KAPS_MAX_PFX_PER_BKT_ROW;

                if (numEntriesPerBrick >= trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr] + 1)
                {
                    trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr]++;
                    trie_p->m_iptBrickScratchpad.m_maxNumEntries[cur_brick_nr] = numEntriesPerBrick;
                    trie_p->m_iptBrickScratchpad.m_gran[cur_brick_nr] = cur_trigger_gran;
                    found = 1;
                    break;
                }
            }
        }

        if (!found)
        {
            /*
             * We searched all the active bricks but couldn't find a slot. So configure a new brick for the trigger
             */
            cur_brick_nr = trie_p->m_iptBrickScratchpad.m_numActiveBricks;
            cur_trigger_length = trie_p->m_iptBrickScratchpad.m_prefixLens[i] + 1;      /* Add 1 for MPE */
            cur_trigger_gran = trie_p->m_lsn_settings_p->m_middleLevelLengthToGran[cur_trigger_length];

            numEntriesPerBrick = (KAPS_BKT_WIDTH_1 / (cur_trigger_gran + KAPS_AD_WIDTH_1));

            if (numEntriesPerBrick > KAPS_MAX_PFX_PER_BKT_ROW)
                numEntriesPerBrick = KAPS_MAX_PFX_PER_BKT_ROW;

            trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr] = 1;
            trie_p->m_iptBrickScratchpad.m_maxNumEntries[cur_brick_nr] = numEntriesPerBrick;
            trie_p->m_iptBrickScratchpad.m_gran[cur_brick_nr] = cur_trigger_gran;

            trie_p->m_iptBrickScratchpad.m_numActiveBricks++;
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_add_to_down_stream_rpt(
    kaps_trie_node * upStreamRptTrieNode,
    kaps_trie_node * downStreamRptTrieNode,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc = upStreamRptTrieNode->m_trie_p->m_trie_global->m_alloc_p;
    NlmNsDownStreamRptNodes *newListNode;

    newListNode = kaps_nlm_allocator_calloc(alloc, 1, sizeof(NlmNsDownStreamRptNodes));

    if (!newListNode)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    newListNode->rptNode = downStreamRptTrieNode;
    newListNode->next = upStreamRptTrieNode->m_downstreamRptNodes;
    upStreamRptTrieNode->m_downstreamRptNodes = newListNode;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_update_down_stream_rpt_during_rpt_split(
    kaps_trie_node * upStreamRptTrieNode,
    kaps_trie_node * newRptTrieNode,
    NlmReasonCode * o_reason)
{
    NlmNsDownStreamRptNodes *curListNode, *prevListNode, *nextListNode;
    uint32_t rearrange;
    NlmErrNum_t errNum;

    /*
     * upStreamRptTrieNode should contain the new RPT trie node in its down stream list
     */
    errNum = kaps_trie_add_to_down_stream_rpt(upStreamRptTrieNode, newRptTrieNode, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    /*
     * We may have to move some of the RPT nodes from the down stream list of the upStreamRptTrieNode to the down
     * stream list of the newRptTrieNode. This can happen if the newRptTrieNode is sandwiched between two RPT nodes
     */
    curListNode = upStreamRptTrieNode->m_downstreamRptNodes;
    prevListNode = NULL;
    while (curListNode)
    {
        nextListNode = curListNode->next;

        rearrange = 0;
        if (curListNode->rptNode != newRptTrieNode)
        {
            if (kaps_prefix_pvt_is_more_specific_equal
                (curListNode->rptNode->m_lp_prefix_p->m_data, curListNode->rptNode->m_lp_prefix_p->m_nPfxSize,
                 newRptTrieNode->m_lp_prefix_p->m_data, newRptTrieNode->m_lp_prefix_p->m_nPfxSize))
            {
                rearrange = 1;
            }
        }

        if (rearrange)
        {
            /*
             * Remove the node from the upStreamRptTrieNode and add it to the newRptTrieNode
             */
            curListNode->next = newRptTrieNode->m_downstreamRptNodes;
            newRptTrieNode->m_downstreamRptNodes = curListNode;

            if (!prevListNode)
            {
                upStreamRptTrieNode->m_downstreamRptNodes = nextListNode;
            }
            else
            {
                prevListNode->next = nextListNode;
            }

        }
        else
        {
            prevListNode = curListNode;
        }

        curListNode = nextListNode;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_update_down_stream_rpt_for_new_rpt(
    kaps_trie_node * curTrieNode,
    kaps_trie_node * newRptNode,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    if (!curTrieNode)
        return NLMERR_OK;

    if (curTrieNode->m_isRptNode)
    {
        errNum = kaps_trie_add_to_down_stream_rpt(newRptNode, curTrieNode, o_reason);
        return errNum;
    }

    if (curTrieNode->m_child_p[0])
    {
        errNum = kaps_trie_update_down_stream_rpt_for_new_rpt(curTrieNode->m_child_p[0], newRptNode, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
    }

    if (curTrieNode->m_child_p[1])
    {
        errNum = kaps_trie_update_down_stream_rpt_for_new_rpt(curTrieNode->m_child_p[1], newRptNode, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_update_down_stream_rpt_for_rpt_delete(
    kaps_trie_node * upStreamRptTrieNode,
    kaps_trie_node * rptTrieNodeToDelete,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc = rptTrieNodeToDelete->m_trie_p->m_trie_global->m_alloc_p;
    NlmNsDownStreamRptNodes *curListNode, *prevListNode, *nextListNode;

    (void) o_reason;

    /*
     * If the upstream RPT Trie node doesn't exist, then delete the downstream RPT nodes in the list of
     * rptTrieNodeToDelete and return
     */
    if (!upStreamRptTrieNode)
    {
        curListNode = rptTrieNodeToDelete->m_downstreamRptNodes;

        while (curListNode)
        {
            nextListNode = curListNode->next;
            kaps_nlm_allocator_free(alloc, curListNode);
            curListNode = nextListNode;
        }

        rptTrieNodeToDelete->m_downstreamRptNodes = NULL;
        return NLMERR_OK;
    }

    /*
     * Add the RPT nodes in the down stream list of the RPT node being deleted to the up stream RPT node
     */
    curListNode = rptTrieNodeToDelete->m_downstreamRptNodes;
    if (curListNode)
    {
        /*
         * Find the last node
         */
        while (curListNode->next)
        {
            curListNode = curListNode->next;
        }

        /*
         * Link the last node in RPT node being deleted to the head of the upstream RPT node list 
         */
        curListNode->next = upStreamRptTrieNode->m_downstreamRptNodes;
        upStreamRptTrieNode->m_downstreamRptNodes = rptTrieNodeToDelete->m_downstreamRptNodes;
    }

    rptTrieNodeToDelete->m_downstreamRptNodes = NULL;

    /*
     * Remove the RPT trie node being deleted from the down stream list of the up stream RPT node
     */
    curListNode = upStreamRptTrieNode->m_downstreamRptNodes;
    prevListNode = NULL;
    while (curListNode)
    {
        if (curListNode->rptNode == rptTrieNodeToDelete)
        {
            break;
        }

        prevListNode = curListNode;
        curListNode = curListNode->next;
    }

    if (!curListNode)
    {
        kaps_assert(0, "Unable to find the downstream RPT node to be deleted \n");
        return NLMERR_FAIL;
    }

    if (!prevListNode)
    {
        upStreamRptTrieNode->m_downstreamRptNodes = curListNode->next;
    }
    else
    {
        prevListNode->next = curListNode->next;
    }

    kaps_nlm_allocator_free(alloc, curListNode);

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_trie_pvt_add_rpt_nodes_to_split_or_move(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * rptNode_p)
{
    uint32_t max, loopvar;
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    kaps_trie_node *iterRptNode_p;
    struct kaps_db *db = trie_global->fibtblmgr->m_curFibTbl->m_db;

    if (db->num_algo_levels_in_db == 2)
        return NLMERR_OK;

    max = trie_global->m_num_items_in_rpt_nodes_to_split_or_move;

    for (loopvar = 0; loopvar < max; loopvar++)
    {
        iterRptNode_p = trie_global->m_rptNodesToSplitOrMove[loopvar];
        if (iterRptNode_p == rptNode_p)
            return NLMERR_OK;
    }

    if (max >= KAPS_MAX_SIZE_OF_TRIE_ARRAY)
        kaps_assert(0, "m_rptNodesToSplitOrMove overflow \n");

    trie_global->m_rptNodesToSplitOrMove[max] = rptNode_p;
    trie_global->m_num_items_in_rpt_nodes_to_split_or_move++;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_trie_pvt_get_new_pool(
    kaps_pool_mgr * poolMgr,
    kaps_trie_node * rptTrieNode_p,
    uint32_t numIptEntriesRequired,
    uint32_t numAptEntriesRequired,
    uint32_t numPools,
    NlmReasonCode * o_reason)
{
    uint32_t max_trigger_len_1;
    kaps_fib_tbl *fibTbl = rptTrieNode_p->m_trie_p->m_tbl_ptr;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    int32_t was_dba_dynamic_alloc = 0;
    struct kaps_db *db = fibTbl->m_db;

    if (db->num_algo_levels_in_db == 2)
    {
        errNum = kaps_pool_mgr_assign_pool_to_rpt_entry(poolMgr, rptTrieNode_p, numIptEntriesRequired,
                                                        numAptEntriesRequired, numPools, KAPS_RPB_WIDTH_1,
                                                        &was_dba_dynamic_alloc, o_reason);
        return errNum;
    }

   
    {
        kaps_lsn_mc_settings *settings = rptTrieNode_p->m_trie_p->m_lsn_settings_p;
        uint32_t granularity;
        uint32_t ads2_num;

        kaps_sassert(db->num_algo_levels_in_db == 3);

        /*
         * Smallest granularity in the LSN is 8 bits. If we remove 1 bit for MPE, we can store 7 bits in LSN
         */
        max_trigger_len_1 = fibTbl->m_width - 7;

        /*
         * Add one for MPE
         */
        max_trigger_len_1 += 1;

        if (poolMgr->m_force_trigger_width)
            max_trigger_len_1 = poolMgr->m_cur_max_trigger_length_1;

        ads2_num = 0;
        if (!db->is_type_a)
        {
            ads2_num = 1;
        }

        if (device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots < KAPS_ADS2_VIRTUAL_IT_SIZE_PER_AB)
        {
            *o_reason = NLMRSC_IT_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        granularity = settings->m_middleLevelLengthToGran[max_trigger_len_1];

        errNum = kaps_pool_mgr_assign_pool_to_rpt_entry(poolMgr, rptTrieNode_p, numIptEntriesRequired,
                                                        numAptEntriesRequired, numPools, granularity,
                                                        &was_dba_dynamic_alloc, o_reason);

    }

    return errNum;
}

NlmErrNum_t
kaps_trie_commit_rpt_and_rit(
    kaps_trie_node * rptNode_p,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_trie_global *trieGlobal_p = rptNode_p->m_trie_p->m_trie_global;
    kaps_rpm *rpm_p = trieGlobal_p->m_rpm_p;
    uint8_t devNum = 0;
    struct kaps_pct ritEntry[MAX_COPIES_PER_LPM_DB];
    kaps_fib_tbl_mgr *fibTblMgr_p = trieGlobal_p->fibtblmgr;
    kaps_fib_tbl *fibTbl = rptNode_p->m_trie_p->m_tbl_ptr;
    uint8_t dbId = fibTbl->m_tblId;
    uint8_t is_extra_byte_added = fibTbl->m_db->common_info->is_extra_byte_added;

    if (fibTblMgr_p->m_devMgr_p->issu_status == KAPS_ISSU_INIT)
    {
        rptNode_p->m_rpt_prefix_p =
            kaps_pfx_bundle_create_from_string(trieGlobal_p->m_alloc_p, rptNode_p->m_lp_prefix_p->m_data,
                                               rptNode_p->m_depth, KAPS_LSN_NEW_INDEX, sizeof(void *), 0);

        kaps_memcpy(KAPS_PFX_BUNDLE_GET_ASSOC_PTR(rptNode_p->m_rpt_prefix_p), &rptNode_p, sizeof(void *));

        errNum = kaps_rpm_add_entry(rpm_p, rptNode_p->m_rpt_prefix_p, dbId, is_extra_byte_added, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    kaps_rpm_construct_rit(rptNode_p, ritEntry);

    errNum =
        kaps_fib_rit_write(fibTblMgr_p, devNum, ritEntry, (uint16_t) rptNode_p->m_rpt_prefix_p->m_nIndex, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_rpm_write_entry_to_hw(rpm_p, rptNode_p->m_rpt_prefix_p, dbId, is_extra_byte_added);

    return errNum;
}

NlmErrNum_t
kaps_trie_copy_ipt_and_apt_to_new_pool(
    kaps_trie_node * node_p,
    NlmReasonCode * o_reason)
{
    kaps_ipm *ipm_p = NULL;
    uint16_t newNumRptBitsLoppedOff = 0;
    kaps_lpm_trie *trie_p = NULL;
    kaps_trie_global *trie_global = NULL;
    uint32_t *indexBuffer_p = NULL;
    kaps_pool_mgr *poolMgr = NULL;
    NlmErrNum_t errNum;
    kaps_lsn_mc_settings *settings;
    uint32_t oldIndex;
    uint8_t hb_value = 0;
    uint32_t actualNewPoolId;
    uint32_t shouldAddEntryToIPM;

    if (node_p)
    {
        trie_p = node_p->m_trie_p;
        settings = trie_p->m_lsn_settings_p;
        poolMgr = trie_p->m_trie_global->poolMgr;

        if (node_p->m_isRptNode)
        {
            /*
             * We are trying to process a different RPT node that is below the current RPT node being processed. So
             * return 
             */
            return NLMERR_OK;
        }

        node_p->m_rptParent_p = trie_p->m_trie_global->m_newRptNode_p;
        node_p->m_poolId = trie_p->m_trie_global->m_newPoolId;
        node_p->m_rptId = trie_p->m_trie_global->m_newRptId;
        trie_global = trie_p->m_trie_global;
        indexBuffer_p = trie_global->m_indexBuffer_p;

        newNumRptBitsLoppedOff =
            kaps_trie_get_num_rpt_bits_lopped_off(trie_global->fibtblmgr->m_devMgr_p, trie_global->m_newRptDepth);

        if (node_p->m_node_type == NLMNSTRIENODE_LP)
        {
            /*
             * There may be a case where we reach a node at depth 11. This node will have node type as
             * NLMNSTRIENODE_LP, but is not a true IPT entry. To identify these kinds of spurious IPT entries, we will
             * look at the index of the prefix bundle and if it is an invalid index, then we will ignore it 
             */
            if (node_p->m_lp_prefix_p->m_nIndex != KAPS_LSN_NEW_INDEX)
            {
                node_p->m_poolId = trie_global->m_newPoolId;

                oldIndex = node_p->m_lp_prefix_p->m_nIndex;
                indexBuffer_p[trie_global->m_indexBufferIter] = oldIndex;

                trie_global->m_indexBufferIter++;

                actualNewPoolId = trie_global->m_newPoolId;
                shouldAddEntryToIPM = 1;

                /*
                 * Add the entry to the IPT here 
                 */
                ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, actualNewPoolId);

                if (settings->m_areIPTHitBitsPresent && node_p->m_iitLmpsofarPfx_p
                    && node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
                {
                    kaps_ipm *oldIpm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, trie_global->m_oldPoolId);
                    struct kaps_ab_info *old_ab_info = oldIpm_p->m_ab_info;

                    errNum =
                        kaps_kaps_read_trigger_hb_in_small_bb(ipm_p->m_fib_tbl_mgr, old_ab_info, oldIndex,
                                                         &hb_value, o_reason);

                    if (errNum != NLMERR_OK)
                        return errNum;
                }

                if (shouldAddEntryToIPM)
                {

                    errNum = kaps_ipm_add_entry(ipm_p, node_p->m_lp_prefix_p, trie_global->m_newRptId,
                                                node_p->m_depth - newNumRptBitsLoppedOff, newNumRptBitsLoppedOff,
                                                o_reason);

                    if (errNum != NLMERR_OK)
                        return errNum;

                    /*
                     * No potential coherency problem in KAPS as the IPT stage is in the SRAM. So no need to delete
                     * before writing
                     */
                    if (trie_global->fibtblmgr->m_devMgr_p->type != KAPS_DEVICE_KAPS)
                    {
                        kaps_ipm_delete_entry_in_hw(ipm_p, node_p->m_lp_prefix_p->m_nIndex, trie_global->m_newRptId,
                                                    node_p->m_depth - newNumRptBitsLoppedOff, o_reason);
                    }

                    NLM_STRY(kaps_lsn_mc_commitIIT(node_p->m_lsn_p, node_p->m_lp_prefix_p->m_nIndex, o_reason));

                    kaps_ipm_write_entry_to_hw(ipm_p, node_p->m_lp_prefix_p, trie_global->m_newRptId,
                                               node_p->m_depth - newNumRptBitsLoppedOff, newNumRptBitsLoppedOff,
                                               o_reason);
                }

                if (settings->m_areIPTHitBitsPresent && node_p->m_iitLmpsofarPfx_p
                    && node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
                {
                    errNum = kaps_kaps_write_trigger_hb_in_small_bb(ipm_p->m_fib_tbl_mgr, ipm_p->m_ab_info,
                                                               node_p->m_lp_prefix_p->m_nIndex, hb_value, o_reason);

                    if (errNum != NLMERR_OK)
                        return errNum;
                }
            }
        }

        errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(node_p->m_child_p[0], o_reason);
        if (errNum != NLMERR_OK)
            return errNum;

        errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(node_p->m_child_p[1], o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_delete_ipt_and_apt_in_old_pool(
    kaps_trie_node * node_p,
    NlmReasonCode * o_reason)
{
    kaps_ipm *ipm_p = NULL;
    uint16_t oldNumRptBitsLoppedOff = 0;
    kaps_lpm_trie *trie_p = NULL;
    kaps_trie_global *trie_global = NULL;
    uint32_t *indexBuffer_p = NULL;
    kaps_pool_mgr *poolMgr = NULL;
    struct kaps_device *device;
    uint32_t actualPoolId;
    uint32_t shouldDeleteEntryInIPM;
    struct kaps_db *db;

    if (node_p)
    {
        trie_p = node_p->m_trie_p;
        poolMgr = trie_p->m_trie_global->poolMgr;
        db = trie_p->m_tbl_ptr->m_db;

        if (node_p->m_isRptNode)
        {
            /*
             * We are trying to process a different RPT node that is below the current RPT node being processed. So
             * return 
             */
            return NLMERR_OK;
        }

        node_p->m_rptParent_p = trie_p->m_trie_global->m_newRptNode_p;
        node_p->m_poolId = trie_p->m_trie_global->m_newPoolId;
        trie_global = trie_p->m_trie_global;
        device = trie_p->m_tbl_ptr->m_db->device;
        indexBuffer_p = trie_global->m_indexBuffer_p;

        oldNumRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, trie_global->m_oldRptDepth);

        if (node_p->m_node_type == NLMNSTRIENODE_LP)
        {
            /*
             * There may be a case where we reach a node at depth 11. This node will have node type as
             * NLMNSTRIENODE_LP, but is not a true IPT entry. To identify these kinds of spurious IPT entries, we will
             * look at the index of the prefix bundle and if it is an invalid index, then we will ignore it 
             */
            if (node_p->m_lp_prefix_p->m_nIndex != KAPS_LSN_NEW_INDEX)
            {
                /*
                 * We have to figure out the correct poolId depending on whether the old trigger was in Reserved-160 or 
                 * not AB
                 */
                actualPoolId = trie_global->m_oldPoolId;

                shouldDeleteEntryInIPM = 1;

                /*
                 * Delete IPT entry in the old IPT Manager 
                 */
                ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, actualPoolId);

                /*
                 * In extremely rare case, all the IPT entries under the old RPT entry can move to the new RPT entry in 
                 * this case, the old IPT pool may no longer be present. So check if ipm_p is valid before performing
                 * the operations
                 */
                if (ipm_p && shouldDeleteEntryInIPM)
                {
                    kaps_ipm_delete_entry_in_hw(ipm_p, indexBuffer_p[trie_global->m_indexBufferIter],
                                                trie_global->m_oldRptId, node_p->m_depth - oldNumRptBitsLoppedOff,
                                                o_reason);

                    kaps_ipm_remove_entry(ipm_p, indexBuffer_p[trie_global->m_indexBufferIter],
                                          trie_global->m_oldRptId, node_p->m_depth - oldNumRptBitsLoppedOff, o_reason);

                    if (db->num_algo_levels_in_db == 3)
                    {
                        uint32_t ads2_nr;
                        uint32_t virtual_it_index, real_it_index;

                        ads2_nr = 1;
                        if (db->is_type_a)
                            ads2_nr = 0;

                        virtual_it_index = ipm_p->m_ab_info->base_addr + indexBuffer_p[trie_global->m_indexBufferIter];
                        real_it_index = device->kaps_shadow->ads2_overlay[ads2_nr].x_table[virtual_it_index];
                        kaps_set_bit(&device->kaps_shadow->ads2_overlay[ads2_nr].it_fbmp, real_it_index);
                        device->kaps_shadow->ads2_overlay[ads2_nr].num_free_it_slots++;
                        device->kaps_shadow->ads2_overlay[ads2_nr].x_table[virtual_it_index] = -1;
                    }
                }

                trie_global->m_indexBufferIter++;
            }
        }

        kaps_trie_delete_ipt_and_apt_in_old_pool(node_p->m_child_p[0], o_reason);
        kaps_trie_delete_ipt_and_apt_in_old_pool(node_p->m_child_p[1], o_reason);
    }

    return NLMERR_OK;
}

void
kaps_trie_propagate_rpt_attributes(
    kaps_trie_node * curNode_p,
    kaps_trie_node * rptNode_p)
{
    if (!curNode_p)
        return;

    if (curNode_p->m_isRptNode)
        return;

    curNode_p->m_rptParent_p = rptNode_p;
    if (rptNode_p)
    {
        curNode_p->m_poolId = rptNode_p->m_poolId;
        curNode_p->m_rptId = rptNode_p->m_rptId;
    }

    kaps_trie_propagate_rpt_attributes(curNode_p->m_child_p[0], rptNode_p);
    kaps_trie_propagate_rpt_attributes(curNode_p->m_child_p[1], rptNode_p);
}

void
kaps_trie_pvt_calc_num_ipt_and_apt(
    kaps_trie_node * node_p,
    uint32_t * longest_normal_trig_len)
{
    uint32_t numIptInLeftSubtrie = 0;
    uint32_t numAptInLeftSubtrie = 0;
    uint32_t numIptInRightSubtrie = 0;
    uint32_t numAptInRightSubtrie = 0;
    uint32_t numIptEntriesInCurNode = 0;
    uint32_t numAptEntriesInCurNode = 0;
    uint32_t num160bIptInLeftSubtrie = 0;
    uint32_t num160bIptInRightSubtrie = 0;
    uint32_t num160bIptInCurNode = 0;

    if (node_p)
    {

        if (node_p->m_child_p[0] && !node_p->m_child_p[0]->m_isRptNode)
        {
            kaps_trie_pvt_calc_num_ipt_and_apt(node_p->m_child_p[0], longest_normal_trig_len);
            numIptInLeftSubtrie = node_p->m_child_p[0]->m_numIptEntriesInSubtrie;
            num160bIptInLeftSubtrie = node_p->m_child_p[0]->m_numReserved160bTrig;
            numAptInLeftSubtrie = node_p->m_child_p[0]->m_numAptEntriesInSubtrie;
        }

        if (node_p->m_child_p[1] && !node_p->m_child_p[1]->m_isRptNode)
        {
            kaps_trie_pvt_calc_num_ipt_and_apt(node_p->m_child_p[1], longest_normal_trig_len);
            numIptInRightSubtrie = node_p->m_child_p[1]->m_numIptEntriesInSubtrie;
            num160bIptInRightSubtrie = node_p->m_child_p[1]->m_numReserved160bTrig;
            numAptInRightSubtrie = node_p->m_child_p[1]->m_numAptEntriesInSubtrie;
        }

        if (node_p->m_node_type == NLMNSTRIENODE_LP && node_p->m_lp_prefix_p->m_nIndex != KAPS_LSN_NEW_INDEX)
        {
            numIptEntriesInCurNode = 1;
        }

        if (node_p->m_aptLmpsofarPfx_p)
            numAptEntriesInCurNode++;

        if (node_p->m_rptAptLmpsofarPfx)
            numAptEntriesInCurNode++;

        node_p->m_numIptEntriesInSubtrie = numIptInLeftSubtrie + numIptInRightSubtrie + numIptEntriesInCurNode;

        node_p->m_numReserved160bTrig = num160bIptInLeftSubtrie + num160bIptInRightSubtrie + num160bIptInCurNode;

        node_p->m_numAptEntriesInSubtrie = numAptInLeftSubtrie + numAptInRightSubtrie + numAptEntriesInCurNode;
    }
}

void
kaps_trie_pvt_calc_longest_size_for_ipt_and_apt(
    kaps_trie_node * node_p,
    kaps_trie_node * rpt_node_p,
    uint32_t * longest_apt_entry_depth,
    uint32_t * longest_ipt_entry_depth)
{
    if (node_p)
    {
        if (node_p->m_isRptNode && node_p != rpt_node_p)
            return;

        if (node_p->m_aptLmpsofarPfx_p)
        {
            if ((*longest_apt_entry_depth) < node_p->m_aptLmpsofarPfx_p->m_nPfxSize)
                *longest_apt_entry_depth = node_p->m_aptLmpsofarPfx_p->m_nPfxSize;
        }

        if (node_p->m_rptAptLmpsofarPfx)
        {
            if ((*longest_apt_entry_depth) < node_p->m_rptAptLmpsofarPfx->m_nPfxSize)
                *longest_apt_entry_depth = node_p->m_rptAptLmpsofarPfx->m_nPfxSize;
        }

        if (node_p->m_node_type == NLMNSTRIENODE_LP && (*longest_ipt_entry_depth) < node_p->m_depth)
            *longest_ipt_entry_depth = node_p->m_depth;

        if (node_p->m_node_type == NLMNSTRIENODE_LP)
        {
            struct kaps_device *device = node_p->m_trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
            struct kaps_db *db = node_p->m_trie_p->m_tbl_ptr->m_db;

            if (db->num_algo_levels_in_db == 3)
            {
                uint32_t loc = node_p->m_trie_p->m_iptBrickScratchpad.m_numPfx;
                uint32_t numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, rpt_node_p->m_depth);

                /*
                 * Store the trigger length into the IPT brick scratchpad
                 */
                node_p->m_trie_p->m_iptBrickScratchpad.m_prefixLens[loc] = node_p->m_depth - numRptBitsLoppedOff;
                node_p->m_trie_p->m_iptBrickScratchpad.m_numPfx++;
            }
        }

        if (node_p->m_child_p[0])
        {
            kaps_trie_pvt_calc_longest_size_for_ipt_and_apt(node_p->m_child_p[0], rpt_node_p, longest_apt_entry_depth,
                                                            longest_ipt_entry_depth);
        }

        if (node_p->m_child_p[1])
        {
            kaps_trie_pvt_calc_longest_size_for_ipt_and_apt(node_p->m_child_p[1], rpt_node_p, longest_apt_entry_depth,
                                                            longest_ipt_entry_depth);
        }
    }
}

NlmErrNum_t
kaps_trie_clear_holes_in_split_ipt_ancestor(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * ancestorIptNode,
    kaps_trie_node * newRptParent_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc *ancestorIptLsn_p = NULL;
    uint32_t i = 0, j = 0, curIndexInLsn = 0;
    uint32_t newix = 0;
    kaps_lpm_lpu_brick *curLpuBrick = NULL;
    kaps_trie_node *trieNodeForMerge;

    if (numPfxToMove == 0)
        return NLMERR_OK;

    ancestorIptLsn_p = ancestorIptNode->m_lsn_p;

    kaps_assert((ancestorIptLsn_p != NULL), "Invalid IPT LSN pointer.\n");
    curLpuBrick = ancestorIptLsn_p->m_lpuList;

    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            if (curIndexInLsn == pfxLocationsToMove_p[j])
            {
                newix = kaps_lsn_mc_get_index(ancestorIptLsn_p, pfxLocationsToMove_p[j]);

                NLM_STRY(kaps_lsn_mc_delete_entry_in_hw(ancestorIptLsn_p, newix, o_reason));

                curLpuBrick->m_pfxes[i] = 0;
                curLpuBrick->m_numPfx--;
                ++j;
                if (j >= numPfxToMove)
                {
                    break;
                }
            }
            ++curIndexInLsn;
        }
        if (j >= numPfxToMove)
        {
            break;
        }
        curLpuBrick = curLpuBrick->m_next_p;
    }
    ancestorIptLsn_p->m_nNumPrefixes = ancestorIptLsn_p->m_nNumPrefixes - numPfxToMove;

    if (newRptParent_p)
        newRptParent_p->m_isRptNode = 1;

    if (ancestorIptLsn_p->m_nNumPrefixes == 0)
    {
        kaps_trie_pvt_in_place_delete(trie_p, ancestorIptNode, 1, o_reason);

        for (i = 0; i < trie_p->m_trie_global->m_numTrieNodesForMerge; ++i)
        {
            trieNodeForMerge = trie_p->m_trie_global->m_trieNodesForRelatedMerge[i];

            if (trieNodeForMerge == ancestorIptNode)
            {
                trie_p->m_trie_global->m_trieNodesForRelatedMerge[i] = NULL;
            }
        }

    }
    else
    {
        if (trie_p->m_lsn_settings_p->m_isShrinkEnabled)
        {
            if (trie_p->m_lsn_settings_p->m_isPerLpuGran)
            {
                kaps_lsn_mc_shrink_per_lpu_gran(ancestorIptLsn_p, o_reason);
            }
            else
            {
                kaps_lsn_mc_shrink_per_lsn_gran(ancestorIptLsn_p, o_reason);
            }
        }
    }

    if (newRptParent_p)
        newRptParent_p->m_isRptNode = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_check_split_nearest_ipt_ancestor(
    kaps_trie_node * newRptNode_p,
    kaps_lsn_mc ** ancestorIptLsn_pp,
    uint32_t pfxLocationsToMove_p[],
    uint16_t * numPfxToMove_p,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *iterTrieNode_p = newRptNode_p;
    kaps_lsn_mc *ancestorIptLsn_p = NULL;
    uint32_t byteOffset = 0;
    uint32_t commonLength = 0;
    uint32_t i = 0;
    uint16_t numPfxToMove = 0;
    kaps_pfx_bundle *pfxBundle_p = NULL;
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t indexInLsn;

    (void) o_reason;
    *numPfxToMove_p = 0;

    /*
     * If the RPT node is also already an IPT entry, then return with success 
     */
    if (newRptNode_p->m_node_type == NLMNSTRIENODE_LP)
    {
        return NLMERR_OK;
    }

    /*
     * Find the nearest IPT ancestor trie node for the new RPT entry 
     */
    while (iterTrieNode_p != NULL && iterTrieNode_p->m_node_type != NLMNSTRIENODE_LP)
    {
        iterTrieNode_p = iterTrieNode_p->m_parent_p;
    }

    if (!iterTrieNode_p || iterTrieNode_p->m_node_type != NLMNSTRIENODE_LP)
    {
        return NLMERR_FAIL;
    }

    ancestorIptLsn_p = iterTrieNode_p->m_lsn_p;
    byteOffset = ancestorIptLsn_p->m_nDepth / 8;
    commonLength = byteOffset * 8;

    /*
     * Iterate through the LSN of the nearest IPT ancestor and move the prefix from the IPT ancestor to the new Rpt
     * node if the prefix shares the first N bits with the new Rpt node where N is the depth of the RPT node 
     */
    curLpuBrick = ancestorIptLsn_p->m_lpuList;
    indexInLsn = 0;
    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            pfxBundle_p = curLpuBrick->m_pfxes[i];

            if (pfxBundle_p)
            {
                uint32_t doMove = kaps_prefix_pvt_is_more_specific_equal(&pfxBundle_p->m_data[byteOffset],
                                                                         pfxBundle_p->m_nPfxSize - commonLength,
                                                                         &newRptNode_p->
                                                                         m_lp_prefix_p->m_data[byteOffset],
                                                                         newRptNode_p->m_lp_prefix_p->m_nPfxSize -
                                                                         commonLength);
                if (doMove)
                {
                    pfxLocationsToMove_p[numPfxToMove] = indexInLsn;
                    numPfxToMove++;
                }
            }
            ++indexInLsn;
        }
        curLpuBrick = curLpuBrick->m_next_p;
    }

    *numPfxToMove_p = numPfxToMove;
    *ancestorIptLsn_pp = ancestorIptLsn_p;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_compute_ipt_ancestor_pfx_sort_data(
    kaps_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    uint16_t numPfxForEachLength[],
    uint16_t whereForEachLength[])
{
    kaps_lpm_lpu_brick *ancestorLpuBrick;
    uint16_t i, j, indexInLsn;
    kaps_pfx_bundle *pfxBundle_p;
    uint16_t curLoc;
    uint32_t length;

    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        numPfxForEachLength[i] = 0;
    }

    ancestorLpuBrick = ancestorIptLsn_p->m_lpuList;
    j = 0;
    indexInLsn = 0;
    while (ancestorLpuBrick)
    {
        for (i = 0; i < ancestorLpuBrick->m_maxCapacity; i++)
        {
            if (indexInLsn == pfxLocationsToMove_p[j])
            {
                pfxBundle_p = ancestorLpuBrick->m_pfxes[i];
                length = pfxBundle_p->m_nPfxSize - ancestorIptLsn_p->m_nDepth;
                numPfxForEachLength[length]++;
                ++j;
                if (j >= numPfxToMove)
                {
                    break;
                }
            }
            ++indexInLsn;
        }

        if (j >= numPfxToMove)
        {
            break;
        }
        ancestorLpuBrick = ancestorLpuBrick->m_next_p;
    }

    curLoc = 0;
    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        whereForEachLength[i] = curLoc;
        curLoc += numPfxForEachLength[i];
    }

    return NLMERR_OK;
}

/*New node may be a new IPT node or a new RPT node*/
NlmErrNum_t
kaps_trie_ipt_ancestor_iit_lmpsofar(
    kaps_trie_node * newNode_p,
    kaps_lsn_mc * ancestorIptLsn_p)
{
    kaps_lpm_trie *trie_p = newNode_p->m_trie_p;
    kaps_lsn_mc *newLsn_p = newNode_p->m_lsn_p;
    uint32_t i;
    kaps_lpm_lpu_brick *ancestorLpuBrick;
    kaps_pfx_bundle *ancestorPfxBundle, *lmpsofarPfxBundle;
    kaps_trie_node *ancestorIptNode_p;
    void *tmp_ptr;

    if (!trie_p->m_trie_global->m_isIITLmpsofar)
        return NLMERR_OK;

    if (!ancestorIptLsn_p)
        return NLMERR_OK;

    tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(ancestorIptLsn_p->m_pParentHandle);
    kaps_memcpy(&ancestorIptNode_p, tmp_ptr, sizeof(kaps_trie_node *));
    lmpsofarPfxBundle = ancestorIptNode_p->m_iitLmpsofarPfx_p;

    ancestorLpuBrick = ancestorIptLsn_p->m_lpuList;
    while (ancestorLpuBrick)
    {
        for (i = 0; i < ancestorLpuBrick->m_maxCapacity; ++i)
        {
            ancestorPfxBundle = ancestorLpuBrick->m_pfxes[i];
            if (ancestorPfxBundle && ancestorPfxBundle->m_nPfxSize < newLsn_p->m_nDepth &&
                !ancestorPfxBundle->m_isPfxCopy)
            {
                if (kaps_prefix_pvt_is_more_specific_equal
                    (newNode_p->m_lp_prefix_p->m_data, newNode_p->m_lp_prefix_p->m_nPfxSize, ancestorPfxBundle->m_data,
                     ancestorPfxBundle->m_nPfxSize))
                {
                    if (!lmpsofarPfxBundle
                        || (ancestorPfxBundle->m_backPtr->meta_priority < lmpsofarPfxBundle->m_backPtr->meta_priority)
                        || (ancestorPfxBundle->m_backPtr->meta_priority == lmpsofarPfxBundle->m_backPtr->meta_priority
                            && ancestorPfxBundle->m_nPfxSize > lmpsofarPfxBundle->m_nPfxSize))
                    {
                        lmpsofarPfxBundle = ancestorPfxBundle;
                    }
                }
            }
        }

        ancestorLpuBrick = ancestorLpuBrick->m_next_p;
    }

    if (!lmpsofarPfxBundle)
    {
        return NLMERR_OK;
    }

    newNode_p->m_iitLmpsofarPfx_p = lmpsofarPfxBundle;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_ipt_anc_split_get_prefixes_for_new_lsn(
    kaps_trie_node * newRptNode_p,
    kaps_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc *newLsn_p = NULL;
    uint32_t i = 0, j = 0;
    kaps_pfx_bundle *pfxBundle_p = NULL;
    uint32_t indexInLsn = 0;
    kaps_lpm_lpu_brick *ancestorLpuBrick;
    kaps_nlm_allocator *alloc_p = ancestorIptLsn_p->m_pSettings->m_pAlloc;
    kaps_flat_lsn_data *flatLsnData_p;
    uint16_t numPfxForEachLength[KAPS_LPM_KEY_MAX_WIDTH_1 + 1], whereForEachLength[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
    uint16_t curLoc;
    uint32_t length;
    uint32_t doesLsnFit;
    NlmErrNum_t errNum;

    newLsn_p = newRptNode_p->m_lsn_p;

    if (!newLsn_p)
    {
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    flatLsnData_p = kaps_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
    if (!flatLsnData_p)
        return NLMERR_FAIL;

    flatLsnData_p->m_commonPfx =
        kaps_prefix_create(alloc_p, KAPS_LPM_KEY_MAX_WIDTH_1, ancestorIptLsn_p->m_nDepth,
                           ancestorIptLsn_p->m_pParentHandle->m_data);

    if (!flatLsnData_p->m_commonPfx)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (numPfxToMove)
    {
        kaps_trie_compute_ipt_ancestor_pfx_sort_data(ancestorIptLsn_p, pfxLocationsToMove_p, numPfxToMove,
                                                     numPfxForEachLength, whereForEachLength);

        ancestorLpuBrick = ancestorIptLsn_p->m_lpuList;
        j = 0;
        indexInLsn = 0;
        while (ancestorLpuBrick)
        {
            for (i = 0; i < ancestorLpuBrick->m_maxCapacity; i++)
            {
                if (indexInLsn == pfxLocationsToMove_p[j])
                {
                    pfxBundle_p = ancestorLpuBrick->m_pfxes[i];
                    length = pfxBundle_p->m_nPfxSize - ancestorIptLsn_p->m_nDepth;

                    curLoc = whereForEachLength[length];
                    flatLsnData_p->m_pfxesInLsn[curLoc] = pfxBundle_p;
                    whereForEachLength[length]++;

                    flatLsnData_p->m_numPfxInLsn++;
                    if (flatLsnData_p->m_maxPfxLenInBits < pfxBundle_p->m_nPfxSize)
                    {
                        flatLsnData_p->m_maxPfxLenInBits = pfxBundle_p->m_nPfxSize;
                    }

                    ++j;
                    if (j >= numPfxToMove)
                    {
                        break;
                    }
                }

                ++indexInLsn;
            }
            if (j >= numPfxToMove)
            {
                break;
            }
            ancestorLpuBrick = ancestorLpuBrick->m_next_p;
        }
    }

    kaps_lsn_mc_assign_flat_data_colors(newLsn_p->m_pSettings, flatLsnData_p);

    doesLsnFit = 0;
    errNum = kaps_lsn_mc_convert_flat_data_to_lsn(flatLsnData_p, newLsn_p, &doesLsnFit, o_reason);

    if (errNum != NLMERR_OK || !doesLsnFit)
    {
        kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, flatLsnData_p);
        kaps_lsn_mc_destroy(newLsn_p);

        newRptNode_p->m_lsn_p =
            kaps_lsn_mc_create(ancestorIptLsn_p->m_pSettings, newRptNode_p->m_trie_p, newRptNode_p->m_depth);

        return NLMERR_FAIL;
    }

    kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, flatLsnData_p);

    newLsn_p->m_nNumPrefixes = numPfxToMove;
    newLsn_p->m_bIsNewLsn = 1;
    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_split_nearest_ipt_ancestor(
    kaps_trie_node * newRptNode_p,
    kaps_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    NlmReasonCode * o_reason)
{
    kaps_lpm_trie *trie_p = newRptNode_p->m_trie_p;
    kaps_lsn_mc *newLsn_p = NULL;
    kaps_ipm *ipm_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t numRptBitsLoppedOff = 0;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    kaps_lpm_lpu_brick *cur_lpu_brick;
    uint32_t curBrickIter;
    kaps_lsn_mc_settings *settings = ancestorIptLsn_p->m_pSettings;

    newLsn_p = newRptNode_p->m_lsn_p;

    if (!newLsn_p)
        return NLMERR_FAIL;

    errNum = kaps_trie_ipt_anc_split_get_prefixes_for_new_lsn(newRptNode_p, ancestorIptLsn_p,
                                                              pfxLocationsToMove_p, numPfxToMove, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_lsn_mc_acquire_resources(newLsn_p, newLsn_p->m_nLsnCapacity, newLsn_p->m_numLpuBricks, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_lsn_mc_destroy(newLsn_p);
        newRptNode_p->m_lsn_p = kaps_trie_node_pvt_create_lsn(newRptNode_p);
        return errNum;
    }

    if (settings->m_isJoinedUdc && settings->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(newLsn_p, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    if (newLsn_p->m_pSettings->m_isPerLpuGran)
    {
        cur_lpu_brick = newLsn_p->m_lpuList;
        curBrickIter = 0;
        while (cur_lpu_brick)
        {
            errNum = kaps_lsn_mc_acquire_resources_per_lpu(newLsn_p, cur_lpu_brick, curBrickIter,
                                                           cur_lpu_brick->m_maxCapacity, cur_lpu_brick->ad_db,
                                                           o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_free_resources_per_lpu(newLsn_p);
                kaps_lsn_mc_destroy(newLsn_p);
                newRptNode_p->m_lsn_p = kaps_trie_node_pvt_create_lsn(newRptNode_p);
                return errNum;
            }
            curBrickIter++;
            cur_lpu_brick = cur_lpu_brick->m_next_p;
        }
        newLsn_p->m_nNumIxAlloced = newLsn_p->m_nLsnCapacity;
    }

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(newLsn_p, o_reason);

    if (trie_p->m_trie_global->m_isIITLmpsofar)
        kaps_trie_ipt_ancestor_iit_lmpsofar(newRptNode_p, ancestorIptLsn_p);

    NLM_STRY(kaps_lsn_mc_commit(newLsn_p, 0, o_reason));

    newRptNode_p->m_lsn_p = newLsn_p;
    newLsn_p->m_pParentHandle = newRptNode_p->m_lp_prefix_p;

    trie_p->m_trie_global->m_isNearestIptAncestorSplit = 1;

    newRptNode_p->m_node_type = NLMNSTRIENODE_LP;

    newRptNode_p->m_rptParent_p = newRptNode_p;

    ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, newRptNode_p->m_poolId);

    numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(poolMgr->fibTblMgr->m_devMgr_p, newRptNode_p->m_depth);

    errNum = kaps_ipm_add_entry(ipm_p, newRptNode_p->m_lp_prefix_p, newRptNode_p->m_rptId,
                                newRptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    kaps_ipm_delete_entry_in_hw(ipm_p, newRptNode_p->m_lp_prefix_p->m_nIndex, newRptNode_p->m_rptId,
                                newRptNode_p->m_depth - numRptBitsLoppedOff, o_reason);

    errNum = kaps_lsn_mc_commitIIT(newRptNode_p->m_lsn_p, newRptNode_p->m_lp_prefix_p->m_nIndex, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_ipm_write_entry_to_hw(ipm_p, newRptNode_p->m_lp_prefix_p, newRptNode_p->m_rptId,
                                        newRptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason);

    newLsn_p->m_bIsNewLsn = 0;

    return errNum;
}

NlmErrNum_t
kaps_trie_configure_uda_mgr(
    kaps_trie_global * global,
    struct kaps_db *db,
    struct kaps_uda_mgr * uda_mem_mgr,
    NlmReasonCode * o_reason)
{
    struct kaps_device *device = global->fibtblmgr->m_devMgr_p;
    kaps_status status;
    struct uda_config config;
    uint32_t i, bb_nr;

    if (db->num_algo_levels_in_db != 3)
        return NLMERR_OK;

    kaps_memset(&config, 0, sizeof(config));

    for (i = 0; i < device->hw_res->total_num_bb / 2; ++i)
    {
        bb_nr = 2 * i;

        if (device->map->bb[bb_nr].row->owner &&
            (device->map->bb[bb_nr].row->owner == device->map->bb[bb_nr + 1].row->owner))
        {
            config.joined_udcs[bb_nr + 1] = 1;
        }
    }

    status = kaps_uda_mgr_configure(uda_mem_mgr, &config);

    if (status != KAPS_OK)
    {
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

kaps_trie_global *
kaps_trie_three_level_fib_module_init(
    kaps_nlm_allocator * alloc,
    kaps_fib_tbl_mgr * fibTblMgr,
    uint32_t is_cascaded,
    NlmReasonCode * o_reason)
{
    kaps_trie_global *global;
    uint32_t devId = 0;
    struct kaps_device *device = NULL;
    struct kaps_lpm_mgr *lpm_mgr;
    NlmErrNum_t errNum;
    struct kaps_c_list *ab_list;
    uint32_t i;

    (void) o_reason;

    if (!alloc || !fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NULL;
    }

    device = fibTblMgr->m_devMgr_p;

    /*
     * Allocate memory for trie global data structure 
     */
    global = kaps_nlm_allocator_calloc(alloc, 1, sizeof(kaps_trie_global));
    if (!global)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    global->m_alloc_p = alloc;
    global->fibtblmgr = fibTblMgr;
    global->m_areAllPoolsHealthy = 1;
    global->m_isAptPresent = 1;
    global->m_isLsnLmpsofar = 0;

    if (device->hw_res->no_overflow_lpm)
    {
        global->m_isAptPresent = 0;
        global->m_isLsnLmpsofar = 1;
    }

    global->m_maxIptEntriesForRptMove = 100;
    global->m_maxAptEntriesForRptMove = 100;

    /*
     * For palladium, don't move an RPT entry if the pool is close to full. Instead directly split an RPT entry in the
     * pool 
     */
    if (device->num_80b_entries_ab < global->m_maxIptEntriesForRptMove)
        global->m_maxIptEntriesForRptMove = 0;

    if (device->num_80b_entries_ab < global->m_maxAptEntriesForRptMove)
        global->m_maxAptEntriesForRptMove = 0;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        global->m_maxIptEntriesForRptMove = 0;
        global->m_maxAptEntriesForRptMove = 0;
        global->m_isAptPresent = 0;
        global->m_isIITLmpsofar = 1;
    }

    global->m_iptEntriesMoveDelta = 0;

    lpm_mgr = fibTblMgr->m_lpm_mgr;
        
    if (lpm_mgr)
    {
        struct kaps_db_hw_resource *db_hw_res = lpm_mgr->resource_db->hw_res.db_res;
        for (devId = 0; devId < fibTblMgr->m_numOfDevices; devId++)
        {
            kaps_status status;
            int32_t combined_lsn_size;

            if (lpm_mgr->resource_db->common_uda_owner_db)
                global->is_common_uda_mgr_used[devId] = 1;

            combined_lsn_size = db_hw_res->lsn_info[0].max_lsn_size;
            if (combined_lsn_size < db_hw_res->lsn_info[0].combined_lsn_size)
                combined_lsn_size = db_hw_res->lsn_info[0].combined_lsn_size;

            status = kaps_uda_mgr_init((is_cascaded) ? device->other_core : device,
                                       lpm_mgr->resource_db, 0, device->alloc,
                                       db_hw_res->lsn_info[0].alloc_udm, &global->m_mlpmemmgr[devId],
                                       combined_lsn_size,
                                       0,
                                       0,
                                       UDM_BOTH_HALF,
                                       kaps_fib_uda_sub_block_copy_callback,
                                       kaps_fib_update_it_callback,
                                       kaps_fib_kaps_update_lsn_size,
                                       0);

            if (status != KAPS_OK)
            {
                kaps_trie_convert_kbp_status_to_err_num(status, o_reason);
                kaps_trie_module_destroy(global);
                return NULL;
            }

            errNum = kaps_trie_configure_uda_mgr(global, lpm_mgr->resource_db, 
                            global->m_mlpmemmgr[0], o_reason);
            
            if (errNum != NLMERR_OK)
            {
                kaps_trie_module_destroy(global);
                return NULL;
            }
        }

        if (lpm_mgr->resource_db->common_info->is_cascaded && is_cascaded == 1)
            ab_list = &lpm_mgr->resource_db->hw_res.db_res->cas_ab_list;
        else
            ab_list = &lpm_mgr->resource_db->hw_res.db_res->ab_list;

        global->poolMgr = kaps_pool_mgr_create(fibTblMgr, global->m_alloc_p, is_cascaded, ab_list);

    }
    else
    {
        global->poolMgr = kaps_pool_mgr_create(fibTblMgr, global->m_alloc_p, is_cascaded, NULL);
    }

    if (!global->poolMgr)
    {
        kaps_trie_module_destroy(global);
        return NULL;
    }

    global->m_rpm_p = kaps_rpm_init(fibTblMgr, is_cascaded);
    if (!global->m_rpm_p)
    {
        kaps_trie_module_destroy(global);
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    global->m_indexBuffer_p = kaps_nlm_allocator_calloc(global->m_alloc_p, NLMNS_MAX_INDEX_BUFFER_SIZE,
                                                        sizeof(uint32_t));

    if (!global->m_indexBuffer_p)
    {
        kaps_trie_module_destroy(global);
        return NULL;
    }

    global->m_isReserved160bTrigBuffer = kaps_nlm_allocator_calloc(global->m_alloc_p, NLMNS_MAX_INDEX_BUFFER_SIZE,
                                                                   sizeof(uint8_t));

    if (!global->m_isReserved160bTrigBuffer)
    {
        kaps_trie_module_destroy(global);
        return NULL;
    }

    for (i = 0; i < KAPS_MAX_NUM_RPT_UUID; ++i)
    {
        global->m_rpt_uuid_table[i] = 1;
    }

    for (i = 0; i < 2; ++i)
    {
        global->poolIdsForReserved160bTrig[i] = KAPS_INVALID_POOL_INDEX;
    }

    global->m_max_reserved_160b_trig_per_rpt = 50;

    return global;
}

kaps_trie_global *
kaps_trie_two_level_fib_module_init(
    kaps_nlm_allocator * alloc,
    kaps_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason)
{
    kaps_trie_global *global;
    kaps_status status;
    struct kaps_lpm_mgr *lpm_mgr = NULL;
    struct kaps_db_hw_resource *db_hw_res = NULL;
    struct kaps_device *device = NULL;

    (void) o_reason;

    if (!alloc || !fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NULL;
    }
    lpm_mgr = fibTblMgr->m_lpm_mgr;
    device = fibTblMgr->m_devMgr_p;
    if (!lpm_mgr)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NULL;
    }

    db_hw_res = lpm_mgr->resource_db->hw_res.db_res;

    /*
     * Allocate memory for trie global data structure 
     */
    global = kaps_nlm_allocator_calloc(alloc, 1, sizeof(kaps_trie_global));
    if (!global)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    global->m_alloc_p = alloc;
    global->fibtblmgr = fibTblMgr;
    global->m_areAllPoolsHealthy = 1;
    global->m_isIITLmpsofar = 1;

    status = kaps_uda_mgr_init(device, lpm_mgr->resource_db, 0, device->alloc,
                               db_hw_res->lsn_info[0].alloc_udm, &global->m_mlpmemmgr[0],
                               db_hw_res->lsn_info[0].max_lsn_size,
                               0,
                               0,
                               UDM_BOTH_HALF,
                               kaps_fib_uda_sub_block_copy_callback,
                               kaps_fib_update_it_callback,
                               kaps_fib_kaps_update_lsn_size,
                               0);

    if (status != KAPS_OK)
    {
        kaps_trie_convert_kbp_status_to_err_num(status, o_reason);
        kaps_trie_module_destroy(global);
        return NULL;
    }


    if (kaps_db_is_reduced_algo_levels(lpm_mgr->resource_db))
    {
        global->poolMgr = kaps_pool_mgr_create(fibTblMgr, global->m_alloc_p, 0, &(lpm_mgr->resource_db->hw_res.db_res->rpb_ab_list));
    }
    else
    {
        global->poolMgr = kaps_pool_mgr_create(fibTblMgr, global->m_alloc_p, 0, kaps_resource_get_ab_list(lpm_mgr->resource_db));
    }
    
    if (!global->poolMgr)
    {
        kaps_trie_module_destroy(global);
        return NULL;
    }

    global->m_indexBuffer_p = kaps_nlm_allocator_calloc(global->m_alloc_p, NLMNS_MAX_INDEX_BUFFER_SIZE,
                                                        sizeof(uint32_t));

    if (!global->m_indexBuffer_p)
    {
        kaps_trie_module_destroy(global);
        return NULL;
    }

    return global;
}

void
kaps_trie_module_destroy(
    void *global)
{
    kaps_trie_global *self = (kaps_trie_global *) global;
    kaps_fib_tbl_mgr *fibTblMgr;
    kaps_nlm_allocator *alloc_p = NULL;
    uint32_t devId = 0;

    if (!self)
        return;

    fibTblMgr = self->fibtblmgr;
    for (devId = 0; devId < fibTblMgr->m_numOfDevices; devId++)
    {
        if (self->m_mlpmemmgr[devId])
        {
            if (!self->is_common_uda_mgr_used[devId])
            {
                kaps_uda_mgr_destroy(self->m_mlpmemmgr[devId]);
            }
            else
            {
                self->m_mlpmemmgr[devId] = NULL;
                self->is_common_uda_mgr_used[devId] = 0;
            }
        }
    }

    kaps_pool_mgr_destroy(self->poolMgr, self->m_alloc_p);

    if (self->m_indexBuffer_p)
        kaps_nlm_allocator_free(self->m_alloc_p, self->m_indexBuffer_p);

    if (self->m_isReserved160bTrigBuffer)
        kaps_nlm_allocator_free(self->m_alloc_p, self->m_isReserved160bTrigBuffer);

    alloc_p = self->m_alloc_p;

    kaps_nlm_allocator_free(alloc_p, self);
}

kaps_lpm_trie *
kaps_trie_create(
    void *trieGlobal,
    void *fibTbl_p,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc_p = NULL;
    kaps_lpm_trie *trie_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_assert(trieGlobal && fibTbl_p, "Invalid Pointers passed");

    alloc_p = ((kaps_trie_global *) trieGlobal)->m_alloc_p;
    kaps_assert(alloc_p, "Invalid Alloc pointers in triesettings ");

    trie_p = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_lpm_trie));
    if (!trie_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    errNum = kaps_trie_ctor(trie_p, trieGlobal, fibTbl_p, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_nlm_allocator_free(alloc_p, trie_p);
        return NULL;
    }

    return trie_p;
}

NlmErrNum_t
kaps_trie_commit_iits(
    kaps_lpm_trie * trie,
    NlmReasonCode * o_reason)
{
    uint32_t i = 0, nPending = trie->m_num_items_in_to_update_sit;
    kaps_trie_node *lpmnode = NULL;

    for (i = 0; i < nPending; i++)
    {
        lpmnode = trie->m_toupdate_sit[i];
        NLM_STRY(kaps_trie_node_update_iit(lpmnode, o_reason));
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_trie_compute_num_joined_sets(
    struct kaps_device *device,
    struct kaps_db *db,
    uint32_t * numJoinedSets,
    uint32_t * numJoinedBricksInOneSet)
{
    uint32_t i;

    *numJoinedBricksInOneSet = 1;
    *numJoinedSets = 0;

    if (db->num_algo_levels_in_db == 3)
    {

        for (i = 0; i < KAPS_ALLOC_UDM_MAX_COLS; i += 2)
        {
            if (db->hw_res.db_res->lsn_info[0].alloc_udm[0][0][i] && db->hw_res.db_res->lsn_info[0].alloc_udm[0][0][i + 1])
            {
                (*numJoinedSets)++;
            }
        }

        *numJoinedBricksInOneSet = 2;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_trie_ctor(
    kaps_lpm_trie * trie_p,
    kaps_trie_global * trie_global,
    void *fibTbl_p,
    NlmReasonCode * o_reason)
{
    kaps_nlm_allocator *alloc_p = NULL;
    kaps_fib_tbl *fibtbl = (kaps_fib_tbl *) fibTbl_p;
    struct kaps_device *device = trie_global->fibtblmgr->m_devMgr_p;
    kaps_fib_tbl_mgr *fibTblMgr = trie_global->fibtblmgr;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_status status;
    uint32_t init_hashtbl_size = 0, i = 0;
    struct kaps_db *main_db;
    uint32_t key_width_1;
    uint32_t numJoinedSets, numJoinedBricksInOneSet;

    kaps_assert(trie_p && trie_global && fibTbl_p, "Invalid Value passed ");

    trie_p->m_tbl_ptr = fibTbl_p;
    trie_p->m_trie_global = trie_global;
    alloc_p = trie_global->m_alloc_p;

    
    main_db = fibtbl->m_db;
    if (main_db->parent)
        main_db = main_db->parent;

    if (fibtbl->m_db->hw_res.db_res->num_dba_dt == 0)
    {
        trie_global->m_isAptPresent = 0;
        trie_global->m_isLsnLmpsofar = 1;
    }

    numJoinedSets = 0;
    numJoinedBricksInOneSet = 1;

    if (device->type == KAPS_DEVICE_KAPS)
    {

        kaps_trie_compute_num_joined_sets(device, main_db, &numJoinedSets, &numJoinedBricksInOneSet);

        trie_p->m_lsn_settings_p = kaps_lsn_mc_settings_create(trie_global->m_alloc_p,
                                                               fibtbl,
                                                               fibTblMgr->m_devMgr_p,
                                                               &trie_global->m_mlpmemmgr[0],
                                                               fibTblMgr->m_indexChangeCallBackFn,
                                                               fibTblMgr->m_lpm_mgr,
                                                               main_db->hw_res.db_res->lsn_info[0].max_lsn_size,
                                                               numJoinedSets, numJoinedBricksInOneSet);

    }

    if (!trie_p->m_lsn_settings_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    /*
     * Allocate memory for stacks required by trie 
     */
    for (i = 0; i < KAPS_TRIE_NUMSTACK; i++)
    {
        trie_p->m_recurse_stack[i] =
            (kaps_trie_node **) kaps_nlm_allocator_malloc(trie_global->m_alloc_p, sizeof(kaps_trie_node *) * 320);
        trie_p->m_recurse_stack_inuse[i] = 0;
        trie_p->m_recurse_args_stack[i] = (NlmNsTrieNode__TraverseArgs *)
            kaps_nlm_allocator_malloc(trie_global->m_alloc_p, sizeof(NlmNsTrieNode__TraverseArgs) * 320);
        trie_p->m_recurse_args_stack_inuse[i] = 0;
    }

    trie_p->m_ancestors_space_inuse = 0;
    trie_p->m_ancestors_space =
        (kaps_trie_node **) kaps_nlm_allocator_malloc(trie_global->m_alloc_p, sizeof(kaps_trie_node *) * 320);

    trie_p->m_doverify = (uint8_t) 0;

    key_width_1 = fibtbl->m_db->key->width_1;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        struct kaps_key_field *f;
        uint32_t table_id_width_1 = 0;

        for (f = fibtbl->m_db->key->first_field; f; f = f->next)
        {
            if (f->type == KAPS_KEY_FIELD_TABLE_ID)
            {
                table_id_width_1 = f->width_1;
            }
        }

        trie_p->m_min_lopoff = 0;

        if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
        {
            if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_FM4
                || device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
            {
                /*
                 * For JR+ 4-format, the last 12 bits in the key are used to store 3 format fields from the IT. So if
                 * key width is > 148 bits, we have to ensure there is a minimum lopoff so that the last 12 bits will
                 * be free 
                 */
                if (key_width_1 > KAPS_JERICHO_PLUS_FM4_MAX_KEY_WIDTH_1)
                {
                    trie_p->m_min_lopoff = key_width_1 - KAPS_JERICHO_PLUS_FM4_MAX_KEY_WIDTH_1;
                }
            }
        }

        if (table_id_width_1 > trie_p->m_min_lopoff)
            trie_p->m_min_lopoff = table_id_width_1;
    }
    else
    {
        trie_p->m_min_lopoff = NLMNS_TRIE_DEFAULT_LOPOFF;
    }

    
    if (fibtbl->m_cascaded_fibtbl)
        trie_p->m_ixMgr = main_db->hw_res.db_res->cascaded_index_mgr;
    else
        trie_p->m_ixMgr = main_db->hw_res.db_res->index_mgr;

    /*
     * Create root node of the trie for the given table 
     */
    trie_p->m_roots_trienode_p = kaps_trie_node_create_root_node(trie_p, 0);

    /*
     * We are choosing expansion depth to be equal to m_min_lopoff. Suppose we make expansion depth ( = 8) >
     * m_min_lopoff (= 0), then we can have problems with iitLmpsofar calculation. So if the first entry added is /0,
     * it will go to the root. Let us say that next entry added is 4.0.0.0/32, which will create an LSN at depth 8. Now 
     * if we issue search for 4.1.1.1, we should get a hit from the /0 entry, but we get a miss. This is because the /0 
     * which is lmpsofar of LSN at depth 8 has not been propagated as lmpsofar of that LSN since /0 was added earlier.
     * We don't have this support in the code. 
     */
    trie_p->m_expansion_depth = trie_p->m_min_lopoff;

    /*
     * Expand the trie to ensure minimum lopoff of longer prefix 
     */
    kaps_trie_node_pvt_expand_trie(trie_p->m_roots_trienode_p, trie_p->m_expansion_depth);

    if (trie_p->m_roots_trienode_p)
        trie_p->m_roots_trienode_p->m_node_type = NLMNSTRIENODE_LP;

    trie_p->m_cache_trienode_p = NULL;

    /*
     * Initialize and create hash table for the given size 
     */
    init_hashtbl_size = fibtbl->m_tblIndexRange.m_indexHighValue - fibtbl->m_tblIndexRange.m_indexLowValue + 1;

    if (fibtbl->m_tblIndexRange.m_indexHighValue == 0 && fibtbl->m_tblIndexRange.m_indexLowValue == 0)
    {
        if (fibtbl->m_db->common_info->capacity)
            init_hashtbl_size = fibtbl->m_db->common_info->capacity;
        else
            init_hashtbl_size = 1 * 1024 * 1024;
    }

    if (fibtbl->m_db->common_info->ad_info.ad)
    {
        struct kaps_ad_db *ad_db = (struct kaps_ad_db *) fibtbl->m_db->common_info->ad_info.ad;
        uint32_t total_ad_capacity = 0;

        while (ad_db)
        {
            total_ad_capacity += ad_db->db_info.common_info->capacity;
            ad_db = ad_db->next;
        }

        if (total_ad_capacity < 1024 * 1024)
            total_ad_capacity = 1024 * 1024;

        if (fibtbl->m_db->common_info->capacity > total_ad_capacity)
            total_ad_capacity = fibtbl->m_db->common_info->capacity;

        init_hashtbl_size = total_ad_capacity;
    }

    if (init_hashtbl_size < 1024 * 1024)
        init_hashtbl_size = 1024 * 1024;

    if (!fibtbl->m_cascaded_fibtbl)
    {
        status = kaps_pfx_hash_table_create(device->alloc, init_hashtbl_size, 70, 100,
                                       fibtbl->m_width, 0, fibtbl->m_db, &trie_p->m_hashtable_p);
        if (status != KAPS_OK)
        {
            errNum = kaps_trie_convert_kbp_status_to_err_num(status, o_reason);
            return errNum;
        }
    }

    trie_p->m_isCompactionEnabled = 1;
    trie_p->m_isCompactionRequired = 0;
    trie_p->m_isOverAllocEnabled = 0;
    trie_p->m_numOfMCHoles = 16;

    if (fibtbl->m_db->num_algo_levels_in_db > 2)
    {
        kaps_rpm_init_db(trie_global->m_rpm_p, fibtbl->m_tblId, o_reason);
    }

    if (trie_p->m_lsn_settings_p->m_areIPTHitBitsPresent)
    {
        if (fibtbl->m_db->num_algo_levels_in_db == 2)
        {
            trie_p->m_iptHitBitBuffer =
                kaps_nlm_allocator_calloc(alloc_p, (device->num_80b_entries_ab / 2), sizeof(uint8_t));
        }
        else
        {
            trie_p->m_iptHitBitBuffer =
                kaps_nlm_allocator_calloc(alloc_p,
                                          (device->hw_res->total_small_bb / 2) * device->hw_res->num_rows_in_small_bb *
                                          KAPS_HB_ROW_WIDTH_1, sizeof(uint8_t));
        }
    }

    if (trie_p->m_lsn_settings_p->m_areRPTHitBitsPresent)
    {
        trie_p->m_rptTcamHitBitBuffer =
            kaps_nlm_allocator_calloc(alloc_p, (device->num_80b_entries_ab / 2), sizeof(uint8_t));
    }

    return errNum;
}

static void
kaps_trie_dtor(
    kaps_lpm_trie * self)
{
    kaps_nlm_allocator *alloc;
    uint32_t i = 0;

    if (!self)
        return;

    alloc = self->m_trie_global->m_alloc_p;

    kaps_trie_node_destroy(self->m_roots_trienode_p, alloc, 1);

    /*
     * Free stack space memory associated with given trie 
     */
    for (i = 0; i < KAPS_TRIE_NUMSTACK; i++)
    {
        kaps_nlm_allocator_free(self->m_trie_global->m_alloc_p, self->m_recurse_stack[i]);
        kaps_nlm_allocator_free(self->m_trie_global->m_alloc_p, self->m_recurse_args_stack[i]);
    }

    kaps_nlm_allocator_free(self->m_trie_global->m_alloc_p, self->m_ancestors_space);

    /*
     * Destroy hash table associated with given trie 
     */
    if (self->m_hashtable_p)
        kaps_pfx_hash_table_destroy(self->m_hashtable_p);

    if (self->m_iptHitBitBuffer)
    {
        kaps_nlm_allocator_free(alloc, self->m_iptHitBitBuffer);
        self->m_iptHitBitBuffer = NULL;
    }

    if (self->m_rptTcamHitBitBuffer)
    {
        kaps_nlm_allocator_free(alloc, self->m_rptTcamHitBitBuffer);
        self->m_rptTcamHitBitBuffer = NULL;
    }

    if (self->m_lsn_settings_p)
        kaps_lsn_mc_settings_destroy(self->m_lsn_settings_p);
}

void
kaps_trie_destroy(
    kaps_lpm_trie * self)
{
    if (self)
    {
        kaps_trie_dtor(self);
        kaps_nlm_allocator_free(self->m_trie_global->m_alloc_p, self);
    }
}

/* This function returns pointer to cache trienode if __FindDestLpNode for the
 * prefix can be started from the cache trienode itself. Else NULL.
 */
kaps_trie_node *
kaps_trie_pvt_is_cache_hit(
    kaps_trie_node * trienode_p,
    uint8_t * pfxData,
    uint32_t pfx_len_1)
{
    if (trienode_p && (trienode_p->m_node_type == NLMNSTRIENODE_LP) && (pfx_len_1 >= trienode_p->m_depth))
    {
        uint8_t *cachePrefix = trienode_p->m_lp_prefix_p->m_data;
        uint32_t bits = trienode_p->m_depth;
        uint32_t isMatching = 1;
        int32_t len;

        /*
         * Instead of using kaps_memcmp, we iterate backwards because it is actually faster than kaps_memcmp because
         * more often than not, the very first byte mismatches if we iterate backwards 
         */
        len = (bits >> 3) - 1;

        for (; len >= 0; len--)
        {
            if (pfxData[len] != cachePrefix[len])
            {
                isMatching = 0;
                break;
            }
        }

        if (isMatching == 1)
        {
            uint8_t mask = 0;
            if (bits % 8)
            {
                mask = (uint8_t) (0xFF << (8 - (bits % 8)));

                if ((pfxData[bits >> 3] & mask) == (cachePrefix[bits >> 3] & mask))
                {
                    return trienode_p;
                }
            }
            else
                return trienode_p;
        }
    }

    return NULL;
}

void
kaps_trie_log_rpt_node(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * curRptNode,
    FILE * fp)
{
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    kaps_pool_info *ipmPoolInfo;
    struct kaps_ab_info *ab;
    uint32_t poolIndex;
    uint32_t i, j, k, num_free_slots;
    struct kaps_db *db = poolMgr->fibTblMgr->m_curFibTbl->m_db;

    kaps_fprintf(fp, "Submit Rqt = %d, \n", g_submitRqtHitCnt);
    kaps_fprintf(fp, "RPT Node Id = %d, depth = %d, Num Triggers = %d\n",
                 curRptNode->m_trienodeId, curRptNode->m_depth, curRptNode->m_numIptEntriesInSubtrie);

    poolIndex = KAPS_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(curRptNode->m_poolId);

    kaps_fprintf(fp, "IPM Pool Id = %d\n", poolIndex);

    ipmPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];
    ab = ipmPoolInfo->m_dba_mgr->m_ab_info;

    kaps_fprintf(fp, "AB Num = %d", ab->ab_num);

    if (db->num_algo_levels_in_db == 3)
    {
        kaps_fprintf(fp, ", Num bricks = %d ", device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks);

        for (i = 0; i < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++i)
        {
            num_free_slots = 0;
            for (j = 0; j < KAPS_BMP_IN_SMALL_BB_8; ++j)
            {
                for (k = 0; k < 8; ++k)
                {
                    if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[i].free_slot_bmp[j] & (1u << k))
                    {
                        ++num_free_slots;
                    }
                }
            }

            kaps_fprintf(fp, "(Brick = %d, Gran = %d, Num free slots = %d), ", i,
                         device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[i].sub_ab_gran, num_free_slots);
        }
    }

    kaps_fprintf(fp, "\n");

    kaps_fprintf(fp, "Cur num DBA entries = %d, Max num DBA entries = %d\n",
                 ipmPoolInfo->m_curNumDbaEntries, ipmPoolInfo->m_maxNumDbaEntries);

    kaps_fprintf(fp, "\n");
}

void
kaps_trie_log_rpt_split(
    kaps_lpm_trie * trie_p)
{
    static uint32_t g_rptSplitCnt;
    FILE *fp;
    kaps_trie_global *trie_global = trie_p->m_trie_global;

    g_rptSplitCnt++;

    if (g_rptSplitCnt <= 1)
    {
        fp = kaps_fopen("RPT_Split.txt", "w");
    }
    else
    {
        fp = kaps_fopen("RPT_Split.txt", "a");
    }

    kaps_fprintf(fp, "RPT Split Nr = %d\n", g_rptSplitCnt);
    kaps_fprintf(fp, "Splitting RPT Node \n");

    kaps_trie_log_rpt_node(trie_p, trie_global->m_curRptNodeToSplit_p, fp);

    kaps_fprintf(fp, "________________________________________________\n");

    kaps_fclose(fp);
}

void
kaps_trie_log_rpt_move(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * curRptNodeToMove)
{
    static uint32_t g_rptMoveCnt;
    FILE *fp;

    g_rptMoveCnt++;

    if (g_rptMoveCnt <= 1)
    {
        fp = kaps_fopen("RPT_Split.txt", "w");
    }
    else
    {
        fp = kaps_fopen("RPT_Split.txt", "a");
    }

    kaps_fprintf(fp, "RPT Move Nr = %d\n", g_rptMoveCnt);
    kaps_fprintf(fp, "Moving RPT Node \n");

    kaps_trie_log_rpt_node(trie_p, curRptNodeToMove, fp);

    kaps_fprintf(fp, "________________________________________________\n");

    kaps_fclose(fp);
}

NlmErrNum_t
kaps_trie_move_rpt_to_new_pool(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * rptNodeToMove_p,
    uint32_t numExtraIptEntriesNeeded,
    uint32_t numExtraAptEntriesNeeded,
    uint32_t shouldBufferKapsWrites,
    NlmReasonCode * o_reason)
{
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    kaps_fib_tbl_mgr *fibTblMgr_p = trie_global->fibtblmgr;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    uint32_t oldPoolId, oldRptId;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t devNum = 0;
    struct kaps_pct ritEntry[MAX_COPIES_PER_LPM_DB];
    uint32_t numPools;
    uint32_t numRptBitsLoppedoff;

    numPools = 1;
    if (trie_p->m_trie_global->m_isAptPresent)
        numPools = 2;

    trie_global->m_numReserved160bThatBecameRegular = 0;

    oldPoolId = rptNodeToMove_p->m_poolId;
    oldRptId = rptNodeToMove_p->m_rptId;
    numRptBitsLoppedoff = kaps_trie_get_num_rpt_bits_lopped_off(device, rptNodeToMove_p->m_depth);

    {

        uint32_t longest_size = 0;
        uint32_t longest_apt_entry_depth = 0, longest_ipt_entry_depth = 0;

        if (!device->hw_res->no_dba_compression && !poolMgr->m_force_trigger_width)
        {

            kaps_trie_pvt_calc_longest_size_for_ipt_and_apt(rptNodeToMove_p, rptNodeToMove_p,
                                                            &longest_apt_entry_depth, &longest_ipt_entry_depth);

            if (longest_apt_entry_depth > longest_ipt_entry_depth)
                longest_size = longest_apt_entry_depth;
            else
                longest_size = longest_ipt_entry_depth;

            poolMgr->m_cur_max_trigger_length_1 = longest_size - numRptBitsLoppedoff;

            poolMgr->m_force_trigger_width = 1;
        }
    }

    errNum =
        kaps_trie_pvt_get_new_pool(poolMgr, rptNodeToMove_p,
                                   rptNodeToMove_p->m_numIptEntriesInSubtrie + numExtraIptEntriesNeeded,
                                   rptNodeToMove_p->m_numAptEntriesInSubtrie + numExtraAptEntriesNeeded, numPools,
                                   o_reason);
    if (poolMgr->m_force_trigger_width)
        poolMgr->m_force_trigger_width = 0;

    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    /*
     * Decrease the count of IPT entries in the pool manager in the old pool. 
     */
    kaps_pool_mgr_decr_num_entries(poolMgr, oldPoolId, rptNodeToMove_p->m_numIptEntriesInSubtrie, KAPS_IPT_POOL);

    /*
     * Increase the number of IPT entries in the new pool manager 
     */
    kaps_pool_mgr_incr_num_entries(poolMgr, rptNodeToMove_p->m_poolId,
                                   rptNodeToMove_p->m_numIptEntriesInSubtrie, KAPS_IPT_POOL);

    rptNodeToMove_p->m_isRptNode = 0;
    trie_global->m_oldPoolId = oldPoolId;
    trie_global->m_oldRptId = oldRptId;
    trie_global->m_old_rpt_uuid = rptNodeToMove_p->m_rpt_uuid;
    trie_global->m_oldRptDepth = rptNodeToMove_p->m_depth;

    trie_global->m_newPoolId = rptNodeToMove_p->m_poolId;
    trie_global->m_newRptId = rptNodeToMove_p->m_rptId;
    trie_global->m_new_rpt_uuid = rptNodeToMove_p->m_rpt_uuid;
    trie_global->m_newRptDepth = rptNodeToMove_p->m_depth;

    trie_global->m_newRptNode_p = rptNodeToMove_p;
    trie_global->m_indexBufferIter = 0;
    trie_global->m_deleteRptAptLmpsofar = 1;

    trie_global->m_rptOp = KAPS_RPT_MOVE;

    if (shouldBufferKapsWrites && device->type == KAPS_DEVICE_KAPS)
    {
        fibTblMgr_p->m_bufferKapsABWrites = 1;
    }

    trie_p->m_isCopyIptAndAptToNewPoolInProgress = 1;

    errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(rptNodeToMove_p, o_reason);

    trie_p->m_isCopyIptAndAptToNewPoolInProgress = 0;

    fibTblMgr_p->m_bufferKapsABWrites = 0;

    if (errNum != NLMERR_OK)
        return errNum;

    if (shouldBufferKapsWrites && device->type == KAPS_DEVICE_KAPS)
    {
        kaps_ipm *newIpm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, trie_global->m_newPoolId);
        kaps_kaps_flush_buffered_small_bb_writes(fibTblMgr_p, newIpm_p->m_ab_info, o_reason);
    }

    kaps_assert(trie_global->m_numReserved160bThatBecameRegular == 0,
                "We should have not moved any reserved 160b triggers\n");

    kaps_rpm_construct_rit(rptNodeToMove_p, ritEntry);

    errNum = kaps_fib_rit_write(fibTblMgr_p, devNum, ritEntry,
                                (uint16_t) rptNodeToMove_p->m_rpt_prefix_p->m_nIndex, o_reason);

    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    if (shouldBufferKapsWrites && device->type == KAPS_DEVICE_KAPS)
    {
        fibTblMgr_p->m_bufferKapsABWrites = 1;
    }

    trie_global->m_indexBufferIter = 0;
    kaps_trie_delete_ipt_and_apt_in_old_pool(rptNodeToMove_p, o_reason);

    fibTblMgr_p->m_bufferKapsABWrites = 0;

    trie_global->m_move_reserved_160b_trig_to_regular_pool = 0;

    if (shouldBufferKapsWrites && device->type == KAPS_DEVICE_KAPS)
    {
        kaps_ipm *oldIpm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, trie_global->m_oldPoolId);
        kaps_kaps_flush_buffered_small_bb_writes(fibTblMgr_p, oldIpm_p->m_ab_info, o_reason);
    }

    kaps_pool_mgr_release_pool_for_rpt_entry(poolMgr, rptNodeToMove_p, oldPoolId, oldRptId);

    rptNodeToMove_p->m_isRptNode = 1;
    rptNodeToMove_p->m_trie_p->m_tbl_ptr->m_fibStats.numRptMoves++;

    return NLMERR_OK;
}

kaps_trie_node *
kaps_trie_find_new_rpt_push_node(
    kaps_lpm_trie * trie_p)
{
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    kaps_trie_node *curTrieNode = trie_global->m_curRptNodeToPush_p;
    uint32_t leftSubTrieNumIpt, rightSubTrieNumIpt;
    uint32_t leftSubTrieNumApt, rightSubTrieNumApt;
    uint32_t isLeftSubTrieEmpty, isRightSubTrieEmpty;
    kaps_trie_node *newRptPushNode = NULL;

    /*
     * We need to find the maximum number of nodes that we can traverse downwards from the cur RPT Node to be pushed.
     * So we keep traversing down, picking either the left child or the right child until we find an IPT/APT node
     */

    while (curTrieNode)
    {
        newRptPushNode = curTrieNode;

        /*
         * If we found an IPT entry or an APT Lmpsofar entry, then we should not go deeper. We have found the new RPT
         * push node. Return from here
         */
        if (curTrieNode->m_node_type == 1 || curTrieNode->m_aptLmpsofarPfx_p)
            break;

        leftSubTrieNumIpt = 0;
        leftSubTrieNumApt = 0;
        if (curTrieNode->m_child_p[0])
        {
            leftSubTrieNumIpt = curTrieNode->m_child_p[0]->m_numIptEntriesInSubtrie;
            leftSubTrieNumApt = curTrieNode->m_child_p[0]->m_numAptEntriesInSubtrie;
        }

        rightSubTrieNumIpt = 0;
        rightSubTrieNumApt = 0;
        if (curTrieNode->m_child_p[1])
        {
            rightSubTrieNumIpt = curTrieNode->m_child_p[1]->m_numIptEntriesInSubtrie;
            rightSubTrieNumApt = curTrieNode->m_child_p[1]->m_numAptEntriesInSubtrie;
        }

        /*
         * Find if the Left Subtrie is Empty
         */
        isLeftSubTrieEmpty = 1;
        if (leftSubTrieNumIpt || leftSubTrieNumApt)
        {
            isLeftSubTrieEmpty = 0;
        }

        /*
         * Find if the Right Subtrie is Empty
         */
        isRightSubTrieEmpty = 1;
        if (rightSubTrieNumIpt || rightSubTrieNumApt)
        {
            isRightSubTrieEmpty = 0;
        }

        if (isLeftSubTrieEmpty && !isRightSubTrieEmpty)
        {
            /*
             * Choose the Right Sub-trie
             */
            curTrieNode = curTrieNode->m_child_p[1];
        }
        else if (isRightSubTrieEmpty && !isLeftSubTrieEmpty)
        {
            /*
             * Choose the Left Sub-trie
             */
            curTrieNode = curTrieNode->m_child_p[0];
        }
        else if (!isLeftSubTrieEmpty && !isRightSubTrieEmpty)
        {
            /*
             * Both left and right sub-tries have entries in them. So we stop
             */
            break;
        }
        else
        {
            newRptPushNode = NULL;
            break;
        }
    }

    return newRptPushNode;
}

NlmErrNum_t
kaps_trie_push_rpt_node(
    kaps_lpm_trie * trie_p,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *newRptPushNode_p = NULL;
    uint32_t oldPoolId = 0, oldNumIptEntries = 0, oldNumAptEntries = 0;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    uint32_t *indexBuffer_p = trie_global->m_indexBuffer_p;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t numPools;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    kaps_trie_node *newUpstreamRptParent;
    uint8_t dbId = trie_p->m_tbl_ptr->m_tblId;
    NlmNsDownStreamRptNodes *curListNode, *nextListNode;
    uint32_t isListNodeMoreSpecificEqual;

    numPools = 2;
    if (!trie_p->m_trie_global->m_isAptPresent)
        numPools = 1;

    if (trie_global->m_curRptNodeToPush_p)
    {
        oldPoolId = trie_global->m_curRptNodeToPush_p->m_poolId;

        oldNumIptEntries = trie_global->m_curRptNodeToPush_p->m_numIptEntriesInSubtrie;
        oldNumAptEntries = trie_global->m_curRptNodeToPush_p->m_numAptEntriesInSubtrie;

        trie_global->m_rptNodeForCalcIptApt = trie_global->m_curRptNodeToPush_p;
        trie_global->useTrigLenForCalcIptApt = 0;
        kaps_trie_pvt_calc_num_ipt_and_apt(trie_global->m_curRptNodeToPush_p, NULL);

        kaps_assert(oldNumIptEntries == trie_global->m_curRptNodeToPush_p->m_numIptEntriesInSubtrie,
                    "Mismatch in number of IPT entries in the subtrie in RPT Push \n");

        kaps_assert(oldNumAptEntries == trie_global->m_curRptNodeToPush_p->m_numAptEntriesInSubtrie,
                    "Mismatch in number of APT entries in the subtrie in RPT Push \n");

        /*
         * Compute new RPT Push node
         */
        newRptPushNode_p = kaps_trie_find_new_rpt_push_node(trie_p);
        /*
         * If the new RPT Push node is empty or if the new RPT Push node is the same as cur RPT Node to Push, then
         * simply return
         */
        if (!newRptPushNode_p || newRptPushNode_p == trie_global->m_curRptNodeToPush_p)
        {
            return NLMERR_OK;
        }

        if (newRptPushNode_p->m_depth < trie_global->m_curRptNodeToPush_p->m_depth)
            return NLMERR_OK;

        /*
         * Ensure that we are going deeper by at least 8 bits once we push
         */
        if (newRptPushNode_p->m_depth - trie_global->m_curRptNodeToPush_p->m_depth < 8)
            return NLMERR_OK;

        /*
         * Assign a new pool to the RPT entries. 
         */
        if (!device->hw_res->no_dba_compression || device->type == KAPS_DEVICE_KAPS)
        {
            uint32_t longest_apt_entry_size = 0, longest_ipt_entry_size = 0, longest_size = 0;

            /*
             * Reset the IPT brick scratchpad before calling the recursive function
             * kaps_trie_pvt_calc_longest_size_for_ipt_and_apt
             */
            kaps_memset(&trie_p->m_iptBrickScratchpad, 0, sizeof(kaps_ipt_brick_scratchpad));

            kaps_trie_pvt_calc_longest_size_for_ipt_and_apt(newRptPushNode_p, newRptPushNode_p,
                                                            &longest_apt_entry_size, &longest_ipt_entry_size);
            if (longest_ipt_entry_size > longest_apt_entry_size)
                longest_size = longest_ipt_entry_size;
            else
                longest_size = longest_apt_entry_size;

            poolMgr->m_force_trigger_width = 1;

            poolMgr->m_cur_max_trigger_length_1 =
                longest_size - kaps_trie_get_num_rpt_bits_lopped_off(device, newRptPushNode_p->m_depth);

            if (device->type == KAPS_DEVICE_KAPS)
            {
                poolMgr->m_cur_max_trigger_length_1 += 1;       /* Add 1 for MPE */
            }
        }

        if (device->type == KAPS_DEVICE_KAPS)
        {
            /*
             * Calculate how many bricks we need for the triggers stored in the IPT Scratchpad
             */
            kaps_trie_find_num_bricks_for_big_kaps_triggers(trie_p);
        }

        errNum = kaps_trie_pvt_get_new_pool(poolMgr, newRptPushNode_p,
                                            newRptPushNode_p->m_numIptEntriesInSubtrie,
                                            newRptPushNode_p->m_numAptEntriesInSubtrie, numPools, o_reason);

        poolMgr->m_force_trigger_width = 0;

        if (errNum != NLMERR_OK)
        {
            return errNum;
        }

        /*
         * Create the RPT node 
         */
        newRptPushNode_p->m_isRptNode = 1;

        /*
         * Decrement the number of IPT entries and APT entries in the node to split 
         */
        kaps_trie_decr_num_ipt_entries_in_subtrie(trie_global->m_curRptNodeToPush_p,
                                                  newRptPushNode_p->m_numIptEntriesInSubtrie);

        /*
         * Decrease the count of IPT entries in the pool manager in the old pool. 
         */
        kaps_pool_mgr_decr_num_entries(poolMgr, oldPoolId, newRptPushNode_p->m_numIptEntriesInSubtrie, KAPS_IPT_POOL);

        /*
         * Increase the number of IPT entries in the new pool manager 
         */
        kaps_pool_mgr_incr_num_entries(poolMgr, newRptPushNode_p->m_poolId, newRptPushNode_p->m_numIptEntriesInSubtrie,
                                       KAPS_IPT_POOL);

        /*
         * Before calling Redistribute prefixes, make isRptNode false, so that we don't skip the RPT entry and its
         * subtrie while redistributing the IPT entries. This is needed since in some cases the RPT entry can also be
         * an IPT entry ? After calling the redistribute function, make isRptNode true 
         */

        newRptPushNode_p->m_isRptNode = 0;

        trie_global->m_oldPoolId = oldPoolId;
        trie_global->m_oldRptId = trie_global->m_curRptNodeToPush_p->m_rptId;
        trie_global->m_old_rpt_uuid = trie_global->m_curRptNodeToPush_p->m_rpt_uuid;
        trie_global->m_oldRptDepth = trie_global->m_curRptNodeToPush_p->m_depth;

        trie_global->m_newPoolId = newRptPushNode_p->m_poolId;
        trie_global->m_newRptId = newRptPushNode_p->m_rptId;
        trie_global->m_new_rpt_uuid = newRptPushNode_p->m_rpt_uuid;
        trie_global->m_newRptDepth = newRptPushNode_p->m_depth;

        trie_global->m_newRptNode_p = newRptPushNode_p;
        trie_global->m_deleteRptAptLmpsofar = 0;
        trie_global->m_indexBufferIter = 0;
        kaps_memset(indexBuffer_p, 0, NLMNS_MAX_INDEX_BUFFER_SIZE * 4);

        trie_global->m_rptOp = KAPS_RPT_PUSH;

        /*
         * Copy the IPT and APT to the new Pool
         */
        trie_p->m_isCopyIptAndAptToNewPoolInProgress = 1;

        errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(newRptPushNode_p, o_reason);

        trie_p->m_isCopyIptAndAptToNewPoolInProgress = 0;

        if (errNum != NLMERR_OK)
            return errNum;

        /*
         * Copy the IITLmpsofar
         */
        newRptPushNode_p->m_iitLmpsofarPfx_p = trie_global->m_curRptNodeToPush_p->m_iitLmpsofarPfx_p;

        /*
         * Commit the New RPT Entry
         */
        errNum = kaps_trie_commit_rpt_and_rit(newRptPushNode_p, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;

        trie_global->m_indexBufferIter = 0;

        /*
         * Delete the IPT and APT in the old pool
         */
        kaps_trie_delete_ipt_and_apt_in_old_pool(newRptPushNode_p, o_reason);

        newRptPushNode_p->m_isRptNode = 1;

        newRptPushNode_p->m_rptParent_p = newRptPushNode_p;

        /*
         * Delete the RPT entry being pushed from the RPT block
         */
        kaps_rpm_delete_entry_in_hw(trie_global->m_rpm_p, trie_global->m_curRptNodeToPush_p->m_rpt_prefix_p);

        kaps_rpm_remove_entry(trie_global->m_rpm_p, trie_global->m_curRptNodeToPush_p->m_rpt_prefix_p, dbId);

        kaps_pfx_bundle_destroy(trie_global->m_curRptNodeToPush_p->m_rpt_prefix_p, trie_global->m_alloc_p);

        trie_global->m_curRptNodeToPush_p->m_rpt_prefix_p = NULL;

        kaps_pool_mgr_release_pool_for_rpt_entry(trie_global->poolMgr,
                                                 trie_global->m_curRptNodeToPush_p,
                                                 trie_global->m_curRptNodeToPush_p->m_poolId,
                                                 trie_global->m_curRptNodeToPush_p->m_rptId);

        kaps_trie_release_rpt_uuid(trie_global->m_curRptNodeToPush_p);

        trie_global->m_curRptNodeToPush_p->m_rptParent_p = NULL;
        trie_global->m_curRptNodeToPush_p->m_isRptNode = 0;
        trie_global->m_curRptNodeToPush_p->m_iitLmpsofarPfx_p = NULL;

        /*
         * Find the upstream RPT Node above the RPT Node that we are trying to push
         */
        newUpstreamRptParent = trie_global->m_curRptNodeToPush_p->m_parent_p;
        while (newUpstreamRptParent && !newUpstreamRptParent->m_isRptNode)
        {
            newUpstreamRptParent = newUpstreamRptParent->m_parent_p;
        }

        /*
         * Add the downstream RPT node in the cur RPT Node being pushed to the newRptNode or to upstreamRptNode or
         * delete the list node. Suppose /37 node is being pushed and new RPT node is /46. Then if we encounter /45,
         * then /45 has to be added to upstreamRptNode. If upStreamRptNode is NULL, then it should be deleted
         */
        curListNode = trie_global->m_curRptNodeToPush_p->m_downstreamRptNodes;
        while (curListNode)
        {
            nextListNode = curListNode->next;

            isListNodeMoreSpecificEqual =
                kaps_prefix_pvt_is_more_specific_equal(curListNode->rptNode->m_lp_prefix_p->m_data,
                                                       curListNode->rptNode->m_lp_prefix_p->m_nPfxSize,
                                                       newRptPushNode_p->m_lp_prefix_p->m_data,
                                                       newRptPushNode_p->m_lp_prefix_p->m_nPfxSize);

            if (isListNodeMoreSpecificEqual)
            {
                curListNode->next = newRptPushNode_p->m_downstreamRptNodes;
                newRptPushNode_p->m_downstreamRptNodes = curListNode;
            }
            else
            {
                if (newUpstreamRptParent)
                {
                    curListNode->next = newUpstreamRptParent->m_downstreamRptNodes;
                    newUpstreamRptParent->m_downstreamRptNodes = curListNode;
                }
                else
                {
                    kaps_nlm_allocator_free(trie_global->m_alloc_p, curListNode);
                }
            }

            curListNode = nextListNode;
        }
        trie_global->m_curRptNodeToPush_p->m_downstreamRptNodes = NULL;

        if (newUpstreamRptParent)
        {
            /*
             * Go through the down stream RPT nodes and replace m_curRptNodeToPush_p with newRptPushNode_p
             */
            curListNode = newUpstreamRptParent->m_downstreamRptNodes;
            while (curListNode)
            {
                if (curListNode->rptNode == trie_global->m_curRptNodeToPush_p)
                {
                    curListNode->rptNode = newRptPushNode_p;
                    break;
                }

                curListNode = curListNode->next;
            }
        }

        /*
         * The upstream RPT Node will be the new RPT node for the nodes between m_curRptNodeToPush_p and
         * newRptPushNode_p Since we have set newRptPushNode_p->m_isRptNode = 1, we will stop once we reach
         * newRptPushNode_p. We have to perform this even if newUpstreamRptParent is NULL since NULL should get
         * propagated
         */
        kaps_trie_propagate_rpt_attributes(trie_global->m_curRptNodeToPush_p, newUpstreamRptParent);

        /*
         * Make newRptPushNode_p as the RPT parent for the node under it
         */
        newRptPushNode_p->m_isRptNode = 0;
        kaps_trie_propagate_rpt_attributes(newRptPushNode_p, newRptPushNode_p);
        newRptPushNode_p->m_isRptNode = 1;

        trie_p->m_tbl_ptr->m_fibStats.numRptPushes++;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_node_check_and_push_rpt_node(
    kaps_lpm_trie * trie_p)
{
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_device *device = trie_global->fibtblmgr->m_devMgr_p;
    NlmErrNum_t errNum;
    uint32_t numFreeSlotsInRpt, numFreeSlotsInIT;
    uint32_t ads2_num;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;

    /*
     * Enabling RPT push only for 3 level KAPS for the time being
     */
    if (db->num_algo_levels_in_db == 3)
    {
        if (trie_global->m_curRptNodeToPush_p->m_node_type == 1
            && trie_global->m_curRptNodeToPush_p->m_lsn_p->m_nNumPrefixes)
        {
            errNum =
                kaps_trie_giveout_pending_lsns(trie_p, trie_global->m_curRptNodeToPush_p->m_lsn_p, NULL, 1, &reason);
            if (errNum != NLMERR_OK)
                return NLMERR_OK;

            errNum = kaps_trie_update_hardware(trie_p, NULL, &reason);

            if (errNum != NLMERR_OK)
            {
                return errNum;
            }
            trie_global->curLsnToSplit = NULL;
        }

        numFreeSlotsInRpt = kaps_rpm_get_num_free_slots(trie_p->m_trie_global->m_rpm_p);

        ads2_num = 1;
        if (db->is_type_a)
            ads2_num = 0;

        numFreeSlotsInIT = device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots;

        if (numFreeSlotsInRpt && numFreeSlotsInIT)
        {
            kaps_trie_push_rpt_node(trie_p, &reason);
        }
    }

    trie_global->m_curRptNodeToPush_p = NULL;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_process_rpt_split_or_move(
    kaps_lpm_trie * self_p,
    NlmReasonCode * o_reason_p,
    uint32_t retry_with_rpt_split)
{
    uint32_t max = 0, loopvar = 0;
    kaps_trie_global *trieGlobal_p = self_p->m_trie_global;
    kaps_fib_tbl_mgr *fibTblMgr_p = self_p->m_trie_global->fibtblmgr;
    int32_t isMoveSuccessful, isRptSplitAllowed;
    kaps_trie_node *curRptNode_p;
    NlmErrNum_t errNum;
    kaps_pool_mgr *poolMgr = self_p->m_trie_global->poolMgr;
    struct kaps_device *device = fibTblMgr_p->m_devMgr_p;
    struct kaps_db *db = fibTblMgr_p->m_curFibTbl->m_db;
    uint32_t isIptAboveThreshold;
    uint32_t areFreeSlotsPresentForNewRpt;
    uint32_t ads2_num;

    if (device->main_dev)
        device = device->main_dev;

    if (db->parent)
        db = db->parent;

    if (db->num_algo_levels_in_db == 2)
    {
        /*
         * device will not have any RPT entries. So simply return
         */
        return NLMERR_OK;
    }

    max = trieGlobal_p->m_num_items_in_rpt_nodes_to_split_or_move;

    for (loopvar = 0; loopvar < max; loopvar++)
    {

        *o_reason_p = NLMRSC_REASON_OK;

        curRptNode_p = trieGlobal_p->m_rptNodesToSplitOrMove[loopvar];

        /*
         * If there are only a few IPT and APT entries in the RPT entry, then first try to move the RPT to a new pool
         */
        isMoveSuccessful = 0;

        
        {
            uint32_t num_bricks = self_p->m_iptBrickScratchpad.m_numActiveBricks;
            uint32_t numExtraEntriesRequired;

            if (num_bricks <= db->hw_res.db_res->max_num_bricks_per_ab)
            {

                numExtraEntriesRequired = 1;
                if (self_p->m_numExtraIptEntriesNeededDuringGiveout > numExtraEntriesRequired)
                    numExtraEntriesRequired = self_p->m_numExtraIptEntriesNeededDuringGiveout;

                errNum = kaps_trie_move_rpt_to_new_pool(self_p, curRptNode_p,
                                                        numExtraEntriesRequired, 0, 1, o_reason_p);

                poolMgr->m_force_trigger_width = 0;
                poolMgr->m_cur_max_trigger_length_1 = 0;

                if (errNum == NLMERR_OK)
                    isMoveSuccessful = 1;
            }

        }
        
        /*
         * If we couldn't move the RPT entry or expand the IPM, try to split the RPT entry
         */
        if (!isMoveSuccessful)
        {
            *o_reason_p = NLMRSC_REASON_OK;

            areFreeSlotsPresentForNewRpt = kaps_rpm_get_num_free_slots(self_p->m_trie_global->m_rpm_p);

            if (device->type == KAPS_DEVICE_KAPS)
            {
                ads2_num = 1;
                if (db->is_type_a)
                    ads2_num = 0;

                if (!device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots)
                    areFreeSlotsPresentForNewRpt = 0;
            }

            if (areFreeSlotsPresentForNewRpt)
            {

                isIptAboveThreshold = 0;
                if (kaps_pool_mgr_get_num_entries(poolMgr, curRptNode_p->m_poolId, KAPS_IPT_POOL) >=
                    kaps_pool_mgr_get_max_allowed_entries(poolMgr, curRptNode_p->m_poolId, KAPS_IPT_POOL))
                {
                    isIptAboveThreshold = 1;
                }

                if (curRptNode_p->m_numIptEntriesInSubtrie >= curRptNode_p->m_trie_p->m_maxAllowedIptEntriesForRpt)
                    isIptAboveThreshold = 1;

                isRptSplitAllowed = 0;

                if (isIptAboveThreshold)
                {
                    isRptSplitAllowed = 1;
                    if (curRptNode_p->m_numIptEntriesInSubtrie < KAPS_MIN_ENTRIES_NEEDED_FOR_SPLIT)
                    {
                        isRptSplitAllowed = 0;
                    }
                }

                /*
                 * If we are retrying after trying to split an RPT entry, then we don't check if IPT/APT is above
                 * threshold
                 */
                if (retry_with_rpt_split && curRptNode_p->m_numIptEntriesInSubtrie >= KAPS_MIN_ENTRIES_NEEDED_FOR_SPLIT)
                    isRptSplitAllowed = 1;

                if (isRptSplitAllowed)
                {
                    trieGlobal_p->m_curRptNodeToSplit_p = curRptNode_p;

                    kaps_trie_split_rpt_node(trieGlobal_p->m_curRptNodeToSplit_p->m_trie_p, o_reason_p);

                    if (*o_reason_p == NLMRSC_REASON_OK)
                        curRptNode_p->m_trie_p->m_tbl_ptr->m_fibStats.numRptSplits++;

                    trieGlobal_p->m_curRptNodeToPush_p = trieGlobal_p->m_curRptNodeToSplit_p;
                    kaps_trie_node_check_and_push_rpt_node(self_p);
                    trieGlobal_p->m_curRptNodeToPush_p = NULL;
                }

                trieGlobal_p->m_curRptNodeToSplit_p = NULL;
            }
        }
    }

    if (max > 0)
        trieGlobal_p->m_num_items_in_rpt_nodes_to_split_or_move = 0;

    /*
     * Suppress any error if exists as down the code path we will handle those errors 
     */
    self_p->m_rptSplitReasonCode = *o_reason_p;
    *o_reason_p = NLMRSC_REASON_OK;
    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_commit_iit_lmpsofar(
    kaps_lpm_trie * trie_p,
    NlmReasonCode * o_reason)
{
    NLM_STRY(kaps_trie_commit_iits(trie_p, o_reason));

    trie_p->m_num_items_in_to_update_sit = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_commit_rit_iit_lmpsofar(
    kaps_lpm_trie * trie_p,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_fib_tbl_mgr *fibTblMgr = trie_p->m_trie_global->fibtblmgr;
    struct kaps_pct ritEntry[MAX_COPIES_PER_LPM_DB];

    uint32_t i = 0, nPending = trie_p->m_num_items_in_to_update_rit;
    kaps_trie_node *rptNode = NULL;

    for (i = 0; i < nPending; i++)
    {
        rptNode = trie_p->m_toupdate_rit[i];

        errNum = kaps_rpm_construct_rit(rptNode, ritEntry);
        if (errNum != NLMERR_OK)
            kaps_assert(0, "Error while constructing RIT \n");

        errNum = kaps_fib_rit_write(fibTblMgr, 0, ritEntry, rptNode->m_rpt_prefix_p->m_nIndex, o_reason);
        if (errNum != NLMERR_OK)
            kaps_assert(0, "Error writing RIT entry \n");
    }

    trie_p->m_num_items_in_to_update_rit = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_process_iit_lmpsofar(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * destnode,
    struct kaps_lpm_entry * entry,
    NlmTblRqtCode rqtCode,
    NlmReasonCode * o_reason)
{
    kaps_pfx_bundle *lmpsofarPfxBundle_p = NULL;
    kaps_trie_node *iterTrieNode;
    uint16_t depth;
    uint8_t bit;
    uint8_t *pfxData = entry->pfx_bundle->m_data;
    uint16_t pfxLen = entry->pfx_bundle->m_nPfxSize;
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    uint32_t recomputeLmpsofar;
    struct kaps_db *db = trie_global->fibtblmgr->m_curFibTbl->m_db;

    if (rqtCode == NLM_FIB_PREFIX_INSERT || rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
    {
        /*
         * During restore state, TrieSubmitRqt is called using prefix copies. We don't need to propagate prefix copies
         * downstream. Only the real prefixes added by the user have to be propagated further down stream. So returning 
         * if we have a prefix copy
         */
        if (entry->pfx_bundle->m_isPfxCopy)
        {
            return NLMERR_OK;
        }
        lmpsofarPfxBundle_p = entry->pfx_bundle;
    }
    else if (rqtCode == NLM_FIB_PREFIX_DELETE)
    {
        /*
         * If the operation is a delete, then find the longest prefix that is shorter than the prefix being deleted and 
         * that is in the same path of the prefix being deleted. If such a prefix exists, then it will be the new
         * lmpsofar to be propagated down in the trie. Otherwise the lmpsofar of the trienode has to be propagated down 
         * in the trie
         */
        lmpsofarPfxBundle_p = kaps_lsn_mc_locate_lpm(destnode->m_lsn_p, pfxData, pfxLen, NULL, NULL, NULL);
        if (!lmpsofarPfxBundle_p || lmpsofarPfxBundle_p->m_isPfxCopy
            || (destnode->m_iitLmpsofarPfx_p
                && destnode->m_iitLmpsofarPfx_p->m_backPtr->meta_priority <
                lmpsofarPfxBundle_p->m_backPtr->meta_priority))
        {
            /*
             * Suppose the lmpsofar is not found in the LSN, then pick the iitLmpsofarPfx_p from the dest trie node
             * Suppose the lmpsofar is a prefix copy, then also pick the iitLmpsofarPfx_p from the dest trie node since 
             * we don't propagate prefix copies further down for iitLmpsofar processing Suppose the
             * destnode->m_iitLmpsofarPfx_p is of higher priority than lmpsofar in the LSN, pick the iitLmpsofarPfx_p
             * from the dest trie node 
             */
            lmpsofarPfxBundle_p = destnode->m_iitLmpsofarPfx_p;
        }
    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
    {
        lmpsofarPfxBundle_p = entry->pfx_bundle;
    }
    else
    {
        return NLMERR_FAIL;
    }

    /*
     * Traverse the trie till we reach the trie node that corresponds to the lmpsofar to be propagated down
     */
    depth = destnode->m_depth;
    iterTrieNode = destnode;
    while (depth < pfxLen && iterTrieNode)
    {
        bit = KAPS_PREFIX_PVT_GET_BIT(pfxData, pfxLen, depth);
        iterTrieNode = iterTrieNode->m_child_p[bit];
        depth++;
    }
    /*
     * Suppose we have LsnLmpsofar and we support multiple meta priorities. Let us say that /17 with meta priority of 2 
     * was deleted from LSN with depth 12. lmpsofarPfxBundle_p will hold the prefix with highest meta priority that is
     * shallower than 17. Let us say that it is /14 with meta priority 3. But there may be a deeper prefix /18 with
     * meta priority 3 that is the lmpsofar of one of the LSNs at depth 21 that is immediately below original LSN with
     * depth 12. A different LSN at depth 20 that is immediately below LSN at depth 12 may have a different Lmpsofar,
     * say /19 with meta priority 3. So when we perform delete, in the LSNs immediately below an LSN, we have to
     * recompute the lmpsofar by looking at the ancestor LSN
     */
    recomputeLmpsofar = 0;
    if (rqtCode == NLM_FIB_PREFIX_DELETE && destnode->m_lsn_p->m_pSettings->m_recomputeLmpOnDelete)
    {
        recomputeLmpsofar = 1;
    }

    /*
     * If the trie node corresponding to the lmpsofar to be propagated down exists, then propagate the lmpsofar in the
     * sub-trie
     */
    if (iterTrieNode)
    {
        kaps_trie_node_update_iitLmpsofar(iterTrieNode->m_child_p[0], destnode, lmpsofarPfxBundle_p, entry->pfx_bundle,
                                          rqtCode, recomputeLmpsofar);
        kaps_trie_node_update_iitLmpsofar(iterTrieNode->m_child_p[1], destnode, lmpsofarPfxBundle_p, entry->pfx_bundle,
                                          rqtCode, recomputeLmpsofar);
    }

    if (trie_global->m_isIITLmpsofar)
    {
        NLM_STRY(kaps_trie_commit_iit_lmpsofar(trie_p, o_reason));
        if (db->num_algo_levels_in_db > 2)
            NLM_STRY(kaps_trie_commit_rit_iit_lmpsofar(trie_p, o_reason));
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_get_db_details(
    struct kaps_device *device,
    uint32_t * actual_num_entries_in_dev_p,
    uint32_t * normalized_num_entries_p,
    uint32_t * num_dbs,
    uint32_t * is_public_db_present)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *db, *tab;
    struct kaps_lpm_db *lpm_db;
    uint32_t normalized_num_entries;

    *actual_num_entries_in_dev_p = 0;
    normalized_num_entries = 0;
    *num_dbs = 0;
    *is_public_db_present = 0;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_DB_LPM)
            continue;
        if (db->is_bc_required && (!db->is_main_bc_db))
            continue;

        (*num_dbs)++;

        if (db->is_public)
            *is_public_db_present = 1;

        tab = db;

        while (tab)
        {
            if (tab->is_clone)
            {
                tab = tab->next_tab;
                continue;
            }

            lpm_db = (struct kaps_lpm_db *) tab;

            *actual_num_entries_in_dev_p += lpm_db->fib_tbl->m_numPrefixes;

            if (tab->key->width_1 < 88)
                normalized_num_entries += lpm_db->fib_tbl->m_numPrefixes;
            else if (tab->key->width_1 < 128)
                normalized_num_entries += 2 * lpm_db->fib_tbl->m_numPrefixes;
            else
                normalized_num_entries += 4 * lpm_db->fib_tbl->m_numPrefixes;

            tab = tab->next_tab;
        }
    }

    if (normalized_num_entries_p)
        *normalized_num_entries_p = normalized_num_entries;

    return NLMERR_OK;
}

static uint32_t
kaps_trie_is_dev_cap_full(
    kaps_lpm_trie * trie,
    struct kaps_device *device)
{

    return 0;
}

NlmErrNum_t
kaps_trie_clear_hit_bit_for_new_entry(
    kaps_lpm_trie * trie_p,
    struct kaps_lpm_entry * lpm_entry)
{
    struct kaps_device *device = trie_p->m_tbl_ptr->m_db->device;
    uint32_t hb_index;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;
    struct kaps_hb_db *hb_db;
    struct kaps_hb *hb = NULL;
    struct kaps_aging_entry *active_aging_table;

    if (device->type != KAPS_DEVICE_KAPS)
        return NLMERR_OK;

    if (db->parent)
        db = db->parent;

    hb_db = (struct kaps_hb_db *) db->common_info->hb_info.hb;

    if (hb_db && lpm_entry->hb_user_handle != 0)
    {
        active_aging_table = kaps_device_get_active_aging_table(device, db);

        KAPS_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb), (uintptr_t) lpm_entry->hb_user_handle);

        if (hb)
        {
            hb_index = hb->bit_no;

            active_aging_table[hb_index].entry = NULL;

            hb->bit_no = 0;
        }
    }

    return NLMERR_OK;
}

/************************************************
 * Function :
 * kaps_trie_submit_rqtInner
 *
 * Parameters:
 *    kaps_lpm_trie *self,
 *    struct kaps_lpm_entry *entry,
 *    NlmReasonCode *o_reason
 *
 * Summary:
 * __SubmitRqt is called to serve the prefix addition/deletion request for a given ftm table.
 *    In case of prefix addition, it checks for duplicate prefixes and then It traverses trie to locate
 * a lp node for that prefix then forwards the request to desired lp node.
 * In case of deletion, it locates that prefix and if found then it deletes that prefix.
 *
 * It returns NLMERR_OK if successful, otherwise it returns NLMERR_FAIL and sets
 * the *o_reason accordingly.
 *
 ***********************************************/
NlmErrNum_t
kaps_trie_submit_rqtInner(
    kaps_lpm_trie * trie_p,
    NlmTblRqtCode rqtCode,
    struct kaps_lpm_entry * entry,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *destnode = NULL;
    kaps_trie_node *lastseen_node_p = NULL;
    kaps_trie_node *temp_node_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_lpm_entry **entrySlotInHash = NULL;
    uint8_t *pfxData = NULL;
    kaps_trie_global *trie_global;
    struct kaps_device *device = NULL;
    kaps_status status;
    kaps_pool_mgr *poolMgr;
    uint32_t i;
    uint32_t wasGiveoutPerformed = 0;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;

    if (!trie_p || !entry)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NLMERR_FAIL;
    }

    trie_global = trie_p->m_trie_global;
    poolMgr = trie_global->poolMgr;

    device = trie_global->fibtblmgr->m_devMgr_p;

    if (device->main_dev)
        device = device->main_dev;

    trie_p->m_lsn_settings_p->m_device = device;

    trie_p->m_trie_global->m_numTrieNodesForMerge = 0;

    if (rqtCode == NLM_FIB_PREFIX_INSERT)
    {
        if (kaps_trie_is_dev_cap_full(trie_p, device))
        {
            *o_reason = NLMRSC_UDA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }
    }

    trie_global->cur_ad_db = NULL;
    if (entry->ad_handle)
    {
        KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, trie_global->cur_ad_db);
    }

    if ((rqtCode == NLM_FIB_PREFIX_INSERT)
        && trie_p->m_lsn_settings_p->m_hasSpecialEntry
        && trie_global->cur_ad_db
        && trie_global->cur_ad_db->db_info.hw_res.ad_res->is_1_1 && trie_global->cur_ad_db->mgr[device->core_id]->size)
    {

        struct kaps_ix_mgr *mgr = trie_global->cur_ad_db->mgr[device->core_id];
        int32_t num_free_ix = mgr->size - mgr->num_allocated_ix - mgr->cb_size;

        kaps_sassert(num_free_ix >= 0);

        if (num_free_ix < KAPS_MAX_RESV_IX_FOR_SPECIAL_ENTRIES)
        {

            status = kaps_kaps_ix_mgr_alloc_ad_dynamically(trie_p->m_ixMgr, trie_global->cur_ad_db, 0, 0);

            if (status == KAPS_OUT_OF_AD)
            {
                *o_reason = NLMRSC_OUT_OF_AD;
                return NLMERR_FAIL;
            }
            else if (status != KAPS_OK)
            {
                *o_reason = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }
        }
    }

    pfxData = entry->pfx_bundle->m_data;
    trie_global->curLsnToSplit = NULL;
    for (i = 0; i < KAPS_MAX_NUM_STORED_LSN_INFO; ++i)
    {
        trie_p->m_lsn_settings_p->m_isLsnInfoValid[i] = 0;
    }

    if (rqtCode == NLM_FIB_PREFIX_DELETE)
        trie_global->m_numDeletes++;

    /*
     * If the hashtable exists, then look inside the hash table for incoming prefix (NKG) 
     */
    if (trie_p->m_hashtable_p)
    {
        struct kaps_entry **ht_slot;

        status = kaps_pfx_hash_table_locate(trie_p->m_hashtable_p, pfxData, entry->pfx_bundle->m_nPfxSize, &ht_slot);
        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        entrySlotInHash = (struct kaps_lpm_entry **) ht_slot;

        switch (rqtCode)
        {
            case NLM_FIB_PREFIX_INSERT_WITH_INDEX:
                if (entrySlotInHash && !entry->pfx_bundle->m_isPfxCopy)
                {
                    /*
                     * inform the caller that the prefix already exist 
                     */
                    *o_reason = NLMRSC_DUPLICATE_PREFIX;
                    return NLMERR_FAIL;
                }
                break;
            case NLM_FIB_PREFIX_INSERT:
            case NLM_FIB_PREFIX_DELETE:
            case NLM_FIB_PREFIX_UPDATE_AD:
            case NLM_FIB_PREFIX_UPDATE_AD_ADDRESS:
                if (!entrySlotInHash)
                {
                    /*
                     * inform the caller that the prefix does not exist 
                     */
                    *o_reason = NLMRSC_PREFIX_NOT_FOUND;
                    return NLMERR_FAIL;
                }
                break;
        }
    }

    kaps_seq_num_gen_generate_next_pfx_seq_nr();

    switch (rqtCode)
    {
        case NLM_FIB_PREFIX_INSERT:
        case NLM_FIB_PREFIX_INSERT_WITH_INDEX:

            temp_node_p =
                kaps_trie_pvt_is_cache_hit(trie_p->m_cache_trienode_p, pfxData, entry->pfx_bundle->m_nPfxSize);
            if (!temp_node_p)
                temp_node_p = trie_p->m_roots_trienode_p;

            /*
             * Find the destination node for the requested prefix 
             */
            destnode =
                kaps_trie_find_dest_lp_node(trie_p, temp_node_p, pfxData, entry->pfx_bundle->m_nPfxSize,
                                            &lastseen_node_p, 1);

            trie_p->m_cache_trienode_p = destnode;

            break;
        case NLM_FIB_PREFIX_DELETE:
        case NLM_FIB_PREFIX_UPDATE_AD:
        case NLM_FIB_PREFIX_UPDATE_AD_ADDRESS:
        {
            /*
             * Find the destination node for the requested prefix 
             */
            destnode = kaps_trie_find_dest_lp_node(trie_p, trie_p->m_roots_trienode_p, pfxData,
                                                   entry->pfx_bundle->m_nPfxSize, &lastseen_node_p, 0);
            trie_p->m_cache_trienode_p = NULL;

            break;
        }
    }

    if (!destnode)
    {
        *o_reason = NLMRSC_LOCATE_NODE_FAILED;
        return NLMERR_FAIL;
    }

    if (destnode->m_lsn_p->m_bIsNewLsn && !destnode->m_rptParent_p)
    {
        errNum = kaps_trie_check_rpt_resources_for_new_lsn(destnode, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
    }

    /*
     * Submit the request to LSN Manager 
     */
    errNum = kaps_lsn_mc_submit_rqt(destnode->m_lsn_p, rqtCode, entry, entrySlotInHash, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    if (rqtCode == NLM_FIB_PREFIX_INSERT)
    {
        if (destnode->m_lsn_p->m_bIsNewLsn)
        {
            if (!destnode->m_rptParent_p)
            {
                errNum = kaps_trie_compute_rpt_parent(destnode, o_reason);
                if (errNum != NLMERR_OK)
                {
                    kaps_lsn_mc_destroy(destnode->m_lsn_p);
                    destnode->m_lsn_p = kaps_trie_node_pvt_create_lsn(destnode);

                    if (*o_reason != NLMRSC_LOW_MEMORY)
                        *o_reason = NLMRSC_DBA_ALLOC_FAILED;

                    entry->pfx_bundle->m_nIndex = KAPS_LSN_NEW_INDEX;
                    kaps_trie_clear_hit_bit_for_new_entry(trie_p, entry);

                    return errNum;
                }
            }

            /*
             * The max depth here will be 8-bits. So we mostly don't need any special handling for Reserved-160b ABs
             */
            if (kaps_pool_mgr_get_num_entries(poolMgr, destnode->m_rptParent_p->m_poolId, KAPS_IPT_POOL)
                >= kaps_pool_mgr_get_max_entries(poolMgr, destnode->m_rptParent_p->m_poolId, KAPS_IPT_POOL))
            {

                kaps_lsn_mc_destroy(destnode->m_lsn_p);
                destnode->m_lsn_p = kaps_trie_node_pvt_create_lsn(destnode);

                entry->pfx_bundle->m_nIndex = KAPS_LSN_NEW_INDEX;
                kaps_trie_clear_hit_bit_for_new_entry(trie_p, entry);

                if (db->num_algo_levels_in_db == 3)
                {
                    kaps_trie_big_kaps_get_trigger_lengths_into_scratchpad(trie_p,
                                                                           kaps_trie_get_num_rpt_bits_lopped_off
                                                                           (device,
                                                                            destnode->m_rptParent_p->m_depth),
                                                                           destnode->m_rptParent_p->m_poolId, 1,
                                                                           destnode->m_depth);

                    kaps_trie_find_num_bricks_for_big_kaps_triggers(trie_p);


                    kaps_trie_pvt_add_rpt_nodes_to_split_or_move(trie_p, destnode->m_rptParent_p);

                    trie_p->m_isRptSplitDueToIpt = 1;
                    *o_reason = NLMRSC_RETRY_WITH_RPT_SPLIT;
                    return NLMERR_FAIL;
                }
                else
                {
                    *o_reason = NLMRSC_DBA_ALLOC_FAILED;
                    return NLMERR_FAIL;
                }
            }

            errNum = kaps_trie_add_ses(trie_p, destnode, destnode->m_lp_prefix_p, o_reason);
            if (errNum != NLMERR_OK)
                return errNum;

            destnode->m_lsn_p->m_bIsNewLsn = 0;

        }
        else if (destnode->m_lsn_p->m_bDoGiveout)
        {
            destnode->m_lsn_p->m_bDoGiveout = 0;

            errNum = kaps_trie_giveout_pending_lsns(trie_p, destnode->m_lsn_p, entry, 0, o_reason);
            if (errNum != NLMERR_OK)
            {
                return errNum;
            }

            wasGiveoutPerformed = 1;

            errNum = kaps_trie_update_hardware(trie_p, entry, o_reason);

            if (errNum != NLMERR_OK)
            {
                return errNum;
            }
            trie_global->curLsnToSplit = NULL;
        }

        if (trie_global->m_isIITLmpsofar || trie_global->m_isLsnLmpsofar)
            errNum = kaps_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);

        if (wasGiveoutPerformed)
        {
            kaps_trie_process_merge_after_giveout(trie_p);
        }

    }
    else if (rqtCode == NLM_FIB_PREFIX_DELETE)
    {
        if (trie_global->m_isIITLmpsofar || trie_global->m_isLsnLmpsofar)
            errNum = kaps_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);

        if (destnode->m_lsn_p->m_nNumPrefixes == 0)
        {
            /*
             * This is empty LSN so delete it 
             */
            NLM_STRY(kaps_trie_pvt_in_place_delete(trie_p, destnode, 1, o_reason));
        }
        else
        {
            NLM_STRY(kaps_trie_pvt_merge_ancestor_lsns(trie_p, destnode, o_reason));
        }
    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
    {
        if (trie_global->m_isIITLmpsofar || trie_global->m_isLsnLmpsofar)
            errNum = kaps_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);

    }
    else if (rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
    {
        if (trie_global->m_isIITLmpsofar || trie_global->m_isLsnLmpsofar)
            errNum = kaps_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);
    }

    trie_p->m_isRecursiveSplit = 0;

    return errNum;
}

extern NlmErrNum_t
kaps_trie_submit_rqt(
    kaps_lpm_trie * trie_p,
    NlmTblRqtCode rqtCode,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t err = NLMERR_OK;
    uint32_t retry_with_rpt_split;
    uint32_t doSubmitRqt;
    uint32_t maxRetryCnt, curRetryCnt;
    uint32_t isTblFull = 0;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;

    g_submitRqtHitCnt++;

    trie_p->m_rptSplitReasonCode = NLMRSC_REASON_OK;
    trie_p->m_isRptSplitDueToIpt = 0;
    trie_p->m_isRptSplitDueToApt = 0;
    trie_p->m_tbl_ptr->m_fibTblMgr_p->m_bufferKapsABWrites = 0;
    trie_p->m_trie_global->m_move_reserved_160b_trig_to_regular_pool = 0;

    maxRetryCnt = 5;
    curRetryCnt = 0;
    doSubmitRqt = 1;
    retry_with_rpt_split = 0;

    while (doSubmitRqt)
    {
        curRetryCnt++;

        err = kaps_trie_submit_rqtInner(trie_p, rqtCode, entry, o_reason);

        if (*o_reason == NLMRSC_REASON_OK || *o_reason == NLMRSC_RETRY_WITH_RPT_SPLIT)
        {

            retry_with_rpt_split = 0;
            if (*o_reason == NLMRSC_RETRY_WITH_RPT_SPLIT)
            {
                *o_reason = NLMRSC_REASON_OK;
                retry_with_rpt_split = 1;
            }

            if (rqtCode == NLM_FIB_PREFIX_INSERT || rqtCode == NLM_FIB_PREFIX_DELETE)
            {
                kaps_trie_process_rpt_split_or_move(trie_p, o_reason, retry_with_rpt_split);
            }

        }
        else
        {
            break;
        }

        trie_p->m_isRptSplitDueToIpt = 0;
        trie_p->m_isRptSplitDueToApt = 0;

        doSubmitRqt = 0;
        if (retry_with_rpt_split && curRetryCnt < maxRetryCnt)
            doSubmitRqt = 1;

        trie_p->m_rpt_move_failed_during_ipt_gran_change = 0;
    }

    if (retry_with_rpt_split)
    {
        if (*o_reason == NLMRSC_REASON_OK)
        {
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
        }

        if (trie_p->m_rptSplitReasonCode != NLMRSC_REASON_OK)
        {
            *o_reason = trie_p->m_rptSplitReasonCode;
        }

        if (db->num_algo_levels_in_db == 3 && kaps_rpm_get_num_free_slots(trie_p->m_trie_global->m_rpm_p) == 0)
        {
            *o_reason = NLMRSC_PCM_ALLOC_FAILED;
        }

        if (db->num_algo_levels_in_db == 3)
        {
            if (kaps_trie_is_big_kaps_it_full(trie_p))
            {
                *o_reason = NLMRSC_IT_ALLOC_FAILED;
            }
        }
    }

    if (curRetryCnt > trie_p->m_tbl_ptr->m_fibStats.maxNumRptSplitsInOneOperation)
        trie_p->m_tbl_ptr->m_fibStats.maxNumRptSplitsInOneOperation = curRetryCnt;

    if (curRetryCnt > 1)
    {
        trie_p->m_tbl_ptr->m_fibStats.numRecursiveRptSplits++;
    }

    trie_p->m_rpt_move_failed_during_ipt_gran_change = 0;
    trie_p->m_numExtraIptEntriesNeededDuringGiveout = 0;
    if (db->num_algo_levels_in_db == 3)
    {
        kaps_memset(&trie_p->m_iptBrickScratchpad, 0, sizeof(kaps_ipt_brick_scratchpad));
    }

    (void) isTblFull;

    if (*o_reason == NLMRSC_PCM_ALLOC_FAILED ||
        *o_reason == NLMRSC_DBA_ALLOC_FAILED ||
        *o_reason == NLMRSC_IT_ALLOC_FAILED || 
        *o_reason == NLMRSC_UDA_ALLOC_FAILED || 
        *o_reason == NLMRSC_OUT_OF_AD)
    {
        isTblFull = 1;
        /* kaps_trie_log_stats_and_html(trie_p);*/
    }

    return err;
}

/*
 * This is called after inplace deletes and needs to propagate
 * devid changes and clean up anything remaining on any of the lists.
 * Null's are deleted in-line in kaps_del_pfx_in_place()
 */
 /************************************************
 * Function :
 * kaps_trie_pvt_cleanup_del_nodes
 *
 * Parameters:
 *    kaps_lpm_trie *self,
 *    void *rqtprefix,
 *    NlmReasonCode *o_reason
 *
 * Summary:
 * __pvt_CleanupDelNodes is called after inplace deletes and needs to propagate
 * devid changes and clean up anything remaining on any of the lists.
 * Null's are deleted in-line in kaps_del_pfx_in_place()
 *
 ***********************************************/
static void
kaps_trie_pvt_cleanup_del_nodes(
    kaps_lpm_trie * trie_p)
{
    kaps_lsn_mc *lsn_p = NULL;
    uint32_t i = 0, max = 0;

    /*
     * Now, scan through the lists, perform required tasks
     * and clean them up.  CleanupSpecial() can leave update_lsns and backups,
     * but they will never be needed since the CleanupSpecial will never fail
     */
    /*
     * Delete all the LSNs in the list 
     */
    max = trie_p->m_num_items_in_to_delete_lsns;
    for (i = 0; i < max; i++)
    {
        lsn_p = trie_p->m_todelete_lsns[i];
        kaps_lsn_mc_destroy(lsn_p);
    }
    trie_p->m_num_items_in_to_delete_lsns = 0;

    /*
     * Delete all the trie node in the list 
     */
    max = trie_p->m_num_items_in_to_delete_nodes;
    for (i = 0; i < max; i++)
    {
        kaps_trie_node *node_p = trie_p->m_todelete_nodes[i];
        kaps_trie_node_destroy(node_p, trie_p->m_trie_global->m_alloc_p, 0);
    }
    trie_p->m_num_items_in_to_delete_nodes = 0;
}

/************************************************
 * Function :
 * kaps_trie_find_dest_lp_node
 *
 * Summary:
 * __FindDestLpNode finds the destinationLP node, according to prefix data,
 * from the root node. It also finds the last seen node in the trie and sets its value.
 *
 * It returns destination LP node if successful, otherwise it returns NULL.
 *
 ***********************************************/

static kaps_trie_node *
kaps_trie_find_dest_lp_node(
    kaps_lpm_trie * self,
    kaps_trie_node * start_node,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    kaps_trie_node ** last_seen_node,
    int32_t isPfxInsertion)
{
    kaps_trie_node *node = start_node;
    kaps_trie_node *remember_lp = node;
    kaps_trie_node *last_node = node;
    uint32_t i = 0, val = 0;
    uint32_t chosen_depth;

    if (pfxlen < self->m_min_lopoff)
        kaps_assert(0, "Prefix should be >= minimum threshold\n");

    chosen_depth = self->m_expansion_depth;

    for (i = node->m_depth; (i < pfxlen); i++)
    {
        val = KAPS_PREFIX_PVT_GET_BIT(pfxdata, pfxlen, i);

        node = node->m_child_p[val];
        if (!node)
            break;

        if (node->m_node_type != NLMNSTRIENODE_REGULAR)
        {
            remember_lp = node;
        }
        else if (isPfxInsertion && node->m_isRptNode)
        {
            remember_lp = node;
        }
        else if (isPfxInsertion && remember_lp == self->m_roots_trienode_p && node->m_depth == chosen_depth)
        {
            remember_lp = node;
        }

        last_node = node;
    }

    if (last_seen_node)
        *last_seen_node = last_node;

    return remember_lp;
}

/************************************************
 * Function :
 * kaps_trie_return_stack_space
 *
 * Parameters:
 *    kaps_lpm_trie *self,
 *    kaps_trie_node *stack
 *
 * Summary:
 * __ReturnStackSpace returns the specified stack back to trie.
 *
 ***********************************************/
void
kaps_trie_return_stack_space(
    kaps_lpm_trie * self,
    kaps_trie_node ** stack)
{
    if (stack == self->m_recurse_stack[0])
    {
        kaps_assert(self->m_recurse_stack_inuse[0] == 1, "Stack inuse field improper");
        self->m_recurse_stack_inuse[0] = 0;
    }
    else if (stack == self->m_recurse_stack[1])
    {
        kaps_assert(self->m_recurse_stack_inuse[1] == 1, "Stack inuse field improper");
        self->m_recurse_stack_inuse[1] = 0;
    }
    else if (stack == self->m_recurse_stack[2])
    {
        kaps_assert(self->m_recurse_stack_inuse[2] == 1, "Stack inuse field improper");
        self->m_recurse_stack_inuse[2] = 0;
    }
    else
        kaps_assert(0, "Invalid stack pointer ");
}

/************************************************
 * Function :
 * kaps_trie_get_stack_space
 *
 * Parameters:
 *    kaps_lpm_trie *self
 *
 * Summary:
 * __GetStackSpace gives the first stack available to the caller.
 *
 ***********************************************/
kaps_trie_node **
kaps_trie_get_stack_space(
    kaps_lpm_trie * self)
{
    if (self->m_recurse_stack_inuse[0])
    {
        if (self->m_recurse_stack_inuse[1])
        {
            if (self->m_recurse_stack_inuse[2])
            {
                kaps_assert(0, "Stack space not available");
                return 0;
            }
            else
            {
                self->m_recurse_stack_inuse[2] = 1;
                return self->m_recurse_stack[2];
            }
        }
        else
        {
            self->m_recurse_stack_inuse[1] = 1;
            return self->m_recurse_stack[1];
        }
    }
    else
    {
        self->m_recurse_stack_inuse[0] = 1;
        return self->m_recurse_stack[0];
    }
}

/************************************************
 * Function :
 * kaps_trie_node_get_stack_space_args
 *
 * Parameters:
 *    kaps_lpm_trie *self
 *
 * Summary:
 * __GetStackSpaceArgs gives the first stack space for arguments
 * available to the caller.
 *
 ***********************************************/
NlmNsTrieNode__TraverseArgs *
kaps_trie_node_get_stack_space_args(
    kaps_lpm_trie * self)
{
    if (self->m_recurse_args_stack_inuse[0])
    {
        if (self->m_recurse_args_stack_inuse[1])
        {
            if (self->m_recurse_args_stack_inuse[2])
            {
                kaps_assert(0, "All the stacks are in use");
                return 0;
            }
            else
            {
                self->m_recurse_args_stack_inuse[2] = 1;
                return self->m_recurse_args_stack[2];
            }
        }
        else
        {
            self->m_recurse_args_stack_inuse[1] = 1;
            return self->m_recurse_args_stack[1];
        }
    }
    else
    {
        self->m_recurse_args_stack_inuse[0] = 1;
        return self->m_recurse_args_stack[0];
    }
}

/************************************************
 * Function :
 * kaps_trie_node_return_stack_space_args
 *
 * Parameters:
 *    kaps_lpm_trie *self
 *    NlmNsTrieNode__TraverseArgs * stk
 *
 * Summary:
 * __ReturnStackSpaceArgs returns the specified stack space for arguments
 * back to trie.
 *
 ***********************************************/
void
kaps_trie_node_return_stack_space_args(
    kaps_lpm_trie * self,
    NlmNsTrieNode__TraverseArgs * stk)
{
    if (stk == self->m_recurse_args_stack[0])
    {
        kaps_assert(self->m_recurse_args_stack_inuse[0] == 1, "Must be in use to return stack space");
        self->m_recurse_args_stack_inuse[0] = 0;
    }
    else if (stk == self->m_recurse_args_stack[1])
    {
        kaps_assert(self->m_recurse_args_stack_inuse[1] == 1, "Must be in use to return stack space");
        self->m_recurse_args_stack_inuse[1] = 0;
    }
    else if (stk == self->m_recurse_args_stack[2])
    {
        kaps_assert(self->m_recurse_args_stack_inuse[2] == 1, "Must be in use to return stack space");
        self->m_recurse_args_stack_inuse[2] = 0;
    }
    else
        kaps_assert(0, "Junk stack passed");
}

static NlmErrNum_t
kaps_trie_pvt_delete_ses_in_all_dev(
    kaps_lpm_trie * self,
    kaps_pfx_bundle * pfxBundle,
    uint32_t isMoveToNewPoolAllowed,
    NlmReasonCode * o_reason)
{
    uint8_t *tempPtr = NULL;
    kaps_trie_node *trienode = NULL, *rptParent_p = NULL;
    int32_t rptId = 0, rptDepth = 0;
    uint16_t numRptBitsLoppedOff = 0;
    kaps_pool_mgr *poolMgr = self->m_trie_global->poolMgr;
    kaps_lsn_mc_settings *settings = self->m_lsn_settings_p;
    struct kaps_device *device = self->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
    kaps_ipm *ipm_p;
    uint32_t virtual_it_index, real_it_index;
    uint32_t actualPoolId;
    struct kaps_db *db = self->m_tbl_ptr->m_db;

    (void) o_reason;

    tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle);
    kaps_memcpy(&trienode, tempPtr, sizeof(kaps_trie_node *));

    rptParent_p = trienode->m_rptParent_p;

    if (!rptParent_p->m_isRptNode)
        kaps_assert(0, "Could not find the Rpt node \n");

    if (trienode->m_iitLmpsofarPfx_p)
    {
        if (settings->m_areHitBitsPresent && trienode->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
        {
            struct kaps_db *db;
            struct kaps_hb *hb = NULL;

            KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, trienode->m_iitLmpsofarPfx_p->m_backPtr, db)
                KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb),
                                        (uintptr_t) trienode->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

            /*
             * The number of idles is set to 0xffffffff and after incrementing we will get num_idles equal to 0. We do
             * this because, it is possible that Lmpsofar entry was hit and before calling hit bit dump and we are
             * deleting the SES entry corresponding to the Lmpsofar before calling hit bit dump. Then this entry will
             * be reported as a MISS. So to report such an entry as possible hit, we are marking the special value.
             * Note that we treat the entry as searched even if lmpsofar was not hit
             */
            if (hb)
            {
                struct kaps_aging_entry *active_aging_table = kaps_device_get_active_aging_table(device, db);

                active_aging_table[hb->bit_no].num_idles = KAPS_HB_SPECIAL_VALUE;
            }
        }

        if (self->m_trie_global->m_isIITLmpsofar && device->hw_res->num_algo_levels > 2 && trienode->m_isRptNode)
        {
            /*
             * Do nothing
             */
        }
        else
        {
            trienode->m_iitLmpsofarPfx_p = NULL;
        }
    }

    /*
     * Figure out the correct poolId 
     */
    actualPoolId = rptParent_p->m_poolId;

    kaps_trie_decr_num_ipt_entries_in_subtrie(rptParent_p, 1);

    rptId = rptParent_p->m_rptId;
    rptDepth = rptParent_p->m_depth;
    kaps_pool_mgr_decr_num_entries(poolMgr, actualPoolId, 1, KAPS_IPT_POOL);

    trienode->m_node_type = NLMNSTRIENODE_REGULAR;
    trienode->m_isReserved160bTrig = 0;

    ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, actualPoolId);

    if (pfxBundle->m_nIndex != KAPS_LSN_NEW_INDEX)
    {
        numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, rptDepth);

        NLM_STRY(kaps_ipm_delete_entry_in_hw
                 (ipm_p, pfxBundle->m_nIndex, rptId, trienode->m_depth - numRptBitsLoppedOff, o_reason));

        NLM_STRY(kaps_ipm_remove_entry
                 (ipm_p, pfxBundle->m_nIndex, rptId, trienode->m_depth - numRptBitsLoppedOff, o_reason));

        if (db->num_algo_levels_in_db == 3)
        {
            struct kaps_db *db = self->m_tbl_ptr->m_db;
            struct kaps_ab_info *ab = ipm_p->m_ab_info;
            NlmReasonCode temp_reason = NLMRSC_REASON_OK;
            uint32_t ads2_nr;

            ads2_nr = 1;
            if (db->is_type_a)
                ads2_nr = 0;

            virtual_it_index = ab->base_addr + pfxBundle->m_nIndex;
            real_it_index = device->kaps_shadow->ads2_overlay[ads2_nr].x_table[virtual_it_index];
            kaps_set_bit(&device->kaps_shadow->ads2_overlay[ads2_nr].it_fbmp, real_it_index);
            device->kaps_shadow->ads2_overlay[ads2_nr].num_free_it_slots++;
            device->kaps_shadow->ads2_overlay[ads2_nr].x_table[virtual_it_index] = -1;

            /*
             * We will try to check if IPT shrink is possible. For Shrink to Kick in 1. RPT split should not be in
             * progress 2. Warmboot should not be in progress 3. This function should be called from
             * kaps_trie_pvt_in_place_delete and not from UpdateHardware 
             */
            if (!self->m_trie_global->m_curRptNodeToSplit_p && !device->issu_in_progress && isMoveToNewPoolAllowed)
            {

                kaps_trie_big_kaps_get_trigger_lengths_into_scratchpad(self, numRptBitsLoppedOff, actualPoolId, 0, 0);

                kaps_trie_find_num_bricks_for_big_kaps_triggers(self);

                /*
                 * If the trigger removal results in the number of bricks to reduce, then shrink the IPT by moving the
                 * triggers to a new pool
                 */
                if (self->m_iptBrickScratchpad.m_numActiveBricks > 0
                    && self->m_iptBrickScratchpad.m_numActiveBricks
                    < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks)
                {

                    /*
                     * coverity[check_return] 
                     */
                    /*
                     * coverity[unchecked_value] 
                     */
                    kaps_trie_move_rpt_to_new_pool(self, rptParent_p, 0, 0, 0, &temp_reason);
                }
            }
        }

        pfxBundle->m_nIndex = KAPS_LSN_NEW_INDEX;

    }
    else
    {
        kaps_assert(0, "Index value of the IPT pfx bundle to be deleted is KAPS_LSN_NEW_INDEX \n");
    }

    return NLMERR_OK;
}

/************************************************
 * Function :
 * kaps_trie_pvt_in_place_delete
 *
 * Parameters:
 *    kaps_lpm_trie *self
 *    kaps_trie_node *node_p
 *
 * Summary:
 * __pvt_InPlaceDelete deletes the specified trie node. If trie node is
 * an LP node then it deletes the SES entry, LSN data corresponding
 * to that trie node and updates the LPMSOFAR value.
 *
 ***********************************************/
static NlmErrNum_t
kaps_trie_pvt_in_place_delete(
    kaps_lpm_trie * self,
    kaps_trie_node * node,
    uint32_t isMoveToNewPoolAllowed,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc *lsn_p;
    kaps_lpm_trie *trie_p = self;
    kaps_trie_node *rptNode = NULL, *newRptParent;
    struct kaps_device *device = self->m_trie_global->fibtblmgr->m_devMgr_p;
    uint32_t isIptEmpty, isAptEmpty;
    kaps_fib_tbl *fibTbl = self->m_tbl_ptr;
    uint8_t dbId = fibTbl->m_tblId;
    kaps_lpm_lpu_brick *curBrick;
    kaps_pfx_bundle *rptLsnLmpsofarPfx;
    struct kaps_db *db = self->m_tbl_ptr->m_db;

    if (node->m_node_type != NLMNSTRIENODE_LP)
        return NLMERR_OK;

    lsn_p = node->m_lsn_p;

    if (self->m_trie_global->m_isLsnLmpsofar && node->m_isRptNode)
    {
        /*
         * Suppose we have LSN Lmpsofar and we are currently processing an RPT node If there are other IPT entries
         * under this RPT node, then we can't delete the IPT entry associated directly with this node even if this IPT
         * entry points to an empty LSN. So simply return.
         * 
         * Only if the RPT node has no IPT/APT entries under it, can we completely delete the IPT entry associated
         * directly with this node and delete the RPT node
         */
        if (node->m_numIptEntriesInSubtrie > 1)
            return NLMERR_OK;

        if (node->m_numReserved160bTrig > 0)
            return NLMERR_OK;
    }

    if (node->m_node_type == NLMNSTRIENODE_LP)
    {
        NLM_STRY(kaps_trie_pvt_delete_ses_in_all_dev(self, node->m_lp_prefix_p, isMoveToNewPoolAllowed, o_reason));

        lsn_p = node->m_lsn_p;
        kaps_lsn_mc_destroy(lsn_p);
        node->m_lsn_p = kaps_trie_node_pvt_recreate_lsn(node);
        node->m_node_type = NLMNSTRIENODE_REGULAR;

        if (self->m_trie_global->m_isIITLmpsofar && device->hw_res->num_algo_levels > 2 && node->m_isRptNode)
        {
            /*
             * Do nothing 
             */
        }
        else
        {
            node->m_iitLmpsofarPfx_p = NULL;
        }

        rptNode = node->m_rptParent_p;
    }

    kaps_trie_node_remove_child_path(node);
    kaps_trie_pvt_cleanup_del_nodes(trie_p);

    if (db->num_algo_levels_in_db == 2)
    {
        return NLMERR_OK;
    }

    isIptEmpty = isAptEmpty = 0;
    if (rptNode->m_numIptEntriesInSubtrie == 0 && rptNode->m_numReserved160bTrig == 0)
    {
        isIptEmpty = 1;
    }

    if (self->m_trie_global->m_isLsnLmpsofar && rptNode->m_numIptEntriesInSubtrie == 1
        && rptNode->m_numReserved160bTrig == 0)
    {
        if (rptNode->m_lsn_p->m_nNumPrefixes == 0)
        {
            isIptEmpty = 1;
        }

        if (rptNode->m_lsn_p->m_nNumPrefixes == 1 && rptNode->m_iitLmpsofarPfx_p)
        {
            isIptEmpty = 1;
        }
    }

    if (rptNode->m_numAptEntriesInSubtrie == 0 ||
        (rptNode->m_numAptEntriesInSubtrie == 1 && rptNode->m_rptAptLmpsofarPfx))
    {
        isAptEmpty = 1;
    }

    if (!isIptEmpty || !isAptEmpty)
        return NLMERR_OK;

    if (self->m_trie_global->m_isLsnLmpsofar && rptNode->m_numIptEntriesInSubtrie == 1)
    {

        curBrick = rptNode->m_lsn_p->m_lpuList;
        while (curBrick)
        {
            if (curBrick->m_hasReservedSlot)
            {
                rptLsnLmpsofarPfx = curBrick->m_pfxes[curBrick->m_maxCapacity - 1];
                if (rptLsnLmpsofarPfx)
                {
                    kaps_pfx_bundle_destroy(rptLsnLmpsofarPfx, trie_p->m_lsn_settings_p->m_pAlloc);
                }

                break;
            }
            curBrick = curBrick->m_next_p;
        }

        NLM_STRY(kaps_trie_pvt_delete_ses_in_all_dev(self, rptNode->m_lp_prefix_p, isMoveToNewPoolAllowed, o_reason));

        lsn_p = rptNode->m_lsn_p;
        kaps_lsn_mc_destroy(lsn_p);
        rptNode->m_lsn_p = kaps_trie_node_pvt_recreate_lsn(rptNode);
        rptNode->m_node_type = NLMNSTRIENODE_REGULAR;
    }

    if (rptNode->m_rptAptLmpsofarPfx)
    {
        rptNode->m_rptAptLmpsofarPfx = NULL;
    }

    kaps_rpm_delete_entry_in_hw(rptNode->m_trie_p->m_trie_global->m_rpm_p, rptNode->m_rpt_prefix_p);
    kaps_rpm_remove_entry(rptNode->m_trie_p->m_trie_global->m_rpm_p, rptNode->m_rpt_prefix_p, dbId);

    kaps_pfx_bundle_destroy(rptNode->m_rpt_prefix_p, self->m_trie_global->m_alloc_p);

    rptNode->m_rpt_prefix_p = NULL;

    kaps_pool_mgr_release_pool_for_rpt_entry(self->m_trie_global->poolMgr, rptNode, rptNode->m_poolId,
                                             rptNode->m_rptId);

    kaps_trie_release_rpt_uuid(rptNode);

    if (rptNode->m_iitLmpsofarPfx_p)
    {
        if (self->m_lsn_settings_p->m_areHitBitsPresent && rptNode->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
        {
            struct kaps_db *db;
            struct kaps_hb *hb = NULL;
            

            KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, rptNode->m_iitLmpsofarPfx_p->m_backPtr, db)
                KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb),
                                        (uintptr_t) rptNode->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

            /*
             * The number of idles is set to 0xffffffff and after incrementing we will get num_idles equal to 0. We do
             * this because, it is possible that Lmpsofar entry was hit and before calling hit bit dump and we are
             * deleting the SES entry corresponding to the Lmpsofar before calling hit bit dump. Then this entry will
             * be reported as a MISS. So to report such an entry as possible hit, we are marking the special value.
             * Note that we treat the entry as searched even if lmpsofar was not hit
             */
            if (hb)
            {
                struct kaps_aging_entry *active_aging_table = kaps_device_get_active_aging_table(device, db);
                
                active_aging_table[hb->bit_no].num_idles = KAPS_HB_SPECIAL_VALUE;
            }
        }
    }

    rptNode->m_rptParent_p = NULL;
    rptNode->m_isRptNode = 0;
    rptNode->m_iitLmpsofarPfx_p = NULL;

    newRptParent = rptNode->m_parent_p;
    while (newRptParent && !newRptParent->m_isRptNode)
    {
        newRptParent = newRptParent->m_parent_p;
    }

    kaps_trie_update_down_stream_rpt_for_rpt_delete(newRptParent, rptNode, o_reason);

    kaps_trie_propagate_rpt_attributes(rptNode, newRptParent);

    kaps_trie_node_remove_child_path(rptNode);
    kaps_trie_pvt_cleanup_del_nodes(trie_p);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_check_rpt_resources_for_new_lsn(
    kaps_trie_node * trienode,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *iterNode_p;
    kaps_trie_global *trieGlobal = trienode->m_trie_p->m_trie_global;
    struct kaps_db *db = trieGlobal->fibtblmgr->m_curFibTbl->m_db;

    /*
     * If we have a new LSN, then we might need to assign a new RPT entry for that LSN. In such as case we have to make 
     * sure that the RPT hardware block has sufficient space to accommodate the new entry
     */

    /*
     * There is no RPT for 2 level device
     */
    if (db->num_algo_levels_in_db == 2)
    {
        return NLMERR_OK;
    }

    iterNode_p = trienode;
    while (iterNode_p->m_depth > KAPS_DEFAULT_RPT_DEPTH && !iterNode_p->m_isRptNode)
    {
        iterNode_p = iterNode_p->m_parent_p;
    }

    if (iterNode_p->m_depth == KAPS_DEFAULT_RPT_DEPTH && !iterNode_p->m_isRptNode)
    {
        /*
         * We will need a new RPT entry. Ensure that the hardware block has space for it
         */
        if (!kaps_rpm_get_num_free_slots(trieGlobal->m_rpm_p))
        {
            *o_reason = NLMRSC_PCM_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        if (db->num_algo_levels_in_db == 3)
        {
            if (kaps_trie_is_big_kaps_it_full(trienode->m_trie_p))
            {
                *o_reason = NLMRSC_IT_ALLOC_FAILED;
                return NLMERR_FAIL;
            }
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_compute_rpt_parent(
    kaps_trie_node * trienode,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_trie_node *iterNode_p = trienode;
    kaps_trie_global *trieGlobal = trienode->m_trie_p->m_trie_global;
    struct kaps_device *device = trieGlobal->fibtblmgr->m_devMgr_p;
    uint32_t depthToCheck;
    kaps_lpm_trie *trie = trienode->m_trie_p;
    uint32_t trigger_length;
    struct kaps_db *db = trie->m_tbl_ptr->m_db;

    if (db->num_algo_levels_in_db == 2)
    {
        /*
         * The two level device doesn't have any real RPT entries. To make the code work generically, the root node of
         * the trie is made as the RPT parent for the IPT node
         */
        kaps_trie_node *root_node = trienode->m_trie_p->m_roots_trienode_p;
        trienode->m_rptParent_p = root_node;
        if (!root_node->m_isRptNode)
        {
            root_node->m_isRptNode = 1;
            root_node->m_rptParent_p = root_node;

            errNum = kaps_trie_pvt_get_new_pool(trieGlobal->poolMgr, root_node, 1, 0, 1, o_reason);

            if (errNum != NLMERR_OK)
            {
                return NLMERR_FAIL;
            }
        }
        return NLMERR_OK;
    }

    depthToCheck = KAPS_DEFAULT_RPT_DEPTH;
    if (device->type == KAPS_DEVICE_KAPS)
    {
        depthToCheck = trienode->m_trie_p->m_min_lopoff;
    }

    while (iterNode_p->m_depth > depthToCheck && !iterNode_p->m_isRptNode)
    {
        if (iterNode_p->m_rptParent_p)
        {
            trienode->m_rptParent_p = iterNode_p->m_rptParent_p;
            return NLMERR_OK;
        }
        iterNode_p = iterNode_p->m_parent_p;
    }

    /*
     * If an RPT entry does not exist then create it 
     */
    if (iterNode_p->m_depth == depthToCheck && !iterNode_p->m_isRptNode)
    {

        if (!kaps_rpm_get_num_free_slots(trieGlobal->m_rpm_p))
        {
            *o_reason = NLMRSC_PCM_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        if (db->num_algo_levels_in_db == 3)
        {
            if (kaps_trie_is_big_kaps_it_full(trie))
            {
                *o_reason = NLMRSC_IT_ALLOC_FAILED;
                return NLMERR_FAIL;
            }
        }

        if (device->type == KAPS_DEVICE_KAPS)
        {
            trieGlobal->poolMgr->m_force_trigger_width = 1;
            trieGlobal->poolMgr->m_cur_max_trigger_length_1 = trienode->m_depth;

            /*
             * Trie node represents the IPT. Iter node represents the new RPT node. So calculate the trigger length
             */
            trigger_length = trienode->m_depth - iterNode_p->m_depth;
            trigger_length += 1;        /* Add 1 for MPE */

            /*
             * Fill up the IPT Brick Scratchpad. No need for memsetting brick scratchpad to 0 first, since we are
             * directly filling up actual values
             */
            trie->m_iptBrickScratchpad.m_gran[0] = trie->m_lsn_settings_p->m_middleLevelLengthToGran[trigger_length];
            trie->m_iptBrickScratchpad.m_numOccupiedEntries[0] = 1;
            trie->m_iptBrickScratchpad.m_numActiveBricks = 1;
        }

        if (trieGlobal->m_isLsnLmpsofar || trieGlobal->m_isIITLmpsofar)
        {
            errNum = kaps_trie_pvt_get_new_pool(trieGlobal->poolMgr, iterNode_p, 1, 0, 1, o_reason);
        }
        else
        {
            errNum = kaps_trie_pvt_get_new_pool(trieGlobal->poolMgr, iterNode_p, 1, 1, 2, o_reason);
        }


        trieGlobal->poolMgr->m_force_trigger_width = 0;
        trieGlobal->poolMgr->m_cur_max_trigger_length_1 = 0;


        if (errNum != NLMERR_OK)
        {
            kaps_trie_release_rpt_uuid(iterNode_p);
            return NLMERR_FAIL;
        }

        errNum = kaps_trie_commit_rpt_and_rit(iterNode_p, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_trie_release_rpt_uuid(iterNode_p);
            return NLMERR_FAIL;
        }

        iterNode_p->m_isRptNode = 1;
        iterNode_p->m_rptParent_p = iterNode_p;

        kaps_trie_propagate_rpt_attributes(iterNode_p->m_child_p[0], iterNode_p);
        kaps_trie_propagate_rpt_attributes(iterNode_p->m_child_p[1], iterNode_p);

        errNum = kaps_trie_update_down_stream_rpt_for_new_rpt(iterNode_p->m_child_p[0], iterNode_p, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;

        errNum = kaps_trie_update_down_stream_rpt_for_new_rpt(iterNode_p->m_child_p[1], iterNode_p, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
    }

    trienode->m_rptParent_p = iterNode_p;

    return errNum;
}

NlmErrNum_t
kaps_trie_add_ses(
    kaps_lpm_trie * self,
    kaps_trie_node * trienode,
    kaps_pfx_bundle * pfxBundle,
    NlmReasonCode * o_reason)
{
    /*
     * Update the trie up to the RPT with the number of IPT nodes present in the sub-trie 
     */
    kaps_trie_node *rptParent_p = NULL;
    uint32_t numIptEntriesInPool = 0, maxAllowedIptEntriesInPool;
    kaps_ipm *ipm_p = NULL;
    uint16_t numRptBitsLoppedOff = 0;
    kaps_pool_mgr *poolMgr = self->m_trie_global->poolMgr;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    uint32_t can_add_rpt_entries_to_split_or_move;
    uint32_t actualPoolId;
    struct kaps_db *db = self->m_tbl_ptr->m_db;

    /*
     * For 2 level KAPS, there is no RPT. For 3 level KAPS, we are not going to add the RPT entry to split/move
     * here. So we are setting can_add_rpt_entries_to_split_or_move to 0. The RPT split will be triggered by
     * issuing a RETRY_WITH_RPT_SPLIT
     */
    can_add_rpt_entries_to_split_or_move = 0;


    rptParent_p = trienode->m_rptParent_p;

    numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, rptParent_p->m_depth);

    trienode->m_node_type = NLMNSTRIENODE_LP;

    trienode->m_poolId = rptParent_p->m_poolId;

    actualPoolId = rptParent_p->m_poolId;

    kaps_trie_incr_num_ipt_entries_in_subtrie(rptParent_p, 1);

    /*
     * Increment the number of IPT entries in the pool 
     */
    kaps_pool_mgr_incr_num_entries(poolMgr, actualPoolId, 1, KAPS_IPT_POOL);

    /*
     * If we are running out the pool, then indicate that we have to perform a split in the root 
     */
    numIptEntriesInPool = kaps_pool_mgr_get_num_entries(poolMgr, rptParent_p->m_poolId, KAPS_IPT_POOL);
    maxAllowedIptEntriesInPool = kaps_pool_mgr_get_max_allowed_entries(poolMgr, rptParent_p->m_poolId, KAPS_IPT_POOL);

    if (rptParent_p->m_numIptEntriesInSubtrie >= rptParent_p->m_trie_p->m_maxAllowedIptEntriesForRpt
        && can_add_rpt_entries_to_split_or_move)
    {
        kaps_trie_pvt_add_rpt_nodes_to_split_or_move(self, rptParent_p);
        can_add_rpt_entries_to_split_or_move = 0;
    }

    if (self->m_trie_global->m_areAllPoolsHealthy
        && numIptEntriesInPool >= maxAllowedIptEntriesInPool && can_add_rpt_entries_to_split_or_move)
    {
        kaps_trie_pvt_add_rpt_nodes_to_split_or_move(self,
                                                     kaps_pool_mgr_get_rpt_with_most_entries(poolMgr,
                                                                                             rptParent_p->m_poolId,
                                                                                             KAPS_IPT_POOL));
        can_add_rpt_entries_to_split_or_move = 0;
    }

    ipm_p = kaps_pool_mgr_get_ipm_for_pool(poolMgr, actualPoolId);

    NLM_STRY(kaps_ipm_add_entry(ipm_p, pfxBundle, rptParent_p->m_rptId,
                                trienode->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason));

    if (db->num_algo_levels_in_db == 3)
    {
        /*
         * Do nothing. No need to delete since the triggers are in SRAM and we don't expect coherency issues
         */
    }
    else
    {
        NLM_STRY(kaps_ipm_delete_entry_in_hw(ipm_p, pfxBundle->m_nIndex, rptParent_p->m_rptId,
                                             trienode->m_depth - numRptBitsLoppedOff, o_reason));
    }

    NLM_STRY(kaps_lsn_mc_commitIIT(trienode->m_lsn_p, pfxBundle->m_nIndex, o_reason));

    NLM_STRY(kaps_ipm_write_entry_to_hw(ipm_p, pfxBundle, rptParent_p->m_rptId,
                                        trienode->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason));

    return NLMERR_OK;
}

void
kaps_trie_process_merge_after_giveout(
    kaps_lpm_trie * self)
{
    uint32_t loopvar, j;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    uint32_t numTrieNodesForMerge = self->m_trie_global->m_numTrieNodesForMerge;

    for (loopvar = 0; loopvar < numTrieNodesForMerge; loopvar++)
    {
        kaps_trie_node *trieNode1, *trieNode2, *newTrieNode;

        if (self->m_lsn_settings_p->m_isRelatedLsnMergeEnabled)
        {

            trieNode1 = self->m_trie_global->m_trieNodesForRelatedMerge[loopvar];

            if (trieNode1 && kaps_trie_check_if_related_ipt_nodes_can_be_merged(trieNode1, &trieNode2, &newTrieNode))
            {

                /*
                 * Merge the related LSNs
                 */
                kaps_trie_merge_related_lsns(self, trieNode1, trieNode2, newTrieNode, &reason);

                /*
                 * We may decide to merge trieNode1 with another of the trienodes that we just created during the
                 * Giveout. In this case, we have to remove it from the array m_trieNodesForRelatedMerge
                 */
                for (j = 0; j < numTrieNodesForMerge; ++j)
                {
                    if (self->m_trie_global->m_trieNodesForRelatedMerge[j] == trieNode2)
                    {
                        self->m_trie_global->m_trieNodesForRelatedMerge[j] = NULL;
                    }
                }
            }
        }

        self->m_trie_global->m_trieNodesForRelatedMerge[loopvar] = NULL;
    }

    self->m_trie_global->m_numTrieNodesForMerge = 0;
}

static NlmErrNum_t
kaps_trie_update_hardware(
    kaps_lpm_trie * self,
    struct kaps_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc *lsn_p = NULL;
    kaps_pfx_bundle *bundle = NULL;
    uint32_t max = 0, loopvar = 0;
    kaps_pfx_bundle *pfxBundle = NULL;
    uint8_t *tempPtr = NULL;
    kaps_trie_node *trienode = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_trie_node *delayedNode;
    kaps_lsn_mc *delayedLsn;
    uint32_t numTrieNodesForMerge;

    max = self->m_num_items_in_to_add_ses;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        pfxBundle = self->m_toadd_ses[loopvar];
        tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle);
        kaps_memcpy(&trienode, tempPtr, sizeof(kaps_trie_node *));

        if (!trienode->m_rptParent_p)
        {
            errNum = kaps_trie_compute_rpt_parent(trienode, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;
        }
    }

    /*
     * Commit LSNs into hardware 
     */
    max = self->m_num_items_in_to_update_lsns;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        lsn_p = self->m_toupdate_lsns[loopvar];
        NLM_STRY(kaps_lsn_mc_commit(lsn_p, 0, o_reason));
        lsn_p->m_bIsNewLsn = 0;
    }

    self->m_num_items_in_to_update_lsns = 0;

    /*
     * Commit SES entries 
     */
    max = self->m_num_items_in_to_add_ses;
    numTrieNodesForMerge = 0;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        pfxBundle = self->m_toadd_ses[loopvar];

        tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle);
        kaps_memcpy(&trienode, tempPtr, sizeof(kaps_trie_node *));

        NLM_STRY(kaps_trie_add_ses(self, trienode, pfxBundle, o_reason));

        if (numTrieNodesForMerge < KAPS_MAX_NUM_TRIE_NODES_FOR_RELATED_MERGE)
        {
            self->m_trie_global->m_trieNodesForRelatedMerge[numTrieNodesForMerge] = trienode;
            numTrieNodesForMerge++;
        }

    }
    self->m_num_items_in_to_add_ses = 0;
    self->m_trie_global->m_numTrieNodesForMerge = numTrieNodesForMerge;

    /*
     * Update the delayed LSN assignment to the trie node
     */
    max = self->m_num_items_in_delayed_trie_node;

    for (loopvar = 0; loopvar < max; loopvar++)
    {
        delayedNode = self->m_delayedTrieNode[loopvar];
        delayedLsn = self->m_delayedLsn[loopvar];
        delayedNode->m_lsn_p = delayedLsn;
    }
    self->m_num_items_in_delayed_trie_node = 0;
    self->m_num_items_in_delayed_lsn = 0;

    /*
     * Update the SITs 
     */
    NLM_STRY(kaps_trie_commit_iits(self, o_reason));
    self->m_num_items_in_to_update_sit = 0;

    /*
     * Delete SES entries 
     */
    max = self->m_num_items_in_to_delete_ses;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        bundle = self->m_todelete_ses[loopvar];

        tempPtr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(bundle);
        kaps_memcpy(&trienode, tempPtr, sizeof(kaps_trie_node *));

        /*
         * Delete SES for all dev. Pass 0 to indicate that we should not try to Shrink the IPT triggers
         */
        NLM_STRY(kaps_trie_pvt_delete_ses_in_all_dev(self, bundle, 0, o_reason));

        trienode->m_lsn_p = kaps_lsn_mc_create(self->m_lsn_settings_p, self, trienode->m_depth);
        trienode->m_lsn_p->m_pParentHandle = trienode->m_lp_prefix_p;
    }
    self->m_num_items_in_to_delete_ses = 0;

    /*
     * now remove all the to be deleted LSNs and the backup node list 
     */
    max = self->m_num_items_in_to_delete_lsns;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        lsn_p = self->m_todelete_lsns[loopvar];
        kaps_lsn_mc_destroy(lsn_p);
    }
    self->m_num_items_in_to_delete_lsns = 0;

    return NLMERR_OK;
}

/************************************************
 * Function :
 * kaps_trie_locate_lpm
 *
 * Parameters:
 *    kaps_lpm_trie *self,
 *    kaps_prefix *prefix,
 *    NlmReasonCode *o_reason
 *
 * Summary:
 * __LocateLPM is called to locate longest prefix match with lowest meta priority for the given prefix
 * It returns The pfx bundle of the longest match prefix with lowest meta priority.
 * If it returns NULL, then there is no match for the given prefix data.
 *
 ***********************************************/
kaps_pfx_bundle *
kaps_trie_locate_lpm(
    kaps_lpm_trie * self,
    uint8_t * pfxData,
    uint32_t pfxLen,
    uint32_t * o_lpmHitIndex_p,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *node_p = NULL;
    kaps_pfx_bundle *lsnPfxbundle_p = NULL, *high_lsnPfxbundle_p = NULL;
    kaps_pfx_bundle *pfxbundle_p = NULL;
    kaps_trie_node *lastseen_node_p = NULL;
    kaps_trie_node *nearestLpNode_p = NULL;
    kaps_fib_tbl *fibTbl = self->m_tbl_ptr;
    struct NlmFibStats *fibStats = &fibTbl->m_fibStats;

    uint32_t matchBrickIter = 0, matchPosInBrick = 0;

    (void) o_reason;

    nearestLpNode_p = kaps_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData, pfxLen, &lastseen_node_p, 0);

    node_p = nearestLpNode_p;

    while (node_p)
    {
        if (node_p->m_node_type == NLMNSTRIENODE_LP)
        {
            lsnPfxbundle_p = kaps_lsn_mc_locate_lpm(node_p->m_lsn_p, pfxData, pfxLen, NULL, &matchBrickIter,
                                                    &matchPosInBrick);

            if (lsnPfxbundle_p
                && ((!high_lsnPfxbundle_p)
                    || lsnPfxbundle_p->m_backPtr->meta_priority < high_lsnPfxbundle_p->m_backPtr->meta_priority))
            {
                high_lsnPfxbundle_p = lsnPfxbundle_p;
            }

            if (self->m_trie_global->m_isIITLmpsofar)
            {
                if (high_lsnPfxbundle_p)
                    break;
                high_lsnPfxbundle_p = node_p->m_iitLmpsofarPfx_p;

                if (node_p->m_iitLmpsofarPfx_p)
                {
                    fibStats->numLmpsofarHitInSearch++;
                }
                break;
            }
        }

        node_p = node_p->m_parent_p;
    }

    nearestLpNode_p = kaps_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData, pfxLen, &lastseen_node_p, 1);

    node_p = NULL;
    if (nearestLpNode_p)
        node_p = nearestLpNode_p->m_rptParent_p;

    pfxbundle_p = high_lsnPfxbundle_p;

    if (!pfxbundle_p)
    {
        *o_lpmHitIndex_p = KAPS_FIB_INVALID_INDEX;

        fibStats->numMissesInSearch++;

    }
    else
    {
        *o_lpmHitIndex_p = pfxbundle_p->m_nIndex;
    }

    if (pfxbundle_p && pfxbundle_p->m_isPfxCopy)
        fibStats->numLmpsofarHitInSearch++;

    return pfxbundle_p;
}

/************************************************
 * Function :
 * kaps_trie_locate_exact
 *
 *
 * Summary:
 * __LocateExact is called to locate exact match for the given prefix
 * It returns The pfx bundle of the exact match prefix.
 * If it returns NULL, then there is no match prefix in ftm table.
 *
 ***********************************************/
uint32_t
kaps_trie_locate_exact(
    kaps_lpm_trie * self,
    uint8_t * pfxData_p,
    uint16_t inUse,
    kaps_pfx_bundle ** o_pfxBundle_pp,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *node_p = NULL;
    kaps_pfx_bundle *pfxBundle = NULL, *pfxBundleInHash = NULL;
    kaps_pfx_bundle *aptPfxBundle = NULL;
    struct kaps_lpm_entry **entrySlotInHash = NULL;
    kaps_trie_node *lastseen_node_p = NULL;
    uint32_t locateExactIndex = 0;
    struct kaps_entry **ht_slot;
    struct kaps_lpm_entry *entry;
    kaps_status status;
    uint32_t match_brick_num = 0;

    (void) o_reason;
    (void) aptPfxBundle;

    node_p = kaps_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData_p, inUse, &lastseen_node_p, 0);
    (void) node_p;

    if (!pfxBundle)
    {
        pfxBundle = kaps_lsn_mc_locate_exact(node_p->m_lsn_p, pfxData_p, inUse, &match_brick_num);
    }

    node_p = kaps_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData_p, inUse, &lastseen_node_p, 1);
    (void) node_p;

    if (self->m_hashtable_p)
    {
        status = kaps_pfx_hash_table_locate(self->m_hashtable_p, pfxData_p, inUse, &ht_slot);
        if (status != KAPS_OK)
        {
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        entrySlotInHash = (struct kaps_lpm_entry **) ht_slot;
        if (entrySlotInHash)
            pfxBundleInHash = (*entrySlotInHash)->pfx_bundle;
    }

    if (pfxBundle != pfxBundleInHash)
    {
        kaps_assert(0, "Prefix bundle in data structures does not match the hash prefix bundle \n");
    }

    if (!pfxBundle)
    {
        locateExactIndex = KAPS_FIB_INVALID_INDEX;
    }
    else
    {
        locateExactIndex = pfxBundle->m_nIndex;
    }

    if (pfxBundle)
    {
        entry = pfxBundle->m_backPtr;

        if (entry->ad_handle)
        {
            struct kaps_device *device = self->m_trie_global->fibtblmgr->m_devMgr_p;
            struct kaps_ad_db *ad_db;
            enum kaps_ad_type ad_type;

            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db)
                ad_type = kaps_resource_get_ad_type(&ad_db->db_info);

            if (ad_db->user_width_1)
            {
                if (ad_type == KAPS_AD_TYPE_INPLACE)
                {
                    locateExactIndex = entry->ad_handle->value[0] << 16;
                    locateExactIndex |= entry->ad_handle->value[1] << 8;
                    locateExactIndex |= entry->ad_handle->value[2];
                }
            }
        }
    }

    if (o_pfxBundle_pp)
    {
        *o_pfxBundle_pp = pfxBundle;
    }

    return locateExactIndex;
}

NlmErrNum_t
kaps_trie_get_prefix_location(
    kaps_lpm_trie * trie_p,
    struct kaps_lpm_entry * lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row)
{
    uint32_t match_brick_num = 0;
    kaps_trie_node *node_p, *lastseen_node_p = NULL;
    kaps_pfx_bundle *pfxBundle = NULL;

    node_p = kaps_trie_find_dest_lp_node(trie_p, trie_p->m_roots_trienode_p, lpm_entry->pfx_bundle->m_data,
                                         lpm_entry->pfx_bundle->m_nPfxSize, &lastseen_node_p, 0);

    pfxBundle =
        kaps_lsn_mc_locate_exact(node_p->m_lsn_p, lpm_entry->pfx_bundle->m_data, lpm_entry->pfx_bundle->m_nPfxSize,
                                 &match_brick_num);

    if (pfxBundle)
    {
        kaps_uda_mgr_compute_abs_brick_udc_and_row(trie_p->m_trie_global->m_mlpmemmgr[0], node_p->m_lsn_p->m_mlpMemInfo,
                                                   match_brick_num, abs_udc, abs_row);
    }

    return NLMERR_OK;
}

kaps_trie_node *
kaps_trie_pvt_find_suitable_new_rpt_node(
    kaps_lpm_trie * trie_p)
{
    kaps_trie_node *curTrieNode_p = NULL, *newRptNode_p = NULL;
    kaps_trie_global *trieGlobal_p = trie_p->m_trie_global;
    int32_t done = 0;
    uint32_t lowerThreshold = 0, upperThreshold = 0;
    uint32_t numEntriesInCurrNode = 0;
    kaps_trie_node *rptNodeToSplit = trieGlobal_p->m_curRptNodeToSplit_p;
    uint32_t i, depthDiffBetweenRptNodes;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;

    curTrieNode_p = trieGlobal_p->m_curRptNodeToSplit_p;

	lowerThreshold = (trieGlobal_p->m_curRptNodeToSplit_p->m_numIptEntriesInSubtrie) / 3;
	upperThreshold = (trieGlobal_p->m_curRptNodeToSplit_p->m_numIptEntriesInSubtrie * 2) / 3;

    /*
     * Traverse the trie to find the deeper node that has about half the entries 
     */
    while (!done)
    {
        if (curTrieNode_p->m_child_p[0] && !curTrieNode_p->m_child_p[0]->m_isRptNode)
        {
            numEntriesInCurrNode = curTrieNode_p->m_child_p[0]->m_numIptEntriesInSubtrie;

            if (numEntriesInCurrNode >= lowerThreshold && numEntriesInCurrNode <= upperThreshold)
            {
                newRptNode_p = curTrieNode_p->m_child_p[0];
                done = 1;
                break;
            }
            else if (numEntriesInCurrNode > upperThreshold)
            {
                curTrieNode_p = curTrieNode_p->m_child_p[0];
                continue;
            }
        }

        if (curTrieNode_p->m_child_p[1] && !curTrieNode_p->m_child_p[1]->m_isRptNode)
        {
            numEntriesInCurrNode = curTrieNode_p->m_child_p[1]->m_numIptEntriesInSubtrie;

            if (numEntriesInCurrNode >= lowerThreshold && numEntriesInCurrNode <= upperThreshold)
            {
                newRptNode_p = curTrieNode_p->m_child_p[1];
                done = 1;
                break;
            }
            else if (numEntriesInCurrNode > upperThreshold)
            {
                curTrieNode_p = curTrieNode_p->m_child_p[1];
                continue;
            }
        }

        return NULL;
    }

    /*
     * For JR2 KAPS perform an asymmetric RPT split. One prefix pattern where this is needed is if there is /0 followed 
     * by sequential IPv6 pattern. We will always keep splitting the RPT node at depth 0. Since each RPT in JR2 KAPS
     * can't hold many entries, the RPT node at depth 0 will continuously split and we will run out of RPT entries. So
     * if the RPT node has had too many splits and depth difference between new RPT node and RPT node being split is
     * big, then change the new RPT node to a node higher up in the trie even if we can't guarantee 1/3 - 2/3
     * partitioning 
     */
    if (db->num_algo_levels_in_db == 3)
    {
        depthDiffBetweenRptNodes = newRptNode_p->m_depth - rptNodeToSplit->m_depth;

        if (rptNodeToSplit->m_numRptSplits >= 100 && depthDiffBetweenRptNodes > 30)
        {

            i = 0;

            /*
             * Pick a new RPT node which is halfway up the distance from the rpt node being split
             */
            while (i < depthDiffBetweenRptNodes / 2)
            {
                if (newRptNode_p->m_parent_p->m_numIptEntriesInSubtrie < rptNodeToSplit->m_numIptEntriesInSubtrie)
                {
                    newRptNode_p = newRptNode_p->m_parent_p;
                }
                else
                {
                    /*
                     * We can't choose a new RPT node that has the same number of triggers as the RPT node being split. 
                     * So stop at this node
                     */
                    break;
                }

                ++i;
            }
        }
    }

    return newRptNode_p;
}

NlmErrNum_t
kaps_trie_split_rpt_node(
    kaps_lpm_trie * trie_p,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *newRptNode_p = NULL;
    uint32_t oldPoolId = 0;
    uint32_t oldNumIptEntriesBeforeSplit = 0, oldNumAptEntriesBeforeSplit = 0, oldNumReserved160bTrigBeforeSplit = 0;
    NlmErrNum_t errNum = NLMERR_OK;
    uint16_t numAncestorIptPfxToMove = 0;
    kaps_lsn_mc *ancestorIptLsn_p = NULL;
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    uint32_t *indexBuffer_p = trie_global->m_indexBuffer_p;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t numPools, oldNumRptSplitsInThisNode;
    uint32_t numExtraIpt, numExtraApt;
    uint32_t wasLsnForRptLmpsofarAdded = 0;
    struct kaps_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    uint32_t wasNewTriggerCreatedForNewRpt;
    uint32_t numOldRegularTriggersThatMovedOut;

    numPools = 2;
    if (!trie_p->m_trie_global->m_isAptPresent)
        numPools = 1;

    if (trie_global->m_curRptNodeToSplit_p)
    {
        oldPoolId = trie_global->m_curRptNodeToSplit_p->m_poolId;

        oldNumIptEntriesBeforeSplit = trie_global->m_curRptNodeToSplit_p->m_numIptEntriesInSubtrie;
        oldNumAptEntriesBeforeSplit = trie_global->m_curRptNodeToSplit_p->m_numAptEntriesInSubtrie;
        oldNumReserved160bTrigBeforeSplit = trie_global->m_curRptNodeToSplit_p->m_numReserved160bTrig;

        trie_global->m_rptNodeForCalcIptApt = trie_global->m_curRptNodeToSplit_p;
        trie_global->useTrigLenForCalcIptApt = 0;
        kaps_trie_pvt_calc_num_ipt_and_apt(trie_global->m_curRptNodeToSplit_p, NULL);

        kaps_assert(oldNumIptEntriesBeforeSplit == trie_global->m_curRptNodeToSplit_p->m_numIptEntriesInSubtrie,
                    "Mismatch in number of IPT entries in the subtrie \n");

        kaps_assert(oldNumAptEntriesBeforeSplit == trie_global->m_curRptNodeToSplit_p->m_numAptEntriesInSubtrie,
                    "Mismatch in number of APT entries in the subtrie \n");

        newRptNode_p = kaps_trie_pvt_find_suitable_new_rpt_node(trie_p);

        if (!newRptNode_p)
        {
            trie_p->m_alloc_failed = 1;
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        kaps_trie_check_split_nearest_ipt_ancestor(newRptNode_p, &ancestorIptLsn_p, trie_global->m_pfxLocationsToMove,
                                                   &numAncestorIptPfxToMove, o_reason);

        numExtraIpt = 0;
        wasNewTriggerCreatedForNewRpt = 0;
        if (numAncestorIptPfxToMove || trie_global->m_isLsnLmpsofar)
        {
            if (newRptNode_p->m_node_type == 0)
            {
                numExtraIpt = 1;
                wasNewTriggerCreatedForNewRpt = 1;
            }

            if (numAncestorIptPfxToMove)
            {
                kaps_lsn_mc_store_old_lsn_info(ancestorIptLsn_p, 0);
            }
        }

        /*
         * We need an extra APT entry to store the RPT lmpsofar
         */
        numExtraApt = 0;
        if (trie_global->m_isAptPresent)
            numExtraApt = 1;

        trie_global->m_move_reserved_160b_trig_to_regular_pool = 0;

        /*
         * Assign a new pool to the RPT entries. It shouldn't be the same as the old pool. Right now it should be ok to 
         * just assign a new pool and not check that it is not the old pool since we won't be having space in the old
         * pool anyway 
         */
        {
            uint32_t longest_apt_entry_size = 0, longest_ipt_entry_size = 0, longest_size = 0;

            /*
             * Reset the IPT brick scratchpad before calling the recursive function
             * kaps_trie_pvt_calc_longest_size_for_ipt_and_apt
             */
            kaps_memset(&trie_p->m_iptBrickScratchpad, 0, sizeof(kaps_ipt_brick_scratchpad));

            kaps_trie_pvt_calc_longest_size_for_ipt_and_apt(newRptNode_p, newRptNode_p,
                                                            &longest_apt_entry_size, &longest_ipt_entry_size);
            if (longest_ipt_entry_size > longest_apt_entry_size)
                longest_size = longest_ipt_entry_size;
            else
                longest_size = longest_apt_entry_size;

            poolMgr->m_force_trigger_width = 1;

            poolMgr->m_cur_max_trigger_length_1 =
                longest_size - kaps_trie_get_num_rpt_bits_lopped_off(device, newRptNode_p->m_depth);

            poolMgr->m_cur_max_trigger_length_1 += 1;       /* Add 1 for MPE */

        }

        
        {
            if (numExtraIpt)
            {
                /*
                 * numExtraIpt can be 1 if we have to Split the Nearest IPT Ancestor. So add the new IPT entry into the 
                 * brick scratchpad
                 */
                uint32_t newIptTriggerLen =
                    newRptNode_p->m_depth - kaps_trie_get_num_rpt_bits_lopped_off(device, newRptNode_p->m_depth);
                trie_p->m_iptBrickScratchpad.m_prefixLens[trie_p->m_iptBrickScratchpad.m_numPfx] = newIptTriggerLen;
                trie_p->m_iptBrickScratchpad.m_numPfx++;
            }

            /*
             * Calculate how many bricks we need for the triggers stored in the IPT Scratchpad
             */
            kaps_trie_find_num_bricks_for_big_kaps_triggers(trie_p);
        }

        errNum = kaps_trie_pvt_get_new_pool(poolMgr, newRptNode_p,
                                            newRptNode_p->m_numIptEntriesInSubtrie + numExtraIpt,
                                            newRptNode_p->m_numAptEntriesInSubtrie + numExtraApt, numPools, o_reason);

        poolMgr->m_force_trigger_width = 0;

        if (errNum != NLMERR_OK)
        {
            trie_p->m_alloc_failed = 1;
            kaps_trie_release_rpt_uuid(newRptNode_p);
            return errNum;
        }

        trie_global->m_isNearestIptAncestorSplit = 0;

        if (numAncestorIptPfxToMove)
        {
            kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(ancestorIptLsn_p);

            /*
             * Split the nearest IPT node that is an ancestor to the new RPT node if needed 
             */
            errNum =
                kaps_trie_split_nearest_ipt_ancestor(newRptNode_p, ancestorIptLsn_p, trie_global->m_pfxLocationsToMove,
                                                     numAncestorIptPfxToMove, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_pool_mgr_release_pool_for_rpt_entry(poolMgr, newRptNode_p, newRptNode_p->m_poolId,
                                                         newRptNode_p->m_rptId);
                kaps_trie_release_rpt_uuid(newRptNode_p);
                newRptNode_p->m_poolId = 0;
                newRptNode_p->m_rptId = 0;
                trie_p->m_alloc_failed = 1;
                return errNum;
            }
        }
        else if (trie_global->m_isIITLmpsofar)
        {
            kaps_trie_ipt_ancestor_iit_lmpsofar(newRptNode_p, ancestorIptLsn_p);
        }


        /*
         * Before calling Redistribute prefixes, make isRptNode false, so that we don't skip the RPT entry and its
         * subtrie while redistributing the IPT entries. This is needed since in some cases the RPT entry can also be
         * an IPT entry ? After calling the redistribute function, make isRptNode true 
         */

        newRptNode_p->m_isRptNode = 0;

        trie_global->m_oldPoolId = oldPoolId;
        trie_global->m_oldRptId = trie_global->m_curRptNodeToSplit_p->m_rptId;
        trie_global->m_old_rpt_uuid = trie_global->m_curRptNodeToSplit_p->m_rpt_uuid;
        trie_global->m_oldRptDepth = trie_global->m_curRptNodeToSplit_p->m_depth;

        trie_global->m_newPoolId = newRptNode_p->m_poolId;
        trie_global->m_newRptId = newRptNode_p->m_rptId;
        trie_global->m_new_rpt_uuid = newRptNode_p->m_rpt_uuid;
        trie_global->m_newRptDepth = newRptNode_p->m_depth;

        trie_global->m_newRptNode_p = newRptNode_p;
        trie_global->m_deleteRptAptLmpsofar = 0;
        trie_global->m_indexBufferIter = 0;
        kaps_memset(indexBuffer_p, 0, NLMNS_MAX_INDEX_BUFFER_SIZE * 4);

        trie_global->m_rptOp = KAPS_RPT_SPLIT;

        trie_global->m_numReserved160bThatBecameRegular = 0;
        trie_global->m_numRegularThatBecameReserved160b = 0;

        trie_p->m_isCopyIptAndAptToNewPoolInProgress = 1;

        if (trie_global->m_isNearestIptAncestorSplit || wasLsnForRptLmpsofarAdded)
        {
            /*
             * If we added a new LSN to the newRptNode, then if we directly call kaps_trie_copy_ipt_and_apt_to_new_pool 
             * with the newRptNode, then we will attempt to copy the new LSN IPT from old pool to new pool, but the IPT 
             * was never present in the old pool. To overcome this problem we are first processing the newRptNode and
             * then calling kaps_trie_copy_ipt_and_apt_to_new_pool on its children
             */

            errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(newRptNode_p->m_child_p[0], o_reason);
            if (errNum != NLMERR_OK)
            {
                trie_p->m_isCopyIptAndAptToNewPoolInProgress = 0;
                return errNum;
            }

            errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(newRptNode_p->m_child_p[1], o_reason);
            if (errNum != NLMERR_OK)
            {
                trie_p->m_isCopyIptAndAptToNewPoolInProgress = 0;
                return errNum;
            }

        }
        else
        {
            errNum = kaps_trie_copy_ipt_and_apt_to_new_pool(newRptNode_p, o_reason);
            if (errNum != NLMERR_OK)
            {
                trie_p->m_isCopyIptAndAptToNewPoolInProgress = 0;
                return errNum;
            }
        }

        trie_p->m_isCopyIptAndAptToNewPoolInProgress = 0;

        if (trie_global->m_move_reserved_160b_trig_to_regular_pool)
        {
            kaps_trie_incr_num_ipt_entries_in_subtrie(newRptNode_p, newRptNode_p->m_numReserved160bTrig);
        }

        numOldRegularTriggersThatMovedOut =
            newRptNode_p->m_numIptEntriesInSubtrie - trie_global->m_numReserved160bThatBecameRegular +
            trie_global->m_numRegularThatBecameReserved160b;

        /*
         * Decrement the number of IPT entries in the node to split 
         */
        kaps_trie_decr_num_ipt_entries_in_subtrie(trie_global->m_curRptNodeToSplit_p,
                                                  numOldRegularTriggersThatMovedOut);

        /*
         * Decrease the count of IPT entries in the pool manager in the old pool. 
         */
        kaps_pool_mgr_decr_num_entries(poolMgr, oldPoolId, numOldRegularTriggersThatMovedOut, KAPS_IPT_POOL);

        /*
         * Increase the number of IPT entries in the new pool manager 
         */
        kaps_pool_mgr_incr_num_entries(poolMgr, newRptNode_p->m_poolId, newRptNode_p->m_numIptEntriesInSubtrie,
                                       KAPS_IPT_POOL);

        if (trie_global->m_isNearestIptAncestorSplit || wasLsnForRptLmpsofarAdded)
        {
            kaps_trie_incr_num_ipt_entries_in_subtrie(newRptNode_p, 1);
            kaps_pool_mgr_incr_num_entries(poolMgr, newRptNode_p->m_poolId, 1, KAPS_IPT_POOL);
        }

        kaps_assert(oldNumIptEntriesBeforeSplit + oldNumReserved160bTrigBeforeSplit + wasNewTriggerCreatedForNewRpt
                    ==
                    trie_global->m_curRptNodeToSplit_p->m_numIptEntriesInSubtrie +
                    trie_global->m_curRptNodeToSplit_p->m_numReserved160bTrig + newRptNode_p->m_numIptEntriesInSubtrie +
                    newRptNode_p->m_numReserved160bTrig, "Incorrect number of entries in Trie after RPT split \n");

        trie_global->m_move_reserved_160b_trig_to_regular_pool = 0;
        trie_global->m_numReserved160bThatBecameRegular = 0;

        /*
         * Add the newRptNode to the downstream RPT node list of the current RPT node to split
         */
        errNum =
            kaps_trie_update_down_stream_rpt_during_rpt_split(trie_global->m_curRptNodeToSplit_p, newRptNode_p,
                                                              o_reason);
        if (errNum != NLMERR_OK)
            return errNum;

        errNum = kaps_trie_commit_rpt_and_rit(newRptNode_p, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;

        trie_global->m_indexBufferIter = 0;

        if (trie_global->m_isNearestIptAncestorSplit || wasLsnForRptLmpsofarAdded)
        {

            if (trie_global->m_isNearestIptAncestorSplit)
            {
                kaps_trie_node *ancestorIptNode = NULL;
                void *tmp_ptr;

                tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(ancestorIptLsn_p->m_pParentHandle);
                kaps_memcpy(&ancestorIptNode, tmp_ptr, sizeof(kaps_trie_node *));

                NLM_STRY(kaps_trie_clear_holes_in_split_ipt_ancestor
                         (trie_p, ancestorIptNode, newRptNode_p, trie_global->m_pfxLocationsToMove,
                          numAncestorIptPfxToMove, o_reason));
            }

            kaps_trie_delete_ipt_and_apt_in_old_pool(newRptNode_p->m_child_p[0], o_reason);
            kaps_trie_delete_ipt_and_apt_in_old_pool(newRptNode_p->m_child_p[1], o_reason);
        }
        else
        {
            kaps_trie_delete_ipt_and_apt_in_old_pool(newRptNode_p, o_reason);
        }

        newRptNode_p->m_isRptNode = 1;

        newRptNode_p->m_rptParent_p = newRptNode_p;

        /*
         * There are limited number of bits for storing the number of RPT splits and we don't want to overflow
         */
        oldNumRptSplitsInThisNode = trie_global->m_curRptNodeToSplit_p->m_numRptSplits;
        trie_global->m_curRptNodeToSplit_p->m_numRptSplits++;
        if (trie_global->m_curRptNodeToSplit_p->m_numRptSplits == 0)
        {
            trie_global->m_curRptNodeToSplit_p->m_numRptSplits = oldNumRptSplitsInThisNode;
        }
    }

    return NLMERR_OK;
}

/*IPT Split will form new triggers. Check if we can store the new triggers within the existing pool allocated for the IPT entries. Use the IPT brick
scratchpad for calculating*/
uint32_t
kaps_trie_check_if_big_kaps_triggers_fit_during_ipt_split(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * originalLsnTrienode_p,
    uint32_t originalPoolId)
{
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    kaps_ipm *ipm = kaps_pool_mgr_get_ipm_for_pool(poolMgr, originalPoolId);
    struct kaps_ab_info *ab = ipm->m_ab_info;
    struct kaps_device *device = trie_p->m_trie_global->fibtblmgr->m_devMgr_p;
    uint32_t cur_brick_nr, i, j, num_bricks, maxNumEntriesPerBrick;
    uint32_t gran, curNumEntriesInBrick;
    uint32_t trigger_length_1, numRptBitsLoppedOff;
    kaps_flat_lsn_data *curGiveoutData_p;
    uint32_t found;

    numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, originalLsnTrienode_p->m_rptParent_p->m_depth);

    /*
     * Reinitialize the IPT Brick Scratchpad
     */
    kaps_memset(&trie_p->m_iptBrickScratchpad, 0, sizeof(kaps_ipt_brick_scratchpad));

    num_bricks = device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks;

    /*
     * Fill up the IPT Brick Scratchpad by looking at the ab_to_small_bb information
     */
    for (cur_brick_nr = 0; cur_brick_nr < num_bricks; ++cur_brick_nr)
    {
        gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].sub_ab_gran;

        maxNumEntriesPerBrick = (KAPS_BKT_WIDTH_1 / (gran + KAPS_AD_WIDTH_1));

        if (maxNumEntriesPerBrick > KAPS_MAX_PFX_PER_BKT_ROW)
            maxNumEntriesPerBrick = KAPS_MAX_PFX_PER_BKT_ROW;

        /*
         * Find out how many entries are present in the small BB brick by looking at the free slot bmp
         */
        curNumEntriesInBrick = 0;
        for (j = 0; j < maxNumEntriesPerBrick; ++j)
        {
            if (kaps_array_check_bit
                (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].free_slot_bmp, j))
            {
                /*
                 * It is a free slot. Do nothing
                 */
            }
            else
            {
                /*
                 * It is an occupied slot
                 */
                curNumEntriesInBrick++;
            }
        }

        trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr] = curNumEntriesInBrick;
        trie_p->m_iptBrickScratchpad.m_maxNumEntries[cur_brick_nr] = maxNumEntriesPerBrick;
        trie_p->m_iptBrickScratchpad.m_gran[cur_brick_nr] = gran;
    }

    /*
     * Now try to fit the new triggers into the brick scratchpad
     */
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        trigger_length_1 = curGiveoutData_p->m_commonPfx->m_inuse - numRptBitsLoppedOff;
        trigger_length_1 += 1;  /* Adding 1 for MPE */

        found = 0;
        for (cur_brick_nr = 0; cur_brick_nr < num_bricks; ++cur_brick_nr)
        {
            gran = trie_p->m_iptBrickScratchpad.m_gran[cur_brick_nr];
            curNumEntriesInBrick = trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr];
            maxNumEntriesPerBrick = trie_p->m_iptBrickScratchpad.m_maxNumEntries[cur_brick_nr];

            if (trigger_length_1 <= gran && curNumEntriesInBrick < maxNumEntriesPerBrick)
            {
                found = 1;
                trie_p->m_iptBrickScratchpad.m_numOccupiedEntries[cur_brick_nr]++;
                break;
            }
        }

        if (!found)
            return 0;   /* Fit is not possible */
    }

    /*
     * Fit is possible
     */
    return 1;
}

NlmErrNum_t
kaps_trie_undo_giveout(
    kaps_lpm_trie * trie_p)
{
    uint32_t i;
    kaps_flat_lsn_data *curGiveoutData_p;
    kaps_nlm_allocator *alloc_p = trie_p->m_trie_global->m_alloc_p;

    for (i = 0; i < trie_p->m_num_items_in_pending_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_pendingGiveouts[i];
        kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);
    }
    trie_p->m_num_items_in_pending_giveouts = 0;

    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->lsn_cookie)
        {
            kaps_lsn_mc_destroy(curGiveoutData_p->lsn_cookie);
        }

        if (curGiveoutData_p->temp_uda_chunk)
        {
            kaps_uda_mgr_free(trie_p->m_trie_global->m_mlpmemmgr[0], curGiveoutData_p->temp_uda_chunk);
            curGiveoutData_p->temp_uda_chunk = NULL;
        }

        kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);
    }

    trie_p->m_num_items_in_completed_giveouts = 0;

    trie_p->m_num_items_in_to_update_lsns = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_add_sorted_prefix_to_addses_list(
    kaps_lpm_trie * trie_p,
    kaps_pfx_bundle * item)
{
    kaps_pfx_bundle *curBundle_p = NULL;
    kaps_pfx_bundle *itemBundle = (kaps_pfx_bundle *) item;
    int32_t i = 0, j = 0;

    if (trie_p->m_num_items_in_to_add_ses >= KAPS_MAX_SIZE_OF_TRIE_ARRAY)
        kaps_assert(0, "m_toadd_ses overflow \n");

    for (i = 0; i < (int32_t) trie_p->m_num_items_in_to_add_ses; ++i)
    {
        curBundle_p = trie_p->m_toadd_ses[i];

        if (curBundle_p->m_nPfxSize < itemBundle->m_nPfxSize)
        {
            for (j = trie_p->m_num_items_in_to_add_ses; j > i; --j)
            {
                trie_p->m_toadd_ses[j] = trie_p->m_toadd_ses[j - 1];
            }
            break;
        }
    }

    trie_p->m_toadd_ses[i] = item;

    ++trie_p->m_num_items_in_to_add_ses;

    return NLMERR_OK;

}

NlmErrNum_t
kaps_trie_giveout_pending_lsns(
    kaps_lpm_trie * trie_p,
    kaps_lsn_mc * originalLsn,
    struct kaps_lpm_entry * entryToInsert,
    uint32_t tryOnlyLsnPush,
    NlmReasonCode * o_reason)
{
    kaps_flat_lsn_data *curGiveoutData_p, *keepData_p, *giveData_p;
    kaps_lsn_mc_settings *lsnSettings_p = originalLsn->m_pSettings;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t numGiveouts = 0, i;
    kaps_lsn_mc *newLsn = NULL;
    kaps_trie_global *trie_global = trie_p->m_trie_global;
    kaps_nlm_allocator *alloc_p = trie_p->m_trie_global->m_alloc_p;
    int32_t deleteOriginalSes;
    void *tmp_ptr;
    kaps_trie_node *originalLsnTrienode_p, *curLsnTrieNode_p = NULL;
    uint32_t curBrickIter;
    uint32_t num_new_ses = 0;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    struct kaps_device *device = trie_p->m_trie_global->fibtblmgr->m_devMgr_p;
    struct kaps_fib_tbl *fibTbl = trie_p->m_trie_global->fibtblmgr->m_curFibTbl;
    struct kaps_db *db = fibTbl->m_db;
    uint32_t originalPoolId;
    uint32_t numIptEntriesInPool;
    uint32_t numIptEntriesToAdd;
    uint32_t isLsnFitPossible = 0;
    uint32_t doTriggersFit;
    uint32_t num_new_160b_triggers;
    uint32_t lsn_depth = 0, trigger_length_1 = 0, numRptBitsLoppedOff;
    kaps_pfx_bundle *pfxToInsert, **pfxToInsert_pp;
    uint32_t numExtraPfx;
    uint32_t list_size;

    pfxToInsert = NULL;
    pfxToInsert_pp = NULL;
    numExtraPfx = 0;
    if (entryToInsert)
    {
        pfxToInsert = entryToInsert->pfx_bundle;
        pfxToInsert_pp = &pfxToInsert;
        kaps_seq_num_gen_set_current_pfx_seq_nr(pfxToInsert);
        numExtraPfx = 1;
    }

    trie_p->m_numExtraIptEntriesNeededDuringGiveout = 0;

    if (db->num_algo_levels_in_db == 3)
    {
        uint32_t ads2_num;

        ads2_num = 1;
        if (db->is_type_a)
            ads2_num = 0;

        if (device->kaps_shadow->ads2_overlay[ads2_num].num_free_it_slots < 10)
        {
            *o_reason = NLMRSC_IT_ALLOC_FAILED;
            return NLMERR_FAIL;
        }
    }

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(originalLsn);

    kaps_lsn_mc_store_old_lsn_info(originalLsn, 0);

    tmp_ptr = KAPS_PFX_BUNDLE_GET_ASSOC_PTR(originalLsn->m_pParentHandle);
    kaps_memcpy(&originalLsnTrienode_p, tmp_ptr, sizeof(kaps_trie_node *));

    originalPoolId = originalLsnTrienode_p->m_rptParent_p->m_poolId;

    numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, originalLsnTrienode_p->m_rptParent_p->m_depth);

    curGiveoutData_p = kaps_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
    if (!curGiveoutData_p)
        return NLMERR_FAIL;

    errNum = kaps_lsn_mc_convert_lsn_to_flat_data(originalLsn, pfxToInsert_pp, numExtraPfx, curGiveoutData_p, o_reason);
    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    curGiveoutData_p->m_iitLmpsofarPfx = originalLsnTrienode_p->m_iitLmpsofarPfx_p;

    list_size = trie_p->m_num_items_in_pending_giveouts;
    trie_p->m_pendingGiveouts[list_size] = curGiveoutData_p;
    trie_p->m_num_items_in_pending_giveouts++;

    while (trie_p->m_num_items_in_pending_giveouts)
    {
        curGiveoutData_p = trie_p->m_pendingGiveouts[0];

        keepData_p = kaps_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
        if (!keepData_p)
        {
            kaps_trie_undo_giveout(trie_p);
            return NLMERR_FAIL;
        }
        keepData_p->m_iitLmpsofarPfx = curGiveoutData_p->m_iitLmpsofarPfx;

        giveData_p = kaps_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
        if (!giveData_p)
        {
            kaps_trie_undo_giveout(trie_p);
            return NLMERR_FAIL;
        }
        giveData_p->isGiveLsn = 1;
        giveData_p->m_iitLmpsofarPfx = curGiveoutData_p->m_iitLmpsofarPfx;

        errNum = kaps_lsn_mc_giveout(lsnSettings_p, fibTbl, curGiveoutData_p, keepData_p, giveData_p,
                                     pfxToInsert, alloc_p, tryOnlyLsnPush, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_trie_undo_giveout(trie_p);
            kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
            kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);

            if (*o_reason != NLMRSC_LOW_MEMORY)
                *o_reason = NLMRSC_UDA_ALLOC_FAILED;

            return errNum;
        }

        /*
         * If KEEP LSN is having prefixes then insert it into completed/pending giveout list else destroy it. 
         */
        if (keepData_p->m_numPfxInLsn)
        {

            newLsn = kaps_lsn_mc_create(trie_p->m_lsn_settings_p, trie_p, keepData_p->m_commonPfx->m_inuse);

            if (!newLsn)
            {
                kaps_trie_undo_giveout(trie_p);
                kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
                kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                *o_reason = NLMRSC_LOW_MEMORY;
                return NLMERR_FAIL;
            }

            isLsnFitPossible = 0;

            errNum = kaps_lsn_mc_convert_flat_data_to_lsn(keepData_p, newLsn, &isLsnFitPossible, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_trie_undo_giveout(trie_p);
                kaps_lsn_mc_destroy(newLsn);
                kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
                kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                return errNum;
            }

            if (isLsnFitPossible)
            {

                /*
                 * If we want to split if there is low UDA, then we need to check now if the UDA can satisfy the
                 * request. If UDA can't satisfy the request, then we will force a split
                 */
                if (lsnSettings_p->m_splitIfLowUda)
                {
                    kaps_uda_mgr_alloc(trie_global->m_mlpmemmgr[0], newLsn->m_numLpuBricks, device, newLsn,
                                       &keepData_p->temp_uda_chunk, trie_global->m_mlpmemmgr[0]->db);
                }

                /*
                 * We are checking with newLsn->m_numLpuBricks == 1 and adding to completed Giveouts, even if UDA is
                 * not available, since we don't can't split further below 1 brick and we want to stop at this stage
                 * without adding to pending giveouts. Later when we acquire resources, it will fail and we will come
                 * out of Giveout
                 */
                if (!lsnSettings_p->m_splitIfLowUda || keepData_p->temp_uda_chunk || newLsn->m_numLpuBricks == 1)
                {
                    keepData_p->lsn_cookie = newLsn;

                    list_size = trie_p->m_num_items_in_completed_giveouts;
                    trie_p->m_completedGiveouts[list_size] = keepData_p;
                    trie_p->m_num_items_in_completed_giveouts++;

                }
                else
                {
                    if (keepData_p->temp_uda_chunk)
                    {
                        kaps_uda_mgr_free(trie_global->m_mlpmemmgr[0], keepData_p->temp_uda_chunk);
                        keepData_p->temp_uda_chunk = NULL;
                    }

                    kaps_lsn_mc_destroy(newLsn);

                    list_size = trie_p->m_num_items_in_pending_giveouts;
                    trie_p->m_pendingGiveouts[list_size] = keepData_p;
                    trie_p->m_num_items_in_pending_giveouts++;
                }

            }
            else
            {
                kaps_lsn_mc_destroy(newLsn);

                list_size = trie_p->m_num_items_in_pending_giveouts;
                trie_p->m_pendingGiveouts[list_size] = keepData_p;
                trie_p->m_num_items_in_pending_giveouts++;

            }
        }
        else
        {
            kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
            keepData_p = NULL;
        }

        /*
         * If GIVE LSN is having prefixes then insert it into completed/pending giveout list else destroy it. 
         */
        if (giveData_p->m_numPfxInLsn)
        {

            newLsn = kaps_lsn_mc_create(trie_p->m_lsn_settings_p, trie_p, giveData_p->m_commonPfx->m_inuse);

            if (!newLsn)
            {
                kaps_trie_undo_giveout(trie_p);
                kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                *o_reason = NLMRSC_LOW_MEMORY;
                return NLMERR_FAIL;
            }

            isLsnFitPossible = 0;
            errNum = kaps_lsn_mc_convert_flat_data_to_lsn(giveData_p, newLsn, &isLsnFitPossible, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_trie_undo_giveout(trie_p);
                kaps_lsn_mc_destroy(newLsn);
                kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                return errNum;
            }

            if (isLsnFitPossible)
            {
                /*
                 * If we want to split if there is low UDA, then we need to check now if the UDA can satisfy the
                 * request. If UDA can't satisfy the request, then we will force a split
                 */
                if (lsnSettings_p->m_splitIfLowUda)
                {
                    kaps_uda_mgr_alloc(trie_global->m_mlpmemmgr[0], newLsn->m_numLpuBricks, device, newLsn,
                                       &giveData_p->temp_uda_chunk, trie_global->m_mlpmemmgr[0]->db);
                }

                /*
                 * We are checking with newLsn->m_numLpuBricks == 1 and adding to completed Giveouts, even if UDA is
                 * not available, since we don't can't split further below 1 brick and we want to stop at this stage
                 * without adding to pending giveouts. Later when we acquire resources, it will fail and we will come
                 * out of Giveout
                 */
                if (!lsnSettings_p->m_splitIfLowUda || giveData_p->temp_uda_chunk || newLsn->m_numLpuBricks == 1)
                {
                    giveData_p->lsn_cookie = newLsn;

                    list_size = trie_p->m_num_items_in_completed_giveouts;
                    trie_p->m_completedGiveouts[list_size] = giveData_p;
                    trie_p->m_num_items_in_completed_giveouts++;

                }
                else
                {
                    if (giveData_p->temp_uda_chunk)
                    {
                        kaps_uda_mgr_free(trie_global->m_mlpmemmgr[0], giveData_p->temp_uda_chunk);
                        giveData_p->temp_uda_chunk = NULL;
                    }

                    kaps_lsn_mc_destroy(newLsn);

                    list_size = trie_p->m_num_items_in_pending_giveouts;
                    trie_p->m_pendingGiveouts[list_size] = giveData_p;
                    trie_p->m_num_items_in_pending_giveouts++;
                }

            }
            else
            {
                kaps_lsn_mc_destroy(newLsn);

                list_size = trie_p->m_num_items_in_pending_giveouts;
                trie_p->m_pendingGiveouts[list_size] = giveData_p;
                trie_p->m_num_items_in_pending_giveouts++;
            }
        }
        else
        {
            kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
            giveData_p = NULL;
        }

        /*
         * Remove the first element in the pending giveout list and shift the remaining elements
         */
        list_size = trie_p->m_num_items_in_pending_giveouts;

        for (i = 1; i < list_size; ++i)
        {
            trie_p->m_pendingGiveouts[i - 1] = trie_p->m_pendingGiveouts[i];
        }

        trie_p->m_num_items_in_pending_giveouts--;

        kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);

        ++numGiveouts;
        if (numGiveouts > 1)
        {
            trie_p->m_isRecursiveSplit = 1;
        }
    }

    /*
     * Release the temporary UDA chunks stored in the giveout data back to the kaps_uda_mgr
     */
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->temp_uda_chunk)
        {
            kaps_uda_mgr_free(trie_p->m_trie_global->m_mlpmemmgr[0], curGiveoutData_p->temp_uda_chunk);
            curGiveoutData_p->temp_uda_chunk = NULL;
        }
    }

    /*
     * For the devices where we store the Lmpsofar in the LSN, we have to continue to have an LSN in the RPT node even
     * if the LSN is empty because we need space to store the RPT Lmpsofar in the brick of the LSN of the RPT node
     */
    /*
     * Count the number of new 160b triggers
     */
    num_new_160b_triggers = 0;
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];
        lsn_depth = curGiveoutData_p->m_commonPfx->m_inuse;
        trigger_length_1 = lsn_depth - numRptBitsLoppedOff;

        trigger_length_1 += 1;      /* Adding 1 for MPE */


        if (trigger_length_1 > 80)
        {
            num_new_160b_triggers++;
        }
    }

    numIptEntriesToAdd = trie_p->m_num_items_in_completed_giveouts;

    deleteOriginalSes = 1;

    if (db->num_algo_levels_in_db == 2)
    {
        numIptEntriesInPool = kaps_pool_mgr_get_num_entries(poolMgr, originalPoolId, KAPS_IPT_POOL);
        if (numIptEntriesInPool + numIptEntriesToAdd > (device->num_80b_entries_ab / 2))
        {
            kaps_trie_undo_giveout(trie_p);
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }
    }

    if (db->num_algo_levels_in_db == 3)
    {

        if (numIptEntriesToAdd)
        {

            doTriggersFit =
                kaps_trie_check_if_big_kaps_triggers_fit_during_ipt_split(trie_p, originalLsnTrienode_p,
                                                                          originalPoolId);

            if (!doTriggersFit)
            {
                kaps_trie_big_kaps_get_trigger_lengths_into_scratchpad(trie_p,
                                                                       kaps_trie_get_num_rpt_bits_lopped_off(device,
                                                                                                             originalLsnTrienode_p->m_rptParent_p->m_depth),
                                                                       originalPoolId, 0, 0);

                kaps_trie_find_num_bricks_for_big_kaps_triggers(trie_p);
            }



            if (!doTriggersFit)
            {
                kaps_trie_undo_giveout(trie_p);
                kaps_trie_pvt_add_rpt_nodes_to_split_or_move(trie_p,
                                                             kaps_pool_mgr_get_rpt_with_most_entries(poolMgr,
                                                                                                     originalPoolId,
                                                                                                     KAPS_IPT_POOL));

                trie_p->m_numExtraIptEntriesNeededDuringGiveout = numIptEntriesToAdd;

                trie_p->m_isRptSplitDueToIpt = 1;
                *o_reason = NLMRSC_RETRY_WITH_RPT_SPLIT;
                return NLMERR_FAIL;
            }
        }

    }

    /*
     * Convert the flat LSNs to actual LSNs and allocate resources for the new LSNs 
     */
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->m_commonPfx->m_inuse == originalLsn->m_nDepth)
        {
            deleteOriginalSes = 0;
        }

        newLsn = curGiveoutData_p->lsn_cookie;

        errNum = kaps_lsn_mc_acquire_resources(newLsn, newLsn->m_nLsnCapacity, newLsn->m_numLpuBricks, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_trie_undo_giveout(trie_p);
            return errNum;
        }

        if (newLsn->m_pSettings->m_isJoinedUdc && newLsn->m_pSettings->m_isPerLpuGran)
        {
            errNum = kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(newLsn, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;
        }

        if (newLsn->m_pSettings->m_isPerLpuGran)
        {
            kaps_lpm_lpu_brick *cur_lpu_brick;
            cur_lpu_brick = newLsn->m_lpuList;
            curBrickIter = 0;
            while (cur_lpu_brick)
            {
                uint32_t ixRqtSize = cur_lpu_brick->m_maxCapacity;

                errNum = kaps_lsn_mc_acquire_resources_per_lpu(newLsn, cur_lpu_brick, curBrickIter,
                                                               ixRqtSize, cur_lpu_brick->ad_db, o_reason);

                if (errNum != NLMERR_OK)
                {
                    kaps_trie_undo_giveout(trie_p);
                    return errNum;
                }

                curBrickIter++;
                cur_lpu_brick = cur_lpu_brick->m_next_p;
            }
            newLsn->m_nNumIxAlloced = newLsn->m_nLsnCapacity;
        }

        kaps_lsn_mc_add_extra_brick_for_joined_udcs(newLsn, o_reason);

        list_size = trie_p->m_num_items_in_to_update_lsns;

        if (list_size >= KAPS_MAX_SIZE_OF_TRIE_ARRAY)
            kaps_assert(0, "m_toupdate_lsns overflow \n");

        trie_p->m_toupdate_lsns[list_size] = newLsn;
        trie_p->m_num_items_in_to_update_lsns++;
    }

    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->m_commonPfx->m_inuse != originalLsn->m_nDepth)
            num_new_ses++;

        if (num_new_ses > poolMgr->m_numAlwaysEmptyIptSlotsPerPool)
        {
            kaps_lsn_mc_free_resources_per_lpu(newLsn);
            kaps_trie_undo_giveout(trie_p);
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }
    }

    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];
        newLsn = trie_p->m_toupdate_lsns[i];

        kaps_assert(curGiveoutData_p->m_commonPfx->m_inuse >= originalLsn->m_nDepth,
                    "The common prefix is shorter than original LSN during giveout \n");

        if (curGiveoutData_p->m_commonPfx->m_inuse == originalLsn->m_nDepth)
        {
            /*
             * Don't directly perform the assignment originalLsnTrienode_p->m_lsn_p = newLsn here. It can cause
             * coherency problems, when the new IPT entry created causes the shuffle of the old IPT entry that is still 
             * retained during Giveout 
             */
            list_size = trie_p->m_num_items_in_delayed_trie_node;
            trie_p->m_delayedTrieNode[list_size] = originalLsnTrienode_p;
            trie_p->m_num_items_in_delayed_trie_node++;

            list_size = trie_p->m_num_items_in_delayed_lsn;
            trie_p->m_delayedLsn[list_size] = newLsn;
            trie_p->m_num_items_in_delayed_lsn++;

            newLsn->m_pParentHandle = originalLsnTrienode_p->m_lp_prefix_p;
            curLsnTrieNode_p = originalLsnTrienode_p;
        }
        else
        {
            curLsnTrieNode_p = kaps_trie_node_insertPathFromPrefix(originalLsnTrienode_p,
                                                                   curGiveoutData_p->m_commonPfx, originalLsn->m_nDepth,
                                                                   curGiveoutData_p->m_commonPfx->m_inuse - 1);

            kaps_trie_add_sorted_prefix_to_addses_list(trie_p, curLsnTrieNode_p->m_lp_prefix_p);

            kaps_lsn_mc_destroy(curLsnTrieNode_p->m_lsn_p);
            curLsnTrieNode_p->m_lsn_p = newLsn;
            newLsn->m_pParentHandle = curLsnTrieNode_p->m_lp_prefix_p;
            curLsnTrieNode_p->m_node_type = NLMNSTRIENODE_LP;
        }

        if (trie_p->m_trie_global->m_isIITLmpsofar || trie_p->m_trie_global->m_isLsnLmpsofar)
        {
            curLsnTrieNode_p->m_iitLmpsofarPfx_p = curGiveoutData_p->m_iitLmpsofarPfx;
        }

        kaps_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);
    }

    trie_p->m_num_items_in_completed_giveouts = 0;

    list_size = trie_p->m_num_items_in_to_delete_lsns;
    if (list_size >= KAPS_MAX_SIZE_OF_TRIE_ARRAY)
        kaps_assert(0, "m_todelete_lsns overflow \n");

    trie_p->m_todelete_lsns[list_size] = originalLsn;
    trie_p->m_num_items_in_to_delete_lsns++;

    trie_p->m_trie_global->curLsnToSplit = originalLsn;

    if (deleteOriginalSes)
    {
        list_size = trie_p->m_num_items_in_to_delete_ses;
        if (list_size >= KAPS_MAX_SIZE_OF_TRIE_ARRAY)
            kaps_assert(0, "m_todelete_ses overflow \n");

        trie_p->m_todelete_ses[list_size] = originalLsnTrienode_p->m_lp_prefix_p;
        trie_p->m_num_items_in_to_delete_ses++;
    }
    else
    {
        list_size = trie_p->m_num_items_in_to_update_sit;
        if (list_size >= KAPS_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY)
            kaps_assert(0, "m_toupdate_sit overflow \n");

        trie_p->m_toupdate_sit[list_size] = originalLsnTrienode_p;
        trie_p->m_num_items_in_to_update_sit++;
    }

    trie_p->m_tbl_ptr->m_fibStats.numLsnGiveOuts++;

    if (numGiveouts > trie_p->m_tbl_ptr->m_fibStats.maxNumIptSplitsInOneOperation)
    {
        trie_p->m_tbl_ptr->m_fibStats.maxNumIptSplitsInOneOperation = numGiveouts;
    }

    if (numGiveouts > 1)
    {
        trie_p->m_tbl_ptr->m_fibStats.numRecursiveIptSplits++;
    }

    return NLMERR_OK;
}

uint32_t kaps_trie_get_total_entries_in_this_ad_mgr(
    struct kaps_device *device,
    struct kaps_db *parent_db)
{
    uint32_t total_entries_in_this_ad_mgr;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_db *tab;
    struct kaps_lpm_db *lpm_db;

    /*Find the number of entries present in all databases and tables for this index manager*/
    total_entries_in_this_ad_mgr = 0;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL) {
        struct kaps_db *db_iter = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db_iter->type != KAPS_DB_LPM) {
            continue;
        }

        if (db_iter->hw_res.db_res->index_mgr != parent_db->hw_res.db_res->index_mgr) {
            continue;
        }

        /*Go through the tables in this database*/
        tab = db_iter;
        while (tab) {
            if (tab->is_clone) {
                tab = tab->next_tab;
                continue;
            }

            lpm_db = (struct kaps_lpm_db*) tab;

            total_entries_in_this_ad_mgr += lpm_db->fib_tbl->m_numPrefixes;
            tab = tab->next_tab;
        }
    }

    return total_entries_in_this_ad_mgr;
}



uint32_t kaps_trie_get_uda_estimate(
    struct kaps_device *device,
    kaps_fib_tbl *fibTbl,
    kaps_lsn_mc_settings *settings,
    struct uda_mgr_stats *uda_stats,
    uint32_t *avg_num_pfx_per_brick,
    uint32_t log_info)
{
    uint32_t num_ad_bits_per_entry, num_uda_pfx_bits_per_entry, total_num_bits_per_entry;
    uint32_t num_unassigned_lpu_bricks_in_device;
    uint32_t num_assigned_bricks;
    int32_t total_unassigned_bits;
    uint32_t uda_based_estimate;
    uint32_t num_pfx_in_free_bricks_in_uda_mgr;
    struct kaps_db *parent_db;

    parent_db = fibTbl->m_db;
    if (parent_db->parent)
        parent_db = parent_db->parent;

    num_ad_bits_per_entry = 0;


    if (log_info) {
        kaps_printf("UDA ESTIMATE: Num Pfx in table = %d\n", fibTbl->m_numPrefixes);
        kaps_printf("UDA ESTIMATE: Num allocated bricks for all LSNs = %d\n", settings->m_numAllocatedBricksForAllLsns);
        kaps_printf("UDA ESTIMATE: Num allocated bricks in UDA Mgr = %d\n", uda_stats->total_num_allocated_lpu_bricks);
    }


    /*Find out the number of bits per entry*/
    if (fibTbl->m_numPrefixes >= 10) {
        num_assigned_bricks = uda_stats->total_num_allocated_lpu_bricks;

        num_uda_pfx_bits_per_entry = (num_assigned_bricks * settings->m_lpu_brick_width_1) /  fibTbl->m_numPrefixes;
        num_uda_pfx_bits_per_entry++; /*Adding 1 to take into effect the impact of integer division*/


    } else {
        /*If we have very few entries, then guess the num bits per entry in the UDA*/
        if (fibTbl->m_width <= 56) {
            num_uda_pfx_bits_per_entry = 32;
        } else {
            num_uda_pfx_bits_per_entry = 96;
        }
    }


    total_num_bits_per_entry = num_uda_pfx_bits_per_entry + num_ad_bits_per_entry;

    if (log_info) {
        kaps_printf("UDA ESTIMATE: Num UDA Bits / Pfx = %d\n", num_uda_pfx_bits_per_entry);
        kaps_printf("UDA ESTIMATE: Num AD Bits / Pfx = %d\n", num_ad_bits_per_entry);
        kaps_printf("UDA ESTIMATE: total_num_bits_per_entry = %d\n", total_num_bits_per_entry);
    }


    /*Find the number of unassigned uda bits on the device. This space has not been allocated to any database
    by resource manager*/
    num_unassigned_lpu_bricks_in_device = 0;

    total_unassigned_bits = num_unassigned_lpu_bricks_in_device * settings->m_lpu_brick_width_1;


    /*Find out how many prefixes can fit in the free bricks in the UDA manager*/
    num_pfx_in_free_bricks_in_uda_mgr = (uda_stats->total_num_free_lpu_bricks * settings->m_lpu_brick_width_1) / num_uda_pfx_bits_per_entry;

    if (log_info) {
        kaps_printf("UDA ESTIMATE: Number of Free Bricks in UDA Manager = %d\n", uda_stats->total_num_free_lpu_bricks);
        kaps_printf("UDA ESTIMATE: Number of prefix in Free Bricks in UDA Manager = %d\n", num_pfx_in_free_bricks_in_uda_mgr);
    }


    /*Initialize the UDA based estimate*/
    uda_based_estimate = fibTbl->m_numPrefixes;

    uda_based_estimate += num_pfx_in_free_bricks_in_uda_mgr;
    uda_based_estimate += total_unassigned_bits / num_uda_pfx_bits_per_entry;


    *avg_num_pfx_per_brick = settings->m_lpu_brick_width_1 / num_uda_pfx_bits_per_entry;

    if (log_info) {
        kaps_printf("UDA ESTIMATE: avg_num_pfx_per_brick = %d\n", *avg_num_pfx_per_brick);
        kaps_printf("UDA ESTIMATE: uda_based_estimate = %d\n", uda_based_estimate);
    }

    return uda_based_estimate;

}


void kaps_trie_get_resource_usage(
    kaps_fib_tbl *fibTbl,
    kaps_fib_resource_usage * rxcUsed_p)
{
    kaps_trie_global *trieGlobal_p = fibTbl->m_fibTblMgr_p->m_trieGlobal;
    struct kaps_device *device = fibTbl->m_fibTblMgr_p->m_devMgr_p;
    struct kaps_uda_mgr *mlpMemMgr_p = trieGlobal_p->m_mlpmemmgr[0];
    struct uda_mgr_stats uda_stats;
    uint32_t udaUsedInBits = 0;
    uint32_t numOfABsUsed = 0, numOfAitUsed = 0;
    uint32_t num_ab_no_compression = 0, num_ab_compression_1 = 0, num_ab_compression_2 = 0;
    uint32_t is_dba_brimmed = 0;
    uint32_t numABsReleased = 0;
    struct kaps_db *parent_db;
    struct kaps_db *tab;
    struct kaps_lpm_db *lpm_db;
    uint32_t total_entries_in_this_db;
    uint32_t uda_based_estimate;
    uint32_t dba_based_estimate, dba_extra_entry_estimate = 0;
    uint32_t rpt_based_estimate, rpt_extra_entry_estimate;
    uint32_t num_free_rpt_slots, num_allocated_rpt_slots;
    struct kaps_lsn_mc_settings *settings = fibTbl->m_trie->m_lsn_settings_p;
    uint32_t avg_num_pfx_per_brick = 1;
    uint32_t log_info = 0;

    parent_db = fibTbl->m_db;
    if (parent_db->parent)
        parent_db = parent_db->parent;

    /*Calculate the number of entries in all tables in this db*/
    total_entries_in_this_db = 0;
    tab = parent_db;

    while (tab) {
        if (tab->is_clone) {
            tab = tab->next_tab;
            continue;
        }

        lpm_db = (struct kaps_lpm_db*) tab;

        total_entries_in_this_db += lpm_db->fib_tbl->m_numPrefixes;
        tab = tab->next_tab;
    }

    kaps_uda_mgr_calc_stats(mlpMemMgr_p, &uda_stats);

    udaUsedInBits = uda_stats.total_num_allocated_lpu_bricks * fibTbl->m_trie->m_lsn_settings_p->m_lpu_brick_width_1;
    rxcUsed_p->m_udaUsedInKb = (udaUsedInBits + 1024 - 1) >> 10;

    uda_based_estimate = kaps_trie_get_uda_estimate(device, fibTbl, settings, &uda_stats, &avg_num_pfx_per_brick, log_info);

    kaps_pool_mgr_get_dba_stats(trieGlobal_p->poolMgr, parent_db, avg_num_pfx_per_brick, total_entries_in_this_db, &numOfABsUsed, &numOfAitUsed, &num_ab_no_compression,
                            &num_ab_compression_1, &num_ab_compression_2, &is_dba_brimmed, &numABsReleased, &dba_extra_entry_estimate, log_info);

    rxcUsed_p->m_numOfABsUsed = numOfABsUsed;

    rxcUsed_p->m_numABsReleased = numABsReleased;

    rxcUsed_p->m_num_ab_no_compression = num_ab_no_compression;
    rxcUsed_p->m_num_ab_compression_1 = num_ab_compression_1;
    rxcUsed_p->m_num_ab_compression_2 = num_ab_compression_2;


     /*Calculate the dba based estimate*/
    dba_based_estimate = dba_extra_entry_estimate + fibTbl->m_numPrefixes;

    if (log_info) {
        kaps_printf("DBA Estimate: dba_extra_entry_estimate + fibTbl->m_numPrefixes \n");
        kaps_printf("DBA Estimate: dba_based_estimate = %d\n", dba_based_estimate);
    }

    rxcUsed_p->m_estCapacity = uda_based_estimate;

    if (rxcUsed_p->m_estCapacity > dba_based_estimate)
        rxcUsed_p->m_estCapacity = dba_based_estimate;

    /*Take into account the RPT */
    if (parent_db->num_algo_levels_in_db == 3) {
        num_free_rpt_slots = kaps_rpm_get_num_free_slots(trieGlobal_p->m_rpm_p);

        num_allocated_rpt_slots = trieGlobal_p->m_rpm_p->kaps_simple_dba.pool_size - num_free_rpt_slots;

        if (log_info) {
            kaps_printf("\nRPT Estimate: num_free_rpt_slots = %d, num_allocated_rpt_slots = %d\n",
                        num_free_rpt_slots, num_allocated_rpt_slots);
        }

        if (num_allocated_rpt_slots > 1) {
            rpt_extra_entry_estimate = (1.0 * num_free_rpt_slots / num_allocated_rpt_slots) * total_entries_in_this_db;

            rpt_based_estimate = rpt_extra_entry_estimate + fibTbl->m_numPrefixes;

            if (log_info) {
                kaps_printf("RPT Estimate: Num Extra entries = (num_free_rpt_slots / num_allocated_rpt_slots) * total_entries_in_this_db\n");
                kaps_printf("RPT Estimate: Num Extra entries based on RPT = %d\n", rpt_extra_entry_estimate);
                kaps_printf("RPT Estimate: RPT Based Estimate = (%d + %d) = %d\n", rpt_extra_entry_estimate,
                        fibTbl->m_numPrefixes, rpt_based_estimate);
            }

            if (rxcUsed_p->m_estCapacity > rpt_based_estimate)
                rxcUsed_p->m_estCapacity = rpt_based_estimate;
        }
    }



}


void
kaps_trie_verify_kaps_ab_and_pool_mapping(
    kaps_lpm_trie * trie_p)
{
    uint32_t i, j, k;
    struct kaps_device *device = trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t num_free_slots_in_bmp, numFreeSlotsInPoolMgr;
    uint32_t poolId, ab_num;

    for (poolId = 0; poolId < KAPS_MAX_NUM_POOLS; ++poolId)
    {

        if (!poolMgr->m_ipmPoolInfo[poolId].m_isInUse)
            continue;

        ab_num = poolMgr->m_ipmPoolInfo[poolId].m_dba_mgr->m_ab_info->ab_num;

        num_free_slots_in_bmp = 0;
        for (i = 0; i < device->kaps_shadow->ab_to_small_bb[ab_num].num_bricks; ++i)
        {
            for (j = 0; j < 2; ++j)
            {
                for (k = 0; k < 8; ++k)
                {
                    if (device->kaps_shadow->ab_to_small_bb[ab_num].sub_ab_bricks[i].free_slot_bmp[j] & (1u << k))
                    {
                        num_free_slots_in_bmp++;
                    }
                }
            }
        }

        numFreeSlotsInPoolMgr = poolMgr->m_ipmPoolInfo[poolId].m_maxNumDbaEntries -
            poolMgr->m_ipmPoolInfo[poolId].m_curNumDbaEntries;

        if (num_free_slots_in_bmp != numFreeSlotsInPoolMgr)
        {
            kaps_assert(0, "Incorrect number of entries in bitmap and pool\n");
        }
    }
}

void
kaps_trie_verify_num_ipt_and_apt_in_rpt_node(
    kaps_trie_node * curNode)
{
    uint32_t oldNumIptEntries, oldNumAptEntries;

    if (curNode->m_isRptNode)
    {
        oldNumIptEntries = curNode->m_numIptEntriesInSubtrie;
        oldNumAptEntries = curNode->m_numAptEntriesInSubtrie;

        kaps_trie_pvt_calc_num_ipt_and_apt(curNode, NULL);

        kaps_assert(oldNumIptEntries == curNode->m_numIptEntriesInSubtrie,
                    "Mismatch in number of IPT entries in the subtrie \n");

        kaps_assert(oldNumAptEntries == curNode->m_numAptEntriesInSubtrie,
                    "Mismatch in number of APT entries in the subtrie \n");
    }

    if (curNode->m_child_p[0])
    {
        kaps_trie_verify_num_ipt_and_apt_in_rpt_node(curNode->m_child_p[0]);
    }

    if (curNode->m_child_p[1])
    {
        kaps_trie_verify_num_ipt_and_apt_in_rpt_node(curNode->m_child_p[1]);
    }
}

void
kaps_trie_verify_lsn(
    kaps_trie_node * curNode)
{
    if (curNode->m_node_type == NLMNSTRIENODE_LP)
    {
        kaps_lsn_mc_verify(curNode->m_lsn_p);
    }

    if (curNode->m_child_p[0])
    {
        kaps_trie_verify_lsn(curNode->m_child_p[0]);
    }

    if (curNode->m_child_p[1])
    {
        kaps_trie_verify_lsn(curNode->m_child_p[1]);
    }
}

void
kaps_trie_verify_reserved_pfx(
    kaps_trie_node * node_p)
{
    kaps_pfx_bundle *reservedPfxBundle;
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t chosenBrickIter;
    uint32_t numBricksWithReservedSlot;

    reservedPfxBundle = NULL;
    numBricksWithReservedSlot = 0;
    curLpuBrick = node_p->m_lsn_p->m_lpuList;
    chosenBrickIter = 0;
    while (curLpuBrick)
    {
        if (curLpuBrick->m_hasReservedSlot)
        {
            reservedPfxBundle = curLpuBrick->m_pfxes[curLpuBrick->m_maxCapacity - 1];
            ++numBricksWithReservedSlot;

            if (numBricksWithReservedSlot > 1)
            {
                kaps_assert(0, "More than one bricks have reserved slot\n");
            }
        }
        chosenBrickIter++;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (node_p->m_iitLmpsofarPfx_p && !reservedPfxBundle)
    {
        kaps_assert(0, "Lsn Lmpsofar is not present in the reserved slot\n");
    }

    if (reservedPfxBundle && reservedPfxBundle->m_nPfxSize != node_p->m_depth)
    {
        kaps_assert(0, "The length of the reserved LSN Lmpsofar prefix is wrong\n");
    }

    if (reservedPfxBundle && node_p->m_lsn_p->m_pSettings->m_onlyPfxCopyInReservedSlot
        && !reservedPfxBundle->m_isPfxCopy)
    {
        kaps_assert(0, "Prefix in reserved slot should be a prefix copy \n");
    }
}

void
kaps_trie_verify_iit_lmpsofar(
    kaps_trie_node * node_p,
    kaps_trie_node * ancestorIptNode_p)
{
    kaps_pfx_bundle *lmp_in_prev_lsn;
    kaps_lpm_trie *trie_p = NULL;
    uint32_t is_candidate_node;
    struct kaps_db *db;

    if (!node_p)
        return;

    trie_p = node_p->m_trie_p;
    db = trie_p->m_tbl_ptr->m_db;

    if (trie_p->m_trie_global->m_isLsnLmpsofar && node_p->m_isRptNode && node_p->m_depth > 8
        && !node_p->m_lsn_p->m_lpuList)
    {
        kaps_assert(0, "RPT node with depth greater than 8 should have a valid LSN if LSN Lmpsofar is enabled\n");
    }

    is_candidate_node = 0;
    if (node_p->m_node_type == NLMNSTRIENODE_LP)
        is_candidate_node = 1;

    if (trie_p->m_trie_global->m_isIITLmpsofar && db->num_algo_levels_in_db > 2 && node_p->m_isRptNode)
    {
        is_candidate_node = 1;
    }

    if (is_candidate_node)
    {

        if (ancestorIptNode_p)
        {
            lmp_in_prev_lsn = kaps_lsn_mc_locate_lpm(ancestorIptNode_p->m_lsn_p, node_p->m_lp_prefix_p->m_data,
                                                     node_p->m_lp_prefix_p->m_nPfxSize, NULL, NULL, NULL);
            /*
             * Find the prefix in the ancestor LSN that is lmpsofar of current LSN and the prefix is not a prefix copy
             */
            if (lmp_in_prev_lsn && !lmp_in_prev_lsn->m_isPfxCopy)
            {
                if (node_p->m_iitLmpsofarPfx_p == lmp_in_prev_lsn)
                {
                    /*
                     * If the lmp_in_prev_lsn is the m_iitLmpsofarPfx_p of the current node, then either
                     * ancestorIptNode has no m_iitLmpsofarPfx_p or the meta priority of m_iitLmpsofarPfx_p of
                     * ancestorIptNode is lower or equal to priority of lmp_in_prev_lsn 
                     */
                    if (ancestorIptNode_p->m_iitLmpsofarPfx_p)
                    {
                        if (ancestorIptNode_p->m_iitLmpsofarPfx_p->m_backPtr->meta_priority <
                            lmp_in_prev_lsn->m_backPtr->meta_priority)
                            kaps_assert(0, "Higher priority IIT Lmpsofar exists in the trie path\n");
                    }

                }
                else
                {
                    /*
                     * m_iitLmpsofarPfx_p of cur node should match with that of the ancestor
                     */
                    if (node_p->m_iitLmpsofarPfx_p != ancestorIptNode_p->m_iitLmpsofarPfx_p)
                        kaps_assert(0, "IIT Lmpsofar of the node doesn't match with IIT Lmpsofar of the Ancestor\n");

                    /*
                     * ancestor m_iitLmpsofarPfx_p should not be NULL
                     */
                    if (!ancestorIptNode_p->m_iitLmpsofarPfx_p)
                        kaps_assert(0, "Lmpsofar is present in the Ancestor LSN\n");

                    /*
                     * ancestor m_iitLmpsofarPfx_p should have more priority than lmp_in_prev_lsn
                     */
                    if (ancestorIptNode_p->m_iitLmpsofarPfx_p->m_backPtr->meta_priority >=
                        lmp_in_prev_lsn->m_backPtr->meta_priority)
                        kaps_assert(0, "Lmpsofar in prev LSN should be IIT Lmpsofar\n");
                }
            }
            else
            {
                /*
                 * If lmp_in_prev_lsn doesn't exist or if lmp_in_prev_lsn is a prefix copy, then m_iitLmpsofarPfx_p of
                 * the node and its ancestor should match 
                 */
                if (node_p->m_iitLmpsofarPfx_p != ancestorIptNode_p->m_iitLmpsofarPfx_p)
                    kaps_assert(0, "The previous LP node and current LP node should have same IIT Lmpssofar\n");
            }
        }
        else
        {
            if (node_p->m_iitLmpsofarPfx_p != NULL)
                kaps_assert(0, "The IIT lmpsofar of the current node should be NULL\n");
        }

        if (node_p->m_iitLmpsofarPfx_p && node_p->m_iitLmpsofarPfx_p->m_nPfxSize >= node_p->m_depth)
            kaps_assert(0, "IIT lmpsofar should be less than the node depth\n");

        if (node_p->m_iitLmpsofarPfx_p && node_p->m_iitLmpsofarPfx_p->m_isPfxCopy)
            kaps_assert(0, "IIT lmpsofar should not be a prefix copy\n");

        if (node_p->m_iitLmpsofarPfx_p && !node_p->m_iitLmpsofarPfx_p->m_backPtr)
            kaps_assert(0, "IIT lmpsofar entry handle should not be NULL\n");

        if (trie_p->m_trie_global->m_isLsnLmpsofar)
        {
            kaps_trie_verify_reserved_pfx(node_p);
        }

        ancestorIptNode_p = node_p;
    }
    else
    {
        if (node_p->m_iitLmpsofarPfx_p != NULL)
            kaps_assert(0, "Non IPT node has iitLmpsofar set\n");
    }

    if (node_p->m_child_p[0])
        kaps_trie_verify_iit_lmpsofar(node_p->m_child_p[0], ancestorIptNode_p);

    if (node_p->m_child_p[1])
        kaps_trie_verify_iit_lmpsofar(node_p->m_child_p[1], ancestorIptNode_p);
}

void
kaps_trie_verify_rpt_apt_lmpsofar(
    kaps_trie_node * cur_node,
    kaps_trie_node * highestPriorityAptTrieNode)
{
    if (!cur_node)
        return;

    if (!cur_node->m_isRptNode && cur_node->m_rptAptLmpsofarPfx)
        kaps_assert(0, "Non RPT node has m_rptAptLmpsofarPfx set \n");

    if (cur_node->m_aptLmpsofarPfx_p && cur_node->m_aptLmpsofarPfx_p->m_isPfxCopy)
        kaps_assert(0, "APT LmpsofarPfx can't be prefix copy \n");

    if (cur_node->m_rptAptLmpsofarPfx && !cur_node->m_rptAptLmpsofarPfx->m_isPfxCopy)
        kaps_assert(0, "RPT APT lmpsofar should be prefix copy \n");

    if (highestPriorityAptTrieNode)
    {
        if (cur_node->m_aptLmpsofarPfx_p
            && cur_node->m_aptLmpsofarPfx_p->m_backPtr->meta_priority <=
            highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr->meta_priority)
        {
            if (cur_node->m_rptAptLmpsofarPfx)
            {
                kaps_assert(0,
                            "RPT APT Lmpsofar should not be present since trie node APT lmpsofar itself is of highest priority\n");
            }
        }
        else
        {
            if (cur_node->m_isRptNode &&
                (!cur_node->m_rptAptLmpsofarPfx
                 || cur_node->m_rptAptLmpsofarPfx->m_backPtr !=
                 highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr))
            {
                kaps_assert(0, "RPT APT lmpsofar is not the highest priority in the trie path \n");
            }
        }
    }
    else
    {
        /*
         * If highest priority APT trie node is NULL, then m_rptAptLmpsofarPfx should also be NULL
         */
        if (cur_node->m_rptAptLmpsofarPfx != NULL)
        {
            kaps_assert(0, "RPT APT Lmpsofar should be NULL");
        }
    }

    /*
     * update the highest priority APT trie node for the next level
     */
    if (highestPriorityAptTrieNode)
    {
        if (cur_node->m_aptLmpsofarPfx_p
            && cur_node->m_aptLmpsofarPfx_p->m_backPtr->meta_priority <=
            highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr->meta_priority)
        {
            highestPriorityAptTrieNode = cur_node;
        }
    }
    else
    {
        if (cur_node->m_aptLmpsofarPfx_p)
        {
            highestPriorityAptTrieNode = cur_node;
        }
    }

    if (cur_node->m_child_p[0])
        kaps_trie_verify_rpt_apt_lmpsofar(cur_node->m_child_p[0], highestPriorityAptTrieNode);

    if (cur_node->m_child_p[1])
        kaps_trie_verify_rpt_apt_lmpsofar(cur_node->m_child_p[1], highestPriorityAptTrieNode);
}

void
kaps_trie_verify_down_stream_rpt(
    kaps_trie_node * curTrieNode,
    kaps_trie_node * upstreamRptNode)
{
    NlmNsDownStreamRptNodes *listNode;
    uint32_t found;

    if (!curTrieNode)
        return;

    if (!curTrieNode->m_isRptNode && curTrieNode->m_downstreamRptNodes)
        kaps_assert(0, "Non RPT node has down stream RPT nodes \n");

    if (curTrieNode->m_isRptNode)
    {
        listNode = curTrieNode->m_downstreamRptNodes;

        /*
         * Verify the all the RPT nodes in the downstream list of the current trie node
         */
        while (listNode)
        {
            if (listNode->rptNode->m_depth <= curTrieNode->m_depth)
            {
                kaps_assert(0, "upstream RPT node is present in downstream RPT list \n");
            }

            if (!listNode->rptNode->m_isRptNode)
            {
                kaps_assert(0, "Non RPT node is present in the downstream RPT list \n");
            }

            if (listNode->rptNode == curTrieNode)
            {
                kaps_assert(0, "Circular reference in the downstream RPT list \n");
            }

            if (!kaps_prefix_pvt_is_more_specific
                (listNode->rptNode->m_lp_prefix_p->m_data, listNode->rptNode->m_lp_prefix_p->m_nPfxSize,
                 curTrieNode->m_lp_prefix_p->m_data, curTrieNode->m_lp_prefix_p->m_nPfxSize))
            {
                kaps_assert(0, "In correct RPT node linked in the list of down stream RPT nodes \n");
            }

            listNode = listNode->next;
        }

        /*
         * Check if the current trie node is present in the upstream RPT node's down stream list
         */
        if (upstreamRptNode)
        {
            listNode = upstreamRptNode->m_downstreamRptNodes;
            found = 0;
            while (listNode)
            {
                if (listNode->rptNode == curTrieNode)
                {
                    found = 1;
                    break;
                }
                listNode = listNode->next;
            }

            if (!found)
            {
                kaps_assert(0, "Unable to find the current trie node in the upstream RPT node");
            }
        }

        upstreamRptNode = curTrieNode;
    }

    if (curTrieNode->m_child_p[0])
        kaps_trie_verify_down_stream_rpt(curTrieNode->m_child_p[0], upstreamRptNode);

    if (curTrieNode->m_child_p[1])
        kaps_trie_verify_down_stream_rpt(curTrieNode->m_child_p[1], upstreamRptNode);
}

void
kaps_trie_verify_dba_reduction(
    kaps_trie_node * node_p)
{
    if (!node_p->m_trie_p->m_trie_global->use_reserved_abs_for_160b_trig)
        return;

    if (node_p->m_isRptNode)
    {
        kaps_ipm *ipm = kaps_pool_mgr_get_ipm_for_pool(node_p->m_trie_p->m_trie_global->poolMgr, node_p->m_poolId);

        if (node_p->m_numReserved160bTrig)
        {

            if (ipm->m_ab_info->ab_num > 1 && ipm->m_ab_info->conf == 1)
            {
                kaps_assert(0, "The RPT with reserved entries is in non reserved pool\n");
            }
        }

        if (node_p->m_numIptEntriesInSubtrie)
        {
            if (ipm->m_ab_info->ab_num <= 1)
            {
                kaps_assert(0, "Normal IPT entries are stored in Reserved ABs\n");
            }
        }
    }

    if (node_p->m_child_p[0])
    {
        kaps_trie_verify_dba_reduction(node_p->m_child_p[0]);
    }

    if (node_p->m_child_p[1])
    {
        kaps_trie_verify_dba_reduction(node_p->m_child_p[1]);
    }
}

void
kaps_trie_verify_kaps_joined_udcs(
    kaps_trie_node * node_p)
{
    uint32_t lpu_nr, row_nr;

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        struct kaps_uda_mgr *mgr = node_p->m_lsn_p->m_pSettings->m_pMlpMemMgr[0];

        kaps_uda_mgr_compute_abs_brick_udc_and_row(mgr, node_p->m_lsn_p->m_mlpMemInfo,
                                                   node_p->m_lsn_p->m_numLpuBricks - 1, &lpu_nr, &row_nr);

        if (lpu_nr % 2 == 0 && mgr->config.joined_udcs[lpu_nr + 1])
        {
            kaps_assert(0, "Last Brick should not be joined \n");
        }
    }

    if (node_p->m_child_p[0])
    {
        kaps_trie_verify_kaps_joined_udcs(node_p->m_child_p[0]);
    }

    if (node_p->m_child_p[1])
    {
        kaps_trie_verify_kaps_joined_udcs(node_p->m_child_p[1]);
    }
}

/*For the first time this function is called, we will be passing the commonNode as the nodeToCheckForMerge.
The commonNode will serve as the starting point of the sub-tree to be explored. As we begin
exploring the tree, nodeToCheckForMerge represents the potential trieNode2 which can be merged with
original IPT Node*/
kaps_trie_node *
kaps_trie_check_sub_tree_for_ipt_merge(
    kaps_trie_node * originalIptNode,
    kaps_trie_node * nodeToCheckForMerge,
    kaps_trie_node * skipNode)
{
    kaps_trie_node *trieNodeToMergeWith = NULL;

    /*
     * If nodeToCheckForMerge is the same as skipNode, then we have already explored this sub-tree previously and we
     * don't want to explore it again. So immediately return
     */
    if (nodeToCheckForMerge == originalIptNode || nodeToCheckForMerge == skipNode)
        return NULL;

    /*
     * If we traverse beyond the current RPT node, then immediately return
     */
    if (nodeToCheckForMerge->m_isRptNode && nodeToCheckForMerge != originalIptNode->m_rptParent_p)
    {
        return NULL;
    }

    if (nodeToCheckForMerge->m_node_type == NLMNSTRIENODE_LP)
    {
        kaps_lsn_mc_settings *settings = originalIptNode->m_lsn_p->m_pSettings;
        kaps_lpm_lpu_brick *curBrick;
        uint16_t maxGran, maxGranIx, diffInDepth;
        uint32_t numPfxThatCanBeFitWithMaxGran, actualNumPfx;

        if (originalIptNode->m_lsn_p->m_numLpuBricks + nodeToCheckForMerge->m_lsn_p->m_numLpuBricks + 1 >
            settings->m_maxLpuPerLsn)
        {
            return NULL;
        }

        /*
         * Find the maximum granularity in both the LSNs
         */

        maxGran = 0;

        curBrick = originalIptNode->m_lsn_p->m_lpuList;
        while (curBrick)
        {
            if (curBrick->m_gran > maxGran)
            {
                maxGran = curBrick->m_gran;
            }

            curBrick = curBrick->m_next_p;
        }

        curBrick = nodeToCheckForMerge->m_lsn_p->m_lpuList;
        while (curBrick)
        {
            if (curBrick->m_gran > maxGran)
            {
                maxGran = curBrick->m_gran;
            }

            curBrick = curBrick->m_next_p;
        }

        if (originalIptNode->m_depth > nodeToCheckForMerge->m_depth)
        {
            diffInDepth = originalIptNode->m_depth - nodeToCheckForMerge->m_depth;
        }
        else
        {
            diffInDepth = nodeToCheckForMerge->m_depth - originalIptNode->m_depth;
        }

        /*
         * maxGran has the maximum granularity in the LSN portion after lopoff in the two LSNs to be merged. Since we
         * are going to move the prefixes up, we are also now adding the maximum distance by which we should move up.
         */
        maxGran += diffInDepth;
        maxGran = kaps_lsn_mc_compute_gran(settings, maxGran, &maxGranIx);

        numPfxThatCanBeFitWithMaxGran = settings->m_maxPfxInBrickForGranIx[maxGranIx] * (settings->m_maxLpuPerLsn - 1);

        actualNumPfx = originalIptNode->m_lsn_p->m_nNumPrefixes + nodeToCheckForMerge->m_lsn_p->m_nNumPrefixes;

        if (actualNumPfx > numPfxThatCanBeFitWithMaxGran)
        {
            return NULL;
        }

        return nodeToCheckForMerge;
    }

    if (nodeToCheckForMerge->m_child_p[0])
    {
        trieNodeToMergeWith = kaps_trie_check_sub_tree_for_ipt_merge(originalIptNode,
                                                                     nodeToCheckForMerge->m_child_p[0], skipNode);

        if (trieNodeToMergeWith)
        {
            return trieNodeToMergeWith;
        }
    }

    if (nodeToCheckForMerge->m_child_p[1])
    {
        trieNodeToMergeWith = kaps_trie_check_sub_tree_for_ipt_merge(originalIptNode,
                                                                     nodeToCheckForMerge->m_child_p[1], skipNode);

        if (trieNodeToMergeWith)
        {
            return trieNodeToMergeWith;
        }
    }

    return NULL;
}

uint32_t
kaps_trie_check_if_related_ipt_nodes_can_be_merged(
    kaps_trie_node * originalIptNode,
    kaps_trie_node ** trieNode2_pp,
    kaps_trie_node ** newTrieNode_pp)
{
    uint32_t canIptNodeBeMerged = 0;
    kaps_trie_node *curNode;
    kaps_trie_node *prevNode;
    kaps_trie_node *trieNodeToMergeWith = NULL;

    *trieNode2_pp = NULL;
    *newTrieNode_pp = NULL;

    /*
     * We can't go further up the trie to find another IPT node with which we can merge since this node is an RPT node
     * and only triggers belonging to the same RPT node can be merged. So return from here itself
     */
    if (originalIptNode->m_isRptNode)
        return 0;

    /*
     * During the processing of many related nodes after the giveout, a node that was IPT node may no longer be an IPT
     * node now So if the node is not an IPT node right now, then simply return 
     */
    if (originalIptNode->m_node_type != NLMNSTRIENODE_LP)
    {
        return 0;
    }

    curNode = originalIptNode->m_parent_p;
    prevNode = originalIptNode;
    canIptNodeBeMerged = 0;
    /*
     * Traverse up the tree from the originalIptNode. Pick a node up the path and search the other sub-tree under this
     * common node to see if there is a IPT node with which we can merge the originalIptNode
     */
    while (curNode)
    {
        if (curNode->m_node_type == NLMNSTRIENODE_LP)
        {
            break;
        }

        /*
         * prevNode represents the path which has already been explored. So pass this as the path to be skipped curNode 
         * is commonNode under which we should start the search for the new IPT node that we can merge the
         * originalIptNode with
         */
        trieNodeToMergeWith = kaps_trie_check_sub_tree_for_ipt_merge(originalIptNode, curNode, prevNode);

        if (trieNodeToMergeWith)
        {
            *trieNode2_pp = trieNodeToMergeWith;
            *newTrieNode_pp = curNode;
            canIptNodeBeMerged = 1;
            break;
        }

        if (curNode->m_isRptNode)
        {
            break;
        }

        prevNode = curNode;
        curNode = curNode->m_parent_p;

    }

    return canIptNodeBeMerged;
}

NlmErrNum_t
kaps_trie_pvt_undo_related_lsn_merge(
    kaps_lpm_trie * trie_p,
    kaps_lsn_mc * tempMergedLsn,
    kaps_lsn_mc * newMergedLsn,
    kaps_flat_lsn_data * mergedFlatLsnData)
{
    kaps_nlm_allocator *alloc = trie_p->m_trie_global->m_alloc_p;

    if (tempMergedLsn)
    {
        kaps_nlm_allocator_free(alloc, (void *) tempMergedLsn);
    }

    if (newMergedLsn)
    {
        kaps_lsn_mc_destroy(newMergedLsn);
    }

    if (mergedFlatLsnData)
    {
        kaps_lsn_mc_destroy_flat_lsn_data(alloc, mergedFlatLsnData);
    }

    return NLMERR_OK;
}

uint32_t
kaps_trie_big_kaps_can_trigger_fit(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * newIptTrieNode)
{
    struct kaps_device *device = trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t gran, maxNumEntriesPerBrick, cur_brick_nr, j;
    uint32_t curNumFreeSlotsInBrick, trigger_length;
    kaps_ipm *ipm = kaps_pool_mgr_get_ipm_for_pool(poolMgr, newIptTrieNode->m_rptParent_p->m_poolId);
    struct kaps_ab_info *ab = ipm->m_ab_info;
    uint32_t num_bricks = device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks;
    uint32_t canTriggerFit;
    kaps_trie_node *rptParent = newIptTrieNode->m_rptParent_p;

    trigger_length = newIptTrieNode->m_depth - kaps_trie_get_num_rpt_bits_lopped_off(device, rptParent->m_depth);
    trigger_length += 1;        /* Add 1 for MPE */

    canTriggerFit = 0;

    /*
     * Fill up the IPT Brick Scratchpad by looking at the ab_to_small_bb information
     */
    for (cur_brick_nr = 0; cur_brick_nr < num_bricks; ++cur_brick_nr)
    {
        gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].sub_ab_gran;

        if (trigger_length > gran)
            continue;

        maxNumEntriesPerBrick = (KAPS_BKT_WIDTH_1 / (gran + KAPS_AD_WIDTH_1));

        if (maxNumEntriesPerBrick > KAPS_MAX_PFX_PER_BKT_ROW)
            maxNumEntriesPerBrick = KAPS_MAX_PFX_PER_BKT_ROW;

        /*
         * Find out how many entries are present in the small BB brick by looking at the free slot bmp
         */
        curNumFreeSlotsInBrick = 0;
        for (j = 0; j < maxNumEntriesPerBrick; ++j)
        {
            if (kaps_array_check_bit
                (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].free_slot_bmp, j))
            {
                /*
                 * It is a free slot. Do nothing
                 */
                curNumFreeSlotsInBrick++;
                break;
            }
        }

        if (curNumFreeSlotsInBrick)
        {
            canTriggerFit = 1;
            break;
        }
    }

    return canTriggerFit;
}

NlmErrNum_t
kaps_trie_merge_related_lsns(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * trieNodeToMerge1,
    kaps_trie_node * trieNodeToMerge2,
    kaps_trie_node * newIptTrieNode,
    NlmReasonCode * o_reason)
{
    kaps_lsn_mc_settings *settings_p = trie_p->m_lsn_settings_p;
    kaps_flat_lsn_data *mergedFlatLsnData = NULL;
    kaps_lsn_mc *tempMergedLsn = NULL, *newMergedLsn = NULL;
    NlmErrNum_t errNum;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    kaps_lsn_mc *oldLsn1, *oldLsn2;
    kaps_lpm_lpu_brick *iterBrick, *lastBrickOfLsn1;
    uint32_t i, j, doesLsnFit, brickNum;
    kaps_pfx_bundle *curPfxBundle, *toDeletePfxCopy = NULL;
    kaps_fib_prefix_index_changed_app_cb_t appCB = settings_p->m_pAppCB;
    kaps_fib_tbl *tbl;
    uint32_t numIptEntriesInPool, maxAllowedIptEntriesInPool;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t numExtraPfxFromIptParent;
    kaps_trie_node *iptAncestorNode, *iterNode;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;

    (void) o_reason;

    if (!settings_p->m_isRelatedLsnMergeEnabled)
        return NLMERR_OK;

    if (db->is_destroy_in_progress)
        return NLMERR_OK;

    if (trieNodeToMerge1->m_rptParent_p != trieNodeToMerge2->m_rptParent_p)
    {
        kaps_assert(0, "Incorrect trie nodes trying to be merged \n");
        return NLMERR_OK;
    }

    if (trieNodeToMerge1->m_node_type != NLMNSTRIENODE_LP || trieNodeToMerge2->m_node_type != NLMNSTRIENODE_LP)
        return NLMERR_OK;

    /*
     * Check that we have space in the AB since in this case we will have a new IPT entry and a new IT slot
     */
    numIptEntriesInPool =
        kaps_pool_mgr_get_num_entries(poolMgr, trieNodeToMerge1->m_rptParent_p->m_poolId, KAPS_IPT_POOL);
    maxAllowedIptEntriesInPool =
        kaps_pool_mgr_get_max_allowed_entries(poolMgr, trieNodeToMerge1->m_rptParent_p->m_poolId, KAPS_IPT_POOL);

    if (numIptEntriesInPool >= maxAllowedIptEntriesInPool)
        return NLMERR_OK;

    if (db->num_algo_levels_in_db == 3)
    {
        if (!kaps_trie_big_kaps_can_trigger_fit(trie_p, newIptTrieNode))
        {
            return NLMERR_OK;
        }
    }

    /*
     * Check that we have space in the IT
     */
    if (db->num_algo_levels_in_db == 3)
    {
        if (kaps_trie_is_big_kaps_it_full(trie_p))
            return NLMERR_OK;
    }

    iptAncestorNode = NULL;
    iterNode = newIptTrieNode->m_parent_p;
    while (iterNode)
    {
        if (iterNode->m_node_type == NLMNSTRIENODE_LP)
        {
            iptAncestorNode = iterNode;
            break;
        }

        iterNode = iterNode->m_parent_p;
    }

    oldLsn1 = trieNodeToMerge1->m_lsn_p;
    oldLsn2 = trieNodeToMerge2->m_lsn_p;

    tbl = oldLsn1->m_pTbl;

    mergedFlatLsnData = kaps_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, &reason);
    if (!mergedFlatLsnData)
    {
        return NLMERR_OK;
    }

    /*
     * Create the temporary merged LSN
     */
    tempMergedLsn = kaps_lsn_mc_create(settings_p, trie_p, newIptTrieNode->m_depth);
    if (!tempMergedLsn)
    {
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    /*
     * Find the last brick in the oldLsn1
     */
    lastBrickOfLsn1 = NULL;
    iterBrick = oldLsn1->m_lpuList;
    while (iterBrick)
    {
        lastBrickOfLsn1 = iterBrick;
        iterBrick = iterBrick->m_next_p;
    }

    /*
     * Link the oldLsn1 list and the oldLsn2 list
     */
    lastBrickOfLsn1->m_next_p = oldLsn2->m_lpuList;

    /*
     * Make the temporary merged LSN point to the merged LPU List
     */
    tempMergedLsn->m_lpuList = oldLsn1->m_lpuList;
    tempMergedLsn->m_numLpuBricks = oldLsn1->m_numLpuBricks + oldLsn2->m_numLpuBricks;
    tempMergedLsn->m_nLsnCapacity = oldLsn1->m_nLsnCapacity + oldLsn2->m_nLsnCapacity;
    tempMergedLsn->m_nNumPrefixes = oldLsn1->m_nNumPrefixes + oldLsn2->m_nNumPrefixes;
    tempMergedLsn->m_pParentHandle = newIptTrieNode->m_lp_prefix_p;

    /*
     * Find the prefixes from the IPT Ancestor that need to be moved to the new Lsn. Store these entries in an array 
     */

    numExtraPfxFromIptParent = 0;
    if (iptAncestorNode)
    {
        kaps_lsn_mc_find_prefixes_in_path(iptAncestorNode->m_lsn_p,
                                          newIptTrieNode->m_lp_prefix_p->m_data,
                                          newIptTrieNode->m_lp_prefix_p->m_nPfxSize,
                                          trie_p->m_trie_global->m_extraPfxArray,
                                          trie_p->m_trie_global->m_pfxLocationsToMove, &numExtraPfxFromIptParent);
    }

    /*
     * Convert the temporary merged LSN into Flat LSN Data
     */
    errNum =
        kaps_lsn_mc_convert_lsn_to_flat_data(tempMergedLsn, trie_p->m_trie_global->m_extraPfxArray,
                                             numExtraPfxFromIptParent, mergedFlatLsnData, &reason);

    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        lastBrickOfLsn1->m_next_p = NULL;
        return NLMERR_OK;
    }

    /*
     * Unlink the lsn1 bricks from the lsn2 bricks
     */
    lastBrickOfLsn1->m_next_p = NULL;

    /*
     * Create the new Merged LSN
     */
    newMergedLsn = kaps_lsn_mc_create(settings_p, trie_p, newIptTrieNode->m_depth);
    if (!newMergedLsn)
    {
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    newMergedLsn->m_pParentHandle = newIptTrieNode->m_lp_prefix_p;

    /*
     * if both the LSN are having reserved slots free one of them 
     */
    for (i = 0; i < mergedFlatLsnData->m_numPfxInLsn; i++)
    {
        curPfxBundle = mergedFlatLsnData->m_pfxesInLsn[i];

        if (curPfxBundle->m_isPfxCopy && curPfxBundle->m_nPfxSize > newMergedLsn->m_nDepth)
        {
            toDeletePfxCopy = curPfxBundle;
            mergedFlatLsnData->m_pfxesInLsn[i] = NULL;
            for (j = i + 1; j < mergedFlatLsnData->m_numPfxInLsn; j++, i++)
            {
                mergedFlatLsnData->m_pfxesInLsn[i] = mergedFlatLsnData->m_pfxesInLsn[j];
                mergedFlatLsnData->pfx_color[i] = mergedFlatLsnData->pfx_color[j];
            }
            mergedFlatLsnData->m_numPfxInLsn--;
            break;
        }
    }

    /*
     * Convert the Flat LSN Data to new merged LSN
     */
    errNum = kaps_lsn_mc_convert_flat_data_to_lsn(mergedFlatLsnData, newMergedLsn, &doesLsnFit, &reason);
    if (errNum != NLMERR_OK || !doesLsnFit)
    {
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    /*
     * Destroy the temporary LSN and the temporary Flat LSN Data
     */
    kaps_nlm_allocator_free(settings_p->m_pAlloc, (void *) tempMergedLsn);
    tempMergedLsn = NULL;

    kaps_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, mergedFlatLsnData);
    mergedFlatLsnData = NULL;

    /*
     * Acquire the resources
     */
    errNum =
        kaps_lsn_mc_acquire_resources(newMergedLsn, newMergedLsn->m_nLsnCapacity, newMergedLsn->m_numLpuBricks,
                                      &reason);
    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(newMergedLsn, &reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    /*
     * Acquire the Per LPU resources
     */
    if (settings_p->m_isPerLpuGran)
    {
        iterBrick = newMergedLsn->m_lpuList;
        brickNum = 0;

        while (iterBrick)
        {
            errNum = kaps_lsn_mc_acquire_resources_per_lpu(newMergedLsn, iterBrick, brickNum, iterBrick->m_maxCapacity,
                                                           iterBrick->ad_db, &reason);
            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_free_resources(newMergedLsn);
                kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
                return NLMERR_OK;
            }

            if (newMergedLsn->m_pSettings->m_hasSpecialEntry && iterBrick->m_hasReservedSlot
                && iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1]
                && iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1]->m_backPtr->ad_handle)
            {
                struct kaps_ad_db *e_ad_db;
                kaps_status status;

                KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(settings_p->m_device,
                                                  iterBrick->m_pfxes[iterBrick->m_maxCapacity -
                                                                     1]->m_backPtr->ad_handle, e_ad_db);

                if (e_ad_db->db_info.hw_res.ad_res->is_1_1)
                {
                    status =
                        kaps_ix_mgr_alloc(newMergedLsn->m_pTrie->m_ixMgr, e_ad_db, 1, newMergedLsn, IX_USER_LSN,
                                          &newMergedLsn->m_ixInfo_specialEntry);
                    if (status != KAPS_OK)
                    {
                        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
                        return NLMERR_OK;
                    }
                }
            }

            brickNum++;
            iterBrick = iterBrick->m_next_p;
        }

        newMergedLsn->m_nNumIxAlloced = newMergedLsn->m_nLsnCapacity;
    }

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(newMergedLsn, &reason);

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(oldLsn1);
    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(oldLsn2);

    kaps_lsn_mc_store_old_lsn_info(oldLsn1, 0);
    kaps_lsn_mc_store_old_lsn_info(oldLsn2, 1);

    if (numExtraPfxFromIptParent)
    {
        kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(iptAncestorNode->m_lsn_p);
        kaps_lsn_mc_store_old_lsn_info(iptAncestorNode->m_lsn_p, 2);
    }

    if (trie_p->m_trie_global->m_isIITLmpsofar)
    {
        if (iptAncestorNode)
        {
            kaps_trie_ipt_ancestor_iit_lmpsofar(newIptTrieNode, iptAncestorNode->m_lsn_p);
        }
        else
        {
            newIptTrieNode->m_iitLmpsofarPfx_p = NULL;
        }
    }

    /*
     * Commit the New Merged LSN
     */
    errNum = kaps_lsn_mc_commit(newMergedLsn, 0, &reason);
    if (errNum != NLMERR_OK)
    {
        newIptTrieNode->m_iitLmpsofarPfx_p = NULL;
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    kaps_lsn_mc_destroy(newIptTrieNode->m_lsn_p);
    newIptTrieNode->m_lsn_p = newMergedLsn;

    if (!newIptTrieNode->m_rptParent_p)
        newIptTrieNode->m_rptParent_p = trieNodeToMerge1->m_rptParent_p;

    /*
     * Commit IIT will happen in AddSes function itself
     */
    errNum = kaps_trie_add_ses(trie_p, newIptTrieNode, newIptTrieNode->m_lp_prefix_p, &reason);
    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (toDeletePfxCopy)
    {
        if (appCB)
        {
            appCB(settings_p->m_pAppData, tbl, toDeletePfxCopy, toDeletePfxCopy->m_nIndex, KAPS_LSN_NEW_INDEX);
        }
        kaps_pfx_bundle_destroy(toDeletePfxCopy, trie_p->m_trie_global->m_alloc_p);
    }

    newMergedLsn->m_bIsNewLsn = 0;

    /*
     * The oldLsn1 and oldLsn2 will be destroyed when InPlaceDelete is called
     */
    kaps_trie_pvt_in_place_delete(trie_p, trieNodeToMerge1, 0, &reason);
    kaps_trie_pvt_in_place_delete(trie_p, trieNodeToMerge2, 0, &reason);

    /*
     * Delete the entries (both in hardware and software) in the IPT Ancestor
     */
    if (iptAncestorNode && numExtraPfxFromIptParent)
    {
        kaps_trie_clear_holes_in_split_ipt_ancestor(trie_p, iptAncestorNode, NULL,
                                                    trie_p->m_trie_global->m_pfxLocationsToMove,
                                                    numExtraPfxFromIptParent, &reason);
    }

    trie_p->m_tbl_ptr->m_fibStats.numRelatedLsnMerges++;

    return NLMERR_OK;
}

void
kaps_trie_calc_trie_lsn_stats(
    kaps_trie_node * node_p,
    struct NlmFibStats *stats_p)
{
    kaps_lsn_mc *lsn_p = NULL;
    kaps_lpm_lpu_brick *curLpuBrick;
    uint32_t numRptBitsLoppedOff;
    uint32_t trigger_len;
    kaps_trie_node *rptParent;
    struct kaps_device *device;
    struct kaps_db *db;

    if (!node_p)
        return;

    device = node_p->m_trie_p->m_trie_global->fibtblmgr->m_devMgr_p;
    db = node_p->m_trie_p->m_tbl_ptr->m_db;

    stats_p->numTrieNodes++;

    if (node_p->m_isRptNode)
    {
        stats_p->numRPTEntries++;
        numRptBitsLoppedOff = kaps_trie_get_num_rpt_bits_lopped_off(device, node_p->m_depth);
        stats_p->totalRptBytesLoppedOff += numRptBitsLoppedOff / 8;
        stats_p->rpt_lopoff_info[numRptBitsLoppedOff / 8]++;
    }

    if (node_p->m_aptLmpsofarPfx_p)
    {
        stats_p->numLmpsofarPfx++;
        stats_p->numAPTLmpsofarEntries++;
    }

    if (node_p->m_rptAptLmpsofarPfx)
    {
        stats_p->numLmpsofarPfx++;
        stats_p->numAPTLmpsofarEntries++;
    }

    if (node_p->m_numRptSplits > stats_p->maxNumRptSplitsInANode)
    {
        stats_p->maxNumRptSplitsInANode = node_p->m_numRptSplits;
        stats_p->depthofNodeWithMaxRptSplits = node_p->m_depth;
        stats_p->idOfNodeWithMaxRptSplits = node_p->m_trienodeId;
    }

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        stats_p->numIPTEntries++;

        rptParent = node_p->m_rptParent_p;

        if (rptParent)
        {
            trigger_len = node_p->m_depth - kaps_trie_get_num_rpt_bits_lopped_off(device, rptParent->m_depth);

            if (db->num_algo_levels_in_db == 3)
            {
                trigger_len += 1;   /* Add 1 for MPE */
            }


            if (trigger_len <= 40)
            {
                stats_p->num40bTriggers++;
            }
            else if (trigger_len <= 80)
            {
                stats_p->num80bTriggers++;
            }
            else
            {
                stats_p->num160bTriggers++;
            }

            stats_p->avgTrigLen += trigger_len;
        }

        lsn_p = node_p->m_lsn_p;
        curLpuBrick = lsn_p->m_lpuList;
        if (lsn_p->m_numLpuBricks)
            stats_p->numLsnForEachSize[lsn_p->m_numLpuBricks - 1]++;
        while (curLpuBrick)
        {
            stats_p->numBricksForEachGran[curLpuBrick->m_granIx]++;
            stats_p->numHolesForEachGran[curLpuBrick->m_granIx] += curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            if (lsn_p->m_numLpuBricks)
                stats_p->numHolesForEachLsnSize[lsn_p->m_numLpuBricks - 1] +=
                    curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            stats_p->numPfxForEachGran[curLpuBrick->m_granIx] += curLpuBrick->m_numPfx;
            stats_p->totalNumHolesInLsns += curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            stats_p->numUsedBricks++;

            if (curLpuBrick->m_numPfx == 0)
            {
                stats_p->numUsedButEmptyBricks++;
                stats_p->numEmptyBricksForEachGran[curLpuBrick->m_granIx]++;
            }

            if (curLpuBrick->m_numPfx > 0 && curLpuBrick->m_numPfx < curLpuBrick->m_maxCapacity)
            {
                stats_p->numPartiallyOccupiedBricks++;
            }

            curLpuBrick = curLpuBrick->m_next_p;
        }
        if (node_p->m_iitLmpsofarPfx_p)
            stats_p->numLmpsofarPfx++;
    }

    if (node_p->m_child_p[0])
        kaps_trie_calc_trie_lsn_stats(node_p->m_child_p[0], stats_p);

    if (node_p->m_child_p[1])
        kaps_trie_calc_trie_lsn_stats(node_p->m_child_p[1], stats_p);
}

void
kaps_trie_calc_kaps_small_bb_stats(
    kaps_fib_tbl * fibTbl,
    struct kaps_small_bb_stats *stats,
    uint32_t doVerify)
{
    uint32_t poolIter = 0, granIx;
    kaps_pool_info *curPoolInfo;
    kaps_trie_global *trieGlobal_p = fibTbl->m_fibTblMgr_p->m_trieGlobal;
    kaps_pool_mgr *poolMgr = trieGlobal_p->poolMgr;
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) fibTbl->m_db;
    struct kaps_device *device = lpm_db->db_info.device;
    uint32_t numFreeBricksInBitmap;
    uint32_t i, j;
    struct kaps_ab_info *ab;
    uint32_t cur_brick_nr, cur_brick_gran;
    uint32_t maxNumPfxPerBrick, numPfxInBrick;

    if (lpm_db->db_info.parent)
        lpm_db = (struct kaps_lpm_db *) lpm_db->db_info.parent;

    if (!stats)
    {
        stats = &fibTbl->m_fibStats.kaps_small_bb_stats;
        kaps_memset(stats, 0, sizeof(struct kaps_small_bb_stats));
    }

    stats->maxLsnWidth = lpm_db->db_info.hw_res.db_res->max_num_bricks_per_ab;

    stats->maxBricks = lpm_db->db_info.hw_res.db_res->num_small_bb_per_row * KAPS_NUM_ROWS_IN_SMALL_BB;

    for (poolIter = 0; poolIter < poolMgr->m_numActiveIptPools; ++poolIter)
    {
        curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIter];

        if (!curPoolInfo->m_isInUse)
            continue;

        ab = curPoolInfo->m_dba_mgr->m_ab_info;

        stats->totalNumEntriesInLsns += curPoolInfo->m_curNumDbaEntries;
        stats->totalNumHolesInLsns += (curPoolInfo->m_maxNumDbaEntries - curPoolInfo->m_curNumDbaEntries);

        for (cur_brick_nr = 0; cur_brick_nr < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks;
             ++cur_brick_nr)
        {
            cur_brick_gran = device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].sub_ab_gran;

            granIx = fibTbl->m_trie->m_lsn_settings_p->m_middleLevelLengthToGranIx[cur_brick_gran];

            maxNumPfxPerBrick = fibTbl->m_trie->m_lsn_settings_p->m_maxPfxInBrickForGranIx[granIx];

            numPfxInBrick = 0;
            for (i = 0; i < maxNumPfxPerBrick; ++i)
            {
                if (kaps_array_check_bit
                    (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].free_slot_bmp, i))
                {
                    /*
                     * Do Nothing
                     */
                }
                else
                {
                    numPfxInBrick++;
                }
            }

            stats->numHolesForEachGran[granIx] += maxNumPfxPerBrick - numPfxInBrick;
            stats->numPfxForEachGran[granIx] += numPfxInBrick;
            stats->numBricksForEachGran[granIx]++;
        }
        stats->numUsedBricks += device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks;

        stats->numLsnForEachSize[device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks - 1]++;
        stats->numHolesForEachLsnSize[device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks - 1] +=
            (curPoolInfo->m_maxNumDbaEntries - curPoolInfo->m_curNumDbaEntries);
    }

    stats->numFreeBricks = stats->maxBricks - stats->numUsedBricks;

    if (doVerify)
    {
        numFreeBricksInBitmap = 0;
        for (i = 0; i < KAPS_SMALL_BB_BMP_8; ++i)
        {
            for (j = 0; j < KAPS_BITS_IN_BYTE; ++j)
            {
                if (lpm_db->db_info.hw_res.db_res->small_bb_brick_bitmap[i] & (1u << j))
                {
                    /*
                     * The bit is set to 1, which means it is a free brick
                     */
                    numFreeBricksInBitmap++;
                }
            }
        }

        if (numFreeBricksInBitmap != stats->numFreeBricks)
        {
            kaps_assert(0, "Incorrect Small BB bitmap \n");
        }
    }
}

NlmErrNum_t
kaps_trie_pvt_undo_ancestor_lsn_merge(
    kaps_lpm_trie * trie_p,
    kaps_lsn_mc * tempMergedLsn,
    kaps_lsn_mc * newMergedLsn,
    kaps_flat_lsn_data * mergedFlatLsnData)
{
    kaps_nlm_allocator *alloc = trie_p->m_trie_global->m_alloc_p;

    if (tempMergedLsn)
    {
        kaps_nlm_allocator_free(alloc, (void *) tempMergedLsn);
    }

    if (newMergedLsn)
    {
        kaps_lsn_mc_destroy(newMergedLsn);
    }

    if (mergedFlatLsnData)
    {
        kaps_lsn_mc_destroy_flat_lsn_data(alloc, mergedFlatLsnData);
    }

    return NLMERR_OK;
}

void
kaps_trie_pvt_find_longest_pfx_len(
    kaps_lsn_mc * curLsn,
    uint32_t * longestPfxLen_p)
{
    uint32_t i;
    kaps_lpm_lpu_brick *curBrick = curLsn->m_lpuList;

    while (curBrick)
    {
        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            kaps_pfx_bundle *curPfx = curBrick->m_pfxes[i];
            if (curPfx && curPfx->m_nPfxSize > *longestPfxLen_p)
                *longestPfxLen_p = curPfx->m_nPfxSize;
        }
        curBrick = curBrick->m_next_p;
    }
}

NlmErrNum_t
kaps_trie_pvt_merge_ancestor_lsns(
    kaps_lpm_trie * trie_p,
    kaps_trie_node * downStreamNode_p,
    NlmReasonCode * o_reason)
{
    kaps_trie_node *iterNode_p, *upStreamNode_p;
    kaps_lsn_mc *upStreamLsn, *downStreamLsn;
    kaps_lsn_mc_settings *settings_p = trie_p->m_lsn_settings_p;
    kaps_fib_prefix_index_changed_app_cb_t appCB = settings_p->m_pAppCB;
    kaps_lpm_lpu_brick *iterBrick, *lastUpStreamBrick;
    kaps_flat_lsn_data *mergedFlatLsnData = NULL;
    kaps_lsn_mc *tempMergedLsn = NULL, *newMergedLsn = NULL;
    NlmErrNum_t errNum;
    uint32_t i, j, brickNum;
    kaps_pfx_bundle *toDeletePfxCopy = NULL, *curPfxBundle;
    uint32_t doesLsnFit;
    kaps_fib_tbl *tbl;

    if (!settings_p->m_isAncestorLsnMergeEnabled)
        return NLMERR_OK;

    if (downStreamNode_p->m_isRptNode)
        return NLMERR_OK;

    if (trie_p->m_tbl_ptr->m_db->is_destroy_in_progress)
        return NLMERR_OK;

    iterNode_p = downStreamNode_p->m_parent_p;
    upStreamNode_p = NULL;
    while (iterNode_p)
    {
        if (iterNode_p->m_node_type == NLMNSTRIENODE_LP)
        {
            upStreamNode_p = iterNode_p;
            break;
        }

        if (iterNode_p->m_isRptNode)
            break;

        iterNode_p = iterNode_p->m_parent_p;
    }

    if (!upStreamNode_p)
        return NLMERR_OK;

    upStreamLsn = upStreamNode_p->m_lsn_p;
    tbl = upStreamLsn->m_pTbl;

    if (!upStreamLsn->m_lpuList)
        return NLMERR_OK;

    /*
     * Let us say that expansion depth (= 8) and trie lopoff (=0) are different. Suppose there is a downstream LSN at
     * depth 16 with a /24 prefix. We want to merge it with the root. If we allow this to happen, then /24 prefix will
     * reside in the LSN at the root. Later on when we add a /20 entry in the same path, FindDestLpNode will return a
     * new LSN at the expansion depth of 8. This will cause a problem since now /24 should move to the new LSN at depth 
     * 8 To avoid this problem, avoid merging if the upstream LSN has a depth < expansion depth
     */
    if (upStreamLsn->m_nDepth < trie_p->m_expansion_depth)
        return NLMERR_OK;

    downStreamLsn = downStreamNode_p->m_lsn_p;

    if (!downStreamLsn->m_lpuList)
        return NLMERR_OK;

    if (!settings_p->m_isPerLpuGran)
    {
        uint32_t totalNumPfx = downStreamLsn->m_nNumPrefixes + upStreamLsn->m_nNumPrefixes;
        uint32_t longestPfxLen, longestPfxGran, maxPfxPerBrick, numBricksNeeded;

        longestPfxLen = 0;
        kaps_trie_pvt_find_longest_pfx_len(downStreamLsn, &longestPfxLen);
        kaps_trie_pvt_find_longest_pfx_len(upStreamLsn, &longestPfxLen);

        longestPfxLen -= upStreamLsn->m_nDepth;

        longestPfxGran = kaps_lsn_mc_compute_gran(settings_p, longestPfxLen, NULL);
        maxPfxPerBrick = kaps_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, NULL, 0, 0xf, longestPfxGran);
        numBricksNeeded = (totalNumPfx + maxPfxPerBrick - 1) / maxPfxPerBrick;

        if (numBricksNeeded >= settings_p->m_maxLpuPerLsn / 2)
            return NLMERR_OK;

    }
    else
    {
        if (downStreamLsn->m_numLpuBricks + upStreamLsn->m_numLpuBricks >= (settings_p->m_maxLpuPerLsn + 1) / 2)
            return NLMERR_OK;
    }

    mergedFlatLsnData = kaps_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, o_reason);
    if (!mergedFlatLsnData)
    {
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    /*
     * Create the temporary merged LSN
     */
    tempMergedLsn = kaps_lsn_mc_create(settings_p, trie_p, upStreamLsn->m_nDepth);
    if (!tempMergedLsn)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    /*
     * Find the last brick in the upstream LSN
     */
    lastUpStreamBrick = NULL;
    iterBrick = upStreamLsn->m_lpuList;
    while (iterBrick)
    {
        lastUpStreamBrick = iterBrick;
        iterBrick = iterBrick->m_next_p;
    }

    /*
     * Link the upstream list and the downstream list
     */
    lastUpStreamBrick->m_next_p = downStreamLsn->m_lpuList;

    /*
     * Make the temporary merged LSN point to the merged LPU List
     */
    tempMergedLsn->m_lpuList = upStreamLsn->m_lpuList;
    tempMergedLsn->m_numLpuBricks = upStreamLsn->m_numLpuBricks + downStreamLsn->m_numLpuBricks;
    tempMergedLsn->m_nLsnCapacity = upStreamLsn->m_nLsnCapacity + downStreamLsn->m_nLsnCapacity;
    tempMergedLsn->m_nNumPrefixes = upStreamLsn->m_nNumPrefixes + downStreamLsn->m_nNumPrefixes;
    tempMergedLsn->m_pParentHandle = upStreamLsn->m_pParentHandle;

    /*
     * Convert the temporary merged LSN into Flat LSN Data
     */
    errNum = kaps_lsn_mc_convert_lsn_to_flat_data(tempMergedLsn, NULL, 0, mergedFlatLsnData, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        lastUpStreamBrick->m_next_p = NULL;
        return NLMERR_OK;
    }

    /*
     * Unlink the upstream bricks from the downstream bricks
     */
    lastUpStreamBrick->m_next_p = NULL;

    /*
     * Create the new Merged LSN
     */
    newMergedLsn = kaps_lsn_mc_create(settings_p, trie_p, upStreamLsn->m_nDepth);
    if (!newMergedLsn)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    newMergedLsn->m_pParentHandle = upStreamLsn->m_pParentHandle;

    /*
     * if both the LSN are having reserved slots free one of them 
     */
    for (i = 0; i < mergedFlatLsnData->m_numPfxInLsn; i++)
    {
        curPfxBundle = mergedFlatLsnData->m_pfxesInLsn[i];

        if (curPfxBundle->m_isPfxCopy && curPfxBundle->m_nPfxSize > newMergedLsn->m_nDepth)
        {
            toDeletePfxCopy = curPfxBundle;
            mergedFlatLsnData->m_pfxesInLsn[i] = NULL;
            for (j = i + 1; j < mergedFlatLsnData->m_numPfxInLsn; j++, i++)
            {
                mergedFlatLsnData->m_pfxesInLsn[i] = mergedFlatLsnData->m_pfxesInLsn[j];
                mergedFlatLsnData->pfx_color[i] = mergedFlatLsnData->pfx_color[j];
            }
            mergedFlatLsnData->m_numPfxInLsn--;
            break;
        }
    }

    /*
     * Convert the Flat LSN Data to new merged LSN
     */
    errNum = kaps_lsn_mc_convert_flat_data_to_lsn(mergedFlatLsnData, newMergedLsn, &doesLsnFit, o_reason);
    if (errNum != NLMERR_OK || !doesLsnFit)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    /*
     * Destroy the temporary LSN and the temporary Flat LSN Data
     */
    kaps_nlm_allocator_free(settings_p->m_pAlloc, (void *) tempMergedLsn);
    tempMergedLsn = NULL;

    kaps_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, mergedFlatLsnData);
    mergedFlatLsnData = NULL;

    /*
     * If there are too many bricks in the new merged LSN, then simply return
     */
    if (!settings_p->m_isFullWidthLsn)
    {
        if (newMergedLsn->m_numLpuBricks >= (settings_p->m_maxLpuPerLsn + 1) / 2)
        {
            kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
            return NLMERR_OK;
        }
    }

    /*
     * Acquire the resources
     */
    errNum =
        kaps_lsn_mc_acquire_resources(newMergedLsn, newMergedLsn->m_nLsnCapacity, newMergedLsn->m_numLpuBricks,
                                      o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_lsn_mc_rearrange_prefixes_for_joined_udcs(newMergedLsn, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    /*
     * Acquire the Per LPU resources
     */
    if (settings_p->m_isPerLpuGran)
    {
        iterBrick = newMergedLsn->m_lpuList;
        brickNum = 0;

        while (iterBrick)
        {
            errNum = kaps_lsn_mc_acquire_resources_per_lpu(newMergedLsn, iterBrick, brickNum, iterBrick->m_maxCapacity,
                                                           iterBrick->ad_db, o_reason);
            if (errNum != NLMERR_OK)
            {
                kaps_lsn_mc_free_resources(newMergedLsn);
                kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
                return NLMERR_OK;
            }

            if (newMergedLsn->m_pSettings->m_hasSpecialEntry && iterBrick->m_hasReservedSlot
                && iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1]
                && iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1]->m_backPtr->ad_handle)
            {
                struct kaps_ad_db *e_ad_db;
                kaps_status status;

                KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(settings_p->m_device,
                                                  iterBrick->m_pfxes[iterBrick->m_maxCapacity -
                                                                     1]->m_backPtr->ad_handle, e_ad_db);

                if (e_ad_db->db_info.hw_res.ad_res->is_1_1)
                {
                    status =
                        kaps_ix_mgr_alloc(newMergedLsn->m_pTrie->m_ixMgr, e_ad_db, 1, newMergedLsn, IX_USER_LSN,
                                          &newMergedLsn->m_ixInfo_specialEntry);
                    if (status != KAPS_OK)
                    {
                        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
                        return NLMERR_OK;
                    }
                }
            }

            brickNum++;
            iterBrick = iterBrick->m_next_p;
        }

        newMergedLsn->m_nNumIxAlloced = newMergedLsn->m_nLsnCapacity;
    }

    kaps_lsn_mc_add_extra_brick_for_joined_udcs(newMergedLsn, o_reason);

    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(upStreamLsn);
    kaps_lsn_mc_get_hit_bits_for_lsn_from_hw(downStreamLsn);

    kaps_lsn_mc_store_old_lsn_info(upStreamLsn, 0);
    kaps_lsn_mc_store_old_lsn_info(downStreamLsn, 1);

    /*
     * Commit the New Merged LSN
     */
    errNum = kaps_lsn_mc_commit(newMergedLsn, 0, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    /*
     * Commit the IIT
     */
    errNum = kaps_lsn_mc_update_iit(newMergedLsn, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (toDeletePfxCopy)
    {
        if (appCB)
        {
            appCB(settings_p->m_pAppData, tbl, toDeletePfxCopy, toDeletePfxCopy->m_nIndex, KAPS_LSN_NEW_INDEX);
        }
        kaps_pfx_bundle_destroy(toDeletePfxCopy, trie_p->m_trie_global->m_alloc_p);
    }

    newMergedLsn->m_bIsNewLsn = 0;

    kaps_lsn_mc_destroy(upStreamLsn);

    upStreamNode_p->m_lsn_p = newMergedLsn;

    kaps_trie_pvt_in_place_delete(trie_p, downStreamNode_p, 1, o_reason);

    trie_p->m_tbl_ptr->m_fibStats.numLsnMerges++;

    return NLMERR_OK;
}

void
kaps_trie_update_hit_bits_iit_lmpsofar(
    kaps_trie_node * node_p)
{
    if (!node_p)
        return;

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        if (node_p->m_iitLmpsofarPfx_p)
        {
            struct kaps_device *device = node_p->m_trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
            struct kaps_hb *hb_entry = NULL;

            if (node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
            {
                kaps_lpm_trie *trie_p = node_p->m_trie_p;
                struct kaps_db *db;
                struct kaps_aging_entry *active_aging_table;

                KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, node_p->m_iitLmpsofarPfx_p->m_backPtr, db)
                    KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                            (uintptr_t) node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

                active_aging_table = kaps_device_get_active_aging_table(device, db);

                if (trie_p->m_iptHitBitBuffer)
                {
                    if (db->num_algo_levels_in_db == 2)
                    {
                        if (trie_p->m_iptHitBitBuffer[node_p->m_lp_prefix_p->m_nIndex])
                        {
                            active_aging_table[hb_entry->bit_no].num_idles = 0;
                        }
                    }
                    else
                    {
                        kaps_ipm *ipm =
                            kaps_pool_mgr_get_ipm_for_pool(trie_p->m_trie_global->poolMgr, node_p->m_poolId);
                        struct kaps_ab_info *ab = ipm->m_ab_info;
                        uint32_t cur_brick_nr;
                        uint32_t brickInAB = 0, pfxIndexInBkt = 0, found;
                        uint32_t small_bb_num, rowNr;
                        uint32_t indexInBuffer;

                        found = 0;
                        for (cur_brick_nr = 0;
                             cur_brick_nr < device->kaps_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++cur_brick_nr)
                        {
                            if (device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index
                                <= node_p->m_lp_prefix_p->m_nIndex
                                && node_p->m_lp_prefix_p->m_nIndex <=
                                device->kaps_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].upper_index)
                            {

                                brickInAB = cur_brick_nr;
                                pfxIndexInBkt = node_p->m_lp_prefix_p->m_nIndex -
                                    device->kaps_shadow->ab_to_small_bb[ab->ab_num].
                                    sub_ab_bricks[cur_brick_nr].lower_index;

                                found = 1;
                                break;
                            }
                        }

                        if (!found)
                        {
                            kaps_assert(0, "Unable to find the prefix index while updating Lmpsofar hit bits\n");
                            return;
                        }

                        small_bb_num =
                            device->kaps_shadow->ab_to_small_bb[ipm->m_ab_info->ab_num].
                            sub_ab_bricks[brickInAB].small_bb_num;
                        rowNr =
                            device->kaps_shadow->ab_to_small_bb[ipm->m_ab_info->ab_num].
                            sub_ab_bricks[brickInAB].row_num;

                        indexInBuffer = rowNr * db->hw_res.db_res->num_small_bb_per_row * KAPS_HB_ROW_WIDTH_1;
                        indexInBuffer += (small_bb_num - db->hw_res.db_res->start_small_bb_nr) * KAPS_HB_ROW_WIDTH_1;
                        indexInBuffer += pfxIndexInBkt;

                        if (trie_p->m_iptHitBitBuffer[indexInBuffer])
                        {
                            active_aging_table[hb_entry->bit_no].num_idles = 0;
                        }
                    }
                }
            }
        }
    }

    if (node_p->m_isRptNode)
    {
        if (node_p->m_iitLmpsofarPfx_p)
        {
            struct kaps_device *device = node_p->m_trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
            struct kaps_hb *hb_entry = NULL;

            if (node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
            {
                kaps_lpm_trie *trie_p = node_p->m_trie_p;
                struct kaps_db *db;
                struct kaps_aging_entry *active_aging_table;

                KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, node_p->m_iitLmpsofarPfx_p->m_backPtr, db)
                    KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                            (uintptr_t) node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

                active_aging_table = kaps_device_get_active_aging_table(device, db);

                if (trie_p->m_rptTcamHitBitBuffer)
                {
                    if (trie_p->m_rptTcamHitBitBuffer[node_p->m_rpt_prefix_p->m_nIndex])
                    {
                        active_aging_table[hb_entry->bit_no].num_idles = 0;
                    }
                }
            }
        }
    }

    if (node_p->m_child_p[0])
        kaps_trie_update_hit_bits_iit_lmpsofar(node_p->m_child_p[0]);

    if (node_p->m_child_p[1])
        kaps_trie_update_hit_bits_iit_lmpsofar(node_p->m_child_p[1]);
}

NlmErrNum_t
kaps_trie_process_hit_bits_iit_lmpsofar(
    kaps_lpm_trie * trie_p)
{
    kaps_lsn_mc_settings *settings = trie_p->m_lsn_settings_p;
    struct kaps_device *device = settings->m_device;
    kaps_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t num_rows_to_read = (device->num_80b_entries_ab / 2) / KAPS_HB_ROW_WIDTH_1;
    struct kaps_ab_info *ab;
    uint32_t i, j, pos, offset;
    struct kaps_db *db = trie_p->m_tbl_ptr->m_db;
    uint16_t value;

    if (settings->m_areIPTHitBitsPresent)
    {
        if (db->num_algo_levels_in_db == 2)
        {

            kaps_ipm *ipm = kaps_pool_mgr_get_ipm_for_pool(poolMgr, trie_p->m_roots_trienode_p->m_poolId);

            if (trie_p->m_iptHitBitBuffer)
                kaps_memset(trie_p->m_iptHitBitBuffer, 0, device->num_80b_entries_ab / 2);

            ab = ipm->m_ab_info;
            while (ab)
            {
                kaps_dm_kaps_hb_dump(device, db, device->dba_offset + ab->ab_num, 0,
                                     device->dba_offset + ab->ab_num, num_rows_to_read - 1, 1, device->hb_buffer);

                if (trie_p->m_iptHitBitBuffer)
                {
                    pos = 0;
                    offset = 0;
                    for (i = 0; i < num_rows_to_read; ++i)
                    {
                        value = KapsReadBitsInArrray(&device->hb_buffer[offset], KAPS_HB_ROW_WIDTH_8,
                                                 KAPS_HB_ROW_WIDTH_1 - 1, 0);

                        for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                        {
                            if (value & (1u << j))
                            {
                                trie_p->m_iptHitBitBuffer[pos] = 1;
                            }
                            ++pos;
                        }
                        offset += KAPS_HB_ROW_WIDTH_8;
                    }
                }

                ab = ab->dup_ab;
            }
        }
        else
        {
            uint32_t start_dump_blk, end_dump_blk;
            uint32_t loop_cnt, num_loops;
            uint32_t blk_num, cur_row;

            if (trie_p->m_iptHitBitBuffer)
            {
                kaps_memset(trie_p->m_iptHitBitBuffer, 0,
                            (device->hw_res->total_small_bb / 2) * device->hw_res->num_rows_in_small_bb *
                            KAPS_HB_ROW_WIDTH_1);
            }

            num_rows_to_read = device->hw_res->num_rows_in_small_bb;

            num_loops = 1;
            if (device->is_small_bb_on_second_half)
            {
                num_loops = 2;
            }

            for (loop_cnt = 0; loop_cnt < num_loops; ++loop_cnt)
            {

                start_dump_blk = device->small_bb_offset + db->hw_res.db_res->start_small_bb_nr;
                end_dump_blk = device->small_bb_offset + db->hw_res.db_res->end_small_bb_nr;

                if (loop_cnt == 1)
                {
                    start_dump_blk += (device->hw_res->total_small_bb / 2);
                    end_dump_blk += (device->hw_res->total_small_bb / 2);
                }

                kaps_dm_kaps_hb_dump(device, db, start_dump_blk, 0,
                                     end_dump_blk, num_rows_to_read - 1, 1, device->hb_buffer);

                offset = 0;
                for (blk_num = db->hw_res.db_res->start_small_bb_nr; blk_num <= db->hw_res.db_res->end_small_bb_nr;
                     ++blk_num)
                {

                    for (cur_row = 0; cur_row < num_rows_to_read; ++cur_row)
                    {
                        value = KapsReadBitsInArrray(&device->hb_buffer[offset], KAPS_HB_ROW_WIDTH_8,
                                                 KAPS_HB_ROW_WIDTH_1 - 1, 0);

                        pos = (cur_row * db->hw_res.db_res->num_small_bb_per_row * KAPS_HB_ROW_WIDTH_1)
                            + ((blk_num - db->hw_res.db_res->start_small_bb_nr) * KAPS_HB_ROW_WIDTH_1);

                        for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                        {
                            if (value & (1u << j))
                            {
                                kaps_sassert(trie_p->m_iptHitBitBuffer != NULL);
                                trie_p->m_iptHitBitBuffer[pos] = 1;
                            }
                            ++pos;
                        }

                        offset += KAPS_HB_ROW_WIDTH_8;
                    }
                }
            }
        }
    }

    if (settings->m_areRPTHitBitsPresent)
    {
        uint64_t rptMap;
        uint32_t rptNr;

        num_rows_to_read = (device->num_80b_entries_ab / 2) / KAPS_HB_ROW_WIDTH_1;

        if (trie_p->m_rptTcamHitBitBuffer)
            kaps_memset(trie_p->m_rptTcamHitBitBuffer, 0, device->num_80b_entries_ab / 2);

        rptMap = db->hw_res.db_res->rpt_map[0];

        for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
        {
            if (rptMap & (1ULL << rptNr))
            {
                kaps_dm_kaps_hb_dump(device, db, device->dba_offset + rptNr, 0,
                                     device->dba_offset + rptNr, num_rows_to_read - 1, 1, device->hb_buffer);
            }

            if (trie_p->m_rptTcamHitBitBuffer)
            {
                pos = 0;
                offset = 0;

                for (i = 0; i < num_rows_to_read; ++i)
                {
                    value = KapsReadBitsInArrray(&device->hb_buffer[offset], KAPS_HB_ROW_WIDTH_8,
                                             KAPS_HB_ROW_WIDTH_1 - 1, 0);

                    for (j = 0; j < KAPS_HB_ROW_WIDTH_1; ++j)
                    {
                        if (value & (1u << j))
                        {

                            if (pos >= device->num_80b_entries_ab / 2)
                                kaps_assert(0,
                                            "Incorrect RPB hit bit while copying hardware hit bits into software buffer\n");

                            trie_p->m_rptTcamHitBitBuffer[pos] = 1;
                        }
                        ++pos;
                    }

                    offset += KAPS_HB_ROW_WIDTH_8;
                }
            }
        }
    }

    if (settings->m_areIPTHitBitsPresent || settings->m_areRPTHitBitsPresent)
    {
        kaps_trie_update_hit_bits_iit_lmpsofar(trie_p->m_roots_trienode_p);
    }

    return NLMERR_OK;
}

void
kaps_trie_get_algo_hit_bit_from_hw(
    kaps_lpm_trie * trie,
    kaps_trie_node * curNode,
    uint32_t clear_on_read,
    uint32_t * bit_value)
{
    kaps_pool_mgr *poolMgr = trie->m_trie_global->poolMgr;
    struct kaps_device *device = trie->m_lsn_settings_p->m_device;
    struct kaps_ab_info *ab;
    kaps_ipm *ipm;
    kaps_fib_tbl_mgr *fibTblMgr = trie->m_trie_global->fibtblmgr;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    kaps_status status;
    struct kaps_db *db = trie->m_tbl_ptr->m_db;

    if (db->num_algo_levels_in_db == 3)
    {
        uint8_t hb_value = 0;

        ipm = kaps_pool_mgr_get_ipm_for_pool(poolMgr, curNode->m_rptParent_p->m_poolId);
        ab = ipm->m_ab_info;

        kaps_kaps_read_trigger_hb_in_small_bb(fibTblMgr, ab, curNode->m_lp_prefix_p->m_nIndex, &hb_value, &reason);

        if (hb_value)
        {
            *bit_value = 1;
        }

        if (clear_on_read)
        {
            hb_value = 0;

            kaps_kaps_write_trigger_hb_in_small_bb(fibTblMgr, ab, curNode->m_lp_prefix_p->m_nIndex, hb_value, &reason);
        }

    }
    else
    {
        uint8_t hb_data[KAPS_HB_ROW_WIDTH_8];

        ipm = kaps_pool_mgr_get_ipm_for_pool(poolMgr, trie->m_roots_trienode_p->m_poolId);
        ab = ipm->m_ab_info;

        status = kaps_dm_kaps_hb_read(device, db, device->dba_offset + ab->ab_num, curNode->m_lp_prefix_p->m_nIndex, hb_data);

        if (status != KAPS_OK)
        	return;

        if (hb_data[0] & (1u << 7))
            *bit_value = 1;

        if (clear_on_read)
        {
            kaps_memset(hb_data, 0, KAPS_HB_ROW_WIDTH_8);

            kaps_dm_kaps_hb_write(device, db, device->dba_offset + ab->ab_num, curNode->m_lp_prefix_p->m_nIndex, hb_data);
        }

    }
}

void
kaps_trie_explore_trie_for_algo_hit_bit(
    kaps_lpm_trie * trie,
    kaps_pfx_bundle * pfxBundle,
    kaps_trie_node * curNode,
    uint32_t clear_on_read,
    uint32_t * bit_value)
{
    if (curNode->m_node_type == NLMNSTRIENODE_LP)
    {
        if (curNode->m_iitLmpsofarPfx_p != pfxBundle)
        {
            return;
        }
        else
        {
            kaps_trie_get_algo_hit_bit_from_hw(trie, curNode, clear_on_read, bit_value);

            /*
             * If we don't have to clear on read and we have found one location where the hit bit is set, then
             * immediately return
             */
            if (*bit_value && !clear_on_read)
            {
                return;
            }
        }
    }

    if (curNode->m_child_p[0])
        kaps_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, curNode->m_child_p[0], clear_on_read, bit_value);

    /*
     * If we don't have to clear on read and we have found one location where the hit bit is set, then immediately
     * return
     */
    if (*bit_value && !clear_on_read)
    {
        return;
    }

    if (curNode->m_child_p[1])
        kaps_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, curNode->m_child_p[1], clear_on_read, bit_value);

}

NlmErrNum_t
kaps_trie_get_algo_hit_bit_value(
    kaps_lpm_trie * trie,
    struct kaps_lpm_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value)
{
    uint32_t i, val;
    kaps_pfx_bundle *pfxBundle = entry->pfx_bundle;
    kaps_trie_node *node = trie->m_roots_trienode_p;
    struct kaps_lpm_db *lpm_db = (struct kaps_lpm_db *) trie->m_tbl_ptr->m_db;

    if (!trie->m_lsn_settings_p->m_areIPTHitBitsPresent)
    {
        return NLMERR_OK;
    }

    /*
     * We don't want to clear the hit bit for the IPT for KAPS during entry delete as this can slow down the delete
     * speed
     */
    if (lpm_db->is_entry_delete_in_progress)
        clear_on_read = 0;

    /*
     * Don't change the bit_value incase the node is NULL since bit_value already contains the hit bit result from the
     * BB
     */
    if (!node)
        return NLMERR_OK;

    for (i = node->m_depth; i < pfxBundle->m_nPfxSize; i++)
    {
        val = KAPS_PREFIX_PVT_GET_BIT(pfxBundle->m_data, pfxBundle->m_nPfxSize, i);

        node = node->m_child_p[val];
        if (!node)
            break;
    }

    /*
     * Don't change the bit_value incase the node is NULL since bit_value already contains the hit bit result from the
     * BB
     */
    if (!node)
        return NLMERR_OK;

    /*
     * The current node is at the same depth as the prefix. The prefix can be Lmpsofar only for deeper nodes than the
     * current node. So explore the trie from the children of the current node
     */
    if (node->m_child_p[0])
        kaps_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, node->m_child_p[0], clear_on_read, bit_value);

    if (node->m_child_p[1])
        kaps_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, node->m_child_p[1], clear_on_read, bit_value);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_convert_kbp_status_to_err_num(
    kaps_status status,
    NlmReasonCode * o_reason_p)
{
    NlmErrNum_t errNum = NLMERR_OK;
    switch (status)
    {
        case KAPS_OK:
            errNum = NLMERR_OK;
            *o_reason_p = NLMRSC_REASON_OK;
            break;

        case KAPS_INVALID_ARGUMENT:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_INVALID_PARAM;
            break;

        case KAPS_OUT_OF_MEMORY:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_LOW_MEMORY;
            break;

        case KAPS_INTERNAL_ERROR:
            errNum = NLMRSC_INTERNAL_ERROR;
            *o_reason_p = NLMRSC_INTERNAL_ERROR;
            break;

        case KAPS_OUT_OF_UDA:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_UDA_ALLOC_FAILED;
            break;

        case KAPS_OUT_OF_DBA:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_DBA_ALLOC_FAILED;
            break;

        case KAPS_OUT_OF_INDEX:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_IDX_RANGE_FULL;
            break;

        case KAPS_OUT_OF_UIT:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_IT_ALLOC_FAILED;
            break;

        case KAPS_EXHAUSTED_PCM_RESOURCE:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_PCM_ALLOC_FAILED;
            break;

        case KAPS_OUT_OF_AD:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_OUT_OF_AD;
            break;

        default:
            errNum = NLMRSC_INTERNAL_ERROR;
            *o_reason_p = NLMRSC_INTERNAL_ERROR;
            break;
    }

    return errNum;
}

/* warmboot functions start here */

NlmErrNum_t
kaps_kaps_trie_wb_save_pools(
    kaps_trie_global * trie_g,
    struct kaps_wb_cb_functions * wb_fun)
{
    uint32_t num_active_pools;
    int32_t i;

    /*
     * Store number of active IPT and APT pool count 
     */
    num_active_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveIptPools; i++)
    {
        if (trie_g->poolMgr->m_ipmPoolInfo[i].m_isInUse)
            num_active_pools++;
    }
    if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                              sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    num_active_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveAptPools; i++)
    {
        if (trie_g->poolMgr->m_apmPoolInfo[i].m_isInUse)
            num_active_pools++;
    }

    if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                              sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    return kaps_pool_mgr_wb_save_pools(trie_g->poolMgr, wb_fun->write_fn, wb_fun->handle, wb_fun->nv_offset);
}

void
kaps_trie_cr_save_pools(
    kaps_trie_global * trie_g,
    struct kaps_wb_cb_functions *wb_fun)
{
    uint32_t *num_active_pools, num_pools;
    int32_t i;

    if (!trie_g->poolMgr->m_are_pools_updated)
    {
        return;
    }

    trie_g->poolMgr->m_are_pools_updated = 0;

    num_active_pools = (uint32_t *) wb_fun->nv_ptr;
    wb_fun->nv_ptr += sizeof(*num_active_pools);

    /*
     * Store number of active IPT and APT pool count 
     */
    num_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveIptPools; i++)
    {
        if (trie_g->poolMgr->m_ipmPoolInfo[i].m_isInUse)
            num_pools++;
    }
    *num_active_pools = num_pools;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

    num_active_pools = (uint32_t *) wb_fun->nv_ptr;
    wb_fun->nv_ptr += sizeof(*num_active_pools);

    num_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveAptPools; i++)
    {
        if (trie_g->poolMgr->m_apmPoolInfo[i].m_isInUse)
            num_pools++;
    }
    *num_active_pools = num_pools;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

    kaps_pool_mgr_cr_save_pools(trie_g->poolMgr, wb_fun);
}

kaps_status
kaps_kaps_trie_wb_save_rpt_ipt_and_apt_entries(
    kaps_trie_node * trie_node,
    uint32_t * num_rpt_entries,
    uint32_t * nv_offset,
    kaps_pool_mgr * pool_mgr,
    kaps_device_issu_write_fn write_fn,
    void *handle)
{
    kaps_pool_info *pool_info = NULL;
    uint32_t pool_ix, num_entries;

    if (trie_node == NULL)
        return 0;

    if (trie_node->m_isRptNode)
    {
        if (0 != kaps_trienode_wb_save_rpt(trie_node, nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        (*num_rpt_entries)++;

        pool_ix = KAPS_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(trie_node->m_poolId);
        if (pool_ix != KAPS_INVALID_POOL_INDEX)
        {
            pool_info = &pool_mgr->m_ipmPoolInfo[pool_ix];

            num_entries =
                kaps_ipm_wb_store_ipt_entries(pool_info->m_dba_mgr, trie_node->m_rptId, nv_offset, write_fn, handle);
            if (num_entries == -1)
                return KAPS_NV_READ_WRITE_FAILED;
            kaps_assert((num_entries == trie_node->m_numIptEntriesInSubtrie), "Num IPT entries mismatch");
        }

    }

    if (trie_node->m_child_p[0])
        KAPS_STRY(kaps_kaps_trie_wb_save_rpt_ipt_and_apt_entries(trie_node->m_child_p[0], num_rpt_entries, nv_offset,
                                                                 pool_mgr, write_fn, handle));

    if (trie_node->m_child_p[1])
        KAPS_STRY(kaps_kaps_trie_wb_save_rpt_ipt_and_apt_entries(trie_node->m_child_p[1], num_rpt_entries, nv_offset,
                                                                 pool_mgr, write_fn, handle));

    return KAPS_OK;
}

kaps_status
kaps_trie_wb_save(
    kaps_lpm_trie * trie_p,
    uint32_t * nv_offset)
{
    kaps_trie_global *trie_g = trie_p->m_trie_global;
    uint32_t num_rpts_offset, num_rpt_entries = 0;
    uint16_t no_of_regions;
    struct region region_info[MAX_UDA_REGIONS];

    if (0 != trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_are_reserved_160b_abs_initialized,
                                      sizeof(trie_g->m_are_reserved_160b_abs_initialized), *nv_offset))
    {
        return KAPS_NV_READ_WRITE_FAILED;
    }
    *nv_offset += sizeof(trie_g->m_are_reserved_160b_abs_initialized);

    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle,
                                 (uint8_t *) & trie_p->m_lsn_settings_p->m_numAllocatedBricksForAllLsns,
                                 sizeof(trie_p->m_lsn_settings_p->m_numAllocatedBricksForAllLsns), *nv_offset))
    {
        return KAPS_NV_READ_WRITE_FAILED;
    }
    *nv_offset += sizeof(trie_p->m_lsn_settings_p->m_numAllocatedBricksForAllLsns);

    if (0 != trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_p->m_lsn_settings_p->m_maxLpuPerLsn,
                                      sizeof(trie_p->m_lsn_settings_p->m_maxLpuPerLsn), *nv_offset))
    {
        return KAPS_NV_READ_WRITE_FAILED;
    }
    *nv_offset += sizeof(trie_p->m_lsn_settings_p->m_maxLpuPerLsn);

    /*
     * Store the number of regions in the UDA Mgr 
     */
    no_of_regions = trie_g->m_mlpmemmgr[0]->no_of_regions;
    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & no_of_regions, sizeof(no_of_regions),
                                 *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(no_of_regions);

    /*
     * Store the UDA Mgr region info 
     */
    kaps_memcpy(region_info, trie_g->m_mlpmemmgr[0]->region_info, sizeof(region_info));
    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & region_info, sizeof(region_info), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(region_info);

    /*
     * num_rpt_entries is written to NV afterwards. 
     */
    num_rpts_offset = *nv_offset;
    *nv_offset += sizeof(num_rpt_entries);

    KAPS_STRY(kaps_ix_mgr_wb_save_uda_translate
              (trie_p->m_ixMgr, trie_g->wb_fun->write_fn, trie_g->wb_fun->handle, nv_offset));

    /*
     * Traverse trie to save RPT, IPT and APT entries 
     */
    KAPS_STRY(kaps_kaps_trie_wb_save_rpt_ipt_and_apt_entries(trie_p->m_roots_trienode_p, &num_rpt_entries, nv_offset,
                                                             trie_g->poolMgr, trie_g->wb_fun->write_fn,
                                                             trie_g->wb_fun->handle));

    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & num_rpt_entries, sizeof(num_rpt_entries),
                                 num_rpts_offset))
        return KAPS_NV_READ_WRITE_FAILED;

    return KAPS_OK;
}

NlmErrNum_t
kaps_trie_wb_cr_post_process(
    kaps_trie_node * rpt_parent,
    kaps_trie_node * curr_node,
    kaps_trie_node * highestPriorityAptTrieNode,
    uint8_t is_crash_recovery,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;

    if (curr_node == NULL)
        return NLMERR_OK;

    if (curr_node->m_isRptNode)
    {
        if (rpt_parent)
        {

            errNum = kaps_trie_add_to_down_stream_rpt(rpt_parent, curr_node, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;
        }

        rpt_parent = curr_node;

        if (is_crash_recovery && highestPriorityAptTrieNode)
        {
            if (curr_node->m_rptAptLmpsofarPfx)
            {
                curr_node->m_rptAptLmpsofarPfx->m_backPtr = highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr;
            }
        }
    }

    /*
     * During warmboot m_aptLmpsofarPfx_p will not have backPtr set when this function is called. So we can't properly
     * calculate highestPriorityAptTrieNode
     */
    if (is_crash_recovery)
    {
        if (highestPriorityAptTrieNode)
        {
            if (curr_node->m_aptLmpsofarPfx_p
                && curr_node->m_aptLmpsofarPfx_p->m_backPtr->meta_priority <=
                highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr->meta_priority)
            {
                highestPriorityAptTrieNode = curr_node;
            }
        }
        else
        {
            if (curr_node->m_aptLmpsofarPfx_p)
            {
                highestPriorityAptTrieNode = curr_node;
            }
        }
    }

    curr_node->m_rptParent_p = rpt_parent;
    if (rpt_parent)
        curr_node->m_poolId = rpt_parent->m_poolId;

    errNum =
        kaps_trie_wb_cr_post_process(rpt_parent, curr_node->m_child_p[0], highestPriorityAptTrieNode, is_crash_recovery,
                                     o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum =
        kaps_trie_wb_cr_post_process(rpt_parent, curr_node->m_child_p[1], highestPriorityAptTrieNode, is_crash_recovery,
                                     o_reason);

    return errNum;
}

NlmErrNum_t
kaps_kaps_trie_wb_restore_ipt_entry(
    kaps_lpm_trie * trie,
    kaps_trie_node * rpt_parent,
    struct kaps_wb_ipt_entry_info * ipt_node,
    uint32_t * nv_offset)
{
    kaps_nlm_allocator *alloc = NULL;
    kaps_ipm *ipm = NULL;
    kaps_trie_node *ipt_trienode = NULL;
    kaps_prefix *pfx = NULL;
    NlmErrNum_t err_num = NLMERR_OK;
    uint32_t ipt_depth, ipt_length_1, rpt_lopoff_1;
    NlmReasonCode o_reason;

    if (trie == NULL || ipt_node == NULL)
        return NLMERR_FAIL;

    alloc = trie->m_trie_global->m_alloc_p;

    /*
     * ipt_length is after RPT lopoff. IPT trienode will be created at depth the is sum of RPT lopoff 
     */
    ipt_length_1 = ipt_node->ipt_entry_len_1;
    rpt_lopoff_1 =
        kaps_trie_get_num_rpt_bits_lopped_off(trie->m_trie_global->fibtblmgr->m_devMgr_p, rpt_parent->m_depth);
    ipt_depth = ipt_length_1 + rpt_lopoff_1;

    pfx = kaps_prefix_create(alloc, ipt_depth, ipt_depth, ipt_node->ipt_entry);
    if (!pfx)
        return NLMERR_FAIL;

    /*
     * add this ipt entry into trie 
     */
    if (ipt_depth)
        ipt_trienode = kaps_trie_node_insertPathFromPrefix(trie->m_roots_trienode_p, pfx,
                                                           trie->m_roots_trienode_p->m_depth, (ipt_depth - 1));
    else
        ipt_trienode = rpt_parent;

    if (ipt_trienode == NULL)
        return NLMERR_FAIL;

    ipt_trienode->m_node_type = NLMNSTRIENODE_LP;

    ipt_trienode->m_rptParent_p = rpt_parent;

    ipt_trienode->m_isReserved160bTrig = ipt_node->is_reserved_160b_trig;

    ipm = kaps_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, rpt_parent->m_poolId);
    if (ipm == NULL)
        return NLMERR_FAIL;

    /*
     * update IPM data structures 
     */
    err_num = kaps_ipm_wb_add_entry(ipm, ipt_trienode->m_lp_prefix_p, rpt_parent->m_rptId,
                                    ipt_length_1, ipt_node->addr);
    if (err_num != NLMERR_OK)
        return err_num;

    /*
     * Shadow memory update of the IPT entry. IIT SM update is taken care by LSN Manager below 
     */
    err_num = kaps_ipm_write_entry_to_hw(ipm, ipt_trienode->m_lp_prefix_p, rpt_parent->m_rptId,
                                         ipt_length_1, rpt_lopoff_1, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num = kaps_lsn_mc_wb_create_lsn(ipt_trienode->m_lsn_p, &ipt_node->lsn_info, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num = kaps_lsn_mc_commitIIT(ipt_trienode->m_lsn_p, ipt_trienode->m_lp_prefix_p->m_nIndex, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num =
        kaps_lsn_mc_wb_restore_pfx_data(ipt_trienode->m_lsn_p, nv_offset, trie->m_trie_global->wb_fun->read_fn,
                                        trie->m_trie_global->wb_fun->handle);
    if (err_num != NLMERR_OK)
        return err_num;

    kaps_prefix_destroy(pfx, alloc);

    kaps_ipm_verify(ipm, rpt_parent->m_rptId);

    return err_num;
}

NlmErrNum_t
kaps_kaps_trie_cr_restore_ipt_entry(
    kaps_lpm_trie * trie,
    kaps_trie_node * rpt_parent,
    struct kaps_wb_ipt_entry_info * ipt_node)
{
    kaps_nlm_allocator *alloc = NULL;
    kaps_ipm *ipm = NULL;
    kaps_trie_node *ipt_trienode = NULL;
    kaps_prefix *pfx = NULL;
    NlmErrNum_t err_num = NLMERR_OK;
    uint32_t ipt_depth_1, ipt_length_1, rpt_lopoff_1;
    NlmReasonCode o_reason;

    if (trie == NULL || ipt_node == NULL)
        return NLMERR_FAIL;

    alloc = trie->m_trie_global->m_alloc_p;

    /*
     * ipt_length is after RPT lopoff. IPT trienode will be created at depth the is sum of RPT lopoff 
     */
    ipt_length_1 = ipt_node->ipt_entry_len_1;
    rpt_lopoff_1 =
        kaps_trie_get_num_rpt_bits_lopped_off(trie->m_trie_global->fibtblmgr->m_devMgr_p, rpt_parent->m_depth);
    ipt_depth_1 = ipt_length_1 + rpt_lopoff_1;

    pfx = kaps_prefix_create(alloc, ipt_depth_1, ipt_depth_1, ipt_node->ipt_entry);
    if (!pfx)
        return NLMERR_FAIL;

    /*
     * add this ipt entry into trie 
     */
    if (ipt_depth_1)
        ipt_trienode = kaps_trie_node_insertPathFromPrefix(trie->m_roots_trienode_p, pfx,
                                                           trie->m_roots_trienode_p->m_depth, (ipt_depth_1 - 1));
    else
        ipt_trienode = rpt_parent;

    if (ipt_trienode == NULL)
        return NLMERR_FAIL;

    ipt_trienode->m_node_type = NLMNSTRIENODE_LP;

    ipt_trienode->m_rptParent_p = rpt_parent;

    ipm = kaps_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, rpt_parent->m_poolId);
    if (ipm == NULL)
        return NLMERR_FAIL;

    /*
     * update IPM data structures 
     */
    err_num = kaps_ipm_wb_add_entry(ipm, ipt_trienode->m_lp_prefix_p, rpt_parent->m_rptId,
                                    ipt_length_1, ipt_node->addr);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num = kaps_lsn_mc_recover_rebuild_lsn(&ipt_node->lsn_info, ipt_trienode->m_lsn_p, pfx, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    kaps_prefix_destroy(pfx, alloc);

    kaps_ipm_verify(ipm, rpt_parent->m_rptId);

    return err_num;
}

static uint32_t
kaps_get_length_of_prefix_from_mask(
    uint8_t * data,
    uint8_t num_bytes)
{
    uint32_t len = 0;
    uint32_t i = 0;

    for (i = 0; i < num_bytes; i++)
    {
        if (data[i] == 0)
            len += 8;
        else
            break;
    }

    if (i < num_bytes)
    {
        uint8_t byte = data[i];
        uint8_t num_of_ones = 0;

        while (byte)
        {
            num_of_ones++;
            byte >>= 1;
        }

        len += (8 - num_of_ones);
    }

    return len;
}

NlmErrNum_t
kaps_kaps_trie_cr_restore_rpt_entry(
    kaps_lpm_trie * trie,
    kaps_trie_node * rpt_trienode,
    struct kaps_wb_rpt_entry_info * rpt_entry_info)
{
    kaps_trie_global *trie_g = trie->m_trie_global;
    uint32_t ab_num, row_num = 0, lpu_no, num_lpu, brick_no, region_id, chunk_offset;
    struct kaps_device *device_p = trie_g->fibtblmgr->m_devMgr_p;
    struct kaps_wb_ipt_entry_info ipt_node;
    NlmErrNum_t err_num = NLMERR_OK;
    kaps_ipm *ipm_p;

    ipm_p = kaps_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, rpt_trienode->m_poolId);
    if (ipm_p)
    {
        ab_num = ipm_p->m_ab_info->ab_num;
        for (row_num = 0; row_num < ipm_p->m_ab_info->num_slots; row_num++)
        {
            if (device_p->type == KAPS_DEVICE_KAPS)
            {
                struct kaps_dba_entry data, mask;
                struct kaps_ads ads_info;
                uint8_t ads_read_buf[16];

                KAPS_STRY(kaps_dm_kaps_rpb_read(device_p, ab_num, row_num, &data, &mask));
                if (device_p->kaps_shadow->rpb_blks[ab_num].rpb_rows[row_num].rpb_tbl_id != trie->m_tbl_ptr->m_tblId)
                    continue;
                if (data.is_valid && mask.is_valid)
                {
                    ipt_node.addr = row_num;
                    ipt_node.blk_num = ab_num;
                    ipt_node.ipt_entry_len_1 = kaps_get_length_of_prefix_from_mask(mask.key, KAPS_LPM_KEY_MAX_WIDTH_8);
                    kaps_memcpy(ipt_node.ipt_entry, data.key, KAPS_LPM_KEY_MAX_WIDTH_8);

                    KAPS_STRY(kaps_dm_kaps_iit_read
                              (trie_g->fibtblmgr->m_devMgr_p, ab_num, row_num, sizeof(ads_info), ads_read_buf));
                    ads_info.bpm_ad = KapsReadBitsInArrray(ads_read_buf, 16, 19, 0);
                    ads_info.bpm_len = KapsReadBitsInArrray(ads_read_buf, 16, 27, 20);
                    ads_info.row_offset = KapsReadBitsInArrray(ads_read_buf, 16, 31, 28);

                    ads_info.format_map_00 = KapsReadBitsInArrray(ads_read_buf, 16, 35, 32);
                    ads_info.format_map_01 = KapsReadBitsInArrray(ads_read_buf, 16, 39, 36);
                    ads_info.format_map_02 = KapsReadBitsInArrray(ads_read_buf, 16, 43, 40);
                    ads_info.format_map_03 = KapsReadBitsInArrray(ads_read_buf, 16, 47, 44);
                    ads_info.format_map_04 = KapsReadBitsInArrray(ads_read_buf, 16, 51, 48);
                    ads_info.format_map_05 = KapsReadBitsInArrray(ads_read_buf, 16, 55, 52);
                    ads_info.format_map_06 = KapsReadBitsInArrray(ads_read_buf, 16, 59, 56);
                    ads_info.format_map_07 = KapsReadBitsInArrray(ads_read_buf, 16, 63, 60);
                    ads_info.format_map_08 = KapsReadBitsInArrray(ads_read_buf, 16, 67, 64);
                    ads_info.format_map_09 = KapsReadBitsInArrray(ads_read_buf, 16, 71, 68);
                    ads_info.format_map_10 = KapsReadBitsInArrray(ads_read_buf, 16, 75, 72);
                    ads_info.format_map_11 = KapsReadBitsInArrray(ads_read_buf, 16, 79, 76);
                    ads_info.format_map_12 = KapsReadBitsInArrray(ads_read_buf, 16, 83, 80);
                    ads_info.format_map_13 = KapsReadBitsInArrray(ads_read_buf, 16, 87, 84);
                    ads_info.format_map_14 = KapsReadBitsInArrray(ads_read_buf, 16, 91, 88);
                    ads_info.format_map_15 = KapsReadBitsInArrray(ads_read_buf, 16, 95, 92);

                    ads_info.bkt_row = KapsReadBitsInArrray(ads_read_buf, 16, 108, 96);
                    ads_info.reserved = KapsReadBitsInArrray(ads_read_buf, 16, 111, 109);
                    ads_info.key_shift = KapsReadBitsInArrray(ads_read_buf, 16, 119, 112);
                    ads_info.ecc = KapsReadBitsInArrray(ads_read_buf, 16, 127, 120);

                    KAPS_STRY(kaps_uda_mgr_get_relative_params_from_abs((*(trie->m_lsn_settings_p->m_pMlpMemMgr)),
                                                                        ads_info.row_offset, ads_info.bkt_row,
                                                                        &region_id, &chunk_offset));
                    ipt_node.lsn_info.region_id = region_id;
                    ipt_node.lsn_info.offset = chunk_offset;

                    num_lpu = (*(trie->m_lsn_settings_p->m_pMlpMemMgr))->region_info[0].num_lpu;
                    brick_no = 0;
                    ipt_node.lsn_info.num_lpus = 0;

                    while (num_lpu)
                    {
                        uint32_t gran_ix;
                        lpu_no =
                            kaps_kaps_uda_mgr_compute_brick_udc_from_region_id((*
                                                                                (trie->m_lsn_settings_p->m_pMlpMemMgr)),
                                                                               ipt_node.lsn_info.region_id,
                                                                               ipt_node.lsn_info.offset, brick_no);
                        err_num = kaps_kaps_format_map_get_gran(trie->m_lsn_settings_p, &ads_info, lpu_no, &gran_ix);
                        if (err_num != NLMERR_OK)
                            return err_num;
                        if (gran_ix != -1)
                        {
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].gran_ix = gran_ix;
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].gran
                                = trie->m_lsn_settings_p->m_granIxToGran[gran_ix];
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].base_address = 0;
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].ad_db
                                = (struct kaps_ad_db *) trie->m_tbl_ptr->m_db->common_info->ad_info.ad;
                            ipt_node.lsn_info.num_lpus++;
                        }
                        num_lpu--;
                        brick_no++;
                    }

                    err_num = kaps_kaps_trie_cr_restore_ipt_entry(trie, rpt_trienode, &ipt_node);
                    if (err_num != NLMERR_OK)
                        return err_num;
                    kaps_trie_incr_num_ipt_entries_in_subtrie(rpt_trienode, 1);
                    kaps_pool_mgr_incr_num_entries(trie->m_trie_global->poolMgr, rpt_trienode->m_poolId,
                                                   1, KAPS_IPT_POOL);
                }
            }

        }
    }

    return KAPS_OK;
}

/* add rpt trienode to trie, add this entry to pool manager, update RPT & RIT shadow memory */
NlmErrNum_t
kaps_trie_wb_cr_restore_rpt_entry(
    kaps_lpm_trie * trie,
    struct kaps_wb_rpt_entry_info * rpt_entry_info,
    uint32_t * nv_offset)
{
    kaps_trie_global *trie_g = NULL;
    struct kaps_wb_ipt_entry_info ipt_node;
    kaps_nlm_allocator *alloc;
    kaps_trie_node *rpt_trienode;
    kaps_prefix *pfx = NULL;
    NlmErrNum_t err_num = NLMERR_OK;
    uint32_t num_entries = 0, num_bytes_read, iter;
    kaps_fib_tbl *fibTbl_p = trie->m_tbl_ptr;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_db *db = fibTbl_p->m_db;

    alloc = trie->m_trie_global->m_alloc_p;
    trie_g = trie->m_trie_global;

    if (db->num_algo_levels_in_db == 3)
    {

        if (rpt_entry_info->rpt_entry_len_1 > 0)
        {
            pfx = kaps_prefix_create(alloc, rpt_entry_info->rpt_entry_len_1, rpt_entry_info->rpt_entry_len_1,
                                     rpt_entry_info->rpt_entry);
            if (!pfx)
                return NLMERR_FAIL;

            /*
             * add this rpt entry into trie 
             */
            rpt_trienode =
                kaps_trie_node_insertPathFromPrefix(trie->m_roots_trienode_p, pfx, trie->m_roots_trienode_p->m_depth,
                                                    (rpt_entry_info->rpt_entry_len_1 - 1));

            if (rpt_trienode == NULL)
                return NLMERR_FAIL;

            /*
             * Done with prefix, destroy it 
             */
            kaps_prefix_destroy(pfx, alloc);
        }
        else
        {
            rpt_trienode = trie->m_roots_trienode_p;
        }

        rpt_trienode->m_rpt_prefix_p = kaps_pfx_bundle_create_from_string(alloc, rpt_trienode->m_lp_prefix_p->m_data,
                                                                          rpt_trienode->m_depth, KAPS_LSN_NEW_INDEX,
                                                                          sizeof(void *), 0);

        kaps_memcpy(KAPS_PFX_BUNDLE_GET_ASSOC_PTR(rpt_trienode->m_rpt_prefix_p), &rpt_trienode, sizeof(void *));

        err_num = kaps_rpm_wb_add_entry(trie->m_trie_global->m_rpm_p, rpt_trienode->m_rpt_prefix_p,
                                        rpt_trienode->m_depth, rpt_entry_info->rpt_entry_location, fibTbl_p->m_tblId);

        if (err_num != NLMERR_OK)
            return err_num;
    }
    else
    {
        rpt_trienode = trie->m_roots_trienode_p;
    }

    rpt_trienode->m_isRptNode = 1;

    rpt_trienode->m_rptId = rpt_entry_info->rpt_id;
    rpt_trienode->m_rpt_uuid = rpt_entry_info->rpt_uuid;
    if (rpt_trienode->m_rpt_uuid != NLM_INVALID_RPT_UUID)
        trie->m_trie_global->m_rpt_uuid_table[rpt_entry_info->rpt_uuid] = 0;

    rpt_trienode->m_poolId = rpt_entry_info->pool_id;
    rpt_trienode->m_rptParent_p = rpt_trienode;

    /*
     * Add this rpt entry to the pool 
     */
    err_num = kaps_pool_mgr_wb_add_rpt_entry_to_pool(trie->m_trie_global->poolMgr, rpt_trienode);
    if (err_num != NLMERR_OK)
        return err_num;

    if (db->num_algo_levels_in_db == 3)
    {

        /*
         * Shadow memory updates w/o writing to the actual device now 
         */
        err_num = kaps_trie_commit_rpt_and_rit(rpt_trienode, &reason);
        if (err_num != NLMERR_OK)
            return err_num;
    }

    /*
     * Handle crash recovery here and return
     */
    if (trie_g->fibtblmgr->m_devMgr_p->nv_ptr)
    {
        return kaps_kaps_trie_cr_restore_rpt_entry(trie, rpt_trienode, rpt_entry_info);
    }

    /*
     * Now create IPT entries under this RPT entry 
     */
    num_entries = rpt_entry_info->num_ipt_entries;

    if (num_entries)
    {
        for (iter = 0; iter < num_entries; iter++)
        {
            if (0 !=
                trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & ipt_node,
                                        sizeof(struct kaps_wb_ipt_entry_info), *nv_offset))
                return NLMERR_FAIL;
            num_bytes_read = sizeof(struct kaps_wb_ipt_entry_info);
            *nv_offset += num_bytes_read;

            err_num = kaps_kaps_trie_wb_restore_ipt_entry(trie, rpt_trienode, &ipt_node, nv_offset);
            if (err_num != NLMERR_OK)
                return err_num;
        }

        kaps_trie_incr_num_ipt_entries_in_subtrie(rpt_trienode, num_entries);
        kaps_pool_mgr_incr_num_entries(trie->m_trie_global->poolMgr, rpt_trienode->m_poolId,
                                       num_entries, KAPS_IPT_POOL);
    }

    return err_num;
}

NlmErrNum_t
kaps_kaps_trie_wb_restore_pools(
    kaps_trie_global * trie_g,
    struct kaps_wb_cb_functions * wb_fun)
{
    uint32_t num_active_pools;

    /*
     * Restore number of active IPT and APT pool count 
     */
    if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                             sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    trie_g->poolMgr->m_numActiveIptPools = num_active_pools;

    if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                             sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    trie_g->poolMgr->m_numActiveAptPools = num_active_pools;

    return kaps_pool_mgr_wb_restore_pools(trie_g->poolMgr, wb_fun->read_fn, wb_fun->handle, wb_fun->nv_offset);
}

NlmErrNum_t
kaps_kaps_trie_cr_restore_pools(
    kaps_lpm_trie * trie,
    struct kaps_wb_cb_functions * wb_fun)
{
    kaps_trie_global *trie_g = trie->m_trie_global;
    struct kaps_device *device = trie_g->fibtblmgr->m_devMgr_p;
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode o_reason;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        /*
         * KAPS device doesn't have any real RPT entries. To make the code work generically, the root node of the trie
         * is made as the RPT parent for the IPT node
         */
        kaps_trie_node *root_node = trie->m_roots_trienode_p;

        if (!root_node->m_isRptNode)
        {
            root_node->m_isRptNode = 1;
            root_node->m_rptParent_p = root_node;

            err_num = kaps_trie_pvt_get_new_pool(trie_g->poolMgr, root_node, 1, 0, 1, &o_reason);

            if (err_num != NLMERR_OK)
            {
                return NLMERR_FAIL;
            }
        }
        return NLMERR_OK;
    }
    else
    {
        uint32_t *num_active_pools;

        num_active_pools = (uint32_t *) wb_fun->nv_ptr;
        wb_fun->nv_ptr += sizeof(*num_active_pools);

        /*
         * Store number of active IPT and APT pool count 
         */
        trie_g->poolMgr->m_numActiveIptPools = *num_active_pools;
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

        num_active_pools = (uint32_t *) wb_fun->nv_ptr;
        wb_fun->nv_ptr += sizeof(*num_active_pools);

        trie_g->poolMgr->m_numActiveAptPools = *num_active_pools;
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

        kaps_pool_mgr_cr_restore_pools(trie_g->poolMgr, wb_fun);
    }

    return NLMERR_OK;
}

kaps_status
kaps_trie_wb_restore(
    kaps_lpm_trie * trie,
    uint32_t * nv_offset)
{
    kaps_trie_global *trie_g;
    struct kaps_wb_rpt_entry_info rpt_info;
    NlmErrNum_t err_num;
    uint32_t num_rpt_entries, iter;
    kaps_fib_tbl *fibTbl = trie->m_tbl_ptr;
    uint16_t no_of_regions;
    struct region region_info[MAX_UDA_REGIONS];
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_db *db = fibTbl->m_db;

    trie_g = trie->m_trie_global;

    if (0 != trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_are_reserved_160b_abs_initialized,
                                     sizeof(trie_g->m_are_reserved_160b_abs_initialized), *nv_offset))
    {
        return KAPS_NV_READ_WRITE_FAILED;
    }

    *nv_offset += sizeof(trie_g->m_are_reserved_160b_abs_initialized);

    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle,
                                (uint8_t *) & trie->m_lsn_settings_p->m_numAllocatedBricksForAllLsns,
                                sizeof(trie->m_lsn_settings_p->m_numAllocatedBricksForAllLsns), *nv_offset))
    {
        return KAPS_NV_READ_WRITE_FAILED;
    }

    *nv_offset += sizeof(trie->m_lsn_settings_p->m_numAllocatedBricksForAllLsns);

    if (0 != trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie->m_lsn_settings_p->m_maxLpuPerLsn,
                                     sizeof(trie->m_lsn_settings_p->m_maxLpuPerLsn), *nv_offset))
    {
        return KAPS_NV_READ_WRITE_FAILED;
    }

    *nv_offset += sizeof(trie->m_lsn_settings_p->m_maxLpuPerLsn);

    /*
     * Restore the number of regions in the UDA Mgr 
     */
    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_mlpmemmgr[0]->no_of_regions,
                                sizeof(no_of_regions), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(no_of_regions);

    /*
     * Restore the UDA Mgr region info 
     */
    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_mlpmemmgr[0]->region_info,
                                sizeof(region_info), *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(region_info);

    no_of_regions = trie_g->m_mlpmemmgr[0]->no_of_regions;
    kaps_memcpy(region_info, trie_g->m_mlpmemmgr[0]->region_info, sizeof(region_info));

    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & num_rpt_entries, sizeof(num_rpt_entries),
                                *nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;

    *nv_offset += sizeof(num_rpt_entries);

    KAPS_STRY(kaps_ix_mgr_wb_restore_uda_translate
              (trie->m_ixMgr, trie_g->wb_fun->read_fn, trie_g->wb_fun->handle, nv_offset));

    for (iter = 0; iter < num_rpt_entries; iter++)
    {
        if (0 !=
            trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & rpt_info,
                                    sizeof(struct kaps_wb_rpt_entry_info), *nv_offset))
            return KAPS_NV_READ_WRITE_FAILED;
        *nv_offset += sizeof(struct kaps_wb_rpt_entry_info);

        err_num = kaps_trie_wb_cr_restore_rpt_entry(trie, &rpt_info, nv_offset);
        if (err_num != NLMERR_OK)
            return err_num;
    }

    if (db->num_algo_levels_in_db == 3)
        kaps_rpm_verify(trie_g->m_rpm_p, fibTbl->m_tblId);

    /*
     * Compute and assign RPT parent for each trienode in the trie 
     */
    err_num = kaps_trie_wb_cr_post_process(NULL, trie->m_roots_trienode_p, NULL, 0, &reason);
    if (err_num != NLMERR_OK)
        return err_num;

    if (trie_g->fibtblmgr->m_devMgr_p->type != KAPS_DEVICE_KAPS)
    {
        kaps_trie_wb_verify(trie->m_roots_trienode_p);
    }

    if (!trie_g->m_mlpmemmgr[0]->chunk_map[0])
    {
        trie_g->m_mlpmemmgr[0]->no_of_regions = no_of_regions;
        kaps_memcpy(trie_g->m_mlpmemmgr[0]->region_info, region_info, sizeof(region_info));
        KAPS_STRY(kaps_uda_mgr_wb_restore_regions
                  (trie_g->m_mlpmemmgr[0], fibTbl->m_fibTblMgr_p->m_devMgr_p->alloc, no_of_regions, region_info));
    }

    return KAPS_OK;
}

void
kaps_trie_wb_compute_iit_lmpsofar(
    kaps_trie_node * node_p,
    kaps_trie_node * ancestorIptNode_p)
{
    kaps_pfx_bundle *lmp_in_prev_lsn;

    if (ancestorIptNode_p)
    {
        lmp_in_prev_lsn = kaps_lsn_mc_locate_lpm(ancestorIptNode_p->m_lsn_p, node_p->m_lp_prefix_p->m_data,
                                                 node_p->m_lp_prefix_p->m_nPfxSize, NULL, NULL, NULL);
        /*
         * Find the prefix in the ancestor LSN that is lmpsofar of current LSN and the prefix is not a prefix copy
         */
        if (lmp_in_prev_lsn && !lmp_in_prev_lsn->m_isPfxCopy)
        {
            node_p->m_iitLmpsofarPfx_p = lmp_in_prev_lsn;
        }
        else
        {
            /*
             * If lmp_in_prev_lsn doesn't exist or if lmp_in_prev_lsn is a prefix copy, then m_iitLmpsofarPfx_p of the
             * node and its ancestor should match 
             */
            node_p->m_iitLmpsofarPfx_p = ancestorIptNode_p->m_iitLmpsofarPfx_p;
        }
    }
}

void
kaps_kaps_trie_wb_restore_iit_lmpsofar(
    kaps_trie_node * node_p,
    kaps_trie_node * ancestorIptNode_p)
{
    if (!node_p)
        return;

    node_p->m_iitLmpsofarPfx_p = NULL;

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        kaps_trie_wb_compute_iit_lmpsofar(node_p, ancestorIptNode_p);

        ancestorIptNode_p = node_p;
    }

    if (node_p->m_child_p[0])
        kaps_kaps_trie_wb_restore_iit_lmpsofar(node_p->m_child_p[0], ancestorIptNode_p);

    if (node_p->m_child_p[1])
        kaps_kaps_trie_wb_restore_iit_lmpsofar(node_p->m_child_p[1], ancestorIptNode_p);
}

kaps_status
kaps_trie_cr_restore(
    kaps_lpm_trie * trie,
    uint32_t * nv_offset)
{
    struct kaps_wb_rpt_entry_info rpt_info;
    NlmErrNum_t err_num;
    NlmReasonCode reason = NLMRSC_REASON_OK;

    kaps_memset(&rpt_info, 0, sizeof(struct kaps_wb_rpt_entry_info));
    if (trie->m_lsn_settings_p->m_device->type == KAPS_DEVICE_KAPS)
    {
        rpt_info.rpt_id = trie->m_roots_trienode_p->m_rptId;

        err_num = kaps_trie_wb_cr_restore_rpt_entry(trie, &rpt_info, nv_offset);
        if (err_num != NLMERR_OK)
            return err_num;
    }

    /*
     * Compute and assign RPT parent for each trienode in the trie 
     */
    err_num = kaps_trie_wb_cr_post_process(NULL, trie->m_roots_trienode_p, NULL, 1, &reason);
    if (err_num != NLMERR_OK)
        return err_num;

    return KAPS_OK;
}

kaps_status
kaps_trie_wb_dump_pools(
    FILE * bin_fp,
    FILE * txt_fp,
    uint32_t * bin_offset)
{
    uint32_t ipt_pools, apt_pools;

    kaps_fprintf(txt_fp, "Start of Pool Information \n");
    kaps_fprintf(txt_fp, "-------------------------\n");

    /*
     * Number of IPT pools followed by APT pools 
     */
    kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & ipt_pools, sizeof(ipt_pools), *bin_offset);
    *bin_offset += sizeof(ipt_pools);

    kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & apt_pools, sizeof(apt_pools), *bin_offset);
    *bin_offset += sizeof(apt_pools);

    kaps_fprintf(txt_fp, "Num IPT Pools = %d, Num APT Pools = %d\n\n", ipt_pools, apt_pools);

    kaps_pool_mgr_wb_dump_pools(bin_fp, txt_fp, (ipt_pools + apt_pools), bin_offset);

    kaps_fprintf(txt_fp, "\nEnd of Pool Information \n");
    kaps_fprintf(txt_fp, "-----------------------\n\n");

    return 0;
}

NlmErrNum_t
kaps_trie_wb_dump_rpt_entry(
    FILE * fp,
    struct kaps_wb_rpt_entry_info * rpt_entry)
{
    uint32_t iter, len_8;

    len_8 = (rpt_entry->rpt_entry_len_1 + 7) >> 3;
    kaps_fprintf(fp, "\nRPT = ");

    for (iter = 0; iter < len_8; iter++)
    {
        if (iter != 0)
            kaps_fprintf(fp, ".");

        kaps_fprintf(fp, "%d", rpt_entry->rpt_entry[iter]);
    }
    kaps_fprintf(fp, "/%d\n", rpt_entry->rpt_entry_len_1);

    kaps_fprintf(fp, "\tIndex = %d, RPT ID = %d, Pool ID = %d\n", rpt_entry->rpt_entry_location,
                 rpt_entry->rpt_id, rpt_entry->pool_id);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_trie_wb_dump_ipt_entry(
    FILE * bin_fp,
    FILE * txt_fp,
    struct kaps_wb_ipt_entry_info * ipt_entry,
    uint32_t rpt_lopoff_1,
    uint32_t * bin_offset)
{
    uint32_t lopoff_1, iter, ipt_len_8;
    uint32_t rpt_lopoff_8 = rpt_lopoff_1 / 8;

    /*
     * len_1 is after RPT lopoff 
     */
    ipt_len_8 = (ipt_entry->ipt_entry_len_1 + 7) >> 3;
    kaps_fprintf(txt_fp, "\nIPT = ");

    for (iter = 0; iter < ipt_len_8; iter++)
    {
        if (iter != 0)
            kaps_fprintf(txt_fp, ".");

        /*
         * print bytes after RPT lopoff 
         */
        kaps_fprintf(txt_fp, "%d", ipt_entry->ipt_entry[rpt_lopoff_8 + iter]);
    }
    kaps_fprintf(txt_fp, "/%d\n", ipt_entry->ipt_entry_len_1);

    kaps_fprintf(txt_fp, "\tBlock Num = %d, Index = %d\n", ipt_entry->blk_num, ipt_entry->addr);

    /*
     * Dump prefixes under this IPT entry. Prefix remainder is stored at byte boundary. For e.g. sum of RPT and IPT
     * lopoff is 18, we store prefix remainder from 3rd byte itself. For this example first 2 bits are duplicated in
     * IPT and prefix remainder. 
     */
    lopoff_1 = (rpt_lopoff_1 + ipt_entry->ipt_entry_len_1);
    kaps_lsn_mc_wb_dump_prefixes(bin_fp, txt_fp, lopoff_1, bin_offset);

    return NLMERR_OK;
}

kaps_status
kaps_trie_wb_dump_trie(
    FILE * bin_fp,
    FILE * txt_fp,
    uint8_t is_b0_silicon,
    uint32_t * bin_offset)
{
    struct kaps_wb_rpt_entry_info rpt_entry;
    struct kaps_wb_ipt_entry_info ipt_entry;
    uint32_t db_id, addr, num_rpt_entries, iter1, iter2;

    if (is_b0_silicon)
    {
        kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & db_id, sizeof(db_id), *bin_offset);
        *bin_offset += sizeof(db_id);

        kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & addr, sizeof(addr), *bin_offset);
        *bin_offset += sizeof(addr);

        kaps_fprintf(txt_fp, "\t Rev-B Default RPT : DB_ID = %d, Location = %d\n", db_id, addr);
    }

    kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & num_rpt_entries, sizeof(num_rpt_entries), *bin_offset);
    *bin_offset += sizeof(num_rpt_entries);

    kaps_fprintf(txt_fp, "Num RPT Entries = %d\n", num_rpt_entries);

    for (iter1 = 0; iter1 < num_rpt_entries; iter1++)
    {
        kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & rpt_entry, sizeof(struct kaps_wb_rpt_entry_info), *bin_offset);
        *bin_offset += sizeof(struct kaps_wb_rpt_entry_info);

        kaps_trie_wb_dump_rpt_entry(txt_fp, &rpt_entry);

        kaps_fprintf(txt_fp, "\tNum IPT Entries: %d\n\n", rpt_entry.num_ipt_entries);

        for (iter2 = 0; iter2 < rpt_entry.num_ipt_entries; iter2++)
        {
            kaps_lpm_wb_file_read(bin_fp, (uint8_t *) & ipt_entry, sizeof(struct kaps_wb_ipt_entry_info), *bin_offset);
            *bin_offset += sizeof(struct kaps_wb_ipt_entry_info);

            kaps_trie_wb_dump_ipt_entry(bin_fp, txt_fp, &ipt_entry, rpt_entry.rpt_entry_len_1, bin_offset);
        }

        kaps_fprintf(txt_fp, "\tNum APT Entries: %d\n\n", rpt_entry.num_apt_entries);

    }

    return 0;
}

void
kaps_kaps_trie_wb_traverse_and_print_trienodes(
    kaps_trie_node * trienode,
    FILE * fp)
{
    /*
     * Important -- the format specifier should be used carefully. Otherwise, reaching back using fscanf will create
     * problems. Use "%hd" for ushort and "%c" for uchar 
     */
    kaps_fprintf(fp, "%u %u ", trienode->m_rptId, trienode->m_numAptEntriesInSubtrie);
    kaps_fprintf(fp, "%u %u ", trienode->m_child_id, trienode->m_isRptNode);

    /*
     * Mark RPT parent if exists 
     */
    if (trienode->m_rptParent_p)
        kaps_fprintf(fp, "1 ");

    /*
     * Mark children 
     */
    if (trienode->m_child_p[0])
        kaps_fprintf(fp, "1 ");
    if (trienode->m_child_p[1])
        kaps_fprintf(fp, "1 ");

    /*
     * print various prefix bundles here 
     */

    /*
     * print LSN info here 
     */

    if (trienode->m_child_p[0])
        kaps_kaps_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[0], fp);

    if (trienode->m_child_p[1])
        kaps_kaps_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[1], fp);
}

kaps_status
kaps_trie_wb_traverse_and_print_trie(
    kaps_lpm_trie * trie,
    FILE * fp)
{
    /*
     * re-visit -- should we print the kaps_lpm_trie DS also? 
     */

    kaps_kaps_trie_wb_traverse_and_print_trienodes(trie->m_roots_trienode_p, fp);

    return KAPS_OK;
}

kaps_status
kaps_trie_wb_traverse_and_verify_trienodes(
    kaps_trie_node * trienode,
    FILE * fp)
{
    uint32_t uint1, uint2, uint3, uint4, uint5;
    uint16_t ushort1;
    uint8_t uchar1;

    (void) kaps_fscanf(fp, "%u %hd %c %u %u ", &uint1, &ushort1, &uchar1, &uint2, &uint3);

    KAPS_WB_IS_EQUAL(trienode->m_node_type, uint1, "node_type mismatch ");
    KAPS_WB_IS_EQUAL(trienode->m_depth, ushort1, "depth mismatch ");
    KAPS_WB_IS_EQUAL(trienode->m_numIptEntriesInSubtrie, uint2, "numIPTEntries mismatch ");
    KAPS_WB_IS_EQUAL(trienode->m_poolId, uint1, "poolId mismatch ");

    (void) kaps_fscanf(fp, "%u %u %u ", &uint1, &uint2, &uint3);

    KAPS_WB_IS_EQUAL(trienode->m_rptId, uint1, "rptId mismatch ");
    KAPS_WB_IS_EQUAL(trienode->m_numAptEntriesInSubtrie, uint2, "numAPTEntries mismatch ");

    (void) kaps_fscanf(fp, "%u %u %u %u %u ", &uint1, &uint2, &uint3, &uint4, &uint5);

    KAPS_WB_IS_EQUAL(trienode->m_child_id, uint1, "child_id mismatch ");
    KAPS_WB_IS_EQUAL(trienode->m_isRptNode, uint4, "isRptNode mismatch ");

    if (trienode->m_rptParent_p)
    {
        (void) kaps_fscanf(fp, "%u ", &uint1);

        KAPS_WB_IS_EQUAL(1, uint1, "rptParent missing ");
    }

    if (trienode->m_child_p[0])
    {
        (void) kaps_fscanf(fp, "%u ", &uint1);

        KAPS_WB_IS_EQUAL(1, uint1, "left child missing ");
    }

    if (trienode->m_child_p[1])
    {
        (void) kaps_fscanf(fp, "%u ", &uint1);

        KAPS_WB_IS_EQUAL(1, uint1, "right child missing ");
    }

    if (trienode->m_child_p[0])
        kaps_kaps_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[0], fp);

    if (trienode->m_child_p[1])
        kaps_kaps_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[1], fp);

    return KAPS_OK;
}

kaps_status
kaps_kaps_trie_wb_verify_trie(
    kaps_lpm_trie * trie,
    FILE * fp)
{
    kaps_trie_wb_traverse_and_verify_trienodes(trie->m_roots_trienode_p, fp);

    return KAPS_OK;
}

void
kaps_trie_wb_verify(
    kaps_trie_node * trie_node)
{
    uint32_t old_num_ipt_entries, old_num_apt_entries;

    if (trie_node == NULL)
        return;

    if (trie_node->m_isRptNode)
    {
        old_num_ipt_entries = trie_node->m_numIptEntriesInSubtrie;
        old_num_apt_entries = trie_node->m_numAptEntriesInSubtrie;

        trie_node->m_trie_p->m_trie_global->m_rptNodeForCalcIptApt = trie_node;
        trie_node->m_trie_p->m_trie_global->useTrigLenForCalcIptApt = 0;
        kaps_trie_pvt_calc_num_ipt_and_apt(trie_node, NULL);

        kaps_assert(old_num_ipt_entries == trie_node->m_numIptEntriesInSubtrie,
                    "Mismatch in number of IPT entries in the subtrie \n");

        kaps_assert(old_num_apt_entries == trie_node->m_numAptEntriesInSubtrie,
                    "Mismatch in number of APT entries in the subtrie \n");
    }

    kaps_trie_wb_verify(trie_node->m_child_p[0]);
    kaps_trie_wb_verify(trie_node->m_child_p[1]);
}

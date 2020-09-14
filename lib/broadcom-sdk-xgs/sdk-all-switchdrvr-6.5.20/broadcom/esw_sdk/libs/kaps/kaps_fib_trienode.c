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
#include <kaps_fib_cmn_pfxbundle.h>
#include <kaps_fib_trienode.h>
#include <kaps_fib_trie.h>
#include "kaps_fib_lsnmc_hw.h"
#include "kaps_utility.h"
#include "kaps_handle.h"

/* If this returns True, recusing children will be pruned for pre-order traversal
 * Return value ignored for post-order traversal
 */
typedef int32_t(
    *kaps_trie_node_traverse_dfs_visitor_t) (
    kaps_trie_node * node,
    NlmNsTrieNode__TraverseArgs * data);

/* Constructor to create and initialize internal data of trie node */
static void kaps_trie_node_ctor(
    kaps_trie_node * self,
    kaps_trie_node * parent,
    kaps_lpm_trie * trie);

/* Traverses the trie and removes the given node from the trie */
static void kaps_trie_node_traverse_dfs_for_remove(
    kaps_trie_node * node,
    kaps_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args);

/* It inserts the child node for given trie node. If child is 0 it inserts
 * left child otherwise right child.
 */
static kaps_trie_node *kaps_trie_node_insert(
    kaps_trie_node * self,
    kaps_lpm_trie * trie,
    uint8_t child);

/* Constructor to create and initialize internal data of trie node */
static void
kaps_trie_node_ctor(
    kaps_trie_node * self,
    kaps_trie_node * parent,
    kaps_lpm_trie * trie_p)
{
    static uint32_t trienodeId = 1;

    kaps_assert(self != NULL, "Trienode Pointer is NULL");

    kaps_memset(self, 0, sizeof(kaps_trie_node));
    self->m_parent_p = parent;
    self->m_trie_p = trie_p;
    self->m_node_type = NLMNSTRIENODE_REGULAR;
    if (parent)
    {
        self->m_poolId = parent->m_poolId;
        self->m_rptId = parent->m_rptId;
    }
    self->m_rpt_uuid = NLM_INVALID_RPT_UUID;

    self->m_trienodeId = trienodeId++;

    if (trie_p->m_lsn_settings_p)
        self->m_lsn_p = kaps_lsn_mc_create(trie_p->m_lsn_settings_p, trie_p, parent ? parent->m_depth + 1 : 0);

    if (parent)
    {
        self->m_rptParent_p = parent->m_rptParent_p;
        self->m_poolId = parent->m_poolId;
    }

}

/************************************************
 * Function :
 * kaps_trie_node_set_pfx_bundle
 *
 * Parameters:
 * kaps_trie_node * self,
 * kaps_pfx_bundle * pfx
 *
 * Summary:
 * __SetPfxBundle sets the given prefix bundle with given trie.
 *
 ***********************************************/
void
kaps_trie_node_set_pfx_bundle(
    kaps_trie_node * self,
    kaps_pfx_bundle * pfx)
{
    self->m_lp_prefix_p = pfx;
    if (self->m_lsn_p)
        self->m_lsn_p->m_pParentHandle = pfx;
    kaps_memcpy(KAPS_PFX_BUNDLE_GET_ASSOC_PTR(pfx), &self, sizeof(void *));

}

static int32_t
kaps_trie_node_destroyHelper(
    kaps_trie_node * self,
    NlmNsTrieNode__TraverseArgs * args)
{
    kaps_nlm_allocator *alloc = (kaps_nlm_allocator *) args->arg1;
    int32_t delPfxes = (int32_t) (uintptr_t) args->arg2;
    NlmNsDownStreamRptNodes *curNode, *nextNode;

    if (delPfxes && self->m_lsn_p)
        kaps_lsn_mc_free_prefixes_safe(self->m_lsn_p);

    /*
     * Reset the cache trie node if that is deleted 
     */
    if (self == self->m_trie_p->m_cache_trienode_p)
        self->m_trie_p->m_cache_trienode_p = NULL;

    if (self->m_lsn_p)
        kaps_lsn_mc_destroy(self->m_lsn_p);

    kaps_pfx_bundle_destroy(self->m_lp_prefix_p, alloc);

    if (self->m_rpt_prefix_p)
        kaps_pfx_bundle_destroy(self->m_rpt_prefix_p, alloc);

    if (self->m_rptAptLmpsofarPfx)
        kaps_pfx_bundle_destroy(self->m_rptAptLmpsofarPfx, alloc);

    curNode = self->m_downstreamRptNodes;
    while (curNode)
    {
        nextNode = curNode->next;
        kaps_nlm_allocator_free(alloc, curNode);
        curNode = nextNode;
    }

    kaps_nlm_allocator_free(alloc, self);

    return 1;
}

static kaps_trie_node *
kaps_trie_node_create(
    kaps_nlm_allocator * alloc,
    kaps_trie_node * parent,
    kaps_lpm_trie * trie)
{
    kaps_trie_node *self;

    kaps_assert(alloc, "No Allocator pointer");

    self = (kaps_trie_node *) kaps_nlm_allocator_calloc(alloc, 1, sizeof(kaps_trie_node));
    kaps_trie_node_ctor(self, parent, trie);

    return self;
}

/************************************************
 * Function :
 * kaps_trie_node_destroy
 *
 * Parameters:
 *   kaps_trie_node * self,
 *   kaps_nlm_allocator * alloc,
 *   int32_t delPfxes
 *
 * Summary:
 * __destroy destroys the trie from that node.
 *
 ***********************************************/
void
kaps_trie_node_destroy(
    kaps_trie_node * self,
    kaps_nlm_allocator * alloc,
    int32_t delPfxes)
{
    if (self)
    {
        NlmNsTrieNode__TraverseArgs args;
        args.arg1 = alloc;
        args.arg2 = KAPS_CAST_U32_TO_PTR(delPfxes);
        kaps_trie_node_traverse_dfs_for_remove(self, kaps_trie_node_destroyHelper, &args);
    }
}

/************************************************
 * Function :
 * kaps_trie_node_create_root_node
 *
 * Parameters:
 *    struct kaps_lpm_trie *trie,
 *    uint32_t child
 *
 * Summary:
 * __CreateRootNode creates the "root" node of the trie. It is at a
 * depth that is equal to tblid length as we always lopoff the tblid bits of the
 * prefix and so we can start from a node at depth=tblid_len and we don't need
 * the nodes above it
 *
 ***********************************************/
kaps_trie_node *
kaps_trie_node_create_root_node(
    kaps_lpm_trie * trie,
    uint32_t child)
{
    kaps_trie_node *newnode_p;
    kaps_pfx_bundle *bundle;

    newnode_p = kaps_trie_node_create(trie->m_trie_global->m_alloc_p, NULL, trie);

    newnode_p->m_child_id = child;

    bundle =
        kaps_pfx_bundle_create_from_string(trie->m_trie_global->m_alloc_p, NULL, 0, KAPS_LSN_NEW_INDEX, sizeof(void *),
                                           0);

    kaps_trie_node_set_pfx_bundle(newnode_p, bundle);

    newnode_p->m_depth = 0;
    if (newnode_p->m_lsn_p)
    {
        newnode_p->m_lsn_p->m_nDepth = 0;
        newnode_p->m_lsn_p->m_nLopoff = 0;
    }

    return newnode_p;
}

kaps_trie_node *
kaps_trie_node_add_child(
    kaps_trie_node * self,
    kaps_lpm_trie * trie,
    uint8_t child)
{
    kaps_trie_node *newnode_p;
    kaps_pfx_bundle *bundle;
    uint8_t tmpbuf[320 / KAPS_BITS_IN_BYTE + 1];
    uint8_t *tmpptr;

    newnode_p = kaps_trie_node_create(trie->m_trie_global->m_alloc_p, self, trie);

    self->m_child_p[child] = newnode_p;

    newnode_p->m_child_id = child;
    newnode_p->m_depth = (uint16_t) (self->m_depth + 1);
    if (newnode_p->m_lsn_p)
        newnode_p->m_lsn_p->m_nDepth = newnode_p->m_depth;

    if ((self->m_depth & 7) == 0)
    {
        uint32_t ptrLen = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(self->m_depth);

        kaps_memcpy(tmpbuf, KAPS_PFX_BUNDLE_GET_PFX_DATA(self->m_lp_prefix_p), ptrLen);
        tmpptr = tmpbuf;
        tmpptr[ptrLen] = 0;
    }
    else
        tmpptr = KAPS_PFX_BUNDLE_GET_PFX_DATA(self->m_lp_prefix_p);

    bundle =
        kaps_pfx_bundle_create_from_string(trie->m_trie_global->m_alloc_p, tmpptr, newnode_p->m_depth,
                                           KAPS_LSN_NEW_INDEX, sizeof(void *), 0);

    kaps_pfx_bundle_set_bit(bundle, self->m_depth, child);

    kaps_trie_node_set_pfx_bundle(newnode_p, bundle);

    kaps_assert(trie == newnode_p->m_trie_p, "kaps_trie_node_add_child");

    if (trie->m_maxDepth < newnode_p->m_depth)
        trie->m_maxDepth = newnode_p->m_depth;

    return newnode_p;
}

static void
kaps_trie_node_traverse_dfs_preorder(
    kaps_trie_node * node,
    kaps_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args)
{
    if (node)
    {
        int32_t ret;
        kaps_lpm_trie *trie = node->m_trie_p;
        kaps_trie_node **ary = kaps_trie_get_stack_space(trie);
        uint32_t n = 0;
        kaps_trie_node *left, *right;

        while (node)
        {
again:
            ret = func(node, args);
            if (!ret)
            {
                if (!n)
                    break;
                node = ary[--n];
                goto again;
            }
            left = node->m_child_p[NLMNSTRIE_LCHILD];
            right = node->m_child_p[NLMNSTRIE_RCHILD];
            if (right)
            {
                kaps_assert(n < KAPS_LPM_KEY_MAX_WIDTH_1, "kaps_trie_node_traverse_dfs_preorder");
                ary[n++] = right;
            }

            node = left ? left : (n ? ary[--n] : 0);
        }

        kaps_trie_return_stack_space(trie, ary);
    }
}

/* Traverses the trie and removes the given node from the trie */
static void
kaps_trie_node_traverse_dfs_for_remove(
    kaps_trie_node * node,
    kaps_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args)
{
    if (node)
    {
        kaps_lpm_trie *trie = node->m_trie_p;
        kaps_trie_node **ary = kaps_trie_get_stack_space(trie);
        uint32_t n = 0;
        kaps_trie_node *left, *right;

        kaps_assert(args != 0, "No args ");

        while (node)
        {
            kaps_trie_node *cur = node;
            left = node->m_child_p[NLMNSTRIE_LCHILD];
            right = node->m_child_p[NLMNSTRIE_RCHILD];
            if (right)
            {
                kaps_assert(n < 320, "Depth of node is invalid ");
                ary[n++] = right;
            }

            node = left ? left : (n ? ary[--n] : 0);
            func(cur, args);
        }

        kaps_trie_return_stack_space(trie, ary);
    }
}

int32_t
kaps_trie_node_pvt_is_lp_node(
    kaps_trie_node * self)
{
    if (self)
    {
        switch (self->m_node_type)
        {
            case NLMNSTRIENODE_REGULAR:
                break;
            default:
                return 1;
        }
    }
    return 0;
}

/************************************************
 * Function :
 * kaps_trie_node_pvt_create_lsn
 *
 * Parameters:
 *    kaps_trie_node *self,
 *
 * Summary:
 * _CreateLsn creates the lsn corrosponding to the given trie node.
 *
 ***********************************************/
kaps_lsn_mc *
kaps_trie_node_pvt_create_lsn(
    kaps_trie_node * self)
{
    kaps_lsn_mc *lsn_p;
    lsn_p = kaps_lsn_mc_create(self->m_trie_p->m_lsn_settings_p, self->m_trie_p, self->m_depth);
    lsn_p->m_pParentHandle = self->m_lp_prefix_p;
    return lsn_p;
}

/************************************************
 * Function :
 * kaps_trie_node_pvt_recreate_lsn
 *
 * Parameters:
 *    kaps_trie_node *self,
 *
 * Summary:
 * _RecreateLsn creates the lsn corrosponding to the given trie node.
 *
 ***********************************************/
kaps_lsn_mc *
kaps_trie_node_pvt_recreate_lsn(
    kaps_trie_node * self)
{
    /*
     * kaps_trie_node *parent;
     * 
     * parent = self->m_parent_p;
     */

    return (kaps_trie_node_pvt_create_lsn(self));
}

/************************************************
 * Function :
 * kaps_trie_node_update_iit
 *
 * Parameters:
 *   kaps_trie_node *self,

 *
 * Summary:
 * __UpdateSIT updates SIT value into hardware for the given lpm node.
 *
 ***********************************************/
NlmErrNum_t
kaps_trie_node_update_iit(
    kaps_trie_node * self,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;

    errNum = kaps_lsn_mc_update_iit(self->m_lsn_p, o_reason);

    return errNum;
}

/* It inserts the child node for given trie node. If child is 0 it inserts
 * left child otherwise right child.
 */
static kaps_trie_node *
kaps_trie_node_insert(
    kaps_trie_node * self,
    kaps_lpm_trie * trie,
    uint8_t child)
{
    kaps_trie_node *newnode_p;
    kaps_trie_node *nextnode_p;

    nextnode_p = self->m_child_p[child];

    newnode_p = kaps_trie_node_add_child(self, trie, child);

    newnode_p->m_child_p[child] = nextnode_p;
    if (nextnode_p)
        nextnode_p->m_parent_p = newnode_p;

    return newnode_p;
}

kaps_trie_node *
kaps_trie_node_insertPathFromPrefix(
    kaps_trie_node * self,
    kaps_prefix * prefix,
    uint32_t startBitPos,
    uint32_t endBitPos)
{
    uint8_t bit;
    uint32_t curBitPos = startBitPos;

    while (curBitPos <= endBitPos)
    {
        bit = KAPS_PREFIX_GET_BIT(prefix, curBitPos);
        if (self->m_child_p[bit] == NULL)
            kaps_trie_node_insert(self, self->m_trie_p, bit);
        self = self->m_child_p[bit];
        curBitPos++;
    }

    return self;
}

kaps_trie_node *
kaps_trie_node_insertPathFromPrefixBundle(
    kaps_trie_node * self,
    kaps_pfx_bundle * pfxBundle)
{
    uint8_t bit;
    uint32_t curBitPos = 0;

    while (curBitPos < pfxBundle->m_nPfxSize)
    {
        bit = KAPS_PREFIX_PVT_GET_BIT(pfxBundle->m_data, pfxBundle->m_nSize, curBitPos);

        if (self->m_child_p[bit] == NULL)
            kaps_trie_node_insert(self, self->m_trie_p, bit);

        self = self->m_child_p[bit];
        curBitPos++;
    }

    return self;
}

/************************************************
 * Function :
 * kaps_trie_node_remove_child_path
 *
 * Parameters:
 *    kaps_trie_node *self,
 *
 * Summary:
 * __RemoveChildPath removes path of that trie node from trie
 * if it is not a lp node.
 *
 ***********************************************/
void
kaps_trie_node_remove_child_path(
    kaps_trie_node * self)
{
    kaps_trie_node *cur_node_p = self;
    kaps_trie_node *parent_node_p;
    kaps_lpm_trie *trie_p = cur_node_p->m_trie_p;
    uint32_t list_size;

    /*
     * remove a node if it is not an LP node, no prefixes stored in the LSN and
     * * is a leaf node
     */
    while (cur_node_p)
    {
        if (kaps_lsn_mc_get_prefix_count(cur_node_p->m_lsn_p) || cur_node_p->m_child_p[NLMNSTRIE_LCHILD] != NULL
            || cur_node_p->m_child_p[NLMNSTRIE_RCHILD] != NULL)
            return;

        if (cur_node_p->m_depth <= trie_p->m_expansion_depth)
            return;

        parent_node_p = cur_node_p->m_parent_p;
        if (cur_node_p->m_node_type != NLMNSTRIENODE_LP && !cur_node_p->m_isRptNode && !cur_node_p->m_aptLmpsofarPfx_p)
        {
            if (cur_node_p->m_rptAptLmpsofarPfx)
                kaps_assert(0, "RPT APT Lmpsofar should be NULL \n");

            parent_node_p->m_child_p[cur_node_p->m_child_id] = NULL;

            list_size = trie_p->m_num_items_in_to_delete_nodes;
            if (list_size >= KAPS_MAX_SIZE_OF_TRIE_ARRAY)
                kaps_assert(0, "m_todelete_nodes overflow \n");

            trie_p->m_todelete_nodes[list_size] = cur_node_p;
            trie_p->m_num_items_in_to_delete_nodes++;

            cur_node_p = cur_node_p->m_parent_p;
        }
        else
        {
            break;
        }
    }
}

/*
 * Create an expanded sub-tree at 'self'. Expand to 'depth' levels
 */

static int32_t
kaps_trie_node_pvt_expand_trieHelper(
    kaps_trie_node * self,
    NlmNsTrieNode__TraverseArgs * args)
{
    uint32_t d = self->m_depth;

    if (d == KAPS_CAST_PTR_TO_U32(args->arg1))
    {
        return 0;
    }
    if (self->m_child_p[NLMNSTRIE_LCHILD] == NULL)
    {
        kaps_trie_node_add_child(self, self->m_trie_p, 0);
    }
    if (self->m_child_p[NLMNSTRIE_RCHILD] == NULL)
    {
        kaps_trie_node_add_child(self, self->m_trie_p, 1);
    }

    return 1;
}

/************************************************
 * Function :
 * kaps_trie_node_pvt_expand_trie
 *
 * Parameters:
 *    kaps_trie_node *self,
 *    uint32_t depth
 *
 * Summary:
 * __pvt_ExpandTrie Expand the trie to the given depth to
 * ensure minimum lopoff of longer prefix
 *
 ***********************************************/
void
kaps_trie_node_pvt_expand_trie(
    kaps_trie_node * self,
    uint32_t depth)
{
    NlmNsTrieNode__TraverseArgs args;
    args.arg1 = KAPS_CAST_U32_TO_PTR(depth);
    kaps_trie_node_traverse_dfs_preorder(self, kaps_trie_node_pvt_expand_trieHelper, &args);
}

static int32_t
kaps_trie_node_printHelper(
    kaps_trie_node * self,
    NlmNsTrieNode__TraverseArgs * args)
{
    FILE *fp = args->arg1;

    if (kaps_trie_node_pvt_is_lp_node(self) && (kaps_lsn_mc_get_prefix_count(self->m_lsn_p)))
    {
        kaps_lsn_mc_print(self->m_lsn_p, fp);
    }
    return 1;
}

/************************************************
 * Function :
 * kaps_trie_node_print
 *
 * Parameters:
 *    kaps_trie_node  *self,
 *    NlmCmFile        *fp
 *
 * Summary:
 * __Print prints information of all the nodes of subtrie starting from self.
 *
 ***********************************************/
void
kaps_trie_node_print(
    kaps_trie_node * self,
    FILE * fp)
{
    if (self && fp)
    {
        NlmNsTrieNode__TraverseArgs args;
        args.arg1 = (void *) fp;
        args.arg2 = NULL;
        kaps_trie_node_traverse_dfs_preorder(self, kaps_trie_node_printHelper, &args);
    }
}

kaps_trie_node *
kaps_trie_node_get_path(
    kaps_trie_node * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen)
{
    uint8_t bit;
    uint16_t depth;

    depth = self->m_depth;
    while (depth < pfxlen && self)
    {
        bit = KAPS_PREFIX_PVT_GET_BIT(pfxdata, pfxlen, depth);
        self = self->m_child_p[bit];
        depth++;
    }
    return self;
}

int32_t
kaps_trienode_wb_save_rpt(
    kaps_trie_node * rpt_node,
    uint32_t * nv_offset)
{
    kaps_trie_global *trie_global = rpt_node->m_trie_p->m_trie_global;
    struct kaps_wb_rpt_entry_info rpt_entry;
    uint32_t len_in_bytes;
    struct kaps_db *db = rpt_node->m_trie_p->m_tbl_ptr->m_db;

    rpt_entry.rpt_entry_len_1 = rpt_node->m_depth;

    if (db->num_algo_levels_in_db == 3)
    {

        len_in_bytes = KAPS_PFX_BUNDLE_GET_NUM_PFX_BYTES(rpt_node->m_rpt_prefix_p->m_nPfxSize);
        kaps_memcpy(rpt_entry.rpt_entry, rpt_node->m_rpt_prefix_p->m_data, len_in_bytes);
        rpt_entry.rpt_entry_location = rpt_node->m_rpt_prefix_p->m_nIndex;
    }

    rpt_entry.rpt_id = rpt_node->m_rptId;
    rpt_entry.rpt_uuid = rpt_node->m_rpt_uuid;
    rpt_entry.pool_id = rpt_node->m_poolId;
    rpt_entry.num_ipt_entries = rpt_node->m_numIptEntriesInSubtrie;
    rpt_entry.num_apt_entries = rpt_node->m_numAptEntriesInSubtrie;
    rpt_entry.num_reserved_160b_trig = rpt_node->m_numReserved160bTrig;

    len_in_bytes = sizeof(struct kaps_wb_rpt_entry_info);
    *nv_offset += len_in_bytes;
    return trie_global->wb_fun->write_fn(trie_global->wb_fun->handle,
                                         (uint8_t *) & rpt_entry, len_in_bytes, *nv_offset - len_in_bytes);

}

/*
lmpsofarPfx is the lmpsofar prefix bundle which should be propagated down
rqtPfx is the prefix bundle on which the user is performing the current operation (add prefix, delete prefix or update AD.
The rqtPfx will be needed for the delete and update operation. It is currently not needed for the add operation but is
passed anyway
*/
void
kaps_trie_node_update_iitLmpsofar(
    kaps_trie_node * self,
    kaps_trie_node * ancestorIptNode,
    kaps_pfx_bundle * lmpsofarPfx,
    kaps_pfx_bundle * rqtPfx,
    NlmTblRqtCode rqtCode,
    uint32_t recomputeLmpsofar)
{
    struct kaps_lsn_mc_settings *settings;
    struct kaps_device *device;
    struct kaps_hb *hb_entry;
    uint32_t is_candidate_node;
    kaps_pfx_bundle *recomputePfxBundle;
    uint32_t list_size;
    struct kaps_db *db;

    if (!self)
        return;

    settings = self->m_trie_p->m_lsn_settings_p;
    device = settings->m_device;
    db = self->m_trie_p->m_tbl_ptr->m_db;

    is_candidate_node = 0;
    if (self->m_node_type == NLMNSTRIENODE_LP)
        is_candidate_node = 1;

    if (self->m_trie_p->m_trie_global->m_isIITLmpsofar && db->num_algo_levels_in_db > 2 && self->m_isRptNode)
    {
        is_candidate_node = 1;
    }

    if (is_candidate_node)
    {

        if (rqtCode == NLM_FIB_PREFIX_INSERT || rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
        {
            /*
             * If the node is already having an lmpsofar and the lmpsofar is higher priority, then simply return
             */
            if (self->m_iitLmpsofarPfx_p)
            {
                if (self->m_iitLmpsofarPfx_p->m_backPtr->meta_priority < lmpsofarPfx->m_backPtr->meta_priority
                    || (self->m_iitLmpsofarPfx_p->m_backPtr->meta_priority == lmpsofarPfx->m_backPtr->meta_priority
                        && self->m_iitLmpsofarPfx_p->m_nPfxSize >= lmpsofarPfx->m_nPfxSize))
                {
                    return;
                }
            }

            if (self->m_iitLmpsofarPfx_p)
            {
                if (settings->m_areHitBitsPresent)
                {
                    hb_entry = NULL;
                    if (self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
                    {
                        struct kaps_db *db;
                        struct kaps_aging_entry *active_aging_table;

                        KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, self->m_iitLmpsofarPfx_p->m_backPtr, db);

                        KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                                (uintptr_t) self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

                        active_aging_table = kaps_device_get_active_aging_table(device, db);

                        active_aging_table[hb_entry->bit_no].num_idles = KAPS_HB_SPECIAL_VALUE;
                    }
                }
            }

            self->m_iitLmpsofarPfx_p = lmpsofarPfx;

        }
        else if (rqtCode == NLM_FIB_PREFIX_DELETE)
        {
            /*
             * A prefix delete operation is taking place. If the lmpsofar of the node is not equal to the prefix being
             * deleted then simply return 
             */
            if (self->m_iitLmpsofarPfx_p != rqtPfx)
                return;

            if (recomputeLmpsofar)
            {
                /*
                 * Let us that we are deleting /17 with meta priority 2. In the downstream LSNs, the prefix copies of
                 * /17 have not yet been removed. But we should not be considering these prefix copies while searching
                 * in the downstream LSNs since the original /17 is being deleted So we are passing rqtPfx->m_backPtr
                 * to skip this entry while locating the LPM in the LSN
                 */
                recomputePfxBundle = kaps_lsn_mc_locate_lpm(ancestorIptNode->m_lsn_p, self->m_lp_prefix_p->m_data,
                                                            self->m_lp_prefix_p->m_nPfxSize, rqtPfx->m_backPtr, NULL,
                                                            NULL);

                if (recomputePfxBundle && !recomputePfxBundle->m_isPfxCopy)
                {
                    if (!lmpsofarPfx
                        || recomputePfxBundle->m_backPtr->meta_priority < lmpsofarPfx->m_backPtr->meta_priority
                        || (recomputePfxBundle->m_backPtr->meta_priority == lmpsofarPfx->m_backPtr->meta_priority
                            && recomputePfxBundle->m_nPfxSize > lmpsofarPfx->m_nPfxSize))
                    {
                        lmpsofarPfx = recomputePfxBundle;
                    }
                }
            }

            if (self->m_iitLmpsofarPfx_p)
            {
                if (settings->m_areHitBitsPresent)
                {
                    hb_entry = NULL;
                    if (self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
                    {
                        struct kaps_db *db;
                        struct kaps_aging_entry *active_aging_table;

                        KAPS_CONVERT_DB_SEQ_NUM_TO_PTR(device, self->m_iitLmpsofarPfx_p->m_backPtr, db)
                            KAPS_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                                    (uintptr_t) self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

                        active_aging_table = kaps_device_get_active_aging_table(device, db);

                        active_aging_table[hb_entry->bit_no].num_idles = KAPS_HB_SPECIAL_VALUE;
                    }
                }
            }

            self->m_iitLmpsofarPfx_p = lmpsofarPfx;

        }
        else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
        {
            /*
             * The AD is being updated. If the lmpsofar of the trie node is not equal to the prefix whose AD is being
             * updated then simply return 
             */
            if (self->m_iitLmpsofarPfx_p != rqtPfx)
                return;
        }
        else
        {
            return;
        }

        if (self->m_node_type == NLMNSTRIENODE_LP)
        {
            list_size = self->m_trie_p->m_num_items_in_to_update_sit;

            if (list_size >= KAPS_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY)
                kaps_assert(0, "m_toupdate_sit overflow \n");

            self->m_trie_p->m_toupdate_sit[list_size] = self;
            self->m_trie_p->m_num_items_in_to_update_sit++;
        }

        if (self->m_isRptNode)
        {
            list_size = self->m_trie_p->m_num_items_in_to_update_rit;

            if (list_size >= KAPS_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY)
                kaps_assert(0, "m_toupdate_rit overflow \n");

            self->m_trie_p->m_toupdate_rit[list_size] = self;
            self->m_trie_p->m_num_items_in_to_update_rit++;

        }

        if (self->m_node_type == NLMNSTRIENODE_LP)
            ancestorIptNode = self;
    }

    if (self->m_child_p[0])
        kaps_trie_node_update_iitLmpsofar(self->m_child_p[0], ancestorIptNode, lmpsofarPfx, rqtPfx, rqtCode,
                                          recomputeLmpsofar);

    if (self->m_child_p[1])
        kaps_trie_node_update_iitLmpsofar(self->m_child_p[1], ancestorIptNode, lmpsofarPfx, rqtPfx, rqtCode,
                                          recomputeLmpsofar);
}

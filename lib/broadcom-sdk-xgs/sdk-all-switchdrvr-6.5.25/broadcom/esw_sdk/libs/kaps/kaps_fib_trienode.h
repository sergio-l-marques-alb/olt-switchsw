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

#ifndef INCLUDED_KAPS_TRIENODE_H
#define INCLUDED_KAPS_TRIENODE_H

#include "kaps_fib_cmn_pfxbundle.h"
#include "kaps_portable.h"
#include "kaps_fib_cmn_nstblrqt.h"
#include "kaps_device.h"

#include "kaps_externcstart.h"

#define NLMNSTRIE_MAXCHILD 2
#define NLMNSTRIE_LCHILD 0
#define NLMNSTRIE_RCHILD 1

#define NLM_INVALID_RPT_UUID 0xFFFF

/* Enum specifying type of trie node */
typedef enum NlmNsTrieNodeType
{
    NLMNSTRIENODE_REGULAR = 0,
    NLMNSTRIENODE_LP
} NlmNsTrieNodeType;

typedef struct NlmNsDownStreamRptNodes
{
    struct kaps_trie_node *rptNode;
    struct NlmNsDownStreamRptNodes *next;
} NlmNsDownStreamRptNodes;

/* Data structure of trie node */
typedef struct kaps_trie_node
{
    struct kaps_trie_node *m_parent_p;
    struct kaps_trie_node *m_child_p[NLMNSTRIE_MAXCHILD];
    struct kaps_lpm_trie *m_trie_p;
    struct kaps_lsn_mc *m_lsn_p;

    struct kaps_trie_node *m_rptParent_p;

    kaps_pfx_bundle *m_lp_prefix_p;     /* lp entree prefix */
    kaps_pfx_bundle *m_rpt_prefix_p;

    /*
     * Prefix bundle corresponding to the Auxiliary prefix table lmpsofar. Note that the trie node corresponding to the 
     * aptLmpsofarPfx_p is currently being stored in prefix bundle pointed to by m_aptLmpsofarPfx_p. This will never be 
     * prefix copy 
     */
    kaps_pfx_bundle *m_aptLmpsofarPfx_p;

    /*
     * Prefix bundle corresponding to the RPT's APT lmpsofar. This will be different from m_aptLmpsofarPfx_p. This
     * prefix bundle will always be a prefix copy. rptAptLmpsofarPfx will be set only if it has a better meta-priority
     * compared to aptLmpsofarPfx_p. So for instance, if we have 10.0/16 with meta priority of 2 which is the lmpsofar
     * of RPT entry 10.0.0/24. The user has also added entry 10.0.0/24 with meta priority 2. In this case, if we create 
     * a prefix copy of RPT lmpsofar 10.0/16 as 10.0.0/24 then we will have two entries in the APT - the user added
     * prefix 10.0.0/24 and the prefix copy of 10.0/16 with the same length and priority. In this case, we can't
     * guarantee which entry will produce a match from the TCAM. But in this case, we should get the user added prefix
     * as the result. To overcome this problem, we are setting rptLmpsofarPfx only if it has a higher priority than
     * m_aptLmpsofarPfx
     */
    kaps_pfx_bundle *m_rptAptLmpsofarPfx;

    /*
     * Prefix bundle corresponding to the Intermediate Information Table Lmpsofar AND also corresponding to the LSN
     * lmpsofar. So if the user has added prefix 10.0/16 which happens to be the lmpsofar of the LSN 10.0.0/24, then
     * m_iitLmpsofarPfx_p of the trie node at depth 24 will have the prefix bundle of the user added prefix 10.0/16.
     * The m_iitLmpsofarPfx_p will not have the expanded prefix 10.0.0/24. The m_iitLmpsofarPfx_p will contain ONLY
     * user added prefix and never contain a prefix copy (expanded prefix). The m_iitLmpsofarPfx_p will always have a
     * length that is LESS THAN the depth of the current LSN
     */
    kaps_pfx_bundle *m_iitLmpsofarPfx_p;

    NlmNsDownStreamRptNodes *m_downstreamRptNodes;

    uint16_t m_depth;           /* node depth */

    /*
     * Indicates the number of IPT entries that are present in the subtrie of the trie node. This value is maintained
     * correctly for RPT trie nodes at all times. For non-RPT trie nodes this value is correctly filled up only during
     * an RPT split 
     */
    uint16_t m_numIptEntriesInSubtrie;

    /*
     * Indicates the number of APT entries that are present in the subtrie of the trie node. This value is maintained
     * correctly for RPT trie nodes at all times. For non-RPT trie nodes this value is correctly filled up only during
     * an RPT split 
     */
    uint16_t m_numAptEntriesInSubtrie;

    uint32_t m_poolId;
    uint32_t m_trienodeId;

    uint16_t m_rptId;
    uint16_t m_rpt_uuid;        /* universally unique identifier for RPTs within the database. this will be stored
                                 * correctly only in the RPT node */

    uint32_t m_node_type:1;
    uint32_t m_numRptSplits:12;
    uint32_t m_child_id:1;
    uint32_t m_isRptNode:1;
    uint32_t m_isReserved160bTrig:1;
    uint32_t m_numReserved160bTrig:16;

} kaps_trie_node;

/* Data structure to store arguments which needs to
be passed to trie node while traversing trie */
typedef struct NlmNsTrieNode__TraverseArgs
{
    void *arg1;                 /* Optional Argument 1 */
    void *arg2;                 /* Optional Argument 2 */
    void *arg3;                 /* Optional Argument 3 */
    void *arg4;                 /* Optional Argument 4 */
    void *arg5;                 /* Optional Argument 5 */
} NlmNsTrieNode__TraverseArgs;


typedef enum kaps_recursion_status_t
{
    KAPS_RECURSION_STOP,
    KAPS_RECURSION_ERROR,
    KAPS_RECURSION_CONTINUE
} kaps_recursion_status;


typedef enum kaps_trie_node_processing_status_t
{
    KAPS_UNTRAVERSED_NODE,
    KAPS_LEFT_CHILD_EXPANDED,
    KAPS_RIGHT_CHILD_EXPANDED
} kaps_trie_node_processing_status;



typedef int32_t (
    *kaps_trie_node_traverse_dfs_visitor_t) (
    kaps_trie_node * node,
    NlmNsTrieNode__TraverseArgs * data);


typedef int (
    *kaps_trie_node_traverse_dfs_visitor_for_remove_t) (
    kaps_trie_node * node,
    NlmNsTrieNode__TraverseArgs * data);



/************************************************
 * Function :
 * kaps_trie_node_create_root_node
 *
 * Parameters:
 *    struct kaps_lpm_trie      *trie,
 *    uint8_t                tblId,
 *    uint32_t               child
 *
 * Summary:
 * __CreateRootNode creates the "root" node of the trie. It is at a
 * depth that is equal to tblid length as we always lopoff the tblid bits of the
 * prefix and so we can start from a node at depth=tblid_len and we don't need
 * the nodes above it
 *
 ***********************************************/
kaps_trie_node *kaps_trie_node_create_root_node(
    struct kaps_lpm_trie *trie,
    uint32_t child);

/************************************************
 * Function :
 * kaps_trie_node_destroy
 *
 * Parameters:
 *   kaps_trie_node      * self,
 *   kaps_nlm_allocator     * alloc,
 *   int32_t            delPfxes
 *
 * Summary:
 * __destroy destroys the trie from that node.
 *
 ***********************************************/
void kaps_trie_node_destroy(
    kaps_trie_node * self,
    kaps_nlm_allocator * alloc,
    int32_t delPfxes);

/************************************************
 * Function :
 * kaps_trie_node_pvt_recreate_lsn
 *
 * Parameters:
 *    kaps_trie_node         *self,
 *
 * Summary:
 * _RecreateLsn creates the lsn corresponding to the given trie node.
 *
 ***********************************************/
extern struct kaps_lsn_mc *kaps_trie_node_pvt_recreate_lsn(
    kaps_trie_node * self);

/************************************************
 * Function :
 * kaps_trie_node_pvt_create_lsn
 *
 * Parameters:
 *    kaps_trie_node         *self,
 *
 * Summary:
 * _CreateLsn creates the lsn corresponding to the given trie node.
 *
 ***********************************************/
extern struct kaps_lsn_mc *kaps_trie_node_pvt_create_lsn(
    kaps_trie_node * self);

/************************************************
 * Function :
 * kaps_trie_node_remove_child_path
 *
 * Parameters:
 *    kaps_trie_node         *self,
 *
 * Summary:
 * __RemoveChildPath removes path of that trie node from trie
 * if it is not a lp node.
 *
 ***********************************************/
void kaps_trie_node_remove_child_path(
    kaps_trie_node * self);

/************************************************
 * Function :
 * NlmNsTrieNode__GiveOut
 *
 * Parameters:
 *    kaps_trie_node         *self,
 *    kaps_trie_node         *target_node
 *
 * Summary:
 * __GiveOut divides a node into 2 pieces, where one piece is the
 * starting node and the other piece is a deeped node containing
 * between 1/3 and 2/3 of the original prefixes.
 *
 ***********************************************/
kaps_trie_node *NlmNsTrieNode__GiveOut(
    kaps_trie_node * self,
    kaps_trie_node * target_node);

/************************************************
 * Function :
 * kaps_trie_node_set_pfx_bundle
 *
 * Parameters:
 * kaps_trie_node        * self,
 * kaps_pfx_bundle       * pfx
 *
 * Summary:
 * __SetPfxBundle sets the given prefix bundle with given trie.
 *
 ***********************************************/
void kaps_trie_node_set_pfx_bundle(
    kaps_trie_node * self,
    kaps_pfx_bundle * pfx);

/************************************************
 * Function :
 * kaps_trie_node_update_iit
 *
 * Parameters:
 *   kaps_trie_node      *self,

 *
 * Summary:
 * __UpdateSIT updates SIT value into hardware for the given lpm node.
 *
 ***********************************************/
NlmErrNum_t kaps_trie_node_update_iit(
    kaps_trie_node * self,
    NlmReasonCode * o_reason);

/************************************************
 * Function :
 * kaps_trie_node_pvt_expand_trie
 *
 * Parameters:
 *    kaps_trie_node             *self,
 *    uint32_t                   depth
 *
 * Summary:
 * __pvt_ExpandTrie Expand the trie to the given depth to
 * ensure minimum lopoff of longer prefix
 *
 ***********************************************/
void kaps_trie_node_pvt_expand_trie(
    kaps_trie_node * self,
    uint32_t depth);

/* --- Trie stats print and verify  functions ---*/

void kaps_trie_node_print(
    kaps_trie_node * self,
    FILE * fp);

kaps_trie_node *kaps_trie_node_get_path(
    kaps_trie_node * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen);

kaps_trie_node *kaps_trie_node_insertPathFromPrefix(
    kaps_trie_node * self,
    kaps_prefix * prefix,
    uint32_t depth,
    uint32_t upto);

kaps_trie_node *kaps_trie_node_insertPathFromPrefixBundle(
    kaps_trie_node * self,
    kaps_pfx_bundle * pfxBundle);

void kaps_trie_node_update_iitLmpsofar(
    kaps_trie_node * self,
    kaps_trie_node * ancestorIptNode,
    kaps_pfx_bundle ** upStreamPfx_pp,
    kaps_pfx_bundle * rqtPfx,
    NlmTblRqtCode rqtCode);

void
kaps_trie_node_traverse_dfs_preorder(
    kaps_trie_node * node,
    kaps_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args);


void
kaps_trie_node_traverse_dfs_postorder(
    kaps_trie_node * node,
    kaps_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args);



/**
 * Saves RPT entry to non-volatile memory
 *
 * @param rpt_node RPT node to be saved
 * @param nv_offset Offset in the non-volatile memory to write to
 *
 * @return 0 on success or non-zero value otherwise
 */
int32_t kaps_trienode_wb_save_rpt(
    kaps_trie_node * rpt_node,
    uint32_t * nv_offset);

#endif /* INCLUDED_NLMNSTRIENODE_H */

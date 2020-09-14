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

#include "kaps_fibmgr.h"
#include "kaps_fib_trie.h"
#include "kaps_fib_hw.h"
#include "kaps_dbllinklist.h"
#include "kaps_algo_common.h"
#include "kaps_key_internal.h"
#include "kaps_ad_internal.h"

static void kaps_tbl_dtor(
    kaps_fib_tbl * self);

/*
Function: kaps_tbl_list_destroy_node
Parameters: kaps_dbl_link_list* node
            kaps_nlm_allocator* alloc
Return Type: void
Description: Deletes a node and free the memory held by the node. It calls the
destructor of fib table to free all the memory held by the fib table.
This function just deletes the node and do not update the doubly link list.
It takes the node pointer to be deleted as input and returns nothing.
*/
static void
kaps_tbl_list_destroy_node(
    kaps_dbl_link_list * node,
    void *arg)
{
    kaps_fib_tbl_list *fibTblList = (kaps_fib_tbl_list *) node;
    kaps_nlm_allocator *alloc = (kaps_nlm_allocator *) arg;
    kaps_tbl_dtor(fibTblList->m_fibTbl_p);

    kaps_nlm_allocator_free(alloc, fibTblList->m_fibTbl_p);

    kaps_nlm_allocator_free(alloc, fibTblList);
}

static void
kaps_fib_tbl_list_destroy_node(
    kaps_dbl_link_list * node,
    void *arg)
{
    kaps_nlm_allocator *alloc = (kaps_nlm_allocator *) arg;
    kaps_fib_tbl_list *fibDependentTblsList = (kaps_fib_tbl_list *) node;

    kaps_nlm_allocator_free(alloc, fibDependentTblsList);
}

/*
Function: kaps_tbl_list_destroy
Parameters: kaps_fib_tbl_list* head,
            kaps_nlm_allocator* alloc_p
Return Type: void
Description: This function destroys the whole fib table list. It takes the
list pointer which is "head" as input and returns nothing.
*/
static void
kaps_tbl_list_destroy(
    kaps_fib_tbl_list * head,
    kaps_nlm_allocator * alloc_p,
    kaps_dbl_link_list_destroy_node_t destroyNode)
{
    kaps_dbl_link_list_destroy((kaps_dbl_link_list *) head, destroyNode, alloc_p);
}

static void
kaps_tbl_dtor(
    kaps_fib_tbl * self)
{
    if (!self)
        return;
    if (self->m_dependentTbls_p)
    {
        kaps_tbl_list_destroy((kaps_fib_tbl_list *) self->m_dependentTbls_p, self->m_alloc_p,
                              kaps_fib_tbl_list_destroy_node);
    }
}

/*
Function: kaps_tbl_list_init
Parameters: kaps_nlm_allocator* alloc_p
Return Type: kaps_fib_tbl_list*
Description: Initializes empty fib table list by creating "head" node.
"head" contains "kaps_fib_tbl*" = NULL. Parameter alloc_p is
used for memory allocation. Function returns list pointer which is "head".
*/
static kaps_fib_tbl_list *
kaps_tbl_list_init(
    kaps_nlm_allocator * alloc_p)
{
    kaps_fib_tbl_list *head = kaps_nlm_allocator_calloc(alloc_p, 1,
                                                        sizeof(kaps_fib_tbl_list));

    if (!head)
        return NULL;

    kaps_dbl_link_list_init((kaps_dbl_link_list *) head);

    return head;
}

/*
Function: kaps_tbl_list_insert
Parameters: kaps_fib_tbl_list* head,
            kaps_fib_tbl* fibTbl_p,
            kaps_nlm_allocator* alloc_p
Return Type: kaps_fib_tbl_list*
Description: Creates a fib table list node containing a "kaps_fib_tbl"
pointer of newly created table and inserts this node into the fib table list.
It takes "head" as input and returns the newly created node pointer.
*/
static kaps_fib_tbl_list *
kaps_tbl_list_insert(
    kaps_fib_tbl_list * head,
    kaps_fib_tbl * fibTbl_p,
    kaps_nlm_allocator * alloc_p)
{
    kaps_fib_tbl_list *node = kaps_nlm_allocator_calloc(alloc_p, 1,
                                                        sizeof(kaps_fib_tbl_list));

    if (!node)
        return NULL;

    node->m_fibTbl_p = fibTbl_p;

    kaps_dbl_link_list_insert((kaps_dbl_link_list *) head, (kaps_dbl_link_list *) node);

    return node;
}

/*
Function: kaps_tbl_list_remove
Parameters: kaps_fib_tbl_list* node,
            kaps_nlm_allocator* alloc_p
Return Type: void
Description: This function removes a generic table list node from the list.
It actually calls "kaps_dbl_link_list_remove" which frees the node from the list
and update the list and then it calls "kaps_tbl_list_destroy_node" to free
the memory held by the node. It takes the node pointer to be deleted as input
and returns nothing.
*/
static void
kaps_tbl_list_remove(
    kaps_fib_tbl_list * node,
    kaps_nlm_allocator * alloc_p,
    kaps_dbl_link_list_destroy_node_t destroyNode)
{
    kaps_dbl_link_list_remove((kaps_dbl_link_list *) node, destroyNode, alloc_p);
}

static kaps_fib_tbl *
kaps_tbl_search(
    kaps_fib_tbl_list * head,
    uint8_t tblId,
    uint8_t core_id)
{
    kaps_fib_tbl_list *node;

    /*
     * First node in the list does not contain any table, advance to next node
     */
    node = head->m_next_p;

    while (node != head)
    {
        if (node->m_fibTbl_p->m_tblId == tblId && node->m_fibTbl_p->m_coreId == core_id)
            return node->m_fibTbl_p;
        node = node->m_next_p;
    }
    /*
     * If Not found
     */
    return NULL;
}

void
kaps_set_cur_fib_tbl_in_all_dev(
    kaps_fib_tbl_mgr * fibTblMgr,
    kaps_fib_tbl * fibTbl)
{
    struct kaps_device *main_device;
    struct kaps_device *cur_device;

    main_device = fibTblMgr->m_devMgr_p->main_dev;
    if (!main_device)
        main_device = fibTblMgr->m_devMgr_p;

    if (main_device->type == KAPS_DEVICE_KAPS)
    {
        fibTblMgr->m_curFibTbl = fibTbl;
        return;
    }

    fibTblMgr->m_curFibTbl = fibTbl;

    cur_device = main_device;
    while (cur_device != NULL)
    {
        cur_device->lpm_mgr->fib_tbl_mgr->m_curFibTbl = fibTbl;
        cur_device = cur_device->next_dev;
    }
}

static void
kaps_fib_tbl_mgr_ctor(
    kaps_fib_tbl_mgr * self,
    kaps_nlm_allocator * alloc_p,
    struct kaps_device *devMgr_p,
    kaps_fib_prefix_index_changed_app_cb_t indexChangedAppCb,
    struct kaps_lpm_mgr *lpm_mgr,
    NlmReasonCode * o_reason)
{
    self->m_alloc_p = alloc_p;
    self->m_devMgr_p = devMgr_p;
    self->m_fibTblCount = 0;
    self->m_fibTblList_p = NULL;
    self->m_IsConfigLocked = 0;
    self->m_indexChangeCallBackFn = indexChangedAppCb;
    self->m_lpm_mgr = lpm_mgr;

    /*
     * Initialize the table list
     */
    if ((self->m_fibTblList_p = kaps_tbl_list_init(alloc_p)) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return;
    }

    return;
}

static void
kaps_fib_tbl_mgr_dtor(
    kaps_fib_tbl_mgr * self)
{
    /*
     * Destroy the list of tbls maintained by the fib tbl mgr
     */
    if (self->m_fibTblList_p)
    {
        kaps_tbl_list_destroy((kaps_fib_tbl_list *) self->m_fibTblList_p, self->m_alloc_p, kaps_tbl_list_destroy_node);
    }
}

kaps_fib_tbl *
kaps_tbl_ctor(
    kaps_fib_tbl * self,
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t fibTblId,
    kaps_fib_tbl_index_range * fibTblIndexRange,
    uint16_t fibMaxPrefixLength,
    NlmReasonCode * o_reason)
{
    self->m_fibTblMgr_p = fibTblMgr;
    self->m_alloc_p = fibTblMgr->m_alloc_p;
    self->m_numPrefixes = 0;
    self->m_tblIndexRange.m_indexLowValue = fibTblIndexRange->m_indexLowValue;
    self->m_startBit = 159;

    if (fibTblIndexRange->m_indexHighValue == 0 && fibTblIndexRange->m_indexLowValue == 0)
        self->m_tblIndexRange.m_indexHighValue = KAPS_FIB_MAX_INDEX_RANGE;
    else if (fibTblMgr->m_devMgr_p->type == KAPS_DEVICE_KAPS)
        self->m_tblIndexRange.m_indexHighValue = self->m_db->common_info->capacity;
    else
        self->m_tblIndexRange.m_indexHighValue = fibTblIndexRange->m_indexHighValue;

    self->m_width = fibMaxPrefixLength;

    self->m_tblId = fibTblId;

    /*
     * Initialize the dependent tables list
     */
    if ((self->m_dependentTbls_p = kaps_tbl_list_init(fibTblMgr->m_alloc_p)) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    return self;
}

void
kaps_ftm_destroy_internal(
    kaps_fib_tbl_mgr * fibTblMgr)
{
    if (!fibTblMgr)
        return;

    /*
     * Destroy the Trie Module
     */
    if (fibTblMgr->m_trieGlobal)
        kaps_trie_module_destroy(fibTblMgr->m_trieGlobal);

    kaps_nlm_allocator_free(fibTblMgr->m_alloc_p, fibTblMgr->m_devWriteCallBackFns);
}

NlmErrNum_t
kaps_ftm_init_internal(
    kaps_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr_callback_fn_ptrs *callBackFnPtrs;
    
    struct kaps_lpm_mgr *lpm_mgr = fibTblMgr->m_lpm_mgr;

    if ((callBackFnPtrs =
         kaps_nlm_allocator_calloc(fibTblMgr->m_alloc_p, 1, sizeof(kaps_fib_tbl_mgr_callback_fn_ptrs))) == NULL)
    {
        kaps_ftm_destroy_internal(fibTblMgr);
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (fibTblMgr->m_devMgr_p->type == KAPS_DEVICE_KAPS)
    {
        /*
         * Initialize the trie module
         */
        if (lpm_mgr->curr_db->num_algo_levels_in_db == 2)
        {
            if ((fibTblMgr->m_trieGlobal =
                 kaps_trie_two_level_fib_module_init(fibTblMgr->m_alloc_p, fibTblMgr, o_reason)) == NULL)
            {
                /*
                 * If Trie module init fails destroy the data structures created
                 */
                kaps_ftm_destroy_internal(fibTblMgr);
                return NLMERR_FAIL;
            }
        }
        else
        {
            if ((fibTblMgr->m_trieGlobal =
                 kaps_trie_three_level_fib_module_init(fibTblMgr->m_alloc_p, fibTblMgr, 0, o_reason)) == NULL)
            {
                /*
                 * If Trie module init fails destroy the data structures created
                 */
                kaps_ftm_destroy_internal(fibTblMgr);
                return NLMERR_FAIL;
            }
        }

        callBackFnPtrs->m_writeABData = kaps_kaps_write_ab_data;
        callBackFnPtrs->m_deleteABData = kaps_kaps_delete_ab_data;
    }

    fibTblMgr->m_devWriteCallBackFns = callBackFnPtrs;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_create_table_internal(
    kaps_fib_tbl * fibTbl,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr = fibTbl->m_fibTblMgr_p;

    /*
     * perform some device specific error checks
     */
    if (fibTbl->m_width > KAPS_LPM_KEY_MAX_WIDTH_1)
    {
        *o_reason = NLMRSC_INVALID_FIB_MAX_PREFIX_LENGTH;
        return NLMERR_FAIL;
    }

    /*
     * Create a trie for the tbl
     */
    if ((fibTbl->m_trie = kaps_trie_create(fibTblMgr->m_trieGlobal, fibTbl, o_reason)) == NULL)
    {
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

kaps_fib_tbl_mgr *
kaps_ftm_kaps_init(
    kaps_nlm_allocator * alloc_p,
    struct kaps_device * devMgr_p,
    void *client_p,
    kaps_fib_prefix_index_changed_app_cb_t indexChangedAppCb,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *self = NULL;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    /*
     * perform error input checks
     */
    if (!alloc_p)
    {
        *o_reason = NLMRSC_INVALID_MEMALLOC_PTR;
        return NULL;
    }
    if (!devMgr_p)
    {
        *o_reason = NLMRSC_INVALID_DEVMGR_PTR;
        return NULL;
    }

    /*
     * Create fib tbl mgr
     */
    if ((self = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_fib_tbl_mgr))) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    /*
     * Invoke fib tbl mgr constructor which will initialize fib tbl mgr
     */
    kaps_fib_tbl_mgr_ctor(self, alloc_p, devMgr_p, indexChangedAppCb, client_p, o_reason);
    if (*o_reason != NLMRSC_REASON_OK)
    {
        kaps_nlm_allocator_free(alloc_p, self);
        return NULL;
    }

    self->m_numOfDevices = 1;

    if (NLMERR_OK != kaps_ftm_init_internal(self, o_reason))
    {
        /*
         * If Init fails destroy the data structures created
         */
        kaps_fib_tbl_mgr_dtor(self);
        kaps_nlm_allocator_free(alloc_p, self);
        return NULL;
    }

    self->m_arenaSizeInBytes = KAPS_DEFAULT_ARENA_SIZE_IN_BYTES;

    /*
     * initialize arena
     */
    if (!self->m_alloc_p->m_arena_info)
    {
        if (kaps_arena_init(self->m_alloc_p, self->m_arenaSizeInBytes, o_reason) != NLMERR_OK)
        {
            kaps_ftm_destroy(self, o_reason);
            self = NULL;
            *o_reason = NLMRSC_LOW_MEMORY;
        }
    }

    return self;
}

NlmErrNum_t
kaps_ftm_destroy(
    kaps_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_list *fibTblList_p;
    kaps_fib_tbl_list *fibTblListNextNode_p;
    kaps_nlm_allocator *alloc_p;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_FIB_MGR;
        return NLMERR_NULL_PTR;
    }

    /*
     * Destroy all the tables which are created and are not destroyed by the application
     */
    fibTblList_p = ((kaps_fib_tbl_list *) fibTblMgr->m_fibTblList_p)->m_next_p;
    while (fibTblList_p->m_fibTbl_p)
    {
        fibTblListNextNode_p = fibTblList_p->m_next_p;
        kaps_ftm_destroy_table(fibTblList_p->m_fibTbl_p, o_reason);
        fibTblList_p = fibTblListNextNode_p;
    }

    /*
     * Destroy the Trie Module
     */
    if (fibTblMgr->m_core1trieGlobal)
    {
        kaps_trie_module_destroy(fibTblMgr->m_core0trieGlobal);
        kaps_trie_module_destroy(fibTblMgr->m_core1trieGlobal);
    }
    else
    {
        if (fibTblMgr->m_trieGlobal)
            kaps_trie_module_destroy(fibTblMgr->m_trieGlobal);
    }

    kaps_nlm_allocator_free(fibTblMgr->m_alloc_p, fibTblMgr->m_devWriteCallBackFns);

    kaps_fib_tbl_mgr_dtor(fibTblMgr);
    alloc_p = fibTblMgr->m_alloc_p;

    kaps_nlm_allocator_free(alloc_p, fibTblMgr);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_destroy_table(
    kaps_fib_tbl * fibTbl,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    kaps_fib_tbl_list *node;
    kaps_fib_tbl_list *dependentTblNode;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    /*
     * Destroy the Trie corresponding to the tbl
     */
    if (fibTbl->m_trie)
        kaps_trie_destroy(fibTbl->m_trie);

    /*
     * Remove the table from the dependency list of the dependent tbls
     */
    if (fibTbl->m_dependentTbls_p)
    {
        node = ((kaps_fib_tbl_list *) fibTbl->m_dependentTbls_p)->m_next_p;
        while (node->m_fibTbl_p)
        {
            dependentTblNode = ((kaps_fib_tbl_list *) node->m_fibTbl_p->m_dependentTbls_p)->m_next_p;
            while (dependentTblNode->m_fibTbl_p)
            {
                if (dependentTblNode->m_fibTbl_p == fibTbl)
                    break;

                /*
                 * Move to the next node
                 */
                dependentTblNode = dependentTblNode->m_next_p;
            }
            if (dependentTblNode->m_fibTbl_p)
                kaps_tbl_list_remove(dependentTblNode, fibTbl->m_alloc_p, kaps_fib_tbl_list_destroy_node);

            node = node->m_next_p;
        }
    }

    /*
     * Get the node containing the tbl from tbls list
     */
    node = ((kaps_fib_tbl_list *) fibTblMgr->m_fibTblList_p)->m_next_p;
    while (node->m_fibTbl_p)
    {
        if (node->m_fibTbl_p == fibTbl)
            break;

        /*
         * Move to the next node
         */
        node = node->m_next_p;
    }
    /*
     * Remove the node from the list
     */
    if (node->m_fibTbl_p)
        kaps_tbl_list_remove(node, fibTbl->m_alloc_p, kaps_tbl_list_destroy_node);
    else
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_FAIL;
    }
    fibTblMgr->m_fibTblCount--;

    return NLMERR_OK;
}

kaps_fib_tbl *
kaps_ftm_create_table(
    kaps_fib_tbl_mgr * fibTblMgr,
    uint8_t fibTblId,           /* indicating Fib Tbl Id */
    kaps_fib_tbl_index_range * fibTblIndexRange,
    uint16_t fibMaxPrefixLength,
    struct kaps_db * db,
    uint32_t is_cascaded,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl *self;
    kaps_nlm_allocator *alloc_p;
    NlmReasonCode dummyVar;
    uint32_t width_1 = 0;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_FIB_MGR;
        return NULL;
    }

    if (1 == fibTblMgr->m_IsConfigLocked)
    {
        *o_reason = NLMRSC_CONFIGURATION_LOCKED;
        return NULL;
    }

    /*
     * Check if table with same tbl id already exist
     */
    if (NULL != kaps_tbl_search(fibTblMgr->m_fibTblList_p, fibTblId, is_cascaded))
    {
        *o_reason = NLMRSC_DUPLICATE_FIB_TBL_ID;
        return NULL;
    }

    if (fibTblIndexRange == NULL)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL_INDEX_RANGE;
        return NULL;
    }

    if (fibTblIndexRange->m_indexHighValue < fibTblIndexRange->m_indexLowValue
        || fibTblIndexRange->m_indexHighValue > KAPS_FIB_MAX_INDEX_RANGE)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL_INDEX_RANGE;
        return NULL;
    }

    alloc_p = fibTblMgr->m_alloc_p;
    if ((self = kaps_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_fib_tbl))) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    self->m_db = db;

    self->m_cascaded_fibtbl = is_cascaded;

    width_1 = db->key->width_1;

    if (db->common_info->is_extra_byte_added)
        width_1 += KAPS_BITS_IN_BYTE;

    /*
     * Set the fib table in all the devices since we need to write the default RPT entry in all devices
     */
    kaps_set_cur_fib_tbl_in_all_dev(fibTblMgr, self);

    /*
     * Invoke constructor of fib table
     */
    if ((self = kaps_tbl_ctor(self, fibTblMgr, fibTblId, fibTblIndexRange, fibMaxPrefixLength, o_reason)) == NULL)
    {
        return NULL;
    }

    if (NLMERR_OK != kaps_ftm_create_table_internal(self, o_reason))
    {
        /*
         * if table is not being created then call table_destructor and free the memory
         */
        kaps_tbl_dtor(self);
        kaps_nlm_allocator_free(alloc_p, self);
        return NULL;
    }

    /*
     * insert the table created into doubly link list
     */
    if (NULL == kaps_tbl_list_insert((kaps_fib_tbl_list *) fibTblMgr->m_fibTblList_p, self, alloc_p))
    {
        kaps_tbl_dtor(self);
        kaps_nlm_allocator_free(alloc_p, self);
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    if (!self->m_trie->m_trie_global->fibtblmgr->m_devMgr_p->hw_res->no_dba_compression)
    {
        if (width_1 > 80)
        {
            self->m_trie->m_maxAllowedAptEntriesForRpt =
                (fibTblMgr->m_devMgr_p->num_80b_entries_ab / 2) -
                fibTblMgr->m_trieGlobal->poolMgr->m_numAlwaysEmptyAptSlotsPerPool;
            self->m_trie->m_maxAllowedIptEntriesForRpt =
                (fibTblMgr->m_devMgr_p->num_80b_entries_ab / 2) -
                fibTblMgr->m_trieGlobal->poolMgr->m_numAlwaysEmptyIptSlotsPerPool;
        }
        else
        {
            self->m_trie->m_maxAllowedAptEntriesForRpt =
                fibTblMgr->m_devMgr_p->num_80b_entries_ab -
                fibTblMgr->m_trieGlobal->poolMgr->m_numAlwaysEmptyAptSlotsPerPool;
            self->m_trie->m_maxAllowedIptEntriesForRpt =
                fibTblMgr->m_devMgr_p->num_80b_entries_ab -
                fibTblMgr->m_trieGlobal->poolMgr->m_numAlwaysEmptyIptSlotsPerPool;
        }
    }
    else
    {
        if (width_1 > 80)
        {
            self->m_trie->m_maxAllowedAptEntriesForRpt = (fibTblMgr->m_devMgr_p->num_80b_entries_ab / 2);
            self->m_trie->m_maxAllowedIptEntriesForRpt = (fibTblMgr->m_devMgr_p->num_80b_entries_ab / 2);
        }
        else
        {
            self->m_trie->m_maxAllowedAptEntriesForRpt = fibTblMgr->m_devMgr_p->num_80b_entries_ab;
            self->m_trie->m_maxAllowedIptEntriesForRpt = fibTblMgr->m_devMgr_p->num_80b_entries_ab;
        }
    }

    /*
     * Do not directly set these values to 1000 as we may have a palladium device
     */
    if (self->m_trie->m_maxAllowedIptEntriesForRpt > 1000)
    {
        self->m_trie->m_maxAllowedIptEntriesForRpt = 1000;
    }

    if (self->m_trie->m_maxAllowedAptEntriesForRpt > 1000)
    {
        self->m_trie->m_maxAllowedAptEntriesForRpt = 1000;
    }

    fibTblMgr->m_fibTblCount++;

    self->m_coreId = is_cascaded;

    return self;
}

NlmErrNum_t
kaps_ftm_add_prefix_internal(
    kaps_fib_tbl * fibTbl,      /* FibTbl to which prefix is added */
    struct kaps_lpm_entry * entry,      /* represents prefix being added */
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;
    struct kaps_device *device = NULL;
    uint32_t i = 0, num_bytes;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }

    device = fibTbl->m_fibTblMgr_p->m_devMgr_p;

    if (!device)
    {
        *o_reason = NLMRSC_INVALID_DEV_PTR;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_NULL_PTR;
    }
    if (entry->pfx_bundle->m_nPfxSize > fibTbl->m_width)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_FAIL;
    }

    if (device->main_dev)
        device = device->main_dev;

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    fibTbl->m_db->common_info->compaction_done = 0;

    /*
     * Invoke the Trie Submit Rqt which performs addition of specified prefix
     */
    errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);

    /*
     * If we ran out of UDA, first try to compact the UDA and retry
     */
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_status status = KAPS_OUT_OF_UDA;

        status = kaps_kaps_uda_mgr_compact_all_regions(mgr);
        if (status == KAPS_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDACompactionCalls++;
        }

        if (status == KAPS_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    /*
     * Try to allocate a new region that has a width of maxLpuPerLsn
     */
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_status status = KAPS_OUT_OF_UDA;

        status = kaps_uda_mgr_dynamic_uda_alloc_n_lpu(mgr, fibTbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn);
        if (status == KAPS_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDADynamicAllocCalls++;
        }

        if (status == KAPS_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    /*
     * For non-XOR cases, we haven't yet tried expanding the current region. So try to expand the existing region.
     * Region expansion will involve UDA compaction
     */
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_status status = KAPS_OUT_OF_UDA;

        status = kaps_resource_expand_uda_mgr_regions(mgr);
        if (status == KAPS_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDAExpansionCalls++;
        }

        if (status == KAPS_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    /*
     * Try to allocate a new region that has a width that is smaller than maxLpuPerLsn
     */
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_status status = KAPS_OUT_OF_UDA;
        int32_t num_udms_required;

        num_udms_required = fibTbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn - 1;
        while (num_udms_required)
        {
            status = kaps_uda_mgr_dynamic_uda_alloc_n_lpu(mgr, num_udms_required);
            if (status == KAPS_OK)
                break;
            num_udms_required--;
        }

        if (status == KAPS_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDADynamicAllocCalls++;
        }

        if (status == KAPS_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);

            if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
            {
                status = kaps_uda_mgr_release_last_allocated_regions(mgr);
                kaps_sassert(status == KAPS_OK);
            }
        }
    }

    kaps_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    fibTbl->m_numPrefixes++;

    num_bytes = (entry->pfx_bundle->m_nPfxSize + 7) / 8;
    for (i = 0; i < num_bytes; ++i)
    {
        device->hw_res->running_sum += entry->pfx_bundle->m_data[i];
    }

    return errNum;
}

NlmErrNum_t
kaps_ftm_add_prefix(
    struct kaps_lpm_db * lpm_db,        /* FibTbl to which prefix is added */
    struct kaps_lpm_entry * entry,      /* represents prefix being added */
    NlmReasonCode * o_reason)
{
    NlmErrNum_t err_num = NLMERR_OK;

    err_num = kaps_ftm_add_prefix_internal(lpm_db->fib_tbl, entry, o_reason);

    return err_num;
}

NlmErrNum_t
kaps_ftm_wb_add_prefix(
    kaps_fib_tbl * fibTbl,      /* FibTbl to which prefix is added */
    struct kaps_lpm_entry * entry,      /* represents prefix being added */
    uint32_t index,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_NULL_PTR;
    }
    if (entry->pfx_bundle->m_nPfxSize > fibTbl->m_width)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_FAIL;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    entry->pfx_bundle->m_nIndex = index;

    /*
     * Invoke the Trie Submit Rqt which performs addition of specified prefix
     */
    errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT_WITH_INDEX, entry, o_reason);
    kaps_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    if (!entry->pfx_bundle->m_isPfxCopy)
        fibTbl->m_numPrefixes++;

    return errNum;
}

NlmErrNum_t
kaps_ftm_delete_prefix(
    kaps_fib_tbl * fibTbl,      /* FibTbl from which prefix is to be removed */
    struct kaps_lpm_entry * entry,      /* represents prefix being deleted */
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;
    uint32_t i, num_bytes;
    struct kaps_device *device;

    if (!o_reason)
        o_reason = &dummyVar;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }

    *o_reason = NLMRSC_REASON_OK;

    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    /*
     * Invoke the Trie Submit Rqt which performs deletion of specified prefix
     */
    errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_DELETE, entry, o_reason);
    kaps_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    fibTbl->m_numPrefixes--;

    device = fibTbl->m_fibTblMgr_p->m_devMgr_p;
    if (device->main_dev)
        device = device->main_dev;

    num_bytes = (entry->pfx_bundle->m_nPfxSize + 7) / 8;
    for (i = 0; i < num_bytes; ++i)
    {
        device->hw_res->running_sum -= entry->pfx_bundle->m_data[i];
    }

    return errNum;
}

NlmErrNum_t
kaps_ftm_get_index(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry * entry,
    int32_t * nindices,
    int32_t ** indices,
    NlmReasonCode * o_reason)
{
    int32_t *ret_indices = NULL;
    int32_t num_indices;
    kaps_fib_tbl_mgr *fibTblMgr;
    NlmReasonCode dummyVar;
    struct kaps_allocator *alloc;
    uint32_t mainPfxIndex;
    kaps_pfx_bundle *mainPfxBundle = NULL;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    if (entry->pfx_bundle->m_nPfxSize > fibTbl->m_width)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_FAIL;
    }

    alloc = fibTbl->m_fibTblMgr_p->m_devMgr_p->alloc;
    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    num_indices = 1;
    if (entry->pfx_bundle->m_status == 0)
        mainPfxIndex = KAPS_FIB_INVALID_INDEX;
    else
        mainPfxIndex = kaps_trie_locate_exact(fibTbl->m_trie, entry->pfx_bundle->m_data,
                                              entry->pfx_bundle->m_nPfxSize, &mainPfxBundle, o_reason);

    ret_indices = alloc->xcalloc(alloc->cookie, 1, (num_indices * sizeof(int32_t)));
    if (!ret_indices)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    ret_indices[0] = mainPfxIndex;

    *indices = ret_indices;
    *nindices = num_indices;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_locate_lpm(
    kaps_fib_tbl * fibTbl,
    uint8_t * key,
    struct kaps_lpm_entry ** entry_pp,
    uint32_t * index,
    uint32_t * length,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    kaps_pfx_bundle *pfxBundle;
    NlmReasonCode dummyVar;
    struct kaps_device *device;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!key)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    device = fibTbl->m_fibTblMgr_p->m_devMgr_p;
    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    pfxBundle = kaps_trie_locate_lpm(fibTbl->m_trie, key, fibTbl->m_width, index, o_reason);

    if (pfxBundle)
    {
        *entry_pp = pfxBundle->m_backPtr;
        *length = pfxBundle->m_nPfxSize;

        if (pfxBundle->m_isPfxCopy)
            *length = pfxBundle->m_backPtr->pfx_bundle->m_nPfxSize;

        if (pfxBundle->m_backPtr->ad_handle && device->type != KAPS_DEVICE_KAPS)
        {
            struct kaps_ad_db *ad_db;
            enum kaps_ad_type ad_type;

            KAPS_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, pfxBundle->m_backPtr->ad_handle, ad_db);
            ad_type = kaps_resource_get_ad_type(&ad_db->db_info);

            if (ad_db->user_width_1 != 0)
            {
                if (ad_type == KAPS_AD_TYPE_INPLACE)
                {
                    *index = pfxBundle->m_backPtr->ad_handle->value[0] << 16;
                    *index |= pfxBundle->m_backPtr->ad_handle->value[1] << 8;
                    *index |= pfxBundle->m_backPtr->ad_handle->value[2];
                }
            }
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_update_ad(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry * entry,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_UPDATE_AD, entry, o_reason);

    kaps_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    return errNum;
}

NlmErrNum_t
kaps_ftm_update_ad_address(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry * entry,
    NlmReasonCode * o_reason)
{
    kaps_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_UPDATE_AD_ADDRESS, entry, o_reason);

    kaps_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    return errNum;
}

NlmErrNum_t
kaps_ftm_get_resource_usage(
    kaps_fib_tbl * fibTbl,
    kaps_fib_resource_usage * rxcUsage,
    NlmReasonCode * o_reason)
{
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_FAIL;
    }

    if (!rxcUsage)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    kaps_trie_get_resource_usage(fibTbl, rxcUsage);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_get_prefix_handle(
    kaps_fib_tbl * fibTbl,
    uint8_t * data,
    uint32_t length,
    struct kaps_lpm_entry ** entry)
{
    struct pfx_hash_table *hash_table = fibTbl->m_trie->m_hashtable_p;
    struct kaps_entry **ht_slot = NULL;
    kaps_status status;

    status = kaps_pfx_hash_table_locate(hash_table, data, length, &ht_slot);

    if (status != KAPS_OK)
    {
        *entry = NULL;
        return NLMERR_FAIL;
    }

    if (!ht_slot)
    {
        *entry = NULL;
        return NLMERR_OK;
    }

    *entry = *((struct kaps_lpm_entry **) ht_slot);
    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_update_hash_table(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry * entry,
    uint32_t is_del)
{
    struct pfx_hash_table *hash_table = fibTbl->m_trie->m_hashtable_p;
    kaps_status status;
    struct kaps_entry **ht_slot;

    if (!entry)
        return NLMERR_FAIL;

    status =
        kaps_pfx_hash_table_locate(fibTbl->m_trie->m_hashtable_p, entry->pfx_bundle->m_data, entry->pfx_bundle->m_nPfxSize,
                              &ht_slot);
    if (status != KAPS_OK)
    {
        return NLMERR_FAIL;
    }

    if (is_del)
    {
        if (!ht_slot)
            return NLMERR_FAIL;
        status = kaps_pfx_hash_table_delete(hash_table, ht_slot);
    }
    else
    {
        struct kaps_entry *ht_entry = (struct kaps_entry *) entry;
        if (ht_slot)
            return NLMERR_FAIL;

        status = kaps_pfx_hash_table_insert(hash_table, ht_entry);
    }

    if (status != KAPS_OK)
    {
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_get_prefix_location(
    kaps_fib_tbl * fibTbl,
    struct kaps_lpm_entry * lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row,
    NlmReasonCode * o_reason)
{
    *o_reason = NLMRSC_REASON_OK;

    kaps_trie_get_prefix_location(fibTbl->m_trie, lpm_entry, abs_udc, abs_row);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_ftm_process_hit_bits(
    kaps_fib_tbl * fibTbl,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_device *device = fibTbl->m_fibTblMgr_p->m_devMgr_p;

    *o_reason = NLMRSC_REASON_OK;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        errNum = kaps_trie_process_hit_bits_iit_lmpsofar(fibTbl->m_trie);
    }

    return errNum;
}

NlmErrNum_t
kaps_ftm_get_algo_hit_bit_value(
    kaps_fib_tbl * fibTbl,
    struct kaps_entry * entry,
    uint8_t clear_on_read,
    uint32_t * bit_value,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_device *device = fibTbl->m_fibTblMgr_p->m_devMgr_p;

    if (device->type == KAPS_DEVICE_KAPS)
    {
        errNum =
            kaps_trie_get_algo_hit_bit_value(fibTbl->m_trie, (struct kaps_lpm_entry *) entry, clear_on_read, bit_value);
    }

    return errNum;
}

void
kaps_kaps_print_rpb_stats_html(
    FILE * f,
    struct kaps_device *dev,
    struct kaps_lpm_db *db,
    struct NlmFibStats *fib_stats)
{
/*    uint32_t i;*/

    kaps_fprintf(f, "<h3>RPB Stats</h3>\n");
    kaps_fprintf(f, "<table style= \"float: left; margin-right:50px;\">\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>Total</th>\n");
    kaps_fprintf(f, "  <th>Used</th>\n");
    kaps_fprintf(f, "  <th>Avg Width (Bytes)</th>\n");
    kaps_fprintf(f, "</tr>\n");

    if (fib_stats->numRPTEntries)
    {
        kaps_fprintf(f, "<tr>\n");
        kaps_fprintf(f, "  <td>%d</td>\n", db->fib_tbl->m_trie->m_trie_global->m_rpm_p->kaps_simple_dba.pool_size);
        kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numRPTEntries);
        kaps_fprintf(f, "  <td>%.02f</td>\n",
                     (float) ((1.0 * fib_stats->totalRptBytesLoppedOff) / fib_stats->numRPTEntries));
        kaps_fprintf(f, "</tr>\n");
    }

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");

    kaps_fprintf(f, "<br>\n");

#if 0
    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>Length</th>\n");
    kaps_fprintf(f, "  <th>Num Entries</th>\n");
    kaps_fprintf(f, "  <th>Avg Width</th>\n");
    kaps_fprintf(f, "</tr>\n");

    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_8; ++i)
    {
        if (fib_stats.rpt_lopoff_info[i])
        {
            kaps_printf("                Length = %d bytes, Num entries = %d\n", i, fib_stats.rpt_lopoff_info[i]);
        }
    }

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");

    kaps_fprintf(f, "<br>\n");
#endif
}

void
kaps_kaps_print_large_bb_stats_html(
    FILE * f,
    struct kaps_device *device,
    struct kaps_db *db,
    struct NlmFibStats *fib_stats)
{
    struct kaps_lpm_db *lpm_db;
    struct large_kaps2_stats l_bb_stats;
    struct kaps_db_stats stats;
    uint32_t i, gran;

    (void) lpm_db;
    lpm_db = (struct kaps_lpm_db *) db;
    kaps_ftm_large_kaps2_calc_stats(db, &l_bb_stats);
    /*
     * coverity[check_return]
     */
    /*
     * coverity[unchecked_value]
     */
    kaps_db_stats(db, &stats);

    /*
     * Legend of the databases
     */
    kaps_fprintf(f, "<h3>Large BB Stats</h3>\n");

    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>Total Bricks</th>\n");
    kaps_fprintf(f, "  <th>Used Bricks</th>\n");
    kaps_fprintf(f, "  <th>Free Bricks</th>\n");
    kaps_fprintf(f, "  <th>Total LSN Holes</th>\n");
    kaps_fprintf(f, "  <th>Max LSN Width</th>\n");
    kaps_fprintf(f, "  <th>Avg LSN Width</th>\n");
    kaps_fprintf(f, "  <th>Bits / Prefix</th>\n");
    kaps_fprintf(f, "</tr>\n");

    kaps_fprintf(f, "<tr>\n");
    kaps_fprintf(f, "  <td>%d</td>\n", l_bb_stats.total_large_bb_rows);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numUsedBricks);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numFreeBricks);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->totalNumHolesInLsns);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->maxLsnWidth);
    kaps_fprintf(f, "  <td><font font-weight=\"bold\">%.02f</font></td>\n",
                 (float) ((1.0 * fib_stats->numUsedBricks) / fib_stats->numIPTEntries));
    kaps_fprintf(f, "  <td>%.02f</td>\n",
                 (1.0 * l_bb_stats.total_large_bb_rows * KAPS_BKT_WIDTH_1) / stats.num_entries);
    kaps_fprintf(f, "</tr>\n");

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");

    kaps_fprintf(f, "<br>\n");

    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>Gran</th>\n");
    kaps_fprintf(f, "  <th>Num Bricks</th>\n");
    kaps_fprintf(f, "  <th>Num Prefixes</th>\n");
    kaps_fprintf(f, "  <th>Num Holes</th>\n");
    kaps_fprintf(f, "</tr>\n");

    for (i = 0; i < device->hw_res->lpm_num_gran; ++i)
    {
        gran = device->hw_res->lpm_gran_array[i];
        if (fib_stats->numBricksForEachGran[i])
        {
            kaps_fprintf(f, "<tr>\n");
            kaps_fprintf(f, "  <td>%d</td>\n", gran);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numBricksForEachGran[i]);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numPfxForEachGran[i]);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numHolesForEachGran[i]);
            kaps_fprintf(f, "</tr>\n");
        }
    }

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");

    kaps_fprintf(f, "<br>\n");

    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>LSN Width</th>\n");
    kaps_fprintf(f, "  <th>Num LSNs</th>\n");
    kaps_fprintf(f, "  <th>Num Holes</th>\n");
    kaps_fprintf(f, "</tr>\n");

    for (i = 1; i <= lpm_db->fib_tbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn; i++)
    {
        if (fib_stats->numLsnForEachSize[i - 1])
        {
            kaps_fprintf(f, "<tr>\n");
            kaps_fprintf(f, "  <td>%d</td>\n", i);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numLsnForEachSize[i - 1]);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->numHolesForEachLsnSize[i - 1]);
            kaps_fprintf(f, "</tr>\n");
        }
    }

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");
}

void
kaps_kaps_print_small_bb_stats_html(
    FILE * f,
    struct kaps_device *device,
    struct kaps_db *db,
    struct NlmFibStats *fib_stats)
{
    struct kaps_lpm_db *lpm_db;
    uint32_t i, gran;

    (void) lpm_db;
    lpm_db = (struct kaps_lpm_db *) db;

    /*
     * Legend of the databases
     */
    kaps_fprintf(f, "<h3>Small BB Stats</h3>\n");

    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>Total Bricks</th>\n");
    kaps_fprintf(f, "  <th>Used Bricks</th>\n");
    kaps_fprintf(f, "  <th>Free Bricks</th>\n");
    kaps_fprintf(f, "  <th>Total Triggers</th>\n");
    kaps_fprintf(f, "  <th>Total LSN Holes</th>\n");
    kaps_fprintf(f, "  <th>Max LSN Width</th>\n");
    kaps_fprintf(f, "  <th>Avg LSN Width</th>\n");
    kaps_fprintf(f, "</tr>\n");

    kaps_fprintf(f, "<tr>\n");
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.maxBricks);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numUsedBricks);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numFreeBricks);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.totalNumEntriesInLsns);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.totalNumHolesInLsns);
    kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.maxLsnWidth);
    kaps_fprintf(f, "  <td><font font-weight=\"bold\">%.02f</font></td>\n",
                 (float) ((1.0 * fib_stats->kaps_small_bb_stats.numUsedBricks) / fib_stats->numRPTEntries));
    kaps_fprintf(f, "</tr>\n");

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");

    kaps_fprintf(f, "<br>\n");

    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>Gran</th>\n");
    kaps_fprintf(f, "  <th>Num Bricks</th>\n");
    kaps_fprintf(f, "  <th>Num Prefixes</th>\n");
    kaps_fprintf(f, "  <th>Num Holes</th>\n");
    kaps_fprintf(f, "</tr>\n");

    for (i = 0; i < device->hw_res->lpm_num_gran; i++)
    {
        gran = device->hw_res->lpm_gran_array[i];
        if (fib_stats->kaps_small_bb_stats.numBricksForEachGran[i])
        {
            kaps_fprintf(f, "<tr>\n");
            kaps_fprintf(f, "  <td>%d</td>\n", gran);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numBricksForEachGran[i]);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numPfxForEachGran[i]);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numHolesForEachGran[i]);
            kaps_fprintf(f, "</tr>\n");
        }
    }

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");

    kaps_fprintf(f, "<br>\n");

    kaps_fprintf(f, "<table>\n");
    kaps_fprintf(f, "<tbody>\n");
    kaps_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_fprintf(f, "  <th>LSN Width</th>\n");
    kaps_fprintf(f, "  <th>Num LSNs</th>\n");
    kaps_fprintf(f, "  <th>Num Holes</th>\n");
    kaps_fprintf(f, "</tr>\n");

    for (i = 1; i <= fib_stats->kaps_small_bb_stats.maxLsnWidth; i++)
    {
        if (fib_stats->kaps_small_bb_stats.numLsnForEachSize[i - 1])
        {
            kaps_fprintf(f, "<tr>\n");
            kaps_fprintf(f, "  <td>%d</td>\n", i - 1);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numLsnForEachSize[i - 1]);
            kaps_fprintf(f, "  <td>%d</td>\n", fib_stats->kaps_small_bb_stats.numHolesForEachLsnSize[i - 1]);
            kaps_fprintf(f, "</tr>\n");
        }
    }

    kaps_fprintf(f, "</tbody>\n");
    kaps_fprintf(f, "</table>\n");
}

void
kaps_kaps_print_detailed_stats_html(
    FILE * f,
    struct kaps_device *dev,
    struct kaps_db *db)
{
    struct kaps_db_stats stats;
    struct NlmFibStats *fib_stats_ptr;
    struct kaps_lpm_db *lpm_db;

    (void) kaps_db_stats(db, &stats);

    if (stats.num_entries == 0)
        return;

    lpm_db = (struct kaps_lpm_db *) db;

    fib_stats_ptr =
        lpm_db->db_info.device->alloc->xmalloc(lpm_db->db_info.device->alloc->cookie, sizeof(struct NlmFibStats));
    if (fib_stats_ptr == NULL)
        return;

    (void) kaps_lpm_db_get_fib_stats(lpm_db, fib_stats_ptr);
#if 0
    kaps_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n");
    kaps_fprintf(f, "\"http://www.w3.org/TR/html4/strict.dtd\">\n");
    kaps_fprintf(f, "<HTML lang=\"en\">\n");
    kaps_fprintf(f, "<HEAD>\n");
    kaps_fprintf(f, "<STYLE type=\"text/css\">\n");
    kaps_fprintf(f, "td,th {padding-bottom:4pt;padding-top:4pt;padding-left:4pt;padding-right:4pt}\n");
    kaps_fprintf(f, "table {border-collapse:collapse}\n");
    kaps_fprintf(f, "td {text-align:center;font-family:Courier New;font-weight:bold;font-size:100%%}\n");
    kaps_fprintf(f, "table,td,th {border:2px solid #adafb2}\n");
    kaps_fprintf(f, "tr.broadcom1 {background:#e31837;color:#ffffff}\n");
    kaps_fprintf(f, "tr.broadcom2 {background:#005568;color:#ffffff}\n");
    kaps_fprintf(f, "tr.broadcom3 {background:#ffd457;color:#000000}\n");
    kaps_fprintf(f, "body {margin:20px 20px 20px 20px}\n");
    kaps_fprintf(f, "</STYLE>\n");
    kaps_fprintf(f, "</HEAD>\n");

    kaps_fprintf(f, "<BODY>\n");
#endif

    kaps_fprintf(f, "<h2>TYPE %s Capacity %d </h2>\n", db->is_type_a ? "A" : "B", stats.num_entries);

    /* kaps_kaps_print_rpb_stats_html(f, dev, (struct kaps_lpm_db *)db, fib_stats_ptr);*/

    kaps_kaps_print_small_bb_stats_html(f, dev, db, fib_stats_ptr);

    kaps_kaps_print_large_bb_stats_html(f, dev, db, fib_stats_ptr);

#if 0
    kaps_fprintf(f, "</BODY>\n");
    kaps_fprintf(f, "</HTML>\n");
#endif
    lpm_db->db_info.device->alloc->xfree(lpm_db->db_info.device->alloc->cookie, fib_stats_ptr);
}

NlmErrNum_t
kaps_ftm_calc_stats(
    kaps_fib_tbl * fibTbl)
{
    kaps_trie_global *trieGlobal_p = fibTbl->m_fibTblMgr_p->m_trieGlobal;
    struct NlmFibStats *fibStats = &fibTbl->m_fibStats;
    struct uda_mgr_stats uda_stats;
    struct kaps_ix_mgr_stats ix_stats = { 0 };
    uint32_t i;
    struct kaps_ad_db *ad_db;

    for (i = 0; i < KAPS_HW_MAX_NUM_LPU_GRAN; ++i)
    {
        fibStats->numBricksForEachGran[i] = 0;
        fibStats->numEmptyBricksForEachGran[i] = 0;
        fibStats->numPfxForEachGran[i] = 0;
        fibStats->numHolesForEachGran[i] = 0;
    }

    for (i = 0; i < KAPS_HW_MAX_LPUS_PER_LPM_DB; ++i)
    {
        fibStats->numLsnForEachSize[i] = 0;
        fibStats->numHolesForEachLsnSize[i] = 0;
    }

    for (i = 0; i <= KAPS_LPM_KEY_MAX_WIDTH_8; ++i)
    {
        fibStats->rpt_lopoff_info[i] = 0;
    }

    for (i = 0; i < KAPS_MAX_NUM_POOLS; ++i)
    {
        fibStats->isABUsed[i] = 0;
        fibStats->widthOfAB[i] = 0;
        fibStats->numTriggersInAB[i] = 0;
        fibStats->numColsInAB[i] = 0;
    }

    fibStats->rxcTotalNumABAssigned = 0;
    fibStats->numTotalIptAB = 0;
    fibStats->numTotalAptAB = 0;

    fibStats->numTotal80bIptAB = 0;
    fibStats->numTotal160bIptAB = 0;
    fibStats->numTotal80bSmallIptAB = 0;
    fibStats->numTotal160bSmallIptAB = 0;
    fibStats->numDupIptAB = 0;

    fibStats->numTotal80bAptAB = 0;
    fibStats->numTotal160bAptAB = 0;
    fibStats->numTotal80bSmallAptAB = 0;
    fibStats->numTotal160bSmallAptAB = 0;
    fibStats->numDupAptAB = 0;

    fibStats->num40bTriggers = 0;
    fibStats->num80bTriggers = 0;
    fibStats->num160bTriggers = 0;
    fibStats->avgTrigLen = 0;

    fibStats->totalNumHolesInLsns = 0;
    fibStats->numIPTEntries = 0;
    fibStats->numRPTEntries = 0;
    fibStats->numUsedBricks = 0;
    fibStats->numUsedButEmptyBricks = 0;
    fibStats->numPartiallyOccupiedBricks = 0;
    fibStats->numTrieNodes = 0;
    fibStats->numLmpsofarPfx = 0;
    fibStats->numAptLongPfx = 0;
    fibStats->numAPTLmpsofarEntries = 0;
    fibStats->numUsedUitBanks = 0;
    fibStats->totalRptBytesLoppedOff = 0;
    fibStats->numADWrites = 0;

    fibStats->maxNumRptSplitsInANode = 0;
    fibStats->depthofNodeWithMaxRptSplits = 0;
    fibStats->idOfNodeWithMaxRptSplits = 0;

    fibStats->numAllocatedIx = 0;
    fibStats->numFreeIx = 0;
    kaps_memset(&fibStats->kaps_small_bb_stats, 0, sizeof(fibStats->kaps_small_bb_stats));

    fibStats->maxLsnWidth = fibTbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn;
    fibStats->initialLsnWidth = fibTbl->m_trie->m_lsn_settings_p->m_initialMaxLpuPerLsn;

    kaps_trie_calc_trie_lsn_stats(fibTbl->m_trie->m_roots_trienode_p, fibStats);

    if (fibStats->numIPTEntries)
        fibStats->avgTrigLen = fibStats->avgTrigLen / fibStats->numIPTEntries;

    if (fibTbl->m_db->num_algo_levels_in_db == 3)
        kaps_trie_calc_kaps_small_bb_stats(fibTbl, &fibStats->kaps_small_bb_stats, 0);

    kaps_pool_mgr_get_pool_stats(fibTbl->m_trie->m_trie_global->poolMgr, fibStats);

    kaps_uda_mgr_calc_stats(trieGlobal_p->m_mlpmemmgr[0], &uda_stats);
    /* kaps_uda_mgr_print_detailed_stats(trieGlobal_p->m_mlpmemmgr[0]);*/
    fibStats->numFreeBricks = uda_stats.total_num_free_lpu_bricks;
    fibStats->numRegions = trieGlobal_p->m_mlpmemmgr[0]->no_of_regions;

    kaps_ix_mgr_calc_stats(fibTbl->m_trie->m_ixMgr, &ix_stats);
    fibStats->numAllocatedIx = ix_stats.total_num_allocated_ix;
    fibStats->numFreeIx = ix_stats.total_num_free_ix;

    ad_db = (struct kaps_ad_db *) fibTbl->m_db->common_info->ad_info.ad;
    while (ad_db)
    {
        fibStats->numADWrites += ad_db->num_writes;
        ad_db = ad_db->next;
    }

    return NLMERR_OK;
}

kaps_status
kaps_ftm_wb_process_rpt(
    kaps_fib_tbl * fib_tbl)
{
    NlmReasonCode reason = NLMRSC_REASON_OK;

    /*
     * Clear any pending RPT splits before saving the state. It is possible that an insert prefix prior to the
     * warmboot_save operation could result into an RPT split which would be postponed to the next operation.
     */
    fib_tbl->m_fibTblMgr_p->m_curFibTbl = fib_tbl;
    kaps_trie_process_rpt_split_or_move(fib_tbl->m_trie, &reason, 0);

    return KAPS_OK;
}

kaps_status
kaps_ftm_wb_save(
    kaps_fib_tbl * fib_tbl,
    uint32_t * nv_offset)
{
    if (KAPS_OK != kaps_trie_wb_save(fib_tbl->m_trie, nv_offset))
        return KAPS_NV_READ_WRITE_FAILED;

    return KAPS_OK;
}

kaps_status
kaps_ftm_wb_restore(
    kaps_fib_tbl * fib_tbl,
    uint32_t * nv_offset)
{
    return kaps_trie_wb_restore(fib_tbl->m_trie, nv_offset);
}

kaps_status
kaps_ftm_cr_restore(
    kaps_fib_tbl * fib_tbl,
    struct kaps_wb_cb_functions * cb_fun)
{
    return kaps_trie_cr_restore(fib_tbl->m_trie, cb_fun->nv_offset);
}

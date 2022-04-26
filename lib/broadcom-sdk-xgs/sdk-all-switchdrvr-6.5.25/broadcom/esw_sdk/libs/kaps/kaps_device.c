/*
**************************************************************************************
Copyright 2012-2019 Broadcom Corporation

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

#include <unistd.h>

#include "kaps_handle.h"
#include "kaps_ab.h"
#include "kaps_device_wb.h"
#include "kaps_device_internal.h"
#include "kaps_cr_pool_mgr.h"
#include "kaps_pfx_hash_table.h"
#include "kaps_key_internal.h"
#include "kaps_errors.h"
#include "kaps_device_alg.h"
#include "kaps_fib_hw.h"
#include "kaps_xpt.h"

static kaps_status
initialize_nlm_allocator(
    struct kaps_device *device)
{
    kaps_nlm_allocator *alloc_p;

    alloc_p = kaps_nlm_allocator_ctor(&device->nlm_alloc);
    alloc_p->m_vtbl.m_malloc = (kaps_nlm_allocator_alloc_t) device->alloc->xmalloc;
    alloc_p->m_vtbl.m_calloc = (kaps_nlm_allocator_calloc_t) device->alloc->xcalloc;
    alloc_p->m_vtbl.m_free = (kaps_nlm_allocator_free_t) device->alloc->xfree;
    alloc_p->m_clientData_p = device->alloc->cookie;

    return KAPS_OK;
}


kaps_status
kaps_device_init_internal(
    struct kaps_allocator * alloc,
    enum kaps_device_type type,
    uint32_t flags,
    void *xpt,
    struct kaps_device_config * config,
    struct kaps_device * device,
    int32_t core_id)
{
    kaps_status status;

    device->num_cores = 1;
    device->core_id = core_id;
    device->type = type;

    KAPS_STRY(kaps_resource_init_module(device));

    device->xpt = xpt;
    device->flags = flags;
    device->dba_err_threshold = 10;
    device->prop.early_block_clear = 1;
    device->prop.enable_adv_uda_write = 0;
    device->prop.lpt_mode = 1;
    device->bc_needed = 0;
    device->magic_db_entry_invalidate = 0;
    device->handle_interface_errors = 1;

    /*
     * Legacy use
     */
    initialize_nlm_allocator(device);
    
    device->max_num_pfx_log_records = 0;
    
    if (device->max_num_pfx_log_records && !device->pfx_log_records)
    {
        
        device->pfx_log_records = alloc->xcalloc(alloc->cookie, 
                                        device->max_num_pfx_log_records,
                                        sizeof(kaps_prefix_log_record));
                                        
        if (!device->pfx_log_records)
        {
            alloc->xfree(alloc->cookie, device);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    switch (type)
    {

        case KAPS_DEVICE_KAPS:
            {
                KAPS_STRY(kaps_dm_kaps_init(device));
            }

            device->num_ab[0] = 4;
            device->num_udc = KAPS_MAX_UDCS;
            device->num_uda_per_sb = KAPS_UDM_PER_UDC;
            device->num_dba_sb = 4;
            device->max_num_searches = 4;
            device->max_num_clone_parallel_lookups = 2;
            device->max_search_key_width_1 = 640;
            device->max_num_kpu_key_segments = 0;
            device->num_small_hb_blocks = 0;
            device->num_rows_in_small_hb_block = 0;
            device->tcam_format = KAPS_TCAM_FORMAT_1;
            device->combine_tcam_xy_write = 0;
            device->xpt_version = KAPS_XPT_VERSION_1;

            if (device->id == KAPS_QUMRAN_DEVICE_ID)
            {
                device->num_ab[0] = 2;
                device->num_dba_sb = 2;
                device->max_num_searches = 2;

                device->num_hb_blocks = 16;
                device->num_rows_in_hb_block = 1024;
            }

            if (device->id == KAPS_QUX_DEVICE_ID)
            {
                device->num_ab[0] = 2;
                device->num_dba_sb = 2;
                device->max_num_searches = 2;

                device->num_hb_blocks = 16;
                device->num_rows_in_hb_block = 256;
            }

            if (device->id == KAPS_JERICHO_PLUS_DEVICE_ID)
            {
                device->num_hb_blocks = 32;

                device->num_rows_in_hb_block = 4096;

                if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
                {
                    device->num_rows_in_hb_block = 2048;
                }
                else if (device->silicon_sub_type == KAPS_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
                {
                    device->num_rows_in_hb_block = 2048 + 256;
                }
            }

            if (device->id == KAPS_JERICHO_2_DEVICE_ID)
            {
                device->num_ab[0] = 8;
                device->num_dba_sb = 8;
                device->max_num_searches = 8;
 
                device->max_num_clone_parallel_lookups = 4;
                device->max_search_key_width_1 = 640 * 2;

                device->num_small_hb_blocks = 16;
                device->num_rows_in_small_hb_block = 512;
                
                device->num_hb_blocks = 28;
                device->num_rows_in_hb_block = 16 * 1024;
                device->tcam_format = KAPS_TCAM_FORMAT_2;
                device->combine_tcam_xy_write = 1;

            }

            device->num_ab_per_sb = 1;

            device->max_ad_width_1 = 20;
            device->max_interface_width_1 = 0;
            device->dba_offset = 1;
            break;

        default:
            alloc->xfree(alloc->cookie, device);
            return KAPS_INVALID_DEVICE_TYPE;
    }

    status = kaps_device_advanced_init(device);
    if (status != KAPS_OK)
    {
        alloc->xfree(alloc->cookie, device);
        return status;
    }

    status = kaps_resource_init_mem_map(device);
    if (status != KAPS_OK)
    {
        alloc->xfree(alloc->cookie, device);
        return status;
    }

    return KAPS_OK;
}

kaps_status
kaps_device_init(
    struct kaps_allocator * alloc,
    enum kaps_device_type type,
    uint32_t flags,
    void *xpt,
    struct kaps_device_config * config,
    struct kaps_device ** device_pp)
{
    struct kaps_device *device;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %u %u %p %p %p\n", alloc, type, flags, xpt, config, device_pp);

    if (!alloc || !device_pp)
        status = KAPS_INVALID_ARGUMENT;
    else if (type > KAPS_DEVICE_KAPS)
        status = KAPS_INVALID_DEVICE_TYPE;

    if (status == KAPS_OK)
    {
        device = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_device));
        if (!device)
            status = KAPS_OUT_OF_MEMORY;
    }

    if (status == KAPS_OK)
    {
        flags |= KAPS_DEVICE_ISSU;
        device->alloc = alloc;
        status = kaps_device_init_internal(alloc, type, flags, xpt, config, device, 0);
    }

    if (status == KAPS_OK)
    {
        *device_pp = device;

    }

    KAPS_TRACE_OUT("%p\n", *device_pp);
    return status;
}





kaps_status
kaps_device_construct_mask(
    struct kaps_db *in_db,
    uint8_t * g_mask)
{
    struct kaps_key_field *tmp = in_db->key->first_field;

    while (tmp)
    {
        if (tmp->type == KAPS_KEY_FIELD_HOLE && !tmp->do_not_bmr)
        {
            uint32_t offset = tmp->offset_1 / 8;
            uint32_t width = tmp->width_1 / 8;

            kaps_memset(&g_mask[offset], 0xff, width);
        }
        tmp = tmp->next;
    }
    return KAPS_OK;
}

int32_t
kaps_check_if_inst_needed(
    struct kaps_device * device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_DMA || db->type == KAPS_DB_TAP)
            return 0;
    }

    return 1;
}

kaps_status
kaps_device_check_alg_db_single_device(
    struct kaps_device * device)
{
    struct kaps_c_list_iter db_it;
    struct kaps_list_node *db_el;
    uint32_t is_alg_db_present;
    struct kaps_device *cascade;

    is_alg_db_present = 0;
    kaps_c_list_iter_init(&device->db_list, &db_it);
    while ((db_el = kaps_c_list_iter_next(&db_it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(db_el);

        if (db->type != KAPS_DB_ACL && db->type != KAPS_DB_LPM && db->type != KAPS_DB_EM)
            continue;

        if (kaps_db_get_algorithmic(db))
            is_alg_db_present = 1;
    }

    if (is_alg_db_present)
    {
        cascade = device;
        while (cascade)
        {
            cascade->is_alg_db_present = 1;
            cascade = cascade->next_dev;
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_check_alg_db(
    struct kaps_device * device)
{
    while (device)
    {
        KAPS_STRY(kaps_device_check_alg_db_single_device(device));
        device = device->next_bc_device;
    }
    return KAPS_OK;
}

kaps_status
kaps_device_db_assign_seq_num(
    struct kaps_device * device)
{
    struct kaps_device *bc_device;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_seq_num_to_ptr *seq_num_to_ptr;
    uint32_t db_seq_num = 1, ad_seq_num = 1, hb_seq_num = 1;

    if (device->main_bc_device)
        device = device->main_bc_device;

    if (device->is_config_locked)
        return KAPS_OK;

    if (device->seq_num_to_ptr && device->seq_num_to_ptr->is_memory_allocated)
        return KAPS_OK;

    bc_device = device;

    seq_num_to_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_seq_num_to_ptr));

    if (seq_num_to_ptr == NULL)
        return KAPS_OUT_OF_MEMORY;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        kaps_c_list_iter_init(&bc_device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
            struct kaps_db *tab = db;

            while (tab)
            {
                switch (tab->type)
                {
                    case KAPS_DB_ACL:
                    case KAPS_DB_LPM:
                    case KAPS_DB_EM:
                    case KAPS_DB_TAP:
                        tab->seq_num = db_seq_num++;
                        break;
                    case KAPS_DB_AD:
                        tab->seq_num = ad_seq_num++;
                        break;
                    case KAPS_DB_COUNTER:
                    case KAPS_DB_DMA:
                        break;
                    case KAPS_DB_HB:
                        tab->seq_num = hb_seq_num++;
                        break;
                    default:
                        kaps_sassert(0);
                        break;
                }
                tab = tab->next_tab;
            }
        }
    }

    if (db_seq_num > 255)
        return KAPS_EXCEEDED_NUM_DB;

    if (ad_seq_num > 255)
        return KAPS_EXCEEDED_NUM_AD_DB;

    if (hb_seq_num > 255)
        return KAPS_EXCEEDED_NUM_HB_DB;

    if (db_seq_num)
    {
        seq_num_to_ptr->db_ptr = device->alloc->xcalloc(device->alloc->cookie, db_seq_num, sizeof(struct kaps_db *));
        if (seq_num_to_ptr->db_ptr == NULL)
        {
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    if (ad_seq_num)
    {
        seq_num_to_ptr->ad_db_ptr = device->alloc->xcalloc(device->alloc->cookie, ad_seq_num, sizeof(struct kaps_db *));
        if (seq_num_to_ptr->ad_db_ptr == NULL)
        {
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr->db_ptr);
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    if (hb_seq_num)
    {
        seq_num_to_ptr->hb_db_ptr = device->alloc->xcalloc(device->alloc->cookie, hb_seq_num, sizeof(struct kaps_db *));
        if (seq_num_to_ptr->hb_db_ptr == NULL)
        {
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr->ad_db_ptr);
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr->db_ptr);
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr);
            return KAPS_OUT_OF_MEMORY;
        }
    }

    if (device->main_bc_device)
        device = device->main_bc_device;

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        kaps_c_list_iter_init(&bc_device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
            struct kaps_db *tab = db;

            while (tab)
            {
                switch (tab->type)
                {
                    case KAPS_DB_ACL:
                    case KAPS_DB_LPM:
                    case KAPS_DB_EM:
                    case KAPS_DB_TAP:
                        seq_num_to_ptr->db_ptr[tab->seq_num] = tab;
                        break;
                    case KAPS_DB_AD:
                        seq_num_to_ptr->ad_db_ptr[tab->seq_num] = (struct kaps_ad_db *) tab;
                        break;
                    case KAPS_DB_COUNTER:
                    case KAPS_DB_DMA:
                        break;
                    case KAPS_DB_HB:
                        seq_num_to_ptr->hb_db_ptr[tab->seq_num] = (struct kaps_hb_db *) tab;
                        break;
                    default:
                        kaps_sassert(0);
                        break;
                }
                tab = tab->next_tab;
            }
        }
    }

    seq_num_to_ptr->num_db = db_seq_num - 1;
    seq_num_to_ptr->num_ad_db = ad_seq_num - 1;
    seq_num_to_ptr->num_hb_db = hb_seq_num - 1;
    seq_num_to_ptr->is_memory_allocated = 1;
    if (device->main_bc_device)
        device = device->main_bc_device;

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_device *dev;

        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        for (dev = bc_device; dev; dev = dev->next_dev)
        {
            dev->seq_num_to_ptr = seq_num_to_ptr;
            if (dev->smt)
            {
                dev->smt->seq_num_to_ptr = seq_num_to_ptr;
            }
        }
    }
    return KAPS_OK;
}

static kaps_status
find_if_db_is_public(
    struct kaps_db *db)
{
    uint32_t em_len_at_beginning_1 = 0;
    struct kaps_key_field *f;
    uint8_t is_prefix_reached = 0;

    if (db->type != KAPS_DB_LPM)
        return KAPS_OK;

    for (f = db->key->first_field; f; f = f->next)
    {
        if (f->type == KAPS_KEY_FIELD_PREFIX)
            is_prefix_reached = 1;

        if (f->type == KAPS_KEY_FIELD_TABLE_ID || f->type == KAPS_KEY_FIELD_EM)
        {
            if (!is_prefix_reached)
            {
                em_len_at_beginning_1 += f->width_1;
            }
        }
    }

    /*
     * For a private database, the VRF will most likely be at least 10 bits. So if the exact match portion at the
     * beginning is 8 bits or less then we are assuming that the database is a public database
     */
    if (em_len_at_beginning_1 <= 8)
    {
        db->is_public = 1;
    }

    return KAPS_OK;
}

kaps_status
kaps_device_cr_update_nv(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * cb_fun)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint32_t db_iter;
    uint32_t nv_offset = *(cb_fun->nv_offset);
    uint8_t *tmp_ptr;

    db_iter = 0;

    cb_fun->nv_offset = &nv_offset;
    tmp_ptr = device->nv_ptr;

    /*
     * Pre calculate entry size & sequence number 
     */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            db->common_info->entry_size = db->fn_table->cr_calc_entry_mem(db);
            db->common_info->nv_db_iter = db_iter;
            db_iter++;
        }
    }

    device->nv_mem_mgr->offset_device_info_start = nv_offset;
    /*
     * Save the DB and Instructions Layout 
     */
    KAPS_STRY(kaps_wb_save_device_info(device, cb_fun));

    device->nv_mem_mgr->offset_device_lpm_info = nv_offset;

    if (device->lpm_mgr)
        KAPS_STRY(kaps_lpm_cr_save_state(device, cb_fun));

    device->nv_mem_mgr->offset_device_hb_info = nv_offset;
    if (device->aging_table && device->type == KAPS_DEVICE_KAPS)
        KAPS_STRY(kaps_kaps_hb_cr_init(device, cb_fun));

    /*
     * Save the NV memory for trie info 
     */
    db_iter = 0;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            device->nv_mem_mgr->offset_db_trie_has_been_built[db_iter] = nv_offset;
            nv_offset += (sizeof(uint32_t) * 2);
            if (nv_offset > device->nv_size)
                return KAPS_EXHAUSTED_NV_MEMORY;
            if (cb_fun->nv_ptr)
            {
                cb_fun->nv_ptr += (sizeof(uint32_t) * 2);
            }
            db_iter++;
        }
    }

    /*
     * Save nv memory for trie SB bitmap 
     */
    db_iter = 0;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            device->nv_mem_mgr->offset_db_trie_sb_bitmap[db_iter] = nv_offset;
            nv_offset += (sizeof(uint32_t) * 2);
            if (nv_offset > device->nv_size)
                return KAPS_EXHAUSTED_NV_MEMORY;
            if (cb_fun->nv_ptr)
            {
                cb_fun->nv_ptr += (sizeof(uint32_t) * 2);
            }
            db_iter++;
        }
    }

    /*
     * Save the common info for ACL DBs 
     */
    db_iter = 0;
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
        {
            device->nv_mem_mgr->offset_db_info_start[db_iter] = nv_offset;
            db->fn_table->save_cr_state(db, cb_fun);
            device->nv_mem_mgr->offset_db_info_end[db_iter] = nv_offset;
            db_iter++;
        }
    }

    /*
     * save pending list offset across device Keep this always at end of nv_memory, as we will grow dynamically 
     */
    device->nv_mem_mgr->offset_device_pending_list = nv_offset;
    nv_offset += sizeof(uint32_t);
    if (nv_offset > device->nv_size)
        return KAPS_EXHAUSTED_NV_MEMORY;
    if (cb_fun->nv_ptr)
        cb_fun->nv_ptr += sizeof(uint32_t);

    if (tmp_ptr)
    {
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->num_db;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_lpm_shadow_info;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_info_start;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_pending_list;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_lpm_info;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_hb_info;
        tmp_ptr += sizeof(uint32_t);

        for (db_iter = 0; db_iter < device->nv_mem_mgr->num_db; db_iter++)
        {
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_trie_has_been_built[db_iter];
            tmp_ptr += sizeof(uint32_t);
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_trie_sb_bitmap[db_iter];
            tmp_ptr += sizeof(uint32_t);
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_info_start[db_iter];
            tmp_ptr += sizeof(uint32_t);
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_info_end[db_iter];
            tmp_ptr += sizeof(uint32_t);
        }
    }
    return KAPS_OK;
}

static kaps_status
kaps_device_create_cr_pool_mgr(
    struct kaps_device * device,
    struct kaps_wb_cb_functions * cb_fun)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    uint8_t lpm_present = 0;
    uint8_t *end_nv_ptr = device->nv_ptr;

    cb_fun->read_fn = device->nv_read_fn;
    cb_fun->write_fn = device->nv_write_fn;
    cb_fun->handle = device->nv_handle;
    cb_fun->nv_ptr = device->nv_ptr;

    end_nv_ptr = end_nv_ptr + device->nv_size - 1;
    /*
     * Initialize NV memory Manager 
     */
    KAPS_STRY(kaps_device_nv_memory_manager_init(device));
    KAPS_STRY(kaps_cr_pool_mgr_init(device, end_nv_ptr, &device->nv_mem_mgr->kaps_cr_pool_mgr));

    /*
     * Leave Space for NV memory manager 
     */
    *(cb_fun->nv_offset) += sizeof(uint32_t);   /* num_db */
    *(cb_fun->nv_offset) += sizeof(uint32_t);   /* LPM Shadow Info */
    *(cb_fun->nv_offset) += sizeof(uint32_t);   /* Device info */
    *(cb_fun->nv_offset) += sizeof(uint32_t);   /* Pending list offset */
    *(cb_fun->nv_offset) += sizeof(uint32_t);   /* LPM Info */
    *(cb_fun->nv_offset) += sizeof(uint32_t);   /* HB Info */

    *(cb_fun->nv_offset) += (device->nv_mem_mgr->num_db * 6 * sizeof(uint32_t));        /* DB/Table/entry info */

    if (cb_fun->nv_ptr)
    {
        cb_fun->nv_ptr += sizeof(uint32_t);     /* num_db */
        cb_fun->nv_ptr += sizeof(uint32_t);     /* LPM shadow info */
        cb_fun->nv_ptr += sizeof(uint32_t);     /* Device Info */
        cb_fun->nv_ptr += sizeof(uint32_t);     /* Pending list offset */
        cb_fun->nv_ptr += sizeof(uint32_t);     /* LPM INFO */
        cb_fun->nv_ptr += sizeof(uint32_t);     /* HB INFO */
        cb_fun->nv_ptr += (device->nv_mem_mgr->num_db * 6 * sizeof(uint32_t));
    }

    /*
     * Check If LPM is present 
     */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_LPM)
            lpm_present = 1;
    }

    device->nv_mem_mgr->offset_device_lpm_shadow_info = *(cb_fun->nv_offset);

    if (lpm_present)
    {
        kaps_lpm_cr_get_adv_ptr(device, cb_fun);
    }

    if (*(cb_fun->nv_offset) > device->nv_size)
        return KAPS_EXHAUSTED_NV_MEMORY;

    return KAPS_OK;
}

static kaps_status
kaps_device_update_db_handle_free_lists(
    struct kaps_device * device)
{
    uint32_t nv_offset = 0;
    struct kaps_wb_cb_functions cb_fun;
    struct kaps_device *cascade;
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    for (cascade = device; cascade; cascade = cascade->next_dev)
    {
        cascade->issu_status = KAPS_ISSU_INIT;
        cascade->is_config_locked = 1;
        if (cascade->smt)
        {
            cascade->smt->issu_status = KAPS_ISSU_INIT;
            cascade->smt->is_config_locked = 1;
        }
    }

    if (device->nv_ptr)
    {
        uint32_t offset = 0;
        uint8_t *pending_list = NULL;

        /*
         * update the db handle free lists 
         */
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
                KAPS_STRY(db->fn_table->cr_reconcile(db));
        }
        offset = device->nv_mem_mgr->offset_device_pending_list;
        pending_list = (uint8_t *) device->nv_ptr + offset;
        *(uint32_t *) pending_list = 0;
    }

    /*
     * update the db handle free lists 
     */
    kaps_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_DB_DMA || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP)
            continue;

        if (db->is_bc_required && !db->is_main_bc_db)
            continue;

        if (KAPS_WB_HANDLE_TABLE_IS_FREELIST_INIT_DONE(db) == 0)
            KAPS_WB_HANDLE_TABLE_FREELIST_INIT(db);
    }

    if (device->nv_ptr)
    {
        nv_offset = 0;

        cb_fun.read_fn = device->nv_read_fn;
        cb_fun.write_fn = device->nv_write_fn;
        cb_fun.handle = device->nv_handle;
        cb_fun.nv_offset = &nv_offset;
        cb_fun.nv_ptr = device->nv_ptr;

        nv_offset = device->nv_mem_mgr->offset_device_info_start;
        cb_fun.nv_ptr += nv_offset;
        KAPS_STRY(kaps_wb_save_device_info(device, &cb_fun));
    }
    return KAPS_OK;
}

void
kaps_device_find_non_algorithmic_databases(
    struct kaps_device *device)
{
    struct kaps_device *bc_device;
    uint32_t mp_db_present = 1;
    uint32_t inplace_ad_present = 1;

    if (device->main_bc_device)
        device = device->main_bc_device;

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_c_list_iter it;
        struct kaps_list_node *el;

        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        kaps_c_list_iter_init(&bc_device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_ACL || db->type == KAPS_DB_LPM)
            {
                struct kaps_ad_db *ad_db = NULL;

                if (!kaps_db_get_algorithmic(db))
                    mp_db_present = 1;

                ad_db = (struct kaps_ad_db *) db->common_info->ad_info.ad;

                while (ad_db)
                {
                    if (kaps_resource_get_ad_type(&ad_db->db_info) == KAPS_AD_TYPE_INPLACE)
                        inplace_ad_present = 1;
                    ad_db = ad_db->next;
                }
            }
        }
    }

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_device *dev;

        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        for (dev = bc_device; dev; dev = dev->next_dev)
        {
            dev->mp_db_present = mp_db_present;
            dev->inplace_ad_present = inplace_ad_present;

            if (dev->smt)
            {
                dev->smt->mp_db_present = mp_db_present;
                dev->smt->inplace_ad_present = inplace_ad_present;
            }
        }
    }
}

kaps_status
kaps_device_lock(
    struct kaps_device *bc_device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_wb_cb_functions cb_fun;
    uint32_t nv_offset = 0;
    struct kaps_device *device;

    KAPS_TRACE_IN("%p\n", bc_device);

    if (!bc_device)
        return KAPS_INVALID_DEVICE_PTR;

    for (device = bc_device; device; device = device->next_bc_device)
    {
        if (device->issu_in_progress)
            KAPS_STRY(kaps_wb_refresh_db_handle_in_map(device));
    }

    for (device = bc_device; device; device = device->next_bc_device)
    {
        device->execute_the_next_loop = 0;

        if (device->is_generic_bc_device)
        {
            continue;
        }

        if (device->main_dev)
            device = device->main_dev;

        if (device->is_config_locked)
            return KAPS_DEVICE_ALREADY_LOCKED;

        if (device->flags & KAPS_DEVICE_SMT)
        {
            if (!device->smt)
            {
                return KAPS_SMT_THREAD_NOT_CREATED;
            }
        }

        device->dynamic_alloc_fp = bc_device->dynamic_alloc_fp;

        /*
         * Find and if there is a public database and mark as public if found
         */
        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            find_if_db_is_public(db);
        }
        /*
         * First check if the databases and instructions
         * look clean and as expected before we proceed.
         * Initially all databases and instructions are
         * on the same device
         */
        {
            uint32_t db_count = 0;

            if (kaps_check_if_inst_needed(device) && kaps_c_list_count(&device->inst_list) == 0)
                return KAPS_DEVICE_NO_INSTRUCTIONS;

            kaps_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);
                /*
                 * Ensure basic key checking 
                 */
                if (db->type != KAPS_DB_AD && db->type != KAPS_DB_COUNTER && db->type != KAPS_DB_TAP
                    && db->type != KAPS_DB_DMA && db->type != KAPS_DB_HB)
                {
                    struct kaps_db *next_tab = db;

                    for (; next_tab; next_tab = next_tab->next_tab)
                    {
                        if (next_tab->key == NULL)
                            return KAPS_MISSING_KEY;
                    }
                }
                db_count++;
            }

            if (db_count == 0)
            {
                return KAPS_DEVICE_NO_DB;
            }
        }

        if (device->issu_in_progress)
        {
            if (device->issu_status != KAPS_ISSU_RESTORE_START)
            {
                device->issu_in_progress = 0;
                if (device->smt)
                    device->smt->issu_in_progress = 0;
            }
        }

        /*
         * Assign the seq num to every database 
         */
        if (device == bc_device)
        {
            KAPS_STRY(kaps_device_db_assign_seq_num(bc_device));
            kaps_device_find_non_algorithmic_databases(bc_device);
        }

        /*
         * Resetting the flag with KAPS_ISSU_INIT and making the config_lock to one with the user call on this API 
         */
        if (device->flags & KAPS_DEVICE_ISSU)
        {
            if (device->issu_status == KAPS_ISSU_RESTORE_END || device->issu_status == KAPS_ISSU_RECONCILE_END)
            {
                KAPS_STRY(kaps_device_update_db_handle_free_lists(device));
                continue;
            }
        }

        if (device->bc_id == 0)
        {
            KAPS_STRY(kaps_device_check_alg_db(device));
        }

        if (device->bc_id == 0)
        {
            KAPS_STRY(kaps_device_check_lpm_constraints(device));
        }

        device->execute_the_next_loop = 1;
    }

    for (device = bc_device; device; device = device->next_bc_device)
    {
        if (!device->execute_the_next_loop)
            continue;
        if (device->main_dev)
            device = device->main_dev;

        KAPS_STRY(kaps_resource_finalize(device));
    }

    for (device = bc_device; device; device = device->next_bc_device)
    {

        if (!device->execute_the_next_loop)
            continue;

        if (device->main_dev)
            device = device->main_dev;

        if (device->nv_ptr && device->issu_status == KAPS_ISSU_INIT)
        {
            cb_fun.nv_offset = &nv_offset;
            KAPS_STRY(kaps_device_create_cr_pool_mgr(device, &cb_fun));
        }

        kaps_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_c_list_iter_next(&it)) != NULL)
        {
            uint32_t org_bmp;
            struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP
                || db->type == KAPS_DB_DMA || db->type == KAPS_DB_HB)
                continue;
            org_bmp = db->device->db_bc_bitmap;
            db->device->db_bc_bitmap = db->bc_bitmap;
            KAPS_STRY(db->fn_table->lock_db(db));
            db->device->db_bc_bitmap = org_bmp;
        }

        if (device->type == KAPS_DEVICE_KAPS)
        {
            device->is_config_locked = 1;
        }

        if (device->nv_ptr && device->issu_status == KAPS_ISSU_INIT)
        {
            KAPS_STRY(kaps_device_cr_update_nv(device, &cb_fun));
        }
    }

    return KAPS_OK;
}

kaps_status
kaps_device_destroy(
    struct kaps_device * cascade_device)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;
    struct kaps_device *device = cascade_device;

    KAPS_TRACE_IN("%p\n", cascade_device);

    if (!device)
        return KAPS_INVALID_DEVICE_PTR;

    
    if (device->pfx_log_records)
    {
        device->alloc->xfree(device->alloc->cookie, device->pfx_log_records);
    }

    /*
     * Reset the flag so that the destroy goes through
     * to recover the heap memory, to clean up the device.
     */
    device->fatal_transport_error = 0;

    if (device->is_wb_continue)
    {
        struct kaps_device *bc_device = device;

        device->issu_status = KAPS_ISSU_INIT;

        for (; bc_device; bc_device = bc_device->next_bc_device)
        {
            struct kaps_device *device = bc_device;
            struct kaps_device *tmp_device = device;

            tmp_device = device;
            while (tmp_device)
            {
                tmp_device->issu_in_progress = NlmTrue;
                if (tmp_device->smt)
                    tmp_device->smt->issu_in_progress = NlmTrue;
                tmp_device = tmp_device->next_dev;
            }
        }
    }

    while (cascade_device)
    {
        struct kaps_device *next_cascade_dev = cascade_device->next_dev;
        struct kaps_device *bc_device;

        for (bc_device = cascade_device; bc_device; bc_device = bc_device->next_bc_device)
        {
            struct kaps_db *db_list[64];
            int32_t db_count = 0, db_index;

            device = bc_device;
            if (device->is_generic_bc_device)
            {
                continue;
            }
            /*
             * Destroy any databases still alive
             */

            kaps_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

                if (db->type == KAPS_DB_AD || db->type == KAPS_DB_COUNTER || db->type == KAPS_DB_TAP
                    || db->type == KAPS_DB_HB || db->type == KAPS_DB_DMA)
                    continue;   /* cannot destroy AD until all remaining DBs are destroyed */

                db_list[db_count++] = db;
            }
            for (db_index = db_count - 1; db_index >= 0; db_index--)
            {
                struct kaps_db *db = db_list[db_index];
                struct kaps_db *ad_db, *ad_db_next;

                /*
                 * If is_destroyed flag is set, user has already destroyed this DB, no need to call destroy again 
                 */
                if (!db->is_destroyed)
                {
                    uint32_t org_bmp;

                    ad_db = db->common_info->ad_info.ad;

                    org_bmp = device->db_bc_bitmap;
                    device->db_bc_bitmap = db->bc_bitmap;
                    db->fn_table->db_destroy(db);
                    device->db_bc_bitmap = org_bmp;

                    while (ad_db)
                    {
                        ad_db_next = ad_db->next_tab;
                        ad_db->common_info->ad_info.db = NULL;
                        kaps_c_list_remove_node(&device->db_list, &ad_db->node, NULL);
                        KAPS_STRY(kaps_ad_db_destroy((struct kaps_ad_db *) ad_db));

                        ad_db = ad_db_next;
                    }
                }
            }

            /*
             * Destroy AD databases
             */

            kaps_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

                if (db->type != KAPS_DB_AD)
                    continue;
                /*
                 * We have already destroyed the non AD-databases. So set the db pointer of the ad
                 * database to NULL
                 */
                db->common_info->ad_info.db = NULL;
                kaps_c_list_remove_node(&device->db_list, el, &it);
                KAPS_STRY(kaps_ad_db_destroy((struct kaps_ad_db *) db));
            }

            /*
             * Destroy HB databases
             */

            kaps_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_db *db = KAPS_SSDBLIST_TO_ENTRY(el);

                if (db->type != KAPS_DB_HB)
                    continue;

                db->common_info->hb_info.db = NULL;
                kaps_c_list_remove_node(&device->db_list, el, &it);
                KAPS_STRY(kaps_hb_db_destroy((struct kaps_hb_db *) db));
            }

            /*
             * Destroy Hitbit buffer and aging table
             */

            if (device->hb_buffer && !device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
            device->hb_buffer = 0;

            if (device->hb_vbuffer && !device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, device->hb_vbuffer);
            device->hb_vbuffer = 0;

            if (device->aging_valid_memory)
            {
                device->alloc->xfree(device->alloc->cookie, device->aging_valid_memory);
                device->aging_valid_memory = NULL;
            }

            if (device->aging_table && !device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, device->aging_table);
            
            device->aging_table = NULL;

            if (device->small_aging_table)
                device->alloc->xfree(device->alloc->cookie, device->small_aging_table);

            device->small_aging_table = NULL;
        }

        for (bc_device = cascade_device; bc_device; bc_device = bc_device->next_bc_device)
        {
            device = bc_device;
            if (device->is_generic_bc_device)
            {
                continue;
            }
            /*
             * Destroy any instructions still alive
             */

            kaps_c_list_iter_init(&device->inst_list, &it);
            while ((el = kaps_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_instruction *ins = KAPS_INSTLIST_TO_ENTRY(el);

                kaps_c_list_remove_node(&device->inst_list, el, &it);
                KAPS_STRY(kaps_instruction_destroy(ins));
            }
        }

        /*
         * due to cascading, LPM destroy should be called in all devices before kaps_resource_fini_module can be called 
         */

        for (bc_device = cascade_device; bc_device; bc_device = bc_device->next_bc_device)
        {
            device = bc_device;
            kaps_lpm_destroy(device);
        }

        bc_device = cascade_device;
        while (bc_device)
        {
            struct kaps_device *device = bc_device;

            bc_device = bc_device->next_bc_device;

            kaps_resource_fini_module(device);
            kaps_device_advanced_destroy(device);

            if (!next_cascade_dev && (device->bc_id == 0) && device->seq_num_to_ptr)
            {
                if (device->seq_num_to_ptr->db_ptr)
                    device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr->db_ptr);
                if (device->seq_num_to_ptr->ad_db_ptr)
                    device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr->ad_db_ptr);
                if (device->seq_num_to_ptr->hb_db_ptr)
                    device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr->hb_db_ptr);

                device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr);
            }

            if (device->smt)
                device->alloc->xfree(device->alloc->cookie, device->smt);
            if (device->description)
                device->alloc->xfree(device->alloc->cookie, device->description);

            device->alloc->xfree(device->alloc->cookie, device);
        }

        cascade_device = next_cascade_dev;
    }

    return KAPS_OK;
}

char *
kaps_device_db_name(
    struct kaps_db *db)
{
    if (db->common_info->dummy_db)
        return "DUMMY";

    switch (db->type)
    {
        case KAPS_DB_INVALID:
            return "ERROR";
        case KAPS_DB_ACL:
            return "ACL";
        case KAPS_DB_LPM:
            return "LPM";
        case KAPS_DB_EM:
            return "EM";
        case KAPS_DB_AD:
            return "AD";
        case KAPS_DB_COUNTER:
            return "COUNTER";
        case KAPS_DB_TAP:
            return "TAP";
        case KAPS_DB_DMA:
            return "DMA";
        default:
            kaps_sassert(0);
    }
    return NULL;
}

kaps_status
kaps_device_set_property(
    struct kaps_device * device,
    enum kaps_device_properties property,
    ...)
{
    va_list a_list;
    char *desc;
    int32_t val;
    kaps_status status = KAPS_OK;
    struct kaps_device *dev;
    int32_t is_generic_bc = 0;
    uint8_t *tmp_ptr;
    uint32_t mem_consumed = 0;

    KAPS_TRACE_IN("%p %u ", device, property);

    if (device == NULL)
    {
        KAPS_TRACE_PRINT("%s\n", " ");
        return KAPS_INVALID_DEVICE_PTR;
    }

    if (device->main_dev)
        dev = device->main_dev;
    else
        dev = device;

    va_start(a_list, property);

    switch (property)
    {
        case KAPS_DEVICE_PROP_HANDLE_INTERFACE_ERRORS:
            val = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->handle_interface_errors = val;
                if (dev->smt)
                    device->smt->handle_interface_errors = val;
                if (dev->other_core)
                    device->other_core->handle_interface_errors = val;
            }
            break;
        case KAPS_DEVICE_PROP_FORCE_INVALIDATE_LOC:
            val = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->magic_db_entry_invalidate = val;
                if (dev->smt)
                    device->smt->magic_db_entry_invalidate = val;
                if (dev->other_core)
                    device->other_core->magic_db_entry_invalidate = val;
            }
            break;
        case KAPS_DEVICE_PROP_MIN_RESOURCE:
            val = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->prop.min_rxc = val;
                if (dev->smt)
                    device->smt->prop.min_rxc = val;
                if (dev->other_core)
                    device->other_core->prop.min_rxc = val;
            }
            break;

        case KAPS_DEVICE_PROP_ADV_UDA_WRITE:
            val = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->prop.enable_adv_uda_write = val;
                if (dev->smt)
                    device->smt->prop.enable_adv_uda_write = val;
                if (dev->other_core)
                    device->other_core->prop.enable_adv_uda_write = val;
            }
            break;
        case KAPS_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS:
            val = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->prop.return_error_on_asserts = val;
                if (dev->smt)
                    device->smt->prop.return_error_on_asserts = val;
                if (dev->other_core)
                    device->other_core->prop.return_error_on_asserts = val;
            }
            break;
        case KAPS_DEVICE_PROP_READ_TYPE:
            val = va_arg(a_list, int32_t);
            KAPS_TRACE_PRINT("%d", val);

            /*
             * device lock check excluded as the debug API can read from s/w or h/w 
             */
            if (val != 0 && val != 1 && val != 2)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->prop.read_type = val;
                if (dev->smt)
                    dev->smt->prop.read_type = val;
                if (dev->other_core)
                    device->other_core->prop.read_type = val;
            }
            break;

        case KAPS_DEVICE_PROP_DEBUG_LEVEL:
            /*
             * device lock check as debug level is one time set/reset property 
             */
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }

            val = va_arg(a_list, int32_t);
            KAPS_TRACE_PRINT("%d", val);

            /*
             * default no debug prints, any positive value; according to the level will dump the data 
             */
            if (val < 0)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->debug_level = val;
                if (dev->smt)
                    dev->smt->debug_level = val;
                if (dev->other_core)
                    device->other_core->debug_level = val;
            }
            break;

        case KAPS_DEVICE_PROP_DESCRIPTION:
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }
            desc = va_arg(a_list, char *);
            KAPS_TRACE_PRINT("\"%s\"", desc);

            if (!desc)
            {
                status = KAPS_INVALID_DEVICE_DESC_PTR;
                break;
            }
            device->description = device->alloc->xcalloc(device->alloc->cookie, 1, (strlen(desc) + 1));
            if (!device->description)
            {
                status = KAPS_OUT_OF_MEMORY;
                break;
            }

            strcpy(device->description, desc);
            break;

        case KAPS_DEVICE_PROP_LTR_SM_UPDATE:
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->is_ltr_sm_update_needed = 1;
                if (dev->smt)
                    dev->smt->is_ltr_sm_update_needed = 1;
                if (dev->other_core)
                    dev->other_core->is_ltr_sm_update_needed = 1;
            }
            break;

        case KAPS_DEVICE_PROP_INTERRUPT:
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }
            val = va_arg(a_list, int32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val != 0 && val != 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->interrupt_enable = val;
                if (dev->smt)
                    dev->smt->interrupt_enable = val;
                if (dev->other_core)
                    device->other_core->interrupt_enable = val;
            }

            break;

        case KAPS_DEVICE_PROP_IFSR_THRESHOLD:
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }
            val = va_arg(a_list, int32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val < 0 || val > 63)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->dba_err_threshold = val;
                if (dev->smt)
                    dev->smt->dba_err_threshold = val;
                if (dev->other_core)
                    device->other_core->dba_err_threshold = val;
            }
            break;

        case KAPS_DEVICE_PRE_CLEAR_ABS:
            val = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->prop.early_block_clear = val;
                if (dev->smt)
                    dev->smt->prop.early_block_clear = val;

                if (dev->other_core)
                {
                    dev->other_core->prop.early_block_clear = val;
                    if (dev->other_core->smt)
                        dev->other_core->smt->prop.early_block_clear = val;
                }
            }
            break;

        case KAPS_DEVICE_ADD_BROADCAST:
            is_generic_bc = 1;
            /*
             * fallthrough, dont break 
             */
        case KAPS_DEVICE_ADD_SELECTIVE_BROADCAST:
        {
            struct kaps_device *main_bc_device = NULL;

            if (!device->main_bc_device)
            {
                device->main_bc_device = device;
            }

            val = va_arg(a_list, int32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val < 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }

            dev = va_arg(a_list, struct kaps_device *);
            KAPS_TRACE_PRINT("%p", dev);

            dev->main_bc_device = device->main_bc_device;
            dev->bc_id = val;
            dev->bc_needed = 0;

            {
                if (is_generic_bc)
                {
                    dev->is_generic_bc_device = 1;
                    device->generic_bc_bitmap |= (1 << dev->bc_id);
                }
                else
                {
                    device->is_selective_bc_enabled = 1;
                    dev->is_selective_bc_enabled = 1;
                }

                main_bc_device = device;
                main_bc_device->bc_needed = 0;

                while (main_bc_device->next_bc_device)
                {
                    main_bc_device->bc_needed = 0;
                    main_bc_device = main_bc_device->next_bc_device;
                }
                main_bc_device->next_bc_device = dev;
            }

            break;
        }

        case KAPS_DEVICE_PROP_CRASH_RECOVERY:
        {
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }

            kaps_resource_fini_module(device);

            device->is_fresh_nv = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", device->is_fresh_nv);
            device->nv_ptr = va_arg(a_list, void *);
            KAPS_TRACE_PRINT("%p", device->nv_ptr);

            if (!device->nv_ptr)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }

            tmp_ptr = (uint8_t *) device->nv_ptr;
            device->nv_size = va_arg(a_list, uint32_t);
            KAPS_TRACE_PRINT("%d", device->nv_size);

            if (!device->nv_size)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }

            if (device->is_fresh_nv)
                kaps_memset(device->nv_ptr, 0, device->nv_size);

            status = kaps_resource_init_mem_map(device);
            if (status != KAPS_OK)
            {
                break;
            }

            if (device->type == KAPS_DEVICE_KAPS)
            {
                status = kaps_initialize_advanced_shadow(device);
                if (status != KAPS_OK)
                {
                    break;
                }
            }

            dev = device;
            while (dev)
            {
                device->cr_status = (enum kaps_restore_status *) device->nv_ptr;
                device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + sizeof(enum kaps_restore_status));
                dev = dev->next_dev;
            }
            mem_consumed = (uint8_t *) device->nv_ptr - tmp_ptr;
            device->nv_size -= mem_consumed;

            dev = device;
            while (dev)
            {
                if (dev->smt)
                {
                    dev->smt->nv_ptr = device->nv_ptr;
                    dev->smt->nv_size = device->nv_size;
                }
                dev = dev->next_dev;
            }
            break;
        }

        case KAPS_DEVICE_PROP_DEFER_LTR_WRITES:
        {
            val = va_arg(a_list, int32_t);
            KAPS_TRACE_PRINT("%d", val);
            if (val < 0 || val > 1)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }

            for (; dev; dev = dev->next_dev)
            {
                dev->defer_ltr_writes = val;
                if (dev->smt)
                    dev->smt->defer_ltr_writes = val;
                if (dev->other_core)
                    device->other_core->defer_ltr_writes = val;
            }

            break;
        }


        case KAPS_DEVICE_PROP_DUMP_ON_ASSERT:
        {
            if (device->is_config_locked)
            {
                status = KAPS_DEVICE_ALREADY_LOCKED;
                break;
            }

            desc = va_arg(a_list, char *);
            KAPS_TRACE_PRINT("\"%s\"", desc);
            if (desc == NULL)
            {
                status = KAPS_INVALID_ARGUMENT;
                break;
            }

            device->dump_on_assert = 1;
            strcpy(device->fname_dump, desc);

            break;
        }


        case KAPS_DEVICE_PROP_LAST_PARITY_ERRORS:
        case KAPS_DEVICE_PROP_LAST_PARITY_ERRORS_STATUS:
        case KAPS_DEVICE_PROP_NON_REPAIRABLE_ABS:
        case KAPS_DEVICE_PROP_LAST_UDA_PARITY_ERRORS:
        {
            status = KAPS_READ_ONLY_PROPERTY;
            break;
        }


        default:
            if (device->is_config_locked)
            {
                if ((int)property != 750)
                {
                    status = KAPS_DEVICE_ALREADY_LOCKED;
                    break;
                }
            }
            status = kaps_device_advanced_set_property(device, property, a_list);
            break;
    }

    va_end(a_list);

    KAPS_TRACE_PRINT("%s\n", " ");
    return status;
}

kaps_status
kaps_device_get_property(
    struct kaps_device * device,
    enum kaps_device_properties property,
    ...)
{
    va_list a_list;
    char **desc;
    int32_t *val;
    kaps_status status = KAPS_OK;

    KAPS_TRACE_IN("%p %u\n", device, property);

    if (device == NULL)
        return KAPS_INVALID_DEVICE_PTR;

    va_start(a_list, property);
    switch (property)
    {

        case KAPS_DEVICE_PROP_HANDLE_INTERFACE_ERRORS:
            val = va_arg(a_list, int32_t *);
            *val = device->handle_interface_errors;
            break;

        case KAPS_DEVICE_PROP_FORCE_INVALIDATE_LOC:
            val = va_arg(a_list, int32_t *);
            *val = device->magic_db_entry_invalidate;
            break;

        case KAPS_DEVICE_PROP_MIN_RESOURCE:
            val = va_arg(a_list, int32_t *);
            *val = device->prop.min_rxc;
            break;

        case KAPS_DEVICE_PROP_ADV_UDA_WRITE:
            val = va_arg(a_list, int32_t *);
            *val = device->prop.enable_adv_uda_write;
            break;

        case KAPS_DEVICE_PROP_DEFER_LTR_WRITES:
            val = va_arg(a_list, int32_t *);
            *val = device->defer_ltr_writes;
            break;

        case KAPS_DEVICE_PRE_CLEAR_ABS:
            val = va_arg(a_list, int32_t *);
            *val = device->prop.early_block_clear;
            break;

        case KAPS_DEVICE_PROP_READ_TYPE:
            val = va_arg(a_list, int32_t *);
            *val = device->prop.read_type;
            break;

        case KAPS_DEVICE_PROP_DEBUG_LEVEL:
            val = va_arg(a_list, int32_t *);
            *val = device->debug_level;
            break;

        case KAPS_DEVICE_PROP_DESCRIPTION:
            desc = va_arg(a_list, char **);
            if (!desc)
            {
                status = KAPS_INVALID_DEVICE_DESC_PTR;
                break;
            }
            *desc = device->description;
            break;

        case KAPS_DEVICE_PROP_LTR_SM_UPDATE:
            val = va_arg(a_list, int32_t *);
            *val = device->is_ltr_sm_update_needed;
            break;

        case KAPS_DEVICE_PROP_INTERRUPT:
            val = va_arg(a_list, int32_t *);
            *val = device->interrupt_enable;
            break;

        case KAPS_DEVICE_PROP_INST_LATENCY:
            val = va_arg(a_list, int32_t *);
            *val = device->prop.instruction_latency;
            break;

        default:
            status = kaps_device_advanced_get_property(device, property, a_list);
            break;
    }

    va_end(a_list);
    return status;
}

kaps_status
kaps_device_print_html(
    struct kaps_device * device,
    FILE * fp)
{
    KAPS_TRACE_IN("%p %p\n", device, fp);
    if (!device || !fp)
        return KAPS_INVALID_ARGUMENT;

    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_ISSU_INIT)
        {
            return KAPS_ISSU_IN_PROGRESS;
        }
    }

    kaps_resource_print_html(device, fp);
    return KAPS_OK;
}

kaps_status
kaps_device_print_sw_state(
    struct kaps_device * device,
    FILE * fp)
{
    KAPS_TRACE_IN("%p %p\n", device, fp);
    if (!device || !fp)
        return KAPS_INVALID_ARGUMENT;

    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_ISSU_INIT)
        {
            return KAPS_ISSU_IN_PROGRESS;
        }
    }

    device->map_print = 1;
    kaps_resource_print_html(device, fp);
    device->map_print = 0;
    return KAPS_OK;
}

kaps_status
kaps_device_print(
    struct kaps_device * device,
    FILE * fp)
{
    struct kaps_c_list_iter it;
    struct kaps_list_node *el;

    KAPS_TRACE_IN("%p %p\n", device, fp);

    if (!device || !fp)
        return KAPS_INVALID_ARGUMENT;

    if (!device->is_config_locked)
        return KAPS_DEVICE_UNLOCKED;

    if (device->flags & KAPS_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_ISSU_INIT)
        {
            return KAPS_ISSU_IN_PROGRESS;
        }
    }

    if (device->description)
        kaps_fprintf(fp, "%s\n", device->description);

    kaps_c_list_iter_init(&device->inst_list, &it);
    while ((el = kaps_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_instruction *instruction = KAPS_INSTLIST_TO_ENTRY(el);

        kaps_instruction_print(instruction, fp);
    }

    return KAPS_OK;
}

const char *
kaps_device_get_sdk_version(
    void)
{
    KAPS_TRACE_PRINT("%s\n", " ");
    return KAPS_SDK_VERSION_FULL;
}

kaps_status
kaps_device_start_transaction(
    struct kaps_device * device)
{
    KAPS_TRACE_IN("%p\n", device);
    if (!device)
        return KAPS_INVALID_ARGUMENT;

    
    if (!device->nv_ptr)
        return KAPS_INVALID_ARGUMENT;

    device->txn_in_progress = 1;
    *device->cr_status = KAPS_RESTORE_NO_CHANGE;
    return KAPS_OK;
}

kaps_status
kaps_device_end_transaction(
    struct kaps_device * device)
{
    uint32_t offset;
    uint8_t *pending_count;

    KAPS_TRACE_IN("%p\n", device);
    if (!device)
        return KAPS_INVALID_ARGUMENT;

    
    if (!device->nv_ptr)
        return KAPS_INVALID_ARGUMENT;

    *device->cr_status = KAPS_RESTORE_CHANGES_ABORTED;
    offset = device->nv_mem_mgr->offset_device_pending_list;
    pending_count = (uint8_t *) device->nv_ptr + offset;
    *(uint32_t *) pending_count = 0;
    device->txn_in_progress = 0;
    return KAPS_OK;
}

kaps_status
kaps_device_query_restore_status(
    struct kaps_device * device,
    enum kaps_restore_status * status)
{
    KAPS_TRACE_IN("%p %p\n", device, status);
    if (!device)
        return KAPS_INVALID_ARGUMENT;

    if (!device->nv_ptr)
        return KAPS_INVALID_ARGUMENT;

    *status = *device->cr_status;
    KAPS_TRACE_OUT("%d\n", *status);
    return KAPS_OK;
}

kaps_status
kaps_device_clear_restore_status(
    struct kaps_device * device)
{
    KAPS_TRACE_IN("%p\n", device);
    if (!device)
        return KAPS_INVALID_ARGUMENT;

    if (!device->nv_ptr)
        return KAPS_INVALID_ARGUMENT;

    *device->cr_status = KAPS_RESTORE_NO_CHANGE;
    return KAPS_OK;
}

kaps_status
kaps_device_log_error(
    struct kaps_device * device,
    kaps_status return_status,
    char *fmt,
    ...)
{
    va_list ap;
    va_start(ap, fmt);

    kaps_sassert(device);

    if (device->main_dev)
        device = device->main_dev;

    kaps_vsnprintf(device->verbose_error_string, sizeof(device->verbose_error_string), fmt, ap);
    va_end(ap);

    return return_status;
}

const char *
kaps_device_get_last_error(
    struct kaps_device *device)
{
    KAPS_TRACE_IN("%p\n", device);
    kaps_sassert(device);

    if (device->main_dev)
        device = device->main_dev;

    return device->verbose_error_string;
}

kaps_status
kaps_device_thread_init(
    struct kaps_device * device,
    uint32_t tid,
    struct kaps_device ** thr_device)
{
    return KAPS_OK;
}

struct kaps_device *
kaps_get_main_bc_device(
    struct kaps_device *device)
{
    kaps_sassert(device);

    if (!device->is_selective_bc_enabled)
        return device;

    if (device->main_dev)
        device = device->main_dev;
    if (device->main_bc_device)
        return device->main_bc_device;

    return device;
}


kaps_status
kaps_device_get_ser_data(
    struct kaps_device *device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t func_num,
    uint32_t nbytes,
    uint8_t *bytes)
{
    kaps_status status = KAPS_OK;
    struct kaps_shadow_device *shadow = device->kaps_shadow;
    uint32_t db_group_id = 0;

    kaps_memset(bytes, 0, nbytes);

    if (KAPS_RPB_BLOCK_START <= blk_nr && blk_nr <= KAPS2_RPB_BLOCK_END && func_num == KAPS_FUNC1)
    {
        /*We need to get data for the Root Pivot Block*/
        uint32_t rpb_num = blk_nr - KAPS_RPB_BLOCK_START;
        uint8_t data_x[KAPS_RPB_WIDTH_8];
        uint8_t data_y[KAPS_RPB_WIDTH_8];
        uint32_t i;
        uint32_t shadow_row_num;

        kaps_assert(row_nr < 2 * device->hw_res->num_rows_in_rpb[blk_nr - KAPS_RPB_BLOCK_START],
                    "Incorrect RPB Row Nr \n");

        kaps_assert(nbytes == 2 * (KAPS_RPB_WIDTH_8 + 1), "Incorrect number of RPB bytes \n");

        kaps_memset(bytes, 0, nbytes);

        shadow_row_num = row_nr / 2;

        if (shadow->rpb_blks[rpb_num].rpb_rows[shadow_row_num].rpb_valid_data)
        {

            kaps_convert_dm_to_xy(shadow->rpb_blks[rpb_num].rpb_rows[shadow_row_num].data,
                                shadow->rpb_blks[rpb_num].rpb_rows[shadow_row_num].mask,
                                data_x,
                                data_y,
                                KAPS_RPB_WIDTH_8);


            /*
             * Copy X which goes to even row
             */
            for (i = 0; i < KAPS_RPB_WIDTH_8; i++)
            {
                KapsWriteBitsInArray(bytes, nbytes, 168 + ((KAPS_RPB_WIDTH_8 - i) * 8) + 3,
                                 168 + ((KAPS_RPB_WIDTH_8 - i) * 8) - 4, data_x[i]);
            }

            KapsWriteBitsInArray(bytes, nbytes, 168 + 3, 168 + 0, 0);
            KapsWriteBitsInArray(bytes, nbytes, 168 + 165, 168 + 164, 0x3); /*Valid */
            KapsWriteBitsInArray(bytes, nbytes, 168 + 167, 168 + 166, 0x0); /* Write Enable */


            /*
             * Copy Y which goes to odd row
             */
            for (i = 0; i < KAPS_RPB_WIDTH_8; i++)
            {
                KapsWriteBitsInArray(bytes, nbytes, ((KAPS_RPB_WIDTH_8 - i) * 8) + 3,
                           ((KAPS_RPB_WIDTH_8 - i) * 8) - 4, data_y[i]);
            }

            KapsWriteBitsInArray(bytes, nbytes, 3, 0, 0);
            /*
             * From Jericho2 onwards, X and Y share a common valid bit. So for coherency reasons, we set valid bit
             * to 0 for the odd entry in the X-Y pair (the entry that we were writing first in the old code)
             */
            KapsWriteBitsInArray(bytes, nbytes, 165, 164, 0x3);    /*Valid*/
            KapsWriteBitsInArray(bytes, nbytes, 167, 166, 0x0);    /* Write Enable */
        }

    }
    else if (KAPS_RPB_BLOCK_START <= blk_nr && blk_nr <= KAPS2_RPB_BLOCK_END && func_num == KAPS_FUNC4)
    {
        /*We need to get data for the ADS in between RPB and Small BB */
        uint32_t ads1_num = blk_nr - KAPS_RPB_BLOCK_START;
        struct kaps_ads *cur_ads1_row;

        kaps_assert(row_nr < device->hw_res->num_rows_in_rpb[blk_nr - KAPS_RPB_BLOCK_START], 
                    "Incorrect ADS1 Row Nr \n");

        cur_ads1_row = &shadow->ads_blks[ads1_num].ads_rows[row_nr];

        kaps_assert(nbytes == KAPS_ADS_WIDTH_8, "Incorrect number of ADS1 bytes \n");


        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 19, 0,  cur_ads1_row->bpm_ad);
        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 27, 20, cur_ads1_row->bpm_len );
        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 31, 28, cur_ads1_row->row_offset);

        kaps_write_first_half_of_fmap(bytes, KAPS_ADS_WIDTH_8, 32, cur_ads1_row);

        kaps_write_second_half_of_fmap(bytes, KAPS_ADS_WIDTH_8, 64, cur_ads1_row);

        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 103, 96, cur_ads1_row->key_shift );
        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 112, 104, cur_ads1_row->bkt_row);


    } 
    else if (KAPS2_SMALL_BB_START <= blk_nr && blk_nr <= KAPS2_SMALL_BB_END && func_num == KAPS_FUNC2)
    {
        /*We need to get data for the Small BBs */
        uint32_t small_bb_num = blk_nr - KAPS2_SMALL_BB_START;

        kaps_assert(row_nr < device->hw_res->num_rows_in_small_bb[0], "Incorrect Small BB Row Nr \n");

        kaps_assert(nbytes == KAPS_BKT_WIDTH_8, "Incorrect number of Small BB bytes \n");
        kaps_memcpy(bytes, shadow->small_bbs[small_bb_num].bkt_rows[row_nr].data, KAPS_BKT_WIDTH_8);

    }
    else if (KAPS2_ADS2_BLOCK_START <= blk_nr && blk_nr <= KAPS2_ADS2_BLOCK_END && func_num == KAPS_FUNC16)
    {
        /*We need to get the data for the ADS2*/
        uint32_t ads2_num = blk_nr - KAPS2_ADS2_BLOCK_START;
        struct kaps_ads *cur_ads2_row;

        if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
            kaps_assert(0, "ADS2 not valid for Small KAPS Device \n");

        kaps_assert(row_nr < KAPS_ADS2_MAX_NUM_ROWS, "Incorrect ADS2 Row Number \n");

        cur_ads2_row = &shadow->ads2_blks[ads2_num].ads_rows[row_nr];

        kaps_assert(nbytes == KAPS_ADS_WIDTH_8, "Incorrect number of ADS2 bytes \n");


        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 19, 0,  cur_ads2_row->bpm_ad);
        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 27, 20, cur_ads2_row->bpm_len );
        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 31, 28, cur_ads2_row->row_offset);

        kaps_write_first_half_of_fmap(bytes, KAPS_ADS_WIDTH_8, 32, cur_ads2_row);

        kaps_write_second_half_of_fmap(bytes, KAPS_ADS_WIDTH_8, 64, cur_ads2_row);

        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 103, 96, cur_ads2_row->key_shift );
        KapsWriteBitsInArray(bytes, KAPS_ADS_WIDTH_8, 117, 104, cur_ads2_row->bkt_row);


    }
    else if (KAPS2_LARGE_BB_START <= blk_nr && blk_nr <= KAPS2_LARGE_BB_END &&
        (func_num == KAPS_FUNC2 || func_num == KAPS_FUNC10))
    {
        /*We need to get the data for the Large BBs*/
        uint32_t large_bb_num = blk_nr - KAPS2_LARGE_BB_START;
        uint32_t sw_bb_num = 2 * large_bb_num;

        if (func_num == KAPS_FUNC10)
            sw_bb_num += 1;

        if (device->silicon_sub_type == KAPS_JERICHO_2_SUB_TYPE_TWO_LEVEL)
            kaps_assert(0, "Large KAPS BB not valid for Small KAPS Device \n");
                
        if (device->map->bb_map[db_group_id][sw_bb_num].bb_config == KAPS_LARGE_BB_WIDTH_A000_B000) 
        {
            /*Do nothing since the Large BB is not configured for use*/
        } 
        else 
        {
            kaps_assert(row_nr < device->map->bb_map[db_group_id][sw_bb_num].bb_num_rows, "Incorrect BB row number\n");

            kaps_assert(nbytes == KAPS_BKT_WIDTH_8, "Incorrect number of Large BB bytes \n");
            kaps_memcpy(bytes, shadow->bkt_blks[sw_bb_num].bkt_rows[row_nr].data, KAPS_BKT_WIDTH_8);
        }

    }
    else
    {
        status = KAPS_INVALID_ARGUMENT;
    }

    return status;
}




uint32_t
kaps_device_get_final_level_offset(
    struct kaps_device *device,
    struct kaps_db *db)
{
    uint32_t final_level_offset;
    uint32_t db_group_id = db->db_group_id;
    
    if (device->hw_res->total_small_bb[db_group_id])
    {
        /*If Small BBs are present in the device, then we need to
        figure out if we are writing to Small BB or Large BB*/
        if (db->num_algo_levels_in_db == 2)
        {
            final_level_offset = device->small_bb_offset;
        }
        else
        {
            final_level_offset = device->uda_offset;
        }
    }
    else 
    {
        final_level_offset = device->uda_offset;
    }

    return final_level_offset;
}


uint32_t
kaps_device_get_num_final_level_bbs(
    struct kaps_device *device,
    struct kaps_db *db)
{
    uint32_t num_bbs_in_final_level;
    uint32_t db_group_id = db->db_group_id;
    
    if (device->hw_res->total_small_bb[db_group_id])
    {
        /*If Small BBs are present in the device, then we need to
        figure out if we are writing to Small BB or Large BB*/
        if (db->num_algo_levels_in_db == 2)
        {
            num_bbs_in_final_level = device->hw_res->total_small_bb[db_group_id];
        }
        else
        {
            num_bbs_in_final_level = device->hw_res->total_lpus;
        }
    }
    else 
    {
        num_bbs_in_final_level = device->hw_res->total_lpus;
    }

    return num_bbs_in_final_level;
}


struct kaps_aging_entry*
kaps_device_get_active_aging_table(
    struct kaps_device *device,
    struct kaps_db *db)
{
    struct kaps_aging_entry *active_aging_table = NULL;
    uint32_t db_group_id = db->db_group_id;

    if (device->hw_res->total_small_bb[db_group_id])
    {
        /*If Small BBs are present in the device, then we need to
        figure out if we should operate on Small ageing table or Large ageing table*/
        if (db->num_algo_levels_in_db == 2)
        {
            active_aging_table = device->small_aging_table;
        }
        else
        {
            active_aging_table = device->aging_table;
        }
    }
    else 
    {
        active_aging_table = device->aging_table;
    }

    return active_aging_table;
}



uint32_t 
kaps_device_get_active_num_hb_blocks(
    struct kaps_device *device,
    struct kaps_db *db)
{
    uint32_t num_active_hb_blocks = 0;
    uint32_t db_group_id = db->db_group_id;

    if (device->hw_res->total_small_bb[db_group_id])
    {
        if (db->num_algo_levels_in_db == 2)
        {
            num_active_hb_blocks = device->num_small_hb_blocks;
        }
        else
        {
            num_active_hb_blocks = device->num_hb_blocks;
        }
    }
    else 
    {
        num_active_hb_blocks = device->num_hb_blocks;
    }

    return num_active_hb_blocks;
}



uint32_t kaps_device_get_active_num_hb_rows(
    struct kaps_device *device,
    struct kaps_db *db)
{
    uint32_t num_active_hb_rows = 0;
    uint32_t db_group_id = db->db_group_id;
    
    if (device->hw_res->total_small_bb[db_group_id])
    {
        if (db->num_algo_levels_in_db == 2)
        {
            num_active_hb_rows = device->num_rows_in_small_hb_block;
        }
        else
        {
            num_active_hb_rows = device->num_rows_in_hb_block;
        }
    }
    else 
    {
        num_active_hb_rows = device->num_rows_in_hb_block;
    }

    return num_active_hb_rows;
    
}





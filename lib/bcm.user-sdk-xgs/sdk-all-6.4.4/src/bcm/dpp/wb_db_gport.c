/*
 * $Id: wb_db_gport.c,v 1.25 Broadcom SDK $
 *
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * Warmboot - Level 2 support (GPORT Library)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/module.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/wb_db_gport.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/drv.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/sw_db.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

STATIC int
_bcm_dpp_wb_gport_pack_port_state(int unit, int version, _BCM_GPORT_PHY_PORT_INFO *state, void *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_port_state(int unit, int version, void *wb_state, _BCM_GPORT_PHY_PORT_INFO *state);

STATIC int
_bcm_dpp_wb_gport_pack_trill_info_state(int unit, int version, _bcm_petra_trill_info_t *state, void *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_trill_info_state(int unit, int version, void *wb_state, _bcm_petra_trill_info_t *state);

STATIC int
_bcm_dpp_wb_gport_pack_mc_trill_state(int unit, int version, bcm_gport_t *state, void *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_mc_trill_state(int unit, int version, void *wb_state, bcm_gport_t *state);

STATIC int
_bcm_dpp_wb_gport_pack_trill_route_state(int unit, int version, _bcm_petra_trill_mc_trill_route_list_t *state, void *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_trill_route_state(int unit, int version, void *wb_state, _bcm_petra_trill_mc_trill_route_list_t *state);

STATIC int
_bcm_dpp_wb_gport_pack_trill_src_state(int unit, int version, _bcm_petra_trill_mc_trill_src_list_t *state, void *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_trill_src_state(int unit, int version, void *wb_state, _bcm_petra_trill_mc_trill_src_list_t *state);

STATIC int
_bcm_dpp_wb_gport_pack_trill_vpn_state(int unit, int version, _bcm_petra_trill_vpn_info_t *state, void *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_trill_vpn_state(int unit, int version, void *wb_state, _bcm_petra_trill_vpn_info_t *state);

STATIC int
_bcm_dpp_wb_gport_pack_inlif_match_state(int unit, int version, _bcm_dpp_inlif_match_info_t *state, _bcm_dpp_inlif_match_info_t  *wb_state);

STATIC int
_bcm_dpp_wb_gport_unpack_inlif_match_state(int unit, int version, _bcm_dpp_inlif_match_info_t  *wb_state, _bcm_dpp_inlif_match_info_t *state);

bcm_dpp_wb_gport_info_t *_dpp_wb_gport_info_p[BCM_MAX_NUM_UNITS] = {0};

/*
 * functions for hash management on backing storage
 */

uint32
shr_wb_hl_default_hash_f(shr_wb_htb_key_t key, uint32 size)
{
    return(_shr_crc32(~0, key, size));
}

int
shr_wb_hl_default_key_cmp_f(shr_wb_htb_key_t key_a, shr_wb_htb_key_t key_b, uint32 size)
{
    return(sal_memcmp(key_a, key_b, size));
}

int
shr_wb_hl_hash_func_set(shr_wb_hash_list_info_t *hl_info, shr_wb_htb_hash_f func)
{
    int                      rc = BCM_E_NONE;

    if (hl_info == NULL) {
        return(BCM_E_PARAM);
    }

    hl_info->hash_f = func;

    return(rc);
}
int
shr_wb_hl_key_cmp_func_set(shr_wb_hash_list_info_t *hl_info, shr_wb_htb_key_cmp_f func)
{
    int                      rc = BCM_E_NONE;

    if (hl_info == NULL) {
        return(BCM_E_PARAM);
    }

    hl_info->key_cmp_f = func;

    return(rc);
}

int
shr_wb_hl_element_size_get(int                        unit,
                           shr_wb_hash_list_desc_t   *hl_desc,
                           int                       *element_size)
{
    int                      rc = BCM_E_NONE;


    /* size of all the data elements */
    (*element_size) = (sizeof(shr_wb_hash_entry_t) -
                               (sizeof(shr_wb_htb_key_t) + sizeof(shr_wb_htb_data_t))) +
                   hl_desc->key_size +
                   hl_desc->data_size;
    if (hl_desc->is_offset_aligned) {
        if (*element_size & (0x3)) (*element_size) = ((*element_size) + 4) & ~(0x3);
    }  

    return(rc);
}

int
shr_wb_hl_resource_info_get(int                        unit,
                            shr_wb_hash_list_desc_t   *hl_desc,
                            shr_wb_hast_list_rinfo_t  *rinfo) 
{
    int                      rc = BCM_E_NONE;
    int                      element_size = 0;


    /* size of hash hdr offsets (hash elements and free list) */
    rinfo->size = 0;

    rinfo->hd_lists_off = rinfo->size + hl_desc->scache_off;
    rinfo->size += sizeof(shr_wb_htb_off_t) * hl_desc->size_hdr_list;

    rinfo->free_list_off = rinfo->size + hl_desc->scache_off;
    rinfo->size += sizeof(shr_wb_htb_off_t);

    if (hl_desc->is_offset_aligned) {
        if (rinfo->size & (0x3)) rinfo->size = (rinfo->size + 4) & ~(0x3);
    }

    /* size of all the data elements */
    rinfo->data_off = rinfo->size + hl_desc->scache_off;
    shr_wb_hl_element_size_get(unit, hl_desc, &element_size);
    rinfo->size += element_size * hl_desc->max_elements;
    rinfo->element_size = element_size;

    if (hl_desc->is_offset_aligned) {
        if (rinfo->size & (0x3)) rinfo->size = (rinfo->size + 4) & ~(0x3);
    }

    return(rc);
}

#define SHR_WB_DB_GPORT_SAVE_OVERWRITE_DATA 0x1
/*
 * initialize hash list pointers and possibly complete list in persistent storage
 */
int
shr_wb_hl_init(int unit, int flags, shr_wb_hash_list_info_t **hl_info,
                                    shr_wb_hash_list_desc_t *hl_desc)
{
    int                        rc = BCM_E_NONE;
    shr_wb_hash_list_info_t   *tmp_hl_info = NULL;
    shr_wb_htb_off_t           base_element_off, element_off;
    int                        hl_nbr, element_nbr, element_size;
    shr_wb_hast_list_rinfo_t   rinfo;
    shr_wb_hash_entry_t       *hash_entry;


    BCMDNX_INIT_FUNC_DEFS;

    /* consistency check, check that hash table size is power of 2 */
    if((hl_desc->size_hdr_list & (hl_desc->size_hdr_list - 1)) != 0) {
        rc = BCM_E_MEMORY;
    }

    tmp_hl_info = sal_alloc(sizeof(shr_wb_hash_list_info_t), "hash list");
    if (tmp_hl_info == NULL) {
        rc = BCM_E_MEMORY;
        goto err;
    }
    sal_memset(tmp_hl_info, 0, sizeof(shr_wb_hash_list_info_t));

    /* create lock */
    tmp_hl_info->lock = sal_mutex_create("wb hl info");
    if (tmp_hl_info->lock == NULL) {
        rc = BCM_E_RESOURCE;
        goto err;
    }

    /* update hash list description */
    tmp_hl_info->type = hl_desc->type;
    tmp_hl_info->size_hdr_list = hl_desc->size_hdr_list;
    tmp_hl_info->max_elements = hl_desc->max_elements;
    tmp_hl_info->key_size = hl_desc->key_size;
    tmp_hl_info->data_size = hl_desc->data_size;
    tmp_hl_info->scache_handle = hl_desc->scache_handle;
    tmp_hl_info->scache_ptr = hl_desc->scache_ptr;
    tmp_hl_info->is_offset_aligned = hl_desc->is_offset_aligned;
    tmp_hl_info->scache_off = hl_desc->scache_off;

    shr_wb_hl_resource_info_get(unit, hl_desc, &rinfo);
    tmp_hl_info->element_size = rinfo.element_size;
    element_off = rinfo.data_off;
    rc = shr_wb_hl_hash_func_set(tmp_hl_info, shr_wb_hl_default_hash_f);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = shr_wb_hl_key_cmp_func_set(tmp_hl_info, shr_wb_hl_default_key_cmp_f);
    BCMDNX_IF_ERR_EXIT(rc);

    /* initialize hash list head pointers */
    tmp_hl_info->hd_lists_off = rinfo.hd_lists_off;
    tmp_hl_info->hd_lists_off_p = (shr_wb_htb_off_t *)SHR_WB_HTB_OFF2PTR(tmp_hl_info, rinfo.hd_lists_off);
    if (flags & SHR_WB_DB_GPORT_SAVE_OVERWRITE_DATA) {
        for (hl_nbr = 0; hl_nbr < tmp_hl_info->size_hdr_list; hl_nbr++) {
            *(tmp_hl_info->hd_lists_off_p + hl_nbr) = SHR_WB_NULL_OFFSET;
        }
    }

    /* initialize hash list free pointer list */
    tmp_hl_info->free_list_off = rinfo.free_list_off;
    tmp_hl_info->free_list_off_p = (shr_wb_htb_off_t *)SHR_WB_HTB_OFF2PTR(tmp_hl_info, rinfo.free_list_off);
    if (flags & SHR_WB_DB_GPORT_SAVE_OVERWRITE_DATA) {
        *(tmp_hl_info->free_list_off_p) = SHR_WB_NULL_OFFSET;
    }
    base_element_off = rinfo.data_off;
    element_size = rinfo.element_size;
    element_off = base_element_off; 

    if (flags & SHR_WB_DB_GPORT_SAVE_OVERWRITE_DATA) {
        *(tmp_hl_info->free_list_off_p) = element_off;
        for (element_nbr = 0; element_nbr < (hl_desc->max_elements - 1); element_nbr++) {
            hash_entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(tmp_hl_info, element_off);
            element_off += element_size;
            SHR_WB_OFFSET_SET(&(hash_entry->next_offset), element_off, hl_desc->is_aligned);
        }
        hash_entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(tmp_hl_info, element_off);
        SHR_WB_OFFSET_SET(&(hash_entry->next_offset), SHR_WB_NULL_OFFSET, hl_desc->is_aligned);
    }

    (*hl_info) = tmp_hl_info;

    BCMDNX_IF_ERR_EXIT(rc);
    BCM_EXIT;

err:
    if (tmp_hl_info != NULL) {
        sal_free(tmp_hl_info);
    }
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * find hash list element in persistent storage
 */
STATIC int
shr_wb_hl_find_element(int unit, shr_wb_hash_list_info_t *hl_info, shr_wb_htb_key_t key,
                                         uint32 *hash_index, shr_wb_hash_entry_t **entry,
                                                            shr_wb_hash_entry_t **prev_entry)
{
    int                      rc = BCM_E_NONE;
    uint32                   offset;
    int                      found, cur_iter;


    /* compute hash */
    (*hash_index) = hl_info->hash_f(key, hl_info->key_size);

    /* reduce hash_index to be within the range of hash header list array */
    (*hash_index) &= (hl_info->size_hdr_list - 1);
    
    offset = *(hl_info->hd_lists_off_p + (*hash_index));
    (*prev_entry) = (*entry) = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, offset);
    
    for (cur_iter = 0, found = FALSE; ((offset != SHR_WB_NULL_OFFSET) && (cur_iter < SHR_WB_MAX_ITER)); cur_iter++) {
        /* check if the entry matches */
        if (hl_info->key_cmp_f(key, (shr_wb_htb_key_t)&((*entry)->key), hl_info->key_size) == 0) {
            found = TRUE;
            break;
        }

        /* move to next entry */
        offset = SHR_WB_OFFSET_GET(&((*entry)->next_offset), hl_info->is_offset_aligned);
        (*prev_entry) = (*entry);
        (*entry) = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, offset);
    }

    if (found != TRUE) {
        return(BCM_E_NOT_FOUND);
    }

    return(rc);
}

/*
 * allocate hash list element in persistent storage
 */
int
shr_wb_hl_alloc_element(int unit, shr_wb_hash_list_info_t *hl_info, shr_wb_htb_key_t key,
                                                      shr_wb_htb_data_t data, int update)
{
    int                      rc = BCM_E_NONE;
    uint32                   hash_index, fl_offset, fl_next_offset, hl_offset;
    shr_wb_hash_entry_t     *fl_entry;


    SHR_WB_HL_LOCK(unit, hl_info);

    /*
     * allocate element from free list
     */
    fl_offset = *(hl_info->free_list_off_p);
    if (fl_offset == SHR_WB_NULL_OFFSET) {
        rc = BCM_E_NOT_FOUND;
        goto err;
    }

    fl_entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, fl_offset);
    fl_next_offset = SHR_WB_OFFSET_GET(&(fl_entry->next_offset), hl_info->is_offset_aligned);
    *(hl_info->free_list_off_p) = fl_next_offset;

    /*
     * update element
     */
    sal_memcpy(&(fl_entry->key), key, hl_info->key_size);
    sal_memcpy(&(fl_entry->data), data, hl_info->data_size);

    /*
     * insert element into hash list
     */
    hash_index = hl_info->hash_f(key, hl_info->key_size);
    hash_index &= (hl_info->size_hdr_list - 1);
    hl_offset = *(hl_info->hd_lists_off_p + hash_index);
    SHR_WB_OFFSET_SET(&(fl_entry->next_offset), hl_offset, hl_info->is_offset_aligned);
    *(hl_info->hd_lists_off_p + hash_index) = fl_offset;


    /*
     * update persistent storage
     */
    if (update == TRUE) {
        /* update free list */
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, sizeof(shr_wb_htb_off_t),
               (uint8 *)hl_info->free_list_off_p,
               SHR_WB_HTB_COMMIT_OFF(hl_info, hl_info->free_list_off));
        if (rc != BCM_E_NONE)
            goto err;

        /* update element */
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, hl_info->element_size,
                              (uint8 *)fl_entry, SHR_WB_HTB_COMMIT_OFF(hl_info, fl_offset));
        if (rc != BCM_E_NONE)
            goto err;

        /* update hash element list */
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, sizeof(shr_wb_htb_off_t),
              (uint8 *)(hl_info->hd_lists_off_p + hash_index),
              SHR_WB_HTB_COMMIT_OFF(hl_info, hl_info->hd_lists_off + hash_index * sizeof(shr_wb_htb_off_t)));
        if (rc != BCM_E_NONE)
            goto err;
    }

    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);

err:
    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);
}

/*
 * update hash list element in persistent storage
 */
int
shr_wb_hl_update_element(int unit, shr_wb_hash_list_info_t *hl_info, shr_wb_htb_key_t key,
                                                        shr_wb_htb_data_t data, int update)
{
    int                      rc = BCM_E_NONE;
    shr_wb_hash_entry_t     *entry, *prev_entry;
    uint32                   hash_index, offset;


    SHR_WB_HL_LOCK(unit, hl_info);

    /* find element */
    rc = shr_wb_hl_find_element(unit, hl_info, key, &hash_index, &entry, &prev_entry);
    if (rc != BCM_E_NONE) {
        goto err;
    }

    /* update element */
    sal_memcpy(&(entry->key), key, hl_info->key_size);
    sal_memcpy(&(entry->data), data, hl_info->data_size);

    /* update persistent storage */
    if (update == TRUE) {
        offset = SHR_WB_OFFSET_GET(&(prev_entry->next_offset), hl_info->is_offset_aligned);
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, hl_info->element_size,
                              (uint8 *)entry, SHR_WB_HTB_COMMIT_OFF(hl_info, offset));
        if (rc != BCM_E_NONE)
            goto err;
    }

    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);

err:
    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);
}

/*
 * de-allocate hash list element in persistent storage
 */
int
shr_wb_hl_dealloc_element(int unit, shr_wb_hash_list_info_t *hl_info, shr_wb_htb_key_t key,
                                                                                  int update)
{
    int                      rc = BCM_E_NONE;
    shr_wb_hash_entry_t     *entry, *prev_entry;
    uint32                   hash_index, offset, next_offset, fl_offset;


    SHR_WB_HL_LOCK(unit, hl_info);

    /* find element */
    rc = shr_wb_hl_find_element(unit, hl_info, key, &hash_index, &entry, &prev_entry);
    if (rc != BCM_E_NONE) {
        goto err;
    }

    /* delete element from hash list */
    offset = SHR_WB_OFFSET_GET(&(prev_entry->next_offset), hl_info->is_offset_aligned);
    next_offset = SHR_WB_OFFSET_GET(&(entry->next_offset), hl_info->is_offset_aligned);
    SHR_WB_OFFSET_SET(&(prev_entry->next_offset), next_offset, hl_info->is_offset_aligned);
   
    /* put element into free list */
    fl_offset = *(hl_info->free_list_off_p);
    SHR_WB_OFFSET_SET(&(entry->next_offset), fl_offset, hl_info->is_offset_aligned);
    *(hl_info->free_list_off_p) = offset;

    /* update persistent storage */
    if (update == TRUE) {
        /* update hash list */
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, sizeof(shr_wb_htb_off_t),
                 (uint8 *)&(prev_entry->next_offset), SHR_WB_HTB_COMMIT_OFF(hl_info,
                                  SHR_WB_HTB_PTR2OFF(hl_info, &(prev_entry->next_offset))) );
        if (rc != BCM_E_NONE)
            goto err;

        /* update deleted entry */
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, sizeof(shr_wb_htb_off_t),
                 (uint8 *)&(entry->next_offset), SHR_WB_HTB_COMMIT_OFF(hl_info, offset));
        if (rc != BCM_E_NONE)
            goto err;

        /* update free list */
        rc = soc_scache_commit_specific_data(unit, hl_info->scache_handle, sizeof(shr_wb_htb_off_t),
                 (uint8 *)(hl_info->free_list_off_p),
                 SHR_WB_HTB_COMMIT_OFF(hl_info, hl_info->free_list_off));
        if (rc != BCM_E_NONE)
            goto err;
    }

    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);

err:
    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);
}

/*
 * re-initialize hash list in persistent storage.
 * hash list headers marked empty and all resources returned to free list.
 */
int
shr_wb_hl_reinit(int unit, shr_wb_hash_list_info_t *hl_info)
{
    int                        rc = BCM_E_NONE;
    shr_wb_hash_list_desc_t    hl_desc;
    shr_wb_hast_list_rinfo_t   rinfo;
    shr_wb_htb_off_t           base_element_off, element_off;
    int                        hl_nbr, element_nbr, element_size;
    shr_wb_hash_entry_t       *hash_entry;


    if (hl_info == NULL) {
        return(BCM_E_PARAM);
    }

    SHR_WB_HL_LOCK(unit, hl_info);

    BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, 0, hl_info->size_hdr_list,
                                  hl_info->max_elements, hl_info->key_size, hl_info->data_size,
                                  hl_info->scache_handle, hl_info->scache_ptr, hl_info->scache_off,
                                  hl_info->is_offset_aligned);
    shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);

    /* initialize hash list head pointers */
    for (hl_nbr = 0; hl_nbr < hl_info->size_hdr_list; hl_nbr++) {
        *(hl_info->hd_lists_off_p + hl_nbr) = SHR_WB_NULL_OFFSET;
    }

    /* initialize hash list free pointer list */
    *(hl_info->free_list_off_p) = SHR_WB_NULL_OFFSET;
    base_element_off = rinfo.data_off;
    element_size = rinfo.element_size;
    element_off = base_element_off; 

    *(hl_info->free_list_off_p) = element_off;
    for (element_nbr = 0; element_nbr < (hl_desc.max_elements - 1); element_nbr++) {
        hash_entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, element_off);
        element_off += element_size;
        SHR_WB_OFFSET_SET(&(hash_entry->next_offset), element_off, hl_desc.is_aligned);
    }
    hash_entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, element_off);
    SHR_WB_OFFSET_SET(&(hash_entry->next_offset), SHR_WB_NULL_OFFSET, hl_desc.is_aligned);

    SHR_WB_HL_UNLOCK(unit, hl_info);

    return(rc);
}

/*
 * de-initialize hash list in persistent storage
 * release memory resources
 */
int
shr_wb_hl_deinit(int unit, shr_wb_hash_list_info_t *hl_info)
{
    int                      rc = BCM_E_NONE;


    if (hl_info == NULL) {
        return(rc);
    }

    sal_mutex_destroy(hl_info->lock);
    sal_free(hl_info);

    return(rc);
}

/*
 * iterate over the hash list and invoke callback function
 * NOTE: If require during init of hash list a callback function can be registered.
 */
int
shr_wb_hl_iterate(int unit, int version, shr_wb_hash_list_info_t *hl_info, shr_wb_cb_t restore_cb)
{
    int                      rc = BCM_E_NONE;
    int                      hl_nbr;
    shr_wb_hash_entry_t     *entry;
    uint32                   offset;
    int                      cur_iter;
    shr_wb_htb_key_t         key;
    shr_wb_htb_data_t        data;


    if ( (hl_info == NULL) || (restore_cb == NULL) ) {
        return(BCM_E_PARAM);
    }

    /* walk through all the hash list head pointers */
    for (hl_nbr = 0; hl_nbr < hl_info->size_hdr_list; hl_nbr++) {
        offset = *(hl_info->hd_lists_off_p + hl_nbr);
        entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, offset);
    
        /* walk single hash list head pointer */
        for (cur_iter = 0; ((offset != SHR_WB_NULL_OFFSET) && (cur_iter < SHR_WB_MAX_ITER)); cur_iter++) {
            key = (shr_wb_htb_key_t)&entry->key;
            data = (shr_wb_htb_data_t)&entry->data;
            rc = (*restore_cb)(unit, version, key, data);
            if (rc != BCM_E_NONE) {
                return(rc);
            }

            /* move to next entry */
            offset = SHR_WB_OFFSET_GET(&(entry->next_offset), hl_info->is_offset_aligned);
            entry = (shr_wb_hash_entry_t *)SHR_WB_HTB_OFF2PTR(hl_info, offset);
        }
    }

    return(rc);
}

/*
 * local functions
 */
STATIC int
_bcm_dpp_wb_gport_layout_init(int unit, int version, soc_scache_handle_t scache_handle, int init_hl_fl_ptrs, int init_hl_fl)
{
    int                        rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t   *wb_info; 
    int                        entry_size, total_size = 0;
    shr_wb_hash_list_desc_t    hl_desc;
    shr_wb_hast_list_rinfo_t   rinfo;
    int                        is_offset_aligned = FALSE;
    int                        flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    wb_info->version = version;

    if (init_hl_fl) {
        flags |= SHR_WB_DB_GPORT_SAVE_OVERWRITE_DATA;
    }

#ifdef _SHR_WB_HTB_OFFSET_ALIGNMENT
    is_offset_aligned = TRUE;
#else /* _SHR_WB_HTB_OFFSET_ALIGNMENT */
    is_offset_aligned = FALSE;
#endif /* !(_SHR_WB_HTB_OFFSET_ALIGNMENT) */

    switch (version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:

            /*
             * LIF Match
             */
            wb_info->inlif_match_off = total_size;
            entry_size = sizeof(_bcm_dpp_inlif_match_info_t) * BCM_DPP_WB_GPORT_HL_LIF_MATCH_NBR_ENTRIES(unit);
            if (entry_size & (0x3)) entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            total_size += entry_size;

            /*
             * port info
             */
            entry_size = sizeof(bcm_dpp_wb_gport_port_info_t);

            BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, shr_wb_hl_port_info,
                                  BCM_DPP_WB_GPORT_HL_PORT_INFO_HL_SZ(unit),
                                  BCM_DPP_WB_GPORT_HL_PORT_INFO_NBR_ENTRIES(unit),
                                  BCM_DPP_WB_GPORT_HL_PORT_INFO_KEY_SZ(unit),
                                  entry_size,
                                  scache_handle,
                                  wb_info->scache_ptr,
                                  total_size,
                                  is_offset_aligned);

            shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);
            total_size += rinfo.size;

            if (init_hl_fl_ptrs == TRUE) {
                rc = shr_wb_hl_deinit(unit, wb_info->port_info);
                BCMDNX_IF_ERR_EXIT(rc);

                rc = shr_wb_hl_init(unit, flags, &(wb_info->port_info), &hl_desc);
                BCMDNX_IF_ERR_EXIT(rc);
            }

            /*
             * trill info
             */
            entry_size = sizeof(bcm_dpp_wb_gport_trill_info_t);

            BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, shr_wb_hl_trill_info,
                                  BCM_DPP_WB_GPORT_HL_TRILL_INFO_HL_SZ(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_INFO_NBR_ENTRIES(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_INFO_KEY_SZ(unit),
                                  entry_size,
                                  scache_handle,
                                  wb_info->scache_ptr,
                                  total_size,
                                  is_offset_aligned);

            shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);
            total_size += rinfo.size;

            if (init_hl_fl_ptrs == TRUE) {
                rc = shr_wb_hl_deinit(unit, wb_info->trill_info);
                BCMDNX_IF_ERR_EXIT(rc);

                rc = shr_wb_hl_init(unit, flags, &(wb_info->trill_info), &hl_desc);
                BCMDNX_IF_ERR_EXIT(rc);
            }

            /*
             * mc trill
             */
            /* old implementation that use sw db for mapping mc-id to nickname*/
            if (SOC_DPP_CONFIG(unit)->trill.mc_id){

                entry_size = sizeof(bcm_dpp_wb_gport_mc_trill_t);

                BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, shr_wb_hl_mc_trill_info,
                                      BCM_DPP_WB_GPORT_HL_MC_TRILL_HL_SZ(unit),
                                      BCM_DPP_WB_GPORT_HL_MC_TRILL_NBR_ENTRIES(unit),
                                      BCM_DPP_WB_GPORT_HL_MC_TRILL_KEY_SZ(unit),
                                      entry_size,
                                      scache_handle,
                                      wb_info->scache_ptr,
                                      total_size,
                                      is_offset_aligned);
      
                shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);
                total_size += rinfo.size;
      
                if (init_hl_fl_ptrs == TRUE) {
                    rc = shr_wb_hl_deinit(unit, wb_info->mc_trill_info);
                    BCMDNX_IF_ERR_EXIT(rc);

                    rc = shr_wb_hl_init(unit, flags, &(wb_info->mc_trill_info), &hl_desc);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }
            /*
             * trill route
             */
            entry_size = sizeof(bcm_dpp_wb_gport_trill_route_info_t);

            BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, shr_wb_hl_trill_route_info,
                                  BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_HL_SZ(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_NBR_ENTRIES(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_ROUTE_KEY_SZ(unit),
                                  entry_size,
                                  scache_handle,
                                  wb_info->scache_ptr,
                                  total_size,
                                  is_offset_aligned);

            shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);
            total_size += rinfo.size;

            if (init_hl_fl_ptrs == TRUE) {
                rc = shr_wb_hl_deinit(unit, wb_info->trill_route_info);
                BCMDNX_IF_ERR_EXIT(rc);

                rc = shr_wb_hl_init(unit, flags, &(wb_info->trill_route_info), &hl_desc);
                BCMDNX_IF_ERR_EXIT(rc);
            }

            /*
             * trill src
             */
            entry_size = sizeof(bcm_dpp_wb_gport_trill_src_info_t);

            BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, shr_wb_hl_trill_src_info,
                                  BCM_DPP_WB_GPORT_HL_TRILL_SRC_HL_SZ(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_SRC_NBR_ENTRIES(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_SRC_KEY_SZ(unit),
                                  entry_size,
                                  scache_handle,
                                  wb_info->scache_ptr,
                                  total_size,
                                  is_offset_aligned);

            shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);
            total_size += rinfo.size;

            if (init_hl_fl_ptrs == TRUE) {
                rc = shr_wb_hl_deinit(unit, wb_info->trill_src_info);
                BCMDNX_IF_ERR_EXIT(rc);

                rc = shr_wb_hl_init(unit, flags, &(wb_info->trill_src_info), &hl_desc);
                BCMDNX_IF_ERR_EXIT(rc);
            }     
                  
            /*
             * trill vpn
             */
            entry_size = sizeof(bcm_dpp_wb_gport_trill_vpn_info_t);

            BCM_DPP_WB_GPORT_HL_DESC_INIT(&hl_desc, shr_wb_hl_trill_vpn_info,
                                  BCM_DPP_WB_GPORT_HL_TRILL_VPN_HL_SZ(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_VPN_NBR_ENTRIES(unit),
                                  BCM_DPP_WB_GPORT_HL_TRILL_VPN_KEY_SZ(unit),
                                  entry_size,
                                  scache_handle,
                                  wb_info->scache_ptr,
                                  total_size,
                                  is_offset_aligned);

            shr_wb_hl_resource_info_get(unit, &hl_desc, &rinfo);
            total_size += rinfo.size;

            if (init_hl_fl_ptrs == TRUE) {
                rc = shr_wb_hl_deinit(unit, wb_info->trill_vpn_info);
                BCMDNX_IF_ERR_EXIT(rc);

                rc = shr_wb_hl_init(unit, flags, &(wb_info->trill_vpn_info), &hl_desc);
                BCMDNX_IF_ERR_EXIT(rc);
            }

            /*
             * update global information
             */
            wb_info->size = total_size;

            break;

        default:
            /* no other version supported */
            rc = BCM_E_INTERNAL;
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_port_state_restore_cb(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data)
{
    int                       rc = BCM_E_NONE;
    _BCM_GPORT_PHY_PORT_INFO  state, *port_state = NULL;
    int                       found = FALSE;
    shr_htb_data_t*           htb_pointer;


    BCMDNX_INIT_FUNC_DEFS;

    htb_pointer = (shr_htb_data_t *)(void*)&port_state;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* determine if the element is already allocated during init() phase. */
            /* NOTE: this check can be optimized                                  */
            rc = _bcm_dpp_sw_db_hash_vlan_find(unit, (shr_htb_key_t)key, htb_pointer, FALSE);
            if (rc == BCM_E_NONE) {
                found = TRUE;   
            }

            rc = _bcm_dpp_wb_gport_unpack_port_state(unit, version, (void *)data, &state);
            if (rc != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to unpack port state\n")));
            }

            if (found == FALSE) {
                /* allocate run time data structure */
                /* NOTE: description of memory block is not restored */
                BCMDNX_ALLOC(port_state, sizeof(_BCM_GPORT_PHY_PORT_INFO), "port state");
                if (port_state == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("port state restore")));
                }
            }
         
            /* update state */
            (*port_state) = state;

            if (found == FALSE) {
                /* insert in hash list */
                rc = _bcm_dpp_sw_db_hash_vlan_insert(unit, key, port_state);
                BCMDNX_IF_ERR_EXIT(rc);
                found = TRUE;
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if ( (found == FALSE) && (port_state != NULL) ) {
        BCM_FREE(port_state);
    }

    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_gport_restore_port_state(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_iterate(unit, wb_info->version, wb_info->port_info, _bcm_dpp_gport_port_state_restore_cb);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_trill_info_state_restore_cb(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data)
{
    int                       rc = BCM_E_NONE;
     _bcm_petra_trill_info_t  state, *trill_info_state = NULL;
    int                       found = FALSE;
    shr_htb_data_t*           htb_pointer;


    BCMDNX_INIT_FUNC_DEFS;

    htb_pointer = (shr_htb_data_t *)(void*)&trill_info_state;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* determine if the element is already allocated during init() phase. */
            /* NOTE: this check can be optimized                                  */
            rc = _bcm_dpp_sw_db_hash_trill_info_find(unit, (shr_htb_key_t)key, htb_pointer, FALSE);
            if (rc == BCM_E_NONE) {
                found = TRUE;   
            }

            rc = _bcm_dpp_wb_gport_unpack_trill_info_state(unit, version, (void *)data, &state);
            if (rc != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to unpack trill info state\n")));
            }

            if (found == FALSE) {
                /* allocate run time data structure */
                /* NOTE: description of memory block is not restored */
                BCMDNX_ALLOC(trill_info_state, sizeof(_bcm_petra_trill_info_t), "trill info state");
                if (trill_info_state == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("trill info state restore")));
                }
            }
         
            /* update state */
            (*trill_info_state) = state;

            if (found == FALSE) {
                /* insert in hash list */
                rc = _bcm_dpp_sw_db_hash_trill_info_insert(unit, key, trill_info_state);
                BCMDNX_IF_ERR_EXIT(rc);
                found = TRUE;
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if ( (found == FALSE) && (trill_info_state != NULL) ) {
        BCM_FREE(trill_info_state);
    }

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_restore_trill_info_state(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_iterate(unit, wb_info->version, wb_info->trill_info, _bcm_dpp_gport_trill_info_state_restore_cb);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_trill_vpn_state_restore_cb(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data)
{
    int                       rc = BCM_E_NONE;
    _bcm_petra_trill_vpn_info_t  state, *trill_vpn_state = NULL;
    int                       found = FALSE;
    shr_htb_data_t*           htb_pointer;


    BCMDNX_INIT_FUNC_DEFS;

    htb_pointer = (shr_htb_data_t *)(void*)&trill_vpn_state;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* determine if the element is already allocated during init() phase. */
            /* NOTE: this check can be optimized                                  */
            rc = _bcm_dpp_sw_db_hash_trill_vpn_find(unit, (shr_htb_key_t)key, htb_pointer, FALSE);
            if (rc == BCM_E_NONE) {
                found = TRUE;   
            }

            rc = _bcm_dpp_wb_gport_unpack_trill_vpn_state(unit, version, (void *)data, &state);
            if (rc != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to unpack trill info state\n")));
            }

            if (found == FALSE) {
                /* allocate run time data structure */
                /* NOTE: description of memory block is not restored */
                BCMDNX_ALLOC(trill_vpn_state, sizeof(_bcm_petra_trill_vpn_info_t), "trill vpn state");
                if (trill_vpn_state == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("trill vpn state restore")));
                }
            }
         
            /* update state */
            (*trill_vpn_state) = state;

            if (found == FALSE) {
                /* insert in hash list */
                rc = _bcm_dpp_sw_db_hash_trill_vpn_insert(unit, key, trill_vpn_state);
                BCMDNX_IF_ERR_EXIT(rc);
                found = TRUE;
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if ( (found == FALSE) && (trill_vpn_state != NULL) ) {
        BCM_FREE(trill_vpn_state);
    }

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_restore_trill_vpn_state(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_iterate(unit, wb_info->version, wb_info->trill_vpn_info, _bcm_dpp_gport_trill_vpn_state_restore_cb);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_dpp_gport_mc_trill_state_restore_cb(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data)
{
    int                       rc = BCM_E_NONE;
    bcm_gport_t               state, *mc_trill_state = NULL;
    int                       found = FALSE;
    shr_htb_data_t*           htb_pointer;


    BCMDNX_INIT_FUNC_DEFS;

    htb_pointer = (shr_htb_data_t *)(void*)&mc_trill_state;
    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* determine if the element is already allocated during init() phase. */
            /* NOTE: this check can be optimized                                  */
            rc = _bcm_dpp_sw_db_hash_mc_trill_find(unit, (shr_htb_key_t)key, htb_pointer, FALSE);
            if (rc == BCM_E_NONE) {
                found = TRUE;   
            }

            rc = _bcm_dpp_wb_gport_unpack_mc_trill_state(unit, version, (void *)data, &state);
            if (rc != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to unpack mc trill state\n")));
            }

            if (found == FALSE) {
                /* allocate run time data structure */
                /* NOTE: description of memory block is not restored */
                BCMDNX_ALLOC(mc_trill_state, sizeof(bcm_gport_t), "mc trill state");
                if (mc_trill_state == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("mc trill state restore")));
                }
            }
         
            /* update state */
            (*mc_trill_state) = state;

            if (found == FALSE) {
                /* insert in hash list */
                rc = _bcm_dpp_sw_db_hash_mc_trill_insert(unit, key, mc_trill_state);
                BCMDNX_IF_ERR_EXIT(rc);
                found = TRUE;
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if ( (found == FALSE) && (mc_trill_state != NULL) ) {
        BCM_FREE(mc_trill_state);
    }

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_restore_mc_trill_state(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_iterate(unit, wb_info->version, wb_info->mc_trill_info, _bcm_dpp_gport_mc_trill_state_restore_cb);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_trill_route_state_restore_cb(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data)
{
    int                       rc = BCM_E_NONE;
      _bcm_petra_trill_mc_trill_route_list_t state, *trill_route_state = NULL;
    int                       found = FALSE;
    shr_htb_data_t*           htb_pointer;


    BCMDNX_INIT_FUNC_DEFS;

    htb_pointer = (shr_htb_data_t *)(void*)&trill_route_state;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* determine if the element is already allocated during init() phase. */
            /* NOTE: this check can be optimized                                  */
            rc = _bcm_dpp_sw_db_hash_trill_route_info_find(unit, (shr_htb_key_t)key, htb_pointer, FALSE);
            if (rc == BCM_E_NONE) {
                found = TRUE;   
            }

            rc = _bcm_dpp_wb_gport_unpack_trill_route_state(unit, version, (void *)data, &state);
            if (rc != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to unpack trill route state\n")));
            }

            if (found == FALSE) {
                /* allocate run time data structure */
                /* NOTE: description of memory block is not restored */
                BCMDNX_ALLOC(trill_route_state, sizeof(_bcm_petra_trill_mc_trill_route_list_t), "trill route state");
                if (trill_route_state == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("trill route state restore")));
                }
            }
         
            /* update state */
            (*trill_route_state) = state;

            if ( found == FALSE) {
                /* insert in hash list */
                rc = _bcm_dpp_sw_db_hash_trill_route_info_insert(unit, key, trill_route_state);
                BCMDNX_IF_ERR_EXIT(rc);
                found = TRUE;
            }
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if ( (found == FALSE) && (trill_route_state != NULL) ) {
        BCM_FREE(trill_route_state);
    }

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_restore_trill_route_state(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_iterate(unit, wb_info->version, wb_info->trill_route_info, _bcm_dpp_gport_trill_route_state_restore_cb);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_trill_src_state_restore_cb(int unit, int version, shr_wb_htb_key_t key, shr_wb_htb_data_t data)
{
    int                       rc = BCM_E_NONE;
    _bcm_petra_trill_mc_trill_src_list_t  state, *trill_src_state = NULL;
    int                       found = FALSE;
    shr_htb_data_t*           htb_pointer;


    BCMDNX_INIT_FUNC_DEFS;

    htb_pointer = (shr_htb_data_t *)(void*)&trill_src_state;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* determine if the element is already allocated during init() phase. */
            /* NOTE: this check can be optimized                                  */
            rc = _bcm_dpp_sw_db_hash_trill_root_src_find(unit, (shr_htb_key_t)key, htb_pointer, FALSE);
            if (rc == BCM_E_NONE) {
                found = TRUE;   
            }

            rc = _bcm_dpp_wb_gport_unpack_trill_src_state(unit, version, (void *)data, &state);
            if (rc != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unable to unpack trill mc state\n")));
            }

            if (found == FALSE) {
                /* allocate run time data structure */
                /* NOTE: description of memory block is not restored */
                BCMDNX_ALLOC(trill_src_state, sizeof( _bcm_petra_trill_mc_trill_src_list_t), "trill src state");
                if (trill_src_state == NULL) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("trill src state restore")));
                }
            }
         
            /* update state */
            (*trill_src_state) = state;

            if (found == FALSE) {
                /* insert in hash list */
                rc = _bcm_dpp_sw_db_hash_trill_root_src_insert(unit, key, trill_src_state);
                BCMDNX_IF_ERR_EXIT(rc);
                found = TRUE;
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if ( (found == FALSE) && (trill_src_state != NULL) ) {
        BCM_FREE(trill_src_state);
    }

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_restore_trill_src_state(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_iterate(unit, wb_info->version, wb_info->trill_src_info, _bcm_dpp_gport_trill_src_state_restore_cb);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_gport_restore_lif_match_state(int unit)
{
    int                              rc = BCM_E_NONE, i;
    bcm_dpp_wb_gport_info_t         *wb_info; 

    _bcm_dpp_inlif_match_info_t       *wb_lif_match;
    _bcm_dpp_inlif_bookkeeping_t      *lif_info = INLIF_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - LIF Match Info State\n")));
           
    wb_lif_match = (_bcm_dpp_inlif_match_info_t *) (wb_info->scache_ptr + wb_info->inlif_match_off);
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            
            for (i=0; i < _BCM_GPORT_NOF_LIFS; i++) {

                rc = _bcm_dpp_wb_gport_unpack_inlif_match_state(unit, wb_info->version, &wb_lif_match[i], &lif_info->match_key[i]);
                BCMDNX_IF_ERR_EXIT(rc);

            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_gport_restore_gbl_state(int unit)
{
    int                                    rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(rc);

exit:

    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_gport_restore_gbl_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_gport_restore_port_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_gport_restore_trill_info_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id){
        rc = _bcm_dpp_wb_gport_restore_mc_trill_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if(SOC_IS_PETRAB(unit))
    {
        rc = _bcm_dpp_wb_gport_restore_trill_route_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        rc = _bcm_dpp_wb_gport_restore_trill_src_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    rc = _bcm_dpp_wb_gport_restore_trill_vpn_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_gport_restore_lif_match_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_port_state_save_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_update_port_state(unit, bcm_dpp_wb_gport_oper_save, key, data);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_save_port_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - Port, State\n")));

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            /* clear current state. The alternative is to update state that already exists */
            /* and create state that does not exist.                                       */
            rc = shr_wb_hl_reinit(unit, wb_info->port_info);
            BCMDNX_IF_ERR_EXIT(rc);
           
            /* save state */
            rc = _bcm_dpp_sw_db_hash_vlan_iterate(unit, _bcm_dpp_gport_port_state_save_cb);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_gport_trill_info_state_save_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_update_trill_info_state(unit, bcm_dpp_wb_gport_oper_save, key, data);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_save_trill_info_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - Trill MC State\n")));
           
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_reinit(unit, wb_info->trill_info);
            BCMDNX_IF_ERR_EXIT(rc);
           
            /* save state */
            rc = _bcm_dpp_sw_db_hash_trill_info_iterate(unit, _bcm_dpp_gport_trill_info_state_save_cb);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_gport_mc_trill_state_save_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_update_mc_trill_state(unit, bcm_dpp_wb_gport_oper_save, key, data);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_save_mc_trill_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - MC Trill State\n")));
           
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_reinit(unit, wb_info->mc_trill_info);
            BCMDNX_IF_ERR_EXIT(rc);
           
            /* save state */
            rc = _bcm_dpp_sw_db_hash_mc_trill_iterate(unit, _bcm_dpp_gport_mc_trill_state_save_cb);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_gport_trill_route_state_save_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_update_trill_route_state(unit, bcm_dpp_wb_gport_oper_save, key, data);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_save_trill_route_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - MC Trill State\n")));
           
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_reinit(unit, wb_info->trill_route_info);
            BCMDNX_IF_ERR_EXIT(rc);
           
            /* save state */
            rc = _bcm_dpp_sw_db_hash_trill_route_info_iterate(unit, _bcm_dpp_gport_trill_route_state_save_cb);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_gport_trill_src_state_save_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_update_trill_src_state(unit, bcm_dpp_wb_gport_oper_save, key, data);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_save_trill_src_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - MC Trill State\n")));
           
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_reinit(unit, wb_info->trill_src_info);
            BCMDNX_IF_ERR_EXIT(rc);
           
            /* save state */
            rc = _bcm_dpp_sw_db_hash_trill_src_iterate(unit, _bcm_dpp_gport_trill_src_state_save_cb);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_dpp_gport_trill_vpn_state_save_cb(int unit, shr_htb_key_t key, shr_htb_data_t data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_update_trill_vpn_state(unit, bcm_dpp_wb_gport_oper_save, key, data);
            BCMDNX_IF_ERR_EXIT(rc);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version\n")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_dpp_wb_gport_save_trill_vpn_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - VPN Trill State\n")));
           
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = shr_wb_hl_reinit(unit, wb_info->trill_vpn_info);
            BCMDNX_IF_ERR_EXIT(rc);
           
            /* save state */
            rc = _bcm_dpp_sw_db_hash_trill_vpn_iterate(unit, _bcm_dpp_gport_trill_vpn_state_save_cb);
            BCMDNX_IF_ERR_EXIT(rc);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_dpp_wb_gport_save_lif_match_state(int unit)
{
    int                              rc = BCM_E_NONE, i;
    bcm_dpp_wb_gport_info_t         *wb_info; 

    _bcm_dpp_inlif_match_info_t       *wb_inlif_match;
    _bcm_dpp_inlif_bookkeeping_t      *inlif_info = INLIF_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - LIF Match Info State\n")));
           
    wb_inlif_match = (_bcm_dpp_inlif_match_info_t *) (wb_info->scache_ptr + wb_info->inlif_match_off);
    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            
            for (i=0; i < _BCM_GPORT_NOF_LIFS; i++) {

                rc = _bcm_dpp_wb_gport_pack_inlif_match_state(unit, wb_info->version, &inlif_info->match_key[i], &wb_inlif_match[i]);
                BCMDNX_IF_ERR_EXIT(rc);

            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown Scache Version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_gport_save_gbl_state(int unit)
{
    int                                    rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_INIT,
              (BSL_META_U(unit,
                          "\nGport - Global, State\n")));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_gport_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_gport_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_gport_info_p[unit], sizeof(bcm_dpp_wb_gport_info_t), "wb gport");
        if (_dpp_wb_gport_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Unable to allocate WB Gport info memory\n")));
        }
    }

    sal_memset(_dpp_wb_gport_info_p[unit], 0x00, sizeof(bcm_dpp_wb_gport_info_t));

#ifdef _SHR_WB_HTB_OFFSET_ALIGNMENT
    BCM_DPP_WB_GPORT_INFO(unit)->is_offset_aligned = TRUE;
#else /* _SHR_WB_HTB_OFFSET_ALIGNMENT */
    BCM_DPP_WB_GPORT_INFO(unit)->is_offset_aligned = FALSE;
#endif /* !(_SHR_WB_HTB_OFFSET_ALIGNMENT) */

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_gport_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    /* this unit not init, no nothing to detach; done. */
    if (_dpp_wb_gport_info_p[unit] == NULL) {
        BCM_EXIT;
    }
    
    BCM_FREE(_dpp_wb_gport_info_p[unit]->port_info);
    BCM_FREE(_dpp_wb_gport_info_p[unit]->trill_info);
    BCM_FREE(_dpp_wb_gport_info_p[unit]->mc_trill_info);
    BCM_FREE(_dpp_wb_gport_info_p[unit]->trill_route_info);
    BCM_FREE(_dpp_wb_gport_info_p[unit]->trill_src_info);
    BCM_FREE(_dpp_wb_gport_info_p[unit]->trill_vpn_info);

    if (_dpp_wb_gport_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_gport_info_p[unit]);
        _dpp_wb_gport_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 *
 * The save functionality has been implemented
 *   - some consistency checks rather then blindly coping data structures
 *     Thus easier to debug, catch errors.
 *   - The above implementation aslo make its semetric with the the per
 *     API update.
 */

int
_bcm_dpp_wb_gport_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_GPORT_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_gport_save_port_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_gport_save_trill_info_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    /* old implementation that use sw db for mapping mc-id to nickname*/
    if (SOC_DPP_CONFIG(unit)->trill.mc_id){
        rc = _bcm_dpp_wb_gport_save_mc_trill_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    if(SOC_IS_PETRAB(unit))
    {
        rc = _bcm_dpp_wb_gport_save_trill_route_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        rc = _bcm_dpp_wb_gport_save_trill_src_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    rc = _bcm_dpp_wb_gport_save_trill_vpn_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_gport_save_lif_match_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_gport_save_gbl_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_GPORT_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * if init code is not changed (i.e to allocate hash list resources), restore can be one of the
 * last operations that is done. This will ensure that duplicate entries are not created by init
 * phase and restore phase (e.g. restore phase runs before init phase and both of them
 * allocate resources). By running restore phase later, no existing code has to change as
 * 
 * The other option is that restore operation runs before init code. However this would require
 * modifications to init code and will make the init code more difficult to read. This is thus
 * not a preferred solution. The preferred option is that there is minimum disruption to the
 * normal code execution path.
 */
int
_bcm_dpp_wb_gport_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_GPORT_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_gport_info_t *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, (BCM_MODULE_GPORT), 0);

    rc = _bcm_dpp_wb_gport_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    wb_info->scache_handle = wb_handle;

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_gport_layout_init(unit, recovered_ver, wb_info->scache_handle, 
                                           TRUE /* reinit ptrs */, FALSE /* don't reinit lists */);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        /* restore state */
        rc = _bcm_dpp_wb_gport_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_gport_layout_init(unit, version, wb_info->scache_handle, 
                                               FALSE /* don't reinit ptrs */, FALSE /* don't reinit lists */);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_gport_layout_init(unit, version, wb_info->scache_handle, 
                                               TRUE /* reinit ptrs */, FALSE /* don't reinit lists */);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_gport_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_gport_layout_init(unit, version, wb_info->scache_handle, 
                                           FALSE /* don't reinit ptrs */, FALSE /* don't reinit lists */);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_gport_layout_init(unit, version, wb_info->scache_handle, 
                                           TRUE /* reinit ptrs */, TRUE /* reinit lists */);
        BCMDNX_IF_ERR_EXIT(rc);

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_gport_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_gport_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
        }
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_gport_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_gport_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_gport_update_port_state(int                           unit,
                                    bcm_dpp_wb_gport_oper_type_t  oper,
                                    shr_htb_key_t                 key,
                                    shr_htb_data_t                data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 
    _BCM_GPORT_PHY_PORT_INFO        *phy_port;
    bcm_dpp_wb_gport_port_info_t     wb_port_info;
    shr_wb_htb_data_t                wb_data;
    int                              is_update = TRUE;


    BCMDNX_INIT_FUNC_DEFS;

    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from gport_init().                                                     */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    if (oper == bcm_dpp_wb_gport_oper_save) {
        is_update = FALSE;
    }
    else {
        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            /* mark dirty state. Currently no other processing done           */
            /* if required framework cache could be updated without updating  */
            /* the persistent storage                                         */
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        if (SOC_WARM_BOOT(unit)) {
            /* no update required during warmboot bringup. Backing/persistent */
            /* storage is already in sync with the last state.                */
            BCM_EXIT;
        }
    }

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    phy_port = (_BCM_GPORT_PHY_PORT_INFO *)data;

    if ( (oper == bcm_dpp_wb_gport_oper_add) || (oper == bcm_dpp_wb_gport_oper_update) ||
                                                       (oper == bcm_dpp_wb_gport_oper_save) ) {
        switch(wb_info->version) {
            case BCM_DPP_WB_GPORT_VERSION_1_0:
                _bcm_dpp_wb_gport_pack_port_state(unit, wb_info->version, phy_port, (void *)&wb_port_info);
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
                break;
        }

        wb_data = (shr_wb_htb_data_t)&wb_port_info;

        if (oper == bcm_dpp_wb_gport_oper_update) {
            rc = shr_wb_hl_update_element(unit, wb_info->port_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_add) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->port_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_save) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->port_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (oper == bcm_dpp_wb_gport_oper_delete) {
        rc = shr_wb_hl_dealloc_element(unit, wb_info->port_info, (shr_wb_htb_key_t)key, is_update);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version ")));
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_gport_update_trill_info_state(int                           unit,
                                        bcm_dpp_wb_gport_oper_type_t  oper,
                                        shr_htb_key_t                 key,
                                        shr_htb_data_t                data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 
    _bcm_petra_trill_info_t          *trill_info;
    bcm_dpp_wb_gport_trill_info_t    wb_trill_info;
    shr_wb_htb_data_t                wb_data;
    int                              is_update = TRUE;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    if (oper == bcm_dpp_wb_gport_oper_save) {
        is_update = FALSE;
    }
    else {
        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        if (SOC_WARM_BOOT(unit)) {
            BCM_EXIT;
        }
    }

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    trill_info = (_bcm_petra_trill_info_t *) data;

    if ( (oper == bcm_dpp_wb_gport_oper_add) || (oper == bcm_dpp_wb_gport_oper_update) ||
                                                       (oper == bcm_dpp_wb_gport_oper_save) ) {
        switch(wb_info->version) {
            case BCM_DPP_WB_GPORT_VERSION_1_0:
                _bcm_dpp_wb_gport_pack_trill_info_state(unit, wb_info->version, trill_info, (void *)&wb_trill_info);
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
                break;
        }

        wb_data = (shr_wb_htb_data_t)&wb_trill_info;

        if (oper == bcm_dpp_wb_gport_oper_update) {
            rc = shr_wb_hl_update_element(unit, wb_info->trill_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_add) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_save) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (oper == bcm_dpp_wb_gport_oper_delete) {
        rc = shr_wb_hl_dealloc_element(unit, wb_info->trill_info, (shr_wb_htb_key_t)key, is_update);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version ")));
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_gport_update_mc_trill_state(int                           unit,
                                        bcm_dpp_wb_gport_oper_type_t  oper,
                                        shr_htb_key_t                 key,
                                        shr_htb_data_t                data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 
    bcm_gport_t                      port_p;
    bcm_dpp_wb_gport_mc_trill_t      wb_mc_trill;
    shr_wb_htb_data_t                wb_data;
    int                              is_update = TRUE;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    if (oper == bcm_dpp_wb_gport_oper_save) {
        is_update = FALSE;
    }
    else {
        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        if (SOC_WARM_BOOT(unit)) {
            BCM_EXIT;
        }
    }

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    port_p = *((bcm_gport_t *)data);

    if ( (oper == bcm_dpp_wb_gport_oper_add) || (oper == bcm_dpp_wb_gport_oper_update) ||
                                                       (oper == bcm_dpp_wb_gport_oper_save) ) {
        switch(wb_info->version) {
            case BCM_DPP_WB_GPORT_VERSION_1_0:
                _bcm_dpp_wb_gport_pack_mc_trill_state(unit, wb_info->version, &port_p, (void *)&wb_mc_trill);
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
                break;
        }

        wb_data = (shr_wb_htb_data_t)&wb_mc_trill;

        if (oper == bcm_dpp_wb_gport_oper_update) {
            rc = shr_wb_hl_update_element(unit, wb_info->mc_trill_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_add) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->mc_trill_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_save) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->mc_trill_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (oper == bcm_dpp_wb_gport_oper_delete) {
        rc = shr_wb_hl_dealloc_element(unit, wb_info->mc_trill_info, (shr_wb_htb_key_t)key, is_update);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version ")));
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_dpp_wb_gport_update_trill_vpn_state(int                           unit,
                                        bcm_dpp_wb_gport_oper_type_t  oper,
                                        shr_htb_key_t                 key,
                                        shr_htb_data_t                data)
{
    int                                 rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t             *wb_info; 
    _bcm_petra_trill_vpn_info_t         *vpn_info;
    bcm_dpp_wb_gport_trill_vpn_info_t   wb_vpn_info;
    shr_wb_htb_data_t                   wb_data;
    int                                 is_update = TRUE;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    if (oper == bcm_dpp_wb_gport_oper_save) {
        is_update = FALSE;
    }
    else {
        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        if (SOC_WARM_BOOT(unit)) {
            BCM_EXIT;
        }
    }

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    vpn_info = (_bcm_petra_trill_vpn_info_t *) data;

    if ( (oper == bcm_dpp_wb_gport_oper_add) || (oper == bcm_dpp_wb_gport_oper_update) ||
                                                       (oper == bcm_dpp_wb_gport_oper_save) ) {
        switch(wb_info->version) {
            case BCM_DPP_WB_GPORT_VERSION_1_0:
                _bcm_dpp_wb_gport_pack_trill_vpn_state(unit, wb_info->version, vpn_info, (void *)&wb_vpn_info);
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
                break;
        }

        wb_data = (shr_wb_htb_data_t)&wb_vpn_info;

        if (oper == bcm_dpp_wb_gport_oper_update) {
            rc = shr_wb_hl_update_element(unit, wb_info->trill_vpn_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_add) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_vpn_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_save) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_vpn_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (oper == bcm_dpp_wb_gport_oper_delete) {
        rc = shr_wb_hl_dealloc_element(unit, wb_info->trill_vpn_info, (shr_wb_htb_key_t)key, is_update);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version ")));
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_gport_update_trill_route_state(int                           unit,
                                        bcm_dpp_wb_gport_oper_type_t  oper,
                                        shr_htb_key_t                 key,
                                        shr_htb_data_t                data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 
    _bcm_petra_trill_mc_trill_route_list_t *trill_route;
    bcm_dpp_wb_gport_trill_route_info_t wb_trill_route;
    shr_wb_htb_data_t                wb_data;
    int                              is_update = TRUE;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    if (oper == bcm_dpp_wb_gport_oper_save) {
        is_update = FALSE;
    }
    else {
        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        if (SOC_WARM_BOOT(unit)) {
            BCM_EXIT;
        }
    }

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    trill_route = (_bcm_petra_trill_mc_trill_route_list_t *)data;

    if ( (oper == bcm_dpp_wb_gport_oper_add) || (oper == bcm_dpp_wb_gport_oper_update) ||
                                                       (oper == bcm_dpp_wb_gport_oper_save) ) {
        switch(wb_info->version) {
            case BCM_DPP_WB_GPORT_VERSION_1_0:
                _bcm_dpp_wb_gport_pack_trill_route_state(unit, wb_info->version, trill_route, (void *)&wb_trill_route);
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
                break;
        }

        wb_data = (shr_wb_htb_data_t)&wb_trill_route;

        if (oper == bcm_dpp_wb_gport_oper_update) {
            rc = shr_wb_hl_update_element(unit, wb_info->trill_route_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_add) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_route_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_save) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_route_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (oper == bcm_dpp_wb_gport_oper_delete) {
        rc = shr_wb_hl_dealloc_element(unit, wb_info->trill_route_info, (shr_wb_htb_key_t)key, is_update);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version ")));
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_gport_update_trill_src_state(int                           unit,
                                        bcm_dpp_wb_gport_oper_type_t  oper,
                                        shr_htb_key_t                 key,
                                        shr_htb_data_t                data)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 
    _bcm_petra_trill_mc_trill_src_list_t *trill_src;
    bcm_dpp_wb_gport_trill_src_info_t wb_trill_src;
    shr_wb_htb_data_t                wb_data;
    int                              is_update = TRUE;


    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    if (oper == bcm_dpp_wb_gport_oper_save) {
        is_update = FALSE;
    }
    else {
        /* check if immediate sync not enabled  */
        if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
            BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
            BCM_EXIT;
        }

        if (SOC_WARM_BOOT(unit)) {
            BCM_EXIT;
        }
    }

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    trill_src = (_bcm_petra_trill_mc_trill_src_list_t *)data;

    if ( (oper == bcm_dpp_wb_gport_oper_add) || (oper == bcm_dpp_wb_gport_oper_update) ||
                                                       (oper == bcm_dpp_wb_gport_oper_save) ) {
        switch(wb_info->version) {
            case BCM_DPP_WB_GPORT_VERSION_1_0:
                _bcm_dpp_wb_gport_pack_trill_src_state(unit, wb_info->version, trill_src, (void *)&wb_trill_src);
                break;

            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
                break;
        }

        wb_data = (shr_wb_htb_data_t)&wb_trill_src;

        if (oper == bcm_dpp_wb_gport_oper_update) {
            rc = shr_wb_hl_update_element(unit, wb_info->trill_src_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_add) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_src_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else if (oper == bcm_dpp_wb_gport_oper_save) {
            rc = shr_wb_hl_alloc_element(unit, wb_info->trill_src_info, (shr_wb_htb_key_t)key, wb_data, is_update);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    else if (oper == bcm_dpp_wb_gport_oper_delete) {
        rc = shr_wb_hl_dealloc_element(unit, wb_info->trill_src_info, (shr_wb_htb_key_t)key, is_update);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version ")));
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_gport_update_inlif_match_state(int                           unit,
                                           int                           lif,
                                           _bcm_dpp_inlif_match_info_t     *match)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_info_t         *wb_info; 
    _bcm_dpp_inlif_match_info_t       *wb_lif_match;
    soc_scache_handle_t              wb_handle;
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_DPP_WB_GPORT_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        BCM_DPP_WB_DEV_GPORT_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }
    
    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_GPORT, 0);

    wb_info = BCM_DPP_WB_GPORT_INFO(unit);
    wb_lif_match = (_bcm_dpp_inlif_match_info_t *) (wb_info->scache_ptr + wb_info->inlif_match_off);

    switch(wb_info->version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            rc = _bcm_dpp_wb_gport_pack_inlif_match_state(unit, wb_info->version, match, &wb_lif_match[lif]);
            BCMDNX_IF_ERR_EXIT(rc);
            data = (uint8 *)&(wb_lif_match[lif]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(_bcm_dpp_inlif_match_info_t);
            break;
            
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unknown version")));
            break;
    }
    
    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * local functions
 */
STATIC int
_bcm_dpp_wb_gport_pack_port_state(int unit, int version, _BCM_GPORT_PHY_PORT_INFO *state, void *wb_state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_port_info_t    *wb_info;                                             


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_port_info_t *)wb_state;
            _bcm_dpp_wb_store32(state->phy_gport, (uint8*)&wb_info->phy_gport);
            _bcm_dpp_wb_store32(state->encap_id, (uint8*)&wb_info->encap_id);
            wb_info->type = state->type;

            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_unpack_port_state(int unit, int version, void *wb_state, _BCM_GPORT_PHY_PORT_INFO *state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_port_info_t    *wb_info; /* this data structure is in cache. Thus structure */
                                              /* may not be aligned.                             */


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_port_info_t *)wb_state;
            state->phy_gport = _bcm_dpp_wb_load32((uint8 *)&wb_info->phy_gport);
            state->encap_id = _bcm_dpp_wb_load32((uint8 *)&wb_info->encap_id);
            state->type = wb_info->type;

            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_pack_trill_info_state(int unit, int version, _bcm_petra_trill_info_t *state, void *wb_state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_info_t *wb_info;


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_info_t *)wb_state;
            _bcm_dpp_wb_store32(state->type, (uint8*)&wb_info->type);
            _bcm_dpp_wb_store32(state->fec_id, (uint8*)&wb_info->fec_id);
            _bcm_dpp_wb_store32(state->use_cnt, (uint8*)&wb_info->use_cnt);
            
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_unpack_trill_info_state(int unit, int version, void *wb_state, _bcm_petra_trill_info_t *state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_info_t    *wb_info;


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_info_t *)wb_state;
            state->type = _bcm_dpp_wb_load32((uint8 *)&wb_info->type); /* currently it's aligned */
            state->fec_id = _bcm_dpp_wb_load32((uint8 *)&wb_info->fec_id); /* currently it's aligned */
            state->use_cnt = _bcm_dpp_wb_load32((uint8 *)&wb_info->use_cnt);
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}
STATIC int
_bcm_dpp_wb_gport_pack_trill_vpn_state(int unit, int version, _bcm_petra_trill_vpn_info_t *state, void *wb_state)
{
    int                                     rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_vpn_info_t       *wb_info;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_vpn_info_t *)wb_state;
            _bcm_dpp_wb_store32(state->flags, (uint8*)&wb_info->flags);
            _bcm_dpp_wb_store16(state->high_vid, (uint8*)&wb_info->high_vid);
            _bcm_dpp_wb_store16(state->low_vid, (uint8*)&wb_info->low_vid);
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}
STATIC int
_bcm_dpp_wb_gport_unpack_trill_vpn_state(int unit, int version, void *wb_state, _bcm_petra_trill_vpn_info_t *state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_vpn_info_t    *wb_info;


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_vpn_info_t *)wb_state;
            state->flags        = _bcm_dpp_wb_load32((uint8 *)&wb_info->flags); 
            state->high_vid     = _bcm_dpp_wb_load16((uint8 *)&wb_info->high_vid); 
            state->low_vid      = _bcm_dpp_wb_load16((uint8 *)&wb_info->low_vid); 

            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_pack_mc_trill_state(int unit, int version, bcm_gport_t *state, void *wb_state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_mc_trill_t *wb_info;


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_mc_trill_t *)wb_state;
            _bcm_dpp_wb_store32(*state, (uint8*)&wb_info->gport);

            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_unpack_mc_trill_state(int unit, int version, void *wb_state, bcm_gport_t *state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_mc_trill_t    *wb_info;


    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_mc_trill_t *)wb_state;
            (*state) = _bcm_dpp_wb_load32((uint8 *)&wb_info->gport); /* currently it's aligned */

            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}


STATIC int
_bcm_dpp_wb_gport_pack_trill_route_state(int unit, int version, _bcm_petra_trill_mc_trill_route_list_t *state, void *wb_state)
{
    int rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_route_info_t *wb_info;
    int route;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_route_info_t *)wb_state;
            _bcm_dpp_wb_store32(state->nof_routes, (uint8*)&wb_info->nof_routes);
            for (route=0; route < _BCM_PETRA_TRILL_MC_ROOT_NOF_VLAN; route++) {
                _bcm_dpp_wb_store32(state->route[route].flags, (uint8*)&wb_info->route[route].flags);
                _bcm_dpp_wb_store32(state->route[route].root_name, (uint8*)&wb_info->route[route].root_name);
                _bcm_dpp_wb_store16(state->route[route].vlan, (uint8*)&wb_info->route[route].vlan);
                _bcm_dpp_wb_store32(state->route[route].group, (uint8*)&wb_info->route[route].group);
            }
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_unpack_trill_route_state(int unit, int version, void *wb_state, _bcm_petra_trill_mc_trill_route_list_t *state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_route_info_t    *wb_info;
    int route;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_route_info_t *)wb_state;
            state->nof_routes = _bcm_dpp_wb_load32((uint8 *)&wb_info->nof_routes); /* currently it's aligned */
            for (route=0; route < _BCM_PETRA_TRILL_MC_ROOT_NOF_VLAN; route++) {
                state->route[route].flags =     _bcm_dpp_wb_load32((uint8 *)&wb_info->route[route].flags);
                state->route[route].root_name = _bcm_dpp_wb_load32((uint8 *)&wb_info->route[route].root_name); 
                state->route[route].vlan =      _bcm_dpp_wb_load16((uint8 *)&wb_info->route[route].vlan);  
                state->route[route].group =     _bcm_dpp_wb_load32((uint8 *)&wb_info->route[route].group);     
            }

            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}


STATIC int
_bcm_dpp_wb_gport_pack_trill_src_state(int unit, int version, _bcm_petra_trill_mc_trill_src_list_t *state, void *wb_state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_src_info_t *wb_info;
    int bridge;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_src_info_t *)wb_state;
            _bcm_dpp_wb_store32(state->nof_src_rbridge, (uint8*)&wb_info->nof_src_rbridge);
            for (bridge=0; bridge < _BCM_PETRA_TRILL_MC_ROOT_NOF_SRC_RBRIDGE; bridge++) {
                _bcm_dpp_wb_store32(state->src_rbridge[bridge].src_rbridge, (uint8*)&wb_info->src_rbridge[bridge].src_rbridge);
                _bcm_dpp_wb_store32(state->src_rbridge[bridge].port, (uint8*)&wb_info->src_rbridge[bridge].port);
            }
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_unpack_trill_src_state(int unit, int version, void *wb_state, _bcm_petra_trill_mc_trill_src_list_t *state)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_gport_trill_src_info_t    *wb_info;
    int bridge;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (bcm_dpp_wb_gport_trill_src_info_t *)wb_state;
            state->nof_src_rbridge = _bcm_dpp_wb_load32((uint8 *)&wb_info->nof_src_rbridge); /* currently it's aligned */
            for (bridge=0; bridge < _BCM_PETRA_TRILL_MC_ROOT_NOF_SRC_RBRIDGE; bridge++) {
                state->src_rbridge[bridge].src_rbridge = _bcm_dpp_wb_load32((uint8 *)&wb_info->src_rbridge[bridge].src_rbridge);
                state->src_rbridge[bridge].port = _bcm_dpp_wb_load32((uint8 *)&wb_info->src_rbridge[bridge].port); 
            }
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}


STATIC int
_bcm_dpp_wb_gport_pack_inlif_match_state(int unit, int version, _bcm_dpp_inlif_match_info_t *state, _bcm_dpp_inlif_match_info_t  *wb_state)
{
    int                        rc = BCM_E_NONE;
    _bcm_dpp_inlif_match_info_t *wb_info;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (_bcm_dpp_inlif_match_info_t *)wb_state;
            _bcm_dpp_wb_store32(state->lif_type, (uint8*)&wb_info->lif_type);
            _bcm_dpp_wb_store32(state->flags, (uint8*)&wb_info->flags);
            _bcm_dpp_wb_store32(state->criteria, (uint8*)&wb_info->criteria);
            _bcm_dpp_wb_store32(state->port, (uint8*)&wb_info->port);
            _bcm_dpp_wb_store32(state->match1, (uint8*)&wb_info->match1);
            _bcm_dpp_wb_store32(state->match2, (uint8*)&wb_info->match2);
            _bcm_dpp_wb_store32(state->match_tunnel, (uint8*)&wb_info->match_tunnel);
            _bcm_dpp_wb_store32(state->match_ethertype, (uint8*)&wb_info->match_ethertype);
            _bcm_dpp_wb_store32(state->gport_id, (uint8*)&wb_info->gport_id);
            _bcm_dpp_wb_store32(state->key1, (uint8*)&wb_info->key1);
            _bcm_dpp_wb_store32(state->peer_gport, (uint8*)&wb_info->peer_gport);
			_bcm_dpp_wb_store32(state->learn_gport_id, (uint8*)&wb_info->learn_gport_id);
            _bcm_dpp_wb_store32(state->tpid_profile_type, (uint8*)&wb_info->tpid_profile_type);
            _bcm_dpp_wb_store32(state->vsi, (uint8*)&wb_info->vsi);
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

STATIC int
_bcm_dpp_wb_gport_unpack_inlif_match_state(int unit, int version, _bcm_dpp_inlif_match_info_t  *wb_state, _bcm_dpp_inlif_match_info_t *state)
{
    int                          rc = BCM_E_NONE;
    _bcm_dpp_inlif_match_info_t   *wb_info;

    switch(version) {
        case BCM_DPP_WB_GPORT_VERSION_1_0:
            wb_info = (_bcm_dpp_inlif_match_info_t *)wb_state;
            state->lif_type           = _bcm_dpp_wb_load32((uint8 *)&wb_info->lif_type);
            state->flags              = _bcm_dpp_wb_load32((uint8 *)&wb_info->flags);
            state->criteria           = _bcm_dpp_wb_load32((uint8 *)&wb_info->criteria);
            state->port               = _bcm_dpp_wb_load32((uint8 *)&wb_info->port);
            state->match1             = _bcm_dpp_wb_load32((uint8 *)&wb_info->match1);
            state->match2             = _bcm_dpp_wb_load32((uint8 *)&wb_info->match2);
            state->match_tunnel       = _bcm_dpp_wb_load32((uint8 *)&wb_info->match_tunnel);
            state->match_ethertype    = _bcm_dpp_wb_load32((uint8 *)&wb_info->match_ethertype);
            state->gport_id           = _bcm_dpp_wb_load32((uint8 *)&wb_info->gport_id);
            state->key1               = _bcm_dpp_wb_load32((uint8 *)&wb_info->key1);
            state->peer_gport         = _bcm_dpp_wb_load32((uint8 *)&wb_info->peer_gport);
			state->learn_gport_id         = _bcm_dpp_wb_load32((uint8 *)&wb_info->learn_gport_id);
            state->tpid_profile_type  = _bcm_dpp_wb_load32((uint8 *)&wb_info->tpid_profile_type);
            state->vsi                = _bcm_dpp_wb_load32((uint8 *)&wb_info->vsi);
            break;

        default:
            rc = BCM_E_INTERNAL;
            break;
    }

    return(rc);
}

#endif /* BCM_WARM_BOOT_SUPPORT */


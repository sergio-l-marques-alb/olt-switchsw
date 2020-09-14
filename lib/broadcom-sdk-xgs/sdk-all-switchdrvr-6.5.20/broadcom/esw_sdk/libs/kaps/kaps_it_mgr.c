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

#include "kaps_it_mgr.h"
#include "kaps_bitmap.h"
#include "kaps_db_internal.h"
#include "kaps_algo_hw.h"

static kaps_status
kaps_kaps_it_alloc(
    struct it_mgr *self,
    struct kaps_db *db,
    enum kaps_entry_status type,
    uint32_t size,
    struct kaps_ab_info *ab)
{
    struct kaps_it_mgr *kaps_mgr = &self->u.kaps;
    uint32_t bank_nr = -1;

    if (type != DECISION_TRIE_0)
        return KAPS_INTERNAL_ERROR;

    if (db->num_algo_levels_in_db == 3)
    {
        uint32_t ab_num_for_db;

        ab_num_for_db = ab->ab_num % KAPS2_NUM_AB_PER_RPB;

        ab->base_addr = ab_num_for_db * KAPS_ADS2_VIRTUAL_IT_SIZE_PER_AB;
        return KAPS_OK;
    }

    bank_nr = ab->ab_num;

    if (bank_nr == -1)
        return KAPS_INTERNAL_ERROR;

    if (kaps_mgr->bank[bank_nr].type != UNHANDLED)
        return KAPS_OUT_OF_UIT;

    kaps_mgr->bank[bank_nr].type = type;
    kaps_mgr->bank[bank_nr].is_used_bmp = 1;
    kaps_mgr->bank[bank_nr].nab = 1;
    kaps_mgr->bank[bank_nr].db = db;

    ab->base_addr = 0;
    return KAPS_OK;
}

static kaps_status
kaps_kaps_it_free(
    struct it_mgr *self,
    struct kaps_db *db,
    struct kaps_ab_info *ab,
    uint32_t size)
{
    uint32_t bank_nr;
    struct kaps_it_mgr *kaps_mgr = &self->u.kaps;

    (void) size;

    if (db->num_algo_levels_in_db == 3)
    {
        return KAPS_OK;
    }

    bank_nr = ab->ab_num;

    kaps_mgr->bank[bank_nr].type = UNHANDLED;
    kaps_mgr->bank[bank_nr].is_used_bmp = 0;
    kaps_mgr->bank[bank_nr].nab = 0;
    kaps_mgr->bank[bank_nr].db = NULL;

    return KAPS_OK;
}

static kaps_status
kaps_it_mgr_fit_ab(
    struct kaps_ab_info *ab,
    uint32_t * ab_width,
    uint32_t * size,
    uint32_t * max_ab_per_it_bank)
{
    uint32_t ab_width_in_bytes = *ab_width;
    struct kaps_db *db = ab->db;

    if (db->parent)
        db = db->parent;

    if (db->num_algo_levels_in_db == 3)
    {

        *size = ab->num_slots;
        /*
         * The BData in the ADS-1 can't hold the lmpsofar AD. The BData is a pointer to ADS-2. This location in ADS-2
         * will hold the lmpsofar AD. So we need an extra entry in the IT to store the lmpsofar AD. So adding 1 to the
         * size
         */
        *size = *size + 1;
        *max_ab_per_it_bank = 1;
        return KAPS_OK;
    }

    if (ab_width_in_bytes <= 10)
    {
        *size = 4096;
        *max_ab_per_it_bank = 1;
    }
    else if (ab_width_in_bytes <= 20)
    {
        *size = 2048;
        *max_ab_per_it_bank = 2;
    }
    else if (ab_width_in_bytes <= 40)
    {
        *size = 1024;
        *max_ab_per_it_bank = 4;
    }
    else if (ab_width_in_bytes <= 80)
    {
        *size = 512;
        *max_ab_per_it_bank = 8;
    }
    else
    {
        kaps_sassert(0);
        return KAPS_INTERNAL_ERROR;
    }
    return KAPS_OK;
}

kaps_status
kaps_it_mgr_init(
    struct kaps_device * device,
    struct kaps_allocator * alloc,
    struct it_mgr ** mgr_pp)
{
    struct it_mgr *mgr;
    uint32_t i;

    mgr = alloc->xcalloc(alloc->cookie, 1, sizeof(struct it_mgr));
    if (!mgr)
        return KAPS_OUT_OF_MEMORY;

    mgr->alloc = alloc;
    mgr->device = device;

    for (i = 0; i < KAPS_MAX_NUM_ADS_BLOCKS; ++i)
    {
        mgr->u.kaps.bank[i].bank_no = i;
        mgr->u.kaps.bank[i].is_used_bmp = 0;
        mgr->u.kaps.bank[i].nab = 0;
        mgr->u.kaps.bank[i].type = UNHANDLED;
        mgr->u.kaps.bank[i].db = NULL;
    }

    *mgr_pp = mgr;
    return KAPS_OK;
}

kaps_status
kaps_it_mgr_wb_alloc(
    struct it_mgr * self,
    struct kaps_db * db,
    enum kaps_entry_status type,
    uint32_t ab_width_in_bytes,
    struct kaps_ab_info * ab)
{
    uint32_t size = 0;
    uint32_t max_ab_per_it_bank = 0;

    kaps_it_mgr_fit_ab(ab, &ab_width_in_bytes, &size, &max_ab_per_it_bank);

    if (db->parent)
        db = db->parent;

    KAPS_STRY(kaps_kaps_it_alloc(self, db, type, size, ab));

    return KAPS_OK;
}

kaps_status
kaps_it_mgr_alloc(
    struct it_mgr * self,
    struct kaps_db * db,
    enum kaps_entry_status type,
    enum it_instance_id instance_id,
    uint32_t ab_width_in_bytes,
    struct kaps_ab_info * ab)
{
    uint32_t size = 0;
    uint32_t max_ab_per_it_bank = 0;
    uint32_t ab_width_8 = ab_width_in_bytes;

    if (ab->ab_init_done)
        return KAPS_OK;

    kaps_it_mgr_fit_ab(ab, &ab_width_in_bytes, &size, &max_ab_per_it_bank);

    /*
     * For trigger compression should pass the AB column width 
     */
    if (db->type == KAPS_DB_LPM)
    {
        ab_width_in_bytes = ab_width_8;
        
        if (ab_width_8 < 5)
            max_ab_per_it_bank = 1;
    }

    if (db->parent)
        db = db->parent;

    KAPS_STRY(kaps_kaps_it_alloc(self, db, type, size, ab));

    return KAPS_OK;
}

kaps_status
kaps_it_mgr_free(
    struct it_mgr * self,
    struct kaps_db *db,
    struct kaps_ab_info * ab,
    uint32_t ab_width_8,
    enum kaps_entry_status type)
{
    uint32_t size = 0;
    uint32_t max_ab_per_it_bank = 0;
    uint32_t ab_width_in_bytes = ab_width_8;

    kaps_it_mgr_fit_ab(ab, &ab_width_in_bytes, &size, &max_ab_per_it_bank);

    /*
     * For trigger compression should pass the AB column width 
     */
    if (ab->db->type == KAPS_DB_LPM)
    {
        ab_width_in_bytes = ab_width_8;
        
        if (ab_width_8 < 5)
            max_ab_per_it_bank = 1;
    }

    KAPS_STRY(kaps_kaps_it_free(self, db, ab, size));

    return KAPS_OK;
}

kaps_status
kaps_it_mgr_destroy(
    struct it_mgr * self)
{
    if (self == NULL)
        return KAPS_INVALID_ARGUMENT;
    else
    {
        struct kaps_allocator *alloc = self->alloc;

        alloc->xfree(alloc->cookie, self);

        return KAPS_OK;
    }
}

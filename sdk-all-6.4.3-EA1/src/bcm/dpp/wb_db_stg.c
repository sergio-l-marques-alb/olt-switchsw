/*
 * $Id: wb_db_stg.c,v 1.6 Broadcom SDK $
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
 * Warmboot - Level 2 support (STG Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/stg.h>
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/stg.h>
#include <bcm_int/dpp/wb_db_stg.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/state.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

bcm_dpp_wb_stg_info_t   *_dpp_wb_stg_info_p[BCM_MAX_NUM_UNITS] = {0};

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_stg_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t    *wb_info; 
    int                      entry_size = 0, sz_bmp, size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    wb_info->version = version;

    sz_bmp = SHR_BITALLOCSIZE(SOC_DPP_STATE(unit)->stg_info->stg_max+1);
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch (version) {
        case BCM_DPP_WB_STG_VERSION_1_0:
            /* Stg State */

            wb_info->count_off = entry_size;
            entry_size += sizeof(int);

            wb_info->defl_off = entry_size;
            entry_size += sizeof(bcm_stg_t);

            wb_info->bitmap_off = entry_size;
            entry_size += sz_bmp;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->enable_off = entry_size;
            entry_size += size;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->state_h_off = entry_size;
            entry_size += size;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->state_l_off = entry_size;
            entry_size += size;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->vlan_f_off = entry_size;
            entry_size += (sz_bmp * sizeof(bcm_vlan_t));
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            
            wb_info->vlan_n_off = entry_size;
            entry_size += (BCM_VLAN_COUNT * sizeof(bcm_vlan_t));
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->size = entry_size;
            
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
_bcm_dpp_wb_stg_restore_stg_count(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    uint8                                     *data;
    int                                       *stg_count_data;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8*) (wb_info->scache_ptr + wb_info->count_off);
            stg_count_data = (int*)data;
            SOC_DPP_STATE(unit)->stg_info->stg_count = *stg_count_data;
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_restore_stg_defl(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    uint8                                    *data;
    bcm_stg_t                                *stg_defl_data;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:
            data = (uint8 *) (wb_info->scache_ptr + wb_info->defl_off);
            stg_defl_data = (bcm_stg_t*)data;
            SOC_DPP_STATE(unit)->stg_info->stg_defl = *stg_defl_data;
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_restore_stg_bitmap(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      sz_bmp;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);
    sz_bmp = SHR_BITALLOCSIZE(SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy(SOC_DPP_STATE(unit)->stg_info->stg_bitmap, (wb_info->scache_ptr + wb_info->bitmap_off), sz_bmp);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_restore_stg_enable(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);    
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy(SOC_DPP_STATE(unit)->stg_info->stg_enable, (wb_info->scache_ptr + wb_info->enable_off), size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_restore_stg_state_h(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);    
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy(SOC_DPP_STATE(unit)->stg_info->stg_state_h, (wb_info->scache_ptr + wb_info->state_h_off), size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_restore_stg_state_l(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);    
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy(SOC_DPP_STATE(unit)->stg_info->stg_state_l, (wb_info->scache_ptr + wb_info->state_l_off), size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_restore_stg_vlan_f(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      sz_bmp, size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);
    sz_bmp = SHR_BITALLOCSIZE(SOC_DPP_STATE(unit)->stg_info->stg_max+1);
    size = sizeof(bcm_vlan_t) * sz_bmp;

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy(SOC_DPP_STATE(unit)->stg_info->vlan_first, (wb_info->scache_ptr + wb_info->vlan_f_off), size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_restore_stg_vlan_n(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);
    size = sizeof(bcm_vlan_t) * BCM_VLAN_COUNT;

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy(SOC_DPP_STATE(unit)->stg_info->vlan_next, (wb_info->scache_ptr + wb_info->vlan_n_off), size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_stg_restore_stg_count(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_defl(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_bitmap(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_enable(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_state_h(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_state_l(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_vlan_f(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_restore_stg_vlan_n(unit);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_save_stg_count(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    uint8                                    *data;
    int                                      *stg_count_data;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:
            data = (uint8 *) (wb_info->scache_ptr + wb_info->count_off);
            stg_count_data = (int*) data;
            *stg_count_data = SOC_DPP_STATE(unit)->stg_info->stg_count;
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_save_stg_defl(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    uint8                                    *data;
    bcm_stg_t                                *stg_defl_data;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:
            data = (uint8 *) (wb_info->scache_ptr + wb_info->defl_off);
            stg_defl_data = (bcm_stg_t*) data;
            *stg_defl_data = SOC_DPP_STATE(unit)->stg_info->stg_defl;
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_save_stg_bitmap(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      sz_bmp;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);
    sz_bmp = SHR_BITALLOCSIZE(SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy((wb_info->scache_ptr + wb_info->bitmap_off), SOC_DPP_STATE(unit)->stg_info->stg_bitmap, sz_bmp);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_save_stg_enable(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);    
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy((wb_info->scache_ptr + wb_info->enable_off), SOC_DPP_STATE(unit)->stg_info->stg_enable, size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_save_stg_state_h(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);    
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy((wb_info->scache_ptr + wb_info->state_h_off), SOC_DPP_STATE(unit)->stg_info->stg_state_h, size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_stg_save_stg_state_l(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);    
    size = sizeof(bcm_pbmp_t) * (SOC_DPP_STATE(unit)->stg_info->stg_max+1);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy((wb_info->scache_ptr + wb_info->state_l_off), SOC_DPP_STATE(unit)->stg_info->stg_state_l, size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_save_stg_vlan_f(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                      sz_bmp, size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);
    sz_bmp = SHR_BITALLOCSIZE(SOC_DPP_STATE(unit)->stg_info->stg_max+1);
    size = sizeof(bcm_vlan_t) * sz_bmp;

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy((wb_info->scache_ptr + wb_info->vlan_f_off), SOC_DPP_STATE(unit)->stg_info->vlan_first, size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_save_stg_vlan_n(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t                    *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);
    size = sizeof(bcm_vlan_t) * BCM_VLAN_COUNT;

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            sal_memcpy((wb_info->scache_ptr + wb_info->vlan_n_off), SOC_DPP_STATE(unit)->stg_info->vlan_next, size);
            break;
            
        default:
            rc = BCM_E_INTERNAL;
            BCM_EXIT;
            break;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_stg_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_stg_info_p[unit], sizeof(bcm_dpp_wb_stg_info_t), "wb stg");
        if (_dpp_wb_stg_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Not enough memory available to allocate wb Stg")));
        }
    }

    sal_memset(_dpp_wb_stg_info_p[unit], 0x00, sizeof(bcm_dpp_wb_stg_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stg_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_stg_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_stg_info_p[unit]);
        _dpp_wb_stg_info_p[unit] = NULL;
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
_bcm_dpp_wb_stg_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_STG_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_stg_save_stg_count(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_defl(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_bitmap(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_enable(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_state_h(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_state_l(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_vlan_f(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stg_save_stg_vlan_n(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_STG_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_stg_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_STG_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_stg_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    rc = _bcm_dpp_wb_stg_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_stg_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_stg_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_stg_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_stg_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_stg_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_stg_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_stg_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_stg_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_stg_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_stg_update_count_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                             *stg_count_data;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->count_off);
            stg_count_data = (int*) data;

            /* update framework cache and persistent storage */
            *stg_count_data = SOC_DPP_STATE(unit)->stg_info->stg_count;

            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_stg_update_defl_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    bcm_stg_t                        *stg_defl_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->defl_off);
            stg_defl_data = (bcm_stg_t*)data;

            /* update framework cache and peristent storage */
            *stg_defl_data = SOC_DPP_STATE(unit)->stg_info->stg_defl;

            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(bcm_stg_t);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_stg_update_bitmap_state(int unit, int stg)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    SHR_BITDCL                      *stg_bitmap_data;

    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->bitmap_off);
            stg_bitmap_data = (SHR_BITDCL*) data;

            /* update framework cache and peristent storage                       */
            stg_bitmap_data[stg/32] = SOC_DPP_STATE(unit)->stg_info->stg_bitmap[stg/32];

            data = (uint8 *)&(stg_bitmap_data[stg/32]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_stg_update_enable_state(int unit, int stg)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    uint32                          *stg_enable_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->enable_off);
            stg_enable_data = (uint32*)data;

            /* update framework cache and peristent storage */
            stg_enable_data[stg] = SOC_PBMP_WORD_GET(*SOC_DPP_STATE(unit)->stg_info->stg_enable, stg);

            data = (uint8 *)&(stg_enable_data[stg]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_stg_update_state_h_state(int unit, int stg)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    uint32                          *stg_state_h_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    

    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not state_hd  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->state_h_off);
            stg_state_h_data = (uint32*)data;

            /* update framework cache and peristent storage                       */
            stg_state_h_data[stg] = SOC_PBMP_WORD_GET(*SOC_DPP_STATE(unit)->stg_info->stg_state_h, stg);

            data = (uint8 *)&(stg_state_h_data[stg]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_stg_update_state_l_state(int unit, int stg)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    uint32                          *stg_state_l_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not state_ld  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->state_l_off);
            stg_state_l_data = (uint32*) data;

            /* update framework cache and peristent storage                       */
            stg_state_l_data[stg] = SOC_PBMP_WORD_GET(*SOC_DPP_STATE(unit)->stg_info->stg_state_l, stg);

            data = (uint8 *)&(stg_state_l_data[stg]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(int);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_stg_update_vlan_f_state(int unit, int vlan)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_vlan_t                      *vlan_f_data;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not state_ld  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->vlan_f_off);
            vlan_f_data = (bcm_vlan_t*) data; 

            /* update framework cache and peristent storage                       */
            vlan_f_data[vlan] = SOC_DPP_STATE(unit)->stg_info->vlan_first[vlan];

            data = (uint8 *)&(vlan_f_data[vlan]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(bcm_vlan_t);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



int
_bcm_dpp_wb_stg_update_vlan_n_state(int unit, int vlan)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_stg_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_vlan_t                      *vlan_n_data;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_STG_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not state_ld  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_STG_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STG, 0);

    wb_info = BCM_DPP_WB_STG_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_STG_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->vlan_n_off);
            vlan_n_data = (bcm_vlan_t*)data;

            /* update framework cache and peristent storage                       */
            vlan_n_data[vlan] = SOC_DPP_STATE(unit)->stg_info->vlan_next[vlan];

            data = (uint8 *)&(vlan_n_data[vlan]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(bcm_vlan_t);
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid WB Version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * $Id: wb_db_l3.c,v 1.8 Broadcom SDK $
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
 * Warmboot - Level 2 support (L3 Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/l3.h>
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/wb_db_l3.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h>
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

bcm_dpp_wb_l3_info_t   *_dpp_wb_l3_info_p[BCM_MAX_NUM_UNITS] = {0};

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_l3_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t    *wb_info; 
    int                      entry_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            /* L3 State */
            wb_info->urpf_mode_off = entry_size;
            entry_size += sizeof(int);

            wb_info->egr_tnl_id_off = entry_size;
            entry_size += sizeof(int) * _BCM_GPORT_NOF_LIFS;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->vsi_usage_off = entry_size;
            entry_size += sizeof(uint8) * (((_BCM_GPORT_NOF_VSIS + 7) / 8) * 2);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->tnl_to_eep_off = entry_size;
            entry_size += sizeof(int) * _BCM_PETRA_L3_NOF_ITFS;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->l3_info_off = entry_size;
            entry_size += sizeof(bcm_dpp_l3_info_t);
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
_bcm_dpp_wb_l3_restore_urpf_mode_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
       case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(int);
            sal_memcpy(&(SOC_DPP_STATE(unit)->l3_state->urpf_mode), (wb_info->scache_ptr + wb_info->urpf_mode_off), size);
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
_bcm_dpp_wb_l3_restore_egr_tnl_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    _bcm_dpp_tunnel_bookkeeping_t            *ipt_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    ipt_info = &(SOC_DPP_STATE(unit)->l3_state->_bcm_dpp_tunnel_bk_info);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(int) * _BCM_GPORT_NOF_LIFS;
            sal_memcpy(ipt_info->egress_tunnel_id, (wb_info->scache_ptr + wb_info->egr_tnl_id_off), size);
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
_bcm_dpp_wb_l3_restore_vsi_usage_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    _bcm_dpp_tunnel_bookkeeping_t            *ipt_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    ipt_info = &(SOC_DPP_STATE(unit)->l3_state->_bcm_dpp_tunnel_bk_info);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(uint8) * (((_BCM_GPORT_NOF_VSIS + 7) / 8) *2);
            sal_memcpy(ipt_info->vsi_usage, (wb_info->scache_ptr + wb_info->vsi_usage_off), size);
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
_bcm_dpp_wb_l3_restore_intf_to_eep_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(int) * _BCM_PETRA_L3_NOF_ITFS;
            sal_memcpy(SOC_DPP_STATE(unit)->l3_state->_bcm_tunnel_intf_to_eep, 
                       (wb_info->scache_ptr + wb_info->tnl_to_eep_off), size);
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
_bcm_dpp_wb_l3_restore_info_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    bcm_dpp_l3_info_t                        *l3_info;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            l3_info = (bcm_dpp_l3_info_t *) (wb_info->scache_ptr + wb_info->l3_info_off);
            SOC_DPP_STATE(unit)->l3_state->l3_info->used_intf = l3_info->used_intf;
            SOC_DPP_STATE(unit)->l3_state->l3_info->used_vrf = l3_info->used_vrf;
            SOC_DPP_STATE(unit)->l3_state->l3_info->used_host = l3_info->used_host;
            SOC_DPP_STATE(unit)->l3_state->l3_info->used_route = l3_info->used_route;
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
_bcm_dpp_wb_l3_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_l3_restore_urpf_mode_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_restore_egr_tnl_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_restore_vsi_usage_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_restore_intf_to_eep_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_restore_info_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_l3_save_urpf_mode_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:         
            size = sizeof(int);
            sal_memcpy((wb_info->scache_ptr + wb_info->urpf_mode_off), (&(SOC_DPP_STATE(unit)->l3_state->urpf_mode)), size);
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
_bcm_dpp_wb_l3_save_egr_tnl_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    _bcm_dpp_tunnel_bookkeeping_t            *ipt_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    ipt_info = &(SOC_DPP_STATE(unit)->l3_state->_bcm_dpp_tunnel_bk_info);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(int) * _BCM_GPORT_NOF_LIFS;
            sal_memcpy((wb_info->scache_ptr + wb_info->egr_tnl_id_off), ipt_info->egress_tunnel_id, size);
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
_bcm_dpp_wb_l3_save_vsi_usage_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    _bcm_dpp_tunnel_bookkeeping_t            *ipt_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    ipt_info = &(SOC_DPP_STATE(unit)->l3_state->_bcm_dpp_tunnel_bk_info);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(uint8) * (((_BCM_GPORT_NOF_VSIS + 7) / 8) * 2);
            sal_memcpy((wb_info->scache_ptr + wb_info->vsi_usage_off), ipt_info->vsi_usage, size);
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
_bcm_dpp_wb_l3_save_intf_to_eep_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    int                                       size;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            size = sizeof(int) * _BCM_PETRA_L3_NOF_ITFS;
            sal_memcpy((wb_info->scache_ptr + wb_info->tnl_to_eep_off), 
                       SOC_DPP_STATE(unit)->l3_state->_bcm_tunnel_intf_to_eep, size);
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
_bcm_dpp_wb_l3_save_info_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t                     *wb_info;
    bcm_dpp_l3_info_t                        *l3_info;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:
            l3_info = (bcm_dpp_l3_info_t *) (wb_info->scache_ptr + wb_info->l3_info_off);
            l3_info->used_intf = SOC_DPP_STATE(unit)->l3_state->l3_info->used_intf;
            l3_info->used_vrf = SOC_DPP_STATE(unit)->l3_state->l3_info->used_vrf;
            l3_info->used_host = SOC_DPP_STATE(unit)->l3_state->l3_info->used_host;
            l3_info->used_route = SOC_DPP_STATE(unit)->l3_state->l3_info->used_route;
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
_bcm_dpp_wb_l3_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_l3_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_l3_info_p[unit], sizeof(bcm_dpp_wb_l3_info_t), "wb l3");
        if (_dpp_wb_l3_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Not enough memory available to allocate wb L3")));
        }
    }

    sal_memset(_dpp_wb_l3_info_p[unit], 0x00, sizeof(bcm_dpp_wb_l3_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_l3_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_l3_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_l3_info_p[unit]);
        _dpp_wb_l3_info_p[unit] = NULL;
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
_bcm_dpp_wb_l3_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_L3_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_l3_save_urpf_mode_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_save_egr_tnl_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_save_vsi_usage_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_save_intf_to_eep_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_l3_save_info_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_L3_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_l3_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_L3_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_l3_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_L3, 0);

    rc = _bcm_dpp_wb_l3_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_l3_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_l3_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_l3_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_l3_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_l3_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_l3_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_l3_sync(unit);
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
_bcm_dpp_wb_l3_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_l3_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_l3_update_urpf_mode(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t            *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    int                             *urpf_mode_data;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_L3_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_L3_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_L3, 0);

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->urpf_mode_off);
            urpf_mode_data = (int *)data;

            /* update framework cache and peristent storage                       */
            *urpf_mode_data = (SOC_DPP_STATE(unit)->l3_state->urpf_mode);
            data = (uint8 *)(urpf_mode_data);
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
_bcm_dpp_wb_l3_update_egr_tnl_id(int unit, int lif)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t            *wb_info; 
    uint32                           data_size;
    _bcm_dpp_tunnel_bookkeeping_t   *ipt_info;
    uint8                           *data;
    int                              offset;
    int                             *egr_tnl_data;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_L3_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_L3_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_L3, 0);

    wb_info = BCM_DPP_WB_L3_INFO(unit);
    ipt_info = &(SOC_DPP_STATE(unit)->l3_state->_bcm_dpp_tunnel_bk_info);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->egr_tnl_id_off);
            egr_tnl_data = (int *)data;

            /* update framework cache and peristent storage                       */
            egr_tnl_data[lif] = ipt_info->egress_tunnel_id[lif];
            data = (uint8 *)&(egr_tnl_data[lif]);
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
_bcm_dpp_wb_l3_update_vsi_usage(int unit, int vsi)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t            *wb_info; 
    _bcm_dpp_tunnel_bookkeeping_t   *ipt_info;
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_L3_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_L3_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_L3, 0);

    wb_info = BCM_DPP_WB_L3_INFO(unit);
    ipt_info = &(SOC_DPP_STATE(unit)->l3_state->_bcm_dpp_tunnel_bk_info);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->vsi_usage_off);

            /* update framework cache and peristent storage                       */
            data[vsi] = ipt_info->vsi_usage[vsi];
            data = (uint8 *)&(data[vsi]);
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
_bcm_dpp_wb_l3_update_tnl_to_eep(int unit, int eep_idx)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                             *eep_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_L3_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_L3_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_L3, 0);

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->tnl_to_eep_off);
            eep_data = (int*)data;

            /* update framework cache and peristent storage                       */
            eep_data[eep_idx] = SOC_DPP_STATE(unit)->l3_state->_bcm_tunnel_intf_to_eep[eep_idx];
            data = (uint8 *)&(eep_data[eep_idx]);
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
_bcm_dpp_wb_l3_update_info_state(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_l3_info_t            *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_dpp_l3_info_t               *l3_info_data;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_L3_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_L3_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_L3, 0);

    wb_info = BCM_DPP_WB_L3_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_L3_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->l3_info_off);
            l3_info_data = (bcm_dpp_l3_info_t*) data;

            /* update framework cache and peristent storage                       */
            *l3_info_data = *(SOC_DPP_STATE(unit)->l3_state->l3_info);

            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(bcm_dpp_l3_info_t);
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


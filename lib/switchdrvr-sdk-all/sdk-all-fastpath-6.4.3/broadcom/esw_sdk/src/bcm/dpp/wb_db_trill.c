/*
 * $Id: wb_db_trill.c,v 1.10 Broadcom SDK $
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
 * Warmboot - Level 2 support (QOS Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/qos.h>
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm/trill.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/wb_db_trill.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/wb_db_cosq.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/state.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

bcm_dpp_wb_trill_info_t   *_dpp_wb_trill_info_p[BCM_MAX_NUM_UNITS] = {0};

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_trill_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t    *wb_info; 
    int                      entry_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:
            /* Trill State */
            wb_info->mask_set_off = 0;
            entry_size  = sizeof(bcm_dpp_wb_trill_mask_set_t);
            if (entry_size & 0x03) entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->trill_out_ac_off = entry_size;
            entry_size += sizeof(bcm_dpp_wb_trill_out_ac_t);
            if (entry_size & 0x3) entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->last_used_id_off = entry_size;
            entry_size += sizeof(bcm_dpp_wb_trill_last_used_id_t);
            if (entry_size & 0x3) entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */

            wb_info->trill_ports_off = entry_size;
            entry_size += sizeof(bcm_dpp_wb_trill_ports_t);
            if (entry_size & 0x3) entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
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
_bcm_dpp_wb_trill_restore_trill_state(int unit)
{
    int                             rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t         *wb_info;
    bcm_dpp_trill_state_t           *trill_state;            
    bcm_dpp_wb_trill_ports_t        *trill_ports_wb_db;
    bcm_dpp_wb_trill_mask_set_t     *trill_mask_set_wb_db;
    bcm_dpp_wb_trill_out_ac_t       *trill_out_ac_wb_db;
    bcm_dpp_wb_trill_last_used_id_t *trill_last_used_id_wb_db;
    int                              i;
    bcm_gport_t                      gport;
    int                              port_cnt;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    trill_state = SOC_DPP_STATE(unit)->trill_state;

    switch(wb_info->version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:
            trill_out_ac_wb_db = (bcm_dpp_wb_trill_out_ac_t*)(wb_info->scache_ptr + wb_info->trill_out_ac_off);
            trill_state->trill_out_ac = _bcm_dpp_wb_load32((uint8 *)trill_out_ac_wb_db);

            trill_mask_set_wb_db = (bcm_dpp_wb_trill_mask_set_t*)(wb_info->scache_ptr + wb_info->mask_set_off);
            trill_state->mask_set = _bcm_dpp_wb_load32((uint8 *)trill_mask_set_wb_db);
  
            trill_last_used_id_wb_db = (bcm_dpp_wb_trill_last_used_id_t*)(wb_info->scache_ptr + wb_info->last_used_id_off);
            trill_state->last_used_id = _bcm_dpp_wb_load32((uint8 *)trill_last_used_id_wb_db);
                     
            trill_ports_wb_db = (bcm_dpp_wb_trill_ports_t*)(wb_info->scache_ptr + wb_info->trill_ports_off);
            port_cnt = _bcm_dpp_wb_load32((uint8 *)&trill_ports_wb_db->port_cnt);
            /* set default value for allocated_cnt, its value will be updated if necessary when adding the gport to the list  */
            trill_state->trill_ports->allocated_cnt = _BCM_TRILL_PORTS_ALLOC_SIZE; 

            if (port_cnt > 0) {
                for (i=0; i<port_cnt; i++) {
                    gport = _bcm_dpp_wb_load32((uint8 *)&trill_ports_wb_db->ports[i]);                   
                    BCMDNX_IF_ERR_EXIT(_bcm_petra_add_to_trill_port_list(unit, gport));
                }
            }
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
_bcm_dpp_wb_trill_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_trill_restore_trill_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_wb_trill_save_trill_state(int unit)
{
    int                             rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t         *wb_info;
    bcm_dpp_trill_state_t           *trill_state;            
    bcm_dpp_wb_trill_ports_t        *trill_ports_wb_db;
    bcm_dpp_wb_trill_mask_set_t     *trill_mask_set_wb_db;
    bcm_dpp_wb_trill_out_ac_t       *trill_out_ac_wb_db;
    bcm_dpp_wb_trill_last_used_id_t *trill_last_used_id_wb_db;

    int                              i;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    trill_state = SOC_DPP_STATE(unit)->trill_state;

    switch(wb_info->version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:
 
            trill_out_ac_wb_db = (bcm_dpp_wb_trill_out_ac_t*)(wb_info->scache_ptr + wb_info->trill_out_ac_off);
            _bcm_dpp_wb_store32(trill_state->trill_out_ac,(uint8 *)trill_out_ac_wb_db);
            
            trill_mask_set_wb_db = (bcm_dpp_wb_trill_mask_set_t*)(wb_info->scache_ptr + wb_info->mask_set_off);
            _bcm_dpp_wb_store32(trill_state->mask_set, (uint8 *)trill_mask_set_wb_db);
            
            trill_last_used_id_wb_db = (bcm_dpp_wb_trill_last_used_id_t*)(wb_info->scache_ptr + wb_info->last_used_id_off);
            _bcm_dpp_wb_store32(trill_state->last_used_id, (uint8 *)trill_last_used_id_wb_db);
                        
            trill_ports_wb_db = (bcm_dpp_wb_trill_ports_t*)(wb_info->scache_ptr + wb_info->trill_ports_off);
            _bcm_dpp_wb_store32(trill_state->trill_ports->port_cnt, (uint8 *)&trill_ports_wb_db->port_cnt);
            
            if (trill_state->trill_ports->port_cnt > 0) {
                for (i=0; i<trill_state->trill_ports->port_cnt; i++) {
                    _bcm_dpp_wb_store32(trill_state->trill_ports->ports[i], (uint8 *)&trill_ports_wb_db->ports[i]);
                }
            }
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
_bcm_dpp_wb_trill_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_trill_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_trill_info_p[unit], sizeof(bcm_dpp_wb_trill_info_t), "wb trill");
        if (_dpp_wb_trill_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Not enough memory available to allocate wb Trill")));
        }
    }

    sal_memset(_dpp_wb_trill_info_p[unit], 0x00, sizeof(bcm_dpp_wb_trill_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_trill_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_trill_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_trill_info_p[unit]);
        _dpp_wb_trill_info_p[unit] = NULL;
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
_bcm_dpp_wb_trill_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    if (!(BCM_DPP_WB_TRILL_INFO_INIT(unit))) {
        BCM_EXIT;
    }
    /* check if there was any state change or a part of init sequence */
    if (!(BCM_DPP_WB_TRILL_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_trill_save_trill_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_TRILL_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_trill_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_TRILL_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_trill_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_TRILL, 0);

    rc = _bcm_dpp_wb_trill_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_trill_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_trill_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_trill_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
            rc = _bcm_dpp_wb_trill_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_trill_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
            rc = _bcm_dpp_wb_trill_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
            rc = _bcm_dpp_wb_trill_sync(unit);
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
_bcm_dpp_wb_trill_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_trill_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_trill_update_mask_set_state(int unit, int trill_mask_set)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_dpp_wb_trill_mask_set_t      *trill_mask_set_wb_db;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_TRILL_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_TRILL, 0);

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:
            /* update framework cache and persistent storage  */
            trill_mask_set_wb_db = (bcm_dpp_wb_trill_mask_set_t*)(wb_info->scache_ptr + wb_info->mask_set_off);
            _bcm_dpp_wb_store32(trill_mask_set, (uint8*)trill_mask_set_wb_db);
            data_size = sizeof(bcm_dpp_wb_trill_mask_set_t);
            data = (uint8*) trill_mask_set_wb_db;
            offset = (uint32)(data - wb_info->scache_ptr);
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
_bcm_dpp_wb_trill_update_out_ac_state(int unit, SOC_PPD_AC_ID trill_out_ac)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_dpp_wb_trill_out_ac_t       *trill_out_ac_wb_db;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_TRILL_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_TRILL, 0);

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:

            trill_out_ac_wb_db = (bcm_dpp_wb_trill_out_ac_t*)(wb_info->scache_ptr + wb_info->trill_out_ac_off);
            _bcm_dpp_wb_store32(trill_out_ac, (uint8*)trill_out_ac_wb_db);
            data_size = sizeof(bcm_dpp_wb_trill_out_ac_t);
            data = (uint8*) trill_out_ac_wb_db;
            offset = (uint32)(data - wb_info->scache_ptr);
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
_bcm_dpp_wb_trill_update_ports_state(int unit, _bcm_petra_trill_port_list_t *trill_ports)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t         *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_dpp_wb_trill_ports_t        *trill_ports_wb_db;
    int                              i;

    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_TRILL_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_TRILL, 0);

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:

            trill_ports_wb_db = (bcm_dpp_wb_trill_ports_t*)(wb_info->scache_ptr + wb_info->trill_ports_off);
            _bcm_dpp_wb_store32(trill_ports->port_cnt, (uint8 *)&trill_ports_wb_db->port_cnt);

            if (trill_ports->port_cnt > 0) {
                for (i=0; i< trill_ports->port_cnt; i++) {
                    _bcm_dpp_wb_store32(trill_ports->ports[i], (uint8 *)&trill_ports_wb_db->ports[i]);
                }
            }

            data_size = sizeof(bcm_dpp_wb_trill_ports_t);
            data = (uint8*) trill_ports_wb_db;
            offset = (uint32)(data - wb_info->scache_ptr);


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
_bcm_dpp_wb_trill_update_last_used_id_state(int unit, int last_used_id)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_trill_info_t           *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    bcm_dpp_wb_trill_last_used_id_t      *trill_last_used_id_wb_db;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_TRILL_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_TRILL_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_TRILL, 0);

    wb_info = BCM_DPP_WB_TRILL_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_TRILL_VERSION_1_0:
            /* update framework cache and persistent storage  */
            trill_last_used_id_wb_db = (bcm_dpp_wb_trill_last_used_id_t*)(wb_info->scache_ptr + wb_info->last_used_id_off);
            _bcm_dpp_wb_store32(last_used_id, (uint8*)trill_last_used_id_wb_db);
            data_size = sizeof(bcm_dpp_wb_trill_last_used_id_t);
            data = (uint8*) trill_last_used_id_wb_db;
            offset = (uint32)(data - wb_info->scache_ptr);
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


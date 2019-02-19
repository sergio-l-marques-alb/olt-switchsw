/*
 * $Id: wb_db_mirror.c,v 1.10 Broadcom SDK $
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
 * Warmboot - Level 2 support (MIRROR Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/mirror.h>
#include <bcm/fabric.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/wb_db_mirror.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/dpp/sw_db.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>


#if defined(BCM_WARM_BOOT_SUPPORT)

bcm_dpp_wb_mirror_info_t   *_dpp_wb_mirror_info_p[BCM_MAX_NUM_UNITS] = {0};

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_mirror_layout_init(int unit, int version)
{
    int                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t *wb_info; 
    int                       entry_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:
            /* Mirror State */
            wb_info->refCount_off = entry_size;
            entry_size  = sizeof(int) * SOC_DPP_STATE(unit)->mirror_state->ingressCount;
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
#ifdef BCM_PETRAB_SUPPORT
            if (SOC_IS_PETRAB(unit))
            {
                wb_info->mirror_off = entry_size;
                entry_size += sizeof(bcm_dpp_mirror_sw_db_t);
                entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            }
#endif
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
_bcm_dpp_wb_mirror_restore_refCount_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t                 *wb_info;
    uint32                                   *data;
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (uint32 *) (wb_info->scache_ptr + wb_info->refCount_off);
            for (i=0; i < SOC_DPP_STATE(unit)->mirror_state->ingressCount; i++) {
                SOC_DPP_STATE(unit)->mirror_state->ingress[i].refCount = data[i];
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

#ifdef BCM_PETRAB_SUPPORT
STATIC int
_bcm_dpp_wb_mirror_restore_mirror_mode(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t                 *wb_info;
    bcm_dpp_mirror_sw_db_t                   *data;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_PETRAB(unit)) {
        BCM_EXIT;
    }    

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (bcm_dpp_mirror_sw_db_t *) (wb_info->scache_ptr + wb_info->mirror_off);
            rc = _bcm_sw_db_petra_mirror_mode_set(unit, data->mirror_mode);
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
_bcm_dpp_wb_mirror_restore_port_profile(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t                 *wb_info;
    bcm_dpp_mirror_sw_db_t                   *data;
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_PETRAB(unit)) {
        BCM_EXIT;
    } 

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (bcm_dpp_mirror_sw_db_t *) (wb_info->scache_ptr + wb_info->mirror_off);
            for (i = 0; i < SOC_PB_PP_MAX_NOF_LOCAL_PORTS; i++) {   
                rc = _bcm_sw_db_outbound_mirror_port_profile_set(unit, i, data->egress_port_profile[i]);
                BCMDNX_IF_ERR_EXIT(rc);
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
#endif /* BCM_PETRAB_SUPPORT */


STATIC int
_bcm_dpp_wb_mirror_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_mirror_restore_refCount_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit))
    {
        rc = _bcm_dpp_wb_mirror_restore_mirror_mode(unit);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = _bcm_dpp_wb_mirror_restore_port_profile(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }
#endif

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_dpp_wb_mirror_save_refCount_state(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t                 *wb_info;
    uint32                                   *data;
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (uint32 *) (wb_info->scache_ptr + wb_info->refCount_off);
            for (i=0; i < SOC_DPP_STATE(unit)->mirror_state->ingressCount; i++) {
                data[i] = SOC_DPP_STATE(unit)->mirror_state->ingress[i].refCount;
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


#ifdef BCM_PETRAB_SUPPORT
STATIC int
_bcm_dpp_wb_mirror_save_mirror_mode(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t                 *wb_info;
    bcm_dpp_mirror_sw_db_t                   *data;
    int                                       mirror_mode;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_PETRAB(unit)) {
        BCM_EXIT;
    }

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (bcm_dpp_mirror_sw_db_t *) (wb_info->scache_ptr + wb_info->mirror_off);
            rc = _bcm_sw_db_petra_mirror_mode_get(unit, &mirror_mode);
            BCMDNX_IF_ERR_EXIT(rc);
            data->mirror_mode = mirror_mode;
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
_bcm_dpp_wb_mirror_save_port_profile(int unit)
{
    int                                       rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t                 *wb_info;
    bcm_dpp_mirror_sw_db_t                   *data;
    uint8                                     port_profile;
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_PETRAB(unit)) {
        BCM_EXIT;
    }

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (bcm_dpp_mirror_sw_db_t *) (wb_info->scache_ptr + wb_info->mirror_off);
            for (i=0; i < SOC_PB_PP_MAX_NOF_LOCAL_PORTS; i++) {
                rc = _bcm_sw_db_outbound_mirror_port_profile_get(unit, i, &port_profile);
                BCMDNX_IF_ERR_EXIT(rc);
                data->egress_port_profile[i] = port_profile;
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
#endif /* BCM_PETRAB_SUPPORT */



STATIC int
_bcm_dpp_wb_mirror_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_mirror_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_mirror_info_p[unit], sizeof(bcm_dpp_wb_mirror_info_t), "wb mirror");
        if (_dpp_wb_mirror_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Not enough memory available to allocate wb Mirror")));
        }
    }

    sal_memset(_dpp_wb_mirror_info_p[unit], 0x00, sizeof(bcm_dpp_wb_mirror_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_mirror_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_mirror_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_mirror_info_p[unit]);
        _dpp_wb_mirror_info_p[unit] = NULL;
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
_bcm_dpp_wb_mirror_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t   *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_MIRROR_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_mirror_save_refCount_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);    

#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit))
    {
        rc = _bcm_dpp_wb_mirror_save_mirror_mode(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        rc = _bcm_dpp_wb_mirror_save_port_profile(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }
#endif

    BCM_DPP_WB_MIRROR_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_mirror_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size, free, ppdr;
    uint16                   version = BCM_DPP_WB_MIRROR_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_mirror_info_t   *wb_info; 
    _bcm_petra_mirror_unit_data_t *unitData;
    SOC_PB_ACTION_CMD_MIRROR_INFO mirrorInfo;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MIRROR, 0);

    rc = _bcm_dpp_wb_mirror_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_mirror_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_mirror_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_mirror_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_mirror_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }

        /* Retrieve the rest of the state from H/W */
        unitData = SOC_DPP_STATE(unit)->mirror_state;
        for (free=0; free < unitData->ingressCount; free++) {
            /* Skip the first entry as reserved */
            if ((free == 0) || (unitData->ingress[free].refCount == 0)) {
                continue;
            }
            SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&mirrorInfo);
            ppdr = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_get,(unit, free, &mirrorInfo));            
            BCMDNX_IF_ERR_EXIT(handle_sand_result(ppdr));
            sal_memcpy(&(unitData->ingress[free].mirrorInfo),
                       &mirrorInfo,
                       sizeof(unitData->ingress[free].mirrorInfo));
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_mirror_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_mirror_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_mirror_sync(unit);
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
_bcm_dpp_wb_mirror_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_mirror_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_mirror_update_refCount_state(int unit, int index)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t        *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    uint32                          *refCount_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    BCM_DPP_UNIT_CHECK(unit);
    if (!(BCM_DPP_WB_MIRROR_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_MIRROR_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MIRROR, 0);

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->refCount_off);
            refCount_data = (uint32 *)data;
            
            /* update framework cache and peristent storage                       */
            refCount_data[index] = SOC_DPP_STATE(unit)->mirror_state->ingress[index].refCount;

            data = (uint8 *)&(refCount_data[index]);
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

#ifdef BCM_PETRAB_SUPPORT
int
_bcm_dpp_wb_mirror_update_mirror_mode(int unit)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t        *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    bcm_dpp_mirror_sw_db_t          *mirror_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    int                              mirror_mode;


    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_PETRAB(unit)) {
        BCM_EXIT;
    }
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_MIRROR_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_MIRROR_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MIRROR, 0);

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->mirror_off);
            mirror_data = (bcm_dpp_mirror_sw_db_t *)data;
            
            /* update framework cache and peristent storage                       */
            rc = _bcm_sw_db_petra_mirror_mode_get(unit, &mirror_mode);
            mirror_data->mirror_mode = mirror_mode;

            data = (uint8 *)&(mirror_data->mirror_mode);
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
_bcm_dpp_wb_mirror_update_port_profile(int unit, int port)
{
    int                              rc = BCM_E_NONE;
    bcm_dpp_wb_mirror_info_t        *wb_info; 
    uint32                           data_size;
    uint8                           *data;
    bcm_dpp_mirror_sw_db_t          *mirror_data;
    int                              offset;
    soc_scache_handle_t              wb_handle;
    uint8                            port_profile;


    BCMDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_PETRAB(unit)) {
        BCM_EXIT;
    }
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from cosq_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_MIRROR_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_MIRROR_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MIRROR, 0);

    wb_info = BCM_DPP_WB_MIRROR_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MIRROR_VERSION_1_0:

            data = (uint8 *)(wb_info->scache_ptr + wb_info->mirror_off);
            mirror_data = (bcm_dpp_mirror_sw_db_t *)data;
            
            /* update framework cache and peristent storage                       */
            rc = _bcm_sw_db_outbound_mirror_port_profile_get(unit, port, &port_profile);
            mirror_data->egress_port_profile[port] = port_profile;

            data = (uint8 *)&(mirror_data->egress_port_profile[port]);
            offset = (uint32)(data - wb_info->scache_ptr);
            data_size = sizeof(uint8);
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
#endif /*# BCM_PETRAB_SUPPORT */

#endif /* BCM_WARM_BOOT_SUPPORT */


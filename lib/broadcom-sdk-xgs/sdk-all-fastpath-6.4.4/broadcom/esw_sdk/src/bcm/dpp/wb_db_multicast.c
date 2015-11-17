/*
 * $Id$
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
 * Warmboot - Level 2 support (MULTICAST Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm/multicast.h>
#include <soc/error.h>
#include <soc/types.h>
#include <soc/dpp/drv.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/wb_db_multicast.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/soc_sw_db.h>
#include <shared/shr_resmgr.h>
#include <shared/shr_template.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/error.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#include <soc/dpp/mbcm.h>

#if defined(BCM_WARM_BOOT_SUPPORT)

bcm_dpp_wb_multicast_info_t   *_dpp_wb_multicast_info_p[BCM_MAX_NUM_UNITS] = {0};
 

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_multicast_layout_init(int unit, int version)
{
    int                          rc = BCM_E_NONE;
    bcm_dpp_wb_multicast_info_t  *wb_info; 
    int                          entry_size, total_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_MULTICAST_VERSION_1_0:
            entry_size = sizeof(bcm_dpp_wb_multicast_ingress_mc_config_t) * BCM_DPP_WB_MULTICAST_MAX_MC_GROUPS_INGREES(unit);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_ingress_mc = BCM_DPP_WB_MULTICAST_MAX_MC_GROUPS_INGREES(unit);
            wb_info->ingress_mc_off = 0;
            total_size += entry_size;

            entry_size = sizeof(bcm_dpp_wb_multicast_egress_mc_config_t) * BCM_DPP_WB_MULTICAST_MAX_MC_GROUPS_EGREES(unit);
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */
            wb_info->max_egress_mc = BCM_DPP_WB_MULTICAST_MAX_MC_GROUPS_EGREES(unit);
            wb_info->egress_mc_off = total_size;
            total_size += entry_size;

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
_bcm_dpp_wb_multicast_restore_ingress_mc_state(int unit)
{
    int                                   rv = BCM_E_NONE;
    int                                   rc = BCM_E_NONE;
    uint32                                mc_id;
    bcm_dpp_wb_multicast_info_t          *wb_info; 
    bcm_dpp_wb_multicast_ingress_mc_config_t    *ingress_mc_state;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MULTICAST_VERSION_1_0:

            ingress_mc_state = (bcm_dpp_wb_multicast_ingress_mc_config_t *)(wb_info->scache_ptr + wb_info->ingress_mc_off);

            /* read all mc_id entries */
            for (mc_id = 0; mc_id < wb_info->max_ingress_mc; mc_id++) {

                if ((ingress_mc_state + mc_id)->is_alloc == FALSE) {
                    continue;
                }

                rv = bcm_dpp_am_mc_alloc(unit, SHR_RES_ALLOC_WITH_ID, &mc_id, FALSE);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Warmboot: Trying to restore data from an unsupported version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_multicast_restore_egress_mc_state(int unit)
{
    int                                   rv = BCM_E_NONE;
    int                                   rc = BCM_E_NONE;
    uint32                                mc_id;
    bcm_dpp_wb_multicast_info_t          *wb_info; 
    bcm_dpp_wb_multicast_egress_mc_config_t    *egress_mc_state;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MULTICAST_VERSION_1_0:

            egress_mc_state = (bcm_dpp_wb_multicast_egress_mc_config_t *)(wb_info->scache_ptr + wb_info->egress_mc_off);

            /* read all mc_id entries */
            for (mc_id = 0; mc_id < wb_info->max_egress_mc; mc_id++) {

                if ((egress_mc_state + mc_id)->is_alloc == FALSE) {
                    continue;
                }

                rv = bcm_dpp_am_mc_alloc(unit, SHR_RES_ALLOC_WITH_ID, &mc_id, TRUE);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Warmboot: Trying to restore data from an unsupported version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_multicast_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_multicast_restore_ingress_mc_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_multicast_restore_egress_mc_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
STATIC int
_bcm_dpp_wb_multicast_save_ingress_mc_state(int unit)
{
    int                                          res, rc = BCM_E_NONE;
    uint32                                       mc_id;
    bcm_dpp_wb_multicast_info_t                 *wb_info; 
    bcm_dpp_wb_multicast_ingress_mc_config_t    *ingress_mc_state;


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MULTICAST_VERSION_1_0:

            ingress_mc_state = (bcm_dpp_wb_multicast_ingress_mc_config_t *)(wb_info->scache_ptr + wb_info->ingress_mc_off);

            /* read all mc id entries */
            for (mc_id = 0; mc_id < wb_info->max_ingress_mc; mc_id++) {
                res = bcm_dpp_am_mc_is_alloced(unit, mc_id, FALSE);
                if (res == BCM_E_NOT_FOUND) {
                    (ingress_mc_state + mc_id)->is_alloc = FALSE;
                } else if ( res == BCM_E_EXISTS) {
                    (ingress_mc_state + mc_id)->is_alloc = TRUE;
                } else {
                    BCMDNX_IF_ERR_EXIT(res == BCM_E_NONE ? BCM_E_INTERNAL : res);
                }
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Warmboot: Trying to save data of an unsupported version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_multicast_save_egress_mc_state(int unit)
{
    int                                          res, rc = BCM_E_NONE;
    uint32                                       mc_id;
    bcm_dpp_wb_multicast_info_t                 *wb_info; 
    bcm_dpp_wb_multicast_egress_mc_config_t    *egress_mc_state;


    BCMDNX_INIT_FUNC_DEFS;
    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    switch(wb_info->version) {
        case BCM_DPP_WB_MULTICAST_VERSION_1_0:

            egress_mc_state = (bcm_dpp_wb_multicast_egress_mc_config_t *)(wb_info->scache_ptr + wb_info->egress_mc_off);

            /* read all mc id entries */
            for (mc_id = 0; mc_id < wb_info->max_egress_mc; mc_id++) {
                res = bcm_dpp_am_mc_is_alloced(unit, mc_id, TRUE);
                if (res == BCM_E_NOT_FOUND) {
                    (egress_mc_state + mc_id)->is_alloc = FALSE;
                } else if ( res == BCM_E_EXISTS) {
                    (egress_mc_state + mc_id)->is_alloc = TRUE;
                } else {
                    BCMDNX_IF_ERR_EXIT(res == BCM_E_NONE ? BCM_E_INTERNAL : res);
                }
            }

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Warmboot: Trying to save data of an unsupported version")));
            break;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* MCAST_WARM_BOOT_UPDATE_ENABLED */

STATIC int
_bcm_dpp_wb_multicast_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_multicast_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_multicast_info_p[unit], sizeof(bcm_dpp_wb_multicast_info_t), "wb multicast");
        if (_dpp_wb_multicast_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
    }

    sal_memset(_dpp_wb_multicast_info_p[unit], 0x00, sizeof(bcm_dpp_wb_multicast_info_t));

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_multicast_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    if (_dpp_wb_multicast_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_multicast_info_p[unit]);
        _dpp_wb_multicast_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 *
 *  save functionality
 */

int
_bcm_dpp_wb_multicast_sync(int unit)
{
#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
    int                          rc = BCM_E_NONE;
    bcm_dpp_wb_multicast_info_t  *wb_info; 
#endif

    BCMDNX_INIT_FUNC_DEFS;

#ifdef MCAST_WARM_BOOT_UPDATE_ENABLED
    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if ( !(BCM_DPP_WB_MULTICAST_IS_DIRTY(unit)) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_multicast_save_ingress_mc_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_multicast_save_egress_mc_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_MULTICAST_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);
#else
    /* No sync functionality for multicast, handled in gport_mgmt.c */
    BCM_EXIT;
#endif

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_multicast_state_init(int unit)
{
    int                          rc = BCM_E_NONE;
    soc_scache_handle_t          wb_handle;
    int                          flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                       size;
    uint16                       version = BCM_DPP_WB_MULTICAST_CURRENT_VERSION, recovered_ver;
    uint8                        *scache_ptr;
    bcm_dpp_wb_multicast_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MULTICAST, 0);

    rc = _bcm_dpp_wb_multicast_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_multicast_layout_init(unit, recovered_ver);
        if (rc != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(rc);
        }

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_multicast_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_multicast_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_multicast_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_multicast_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        if (already_exists == TRUE) {
            /* state should have been removed by Host. Stale State */
            rc = _bcm_dpp_wb_multicast_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
        else {
            /* Initialize the state to know value */
            rc = _bcm_dpp_wb_multicast_sync(unit);
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
_bcm_dpp_wb_multicast_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_wb_multicast_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_multicast_update_ingress_mc_state(int unit, uint32 mc_id, uint8 is_alloc)
{
    int                                         rc = BCM_E_NONE;
    bcm_dpp_wb_multicast_info_t                *wb_info;
    bcm_dpp_wb_multicast_ingress_mc_config_t   *ingress_mc_state;
    uint32                                      data_size;
    uint8                                      *data;
    int                                         offset;
    soc_scache_handle_t                         wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from multicast_init().                                                 */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    /* or if ingress multicast disabled .                                           */
    if (!(BCM_DPP_WB_MULTICAST_INFO_INIT(unit))) 
    {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_MULTICAST_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MULTICAST, 0);

    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    switch(wb_info->version) {
    case BCM_DPP_WB_MULTICAST_VERSION_1_0:

            data_size = sizeof(bcm_dpp_wb_multicast_ingress_mc_config_t);
            ingress_mc_state = (bcm_dpp_wb_multicast_ingress_mc_config_t *)(wb_info->scache_ptr + wb_info->ingress_mc_off);
            ingress_mc_state += mc_id;
            ingress_mc_state->is_alloc = is_alloc;

            data_size = sizeof(bcm_dpp_wb_multicast_ingress_mc_config_t);
            data = (uint8 *)ingress_mc_state;
            offset = (uint32)(data - wb_info->scache_ptr);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("WarmBoot: Trying to update data of an unsupported version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_wb_multicast_update_egress_mc_state(int unit, uint32 mc_id, uint8 is_alloc)
{
    int                                         rc = BCM_E_NONE;
    bcm_dpp_wb_multicast_info_t                *wb_info;
    bcm_dpp_wb_multicast_egress_mc_config_t    *egress_mc_state;
    uint32                                      data_size;
    uint8                                      *data;
    int                                         offset;
    soc_scache_handle_t                         wb_handle;


    BCMDNX_INIT_FUNC_DEFS;
    /* handle corner case of being invoked before the wb module is initialized      */
    /* (e.g. from multicast_init().                                                      */
    /* default configuration either does not have to be saved or has to be handled  */
    /* in a special manner.                                                         */
    if (!(BCM_DPP_WB_MULTICAST_INFO_INIT(unit))) {
        BCM_EXIT;
    }

    /* check if immediate sync not enabled  */
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {
        /* mark dirty state. Currently no other processing done           */
        /* if required framework cache could be updated without updating  */
        /* the persistent storage                                         */
        BCM_DPP_WB_DEV_MULTICAST_DIRTY_BIT_SET(unit);
        BCM_EXIT;
    }

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_MULTICAST, 0);

    wb_info = BCM_DPP_WB_MULTICAST_INFO(unit);

    switch(wb_info->version) {
    case BCM_DPP_WB_MULTICAST_VERSION_1_0:

            data_size = sizeof(bcm_dpp_wb_multicast_egress_mc_config_t);
            egress_mc_state = (bcm_dpp_wb_multicast_egress_mc_config_t *)(wb_info->scache_ptr + wb_info->egress_mc_off);
            egress_mc_state += mc_id;
            egress_mc_state->is_alloc = is_alloc;

            data_size = sizeof(bcm_dpp_wb_multicast_egress_mc_config_t);
            data = (uint8 *)egress_mc_state;
            offset = (uint32)(data - wb_info->scache_ptr);

            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("WarmBoot: Trying to update data of an unsupported version")));
            break;
    }

    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Routines to make code Independent of Host processor alignment constaints
 */  
#endif /* BCM_WARM_BOOT_SUPPORT */


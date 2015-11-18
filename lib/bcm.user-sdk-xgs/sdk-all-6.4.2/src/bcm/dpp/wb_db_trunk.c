/*
 * $Id: wb_db_trunk.c,v 1.7 Broadcom SDK $
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
 * Warmboot - Level 2 support (TRUNK Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/dpp/wb_db_trunk.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/common/debug.h>
#include <soc/dpp/soc_sw_db.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * runtime information
 */
typedef struct bcm_dpp_wb_trunk_info_s {
    int                      init_done;
    trunk_cntl_t            *trunk_config;

    int                      is_dirty;

    uint16                   version;
    uint8                   *scache_ptr;
    int                      size;

    uint32                   trunk_id_num;
    uint32                   trunk_id_off;

    uint32                   in_use_num;
    uint32                   in_use_off;

} bcm_dpp_wb_trunk_info_t;

static bcm_dpp_wb_trunk_info_t   *_dpp_wb_trunk_info_p[BCM_MAX_NUM_UNITS];

#define BCM_DPP_WB_TRUNK_INFO(unit)             (_dpp_wb_trunk_info_p[unit])
#define BCM_DPP_WB_TRUNK_INFO_INIT(unit)        ( (_dpp_wb_trunk_info_p[unit] != NULL) &&          \
                                                 (_dpp_wb_trunk_info_p[unit]->init_done == TRUE) )
#endif /* BCM_WARM_BOOT_SUPPORT */

trunk_wb_cntl_t bcm_wb_trunk_control[BCM_MAX_NUM_UNITS];


#define BCM_DPP_WB_TRUNK_SET_AND_UPDATE_ARR(_dest, _var, _var_name, arr_indx1)         \
    BCM_DPP_WB_SET_AND_UPDATE_ARR(_dest, _var, TRUNK , trunk, _var_name, arr_indx1, 0x0);



#define BCM_DPP_WB_TRUNK_GET(_dest, _var)        \
    BCM_DPP_WB_GET(_dest, _var, 0x0)

int
_bcm_dpp_wb_trunk_control_allocate(int unit, int ngroups)
{
    int alloc_size;
    BCMDNX_INIT_FUNC_DEFS;

    alloc_size = ngroups * sizeof(bcm_dpp_trunk_private_t);
    BCMDNX_ALLOC(bcm_wb_trunk_control[unit].t_info, alloc_size, "Trunk-private");
    if (bcm_wb_trunk_control[unit].t_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate Trunk-private"), unit));
    }    
    sal_memset(bcm_wb_trunk_control[unit].t_info, 0, alloc_size);
       
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_trunk_control_free(int unit)
{
    if (bcm_wb_trunk_control[unit].t_info) {
        BCM_FREE(bcm_wb_trunk_control[unit].t_info);
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_trunk_trunk_id_get(int unit, bcm_trunk_t *trunk_id, uint32 indx1)
{
    BCM_DPP_WB_TRUNK_GET(bcm_wb_trunk_control[unit].t_info[indx1].trunk_id, trunk_id);
}

int
_bcm_dpp_wb_trunk_trunk_id_set(int unit, bcm_trunk_t trunk_id, uint32 indx1)
{
    BCM_DPP_WB_TRUNK_SET_AND_UPDATE_ARR(
        bcm_wb_trunk_control[unit].t_info[indx1].trunk_id, trunk_id, trunk_id, indx1);
}

int
_bcm_dpp_wb_trunk_in_use_get(int unit, int *in_use, uint32 indx1)
{
    BCM_DPP_WB_TRUNK_GET(bcm_wb_trunk_control[unit].t_info[indx1].in_use, in_use);
}

int
_bcm_dpp_wb_trunk_in_use_set(int unit, int in_use, uint32 indx1)
{
    BCM_DPP_WB_TRUNK_SET_AND_UPDATE_ARR(
        bcm_wb_trunk_control[unit].t_info[indx1].in_use, in_use, in_use, indx1);
}





#if defined(BCM_WARM_BOOT_SUPPORT)


#define BCM_DPP_WB_TRUNK_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_TRUNK_CURRENT_VERSION        BCM_DPP_WB_TRUNK_VERSION_1_0


/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */
#define BCM_DPP_WB_TRUNK_TRUNK_ID_NUM        \
    (BCM_DPP_MAX_TRUNKS)

#define BCM_DPP_WB_TRUNK_IN_USE_NUM        \
    (BCM_DPP_MAX_TRUNKS)







typedef struct bcm_dpp_wb_trunk_trunk_id_config_s {
    int8     trunk_id;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_trunk_trunk_id_config_t;


typedef struct bcm_dpp_wb_trunk_in_use_config_s {
    uint8    in_use;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_trunk_in_use_config_t;



#define BCM_DPP_WB_DEV_TRUNK_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_TRUNK_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_TRUNK_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_TRUNK_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_TRUNK_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_TRUNK_INFO(unit)->is_dirty == 1)






 
#define BCM_DPP_WB_TRUNK_UPDATE_ARR_STATE(_var, _sw_db_var, _indx1)     \
    BCM_DPP_WB_UPDATE_ARR_STATE(TRUNK, trunk, _var, _sw_db_var, _indx1)

 



#define BCM_DPP_WB_TRUNK_RESTORE_ARR_STATE(_var, _sw_db_var)        \
    BCM_DPP_WB_RESTORE_ARR_STATE(TRUNK, trunk, _var, _sw_db_var)





#define BCM_DPP_WB_TRUNK_SAVE_ARR_STATE(_var, _sw_db_var)       \
    BCM_DPP_WB_SAVE_ARR_STATE(TRUNK, trunk, _var, _sw_db_var)




#define BCM_DPP_WB_DB_LAYOUT_INIT_TRUNK_VARIABLE(_var, _var_num)        \
    BCM_DPP_WB_DB_LAYOUT_INIT_VARIABLE(trunk, _var, _var_num)

/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_trunk_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_trunk_info_t  *wb_info; 
    int                      entry_size, total_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_TRUNK_INFO(unit);

    wb_info->version = version;

    switch (version) {

        case BCM_DPP_WB_TRUNK_VERSION_1_0:


            BCM_DPP_WB_DB_LAYOUT_INIT_TRUNK_VARIABLE(trunk_id, 
                                                     BCM_DPP_WB_TRUNK_TRUNK_ID_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_TRUNK_VARIABLE(in_use, 
                                                     BCM_DPP_WB_TRUNK_IN_USE_NUM); 

        
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
_bcm_dpp_wb_trunk_restore_trunk_id_state(int unit)
{
    BCM_DPP_WB_TRUNK_RESTORE_ARR_STATE(trunk_id, 
                                       t_info[arr_indx].trunk_id);
}

STATIC int
_bcm_dpp_wb_trunk_restore_in_use_state(int unit)
{
    BCM_DPP_WB_TRUNK_RESTORE_ARR_STATE(in_use, 
                                       t_info[arr_indx].in_use);
}


STATIC int
_bcm_dpp_wb_trunk_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_trunk_restore_trunk_id_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_trunk_restore_in_use_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_dpp_wb_trunk_save_trunk_id_state(int unit)
{
    BCM_DPP_WB_TRUNK_SAVE_ARR_STATE(trunk_id, 
                                    t_info[arr_indx].trunk_id);
}


STATIC int
_bcm_dpp_wb_trunk_save_in_use_state(int unit)
{
    BCM_DPP_WB_TRUNK_SAVE_ARR_STATE(in_use, 
                                    t_info[arr_indx].in_use);
}



STATIC int
_bcm_dpp_wb_trunk_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (_dpp_wb_trunk_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_trunk_info_p[unit], sizeof(bcm_dpp_wb_trunk_info_t), "wb trunk");
        if (_dpp_wb_trunk_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Out of memory")));
        }
    }

    sal_memset(_dpp_wb_trunk_info_p[unit], 0x00, sizeof(bcm_dpp_wb_trunk_info_t));

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_trunk_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (_dpp_wb_trunk_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_trunk_info_p[unit]);
        _dpp_wb_trunk_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 */

int
_bcm_dpp_wb_trunk_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_trunk_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_TRUNK_INFO(unit);

     /* check if there was any state change or a part of init sequence */
    if (!BCM_DPP_WB_TRUNK_IS_DIRTY(unit) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_trunk_save_trunk_id_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_trunk_save_in_use_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_TRUNK_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_trunk_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_TRUNK_CURRENT_VERSION, recovered_ver;
    uint8                    *scache_ptr;
    bcm_dpp_wb_trunk_info_t  *wb_info; 

    BCMDNX_INIT_FUNC_DEFS;

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_TRUNK, 0);

    rc = _bcm_dpp_wb_trunk_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_TRUNK_INFO(unit);
    wb_info->trunk_config = (trunk_cntl_t *) &bcm_wb_trunk_control[unit];

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_trunk_layout_init(unit, recovered_ver);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_trunk_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_trunk_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_trunk_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_trunk_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* state should have been removed by Host. Stale State */
        rc = _bcm_dpp_wb_trunk_sync(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* writing to persistent storage initiated by bcm_petra_init() */
        BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
    }

    wb_info->init_done = TRUE;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_trunk_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_trunk_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}




int
_bcm_dpp_wb_trunk_update_trunk_id_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_TRUNK_UPDATE_ARR_STATE(trunk_id, 
                                      t_info[arr_indx].trunk_id, 
                                      arr_indx);
}

int
_bcm_dpp_wb_trunk_update_in_use_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_TRUNK_UPDATE_ARR_STATE(in_use, 
                                      t_info[arr_indx].in_use, 
                                      arr_indx);
}


#endif /* BCM_WARM_BOOT_SUPPORT */

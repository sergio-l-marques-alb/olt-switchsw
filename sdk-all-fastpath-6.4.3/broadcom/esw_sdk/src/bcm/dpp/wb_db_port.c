/*
 * $Id: wb_db_port.c,v 1.21 Broadcom SDK $
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
 * Warmboot - Level 2 support (PORT Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/dpp/wb_db_port.h>
#include <bcm_int/dpp/wb_db_cmn.h>
#include <bcm_int/common/debug.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/drv.h>

#include <bcm_int/dpp/alloc_mngr.h>
#include <shared/shr_template.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
bcm_dpp_wb_port_info_t   _dpp_wb_port_info_p[BCM_MAX_NUM_UNITS] = {{0}};

#define BCM_DPP_WB_PORT_INFO(unit)             (&_dpp_wb_port_info_p[unit])
#define BCM_DPP_WB_PORT_INFO_INIT(unit)        (_dpp_wb_port_info_p[unit].init_done == TRUE) 
#endif /* BCM_WARM_BOOT_SUPPORT */

    

#define BCM_DPP_WB_PORT_SET_AND_UPDATE_ARR(_dest, _var, _var_name, arr_indx1)         \
        BCM_DPP_WB_SET_AND_UPDATE_ARR(_dest, _var, PORT , port, _var_name, arr_indx1, 0x1);


#define BCM_DPP_WB_PORT_SET_AND_UPDATE_DOUBLE_ARR(_dest, _var, _var_name, arr_indx1, arr_indx2)           \
        BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR (_dest, _var, PORT , port, _var_name, arr_indx1, arr_indx2, 0x1);



#define BCM_DPP_WB_PORT_GET(_dest, _var)        \
    BCM_DPP_WB_GET(_dest, _var, 0x0)

bcm_dpp_wb_port_config_t bcm_dpp_wb_port_config[BCM_MAX_NUM_UNITS];

int
_bcm_dpp_wb_port_tpid_is_allocated(int unit, uint8 *is_alloc)
{
    *is_alloc = !(bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info == NULL);
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_port_tpid_allocate(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info == NULL) {

        BCMDNX_ALLOC(bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info,
                   sizeof(_bcm_petra_port_tpid_info), (char*)FUNCTION_NAME());

        if (bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate port_tpid_info memory"), unit));
        }

        sal_memset(bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info, 0, sizeof(_bcm_petra_port_tpid_info));
    } 
       
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_port_tpid_free(int unit)
{
    if (bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info) {
        BCM_FREE(bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info);
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_wb_port_map_tbl_use_set(int unit, int map_tbl_use, uint32 indx1, uint32 indx2)
{
    BCM_DPP_WB_PORT_SET_AND_UPDATE_DOUBLE_ARR(
        bcm_dpp_wb_port_config[unit].map_tbl_use[indx1][indx2], map_tbl_use, map_tbl_use, indx1, indx2);    
}

int
_bcm_dpp_wb_port_map_tbl_use_get(int unit, int *map_tbl_use, uint32 indx1, uint32 indx2)
{
    BCM_DPP_WB_PORT_GET(bcm_dpp_wb_port_config[unit].map_tbl_use[indx1][indx2], map_tbl_use);
}

int
_bcm_dpp_wb_port_trap_to_flag_set(int unit, int trap_to_flag, uint32 indx1)
{
    BCM_DPP_WB_PORT_SET_AND_UPDATE_ARR(
        bcm_dpp_wb_port_config[unit].trap_to_flag[indx1], trap_to_flag, trap_to_flag, indx1);
}

int
_bcm_dpp_wb_port_trap_to_flag_get(int unit, int *trap_to_flag, uint32 indx1)
{
    BCM_DPP_WB_PORT_GET(bcm_dpp_wb_port_config[unit].trap_to_flag[indx1], trap_to_flag);    
}

int
_bcm_dpp_wb_port_tpid_set(int unit, uint16 tpid, uint32 indx1, uint32 indx2)
{
    BCM_DPP_WB_PORT_SET_AND_UPDATE_DOUBLE_ARR(
        bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info->tpid[indx1][indx2], tpid, tpid, indx1, indx2);
}

int
_bcm_dpp_wb_port_tpid_get(int unit, uint16 *tpid, uint32 indx1, uint32 indx2)
{
    BCM_DPP_WB_PORT_GET(bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info->tpid[indx1][indx2], tpid);
}

int
_bcm_dpp_wb_port_tpid_count_set(int unit, int tpid_count, uint32 indx1, uint32 indx2)
{
    BCM_DPP_WB_PORT_SET_AND_UPDATE_DOUBLE_ARR(
        bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info->tpid_count[indx1][indx2], tpid_count, tpid_count, indx1, indx2);
}

int
_bcm_dpp_wb_port_tpid_count_get(int unit, int *tpid_count, uint32 indx1, uint32 indx2)
{
    BCM_DPP_WB_PORT_GET(bcm_dpp_wb_port_config[unit]._bcm_port_tpid_info->tpid_count[indx1][indx2], tpid_count);
}


#if defined(BCM_WARM_BOOT_SUPPORT)


#define BCM_DPP_WB_PORT_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_PORT_CURRENT_VERSION        BCM_DPP_WB_PORT_VERSION_1_0



/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */
/* double dimention arrays are saved in scache as spread single dimention array */
#define BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_MAP_TBL_USE   \
    _BCM_DPP_PORT_MAP_MAX_NOF_TBLS
#define BCM_DPP_WB_PORT_MAP_TBL_USE_NUM                                 \
    (_bcm_dpp_port_map_type_count * BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_MAP_TBL_USE)

#define BCM_DPP_WB_PORT_TRAP_TO_FLAG_NUM        \
    (_BCM_PETRA_PORT_LEARN_NOF_TRAPS)

/* double dimention arrays are saved in scache as spread single dimention array */
#define BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID  \
    _BCM_PORT_NOF_TPID_PROFILES
#define BCM_DPP_WB_PORT_TPID_NUM                                        \
    (_BCM_PETRA_NOF_TPIDS_PER_PORT * BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID)

/* double dimention arrays are saved in scache as spread single dimention array */
#define BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID_COUNT  \
    _BCM_PORT_NOF_TPID_PROFILES
#define BCM_DPP_WB_PORT_TPID_COUNT_NUM                                          \
    (_BCM_PETRA_NOF_TPIDS_PER_PORT * BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID_COUNT)


/*
 * map_tbl_use data structure
 * NOTE
 *   - if required optimization of packing multiple instances in a byte can be done.
 */
typedef struct bcm_dpp_wb_port_map_tbl_use_config_s {
    int    map_tbl_use;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_port_map_tbl_use_config_t;

/*
 * trap_to_flag data structure
 * NOTE
 *   - if required optimization of packing multiple instances in a byte can be done.
 */
typedef struct bcm_dpp_wb_port_trap_to_flag_config_s {
    int    trap_to_flag;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_port_trap_to_flag_config_t;


/*
 * tpid data structure
 */
typedef struct bcm_dpp_wb_port_tpid_config_s {
    uint16    tpid;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_port_tpid_config_t;

/*
 * tpid_count data structure
 */
typedef struct bcm_dpp_wb_port_tpid_count_config_s {
    uint32    tpid_count;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_port_tpid_count_config_t;


#define BCM_DPP_WB_DEV_PORT_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_PORT_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_PORT_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_PORT_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_PORT_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_PORT_INFO(unit)->is_dirty == 1)



typedef bcm_dpp_wb_port_config_t port_cntl_t;

 
#define BCM_DPP_WB_PORT_UPDATE_ARR_STATE(_var, _sw_db_var, _indx1)      \
    BCM_DPP_WB_UPDATE_ARR_STATE(PORT, port, _var, _sw_db_var, _indx1)

#define BCM_DPP_WB_PORT_UPDATE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
    BCM_DPP_WB_UPDATE_DBL_ARR_STATE(PORT, port, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2)
 



#define BCM_DPP_WB_PORT_RESTORE_ARR_STATE(_var, _sw_db_var)     \
    BCM_DPP_WB_RESTORE_ARR_STATE(PORT, port, _var, _sw_db_var)

#define BCM_DPP_WB_PORT_RESTORE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size) \
    BCM_DPP_WB_RESTORE_DBL_ARR_STATE(PORT, port, _var, _sw_db_var, _arr_iner_size)

#define BCM_DPP_WB_PORT_RESTORE_DBL_ARR_STATE_WITH_NULL_CHECK(_var, _sw_db_var, _sw_db_null_check, _arr_iner_size) \
    BCM_DPP_WB_RESTORE_DBL_ARR_STATE_WITH_NULL_CHECK(PORT, port, _var, _sw_db_var, _sw_db_null_check, _arr_iner_size)




#define BCM_DPP_WB_PORT_SAVE_ARR_STATE(_var, _sw_db_var)    \
    BCM_DPP_WB_SAVE_ARR_STATE(PORT, port, _var, _sw_db_var)

#define BCM_DPP_WB_PORT_SAVE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size) \
    BCM_DPP_WB_SAVE_DBL_ARR_STATE(PORT, port, _var, _sw_db_var, _arr_iner_size)

#define BCM_DPP_WB_PORT_SAVE_DBL_ARR_STATE_WITH_NULL_CHECK(_var, _sw_db_var, _sw_db_null_check, _arr_iner_size) \
    BCM_DPP_WB_SAVE_DBL_ARR_STATE_WITH_NULL_CHECK(PORT, port, _var, _sw_db_var, _sw_db_null_check, _arr_iner_size)


#define BCM_DPP_WB_DB_LAYOUT_INIT_PORT_VARIABLE(_var, _var_num) \
    BCM_DPP_WB_DB_LAYOUT_INIT_VARIABLE(port, _var, _var_num)


/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_port_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_port_info_t  *wb_info; 
    int                      entry_size, total_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_PORT_INFO(unit);

    wb_info->version = version;

    switch (version) {
        case BCM_DPP_WB_PORT_VERSION_1_0:

            BCM_DPP_WB_DB_LAYOUT_INIT_PORT_VARIABLE(map_tbl_use, 
                                                    BCM_DPP_WB_PORT_MAP_TBL_USE_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_PORT_VARIABLE(trap_to_flag, 
                                                    BCM_DPP_WB_PORT_TRAP_TO_FLAG_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_PORT_VARIABLE(tpid, 
                                                    BCM_DPP_WB_PORT_TPID_NUM); 

            BCM_DPP_WB_DB_LAYOUT_INIT_PORT_VARIABLE(tpid_count, 
                                                    BCM_DPP_WB_PORT_TPID_COUNT_NUM); 

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
_bcm_dpp_wb_port_restore_map_tbl_use_state(int unit)
{
    BCM_DPP_WB_PORT_RESTORE_DBL_ARR_STATE(map_tbl_use, 
                                          map_tbl_use[arr_indx1][arr_indx2], 
                                          BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_MAP_TBL_USE);
}

STATIC int
_bcm_dpp_wb_port_restore_trap_to_flag_state(int unit)
{
    BCM_DPP_WB_PORT_RESTORE_ARR_STATE(trap_to_flag, 
                                      trap_to_flag[arr_indx]);
}

STATIC int
_bcm_dpp_wb_port_restore_tpid_state(int unit)
{
    BCM_DPP_WB_PORT_RESTORE_DBL_ARR_STATE_WITH_NULL_CHECK(tpid, 
                                                          _bcm_port_tpid_info->tpid[arr_indx1][arr_indx2], 
                                                          _bcm_port_tpid_info, 
                                                          BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID);
}

STATIC int
_bcm_dpp_wb_port_restore_tpid_count_state(int unit)
{
    BCM_DPP_WB_PORT_RESTORE_DBL_ARR_STATE_WITH_NULL_CHECK(tpid_count, 
                                                          _bcm_port_tpid_info->tpid_count[arr_indx1][arr_indx2],
                                                          _bcm_port_tpid_info, 
                                                          BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID_COUNT);
}


STATIC int
_bcm_dpp_wb_port_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_port_restore_map_tbl_use_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_port_restore_trap_to_flag_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_port_restore_tpid_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_port_restore_tpid_count_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_dpp_wb_port_save_map_tbl_use_state(int unit)
{
    BCM_DPP_WB_PORT_SAVE_DBL_ARR_STATE(map_tbl_use, 
                                       map_tbl_use[arr_indx1][arr_indx2], 
                                       BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_MAP_TBL_USE);
}

STATIC int
_bcm_dpp_wb_port_save_trap_to_flag_state(int unit)
{
    BCM_DPP_WB_PORT_SAVE_ARR_STATE(trap_to_flag, 
                                   trap_to_flag[arr_indx]);
}

STATIC int
_bcm_dpp_wb_port_save_tpid_state(int unit)
{
    BCM_DPP_WB_PORT_SAVE_DBL_ARR_STATE_WITH_NULL_CHECK(tpid, 
                                                       _bcm_port_tpid_info->tpid[arr_indx1][arr_indx2],
                                                       _bcm_port_tpid_info,
                                                       BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID);
}

STATIC int
_bcm_dpp_wb_port_save_tpid_count_state(int unit)
{
    BCM_DPP_WB_PORT_SAVE_DBL_ARR_STATE_WITH_NULL_CHECK(tpid_count, 
                                                       _bcm_port_tpid_info->tpid_count[arr_indx1][arr_indx2], 
                                                       _bcm_port_tpid_info,
                                                       BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID_COUNT);
}


/*
 * Global functions
 */

int
_bcm_dpp_wb_port_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_port_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_PORT_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if (!BCM_DPP_WB_PORT_IS_DIRTY(unit) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_port_save_map_tbl_use_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_port_save_trap_to_flag_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_port_save_tpid_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_port_save_tpid_count_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_DPP_WB_PORT_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_port_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_PORT_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    bcm_dpp_wb_port_info_t  *wb_info; 

    BCMDNX_INIT_FUNC_DEFS;

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_PORT, 0);

    wb_info = BCM_DPP_WB_PORT_INFO(unit);    
    wb_info->port_config = &bcm_dpp_wb_port_config[unit];

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        rc = _bcm_dpp_wb_port_tpid_allocate(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_port_layout_init(unit, recovered_ver);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_port_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_port_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_port_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */
        sal_memset(&bcm_dpp_wb_port_config[unit], 0x0, sizeof(bcm_dpp_wb_port_config[unit]));

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_port_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* state should have been removed by Host. Stale State */
        rc = _bcm_dpp_wb_port_sync(unit);
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
_bcm_dpp_wb_port_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;    

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_port_update_map_tbl_use_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    BCM_DPP_WB_PORT_UPDATE_DBL_ARR_STATE(map_tbl_use, 
                                         map_tbl_use[arr_indx1][arr_indx2], 
                                         BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_MAP_TBL_USE, 
                                         arr_indx1, 
                                         arr_indx2);
}

int
_bcm_dpp_wb_port_update_trap_to_flag_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_PORT_UPDATE_ARR_STATE(trap_to_flag, 
                                     trap_to_flag[arr_indx], 
                                     arr_indx);
}

int
_bcm_dpp_wb_port_update_tpid_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    BCM_DPP_WB_PORT_UPDATE_DBL_ARR_STATE(tpid, 
                                         _bcm_port_tpid_info->tpid[arr_indx1][arr_indx2], 
                                         BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID, 
                                         arr_indx1, 
                                         arr_indx2);
}

int
_bcm_dpp_wb_port_update_tpid_count_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    BCM_DPP_WB_PORT_UPDATE_DBL_ARR_STATE(tpid_count, 
                                         _bcm_port_tpid_info->tpid_count[arr_indx1][arr_indx2], 
                                         BCM_DPP_WB_PORT_DBL_ARR_INER_SIZE_TPID_COUNT, 
                                         arr_indx1, 
                                         arr_indx2);
}


/*
 * Function:
 *      _bcm_dpp_wb_port_state_is_initialized
 * Purpose:
 *      check whether _bcm_dpp_wb_port_state_init has been called.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      TRUE or FALSE
 * Notes:
 */
int
_bcm_dpp_wb_port_state_is_initialized(int unit)
{
    return BCM_DPP_WB_PORT_INFO_INIT(unit);
}

#endif /* BCM_WARM_BOOT_SUPPORT */

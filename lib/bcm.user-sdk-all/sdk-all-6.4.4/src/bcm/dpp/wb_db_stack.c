/*
 * $Id: wb_db_stack.c,v 1.9 Broadcom SDK $
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
 * Warmboot - Level 2 support (STACK Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INIT

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/dpp/wb_db_stack.h>
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
typedef struct bcm_dpp_wb_stack_info_s {
    int                       init_done;
    bcm_dpp_wb_stack_config_t *stack_config;

    int                       is_dirty;

    uint16                    version;
    uint8                     *scache_ptr;
    int                       size;

    uint32                    _sysport_erp_num;
    uint32                    _sysport_erp_off;

    uint32                    _modid_to_domain_num;
    uint32                    _modid_to_domain_off;
    
    uint32                    _domain_to_stk_trunk_num;
    uint32                    _domain_to_stk_trunk_off;

} bcm_dpp_wb_stack_info_t;

static bcm_dpp_wb_stack_info_t   *_dpp_wb_stack_info_p[BCM_MAX_NUM_UNITS];


#define BCM_DPP_WB_STACK_INFO(unit)             (_dpp_wb_stack_info_p[unit])
#define BCM_DPP_WB_STACK_INFO_INIT(unit)        ( (_dpp_wb_stack_info_p[unit] != NULL) &&          \
                                                 (_dpp_wb_stack_info_p[unit]->init_done == TRUE) )
#endif /* BCM_WARM_BOOT_SUPPORT */




#define BCM_DPP_WB_STACK_SET_AND_UPDATE_ARR(_dest, _var, _var_name, arr_indx1)         \
    BCM_DPP_WB_SET_AND_UPDATE_ARR(_dest, _var, STACK , stack, _var_name, arr_indx1, 0x0);

#define BCM_DPP_WB_STACK_SET_AND_UPDATE_DOUBLE_ARR(_dest, _var, _var_name, arr_indx1, arr_indx2)           \
    BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR (_dest, _var, STACK , stack, _var_name, arr_indx1, arr_indx2, 0x0);


#define BCM_DPP_WB_STACK_GET(_dest, _var)        \
    BCM_DPP_WB_GET(_dest, _var, 0x0)


bcm_dpp_wb_stack_config_t bcm_dpp_wb_stack_config[BCM_MAX_NUM_UNITS];

int
_bcm_dpp_wb_stack__sysport_erp_get(int unit, int *_sysport_erp, uint32 indx1)
{
    BCM_DPP_WB_STACK_GET(bcm_dpp_wb_stack_config[unit]._sysport_erp[indx1], _sysport_erp);
}

int
_bcm_dpp_wb_stack__sysport_erp_set(int unit, int _sysport_erp, uint32 indx1)
{
    BCM_DPP_WB_STACK_SET_AND_UPDATE_ARR(
        bcm_dpp_wb_stack_config[unit]._sysport_erp[indx1], _sysport_erp, _sysport_erp, indx1);
}

uint32*
_bcm_dpp_wb_stack__modid_to_domain_get(int unit, uint32 domain)
{
    return bcm_dpp_wb_stack_config[unit]._modid_to_domain[domain];
}

int
_bcm_dpp_wb_stack__modid_to_domain_set(int unit, uint32 domain, uint32 modid, uint32 val)
{
    uint32  modid_bm, indx2, bit_num;
    
    indx2 = modid/32;
    bit_num = modid%32;
    
    modid_bm = bcm_dpp_wb_stack_config[unit]._modid_to_domain[domain][indx2];  
    
    if(val) {
        modid_bm |= ( 1 << (bit_num));
    } else {
        modid_bm &= ~( 1 << (bit_num));
    }
    {
        BCM_DPP_WB_STACK_SET_AND_UPDATE_DOUBLE_ARR(bcm_dpp_wb_stack_config[unit]._modid_to_domain[domain][indx2] , modid_bm, _modid_to_domain, domain, indx2);    
    }

}    

uint32*
_bcm_dpp_wb_stack__stk_trunk_to_domains_get(int unit, uint32 stk_trunk)
{
    return bcm_dpp_wb_stack_config[unit]._domain_to_stk_trunk[stk_trunk];
}


int
_bcm_dpp_wb_stack__stk_trunk_to_domain_set(int unit, uint32 stk_trunk, uint32 domain, uint32 val)
{
    uint32  domain_bm, indx2, bit_num;
    
    indx2 = domain/32;
    bit_num = domain%32;
    
    domain_bm = bcm_dpp_wb_stack_config[unit]._domain_to_stk_trunk[stk_trunk][indx2];  
    
    if(val) {
        domain_bm |= ( 1 << (bit_num));
    } else {
        domain_bm &= ~( 1 << (bit_num));
    }
    {
        BCM_DPP_WB_STACK_SET_AND_UPDATE_DOUBLE_ARR(bcm_dpp_wb_stack_config[unit]._domain_to_stk_trunk[stk_trunk][indx2] , domain_bm, _domain_to_stk_trunk, stk_trunk, indx2);  
    }
}

#if defined(BCM_WARM_BOOT_SUPPORT)


#define BCM_DPP_WB_STACK_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_STACK_CURRENT_VERSION        BCM_DPP_WB_STACK_VERSION_1_0


/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */
#define BCM_DPP_WB_STACK__SYSPORT_ERP_NUM        \
    (_DPP_STACK_MAX_DEVICES)

#define BCM_DPP_WB_STACK__MODID_TO_DOMAIN_NUM        \
    (_DPP_STACK_MAX_TM_DOMAIN * _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP)

#define BCM_DPP_WB_STACK__DOMAIN_TO_STK_TRUNK_NUM        \
    (_DPP_STACK_MAX_TM_DOMAIN * _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP)


typedef struct bcm_dpp_wb_stack__sysport_erp_config_s {
    uint32    _sysport_erp;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_stack__sysport_erp_config_t;

typedef struct bcm_dpp_wb_stack__modid_to_domain_config_s {
    uint32    _modid_to_domain;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_stack__modid_to_domain_config_t;

typedef struct bcm_dpp_wb_stack__domain_to_stk_trunk_config_s {
    uint32    _domain_to_stk_trunk;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_stack__domain_to_stk_trunk_config_t;


#define BCM_DPP_WB_DEV_STACK_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_STACK_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_STACK_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_STACK_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_STACK_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_STACK_INFO(unit)->is_dirty == 1)



typedef bcm_dpp_wb_stack_config_t stack_cntl_t;


 
#define BCM_DPP_WB_STACK_UPDATE_ARR_STATE(_var, _sw_db_var, _indx1)     \
    BCM_DPP_WB_UPDATE_ARR_STATE(STACK, stack, _var, _sw_db_var, _indx1)

#define BCM_DPP_WB_STACK_UPDATE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
    BCM_DPP_WB_UPDATE_DBL_ARR_STATE(STACK, stack, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2)
 



#define BCM_DPP_WB_STACK_RESTORE_ARR_STATE(_var, _sw_db_var)        \
    BCM_DPP_WB_RESTORE_ARR_STATE(STACK, stack, _var, _sw_db_var)

#define BCM_DPP_WB_STACK_RESTORE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size) \
    BCM_DPP_WB_RESTORE_DBL_ARR_STATE(STACK, stack, _var, _sw_db_var, _arr_iner_size)




#define BCM_DPP_WB_STACK_SAVE_ARR_STATE(_var, _sw_db_var)       \
    BCM_DPP_WB_SAVE_ARR_STATE(STACK, stack, _var, _sw_db_var)

#define BCM_DPP_WB_STACK_SAVE_DBL_ARR_STATE(_var, _sw_db_var, _arr_iner_size) \
    BCM_DPP_WB_SAVE_DBL_ARR_STATE(STACK, stack, _var, _sw_db_var, _arr_iner_size)



#define BCM_DPP_WB_DB_LAYOUT_INIT_STACK_VARIABLE(_var, _var_num)    \
    BCM_DPP_WB_DB_LAYOUT_INIT_VARIABLE(stack, _var, _var_num)




/*
 * local functions
 */

STATIC int
_bcm_dpp_wb_stack_layout_init(int unit, int version)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_stack_info_t  *wb_info; 
    int                      entry_size, total_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STACK_INFO(unit);

    wb_info->version = version;

    switch (version) {

        case BCM_DPP_WB_STACK_VERSION_1_0:

            BCM_DPP_WB_DB_LAYOUT_INIT_STACK_VARIABLE(_sysport_erp, BCM_DPP_WB_STACK__SYSPORT_ERP_NUM);

            BCM_DPP_WB_DB_LAYOUT_INIT_STACK_VARIABLE(_modid_to_domain, BCM_DPP_WB_STACK__MODID_TO_DOMAIN_NUM);
            
            BCM_DPP_WB_DB_LAYOUT_INIT_STACK_VARIABLE(_domain_to_stk_trunk, BCM_DPP_WB_STACK__DOMAIN_TO_STK_TRUNK_NUM);


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
_bcm_dpp_wb_stack_restore__sysport_erp_state(int unit)
{
    BCM_DPP_WB_STACK_RESTORE_ARR_STATE(_sysport_erp, 
                                       _sysport_erp[arr_indx]);
}

STATIC int
_bcm_dpp_wb_stack_restore__modid_to_domain_state(int unit)
{
    BCM_DPP_WB_STACK_RESTORE_DBL_ARR_STATE(_modid_to_domain, 
                                            _modid_to_domain[arr_indx1][arr_indx2],
                                            _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP);
}

STATIC int
_bcm_dpp_wb_stack_restore__domain_to_stk_trunk_state(int unit)
{
    BCM_DPP_WB_STACK_RESTORE_DBL_ARR_STATE(_domain_to_stk_trunk, 
                                       _domain_to_stk_trunk[arr_indx1][arr_indx2],
                                       _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP);
}


STATIC int
_bcm_dpp_wb_stack_restore_state(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_stack_restore__sysport_erp_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stack_restore__modid_to_domain_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);
    
    rc = _bcm_dpp_wb_stack_restore__domain_to_stk_trunk_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_dpp_wb_stack_save__sysport_erp_state(int unit)
{
    BCM_DPP_WB_STACK_SAVE_ARR_STATE(_sysport_erp, 
                                    _sysport_erp[arr_indx]);
}

STATIC int
_bcm_dpp_wb_stack_save__modid_to_domain_state(int unit)
{
    BCM_DPP_WB_STACK_SAVE_DBL_ARR_STATE(_modid_to_domain, 
                                            _modid_to_domain[arr_indx1][arr_indx2],
                                            _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP);
}

STATIC int
_bcm_dpp_wb_stack_save__domain_to_stk_trunk_state(int unit)
{
    BCM_DPP_WB_STACK_SAVE_DBL_ARR_STATE(_domain_to_stk_trunk, 
                                       _domain_to_stk_trunk[arr_indx1][arr_indx2],
                                       _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP);
}


STATIC int
_bcm_dpp_wb_stack_info_alloc(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (_dpp_wb_stack_info_p[unit] == NULL) {
        BCMDNX_ALLOC(_dpp_wb_stack_info_p[unit], sizeof(bcm_dpp_wb_stack_info_t), "wb stack");
        if (_dpp_wb_stack_info_p[unit] == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Out of memory")));
        }
    }

    sal_memset(_dpp_wb_stack_info_p[unit], 0x00, sizeof(bcm_dpp_wb_stack_info_t));

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_wb_stack_info_dealloc(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (_dpp_wb_stack_info_p[unit] != NULL) {
       BCM_FREE(_dpp_wb_stack_info_p[unit]);
        _dpp_wb_stack_info_p[unit] = NULL;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Global functions
 */

int
_bcm_dpp_wb_stack_sync(int unit)
{
    int                      rc = BCM_E_NONE;
    bcm_dpp_wb_stack_info_t  *wb_info; 


    BCMDNX_INIT_FUNC_DEFS;

    wb_info = BCM_DPP_WB_STACK_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if (!BCM_DPP_WB_STACK_IS_DIRTY(unit) && (wb_info->init_done == TRUE)) {
        BCM_EXIT;
    }

    rc = _bcm_dpp_wb_stack_save__sysport_erp_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_wb_stack_save__modid_to_domain_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);
    
    rc = _bcm_dpp_wb_stack_save__domain_to_stk_trunk_state(unit);
    BCMDNX_IF_ERR_EXIT(rc);


    BCM_DPP_WB_STACK_DIRTY_BIT_CLEAR(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_wb_stack_state_init(int unit)
{
    int                      rc = BCM_E_NONE;
    soc_scache_handle_t      wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = BCM_DPP_WB_STACK_CURRENT_VERSION, recovered_ver;
    uint8                    *scache_ptr;
    bcm_dpp_wb_stack_info_t  *wb_info; 

    BCMDNX_INIT_FUNC_DEFS;

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_STACK, 0);

    rc = _bcm_dpp_wb_stack_info_alloc(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    wb_info = BCM_DPP_WB_STACK_INFO(unit);
    wb_info->stack_config = bcm_dpp_wb_stack_config;

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */

        size = 0;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_stack_layout_init(unit, recovered_ver);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        rc = _bcm_dpp_wb_stack_restore_state(unit);
        BCMDNX_IF_ERR_EXIT(rc);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            rc = _bcm_dpp_wb_stack_layout_init(unit, version);
            BCMDNX_IF_ERR_EXIT(rc);

            size = wb_info->size;
            rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            BCMDNX_IF_ERR_EXIT(rc);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            rc = _bcm_dpp_wb_stack_sync(unit);
            BCMDNX_IF_ERR_EXIT(rc);

            /* writing to persistent storage initiated by bcm_petra_init() */
            BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        /* layout corresponding to recovered version */
        rc = _bcm_dpp_wb_stack_layout_init(unit, version);
        BCMDNX_IF_ERR_EXIT(rc);

        size = wb_info->size;
        rc = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        BCMDNX_IF_ERR_EXIT(rc);

        wb_info->scache_ptr = scache_ptr;

        /* state should have been removed by Host. Stale State */
        rc = _bcm_dpp_wb_stack_sync(unit);
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
_bcm_dpp_wb_stack_state_deinit(int unit)
{
    int                     rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_wb_stack_info_dealloc(unit);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}




int
_bcm_dpp_wb_stack_update__sysport_erp_state(int unit, uint32 arr_indx)
{
    BCM_DPP_WB_STACK_UPDATE_ARR_STATE(_sysport_erp, 
                                      _sysport_erp[arr_indx],
                                      arr_indx);
}

int
_bcm_dpp_wb_stack_update__modid_to_domain_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    BCM_DPP_WB_STACK_UPDATE_DBL_ARR_STATE(_modid_to_domain, 
                                            _modid_to_domain[arr_indx1][arr_indx2],
                                            _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP,
                                            arr_indx1,
                                            arr_indx2);
}

int
_bcm_dpp_wb_stack_update__domain_to_stk_trunk_state(int unit, uint32 arr_indx1, uint32 arr_indx2)
{
    BCM_DPP_WB_STACK_UPDATE_DBL_ARR_STATE(_domain_to_stk_trunk, 
                                       _domain_to_stk_trunk[arr_indx1][arr_indx2],
                                       _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP,
                                       arr_indx1,
                                       arr_indx2);
}

#endif /* BCM_WARM_BOOT_SUPPORT */

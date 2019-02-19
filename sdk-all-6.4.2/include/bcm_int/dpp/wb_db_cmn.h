/*
 * $Id: wb_db_cmn.h,v 1.14 Broadcom SDK $
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
 * File:        wb_db_cmn.h
 * Purpose:     WarmBoot - Level 2 support common file
 */

#ifndef _BCM_DPP_WB_DB_CMN_H_
#define _BCM_DPP_WB_DB_CMN_H_


#include <soc/drv.h>
#include <soc/dpp/drv.h>


/*get single value content */
#define BCM_DPP_WB_GET(_dest, _var, _flags)        \
    BCM_DPP_WB_GET_INIT_DEFS;                      \
    BCM_DPP_WB_GET_CONTENT(_dest, _var, _flags);

#define BCM_DPP_WB_GET_INIT_DEFS    \
    BCMDNX_INIT_FUNC_DEFS;

#define BCM_DPP_WB_GET_CONTENT(_dest, _var, _flags)  \
    *_var = _dest;                                   \
    BCM_EXIT;                                        \
                                                     \
exit:                                                \
    BCMDNX_FUNC_RETURN;

/*set single value content */
#define BCM_DPP_WB_SET(_dest, _var, _MODULE, _module, _flags)        \
    BCM_DPP_WB_SET_INIT_DEFS;                               \
    BCM_DPP_WB_SET_CONTENT(_dest, _var, _MODULE, _module, _flags);

#define BCM_DPP_WB_SET_CONTENT(_dest, _var, _MODULE, _module, _flags)  \
    _dest = _var;                                             \
    BCM_EXIT;                                                 \
                                                              \
exit:                                                         \
BCMDNX_FUNC_RETURN;


/* set and update single value content 
 * flags=0x1 - check that init is done before performing wb update
 */

#if defined(BCM_WARM_BOOT_SUPPORT)
#define BCM_DPP_WB_SET_AND_UPDATE_CONTENT(_dest, _var, _MODULE, _module, _var_name, _flags)  \
    _dest = _var;                                                             \
    if ((_flags & 0x1) && !BCM_DPP_WB_##_MODULE##_INFO_INIT(unit)){      \
        BCM_EXIT;                                                             \
    }                                                                         \
    _rv = _bcm_dpp_wb_##_module##_update_##_var_name##_state(unit);           \
    BCMDNX_IF_ERR_EXIT_NO_UNIT(_rv);                                             \
    BCM_EXIT;                                                                 \
exit:                                                                         \
BCMDNX_FUNC_RETURN;

#else
#define BCM_DPP_WB_SET_AND_UPDATE_CONTENT(_dest, _var, _MODULE, _module, _var_name, _flags)  \
    _dest = _var;                                                        \
    BCM_EXIT;                                                            \
                                                                         \
exit:                                                                    \
BCMDNX_FUNC_RETURN;

#endif /* defined(BCM_WARM_BOOT_SUPPORT) && (_flags == 0x1) */



/*set and update double array value content 
 * flags=0x1 - check that init is done before performing wb update
 */
#define BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR(_dest, _var, _MODULE, _module, _var_name, arr_indx1, arr_indx2, _flags)    \
    BCM_DPP_WB_SET_INIT_DEFS;                                                     \
    BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR_CONTENT(_dest, _var, _MODULE, _module, _var_name, arr_indx1, arr_indx2, _flags);

#if defined(BCM_WARM_BOOT_SUPPORT)
#define BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR_CONTENT(_dest, _var, _MODULE, _module, _var_name, arr_indx1, arr_indx2, _flags)  \
    _dest = _var;                                                        \
    if ((_flags & 0x1) && !BCM_DPP_WB_##_MODULE##_INFO_INIT(unit)){ \
        BCM_EXIT;                                                             \
    }                                                                         \
    _rv = _bcm_dpp_wb_##_module##_update_##_var_name##_state(unit, arr_indx1, arr_indx2);      \
    BCMDNX_IF_ERR_EXIT_NO_UNIT(_rv);                                        \
    BCM_EXIT;                                                            \
                                                                         \
exit:                                                                    \
BCMDNX_FUNC_RETURN;

#else
#define BCM_DPP_WB_SET_AND_UPDATE_DOUBLE_ARR_CONTENT(_dest, _var, _MODULE, _module, _var_name, arr_indx1, arr_indx2, _flags)  \
    _dest = _var;                                                        \
    BCM_EXIT;                                                            \
                                                                         \
exit:                                                                    \
BCMDNX_FUNC_RETURN;

#endif /* defined(BCM_WARM_BOOT_SUPPORT) && (_flags == 0x1) */

/*set and update array value content 
 * flags=0x1 - check that init is done before performing wb update
 */
#define BCM_DPP_WB_SET_AND_UPDATE_ARR(_dest, _var, _MODULE, _module, _var_name, arr_indx1, _flags)    \
    BCM_DPP_WB_SET_INIT_DEFS;                                                     \
    BCM_DPP_WB_SET_AND_UPDATE_ARR_CONTENT(_dest, _var, _MODULE, _module, _var_name, arr_indx1, _flags);

#if defined(BCM_WARM_BOOT_SUPPORT)
#define BCM_DPP_WB_SET_AND_UPDATE_ARR_CONTENT(_dest, _var, _MODULE, _module, _var_name, arr_indx1, _flags)  \
    _dest = _var;                                                        \
    if ((_flags & 0x1) && !BCM_DPP_WB_##_MODULE##_INFO_INIT(unit)){ \
        BCM_EXIT;                                                             \
    }                                                                         \
    _rv = _bcm_dpp_wb_##_module##_update_##_var_name##_state(unit, arr_indx1);      \
    BCMDNX_IF_ERR_EXIT_NO_UNIT(_rv);                                        \
    BCM_EXIT;                                                            \
                                                                         \
exit:                                                                    \
BCMDNX_FUNC_RETURN;

#else
#define BCM_DPP_WB_SET_AND_UPDATE_ARR_CONTENT(_dest, _var, _MODULE, _module, _var_name, arr_indx1, _flags)  \
    _dest = _var;                                                        \
    BCM_EXIT;                                                            \
                                                                         \
exit:                                                                    \
BCMDNX_FUNC_RETURN;

#endif /* defined(BCM_WARM_BOOT_SUPPORT) && (_flags == 0x1) */


#define BCM_DPP_WB_SET_INIT_DEFS    \
    BCMDNX_INIT_FUNC_DEFS;


#if defined(BCM_WARM_BOOT_SUPPORT)

#define BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var)                     \
                                                                        \
    int                              rc = BCM_E_NONE;                   \
    bcm_dpp_wb_##_module##_info_t          *wb_info;                    \
    bcm_dpp_wb_##_module##_##_var##_config_t    *_var##_state;          \
    _module##_cntl_t                     *_module##_config;             \
    BCMDNX_INIT_FUNC_DEFS;                                                 \
    /* NOTE: This function has to handle conditions where the s/w state is already intialized */ \
    /*       (e.g. default state that is allocated during init).                              */ \
    /*       One approach is during warmboot module_init() does no initialize the s/w state   */ \
    /*       and depends on restore for update of the state. State that never changes does    */ \
    /*       not have to be stored.                                                           */


#define BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var)          \
                                                                        \
                                                                        \
    wb_info = BCM_DPP_WB_##_MODULE##_INFO(unit);                        \
    _module##_config = wb_info->_module##_config;                       \
                                                                        \
    switch(wb_info->version) {                                          \
    case BCM_DPP_WB_##_MODULE##_VERSION_1_0:                            \
                                                                        \
    _var##_state = (bcm_dpp_wb_##_module##_##_var##_config_t *)(wb_info->scache_ptr + wb_info->_var##_off);


#define BCM_DPP_WB_GENERIC_FUNC_SUFFIX                                  \
                                                                        \
    break;                                                              \
                                                                        \
    default:                                                            \
    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown scache data version"))); \
    break;                                                              \
    }                                                                   \
                                                                        \
BCMDNX_IF_ERR_EXIT(rc);                                                    \
exit:                                                                   \
BCMDNX_FUNC_RETURN;




/* saving single var content */
#define BCM_DPP_WB_SAVE_SINGLE_STATE(_MODULE, _module, _var, _sw_db_var) \
                                                                        \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    _var##_state->_var = _module##_config->_sw_db_var;                  \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;


/* saving array content */
#define BCM_DPP_WB_SAVE_ARR_STATE(_MODULE, _module, _var, _sw_db_var)   \
                                                                        \
    int                                 arr_indx;                       \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        _var##_state->_var = _module##_config->_sw_db_var;              \
                                                                        \
    }                                                                   \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;

/* saving double array content */
#define BCM_DPP_WB_SAVE_DBL_ARR_STATE(_MODULE, _module, _var, _sw_db_var, _arr_iner_size) \
                                                                        \
    int                                 arr_indx;                       \
    uint32                              arr_indx1, arr_indx2;           \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arr_indx1 = arr_indx / (_arr_iner_size);                        \
        arr_indx2 = arr_indx % (_arr_iner_size);                        \
                                                                        \
        _var##_state->_var = _module##_config->_sw_db_var;              \
                                                                        \
    }                                                                   \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;

/* saving double array content with NULL pointer check.
 (NULL pointer check is needed when data is allocated in persistent storage but not always allocated in sw data base.
 this data is allocated in persitent storage because sw data base allocation relays on sw data base internal logic,
 this logic is unavailable/unaware in wb layer) */
#define BCM_DPP_WB_SAVE_DBL_ARR_STATE_WITH_NULL_CHECK(_MODULE, _module, _var, _sw_db_var, _sw_db_null_check, _arr_iner_size) \
                                                                        \
    int                                 arr_indx;                       \
    uint32                              arr_indx1, arr_indx2;           \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arr_indx1 = arr_indx / (_arr_iner_size);                        \
        arr_indx2 = arr_indx % (_arr_iner_size);                        \
                                                                        \
        if(NULL != _module##_config->_sw_db_null_check) {               \
            _var##_state->_var = _module##_config->_sw_db_var;          \
        }                                                               \
                                                                        \
    }                                                                   \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;





/* restoring single var content */
#define BCM_DPP_WB_RESTORE_SINGLE_STATE(_MODULE, _module, _var, _sw_db_var) \
                                                                        \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    _module##_config->_sw_db_var = _var##_state->_var;                  \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;


/* restoring array content */
#define BCM_DPP_WB_RESTORE_ARR_STATE(_MODULE, _module, _var, _sw_db_var) \
                                                                        \
    int                                 arr_indx;                       \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        _module##_config->_sw_db_var = _var##_state->_var;              \
                                                                        \
    }                                                                   \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;

/* restoring double array content */
#define BCM_DPP_WB_RESTORE_DBL_ARR_STATE(_MODULE, _module, _var, _sw_db_var, _arr_iner_size) \
                                                                        \
    int                                 arr_indx;                       \
    uint32                              arr_indx1, arr_indx2;           \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arr_indx1 = arr_indx / (_arr_iner_size);                        \
        arr_indx2 = arr_indx % (_arr_iner_size);                        \
                                                                        \
        _module##_config->_sw_db_var = _var##_state->_var;              \
                                                                        \
    }                                                                   \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;


/* restoring double array content with NULL pointer check.
 (NULL pointer check is needed when data is allocated in persistent storage but not always allocated in sw data base.
 this data is allocated in persitent storage because sw data base allocation relays on sw data base internal logic,
 this logic is unavailable/unaware in wb layer) */
#define BCM_DPP_WB_RESTORE_DBL_ARR_STATE_WITH_NULL_CHECK(_MODULE, _module, _var, _sw_db_var, _sw_db_null_check, _arr_iner_size) \
                                                                        \
    int                                 arr_indx;                       \
    uint32                              arr_indx1, arr_indx2;           \
    BCM_DPP_WB_GENERIC_INIT_DEFS(_module, _var);                        \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_PREFIX(_MODULE, _module, _var);             \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arr_indx1 = arr_indx / (_arr_iner_size);                        \
        arr_indx2 = arr_indx % (_arr_iner_size);                        \
                                                                        \
        if(NULL != _module##_config->_sw_db_null_check) {               \
            _module##_config->_sw_db_var = _var##_state->_var;          \
        }                                                               \
                                                                        \
    }                                                                   \
                                                                        \
    BCM_DPP_WB_GENERIC_FUNC_SUFFIX;





/*update single value content */
#define BCM_DPP_WB_UPDATE_SINGLE_STATE(_MODULE, _module, _var, _sw_db_var)  \
                                                                        \
    BCM_DPP_WB_UPDATE_STATE_INIT_DEFS(_module, _var);                   \
                                                                        \
    BCM_DPP_WB_UPDATE_SINGLE_CONTENT(_MODULE, _module, _var, _sw_db_var);

/*update array content */
#define BCM_DPP_WB_UPDATE_ARR_STATE(_MODULE, _module, _var, _sw_db_var, _indx1) \
                                                                        \
    BCM_DPP_WB_UPDATE_STATE_INIT_DEFS(_module, _var);                   \
                                                                        \
    BCM_DPP_WB_UPDATE_ARR_CONTENT(_MODULE, _module, _var, _sw_db_var, _indx1);

/*update double array content */
#define BCM_DPP_WB_UPDATE_DBL_ARR_STATE(_MODULE, _module, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
                                                                        \
    BCM_DPP_WB_UPDATE_STATE_INIT_DEFS(_module, _var);                   \
                                                                        \
    BCM_DPP_WB_UPDATE_DBL_ARR_CONTENT(_MODULE, _module, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2);




#define BCM_DPP_WB_UPDATE_STATE_INIT_DEFS(_module, _var)                \
                                                                        \
    int                              rc = BCM_E_NONE;                   \
    bcm_dpp_wb_##_module##_info_t          *wb_info;                    \
    bcm_dpp_wb_##_module##_##_var##_config_t    *_var##_state;          \
    _module##_cntl_t                     *_module##_config;             \
    uint32                           data_size;                         \
    uint8                            *data;                             \
    int                              offset;                            \
    soc_scache_handle_t              wb_handle;                         \
    uint32                           sd_arr_indx;  /* single dimention array index */ \
                                                                        \
    BCMDNX_INIT_FUNC_DEFS;



#define BCM_DPP_WB_UPDATE_SINGLE_CONTENT(_MODULE, _module, _var, _sw_db_var) \
    BCM_DPP_WB_UPDATE_ARR_CONTENT(_MODULE, _module, _var, _sw_db_var, 0) 

#define BCM_DPP_WB_UPDATE_ARR_CONTENT(_MODULE, _module, _var, _sw_db_var, _indx1)        \
/* we can't use this - it causes coverity defects    BCM_DPP_WB_UPDATE_DBL_ARR_CONTENT(_MODULE, _module, _var, _sw_db_var, 0, 0, _indx1) */\
                                                                        \
    /* handle corner case of being invoked before the wb module is initialized      */ \
    /* (e.g. from module_init().                                                      */ \
    /* default configuration either does not have to be saved or has to be handled  */ \
    /* in a special manner.                                                         */ \
    if (!(BCM_DPP_WB_##_MODULE##_INFO_INIT(unit))) {                    \
        BCM_EXIT;                                                       \
    }                                                                   \
                                                                        \
    /* check if immediate sync not enabled  */                          \
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {                     \
        /* mark dirty state. Currently no other processing done           */ \
        /* if required framework cache could be updated with updating the */ \
        /* persistent storage                                             */ \
        BCM_DPP_WB_DEV_##_MODULE##_DIRTY_BIT_SET(unit);                 \
        BCM_EXIT;                                                       \
    }                                                                   \
                                                                        \
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_##_MODULE, 0);    \
                                                                        \
    wb_info = BCM_DPP_WB_##_MODULE##_INFO(unit);                        \
    _module##_config = wb_info->_module##_config;                       \
                                                                        \
    switch(wb_info->version) {                                          \
        case BCM_DPP_WB_##_MODULE##_VERSION_1_0:                        \
                                                                        \
            /* update framework cache and peristent storage                       */ \
            _var##_state = (bcm_dpp_wb_##_module##_##_var##_config_t *)(wb_info->scache_ptr + wb_info->_var##_off); \
                                                                        \
                                                                        \
            sd_arr_indx = (_indx1);                                     \
                                                                        \
            if(sd_arr_indx >= wb_info->_var##_num) {                    \
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Parameter error, index out of bound"))); \
            }                                                           \
                                                                        \
            _var##_state += sd_arr_indx;                                \
                                                                        \
            _var##_state->_var = _module##_config->_sw_db_var;          \
                                                                        \
                                                                        \
            data_size = sizeof(bcm_dpp_wb_##_module##_##_var##_config_t); \
            data = (uint8 *)_var##_state;                               \
            offset = (uint32)(data - wb_info->scache_ptr);              \
                                                                        \
            break;                                                      \
                                                                        \
        default:                                                        \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown scache data version"))); \
            break;                                                      \
    }                                                                   \
                                                                        \
    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset); \
    BCMDNX_IF_ERR_EXIT(rc);                                                \
exit:                                                                   \
BCMDNX_FUNC_RETURN;

#define BCM_DPP_WB_UPDATE_DBL_ARR_CONTENT(_MODULE, _module, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
                                                                        \
    /* handle corner case of being invoked before the wb module is initialized      */ \
    /* (e.g. from module_init().                                                      */ \
    /* default configuration either does not have to be saved or has to be handled  */ \
    /* in a special manner.                                                         */ \
    if (!(BCM_DPP_WB_##_MODULE##_INFO_INIT(unit))) {                    \
        BCM_EXIT;                                                       \
    }                                                                   \
                                                                        \
    /* check if immediate sync not enabled  */                          \
    if (!_bcm_dpp_switch_is_immediate_sync(unit)) {                     \
        /* mark dirty state. Currently no other processing done           */ \
        /* if required framework cache could be updated with updating the */ \
        /* persistent storage                                             */ \
        BCM_DPP_WB_DEV_##_MODULE##_DIRTY_BIT_SET(unit);                 \
        BCM_EXIT;                                                       \
    }                                                                   \
                                                                        \
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, BCM_MODULE_##_MODULE, 0);    \
                                                                        \
    wb_info = BCM_DPP_WB_##_MODULE##_INFO(unit);                        \
    _module##_config = wb_info->_module##_config;                       \
                                                                        \
    switch(wb_info->version) {                                          \
        case BCM_DPP_WB_##_MODULE##_VERSION_1_0:                        \
                                                                        \
            /* update framework cache and peristent storage                       */ \
            _var##_state = (bcm_dpp_wb_##_module##_##_var##_config_t *)(wb_info->scache_ptr + wb_info->_var##_off); \
                                                                        \
                                                                        \
            sd_arr_indx = ((_indx1) * (_arr_iner_size)) + (_indx2);     \
                                                                        \
            if(sd_arr_indx >= wb_info->_var##_num ||                    \
                (_indx2)         >= (_arr_iner_size)) {                 \
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Parameter error, index out of bound"))); \
            }                                                           \
                                                                        \
            _var##_state += sd_arr_indx;                                \
                                                                        \
            _var##_state->_var = _module##_config->_sw_db_var;          \
                                                                        \
                                                                        \
            data_size = sizeof(bcm_dpp_wb_##_module##_##_var##_config_t); \
            data = (uint8 *)_var##_state;                               \
            offset = (uint32)(data - wb_info->scache_ptr);              \
                                                                        \
            break;                                                      \
                                                                        \
        default:                                                        \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unknown scache data version"))); \
            break;                                                      \
    }                                                                   \
                                                                        \
    rc = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset); \
    BCMDNX_IF_ERR_EXIT(rc);                                                \
exit:                                                                   \
BCMDNX_FUNC_RETURN;





#define BCM_DPP_WB_DB_LAYOUT_INIT_VARIABLE(_module, _var, _var_num)     \
                                                                        \
    entry_size = sizeof(bcm_dpp_wb_##_module##_##_var##_config_t) * _var_num; \
    entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */ \
    wb_info->_var##_num = _var_num;                                     \
    wb_info->_var##_off  = total_size;                                  \
    total_size += entry_size;                                   



#define BCM_DPP_WB_DEV_DIRTY_BIT_SET(unit)                                            \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        SOC_CONTROL_UNLOCK(unit);





#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _BCM_DPP_WB_DB_CMN_H_ */

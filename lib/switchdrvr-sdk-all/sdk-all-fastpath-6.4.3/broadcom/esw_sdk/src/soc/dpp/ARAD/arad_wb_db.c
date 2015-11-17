/*
 * $Id: arad_wb_db.c,v 1.24 Broadcom SDK $
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
 * Warmboot - Level 2 support (EGR_PORT Module)
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/ARAD/arad_wb_db.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/wb_utils.h>
#include <soc/dpp/dpp_wb_engine.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */


#ifdef BCM_WARM_BOOT_SUPPORT

#define ARAD_WB_DB_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define ARAD_WB_DB_CURRENT_VERSION        ARAD_WB_DB_VERSION_1_0



#define ARAD_WB_DB_INFO(unit)             (_arad_wb_db_info_p[unit])
#define ARAD_WB_DB_INFO_INIT(unit)        ( (_arad_wb_db_info_p[unit] != NULL) &&          \
                                                 (_arad_wb_db_info_p[unit]->init_done == TRUE) )



#define ARAD_UNIT_DIRTY_BIT_SET(unit)          \
                     SOC_CONTROL_LOCK(unit);                     \
                     SOC_CONTROL(unit)->scache_dirty = 1;        \
                     SOC_CONTROL_UNLOCK(unit);



#define ARAD_WB_DB_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        ARAD_WB_DB_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define ARAD_WB_DB_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        ARAD_WB_DB_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define ARAD_WB_DB_IS_DIRTY(unit)                                                \
                        (ARAD_WB_DB_INFO(unit)->is_dirty == 1)







/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */
#define ARAD_WB_DB_EGR_PORT_QUEUE_RATE_NUM         \
    (SOC_DPP_DEFS_MAX(NOF_CORES))

#define ARAD_WB_DB_EGR_PORT_TCG_RATE_NUM         \
    (SOC_DPP_DEFS_MAX(NOF_CORES))

/* double dimention arrays are saved in scache as spread single dimention array */
#define ARAD_WB_DB_EGR_PORT_DBL_ARR_INER_SIZE_RATES  \
    ARAD_EGR_NOF_BASE_Q_PAIRS
#define ARAD_WB_DB_EGR_PORT_RATES_NUM         \
    (SOC_DPP_DEFS_MAX(NOF_CORES) * ARAD_WB_DB_EGR_PORT_DBL_ARR_INER_SIZE_RATES)

#define ARAD_WB_DB_EGR_PORT_NOF_CALCAL_INSTANCES_NUM         \
    (SOC_DPP_DEFS_MAX(NOF_CHANNELIZED_CALENDARS))

#define ARAD_WB_DB_EGR_PORT_CALCAL_LENGTH_NUM         \
    (1)

#define ARAD_WB_DB_EGR_PORT_UPDATE_DEVICE_NUM         \
    (1)

#define ARAD_WB_DB_EGR_PORT_EGQ_TCG_QPAIR_SHAPER_ENABLE_NUM         \
    (1)

#define ARAD_WB_DB_EGR_PORT_ERP_INTERFACE_NUM         \
    (1)

#define ARAD_WB_DB_EGR_PORT_DSP_PP_TO_BASE_BASE_QUEUE_PAIR_MAPPING_NUM         \
    (ARAD_NOF_FAP_PORTS * SOC_DPP_DEFS_MAX(NOF_CORES))

#define ARAD_WB_DB_EGR_PORT_DSP_PP_NOF_QUEUE_PAIRS_NUM         \
    (ARAD_NOF_FAP_PORTS * SOC_DPP_DEFS_MAX(NOF_CORES))

#define ARAD_WB_DB_EGR_PORT_PORTS_PROG_EDITOR_PROFILE_NUM         \
    (ARAD_NOF_FAP_PORTS)



#define ARAD_WB_DB_LAG_IN_USE_NUM        \
    (ARAD_NOF_LAG_GROUPS_MAX)


#define ARAD_WB_DB_LAG_LOCAL_TO_REASSEMBLY_CONTEXT_NUM        \
    (ARAD_NOF_LOCAL_PORTS)

#define ARAD_WB_DB_TDM_CONTEXT_MAP_NUM        \
    (ARAD_NOF_TDM_CONTEXT_MAP)




#define ARAD_WB_DB_CELL_CURRENT_CELL_IDENT_NUM         \
    (1)

#define ARAD_WB_DB_DRAM_DELETED_BUFF_MAX_NUM \
    ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR

#define ARAD_WB_DB_INTERRUPTS_CMCS_NUM   \
    SOC_CMCS_NUM_MAX

#define ARAD_WB_DB_INTERRUPTS_INTERRUPTS_NUM         \
    ARAD_INT_LAST

#define ARAD_WB_DB_Q_TYPE_REF_COUNT_NUM(unit)        \
    ARAD_SW_DB_NOF_QUEUE_TYPES(unit)

#define ARAD_WB_DB__Q_TYPE_MAP_NUM        \
    ARAD_SW_DB_NOF_DYNAMIC_QUEUE_TYPES

#define ARAD_WB_DB_VSI_TO_ISID_NUM         \
    (32*1024)

#define ARAD_WB_DB_TM_IS_SIMPLE_Q_TO_RATE_CLS_MODE_NUM \
            (1)

#define ARAD_WB_DB_TM_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_NUM \
            SOC_TMC_ITM_NOF_RATE_CLASSES

#define ARAD_WB_DB_TM_SYSPORT2BASEQUEUE_COUNT_NUM(unit) \
            (ARAD_NOF_SYS_PHYS_PORTS_GET(unit) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)

#define ARAD_WB_DB_MODPORT2SYSPORT_COUNT_NUM(unit) \
                (ARAD_NOF_MODPORT)

typedef struct arad_wb_db_egr_port_port_priority_cal_config_s {
    ARAD_SW_DB_DEV_EGR_PORT_PRIORITY    port_priority_cal;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_port_priority_cal_config_t;

typedef struct arad_wb_db_egr_port_tcg_cal_config_s {
    ARAD_SW_DB_DEV_EGR_TCG    tcg_cal;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_tcg_cal_config_t;

typedef struct arad_wb_db_egr_port_rates_config_s {
    ARAD_SW_DB_DEV_EGR_RATE    rates;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_rates_config_t;

typedef struct arad_wb_db_egr_port_nof_calcal_instances_config_s {
    uint32     nof_calcal_instances;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_nof_calcal_instances_config_t;

typedef struct arad_wb_db_egr_port_calcal_length_config_s {
    uint32     calcal_length;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_calcal_length_config_t;

typedef struct arad_wb_db_egr_port_update_device_config_s {
    uint8      update_device;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_update_device_config_t;

typedef struct arad_wb_db_egr_port_egq_tcg_qpair_shaper_enable_config_s {
    uint8     egq_tcg_qpair_shaper_enable;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_egq_tcg_qpair_shaper_enable_config_t;

typedef struct arad_wb_db_egr_port_erp_interface_config_s {
    uint8     erp_interface;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_erp_interface_config_t;

typedef struct arad_wb_db_egr_port_ports_prog_editor_profile_config_s {
    uint16     ports_prog_editor_profile;
}  __ATTRIBUTE_PACKED__ arad_wb_db_egr_port_ports_prog_editor_profile_config_t;


typedef struct arad_wb_db_lag_in_use_config_s {
    uint8      in_use;
}  __ATTRIBUTE_PACKED__ arad_wb_db_lag_in_use_config_t;

typedef struct arad_wb_db_lag_local_to_reassembly_context_config_s {
    uint32     local_to_reassembly_context;
}  __ATTRIBUTE_PACKED__ arad_wb_db_lag_local_to_reassembly_context_config_t;

typedef struct arad_wb_db_tdm_context_map_config_s {
    uint32     context_map;
}  __ATTRIBUTE_PACKED__ arad_wb_db_tdm_context_map_config_t;

            

typedef struct arad_wb_db_cell_current_cell_ident_config_s {
    uint16     current_cell_ident;
}  __ATTRIBUTE_PACKED__ arad_wb_db_cell_current_cell_ident_config_t;

typedef struct arad_wb_db_dram_dram_deleted_buff_list_config_s {
    uint32 dram_deleted_buff_list;
}  __ATTRIBUTE_PACKED__ arad_wb_db_dram_dram_deleted_buff_list_config_t;

/* The following 3 functions are not used and remain for future use */
typedef struct arad_wb_db_interrupts_cmc_irq2_mask_config_s {
    uint32 cmc_irq2_mask;
}  __ATTRIBUTE_PACKED__ arad_wb_db_interrupts_cmc_irq2_mask_config_t;

typedef struct arad_wb_db_interrupts_cmc_irq3_mask_config_s {
    uint32 cmc_irq3_mask;
}  __ATTRIBUTE_PACKED__ arad_wb_db_interrupts_cmc_irq3_mask_config_t;

typedef struct arad_wb_db_interrupts_cmc_irq4_mask_config_s {
    uint32 cmc_irq4_mask;
}  __ATTRIBUTE_PACKED__ arad_wb_db_interrupts_cmc_irq4_mask_config_t;

typedef struct arad_wb_db_interrupts_interrupt_flags_config_s {
    uint32 interrupt_flags;
}  __ATTRIBUTE_PACKED__ arad_wb_db_interrupts_interrupt_flags_config_t;

typedef struct arad_wb_db_interrupts_interrupt_storm_timed_count_config_s {
    uint32 interrupt_storm_timed_count;
}  __ATTRIBUTE_PACKED__ arad_wb_db_interrupts_interrupt_storm_timed_count_config_t;
typedef struct arad_wb_db_interrupts_interrupt_storm_timed_period_config_s {
    uint32 interrupt_storm_timed_period;
}  __ATTRIBUTE_PACKED__ arad_wb_db_interrupts_interrupt_storm_timed_period_config_t;

typedef struct arad_wb_db_multicast_egress_groups_open_data_config_s {
    uint32     egress_groups_open_data;
}  __ATTRIBUTE_PACKED__ arad_wb_db_multicast_egress_groups_open_data_config_t;

typedef struct arad_wb_db_qt_q_type_ref_count_config_s {
    uint8     q_type_ref_count;
}  __ATTRIBUTE_PACKED__ arad_wb_db_qt_q_type_ref_count_config_t;

typedef struct arad_wb_db_qt_q_type_map_config_s {
    uint8     q_type_map;
}  __ATTRIBUTE_PACKED__ arad_wb_db_qt_q_type_map_config_t;

typedef struct arad_wb_db_vsi_vsi_to_isid_config_s {
    uint32     vsi_to_isid;
}  __ATTRIBUTE_PACKED__ arad_wb_db_vsi_vsi_to_isid_config_t;

typedef struct arad_wb_db_tm_queue_to_rate_class_mapping_is_simple_config_s {
    uint8     queue_to_rate_class_mapping_is_simple;
} __ATTRIBUTE_PACKED__ arad_wb_db_tm_queue_to_rate_class_mapping_is_simple_config_t;

typedef struct arad_wb_db_tm_queue_to_rate_class_mapping_ref_count_config_s {
    uint32     queue_to_rate_class_mapping_ref_count;
} __ATTRIBUTE_PACKED__ arad_wb_db_tm_queue_to_rate_class_mapping_ref_count_config_t;

typedef struct arad_wb_db_tm_sysport2basequeue_config_s {
    ARAD_SW_DB_SYSPORT_TO_BASE_QUEUE sysport2basequeue;
} __ATTRIBUTE_PACKED__ arad_wb_db_tm_sysport2basequeue_config_t;

typedef struct arad_wb_db_modport2sysport_config_s {
     ARAD_SYSPORT modport2sysport;
} __ATTRIBUTE_PACKED__ arad_wb_db_modport2sysport_modport2sysport_config_t;


arad_wb_db_info_t   *_arad_wb_db_info_p[SOC_MAX_NUM_DEVICES];




/*update single value content */
#define ARAD_WB_DB_UPDATE_SINGLE_STATE(_function, _module, _var, _sw_db_var)  \
                                                                        \
    ARAD_WB_DB_UPDATE_STATE_INIT_DEFS(_module, _var)                    \
                                                                        \
    SOC_SAND_INIT_ERROR_DEFINITIONS(_function);                             \
                                                                        \
    ARAD_WB_DB_UPDATE_SINGLE_CONTENT(_module, _var, _sw_db_var);

/*update array content */
#define ARAD_WB_DB_UPDATE_ARR_STATE(_function, _module, _var, _sw_db_var, _indx1) \
                                                                        \
    ARAD_WB_DB_UPDATE_STATE_INIT_DEFS(_module, _var)                    \
                                                                        \
    SOC_SAND_INIT_ERROR_DEFINITIONS(_function);                             \
                                                                        \
    ARAD_WB_DB_UPDATE_ARR_CONTENT(_module, _var, _sw_db_var, _indx1);

/*update double array content */
#define ARAD_WB_DB_UPDATE_DBL_ARR_STATE(_function, _module, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
                                                                        \
    ARAD_WB_DB_UPDATE_STATE_INIT_DEFS(_module, _var)                    \
                                                                        \
    SOC_SAND_INIT_ERROR_DEFINITIONS(_function);                             \
                                                                        \
    ARAD_WB_DB_UPDATE_DBL_ARR_CONTENT(_module, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2);




#define ARAD_WB_DB_UPDATE_STATE_INIT_DEFS(_module, _var)                \
                                                                        \
    uint32                            res = SOC_SAND_OK;                  \
    arad_wb_db_info_t                  *wb_info;                        \
    arad_wb_db_##_module##_##_var##_config_t *_var##_state;             \
    ARAD_SW_DB_DEVICE                  *arad_sw_db;                     \
    uint32                              data_size;                      \
    uint8                              *data;                           \
    int                                 offset;                         \
    soc_scache_handle_t                 wb_handle;                      \
    uint32                              sd_arr_indx; /* single dimention array index */


#define ARAD_WB_DB_UPDATE_SINGLE_CONTENT(_module, _var, _sw_db_var) \
    ARAD_WB_DB_UPDATE_ARR_CONTENT(_module, _var, _sw_db_var, 0) 

#define ARAD_WB_DB_UPDATE_ARR_CONTENT(_module, _var, _sw_db_var, _indx1)        \
/* we can't do    ARAD_WB_DB_UPDATE_DBL_ARR_CONTENT(_module, _var, _sw_db_var, 0, 0, _indx1) */\
/* It cause coverity defects */\
                                                                        \
    /* handle corner case of being invoked before the wb module is initialized      */ \
    /* (e.g. from egr_port_init().                                                      */ \
    /* default configuration either does not have to be saved or has to be handled  */ \
    /* in a special manner.                                                         */ \
    if (!(ARAD_WB_DB_INFO_INIT(unit))) {                                \
        SOC_SAND_EXIT_NO_ERROR;                                             \
    }                                                                   \
                                                                        \
    /* check if immediate sync not enabled  */                          \
    if (!(SOC_CONTROL(unit)->autosync)) {                               \
        /* mark dirty state. Currently no other processing done           */ \
        /* if required framework cache could be updated with updating the */ \
        /* persistent storage                                             */ \
        ARAD_WB_DB_DIRTY_BIT_SET(unit);                                 \
        SOC_SAND_EXIT_NO_ERROR;                                             \
    }                                                                   \
                                                                        \
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, ARAD_WB_DB_MODULE, 0);       \
                                                                        \
    wb_info = ARAD_WB_DB_INFO(unit);                                    \
    arad_sw_db = wb_info->arad_sw_db;                                   \
                                                                        \
    switch(wb_info->version) {                                          \
        case ARAD_WB_DB_VERSION_1_0:                                    \
                                                                        \
            /* update framework cache and peristent storage                       */ \
            _var##_state = (arad_wb_db_##_module##_##_var##_config_t *)(wb_info->scache_ptr + wb_info->_var##_off); \
                                                                        \
                                                                        \
            sd_arr_indx = _indx1;                                       \
                                                                        \
            if(sd_arr_indx >= wb_info->_var##_num) {                    \
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit); \
            }                                                           \
                                                                        \
            _var##_state += sd_arr_indx;                                \
                                                                        \
            _var##_state->_var = arad_sw_db->_sw_db_var;                \
                                                                        \
                                                                        \
            data_size = sizeof(arad_wb_db_##_module##_##_var##_config_t); \
            data = (uint8 *)_var##_state;                               \
            offset = (uint32)(data - wb_info->scache_ptr);              \
                                                                        \
            break;                                                      \
                                                                        \
        default:                                                        \
            SOC_SAND_SET_ERROR_CODE(ARAD_WB_DB_UNSUPPORTED_SCACHE_DATA_VERSION_ERR, 20, exit); \
            break;                                                      \
    }                                                                   \
                                                                        \
    res = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset); \
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);                              \
                                                                        \
exit:                                                                   \
SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_wb_db_"#_module"_update_"#_var"_state()", 0, 0);

#define ARAD_WB_DB_UPDATE_DBL_ARR_CONTENT(_module, _var, _sw_db_var, _arr_iner_size, _indx1, _indx2) \
                                                                        \
    /* handle corner case of being invoked before the wb module is initialized      */ \
    /* (e.g. from egr_port_init().                                                      */ \
    /* default configuration either does not have to be saved or has to be handled  */ \
    /* in a special manner.                                                         */ \
    if (!(ARAD_WB_DB_INFO_INIT(unit))) {                                \
        SOC_SAND_EXIT_NO_ERROR;                                             \
    }                                                                   \
                                                                        \
    /* check if immediate sync not enabled  */                          \
    if (!(SOC_CONTROL(unit)->autosync)) {                               \
        /* mark dirty state. Currently no other processing done           */ \
        /* if required framework cache could be updated with updating the */ \
        /* persistent storage                                             */ \
        ARAD_WB_DB_DIRTY_BIT_SET(unit);                                 \
        SOC_SAND_EXIT_NO_ERROR;                                             \
    }                                                                   \
                                                                        \
    SOC_SCACHE_HANDLE_SET(wb_handle, unit, ARAD_WB_DB_MODULE, 0);       \
                                                                        \
    wb_info = ARAD_WB_DB_INFO(unit);                                    \
    arad_sw_db = wb_info->arad_sw_db;                                   \
                                                                        \
    switch(wb_info->version) {                                          \
        case ARAD_WB_DB_VERSION_1_0:                                    \
                                                                        \
            /* update framework cache and peristent storage                       */ \
            _var##_state = (arad_wb_db_##_module##_##_var##_config_t *)(wb_info->scache_ptr + wb_info->_var##_off); \
                                                                        \
                                                                        \
            sd_arr_indx = (_indx1 * _arr_iner_size) + _indx2;           \
                                                                        \
            if(sd_arr_indx >= wb_info->_var##_num ||                    \
                _indx2         >= _arr_iner_size) {                     \
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 10, exit); \
            }                                                           \
                                                                        \
            _var##_state += sd_arr_indx;                                \
                                                                        \
            _var##_state->_var = arad_sw_db->_sw_db_var;                \
                                                                        \
                                                                        \
            data_size = sizeof(arad_wb_db_##_module##_##_var##_config_t); \
            data = (uint8 *)_var##_state;                               \
            offset = (uint32)(data - wb_info->scache_ptr);              \
                                                                        \
            break;                                                      \
                                                                        \
        default:                                                        \
            SOC_SAND_SET_ERROR_CODE(ARAD_WB_DB_UNSUPPORTED_SCACHE_DATA_VERSION_ERR, 20, exit); \
            break;                                                      \
    }                                                                   \
                                                                        \
    res = soc_scache_commit_specific_data(unit, wb_handle, data_size, data, offset); \
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);                              \
                                                                        \
exit:                                                                   \
SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_wb_db_"#_module"_update_"#_var"_state()", 0, 0);













#define ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)         \
                                                            \
    arad_wb_db_info_t                  *wb_info;            \
    arad_wb_db_##_module##_##_var##_config_t *_var##_state; \
    ARAD_SW_DB_DEVICE                  *arad_sw_db;


#define ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var)        \
                                                                        \
    SOC_SAND_INIT_ERROR_DEFINITIONS(_function);                         \
                                                                        \
    wb_info = ARAD_WB_DB_INFO(unit);                                    \
    arad_sw_db = wb_info->arad_sw_db;                                   \
                                                                        \
    switch(wb_info->version) {                                          \
    case ARAD_WB_DB_VERSION_1_0:                                        \
                                                                        \
    _var##_state = (arad_wb_db_##_module##_##_var##_config_t *)(wb_info->scache_ptr + wb_info->_var##_off);


#define ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var)                   \
                                                                        \
    break;                                                              \
                                                                        \
    default:                                                            \
    SOC_SAND_SET_ERROR_CODE(ARAD_WB_DB_UNSUPPORTED_SCACHE_DATA_VERSION_ERR, 10, exit); \
    break;                                                              \
    }                                                                   \
                                                                        \
exit:                                                                   \
SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_wb_db_"#_module"_save_"#_var"_state()", 0, 0);



/* saving single var content */
#define ARAD_WB_DB_SAVE_SINGLE_STATE(_function, _module, _var, _sw_db_var) \
                                                                        \
    ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)                         \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var);           \
                                                                        \
    _var##_state->_var = arad_sw_db->_sw_db_var;                        \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var);


/* saving array content */
#define ARAD_WB_DB_SAVE_ARR_STATE(_function, _module, _var, _sw_db_var) \
                                                                        \
    int                                 arr_indx;                       \
    ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)                         \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var);           \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        _var##_state->_var = arad_sw_db->_sw_db_var;                    \
                                                                        \
    }                                                                   \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var);


/* saving double array content */
#define ARAD_WB_DB_SAVE_DBL_ARR_STATE(_function, _module, _var, _sw_db_var, _arr_iner_size) \
                                                                        \
    int                                 arr_indx;                       \
    uint32                              arr_indx1, arr_indx2;           \
    ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)                         \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var);           \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arr_indx1 = arr_indx / _arr_iner_size;                          \
        arr_indx2 = arr_indx % _arr_iner_size;                          \
                                                                        \
        _var##_state->_var = arad_sw_db->_sw_db_var;                    \
                                                                        \
    }                                                                   \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var);








#define ARAD_WB_DB_RESTORE_SINGLE_STATE(_function, _module, _var, _sw_db_var) \
                                                                        \
    ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)                         \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var);           \
                                                                        \
    arad_sw_db->_sw_db_var = _var##_state->_var;                        \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var);


/* saving array content */
#define ARAD_WB_DB_RESTORE_ARR_STATE(_function, _module, _var, _sw_db_var) \
                                                                        \
    int                                 arr_indx;                       \
    ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)                         \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var);           \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arad_sw_db->_sw_db_var = _var##_state->_var;                    \
                                                                        \
    }                                                                   \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var);


/* saving double array content */
#define ARAD_WB_DB_RESTORE_DBL_ARR_STATE(_function, _module, _var, _sw_db_var, _arr_iner_size) \
                                                                        \
    int                                 arr_indx;                       \
    uint32                              arr_indx1, arr_indx2;           \
    ARAD_WB_DB_GENERIC_INIT_DEFS(_module, _var)                         \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_PREFIX(_function, _module, _var);           \
                                                                        \
    for (arr_indx = 0; arr_indx < wb_info->_var##_num; arr_indx++, _var##_state++) { \
                                                                        \
        arr_indx1 = arr_indx / _arr_iner_size;                          \
        arr_indx2 = arr_indx % _arr_iner_size;                          \
                                                                        \
        arad_sw_db->_sw_db_var = _var##_state->_var;                    \
                                                                        \
    }                                                                   \
                                                                        \
    ARAD_WB_DB_GENERIC_FUNC_SUFFIX(_module, _var);






/*
 * local functions
 */

STATIC uint32
_arad_wb_db_layout_init(SOC_SAND_IN int unit, 
                        SOC_SAND_IN int version)
{
    uint32            res = SOC_SAND_OK;
    arad_wb_db_info_t  *wb_info; 
    int                 entry_size, total_size = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_WB_DB_LAYOUT_INIT);

    wb_info = ARAD_WB_DB_INFO(unit);

    wb_info->version = version;

    switch (version) {

        case ARAD_WB_DB_VERSION_1_0:

#define ARAD_WB_DB_LAYOUT_INIT_VARIABLE(_module, _var, _var_num)    \
            \
            entry_size = sizeof(arad_wb_db_##_module##_##_var##_config_t) * _var_num;\
            entry_size = (entry_size + 4) & ~(0x3); /* next offset at a word boundary */\
            wb_info->_var##_num = _var_num;\
            wb_info->_var##_off  = total_size;\
            total_size += entry_size;\


            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            port_priority_cal,                                             
                                            ARAD_WB_DB_EGR_PORT_QUEUE_RATE_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            tcg_cal, 
                                            ARAD_WB_DB_EGR_PORT_TCG_RATE_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            rates, 
                                            ARAD_WB_DB_EGR_PORT_RATES_NUM);
            
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            nof_calcal_instances, 
                                            ARAD_WB_DB_EGR_PORT_NOF_CALCAL_INSTANCES_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            calcal_length, 
                                            ARAD_WB_DB_EGR_PORT_CALCAL_LENGTH_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            update_device, 
                                            ARAD_WB_DB_EGR_PORT_UPDATE_DEVICE_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            egq_tcg_qpair_shaper_enable, 
                                            ARAD_WB_DB_EGR_PORT_EGQ_TCG_QPAIR_SHAPER_ENABLE_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            erp_interface, 
                                            ARAD_WB_DB_EGR_PORT_ERP_INTERFACE_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(egr_port, 
                                            ports_prog_editor_profile, 
                                            ARAD_WB_DB_EGR_PORT_PORTS_PROG_EDITOR_PROFILE_NUM);


            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(lag, 
                                            in_use, 
                                            ARAD_WB_DB_LAG_IN_USE_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(lag, 
                                            local_to_reassembly_context, 
                                            ARAD_WB_DB_LAG_LOCAL_TO_REASSEMBLY_CONTEXT_NUM);

            

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(cell, 
                                            current_cell_ident, 
                                            ARAD_WB_DB_CELL_CURRENT_CELL_IDENT_NUM);
   

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(tdm, 
                                            context_map, 
                                            ARAD_WB_DB_TDM_CONTEXT_MAP_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(dram, 
                                            dram_deleted_buff_list, 
                                            ARAD_WB_DB_DRAM_DELETED_BUFF_MAX_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(interrupts, 
                                            cmc_irq2_mask, 
                                            ARAD_WB_DB_INTERRUPTS_CMCS_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(interrupts, 
                                            cmc_irq3_mask, 
                                            ARAD_WB_DB_INTERRUPTS_CMCS_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(interrupts, 
                                            cmc_irq4_mask, 
                                            ARAD_WB_DB_INTERRUPTS_CMCS_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(interrupts, 
                                            interrupt_flags, 
                                            ARAD_WB_DB_INTERRUPTS_INTERRUPTS_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(interrupts, 
                                            interrupt_storm_timed_count, 
                                            ARAD_WB_DB_INTERRUPTS_INTERRUPTS_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(interrupts, 
                                            interrupt_storm_timed_period, 
                                            ARAD_WB_DB_INTERRUPTS_INTERRUPTS_NUM);

            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(multicast, 
                                            egress_groups_open_data, 
                                            (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids + 31)/ 32);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(qt, 
                                            q_type_ref_count, 
                                            ARAD_WB_DB_Q_TYPE_REF_COUNT_NUM(unit));
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(qt, 
                                            q_type_map, 
                                            ARAD_WB_DB__Q_TYPE_MAP_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(vsi, 
                                            vsi_to_isid, 
                                            ARAD_WB_DB_VSI_TO_ISID_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(tm, 
                                            queue_to_rate_class_mapping_is_simple, 
                                            ARAD_WB_DB_TM_IS_SIMPLE_Q_TO_RATE_CLS_MODE_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(tm, 
                                            queue_to_rate_class_mapping_ref_count, 
                                            ARAD_WB_DB_TM_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_NUM);
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(tm, 
                                            sysport2basequeue,
                                            ARAD_WB_DB_TM_SYSPORT2BASEQUEUE_COUNT_NUM(unit));
            ARAD_WB_DB_LAYOUT_INIT_VARIABLE(modport2sysport, 
                                            modport2sysport,
                                            ARAD_WB_DB_MODPORT2SYSPORT_COUNT_NUM(unit));
            wb_info->size = total_size;
            
            break;

        default:
            /* no other version supported */
            res = SOC_SAND_FAILURE_INTERNAL_ERR;
            break;
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in _arad_wb_db_layout_init()",0,0);
}


STATIC uint32
_arad_wb_db_egr_port_restore_queue_rate_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_QUEUE_RATE_STATE,
                                 egr_port, 
                                 port_priority_cal,
                                 arad_sw_db_egr_ports.port_priority_cal[arr_indx]);
}

STATIC uint32
_arad_wb_db_egr_port_restore_tcg_rate_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_TCG_RATE_STATE,
                                     egr_port, 
                                     tcg_cal,
                                     arad_sw_db_egr_ports.tcg_cal[arr_indx]);
}

STATIC uint32
_arad_wb_db_egr_port_restore_rates_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_DBL_ARR_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_RATES_STATE,
                                     egr_port, 
                                     rates,
                                     arad_sw_db_egr_ports.rates[arr_indx1][arr_indx2],
                                     ARAD_WB_DB_EGR_PORT_DBL_ARR_INER_SIZE_RATES);
}

STATIC uint32
_arad_wb_db_egr_port_restore_nof_calcal_instances_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_DBL_ARR_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_NOF_CALCAL_INSTANCES_STATE,
                                 egr_port, 
                                 nof_calcal_instances,
                                 arad_sw_db_egr_ports.chan_arb[arr_indx1][arr_indx2].nof_calcal_instances,
                                 SOC_DPP_DEFS_MAX(NOF_CORES));
}

STATIC uint32
_arad_wb_db_egr_port_restore_calcal_length_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_CALCAL_LENGTH_STATE,
                                    egr_port, 
                                    calcal_length,
                                    arad_sw_db_egr_ports.calcal_length[arr_indx]);
}

STATIC uint32
_arad_wb_db_egr_port_restore_egq_tcg_qpair_shaper_enable_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_SINGLE_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_EGQ_TCG_QPAIR_SHAPER_ENABLE_STATE,
                                 egr_port, 
                                 egq_tcg_qpair_shaper_enable,
                                 arad_sw_db_egr_ports.egq_tcg_qpair_shaper_enable);
}

STATIC uint32
_arad_wb_db_egr_port_restore_erp_interface_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_SINGLE_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_ERP_INTERFACE_STATE,
                                 egr_port, 
                                 erp_interface,
                                 arad_sw_db_egr_ports.erp_interface_id);
}

STATIC uint32
_arad_wb_db_egr_port_restore_ports_prog_editor_profile_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_EGR_PORT_RESTORE_PORTS_PROG_EDITOR_PROFILE_STATE,
                                 egr_port, 
                                 ports_prog_editor_profile,
                                 arad_sw_db_egr_ports.ports_prog_editor_profile[arr_indx]);
}

STATIC uint32
_arad_wb_db_tdm_restore_context_map_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_TDM_RESTORE_CONTEXT_MAP_STATE,
                                 tdm, 
                                 context_map,
                                 tdm.context_map[arr_indx]);
}


STATIC uint32
_arad_wb_db_lag_restore_local_to_reassembly_context_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_LAG_RESTORE_LOCAL_TO_REASSEMBLY_CONTEXT_STATE,
                                 lag, 
                                 local_to_reassembly_context,
                                 lag.local_to_reassembly_context[arr_indx]);
}

STATIC uint32
_arad_wb_db_lag_restore_in_use_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_LAG_RESTORE_IN_USE_STATE,
                                 lag, 
                                 in_use,
                                 lag.in_use[arr_indx]);
}


STATIC uint32
_arad_wb_db_cell_restore_current_cell_ident_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_SINGLE_STATE(ARAD_WB_DB_CELL_RESTORE_CURRENT_CELL_IDENT_STATE,
                                    cell, 
                                    current_cell_ident,
                                    cell.current_cell_ident);
}

STATIC uint32
_arad_wb_db_dram_restore_dram_deleted_buff_list_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 dram, 
                                 dram_deleted_buff_list, 
                                 dram.dram_deleted_buff_list[arr_indx]);

}

/* The following 3 functions aare not used and remain for future use. If we will need to use them in futer,
   They need to be static and called from inside the files. */
uint32
_arad_wb_db_interrupts_restore_cmc_irq2_mask_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 interrupts, 
                                 cmc_irq2_mask, 
                                 interrupts.cmc_irq2_mask[arr_indx]);
}
uint32
_arad_wb_db_interrupts_restore_cmc_irq3_mask_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 interrupts, 
                                 cmc_irq3_mask, 
                                 interrupts.cmc_irq3_mask[arr_indx]);
}
uint32
_arad_wb_db_interrupts_restore_cmc_irq4_mask_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 interrupts, 
                                 cmc_irq4_mask, 
                                 interrupts.cmc_irq4_mask[arr_indx]);
}

STATIC uint32
_arad_wb_db_interrupts_restore_interrupt_flags_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 interrupts, 
                                 interrupt_flags, 
                                 interrupts.interrupt_data[arr_indx].flags);
}
STATIC uint32
_arad_wb_db_interrupts_restore_interrupt_storm_timed_count_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 interrupts, 
                                 interrupt_storm_timed_count, 
                                 interrupts.interrupt_data[arr_indx].storm_timed_count);
}
STATIC uint32
_arad_wb_db_interrupts_restore_interrupt_storm_timed_period_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0, 
                                 interrupts, 
                                 interrupt_storm_timed_period, 
                                 interrupts.interrupt_data[arr_indx].storm_timed_period);
}

STATIC uint32
_arad_wb_db_multicast_restore_egress_groups_open_data_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_MULTICAST_RESTORE_EGRESS_GROUPS_OPEN_DATA_STATE,
                                 multicast, 
                                 egress_groups_open_data,
                                 arad_sw_db_multicast.egress_groups_open_data[arr_indx]);
}

STATIC uint32
_arad_wb_db_restore_q_type_ref_count_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0,
                                 qt, 
                                 q_type_ref_count,
                                 q_type_ref_count[arr_indx]);
}

STATIC uint32
_arad_wb_db__restore_q_type_map_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(0,
                                 qt, 
                                 q_type_map,
                                 q_type_map[arr_indx]);
}

STATIC uint32
_arad_wb_db_vsi_restore_isid_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_VSI_RESTORE_ISID_STATE,
                                 vsi, 
                                 vsi_to_isid,
                                 vsi.vsi_to_isid[arr_indx]);
}

STATIC uint32
_arad_sw_db_tm_restore_queue_to_rate_class_mapping_is_simple_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_SINGLE_STATE(ARAD_SW_DB_TM_RESTORE_IS_SIMPLE_Q_TO_RATE_CLS_MODE_STATE,
                                    tm, 
                                    queue_to_rate_class_mapping_is_simple,
                                    tm.queue_to_rate_class_mapping.is_simple_mode
                                    );
}

STATIC uint32
_arad_sw_db_tm_restore_queue_to_rate_class_mapping_ref_count_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_SW_DB_TM_RESTORE_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_STATE,
                                 tm, 
                                 queue_to_rate_class_mapping_ref_count,
                                 tm.queue_to_rate_class_mapping.ref_count[arr_indx]);
}

STATIC uint32 
_arad_sw_db_tm_restore_sysport2basequeue_state(SOC_SAND_IN int unit)
{
        ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_SW_DB_TM_RESTORE_SYSPORT_TO_BASEQUEUE_STATE,
                                         tm,
                                         sysport2basequeue,
                                         tm.sysport2basequeue[arr_indx]);

}

STATIC uint32 
_arad_wb_db_restore_modport2sysport_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_RESTORE_ARR_STATE(ARAD_WB_DB_TM_RESTORE_MODPORT_TO_SYSPORT_STATE,
                                     modport2sysport,
                                     modport2sysport,
                                     modport2sysport[arr_indx]);
}

STATIC uint32
_arad_wb_db_restore_state(SOC_SAND_IN int unit)
{
    uint32                 value;
    uint32                 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_WB_DB_RESTORE_STATE);

    res = _arad_wb_db_egr_port_restore_queue_rate_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = _arad_wb_db_egr_port_restore_tcg_rate_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = _arad_wb_db_egr_port_restore_rates_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = _arad_wb_db_egr_port_restore_nof_calcal_instances_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = _arad_wb_db_egr_port_restore_calcal_length_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = _arad_wb_db_egr_port_restore_egq_tcg_qpair_shaper_enable_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);

    res = _arad_wb_db_egr_port_restore_erp_interface_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);

    res = _arad_wb_db_egr_port_restore_ports_prog_editor_profile_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);



    res = _arad_wb_db_lag_restore_in_use_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    res = _arad_wb_db_lag_restore_local_to_reassembly_context_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
	
            

    res = _arad_wb_db_cell_restore_current_cell_ident_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  
    res = _arad_wb_db_dram_restore_dram_deleted_buff_list_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = _arad_wb_db_interrupts_restore_interrupt_flags_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    res = _arad_wb_db_interrupts_restore_interrupt_storm_timed_count_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    res = _arad_wb_db_interrupts_restore_interrupt_storm_timed_period_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    /* This part is different from the rest of the WB because the var saved in the WB is not exclusivly a part of dpp but also shared with other modules */
    res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_DPP_INTERRUPTS_BCM_SWITCH_EVENT_STORM_NOMINAL, &value);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    SOC_SWITCH_EVENT_NOMINAL_STORM(unit) = value;

    res = _arad_wb_db_tdm_restore_context_map_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    res = _arad_wb_db_multicast_restore_egress_groups_open_data_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    res = _arad_wb_db_restore_q_type_ref_count_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

    res = _arad_wb_db__restore_q_type_map_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

    res = _arad_wb_db_vsi_restore_isid_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

    res = _arad_sw_db_tm_restore_queue_to_rate_class_mapping_is_simple_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

    res = _arad_sw_db_tm_restore_queue_to_rate_class_mapping_ref_count_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

    if (!SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)){
        res = _arad_sw_db_tm_restore_sysport2basequeue_state(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
    }
    res = _arad_wb_db_restore_modport2sysport_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_wb_db_restore_state()", 0, 0);
}


STATIC uint32
_arad_wb_db_egr_port_save_queue_rate_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_EGR_PORT_SAVE_QUEUE_RATE_STATE,
                              egr_port, 
                              port_priority_cal, 
                              arad_sw_db_egr_ports.port_priority_cal[arr_indx]);
}

STATIC uint32
_arad_wb_db_egr_port_save_tcg_rate_state(SOC_SAND_IN int unit)
{    
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_EGR_PORT_SAVE_TCG_RATE_STATE,
                                  egr_port, 
                                  tcg_cal, 
                                  arad_sw_db_egr_ports.tcg_cal[arr_indx]);
}

STATIC uint32
_arad_wb_db_egr_port_save_rates_state(SOC_SAND_IN int unit)
{    
    ARAD_WB_DB_SAVE_DBL_ARR_STATE(ARAD_WB_DB_EGR_PORT_SAVE_RATES_STATE,
                                  egr_port, 
                                  rates, 
                                  arad_sw_db_egr_ports.rates[arr_indx1][arr_indx2],
                                  ARAD_WB_DB_EGR_PORT_DBL_ARR_INER_SIZE_RATES);
}

STATIC uint32
_arad_wb_db_egr_port_save_nof_calcal_instances_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_DBL_ARR_STATE(ARAD_WB_DB_EGR_PORT_SAVE_NOF_CALCAL_INSTANCES_STATE,
                              egr_port, 
                              nof_calcal_instances, 
                              arad_sw_db_egr_ports.chan_arb[arr_indx1][arr_indx2].nof_calcal_instances,
                              SOC_DPP_DEFS_MAX(NOF_CORES));
}

STATIC uint32
_arad_wb_db_egr_port_save_calcal_length_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_EGR_PORT_SAVE_CALCAL_LENGTH_STATE,
                                 egr_port, 
                                 calcal_length, 
                                 arad_sw_db_egr_ports.calcal_length[arr_indx]);
}

STATIC uint32
_arad_wb_db_egr_port_save_egq_tcg_qpair_shaper_enable_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_SINGLE_STATE(ARAD_WB_DB_EGR_PORT_SAVE_EGQ_TCG_QPAIR_SHAPER_ENABLE_STATE,
                              egr_port, 
                              egq_tcg_qpair_shaper_enable, 
                              arad_sw_db_egr_ports.egq_tcg_qpair_shaper_enable);
}

STATIC uint32
_arad_wb_db_egr_port_save_erp_interface_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_SINGLE_STATE(ARAD_WB_DB_EGR_PORT_SAVE_ERP_INTERFACE_STATE,
                              egr_port, 
                              erp_interface, 
                              arad_sw_db_egr_ports.erp_interface_id);
}

STATIC uint32
_arad_wb_db_egr_port_save_ports_prog_editor_profile_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_EGR_PORT_SAVE_PORTS_PROG_EDITOR_PROFILE_STATE,
                              egr_port, 
                              ports_prog_editor_profile, 
                              arad_sw_db_egr_ports.ports_prog_editor_profile[arr_indx]);
}

STATIC uint32
_arad_wb_db_lag_save_in_use_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_LAG_SAVE_IN_USE_STATE,
                              lag, 
                              in_use, 
                              lag.in_use[arr_indx]);
}


STATIC uint32
_arad_wb_db_lag_save_local_to_reassembly_context_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_LAG_SAVE_LOCAL_TO_REASSEMBLY_CONTEXT_STATE,
                              lag, 
                              local_to_reassembly_context, 
                              lag.local_to_reassembly_context[arr_indx]);
}

STATIC uint32
_arad_wb_db_tdm_save_context_map_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_TDM_SAVE_CONTEXT_MAP_STATE,
                              tdm, 
                              context_map, 
                              tdm.context_map[arr_indx]);
}


STATIC uint32
_arad_wb_db_cell_save_current_cell_ident_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_SINGLE_STATE(ARAD_WB_DB_CELL_SAVE_CURRENT_CELL_IDENT_STATE,
                                 cell, 
                                 current_cell_ident, 
                                 cell.current_cell_ident);
}

STATIC uint32
_arad_wb_db_dram_save_dram_deleted_buff_list_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 dram, 
                                 dram_deleted_buff_list, 
                                 dram.dram_deleted_buff_list[arr_indx]);
}

/* The following 3 functions aare not used and remain for future use. If we will need to use them in futer,
   They need to be static and called from inside the files. */
uint32
_arad_wb_db_interrupts_save_cmc_irq2_mask_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 interrupts, 
                                 cmc_irq2_mask, 
                                 interrupts.cmc_irq2_mask[arr_indx]);
}
uint32
_arad_wb_db_interrupts_save_cmc_irq3_mask_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 interrupts, 
                                 cmc_irq3_mask, 
                                 interrupts.cmc_irq3_mask[arr_indx]);
}
uint32
_arad_wb_db_interrupts_save_cmc_irq4_mask_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 interrupts, 
                                 cmc_irq4_mask, 
                                 interrupts.cmc_irq4_mask[arr_indx]);
}
STATIC uint32
_arad_wb_db_interrupts_save_interrupt_flags_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 interrupts, 
                                 interrupt_flags, 
                                 interrupts.interrupt_data[arr_indx].flags);
}
STATIC uint32
_arad_wb_db_interrupts_save_interrupt_storm_timed_count_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 interrupts, 
                                 interrupt_storm_timed_count, 
                                 interrupts.interrupt_data[arr_indx].storm_timed_count);
}
STATIC uint32
_arad_wb_db_interrupts_save_interrupt_storm_timed_period_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                                 interrupts, 
                                 interrupt_storm_timed_period, 
                                 interrupts.interrupt_data[arr_indx].storm_timed_period);
}

STATIC uint32
_arad_wb_db_multicast_save_egress_groups_open_data_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_MULTICAST_SAVE_EGRESS_GROUPS_OPEN_DATA_STATE,
                              multicast, 
                              egress_groups_open_data, 
                              arad_sw_db_multicast.egress_groups_open_data[arr_indx]);
}

STATIC uint32
_arad_wb_db_save_q_type_ref_count_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                              qt, 
                              q_type_ref_count, 
                              q_type_ref_count[arr_indx]);
}

STATIC uint32
_arad_wb_db__save_q_type_map_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(0,
                              qt, 
                              q_type_map, 
                              q_type_map[arr_indx]);
}

STATIC uint32
_arad_wb_db_vsi_save_isid_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_VSI_SAVE_ISID_STATE,
                              vsi, 
                              vsi_to_isid, 
                              vsi.vsi_to_isid[arr_indx]);
}
STATIC uint32
_arad_sw_db_tm_save_queue_to_rate_class_mapping_is_simple_state(SOC_SAND_IN uint32 unit)
{
    ARAD_WB_DB_SAVE_SINGLE_STATE(ARAD_SW_DB_TM_SAVE_IS_SIMPLE_Q_TO_RATE_CLS_MODE_STATE,
                                 tm, 
                                 queue_to_rate_class_mapping_is_simple,
                                 tm.queue_to_rate_class_mapping.is_simple_mode);
}

STATIC uint32
_arad_sw_db_tm_save_queue_to_rate_class_mapping_ref_count_state(SOC_SAND_IN uint32 unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_SW_DB_TM_SAVE_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_STATE,
                              tm,
                              queue_to_rate_class_mapping_ref_count,
                              tm.queue_to_rate_class_mapping.ref_count[arr_indx]);
}

STATIC uint32 
arad_sw_db_tm_save_sysport2basequeue_state(SOC_SAND_IN int unit)
{
        ARAD_WB_DB_SAVE_ARR_STATE(ARAD_SW_DB_TM_SAVE_SYSPORT_TO_BASEQUEUE_STATE,
                                 tm,
                                 sysport2basequeue,
                                 tm.sysport2basequeue[arr_indx]);
}

STATIC uint32 
_arad_wb_db_save_modport2sysport_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_SAVE_ARR_STATE(ARAD_WB_DB_TM_SAVE_MODPORT_TO_SYSPORT_STATE,
                             modport2sysport,
                             modport2sysport,
                             modport2sysport[arr_indx]);
}

STATIC uint32
_arad_wb_db_info_alloc(SOC_SAND_IN int unit)
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_WB_DB_INFO_ALLOC);

    if (_arad_wb_db_info_p[unit] == NULL) {
        _arad_wb_db_info_p[unit] = sal_alloc(sizeof(arad_wb_db_info_t), "wb egr_port");
        if (_arad_wb_db_info_p[unit] == NULL) {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
        }
    }

    sal_memset(_arad_wb_db_info_p[unit], 0x00, sizeof(arad_wb_db_info_t));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in _arad_wb_db_info_alloc()",0,0);
}


STATIC void
_arad_wb_db_info_dealloc(SOC_SAND_IN int unit)
{

    if (_arad_wb_db_info_p[unit] != NULL) {
        sal_free(_arad_wb_db_info_p[unit]);
        _arad_wb_db_info_p[unit] = NULL;
    }

}



/*
 * Global functions
 */

uint32
arad_wb_db_sync(SOC_SAND_IN int unit)
{
    uint32                 res = SOC_SAND_OK;
    arad_wb_db_info_t  *wb_info; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_WB_DB_SYNC);

    wb_info = ARAD_WB_DB_INFO(unit);

    /* check if there was any state change or a part of init sequence */
    if (!ARAD_WB_DB_IS_DIRTY(unit) && (wb_info->init_done == TRUE)) {
        SOC_SAND_EXIT_NO_ERROR;
    }

    res = _arad_wb_db_egr_port_save_queue_rate_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = _arad_wb_db_egr_port_save_tcg_rate_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = _arad_wb_db_egr_port_save_rates_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = _arad_wb_db_egr_port_save_nof_calcal_instances_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = _arad_wb_db_egr_port_save_calcal_length_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = _arad_wb_db_egr_port_save_egq_tcg_qpair_shaper_enable_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);

    res = _arad_wb_db_egr_port_save_erp_interface_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 83, exit);

    res = _arad_wb_db_egr_port_save_ports_prog_editor_profile_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);



    res = _arad_wb_db_lag_save_in_use_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    res = _arad_wb_db_lag_save_local_to_reassembly_context_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

            

    res = _arad_wb_db_cell_save_current_cell_ident_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = _arad_wb_db_dram_save_dram_deleted_buff_list_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = _arad_wb_db_interrupts_save_interrupt_flags_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    res = _arad_wb_db_interrupts_save_interrupt_storm_timed_count_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    res = _arad_wb_db_interrupts_save_interrupt_storm_timed_period_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

    res = _arad_wb_db_tdm_save_context_map_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

    res = _arad_wb_db_multicast_save_egress_groups_open_data_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    res = _arad_wb_db_save_q_type_ref_count_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

    res = _arad_wb_db__save_q_type_map_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

    res = _arad_wb_db_vsi_save_isid_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

    res = _arad_sw_db_tm_save_queue_to_rate_class_mapping_is_simple_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

    res = _arad_sw_db_tm_save_queue_to_rate_class_mapping_ref_count_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

    if (!SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)){
        res = arad_sw_db_tm_save_sysport2basequeue_state(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
    }
    res = _arad_wb_db_save_modport2sysport_state(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);

    ARAD_WB_DB_DIRTY_BIT_CLEAR(unit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in _arad_wb_db_sync()",0,0);
}

uint32
arad_wb_db_init(SOC_SAND_IN int unit, 
                SOC_SAND_IN ARAD_SW_DB_DEVICE *arad_sw_db)
{
    uint32                   res = SOC_SAND_OK;
    soc_error_t              rv;
    soc_scache_handle_t      wb_handle;
    int                      fegr_ports = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = ARAD_WB_DB_CURRENT_VERSION, recovered_ver;
    uint8                   *scache_ptr;
    arad_wb_db_info_t       *wb_info; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_WB_DB_INIT);

    SOC_SCACHE_HANDLE_SET(wb_handle, unit, ARAD_WB_DB_MODULE, 0);

    res = _arad_wb_db_info_alloc(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    wb_info = ARAD_WB_DB_INFO(unit);
    wb_info->arad_sw_db = (ARAD_SW_DB_DEVICE *)arad_sw_db;

    if (SOC_WARM_BOOT(unit)) {
        /* warmboot */
        rv = soc_warmboot_is_allowed_verify(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 15, exit);

        size = 0;
        rv = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetreive, fegr_ports,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 20, exit);

        /* layout corresponding to recovered version */
        res = _arad_wb_db_layout_init(unit, recovered_ver);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        wb_info->scache_ptr = scache_ptr;

        /* restore state */
        res = _arad_wb_db_restore_state(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        /* if version difference save current state */
        if (version != recovered_ver) {
            /* layout corresponding to current version */
            res = _arad_wb_db_layout_init(unit, version);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            size = wb_info->size;
            rv = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRealloc, fegr_ports,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 60, exit);

            wb_info->scache_ptr = scache_ptr;

            /* update persistent state */
            res = arad_wb_db_sync(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            /* writing to persistent storage initiated by bcm_init() */
            ARAD_UNIT_DIRTY_BIT_SET(unit);
        }
    }
    else {
        /* coldboot */

        rv = _soc_scache_sdk_version_set(unit);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 75, exit);

        /* layout corresponding to recovered version */
        res = _arad_wb_db_layout_init(unit, version);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

        size = wb_info->size;
        rv = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, fegr_ports,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 90, exit);

        wb_info->scache_ptr = scache_ptr;

        /* state should have been removed by Host. Stale State */
        res = arad_wb_db_sync(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        /* writing to persistent storage initiated by bcm_init() */
        ARAD_UNIT_DIRTY_BIT_SET(unit);
    }

    wb_info->init_done = TRUE;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in _arad_wb_db_init()",0,0);
}

void
arad_wb_db_deinit(SOC_SAND_IN int unit)
{

    _arad_wb_db_info_dealloc(unit);

}

uint32
arad_wb_db_egr_port_update_queue_rate_state(SOC_SAND_IN int unit,
                                            SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_QUEUE_RATE_STATE,
                                egr_port, 
                                port_priority_cal,
                                arad_sw_db_egr_ports.port_priority_cal[arr_indx],
                                arr_indx
                                );
}

uint32
arad_wb_db_egr_port_update_tcg_rate_state(SOC_SAND_IN int unit, 
                                          SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_TCG_RATE_STATE,
                                    egr_port, 
                                    tcg_cal,
                                    arad_sw_db_egr_ports.tcg_cal[arr_indx],
                                    arr_indx);
}

uint32
arad_wb_db_egr_port_update_rates_state(SOC_SAND_IN int unit, 
                                       SOC_SAND_IN uint32 arr_indx1, 
                                       SOC_SAND_IN uint32 arr_indx2)
{
    ARAD_WB_DB_UPDATE_DBL_ARR_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_RATES_STATE,
                                    egr_port, 
                                    rates,
                                    arad_sw_db_egr_ports.rates[arr_indx1][arr_indx2],
                                    ARAD_WB_DB_EGR_PORT_DBL_ARR_INER_SIZE_RATES,
                                    arr_indx1, 
                                    arr_indx2);
}

uint32
arad_wb_db_egr_port_update_nof_calcal_instances_state(SOC_SAND_IN int unit, 
                                                      SOC_SAND_IN uint32 arr_indx1,
                                                      SOC_SAND_IN uint32 arr_indx2)
{
    ARAD_WB_DB_UPDATE_DBL_ARR_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_NOF_CALCAL_INSTANCES_STATE,
                                egr_port, 
                                nof_calcal_instances,
                                arad_sw_db_egr_ports.chan_arb[arr_indx1][arr_indx2].nof_calcal_instances,
                                SOC_DPP_DEFS_MAX(NOF_CORES),
                                arr_indx1,
                                arr_indx2);
}

uint32
arad_wb_db_egr_port_update_calcal_length_state(SOC_SAND_IN int unit,
                                               SOC_SAND_IN int core)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_CALCAL_LENGTH_STATE,
                                   egr_port, 
                                   calcal_length,
                                   arad_sw_db_egr_ports.calcal_length[core],
                                   core);
}

uint32
arad_wb_db_egr_port_update_egq_tcg_qpair_shaper_enable_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_UPDATE_SINGLE_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_EGQ_TCG_QPAIR_SHAPER_ENABLE_STATE,
                                egr_port, 
                                egq_tcg_qpair_shaper_enable,
                                arad_sw_db_egr_ports.egq_tcg_qpair_shaper_enable);
}


uint32
arad_wb_db_egr_port_update_erp_interface_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_UPDATE_SINGLE_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_ERP_INTERFACE_STATE,
                                egr_port, 
                                erp_interface,
                                arad_sw_db_egr_ports.erp_interface_id);
}

uint32
arad_wb_db_egr_port_update_ports_prog_editor_profile_state(SOC_SAND_IN int unit, 
                                                           SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_EGR_PORT_UPDATE_PORTS_PROG_EDITOR_PROFILE_STATE,
                                egr_port, 
                                ports_prog_editor_profile,
                                arad_sw_db_egr_ports.ports_prog_editor_profile[arr_indx],
                                arr_indx);
}

uint32
arad_wb_db_lag_update_in_use_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_LAG_UPDATE_IN_USE_STATE,
                                lag, 
                                in_use,
                                lag.in_use[arr_indx],
                                arr_indx);
}

uint32
arad_wb_db_lag_update_local_to_reassembly_context_state(SOC_SAND_IN int unit, 
                                                        SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_LAG_UPDATE_LOCAL_TO_REASSEMBLY_CONTEXT_STATE,
                                lag, 
                                local_to_reassembly_context,
                                lag.local_to_reassembly_context[arr_indx],
                                arr_indx);
}

uint32
arad_wb_db_tdm_update_context_map_state(SOC_SAND_IN int unit, 
                                   SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_TDM_UPDATE_CONTEXT_MAP_STATE,
                                tdm, 
                                context_map,
                                tdm.context_map[arr_indx],
                                arr_indx);
}


uint32
arad_wb_db_cell_update_current_cell_ident_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_UPDATE_SINGLE_STATE(ARAD_WB_DB_CELL_UPDATE_CURRENT_CELL_IDENT_STATE,
                                   cell, 
                                   current_cell_ident,
                                   cell.current_cell_ident);
}


extern uint32
arad_wb_db_dram_update_dram_deleted_buff_list_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 indx)
{
     ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                dram, 
                                dram_deleted_buff_list,
                                dram.dram_deleted_buff_list[indx],
                                indx);
}


/* The following 3 functions are not used and remain for future use */
uint32
arad_wb_db_interrupts_update_cmc_irq2_mask_state(SOC_SAND_IN int unit, SOC_SAND_IN uint32 cmc)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                interrupts, 
                                cmc_irq2_mask,
                                interrupts.cmc_irq2_mask[cmc],
                                cmc);
}

uint32
arad_wb_db_interrupts_update_cmc_irq3_mask_state(SOC_SAND_IN int unit, SOC_SAND_IN uint32 cmc)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                interrupts, 
                                cmc_irq3_mask,
                                interrupts.cmc_irq3_mask[cmc],
                                cmc);
}

uint32
arad_wb_db_interrupts_update_cmc_irq4_mask_state(SOC_SAND_IN int unit, SOC_SAND_IN uint32 cmc)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                interrupts, 
                                cmc_irq4_mask,
                                interrupts.cmc_irq4_mask[cmc],
                                cmc);
}

uint32
arad_wb_db_interrupts_update_interrupt_flags_state(SOC_SAND_IN int unit, SOC_SAND_IN uint32 intr_id)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                   interrupts, 
                                   interrupt_flags,
                                   interrupts.interrupt_data[intr_id].flags,
                                   intr_id);
}

uint32
arad_wb_db_interrupts_update_interrupt_storm_timed_count_state(SOC_SAND_IN int unit, SOC_SAND_IN uint32 intr_id)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                interrupts, 
                                interrupt_storm_timed_count,
                                interrupts.interrupt_data[intr_id].storm_timed_count,
                                intr_id);
}

uint32
arad_wb_db_interrupts_update_interrupt_storm_timed_period_state(SOC_SAND_IN int unit, SOC_SAND_IN uint32 intr_id)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                interrupts, 
                                interrupt_storm_timed_period,
                                interrupts.interrupt_data[intr_id].storm_timed_period,
                                intr_id);
}

uint32
arad_wb_db_multicast_update_egress_groups_open_data_state(SOC_SAND_IN int unit, 
                                         SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_MULTICAST_UPDATE_EGRESS_GROUPS_OPEN_DATA_STATE,
                                multicast, 
                                egress_groups_open_data,
                                arad_sw_db_multicast.egress_groups_open_data[arr_indx],
                                arr_indx);
}

uint32
arad_wb_db_update_q_type_ref_count_state(SOC_SAND_IN int unit, 
                                         SOC_SAND_IN uint32 arr_indx)
{ 
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                qt, 
                                q_type_ref_count,
                                q_type_ref_count[arr_indx],
                                arr_indx);
}

uint32
arad_wb_db__update_q_type_map_state(SOC_SAND_IN int unit, 
                                         SOC_SAND_IN uint32 arr_indx)
{ 
    ARAD_WB_DB_UPDATE_ARR_STATE(0,
                                qt, 
                                q_type_map,
                                q_type_map[arr_indx],
                                arr_indx);
}

uint32
arad_wb_db_vsi_update_isid_state(SOC_SAND_IN int unit, 
                                         SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_VSI_UPDATE_ISID_STATE,
                                vsi, 
                                vsi_to_isid,
                                vsi.vsi_to_isid[arr_indx],
                                arr_indx);
}
uint32 
arad_sw_db_tm_update_queue_to_rate_class_mapping_is_simple_state(SOC_SAND_IN int unit)
{
    ARAD_WB_DB_UPDATE_SINGLE_STATE(ARAD_SW_DB_TM_UPDATE_IS_SIMPLE_Q_TO_RATE_CLS_MODE_STATE,
                                   tm, 
                                   queue_to_rate_class_mapping_is_simple,
                                   tm.queue_to_rate_class_mapping.is_simple_mode);
}

uint32 
arad_sw_db_tm_update_queue_to_rate_class_mapping_ref_count_state(SOC_SAND_IN int    unit, 
                                                                 SOC_SAND_IN uint32 arr_indx)

{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_SW_DB_TM_UPDATE_QUEUE_TO_RATE_CLASS_MAPPING_REF_COUNT_STATE,
                                tm, 
                                queue_to_rate_class_mapping_ref_count,
                                tm.queue_to_rate_class_mapping.ref_count[arr_indx],
                                arr_indx);
}

uint32 
arad_sw_db_tm_update_sysport2basequeue_state(SOC_SAND_IN int unit, 
                                             SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_SW_DB_TM_UPDATE_SYSPORT_TO_BASEQUEUE_STATE,
                               tm,
                               sysport2basequeue,
                               tm.sysport2basequeue[arr_indx],
                               arr_indx);
}

uint32 
arad_wb_db_update_modport2sysport_state(SOC_SAND_IN int unit, 
                                        SOC_SAND_IN uint32 arr_indx)
{
    ARAD_WB_DB_UPDATE_ARR_STATE(ARAD_WB_DB_TM_UPDATE_MODPORT_TO_SYSPORT_STATE,
                                modport2sysport,
                                modport2sysport,
                                modport2sysport[arr_indx],
                                arr_indx);
}

#endif /* BCM_WARM_BOOT_SUPPORT */


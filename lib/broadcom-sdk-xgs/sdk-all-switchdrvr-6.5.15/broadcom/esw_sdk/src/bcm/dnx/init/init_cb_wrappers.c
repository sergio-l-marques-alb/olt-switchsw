/** \file init_cb_wrappers.c
 * DNX init sequence CB wrapper functions.
 *
 * The DNX init sequence uses CB functions for init and deinit steps. new functions are written according the required
 * definitions, old ones however are wrapped and placed in this file.
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

#define ERPP_MAPPING_INLIF_PROFILE_SIZE 256
#define ERPP_MAPPING_OUTLIF_PROFILE_SIZE 16
#define ERPP_FILTER_PER_FWD_CONTEXT_SIZE 64
#define ERPP_FILTER_PER_PORT_SIZE  256
#define FWD_CONTEXT_ETHERNET 6

#include "init_cb_wrappers.h"

#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/rx/rx.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/drv.h>
#include <soc/counter.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/pemladrv/pemladrv.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/common/tx.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/common/family.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>


#define FIELD_0_5_DEFAULT 61
#define FIELD_6_6_DEFAULT 1
#define FIELD_7_11_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_MPLS
#define FIELD_12_16_DEFAULT DBAL_ENUM_FVAL_LAYER_TYPES_IPV4
#define FIELD_7_16_DEFAULT (FIELD_7_11_DEFAULT) | (FIELD_12_16_DEFAULT << 5)
#define FIELD_17_24_DEFAULT 0xFF
#define FIELD_25_27_DEFAULT 6
#define FIELD_28_49_DEFAULT 0
#define FIELD_50_51_DEFAULT 2
#define FIELD_52_69_DEFAULT 0
#define FIELD_70_72_DEFAULT 4
#define FIELD_73_80_DEFAULT 1
#define FIELD_81_82_DEFAULT 3
#define FIELD_83_85_DEFAULT 0
#define FIELD_86_88_DEFAULT 4
#define FIELD_89_92_DEFAULT 7
#define FIELD_93_94_DEFAULT 2
#define FIELD_95_97_DEFAULT 0
#define FIELD_98_99_DEFAULT 3
#define FIELD_100_102_DEFAULT 0
#define BYPASS_STAMP_FTMH_ENABLE_DEFAULT 0
#define IPPF_REG_03A0_FIELD_17_17_DEFAULT 0
#define VTT_ST_TWO_LE_SHARED_PDS_STAGE_SELECTORS_DEFAULT 2
#define ITPP_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe
#define ITPPD_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT 0x3ffffe
/*
 * }
 */

/*
 * ENUMs
 * {
 */

/*
 * }
 */

extern int _bcm_rx_shutdown(
    int unit);

shr_error_e
dnx_init_mib_thread_start(
    int unit)
{
    uint32 interval;
    soc_control_t *soc = SOC_CONTROL(unit);
    SHR_FUNC_INIT_VARS(unit);
    /*
     * start counter
     */
    interval = dnx_data_mib.general.stat_interval_get(unit);

    if (interval > 0 && interval < ((SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8) && (SAL_BOOT_PLISIM))
    {
        interval = (SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8;
    }
    if (SOC_WARM_BOOT(unit))
    {
        /** Allow writing to registers for the counter start time */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_DBAL));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, DNXC_HA_ALLOW_SCHAN));
    }

    SHR_IF_ERR_EXIT(soc_counter_start(unit, 0, interval, soc->counter_pbmp));

    if (SOC_WARM_BOOT(unit))
    {
        /** Return to warmboot normal mode */
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_SCHAN));
        SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, DNXC_HA_ALLOW_DBAL));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_mib_thread_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_counter_stop(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_done_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(adapter_dnx_init_done(unit));
#endif
    /*
     * Notify DNX DATA -  init done
     */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_set(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

    /*
     * Notify DBAL -  init done
     */
    SHR_IF_ERR_EXIT(dbal_device_init_done(unit));

    /*
     * Mark soc control as done
     */
    SOC_CONTROL(unit)->soc_flags |= SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags |= SOC_F_ALL_MODULES_INITED;
    /*
     * start counter
     */
    SHR_IF_ERR_EXIT(dnx_init_mib_thread_start(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_done_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Stop counter thread
     */
    SHR_IF_ERR_EXIT(dnx_init_mib_thread_stop(unit));

    /*
     * Update DNXC DATA module about the state.
     */
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_state_reset(unit, DNXC_DATA_STATE_F_BCM_INIT_DONE));

    /*
     * declare that driver is detaching
     */
    SOC_DETACH(unit, 1);

    /*
     * Mark soc control as not done
     */
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_INITED;
    SOC_CONTROL(unit)->soc_flags &= ~SOC_F_ALL_MODULES_INITED;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_info_config_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_dnx_info_config(unit));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_info_config_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NONE, "place holder for actual deinit code\n%s%s%s", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_feature_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    soc_feature_init(unit);

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_feature_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_NONE, "place holder for actual deinit code\n%s%s%s", EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_family_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_chip_family_set(unit, BCM_FAMILY_DNX));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_family_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    /** do nothing */
    SHR_FUNC_EXIT;
}

#ifdef ADAPTER_SERVER_MODE
shr_error_e
dnx_init_adapter_reg_access_init(
    int unit)
{
    soc_control_t *soc;

    SHR_FUNC_INIT_VARS(unit);

    soc = SOC_CONTROL(unit);

    /*
     * do not enable memscan task, otherwise tr 7 will segmentation fail
     */
    soc->mem_scan_pid = SAL_THREAD_ERROR;
    soc->mem_scan_interval = 0;

    SHR_IF_ERR_EXIT(adapter_reg_access_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_adapter_reg_access_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_reg_access_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dnx_init_rx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_bcm_common_rx_init(unit));

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dnx_rx_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_rx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_stop(unit, NULL));
    /*
     * free up the rx pool
     */
    SHR_IF_ERR_EXIT(bcm_rx_pool_cleanup());

    /*
     * clear rx info
     */
    SHR_IF_ERR_EXIT(_bcm_rx_shutdown(unit));

    /*
     * The respective Deinit functions need to be put here. Currently there is no need, since no memory is allocated at init.
     */
    SHR_IF_ERR_EXIT(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_tx_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_common_tx_init(unit));

    /*
     * JR2 and above - CPU channel derived from module header (if exists), otherwise 0
     */
    if (!SOC_WARM_BOOT(unit))
    {
        uint32 reg32_val;
        SHR_IF_ERR_EXIT(READ_ECI_CPU_CHANNEL_CONFIGURATIONr(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_CPU_CHANNEL_CONFIGURATIONr, &reg32_val, CPU_CHANNEL_SELECTf, 0x2);
        SHR_IF_ERR_EXIT(WRITE_ECI_CPU_CHANNEL_CONFIGURATIONr(unit, reg32_val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_tx_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(bcm_common_tx_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_hw_overwrite_init(
    int unit)
{
    uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32] = { 0 };
    uint32 field_data[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32 reg_val;

#ifdef ADAPTER_SERVER_MODE
    int egr_if = 2;
    int nif_port = 63;
#endif
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * OAM overwrites
     */

#ifdef ADAPTER_SERVER_MODE
/* { */
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        bcm_core_t core_id;

        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        core_id = DBAL_CORE_ALL;
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        /*
         * Work Around for OAMP - Map EGQ interface of OAMP port to NIF port number
         * This WA is needed for C-model only
         */
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_NUM, INST_SINGLE, (uint32) (nif_port));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * OLP overwrites
         *
         * Work Around for OLP - Map EGQ interface of OLP port to NIF port number
         * This WA is needed for C-model only
         */
        egr_if = 3;
        nif_port = 61;
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        /*
         * Is that redundant??
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_NUM, INST_SINGLE, (uint32) (nif_port));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * RCY overwrites
         *
         * Work Around for OLP - Map EGQ interface of recycle port to NIF port number
         * This WA is needed for C-model only
         */
        egr_if = 4;
        nif_port = 62;
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        /*
         * Is that redundant??
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_NUM, INST_SINGLE, (uint32) (nif_port));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
/* } */
#endif


    sal_memset(field_data, 0, sizeof(field_data));
    sal_memset(data, 0x0, sizeof(data));


    field_data[0] = FIELD_0_5_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_0_5f, field_data);


    field_data[0] = FIELD_6_6_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_6_6f, field_data);


    field_data[0] = FIELD_7_16_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_7_16f, field_data);


    field_data[0] = FIELD_17_24_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_17_24f, field_data);


    field_data[0] = FIELD_25_27_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_25_27f, field_data);


    field_data[0] = FIELD_28_49_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_28_49f, field_data);


    field_data[0] = FIELD_50_51_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_50_51f, field_data);


    field_data[0] = FIELD_52_69_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_52_69f, field_data);


    field_data[0] = FIELD_70_72_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_70_72f, field_data);


    field_data[0] = FIELD_73_80_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_73_80f, field_data);


    field_data[0] = FIELD_81_82_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_81_82f, field_data);


    field_data[0] = FIELD_83_85_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_83_85f, field_data);


    field_data[0] = FIELD_86_88_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_86_88f, field_data);


    field_data[0] = FIELD_89_92_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_89_92f, field_data);


    field_data[0] = FIELD_93_94_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_93_94f, field_data);


    field_data[0] = FIELD_95_97_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_95_97f, field_data);


    field_data[0] = FIELD_98_99_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_98_99f, field_data);


    field_data[0] = FIELD_100_102_DEFAULT;
    soc_reg_above_64_field_set(unit, OAMP_REG_0129r, data, FIELD_100_102f, field_data);

    SHR_IF_ERR_EXIT(WRITE_OAMP_REG_0129r(unit, data));

    
    /** Have ETPP not stamp the packets */
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, ETPPA_BYPASS_STAMP_FTMH_ENABLEr, SOC_CORE_ALL, 0, &reg_val));
    soc_reg_field_set(unit, ETPPA_BYPASS_STAMP_FTMH_ENABLEr, &reg_val, BYPASS_STAMP_FTMH_ENABLEf,
                      BYPASS_STAMP_FTMH_ENABLE_DEFAULT);
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, ETPPA_BYPASS_STAMP_FTMH_ENABLEr, SOC_CORE_ALL, 0, reg_val));

    
    
    soc_reg_above_64_field32_modify(unit, ITPP_ITPP_GENERAL_CFGr, SOC_CORE_ALL, 0, PD_CUD_INVALID_VALUEf,
                                    ITPP_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT);
    soc_reg_above_64_field32_modify(unit, ITPPD_ITPP_GENERAL_CFGr, SOC_CORE_ALL, 0, PD_CUD_INVALID_VALUEf,
                                    ITPPD_ITPP_GENERAL_CFG_PD_CUD_INVALID_VALUE_DEFAULT);
    
    {
        uint32 entry_handle_id;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_IRPP_FWD1_CONTEXT_PROPERTIES, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD1_CONTEXT_ID, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARNING_FIXED_KEY_ENABLE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    {
        soc_reg_above_64_val_t data = { 0 };
        data[0] = 0xfffffffc;
        soc_reg_above_64_set(unit, IPPB_RESERVED_SPARE_1r, REG_PORT_ANY, 0, data);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_init_hw_overwrite_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The respective DeInit functions need to be put here.
     */
    SHR_FUNC_EXIT;
}

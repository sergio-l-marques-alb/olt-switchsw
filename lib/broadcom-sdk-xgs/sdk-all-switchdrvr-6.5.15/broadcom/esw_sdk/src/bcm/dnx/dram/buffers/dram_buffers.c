/** \file src/bcm/dnx/dram/buffers/dram_buffers.c
 *
 *
 *  This file contains implementation of functions for dram buffers
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_DRAM

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/dram/buffers/dram_buffers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_dram_access.h>
#include "buffers_quarantine_mechanism.h"

/*
 * }
 */

/*
 * defines
 * {
 */

/*
 * }
 */

/** see header file */
shr_error_e
dnx_dram_buffers_init(
    int unit)
{
    uint32 nof_bdbs_per_bank;
    int nof_hbms;
    int nof_enabled_hbms = 0;
    uint32 entry_handle_id;
    uint32 dram_bitmap[1];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** create stuff */
    SHR_IF_ERR_EXIT(dnx_dram_db.sync_manager.deleted_buffers_file_mutex.create(unit));

    /** this is done to prevent coverity issue of ARRAY_VS_SINGLETON from occuring */
    dram_bitmap[0] = dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap;

    /** get number of enabled HBMs */
    nof_hbms = dnx_data_dram.hbm.nof_hbms_get(unit);
    shr_bitop_range_count(dram_bitmap, 0, nof_hbms, &nof_enabled_hbms);

    /** get number of BDBs per bank */
    nof_bdbs_per_bank =
        (dnx_data_dram.buffers.nof_bdbs_get(unit) / nof_hbms * nof_enabled_hbms) /
        dnx_data_dram.buffers.nof_fpc_banks_get(unit);

    /** set BDBs ranges for each bank */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDM_FPC_CONFIG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_FIRST_PTR, INST_ALL, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_LAST_PTR, INST_ALL,
                                 (nof_bdbs_per_bank == 0) ? nof_bdbs_per_bank : nof_bdbs_per_bank - 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** set FPC init */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDM_INIT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FPC_INIT, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** poll for FPC and BDM init done */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDM_INIT, entry_handle_id));
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_FPC_INIT, 0));
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, 1000000, 1000000, entry_handle_id, DBAL_FIELD_BDM_INIT_STATUS, 0));

    /** enable BDBs allocation */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_BDB_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BDB_ALLOCATE_EN, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** enable read and write data path */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_DDP_GENERAL_CONFIGURATION, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WR_DATA_PATH_EN, INST_ALL, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RD_DATA_PATH_EN, INST_ALL, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** init buffers quarantine mechanism */
    SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_dram_buffers_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dram_buffers_status_get(
    int unit,
    int core,
    uint8 deleted,
    uint32 *buffers_array,
    int max_size,
    int *size)
{
    uint32 entry_handle_id;
    uint32 core_rule_value = core;
    uint32 error_counter_rule_value;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set size to 0 */
    *size = 0;

    /** Taking a handle */
    DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BDB_ERRORS_COUNTER, &entry_handle_id);
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /** Add Key rule to hit only relevant core */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                                           DBAL_CONDITION_EQUAL_TO, &core_rule_value, NULL));
    if (deleted)
    {
        /** Add value rule to hit only deleted and about to be deleted buffers */
        error_counter_rule_value = dnx_data_dram.buffers.allowed_errors_get(unit);
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER,
                                                                 INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                                                                 &error_counter_rule_value, NULL));
    }
    else
    {
        /** Add value rule to hit quarantined buffers */
        error_counter_rule_value = 0;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add(unit, entry_handle_id, DBAL_FIELD_ERROR_COUNTER,
                                                                 INST_SINGLE, DBAL_CONDITION_BIGGER_THAN,
                                                                 &error_counter_rule_value, NULL));
    }
    /** fire up first iteration */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /** run iterator */
    while (!is_end)
    {
        if (*size < max_size)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_BDB_INDEX, &(buffers_array[*size])));
        }
        ++(*size);
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

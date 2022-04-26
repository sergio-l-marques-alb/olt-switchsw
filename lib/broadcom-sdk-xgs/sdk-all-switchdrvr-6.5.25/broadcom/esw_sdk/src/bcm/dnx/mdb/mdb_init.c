
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */


#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/types/mdb_types.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sbusdma_desc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/drv.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include "mdb_init_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX






#define MDB_INIT_DIRECT_PHYSICAL_ADDRESS_MAX_VAL      (5)



#define MDB_INIT_CUCKOO_WATCHDOG_STEPS_DEF_VALS  (0x0)



#define MDB_INIT_EM_DFG_PERIOD_VAL               (0x40000000)


#define MDB_INIT_MAX_NOF_MDB_TABLES_PER_DBAL          (3)



#define MDB_INIT_MAX_NOF_TABLES_RESOURCE_SHARE        (2)



#define MDB_INIT_EM_FORMAT_MASK_SHIFT                  (2)



#define MDB_INIT_EM_FORMAT_OFFSET_MASK                 (0x3)



#define MDB_EM_DEFAULT_AGE_PROFILE                      (0)
#define MDB_EM_MACT_AGE_PROFILE                         (1)



#define MDB_EEDB_POINTER_FORMAT_17BIT_RES_1k     (0)
#define MDB_EEDB_POINTER_FORMAT_18BIT_RES_1k     (1)
#define MDB_EEDB_POINTER_FORMAT_19BIT_RES_2k     (2)
#define MDB_EEDB_POINTER_FORMAT_20BIT_RES_4k     (3)


#define MDB_NOF_PHASES_PER_EEDB_BANK                    (3)
#define MDB_NOF_PHASES_PER_EEDB_BANK_BITS               (2)
#define MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS     (1)
#define MDB_PHASE_BANK_SELECT_DEFAULT                   (0)









typedef enum
{
    MDB_INIT_CLUSTER_ADD,
    MDB_INIT_CLUSTER_DELETE,

    MDB_NOF_INIT_CLUSTER_OPERATIONS
} mdb_init_cluster_func_e;










shr_error_e
mdb_init_fec_ranges_allocation_info_get(
    int unit,
    mdb_physical_table_e mdb_fec_physical_table_id,
    uint32 *range_start,
    uint32 *range_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!MDB_INIT_IS_FEC_TABLE(mdb_fec_physical_table_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not a valid database, must be FEC database %d .\n", mdb_fec_physical_table_id);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_id_range_start.get(unit,
                                                                    dnx_data_l3.fec.fec_tables_info_get(unit,
                                                                                                        mdb_fec_physical_table_id)->index,
                                                                    range_start));

    SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_id_range_size.get(unit,
                                                                   dnx_data_l3.fec.fec_tables_info_get(unit,
                                                                                                       mdb_fec_physical_table_id)->index,
                                                                   range_size));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_init_fec_ranges_allocation_info_set(
    int unit,
    mdb_physical_table_e mdb_fec_physical_table_id,
    uint32 range_start,
    uint32 range_size)
{

    SHR_FUNC_INIT_VARS(unit);

    if (!MDB_INIT_IS_FEC_TABLE(mdb_fec_physical_table_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Not a valid database, must be FEC database %d .\n", mdb_fec_physical_table_id);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_id_range_start.set(unit,
                                                                    dnx_data_l3.fec.fec_tables_info_get(unit,
                                                                                                        mdb_fec_physical_table_id)->index,
                                                                    range_start));

    SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_id_range_size.set(unit,
                                                                   dnx_data_l3.fec.fec_tables_info_get(unit,
                                                                                                       mdb_fec_physical_table_id)->index,
                                                                   range_size));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_init_em_age_nof_bits_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_em_entry_encoding_e entry_encoding,
    int *nof_age_bits)
{
    uint32 entry_handle_mdb_24_table;

    

    uint32 age_cnt_em_row_width;

    

    uint32 age_cnt_em_entry_width;
    uint8 max_nof_age_entry_bits;
    mdb_physical_table_e mdb_physical_table =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit,
                     dnx_data_mdb.em.mdb_24_info_get(unit, mdb_physical_table)->dbal_table,
                     &entry_handle_mdb_24_table));

    dbal_value_field32_request(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE,
                               &age_cnt_em_row_width);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_24_table, DBAL_COMMIT));

    max_nof_age_entry_bits = dnx_data_mdb.em.em_aging_info_get(unit, dbal_physical_table_id)->max_nof_age_entry_bits;

    

    age_cnt_em_entry_width = age_cnt_em_row_width / (1 << entry_encoding);
    age_cnt_em_entry_width =
        (age_cnt_em_entry_width > max_nof_age_entry_bits) ? max_nof_age_entry_bits : age_cnt_em_entry_width;

    *nof_age_bits = age_cnt_em_entry_width;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_em_age_emp(
    int unit)
{
    uint32 entry_handle_table_emp;
    uint32 entry_handle_table_21;
    uint32 entry_handle_table_22;
    uint32 entry_handle_table_24;
    uint32 nof_supported_age_ratios;
    int emp_table_idx, age_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);



    nof_supported_age_ratios =
        dnx_data_mdb.em.age_profile_per_ratio_support_get(unit) ? MDB_STEP_TABLE_NOF_ENTRY_SIZES : 1;

    for (emp_table_idx = 0; emp_table_idx < dnx_data_mdb.em.nof_emp_tables_get(unit); emp_table_idx++)
    {
        dbal_physical_tables_e dbal_physical_table_id;
        mdb_physical_table_e mdb_physical_table_id;

        dbal_physical_table_id = dnx_data_mdb.em.emp_index_table_mapping_get(unit, emp_table_idx)->dbal_id;
        mdb_physical_table_id = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit,
                         dnx_data_mdb.em.mdb_emp_tables_mapping_get(unit, mdb_physical_table_id)->emp_age_cfg_table,
                         &entry_handle_table_emp));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_21_info_get(unit, mdb_physical_table_id)->dbal_table,
                         &entry_handle_table_21));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_emp_tables_mapping_get(unit, mdb_physical_table_id)->mdb_22_table,
                         &entry_handle_table_22));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_24_info_get(unit, mdb_physical_table_id)->dbal_table,
                         &entry_handle_table_24));

        

        for (age_profile = 0; age_profile < dnx_data_mdb.em.nof_aging_profiles_get(unit); age_profile++)
        {
            uint32 ratio;
            uint32 instance = age_profile;

            

            for (ratio = 0; ratio < nof_supported_age_ratios; ratio++)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                                (unit,
                                 dnx_data_mdb.em.mdb_emp_tables_mapping_get(unit,
                                                                            mdb_physical_table_id)->emp_age_cfg_table,
                                 entry_handle_table_emp));

                if (dnx_data_mdb.em.age_profile_per_ratio_support_get(unit))
                {
                    dbal_entry_key_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_KEY_0,
                                               (age_profile * nof_supported_age_ratios) + ratio);
                    instance = INST_SINGLE;
                }

                dbal_entry_value_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_ITEM_0, instance,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->elephant_values);
                dbal_entry_value_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_ITEM_1, instance,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->mouse_values);
                dbal_entry_value_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_ITEM_2, instance,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->age_max_values);
                dbal_entry_value_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_ITEM_3, instance,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->increment_values);
                dbal_entry_value_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_ITEM_4, instance,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->decrement_values);
                dbal_entry_value_field32_set(unit, entry_handle_table_emp, DBAL_FIELD_MDB_ITEM_5, instance,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->out_values);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_emp, DBAL_COMMIT));

                dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_0,
                                             (age_profile * nof_supported_age_ratios) + ratio,
                                             dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                              dbal_physical_table_id)->init_value);

            }
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_1, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          dbal_physical_table_id)->global_value);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_2, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          dbal_physical_table_id)->global_mask);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_3, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          dbal_physical_table_id)->external_profile);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_4, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          dbal_physical_table_id)->aging_disable);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_5, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          dbal_physical_table_id)->elephant_disable);

            dbal_entry_value_field32_set(unit, entry_handle_table_24, DBAL_FIELD_MDB_ITEM_2, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          dbal_physical_table_id)->hit_bit_mode);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_21, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_24, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_table_emp);
        DBAL_HANDLE_FREE(unit, entry_handle_table_21);
        DBAL_HANDLE_FREE(unit, entry_handle_table_24);

        

        dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, 0);
        if (dnx_data_mdb.em.scan_bank_participate_in_cuckoo_partial_support_get(unit))
        {
            int inaccurate_mode;
            

            if (dnx_data_mdb.em.scan_bank_participate_in_cuckoo_support_get(unit))
            {
                inaccurate_mode = TRUE;
            }
            else
            {
                inaccurate_mode = FALSE;
            }
            dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE,
                                         inaccurate_mode);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_22, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_table_22);

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_verify_tables(
    int unit)
{
    dbal_tables_e db_index;
    dbal_enum_value_field_mdb_db_type_e mdb_db_type;

    SHR_FUNC_INIT_VARS(unit);

    for (db_index = 0; db_index < DBAL_NOF_TABLES; db_index++)
    {
        uint32 key_size;
        int payload_size;
        const dbal_logical_table_t *dbal_logical_table;
        int is_table_active;
        uint8 is_mdb;

        SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, db_index, &is_table_active));

        if (!is_table_active)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, db_index, &dbal_logical_table));

        

        SHR_IF_ERR_EXIT(mdb_dbal_table_is_mdb(unit, db_index, &is_mdb));
        if ((is_mdb == FALSE)
            || (dbal_logical_table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_TCAM)
            || (dbal_logical_table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_PPMC))
        {
            continue;
        }

        mdb_db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->db_type;

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            int key_size_int, payload_size_int;

            SHR_IF_ERR_EXIT(dbal_tables_sizes_get(unit, db_index, &key_size_int, &payload_size_int));
            SHR_IF_ERR_EXIT(mdb_em_get_key_size
                            (unit, dbal_logical_table->physical_db_id[0], dbal_logical_table->app_id, &key_size));
            payload_size = payload_size_int;
        }
        else
        {
            key_size = dbal_logical_table->key_size;
            payload_size = dbal_logical_table->max_payload_size;
        }

        if (dbal_logical_table->core_mode == DBAL_CORE_MODE_DPC)
        {
            key_size -= dbal_logical_table->core_id_nof_bits;
        }
        if (SHR_IS_BITSET(dbal_logical_table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
        {
            payload_size -= dbal_logical_table->multi_res_info[0].results_info[0].field_nof_bits;
        }

        

        if (key_size > dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size)
        {
            LOG_CLI((BSL_META("%s: key_size(%d) is larger than allowed by the pipeline(%d).\n"),
                     dbal_logical_table->table_name, key_size,
                     dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size));
        }

        if (payload_size >
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_payload_size)
        {
            LOG_CLI((BSL_META("%s: payload_size(%d) is larger than allowed by the pipeline(%d).\n"),
                     dbal_logical_table->table_name, payload_size,
                     dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                         dbal_logical_table->physical_db_id[0])->max_payload_size));
        }

        

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM)
        {
            continue;
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
        {

            

            if (key_size > dnx_data_mdb.kaps.key_width_in_bits_get(unit))
            {
                LOG_CLI((BSL_META
                         ("%s: KAPS key is too large. key_size(%d) > dnx_data_mdb.kaps.key_width_in_bits_get(unit)(%d).\n"),
                         dbal_logical_table->table_name, key_size, dnx_data_mdb.kaps.key_width_in_bits_get(unit)));
            }

            if (payload_size > mdb_lpm_ad_size_enum_to_bits[MDB_NOF_LPM_AD_SIZES - 1])
            {
                LOG_CLI((BSL_META
                         ("%s: KAPS payload is too large. payload_size(%d) > max_payload_size(%d).\n"),
                         dbal_logical_table->table_name, payload_size,
                         mdb_lpm_ad_size_enum_to_bits[MDB_NOF_LPM_AD_SIZES - 1]));
            }
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            uint32 configured_key_size;
            uint32 addr_bits;

            

             
            SHR_IF_ERR_EXIT(mdb_em_get_key_size
                            (unit, dbal_logical_table->physical_db_id[0], dbal_logical_table->app_id,
                             &configured_key_size));
            if (key_size > configured_key_size)
            {
                LOG_CLI((BSL_META("%s: key_size(%d) is larger than configured during init (%d).\n"),
                         dbal_logical_table->table_name, key_size, configured_key_size));
            }

            

            if (key_size + dbal_logical_table->app_id_size >
                dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size)
            {
                LOG_CLI((BSL_META("%s: key_size(%d) + app_id_size(%d) > pipeline_max_key_size(%d).\n"),
                         dbal_logical_table->table_name, key_size, dbal_logical_table->app_id_size,
                         dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                             dbal_logical_table->physical_db_id[0])->max_key_size));
            }

            

            SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, dbal_logical_table->physical_db_id[0], &addr_bits));
            if (addr_bits != 0)
            {
                SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                                (unit, dbal_logical_table->physical_db_id[0], key_size, payload_size,
                                 dbal_logical_table->app_id, dbal_logical_table->app_id_size, NULL));
            }

            

            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
        {

            

            continue;
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
        {

            

            continue;
        }
        else
        {
            LOG_CLI((BSL_META
                     ("Physical table %d not associated with TCAM/LPM/EM/Direct/EEDB, but its access method is MDB.\n"),
                     dbal_logical_table->physical_db_id[0]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT)

static shr_error_e
mdb_init_fec_bank_to_stage_mapping_table_set(
    int unit,
    uint32 first_bank,
    uint32 last_bank,
    mdb_physical_table_e mdb_physical_table_id)
{
    uint32 entry_handle_id;
    uint32 bank_iter;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_BANK_TO_STAGE_MAP, &entry_handle_id));

    for (bank_iter = first_bank; bank_iter <= last_bank; bank_iter++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL,
                                     (bank_iter / dnx_data_l3.fer.mdb_granularity_ratio_get(unit)),
                                     dnx_data_l3.fec.fec_tables_info_get(unit, mdb_physical_table_id)->hierarchy);

        SHR_IF_ERR_EXIT(mdb_db_infos.fec_db_info.fec_db_map.set(unit, bank_iter, mdb_physical_table_id));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif



static shr_error_e
mdb_init_fec_bank_to_stage_map(
    int unit,
    mdb_physical_table_e mdb_physical_table_id)
{
    uint32 fec_id_range_start = 0, fec_id_range_size = 0;
    uint32 first_bank, last_bank;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    SHR_IF_ERR_EXIT(mdb_init_fec_ranges_allocation_info_get
                    (unit, mdb_physical_table_id, &fec_id_range_start, &fec_id_range_size));

    first_bank = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, fec_id_range_start);
    last_bank = DNX_ALGO_L3_BANK_ID_FROM_FEC_ID_GET(unit, (fec_id_range_start + fec_id_range_size - 1));

#if defined(BCM_DNX2_SUPPORT)
    {
        SHR_IF_ERR_EXIT(mdb_init_fec_bank_to_stage_mapping_table_set
                        (unit, first_bank, last_bank, mdb_physical_table_id));
    }
#endif
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_direct_table(
    int unit,
    mdb_physical_table_e mdb_physical_table)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_tables_e mdb_table_11 = dnx_data_mdb.direct.mdb_11_info_get(unit, mdb_physical_table)->dbal_table;

    if (mdb_table_11 != DBAL_TABLE_EMPTY)
    {
        uint8 multiplier = 0;
        uint8 divider = 0;
        uint8 address_space_field_val = 0;
        uint32 mapping_compare_bits;
        uint32 entry_handle_mdb_11_table;
        int basic_size;
        int row_size;
        int cluster_idx;
        int max_end_address = 0;
        int address_space_max_bits;
        int address_space_bits;
        int max_macro_mapping_index, macro_mapping_index;
        int mult_iter;
        int mult_array_size;
        int nof_clusters;
        uint8 macro_idx;
        mdb_macro_types_e macro_type;
        mdb_eedb_mem_type_e eedb_mem_type;


        

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, mdb_table_11, &entry_handle_mdb_11_table));

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters));

        row_size = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->row_width;

        max_macro_mapping_index = dnx_data_mdb.direct.mdb_11_info_get(unit, mdb_physical_table)->mdb_item_0_array_size;

        divider = MDB_DIRECT_ROW_SIZE_TO_DIVIDER(row_size);

        
        if ((dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
            && (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->db_subtype == MDB_DIRECT_TYPE_VSI))
        {
            address_space_max_bits = dnx_data_mdb.direct.vsi_physical_address_max_bits_get(unit);
        }
        else
        {
            address_space_max_bits = dnx_data_mdb.direct.physical_address_max_bits_get(unit);
        }

        

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            int current_end_address;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            end_address.get(unit, mdb_physical_table, cluster_idx, &current_end_address));

            if (max_end_address < current_end_address)
            {
                max_end_address = current_end_address;
            }
        }

        
        eedb_mem_type = mdb_eedb_table_to_mem_type(unit, mdb_physical_table);
        if (eedb_mem_type == MDB_EEDB_MEM_TYPE_DATA_CLUSTER)
        {
            
            address_space_bits = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);
            address_space_field_val = address_space_max_bits - address_space_bits;
        }
        else if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
        {
            
            address_space_bits =
                dnx_data_lif.out_lif.outlif_eedb_banks_pointer_size_get(unit) -
                utilex_log2_round_up(MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE);
            address_space_field_val = address_space_max_bits - address_space_bits;
        }
        else
        {
            address_space_bits = address_space_max_bits;
            while (((1 << address_space_bits) >= max_end_address)
                   && (address_space_field_val < MDB_INIT_DIRECT_PHYSICAL_ADDRESS_MAX_VAL))
            {
                address_space_bits--;
                address_space_field_val++;
            }

            

            if (address_space_field_val > 0)
            {
                address_space_bits++;
                address_space_field_val--;
            }
        }
        
        SHR_IF_ERR_EXIT(mdb_db_infos.db.address_space_bits.set(unit, mdb_physical_table, address_space_bits));

        if (MDB_INIT_IS_FEC_TABLE(mdb_physical_table))
        {
            uint32 max_fec_id;
            SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));

            

            if (max_fec_id > dnx_data_l3.fec.max_fec_id_for_single_dhb_cluster_pair_granularity_get(unit)
                && address_space_bits >
                dnx_data_mdb.direct.physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get(unit))
            {

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. FEC address space %d is larger than the max address size supported (%d) when working in 26k FEC allocation granularity.\n",
                             address_space_bits,
                             dnx_data_mdb.
                             direct.physical_address_max_bits_support_single_macro_b_granularity_fec_alloc_get(unit));
            }

            if (!dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fec_hit_bit))
            {
                uint32 entry_handle_42;
                uint32 value = TRUE;
                uint32 fec_iter;

                for (fec_iter = 0; fec_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_iter++)
                {
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                    (unit,
                                     dnx_data_mdb.direct.mdb_direct_mdb_phy_tables_info_get(unit,
                                                                                            mdb_physical_table)->mdb_42_table,
                                     &entry_handle_42));

                    dbal_entry_value_field_arr32_set(unit, entry_handle_42, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, &value);

                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_42, DBAL_COMMIT));
                }
            }

        }

        mult_array_size = dnx_data_mdb.direct.mdb_11_info_get(unit, mdb_physical_table)->mdb_item_3_array_size;

        for (mult_iter = 0; mult_iter < mult_array_size; mult_iter++)
        {

            if (mult_array_size == 1)
            {
                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_physical_table, &basic_size));
            }
            else
            {
                
                dbal_enum_value_field_mdb_eedb_phase_e eedb_phase;
                dbal_physical_tables_e dbal_physical_table_id;
                uint32 address_granularity;

                eedb_phase = mdb_eedb_table_to_phase(unit, mdb_physical_table) + mult_iter;

                dbal_physical_table_id = DBAL_PHYSICAL_TABLE_EEDB_1 + eedb_phase;

                SHR_IF_ERR_EXIT(mdb_eedb_table_data_granularity_get
                                (unit, dbal_physical_table_id, &address_granularity));

                basic_size = address_granularity;
            }

            multiplier = MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(basic_size);

            dbal_entry_value_field32_set(unit, entry_handle_mdb_11_table, DBAL_FIELD_MDB_ITEM_3, mult_iter, multiplier);

            dbal_entry_value_field32_set(unit, entry_handle_mdb_11_table, DBAL_FIELD_MDB_ITEM_4, mult_iter, multiplier);
        }

        dbal_entry_value_field32_set(unit, entry_handle_mdb_11_table, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, divider);

        dbal_entry_value_field32_set(unit, entry_handle_mdb_11_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                     address_space_field_val);

        

        mapping_compare_bits = utilex_log2_round_up(max_macro_mapping_index);

        for (macro_mapping_index = 0; macro_mapping_index < max_macro_mapping_index; macro_mapping_index++)
        {
            uint32 macro_mapping_val = 0;
            uint32 macro_mapping_addr = macro_mapping_index << (address_space_bits - mapping_compare_bits);
            shr_error_e search_rv;

            if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
            {

                

                int logical_address;
                mdb_physical_table_e mdb_physical_table_id_data;
                int cluster_index_data;
                mdb_physical_table_e mdb_physical_table_id_ll;
                int cluster_index_ll;
                dbal_enum_value_field_mdb_eedb_phase_e phase;
                mdb_physical_table_e mdb_physical_table_pair;
                int phase_pair_enabled = dnx_data_mdb.eedb.nof_phase_per_mag_get(unit) == 2;

                logical_address = macro_mapping_addr * MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE;

                search_rv = mdb_eedb_address_mapping_get(unit, logical_address, &mdb_physical_table_id_data,
                                                         &cluster_index_data, &mdb_physical_table_id_ll,
                                                         &cluster_index_ll);

                if (search_rv != _SHR_E_RESOURCE)
                {
                    

                    phase = mdb_eedb_table_to_phase(unit, mdb_physical_table);
                    if (phase_pair_enabled)
                    {
                        SHR_IF_ERR_EXIT(mdb_eedb_translate
                                        (unit, MDB_EEDB_MEM_TYPE_LL, phase + 1, &mdb_physical_table_pair));
                    }

                    
                    if ((mdb_physical_table_id_ll != mdb_physical_table)
                        && ((!phase_pair_enabled) || (mdb_physical_table_id_ll != mdb_physical_table_pair)))
                    {
                        search_rv = _SHR_E_RESOURCE;
                    }
                }
            }
            else
            {

                

                search_rv = mdb_direct_address_mapping_get(unit, mdb_physical_table, macro_mapping_addr, &cluster_idx);
            }

            

            if (search_rv != _SHR_E_RESOURCE)
            {

                

                if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
                {
                    macro_mapping_val = 0;
                }
                else
                {
                    int adjusted_cluster_macro_idx;

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    macro_type.get(unit, mdb_physical_table, cluster_idx, &macro_type));
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    macro_index.get(unit, mdb_physical_table, cluster_idx, &macro_idx));

                    

                    adjusted_cluster_macro_idx =
                        macro_idx + (dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->global_start_index);

                    macro_mapping_val =
                        dnx_data_mdb.dh.dh_info_get(unit,
                                                    mdb_physical_table)->table_macro_interface_mapping
                        [adjusted_cluster_macro_idx];
                }
            }
            else if (eedb_mem_type == MDB_EEDB_MEM_TYPE_DATA_CLUSTER)
            {

                

                uint32 eedb_bank_pos_in_array =
                    dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;
                macro_mapping_val =
                    dnx_data_mdb.dh.dh_info_get(unit,
                                                mdb_physical_table)->table_macro_interface_mapping
                    [eedb_bank_pos_in_array];
            }
            else if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
            {

                

                macro_mapping_val = 1;
            }

            dbal_entry_value_field32_set(unit, entry_handle_mdb_11_table, DBAL_FIELD_MDB_ITEM_0, macro_mapping_index,
                                         macro_mapping_val);

        }

        

        dbal_entry_value_field32_set(unit, entry_handle_mdb_11_table, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE,
                                     dnx_data_mdb.direct.physical_address_ovf_enable_val_get(unit));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_11_table, DBAL_COMMIT));


        

        if (MDB_INIT_IS_FEC_TABLE(mdb_physical_table))
        {

            int address_mapping_index;
            
            uint32 entry_handle_mdb_12_table;
            int fec_abk_mapping = dnx_data_mdb.direct.feature_get(unit, dnx_data_mdb_direct_fec_abk_mapping);

            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_physical_table, &basic_size));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.direct.mdb_direct_mdb_phy_tables_info_get(unit,
                                                                                    mdb_physical_table)->mdb_12_table,
                             &entry_handle_mdb_12_table));

            

            for (address_mapping_index = 0;
                 address_mapping_index < (1 << dnx_data_mdb.direct.fec_address_mapping_bits_get(unit));
                 address_mapping_index++)
            {
                uint32 val = address_mapping_index;

                dbal_entry_value_field32_set(unit, entry_handle_mdb_12_table, DBAL_FIELD_MDB_ITEM_0, val, val);

                if (fec_abk_mapping == TRUE)
                {
                    dbal_entry_value_field32_set(unit, entry_handle_mdb_12_table, DBAL_FIELD_MDB_ITEM_1, val, val);
                }
            }

            

#ifndef ADAPTER_SERVER_MODE
            if (dnx_data_mdb.direct.feature_get(unit, dnx_data_mdb_direct_fec_address_remap))
            {
                int fec_cluster_granularity = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                 mdb_physical_table)->row_width /
                    DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS;

                

                for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx += fec_cluster_granularity)
                {
                    int start_address, end_address;
                    int map_index, map_index_start, map_index_end;
                    int address_mapping_field_offset;
                    uint8 cluster_idx_in_macro;
                    uint32 map_value;
                    uint32 relevant_bits_mask, relevant_bits_offset;
                    uint32 fec_address_mapping_relevant_bits_offset;
                    mdb_macro_types_e macro_type;

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    start_address.get(unit, mdb_physical_table, cluster_idx, &start_address));
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    end_address.get(unit, mdb_physical_table, cluster_idx, &end_address));
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    cluster_index.get(unit, mdb_physical_table, cluster_idx, &cluster_idx_in_macro));
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    macro_type.get(unit, mdb_physical_table, cluster_idx, &macro_type));

                    address_mapping_field_offset =
                        address_space_bits - dnx_data_mdb.direct.fec_address_mapping_bits_get(unit);

                    map_index_start = start_address >> address_mapping_field_offset;
                    map_index_end = end_address >> address_mapping_field_offset;

                    
                    fec_address_mapping_relevant_bits_offset =
                        dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_address_bits;

                    

                    if (fec_address_mapping_relevant_bits_offset > address_mapping_field_offset)
                    {
                        relevant_bits_offset = fec_address_mapping_relevant_bits_offset - address_mapping_field_offset;
                    }
                    else
                    {
                        relevant_bits_offset = 0;
                    }

                    relevant_bits_mask =
                        UTILEX_ZERO_BITS_MASK((relevant_bits_offset +
                                               dnx_data_mdb.direct.fec_address_mapping_bits_relevant_get(unit)) - 1,
                                              relevant_bits_offset);

                    

                    for (map_index = map_index_start; map_index < map_index_end; map_index++)
                    {

                        map_value =
                            (map_index & relevant_bits_mask) | ((cluster_idx_in_macro / fec_cluster_granularity) <<
                                                                relevant_bits_offset);
                        dbal_entry_value_field32_set(unit, entry_handle_mdb_12_table, DBAL_FIELD_MDB_ITEM_0, map_index,
                                                     map_value);

                        if (fec_abk_mapping == TRUE)
                        {
                            dbal_entry_value_field32_set(unit, entry_handle_mdb_12_table, DBAL_FIELD_MDB_ITEM_1,
                                                         map_index, map_value);
                        }
                    }
                }
            }
#endif 

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_12_table, DBAL_COMMIT));

            DBAL_HANDLE_FREE(unit, entry_handle_mdb_12_table);

            if (nof_clusters > 0)
            {
                SHR_IF_ERR_EXIT(mdb_init_fec_bank_to_stage_map(unit, mdb_physical_table));
            }

        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_set_cluster_alloc(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_info_t * cluster_info_p,
    mdb_eedb_mem_type_e mdb_eedb_type,
    int table_entry_size,
    int current_output_cluster_idx,
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS])
{
    int entry_size;
    uint32 row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;

    SHR_FUNC_INIT_VARS(unit);

    if (current_output_cluster_idx >= DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. current_output_cluster_idx %d is larger than %d.\n",
                     current_output_cluster_idx, DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS);
    }

    

    if (mdb_eedb_type == MDB_EEDB_MEM_TYPE_LL)
    {
        entry_size = MDB_DIRECT_BASIC_ENTRY_SIZE;
    }
    else
    {
        entry_size = table_entry_size;
    }

    clusters[current_output_cluster_idx].cluster_idx = cluster_info_p->cluster_index;
    clusters[current_output_cluster_idx].macro_idx = cluster_info_p->macro_index;

    clusters[current_output_cluster_idx].macro_type = cluster_info_p->macro_type;
    SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows
                    (unit, cluster_info_p->macro_type, &clusters[current_output_cluster_idx].macro_nof_rows));
    clusters[current_output_cluster_idx].macro_row_width = row_width;
    clusters[current_output_cluster_idx].dbal_physical_table_id = dbal_physical_table_id;

    

    if (cluster_info_p->start_address == MDB_DIRECT_INVALID_START_ADDRESS)
    {
        clusters[current_output_cluster_idx].logical_start_address = MDB_DIRECT_INVALID_START_ADDRESS;
    }
    else
    {
        clusters[current_output_cluster_idx].logical_start_address =
            UTILEX_DIV_ROUND_UP(cluster_info_p->start_address * row_width, entry_size);
    }

    clusters[current_output_cluster_idx].mdb_eedb_type = mdb_eedb_type;
    clusters[current_output_cluster_idx].entry_size = entry_size;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_get_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    mdb_physical_table_e * mdb_physical_table_id)
{
    int index = MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macro_type, macro_idx, cluster_idx);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                    pdb.get(unit, macro_type, index, mdb_physical_table_id));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_get_table_resources(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS],
    int *nof_valid_clusters)
{
    int current_output_cluster_idx = 0;
    mdb_physical_table_e mdb_table = MDB_PHYSICAL_TABLE_EEDB_1_DATA;
    mdb_physical_table_e physical_mdb_tables[MDB_INIT_MAX_NOF_MDB_TABLES_PER_DBAL];
    int num_of_physical_table, table_iter;
    int cluster_idx;
    int eedb_bank;
    int nof_clusters;
    mdb_macro_types_e macro_type;
    uint8 info_cluster_index;
    dbal_enum_value_field_mdb_db_type_e db_type =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type;
    uint32 eedb_nof_phase_per_mag = dnx_data_mdb.eedb.nof_phase_per_mag_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_physical_table_id >= DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid DBAL physical table %d.\n", dbal_physical_table_id);
    }

    if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        
        int phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
        int table_idx = 0;

        SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, &physical_mdb_tables[table_idx++]));
        SHR_IF_ERR_EXIT(mdb_eedb_translate
                        (unit, MDB_EEDB_MEM_TYPE_DATA_BANK, phase_index, &physical_mdb_tables[table_idx++]));

        {
            SHR_IF_ERR_EXIT(mdb_eedb_translate
                            (unit, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, phase_index, &physical_mdb_tables[table_idx++]));

        }

        num_of_physical_table = table_idx;
    }
    else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        int lpm_db_idx = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_subtype;

        {
            
            num_of_physical_table = 2;

            physical_mdb_tables[0] = dnx_data_mdb.kaps.db_info_get(unit, lpm_db_idx)->mdb_ads_id;
            physical_mdb_tables[1] = dnx_data_mdb.kaps.db_info_get(unit, lpm_db_idx)->mdb_bb_id;
        }
    }
    else
    {
        num_of_physical_table = 1;
        physical_mdb_tables[0] = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

        if (physical_mdb_tables[0] == MDB_NOF_PHYSICAL_TABLES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. DBAL physical table %d is not associated with an MDB table.\n",
                         dbal_physical_table_id);
        }
    }

    for (table_iter = 0; table_iter < num_of_physical_table; table_iter++)
    {
        mdb_table = physical_mdb_tables[table_iter];

        
        if (mdb_table == MDB_NOF_PHYSICAL_TABLES)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table, &nof_clusters));

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            mdb_cluster_info_t cluster_info;
            mdb_eedb_mem_type_e mdb_eedb_type = MDB_NOF_EEDB_MEM_TYPES;
            int entry_size;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table, cluster_idx, &cluster_info));

            if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
            {
                uint32 phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
                uint32 payload_type;
                SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase_index, &payload_type));
                entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

                

                if (table_iter == 0)
                {
                    mdb_eedb_type = MDB_EEDB_MEM_TYPE_LL;
                }
                else if (table_iter == 1)
                {
                    mdb_eedb_type = MDB_EEDB_MEM_TYPE_DATA_BANK;
                }
                else
                {
                    mdb_eedb_type = MDB_EEDB_MEM_TYPE_DATA_CLUSTER;
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_table, &entry_size));
            }

            SHR_IF_ERR_EXIT(mdb_init_set_cluster_alloc
                            (unit, dbal_physical_table_id, &cluster_info, mdb_eedb_type, entry_size,
                             current_output_cluster_idx, clusters));

            current_output_cluster_idx++;
        }
    }

    

    if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        uint32 nof_eedb_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
        for (eedb_bank = 0; eedb_bank < nof_eedb_banks; eedb_bank++)
        {
            uint32 phase_bank_index;
            int phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;

            SHR_IF_ERR_EXIT(mdb_eedb_phase_bank_to_index(unit, eedb_bank, phase_index, &phase_bank_index));

            

            if (phase_bank_index != DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
            {
                int phase_index_iter;
                int is_allocated = 0;

                for (phase_index_iter = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1;
                     phase_index_iter < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES; phase_index_iter++)
                {
                    SHR_IF_ERR_EXIT(mdb_eedb_phase_bank_to_index(unit, eedb_bank, phase_index_iter, &phase_bank_index));

                    

                    if (phase_bank_index != DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
                    {
                        mdb_physical_table_e mdb_table_assoc;
                        mdb_physical_table_e mdb_table_iter;
                        mdb_eedb_mem_type_e data_ll_iter;

                        SHR_IF_ERR_EXIT(mdb_init_get_cluster_assoc
                                        (unit, MDB_EEDB_BANK, 0, eedb_bank, &mdb_table_assoc));

                        
                        for (data_ll_iter = MDB_EEDB_MEM_TYPE_DATA_BANK; data_ll_iter < MDB_NOF_EEDB_MEM_TYPES;
                             data_ll_iter++)
                        {
                            SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, data_ll_iter, phase_index_iter, &mdb_table_iter));

                            if (mdb_table_assoc == mdb_table_iter)
                            {
                                is_allocated = 1;
                                break;
                            }
                        }

                        if ((is_allocated == 1) && (phase_index_iter != phase_index))
                        {
                            mdb_cluster_info_t cluster_info;
                            uint32 payload_type;
                            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                            number_of_clusters.get(unit, mdb_table_assoc, &nof_clusters));

                            

                            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
                            {
                                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                                macro_type.get(unit, mdb_table_iter, cluster_idx, &macro_type));
                                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                                cluster_index.get(unit, mdb_table_iter, cluster_idx,
                                                                  &info_cluster_index));

                                if ((info_cluster_index == eedb_bank) && (macro_type == MDB_EEDB_BANK))
                                {
                                    break;
                                }
                            }

                            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                            clusters_info.get(unit, mdb_table_iter, cluster_idx, &cluster_info));
                            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase_index_iter, &payload_type));
                            SHR_IF_ERR_EXIT(mdb_init_set_cluster_alloc
                                            (unit, DBAL_PHYSICAL_TABLE_EEDB_1 + phase_index_iter, &cluster_info,
                                             MDB_EEDB_MEM_TYPE_LL,
                                             MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type),
                                             current_output_cluster_idx, clusters));

                            current_output_cluster_idx++;
                        }
                    }

                    

                    if (is_allocated == 1)
                    {
                        break;
                    }
                }

                

                if (is_allocated == 0)
                {
                    mdb_cluster_info_t cluster_info;

                    

                    cluster_info.macro_type = MDB_EEDB_BANK;
                    cluster_info.macro_index = 0;
                    cluster_info.cluster_index = eedb_bank;
                    cluster_info.start_address = MDB_DIRECT_INVALID_START_ADDRESS;
                    cluster_info.end_address = MDB_DIRECT_INVALID_START_ADDRESS;

                    SHR_IF_ERR_EXIT(mdb_init_set_cluster_alloc
                                    (unit, dbal_physical_table_id, &cluster_info,
                                     MDB_EEDB_MEM_TYPE_LL,
                                     MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B),
                                     current_output_cluster_idx, clusters));
                    current_output_cluster_idx++;
                }
            }
        }
    }

    
#if defined(BCM_DNX2_SUPPORT)
    if (((eedb_nof_phase_per_mag == 2) && (dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_5)
         && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
        || ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_FEC_1)
            && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_FEC_3)))
    {

        

        mdb_physical_table_e mdb_table_pairs[MDB_INIT_MAX_NOF_TABLES_RESOURCE_SHARE];

        

        dbal_physical_tables_e dbal_table_pairs[MDB_INIT_MAX_NOF_TABLES_RESOURCE_SHARE];

        

        mdb_macro_types_e macro_types[MDB_INIT_MAX_NOF_TABLES_RESOURCE_SHARE];
        int macro_idxs[MDB_INIT_MAX_NOF_TABLES_RESOURCE_SHARE];

        int num_of_pair_tables = 0;
        int table_iterator;
        int entry_size;
        mdb_eedb_mem_type_e mem_type = MDB_NOF_EEDB_MEM_TYPES;

        

        if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_5)
            || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_6))
        {
            mdb_table_pairs[0] = MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_EEDB_7;
            macro_types[0] = MDB_MACRO_A;
            macro_idxs[0] = 1;

            num_of_pair_tables = 1;
            mem_type = MDB_EEDB_MEM_TYPE_DATA_CLUSTER;
        }
        else if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_7)
                 || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_8))
        {
            mdb_table_pairs[0] = MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_EEDB_5;
            macro_types[0] = MDB_MACRO_A;
            macro_idxs[0] = 1;

            num_of_pair_tables = 1;
            mem_type = MDB_EEDB_MEM_TYPE_DATA_CLUSTER;
        }
        else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_1)
        {
            mdb_table_pairs[0] = MDB_PHYSICAL_TABLE_FEC_2;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_FEC_2;
            macro_types[0] = MDB_MACRO_B;
            macro_idxs[0] = 7;

            num_of_pair_tables = 1;
            mem_type = MDB_NOF_EEDB_MEM_TYPES;
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, MDB_PHYSICAL_TABLE_FEC_1, &entry_size));
        }
        else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_2)
        {
            mdb_table_pairs[0] = MDB_PHYSICAL_TABLE_FEC_1;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_FEC_1;
            macro_types[0] = MDB_MACRO_B;
            macro_idxs[0] = 7;

            mdb_table_pairs[1] = MDB_PHYSICAL_TABLE_FEC_3;
            dbal_table_pairs[1] = DBAL_PHYSICAL_TABLE_FEC_3;
            macro_types[1] = MDB_MACRO_A;
            macro_idxs[1] = 5;

            num_of_pair_tables = 2;
            mem_type = MDB_NOF_EEDB_MEM_TYPES;
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, MDB_PHYSICAL_TABLE_FEC_2, &entry_size));
        }
        else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_3)
        {
            mdb_table_pairs[0] = MDB_PHYSICAL_TABLE_FEC_2;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_FEC_2;
            macro_types[0] = MDB_MACRO_A;
            macro_idxs[0] = 5;

            num_of_pair_tables = 1;
            mem_type = MDB_NOF_EEDB_MEM_TYPES;
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, MDB_PHYSICAL_TABLE_FEC_3, &entry_size));
        }

        for (table_iterator = 0; table_iterator < num_of_pair_tables; table_iterator++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            number_of_clusters.get(unit, mdb_table_pairs[table_iterator], &nof_clusters));

            

            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
            {
                mdb_cluster_info_t cluster_info;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                clusters_info.get(unit, mdb_table_pairs[table_iterator], cluster_idx, &cluster_info));

                if ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_5)
                    && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
                {
                    uint32 payload_type;
                    SHR_IF_ERR_EXIT(mdb_db_infos.
                                    eedb_payload_type.get(unit, dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1,
                                                          &payload_type));
                    entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);
                }

                if ((cluster_info.macro_type == macro_types[table_iterator])
                    && (cluster_info.macro_index == macro_idxs[table_iterator]))
                {
                    SHR_IF_ERR_EXIT(mdb_init_set_cluster_alloc
                                    (unit, dbal_table_pairs[table_iterator], &cluster_info, mem_type, entry_size,
                                     current_output_cluster_idx, clusters));

                    current_output_cluster_idx++;
                }
            }
        }
    }
#endif

    *nof_valid_clusters = current_output_cluster_idx;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_update_address_mapping(
    int unit,
    mdb_physical_table_e mdb_table_id,
    int physical_start_address,
    int physical_end_address,
    int logical_start_address,
    int logical_end_address,
    int set,
    int cluster_idx)
{

    SHR_FUNC_INIT_VARS(unit);

    

    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
    {
        if (set == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set
                            (unit, mdb_table_id, physical_start_address, physical_end_address, cluster_idx));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_direct_address_mapping_unset
                            (unit, mdb_table_id, physical_start_address, physical_end_address));
        }
    }

    

    else if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        if (mdb_eedb_table_to_mem_type(unit, mdb_table_id) == MDB_EEDB_MEM_TYPE_DATA_CLUSTER)
        {
            if (set == TRUE)
            {
                SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set
                                (unit, mdb_table_id, physical_start_address, physical_end_address, cluster_idx));
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_direct_address_mapping_unset
                                (unit, mdb_table_id, physical_start_address, physical_end_address));
            }
        }

        if (set == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_set
                            (unit, mdb_table_id, logical_start_address, logical_end_address, cluster_idx));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_unset
                            (unit, mdb_table_id, logical_start_address, logical_end_address));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_set_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS],
    mdb_init_cluster_func_e func)
{
    uint32 global_macro_index;
    uint32 skip_cluster[MDB_NOF_PHYSICAL_TABLES] = { 0 };
    int cluster_iter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (func >= MDB_NOF_INIT_CLUSTER_OPERATIONS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unrecognized func %d.\n", func);
    }

    

    for (cluster_iter = 0; cluster_iter < nof_valid_clusters; cluster_iter++)
    {
        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, clusters[cluster_iter].dbal_physical_table_id)->db_type ==
            DBAL_NOF_ENUM_MDB_DB_TYPE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. The dbal_physical_table_id %s in cluster %d, is not associated with MDB.\n",
                         dbal_physical_table_to_string(unit, clusters[cluster_iter].dbal_physical_table_id),
                         cluster_iter);
        }

        if (clusters[cluster_iter].macro_type >= MDB_NOF_MACRO_TYPES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid macro_type %d in cluster %d.\n",
                         clusters[cluster_iter].macro_type, cluster_iter);
        }

        if (((clusters[cluster_iter].dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
             && (clusters[cluster_iter].dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
            && (clusters[cluster_iter].mdb_eedb_type >= MDB_NOF_EEDB_MEM_TYPES))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid mdb_eedb_type %d in cluster %d.\n",
                         clusters[cluster_iter].mdb_eedb_type, cluster_iter);
        }

        if (clusters[cluster_iter].macro_type >= MDB_NOF_MACRO_TYPES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid macro_type %d in cluster %d.\n",
                         clusters[cluster_iter].macro_type, cluster_iter);
        }

        if (clusters[cluster_iter].macro_idx >=
            dnx_data_mdb.dh.macro_type_info_get(unit, clusters[cluster_iter].macro_type)->nof_macros)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid combination of macro_type %d and macro_idx %d in cluster %d.\n",
                         clusters[cluster_iter].macro_type, clusters[cluster_iter].macro_idx, cluster_iter);
        }

        if (clusters[cluster_iter].cluster_idx >=
            (dnx_data_mdb.dh.macro_type_info_get(unit, clusters[cluster_iter].macro_type)->nof_clusters))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. Invalid combination of macro_type %d and cluster_idx %d in cluster %d.\n",
                         clusters[cluster_iter].macro_type, clusters[cluster_iter].cluster_idx, cluster_iter);
        }
    }

    

    for (cluster_iter = 0; cluster_iter < nof_valid_clusters; cluster_iter++)
    {
        uint32 macro_cluster_allocation = 0;
        uint32 add_removed_clusters = 0;
        dbal_enum_value_field_mdb_db_type_e db_type =
            dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, clusters[cluster_iter].dbal_physical_table_id)->db_type;
        mdb_physical_table_e mdb_table_id;
        int nof_table_clusters;
        int cluster_rows;
        int calc_entry_size;
        uint32 row_width;

        
        if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
        {
            
            SHR_IF_ERR_EXIT(mdb_eedb_translate
                            (unit, clusters[cluster_iter].mdb_eedb_type,
                             clusters[cluster_iter].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1,
                             &mdb_table_id));
        }
        else
        {

            

            mdb_table_id =
                dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                    clusters[cluster_iter].dbal_physical_table_id)->logical_to_physical;
        }

        row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_table_id)->row_width;

        global_macro_index =
            dnx_data_mdb.dh.macro_type_info_get(unit,
                                                clusters[cluster_iter].macro_type)->global_start_index +
            clusters[cluster_iter].macro_idx;

        if (clusters[cluster_iter].macro_type < MDB_EEDB_BANK)
        {

            

            if (((skip_cluster[mdb_table_id] >> global_macro_index) & 0x1) == 0)
            {
                int cluster_idx;

                skip_cluster[mdb_table_id] |= (1 << global_macro_index);

                

                for (cluster_idx = cluster_iter; cluster_idx < nof_valid_clusters; cluster_idx++)
                {
                    if ((clusters[cluster_iter].macro_idx == clusters[cluster_idx].macro_idx) &&
                        (clusters[cluster_iter].macro_type == clusters[cluster_idx].macro_type) &&
                        (clusters[cluster_iter].dbal_physical_table_id == clusters[cluster_idx].dbal_physical_table_id))
                    {
                        add_removed_clusters |= (1 << clusters[cluster_idx].cluster_idx);
                    }
                }

                SHR_IF_ERR_EXIT(mdb_get_macro_allocation(unit, mdb_table_id, clusters[cluster_iter].macro_idx,
                                                         clusters[cluster_iter].macro_type, &macro_cluster_allocation));

                

                if (func == MDB_INIT_CLUSTER_DELETE)
                {
                    SHR_IF_ERR_EXIT(mdb_verify_macro_cluster_allocation
                                    (unit, macro_cluster_allocation & (~add_removed_clusters)));
                }
                else
                {
                    SHR_IF_ERR_EXIT(mdb_verify_macro_cluster_allocation
                                    (unit, macro_cluster_allocation | add_removed_clusters));
                }
            }
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table_id, &nof_table_clusters));

        

        if ((clusters[cluster_iter].macro_type == MDB_EEDB_BANK)
            && (clusters[cluster_iter].mdb_eedb_type == MDB_EEDB_MEM_TYPE_LL))
        {
            calc_entry_size = MDB_DIRECT_BASIC_ENTRY_SIZE;
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size_using_logical_mdb_table
                            (unit, clusters[cluster_iter].dbal_physical_table_id, &calc_entry_size));
        }

        SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, clusters[cluster_iter].macro_type, &cluster_rows));

        

        if (func == MDB_INIT_CLUSTER_DELETE)
        {
            int cluster_idx;

            

            if ((clusters[cluster_iter].macro_type == MDB_EEDB_BANK) && (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
            {
                SHR_IF_ERR_EXIT(mdb_init_eedb_deinit_bank(unit, clusters[cluster_iter].cluster_idx));
            }

            for (cluster_idx = 0; cluster_idx < nof_table_clusters; cluster_idx++)
            {
                int shift_index;
                mdb_cluster_info_t cluster_info;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table_id, cluster_idx, &cluster_info));

                if ((clusters[cluster_iter].macro_type == cluster_info.macro_type)
                    && (clusters[cluster_iter].macro_idx == cluster_info.macro_index)
                    && (clusters[cluster_iter].cluster_idx == cluster_info.cluster_index))
                {

                    

                    if (cluster_iter % (row_width / MDB_NOF_CLUSTER_ROW_BITS) == 0)
                    {
                        int logical_start_address_calc;
                        int physical_start_address_calc;

                        

                        physical_start_address_calc = cluster_info.start_address;
                        logical_start_address_calc = physical_start_address_calc * row_width / calc_entry_size;

                        SHR_IF_ERR_EXIT(mdb_init_update_address_mapping(unit,
                                                                        mdb_table_id,
                                                                        physical_start_address_calc,
                                                                        physical_start_address_calc + cluster_rows,
                                                                        logical_start_address_calc,
                                                                        logical_start_address_calc +
                                                                        ((cluster_rows * row_width) / calc_entry_size),
                                                                        FALSE, 0));
                    }

                    

                    SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                                    (unit, cluster_info.macro_type, cluster_info.macro_index,
                                     cluster_info.cluster_index, MDB_NOF_PHYSICAL_TABLES));

                    

                    shift_index = cluster_idx;
                    for (shift_index = cluster_idx; shift_index < nof_table_clusters - 1; shift_index++)
                    {
                        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                        clusters_info.get(unit, mdb_table_id, shift_index + 1, &cluster_info));
                        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                        clusters_info.set(unit, mdb_table_id, shift_index, &cluster_info));

                        

                        SHR_IF_ERR_EXIT(mdb_init_update_address_mapping(unit,
                                                                        mdb_table_id,
                                                                        cluster_info.start_address,
                                                                        cluster_info.end_address,
                                                                        (cluster_info.start_address * row_width) /
                                                                        calc_entry_size,
                                                                        (cluster_info.end_address * row_width) /
                                                                        calc_entry_size, FALSE, 0));
                        SHR_IF_ERR_EXIT(mdb_init_update_address_mapping
                                        (unit, mdb_table_id, cluster_info.start_address, cluster_info.end_address,
                                         (cluster_info.start_address * row_width) / calc_entry_size,
                                         (cluster_info.end_address * row_width) / calc_entry_size, TRUE, shift_index));
                    }

                    

                    sal_memset(&cluster_info, 0x0, sizeof(cluster_info));
                    cluster_info.macro_type = MDB_NOF_MACRO_TYPES;
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                    clusters_info.set(unit, mdb_table_id, nof_table_clusters - 1, &cluster_info));

                    

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_table_id, --nof_table_clusters));

                    break;
                }
            }

            

            if (clusters[cluster_iter].macro_type != MDB_EEDB_BANK)
            {
                SHR_IF_ERR_EXIT(mdb_dh_set_bpu_setting_macro
                                (unit, clusters[cluster_iter].macro_type, clusters[cluster_iter].macro_idx));
            }

        }

        if (func == MDB_INIT_CLUSTER_ADD)
        {
            int cluster_idx;
            mdb_cluster_info_t cluster_info;
            int logical_start_address_calc;
            int physical_start_address_calc;

            
            for (cluster_idx = 0; cluster_idx < nof_table_clusters; cluster_idx++)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table_id, cluster_idx, &cluster_info));

                if ((clusters[cluster_iter].macro_type == cluster_info.macro_type)
                    && (clusters[cluster_iter].macro_idx == cluster_info.macro_index)
                    && (clusters[cluster_iter].cluster_idx == cluster_info.cluster_index))
                {
                    break;
                }
            }

            

            if (cluster_idx == nof_table_clusters)
            {
                sal_memset(&cluster_info, 0x0, sizeof(mdb_cluster_info_t));

                

                cluster_info.cluster_index = clusters[cluster_iter].cluster_idx;
                cluster_info.macro_index = clusters[cluster_iter].macro_idx;
                cluster_info.macro_type = clusters[cluster_iter].macro_type;
            }

            

            physical_start_address_calc = clusters[cluster_iter].logical_start_address * calc_entry_size / row_width;
            cluster_info.start_address = physical_start_address_calc;

            logical_start_address_calc = UTILEX_DIV_ROUND_UP(cluster_info.start_address * row_width, calc_entry_size);

            if (logical_start_address_calc != clusters[cluster_iter].logical_start_address)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The logical_start_address 0x%x of cluster %d is mapped incorrectly to HW logical_start_address 0x%x due to rounding.\n",
                             clusters[cluster_iter].logical_start_address, cluster_iter, logical_start_address_calc);
            }

            if (cluster_info.start_address % cluster_rows != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The logical_start_address 0x%x of cluster %d is mapped incorrectly to HW physical_start_address 0x%x, which is not a whole multiple of the number of rows in a cluster 0x%x.\n",
                             clusters[cluster_iter].logical_start_address, cluster_iter, cluster_info.start_address,
                             cluster_rows);
            }

            cluster_info.end_address = cluster_info.start_address + cluster_rows;

            if (cluster_idx == nof_table_clusters)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_table_id, ++nof_table_clusters));

                

                SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                                (unit, cluster_info.macro_type, cluster_info.macro_index,
                                 cluster_info.cluster_index, mdb_table_id));

            }

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.set(unit, mdb_table_id, cluster_idx, &cluster_info));

            

            if (cluster_iter % (row_width / MDB_NOF_CLUSTER_ROW_BITS) == 0)
            {
                SHR_IF_ERR_EXIT(mdb_init_update_address_mapping(unit,
                                                                mdb_table_id,
                                                                physical_start_address_calc,
                                                                physical_start_address_calc + cluster_rows,
                                                                clusters[cluster_iter].logical_start_address,
                                                                clusters[cluster_iter].logical_start_address +
                                                                ((cluster_rows * row_width) / calc_entry_size), TRUE,
                                                                cluster_idx));
            }
        }
    }

    

    for (cluster_iter = 0; cluster_iter < nof_valid_clusters; cluster_iter++)
    {
        int cluster_iter_inner;
        int skip_table = 0;
        int capacity;
        dbal_enum_value_field_mdb_db_type_e db_type;

        

        for (cluster_iter_inner = 0; cluster_iter_inner < cluster_iter; cluster_iter_inner++)
        {
            if (clusters[cluster_iter].dbal_physical_table_id == clusters[cluster_iter_inner].dbal_physical_table_id)
            {
                skip_table = 1;
                break;
            }
        }

        if (skip_table == 1)
        {
            continue;
        }

        db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, clusters[cluster_iter].dbal_physical_table_id)->db_type;

        if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
        {

            

            soc_reg_above_64_val_t type_bank_select;
            soc_reg_above_64_val_t phase_bank_select;
            soc_reg_above_64_val_t bank_id;
            uint32 bank_id_32, data_32;
            int nof_inst;
            int nof_inst_a;
            int nof_inst_c;
            int inst_iter;
            uint32 mdb_table_9_handle_id;

            

            sal_memset(phase_bank_select, 0, sizeof phase_bank_select);

            SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get
                            (unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_1, 0, 0, &nof_inst_a));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_9, &mdb_table_9_handle_id));

            for (inst_iter = 0; inst_iter < nof_inst_a; inst_iter++)
            {
                dbal_value_field_arr32_request(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_1, inst_iter, &data_32);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, mdb_table_9_handle_id, DBAL_COMMIT));

                SHR_BITCOPY_RANGE(phase_bank_select, inst_iter * MDB_NOF_PHASES_PER_EEDB_BANK_BITS, &data_32, 0,
                                  MDB_NOF_PHASES_PER_EEDB_BANK_BITS);
            }

            sal_memset(type_bank_select, 0, sizeof type_bank_select);

            SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get
                            (unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_2, 0, 0, &nof_inst_c));

            for (inst_iter = 0; inst_iter < nof_inst_c; inst_iter++)
            {
                dbal_value_field_arr32_request(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_2, inst_iter, &data_32);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, mdb_table_9_handle_id, DBAL_COMMIT));

                SHR_BITCOPY_RANGE(type_bank_select, inst_iter * MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS, &data_32,
                                  0, MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS);
            }

            sal_memset(bank_id, 0, sizeof bank_id);

            SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get
                            (unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_3, 0, 0, &nof_inst));

            for (int bank_id_iter = 0; bank_id_iter < nof_inst; bank_id_iter++)
            {
                dbal_value_field_arr32_request(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, bank_id_iter,
                                               &bank_id_32);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, mdb_table_9_handle_id, DBAL_COMMIT));

                SHR_BITCOPY_RANGE(bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit), &bank_id_32, 0,
                                  dnx_data_mdb.eedb.bank_id_bits_get(unit));
            }

            SHR_IF_ERR_EXIT(mdb_init_eedb_phase
                            (unit, clusters[cluster_iter].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1,
                             type_bank_select, phase_bank_select, bank_id));

            

            for (inst_iter = 0; inst_iter < nof_inst_a; inst_iter++)
            {
                data_32 = 0;
                SHR_BITCOPY_RANGE(&data_32, 0, phase_bank_select, inst_iter * MDB_NOF_PHASES_PER_EEDB_BANK_BITS,
                                  MDB_NOF_PHASES_PER_EEDB_BANK_BITS);

                dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_1, inst_iter, data_32);

            }

            for (inst_iter = 0; inst_iter < nof_inst_c; inst_iter++)
            {
                data_32 = 0;
                SHR_BITCOPY_RANGE(&data_32, 0, type_bank_select,
                                  inst_iter * MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS,
                                  MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS);

                dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_2, inst_iter, data_32);

            }

            for (int bank_id_iter = 0; bank_id_iter < nof_inst; bank_id_iter++)
            {
                bank_id_32 = 0;
                SHR_BITCOPY_RANGE(&bank_id_32, 0, bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                                  dnx_data_mdb.eedb.bank_id_bits_get(unit));

                dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, bank_id_iter,
                                             bank_id_32);

            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

            

            {
                mdb_physical_table_e mdb_physical_table_id_ll;
                dbal_enum_value_field_mdb_eedb_phase_e phase_ll;

                

                phase_ll = clusters[cluster_iter].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
                phase_ll -= (phase_ll % dnx_data_mdb.eedb.nof_phase_per_mag_get(unit));

                SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_ll, &mdb_physical_table_id_ll));
                SHR_IF_ERR_EXIT(mdb_init_direct_table(unit, mdb_physical_table_id_ll));
            }
        }

        

        if ((clusters[cluster_iter].macro_type != MDB_EEDB_BANK)
            || (clusters[cluster_iter].mdb_eedb_type != MDB_EEDB_MEM_TYPE_LL))
        {
            dbal_physical_tables_e dbal_physical_table_id;

            if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
            {

                

                dbal_physical_table_id =
                    DBAL_PHYSICAL_TABLE_EEDB_1 +
                    (((clusters[cluster_iter].dbal_physical_table_id -
                       DBAL_PHYSICAL_TABLE_EEDB_1) / dnx_data_mdb.eedb.nof_phase_per_mag_get(unit)) *
                     dnx_data_mdb.eedb.nof_phase_per_mag_get(unit));
            }
            else
            {
                dbal_physical_table_id = clusters[cluster_iter].dbal_physical_table_id;
            }

            

            SHR_IF_ERR_EXIT(mdb_init_direct_table
                            (unit,
                             dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical));

            SHR_IF_ERR_EXIT(mdb_dh_init_table
                            (unit,
                             dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical));
        }

        

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, clusters[cluster_iter].dbal_physical_table_id, &capacity));
        SHR_IF_ERR_EXIT(mdb_db_infos.capacity.set(unit, clusters[cluster_iter].dbal_physical_table_id, capacity));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_delete_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_init_set_table_resources(unit, nof_valid_clusters, clusters, MDB_INIT_CLUSTER_DELETE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_add_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[DNX_DATA_MAX_MDB_DH_MAX_NOF_DB_CLUSTERS])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_init_set_table_resources(unit, nof_valid_clusters, clusters, MDB_INIT_CLUSTER_ADD));

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
mdb_init_direct(
    int unit)
{
    mdb_physical_table_e db_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (db_index = 0; db_index < MDB_NOF_PHYSICAL_TABLES; db_index++)
    {
        SHR_IF_ERR_EXIT(mdb_init_direct_table(unit, db_index));
    }


exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_table_em_shift_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    soc_reg_above_64_val_t data_above_64;
    uint32 vmv_table_handle_id;

    int nof_valid_regs;
    int reg_iter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_valid_regs = dnx_data_mdb.em.mdb_em_shift_vmv_regs_get(unit, dbal_physical_table_id)->nof_valid_regs;

    for (reg_iter = 0; reg_iter < nof_valid_regs; reg_iter++)
    {
        dbal_tables_e table_name;
        dbal_fields_e field_name;
        dbal_fields_e key_name;
        uint8 stage_index;

        int field_size;
        int nof_inst;
        int instance_idx;
        uint32 data_32;

        table_name = dnx_data_mdb.em.mdb_em_shift_vmv_regs_get(unit, dbal_physical_table_id)->table_name_arr[reg_iter];
        field_name = dnx_data_mdb.em.mdb_em_shift_vmv_regs_get(unit, dbal_physical_table_id)->field_name_arr[reg_iter];
        key_name = dnx_data_mdb.em.mdb_em_shift_vmv_regs_get(unit, dbal_physical_table_id)->key_name_arr[reg_iter];
        stage_index =
            dnx_data_mdb.em.mdb_em_shift_vmv_regs_get(unit, dbal_physical_table_id)->stage_index_arr[reg_iter];

        if (reg_iter == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &vmv_table_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_name, vmv_table_handle_id));
        }

        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_name, field_name, 0, 0, 0, &field_size));
        SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, table_name, field_name, 0, 0, &nof_inst));

        sal_memset(data_above_64, 0, sizeof data_above_64);

        if (key_name != DBAL_FIELD_EMPTY)
        {
            dbal_entry_key_field32_set(unit, vmv_table_handle_id, key_name, stage_index);
        }

        SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs
                        (unit, MDB_EM_ENCODING_TABLE_ENTRY, MDB_EM_ENCODING_TABLE_ENTRY, dbal_physical_table_id,
                         dnx_data_mdb.em.mdb_em_shift_vmv_regs_get(unit,
                                                                   dbal_physical_table_id)->esem_cmd_indication
                         [reg_iter], NULL, data_above_64));

        for (instance_idx = 0; instance_idx < nof_inst; instance_idx++)
        {
            data_32 = 0;

            SHR_BITCOPY_RANGE(&data_32, 0, data_above_64, instance_idx * field_size, field_size);

            dbal_entry_value_field32_set(unit, vmv_table_handle_id, field_name, instance_idx, data_32);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, vmv_table_handle_id, DBAL_COMMIT));
    }

    
    if ((dbal_physical_table_id == dnx_data_mdb.em.my_mac_ippf_em_dbal_id_get(unit))
        || (dbal_physical_table_id == dnx_data_mdb.em.my_mac_ippa_em_dbal_id_get(unit)))
    {
        uint8 vmv_size;
        uint8 vmv_value;

        SOC_REG_ABOVE_64_CLEAR(data_above_64);

        SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value
                        (unit, dbal_physical_table_id, MDB_EM_ENTRY_ENCODING_QUARTER, &vmv_size, &vmv_value));

        data_above_64[0] = vmv_size;

        if (nof_valid_regs == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_VTT_MY_MAC_VRID_VMV_SHIFT, &vmv_table_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_VTT_MY_MAC_VRID_VMV_SHIFT, vmv_table_handle_id));
        }

        if (dbal_physical_table_id == dnx_data_mdb.em.my_mac_ippf_em_dbal_id_get(unit))
        {
            dbal_entry_value_field32_set(unit, vmv_table_handle_id, DBAL_FIELD_IPPF_VRID_VMV_SHIFT_VALUE, INST_SINGLE,
                                         data_above_64[0]);
        }
        else
        {
            dbal_entry_value_field32_set(unit, vmv_table_handle_id, DBAL_FIELD_IPPA_VRID_VMV_SHIFT_VALUE, INST_SINGLE,
                                         data_above_64[0]);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, vmv_table_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}



shr_error_e
mdb_init_em_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{

    uint32 entry_handle_mdb_14_table;
    uint32 entry_handle_mdb_18_table;
    uint32 data[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32 data_size[SOC_REG_ABOVE_64_MAX_SIZE_U32];
    uint32 *data_size_ptr = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit,
                     dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get(unit, dbal_physical_table_id)->mdb_14_table,
                     &entry_handle_mdb_14_table));

    

    dbal_value_field_arr32_request(unit, entry_handle_mdb_14_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_14_table, DBAL_COMMIT));

    
    if (dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get(unit, dbal_physical_table_id)->mdb_18_table != DBAL_TABLE_EMPTY)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get(unit,
                                                                               dbal_physical_table_id)->mdb_18_table,
                         &entry_handle_mdb_18_table));
        dbal_value_field_arr32_request(unit, entry_handle_mdb_18_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, data_size);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_18_table, DBAL_COMMIT));
        data_size_ptr = data_size;
    }

    SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs
                    (unit, MDB_EM_ENCODING_TABLE_ENTRY, MDB_EM_ENCODING_TABLE_ENTRY, dbal_physical_table_id, 0, data,
                     data_size_ptr));

    dbal_entry_value_field_arr32_set(unit, entry_handle_mdb_14_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_14_table, DBAL_COMMIT));

    DBAL_HANDLE_FREE(unit, entry_handle_mdb_14_table);

    
    if (dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get(unit, dbal_physical_table_id)->mdb_18_table != DBAL_TABLE_EMPTY)
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_mdb_18_table, DBAL_FIELD_MDB_ITEM_0,
                                         INST_SINGLE, data_size);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_18_table, DBAL_COMMIT));

        DBAL_HANDLE_FREE(unit, entry_handle_mdb_18_table);
    }

    

    SHR_IF_ERR_EXIT(mdb_init_table_em_shift_vmv(unit, dbal_physical_table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_update_vmv(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(mdb_init_em_vmv(unit, dbal_physical_table_id));

    

    SHR_IF_ERR_EXIT(mdb_dh_init_table_em_encoding(unit, dbal_physical_table_id));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_dh_add_em_format(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e dbal_physical_table_id)
{
    int bank_iter;
    uint32 cluster_pos;
    uint32 nof_clusters;
    uint32 global_macro_index;
    uint8 format_count = 0;
    em_key_format_info key_format_info;
    int nof_banks;
    uint32 max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.format_count.get(unit, dbal_physical_table_id, &format_count));

    if (format_count == dnx_data_mdb.em.nof_formats_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "%s has more different key sizes than the number of EM formats (%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id),
                     dnx_data_mdb.em.nof_formats_get(unit));
    }
    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                    key_to_format_map.set(unit, dbal_physical_table_id, key_size, format_count));

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.format_count.set(unit, dbal_physical_table_id, format_count + 1));

    SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.get(unit, dbal_physical_table_id, &key_format_info));

    nof_clusters =
        dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width / MDB_NOF_CLUSTER_ROW_BITS;

    nof_banks = dnx_data_mdb.pdbs.max_nof_interface_dhs_get(unit);

    for (bank_iter = 0; bank_iter < nof_banks; bank_iter++)
    {

        if (key_format_info.em_bank_info[bank_iter].bank_size_used_nof_rows > 0)
        {
            uint32 cluster_index = 0;

            while (cluster_index < max_nof_macro_clusters)
            {

                if (utilex_bitstream_test_bit(&key_format_info.em_bank_info[bank_iter].cluster_mapping, cluster_index))
                {
                    uint32 mask = 0;
                    uint32 clusters_granularity = (nof_clusters << key_format_info.em_bank_info[bank_iter].is_flex);

                    

                    uint32 compare_size =
                        key_size -
                        utilex_log2_round_up(key_format_info.em_bank_info[bank_iter].bank_size_used_nof_rows >>
                                             key_format_info.em_bank_info[bank_iter].is_flex);

                    

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                    (&mask,
                                     (compare_size % dnx_data_mdb.em.format_granularity_get(unit)) +
                                     MDB_INIT_EM_FORMAT_MASK_SHIFT, SAL_UINT32_NOF_BITS - 1));

                    for (cluster_pos = 0; cluster_pos < clusters_granularity; cluster_pos++)
                    {

                        uint32 offset;
                        uint32 offset_shift = (cluster_pos == 0) ? 0 : MDB_INIT_EM_FORMAT_MASK_SHIFT;

                        

                        uint32 entry_handle_mdb_table_6;

                        uint32 format_res;

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_6, &entry_handle_mdb_table_6));

                        offset =
                            ((compare_size /
                              dnx_data_mdb.
                              em.format_granularity_get(unit)) >> offset_shift) & MDB_INIT_EM_FORMAT_OFFSET_MASK;

                        global_macro_index =
                            MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, key_format_info.em_bank_info[bank_iter].macro_type,
                                                          key_format_info.em_bank_info[bank_iter].macro_index);

                        format_res = mask | offset;

                        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_0,
                                                   global_macro_index);
                        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_1,
                                                   cluster_index + cluster_pos);
                        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_2, format_count);

                        dbal_entry_value_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                                     format_res);

                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_table_6, DBAL_COMMIT));

                        DBAL_HANDLE_FREE(unit, entry_handle_mdb_table_6);
                    }
                    cluster_index += clusters_granularity;

                }
                else
                {
                    cluster_index++;
                }
            }
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_em_get_lfsr_and_spn_sizes(
    int unit,
    uint32 key_size,
    dbal_physical_tables_e physical_db_id,
    uint32 *spn_size_index,
    uint32 *spn_enable,
    uint32 *lfsr_size_index)
{
    SHR_FUNC_INIT_VARS(unit);

    *spn_size_index = 0;

    

    if (dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[(*spn_size_index)] > key_size)
    {

        

        *spn_enable = FALSE;
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "the key size %d is smaller than the minimum supported key size %d.\n",
                     key_size, dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[(*spn_size_index)]);
    }
    else
    {
        *spn_enable = TRUE;
        while (((*spn_size_index) < dnx_data_mdb.em.spn_get(unit, physical_db_id)->nof_spn_size - 1)
               && (dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[(*spn_size_index) + 1] <= key_size))
        {
            (*spn_size_index)++;
        }
    }

    

    *lfsr_size_index = 0;
    while (((*lfsr_size_index) < dnx_data_mdb.em.nof_lfsr_sizes_get(unit) - 1)
           && (dnx_data_mdb.em.lfsr_get(unit, (*lfsr_size_index) + 1)->lfsr_size <= key_size))
    {
        (*lfsr_size_index)++;
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_init_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    void *access_info)
{
    dbal_enum_value_field_mdb_db_type_e mdb_db_type;
    const dbal_logical_table_t *dbal_logical_table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set(unit, dbal_table, DBAL_ACCESS_METHOD_MDB, access_info));
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table, &dbal_logical_table));

    mdb_db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->db_type;

    if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        SHR_IF_ERR_EXIT(mdb_init_em_logical_table(unit, dbal_table, dbal_logical_table));
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
    {

        

        SHR_ERR_EXIT(_SHR_E_BADID, "Error. dbal_physical_table %d is not associated with an MDB memory.\n",
                     dbal_logical_table->physical_db_id[0]);
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_em_logical_table(
    int unit,
    dbal_tables_e dbal_table,
    const dbal_logical_table_t * dbal_logical_table)
{
    int ii;
    int key_size_int = 0, payload_size_int = 0;
    uint32 key_size;
    uint32 compare_size;

    
    uint32 entry_handle_mdb_13_table;

    
    uint32 entry_handle_mdb_24_table;
    uint8 is_mdb;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_sizes_get(unit, dbal_table, &key_size_int, &payload_size_int));

    SHR_IF_ERR_EXIT(mdb_dbal_table_is_mdb(unit, dbal_table, &is_mdb));
    if (is_mdb == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. dbal_physical_table %s is not associated with MDB.\n",
                     dbal_physical_table_to_string(unit, dbal_logical_table->physical_db_id[0]));
    }

    

    for (ii = 0; ii < dbal_logical_table->nof_physical_tables; ii++)
    {

        

        dbal_physical_tables_e physical_db_id = dbal_logical_table->physical_db_id[ii];
        int emp_table_enable;
        uint32 em_address_size;
        uint32 app_id;
        uint32 nof_app_id_bits = dnx_data_mdb.em.em_info_get(unit, physical_db_id)->tid_size;
        uint32 enable_spn;
        uint32 age_profile;

                

        uint32 spn_size_index;

                

        uint32 lfsr_size_index;
        uint32 em_format;
        uint8 is_mact;

        if (nof_app_id_bits < dbal_logical_table->app_id_size)
        {
            LOG_CLI((BSL_META
                     ("%s has app_id_size of %d, while the nof_app_id_bits of %s is %d.\n"),
                     dbal_logical_table->table_name,
                     dbal_logical_table->app_id_size,
                     dbal_physical_table_to_string(unit, physical_db_id), nof_app_id_bits));
        }

        is_mact = dbal_logical_table_is_mact(unit, dbal_table);

        
        key_size = UTILEX_MAX(key_size_int, dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[0]);

        SHR_IF_ERR_EXIT(mdb_init_em_get_lfsr_and_spn_sizes
                        (unit, key_size, physical_db_id, &spn_size_index, &enable_spn, &lfsr_size_index));

        

        if ((!is_mact) && (physical_db_id != DBAL_PHYSICAL_TABLE_PPMC)
            && (dnx_data_mdb.em.lfsr_get(unit, lfsr_size_index)->lfsr_size != key_size)
            && (dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[spn_size_index] != key_size))
        {

            

            uint8 lfsr_dist = DNX_DATA_MAX_MDB_PDBS_MAX_KEY_SIZE,
                spn_dist = DNX_DATA_MAX_MDB_PDBS_MAX_KEY_SIZE, min_dist;
            mdb_em_entry_encoding_e entry_encoding = MDB_EM_ENTRY_ENCODING_ONE;
            uint8 vmv_size;
            uint32 entry_addr_bits;
            int entry_size_capacity;
            int result_type_iter;
            
            uint32 free_bits = key_size;

            

            if (lfsr_size_index + 1 < dnx_data_mdb.em.nof_lfsr_sizes_get(unit))
            {
                lfsr_dist = dnx_data_mdb.em.lfsr_get(unit, lfsr_size_index + 1)->lfsr_size - key_size;
            }

            

            if (spn_size_index + 1 < dnx_data_mdb.em.spn_get(unit, physical_db_id)->nof_spn_size)
            {
                spn_dist = dnx_data_mdb.em.spn_get(unit, physical_db_id)->spn_array[spn_size_index + 1] - key_size;
            }

            

            min_dist = lfsr_dist < spn_dist ? lfsr_dist : spn_dist;

            SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, physical_db_id, &entry_addr_bits));

            
            for (result_type_iter = 0; result_type_iter < dbal_logical_table->nof_result_types; result_type_iter++)
            {
                int payload_size;
                int free_bits_temp;

                payload_size = dbal_logical_table->multi_res_info[result_type_iter].entry_payload_size;

                
                if (SHR_IS_BITSET(dbal_logical_table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
                {
                    payload_size -= dbal_logical_table->multi_res_info[0].results_info[0].field_nof_bits;
                }

                

                SHR_IF_ERR_CONT(mdb_em_get_entry_encoding
                                (unit, physical_db_id, key_size, payload_size,
                                 dbal_logical_table->app_id, dbal_logical_table->app_id_size, &entry_encoding));

                if (physical_db_id == DBAL_PHYSICAL_TABLE_PPMC)
                {

                    

                    vmv_size = 0;
                }
                else
                {
                    SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.get(unit, physical_db_id, entry_encoding, &vmv_size));
                }
                entry_size_capacity =
                    dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, physical_db_id)->row_width / (1 << entry_encoding);

                free_bits_temp =
                    entry_size_capacity - (key_size - entry_addr_bits + dbal_logical_table->app_id_size + payload_size +
                                           vmv_size);

                if (free_bits_temp < free_bits)
                {
                    free_bits = free_bits_temp;
                }
            }

            

            if (free_bits >
                dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                    physical_db_id)->max_key_size - key_size -
                dbal_logical_table->app_id_size)
            {
                free_bits =
                    dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                        physical_db_id)->max_key_size - key_size -
                    dbal_logical_table->app_id_size;
            }

            

            if (free_bits >= min_dist)
            {
                key_size += min_dist;
                SHR_IF_ERR_EXIT(mdb_init_em_get_lfsr_and_spn_sizes
                                (unit, key_size, physical_db_id, &spn_size_index, &enable_spn, &lfsr_size_index));
            }
        }

        if (enable_spn == FALSE)
        {
            LOG_CLI((BSL_META
                     ("%s: key_size (%d) is smaller than min SPN.\n"),
                     dbal_logical_table->table_name, dbal_logical_table->key_size));
        }

        compare_size = key_size + dbal_logical_table->app_id_size;

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                        key_to_format_map.get(unit, physical_db_id, compare_size, &em_format));

        if (em_format == MDB_INVALID_EM_FORMAT_VALUE)
        {
            SHR_IF_ERR_EXIT(mdb_init_dh_add_em_format(unit, compare_size, physical_db_id));
            SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                            key_to_format_map.get(unit, physical_db_id, compare_size, &em_format));
        }

        SHR_IF_ERR_EXIT(mdb_em_get_addr_bits(unit, physical_db_id, &em_address_size));

        

        age_profile = is_mact ? MDB_EM_MACT_AGE_PROFILE : MDB_EM_DEFAULT_AGE_PROFILE;

        

        if (dbal_logical_table->app_id >= (1 << dbal_logical_table->app_id_size))
        {
            SHR_ERR_EXIT(_SHR_E_BADID,
                         "%s has the APP ID value of %d, while its app_id_size is %d and the nof_app_id_bits of %s is %d.\n",
                         dbal_logical_table->table_name, dbal_logical_table->app_id,
                         dbal_logical_table->app_id_size, dbal_physical_table_to_string(unit, physical_db_id),
                         nof_app_id_bits);
        }

        emp_table_enable = mdb_em_age_supported(unit, physical_db_id);

        if (emp_table_enable)
        {

            

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.em.mdb_24_info_get(unit,
                                                             dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                                                 physical_db_id)->logical_to_physical)->
                             dbal_table, &entry_handle_mdb_24_table));
        }

        

        for (app_id = dbal_logical_table->app_id;
             app_id < (1 << nof_app_id_bits); app_id += (1 << dbal_logical_table->app_id_size))
        {
            uint32 existing_key_size;

            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                            app_id_size.set(unit, physical_db_id, app_id, dbal_logical_table->app_id_size));
            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.max_payload_size.set(unit, physical_db_id, app_id, payload_size_int));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get(unit, physical_db_id)->mdb_13_table,
                             &entry_handle_mdb_13_table));

            dbal_entry_key_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_KEY_0, app_id);

            dbal_value_field32_request(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE,
                                       &existing_key_size);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_13_table, DBAL_COMMIT));

            if (existing_key_size != 0)
            {

                

                int search_db_index;
                int search_app_id;
                const dbal_logical_table_t *search_dbal_logical_table;

                for (search_db_index = 0; search_db_index < DBAL_NOF_TABLES; search_db_index++)
                {
                    if (search_db_index != dbal_table)
                    {
                        int jj;
                        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, search_db_index, &search_dbal_logical_table));
                        for (jj = 0; jj < search_dbal_logical_table->nof_physical_tables; jj++)
                        {
                            int is_table_active;

                            SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, search_db_index, &is_table_active));

                            if (is_table_active)
                            {
                                if (search_dbal_logical_table->physical_db_id[jj] == physical_db_id)
                                {
                                    for (search_app_id = search_dbal_logical_table->app_id;
                                         search_app_id < (1 << nof_app_id_bits);
                                         search_app_id += (1 << search_dbal_logical_table->app_id_size))
                                    {

                                        

                                        if ((app_id == search_app_id)
                                            && (dbal_logical_table->key_size != search_dbal_logical_table->key_size))
                                        {
                                            LOG_CLI((BSL_META
                                                     ("%s: APP ID %d is already configured by %s for physical table %s.\n"),
                                                     dbal_logical_table->table_name, app_id,
                                                     search_dbal_logical_table->table_name,
                                                     dbal_physical_table_to_string(unit, physical_db_id)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.key_size.set(unit, physical_db_id, app_id, key_size));

            dbal_entry_value_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                         em_format);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                         lfsr_size_index);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE,
                                         spn_size_index);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE,
                                         enable_spn);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_4, INST_SINGLE,
                                         (uint32) (dbal_logical_table->app_id_size));
            dbal_entry_value_field32_set(unit, entry_handle_mdb_13_table, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE, key_size);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_13_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_13_table);

            if (emp_table_enable)
            {

                

                dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_4, app_id,
                                             age_profile);

            }
        }

        if (emp_table_enable)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_24_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_24_table);
        }

        

        if (!(is_mact && dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled)))
        {

            

            

            uint32 entry_handle_mdb_17_table;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_em_dbal_phy_tables_info_get(unit, physical_db_id)->mdb_17_table,
                             &entry_handle_mdb_17_table));

            for (app_id = dbal_logical_table->app_id; app_id < (1 << nof_app_id_bits);
                 app_id += (1 << dbal_logical_table->app_id_size))
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_17_table, DBAL_FIELD_MDB_ITEM_0, app_id, 0x1);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_17_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_17_table);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_em_dbal_to_mdb(
    int unit)
{
    int db_index, nof_tables;
    const dbal_logical_table_t *dbal_logical_table;
    uint32 entry_handle_mdb_15_table;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    

    nof_tables = DBAL_NOF_TABLES;

    for (db_index = 0; db_index < nof_tables; db_index++)
    {
        int is_table_active;
        uint8 is_mdb;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, db_index, &dbal_logical_table));

        SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, db_index, &is_table_active));

        if (!is_table_active)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(mdb_dbal_table_is_mdb(unit, db_index, &is_mdb));

        

        if ((is_mdb == TRUE)
            && dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                   dbal_logical_table->physical_db_id[0])->db_type ==
            DBAL_ENUM_FVAL_MDB_DB_TYPE_EM && (db_index != DBAL_TABLE_GLOBAL_RIF_EM))
        {
            SHR_IF_ERR_EXIT(mdb_init_em_logical_table(unit, db_index, dbal_logical_table));
        }
    }

    

    for (db_index = DBAL_PHYSICAL_TABLE_NONE; db_index < DBAL_NOF_PHYSICAL_TABLES; db_index++)
    {

        if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, db_index)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {

            SHR_IF_ERR_EXIT(mdb_init_em_vmv(unit, db_index));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_15_info_get(unit, db_index)->dbal_table,
                             &entry_handle_mdb_15_table));

            

            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                         DBAL_ENUM_FVAL_MDB_EM_HIT_BIT_MODE_A_OR_B);

            

            if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_dfg_ovf_cam_disabled))
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                             FALSE);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, TRUE);
            }

            

            if (dnx_data_mdb.feature.feature_get(unit, dnx_data_mdb_feature_em_sbus_interface_shutdown))
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_8, INST_SINGLE,
                                             FALSE);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_8, INST_SINGLE, TRUE);
            }
            if (dnx_data_mdb.em.defragmentation_priority_supported_get(unit))
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, 0);
            }
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_4, INST_SINGLE,
                                         MDB_INIT_EM_DFG_PERIOD_VAL);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_6, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_7, INST_SINGLE,
                                         MDB_INIT_CUCKOO_WATCHDOG_STEPS_DEF_VALS);


            
            if (dnx_data_mdb.em.defrag_interrupt_support_get(unit))
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_13, INST_SINGLE,
                                             TRUE);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_15_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_15_table);
        }
    }

    
    SHR_IF_ERR_EXIT(mdb_init_em_age_emp(unit));

    SHR_IF_ERR_EXIT(mdb_step_table_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_access_global_configuration(
    int unit)
{
    uint32 table_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_INIT_ACCESS_CONFIGURATION, &table_handle_id));

    dbal_entry_value_field32_set(unit, table_handle_id, DBAL_FIELD_DISABLE_CLEAR_ON_READ, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, table_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}



shr_error_e
mdb_init_direct_pre_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    {
#if defined(BCM_DNX2_SUPPORT)
        uint32 idx;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FEC_BANK_TO_STAGE_MAP, &entry_handle_id));

        for (idx = 0;
             idx < dnx_data_l3.fec.max_nof_banks_get(unit) / dnx_data_l3.fer.mdb_granularity_ratio_get(unit); idx++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HIERARCHY_LEVEL, idx,
                                         DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
#endif 
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_set_pre_init_values(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_parse_xml_em_vmv(unit));

    SHR_IF_ERR_EXIT(mdb_init_access_global_configuration(unit));

    SHR_IF_ERR_EXIT(mdb_init_direct_pre_init(unit));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_dynamic_memory_access_enable(
    int unit)
{
    
    dbal_tables_e dynamic_memory_dbal_table_arr[] = {
#if defined(BCM_DNX2_SUPPORT)
        DBAL_TABLE_MDB_DYNAMIC_MEMORY_ACCESS,
        DBAL_TABLE_MDB_MACT_DYNAMIC_MEMORY_ACCESS,
        DBAL_TABLE_MDB_KAPS_DYNAMIC_MEMORY_ACCESS_DPC,
        DBAL_TABLE_MDB_KAPS_DYNAMIC_MEMORY_ACCESS_SBC,
        DBAL_TABLE_MDB_DDHA_DYNAMIC_MEMORY_ACCESS_DPC,
        DBAL_TABLE_MDB_DDHA_DYNAMIC_MEMORY_ACCESS_SBC,
        DBAL_TABLE_MDB_DDHB_DYNAMIC_MEMORY_ACCESS,
        DBAL_TABLE_MDB_DHC_DYNAMIC_MEMORY_ACCESS,
#endif
    };
    int dynamic_memory_dbal_table_arr_len =
        sizeof(dynamic_memory_dbal_table_arr) / sizeof(dynamic_memory_dbal_table_arr[0]);
    int dbal_table_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    for (dbal_table_idx = 0; dbal_table_idx < dynamic_memory_dbal_table_arr_len; dbal_table_idx++)
    {
        dbal_tables_e dbal_table_id = dynamic_memory_dbal_table_arr[dbal_table_idx];
        int is_table_active;

        SHR_IF_ERR_EXIT(dbal_tables_is_table_active(unit, dbal_table_id, &is_table_active));

        if (is_table_active == TRUE)
        {
            uint32 entry_handle_id;
            uint8 table_has_core;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

            
            SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, dbal_table_id, DBAL_FIELD_CORE_ID, &table_has_core));
            if (table_has_core == TRUE)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
            }
            dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MEMORY_ACCESS, INST_SINGLE,
                                                       DBAL_PREDEF_VAL_MAX_VALUE);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_eedb_payload_data_size_etps_format_get(
    int unit,
    uint32 etps_format,
    int *data_payload_size)
{
    int ii, jj;
    CONST dbal_logical_table_t *table;
    dbal_table_status_e table_status;
    uint8 is_std_1;
    uint32 raw_etps_format_min, raw_etps_format_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));

    if (!is_std_1)
    {
        
        *data_payload_size = 120;
        SHR_EXIT();
    }

    

    raw_etps_format_min = 0x3 << (dnx_data_pp.ETPP.etps_type_size_bits_get(unit) - 2);
    raw_etps_format_max = (1 << dnx_data_pp.ETPP.etps_type_size_bits_get(unit)) - 1;
    if ((etps_format >= raw_etps_format_min) && (etps_format <= raw_etps_format_max))
    {
        
        *data_payload_size = 120;
        SHR_EXIT();
    }

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        uint8 is_mdb;

        SHR_IF_ERR_EXIT(mdb_dbal_table_is_mdb(unit, ii, &is_mdb));
        if (is_mdb == FALSE)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, ii, &table));

        if (!dbal_table_is_out_lif(table))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, ii, &table_status));
        if (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
        {
            continue;
        }

        for (jj = 0; jj < table->nof_result_types; jj++)
        {
            
            uint32 hw_value_aligned_to_etps_size;
            uint32 etps_format_no_masked_bits;
            uint32 nof_masked_bits;
            uint32 masked_bits_mask;

            if (table->multi_res_info[jj].nof_result_fields == 0)
            {
                continue;
            }

            nof_masked_bits =
                dnx_data_pp.ETPP.etps_type_size_bits_get(unit) -
                table->multi_res_info[jj].results_info[0].field_nof_bits;
            masked_bits_mask = (1 << nof_masked_bits) - 1;
            masked_bits_mask = ~masked_bits_mask;

            hw_value_aligned_to_etps_size = table->multi_res_info[jj].result_type_hw_value[0] << nof_masked_bits;

            etps_format_no_masked_bits = etps_format & masked_bits_mask;

            if ((hw_value_aligned_to_etps_size == etps_format_no_masked_bits) &&
                (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW))
                && (!table->multi_res_info[jj].is_disabled))
            {
                uint8 has_ll;
                int payload_size;

                
                SHR_IF_ERR_EXIT(dnx_lif_mngr_eedb_table_rt_has_linked_list(unit, ii, jj, &has_ll));
                if (has_ll)
                {
                    payload_size = table->multi_res_info[jj].entry_payload_size - MDB_DIRECT_BASIC_ENTRY_SIZE;
                }
                else
                {
                    payload_size = table->multi_res_info[jj].entry_payload_size;
                }
                *data_payload_size = payload_size;
                SHR_EXIT();
            }
        }
    }
    *data_payload_size = -1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_eedb_set_entry_fmt(
    int unit)
{
    uint32 fmt;
    uint32 fmt_iter;
    int data_payload_size_bits;

    uint32 mdb_table_40_handle_id;
    uint32 data_32;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_40, &mdb_table_40_handle_id));

    for (fmt_iter = 0; fmt_iter < (1 << dnx_data_mdb.eedb.entry_format_bits_get(unit)); fmt_iter++)
    {
        SHR_IF_ERR_EXIT(mdb_init_eedb_payload_data_size_etps_format_get(unit, fmt_iter, &data_payload_size_bits));

        if (data_payload_size_bits > MDB_NOF_CLUSTER_ROW_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error. entry_size_bits (%d) must be smaller than the maximal entry_size (%d).\n",
                         data_payload_size_bits, MDB_NOF_CLUSTER_ROW_BITS);
        }
        else
        {

            

            if (data_payload_size_bits <= MDB_DIRECT_BASIC_ENTRY_SIZE)
            {
                fmt = 0;
            }
            else
            {
                fmt = ((data_payload_size_bits + MDB_DIRECT_BASIC_ENTRY_SIZE - 1) / MDB_DIRECT_BASIC_ENTRY_SIZE) - 1;
            }
        }

        data_32 = 0;
        SHR_BITCOPY_RANGE(&data_32, 0, &fmt, 0, dnx_data_mdb.eedb.entry_format_encoding_bits_get(unit));
        dbal_entry_value_field32_set(unit, mdb_table_40_handle_id, DBAL_FIELD_MDB_ITEM_0, fmt_iter, data_32);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_40_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_eedb_set_phase_bank_select(
    int unit,
    int eedb_bank_index,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    soc_reg_above_64_val_t phase_bank_select)
{

    uint32 phase_bank_index = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(mdb_eedb_phase_bank_to_index(unit, eedb_bank_index, phase, &phase_bank_index));

    

    if (phase_bank_index == DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        phase_bank_index = dnx_data_mdb.eedb.phase_bank_select_default_get(unit);
    }

    SHR_BITCOPY_RANGE(phase_bank_select, eedb_bank_index * dnx_data_mdb.eedb.nof_phases_per_eedb_bank_size_get(unit),
                      &phase_bank_index, 0, dnx_data_mdb.eedb.nof_phases_per_eedb_bank_size_get(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_eedb_deinit_bank(
    int unit,
    int bank_idx)
{
    soc_reg_above_64_val_t phase_bank_select;
    soc_reg_above_64_val_t bank_id;
    soc_reg_above_64_val_t type_bank_select;
    uint32 bank_id_iter, inst_iter;
    uint8 bank_id_used[(1 << DNX_DATA_MAX_MDB_EEDB_BANK_ID_BITS)];
    uint32 bank_id_value = 0;

    uint32 mdb_table_9_handle_id;
    uint32 bank_id_32, data_32;
    int nof_inst;
    int nof_inst_a;
    int nof_inst_c;

    uint32 nof_eedb_banks;
    uint32 bank_id_bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_eedb_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
    bank_id_bits = dnx_data_mdb.eedb.bank_id_bits_get(unit);

    

    sal_memset(phase_bank_select, 0, sizeof phase_bank_select);

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get
                    (unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_1, 0, 0, &nof_inst_a));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_9, &mdb_table_9_handle_id));

    for (inst_iter = 0; inst_iter < nof_inst_a; inst_iter++)
    {
        dbal_value_field_arr32_request(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_1, inst_iter, &data_32);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, mdb_table_9_handle_id, DBAL_COMMIT));

        SHR_BITCOPY_RANGE(phase_bank_select, inst_iter * MDB_NOF_PHASES_PER_EEDB_BANK_BITS, &data_32, 0,
                          MDB_NOF_PHASES_PER_EEDB_BANK_BITS);
    }

    sal_memset(type_bank_select, 0, sizeof type_bank_select);

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get
                    (unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_2, 0, 0, &nof_inst_c));

    for (inst_iter = 0; inst_iter < nof_inst_c; inst_iter++)
    {
        dbal_value_field_arr32_request(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_2, inst_iter, &data_32);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, mdb_table_9_handle_id, DBAL_COMMIT));

        SHR_BITCOPY_RANGE(type_bank_select, inst_iter * MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS, &data_32, 0,
                          MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS);
    }

    sal_memset(bank_id, 0, sizeof bank_id);

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_3, 0, 0, &nof_inst));

    for (bank_id_iter = 0; bank_id_iter < nof_inst; bank_id_iter++)
    {
        dbal_value_field_arr32_request(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, bank_id_iter, &bank_id_32);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, mdb_table_9_handle_id, DBAL_COMMIT));

        SHR_BITCOPY_RANGE(bank_id, bank_id_iter * bank_id_bits, &bank_id_32, 0, bank_id_bits);
    }

    SHR_IF_ERR_EXIT(mdb_init_eedb_set_phase_bank_select
                    (unit, bank_idx, DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES, phase_bank_select));

    SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit(type_bank_select, bank_idx));

    

    sal_memset(bank_id_used, 0x0, sizeof(bank_id_used));
    for (bank_id_iter = 0; bank_id_iter < nof_eedb_banks; bank_id_iter++)
    {
        mdb_physical_table_e mdb_physical_table_id;

        

        if (bank_id_iter == bank_idx)
        {
            continue;
        }

        SHR_BITCOPY_RANGE(&bank_id_value, 0, bank_id, bank_id_iter * bank_id_bits, bank_id_bits);

        

        SHR_IF_ERR_EXIT(mdb_init_get_cluster_assoc(unit, MDB_EEDB_BANK, 0, bank_id_iter, &mdb_physical_table_id));

        if (mdb_physical_table_id != MDB_NOF_PHYSICAL_TABLES)
        {
            bank_id_used[bank_id_value] = 1;
        }
    }

    

    for (bank_id_iter = 0; bank_id_iter < (1 << bank_id_bits); bank_id_iter++)
    {
        int bank_id_reverse = (1 << bank_id_bits) - bank_id_iter - 1;
        if (bank_id_used[bank_id_reverse] == 0)
        {
            bank_id_value = bank_id_reverse;
            break;
        }
    }

    SHR_BITCOPY_RANGE(bank_id, bank_idx * bank_id_bits, &bank_id_value, 0, bank_id_bits);

    

    for (inst_iter = 0; inst_iter < nof_inst_a; inst_iter++)
    {
        data_32 = 0;
        SHR_BITCOPY_RANGE(&data_32, 0, phase_bank_select, inst_iter * MDB_NOF_PHASES_PER_EEDB_BANK_BITS,
                          MDB_NOF_PHASES_PER_EEDB_BANK_BITS);

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_1, inst_iter, data_32);
    }

    for (inst_iter = 0; inst_iter < nof_inst_c; inst_iter++)
    {
        data_32 = 0;
        SHR_BITCOPY_RANGE(&data_32, 0, type_bank_select, inst_iter * MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS,
                          MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS);

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_2, inst_iter, data_32);
    }

    for (bank_id_iter = 0; bank_id_iter < nof_inst; bank_id_iter++)
    {
        bank_id_32 = 0;
        SHR_BITCOPY_RANGE(&bank_id_32, 0, bank_id, bank_id_iter * bank_id_bits, bank_id_bits);

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, bank_id_iter, bank_id_32);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_eedb_phase(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index,
    soc_reg_above_64_val_t type_bank_select,
    soc_reg_above_64_val_t phase_bank_select,
    soc_reg_above_64_val_t bank_id)
{
    mdb_physical_table_e ll_table, data_table, data_macro_table;
    int cluster_idx;
    int nof_clusters;
    mdb_cluster_info_t cluster_info;

    

    uint8 bank_id_used[(1 << DNX_DATA_MAX_MDB_EEDB_BANK_ID_BITS)];

    

    uint8 bank_used[DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(bank_id_used, 0x0, sizeof(bank_id_used));
    sal_memset(bank_used, 0x0, sizeof(bank_used));

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, &ll_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_BANK, phase_index, &data_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, phase_index, &data_macro_table));

    

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, ll_table, &nof_clusters));
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        uint32 physical_start_address;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, ll_table, cluster_idx, &cluster_info));
        physical_start_address = cluster_info.start_address;

        

        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit(type_bank_select, cluster_info.cluster_index));

        

        SHR_IF_ERR_EXIT(mdb_init_eedb_set_phase_bank_select
                        (unit, cluster_info.cluster_index, phase_index, phase_bank_select));

        

        SHR_BITCOPY_RANGE(bank_id, cluster_info.cluster_index * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          &physical_start_address,
                          (dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_address_bits),
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));

        

        if (phase_index == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
        {
            uint32 bank_id_value = 0;

            SHR_BITCOPY_RANGE(&bank_id_value, 0, bank_id,
                              cluster_info.cluster_index * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                              dnx_data_mdb.eedb.bank_id_bits_get(unit));

            bank_id_used[bank_id_value] = 1;
            bank_used[cluster_info.cluster_index] = 1;
        }
    }

    

    if (phase_index == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
    {
        uint32 bank_id_iter;
        uint32 phase_bank_value;
        uint32 nof_eedb_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);

        

        for (bank_id_iter = 0; bank_id_iter < nof_eedb_banks; bank_id_iter++)
        {

            

            if (bank_used[bank_id_iter] == 1)
            {
                continue;
            }
            phase_bank_value = 0;
            SHR_BITCOPY_RANGE(&phase_bank_value, 0, phase_bank_select,
                              bank_id_iter * dnx_data_mdb.eedb.nof_phases_per_eedb_bank_size_get(unit),
                              dnx_data_mdb.eedb.nof_phases_per_eedb_bank_size_get(unit));

            

            if (phase_bank_value == dnx_data_mdb.eedb.phase_bank_select_default_get(unit))
            {
                uint32 bank_id_value;
                bank_id_value = 0;
                SHR_BITCOPY_RANGE(&bank_id_value, 0, bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                                  dnx_data_mdb.eedb.bank_id_bits_get(unit));

                

                if (bank_id_used[bank_id_value] == 1)
                {
                    mdb_init_eedb_deinit_bank(unit, bank_id_iter);
                }
            }
        }
    }

    

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_table, &nof_clusters));
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        uint32 physical_start_address;
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, data_table, cluster_idx, &cluster_info));
        physical_start_address = cluster_info.start_address;

        

        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(type_bank_select, cluster_info.cluster_index));

        

        SHR_IF_ERR_EXIT(mdb_init_eedb_set_phase_bank_select
                        (unit, cluster_info.cluster_index, phase_index, phase_bank_select));

        

        SHR_BITCOPY_RANGE(bank_id, cluster_info.cluster_index * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          &physical_start_address,
                          dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_address_bits,
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_eedb_pointer_size_verify(
    int unit)
{

    

    int min_eedb_pointer_bits;
    dbal_physical_tables_e dbal_table_iter;
    int total_eedb_data_entries = 0;

    int eedb_pointer_bits;

    SHR_FUNC_INIT_VARS(unit);

    eedb_pointer_bits = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);

    

    for (dbal_table_iter = DBAL_PHYSICAL_TABLE_EEDB_1; dbal_table_iter <= DBAL_PHYSICAL_TABLE_EEDB_8; dbal_table_iter++)
    {
        int phase_capacity;

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, dbal_table_iter, &phase_capacity));
        total_eedb_data_entries += phase_capacity;
    }

    

    total_eedb_data_entries /= dnx_data_mdb.eedb.nof_phase_per_mag_get(unit);

    min_eedb_pointer_bits = utilex_log2_round_up(total_eedb_data_entries);

    if (min_eedb_pointer_bits > eedb_pointer_bits)
    {

        

        if (min_eedb_pointer_bits <= DNX_DATA_MAX_LIF_OUT_LIF_GLEM_RESULT)
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "MDB EEDB pointer size is too small, consider increasing it by setting soc property %s. Required: %d. Current: %d. \n"),
                      spn_OUTLIF_NOF_BITS, min_eedb_pointer_bits, eedb_pointer_bits));
        }
        else
        {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U
                      (unit,
                       "MDB EEDB pointer size is too small, EEDB data clusters exceed maximal capacity. Required: %d. Current: %d. Max: %d. \n"),
                      min_eedb_pointer_bits, eedb_pointer_bits, DNX_DATA_MAX_LIF_OUT_LIF_GLEM_RESULT));
        }
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_init_eedb(
    int unit)
{
    soc_reg_above_64_val_t reg_above64_val;

    

    soc_reg_above_64_val_t type_bank_select;

    

    soc_reg_above_64_val_t phase_bank_select;

    

    soc_reg_above_64_val_t bank_id;

    uint32 mdb_table_9_handle_id;

    int nof_inst, instance_idx;
    uint32 data_32;

    int eedb_pointer_bits = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);
    int phase_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    sal_memset(reg_above64_val, 0x0, sizeof(reg_above64_val));
    sal_memset(type_bank_select, 0x0, sizeof(type_bank_select));
    sal_memset(phase_bank_select, 0x0, sizeof(phase_bank_select));
    sal_memset(bank_id, 0xFF, sizeof(reg_above64_val));

    

    SHR_IF_ERR_EXIT(mdb_init_eedb_pointer_size_verify(unit));

    SHR_IF_ERR_EXIT(mdb_init_eedb_set_entry_fmt(unit));

    

    

    

    if (eedb_pointer_bits == 20)
    {
        reg_above64_val[0] = MDB_EEDB_POINTER_FORMAT_20BIT_RES_4k;
    }
    else if (eedb_pointer_bits == 19)
    {
        reg_above64_val[0] = MDB_EEDB_POINTER_FORMAT_19BIT_RES_2k;
    }
    else if (eedb_pointer_bits == 18)
    {
        reg_above64_val[0] = MDB_EEDB_POINTER_FORMAT_18BIT_RES_1k;
    }
    else if (eedb_pointer_bits == 17)
    {
        reg_above64_val[0] = MDB_EEDB_POINTER_FORMAT_17BIT_RES_1k;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unsupported EEDB pointer format bits: %d.\n", eedb_pointer_bits);
    }

    

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_9, &mdb_table_9_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, reg_above64_val[0]);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

    
    for (phase_index = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1; phase_index < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
         phase_index++)
    {
        dbal_tables_e bucket_enable_dbal_table =
            dnx_data_mdb.eedb.phase_ll_bucket_enable_get(unit, phase_index)->bucket_enable_dbal_table;

        if (bucket_enable_dbal_table != DBAL_NOF_TABLES)
        {
            uint32 mdb_table_26_ll_handle_id;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, bucket_enable_dbal_table, &mdb_table_26_ll_handle_id));
            dbal_entry_value_field32_set(unit, mdb_table_26_ll_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, mdb_table_26_ll_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_26_ll_handle_id, DBAL_COMMIT));
        }
    }

    for (phase_index = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1; phase_index < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
         phase_index++)
    {

        

        SHR_IF_ERR_EXIT(mdb_init_eedb_phase(unit, phase_index, type_bank_select, phase_bank_select, bank_id));
    }

    

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_1, 0, 0, &nof_inst));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_9, &mdb_table_9_handle_id));

    for (instance_idx = 0; instance_idx < nof_inst; instance_idx++)
    {
        data_32 = 0;
        SHR_BITCOPY_RANGE(&data_32, 0, phase_bank_select, instance_idx * MDB_NOF_PHASES_PER_EEDB_BANK_BITS,
                          MDB_NOF_PHASES_PER_EEDB_BANK_BITS);

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_1, instance_idx, data_32);

    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

    

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_2, 0, 0, &nof_inst));

    for (instance_idx = 0; instance_idx < nof_inst; instance_idx++)
    {
        data_32 = 0;
        SHR_BITCOPY_RANGE(&data_32, 0, type_bank_select, instance_idx * MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS,
                          MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS);

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_2, instance_idx, data_32);

    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

    

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, DBAL_TABLE_MDB_9, DBAL_FIELD_MDB_ITEM_3, 0, 0, &nof_inst));

    for (instance_idx = 0; instance_idx < nof_inst; instance_idx++)
    {
        data_32 = 0;
        SHR_BITCOPY_RANGE(&data_32, 0, bank_id, instance_idx * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, instance_idx, data_32);

    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



int
dnx_mdb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(mdb_init_dynamic_memory_access_enable(unit));

        SHR_IF_ERR_EXIT(mdb_init_set_pre_init_values(unit));

        SHR_IF_ERR_EXIT(mdb_init_direct(unit));

        SHR_IF_ERR_EXIT(mdb_dh_init(unit));

        SHR_IF_ERR_EXIT(mdb_direct_init(unit));

        
        SHR_IF_ERR_EXIT(mdb_em_init_shadow_db(unit));

        SHR_IF_ERR_EXIT(mdb_init_em_dbal_to_mdb(unit));

        SHR_IF_ERR_EXIT(mdb_em_init_overrides(unit));
    }

    {
        SHR_IF_ERR_EXIT(mdb_lpm_init(unit));

        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(mdb_init_lpm_xpt_inner(unit));

        }
    }

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(mdb_init_eedb(unit));

        SHR_IF_ERR_EXIT(mdb_eedb_hitbit_init(unit));

        SHR_IF_ERR_EXIT(mdb_init_verify_tables(unit));
    }

#ifdef ADAPTER_SERVER_MODE
    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(adapter_mdb_init(unit));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}



int
dnx_mdb_deinit(
    int unit)
{
    int no_sync_flag = 0;

    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(mdb_lpm_deinit(unit, no_sync_flag));
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_set_cluster_assoc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    mdb_physical_table_e mdb_physical_table_id)
{
    int index;
    mdb_physical_table_e dst_mdb_table_id;

    SHR_FUNC_INIT_VARS(unit);

    index = MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macro_type, macro_idx, cluster_idx);

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                    pdb.get(unit, macro_type, index, &dst_mdb_table_id));

    

    if (((mdb_physical_table_id != MDB_NOF_PHYSICAL_TABLES)
         && (dst_mdb_table_id == MDB_NOF_PHYSICAL_TABLES)) || (mdb_physical_table_id == MDB_NOF_PHYSICAL_TABLES))
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                        pdb.set(unit, macro_type, index, mdb_physical_table_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE,
                     "Error. MDB resource of type %d, macro index %d, cluster index %d is associated with MDB table %d and can't be associated with MDB table %d.\n",
                     macro_type, macro_idx, cluster_idx, dst_mdb_table_id, mdb_physical_table_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_lpm_xpt_inner(
    int unit)
{
    uint8 db_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (db_idx = 0; db_idx < dnx_data_mdb.kaps.nof_dbs_get(unit); db_idx++)
    {
        
        if (dnx_data_mdb.kaps.db_info_get(unit, db_idx)->dbal_table_db_atr == DBAL_TABLE_EMPTY)
        {
            continue;
        }

        {
            int big_kaps_enabled;
            

            uint32 mdb_table_28_kaps_handle_id;

            

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.kaps.db_info_get(unit, db_idx)->dbal_table_db_atr,
                             &mdb_table_28_kaps_handle_id));
            dbal_entry_value_field32_set(unit, mdb_table_28_kaps_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0x0);

            

            SHR_IF_ERR_EXIT(mdb_lpm_xpt_big_kaps_enabled(unit, db_idx, &big_kaps_enabled));

            if (big_kaps_enabled == TRUE)
            {
                 
                dbal_entry_value_field32_set(unit, mdb_table_28_kaps_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                             0x1);
            }
            else
            {
                dbal_entry_value_field32_set(unit, mdb_table_28_kaps_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                             0x0);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_28_kaps_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, mdb_table_28_kaps_handle_id));
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_em_mact_age_refresh_mode_set(
    int unit,
    mdb_physical_table_e mdb_table,
    uint32 mode)
{
    uint32 entry_handle_table_24;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_data_mdb.em.mdb_24_info_get(unit, mdb_table)->dbal_table, &entry_handle_table_24));

    dbal_entry_value_field32_set(unit, entry_handle_table_24, DBAL_FIELD_MDB_ITEM_2, MDB_EM_MACT_AGE_PROFILE, mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_24, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_mact_age_refresh_mode_get(
    int unit,
    uint32 *mode)
{
    uint32 entry_handle_table_24;
    uint32 hitbit_mode;
    mdb_physical_table_e mdb_table = MDB_NOF_PHYSICAL_TABLES;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

#if defined(BCM_DNX2_SUPPORT)
    mdb_table = MDB_PHYSICAL_TABLE_LEM;
#endif

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_data_mdb.em.mdb_24_info_get(unit, mdb_table)->dbal_table, &entry_handle_table_24));

    dbal_value_field32_request(unit, entry_handle_table_24, DBAL_FIELD_MDB_ITEM_2, MDB_EM_MACT_AGE_PROFILE,
                               &hitbit_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_table_24, DBAL_COMMIT));
    *mode = hitbit_mode;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

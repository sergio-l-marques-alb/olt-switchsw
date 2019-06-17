/*
 * ! \file mdb_init.c $Id$ Contains all of the MDB initialization sequences.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/mdb_sbusdma_desc.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
/*
 * }
 */

/*
 * Defines
 * {
 */
#define MDB_DIRECT_PHYSICAL_ADDRESS_MAX_VAL      (4)

#define MDB_INIT_CUCKOO_WATCHDOG_STEPS_DEF_VALS  (0xFFFFFFFF)

#define MDB_MAX_NOF_MDB_TABLES_PER_DBAL          (3)
#define MDB_MAX_NOF_TABLES_RESOURCE_SHARE        (2)

/*
 * }
 */

/*
 * MACROs
 * {
 */

#define MDB_EM_SW_ACCESS mdb_em_db
#define MDB_TABLE_IS_EEDB(mdb_table) ((mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA) ||\
                                (mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA) ||\
                                (mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA) ||\
                                (mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA))

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
/*
 * Structures
 * {
 */

/*
 * }
 */
/*
 * Globals
 * {
 */

const dbal_enum_value_field_mdb_physical_table_e mdb_em_emp_table_to_mdb_emp_table[DBAL_NOF_ENUM_MDB_EMP_TABLE_VALUES] =
    { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3,
    DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4
};

/*
 * }
 */

shr_error_e
mdb_em_age_emp_init(
    int unit)
{
    uint32 entry_handle_table_19;
    uint32 entry_handle_table_21;
    uint32 entry_handle_table_22;

    uint32 entry_handle_table_24;
    int emp_table, age_profile;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (emp_table = 0; emp_table < DBAL_NOF_ENUM_MDB_EMP_TABLE_VALUES; emp_table++)
    {
        dbal_enum_value_field_mdb_physical_table_e mdb_table = mdb_em_emp_table_to_mdb_emp_table[emp_table];

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_emp_tables_info_get(unit, mdb_table)->mdb_19_table,
                         &entry_handle_table_19));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_21_info_get(unit, mdb_table)->dbal_table, &entry_handle_table_21));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_emp_tables_info_get(unit, mdb_table)->mdb_22_table,
                         &entry_handle_table_22));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_24_info_get(unit, mdb_table)->dbal_table, &entry_handle_table_24));

        for (age_profile = 0; age_profile < dnx_data_mdb.em.nof_aging_profiles_get(unit); age_profile++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_table_19, DBAL_FIELD_MDB_ITEM_0, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          emp_table)->elephant_values);
            dbal_entry_value_field32_set(unit, entry_handle_table_19, DBAL_FIELD_MDB_ITEM_1, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile, emp_table)->mouse_values);
            dbal_entry_value_field32_set(unit, entry_handle_table_19, DBAL_FIELD_MDB_ITEM_2, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          emp_table)->age_max_values);
            dbal_entry_value_field32_set(unit, entry_handle_table_19, DBAL_FIELD_MDB_ITEM_3, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          emp_table)->increment_values);
            dbal_entry_value_field32_set(unit, entry_handle_table_19, DBAL_FIELD_MDB_ITEM_4, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          emp_table)->decrement_values);
            dbal_entry_value_field32_set(unit, entry_handle_table_19, DBAL_FIELD_MDB_ITEM_5, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile, emp_table)->out_values);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_0, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile, emp_table)->init_value);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_1, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile, emp_table)->global_value);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_2, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile, emp_table)->global_mask);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_3, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          emp_table)->external_profile);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_4, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile, emp_table)->aging_disable);
            dbal_entry_value_field32_set(unit, entry_handle_table_21, DBAL_FIELD_MDB_ITEM_5, age_profile,
                                         dnx_data_mdb.em.em_aging_cfg_get(unit, age_profile,
                                                                          emp_table)->elephant_disable);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_19, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_21, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_table_19);
        DBAL_HANDLE_FREE(unit, entry_handle_table_21);

        dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_table_22, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_22, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_table_22);

        dbal_entry_value_field32_set(unit, entry_handle_table_24, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE,
                                     MDB_EM_HIT_BIT_MODE_A_OR_B);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_table_24, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_table_24);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_verify_tables_init(
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

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, db_index, &dbal_logical_table));

        if (dbal_logical_table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        if (dbal_logical_table->mdb_image_type == DBAL_MDB_IMG_STD_1_NOT_ACTIVE)
        {
            continue;
        }

        if ((dbal_logical_table->access_method != DBAL_ACCESS_METHOD_MDB)
            || (dbal_logical_table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_TCAM)
            || (dbal_logical_table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_PPMC))
        {
            continue;
        }

        mdb_db_type = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->db_type;

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            key_size = MDB_EM_KEY_MIN_OR_ABOVE(unit, dbal_logical_table->key_size);
        }
        else
        {
            key_size = dbal_logical_table->key_size;
        }
        payload_size = dbal_logical_table->max_payload_size;
        if (dbal_logical_table->core_mode == DBAL_CORE_MODE_DPC)
        {
            key_size -= dbal_logical_table->core_id_nof_bits;
        }
        if (dbal_logical_table->result_type_mapped_to_sw)
        {
            payload_size -= dbal_logical_table->multi_res_info[0].results_info[0].field_nof_bits;
        }

        if (key_size > dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size)
        {
            LOG_CLI((BSL_META("%s: key_size(%d) is larger than allowed by the pipeline(%d).\n"),
                     dbal_logical_table->table_name, key_size,
                     dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size));
        }

        if (payload_size >
            dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_payload_size)
        {
            LOG_CLI((BSL_META("%s: payload_size(%d) is larger than allowed by the pipeline(%d).\n"),
                     dbal_logical_table->table_name, payload_size,
                     dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_payload_size));
        }

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM)
        {
            continue;
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
        {
#if defined(INCLUDE_KBP)
            if (TRUE)
            {
                if (key_size > dnx_data_mdb.kaps.key_width_in_bits_get(unit))
                {
                    LOG_CLI((BSL_META
                             ("%s: KAPS key is too large. key_size(%d) > dnx_data_mdb.kaps.key_width_in_bits_get(unit)(%d).\n"),
                             dbal_logical_table->table_name, key_size, dnx_data_mdb.kaps.key_width_in_bits_get(unit)));
                }

                if (payload_size > dnx_data_mdb.kaps.ad_width_in_bits_get(unit))
                {
                    LOG_CLI((BSL_META
                             ("%s: KAPS payload is too large. payload_size(%d) > dnx_data_mdb.kaps.ad_width_in_bits_get(unit)(%d).\n"),
                             dbal_logical_table->table_name, payload_size,
                             dnx_data_mdb.kaps.ad_width_in_bits_get(unit)));
                }
            }
            else
#endif /* defined(INCLUDE_KBP) */
            {
                LOG_CLI((BSL_META("Compilation does not include KBPSDK flags\n")));
            }
        }
        else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            uint32 configured_key_size;

            SHR_IF_ERR_EXIT(mdb_em_get_key_size
                            (unit, dbal_logical_table->physical_db_id[0], dbal_logical_table->app_id,
                             &configured_key_size));
            if (key_size > configured_key_size)
            {
                LOG_CLI((BSL_META("%s: key_size(%d) is larger than configured during init (%d).\n"),
                         dbal_logical_table->table_name, key_size, configured_key_size));
            }

            if (key_size + dbal_logical_table->app_id_size >
                dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size)
            {
                LOG_CLI((BSL_META("%s: key_size(%d) + app_id_size(%d) > pipeline_max_key_size(%d).\n"),
                         dbal_logical_table->table_name, key_size, dbal_logical_table->app_id_size,
                         dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_logical_table->physical_db_id[0])->max_key_size));
            }

            SHR_IF_ERR_CONT(mdb_em_get_entry_encoding
                            (unit, dbal_logical_table->physical_db_id[0], key_size, payload_size,
                             dbal_logical_table->app_id, dbal_logical_table->app_id_size, NULL));

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

shr_error_e
mdb_init_direct_table(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_tables_e mdb_table_11 = dnx_data_mdb.pdbs.mdb_11_info_get(unit, mdb_physical_table)->dbal_table;

    if (mdb_table_11 != DBAL_TABLE_EMPTY)
    {
        uint8 multiplier = 0;
        uint8 divider = 0;
        uint8 address_space_field_val = 0;
        uint32 mapping_compare_bits;
        uint32 entry_handle_id;
        int basic_size;
        int row_size;
        int cluster_idx;
        int max_end_address = 0;
        int address_space_max_bits;
        int address_space_bits;
        int max_macro_mapping_index, macro_mapping_index;
        int mult_iter;
        int mult_array_size;
        int nof_clusters, start_address, end_address;
        uint8 macro_idx;
        mdb_macro_types_e macro_type;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, mdb_table_11, &entry_handle_id));

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters));

        row_size = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->row_width;

        max_macro_mapping_index = dnx_data_mdb.pdbs.mdb_11_info_get(unit, mdb_physical_table)->mdb_item_0_array_size;

        divider = MDB_DIRECT_ROW_SIZE_TO_DIVIDER(row_size);

        if ((mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI)
            || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI))
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

        if (MDB_TABLE_IS_EEDB(mdb_physical_table))
        {
            address_space_bits = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);
            address_space_field_val = address_space_max_bits - address_space_bits;
        }
        else
        {
            address_space_bits = address_space_max_bits;
            while (((1 << address_space_bits) >= max_end_address)
                   && (address_space_field_val < MDB_DIRECT_PHYSICAL_ADDRESS_MAX_VAL))
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

        if ((mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1)
            || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2)
            || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3))
        {
            uint32 max_fec_id;
            SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
            if (max_fec_id > dnx_data_l3.fec.max_fec_id_26k_granularity_get(unit)
                && address_space_bits >
                dnx_data_mdb.direct.physical_address_max_bits_support_26k_granularity_fec_alloc_get(unit))
            {

                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. FEC address space %d is larger than the max address size supported (%d) when working in 26k FEC allocation granularity.\n",
                             address_space_bits,
                             dnx_data_mdb.direct.physical_address_max_bits_support_26k_granularity_fec_alloc_get(unit));
            }

        }

        mult_array_size = dnx_data_mdb.pdbs.mdb_11_info_get(unit, mdb_physical_table)->mdb_item_3_array_size;

        for (mult_iter = 0; mult_iter < mult_array_size; mult_iter++)
        {

            if (mult_array_size == 1)
            {
                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_physical_table, &basic_size));
            }
            else
            {
                dbal_enum_value_field_mdb_physical_table_e dbal_physical_table_id_phase;
                if (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
                {
                    dbal_physical_table_id_phase =
                        mult_iter ==
                        0 ? DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA :
                        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA;
                }
                else if (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
                {
                    dbal_physical_table_id_phase =
                        mult_iter ==
                        0 ? DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA :
                        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA;
                }
                else if (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
                {
                    dbal_physical_table_id_phase =
                        mult_iter ==
                        0 ? DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA :
                        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA;
                }
                else
                {
                    dbal_physical_table_id_phase =
                        mult_iter ==
                        0 ? DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA :
                        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA;
                }

                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, dbal_physical_table_id_phase, &basic_size));
            }

            multiplier = MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(basic_size);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3, mult_iter, multiplier);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_4, mult_iter, multiplier);
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, divider);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                     address_space_field_val);

        mapping_compare_bits = utilex_log2_round_up(max_macro_mapping_index);

        for (macro_mapping_index = 0; macro_mapping_index < max_macro_mapping_index; macro_mapping_index++)
        {
            uint32 macro_mapping_val = 0;

            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                start_address.get(unit, mdb_physical_table, cluster_idx, &start_address));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                end_address.get(unit, mdb_physical_table, cluster_idx, &end_address));
                if (((start_address >> (address_space_bits - mapping_compare_bits)) <= macro_mapping_index)
                    && ((end_address >> (address_space_bits - mapping_compare_bits)) > macro_mapping_index))
                {
                    break;
                }
            }

            if (cluster_idx != nof_clusters)
            {
                int adjusted_cluster_macro_idx;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                macro_type.get(unit, mdb_physical_table, cluster_idx, &macro_type));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                macro_index.get(unit, mdb_physical_table, cluster_idx, &macro_idx));
                adjusted_cluster_macro_idx =
                    macro_idx + (macro_type == MDB_MACRO_A ? 0 : dnx_data_mdb.dh.nof_macroes_type_A_get(unit));

                macro_mapping_val =
                    dnx_data_mdb.dh.dh_info_get(unit,
                                                mdb_physical_table)->table_macro_interface_mapping
                    [adjusted_cluster_macro_idx];
            }
            else if (MDB_TABLE_IS_EEDB(mdb_physical_table))
            {

                uint32 eedb_bank_pos_in_array =
                    dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;
                macro_mapping_val =
                    dnx_data_mdb.dh.dh_info_get(unit,
                                                mdb_physical_table)->table_macro_interface_mapping
                    [eedb_bank_pos_in_array];
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, macro_mapping_index,
                                         macro_mapping_val);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        if ((mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1)
            || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2)
            || (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3))
        {
            int address_mapping_index;
            uint32 entry_handle_mdb_12_table;

            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_physical_table, &basic_size));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.pdbs.mdb_12_info_get(unit, mdb_physical_table)->dbal_table,
                             &entry_handle_mdb_12_table));

            for (address_mapping_index = 0;
                 address_mapping_index < (1 << dnx_data_mdb.direct.fec_address_mapping_bits_get(unit));
                 address_mapping_index++)
            {
                uint32 val = address_mapping_index;

                dbal_entry_value_field32_set(unit, entry_handle_mdb_12_table, DBAL_FIELD_MDB_ITEM_0, val, val);
            }

#ifndef ADAPTER_SERVER_MODE
            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx += 2)
            {
                uint32 map_value;
                uint8 cluster_idx_in_macro;
                int map_index, map_index_start, map_index_end;
                int address_mapping_offset;
                uint32 relevant_bits_mask, relevant_bits_offset;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                start_address.get(unit, mdb_physical_table, cluster_idx, &start_address));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                end_address.get(unit, mdb_physical_table, cluster_idx, &end_address));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                cluster_index.get(unit, mdb_physical_table, cluster_idx, &cluster_idx_in_macro));

                address_mapping_offset = address_space_bits - dnx_data_mdb.direct.fec_address_mapping_bits_get(unit);

                map_index_start = start_address >> address_mapping_offset;
                map_index_end = end_address >> address_mapping_offset;

                relevant_bits_offset =
                    dnx_data_mdb.direct.fec_address_mapping_bits_offset_get(unit) - (address_space_bits -
                                                                                     dnx_data_mdb.
                                                                                     direct.fec_address_mapping_bits_get
                                                                                     (unit));
                relevant_bits_mask =
                    ~(((1 << dnx_data_mdb.direct.fec_address_mapping_bits_relevant_get(unit)) -
                       1) << relevant_bits_offset);

                for (map_index = map_index_start; map_index < map_index_end; map_index++)
                {
                    map_value = (map_index & relevant_bits_mask) | ((cluster_idx_in_macro / 2) << relevant_bits_offset);

                    dbal_entry_value_field32_set(unit, entry_handle_mdb_12_table, DBAL_FIELD_MDB_ITEM_0, map_index,
                                                 map_value);
                }
            }
#endif /* ifndef ADAPTER_SERVER_MODE */

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_12_table, DBAL_COMMIT));

            DBAL_HANDLE_FREE(unit, entry_handle_mdb_12_table);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_set_cluster_alloc(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_info_t * cluster_info_p,
    mdb_eedb_mem_type_e mdb_eedb_type,
    int table_entry_size,
    int current_output_cluster_idx,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS])
{

    int entry_iter;
    int entry_size;
    uint32 row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;

    SHR_FUNC_INIT_VARS(unit);

    if (current_output_cluster_idx >= MDB_MAX_NOF_CLUSTERS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. current_output_cluster_idx %d is larger than MDB_MAX_NOF_CLUSTERS %d.\n",
                     current_output_cluster_idx, MDB_MAX_NOF_CLUSTERS);
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

    for (entry_iter = 0; entry_iter < (MDB_NOF_ROWS_IN_MACRO_A_CLUSTER / MDB_NOF_ROWS_IN_EEDB_BANK); entry_iter++)
    {
        if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
        {
            clusters[current_output_cluster_idx].phase_alloc_logical_entries[entry_iter] =
                cluster_info_p->phase_alloc_rows[entry_iter / (row_width / entry_size)];
        }
        else
        {
            clusters[current_output_cluster_idx].phase_alloc_logical_entries[entry_iter] =
                cluster_info_p->phase_alloc_rows[0];
        }
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
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (macro_type == MDB_MACRO_A)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_a_cluster_assoc.get(unit,
                                                                                 (macro_idx *
                                                                                  dnx_data_mdb.
                                                                                  dh.nof_macro_clusters_get(unit)) +
                                                                                 cluster_idx, mdb_physical_table_id));
    }
    else if (macro_type == MDB_MACRO_B)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_b_cluster_assoc.get(unit,
                                                                                 (macro_idx *
                                                                                  dnx_data_mdb.
                                                                                  dh.nof_macro_clusters_get(unit)) +
                                                                                 cluster_idx, mdb_physical_table_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_assoc.get(unit, cluster_idx, mdb_physical_table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_get_table_resources(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS],
    int *nof_valid_clusters)
{
    int current_output_cluster_idx = 0;
    dbal_enum_value_field_mdb_physical_table_e mdb_table = DBAL_PHYSICAL_TABLE_EEDB_1;
    dbal_enum_value_field_mdb_physical_table_e physical_mdb_tables[MDB_MAX_NOF_MDB_TABLES_PER_DBAL];
    int num_of_physical_table, table_iter;
    int cluster_idx;
    int eedb_bank;
    int nof_clusters;
    mdb_macro_types_e macro_type;
    uint8 info_cluster_index;
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_physical_table_id >= DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid DBAL physical table %d.\n", dbal_physical_table_id);
    }

    if ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
        && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        int phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;

        SHR_IF_ERR_EXIT(mdb_eedb_translate
                        (unit, MDB_EEDB_MEM_TYPE_LL, phase_index, MDB_EEDB_BANK, &physical_mdb_tables[0]));
        SHR_IF_ERR_EXIT(mdb_eedb_translate
                        (unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, MDB_EEDB_BANK, &physical_mdb_tables[1]));
        SHR_IF_ERR_EXIT(mdb_eedb_translate
                        (unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, ~MDB_EEDB_BANK, &physical_mdb_tables[2]));
        num_of_physical_table = MDB_MAX_NOF_MDB_TABLES_PER_DBAL;
    }
    else if ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_KAPS_1)
             && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_KAPS_2))
    {
        num_of_physical_table = 2;

        if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_KAPS_1)
        {
            physical_mdb_tables[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;
            physical_mdb_tables[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        }
        else
        {
            physical_mdb_tables[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;
            physical_mdb_tables[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        }
    }
    else
    {
        num_of_physical_table = 1;
        physical_mdb_tables[0] = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

        if (physical_mdb_tables[0] == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. DBAL physical table %d is not associated with an MDB table.\n",
                         dbal_physical_table_id);
        }
    }

    for (table_iter = 0; table_iter < num_of_physical_table; table_iter++)
    {
        mdb_table = physical_mdb_tables[table_iter];

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table, &nof_clusters));

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            mdb_cluster_info_t cluster_info;
            mdb_eedb_mem_type_e mdb_eedb_type = MDB_NOF_EEDB_MEM_TYPES;
            int entry_size;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table, cluster_idx, &cluster_info));

            if ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
                && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
            {
                uint32 phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
                uint32 payload_type;
                SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase_index, &payload_type));
                entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);
                if (table_iter == 0)
                {
                    mdb_eedb_type = MDB_EEDB_MEM_TYPE_LL;
                }
                else
                {
                    mdb_eedb_type = MDB_EEDB_MEM_TYPE_PHY;
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_table, &entry_size));
            }

            SHR_IF_ERR_EXIT(mdb_set_cluster_alloc
                            (unit, dbal_physical_table_id, &cluster_info, mdb_eedb_type, entry_size,
                             current_output_cluster_idx, clusters));

            current_output_cluster_idx++;
        }
    }

    if ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
        && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        for (eedb_bank = 0; eedb_bank < dnx_data_mdb.eedb.nof_eedb_banks_get(unit); eedb_bank++)
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
                        dbal_enum_value_field_mdb_physical_table_e mdb_table_assoc;
                        dbal_enum_value_field_mdb_physical_table_e mdb_table_iter;
                        mdb_eedb_mem_type_e data_ll_iter;

                        SHR_IF_ERR_EXIT(mdb_init_get_cluster_assoc
                                        (unit, MDB_EEDB_BANK, 0, eedb_bank, &mdb_table_assoc));

                        for (data_ll_iter = MDB_EEDB_MEM_TYPE_PHY; data_ll_iter < MDB_NOF_EEDB_MEM_TYPES;
                             data_ll_iter++)
                        {
                            SHR_IF_ERR_EXIT(mdb_eedb_translate
                                            (unit, data_ll_iter, phase_index_iter, MDB_EEDB_BANK, &mdb_table_iter));

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
                            SHR_IF_ERR_EXIT(mdb_set_cluster_alloc
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
                    cluster_info.start_address = 0;
                    cluster_info.end_address = MDB_NOF_ROWS_IN_EEDB_BANK;

                    SHR_IF_ERR_EXIT(mdb_set_cluster_alloc
                                    (unit, dbal_physical_table_id, &cluster_info,
                                     MDB_EEDB_MEM_TYPE_LL,
                                     MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B),
                                     current_output_cluster_idx, clusters));
                    current_output_cluster_idx++;
                }
            }
        }
    }

    if (((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_5)
         && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
        || ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_FEC_1)
            && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_FEC_3)))
    {
        dbal_enum_value_field_mdb_physical_table_e mdb_table_pairs[MDB_MAX_NOF_TABLES_RESOURCE_SHARE];
        dbal_physical_tables_e dbal_table_pairs[MDB_MAX_NOF_TABLES_RESOURCE_SHARE];

        mdb_macro_types_e macro_types[MDB_MAX_NOF_TABLES_RESOURCE_SHARE];
        int macro_idxs[MDB_MAX_NOF_TABLES_RESOURCE_SHARE];

        int num_of_pair_tables = 0;
        int table_iterator;
        int entry_size;
        mdb_eedb_mem_type_e mem_type = MDB_NOF_EEDB_MEM_TYPES;

        if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_5)
            || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_6))
        {
            mdb_table_pairs[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_EEDB_7;
            macro_types[0] = MDB_MACRO_A;
            macro_idxs[0] = 1;

            num_of_pair_tables = 1;
            mem_type = MDB_EEDB_MEM_TYPE_PHY;
        }
        else if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_7)
                 || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_EEDB_8))
        {
            mdb_table_pairs[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_EEDB_5;
            macro_types[0] = MDB_MACRO_A;
            macro_idxs[0] = 1;

            num_of_pair_tables = 1;
            mem_type = MDB_EEDB_MEM_TYPE_PHY;
        }
        else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_1)
        {
            mdb_table_pairs[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_FEC_2;
            macro_types[0] = MDB_MACRO_B;
            macro_idxs[0] = 7;

            num_of_pair_tables = 1;
            mem_type = MDB_NOF_EEDB_MEM_TYPES;
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, DBAL_PHYSICAL_TABLE_FEC_1, &entry_size));
        }
        else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_2)
        {
            mdb_table_pairs[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_FEC_1;
            macro_types[0] = MDB_MACRO_B;
            macro_idxs[0] = 7;

            mdb_table_pairs[1] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3;
            dbal_table_pairs[1] = DBAL_PHYSICAL_TABLE_FEC_3;
            macro_types[1] = MDB_MACRO_A;
            macro_idxs[1] = 5;

            num_of_pair_tables = 2;
            mem_type = MDB_NOF_EEDB_MEM_TYPES;
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, DBAL_PHYSICAL_TABLE_FEC_2, &entry_size));
        }
        else if (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_3)
        {
            mdb_table_pairs[0] = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2;
            dbal_table_pairs[0] = DBAL_PHYSICAL_TABLE_FEC_2;
            macro_types[0] = MDB_MACRO_A;
            macro_idxs[0] = 5;

            num_of_pair_tables = 1;
            mem_type = MDB_NOF_EEDB_MEM_TYPES;
            SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, DBAL_PHYSICAL_TABLE_FEC_3, &entry_size));
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
                    SHR_IF_ERR_EXIT(mdb_set_cluster_alloc
                                    (unit, dbal_table_pairs[table_iterator], &cluster_info, mem_type, entry_size,
                                     current_output_cluster_idx, clusters));

                    current_output_cluster_idx++;
                }
            }
        }
    }

    *nof_valid_clusters = current_output_cluster_idx;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_set_table_resources(
    int unit,
    int nof_valid_clusters,
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS],
    mdb_init_cluster_func_e func)
{
    int cluster_idx;
    dbal_enum_value_field_mdb_physical_table_e mdb_table_id;
    mdb_cluster_info_t cluster_info;
    uint32 row_width;
    int basic_entry_size;
    uint32 global_macro_index;
    uint32 skip_cluster[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] = { 0 };
    int cluster_rows;
    int address_factor;
    int cluster_iter;
    int calc_entry_size;
    int nof_clusters;
    int logical_start_address_calc;

    int nof_inst;
    int nof_inst_a;
    int nof_inst_c;
    uint32 mdb_table_9_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (func >= MDB_NOF_INIT_CLUSTER_OPERATIONS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unrecognized func %d.\n", func);
    }

    for (cluster_iter = 0; cluster_iter < nof_valid_clusters; cluster_iter++)
    {
        if (dnx_data_mdb.pdbs.pdb_info_get(unit, clusters[cluster_iter].dbal_physical_table_id)->db_type ==
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

        if (((clusters[cluster_iter].macro_type == MDB_MACRO_A)
             && (clusters[cluster_iter].macro_idx >= dnx_data_mdb.dh.nof_macroes_type_A_get(unit)))
            || ((clusters[cluster_iter].macro_type == MDB_MACRO_B)
                && (clusters[cluster_iter].macro_idx >= dnx_data_mdb.dh.nof_macroes_type_B_get(unit)))
            || ((clusters[cluster_iter].macro_type == MDB_EEDB_BANK) && (clusters[cluster_iter].macro_idx > 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid combination of macro_type %d and macro_idx %d in cluster %d.\n",
                         clusters[cluster_iter].macro_type, clusters[cluster_iter].macro_idx, cluster_iter);
        }

        if (((clusters[cluster_iter].macro_type == MDB_MACRO_A)
             && (clusters[cluster_iter].cluster_idx >= dnx_data_mdb.dh.nof_macro_clusters_get(unit)))
            || ((clusters[cluster_iter].macro_type == MDB_MACRO_B)
                && (clusters[cluster_iter].cluster_idx >= dnx_data_mdb.dh.nof_macro_clusters_get(unit)))
            || ((clusters[cluster_iter].macro_type == MDB_EEDB_BANK)
                && (clusters[cluster_iter].cluster_idx >= dnx_data_mdb.eedb.nof_eedb_banks_get(unit))))
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

        int row_iter;
        if ((clusters[cluster_iter].dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
            && (clusters[cluster_iter].dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
        {
            SHR_IF_ERR_EXIT(mdb_eedb_translate
                            (unit, clusters[cluster_iter].mdb_eedb_type,
                             clusters[cluster_iter].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1,
                             clusters[cluster_iter].macro_type, &mdb_table_id));
        }
        else
        {
            mdb_table_id =
                dnx_data_mdb.pdbs.pdb_info_get(unit,
                                               clusters[cluster_iter].dbal_physical_table_id)->logical_to_physical;
        }

        row_width = dnx_data_mdb.dh.dh_info_get(unit, mdb_table_id)->row_width;

        global_macro_index = (clusters[cluster_iter].macro_type ==
                              MDB_MACRO_A) ? clusters[cluster_iter].macro_idx : clusters[cluster_iter].macro_idx +
            dnx_data_mdb.dh.nof_macroes_type_A_get(unit);

        if (clusters[cluster_iter].macro_type == MDB_MACRO_A || clusters[cluster_iter].macro_type == MDB_MACRO_B)
        {
            if (((skip_cluster[mdb_table_id] >> clusters[cluster_iter].macro_idx) & 0x1) == 0)
            {
                skip_cluster[mdb_table_id] |= (1 << clusters[cluster_iter].macro_idx);

                for (cluster_idx = cluster_iter; cluster_idx < nof_valid_clusters; cluster_idx++)
                {
                    if (clusters[cluster_iter].macro_idx == clusters[cluster_idx].macro_idx
                        && clusters[cluster_iter].macro_type == clusters[cluster_idx].macro_type)
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

                SHR_IF_ERR_EXIT(mdb_remove_bpu_setup_setting(unit, macro_cluster_allocation, global_macro_index));

            }
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table_id, &nof_clusters));
        if (func == MDB_INIT_CLUSTER_DELETE)
        {
            if ((clusters[cluster_iter].macro_type == MDB_EEDB_BANK) &&
                (clusters[cluster_iter].dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
                && (clusters[cluster_iter].dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
            {
                SHR_IF_ERR_EXIT(mdb_eedb_deinit_bank(unit, clusters[cluster_iter].cluster_idx));
            }

            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
            {
                int shift_index;
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table_id, cluster_idx, &cluster_info));

                if ((clusters[cluster_iter].macro_type == cluster_info.macro_type)
                    && (clusters[cluster_iter].macro_idx == cluster_info.macro_index)
                    && (clusters[cluster_iter].cluster_idx == cluster_info.cluster_index))
                {
                    SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                                    (unit, cluster_info.macro_type, cluster_info.macro_index,
                                     cluster_info.cluster_index, DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES));

                    shift_index = cluster_idx;
                    for (shift_index = cluster_idx; shift_index < nof_clusters - 1; shift_index++)
                    {
                        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                        clusters_info.get(unit, mdb_table_id, shift_index + 1, &cluster_info));
                        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                        clusters_info.set(unit, mdb_table_id, shift_index, &cluster_info));
                    }

                    sal_memset(&cluster_info, 0x0, sizeof(cluster_info));
                    cluster_info.macro_type = MDB_NOF_MACRO_TYPES;
                    {
                        int row_iter;
                        for (row_iter = 0;
                             row_iter <
                             DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER /
                             DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK; row_iter++)
                        {
                            cluster_info.phase_alloc_rows[row_iter] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

                        }
                    }
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                    clusters_info.set(unit, mdb_table_id, nof_clusters - 1, &cluster_info));

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_table_id, --nof_clusters));

                    break;
                }
            }
        }

        if (func == MDB_INIT_CLUSTER_ADD)
        {
            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table_id, cluster_idx, &cluster_info));

                if ((clusters[cluster_iter].macro_type == cluster_info.macro_type)
                    && (clusters[cluster_iter].macro_idx == cluster_info.macro_index)
                    && (clusters[cluster_iter].cluster_idx == cluster_info.cluster_index))
                {
                    break;
                }
            }

            if (cluster_idx == nof_clusters)
            {
                sal_memset(&cluster_info, 0x0, sizeof(mdb_cluster_info_t));

                cluster_info.cluster_index = clusters[cluster_iter].cluster_idx;
                cluster_info.macro_index = clusters[cluster_iter].macro_idx;
                cluster_info.macro_type = clusters[cluster_iter].macro_type;
            }

            row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, clusters[cluster_iter].dbal_physical_table_id)->row_width;
            if ((clusters[cluster_iter].macro_type == MDB_EEDB_BANK)
                && (clusters[cluster_iter].mdb_eedb_type == MDB_EEDB_MEM_TYPE_LL))
            {
                basic_entry_size = MDB_DIRECT_BASIC_ENTRY_SIZE;
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size_using_logical_mdb_table
                                (unit, clusters[cluster_iter].dbal_physical_table_id, &basic_entry_size));
            }
            address_factor = row_width / basic_entry_size;
            calc_entry_size = basic_entry_size;

            SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, clusters[cluster_iter].macro_type, &cluster_rows));

            cluster_info.start_address = clusters[cluster_iter].logical_start_address * calc_entry_size / row_width;

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

            for (row_iter = 0; row_iter < (MDB_NOF_ROWS_IN_MACRO_A_CLUSTER / MDB_NOF_ROWS_IN_EEDB_BANK); row_iter++)
            {
                if (clusters[cluster_iter].phase_alloc_logical_entries[row_iter * address_factor /
                                                                       (row_width / MDB_DIRECT_BASIC_ENTRY_SIZE)] !=
                    DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
                {
                    cluster_info.phase_alloc_rows[row_iter] =
                        clusters[cluster_iter].phase_alloc_logical_entries[row_iter * address_factor /
                                                                           (row_width / MDB_DIRECT_BASIC_ENTRY_SIZE)];
                }
            }

            if (cluster_idx == nof_clusters)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_table_id, ++nof_clusters));

                SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                                (unit, cluster_info.macro_type, cluster_info.macro_index,
                                 cluster_info.cluster_index, mdb_table_id));

            }

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.set(unit, mdb_table_id, cluster_idx, &cluster_info));
        }

    }

    for (cluster_iter = 0; cluster_iter < nof_valid_clusters; cluster_iter++)
    {
        int cluster_iter_inner;
        int skip_table = 0;
        int capacity;

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

        if ((clusters[cluster_iter].dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
            && (clusters[cluster_iter].dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
        {
            soc_reg_above_64_val_t type_bank_select;
            soc_reg_above_64_val_t phase_bank_select;
            soc_reg_above_64_val_t bank_id;
            uint32 bank_id_32, data_32;
            int inst_iter;

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

            SHR_IF_ERR_EXIT(mdb_eedb_init_phase
                            (unit, clusters[cluster_iter].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1,
                             type_bank_select, phase_bank_select, bank_id));

            for (inst_iter = 0; inst_iter < nof_inst_a; inst_iter++)
            {
                data_32 = 0;
                SHR_BITCOPY_RANGE(&data_32, 0, phase_bank_select, inst_iter * MDB_NOF_PHASES_PER_EEDB_BANK_BITS,
                                  MDB_NOF_PHASES_PER_EEDB_BANK_BITS);

                dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_1, inst_iter, data_32);

            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

            for (inst_iter = 0; inst_iter < nof_inst_c; inst_iter++)
            {
                data_32 = 0;
                SHR_BITCOPY_RANGE(&data_32, 0, type_bank_select,
                                  inst_iter * MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS,
                                  MDB_NOF_TYPE_BANK_SELECT_PER_EEDB_BANK_BITS);

                dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_2, inst_iter, data_32);

            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

            for (int bank_id_iter = 0; bank_id_iter < nof_inst; bank_id_iter++)
            {
                bank_id_32 = 0;
                SHR_BITCOPY_RANGE(&bank_id_32, 0, bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                                  dnx_data_mdb.eedb.bank_id_bits_get(unit));

                dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, bank_id_iter,
                                             bank_id_32);

            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(mdb_eedb_init_ll_db_atr
                            (unit, clusters[cluster_iter].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1));
        }

        if ((clusters[cluster_iter].macro_type != MDB_EEDB_BANK)
            || (clusters[cluster_iter].mdb_eedb_type != MDB_EEDB_MEM_TYPE_LL))
        {
            dbal_physical_tables_e dbal_physical_table_id;

            if ((clusters[cluster_iter].dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
                && (clusters[cluster_iter].dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
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
                            (unit, dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical));

            SHR_IF_ERR_EXIT(mdb_dh_init_table
                            (unit, dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical));
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
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS])
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
    mdb_cluster_alloc_info_t clusters[MDB_MAX_NOF_CLUSTERS])
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
    dbal_enum_value_field_mdb_physical_table_e db_index;
    uint32 fec_iter;
    dbal_physical_tables_e fecs_tables[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
        { DBAL_PHYSICAL_TABLE_FEC_1, DBAL_PHYSICAL_TABLE_FEC_2, DBAL_PHYSICAL_TABLE_FEC_3 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (db_index = 0; db_index < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES; db_index++)
    {
        
        if ((db_index != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL) &&
            (db_index != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL) &&
            (db_index != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL) &&
            (db_index != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL))
            SHR_IF_ERR_EXIT(mdb_init_direct_table(unit, db_index));
    }
    {
        uint32 banks_required, max_banks_supported_26k_granularity;
        int fec_total_capacity = 0;
        int fec_capacity = 0;
        for (fec_iter = 0; fec_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_get_capacity(unit, fecs_tables[fec_iter], &fec_capacity));
            fec_total_capacity += fec_capacity;
        }

        banks_required = fec_total_capacity / dnx_data_l3.fec.fec_stage_map_size_get(unit);
        max_banks_supported_26k_granularity =
            dnx_data_l3.fec.max_fec_id_26k_granularity_get(unit) / dnx_data_l3.fec.fec_stage_map_size_get(unit);

        if (banks_required <= max_banks_supported_26k_granularity)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.
                            max_fec_id_value.set(unit, dnx_data_l3.fec.max_fec_id_26k_granularity_get(unit)));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.
                            max_fec_id_value.set(unit, dnx_data_l3.fec.max_fec_id_52k_granularity_get(unit)));
        }
    }

    if (!dnx_data_l3.hw_bug.feature_get(unit, dnx_data_l3_hw_bug_fec_hit_bit))
    {
        uint32 entry_handle;
        uint32 value = TRUE;
        for (fec_iter = 0; fec_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_iter++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.direct.mdb_42_info_get(unit, fecs_tables[fec_iter])->dbal_table,
                             &entry_handle));

            dbal_entry_value_field_arr32_set(unit, entry_handle, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, &value);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_vmv_init(
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
                     dnx_data_mdb.em.mdb_em_tables_info_get(unit, dbal_physical_table_id)->mdb_14_table,
                     &entry_handle_mdb_14_table));

    dbal_value_field_arr32_request(unit, entry_handle_mdb_14_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_14_table, DBAL_COMMIT));

    if (MDB_INIT_EM_IS_EMP(dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit,
                         dnx_data_mdb.em.mdb_18_info_get(unit, dbal_physical_table_id)->dbal_table,
                         &entry_handle_mdb_18_table));

        dbal_value_field_arr32_request(unit, entry_handle_mdb_18_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, data_size);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_mdb_18_table, DBAL_COMMIT));
        data_size_ptr = data_size;

    }

    SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs
                    (unit, MDB_EM_ENCODING_TABLE_ENTRY, dbal_physical_table_id, data, data_size_ptr));

    dbal_entry_value_field_arr32_set(unit, entry_handle_mdb_14_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_14_table, DBAL_COMMIT));

    DBAL_HANDLE_FREE(unit, entry_handle_mdb_14_table);

    if (MDB_INIT_EM_IS_EMP(dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical))
    {
        dbal_entry_value_field_arr32_set(unit, entry_handle_mdb_18_table, DBAL_FIELD_MDB_ITEM_0,
                                         INST_SINGLE, data_size);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_18_table, DBAL_COMMIT));

        DBAL_HANDLE_FREE(unit, entry_handle_mdb_18_table);
    }

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

    SHR_IF_ERR_EXIT(mdb_em_vmv_init(unit, dbal_physical_table_id));

    SHR_IF_ERR_EXIT(mdb_dh_init_table_em_encoding(unit, dbal_physical_table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_dbal_to_mdb_init(
    int unit)
{
    int db_index;
    const dbal_logical_table_t *dbal_logical_table;
    uint32 entry_handle_mdb_15_table;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (db_index = 0; db_index < DBAL_NOF_TABLES; db_index++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, db_index, &dbal_logical_table));

        if (dbal_logical_table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        if (dbal_logical_table->mdb_image_type == DBAL_MDB_IMG_STD_1_NOT_ACTIVE)
        {
            continue;
        }

        if (dbal_logical_table->access_method == DBAL_ACCESS_METHOD_MDB
            && dnx_data_mdb.pdbs.pdb_info_get(unit,
                                              dbal_logical_table->physical_db_id[0])->db_type ==
            DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            SHR_IF_ERR_EXIT(mdb_em_init_logical_table(unit, db_index, dbal_logical_table));
        }
    }

    for (db_index = DBAL_PHYSICAL_TABLE_NONE; db_index < DBAL_NOF_PHYSICAL_TABLES; db_index++)
    {

        if (dnx_data_mdb.pdbs.pdb_info_get(unit, db_index)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {

            SHR_IF_ERR_EXIT(mdb_em_vmv_init(unit, db_index));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_em_tables_info_get(unit, db_index)->mdb_15_table,
                             &entry_handle_mdb_15_table));

            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                         MDB_EM_HIT_BIT_MODE_A);

            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, FALSE);
            if (dnx_data_mdb.em.defragmentation_priority_supported_get(unit))
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, 0);
            }
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, TRUE);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_6, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_mdb_15_table, DBAL_FIELD_MDB_ITEM_7, INST_SINGLE,
                                         MDB_INIT_CUCKOO_WATCHDOG_STEPS_DEF_VALS);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_15_table, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_15_table);

        }
    }

    /*
     * SDK-SDK-142493 add support to profile per ratio.
     */
    if (!dnx_data_mdb.em.age_support_per_entry_size_ratio_get(unit))
    {
        SHR_IF_ERR_EXIT(mdb_em_age_emp_init(unit));
    }

    SHR_IF_ERR_EXIT(mdb_step_table_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

void
mdb_shift_vmv_reg_fetch(
    int stage,
    dbal_tables_e * table_name,
    dbal_fields_e * field_name,
    dbal_fields_e * key_name)
{

    switch (stage)
    {
        case MDB_EM_SHIFT_VMV_FLP:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
            *field_name = DBAL_FIELD_FLPLE_VMV_SHIFT_VALUE;
            *key_name = DBAL_FIELD_STAGE_INDEX;
            break;
        case MDB_EM_SHIFT_VMV_VTT_1:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
            *field_name = DBAL_FIELD_VTT_ST_ONE_LE_VMV_SHIFT_VALUE_MAP;
            *key_name = DBAL_FIELD_STAGE_INDEX;
            break;
        case MDB_EM_SHIFT_VMV_VTT_2_3:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
            *field_name = DBAL_FIELD_VTT_ST_TOW_LE_VMV_SHIFT_VALUE_MAP;
            *key_name = DBAL_FIELD_STAGE_INDEX;
            break;
        case MDB_EM_SHIFT_VMV_VTT_4_5:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT_WITH_STAGES;
            *field_name = DBAL_FIELD_VTT_ST_TH_LE_VMV_SHIFT_VALUE_MAP;
            *key_name = DBAL_FIELD_STAGE_INDEX;
            break;
        case MDB_EM_SHIFT_VMV_IOAM:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT;
            *field_name = DBAL_FIELD_IPPB_OAM_VMV_SHIFT_VALUE;
            *key_name = DBAL_FIELD_EMPTY;
            break;
        case MDB_EM_SHIFT_VMV_EOAM:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT;
            *field_name = DBAL_FIELD_ETPPC_OAM_VMV_SHIFT_VALUE;
            *key_name = DBAL_FIELD_EMPTY;
            break;
        case MDB_EM_SHIFT_VMV_RMEP:
            *table_name = DBAL_TABLE_EMPTY;
            *field_name = DBAL_FIELD_EMPTY;
            *key_name = DBAL_FIELD_EMPTY;
            break;
        case MDB_EM_SHIFT_VMV_ESEM_1:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT;
            *field_name = DBAL_FIELD_ESEM_1_SHIFT_VAL;
            *key_name = DBAL_FIELD_EMPTY;
            break;
        case MDB_EM_SHIFT_VMV_ESEM_2:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT;
            *field_name = DBAL_FIELD_ESEM_2_SHIFT_VAL;
            *key_name = DBAL_FIELD_EMPTY;
            break;
        case MDB_EM_SHIFT_VMV_ESEM_3:
            *table_name = DBAL_TABLE_MDB_VMV_SHIFT;
            *field_name = DBAL_FIELD_ESEM_3_SHIFT_VAL;
            *key_name = DBAL_FIELD_EMPTY;
            break;
        default:
            *table_name = DBAL_TABLE_EMPTY;
            *field_name = DBAL_FIELD_EMPTY;
            *key_name = DBAL_FIELD_EMPTY;
    }
}

shr_error_e
mdb_init_em_entry_encoding(
    int unit)
{
    int stage, stage_index;
    soc_reg_above_64_val_t field_data;
    soc_reg_above_64_val_t data_above_64;
    uint8 vmv_size;
    uint8 vmv_value;

    dbal_tables_e table_name;
    dbal_fields_e field_name;
    dbal_fields_e key_name;
    int field_size;
    int nof_inst;
    int instance_idx;

    uint32 vmv_table_handle_id;
    uint32 data_32;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_parse_xml_em_vmv(unit));

    SHR_IF_ERR_EXIT(mdb_em_init_shift_vmv_conf(unit));

    /** This part configures the hw in bcm layer */
    for (stage = 0; stage < MDB_EM_SHIFT_VMV_STAGES; stage++)
    {

        mdb_shift_vmv_reg_fetch(stage, &table_name, &field_name, &key_name);

        if (table_name != DBAL_TABLE_EMPTY)
        {
            if (stage == 0)
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &vmv_table_handle_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_name, vmv_table_handle_id));
            }

            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_name, field_name, 0, 0, 0, &field_size));
            SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, table_name, field_name, 0, 0, &nof_inst));

            for (stage_index = 0; stage_index < dnx_data_mdb.em.shift_vmv_max_size_get(unit); stage_index++)
            {
                dbal_physical_tables_e dbal_table_id;

                SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.shift_vmv_mapping.get(unit, stage, stage_index, &dbal_table_id));

                if (dbal_table_id != DBAL_NOF_PHYSICAL_TABLES)
                {

                    sal_memset(data_above_64, 0, sizeof data_above_64);

                    if (key_name != DBAL_FIELD_EMPTY)
                    {
                        dbal_entry_key_field32_set(unit, vmv_table_handle_id, key_name, stage_index);
                    }

                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, vmv_table_handle_id, DBAL_GET_ALL_FIELDS));

                    for (instance_idx = 0; instance_idx < nof_inst; instance_idx++)
                    {

                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, vmv_table_handle_id, field_name, instance_idx, &data_32));

                        SHR_BITCOPY_RANGE(data_above_64, instance_idx * field_size, &data_32, 0, field_size);
                    }

                    SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs
                                    (unit, MDB_EM_ENCODING_TABLE_ENTRY, dbal_table_id, NULL, data_above_64));

                    for (instance_idx = 0; instance_idx < nof_inst; instance_idx++)
                    {
                        data_32 = 0;

                        SHR_BITCOPY_RANGE(&data_32, 0, data_above_64, instance_idx * field_size, field_size);

                        dbal_entry_value_field32_set(unit, vmv_table_handle_id, field_name, instance_idx, data_32);
                    }

                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, vmv_table_handle_id, DBAL_COMMIT));
                }
            }
        }

    }

    SOC_REG_ABOVE_64_CLEAR(field_data);

    SHR_IF_ERR_EXIT(mdb_get_vmv_size_value
                    (unit, DBAL_PHYSICAL_TABLE_SEXEM_1, MDB_EM_ENTRY_ENCODING_QUARTER, &vmv_size, &vmv_value));

    field_data[0] = vmv_size;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_27, vmv_table_handle_id));
    dbal_entry_value_field32_set(unit, vmv_table_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, field_data[0]);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, vmv_table_handle_id, DBAL_COMMIT));

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
mdb_set_pre_init_values(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_init_em_entry_encoding(unit));

    SHR_IF_ERR_EXIT(mdb_init_access_global_configuration(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dynamic_memory_access_enable(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_33, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_35, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_36, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_mdb.dh.dhc_nof_blocks_get(unit) > 0)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_37, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, TRUE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_38, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_39, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_init_ll_db_atr(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index)
{

    int phase_pair_index = phase_index / dnx_data_mdb.eedb.nof_phase_per_mag_get(unit);

    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_db[MDB_NOF_EEDB_TABLES] = {
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL
    };

    uint32 mdb_table_11_handle_id;

    dbal_tables_e mdb_table_11 =
        dnx_data_mdb.pdbs.mdb_11_info_get(unit, mdb_physical_table_db[phase_pair_index])->dbal_table;

    uint64 field_data64;

    uint8 multiplier = 0;
    uint8 divider = 0;
    uint8 address_space_field_val = 0;

    int eedb_pointer_bits = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    address_space_field_val =
        dnx_data_mdb.direct.physical_address_max_bits_get(unit) - eedb_pointer_bits +
        utilex_log2_round_up(MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE);
    multiplier = MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(MDB_DIRECT_BASIC_ENTRY_SIZE);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, mdb_table_11, &mdb_table_11_handle_id));

    COMPILER_64_SET(field_data64, 0, multiplier);
    dbal_entry_value_field64_set(unit, mdb_table_11_handle_id, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, field_data64);
    dbal_entry_value_field64_set(unit, mdb_table_11_handle_id, DBAL_FIELD_MDB_ITEM_4, INST_SINGLE, field_data64);

    COMPILER_64_SET(field_data64, 0, divider);
    dbal_entry_value_field64_set(unit, mdb_table_11_handle_id, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, field_data64);

    COMPILER_64_SET(field_data64, 0, address_space_field_val);
    dbal_entry_value_field64_set(unit, mdb_table_11_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, field_data64);

    {
        int max_macro_mapping_index = 64;
        int macro_mapping_index;
        uint32 mapping_compare_bits;
        int cluster_idx;
        int nof_clusters, start_address, end_address;
        dbal_enum_value_field_mdb_physical_table_e ll_table;
        int ll_table_iter;
        int address_space_bits = dnx_data_mdb.direct.physical_address_max_bits_get(unit) - address_space_field_val;

        dbal_entry_value_field32_set(unit, mdb_table_11_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_ALL, 1);

        for (ll_table_iter = 0; ll_table_iter < dnx_data_mdb.eedb.nof_phase_per_mag_get(unit); ll_table_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_translate
                            (unit, MDB_EEDB_MEM_TYPE_LL,
                             phase_pair_index * dnx_data_mdb.eedb.nof_phase_per_mag_get(unit) + ll_table_iter,
                             MDB_EEDB_BANK, &ll_table));

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, ll_table, &nof_clusters));

            mapping_compare_bits = utilex_log2_round_up(max_macro_mapping_index);

            for (macro_mapping_index = 0; macro_mapping_index < max_macro_mapping_index; macro_mapping_index++)
            {
                for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
                {
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    start_address.get(unit, ll_table, cluster_idx, &start_address));
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    end_address.get(unit, ll_table, cluster_idx, &end_address));
                    if (((start_address >> (address_space_bits - mapping_compare_bits)) <= macro_mapping_index)
                        && ((end_address >> (address_space_bits - mapping_compare_bits)) > macro_mapping_index))
                    {
                        break;
                    }
                }

                if (cluster_idx != nof_clusters)
                {

                    dbal_entry_value_field32_set(unit, mdb_table_11_handle_id, DBAL_FIELD_MDB_ITEM_0,
                                                 macro_mapping_index, 0);

                }
            }
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_11_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

shr_error_e
mdb_eedb_set_entry_fmt(
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
        SHR_IF_ERR_EXIT(dbal_mdb_eedb_payload_data_size_etps_format_get(unit, fmt_iter, &data_payload_size_bits));

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
mdb_eedb_set_phase_bank_select(
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
mdb_eedb_deinit_bank(
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

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

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

        SHR_BITCOPY_RANGE(bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit), &bank_id_32, 0,
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));
    }

    SHR_IF_ERR_EXIT(mdb_eedb_set_phase_bank_select
                    (unit, bank_idx, DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES, phase_bank_select));

    SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit(type_bank_select, bank_idx));

    sal_memset(bank_id_used, 0x0, sizeof(bank_id_used));
    for (bank_id_iter = 0; bank_id_iter < dnx_data_mdb.eedb.nof_eedb_banks_get(unit); bank_id_iter++)
    {
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;

        if (bank_id_iter == bank_idx)
        {
            continue;
        }

        SHR_BITCOPY_RANGE(&bank_id_value, 0, bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));

        SHR_IF_ERR_EXIT(mdb_init_get_cluster_assoc(unit, MDB_EEDB_BANK, 0, bank_id_iter, &mdb_physical_table_id));

        if (mdb_physical_table_id != DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            bank_id_used[bank_id_value] = 1;
        }
    }

    for (bank_id_iter = 0; bank_id_iter < (1 << dnx_data_mdb.eedb.bank_id_bits_get(unit)); bank_id_iter++)
    {
        int bank_id_reverse = (1 << dnx_data_mdb.eedb.bank_id_bits_get(unit)) - bank_id_iter - 1;
        if (bank_id_used[bank_id_reverse] == 0)
        {
            bank_id_value = bank_id_reverse;
            break;
        }
    }

    SHR_BITCOPY_RANGE(bank_id, bank_idx * dnx_data_mdb.eedb.bank_id_bits_get(unit), &bank_id_value, 0,
                      dnx_data_mdb.eedb.bank_id_bits_get(unit));

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
        SHR_BITCOPY_RANGE(&bank_id_32, 0, bank_id, bank_id_iter * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));

        dbal_entry_value_field32_set(unit, mdb_table_9_handle_id, DBAL_FIELD_MDB_ITEM_3, bank_id_iter, bank_id_32);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_9_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_init_phase(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase_index,
    soc_reg_above_64_val_t type_bank_select,
    soc_reg_above_64_val_t phase_bank_select,
    soc_reg_above_64_val_t bank_id)
{
    dbal_enum_value_field_mdb_physical_table_e ll_table, data_table, data_macro_table;
    int cluster_idx;
    int nof_clusters;
    mdb_cluster_info_t cluster_info;
    uint8 bank_id_used[(1 << DNX_DATA_MAX_MDB_EEDB_BANK_ID_BITS)];
    uint8 bank_used[DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(bank_id_used, 0x0, sizeof(bank_id_used));
    sal_memset(bank_used, 0x0, sizeof(bank_used));

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, MDB_EEDB_BANK, &ll_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, MDB_EEDB_BANK, &data_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, ~MDB_EEDB_BANK, &data_macro_table));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, ll_table, &nof_clusters));
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        uint32 physical_start_address;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, ll_table, cluster_idx, &cluster_info));
        physical_start_address = cluster_info.start_address;

        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit(type_bank_select, cluster_info.cluster_index));

        SHR_IF_ERR_EXIT(mdb_eedb_set_phase_bank_select
                        (unit, cluster_info.cluster_index, phase_index, phase_bank_select));

        SHR_BITCOPY_RANGE(bank_id, cluster_info.cluster_index * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          &physical_start_address,
                          (dnx_data_lif.out_lif.outlif_pointer_size_get(unit) -
                           dnx_data_mdb.eedb.bank_id_bits_get(unit) - 2), dnx_data_mdb.eedb.bank_id_bits_get(unit));

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

        for (bank_id_iter = 0; bank_id_iter < dnx_data_mdb.eedb.nof_eedb_banks_get(unit); bank_id_iter++)
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
                    mdb_eedb_deinit_bank(unit, bank_id_iter);
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_table, &nof_clusters));
    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        uint32 physical_start_address;
        uint32 cluster_payload_size;
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, data_table, cluster_idx, &cluster_info));
        SHR_IF_ERR_EXIT(mdb_eedb_get_payload_size_from_cluster(unit, &cluster_info, &cluster_payload_size));
        physical_start_address = cluster_info.start_address;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(type_bank_select, cluster_info.cluster_index));

        SHR_IF_ERR_EXIT(mdb_eedb_set_phase_bank_select
                        (unit, cluster_info.cluster_index, phase_index, phase_bank_select));

        SHR_BITCOPY_RANGE(bank_id, cluster_info.cluster_index * dnx_data_mdb.eedb.bank_id_bits_get(unit),
                          &physical_start_address, (utilex_log2_round_up(MDB_NOF_ROWS_IN_EEDB_BANK)),
                          dnx_data_mdb.eedb.bank_id_bits_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_init(
    int unit)
{
    soc_reg_above_64_val_t reg_above64_val;
    soc_reg_above_64_val_t type_bank_select;
    soc_reg_above_64_val_t phase_bank_select;
    soc_reg_above_64_val_t bank_id;

    uint32 mdb_table_9_handle_id, mdb_table_26_ll_1_handle_id, mdb_table_26_ll_2_handle_id;
    uint32 mdb_table_26_ll_3_handle_id, mdb_table_26_ll_4_handle_id;

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

    SHR_IF_ERR_EXIT(mdb_eedb_set_entry_fmt(unit));

    /*
     * SHR_IF_ERR_EXIT(WRITE_MDB_FAP_GLOBAL_PP_10r(unit, 0));
     */

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

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_26_LL_1, &mdb_table_26_ll_1_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_1_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_1_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_26_ll_1_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_26_LL_2, &mdb_table_26_ll_2_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_2_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_2_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_26_ll_2_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_26_LL_3, &mdb_table_26_ll_3_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_3_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_3_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_26_ll_3_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_26_LL_4, &mdb_table_26_ll_4_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_4_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, mdb_table_26_ll_4_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_26_ll_4_handle_id, DBAL_COMMIT));

    for (phase_index = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1; phase_index < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
         phase_index++)
    {

        SHR_IF_ERR_EXIT(mdb_eedb_init_phase(unit, phase_index, type_bank_select, phase_bank_select, bank_id));

        if (phase_index % dnx_data_mdb.eedb.nof_phase_per_mag_get(unit) == 0)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_init_ll_db_atr(unit, phase_index));
        }
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

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    SHR_IF_ERR_EXIT(mdb_sbusdma_desc_init(unit));
#endif

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(MDB_EM_SW_ACCESS.init(unit));

        SHR_IF_ERR_EXIT(mdb_dynamic_memory_access_enable(unit));

        SHR_IF_ERR_EXIT(mdb_set_pre_init_values(unit));

        SHR_IF_ERR_EXIT(mdb_init_direct(unit));

        SHR_IF_ERR_EXIT(mdb_dh_init(unit));

        SHR_IF_ERR_EXIT(mdb_em_init_shadow_db(unit));

        SHR_IF_ERR_EXIT(mdb_em_dbal_to_mdb_init(unit));

        SHR_IF_ERR_EXIT(mdb_em_init_timeout_overrides(unit));
    }
#if defined(INCLUDE_KBP)
    {
        SHR_IF_ERR_EXIT(mdb_lpm_init(unit));

        if (!SOC_WARM_BOOT(unit))
        {
            SHR_IF_ERR_EXIT(mdb_lpm_xpt_init_inner(unit));
        }
    }
#endif /* defined(INCLUDE_KBP) */

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(mdb_eedb_init(unit));

        SHR_IF_ERR_EXIT(mdb_eedb_hitbit_init(unit));

        SHR_IF_ERR_EXIT(mdb_verify_tables_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_mdb_deinit(
    int unit)
{
#if defined(INCLUDE_KBP)
    int no_sync_flag = 0;
#endif /* defined(INCLUDE_KBP) */

    SHR_FUNC_INIT_VARS(unit);

#if defined(INCLUDE_KBP)
    SHR_IF_ERR_EXIT(mdb_lpm_deinit_app(unit, no_sync_flag));
#endif /* defined(INCLUDE_KBP) */

#if !defined(ADAPTER_SERVER_MODE) && !defined(PLISIM)
    SHR_IF_ERR_EXIT(mdb_sbusdma_desc_deinit(unit));
#endif

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
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id)
{
    dbal_enum_value_field_mdb_physical_table_e dst_mdb_table_id;

    SHR_FUNC_INIT_VARS(unit);

    if (macro_type == MDB_MACRO_A)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_a_cluster_assoc.get(unit,
                                                                                 (macro_idx *
                                                                                  dnx_data_mdb.
                                                                                  dh.nof_macro_clusters_get(unit)) +
                                                                                 cluster_idx, &dst_mdb_table_id));
    }
    else if (macro_type == MDB_MACRO_B)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_b_cluster_assoc.get(unit,
                                                                                 (macro_idx *
                                                                                  dnx_data_mdb.
                                                                                  dh.nof_macro_clusters_get(unit)) +
                                                                                 cluster_idx, &dst_mdb_table_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_assoc.get(unit, cluster_idx, &dst_mdb_table_id));
    }

    if (((mdb_physical_table_id != DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
         && (dst_mdb_table_id == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES))
        || (mdb_physical_table_id == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES))
    {
        int global_cluster_idx = (macro_idx * dnx_data_mdb.dh.nof_macro_clusters_get(unit)) + cluster_idx;

        if (macro_type == MDB_MACRO_A)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                            macro_a_cluster_assoc.set(unit, global_cluster_idx, mdb_physical_table_id));
        }
        else if (macro_type == MDB_MACRO_B)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                            macro_b_cluster_assoc.set(unit, global_cluster_idx, mdb_physical_table_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                            eedb_banks_assoc.set(unit, cluster_idx, mdb_physical_table_id));
        }
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
mdb_lpm_xpt_init_inner(
    int unit)
{
    /** uint32 db_atr; */
    int big_kaps_enabled;

    uint32 ads_addr_space_bits = 15;

    uint32 mdb_table_11_ads_1_handle_id;
    uint32 mdb_table_11_ads_2_handle_id;
    uint32 mdb_table_28_kaps_1_handle_id;
    uint32 mdb_table_28_kaps_2_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_28_KAPS_1, &mdb_table_28_kaps_1_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_28_kaps_1_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0x0);

#if defined(INCLUDE_KBP)
    SHR_IF_ERR_EXIT(mdb_lpm_big_kaps_enabled(unit, MDB_KAPS_IP_PRIVATE_DB_ID, &big_kaps_enabled));
#else
    big_kaps_enabled = FALSE;
#endif /* defined(INCLUDE_KBP) */
    if (big_kaps_enabled == TRUE)
    {
        dbal_entry_value_field32_set(unit, mdb_table_28_kaps_1_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0x1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, mdb_table_28_kaps_1_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0x0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_28_kaps_1_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_28_KAPS_2, &mdb_table_28_kaps_2_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_28_kaps_2_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, 0x0);

#if defined(INCLUDE_KBP)
    SHR_IF_ERR_EXIT(mdb_lpm_big_kaps_enabled(unit, MDB_KAPS_IP_PUBLIC_DB_ID, &big_kaps_enabled));
#else
    big_kaps_enabled = FALSE;
#endif /* defined(INCLUDE_KBP) */
    if (big_kaps_enabled == TRUE)
    {
        dbal_entry_value_field32_set(unit, mdb_table_28_kaps_2_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0x1);
    }
    else
    {
        dbal_entry_value_field32_set(unit, mdb_table_28_kaps_2_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0x0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_28_kaps_2_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_11_ADS_1, &mdb_table_11_ads_1_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_11_ads_1_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                 dnx_data_mdb.direct.physical_address_max_bits_get(unit) - ads_addr_space_bits);

    {
        int nof_clusters;
        int cluster_idx;
        dbal_enum_value_field_mdb_physical_table_e mdb_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1;

        SHR_FUNC_INIT_VARS(unit);

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table_id, &nof_clusters));

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            mdb_macro_types_e macro_type;
            int start_address;
            int field_idx;
            int cluster_rows;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.macro_type.get(unit, mdb_table_id, cluster_idx, &macro_type));
            mdb_db_infos.db.clusters_info.start_address.get(unit, mdb_table_id, cluster_idx, &start_address);

            SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, macro_type, &cluster_rows));

            for (field_idx = start_address / (4 * 1024);
                 field_idx < start_address / (4 * 1024) + cluster_rows / (4 * 1024); field_idx++)
            {
                dbal_entry_value_field32_set(unit, mdb_table_11_ads_1_handle_id, DBAL_FIELD_MDB_ITEM_0, field_idx,
                                             macro_type);
            }
        }
    }

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_1_handle_id, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, 0x0);

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_1_handle_id, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, 0x4);

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_1_handle_id, DBAL_FIELD_MDB_ITEM_4, INST_SINGLE, 0x4);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_11_ads_1_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_11_ADS_2, &mdb_table_11_ads_2_handle_id));
    dbal_entry_value_field32_set(unit, mdb_table_11_ads_2_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                 dnx_data_mdb.direct.physical_address_max_bits_get(unit) - ads_addr_space_bits);

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_2_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_ALL, 0x0);

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_2_handle_id, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, 0x0);

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_2_handle_id, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, 0x4);

    dbal_entry_value_field32_set(unit, mdb_table_11_ads_2_handle_id, DBAL_FIELD_MDB_ITEM_4, INST_SINGLE, 0x4);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, mdb_table_11_ads_2_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * ! \file mdb_dh_init.c $Id$ Contains all of the MDB data handlers initialization sequences.
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
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#endif

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
#define MDB_DH_INIT_IF_CLUSTER_ID_DEFAULT_VALUE      (7)
#define MDB_DH_INIT_CLUSTER_ID_DEFAULT_VALUE         (7)
#define MDB_DH_INIT_SIZE_DH_ENCODING_32BIT           (4)
#define MDB_DH_INIT_FIRST_CLUSTER_WAS_USED           (0x1)
#define MDB_DH_INIT_SECOND_CLUSTER_WAS_USED          (0x2)
#define MDB_DH_INIT_FIRST_IF_CLUSTER_WAS_UPDATED     (0x4)
#define MDB_DH_INIT_SECOND_IF_CLUSTER_WAS_UPDATED    (0x8)

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define MDB_DH_SUPPORTED_DB_TYPE(db_type) ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
#define MDB_DH_INTI_BPU_PDB_ON_CLUSTER_MASK(unit)          ((0x1<<dnx_data_mdb.dh.nof_macro_clusters_get(unit)) | 0x1)

#define MDB_DH_IS_ADS_TABLE(mdb_physical_table_id) (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 || mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2)
#define MDB_DH_IS_EEDB_DATA_TABLE(mdb_physical_table_id) (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA || \
                                                          mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA)

#define MDB_DH_IS_EEDB_LL_TABLE(mdb_physical_table_id) (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL || \
                                                        mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL)
#define MDB_IS_MAG_SUPPORTS_FLEX(unit,mdb_table) (dnx_data_mdb.em.flex_mag_supported_get(unit) && \
                                                  (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM) && \
                                                   dnx_data_mdb.dh.dh_info_get(unit, mdb_table)->row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))

/*
 * }
 */
/*
 * Enums
 * {
 */
typedef enum
{
    MDB_DH_INIT_FIRST_IF_USED = 0,
    MDB_DH_INIT_SECOND_IF_USED = 1,
    MDB_DH_INIT_MAX_AVAILABLE_INTERFACES = 2
} mdb_dh_init_interface_instance_in_macro_e;

typedef enum
{
    MDB_DH_INIT_SERIALIZER_MODE_120 = 0,
    MDB_DH_INIT_SERIALIZER_MODE_60 = 1,
    MDB_DH_INIT_SERIALIZER_MODE_30 = 2,
    MDB_DH_INIT_SERIALIZER_MODE_20 = 3,
    MDB_DH_INIT_SERIALIZER_MODE_10 = 4
} mdb_dh_init_macro_serializer_modes_e;

typedef enum
{
    MDB_EM_EMP_AGING_BITS_NONE = 0,
    MDB_EM_EMP_AGING_BITS_4 = 4,
    MDB_EM_EMP_AGING_BITS_8 = 8,
    MDB_EM_EMP_AGING_BITS_16 = 16,
    MDB_EM_EMP_AGING_BITS_32 = 32
} mdb_em_emp_nof_aging_bit_options_e;
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

/*
 * }
 */
/*
 * Externs
 * {
 */

/*
 * }
 */
/*
 * functions
 * {
 */

mdb_dh_init_macro_serializer_modes_e
mdb_dh_em_get_serializer_mode(
    uint32 entry_size)
{
    if (entry_size < 20)
    {
        return MDB_DH_INIT_SERIALIZER_MODE_10;
    }
    else if (entry_size < 30)
    {
        return MDB_DH_INIT_SERIALIZER_MODE_20;
    }
    else if (entry_size < 60)
    {
        return MDB_DH_INIT_SERIALIZER_MODE_30;
    }
    else if (entry_size < 120)
    {
        return MDB_DH_INIT_SERIALIZER_MODE_60;
    }
    else
    {
        return MDB_DH_INIT_SERIALIZER_MODE_120;
    }
}
shr_error_e
mdb_dh_em_init(
    int unit,
    mdb_cluster_info_t * cluster_info_p,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table,
    uint32 *encoding)
{
    uint32 entry_handle_id;
    int cluster_itr;
    uint32 global_macro_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    global_macro_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info_p->macro_type, cluster_info_p->macro_index);

    if (cluster_info_p->cluster_position_in_entry == 0)
    {
        int nof_clusters = dnx_data_mdb.dh.dh_info_get(unit,
                                                       mdb_physical_table)->row_width /
            dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_4, &entry_handle_id));

        for (cluster_itr = 0; cluster_itr < nof_clusters; cluster_itr++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, global_macro_index);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_1,
                                       cluster_info_p->cluster_index + cluster_itr);

            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                             &encoding[MDB_DH_INIT_SIZE_DH_ENCODING_32BIT * cluster_itr]);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_4, entry_handle_id);
        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_30, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, global_macro_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_info_p->cluster_index,
                                 mdb_dh_em_get_serializer_mode(dnx_data_mdb.dh.
                                                               dh_info_get(unit, mdb_physical_table)->dh_in_width));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, cluster_info_p->cluster_index,
                                 mdb_dh_em_get_serializer_mode(dnx_data_mdb.dh.
                                                               dh_info_get(unit, mdb_physical_table)->dh_out_width));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_30, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_dh_init_update_if_cluster_offset(
    int unit,
    int second_interface,
    mdb_cluster_info_t * cluster_info_p,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table)
{
    int index;
    int macro_global_index;
    int entry_data_out_size;
    int chunk_offset;
    int chunk_start_index;
    uint32 entry_handle_id;
    uint32 row_width;
    uint32 data_width_pos_in_array;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    macro_global_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info_p->macro_type, cluster_info_p->macro_index);

    row_width = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->row_width;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_data_mdb.dh.mdb_3_info_get(unit, macro_global_index)->dbal_table, &entry_handle_id));

    data_width_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_data_width_get(unit) - 1;

    entry_data_out_size =
        dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->if_cluster_offsets_values[data_width_pos_in_array];

    chunk_start_index =
        dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->if_cluster_offsets_values[macro_global_index] +
        (second_interface * entry_data_out_size);

    if (mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1
        || mdb_physical_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2)
    {
        chunk_offset =
            (cluster_info_p->cluster_index * dnx_data_mdb.dh.cluster_row_width_bits_get(unit)) /
            dnx_data_mdb.dh.data_out_granularity_get(unit);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, chunk_start_index, chunk_offset);
    }
    else
    {

        chunk_offset =
            (cluster_info_p->cluster_index * dnx_data_mdb.dh.cluster_row_width_bits_get(unit) +
             row_width) / dnx_data_mdb.dh.data_out_granularity_get(unit) - entry_data_out_size;

        for (index = chunk_start_index; index < (chunk_start_index + entry_data_out_size); index++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, index, chunk_offset++);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_set_active_dhs(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table,
    uint32 active_dhs_bitmap)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);



    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_data_mdb.pdbs.mdb_26_info_get(unit, mdb_physical_table)->dbal_table, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, active_dhs_bitmap);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, active_dhs_bitmap);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        uint32 ovf_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;
        uint32 ovf_pos =
            dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->table_macro_interface_mapping[ovf_pos_in_array];
        uint32 active_dhs_bitmap_and_ovf;
        if (MDB_IS_MAG_SUPPORTS_FLEX(unit, mdb_physical_table))
        {
            ovf_pos = (ovf_pos << 1);
        }

        active_dhs_bitmap_and_ovf = active_dhs_bitmap | (1 << ovf_pos);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_16_info_get(unit, mdb_physical_table)->dbal_table,
                         &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                     active_dhs_bitmap_and_ovf);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        if (MDB_INIT_EM_IS_EMP(mdb_physical_table))
        {
            uint32 nof_ones = utilex_bitstream_get_nof_on_bits(&active_dhs_bitmap, 1);
            uint32 split_pos = 0;
            uint32 found_count = 0;
            uint32 entry_handle_emp_id;
            if (nof_ones != 0)
            {
                split_pos = ovf_pos;
                while (found_count < ((nof_ones + 1) >> 1))
                {
                    split_pos--;
                    found_count += (active_dhs_bitmap >> split_pos) & 0x1;
                }
                if ((nof_ones & 0x1) == 0 && ((active_dhs_bitmap >> (split_pos - 1)) & 0x1) == 0)
                {
                    split_pos--;
                }
            }
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.em.mdb_23_info_get(unit,
                                                             mdb_physical_table)->dbal_table, &entry_handle_emp_id));

            dbal_entry_value_field32_set(unit, entry_handle_emp_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                         active_dhs_bitmap_and_ovf);

            dbal_entry_value_field32_set(unit, entry_handle_emp_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, split_pos);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_emp_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_em_emp_get_nof_aging_bits(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_table,
    uint32 nof_entries,
    uint32 *nof_aging_bits)
{
    uint32 available_aging_bits;
    SHR_FUNC_INIT_VARS(unit);

    if (nof_entries > 0)
    {
        if (mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM)
        {
            available_aging_bits = dnx_data_mdb.em.total_nof_lem_aging_bits_get(unit) / nof_entries;
        }
        else if (mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3
                 || mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4)
        {
            available_aging_bits = dnx_data_mdb.em.total_nof_exem_aging_bits_get(unit) / nof_entries;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_BADID, "%d isn't a valid EMP table (only LEM,EXEM 3  and EXEM 4 supported).\n",
                         mdb_table);
        }
        if (available_aging_bits >= MDB_EM_EMP_AGING_BITS_32)
        {
            *nof_aging_bits = MDB_EM_EMP_AGING_BITS_32;
        }
        else if (available_aging_bits >= MDB_EM_EMP_AGING_BITS_16)
        {
            *nof_aging_bits = MDB_EM_EMP_AGING_BITS_16;
        }
        else if (available_aging_bits >= MDB_EM_EMP_AGING_BITS_8)
        {
            *nof_aging_bits = MDB_EM_EMP_AGING_BITS_8;
        }
        else if (available_aging_bits >= MDB_EM_EMP_AGING_BITS_4)
        {
            *nof_aging_bits = MDB_EM_EMP_AGING_BITS_4;
        }
        else
        {
            *nof_aging_bits = MDB_EM_EMP_AGING_BITS_NONE;
        }
    }
    else
    {
        *nof_aging_bits = MDB_EM_EMP_AGING_BITS_NONE;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_update_em_bank_sizes(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table,
    dh_em_bank_info * em_bank_info)
{
    uint32 entry_handle_mdb_29_table;
    uint32 entry_handle_mdb_24_table;
    uint32 nof_entries = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        int bank_iter;
        int nof_banks = dnx_data_mdb.em.mdb_29_info_get(unit, mdb_physical_table)->mdb_item_0_array_size;
        int table_used = FALSE;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_29_info_get(unit, mdb_physical_table)->dbal_table,
                         &entry_handle_mdb_29_table));

        if (MDB_INIT_EM_IS_EMP(mdb_physical_table))
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_24_info_get(unit, mdb_physical_table)->dbal_table,
                             &entry_handle_mdb_24_table));
        }

        for (bank_iter = 0; bank_iter < nof_banks; bank_iter++)
        {

            if (em_bank_info[bank_iter].bank_size_used_8k > 0)
            {
                uint32 bucket_size_log2 = utilex_log2_round_up(em_bank_info[bank_iter].bank_size_used_8k);

                dbal_entry_value_field32_set(unit, entry_handle_mdb_29_table, DBAL_FIELD_MDB_ITEM_0, bank_iter,
                                             bucket_size_log2);

                table_used = TRUE;

                if (MDB_INIT_EM_IS_EMP(mdb_physical_table))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_1, bank_iter,
                                                 nof_entries / 8);

                    
                    dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_0, bank_iter,
                                                 utilex_log2_round_up(em_bank_info[bank_iter].bank_size_used_8k << 13));

                    nof_entries += (em_bank_info[bank_iter].bank_size_used_8k << 13);
                }

            }
        }

        if (table_used == TRUE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_29_table, DBAL_COMMIT));
        }

        if (MDB_INIT_EM_IS_EMP(mdb_physical_table))
        {
            uint32 nof_aging_bits;
            uint32 ovf_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;
            uint32 ovf_pos =
                dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->table_macro_interface_mapping[ovf_pos_in_array];
            dbal_physical_tables_e dbal_physical_table_id = mdb_direct_mdb_to_dbal[mdb_physical_table];
            uint32 log2_ovf_size =
                utilex_log2_round_up(dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->ovf_cam_size);

            dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_0, ovf_pos,
                                         log2_ovf_size);

            SHR_IF_ERR_EXIT(mdb_em_emp_get_nof_aging_bits(unit, mdb_physical_table, nof_entries, &nof_aging_bits));

            dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE,
                                         nof_aging_bits);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_24_table, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_macro_bpu_setup(
    int unit,
    uint32 global_macro_index,
    uint32 *current_bpu_setup)
{
    uint32 entry_handle_id;
    uint32 bucket_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_2, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, global_macro_index);

    for (bucket_index = 0; bucket_index < dnx_data_mdb.dh.nof_buckets_in_macro_get(unit); bucket_index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, bucket_index,
                                   &current_bpu_setup[bucket_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_2, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_remove_bpu_setup_setting(
    int unit,
    uint32 current_allocation,
    uint32 global_macro_index)
{
    uint32 entry_handle_id;
    uint32 current_bpu_setup[DNX_DATA_MAX_MDB_DH_NOF_BUCKETS_IN_MACRO] = { 0 };
    uint32 bucket_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_get_macro_bpu_setup(unit, global_macro_index, current_bpu_setup));

    if ((current_allocation ^ MDB_BPU_8_CLUSTERS_BIT_MASK) == 0)
    {
        current_bpu_setup[0] = MDB_BPU_SIZE_120x4;
        current_bpu_setup[1] = MDB_BPU_SIZE_120x4;
    }
    else if ((current_allocation ^ MDB_BPU_6_CLUSTERS_BIT_MASK) == 0)
    {
        current_bpu_setup[0] = MDB_BPU_SIZE_120x4;
        if (current_bpu_setup[1] == MDB_BPU_SIZE_240x2)
        {
            current_bpu_setup[1] = MDB_BPU_SIZE_240_120x2;
        }
        else
        {
            current_bpu_setup[1] = MDB_BPU_SIZE_120x4;
        }
    }
    else
    {
        uint32 allocated_bucket = ((MDB_BPU_BUCKET_BIT_MASK & current_allocation) == 0) ? 1 : 0;
        uint32 bucket_allocation =
            current_allocation >> (allocated_bucket * dnx_data_mdb.dh.nof_bucket_clusters_get(unit));

        if ((bucket_allocation ^ MDB_BPU_BUCKET_BIT_MASK) == 0)
        {
            current_bpu_setup[allocated_bucket] = MDB_BPU_SIZE_120x4;
        }
        else if ((bucket_allocation ^ MDB_BPU_240_RIGHT_BKT_SIDE_BIT_MASK) == 0)
        {

            if (current_bpu_setup[allocated_bucket] == MDB_BPU_SIZE_240x2)
            {
                current_bpu_setup[allocated_bucket] = MDB_BPU_SIZE_240_120x2;
            }
            else
            {
                current_bpu_setup[allocated_bucket] = MDB_BPU_SIZE_120x4;
            }
        }
        else if ((bucket_allocation ^ MDB_BPU_240_LEFT_BKT_SIDE_BIT_MASK) == 0)
        {
            if (current_bpu_setup[allocated_bucket] == MDB_BPU_SIZE_240x2)
            {
                current_bpu_setup[allocated_bucket] = MDB_BPU_SIZE_120x2_240;
            }
            else
            {
                current_bpu_setup[allocated_bucket] = MDB_BPU_SIZE_120x4;
            }
        }
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_2, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, global_macro_index);

    for (bucket_index = 0; bucket_index < dnx_data_mdb.dh.nof_buckets_in_macro_get(unit); bucket_index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, bucket_index,
                                     current_bpu_setup[bucket_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_2, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_set_bpu_setting(
    int unit,
    uint32 row_width,
    uint32 *macro_bit_map,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id)
{
    int macro_index;
    int bucket_index;
    int interface_index;
    uint32 entry_handle_id;
    dbal_enum_value_field_mdb_db_type_e db_type;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    db_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_2, &entry_handle_id));

    for (macro_index = 0; macro_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_index++)
    {
        if (macro_bit_map[macro_index] != 0)
        {
            uint32 current_bpu_setup[DNX_DATA_MAX_MDB_DH_NOF_BUCKETS_IN_MACRO] = { 0 };
            int cluster_index;

            SHR_IF_ERR_EXIT(mdb_get_macro_bpu_setup(unit, macro_index, current_bpu_setup));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, macro_index);

            for (interface_index = 0; interface_index < MDB_DH_INIT_MAX_AVAILABLE_INTERFACES; interface_index++)
            {
                uint32 macro_update_offset = dnx_data_mdb.dh.nof_macro_clusters_get(unit) * interface_index;
                uint32 macro_update_bits =
                    (macro_bit_map[macro_index] & (MDB_BPU_8_CLUSTERS_BIT_MASK << macro_update_offset)) >>
                    macro_update_offset;

                if ((macro_update_bits ^ MDB_BPU_8_CLUSTERS_BIT_MASK) == 0)
                {
                    current_bpu_setup[0] = MDB_BPU_SIZE_960_ODD;
                    current_bpu_setup[1] = MDB_BPU_SIZE_960;
                }
                else if ((macro_update_bits ^ MDB_BPU_6_CLUSTERS_BIT_MASK) == 0)
                {
                    current_bpu_setup[0] = MDB_BPU_SIZE_960;
                    if (current_bpu_setup[1] == MDB_BPU_SIZE_240_120x2)
                    {
                        current_bpu_setup[1] = MDB_BPU_SIZE_240x2;
                    }
                    else
                    {
                        current_bpu_setup[1] = MDB_BPU_SIZE_120x2_240;
                    }
                }
                else
                {

                    for (bucket_index = 0; bucket_index < dnx_data_mdb.dh.nof_buckets_in_macro_get(unit);
                         bucket_index++)
                    {
                        uint32 bucket_bits_offset =
                            (bucket_index +
                             (interface_index * MDB_DH_INIT_MAX_AVAILABLE_INTERFACES)) *
                            dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
                        uint32 bucket_update_bits =
                            (macro_bit_map[macro_index] & (MDB_BPU_BUCKET_BIT_MASK << bucket_bits_offset)) >>
                            bucket_bits_offset;

                        if ((bucket_update_bits ^ MDB_BPU_BUCKET_BIT_MASK) == 0)
                        {
                            current_bpu_setup[bucket_index] = MDB_BPU_SIZE_480;
                        }
                        else if ((bucket_update_bits ^ MDB_BPU_240_RIGHT_BKT_SIDE_BIT_MASK) == 0)
                        {
                            if (current_bpu_setup[bucket_index] == MDB_BPU_SIZE_240_120x2)
                            {
                                current_bpu_setup[bucket_index] = MDB_BPU_SIZE_240x2;
                            }
                            else
                            {
                                current_bpu_setup[bucket_index] = MDB_BPU_SIZE_120x2_240;
                            }
                        }
                        else if ((bucket_update_bits ^ MDB_BPU_240_LEFT_BKT_SIDE_BIT_MASK) == 0)
                        {
                            if (current_bpu_setup[bucket_index] == MDB_BPU_SIZE_120x2_240)
                            {
                                current_bpu_setup[bucket_index] = MDB_BPU_SIZE_240x2;
                            }
                            else
                            {
                                current_bpu_setup[bucket_index] = MDB_BPU_SIZE_240_120x2;
                            }
                        }
                    }
                }
            }

            for (bucket_index = 0; bucket_index < dnx_data_mdb.dh.nof_buckets_in_macro_get(unit); bucket_index++)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, bucket_index,
                                             current_bpu_setup[bucket_index]);
            }

            for (cluster_index = 0; cluster_index < dnx_data_mdb.dh.nof_macro_clusters_get(unit); cluster_index++)
            {
                if ((macro_bit_map[macro_index] & ((MDB_DH_INTI_BPU_PDB_ON_CLUSTER_MASK(unit)) << cluster_index)) > 0)
                {
                    uint32 bpu_mode;
                    if ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT || db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB
                         || db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
                        && row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                    {
                        bpu_mode = MDB_BPU_MODE_DIRECT_120;
                    }
                    else if ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT
                              || db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
                             && row_width == 2 * dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                    {
                        bpu_mode = MDB_BPU_MODE_DIRECT_240;
                    }
                    else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
                    {
                        bpu_mode = MDB_BPU_MODE_LPM;
                    }
                    else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM
                             && row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                    {
                        bpu_mode = MDB_BPU_MODE_EM_120;
                    }
                    else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM
                             && row_width == 2 * dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                    {
                        bpu_mode = MDB_BPU_MODE_EM_240;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Error. MDB physical table %d is not associated with a valid BPU mode value.\n",
                                     mdb_physical_table_id);
                    }

                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_index, bpu_mode);
                }
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_2, entry_handle_id);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_lpm_init(
    int unit,
    uint32 row_width,
    int cluster_idx,
    mdb_cluster_info_t * cluster_info_p)
{
    uint32 entry_handle_mdb_table_6;
    uint8 fmt_index;
    uint32 global_macro_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_6, &entry_handle_mdb_table_6));

    for (fmt_index = 0; fmt_index < MDB_NOF_DIRECT_FMT; fmt_index++)
    {
        uint32 fmt_val = 0;

        if (row_width ==
            dnx_data_mdb.dh.cluster_row_width_bits_get(unit) * dnx_data_mdb.dh.nof_bucket_clusters_get(unit))
        {
            if ((fmt_index >= MDB_LPM_MIN_FMT) &&
                (fmt_index <= MDB_LPM_MAX_FMT) &&
                (fmt_index >=
                 (MDB_LPM_NOF_FMT_BITS *
                  (cluster_info_p->cluster_index % dnx_data_mdb.dh.nof_bucket_clusters_get(unit))))
                && (fmt_index <
                    (MDB_LPM_NOF_FMT_BITS *
                     ((cluster_info_p->cluster_index % dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) + 1))))
            {
                fmt_val =
                    (1 <<
                     (fmt_index -
                      (MDB_LPM_NOF_FMT_BITS *
                       (cluster_info_p->cluster_index % dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))));
            }
            else
            {
                fmt_val = 0;
            }

#ifdef ADAPTER_SERVER_MODE
            fmt_val = fmt_index;
#endif
        }
        else if (row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
        {
            fmt_val = 0x8;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected LPM row width %d.\n", row_width);
        }

        global_macro_index =
            MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info_p->macro_type, cluster_info_p->macro_index);

        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_0, global_macro_index);

        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_1, cluster_info_p->cluster_index);

        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_2, fmt_index);

        dbal_entry_value_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, fmt_val);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_table_6, DBAL_COMMIT));

        DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_6, entry_handle_mdb_table_6);

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_direct_init(
    int unit,
    uint32 row_width,
    int cluster_idx,
    mdb_cluster_info_t * cluster_info_p,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id)
{
    uint32 entry_handle_mdb_table_6;
    uint32 entry_handle_mdb_table_5;
    uint32 table_entry_size;
    dbal_enum_value_field_direct_payload_sizes_e payload_type;
    int key_index;
    uint32 cluster_offset = 0;
    uint32 global_macro_index;
    uint8 multiplier = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {

        SHR_IF_ERR_EXIT(mdb_eedb_get_payload_size_from_cluster(unit, cluster_info_p, (uint32 *) &payload_type));
    }
    else
    {
        payload_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->direct_payload_type;
    }

    table_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

    multiplier = MDB_DIRECT_PAYLOAD_SIZE_TO_MULTIPLIER(table_entry_size);

    global_macro_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info_p->macro_type, cluster_info_p->macro_index);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_5, &entry_handle_mdb_table_5));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_6, &entry_handle_mdb_table_6));

    for (key_index = 0; key_index < MDB_NOF_DIRECT_FMT; key_index++)
    {
        uint8 fmt = (key_index * multiplier) % MDB_NOF_DIRECT_FMT;

        int remainder_size;

        uint32 fmt_atr_bitmap = 0;

        remainder_size = MDB_ENTRY_REMAINDER_CALC(key_index, table_entry_size, row_width);
        if ((((key_index * multiplier) % MDB_NOF_DIRECT_FMT) % multiplier == 0)
            && ((key_index * multiplier) >= MDB_NOF_DIRECT_FMT))
        {
            break;
        }

        if (row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
        {
            fmt_atr_bitmap =
                1 <<
                ((dnx_data_mdb.dh.cluster_row_width_bits_get(unit) - MDB_DIRECT_BASIC_ENTRY_SIZE -
                  remainder_size) / MDB_DIRECT_BASIC_ENTRY_SIZE);
        }
        else
        {
            if (cluster_idx % 2 == 0)
            {
                if (remainder_size >= dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                {
                    fmt_atr_bitmap =
                        1 <<
                        ((2 * dnx_data_mdb.dh.cluster_row_width_bits_get(unit) - MDB_DIRECT_BASIC_ENTRY_SIZE -
                          remainder_size) / MDB_DIRECT_BASIC_ENTRY_SIZE);
                }
            }
            else
            {
                if (remainder_size >= dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                {
                    uint32 table_actual_entry_size;

                    if ((row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit) * 2)
                        && (payload_type == DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_60B))
                    {
                        table_actual_entry_size =
                            MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_180B);
                    }
                    else
                    {
                        table_actual_entry_size = table_entry_size;
                    }

                    if (remainder_size + table_actual_entry_size > 2 * dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                    {
                        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&cluster_offset, fmt));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit(&cluster_offset, fmt));
                    }
                }
                else
                {
                    fmt_atr_bitmap =
                        1 <<
                        ((dnx_data_mdb.dh.cluster_row_width_bits_get(unit) - MDB_DIRECT_BASIC_ENTRY_SIZE -
                          remainder_size) / MDB_DIRECT_BASIC_ENTRY_SIZE);
                }
            }
        }

        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_0, global_macro_index);

        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_1, cluster_info_p->cluster_index);

        dbal_entry_key_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_KEY_2, fmt);

        dbal_entry_value_field32_set(unit, entry_handle_mdb_table_6, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                     fmt_atr_bitmap);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_table_6, DBAL_COMMIT));

        DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_6, entry_handle_mdb_table_6);

    }

    dbal_entry_key_field32_set(unit, entry_handle_mdb_table_5, DBAL_FIELD_MDB_KEY_0, global_macro_index);

    dbal_entry_key_field32_set(unit, entry_handle_mdb_table_5, DBAL_FIELD_MDB_KEY_1, cluster_info_p->cluster_index);

    dbal_entry_value_field32_set(unit, entry_handle_mdb_table_5, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, cluster_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_table_5, DBAL_COMMIT));

    DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_5, entry_handle_mdb_table_5);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_dh_init_update_default_values(
    int unit)
{
    int macro_iter, cluster_iter;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (macro_iter = 0; macro_iter < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_iter++)
    {
        int mag_iter, nof_mag;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.dh.mdb_1_info_get(unit, macro_iter)->dbal_table, &entry_handle_id));

        nof_mag = dnx_data_mdb.dh.mdb_1_info_get(unit, macro_iter)->mdb_item_3_array_size;

        for (mag_iter = 0; mag_iter < nof_mag; mag_iter++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3,
                                         mag_iter, MDB_DH_INIT_IF_CLUSTER_ID_DEFAULT_VALUE);
        }


        for (cluster_iter = 0; cluster_iter < dnx_data_mdb.dh.nof_macro_clusters_get(unit); cluster_iter++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_iter,
                                         MDB_DH_INIT_CLUSTER_ID_DEFAULT_VALUE);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, cluster_iter,
                                         MDB_DH_INIT_CLUSTER_ID_DEFAULT_VALUE);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        DBAL_HANDLE_FREE(unit, entry_handle_id);

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_init_table_em_encoding(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id)
{
    soc_reg_above_64_val_t vmv_encoding_data;
    mdb_em_encoding_request_e encoding_request = MDB_EM_ENCODING_DH_ENTRY_120;
    uint32 row_width;
    int nof_clusters;
    int cluster_idx;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    mdb_physical_table = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

    row_width = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->row_width;

    sal_memset(vmv_encoding_data, 0x0, sizeof(vmv_encoding_data[0]) * SOC_REG_ABOVE_64_MAX_SIZE_U32);
    encoding_request =
        (row_width ==
         dnx_data_mdb.
         dh.cluster_row_width_bits_get(unit)) ? MDB_EM_ENCODING_DH_ENTRY_120 : MDB_EM_ENCODING_DH_ENTRY_240;
    SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs(unit, encoding_request, dbal_physical_table_id, vmv_encoding_data, NULL));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters));

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        mdb_cluster_info_t cluster_info;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table, cluster_idx, &cluster_info));

        SHR_IF_ERR_EXIT(mdb_dh_em_init(unit, &cluster_info, mdb_physical_table, vmv_encoding_data));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_init_table(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id)
{
    int nof_clusters;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
    if (!MDB_DH_IS_EEDB_DATA_TABLE(mdb_physical_table_id) && !MDB_DH_IS_EEDB_LL_TABLE(mdb_physical_table_id))
    {
        int cluster_idx;
        uint32 entry_handle_id;
        uint32 row_width;
        dbal_physical_tables_e dbal_physical_table_id;
        dbal_enum_value_field_mdb_db_type_e db_type;

        uint8 macro_usage_state[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES] = { 0 };
        int macro_last_address[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES] = { 0 };

        uint32 table_allocation[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES] = { 0 };

        em_key_format_info key_format_info;
        uint8 macro_cluster_counter = 0;

        uint32 used_macros = 0;

        db_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type;

        if (!MDB_DH_SUPPORTED_DB_TYPE(db_type))
        {
            SHR_EXIT();
        }

        dbal_physical_table_id = mdb_direct_mdb_to_dbal[mdb_physical_table_id];

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.get(unit, dbal_physical_table_id, &key_format_info));

        row_width = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->row_width;

        if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            SHR_IF_ERR_EXIT(mdb_dh_init_table_em_encoding(unit, dbal_physical_table_id));
        }

        if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
        {
            uint32 pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;

            uint32 interface = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->table_macro_interface_mapping
                [pos_in_array];

            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&used_macros, interface));
        }

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            mdb_macro_types_e macro_type;
            int abk_cluster_counter;
            mdb_dh_init_interface_instance_in_macro_e if_instance_used = MDB_DH_INIT_FIRST_IF_USED;
            uint8 iter_cluster_if, cluster_interface_id = dnx_data_mdb.dh.max_nof_cluster_interfaces_get(unit);
            uint32 macro_index;
            uint32 interface_dh_interface;
            mdb_cluster_info_t cluster_info;
            uint32 data_width_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_data_width_get(unit) - 1;
            int nof_cluster_per_entry;
            uint32 start_address_remapped = 0;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, cluster_idx, &cluster_info));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            nof_cluster_per_entry.get(unit, mdb_physical_table_id, &nof_cluster_per_entry));

            macro_type = cluster_info.macro_type;

            if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
            {
                int phase_rows_iter;
                int is_allocated = 0;

                for (phase_rows_iter = 0;
                     phase_rows_iter <
                     DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK;
                     phase_rows_iter++)
                {
                    if (cluster_info.phase_alloc_rows[phase_rows_iter] != DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
                    {
                        is_allocated = 1;
                        break;
                    }
                }

                if (is_allocated == 1)
                {
                    start_address_remapped = cluster_info.start_address;
                }
                else
                {
                    int cluster_idx_inner;
                    int cluster_id_iter;
                    mdb_cluster_info_t cluster_info_inner;
                    uint8 cluster_id_used[DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS];
                    uint8 shift_bits =
                        (macro_type ==
                         MDB_MACRO_A) ? utilex_log2_round_up(MDB_NOF_ROWS_IN_MACRO_A_CLUSTER) :
                        utilex_log2_round_up(MDB_NOF_ROWS_IN_MACRO_B_CLUSTER);

                    sal_memset(cluster_id_used, 0x0, sizeof(cluster_id_used));

                    for (cluster_idx_inner = 0; cluster_idx_inner < nof_clusters; cluster_idx_inner++)
                    {
                        if (cluster_idx_inner == cluster_idx)
                        {
                            continue;
                        }

                        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                        clusters_info.get(unit, mdb_physical_table_id, cluster_idx_inner,
                                                          &cluster_info_inner));
                        if ((cluster_info.macro_type == cluster_info_inner.macro_type)
                            && (cluster_info.macro_index == cluster_info_inner.macro_index))
                        {
                            cluster_id_used[(cluster_info.start_address >> shift_bits) %
                                            dnx_data_mdb.dh.nof_macro_clusters_get(unit)] = 1;
                        }
                    }

                    for (cluster_id_iter = 0; cluster_id_iter < dnx_data_mdb.dh.nof_macro_clusters_get(unit);
                         cluster_id_iter++)
                    {
                        if (cluster_id_used[cluster_id_iter] == 0)
                        {
                            start_address_remapped = cluster_id_iter << shift_bits;
                            break;
                        }
                    }
                }
            }
#ifndef ADAPTER_SERVER_MODE
            else if ((mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1)
                     || (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2)
                     || (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3))
            {
                uint32 cluster_pair_index = (cluster_info.cluster_index / 2);
                uint32 start_address_mask;

                start_address_mask = ~(((1 << dnx_data_mdb.direct.fec_address_mapping_bits_get(unit)) - 1)
                                       << dnx_data_mdb.direct.fec_address_mapping_bits_offset_get(unit));
                start_address_remapped =
                    (cluster_info.start_address & start_address_mask) | (cluster_pair_index << dnx_data_mdb.
                                                                         direct.fec_address_mapping_bits_offset_get
                                                                         (unit));
            }
#endif /* ifndef ADAPTER_SERVER_MODE */
            else
            {
                start_address_remapped = cluster_info.start_address;
            }

            if (macro_type == MDB_MACRO_A)
            {
                macro_cluster_counter =
                    (start_address_remapped / MDB_NOF_ROWS_IN_MACRO_A_CLUSTER) %
                    dnx_data_mdb.dh.nof_macro_clusters_get(unit);
                macro_index = cluster_info.macro_index;
            }
            else
            {
                macro_cluster_counter =
                    (start_address_remapped / MDB_NOF_ROWS_IN_MACRO_B_CLUSTER) %
                    dnx_data_mdb.dh.nof_macro_clusters_get(unit);
                macro_index = cluster_info.macro_index + dnx_data_mdb.dh.nof_macroes_type_A_get(unit);
            }
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.dh.mdb_1_info_get(unit, macro_index)->dbal_table, &entry_handle_id));

            abk_cluster_counter = ((macro_cluster_counter * nof_cluster_per_entry) +
                                   cluster_info.cluster_position_in_entry) %
                dnx_data_mdb.dh.nof_macro_clusters_get(unit);
            if ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
                && (cluster_info.cluster_index >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit))
                && (cluster_idx >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
            {
                mdb_cluster_info_t cluster_info_first_bucket;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id,
                                                                  cluster_idx -
                                                                  dnx_data_mdb.dh.nof_bucket_clusters_get(unit),
                                                                  &cluster_info_first_bucket));

                if ((cluster_info.macro_type == cluster_info_first_bucket.macro_type) &&
                    (cluster_info.macro_index == cluster_info_first_bucket.macro_index) &&
                    (cluster_info.start_address == cluster_info_first_bucket.start_address))
                {
                    abk_cluster_counter += dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
                }
            }


            for (iter_cluster_if = 0; iter_cluster_if < dnx_data_mdb.dh.max_nof_cluster_interfaces_get(unit);
                 iter_cluster_if++)
            {

                if (dnx_data_mdb.dh.macro_interface_mapping_get(unit, macro_index)->interfaces[iter_cluster_if] ==
                    mdb_physical_table_id)
                {
                    if (iter_cluster_if < (dnx_data_mdb.dh.max_nof_cluster_interfaces_get(unit) - 1))
                    {
                        if ((macro_usage_state[macro_index] & MDB_DH_INIT_SECOND_CLUSTER_WAS_USED) ||
                            ((macro_usage_state[macro_index] & MDB_DH_INIT_FIRST_CLUSTER_WAS_USED)
                             && cluster_info.cluster_position_in_entry == 0
                             && cluster_info.start_address <= macro_last_address[macro_index]
                             && dnx_data_mdb.dh.macro_interface_mapping_get(unit,
                                                                            macro_index)->interfaces[iter_cluster_if +
                                                                                                     1] ==
                             mdb_physical_table_id))
                        {
                            if ((macro_usage_state[macro_index] & MDB_DH_INIT_SECOND_CLUSTER_WAS_USED) == 0)
                            {
                                macro_usage_state[macro_index] |= MDB_DH_INIT_SECOND_CLUSTER_WAS_USED;
                            }

                            if_instance_used = MDB_DH_INIT_SECOND_IF_USED;
                        }
                    }
                    cluster_interface_id = iter_cluster_if + if_instance_used;

                    macro_usage_state[macro_index] |= MDB_DH_INIT_FIRST_CLUSTER_WAS_USED;

                    break;
                }
            }

            if (cluster_interface_id == dnx_data_mdb.dh.max_nof_cluster_interfaces_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Cluster_idx %d, macro_idx %d of macro type %d does not have an interface with table %d.\n",
                             cluster_info.cluster_index, cluster_info.macro_index, macro_type, mdb_physical_table_id);
            }

            interface_dh_interface =
                dnx_data_mdb.dh.dh_info_get(unit,
                                            mdb_physical_table_id)->table_macro_interface_mapping[macro_index] +
                if_instance_used;


            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&used_macros, interface_dh_interface));

            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                            (&table_allocation[macro_index],
                             cluster_info.cluster_index +
                             (if_instance_used * dnx_data_mdb.dh.nof_macro_clusters_get(unit))));

            if (cluster_info.cluster_position_in_entry == 0 && db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
            {
                key_format_info.em_bank_info[interface_dh_interface].bank_size_used_8k +=
                    ((macro_type == MDB_MACRO_A) ? 2 : 1);
                key_format_info.em_bank_info[interface_dh_interface].macro_index = cluster_info.macro_index;
                key_format_info.em_bank_info[interface_dh_interface].macro_type = macro_type;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                                (&key_format_info.em_bank_info[interface_dh_interface].cluster_mapping,
                                 cluster_info.cluster_index));
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, cluster_info.cluster_index,
                                         cluster_interface_id);

            macro_last_address[macro_index] = cluster_info.start_address;
            if (((macro_usage_state[macro_index] & MDB_DH_INIT_FIRST_IF_CLUSTER_WAS_UPDATED) == 0
                 && if_instance_used == MDB_DH_INIT_FIRST_IF_USED)
                || ((macro_usage_state[macro_index] & MDB_DH_INIT_SECOND_IF_CLUSTER_WAS_UPDATED) == 0
                    && if_instance_used == MDB_DH_INIT_SECOND_IF_USED))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3,
                                             cluster_interface_id, cluster_info.cluster_index);

                SHR_IF_ERR_EXIT(mdb_dh_init_update_if_cluster_offset
                                (unit, if_instance_used, &cluster_info, mdb_physical_table_id));

                macro_usage_state[macro_index] |=
                    (if_instance_used ==
                     MDB_DH_INIT_FIRST_IF_USED) ? MDB_DH_INIT_FIRST_IF_CLUSTER_WAS_UPDATED :
                    MDB_DH_INIT_SECOND_IF_CLUSTER_WAS_UPDATED;
            }

            if (MDB_NOF_CLUSTER_ROW_BYTE *
                (cluster_info.cluster_position_in_entry % dnx_data_mdb.dh.nof_pair_clusters_get(unit)) <=
                dnx_data_mdb.dh.dh_info_get(unit,
                                            mdb_physical_table_id)->cluster_if_offsets_values[data_width_pos_in_array])
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_4,
                                             cluster_info.cluster_index,
                                             dnx_data_mdb.dh.dh_info_get(unit,
                                                                         mdb_physical_table_id)->cluster_if_offsets_values
                                             [macro_index] + dnx_data_mdb.dh.dh_info_get(unit,
                                                                                         mdb_physical_table_id)->cluster_if_offsets_values
                                             [data_width_pos_in_array] * if_instance_used +
                                             MDB_NOF_CLUSTER_ROW_BYTE * (cluster_info.cluster_position_in_entry %
                                                                         dnx_data_mdb.dh.nof_pair_clusters_get(unit)));
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_info.cluster_index,
                                         macro_cluster_counter);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, cluster_info.cluster_index,
                                         abk_cluster_counter);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            DBAL_HANDLE_FREE(unit, entry_handle_id);

            if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS || MDB_DH_IS_ADS_TABLE(mdb_physical_table_id))
            {
                SHR_IF_ERR_EXIT(mdb_dh_lpm_init(unit, row_width, cluster_idx, &cluster_info));
            }
            else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT || db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
            {
                SHR_IF_ERR_EXIT(mdb_dh_direct_init(unit, row_width, cluster_idx, &cluster_info, mdb_physical_table_id));
            }

        }
        SHR_IF_ERR_EXIT(mdb_set_bpu_setting(unit, row_width, table_allocation, mdb_physical_table_id));

        SHR_IF_ERR_EXIT(mdb_dh_set_active_dhs(unit, mdb_physical_table_id, used_macros));

        SHR_IF_ERR_EXIT(mdb_dh_update_em_bank_sizes(unit, mdb_physical_table_id, key_format_info.em_bank_info));

        if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.set(unit, dbal_physical_table_id, &key_format_info));
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_init(
    int unit)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_dh_init_update_default_values(unit));

    for (mdb_physical_table_id = 0; mdb_physical_table_id < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
         mdb_physical_table_id++)
    {
        SHR_IF_ERR_EXIT(mdb_dh_init_table(unit, mdb_physical_table_id));
    }
exit:
    SHR_FUNC_EXIT;
}

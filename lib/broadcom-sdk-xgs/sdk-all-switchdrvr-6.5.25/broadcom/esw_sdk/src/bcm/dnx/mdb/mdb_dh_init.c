
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */


#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>

#include "mdb_init_internal.h"

#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#endif

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX






#define MDB_DH_INIT_IF_CLUSTER_ID_DEFAULT_VALUE      (7)



#define MDB_DH_INIT_CLUSTER_ID_DEFAULT_VALUE         (7)



#define MDB_DH_INIT_SIZE_DH_ENCODING_32BIT           (4)








#define MDB_DH_SUPPORTED_DB_TYPE(db_type) ((db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS) || (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))



#define MDB_DH_IS_MAG_SUPPORTS_FLEX(unit,mdb_table) (dnx_data_mdb.em.flex_mag_supported_get(unit) && MDB_DH_IS_TABLE_SUPPORT_FLEX(unit,mdb_table))



#define MDB_DH_EFFECTIVE_CLUSTER_POS(is_flex,cluster_info) (is_flex ? (cluster_info.cluster_index & 0x1) : cluster_info.cluster_position_in_entry)






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
} mdb_dh_em_emp_nof_aging_bit_options_e;



typedef enum
{
    MDB_EM_BUCKET_WIDTH_120 = 0,
    MDB_EM_BUCKET_WIDTH_240 = 1
} mdb_dh_em_bucket_width_e;














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
    uint8 flex,
    mdb_cluster_info_t * cluster_info_p,
    mdb_physical_table_e mdb_physical_table,
    uint32 *encoding)
{
    uint32 entry_handle_id;
    int cluster_itr;
    uint32 global_macro_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    global_macro_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info_p->macro_type, cluster_info_p->macro_index);

    if (MDB_DH_EFFECTIVE_CLUSTER_POS(flex, (*cluster_info_p)) == 0)
    {

        int nof_clusters =
            (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->row_width /
             dnx_data_mdb.dh.cluster_row_width_bits_get(unit)) << flex;



        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_4, &entry_handle_id));

        for (cluster_itr = 0; cluster_itr < nof_clusters; cluster_itr++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, global_macro_index);

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_1,
                                       cluster_info_p->cluster_index + cluster_itr);

            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                             &encoding[MDB_DH_INIT_SIZE_DH_ENCODING_32BIT * cluster_itr]);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_4, entry_handle_id));
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

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_30, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_dh_init_update_if_cluster_offset(
    int unit,
    int cluster_flags,
    mdb_cluster_info_t * cluster_info_p,
    mdb_physical_table_e mdb_physical_table)
{
    int index;

    

    int macro_global_index;

    

    int entry_data_out_size;

    

    int chunk_offset;
    int chunk_start_index;
    int second_interface;
    uint32 entry_handle_id;
    uint32 row_width;
    uint32 data_width_pos_in_array;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    second_interface = _SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES);

    macro_global_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info_p->macro_type, cluster_info_p->macro_index);

    row_width =
        _SHR_IS_FLAG_SET(cluster_flags,
                         MDB_INIT_CLUSTER_FLAG_FLEX) ? (dnx_data_mdb.
                                                        dh.cluster_row_width_bits_get(unit) << 1) : dnx_data_mdb.
        pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->row_width;



    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_data_mdb.dh.mdb_3_info_get(unit, macro_global_index)->dbal_table, &entry_handle_id));

    

    data_width_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_data_width_get(unit) - 1;

    entry_data_out_size =
        dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->if_cluster_offsets_values[data_width_pos_in_array];

    chunk_start_index =
        dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->if_cluster_offsets_values[macro_global_index] +
        (second_interface * entry_data_out_size);


    if (mdb_lpm_xpt_physical_table_is_bb(unit, mdb_physical_table))
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
    mdb_physical_table_e mdb_physical_table,
    mdb_dh_macro_info_t * macros)
{
    uint32 entry_handle_id;
    uint32 active_dhs_bitmap = 0;
    uint32 flex_active_dhs_bitmap = 0;
    uint32 macro_index;
    uint32 macro_index_max;
    uint32 ovf_pos = 0;
    uint32 last_position_in_macro_interface_array;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    last_position_in_macro_interface_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;

    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {

    

        uint32 interface = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->table_macro_interface_mapping
            [last_position_in_macro_interface_array];

        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&active_dhs_bitmap, interface));
    }

    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        ovf_pos =
            dnx_data_mdb.dh.dh_info_get(unit,
                                        mdb_physical_table)->table_macro_interface_mapping
            [last_position_in_macro_interface_array];
    }

    macro_index_max = dnx_data_mdb.dh.total_nof_macroes_get(unit);


    

    for (macro_index = 0; macro_index < macro_index_max; macro_index++)
    {
        uint32 dh_interfaces;

        

        for (dh_interfaces = 0; dh_interfaces < MDB_DH_INIT_MAX_AVAILABLE_INTERFACES; dh_interfaces++)
        {
            if (macros[macro_index].intf[dh_interfaces].alloc_bit_map != 0)
            {
                uint32 macro_serial_pos_of_pdb = dnx_data_mdb.dh.dh_info_get(unit,
                                                                             mdb_physical_table)->
                    table_macro_interface_mapping[macro_index] + dh_interfaces;

                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&active_dhs_bitmap, macro_serial_pos_of_pdb));

                

                if (_SHR_IS_FLAG_SET(macros[macro_index].intf[dh_interfaces].flags, MDB_INIT_CLUSTER_FLAG_FLEX))
                {
                    uint32 entry_handle_id_dual_mode;

                    uint32 dual_mode = 0;

                    

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit(&flex_active_dhs_bitmap, macro_serial_pos_of_pdb));



                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_43, &entry_handle_id_dual_mode));

                    dbal_entry_key_field32_set(unit, entry_handle_id_dual_mode, DBAL_FIELD_MDB_KEY_0, macro_index);

                    dbal_value_field32_request(unit, entry_handle_id_dual_mode, DBAL_FIELD_MDB_ITEM_0,
                                               INST_SINGLE, &dual_mode);

                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_dual_mode, DBAL_COMMIT));

                    dual_mode |= macros[macro_index].intf[dh_interfaces].alloc_bit_map;

                    dbal_entry_value_field32_set(unit, entry_handle_id_dual_mode, DBAL_FIELD_MDB_ITEM_0,
                                                 INST_SINGLE, dual_mode);

                    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_dual_mode, DBAL_COMMIT));
                }
            }

        }
    }



    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit, dnx_data_mdb.direct.mdb_26_info_get(unit, mdb_physical_table)->dbal_table,
                     &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, active_dhs_bitmap);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, active_dhs_bitmap);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        uint32 active_dhs_with_flex = active_dhs_bitmap;

        

        if (MDB_DH_IS_MAG_SUPPORTS_FLEX(unit, mdb_physical_table))
        {
            active_dhs_with_flex |= (flex_active_dhs_bitmap << ovf_pos);

            ovf_pos <<= 1;
        }



        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_16_info_get(unit, mdb_physical_table)->dbal_table,
                         &entry_handle_id));

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                     active_dhs_with_flex | (1 << ovf_pos));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        

        if (mdb_em_age_supported_internal(unit, mdb_physical_table) == TRUE)
        {

            

            uint32 nof_ones = utilex_bitstream_get_nof_on_bits(&active_dhs_with_flex, 1);
            uint32 split_pos = 0;
            uint32 found_count = 0;
            uint32 entry_handle_emp_id;
            if (nof_ones != 0)
            {

                

                split_pos = ovf_pos;
                while (found_count < ((nof_ones + 1) >> 1))
                {
                    split_pos--;
                    found_count += (active_dhs_with_flex >> split_pos) & 0x1;
                }

                

                if ((nof_ones & 0x1) == 0 && ((active_dhs_with_flex >> (split_pos - 1)) & 0x1) == 0)
                {
                    split_pos--;
                }
            }



            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit,
                             dnx_data_mdb.em.mdb_23_info_get(unit,
                                                             mdb_physical_table)->dbal_table, &entry_handle_emp_id));

            dbal_entry_value_field32_set(unit, entry_handle_emp_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                         active_dhs_with_flex | (1 << ovf_pos));

            dbal_entry_value_field32_set(unit, entry_handle_emp_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, split_pos);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_emp_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_dh_em_emp_get_nof_aging_bits(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 nof_rows,
    uint32 *nof_aging_bits)
{
    uint32 available_aging_bits;
    SHR_FUNC_INIT_VARS(unit);

    if (nof_rows > 0)
    {
        available_aging_bits =
            dnx_data_mdb.em.em_aging_info_get(unit, dbal_physical_table_id)->total_nof_aging_bits / nof_rows;

        

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

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_dh_init_update_em_bank_sizes(
    int unit,
    mdb_physical_table_e mdb_physical_table,
    dh_em_bank_info * em_bank_info)
{
    uint32 entry_handle_mdb_29_table;
    uint32 entry_handle_mdb_24_table;
    uint32 nof_rows = 0;
    uint32 nof_aging_bits = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        int bank_iter;
        int nof_banks = dnx_data_mdb.em.mdb_29_info_get(unit, mdb_physical_table)->mdb_item_0_array_size;
        int table_used = FALSE;
        int row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->row_width;
        int nof_rows_granularity = dnx_data_mdb.dh.macro_type_info_get(unit,
                                                                       dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                          mdb_physical_table)->macro_granularity)->
            nof_rows;
        dbal_physical_tables_e dbal_physical_table_id =
            dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->physical_to_logical;
        int age_support = mdb_em_age_supported_internal(unit, mdb_physical_table);

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.em.mdb_29_info_get(unit, mdb_physical_table)->dbal_table,
                         &entry_handle_mdb_29_table));

        if (age_support == TRUE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.em.mdb_24_info_get(unit, mdb_physical_table)->dbal_table,
                             &entry_handle_mdb_24_table));
        }

        for (bank_iter = 0; bank_iter < nof_banks; bank_iter++)
        {
            mdb_dh_em_bucket_width_e bucket_width = (em_bank_info[bank_iter].is_flex
                                                     || (row_width ==
                                                         2 *
                                                         dnx_data_mdb.
                                                         dh.cluster_row_width_bits_get(unit))) ? MDB_EM_BUCKET_WIDTH_240
                : MDB_EM_BUCKET_WIDTH_120;
            if (em_bank_info[bank_iter].bank_size_used_nof_rows > 0)
            {

                

                uint32 bucket_size_log2 =
                    utilex_log2_round_up((em_bank_info[bank_iter].bank_size_used_nof_rows /
                                          nof_rows_granularity) >> em_bank_info[bank_iter].is_flex);


                dbal_entry_value_field32_set(unit, entry_handle_mdb_29_table, DBAL_FIELD_MDB_ITEM_0, bank_iter,
                                             bucket_size_log2);

                if (dnx_data_mdb.em.flex_mag_supported_get(unit))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_mdb_29_table, DBAL_FIELD_MDB_ITEM_1, bank_iter,
                                                 bucket_width);
                }

                table_used = TRUE;

                

                if (age_support == TRUE)
                {
                    uint32 banks_to_update = 1;
                    uint32 emp_bank_iter;

                    

                    uint32 nof_entries_in_bank = em_bank_info[bank_iter].bank_size_used_nof_rows;

                    

                    if (em_bank_info[bank_iter].is_flex)
                    {
                        banks_to_update = 2;
                        nof_entries_in_bank >>= 1;
                    }

                    for (emp_bank_iter = 0; emp_bank_iter < banks_to_update; emp_bank_iter++)
                    {

                        

                        dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_1,
                                                     bank_iter + (nof_banks * emp_bank_iter), nof_rows >> 3);

                        dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_0,
                                                     bank_iter + (nof_banks * emp_bank_iter),
                                                     utilex_log2_round_up(nof_entries_in_bank));

                        

                        nof_rows += nof_entries_in_bank;
                    }
                }

            }
        }

        if (table_used == TRUE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_29_table, DBAL_COMMIT));
        }

        if (age_support == TRUE)
        {
            

            uint32 ovf_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;
            uint32 ovf_pos =
                dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table)->table_macro_interface_mapping[ovf_pos_in_array];
            uint32 log2_ovf_size =
                utilex_log2_round_up(dnx_data_mdb.em.em_info_get(unit, dbal_physical_table_id)->ovf_cam_size);

            

            if (MDB_DH_IS_MAG_SUPPORTS_FLEX(unit, mdb_physical_table))
            {
                ovf_pos <<= 1;
            }

            dbal_entry_value_field32_set(unit, entry_handle_mdb_24_table, DBAL_FIELD_MDB_ITEM_0, ovf_pos,
                                         log2_ovf_size);

            SHR_IF_ERR_EXIT(mdb_dh_em_emp_get_nof_aging_bits(unit, dbal_physical_table_id, nof_rows, &nof_aging_bits));

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
mdb_dh_em_way_index_get(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    mdb_macro_types_e macro_type,
    uint8 macro_idx,
    uint8 cluster_idx,
    uint32 *way_index)
{
    uint32 way_index_temp;
    int adjusted_cluster_macro_idx;
    uint8 cluster_flags;
    uint32 ovf_pos;
    uint32 last_position_in_macro_interface_array;

    SHR_FUNC_INIT_VARS(unit);

    if (mdb_physical_table_id >= MDB_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid MDB table ID %d.\n", mdb_physical_table_id);
    }

    last_position_in_macro_interface_array = dnx_data_mdb.dh.total_nof_macroes_plus_em_ovf_or_eedb_bank_get(unit) - 1;

    ovf_pos = dnx_data_mdb.dh.dh_info_get(unit,
                                          mdb_physical_table_id)->table_macro_interface_mapping
        [last_position_in_macro_interface_array];

    if (macro_type == MDB_NOF_MACRO_TYPES)
    {

        

        if (MDB_DH_IS_MAG_SUPPORTS_FLEX(unit, mdb_physical_table_id))
        {
            way_index_temp = ovf_pos * 2;
        }
        else
        {
            way_index_temp = ovf_pos;
        }
    }
    else if (macro_type == MDB_EEDB_BANK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MDB EM tables are not associated with EEDB bank macro_type.\n");
    }
    else
    {

        

        adjusted_cluster_macro_idx = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, macro_type, macro_idx);

        

        way_index_temp =
            dnx_data_mdb.dh.dh_info_get(unit,
                                        mdb_physical_table_id)->table_macro_interface_mapping
            [adjusted_cluster_macro_idx];

        if (way_index_temp >= ovf_pos)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Invalid combination of MDB table ID %d, macro_type %d, macro_idx %d, cluster_idx %d.\n",
                         mdb_physical_table_id, macro_type, macro_idx, cluster_idx);
        }

        

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.flags.get(unit, macro_type,
                                                                                     MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE
                                                                                     (unit, macro_type, macro_idx,
                                                                                      cluster_idx), &cluster_flags));

        

        if ((_SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_FLEX)) && (cluster_idx & 0x1))
        {
            way_index_temp += ovf_pos;
        }

        

        if (_SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES))
        {
            way_index_temp++;
        }
    }

    *way_index = way_index_temp;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
mdb_dh_get_cluster_granularity(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx,
    int cluster_idx,
    int *cluster_granularity)
{
    dbal_tables_e dbal_table;
    uint32 entry_handle_id;
    uint32 bpu_mode;
    int cluster_granularity_temp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    dbal_table = dnx_data_mdb.dh.mdb_2_info_get(unit,
                                                MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, macro_type, macro_idx))->dbal_table;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_idx, &bpu_mode);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if ((bpu_mode == MDB_BPU_MODE_DIRECT_120) || (bpu_mode == MDB_BPU_MODE_EM_120))
    {
        cluster_granularity_temp = 1;
    }
    else if ((bpu_mode == MDB_BPU_MODE_DIRECT_240) || (bpu_mode == MDB_BPU_MODE_EM_240))
    {
        cluster_granularity_temp = 2;
    }
    else if (bpu_mode == MDB_BPU_MODE_LPM)
    {
        cluster_granularity_temp = 4;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. Unexpected bpu_mode (%d) for macro_type (%d), macro_idx (%d) and cluster_idx (%d).\n",
                     bpu_mode, macro_type, macro_idx, cluster_idx);
    }

    *cluster_granularity = cluster_granularity_temp;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_dh_set_bpu_setting_macro(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_idx)
{
    mdb_physical_table_e mdb_physical_table_id_array[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS];
    int cluster_idx;
    uint32 bpu_setup[DNX_DATA_MAX_MDB_DH_NOF_BUCKETS_IN_MACRO] = { 0 };

    

    uint32 bpu_setup_bitmap = 0;
    int nof_clusters_in_bucket;
    int bucket_index;
    uint32 entry_handle_id;
    uint8 prev_dh_interface = 0;
    uint8 kaps_480_bpu = 0;
    uint32 max_nof_macro_clusters;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_clusters_in_bucket = dnx_data_mdb.dh.nof_bucket_clusters_get(unit);

    if (macro_type >= MDB_EEDB_BANK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. Invalid macro_type %d, only macro A or macro B have an associated BPU setting.\n",
                     macro_type);
    }

    

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                    (unit,
                     dnx_data_mdb.dh.mdb_2_info_get(unit,
                                                    MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, macro_type,
                                                                                  macro_idx))->dbal_table,
                     &entry_handle_id));

    

    max_nof_macro_clusters = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters;
    for (cluster_idx = 0; cluster_idx < max_nof_macro_clusters; cluster_idx++)
    {
        dbal_enum_value_field_mdb_db_type_e db_type;
        uint32 bpu_mode;
        uint32 em_240_mode;
        uint32 row_width;
        uint8 cluster_flags;
        uint8 current_dh_interface;
        int cluster_glob_index = MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit, macro_type, macro_idx, cluster_idx);

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                        pdb.get(unit, macro_type, cluster_glob_index, &mdb_physical_table_id_array[cluster_idx]));
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                        flags.get(unit, macro_type, cluster_glob_index, &cluster_flags));

        current_dh_interface = _SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES);

        if ((cluster_idx == 0) || (mdb_physical_table_id_array[cluster_idx] == MDB_NOF_PHYSICAL_TABLES)
            || (mdb_physical_table_id_array[cluster_idx] != mdb_physical_table_id_array[cluster_idx - 1])
            || (current_dh_interface != prev_dh_interface))
        {
            bpu_setup_bitmap |= 1 << cluster_idx;
        }

        

        if (mdb_physical_table_id_array[cluster_idx] == MDB_NOF_PHYSICAL_TABLES)
        {
            bpu_mode = MDB_BPU_MODE_EM_120;
        }
        else
        {
            db_type = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id_array[cluster_idx])->db_type;
            row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id_array[cluster_idx])->row_width;
            em_240_mode = ((row_width == 2 * dnx_data_mdb.dh.cluster_row_width_bits_get(unit))
                           || _SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_FLEX)) ? 1 : 0;

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
                kaps_480_bpu |= _SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_480_BPU);
                bpu_mode = MDB_BPU_MODE_LPM;
            }
            else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM && !em_240_mode)
            {
                bpu_mode = MDB_BPU_MODE_EM_120;

                

                if (_SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES) && (cluster_idx != 0)
                    && (mdb_physical_table_id_array[cluster_idx] == mdb_physical_table_id_array[cluster_idx - 1]))
                {
                    int nof_clusters;
                    int table_cluster_idx;

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                    number_of_clusters.get(unit, mdb_physical_table_id_array[cluster_idx],
                                                           &nof_clusters));

                    

                    for (table_cluster_idx = 0; table_cluster_idx < nof_clusters; table_cluster_idx++)
                    {
                        mdb_cluster_info_t cluster_info;

                        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                        clusters_info.get(unit, mdb_physical_table_id_array[cluster_idx],
                                                          table_cluster_idx, &cluster_info));

                        if ((cluster_info.macro_type == macro_type) && (cluster_info.macro_index == macro_idx) &&
                            (cluster_info.cluster_index == cluster_idx) && (cluster_info.start_address == 0))
                        {
                            bpu_setup_bitmap |= 1 << cluster_idx;
                            break;
                        }
                    }
                }
            }
            else if (db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM && em_240_mode)
            {
                bpu_mode = MDB_BPU_MODE_EM_240;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. MDB physical table %d is not associated with a valid BPU mode value.\n",
                             mdb_physical_table_id_array[cluster_idx]);
            }
        }
        prev_dh_interface = current_dh_interface;
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_idx, bpu_mode);
    }

    

    if (bpu_setup_bitmap == 0x1)
    {

        

        

        if (kaps_480_bpu)
        {
            bpu_setup[0] =
                dnx_data_mdb.dh.bpu_setup_size_480_get(unit) | dnx_data_mdb.dh.bpu_setup_bb_connected_get(unit);
            bpu_setup[1] = dnx_data_mdb.dh.bpu_setup_size_480_get(unit);
        }
        else
        {
            bpu_setup[0] =
                dnx_data_mdb.dh.bpu_setup_size_480_odd_get(unit) | dnx_data_mdb.dh.bpu_setup_bb_connected_get(unit);
            bpu_setup[1] =
                dnx_data_mdb.dh.bpu_setup_size_480_get(unit) | dnx_data_mdb.dh.bpu_setup_bb_connected_get(unit);
        }
    }
    else
    {

        

        if (utilex_bitstream_test_bit(&bpu_setup_bitmap, nof_clusters_in_bucket) == FALSE)
        {

            

            bpu_setup[0] |= dnx_data_mdb.dh.bpu_setup_bb_connected_get(unit);
        }

        

        bpu_setup_bitmap |= 1 << nof_clusters_in_bucket;

        

        for (bucket_index = 0; bucket_index < dnx_data_mdb.dh.nof_buckets_in_macro_get(unit); bucket_index++)
        {

            

            uint32 relevant_bpu_setup_bitmap =
                (bpu_setup_bitmap >> (nof_clusters_in_bucket * bucket_index)) & ((1 << nof_clusters_in_bucket) - 1);
            int bpu_setup_extended_support =
                dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_bpu_setup_extended_support);

            

            if (relevant_bpu_setup_bitmap == 0x1)
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_480_get(unit);
            }
            else if ((bpu_setup_extended_support == TRUE) && (relevant_bpu_setup_bitmap == 0x3))
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_360_120_get(unit);
            }
            else if (relevant_bpu_setup_bitmap == 0x5)
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_240_240_get(unit);
            }
            else if (relevant_bpu_setup_bitmap == 0x7)
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_240_120_120_get(unit);
            }
            else if ((bpu_setup_extended_support == TRUE) && (relevant_bpu_setup_bitmap == 0x9))
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_120_360_get(unit);
            }
            else if ((bpu_setup_extended_support == TRUE) && (relevant_bpu_setup_bitmap == 0xb))
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_120_240_120_get(unit);
            }
            else if (relevant_bpu_setup_bitmap == 0xd)
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_120_120_240_get(unit);
            }
            else if (relevant_bpu_setup_bitmap == 0xf)
            {
                bpu_setup[bucket_index] |= dnx_data_mdb.dh.bpu_setup_size_120_120_120_120_get(unit);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. MDB macro type %d, macro index %d is not associated with a valid BPU mode value.\n",
                             macro_type, macro_idx);
            }
        }
    }

    for (bucket_index = 0; bucket_index < dnx_data_mdb.dh.nof_buckets_in_macro_get(unit); bucket_index++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, bucket_index,
                                     bpu_setup[bucket_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_dh_set_bpu_setting(
    int unit)
{
    int global_macro_index;
    int macro_index;
    mdb_macro_types_e macro_type = 0;
    int total_nof_macros = dnx_data_mdb.dh.total_nof_macroes_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (global_macro_index = 0; global_macro_index < total_nof_macros; global_macro_index++)
    {
        macro_type = dnx_data_mdb.dh.logical_macro_info_get(unit, global_macro_index)->macro_type;
        macro_index = dnx_data_mdb.dh.logical_macro_info_get(unit, global_macro_index)->macro_index;

        SHR_IF_ERR_EXIT(mdb_dh_set_bpu_setting_macro(unit, macro_type, macro_index));

    }

exit:
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
        uint32 max_fmt_val = dnx_data_mdb.kaps.max_fmt_get(unit);

        if (row_width ==
            dnx_data_mdb.dh.cluster_row_width_bits_get(unit) * dnx_data_mdb.dh.nof_bucket_clusters_get(unit))
        {

            

            if ((fmt_index >= MDB_LPM_MIN_FMT) &&
                (fmt_index <= max_fmt_val) &&
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
            if (!dnx_data_mdb.global.adapter_stub_enable_get(unit))
            {
                fmt_val = fmt_index;
            }
#endif 
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

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_6, entry_handle_mdb_table_6));

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
    mdb_physical_table_e mdb_physical_table_id)
{
    uint32 entry_handle_mdb_table_6;
    uint32 entry_handle_mdb_table_5;
    int table_entry_size;
    int key_index;
    uint32 cluster_offset = 0;
    uint32 global_macro_index;
    uint8 multiplier = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);



    SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, mdb_physical_table_id, &table_entry_size));

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

                    

                    table_actual_entry_size =
                        MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                                 mdb_physical_table_id)->direct_max_payload_type);

                    

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

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_6, entry_handle_mdb_table_6));

    }

    dbal_entry_key_field32_set(unit, entry_handle_mdb_table_5, DBAL_FIELD_MDB_KEY_0, global_macro_index);

    dbal_entry_key_field32_set(unit, entry_handle_mdb_table_5, DBAL_FIELD_MDB_KEY_1, cluster_info_p->cluster_index);

    dbal_entry_value_field32_set(unit, entry_handle_mdb_table_5, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, cluster_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_table_5, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_5, entry_handle_mdb_table_5));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_dh_init_update_default_values(
    int unit)
{
    int macro_iter, cluster_iter;
    uint32 entry_handle_id, entry_handle_mdb_table_abk;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    for (macro_iter = 0; macro_iter < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_iter++)
    {
        int mag_iter, nof_mag;
        mdb_macro_types_e macro_type = dnx_data_mdb.dh.logical_macro_info_get(unit, macro_iter)->macro_type;
        uint32 max_nof_macro_clusters = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters;
        uint8 hitbit_support = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->hitbit_support;



        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                        (unit, dnx_data_mdb.dh.mdb_1_info_get(unit, macro_iter)->dbal_table, &entry_handle_id));


        
        
        if (hitbit_support == TRUE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_35, &entry_handle_mdb_table_abk));

            dbal_entry_key_field32_set(unit, entry_handle_mdb_table_abk, DBAL_FIELD_MDB_KEY_0, macro_iter);
        }

        nof_mag = dnx_data_mdb.dh.mdb_1_info_get(unit, macro_iter)->mdb_item_2_array_size;

        

        for (mag_iter = 0; mag_iter < nof_mag; mag_iter++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2,
                                         mag_iter, MDB_DH_INIT_IF_CLUSTER_ID_DEFAULT_VALUE);
        }

        

        for (cluster_iter = 0; cluster_iter < max_nof_macro_clusters; cluster_iter++)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_iter,
                                         MDB_DH_INIT_CLUSTER_ID_DEFAULT_VALUE);

            if (hitbit_support == TRUE)
            {
                dbal_entry_value_field32_set(unit, entry_handle_mdb_table_abk, DBAL_FIELD_MDB_ITEM_0, cluster_iter,
                                             MDB_DH_INIT_CLUSTER_ID_DEFAULT_VALUE);
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);

        if (hitbit_support == TRUE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_mdb_table_abk, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_mdb_table_abk);
        }
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
    soc_reg_above_64_val_t vmv_encoding_data_flex;
    mdb_em_encoding_request_e encoding_request;
    uint32 row_width;
    int nof_clusters;
    int cluster_idx;
    mdb_physical_table_e mdb_physical_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    mdb_physical_table = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;

    row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table)->row_width;

    encoding_request =
        (row_width ==
         dnx_data_mdb.
         dh.cluster_row_width_bits_get(unit)) ? MDB_EM_ENCODING_DH_ENTRY_120 : MDB_EM_ENCODING_DH_ENTRY_240;

    sal_memset(vmv_encoding_data, 0x0, sizeof(vmv_encoding_data[0]) * SOC_REG_ABOVE_64_MAX_SIZE_U32);
    SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs
                    (unit, encoding_request, encoding_request, dbal_physical_table_id, 0, vmv_encoding_data, NULL));

    

    if (encoding_request == MDB_EM_ENCODING_DH_ENTRY_120)
    {
        sal_memset(vmv_encoding_data_flex, 0x0, sizeof(vmv_encoding_data_flex[0]) * SOC_REG_ABOVE_64_MAX_SIZE_U32);
        SHR_IF_ERR_EXIT(mdb_em_calc_vmv_regs
                        (unit, encoding_request, MDB_EM_ENCODING_DH_ENTRY_240, dbal_physical_table_id,
                         0, vmv_encoding_data_flex, NULL));
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters));

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        mdb_cluster_info_t cluster_info;
        uint8 cluster_flags;
        uint8 is_flex;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table, cluster_idx, &cluster_info));

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.flags.get(unit, cluster_info.macro_type,
                                                                                     MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE
                                                                                     (unit, cluster_info.macro_type,
                                                                                      cluster_info.macro_index,
                                                                                      cluster_info.cluster_index),
                                                                                     &cluster_flags));

        is_flex = _SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_FLEX);

        SHR_IF_ERR_EXIT(mdb_dh_em_init
                        (unit, is_flex, &cluster_info, mdb_physical_table,
                         (is_flex ? vmv_encoding_data_flex : vmv_encoding_data)));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dh_init_table(
    int unit,
    mdb_physical_table_e mdb_physical_table_id)
{
    int nof_clusters;
    int max_nof_macro_clusters = dnx_data_mdb.dh.max_nof_clusters_get(unit);

    
    dbal_physical_tables_e dbal_physical_table_id;

    dbal_enum_value_field_mdb_db_type_e dbal_db_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

    dbal_physical_table_id = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->physical_to_logical;

    
    if (dbal_physical_table_id == DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_EXIT();
    }

    dbal_db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->db_type;

    

    if ((dbal_db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
        || (mdb_eedb_table_to_mem_type(unit, mdb_physical_table_id) == MDB_EEDB_MEM_TYPE_DATA_CLUSTER))
    {
        int cluster_idx;
        uint32 entry_handle_id, entry_handle_id_abk;
        uint32 row_width;

        
        dbal_enum_value_field_mdb_db_type_e mdb_db_type;

        

        mdb_dh_macro_info_t macros[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES];

        

        em_key_format_info key_format_info;

        

        uint8 macro_cluster_counter = 0;

        sal_memset(macros, 0x0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES * sizeof(mdb_dh_macro_info_t));

        mdb_db_type = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->db_type;

        

        if (!MDB_DH_SUPPORTED_DB_TYPE(mdb_db_type))
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.get(unit, dbal_physical_table_id, &key_format_info));

        row_width = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->row_width;

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        {
            SHR_IF_ERR_EXIT(mdb_dh_init_table_em_encoding(unit, dbal_physical_table_id));
        }

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            mdb_macro_types_e macro_type;
            int abk_cluster_counter = 0;
            uint8 cluster_interface_id;
            mdb_dh_init_interface_instance_in_macro_e if_instance_used;
            uint32 global_macro_index;

            

            uint32 dh_serial_number_in_mag;
            mdb_cluster_info_t cluster_info;

            

            uint32 data_width_pos_in_array = dnx_data_mdb.dh.total_nof_macroes_plus_data_width_get(unit) - 1;
            int nof_cluster_per_entry;
            uint32 start_address_remapped = 0;
            uint8 cluster_flags;
            uint8 is_flex;

            
            mdb_physical_table_e dh_mdb_table_id;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, cluster_idx, &cluster_info));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            nof_cluster_per_entry.get(unit, mdb_physical_table_id, &nof_cluster_per_entry));

            macro_type = cluster_info.macro_type;

            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.flags.get(unit, macro_type,
                                                                                         MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE
                                                                                         (unit, cluster_info.macro_type,
                                                                                          cluster_info.macro_index,
                                                                                          cluster_info.cluster_index),
                                                                                         &cluster_flags));

            if_instance_used =
                _SHR_IS_FLAG_SET(cluster_flags,
                                 MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES) ? MDB_DH_INIT_SECOND_IF_USED :
                MDB_DH_INIT_FIRST_IF_USED;

            is_flex = _SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_FLEX);

            if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
            {

                

                if (cluster_info.start_address != MDB_DIRECT_INVALID_START_ADDRESS)
                {

                    

                    start_address_remapped = cluster_info.start_address;
                }
                else
                {

                    

                    int cluster_idx_inner;
                    int cluster_id_iter;
                    mdb_cluster_info_t cluster_info_inner;
                    uint8 cluster_id_used[DNX_DATA_MAX_MDB_DH_MAX_NOF_CLUSTERS];
                    uint8 shift_bits = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_address_bits;

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
                            cluster_id_used[(cluster_info.start_address >> shift_bits) % max_nof_macro_clusters] = 1;
                        }
                    }

                    

                    for (cluster_id_iter = 0; cluster_id_iter < max_nof_macro_clusters; cluster_id_iter++)
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
            else if (MDB_INIT_IS_FEC_TABLE(mdb_physical_table_id)
                     && dnx_data_mdb.direct.feature_get(unit, dnx_data_mdb_direct_fec_address_remap))
            {
                int address_space_bits = 0;
                int fec_cluster_granularity = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit,
                                                                                 mdb_physical_table_id)->row_width /
                    DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS;
                uint32 cluster_unit_index = (cluster_info.cluster_index / fec_cluster_granularity);
                uint32 fec_address_mapping_bits_offset =
                    dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_address_bits;
                uint32 address_mapping_field_offset;

                

                uint32 start_address_mask;

                
                SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                address_space_bits.get(unit, mdb_physical_table_id, &address_space_bits));

                
                address_mapping_field_offset =
                    address_space_bits - dnx_data_mdb.direct.fec_address_mapping_bits_get(unit);

                

                if (fec_address_mapping_bits_offset < address_mapping_field_offset)
                {
                    fec_address_mapping_bits_offset = address_mapping_field_offset;
                }

                
                start_address_mask =
                    UTILEX_ZERO_BITS_MASK((fec_address_mapping_bits_offset +
                                           dnx_data_mdb.direct.fec_address_mapping_bits_relevant_get(unit)) - 1,
                                          fec_address_mapping_bits_offset);

                start_address_remapped =
                    (cluster_info.start_address & start_address_mask) | (cluster_unit_index <<
                                                                         fec_address_mapping_bits_offset);
            }
#endif 
            else
            {
                start_address_remapped = cluster_info.start_address;
            }

            macro_cluster_counter =
                (start_address_remapped / (dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_rows)) %
                max_nof_macro_clusters;
            global_macro_index =
                dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->global_start_index + cluster_info.macro_index;



            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, dnx_data_mdb.dh.mdb_1_info_get(unit, global_macro_index)->dbal_table,
                             &entry_handle_id));

            
            
            {
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_35, &entry_handle_id_abk));
                dbal_entry_key_field32_set(unit, entry_handle_id_abk, DBAL_FIELD_MDB_KEY_0, global_macro_index);

                abk_cluster_counter = ((macro_cluster_counter * (nof_cluster_per_entry << is_flex)) +
                                       MDB_DH_EFFECTIVE_CLUSTER_POS(is_flex, cluster_info)) % max_nof_macro_clusters;
            }

            if ((mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
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
                    (cluster_info.start_address == cluster_info_first_bucket.start_address)
                    )
                {
                    abk_cluster_counter += dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
                }
            }

            {
                dh_mdb_table_id = mdb_physical_table_id;
            }

            
            cluster_interface_id =
                dnx_data_mdb.dh.dh_info_get(unit, dh_mdb_table_id)->macro_mapping[global_macro_index];

            if (cluster_interface_id == dnx_data_mdb.dh.max_nof_cluster_interfaces_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Cluster_idx %d, macro_idx %d of macro type %d does not have an interface with table %d.\n",
                             cluster_info.cluster_index, cluster_info.macro_index, macro_type, mdb_physical_table_id);
            }


            cluster_interface_id += if_instance_used;

            dh_serial_number_in_mag =
                dnx_data_mdb.dh.dh_info_get(unit,
                                            mdb_physical_table_id)->table_macro_interface_mapping[global_macro_index] +
                if_instance_used;

            

            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                            (&macros[global_macro_index].intf[if_instance_used].alloc_bit_map,
                             cluster_info.cluster_index));

            

            if (cluster_info.cluster_position_in_entry == 0 && mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
            {
                key_format_info.em_bank_info[dh_serial_number_in_mag].bank_size_used_nof_rows +=
                    dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_rows;
                key_format_info.em_bank_info[dh_serial_number_in_mag].macro_index = cluster_info.macro_index;
                key_format_info.em_bank_info[dh_serial_number_in_mag].macro_type = macro_type;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                                (&key_format_info.em_bank_info[dh_serial_number_in_mag].cluster_mapping,
                                 cluster_info.cluster_index));
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, cluster_info.cluster_index,
                                         cluster_interface_id);

            

            if (_SHR_IS_FLAG_SET(cluster_flags, MDB_INIT_CLUSTER_FLAG_FIRST_INTF_CLUSTER))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2,
                                             cluster_interface_id, cluster_info.cluster_index);

                SHR_IF_ERR_EXIT(mdb_dh_init_update_if_cluster_offset
                                (unit, cluster_flags, &cluster_info, mdb_physical_table_id));

                key_format_info.em_bank_info[dh_serial_number_in_mag].is_flex = is_flex;

                macros[global_macro_index].intf[if_instance_used].flags |= (cluster_flags & MDB_INIT_CLUSTER_FLAG_FLEX);
            }

            

            if (MDB_NOF_CLUSTER_ROW_BYTE *
                (cluster_info.cluster_position_in_entry % dnx_data_mdb.dh.nof_pair_clusters_get(unit)) <=
                dnx_data_mdb.dh.dh_info_get(unit,
                                            mdb_physical_table_id)->cluster_if_offsets_values[data_width_pos_in_array])
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3,
                                             cluster_info.cluster_index,
                                             dnx_data_mdb.dh.dh_info_get(unit,
                                                                         mdb_physical_table_id)->cluster_if_offsets_values
                                             [global_macro_index] + dnx_data_mdb.dh.dh_info_get(unit,
                                                                                                mdb_physical_table_id)->cluster_if_offsets_values
                                             [data_width_pos_in_array] * if_instance_used +
                                             MDB_NOF_CLUSTER_ROW_BYTE * (cluster_info.cluster_position_in_entry %
                                                                         dnx_data_mdb.dh.nof_pair_clusters_get(unit)));
            }

            

            if ((cluster_info.start_address != MDB_DIRECT_INVALID_START_ADDRESS) &&
                (cluster_info.start_address %
                 dnx_data_mdb.dh.macro_type_info_get(unit, cluster_info.macro_type)->nof_rows != 0))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "%s: The address %d is not a multiple of the basic size of macro type %s.\n",
                             dbal_physical_table_to_string(unit, dbal_physical_table_id), cluster_info.start_address,
                             dnx_data_mdb.dh.macro_type_info_get(unit, cluster_info.macro_type)->name);
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, cluster_info.cluster_index,
                                         macro_cluster_counter);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            DBAL_HANDLE_FREE(unit, entry_handle_id);

            {
                dbal_entry_value_field32_set(unit, entry_handle_id_abk, DBAL_FIELD_MDB_ITEM_0,
                                             cluster_info.cluster_index, abk_cluster_counter);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_abk, DBAL_COMMIT));
                DBAL_HANDLE_FREE(unit, entry_handle_id_abk);
            }

            
            if ((mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
                && (row_width != dnx_data_mdb.dh.cluster_row_width_bits_get(unit)))
            {
                SHR_IF_ERR_EXIT(mdb_dh_lpm_init(unit, row_width, cluster_idx, &cluster_info));
            }
            else if ((mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
                     || (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
                     || ((mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
                         && (row_width == dnx_data_mdb.dh.cluster_row_width_bits_get(unit))))
            {
                SHR_IF_ERR_EXIT(mdb_dh_direct_init(unit, row_width, cluster_idx, &cluster_info, mdb_physical_table_id));
            }

        }

        {
            SHR_IF_ERR_EXIT(mdb_dh_set_active_dhs(unit, mdb_physical_table_id, macros));
        }

        SHR_IF_ERR_EXIT(mdb_dh_init_update_em_bank_sizes(unit, mdb_physical_table_id, key_format_info.em_bank_info));

        if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
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
    mdb_physical_table_e mdb_physical_table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_dh_init_update_default_values(unit));

    for (mdb_physical_table_id = 0; mdb_physical_table_id < MDB_NOF_PHYSICAL_TABLES; mdb_physical_table_id++)
    {
        {
            SHR_IF_ERR_EXIT(mdb_dh_init_table(unit, mdb_physical_table_id));
        }
    }

    SHR_IF_ERR_EXIT(mdb_dh_set_bpu_setting(unit));
exit:
    SHR_FUNC_EXIT;
}

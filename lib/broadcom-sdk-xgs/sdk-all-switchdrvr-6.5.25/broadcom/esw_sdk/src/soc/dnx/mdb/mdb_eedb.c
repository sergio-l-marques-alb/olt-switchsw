
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/utils/dnx_ire_packet_utils.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include "../dbal/dbal_internal.h"
#include "mdb_internal.h"
#include <soc/dnx/utils/dnx_sbusdma_desc.h>







#define EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit)  (dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get(_unit))

#define EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW (MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE)

#define EEDB_HIT_BIT_NOF_ENTRIES_PER_ROW(_unit)    (EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit) * EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW)



#define EEDB_ADDR_MAP_LL_TABLE_OFFSET        (0)
#define EEDB_ADDR_MAP_LL_CLUSTER_OFFSET      (8)
#define EEDB_ADDR_MAP_DATA_TABLE_OFFSET     (16)
#define EEDB_ADDR_MAP_DATA_CLUSTER_OFFSET   (24)
#define EEDB_ADDR_MAP_MASK                (0xFF)
#define EEDB_ADDR_MAP_INVALID             (0xFF)







#define EEDB_DATA_ROW_TO_HIT_BIT_ROW(_unit, _data_cluster_row) \
    (_data_cluster_row / EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit))



#define EEDB_GET_HIT_BIT_ROW_OFFSET(_unit, _data_cluster_row, _data_row_offset) \
    (((_data_cluster_row % EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit)) * EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW) \
    + (EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW - (_data_row_offset / MDB_DIRECT_BASIC_ENTRY_SIZE) - 1))











typedef struct mdb_eedb_entry_info
{

    

    dbal_enum_value_field_mdb_eedb_phase_e eedb_phase;

    int ll_valid;

    

    int ll_bank_idx;

    

    int ll_bank_row;

    

    int ll_row_offset;

    int data_valid;

    

    int data_bank_idx;

    

    int data_bank_row;

    

    int data_row_offset;

    

    dbal_enum_value_field_direct_payload_sizes_e payload_size_type;

} mdb_eedb_entry_info_t;





    

const mdb_physical_table_e mdb_eedb_phase_to_mdb_data_enum[DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES] =
    { MDB_PHYSICAL_TABLE_EEDB_1_DATA, MDB_PHYSICAL_TABLE_EEDB_2_DATA,
    MDB_PHYSICAL_TABLE_EEDB_3_DATA,
    MDB_PHYSICAL_TABLE_EEDB_4_DATA,
    MDB_PHYSICAL_TABLE_EEDB_5_DATA, MDB_PHYSICAL_TABLE_EEDB_6_DATA,
    MDB_PHYSICAL_TABLE_EEDB_7_DATA,
    MDB_PHYSICAL_TABLE_EEDB_8_DATA
};



shr_error_e
mdb_eedb_address_mapping_alloc(
    int unit)
{
    int alloc_size;

    SHR_FUNC_INIT_VARS(unit);

    

    alloc_size =
        (1 << dnx_data_mdb.direct.physical_address_max_bits_get(unit)) /
        dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows;
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_address_mapping.alloc(unit, alloc_size));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_eedb_address_mapping_set_val(
    int unit,
    mdb_eedb_mem_type_e eedb_mem_type,
    int logical_address_start,
    int logical_address_end,
    uint32 mdb_physical_table_id_val,
    uint32 cluster_index_val)
{
    int address_iter;
    int alloc_range_size;
    uint32 addr_map_val;

    SHR_FUNC_INIT_VARS(unit);

    alloc_range_size = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows;

    for (address_iter = logical_address_start / alloc_range_size; address_iter < logical_address_end / alloc_range_size;
         address_iter++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_address_mapping.get(unit, address_iter, &addr_map_val));

        if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
        {

            

            if ((mdb_physical_table_id_val != EEDB_ADDR_MAP_INVALID)
                && ((addr_map_val >> EEDB_ADDR_MAP_LL_TABLE_OFFSET) & EEDB_ADDR_MAP_MASK) != EEDB_ADDR_MAP_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Address 0x%x is already set for LL.\n", address_iter);
            }

            

            addr_map_val &=
                ~((EEDB_ADDR_MAP_MASK << EEDB_ADDR_MAP_LL_TABLE_OFFSET) |
                  (EEDB_ADDR_MAP_MASK << EEDB_ADDR_MAP_LL_CLUSTER_OFFSET));

            

            addr_map_val |=
                ((mdb_physical_table_id_val << EEDB_ADDR_MAP_LL_TABLE_OFFSET) |
                 (cluster_index_val << EEDB_ADDR_MAP_LL_CLUSTER_OFFSET));
        }
        else if ((eedb_mem_type == MDB_EEDB_MEM_TYPE_DATA_BANK) || (eedb_mem_type == MDB_EEDB_MEM_TYPE_DATA_CLUSTER))
        {

            

            if ((mdb_physical_table_id_val != EEDB_ADDR_MAP_INVALID)
                && (((addr_map_val >> EEDB_ADDR_MAP_DATA_TABLE_OFFSET) & EEDB_ADDR_MAP_MASK) != EEDB_ADDR_MAP_INVALID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Address 0x%x is already set for data.\n", address_iter);
            }

            

            addr_map_val &=
                ~(((uint32) EEDB_ADDR_MAP_MASK << EEDB_ADDR_MAP_DATA_TABLE_OFFSET) |
                  ((uint32) EEDB_ADDR_MAP_MASK << EEDB_ADDR_MAP_DATA_CLUSTER_OFFSET));

            

            addr_map_val |=
                ((mdb_physical_table_id_val << EEDB_ADDR_MAP_DATA_TABLE_OFFSET) |
                 (cluster_index_val << EEDB_ADDR_MAP_DATA_CLUSTER_OFFSET));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected eedb_mem_type: %d.\n", eedb_mem_type);
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_address_mapping.set(unit, address_iter, addr_map_val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_address_mapping_set(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int logical_address_start,
    int logical_address_end,
    int cluster_index)
{
    uint32 cluster_index_val = cluster_index;
    uint32 mdb_physical_table_id_val = mdb_physical_table_id;
    mdb_eedb_mem_type_e eedb_mem_type;

    SHR_FUNC_INIT_VARS(unit);

    if ((cluster_index_val > EEDB_ADDR_MAP_INVALID) || (mdb_physical_table_id_val > EEDB_ADDR_MAP_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid cluster index (%d) or physical table (%d).\n", cluster_index_val,
                     mdb_physical_table_id_val);
    }

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_verify(unit, logical_address_start, logical_address_end));

    eedb_mem_type = mdb_eedb_table_to_mem_type(unit, mdb_physical_table_id);

    SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_set_val
                    (unit, eedb_mem_type, logical_address_start, logical_address_end, mdb_physical_table_id_val,
                     cluster_index_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_address_mapping_unset(
    int unit,
    mdb_physical_table_e mdb_physical_table_id,
    int logical_address_start,
    int logical_address_end)
{
    mdb_eedb_mem_type_e eedb_mem_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_verify(unit, logical_address_start, logical_address_end));

    eedb_mem_type = mdb_eedb_table_to_mem_type(unit, mdb_physical_table_id);

    SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_set_val
                    (unit, eedb_mem_type, logical_address_start, logical_address_end, EEDB_ADDR_MAP_INVALID,
                     EEDB_ADDR_MAP_INVALID));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_address_mapping_get(
    int unit,
    int logical_address,
    mdb_physical_table_e * mdb_physical_table_id_data,
    int *cluster_index_data,
    mdb_physical_table_e * mdb_physical_table_id_ll,
    int *cluster_index_ll)
{
    int alloc_range_size;
    uint32 addr_map_val;

    SHR_FUNC_INIT_VARS(unit);

    alloc_range_size = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_EEDB_BANK)->nof_rows;
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                    eedb_address_mapping.get(unit, logical_address / alloc_range_size, &addr_map_val));

    if (addr_map_val == SAL_UINT32_MAX)
    {

        

        SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
        SHR_EXIT();
    }
    else
    {
        *mdb_physical_table_id_data = (addr_map_val >> EEDB_ADDR_MAP_DATA_TABLE_OFFSET) & EEDB_ADDR_MAP_MASK;
        *cluster_index_data = (addr_map_val >> EEDB_ADDR_MAP_DATA_CLUSTER_OFFSET) & EEDB_ADDR_MAP_MASK;
        *mdb_physical_table_id_ll = (addr_map_val >> EEDB_ADDR_MAP_LL_TABLE_OFFSET) & EEDB_ADDR_MAP_MASK;
        *cluster_index_ll = (addr_map_val >> EEDB_ADDR_MAP_LL_CLUSTER_OFFSET) & EEDB_ADDR_MAP_MASK;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_cluster_type_to_rows(
    int unit,
    mdb_macro_types_e macro_type,
    int *cluster_rows)
{
    SHR_FUNC_INIT_VARS(unit);

    *cluster_rows = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_rows;

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_calc_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *capacity)
{
    dbal_enum_value_field_mdb_eedb_phase_e phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
    int temp_capacity;
    int data_capacity = 0;
    int cluster_idx;
    mdb_cluster_info_t cluster_info;
    int sum_iteration;
    mdb_physical_table_e mdb_physical_table_id;
    int row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_clusters;

    SHR_FUNC_INIT_VARS(unit);

    if (phase_index >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. DBAL physical table is not an EEDB table: %s.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    

    for (sum_iteration = 0; sum_iteration < 2; sum_iteration++)
    {
        temp_capacity = 0;
        if (sum_iteration == 1)
        {
            mdb_physical_table_id =
                dnx_data_mdb.eedb.phase_to_table_get(unit, MDB_EEDB_MEM_TYPE_DATA_BANK,
                                                     phase_index)->mdb_physical_table_id;
        }
        else
        {
            mdb_physical_table_id =
                dnx_data_mdb.eedb.phase_to_table_get(unit, MDB_EEDB_MEM_TYPE_DATA_CLUSTER,
                                                     phase_index)->mdb_physical_table_id;
        }
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            uint32 cluster_payload_size;
            int cluster_rows;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, cluster_idx, &cluster_info));

            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase_index, &cluster_payload_size));

            SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, cluster_info.macro_type, &cluster_rows));

            

            temp_capacity += cluster_rows * row_width / MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(cluster_payload_size);
        }

        data_capacity += temp_capacity;
    }

    

    *capacity = data_capacity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_phase_bank_to_index(
    int unit,
    int eedb_bank_index,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    uint32 *phase_bank_index)
{
    uint32 phase_bank_index_temp = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

    SHR_FUNC_INIT_VARS(unit);

    

    if (eedb_bank_index < dnx_data_mdb.eedb.nof_eedb_banks_get(unit) / 4)
    {
        if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
        {
            phase_bank_index_temp = 0;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3)
        {
            phase_bank_index_temp = 1;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7)
        {
            phase_bank_index_temp = 2;
        }
    }
    else if (eedb_bank_index < dnx_data_mdb.eedb.nof_eedb_banks_get(unit) / 2)
    {
        if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
        {
            phase_bank_index_temp = 0;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3)
        {
            phase_bank_index_temp = 1;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8)
        {
            phase_bank_index_temp = 2;
        }
    }
    else if (eedb_bank_index < dnx_data_mdb.eedb.nof_eedb_banks_get(unit) * 3 / 4)
    {
        if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
        {
            phase_bank_index_temp = 0;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5)
        {
            phase_bank_index_temp = 1;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6)
        {
            phase_bank_index_temp = 2;
        }
    }
    else if (eedb_bank_index < dnx_data_mdb.eedb.nof_eedb_banks_get(unit) * 7 / 8)
    {
        if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
        {
            phase_bank_index_temp = 0;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5)
        {
            phase_bank_index_temp = 1;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4)
        {
            phase_bank_index_temp = 2;
        }
    }
    else if (eedb_bank_index < dnx_data_mdb.eedb.nof_eedb_banks_get(unit))
    {
        if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1)
        {
            phase_bank_index_temp = 0;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5)
        {
            phase_bank_index_temp = 1;
        }
        else if (phase == DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2)
        {
            phase_bank_index_temp = 2;
        }
    }

    *phase_bank_index = phase_bank_index_temp;

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_translate(
    int unit,
    mdb_eedb_mem_type_e eedb_mem_type,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    mdb_physical_table_e * mdb_physical_table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (eedb_mem_type >= MDB_NOF_EEDB_MEM_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected eedb_mem_type: %d.\n", eedb_mem_type);
    }

    if (phase >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected phase: %d.\n", phase);
    }

    *mdb_physical_table_id = dnx_data_mdb.eedb.phase_to_table_get(unit, eedb_mem_type, phase)->mdb_physical_table_id;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
mdb_eedb_get_cluster_map_info(
    int unit,
    uint32 logical_address,
    mdb_eedb_entry_info_t * eedb_entry_info)
{
    mdb_cluster_info_t cluster_info;
    int physical_address;
    int address_granularity;

    mdb_physical_table_e mdb_physical_table_id_data;
    int cluster_index_data;
    mdb_physical_table_e mdb_physical_table_id_ll;
    int cluster_index_ll;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(eedb_entry_info, 0x0, sizeof(mdb_eedb_entry_info_t));

    

    SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_get
                    (unit, logical_address, &mdb_physical_table_id_data, &cluster_index_data, &mdb_physical_table_id_ll,
                     &cluster_index_ll));

    

    if ((int) mdb_physical_table_id_ll != EEDB_ADDR_MAP_INVALID)
    {
        eedb_entry_info->eedb_phase = mdb_eedb_table_to_phase(unit, mdb_physical_table_id_ll);
    }
    else
    {
        eedb_entry_info->eedb_phase = mdb_eedb_table_to_phase(unit, mdb_physical_table_id_data);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.
                    eedb_payload_type.get(unit, eedb_entry_info->eedb_phase, &eedb_entry_info->payload_size_type));

    

    if ((int) mdb_physical_table_id_ll != EEDB_ADDR_MAP_INVALID)
    {
        eedb_entry_info->ll_valid = TRUE;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                        clusters_info.get(unit, mdb_physical_table_id_ll, cluster_index_ll, &cluster_info));

        

        address_granularity = MDB_DIRECT_BASIC_ENTRY_SIZE;
        physical_address = logical_address / (MDB_NOF_CLUSTER_ROW_BITS / address_granularity);

        eedb_entry_info->ll_bank_idx = cluster_info.cluster_index;
        eedb_entry_info->ll_bank_row = physical_address - cluster_info.start_address;

        

        eedb_entry_info->ll_row_offset =
            (logical_address -
             (physical_address * (MDB_NOF_CLUSTER_ROW_BITS / address_granularity))) * address_granularity;
    }
    else
    {
        eedb_entry_info->ll_valid = FALSE;
    }

    

    if ((int) mdb_physical_table_id_data != EEDB_ADDR_MAP_INVALID)
    {
        eedb_entry_info->data_valid = TRUE;

        

        address_granularity = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(eedb_entry_info->payload_size_type);
        physical_address = logical_address / (MDB_NOF_CLUSTER_ROW_BITS / address_granularity);

        

        if ((mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_1_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_2_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_3_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_4_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_5_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_6_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_7_DATA) ||
            (mdb_physical_table_id_data == MDB_PHYSICAL_TABLE_EEDB_8_DATA))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            clusters_info.get(unit, mdb_physical_table_id_data, cluster_index_data, &cluster_info));

            eedb_entry_info->data_bank_idx = cluster_info.cluster_index;
            eedb_entry_info->data_bank_row = physical_address - cluster_info.start_address;
        }
        else
        {
            eedb_entry_info->data_bank_idx = DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS;
        }

        

        eedb_entry_info->data_row_offset =
            (logical_address -
             (physical_address * (MDB_NOF_CLUSTER_ROW_BITS / address_granularity))) * address_granularity;
    }
    else
    {
        eedb_entry_info->data_valid = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

    

static shr_error_e
mdb_eedb_table_hitbit_get_and_clear(
    int unit,
    int eedb_bank_idx,
    int eedb_bank_row,
    int eedb_row_offset,
    uint16 *hitbit_flags)
{
    
    uint32 hitbit_row_value;
    uint32 hitbit_row_cache_value = 0;
    uint32 hitbit;
    uint32 row_in_hitbit_table = EEDB_DATA_ROW_TO_HIT_BIT_ROW(unit, eedb_bank_row);
    int hitbit_offset = EEDB_GET_HIT_BIT_ROW_OFFSET(unit, eedb_bank_row, eedb_row_offset);
    uint32 hitbit_mask = (1 << hitbit_offset);
    SHR_FUNC_INIT_VARS(unit);

    hitbit_row_value = 0;

    

    if (!dnx_data_mdb.eedb.feature_get(unit, dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear))
    {

        

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_hitbit.get(unit, eedb_bank_idx, row_in_hitbit_table,
                                                                             &hitbit_row_cache_value));
    }
    else
    {

        

        if (*hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_INDIRECT_WR_MASKr, 0x0, 0x0, hitbit_mask));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_INDIRECT_WR_MASKr, 0x0, 0x0, 0x0));
        }
    }

    SHR_IF_ERR_EXIT(soc_mem_array_read
                    (unit, dnx_data_mdb.eedb.abk_bank_get(unit), eedb_bank_idx, MDB_BLOCK(unit), row_in_hitbit_table,
                     &hitbit_row_value));

    hitbit_row_cache_value |= hitbit_row_value;
    hitbit = hitbit_row_cache_value & hitbit_mask;

    if (hitbit != 0)
    {
        *hitbit_flags |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
    }
    else
    {
        *hitbit_flags &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
    }

    if (!dnx_data_mdb.eedb.feature_get(unit, dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear))
    {
        if (*hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
        {

            

            hitbit_row_cache_value &= ~hitbit_mask;
        }

        

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_hitbit.set(unit, eedb_bank_idx, row_in_hitbit_table,
                                                                             hitbit_row_cache_value));
    }
    else
    {

        

        hitbit_mask = (1 << EEDB_HIT_BIT_NOF_ENTRIES_PER_ROW(unit)) - 1;
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, MDB_INDIRECT_WR_MASKr, 0x0, 0x0, hitbit_mask));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_entry_add(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; 
    int eedb_local_index = entry->key[0];
    mdb_eedb_entry_info_t eedb_entry_info;
    int entry_max_size;
    int ll_entry_size = 0;
    bsl_severity_t severity;
    uint8 cache_enabled;
    uint32 flags_as_arr[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);


    flags_as_arr[0] = flags;


    

    SHR_IF_ERR_EXIT(mdb_eedb_get_cluster_map_info(unit, eedb_local_index, &eedb_entry_info));

    

    if (!SHR_IS_BITSET(flags_as_arr, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS))
    {
        eedb_entry_info.ll_valid = FALSE;
    }

    

    if ((eedb_entry_info.ll_valid == TRUE) && (eedb_entry_info.data_valid == TRUE))
    {

        

        entry_max_size =
            MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B) +
            MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset;
    }
    else if (eedb_entry_info.ll_valid == TRUE)
    {

        

        entry_max_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B);
    }
    else if (eedb_entry_info.data_valid == TRUE)
    {
        if ((SHR_IS_BITSET(flags_as_arr, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS)) == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. Entry key: %d, is indicated to use LL, but has no LL HW associated with it.\n",
                         eedb_local_index);
        }
        entry_max_size = MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Entry key: %d, is not associated with any HW resources.\n",
                     eedb_local_index);
    }

    if (entry_max_size < entry->payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. entry->payload_size:%d does not fit in entry 0x%x, max size for this entry:%d.\n",
                     entry->payload_size, eedb_local_index, entry_max_size);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));

    if (eedb_entry_info.ll_valid == TRUE)
    {
        uint32 row_data[MDB_ENTRY_DATA_SIZE_IN_UINT32];

        ll_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B);

        

        ll_entry_size = ll_entry_size > entry->payload_size ? entry->payload_size : ll_entry_size;

        sal_memset(row_data, 0, sizeof(row_data));

        

        if (cache_enabled == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_direct_cache_get
                            (unit, MDB_EEDB_BANK, 0, eedb_entry_info.ll_bank_idx, eedb_entry_info.ll_bank_row, 0,
                             row_data));
        }
        else
        {
            MDB_EEDB_ACCESS_UNLOCK(unit, soc_mem_array_read
                                   (unit, dnx_data_mdb.eedb.entry_bank_get(unit), eedb_entry_info.ll_bank_idx,
                                    MEM_BLOCK_ANY, eedb_entry_info.ll_bank_row, row_data));
        }

        SHR_BITCOPY_RANGE(row_data, (MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.ll_row_offset) - ll_entry_size,
                          entry->payload, 0, ll_entry_size);

        

        if ((dnx_sbusdma_desc_is_enabled(unit, SBUSDMA_DESC_MODULE_ENABLE_MDB_EEDB) == TRUE) && (cache_enabled == TRUE))
        {
            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem
                            (unit, dnx_data_mdb.eedb.entry_bank_get(unit), eedb_entry_info.ll_bank_idx, MDB_BLOCK(unit),
                             eedb_entry_info.ll_bank_row, row_data));
        }
        else
        {
            MDB_EEDB_ACCESS_UNLOCK(unit, soc_mem_array_write
                                   (unit, dnx_data_mdb.eedb.entry_bank_get(unit), eedb_entry_info.ll_bank_idx,
                                    MDB_BLOCK(unit), eedb_entry_info.ll_bank_row, row_data));
        }
        if (cache_enabled == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_direct_cache_set
                            (unit, MDB_EEDB_BANK, 0, eedb_entry_info.ll_bank_idx, eedb_entry_info.ll_bank_row, 0,
                             row_data));
        }
    }

    if (eedb_entry_info.data_valid == TRUE)
    {
        int entry_size = entry->payload_size - ll_entry_size;

        

        if (entry_size > 0)
        {
            if (eedb_entry_info.data_bank_idx != DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS)
            {
                uint32 row_data[MDB_ENTRY_DATA_SIZE_IN_UINT32];
                dbal_enum_value_field_direct_payload_sizes_e payload_type;
                uint32 table_entry_size;

                

                uint32 payload_size_rounded_up;
                uint32 payload_size_rounded_up_diff;

                sal_memset(row_data, 0, sizeof(row_data));

                SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, eedb_entry_info.eedb_phase, &payload_type));
                table_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);
                payload_size_rounded_up = UTILEX_DIV_ROUND_UP(entry_size, table_entry_size) * table_entry_size;
                payload_size_rounded_up_diff = payload_size_rounded_up - entry_size;

                if (payload_size_rounded_up != MDB_NOF_CLUSTER_ROW_BITS)
                {

                    

                    if (cache_enabled == TRUE)
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                        (unit, MDB_EEDB_BANK, 0, eedb_entry_info.data_bank_idx,
                                         eedb_entry_info.data_bank_row, 0, row_data));
                    }
                    else
                    {
                        MDB_EEDB_ACCESS_UNLOCK(unit, soc_mem_array_read
                                               (unit, dnx_data_mdb.eedb.entry_bank_get(unit),
                                                eedb_entry_info.data_bank_idx, MEM_BLOCK_ANY,
                                                eedb_entry_info.data_bank_row, row_data));
                    }
                }

                SHR_BITCOPY_RANGE(row_data, (MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset) - entry_size,
                                  entry->payload, ll_entry_size, entry_size);

                

                if (payload_size_rounded_up_diff != 0)
                {
                    SHR_BITCLR_RANGE(row_data,
                                     (MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset) - entry_size -
                                     payload_size_rounded_up_diff, payload_size_rounded_up_diff);
                }

                

                if ((dnx_sbusdma_desc_is_enabled(unit, SBUSDMA_DESC_MODULE_ENABLE_MDB_EEDB) == TRUE)
                    && (cache_enabled == TRUE))
                {
                    SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem
                                    (unit, dnx_data_mdb.eedb.entry_bank_get(unit), eedb_entry_info.data_bank_idx,
                                     MDB_BLOCK(unit), eedb_entry_info.data_bank_row, row_data));
                }
                else
                {
                    MDB_EEDB_ACCESS_UNLOCK(unit, soc_mem_array_write
                                           (unit, dnx_data_mdb.eedb.entry_bank_get(unit),
                                            eedb_entry_info.data_bank_idx, MDB_BLOCK(unit),
                                            eedb_entry_info.data_bank_row, row_data));
                }
                if (cache_enabled == TRUE)
                {
                    SHR_IF_ERR_EXIT(mdb_direct_cache_set
                                    (unit, MDB_EEDB_BANK, 0, eedb_entry_info.data_bank_idx,
                                     eedb_entry_info.data_bank_row, 0, row_data));
                }
            }
            else
            {

                

                dbal_physical_entry_t data_entry;
                dbal_physical_tables_e direct_physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };

                sal_memset(&data_entry, 0x0, sizeof(data_entry));
                data_entry.key[0] = entry->key[0];

                

                SHR_BITCOPY_RANGE(data_entry.payload, 0, entry->payload, ll_entry_size, entry_size);
                data_entry.payload_size = entry_size;

                direct_physical_tables[0] = DBAL_PHYSICAL_TABLE_EEDB_1 + eedb_entry_info.eedb_phase;
                SHR_IF_ERR_EXIT(mdb_direct_table_entry_add(unit, direct_physical_tables, app_id, &data_entry, 0));
            }
        }
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_eedb_write
                        (unit, dbal_physical_table_id, app_id, eedb_entry_info.ll_valid, entry));

    }
#endif

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if ((severity >= bslSeverityVerbose)
        )
    {
        int row_offset;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_eedb_table_entry_add: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                "entry.key: %d. entry.payload_size: %d, physical_table: %s, phase: %d.\n"),
                                     entry->key[0], entry->payload_size,
                                     dbal_physical_table_to_string(unit, dbal_physical_table_id),
                                     eedb_entry_info.eedb_phase));
        for (row_offset = 0; row_offset < BITS2WORDS(entry->payload_size); row_offset++)
        {
            uint32 print_index = BITS2WORDS(entry->payload_size) - 1 - row_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry.payload[%d]: 0x%08x.\n"), print_index, entry->payload[print_index]));
        }

        if (eedb_entry_info.ll_valid == TRUE)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "eedb_entry_info.ll_bank_idx: %d. eedb_entry_info.ll_bank_row: %d. eedb_entry_info.ll_row_offset: %d.\n"),
                         eedb_entry_info.ll_bank_idx, eedb_entry_info.ll_bank_row, eedb_entry_info.ll_row_offset));
        }

        if (eedb_entry_info.data_valid == TRUE)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "eedb_entry_info.data_bank_idx: %d. eedb_entry_info.data_bank_row: %d. eedb_entry_info.data_row_offset: %d.\n"),
                         eedb_entry_info.data_bank_idx, eedb_entry_info.data_bank_row,
                         eedb_entry_info.data_row_offset));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "payload_size_type: %d.\n"), eedb_entry_info.payload_size_type));
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_eedb_table_entry_add: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    int eedb_local_index = entry->key[0];
    mdb_eedb_entry_info_t eedb_entry_info;
    int entry_max_size;
    int ll_entry_size = 0;
    bsl_severity_t severity;
    uint8 cache_enabled;
    uint32 row_data[MDB_ENTRY_DATA_SIZE_IN_UINT32];
    uint32 flags_as_arr[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_eedb_read(unit, dbal_physical_table_id, app_id, entry));
    }
    else
#endif
    {
        flags_as_arr[0] = flags;

        
        SHR_IF_ERR_EXIT(mdb_eedb_get_cluster_map_info(unit, eedb_local_index, &eedb_entry_info));

        
        if (!SHR_IS_BITSET(flags_as_arr, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS))
        {
            eedb_entry_info.ll_valid = FALSE;
        }

        
        if ((eedb_entry_info.ll_valid == TRUE) && (eedb_entry_info.data_valid == TRUE))
        {
            
            entry_max_size =
                MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B) +
                MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset;
        }
        else if (eedb_entry_info.ll_valid == TRUE)
        {
            
            entry_max_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B);
        }
        else if (eedb_entry_info.data_valid == TRUE)
        {
            if (SHR_IS_BITSET(flags_as_arr, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Error. Entry key: %d, is indicated to use LL, but has no LL HW associated with it.\n",
                             eedb_local_index);
            }
            entry_max_size = MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Entry key: %d, is not associated with any HW resources.\n",
                         eedb_local_index);
        }

        if (entry_max_size < entry->payload_size)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. entry->payload_size:%d does not fit in entry 0x%x, max size for this entry:%d.\n",
                         entry->payload_size, eedb_local_index, entry_max_size);
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));
        cache_enabled &= ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL) ||
                          (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW));

        if (eedb_entry_info.ll_valid == TRUE)
        {
            ll_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B);

            
            ll_entry_size = ll_entry_size > entry->payload_size ? entry->payload_size : ll_entry_size;

            if (cache_enabled == TRUE)
            {
                SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                (unit, MDB_EEDB_BANK, 0, eedb_entry_info.ll_bank_idx, eedb_entry_info.ll_bank_row, 0,
                                 row_data));
            }
            else
            {
                MDB_EEDB_ACCESS_UNLOCK(unit, soc_mem_array_read
                                       (unit, dnx_data_mdb.eedb.entry_bank_get(unit), eedb_entry_info.ll_bank_idx,
                                        MEM_BLOCK_ANY, eedb_entry_info.ll_bank_row, row_data));
            }

            SHR_BITCOPY_RANGE(entry->payload, 0, row_data,
                              (MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.ll_row_offset) - ll_entry_size,
                              ll_entry_size);

        }

        if (eedb_entry_info.data_valid == TRUE)
        {
            int entry_size = entry->payload_size - ll_entry_size;

            

            if (entry_size > 0)
            {
                if (eedb_entry_info.data_bank_idx != DNX_DATA_MAX_MDB_EEDB_NOF_EEDB_BANKS)
                {
                    if (cache_enabled == TRUE)
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                        (unit, MDB_EEDB_BANK, 0, eedb_entry_info.data_bank_idx,
                                         eedb_entry_info.data_bank_row, 0, row_data));
                    }
                    else
                    {
                        MDB_EEDB_ACCESS_UNLOCK(unit, soc_mem_array_read
                                               (unit, dnx_data_mdb.eedb.entry_bank_get(unit),
                                                eedb_entry_info.data_bank_idx, MEM_BLOCK_ANY,
                                                eedb_entry_info.data_bank_row, row_data));
                    }

                    SHR_BITCOPY_RANGE(entry->payload, ll_entry_size, row_data,
                                      (MDB_NOF_CLUSTER_ROW_BITS - eedb_entry_info.data_row_offset) - entry_size,
                                      entry_size);

                    if (_SHR_IS_FLAG_SET(entry->hitbit, DBAL_PHYSICAL_KEY_HITBIT_ACTION))
                    {
                        SHR_IF_ERR_EXIT(mdb_eedb_table_hitbit_get_and_clear
                                        (unit, eedb_entry_info.data_bank_idx, eedb_entry_info.data_bank_row,
                                         eedb_entry_info.data_row_offset, &entry->hitbit));
                    }
                }
                else
                {

                    

                    dbal_physical_entry_t data_entry;

                    sal_memcpy(&data_entry, entry, sizeof(data_entry));
                    data_entry.payload_size = entry_size;

                    SHR_IF_ERR_EXIT(mdb_direct_table_entry_get
                                    (unit, DBAL_PHYSICAL_TABLE_EEDB_1 + eedb_entry_info.eedb_phase, app_id, &data_entry,
                                     0));

                    

                    SHR_BITCOPY_RANGE(entry->payload, ll_entry_size, data_entry.payload, 0, entry_size);
                    entry->hitbit = data_entry.hitbit;
                }
            }
        }
    }
    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        int row_offset;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_eedb_table_entry_get: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "entry.key: %d. entry.payload_size: %d, physical_table: %s, phase: %d.\n"),
                     entry->key[0], entry->payload_size, dbal_physical_table_to_string(unit, dbal_physical_table_id),
                     eedb_entry_info.eedb_phase));
        for (row_offset = 0; row_offset < BITS2WORDS(entry->payload_size); row_offset++)
        {
            uint32 print_index = BITS2WORDS(entry->payload_size) - 1 - row_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry.payload[%d]: 0x%08x.\n"), print_index, entry->payload[print_index]));
        }

        if (eedb_entry_info.ll_valid == TRUE)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "eedb_entry_info.ll_bank_idx: %d. eedb_entry_info.ll_bank_row: %d. eedb_entry_info.ll_row_offset: %d.\n"),
                         eedb_entry_info.ll_bank_idx, eedb_entry_info.ll_bank_row, eedb_entry_info.ll_row_offset));
        }

        if (eedb_entry_info.data_valid == TRUE)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "eedb_entry_info.data_bank_idx: %d. eedb_entry_info.data_bank_row: %d. eedb_entry_info.data_row_offset: %d.\n"),
                         eedb_entry_info.data_bank_idx, eedb_entry_info.data_bank_row,
                         eedb_entry_info.data_row_offset));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "payload_size_type: %d.\n"), eedb_entry_info.payload_size_type));
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_eedb_table_entry_get: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_entry_delete(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_physical_entry_t temp_entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&temp_entry, entry, sizeof(dbal_physical_entry_t));

    sal_memset(temp_entry.payload, 0x0, sizeof(temp_entry.payload));

    SHR_IF_ERR_EXIT(mdb_eedb_table_entry_add(unit, physical_tables, app_id, &temp_entry, flags));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int full_time)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_entry_t entry;
    uint32 row_data_write[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    uint32 row_data_read[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    int entry_counter;
    int uint32_counter;
    dbal_enum_value_field_mdb_eedb_phase_e phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
    mdb_physical_table_e ll_table, data_table, data_macro_table, mdb_physical_table_id;
    int table_iter;
    dbal_enum_value_field_direct_payload_sizes_e entry_payload_type, table_payload_type;
    uint32 entry_size;
    uint32 entry_index_size;
    int index_factor;
    int row_width;
    int nof_clusters, start_address, end_address;
    uint32 add_total_time_usecs = 0, get_total_time_usecs = 0, del_total_time_usecs = 0, get_post_total_time_usecs = 0;
    uint32 total_entry_count = 0;
    uint32 timer_usec = 0;
    int op_iter;
    uint32 entry_flags[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    sal_srand(55555);

    if ((dbal_physical_table_id < DBAL_PHYSICAL_TABLE_EEDB_1) || (dbal_physical_table_id > DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. DBAL physical table %d is not associated with an MDB EEDB table.\n",
                     dbal_physical_table_id);
    }

    physical_tables[0] = dbal_physical_table_id;

    sal_memset(row_data_write, 0x0, (MDB_ENTRY_DATA_SIZE_IN_UINT32) * sizeof(row_data_write[0]));
    sal_memset(row_data_read, 0x0, (MDB_ENTRY_DATA_SIZE_IN_UINT32) * sizeof(row_data_read[0]));

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, &ll_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_BANK, phase_index, &data_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, phase_index, &data_macro_table));

    entry_counter = 0;

    

    for (op_iter = 0; op_iter < 4; op_iter++)
    {

        

        sal_srand(55555);
        table_iter = 0;
#ifdef ADAPTER_SERVER_MODE
        if (dnx_data_mdb.global.adapter_stub_enable_get(unit))
        {
            table_iter = 1;
        }
#endif
        for (; table_iter < 3; table_iter++)
        {
            int cluster_index = 0;
            int entry_index;
            int entries_per_row;
            int cluster_entry_size;

            if (table_iter == 0)
            {
                mdb_physical_table_id = ll_table;

                

                entry_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
            }
            else if (table_iter == 1)
            {
                mdb_physical_table_id = data_table;
                entry_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
            }
            else
            {
                mdb_physical_table_id = data_macro_table;
                entry_payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_120B;
            }

            entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(entry_payload_type);

            SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id, &table_payload_type));

            entry_index_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(table_payload_type);

            

            index_factor =
                ((UTILEX_DIV_ROUND_UP(MDB_NOF_CLUSTER_ROW_BITS, MDB_DIRECT_BASIC_ENTRY_SIZE)) *
                 MDB_DIRECT_BASIC_ENTRY_SIZE) / entry_index_size;

            if (table_iter == 0)
            {

                

                cluster_entry_size = MDB_DIRECT_BASIC_ENTRY_SIZE;
                row_width = MDB_NOF_CLUSTER_ROW_BITS;
            }
            else
            {
                cluster_entry_size = entry_index_size;
                row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;
            }

            entries_per_row =
                row_width / ((UTILEX_DIV_ROUND_UP(cluster_entry_size, MDB_DIRECT_BASIC_ENTRY_SIZE)) *
                             MDB_DIRECT_BASIC_ENTRY_SIZE);

            if (full_time == TRUE)
            {
                timer_usec = sal_time_usecs();
            }

            

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

            while (cluster_index < nof_clusters)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                start_address.get(unit, mdb_physical_table_id, cluster_index, &start_address));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                end_address.get(unit, mdb_physical_table_id, cluster_index, &end_address));

                entry_index = start_address * entries_per_row;

                while (entry_index < end_address * entries_per_row)
                {

                    

                    for (uint32_counter = 0; uint32_counter < (UTILEX_DIV_ROUND_UP(entry_size, SAL_UINT32_NOF_BITS));
                         uint32_counter++)
                    {

                        

                        row_data_write[uint32_counter] =
                            (sal_rand() & 0x7FFF) | ((sal_rand() & 0x7FFF) << 15) | ((sal_rand() & 0x3) << 30);
                    }

                    sal_memset(&entry, 0x0, sizeof(entry));

                    entry.key[0] = entry_index;
                    entry.payload_size = entry_size;
                    if (table_iter == 0)
                    {

                        

                        SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
                    }
                    else
                    {
                        entry_flags[0] = 0;
                    }

                    

                    if (op_iter == 0)
                    {
                        SHR_BITCOPY_RANGE(entry.payload, 0, row_data_write, 0, entry.payload_size);

                        if (full_time == FALSE)
                        {
                            timer_usec = sal_time_usecs();
                        }
                        SHR_IF_ERR_EXIT(mdb_eedb_table_entry_add
                                        (unit, physical_tables, 0  , &entry, entry_flags[0]));
                        if (full_time == FALSE)
                        {
                            add_total_time_usecs += sal_time_usecs() - timer_usec;
                        }
                        total_entry_count++;
                    }

                    

                    if (op_iter == 1)
                    {
                        entry.mdb_action_apply = DBAL_MDB_ACTION_APPLY_HW_ONLY;
                        if (full_time == FALSE)
                        {
                            timer_usec = sal_time_usecs();
                        }
                        SHR_IF_ERR_EXIT(mdb_eedb_table_entry_get
                                        (unit, dbal_physical_table_id, 0  , &entry, entry_flags[0]));
                        if (full_time == FALSE)
                        {
                            get_total_time_usecs += sal_time_usecs() - timer_usec;
                        }

                        

                        sal_memcpy(row_data_read, entry.payload,
                                   (MDB_MAX_DIRECT_PAYLOAD_SIZE_32) * sizeof(row_data_read[0]));

                        

                        SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                        (entry.payload, row_data_write, MDB_MAX_DIRECT_PAYLOAD_SIZE_32));

                        if (utilex_bitstream_have_one_in_range
                            (entry.payload, 0  , entry.payload_size - 1))
                        {
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                            for (uint32_counter = MDB_MAX_DIRECT_PAYLOAD_SIZE_32 - 1; uint32_counter >= 0;
                                 uint32_counter--)
                            {
                                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), row_data_write[uint32_counter]));
                            }
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                            for (uint32_counter = MDB_MAX_DIRECT_PAYLOAD_SIZE_32 - 1; uint32_counter >= 0;
                                 uint32_counter--)
                            {
                                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), row_data_read[uint32_counter]));
                            }
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload size: %d\n"), entry.payload_size));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_counter: %d\n"), entry_counter));

                            SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, read data is not equal to written data.\n");
                        }
                    }

                    

                    if (op_iter == 2)
                    {
                        if (full_time == FALSE)
                        {
                            timer_usec = sal_time_usecs();
                        }
                        SHR_IF_ERR_EXIT(mdb_eedb_table_entry_delete
                                        (unit, physical_tables, 0  , &entry, entry_flags[0]));
                        if (full_time == FALSE)
                        {
                            del_total_time_usecs += sal_time_usecs() - timer_usec;
                        }
                    }

                    

                    if (op_iter == 3)
                    {
                        if (full_time == FALSE)
                        {
                            timer_usec = sal_time_usecs();
                        }
                        SHR_IF_ERR_EXIT(mdb_eedb_table_entry_get
                                        (unit, dbal_physical_table_id, 0  , &entry, entry_flags[0]));
                        if (full_time == FALSE)
                        {
                            del_total_time_usecs += sal_time_usecs() - timer_usec;
                        }

                        if (utilex_bitstream_have_one_in_range
                            (entry.payload, 0  , entry.payload_size - 1))
                        {
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload size: %d\n"), entry.payload_size));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_counter: %d\n"), entry_counter));

                            SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, data different from 0 after delete.\n");
                        }
                    }

                    if (mode == MDB_TEST_CAPACITY)
                    {
                        entry_index += index_factor;
                    }
                    else
                    {
                        entry_index += (1 + (sal_rand() % MDB_TEST_BRIEF_FACTOR)) * index_factor;
                    }
                    entry_counter++;
                }
                cluster_index += row_width / MDB_NOF_CLUSTER_ROW_BITS;
            }

            SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));
            if (full_time == TRUE)
            {
                if (op_iter == 0)
                {
                    add_total_time_usecs += sal_time_usecs() - timer_usec;
                }
                else if (op_iter == 1)
                {
                    get_total_time_usecs += sal_time_usecs() - timer_usec;
                }
                else if (op_iter == 2)
                {
                    del_total_time_usecs += sal_time_usecs() - timer_usec;
                }
                else if (op_iter == 3)
                {
                    get_post_total_time_usecs += sal_time_usecs() - timer_usec;
                }
            }
        }
    }

    if (total_entry_count != 0)
    {
        
        int entry_print_factor = 10;
        int total_entry_count_factored = total_entry_count / entry_print_factor;

        total_entry_count_factored = total_entry_count_factored ? total_entry_count_factored : 1;

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U
                  (unit,
                   "In total %d entries were added, full time measurement = %d,\nentry batch = %d\navg usecs per entry batch add: %d\navg usecs per entry batch HW get: %d\navg usecs per entry batch delete: %d\navg usecs per entry batch SW get after delete: %d\n"),
                  total_entry_count == 1 ? 0 : total_entry_count, full_time, entry_print_factor,
                  add_total_time_usecs / total_entry_count_factored, get_total_time_usecs / total_entry_count_factored,
                  del_total_time_usecs / total_entry_count_factored,
                  get_post_total_time_usecs / total_entry_count_factored));
    }
    else
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "In total 0 entries were added\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    dbal_enum_value_field_mdb_eedb_phase_e phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
    mdb_physical_table_e data_bank_table_id, data_cluster_table_id;
    int row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_banks, nof_clusters;
    int start_address;
    dbal_enum_value_field_direct_payload_sizes_e payload_type;

    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_BANK, phase_index, &data_bank_table_id));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, phase_index, &data_cluster_table_id));
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_bank_table_id, &nof_banks));
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_cluster_table_id, &nof_clusters));
    if ((nof_banks == 0) && (nof_clusters == 0))
    {
        start_address = 0;
    }
    else if (nof_banks > 0)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.start_address.get(unit, data_bank_table_id, 0, &start_address));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        start_address.get(unit, data_cluster_table_id, 0, &start_address));
    }

    physical_entry_iterator->mdb_cluster_index = 0;

    SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id, &payload_type));

    physical_entry_iterator->payload_basic_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);
    if (start_address == MDB_DIRECT_INVALID_START_ADDRESS)
    {
        physical_entry_iterator->mdb_entry_index = MDB_DIRECT_INVALID_START_ADDRESS;
    }
    else
    {
        physical_entry_iterator->mdb_entry_index =
            start_address * row_width / physical_entry_iterator->payload_basic_size;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    dbal_enum_value_field_mdb_eedb_phase_e phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
    mdb_physical_table_e ll_bank_table_id;
    mdb_physical_table_e data_bank_table_id, data_cluster_table_id;
    int row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_banks, nof_clusters, start_address, end_address;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(*entry));

    

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_BANK, phase_index, &data_bank_table_id));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, &ll_bank_table_id));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_DATA_CLUSTER, phase_index, &data_cluster_table_id));
    while ((*is_end == FALSE) && (!utilex_bitstream_have_one(entry->payload, BITS2WORDS(entry->payload_size))))
    {
        sal_memset(entry, 0x0, sizeof(*entry));

        

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_bank_table_id, &nof_banks));
        if (physical_entry_iterator->mdb_cluster_index < nof_banks)
        {

            

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.end_address.get(unit, data_bank_table_id,
                                                                          physical_entry_iterator->mdb_cluster_index,
                                                                          &end_address));
            if ((physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size / row_width) >=
                end_address)
            {
                physical_entry_iterator->mdb_cluster_index++;

                

                if (!(physical_entry_iterator->mdb_cluster_index >= nof_banks))
                {
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.start_address.get(unit, data_bank_table_id,
                                                                                    physical_entry_iterator->mdb_cluster_index,
                                                                                    &start_address));
                    physical_entry_iterator->mdb_entry_index =
                        start_address * row_width / physical_entry_iterator->payload_basic_size;
                }
            }
        }

        

        if (physical_entry_iterator->mdb_cluster_index >= nof_banks)
        {
            int local_cluster_idx = physical_entry_iterator->mdb_cluster_index - nof_banks;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_cluster_table_id, &nof_clusters));

            

            if (nof_clusters == 0)
            {
                *is_end = TRUE;
            }

            

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            end_address.get(unit, data_cluster_table_id, local_cluster_idx, &end_address));
            while ((*is_end == FALSE)
                   && ((end_address == MDB_DIRECT_INVALID_START_ADDRESS)
                       || (physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size /
                           row_width) >= end_address))
            {
                physical_entry_iterator->mdb_cluster_index++;
                local_cluster_idx = physical_entry_iterator->mdb_cluster_index - nof_banks;

                if (local_cluster_idx >= nof_clusters)
                {

                    

                    *is_end = TRUE;
                }
                else
                {
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    end_address.get(unit, data_cluster_table_id, local_cluster_idx, &end_address));

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    start_address.get(unit, data_cluster_table_id, local_cluster_idx, &start_address));

                    physical_entry_iterator->mdb_entry_index =
                        start_address * row_width / physical_entry_iterator->payload_basic_size;
                }
            }
        }

        if (*is_end == FALSE)
        {
            int ll_cluster_index;
            uint32 entry_flags[1] = { 0 };

            entry->key[0] = physical_entry_iterator->mdb_entry_index;
            entry->payload_size = physical_entry_iterator->payload_basic_size;
            entry->hitbit = physical_entry_iterator->hit_bit_flags;
            entry->mdb_action_apply = physical_entry_iterator->mdb_action_apply;

            

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, ll_bank_table_id, &nof_banks));
            for (ll_cluster_index = 0; ll_cluster_index < nof_banks; ll_cluster_index++)
            {
                int entry_index_factor = MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE;
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                start_address.get(unit, ll_bank_table_id, ll_cluster_index, &start_address));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                end_address.get(unit, ll_bank_table_id, ll_cluster_index, &end_address));
                if ((start_address * entry_index_factor <= entry->key[0])
                    && (end_address * entry_index_factor > entry->key[0]))
                {
                    entry->payload_size += MDB_DIRECT_BASIC_ENTRY_SIZE;
                    SHR_BITSET(entry_flags, DBAL_MDB_ENTRY_FLAGS_EEDB_LL_EXISTS);
                    break;
                }
            }

            SHR_IF_ERR_EXIT(mdb_eedb_table_entry_get(unit, dbal_physical_table_id, app_id, entry, entry_flags[0]));
            physical_entry_iterator->mdb_entry_index++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_clear(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0]; 
    dbal_physical_entry_iterator_t physical_entry_iterator;
    uint8 is_end = FALSE;
    dbal_physical_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&physical_entry_iterator, 0, sizeof(physical_entry_iterator));
    SHR_IF_ERR_EXIT(mdb_eedb_table_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

    SHR_IF_ERR_EXIT(mdb_eedb_table_iterator_get_next
                    (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));
    while (is_end == FALSE)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_entry_delete(unit, physical_tables, app_id, &entry, 0));

        SHR_IF_ERR_EXIT(mdb_eedb_table_iterator_get_next
                        (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));
    }

exit:
    SHR_IF_ERR_CONT(mdb_eedb_table_iterator_deinit(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_data_granularity_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 *address_granularity)
{
    uint32 payload_type;
    SHR_FUNC_INIT_VARS(unit);

    if ((dbal_physical_table_id < DBAL_PHYSICAL_TABLE_EEDB_1) || (dbal_physical_table_id > DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        *address_granularity = UTILEX_U32_MAX;
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. DBAL table %s is not an EEDB data table.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.
                    eedb_payload_type.get(unit, dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1, &payload_type));

    *address_granularity = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

exit:
    SHR_FUNC_EXIT;
}

dbal_enum_value_field_mdb_eedb_phase_e
mdb_eedb_table_to_phase(
    int unit,
    mdb_physical_table_e mdb_physical_table_id)
{
    dbal_enum_value_field_mdb_eedb_phase_e phase_index;

    if (dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        
        phase_index = dnx_data_mdb.pdbs.mdb_pdb_info_get(unit, mdb_physical_table_id)->db_subtype;
    }
    else
    {
        phase_index = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
    }

    return phase_index;
}

mdb_eedb_mem_type_e
mdb_eedb_table_to_mem_type(
    int unit,
    mdb_physical_table_e mdb_physical_table_id)
{
    mdb_eedb_mem_type_e eedb_mem_type;
    switch (mdb_physical_table_id)
    {
        case MDB_PHYSICAL_TABLE_EEDB_1_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_2_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_3_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_4_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_5_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_6_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_7_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_8_DATA:
            eedb_mem_type = MDB_EEDB_MEM_TYPE_DATA_BANK;
            break;
#if defined(BCM_DNX2_SUPPORT)
        case MDB_PHYSICAL_TABLE_EEDB_1_2_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_3_4_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_5_6_DATA:
        case MDB_PHYSICAL_TABLE_EEDB_7_8_DATA:
#endif
            eedb_mem_type = MDB_EEDB_MEM_TYPE_DATA_CLUSTER;
            break;
        case MDB_PHYSICAL_TABLE_EEDB_1_LL:
        case MDB_PHYSICAL_TABLE_EEDB_2_LL:
        case MDB_PHYSICAL_TABLE_EEDB_3_LL:
        case MDB_PHYSICAL_TABLE_EEDB_4_LL:
        case MDB_PHYSICAL_TABLE_EEDB_5_LL:
        case MDB_PHYSICAL_TABLE_EEDB_6_LL:
        case MDB_PHYSICAL_TABLE_EEDB_7_LL:
        case MDB_PHYSICAL_TABLE_EEDB_8_LL:
            eedb_mem_type = MDB_EEDB_MEM_TYPE_LL;
            break;
        default:
            eedb_mem_type = MDB_NOF_EEDB_MEM_TYPES;
    }

    return eedb_mem_type;
}

shr_error_e
mdb_eedb_table_nof_entries_per_cluster_type_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_macro_types_e macro_type,
    uint32 *nof_entries)
{
    uint32 address_granularity, macro_row_width;
    int nof_rows_in_cluster;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_eedb_table_data_granularity_get(unit, dbal_physical_table_id, &address_granularity));

    SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, macro_type, &nof_rows_in_cluster));

    macro_row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;

    *nof_entries = nof_rows_in_cluster * macro_row_width / address_granularity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_hitbit_init(
    int unit)
{
    int nof_eedb_hitbit_banks, nof_hitbit_rows;
    int eedb_hitbit_bank, allocation_size;
    uint32 *allocated_mem = NULL;
    soc_mem_t abk_bank;
    int use_dma =
#ifdef PLISIM
        !SAL_BOOT_PLISIM &&
#endif
        soc_mem_dmaable(unit, dnx_data_mdb.eedb.abk_bank_get(unit), MDB_BLOCK(unit));
    SHR_FUNC_INIT_VARS(unit);

    

    if (!dnx_data_mdb.eedb.feature_get(unit, dnx_data_mdb_eedb_eedb_bank_hitbit_masked_clear))
    {
        abk_bank = dnx_data_mdb.eedb.abk_bank_get(unit);

        

        nof_eedb_hitbit_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
        nof_hitbit_rows =
            dnx_data_mdb.dh.macro_type_info_get(unit,
                                                MDB_EEDB_BANK)->nof_rows /
            dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get(unit);

        for (eedb_hitbit_bank = 0; eedb_hitbit_bank < nof_eedb_hitbit_banks; eedb_hitbit_bank++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_hitbit.alloc(unit, eedb_hitbit_bank));
        }

        

        allocation_size = sizeof(uint32) * nof_hitbit_rows;
        if (use_dma)
        {
            allocated_mem = soc_cm_salloc(unit, allocation_size, "dma-eedb-abk-buffer");
        }
        else
        {
            SHR_ALLOC(allocated_mem, allocation_size, "eedb-abk-buffer", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        }

        if (allocated_mem == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate DMA buffer");
        }

        for (eedb_hitbit_bank = 0; eedb_hitbit_bank < nof_eedb_hitbit_banks; eedb_hitbit_bank++)
        {
            SHR_IF_ERR_EXIT(soc_mem_array_read_range
                            (unit, abk_bank, eedb_hitbit_bank, MDB_BLOCK(unit), 0, nof_hitbit_rows - 1, allocated_mem));
        }
    }

exit:
    if (allocated_mem != NULL)
    {
        if (use_dma)
        {
            soc_cm_sfree(unit, allocated_mem);
        }
        else
        {
            SHR_FREE(allocated_mem);
        }
    }
    SHR_FUNC_EXIT;
}

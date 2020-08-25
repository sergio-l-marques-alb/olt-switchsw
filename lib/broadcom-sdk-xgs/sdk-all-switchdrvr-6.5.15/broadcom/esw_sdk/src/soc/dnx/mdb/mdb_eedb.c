/**
 * \file mdb_eedb.c $Id$ Contains all of the MDB eedb table access functions provided to the DBAL.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Includes
 * {
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include "../dbal/dbal_internal.h"
#include "mdb_internal.h"
#include <soc/dnx/mdb_sbusdma_desc.h>

/*
 * }
 */

/*
 * Defines
 * {
 */
#define EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit)  (dnx_data_mdb.dh.nof_data_rows_per_hitbit_row_get(_unit))

#define EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW (MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE)

#define EEDB_HIT_BIT_NOF_ENTRIES_PER_ROW(_unit)    (EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit) * EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW)

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define EEDB_DATA_ROW_TO_HIT_BIT_ROW(_unit, _data_cluster_row) \
    (_data_cluster_row / EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit))

#define EEDB_GET_HIT_BIT_ROW_OFFSET(_unit, _data_cluster_row, _data_row_offset) \
    (((_data_cluster_row % EEDB_HIT_BIT_NOF_DATA_ROWS_PER_ROW(_unit)) * EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW) \
    + (EEDB_BANK_NOF_BASIC_ENTRIES_PER_ROW - (_data_row_offset / MDB_DIRECT_BASIC_ENTRY_SIZE) - 1))

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
 * globals
 * {
 */

static char *mdb_macro_type_strings[MDB_NOF_MACRO_TYPES] = {
    "MDB_MACRO_A",
    "MDB_MACRO_B",
    "MDB_EEDB_BANK"
};

const dbal_enum_value_field_mdb_physical_table_e mdb_eedb_phase_to_mdb_data_enum[DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES] =
    { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA,
    DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA,
    DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA,
    DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA,
    DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA,
    DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA
};

/*
 * }
 */

shr_error_e
mdb_eedb_cluster_type_to_rows(
    int unit,
    mdb_macro_types_e macro_type,
    int *cluster_rows)
{
    SHR_FUNC_INIT_VARS(unit);

    if (macro_type == MDB_MACRO_A)
    {
        *cluster_rows = MDB_NOF_ROWS_IN_MACRO_A_CLUSTER;
    }
    else if (macro_type == MDB_MACRO_B)
    {
        *cluster_rows = MDB_NOF_ROWS_IN_MACRO_B_CLUSTER;
    }
    else if (macro_type == MDB_EEDB_BANK)
    {
        *cluster_rows = MDB_NOF_ROWS_IN_EEDB_BANK;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected macro type: %d.\n", macro_type);
    }

exit:
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
    int ll_capacity = 0;
    int data_capacity = 0;
    int cluster_idx;
    mdb_cluster_info_t cluster_info;
    int sum_iteration;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_clusters;
    dbal_enum_value_field_mdb_physical_table_e eedb_ll_banks[] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL
    };
    dbal_enum_value_field_mdb_physical_table_e eedb_data_banks[] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA
    };
    dbal_enum_value_field_mdb_physical_table_e eedb_data_clusters[] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA
    };

    SHR_FUNC_INIT_VARS(unit);

    if (phase_index >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. DBAL physical table is not an EEDB table: %s.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    for (sum_iteration = 0; sum_iteration < 3; sum_iteration++)
    {
        temp_capacity = 0;
        if (sum_iteration == 0)
        {
            mdb_physical_table_id = eedb_ll_banks[phase_index];
        }
        else if (sum_iteration == 1)
        {
            mdb_physical_table_id = eedb_data_banks[phase_index];
        }
        else
        {
            mdb_physical_table_id = eedb_data_clusters[phase_index / 2];
        }
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            uint32 cluster_payload_size;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, cluster_idx, &cluster_info));

            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase_index, &cluster_payload_size));

            temp_capacity +=
                (cluster_info.end_address -
                 cluster_info.start_address) * row_width / MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(cluster_payload_size);
        }

        if (sum_iteration == 0)
        {
            ll_capacity += temp_capacity;
        }
        else
        {
            data_capacity += temp_capacity;
        }
    }

    *capacity = data_capacity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_set_phase_map_element(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    int start_index,
    int end_index,
    uint32 *phase_map)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = start_index; ii < end_index; ii++)
    {

        if ((phase_map[ii] < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES) && (phase_map[ii] != phase))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. phase_map element already configured with a different phase. Currently set: %d. Attempted set: %d.\n",
                         phase_map[ii], phase);
        }
        else
        {
            phase_map[ii] = phase;
        }
    }

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
    mdb_macro_types_e macro_type,
    dbal_enum_value_field_mdb_physical_table_e * mdb_physical_table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
    {
        switch (phase)
        {
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL;
                break;
            case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8:
                *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected phase: %d.\n", phase);
        }
    }
    else if (eedb_mem_type == MDB_EEDB_MEM_TYPE_PHY)
    {
        if (macro_type == MDB_EEDB_BANK)
        {
            switch (phase)
            {
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected phase: %d.\n", phase);
            }
        }
        else
        {
            switch (phase)
            {
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
                    break;
                case DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8:
                    *mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected phase: %d.\n", phase);
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected eedb_mem_type: %d.\n", eedb_mem_type);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_phase_to_dbal(
    int unit,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    dbal_physical_tables_e * dbal_physical_table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (phase >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid EEDB phase: %d.\n", phase);
    }

    *dbal_physical_table_id = phase + DBAL_PHYSICAL_TABLE_EEDB_1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_get_cluster_map_info(
    int unit,
    mdb_eedb_mem_type_e eedb_mem_type,
    dbal_enum_value_field_mdb_eedb_phase_e phase,
    uint32 entry,
    mdb_macro_types_e * cluster_type,
    int *macro_idx,
    int *cluster_idx,
    int *cluster_row,
    int *row_offset,
    dbal_enum_value_field_mdb_direct_payload_e * payload_size_type)
{
    int table_cluster_idx = -1;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;
    dbal_enum_value_field_mdb_direct_payload_e payload_type;
    mdb_cluster_info_t cluster_info;
    int physical_address;
    int address_granularity;
    int nof_clusters, start_address, end_address;
    SHR_FUNC_INIT_VARS(unit);

    if (eedb_mem_type == MDB_EEDB_MEM_TYPE_LL)
    {
        int ii;
        SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, eedb_mem_type, phase, MDB_EEDB_BANK, &mdb_physical_table_id));
        address_granularity = MDB_DIRECT_BASIC_ENTRY_SIZE;
        physical_address = entry / (MDB_NOF_CLUSTER_ROW_BITS / address_granularity);
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
        for (ii = 0; ii < nof_clusters; ii++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            start_address.get(unit, mdb_physical_table_id, ii, &start_address));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            end_address.get(unit, mdb_physical_table_id, ii, &end_address));
            if ((physical_address >= start_address) && (physical_address < end_address))
            {
                table_cluster_idx = ii;
                break;
            }
        }
    }
    else if (eedb_mem_type == MDB_EEDB_MEM_TYPE_PHY)
    {
        int search_iteration;
        for (search_iteration = 0; search_iteration < MDB_NOF_EEDB_MEM_TYPES; search_iteration++)
        {
            mdb_macro_types_e mem_type;

            if (search_iteration == 0)
            {
                mem_type = MDB_EEDB_BANK;
            }
            else
            {
                mem_type = MDB_NOF_MACRO_TYPES;
            }

            SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, eedb_mem_type, phase, mem_type, &mdb_physical_table_id));

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase, &payload_type));

            address_granularity = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

            physical_address = entry / (MDB_NOF_CLUSTER_ROW_BITS / address_granularity);

            for (table_cluster_idx = 0; table_cluster_idx < nof_clusters; table_cluster_idx++)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                clusters_info.get(unit, mdb_physical_table_id, table_cluster_idx, &cluster_info));

                if ((physical_address >= cluster_info.start_address) && (physical_address < cluster_info.end_address))
                {
                    if (mem_type != MDB_EEDB_BANK)
                    {
                        int phase_alloc_idx =
                            (physical_address - cluster_info.start_address) / MDB_NOF_ROWS_IN_EEDB_BANK;
                        if (cluster_info.phase_alloc_rows[phase_alloc_idx] != phase)
                        {
                            continue;
                        }
                    }

                    break;
                }
            }

            if (table_cluster_idx != nof_clusters)
            {
                break;
            }
            else
            {
                table_cluster_idx = -1;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected eedb_mem_type: %d.\n", eedb_mem_type);
    }

    if (table_cluster_idx != -1)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                        clusters_info.get(unit, mdb_physical_table_id, table_cluster_idx, &cluster_info));
        *cluster_type = cluster_info.macro_type;
        *macro_idx = cluster_info.macro_index;
        *cluster_idx = cluster_info.cluster_index;
        *cluster_row = physical_address - cluster_info.start_address;
        *row_offset =
            (entry - (physical_address * (MDB_NOF_CLUSTER_ROW_BITS / address_granularity))) * address_granularity;
        SHR_IF_ERR_EXIT(mdb_eedb_get_payload_size_from_cluster(unit, &cluster_info, (uint32 *) payload_size_type));
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
    uint8 *hitbit_flags)
{
    /*
     * Get hitbit if required.
     */
    uint32 hitbit_row_value;
    uint32 hitbit_row_cache_value;
    uint32 hitbit;
    uint32 row_in_hitbit_table = EEDB_DATA_ROW_TO_HIT_BIT_ROW(unit, eedb_bank_row);
    int hitbit_offset = EEDB_GET_HIT_BIT_ROW_OFFSET(unit, eedb_bank_row, eedb_row_offset);
    SHR_FUNC_INIT_VARS(unit);

    hitbit_row_value = 0;


    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_hitbit.get(unit, eedb_bank_idx, row_in_hitbit_table,
                                                                         &hitbit_row_cache_value));

    SHR_IF_ERR_EXIT(soc_mem_array_read
                    (unit, MDB_EEDB_ABK_BANKm, eedb_bank_idx, MDB_BLOCK(unit), row_in_hitbit_table, &hitbit_row_value));

    hitbit_row_cache_value |= hitbit_row_value;
    hitbit = hitbit_row_cache_value & (1 << hitbit_offset);

    if (hitbit != 0)
    {
        *hitbit_flags |= DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
    }
    else
    {
        *hitbit_flags &= ~DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A;
    }

    if (*hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
    {
        hitbit_row_cache_value &= ~(1 << hitbit_offset);
    }


    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.eedb_banks_hitbit.set(unit, eedb_bank_idx, row_in_hitbit_table,
                                                                         hitbit_row_cache_value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    dbal_enum_value_field_mdb_eedb_phase_e eedb_phase = 0;
    int eedb_local_index = entry->key[0];
    mdb_macro_types_e ll_cluster_type = MDB_NOF_MACRO_TYPES;
    int ll_macro_idx = 0;
    int ll_cluster_idx = 0;
    int ll_cluster_row = 0;
    int ll_row_offset = 0;
    int ll_entry_size = 0;

    mdb_macro_types_e data_cluster_type = MDB_NOF_MACRO_TYPES;
    int data_macro_idx = 0;
    int data_cluster_idx = 0;
    int data_cluster_row = 0;
    int data_row_offset = 0;

    int entry_max_size;
    dbal_physical_entry_t *entry_p = entry;
    bsl_severity_t severity;
    uint8 cache_enabled;

    dbal_enum_value_field_mdb_direct_payload_e payload_size_type;

    SHR_FUNC_INIT_VARS(unit);

    while ((((ll_cluster_type == MDB_NOF_MACRO_TYPES) && (entry->eedb_ll == TRUE))
            || (data_cluster_type == MDB_NOF_MACRO_TYPES)) && (eedb_phase < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES))
    {
        if (entry->eedb_ll == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_get_cluster_map_info
                            (unit, MDB_EEDB_MEM_TYPE_LL, eedb_phase, eedb_local_index, &ll_cluster_type, &ll_macro_idx,
                             &ll_cluster_idx, &ll_cluster_row, &ll_row_offset, &payload_size_type));
        }

        SHR_IF_ERR_EXIT(mdb_eedb_get_cluster_map_info
                        (unit, MDB_EEDB_MEM_TYPE_PHY, eedb_phase, eedb_local_index, &data_cluster_type, &data_macro_idx,
                         &data_cluster_idx, &data_cluster_row, &data_row_offset, &payload_size_type));
        eedb_phase++;
    }
    eedb_phase--;

    if ((ll_cluster_type == MDB_NOF_MACRO_TYPES) && (data_cluster_type == MDB_NOF_MACRO_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unable to find phase associated with entry: %d.\n", eedb_local_index);
    }

    if ((ll_cluster_type != MDB_NOF_MACRO_TYPES) && (data_cluster_type != MDB_NOF_MACRO_TYPES))
    {
        entry_max_size =
            MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B) + MDB_NOF_CLUSTER_ROW_BITS -
            data_row_offset;
    }
    else if (ll_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        entry_max_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B);
    }
    else if (data_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        if (entry->eedb_ll == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. Entry key: %d, is indicated to use LL, but has no LL HW associated with it.\n",
                         eedb_local_index);
        }
        entry_max_size = MDB_NOF_CLUSTER_ROW_BITS - data_row_offset;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Entry key: %d, is not associated with any HW resources.\n",
                     eedb_local_index);
    }

    if (entry_max_size < entry_p->payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. entry->payload_size:%d does not fit in entry 0x%x, max size for this entry:%d.\n",
                     entry_p->payload_size, eedb_local_index, entry_max_size);
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));

    if (ll_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        ll_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B);
        ll_entry_size = ll_entry_size > entry_p->payload_size ? entry_p->payload_size : ll_entry_size;

        if (ll_cluster_type == MDB_EEDB_BANK)
        {
            uint32 row_data[MDB_ENTRY_DATA_SIZE_IN_UINT32];

            sal_memset(row_data, 0, sizeof(row_data));
            if (cache_enabled == TRUE)
            {
                SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                (unit, MDB_EEDB_BANK, 0, ll_cluster_idx, ll_cluster_row, 0, row_data));
            }
            else
            {
                SHR_IF_ERR_EXIT(soc_mem_array_read
                                (unit, MDB_EEDB_ENTRY_BANKm, ll_cluster_idx, MEM_BLOCK_ANY, ll_cluster_row, row_data));
            }

            SHR_BITCOPY_RANGE(row_data, (MDB_NOF_CLUSTER_ROW_BITS - ll_row_offset) - ll_entry_size,
                              entry_p->payload, 0, ll_entry_size);
#ifdef MDB_FPP_DUMPS
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "********************MDB-LOGIC-START********************\n")));

            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "//DB(EEDB_Bank)(EEDB_Bank_phase_%d), logical addr 'h%x\n`MDB_REGFILE.eedb_entry_bank_mem[%d].bd_write_entry(%d, 120'h"),
                         eedb_phase, eedb_local_index, ll_cluster_idx, ll_cluster_row));

            {
                int row_offset;
                for (row_offset = 0; row_offset < BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS); row_offset++)
                {
                    if (row_offset == 0)
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%06x"),
                                                     0xffffff & row_data[BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS) -
                                                                         row_offset - 1]));
                    }
                    else
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"),
                                                     row_data[BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS) - row_offset - 1]));
                    }
                }
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, ");\n")));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-LOGIC-END********************\n")));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-CMIC-START********************\n")));
#endif
            if (mdb_sbusdma_desc_is_enabled(unit) == TRUE)
            {
                SHR_IF_ERR_EXIT(mdb_sbusdma_desc_add_mem
                                (unit, MDB_EEDB_ENTRY_BANKm, ll_cluster_idx, MDB_BLOCK(unit), ll_cluster_row,
                                 row_data));
            }
            else
            {
                SHR_IF_ERR_EXIT(soc_mem_array_write
                                (unit, MDB_EEDB_ENTRY_BANKm, ll_cluster_idx, MEM_BLOCK_ALL, ll_cluster_row, row_data));
            }
            if (cache_enabled == TRUE)
            {
                SHR_IF_ERR_EXIT(mdb_direct_cache_set
                                (unit, MDB_EEDB_BANK, 0, ll_cluster_idx, ll_cluster_row, 0, row_data));
            }
#ifdef MDB_FPP_DUMPS
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "********************MDB-CMIC-END********************\n")));
#endif
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected LL cluster type: %d.\n", ll_cluster_type);
        }
    }

    if (data_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        int entry_size = entry_p->payload_size - ll_entry_size;

        if (entry_size > 0)
        {
            if (data_cluster_type == MDB_EEDB_BANK)
            {
                uint32 row_data[MDB_ENTRY_DATA_SIZE_IN_UINT32];

                sal_memset(row_data, 0, sizeof(row_data));
                if (entry_size != MDB_NOF_CLUSTER_ROW_BITS)
                {
                    if (cache_enabled == TRUE)
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                        (unit, MDB_EEDB_BANK, 0, data_cluster_idx, data_cluster_row, 0, row_data));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(soc_mem_array_read
                                        (unit, MDB_EEDB_ENTRY_BANKm, data_cluster_idx, MEM_BLOCK_ANY, data_cluster_row,
                                         row_data));
                    }
                }

                SHR_BITCOPY_RANGE(row_data, (MDB_NOF_CLUSTER_ROW_BITS - data_row_offset) - entry_size,
                                  entry_p->payload, ll_entry_size, entry_size);

                if (mdb_sbusdma_desc_is_enabled(unit) == TRUE)
                {
                    SHR_IF_ERR_EXIT(mdb_sbusdma_desc_add_mem
                                    (unit, MDB_EEDB_ENTRY_BANKm, data_cluster_idx, MDB_BLOCK(unit), data_cluster_row,
                                     row_data));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_mem_array_write
                                    (unit, MDB_EEDB_ENTRY_BANKm, data_cluster_idx, MEM_BLOCK_ALL, data_cluster_row,
                                     row_data));
                }
                if (cache_enabled == TRUE)
                {
                    SHR_IF_ERR_EXIT(mdb_direct_cache_set
                                    (unit, MDB_EEDB_BANK, 0, data_cluster_idx, data_cluster_row, 0, row_data));
                }

                SHR_IF_ERR_EXIT(mdb_eedb_table_hitbit_get_and_clear(unit, data_cluster_idx, data_cluster_row,
                                                                    data_row_offset, &entry->hitbit));
            }
            else
            {
                dbal_physical_entry_t data_entry;

                sal_memset(&data_entry, 0x0, sizeof(data_entry));
                data_entry.key[0] = entry->key[0];
                SHR_BITCOPY_RANGE(data_entry.payload, 0, entry_p->payload, ll_entry_size, entry_size);
                data_entry.payload_size = entry_size;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(data_entry.p_mask, 0, entry_size));

                SHR_IF_ERR_EXIT(mdb_direct_table_entry_add
                                (unit, DBAL_PHYSICAL_TABLE_EEDB_1 + eedb_phase, app_id, &data_entry));
            }
        }
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        int row_offset;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_eedb_table_entry_add: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "entry.key: %d. entry.payload_size: %d, entry.payload_offset: %d, physical_table: %s, phase: %d.\n"),
                     entry_p->key[0], entry_p->payload_size, entry_p->payload_offset,
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), eedb_phase));
        for (row_offset = 0; row_offset < BITS2WORDS(entry_p->payload_size); row_offset++)
        {
            uint32 print_index = BITS2WORDS(entry_p->payload_size) - 1 - row_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry.payload[%d]: 0x%08x.\n"), print_index, entry_p->payload[print_index]));
        }

        if (ll_cluster_type != MDB_NOF_MACRO_TYPES)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "ll_cluster_type: %s. ll_cluster_addr: %d. ll_cluster_row: %d. ll_cluster_offset: %d.\n"),
                         mdb_macro_type_strings[ll_cluster_type], ll_cluster_idx, ll_cluster_row, ll_row_offset));
        }

        if (data_cluster_type != MDB_NOF_MACRO_TYPES)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "data_cluster_type: %s. data_cluster_addr: %d. data_cluster_row: %d. data_cluster_offset: %d.\n"),
                         mdb_macro_type_strings[data_cluster_type], data_cluster_idx, data_cluster_row,
                         data_row_offset));

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "payload_size_type: %d.\n"), payload_size_type));
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
    dbal_physical_entry_t * entry)
{
    dbal_enum_value_field_mdb_eedb_phase_e eedb_phase = 0;
    int eedb_local_index = entry->key[0];
    mdb_macro_types_e ll_cluster_type = MDB_NOF_MACRO_TYPES;
    int ll_macro_idx = 0;
    int ll_cluster_idx = 0;
    int ll_cluster_row = 0;
    int ll_row_offset = 0;
    int ll_entry_size = 0;

    mdb_macro_types_e data_cluster_type = MDB_NOF_MACRO_TYPES;
    int data_macro_idx = 0;
    int data_cluster_idx = 0;
    int data_cluster_row = 0;
    int data_row_offset = 0;
    uint32 row_data[MDB_ENTRY_DATA_SIZE_IN_UINT32];

    int entry_max_size;
    bsl_severity_t severity;
    uint8 cache_enabled;

    dbal_enum_value_field_mdb_direct_payload_e payload_size_type;

    SHR_FUNC_INIT_VARS(unit);

    while ((((ll_cluster_type == MDB_NOF_MACRO_TYPES) && (entry->eedb_ll == TRUE))
            || (data_cluster_type == MDB_NOF_MACRO_TYPES)) && (eedb_phase < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES))
    {
        if (entry->eedb_ll == TRUE)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_get_cluster_map_info
                            (unit, MDB_EEDB_MEM_TYPE_LL, eedb_phase, eedb_local_index, &ll_cluster_type, &ll_macro_idx,
                             &ll_cluster_idx, &ll_cluster_row, &ll_row_offset, &payload_size_type));
        }

        SHR_IF_ERR_EXIT(mdb_eedb_get_cluster_map_info
                        (unit, MDB_EEDB_MEM_TYPE_PHY, eedb_phase, eedb_local_index, &data_cluster_type, &data_macro_idx,
                         &data_cluster_idx, &data_cluster_row, &data_row_offset, &payload_size_type));
        eedb_phase++;
    }

    eedb_phase--;

    if (eedb_phase >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Unable to find phase associated with entry: %d.\n", eedb_local_index);
    }

    if ((ll_cluster_type != MDB_NOF_MACRO_TYPES) && (data_cluster_type != MDB_NOF_MACRO_TYPES))
    {
        entry_max_size =
            MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B) + MDB_NOF_CLUSTER_ROW_BITS -
            data_row_offset;
    }
    else if (ll_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        entry_max_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B);
    }
    else if (data_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        if (entry->eedb_ll == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. Entry key: %d, is indicated to use LL, but has no LL HW associated with it.\n",
                         eedb_local_index);
        }
        entry_max_size = MDB_NOF_CLUSTER_ROW_BITS - data_row_offset;
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

    if (ll_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        ll_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_30B);
        ll_entry_size = ll_entry_size > entry->payload_size ? entry->payload_size : ll_entry_size;

        if (ll_cluster_type == MDB_EEDB_BANK)
        {
            if ((cache_enabled == TRUE) && ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                                            || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)))
            {
                SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                (unit, MDB_EEDB_BANK, 0, ll_cluster_idx, ll_cluster_row, 0, row_data));
            }
            else
            {
                SHR_IF_ERR_EXIT(soc_mem_array_read
                                (unit, MDB_EEDB_ENTRY_BANKm, ll_cluster_idx, MEM_BLOCK_ANY, ll_cluster_row, row_data));
            }

            SHR_BITCOPY_RANGE(entry->payload, 0, row_data,
                              (MDB_NOF_CLUSTER_ROW_BITS - ll_row_offset) - ll_entry_size, ll_entry_size);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected LL cluster type: %d.\n", ll_cluster_type);
        }
    }

    if (data_cluster_type != MDB_NOF_MACRO_TYPES)
    {
        int entry_size = entry->payload_size - ll_entry_size;

        if (entry_size > 0)
        {
            if (data_cluster_type == MDB_EEDB_BANK)
            {
                if ((cache_enabled == TRUE) && ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL)
                                                || (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW)))
                {
                    SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                    (unit, MDB_EEDB_BANK, 0, data_cluster_idx, data_cluster_row, 0, row_data));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_mem_array_read
                                    (unit, MDB_EEDB_ENTRY_BANKm, data_cluster_idx, MEM_BLOCK_ANY, data_cluster_row,
                                     row_data));
                }

                SHR_BITCOPY_RANGE(entry->payload, ll_entry_size, row_data,
                                  (MDB_NOF_CLUSTER_ROW_BITS - data_row_offset) - entry_size, entry_size);

                if (_SHR_IS_FLAG_SET(entry->hitbit, DBAL_PHYSICAL_KEY_HITBIT_ACTION))
                {
                    SHR_IF_ERR_EXIT(mdb_eedb_table_hitbit_get_and_clear(unit, data_cluster_idx, data_cluster_row,
                                                                        data_row_offset, &entry->hitbit));
                }
            }
            else
            {
                dbal_physical_entry_t data_entry;

                sal_memset(&data_entry, 0x0, sizeof(data_entry));
                data_entry.key[0] = entry->key[0];
                data_entry.payload_size = entry_size;
                if (_SHR_IS_FLAG_SET(entry->hitbit, DBAL_PHYSICAL_KEY_HITBIT_ACTION))
                {
                    /*
                     * If hitbit was required, then retrieve it as well.
                     */
                    data_entry.hitbit = entry->hitbit;
                }

                SHR_IF_ERR_EXIT(mdb_direct_table_entry_get
                                (unit, DBAL_PHYSICAL_TABLE_EEDB_1 + eedb_phase, app_id, &data_entry));

                SHR_BITCOPY_RANGE(entry->payload, ll_entry_size, data_entry.payload, 0, entry_size);
                entry->hitbit = data_entry.hitbit;
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
                      "entry.key: %d. entry.payload_size: %d, entry.payload_offset: %d, physical_table: %s, phase: %d.\n"),
                     entry->key[0], entry->payload_size, entry->payload_offset,
                     dbal_physical_table_to_string(unit, dbal_physical_table_id), eedb_phase - 1));
        for (row_offset = 0; row_offset < BITS2WORDS(entry->payload_size); row_offset++)
        {
            uint32 print_index = BITS2WORDS(entry->payload_size) - 1 - row_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry.payload[%d]: 0x%08x.\n"), print_index, entry->payload[print_index]));
        }

        if (ll_cluster_type != MDB_NOF_MACRO_TYPES)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "ll_cluster_type: %s. ll_cluster_idx: %d. ll_cluster_row: %d. ll_row_offset: %d.\n"),
                         mdb_macro_type_strings[ll_cluster_type], ll_cluster_idx, ll_cluster_row, ll_row_offset));
        }

        if (data_cluster_type != MDB_NOF_MACRO_TYPES)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U
                         (unit,
                          "data_cluster_type: %s. data_cluster_idx: %d. data_cluster_row: %d. data_row_offset: %d.\n"),
                         mdb_macro_type_strings[data_cluster_type], data_cluster_idx, data_cluster_row,
                         data_row_offset));

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "payload_size_type: %d.\n"), payload_size_type));
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_eedb_table_entry_get: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry)
{
    dbal_physical_entry_t temp_entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&temp_entry, entry, sizeof(dbal_physical_entry_t));

    sal_memset(temp_entry.payload, 0x0, sizeof(temp_entry.payload));
    sal_memset(temp_entry.p_mask, 0xFF, sizeof(temp_entry.p_mask));

    SHR_IF_ERR_EXIT(mdb_eedb_table_entry_add(unit, dbal_physical_table_id, app_id, &temp_entry));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode)
{
    dbal_physical_entry_t entry;
    uint32 row_data_write[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    uint32 row_data_read[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    int entry_counter;
    int uint32_counter;
    dbal_enum_value_field_mdb_eedb_phase_e phase_index = dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;
    dbal_enum_value_field_mdb_physical_table_e ll_table, data_table, data_macro_table, mdb_physical_table_id;
    int table_iter;
    dbal_enum_value_field_mdb_direct_payload_e entry_payload_type, table_payload_type;
    uint32 entry_size;
    uint32 entry_index_size;
    int index_factor;
    int row_width;
    int nof_clusters, start_address, end_address;
    SHR_FUNC_INIT_VARS(unit);

    sal_srand(55555);

    if ((dbal_physical_table_id < DBAL_PHYSICAL_TABLE_EEDB_1) || (dbal_physical_table_id > DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. DBAL physical table %d is not associated with an MDB EEDB table.\n",
                     dbal_physical_table_id);
    }

    sal_memset(row_data_write, 0x0, (MDB_ENTRY_DATA_SIZE_IN_UINT32) * sizeof(row_data_write[0]));
    sal_memset(row_data_read, 0x0, (MDB_ENTRY_DATA_SIZE_IN_UINT32) * sizeof(row_data_read[0]));

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, MDB_EEDB_BANK, &ll_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, MDB_EEDB_BANK, &data_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, ~MDB_EEDB_BANK, &data_macro_table));

    entry_counter = 0;
    for (table_iter = 0; table_iter < 3; table_iter++)
    {
        int cluster_index = 0;
        int entry_index;

        if (table_iter == 0)
        {
            mdb_physical_table_id = ll_table;
            entry_payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_150B;
        }
        else if (table_iter == 1)
        {
            mdb_physical_table_id = data_table;
            entry_payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
        }
        else
        {
            mdb_physical_table_id = data_macro_table;
            entry_payload_type = DBAL_ENUM_FVAL_MDB_DIRECT_PAYLOAD_TYPE_120B;
        }

        entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(entry_payload_type);

        SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id, &table_payload_type));

        entry_index_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(table_payload_type);

        index_factor =
            ((MDB_CEILING(MDB_DIRECT_BASIC_ENTRY_SIZE, MDB_NOF_CLUSTER_ROW_BITS)) * MDB_DIRECT_BASIC_ENTRY_SIZE) /
            entry_index_size;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
        while (cluster_index < nof_clusters)
        {
            int entries_per_row;
            int cluster_entry_size;
            if (table_iter == 0)
            {
                cluster_entry_size = MDB_DIRECT_BASIC_ENTRY_SIZE;
                row_width = MDB_NOF_CLUSTER_ROW_BITS;
            }
            else
            {
                cluster_entry_size = entry_index_size;
                row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
            }
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            start_address.get(unit, mdb_physical_table_id, cluster_index, &start_address));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            end_address.get(unit, mdb_physical_table_id, cluster_index, &end_address));

            entries_per_row =
                row_width / ((MDB_CEILING(MDB_DIRECT_BASIC_ENTRY_SIZE, cluster_entry_size)) *
                             MDB_DIRECT_BASIC_ENTRY_SIZE);

            entry_index = start_address * entries_per_row;

            while (entry_index < end_address * entries_per_row)
            {
                if (table_iter == 2)
                {
                    int row_idx;
                    uint8 phase_alloc_rows;
                    int row_idx_factor = DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK * entries_per_row;

                    row_idx =
                        ((entry_index / entries_per_row) - start_address) / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK;

                    mdb_db_infos.db.clusters_info.phase_alloc_rows.get(unit, mdb_physical_table_id, cluster_index,
                                                                       row_idx, &phase_alloc_rows);

                    if (phase_alloc_rows >= DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
                    {
                        entry_index += row_idx_factor;
                        continue;
                    }
                }

                for (uint32_counter = 0; uint32_counter < (MDB_CEILING(SAL_UINT32_NOF_BITS, entry_size));
                     uint32_counter++)
                {
                    row_data_write[uint32_counter] = sal_rand() | (sal_rand() << 15) | ((sal_rand() % 0x3) << 30);
                }

                SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                                (row_data_write,
                                 MDB_DIRECT_BASIC_ENTRY_SIZE - dnx_data_mdb.em.max_nof_vmv_size_get(unit) - 1,
                                 MDB_DIRECT_BASIC_ENTRY_SIZE - 1));

                sal_memset(&entry, 0x0, sizeof(entry));

                entry.key[0] = entry_index;
                entry.payload_size = entry_size;
                if (table_iter == 0)
                {
                    entry.eedb_ll = TRUE;
                }
                else
                {
                    entry.eedb_ll = FALSE;
                }
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry.p_mask, 0, entry.payload_size));
                SHR_BITCOPY_RANGE(entry.payload, 0, row_data_write, 0, entry.payload_size);

                SHR_IF_ERR_EXIT(mdb_eedb_table_entry_add(unit, dbal_physical_table_id, 0 /* app_id */ , &entry));

                sal_memset(entry.payload, 0x0, (MDB_MAX_DIRECT_PAYLOAD_SIZE_32) * sizeof(entry.payload[0]));

                SHR_IF_ERR_EXIT(mdb_eedb_table_entry_get(unit, dbal_physical_table_id, 0 /* app_id */ , &entry));

                sal_memcpy(row_data_read, entry.payload, (MDB_MAX_DIRECT_PAYLOAD_SIZE_32) * sizeof(row_data_read[0]));

                SHR_IF_ERR_EXIT(utilex_bitstream_xor(entry.payload, row_data_write, MDB_MAX_DIRECT_PAYLOAD_SIZE_32));

                if (utilex_bitstream_have_one_in_range(entry.payload, 0 /* start_place */ , entry.payload_size - 1))
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                    for (uint32_counter = MDB_MAX_DIRECT_PAYLOAD_SIZE_32 - 1; uint32_counter >= 0; uint32_counter--)
                    {
                        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), row_data_write[uint32_counter]));
                    }
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                    for (uint32_counter = MDB_MAX_DIRECT_PAYLOAD_SIZE_32 - 1; uint32_counter >= 0; uint32_counter--)
                    {
                        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), row_data_read[uint32_counter]));
                    }
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload size: %d\n"), entry.payload_size));
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_counter: %d\n"), entry_counter));

                    SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, read data is not equal to written data.\n");
                }

                SHR_IF_ERR_EXIT(mdb_eedb_table_entry_delete(unit, dbal_physical_table_id, 0 /* app_id */ , &entry));

                SHR_IF_ERR_EXIT(mdb_eedb_table_entry_get(unit, dbal_physical_table_id, 0 /* app_id */ , &entry));

                if (utilex_bitstream_have_one_in_range(entry.payload, 0 /* start_place */ , entry.payload_size - 1))
                {
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload size: %d\n"), entry.payload_size));
                    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_counter: %d\n"), entry_counter));

                    SHR_ERR_EXIT(_SHR_E_FAIL, "Test failed, data different from 0 after delete.\n");
                }

                if (mode == MDB_TEST_FULL)
                {
                    entry_index += index_factor;
                }
                else
                {
                    entry_index += (sal_rand() % MDB_TEST_BRIEF_FACTOR) * index_factor;
                }
                entry_counter++;
            }
            cluster_index += row_width / MDB_NOF_CLUSTER_ROW_BITS;
        }
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
    dbal_enum_value_field_mdb_physical_table_e data_bank_table_id, data_cluster_table_id;
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_banks;
    dbal_physical_table_def_t *dbal_physical_table;
    int start_address;
    dbal_enum_value_field_mdb_direct_payload_e payload_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, dbal_physical_table_id, &dbal_physical_table));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, MDB_EEDB_BANK, &data_bank_table_id));
    SHR_IF_ERR_EXIT(mdb_eedb_translate
                    (unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, ~MDB_EEDB_BANK, &data_cluster_table_id));
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_bank_table_id, &nof_banks));
    if (nof_banks > 0)
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
    dbal_enum_value_field_mdb_physical_table_e ll_bank_table_id;
    dbal_enum_value_field_mdb_physical_table_e data_bank_table_id, data_cluster_table_id;
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_banks, nof_clusters, start_address, end_address;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(*entry));

    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, MDB_EEDB_BANK, &data_bank_table_id));
    SHR_IF_ERR_EXIT(mdb_eedb_translate(unit, MDB_EEDB_MEM_TYPE_LL, phase_index, MDB_EEDB_BANK, &ll_bank_table_id));
    SHR_IF_ERR_EXIT(mdb_eedb_translate
                    (unit, MDB_EEDB_MEM_TYPE_PHY, phase_index, ~MDB_EEDB_BANK, &data_cluster_table_id));
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

                SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_bank_table_id, &nof_banks));
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

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_bank_table_id, &nof_banks));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, data_cluster_table_id, &nof_clusters));
        if (physical_entry_iterator->mdb_cluster_index >= nof_banks)
        {
            if (nof_clusters == 0)
            {
                *is_end = TRUE;
            }

            while ((*is_end == FALSE)
                   &&
                   (((physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size /
                      row_width) % MDB_NOF_ROWS_IN_EEDB_BANK == 0)
                    || (physical_entry_iterator->mdb_entry_index == MDB_DIRECT_INVALID_START_ADDRESS)))
            {
                int local_cluster_idx = physical_entry_iterator->mdb_cluster_index - nof_banks;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                end_address.get(unit, data_cluster_table_id, local_cluster_idx, &end_address));
                if ((end_address == MDB_DIRECT_INVALID_START_ADDRESS)
                    ||
                    ((physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size /
                      row_width) >= end_address))
                {
                    uint8 valid_cluster_found = FALSE;

                    physical_entry_iterator->mdb_cluster_index++;
                    local_cluster_idx++;

                    while (valid_cluster_found == FALSE)
                    {
                        if (local_cluster_idx >= nof_clusters)
                        {
                            *is_end = TRUE;
                            break;
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                            start_address.get(unit, data_cluster_table_id, local_cluster_idx,
                                                              &start_address));
                            if (start_address == MDB_DIRECT_INVALID_START_ADDRESS)
                            {
                                physical_entry_iterator->mdb_cluster_index++;
                                local_cluster_idx++;
                            }
                            else
                            {
                                valid_cluster_found = TRUE;
                                physical_entry_iterator->mdb_entry_index =
                                    start_address * row_width / physical_entry_iterator->payload_basic_size;
                            }
                        }
                    }
                }

                if ((*is_end == FALSE)
                    &&
                    ((physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size /
                      row_width) % MDB_NOF_ROWS_IN_EEDB_BANK == 0))
                {
                    int phase_alloc_index;
                    uint8 phase_alloc;
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    start_address.get(unit, data_cluster_table_id, local_cluster_idx, &start_address));

                    phase_alloc_index =
                        ((physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size /
                          row_width) - start_address) / MDB_NOF_ROWS_IN_EEDB_BANK;

                    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                    phase_alloc_rows.get(unit, data_cluster_table_id, local_cluster_idx,
                                                         phase_alloc_index, &phase_alloc));
                    if (phase_alloc == phase_index)
                    {
                        break;
                    }
                    else
                    {
                        physical_entry_iterator->mdb_entry_index +=
                            MDB_NOF_ROWS_IN_EEDB_BANK * row_width / physical_entry_iterator->payload_basic_size;
                    }
                }
            }
        }

        if (*is_end == FALSE)
        {
            int ll_cluster_index;

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
                    entry->eedb_ll = TRUE;
                    break;
                }
            }

            SHR_IF_ERR_EXIT(mdb_eedb_table_entry_get(unit, dbal_physical_table_id, app_id, entry));
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
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id)
{
    dbal_physical_entry_iterator_t physical_entry_iterator;
    uint8 is_end = FALSE;
    dbal_physical_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&physical_entry_iterator, 0x0, sizeof(physical_entry_iterator));

    SHR_IF_ERR_EXIT(mdb_eedb_table_iterator_init(unit, dbal_physical_table_id, app_id, &physical_entry_iterator));

    SHR_IF_ERR_EXIT(mdb_eedb_table_iterator_get_next
                    (unit, dbal_physical_table_id, app_id, &physical_entry_iterator, &entry, &is_end));
    while (is_end == FALSE)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_entry_delete(unit, dbal_physical_table_id, app_id, &entry));

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

    macro_row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;

    *nof_entries = nof_rows_in_cluster * macro_row_width / address_granularity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_entry_to_phase(
    int unit,
    uint32 entry_index,
    dbal_physical_tables_e * dbal_physical_table_id)
{
    int ii, jj;
    int nof_mdb_tables;
    int nof_clusters;
    dbal_physical_tables_e dbal_eedb_tables[] = {
        DBAL_PHYSICAL_TABLE_EEDB_1, DBAL_PHYSICAL_TABLE_EEDB_2, DBAL_PHYSICAL_TABLE_EEDB_3, DBAL_PHYSICAL_TABLE_EEDB_4,
        DBAL_PHYSICAL_TABLE_EEDB_5, DBAL_PHYSICAL_TABLE_EEDB_6, DBAL_PHYSICAL_TABLE_EEDB_7, DBAL_PHYSICAL_TABLE_EEDB_8
    };
    dbal_enum_value_field_mdb_physical_table_e mdb_eedb_data_banks[] = {
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL
    };

    SHR_FUNC_INIT_VARS(unit);

    nof_mdb_tables = sizeof(mdb_eedb_data_banks) / sizeof(dbal_enum_value_field_mdb_physical_table_e);

    for (ii = 0; ii < nof_mdb_tables; ii++)
    {

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_eedb_data_banks[ii], &nof_clusters));
        for (jj = 0; jj < nof_clusters; jj++)
        {
            mdb_cluster_info_t cluster_info;

            int physical_address;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_eedb_data_banks[ii], jj, &cluster_info));

            physical_address = (entry_index * MDB_DIRECT_BASIC_ENTRY_SIZE) / MDB_NOF_CLUSTER_ROW_BITS;

            if ((physical_address >= cluster_info.start_address) && (physical_address < cluster_info.end_address))
            {
                *dbal_physical_table_id = dbal_eedb_tables[ii];
                break;
            }
        }
        if (jj < nof_clusters)
        {
            break;
        }
    }

    if (ii == nof_mdb_tables)
    {
        *dbal_physical_table_id = DBAL_NOF_PHYSICAL_TABLES;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
mdb_profiles_e
mdb_eedb_get_payload_size_from_cluster(
    int unit,
    mdb_cluster_info_t * cluster_info_p,
    uint32 *payload_size)
{
    int row_iter;
    SHR_FUNC_INIT_VARS(unit);
    for (row_iter = 0;
         row_iter < DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK;
         row_iter++)
    {
        if (cluster_info_p->phase_alloc_rows[row_iter] != DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.
                            eedb_payload_type.get(unit, cluster_info_p->phase_alloc_rows[row_iter], payload_size));
            break;
        }
    }

    if (row_iter == DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No EEDB associated payload size was found for cluster %d at macro %d.\n",
                     cluster_info_p->cluster_index, cluster_info_p->macro_index);
    }
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

    abk_bank = dnx_data_mdb.eedb.abk_bank_get(unit);

    nof_eedb_hitbit_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
    nof_hitbit_rows = dnx_data_mdb.dh.nof_rows_in_eedb_hitbit_bank_get(unit);

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

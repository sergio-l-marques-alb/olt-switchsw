/*
 * ! \file mdb_direct.c $Id$ Contains all of the MDB direct table access functions provided to the DBAL.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/adapter/adapter_mdb_access.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>
#include "../dbal/dbal_internal.h"
#include "mdb_internal.h"
#include <soc/dnx/dnx_state_snapshot_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX

#define MDB_ENTRY_EMPTY_CONTROL           0

#define MDB_TABLE_NAME_MAX_LENGTH         50

#define ADAPTER_TABLE_ENTRY_MAX_SIZE      256

#define MDB_MAX_CLUSTER_PER_DB            2

#define MDB_HITBIT_DATA_OFFSET            0
#define MDB_HITBIT_DATA_SIZE              (MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE)
#define MDB_HITBIT_DATA_NUM(unit)         (dnx_data_mdb.dh.nof_bucket_clusters_get(unit))

#define MDB_HITBIT_MASK_OFFSET(unit)      (dnx_data_mdb.dh.nof_bucket_clusters_get(unit) * MDB_HITBIT_DATA_SIZE)
#define MDB_HITBIT_MASK_SIZE              (MDB_NOF_CLUSTER_ROW_BITS / MDB_DIRECT_BASIC_ENTRY_SIZE)
#define MDB_HITBIT_MASK_NUM(unit)         (dnx_data_mdb.dh.nof_bucket_clusters_get(unit))

#define MDB_HITBIT_CMD_OFFSET(unit)       (MDB_HITBIT_MASK_OFFSET(unit) + MDB_HITBIT_MASK_SIZE * MDB_HITBIT_MASK_NUM(unit))
#define MDB_HITBIT_CMD_SIZE               (3)
#define MDB_HITBIT_CMD_NUM(unit)          (dnx_data_mdb.dh.nof_bucket_clusters_get(unit))

#define MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,macro_type)  (dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_address_bits)
#define MDB_DIRECT_ADDR_OFFSET_BITS               (2)
#define MDB_DIRECT_ADDR_OFFSET_MASK(unit,macro_type)    ((1 << MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,macro_type)) - 1)

#define MDB_GET_REMAINDER_TYPE(entry_index,entries_size,row_width)\
                             (MDB_ENTRY_REMAINDER_CALC(entry_index,entries_size,row_width)/MDB_DIRECT_BASIC_ENTRY_SIZE)

#define MDB_GET_HIT_BIT_OFFSET(remainder_size) (3 - ((remainder_size % MDB_NOF_CLUSTER_ROW_BITS) / MDB_DIRECT_BASIC_ENTRY_SIZE))

typedef enum
{

    MDB_CLUSTER_OFFSET_NONE,

    MDB_CLUSTER_OFFSET_NEXT_ROW = 1
} mdb_cluster_offset_e;

typedef enum
{

    MDB_CLUSTER_USE_NONE,

    MDB_CLUSTER_USE_ONE = 1,

    MDB_CLUSTER_USE_TWO = 2
} mdb_nof_clusters_to_use_e;

typedef enum
{

    MDB_CLUSTER_BITMAP_NONE = 0x0,

    MDB_CLUSTER_BITMAP_FIRST = 0x1,

    MDB_CLUSTER_BITMAP_SECOND = 0x2,

    MDB_CLUSTER_BITMAP_BOTH = 0x3
} mdb_cluster_use_bitmap_e;

typedef enum
{

    MDB_CLUSTER_INDEX_OFFSET_NONE = 0,

    MDB_CLUSTER_INDEX_OFFSET_ONE = 1
} mdb_cluster_index_offset_e;

typedef enum
{

    MDB_ENTRY_OFFSET_0b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 0),

    MDB_ENTRY_OFFSET_30b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 1),

    MDB_ENTRY_OFFSET_60b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 2),

    MDB_ENTRY_OFFSET_90b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 3),

    MDB_ENTRY_OFFSET_120b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 4)
} mdb_entry_offset_e;

typedef enum
{

    MDB_ENTRY_SIZE_0b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 0),

    MDB_ENTRY_SIZE_30b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 1),

    MDB_ENTRY_SIZE_60b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 2),

    MDB_ENTRY_SIZE_90b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 3),

    MDB_ENTRY_SIZE_120b = (MDB_DIRECT_BASIC_ENTRY_SIZE * 4)
} mdb_entry_size_e;

typedef struct mdb_cluster_direct_entry_access_info
{

    mdb_cluster_index_offset_e cluster_offset;

    int cluster_row_offset;

    mdb_entry_size_e entry_size;

    mdb_entry_offset_e entry_offset;
} mdb_cluster_direct_entry_access_info_t;

typedef struct mdb_cluster_direct_entry_info
{

    mdb_cluster_use_bitmap_e active_bitmap;

    mdb_nof_clusters_to_use_e nof_used_clusters;

    mdb_cluster_offset_e clusters_offset[MDB_MAX_CLUSTER_PER_DB];

    mdb_cluster_direct_entry_access_info_t access_info[MDB_MAX_CLUSTER_PER_DB];
} mdb_cluster_direct_entry_info_t;

const STATIC uint32 mdb_direct_cluster_data_offset_in_bucket[] = {
    0 * MDB_NOF_CLUSTER_ROW_BITS,
    1 * MDB_NOF_CLUSTER_ROW_BITS,
    2 * MDB_NOF_CLUSTER_ROW_BITS,
    3 * MDB_NOF_CLUSTER_ROW_BITS
};

#ifndef ADAPTER_SERVER_MODE
const STATIC uint8 mdb_direct_hit_bit_accessed_indications[MDB_DIRECT_ENTRY_NOF_HIT_BIT_INDEXS][MDB_NOF_ABK_BANKS] =
    { {DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_A, DBAL_PHYSICAL_KEY_PRIMARY_HITBIT_ACCESSED_B},
{DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_A, DBAL_PHYSICAL_KEY_SECONDARY_HITBIT_ACCESSED_B}
};
#endif

sbusdma_desc_module_enable_e
mdb_direct_dbal_table_to_dma_enum(
    dbal_physical_tables_e dbal_physical_table_id)
{
    sbusdma_desc_module_enable_e desc_module;

    switch (dbal_physical_table_id)
    {
        case DBAL_PHYSICAL_TABLE_KAPS_1:
        case DBAL_PHYSICAL_TABLE_KAPS_2:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_LPM;
            break;
        case DBAL_PHYSICAL_TABLE_INLIF_1:
        case DBAL_PHYSICAL_TABLE_INLIF_2:
        case DBAL_PHYSICAL_TABLE_INLIF_3:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_INLIF;
            break;
        case DBAL_PHYSICAL_TABLE_IVSI:
        case DBAL_PHYSICAL_TABLE_EVSI:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_VSI;
            break;
        case DBAL_PHYSICAL_TABLE_MAP:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_MAP;
            break;
        case DBAL_PHYSICAL_TABLE_FEC_1:
        case DBAL_PHYSICAL_TABLE_FEC_2:
        case DBAL_PHYSICAL_TABLE_FEC_3:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_FEC;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_1:
        case DBAL_PHYSICAL_TABLE_EEDB_2:
        case DBAL_PHYSICAL_TABLE_EEDB_3:
        case DBAL_PHYSICAL_TABLE_EEDB_4:
        case DBAL_PHYSICAL_TABLE_EEDB_5:
        case DBAL_PHYSICAL_TABLE_EEDB_6:
        case DBAL_PHYSICAL_TABLE_EEDB_7:
        case DBAL_PHYSICAL_TABLE_EEDB_8:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_MDB_EEDB;
            break;
        default:
            desc_module = SBUSDMA_DESC_MODULE_ENABLE_NOF_MODULES;
    }

    return desc_module;
}

shr_error_e
mdb_direct_init(
    int unit)
{
    dbal_physical_tables_e dbal_physical_table_id;
    uint8 is_standard_image = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_standard_image));
    if ((is_standard_image)
        && (!(dnx_data_sbusdma_desc.global.feature_get(unit, dnx_data_sbusdma_desc_global_force_enable))))
    {
        for (dbal_physical_table_id = DBAL_PHYSICAL_TABLE_NONE; dbal_physical_table_id < DBAL_NOF_PHYSICAL_TABLES;
             dbal_physical_table_id++)
        {
            if ((dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_1)
                || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_2)
                || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_FEC_3)
                || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_KAPS_1)
                || (dbal_physical_table_id == DBAL_PHYSICAL_TABLE_KAPS_2))
            {
                continue;
            }

            if (dnx_sbusdma_desc_is_enabled(unit, mdb_direct_dbal_table_to_dma_enum(dbal_physical_table_id)) == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Descriptor DMA soc property (%s) is not supported for Direct/EEDB tables (%s).\n",
                             spn_DMA_DESC_AGGREGATOR_ENABLE_SPECIFIC, dbal_physical_table_to_string(unit,
                                                                                                    dbal_physical_table_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_calc_cluster_info(
    int unit,
    int row_width,
    int payload_size,
    int remainder_size,
    mdb_cluster_direct_entry_info_t * cluster_info)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(cluster_info, 0x0, sizeof(*cluster_info));

    if (row_width == MDB_NOF_CLUSTER_ROW_BITS)
    {

        if (remainder_size + payload_size > MDB_NOF_CLUSTER_ROW_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. remainder_size (%d) + payload_size (%d) > row_width (%d) in single cluster mode.\n",
                         remainder_size, payload_size, MDB_NOF_CLUSTER_ROW_BITS);
        }

        cluster_info->active_bitmap = MDB_CLUSTER_BITMAP_FIRST;
        cluster_info->nof_used_clusters = MDB_CLUSTER_USE_ONE;
        cluster_info->clusters_offset[0] = MDB_CLUSTER_OFFSET_NONE;
        cluster_info->access_info[0].entry_size = payload_size;
        cluster_info->access_info[0].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_NONE;
        cluster_info->access_info[0].cluster_row_offset =
            MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[0].entry_size - remainder_size;
        cluster_info->access_info[0].entry_offset = 0;
    }
    else if (row_width == 2 * MDB_NOF_CLUSTER_ROW_BITS)
    {

        if (remainder_size < MDB_NOF_CLUSTER_ROW_BITS)
        {

            if (remainder_size + payload_size > MDB_NOF_CLUSTER_ROW_BITS)
            {

                cluster_info->active_bitmap = MDB_CLUSTER_BITMAP_BOTH;
                cluster_info->nof_used_clusters = MDB_CLUSTER_USE_TWO;
                cluster_info->clusters_offset[0] = MDB_CLUSTER_OFFSET_NONE;
                cluster_info->clusters_offset[1] = MDB_CLUSTER_OFFSET_NONE;

                cluster_info->access_info[1].entry_size = MDB_NOF_CLUSTER_ROW_BITS - remainder_size;
                cluster_info->access_info[1].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_ONE;
                cluster_info->access_info[1].cluster_row_offset =
                    MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[1].entry_size - remainder_size;
                cluster_info->access_info[1].entry_offset = payload_size - cluster_info->access_info[1].entry_size;

                cluster_info->access_info[0].entry_size = payload_size - cluster_info->access_info[1].entry_size;
                cluster_info->access_info[0].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_NONE;
                cluster_info->access_info[0].cluster_row_offset =
                    MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[0].entry_size;
                cluster_info->access_info[0].entry_offset = 0;
            }
            else
            {

                cluster_info->active_bitmap = MDB_CLUSTER_BITMAP_SECOND;
                cluster_info->nof_used_clusters = MDB_CLUSTER_USE_ONE;
                cluster_info->clusters_offset[1] = MDB_CLUSTER_OFFSET_NONE;
                cluster_info->access_info[1].entry_size = payload_size;
                cluster_info->access_info[1].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_ONE;
                cluster_info->access_info[1].cluster_row_offset =
                    MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[1].entry_size - remainder_size;
                cluster_info->access_info[1].entry_offset = 0;
            }
        }
        else
        {

            if (remainder_size - MDB_NOF_CLUSTER_ROW_BITS + payload_size > MDB_NOF_CLUSTER_ROW_BITS)
            {

                cluster_info->active_bitmap = MDB_CLUSTER_BITMAP_BOTH;
                cluster_info->nof_used_clusters = MDB_CLUSTER_USE_TWO;
                cluster_info->clusters_offset[0] = MDB_CLUSTER_OFFSET_NONE;
                cluster_info->clusters_offset[1] = MDB_CLUSTER_OFFSET_NEXT_ROW;

                cluster_info->access_info[0].entry_size = row_width - remainder_size;
                cluster_info->access_info[0].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_NONE;
                cluster_info->access_info[0].cluster_row_offset =
                    MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[0].entry_size - (remainder_size -
                                                                                          MDB_NOF_CLUSTER_ROW_BITS);
                cluster_info->access_info[0].entry_offset = payload_size - cluster_info->access_info[0].entry_size;

                cluster_info->access_info[1].entry_size = payload_size - cluster_info->access_info[0].entry_size;
                cluster_info->access_info[1].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_ONE;
                cluster_info->access_info[1].cluster_row_offset =
                    MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[1].entry_size;
                cluster_info->access_info[1].entry_offset = 0;
            }
            else
            {

                cluster_info->active_bitmap = MDB_CLUSTER_BITMAP_FIRST;
                cluster_info->nof_used_clusters = MDB_CLUSTER_USE_ONE;
                cluster_info->clusters_offset[0] = MDB_CLUSTER_OFFSET_NONE;
                cluster_info->access_info[0].entry_size = payload_size;
                cluster_info->access_info[0].cluster_offset = MDB_CLUSTER_INDEX_OFFSET_NONE;
                cluster_info->access_info[0].cluster_row_offset =
                    MDB_NOF_CLUSTER_ROW_BITS - cluster_info->access_info[0].entry_size - (remainder_size -
                                                                                          MDB_NOF_CLUSTER_ROW_BITS);
                cluster_info->access_info[0].entry_offset = 0;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. row_width %d is not supported.\n", row_width);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_calc_macro_mem_blk(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    soc_mem_t * mem_entry,
    soc_reg_t * overflow_reg,
    soc_mem_t mem_hitbits[MDB_NOF_ABK_BANKS],
    int *blk)
{

    uint32 global_macro_index;

    SHR_FUNC_INIT_VARS(unit);

    global_macro_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, macro_type, macro_index);

    SHR_IF_ERR_EXIT(mdb_get_dh_block(unit, global_macro_index, blk));

    if (mem_entry != NULL)
    {
        *mem_entry = dnx_data_mdb.dh.entry_banks_info_get(unit, global_macro_index)->entry_bank;
    }

    if (overflow_reg != NULL)
    {
        *overflow_reg = dnx_data_mdb.dh.entry_banks_info_get(unit, global_macro_index)->overflow_reg;
    }

    if (mem_hitbits != NULL)
    {
        mem_hitbits[MDB_ABK_BANK_A] = dnx_data_mdb.dh.entry_banks_info_get(unit, global_macro_index)->abk_bank_a;

        mem_hitbits[MDB_ABK_BANK_B] = dnx_data_mdb.dh.entry_banks_info_get(unit, global_macro_index)->abk_bank_b;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_mem_blk_to_macro_string(
    int unit,
    soc_mem_t mem,
    int blk,
    char *macro_str)
{
    int ii, jj;
    int max_macro_index[2] = { 12, 8 };
    soc_mem_t mem_get;
    int blk_get;
    int macro_a_idx_trans[12] = { 0, 4, 6, 11, 7, 10, 2, 1, 8, 3, 5, 9 };
    int macro_b_idx_trans[8] = { 0, 1, 5, 3, 7, 4, 2, 6 };

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < 2; ii++)
    {
        for (jj = 0; jj < max_macro_index[ii]; jj++)
        {
            SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk(unit, ii, jj, &mem_get, NULL, NULL, &blk_get));
            if ((blk_get == blk) && (mem_get == mem))
            {
                break;
            }
        }
        if (jj != max_macro_index[ii])
        {
            break;
        }
    }
    if (ii == 2)
    {
        sal_strcpy(macro_str, "macro_not_found");
    }
    else
    {
        char str[128];
        int count = 0;
        if (ii == 0)
        {
            count = sal_sprintf(str, "%s%d", "dha", macro_a_idx_trans[jj]);
            str[count] = '\0';
        }
        else
        {
            count = sal_sprintf(str, "%s%d", "dhb", macro_b_idx_trans[jj]);
            str[count] = '\0';
        }
        sal_strcpy(macro_str, str);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_direct_update_bucket_row_control(
    int unit,
    soc_mem_t mem,
    mdb_macro_types_e macro_type,
    uint32 clusters_in_use_bitmap,
    const uint32 *clusters_format,
    uint32 *row_data,
    uint32 *row_in_cluster)
{
    int cluster_index, cluster_max;
    int format_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    cluster_max = dnx_data_mdb.dh.nof_bucket_clusters_get(unit);

    for (cluster_index = 0; cluster_index < cluster_max; cluster_index++)
    {
        if ((clusters_in_use_bitmap & (1 << cluster_index)) != 0)
        {

            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range
                            (row_in_cluster,
                             MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                           macro_type) + MDB_DIRECT_ADDR_OFFSET_BITS * cluster_index,
                             MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                           macro_type) + MDB_DIRECT_ADDR_OFFSET_BITS * cluster_index +
                             1));

            if (clusters_format[format_index] != 0)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                                (row_in_cluster,
                                 MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                               macro_type) +
                                 MDB_DIRECT_ADDR_OFFSET_BITS * cluster_index + 1));
            }
            format_index++;
        }
        else
        {

            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                            (row_in_cluster,
                             MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                           macro_type) + MDB_DIRECT_ADDR_OFFSET_BITS * cluster_index,
                             MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                           macro_type) + MDB_DIRECT_ADDR_OFFSET_BITS * cluster_index +
                             1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_address_mapping_alloc(
    int unit)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id_iter;
    int alloc_size;

    SHR_FUNC_INIT_VARS(unit);

    alloc_size =
        (1 << dnx_data_mdb.direct.physical_address_max_bits_get(unit)) /
        dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_B)->nof_rows;
    for (mdb_physical_table_id_iter = 0; mdb_physical_table_id_iter < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
         mdb_physical_table_id_iter++)
    {
        if ((dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id_iter)->db_type ==
             DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
            || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
            || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
            || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
            || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                            direct_address_mapping.alloc(unit, mdb_physical_table_id_iter, alloc_size));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_address_mapping_verify(
    int unit,
    int physical_address_start,
    int physical_address_end)
{
    int max_physical_address;

    SHR_FUNC_INIT_VARS(unit);

    max_physical_address = (1 << dnx_data_mdb.direct.physical_address_max_bits_get(unit));
    if ((physical_address_start >= max_physical_address) || (physical_address_end >= max_physical_address)
        || (physical_address_start >= physical_address_end))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Invalid physical address range, start 0x%x, end 0x%x.\n",
                     physical_address_start, physical_address_end);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_address_mapping_set_val(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end,
    uint32 cluster_index_val)
{
    int address_iter;
    int alloc_range_size;

    SHR_FUNC_INIT_VARS(unit);

    alloc_range_size = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_B)->nof_rows;

    for (address_iter = physical_address_start / alloc_range_size;
         address_iter < physical_address_end / alloc_range_size; address_iter++)
    {
        uint32 cluster_index_val_prev;

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                        direct_address_mapping.get(unit, mdb_physical_table_id, address_iter, &cluster_index_val_prev));
        if (cluster_index_val_prev != SAL_UINT32_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error. Address 0x%x is already set for mdb_physical_table_id %d.\n",
                         address_iter, mdb_physical_table_id);
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                        direct_address_mapping.set(unit, mdb_physical_table_id, address_iter, cluster_index_val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_address_mapping_set(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end,
    int cluster_index)
{
    uint32 cluster_index_val = cluster_index;

    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) &&
        ((mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
         && (mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
         && (mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
         && (mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. MDB physical table (%d) is not direct.\n", mdb_physical_table_id);
    }

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_verify(unit, physical_address_start, physical_address_end));

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set_val
                    (unit, mdb_physical_table_id, physical_address_start, physical_address_end, cluster_index_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_address_mapping_unset(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address_start,
    int physical_address_end)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT) &&
        ((mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
         && (mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
         && (mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
         && (mdb_physical_table_id != DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. MDB physical table (%d) is not direct.\n", mdb_physical_table_id);
    }

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_verify(unit, physical_address_start, physical_address_end));

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set_val
                    (unit, mdb_physical_table_id, physical_address_start, physical_address_end, SAL_UINT32_MAX));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_address_mapping_get(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int physical_address,
    int *cluster_index)
{
    int alloc_range_size;
    uint32 cluster_index_u32;

    SHR_FUNC_INIT_VARS(unit);

    alloc_range_size = dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_B)->nof_rows;
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                    direct_address_mapping.get(unit, mdb_physical_table_id, physical_address / alloc_range_size,
                                               &cluster_index_u32));

    if (cluster_index_u32 == SAL_UINT32_MAX)
    {

        SHR_SET_CURRENT_ERR(_SHR_E_RESOURCE);
        SHR_EXIT();
    }
    else
    {
        *cluster_index = cluster_index_u32;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_cache_init(
    int unit)
{
    mdb_macro_types_e macro_type_iter;

    SHR_FUNC_INIT_VARS(unit);

    for (macro_type_iter = 0; macro_type_iter < MDB_NOF_MACRO_TYPES; macro_type_iter++)
    {
        uint32 nof_clusters = dnx_data_mdb.dh.macro_type_info_get(unit,
                                                                  macro_type_iter)->nof_macros *
            dnx_data_mdb.dh.macro_type_info_get(unit,
                                                macro_type_iter)->nof_clusters;

        if (nof_clusters > 0)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                            macro_cluster_cache.alloc(unit, macro_type_iter, nof_clusters));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_cache_alloc(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index)
{
    int global_cluster_idx =
        (macro_index * dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters) + cluster_index;
    int alloc_size;

    SHR_FUNC_INIT_VARS(unit);

    alloc_size = dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_rows + 1;

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_cache.
                    data.alloc(unit, macro_type, global_cluster_idx, alloc_size));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_cache_set(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 src_offset_bits,
    uint32 *src_array)
{
    uint32 row_cache[BITS2WORDS(DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS)];
    int global_cluster_idx =
        (macro_index * dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters) + cluster_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCOPY_RANGE(row_cache, 0, src_array, src_offset_bits, MDB_NOF_CLUSTER_ROW_BITS);

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_cache.
                    data.range_write(unit, macro_type, global_cluster_idx, row_index,
                                     BITS2WORDS(DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS), row_cache));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_cache_get(
    int unit,
    mdb_macro_types_e macro_type,
    int macro_index,
    int cluster_index,
    int row_index,
    uint32 dst_offset_bits,
    uint32 *dst_array)
{
    uint32 row_cache[BITS2WORDS(DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS)];
    int global_cluster_idx =
        (macro_index * dnx_data_mdb.dh.macro_type_info_get(unit, macro_type)->nof_clusters) + cluster_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_cache.
                    data.range_read(unit, macro_type, global_cluster_idx, row_index,
                                    BITS2WORDS(DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS), row_cache));

    SHR_BITCOPY_RANGE(dst_array, dst_offset_bits, row_cache, 0, MDB_NOF_CLUSTER_ROW_BITS);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_direct_table_read_entry_from_cluster(
    int unit,
    soc_mem_t mem,
    mdb_macro_types_e macro_type,
    int macro_index,
    uint32 bucket_index,
    uint32 cluster_index_in_bucket,
    int blk,
    mdb_cluster_direct_entry_info_t * cluster_info,
    dbal_mdb_action_apply_type_e mdb_action_apply,
    uint32 row_in_cluster,
    uint32 *row_data,
    uint8 cache_enabled)
{
    bsl_severity_t severity;
    uint32 row_in_cluster_cmd = row_in_cluster;

    SHR_FUNC_INIT_VARS(unit);

    if (cache_enabled == TRUE)
    {
        int cluster_iter;
        int cluster_idx_in_bucket_iter;

        for (cluster_iter = 0; cluster_iter < MDB_CLUSTER_USE_TWO; cluster_iter++)
        {
            if (cluster_info->active_bitmap & (1 << cluster_iter))
            {
                cluster_idx_in_bucket_iter =
                    cluster_index_in_bucket + cluster_info->access_info[cluster_iter].cluster_offset;

                SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                (unit, macro_type, macro_index,
                                 (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) +
                                 cluster_idx_in_bucket_iter,
                                 row_in_cluster + cluster_info->clusters_offset[cluster_iter],
                                 mdb_direct_cluster_data_offset_in_bucket[cluster_idx_in_bucket_iter], row_data));
            }
        }
    }
    else
    {

        SHR_IF_ERR_EXIT(mdb_direct_update_bucket_row_control
                        (unit, mem, macro_type, cluster_info->active_bitmap << cluster_index_in_bucket,
                         cluster_info->clusters_offset, row_data, &row_in_cluster_cmd));

        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, mem, bucket_index, blk, row_in_cluster_cmd, row_data));
    }

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        int data_traverse = 0;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_direct_table_read_entry_from_cluster: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit, "Mem: %s. bucket_index: %d. blk: %d. row_in_cluster: %d. row_in_cluster_cmd: %d.\n"),
                     SOC_MEM_NAME(unit, mem), bucket_index, blk, row_in_cluster, row_in_cluster_cmd));
        for (data_traverse = 0; data_traverse < MDB_ENTRY_BANK_SIZE_UINT32; data_traverse++)
        {
            if ((data_traverse % MDB_ENTRY_DATA_PER_BANK == 0) && (data_traverse != 0))
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
            }
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "row_data[%d]: 0x%08X."), data_traverse, row_data[data_traverse]));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nmdb_direct_table_read_entry_from_cluster: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_calc_index_address(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e physical_table,
    uint32 entry_index,
    int entry_size,
    uint32 row_width,
    soc_mem_t * mem_entry,
    soc_reg_t * overflow_reg,
    soc_mem_t * mem_hitbits,
    uint32 *row_in_cluster,
    int *blk,
    uint32 *bucket_index,
    uint32 *cluster_index_in_bucket,
    mdb_macro_types_e * macro_type,
    int *macro_index,
    int *overflow_reg_instance)
{
    mdb_cluster_info_t cluster_info;
    int cluster_idx;
    int physical_address;
    int nof_rows_in_cluster;
    soc_reg_t overflow_reg_temp;

    SHR_FUNC_INIT_VARS(unit);

    physical_address = (entry_index * entry_size) / row_width;

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_get(unit, physical_table, physical_address, &cluster_idx));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, physical_table, cluster_idx, &cluster_info));

    *row_in_cluster = physical_address - cluster_info.start_address;
    *cluster_index_in_bucket = cluster_info.cluster_index % dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    *bucket_index = cluster_info.cluster_index / dnx_data_mdb.dh.nof_bucket_clusters_get(unit);
    *macro_type = cluster_info.macro_type;
    *macro_index = cluster_info.macro_index;

    SHR_IF_ERR_EXIT(mdb_direct_calc_macro_mem_blk
                    (unit, cluster_info.macro_type, cluster_info.macro_index, mem_entry, &overflow_reg_temp,
                     mem_hitbits, blk));

    SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, cluster_info.macro_type, &nof_rows_in_cluster));

    if ((overflow_reg != NULL) && (overflow_reg_instance != NULL))
    {
        if ((row_width == DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS * 2)
            && (*row_in_cluster == nof_rows_in_cluster - 1))
        {
            *overflow_reg = overflow_reg_temp;
            *overflow_reg_instance =
                dnx_data_mdb.dh.block_info_get(unit,
                                               MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info.macro_type,
                                                                             cluster_info.macro_index))->block_index;
        }
        else
        {
            *overflow_reg = INVALIDr;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int basic_size;
    int nof_clusters;
    dbal_physical_table_def_t *dbal_physical_table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, dbal_physical_table_id, &dbal_physical_table));

    SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size_using_logical_mdb_table(unit, dbal_physical_table_id, &basic_size));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

    physical_entry_iterator->payload_basic_size = (uint32) basic_size;

    if (nof_clusters > 0)
    {
        int start_address;
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        start_address.get(unit, mdb_physical_table_id, 0, &start_address));
        physical_entry_iterator->mdb_entry_index = UTILEX_DIV_ROUND_UP((start_address * row_width), basic_size);
    }
    else
    {
        physical_entry_iterator->mdb_entry_index = 0;
    }
    physical_entry_iterator->mdb_cluster_index = 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_clusters, start_address, end_address;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

    sal_memset(entry, 0x0, sizeof(*entry));

    entry->hitbit |= physical_entry_iterator->hit_bit_flags;

    entry->payload_size = physical_entry_iterator->payload_basic_size;

    entry->mdb_action_apply = physical_entry_iterator->mdb_action_apply;

    if (nof_clusters == 0)
    {
        *is_end = TRUE;
    }

    while ((*is_end == FALSE) && (!utilex_bitstream_have_one(entry->payload, BITS2WORDS(entry->payload_size))))
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.end_address.get(unit, mdb_physical_table_id,
                                                                      physical_entry_iterator->mdb_cluster_index,
                                                                      &end_address));

        if ((physical_entry_iterator->mdb_entry_index * physical_entry_iterator->payload_basic_size / row_width) >=
            end_address)
        {
            physical_entry_iterator->mdb_cluster_index += row_width / MDB_NOF_CLUSTER_ROW_BITS;

            if (physical_entry_iterator->mdb_cluster_index >= nof_clusters)
            {
                *is_end = TRUE;
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.start_address.get(unit, mdb_physical_table_id,
                                                                                physical_entry_iterator->mdb_cluster_index,
                                                                                &start_address));
                physical_entry_iterator->mdb_entry_index =
                    UTILEX_DIV_ROUND_UP((start_address * row_width), physical_entry_iterator->payload_basic_size);
            }
        }

        if (*is_end == FALSE)
        {
            entry->key[0] = physical_entry_iterator->mdb_entry_index;
            SHR_IF_ERR_EXIT(mdb_direct_table_entry_get(unit, dbal_physical_table_id, app_id, entry, 0));
            physical_entry_iterator->mdb_entry_index++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_clear(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id)
{
    dbal_physical_tables_e dbal_physical_table_id = physical_tables[0];
    int cluster_idx;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    dbal_enum_value_field_mdb_direct_payload_e payload_type;
    dbal_physical_entry_t entry;
    int nof_clusters = 0, start_address = 0, end_address = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id, &payload_type));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
    sal_memset(&entry, 0x0, sizeof(entry));
    entry.payload_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        uint32 entry_index;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        start_address.get(unit, mdb_physical_table_id, cluster_idx, &start_address));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                        end_address.get(unit, mdb_physical_table_id, cluster_idx, &end_address));

        entry_index = start_address * row_width / MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

        while (entry_index < end_address * row_width / MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type))
        {
            entry.key[0] = entry_index;

            SHR_IF_ERR_EXIT(mdb_direct_table_entry_delete(unit, physical_tables, app_id, &entry, 0));

            entry_index++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_entry_add(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE];
    mdb_cluster_direct_entry_info_t cluster_info;
    dbal_enum_value_field_mdb_direct_payload_e payload_type;
    int remainder_size;
    soc_mem_t mem;
    soc_reg_t overflow_reg;
    uint32 row_data[BITS2WORDS(MDB_ENTRY_BANK_SIZE_IN_BITS)];

    uint32 cluster_payload[BITS2WORDS(MDB_NOF_CLUSTER_ROW_BITS)];

    uint32 entry_payload[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    uint32 entry_index = entry->key[0];
    uint32 bucket_index;
    uint32 cluster_index_in_bucket;
    uint32 row_width;
    uint32 cluster_iter;
    uint32 row_offset;
    bsl_severity_t severity;

    uint32 table_entry_size;
    uint32 cluster_index;
    uint32 row_in_cluster;
    int blk;
    int overflow_reg_instance;
    int use_overflow_reg = FALSE;

    soc_reg_above_64_val_t overflow_row;
    mdb_macro_types_e macro_type;
    int macro_index;
    uint8 cache_enabled;

    uint32 payload_size_rounded_up;
    uint32 payload_size_rounded_up_diff;
    int tables_num = 0, table_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_num_valid_tables(unit, dbal_physical_table_id_arr, &tables_num));

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));

    cache_enabled &= ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL) ||
                      (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW));

    for (table_iter = 0; table_iter < tables_num; table_iter++)
    {
        mdb_physical_table_arr[table_iter] =
            dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id_arr[table_iter])->logical_to_physical;

        if (mdb_physical_table_arr[table_iter] == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. DBAL physical table %d is not associated with an MDB direct table.\n",
                         dbal_physical_table_id_arr[table_iter]);
        }
    }

    SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id_arr[0], &payload_type));

    table_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);
    payload_size_rounded_up = UTILEX_DIV_ROUND_UP(entry->payload_size, table_entry_size) * table_entry_size;
    payload_size_rounded_up_diff = payload_size_rounded_up - entry->payload_size;

    row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id_arr[0])->row_width;

    remainder_size = MDB_ENTRY_REMAINDER_CALC(entry_index, table_entry_size, row_width);

    if (payload_size_rounded_up > row_width)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. %s (app_id=%d): Payload size rounded up (%d rounded up to %d) is larger than row width (%d).\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id_arr[table_iter]), app_id,
                     entry->payload_size, payload_size_rounded_up, row_width);
    }

    SHR_IF_ERR_EXIT(mdb_direct_calc_cluster_info
                    (unit, row_width, payload_size_rounded_up, remainder_size, &cluster_info));

    sal_memset(entry_payload, 0x0, MDB_MAX_DIRECT_PAYLOAD_SIZE_32 * sizeof(entry_payload[0]));
    SHR_BITCOPY_RANGE(entry_payload, payload_size_rounded_up_diff, entry->payload, 0, entry->payload_size);

    for (table_iter = 0; table_iter < tables_num; table_iter++)
    {

        SHR_IF_ERR_EXIT(mdb_direct_table_calc_index_address
                        (unit, mdb_physical_table_arr[table_iter], entry_index, table_entry_size, row_width,
                         &mem, &overflow_reg, NULL, &row_in_cluster, &blk, &bucket_index, &cluster_index_in_bucket,
                         &macro_type, &macro_index, &overflow_reg_instance));

#ifndef ADAPTER_SERVER_MODE

        use_overflow_reg = (overflow_reg != INVALIDr)
            && ((cluster_info.clusters_offset[MDB_MAX_CLUSTER_PER_DB - 1] == MDB_CLUSTER_OFFSET_NEXT_ROW) ||
                ((cluster_info.active_bitmap == MDB_CLUSTER_BITMAP_FIRST)
                 && (row_width - remainder_size <
                     MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(dnx_data_mdb.pdbs.pdbs_info_get(unit,
                                                                                           mdb_physical_table_arr
                                                                                           [table_iter])->direct_max_payload_type))));
#endif

        if (use_overflow_reg)
        {
            sal_memset(&overflow_row, 0x0, sizeof(soc_reg_above_64_val_t));

            for (cluster_iter = 0; cluster_iter < MDB_CLUSTER_USE_TWO; cluster_iter++)
            {
                if (cluster_info.active_bitmap & (1 << cluster_iter))
                {
                    int cluster_index_offset = cluster_info.access_info[cluster_iter].cluster_offset;
                    row_offset =
                        mdb_direct_cluster_data_offset_in_bucket[cluster_index_offset] +
                        cluster_info.access_info[cluster_iter].cluster_row_offset;
                    SHR_BITCOPY_RANGE(overflow_row, row_offset, entry_payload,
                                      cluster_info.access_info[cluster_iter].entry_offset,
                                      cluster_info.access_info[cluster_iter].entry_size);

                    if (cache_enabled == TRUE)
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_cache_set
                                        (unit, macro_type, macro_index,
                                         (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) +
                                         cluster_index_in_bucket +
                                         cluster_info.access_info[cluster_iter].cluster_offset, row_in_cluster + 1,
                                         mdb_direct_cluster_data_offset_in_bucket[cluster_index_offset], overflow_row));
                    }
                }
            }

            if ((dnx_sbusdma_desc_is_enabled
                 (unit, mdb_direct_dbal_table_to_dma_enum(dbal_physical_table_id_arr[table_iter])) == TRUE)
                && (cache_enabled == TRUE))
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_reg(unit, overflow_reg, overflow_reg_instance,
                                                         (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) +
                                                          cluster_index_in_bucket) / MDB_MAX_CLUSTER_PER_DB,
                                                         overflow_row));
            }
            else
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set
                                (unit, overflow_reg, blk | SOC_REG_ADDR_BLOCK_ID_MASK,
                                 (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) +
                                  cluster_index_in_bucket) / MDB_MAX_CLUSTER_PER_DB, overflow_row));
            }
        }
        else
        {

            sal_memset(row_data, 0x0, (MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS) * sizeof(row_data[0]));

            if ((cluster_info.nof_used_clusters > MDB_CLUSTER_USE_ONE)
                || (table_entry_size != MDB_NOF_CLUSTER_ROW_BITS))
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "mdb_direct_table_entry_add: Performing read-modify-write.\n")));

                SHR_IF_ERR_EXIT(mdb_direct_table_read_entry_from_cluster
                                (unit, mem, macro_type, macro_index, bucket_index, cluster_index_in_bucket, blk,
                                 &cluster_info, entry->mdb_action_apply, row_in_cluster, row_data, cache_enabled));

            }

            for (cluster_iter = 0; cluster_iter < MDB_CLUSTER_USE_TWO; cluster_iter++)
            {
                if (cluster_info.active_bitmap & (1 << cluster_iter))
                {
                    cluster_index = cluster_index_in_bucket + cluster_info.access_info[cluster_iter].cluster_offset;

                    row_offset =
                        mdb_direct_cluster_data_offset_in_bucket[cluster_index] +
                        cluster_info.access_info[cluster_iter].cluster_row_offset;
                    sal_memset(cluster_payload, 0x0, sizeof(cluster_payload));

                    SHR_BITCOPY_RANGE(cluster_payload, 0, entry_payload,
                                      cluster_info.access_info[cluster_iter].entry_offset,
                                      cluster_info.access_info[cluster_iter].entry_size);

                    SHR_BITCOPY_RANGE(row_data, row_offset, cluster_payload, 0,
                                      cluster_info.access_info[cluster_iter].entry_size);

                    if (cache_enabled == TRUE)
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_cache_set
                                        (unit, macro_type, macro_index,
                                         (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) + cluster_index,
                                         row_in_cluster + cluster_info.clusters_offset[cluster_iter],
                                         mdb_direct_cluster_data_offset_in_bucket[cluster_index], row_data));
                    }
                }
            }

            SHR_IF_ERR_EXIT(mdb_direct_update_bucket_row_control
                            (unit, mem, macro_type, cluster_info.active_bitmap << cluster_index_in_bucket,
                             cluster_info.clusters_offset, row_data, &row_in_cluster));

            if ((dnx_sbusdma_desc_is_enabled
                 (unit, mdb_direct_dbal_table_to_dma_enum(dbal_physical_table_id_arr[table_iter])) == TRUE)
                && (cache_enabled == TRUE))
            {
                SHR_IF_ERR_EXIT(dnx_sbusdma_desc_add_mem(unit, mem, bucket_index, blk, row_in_cluster, row_data));
            }
            else
            {
                SHR_IF_ERR_EXIT(soc_mem_array_write(unit, mem, bucket_index, blk, row_in_cluster, row_data));
            }
        }
    }

#ifdef ADAPTER_SERVER_MODE
    if (dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_adapter_use_stub) &&
        ((dbal_physical_table_id_arr[0] < DBAL_PHYSICAL_TABLE_EEDB_1) ||
         (dbal_physical_table_id_arr[0] > DBAL_PHYSICAL_TABLE_EEDB_8)))
    {
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            SHR_IF_ERR_EXIT(adapter_mdb_access_direct_table_write
                            (unit, dbal_physical_table_id_arr[table_iter], app_id, entry));
        }

    }
#endif

    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        int index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_direct_table_entry_add: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "entry.key: %d. entry.payload_size: %d, entry->mdb_action_apply: 0x%x, tables_num: %d.\n"),
                     entry->key[0], entry->payload_size, entry->mdb_action_apply, tables_num));
        for (table_iter = 0; table_iter < tables_num; table_iter++)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "DBAL table %d: %s.\n"), table_iter,
                         dbal_physical_table_to_string(unit, dbal_physical_table_id_arr[table_iter])));
        }

        for (row_offset = 0; row_offset < BITS2WORDS(entry->payload_size); row_offset++)
        {
            uint32 print_index = BITS2WORDS(entry->payload_size) - 1 - row_offset;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry.payload[%d]: 0x%08x.\n"), print_index, entry->payload[print_index]));
        }

        if (use_overflow_reg)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "overflow_reg: %s. overflow_index: %d. blk: %d.\n"),
                         SOC_REG_NAME(unit, overflow_reg),
                         (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) +
                          cluster_index_in_bucket) / MDB_MAX_CLUSTER_PER_DB, blk));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "overflow_row: 0x")));
            for (index =
                 UTILEX_DIV_ROUND_UP(MDB_MAX_CLUSTER_PER_DB * MDB_NOF_CLUSTER_ROW_BITS, SAL_UINT32_NOF_BITS) - 1;
                 index > 0; index--)
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), overflow_row[index]));
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), overflow_row[0]));
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        }
        else
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Mem: %s. bucket_index: %d. blk: %d. row_in_cluster: %d. data_index: %d.\n"),
                         SOC_MEM_NAME(unit, mem), bucket_index, blk, row_in_cluster, cluster_index_in_bucket));
            for (index = 0; index < MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS; index++)
            {
                if ((index % MDB_ENTRY_DATA_PER_BANK == 0) && (index != 0))
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                }
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "row_data[%d]: 0x%08x. "), index, row_data[index]));
            }
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
        }

        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_direct_table_entry_add: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    mdb_cluster_direct_entry_info_t cluster_info;
    dbal_enum_value_field_mdb_direct_payload_e payload_type;
    int remainder_size;
    soc_mem_t mem;
    soc_reg_t overflow_reg;
    int overflow_reg_instance;
    int use_overflow_reg = FALSE;
    soc_reg_above_64_val_t overflow_row;

    soc_mem_t mem_hitbit[MDB_NOF_ABK_BANKS];
    uint32 row_data[MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS];
    uint32 entry_index = entry->key[0];
    uint32 bucket_index;
    uint32 row_width;
    uint32 cluster_index_in_bucket;
    uint32 cluster_iter;
    uint32 row_offset;

    uint32 table_entry_size;
    uint32 cluster_index;
    int blk;
    uint32 row_in_cluster;
    mdb_macro_types_e macro_type;
    int macro_index;
    bsl_severity_t severity;
    uint8 cache_enabled;

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE

    if (dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_adapter_use_stub))
    {
        SHR_IF_ERR_EXIT(adapter_mdb_access_direct_table_read(unit, dbal_physical_table_id, app_id, entry));
    }
    else
#endif
    {
        if (dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical ==
            DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error. DBAL physical table %d is not associated with an MDB direct table.\n",
                         dbal_physical_table_id);
        }

        SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id, &payload_type));

        table_entry_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);

        row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;

        SHR_IF_ERR_EXIT(mdb_direct_table_calc_index_address
                        (unit, mdb_physical_table, entry_index, table_entry_size, row_width,
                         &mem, &overflow_reg, mem_hitbit, &row_in_cluster, &blk, &bucket_index,
                         &cluster_index_in_bucket, &macro_type, &macro_index, &overflow_reg_instance));

        remainder_size = MDB_ENTRY_REMAINDER_CALC(entry_index, table_entry_size, row_width);

        SHR_IF_ERR_EXIT(mdb_direct_calc_cluster_info
                        (unit, row_width, entry->payload_size, remainder_size, &cluster_info));

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));
        cache_enabled &= ((entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_ALL) ||
                          (entry->mdb_action_apply == DBAL_MDB_ACTION_APPLY_SW_SHADOW));

#ifndef ADAPTER_SERVER_MODE

        use_overflow_reg = (overflow_reg != INVALIDr)
            && ((cluster_info.clusters_offset[MDB_MAX_CLUSTER_PER_DB - 1] == MDB_CLUSTER_OFFSET_NEXT_ROW) ||
                ((cluster_info.active_bitmap == MDB_CLUSTER_BITMAP_FIRST)
                 && (row_width - remainder_size <
                     MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(dnx_data_mdb.pdbs.pdbs_info_get(unit,
                                                                                           mdb_physical_table)->direct_max_payload_type))));
#endif

        if (use_overflow_reg)
        {
            sal_memset(&overflow_row, 0x0, sizeof(soc_reg_above_64_val_t));

            if (cache_enabled == TRUE)
            {
                for (cluster_iter = 0; cluster_iter < MDB_CLUSTER_USE_TWO; cluster_iter++)
                {
                    SHR_IF_ERR_EXIT(mdb_direct_cache_get
                                    (unit, macro_type, macro_index,
                                     (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit)) +
                                     cluster_index_in_bucket + cluster_info.access_info[cluster_iter].cluster_offset,
                                     row_in_cluster + 1, mdb_direct_cluster_data_offset_in_bucket[cluster_iter],
                                     overflow_row));
                }

            }
            else
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_get
                                (unit, overflow_reg, blk | SOC_REG_ADDR_BLOCK_ID_MASK,
                                 (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) +
                                  cluster_index_in_bucket) / MDB_MAX_CLUSTER_PER_DB, overflow_row));
            }

            for (cluster_iter = 0; cluster_iter < MDB_CLUSTER_USE_TWO; cluster_iter++)
            {
                if (cluster_info.active_bitmap & (1 << cluster_iter))
                {
                    cluster_index = cluster_info.access_info[cluster_iter].cluster_offset;
                    row_offset =
                        mdb_direct_cluster_data_offset_in_bucket[cluster_index] +
                        cluster_info.access_info[cluster_iter].cluster_row_offset;
                    SHR_BITCOPY_RANGE(entry->payload, cluster_info.access_info[cluster_iter].entry_offset, overflow_row,
                                      row_offset, cluster_info.access_info[cluster_iter].entry_size);
                }
            }
        }
        else
        {
            sal_memset(row_data, 0x0, (MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS) * sizeof(row_data[0]));

            SHR_IF_ERR_EXIT(mdb_direct_table_read_entry_from_cluster
                            (unit, mem, macro_type, macro_index, bucket_index, cluster_index_in_bucket, blk,
                             &cluster_info, entry->mdb_action_apply, row_in_cluster, row_data, cache_enabled));

            for (cluster_iter = 0; cluster_iter < MDB_CLUSTER_USE_TWO; cluster_iter++)
            {
                if (cluster_info.active_bitmap & (1 << cluster_iter))
                {
                    cluster_index = cluster_index_in_bucket + cluster_info.access_info[cluster_iter].cluster_offset;
                    row_offset =
                        mdb_direct_cluster_data_offset_in_bucket[cluster_index] +
                        cluster_info.access_info[cluster_iter].cluster_row_offset;
                    SHR_BITCOPY_RANGE(entry->payload, cluster_info.access_info[cluster_iter].entry_offset, row_data,
                                      row_offset, cluster_info.access_info[cluster_iter].entry_size);
                }
            }
        }

#ifndef ADAPTER_SERVER_MODE

        if (entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_ACTION)
        {
            mdb_direct_entry_hit_bit_index_e hit_bit_index;
            for (hit_bit_index = MDB_DIRECT_ENTRY_HIT_BIT_INDEX_1; hit_bit_index < MDB_DIRECT_ENTRY_NOF_HIT_BIT_INDEXS;
                 hit_bit_index++)
            {
                if (((hit_bit_index == MDB_DIRECT_ENTRY_HIT_BIT_INDEX_1)
                     && _SHR_IS_FLAG_SET(entry->hitbit, DBAL_PHYSICAL_KEY_HITBIT_PRIMARY))
                    || ((hit_bit_index == MDB_DIRECT_ENTRY_HIT_BIT_INDEX_2)
                        && _SHR_IS_FLAG_SET(entry->hitbit, DBAL_PHYSICAL_KEY_HITBIT_SECONDARY)))
                {
                    soc_reg_above_64_val_t hitbit_row;
                    int relevant_cluster;
                    int hitbit_offset;

                    int hitbit_abk_position = (((row_width - remainder_size) == MDB_DIRECT_BASIC_ENTRY_SIZE)
                                               && (hit_bit_index ==
                                                   MDB_DIRECT_ENTRY_HIT_BIT_INDEX_2)) ? -1 : dnx_data_mdb.
                        direct.hit_bit_pos_in_abk_get(unit, hit_bit_index)->hit_bit_position;
                    uint32 hit_bit_pos_on_cluster_1;
                    uint32 hitbit;
                    uint32 row_in_cluster_modified;
                    uint32 hit_bit_entry_remainder_size;
                    mdb_abk_banks_e bank_iter;
                    sal_memset(hitbit_row, 0, sizeof(hitbit_row));

                    hit_bit_entry_remainder_size = remainder_size + hitbit_abk_position * MDB_DIRECT_BASIC_ENTRY_SIZE;

                    hit_bit_pos_on_cluster_1 = (((hit_bit_entry_remainder_size % row_width) < MDB_NOF_CLUSTER_ROW_BITS)
                                                && (row_width > MDB_NOF_CLUSTER_ROW_BITS)) ? 1 : 0;

                    relevant_cluster = cluster_index_in_bucket + hit_bit_pos_on_cluster_1;

                    hitbit_offset = MDB_GET_HIT_BIT_OFFSET(hit_bit_entry_remainder_size);

                    row_in_cluster_modified =
                        (row_in_cluster +
                         (hit_bit_entry_remainder_size / row_width)) & MDB_DIRECT_ADDR_OFFSET_MASK(unit, macro_type);

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                    (&row_in_cluster_modified,
                                     MDB_DIRECT_ADDR_OFFSET_OFFSET(unit, macro_type),
                                     MDB_DIRECT_ADDR_OFFSET_OFFSET(unit, macro_type) +
                                     MDB_DIRECT_ADDR_OFFSET_BITS * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) - 1));

                    SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit
                                    (&row_in_cluster_modified,
                                     MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                                   macro_type) +
                                     MDB_DIRECT_ADDR_OFFSET_BITS * relevant_cluster + 1));

                    if (entry->hitbit & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
                    {

                        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                                        (hitbit_row,
                                         MDB_HITBIT_MASK_OFFSET(unit) + MDB_HITBIT_MASK_SIZE * relevant_cluster +
                                         hitbit_offset));
                    }

                    SHR_IF_ERR_EXIT(soc_mem_array_write
                                    (unit, mem_hitbit[MDB_ABK_BANK_A], bucket_index, blk, row_in_cluster_modified,
                                     hitbit_row));
                    SHR_IF_ERR_EXIT(soc_mem_array_write
                                    (unit, mem_hitbit[MDB_ABK_BANK_B], bucket_index, blk, row_in_cluster_modified,
                                     hitbit_row));

                    SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit
                                    (&row_in_cluster_modified,
                                     MDB_DIRECT_ADDR_OFFSET_OFFSET(unit,
                                                                   macro_type) +
                                     MDB_DIRECT_ADDR_OFFSET_BITS * relevant_cluster));

                    for (bank_iter = MDB_ABK_BANK_A; bank_iter < MDB_NOF_ABK_BANKS; bank_iter++)
                    {
                        SHR_IF_ERR_EXIT(soc_mem_array_read
                                        (unit, mem_hitbit[bank_iter], bucket_index, blk, row_in_cluster_modified,
                                         hitbit_row));
                        hitbit = 0;
                        SHR_BITCOPY_RANGE(&hitbit, 0, hitbit_row,
                                          MDB_HITBIT_DATA_OFFSET + MDB_HITBIT_DATA_SIZE * relevant_cluster +
                                          hitbit_offset, 1);
                        if (hitbit != 0)
                        {
                            entry->hitbit |= mdb_direct_hit_bit_accessed_indications[hit_bit_index][bank_iter];
                        }
                        else
                        {
                            entry->hitbit &= ~mdb_direct_hit_bit_accessed_indications[hit_bit_index][bank_iter];
                        }
                    }
                }

            }
        }
#endif
    }
    SHR_GET_SEVERITY_FOR_MODULE(severity);
    if (severity >= bslSeverityVerbose)
    {
        int payload_iter;
        int index;
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_direct_table_entry_get: start\n")));
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U
                     (unit,
                      "entry.key: %d. entry.payload_size: %d, entry->mdb_action_apply: 0x%x, physical_table: %s.\n"),
                     entry->key[0], entry->payload_size, entry->mdb_action_apply, dbal_physical_table_to_string(unit,
                                                                                                                dbal_physical_table_id)));
        for (payload_iter = 0; payload_iter < BITS2WORDS(entry->payload_size); payload_iter++)
        {
            uint32 print_index = BITS2WORDS(entry->payload_size) - 1 - payload_iter;
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "entry.payload[%d]: 0x%08X.\n"), print_index, entry->payload[print_index]));
        }
        if (!cache_enabled)
        {
            if (use_overflow_reg)
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "overflow_reg: %s. overflow_index: %d. blk: %d.\n"),
                             SOC_REG_NAME(unit, overflow_reg),
                             (bucket_index * dnx_data_mdb.dh.nof_bucket_clusters_get(unit) +
                              cluster_index_in_bucket) / MDB_MAX_CLUSTER_PER_DB, blk));
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "overflow_row: 0x")));
                for (index =
                     UTILEX_DIV_ROUND_UP(MDB_MAX_CLUSTER_PER_DB * MDB_NOF_CLUSTER_ROW_BITS, SAL_UINT32_NOF_BITS) - 1;
                     index > 0; index--)
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), overflow_row[index]));
                }
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%08x"), overflow_row[0]));
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
            }
            else
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U
                             (unit, "Mem: %s. bucket_index: %d. blk: %d. row_in_cluster: %d. data_index: %d.\n"),
                             SOC_MEM_NAME(unit, mem), bucket_index, blk, row_in_cluster, cluster_index_in_bucket));
                for (index = 0; index < MDB_ENTRY_BANK_SIZE_IN_BITS / SAL_UINT32_NOF_BITS; index++)
                {
                    if ((index % MDB_ENTRY_DATA_PER_BANK == 0) && (index != 0))
                    {
                        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                    }
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "row_data[%d]: 0x%08x. "), index, row_data[index]));
                }
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
            }
        }
        else
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload retrieved from cache.\n")));
        }
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "mdb_direct_table_entry_get: end\n")));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_entry_delete(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id_arr[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dbal_physical_entry_t temp_entry;

    SHR_FUNC_INIT_VARS(unit);

    sal_memcpy(&temp_entry, entry, sizeof(dbal_physical_entry_t));

    sal_memset(temp_entry.payload, 0x0, sizeof(temp_entry.payload));

    SHR_IF_ERR_EXIT(mdb_direct_table_entry_add(unit, dbal_physical_table_id_arr, app_id, &temp_entry, 0));

exit:
    SHR_FUNC_EXIT;
}

int
mdb_map_table_names(
    int unit,
    dbal_physical_tables_e table_name,
    char *adapter_table_name)
{
    int i;
    const dbal_physical_tables_e table_names[] =
        { DBAL_PHYSICAL_TABLE_INLIF_1, DBAL_PHYSICAL_TABLE_INLIF_2, DBAL_PHYSICAL_TABLE_INLIF_3,
        DBAL_PHYSICAL_TABLE_IVSI, DBAL_PHYSICAL_TABLE_MAP, DBAL_PHYSICAL_TABLE_FEC_1, DBAL_PHYSICAL_TABLE_FEC_2,
        DBAL_PHYSICAL_TABLE_FEC_3,
        DBAL_PHYSICAL_TABLE_EEDB_1, DBAL_PHYSICAL_TABLE_EEDB_2, DBAL_PHYSICAL_TABLE_EEDB_3,
        DBAL_PHYSICAL_TABLE_EEDB_4, DBAL_PHYSICAL_TABLE_EVSI
    };
    const char *adapter_table_names[] =
        { "InLif1", "InLif2", "InLif3", "Ivsd", "Map", "Fec1", "Fec2", "Fec3", "Eedb1", "Eedb2", "Eedb3", "Eedb4",
        "Evsd"
    };
    int nof_elements;

    SHR_FUNC_INIT_VARS(unit);

    adapter_table_name[0] = 0;
    nof_elements = sizeof(table_names) / sizeof(table_names[0]);

    for (i = 0; i < nof_elements; i++)
    {
        if (table_name == table_names[i])
        {
            sal_strcpy(adapter_table_name, adapter_table_names[i]);
            SHR_SET_CURRENT_ERR(_SHR_E_NONE);
            SHR_EXIT();
        }
    }

exit:

#ifndef ADAPTER_SERVER_MODE
    adapter_table_name[0] = 0;
#endif

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode,
    int full_time)
{
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE] = { 0 };
    dbal_physical_table_def_t *dbal_physical_table;
    dbal_physical_entry_t entry;
    uint32 row_data_write[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    uint32 row_data_read[MDB_MAX_DIRECT_PAYLOAD_SIZE_32];
    int entry_counter;
    int uint32_counter;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->logical_to_physical;
    uint32 entry_size;
    int entry_index_size;
    int index_factor;
    int entry_index, cluster_index;
#ifdef ADAPTER_SERVER_MODE
    uint32 tbl_entry_size;
    uint32 tbl_entry_data_allocated_size;
    char tbl_entry_data[ADAPTER_TABLE_ENTRY_MAX_SIZE];
#endif
    char adapter_table_name[MDB_TABLE_NAME_MAX_LENGTH];
    int row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    int nof_clusters = 0, start_address = 0, end_address = 0;
    uint32 add_total_time_usecs = 0, get_total_time_usecs = 0, del_total_time_usecs = 0, get_post_total_time_usecs = 0;
    uint32 total_entry_count = 0;
    uint32 timer_usec = 0;
    int op_iter;
    SHR_FUNC_INIT_VARS(unit);

    if (mdb_physical_table == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error. DBAL physical table %d is not associated with an MDB direct table.\n",
                     dbal_physical_table_id);
    }

    physical_tables[0] = dbal_physical_table_id;

    sal_memset(row_data_write, 0x0, (MDB_MAX_DIRECT_PAYLOAD_SIZE_32) * sizeof(row_data_write[0]));
    sal_memset(row_data_read, 0x0, (MDB_MAX_DIRECT_PAYLOAD_SIZE_32) * sizeof(row_data_read[0]));

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, dbal_physical_table_id, &dbal_physical_table));

    mdb_map_table_names(unit, dbal_physical_table_id, &(adapter_table_name[0]));

    entry_size = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->max_payload_size;

    SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size_using_logical_mdb_table
                    (unit, dbal_physical_table_id, &entry_index_size));

    index_factor =
        ((UTILEX_DIV_ROUND_UP(entry_size, MDB_DIRECT_BASIC_ENTRY_SIZE)) * MDB_DIRECT_BASIC_ENTRY_SIZE) /
        entry_index_size;

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters));

    for (op_iter = 0; op_iter < 4; op_iter++)
    {

        cluster_index = 0;
        entry_counter = 0;

        sal_srand(55555);

        if (full_time == TRUE)
        {
            timer_usec = sal_time_usecs();
        }

        while (cluster_index < nof_clusters)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            start_address.get(unit, mdb_physical_table, cluster_index, &start_address));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            end_address.get(unit, mdb_physical_table, cluster_index, &end_address));

            entry_index =
                UTILEX_DIV_ROUND_UP(start_address * row_width,
                                    ((UTILEX_DIV_ROUND_UP(entry_index_size, MDB_DIRECT_BASIC_ENTRY_SIZE)) *
                                     MDB_DIRECT_BASIC_ENTRY_SIZE));

            while (entry_index < UTILEX_DIV_ROUND_UP(end_address * row_width, ((UTILEX_DIV_ROUND_UP(entry_index_size,
                                                                                                    MDB_DIRECT_BASIC_ENTRY_SIZE))
                                                                               * MDB_DIRECT_BASIC_ENTRY_SIZE)))
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

                if (op_iter == 0)
                {
                    SHR_BITCOPY_RANGE(entry.payload, 0, row_data_write, 0, entry.payload_size);

                    if (full_time == FALSE)
                    {
                        timer_usec = sal_time_usecs();
                    }
                    SHR_IF_ERR_EXIT(mdb_direct_table_entry_add(unit, physical_tables, 0, &entry, 0));
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
                    SHR_IF_ERR_EXIT(mdb_direct_table_entry_get(unit, dbal_physical_table_id, 0, &entry, 0));
                    if (full_time == FALSE)
                    {
                        get_total_time_usecs += sal_time_usecs() - timer_usec;
                    }

#ifdef ADAPTER_SERVER_MODE
                    if ((adapter_table_name[0] != 0) &&
                        (!dnx_data_mdb.global.feature_get(unit, dnx_data_mdb_global_adapter_use_stub)))
                    {
                        int attempt_counter = 0;
                        tbl_entry_data_allocated_size = ADAPTER_TABLE_ENTRY_MAX_SIZE;

                        sal_memset(tbl_entry_data, 0x0, ADAPTER_TABLE_ENTRY_MAX_SIZE);
                        SHR_IF_ERR_EXIT(control_get_table_data
                                        (unit, &(adapter_table_name[0]), "mdb", entry_index,
                                         tbl_entry_data_allocated_size, &tbl_entry_size, &(tbl_entry_data[0])));

                        SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                        ((uint32 *) tbl_entry_data, entry.payload, BITS2WORDS(entry.payload_size)));

                        while ((utilex_bitstream_have_one((uint32 *) tbl_entry_data, BITS2WORDS(entry.payload_size))
                                == 1) && (attempt_counter < 10000))
                        {
                            attempt_counter++;

                            SHR_IF_ERR_EXIT(control_get_table_data
                                            (unit, &(adapter_table_name[0]), "mdb", entry_index,
                                             tbl_entry_data_allocated_size, &tbl_entry_size, &(tbl_entry_data[0])));

                            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                            ((uint32 *) tbl_entry_data, entry.payload, BITS2WORDS(entry.payload_size)));
                        }

                        if (utilex_bitstream_have_one((uint32 *) tbl_entry_data, BITS2WORDS(entry.payload_size)) == 1)
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                            ((uint32 *) tbl_entry_data, entry.payload, BITS2WORDS(entry.payload_size)));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Written data:\n 0x")));
                            for (uint32_counter = MDB_MAX_DIRECT_PAYLOAD_SIZE_32 - 1; uint32_counter >= 0;
                                 uint32_counter--)
                            {
                                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), entry.payload[uint32_counter]));
                            }
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));

                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Read data:\n 0x")));
                            for (uint32_counter = MDB_MAX_DIRECT_PAYLOAD_SIZE_32 - 1; uint32_counter >= 0;
                                 uint32_counter--)
                            {
                                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%08X"), tbl_entry_data[uint32_counter]));
                            }
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload size: %d\n"), entry.payload_size));
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_counter: %d\n"), entry_counter));

                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "Test failed, the value from mdb_direct_table_entry_get is not equal to the value of control_get_table_data.\n");
                        }
                    }
#endif

                    sal_memcpy(row_data_read, entry.payload,
                               (MDB_MAX_DIRECT_PAYLOAD_SIZE_32) * sizeof(row_data_read[0]));

                    SHR_IF_ERR_EXIT(utilex_bitstream_xor
                                    (entry.payload, row_data_write, MDB_MAX_DIRECT_PAYLOAD_SIZE_32));

                    if (utilex_bitstream_have_one_in_range(entry.payload, 0, entry.payload_size - 1))
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
                }

                if (op_iter == 2)
                {
                    if (full_time == FALSE)
                    {
                        timer_usec = sal_time_usecs();
                    }
                    SHR_IF_ERR_EXIT(mdb_direct_table_entry_delete(unit, physical_tables, 0, &entry, 0));
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
                    SHR_IF_ERR_EXIT(mdb_direct_table_entry_get(unit, dbal_physical_table_id, 0, &entry, 0));
                    if (full_time == FALSE)
                    {
                        del_total_time_usecs += sal_time_usecs() - timer_usec;
                    }

                    if (utilex_bitstream_have_one_in_range(entry.payload, 0, entry.payload_size - 1))
                    {
                        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
                        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Payload size: %d\n"), entry.payload_size));
                        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "entry_index: %d\n"), entry_index));
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
mdb_direct_table_get_direct_payload_type(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    dbal_enum_value_field_direct_payload_sizes_e * payload_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        if ((mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL) ||
            (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL))
        {

            *payload_type = DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B;
        }
        else
        {
            dbal_enum_value_field_mdb_eedb_phase_e phase_index = mdb_eedb_table_to_phase(unit, mdb_physical_table_id);

            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, phase_index, payload_type));
        }
    }
    else
    {
        *payload_type = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->direct_payload_type;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_get_basic_size(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    int *basic_size)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB
        || dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
    {
        dbal_enum_value_field_direct_payload_sizes_e payload_type;

        SHR_IF_ERR_EXIT(mdb_direct_table_get_direct_payload_type(unit, mdb_physical_table_id, &payload_type));

        *basic_size = MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type);
    }
    else
    {

        *basic_size = dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->row_width;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_get_basic_size_using_logical_mdb_table(
    int unit,
    dbal_physical_tables_e logic_table_id,
    int *basic_size)
{
    dbal_enum_value_field_mdb_physical_table_e physical_mdb_table;
    SHR_FUNC_INIT_VARS(unit);
    switch (logic_table_id)
    {
        case DBAL_PHYSICAL_TABLE_EEDB_1:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_2:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_3:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_4:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_5:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_6:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_7:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA;
            break;
        case DBAL_PHYSICAL_TABLE_EEDB_8:
            physical_mdb_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA;
            break;
        default:
            physical_mdb_table = dnx_data_mdb.pdbs.pdb_info_get(unit, logic_table_id)->logical_to_physical;
    }

    SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size(unit, physical_mdb_table, basic_size));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_table_get_row_width(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int *row_width)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_physical_table_id >= DBAL_NOF_PHYSICAL_TABLES)
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "%d is not a valid DBAL table ID.\n", dbal_physical_table_id);
    }

    *row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_direct_get_payload_type(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    dbal_enum_value_field_mdb_direct_payload_e * payload_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((dbal_physical_table_id >= DBAL_PHYSICAL_TABLE_EEDB_1)
        && (dbal_physical_table_id <= DBAL_PHYSICAL_TABLE_EEDB_8))
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.
                        eedb_payload_type.get(unit, dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1, payload_type));
    }
    else
    {
        *payload_type = dnx_data_mdb.direct.direct_info_get(unit, dbal_physical_table_id)->payload_type;
    }

exit:
    SHR_FUNC_EXIT;
}

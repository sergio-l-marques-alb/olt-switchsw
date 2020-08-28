/*
 * ! \file mdb.c $Id$ contains general SOC functiunalety of the MDB.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/mdb.h>
#include "mdb_internal.h"

/*
 * }
 */

/*
 * Defines
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

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

const dbal_physical_tables_e mdb_direct_mdb_to_dbal[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] = {
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1 */
    DBAL_PHYSICAL_TABLE_ISEM_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1 */
    DBAL_PHYSICAL_TABLE_INLIF_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI */
    DBAL_PHYSICAL_TABLE_IVSI,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2 */
    DBAL_PHYSICAL_TABLE_ISEM_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2 */
    DBAL_PHYSICAL_TABLE_INLIF_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3 */
    DBAL_PHYSICAL_TABLE_ISEM_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3 */
    DBAL_PHYSICAL_TABLE_INLIF_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM */
    DBAL_PHYSICAL_TABLE_LEM,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 */
    DBAL_PHYSICAL_TABLE_KAPS_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2 */
    DBAL_PHYSICAL_TABLE_KAPS_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1 */
    DBAL_PHYSICAL_TABLE_KAPS_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2 */
    DBAL_PHYSICAL_TABLE_KAPS_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0 */
    DBAL_PHYSICAL_TABLE_IOEM_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1 */
    DBAL_PHYSICAL_TABLE_IOEM_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP */
    DBAL_PHYSICAL_TABLE_MAP,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1 */
    DBAL_PHYSICAL_TABLE_FEC_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2 */
    DBAL_PHYSICAL_TABLE_FEC_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3 */
    DBAL_PHYSICAL_TABLE_FEC_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID */
    DBAL_PHYSICAL_TABLE_PPMC,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0 */
    DBAL_PHYSICAL_TABLE_GLEM_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1 */
    DBAL_PHYSICAL_TABLE_GLEM_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL */
    DBAL_PHYSICAL_TABLE_EEDB_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL */
    DBAL_PHYSICAL_TABLE_EEDB_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL */
    DBAL_PHYSICAL_TABLE_EEDB_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL */
    DBAL_PHYSICAL_TABLE_EEDB_4,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_4,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL */
    DBAL_PHYSICAL_TABLE_EEDB_5,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL */
    DBAL_PHYSICAL_TABLE_EEDB_5,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL */
    DBAL_PHYSICAL_TABLE_EEDB_6,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_6,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_5,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL */
    DBAL_PHYSICAL_TABLE_EEDB_7,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_7,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL */
    DBAL_PHYSICAL_TABLE_EEDB_8,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_8,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA */
    DBAL_PHYSICAL_TABLE_EEDB_7,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0 */
    DBAL_PHYSICAL_TABLE_EOEM_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1 */
    DBAL_PHYSICAL_TABLE_EOEM_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM */
    DBAL_PHYSICAL_TABLE_ESEM,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI */
    DBAL_PHYSICAL_TABLE_EVSI,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1 */
    DBAL_PHYSICAL_TABLE_SEXEM_1,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2 */
    DBAL_PHYSICAL_TABLE_SEXEM_2,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3 */
    DBAL_PHYSICAL_TABLE_SEXEM_3,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4 */
    DBAL_PHYSICAL_TABLE_LEXEM,
        /** DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP */
    DBAL_PHYSICAL_TABLE_RMEP_EM
};
/*
 * }
 */

shr_error_e
mdb_verify_macro_cluster_allocation(
    int unit,
    uint32 cluster_allocation)
{
    uint32 found_legal_allocation = FALSE;
    uint32 nof_alloc_bits;

    SHR_FUNC_INIT_VARS(unit);

    if (cluster_allocation >= (1 << dnx_data_mdb.dh.nof_macro_clusters_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The allocation 0x%x is invalid", cluster_allocation);
    }

    nof_alloc_bits = utilex_bitstream_get_nof_on_bits(&cluster_allocation, 1);

    if (!dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_bpu_setup_extended_support))
    {
        if ((nof_alloc_bits == 3) || (nof_alloc_bits == 7))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "The allocation 0x%02x is invalid", cluster_allocation);
        }
    }

    if ((nof_alloc_bits == 0) || (nof_alloc_bits == 1))
    {
        found_legal_allocation = TRUE;
    }
    else
    {
        uint32 position = 0;
        uint32 allocation_mask = 0;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(&allocation_mask, 0, nof_alloc_bits - 1));

        while ((position < dnx_data_mdb.dh.nof_macro_clusters_get(unit)) && !found_legal_allocation)
        {
            if (((allocation_mask << position) ^ cluster_allocation) == 0)
            {
                found_legal_allocation = TRUE;
            }

            if (nof_alloc_bits > dnx_data_mdb.dh.nof_bucket_clusters_get(unit))
            {
                break;
            }

            if (nof_alloc_bits % 2 == 0)
            {
                position += nof_alloc_bits;
            }
            else
            {
                position++;
            }
        }
    }

    if (found_legal_allocation == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The allocation 0x%02x is invalid", cluster_allocation);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_capacity(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    int *capacity)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table)->logical_to_physical;
    dbal_enum_value_field_mdb_direct_payload_e payload_type;
    uint32 row_width;
    dbal_enum_value_field_mdb_db_type_e mdb_db_type =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table)->db_type;
    uint32 row_factor;
    SHR_FUNC_INIT_VARS(unit);

    if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        int temp_capacity = 0;
        int cluster_idx;
        int nof_clusters;
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id_other;

        if (dbal_physical_table == DBAL_PHYSICAL_TABLE_KAPS_1)
        {
            mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
            mdb_physical_table_id_other = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
        }
        else
        {
            mdb_physical_table_id = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2;
            mdb_physical_table_id_other = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1;
        }

        row_width = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->row_width;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx += row_width / MDB_NOF_CLUSTER_ROW_BITS)
        {
            mdb_macro_types_e macro_type;
            int cluster_rows;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            macro_type.get(unit, mdb_physical_table_id, cluster_idx, &macro_type));

            SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, macro_type, &cluster_rows));
            temp_capacity += (cluster_rows * dnx_data_mdb.kaps.max_prefix_in_bb_row_get(unit));
        }

        if (temp_capacity == 0)
        {
            int nof_clusters_other;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            number_of_clusters.get(unit, mdb_physical_table_id_other, &nof_clusters_other));

            if (nof_clusters_other == 0)
            {
                uint32 num_of_small_bbs;
                uint32 requested_kaps_cfg = dnx_data_mdb.pdbs.mdb_profile_kaps_cfg_get(unit)->val;
                if (requested_kaps_cfg > dnx_data_mdb.kaps.nof_small_bbs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "%s (%d) in small KAPS configuration must be smaller or equal to the number of small BBs (%d)\n",
                                 spn_MDB_PROFILE_KAPS_CFG, requested_kaps_cfg,
                                 dnx_data_mdb.kaps.nof_small_bbs_get(unit));
                }
                if (dbal_physical_table == DBAL_PHYSICAL_TABLE_KAPS_1)
                {       /* Private DB */
                    num_of_small_bbs = dnx_data_mdb.kaps.nof_small_bbs_get(unit) - (requested_kaps_cfg - 1);
                }
                else if (dbal_physical_table == DBAL_PHYSICAL_TABLE_KAPS_2)
                {       /* Public DB */
                    num_of_small_bbs = requested_kaps_cfg - 1;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, " Error: unrecognized lpm dbal_physical_table: %d\n",
                                 dbal_physical_table);
                }

                temp_capacity = num_of_small_bbs *
                    dnx_data_mdb.kaps.nof_rows_in_small_bb_get(unit) * dnx_data_mdb.kaps.max_prefix_in_bb_row_get(unit);
            }
        }

        *capacity = temp_capacity;
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        int cluster_idx;
        int temp_capacity = 0;
        int nof_clusters;

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            mdb_macro_types_e macro_type;
            int cluster_rows;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                            macro_type.get(unit, mdb_physical_table_id, cluster_idx, &macro_type));

            SHR_IF_ERR_EXIT(mdb_eedb_cluster_type_to_rows(unit, macro_type, &cluster_rows));
            temp_capacity += (cluster_rows * MDB_NOF_CLUSTER_ROW_BITS / (MDB_DIRECT_BASIC_ENTRY_SIZE));
        }
#ifdef ADAPTER_SERVER_MODE
        *capacity = MDB_EM_ADAPTER_SIZE;
#else
        *capacity = temp_capacity + dnx_data_mdb.em.em_info_get(unit, dbal_physical_table)->ovf_cam_size;
#endif

    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_calc_capacity(unit, dbal_physical_table, capacity));
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
    {
        int cluster_idx;

        SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table, &payload_type));

        row_width = dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->row_width;
        row_factor = row_width / MDB_NOF_CLUSTER_ROW_BITS;

        if (payload_type != DBAL_NOF_ENUM_MDB_DIRECT_PAYLOAD_VALUES)
        {
            uint32 min_start_address = 0xFFFFFFFF, max_end_address = 0;
            int nof_clusters, start_address, end_address;
            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

            for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx += row_factor)
            {
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                start_address.get(unit, mdb_physical_table_id, cluster_idx, &start_address));
                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                end_address.get(unit, mdb_physical_table_id, cluster_idx, &end_address));

                if (start_address < min_start_address)
                {
                    min_start_address = start_address;
                }

                if (end_address > max_end_address)
                {
                    max_end_address = end_address;
                }
            }

            if (nof_clusters != 0)
            {
                *capacity = UTILEX_DIV_ROUND_UP((row_width * (max_end_address - min_start_address)),
                                                (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type)));
            }
            else
            {
                *capacity = 0;
            }
        }
        else
        {
            *capacity = 0;
        }
    }
    else if (dbal_physical_table == DBAL_PHYSICAL_TABLE_NONE)
    {
        *capacity = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "Error. dbal_physical_table %d is not associated with an MDB memory.\n",
                     dbal_physical_table);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_capacity_estimate(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    int key_size,
    int payload_size,
    int app_id,
    int app_id_size,
    int *capacity_estimate)
{
    dbal_enum_value_field_mdb_db_type_e mdb_db_type =
        dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->db_type;

    SHR_FUNC_INIT_VARS(unit);

    if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        mdb_em_entry_encoding_e entry_encoding;
        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, dbal_physical_table_id, key_size, payload_size, app_id, app_id_size, &entry_encoding));
        SHR_IF_ERR_EXIT(mdb_db_infos.em_entry_capacity_estimate.get(unit, dbal_physical_table_id, entry_encoding,
                                                                    capacity_estimate));
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        int key_size_threshold = 80;
        int ipv4_capacity = 0;
        int ipv6_capacity = 0;

#if defined(INCLUDE_KBP)
        SHR_IF_ERR_EXIT(mdb_lpm_get_ip_capacity(unit, dbal_physical_table_id, &ipv4_capacity, &ipv6_capacity));
#endif

        if (key_size < key_size_threshold)
        {
            *capacity_estimate = ipv4_capacity;
        }
        else
        {
            *capacity_estimate = ipv6_capacity;
        }
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
    {
        int capacity;
        dbal_enum_value_field_mdb_direct_payload_e payload_type;
        int table_entry_size;

        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, dbal_physical_table_id, &capacity));

        SHR_IF_ERR_EXIT(mdb_direct_get_payload_type(unit, dbal_physical_table_id, &payload_type));
        table_entry_size = (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type));

        *capacity_estimate = capacity / (UTILEX_DIV_ROUND_UP(payload_size, table_entry_size));
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        *capacity_estimate = 0;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_BADID, "Error. dbal_physical_table_id %d is not associated with an MDB memory.\n",
                     dbal_physical_table_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_macro_allocation(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_table_id,
    int macro_idx,
    mdb_macro_types_e macro_type,
    uint32 *macro_cluster_allocation)
{
    uint32 cluster_idx;
    int nof_clusters;
    mdb_cluster_info_t cluster_info;
    SHR_FUNC_INIT_VARS(unit);

    *macro_cluster_allocation = 0;

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_table_id, &nof_clusters));

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_table_id, cluster_idx, &cluster_info));

        if (cluster_info.macro_index == macro_idx && cluster_info.macro_type == macro_type)
        {
            *macro_cluster_allocation |= (1 << cluster_info.cluster_index);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_init_cluster_assoc(
    int unit)
{
    mdb_macro_types_e macro_type;
    int macro_idx;
    int cluster_idx;

    SHR_FUNC_INIT_VARS(unit);

    for (macro_type = 0; macro_type < MDB_NOF_MACRO_TYPES; macro_type++)
    {
        int max_macro, max_cluster;

        if (macro_type == MDB_MACRO_A)
        {
            max_macro = dnx_data_mdb.dh.nof_macroes_type_A_get(unit);
            max_cluster = dnx_data_mdb.dh.nof_macro_clusters_get(unit);
        }
        else if (macro_type == MDB_MACRO_B)
        {
            max_macro = dnx_data_mdb.dh.nof_macroes_type_B_get(unit);
            max_cluster = dnx_data_mdb.dh.nof_macro_clusters_get(unit);
        }
        else
        {
            max_macro = 1;
            max_cluster = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
        }

        for (macro_idx = 0; macro_idx < max_macro; macro_idx++)
        {
            for (cluster_idx = 0; cluster_idx < max_cluster; cluster_idx++)
            {
                SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                                (unit, macro_type, macro_idx, cluster_idx, DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.init(unit));
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.alloc(unit, MDB_MACRO_A,
                                                                             dnx_data_mdb.dh.
                                                                             nof_macroes_type_A_get(unit) *
                                                                             dnx_data_mdb.dh.
                                                                             nof_macro_clusters_get(unit)));
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                    macro_cluster_assoc.alloc(unit, MDB_MACRO_B,
                                              dnx_data_mdb.dh.nof_macroes_type_B_get(unit) *
                                              dnx_data_mdb.dh.nof_macro_clusters_get(unit)));
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                    macro_cluster_assoc.alloc(unit, MDB_EEDB_BANK, dnx_data_mdb.eedb.nof_eedb_banks_get(unit)));
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
mdb_pre_init_step(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_sw_state_init(unit));
    SHR_IF_ERR_EXIT(mdb_init_cluster_assoc(unit));

    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_alloc(unit));

    SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_alloc(unit));

    SHR_IF_ERR_EXIT(mdb_parse_xml_direct_physical_map(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_get_dh_block(
    int unit,
    uint32 global_macro_index,
    int *blk)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (dnx_data_mdb.dh.block_info_get(unit, global_macro_index)->block_type)
    {
        case DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHA:
            *blk = DDHA_BLOCK(unit, dnx_data_mdb.dh.block_info_get(unit, global_macro_index)->block_index);
            break;
        case DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DDHB:
            *blk = DDHB_BLOCK(unit, dnx_data_mdb.dh.block_info_get(unit, global_macro_index)->block_index);
            break;
        case DBAL_ENUM_FVAL_MDB_BLOCK_TYPES_DHC:
            *blk = DHC_BLOCK(unit, dnx_data_mdb.dh.block_info_get(unit, global_macro_index)->block_index);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error. Unexpected block type %d for global DH index %d.\n",
                         dnx_data_mdb.dh.block_info_get(unit, global_macro_index)->block_type, global_macro_index);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_table_is_allocated_get(
    int unit,
    dbal_physical_tables_e dbal_phy_table_id,
    uint8 *is_allocated)
{
    int nof_clusters;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table;

    SHR_FUNC_INIT_VARS(unit);

    mdb_physical_table = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_phy_table_id)->logical_to_physical;
    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table, &nof_clusters));
    *is_allocated = (nof_clusters > 0);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_dbal_table_is_mdb(
    int unit,
    dbal_tables_e dbal_logical_table_id,
    uint8 *is_mdb)
{
    dbal_access_method_e access_method;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_access_method_get(unit, dbal_logical_table_id, &access_method));

    if ((access_method != DBAL_ACCESS_METHOD_MDB)
        || (sal_strstr(dbal_logical_table_to_string(unit, dbal_logical_table_id), "EXAMPLE_") != NULL))
    {
        *is_mdb = FALSE;
    }
    else
    {
        *is_mdb = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

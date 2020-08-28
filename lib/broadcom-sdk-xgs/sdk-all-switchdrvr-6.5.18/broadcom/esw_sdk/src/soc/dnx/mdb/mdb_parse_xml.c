/*
 * ! \file mdb_parse_xml.c $Id$ Contains all of the MDB profile XML parsing capabilities.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Includes
 * {
 */

#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include "../dbal/dbal_internal.h"
#include <soc/dnx/mdb.h>
#include "mdb_internal.h"
#include "src/soc/dnx/mdb/auto_generated/dbx_pre_compiled_mdb_auto_generated_xml_parser.h"

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

#define MDB_NOF_PHY_DBS              38
#define MDB_XML_MAX_STRING_LENGTH    10
#define MDB_XML_INVALID_BUCKET_DEPTH -1

#define MDB_HW_ALLOCATION_DIR_PATH "mdb"

#define MDB_FILE_PATH                   "jericho_2"

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define MDB_DH_IS_FULLY_SUPPORTS_FLEX(unit,mdb_table) (dnx_data_mdb.em.flex_fully_supported_get(unit) && MDB_DH_IS_TABLE_SUPPORT_FLEX(unit,mdb_table))
#define MDB_IS_ODD(number) (number & 0x1)
#define MDB_GET_NEXT_BANK_POINTER(last_alloc_bank_plus_one,skip_odd) (last_alloc_bank_plus_one + ( skip_odd*MDB_IS_ODD(last_alloc_bank_plus_one)))

/*
 * }
 */

/*
 * ENUMs
 * {
 */
typedef enum
{
    MDB_DH_INTERFACE_FSM_NO_CLUSTER_WAS_FOUND = 0,
    MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_FIRST_CLUSTER_FOUND = 1,
    MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_LAST_CLUSTER_FOUND = 2,
    MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND = 3,
    MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_LAST_CLUSTER_FOUND = 4
} mdb_parse_xml_second_interface_fsm_e;
/*
 * }
 */

/*
 * Structures
 * {
 */
typedef struct mdb_parse_xml_fec_hier_alloc_info_s
{
    int cluster_pair_count;
    uint8 valid_alloc;
    uint32 first_bank;
    uint32 nof_banks;
    uint8 high_granularity_required;
    mdb_macro_types_e cluster_type[DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS *
                                   DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];
} mdb_parse_xml_fec_hier_alloc_info_t;

/*
 * }
 */

/*
 * globals
 * {
 */
const int mdb_parse_xml_intf_1_em_120_clusters_division[DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS] =
    { 1, 1, 2, 2, 4, 4, 0, 4 };
/*
 * }
 */

static shr_error_e
mdb_set_clusters_in_swstate(
    int unit,
    int allocation_size,
    int cluster_count,
    mdb_cluster_info_t * clusters,
    dbal_enum_value_field_mdb_physical_table_e current_mdb_table)
{
    int cluster_iter;
    SHR_FUNC_INIT_VARS(unit);
    if (allocation_size > 0 && !SOC_WARM_BOOT(unit))
    {
        allocation_size = UTILEX_MAX(allocation_size, cluster_count);
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.alloc(unit, current_mdb_table, allocation_size));

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_alloc_clusters.set(unit, current_mdb_table, allocation_size));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, current_mdb_table, cluster_count));

        for (cluster_iter = 0; cluster_iter < cluster_count; cluster_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            clusters_info.set(unit, current_mdb_table, cluster_iter, &clusters[cluster_iter]));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_update_fec_address(
    int unit)
{

    static dbal_enum_value_field_mdb_physical_table_e fecs_tables[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3
    };

    mdb_parse_xml_fec_hier_alloc_info_t hier_info[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES];
    uint32 macro_b_dh_count[DNX_DATA_MAX_MDB_DH_NOF_MACROES_TYPE_B];
    mdb_cluster_info_t clusters_info;
    uint32 cluster_iter, fec_hier_iter;
    uint32 total_fec_banks = 0;
    uint32 max_fecs_allowed;
    uint32 bank_top_pointer;
    int bank_bottom_pointer;
    uint32 high_granularity_required_for_one_or_more_hier = 0;
    SHR_FUNC_INIT_VARS(unit);

    bank_top_pointer = dnx_data_l3.fec.max_nof_banks_get(unit) - 1;

    sal_memset(hier_info, 0, DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES * sizeof(hier_info[0]));

    for (fec_hier_iter = 0; fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        int dhb_iter;
        uint32 fec_cluster_alloc_granularity = dnx_data_mdb.dh.dh_info_get(unit,
                                                                           fecs_tables[fec_hier_iter])->row_width /
            dnx_data_mdb.dh.cluster_row_width_bits_get(unit);
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, fecs_tables[fec_hier_iter],
                                                               &hier_info[fec_hier_iter].cluster_pair_count));

        hier_info[fec_hier_iter].cluster_pair_count /= fec_cluster_alloc_granularity;

        sal_memset(macro_b_dh_count, 0, DNX_DATA_MAX_MDB_DH_NOF_MACROES_TYPE_B * sizeof(macro_b_dh_count[0]));

        for (cluster_iter = 0; cluster_iter < hier_info[fec_hier_iter].cluster_pair_count; cluster_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, fecs_tables[fec_hier_iter],
                                                              cluster_iter *
                                                              fec_cluster_alloc_granularity, &clusters_info));

            hier_info[fec_hier_iter].cluster_type[cluster_iter] = clusters_info.macro_type;

            if (hier_info[fec_hier_iter].cluster_type[cluster_iter] == MDB_MACRO_A)
            {
                hier_info[fec_hier_iter].nof_banks += 2;
            }
            else
            {
                hier_info[fec_hier_iter].nof_banks++;
                macro_b_dh_count[clusters_info.cluster_index]++;
            }

        }
        for (dhb_iter = 0; dhb_iter < dnx_data_mdb.dh.nof_macroes_type_B_get(unit); dhb_iter++)
        {
            hier_info[fec_hier_iter].high_granularity_required |= macro_b_dh_count[dhb_iter] & 0x1;
        }

        high_granularity_required_for_one_or_more_hier |= hier_info[fec_hier_iter].high_granularity_required;

        total_fec_banks += hier_info[fec_hier_iter].nof_banks;
    }

    bank_bottom_pointer = dnx_data_l3.fec.first_bank_without_id_alloc_get(unit);

    max_fecs_allowed = dnx_data_l3.fec.max_fec_id_for_single_dhb_cluster_pair_granularity_get(unit);
    for (fec_hier_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2;
         fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        if (!hier_info[fec_hier_iter].high_granularity_required)
        {
            bank_top_pointer -= hier_info[fec_hier_iter].nof_banks;
            hier_info[fec_hier_iter].first_bank = bank_top_pointer + 1;
            hier_info[fec_hier_iter].valid_alloc = TRUE;
            max_fecs_allowed = dnx_data_l3.fec.max_fec_id_for_double_dhb_cluster_pair_granularity_get(unit);
        }
    }
    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.set(unit, max_fecs_allowed));
    if (high_granularity_required_for_one_or_more_hier)
    {
        bank_top_pointer =
            UTILEX_MIN(bank_top_pointer,
                       (dnx_data_l3.fec.max_fec_id_for_single_dhb_cluster_pair_granularity_get(unit) /
                        dnx_data_l3.fec.bank_size_get(unit)) - 1);
    }
    while (bank_bottom_pointer >= 0)
    {
        uint32 bank_bottom_pointer_tmp;
        uint32 odd_indication, odd_even_iter;

        hier_info[DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1].first_bank = (MDB_IS_ODD(bank_bottom_pointer)
                                                                                  &&
                                                                                  (!MDB_IS_ODD
                                                                                   (hier_info
                                                                                    [DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1].nof_banks)))
            ? bank_bottom_pointer + 1 : bank_bottom_pointer;

        bank_bottom_pointer_tmp =
            MDB_GET_NEXT_BANK_POINTER((hier_info[DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1].first_bank +
                                       hier_info[DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1].nof_banks),
                                      dnx_data_l3.hw_bug.feature_get(unit,
                                                                     dnx_data_l3_hw_bug_fer_fec_granularity_double_size));

        odd_indication = MDB_IS_ODD(bank_bottom_pointer_tmp);

        for (odd_even_iter = 0; odd_even_iter < 2; odd_even_iter++)
        {
            for (fec_hier_iter = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2;
                 fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
            {
                if ((!hier_info[fec_hier_iter].valid_alloc)
                    && (MDB_IS_ODD(hier_info[fec_hier_iter].nof_banks) == odd_indication))
                {

                    hier_info[fec_hier_iter].first_bank = bank_bottom_pointer_tmp;

                    bank_bottom_pointer_tmp =
                        MDB_GET_NEXT_BANK_POINTER((hier_info[fec_hier_iter].first_bank +
                                                   hier_info[fec_hier_iter].nof_banks),
                                                  dnx_data_l3.hw_bug.feature_get(unit,
                                                                                 dnx_data_l3_hw_bug_fer_fec_granularity_double_size));
                }
            }

            odd_indication = 1 - odd_indication;
            if ((odd_even_iter == 0) && MDB_IS_ODD(bank_bottom_pointer_tmp) && !odd_indication)
            {
                bank_bottom_pointer_tmp++;
            }
        }
        if ((bank_bottom_pointer_tmp - 1) >= bank_top_pointer)
        {
            bank_bottom_pointer--;
        }
        else
        {
            break;
        }
    }

    if (bank_bottom_pointer < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Couldn't find a valid FEC allocation.\n");
    }

    for (fec_hier_iter = 0; fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        uint32 not_mcaro_types = 2;
        uint32 macro_type_iter, cluster_iter;
        uint32 start_with_macro_B;
        uint32 bank_pointer;
        uint32 fec_cluster_alloc_granularity = dnx_data_mdb.dh.dh_info_get(unit,
                                                                           fecs_tables[fec_hier_iter])->row_width /
            dnx_data_mdb.dh.cluster_row_width_bits_get(unit);

        bank_pointer = hier_info[fec_hier_iter].first_bank;
        start_with_macro_B = (bank_pointer & 0x1);
        for (macro_type_iter = 0; macro_type_iter < not_mcaro_types; macro_type_iter++)
        {
            mdb_macro_types_e current_macro_type =
                (((macro_type_iter + start_with_macro_B) & 0x1) == 0) ? MDB_MACRO_A : MDB_MACRO_B;
            for (cluster_iter = 0; cluster_iter < hier_info[fec_hier_iter].cluster_pair_count; cluster_iter++)
            {
                if (hier_info[fec_hier_iter].cluster_type[cluster_iter] == current_macro_type)
                {
                    int fec_cluster_iter;
                    int step_count = (current_macro_type == MDB_MACRO_A) ? 2 : 1;
                    int pair_cluster_idx;
                    int start_address;
                    int end_address;

                    pair_cluster_idx = (cluster_iter * fec_cluster_alloc_granularity);
                    start_address = bank_pointer * dnx_data_mdb.dh.nof_rows_in_macro_b_cluster_get(unit);
                    end_address = (bank_pointer + step_count) * dnx_data_mdb.dh.nof_rows_in_macro_b_cluster_get(unit);

                    for (fec_cluster_iter = 0; fec_cluster_iter < fec_cluster_alloc_granularity; fec_cluster_iter++)
                    {
                        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                        start_address.set(unit, fecs_tables[fec_hier_iter],
                                                          pair_cluster_idx + fec_cluster_iter, start_address));
                        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                        end_address.set(unit, fecs_tables[fec_hier_iter],
                                                        pair_cluster_idx + fec_cluster_iter, end_address));
                    }

                    bank_pointer += step_count;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_update_address_map(
    int unit)
{
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id_iter;

    SHR_FUNC_INIT_VARS(unit);

    for (mdb_physical_table_id_iter = 0; mdb_physical_table_id_iter < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
         mdb_physical_table_id_iter++)
    {
        if ((dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id_iter)->db_type ==
             DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
            || (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id_iter)->db_type ==
                DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
        {
            int nof_clusters;
            int table_cluster_idx;
            uint32 row_width =
                dnx_data_mdb.pdbs.pdb_info_get(unit, mdb_direct_mdb_to_dbal[mdb_physical_table_id_iter])->row_width;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id_iter, &nof_clusters));

            for (table_cluster_idx = 0; table_cluster_idx < nof_clusters;
                 table_cluster_idx += row_width / MDB_NOF_CLUSTER_ROW_BITS)
            {
                mdb_cluster_info_t cluster_info;

                SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                clusters_info.get(unit, mdb_physical_table_id_iter, table_cluster_idx, &cluster_info));

                if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id_iter)->db_type ==
                    DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
                {
                    SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set
                                    (unit, mdb_physical_table_id_iter, cluster_info.start_address,
                                     cluster_info.end_address, table_cluster_idx));
                }
                else if (cluster_info.start_address != MDB_DIRECT_INVALID_START_ADDRESS)
                {
                    dbal_enum_value_field_mdb_eedb_phase_e eedb_phase;
                    dbal_enum_value_field_mdb_direct_payload_e payload_size_type;
                    int logical_addr_factor;

                    eedb_phase = mdb_eedb_table_to_phase(unit, mdb_physical_table_id_iter);
                    SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, eedb_phase, &payload_size_type));
                    logical_addr_factor =
                        MDB_NOF_CLUSTER_ROW_BITS / MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_size_type);

                    SHR_IF_ERR_EXIT(mdb_eedb_address_mapping_set
                                    (unit, mdb_physical_table_id_iter, logical_addr_factor * cluster_info.start_address,
                                     logical_addr_factor * cluster_info.end_address, table_cluster_idx));

                    if ((mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
                        || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
                        || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
                        || (mdb_physical_table_id_iter == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA))
                    {
                        SHR_IF_ERR_EXIT(mdb_direct_address_mapping_set
                                        (unit, mdb_physical_table_id_iter, cluster_info.start_address,
                                         cluster_info.end_address, table_cluster_idx));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_get_dh_info(
    int unit,
    int bucket_width,
    int cluster_index,
    char *dh_name,
    dbal_enum_value_field_mdb_physical_table_e current_mdb_table,
    mdb_cluster_info_t * clusters,
    int *cluster_count)
{
    uint8 is_dha;
    char dh_name_compare[MDB_XML_MAX_STRING_LENGTH], dh_name_compare_iter[MDB_XML_MAX_STRING_LENGTH];
    int macro_idx, macro_idx_max;
    int nof_clusters = bucket_width / MDB_NOF_CLUSTER_ROW_BITS;
    int cluster_pair_idx;
    SHR_FUNC_INIT_VARS(unit);

    is_dha = sal_strncmp(dh_name, "dha", sizeof("dha") - 1) == 0 ? 1 : 0;
    if (is_dha == 1)
    {
        macro_idx_max = dnx_data_mdb.dh.nof_macroes_type_A_get(unit);
        sal_strcpy(dh_name_compare, "dha");
    }
    else
    {
        macro_idx_max = dnx_data_mdb.dh.nof_macroes_type_B_get(unit);
        sal_strcpy(dh_name_compare, "dhb");
    }

    for (macro_idx = 0; macro_idx < macro_idx_max; macro_idx++)
    {
        sal_sprintf(dh_name_compare_iter, "%s%d", dh_name_compare, macro_idx);
        if (0 == sal_strcmp(dh_name_compare_iter, dh_name))
        {
            break;
        }
    }

    if (macro_idx == macro_idx_max)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized dh: %s. Last DH attempted: %s.\n", dh_name, dh_name_compare_iter);
    }

    for (cluster_pair_idx = 0; cluster_pair_idx < nof_clusters; cluster_pair_idx++)
    {
        uint8 cache_enabled;

        mdb_cluster_info_t *clusters_p = &clusters[*cluster_count];

        sal_memset(clusters_p, 0, sizeof(mdb_cluster_info_t));
        (*cluster_count)++;

        clusters_p->macro_type = (is_dha == 1) ? MDB_MACRO_A : MDB_MACRO_B;
        clusters_p->macro_index = macro_idx;
        clusters_p->cluster_index = cluster_index + cluster_pair_idx;
        clusters_p->cluster_position_in_entry = cluster_pair_idx;

        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_half_nof_clusters_enable))
        {
            if ((clusters_p->macro_type == MDB_MACRO_A)
                && (clusters_p->cluster_index >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Device does not support macro A %d utilization of cluster id %d (max %d).",
                             clusters_p->macro_index, clusters_p->cluster_index,
                             dnx_data_mdb.dh.nof_bucket_clusters_get(unit) - 1);
            }
        }

        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_75_nof_clusters_enable))
        {
            if ((clusters_p->macro_type == MDB_MACRO_A)
                && ((dnx_data_mdb.dh.mdb_75_macro_halved_get(unit, clusters_p->macro_index))->macro_halved == TRUE)
                && (clusters_p->cluster_index >= dnx_data_mdb.dh.nof_bucket_clusters_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Device does not support macro A %d utilization of cluster id %d (max %d).",
                             clusters_p->macro_index, clusters_p->cluster_index,
                             dnx_data_mdb.dh.nof_bucket_clusters_get(unit) - 1);
            }
        }

        SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc
                        (unit, clusters_p->macro_type, clusters_p->macro_index,
                         clusters_p->cluster_index, current_mdb_table));

        SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));
        if (cache_enabled == TRUE)
        {
            if ((dnx_data_mdb.pdbs.pdbs_info_get(unit, current_mdb_table)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
                || (dnx_data_mdb.pdbs.pdbs_info_get(unit, current_mdb_table)->db_type ==
                    DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB))
            {
                SHR_IF_ERR_EXIT(mdb_direct_cache_alloc
                                (unit, clusters_p->macro_type, macro_idx, cluster_index + cluster_pair_idx));
            }
        }

        if ((current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
            || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
            || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
            || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA))
        {
            clusters_p->start_address = MDB_DIRECT_INVALID_START_ADDRESS;
            clusters_p->end_address = MDB_DIRECT_INVALID_START_ADDRESS;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mdb_parse_xml_em_banks_map(
    int unit,
    void *curTop)
{
    void *cur_phy_map, *cur_phydb;
    int phydb_name_idx;

    char *phydb_names[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] =
        { "eoem_0", "eoem_1", "esem", "glem_0", "glem_1", "ioem_0", "ioem_1", "isem_1", "isem_2", "isem_3", "lem",
        "mc_id", "rmep", "exem_1", "exem_2", "exem_3", "exem_4"
    };
    dbal_enum_value_field_mdb_physical_table_e mdb_table_names[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_0, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EOEM_1,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ESEM,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_0, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_GLEM_1,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_0, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IOEM_1,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_1, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_2,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ISEM_3, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_LEM,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MC_ID, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_RMEP,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_1, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_2,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_3, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EXEM_4
    };

    SHR_FUNC_INIT_VARS(unit);

    for (phydb_name_idx = 0; phydb_name_idx < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES; phydb_name_idx++)
    {
        if (phydb_names[phydb_name_idx] != NULL)
        {
            dbal_enum_value_field_mdb_physical_table_e em_table = mdb_table_names[phydb_name_idx];
            mdb_cluster_info_t clusters[DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS *
                                        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];
            int cluster_count = 0;
            int nof_cluster_per_entry = 0;
            int bucket_width;

            cur_phydb = dbx_xml_child_get_first(curTop, phydb_names[phydb_name_idx]);

            if (cur_phydb == NULL)
            {
                continue;
            }

            RHDATA_GET_INT_STOP(cur_phydb, "bucket_width", bucket_width);

            sal_memset(clusters, 0,
                       DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS * DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS *
                       sizeof(mdb_cluster_info_t));

            RHDATA_ITERATOR(cur_phy_map, cur_phydb, "physical_map")
            {
                char dh_name[MDB_XML_MAX_STRING_LENGTH];
                int cluster_idx;

                RHDATA_GET_INT_STOP(cur_phy_map, "cluster0_idx", cluster_idx);
                RHDATA_GET_STR_STOP(cur_phy_map, "dh", dh_name);

                SHR_IF_ERR_EXIT(mdb_parse_xml_get_dh_info
                                (unit, bucket_width, cluster_idx, dh_name, em_table, clusters, &cluster_count));

                nof_cluster_per_entry = bucket_width / MDB_NOF_CLUSTER_ROW_BITS;
            }

            SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate(unit, cluster_count, cluster_count, clusters, em_table));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.set(unit, em_table, nof_cluster_per_entry));

        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_mdb_clusters_map(
    int unit,
    void *curTop)
{
    int res = 0;
    int phydb_name_idx;
    void *cur_phydb, *cur_phy_map;

    char *phydb_names[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] =
        { "eedb_1", "eedb_2", "eedb_3", "eedb_4", "evsi", "fec_1", "fec_2", "fec_3", "inlif_1", "inlif_2",
        "inlif_3", "ivsi", "ads_1", "ads_2", "map", "kaps_1", "kaps_2"
    };
    dbal_enum_value_field_mdb_physical_table_e mdb_table_names[DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EVSI,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_1,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_2, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_INLIF_3,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_IVSI, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_MAP, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2
    };

    SHR_FUNC_INIT_VARS(unit);

    for (phydb_name_idx = 0; phydb_name_idx < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES; phydb_name_idx++)
    {
        if (phydb_names[phydb_name_idx] != NULL)
        {
            int bucket_width, nof_cluster_to_allocate, cluster_count = 0;
            dbal_enum_value_field_mdb_physical_table_e current_mdb_table = mdb_table_names[phydb_name_idx];
            mdb_cluster_info_t clusters[DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS *
                                        DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];
            cur_phydb = dbx_xml_child_get_first(curTop, phydb_names[phydb_name_idx]);

            if (cur_phydb == NULL)
            {
                continue;
            }

            if ((current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1)
                || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2))
            {
                int requested_kaps_cfg;
                int mdb_profile_kaps_cfg;

                requested_kaps_cfg = dnx_data_mdb.pdbs.mdb_profile_kaps_cfg_get(unit)->val;
                RHDATA_GET_INT_STOP(cur_phydb, "cfg", mdb_profile_kaps_cfg);

                while ((requested_kaps_cfg != mdb_profile_kaps_cfg) && (mdb_profile_kaps_cfg != 0))
                {
                    cur_phydb = dbx_xml_child_get_next(cur_phydb);
                    if (cur_phydb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "Unsupported MDB profile and cfg (%d) combination. Max kaps cfg for selected profile is %d.",
                                     requested_kaps_cfg, mdb_profile_kaps_cfg);
                    }
                    RHDATA_GET_INT_STOP(cur_phydb, "cfg", mdb_profile_kaps_cfg);
                }
            }

            sal_memset(clusters, 0,
                       DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS * DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS *
                       sizeof(mdb_cluster_info_t));

            RHDATA_GET_INT_STOP(cur_phydb, "bucket_width", bucket_width);
            SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.set(unit, current_mdb_table,
                                                                      bucket_width / MDB_NOF_CLUSTER_ROW_BITS));

            RHDATA_ITERATOR(cur_phy_map, cur_phydb, "physical_map")
            {
                char dh_name[MDB_XML_MAX_STRING_LENGTH];
                int cluster_idx;

                RHDATA_GET_INT_STOP(cur_phy_map, "cluster0_idx", cluster_idx);
                RHDATA_GET_STR_STOP(cur_phy_map, "dh", dh_name);

                SHR_IF_ERR_EXIT(mdb_parse_xml_get_dh_info
                                (unit, bucket_width, cluster_idx, dh_name, current_mdb_table,
                                 clusters, &cluster_count));
            }

            nof_cluster_to_allocate = cluster_count;
            if (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_1
                || current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_2
                || current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_FEC_3
                || current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA
                || current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA)
            {
                nof_cluster_to_allocate += dnx_data_mdb.dh.nof_macro_clusters_get(unit);
            }

            mdb_set_clusters_in_swstate(unit, nof_cluster_to_allocate, cluster_count, clusters, current_mdb_table);
        }
    }

exit:
    if (res)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("XML PARSING ERROR\n")));
        SHR_SET_CURRENT_ERR(res);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_eedb_banks_map(
    int unit,
    void *curTop)
{
    int res = 0;
    void *eedb_phydb, *current_eedb_phydb;
    int address_granularity[DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES];
    int phase_iterator;
    dbal_enum_value_field_mdb_eedb_phase_e phase_iter;
    mdb_cluster_info_t cluster;
    char *eedb_phase_names[] =
        { "eedb_1_0", "eedb_1_1", "eedb_2_0", "eedb_2_1", "eedb_3_0", "eedb_3_1", "eedb_4_0", "eedb_4_1" };
    dbal_enum_value_field_mdb_physical_table_e mdb_phase_names_ll[] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL
    };
    dbal_enum_value_field_mdb_physical_table_e mdb_phase_names_data[] =
        { DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_DATA, DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_DATA,
        DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_DATA
    };
    int bank_iter;
    uint8 cache_enabled;

    SHR_FUNC_INIT_VARS(unit);

    eedb_phydb = dbx_xml_child_get_first(curTop, "eedb_resources");

    for (phase_iterator = 0; phase_iterator < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES; phase_iterator++)
    {
        address_granularity[phase_iterator] =
            dnx_data_mdb.eedb.outlif_physical_phase_granularity_get(unit, phase_iterator)->data_granularity;
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));

    if (cache_enabled == TRUE)
    {
        int nof_eedb_banks = dnx_data_mdb.eedb.nof_eedb_banks_get(unit);
        for (bank_iter = 0; bank_iter < nof_eedb_banks; bank_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_direct_cache_alloc(unit, MDB_EEDB_BANK, 0, bank_iter));
        }
    }

    for (phase_iter = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1; phase_iter < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
         phase_iter++)
    {
        mdb_cluster_info_t clusters[DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS *
                                    DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS];

        SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.set(unit, phase_iter,
                                                           MDB_PAYLOAD_SIZE_TO_PAYLOAD_SIZE_TYPE(address_granularity
                                                                                                 [phase_iter])));

        sal_memset(clusters, 0,
                   DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS * DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS *
                   sizeof(mdb_cluster_info_t));

        for (bank_iter = 0; bank_iter < dnx_data_mdb.eedb.phase_info_get(unit, phase_iter)->nof_available_banks;
             bank_iter++)
        {
            clusters[bank_iter].macro_type = MDB_NOF_MACRO_TYPES;
        }

        SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate
                        (unit, dnx_data_mdb.eedb.phase_info_get(unit, phase_iter)->nof_available_banks,
                         0, NULL, mdb_phase_names_ll[phase_iter]));
        SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate
                        (unit, dnx_data_mdb.eedb.phase_info_get(unit, phase_iter)->nof_available_banks,
                         dnx_data_mdb.eedb.phase_info_get(unit, phase_iter)->nof_available_banks,
                         clusters, mdb_phase_names_data[phase_iter]));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_phase_names_data[phase_iter], 0));

    }

    RHDATA_ITERATOR(current_eedb_phydb, eedb_phydb, "lldb_mem_block")
    {
        int idx;
        int is_data;
        char mag[MDB_XML_MAX_STRING_LENGTH];
        int offset;
        int eedb_phase_index;
        int nof_clusters;
        dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;

        RHDATA_GET_INT_STOP(current_eedb_phydb, "idx", idx);
        RHDATA_GET_INT_STOP(current_eedb_phydb, "is_data", is_data);
        RHDATA_GET_STR_STOP(current_eedb_phydb, "mag", mag);
        RHDATA_GET_INT_STOP(current_eedb_phydb, "offset", offset);

        for (eedb_phase_index = 0; eedb_phase_index < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES; eedb_phase_index++)
        {
            if (0 == sal_strcmp(mag, eedb_phase_names[eedb_phase_index]))
            {
                break;
            }
        }
        if (eedb_phase_index == DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected mag: %s.", mag);
        }

        if (is_data == 0)
        {
            mdb_physical_table_id = mdb_phase_names_ll[eedb_phase_index];
        }
        else
        {
            mdb_physical_table_id = mdb_phase_names_data[eedb_phase_index];
        }

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, nof_clusters, &cluster));

        cluster.macro_type = MDB_EEDB_BANK;
        cluster.macro_index = 0;
        cluster.cluster_index = idx;
        cluster.start_address = offset;
        cluster.end_address = offset + dnx_data_mdb.dh.nof_rows_in_eedb_bank_get(unit);

        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.set(unit, mdb_physical_table_id, nof_clusters + 1));
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.set(unit, mdb_physical_table_id, nof_clusters, &cluster));

        SHR_IF_ERR_EXIT(mdb_init_set_cluster_assoc(unit, MDB_EEDB_BANK, 0, idx, mdb_physical_table_id));

    }

exit:
    if (res)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("XML PARSING ERROR\n")));
        SHR_SET_CURRENT_ERR(res);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
mdb_parse_xml_em_estimate_set(
    int unit,
    int nof_ways,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (nof_ways > 0)
    {
        int max_capacity;
        int encoding_type_iter;
        SHR_IF_ERR_EXIT(mdb_get_capacity(unit, mdb_direct_mdb_to_dbal[mdb_physical_table_id], &max_capacity));

        for (encoding_type_iter = 0; encoding_type_iter < DNX_DATA_MAX_MDB_EM_NOF_ENCODING_TYPES; encoding_type_iter++)
        {
            int encoding_type_util_percentage = 0;
            int capacity_estimate;

            if (nof_ways >= 4)
            {
                if (encoding_type_iter == MDB_EM_ENCODING_TYPE_ONE)
                {
                    encoding_type_util_percentage = 95;
                }
                else
                {
                    encoding_type_util_percentage = 97;
                }
            }
            else if (nof_ways == 3)
            {
                if (encoding_type_iter == MDB_EM_ENCODING_TYPE_ONE)
                {
                    encoding_type_util_percentage = 80;
                }
                else if (encoding_type_iter == MDB_EM_ENCODING_TYPE_HALF)
                {
                    encoding_type_util_percentage = 94;
                }
                else
                {
                    encoding_type_util_percentage = 94;
                }
            }
            else if (nof_ways == 2)
            {
                if (encoding_type_iter == MDB_EM_ENCODING_TYPE_ONE)
                {
                    encoding_type_util_percentage = 32;
                }
                else if (encoding_type_iter == MDB_EM_ENCODING_TYPE_HALF)
                {
                    encoding_type_util_percentage = 65;
                }
                else
                {
                    encoding_type_util_percentage = 80;
                }
            }
            else
            {
                encoding_type_util_percentage = 5;
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "MDB EM table %s, only has a single hash.\n"),
                           dbal_physical_table_to_string(unit, mdb_direct_mdb_to_dbal[mdb_physical_table_id])));
            }
            capacity_estimate =
                max_capacity * encoding_type_util_percentage / 100 * (1 << encoding_type_iter) *
                MDB_DIRECT_BASIC_ENTRY_SIZE / dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->row_width;

            if (dnx_data_mdb.hw_bug.feature_get(unit, dnx_data_mdb_hw_bug_em_dfg_ovf_cam_disabled))
            {
                capacity_estimate *= 0.75;
            }

            SHR_IF_ERR_EXIT(mdb_db_infos.
                            em_entry_capacity_estimate.set(unit, mdb_direct_mdb_to_dbal[mdb_physical_table_id],
                                                           encoding_type_iter, capacity_estimate));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mdb_parse_xml_address_update_second_interface_use(
    int unit,
    int nof_cluster_per_entry,
    mdb_dh_macro_info_t * macro)
{
    int cluster_idx;
    int second_intf_first_cluster = dnx_data_mdb.dh.nof_macro_clusters_get(unit);
    mdb_parse_xml_second_interface_fsm_e state = MDB_DH_INTERFACE_FSM_NO_CLUSTER_WAS_FOUND;
    mdb_dh_init_interface_instance_in_macro_e counter_type = MDB_DH_INIT_FIRST_IF_USED;
    SHR_FUNC_INIT_VARS(unit);
    macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated = 0;
    for (cluster_idx = 0; cluster_idx < dnx_data_mdb.dh.nof_macro_clusters_get(unit); cluster_idx++)
    {
        int bit = (macro->intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map >> cluster_idx) & 0x1;

        if ((state == MDB_DH_INTERFACE_FSM_NO_CLUSTER_WAS_FOUND) && (bit == 1))
        {
            macro->intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster = cluster_idx;
            state = MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_FIRST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_FIRST_CLUSTER_FOUND) && (bit == 0))
        {
            state = MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_LAST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_FIRST_INTERFACE_LAST_CLUSTER_FOUND) && (bit == 1))
        {
            counter_type = MDB_DH_INIT_SECOND_IF_USED;
            macro->intf[MDB_DH_INIT_SECOND_IF_USED].first_alloc_cluster = cluster_idx;
            state = MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND) && (bit == 0))
        {
            state = MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_LAST_CLUSTER_FOUND;
        }
        else if ((state == MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_LAST_CLUSTER_FOUND) && (bit == 1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "MACRO %d of type %s has an invalid clusters allocation (more than two separate clusters groups).",
                         macro->macro_index, (macro->macro_type == MDB_MACRO_A) ? "A" : "B");
        }

        macro->intf[counter_type].nof_cluster_allocated += bit;
    }

    if (state >= MDB_DH_INTERFACE_FSM_SECOND_INTERFACE_FIRST_CLUSTER_FOUND)
    {
        second_intf_first_cluster = macro->intf[MDB_DH_INIT_SECOND_IF_USED].first_alloc_cluster;
    }
    else if (macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated > nof_cluster_per_entry)
    {
        int first_interface_count;

        if (nof_cluster_per_entry == 1)
        {
            first_interface_count =
                mdb_parse_xml_intf_1_em_120_clusters_division[macro->
                                                              intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated -
                                                              1];
        }
        else
        {
            first_interface_count = ((macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated >> 1) +
                                     (nof_cluster_per_entry - 1)) / nof_cluster_per_entry;
        }

        if (first_interface_count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "MACRO %d of type %s has an invalid clusters allocation (can't split the clusters between the two interfaces).",
                         macro->macro_index, (macro->macro_type == MDB_MACRO_A) ? "A" : "B");
        }

        macro->intf[MDB_DH_INIT_SECOND_IF_USED].nof_cluster_allocated =
            macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated - first_interface_count;
        macro->intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated = first_interface_count;
        second_intf_first_cluster = first_interface_count + macro->intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster;
        macro->intf[MDB_DH_INIT_SECOND_IF_USED].first_alloc_cluster = second_intf_first_cluster;
    }

    macro->intf[MDB_DH_INIT_SECOND_IF_USED].alloc_bit_map =
        macro->intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map & UTILEX_ZERO_BITS_MASK(second_intf_first_cluster - 1, 0);

    macro->intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map -= macro->intf[MDB_DH_INIT_SECOND_IF_USED].alloc_bit_map;
exit:
    SHR_FUNC_EXIT;
}

static void
mdb_parse_xml_address_update_flex_add(
    int unit,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id,
    mdb_dh_macro_info_t * macro)
{
    int interface_iter;

    if (MDB_DH_IS_FULLY_SUPPORTS_FLEX(unit, mdb_physical_table_id))
    {
        for (interface_iter = 0; interface_iter < MDB_DH_INIT_MAX_AVAILABLE_INTERFACES; interface_iter++)
        {
            if ((macro->intf[interface_iter].nof_cluster_allocated > 0)
                && ((macro->intf[interface_iter].nof_cluster_allocated & 0x1) == 0))
            {
                macro->intf[interface_iter].flags |= MDB_INIT_CLUSTER_FLAG_FLEX;
                macro->intf[interface_iter].nof_cluster_per_entry = 2;
            }
        }
    }
}

static shr_error_e
mdb_parse_xml_single_pdb_cluster_address_update(
    int unit,
    uint32 skip_address_update,
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id)
{
    int cluster_idx;
    int macro_global_index;
    int nof_clusters;
    int ways_count = 0;
    int nof_cluster_per_entry;
    int cluster_accumulation = 0;
    int nof_macro_b;
    int ads_half_depth_macro_b_units = 0;
    mdb_cluster_info_t cluster_info;
    uint8 is_kaps = ((mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1)
                     || (mdb_physical_table_id == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_2));
    mdb_dh_macro_info_t macros[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES];
    uint32 min_way_capacity =
        dnx_data_mdb.dh.nof_rows_in_macro_a_cluster_get(unit) * dnx_data_mdb.dh.nof_macro_clusters_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.get(unit, mdb_physical_table_id, &nof_cluster_per_entry));

    SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, mdb_physical_table_id, &nof_clusters));

    sal_memset(macros, 0x0, DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES * sizeof(mdb_dh_macro_info_t));
    for (macro_global_index = 0; macro_global_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_global_index++)
    {
        macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster =
            dnx_data_mdb.dh.nof_macro_clusters_get(unit);
    }

    for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.get(unit, mdb_physical_table_id, cluster_idx, &cluster_info));
        macro_global_index = MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster_info.macro_type, cluster_info.macro_index);
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit
                        (&(macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map),
                         cluster_info.cluster_index));
        macros[macro_global_index].macro_index = cluster_info.macro_index;
        macros[macro_global_index].macro_type = cluster_info.macro_type;
        macros[macro_global_index].cluster_index[cluster_info.cluster_index] = cluster_idx;
        macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_allocated++;
        macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster =
            UTILEX_MIN(macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].first_alloc_cluster,
                       cluster_info.cluster_index);
    }
    if (is_kaps)
    {
        mdb_macro_types_e ads_macro_type;
        dbal_enum_value_field_mdb_physical_table_e ads_db =
            (mdb_physical_table_id ==
             DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_BIG_KAPS_BB_1) ? DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1 :
            DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2;
        SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, ads_db, &nof_clusters));
        for (cluster_idx = 0; cluster_idx < nof_clusters; cluster_idx++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.macro_type.get(unit, ads_db, cluster_idx, &ads_macro_type));
            ads_half_depth_macro_b_units += (ads_macro_type == MDB_MACRO_A) ? 2 : 1;
        }
        ads_half_depth_macro_b_units >>= 1;
    }

    for (macro_global_index = 0; macro_global_index < dnx_data_mdb.dh.total_nof_macroes_get(unit); macro_global_index++)
    {
        if (macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].alloc_bit_map > 0)
        {
            mdb_dh_init_interface_instance_in_macro_e interface_iter;
            ways_count++;
            if (UTILEX_GET_BIT
                (dnx_data_mdb.dh.dh_info_get(unit, mdb_physical_table_id)->two_ways_connectivity_bm,
                 macro_global_index))
            {
                mdb_parse_xml_address_update_second_interface_use(unit, nof_cluster_per_entry,
                                                                  &macros[macro_global_index]);

                if (macros[macro_global_index].intf[MDB_DH_INIT_SECOND_IF_USED].nof_cluster_allocated > 0)
                {
                    macros[macro_global_index].intf[MDB_DH_INIT_SECOND_IF_USED].nof_cluster_per_entry =
                        nof_cluster_per_entry;
                    ways_count++;
                }
            }

            macros[macro_global_index].intf[MDB_DH_INIT_FIRST_IF_USED].nof_cluster_per_entry = nof_cluster_per_entry;

            mdb_parse_xml_address_update_flex_add(unit, mdb_physical_table_id, &macros[macro_global_index]);

            nof_macro_b = (macros[macro_global_index].macro_type == MDB_MACRO_A) ? 2 : 1;
            for (interface_iter = 0; interface_iter < MDB_DH_INIT_MAX_AVAILABLE_INTERFACES; interface_iter++)
            {
                if (macros[macro_global_index].intf[interface_iter].nof_cluster_allocated > 0)
                {
                    uint8 extra_intf_flag = 0;
                    int end_address = 0;
                    int clusters_bundle_iter;
                    int nof_bundles =
                        macros[macro_global_index].intf[interface_iter].nof_cluster_allocated /
                        macros[macro_global_index].intf[interface_iter].nof_cluster_per_entry;

                    if (interface_iter == MDB_DH_INIT_SECOND_IF_USED)
                    {
                        extra_intf_flag |= MDB_INIT_CLUSTER_FLAG_2ND_INTERFACES;
                    }

                    if (is_kaps
                        && (macros[macro_global_index].intf[interface_iter].nof_cluster_allocated ==
                            dnx_data_mdb.dh.nof_macro_clusters_get(unit)))
                    {
                        extra_intf_flag |=
                            (nof_macro_b <
                             ads_half_depth_macro_b_units) ? MDB_INIT_CLUSTER_FLAG_480_BPU :
                            MDB_INIT_CLUSTER_FLAG_960_BPU;
                    }

                    for (clusters_bundle_iter = 0; clusters_bundle_iter < nof_bundles; clusters_bundle_iter++)
                    {
                        int start_pos =
                            macros[macro_global_index].intf[interface_iter].first_alloc_cluster +
                            (clusters_bundle_iter *
                             macros[macro_global_index].intf[interface_iter].nof_cluster_per_entry);
                        for (cluster_idx = 0;
                             cluster_idx < macros[macro_global_index].intf[interface_iter].nof_cluster_per_entry;
                             cluster_idx++)
                        {
                            uint8 first_cluster_flag = (((start_pos + cluster_idx) ==
                                                         macros[macro_global_index].
                                                         intf[interface_iter].first_alloc_cluster) ?
                                                        MDB_INIT_CLUSTER_FLAG_FIRST_INTF_CLUSTER : 0);

                            uint8 cluster_flags = first_cluster_flag
                                | macros[macro_global_index].intf[interface_iter].flags | extra_intf_flag;

                            int cluster_glob_index = MDB_DH_SERIAL_CLUSTER_NUMBER_FOR_MACRO_TYPE(unit,
                                                                                                 macros
                                                                                                 [macro_global_index].
                                                                                                 macro_index,
                                                                                                 start_pos +
                                                                                                 cluster_idx);
                            int cluster_sw_state_idx =
                                macros[macro_global_index].cluster_index[start_pos + cluster_idx];

                            if (!skip_address_update)
                            {
                                int start_address =
                                    cluster_accumulation * dnx_data_mdb.dh.nof_rows_in_macro_b_cluster_get(unit);
                                end_address =
                                    (cluster_accumulation +
                                     nof_macro_b) * dnx_data_mdb.dh.nof_rows_in_macro_b_cluster_get(unit);
                                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                                start_address.set(unit, mdb_physical_table_id, cluster_sw_state_idx,
                                                                  start_address));
                                SHR_IF_ERR_EXIT(mdb_db_infos.db.clusters_info.
                                                end_address.set(unit, mdb_physical_table_id, cluster_sw_state_idx,
                                                                end_address));
                            }

                            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.macro_cluster_assoc.
                                            flags.set(unit, macros[macro_global_index].macro_type, cluster_glob_index,
                                                      cluster_flags));
                        }

                        cluster_accumulation += nof_macro_b;
                        if (is_kaps && (cluster_accumulation >= ads_half_depth_macro_b_units))
                        {
                            cluster_accumulation = 0;
                        }

                    }

                    if (dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type ==
                        DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
                    {
                        cluster_accumulation = 0;

                        if (utilex_nof_on_bits_in_long(end_address) != 1)
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG,
                                         "%s DB in MDB profile XML macro %d of type %s has an EM way with capacity %d which is not a power of 2.",
                                         dbal_physical_table_to_string(unit,
                                                                       mdb_direct_mdb_to_dbal[mdb_physical_table_id]),
                                         macros[macro_global_index].macro_index,
                                         (macros[macro_global_index].macro_type == MDB_MACRO_A) ? "A" : "B",
                                         end_address);
                        }

                        min_way_capacity = UTILEX_MIN(min_way_capacity, end_address);
                    }
                }

            }
        }

    }

    if ((dnx_data_mdb.pdbs.pdbs_info_get(unit, mdb_physical_table_id)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
        && (ways_count > 0))
    {
        SHR_IF_ERR_EXIT(mdb_parse_xml_em_estimate_set(unit, ways_count, mdb_physical_table_id));

        SHR_IF_ERR_EXIT(mdb_db_infos.em_key_format.
                        nof_address_bits.set(unit, mdb_direct_mdb_to_dbal[mdb_physical_table_id],
                                             utilex_log2_round_down(min_way_capacity)));
    }

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
mdb_parse_xml_clusters_address_update(
    int unit)
{
    uint32 skip_address_update;
    dbal_enum_value_field_mdb_physical_table_e mdb_physical_table_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_parse_xml_update_fec_address(unit));

    for (mdb_physical_table_id = 0; mdb_physical_table_id < DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES;
         mdb_physical_table_id++)
    {
        skip_address_update = (MDB_INIT_IS_FEC_TABLE(mdb_physical_table_id)
                               || MDB_INIT_IS_EEDB_RELATED_TABLE(mdb_physical_table_id));

        SHR_IF_ERR_EXIT(mdb_parse_xml_single_pdb_cluster_address_update
                        (unit, skip_address_update, mdb_physical_table_id));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_parse_xml_open_xml(
    int unit,
    void **curTop)
{
    void *curTop_temp = NULL;
    char file_path[RHFILE_MAX_SIZE];
    char *mdb_profile_str = NULL;
    char mdb_profile_str_lower[SOC_PROPERTY_VALUE_MAX];
    uint8 mdb_profile_u8;
    int char_iter;

    SHR_FUNC_INIT_VARS(unit);

    mdb_profile_str = dnx_data_mdb.pdbs.mdb_profile_get(unit)->profile;

    sal_memset(mdb_profile_str_lower, 0x0, sizeof(mdb_profile_str_lower));
    sal_strncpy(mdb_profile_str_lower, mdb_profile_str, SOC_PROPERTY_VALUE_MAX - 1);
    utilex_str_to_lower(mdb_profile_str_lower);

    for (mdb_profile_u8 = 0; mdb_profile_u8 < DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES; mdb_profile_u8++)
    {
        char *mdb_profile_name;
        if (dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->supported == FALSE)
        {
            continue;
        }

        mdb_profile_name = dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->name;

        if (sal_strncmp(mdb_profile_str_lower, mdb_profile_name, sal_strlen(mdb_profile_name)) == 0)
        {
            if ((sal_strlen(mdb_profile_name) == sal_strlen(mdb_profile_str_lower))
                || (sal_strncmp("custom", mdb_profile_name, sal_strlen("custom")) == 0))
            {
                break;
            }
        }
    }

    if (mdb_profile_u8 >= DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES)
    {
        uint8 mdb_profile_iter_u8;
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "Available values for mdb_profile soc property:\n")));
        for (mdb_profile_iter_u8 = 0; mdb_profile_iter_u8 < DNX_DATA_MAX_MDB_PDBS_MDB_NOF_PROFILES;
             mdb_profile_iter_u8++)
        {
            if (dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_iter_u8)->supported == FALSE)
            {
                continue;
            }
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "%s\n"),
                       dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_iter_u8)->name));
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown MDB profile: %s\n", mdb_profile_str);
    }

    for (char_iter = 0; char_iter < sal_strlen(mdb_profile_str_lower); char_iter++)
    {
        if (mdb_profile_str_lower[char_iter] == '-')
        {
            mdb_profile_str_lower[char_iter] = '_';
        }
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_profile.set(unit, mdb_profile_u8));

    sal_memset(file_path, 0x0, sizeof(file_path));
    sal_strncpy(file_path, MDB_HW_ALLOCATION_DIR_PATH, sizeof(file_path));
    sal_strncat_s(file_path, "/", sizeof(file_path));

    sal_strncat_s(file_path, "mdb_profile_", sizeof(file_path));
    sal_strncat_s(file_path, mdb_profile_str_lower, sizeof(file_path));

    if (sal_strncmp("custom", dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->name, sal_strlen("custom"))
        != 0)
    {
        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_half_nof_clusters_enable))
        {
            sal_strncat_s(file_path, "_half_a", sizeof(file_path));
        }
        if (dnx_data_mdb.dh.feature_get(unit, dnx_data_mdb_dh_macro_A_75_nof_clusters_enable))
        {
            sal_strncat_s(file_path, "_75_a", sizeof(file_path));
        }
    }
    sal_strncat_s(file_path, ".xml", sizeof(file_path));

    curTop_temp = dbx_file_get_xml_top(unit, file_path, "opt", CONF_OPEN_PER_DEVICE);
    if (curTop_temp == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find MDB HW allocation xml file: %s\n", file_path);
    }

    *curTop = curTop_temp;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_parse_xml_direct_physical_map(
    int unit)
{
    void *curTop = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_parse_xml_open_xml(unit, &curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_eedb_banks_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_mdb_clusters_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_em_banks_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_clusters_address_update(unit));

    SHR_IF_ERR_EXIT(mdb_parse_xml_update_address_map(unit));

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

shr_error_e
mdb_parse_xml_em_vmv(
    int unit)
{

    char *phydb_names[DBAL_NOF_PHYSICAL_TABLES] =
        { "ISEM1", "ISEM2", "LEM", "IOEM1", "PPMC", "GLEM1", "EOEM1", "ESEM", "SEXEM1", "SEXEM2", "SEXEM3", "LEXEM",
        "RMEP_EM"
    };

    dbal_physical_tables_e physical_tables[DBAL_NOF_PHYSICAL_TABLES] = { DBAL_PHYSICAL_TABLE_ISEM_1,
        DBAL_PHYSICAL_TABLE_ISEM_2,
        DBAL_PHYSICAL_TABLE_LEM,
        DBAL_PHYSICAL_TABLE_IOEM_1,
        DBAL_PHYSICAL_TABLE_PPMC,
        DBAL_PHYSICAL_TABLE_GLEM_1,
        DBAL_PHYSICAL_TABLE_EOEM_1,
        DBAL_PHYSICAL_TABLE_ESEM,
        DBAL_PHYSICAL_TABLE_SEXEM_1,
        DBAL_PHYSICAL_TABLE_SEXEM_2,
        DBAL_PHYSICAL_TABLE_SEXEM_3,
        DBAL_PHYSICAL_TABLE_LEXEM,
        DBAL_PHYSICAL_TABLE_RMEP_EM,
        DBAL_NOF_PHYSICAL_TABLES
    };
    dbal_physical_tables_e mdb_share_config_tables_map[DBAL_NOF_PHYSICAL_TABLES];

    char db_name[MDB_XML_MAX_STRING_LENGTH];
    void *cur_top = NULL;
    void *cur_phy_db = NULL;
    char vmv_file_path[RHFILE_MAX_SIZE];
    char *image_name = NULL;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(mdb_share_config_tables_map, 0, sizeof(mdb_share_config_tables_map));
    sal_memset(&db_name, 0, sizeof(db_name));
    sal_memset(&vmv_file_path, 0, sizeof(vmv_file_path));

    mdb_share_config_tables_map[DBAL_PHYSICAL_TABLE_ISEM_2] = DBAL_PHYSICAL_TABLE_ISEM_3;
    mdb_share_config_tables_map[DBAL_PHYSICAL_TABLE_IOEM_1] = DBAL_PHYSICAL_TABLE_IOEM_2;
    mdb_share_config_tables_map[DBAL_PHYSICAL_TABLE_GLEM_1] = DBAL_PHYSICAL_TABLE_GLEM_2;
    mdb_share_config_tables_map[DBAL_PHYSICAL_TABLE_EOEM_1] = DBAL_PHYSICAL_TABLE_EOEM_2;

    SHR_IF_ERR_EXIT(dnx_pp_prgm_device_image_name_get(unit, &image_name));
    sal_strncpy(vmv_file_path, "mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    sal_strncat_s(vmv_file_path, image_name, sizeof(vmv_file_path));
    sal_strncat_s(vmv_file_path, "/", sizeof(vmv_file_path));
    sal_strncat_s(vmv_file_path, image_name, sizeof(vmv_file_path));
    sal_strncat_s(vmv_file_path, "_vmv_values.xml", sizeof(vmv_file_path));

    cur_top = dbx_pre_compiled_mdb_top_get(unit, vmv_file_path, "VmvCatalog", CONF_OPEN_PER_DEVICE);
    if (cur_top == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find MDB HW allocation xml file: %s\n", vmv_file_path);
    }

    RHDATA_ITERATOR(cur_phy_db, cur_top, "PhyDB")
    {
        int table_index = 0;
        uint8 found = FALSE;

        RHDATA_GET_STR_STOP(cur_phy_db, "Name", db_name);

        while ((physical_tables[table_index] != DBAL_NOF_PHYSICAL_TABLES) && !found)
        {

            found = sal_strncmp(db_name, phydb_names[table_index], MDB_XML_MAX_STRING_LENGTH) == 0 ? TRUE : FALSE;
            if (found)
            {
                dbal_physical_tables_e physical_table = physical_tables[table_index];
                dbal_physical_tables_e mdb_second_core_table = mdb_share_config_tables_map[physical_table];
                void *cur_container;
                RHDATA_ITERATOR(cur_container, cur_phy_db, "Container")
                {
                    void *cur_vmv;
                    int size;
                    int vmv_size = 0, vmv_value = SAL_INT32_MAX;
                    mdb_em_entry_encoding_e encoding = MDB_EM_ENTRY_ENCODING_EMPTY;
                    RHDATA_GET_INT_STOP(cur_container, "Size", size);
                    if (size)
                    {
                        encoding =
                            utilex_log2_round_up(dnx_data_mdb.pdbs.pdb_info_get(unit, physical_table)->row_width /
                                                 size);
                    }

                    if (encoding == MDB_EM_ENTRY_ENCODING_EIGHTH)
                    {
                        continue;
                    }

                    RHDATA_ITERATOR(cur_vmv, cur_container, "Vmv")
                    {
                        uint32 vmv_suffix;
                        int tmp_value, tmp_size;
                        RHDATA_GET_INT_STOP(cur_vmv, "Value", tmp_value);
                        RHDATA_GET_INT_STOP(cur_vmv, "Size", tmp_size);

                        for (vmv_suffix = 0;
                             vmv_suffix < (1 << (dnx_data_mdb.em.max_nof_vmv_size_get(unit) - tmp_size)); vmv_suffix++)
                        {

                            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                            encoding.set(unit, physical_table, tmp_value | vmv_suffix, encoding));
                            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                            size.set(unit, physical_table, tmp_value | vmv_suffix, tmp_size));
                            if (mdb_second_core_table != DBAL_PHYSICAL_TABLE_NONE)
                            {
                                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                                encoding.set(unit, mdb_second_core_table, tmp_value | vmv_suffix,
                                                             encoding));
                                SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.encoding_map.
                                                size.set(unit, mdb_second_core_table, tmp_value | vmv_suffix,
                                                         tmp_size));
                            }
                        }

                        if (tmp_value < vmv_value)
                        {
                            vmv_value = tmp_value;
                            vmv_size = tmp_size;
                        }
                    }
                    if (vmv_value < SAL_INT32_MAX)
                    {
                        int vmv_bit_size = dnx_data_mdb.em.max_nof_vmv_size_get(unit);

                        if (physical_table == DBAL_PHYSICAL_TABLE_GLEM_1)
                        {
                            vmv_bit_size = dnx_data_mdb.em.glem_nof_vmv_size_get(unit);
                        }
                        else if (physical_table == DBAL_PHYSICAL_TABLE_ESEM)
                        {
                            vmv_bit_size = dnx_data_mdb.em.esem_nof_vmv_size_get(unit);
                        }
                        else if (physical_table == DBAL_PHYSICAL_TABLE_LEM && encoding == MDB_EM_ENTRY_ENCODING_HALF)
                        {
                            vmv_bit_size = dnx_data_mdb.em.mact_nof_vmv_size_get(unit);
                        }
                        if (vmv_bit_size < vmv_size)
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid VMV size was set for table %s\n", db_name);
                        }
                        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.value.set(unit, physical_table, encoding, vmv_value));
                        SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.size.set(unit, physical_table, encoding, vmv_size));

                        if (mdb_second_core_table != DBAL_PHYSICAL_TABLE_NONE)
                        {
                            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                            value.set(unit, mdb_second_core_table, encoding, vmv_value));
                            SHR_IF_ERR_EXIT(mdb_em_db.vmv_info.
                                            size.set(unit, mdb_second_core_table, encoding, vmv_size));
                        }
                    }

                }
            }
            table_index++;
        }

        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find matching DB to %s\n", db_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

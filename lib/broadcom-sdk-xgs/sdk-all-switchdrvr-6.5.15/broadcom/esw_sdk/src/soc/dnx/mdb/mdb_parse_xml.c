/*
 * ! \file mdb_parse_xml.c $Id$ Contains all of the MDB profile XML parsing capabilities.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_em_access.h>
#include "../dbal/dbal_internal.h"
#include <soc/dnx/mdb.h>
#include "mdb_internal.h"
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

#define MDB_PROFILE_BALANCED_FILE       "mdb_profile_balanced.xml"
#define MDB_PROFILE_BALANCED_EXEM_FILE  "mdb_profile_balanced_exem.xml"
#define MDB_PROFILE_L2_FILE             "mdb_profile_l2_xl.xml"
#define MDB_PROFILE_L3_FILE             "mdb_profile_l3_xl.xml"
#define MDB_PROFILE_EXT_KBP_FILE        "mdb_profile_ext_kbp.xml"
#define MDB_PROFILE_EXEM_DEBUG_FILE     "mdb_profile_exem_debug.xml"
#define MDB_PROFILE_CUSTOM_FILE         "mdb_profile_custom.xml"

#define MDB_FILE_PATH                   "jericho_2"
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

/*
 * }
 */

shr_error_e
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

shr_error_e
mdb_parse_xml_get_dh_info(
    int unit,
    int bucket_width,
    int address,
    int cluster_index,
    char *dh_name,
    dbal_enum_value_field_mdb_physical_table_e current_mdb_table,
    mdb_cluster_info_t * clusters,
    int *cluster_count)
{
    uint8 is_dha;
    char dh_name_compare[MDB_XML_MAX_STRING_LENGTH], dh_name_compare_iter[MDB_XML_MAX_STRING_LENGTH];
    int macro_idx, unscrambled_macro_idx, macro_idx_max;
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

    if (dnx_data_mdb.pdbs.mdb_profile_get(unit)->val == MDB_EXEM_DEBUG_PROFILE)
    {
        if (is_dha == 1)
        {
            switch (macro_idx)
            {
                case 0:
                    unscrambled_macro_idx = 0;
                    break;
                case 1:
                    unscrambled_macro_idx = 7;
                    break;
                case 2:
                    unscrambled_macro_idx = 6;
                    break;
                case 3:
                    unscrambled_macro_idx = 9;
                    break;
                case 4:
                    unscrambled_macro_idx = 1;
                    break;
                case 5:
                    unscrambled_macro_idx = 10;
                    break;
                case 6:
                    unscrambled_macro_idx = 2;
                    break;
                case 7:
                    unscrambled_macro_idx = 4;
                    break;
                case 8:
                    unscrambled_macro_idx = 8;
                    break;
                case 9:
                    unscrambled_macro_idx = 11;
                    break;
                case 10:
                    unscrambled_macro_idx = 5;
                    break;
                case 11:
                    unscrambled_macro_idx = 3;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected macro_idx: %d for MACRO A.", macro_idx);
            }
        }
        else
        {
            switch (macro_idx)
            {
                case 0:
                    unscrambled_macro_idx = 0;
                    break;
                case 1:
                    unscrambled_macro_idx = 1;
                    break;
                case 2:
                    unscrambled_macro_idx = 6;
                    break;
                case 3:
                    unscrambled_macro_idx = 3;
                    break;
                case 4:
                    unscrambled_macro_idx = 5;
                    break;
                case 5:
                    unscrambled_macro_idx = 2;
                    break;
                case 6:
                    unscrambled_macro_idx = 7;
                    break;
                case 7:
                    unscrambled_macro_idx = 4;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected macro_idx: %d for MACRO B.", macro_idx);
            }
        }
    }
    else
    {
        unscrambled_macro_idx = macro_idx;
    }

    for (cluster_pair_idx = 0; cluster_pair_idx < nof_clusters; cluster_pair_idx++)
    {
        uint8 cache_enabled;

        mdb_cluster_info_t *clusters_p = &clusters[*cluster_count];

        sal_memset(clusters_p, 0, sizeof(mdb_cluster_info_t));
        (*cluster_count)++;

        clusters_p->macro_type = (is_dha == 1) ? MDB_MACRO_A : MDB_MACRO_B;
        clusters_p->macro_index = unscrambled_macro_idx;
        clusters_p->cluster_index = cluster_index + cluster_pair_idx;
        clusters_p->start_address = address;
        clusters_p->cluster_position_in_entry = cluster_pair_idx;
        clusters_p->end_address =
            address + ((is_dha == 1) ? MDB_NOF_ROWS_IN_MACRO_A_CLUSTER : MDB_NOF_ROWS_IN_MACRO_B_CLUSTER);

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
                int alloc_size;
                int global_cluster_idx =
                    (unscrambled_macro_idx * dnx_data_mdb.dh.nof_macro_clusters_get(unit)) + cluster_index +
                    cluster_pair_idx;

                if (is_dha == 1)
                {
                    alloc_size = DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER + 1;
                    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                                    macro_a_cluster_cache.alloc(unit, global_cluster_idx, alloc_size,
                                                                BITS2WORDS
                                                                (DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS)));
                }
                else
                {
                    alloc_size = DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_B_CLUSTER + 1;
                    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                                    macro_b_cluster_cache.alloc(unit, global_cluster_idx, alloc_size,
                                                                BITS2WORDS
                                                                (DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS)));
                }
            }
        }

        if ((current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
            || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
            || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
            || (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA))
        {
            dbal_enum_value_field_mdb_physical_table_e first_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL,
                second_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL;
            dbal_enum_value_field_mdb_eedb_phase_e first_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1, second_phase =
                DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2;

            int phase_index;
            int phase_row_iter;


            if (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_2_DATA)
            {
                first_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_1_LL;
                second_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_2_LL;
                first_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_1;
                second_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_2;
            }
            else if (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_4_DATA)
            {
                first_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_3_LL;
                second_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_4_LL;
                first_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_3;
                second_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_4;
            }
            else if (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_6_DATA)
            {
                first_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_5_LL;
                second_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_6_LL;
                first_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_5;
                second_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_6;
            }
            else if (current_mdb_table == DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_8_DATA)
            {
                first_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_7_LL;
                second_phase_table = DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_EEDB_8_LL;
                first_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_7;
                second_phase = DBAL_ENUM_FVAL_MDB_EEDB_PHASE_PHASE_8;
            }
            clusters_p->start_address = MDB_DIRECT_INVALID_START_ADDRESS;
            clusters_p->end_address = MDB_DIRECT_INVALID_START_ADDRESS;

            for (phase_row_iter = 0;
                 phase_row_iter <
                 DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK;
                 phase_row_iter++)
            {
                clusters_p->phase_alloc_rows[phase_row_iter] = first_phase;
            }

            for (phase_index = 0; phase_index < dnx_data_mdb.eedb.nof_phase_per_mag_get(unit); phase_index++)
            {
                dbal_enum_value_field_mdb_physical_table_e iter_phase_table;
                dbal_enum_value_field_mdb_eedb_phase_e iter_phase;
                int nof_clusters_phase;
                int ll_cluster_idx;
                uint32 payload_type;

                if (phase_index == 0)
                {
                    iter_phase_table = first_phase_table;
                    iter_phase = first_phase;
                }
                else
                {
                    iter_phase_table = second_phase_table;
                    iter_phase = second_phase;
                }

                SHR_IF_ERR_EXIT(mdb_db_infos.eedb_payload_type.get(unit, iter_phase, &payload_type));

                SHR_IF_ERR_EXIT(mdb_db_infos.db.number_of_clusters.get(unit, iter_phase_table, &nof_clusters_phase));
                for (ll_cluster_idx = 0; ll_cluster_idx < nof_clusters_phase; ll_cluster_idx++)
                {
                    mdb_cluster_info_t ll_cluster_info;
                    int ll_start_mapped, ll_end_mapped;
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                    clusters_info.get(unit, iter_phase_table, ll_cluster_idx, &ll_cluster_info));

                    ll_start_mapped =
                        ll_cluster_info.start_address *
                        (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type)) /
                        (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B));
                    ll_end_mapped =
                        ll_cluster_info.end_address *
                        (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(payload_type)) /
                        (MDB_PAYLOAD_SIZE_TYPE_TO_PAYLOAD_SIZE(DBAL_ENUM_FVAL_DIRECT_PAYLOAD_SIZES_30B));

                    if ((ll_start_mapped >= clusters_p->start_address) && (ll_end_mapped <= clusters_p->end_address))
                    {
                        for (phase_row_iter = ll_start_mapped; phase_row_iter < ll_end_mapped;
                             phase_row_iter += MDB_NOF_ROWS_IN_EEDB_BANK)
                        {
                            clusters_p->phase_alloc_rows[(phase_row_iter -
                                                          clusters_p->start_address) /
                                                         MDB_NOF_ROWS_IN_EEDB_BANK] = iter_phase;
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
shr_error_e
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
                int addr, cluster_idx;

                RHDATA_GET_INT_STOP(cur_phy_map, "addr", addr);

                RHDATA_GET_INT_STOP(cur_phy_map, "cluster0_idx", cluster_idx);
                RHDATA_GET_STR_STOP(cur_phy_map, "dh", dh_name);

                SHR_IF_ERR_EXIT(mdb_parse_xml_get_dh_info
                                (unit, bucket_width, addr, cluster_idx, dh_name, em_table, clusters, &cluster_count));

                nof_cluster_per_entry = bucket_width / MDB_NOF_CLUSTER_ROW_BITS;
            }

            SHR_IF_ERR_EXIT(mdb_set_clusters_in_swstate(unit, cluster_count, cluster_count, clusters, em_table));
            SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.set(unit, em_table, nof_cluster_per_entry));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
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

            sal_memset(clusters, 0,
                       DNX_DATA_MAX_MDB_DH_NOF_MACRO_CLUSTERS * DNX_DATA_MAX_MDB_PDBS_MAX_NOF_INTERFACE_DHS *
                       sizeof(mdb_cluster_info_t));

            RHDATA_GET_INT_STOP(cur_phydb, "bucket_width", bucket_width);
            SHR_IF_ERR_EXIT(mdb_db_infos.db.nof_cluster_per_entry.set(unit, current_mdb_table,
                                                                      bucket_width / MDB_NOF_CLUSTER_ROW_BITS));

            RHDATA_ITERATOR(cur_phy_map, cur_phydb, "physical_map")
            {
                char dh_name[MDB_XML_MAX_STRING_LENGTH];
                int addr;
                int cluster_idx;

                RHDATA_GET_INT_STOP(cur_phy_map, "addr", addr);
                RHDATA_GET_INT_STOP(cur_phy_map, "cluster0_idx", cluster_idx);
                RHDATA_GET_STR_STOP(cur_phy_map, "dh", dh_name);

                SHR_IF_ERR_EXIT(mdb_parse_xml_get_dh_info
                                (unit, bucket_width, addr, cluster_idx, dh_name, current_mdb_table,
                                 clusters, &cluster_count));
            }

            if ((current_mdb_table >= DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_1)
                && (current_mdb_table <= DBAL_ENUM_FVAL_MDB_PHYSICAL_TABLE_ADS_2) && (cluster_count != 0))
            {
                int ads_depth;

                ads_depth = clusters[cluster_count - 1].end_address;

                if (ads_depth == MDB_NOF_ROWS_IN_MACRO_A_CLUSTER * 2)
                {
                    clusters[0].big_kaps_alloc_flags = MDB_KAPS_BIG_ADS_DEPTH_32K;
                    clusters[0].big_kaps_alloc_flags_current = MDB_KAPS_BIG_ADS_DEPTH_32K;
                }
                else if (ads_depth == MDB_NOF_ROWS_IN_MACRO_A_CLUSTER)
                {
                    clusters[0].big_kaps_alloc_flags = MDB_KAPS_BIG_ADS_DEPTH_16K;
                    clusters[0].big_kaps_alloc_flags_current = MDB_KAPS_BIG_ADS_DEPTH_16K;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected big KAPS ADS depth %d.", ads_depth);
                }
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

shr_error_e
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

    for (phase_iterator = 0; phase_iterator < DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES; phase_iterator++) {
        address_granularity[phase_iterator] = dnx_data_mdb.eedb.outlif_physical_phase_granularity_get(unit, phase_iterator)->data_granularity;
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.cache_enabled.get(unit, &cache_enabled));

    if (cache_enabled == TRUE)
    {
        for (bank_iter = 0; bank_iter < dnx_data_mdb.eedb.nof_eedb_banks_get(unit); bank_iter++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.mdb_cluster_infos.
                            eedb_banks_cache.alloc(unit, bank_iter, DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK,
                                                   BITS2WORDS(DNX_DATA_MAX_MDB_DH_CLUSTER_ROW_WIDTH_BITS)));
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
            int row_iter;

            for (row_iter = 0;
                 row_iter < DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK;
                 row_iter++)
            {
                clusters[bank_iter].phase_alloc_rows[row_iter] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;
            }
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
        cluster.end_address = offset + MDB_NOF_ROWS_IN_EEDB_BANK;

        if (is_data == 0)
        {
            int row_iter;
            for (row_iter = 0;
                 row_iter < DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_MACRO_A_CLUSTER / DNX_DATA_MAX_MDB_DH_NOF_ROWS_IN_EEDB_BANK;
                 row_iter++)
            {
                cluster.phase_alloc_rows[row_iter] = DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES;

            }
        }

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

shr_error_e
mdb_parse_xml_direct_physical_map(
    int unit)
{
    void *curTop = NULL;
    char file_path[RHFILE_MAX_SIZE];
    mdb_profiles_e mdb_profile;

    SHR_FUNC_INIT_VARS(unit);

    mdb_profile = dnx_data_mdb.pdbs.mdb_profile_get(unit)->val;

    sal_strcpy(file_path, MDB_HW_ALLOCATION_DIR_PATH);
    sal_strncat(file_path, "/", strlen("/"));
    if (mdb_profile == MDB_BALANCED_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_BALANCED_FILE, strlen(MDB_PROFILE_BALANCED_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: Balanced\n")));
    }
    else if (mdb_profile == MDB_BALANCED_EXEM_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_BALANCED_EXEM_FILE, strlen(MDB_PROFILE_BALANCED_EXEM_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: Balanced-Exem\n")));
    }
    else if (mdb_profile == MDB_L2_XL_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_L2_FILE, strlen(MDB_PROFILE_L2_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: L2-XL\n")));
    }
    else if (mdb_profile == MDB_L3_XL_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_L3_FILE, strlen(MDB_PROFILE_L3_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: L3-XL\n")));
    }
    else if (mdb_profile == MDB_EXT_KBP_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_EXT_KBP_FILE, strlen(MDB_PROFILE_EXT_KBP_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: Ext-KBP\n")));
    }
    else if (mdb_profile == MDB_EXEM_DEBUG_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_EXEM_DEBUG_FILE, strlen(MDB_PROFILE_EXEM_DEBUG_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: Exem-debug\n")));
    }
    else if (mdb_profile == MDB_CUSTOM_PROFILE)
    {
        sal_strncat(file_path, MDB_PROFILE_CUSTOM_FILE, strlen(MDB_PROFILE_CUSTOM_FILE));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "                  MDB profile: Custom\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown MDB profile: %d\n", mdb_profile);
    }

    curTop = dbx_file_get_xml_top(unit, file_path, "opt", CONF_OPEN_PER_DEVICE);
    if (curTop == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot find MDB HW allocation xml file: %s\n", file_path);
    }

    SHR_IF_ERR_EXIT(mdb_parse_xml_eedb_banks_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_mdb_clusters_map(unit, curTop));

    SHR_IF_ERR_EXIT(mdb_parse_xml_em_banks_map(unit, curTop));

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
    dbal_physical_tables_e mdb_second_core_tables_map[DBAL_NOF_PHYSICAL_TABLES];

    char db_name[MDB_XML_MAX_STRING_LENGTH];
    void *cur_top = NULL;
    void *cur_phy_db;
    char vmv_file_path[RHFILE_MAX_SIZE];
    char *image_name;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mdb_second_core_tables_map, 0, sizeof(dbal_physical_tables_e) * DBAL_NOF_PHYSICAL_TABLES);

    if (dnx_data_device.general.max_nof_cores_get(unit) == 2)
    {
        mdb_second_core_tables_map[DBAL_PHYSICAL_TABLE_ISEM_2] = DBAL_PHYSICAL_TABLE_ISEM_3;
        mdb_second_core_tables_map[DBAL_PHYSICAL_TABLE_IOEM_1] = DBAL_PHYSICAL_TABLE_IOEM_2;
        mdb_second_core_tables_map[DBAL_PHYSICAL_TABLE_GLEM_1] = DBAL_PHYSICAL_TABLE_GLEM_2;
        mdb_second_core_tables_map[DBAL_PHYSICAL_TABLE_EOEM_1] = DBAL_PHYSICAL_TABLE_EOEM_2;
    }

    image_name = dnx_data_pp.application.device_image_get(unit)->name;
    sal_strncpy(vmv_file_path, "mdb/auto_generated/", RHFILE_MAX_SIZE - 1);
    sal_strncat(vmv_file_path, image_name, RHFILE_MAX_SIZE - 1);
    sal_strncat(vmv_file_path, "/", RHFILE_MAX_SIZE - 1);
    sal_strncat(vmv_file_path, image_name, RHFILE_MAX_SIZE - 1);
    sal_strncat(vmv_file_path, "_vmv_values.xml", RHFILE_MAX_SIZE - 1);

    cur_top = dbx_file_get_xml_top(unit, vmv_file_path, "VmvCatalog", CONF_OPEN_PER_DEVICE);
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
                dbal_physical_tables_e mdb_second_core_table = mdb_second_core_tables_map[physical_table];
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

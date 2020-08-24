/*
 * ! \file mdb_diag.c Contains all of the MDB diag commands
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_MDBDNX
#include <shared/bsl.h>

/*************
 * INCLUDES  *
 *************/
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>

#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <appl/diag/diag.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include "ctest_dnx_mdb.h"

/*************
 * EXTERNS   *
 *************/

/*************
 * DEFINES   *
 *************/

/*************
* FUNCTIONS *
*************/

shr_error_e
mdb_eedb_dynamic_bank_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode)
{
    mdb_cluster_alloc_info_t first_src_cluster, first_dst_cluster;
    mdb_cluster_alloc_info_t second_src_cluster, second_dst_cluster;
    mdb_cluster_alloc_info_t *clusters = NULL;
    mdb_cluster_alloc_info_t *src_clusters = NULL;
    mdb_cluster_alloc_info_t *dst_clusters = NULL;
    int nof_valid_clusters;
    int first_cluster_idx = 0, second_cluster_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(clusters, sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS, "mdb_eedb_dynamic_bank_test_clusters",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(src_clusters, sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS,
              "mdb_eedb_dynamic_bank_test_src_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(dst_clusters, sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS,
              "mdb_eedb_dynamic_bank_test_dst_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_physical_table_id, clusters, &nof_valid_clusters));

    /*
     * Print debug information as to the different clusters available
     */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Available clusters to %s:\n"),
              dbal_physical_table_to_string(unit, dbal_physical_table_id)));
    for (first_cluster_idx = 0; first_cluster_idx < nof_valid_clusters; first_cluster_idx++)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Current DBAL table: %s, macro_type: %d, macro_idx: %d, cluster_idx: %d.\n"),
                  dbal_physical_table_to_string(unit, clusters[first_cluster_idx].dbal_physical_table_id),
                  clusters[first_cluster_idx].macro_type, clusters[first_cluster_idx].macro_idx,
                  clusters[first_cluster_idx].cluster_idx));
    }

    /*
     * Search for the first EEDB bank to modify its allocation, start from the start
     */
    for (first_cluster_idx = 0; first_cluster_idx < nof_valid_clusters; first_cluster_idx++)
    {
        if (clusters[first_cluster_idx].macro_type == MDB_EEDB_BANK)
        {
            /*
             * Search for a second EEDB bank to modify its allocation, start from the end, clusters allocated to other tables
             * are at the end
             */
            for (second_cluster_idx = nof_valid_clusters - 1; second_cluster_idx >= 0; second_cluster_idx--)
            {
                uint32 phase_bank_index;
                int phase_index = clusters[second_cluster_idx].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;

                if (clusters[second_cluster_idx].macro_type == MDB_EEDB_BANK)
                {
                    /*
                     * Make sure the bank pointed to by first_cluster_idx, can be allocated to the DBAL table
                     * associated with second_cluster_idx
                     */
                    SHR_IF_ERR_EXIT(mdb_eedb_phase_bank_to_index
                                    (unit, clusters[first_cluster_idx].cluster_idx, phase_index, &phase_bank_index));

                    /*
                     * If this bank can be allocated to this phase
                     */
                    if (phase_bank_index != DBAL_NOF_ENUM_MDB_EEDB_PHASE_VALUES)
                    {
                        /*
                         * Stop at the first EEDB bank found
                         */
                        break;
                    }
                }
            }

            /*
             * If the second cluster to switch with has been found, the cluster will always find itself so we are
             * always guaranteed to have a switch, albeit a degenerated one
             */
            if (second_cluster_idx >= 0)
            {
                break;
            }
        }
    }

    sal_memcpy(&first_src_cluster, &clusters[first_cluster_idx], sizeof(first_src_cluster));
    /*
     * The first dst will have the properties of the second src, except for its cluster identification
     */
    sal_memcpy(&first_dst_cluster, &clusters[second_cluster_idx], sizeof(first_dst_cluster));
    first_dst_cluster.macro_type = first_src_cluster.macro_type;
    first_dst_cluster.macro_idx = first_src_cluster.macro_idx;
    first_dst_cluster.cluster_idx = first_src_cluster.cluster_idx;

    sal_memcpy(&src_clusters[0], &first_src_cluster, sizeof(first_src_cluster));
    sal_memcpy(&dst_clusters[0], &first_dst_cluster, sizeof(first_dst_cluster));

    sal_memcpy(&second_src_cluster, &clusters[second_cluster_idx], sizeof(second_src_cluster));
    /*
     * The second dst will have the properties of the first src, except for its cluster identification
     */
    sal_memcpy(&second_dst_cluster, &clusters[first_cluster_idx], sizeof(second_dst_cluster));
    second_dst_cluster.macro_type = second_src_cluster.macro_type;
    second_dst_cluster.macro_idx = second_src_cluster.macro_idx;
    second_dst_cluster.cluster_idx = second_src_cluster.cluster_idx;

    sal_memcpy(&src_clusters[1], &second_src_cluster, sizeof(second_src_cluster));
    sal_memcpy(&dst_clusters[1], &second_dst_cluster, sizeof(second_dst_cluster));

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Switching the two clusters:\n")));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "DBAL table: %s, macro_type: %d, macro_idx: %d, cluster_idx: %d.\n"),
              dbal_physical_table_to_string(unit, first_src_cluster.dbal_physical_table_id),
              first_src_cluster.macro_type, first_src_cluster.macro_idx, first_src_cluster.cluster_idx));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "DBAL table: %s, macro_type: %d, macro_idx: %d, cluster_idx: %d.\n"),
              dbal_physical_table_to_string(unit, second_src_cluster.dbal_physical_table_id),
              second_src_cluster.macro_type, second_src_cluster.macro_idx, second_src_cluster.cluster_idx));

    /*
     * Switch the two clusters
     */
    SHR_IF_ERR_EXIT(mdb_init_delete_table_resources(unit, 2 /* nof_valid_clusters */ , src_clusters));
    SHR_IF_ERR_EXIT(mdb_init_add_table_resources(unit, 2 /* nof_valid_clusters */ , dst_clusters));

    /*
     * Run the tests on the DBAL tables
     */
    SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, first_src_cluster.dbal_physical_table_id, mode, FALSE));
    if (first_dst_cluster.dbal_physical_table_id != first_src_cluster.dbal_physical_table_id)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, first_dst_cluster.dbal_physical_table_id, mode, FALSE));
    }

    /*
     * Restore the table resources to their previous value, by reversing the src/dst
     */
    sal_memcpy(&dst_clusters[0], &first_src_cluster, sizeof(first_src_cluster));
    sal_memcpy(&src_clusters[0], &first_dst_cluster, sizeof(first_dst_cluster));
    sal_memcpy(&dst_clusters[1], &second_src_cluster, sizeof(second_src_cluster));
    sal_memcpy(&src_clusters[1], &second_dst_cluster, sizeof(second_dst_cluster));

    SHR_IF_ERR_EXIT(mdb_init_delete_table_resources(unit, 2 /* nof_valid_clusters */ , src_clusters));
    SHR_IF_ERR_EXIT(mdb_init_add_table_resources(unit, 2 /* nof_valid_clusters */ , dst_clusters));
    /*
     * Run the tests on the DBAL tables
     */
    SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, first_src_cluster.dbal_physical_table_id, mode, FALSE));
    if (first_dst_cluster.dbal_physical_table_id != first_src_cluster.dbal_physical_table_id)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, first_dst_cluster.dbal_physical_table_id, mode, FALSE));
    }

exit:
    SHR_FREE(clusters);
    SHR_FREE(src_clusters);
    SHR_FREE(dst_clusters);

    SHR_FUNC_EXIT;
}

shr_error_e
mdb_eedb_dynamic_cluster_test(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    mdb_test_mode_e mode)
{
    mdb_cluster_alloc_info_t first_src_cluster, first_dst_cluster;
    mdb_cluster_alloc_info_t second_src_cluster;
    mdb_cluster_alloc_info_t *clusters = NULL;
    mdb_cluster_alloc_info_t *src_clusters = NULL;
    mdb_cluster_alloc_info_t *dst_clusters = NULL;
    int nof_valid_clusters;
    int first_cluster_idx = 0, second_cluster_idx = 0;
    int row_width;
    int row_factor;
    int cluster_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(clusters, sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS, "mdb_eedb_dynamic_bank_test_clusters",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(src_clusters, sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS,
              "mdb_eedb_dynamic_bank_test_src_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(dst_clusters, sizeof(mdb_cluster_alloc_info_t) * MDB_MAX_NOF_CLUSTERS,
              "mdb_eedb_dynamic_bank_test_dst_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(mdb_direct_table_get_row_width(unit, dbal_physical_table_id, &row_width));
    row_width = dnx_data_mdb.pdbs.pdb_info_get(unit, dbal_physical_table_id)->row_width;
    row_factor = row_width / MDB_NOF_CLUSTER_ROW_BITS;

    SHR_IF_ERR_EXIT(mdb_init_get_table_resources(unit, dbal_physical_table_id, clusters, &nof_valid_clusters));

    /*
     * Print debug information as to the different clusters available
     */
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Available clusters to %s:\n"),
              dbal_physical_table_to_string(unit, dbal_physical_table_id)));
    for (first_cluster_idx = 0; first_cluster_idx < nof_valid_clusters; first_cluster_idx++)
    {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Current DBAL table: %s, macro_type: %d, macro_idx: %d, cluster_idx: %d.\n"),
                  dbal_physical_table_to_string(unit, clusters[first_cluster_idx].dbal_physical_table_id),
                  clusters[first_cluster_idx].macro_type, clusters[first_cluster_idx].macro_idx,
                  clusters[first_cluster_idx].cluster_idx));
    }

    /*
     * Search for the first macro cluster that belongs to a different DBAL table
     */
    for (first_cluster_idx = 0; first_cluster_idx < nof_valid_clusters; first_cluster_idx++)
    {
        if ((clusters[first_cluster_idx].macro_type != MDB_EEDB_BANK)
            && (clusters[first_cluster_idx].dbal_physical_table_id != dbal_physical_table_id))
        {
            /*
             * If the cluster has double row_width, verify the next cluster can also be allocated. Assume that if the
             * cluster is available, it is the next cluster
             */
            if (row_width != MDB_NOF_CLUSTER_ROW_BITS)
            {
                if ((first_cluster_idx + 1 == nof_valid_clusters)
                    || !((clusters[first_cluster_idx + 1].macro_type == clusters[first_cluster_idx].macro_type)
                         && (clusters[first_cluster_idx + 1].macro_idx == clusters[first_cluster_idx].macro_idx)
                         && (clusters[first_cluster_idx + 1].cluster_idx ==
                             clusters[first_cluster_idx].cluster_idx + 1)))
                {
                    continue;
                }
            }
            /*
             * Search for a second macro cluster with the same macro type and index that belongs to the current table, to switch with the first macro cluster
             * No need to verify row_width here as it is assumed to be valid
             */
            for (second_cluster_idx = 0; second_cluster_idx < nof_valid_clusters; second_cluster_idx++)
            {
                if ((clusters[first_cluster_idx].macro_type == clusters[second_cluster_idx].macro_type) &&
                    (clusters[first_cluster_idx].dbal_physical_table_id !=
                     clusters[second_cluster_idx].dbal_physical_table_id))
                {
                    dbal_enum_value_field_mdb_physical_table_e mdb_table_id_dst = dnx_data_mdb.pdbs.pdb_info_get(unit,
                                                                                                                 clusters
                                                                                                                 [first_cluster_idx].
                                                                                                                 dbal_physical_table_id)->logical_to_physical;

                    dbal_enum_value_field_mdb_physical_table_e mdb_table_id_src = dnx_data_mdb.pdbs.pdb_info_get(unit,
                                                                                                                 clusters
                                                                                                                 [second_cluster_idx].
                                                                                                                 dbal_physical_table_id)->logical_to_physical;

                    uint32 source_allocation, destination_allocation;
                    uint32 add_removed_clusters = 0;
                    if (mdb_table_id_dst == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES
                        || mdb_table_id_src == DBAL_NOF_ENUM_MDB_PHYSICAL_TABLE_VALUES)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "Couldn't tranlate on of the following tables %s or %s.\n",
                                     dbal_physical_table_to_string(unit,
                                                                   clusters[first_cluster_idx].dbal_physical_table_id),
                                     dbal_physical_table_to_string(unit,
                                                                   clusters
                                                                   [second_cluster_idx].dbal_physical_table_id));
                    }
                    /** get the cluster allocation of the source table */
                    SHR_IF_ERR_EXIT(mdb_get_macro_allocation
                                    (unit, mdb_table_id_src, clusters[second_cluster_idx].macro_idx,
                                     clusters[second_cluster_idx].macro_type, &source_allocation));
                    /** get the destination allocation on the source macro */
                    SHR_IF_ERR_EXIT(mdb_get_macro_allocation
                                    (unit, mdb_table_id_dst, clusters[second_cluster_idx].macro_idx,
                                     clusters[second_cluster_idx].macro_type, &destination_allocation));

                    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                                    (&add_removed_clusters, clusters[second_cluster_idx].cluster_idx,
                                     clusters[second_cluster_idx].cluster_idx + row_factor - 1));

                    /*
                     * Make sure that adding and removing the cluster we will get a legal cluster allocation.
                     */

                    if (mdb_verify_macro_cluster_allocation(unit, source_allocation & (~add_removed_clusters)) !=
                        _SHR_E_FAIL
                        && mdb_verify_macro_cluster_allocation(unit,
                                                               destination_allocation | add_removed_clusters) !=
                        _SHR_E_FAIL)
                    {
                        break;
                    }
                }
            }

            /*
             * If the second cluster to switch with has been found
             */
            if (second_cluster_idx < nof_valid_clusters)
            {
                break;
            }
        }
    }

    if (first_cluster_idx == nof_valid_clusters)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Unable to find two MDB clusters to switch in table: %s.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
    }

    sal_memcpy(&first_src_cluster, &clusters[first_cluster_idx], sizeof(first_src_cluster));
    /*
     * The first dst will have the properties of the second src, except for its cluster identification
     * The cluster at second_cluster_idx will replace the cluster at first_cluster_idx
     */
    sal_memcpy(&first_dst_cluster, &clusters[second_cluster_idx], sizeof(first_dst_cluster));
    first_dst_cluster.macro_type = clusters[first_cluster_idx].macro_type;
    first_dst_cluster.macro_idx = clusters[first_cluster_idx].macro_idx;
    first_dst_cluster.cluster_idx = clusters[first_cluster_idx].cluster_idx;

    sal_memcpy(&second_src_cluster, &clusters[second_cluster_idx], sizeof(second_src_cluster));

    for (cluster_iter = 0; cluster_iter < row_factor; cluster_iter++)
    {
        sal_memcpy(&src_clusters[2 * cluster_iter], &first_src_cluster, sizeof(first_src_cluster));
        sal_memcpy(&dst_clusters[cluster_iter], &first_dst_cluster, sizeof(first_dst_cluster));
        sal_memcpy(&src_clusters[2 * cluster_iter + 1], &second_src_cluster, sizeof(second_src_cluster));

        src_clusters[2 * cluster_iter].cluster_idx += cluster_iter;
        dst_clusters[cluster_iter].cluster_idx += cluster_iter;
        src_clusters[2 * cluster_iter + 1].cluster_idx += cluster_iter;
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Switching the two clusters:\n")));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "DBAL table: %s, macro_type: %d, macro_idx: %d, cluster_idx: %d.\n"),
              dbal_physical_table_to_string(unit, first_src_cluster.dbal_physical_table_id),
              first_src_cluster.macro_type, first_src_cluster.macro_idx, first_src_cluster.cluster_idx));
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "DBAL table: %s, macro_type: %d, macro_idx: %d, cluster_idx: %d.\n"),
              dbal_physical_table_to_string(unit, second_src_cluster.dbal_physical_table_id),
              second_src_cluster.macro_type, second_src_cluster.macro_idx, second_src_cluster.cluster_idx));

    /*
     * Switch the two clusters
     */
    SHR_IF_ERR_EXIT(mdb_init_delete_table_resources(unit, row_factor * 2 /* nof_valid_clusters */ , src_clusters));
    /*
     * Only assign second cluster instead of the first
     */
    SHR_IF_ERR_EXIT(mdb_init_add_table_resources(unit, row_factor * 1 /* nof_valid_clusters */ , dst_clusters));

    /*
     * Run the test on the DBAL table
     * Just the original table, as we do not replace the cluster in the second table
     */

    if (dnx_data_mdb.pdbs.pdb_info_get(unit, first_src_cluster.dbal_physical_table_id)->db_type ==
        DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, dbal_physical_table_id, mode, FALSE));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_direct_table_test(unit, dbal_physical_table_id, mode, FALSE));
    }

    /*
     * Restore the table resources to their previous value, by reversing the src/dst
     */
    for (cluster_iter = 0; cluster_iter < row_factor; cluster_iter++)
    {
        sal_memcpy(&dst_clusters[(cluster_iter * 2)], &first_src_cluster, sizeof(first_src_cluster));
        sal_memcpy(&src_clusters[cluster_iter], &first_dst_cluster, sizeof(first_dst_cluster));
        sal_memcpy(&dst_clusters[(cluster_iter * 2) + 1], &second_src_cluster, sizeof(second_src_cluster));

        dst_clusters[(cluster_iter * 2)].cluster_idx += cluster_iter;
        src_clusters[cluster_iter].cluster_idx += cluster_iter;
        dst_clusters[(cluster_iter * 2) + 1].cluster_idx += cluster_iter;
    }

    SHR_IF_ERR_EXIT(mdb_init_delete_table_resources(unit, row_factor * 1 /* nof_valid_clusters */ , src_clusters));
    SHR_IF_ERR_EXIT(mdb_init_add_table_resources(unit, row_factor * 2 /* nof_valid_clusters */ , dst_clusters));
    /*
     * Run the tests on the DBAL tables
     */
    if (dnx_data_mdb.pdbs.pdb_info_get(unit, first_src_cluster.dbal_physical_table_id)->db_type ==
        DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, first_src_cluster.dbal_physical_table_id, mode, FALSE));
        SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, second_src_cluster.dbal_physical_table_id, mode, FALSE));
    }
    else
    {
        SHR_IF_ERR_EXIT(mdb_direct_table_test(unit, first_src_cluster.dbal_physical_table_id, mode, FALSE));
        SHR_IF_ERR_EXIT(mdb_direct_table_test(unit, second_src_cluster.dbal_physical_table_id, mode, FALSE));
    }

exit:
    SHR_FREE(clusters);
    SHR_FREE(src_clusters);
    SHR_FREE(dst_clusters);

    SHR_FUNC_EXIT;
}

cmd_result_t
diag_mdb_test_physical_table(
    int unit,
    dbal_physical_tables_e table_id,
    uint32 app_id,
    mdb_test_mode_e mode,
    int dynamic,
    int delete_cmd,
    int full_time)
{
    dbal_enum_value_field_mdb_db_type_e mdb_db_type = dnx_data_mdb.pdbs.pdb_info_get(unit, table_id)->db_type;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\n*****diag_mdb_test_physical_table*****\n" "table=%s(%d), test_mode=%d, dynamic=%d\n"),
             dbal_physical_table_to_string(unit, table_id), table_id, mode, dynamic));

#if defined(ADAPTER_SERVER_MODE)
    if (((mode == MDB_TEST_CAPACITY) || (mode == MDB_TEST_CAPACITY_MIXED_EE))
        && (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Capacity test is not available over cmodel.\n");
    }
#endif

    if ((mode == MDB_TEST_CAPACITY_MIXED_EE) && !(mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Capacity mixed entry encoding test is only available for EM.\n");
    }

    if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM)
    {
        LOG_CLI((BSL_META("This API does not support TCAM tests.\n")));
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        LOG_CLI((BSL_META("Physical table %d associated with LPM table.\n"), table_id));
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
        if (TRUE)
        {
            if (dynamic == 0)
            {
                SHR_IF_ERR_EXIT(mdb_lpm_test(unit, table_id, mode, delete_cmd, full_time));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic table tests are not supported for LPM tables\n");
            }
        }
        else
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
        {
            SHR_ERR_EXIT(_SHR_E_DISABLED, "Compilation does not include KBPSDK flags\n");
        }
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM)
    {
        LOG_CLI((BSL_META("Physical table %d associated with EM.\n"), table_id));
        if (dynamic == 0)
        {
            SHR_IF_ERR_EXIT(mdb_em_test(unit, table_id, app_id, mode, delete_cmd, full_time));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic table tests are not supported for EM tables\n");
        }
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_DIRECT)
    {
        LOG_CLI((BSL_META("Physical table %d associated with direct table.\n"), table_id));
        if (dynamic == 0)
        {
            SHR_IF_ERR_EXIT(mdb_direct_table_test(unit, table_id, mode, full_time));
        }
        else if (dynamic == 1)
        {
            /*
             * This test is also relevant for FEC
             */
            SHR_IF_ERR_EXIT(mdb_eedb_dynamic_cluster_test(unit, table_id, mode));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic table tests are not supported for direct tables\n");
        }
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EEDB)
    {
        LOG_CLI((BSL_META("Physical table %d associated with EEDB table.\n"), table_id));
        if (dynamic == 0)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_table_test(unit, table_id, mode, full_time));
        }
        else if (dynamic == 1)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_dynamic_cluster_test(unit, table_id, mode));
        }
        else if (dynamic == 2)
        {
            SHR_IF_ERR_EXIT(mdb_eedb_dynamic_bank_test(unit, table_id, mode));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic table tests are not supported for EEDB tables\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Physical table %d not associated with TCAM/LPM/EM/Direct/EEDB\n", table_id);
    }

    LOG_CLI((BSL_META("MDB Table test passed!\n")));

    /*
     * sal_srand(sal_time_usecs());
     */

exit:
    SHR_FUNC_EXIT;
}

sh_sand_invoke_t dnx_mdb_tests[] = {
    {"lpm_private", "table=KAPS1", CTEST_PRECOMMIT}
    ,
    {"lpm_public", "table=KAPS2", CTEST_PRECOMMIT}
    ,
    {"ivsi", "table=IVSI", CTEST_PRECOMMIT}
    ,
    {"isem_1", "table=ISEM1", CTEST_PRECOMMIT}
    ,
    {"isem_2", "table=ISEM2", CTEST_PRECOMMIT}
    ,
    {"isem_3", "table=ISEM3", CTEST_PRECOMMIT}
    ,
    {"inlif_1", "table=INLIF1", CTEST_PRECOMMIT}
    ,
    {"inlif_2", "table=INLIF2", CTEST_PRECOMMIT}
    ,
    {"inlif_3", "table=INLIF3", CTEST_PRECOMMIT}
    ,
    {"lem", "table=LEM", CTEST_PRECOMMIT}
    ,
    {"ioem_1", "table=IOEM1", CTEST_PRECOMMIT}
    ,
    {"ioem_2", "table=IOEM2", CTEST_PRECOMMIT}
    ,
    {"map", "table=MAP", CTEST_PRECOMMIT}
    ,
    {"fec_1", "table=FEC1", CTEST_PRECOMMIT}
    ,
    {"fec_2", "table=FEC2", CTEST_PRECOMMIT}
    ,
    {"fec_3", "table=FEC3", CTEST_PRECOMMIT}
    ,
    {"fec_3_dynamic_cluster", "table=FEC3 dynamic=1", CTEST_POSTCOMMIT}
    ,
    {"ppmc", "table=PPMC", CTEST_PRECOMMIT}
    ,
    {"glem_1", "table=GLEM1", CTEST_PRECOMMIT}
    ,
    {"glem_2", "table=GLEM2", CTEST_PRECOMMIT}
    ,
    {"eoem_1", "table=EOEM1", CTEST_PRECOMMIT}
    ,
    {"eoem_2", "table=EOEM2", CTEST_PRECOMMIT}
    ,
    {"esem", "table=ESEM", CTEST_PRECOMMIT}
    ,
    {"evsi", "table=EVSI", CTEST_PRECOMMIT}
    ,
    {"sexem_1", "table=SEXEM1", CTEST_PRECOMMIT}
    ,
    {"sexem_2", "table=SEXEM2", CTEST_PRECOMMIT}
    ,
    {"sexem_3", "table=SEXEM3", CTEST_PRECOMMIT}
    ,
    {"lexem", "table=LEXEM", CTEST_PRECOMMIT}
    ,
    {"rmep", "table=RMEP_EM", CTEST_PRECOMMIT}
    ,
    {"eedb1", "table=EEDB1", CTEST_PRECOMMIT}
    ,
    {"eedb2", "table=EEDB2", CTEST_PRECOMMIT}
    ,
    {"eedb3", "table=EEDB3", CTEST_PRECOMMIT}
    ,
    {"eedb4", "table=EEDB4", CTEST_PRECOMMIT}
    ,
    {"eedb5", "table=EEDB5", CTEST_PRECOMMIT}
    ,
    {"eedb6", "table=EEDB6", CTEST_PRECOMMIT}
    ,
    {"eedb7", "table=EEDB7", CTEST_PRECOMMIT}
    ,
    {"eedb8", "table=EEDB8", CTEST_PRECOMMIT}
    ,
#if !defined(ADAPTER_SERVER_MODE)
    {"lpm_private_capacity", "table=KAPS1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"lpm_public_capacity", "table=KAPS2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"ivsi_capacity", "table=IVSI mode=1", CTEST_POSTCOMMIT}
    ,
    {"isem_1_capacity", "table=ISEM1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"isem_2_capacity", "table=ISEM2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"isem_3_capacity", "table=ISEM3 mode=1", CTEST_POSTCOMMIT}
    ,
    {"inlif_1_capacity", "table=INLIF1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"inlif_2_capacity", "table=INLIF2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"inlif_3_capacity", "table=INLIF3 mode=1", CTEST_POSTCOMMIT}
    ,
    {"lem_capacity", "table=LEM mode=1", CTEST_POSTCOMMIT}
    ,
    {"ioem_1_capacity", "table=IOEM1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"ioem_2_capacity", "table=IOEM2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"map_capacity", "table=MAP mode=1", CTEST_POSTCOMMIT}
    ,
    {"fec_1_capacity", "table=FEC1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"fec_2_capacity", "table=FEC2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"fec_3_capacity", "table=FEC3 mode=1", CTEST_POSTCOMMIT}
    ,
    {"ppmc_capacity", "table=PPMC mode=1", CTEST_POSTCOMMIT}
    ,
    {"glem_1_capacity", "table=GLEM1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"glem_2_capacity", "table=GLEM2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"eoem_1_capacity", "table=EOEM1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"eoem_2_capacity", "table=EOEM2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"esem_capacity", "table=ESEM mode=1", CTEST_POSTCOMMIT}
    ,
    {"evsi_capacity", "table=EVSI mode=1", CTEST_POSTCOMMIT}
    ,
    {"sexem_1_capacity", "table=SEXEM1 mode=1", CTEST_POSTCOMMIT}
    ,
    {"sexem_2_capacity", "table=SEXEM2 mode=1", CTEST_POSTCOMMIT}
    ,
    {"sexem_3_capacity", "table=SEXEM3 mode=1", CTEST_POSTCOMMIT}
    ,
    {"lexem_capacity", "table=LEXEM mode=1", CTEST_POSTCOMMIT}
    ,
    {"rmep_capacity", "table=RMEP_EM mode=1", CTEST_POSTCOMMIT}
    ,
    {"isem_1_capacity_mixed_ar", "table=ISEM1 mode=2", CTEST_POSTCOMMIT}
    ,
    {"isem_2_capacity_mixed_ar", "table=ISEM2 mode=2", CTEST_POSTCOMMIT}
    ,
    {"isem_3_capacity_mixed_ar", "table=ISEM3 mode=2", CTEST_POSTCOMMIT}
    ,
    {"lem_capacity_mixed_ar", "table=LEM mode=2", CTEST_POSTCOMMIT}
    ,
    {"ioem_1_capacity_mixed_ar", "table=IOEM1 mode=2", CTEST_POSTCOMMIT}
    ,
    {"ioem_2_capacity_mixed_ar", "table=IOEM2 mode=2", CTEST_POSTCOMMIT}
    ,
    {"ppmc_capacity_mixed_ar", "table=PPMC mode=2", CTEST_POSTCOMMIT}
    ,
    {"glem_1_capacity_mixed_ar", "table=GLEM1 mode=2", CTEST_POSTCOMMIT}
    ,
    {"glem_2_capacity_mixed_ar", "table=GLEM2 mode=2", CTEST_POSTCOMMIT}
    ,
    {"eoem_1_capacity_mixed_ar", "table=EOEM1 mode=2", CTEST_POSTCOMMIT}
    ,
    {"eoem_2_capacity_mixed_ar", "table=EOEM2 mode=2", CTEST_POSTCOMMIT}
    ,
    {"esem_capacity_mixed_ar", "table=ESEM mode=2", CTEST_POSTCOMMIT}
    ,
    {"sexem_1_capacity_mixed_ar", "table=SEXEM1 mode=2", CTEST_POSTCOMMIT}
    ,
    {"sexem_2_capacity_mixed_ar", "table=SEXEM2 mode=2", CTEST_POSTCOMMIT}
    ,
    {"sexem_3_capacity_mixed_ar", "table=SEXEM3 mode=2", CTEST_POSTCOMMIT}
    ,
    {"lexem_capacity_mixed_ar", "table=LEXEM mode=2", CTEST_POSTCOMMIT}
    ,
    {"rmep_capacity_mixed_ar", "table=RMEP_EM mode=2", CTEST_POSTCOMMIT}
    ,
#endif
    {NULL}
};

sh_sand_option_t dnx_mdb_test_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "DBAL physical table name", ""}
    ,
    {"MoDe", SAL_FIELD_TYPE_INT32,
     "The test mode: 0-fast (tests ~1/10 of the capacity), 1-full(direct)/capacity test (EM/LPM) 2-capacity test mixed entry encoding (EM)",
     "0"}
    ,
    {"all", SAL_FIELD_TYPE_BOOL, "Ignore Table and run the test on all table", "No"}
    ,
    {"DeLeTe", SAL_FIELD_TYPE_BOOL, "Perform a delete operation as part of the test, relevant for EM and LPM", "Yes"}
    ,
    {"DYNAMIC", SAL_FIELD_TYPE_INT32,
     "Run a dynamic variation of the test. For EEDB: 1-dynamic EEDB bank, 2-dynamic MDB cluster", "0"}
    ,
    {"app_id", SAL_FIELD_TYPE_UINT32, "The APP ID", "0xFFFFFFFF"}
    ,
    {"FULL", SAL_FIELD_TYPE_BOOL,
     "Include entry preparation time and descriptor DMA (if enabled) commit to HW in the time measurement.", "No"}
    ,
    {NULL}
};

sh_sand_man_t dnx_mdb_test_man = {
    "Test the MDB physical table operation.",
    "Test the different MDB physical tables by adding random entries, verifying the entries through get and deleting them. "
        "Running with \"all\" option tests all tables and ignores the table parameter. "
        "Mode allows toggling between a short test(0) and a long test(1)."
        "Dynamic allows running dynamic allocation tests on the table, MDB cluster(1), EEDB bank(2).",
    "ctest mdb [all] [table=DBAL_PHYSICAL_TABLE_NAME] [MoDe=0/1/2..]",
    "all\n" "table=LEM mode=1"
};

/**
* \brief
* Change the severity of the MDB module,
* if no parameter than dump the current severity
*****************************************************/
shr_error_e
cmd_mdb_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#if !defined(PLISIM) || defined(ADAPTER_SERVER_MODE)
    int all;
    int dynamic;
    int mode;
    char *table_name = NULL;
    int delete_cmd;
    uint32 app_id;
    int full_time;
#endif

    SHR_FUNC_INIT_VARS(unit);

#if defined(PLISIM) && !defined(ADAPTER_SERVER_MODE)
    LOG_CLI((BSL_META("MDB tests are not available over pcid.\n")));
    SHR_EXIT();
#else
    SH_SAND_GET_STR("Table", table_name);
    SH_SAND_GET_INT32("MoDe", mode);
    SH_SAND_GET_BOOL("all", all);
    SH_SAND_GET_INT32("DYNAMIC", dynamic);
    SH_SAND_GET_BOOL("DeLeTe", delete_cmd);
    SH_SAND_GET_UINT32("app_id", app_id);
    SH_SAND_GET_BOOL("FULL", full_time);

    if (mode > MDB_NOF_TEST_MODES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The mode is out of range.\n");
    }

    /*
     * Ignore table and run the test on all tables
     */
    if (all != 0)
    {
        int ii;
        int rv_array[DBAL_NOF_PHYSICAL_TABLES];
        int nof_tables_pass = 0;
        int nof_tables_failed = 0;
        for (ii = DBAL_PHYSICAL_TABLE_NONE; ii < DBAL_NOF_PHYSICAL_TABLES; ii++)
        {
            /*
             * Simply increment the counter for TCAM/KBP
             */
            if ((ii == DBAL_PHYSICAL_TABLE_TCAM) || (ii == DBAL_PHYSICAL_TABLE_KBP) || (ii == DBAL_PHYSICAL_TABLE_NONE))
            {
                rv_array[ii] = CMD_OK;
                nof_tables_pass++;
                continue;
            }
            rv_array[ii] = diag_mdb_test_physical_table(unit, ii, app_id, mode, dynamic, delete_cmd, full_time);
            if (rv_array[ii] == CMD_OK)
            {
                nof_tables_pass++;
            }
        }

        nof_tables_failed = DBAL_NOF_PHYSICAL_TABLES - nof_tables_pass;
        LOG_CLI((BSL_META("\n\nMDB test all Summary: %d tables passed/skipped, %d tables failed.\n"),
                 nof_tables_pass, nof_tables_failed));

        if (nof_tables_failed != 0)
        {
            LOG_CLI((BSL_META("Failed tables:")));
            for (ii = DBAL_PHYSICAL_TABLE_NONE; ii < DBAL_NOF_PHYSICAL_TABLES; ii++)
            {
                if (rv_array[ii] != CMD_OK)
                {
                    LOG_CLI((BSL_META(" %s(%d)"), dbal_physical_table_to_string(unit, ii), ii));
                }
            }
            LOG_CLI((BSL_META("\n")));
        }
    }
    else
    {
        if (ISEMPTY(table_name))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The Table parameter must be specified if not running on all.\n");
        }
        else
        {
            dbal_physical_tables_e dbal_physical_table_id;
            SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id(unit, table_name, &dbal_physical_table_id));

            SHR_IF_ERR_EXIT(diag_mdb_test_physical_table
                            (unit, dbal_physical_table_id, app_id, mode, dynamic, delete_cmd, full_time));
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

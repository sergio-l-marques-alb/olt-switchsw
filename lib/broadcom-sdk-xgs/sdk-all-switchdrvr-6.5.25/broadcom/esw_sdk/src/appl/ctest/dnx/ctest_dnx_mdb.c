/*
 * ! \file mdb_diag.c Contains all of the MDB diag commands
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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

#include <soc/dnx/mdb_internal_shared.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>

#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <soc/mcm/memregs.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <appl/diag/diag.h>

#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_bitstream.h>

#include <soc/dnx/mdb.h>
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
    int max_nof_db_clusters = dnx_data_mdb.dh.max_nof_db_clusters_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(clusters, sizeof(mdb_cluster_alloc_info_t) * max_nof_db_clusters,
                       "mdb_eedb_dynamic_bank_test_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(src_clusters, sizeof(mdb_cluster_alloc_info_t) * max_nof_db_clusters,
                       "mdb_eedb_dynamic_bank_test_src_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(dst_clusters, sizeof(mdb_cluster_alloc_info_t) * max_nof_db_clusters,
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
        if ((clusters[first_cluster_idx].macro_type == MDB_EEDB_BANK)
            && (clusters[first_cluster_idx].logical_start_address != MDB_DIRECT_INVALID_START_ADDRESS))
        {
            /*
             * Search for a second EEDB bank to modify its allocation, start from the end, clusters allocated to other tables
             * are at the end
             */
            for (second_cluster_idx = nof_valid_clusters - 1; second_cluster_idx >= 0; second_cluster_idx--)
            {
                uint32 phase_bank_index;
                int phase_index = clusters[second_cluster_idx].dbal_physical_table_id - DBAL_PHYSICAL_TABLE_EEDB_1;

                if ((clusters[second_cluster_idx].macro_type == MDB_EEDB_BANK)
                    && (clusters[second_cluster_idx].logical_start_address != MDB_DIRECT_INVALID_START_ADDRESS))
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

    if (first_cluster_idx == nof_valid_clusters)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Unable to find two MDB banks to switch in table: %s.\n",
                     dbal_physical_table_to_string(unit, dbal_physical_table_id));
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
    int max_nof_db_clusters = dnx_data_mdb.dh.max_nof_db_clusters_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(clusters, sizeof(mdb_cluster_alloc_info_t) * max_nof_db_clusters,
                       "mdb_eedb_dynamic_bank_test_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(src_clusters, sizeof(mdb_cluster_alloc_info_t) * max_nof_db_clusters,
                       "mdb_eedb_dynamic_bank_test_src_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(dst_clusters, sizeof(mdb_cluster_alloc_info_t) * max_nof_db_clusters,
                       "mdb_eedb_dynamic_bank_test_dst_clusters", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    row_width = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, dbal_physical_table_id)->row_width;
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
            && (clusters[first_cluster_idx].logical_start_address != MDB_DIRECT_INVALID_START_ADDRESS)
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
                    (clusters[second_cluster_idx].logical_start_address != MDB_DIRECT_INVALID_START_ADDRESS) &&
                    (clusters[first_cluster_idx].dbal_physical_table_id !=
                     clusters[second_cluster_idx].dbal_physical_table_id))
                {
                    mdb_physical_table_e mdb_table_id_dst = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                                                clusters
                                                                                                [first_cluster_idx].
                                                                                                dbal_physical_table_id)->logical_to_physical;

                    mdb_physical_table_e mdb_table_id_src = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit,
                                                                                                clusters
                                                                                                [second_cluster_idx].
                                                                                                dbal_physical_table_id)->logical_to_physical;

                    uint32 source_allocation, destination_allocation;
                    uint32 add_removed_clusters = 0;
                    if (mdb_table_id_dst == MDB_NOF_PHYSICAL_TABLES || mdb_table_id_src == MDB_NOF_PHYSICAL_TABLES)
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

    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, first_src_cluster.dbal_physical_table_id)->db_type ==
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
    if (dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, first_src_cluster.dbal_physical_table_id)->db_type ==
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
    dbal_enum_value_field_mdb_db_type_e mdb_db_type = dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, table_id)->db_type;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("\n*****diag_mdb_test_physical_table*****\n" "table=%s(%d), test_mode=%d, dynamic=%d\n"),
             dbal_physical_table_to_string(unit, table_id), table_id, mode, dynamic));

#if defined(ADAPTER_SERVER_MODE)
    if (((mode == MDB_TEST_CAPACITY) || (mode == MDB_TEST_CAPACITY_MIXED_EE))
        && (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Capacity test is not available over adapter.\n");
    }
#endif

    if ((mode == MDB_TEST_CAPACITY_MIXED_EE) && !(mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_EM))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Capacity mixed entry encoding test is only available for EM.\n");
    }

    if ((mode == MDB_TEST_SER) && (mdb_db_type != DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "SER tests through the MDB are only available for KAPS TCAM.\n");
    }

    if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_TCAM)
    {
        LOG_CLI((BSL_META("This API does not support TCAM tests.\n")));
    }
    else if (mdb_db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_KAPS)
    {
        LOG_CLI((BSL_META("Physical table %d associated with LPM table.\n"), table_id));
        if (dynamic == 0)
        {
            if (mode == MDB_TEST_SER)
            {
                SHR_IF_ERR_EXIT(mdb_lpm_ser_test(unit, table_id));
            }
            else
            {
                SHR_IF_ERR_EXIT(mdb_lpm_test(unit, table_id, mode, delete_cmd, full_time));
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic table tests are not supported for LPM tables\n");
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
    {"all_fast", "all mode=fast", CTEST_PRECOMMIT}
    ,
    {"all_full", "all mode=full", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"fec_3_dynamic_cluster", "table=FEC3 dynamic=1", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"lem_capacity", "table=LEM mode=full app_id=0", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"lem_capacity_mact", "table=LEM mode=full app_id=2", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"all_em_capacity_mixed_ar", "all mode=mixed_entry", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"lem_sw_hw_iterator", "table=lem mode=iterator", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"kaps_1_ser", "table=kaps1 mode=ser_tcam", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"kaps_2_ser", "table=kaps2 mode=ser_tcam", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"lem_hash_packing", "table=lem mode=hash_packing", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {"isem_2_table_clear", "table=ISEM2 mode=clear_test", CTEST_POSTCOMMIT * CTEST_ADAPTER_UNSUPPORTED}
    ,
    {NULL}
};

sh_sand_option_t dnx_mdb_test_options[] = {
    {"Table", SAL_FIELD_TYPE_STR, "DBAL physical table name", ""}
    ,
    {"MoDe", SAL_FIELD_TYPE_STR,
     "The test modes: fast -fast (tests ~1/10 of the capacity), full-full(direct)/capacity test (EM/LPM) mixed_entry-capacity test mixed entry encoding (EM) iterator-MDB iterators test(SW and HW), ser_tcam-SER KAPS TCAM only, hitbit_fast-Hitbit basic test, hitbit_full-hitbit capacity test, hash_packing-Single hash packing test, clear_test-EM table clear test",
     "fast"}
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
* converts MDB test mode from string to int
* returns the mode number if success, shr_error_e error number otherwise
*****************************************************/
shr_error_e
mdb_test_mode_string_to_int(
    int unit,
    char *mode_name,
    int *mode)
{

    int mode_iter;
    char *modes_names[MDB_NOF_TEST_MODES] =
        { "fast", "full", "mixed_entry", "iterator", "ser_tcam", "hitbit_fast", "hitbit_full", "hash_packing",
        "clear_test"
    };
    SHR_FUNC_INIT_VARS(unit);
    for (mode_iter = 0; mode_iter < MDB_NOF_TEST_MODES; mode_iter++)
    {
        if (sal_strcmp(modes_names[mode_iter], mode_name) == 0)
        {
            *mode = mode_iter;
            SHR_EXIT();
            return *mode;
        }
    }
    SHR_ERR_EXIT(_SHR_E_PARAM, "Unrecognized mdb test mode.\n");

exit:
    SHR_FUNC_EXIT;
}

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
    char *mode_name = NULL;
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
    SH_SAND_GET_STR("MoDe", mode_name);
    SH_SAND_GET_BOOL("all", all);
    SH_SAND_GET_INT32("DYNAMIC", dynamic);
    SH_SAND_GET_BOOL("DeLeTe", delete_cmd);
    SH_SAND_GET_UINT32("app_id", app_id);
    SH_SAND_GET_BOOL("FULL", full_time);

    SHR_IF_ERR_EXIT(mdb_test_mode_string_to_int(unit, mode_name, &mode));
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
             * Simply increment the counter for TCAM/KBP/unsupported tables on current device and unsupported adapter stub tables
             */
            if ((dnx_data_mdb.pdbs.dbal_pdb_info_get(unit, ii)->db_type == DBAL_ENUM_FVAL_MDB_DB_TYPE_NONE))
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
            shr_error_e rv;

            rv = dbal_physical_table_string_to_id(unit, table_name, &dbal_physical_table_id);
            if (rv == _SHR_E_NOT_FOUND)
            {
                /*
                 * Do not return an error
                 * Different devices may use different table names
                 * Diag examples are not device specific
                 */
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Table not found.\n")));
                SHR_EXIT();
            }
            else
            {
                if (rv != _SHR_E_NONE)
                {
                    SHR_ERR_EXIT(rv, "dbal_physical_table_string_to_id returned an error.\n");
                }
            }

            SHR_IF_ERR_EXIT(diag_mdb_test_physical_table
                            (unit, dbal_physical_table_id, app_id, mode, dynamic, delete_cmd, full_time));
        }
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

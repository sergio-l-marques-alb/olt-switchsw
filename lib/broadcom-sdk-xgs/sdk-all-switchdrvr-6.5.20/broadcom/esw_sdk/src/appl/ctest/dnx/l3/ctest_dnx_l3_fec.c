/** \file ctest_dnx_l3_fec.c
 * $Id$
 *
 * Tests for L3 fecs
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

 /*
  * Include files.
  * {
  */
#include <bcm/l3.h>
#include <bcm/failover.h>
#include <shared/bsl.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/utilex/utilex_framework.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <src/soc/dnx/mdb/mdb_internal.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#undef BSL_LOG_MODULE
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/*
 * }
 */

/*
 * Globals.
 * {
 */
extern const dbal_enum_value_field_mdb_physical_table_e
    mdb_fec_hierarchy_index_to_fec_table_map[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES];

static sh_sand_man_t sh_dnx_l3_fec_allocation_man = {
    "Allocation test for bcm_l3_egress_create INGRESS_ONLY",
    "Verifies the FEC allocation with and without protection and creating ECMP groups."
};

static sh_sand_man_t sh_dnx_l3_fec_internal_allocation_man = {
    "Test the internal FEC ranges allocation process.",
    "Verifies that the internal allocation sets legal FEC ranges."
};

/*
 * }
 */

/**
 * \brief
 * Main fec allocation testing command
 */
static shr_error_e
sh_dnx_l3_fec_allocation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx;
    const int nof_fecs = 100;
    const int intf_count = 10;
    bcm_l3_egress_ecmp_t *ecmp_list;
    bcm_l3_egress_t *primary_fec_list;
    bcm_l3_egress_t *failover_fec_list;
    bcm_l3_egress_t *fec_list;

    int *failover_fec_id_p = NULL;
    int *primary_fec_id_p = NULL;
    int *fec_id_p = NULL;

    bcm_failover_t fail_id = 200;
    bcm_if_t fecs_buffer[100];

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(failover_fec_id_p, sizeof(*failover_fec_id_p) * nof_fecs,
              "failover_fec_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(primary_fec_id_p, sizeof(*primary_fec_id_p) * nof_fecs,
              "primary_fec_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(fec_id_p, sizeof(*fec_id_p) * nof_fecs, "fec_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    LOG_INFO(BSL_LS_BCM_L3, (BSL_META_U(unit, "FEC allocation test start\n")));
    /** allocate dynamic arrays */
    primary_fec_list = sal_alloc(sizeof(bcm_l3_egress_t) * nof_fecs, "fec_allocation_main.primary_fec_list");
    failover_fec_list = sal_alloc(sizeof(bcm_l3_egress_t) * nof_fecs, "fec_allocation_main.failover_fec_list");
    fec_list = sal_alloc(sizeof(bcm_l3_egress_t) * nof_fecs, "fec_allocation_main.fec_list");
    ecmp_list = sal_alloc(sizeof(bcm_l3_egress_ecmp_t) * nof_fecs, "fec_allocation_main.ecmp_list");

    /** Creating the Failover ID */
    SHR_IF_ERR_EXIT(bcm_failover_create(unit, BCM_FAILOVER_FEC, &fail_id));

    /** STAGE 1: Create FECs with and without protection */
    /** Create FEC pairs with protection - primary and failover */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        bcm_l3_egress_t_init(&failover_fec_list[idx]);
        failover_fec_list[idx].failover_id = fail_id;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY, &failover_fec_list[idx], &failover_fec_id_p[idx]));

        bcm_l3_egress_t_init(&primary_fec_list[idx]);
        primary_fec_list[idx].failover_id = fail_id;
        primary_fec_list[idx].failover_if_id = failover_fec_id_p[idx];
        SHR_IF_ERR_EXIT(bcm_l3_egress_create
                        (unit, BCM_L3_INGRESS_ONLY, &primary_fec_list[idx], &primary_fec_id_p[idx]));
    }

    /** Create FEC list without protection */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        bcm_l3_egress_t_init(&fec_list[idx]);
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &fec_list[idx], &fec_id_p[idx]));
    }

    for (idx = 0; idx < nof_fecs; idx++)
    {
        uint8 is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(failover_fec_id_p[idx]), &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEC %d is not found but it should be allocated\n", failover_fec_id_p[idx]);
        }
        is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(primary_fec_id_p[idx]), &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEC %d is not found but it should be allocated\n", primary_fec_id_p[idx]);
        }
        is_allocated = FALSE;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(fec_id_p[idx]), &is_allocated));
        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FEC %d is not found but it should be allocated\n", fec_id_p[idx]);
        }
    }

    /** STAGE 2: Create ECMP groups */
    /** Case 1: Valid input - FECs without protection */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[0]);
    ecmp_list[0].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[0], intf_count, fec_id_p));

    /** Case 2: valid input - FECs with protection (primary)*/
    bcm_l3_egress_ecmp_t_init(&ecmp_list[1]);
    ecmp_list[1].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[1], intf_count, primary_fec_id_p));

    /** Case 3: invalid input - regular FECs unordered - size doesn't match lowest and biggest element */
    fecs_buffer[0] = fec_id_p[31];
    fecs_buffer[1] = fec_id_p[14];
    fecs_buffer[2] = fec_id_p[15];
    fecs_buffer[3] = fec_id_p[12];
    fecs_buffer[4] = fec_id_p[16];
    bcm_l3_egress_ecmp_t_init(&ecmp_list[2]);
    ecmp_list[2].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[2], intf_count / 2, fecs_buffer),
                              _SHR_E_PARAM);

    /** Case 4: invalid input - different protection state. */
    fecs_buffer[0] = fec_id_p[13];
    fecs_buffer[1] = primary_fec_id_p[11];
    fecs_buffer[2] = fec_id_p[17];
    fecs_buffer[3] = primary_fec_id_p[12];
    fecs_buffer[4] = fec_id_p[18];
    bcm_l3_egress_ecmp_t_init(&ecmp_list[3]);
    ecmp_list[3].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[3], intf_count / 2, fecs_buffer),
                              _SHR_E_PARAM);

    /** Case 5: invalid input - different protection state */
    fecs_buffer[0] = fec_id_p[19];
    fecs_buffer[1] = failover_fec_id_p[13];
    fecs_buffer[2] = fec_id_p[2];
    fecs_buffer[3] = failover_fec_id_p[12];
    fecs_buffer[4] = fec_id_p[3];
    bcm_l3_egress_ecmp_t_init(&ecmp_list[8]);
    ecmp_list[8].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[4], intf_count / 2, fecs_buffer),
                              _SHR_E_PARAM);

    /** Case 6: invalid input - failover FECs */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[4]);
    ecmp_list[4].max_paths = intf_count / 2;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_create(unit, &ecmp_list[5], intf_count / 2, &failover_fec_id_p[60]),
                              _SHR_E_PARAM);

    /** Case 7: valid input - FECs with no protection */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[6]);
    ecmp_list[6].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[6], intf_count, &fec_id_p[60]));

    /** Case 8: valid input - FECs with protection */
    bcm_l3_egress_ecmp_t_init(&ecmp_list[7]);
    ecmp_list[7].max_paths = intf_count;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp_list[7], intf_count, &primary_fec_id_p[60]));

    /** STAGE 3: adding FECs to existing ECMP groups */
    /** Case 1: adding a FEC without protection to an ECMP which has no protection - correct index*/
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp_list[0], fec_id_p[10]));

    /** Case 2: adding a FEC with protection to an ECMP without protection */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[0], primary_fec_id_p[39]), _SHR_E_PARAM);

    /** Case 3: adding a FEC without protection to an ECMP with protection */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[1], fec_id_p[40]), _SHR_E_PARAM);

    /** Case 4: adding a FEC with protection to ECMP with protection - correct index */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp_list[1], primary_fec_id_p[10]));

    /** Case 5: adding a FEC without protection to an ECMP without protection - index is out of range. */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[6], fec_id_p[2]), _SHR_E_PARAM);

    /** Case 6: adding a FEC with protection to ECMP without protection. */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[6], primary_fec_id_p[45]), _SHR_E_PARAM);

    /** Case 7: adding a FEC to a group that doesn't exist. */
    ecmp_list[8].ecmp_intf = 8;
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_add(unit, &ecmp_list[8], fec_id_p[50]), _SHR_E_NOT_FOUND);

    /** STAGE 4: Removing FECs from ECMP groups */
    /** Case 1: removing a FEC without protection from an ECMP without protection - from the beginning */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[6], fec_id_p[60]));

    /** Case 2: removing a FEC without protection from an ECMP without protection - from the middle */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[6], fec_id_p[65]), _SHR_E_PARAM);

    /** Case 3: removing a FEC with protection from an ECMP with protection - from the middle */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[1], primary_fec_id_p[5]), _SHR_E_PARAM);

    /** Case 3: removing a FEC with protection from an ECMP with protection - from the middle */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[1], primary_fec_id_p[10]));

    /** Case 4: removing a FEC without protection from an ECMP group to which it doesn't belong. */
    SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_delete(unit, &ecmp_list[1], fec_id_p[0]), _SHR_E_PARAM);

    /** STAGE 5: Clean up */
    /** destroying ECMP groups */
    for (idx = 0; idx < 8; idx++)
    {
        SHR_IF_ERR_EXIT_EXCEPT_IF(bcm_l3_egress_ecmp_destroy(unit, &ecmp_list[idx]), _SHR_E_PARAM);
    }

    /** Destroying FECs */
    for (idx = 0; idx < nof_fecs; idx++)
    {
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fec_id_p[idx]));
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, primary_fec_id_p[idx]));
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, failover_fec_id_p[idx]));
    }

    /** Destroying failover ID */
    SHR_IF_ERR_EXIT(bcm_failover_destroy(unit, fail_id));

    LOG_INFO(BSL_LS_BCM_L3, (BSL_META_U(unit, "FEC allocation test end. PASS\n")));
exit:
    SHR_FREE(failover_fec_id_p);
    SHR_FREE(primary_fec_id_p);
    SHR_FREE(fec_id_p);
    sal_free(primary_fec_list);
    sal_free(failover_fec_list);
    sal_free(fec_list);
    sal_free(ecmp_list);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Update the SW state cluster with the give allocation and also update the expected bank allocation
 * macro_allocation - The number of cluster that each hierarchy holds in each macro
 * hierarhcy_start_pos - the expected first bank of each hierarchy
 * expected_allocation - the expected allocation array which is returned from this function.
 */
shr_error_e
ctest_dnx_l3_fec_update_sw_state_clusters_and_expected_res(
    int unit,
    uint8 macro_allocation[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES][DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES],
    int hierarhcy_start_pos[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES],
    uint8 expected_allocation[DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS])
{

    uint32 cluster_iter, global_macro_iter, fec_hier_iter, fec_pair_iter;
    int macro_index;
    int nof_banks[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] = { 0 };
    int cluster_ptr[DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES] = { 0 };
    mdb_macro_types_e macro_type;
    mdb_cluster_info_t cluster;
    SHR_FUNC_INIT_VARS(unit);

    if (expected_allocation != NULL)
    {
        sal_memset(expected_allocation, 0, DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS);
    }

    /*
     * Update the SW state FEC clusters.
     */
    for (fec_hier_iter = 0; fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        int nof_clusters = 0;
        for (global_macro_iter = 0; global_macro_iter < DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES; global_macro_iter++)
        {
            macro_type = dnx_data_mdb.dh.logical_macro_info_get(unit, global_macro_iter)->macro_type;
            macro_index = dnx_data_mdb.dh.logical_macro_info_get(unit, global_macro_iter)->macro_index;

            for (cluster_iter = 0; cluster_iter < macro_allocation[fec_hier_iter][global_macro_iter]; cluster_iter++)
            {
                for (fec_pair_iter = 0; fec_pair_iter < 2; fec_pair_iter++)
                {
                    cluster.macro_type = macro_type;
                    cluster.macro_index = macro_index;
                    cluster.cluster_index = cluster_ptr[global_macro_iter]++;
                    cluster.start_address = -1;
                    cluster.end_address = -1;
                    SHR_IF_ERR_EXIT(mdb_db_infos.db.
                                    clusters_info.set(unit, mdb_fec_hierarchy_index_to_fec_table_map[fec_hier_iter],
                                                      nof_clusters++, &cluster));
                }

                nof_banks[fec_hier_iter] += MDB_MACRO_NOF_FEC_BANKS_GET(macro_type);
            }
        }
        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                        number_of_clusters.set(unit, mdb_fec_hierarchy_index_to_fec_table_map[fec_hier_iter],
                                               nof_clusters));
/*
 * Update the expected allocation ranges
 */
        if (expected_allocation != NULL)
        {
            sal_memset(&expected_allocation[hierarhcy_start_pos[fec_hier_iter]], fec_hier_iter + 1,
                       nof_banks[fec_hier_iter]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify that the FEC internal function allocated the FEC banks as expected.
 * expected_allocation - the expected allocation
 */
shr_error_e
ctest_dnx_l3_fec_verify_allocation(
    int unit,
    uint8 *expected_allocation)
{
    int nof_clusters;
    mdb_cluster_info_t cluster;
    uint32 bank;
    uint32 fec_hier_iter, cluster_iter, bank_iter;
    uint8 received_allocation[DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS];
    int macro_global_index[DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS];
    uint32 first_bank[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] = { DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS };
    uint32 nof_banks[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES];
    uint8 checked[DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS];
    uint32 high_granularity_max_bank = (dnx_data_l3.fec.max_fec_id_for_single_dhb_cluster_pair_granularity_get(unit) /
                                        dnx_data_l3.fec.bank_size_get(unit)) - 1;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(received_allocation, 0, DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS);
/*
 * Go over all the FEC clusters and get the banks of each hierarchy according to the given address of each cluster
 */
    for (fec_hier_iter = 0; fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.db.
                        number_of_clusters.get(unit, mdb_fec_hierarchy_index_to_fec_table_map[fec_hier_iter],
                                               &nof_clusters));

        nof_banks[fec_hier_iter] = (nof_clusters >> 1);

        for (cluster_iter = 0; cluster_iter < nof_banks[fec_hier_iter]; cluster_iter++)
        {
            uint32 bank_iter, bank_count;

            SHR_IF_ERR_EXIT(mdb_db_infos.db.
                            clusters_info.get(unit, mdb_fec_hierarchy_index_to_fec_table_map[fec_hier_iter],
                                              cluster_iter << 1, &cluster));

            bank = cluster.start_address / dnx_data_mdb.dh.macro_type_info_get(unit, MDB_MACRO_B)->nof_rows;

            first_bank[fec_hier_iter] = UTILEX_MIN(first_bank[fec_hier_iter], bank);

            bank_count = MDB_MACRO_NOF_FEC_BANKS_GET(cluster.macro_type);

            for (bank_iter = 0; bank_iter < bank_count; bank_iter++)
            {
                received_allocation[bank + bank_iter] = fec_hier_iter + 1;

                macro_global_index[bank + bank_iter] =
                    MDB_DH_GET_GLOBAL_MACRO_INDEX(unit, cluster.macro_type, cluster.macro_index);

            }

        }
    }
    /*
     * Verify that the expected allocation is the same as the actual allocation
     */
    if (sal_memcmp(expected_allocation, received_allocation, DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Received allocation doesn't match the expected one.");
    }
    /*
     * Verify that the all the allocation banks are allocated consecutively
     */
    for (fec_hier_iter = 0; fec_hier_iter < dnx_data_mdb.direct.nof_fec_ecmp_hierarchies_get(unit); fec_hier_iter++)
    {
        uint32 start_offset = 0;

        sal_memset(checked, 0, DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS);

        /*
         * There is a single case where it is expected that a MACRO won't be consecutive.
         */
        if ((first_bank[fec_hier_iter] + nof_banks[fec_hier_iter] <= high_granularity_max_bank)
            && (nof_banks[fec_hier_iter] & 0x1) && (first_bank[fec_hier_iter] & 0x1))
        {
            start_offset++;

            if (macro_global_index[first_bank[fec_hier_iter]] !=
                macro_global_index[first_bank[fec_hier_iter] + nof_banks[fec_hier_iter] - 1])
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "It was expected the the last and first bank will be of the same macro (hierarchy %d).",
                             fec_hier_iter + 1);
            }
        }

        for (bank_iter = first_bank[fec_hier_iter] + start_offset;
             bank_iter < first_bank[fec_hier_iter] + nof_banks[fec_hier_iter]; bank_iter++)
        {
            if (!checked[bank_iter])
            {
                checked[bank_iter] = TRUE;
            }
            else
            {
                if (macro_global_index[bank_iter] != macro_global_index[bank_iter - 1])
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "MACROs are not consistent as expected (hierarchy %d).",
                                 fec_hier_iter + 1);
                }
            }
        }
    }
    /*
     * Check that no two macro Bs of different hierarchies are adjacent for the relevant case.
     */
    if (dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fer_fec_granularity_double_size))
    {
        for (bank_iter = 0; bank_iter < DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS >> 1; bank_iter++)
        {
            if ((received_allocation[2 * bank_iter] != 0) && (received_allocation[2 * bank_iter + 1] != 0)
                && (received_allocation[2 * bank_iter] != received_allocation[2 * bank_iter + 1]))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "This device can't support two adjacent macro B banks of two different hierarchies.");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Main fec allocation testing command
 */
static shr_error_e
sh_dnx_l3_fec_internal_allocation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 macro_allocation[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES][DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES];
    uint8 expected_allocation[DNX_DATA_MAX_L3_FEC_MAX_NOF_BANKS];
    uint32 first_bank_after_ecmp = dnx_data_l3.fec.first_bank_without_id_alloc_get(unit);
    int hierarchy_start_positions[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * This is the basic case using only MACRO As.
     */
    sal_memset(macro_allocation, 0,
               DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES * DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES);

    /** 3 MACRO A clusters pairs for hierarchy 1 */
    macro_allocation[0][0] = 3;
    /** 2 MACRO A clusters pairs for hierarchy 2 */
    macro_allocation[1][1] = 2;
    /** 3 MACRO A clusters pairs for hierarchy 3 */
    macro_allocation[2][2] = 3;
    /** Hierarchy 1 is expected to fill the first banks after the ECMP range*/
    hierarchy_start_positions[0] = first_bank_after_ecmp;
    /** Hierarchy 2 is expected to fill the banks at the end */
    hierarchy_start_positions[1] = 26;
    /** Hierarchy 3 is expected to fill the banks just before hierarchy 2*/
    hierarchy_start_positions[2] = 20;

    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_update_sw_state_clusters_and_expected_res
                    (unit, macro_allocation, hierarchy_start_positions, expected_allocation));
    SHR_IF_ERR_EXIT(mdb_parse_xml_update_fec_address(unit));
    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_verify_allocation(unit, expected_allocation));

    /*
     * Prevent hierarchy 2 to be in the low granularity range.
     */
    sal_memset(macro_allocation, 0,
               DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES * DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES);

    /** 3 MACRO A clusters pairs for hierarchy 1 */
    macro_allocation[0][0] = 3;
    /** 3 MACRO B clusters pairs for hierarchy 2 from three different MACROS (not allowed in the low granularity range)*/
    macro_allocation[1][13] = 1;
    macro_allocation[1][14] = 1;
    macro_allocation[1][15] = 1;
    /** 3 MACRO A clusters pairs for hierarchy 3 */
    macro_allocation[2][2] = 3;
    /** Hierarchy 1 is expected to fill the first banks after the ECMP range*/
    hierarchy_start_positions[0] = first_bank_after_ecmp;
    /** Hierarchy 2 is expected to fill the banks that are following hierarchy 1 */
    hierarchy_start_positions[1] = first_bank_after_ecmp + 6;
    /** Hierarchy 3 is expected to fill the banks at the end*/
    hierarchy_start_positions[2] = 24;

    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_update_sw_state_clusters_and_expected_res
                    (unit, macro_allocation, hierarchy_start_positions, expected_allocation));

    SHR_IF_ERR_EXIT(mdb_parse_xml_update_fec_address(unit));

    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_verify_allocation(unit, expected_allocation));

    /*
     * Push hierarchy 1 into the ECMP range.
     */
    sal_memset(macro_allocation, 0,
               DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES * DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES);

    /** Allocate 14 banks for hierarchy 1 */
    macro_allocation[0][0] = 4;
    macro_allocation[0][1] = 2;
    macro_allocation[0][13] = 2;
    /** Allocate 5 clusters pairs on 4 different MACRO Bs to prevent hierarchy 2 to be in the low range*/
    macro_allocation[1][14] = 1;
    macro_allocation[1][15] = 1;
    macro_allocation[1][16] = 1;
    macro_allocation[1][17] = 2;
    /** 3 MACRO A clusters pairs for hierarchy 3 */
    macro_allocation[2][2] = 3;
    /** Hierarchy 1 is expected to be at the highest bank which allow hierarchy 2 to be as well in the high range*/
    hierarchy_start_positions[0] =
        dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fer_fec_granularity_double_size) ? 0 : 1;
    /** Hierarchy 2 is expected to fill the banks that are following hierarchy 1 */
    hierarchy_start_positions[1] =
        dnx_data_l3.feature.feature_get(unit, dnx_data_l3_feature_fer_fec_granularity_double_size) ? 14 : 15;
    /** Hierarchy 3 is expected to fill the banks at the end*/
    hierarchy_start_positions[2] = 24;

    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_update_sw_state_clusters_and_expected_res
                    (unit, macro_allocation, hierarchy_start_positions, expected_allocation));

    SHR_IF_ERR_EXIT(mdb_parse_xml_update_fec_address(unit));

    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_verify_allocation(unit, expected_allocation));

    /*
     * illegal allocation, all hierarchy will be in the high granularity range with enough room.
     */
    sal_memset(macro_allocation, 0,
               DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES * DNX_DATA_MAX_MDB_DH_TOTAL_NOF_MACROES);

    /** 14 banks for hierarchy 1 */
    macro_allocation[0][0] = 4;
    macro_allocation[0][1] = 2;
    macro_allocation[0][13] = 2;
    /** 7 banks on the high granularity range for hierarchy 2*/
    macro_allocation[1][14] = 1;
    macro_allocation[1][15] = 1;
    macro_allocation[1][16] = 1;
    macro_allocation[1][17] = 4;
    /** 7 banks on the high granularity range for hierarchy 3*/
    macro_allocation[1][14] = 1;
    macro_allocation[1][15] = 1;
    macro_allocation[1][16] = 1;
    macro_allocation[1][17] = 4;

    SHR_IF_ERR_EXIT(ctest_dnx_l3_fec_update_sw_state_clusters_and_expected_res(unit, macro_allocation, NULL, NULL));
    /*
     * 28 bank in total on the high granularity range out of 20, expected to fail
     */
    SHR_IF_NOT_EXCEPT_ERR_EXIT(mdb_parse_xml_update_fec_address(unit), _SHR_E_INTERNAL);

    LOG_INFO(BSL_LS_BCM_L3, (BSL_META_U(unit, "FEC internal allocation test end. PASS\n")));

exit:

    SHR_FUNC_EXIT;
}
/* *INDENT-OFF* */
/**
 * \brief DNX L3 fec Tests
 * List of fec ctests modules.
 */
sh_sand_cmd_t dnx_l3_fec_test_cmds[] = {
    {"allocation", sh_dnx_l3_fec_allocation_cmd, NULL, NULL, &sh_dnx_l3_fec_allocation_man, NULL, NULL, CTEST_POSTCOMMIT | CTEST_UM},
    {"internal_allocation", sh_dnx_l3_fec_internal_allocation_cmd, NULL, NULL, &sh_dnx_l3_fec_internal_allocation_man, NULL, NULL, SH_CMD_ROLLBACK | CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE

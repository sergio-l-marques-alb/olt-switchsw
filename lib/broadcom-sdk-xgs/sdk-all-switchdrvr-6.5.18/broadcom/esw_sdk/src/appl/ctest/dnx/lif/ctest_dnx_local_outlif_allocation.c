/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/*****************
 * INCLUDE FILES *
 * ***************
 */
/*
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/types.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <shared/bslnames.h>
#include <appl/diag/bslenable.h>
#include <appl/diag/dnx/diag_dnx_lif.h>

/**
 * }
 */

/*************
 * DEFINES   *
 *************/
/*
 * {
 */

#define CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE 65536      /* (DNX_DATA_MAX_LIF_OUT_LIF_NOF_LOCAL_OUT_LIFS /
                                                                 * DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT) */

/*
 * Number of options for linked list existence: 0: not exists, 1: exists.
 */
#define SEMANTIC_TEST_NOF_LL_OPTIONS     2

/*
 * Number of options for outlif entry size:
 * 0: 30b, 1: 60b, 2: 120b
 */
#define SEMANTIC_TEST_NOF_SIZE_OPTIONS    3

/*
 * Total number of outlifs used in semantic test.
 */
#define CTEST_DNX_LOCAL_OUTLIF_SEMANTIC_TEST_NOF_OUTLIFS    (DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT \
                                            * SEMANTIC_TEST_NOF_LL_OPTIONS * SEMANTIC_TEST_NOF_SIZE_OPTIONS)

/*
 * Num of lifs to be cleared per phase in capacity test
 */
#define CTEST_DNX_LOCAL_OUTLIF_CAPACITY_PHASE_TEST_NUM_CLEAR_LIFS   10
/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */
/*
 * }
 */

extern shr_error_e dbal_print_table_names(
    int unit,
    char *table_name);

/**
 * \brief For each logical phase, we first allocate this number of entries to match JR1, and only then we allocate
 * extra entries.
 * These numbers fit the logical phases in \ref outlif_logical_phase_to_outlif_phase_enum
 */
int outlif_logical_phase_to_nof_jr1_outlifs[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = {
    0x4000,
    0x4000,
    0x4000,
    0x2000,
    0x4000,
    0x2000,
    0x6000,
    0x4000
};

/**
 * \brief These arrays hold the lifs allocated in the test.
 */
int stress_test_allocated_local_lifs[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT]
    [CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE] = { {0} };
int stress_test_allocated_global_lifs[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT]
    [CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE] = { {0} };
int stress_test_nof_allocated_outlifs_per_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT];

lif_mngr_local_outlif_info_t stress_test_local_outlif_info_per_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = {
    /*
     * dbal_table_id dbal_result_type outlif_phase. 
     */
    {DBAL_TABLE_EEDB_RIF_BASIC, DBAL_RESULT_TYPE_EEDB_RIF_BASIC_ETPS_ETH_RIF, LIF_MNGR_OUTLIF_PHASE_RIF},
    {DBAL_TABLE_EEDB_ARP, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC, LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP},
    {DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2,
     LIF_MNGR_OUTLIF_PHASE_VPLS_1},
    {DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_2,
     LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL, DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL,
     LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL, DBAL_RESULT_TYPE_EEDB_IPV4_TUNNEL_ETPS_IPV4_TUNNEL,
     LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4},
    {DBAL_TABLE_EEDB_ARP, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP, LIF_MNGR_OUTLIF_PHASE_ARP},
    {DBAL_TABLE_EEDB_OUT_AC, DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_QOS_OAM, LIF_MNGR_OUTLIF_PHASE_AC}
};

char *phase_names[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = {
    "RIF",
    "ARP PLUS AC",
    "MPLS TUNNEL 1",
    "MPLS TUNNEL 2",
    "IPV4 TUNNEL 1",
    "IPV4 TUNNEL 2",
    "ARP",
    "AC"
};

lif_mngr_local_outlif_info_t semantic_test_local_outlif_info_per_ll_and_size
    [SEMANTIC_TEST_NOF_LL_OPTIONS][SEMANTIC_TEST_NOF_SIZE_OPTIONS] = {
    {
     /*
      * No linked list 
      */
     {DBAL_TABLE_EEDB_RIF_BASIC, DBAL_RESULT_TYPE_EEDB_RIF_BASIC_ETPS_ETH_RIF},
     {DBAL_TABLE_EEDB_RIF_BASIC, DBAL_RESULT_TYPE_EEDB_RIF_BASIC_ETPS_ETH_RIF_STAT},
     {DBAL_TABLE_EEDB_ARP, DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC}
     },
    {
     /*
      * Use linked list 
      */
     {DBAL_TABLE_EEDB_ERSPAN, DBAL_RESULT_TYPE_EEDB_ERSPAN_ETPS_ERSPANV2},
     {DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_PHP_STAT},
     {DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_RESULT_TYPE_EEDB_MPLS_TUNNEL_ETPS_MPLS_1_STAT}
     }
};

/**
 * \brief
 * Man for eedb stress test.
 */
sh_sand_man_t dnx_outlif_stress_test_man = {
    "Stress test for local outlif allocation.",
    "Attempt to allocate all local outlifs, then free them, then allocate again with a different phase."
};

sh_sand_man_t dnx_outlif_semantic_test_man = {
    "Semantic test for local outlif allocation.",
    "A standard allocate-get-delete-repeat test for local outlif."
};

sh_sand_man_t dnx_outlif_allocation_replace_semantic_test_man = {
    "Semantic test for local outlif allocation replace.",
    "Create an outlif, then replace it in several different ways."
};

sh_sand_man_t dnx_outlif_mdb_phase_map_test_man = {
    "Semantic test for mdb phase mapping.",
    "Creates and removes outlifs, compares the phase map with init values."
};

sh_sand_man_t dnx_outlif_phase_capacity_test_man = {
    "Stress test that checks capacity per logical phase.",
    "Creates as much lifs as possible per phase, compares it with an expected value, and cleans as many lifs as requested from each phase. Expected size is calculated based on the requested lif type width, granularity and outlif pointer size"
};

sh_sand_man_t dnx_outlif_negative_ll_test_man = {
    "Negative test, checks allocation of lif with ll in full capacity mode.",
    "In case of max outlifs usage (20 bits outlif pointer for jr2, 19 for q2a), try to allocate a lif with ll with #lif with msb on. Expect failure"
};

sh_sand_option_t ctest_dnx_local_outlif_allocation_semantic_test_options[] = {
    {NULL}
};

sh_sand_man_t dnx_outlif_disable_mdb_clusters_allocation = {
    "Disable outlif allcoation in mdb clusters.",
    "Disable allocation of outlifs in mdb clusters. Set disable=yes before running a regular test using outlifs, "
        "and disable=no after running the test."
};

sh_sand_option_t ctest_dnx_local_outlif_disable_mdb_options[] = {
    /*
     * Option name Option Type Option Description Option Default
     */
    {"disable", SAL_FIELD_TYPE_BOOL, "Disable/enable outlif mdb cluster allocation.", ""}
    ,
    {NULL}
};

sh_sand_option_t ctest_dnx_local_outlif_disable_eedb_data_banks_options[] = {
    /*
     * Option name Option Type Option Description Option Default
     */
    {"disable", SAL_FIELD_TYPE_BOOL, "Disable/enable outlif eedb data banks allocation.", ""}
    ,
    {NULL}
};

sh_sand_option_t ctest_dnx_local_outlif_phase_capacity_test_options[] = {
    /*
     * Option name Option Type Option Description Option Default
     */
    {"dbal_table", SAL_FIELD_TYPE_STR, "Dbal table (to use) name", "EEDB_ARP"}
    ,
    {"dbal_result_type", SAL_FIELD_TYPE_STR, "Dbal table result type(to use) name", "ETPS_ARP"}
    ,
    {"clear_lifs", SAL_FIELD_TYPE_INT32,
     "Number of lifs to clear (per logical phase) before exiting (Note - clearing X 2 lifs - with and without linked list)",
     "10"}
    ,
    {"clear_all_lifs", SAL_FIELD_TYPE_BOOL,
     "Clear all lifs before the test, clear all test allocated lifs during test cleanup", "FALSE"}
    ,
    {"compare_expected", SAL_FIELD_TYPE_BOOL,
     "Compare final allocation with expected result, fail if doesn't meet", "FALSE"}
    ,
    {NULL}
};

sh_sand_option_t ctest_dnx_local_outlif_negative_ll_test_options[] = {
    /*
     * Option name Option Type Option Description Option Default
     */
    {NULL}
};
/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/**
 * \brief
 * Free all allocated outlifs.
 */
static shr_error_e
ctest_dnx_local_outlif_free_all_lifs(
    int unit)
{
    int current_lif_index, current_logical_phase;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free outlifs until reaching an empty one.
     */
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        current_lif_index = 0;
        while (stress_test_allocated_local_lifs[current_logical_phase][current_lif_index] != 0)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_free
                            (unit, stress_test_allocated_global_lifs[current_logical_phase][current_lif_index], NULL,
                             stress_test_allocated_local_lifs[current_logical_phase][current_lif_index]));
            stress_test_allocated_global_lifs[current_logical_phase][current_lif_index] = 0;
            stress_test_allocated_local_lifs[current_logical_phase][current_lif_index] = 0;
            current_lif_index++;
            if (current_lif_index % 4192 == 0)
            {
                cli_out(".");
            }
        }

        stress_test_nof_allocated_outlifs_per_phase[current_logical_phase] = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given a logical phase, its partner phase and the array of number of lifs per stage, allocates the number of 
 *  lifs for this stage, and reduces the number of lifs that were allocated from the number of available lifs.
 */
static shr_error_e
ctest_dnx_local_outlif_allocate_all_lifs_for_stage(
    int unit,
    dnx_algo_local_outlif_logical_phase_e logical_phase,
    int target_nof_outlifs)
{
    int outlif_array_start_index;
    int current_lif_index, global_lif;
    lif_mngr_local_outlif_info_t *local_outlif_info;
    shr_error_e rv;
    int nof_outlifs;
    SHR_FUNC_INIT_VARS(unit);

    local_outlif_info = &stress_test_local_outlif_info_per_phase[logical_phase];

    /*
     * Allocate the given number of outlifs, or if input was 0, set an arbitrarily high number.
     */
    nof_outlifs = (target_nof_outlifs) ? target_nof_outlifs : CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE;
    outlif_array_start_index = stress_test_nof_allocated_outlifs_per_phase[logical_phase];

    for (current_lif_index = 0; current_lif_index < nof_outlifs; current_lif_index++)
    {
        local_outlif_info->local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;
        rv = (dnx_lif_lib_allocate(unit, 0, &global_lif, NULL, local_outlif_info));

        if (rv == _SHR_E_RESOURCE)
        {
            SHR_SET_CURRENT_ERR(rv);
            break;
        }
        SHR_IF_ERR_EXIT(rv);

        if (current_lif_index % 4192 == 0)
        {
            cli_out(".");
        }

        stress_test_allocated_local_lifs[logical_phase][outlif_array_start_index + current_lif_index] =
            local_outlif_info->local_outlif;
        stress_test_allocated_global_lifs[logical_phase][outlif_array_start_index + current_lif_index] = global_lif;
    }

    stress_test_nof_allocated_outlifs_per_phase[logical_phase] += current_lif_index;

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
ctest_dnx_local_outlif_print_stats(
    int unit,
    int fail_if_external_memories_are_unused)
{
    dnx_algo_local_outlif_stats_t outlif_stats;
    dnx_algo_local_outlif_logical_phase_e current_logical_phase;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * For every logical phase, print how many entries it has allocated.
     */
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        cli_out("Phase %d: %s: 0x%x outlifs were allocated.\n", current_logical_phase + 1,
                phase_names[current_logical_phase], stress_test_nof_allocated_outlifs_per_phase[current_logical_phase]);
    }

    /*
     * Get and print outlif stats.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_stats_get(unit, &outlif_stats));

    cli_out("Outlif stats:\n"
            "-------------\n"
            "%d out of %d outlif banks are used in external memory.\n"
            "%d out of %d eedb banks are in use.\n"
            "0x%x out of 0x%x GLEM entries are in use.\n",
            outlif_stats.nof_used_outlif_banks_in_external_memory, outlif_stats.nof_outlif_banks_in_external_memory,
            outlif_stats.nof_used_eedb_banks, outlif_stats.nof_eedb_banks,
            outlif_stats.nof_used_glem_entries, outlif_stats.nof_glem_entries);

    if (fail_if_external_memories_are_unused
        && outlif_stats.nof_used_outlif_banks_in_external_memory < outlif_stats.nof_outlif_banks_in_external_memory)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The allocation sequence should be optimized to use all external memories.\n"
                     "Not all were used.");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Test if the device supports the outlif stress test.
 */
shr_error_e
ctest_dnx_local_outlif_stress_test_support_check(
    int unit,
    rhlist_t * invoke_list)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(sh_cmd_is_device(unit, invoke_list), "");

    /**
     * if init is not done, return that command is available.
     * this is done to prevent access to DNX-Data before it is init
     */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }
    /*
     * If the eedb traffic lock workaround is in effect, then the test becomes completely irrelevant
     * because the eedb is being used to full inefficient capacity from the start.
     */
    if (dnx_data_mdb.hw_bug.feature_get(unit, dnx_data_mdb_hw_bug_eedb_bank_traffic_lock))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A stress test for outlif allocation. Test sequence:
 *    0. Read how many outlifs are available for each logical phase.
 *    1. Allocate this number of outlifs per phase.
 *       - If we get an out of resource error before reaching this number, then print the actual number that
 *          allocatd. Since this is mostly a diagnostic test, we don't fail the test in this case.
 *    2. Free all the outlifs were allocated.
 *    3. Repeat step 1, but reverse the order of phases. In this way, the phase pairs will be allocating the other phase first.
 *    4. Free all newly allocated lifs.
 */
shr_error_e
ctest_dnx_local_outlif_allocation_stress_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_algo_local_outlif_logical_phase_e current_logical_phase;
    shr_error_e rv;
    int allocate_all, allocation_limit, jr1_compatible;
    uint8 mdb_profile_u8;
    char *mdb_profile_name;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Some MDB profiles have less EEDB entries than JR1, so don't expect them to reach
     * the JR1 number of outlifs.
     */
    SHR_IF_ERR_EXIT(mdb_db_infos.mdb_profile.get(unit, &mdb_profile_u8));
    mdb_profile_name = dnx_data_mdb.pdbs.mdb_profiles_info_get(unit, mdb_profile_u8)->name;
    if ((sal_strncmp("balanced", mdb_profile_name, sal_strlen(mdb_profile_name)) == 0) ||
        (sal_strncmp("balanced-exem", mdb_profile_name, sal_strlen(mdb_profile_name)) == 0))
    {
        jr1_compatible = FALSE;
    }
    else
    {
        jr1_compatible = TRUE;
    }

    /*
     * Add the number of rifs to the number of allocated GLEM entries, since the API would allocate them.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count
                    (unit, DNX_ALGO_LIF_EGRESS, dnx_data_l3.rif.nof_rifs_get(unit)));
    stress_test_nof_allocated_outlifs_per_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1] =
        dnx_data_l3.rif.nof_rifs_get(unit);

    cli_out("Step one: allocate JR1 number of lifs.\n" "--------------------------------------\n\n");
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        rv = ctest_dnx_local_outlif_allocate_all_lifs_for_stage(unit, current_logical_phase,
                                                                outlif_logical_phase_to_nof_jr1_outlifs
                                                                [current_logical_phase]);

        if (rv == _SHR_E_RESOURCE)
        {
            cli_out("\nError: only 0x%x outlifs were allocated on phase %d, out of 0x%x required outlifs.\n",
                    stress_test_nof_allocated_outlifs_per_phase[current_logical_phase],
                    current_logical_phase + 1, outlif_logical_phase_to_nof_jr1_outlifs[current_logical_phase]);
            if (jr1_compatible)
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    /*
     * Now go over every phase, and allocate every available lif. First on the MDB clusters,
     * then until it's impossible to allocate.
     */
    cli_out("\n" "Step two: allocate every available lif.\n" "--------------------------------------\n\n");
    for (allocate_all = 0; allocate_all < 2; allocate_all++)
    {
        for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2;
             current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
        {
            if (allocate_all)
            {
                allocation_limit = 0;
            }
            else
            {
                /*
                 * Get the number of potential MDB outlifs on this phase.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_get_potential_nof_mdb_outlifs_for_entry(unit,
                                                                                              &stress_test_local_outlif_info_per_phase
                                                                                              [current_logical_phase],
                                                                                              &allocation_limit));
            }

            rv = ctest_dnx_local_outlif_allocate_all_lifs_for_stage(unit, current_logical_phase, allocation_limit);

            if (rv == _SHR_E_RESOURCE)
            {
                /*
                 * This is fine, we don't actually care how many outlifs each phase has.
                 */
                continue;
            }
            else if (rv != _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }

    /*
     * Print stats.
     * Fail if the external memories are not full because this sequence should be optimised.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_print_stats(unit, TRUE));

    cli_out("\n" "Step three: free every allocated lif.\n" "-------------------------------------\n\n");
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_free_all_lifs(unit));

    cli_out("\n"
            "Step four: Reallocate all JR1 lifs, on the partner phases first.\n"
            "----------------------------------------------------------------\n\n");
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT - 1;
         current_logical_phase > DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1; current_logical_phase--)
    {
        rv = ctest_dnx_local_outlif_allocate_all_lifs_for_stage(unit, current_logical_phase,
                                                                outlif_logical_phase_to_nof_jr1_outlifs
                                                                [current_logical_phase]);

        if (rv == _SHR_E_RESOURCE)
        {
            cli_out("\nError: only 0x%x outlifs were allocated on phase %d, out of 0x%x required outlifs.\n",
                    stress_test_nof_allocated_outlifs_per_phase[current_logical_phase],
                    current_logical_phase + 1, outlif_logical_phase_to_nof_jr1_outlifs[current_logical_phase]);
            if (jr1_compatible)
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        else if (rv != _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    /*
     * Now go over every phase, and allocate every available lif. First on the MDB clusters,
     * then until it's impossible to allocate.
     */
    cli_out("\n"
            "Step five: allocate every available lif, on the partner phases first.\n"
            "---------------------------------------------------------------------\n\n");
    for (allocate_all = 0; allocate_all < 2; allocate_all++)
    {
        for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT - 1;
             current_logical_phase > DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1; current_logical_phase--)
        {
            if (allocate_all)
            {
                allocation_limit = 0;
            }
            else
            {
                /*
                 * Get the number of potential MDB outlifs on this phase.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_local_outlif_get_potential_nof_mdb_outlifs_for_entry(unit,
                                                                                              &stress_test_local_outlif_info_per_phase
                                                                                              [current_logical_phase],
                                                                                              &allocation_limit));
            }

            rv = ctest_dnx_local_outlif_allocate_all_lifs_for_stage(unit, current_logical_phase, allocation_limit);

            if (rv == _SHR_E_RESOURCE)
            {
                /*
                 * This is fine, we don't actually care how many outlifs each phase has.
                 */
                continue;
            }
            else if (rv != _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
    }

    /*
     * Print stats.
     * Don't fail if external memories are not full because the reverse sequence is less optimized.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_print_stats(unit, FALSE));

    cli_out("\n" "Step six: free all lifs.\n" "-------------------------\n\n");
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_free_all_lifs(unit));

    cli_out("\n" "--------------------\n" "| Test successful! |\n" "--------------------\n\n");
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_local_outlif_disable_mdb_clusters(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int value;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("disable", value);

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_mdb_clusters_disable_set(unit, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * Test steps:
 * This test is used to validate a behavior where all of the lif's data comes from the clusters, and the eedb banks are used only for link list.
 * It is relevant only for profiles with more than 512k entries, and for 20 bits (at least) local outlif width
 *
 * The test steps are:
 * 1. Set mdb profile to ext-kbp (profile which is configured with enough clusters)
 * 2. Disable eedb data banks.
 * 3. Allocate (with ID) 2 lifs with 512k difference between them
 * 4. Validate the allocation (dbal get entry).
 */
shr_error_e
ctest_dnx_local_outlif_disable_eedb_data_banks(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int value, phase, global_lif, use_ll = TRUE;
    int entry_size_index = 1; /**60b*/
    lif_mngr_local_outlif_info_t *local_outlif_info;
    int local_outlif_1 = 159744;
    int local_outlif_2 = 671744;
    lif_mngr_outlif_phase_e outlif_phase;
    ctest_soc_property_t ctest_soc_property[] = { {"mdb_profile.BCM8869X", "ext-kbp"}, {NULL} };
    rhhandle_t ctest_soc_set_h = NULL;
    dbal_tables_e dbal_table_id;
    SHR_FUNC_INIT_VARS(unit);

    /**Test is only relevant for profiles which support 20 bits outlif pointer size*/
    if (dnx_data_lif.out_lif.outlif_pointer_size_get(unit) != 20)
    {
        SHR_EXIT();
    }

    SH_SAND_GET_BOOL("disable", value);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_eedb_data_banks_disable_set(unit, value));

    phase = LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP;
    local_outlif_info = &(semantic_test_local_outlif_info_per_ll_and_size[use_ll][entry_size_index]);
    local_outlif_info->outlif_phase = phase;
    local_outlif_info->local_lif_flags =
        DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID;
    local_outlif_info->local_outlif = local_outlif_1;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, &global_lif, NULL, local_outlif_info));

    local_outlif_info->outlif_phase = LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3;
    local_outlif_info->local_outlif = local_outlif_2;
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, &global_lif, NULL, local_outlif_info));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, local_outlif_1, &dbal_table_id, NULL, &outlif_phase, NULL, NULL));
    if (outlif_phase != LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Received outlif phase %d, expected outlif phase %d for local outlif %d",
                     outlif_phase, LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP, local_outlif_1);
    }

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                    (unit, local_outlif_2, &dbal_table_id, NULL, &outlif_phase, NULL, NULL));
    if (outlif_phase != LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Received outlif phase %d, expected outlif phase %d for local outlif %d",
                     outlif_phase, LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3, local_outlif_2);
    }

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/**
 * Test steps:
 * The local outlif test is mainly used to verify that outlifs on different phases are assigned different
 * banks on the eedb, and that there is at least one bank for each phase no matter what's the allocation order
 * (in a regular use, not including stress cases).
 * In addition, the test is checking the interactions of banks with/without linked list.
 *
 * The test steps are:
 * 1. Allocate one outlif per phase, with/without linked list, entry size 30/60/120, random order.
 * 2. Bank verification
 * 2a. Verify that all outlifs that have different ll/phase are in different banks.
 * 2b. Verify that all outlifs that have the same ll/ phase are in the same bank.
 * 3. Turn all outlifs into tunnels, and use gport_get to verify that their allocation information is identical.
 * 4. Delete all outlifs.
 * 5. Repeat.
 */

shr_error_e
ctest_dnx_local_outlif_allocation_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int global_lif_array[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT]
        [SEMANTIC_TEST_NOF_LL_OPTIONS][SEMANTIC_TEST_NOF_SIZE_OPTIONS];
    int local_lif_array[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT]
        [SEMANTIC_TEST_NOF_LL_OPTIONS][SEMANTIC_TEST_NOF_SIZE_OPTIONS];

    int current_phase, use_ll, entry_size_index;
    int current_outlif, current_iteration;
    int rand_seed;
    SHR_BITDCL *used_banks = NULL;
    int prev_lif, cur_lif;
    int rif_max_value = dnx_data_l3.rif.nof_rifs_get(unit) - 1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(used_banks, SHR_BITALLOCSIZE(DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit)), "used_banks",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SH_SAND_GET_UINT32("seed", rand_seed);

    if (rand_seed == -1)
    {
        rand_seed = sal_time_usecs();
    }

    cli_out("Setting random seed: %u\n", rand_seed);
    sal_srand(rand_seed);

    for (current_iteration = 0; current_iteration < 2; current_iteration++)
    {
        lif_mngr_local_outlif_info_t *local_outlif_info;
        int *global_lif;
        int current_outlif_bank, next_outlif_bank, current_global_lif, current_local_lif, gport;
        dnx_algo_gpm_gport_hw_resources_t gport_hw_res;

        sal_memset(global_lif_array, 0, sizeof(global_lif_array));
        sal_memset(local_lif_array, 0, sizeof(local_lif_array));

        /**
         * 1. Allocate one outlif per phase, with/without linked list, entry size 30/60/120, random order.
         */
        for (current_outlif = 0; current_outlif < CTEST_DNX_LOCAL_OUTLIF_SEMANTIC_TEST_NOF_OUTLIFS; current_outlif++)
        {
            do
            {
                current_phase = sal_rand() % DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT;
                use_ll = sal_rand() % SEMANTIC_TEST_NOF_LL_OPTIONS;
                entry_size_index = sal_rand() % SEMANTIC_TEST_NOF_SIZE_OPTIONS;
            }
            while (global_lif_array[current_phase][use_ll][entry_size_index] != 0);

            local_outlif_info = &(semantic_test_local_outlif_info_per_ll_and_size[use_ll][entry_size_index]);
            local_outlif_info->outlif_phase = stress_test_local_outlif_info_per_phase[current_phase].outlif_phase;
            local_outlif_info->local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;
            local_outlif_info->local_outlif = 0;

            global_lif = &global_lif_array[current_phase][use_ll][entry_size_index];
            SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, global_lif, NULL, local_outlif_info));

            local_lif_array[current_phase][use_ll][entry_size_index] = local_outlif_info->local_outlif;
        }

        /*
         * 2. Bank verification.
         */
        SHR_BITCLR_RANGE(used_banks, 0, DNX_ALGO_LOCAL_OUTLIF_NOF_BANKS(unit));
        for (current_phase = 0; current_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_phase++)
        {
            for (use_ll = 0; use_ll < SEMANTIC_TEST_NOF_LL_OPTIONS; use_ll++)
            {
                /*
                 * 2a. Verify that all outlifs that have different ll/phase are in different banks.
                 * Keep a bitmap of the already used banks, and mark each new one.
                 */
                current_outlif_bank =
                    local_lif_array[current_phase][use_ll][0] / dnx_data_lif.out_lif.allocation_bank_size_get(unit);
                if (!(dnx_data_mdb.hw_bug.feature_get(unit, dnx_data_mdb_hw_bug_eedb_bank_traffic_lock) && use_ll))
                {
                    /*
                     * If we have the eedb bank traffic lock workaround, then the outlifs with and without linked list will
                     * be in the same bank. Don't test the second linked list option.
                     */
                    if (SHR_BITGET(used_banks, current_outlif_bank))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "Two outlifs which were not supposed to be in the same bank are in the same bank");
                    }
                }
                SHR_BITSET(used_banks, current_outlif_bank);

                prev_lif = local_lif_array[current_phase][use_ll][0];
                /*
                 * 2b. Verify that all outlifs that have the same ll/ phase are in the same bank.
                 */
                for (entry_size_index = 0; entry_size_index < SEMANTIC_TEST_NOF_SIZE_OPTIONS - 1; entry_size_index++)
                {
                    cur_lif = local_lif_array[current_phase][use_ll][entry_size_index + 1];
                    next_outlif_bank = cur_lif / dnx_data_lif.out_lif.allocation_bank_size_get(unit);
                    if (((cur_lif < rif_max_value) && (prev_lif > rif_max_value)) ||
                        ((cur_lif > rif_max_value) && (prev_lif < rif_max_value)))
                    {
                        if (current_outlif_bank == next_outlif_bank)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "rif and lif can't exist in the same outlif bank\n"
                                         "Phase is: %d, use_ll: %s, entry_size_indexes: %d,%d and banks are: %d, %d\n",
                                         current_phase + 1, use_ll ? "TRUE" : "FALSE", entry_size_index,
                                         entry_size_index + 1, current_outlif_bank, next_outlif_bank);
                        }
                    }
                    else if (current_outlif_bank != next_outlif_bank)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "Two outlifs which were supposed to be in the same bank are not in the same bank\n"
                                     "Phase is: %d, use_ll: %s, entry_size_indexes: %d,%d and banks are: %d, %d\n",
                                     current_phase + 1, use_ll ? "TRUE" : "FALSE", entry_size_index,
                                     entry_size_index + 1, current_outlif_bank, next_outlif_bank);
                    }
                    current_outlif_bank = next_outlif_bank;
                    prev_lif = local_lif_array[current_phase][use_ll][entry_size_index + 1];
                }
            }
        }

        /*
         * Get and delete.
         */
        sal_memset(&gport_hw_res, 0, sizeof(gport_hw_res));
        for (current_phase = 0; current_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_phase++)
        {
            for (use_ll = 0; use_ll < SEMANTIC_TEST_NOF_LL_OPTIONS; use_ll++)
            {
                for (entry_size_index = 0; entry_size_index < SEMANTIC_TEST_NOF_SIZE_OPTIONS; entry_size_index++)
                {
                    /*
                     * 3. Turn all outlifs into tunnels, and use gport_get to verify that their allocation information is identical.
                     */
                    current_global_lif = global_lif_array[current_phase][use_ll][entry_size_index];
                    current_local_lif = local_lif_array[current_phase][use_ll][entry_size_index];
                    BCM_L3_ITF_SET(gport, BCM_L3_ITF_TYPE_LIF, current_global_lif);
                    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, gport);

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_res));

                    /*
                     * Verify that the hw resources match the lif info.
                     * Don't check outlif phase because returned phase depends on dbal table and to meet the size criteria
                     * we use dbal tables that don't normally exist.
                     */
                    SHR_VAL_VERIFY(gport_hw_res.local_out_lif, local_lif_array[current_phase][use_ll][entry_size_index],
                                   _SHR_E_FAIL, "Mismatching local outlif\n");
                    SHR_VAL_VERIFY(gport_hw_res.outlif_dbal_table_id,
                                   semantic_test_local_outlif_info_per_ll_and_size[use_ll]
                                   [entry_size_index].dbal_table_id, _SHR_E_FAIL, "Mismatching dbal table id\n");
                    SHR_VAL_VERIFY(gport_hw_res.outlif_dbal_result_type,
                                   semantic_test_local_outlif_info_per_ll_and_size[use_ll]
                                   [entry_size_index].dbal_result_type, _SHR_E_FAIL, "Mismatching dbal result type\n");

                    /*
                     * 4. Then delete them.
                     */
                    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, current_global_lif, NULL, current_local_lif));
                    global_lif_array[current_phase][use_ll][entry_size_index] = 0;
                    local_lif_array[current_phase][use_ll][entry_size_index] = 0;
                }
            }
        }
    }

exit:
    /*
     * If anything fails, delete all outlif. Don't check return value.
     */
    if (SHR_FAILURE(SHR_GET_CURRENT_ERR()))
    {
        for (current_phase = 0; current_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_phase++)
        {
            for (use_ll = 0; use_ll < SEMANTIC_TEST_NOF_LL_OPTIONS; use_ll++)
            {
                for (entry_size_index = 0; entry_size_index < SEMANTIC_TEST_NOF_SIZE_OPTIONS - 1; entry_size_index++)
                {
                    if (global_lif_array[current_phase][use_ll][entry_size_index] != 0)
                    {
                        dnx_lif_lib_free(unit, global_lif_array[current_phase][use_ll][entry_size_index],
                                         NULL, local_lif_array[current_phase][use_ll][entry_size_index]);
                    }
                }
            }
        }
    }

    cli_out("Local outlif semantic test - %s\n", SHR_FUNC_ERR()? "failed :(" : "success!");

    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_local_outlif_allocation_replace(
    int unit,
    lif_mngr_local_outlif_info_t * local_outlif_info,
    int global_lif,
    int new_result_type,
    int test_case,
    int should_be_reused)
{
    int gport;
    int can_be_reused;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_res;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that we can replace the large entry with the small entry.
     */
    local_outlif_info->alternative_result_type = local_outlif_info->dbal_result_type;
    local_outlif_info->dbal_result_type = new_result_type;
    local_outlif_info->local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_can_be_reused(unit, local_outlif_info, &can_be_reused));

    if ((should_be_reused && !can_be_reused) || (!should_be_reused && can_be_reused))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error: should%s be able to reuse entry in test case %d.",
                     (should_be_reused ? "" : " not"), test_case);
    }

    if (!can_be_reused)
    {
        int tmp_lif = local_outlif_info->local_outlif;
        lif_mapping_local_lif_info_t lif_mapping_info;
        /*
         * If can't reuse entry, then reserve a new one and delete the old one.
         */
        local_outlif_info->local_lif_flags |=
            DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RESERVE | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, local_outlif_info));

        /*
         * Delete the old entry's global lif mapping.
         */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, global_lif));

        /*
         * Delete the old entry.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, tmp_lif));

        /*
         * Map the new entry to the global lif.
         */
        sal_memset(&lif_mapping_info, 0, sizeof(lif_mapping_info));
        lif_mapping_info.local_lif = local_outlif_info->local_outlif;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_EGRESS, global_lif, &lif_mapping_info));
    }

    /*
     * Now replace entry. Call lif mngr directly because it can't be done through the lif lib.
     */
    local_outlif_info->local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, local_outlif_info));

    /*
     * Now turn the global lif into a gport, and verify new result type.
     */
    BCM_L3_ITF_SET(gport, BCM_L3_ITF_TYPE_LIF, global_lif);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, gport);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, &gport_hw_res));

    if (gport_hw_res.outlif_dbal_result_type != local_outlif_info->dbal_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error: didn't get correct result type in test case %d. "
                     "Expected %d but was %d", test_case, local_outlif_info->dbal_result_type,
                     gport_hw_res.outlif_dbal_result_type);
    }

    local_outlif_info->local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_local_outlif_allocation_replace_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    lif_mngr_local_outlif_info_t local_outlif_info;
    int global_lif;
    SHR_FUNC_INIT_VARS(unit);

    
    sal_memset(&local_outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    local_outlif_info.dbal_table_id = DBAL_TABLE_EEDB_OUT_AC;
    local_outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_STAT_PROTECTION;
    local_outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_AC;
    local_outlif_info.local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;
    local_outlif_info.local_outlif = 0;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, &global_lif, NULL, &local_outlif_info));

    /*
     * Test case 1: Replace entry of 120b to entry of 60b.
     *      Assuming granularity of 60 or 30 would give smaller entry.
     *      Granularity of 120 would give two same size entries.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_allocation_replace(unit, &local_outlif_info, global_lif,
                                                              DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_QOS_OAM, 1, TRUE));

    

    

    /*
     * Test case 5: Now try to replace from 60 to 120.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_allocation_replace(unit, &local_outlif_info, global_lif,
                                                              DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_STAT_PROTECTION,
                                                              5, FALSE));

    

exit:
    /*
     * Now free the outlif.
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif, NULL, local_outlif_info.local_outlif));

    cli_out("Local outlif semantic test - %s\n", SHR_FUNC_ERR()? "failed :(" : "success!");

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Test that validates correct mdb phase mapping and un-mapping (related jira: SDK-183891):
 *    1. Read bank_allocation_size from dnx data.
 *    2. Allocate #bank_allocation_size lifs on the same explicit phase (per all explicit logical phases).
 *    3. Read mdb_phase_map memory.
 *    4. Delete #bank_allocation_size lifs.
 *    5. Validate clearance of the correct mdb phase map entries.
 */
shr_error_e
ctest_dnx_local_outlif_allocation_phase_map_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    int outlif_phase_idx, num_lifs_in_phase, lif_idx, is_end, num_current_rows, mdb_phase_map_key, mdb_phase_map_value,
        mdb_phase_map_idx, rv;
    int num_init_rows = 0;
    int eedb_bank_size = dnx_data_lif.out_lif.allocation_bank_size_get(unit);
    int *local_outlifs = NULL;
    uint32 entry_handle_id;
    uint32 field_value[1];
    uint32 mdb_phase_map_init_keys[256];
    uint32 mdb_phase_map_init_values[256];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    local_outlifs = sal_alloc(sizeof(int) * eedb_bank_size, "eedb_bank_size");

    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
    outlif_info.dbal_result_type = 0;

    /**Get mdb phase map initialized status*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_7, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, field_value));
        mdb_phase_map_init_keys[num_init_rows] = field_value[0];
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, field_value));
        mdb_phase_map_init_values[num_init_rows] = field_value[0];
        num_init_rows++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    outlif_info.local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;

    for (outlif_phase_idx = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1; outlif_phase_idx <= LIF_MNGR_OUTLIF_PHASE_LAST;
         outlif_phase_idx++)
    {
        num_lifs_in_phase = 0;
        outlif_info.outlif_phase = outlif_phase_idx;
        rv = _SHR_E_NONE;
        /**Add allocation bank size lifs per this phase (or as much as we can below it)*/
        for (lif_idx = 0; lif_idx < eedb_bank_size; lif_idx++)
        {
            rv = dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info);
            if (rv == _SHR_E_RESOURCE)
            {
                break;
            }
            else if (rv != _SHR_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to allocate outlif");
            }
            local_outlifs[num_lifs_in_phase++] = outlif_info.local_outlif;
        }
        sal_printf("\nAllocated %d lifs in phase %d. Now delete them\n", num_lifs_in_phase,
                   (outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1));

        /**Delete the added lifs*/
        for (lif_idx = 0; lif_idx < num_lifs_in_phase; lif_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, local_outlifs[lif_idx]));
        }
        /**Get mdb_7 dump, match with the first dump*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MDB_7, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        num_current_rows = 0;
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, field_value));
            mdb_phase_map_key = field_value[0];
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, field_value));
            mdb_phase_map_value = field_value[0];
            for (mdb_phase_map_idx = 0; mdb_phase_map_idx < num_init_rows; mdb_phase_map_idx++)
            {
                if (mdb_phase_map_init_keys[mdb_phase_map_idx] == mdb_phase_map_key)
                {
                    if (mdb_phase_map_init_values[mdb_phase_map_idx] != mdb_phase_map_value)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "Different mapping found - at init was mapped: [key, val] == [0x%x, 0x%x]. Now mapped [key, val] == [0x%x, 0x%x] \n",
                                     mdb_phase_map_init_keys[mdb_phase_map_idx],
                                     mdb_phase_map_init_values[mdb_phase_map_idx], mdb_phase_map_key,
                                     mdb_phase_map_value);
                    }
                    /**entry is similar to init*/
                    else
                    {
                        break;
                    }
                }
            }
            /**Scanned MDB_7 and couldn't find the entry - a leftover*/
            if (mdb_phase_map_idx == num_init_rows)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Redundant mapping found - [key, val] == [0x%x, 0x%x] is a leftover for explicit phase %d \n",
                             mdb_phase_map_key, mdb_phase_map_value,
                             (outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1));
            }
            num_current_rows++;
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        if (num_current_rows < num_init_rows)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Missing an entry in mdb phase map \n");
        }
        sal_printf("\nValidation passed for phase %d\n", (outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Determine the min lifs expected for outlif capacity test, based on requested lif size, outlif pointer size and granularity*/
shr_error_e
ctest_dnx_local_outlif_allocation_capacity_test_decide_threshold(
    int unit,
    lif_mngr_local_outlif_info_t outlif_info,
    int *min_lifs_expected)
{
    CONST dbal_logical_table_t *table;
    int outlif_size, requested_lif_size;
    uint32 granularity;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**Get min expected allocated lifs, according to requested lifs size, outlif pointer size and clusters granularity*/
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, outlif_info.dbal_table_id, &table));
    requested_lif_size = table->multi_res_info[outlif_info.dbal_result_type].entry_payload_size;
    outlif_size = dnx_data_lif.out_lif.outlif_pointer_size_get(unit);
    /**Checking granularity only on first physical phase, assuming all are equal*/
    SHR_IF_ERR_EXIT(mdb_eedb_table_data_granularity_get(unit, DBAL_PHYSICAL_TABLE_EEDB_1, &granularity));

    if (outlif_size == 20)
    {
        if (granularity == 60)
        {
            if (requested_lif_size <= 60)
            {
                *min_lifs_expected = 600000;
            }
            else                                                                                                                 /**Lif > 60*/
            {
                *min_lifs_expected = 300000;
            }
        }
        else                                                                                                                 /**Granularity == 120*/
        {
            if (requested_lif_size <= 60)
            {
                *min_lifs_expected = 300000;
            }
            else                                                                                                                 /**Lif > 60*/
            {
                *min_lifs_expected = 100000;
            }
        }
    }
    else if (outlif_size == 19)
    {
        if (granularity == 60)
        {
            if (requested_lif_size <= 60)
            {
                *min_lifs_expected = 300000;
            }
            else                                                                                                                 /**Lif > 60*/
            {
                *min_lifs_expected = 100000;
            }
        }
        else                                                                                                                 /**Granularity == 120*/
        {
            if (requested_lif_size <= 60)
            {
                *min_lifs_expected = 100000;
            }
            else                                                                                                                 /**Lif > 60*/
            {
                *min_lifs_expected = 50000;
            }
        }
    }
    else if (outlif_size == 18)
    {
        if (granularity == 60)
        {
            if (requested_lif_size <= 60)
            {
                *min_lifs_expected = 150000;
            }
            else                                                                                                                 /**Lif > 60*/
            {
                *min_lifs_expected = 50000;
            }
        }
        else                                                                                                                 /**Granularity == 120*/
        {
            if (requested_lif_size <= 60)
            {
                *min_lifs_expected = 50000;
            }
            else                                                                                                                 /**Lif > 60*/
            {
                *min_lifs_expected = 25000;
            }
        }
    }
    else                                                                                                         /**outlif pointer size > 20 or < 18*/
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No support for outlif pointer size %d", outlif_size);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Test that checks the capacity per phase of the outlifs:
 *    1. Alloc as many outlifs as possible per phase.
 *    2. Once limit is reached - output result.
 *    3. Perform it per logical phase.
 *    4. Compare the total number of allocated lifs to expected
 *    5. On cleanup, clean as many lifs as requested from each logical phase
 */
shr_error_e
ctest_dnx_local_outlif_allocation_capacity_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int outlif_phase_idx, total_outlifs = 0, rv, is_end, lif_idx, lifs_in_allocation_bank;
    uint32 num_clear_lifs = 0;
    int clear_lifs_w_ll_per_stage[LIF_MNGR_OUTLIF_PHASE_COUNT - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1] = { 0 };
    int clear_lifs_wo_ll_per_stage[LIF_MNGR_OUTLIF_PHASE_COUNT - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1] = { 0 };
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    int num_lifs_per_phase[LIF_MNGR_OUTLIF_PHASE_COUNT - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1] = { 0 };
    char *dbal_table;
    char *dbal_result_type;
    int *clear_lifs_wo_ll = NULL;
    int *clear_lifs_with_ll = NULL;
    int clear_lif_idx = 0;
    int min_lifs_expected, total_clear_lifs_w_ll = 0, total_clear_lifs_wo_ll = 0;
    uint8 clear_all_lifs = 0, compare_expected = 0;
    uint32 entry_handle_id, field_value[1];
    bsl_severity_t severity = bslSeverityOff;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("dbal_table", dbal_table);
    SH_SAND_GET_STR("dbal_result_type", dbal_result_type);
    SH_SAND_GET_INT32("clear_lifs", num_clear_lifs);
    SH_SAND_GET_BOOL("clear_all_lifs", clear_all_lifs);
    SH_SAND_GET_BOOL("compare_expected", compare_expected);

    if ((!ISEMPTY(dbal_table) && ISEMPTY(dbal_result_type)) || (ISEMPTY(dbal_table) && !ISEMPTY(dbal_result_type)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dbal table\result type must be given if the other one is set");
    }

    /** table name was specified */
    if (dbal_logical_table_string_to_id(unit, dbal_table, &outlif_info.dbal_table_id) == _SHR_E_NOT_FOUND)
    {
        /** Table was not found */
        LOG_CLI((BSL_META("No matching tables found\nRelated tables:\n")));
        SHR_IF_ERR_EXIT(dbal_print_table_names(unit, dbal_table));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_result_type_string_to_id
                    (unit, outlif_info.dbal_table_id, dbal_result_type, &outlif_info.dbal_result_type));

    if (num_clear_lifs == 0)
    {
        sal_printf("Number of cleared lifs wasn't given, using %d.\n",
                   CTEST_DNX_LOCAL_OUTLIF_CAPACITY_PHASE_TEST_NUM_CLEAR_LIFS);
        num_clear_lifs = CTEST_DNX_LOCAL_OUTLIF_CAPACITY_PHASE_TEST_NUM_CLEAR_LIFS;
    }

    SHR_IF_ERR_EXIT(dnx_lif_diag_logger_close(unit, &severity));
    /**clear_all_lifs --> first empty the existing lifs. at the end - clear everything*/
    if (clear_all_lifs == TRUE)
    {
        /**Get mdb phase map initialized status*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_OUT_LIF_INFO_SW, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, field_value));
            SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, field_value[0]));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }
    else if (num_clear_lifs > 0)
    {
        /**Alloc an array of clear lifs, to be used on cleanup to remove as many lifs as requested from each stage*/
        clear_lifs_wo_ll =
            sal_alloc(sizeof(int) * num_clear_lifs *
                      (LIF_MNGR_OUTLIF_PHASE_LAST - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1), "eedb_bank_size");
        clear_lifs_with_ll =
            sal_alloc(sizeof(int) * num_clear_lifs *
                      (LIF_MNGR_OUTLIF_PHASE_LAST - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1), "eedb_bank_size");
    }

    /**Get test pass indication - the min num of lifs that should be allocated successfully*/
    SHR_IF_ERR_EXIT(ctest_dnx_local_outlif_allocation_capacity_test_decide_threshold
                    (unit, outlif_info, &min_lifs_expected));

    /**Allocate as many lifs as possible for every phase. Ignore arch ll indication mismatches*/

    /**First - allocate #outlif_bank_size lifs per phase - WITH link list*/
    lifs_in_allocation_bank = dnx_data_lif.out_lif.allocation_bank_size_get(unit);
    for (outlif_phase_idx = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1; outlif_phase_idx <= LIF_MNGR_OUTLIF_PHASE_LAST;
         outlif_phase_idx++)
    {
        outlif_info.outlif_phase = outlif_phase_idx;
        rv = _SHR_E_NONE;
        outlif_info.local_lif_flags =
            DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH |
            DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST;

        for (lif_idx = 0; lif_idx < lifs_in_allocation_bank; lif_idx++)
        {
            rv = dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info);
            if (rv == _SHR_E_RESOURCE)
            {
                break;
            }
            num_lifs_per_phase[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]++;
            if ((num_clear_lifs) && (!clear_all_lifs))
            {
                if (clear_lifs_w_ll_per_stage[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]++ < num_clear_lifs)
                {
                    clear_lifs_with_ll[total_clear_lifs_w_ll++] = outlif_info.local_outlif;
                }
            }
        }
    }

    /**Second - allocate #outlif_bank_size lifs per phase - WO link list*/
    for (outlif_phase_idx = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1; outlif_phase_idx <= LIF_MNGR_OUTLIF_PHASE_LAST;
         outlif_phase_idx++)
    {
        outlif_info.outlif_phase = outlif_phase_idx;
        rv = _SHR_E_NONE;
        outlif_info.local_lif_flags =
            DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST;

        for (lif_idx = 0; lif_idx < lifs_in_allocation_bank; lif_idx++)
        {
            rv = dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info);
            if (rv == _SHR_E_RESOURCE)
            {
                break;
            }
            num_lifs_per_phase[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]++;
            if ((num_clear_lifs) && (!clear_all_lifs))
            {
                if (clear_lifs_wo_ll_per_stage[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]++ < num_clear_lifs)
                {
                    clear_lifs_wo_ll[total_clear_lifs_wo_ll++] = outlif_info.local_outlif;
                }
            }
        }
    }

    for (outlif_phase_idx = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1; outlif_phase_idx <= LIF_MNGR_OUTLIF_PHASE_LAST;
         outlif_phase_idx++)
    {
        outlif_info.outlif_phase = outlif_phase_idx;
        rv = _SHR_E_NONE;

        outlif_info.local_lif_flags =
            DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_NO_LINKED_LIST;
        while (rv == _SHR_E_NONE)
        {
            rv = dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info);
            if (rv == _SHR_E_NONE)
            {
                num_lifs_per_phase[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]++;
                if ((num_clear_lifs) && (!clear_all_lifs))
                {
                    if (clear_lifs_wo_ll_per_stage[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]++ <
                        num_clear_lifs)
                    {
                        clear_lifs_wo_ll[total_clear_lifs_wo_ll++] = outlif_info.local_outlif;
                    }
                }
            }
        }

        total_outlifs += num_lifs_per_phase[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1];
        sal_printf("Capacity for phase %d reached: %d.\n", (outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1 + 1),
                   num_lifs_per_phase[outlif_phase_idx - LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1]);
    }
    sal_printf("Total capacity reached: %d.\n", total_outlifs);

    if (compare_expected)
    {
        sal_printf("Min capacity expected: %d\n", min_lifs_expected);
        if (total_outlifs < min_lifs_expected)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_FAIL);
        }
    }

exit:
    SHR_IF_ERR_EXIT(dnx_lif_diag_logger_restore(unit, severity));
    if (clear_all_lifs == TRUE)
    {
        /**Get mdb phase map initialized status*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_LOCAL_OUT_LIF_INFO_SW, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, field_value));
            SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, field_value[0]));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }
    else
    {
        /**Clean #num_lifs from each stage*/
        /**WO ll*/
        for (clear_lif_idx = 0; clear_lif_idx < total_clear_lifs_wo_ll; clear_lif_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, clear_lifs_wo_ll[clear_lif_idx]));
        }
        /**With ll*/
        for (clear_lif_idx = 0; clear_lif_idx < total_clear_lifs_w_ll; clear_lif_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, clear_lifs_with_ll[clear_lif_idx]));
        }

    }
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Verify that for profiles which support msb bit (20 bits for jr2, 19 for q2a etc), lifs with ll are receiving values with msb zeroed
 */
shr_error_e
ctest_dnx_local_outlif_allocation_negative_ll_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv;
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /**Test should only run on profiles with max outlif pointer size possible*/
    if ((dnx_data_lif.out_lif.outlif_profile_width_get(unit)) != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_EXIT();
    }
    outlif_info.local_outlif = 1 << dnx_data_lif.out_lif.outlif_eedb_banks_pointer_size_get(unit);
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_ARP;

    outlif_info.local_lif_flags =
        DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH |
        DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_LINKED_LIST | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_WITH_ID |
        DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_RES_IGNORE_TAG;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_EXPLICIT_1;
    rv = _SHR_E_NONE;

    rv = dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info);

    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocated lif 0x%x with ll, should have failed", outlif_info.local_outlif);
    }
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

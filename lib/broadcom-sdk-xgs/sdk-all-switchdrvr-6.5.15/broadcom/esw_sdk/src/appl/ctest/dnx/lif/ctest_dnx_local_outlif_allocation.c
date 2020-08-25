/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>

#include <bcm_int/dnx/lif/lif_lib.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>

/**
 * }
 */

/*************
 * DEFINES   *
 *************/
/*
 * {
 */

#define CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE    (DNX_DATA_MAX_LIF_OUT_LIF_NOF_LOCAL_OUT_LIFS / DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT)

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
int allocated_local_lifs[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT][CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE] =
    { {0} };
int allocated_global_lifs[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT][CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE] =
    { {0} };
int nof_allocated_outlifs_per_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT];

lif_mngr_local_outlif_info_t local_outlif_info_per_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = {
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

/**
 * \brief
 * Man for eedb test.
 */
sh_sand_man_t dnx_outlif_allocation_test_man = {
    "Stress test for local outlif allocation.",
    "Attempt to allocate all local outlifs, then free them, then allocate again with a different phase."
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
     * Free outlifs until you reach an empty one.
     */
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_2;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        current_lif_index = 0;
        while (allocated_local_lifs[current_logical_phase][current_lif_index] != 0)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_free
                            (unit, allocated_global_lifs[current_logical_phase][current_lif_index], NULL,
                             allocated_local_lifs[current_logical_phase][current_lif_index]));
            allocated_global_lifs[current_logical_phase][current_lif_index] = 0;
            allocated_local_lifs[current_logical_phase][current_lif_index] = 0;
            current_lif_index++;
            if (current_lif_index % 4192 == 0)
            {
                cli_out(".");
            }
        }

        nof_allocated_outlifs_per_phase[current_logical_phase] = 0;
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

    local_outlif_info = &local_outlif_info_per_phase[logical_phase];

    /*
     * Allocate the given number of outlifs, or if input was 0, set an arbitrarily high number.
     */
    nof_outlifs = (target_nof_outlifs) ? target_nof_outlifs : CTEST_DNX_LOCAL_OUTLIF_MAX_OUTLIFS_PER_STAGE;
    outlif_array_start_index = nof_allocated_outlifs_per_phase[logical_phase];

    for (current_lif_index = 0; current_lif_index < nof_outlifs; current_lif_index++)
    {
        local_outlif_info->local_lif_flags = 0;
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

        allocated_local_lifs[logical_phase][outlif_array_start_index + current_lif_index] =
            local_outlif_info->local_outlif;
        allocated_global_lifs[logical_phase][outlif_array_start_index + current_lif_index] = global_lif;
    }

    nof_allocated_outlifs_per_phase[logical_phase] += current_lif_index;

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
                phase_names[current_logical_phase], nof_allocated_outlifs_per_phase[current_logical_phase]);
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
    uint32 mdb_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Some MDB profiles have less EEDB entries than JR1, so don't expect them to reach
     * the JR1 number of outlifs.
     */
    mdb_profile = dnx_data_mdb.pdbs.mdb_profile_get(unit)->val;
    switch (mdb_profile)
    {
        case MDB_BALANCED_PROFILE:
        case MDB_BALANCED_EXEM_PROFILE:
            jr1_compatible = FALSE;
            break;
        default:
            jr1_compatible = TRUE;
            break;
    }

    /*
     * Add the number of rifs to the number of allocated GLEM entries, since the API would allocate them.
     */
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_update_count
                    (unit, DNX_ALGO_LIF_EGRESS, dnx_data_l3.rif.nof_rifs_get(unit)));
    nof_allocated_outlifs_per_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_1] = dnx_data_l3.rif.nof_rifs_get(unit);

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
                    nof_allocated_outlifs_per_phase[current_logical_phase],
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
                                                                                              &local_outlif_info_per_phase
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
                    nof_allocated_outlifs_per_phase[current_logical_phase],
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
                                                                                              &local_outlif_info_per_phase
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

/*
 * }
 */

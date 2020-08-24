/** \file diag_dnx_ecmp_consistent.c
 * $Id$
 *
 * Tests for consistent ecmp
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <shared/bsl.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
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
 * Enumeration
 * {
 */
/*
 * \brief The following enumeration represent different test cases of the consistent tables.
 * The SW behaves different in each one of the following cases.
 */
typedef enum
{
    /*
     * Test case were the actual members in the group equal to the group max size.
     */
    DIAG_DNX_ECMP_CONST_MEMBERS_COUNT_EQUAL_MEX,
    /*
     * Test case were the actual members in the group are less than the group max size.
     */
    DIAG_DNX_ECMP_CONST_MEMBERS_COUNT_SMALLER_MEX,
    /*
     * Test case where the table full capacity is used
     */
    DIAG_DNX_ECMP_CONST_FULL_CAPACITY,
    /*
     * NOF of consistent tables test cases.
     */
    DIAG_DNX_ECMP_CONST_NOF_TBLS_TEST
} diag_dnx_ecmp_const_test_sem_table_update_type_e;
/*
 * }
 */

/*
 * Structures
 * {
 */
/**
 * \brief this structure holds all the relevant field to DNX ECMP group setting.
 */
typedef struct
{

    /*
     * BCM_L3_xxx flags.
     */
    uint32 flags;
    /*
     * The ECMP ID
     */
    bcm_if_t ecmp_intf;
    /*
     * The ECMP max group size (not necessarily the actual NOF of members in the group)
     */
    int max_paths;
    /*
     * BCM_L3_ECMP_xxx flags.
     */
    uint32 ecmp_group_flags;
    /*
     * Actual NOF members in the ECMP group.
     */
    int intf_count;
} diag_dnx_ecmp_relevant_fields_t;
/*
 * }
 */

/*
 * Externs
 * {
 */

/*
 * }
 */
/*
 * Globals.
 * {
 */
const char *ecmp_const_sem_tests_description[DIAG_DNX_ECMP_CONST_NOF_TBLS_TEST] =
    { "ECMP CONSISTENT MAX GROUP SIZE EQUAL GROUP SIZE", "ECMP CONSISTENT MAX GROUP SIZE BIGGER THAN GROUP SIZE",
    "ECMP CONSISTENT FULL CAPACITY"
};

const char *ecmp_const_sem_table_types_description[L3_ECMP_CONSISTENT_TABLE_NOF_TYPES] =
    { "SMALL SONSISTENT TABLE", "MEDIUM SONSISTENT TABLE",
    "LARGE SONSISTENT TABLE"
};

/** Consistent ECMP test details */
static sh_sand_man_t sh_dnx_ecmp_consistent_man = {
    "ECMP consistent hashing testing",
    "Covers basic ECMP consistent semantic testing of all the  bcm_l3_egress_ecmp_ create/destroy/get/add/delete APIs"
};

/** Consistent ECMP test details */
static sh_sand_man_t sh_dnx_ecmp_performance_man = {
    "ECMP performance testing",
    "Times the execution time of the ECMP create API"
};

/** Consistent ECMP test details */
static sh_sand_man_t sh_dnx_ecmp_tunnel_priority_sem_man = {
    "Tunnel priority semantic test",
    "Test the tunnel priority semantic behavior."
};

/*
 * }
 */
/**
 * \brief
 * Verify that the bcm_l3_egress_ecmp_get function return the expected values
 * \param [in] unit -
 *   The unit number.
 * \param [in] ecmp_expected -
 *   The expected ECMP group info
 * \param [in] intf_count_expected -
 *   The expected NOF members
 * \param [in] intf_array_expected -
 *   The expected members
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static bcm_error_t
appl_dnx_ecmp_const_is_received_as_expected(
    int unit,
    bcm_l3_egress_ecmp_t * ecmp_expected,
    int intf_count_expected,
    bcm_if_t * intf_array_expected)
{
    int intf_count_received;
    bcm_l3_egress_ecmp_t ecmp_received;
    bcm_if_t intf_array_received[L3_ECMP_MAX_NOF_ECMP_MEMBERS];

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_ecmp_t_init(&ecmp_received);

    ecmp_received.ecmp_intf = ecmp_expected->ecmp_intf;

    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get
                    (unit, &ecmp_received, intf_count_expected, intf_array_received, &intf_count_received));

    /*
     * Compare all the returned values from the ECMP get API with the expected values
     */
    ecmp_received.flags = ecmp_expected->flags;/** we don't care about the BCM_L3* flags in the compare */
    if ((sal_memcmp(ecmp_expected, &ecmp_received, sizeof(bcm_l3_egress_ecmp_t)) != 0)
        || (intf_count_expected != intf_count_received)
        || (sal_memcmp(intf_array_expected, intf_array_received, intf_count_expected * sizeof(bcm_if_t)) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned value from bcm_l3_egress_ecmp_get isn't matching expected one (interface expected count %d) \n",
                     intf_count_expected);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * semantic test for consistent hashing
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static bcm_error_t
appl_dnx_ecmp_consistent_semantic(
    int unit)
{
    /*
     * This table holds ECMP create API information to create each one of the three tables (see l3_ecmp_consistent_type_e)
     */
    diag_dnx_ecmp_relevant_fields_t ecmp_info[L3_ECMP_CONSISTENT_TABLE_NOF_TYPES] = {
        {BCM_L3_WITH_ID, 33, 12, 0, 6},
        {BCM_L3_WITH_ID, 44, 80, 0, 25},
        {BCM_L3_WITH_ID, 55, 80, BCM_L3_ECMP_LARGE_TABLE, 25}
    };

    int table_iter;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_egress_t egr_intf;
    bcm_if_t intf_array_sent[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int intf_iter;
    int test_type;
    uint32 base_fec, fec_range;
    bcm_if_t intf_id = 0x40001002;
    int intf_count;
    int fec_id;
    uint32 fec_flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    dnx_l3_ecmp_consistent_type_t const_table_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, 0, &base_fec, &fec_range));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP CONSISTENT HASHING SEMANTIC TEST START.\n")));

    for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS; intf_iter++)
    {
        fec_id = base_fec + intf_iter;
        bcm_l3_egress_t_init(&egr_intf);
        egr_intf.intf = intf_id;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, fec_flags, &egr_intf, &fec_id));
        intf_array_sent[intf_iter] = fec_id;
        intf_id++;
    }

    /*
     * Iterate of different scenarios of ECMP NOF group members and max size (see diag_dnx_ecmp_const_test_sem_table_update_type_e)
     */
    for (test_type = 0; test_type < DIAG_DNX_ECMP_CONST_NOF_TBLS_TEST; test_type++)
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "%s.\n"), ecmp_const_sem_tests_description[test_type]));

        /*
         * Iterate over all the consistent table types.
         */
        for (table_iter = L3_ECMP_CONSISTENT_TABLE_SMALL; table_iter < L3_ECMP_CONSISTENT_TABLE_NOF_TYPES; table_iter++)
        {

            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "ECMP CONSISTENT HASHING SEMANTIC FOR %s.\n"),
                      ecmp_const_sem_table_types_description[table_iter]));

            SHR_IF_ERR_EXIT(dnx_l3_ecmp_consistent_members_table_info_get(unit, table_iter, &const_table_info));

            bcm_l3_egress_ecmp_t_init(&ecmp);

            ecmp.ecmp_intf = ecmp_info[table_iter].ecmp_intf;
            ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
            ecmp.flags = ecmp_info[table_iter].flags;
            ecmp.ecmp_group_flags = ecmp_info[table_iter].ecmp_group_flags;

            switch (test_type)
            {
                case DIAG_DNX_ECMP_CONST_MEMBERS_COUNT_EQUAL_MEX:
                {
                    ecmp.max_paths = ecmp_info[table_iter].max_paths;
                    intf_count = ecmp_info[table_iter].max_paths;
                    break;
                }
                case DIAG_DNX_ECMP_CONST_MEMBERS_COUNT_SMALLER_MEX:
                {
                    ecmp.max_paths = ecmp_info[table_iter].max_paths;
                    intf_count = ecmp_info[table_iter].intf_count;
                    break;
                }
                case DIAG_DNX_ECMP_CONST_FULL_CAPACITY:
                {
                    ecmp.max_paths = const_table_info.max_nof_unique_members;
                    intf_count = const_table_info.max_nof_unique_members;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Unknown consistent table test type - %d.\n", test_type);
                }
            }

            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count, intf_array_sent));

            SHR_IF_ERR_EXIT(appl_dnx_ecmp_const_is_received_as_expected(unit, &ecmp, intf_count, intf_array_sent));

            /*
             * Delete all the member till the last one as ECMP can't have less than a single member
             * to verify that the "delete" command works as expected
             */
            for (intf_iter = intf_count - 1; intf_iter > 0; intf_iter--)
            {
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp, intf_array_sent[intf_iter]));

                SHR_IF_ERR_EXIT(appl_dnx_ecmp_const_is_received_as_expected(unit, &ecmp, intf_iter, intf_array_sent));
            }

            /*
             * Add all the members back to verify that the "add" command works as expected
             */
            for (intf_iter = 2; intf_iter < ecmp.max_paths; intf_iter++)
            {
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp, intf_array_sent[intf_iter - 1]));

                SHR_IF_ERR_EXIT(appl_dnx_ecmp_const_is_received_as_expected(unit, &ecmp, intf_iter, intf_array_sent));
            }

            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));

        }
    }

exit:
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP CONSISTENT HASHING SEMANTIC TEST END.\n")));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Main ECMP consistent testing command
 */
static shr_error_e
sh_dnx_ecmp_consistent_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP CONSISTENT HASHING TEST START.\n")));
    SHR_IF_ERR_EXIT(appl_dnx_ecmp_consistent_semantic(unit));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP CONSISTENT HASHING TEST END.\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Main ECMP performance testing command
 */
shr_error_e
sh_dnx_ecmp_performance_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int intf_iter;
    int fec_id;
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int idx;
    int ecmp_intf_start = 2000;
    int intf_count = 10;
    int hier_id;
    int is_active;
    int nof_hierarchies = 3;
    bcm_l3_egress_t egr_intf;
    bcm_l3_egress_ecmp_t ecmp;
    uint32 base_fec, range_size;
    uint32 fec_flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    uint32 timer_group;
    uint32 hier_flags[3] = { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };
    uint32 ecmp_ids[L3_ECMP_MAX_NOF_ECMP_MEMBERS * 6];
    uint32 hits[2] = { 0, 0 };
    uint32 total_time[2] = { 0, 0 };
    uint32 average_time[2] = { 400, 130 };
    char *name;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get
                    (unit, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3, &base_fec, &range_size));

    /** Allocate timer group ID with a given name*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_group_allocate("ECMP time performance", &timer_group));
    utilex_ll_timer_clear_all(timer_group);
    for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS; intf_iter++)
    {
        fec_id = base_fec + intf_iter;
        bcm_l3_egress_t_init(&egr_intf);
        egr_intf.port = 200;
        egr_intf.flags = BCM_L3_3RD_HIERARCHY;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, fec_flags, &egr_intf, &fec_id));
        intf_array[intf_iter] = fec_id;
    }

    printf("Allocating groups with ID and different group profile \n");
    for (hier_id = 0, idx = 0; hier_id < nof_hierarchies; hier_id++)
    {
        int ecmp_start_temp = ecmp_intf_start + (hier_id * 0x2000);
        for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS - intf_count; intf_iter++, idx++)
        {
            bcm_l3_egress_ecmp_t_init(&ecmp);
            ecmp.ecmp_intf = ecmp_start_temp + intf_iter;
            ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
            ecmp.flags = BCM_L3_WITH_ID | hier_flags[hier_id];
            ecmp.max_paths = intf_count + intf_iter;
            utilex_ll_timer_set("bcm_l3_egress_ecmp_create WITH ID", timer_group, 1);
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count + intf_iter, intf_array));
            utilex_ll_timer_stop(timer_group, 1);
            ecmp_ids[idx] = ecmp.ecmp_intf;
        }
    }

    printf("Allocating groups without ID and same group profile \n");
    for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
    {
        for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS; intf_iter++, idx++)
        {
            bcm_l3_egress_ecmp_t_init(&ecmp);
            ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
            ecmp.flags = hier_flags[hier_id];
            ecmp.max_paths = intf_count;
            utilex_ll_timer_set("bcm_l3_egress_ecmp_create without ID", timer_group, 2);
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, intf_count, intf_array));
            utilex_ll_timer_stop(timer_group, 2);
            ecmp_ids[idx] = ecmp.ecmp_intf;
        }
    }

    for (intf_iter = 0; intf_iter < idx; intf_iter++)
    {
        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.ecmp_intf = ecmp_ids[intf_iter];
        SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));
    }
    for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS; intf_iter++)
    {
        SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, intf_array[intf_iter]));
    }

    /** Receive the total time that it took to add nof_entries number of FEC entries*/
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timer_group, 1, &is_active, &name, &hits[0], &total_time[0]));
    SHR_IF_ERR_EXIT(utilex_ll_timer_get_info(timer_group, 2, &is_active, &name, &hits[1], &total_time[1]));

    utilex_ll_timer_stop_all(timer_group);
    utilex_ll_timer_print_all(timer_group);
    utilex_ll_timer_clear_all(timer_group);

    printf("_______________________________________________________\n\n");
    printf("AVERAGE time for bcm_l3_egress_ecmp_create WITH ID - %u us, expected approximately %d us\n",
           (total_time[0] / hits[0]), average_time[0]);
    printf("AVERAGE time for bcm_l3_egress_ecmp_create without ID - %u us, expected approximately %d us\n",
           (total_time[1] / hits[1]), average_time[1]);
    printf("_______________________________________________________\n");

    /** Verify that the average execution time for adding a single entry is within limits. */
#if defined(ADAPTER_SERVER_MODE)
    /** Skip comparing with average expected time for cmodel execution. */
#else
    if ((total_time[0] / hits[0]) > (average_time[0] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ECMP create WITH ID performance is out of 10%% limit - value should be less than %f\n",
                     (average_time[0] * 1.1));
    }
    if ((total_time[1] / hits[1]) > (average_time[1] * 1.1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ECMP create without ID performance is out of 10%% limit - value should be less than %f\n",
                     (average_time[1] * 1.1));
    }

#endif
exit:
    SHR_SET_CURRENT_ERR(utilex_ll_timer_group_free(timer_group));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * ECMP tunnel priority semantic test
 */
shr_error_e
sh_dnx_ecmp_tunnel_priority_sem_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ecmp_id = 50;
    int ecmp_group_size = 80; /** this variable must be divided by 8 */
    int intf_iter;
    int fec_id, out_of_bound_fec_id;
    int interfac_count_get;
    uint32 base_fec, range_size;
    bcm_l3_egress_t egr_intf;
    bcm_l3_egress_ecmp_t ecmp, ecmp_get;
    l3_ecmp_consistent_type_e table_size_iter;
    int ecmp_group_size_per_table_size[L3_ECMP_CONSISTENT_TABLE_NOF_TYPES] = { 16, 256, 256 };
    int intf_array[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    int intf_array_get[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    uint8 found[L3_ECMP_MAX_NOF_ECMP_MEMBERS];
    bcm_l3_ecmp_tunnel_priority_map_info_t map_info;
    bcm_l3_ecmp_tunnel_priority_mode_t tp_mode;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * In case tunnel priority isn't supported exit the test
     */
    if (!dnx_data_l3.ecmp.tunnel_priority_support_get(unit))
    {
        SHR_EXIT();
    }

    /*
     * Allocate FECs for the ECMP group
     */
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get
                    (unit, DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1, &base_fec, &range_size));

    base_fec = UTILEX_MAX(base_fec, dnx_data_l3.ecmp.total_nof_ecmp_get(unit));

    for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS; intf_iter++)
    {
        fec_id = base_fec + intf_iter;
        bcm_l3_egress_t_init(&egr_intf);
        egr_intf.port = 200;
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &egr_intf, &fec_id));
        intf_array[intf_iter] = fec_id;
    }
    /*
     * Allocate another FEC that would be out of the range of the ECMP group for negative testing
     */
    out_of_bound_fec_id = base_fec + 4 * L3_ECMP_MAX_NOF_ECMP_MEMBERS;
    bcm_l3_egress_t_init(&egr_intf);
    egr_intf.port = 200;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &egr_intf, &out_of_bound_fec_id));

    /*
     * Allocate a map priority for the TP
     */
    map_info.l3_flags = 0;
    SHR_IF_ERR_EXIT(bcm_l3_ecmp_tunnel_priority_map_create(unit, &map_info));

    /*
     * Init the ECMP group
     */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.max_paths = ecmp_group_size;
    ecmp.flags = BCM_L3_WITH_ID;
    ecmp.ecmp_intf = ecmp_id;
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    ecmp.tunnel_priority.index = 0;
    ecmp.tunnel_priority.map_profile = map_info.map_profile;
    ecmp.tunnel_priority.mode = bcmL3EcmpTunnelPriorityModeTwoPriorities;
    /*
     * When creating an ECMP with tunnel priority the create function must get a single FEC which is the FEC base
     * and not two FEC as in this case.
     */
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 2, intf_array), _SHR_E_PARAM);
    /*
     * Fail to use out of bound tunnel priority mode
     */
    ecmp.tunnel_priority.mode = bcmL3EcmpTunnelPriorityModeEightPriorities + 1;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array), _SHR_E_PARAM);
    ecmp.tunnel_priority.mode = bcmL3EcmpTunnelPriorityModeTwoPriorities;

    /*
     * Fail to use an unallocated map profile
     * As there are 4 profiles the modulo 4 is used
     */
    ecmp.tunnel_priority.map_profile = (map_info.map_profile + 1) & 0x3;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array), _SHR_E_NOT_FOUND);
    ecmp.tunnel_priority.map_profile = map_info.map_profile;
    /*
     * Fail to TP without resilient mode.
     */
    ecmp.dynamic_mode = 0;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array), _SHR_E_PARAM);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;

    /*
     * Successfully create an ECMP group
     */
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array));

    /*
     * Fail in adding a table with an out of bound index.
     */
    ecmp.tunnel_priority.index = 2;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_tunnel_priority_set(unit, &ecmp, 40, intf_array), _SHR_E_PARAM);
    ecmp.tunnel_priority.index = 0;
    /*
     * Fail in adding a 0 interfaces into the table.
     */
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_tunnel_priority_set(unit, &ecmp, 0, intf_array), _SHR_E_PARAM);
    /*
     * Fail to create the table with a FEC which isn't in the ECMP group.
     */
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_tunnel_priority_set(unit, &ecmp, 1, &out_of_bound_fec_id),
                               _SHR_E_PARAM);

    /*
     * Successfully set tables 0 and 1;
     */
    ecmp.tunnel_priority.index = 0;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_tunnel_priority_set(unit, &ecmp, 20, intf_array));
    ecmp.tunnel_priority.index = 1;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_tunnel_priority_set(unit, &ecmp, 20, intf_array));

    /*
     * Fail to get an ECMP group with an out of range table
     */
    bcm_l3_egress_ecmp_t_init(&ecmp_get);
    ecmp_get.ecmp_intf = ecmp_id;
    ecmp_get.tunnel_priority.index = 2;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_get
                               (unit, &ecmp_get, L3_ECMP_MAX_NOF_ECMP_MEMBERS, intf_array_get, &interfac_count_get),
                               _SHR_E_PARAM);
    /*
     * Successfully get the ECMP group ;
     */
    ecmp_get.tunnel_priority.index = 0;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get
                    (unit, &ecmp_get, L3_ECMP_MAX_NOF_ECMP_MEMBERS, intf_array_get, &interfac_count_get));
    /*
     * Fail to add a member to an out of range table
     */
    ecmp.tunnel_priority.index = 2;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp, intf_array[20]), _SHR_E_PARAM);
    /*
     * Successfully add a member ;
     */
    ecmp.tunnel_priority.index = 1;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp, intf_array[20]));
    /*
     * Fail to remove a member to an out of range table
     */
    ecmp.tunnel_priority.index = 2;
    SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp, intf_array[20]), _SHR_E_PARAM);
    /*
     * Successfully remove a member ;
     */
    ecmp.tunnel_priority.index = 1;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp, intf_array[20]));

    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));

    BCM_L3_ITF_SET(base_fec, BCM_L3_ITF_TYPE_FEC, base_fec);

    /*
     * Iterate over all the consistent table types.
     */
    for (table_size_iter = L3_ECMP_CONSISTENT_TABLE_SMALL; table_size_iter < L3_ECMP_CONSISTENT_TABLE_NOF_TYPES;
         table_size_iter++)
    {
        ecmp_group_size = ecmp_group_size_per_table_size[table_size_iter];
        /*
         * iterate over all the TP modes
         */
        for (tp_mode = bcmL3EcmpTunnelPriorityModeTwoPriorities; tp_mode <= bcmL3EcmpTunnelPriorityModeEightPriorities;
             tp_mode++)
        {
            int table_iter;
            int nof_tabels = L3_ECMP_NOF_CONSISTENT_TABLES_REQUIRED_BY_TP_MODE(tp_mode);
            int interfaces_per_table;

            /*
             * This test uses a different set of members for each TP table, to each TP table will get the total NOF members
             * divided by the NOF tables.
             */
            interfaces_per_table = ecmp_group_size / nof_tabels;

            /*
             * Create an ECMP group
             */
            bcm_l3_egress_ecmp_t_init(&ecmp);
            ecmp.max_paths = ecmp_group_size;
            ecmp.flags = BCM_L3_WITH_ID;
            ecmp.ecmp_group_flags = (table_size_iter == L3_ECMP_CONSISTENT_TABLE_LARGE) ? BCM_L3_ECMP_LARGE_TABLE : 0;
            ecmp.ecmp_intf = ecmp_id;
            ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
            ecmp.tunnel_priority.index = 0;
            ecmp.tunnel_priority.map_profile = map_info.map_profile;
            ecmp.tunnel_priority.mode = tp_mode;

            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, &ecmp, 1, intf_array));

            /*
             * iterate over all the TP tables (according to the NOF priorities each ECMP TP mode has)
             */
            for (table_iter = 0; table_iter < nof_tabels; table_iter++)
            {
                int member_iter, member_to_add, member_to_remove;
                int added_found = FALSE;
                int table_min_fec_id = base_fec + table_iter * interfaces_per_table;

                /*
                 * Fill the current priority table with a unique set of members
                 */
                ecmp.tunnel_priority.index = table_iter;
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_tunnel_priority_set
                                (unit, &ecmp, interfaces_per_table, &intf_array[table_iter * interfaces_per_table]));

                /*
                 * Get the TP table that was just updated.
                 */
                bcm_l3_egress_ecmp_t_init(&ecmp_get);
                ecmp_get.ecmp_intf = ecmp_id;
                ecmp_get.tunnel_priority.index = table_iter;
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get
                                (unit, &ecmp_get, L3_ECMP_MAX_NOF_ECMP_MEMBERS, intf_array_get, &interfac_count_get));

                SHR_ASSERT_EQ(interfac_count_get, interfaces_per_table);

                sal_memset(found, 0, interfaces_per_table);

                /*
                 * Make sure that all the table members were found and that they are in the expected range
                 */
                for (member_iter = 0; member_iter < interfaces_per_table; member_iter++)
                {
                    int member_index = intf_array_get[member_iter];

                    if ((member_index < table_min_fec_id) || (member_index >= table_min_fec_id + interfaces_per_table))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "FEC member 0x%x isn't in the expected range of 0x%x-0x%x.\n", member_index,
                                     table_min_fec_id, table_min_fec_id + interfaces_per_table - 1);
                    }

                    found[member_index - table_min_fec_id] = TRUE;

                }

                for (member_iter = 0; member_iter < interfaces_per_table; member_iter++)
                {
                    if (!found[intf_array_get[member_iter] - table_min_fec_id])
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "FEC member 0x%x wasn't found.\n", intf_array_get[member_iter]);
                    }
                }

                /*
                 * Add and delete a member
                 * The selected member to add will be a member from another TP table range (as we gave each TP a different range in this test).
                 * The selected member to remove will be in the current TP range;
                 */
                member_to_add =
                    base_fec + ((table_iter + 1) % nof_tabels) * interfaces_per_table +
                    (sal_rand() % interfaces_per_table);
                member_to_remove = table_min_fec_id + (sal_rand() % interfaces_per_table);

                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_add(unit, &ecmp, member_to_add));
                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_delete(unit, &ecmp, member_to_remove));

                SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get
                                (unit, &ecmp_get, L3_ECMP_MAX_NOF_ECMP_MEMBERS, intf_array_get, &interfac_count_get));

                SHR_ASSERT_EQ(interfac_count_get, interfaces_per_table);
                SHR_ASSERT_EQ(ecmp.tunnel_priority.map_profile, ecmp_get.tunnel_priority.map_profile);
                SHR_ASSERT_EQ(ecmp.tunnel_priority.mode, ecmp_get.tunnel_priority.mode);

                sal_memset(found, 0, interfaces_per_table);

                /*
                 * Make sure that all the table members were found and that they are in the expected range
                 */
                for (member_iter = 0; member_iter < interfaces_per_table; member_iter++)
                {
                    int member_index = intf_array_get[member_iter];

                    if (member_index == member_to_add)
                    {
                        added_found = TRUE;
                    }
                    else if ((member_index < table_min_fec_id)
                             || (member_index >= table_min_fec_id + interfaces_per_table))
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "FEC member 0x%x isn't in the expected range of 0x%x-0x%x.\n", member_index,
                                     table_min_fec_id, table_min_fec_id + interfaces_per_table - 1);
                    }
                    else
                    {
                        found[member_index - table_min_fec_id] = TRUE;
                    }

                }

               /** Verify that the added member was indeed added */
                if (!added_found)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Member 0x%x was added to the ECMP group but wasn't found.\n",
                                 member_to_add);
                }

                for (member_iter = 0; member_iter < interfaces_per_table; member_iter++)
                {
                    int member_index = intf_array_get[member_iter];

                    /*
                     * Ignore the added member as this was already checked.
                     * Make sure that the removed member was removed and that the rest of the member exists.
                     */
                    if (member_index != member_to_add)
                    {
                        if (member_index == member_to_remove && found[member_index - table_min_fec_id])
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "Member 0x%x was expected to be removed but was found.\n",
                                         member_to_remove);
                        }
                        else if (!found[member_index - table_min_fec_id])
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL, "FEC member 0x%x wasn't found.\n", intf_array_get[member_iter]);
                        }
                    }
                }

            }

            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));
        }

    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP tunnel priority semantic test ended successfully.\n")));

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief DNX ECMP Tests
 * List of ecmp ctests modules.
 */
sh_sand_cmd_t dnx_ecmp_test_cmds[] = {
    {"consistent", sh_dnx_ecmp_consistent_cmd, NULL, NULL, &sh_dnx_ecmp_consistent_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"performance", sh_dnx_ecmp_performance_cmd, NULL, NULL, &sh_dnx_ecmp_performance_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"tunnel_priority_sem", sh_dnx_ecmp_tunnel_priority_sem_cmd, NULL, NULL, &sh_dnx_ecmp_tunnel_priority_sem_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE

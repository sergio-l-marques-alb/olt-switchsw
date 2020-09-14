/** \file diag_dnx_ecmp_consistent.c
 * $Id$
 *
 * Tests for consistent ecmp
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
#include <shared/bsl.h>
#include <bcm/l3.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <bcm/instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include "../../../diag/dnx/pp/diag_dnx_pp.h"
#include <soc/dnx/dnx_err_recovery_manager.h>

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
 * MACROs
 * {
 */
/**
 * Macros used by the pp vis fer fwd diagnostics test:
 */
/** The size of the small group used in the test, has to be up to 16 because of the ECMP mode configuration */
#define CTEST_DNX_L3_ECMP_SMALL_GROUP_SIZE           16

/** The size of the large group used in the test is set to be the maximum consistent group size */
#define CTEST_DNX_L3_ECMP_LARGE_GROUP_SIZE           SAL_BIT(DNX_DATA_MAX_L3_ECMP_GROUP_SIZE_CONSISTENT_NOF_BITS)

/** in and port in use by the test */
#define CTEST_DNX_L3_ECMP_IN_PORT                    200

/** nof out ports in use by the test */
#define CTEST_DNX_L3_ECMP_NOF_OUT_PORTS              4

/** The number of packets that are being send by the test for each ECMP mode */
#define CTEST_DNX_L3_ECMP_NOF_PACKETS                20

/** The ID of the out lif that are being configured and used by the test */
#define CTEST_DNX_L3_ECMP_CONFIG_OUT_RIF             100

/** The ID of the encapsulation ID that are being configured and used by the test */
#define CTEST_DNX_L3_ECMP_CONFIG_ENCAP_ID            0x1384
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
extern uint64 dnx_l3_performance_random_get_64(
    void);
/*
 * }
 */
/*
 * Globals.
 * {
 */
/** ports in use during the pp vis fer fwd diagnostic test*/
static const int out_ports[CTEST_DNX_L3_ECMP_NOF_OUT_PORTS] = { 200, 201, 202, 203 };

const char *ecmp_const_sem_tests_description[DIAG_DNX_ECMP_CONST_NOF_TBLS_TEST] =
    { "ECMP CONSISTENT MAX GROUP SIZE EQUAL GROUP SIZE", "ECMP CONSISTENT MAX GROUP SIZE BIGGER THAN GROUP SIZE",
    "ECMP CONSISTENT FULL CAPACITY"
};

const char *ecmp_const_sem_table_types_description[DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES] =
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

/** Consistent ECMP test details */
static sh_sand_man_t sh_dnx_ecmp_diag_vis_fer_man = {
    "ECMP visibility FER forward test",
    "Diagnostic test for the ECMP FEC forward resolution."
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
    bcm_if_t intf_array_received[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];

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
     * This table holds ECMP create API information to create each one of the three tables (see dbal_enum_value_field_ecmp_consistent_table_e)
     */
    diag_dnx_ecmp_relevant_fields_t ecmp_info[DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES] = {
        {BCM_L3_WITH_ID, 33, 12, 0, 6},
        {BCM_L3_WITH_ID, 44, 80, 0, 25},
        {BCM_L3_WITH_ID, 55, 80, BCM_L3_ECMP_LARGE_TABLE, 25}
    };

    int table_iter;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_l3_egress_t egr_intf;
    bcm_if_t intf_array_sent[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int intf_iter;
    int test_type;
    uint32 base_fec, fec_range;
    bcm_if_t intf_id = 0x40001002;
    int intf_count;
    int fec_id;
    uint32 fec_flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, 0, &base_fec, &fec_range));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP CONSISTENT HASHING SEMANTIC TEST START.\n")));

    for (intf_iter = 0; intf_iter < DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE; intf_iter++)
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
        for (table_iter = DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE;
             table_iter < DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES; table_iter++)
        {

            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "ECMP CONSISTENT HASHING SEMANTIC FOR %s.\n"),
                      ecmp_const_sem_table_types_description[table_iter]));

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
                    ecmp.max_paths =
                        dnx_data_l3.ecmp.consistent_tables_info_get(unit, table_iter)->max_nof_unique_members;
                    intf_count = dnx_data_l3.ecmp.consistent_tables_info_get(unit, table_iter)->max_nof_unique_members;
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
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
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
    uint32 ecmp_ids[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE * 6];
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
    for (intf_iter = 0; intf_iter < DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE; intf_iter++)
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
        for (intf_iter = 0; intf_iter < DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE - intf_count; intf_iter++, idx++)
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
        for (intf_iter = 0; intf_iter < DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE; intf_iter++, idx++)
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
    for (intf_iter = 0; intf_iter < DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE; intf_iter++)
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
    /** Skip comparing with average expected time for adapter execution. */
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
    dbal_enum_value_field_ecmp_consistent_table_e table_size_iter;
    int ecmp_group_size_per_table_size[DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES] = { 16, 256, 256 };
    int intf_array[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    int intf_array_get[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
    uint8 found[DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE];
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

    for (intf_iter = 0; intf_iter < DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE; intf_iter++)
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
    out_of_bound_fec_id = base_fec + 4 * DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE;
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
                               (unit, &ecmp_get, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, intf_array_get,
                                &interfac_count_get), _SHR_E_PARAM);
    /*
     * Successfully get the ECMP group ;
     */
    ecmp_get.tunnel_priority.index = 0;
    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_get
                    (unit, &ecmp_get, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, intf_array_get, &interfac_count_get));
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
    for (table_size_iter = DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE;
         table_size_iter < DBAL_NOF_ENUM_ECMP_CONSISTENT_TABLE_VALUES; table_size_iter++)
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
            ecmp.ecmp_group_flags =
                (table_size_iter == DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_LARGE_SIZE) ? BCM_L3_ECMP_LARGE_TABLE : 0;
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
                                (unit, &ecmp_get, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, intf_array_get,
                                 &interfac_count_get));

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
                                (unit, &ecmp_get, DNX_DATA_MAX_L3_ECMP_MAX_GROUP_SIZE, intf_array_get,
                                 &interfac_count_get));

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

/**
 * \brief
 *    Configures a basic route
 * \param [in]
 *    unit- Relevant unit
 * \param [out]
 *    encap_id- The encoded encapsulation ID that is used in creating the ARP
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static inline shr_error_e
sh_dnx_ecmp_config_basic_route(
    int unit,
    int *encap_id)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t ingress_rif;
    bcm_l3_egress_t l3eg;
    int intf_in = 15;
    int intf_out = CTEST_DNX_L3_ECMP_CONFIG_OUT_RIF;
    int vrf = 1;

    int out_port_iter = 0;

    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */

    SHR_FUNC_INIT_VARS(unit);

    /** Set In-Port to In ETh-RIF*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = CTEST_DNX_L3_ECMP_IN_PORT;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = intf_in;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_gport_add(unit, intf_in, CTEST_DNX_L3_ECMP_IN_PORT, 0),
                        "Test failed, bcm_vlan_gport_add failed.\n");

    /** Set Out-Port default properties, in case of ARP+AC no need*/
    bcm_vlan_port_t_init(&vlan_port);
    bcm_port_match_info_t_init(&match_info);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    SHR_CLI_EXIT_IF_ERR(bcm_vlan_port_create(unit, &vlan_port), "Test failed, bcm_vlan_port_create failed.\n");

    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    for (out_port_iter = 0; out_port_iter < CTEST_DNX_L3_ECMP_NOF_OUT_PORTS; out_port_iter++)
    {
        match_info.port = out_ports[out_port_iter];
        SHR_CLI_EXIT_IF_ERR(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info),
                            "Test failed, bcm_port_match_add failed.\n");
    }

    /*
     * Create ETH-RIF and set its properties
     * Initialize a bcm_l3_intf_t structure.
     */
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_in_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_in;
    l3if.l3a_vid = intf_in;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed, bcm_l3_intf_create failed.\n");

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, intf_out_mac_address, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = intf_out;
    l3if.l3a_vid = intf_out;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_intf_create(unit, &l3if), "Test failed, bcm_l3_intf_create failed.\n");

    /** Set Incoming ETH-RIF properties*/
    bcm_l3_ingress_t_init(&ingress_rif);
    ingress_rif.flags = BCM_L3_INGRESS_WITH_ID;
    ingress_rif.vrf = vrf;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_ingress_create(unit, &ingress_rif, &intf_in),
                        "Test failed, bcm_l3_ingress_create failed.\n");

    /** Create ARP and set its properties*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, arp_next_hop_mac, sizeof(bcm_mac_t));
    l3eg.encap_id = CTEST_DNX_L3_ECMP_CONFIG_ENCAP_ID;
    l3eg.vlan = intf_out;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, NULL),
                        "Test failed, bcm_l3_egress_create failed.\n");
    /** save encoded encapsulation ID */
    *encap_id = l3eg.encap_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Configures a basic packet minus a SIP address
 * \param [in]
 *    unit- Relevant unit
 * \param [out]
 *    packet_tx- The packet configuration
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static inline shr_error_e
sh_dnx_ecmp_config_packet(
    int unit,
    rhhandle_t * packet_tx)
{
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info_in_port;
    uint32 flags;
    char *smac = "00:00:07:00:01:00";
    char *dmac = "00:0c:00:02:00:00";
    char dip[RHNAME_MAX_SIZE] = "127.255.255.2";
    uint32 in_ttl = 80;

    SHR_FUNC_INIT_VARS(unit);

    /** Allocate and init packet_tx.*/
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, packet_tx));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, CTEST_DNX_L3_ECMP_IN_PORT, &flags, &interface_info, &mapping_info_in_port));

    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, *packet_tx, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                    (unit, *packet_tx, "PTCH_2.PP_SSP", &mapping_info_in_port.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, *packet_tx, "ETH0"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, *packet_tx, "ETH0.DA", dmac));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, *packet_tx, "ETH0.SA", smac));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, *packet_tx, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, *packet_tx, "IPv4.DIP", dip));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, *packet_tx, "IPv4.TTL", &in_ttl, 8));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    This function configure the fields required for creating FEC during the pp vis fer fwd diagnostics test
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    hierarchy- The hierarchy of the FEC
 * \param [in]
 *    destination- the destinations of the FEC
 * \param [in]
 *    flags- The flags for the FEC creation
 * \param [in]
 *    encap_id- The encoded encapsulation ID that is used in creating the ARP,
 *     and will be held in the FEC(depended on the hierarchy)
 * \param [out]
 *    l3eg - A pointer to config using the other parameters
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static inline void
sh_dnx_ecmp_config_fec(
    int unit,
    int hierarchy,
    int destination,
    uint32 flags,
    int encap_id,
    bcm_l3_egress_t * l3eg)
{
    bcm_l3_egress_t_init(l3eg);
    if (hierarchy == 0)
    {
        l3eg->intf = CTEST_DNX_L3_ECMP_CONFIG_OUT_RIF;
    }
    else if (hierarchy == 1)
    {
        l3eg->intf = encap_id;
    }
    l3eg->port = destination;
    l3eg->flags = flags;
}
/**
 * \brief
 *    This function creates one hierarchy ECMP
 *    The hierarchy that this function generates is of the following structure:
 *    N = group_size-1
 *
 *                                  +--> FEC_0(hierarchy <hierarchy>) --> destination_array[0]
 *                                  |
 *                                  +--> FEC_1(hierarchy <hierarchy>) --> destination_array[1]
 * ECMP (hierarchy <hierarchy>) --> |        .
 *                                  +-->     .
 *                                  |        .
 *                                  +--> FEC_N(hierarchy <hierarchy>) --> destination_array[N]
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    hierarchy- The hierarchy of the ECMP (and FECs) we want to create
 * \param [in]
 *    ecmp_group_size- The ECMP group size (nof FECs that are attached to the ECMP)
 * \param [in]
 *    destenation_array- An array that holds the destinations for the FECs that the function generates
 *    has to be of size ecmp_group_size
 * \param [in]
 *    encap_id- The encoded encapsulation ID that be held in the FEC(depended on the hierarchy)
 * \param [in,out]
 *    ecmp - A pointer to a partially configured ECMP (only mode relevant fields are configured)
 *    The rest of the relevant ECMP fields are filled before allocating
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static inline shr_error_e
sh_dnx_ecmp_create_one_hierarchy_ecmp(
    int unit,
    int hierarchy,
    int ecmp_group_size,
    int *destenation_array,
    int encap_id,
    bcm_l3_egress_ecmp_t * ecmp)
{
    int hierarchy_array[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] = { BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY,
        BCM_SWITCH_FEC_PROPERTY_2ND_HIERARCHY, BCM_SWITCH_FEC_PROPERTY_3RD_HIERARCHY
    };
    int fec_iter;
    int fec_number;
    uint32 hier_flags[DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES] =
        { 0, BCM_L3_2ND_HIERARCHY, BCM_L3_3RD_HIERARCHY };
    int ecmp_intf_start = 2000;
    bcm_l3_egress_t l3eg;
    bcm_switch_fec_property_config_t fec_config;
    bcm_if_t fec_array[CTEST_DNX_L3_ECMP_LARGE_GROUP_SIZE];
    /** ECMPs are divided into banks of size 2048, each bank has to have the same hierarchy */
    int ecmp_start_temp = ecmp_intf_start + (hierarchy * dnx_data_l3.ecmp.nof_ecmp_per_bank_of_low_ecmps_get(unit));
    SHR_FUNC_INIT_VARS(unit);

    fec_config.flags = hierarchy_array[hierarchy];
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    /** Creating group size number of FECs for the ECMP, using the destination array */
    for (fec_iter = 0; fec_iter < ecmp_group_size; fec_iter++)
    {
        fec_number = fec_config.start + fec_iter;
        sh_dnx_ecmp_config_fec(unit, hierarchy, destenation_array[fec_iter], hier_flags[hierarchy], encap_id, &l3eg);
        SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &fec_number));

        fec_array[fec_iter] = fec_number;
    }

    ecmp->ecmp_intf = ecmp_start_temp;
    ecmp->flags = BCM_L3_WITH_ID | hier_flags[hierarchy];
    ecmp->max_paths = ecmp_group_size;

    SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create(unit, ecmp, ecmp_group_size, fec_array));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *    This function creates a three hierarchies ECMP intended for the diagnostic test - sh_dnx_ecmp_diag_vis_fer_cmd
 *    The hierarchies that this function generates are of the following structure:
 *    N = group_size-1
 *
 *               +-->FEC_0(1st hier)+-->|                  +-->FEC_0(2nd hier)+-->|                  +-->FEC_0(3rd hier)+-->out_port
 *               |        .             |                  |        .             |                  |        .
 *               +-->     .             |                  +-->     .             |                  +-->     .
 * ECMP(1st hier)|        .             |--> ECMP(2nd hier)|        .             |--> ECMP(3rd hier)|
 *               +-->     .             |                  +-->     .             |                  +-->     .
 *               |        .             |                  |        .             |                  |        .
 *               +-->FEC_N(1st hier)+-->|                  +-->FEC_N(2nd hier)+-->|                  +-->FEC_N(3rd hier)+-->out_port
 *
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    group_size- The ECMP group size (nof FECs that are attached to the ECMP)
 * \param [in]
 *    encap_id- The encoded encapsulation ID that be held in the FECs created by this function
 * \param [in,out]
 *    ecmp - A pointer to a partially configured ECMP (only mode relevant fields are configured)
 *    The rest of the relevant ecmp fields are filled for the first hierarchy ECMP
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static inline shr_error_e
sh_dnx_ecmp_create_three_hierarchies_ecmp(
    int unit,
    int group_size,
    int encap_id,
    bcm_l3_egress_ecmp_t * ecmp)
{
    int destination_array_iter;
    int hierarchy_iter = DNX_DATA_MAX_MDB_DIRECT_NOF_FEC_ECMP_HIERARCHIES - 1;
    bcm_gport_t gport_ecmp;

    /** The following arrays' size is determined to be the maximum size that is beefing used during the test */
    int destination_array[CTEST_DNX_L3_ECMP_LARGE_GROUP_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    /** Create third hierarchy */
    for (destination_array_iter = 0; destination_array_iter < group_size; destination_array_iter++)
    {
        /**
         * Evenly spread the out ports over the 3rd hierarchy FECs' destinations.
         * This is done in order to verify the chosen final destination against the final destination
         *  we get from the diagnostics.
         * This is mostly done for adapter verification
         *  (In the device the diagnostic uses signals that shows the chosen super-FECs that we can not use in adapter)
         */
        destination_array[destination_array_iter] = out_ports[destination_array_iter & 3];
    }
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_create_one_hierarchy_ecmp(unit, hierarchy_iter, group_size, destination_array,
                                                          0, ecmp));

    /** Create second hierarchy */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_ecmp, ecmp->ecmp_intf);
    for (destination_array_iter = 0; destination_array_iter < group_size; destination_array_iter++)
    {
        destination_array[destination_array_iter] = gport_ecmp;
    }
    hierarchy_iter--;
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_create_one_hierarchy_ecmp(unit, hierarchy_iter, group_size, destination_array,
                                                          encap_id, ecmp));

    /** Create first hierarchy */
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(gport_ecmp, ecmp->ecmp_intf);
    for (destination_array_iter = 0; destination_array_iter < group_size; destination_array_iter++)
    {
        destination_array[destination_array_iter] = gport_ecmp;
    }
    hierarchy_iter--;
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_create_one_hierarchy_ecmp(unit, hierarchy_iter, group_size, destination_array,
                                                          0, ecmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    This function tests the "PP visibility FER forward" diagnostic in
 *    a specific ECMP mode.
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    group_size- The ECMP group size (nof FECs that are attached to the ECMP)
 * \param [in]
 *    args- The args to pass to the diagnostic function
 * \param [in]
 *    sand_control- The sand_control to pass to the diagnostic function
 * \param [in]
 *    encap_id- The encoded encapsulation ID that be held in the FECs created later in the test
 * \param [in]
 *    ecmp - A pointer to a partially configured ECMP (only "ECMP mode" relevant fields are configured)
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    sh_dnx_pp_vis_fer_cmd
 */
static inline shr_error_e
sh_dnx_ecmp_test_diag(
    int unit,
    int group_size,
    args_t * args,
    sh_sand_control_t * sand_control,
    int encap_id,
    bcm_l3_egress_ecmp_t * ecmp)
{
    char sip[RHNAME_MAX_SIZE];
    rhhandle_t packet_tx = NULL;
    bcm_port_t in_port = CTEST_DNX_L3_ECMP_IN_PORT;
    bcm_l3_host_t host;
    uint32 host_addr = 0x7fffff02;
    int vrf = 1;
    int packet_iter;
    SHR_FUNC_INIT_VARS(unit);

    /** We start the journal in order to revert the allocations made during the test */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    /** We create a three hierarchies ECMP in order to test the most complex scenario */
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_create_three_hierarchies_ecmp(unit, group_size, encap_id, ecmp));

    /**Add host entry with the first hierarchy ECMP as the interface */
    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = host_addr;
    host.l3a_vrf = vrf;
    host.l3a_intf = ecmp->ecmp_intf;
    SHR_CLI_EXIT_IF_ERR(bcm_l3_host_add(unit, &host), "Test failed, bcm_l3_host_add failed.\n");

    /** config basic packet */
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_config_packet(unit, &packet_tx));

    /** We sand numerous packets with different SIP in order to generate different ECMP key for each packet */
    for (packet_iter = 0; packet_iter < CTEST_DNX_L3_ECMP_NOF_PACKETS; packet_iter++)
    {
        uint8 sip_suffix;

        /** New SIP */
        sip_suffix = dnx_l3_performance_random_get_64() & 255;
        sal_sprintf(sip, "%d.%d.%d.%d", 192, 128, packet_iter + 1, sip_suffix);

        /**
         * Add SIP to the packet
         * new SIP each time so that the lb key will give diverse results
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_tx, "IPv4.SIP", sip));
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_tx, SAND_PACKET_RX | SAND_PACKET_RESUME));
        SHR_IF_ERR_EXIT(diag_sand_rx_dump(unit, sand_control));

        /** Run diagnostic function with the current configuration */
        SHR_CLI_EXIT_IF_ERR(sh_dnx_pp_vis_fer_cmd(unit, args, sand_control), "test FAILED diagnostics failed");

    }

exit:
    /** We end the journal in order to revert the allocations made during the test */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    This test tests the diagnostic PP visibility FER forward.
 *    The test calls the diagnostic function numerous times in each "ECMP mode",
 *    and will fail if the diagnostic function does not generate the correct result,
 *    according to it's inner examination
 *
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    sh_dnx_pp_vis_fer_cmd
 */
shr_error_e
sh_dnx_ecmp_diag_vis_fer_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_l3_egress_ecmp_t ecmp;
    int encap_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnx_ecmp_config_basic_route(unit, &encap_id));

    /**
     * Each of the four ECMP modes below are determined by the dynamic mode, group size
     * and the flags as define for the ECMP
     */

    /** 1. multiply and divide mode */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_test_diag
                    (unit, CTEST_DNX_L3_ECMP_SMALL_GROUP_SIZE, args, sand_control, encap_id, &ecmp));

    /** 2. consistent table with small ECMP group size */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_test_diag
                    (unit, CTEST_DNX_L3_ECMP_SMALL_GROUP_SIZE, args, sand_control, encap_id, &ecmp));

    /** 3. consistent table with medium ECMP group size */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_test_diag
                    (unit, CTEST_DNX_L3_ECMP_LARGE_GROUP_SIZE, args, sand_control, encap_id, &ecmp));

    /** 4. consistent table with large ECMP group size */
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
    ecmp.ecmp_group_flags = BCM_L3_ECMP_LARGE_TABLE;
    SHR_IF_ERR_EXIT(sh_dnx_ecmp_test_diag
                    (unit, CTEST_DNX_L3_ECMP_LARGE_GROUP_SIZE, args, sand_control, encap_id, &ecmp));

    LOG_CLI(("\n\n===> test: PASSED!\n"));
exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_l3_ecmp_diag_options[] = {
    {"TEST_MODE", SAL_FIELD_TYPE_BOOL, "Fails the test in the case that the diagnostics fails", "Yes"},
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};

/* *INDENT-OFF* */
/**
 * \brief DNX ECMP Tests
 * List of ecmp ctests modules.
 */
sh_sand_cmd_t dnx_ecmp_test_cmds[] = {
    {"consistent", sh_dnx_ecmp_consistent_cmd, NULL, NULL, &sh_dnx_ecmp_consistent_man, NULL, NULL, CTEST_POSTCOMMIT | CTEST_UM},
    {"performance", sh_dnx_ecmp_performance_cmd, NULL, NULL, &sh_dnx_ecmp_performance_man, NULL, NULL, CTEST_POSTCOMMIT | CTEST_UM},
    {"tunnel_priority_sem", sh_dnx_ecmp_tunnel_priority_sem_cmd, NULL, NULL, &sh_dnx_ecmp_tunnel_priority_sem_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"ecmp_fer_fwd_diagnostics", sh_dnx_ecmp_diag_vis_fer_cmd, NULL, dnx_l3_ecmp_diag_options, &sh_dnx_ecmp_diag_vis_fer_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE

/** \file diag_dnx_ecmp_consistent.c
 * $Id$
 *
 * Tests for consistent ecmp
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
    bcm_if_t base_fec = 40961;
    bcm_if_t intf_id = 0x40001002;
    int intf_count;
    int fec_id;
    uint32 fec_flags = BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID;
    dnx_l3_ecmp_consistent_type_t const_table_info;
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ECMP CONSISTENT HASHING SEMANTIC TEST START.\n")));

    for (intf_iter = 0; intf_iter < L3_ECMP_MAX_NOF_ECMP_MEMBERS; intf_iter++)
    {
        fec_id = base_fec + intf_iter;
        bcm_l3_egress_t_init(&egr_intf);
        egr_intf.intf = intf_id;
        egr_intf.flags = BCM_L3_3RD_HIERARCHY;
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

/* *INDENT-OFF* */
/**
 * \brief DNX ECMP Tests
 * List of ecmp ctests modules.
 */
sh_sand_cmd_t dnx_ecmp_test_cmds[] = {
    {"consistent", sh_dnx_ecmp_consistent_cmd, NULL, NULL, &sh_dnx_ecmp_consistent_man, NULL, NULL, CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */

#undef BSL_LOG_MODULE

/** \file consistent_hashing_manager_ctest.c
 *
 * consistent hashing manager c-tests.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_UTILS

/**
* INCLUDE FILES:
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include "ctest_dnx_algo_hashing.h"
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
/**
 * }
 */

/**
 * DEFINES 
 * { 
 */

/** CHM create strings */
#define DNX_ALGO_CHM_UNIT_TEST_1 "chm unit testing one"
#define DNX_ALGO_CHM_UNIT_TEST_2 "chm unit testing two"
#define DNX_ALGO_CHM_UNIT_TEST_3 "chm unit testing three"
#define DNX_ALGO_CHM_UNIT_TEST_4 "chm unit testing four"
#define DNX_ALGO_CHM_UNIT_TEST_5 "chm unit testing five"
#define DNX_ALGO_CHM_UNIT_TEST_6 "chm unit testing six"

#define DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE 4096
#define DNX_CHM_DUMMY_HW_PROFILE_TO_OFFSET_TABLE 512
#define DNX_CHM_DUMMY_HW_UNIQUE_ID_TO_PROFILE_TABLE 1024
#define DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER 8

/** this is used in order not to use floats */
#define DNX_PERFORMANCE_MULTIPLIER 10000

/**
 * {
 */
sh_sand_option_t dnx_chm_test_options[] = {
    {NULL}
};

sh_sand_man_t dnx_chm_test_man = {
    "Unit Test for Consistent Hashing Manager",
    "Unit Test for Consistent Hashing Manager, tests all valid/invalud input values for each API, and runs scenario to check the overall API behaviour"
};

static int dummy_hw_offset_table[DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE] = { 0 };
static uint32 dummy_hw_profile_to_offset_table[DNX_CHM_DUMMY_HW_PROFILE_TO_OFFSET_TABLE] = { 0 };
static int dummy_hw_unique_id_to_profile_table[DNX_CHM_DUMMY_HW_UNIQUE_ID_TO_PROFILE_TABLE] = { 0 };

typedef enum dnx_chm_unittest_type_e
{
    DNX_CHM_UNITTEST_0,
    DNX_CHM_UNITTEST_1,
    DNX_CHM_UNITTEST_2,
    DNX_CHM_UNITTEST_3,
    DNX_CHM_UNITTEST_4,
    DNX_CHM_UNITTEST_5,
    DNX_CHM_UNITTEST_NOF_TYPES
} dnx_chm_unittest_type_t;

/**
 * Call Back function to get the needed resources per input
 * profile type. each profile type should return a power of 2
 * nof resources. the number of resources should represent the 
 * reduced number of resources the HW utilies. for example if 
 * you have a HW that uses 64, 256 and 512 HW entries per 
 * profile, and the table size is 4k, the prefered method to 
 * deliver the information is 1, 4, and 8 in accordence to above 
 * and during init the total number of resources should be 64 
 */
static shr_error_e
dnx_chm_unittest_nof_resources_per_profile_type_get_cb(
    int unit,
    int profile_type,
    uint32 *nof_resources)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((profile_type < DNX_CHM_UNITTEST_0) || (profile_type >= DNX_CHM_UNITTEST_NOF_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile type\n");
    }

    *nof_resources = 1 << profile_type;
exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to get the profile type from needed 
 * resources. input which is not a power of 2 should fail as 
 * invalid nof_resources.
 */
static shr_error_e
dnx_chm_unittest_profile_type_per_nof_resources_get_cb(
    int unit,
    uint32 nof_resources,
    int *profile_type)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((nof_resources == 0) ||                             /** no resources */
        (((nof_resources - 1) & nof_resources) != 0) ||     /** nof_resources is not a power of 2 */
        (nof_resources > (1 << DNX_CHM_UNITTEST_5)))        /** too many resources */
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nof resources\n");
    }

    *profile_type = utilex_log2_round_up(nof_resources);
exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to set a profile in the HW with its
 * corresponding ch_calendar.
 */
static shr_error_e
dnx_chm_unittest_calendar_set_cb(
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar)
{
    int first;
    int nof_entries_to_write;
    uint32 nof_resources;
    SHR_FUNC_INIT_VARS(unit);

    /** check profile type is valid */
    if ((calendar->profile_type < DNX_CHM_UNITTEST_0) || (calendar->profile_type >= DNX_CHM_UNITTEST_NOF_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile type\n");
    }

    /** deduce from profile type recieved in calendar how many
     *  entries to read from HW, deduce starting position from
     *  the profile offset and the profile type */
    first = DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER * profile_offset;
    SHR_IF_ERR_EXIT(dnx_chm_unittest_nof_resources_per_profile_type_get_cb
                    (unit, calendar->profile_type, &nof_resources));
    nof_entries_to_write = DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER * nof_resources;

    if ((first >= DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE) ||
        ((first + nof_entries_to_write - 1) >= DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "problem detected, attampt to overflow dummy HW array\n");
    }

    /** set calendar */
    for (int entry = 0; entry < nof_entries_to_write; ++entry)
    {
        dummy_hw_offset_table[first + entry] = calendar->lb_key_member_array[entry];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to set a ch_calendar with its 
 * corresponding profile in the HW. in this function the 
 * calendar is both in and out, the in direction is needed to 
 * input the profile's type 
 */
static shr_error_e
dnx_chm_unittest_calendar_get_cb(
    int unit,
    uint32 profile_offset,
    consistent_hashing_calendar_t * calendar)
{
    int first;
    int nof_entries_to_read;
    uint32 nof_resources;
    SHR_FUNC_INIT_VARS(unit);

    /** check profile type is valid */
    if ((calendar->profile_type < DNX_CHM_UNITTEST_0) || (calendar->profile_type >= DNX_CHM_UNITTEST_NOF_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile type\n");
    }

    /** deduce from profile type recieved in calendar how many
     *  entries to read from HW, deduce starting position from
     *  the profile offset and the profile type */
    first = DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER * profile_offset;
    SHR_IF_ERR_EXIT(dnx_chm_unittest_nof_resources_per_profile_type_get_cb
                    (unit, calendar->profile_type, &nof_resources));
    nof_entries_to_read = DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER * nof_resources;

    /** clear array */
    sal_memset(calendar->lb_key_member_array, 0, sizeof(calendar->lb_key_member_array));

    if ((first >= DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE) ||
        ((first + nof_entries_to_read - 1) >= DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "problem detected, attampt to overflow dummy HW array\n");
    }

    /** set calendar */
    for (int entry = 0; entry < nof_entries_to_read; ++entry)
    {
        calendar->lb_key_member_array[entry] = dummy_hw_offset_table[first + entry];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to move a profile from old offset to new 
 * offset in the HW, and to change profile mapping from old 
 * offset to new offset. 
 */
static shr_error_e
dnx_chm_unittest_profile_move_cb(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    int profile_type,
    uint32 nof_consecutive_proifles)
{
    uint32 table_old_offset;
    uint32 table_new_offset;
    uint32 nof_resources = 0;

    consistent_hashing_calendar_t calendar;
    SHR_FUNC_INIT_VARS(unit);

    /** get reasources per profile type */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_nof_resources_per_profile_type_get_cb(unit, profile_type, &nof_resources));

    /** take entries from old offset and copy to new offset, profile_type should indicate how many entries to copy */
    calendar.profile_type = profile_type;
    for (int profile_index = 0; profile_index < nof_consecutive_proifles; ++profile_index)
    {
        int added_offset = profile_index * nof_resources;
        SHR_IF_ERR_EXIT(dnx_chm_unittest_calendar_get_cb(unit, old_offset + added_offset, &calendar));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_calendar_set_cb(unit, new_offset + added_offset, &calendar));
    }

    /** go over profile to offset table, look for old offset and replace with new offset */
    table_old_offset = old_offset * DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER;
    table_new_offset = new_offset * DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER;
    for (int index = 0; index < DNX_CHM_DUMMY_HW_PROFILE_TO_OFFSET_TABLE; ++index)
    {
        if (dummy_hw_profile_to_offset_table[index] == table_old_offset)
        {
            dummy_hw_profile_to_offset_table[index] = table_new_offset;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Call Back function to assign a profile to an object 
 * identified by unique_identifyer. 
 */
static shr_error_e
dnx_chm_unittest_profile_assign_cb(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 profile_offset,
    void *user_info)
{
    int profile;
    SHR_FUNC_INIT_VARS(unit);
    /** get profile coresponding to unique_identifyer */
    profile = dummy_hw_unique_id_to_profile_table[unique_identifyer];
    /** set profile offset to profile */
    dummy_hw_profile_to_offset_table[profile] = profile_offset * DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER;

    SHR_FUNC_EXIT;
}

/**
 * Call Back function to get the profile offset from the
 * connected object unique_identifyer
 */
static shr_error_e
dnx_chm_unittest_profile_offset_get_cb(
    int unit,
    uint32 unique_identifyer,
    uint32 *profile_offset)
{
    int profile;
    SHR_FUNC_INIT_VARS(unit);

    profile = dummy_hw_unique_id_to_profile_table[unique_identifyer];
    *profile_offset = dummy_hw_profile_to_offset_table[profile] / DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER;

    SHR_FUNC_EXIT;
}

/**
 * Call Back function to get the number of entries in a calendar 
 * a certain profile type requires. this is usually different 
 * than number of resources.
 */
static shr_error_e
dnx_chm_unittest_calendar_entries_in_profile_get_cb(
    int unit,
    int profile_type,
    uint32 max_nof_members_in_profile,
    uint32 *nof_calendar_entries)
{
    uint32 nof_resources = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_chm_unittest_nof_resources_per_profile_type_get_cb(unit, profile_type, &nof_resources));
    *nof_calendar_entries = DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER * nof_resources;

exit:
    SHR_FUNC_EXIT;
}

/**
 * compare between 2 calendars and return number of chnages
 */
static shr_error_e
dnx_chm_unittest_calender_changes_compare(
    int unit,
    consistent_hashing_calendar_t * old_calendar,
    consistent_hashing_calendar_t * new_calendar,
    int *changes_counter)
{
    SHR_FUNC_INIT_VARS(unit);
    *changes_counter = 0;
    for (int i = 0; i < DNX_DATA_MAX_CONSISTENT_HASHING_CALENDAR_MAX_NOF_ENTRIES_IN_CALENDAR; ++i)
    {
        if (old_calendar->lb_key_member_array[i] != new_calendar->lb_key_member_array[i])
        {
            ++(*changes_counter);
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - add or remove a member from chm and perform a check 
 *        on that action to validate that changes to tables make
 *        snese
 * 
 * \param [in] unit - unit number
 * \param [in] chm_handle - handle to chm
 * \param [in] unique_identifyer - unique id of the element to 
 *        which you want to add/remove member from
 * \param [in] member_to_act_on - member to add/remove
 * \param [in] current_nof_members - current number of memebers 
 *        in profile
 * \param [in] profile_type - profile type
 * \param [in] action_is_add - indication if to add or remove
 * \param [in] max_nof_members_in_profile - max number of 
 *        members in profile
 * \param [out] performance_number - normilized performance 
 *        number for action - (number_of_actual_changes -
 *        best_case) * DNX_PERFORMANCE_MULTIPLIER / (worst_case
 *        - best_case). when adding first member to profile or
 *        when removing last member from profile this
 *        performance number is not relevant and 0 is returned
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_chm_unittest_add_remove_member_with_check(
    int unit,
    uint32 chm_handle,
    uint32 unique_identifyer,
    uint32 member_to_act_on,
    uint32 current_nof_members,
    int profile_type,
    int action_is_add,
    int max_nof_members_in_profile,
    int *performance_number)
{
    uint32 profile_offset;
    uint32 nof_calendar_entries;
    int changes_counter = 0;
    int expected_changes_worst_case;
    int expected_minimal_nof_actions;
    consistent_hashing_calendar_t old_calendar;
    consistent_hashing_calendar_t new_calendar;
    SHR_FUNC_INIT_VARS(unit);

    /** clear calendars */
    sal_memset(old_calendar.lb_key_member_array, 0, sizeof(old_calendar.lb_key_member_array));
    sal_memset(new_calendar.lb_key_member_array, 0, sizeof(new_calendar.lb_key_member_array));

    /** get profile offset */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_profile_offset_get_cb(unit, unique_identifyer, &profile_offset));
    /** set old calendar type */
    old_calendar.profile_type = profile_type;
    /** get old calendar */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_calendar_get_cb(unit, profile_offset, &old_calendar));
    /** add/remove member */
    if (action_is_add)
    {
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_add
                        (unit, chm_handle, unique_identifyer, 0, member_to_act_on, current_nof_members, NULL));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_remove
                        (unit, chm_handle, unique_identifyer, 0, member_to_act_on, current_nof_members, NULL));
    }
    /** get profile offset */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_profile_offset_get_cb(unit, unique_identifyer, &profile_offset));
    /** set new calendar type */
    new_calendar.profile_type = profile_type;
    /** get calendar */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_calendar_get_cb(unit, profile_offset, &new_calendar));
    /** compare calendar changes */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_calender_changes_compare(unit, &old_calendar, &new_calendar, &changes_counter));
    /** get entries in calendar for profile type */
    SHR_IF_ERR_EXIT(dnx_chm_unittest_calendar_entries_in_profile_get_cb(unit, profile_type, 0, &nof_calendar_entries));
    /** calculate number of entries that should've been changed */
    if (action_is_add)
    {
        /** in worst case scenario the amount of entries changed is:
         *  round_up((1/Nnew)*entries) + round_up((1/Ninitial)*entries) */
        expected_changes_worst_case = (nof_calendar_entries / (current_nof_members + 1)) + 1 +
            (nof_calendar_entries / max_nof_members_in_profile) + 1;
        expected_minimal_nof_actions = nof_calendar_entries / (current_nof_members + 1);
        sal_printf("ADD    :");
    }
    else
    {
        /** on the other hand, when removing a member, the expected
         *  nof changes is round_up((1/Ncurrent)*entries) */
        expected_changes_worst_case = (nof_calendar_entries / current_nof_members) + 1;
        expected_minimal_nof_actions = nof_calendar_entries / current_nof_members;
        sal_printf("REMOVE :");
    }
    sal_printf("member %d, changes_counter=%d, expected_changes_worst_case=%d, expected_minimal_nof_actions=%d\n",
               member_to_act_on, changes_counter, expected_changes_worst_case, expected_minimal_nof_actions);
    if (changes_counter > expected_changes_worst_case)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "performance was worse than worse case, something is wrong");
    }
    /** exclude from performance_number when either adding first
     *  member or when ewmoving last member, those cases are not
     *  relevant for statistics */
    if ((action_is_add) && (current_nof_members == 0))
    {
        *performance_number = 0;
    }
    else if ((!action_is_add) && (current_nof_members == 1))
    {
        *performance_number = 0;
    }
    else
    {
        *performance_number = ((changes_counter - expected_minimal_nof_actions) * DNX_PERFORMANCE_MULTIPLIER) /
            (expected_changes_worst_case - expected_minimal_nof_actions);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - returns 0/1 according to chosen option
 * 
 * \param [in] option_0_weight - weight of option 0
 * \param [in] option_1_weight - weight of option 1
 *   
 * \return
 *   int 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
static int
bianry_options_with_probability_choose(
    int option_0_weight,
    int option_1_weight)
{
    int total = option_0_weight + option_1_weight;
    return ((sal_rand() % total) < option_0_weight) ? 0 : 1;
}

/**
 * reset dummy HW
 */
static void
dnx_chm_unit_test_dummy_hw_reset(
    void)
{
    sal_memset(dummy_hw_offset_table, 0xff, sizeof(dummy_hw_offset_table));
    sal_memset(dummy_hw_profile_to_offset_table, 0xff, sizeof(dummy_hw_profile_to_offset_table));
    sal_memset(dummy_hw_unique_id_to_profile_table, 0xff, sizeof(dummy_hw_unique_id_to_profile_table));
}

/**
 * perform set of tests on consistent hashing manager
 * infrastructure
 */
shr_error_e
dnx_chm_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_resources_to_manage = DNX_CHM_DUMMY_HW_OFFSET_TABLE_SIZE / DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER;
    consistent_hashing_cbs_t cbs;
    uint32 chm_handle;
    int rv;
    int *members_in_profile_p = NULL;
    int *members_not_in_profile_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** allow sw state allocation during tests */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /** load CBs to structure */
    cbs.calendar_entries_in_profile_get = &dnx_chm_unittest_calendar_entries_in_profile_get_cb;
    cbs.calendar_get = &dnx_chm_unittest_calendar_get_cb;
    cbs.calendar_set = &dnx_chm_unittest_calendar_set_cb;
    cbs.nof_resources_per_profile_type_get = &dnx_chm_unittest_nof_resources_per_profile_type_get_cb;
    cbs.profile_assign = &dnx_chm_unittest_profile_assign_cb;
    cbs.profile_move = &dnx_chm_unittest_profile_move_cb;
    cbs.profile_offset_get = &dnx_chm_unittest_profile_offset_get_cb;
    cbs.profile_type_per_nof_resources_get = &dnx_chm_unittest_profile_type_per_nof_resources_get_cb;

    /*
     * Test 1 - simple sanity of create and destroy
     * 1. create 
     * 2. destroy 
     */
    {
        dnx_chm_unit_test_dummy_hw_reset();

        /*
         * use err recovery to revert system state after test
         */
        DNX_ROLLBACK_JOURNAL_START(unit);

        /** 1. create  */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                        (unit, nof_resources_to_manage, DNX_ALGO_CHM_UNIT_TEST_1, &cbs, &chm_handle));
        /** 2. destroy */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, chm_handle));

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
    }

    /*
     * Test 2 - alloc/free with full profiles
     *  1. create,
     *  2. allocate full profiles of different sizes,
     *  3. reach a state of out of resources,
     *  4. try to allocate when full - expect failure
     *  5. try to allocate a profile that is expected to use existing profile - reuse 256 existing entries in table
     *  6. free profile with replication and try to allocate a new profile, expect success on freeing and failure on
     *          allocate - because of replications profile's resources are not realy freed
     *  7. free profiles,
     *  8. Destroy
     */
    {
        dnx_chm_unit_test_dummy_hw_reset();

        DNX_ROLLBACK_JOURNAL_START(unit);

        /** 1. create  */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                        (unit, nof_resources_to_manage, DNX_ALGO_CHM_UNIT_TEST_2, &cbs, &chm_handle));
        /** 2. allocate */
        dummy_hw_unique_id_to_profile_table[0] = 0;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 0, DNX_CHM_UNITTEST_0, 1, 0, 2, 2, NULL, NULL, NULL));
                                                                                /** 8 entries in table */
        dummy_hw_unique_id_to_profile_table[1] = 1;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 1, DNX_CHM_UNITTEST_1, 1, 0, 5, 5, NULL, NULL, NULL));
                                                                                /** 16 entries in table */
        dummy_hw_unique_id_to_profile_table[2] = 2;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 2, DNX_CHM_UNITTEST_2, 1, 0, 6, 6, NULL, NULL, NULL));
                                                                                /** 32 entries in table */
        dummy_hw_unique_id_to_profile_table[3] = 3;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 3, DNX_CHM_UNITTEST_3, 1, 0, 8, 8, NULL, NULL, NULL));
                                                                                /** 64 entries in table */
        dummy_hw_unique_id_to_profile_table[4] = 4;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 4, DNX_CHM_UNITTEST_4, 1, 0, 20, 20, NULL, NULL, NULL));
                                                                                  /** 128 entries in table */
        dummy_hw_unique_id_to_profile_table[5] = 5;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 5, DNX_CHM_UNITTEST_5, 1, 0, 30, 30, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[6] = 6;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 6, DNX_CHM_UNITTEST_5, 1, 0, 31, 31, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[7] = 7;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 7, DNX_CHM_UNITTEST_5, 1, 0, 32, 32, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[8] = 8;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 8, DNX_CHM_UNITTEST_5, 1, 0, 33, 33, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[9] = 9;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 9, DNX_CHM_UNITTEST_5, 1, 0, 34, 34, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[10] = 10;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 10, DNX_CHM_UNITTEST_5, 1, 0, 35, 35, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[11] = 11;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 11, DNX_CHM_UNITTEST_5, 1, 0, 36, 36, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[12] = 12;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 12, DNX_CHM_UNITTEST_5, 1, 0, 37, 37, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[13] = 13;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 13, DNX_CHM_UNITTEST_5, 1, 0, 38, 38, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[14] = 14;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 14, DNX_CHM_UNITTEST_5, 1, 0, 39, 39, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[15] = 15;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 15, DNX_CHM_UNITTEST_5, 1, 0, 40, 40, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[16] = 16;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 16, DNX_CHM_UNITTEST_5, 1, 0, 41, 41, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[17] = 17;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 17, DNX_CHM_UNITTEST_5, 1, 0, 42, 42, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 18, DNX_CHM_UNITTEST_5, 1, 0, 43, 43, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[19] = 19;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 19, DNX_CHM_UNITTEST_5, 1, 0, 44, 44, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        /** 3,4. try to allocate when full - expect failure */
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 45, 45,
                                                               NULL, NULL, NULL);
                                                                                  /** 256 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "attampt to allocated when full, was expecting failure but recieved success\n");
        }
        /** 5. try to allocate a profile that is expected to use
         *  existing profile - reuse 256 existing entries in table */
        dummy_hw_unique_id_to_profile_table[20] = 20;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 38, 38, NULL, NULL, NULL));
        /** 6. free profile with replication and try to allocate a new
         *  profile, expect success on freeing and failure on
         *  allocate - because of replications profile's resources
         *  are not realy freed */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 20, DNX_CHM_UNITTEST_5));
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 45, 45,
                                                               NULL, NULL, NULL);
                                                                                  /** 256 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "attampt to allocated when full, was expecting failure but recieved success\n");
        }
        /** 7. free all other profiles */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 19, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 18, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 17, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 16, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 15, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 14, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 13, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 12, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 11, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 10, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 9, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 8, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 7, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 6, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 5, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 4, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 3, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 2, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 1, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 0, DNX_CHM_UNITTEST_5));

        /** 8. destroy */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, chm_handle));

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
    }

    /*
     * Test 3 - alloc/free with partial profiles and specific partial profiles
     *  0. create
     *  1. try illegal allocation - current > max - expect failure
     *  2. alloc partial profile
     *  3. alloc partial profile with specific members
     *  4. make allocations until out of place, try to allocate partials which are similar
     *  5. allocate partial that similar to partial that was already allocated - expect failure
     *  6. allocate partial with specific members similar to partial that was already allocated - expect failure
     *  7. free all
     *  8. destroy 
     */
    {
        uint32 specific_member_array_0[] = { 0, 4, 2 };

        dnx_chm_unit_test_dummy_hw_reset();

        DNX_ROLLBACK_JOURNAL_START(unit);

        /** 0.create */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                        (unit, nof_resources_to_manage, DNX_ALGO_CHM_UNIT_TEST_3, &cbs, &chm_handle));
        /** 1.try illigeal allocation - current > max - expect failure */
        dummy_hw_unique_id_to_profile_table[0] = 0;
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 0, DNX_CHM_UNITTEST_0, 1, 0, 2, 4,
                                                               NULL, NULL, NULL);
                                                                               /** 8 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt illegal allocation, was expecting failure but received success\n");
        }
        /** 2.alloc partial profile */
        dummy_hw_unique_id_to_profile_table[0] = 0;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 0, DNX_CHM_UNITTEST_0, 1, 0, 4, 2, NULL, NULL, NULL));
                                                                                /** 8 entries in table */
        /** 3.alloc partial profile with specific members */
        dummy_hw_unique_id_to_profile_table[1] = 1;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 1, DNX_CHM_UNITTEST_1, 1, 0, 5, 3, specific_member_array_0, NULL, NULL));
                                                    /** 16 entries in table */

        /** 4.make allocations until out of place, try to allocate
         *  partials which are similar */
        dummy_hw_unique_id_to_profile_table[2] = 2;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 2, DNX_CHM_UNITTEST_2, 1, 0, 6, 6, NULL, NULL, NULL));
                                                                                /** 32 entries in table */
        dummy_hw_unique_id_to_profile_table[3] = 3;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 3, DNX_CHM_UNITTEST_3, 1, 0, 8, 8, NULL, NULL, NULL));
                                                                                /** 64 entries in table */
        dummy_hw_unique_id_to_profile_table[4] = 4;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 4, DNX_CHM_UNITTEST_4, 1, 0, 20, 20, NULL, NULL, NULL));
                                                                                  /** 128 entries in table */
        dummy_hw_unique_id_to_profile_table[5] = 5;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 5, DNX_CHM_UNITTEST_5, 1, 0, 30, 30, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[6] = 6;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 6, DNX_CHM_UNITTEST_5, 1, 0, 30, 25, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[7] = 7;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 7, DNX_CHM_UNITTEST_5, 1, 0, 30, 25, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[8] = 8;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 8, DNX_CHM_UNITTEST_5, 1, 0, 30, 25, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[9] = 9;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 9, DNX_CHM_UNITTEST_5, 1, 0, 30, 22, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[10] = 10;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 10, DNX_CHM_UNITTEST_5, 1, 0, 30, 22, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[11] = 11;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 11, DNX_CHM_UNITTEST_5, 1, 0, 30, 28, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[12] = 12;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 12, DNX_CHM_UNITTEST_5, 1, 0, 37, 30, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[13] = 13;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 13, DNX_CHM_UNITTEST_5, 1, 0, 35, 30, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[14] = 14;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 14, DNX_CHM_UNITTEST_5, 1, 0, 35, 32, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[15] = 15;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 15, DNX_CHM_UNITTEST_5, 1, 0, 35, 22, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[16] = 16;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 16, DNX_CHM_UNITTEST_5, 1, 0, 35, 22, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[17] = 17;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 17, DNX_CHM_UNITTEST_5, 1, 0, 35, 22, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 18, DNX_CHM_UNITTEST_5, 1, 0, 35, 22, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[19] = 19;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 19, DNX_CHM_UNITTEST_5, 1, 0, 35, 3, specific_member_array_0, NULL, NULL));
                                                       /** 256 entries in table */
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 45, 45,
                                                               NULL, NULL, NULL);
                                                                                  /** 256 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** 5.allocate partial that similar to partial that was already allocated - expect failure */
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 35, 22,
                                                               NULL, NULL, NULL);
                                                                                  /** 256 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** 6.allocate partial with specific members similar to partial that was already allocated - expect failure */
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 35, 3,
                                                               specific_member_array_0, NULL, NULL);
                                                                                         /** 256 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** 7.free all */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 19, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 18, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 17, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 16, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 15, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 14, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 13, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 12, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 11, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 10, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 9, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 8, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 7, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 6, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 5, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 4, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 3, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 2, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 1, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 0, DNX_CHM_UNITTEST_5));
        /** 8.destroy */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, chm_handle));

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
    }

    /*
     * Test 4 - add/remove members
     * 1. create 
     * 2. add full member till full 
     * 3. remove member from profile - expect failure 
     * 4. free profile 
     * 5. add replication for profile 
     * 6. remove member from profile 
     * 7. remove few more members from profile 
     * 8. add few members to profile back 
     * 9. add replication to profile 
     * 10. free all 
     * 11. destroy
     */
    {
        int dummy;

        DNX_ROLLBACK_JOURNAL_START(unit);

        dnx_chm_unit_test_dummy_hw_reset();
        /** 1.create  */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                        (unit, nof_resources_to_manage, DNX_ALGO_CHM_UNIT_TEST_4, &cbs, &chm_handle));
        /** 2.add full member till full   */
        dummy_hw_unique_id_to_profile_table[0] = 0;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 0, DNX_CHM_UNITTEST_0, 1, 0, 2, 2, NULL, NULL, NULL));
                                                                                /** 8 entries in table */
        dummy_hw_unique_id_to_profile_table[1] = 1;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 1, DNX_CHM_UNITTEST_1, 1, 0, 5, 5, NULL, NULL, NULL));
                                                                                /** 16 entries in table */
        dummy_hw_unique_id_to_profile_table[2] = 2;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 2, DNX_CHM_UNITTEST_2, 1, 0, 6, 6, NULL, NULL, NULL));
                                                                                /** 32 entries in table */
        dummy_hw_unique_id_to_profile_table[3] = 3;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 3, DNX_CHM_UNITTEST_3, 1, 0, 8, 8, NULL, NULL, NULL));
                                                                                /** 64 entries in table */
        dummy_hw_unique_id_to_profile_table[4] = 4;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 4, DNX_CHM_UNITTEST_4, 1, 0, 20, 20, NULL, NULL, NULL));
                                                                                  /** 128 entries in table */
        dummy_hw_unique_id_to_profile_table[5] = 5;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 5, DNX_CHM_UNITTEST_5, 1, 0, 30, 30, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[6] = 6;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 6, DNX_CHM_UNITTEST_5, 1, 0, 31, 31, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[7] = 7;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 7, DNX_CHM_UNITTEST_5, 1, 0, 32, 32, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[8] = 8;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 8, DNX_CHM_UNITTEST_5, 1, 0, 33, 33, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[9] = 9;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 9, DNX_CHM_UNITTEST_5, 1, 0, 34, 34, NULL, NULL, NULL));
                                                                                  /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[10] = 10;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 10, DNX_CHM_UNITTEST_5, 1, 0, 35, 35, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[11] = 11;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 11, DNX_CHM_UNITTEST_5, 1, 0, 36, 36, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[12] = 12;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 12, DNX_CHM_UNITTEST_5, 1, 0, 37, 37, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[13] = 13;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 13, DNX_CHM_UNITTEST_5, 1, 0, 38, 38, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[14] = 14;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 14, DNX_CHM_UNITTEST_5, 1, 0, 39, 39, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[15] = 15;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 15, DNX_CHM_UNITTEST_5, 1, 0, 40, 40, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[16] = 16;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 16, DNX_CHM_UNITTEST_5, 1, 0, 41, 41, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[17] = 17;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 17, DNX_CHM_UNITTEST_5, 1, 0, 42, 42, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 18, DNX_CHM_UNITTEST_5, 1, 0, 43, 43, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        dummy_hw_unique_id_to_profile_table[19] = 19;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 19, DNX_CHM_UNITTEST_5, 1, 0, 44, 44, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 45, 45,
                                                               NULL, NULL, NULL);
                                                                                  /** 256 entries in table */
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** 3.remove member from profile - expect failure */
        rv = dnx_algo_consistent_hashing_manager_profile_member_remove(unit, chm_handle, 15, 0, 12, 40, NULL);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "removing member from profile requires creating a new profile,"
                         " shouldn't be possible when full, but was successful anyways\n");
        }
        /** 4.free profile */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 8, DNX_CHM_UNITTEST_5));
        /** 5.add replication for profile */
        dummy_hw_unique_id_to_profile_table[20] = 20;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 20, DNX_CHM_UNITTEST_5, 1, 0, 40, 40, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        /** 6.remove member from profile */
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 12, 40, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        /** 7.remove few more members from profile */
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 13, 39, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 16, 38, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 23, 37, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 39, 36, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 1, 35, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 7, 34, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 8, 33, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 3, 32, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 28, 31, DNX_CHM_UNITTEST_5, 0, 40, &dummy));
        /** 8.add few members to profile back */
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 7, 30, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 12, 31, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 39, 32, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 3, 33, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 8, 34, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 28, 35, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 13, 36, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 16, 37, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 1, 38, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                        (unit, chm_handle, 20, 23, 39, DNX_CHM_UNITTEST_5, 1, 40, &dummy));
        /** 9.add replication to profile */
        dummy_hw_unique_id_to_profile_table[21] = 21;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 21, DNX_CHM_UNITTEST_5, 1, 0, 40, 40, NULL, NULL, NULL));
                                                                                   /** 256 entries in table */
        /** 10.free all */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 20, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 21, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 19, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 18, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 17, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 16, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 15, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 14, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 13, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 12, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 11, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 10, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 9, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 7, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 6, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 5, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 4, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 3, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 2, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 1, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 0, DNX_CHM_UNITTEST_5));
        /** 11.destroy */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, chm_handle));

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
    }

    /*
     * Test 5 - randomly add and remove members from profile, gather statistics as well on actions
     */
    {
        int nof_iterations = 100;
        sal_srand(0xdeadbeef);

        dnx_chm_unit_test_dummy_hw_reset();

        DNX_ROLLBACK_JOURNAL_START(unit);

        /** create  */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                        (unit, nof_resources_to_manage, DNX_ALGO_CHM_UNIT_TEST_5, &cbs, &chm_handle));
        /** repeat nof_iteration with different probability */
        for (int i = 0; i < nof_iterations; ++i)
        {
            /** get random parameters */
            int add_probability = (sal_rand() % 100) + 1;
            int remove_probability = (sal_rand() % 100) + 1;
            int nof_actions = (sal_rand() % 100) + 10;
            int max_nof_members_in_profile = (sal_rand() % 40) + 2;
            int unique_id = sal_rand() % 512;
            int profile_type;
            int smallest_profile_type;
            int current_nof_members_in_profile = 0;

            int add_actions = 0;
            int remove_actions = 0;
            int performance_number;
            int average_remove_performance_number = 0;
            int average_add_performance_number = 0;

            sal_printf("\niteration %d started\n", i);
            SHR_ALLOC(members_in_profile_p, sizeof(*members_in_profile_p) * max_nof_members_in_profile,
                      "members_in_profile_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC(members_not_in_profile_p, sizeof(*members_not_in_profile_p) * max_nof_members_in_profile,
                      "members_not_in_profile_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

            /** possible profile type is one that is big enough to
             *  contain max nof entries */
            smallest_profile_type = utilex_log2_round_up
                (UTILEX_DIV_ROUND_UP(max_nof_members_in_profile, DNX_CHM_HW_ENTRIES_TO_RESOURCES_MULTIPLIER));
            profile_type = smallest_profile_type + sal_rand() % (DNX_CHM_UNITTEST_NOF_TYPES - smallest_profile_type);
            sal_printf("profile_type = %d, max_nof_members_in_profile=%d\n", profile_type, max_nof_members_in_profile);
            /** alloc profile of random size */
            dummy_hw_unique_id_to_profile_table[unique_id] = unique_id;
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                            (unit, chm_handle, unique_id, profile_type, 1, 0, max_nof_members_in_profile,
                             max_nof_members_in_profile, NULL, NULL, NULL));
            /** initialize management arrays */
            for (int j = 0; j < max_nof_members_in_profile; ++j)
            {
                members_in_profile_p[j] = j;
            }
            current_nof_members_in_profile = max_nof_members_in_profile;

            for (int j = 0; j < nof_actions; ++j)
            {
                int action_is_add = 0;
                uint32 member_to_act_on;

                /** decide if to add or remove */
                if (current_nof_members_in_profile == max_nof_members_in_profile)
                {
                    action_is_add = 0;
                }
                else if (current_nof_members_in_profile == 0)
                {
                    action_is_add = 1;
                }
                else
                {
                    action_is_add = bianry_options_with_probability_choose(add_probability, remove_probability);
                }

                /** find a member to use */
                if (action_is_add)
                {
                    /** pick random member to add from members not in profile */
                    int current_nof_members_not_in_profile =
                        max_nof_members_in_profile - current_nof_members_in_profile;
                    int member_index = sal_rand() % (current_nof_members_not_in_profile);
                    member_to_act_on = members_not_in_profile_p[member_index];
                    /** align management arrays */
                    members_not_in_profile_p[member_index] =
                        members_not_in_profile_p[current_nof_members_not_in_profile - 1];
                    members_in_profile_p[current_nof_members_in_profile] = member_to_act_on;
                }
                else
                {
                    /** pick random member to remove from members in profile */
                    int current_nof_members_not_in_profile =
                        max_nof_members_in_profile - current_nof_members_in_profile;
                    int member_index = sal_rand() % current_nof_members_in_profile;
                    member_to_act_on = members_in_profile_p[member_index];
                    /** align managment arrays */
                    members_in_profile_p[member_index] = members_in_profile_p[current_nof_members_in_profile - 1];
                    members_not_in_profile_p[current_nof_members_not_in_profile] = member_to_act_on;
                }

                /** add / remove and check action */
                SHR_IF_ERR_EXIT(dnx_chm_unittest_add_remove_member_with_check
                                (unit, chm_handle, unique_id, member_to_act_on, current_nof_members_in_profile,
                                 profile_type, action_is_add, max_nof_members_in_profile, &performance_number));

                /** increase / decrease counters */
                if (action_is_add)
                {
                    ++current_nof_members_in_profile;
                    ++add_actions;
                    average_add_performance_number += performance_number;
                }
                else
                {
                    --current_nof_members_in_profile;
                    ++remove_actions;
                    average_remove_performance_number += performance_number;
                }

            }
            /** calculate average performance number */
            sal_printf("average_add_performance_number = %d in a scale of 0 to %d\n",
                       average_add_performance_number / add_actions, DNX_PERFORMANCE_MULTIPLIER);
            sal_printf("average_remove_performance_number = %d in a scale of 0 to %d\n",
                       average_remove_performance_number / remove_actions, DNX_PERFORMANCE_MULTIPLIER);
            /** free profile */
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free
                            (unit, chm_handle, unique_id, profile_type));
            SHR_FREE(members_in_profile_p);
            SHR_FREE(members_not_in_profile_p);
            sal_printf("iteration %d ended\n", i);

        }

        /** destroy */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, chm_handle));

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
    }

    /*
     * Test 6 - allocation with consecutive profiles
     * 1. create
     * 2. allocate profiles, some of them with consecutive members until full
     * 3. try to allocate when full and expect failure
     * 4. free profile
     * 5. allocate in a state that should force defrag
     * 6. free all
     * 7. destroy
     */
    {
        DNX_ROLLBACK_JOURNAL_START(unit);

        dnx_chm_unit_test_dummy_hw_reset();
        /** create  */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_create
                        (unit, nof_resources_to_manage, DNX_ALGO_CHM_UNIT_TEST_6, &cbs, &chm_handle));

        /** add profiles with and without consecutive profiles till almost full */
        /** 8 entries, 8 new ones (total 8 / 4096) */
        dummy_hw_unique_id_to_profile_table[0] = 0;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 0, DNX_CHM_UNITTEST_0, 1, 0, 2, 2, NULL, NULL, NULL));
        /** 8 entries, 0 new ones (total 8 / 4096) */
        dummy_hw_unique_id_to_profile_table[1] = 1;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 1, DNX_CHM_UNITTEST_0, 1, 0, 2, 2, NULL, NULL, NULL));
        /** 8 entries, 0 new ones (total 8 / 4096) */
        dummy_hw_unique_id_to_profile_table[2] = 2;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 2, DNX_CHM_UNITTEST_0, 1, 0, 2, 2, NULL, NULL, NULL));
        /** 8 entries, 8 new ones (total 16 / 4096) */
        dummy_hw_unique_id_to_profile_table[3] = 3;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 3, DNX_CHM_UNITTEST_0, 1, 0, 2, 1, NULL, NULL, NULL));
        /** 16 entries, 16 new ones (total 32 / 4096) */
        dummy_hw_unique_id_to_profile_table[4] = 4;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 4, DNX_CHM_UNITTEST_0, 2, 0, 2, 2, NULL, NULL, NULL));
        /** 16 entries, 16 new ones (total 48 / 4096) */
        dummy_hw_unique_id_to_profile_table[5] = 5;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 5, DNX_CHM_UNITTEST_0, 2, 0, 2, 1, NULL, NULL, NULL));
        /** attempt to add an invalid amount */
        dummy_hw_unique_id_to_profile_table[6] = 6;
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 6, DNX_CHM_UNITTEST_0, 3, 1, 4, 4,
                                                               NULL, NULL, NULL);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Attempt to allocated not a power of 2 consecutive profiles, was expecting failure but received success\n");
        }
        /** 32 entries, 32 new ones (total 80 / 4096) */
        dummy_hw_unique_id_to_profile_table[6] = 6;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 6, DNX_CHM_UNITTEST_0, 4, 2, 2, 1, NULL, NULL, NULL));
        /** 64 entries, 64 new ones (total 144 / 4096) */
        dummy_hw_unique_id_to_profile_table[7] = 7;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 7, DNX_CHM_UNITTEST_0, 8, 5, 2, 1, NULL, NULL, NULL));
        /** 64 entries, 64 new ones (total 208 / 4096) */
        dummy_hw_unique_id_to_profile_table[8] = 8;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 8, DNX_CHM_UNITTEST_3, 1, 0, 30, 30, NULL, NULL, NULL));
        /** 64 entries, 0 new ones (total 208 / 4096) */
        dummy_hw_unique_id_to_profile_table[9] = 9;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 9, DNX_CHM_UNITTEST_3, 1, 0, 30, 30, NULL, NULL, NULL));
        /** 64 entries, 0 new ones (total 208 / 4096) */
        dummy_hw_unique_id_to_profile_table[10] = 10;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 10, DNX_CHM_UNITTEST_3, 1, 0, 30, 30, NULL, NULL, NULL));
        /** 64 entries, 64 new ones (total 272 / 4096) */
        dummy_hw_unique_id_to_profile_table[11] = 11;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 11, DNX_CHM_UNITTEST_3, 1, 0, 30, 29, NULL, NULL, NULL));
        /** 128 entries, 128 new ones (total 400 / 4096) */
        dummy_hw_unique_id_to_profile_table[12] = 12;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 12, DNX_CHM_UNITTEST_3, 2, 1, 30, 30, NULL, NULL, NULL));
        /** attempt to add an invalid amount */
        dummy_hw_unique_id_to_profile_table[13] = 13;
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 13, DNX_CHM_UNITTEST_3, 3, 1, 30, 30,
                                                               NULL, NULL, NULL);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Attempt to allocated not a power of 2 consecutive profiles, was expecting failure but received success\n");
        }
        /** 256 entries, 256 new ones (total 656 / 4096) */
        dummy_hw_unique_id_to_profile_table[13] = 13;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 13, DNX_CHM_UNITTEST_3, 4, 1, 30, 30, NULL, NULL, NULL));
        /** 1024 entries, 1024 new ones (total 1680 / 4096) */
        dummy_hw_unique_id_to_profile_table[14] = 14;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 14, DNX_CHM_UNITTEST_5, 4, 2, 50, 45, NULL, NULL, NULL));
        /** 2048 entries, 2048 new ones (total 3728 / 4096) */
        dummy_hw_unique_id_to_profile_table[15] = 15;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 15, DNX_CHM_UNITTEST_5, 8, 6, 50, 45, NULL, NULL, NULL));
        /** 256 entries, 256 new ones (total 3984 / 4096) */
        dummy_hw_unique_id_to_profile_table[16] = 16;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 16, DNX_CHM_UNITTEST_5, 1, 0, 50, 50, NULL, NULL, NULL));
        /** 256 entries, 0 new ones (total 3984 / 4096) */
        dummy_hw_unique_id_to_profile_table[17] = 17;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 17, DNX_CHM_UNITTEST_5, 1, 0, 50, 50, NULL, NULL, NULL));
        /** 128 entries, 128  new ones - expect failure */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 18, DNX_CHM_UNITTEST_4, 1, 0, 30, 30,
                                                               NULL, NULL, NULL);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** 128 entries, 128  new ones - expect failure */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 18, DNX_CHM_UNITTEST_2, 4, 2, 30, 30,
                                                               NULL, NULL, NULL);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** 128 entries, 128  new ones - expect failure */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        rv = dnx_algo_consistent_hashing_manager_profile_alloc(unit, chm_handle, 18, DNX_CHM_UNITTEST_3, 2, 1, 30, 30,
                                                               NULL, NULL, NULL);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Attempt to allocated when full, was expecting failure but received success\n");
        }
        /** clear 16 entries, (total 3968 / 4096) */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 4, DNX_CHM_UNITTEST_0));
        /** 128 entries, 128  new ones - (total 4096 / 4096), should force a defrag */
        dummy_hw_unique_id_to_profile_table[18] = 18;
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                        (unit, chm_handle, 18, DNX_CHM_UNITTEST_2, 4, 2, 30, 30, NULL, NULL, NULL));
        /** free all */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 0, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 1, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 2, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 3, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 5, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 6, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 7, DNX_CHM_UNITTEST_0));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 8, DNX_CHM_UNITTEST_3));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 9, DNX_CHM_UNITTEST_3));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 10, DNX_CHM_UNITTEST_3));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 11, DNX_CHM_UNITTEST_3));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 12, DNX_CHM_UNITTEST_3));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 13, DNX_CHM_UNITTEST_3));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 14, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 15, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 16, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 17, DNX_CHM_UNITTEST_5));
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free(unit, chm_handle, 18, DNX_CHM_UNITTEST_2));
        /** destroy */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_destroy(unit, chm_handle));

        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);
    }

    /** forbid sw state allocation during tests */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));

exit:
    SHR_FREE(members_in_profile_p);
    SHR_FREE(members_not_in_profile_p);
    SHR_FUNC_EXIT;
}

/**
 * }
 */

/** \file template_manager_unit_test.c
 *
 * Template management unit test.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

/**
* INCLUDE FILES:
* {
*/

#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm/types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <bcm_int/dnx/algo/template_mngr/template_mngr_internal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/example_temp_mngr_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include "ctest_dnx_algo_template.h"
/**
 * }
 */

/*
 * DEFINES
 */

/**
 * Test template names.
 * {
 */
#define TEMPLATE_TEST_BAD_VALUES                        "template_test_bad_values"
#define TEMPLATE_TEST_WITH_DEFAULT                      "template_test_with_default"
#define TEMPLATE_TEST_WITHOUT_DEFAULT                   "template_test_without_default"
#define TEMPLATE_TEST_PER_CORE                          "template_test_per_core"
#define TEMPLATE_TEST_WITH_DEFAULT_CHECK_VALUES         "template_with_default_check_values"
#define TEMPLATE_TEST_WITHOUT_DEFAULT_CHECK_VALUES      "template_test_without_default_check_values"
#define TEMPLATE_TEST_PER_CORE_CHECK_VALUES             "template_test_per_core_check_values"

/*
 * template parameters
 */
#define FLAGS_NONE 0
#define NOF_PROFILES 16
#define MAX_REFERENCES 32
#define FIRST_PROFILE 0
#define DEFAULT_PROFILE 0
#define DEFAULT_PROFILE_DATA_8 0xFF
#define DEFAULT_PROFILE_DATA_16 0x1122
#define DEFAULT_PROFILE_DATA_32 0x00112233
#define DEFAULT_PROFILE_DATA_INT 0xFFFF
#define MIN_NOF_CORES 2
#define FIRST_CORE_ID  0
#define SECOND_CORE_ID 1
#define NEGATIVE_PROFILE -1
#define NEGATIVE_NOF_PROFILES -1
#define NEGATIVE_MAX_REFERENCES -1
#define NEGATIVE_DEFAULT_PROFILE -1
#define NEGATIVE_DATA_SIZE -1

/*
 * structs to support bcm shell command
 */
/*
 * {
 */
sh_sand_option_t dnx_template_manager_test_options[] = {
    {NULL}
};

sh_sand_man_t dnx_template_manager_test_man = {
    "Unit Test for Template Manager",
    "Unit Test for Template Manager, tests all valid/invalud input values for each API, and runs scenario to check the overall API behaviour"
};

/*
 * }
 */



/**
 * \brief
 * check input valid/invalid values per template manager API
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *   NONE
 * \par INDIRECT OUTPUT
 *   the executed test
 */
static shr_error_e
dnx_algo_template_check_input_valid_values(
    int unit)
{
    int rv, profile, ref_count, new_profile, old_profile, current_profile;
    template_mngr_example_data_t profile_data, replace_profile_data, compare_data;
    uint8 is_last, first_reference;
    dnx_algo_template_create_data_t data, tmp_data;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    replace_profile_data.element_8 = 1;
    replace_profile_data.element_16 = 2;
    replace_profile_data.element_32 = 3;
    replace_profile_data.element_int = 4;

    compare_data.element_8 = 1;
    compare_data.element_16 = 1;
    compare_data.element_32 = 1;
    compare_data.element_int = 1;

    profile_data.element_8 = DEFAULT_PROFILE_DATA_8;
    profile_data.element_16 = DEFAULT_PROFILE_DATA_16;
    profile_data.element_32 = DEFAULT_PROFILE_DATA_32;
    profile_data.element_int = DEFAULT_PROFILE_DATA_INT;
    old_profile = 0;

    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data.first_profile = FIRST_PROFILE;
    data.nof_profiles = NOF_PROFILES;
    data.max_references = MAX_REFERENCES;
    data.default_profile = DEFAULT_PROFILE;
    data.data_size = sizeof(template_mngr_example_data_t);
    data.default_data = &profile_data;

    tmp_data = data;

    /***************************************
     * test dnx_algo_template_create
     **************************************/
    /*
     * {
     */
    /*
     * the templates which are created in this staged will be used by the rest of the APIs later
     */
    /*
     * dnx_algo_template_create with valid data, should pass
     */
    sal_strncpy(data.name, TEMPLATE_TEST_WITH_DEFAULT_CHECK_VALUES, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default_check_values.create(unit, &data, NULL));

    /*
     * dnx_algo_template_create with valid data (without_default), should pass
     */
    tmp_data.flags = FLAGS_NONE;
    sal_strncpy(tmp_data.name, TEMPLATE_TEST_WITHOUT_DEFAULT_CHECK_VALUES, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default_check_values.create(unit, &tmp_data, NULL));

    /*
     * dnx_algo_template_create with valid data (per core), should pass
     */
    tmp_data.flags = 0;
    sal_strncpy(tmp_data.name, TEMPLATE_TEST_PER_CORE_CHECK_VALUES, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core_check_values.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core_check_values.create(unit, core_id, &tmp_data, NULL));
    }

    tmp_data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    /*
     * dnx_algo_template_create with same name, should fail
     */

    rv = algo_temp_mngr_db.template_with_default_check_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "template already exists but creation was successful\n");
    }
    /*
     * dnx_algo_template_create with negative first_profile, should fail
     */
    tmp_data.first_profile = NEGATIVE_PROFILE;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative first_profile but creation was successful\n");
    }
    /*
     * dnx_algo_template_create with first_profile > default_profile, should fail
     */
    tmp_data.first_profile = tmp_data.nof_profiles - 1;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "first_profile > default_profile but creation was successful\n");
    }
    tmp_data.first_profile = FIRST_PROFILE;
    /*
     * dnx_algo_template_create with negative nof_profiles, should fail
     */
    tmp_data.nof_profiles = NEGATIVE_NOF_PROFILES;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative nof_profiles but creation was successful\n");
    }
    tmp_data.nof_profiles = NOF_PROFILES;
    /*
     * dnx_algo_template_create with negative max_references, should fail
     */
    tmp_data.max_references = NEGATIVE_MAX_REFERENCES;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative max_references but creation was successful\n");
    }
    tmp_data.max_references = MAX_REFERENCES;
    /*
     * dnx_algo_template_create with negative default_profile, should fail
     */
    tmp_data.default_profile = NEGATIVE_DEFAULT_PROFILE;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative default_profile but creation was successful\n");
    }
    /*
     * dnx_algo_template_create with default_profile = nof_profiles, should fail
     */
    tmp_data.default_profile = NOF_PROFILES;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "default_profile = nof_profiles but creation was successful\n");
    }
    tmp_data.default_profile = DEFAULT_PROFILE;
    /*
     * dnx_algo_template_create with negative data_size, should fail
     */
    tmp_data.data_size = NEGATIVE_DATA_SIZE;

    rv = algo_temp_mngr_db.template_test_bad_values.create(unit, &tmp_data, NULL);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative data_size but creation was successful\n");
    }
    tmp_data.data_size = sizeof(uint8);

    /*
     * }
     */
    /***************************************
     * test dnx_algo_template_allocate
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_allocate with valid values, should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default_check_values.allocate_single
                    (unit, FLAGS_NONE, &profile_data, NULL, &profile, &first_reference));
    /*
     * dnx_algo_template_allocate with valid values (per core), should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_test_per_core_check_values.allocate_single(unit, FIRST_CORE_ID, FLAGS_NONE, &profile_data,
                                                                        NULL, &profile, &first_reference));
    /*
     * dnx_algo_template_allocate when default profile exists, should fail
     */
    rv = algo_temp_mngr_db.template_with_default_check_values.allocate_single(unit, FLAGS_NONE,
                                                                              &profile_data, NULL, &profile,
                                                                              &first_reference);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "dnx_algo_template_allocate when default profile exists but dnx_algo_template_allocate was successful\n");
    }

    /*
     * dnx_algo_template_allocate with non existent name, should fail
     */
    rv = algo_temp_mngr_db.template_test_bad_values.allocate_single(unit, FLAGS_NONE, &profile_data,
                                                                    NULL, &profile, &first_reference);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "non existent name but dnx_algo_template_allocate was successful\n");
    }

    /*
     * }
     */
    /***************************************
     * test dnx_algo_template_exchange
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_exchange with valid values, should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default_check_values.exchange
                    (unit, FLAGS_NONE, &profile_data, old_profile, NULL, &new_profile, &first_reference, &is_last));

    /*
     * dnx_algo_template_exchange with valid values (per core), should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core_check_values.exchange
                    (unit, FIRST_CORE_ID, FLAGS_NONE, &profile_data, old_profile, NULL, &new_profile, &first_reference,
                     &is_last));

    /*
     * dnx_algo_template_exchange with non existent name, should fail
     */
    rv = algo_temp_mngr_db.template_test_bad_values.exchange
        (unit, FLAGS_NONE, &profile_data, old_profile, NULL, &new_profile, &first_reference, &is_last);

    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "non existent name but dnx_algo_template_exchange was successful\n");
    }
    /*
     * dnx_algo_template_exchange with negative old_profile, should fail
     */
    rv = algo_temp_mngr_db.template_with_default_check_values.exchange
        (unit, FLAGS_NONE, &profile_data, NEGATIVE_PROFILE, NULL, &new_profile, &first_reference, &is_last);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative old_profile but dnx_algo_template_exchange was successful\n");
    }

    /*
     * }
     */

    /***************************************
     * test dnx_algo_template_replace_data
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_replace_data with valid values, should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default_check_values.replace_data(unit, new_profile,
                                                                                      &replace_profile_data));

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_with_default_check_values.profile_data_get(unit, new_profile, &ref_count, &compare_data));

    if (replace_profile_data.element_8 != compare_data.element_8
        || replace_profile_data.element_16 != compare_data.element_16
        || replace_profile_data.element_32 != compare_data.element_32
        || replace_profile_data.element_int != compare_data.element_int)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "The replacement of the data was not successful.\n");
    }

    /*
     * dnx_algo_template_replace_data with non existent name, should fail
     */
    rv = algo_temp_mngr_db.template_test_bad_values.replace_data(unit, new_profile, &replace_profile_data);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "non existent name but dnx_algo_template_replace_data was successful\n");
    }
    /*
     * dnx_algo_template_replace_data with negative profile, should fail
     */
    rv = algo_temp_mngr_db.template_test_bad_values.replace_data(unit, NEGATIVE_PROFILE, &replace_profile_data);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative old_profile but dnx_algo_template_replace_data was successful\n");
    }
    /*
     * dnx_algo_template_replace_data with old_profile = nof_profiles, should fail
     */
    
    rv = algo_temp_mngr_db.template_test_bad_values.replace_data
        (unit, dnx_data_device.general.nof_cores_get(unit), &replace_profile_data);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "old_profile = nof_profiles but dnx_algo_template_replace_data was successful\n");
    }

    /*
     * }
     */

    /***************************************
     * test dnx_algo_template_clear
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_clear with valid values, should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default_check_values.clear(unit));

    /*
     * }
     */
    /***************************************
     * test dnx_algo_template_profile_get
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_profile_get with valid values, should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_test_without_default_check_values.profile_get(unit, &profile_data, &profile));
    /*
     * dnx_algo_template_profile_get with valid values (per core), should pass
     */
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_test_per_core_check_values.profile_get(unit, FIRST_CORE_ID, &profile_data, &profile));

    /*
     * dnx_algo_template_profile_get with non existent name, should fail
     */
    rv = algo_temp_mngr_db.template_test_bad_values.profile_get(unit, &profile_data, &profile);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "non existent name but dnx_algo_template_profile_get was successful\n");
    }
    /*
     * }
     */
    /***************************************
     * test dnx_algo_template_free
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_free with valid values, should pass
     */

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_test_without_default_check_values.free_single(unit, FIRST_PROFILE, &is_last));
    /*
     * dnx_algo_template_free with valid values (per core), should pass
     */

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_test_per_core_check_values.free_single(unit, FIRST_CORE_ID, FIRST_PROFILE, &is_last));
    /*
     * dnx_algo_template_free when default profile exists, should fail
     */

    rv = algo_temp_mngr_db.template_with_default_check_values.free_single(unit, FIRST_PROFILE, &is_last);

    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "dnx_algo_template_free when default profile exists but dnx_algo_template_free was successful\n");
    }

    /*
     * dnx_algo_template_free with negative profile, should fail
     */

    rv = algo_temp_mngr_db.template_with_default_check_values.free_single(unit, FIRST_PROFILE - 1, &is_last);

    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "negative profile but dnx_algo_template_free was successful\n");
    }
    /*
     * dnx_algo_template_free with profile = nof_profiles, should fail
     */
    rv = algo_temp_mngr_db.template_with_default_check_values.free_single(unit, data.nof_profiles, &is_last);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "profile = nof_profiles but dnx_algo_template_free was successful\n");
    }
    /*
     * }
     */
    /***************************************
     * test dnx_algo_template_profile_profile_data_get
     **************************************/
    /*
     * {
     */
    /*
     * dnx_algo_template_profile_profile_data_get with valid values, should pass
     */

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_with_default_check_values.profile_data_get(unit, FIRST_PROFILE, &ref_count,
                                                                        &profile_data));
    /*
     * dnx_algo_template_profile_profile_data_get with valid values (per core), should pass
     */

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_test_per_core_check_values.profile_data_get(unit, FIRST_CORE_ID, FIRST_PROFILE, &ref_count,
                                                                         &profile_data));

    /*
     * dnx_algo_template_profile_profile_data_get with negative profile_id, should fail
     */

    rv = algo_temp_mngr_db.template_with_default_check_values.profile_data_get(unit, FIRST_PROFILE - 1,
                                                                               &ref_count, &profile_data);

    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "negative profile_id but dnx_algo_template_profile_profile_data_get was successful\n");
    }
    /*
     * dnx_algo_template_profile_profile_data_get with profile_id = nof_profiles, should fail
     */
    rv = algo_temp_mngr_db.template_with_default_check_values.profile_data_get(unit, data.nof_profiles,
                                                                               &ref_count, &profile_data);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "profile_id = nof_profiles but dnx_algo_template_profile_profile_data_get was successful\n");
    }
    /*
     * }
     */

     /***************************************
     * test dnx_algo_template_get_next
     **************************************/
    /*
     * {
     */
    current_profile = FIRST_PROFILE;

    rv = algo_temp_mngr_db.template_test_bad_values.get_next(unit, &current_profile);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "dnx_algo_template_get_next should fail because name=TEMPLATE_TEST_BAD_VALUES\n");
    }

    current_profile = DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE;

    rv = algo_temp_mngr_db.template_test_without_default_check_values.get_next(unit, &current_profile);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "dnx_algo_template_get_next should fail because current_profile=DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE\n");
    }

    /*
     * }
     */

    /***************************************
     * test dnx_algo_template_free_all
     **************************************/
    /*
     * {
     */

    rv = algo_temp_mngr_db.template_with_default_check_values.free_all(unit, current_profile);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "dnx_algo_template_get_next should fail because name=TEMPLATE_TEST_WITH_DEFAULT\n");
    }

    current_profile = DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE;

    rv = algo_temp_mngr_db.template_test_without_default_check_values.free_all(unit, current_profile);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "dnx_algo_template_get_next should fail because current_profile=DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE\n");
    }
    /*
     * }
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * run template manager algorithm unit test
 * test summary:
 *  1- check template manager API functions valid/invalid inputs
 *  2- run a scenario which involves all the APIs.
 * \par DIRECT INPUT
 *  \param [in] unit - The Unit number.
 *  \param [in] args - added to match with shell cmds structure
 *  \param [in] sand_control - added to match with shell cmds structure
 * \par DIRECT OUTPUT:
 *   Non-zero in case of an error.
 * \par INDIRECT INPUT
 *  \param [in] *args - see args in direct input above
 *  \param [in] *sand_control - see sand_control in direct input above
 * \par INDIRECT OUTPUT
 *   the executed test
 */
shr_error_e
dnx_algo_template_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv;
    dnx_algo_template_create_data_t data_default, data;
    template_mngr_example_data_t profile_data, allocate_profile_data, new_profile_value;
    uint8 first_reference, is_last;
    int profile, ref_count, profile_counter, new_profile, current_profile;
    bsl_severity_t original_severity_sw_state, original_temp_mngr;
    uint32 core_id;

    SHR_FUNC_INIT_VARS(unit);

    original_severity_sw_state = BSL_INFO;
    original_temp_mngr = BSL_INFO;

    /*
     * init profile_data and allocate_profile_data with different values,
     * each struct contains 4 different types : uint8/16/32 and int
     */
    profile_data.element_8 = DEFAULT_PROFILE_DATA_8;
    profile_data.element_16 = DEFAULT_PROFILE_DATA_16;
    profile_data.element_32 = DEFAULT_PROFILE_DATA_32;
    profile_data.element_int = DEFAULT_PROFILE_DATA_INT;
    allocate_profile_data.element_8 = 0x11;
    allocate_profile_data.element_16 = DEFAULT_PROFILE_DATA_16;
    allocate_profile_data.element_32 = DEFAULT_PROFILE_DATA_32;
    allocate_profile_data.element_int = DEFAULT_PROFILE_DATA_INT;
    profile_counter = 0;

    /*
     * init template parameters for a template without default profile
     */
    sal_memset(&data, 0, sizeof(dnx_algo_template_create_data_t));
    data.flags = FLAGS_NONE;
    data.nof_profiles = NOF_PROFILES;
    data.max_references = MAX_REFERENCES;
    data.data_size = sizeof(template_mngr_example_data_t);
    data.first_profile = FIRST_PROFILE;

    /*
     * init template parameters for a template with default profile
     */
    sal_memset(&data_default, 0, sizeof(dnx_algo_template_create_data_t));
    data_default.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    data_default.first_profile = FIRST_PROFILE;
    data_default.nof_profiles = NOF_PROFILES;
    data_default.max_references = MAX_REFERENCES;
    data_default.default_profile = DEFAULT_PROFILE;
    data_default.data_size = sizeof(template_mngr_example_data_t);
    data_default.default_data = &profile_data;

    /*
     * use err reocvery to revert system state after test
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    /*
     * Turning on the alloc during test flag (set it to 1)
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /*
     * save the original severity level of the sw state and severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, original_temp_mngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_TEMPLATEMNGR, bslSeverityFatal);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.init(unit));

    /*
     * check valid/invalid input values for all APIs
     */
    SHR_IF_ERR_EXIT(dnx_algo_template_check_input_valid_values(unit));

    /*
     * create 3 templates:
     * 1- template without default profile
     * 2- template with a default profile
     * 3- template without default profile and separated per core
     */
    /*
     * create all needed templates for this test
     */
    sal_strncpy(data.name, TEMPLATE_TEST_WITHOUT_DEFAULT, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.create(unit, &data, NULL));

    sal_strncpy(data_default.name, TEMPLATE_TEST_WITH_DEFAULT, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default.create(unit, &data_default, NULL));

    sal_strncpy(data.name, TEMPLATE_TEST_PER_CORE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.alloc(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.create(unit, core_id, &data, NULL));
    }

    /*
     * test profile_get check that default profile was created and it has the right number of references
     */

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_with_default.profile_get(unit, &profile_data, &profile));
    if (profile != DEFAULT_PROFILE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "default profile doesn't exist in index 0\n");
    }

    rv = algo_temp_mngr_db.template_test_without_default.profile_get(unit, &profile_data, &profile);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "default profile shouldnt exist but it was found\n");
    }

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.
                    template_with_default.profile_data_get(unit, DEFAULT_PROFILE, &ref_count, &profile_data));
    if ((profile_data.element_8 != DEFAULT_PROFILE_DATA_8) || (profile_data.element_16 != DEFAULT_PROFILE_DATA_16)
        || (profile_data.element_32 != DEFAULT_PROFILE_DATA_32)
        || (profile_data.element_int != DEFAULT_PROFILE_DATA_INT) || (ref_count != data_default.max_references))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "default profile data is wrong in index 0\n");
    }

    /*
     * check allocate, keep allocating till max_references is reached
     */
    for (profile_counter = FIRST_PROFILE; profile_counter < data_default.max_references; profile_counter++)
    {

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.allocate_single(unit, FLAGS_NONE,
                                                                                        &allocate_profile_data, NULL,
                                                                                        &profile, &first_reference));
        if (profile_counter == FIRST_PROFILE && !first_reference)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "profile was allocated for the first time but first_reference is false, profile : %d\n",
                         profile_counter);
        }
    }

    rv = algo_temp_mngr_db.template_test_without_default.allocate_single
        (unit, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference);

    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "max_references is reached but allocate was successful\n");
    }

    /*
     * test clear
     */

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.clear(unit));

    current_profile = FIRST_PROFILE;

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.get_next(unit, &current_profile));
    if (current_profile != DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "No profiles should be allocated after we cleared all.\n");
    }

    for (profile_counter = FIRST_PROFILE; profile_counter < data_default.max_references; profile_counter++)
    {

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.allocate_single
                        (unit, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference));
        if (profile_counter == FIRST_PROFILE && !first_reference)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "profile was allocated for the first time but first_reference is false, profile : %d\n",
                         profile_counter);
        }
    }

    /*
     * Test dnx_algo_template_get_next and dnx_algo_template_free_all.
     */

    current_profile = FIRST_PROFILE;
    while (TRUE)
    {

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.get_next(unit, &current_profile));
        if (current_profile == DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE)
        {
            break;
        }

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.free_all(unit, current_profile));
        current_profile++;
    }

    current_profile = FIRST_PROFILE;

    SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.get_next(unit, &current_profile));
    if (current_profile != DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "No profiles should be allocated after we cleared all.\n");
    }

    /*
     * check allocate, keep allocating till nof_profiles is reached
     */
    for (profile_counter = FIRST_PROFILE; profile_counter < data.nof_profiles; profile_counter++)
    {
        allocate_profile_data.element_int = profile_counter;
        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.allocate_single
                        (unit, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference));

        if (!first_reference)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "profile was allocated for the first time but first_reference is false, profile: %d\n",
                         profile_counter);
        }
    }

    allocate_profile_data.element_int = profile_counter;
    rv = algo_temp_mngr_db.template_test_without_default.allocate_single
        (unit, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "nof_profiles is reached but allocate was successful\n");
    }

    /*
     * check exchange, exchange odd profiles with even profiles
     */
    for (profile_counter = FIRST_PROFILE + 1; profile_counter < data_default.nof_profiles; profile_counter += 2)
    {

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.profile_data_get
                        (unit, profile_counter - 1, &ref_count, &new_profile_value));

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.exchange
                        (unit, FLAGS_NONE, &new_profile_value, profile_counter, NULL, &new_profile,
                         &first_reference, &is_last));
        if (first_reference || !is_last || new_profile != profile_counter - 1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "exchange wasnt performed properly, profile: %d\n", profile_counter);
        }
    }

    /*
     * check that odd profiles have 0 references and even profiles have 2 references
     */
    for (profile_counter = FIRST_PROFILE; profile_counter < data_default.nof_profiles; profile_counter++)
    {

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_without_default.profile_data_get
                        (unit, profile_counter, &ref_count, &profile_data));
        /*
         * odd
         */
        if ((profile_counter % 2) && (ref_count != 0))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "odd profile with non zero ref_count, profile : %d\n", profile_counter);
        }
        /*
         * even
         */
        if (!(profile_counter % 2) && (ref_count != 2))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "even profile with ref_count != 2, profile : %d\n", profile_counter);
        }
    }

    /*
     * check that templates are separated per core when using multicore in sw state xml
     */
    if (dnx_data_device.general.nof_cores_get(unit) >= MIN_NOF_CORES)
    {

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.allocate_single
                        (unit, FIRST_CORE_ID, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference));

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.allocate_single
                        (unit, SECOND_CORE_ID, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference));
        if (!first_reference)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "profile is allocated for the first time in core 1 but first_reference = false\n");
        }
        allocate_profile_data.element_8 = 0x22;

        SHR_IF_ERR_EXIT(algo_temp_mngr_db.template_test_per_core.allocate_single
                        (unit, FIRST_CORE_ID, FLAGS_NONE, &allocate_profile_data, NULL, &profile, &first_reference));

        rv = algo_temp_mngr_db.template_test_per_core.profile_get(unit, SECOND_CORE_ID, &allocate_profile_data,
                                                                  &profile);
        if (rv == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "profile was allocated only in core 0 but it was found in core 1 as well\n");
        }
    }

    /*
     * }
     */

exit:

    /*
     * set back the severity level of the defragmented chunk and sw state to theirs original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_temp_mngr);

    /*
     * Turning off the alloc during test flag (set it to 0)
     */
    dnxc_sw_state_alloc_during_test_set(unit, 0);

    dnx_rollback_journal_end(unit, TRUE);

    SHR_FUNC_EXIT;
}

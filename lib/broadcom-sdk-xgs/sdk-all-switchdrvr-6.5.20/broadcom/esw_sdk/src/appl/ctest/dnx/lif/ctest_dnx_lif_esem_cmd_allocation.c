/*
 * ctest_dnx_lif_esem_cmd_allocation.c
 *
 *  Created on: Mar 3, 2020
 *      Author: eb892187
 */

/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "../../../../../include/bcm_int/dnx/algo/template_mngr/template_mngr_even_distribution.h"
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/port_pp/algo_port_pp.h>
#include <bcm_int/dnx/port/port_esem.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/**
 * }
 */
/*
 * DEFINEs
 * {
 */
/**
 * \brief
 *   Keyword for test type on 'dir_ext_semantic' command (direct extraction testing)
 *   Type can be either BCM or DNX
 */
/** The amount of free ESEM CMD allocation id's after device is initiated */
#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_MAXIMUM_ALLOCATIONS (dnx_data_esem.access_cmd.nof_cmds_get(unit) - dnx_data_esem.access_cmd.nof_esem_cmd_default_profiles_get(unit))

/** The amount of allocations we want to perform during distribution testing */
#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_DISTRIBUTION_TEST_ALLOCATIONS (32 - dnx_data_esem.access_cmd.nof_esem_cmd_default_profiles_get(unit))

/** The amount of allocations we want to perform during range testing */
#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_RANGE_ALLOCATIONS 4

/** The start value of the range we use during range testing */
#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_RANGE_START 16

/** The end value of the range we use during range testing */
#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_RANGE_END 28

/** Strings used for logging during the course of the tests */
#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST "ESEM CMD allocation even distribution"

#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_BASIC "basic"

#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_MAX_CAPACITY "maximum capacity"

#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_CUSTOM_RANGE "allocation in range"

#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_OUT_OF_RANGE "allocation outside of range"

#define CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_SAME_DATA_DIFFERENT_RANGE "allocation the same data in different ranges"

/*
 * }
 */
/*
 * GLOBALs
 * {
 */
sh_sand_man_t dnx_lif_esem_cmd_allocation_test_man = {
    "Esem cmd allocation test.",
    "Perform a semantic test of ESEM command allocations. allocate/get/free, testing different scenarios."
};
/*
 * This is a global that we use in order to create different ESEM CMD data (dnx_esem_cmd_data_t)
 */
uint8 dnx_lif_esem_cmd_allocation_app_db_id_iterator;
/*
 * }
 */
/**
 * \brief
 *    Fill the esem cmd data with default values for the test
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in,out]
 *    esem_cmd_data- Pointer to dnx_esem_cmd_data_t structure to be filled
 *
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_init_esem_data(
    int unit,
    dnx_esem_cmd_data_t * esem_cmd_data)
{
    SHR_FUNC_INIT_VARS(unit);

    /** ESEM_ACCESS_IF_1 */
    esem_cmd_data->esem[ESEM_ACCESS_IF_1].app_db_id = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_PEM_KEY1;
    esem_cmd_data->esem[ESEM_ACCESS_IF_1].valid = DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID;
    esem_cmd_data->esem[ESEM_ACCESS_IF_1].default_result_profile =
        dnx_data_esem.default_result_profile.default_native_get(unit);

    /** ESEM_ACCESS_IF_2 */
    esem_cmd_data->esem[ESEM_ACCESS_IF_2].app_db_id = DBAL_ENUM_FVAL_ESEM_APP_DB_ID_INLIF_OUTLIF;
    esem_cmd_data->esem[ESEM_ACCESS_IF_2].valid = DNX_PORT_ESEM_DEFAULT_RESULT_PROFILE_INVALID;
    esem_cmd_data->esem[ESEM_ACCESS_IF_2].default_result_profile =
        dnx_data_esem.default_result_profile.default_ac_get(unit);
    /** ESEM_ACCESS_IF_3 */
    esem_cmd_data->esem[ESEM_ACCESS_IF_3].valid = TRUE;

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Fill the esem cmd data with values to get new data that doesn't already exists
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in,out]
 *    esem_cmd_data- Pointer to dnx_esem_cmd_data_t structure to be filled
 *
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_get_new_esem_data(
    int unit,
    dnx_esem_cmd_data_t * esem_cmd_data)
{
    int rv = 0, profile = 0, nof_iterations = 0, max_nof_iterations;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(esem_cmd_data, _SHR_E_PARAM, "esem_cmd_data");
    /** no need to attempt on finding new data for more iterations then the amount of cmds possible */
    max_nof_iterations = dnx_data_esem.access_cmd.nof_cmds_get(unit);
    while ((rv != _SHR_E_NOT_FOUND) && (nof_iterations < max_nof_iterations))
    {
        /**
         * We make small change in the data as an attempt to get data that haven't been allocated yet
         */
        esem_cmd_data->esem[ESEM_ACCESS_IF_3].app_db_id = dnx_lif_esem_cmd_allocation_app_db_id_iterator++;
        rv = algo_port_pp_db.esem.access_cmd.profile_get(unit, esem_cmd_data, &profile);
        nof_iterations++;
    }
    SHR_VAL_VERIFY(rv, _SHR_E_NOT_FOUND, _SHR_E_FAIL, "profile already exists");

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    Verify that allocation is as expected regarding:
 *    first_reference
 *    last_reference
 *    range
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    extra_alloc_info- _ptr extra allocation information
 * \param [in]
 *    profile- allocated pprofile
 * \param [in]
 *    expected_first_reference- The boolean value of the expected first reference
 * \param [in]
 *    expected_last_reference- The boolean value of the expected last reference
 * \param [in]
 *    first_reference- The boolean value of the first reference
 * \param [in]
 *    last_reference- The boolean value of the last reference
 *
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_exchange_esem_profile_verify(
    int unit,
    smart_template_alloc_info_t * extra_alloc_info,
    int profile,
    uint8 expected_first_reference,
    uint8 expected_last_reference,
    uint8 first_reference,
    uint8 last_reference)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_VAL_VERIFY(first_reference, expected_first_reference, _SHR_E_FAIL, "first_reference is not as expected");
    SHR_VAL_VERIFY(last_reference, expected_last_reference, _SHR_E_FAIL, "first_reference is not as expected");

    /** Check range if allocation in range is defined */
    if (extra_alloc_info != NULL
        && _SHR_IS_FLAG_SET(extra_alloc_info->resource_flags, RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE))
    {
        SHR_RANGE_VERIFY(profile, extra_alloc_info->resource_alloc_info.range_start,
                         extra_alloc_info->resource_alloc_info.range_end, _SHR_E_FAIL,
                         "Allocated profile is out of range.\n");
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    Performs one profile exchange to the ESEM allocation template manager
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    extra_alloc_info- Exsta allocation information
  * \param [in]
 *    flags- allocation flags
 * \param [in]
 *    grain size- grain size as defined for ESEM CMD allocation manager
 * \param [in]
 *    esem_cmd_data- Pointer to dnx_esem_cmd_data_t structure to be exchanged
 * \param [in]
 *    expected_first_reference- The boolean value of the expected first reference
 * \param [in]
 *    expected_last_reference- The boolean value of the expected last reference
 * \param [in]
 *    old_profile- The profile that was allocated before the exchange
 * \param [in,out]
 *    new_profile- _ptr for the new allocated profile
 * \param [in,out]
 *    grain_cnt- _ptr counter for nof allocations per grain
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_exchange_profile(
    int unit,
    smart_template_alloc_info_t * extra_alloc_info,
    uint32 flags,
    int grain_size,
    dnx_esem_cmd_data_t * esem_cmd_data,
    uint8 expected_first_reference,
    uint8 expected_last_reference,
    int old_profile,
    int *new_profile,
    int *grain_cnt)
{
    uint8 first_reference, last_reference;
    SHR_FUNC_INIT_VARS(unit);

    /** Call exchange */
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.
                    access_cmd.exchange(unit, flags, esem_cmd_data, old_profile, extra_alloc_info, new_profile,
                                        &first_reference, &last_reference));

    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_exchange_esem_profile_verify(unit,
                                                                                   extra_alloc_info, *new_profile,
                                                                                   expected_first_reference,
                                                                                   expected_last_reference,
                                                                                   first_reference, last_reference));

    if (last_reference)
    {
        cli_out("\n\tfreed profile is: %d.", old_profile);
        grain_cnt[*new_profile / grain_size]--;
    }
    /** Print given profile */
    cli_out("\n\tallocated profile is: %d.", *new_profile);

    /** Increment counter */
    grain_cnt[*new_profile / grain_size]++;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    This function verifies that the allocations are distributed within the allocation range given
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    nof_allocations- Number of allocations made within this range
 * \param [in]
 *    extra_arguments- extra arguments to be forwarded to the exchange function
 *    of type smart_template_alloc_info_t
 * \param [in]
 *    nof_grains- Number of grains in the data base (length of the grain counter array)
 * \param [in]
 *    grain_size- The size of the grain
 * \param [in]
 *    grain_cnt- Pointer to a counter array that holds the information of
 *    how many allocations have been made at each grain
 *
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_check_distribution(
    int unit,
    int nof_allocations,
    smart_template_alloc_info_t * extra_arguments,
    int nof_grains,
    int grain_size,
    int *grain_cnt)
{
    int grain_iter, grain_range_start, grain_range_end;
    int max_allocations_in_one_grain = 0;
    int addition;
    SHR_FUNC_INIT_VARS(unit);

    if (extra_arguments == NULL)
    {
        grain_range_start = 0;
        grain_range_end = nof_grains;
    }
    else
    {
        grain_range_start = extra_arguments->resource_alloc_info.range_start / grain_size;
        grain_range_end = extra_arguments->resource_alloc_info.range_end / grain_size;
    }
    /**
     * If nof allocations is perfectly divided by the range then each grain should have the division result number
     * of allocations, if it doesn't then the remain should add 1 at the most to the result
     */
    addition = (nof_allocations % (grain_range_end - grain_range_start)) ? 1 : 0;

    for (grain_iter = grain_range_start; grain_iter < grain_range_end; grain_iter++)
    {
        if (max_allocations_in_one_grain < grain_cnt[grain_iter])
        {
            max_allocations_in_one_grain = grain_cnt[grain_iter];
        }
    }

    SHR_MAX_VERIFY(max_allocations_in_one_grain, (nof_allocations / (grain_range_end - grain_range_start)) + addition,
                   _SHR_E_FAIL, "Allocated profiles is not distributed as expected.\n");
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    This function verifies that the allocations are distributed within the allocation range given
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    grain_size- Grain size of the data that is being managed by the template manager
 * \param [in]
 *    grain_cnt- counter array that saves the number of allocated data for each grain
 *
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_init_grain_cnt(
    int unit,
    int grain_size,
    int *grain_cnt)
{
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(grain_cnt, 0, sizeof(int) * DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS);
    /** Profiles allocated during the initialization of the device */
    grain_cnt[dnx_data_esem.access_cmd.no_action_get(unit) / grain_size]++;
    grain_cnt[dnx_data_esem.access_cmd.default_native_get(unit) / grain_size]++;
    grain_cnt[dnx_data_esem.access_cmd.default_ac_get(unit) / grain_size]++;
    grain_cnt[dnx_data_esem.access_cmd.sflow_sample_interface_get(unit) / grain_size]++;
    grain_cnt[dnx_data_esem.access_cmd.ip_tunnel_no_action_get(unit) / grain_size]++;

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Basic test for the allocation manager of ESEM commands
 *
 *    1. Exchange unique data and check that the allocation profile is as expected
 *       Reaped this stage as input X indicates.
 *    2. Free the first allocated profile, allocate new data, check that the newly allocated ESEM CMD
 *       is the same ID as the one we freed (first allocated)
 *    3. If the allocation is at its limits try to allocate one more to recive the 'table full' error
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    extra_alloc_info- Extra allocation information in the format of smart_template_alloc_info_t
 *        - see smart template documentation for further explanation
 * \param [in]
 *    flags- Allocation flags
 * \param [in]
 *    nof_allocations - Number of allocations to perform
 * \param [in]
 *    test_name - name of the test that we currently performing
 * \return
 *    shr_error_e
 *
 * \remark
 *    We use exchange instead of free and allocate (even though those are the functions we implemented directly)
 *    because using 'default profile' in smart template prevents a direct use of this functions.
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_basic_test(
    int unit,
    smart_template_alloc_info_t * extra_alloc_info,
    uint32 flags,
    int nof_allocations,
    char *test_name)
{
    dnx_esem_cmd_data_t esem_cmd_data;
    int profile, rv, iter = 0;
    int grain_size;
    int *grain_cnt = NULL;
    int profile_to_be_freed = 0, tmp_profile;
    SHR_FUNC_INIT_VARS(unit);
    cli_out("\n\n %s %s test: ", CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST, test_name);
    dnx_lif_esem_cmd_allocation_app_db_id_iterator = 0;
    /** Allocate grain cnt in order to test distribution */
    SHR_ALLOC(grain_cnt, sizeof(int) * DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS,
              "counter for grain allocations", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Init rollback journal in order to revert the allocations */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    grain_size = dnx_data_esem.access_cmd.nof_cmds_get(unit) / DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS;

    /** Init the grain cnt array with the profiles that have been allocated during device init itme */
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_init_grain_cnt(unit, grain_size, grain_cnt));

    /** Init an esem cmd data structure to be exchanged during the test */
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_init_esem_data(unit, &esem_cmd_data));

    /** 1. Allocate new ESEM CMDs using the allocation manager */
    /** allocate first ESEM CMD and save it for comparison in stage 2*/
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_get_new_esem_data(unit, &esem_cmd_data));

    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_exchange_profile(unit, extra_alloc_info, flags,
                                                                       grain_size, &esem_cmd_data, TRUE, FALSE, 0,
                                                                       &profile, grain_cnt));
    profile_to_be_freed = profile;
    iter++;

    for (; iter < nof_allocations; iter++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_get_new_esem_data(unit, &esem_cmd_data));

        SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_exchange_profile(unit, extra_alloc_info,
                                                                           flags, grain_size, &esem_cmd_data, TRUE,
                                                                           FALSE, 0, &profile, grain_cnt));
    }

    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_check_distribution(unit, nof_allocations +
                                                                         dnx_data_esem.
                                                                         access_cmd.nof_esem_cmd_default_profiles_get
                                                                         (unit), extra_alloc_info,
                                                                         DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS,
                                                                         grain_size, grain_cnt));

    /** 2. Free profile and allocate anew */
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_exchange_profile(unit, extra_alloc_info, flags,
                                                                       grain_size, &esem_cmd_data, FALSE, TRUE,
                                                                       profile_to_be_freed, &profile, grain_cnt));
    tmp_profile = profile;
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_get_new_esem_data(unit, &esem_cmd_data));
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_exchange_profile(unit, extra_alloc_info, flags,
                                                                       grain_size, &esem_cmd_data, TRUE, FALSE,
                                                                       tmp_profile, &profile, grain_cnt));

    SHR_VAL_VERIFY(profile, profile_to_be_freed, _SHR_E_FAIL, "Allocation mngr did not allocate the correct profile");

    /** 3. Allocate another profile for maximum capacity check */
    if (nof_allocations == CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_MAXIMUM_ALLOCATIONS)
    {
        /** Exchange the new profile to see if a 'table full' error is received */
        SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_get_new_esem_data(unit, &esem_cmd_data));
        rv = ctest_dnx_lif_esem_cmd_allocation_exchange_profile(unit, extra_alloc_info, flags,
                                                                grain_size, &esem_cmd_data, TRUE, FALSE, 0, &profile,
                                                                grain_cnt);

        SHR_VAL_VERIFY(rv, _SHR_E_FULL, _SHR_E_FAIL,
                       "Allocation mngr manage to allocate another profile when the table should be full");
    }
    cli_out("\n\n===> %s %s test: PASSED!\n", CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST, test_name);
exit:
    /** Revert allocation operations */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    SHR_FREE(grain_cnt);
    SHR_FUNC_EXIT;
}


static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_out_of_range_test(
    int unit,
    smart_template_alloc_info_t * extra_alloc_info,
    uint32 flags,
    char *test_name)
{
    dnx_esem_cmd_data_t esem_cmd_data;
    int old_profile, new_profile;
    int rv, ref_count;
    uint8 first_reference, last_reference;
    SHR_FUNC_INIT_VARS(unit);
    cli_out("\n\n %s %s test: ", CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST, test_name);

    /** Init rollback journal in order to revert the allocations */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    /** Init an esem cmd data structure to be exchanged during the test */
    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));

    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_init_esem_data(unit, &esem_cmd_data));
    SHR_IF_ERR_EXIT(algo_port_pp_db.esem.access_cmd.exchange(unit, flags, &esem_cmd_data,
                                                             0, extra_alloc_info, &old_profile,
                                                             &first_reference, &last_reference));
    /**
     * get default data from profile 0 (because profile 0 is out of range)
     */
    algo_port_pp_db.esem.access_cmd.profile_data_get(unit, 0, &ref_count, &esem_cmd_data);
    /**
     * exchange the data from profile 0 using the last allocated profile as the old profile
     * this should result in an error because the data we are trying to allocate allready exists and
     * outside of the range
     */
    rv = algo_port_pp_db.esem.access_cmd.exchange(unit, flags, &esem_cmd_data,
                                                  old_profile, extra_alloc_info, &new_profile, &first_reference,
                                                  &last_reference);

    SHR_VAL_VERIFY(rv, _SHR_E_INTERNAL, _SHR_E_FAIL,
                   "Allocation mngr manage to allocate another profile when the table should be full");

    cli_out("\n\n===> %s %s test: PASSED!\n", CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST, test_name);
exit:
    /** Revert allocation operations */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    This test tests the behavior of the ESEM CMD manager in the case of
 *    same data needs to be allocated in different range (because of different ESEM CMD bit size and predefined prefix)
 *
 * \param [in]
 *    unit- Relevant unit
 * \param [in]
 *    extra_alloc_info- Extra allocation information in the format of smart_template_alloc_info_t
 *        - see smart template documentation for further explanation
 * \param [in]
 *    flags- Allocation flags
 * \param [in]
 *    nof_allocations - Number of allocations to perform
 * \param [in]
 *    test_name - name of the test that we currently performing
 * \return
 *    shr_error_e
 *
 * \remark
 *    None
 *
 * \see
 *    None
 */
static shr_error_e
ctest_dnx_lif_esem_cmd_allocation_same_data_different_range(
    int unit,
    smart_template_alloc_info_t * extra_alloc_info,
    uint32 flags,
    char *test_name)
{
    dnx_esem_cmd_data_t esem_cmd_data;
    dnx_esem_cmd_suffix_size_t esem_cmd_size;
    uint8 is_first, is_last;
    int ref_count, new_profile, tag_iter, grain_size;
    uint32 prefix;
    dnx_port_esem_cmd_info_t esem_cmd_info;
    SHR_FUNC_INIT_VARS(unit);
    cli_out("\n\n %s %s test: ", CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST, test_name);

    /** Init rollback journal in order to revert the allocations */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    sal_memset(&esem_cmd_data, 0, sizeof(dnx_esem_cmd_data_t));
    sal_memset(&esem_cmd_info, 0, sizeof(dnx_port_esem_cmd_info_t));

    /**
     * stage 1
     * allocate the same data over different range
     * confirm that the allocations were successful
     */
    /** Define ESEM CMD size that stay as a constant for the following allocations */
    esem_cmd_size = DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_2;
    /**
     * grain size here represents the nof ESEM CMD IDs in the range (range unit size)
     * this is a derivative of the size previously defined
     */
    grain_size = GET_ESEM_CMD_SIZE_FROM_ENUM(DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_2) << 1;
    /** This is the initial prefix, this value is going to change according to the grain */
    prefix = 0;
    algo_port_pp_db.esem.access_cmd.profile_data_get(unit, 0, &ref_count, &esem_cmd_data);

    /**
     * Allocate the same data in a different range unit each time (prefix in incremented, size stays the same)
     * and save the profile allocated to the bit map
     */
    for (tag_iter = 0; tag_iter < DNX_ALGO_PP_PORT_ESEM_CMD_RES_NOF_TAGS; tag_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, esem_cmd_size, prefix, &esem_cmd_info, 0, &new_profile,
                                                   &esem_cmd_data, &is_first, &is_last));

        prefix += grain_size;
        /** Print given profile */
        cli_out("\n\tallocated profile is: %d.", new_profile);
    }

    /**
     * stage 2
     * Repeat stage one using the same data and different sizes and prefix
     * confirm that the data was already allocated (even thougt it was allocated using different prefixes and sizes)
     */
    esem_cmd_size = DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_4;
    grain_size = GET_ESEM_CMD_SIZE_FROM_ENUM(DNX_ALGO_ESEM_CMD_SUFFIX_SIZE_BIT_4) << 1;
    prefix = 0;
    for (tag_iter = 0; tag_iter < (dnx_data_esem.access_cmd.nof_cmds_get(unit) / grain_size); tag_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_port_esem_cmd_exchange(unit, flags, esem_cmd_size, prefix, &esem_cmd_info, 0, &new_profile,
                                                   &esem_cmd_data, &is_first, &is_last));
        SHR_VAL_VERIFY(is_first, FALSE, _SHR_E_FAIL,
                       "Allocation mngr allocated new profile when an already allocated profile should have been allocated");
        prefix += grain_size;
        /** Print given profile */
        cli_out("\n\tallocated profile is: %d.", new_profile);
    }
    cli_out("\n\n===> %s %s test: PASSED!\n", CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST, test_name);
exit:
    /** Revert allocation operations */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Unit test for ESEM CMD allocations
 *    performs 4 tests:
 *
 *    1. Basic allocation test - Check even distribution with no further constraints
 *    2. Maximum capacity test - Check maximum number of esem cmd allocations
 *    3. Range allocation test - Check custom range allocation
 *    4. Allocation outside of range
 *    5. Allocating same data with different ranges
 */
shr_error_e
ctest_dnx_lif_esem_cmd_allocation_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    smart_template_alloc_info_t extra_alloc_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&extra_alloc_info, 0, sizeof(smart_template_alloc_info_t));
    /** Run basic test */
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_basic_test(unit,
                                                                 NULL /** No extra arguments needed for basic test */ ,
                                                                 0,
                                                                 CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_DISTRIBUTION_TEST_ALLOCATIONS,
                                                                 CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_BASIC));

    /** Run maximum capacity test */
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_basic_test(unit, NULL
                                                                 /** No extra arguments needed for maximum capacity */ ,
                                                                 0,
                                                                 CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_MAXIMUM_ALLOCATIONS,
                                                                 CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_MAX_CAPACITY));

    /** Set extra allocation info for range testing */
    extra_alloc_info.resource_alloc_info.range_start = CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_RANGE_START;
    extra_alloc_info.resource_alloc_info.range_end = CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_RANGE_END;
    extra_alloc_info.resource_flags = RESOURCE_TAG_BITMAP_ALLOC_IN_RANGE;
    /** Run range allocation test */
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_basic_test(unit,
                                                                 &extra_alloc_info,
                                                                 0,
                                                                 CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_RANGE_ALLOCATIONS,
                                                                 CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_CUSTOM_RANGE));
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_out_of_range_test(unit,
                                                                        &extra_alloc_info, 0,
                                                                        CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_OUT_OF_RANGE));
    SHR_IF_ERR_EXIT(ctest_dnx_lif_esem_cmd_allocation_same_data_different_range(unit, NULL, 0,
                                                                                CTEST_DNX_LIF_ESEM_CMD_ALLOCATION_TEST_SAME_DATA_DIFFERENT_RANGE));

exit:
    SHR_FUNC_EXIT;

}

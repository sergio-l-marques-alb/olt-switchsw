/** \file ctest_dnx_field_range.h
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
* Include files.
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field_range.h>
/*
 * }
 */

#ifndef CTEST_DNX_FIELD_RANGE_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_RANGE_H_INCLUDED

/**
 *  Number of PMF stages that we set variables for
 */
#define CTEST_DNX_FIELD_RANGE_NOF_STAGES              4
/**
 *  The positive test.
 */
#define CTEST_DNX_FIELD_RANGE_POSITIVE_TEST           1
/**
 *  The negative test.
 */
#define CTEST_DNX_FIELD_RANGE_NEGATIVE_TEST           0
/**
 *  Number of fields in dnx_ctest_range_full_info_t
 *  for positive test.
 */
#define CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_POSITIVE_TEST              9
/**
 *  Number of fields in dnx_ctest_range_full_info_t
 *  for negative test.
 */
#define CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_NEGATIVE_TEST              10
/**
 * \brief
 *   Structure of all needed fields for testing the field_range DNX level.
 */
typedef struct
{

    /**
     * The type of the range. For more information look at the 'dnx_field_range_type_e'
     */
    dnx_field_range_type_e range_type;

    /**
     * Indicate the PMF stage. Different for each RangeType.
     */
    dnx_field_stage_e field_stage;

    /**
     * Identifier of the range: User-provided range ID
     */
    uint32 range_id;

    /**
     * Minimum value of the range.
     */
    uint32 min_val;

    /**
     * Maximum value of the range.
     */
    uint32 max_val;

    /**
     * Expected error for each case.
     */
    _shr_error_t expected_error;

} dnx_ctest_range_full_info_t;

/*
 * }
 */
/**
 * \brief
 *   Options list for 'range' shell command
 */
extern sh_sand_option_t Sh_dnx_field_range_options[];

/**
 * \brief
 *   man for 'range' command
 */
extern sh_sand_man_t Sh_dnx_field_range_man;

/**
 * \brief
 *   Options list for 'range' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_range_tests[];

/**
 * \brief - run range_quals init sequence in diag shell
 *
 * \param [in] unit - Number of hardware unit used
 *      [in] args - Pointer to args_t struct, not used
 *      [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_field_range_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_FIELD_RANGE_H_INCLUDED */

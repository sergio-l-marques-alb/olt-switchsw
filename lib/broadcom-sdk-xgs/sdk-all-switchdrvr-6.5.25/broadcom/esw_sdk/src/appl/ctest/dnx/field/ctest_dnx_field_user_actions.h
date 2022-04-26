/** \file ctest_dnx_field_user_actions.h
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
/*
 * }
 */

#ifndef CTEST_DNX_FIELD_USER_ACTIONS_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_H_INCLUDED

/**
 *  Number of PMF stages that we set variables for
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_NOF_STAGES              4
/**
 * The ID of the created action WITH_ID
 */
#define CTEST_DNX_FIELD_USER_ACTION_ID                       2200
/**
 * Illegal ID out of range [2048-2304]
 */
#define CTEST_DNX_FIELD_USER_ACTION_ILLEGAL_ACTION_ID  (dnx_data_field.action.user_1st_get(unit) + \
                                                        dnx_data_field.action.user_nof_get(unit) + 1)
/** 
 * MAX Number of actions as define ,device independent 
 */
#define CTEST_DNX_USER_ACTIONS_MAX_NOF_ACTIONS    (dnx_data_field.action.user_nof_get(unit) + 1)
/**
 * Size of user created action in bits
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE            2
/**
 * Invalid size for user created action
 * Used in case3 : CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_SIZE
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE_INVALID   25
/**
 * The length of the action name
 */
#define CTEST_DNX_FIELD_USER_ACTION_NAME_LENGTH                 19

/**
 * The list of actions to be created by the positive test.
 */
typedef enum
{

    CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_FIRST = 0x0,
    /**
     * Create action with ID.
     */
    CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_WITHOUT_ID = CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_FIRST,
    /**
     * Create action without ID.
     */
    CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_WITH_ID,
    /**
     * Create void action.
     */
    CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_VOID,
    CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_NOF
} ctest_dnx_field_user_actions_positive_test_e;

/**
 * Holds all of the test stages flags
 * This flags indicate specific positive or one of the negative cases
 * Used in appl_dnx_field_user_actions_semantic()
 */
typedef enum
{

    CTEST_DNX_FIELD_USER_ACTIONS_FIRST = 0x0,
    /**
     * Positive test
     */
    CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST = CTEST_DNX_FIELD_USER_ACTIONS_FIRST,
    /**
     * Negative case1:
     * For creating an action with ID out of range
     */
    CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_ID = 0x1,
    /**
     * Negative case2:
     * For creating an action with different stage then the supplied base_action
     */
    CTEST_DNX_FIELD_USER_ACTIONS_DIFFERENT_STAGE = 0x2,
    /**
     * Negative case3:
     * For creating an action with size bigger then base_action size
     */
    CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_SIZE = 0x3,
    /**
     * Negative case4:
     * For creating an action with same ID
     */
    CTEST_DNX_FIELD_USER_ACTIONS_SAME_ID = 0x4,
    /**
     * Negative case5:
     * For creating more actions than dnx_data_field.action.user_nof_get(unit)
     */
    CTEST_DNX_FIELD_USER_ACTIONS_MAX_NOF_ACTIONS = 0x5,
    /**
     * Negative case6:
     * NULL CHECK
     */
    CTEST_DNX_FIELD_USER_ACTIONS_NULL_CHECK = 0x6,
    CTEST_DNX_FIELD_USER_ACTIONS_NOF
} ctest_dnx_field_user_actions_test_flag_e;
/*
 * }
 */
/**
 * \brief
 *   Options list for 'user_actions_quals' shell command
 */
extern sh_sand_option_t Sh_dnx_field_user_actions_options[];

/**
 * \brief
 *   man for 'user_actions_quals' command
 */
extern sh_sand_man_t Sh_dnx_field_user_actions_man;
/**
 * \brief
 *   Options list for 'user_actions_quals' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_user_actions_tests[];

/**
 * \brief - run user_actions_quals init sequence in diag shell
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
shr_error_e sh_dnx_field_user_actions_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_FIELD_USER_ACTIONS_H_INCLUDED */

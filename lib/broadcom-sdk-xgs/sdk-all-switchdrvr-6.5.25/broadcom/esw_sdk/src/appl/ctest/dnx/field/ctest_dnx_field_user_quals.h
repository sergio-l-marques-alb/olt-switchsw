/** \file ctest_dnx_field_user_quals.h
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

#ifndef CTEST_DNX_FIELD_USER_QUALS_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_USER_QUALS_H_INCLUDED

/**
 * The ID of the created qual WITH_ID
 */
#define CTEST_DNX_FIELD_USER_QUAL_ID                          2200
/**
 * Illegal ID out of range [1024-1280]
 */
#define CTEST_DNX_FIELD_USER_QUAL_ILLEGAL_QUAL_ID  (dnx_data_field.qual.user_1st_get(unit) + \
                                                    dnx_data_field.qual.user_nof_get(unit) + 1)
/** 
 * MAX Number of quals as define ,device independent
 */
#define CTEST_DNX_USER_QUALS_MAX_NOF_QUALS    (dnx_data_field.qual.user_nof_get(unit) + 2 )
/**
 * Size of user created qual in bits
 */
#define CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE              2
/**
 * Invalid size for user created qual
 * Used in case3 : CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_SIZE
 */
#define CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE_INVALID      0
/**
 * The length of the qual name
 */
#define CTEST_DNX_FIELD_USER_QUAL_NAME_LENGTH                 19
/**
 * The number of created quals in the positive test
 * One with ID and one without ID
 */
#define CTEST_DNX_FIELD_USER_QUAL_NOF_QUALS                    2
/**
 * Holds all of the test stages flags
 * This flags indicate specific positive or one of the negative cases
 * Used in appl_dnx_field_user_quals_semantic()
 */
typedef enum
{

    CTEST_DNX_FIELD_USER_QUALS_FIRST = 0x0,
    /**
     * Positive test
     */
    CTEST_DNX_FIELD_USER_QUALS_POSITIVE_TEST = CTEST_DNX_FIELD_USER_QUALS_FIRST,
    /**
     * Negative case1:
     * For creating an qual with ID out of range
     */
    CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_ID = 0x1,
    /**
     * Negative case2:
     * For creating an qual with size bigger then base_qual size
     */
    CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_SIZE = 0x2,
    /**
     * Negative case3:
     * For creating an qual with same ID
     */
    CTEST_DNX_FIELD_USER_QUALS_SAME_ID = 0x3,
    /**
     * Negative case4:
     * For creating more quals than dnx_data_field.qual.user_nof_get(unit)
     */
    CTEST_DNX_FIELD_USER_QUALS_MAX_NOF_QUALS = 0x4,
    /**
     * Negative case5:
     * NULL CHECK
     */
    CTEST_DNX_FIELD_USER_QUALS_NULL_CHECK = 0x5,
    CTEST_DNX_FIELD_USER_QUALS_NOF
} ctest_dnx_field_user_quals_test_flag_e;
/*
 * }
 */
/**
 * \brief
 *   Options list for 'user_quals_quals' shell command
 */
extern sh_sand_option_t Sh_dnx_field_user_quals_options[];

/**
 * \brief
 *   man for 'user_quals_quals' command
 */
extern sh_sand_man_t Sh_dnx_field_user_quals_man;
/**
 * \brief
 *   Options list for 'user_quals_quals' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_user_quals_tests[];

/**
 * \brief - run user_quals_quals init sequence in diag shell
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
shr_error_e sh_dnx_field_user_quals_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_FIELD_USER_QUALS_H_INCLUDED */

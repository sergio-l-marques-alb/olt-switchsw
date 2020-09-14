/** \file ctest_dnx_field_user_actions.c
 * $Id$
 *
 * Field User action set/get usage example.
 * Positive test:
 *   1. Create an action without ID
 *   2. Compare the set and get fields
 *   3. Print the name of the action
 *   4. Delete the created action
 *   5. Verify after delete
 *   6. Create an action with ID and do 1-5
 * Negative test cases:
 *  1. Create an action with ID out of range
 *  2. Create an action with different stage then the supplied base_action
 *  3. Create an action with size bigger then base_action size
 *  4. Create an action with same ID
 *  5. Create more actions than dnx_data_field.action.user_nof_get(unit)
 *  6. NULL check
 */
/**
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

/**
 * Include files.
 * {
 */
#include <bcm/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include "ctest_dnx_field_user_actions.h"
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include "ctest_dnx_field_utils.h"
/**
 * }
 */

/* *INDENT-OFF* */
/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_TYPE              "type"
/**
 * \brief
 *   Keyword for stage of test on user_actions command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for TCL testing, clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_USER_ACTIONS_CLEAN             "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_USER_ACTIONS_OPTION_TEST_COUNT       "count"
/**
 * \brief
 *   Options list for 'user_actions' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_user_actions_options[] = {
     /** Name */                                             /** Type */            /** Description */                     /** Default */
    {CTEST_DNX_FIELD_USER_ACTIONS_TYPE,                SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_USER_ACTIONS_TEST_STAGE,          SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",   "ipmf1",   (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_USER_ACTIONS_CLEAN,               SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {CTEST_DNX_USER_ACTIONS_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}
    /**
     * End of options list - must be last.
     */
};
/**
 * \brief
 *   List of tests for 'user_actions' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_user_actions_tests[] = {
    /*
    {"DNX_field_actions_test_1", "type=DNX stage=ipmf1  count=1", CTEST_POSTCOMMIT},
    {"DNX_field_actions_test_2", "type=DNX stage=ipmf2  count=1", CTEST_POSTCOMMIT},
    {"DNX_field_actions_test_3", "type=DNX stage=ipmf3  count=1", CTEST_POSTCOMMIT},
    {"DNX_field_actions_test_4", "type=DNX stage=epmf   count=1", CTEST_POSTCOMMIT},
    {"BCM_field_actions_test_1", "type=BCM stage=ipmf1  count=1", CTEST_POSTCOMMIT},
    {"BCM_field_actions_test_2", "type=BCM stage=ipmf2  count=1", CTEST_POSTCOMMIT},
    {"BCM_field_actions_test_3", "type=BCM stage=ipmf3  count=1", CTEST_POSTCOMMIT},
    {"BCM_field_actions_test_4", "type=BCM stage=epmf   count=1", CTEST_POSTCOMMIT},
    */
    {NULL}
};
/**
 *  user_actions shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_user_actions_man = {
"Field user create actions related test utilities",
    "Activate field user created actions related test utilities. "
    "BCM-level tests and DNX-level tests. \r\n"
    "Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n"
    "The 'count' variable defines how many times the test will run. \r\n",
    "ctest field user_actions type=<BCM | DNX> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> clean=<0 | 1>"
};
/**
 * Actions per stage
 */
static bcm_field_stage_t
    Ctest_dnx_field_user_actions_action_array[CTEST_DNX_FIELD_USER_ACTIONS_NOF_STAGES] = {
/**
 * IPMF1/2
 */
    bcmFieldActionSnoopRaw,
/**
 * IPMF1/2
 */
    bcmFieldActionSnoopRaw,
/**
 * IPMF3
 */
    bcmFieldActionSnoopRaw,
/**
 * EPMF
 */
    bcmFieldActionPrioIntNew
};
/**
 * Illegal actions per stage
 * Used in Negative case2: Creating an action with different stage
 * then the supplied base_action
 */
static bcm_field_stage_t
    Ctest_dnx_field_user_actions_illegal_action_array[CTEST_DNX_FIELD_USER_ACTIONS_NOF_STAGES] = {
/**
 * IPMF1/2
 */
    bcmFieldActionQosMapIdNew,
/**
 * IPMF1/2
 */
    bcmFieldActionQosMapIdNew,
/**
 * IPMF3
 */
    bcmFieldActionQosMapIdNew,
/**
 * EPMF
 */
    bcmFieldActionUsePolicerResult
};
/**
 * Used in Negative cases for setting different error codes
 * in different cases
 */
static _shr_error_t Ctest_dnx_field_user_actions_expected_errors[CTEST_DNX_FIELD_USER_ACTIONS_NOF] = {
    /**
     * case1: CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_ID
     */
    _SHR_E_PARAM,
    /**
     * case2: CTEST_DNX_FIELD_USER_ACTIONS_DIFFERENT_STAGE
     */
    _SHR_E_NOT_FOUND,
    /**
     * case3: CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_SIZE
     */
    _SHR_E_PARAM,
    /**
     * case4: CTEST_DNX_FIELD_USER_ACTIONS_SAME_ID
     */
    _SHR_E_PARAM,
    /**
     * case5: CTEST_DNX_FIELD_USER_ACTIONS_MAX_NOF_ACTIONS
     */
    _SHR_E_RESOURCE,
    /**
     * case6: CTEST_DNX_FIELD_USER_ACTIONS_NULL_CHECK
     */
    _SHR_E_PARAM

};
/* *INDENT-ON* */
/**
 * \brief
 *  This procedure is used by appl_dnx_field_user_actions_semantic.
 *  It contains the dnx_field_action_create application.
 *  This procedure sets all required HW configuration for
 *  dnx_field_action_create to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] user_create_action_flags - Flags of dnx_field_action_create API.
 *             For now there is only one flag: DNX_FIELD_ACTION_FLAG_WITH_ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] bcm_action - The BCM action upon whihc the user defined action is based.
 * \param [in] test_flag -  Used for negative cases
 *                          For details see 'ctest_dnx_field_user_actions_test_flag_e'
 *                          in the ctest_dnx_field_user_actions.h file
 * \param [in] user_action_size - The size of user create action
 * \param [in] stage_for_size - The field_stage of the action needed to get the action size.
 *
 * \param [in/out] bcm_action_id_p - Pointer to bcm_action_id
 *        as in - in case flag WITH_ID is set, will hold the bcm_action ID
 *        as out - in case flag WITH_ID is not set, will return the created bcm_action ID
 *
 * \param [out] dnx_action_id_p - Pointer to dnx_action_id
 * \param [out] action_info_p - pointer to database info that is to be filled
 *                           in by this procedure. For details, see 'dnx_field_action_in_info_t'
 *
 * \return
 * For positive test:
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 * For negative test:
 *   Setting the error with SHR_SET_CURRENT_ERR macro
 *
 */
static shr_error_e
ctest_dnx_field_user_actions_create(
    int unit,
    dnx_field_action_flags_e user_create_action_flags,
    dnx_field_stage_e field_stage,
    bcm_field_action_t bcm_action,
    ctest_dnx_field_user_actions_test_flag_e test_flag,
    uint32 user_action_size,
    dnx_field_stage_e stage_for_size,
    bcm_field_action_t * bcm_action_id_p,
    dnx_field_action_t * dnx_action_id_p,
    dnx_field_action_in_info_t * action_info_p)
{
    int rv;
    dnx_field_action_t base_dnx_action;
    unsigned int base_action_size;

    SHR_FUNC_INIT_VARS(unit);
     /**
      * NULL check for all pointers
      */
    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_p");
    SHR_NULL_CHECK(dnx_action_id_p, _SHR_E_PARAM, "dnx_action_id_p");

    /** Get the DNX action. */
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, stage_for_size, bcm_action, &base_dnx_action));
    /** Get the size of the base DNX action.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, stage_for_size, base_dnx_action, &base_action_size));
    /**
     * If the case is CTEST_DNX_FIELD_USER_ACTIONS_NULL_CHECK
     * set the action_info_p to NULL and save the result of init function in 'rv'
     * In order to remove the errors from the screen in quite mode
     */
    rv = BCM_E_NONE;
    if (test_flag == CTEST_DNX_FIELD_USER_ACTIONS_NULL_CHECK)
    {
        action_info_p = NULL;
        rv = dnx_field_action_in_info_t_init(unit, action_info_p);
        if (rv != BCM_E_NONE)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }
     /**
      * If the case is different than CTEST_DNX_FIELD_USER_QUALS_NULL_CHECK
      * Init all of the parameters in the structure dnx_field_qualifier_in_info_t
      * Fill the structure and call the API qual_create
      */
    else
    {
          /**
           * Init all of the parameters in the structure dnx_field_presel_entry_data_t
           */
        SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, action_info_p));
          /**
           * Fill the structure needed for dnx_field_action_create API
           */
        action_info_p->stage = field_stage;
        action_info_p->bcm_action = bcm_action;
        action_info_p->size = user_action_size;
        action_info_p->prefix_size = base_action_size - user_action_size;
        /**
         * Calling the API dnx_field_action_create for positive case
         */
        if (test_flag == CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST)
        {
            sal_strncpy_s(action_info_p->name, "User_defined_action", sizeof(action_info_p->name));
            SHR_IF_ERR_EXIT(dnx_field_action_create
                            (unit, user_create_action_flags, action_info_p, bcm_action_id_p, dnx_action_id_p));
        }
        /**
         * In all negative cases the error value is needed to be compare with
         * the expected error located in Ctest_dnx_field_user_quals_expected_errors array.
         * So the error is set using SHR_SET_CURRENT_ERR.
         */
        else
        {
            rv = dnx_field_action_create(unit, user_create_action_flags, action_info_p, bcm_action_id_p,
                                         dnx_action_id_p);
            if (rv != BCM_E_NONE)
            {
                SHR_SET_CURRENT_ERR(rv);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  This procedure is used by appl_dnx_field_user_actions_semantic.
 *  It contains the dnx_field_action_create application.
 *  This procedure sets all required HW configuration for
 *  bcm_field_action_create to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] user_create_action_flags - Flags of dnx_field_action_create API.
 *             For now there is only one flag: BCM_FIELD_FLAG_WITH_ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] bcm_action - The BCM action upon which the user defined action is based.
 * \param [in] test_flag -  Used for negative cases
 *                          For details see 'ctest_dnx_field_user_actions_test_flag_e'
 *                          in the ctest_dnx_field_user_actions.h file
 * \param [in] user_action_size - The size of user create action
 * \param [in] stage_for_size - The field_stage of the action needed to get the action size.
 * \param [in/out] bcm_action_id_p - Pointer to bcm_action_id
 *        as in - in case flag WITH_ID is set, will hold the bcm_action ID
 *        as out - in case flag WITH_ID is not set, will return the created bcm_action ID
 *
 * \param [out] action_info_p - pointer to database info that is to be filled
 *                           in by this procedure. For details, see 'bcm_field_action_in_info_t'
 *
 * \return
 * For positive test:
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 * For negative test:
 *   Setting the error with SHR_SET_CURRENT_ERR macro
 *
 */
static shr_error_e
ctest_dnx_field_user_actions_create_bcm(
    int unit,
    uint32 user_create_action_flags,
    bcm_field_stage_t field_stage,
    bcm_field_action_t bcm_action,
    ctest_dnx_field_user_actions_test_flag_e test_flag,
    uint32 user_action_size,
    dnx_field_stage_e stage_for_size,
    bcm_field_action_t * bcm_action_id_p,
    bcm_field_action_info_t * action_info_p)
{
    int rv;
    dnx_field_action_t base_dnx_action;
    unsigned int base_action_size;

    SHR_FUNC_INIT_VARS(unit);
     /**
      * NULL check for all pointers
      */
    SHR_NULL_CHECK(action_info_p, _SHR_E_PARAM, "action_info_p");
    SHR_NULL_CHECK(bcm_action_id_p, _SHR_E_PARAM, "bcm_action_id_p");

    rv = BCM_E_NONE;

    /** Get the DNX action. */
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, stage_for_size, bcm_action, &base_dnx_action));
    /** Get the size of the base DNX action.*/
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, stage_for_size, base_dnx_action, &base_action_size));

    /**
    * Init all of the parameters in the structure bcm_field_presel_entry_data_t
    */
    bcm_field_action_info_t_init(action_info_p);
    /**
    * Fill the structure needed for bcm_field_qualifier_create API
    */
    action_info_p->stage = field_stage;
    action_info_p->action_type = bcm_action;
    action_info_p->size = user_action_size;
    action_info_p->prefix_size = base_action_size - user_action_size;
    /**
     * Calling the API bcm_field_action_create for positive case
     */
    if (test_flag == CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST)
    {
        sal_strncpy_s((char *) (action_info_p->name), "User_defined_action", sizeof(action_info_p->name));
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, user_create_action_flags, action_info_p, bcm_action_id_p));
    }
    /**
     * In all negative cases the error value is needed to be compare with
     * the expected error located in Ctest_dnx_field_user_quals_expected_errors array.
     * So the error is set using SHR_SET_CURRENT_ERR.
     */
    else
    {
        rv = bcm_field_action_create(unit, user_create_action_flags, action_info_p, bcm_action_id_p);
        if (rv != BCM_E_NONE)
        {
            SHR_SET_CURRENT_ERR(rv);
        }
    }
exit:
    SHR_FUNC_EXIT;
}
 /**
  * \brief
  *
  * This procedure is used by appl_dnx_field_user_actions_semantic()
  *    to compare between info corresponding to 'set' and to 'get'
  *
  * \param [in] unit - The unit number.
  * \param [in] dnx_action_id - pointer to dnx_action
  * \param [in] set_action_info_p - pointer to SET database info from dnx_field_action_create
  * \return
  *   Error code (as per 'bcm_error_e').
  * \see
  *   bcm_error_e
  *
  */
static shr_error_e
ctest_dnx_field_user_actions_compare(
    int unit,
    bcm_field_action_t bcm_action_id,
    dnx_field_action_in_info_t * set_action_info_p)
{
    dnx_field_action_in_info_t get_action_info;
    SHR_FUNC_INIT_VARS(unit);
     /**
      * Init all of the parameters in the structure dnx_field_presel_entry_data_t
      */
    SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &get_action_info));
    SHR_IF_ERR_EXIT(dnx_field_action_get(unit, bcm_action_id, &get_action_info));
     /**
      * Start the compare of the fields
      */
    if (set_action_info_p->stage != get_action_info.stage)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "stage expected %d received %d.\n", set_action_info_p->stage, get_action_info.stage);
    }
    if (get_action_info.bcm_action != set_action_info_p->bcm_action)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "base_action expected %d received %d.\n", set_action_info_p->bcm_action,
                     get_action_info.bcm_action);
    }
    if (set_action_info_p->size != get_action_info.size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "size expected %d received %d.\n", set_action_info_p->size, get_action_info.size);
    }
    if (strncmp(set_action_info_p->name, get_action_info.name, CTEST_DNX_FIELD_USER_ACTION_NAME_LENGTH) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "name expected %s received %s.\n", set_action_info_p->name, get_action_info.name);
    }
    LOG_INFO_EX(BSL_LOG_MODULE, " Done.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *
 * This procedure is used by appl_dnx_field_user_actions_semantic()
 *    to compare between info corresponding to 'set' and to 'get'
 *
 * \param [in] unit - The unit number.
 * \param [in] dnx_action_id - pointer to dnx_action
 * \param [in] set_action_info_p - pointer to SET database info from bcm_field_action_create
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
ctest_dnx_field_user_actions_compare_bcm(
    int unit,
    bcm_field_action_t bcm_action_id,
    bcm_field_action_info_t * set_action_info_p)
{
    bcm_field_action_info_t get_action_info;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Init all of the parameters in the structure bcm_field_presel_entry_data_t
     */
    bcm_field_action_info_t_init(&get_action_info);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, bcm_action_id, bcmFieldStageCount, &get_action_info));
    /**
     * Start the compare of the fields
     */
    if (set_action_info_p->stage != get_action_info.stage)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "stage expected %d received %d.\n", set_action_info_p->stage, get_action_info.stage);
    }
    if (get_action_info.action_type != set_action_info_p->action_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "base_action expected %d received %d.\n", set_action_info_p->action_type,
                     get_action_info.action_type);
    }
    if (set_action_info_p->size != get_action_info.size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "size expected %d received %d.\n", set_action_info_p->size, get_action_info.size);
    }
    if (strncmp
        ((char *) set_action_info_p->name, (char *) get_action_info.name, CTEST_DNX_FIELD_USER_ACTION_NAME_LENGTH) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_action_get: Unexpected data returned.\n"
                     "name expected %s received %s.\n", set_action_info_p->name, get_action_info.name);
    }
    LOG_INFO_EX(BSL_LOG_MODULE, " Done.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}
 /**
  * \brief
  *
  *  This procedure is used for positive testing
  *  The test flow:
  *  1. Create an action without ID
  *  2. Create an action with ID
  *  3. Compare the set and get fields
  *  4. Print the name of the action
  *  5. Delete the created action
  *  6. Verify after delete
  *
  * \param [in] unit - The unit number.
  * \param [in] field_stage - Indicate the PMF stage
  * \param [in] action_info_p - pointer to SET database info from dnx_field_action_create
  *
  * \return
  *  Error code (as per 'bcm_error_e').
  * \see
  *  bcm_error_e
  *
  */

static shr_error_e
appl_dnx_field_user_actions_positive_test(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_info_t * action_info_p)
{
    dnx_field_action_flags_e user_create_action_flags;

    dnx_field_action_t dnx_action_id;
    bcm_field_action_t bcm_action_id;
    int action_indx;
    int user_action_size;
    int rv;
    bcm_field_action_t bcm_base_action;
    SHR_FUNC_INIT_VARS(unit);
    user_action_size = CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE;
     /**
      *  1a. Create an action without ID
      *  1b. Create an action with ID
      *  1c. Create a void action with ID
      *  2. Compare the set and get fields
      *  3. Print the name of the action
      *  4. Delete the created action
      *  5. Verify after delete
      */
    for (action_indx = CTEST_DNX_FIELD_USER_ACTIONS_FIRST; action_indx < CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_NOF;
         action_indx++)
    {
         /**
          * Creating action without ID
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Creating an action(%d) \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_action_id);
        if (action_indx == CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_VOID)
        {
            bcm_base_action = bcmFieldActionVoid;
        }
        else
        {
            bcm_base_action = Ctest_dnx_field_user_actions_action_array[field_stage];
        }
        if (action_indx == CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_WITH_ID)
        {
            user_create_action_flags = DNX_FIELD_ACTION_FLAG_WITH_ID;
            bcm_action_id = CTEST_DNX_FIELD_USER_ACTION_ID;
        }
        else
        {
            user_create_action_flags = 0;
        }
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_actions_create(unit, user_create_action_flags, field_stage,
                                                            bcm_base_action,
                                                            CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST,
                                                            user_action_size + action_indx, field_stage,
                                                            &bcm_action_id, &dnx_action_id, action_info_p));

        /**
         * Compare the get elements and the set elements
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Performing compare between the add and get: %d \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_action_id);
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_actions_compare(unit, bcm_action_id, action_info_p));
         /**
          * Print the action
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .The Action name: %s \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_field_dnx_action_text(unit, dnx_action_id));
         /**
          * Delete the created action
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Delete the action with ID: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_action_id);
        SHR_IF_ERR_EXIT(dnx_field_action_destroy(unit, bcm_action_id));
         /**
          * Verify after delete
          */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Verify after delete: %d \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_action_id);
        rv = dnx_field_action_get(unit, bcm_action_id, action_info_p);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s Action: %d has been deleted successfully \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_action_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Action with ID (%d) exists after having been deleted!\r\n", dnx_action_id);
        }
    }
   /**
    * Adding clean up function
    */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
exit:
    SHR_FUNC_EXIT;
}
/**
  * \brief
  *
  *  This procedure is used for positive testing
  *  The test flow:
  *  1. Create an action without ID
  *  2. Create an action with ID
  *  3. Compare the set and get fields
  *  4. Print the name of the action
  *  5. Delete the created action
  *  6. Verify after delete
  *
  * \param [in] unit - The unit number.
  * \param [in] bcm_field_stage - Indicate the BCM PMF stage
  * \param [in] dnx_field_stage - Indicate the DNX PMF stage
  * \param [in] action_info_p - pointer to SET database info from bcm_field_action_create
  *
  * \return
  *  Error code (as per 'bcm_error_e').
  * \see
  *  bcm_error_e
  *
  */

static shr_error_e
appl_dnx_field_user_actions_positive_test_bcm(
    int unit,
    bcm_field_stage_t bcm_field_stage,
    dnx_field_stage_e dnx_field_stage,
    bcm_field_action_info_t * action_info_p)
{
    uint32 user_create_action_flags;

    bcm_field_action_t bcm_action_id;
    int action_indx;
    int user_action_size;
    int rv;
    bcm_field_action_t bcm_base_action;
    SHR_FUNC_INIT_VARS(unit);
    user_action_size = CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE;
     /**
      *  1a. Create an action without ID
      *  1b. Create an action with ID
      *  1c. Create a void action with ID
      *  2. Compare the set and get fields
      *  3. Print the name of the action
      *  4. Delete the created action
      *  5. Verify after delete
      */
    for (action_indx = CTEST_DNX_FIELD_USER_ACTIONS_FIRST; action_indx < CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_NOF;
         action_indx++)
    {
         /**
          * Creating action without ID
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Creating an action(%d) \r\n",
                    __func__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), bcm_action_id);
        if (action_indx == CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_VOID)
        {
            bcm_base_action = bcmFieldActionVoid;
        }
        else
        {
            bcm_base_action = Ctest_dnx_field_user_actions_action_array[dnx_field_stage];
        }
        if (action_indx == CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_WITH_ID)
        {
            user_create_action_flags = BCM_FIELD_FLAG_WITH_ID;
            bcm_action_id = CTEST_DNX_FIELD_USER_ACTION_ID;
        }
        else
        {
            user_create_action_flags = 0;
        }
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_actions_create_bcm(unit, user_create_action_flags, bcm_field_stage,
                                                                bcm_base_action,
                                                                CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST,
                                                                user_action_size + action_indx, dnx_field_stage,
                                                                &bcm_action_id, action_info_p));

        /**
         * Compare the get elements and the set elements
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Performing compare between the add and get: %d \r\n", __func__,
                    __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), bcm_action_id);
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_actions_compare_bcm(unit, bcm_action_id, action_info_p));
        /**
         * Compare expected HW value and mapped HW value
         */
        if (action_indx != CTEST_DNX_FIELD_USER_ACTIONS_POSITIVE_TEST_VOID)
        {
            uint32 bcm_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS];
            uint32 hw_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS];
            uint32 expected_val;
            int ii;
            for (ii = 0; ii < BCM_FIELD_ACTION_WIDTH_IN_WORDS; ii++)
            {
                bcm_value[ii] = 0;
                hw_value[ii] = 0;
            }

            if (bcm_field_stage != bcmFieldStageEgress)
            {
                bcm_gport_t snoop_gport;
                int snoop_code = 0x5;
                int snoop_str = 0x3;

                BCM_GPORT_TRAP_SET(snoop_gport, snoop_code, 0, snoop_str);
                bcm_value[0] = snoop_gport;
                expected_val = 0x605;   /* This is the value we expect */
            }
            else
            {
                bcm_value[0] = 0x3;
                expected_val = 0x3;
            }

            SHR_IF_ERR_EXIT(bcm_field_action_value_map(unit, bcm_field_stage, bcm_base_action, bcm_value, hw_value));

            if (expected_val != hw_value[0])
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Action mapping failure, Snoop mapped val: 0x%x, expected: 0x%x, received: 0x%x\r\n",
                             bcm_value[0], expected_val, hw_value[0]);
            }
        }
         /**
          * Print the action
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .The Action name: %s \r\n", __func__,
                    __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), dnx_field_bcm_action_text(unit,
                                                                                                   bcm_action_id));
         /**
          * Delete the created action
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Delete the action with ID: %d \r\n",
                    __func__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), bcm_action_id);
        SHR_IF_ERR_EXIT(bcm_field_action_destroy(unit, bcm_action_id));
         /**
          * Verify after delete
          */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Verify after delete: %d \r\n", __func__,
                    __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), bcm_action_id);
        rv = bcm_field_action_info_get(unit, bcm_action_id, bcmFieldStageCount, action_info_p);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s Action: %d has been deleted successfully \r\n",
                        __func__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), bcm_action_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Action with ID (%d) exists after having been deleted!\r\n", bcm_action_id);
        }
    }
   /**
    * Adding clean up function
    */
    SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
exit:
    SHR_FUNC_EXIT;
}
 /**
  * \brief
  *
  *  This procedure is used for creating 6 negative case
  *
  *
  * \param [in] unit - The unit number.
  * \param [in] field_stage - Indicate the PMF stage
  * \param [in] set_action_info_p - Pointer to SET database info from dnx_field_action_create
  * \param [in] expected_error_p - Pointer to array of expected errors
  *           The size of this array is: CTEST_DNX_FIELD_USER_ACTIONS_NOF-1
  *                                     (the number of all negative cases)
  * \return
  *   Error code (as per 'bcm_error_e').
  * \see
  *   bcm_error_e
  */
static shr_error_e
appl_dnx_field_user_actions_negative_test(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_info_t * action_info_p,
    _shr_error_t * expected_error_p)
{
    dnx_field_action_flags_e user_create_action_flags;
    dnx_field_action_t dnx_action_id;
    bcm_field_action_t bcm_action_id;
    bcm_field_action_t negative_test_actions;
    dnx_field_stage_e stage_to_size;
    int action_indx;
    uint32 user_action_size;
    int rv;
     /**
      * Used to give specific number of iteration in the for loop.
      * The for loop is calling the ctest_dnx_field_user_actions_create 'nof_times_to_iterate'
      * number of times for different negative cases.
      */
    int nof_times_to_iterate;
    ctest_dnx_field_user_actions_test_flag_e negative_case_iterator;
    SHR_FUNC_INIT_VARS(unit);
     /**
      * Iterating all of the negative cases.
      * From the first one till NOF
      * switching between the cases
      * Call the ctest_dnx_field_user_actions_create function
      * return proper ERROR
      *
      */
    for (negative_case_iterator = CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_ID;
         negative_case_iterator < CTEST_DNX_FIELD_USER_ACTIONS_NOF; negative_case_iterator++)
    {
        stage_to_size = field_stage;
        negative_test_actions = Ctest_dnx_field_user_actions_action_array[field_stage];
        user_action_size = CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE;
        user_create_action_flags = 0;
        rv = BCM_E_NONE;
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING! CASE: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
         /**
          * Switching between the negative cases
          * and giving the specific value for each of them
          */
        switch (negative_case_iterator)
        {
            case CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_ID:
            {
                nof_times_to_iterate = 1;
                user_create_action_flags = DNX_FIELD_ACTION_FLAG_WITH_ID;
                bcm_action_id = CTEST_DNX_FIELD_USER_ACTION_ILLEGAL_ACTION_ID;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_DIFFERENT_STAGE:
            {
                nof_times_to_iterate = 1;
                negative_test_actions = Ctest_dnx_field_user_actions_illegal_action_array[field_stage];
                if (field_stage != DNX_FIELD_STAGE_EPMF)
                {
                    stage_to_size = DNX_FIELD_STAGE_EPMF;
                }
                else
                {
                    stage_to_size = DNX_FIELD_STAGE_IPMF1;
                }
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_SIZE:
            {
                nof_times_to_iterate = 1;
                user_action_size = CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE_INVALID;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_SAME_ID:
            {
                user_create_action_flags = DNX_FIELD_ACTION_FLAG_WITH_ID;
                bcm_action_id = CTEST_DNX_FIELD_USER_ACTION_ID;
                nof_times_to_iterate = 2;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_MAX_NOF_ACTIONS:
            {
                nof_times_to_iterate = CTEST_DNX_USER_ACTIONS_MAX_NOF_ACTIONS;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_NULL_CHECK:
            {
                nof_times_to_iterate = 1;
                break;
            }
            default:
            {
                 /**
                  * None of the supported cases
                  */
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Negative case:%d \n", negative_case_iterator);
                break;
            }
        }

        /**
         *  The begging of the region of error recovery transaction
         *  Used for clean-up in negative case
         */
        DNX_ROLLBACK_JOURNAL_START(unit);

        /**
         * The for loop is calling the ctest_dnx_field_user_actions_create 'nof_times_to_iterate'
         * number of times for different negative cases.
         */
        for (action_indx = 0; action_indx < nof_times_to_iterate; action_indx++)
        {
             /**
              * Calling the ctest_dnx_field_user_actions_create which is calling the
              * API user_action_create function
              */
            rv = ctest_dnx_field_user_actions_create(unit, user_create_action_flags, field_stage,
                                                     negative_test_actions,
                                                     negative_case_iterator, user_action_size,
                                                     stage_to_size, &bcm_action_id, &dnx_action_id, action_info_p);
        }

        /**
         * The end of the region of error recovery transaction
         */
        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

         /**
          * Giving the proper error message for different negative cases
          */
        if (rv == expected_error_p[negative_case_iterator - 1])
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TEST CASE: %d was successful! \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Illegal action from negative test case: %d was created! Test has failed!\r\n",
                         negative_case_iterator);
        }
        SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *
 *  This procedure is used for creating 6 negative case
 *
 *
 * \param [in] unit - The unit number.
 * \param [in] bcm_field_stage - Indicate the BCM PMF stage
 * \param [in] dnx_field_stage - Indicate the DNX PMF stage
 * \param [in] set_action_info_p - Pointer to SET database info from bcm_field_action_create
 * \param [in] expected_error_p - Pointer to array of expected errors
 *           The size of this array is: CTEST_DNX_FIELD_USER_ACTIONS_NOF-1
 *                                     (the number of all negative cases)
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_field_user_actions_negative_test_bcm(
    int unit,
    bcm_field_stage_t bcm_field_stage,
    dnx_field_stage_e dnx_field_stage,
    bcm_field_action_info_t * action_info_p,
    _shr_error_t * expected_error_p)
{
    uint32 user_create_action_flags;
    bcm_field_action_t bcm_action_id;
    bcm_field_action_t negative_test_actions;
    int action_indx;
    uint32 user_action_size;
    int rv;
    dnx_field_stage_e stage_to_size;
    /**
     * Used to give specific number of iteration in the for loop.
     * The for loop is calling the ctest_dnx_field_user_actions_create_bcm 'nof_times_to_iterate'
     * number of times for different negative cases.
     */
    int nof_times_to_iterate;
    ctest_dnx_field_user_actions_test_flag_e negative_case_iterator;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Iterating all of the negative cases.
     * From the first one till NOF
     * switching between the cases
     * Call the ctest_dnx_field_user_actions_create_bcm function
     * return proper ERROR
     *
     */
    for (negative_case_iterator = CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_ID;
         negative_case_iterator < CTEST_DNX_FIELD_USER_ACTIONS_NOF - 1; negative_case_iterator++)
    {
        stage_to_size = dnx_field_stage;
        negative_test_actions = Ctest_dnx_field_user_actions_action_array[dnx_field_stage];
        user_action_size = CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE;
        user_create_action_flags = 0;
        rv = BCM_E_NONE;
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING! CASE: %d \r\n",
                    __func__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), negative_case_iterator);
        /**
         * Switching between the negative cases
         * and giving the specific value for each of them
         */
        switch (negative_case_iterator)
        {
            case CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_ID:
            {
                nof_times_to_iterate = 1;
                user_create_action_flags = BCM_FIELD_FLAG_WITH_ID;
                bcm_action_id = CTEST_DNX_FIELD_USER_ACTION_ILLEGAL_ACTION_ID;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_DIFFERENT_STAGE:
            {
                nof_times_to_iterate = 1;
                negative_test_actions = Ctest_dnx_field_user_actions_illegal_action_array[dnx_field_stage];
                if (dnx_field_stage != DNX_FIELD_STAGE_EPMF)
                {
                    stage_to_size = DNX_FIELD_STAGE_EPMF;
                }
                else
                {
                    stage_to_size = DNX_FIELD_STAGE_IPMF1;
                }
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_ILLEGAL_SIZE:
            {
                nof_times_to_iterate = 1;
                user_action_size = CTEST_DNX_FIELD_USER_ACTIONS_USER_ACTION_SIZE_INVALID;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_SAME_ID:
            {
                user_create_action_flags = BCM_FIELD_FLAG_WITH_ID;
                bcm_action_id = CTEST_DNX_FIELD_USER_ACTION_ID;
                nof_times_to_iterate = 2;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_MAX_NOF_ACTIONS:
            {
                nof_times_to_iterate = CTEST_DNX_USER_ACTIONS_MAX_NOF_ACTIONS;
                break;
            }
            case CTEST_DNX_FIELD_USER_ACTIONS_NULL_CHECK:
            default:
            {
                /**
                 * None of the supported cases
                 */
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Negative case:%d \n", negative_case_iterator);
                break;
            }
        }
       /**
        *  The begging of the region of error recovery transaction
        *  Used for clean-up in negative case
        */
        DNX_ROLLBACK_JOURNAL_START(unit);

       /**
        * The for loop is calling the ctest_dnx_field_user_actions_create_bcm 'nof_times_to_iterate'
        * number of times for different negative cases.
        */
        for (action_indx = 0; action_indx < nof_times_to_iterate; action_indx++)
        {
            /**
             * Calling the ctest_dnx_field_user_actions_create_bcm which is calling the
             * API user_action_create function
             */
            rv = ctest_dnx_field_user_actions_create_bcm(unit, user_create_action_flags, bcm_field_stage,
                                                         negative_test_actions,
                                                         negative_case_iterator, user_action_size, stage_to_size,
                                                         &bcm_action_id, action_info_p);
        }

       /**
        * The end of the region of error recovery transaction
        */
        DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

        /**
         * Giving the proper error message for different negative cases
         */
        if (rv == expected_error_p[negative_case_iterator - 1])
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TEST CASE: %d was successful! \r\n",
                        __func__, __LINE__, dnx_field_bcm_stage_text(bcm_field_stage), negative_case_iterator);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Illegal action from negative test case: %d was created! Test has failed!\r\n",
                         negative_case_iterator);
        }
        SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *
 * This function is basic 'dnx_field_action_create'
 * (depending on the test mode) testing application.
 *
 * \param [in] unit         - Device ID
 * \param [in] args         - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 *
 *  The test flow:
 *  1. Calling the positive test function
 *  2. Calling the negative test function
 *
 */
static shr_error_e
appl_dnx_field_user_actions_semantic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e dnx_field_stage;
    bcm_field_stage_t bcm_field_stage;
    int field_group_test_type_name;
    dnx_field_action_in_info_t dnx_action_info;
    bcm_field_action_info_t bcm_action_info;
   /**
    * This variables stores the sevirity of the Field processor dnx
    */
    bsl_severity_t original_severity_fldprocdnx;
   /**
    * This variables stores the sevirity of the Resource manager
    */
    bsl_severity_t original_severity_resmngr;
    /**
     * This variables stores the sevirity of the DBAL
     */
    bsl_severity_t original_severity_dbaldnx;
    /**
     * This variables stores the sevirity of the HASH
     */
    bsl_severity_t original_severity_hashdnx;
    /**
     * This variables stores the sevirity of the SW State
     */
    bsl_severity_t original_severity_swstate;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    /**
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_USER_ACTIONS_TYPE, field_group_test_type_name);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_USER_ACTIONS_TEST_STAGE, dnx_field_stage);
    /**
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (field_group_test_type_name == 0)
    {
        /** Convert DNX to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));
        /**
        * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
        */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
        /**
        * Calling the positive function
        */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_actions_positive_test_bcm
                        (unit, bcm_field_stage, dnx_field_stage, &bcm_action_info));
        /**
        * Calling negative test function
        */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_actions_negative_test_bcm
                        (unit, bcm_field_stage, dnx_field_stage, &bcm_action_info,
                         Ctest_dnx_field_user_actions_expected_errors));
        /**
        *  Restore the original severity after the end of Negative test.
        */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    }
    else if (field_group_test_type_name == 1)
    {
        /**
         * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
        /**
         * Calling the positive function
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_actions_positive_test(unit, dnx_field_stage, &dnx_action_info));
        /**
         * Calling negative test function
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_actions_negative_test
                        (unit, dnx_field_stage, &dnx_action_info, Ctest_dnx_field_user_actions_expected_errors));
        /**
         *  Restore the original severity after the end of Negative test.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    }
exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    SHR_FUNC_EXIT;
}

/**
 * \brief - run user_actions init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx user_actions_start"
 */
shr_error_e
sh_dnx_field_user_actions_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_USER_ACTIONS_OPTION_TEST_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_user_actions_semantic(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

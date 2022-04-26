/** \file ctest_dnx_field_user_quals.c
 * $Id$
 *
 * Field User qual set/get usage example.
 * Positive test:
 *   1. Create an qual without ID
 *   2. Compare the set and get fields
 *   3. Print the name of the qual
 *   4. Delete the created qual
 *   5. Verify after delete
 *   6. Create an qual with ID and do 1-5
 * Negative test cases:
 *  1. Create an qual with ID out of range
 *  2. Create an qual with different stage then the supplied base_qual
 *  3. Create an qual with size bigger then base_qual size
 *  4. Create an qual with same ID
 *  5. Create more quals than dnx_data_field.qual.user_nof_get(unit)
 *  6. NULL check
 */
/**
 * $Copyright: (c) 2021 Broadcom.
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
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include "ctest_dnx_field_user_quals.h"
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
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
#define CTEST_DNX_FIELD_USER_QUALS_TYPE              "type"
/**
 * \brief
 *   Keyword for stage of test on user_quals command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define CTEST_DNX_FIELD_USER_QUALS_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for TCL testing, clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_USER_QUALS_CLEAN             "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_USER_QUALS_OPTION_TEST_COUNT       "count"
/**
 * \brief
 *   Options list for 'user_quals' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_user_quals_options[] = {
     /** Name */                                             /** Type */            /** Description */                     /** Default */
    {CTEST_DNX_FIELD_USER_QUALS_TYPE,                SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_USER_QUALS_TEST_STAGE,          SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",   "ipmf1",   (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_USER_QUALS_CLEAN,               SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {CTEST_DNX_USER_QUALS_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      
    /**
     * End of options list - must be last. 
     */
};
/**
 * \brief
 *   List of tests for 'user_quals' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_user_quals_tests[] = {
    {"DNX_field_quals_test_1", "type=DNX stage=ipmf1  count=1", CTEST_POSTCOMMIT},
    {"DNX_field_quals_test_2", "type=DNX stage=ipmf2  count=1", CTEST_POSTCOMMIT},
    {"DNX_field_quals_test_3", "type=DNX stage=ipmf3  count=1", CTEST_POSTCOMMIT},
    {"DNX_field_quals_test_4", "type=DNX stage=epmf   count=1", CTEST_POSTCOMMIT},
    {"BCM_field_quals_test_1", "type=BCM stage=ipmf1  count=1", CTEST_POSTCOMMIT},
    {"BCM_field_quals_test_2", "type=BCM stage=ipmf2  count=1", CTEST_POSTCOMMIT},
    {"BCM_field_quals_test_3", "type=BCM stage=ipmf3  count=1", CTEST_POSTCOMMIT},
    {"BCM_field_quals_test_4", "type=BCM stage=epmf   count=1", CTEST_POSTCOMMIT},
    {NULL}
};
/**
 *  user_quals shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_user_quals_man = {
"Field user create quals related test utilities",
    "Activate field user created quals related test utilities. "
    "BCM-level tests and DNX-level tests. \r\n"
        "Testing may be for various types: 'bcm' or 'dnx' \r\n"
        "Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n"
        "The 'count' variable defines how many times the test will run. \r\n",
    "ctest field user_quals type=<BCM | DNX> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> count = 1"
};
/**
 * Used in Negative cases for setting different error codes
 * in different cases
 */
static _shr_error_t Ctest_dnx_field_user_quals_expected_errors[CTEST_DNX_FIELD_USER_QUALS_NOF] = {
    /**
     * case1: CTEST_DNX_FIELD_USER_QAULS_ILLEGAL_ID
     */
    _SHR_E_INTERNAL,
    /**
     * case2: CTEST_DNX_FIELD_USER_QAULS_ILLEGAL_SIZE
     */
    _SHR_E_PARAM,
    /**
     * case3: CTEST_DNX_FIELD_USER_QAULS_SAME_ID
     */
    _SHR_E_PARAM,
    /**
     * case4: CTEST_DNX_FIELD_USER_QAULS_MAX_NOF_QUALS
     */
    _SHR_E_RESOURCE,
    /**
     * case5: CTEST_DNX_FIELD_USER_QAULS_NULL_CHECK
     */
    _SHR_E_PARAM

};
/* *INDENT-ON* */
/**
 * \brief
 *  This procedure is used by appl_dnx_field_user_quals_semantic.
 *  It contains the dnx_field_qual_create application.
 *  This procedure sets all required HW configuration for
 *  dnx_field_qual_create to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] user_create_qual_flags - Flags of dnx_field_qual_create API.
 *             For now there is only one flag: DNX_FIELD_QUALIFIER_FLAG_WITH_ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] test_flag -  Used for negative cases
 *                          For details see 'ctest_dnx_field_user_quals_test_flag_e'
 *                          in the ctest_dnx_field_user_quals.h file
 * \param [in] user_qual_size - The size of user create qual
 *
 * \param [in/out] bcm_qual_id_p - Pointer to bcm_qual_id
 *        as in - in case flag WITH_ID is set, will hold the bcm_qual ID
 *        as out - in case flag WITH_ID is not set, will return the created bcm_qual ID
 *
 * \param [out] dnx_qual_id_p - Pointer to dnx_qual_id
 * \param [out] qual_info_p - pointer to database info that is to be filled
 *                           in by this procedure. For details, see 'dnx_field_qual_in_info_t'
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
ctest_dnx_field_user_quals_create(
    int unit,
    dnx_field_qual_flags_e user_create_qual_flags,
    dnx_field_stage_e field_stage,
    ctest_dnx_field_user_quals_test_flag_e test_flag,
    uint32 user_qual_size,
    bcm_field_qualify_t * bcm_qual_id_p,
    dnx_field_qual_t * dnx_qual_id_p,
    dnx_field_qualifier_in_info_t * qual_info_p)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * NULL check for all pointers
     */
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");
    SHR_NULL_CHECK(dnx_qual_id_p, _SHR_E_PARAM, "dnx_qual_id_p");
    /**
     * If the case is CTEST_DNX_FIELD_USER_QUALS_NULL_CHECK
     * set the qual_info_p to NULL and save the result of init function in 'rv'
     * In order to remove the errors from the screen in  quite mode
     */
    rv = BCM_E_NONE;
    if (test_flag == CTEST_DNX_FIELD_USER_QUALS_NULL_CHECK)
    {
        qual_info_p = NULL;
        rv = dnx_field_qual_in_info_init(unit, qual_info_p);
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
        SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, qual_info_p));
          /**
           * Fill the structure needed for dnx_field_qual_create API
           */
        qual_info_p->size = user_qual_size;
        /**
         * Calling the API dnx_field_qual_create for positive test
         */
        if (test_flag == CTEST_DNX_FIELD_USER_QUALS_POSITIVE_TEST)
        {
            sal_strncpy_s(qual_info_p->name, "User_defined_qual", sizeof(qual_info_p->name));
            SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, user_create_qual_flags, qual_info_p, bcm_qual_id_p));
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, field_stage, *bcm_qual_id_p, dnx_qual_id_p));
        }
        /**
         * In all negative cases the error value is needed to be compare with
         * the expected error located in Ctest_dnx_field_user_quals_expected_errors array.
         * So the error is set using SHR_SET_CURRENT_ERR.
         */
        else
        {
            rv = dnx_field_qual_create(unit, user_create_qual_flags, qual_info_p, bcm_qual_id_p);

            if (rv != BCM_E_NONE)
            {
                SHR_SET_CURRENT_ERR(rv);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, field_stage, *bcm_qual_id_p, dnx_qual_id_p));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This procedure is used by appl_dnx_field_user_quals_semantic.
 *  It contains the dnx_field_qual_create application.
 *  This procedure sets all required HW configuration for
 *  bcm_field_qualifier_create to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] user_create_qual_flags - Flags of dnx_field_qual_create API.
 *             For now there is only one flag: BCM_FIELD_FLAG_WITH_ID
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] test_flag -  Used for negative cases
 *                          For details see 'ctest_dnx_field_user_quals_test_flag_e'
 *                          in the ctest_dnx_field_user_quals.h file
 * \param [in] user_qual_size - The size of user create qual
 *
 * \param [in/out] bcm_qual_id_p - Pointer to bcm_qual_id
 *        as in - in case flag WITH_ID is set, will hold the bcm_qual ID
 *        as out - in case flag WITH_ID is not set, will return the created bcm_qual ID
 *
 * \param [out] qual_info_p - pointer to database info that is to be filled
 *                           in by this procedure. For details, see 'bcm_field_qualifier_info_create_t'
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
ctest_dnx_field_user_quals_create_bcm(
    int unit,
    uint32 user_create_qual_flags,
    bcm_field_stage_t field_stage,
    ctest_dnx_field_user_quals_test_flag_e test_flag,
    uint32 user_qual_size,
    bcm_field_qualify_t * bcm_qual_id_p,
    bcm_field_qualifier_info_create_t * qual_info_p)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * NULL check for all pointers
     */
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");
    SHR_NULL_CHECK(bcm_qual_id_p, _SHR_E_PARAM, "bcm_qual_id_p");

    rv = BCM_E_NONE;

    bcm_field_qualifier_info_create_t_init(qual_info_p);

  /**
   * Fill the structure needed for bcm_dnx_field_qualifier_create API
   */
    qual_info_p->size = user_qual_size;
    /**
     * Calling the API bcm_field_qualifier_create for positive test
     */
    if (test_flag == CTEST_DNX_FIELD_USER_QUALS_POSITIVE_TEST)
    {
        sal_strncpy_s((char *) (qual_info_p->name), "User_defined_qual", sizeof(qual_info_p->name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, user_create_qual_flags, qual_info_p, bcm_qual_id_p));
    }
    /**
     * In all negative cases the error value is needed to be compare with
     * the expected error located in Ctest_dnx_field_user_quals_expected_errors array.
     * So the error is set using SHR_SET_CURRENT_ERR.
     */
    else
    {
        rv = bcm_field_qualifier_create(unit, user_create_qual_flags, qual_info_p, bcm_qual_id_p);
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
  * This procedure is used by appl_dnx_field_user_quals_semantic()
  *    to compare between info corresponding to 'set' and to 'get'
  *
  * \param [in] unit - The unit number.
  * \param [in] bcm_qual_id - pointer to bcm_qual
  * \param [in] set_qual_info_p - pointer to SET database info from dnx_field_qual_create
  * \return
  *   Error code (as per 'bcm_error_e').
  * \see
  *   bcm_error_e
  *
  */
static shr_error_e
ctest_dnx_field_user_quals_compare(
    int unit,
    bcm_field_qualify_t bcm_qual_id,
    dnx_field_qualifier_in_info_t * set_qual_info_p)
{
    dnx_field_qualifier_in_info_t get_qual_info;
    SHR_FUNC_INIT_VARS(unit);
     /**
      * Init all of the parameters in the structure dnx_field_presel_entry_data_t
      */
    SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &get_qual_info));
    SHR_IF_ERR_EXIT(dnx_field_qual_get(unit, bcm_qual_id, &get_qual_info));
    /**
     * Start the compare of the fields
     */
    if (set_qual_info_p->size != get_qual_info.size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_qual_get: Unexpected data returned.\n"
                     "size expected %d received %d.\n", set_qual_info_p->size, get_qual_info.size);
    }
    if (strncmp(set_qual_info_p->name, get_qual_info.name, CTEST_DNX_FIELD_USER_QUAL_NAME_LENGTH) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_qual_get: Unexpected data returned.\n"
                     "name expected %s received %s.\n", set_qual_info_p->name, get_qual_info.name);
    }
    LOG_INFO_EX(BSL_LOG_MODULE, " Done.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * This procedure is used by appl_dnx_field_user_quals_semantic()
 *    to compare between info corresponding to 'set' and to 'get'
 *
 * \param [in] unit - The unit number.
 * \param [in] bcm_qual_id - pointer to bcm_qual
 * \param [in] set_qual_info_p - pointer to SET database info from bcm_field_qualifier_create
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
ctest_dnx_field_user_quals_compare_bcm(
    int unit,
    bcm_field_qualify_t bcm_qual_id,
    bcm_field_qualifier_info_create_t * set_qual_info_p)
{
    bcm_field_qualifier_info_get_t get_qual_info;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Init all of the parameters in the structure bcm_field_qualifier_info_get_t
     */
    bcm_field_qualifier_info_get_t_init(&get_qual_info);
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcm_qual_id, bcmFieldStageCount, &get_qual_info));
    /**
     * Start the compare of the fields
     */
    if (set_qual_info_p->size != get_qual_info.size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "bcm_field_qualifier_info_get: Unexpected data returned.\n"
                     "size expected %d received %d.\n", set_qual_info_p->size, get_qual_info.size);
    }
    if (strncmp((char *) set_qual_info_p->name, (char *) get_qual_info.name, CTEST_DNX_FIELD_USER_QUAL_NAME_LENGTH) !=
        0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "bcm_field_qualifier_info_get: Unexpected data returned.\n"
                     "name expected %s received %s.\n", set_qual_info_p->name, get_qual_info.name);
    }
    if (get_qual_info.offset != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "bcm_field_qualifier_info_get: Unexpected data returned.\n"
                     "offset expected %d received %d.\n", 0, get_qual_info.size);
    }
    if (get_qual_info.qual_class != bcmFieldQualifierClassUserCreated)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "bcm_field_qualifier_info_get: Unexpected data returned.\n"
                     "qual_class expected %d received %d.\n", bcmFieldQualifierClassUserCreated, get_qual_info.size);
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
  *  1. Create an qual without ID
  *  2. Create an qual with ID
  *  3. Compare the set and get fields
  *  4. Print the name of the qual
  *  5. Delete the created qual
  *  6. Verify after delete
  *
  * \param [in] unit - The unit number.
  * \param [in] field_stage - Indicate the PMF stage
  * \param [in] qual_info_p - pointer to SET database info from dnx_field_qual_create
  *
  * \return
  *  Error code (as per 'bcm_error_e').
  * \see
  *  bcm_error_e
  *
  */
static shr_error_e
appl_dnx_field_user_quals_positive_test(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qualifier_in_info_t * qual_info_p)
{
    dnx_field_qual_flags_e user_create_qual_flags;

    dnx_field_qual_t dnx_qual_id;
    bcm_field_qualify_t bcm_qual_id;
    int qual_indx;
    int user_qual_size;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    user_qual_size = CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE;
    user_create_qual_flags = 0;
     /**
       *  1. Create an qual without ID
       *  2. Create an qual with ID
       *  3. Compare the set and get fields
       *  4. Print the name of the qual
       *  5. Delete the created qual
       *  6. Verify after delete
       */
    for (qual_indx = 0; qual_indx < CTEST_DNX_FIELD_USER_QUAL_NOF_QUALS; qual_indx++)
    {
         /**
          * Creating qual without ID
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Creating an qual(%d) \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_qual_id);
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_quals_create(unit, user_create_qual_flags, field_stage,
                                                          CTEST_DNX_FIELD_USER_QUALS_POSITIVE_TEST, user_qual_size,
                                                          &bcm_qual_id, &dnx_qual_id, qual_info_p));
        /**
         * Compare the get elements and the set elements
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Performing compare between the add and get: %d \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_qual_id);
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_quals_compare(unit, bcm_qual_id, qual_info_p));
         /**
          * Print the qual
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .The qual name: %s \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_field_dnx_qual_text(unit, dnx_qual_id));
         /**
          * Delete the created Qual
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Delete the qual with ID: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_qual_id);
        SHR_IF_ERR_EXIT(dnx_field_qual_destroy(unit, bcm_qual_id));
         /**
          * Verify after delete
          */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Verify after delete: %d \r\n", __func__,
                    __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_qual_id);
        rv = dnx_field_qual_get(unit, bcm_qual_id, qual_info_p);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .QUAL: %d has been deleted successfully \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), dnx_qual_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Qual with ID (%d) exists after having been deleted!\r\n", dnx_qual_id);
        }
         /**
          * Creating qual with ID for next iteration
          */
        user_create_qual_flags = DNX_FIELD_QUALIFIER_FLAG_WITH_ID;
        bcm_qual_id = CTEST_DNX_FIELD_USER_QUAL_ID;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
  * \brief
  *
  *  This procedure is used for positive testing
  *  The test flow:
  *  1. Create an qual without ID
  *  2. Create an qual with ID
  *  3. Compare the set and get fields
  *  4. Print the name of the qual
  *  5. Delete the created qual
  *  6. Verify after delete
  *
  * \param [in] unit - The unit number.
  * \param [in] field_stage - Indicate the PMF stage
  * \param [in] qual_info_p - pointer to SET database info from bcm_field_qualifier_create
  *
  * \return
  *  Error code (as per 'bcm_error_e').
  * \see
  *  bcm_error_e
  *
  */
static shr_error_e
appl_dnx_field_user_quals_positive_test_bcm(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_qualifier_info_create_t * qual_info_p)
{
    uint32 user_create_qual_flags;

    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_qualify_t bcm_qual_id;
    int qual_indx;
    int user_qual_size;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    user_qual_size = CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE;
    user_create_qual_flags = 0;
     /**
       *  1. Create an qual without ID
       *  2. Create an qual with ID
       *  3. Compare the set and get fields
       *  4. Print the name of the qual
       *  5. Delete the created qual
       *  6. Verify after delete
       */
    for (qual_indx = 0; qual_indx < CTEST_DNX_FIELD_USER_QUAL_NOF_QUALS; qual_indx++)
    {
         /**
          * Creating qual without ID
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Creating an qual(%d) \r\n",
                    __func__, __LINE__, dnx_field_bcm_stage_text(field_stage), bcm_qual_id);
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_quals_create_bcm(unit, user_create_qual_flags, field_stage,
                                                              CTEST_DNX_FIELD_USER_QUALS_POSITIVE_TEST, user_qual_size,
                                                              &bcm_qual_id, qual_info_p));
        /**
         * Compare the get elements and the set elements
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Performing compare between the add and get: %d \r\n", __func__,
                    __LINE__, dnx_field_bcm_stage_text(field_stage), bcm_qual_id);
        SHR_IF_ERR_EXIT(ctest_dnx_field_user_quals_compare_bcm(unit, bcm_qual_id, qual_info_p));
         /**
          * Print the qual
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .The qual name: %s \r\n", __func__,
                    __LINE__, dnx_field_bcm_stage_text(field_stage), dnx_field_bcm_qual_text(unit, bcm_qual_id));
         /**
          * Delete the created Qual
          */
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Delete the qual with ID: %d \r\n",
                    __func__, __LINE__, dnx_field_bcm_stage_text(field_stage), bcm_qual_id);
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id));
         /**
          * Verify after delete
          */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Verify after delete: %d \r\n", __func__,
                    __LINE__, dnx_field_bcm_stage_text(field_stage), bcm_qual_id);
        rv = bcm_field_qualifier_info_get(unit, bcm_qual_id, bcmFieldStageCount, &qual_info_get);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .QUAL: %d has been deleted successfully \r\n",
                        __func__, __LINE__, dnx_field_bcm_stage_text(field_stage), bcm_qual_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Qual with ID (%d) exists after having been deleted!\r\n", bcm_qual_id);
        }
         /**
          * Creating qual with ID for next iteration
          */
        user_create_qual_flags = DNX_FIELD_QUALIFIER_FLAG_WITH_ID;
        bcm_qual_id = CTEST_DNX_FIELD_USER_QUAL_ID;
    }

    /** Check value_map (not related to the created qual itself */
    {
        uint32 check_val = 3;
        uint32 expected_val = 2;
        uint32 bcm_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
        uint32 hw_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS];
        bcm_value[0] = check_val;
        SHR_IF_ERR_EXIT(bcm_field_qualifier_value_map(unit, field_stage, bcmFieldQualifyColor, bcm_value, hw_value));
        if (hw_value[0] != expected_val)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Qualifier mapping is not working, DP mapped val: %d, expected: %d, received: %d\r\n",
                         check_val, expected_val, hw_value[0]);
        }
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
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] set_qual_info_p - Pointer to SET database info from dnx_field_qual_create
 * \param [in] expected_error_p - Pointer to array of expected errors
 *           The size of this array is: CTEST_DNX_FIELD_USER_QUALS_NOF-1
 *                                     (the number of all negative cases)
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_field_user_quals_negative_test(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qualifier_in_info_t * qual_info_p,
    _shr_error_t * expected_error_p)
{
    dnx_field_qual_flags_e user_create_qual_flags;
    dnx_field_qual_t dnx_qual_id;
    bcm_field_qualify_t bcm_qual_id;
    int qual_indx;
    uint32 user_qual_size;
    int rv;
    /**
     * Used to give specific number of iteration in the for loop.
     * The for loop is calling the ctest_dnx_field_user_quals_create 'nof_times_to_iterate'
     * number of times for different negative cases.
     */
    int nof_times_to_iterate;
    ctest_dnx_field_user_quals_test_flag_e negative_case_iterator;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Iterating all of the negative cases.
     * From the first one till NOF
     * switching between the cases
     * Call the ctest_dnx_field_user_quals_create function
     * return proper ERROR
     *
     */
    for (negative_case_iterator = CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_ID;
         negative_case_iterator < CTEST_DNX_FIELD_USER_QUALS_NOF; negative_case_iterator++)
    {
        user_qual_size = CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE;
        user_create_qual_flags = 0;
        rv = BCM_E_NONE;
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING! CASE: %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
        /**
         * Switching between the negative cases
         * and giving the specific value for each of them
         */
        switch (negative_case_iterator)
        {
            case CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_ID:
            {
                nof_times_to_iterate = 1;
                user_create_qual_flags = DNX_FIELD_QUALIFIER_FLAG_WITH_ID;
                bcm_qual_id = CTEST_DNX_FIELD_USER_QUAL_ILLEGAL_QUAL_ID;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_SIZE:
            {
                nof_times_to_iterate = 1;
                user_qual_size = CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE_INVALID;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_SAME_ID:
            {
                user_create_qual_flags = DNX_FIELD_QUALIFIER_FLAG_WITH_ID;
                bcm_qual_id = CTEST_DNX_FIELD_USER_QUAL_ID;
                nof_times_to_iterate = 2;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_MAX_NOF_QUALS:
            {
                nof_times_to_iterate = CTEST_DNX_USER_QUALS_MAX_NOF_QUALS;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_NULL_CHECK:
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
          * The for loop is calling the ctest_dnx_field_user_quals_create 'nof_times_to_iterate'
          * number of times for different negative cases.
          */
        for (qual_indx = 0; qual_indx < nof_times_to_iterate; qual_indx++)
        {
            /**
             * Calling the ctest_dnx_field_user_quals_create which is calling the
             * API user_qual_create function
             */
            rv = ctest_dnx_field_user_quals_create(unit, user_create_qual_flags, field_stage,
                                                   negative_case_iterator, user_qual_size,
                                                   &bcm_qual_id, &dnx_qual_id, qual_info_p);
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
                         "Illegal qual from negative test case: %d was created! Test has failed!\r\n",
                         negative_case_iterator);
        }

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
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] set_qual_info_p - Pointer to SET database info from bcm_field_qualifier_create
 * \param [in] expected_error_p - Pointer to array of expected errors
 *           The size of this array is: CTEST_DNX_FIELD_USER_QUALS_NOF-1
 *                                     (the number of all negative cases)
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_field_user_quals_negative_test_bcm(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_qualifier_info_create_t * qual_info_p,
    _shr_error_t * expected_error_p)
{
    uint32 user_create_qual_flags;
    bcm_field_qualify_t bcm_qual_id;
    int qual_indx;
    uint32 user_qual_size;
    int rv;
    /**
     * Used to give specific number of iteration in the for loop.
     * The for loop is calling the ctest_dnx_field_user_quals_create_bcm 'nof_times_to_iterate'
     * number of times for different negative cases.
     */
    int nof_times_to_iterate;
    ctest_dnx_field_user_quals_test_flag_e negative_case_iterator;
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Iterating all of the negative cases.
     * From the first one till NOF
     * switching between the cases
     * Call the ctest_dnx_field_user_quals_create_bcm function
     * return proper ERROR
     */
    for (negative_case_iterator = CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_ID;
         negative_case_iterator < CTEST_DNX_FIELD_USER_QUALS_NOF - 1; negative_case_iterator++)
    {
        user_qual_size = CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE;
        user_create_qual_flags = 0;
        rv = BCM_E_NONE;
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING! CASE: %d \r\n",
                    __func__, __LINE__, dnx_field_bcm_stage_text(field_stage), negative_case_iterator);
        /**
         * Switching between the negative cases
         * and giving the specific value for each of them
         */
        switch (negative_case_iterator)
        {
            case CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_ID:
            {
                nof_times_to_iterate = 1;
                user_create_qual_flags = BCM_FIELD_FLAG_WITH_ID;
                bcm_qual_id = CTEST_DNX_FIELD_USER_QUAL_ILLEGAL_QUAL_ID;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_ILLEGAL_SIZE:
            {
                nof_times_to_iterate = 1;
                user_qual_size = CTEST_DNX_FIELD_USER_QUALS_USER_QUAL_SIZE_INVALID;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_SAME_ID:
            {
                user_create_qual_flags = BCM_FIELD_FLAG_WITH_ID;
                bcm_qual_id = CTEST_DNX_FIELD_USER_QUAL_ID;
                nof_times_to_iterate = 2;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_MAX_NOF_QUALS:
            {
                nof_times_to_iterate = CTEST_DNX_USER_QUALS_MAX_NOF_QUALS;
                break;
            }
            case CTEST_DNX_FIELD_USER_QUALS_NULL_CHECK:
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
          * The for loop is calling the ctest_dnx_field_user_quals_create_bcm 'nof_times_to_iterate'
          * number of times for different negative cases.
          */
        for (qual_indx = 0; qual_indx < nof_times_to_iterate; qual_indx++)
        {
            /**
             * Calling the ctest_dnx_field_user_quals_create_bcm which is calling the
             * API user_qual_create function
             */
            rv = ctest_dnx_field_user_quals_create_bcm(unit, user_create_qual_flags, field_stage,
                                                       negative_case_iterator, user_qual_size,
                                                       &bcm_qual_id, qual_info_p);
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
                        __func__, __LINE__, dnx_field_bcm_stage_text(field_stage), negative_case_iterator);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Illegal qual from negative test case: %d was created! Test has failed!\r\n",
                         negative_case_iterator);
        }
    }

    /*
     * Create two qualifiers with the same name
     */
    {
        shr_error_e rv;
        bcm_field_qualify_t bcm_qual_id_1;
        bcm_field_qualify_t bcm_qual_id_2;
        char name[] = "Identical_name";
        bcm_field_qualifier_info_create_t qual_info;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 160;
        sal_strncpy_s((char *) (qual_info.name), name, sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_1));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        sal_strncpy_s((char *) (qual_info.name), name, sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_2);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected error %d, but received %d when creating two qualifiers with identical names.\r\n",
                         _SHR_E_EXISTS, rv);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id_1));
    }
    /*
     * Create two qualifiers with the same name except for a different case.
     */
    {
        shr_error_e rv;
        bcm_field_qualify_t bcm_qual_id_1;
        bcm_field_qualify_t bcm_qual_id_2;
        char name_1[] = "Identical_name";
        char name_2[] = "Identical_Name";
        bcm_field_qualifier_info_create_t qual_info;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 160;
        sal_strncpy_s((char *) (qual_info.name), name_1, sizeof(qual_info.name));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_1));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        sal_strncpy_s((char *) (qual_info.name), name_2, sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_2);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected error %d, but received %d when creating two qualifeirs with identical names "
                         "except case.\r\n", _SHR_E_EXISTS, rv);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id_1));
    }
    /*
     * Create a qualifer with a name identical to a predefined defined name.
     */
    {
        shr_error_e rv;
        bcm_field_qualify_t bcm_qual_id_1;
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_get_t predef_qual_info_get;

        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                        (unit, bcmFieldQualifyColor, bcmFieldStageIngressPMF1, &predef_qual_info_get));

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 160;
        sal_strncpy_s((char *) (qual_info.name), (char *) (predef_qual_info_get.name), sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_1);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected error %d, but received %d when creating a qualifer with the name of a "
                         "predefines one.\r\n", _SHR_E_EXISTS, rv);
        }
    }
    /*
     * Create two qualifiers with the same name with full size (without NULL)
     */
    {
        shr_error_e rv;
        bcm_field_qualify_t bcm_qual_id_1;
        bcm_field_qualify_t bcm_qual_id_2;
        bcm_field_qualifier_info_create_t qual_info;
        int char_index;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 160;
        for (char_index = 0; char_index < sizeof(qual_info.name); char_index++)
        {
            qual_info.name[char_index] = 'a';
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_1));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        for (char_index = 0; char_index < sizeof(qual_info.name); char_index++)
        {
            qual_info.name[char_index] = 'a';
        }
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_2);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected error %d, but received %d when creating two qualifers with identical names at full size.\r\n",
                         _SHR_E_EXISTS, rv);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id_1));
    }
    /*
     * Create two qualifers with the same name until the last character. Fails becasue last charachter isn't copied.
     */
    {
        shr_error_e rv;
        bcm_field_qualify_t bcm_qual_id_1;
        bcm_field_qualify_t bcm_qual_id_2;
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_get_t qual_info_get_1;
        int char_index;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 160;
        for (char_index = 0; char_index < sizeof(qual_info.name); char_index++)
        {
            qual_info.name[char_index] = 'a';
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_1));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        for (char_index = 0; char_index < sizeof(qual_info.name); char_index++)
        {
            qual_info.name[char_index] = 'a';
        }
        qual_info.name[sizeof(qual_info.name) - 1] = 'b';
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_2);
        if (rv != _SHR_E_EXISTS)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected error %d, but received %d when creating two qualifers with identical names at full size minus one.\r\n",
                         _SHR_E_EXISTS, rv);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcm_qual_id_1, field_stage, &qual_info_get_1));
        if (qual_info_get_1.name[sizeof(qual_info.name) - 2] != 'a')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected character before last to be %d, but is %d.\r\n",
                         'a', qual_info_get_1.name[sizeof(qual_info.name) - 2]);
        }
        if (qual_info_get_1.name[sizeof(qual_info.name) - 1] != '\0')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected last character to be %d (NULL), but is %d.\r\n",
                         '\0', qual_info_get_1.name[sizeof(qual_info.name) - 1]);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id_1));
    }
    /*
     * Create two qualifers with the same name until one before last character.
     */
    {
        bcm_field_qualify_t bcm_qual_id_1;
        bcm_field_qualify_t bcm_qual_id_2;
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_get_t qual_info_get_1;
        bcm_field_qualifier_info_get_t qual_info_get_2;
        int char_index;

        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 160;
        for (char_index = 0; char_index < sizeof(qual_info.name); char_index++)
        {
            qual_info.name[char_index] = 'a';
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_1));
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        for (char_index = 0; char_index < sizeof(qual_info.name); char_index++)
        {
            qual_info.name[char_index] = 'a';
        }
        qual_info.name[sizeof(qual_info.name) - 2] = 'b';
        qual_info.name[sizeof(qual_info.name) - 1] = 'c';
        SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &bcm_qual_id_2));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcm_qual_id_1, field_stage, &qual_info_get_1));
        if (qual_info_get_1.name[sizeof(qual_info.name) - 3] != 'a')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected 2 characters before last to be %d, but is %d.\r\n",
                         'a', qual_info_get_1.name[sizeof(qual_info.name) - 2]);
        }
        if (qual_info_get_1.name[sizeof(qual_info.name) - 2] != 'a')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected character before last to be %d, but is %d.\r\n",
                         'a', qual_info_get_1.name[sizeof(qual_info.name) - 2]);
        }
        if (qual_info_get_1.name[sizeof(qual_info.name) - 1] != '\0')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected last character to be %d (NULL), but is %d.\r\n",
                         '\0', qual_info_get_1.name[sizeof(qual_info.name) - 1]);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcm_qual_id_2, field_stage, &qual_info_get_2));
        if (qual_info_get_2.name[sizeof(qual_info.name) - 3] != 'a')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected 2 character before last to be %d, but is %d.\r\n",
                         'a', qual_info_get_2.name[sizeof(qual_info.name) - 2]);
        }
        if (qual_info_get_2.name[sizeof(qual_info.name) - 2] != 'b')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected character before last to be %d, but is %d.\r\n",
                         'a', qual_info_get_2.name[sizeof(qual_info.name) - 2]);
        }
        if (qual_info_get_2.name[sizeof(qual_info.name) - 1] != '\0')
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Expected last character to be %d (NULL), but is %d.\r\n",
                         '\0', qual_info_get_2.name[sizeof(qual_info.name) - 1]);
        }
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id_1));
        SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, bcm_qual_id_2));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * This function is basic 'dnx_field_qual_create'
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
appl_dnx_field_user_quals_semantic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e dnx_field_stage;
    bcm_field_stage_t bcm_field_stage;
    int field_group_test_type_name;
    dnx_field_qualifier_in_info_t dnx_qual_info;
    bcm_field_qualifier_info_create_t bcm_qual_info;
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
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_USER_QUALS_TYPE, field_group_test_type_name);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_USER_QUALS_TEST_STAGE, dnx_field_stage);
    /**
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (field_group_test_type_name == 0)
    {
        /**
         * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
        /** Convert DNX to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));
        /**
         * Calling the positive function
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_quals_positive_test_bcm(unit, bcm_field_stage, &bcm_qual_info));
        /**
         * Calling negative test function
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_quals_negative_test_bcm
                        (unit, bcm_field_stage, &bcm_qual_info, Ctest_dnx_field_user_quals_expected_errors));
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
        SHR_IF_ERR_EXIT(appl_dnx_field_user_quals_positive_test(unit, dnx_field_stage, &dnx_qual_info));
        /**
         * Calling negative test function
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_user_quals_negative_test
                        (unit, dnx_field_stage, &dnx_qual_info, Ctest_dnx_field_user_quals_expected_errors));
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
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    SHR_FUNC_EXIT;
}

/**
 * \brief - run user_quals init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx user_quals_start"
 */
shr_error_e
sh_dnx_field_user_quals_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_USER_QUALS_OPTION_TEST_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_user_quals_semantic(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

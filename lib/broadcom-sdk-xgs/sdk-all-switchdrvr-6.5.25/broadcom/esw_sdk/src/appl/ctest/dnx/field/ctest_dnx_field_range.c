/** \file ctest_dnx_field_range.c
 * $Id$
 *
 * Field User range set/get usage example.
 * Positive test:
 *   1. Create an range with ID
 *   2. Compare the set and get fields
 * Negative test cases:
 *  1. Create a range with minimum value bigger than the maximum value
 *  For each range_type:
 *  1. Create a range with range_id out of range
 *  2. Create a range with wrong stage.
 *  3. Create a range with maximum value out of range
 *
 */
/**
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* 
        */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

/**
 * Include files.
 * {
 */
#include <bcm/field.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include "ctest_dnx_field_range.h"
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_range.h>
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
#define CTEST_DNX_FIELD_RANGE_TYPE              "type"
/**
 * \brief
 *   Keyword for TCL testing, clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_RANGE_CLEAN             "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_RANGE_OPTION_TEST_COUNT       "count"
/**
 * \brief
 *   Options list for 'range' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_range_options[] = {
     /** Name */                                             /** Type */            /** Description */                     /** Default */
    {CTEST_DNX_FIELD_RANGE_TYPE,                SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_RANGE_CLEAN,               SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {CTEST_DNX_RANGE_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}
    /**
     * End of options list - must be last.
     */
};
/**
 * \brief
 *   List of tests for 'range' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_range_tests[] = {
    {"DNX_field_ranges_test_1", "type=DNX  count=1", CTEST_POSTCOMMIT},
    {NULL}
};
/**
 *  Range shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_range_man = {
"Field user create ranges related test utilities",
    "Activate field user created ranges related test utilities. "
    "DNX-level tests. \r\n"
    "The 'count' variable defines how many times the test will run. \r\n",
    "ctest field range type=<BCM | DNX> clean=<0 | 1 count=1>"
};
/**
 * Give all required information for positive test.
 * DNX_FIELD_RANGE_TYPE_L4_SRC_PORT can be set in IPMF1, IPMF2 and EPMF. Maximum range ID is 23 and the range for min-max is [0-65535].
 * DNX_FIELD_RANGE_TYPE_L4_DST_PORT can be set in IPMF1, IPMF2 and EPMF. Maximum range ID is 23 and the range for min-max is [0-65535].
 * DNX_FIELD_RANGE_TYPE_OUT_LIF_PORT can be set in IPMF1, IPMF2 and IPMF3. Maximum range ID for IPMF1/2 is 2 and for IPMF3 is 6 and the range for min-max is [0-4194303].
 * DNX_FIELD_RANGE_TYPE_L4_DST_PORT can be set in IPMF1, IPMF2. Maximum range ID is 2 and the range for min-max is [0-255].
 */
static dnx_ctest_range_full_info_t Dnx_ctest_range_full_info[CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_POSITIVE_TEST] =
{
    /* range_type */                      /* stage */                   /* range_id */     /* min_val */    /* max_val */
    {DNX_FIELD_RANGE_TYPE_L4_SRC_PORT,    DNX_FIELD_STAGE_IPMF1,             0,                 0,             10},
    {DNX_FIELD_RANGE_TYPE_L4_SRC_PORT,    DNX_FIELD_STAGE_EPMF,              23,                15,            65534},
    {DNX_FIELD_RANGE_TYPE_L4_SRC_PORT,    DNX_FIELD_STAGE_IPMF1,             2,                 50,            10},

    {DNX_FIELD_RANGE_TYPE_L4_DST_PORT,    DNX_FIELD_STAGE_IPMF2,             10,                0,             100},
    {DNX_FIELD_RANGE_TYPE_L4_DST_PORT,    DNX_FIELD_STAGE_EPMF,              23,                27,            65534},

    {DNX_FIELD_RANGE_TYPE_OUT_LIF,        DNX_FIELD_STAGE_IPMF1,             2,                 30,            4194302},
    {DNX_FIELD_RANGE_TYPE_OUT_LIF,        DNX_FIELD_STAGE_IPMF3,             6,                 35,            500},

    {DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE,   DNX_FIELD_STAGE_IPMF1,             0,                 35,            254},
    {DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE,   DNX_FIELD_STAGE_IPMF2,             2,                 35,            150}
};
/**
 * This array is used for negative testing. It gives all required information for negative testing.
 */
static dnx_ctest_range_full_info_t Dnx_ctest_range_full_info_negative[CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_NEGATIVE_TEST] =
{
    /* range_type */                      /* stage */                   /* range_id */     /* min_val */    /* max_val */     /* Expected value*/
    {DNX_FIELD_RANGE_TYPE_L4_SRC_PORT,    DNX_FIELD_STAGE_IPMF3,             0,                 0,            10,              _SHR_E_PARAM}, /** Illegal stage */
    {DNX_FIELD_RANGE_TYPE_L4_SRC_PORT,    DNX_FIELD_STAGE_EPMF,              24,                15,           65534,           _SHR_E_PARAM},   /** Illegal range_id*/

    {DNX_FIELD_RANGE_TYPE_L4_DST_PORT,    DNX_FIELD_STAGE_EPMF,              23,                27,           65536,           _SHR_E_PARAM},   /**Out of range max value */

    {DNX_FIELD_RANGE_TYPE_OUT_LIF,        DNX_FIELD_STAGE_EPMF,              2,                 30,           4194302,         _SHR_E_PARAM}, /** Illegal stage */
    {DNX_FIELD_RANGE_TYPE_OUT_LIF,        DNX_FIELD_STAGE_IPMF1,             6,                 35,           500,             _SHR_E_PARAM},   /** Illegal range_id for IPMF1*/
    {DNX_FIELD_RANGE_TYPE_OUT_LIF,        DNX_FIELD_STAGE_IPMF3,             8,                 30,           4194302,         _SHR_E_PARAM},   /** Illegal range_id for IPMF3*/
    {DNX_FIELD_RANGE_TYPE_OUT_LIF,        DNX_FIELD_STAGE_IPMF3,             6,                 0,            4194306,         _SHR_E_PARAM},   /**Out of range max value */

    {DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE,   DNX_FIELD_STAGE_IPMF3,             0,                 35,           254,             _SHR_E_PARAM}, /** Illegal stage */
    {DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE,   DNX_FIELD_STAGE_IPMF2,             3,                 35,           150,             _SHR_E_PARAM},   /** Illegal range_id*/
    {DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE,   DNX_FIELD_STAGE_IPMF2,             2,                 35,           256,             _SHR_E_PARAM}    /**Out of range max value */
};
/* *INDENT-ON* */
 /**
  * \brief
  *
  * This procedure is used by ctest_dnx_field_range_set()
  *    to compare between info corresponding to 'set' and to 'get'.
  *
  * \param [in] unit - The unit number.
  * \param [in] dnx_range_id - pointer to dnx_range
  * \param [in] set_range_info_p - pointer to SET database info from dnx_field_range_set
  * \return
  *   Error code (as per 'bcm_error_e').
  * \see
  *   bcm_error_e
  *
  */
static shr_error_e
ctest_dnx_field_range_compare(
    int unit,
    uint32 range_id,
    dnx_field_stage_e field_stage,
    dnx_field_range_info_t * set_range_info_p)
{

    dnx_field_range_info_t get_range_info;

    SHR_FUNC_INIT_VARS(unit);

     /**
      * Init all of the parameters in the structure dnx_field_range_info_t
      */
    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &get_range_info));

    /**
     * range_type is always an input!
     */
    get_range_info.range_type = set_range_info_p->range_type;

    /**
     * Get the minimum and maximum value to be compared.
     */
    SHR_IF_ERR_EXIT(dnx_field_range_get(unit, field_stage, &range_id, &get_range_info));

   /**
    * Compare the set minimum value and the get minimum value
    */
    if (set_range_info_p->min_val != get_range_info.min_val)

    {

        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_range_get: Unexpected data returned.\n"
                     "min_val expected %d received %d.\n", set_range_info_p->min_val, get_range_info.min_val);

    }

    /**
     * Compare the set maximum value and the get maximum value
     */
    if (set_range_info_p->max_val != get_range_info.max_val)

    {

        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "dnx_field_range_get: Unexpected data returned.\n"
                     "max_val expected %d received %d.\n", set_range_info_p->max_val, get_range_info.max_val);

    }

    LOG_INFO_EX(BSL_LOG_MODULE, " Done.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  This procedure is used by ctest_dnx_field_range_semantic.
 *  It contains the dnx_field_range_set application.
 *  This procedure sets all required HW configuration for
 *  dnx_field_range_set to be performed.
 *  It includes the positive and the negative test.
 *
 * \param [in] unit - The unit number.
 * \param [in] range_flags - Flags of dnx_field_range_set API.
 *                           We do not use them for now!
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] dnx_ctest_range_full_info_p - Pointer to an array with all required information
 *                           for setting a range.
 * \param [in] test_flag -  Shows if the test is positive or negative:
 *           if the flags is equal to CTEST_DNX_FIELD_RANGE_POSITIVE_TEST the test is positive.
 *           if the flags is equal to CTEST_DNX_FIELD_RANGE_NEGATIVE_TEST the test is negative.
 * \param [in] nof_iterations - Shows the number of iterations for Positive and Negative test:
 *           if the test is positive - CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_POSITIVE_TEST.
 *           if the test is negative - CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_NEGATIVE_TEST.
 * \param [out] dnx_range_id_p - Pointer to range_id
 * \param [out] range_info_p - pointer to database info that is to be filled
 *                           in by this procedure. For details, see 'dnx_field_range_info_t'
 *
 * \return
 * For positive test:
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 * For negative test:
 *   Compare the error with the expected error
 *
 */
static shr_error_e
ctest_dnx_field_range_set(
    int unit,
    uint32 range_flags,
    dnx_field_stage_e field_stage,
    dnx_ctest_range_full_info_t * dnx_ctest_range_full_info_p,
    uint8 positive_test_flag,
    uint32 nof_iterations,
    uint32 *range_id_p,
    dnx_field_range_info_t * range_info_p)
{

    int range_indx;

    int rv;

    SHR_FUNC_INIT_VARS(unit);

     /**
      * NULL check for all pointers
      */
    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");

    SHR_NULL_CHECK(range_id_p, _SHR_E_PARAM, "range_id_p");

    rv = BCM_E_NONE;

   /**
    * Init all of the parameters in the structure dnx_field_range_info_t
    */
    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, range_info_p));

   /**
    * Fill the structure needed for dnx_field_range_set API
    */
    for (range_indx = 0; range_indx < nof_iterations; range_indx++)

    {

        field_stage = dnx_ctest_range_full_info_p[range_indx].field_stage;

        *range_id_p = dnx_ctest_range_full_info_p[range_indx].range_id;

        range_info_p->range_type = dnx_ctest_range_full_info_p[range_indx].range_type;

        range_info_p->min_val = dnx_ctest_range_full_info_p[range_indx].min_val;

        range_info_p->max_val = dnx_ctest_range_full_info_p[range_indx].max_val;

        /**
         * Calling the API dnx_field_range_set for positive case
         */
        if (positive_test_flag == CTEST_DNX_FIELD_RANGE_POSITIVE_TEST)

        {

            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, In Stage: %s Set a range with Range_id: %d \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), *range_id_p);

            SHR_IF_ERR_EXIT(dnx_field_range_set(unit, range_flags, field_stage, *range_id_p, range_info_p));

            /**
             * Compare the set and get elements.
             */
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, In Stage: %s Compare the set and get Range_id: %d \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), *range_id_p);

            SHR_IF_ERR_EXIT(ctest_dnx_field_range_compare(unit, *range_id_p, field_stage, range_info_p));

        }

        /**
         * In all negative cases the error value is needed to be compare with
         * the expected errors.
         */
        else

        {

            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, In Stage: %s NEGATIVE TEST:%d \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), range_indx);

            rv = dnx_field_range_set(unit, range_flags, field_stage, *range_id_p, range_info_p);

            if (rv == dnx_ctest_range_full_info_p[range_indx].expected_error)

            {

                LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, In Stage: %s NEGATIVE TEST:%d was successful!\r\n",
                            __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), range_indx);

            }

            else

            {

                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Illegal range from negative test case: %d was set! Test has failed!\r\n", range_indx);

            }

        }

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *
 * This function is basic 'dnx_field_range_set'
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
 *  1. Calling the positive test
 *  2. Calling the negative test
 *
 */
static shr_error_e
appl_dnx_field_range_semantic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    dnx_field_stage_e dnx_field_stage;

    uint32 range_flags, range_id;

    int field_group_test_type_name;

    dnx_field_range_info_t range_info;

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
    range_flags = 0;
    dnx_field_stage = 0;
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
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_RANGE_TYPE, field_group_test_type_name);

    /**
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (field_group_test_type_name == 1)

    {

        /**
         * Calling the positive function
         */
        SHR_IF_ERR_EXIT(ctest_dnx_field_range_set
                        (unit, range_flags, dnx_field_stage, Dnx_ctest_range_full_info,
                         CTEST_DNX_FIELD_RANGE_POSITIVE_TEST, CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_POSITIVE_TEST,
                         &range_id, &range_info));

        /**
         * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
         */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, bslSeverityFatal);

        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

        /**
         * Calling the negative function
         */
        SHR_IF_ERR_EXIT(ctest_dnx_field_range_set
                        (unit, range_flags, dnx_field_stage, Dnx_ctest_range_full_info_negative,
                         CTEST_DNX_FIELD_RANGE_NEGATIVE_TEST, CTEST_DNX_FIELD_RANGE_NOF_FIELDS_FOR_NEGATIVE_TEST,
                         &range_id, &range_info));

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
 * \brief - run range init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx range_start"
 */
shr_error_e
sh_dnx_field_range_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int count_iter, count;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");

    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    SH_SAND_GET_UINT32(CTEST_DNX_RANGE_OPTION_TEST_COUNT, count);

    for (count_iter = 0; count_iter < count; count_iter++)

    {

        SHR_IF_ERR_EXIT(appl_dnx_field_range_semantic(unit, args, sand_control));

    }

exit:
    SHR_FUNC_EXIT;

}

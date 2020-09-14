/** \file diag_dnx_field_context.c
 * $Id$
 *
 * Field Context set/get usage example.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_context.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>

/*
 * }
 */

/* *INDENT-OFF* */

/** MAX Number of PMF contexts as define ,device independent */
#define CTEST_DNX_FIELD_CONTEXT_MAX_NUM_CONTEXTS         dnx_data_field.common_max_val.nof_contexts_get(unit)

/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_CONTEXT_OPTION_TYPE        "type"
/**
 * \brief
 *   Keyword for stage of test on context command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define CTEST_DNX_FIELD_CONTEXT_OPTION_TEST_STAGE        "stage"
 /**
 * \brief
 *   Keyword for TCL testing, clean can be either 0 or 1,
 *   if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_CONTEXT_OPTION_CLEAN             "clean"

/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_ACTION_OPTION_TEST_COUNT               "count"

/**
 * \brief
 *   Options list for 'context' shell command
 * \remark
 */
sh_sand_option_t dnx_field_context_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {CTEST_DNX_FIELD_CONTEXT_OPTION_TYPE,        SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_CONTEXT_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",   "ipmf1",   (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_CONTEXT_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Will test perform cleanup or not",         "Yes"},
    {CTEST_DNX_ACTION_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'context' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_context_tests[] = {

    {"DNX_fld_context_1", "type=dnx clean=yes stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"DNX_fld_context_2", "type=dnx clean=yes stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"DNX_fld_context_3", "type=dnx clean=yes stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"DNX_fld_context_e", "type=dnx clean=yes stage=epmf  count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_1", "type=bcm clean=yes stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_2", "type=bcm clean=yes stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_3", "type=bcm clean=yes stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_context_e", "type=bcm clean=yes stage=epmf  count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t sh_dnx_field_context_man = {
    "Field context related test utilities",
    "Using context create and get functions to test the context creation and get DNX API functions",
    "ctest field cont  type=<DNX | BCM> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> clean=<YES | NO> count=1"
};

/**
 * \brief
 * Compare function for 'dnx_field_context_info_t'. The function receives the SET info and the context ID.
 * The ID is used in the GET function for the GET info struct, which we compare to the SET info struct.
 *
 * \param [in] unit                 - Device ID
 * \param [in] field_stage          - PMF Stage
 * \param [in] context_id           - Context ID of the context we've created(used for to get the context info.
 * \param [in] *set_context_mode    - Context info that the context has been created with, and we compare against.
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_mode_compare_dnx(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t * set_context_mode)
{
    dnx_field_context_mode_t get_context_mode;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only iPMF1 and iPMF2 have context mode to compare.
     */
    if (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        /** Initialize the GET info structure */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &get_context_mode));

        /** Get the context info of the supplied Context ID */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, context_id, &get_context_mode));

        /*
         * Comparison of iPMF1 context information.
         */
        if (field_stage == DNX_FIELD_STAGE_IPMF1)
        {
            /** Compare context info comapre_mode_1 field */
            if (set_context_mode->context_ipmf1_mode.compare_mode_1 !=
                get_context_mode.context_ipmf1_mode.compare_mode_1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_mode comapre_mode.compare_mode_1 (%s) is not the same as "
                             "Get_context_mode compare_info.compare_mode_1(%s)\r\n",
                             dnx_field_context_compare_mode_e_get_name(set_context_mode->
                                                                       context_ipmf1_mode.compare_mode_1),
                             dnx_field_context_compare_mode_e_get_name(get_context_mode.
                                                                       context_ipmf1_mode.compare_mode_1));
            }

            /** Compare context info comapre_mode_2 field */
            if (set_context_mode->context_ipmf1_mode.compare_mode_2 !=
                get_context_mode.context_ipmf1_mode.compare_mode_2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_mode compare_info.compare_mode_2 (%s) is not the same as "
                             "Get_context_mode compare_info.compare_mode_2(%s)\r\n",
                             dnx_field_context_compare_mode_e_get_name(set_context_mode->
                                                                       context_ipmf1_mode.compare_mode_2),
                             dnx_field_context_compare_mode_e_get_name(get_context_mode.
                                                                       context_ipmf1_mode.compare_mode_2));
            }

            /** Compare context info hashing_info hashing */
            if (set_context_mode->context_ipmf1_mode.hash_mode != get_context_mode.context_ipmf1_mode.hash_mode)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_mode hashing_info.hashing (%d) is not the same as Get_context_mode "
                             "hashing_info.hashing(%d)\r\n",
                             set_context_mode->context_ipmf1_mode.hash_mode,
                             get_context_mode.context_ipmf1_mode.hash_mode);
            }
        }

        /*
         * Comparioson of iPMF1 context information.
         */
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            /** Compare context info Cascated_from field */
            if (set_context_mode->context_ipmf2_mode.cascaded_from != get_context_mode.context_ipmf2_mode.cascaded_from)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_mode cascaded_from (%d) is not the same as "
                             "Get_context_mode cascaded_from(%d)\r\n",
                             set_context_mode->context_ipmf2_mode.cascaded_from,
                             get_context_mode.context_ipmf2_mode.cascaded_from);
            }
        }

        if (strncmp
            ((char *) set_context_mode->name, (char *) get_context_mode.name, BCM_FIELD_MAX_SHORT_NAME_LEN - 1) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "dnx_field_context_mode_get: Unexpected data returned.\n"
                         "name expected %s received %s.\n", set_context_mode->name, get_context_mode.name);
        }

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s .Context Info compare finished successfully for context : %d \r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), context_id);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %s. Context Info compare called for context : %d. "
                    " No Context Info for the stage.\r\n",
                    __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), context_id);

    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * Compare function for 'bcm_field_context_info_t'. The function receives the SET info and the context ID.
 * The ID is used in the GET function for the GET info struct, which we compare to the SET info struct.
 *
 * \param [in] unit                 - Device ID
 * \param [in] field_stage          - PMF Stage
 * \param [in] context_id           - Context ID of the context we've created(used for to get the context info.
 * \param [in] *set_context_info    - Context info that the context has been created with, and we compare against.
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_info_compare_bcm(
    int unit,
    bcm_field_stage_t field_stage,
    bcm_field_context_t context_id,
    bcm_field_context_info_t * set_context_info)
{
    bcm_field_context_info_t get_context_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only iPMF1 and iPMF2 have context mode to compare.
     */
    if (field_stage == bcmFieldStageIngressPMF1 || field_stage == bcmFieldStageIngressPMF2)
    {
        /** Initialize the GET info structure */
        bcm_field_context_info_t_init(&get_context_info);

        /** Get the context info of the supplied Context ID */
        SHR_IF_ERR_EXIT(bcm_field_context_info_get(unit, field_stage, context_id, &get_context_info));

        /*
         * Comparison of iPMF1 context information.
         */
        if (field_stage == bcmFieldStageIngressPMF1)
        {
            /** Compare context info comapre_mode_1 field */
            if (set_context_info->context_compare_modes.compare_1_mode !=
                get_context_info.context_compare_modes.compare_1_mode)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_info comapre_mode.compare_mode_1 (%d) is not the same as "
                             "Get_context_info compare_info.compare_mode_1(%d)\r\n",
                             set_context_info->context_compare_modes.compare_1_mode,
                             get_context_info.context_compare_modes.compare_1_mode);
            }

            /** Compare context info comapre_mode_2 field */
            if (set_context_info->context_compare_modes.compare_2_mode !=
                get_context_info.context_compare_modes.compare_2_mode)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_info compare_info.compare_mode_2 (%d) is not the same as "
                             "Get_context_info compare_info.compare_mode_2(%d)\r\n",
                             set_context_info->context_compare_modes.compare_2_mode,
                             get_context_info.context_compare_modes.compare_2_mode);
            }

            /** Compare context info hashing_info hashing */
            if (set_context_info->hashing_enabled != get_context_info.hashing_enabled)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_info hashing_info.hashing (%d) is not the same as "
                             "Get_context_info hashing_info.hashing(%d)\r\n",
                             set_context_info->hashing_enabled, get_context_info.hashing_enabled);
            }
        }

        /*
         * Comparioson of iPMF2 context information.
         */
        if (field_stage == bcmFieldStageIngressPMF2)
        {

            /** Compare context info Cascated_from field */
            if (set_context_info->cascaded_from != get_context_info.cascaded_from)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Set_context_info cascaded_from (%d) is not the same as "
                             "Get_context_info cascaded_from(%d)\r\n",
                             set_context_info->cascaded_from, get_context_info.cascaded_from);
            }
        }

        if (strncmp
            ((char *) set_context_info->name, (char *) get_context_info.name, BCM_FIELD_MAX_SHORT_NAME_LEN - 1) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "bcm_field_context_info_get: Unexpected data returned.\n"
                         "name expected %s received %s.\n", set_context_info->name, get_context_info.name);
        }

        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %d .Context Info compare finished successfully for context : %d \r\n",
                    __func__, __LINE__, field_stage, context_id);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Stage: %d. Context Info compare called for context : %d. "
                    " No Context Info for the stage.\r\n", __func__, __LINE__, field_stage, context_id);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Simple Case of context creation. We send the created context and the set context_info to
 *  a compare function, where we do a GET and compare on each field.
 *
 * \param [in] unit         - Device ID
 * \param [in] args         - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_context_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_field_context_info_t bcm_context_info, dummy_info;
    bcm_field_context_t bcm_context_id;
    dnx_field_context_flags_e bcm_context_flags;
    bcm_field_stage_t bcm_field_stage;
    bcm_field_context_info_t bcm_get_context_info;

    dnx_field_context_mode_t dnx_context_mode, ipmf1_context_mode;
    dnx_field_context_t dnx_context_id;
    dnx_field_context_flags_e dnx_context_flags;
    dnx_field_stage_e dnx_field_stage;
    int field_context_test_clean;
    dnx_field_context_mode_t dnx_get_context_mode;
    int rv, context_iter;
    bsl_severity_t original_severity_fldprocdnx;
    bsl_severity_t original_severity_resmngr;
    bsl_severity_t original_severity_dbaldnx;
    bsl_severity_t original_severity_hashdnx;
    bsl_severity_t original_severity_swstate;
    dnx_field_context_t free_context;
    dnx_field_context_t free_context_2;
    int max_nof_contexts;
    uint8 is_alloc;

    unsigned int test_type;

    SHR_FUNC_INIT_VARS(unit);

    bcm_context_id = dnx_context_id = 0;
    dnx_field_stage = DNX_FIELD_STAGE_INVALID;
    field_context_test_clean = TRUE;
    test_type = 0;
    free_context = free_context_2 = 0;

    /*
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);

    rv = _SHR_E_NONE;
    field_context_test_clean = 0;

    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_CONTEXT_OPTION_CLEAN, field_context_test_clean);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_CONTEXT_OPTION_TEST_STAGE, dnx_field_stage);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_CONTEXT_OPTION_TYPE, test_type);

    if (test_type == 0)
    {
        bcm_field_context_info_t_init(&dummy_info);
        /** Initialize the SET info structure */
        bcm_field_context_info_t_init(&bcm_context_info);
        /** Initialize the GET info structure */
        bcm_field_context_info_t_init(&bcm_get_context_info);

        /** Convert DNC to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_field_stage, &bcm_field_stage));
        /** Setting valid non-zero values to context_info, so that we can test the GET and COMPARE */
        bcm_context_flags = 0;
        if (bcm_field_stage == bcmFieldStageIngressPMF1)
        {
            bcm_context_info.hashing_enabled = FALSE;
            bcm_context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeSingle;
            bcm_context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
        }
        else if (bcm_field_stage == bcmFieldStageIngressPMF2)
        {
            SHR_IF_ERR_EXIT(bcm_field_context_create
                            (unit, bcm_context_flags, bcmFieldStageIngressPMF1, &dummy_info,
                             &bcm_context_info.cascaded_from));
        }
        sal_strncpy_s((char *) (bcm_context_info.name), "Context_test", sizeof(bcm_context_info.name));

        SHR_IF_ERR_EXIT(bcm_field_context_create
                        (unit, bcm_context_flags, bcm_field_stage, &bcm_context_info, &bcm_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Context=%d  created \n %s%s%s", bcm_context_id, EMPTY, EMPTY, EMPTY);

        /** Calling the custom GET/COMPARE function to see if all info set is same as what we get */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_info_compare_bcm
                        (unit, bcm_field_stage, bcm_context_id, &bcm_context_info));

        /** Destroying the created context */
        SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcm_field_stage, bcm_context_id));

        /** Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        /**
         * Get API after destroying the context should return NOT_FOUND.
         */
        rv = bcm_field_context_info_get(unit, bcm_field_stage, bcm_context_id, &bcm_get_context_info);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %d .Context ID: %d does not exist  \r\n",
                        __func__, __LINE__, bcm_field_stage, bcm_context_id);
        }
        else
        {
            SHR_SET_CURRENT_ERR(rv);
        }
        if (bcm_field_stage == bcmFieldStageIngressPMF2)
        {
            /** Destroying the created context */
            SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_info.cascaded_from));
        }
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);

        /**
         *  --------------------- ALLOCATION AND DEALLOCATION TESTING SCENARIO ------------------------------
         */
        bcm_field_context_info_t_init(&bcm_context_info);
        bcm_context_flags = 0;
        if (bcm_field_stage == bcmFieldStageIngressPMF2)
        {
            /** Set the ''cascaded_from' to the default context for iPMF2.*/
            bcm_context_info.cascaded_from = BCM_FIELD_CONTEXT_ID_DEFAULT;
        }
        /** Test scenario for create/destroy looped 10 times - Testing allocation/deallocation */
        for (context_iter = 0; context_iter < CTEST_DNX_FIELD_NOF_ALLOC_CYCLES; context_iter++)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "Allocation/Deallocation try: %d \n %s%s%s", context_iter, EMPTY, EMPTY, EMPTY);

            /** Creating a context */
            SHR_IF_ERR_EXIT(bcm_field_context_create
                            (unit, bcm_context_flags, bcm_field_stage, &bcm_context_info, &bcm_context_id));

            LOG_INFO_EX(BSL_LOG_MODULE, "Context=%d  created \n %s%s%s", bcm_context_id, EMPTY, EMPTY, EMPTY);

            /** Destroying the created context */
            SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcm_field_stage, bcm_context_id));

            LOG_INFO_EX(BSL_LOG_MODULE, "Context=%d  destroyed \n %s%s%s", bcm_context_id, EMPTY, EMPTY, EMPTY);
        }
    }
    else if (test_type == 1)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_context_mode));
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &ipmf1_context_mode));
        /** Initialize the GET info structure */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_get_context_mode));

        /** Setting valid non-zero values to context_mode, so that we can test the GET and COMPARE */
        dnx_context_flags = 0;
        if (dnx_field_stage == DNX_FIELD_STAGE_IPMF1)
        {
            dnx_context_mode.context_ipmf1_mode.hash_mode = DNX_FIELD_CONTEXT_HASH_MODE_DISABLED;
            dnx_context_mode.context_ipmf1_mode.compare_mode_1 = DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE;
            dnx_context_mode.context_ipmf1_mode.compare_mode_2 = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;
        }
        else if (dnx_field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            /** We create an IPMF1 context so we have a valid context to cascade from. */
            SHR_IF_ERR_EXIT(dnx_field_context_create
                            (unit, dnx_context_flags, DNX_FIELD_STAGE_IPMF1, &ipmf1_context_mode,
                             &dnx_context_mode.context_ipmf2_mode.cascaded_from));
        }
        sal_strncpy_s(dnx_context_mode.name, "Context_name", sizeof(dnx_context_mode.name));

        SHR_IF_ERR_EXIT(dnx_field_context_create
                        (unit, dnx_context_flags, dnx_field_stage, &dnx_context_mode, &dnx_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Context=%d  created \n %s%s%s", dnx_context_id, EMPTY, EMPTY, EMPTY);

        /** Calling the custom GET/COMPARE function to see if all info set is same as what we get */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_mode_compare_dnx
                        (unit, dnx_field_stage, dnx_context_id, &dnx_context_mode));

        /** Destroying the created context */
        SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, dnx_field_stage, dnx_context_id));

        /** Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        /**
         * Get API after destroying the context should return NOT_FOUND.
         */
        rv = dnx_field_context_mode_get(unit, dnx_field_stage, dnx_context_id, &dnx_get_context_mode);
        if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Context ID: %d does not exist  \r\n",
                        __func__, __LINE__, dnx_field_stage_text(unit, dnx_field_stage), dnx_context_id);
        }
        else
        {
            SHR_SET_CURRENT_ERR(rv);
        }
        if (dnx_field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            /** We create an IPMF1 context so we have a valid context to cascade from. */
            SHR_IF_ERR_EXIT(dnx_field_context_destroy
                            (unit, DNX_FIELD_STAGE_IPMF1, dnx_context_mode.context_ipmf2_mode.cascaded_from));
        }
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);

        /**
         *  --------------------- ALLOCATION AND DEALLOCATION TESTING SCENARIO ------------------------------
         */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_context_mode));
        dnx_context_mode.context_ipmf2_mode.cascaded_from = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
        dnx_context_flags = 0;
        /** Test scenario for create/destroy looped 10 times - Testing allocation/deallocation */
        for (context_iter = 0; context_iter < CTEST_DNX_FIELD_NOF_ALLOC_CYCLES; context_iter++)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "Allocation/Deallocation try: %d \n %s%s%s", context_iter, EMPTY, EMPTY, EMPTY);

            /** Creating a context */
            SHR_IF_ERR_EXIT(dnx_field_context_create
                            (unit, dnx_context_flags, dnx_field_stage, &dnx_context_mode, &dnx_context_id));

            LOG_INFO_EX(BSL_LOG_MODULE, "Context=%d  created \n %s%s%s", dnx_context_id, EMPTY, EMPTY, EMPTY);

            /** Destroying the created context */
            SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, dnx_field_stage, dnx_context_id));

            LOG_INFO_EX(BSL_LOG_MODULE, "Context=%d  destroyed \n %s%s%s", dnx_context_id, EMPTY, EMPTY, EMPTY);
        }

        /**
        *  --------------------- ILLEGAL CASCADING TESTING SCENARIO ------------------------------
        */

        /** Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

        if (dnx_field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            max_nof_contexts = dnx_data_field.stage.stage_info_get(unit, dnx_field_stage)->nof_contexts;;
            for (dnx_context_id = 0; dnx_context_id < max_nof_contexts; dnx_context_id++)
            {
                /*
                 * Checking if the context ID is allocated to iPMF2 but not to iPMF1,
                 * and it is not the default context.
                 */
                if (dnx_context_id != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
                {
                    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                                    (unit, DNX_FIELD_STAGE_IPMF2, dnx_context_id, &is_alloc));
                    if (is_alloc)
                    {
                        dnx_field_context_t cascaded_from;
                        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                        (unit, DNX_FIELD_STAGE_IPMF2, dnx_context_id, &cascaded_from));
                        if (dnx_context_id != cascaded_from)
                        {
                            if (free_context == 0)
                            {
                                free_context = dnx_context_id;
                            }
                            else
                            {
                                free_context_2 = dnx_context_id;
                                break;
                            }
                        }
                    }
                }
            }
            /**
             *  ---------------------  CASCADING FROM AND TO THE SAME CONTEXT ------------------------------
             */
            dnx_context_mode.context_ipmf2_mode.cascaded_from = free_context;
            dnx_context_id = free_context;
            dnx_context_flags = DNX_FIELD_CONTEXT_FLAG_WITH_ID;
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "Trying to create context %d in IPMF2 and cascade from context %d form IPMF1 \n %s%s",
                        dnx_context_id, dnx_context_mode.context_ipmf2_mode.cascaded_from, EMPTY, EMPTY);

            /** Creating a context in IPMF2 with same context as the one we cascade from in IPMF1, which should be illegal */
            rv = dnx_field_context_create(unit, dnx_context_flags, dnx_field_stage, &dnx_context_mode, &dnx_context_id);
            if (rv == _SHR_E_PARAM)
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "SUCCESS ON A NEGATIVE TEST: %s(), line %d, Stage: %s .Context cannot be created, cannot create context %d in IPMF2 "
                            "when cascading from the same context in IPMF1 \r\n",
                            __func__, __LINE__, dnx_field_stage_text(unit, dnx_field_stage), dnx_context_id);
            }
            else
            {
                SHR_SET_CURRENT_ERR(rv);
            }

            /**
             *  ---------------------  CASCADING FROM NON-ALLOCATED IPMF1 CONTEXT ------------------------------
             */
            dnx_context_mode.context_ipmf2_mode.cascaded_from = free_context_2;
            dnx_context_id = free_context;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "Trying to create context %d in IPMF2 and cascade from context %d form IPMF1 \n %s%s",
                        dnx_context_id, dnx_context_mode.context_ipmf2_mode.cascaded_from, EMPTY, EMPTY);

            /** Creating a context in IPMF2 which is cascading from non-allocated IPMF1 context */
            rv = dnx_field_context_create(unit, dnx_context_flags, dnx_field_stage, &dnx_context_mode, &dnx_context_id);
            if (rv == _SHR_E_PARAM)
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "SUCCESS ON A NEGATIVE TEST: %s(), line %d, Stage: %s .Context cannot be created, cannot cascade from a non-existing "
                            "IPMF1 context(%d)\r\n",
                            __func__, __LINE__, dnx_field_stage_text(unit, dnx_field_stage),
                            dnx_context_mode.context_ipmf2_mode.cascaded_from);
            }
            else
            {
                SHR_SET_CURRENT_ERR(rv);
            }
        }
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'test_type') is not implemented. Illegal parameter. %s\r\n",
                    __func__, __LINE__, test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_HASHDNX, original_severity_hashdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    if (field_context_test_clean)
    {
        CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - run context init sequence in diag shell
 *
 * \param [in] unit             - the unit number in system
 * \param [in,out] args         - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx context_start"
 */
shr_error_e
sh_dnx_field_context_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_context_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}

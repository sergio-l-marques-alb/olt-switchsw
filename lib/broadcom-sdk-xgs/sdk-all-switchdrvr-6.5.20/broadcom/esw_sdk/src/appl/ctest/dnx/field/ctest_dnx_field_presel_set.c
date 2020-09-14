/*
 * PMF preselection (Context Selection) application procedures for DNX
 *   Sets a preselector in each of the stages.
 *   Apply enhanced comparison between set and get.
 *   Read/modify/write to change existing presel lines
 *   Gets the values and compares.
 *   Negative test cases:
 *   1. Illegal presel_id
 *   2. Illegal program_id
 *   3. Illegal stage
 *   4. Illegal qual type
 *   5. Illegal NOF quals
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
#include <bcm/field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include "ctest_dnx_field_presel_set.h"
#include "ctest_dnx_field_utils.h"
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
/**
 * DEFINEs
 */
/**
 * \brief
 *   Keyword for test type on 'presel_set' command (data base testing)
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for stage of test on 'presel_set' command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_COUNT         "count"
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'presel_set' shell command
 * \remark
 */
 sh_sand_option_t Sh_dnx_field_presel_set_options[] = {
     /* Name */                                    /* Type */              /* Description */                                   /* Default */
    {CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,     "Type (level) of test (dnx or bcm)",                "DNX",   (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,     "Stage of test (ipmf1, ipmf2, ipmf3, epmf)",        "ipmf1", (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_COUNT,  SAL_FIELD_TYPE_UINT32,   "Number of times test will run",                    "1"},
    {NULL}
    /**
     *End of options list - must be last.
     */
};
/**
 * \brief
 *   List of tests for 'presel_set' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_presel_set_tests[] = {
    {"DNX_field_presel_set_1", "type=DNX stage=ipmf1 count=6", CTEST_POSTCOMMIT},
    {"DNX_field_presel_set_2", "type=DNX stage=ipmf2 count=6", CTEST_POSTCOMMIT},
    {"DNX_field_presel_set_3", "type=DNX stage=ipmf3 count=6", CTEST_POSTCOMMIT},
    {"DNX_field_presel_set_e", "type=DNX stage=epmf  count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 * Details for semantic field presel set test.
 */
sh_sand_man_t Sh_dnx_field_presel_set_man = {
    "Field presel set related test utilities",
    "Start a semantic test for field presel, set, get and compare."
        "Testing may be for various types: 'bcm' or 'dnx' \r\n"
        "Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n"
        "The 'count' variable defines how many times the test will run. \r\n",
    "ctest field presel_set type=<BCM | DNX> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> count = 1",
    "type=DNX stage=IPMF1\r\n"
};
/* *INDENT-OFF* */
/**
 * User-provided IDs of each preselector.
 * No need to be unique.
 */
static  dnx_field_presel_t  Ctest_dnx_field_presel_id_arr[CTEST_DNX_PRESEL_SET_NUM_OF_PRESELS] = {40,      41,     42,     43};
/**
 * Initialization table for Ctest_dnx_field_presel_config_map, which configures
 * needed information for each preselector
 * Each line configures the NUM_OF_QUALS, ENTRY_VALID and CONTEXT_ID
 * ENTRY_VALID - must be TRUE or FALSE
 * CONTEXT_ID  - must be in a range[0-63]
 * NUM_OF_QUALS - must be under DNX_FIELD_CS_QUAL_NOF_MAX.
 */
static dnx_ctest_presel_info_t Ctest_dnx_field_presel_config_map[CTEST_DNX_PRESEL_SET_NOF_STAGES] =
{
    /** NUM_OF_QUALS. ENTRY_VALID, CONTEXT_ID */
    {2,                  TRUE,          50},
    /** NUM_OF_QUALS. ENTRY_VALID, CONTEXT_ID */
    {1,                  TRUE,          40},
    /** NUM_OF_QUALS. ENTRY_VALID, CONTEXT_ID */
    {1,                  TRUE,          51},
    /** NUM_OF_QUALS. ENTRY_VALID, CONTEXT_ID */
    {1,                  TRUE,          50}
};
/**
 * Initialization table for Ctest_dnx_field_presel_qual_config_map, which configures
 * all of the fields in dnx_field_presel_qual_data_t structure
 * Each line configures the QUAL_TYPE, QUAL_VALUE and QUAL_MASK
 */
static dnx_ctest_presel_qual_t Ctest_dnx_field_presel_qual_config_map[CTEST_DNX_PRESEL_SET_NOF_STAGES][CTEST_DNX_PRESEL_SET_NUM_OF_QUALS] =
{
    /** The data for first qual                                                       The data for second qual */
    /** QUAL_TYPE                        QUAL_VALUE                       QUAL_MASK   QUAL_TYPE                                 QUAL_VALUE     QUAL_MASK */
    {{DBAL_FIELD_FWD_CONTEXT_PROFILE,    bcmFieldForwardingTypeMpls,      0x1f},      {DBAL_FIELD_INCOMING_TAG_STRUCTURE,       3,             0x1f}},
    /** QUAL_TYPE                        QUAL_VALUE                       QUAL_MASK   QUAL_TYPE                                 QUAL_VALUE     QUAL_MASK */
    {{DBAL_FIELD_PMF1_TCAM_ACTION_0_MSB, 5,                               0xf},       {0,                                       0,             0}},
    /** QUAL_TYPE                        QUAL_VALUE                       QUAL_MASK   QUAL_TYPE                                 QUAL_VALUE     QUAL_MASK */
    {{DBAL_FIELD_FWD_LAYER_TYPE_0,       bcmFieldForwardingTypeIp4Ucast,  0xf},       {0,                                       0,             0}},
    /** QUAL_TYPE                        QUAL_VALUE                       QUAL_MASK   QUAL_TYPE                                 QUAL_VALUE     QUAL_MASK */
    {{DBAL_FIELD_ETHERNET_TAG_FORMAT,    0x123,                           0x3ff},     {0,                                       0,             0}}
};
/**
 * Used in Negative cases for setting different error codes
 * in different cases
 */
static _shr_error_t Ctest_dnx_field_presel_expected_errors[CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_NOF-1] = {
        _SHR_E_PARAM,
        _SHR_E_PARAM,
        _SHR_E_INTERNAL,
        _SHR_E_INTERNAL,
        _SHR_E_PARAM
};
/* *INDENT-ON* */
/**
 * \brief
 *
 *  This function contains the Field_presel_set
 *   application.This function sets all required HW
 *   configuration for field_presel_set to be performed.
 *
 * \param [in] unit - The unit number.
 * \param [in] presel_set_flags - Flags of field presel set API. Currently not used.
 * \param [in] presel_set_test_flag - Used to differ all of the test cases
 *                          For details see 'ctest_dnx_field_presel_set_test_flag_e'
 * \param [in] field_stage - Indicate the PMF stage
 * \param [in] presel_id_p - Pointer to presel_id
 * \param [in] quals_data_p - Pointer to array of qual data used for creation of presel key
 *                            It contains the qual type,value and mask for each qualifier
 *                            For details, see 'dnx_ctest_presel_qual_t'
 * \param [in] presel_entry_p - Pointer to all needed data for setting a preselector
 *                             (number of qualifiers, entry valid and context_id)
 *                              For details, see 'dnx_ctest_presel_info_t'
 * \param [out] presel_entry_id_p - pointer to database info that is to be filled in
 *             by this procedure. For details, see dnx_field_presel_entry_id_t struct
 *             The info stored in this structure is the ID of preselector and the stage.
 * \param [out] presel_entry_data_p - pointer to database info that is to be filled
 *             in by this procedure. For details, see 'dnx_field_presel_entry_data_t'
 *             The info stored in this structure is entry_valid, context_id, nof_quals and
 *             qual_data - array which holds the data(qual type,value and mask) for each qual
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
test_field_presel_set(
    int unit,
    uint32 presel_set_flags,
    ctest_dnx_field_presel_set_negative_test_flag_e presel_set_test_flag,
    dnx_field_stage_e field_stage,
    dnx_field_presel_t presel_id_p,
    dnx_ctest_presel_qual_t * quals_data_p,
    dnx_ctest_presel_info_t * presel_entry_p,
    dnx_field_presel_entry_id_t * presel_entry_id_p,
    dnx_field_presel_entry_data_t * presel_entry_data_p)
{
    int qual_indx = 0;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
   /**
    * Init all of the parameters in the structure dnx_field_presel_entry_id_t
    */
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, presel_entry_id_p));
   /**
    * Init all of the parameters in the structure dnx_field_presel_entry_data_t
    */
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, presel_entry_data_p));
    /**
     * Assign qualifiers data in params structure
     */
    presel_entry_id_p->presel_id = presel_id_p;
    presel_entry_id_p->stage = field_stage;
    presel_entry_data_p->context_id = presel_entry_p->context_id;
    presel_entry_data_p->nof_qualifiers = presel_entry_p->nof_quals;
    presel_entry_data_p->entry_valid = presel_entry_p->entry_valid;
    /**
     * Configure qualifiers data
     */
    for (qual_indx = 0; qual_indx < presel_entry_p->nof_quals; qual_indx++)
    {
        presel_entry_data_p->qual_data[qual_indx].qual_type_dbal_field = quals_data_p[qual_indx].qual_type;
        presel_entry_data_p->qual_data[qual_indx].qual_value = quals_data_p[qual_indx].qual_value;
        presel_entry_data_p->qual_data[qual_indx].qual_mask = quals_data_p[qual_indx].qual_mask;
    }
    /**
     * For negative case1: Illegal presel ID
     */
    if (presel_set_test_flag == CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_PRESEL_ID)
    {
        presel_entry_id_p->presel_id = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CS_LINES;
    }
    /**
     * For negative case2: Illegal context ID
     */
    else if (presel_set_test_flag == CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_CONTEXT_ID)
    {
        presel_entry_data_p->context_id = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS;
    }
    /**
     * For negative case3: Illegal stage
     */
    else if (presel_set_test_flag == CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_STAGE)
    {
        presel_entry_id_p->stage = DNX_FIELD_STAGE_NOF;
    }
    /**
     * For negative case5: Illegal number of quals used for key creation
     */
    else if (presel_set_test_flag == CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_NOF_QUALS)
    {
        presel_entry_data_p->nof_qualifiers = DNX_FIELD_CS_QUAL_NOF_MAX;
    }
    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing presel set with ID: %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, field_stage), presel_entry_id_p->presel_id);

    if (presel_set_test_flag == CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_POSITIVE_TEST)
    {
        SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, presel_set_flags, presel_entry_id_p, presel_entry_data_p));
    }
    /**
     * In all negative cases the error value is needed to be compare with
     * the expected error located in Ctest_dnx_field_user_quals_expected_errors array.
     * So the error is set using SHR_SET_CURRENT_ERR.
     */
    else
    {
        rv = dnx_field_presel_set(unit, presel_set_flags, presel_entry_id_p, presel_entry_data_p);
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
 * This procedure is used by appl_dnx_field_presel_set_semantic()
 *    to compare between info corresponding to 'set' and to 'get'
 *
 * \param [in] unit - The unit number.
 * \param [in] presel_entry_id - The presel ID
 * \param [in] set_presel_entry_data_p - pointer to SET database info from presel_set
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
presel_compare_function(
    int unit,
    dnx_field_presel_entry_id_t presel_entry_id,
    dnx_field_presel_entry_data_t * set_presel_entry_data_p)
{
    int presel_set_flags;
    int qual_indx;
    int qual_indx_get;
    dnx_field_presel_entry_data_t get_presel_entry_data;
    dnx_field_presel_qual_data_t set_qual_data, get_qual_data;
    SHR_FUNC_INIT_VARS(unit);
    presel_set_flags = 0;
    sal_memset(&get_qual_data, 0, sizeof(get_qual_data));
    sal_memset(&set_qual_data, 0, sizeof(set_qual_data));
    /**
     * Init all of the parameters in the structure dnx_field_presel_entry_data_t
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &get_presel_entry_data));
    SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, presel_set_flags, &presel_entry_id, &get_presel_entry_data));
    /**
     * Start the compare of the fields
     */
    if (set_presel_entry_data_p->entry_valid != get_presel_entry_data.entry_valid)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "bcm_field_presel_get: Unexpected data returned.\n"
                     "entry_valid expected %d received %d.\n", set_presel_entry_data_p->entry_valid,
                     get_presel_entry_data.entry_valid);
    }
    /**
     * Only compare the rest of the entry if the entry is valid.
     */
    if (set_presel_entry_data_p->entry_valid)
    {
        if (set_presel_entry_data_p->context_id != get_presel_entry_data.context_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "bcm_field_presel_get: Unexpected data returned.\n"
                         "Context_id expected %d received %d.\n", set_presel_entry_data_p->context_id,
                         get_presel_entry_data.context_id);
        }
        if (set_presel_entry_data_p->nof_qualifiers != get_presel_entry_data.nof_qualifiers)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "bcm_field_presel_get: Unexpected data returned.\n"
                         "nof_qualifiers expected %d received %d.\n", set_presel_entry_data_p->nof_qualifiers,
                         get_presel_entry_data.nof_qualifiers);
        }
        /**
         * Iterate through the number of set qualifiers twice
         * Compare between their types till the match is found
         * Compare the value and the mask of the selected type
         */
        for (qual_indx = 0; qual_indx < set_presel_entry_data_p->nof_qualifiers; qual_indx++)
        {
            for (qual_indx_get = 0; qual_indx_get < set_presel_entry_data_p->nof_qualifiers; qual_indx_get++)
            {
                set_qual_data = set_presel_entry_data_p->qual_data[qual_indx];
                get_qual_data = get_presel_entry_data.qual_data[qual_indx_get];
                if (set_qual_data.qual_type_dbal_field == get_qual_data.qual_type_dbal_field)
                {
                    if (set_qual_data.qual_value != get_qual_data.qual_value)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "bcm_field_presel_get: Unexpected data returned.\n"
                                     "qualifier %d value expected %d received %d.\n",
                                     qual_indx, set_qual_data.qual_value, get_qual_data.qual_value);
                    }
                    if (set_qual_data.qual_mask != get_qual_data.qual_mask)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "bcm_field_presel_get: Unexpected data returned.\n"
                                     "qualifier %d mask expected %d received %d.\n",
                                     qual_indx, set_qual_data.qual_mask, get_qual_data.qual_mask);
                    }
                    break;
                }
            }
            if (qual_indx_get == set_presel_entry_data_p->nof_qualifiers)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "bcm_field_presel_get: Unexpected data returned.\n"
                             "qualifier %d type expected %d received %d.\n",
                             qual_indx, set_qual_data.qual_type_dbal_field, get_qual_data.qual_type_dbal_field);
            }
        }
    }
    LOG_INFO_EX(BSL_LOG_MODULE, " Done.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
 * This procedure is used by appl_dnx_field_presel_set_semantic()
 *    to verify the RAW qualifiers mapping in field_map_data.c
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage to check mapping for.
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 */
static shr_error_e
ctest_dnx_field_presel_set_raw_map_check(
    int unit,
    dnx_field_stage_e field_stage)
{
    rhlist_t *cs_dnx_dump_list = NULL;
    cs_dnx_dump_t *cs_dnx_dump;
    shr_error_e rv;
    int dnx_cs_qual_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dump(unit, &cs_dnx_dump_list));

    RHITERATOR(cs_dnx_dump, cs_dnx_dump_list)
    {
        uint8 raw_qual_found = FALSE;
        cs_dnx_dump_t *cs_dnx_dump_raw;
        if (!cs_dnx_dump->field_id[field_stage])
        {
            continue;
        }

        rv = dbal_tables_field_size_get(unit,
                                        dnx_field_map_stage_info[field_stage].cs_table_id,
                                        cs_dnx_dump->field_id[field_stage], TRUE, 0, 0, &dnx_cs_qual_size);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }

        RHITERATOR(cs_dnx_dump_raw, cs_dnx_dump_list)
        {
            const dnx_field_cs_qual_map_t *cs_qual_map_raw_p = NULL;
            if (!cs_dnx_dump_raw->field_id[field_stage])
            {
                continue;
            }

            cs_qual_map_raw_p = &dnx_field_map_stage_info[field_stage].cs_qual_map[cs_dnx_dump_raw->bcm_qual];

            if ((cs_qual_map_raw_p->flags & BCM_TO_DNX_BASIC_OBJECT) &&
                (cs_dnx_dump->field_id[field_stage] == cs_dnx_dump_raw->field_id[field_stage]))
            {
                /**
                 * If we enter this case second time for same DBAL field, it means there are
                 * more than one appearances of current DBAL field, with BCM_TO_DNX_BASIC_OBJECT flag.
                 */
                if (raw_qual_found)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection DBAL field %d, has more than one appearances "
                                 "with flag BCM_TO_DNX_BASIC_OBJECT for %s stage.\n",
                                 cs_dnx_dump->field_id[field_stage], dnx_field_stage_e_get_name(field_stage));
                }
                raw_qual_found = TRUE;
            }
        }

        /**
         * If no raw qualifier was found, it means that the current qualifier don't have
         * the flag BCM_TO_DNX_BASIC_OBJECT and in such case we return an error.
         */
        if (!raw_qual_found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Context Selection DBAL field %d, doesn't have RAW qualifier mapping for %s stage.\n",
                         cs_dnx_dump->field_id[field_stage], dnx_field_stage_e_get_name(field_stage));
        }
    }

exit:
    if (cs_dnx_dump_list != NULL)
        utilex_rhlist_free_all(cs_dnx_dump_list);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'field_presel_set'
 *   testing application.
 *
 *  \param [in] unit - The unit number.
 *  \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 *  \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 *
 *  The test flow:
 *  1. Create a preselector
 *      with the field_presel_set API
 *  2. Compare the set fields and the get fields
 *  3. Negative testing: 5 different cases
 *     3.1. Illegal presel_id
 *     3.2. Illegal program_id
 *     3.3. Illegal stage
 *     3.4. Illegal qual type
 *     3.5. Illegal NOF quals
 */
static shr_error_e
appl_dnx_field_presel_set_semantic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int field_group_test_type_name;
    uint32 presel_set_flags;
    dnx_field_presel_entry_id_t presel_entry_id;
    dnx_field_presel_entry_data_t presel_entry_data;
    dnx_ctest_presel_qual_t negative_test_qual[CTEST_DNX_PRESEL_SET_NUM_OF_QUALS];
    dnx_field_context_mode_t context_mode;
    dnx_field_presel_t presel_indx;
    int negative_case_iterator;
    int rv;
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
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_TYPE, field_group_test_type_name);
    SH_SAND_GET_ENUM("stage", field_stage);
    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (field_group_test_type_name == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d,The test for %s level has not been implemented yet for %s. \r\n",
                    __func__, __LINE__, "BCM", dnx_field_stage_e_get_name(field_stage));
    }
    else if (field_group_test_type_name == 1)
    {

        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));

        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            context_mode.context_ipmf2_mode.cascaded_from = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
        }
        SHR_IF_ERR_EXIT(dnx_field_context_create(unit,
                                                 DNX_FIELD_CONTEXT_FLAG_WITH_ID,
                                                 field_stage,
                                                 &context_mode,
                                                 &Ctest_dnx_field_presel_config_map[field_stage].context_id));

        presel_set_flags = 0;
        /**
         * 1. Setting a preselector
         * 2. Compare the added and the get fields
         */
        for (presel_indx = CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_PRESEL_ID;
             presel_indx < CTEST_DNX_PRESEL_SET_NUM_OF_PRESELS; presel_indx++)
        {

            /**
             * Setting the preselector
             */
            SHR_IF_ERR_EXIT(test_field_presel_set
                            (unit, presel_set_flags, CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_POSITIVE_TEST, field_stage,
                             Ctest_dnx_field_presel_id_arr[presel_indx],
                             Ctest_dnx_field_presel_qual_config_map[field_stage],
                             &Ctest_dnx_field_presel_config_map[field_stage], &presel_entry_id, &presel_entry_data));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Set presel : %d \r\n", __func__, __LINE__,
                        dnx_field_stage_e_get_name(field_stage), presel_entry_id.presel_id);
            /**
             * Compare the get elements and the set elements
             */
            SHR_IF_ERR_EXIT(presel_compare_function(unit, presel_entry_id, &presel_entry_data));
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Stage: %s .Performing compare between the presel_set and presel_get: %d \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), presel_entry_id.presel_id);
            /**
             * Delete the preselector.
             */
            SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &presel_entry_data));
            presel_entry_data.entry_valid = FALSE;
            SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, presel_set_flags, &presel_entry_id, &presel_entry_data));
            SHR_IF_ERR_EXIT(presel_compare_function(unit, presel_entry_id, &presel_entry_data));

        }
        /**
          * NEGATIVE TESTING
          * {
          */
         /**
          * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
          */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, bslSeverityFatal);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
        /**
         * Iterating all of the negative cases
         * Starts from 1 because the first case is positive
         */
        for (negative_case_iterator = CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_PRESEL_ID;
             negative_case_iterator < CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_NOF; negative_case_iterator++)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TESTING: CASE: %d \r\n",
                        __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
            /**
             * Used for negative case 4
             * Switches the field_stage to field_stage+/-1 in order to indicate wrong one
             */
            if (negative_case_iterator == CTEST_DNX_FIELD_PRESEL_SET_TEST_FLAG_ILLEGAL_QUAL_TYPE)
            {
                if (field_stage == DNX_FIELD_STAGE_EPMF)
                {
                    sal_memcpy(negative_test_qual, Ctest_dnx_field_presel_qual_config_map[field_stage - 2],
                               sizeof(negative_test_qual));
                }
                else
                {
                    sal_memcpy(negative_test_qual, Ctest_dnx_field_presel_qual_config_map[field_stage + 1],
                               sizeof(negative_test_qual));
                }
            }
            else
            {
                sal_memcpy(negative_test_qual, Ctest_dnx_field_presel_qual_config_map[field_stage],
                           sizeof(negative_test_qual));
            }
            rv = test_field_presel_set(unit, presel_set_flags, negative_case_iterator, field_stage,
                                       Ctest_dnx_field_presel_id_arr[0],
                                       negative_test_qual,
                                       &Ctest_dnx_field_presel_config_map[field_stage],
                                       &presel_entry_id, &presel_entry_data);
            if (rv == Ctest_dnx_field_presel_expected_errors[negative_case_iterator - 1])
            {
                LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s NEGATIVE TEST CASE: %d was successful! \r\n",
                            __func__, __LINE__, dnx_field_stage_e_get_name(field_stage), negative_case_iterator);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Illegal presel from negative test case: %d was created! Test has failed!\r\n",
                             negative_case_iterator);
            }
        }
         /**
          *  Restore the original severity after the end of Negative test.
          */
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
         /**
          * }
          */

        /** Check RAW context selection qualifier mapping. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_presel_set_raw_map_check(unit, field_stage));
    }
exit:

    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_resmngr);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_DBALDNX, original_severity_dbaldnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);

    CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));

    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    run TCAM Look-up sequence in diag shell
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 */
shr_error_e
sh_dnx_field_presel_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_FIELD_PRESEL_SET_OPTION_TEST_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_field_presel_set_semantic(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

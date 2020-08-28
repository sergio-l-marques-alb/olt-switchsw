/** \file diag_dnx_field_group.c
 *
 * Database access (incl. TCAM Look-ups) application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_field_group.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>
#include <bcm_int/dnx/field/field_context.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * The various values available for 'mode' input.
 */
#define TEST_MODE_FG_ONLY       "FG_ONLY"
#define TEST_MODE_FG_PLUS_ENTRY "FG_PLUS_ENTRY"
#define TEST_MODE_FG_MULTI      "FG_MULTI"
#define TEST_MODE_FG_320_ENTRY  "FG_320_ENTRY"

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * Enum for both the identifying numbers of the
 * groups and their priorities.
 */
typedef enum
{
    GROUP_1 = 100,
    GROUP_1_PRI = 20,
    GROUP_2 = 110,
    GROUP_2_PRI = 20,
    GROUP_3 = 120,
    GROUP_3_PRI = 20
} bcm_group_ids_e;
/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for test type on 'group' command (data base testing)
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_GROUP_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for mode of test on 'group' command (data base testing)
 *   Mode can be either FG_ONLY, FG_PLUS_ENTRY or FG_MULTI
 */
#define CTEST_DNX_FIELD_GROUP_OPTION_TEST_MODE         "mode"
/**
 * \brief
 *   Keyword for stage of test on 'group' command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define CTEST_DNX_FIELD_GROUP_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_GROUP_OPTION_CLEAN        "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_ACTION_OPTION_TEST_COUNT        "count"

#define DATAQUAL_FOR_TEST
/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'group' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t dnx_field_group_options[] = {
     /* Name */                                 /* Type */              /* Description */                               /* Default */
    {CTEST_DNX_FIELD_GROUP_OPTION_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",                  "DNX",   (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_GROUP_OPTION_TEST_MODE,   SAL_FIELD_TYPE_STR,     "Mode of test (Group-create only or with entry)",     NULL    },
    {CTEST_DNX_FIELD_GROUP_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",             "ipmf1", (void *)Field_stage_enum_table},
    {CTEST_DNX_FIELD_GROUP_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",  "Yes"   },
    {CTEST_DNX_ACTION_OPTION_TEST_COUNT,       SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                      "1"     },
    {NULL}
    /** End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'group' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_field_group_tests[] = {
    {"DNX_fg_create_plus_entry_add_1", "type=DNX mode=FG_PLUS_ENTRY  stage=ipmf1 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_fg_create_plus_entry_add_2", "type=DNX mode=FG_PLUS_ENTRY  stage=ipmf2 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_fg_create_plus_entry_add_3", "type=DNX mode=FG_PLUS_ENTRY  stage=ipmf3 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_fg_create_plus_entry_add_e", "type=DNX mode=FG_PLUS_ENTRY  stage=epmf count=6 ",  CTEST_POSTCOMMIT},
    {"DNX_fg_create_mutl_plus_entry_1", "type=DNX mode=FG_MULTI  stage=ipmf1 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_fg_create_mutl_plus_entry_3", "type=DNX mode=FG_MULTI  stage=ipmf3 count=6 ", CTEST_POSTCOMMIT},
    {"DNX_fg_create_mutl_plus_entry_e", "type=DNX mode=FG_MULTI  stage=epmf count=6 ", CTEST_POSTCOMMIT},
    {"DNX_fg_tcam_ipmf1_320_entry",   "type=DNX mode=FG_320_ENTRY  stage=ipmf1 count=6 ", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
/**
 *  Database (TCAM) Look-ups leaf details
 */
sh_sand_man_t sh_dnx_field_group_man = {
    "Field group related test utilities",
    "Activate field group related test utilities (including TCAM Look-ups). This covers both "
        "BCM-level tests and DNX-level tests. Testing may be for 'field group only' (FG_ONLY)"
        "or 'field group plus single entry' (FG_PLUS_ENTRY)\r\n"
        "Also, Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf' \r\n",
    "ctest field group type=<BCM | DNX> mode=<FG_ONLY | FG_PLUS_ENTRY> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> clean=<YES | NO>",
    "type=DNX mode=FG_ONLY stage=IPMF2\n" "TY=DNX MD=FG_PLUS_ENTRY stage=IPMF3 clean=YES",
};

 /*
  * {
  */
 /* *INDENT-OFF* */



/* -------------------------------------------------------------------------------------- */

static bcm_field_qualify_t bcm_quals_array[CTEST_DNX_FIELD_GROUP_NUM_STAGES][CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
{       {bcmFieldQualifyVlanId, bcmFieldQualifyTpid,  bcmFieldQualifyMplsLabel},
        {bcmFieldQualifyVlanId, bcmFieldQualifyTpid,  bcmFieldQualifyMplsLabel},
        {bcmFieldQualifySrcPort,     bcmFieldQualifyDstRpfGport,bcmFieldQualifyRxTrapCodeForSnoop},
        {bcmFieldQualifyVlanId, bcmFieldQualifyTpid,  bcmFieldQualifyMplsLabel}
};

static uint32 bcm_qual_vals[CTEST_DNX_FIELD_GROUP_NUM_STAGES][CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
{
        {0x200,     0x0800,     0x1000},
        {0x300,     0x8847,     0x1500},
        {0x4321,    0x3333,     0x2   },
        {0x500,     0x8847,     0x2000}
};

static bcm_field_action_t bcm_actions_array[CTEST_DNX_FIELD_GROUP_NUM_STAGES][CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
{
        {bcmFieldActionSnoop,           bcmFieldActionForward,          bcmFieldActionDropPrecedence},
        {bcmFieldActionPphPresentSet,   bcmFieldActionPrioIntNew,       bcmFieldActionFabricHeaderSet},
        {bcmFieldActionPrioIntNew,      bcmFieldActionDropPrecedence,   bcmFieldActionSnoop},
        {bcmFieldActionPrioIntNew,      bcmFieldActionDropPrecedence,   bcmFieldActionCount}
};

static uint32 bcm_action_vals[CTEST_DNX_FIELD_GROUP_NUM_STAGES][CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS] =
{
        {0x5, 0x2000, 0x1},
        {0x1, 0x3,    0x6},
        {0x2, 0x1,    0x3},
        {0x4, 0x0,    0x1}
};

/* -------------------------------------------------------------------------------------- */

/*  Presel Qualifiers per stage */
static int presel_qualifiers_array[CTEST_DNX_FIELD_GROUP_NUM_PRESEL_QUALS] =
{   DBAL_FIELD_INCOMING_TAG_STRUCTURE,  /* IPMF1 */
    DBAL_FIELD_INCOMING_TAG_STRUCTURE,  /* IPMF2 */
    DBAL_FIELD_PTC_CS_PROFILE,             /* IPMF3 */
    DBAL_FIELD_OUT_LIF_PROFILE             /* EPMF  */
};


/* *INDENT-ON* */
/*
 * }
 */

/**
* \brief
*  Creates a Field Group with 320 bit key
* \param [in] unit  - Device Id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
ctest_dnx_field_tcam_fg_320_plus_entry(
    int unit)
{

    dnx_field_context_mode_t context_mode;
    dnx_field_context_t ctx_id = 0;
    dnx_field_group_t grp_id;
    dnx_field_group_info_t fg_info_in;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_presel_entry_id_t presel_entry_id;
    dnx_field_presel_entry_data_t presel_data;
    dnx_field_entry_t entry_info;
    dnx_field_stage_e stage_cfg = DNX_FIELD_STAGE_IPMF1;
    uint32 entry_handle = 9;
    dnx_field_stage_e stage_for_context_create;

    SHR_FUNC_INIT_VARS(unit);

    /**Configure FG*/
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info_in));
    fg_info_in.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    fg_info_in.field_stage = stage_cfg;

    /**2msb disabled and we want only one action*/
    fg_info_in.dnx_actions[0] = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, stage_cfg, DBAL_ENUM_FVAL_IPMF1_ACTION_DP);

    fg_info_in.dnx_quals[0] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, stage_cfg, DNX_FIELD_HEADER_QUAL_IPV4_SRC);
    fg_info_in.dnx_quals[1] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, stage_cfg, DNX_FIELD_HEADER_QUAL_IPV4_DST);
    fg_info_in.dnx_quals[2] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, stage_cfg, DNX_FIELD_HEADER_QUAL_MAC_SRC);
    fg_info_in.dnx_quals[3] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, stage_cfg, DNX_FIELD_HEADER_QUAL_MAC_DST);
    fg_info_in.dnx_quals[4] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, stage_cfg, DNX_FIELD_HEADER_QUAL_ETHERTYPE);
    fg_info_in.dnx_quals[5] = DNX_FIELD_QUAL_TYPE_INVALID;

    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &fg_info_in, &grp_id));

    /** Create Context. For both iPMF1 and iPMF2, create an iPMF1/2 pair.*/
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    if (stage_cfg == DNX_FIELD_STAGE_IPMF2)
    {
        stage_for_context_create = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        stage_for_context_create = stage_cfg;
    }
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, stage_for_context_create, &context_mode, &ctx_id));

    /**Configure attach API info*/
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
    attach_info.dnx_actions[0] = fg_info_in.dnx_actions[0];
    attach_info.dnx_quals[0] = fg_info_in.dnx_quals[0];
    attach_info.dnx_quals[1] = fg_info_in.dnx_quals[1];
    attach_info.dnx_quals[2] = fg_info_in.dnx_quals[2];
    attach_info.dnx_quals[3] = fg_info_in.dnx_quals[3];
    attach_info.dnx_quals[4] = fg_info_in.dnx_quals[4];

    attach_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 8);

    attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    attach_info.qual_info[0].input_arg = 1;
    attach_info.qual_info[0].offset = 0;
    attach_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    attach_info.qual_info[1].input_arg = 1;
    attach_info.qual_info[1].offset = 0;
    attach_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    attach_info.qual_info[2].input_arg = 0;
    attach_info.qual_info[2].offset = 0;
    attach_info.qual_info[3].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    attach_info.qual_info[3].input_arg = 0;
    attach_info.qual_info[3].offset = 0;
    attach_info.qual_info[4].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
    attach_info.qual_info[4].input_arg = 0;
    attach_info.qual_info[4].offset = 0;
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, grp_id, ctx_id, &attach_info));

    /**Configure TCAM entry*/
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));

    entry_info.priority = 7;

    entry_info.key_info.qual_info[0].dnx_qual = fg_info_in.dnx_quals[0];
    entry_info.key_info.qual_info[1].dnx_qual = fg_info_in.dnx_quals[1];
    entry_info.key_info.qual_info[2].dnx_qual = fg_info_in.dnx_quals[2];
    entry_info.key_info.qual_info[3].dnx_qual = fg_info_in.dnx_quals[3];
    entry_info.key_info.qual_info[4].dnx_qual = fg_info_in.dnx_quals[4];

    /**IPv4 SRC 32bit*/
    entry_info.key_info.qual_info[0].qual_value[0] = 1;
    entry_info.key_info.qual_info[0].qual_mask[0] = 0xFFFFFFFF;
    /**IPv4 DST 32bit*/
    entry_info.key_info.qual_info[1].qual_value[0] = 2;
    entry_info.key_info.qual_info[1].qual_mask[0] = 0xFFFFFFFF;
    /**MAC SRC 48bit*/
    entry_info.key_info.qual_info[2].qual_value[0] = 3;
    entry_info.key_info.qual_info[2].qual_mask[0] = 0xFFFFFFFF;
    entry_info.key_info.qual_info[2].qual_mask[1] = 0xFFFF;
    /**MAC DST 48bit*/
    entry_info.key_info.qual_info[3].qual_value[0] = 4;
    entry_info.key_info.qual_info[3].qual_mask[0] = 0xFFFFFFFF;
    entry_info.key_info.qual_info[3].qual_mask[1] = 0xFFFF;
    /**ETHER TYPE 16bit*/
    entry_info.key_info.qual_info[4].qual_value[0] = 0x800;
    entry_info.key_info.qual_info[4].qual_mask[0] = 0xFFFF;

    entry_info.payload_info.action_info[0].dnx_action = fg_info_in.dnx_actions[0];
    entry_info.payload_info.action_info[0].action_value[0] = 2;
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, grp_id, &entry_info, &entry_handle));

    /**Config Presel*/
    presel_entry_id.presel_id = CTEST_DNX_FIELD_GROUP_PRESEL_ID;
    presel_entry_id.stage = stage_cfg;
    presel_data.context_id = ctx_id;
    presel_data.entry_valid = TRUE;
    presel_data.nof_qualifiers = 1;
    presel_data.qual_data[0].qual_type_dbal_field = presel_qualifiers_array[stage_cfg];
    presel_data.qual_data[0].qual_value = CTEST_DNX_FIELD_GROUP_QUAL_VALUE;
    presel_data.qual_data[0].qual_mask = CTEST_DNX_FIELD_GROUP_QUAL_MASK;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is basic the TCAM Look-ups
 *   application.This function sets all required HW
 *   configuration for creating TCAM Look-ups to be performed.
 *   It creates 80 , 160 and 320 bit keys.
 *
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_field_group_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

/* { */
    dnx_field_stage_e field_stage;
    int field_group_test_type_name;
    char *field_group_test_mode_name;
    int field_group_test_clean;
    dnx_field_group_t fg_id[CTEST_DNX_FIELD_GROUP_MULTI_NUM_FGS];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_GROUP_OPTION_TEST_TYPE, field_group_test_type_name);
    SH_SAND_GET_STR(CTEST_DNX_FIELD_GROUP_OPTION_TEST_MODE, field_group_test_mode_name);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_GROUP_OPTION_TEST_STAGE, field_stage);
    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_GROUP_OPTION_CLEAN, field_group_test_clean);

    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (field_group_test_type_name == 0)
    {
        LOG_CLI((BSL_META
                 ("%s(), line %d, %s level has not been implemented yet for %s. \r\n"),
                 __FUNCTION__, __LINE__, "BCM", dnx_field_stage_text(unit, field_stage)));
    }
    else if (field_group_test_type_name == 1)
    {
        int ii;
        dnx_field_presel_entry_id_t presel_entry_id;
        dnx_field_presel_entry_data_t presel_data;
        ctest_dnx_field_util_basic_tcam_fg_t tcam_fg_util;
        dnx_field_context_mode_t context_mode;
        dnx_field_context_t context_id;

        if ((sal_strcasecmp(field_group_test_mode_name, TEST_MODE_FG_ONLY) == 0)
            || (sal_strcasecmp(field_group_test_mode_name, TEST_MODE_FG_PLUS_ENTRY) == 0)
            || (sal_strcasecmp(field_group_test_mode_name, TEST_MODE_FG_MULTI) == 0))
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Testing database creation on %s level: %s()\r\n",
                        __FUNCTION__, __LINE__, "DNX", "dnx_field_group_create");

            /*
             * LOG_INFO_EX(BSL_LOG_MODULE, "==> Stage is %s. Selected field group identifier is %d%s%s\r\n", stage_text,
             * fg_id_array[field_stage], EMPTY, EMPTY);
             */

            context_id = 16;
            SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));

            SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &presel_data));

            presel_entry_id.presel_id = CTEST_DNX_FIELD_GROUP_PRESEL_ID;
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                /**iPMF2 must!! to set iPMF1 contesxt selection */
                presel_entry_id.stage = DNX_FIELD_STAGE_IPMF1;
            }
            else
            {
                presel_entry_id.stage = field_stage;
            }

            SHR_IF_ERR_EXIT(dnx_field_context_create
                            (unit, DNX_FIELD_CONTEXT_FLAG_WITH_ID, presel_entry_id.stage, &context_mode, &context_id));

            presel_data.context_id = context_id;
            presel_data.nof_qualifiers = CTEST_DNX_FIELD_GROUP_NOF_PRESEL;
            presel_data.entry_valid = 1;
            presel_data.qual_data[0].qual_type_dbal_field = presel_qualifiers_array[field_stage];
            presel_data.qual_data[0].qual_value = CTEST_DNX_FIELD_GROUP_QUAL_VALUE;
            presel_data.qual_data[0].qual_mask = CTEST_DNX_FIELD_GROUP_QUAL_MASK;
            SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));

            SHR_IF_ERR_EXIT(ctest_dnx_field_util_basic_tcam_fg_t_init(unit, &tcam_fg_util));

            tcam_fg_util.context = 5;
            tcam_fg_util.field_stage = field_stage;

            for (ii = 0; ii < CTEST_DNX_FIELD_UTIL_TESTER_NUM_QUALS_ACTIONS; ii++)
            {
                tcam_fg_util.bcm_actions[ii] = bcm_actions_array[field_stage][ii];
                tcam_fg_util.action_values[ii].value[0] = bcm_action_vals[field_stage][ii];

                tcam_fg_util.bcm_quals[ii] = bcm_quals_array[field_stage][ii];
                tcam_fg_util.qual_values[ii].value[0] = bcm_qual_vals[field_stage][ii];
            }

            if (sal_strcasecmp(field_group_test_mode_name, TEST_MODE_FG_PLUS_ENTRY) == 0)
            {
                SHR_IF_ERR_EXIT(ctest_dnx_field_util_basic_tcam_fg_full(unit, &tcam_fg_util, &fg_id[0]));
            }
            else if (sal_strcasecmp(field_group_test_mode_name, TEST_MODE_FG_MULTI) == 0)
            {
                for (ii = 0; ii < CTEST_DNX_FIELD_GROUP_MULTI_NUM_FGS; ii++)
                {
                    tcam_fg_util.context = CTEST_DNX_FIELD_GROUP_DEFAULT_CONTEXT + ii;
                    SHR_IF_ERR_EXIT(ctest_dnx_field_util_basic_tcam_fg_full(unit, &tcam_fg_util, &fg_id[ii]));
                }
            }

        }
        else if (sal_strcasecmp(field_group_test_mode_name, TEST_MODE_FG_320_ENTRY) == 0)
        {
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                    ctest_dnx_field_tcam_fg_320_plus_entry(unit);
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_stage = %d not supported 320 key\n", field_stage);
                    break;
            }

        }
        else
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Options %s %s. Not implemented. Illegal parameter. Return with 'success' flag\r\n",
                        __FUNCTION__, __LINE__, EMPTY, EMPTY);
        }

        if (field_group_test_clean == TRUE)
        {
            CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - run TCAM Look-up sequence in diag shell
 */
shr_error_e
sh_dnx_field_group_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32(CTEST_DNX_ACTION_OPTION_TEST_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_field_group_starter(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

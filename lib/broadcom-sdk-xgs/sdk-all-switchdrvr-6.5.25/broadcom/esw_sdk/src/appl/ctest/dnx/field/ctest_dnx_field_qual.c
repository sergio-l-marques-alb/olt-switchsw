/** \file diag_dnx_field_qual.c
 * $Id$
 *
 * Qualifiers and actions testing application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
#include "ctest_dnx_field_qual.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>
#include <bcm_int/dnx/field/field_context.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#define INVALID_QUAL_ID -1
/*
 * The various values available for 'mode' input.
 */
#define TEST_MODE_ACTIONS       "actions"
#define TEST_MODE_QUALIFIERS    "quals"
/*
*   Value defines
*/
/* Qualifiers and actions per stage array initializer */
#define NUM_QUALS_AND_ACTIONS_PER_STAGE     4
/* Random legal value and mask for preselector */
#define QUAL_VALUEA_AND_MASK                0x1
/* Number of qualifiers for preselector */
#define NOF_QUALIFIERS                      1
/* Set valid for preselector entry_valid 0/1 */
#define ENTRY_ENABLE                        1
/* Indicating which layer we want Header Qualifiers to look at */
#define FIRST_LAYER                         1
/*
 * We set 1 valid qualifier and 1 valid action per FG
 * VALID_INDEX points at the valid index in the array.
 * INVALID_INDEX points at the invalid index in the array.
 */
#define VALID_INDEX                         0
#define INVALID_INDEX                       1
/* Indicating parameters if the test mode is actions or qualifiers */
#define TESTING_QUALIFIERS                  1
#define TESTING_ACTIONS                     2

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */

/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for test type on 'qual' command (data base testing)
 *   Type can be either BCM or DNX
 */
#define DNX_DIAG_FIELD_QUAL_OPTION_TEST_TYPE        "type"
/**
 * \brief
 *   Keyword for mode of test on 'qual' command
 *   Mode can be either 'actions', 'quals'
 */
#define DNX_DIAG_FIELD_QUAL_OPTION_TEST_MODE        "mode"
/**
 * \brief
 *   Keyword for stage of test on 'qual' command (data base testing)
 *   stage can be either IPMF1, IPMF2 ,IPMF3, EPMF .
 */
#define DNX_DIAG_FIELD_QUAL_OPTION_TEST_STAGE       "stage"
/**
 * \brief
 *   Keyword for TCL testing of test on 'qual' command (data base testing)
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_FIELD_QUAL_OPTION_CLEAN            "clean"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "count"

#define DATAQUAL_FOR_TEST
/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */

/*
 * Input shell parameters for the test.
 */
sh_sand_option_t dnx_field_qual_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {DNX_DIAG_FIELD_QUAL_OPTION_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,     "Type (level) of test (dnx or bcm)",               "DNX", (void *)Field_level_enum_table},
    {DNX_DIAG_FIELD_QUAL_OPTION_TEST_MODE,   SAL_FIELD_TYPE_STR,     "Mode of test 'quals' or 'actions'",               NULL},
    {DNX_DIAG_FIELD_QUAL_OPTION_TEST_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",          "ipmf1", (void *)Field_stage_enum_table},
    {DNX_DIAG_FIELD_QUAL_OPTION_CLEAN,       SAL_FIELD_TYPE_BOOL,    "Indicates if the test will perform a clean-up",   "Yes"},
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",                   "1"},
    {NULL}
    /** End of options list - must be last. */
};
 /**
 * \brief
 *   List of tests for 'qual' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for postcommit.
 */
sh_sand_invoke_t sh_dnx_field_qual_tests[] = {
    {"qualifiers_test_1", "type=DNX mode=quals stage=ipmf1   clean=Yes count=6", CTEST_POSTCOMMIT},
    {"qualifiers_test_2", "type=DNX mode=quals stage=ipmf2   clean=Yes count=6", CTEST_POSTCOMMIT},
    {"qualifiers_test_3", "type=DNX mode=quals stage=ipmf3   clean=Yes count=6", CTEST_POSTCOMMIT},
    {"qualifiers_test_4", "type=DNX mode=quals stage=epmf    clean=Yes count=6", CTEST_POSTCOMMIT},
    {"actions_test_1",    "type=DNX mode=actions stage=ipmf1 clean=Yes count=6", CTEST_POSTCOMMIT},
    {"actions_test_2",    "type=DNX mode=actions stage=ipmf2 clean=Yes count=6", CTEST_POSTCOMMIT},
    {"actions_test_3",    "type=DNX mode=actions stage=ipmf3 clean=Yes count=6", CTEST_POSTCOMMIT},
    {"actions_test_4",    "type=DNX mode=actions stage=epmf  clean=Yes count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
/**
 *  Qualifiers and Actions semantic testing details.
 */
sh_sand_man_t sh_dnx_field_qual_man = {
    "Field qualifiers and actions related test utilities",
    "Activate field qualifiers and actions related test utilities. This covers both "
        "BCM-level tests and DNX-level tests. Testing may be for 'actions or qualifiers\n"
        "Also, Testing may be for various stages: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf'\n",
    "ctest field qual type=<BCM | DNX> mode=<actions | quals> stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> clean=<0 | 1>",
    "type=dnx"
};

/*  Presel Qualifiers per stage */
static int presel_qualifiers_array[NUM_QUALS_AND_ACTIONS_PER_STAGE] = {
    DBAL_FIELD_PMF_TAG_STRUCTURE,       /* IPMF1 */
    DBAL_FIELD_PMF_TAG_STRUCTURE,       /* IPMF2 */
    DBAL_FIELD_PTC_CS_PROFILE,  /* IPMF3 */
    DBAL_FIELD_PMF_OUT_LIF_PROFILE      /* EPMF */
};

/*Actions per stage, used when testing qualifiers(always use the same action for the selected stage) */
static dnx_field_action_id_t action_id_array[NUM_QUALS_AND_ACTIONS_PER_STAGE] = {
    DNX_FIELD_ACTION_GLOB_IN_LIF_0,     /* IPMF1 */
    DNX_FIELD_ACTION_DST_DATA,  /* IPMF2 */
    DNX_FIELD_ACTION_ACL_CONTEXT,       /* IPMF3 */
    DNX_FIELD_ACTION_TC /* EPMF */
};

/*Qualifiers per stage , used when testing actions(always use the same qualifier for the selected stage)*/
static dnx_field_qual_t quals_array[NUM_QUALS_AND_ACTIONS_PER_STAGE] = {
    DNX_FIELD_QUAL_TC,  /* IPMF1 */
    DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0,  /* IPMF2 */
    DNX_FIELD_QUAL_RPF_ECMP_MODE,       /* IPMF3 */
    DNX_FIELD_QUAL_PPH_NWK_QOS  /* EPMF */
};

/*Qualifier information to pass to function.*/
typedef struct
{
    dnx_field_qual_t dnx_qual;
    dnx_field_input_type_e input_type;
} ctest_dnx_field_qual_qual_info_t;

static shr_error_e
preselector_set(
    int unit,
    dnx_field_stage_e field_stage)
{
    dnx_field_presel_entry_id_t presel_entry_id;
    dnx_field_presel_qual_data_t presel_qual[1];
    dnx_field_presel_entry_data_t presel_data;

    SHR_FUNC_INIT_VARS(unit);

    presel_qual[VALID_INDEX].qual_type_dbal_field = presel_qualifiers_array[field_stage];
    presel_qual[VALID_INDEX].qual_value = QUAL_VALUEA_AND_MASK;
    presel_qual[VALID_INDEX].qual_mask = QUAL_VALUEA_AND_MASK;

    presel_entry_id.presel_id = 0;
    presel_entry_id.stage = ((field_stage - 1) ? field_stage : DNX_FIELD_STAGE_IPMF1);

    presel_data.context_id = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    presel_data.nof_qualifiers = NOF_QUALIFIERS;
    presel_data.entry_valid = ENTRY_ENABLE;
    sal_memcpy(&(presel_data.qual_data[0]), &(presel_qual[0]), sizeof(presel_data.qual_data[0]));

    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));

exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
field_group_fill_and_create(
    int unit,
    dnx_field_action_t dnx_action,
    dnx_field_stage_e field_stage,
    dnx_field_group_t * fg_id,
    dnx_field_group_info_t * fg_info,
    dnx_field_qual_t qual,
    int test_mode)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, fg_info));
    /*
     * Select stage from dnx_field_stage_e
     */
    fg_info->field_stage = field_stage;
    /*
     * Select type from dnx_field_group_type_e
     */
    fg_info->fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    if (test_mode == TESTING_QUALIFIERS)
    {
        fg_info->dnx_quals[VALID_INDEX] = qual;
        /*
         * Note that, in this test, currently, we are only referring to the first '2msb combination'
         */
        fg_info->dnx_actions[VALID_INDEX] =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, action_id_array[fg_info->field_stage]);
    }
    else if (test_mode == TESTING_ACTIONS)
    {
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            fg_info->dnx_quals[VALID_INDEX] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, field_stage, qual);
        }
        else
        {
            fg_info->dnx_quals[VALID_INDEX] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, qual);
        }
        /*
         * Note that, in this test, currently, we are only referring to the first '2msb combination'
         */
        fg_info->dnx_actions[VALID_INDEX] = dnx_action;
    }
    fg_info->dnx_quals[INVALID_INDEX] = DNX_FIELD_QUAL_TYPE_INVALID;
    fg_info->dnx_actions[INVALID_INDEX] = DNX_FIELD_ACTION_INVALID;

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s .Performing group add %s \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, fg_info->field_stage), EMPTY);
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, fg_info, fg_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
field_context_fill_and_attach(
    int unit,
    dnx_field_group_info_t * fg_info,
    void *info,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int test_mode)
{
    dnx_field_group_attach_info_t attach_info;
    dnx_field_qual_class_e class;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
    if (test_mode == TESTING_QUALIFIERS)
    {
        ctest_dnx_field_qual_qual_info_t *qual_info = (ctest_dnx_field_qual_qual_info_t *) info;
        class = DNX_QUAL_CLASS(qual_info->dnx_qual);
        if (class == DNX_FIELD_QUAL_CLASS_HEADER)
        {
            attach_info.qual_info[VALID_INDEX].input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
            attach_info.qual_info[VALID_INDEX].input_arg = FIRST_LAYER;
            attach_info.qual_info[VALID_INDEX].offset = 0;
        }
        else if (class == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
        {
            attach_info.qual_info[VALID_INDEX].input_type = DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD;
            attach_info.qual_info[VALID_INDEX].input_arg = FIRST_LAYER;
            attach_info.qual_info[VALID_INDEX].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
        }
        else if (class == DNX_FIELD_QUAL_CLASS_META2)
        {

            attach_info.qual_info[VALID_INDEX].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
            attach_info.qual_info[VALID_INDEX].input_arg = DNX_FIELD_ATTACH_INFO_FIELD_INPUT_ARG_DEFAULT;
            attach_info.qual_info[VALID_INDEX].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
        }
        else
        {
            attach_info.qual_info[VALID_INDEX].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
            attach_info.qual_info[VALID_INDEX].input_arg = DNX_FIELD_ATTACH_INFO_FIELD_INPUT_ARG_DEFAULT;
            attach_info.qual_info[VALID_INDEX].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
        }

    }
    else
    {
        if (fg_info->field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            attach_info.qual_info[VALID_INDEX].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
        }
        else
        {
            attach_info.qual_info[VALID_INDEX].input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
        }
        attach_info.qual_info[VALID_INDEX].input_arg = DNX_FIELD_ATTACH_INFO_FIELD_INPUT_ARG_DEFAULT;
        attach_info.qual_info[VALID_INDEX].offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
    }

    attach_info.dnx_quals[VALID_INDEX] = fg_info->dnx_quals[VALID_INDEX];
    attach_info.dnx_quals[INVALID_INDEX] = fg_info->dnx_quals[INVALID_INDEX];
    /*
     * Note that, in this test, currently, we are only referring to the first '2msb combination'
     */
    attach_info.dnx_actions[VALID_INDEX] = fg_info->dnx_actions[VALID_INDEX];
    attach_info.dnx_actions[INVALID_INDEX] = fg_info->dnx_actions[INVALID_INDEX];

    /*
     * Due to the use of invalidate next, that requires the priority to demand a specific FES ID,
     * we provide a FES ID.
     */
    attach_info.action_info[VALID_INDEX].priority = BCM_FIELD_ACTION_POSITION(0, 5);

    LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Stage: %s. Performing context attach on conxtex : %d \r\n",
                __func__, __LINE__, dnx_field_stage_text(unit, fg_info->field_stage), context_id);
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *
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
appl_dnx_field_qual_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

/* { */
    dnx_field_stage_e field_stage, context_stage;
    int field_qual_test_type_name;
    char *field_qual_test_mode_name;
    int field_qual_test_clean;
    dnx_field_context_mode_t context_mode;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM("type", field_qual_test_type_name);
    SH_SAND_GET_STR("mode", field_qual_test_mode_name);
    SH_SAND_GET_ENUM("stage", field_stage);
    SH_SAND_GET_BOOL("clean", field_qual_test_clean);
/* } */
    if (field_qual_test_type_name == 1)
    {

        dnx_field_group_info_t fg_info;
        /*
         * dnx_field_group_attach_info_t attach_info;
         */
        dnx_field_qual_t *dnx_qual_list = NULL;
        ctest_dnx_field_qual_qual_info_t dnx_qual_info;
        dnx_field_group_t fg_id = 0;
        dnx_field_context_t context_id;
        dnx_field_action_t *dnx_action_list = NULL;
        CONST char *dnx_qual_name;
        int i_qual, i_act;
        int dnx_field_qual_num;
        int dnx_action_nof;
        dnx_field_qual_class_e dnx_qual_class;
        uint32 qual_size;

        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            context_stage = DNX_FIELD_STAGE_IPMF1;
        }
        else
        {
            context_stage = field_stage;
        }
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, context_stage, &context_mode, &context_id));

        SHR_IF_ERR_EXIT(preselector_set(unit, field_stage));

        if (sal_strncasecmp
            (field_qual_test_mode_name, TEST_MODE_QUALIFIERS, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "Fetching DNX Qualifier List for \"%s\" %s%s%s",
                        dnx_field_stage_text(unit, field_stage), EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(dnx_field_map_qual_list(unit, field_stage, &dnx_qual_list, &dnx_field_qual_num));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Options %d %s. Field Group add returned 'success' flag\r\n",
                        __func__, __LINE__, field_qual_test_type_name, field_qual_test_mode_name);

            for (i_qual = 0; i_qual < dnx_field_qual_num; i_qual++)
            {
                dnx_qual_info.dnx_qual = dnx_qual_list[i_qual];
                dnx_qual_class = DNX_QUAL_CLASS(dnx_qual_info.dnx_qual);
                dnx_qual_name = dnx_field_dnx_qual_text(unit, dnx_qual_info.dnx_qual);
                if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_HEADER)
                {
                    dnx_qual_info.input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
                }
                else if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_META)
                {
                    dnx_qual_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
                }
                else if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_META2)
                {
                    dnx_qual_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
                }
                else if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
                {
                    dnx_qual_info.input_type = DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE;
                }
                else
                {
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "Unsupported class %s (%d) for qual %s (%x). Not performing check.\r\n",
                                dnx_field_qual_class_text(dnx_qual_class), dnx_qual_class,
                                dnx_qual_name, dnx_qual_info.dnx_qual);
                    continue;
                }

                if (sal_strncasecmp(dnx_qual_name, "CORE_ID", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
                {
                    continue;
                }
                dnx_field_map_dnx_qual_size(unit, field_stage, dnx_qual_info.dnx_qual, NULL, NULL, &qual_size);
                if (qual_size > DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE)
                {
                    /*
                     * Skip creation of the databases for the over-sized qualifiers (quals that are bigger then 160b')
                     * */
                    LOG_INFO_EX(BSL_LOG_MODULE, "SKIP Creating Qualifier: %d %s size %d\n%s", i_qual, dnx_qual_name,
                                qual_size, EMPTY);
                    continue;
                }
                LOG_INFO_EX(BSL_LOG_MODULE, "Creating Qualifier: %d \n %s%s%s", i_qual, EMPTY, EMPTY, EMPTY);
                LOG_INFO_EX(BSL_LOG_MODULE, "Qualifier name: %s \n %s%s%s", dnx_qual_name, EMPTY, EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(field_group_fill_and_create
                                (unit, 0, field_stage, &fg_id, &fg_info, dnx_qual_info.dnx_qual, TESTING_QUALIFIERS));

                SHR_IF_ERR_EXIT(field_context_fill_and_attach
                                (unit, &fg_info, &dnx_qual_info, fg_id, context_id, TESTING_QUALIFIERS));
                if (field_qual_test_clean)
                {
                    CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, fg_id, context_id));
                    CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, fg_id));
                }

            }
        }
        else if (sal_strncasecmp
                 (field_qual_test_mode_name, TEST_MODE_ACTIONS, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            LOG_INFO_EX(BSL_LOG_MODULE, "Fetching DNX Actions List for \"%s\" %s%s%s",
                        dnx_field_stage_text(unit, field_stage), EMPTY, EMPTY, EMPTY);

            SHR_IF_ERR_EXIT(dnx_field_map_action_list(unit, field_stage, &dnx_action_list, &dnx_action_nof));
            LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Options %d %s. Field Group add returned 'success' flag\r\n",
                        __func__, __LINE__, field_qual_test_type_name, field_qual_test_mode_name);

            for (i_act = 0; i_act < dnx_action_nof - 1; i_act++)
            {
                LOG_INFO_EX(BSL_LOG_MODULE, "Creating Action: %d \n %s%s%s", i_act, EMPTY, EMPTY, EMPTY);
                LOG_INFO_EX(BSL_LOG_MODULE, "Action name: %s\n %s%s%s",
                            dnx_field_dnx_action_text(unit, dnx_action_list[i_act]), EMPTY, EMPTY, EMPTY);

                SHR_IF_ERR_EXIT(field_group_fill_and_create
                                (unit, dnx_action_list[i_act], field_stage, &fg_id, &fg_info, quals_array[field_stage],
                                 TESTING_ACTIONS));

                SHR_IF_ERR_EXIT(field_context_fill_and_attach
                                (unit, &fg_info, NULL, fg_id, context_id, TESTING_ACTIONS));
                if (field_qual_test_clean)
                {
                    CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, fg_id, context_id));
                    CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, fg_id));
                }
            }
        }
        sal_free(dnx_qual_list);
        sal_free(dnx_action_list);
    }
    else if (field_qual_test_type_name == 0)
    {
        LOG_CLI((BSL_META
                 ("%s(), line %d, %s level has not been implemented yet for %s. \r\n"),
                 __func__, __LINE__, "BCM", dnx_field_stage_text(unit, field_stage)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 */
shr_error_e
sh_dnx_field_qual_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_field_qual_starter(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }
exit:
    SHR_FUNC_EXIT;
}

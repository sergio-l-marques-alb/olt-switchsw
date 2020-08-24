/** \file diag_dnx_field_action.c
 * $Id$
 *
 * 'Action' operations (for payload and FES/FEM configuration and allocation) procedures for DNX.
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
#include <bcm/field.h>

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_field_action.h"
#include "ctest_dnx_field_utils.h"

#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_actions.h>
#include <bcm_int/dnx/field/field_init.h>
#include <bcm_int/dnx/field/field_map.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <include/sal/core/libc.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * The various values available for 'general_test_type' input.
 */
#define TEST_TYPE_DATABASE         "DATABASE"
#define TEST_TYPE_FES_CFG          "FES_CFG"
#define TEST_TYPE_FES_CFG_MUL      "FES_CFG_MUL"
#define TEST_TYPE_FES_CFG_INVALIDATE "FES_CFG_INVALIDATE"
#define TEST_TYPE_FEM              "FEM"

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
 *   Keyword for test type on action command 
 */
#define DNX_DIAG_ACTION_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for the stage for which FESes are to be tested
 */
#define DNX_DIAG_ACTION_OPTION_TEST_STAGE        "stage"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "count"
/**
 * \brief
 *   Keyword to specify the type of FEM test to carry out.
 *   See Field_fem_test_type_enum_table[]
 */
#define DNX_DIAG_ACTION_OPTION_FEM_TEST_TYPE     "fem_type"
/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define DNX_DIAG_ACTION_OPTION_TEST_CLEAN        "clean"
/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */
/**
 * Enum for type of FEM test within 'action' tests
 * See 'ctest_dnx_field_action.c'
 */
static sh_sand_enum_t Field_fem_test_type_enum_table[] = {
    {"FEM_ACTION_INFO",      CTEST_DNX_FIELD_FEM_TEST_ACTION_INFO_TYPE},
    {"FEM_MAP_INDEX",        CTEST_DNX_FIELD_FEM_TEST_MAP_INDEX_TYPE},
    {"FEM_CONDITION_MS_BIT", CTEST_DNX_FIELD_FEM_TEST_BIT_SELECT_TYPE},
    {"ACTIONS_FEM",          CTEST_DNX_FIELD_FEM_TEST_ACTIONS_FEM_TYPE},
    {"FEM_CONTEXT",          CTEST_DNX_FIELD_FEM_TEST_FEM_CONTEXT_TYPE},
    {"FEM_ADD",              CTEST_DNX_FIELD_FEM_TEST_FEM_ADD_TYPE},
    {"FEM_ATTACH",           CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE},
    {"FEM_CASCADE",          CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE},
    {"ENCODED_POSITION",     CTEST_DNX_FIELD_FEM_TEST_ENCODED_POSITION_TYPE},
    {NULL}
};

/**
 * \brief
 *   List of tests for 'action' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_action_tests[] = {
    {"DNX_action_for_DB_create_1", "type=database stage=ipmf1 count=6",              CTEST_POSTCOMMIT},
    {"DNX_action_for_DB_create_2", "type=database stage=ipmf2 count=6",              CTEST_POSTCOMMIT},
    {"DNX_action_for_DB_create_3", "type=database stage=ipmf3 count=6",              CTEST_POSTCOMMIT},
    {"DNX_action_for_DB_create_e", "type=database stage=epmf count=6",               CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_1",   "type=fes_cfg stage=ipmf1 count=2",               CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_2",   "type=fes_cfg stage=ipmf2 count=2",               CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_3",   "type=fes_cfg stage=ipmf3 count=2",               CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_e",   "type=fes_cfg stage=epmf count=2",                CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_mul", "type=fes_cfg_mul count=2",                       CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_invalidate_1","type=fes_cfg_invalidate stage=ipmf1 count=2",       CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_invalidate_2","type=fes_cfg_invalidate stage=ipmf2 count=2",       CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_invalidate_3","type=fes_cfg_invalidate stage=ipmf3 count=2",       CTEST_POSTCOMMIT},
    {"DNX_action_for_FES_cfg_invalidate_e","type=fes_cfg_invalidate stage=epmf count=2",        CTEST_POSTCOMMIT},
    {"DNX_action_fem_actions",     "type=fem fem_type=actions_fem count=2",          CTEST_POSTCOMMIT},
    {"DNX_action_fem_act_info",    "type=fem fem_type=fem_action_info count=2",      CTEST_POSTCOMMIT},
    {"DNX_action_fem_condition",   "type=fem fem_type=fem_condition_ms_bit count=2", CTEST_POSTCOMMIT},
    {"DNX_action_fem_add",         "type=fem fem_type=fem_add count=2",              CTEST_POSTCOMMIT},
    {"DNX_action_fem_attach",      "type=fem fem_type=fem_attach count=2",           CTEST_POSTCOMMIT},
    {"DNX_action_fem_cascade",     "type=fem fem_type=fem_cascade count=2",          CTEST_POSTCOMMIT},
    {"DNX_action_encoded_position","type=fem fem_type=encoded_position",             CTEST_POSTCOMMIT},
    {NULL}
};
/**
 * \brief
 *   Options list for 'action' shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t Sh_dnx_field_action_options[] = {
     /* Name */                              /* Type */              /* Description */                                         /* Default */         /* 'ENUM' table */
    {DNX_DIAG_ACTION_OPTION_TEST_TYPE,       SAL_FIELD_TYPE_STR,     "Type of test (database, fes_cfg, fem)",                     NULL},
    {DNX_DIAG_ACTION_OPTION_TEST_STAGE,      SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",                    "ipmf3",           (void *)Field_stage_enum_table},
    {DNX_DIAG_ACTION_OPTION_FEM_TEST_TYPE,   SAL_FIELD_TYPE_ENUM,    "Type of FEM test (See ctest_dnx_field_fem_action_info_e)",  "FEM_ACTION_INFO", (void *)Field_fem_test_type_enum_table},
    {DNX_DIAG_ACTION_OPTION_TEST_CLEAN,      SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",         "Yes"   },
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,      SAL_FIELD_TYPE_UINT32,  "Number of times test will run",  "1"},
    /* End of options list - must be last. */
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */

/**
 *  Descriptions of testing options.
 */
sh_sand_man_t Sh_dnx_field_action_man = {
    .brief = "'Action' related test utilities",
    .full = "Activate 'action' related test utilities as specified by 'type'\r\n"
        "If type is 'database' then test dnx_field_actions_place_actions() and dnx_field_actions_calc_feses().\r\n"
        "If type is 'fes_cfg' then test\r\n"
        "    dnx_field_actions_fes_set(),\r\n"
        "    dnx_field_actions_context_fes_info_get(),\r\n"
        "    dnx_field_actions_context_fes_info_to_group_fes_info(),\r\n"
        "    dnx_field_actions_fes_detach().\r\n"
        "If type is 'fem' then\r\n"
        "    if 'fem_type' is 'fem_action_info' then test\r\n"
        "        dnx_field_actions_fem_action_info_hw_set()\r\n"
        "        dnx_field_actions_fem_action_info_hw_get()\r\n"
        "    if 'fem_type' is 'fem_map_index' then test\r\n"
        "        dnx_field_actions_fem_map_index_hw_set(),\r\n"
        "        dnx_field_actions_fem_map_index_hw_get()\r\n"
        "    if 'fem_type' is 'fem_condition_ms_bit' then test\r\n"
        "        dnx_field_actions_fem_condition_ms_bit_hw_set()\r\n"
        "        dnx_field_actions_fem_condition_ms_bit_hw_get()\r\n"
        "    if 'fem_type' is 'actions_fem' then test\r\n"
        "        dnx_field_actions_fem_set()\r\n"
        "        dnx_field_actions_fem_get()\r\n"
        "    if 'fem_type' is 'fem_context' then test\r\n"
        "        dnx_field_actions_pmf_fem_context_hw_set()\r\n"
        "        dnx_field_actions_pmf_fem_context_hw_get()\r\n"
        "    if 'fem_type' is 'fem_add' then test\r\n"
        "        dnx_field_fem_action_add()\r\n"
        "        dnx_field_fem_action_remove()\r\n"
        "    if 'fem_type' is 'fem_attach' then test\r\n"
        "        dnx_field_group_fems_context_attach()\r\n"
        "        dnx_field_group_fems_context_detach()\r\n"
        "        dnx_field_actions_dir_ext_fem_key_select_get()\r\n"
        "        dnx_field_actions_fem_key_select_get()\r\n"
        "    if 'fem_type' is 'fem_cascade' then use cascaded contexts to test\r\n"
        "        dnx_field_group_fems_context_attach()\r\n"
        "        dnx_field_group_fems_context_detach()\r\n"
        "        dnx_field_actions_dir_ext_fem_key_select_get()\r\n"
        "        dnx_field_actions_fem_key_select_get()\r\n"
        "    if 'fem_type' is 'encoded_position' then use encode/decode fem_id to test\r\n"
        "        DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET()\r\n"
        "        BCM_FIELD_ACTION_POSITION()\r\n"
        "    Currently no more 'fem_type's are allowed.\r\n"
        "    if clean is NOT 'yes' then test does not return allocated\r\n"
        "    resources. This is mostly for debug exercises.\r\n"
        "Note that 'fem_type' is only relevant when 'type' is 'fem'."
        "Note that 'clean' is only relevant when 'type' is 'fem'.",
    .synopsis =
        "ctest field action type=<database | fes_cfg | fes_cfg_mul | fem> stage=<ipmf1 | ipmf2 | ipmf3 | epmf> \r\n"
        "fem_type=<fem_action_info | fem_map_index | fem_condition_ms_bit | actions_fem | fem_context | fem_add | fem_attach>",
    .examples =
        "type=database\n" "TY=database stage=ipmf1\n" "TY=fem fem_type=fem_action_info\n" "TY=fem fem_type=actions_fem"
};

/** The number of field groups used in the FES configuration test.*/
#define CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG        3
/** The number of actions used per field group in the FES configuration test.*/
#define CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS   2
/** The number ofrom which the field group IDs start in the FES configuration test.*/
#define CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID    20

/**
 * \brief
 *    Performs operational testing for the code in the field actions module.
 *    Tests dnx_field_actions_place_actions().
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which the database is to be created and tested (IPMF1, IPMF2, IPMF3, EPMF).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
static shr_error_e
appl_dnx_action_tester_db_only(
    int unit,
    dnx_field_stage_e field_stage)
{
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP] = { {0} };
    dnx_field_action_attach_info_t actions_attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int num_bits_on_block;
    dbal_enum_value_field_field_io_e field_io;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(actions_attach_info, 0x0, sizeof(actions_attach_info));

    if ((field_stage == DNX_FIELD_STAGE_IPMF1) || (field_stage == DNX_FIELD_STAGE_IPMF2))
    {
        actions_info_arr[0].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT);
        actions_info_arr[0].dont_use_valid_bit = 0;

        actions_info_arr[1].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA);
        actions_info_arr[1].dont_use_valid_bit = 0;

        actions_info_arr[2].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF1_ACTION_DP);
        actions_info_arr[2].dont_use_valid_bit = 0;

        actions_info_arr[3].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS);
        actions_info_arr[3].dont_use_valid_bit = 0;

        actions_info_arr[4].dnx_action = DNX_FIELD_ACTION_INVALID;
        num_bits_on_block = 64;
        SHR_IF_ERR_EXIT(dnx_field_actions_place_actions(unit, field_stage, num_bits_on_block, actions_info_arr));
        num_bits_on_block = 64;
        field_io = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                        (unit, field_stage, field_io, actions_info_arr, 0, actions_attach_info, fes_inst_info));
        /*
         * Do it again with two actions without valid bits.
         */
        actions_info_arr[0].dont_use_valid_bit = 1;
        actions_info_arr[1].dont_use_valid_bit = 1;
        SHR_IF_ERR_EXIT(dnx_field_actions_place_actions(unit, field_stage, num_bits_on_block, actions_info_arr));
        num_bits_on_block = 64;
        field_io = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                        (unit, field_stage, field_io, actions_info_arr, 0, actions_attach_info, fes_inst_info));

    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF3)
    {
        actions_info_arr[0].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE);
        actions_info_arr[0].dont_use_valid_bit = 0;

        actions_info_arr[1].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET);
        actions_info_arr[1].dont_use_valid_bit = 0;

        actions_info_arr[2].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE);
        actions_info_arr[2].dont_use_valid_bit = 0;

        actions_info_arr[3].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE);
        actions_info_arr[3].dont_use_valid_bit = 0;

        actions_info_arr[4].dnx_action = DNX_FIELD_ACTION_INVALID;

        num_bits_on_block = 60;
        SHR_IF_ERR_EXIT(dnx_field_actions_place_actions(unit, field_stage, num_bits_on_block, actions_info_arr));
        num_bits_on_block = 60;
        field_io = DBAL_ENUM_FVAL_FIELD_IO_SEXEM;
        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                        (unit, field_stage, field_io, actions_info_arr, 0, actions_attach_info, fes_inst_info));
        /*
         * Do it again with two actions without valid bits.
         */
        actions_info_arr[0].dont_use_valid_bit = 1;
        actions_info_arr[1].dont_use_valid_bit = 1;
        SHR_IF_ERR_EXIT(dnx_field_actions_place_actions(unit, field_stage, num_bits_on_block, actions_info_arr));
        num_bits_on_block = 60;
        field_io = DBAL_ENUM_FVAL_FIELD_IO_SEXEM;
        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                        (unit, field_stage, field_io, actions_info_arr, 0, actions_attach_info, fes_inst_info));

    }
    else if (field_stage == DNX_FIELD_STAGE_EPMF)
    {
        actions_info_arr[0].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD);
        actions_info_arr[0].dont_use_valid_bit = 0;

        actions_info_arr[1].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE);
        actions_info_arr[1].dont_use_valid_bit = 0;

        actions_info_arr[2].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_EPMF_ACTION_TC);
        actions_info_arr[2].dont_use_valid_bit = 0;

        actions_info_arr[3].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_EPMF_ACTION_DP);
        actions_info_arr[3].dont_use_valid_bit = 0;

        actions_info_arr[4].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP);
        actions_info_arr[4].dont_use_valid_bit = 0;

        actions_info_arr[5].dnx_action = DNX_FIELD_ACTION_INVALID;
        num_bits_on_block = 64;
        SHR_IF_ERR_EXIT(dnx_field_actions_place_actions(unit, field_stage, num_bits_on_block, actions_info_arr));
        num_bits_on_block = 64;
        field_io = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                        (unit, field_stage, field_io, actions_info_arr, 0, actions_attach_info, fes_inst_info));
        /*
         * Do it again with two actions without valid bits.
         */
        actions_info_arr[0].dont_use_valid_bit = 1;
        actions_info_arr[1].dont_use_valid_bit = 1;
        SHR_IF_ERR_EXIT(dnx_field_actions_place_actions(unit, field_stage, num_bits_on_block, actions_info_arr));
        num_bits_on_block = 64;
        field_io = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                        (unit, field_stage, field_io, actions_info_arr, 0, actions_attach_info, fes_inst_info));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given 'actions' and 'qualifiers', load all intermediate parameters to carry
 *   out a required 'dnx_field_group_add()'. On the way, store info on input arrays for
 *   further usage.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] num_quals -
 *   Number of qualifiers for Field Group to be created (on DNX level).
 * \param [in] tcam_quals -
 *   Array of structures of type 'dnx_ctest_field_quals_for_fg_t'. Number of elements
 *   on array is 'num_quals'. Array is loaded by caller as described on
 *   'dnx_ctest_field_quals_for_fg_t'
 * \param [in] num_actions -
 *   Number of actions for Field Group to be created (on DNX level).
 * \param [in] tcam_actions -
 *   Array of structures of type 'dnx_ctest_field_actions_for_fg_t'. Number of elements
 *   on array is 'num_actions'. Array is loaded by caller as described on
 *   'dnx_ctest_field_actions_for_fg_t'
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] index_forward -
 *   Control flag. Boolean. If non-zero then index on qualifiers ('tcam_quals') and actions ('tcam_actions')
 *   runs from zero to the top (num_quals/num_actions).
 *   Otherwise, it runs from top ('num_quals-1'/'num_actions-1') to zero.
 * \param [in] name_suffix -
 *   A suffix added to the names of the field groups, actions and qualifiers to keep the names unique.
 * \param [in,out] fg_info_p -
 *   Pointer to structure of type 'dnx_field_group_info_t'. Information related to the Field Group to
 *   create. Some of the elements are input and some are output:
 *     Input:
 *       field_stage
 *       fg_type
 *     Output:
 *       dnx_quals[]
 *       dnx_actions[]
 * \param [out] fg_id_p -
 *   Pointer to structure of type 'dnx_field_group_t'. This procedure load pointed memory by
 *   the identifier of the newly created Field Group.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */

static shr_error_e
ctest_dnx_field_action_create_fg(
    int unit,
    unsigned int num_quals,
    dnx_ctest_field_quals_for_fg_t * tcam_quals,
    unsigned int num_actions,
    dnx_ctest_field_actions_for_fg_t * tcam_actions,
    sh_sand_control_t * sand_control,
    int index_forward,
    char *name_suffix,
    dnx_field_group_info_t * fg_info_p,
    dnx_field_group_t * fg_id_p)
{
    /*
     * Create owner TCAM field group
     * Create TCAM field group no. 0 (starting from '0')
     */
    dnx_field_group_add_flags_e group_add_flags;
    unsigned int qual_index, action_index;

    bcm_field_qualify_t bcm_qual;
    bcm_field_action_t bcm_action;

    SHR_FUNC_INIT_VARS(unit);
    group_add_flags = 0;
    /*
     * We assume that num_quals is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG !!!
     * We assume that num_actions is smaller than DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG !!!
     */
    if (index_forward != 0)
    {
        for (qual_index = 0; qual_index < num_quals; qual_index++)
        {
            dnx_field_qual_flags_e qual_flags;
            dnx_field_qualifier_in_info_t user_qual_info;

            SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_qual_info));

            sal_snprintf(user_qual_info.name, sizeof(user_qual_info.name), "%s%s", tcam_quals[qual_index].qual_name,
                         name_suffix);
            user_qual_info.size = tcam_quals[qual_index].qual_nof_bits;
            qual_flags = 0;
            SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, qual_flags, &user_qual_info, &bcm_qual));
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                            (unit, fg_info_p->field_stage, bcm_qual, &(tcam_quals[qual_index].dnx_qual)));
            fg_info_p->dnx_quals[qual_index] = tcam_quals[qual_index].dnx_qual;
        }
        /*
         * Note that 'fg_info_p->dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
         * Note that, in this test, we refer to the first '2msb' combination only. Currently,
         * the code only supports that.
         */
        for (action_index = 0; action_index < num_actions; action_index++)
        {
            unsigned int base_action_size;
            dnx_field_action_flags_e action_flags;
            dnx_field_action_in_info_t user_action_info;
            bcm_field_action_t bcm_mapped_action_type;
            dnx_field_action_t user_action;

            action_flags = 0;
            user_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                     fg_info_p->field_stage, tcam_actions[action_index].dbal_action_id);
            /*
             * Get the size of the base DNX action.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                            (unit, fg_info_p->field_stage, user_action, &base_action_size));
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit,
                                                            fg_info_p->field_stage,
                                                            user_action, &bcm_mapped_action_type));
            user_action_info.bcm_action = bcm_mapped_action_type;
            sal_snprintf(user_action_info.name, sizeof(user_action_info.name), "%s%s",
                         tcam_actions[action_index].action_name, name_suffix);
            user_action_info.prefix = 0;
            user_action_info.size = tcam_actions[action_index].action_nof_bits;
            user_action_info.prefix_size = base_action_size - tcam_actions[action_index].action_nof_bits;
            user_action_info.stage = fg_info_p->field_stage;
            SHR_IF_ERR_EXIT(dnx_field_action_create(unit,
                                                    action_flags,
                                                    &user_action_info,
                                                    &bcm_action, &(tcam_actions[action_index].dnx_action)));
            fg_info_p->dnx_actions[action_index] = tcam_actions[action_index].dnx_action;
            if (fg_info_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION
                || user_action_info.bcm_action == bcmFieldActionVoid)
            {
                fg_info_p->use_valid_bit[action_index] = FALSE;
            }
            else
            {
                fg_info_p->use_valid_bit[action_index] = TRUE;
            }
        }
    }
    else
    {
        for (qual_index = (num_quals - 1); (int) qual_index >= 0; qual_index--)
        {
            dnx_field_qual_flags_e qual_flags;
            dnx_field_qualifier_in_info_t user_qual_info;

            SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &user_qual_info));
            sal_snprintf(user_qual_info.name, sizeof(user_qual_info.name), "%s%s", tcam_quals[qual_index].qual_name,
                         name_suffix);
            user_qual_info.size = tcam_quals[qual_index].qual_nof_bits;
            qual_flags = 0;
            SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, qual_flags, &user_qual_info, &bcm_qual));
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                            (unit, fg_info_p->field_stage, bcm_qual, &(tcam_quals[qual_index].dnx_qual)));
            fg_info_p->dnx_quals[qual_index] = tcam_quals[qual_index].dnx_qual;
        }
        /*
         * Note that 'fg_info_p->dnx_actions' has been initiated in 'dnx_field_group_info_t_init' above.
         * Note that, in this test, we refer to the first '2msb' combination only. Currently,
         * the code only supports that.
         */
        for (action_index = (num_actions - 1); (int) action_index >= 0; action_index--)
        {
            unsigned int base_action_size;
            dnx_field_action_flags_e action_flags;
            dnx_field_action_in_info_t user_action_info;
            bcm_field_action_t bcm_mapped_action_type;
            dnx_field_action_t user_action;

            action_flags = 0;
            user_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                     fg_info_p->field_stage, tcam_actions[action_index].dbal_action_id);
            /*
             * Get the size of the base DNX action.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                            (unit, fg_info_p->field_stage, user_action, &base_action_size));
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit,
                                                            fg_info_p->field_stage,
                                                            user_action, &bcm_mapped_action_type));
            user_action_info.bcm_action = bcm_mapped_action_type;
            sal_snprintf(user_action_info.name, sizeof(user_action_info.name), "%s%s",
                         tcam_actions[action_index].action_name, name_suffix);
            user_action_info.prefix = 0;
            user_action_info.size = tcam_actions[action_index].action_nof_bits;
            user_action_info.prefix_size = base_action_size - tcam_actions[action_index].action_nof_bits;
            user_action_info.stage = fg_info_p->field_stage;
            SHR_IF_ERR_EXIT(dnx_field_action_create(unit,
                                                    action_flags,
                                                    &user_action_info,
                                                    &bcm_action, &(tcam_actions[action_index].dnx_action)));
            fg_info_p->dnx_actions[action_index] = tcam_actions[action_index].dnx_action;
            if (fg_info_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION
                || user_action_info.bcm_action == bcmFieldActionVoid)
            {
                fg_info_p->use_valid_bit[action_index] = FALSE;
            }
            else
            {
                fg_info_p->use_valid_bit[action_index] = TRUE;
            }
        }

    }
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, group_add_flags, fg_info_p, fg_id_p));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *   Invoke DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET as a procedure.
 *   See 'remark' below.
 * \param [out] fem_id  -
 *   The FEM ID as extracted from _action_priority.
 * \param [in] unit     -
 *   Device ID.
 * \param [in] encoded_position -
 *   Pointer to variable of type bcm_field_action_priority_t.
 *   This procedure loads pointed memory by encoded position of a FEM.
 *   Encoding is done using 'array_id' and 'position'.
 *   See 'BCM_FIELD_ACTION_POSITION' and 'remark' below
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   This procedure is only for use on negative tests.
 * \see
 *   * DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET
 */
static shr_error_e
dnx_field_action_priority_position_fem_id_get(
    dnx_field_fem_id_t * fem_id_p,
    int unit,
    bcm_field_action_priority_t encoded_position)
{
    dnx_field_fem_id_t fem_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(fem_id, unit, encoded_position);
    *fem_id_p = fem_id;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs FEM operational testing for the code in the field actions module.
 *    Tests:
 *    * dnx_field_actions_fem_action_info_hw_get()/dnx_field_actions_fem_action_info_hw_set()
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fem_action_info -
 *    Identifier of the test to carry out. See 'ctest_dnx_field_fem_action_info_e'.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
static shr_error_e
appl_dnx_action_tester_fem(
    int unit,
    ctest_dnx_field_fem_test_type_e fem_test_type,
    int clean_resources,
    sh_sand_control_t * sand_control)
{
    dnx_field_fem_action_info_t fem_action_info;
    int rv;
    /**
     * This variables stores the severity of the Field processor dnx
     */
    bsl_severity_t original_severity_fldprocdnx;
    /**
     * This variables stores the severity of the Field processor test dnx
     */
    bsl_severity_t original_severity_fldtestdnx;
    /**
     * This variables stores the severity of the Field processor dnx
     */
    bsl_severity_t original_severity_swstate;
    dnx_field_fem_condition_entry_t *fem_condition_entry_p = NULL;
    dnx_field_fem_action_entry_t *fem_action_entry_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    /**
     * Since this procedure includes negative tests, error logs are expected.
     * To avoid seeing these messages, we store the original severity here and, in case of negative tests,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDTESTSDNX, original_severity_fldtestdnx);
    /*
     * Select the test.
     */
    switch (fem_test_type)
    {
        case CTEST_DNX_FIELD_FEM_TEST_ACTION_INFO_TYPE:
        {
            dnx_field_fem_bit_index_t fem_bit_index;

            /*
             * Load third action on 'FEM id' - 0 as follows:
             * All 'fem bit's are programmed to take values from 'key select' and
             * the bit index goes up from 0. Only 4 bits are available.
             */
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_actions_fem_action_info_hw_get()",
                        "dnx_field_actions_fem_action_info_hw_set()");
            fem_action_info.fem_id = 0;
            fem_action_info.fem_map_index = 2;
            fem_action_info.fem_action_entry.fem_action = DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE;
            for (fem_bit_index = 0; fem_bit_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION;
                 fem_bit_index++)
            {
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_format =
                    DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                    (unsigned int) fem_bit_index;
            }
            if (fem_action_info.fem_id <
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
            {
                fem_action_info.fem_action_entry.fem_adder = 0;
            }
            else
            {
                fem_action_info.fem_action_entry.fem_adder = (dnx_field_fem_adder_t) fem_bit_index;
            }
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_set(unit, &fem_action_info));
            /*
             * Load fourth action on 'FEM id' - 0 as follows:
             * All 'fem bit's are programmed to take values from 'key select' and
             * the bit index goes does from 23.  Only 4 bits are available.
             */
            fem_action_info.fem_id = 0;
            fem_action_info.fem_map_index = 3;
            fem_action_info.fem_action_entry.fem_action = DBAL_ENUM_FVAL_IPMF1_ACTION_EEI;
            for (fem_bit_index = 0; fem_bit_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION;
                 fem_bit_index++)
            {
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_format =
                    DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION - 1 - (unsigned int) fem_bit_index;
            }
            if (fem_action_info.fem_id <
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
            {
                fem_action_info.fem_action_entry.fem_adder = 0;
            }
            else
            {
                fem_action_info.fem_action_entry.fem_adder = (dnx_field_fem_adder_t) fem_bit_index;
            }
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_set(unit, &fem_action_info));
            /*
             * Load third action on 'FEM id' - 6 as follows:
             * All 'fem bit's are programmed to take values from 'key select' and
             * the bit index goes up from 0. 24 bits are available.
             */
            fem_action_info.fem_id = 6;
            fem_action_info.fem_map_index = 2;
            fem_action_info.fem_action_entry.fem_action = DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE;
            for (fem_bit_index = 0; fem_bit_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION;
                 fem_bit_index++)
            {
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_format =
                    DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                    (unsigned int) fem_bit_index;
            }
            if (fem_action_info.fem_id <
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
            {
                fem_action_info.fem_action_entry.fem_adder = 0;
            }
            else
            {
                fem_action_info.fem_action_entry.fem_adder = (dnx_field_fem_adder_t) fem_bit_index;
            }
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_set(unit, &fem_action_info));
            /*
             * Load fourth action on 'FEM id' - 0 as follows:
             * All 'fem bit's are programmed to take values from 'key select' and
             * the bit index goes does from 23.  24 bits are available.
             */
            fem_action_info.fem_id = 6;
            fem_action_info.fem_map_index = 3;
            fem_action_info.fem_action_entry.fem_action = DBAL_ENUM_FVAL_IPMF1_ACTION_EEI;
            for (fem_bit_index = 0; fem_bit_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION;
                 fem_bit_index++)
            {
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_format =
                    DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                fem_action_info.fem_action_entry.fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION - 1 - (unsigned int) fem_bit_index;
            }
            if (fem_action_info.fem_id <
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
            {
                fem_action_info.fem_action_entry.fem_adder = 0;
            }
            else
            {
                fem_action_info.fem_action_entry.fem_adder = (dnx_field_fem_adder_t) fem_bit_index;
            }
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_action_info_hw_set(unit, &fem_action_info));

            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_MAP_INDEX_TYPE:
        {
            dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry;
            dnx_field_fem_map_index_entry_t dnx_field_fem_map_index_entry_get;
            dnx_field_fem_map_index_t fem_map_index_max;
            dnx_field_fem_map_data_t fem_map_data_max;
            dbal_enum_value_field_field_fem_action_valid_e fem_action_valid_max;
            dnx_field_fem_condition_t fem_condition_index;
            dnx_field_fem_condition_t fem_condition_max;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_actions_fem_map_index_hw_get()",
                        "dnx_field_actions_fem_map_index_hw_set()");
            /*
             * Maximum number of FEM 'map indices' in iPMF1/2 == 4
             * This is the number of actions available per FEM
             */
            fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
            /*
             * Maximum value of FEM 'map data' in iPMF1/2 plus one == 16
             * This is the number of 'map data' available in the table
             */
            /*
             * Maximum value of FEM 'action valid' flag in iPMF1/2 plus one == 2
             * This is the number of 'action valid' values available
             */
            fem_action_valid_max = DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES;
            /*
             * Maximum number of FEM 'condition's in iPMF1/2 == 4
             * This is the number of conditions available per 'FEM id','FEM program' pair.
             */
            fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
            fem_map_data_max =
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
            sal_memset(&dnx_field_fem_map_index_entry, 0, sizeof(dnx_field_fem_map_index_entry));
            dnx_field_fem_map_index_entry.fem_id = 0;
            dnx_field_fem_map_index_entry.fem_program = 1;
            sal_memset(&dnx_field_fem_map_index_entry_get, 0, sizeof(dnx_field_fem_map_index_entry_get));
            dnx_field_fem_map_index_entry_get.fem_id = dnx_field_fem_map_index_entry.fem_id;
            dnx_field_fem_map_index_entry_get.fem_program = dnx_field_fem_map_index_entry.fem_program;
            for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
            {
                dnx_field_fem_map_index_entry.fem_condition = fem_condition_index;
                /*
                 * Change fem_map_index cyclically over all conditions.
                 */
                dnx_field_fem_map_index_entry.fem_condition_entry.fem_map_index =
                    fem_condition_index % fem_map_index_max;
                /*
                 * Change fem_map_index cyclically over all conditions. (Actually make it
                 * equal to 'condition').
                 */
                dnx_field_fem_map_index_entry.fem_condition_entry.fem_map_data = fem_condition_index % fem_map_data_max;
                /*
                 * Change fem_action_valid cyclically over all conditions. (Actually make it
                 * change from 0 to 1 and back as 'condition' progresses.)
                 */
                dnx_field_fem_map_index_entry.fem_condition_entry.fem_action_valid =
                    fem_condition_index % fem_action_valid_max;
                SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_set(unit, &dnx_field_fem_map_index_entry));
                /*
                 * Now get info from HW.
                 */
                dnx_field_fem_map_index_entry_get.fem_condition = dnx_field_fem_map_index_entry.fem_condition;
                SHR_IF_ERR_EXIT(dnx_field_actions_fem_map_index_hw_get(unit, &dnx_field_fem_map_index_entry_get));
                if (sal_memcmp
                    (&dnx_field_fem_map_index_entry, &dnx_field_fem_map_index_entry_get,
                     sizeof(dnx_field_fem_map_index_entry)) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Compare failed on dnx_field_actions_fem_map_index_hw_get()/dnx_field_actions_fem_map_index_hw_set().\r\n");
                }
                else
                {
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, Compare succeeded on %s/%s .\r\n",
                                __FUNCTION__, __LINE__, "dnx_field_actions_fem_map_index_hw_get()",
                                "dnx_field_actions_fem_map_index_hw_set()");
                }
            }
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_BIT_SELECT_TYPE:
        {
            dnx_field_fem_condition_ms_bit_entry_t dnx_field_fem_condition_ms_bit_entry;
            dnx_field_fem_condition_ms_bit_entry_t dnx_field_fem_condition_ms_bit_entry_get;
            dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit_index, fem_condition_ms_bit_max,
                fem_condition_ms_bit_min;
            dnx_field_fem_program_t fem_program_index, fem_program_max;
            dnx_field_fem_id_t fem_id_index, fem_id_max;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_actions_fem_condition_ms_bit_hw_get()",
                        "dnx_field_actions_fem_condition_ms_bit_hw_set()");
            /*
             * Maximum value of 'FEM bit select' in iPMF1/2 plus one == 32
             * This is the number of 'fem bit select's available, say, in the FIELD_PMF_A_FEM_CONDITION_MS_BIT table
             */
            fem_condition_ms_bit_max =
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_condition_ms_bit_max_value + 1;
            /*
             * Minimal value of 'FEM bit select' in iPMF1/2 == 3
             * This is the lowest value of 'fem bit select' (It is the MS bit of 4
             * bits to be used as 'condition' so the lowest value is '3').
             * See FIELD_PMF_A_FEM_CONDITION_MS_BIT table
             */
            fem_condition_ms_bit_min =
                dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->fem_condition_ms_bit_min_value;
            /*
             * Maximum value of 'FEM program' in iPMF1/2 plus one == 4
             * This is the number of 'fem programs's available, say, in the FIELD_PMF_A_FEM_CONDITION_MS_BIT table
             */
            fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
            /*
             * Maximum value of 'FEM id' in iPMF1/2 plus one == 16
             * This is the number of 'fem id's available in the system
             */
            fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
            sal_memset(&dnx_field_fem_condition_ms_bit_entry, 0, sizeof(dnx_field_fem_condition_ms_bit_entry));
            sal_memset(&dnx_field_fem_condition_ms_bit_entry_get, 0, sizeof(dnx_field_fem_condition_ms_bit_entry_get));
            fem_condition_ms_bit_index = fem_condition_ms_bit_min;
            for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
            {
                dnx_field_fem_condition_ms_bit_entry.fem_id = fem_id_index;
                for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
                {
                    dnx_field_fem_condition_ms_bit_entry.fem_program = fem_program_index;
                    if (fem_condition_ms_bit_index >= fem_condition_ms_bit_max)
                    {
                        fem_condition_ms_bit_index = fem_condition_ms_bit_min;
                    }
                    dnx_field_fem_condition_ms_bit_entry.fem_condition_ms_bit = fem_condition_ms_bit_index;
                    fem_condition_ms_bit_index++;
                    SHR_IF_ERR_EXIT(dnx_field_actions_fem_condition_ms_bit_hw_set
                                    (unit, &dnx_field_fem_condition_ms_bit_entry));
                    /*
                     * Now get info from HW.
                     */
                    dnx_field_fem_condition_ms_bit_entry_get.fem_id = dnx_field_fem_condition_ms_bit_entry.fem_id;
                    dnx_field_fem_condition_ms_bit_entry_get.fem_program =
                        dnx_field_fem_condition_ms_bit_entry.fem_program;
                    SHR_IF_ERR_EXIT(dnx_field_actions_fem_condition_ms_bit_hw_get
                                    (unit, &dnx_field_fem_condition_ms_bit_entry_get));
                    if (dnx_field_fem_condition_ms_bit_entry_get.fem_condition_ms_bit !=
                        dnx_field_fem_condition_ms_bit_entry.fem_condition_ms_bit)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "Compare failed on dnx_field_actions_fem_condition_ms_bit_hw_get()/dnx_field_actions_fem_condition_ms_bit_hw_set().\r\n");
                    }
                    else
                    {
                        LOG_INFO_EX(BSL_LOG_MODULE,
                                    "%s(), line %d, Compare succeeded on %s/%s .\r\n",
                                    __FUNCTION__, __LINE__, "dnx_field_actions_fem_condition_ms_bit_hw_get()",
                                    "dnx_field_actions_fem_condition_ms_bit_hw_set()");
                    }
                }
            }
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_FEM_CONTEXT_TYPE:
        {
            dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_context_entry;
            dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_context_entry_get;
            dnx_field_context_t fem_context_index, fem_context_max;
            dnx_field_fem_program_t fem_program_index, fem_program_max;
            dnx_field_fem_id_t fem_id_index;
            dbal_enum_value_field_field_fem_replace_lsb_select_e replace_lsb_select_index, replace_lsb_select_max;
            dbal_enum_value_field_field_fem_replace_lsb_flag_e replace_lsb_flag_index, replace_lsb_flag_max;
            dbal_enum_value_field_field_pmf_a_fem_key_select_e fem_key_select_index, fem_key_select_max;
            uint32 uint_container;
            int is_key;

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_actions_pmf_fem_context_hw_get()",
                        "dnx_field_actions_pmf_fem_context_hw_set()");
            /*
             * Maximum value of 'fem key select' in iPMF1/2 plus one == 73
             * This is the number of 'fem key select's available in the FIELD_PMF_A_KEY_SELECT_N_FEM_PROG table
             */
            fem_key_select_max = DBAL_NOF_ENUM_FIELD_PMF_A_FEM_KEY_SELECT_VALUES;
            /*
             * Maximum value of 'replace lsb select' in iPMF1/2 plus one == 8
             * This is the number of 'replace lsb select's available in the FIELD_PMF_A_KEY_SELECT_N_FEM_PROG table
             */
            replace_lsb_select_max = DBAL_NOF_ENUM_FIELD_FEM_REPLACE_LSB_SELECT_VALUES;
            /*
             * Maximum value of 'replace lsb flag' in iPMF1/2 plus one == 2
             * This is the number of 'replace lsb flag' values available in the FIELD_PMF_A_KEY_SELECT_N_FEM_PROG table
             */
            replace_lsb_flag_max = DBAL_NOF_ENUM_FIELD_FEM_REPLACE_LSB_FLAG_VALUES;
            /*
             * Maximum value of 'contexts' in iPMF1/2 plus one == 64
             * This is the number of entries in the FIELD_PMF_A_KEY_SELECT_N_FEM_PROG table
             */
            /*
             * This field (FIELD_PMF_CTX_ID) is the 'context', which is part of the key.
             */
            is_key = TRUE;
            SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                            (unit, DBAL_TABLE_FIELD_PMF_A_KEY_SELECT_N_FEM_PROG, DBAL_FIELD_FIELD_PMF_CTX_ID,
                             is_key, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &uint_container));
            fem_context_max = (dnx_field_context_t) (uint_container + 1);
            /*
             * Maximum value of 'FEM program' in iPMF1/2 plus one == 4
             * This is the number of 'fem programs's available, say, in the FIELD_PMF_A_FEM_CONDITION_MS_BIT table
             */
            fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
            fem_context_index = fem_context_max - 1;
            {
                sal_memset(&dnx_field_pmf_fem_context_entry, 0, sizeof(dnx_field_pmf_fem_context_entry));
                dnx_field_pmf_fem_context_entry.context_id = fem_context_index;
                fem_id_index = 0;
                {
                    dnx_field_pmf_fem_context_entry.fem_id = fem_id_index;
                    for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
                    {
                        dnx_field_pmf_fem_context_entry.pmf_fem_program.fem_program = fem_program_index;
                        for (fem_key_select_index = 0; fem_key_select_index < fem_key_select_max;
                             fem_key_select_index++)
                        {
                            dnx_field_pmf_fem_context_entry.pmf_fem_program.fem_key_select = fem_key_select_index;
                            for (replace_lsb_flag_index = 0; replace_lsb_flag_index < replace_lsb_flag_max;
                                 replace_lsb_flag_index++)
                            {
                                dnx_field_pmf_fem_context_entry.pmf_fem_program.replace_lsb_flag =
                                    replace_lsb_flag_index;
                                for (replace_lsb_select_index = 0; replace_lsb_select_index < replace_lsb_select_max;
                                     replace_lsb_select_index++)
                                {
                                    dnx_field_pmf_fem_context_entry.pmf_fem_program.replace_lsb_select =
                                        replace_lsb_select_index;
                                    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_set
                                                    (unit, &dnx_field_pmf_fem_context_entry));
                                    /*
                                     * Now get info from HW.
                                     */
                                    sal_memset(&dnx_field_pmf_fem_context_entry_get, 0,
                                               sizeof(dnx_field_pmf_fem_context_entry_get));
                                    dnx_field_pmf_fem_context_entry_get.fem_id = dnx_field_pmf_fem_context_entry.fem_id;
                                    dnx_field_pmf_fem_context_entry_get.context_id =
                                        dnx_field_pmf_fem_context_entry.context_id;
                                    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get
                                                    (unit, &dnx_field_pmf_fem_context_entry_get));
                                    if (sal_memcmp
                                        (&(dnx_field_pmf_fem_context_entry_get.pmf_fem_program),
                                         &(dnx_field_pmf_fem_context_entry.pmf_fem_program),
                                         sizeof(dnx_field_pmf_fem_context_entry_get.pmf_fem_program)))
                                    {
                                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                                     "Compare failed on dnx_field_actions_pmf_fem_context_hw_get()/dnx_field_actions_pmf_fem_context_hw_set().\r\n");
                                    }
                                    else
                                    {
                                        if ((replace_lsb_select_index == (replace_lsb_select_max - 1)) &&
                                            (replace_lsb_flag_index == (replace_lsb_flag_max - 1))
                                            && (fem_key_select_index == (fem_key_select_max - 1)))
                                        {
                                            LOG_INFO_EX(BSL_LOG_MODULE,
                                                        "%s(), line %d, Compare succeeded on %s/%s .\r\n",
                                                        __FUNCTION__, __LINE__,
                                                        "dnx_field_actions_pmf_fem_context_hw_get()",
                                                        "dnx_field_actions_pmf_fem_context_hw_set()");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_ACTIONS_FEM_TYPE:
        {
            uint8 ignore_actions;
            dnx_field_fem_id_t fem_id;
            dnx_field_fem_program_t fem_program;
            dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit;
            dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
            dnx_field_fem_bit_index_t fem_bit_index_max, fem_bit_index;
            dnx_field_fem_condition_t fem_condition_max, fem_condition_index;

            SHR_ALLOC(fem_condition_entry_p, sizeof(*fem_condition_entry_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                                                  DNX_FIELD_STAGE_IPMF1)->nof_fem_condition,
                      "fem_condition_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC(fem_action_entry_p,
                      sizeof(*fem_action_entry_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                        DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index,
                      "fem_action_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
            fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
            fem_bit_index_max =
                (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry_p[0].fem_bit_info) /
                                             sizeof(fem_action_entry_p[0].fem_bit_info[0]));
            fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;

            fem_id = 6;
            fem_program = 1;
            /*
             * Select bits 16,15,14,12 (from input 32 bits) as 'condition'
             */
            fem_condition_ms_bit = 16;
            for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
            {
                fem_condition_entry_p[fem_condition_index].fem_map_index = fem_condition_index % fem_map_index_max;
                /*
                 * Make map_data equal to 'condition'.
                 */
                fem_condition_entry_p[fem_condition_index].fem_map_data = fem_condition_index;
                /*
                 * Change fem_action_valid cyclically over all conditions. (Actually make it
                 * change from 0 to 1 and back as 'condition' progresses.)
                 */
                fem_condition_entry_p[fem_condition_index].fem_action_valid =
                    fem_condition_index % DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES;
            }
            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                fem_action_entry_p[fem_map_index].fem_action = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index;
                for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                {
                    fem_action_entry_p[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                        DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                    fem_action_entry_p[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                        6 + fem_bit_index;
                }
                if (fem_id <
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                {
                    fem_action_entry_p[fem_map_index].fem_adder = 0;
                }
                else
                {
                    fem_action_entry_p[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (fem_map_index + 16);
                }
            }
            ignore_actions = 0;
            SHR_IF_ERR_EXIT(dnx_field_actions_fem_set
                            (unit, fem_id, fem_program, fem_condition_ms_bit, fem_condition_entry_p,
                             ignore_actions, fem_action_entry_p));
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_FEM_ADD_TYPE:
        {
            dnx_field_group_t
                fg_id[CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_ADD],
                second_fg_id[CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_ADD];
            dnx_field_group_info_t de_fg_info, tcam_fg_info;
            /**
             * Number of qualifiers to use for testing interface
             * of direct extraction on DNX level.
             */
            unsigned int num_quals_for_de;
            /**
             * Number of actions to use for testing interface
             * of direct extraction on DNX level.
             */
            unsigned int num_actions_for_de;
            /**
             * Number of qualifiers to use for testing TCAM interface
             * of direct extraction on DNX level.
             */
            unsigned int num_quals_for_tcam;
            /**
             * Number of actions to use for testing TCAM interface
             * of direct extraction on DNX level.
             */
            unsigned int num_actions_for_tcam;
            dnx_field_fem_id_t fem_id;
            uint8 input_offset, input_size;
            dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit;
            dnx_field_fem_condition_entry_t
                fem_condition_entry[CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS]
                [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
            dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
            dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
            dnx_field_fem_condition_t fem_condition_setup, fem_condition_index, fem_condition_max;
            dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
            dnx_field_fem_map_data_t fem_map_data_max;
            dnx_field_fem_action_valid_t fem_action_valid_max;
            dnx_field_fem_bit_index_t fem_bit_index_max, fem_bit_index;

/* *INDENT-OFF* */
            /*
             * Input parameters for the 'new' direct extraction style. To be used on the 'FEM' test below.
             * First table is qualifiers' parameters and second table contains the matching actions.
             * Note that the total size for both needs to be the same!
             */
            dnx_ctest_field_quals_for_fg_t dir_ext_quals_for_de[] =
            {
                 /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
                {"user_qual_0",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                  5       }},

                {"user_qual_1",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
                {"user_qual_2",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }},

                {"user_qual_3",    3,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 27       }},
                {"user_qual_4",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 30       }},
                {"user_qual_5",    4,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }}
            };
            dnx_ctest_field_actions_for_fg_t dir_ext_actions_for_de[] =
            {
                 /* name */      /* size */      /* DBAL action id */                    /* Priority */
                {"user_action_0",    5,    DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {       3         }},
                {"user_action_1",   15,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       4         }},
                {"user_action_2",   12,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       5         }}
            };
            /*
             * Input parameters for the 'second Field Group' which must be 'TCAM' type. To be used on the 'FEM' test below.
             * First table is qualifiers' parameters and second table contains the matching actions.
             * For now, the setup is NOT MEANINGFUL. Actions should be marked to be located on the low 16 bits.
             */
            dnx_ctest_field_quals_for_fg_t tcam_quals_for_de[] =
            {
                 /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
                {"user_qual_10",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                  5       }},
                {"user_qual_11",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
                {"user_qual_12",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }}
            };
            dnx_ctest_field_actions_for_fg_t tcam_actions_for_de[] =
            {
                 /* name */      /* size */      /* DBAL action id */                    /* Priority */
                {"user_action_10",    5,    DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {       3         }},
                {"user_action_11",   15,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       4         }},
                {"user_action_12",   12,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       5         }},
                {"user_action_13",   32,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {      12         }}
            };
/* *INDENT-ON* */

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_fem_action_add()", "dnx_field_fem_action_remove()");
            {
                num_quals_for_de = sizeof(dir_ext_quals_for_de) / sizeof(dir_ext_quals_for_de[0]);
                num_actions_for_de = sizeof(dir_ext_actions_for_de) / sizeof(dir_ext_actions_for_de[0]);

                /**
                 * Load 'de_fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &de_fg_info));
                /**
                 * Select stage IPMF2 from dnx_field_stage_e
                 */
                de_fg_info.field_stage = DNX_FIELD_STAGE_IPMF2;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                de_fg_info.fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                /*
                 * Load two field groups with the same info. Move forward on qualifiers and actions
                 */
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_de, dir_ext_quals_for_de, num_actions_for_de,
                                 dir_ext_actions_for_de, sand_control, 1, "_DE0", &de_fg_info, &fg_id[0]));
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_de, dir_ext_quals_for_de, num_actions_for_de,
                                 dir_ext_actions_for_de, sand_control, 1, "_DE2", &de_fg_info, &fg_id[2]));
            }

            {
                num_quals_for_de = sizeof(dir_ext_quals_for_de) / sizeof(dir_ext_quals_for_de[0]);
                num_actions_for_de = sizeof(dir_ext_actions_for_de) / sizeof(dir_ext_actions_for_de[0]);

                /**
                 * Load 'de_fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &de_fg_info));
                /**
                 * Select stage IPMF2 from dnx_field_stage_e
                 */
                de_fg_info.field_stage = DNX_FIELD_STAGE_IPMF2;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                de_fg_info.fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                /*
                 * Load two field groups with the same info. Move backwards on qualifiers and actions
                 */
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_de, dir_ext_quals_for_de, num_actions_for_de,
                                 dir_ext_actions_for_de, sand_control, 0, "_DE1", &de_fg_info, &fg_id[1]));
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_de, dir_ext_quals_for_de, num_actions_for_de,
                                 dir_ext_actions_for_de, sand_control, 0, "_DE3", &de_fg_info, &fg_id[3]));
            }

            {
                /*
                 * Field group for 'replace'
                 * Create TCAM field group no. 0 (starting from '0')
                 */
                num_quals_for_tcam = sizeof(tcam_quals_for_de) / sizeof(tcam_quals_for_de[0]);
                num_actions_for_tcam = sizeof(tcam_actions_for_de) / sizeof(tcam_actions_for_de[0]);
                /**
                 * Load 'tcam_fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &tcam_fg_info));
                /**
                 * Select stage IPMF2 from dnx_field_stage_e
                 */
                tcam_fg_info.field_stage = DNX_FIELD_STAGE_IPMF1;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                tcam_fg_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
                /*
                 * Load the first of 'second_fg_id'. Move backwards on qualifiers and actions
                 */
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_tcam, tcam_quals_for_de, num_actions_for_tcam, tcam_actions_for_de,
                                 sand_control, 1, "_TCAM0", &tcam_fg_info, &second_fg_id[0]));
            }

            {
                /*
                 * Field group for 'replace'
                 * Create TCAM field group no. 1 (starting from '0')
                 */
                num_quals_for_tcam = sizeof(tcam_quals_for_de) / sizeof(tcam_quals_for_de[0]);
                num_actions_for_tcam = sizeof(tcam_actions_for_de) / sizeof(tcam_actions_for_de[0]);
                /**
                 * Load 'tcam_fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &tcam_fg_info));
                /**
                 * Select stage IPMF2 from dnx_field_stage_e
                 */
                tcam_fg_info.field_stage = DNX_FIELD_STAGE_IPMF1;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                tcam_fg_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_tcam, tcam_quals_for_de, num_actions_for_tcam, tcam_actions_for_de,
                                 sand_control, 0, "_TCAM1", &tcam_fg_info, &second_fg_id[1]));
            }

            {
                fem_id = 6;
                input_offset = 16;
                input_size = 32;
                fem_condition_ms_bit = 4;
                fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
                fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
                fem_map_data_max =
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
                fem_action_valid_max = DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES;
                fem_bit_index_max =
                    (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry[0].fem_bit_info) /
                                                 sizeof(fem_action_entry[0].fem_bit_info[0]));

                for (fem_condition_setup = 0; fem_condition_setup < CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS;
                     fem_condition_setup++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init
                                    (unit, fem_condition_entry[fem_condition_setup]));
                    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
                    {
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index =
                            fem_condition_index % fem_map_index_max;
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_data =
                            fem_condition_index % fem_map_data_max;
                        if (fem_condition_setup == 0)
                        {
                            /*
                             * For 'fem_condition_setup==0', set 'valid' for first two 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index < 2)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                        else
                        {
                            /*
                             * For other 'fem_condition_setup's, set 'valid' for last three 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index > 0)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                    }
                }
                /*
                 * 'FEM id's '0' and '1' only have 4 bits on action.
                 */
                if (fem_id <
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                {
                    fem_bit_index_max =
                        dnx_data_field.stage.stage_info_get(unit,
                                                            DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry[fem_map_index]));
                    /*
                     * Note that, for now, this code is not meaningful. We still need to pick specific actions, with specific
                     * sizes, and set the bits accordingly.
                     */
                    for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                    {
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                            DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                            fem_bit_index;
                    }
                    if (fem_id <
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                    {
                        fem_action_entry[fem_map_index].fem_adder = 0;
                    }
                    else
                    {
                        fem_action_entry[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (fem_map_index + 16);
                    }
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[0], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[0], input_offset, input_size, second_fg_id[0],
                                 fem_condition_ms_bit, &fem_condition_entry[0][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[1], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[1], input_offset, input_size, DNX_FIELD_GROUP_INVALID,
                                 fem_condition_ms_bit, &fem_condition_entry[1][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[2], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[2], input_offset, input_size, DNX_FIELD_GROUP_INVALID,
                                 fem_condition_ms_bit, &fem_condition_entry[0][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);

                /*
                 * This 'add' will occupy the 4th 'fem program' on specified 'fem_id'
                 */
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[3], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[3], input_offset, input_size, second_fg_id[1],
                                 fem_condition_ms_bit, &fem_condition_entry[1][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);

                if (clean_resources)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[0]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);

                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[1]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);

                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[2]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[3]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);
                }
            }
            {
                fem_id = 0;
                input_offset = 16;
                input_size = 32;
                fem_condition_ms_bit = 6;
                fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
                fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
                fem_map_data_max =
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
                fem_action_valid_max = DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES;
                fem_bit_index_max =
                    (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry[0].fem_bit_info) /
                                                 sizeof(fem_action_entry[0].fem_bit_info[0]));

                for (fem_condition_setup = 0; fem_condition_setup < CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS;
                     fem_condition_setup++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init
                                    (unit, fem_condition_entry[fem_condition_setup]));
                    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
                    {
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index =
                            fem_condition_index % fem_map_index_max;
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_data =
                            fem_condition_index % fem_map_data_max;
                        if (fem_condition_setup == 0)
                        {
                            /*
                             * For 'fem_condition_setup==0', set 'valid' for first two 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index < 2)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                        else
                        {
                            /*
                             * For other 'fem_condition_setup's, set 'valid' for third 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index == 2)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                    }
                }
                /*
                 * 'FEM id's '0' and '1' only have 4 bits on action.
                 */
                if (fem_id <
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                {
                    fem_bit_index_max =
                        dnx_data_field.stage.stage_info_get(unit,
                                                            DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry[fem_map_index]));
                    /*
                     * Note that, for now, this code is not meaningful. We still need to pick specific actions, with specific
                     * sizes, and set the bits accordingly.
                     */
                    for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                    {
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                            DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                            (fem_bit_index_max - 1 - fem_bit_index);
                    }
                    if (fem_id <
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                    {
                        fem_action_entry[fem_map_index].fem_adder = 0;
                    }
                    else
                    {
                        fem_action_entry[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (fem_map_index + 16);
                    }
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[0], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[0], input_offset, input_size, second_fg_id[0],
                                 fem_condition_ms_bit, &fem_condition_entry[0][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[1], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[1], input_offset, input_size, second_fg_id[1],
                                 fem_condition_ms_bit, &fem_condition_entry[1][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                if (clean_resources)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[0]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);

                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[1]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);
                }
            }
            if (clean_resources)
            {
                /*
                 * Negative test:
                 * Create two valid actions which are the same. Should fail.
                 */
                /**
                 * Increase the severity to 'fatal' to avoid seeing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

                fem_id = 0;
                input_offset = 16;
                input_size = 32;
                fem_condition_ms_bit = 6;
                fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
                fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
                fem_map_data_max =
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
                fem_action_valid_max = DBAL_NOF_ENUM_FIELD_FEM_ACTION_VALID_VALUES;
                fem_bit_index_max =
                    (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry[0].fem_bit_info) /
                                                 sizeof(fem_action_entry[0].fem_bit_info[0]));

                for (fem_condition_setup = 0; fem_condition_setup < CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS;
                     fem_condition_setup++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init
                                    (unit, fem_condition_entry[fem_condition_setup]));
                    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
                    {
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index =
                            fem_condition_index % fem_map_index_max;
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_data =
                            fem_condition_index % fem_map_data_max;
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                            (fem_condition_max - 1 - fem_condition_index) % fem_action_valid_max;
                    }
                }
                /*
                 * 'FEM id's '0' and '1' only have 4 bits on action.
                 */
                if (fem_id <
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                {
                    fem_bit_index_max =
                        dnx_data_field.stage.stage_info_get(unit,
                                                            DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry[fem_map_index]));
                    /*
                     * Note that, for now, this code is not meaningful. We still need to pick specific actions, with specific
                     * sizes, and set the bits accordingly.
                     */
                    for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                    {
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                            DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                            (fem_bit_index_max - 1 - fem_bit_index);
                    }
                    /*
                     * Note that this is only meaningful for FEM 2-23 !!!
                     */
                    if (fem_id <
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                    {
                        fem_action_entry[fem_map_index].fem_adder = 0;
                    }
                    else
                    {
                        fem_action_entry[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (16);
                    }
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[0], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0));
                }
                rv = dnx_field_fem_action_add
                    (unit, fem_id, fg_id[0], input_offset, input_size, second_fg_id[0], fem_condition_ms_bit,
                     &fem_condition_entry[0][0], fem_encoded_actions, fem_action_entry);
                if (rv != _SHR_E_NONE)
                {
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, NEGATIVE TEST for dnx_field_fem_action_add() (Two similar actions) has failed as expected! %s%s\r\n",
                                __FUNCTION__, __LINE__, EMPTY, EMPTY);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "NEGATIVE TEST for dnx_field_fem_action_add() (Two similar actions) has succeeded. Test has failed!\r\n");
                }
                /**
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
            }
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_FEM_ATTACH_TYPE:
        {
            dnx_field_group_t fg_id[CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_ATTACH];
            dnx_field_group_info_t fg_info[CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_ATTACH];
            unsigned int qual_index, action_index;
            /**
             * Number of qualifiers to use for testing interface
             * of direct extraction on DNX level.
             */
            unsigned int num_quals_for_de;
            /**
             * Number of actions to use for testing interface
             * of direct extraction on DNX level.
             */
            unsigned int num_actions_for_de;
            /**
             * Number of qualifiers to use for testing interface
             * of TCAM on DNX level.
             */
            unsigned int num_quals_for_tcam;
            /**
             * Number of actions to use for testing interface
             * of TCAM on DNX level.
             */
            unsigned int num_actions_for_tcam;
            dnx_field_fem_id_t fem_id;
            uint8 input_offset, input_size;
            dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit;
            dnx_field_fem_condition_entry_t
                fem_condition_entry[CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS]
                [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
            dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
            dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
            dnx_field_fem_condition_t fem_condition_setup, fem_condition_index, fem_condition_max;
            dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
            dnx_field_fem_map_data_t fem_map_data_max;
            dnx_field_fem_bit_index_t fem_bit_index_max, fem_bit_index;
            dnx_field_context_t ctx_id[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH];

/* *INDENT-OFF* */
            /*
             * Input parameters for the 'new' direct extraction style. To be used on the 'FEM' test below.
             * First table is qualifiers' parameters and second table contains the matching actions.
             * Note that the total size for both needs to be the same!
             */
            dnx_ctest_field_quals_for_fg_t dir_ext_quals_for_de[] =
            {
                 /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
                {"user qual  0",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                  5       }},

                {"user qual  1",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
                {"user qual  2",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }},

                {"user qual  3",    3,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 27       }},
                {"user qual  4",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 30       }},
                {"user qual  5",    4,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }}
            };
            dnx_ctest_field_actions_for_fg_t dir_ext_actions_for_de[] =
            {
                 /* name */      /* size */      /* DBAL action id */                    /* Priority */
                {"user action 0",    5,    DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {       3         }},
                {"user action 1",   15,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       4         }},
                {"user action 2",   12,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       5         }}
            };
            /*
             * Input parameters for general TCAM style. To be used on the 'FEM' test below.
             * First table is qualifiers' parameters and second table contains the matching actions.
             * Note that the total size for both needs to be the same!
             */
            dnx_ctest_field_quals_for_fg_t tcam_quals[] =
            {
                 /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
                {"user qual  10",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                  5       }},
                {"user qual  11",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
                {"user qual  12",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }}
            };
            dnx_ctest_field_actions_for_fg_t tcam_actions[] =
            {
                 /* name */      /* size */      /* DBAL action id */                    /* Priority */
                {"user action 10",    5,    DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {       3         }},
                {"user action 11",   10,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       4         }},
                {"user action 12",   12,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       5         }},
                {"user action 13",   32,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {      12         }}
            };
/* *INDENT-ON* */

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_fem_action_add()", "dnx_field_fem_action_remove()");
            {
                /*
                 * Create Direct extraction field group no. 0 (starting from '0')
                 */
                num_quals_for_de = sizeof(dir_ext_quals_for_de) / sizeof(dir_ext_quals_for_de[0]);
                num_actions_for_de = sizeof(dir_ext_actions_for_de) / sizeof(dir_ext_actions_for_de[0]);

                /**
                 * Load 'fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info[0]));
                /**
                 * Select stage IPMF2 from dnx_field_stage_e
                 */
                fg_info[0].field_stage = DNX_FIELD_STAGE_IPMF2;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                fg_info[0].fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_de, dir_ext_quals_for_de, num_actions_for_de,
                                 dir_ext_actions_for_de, sand_control, 1, "_DE0", &(fg_info[0]), &fg_id[0]));
            }

            {
                /*
                 * Field group for general TCAM
                 * Create TCAM field group no. 1 (starting from '0')
                 */
                num_quals_for_tcam = sizeof(tcam_quals) / sizeof(tcam_quals[0]);
                num_actions_for_tcam = sizeof(tcam_actions) / sizeof(tcam_actions[0]);
                /**
                 * Load 'fg_info[1]'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info[1]));
                /**
                 * Select stage IPMF2 from dnx_field_stage_e
                 */
                fg_info[1].field_stage = DNX_FIELD_STAGE_IPMF1;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                fg_info[1].fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg
                                (unit, num_quals_for_tcam, tcam_quals, num_actions_for_tcam, tcam_actions, sand_control,
                                 1, "_TCAM1", &(fg_info[1]), &fg_id[1]));
            }

            {
                /*
                 * Load info for 'fem_id' no. 6 on the 'context free' side.
                 */
                fem_id = 6;
                input_offset = 16;
                input_size = 32;
                fem_condition_ms_bit = 4;
                fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
                fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
                fem_map_data_max =
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
                fem_bit_index_max =
                    (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry[0].fem_bit_info) /
                                                 sizeof(fem_action_entry[0].fem_bit_info[0]));
                /*
                 * Load array for 'condition's
                 */
                for (fem_condition_setup = 0; fem_condition_setup < CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS;
                     fem_condition_setup++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init
                                    (unit, fem_condition_entry[fem_condition_setup]));
                    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
                    {
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index =
                            fem_condition_index % fem_map_index_max;
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_data =
                            fem_condition_index % fem_map_data_max;
                        if (fem_condition_setup == 0)
                        {
                            /*
                             * For 'fem_condition_setup==0', set 'valid' for first two 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index < 2)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                        else
                        {
                            /*
                             * For other 'fem_condition_setup's, set 'valid' for last three 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index > 0)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                    }
                }
                /*
                 * 'FEM id's '0' and '1' have only 4 bits on action.
                 */
                if (fem_id <
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                {
                    fem_bit_index_max =
                        dnx_data_field.stage.stage_info_get(unit,
                                                            DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
                }
                /*
                 * Load 'map indices' with pointers to 'action's
                 */
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry[fem_map_index]));
                    /*
                     * Note that, for now, this code is not meaningful. We still need to pick specific actions, with specific
                     * sizes, and set the bits accordingly.
                     */
                    for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                    {
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                            DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                            fem_bit_index;
                    }
                    if (fem_id <
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                    {
                        fem_action_entry[fem_map_index].fem_adder = 0;
                    }
                    else
                    {
                        fem_action_entry[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (fem_map_index + 16);
                    }
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[0], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[0], input_offset, input_size, DNX_FIELD_GROUP_INVALID,
                                 fem_condition_ms_bit, &fem_condition_entry[0][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[1], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[1], input_offset, input_size, DNX_FIELD_GROUP_INVALID,
                                 fem_condition_ms_bit, &fem_condition_entry[1][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
            }
            {
                /*
                 * Attach contexts to all Field Groups.
                 */
                dnx_field_group_attach_info_t attach_info[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH];
                dnx_field_context_mode_t context_mode[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH];
                dnx_field_stage_e stage_for_context_create[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH];
                unsigned int attach_index;
                unsigned int fg_index;
                dnx_field_context_flags_e flags;

                flags = 0;
                for (attach_index = 0; attach_index < CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH; attach_index++)
                {
                    /*
                     * Create Context
                     */
                    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode[attach_index]));
                    if (attach_index < 1)
                    {
                        fg_index = 0;
                    }
                    else
                    {
                        fg_index = 1;
                    }
                    stage_for_context_create[attach_index] = fg_info[fg_index].field_stage;
                    if (stage_for_context_create[attach_index] == DNX_FIELD_STAGE_IPMF2)
                    {
                        SHR_IF_ERR_EXIT(dnx_field_context_create
                                        (unit, flags, DNX_FIELD_STAGE_IPMF1, &context_mode[attach_index],
                                         &ctx_id[attach_index]));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_field_context_create
                                        (unit, flags, stage_for_context_create[attach_index],
                                         &context_mode[attach_index], &ctx_id[attach_index]));
                    }
                    /*
                     * Configure attach API info
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info[attach_index]));
                    if (attach_index < 1)
                    {
                        for (qual_index = 0; qual_index < num_quals_for_de; qual_index++)
                        {
                            attach_info[attach_index].dnx_quals[qual_index] = fg_info[fg_index].dnx_quals[qual_index];
                            attach_info[attach_index].qual_info[qual_index].input_type =
                                dir_ext_quals_for_de[qual_index].qual_attach_info.input_type;
                            attach_info[attach_index].qual_info[qual_index].input_arg =
                                dir_ext_quals_for_de[qual_index].qual_attach_info.input_arg;
                            attach_info[attach_index].qual_info[qual_index].offset =
                                dir_ext_quals_for_de[qual_index].qual_attach_info.offset;
                        }
                        for (action_index = 0; action_index < num_actions_for_de; action_index++)
                        {
                            attach_info[attach_index].dnx_actions[action_index] =
                                fg_info[fg_index].dnx_actions[action_index];
                            attach_info[attach_index].action_info[action_index].priority =
                                BCM_FIELD_ACTION_PRIORITY(0,
                                                          dir_ext_actions_for_de[action_index].
                                                          action_attach_info.priority);
                        }
                    }
                    else
                    {
                        for (qual_index = 0; qual_index < num_quals_for_tcam; qual_index++)
                        {
                            attach_info[attach_index].dnx_quals[qual_index] = fg_info[fg_index].dnx_quals[qual_index];
                            attach_info[attach_index].qual_info[qual_index].input_type =
                                tcam_quals[qual_index].qual_attach_info.input_type;
                            attach_info[attach_index].qual_info[qual_index].input_arg =
                                tcam_quals[qual_index].qual_attach_info.input_arg;
                            attach_info[attach_index].qual_info[qual_index].offset =
                                tcam_quals[qual_index].qual_attach_info.offset;
                        }
                        for (action_index = 0; action_index < num_actions_for_tcam; action_index++)
                        {
                            attach_info[attach_index].dnx_actions[action_index] =
                                fg_info[fg_index].dnx_actions[action_index];
                            attach_info[attach_index].action_info[action_index].priority =
                                BCM_FIELD_ACTION_PRIORITY(0, tcam_actions[action_index].action_attach_info.priority);
                        }
                    }
                    SHR_IF_ERR_EXIT(dnx_field_group_context_attach
                                    (unit, 0, fg_id[fg_index], ctx_id[attach_index], &attach_info[attach_index]));
                }
                if (clean_resources)
                {
                    for (attach_index = 0; attach_index < CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_FG_ON_ATTACH;
                         attach_index++)
                    {
                        if (attach_index < 1)
                        {
                            fg_index = 0;
                        }
                        else
                        {
                            fg_index = 1;
                        }
                        SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id[fg_index], ctx_id[attach_index]));
                    }
                }
            }
            if (clean_resources)
            {
                /*
                 * Remove FEMs from field group nos. 0 and 1 (starting from '0')
                 */
                SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[0]));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);

                SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[1]));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);
            }

            if (clean_resources)
            {
                /*
                 * Load info for 'fem_id' no. 0 on the 'context free' side.
                 */
                fem_id = 0;
                input_offset = 16;
                input_size = 32;
                fem_condition_ms_bit = 6;
                fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
                fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
                fem_map_data_max =
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
                fem_bit_index_max =
                    (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry[0].fem_bit_info) /
                                                 sizeof(fem_action_entry[0].fem_bit_info[0]));

                for (fem_condition_setup = 0; fem_condition_setup < CTEST_DNX_FIELD_FEM_NUM_CONDITION_SETUPS;
                     fem_condition_setup++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init
                                    (unit, fem_condition_entry[fem_condition_setup]));
                    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
                    {
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index =
                            fem_condition_index % fem_map_index_max;
                        fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_data =
                            fem_condition_index % fem_map_data_max;
                        if (fem_condition_setup == 0)
                        {
                            /*
                             * For 'fem_condition_setup==0', set 'valid' for first two 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index < 2)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                        else
                        {
                            /*
                             * For other 'fem_condition_setup's, set 'valid' for third 'actions'
                             */
                            if (fem_condition_entry[fem_condition_setup][fem_condition_index].fem_map_index == 2)
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_condition_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                    }
                }
                /*
                 * 'FEM id's '0' and '1' have only 4 bits on action.
                 */
                if (fem_id <
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                {
                    fem_bit_index_max =
                        dnx_data_field.stage.stage_info_get(unit,
                                                            DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry[fem_map_index]));
                    /*
                     * Note that, for now, this code is not meaningful. We still need to pick specific actions, with specific
                     * sizes, and set the bits accordingly.
                     */
                    for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                    {
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                            DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                        fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_value.bit_on_key_select =
                            (fem_bit_index_max - 1 - fem_bit_index);
                    }
                    if (fem_id <
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                    {
                        fem_action_entry[fem_map_index].fem_adder = 0;
                    }
                    else
                    {
                        fem_action_entry[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (fem_map_index + 16);
                    }
                }
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[0], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[0], input_offset, input_size, DNX_FIELD_GROUP_INVALID,
                                 fem_condition_ms_bit, &fem_condition_entry[0][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                {
                    dnx_field_stage_e field_stage;

                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[1], &field_stage));
                    fem_encoded_actions[fem_map_index] =
                        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC,
                                   field_stage, (DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0 + fem_map_index));
                }
                SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                (unit, fem_id, fg_id[1], input_offset, input_size, DNX_FIELD_GROUP_INVALID,
                                 fem_condition_ms_bit, &fem_condition_entry[1][0], fem_encoded_actions,
                                 fem_action_entry));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
            }
            if (clean_resources)
            {
                /*
                 * Remove FEMs from Direct extraction field group nos. 0 and 1 (starting from '0')
                 */
                SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[0]));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);

                SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[1]));
                APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_remove", sand_control);
            }
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_ENCODED_POSITION_TYPE:
        {
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing encoding/decooding of 'fem_id' within 'encoded_position': %s %s\r\n",
                        __FUNCTION__, __LINE__, "DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET()",
                        "BCM_FIELD_ACTION_POSITION()");
            {
                bcm_field_action_priority_t encoded_position;
                int array_id;
                bcm_field_fem_id_t resultant_fem_id;
                bcm_field_fem_id_t input_fem_id;
                bcm_field_fem_id_t fem_id_in_array;
                dnx_field_stage_e field_stage;
                int nof_fem_id_per_array;
                field_stage = DNX_FIELD_STAGE_IPMF2;
                nof_fem_id_per_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fem_id_per_array;
                encoded_position = 0;
                resultant_fem_id = -1;
                input_fem_id = 3;
                if (input_fem_id < nof_fem_id_per_array)
                {
                    array_id = 1;
                    fem_id_in_array = input_fem_id;
                }
                else
                {
                    array_id = 3;
                    fem_id_in_array = input_fem_id - nof_fem_id_per_array;
                }
                encoded_position = BCM_FIELD_ACTION_POSITION(array_id, fem_id_in_array);
                DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(resultant_fem_id, unit, encoded_position);
                if (resultant_fem_id != input_fem_id)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "'fem_id' before encoding is %d. 'fem_id' after decoding is %d. Test has FAILED!\r\n",
                                 input_fem_id, resultant_fem_id);
                }
                input_fem_id = 13;
                if (input_fem_id < nof_fem_id_per_array)
                {
                    array_id = 1;
                    fem_id_in_array = input_fem_id;
                }
                else
                {
                    array_id = 3;
                    fem_id_in_array = input_fem_id - nof_fem_id_per_array;
                }
                encoded_position = BCM_FIELD_ACTION_POSITION(array_id, fem_id_in_array);
                DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(resultant_fem_id, unit, encoded_position);
                if (resultant_fem_id != input_fem_id)
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "'fem_id' before encoding is %d. 'fem_id' after decoding is %d. Test has FAILED!\r\n",
                                 input_fem_id, resultant_fem_id);
                }
                /*
                 * Negative test. Set 'array_id' which is not for FEMs. Expect 'invalid parameter'.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDTESTSDNX, bslSeverityFatal);
                input_fem_id = 3;
                if (input_fem_id < nof_fem_id_per_array)
                {
                    array_id = 0;
                    fem_id_in_array = input_fem_id;
                }
                else
                {
                    array_id = 2;
                    fem_id_in_array = input_fem_id - nof_fem_id_per_array;
                }
                encoded_position = BCM_FIELD_ACTION_POSITION(array_id, fem_id_in_array);
                SHR_SET_CURRENT_ERR(dnx_field_action_priority_position_fem_id_get
                                    (&resultant_fem_id, unit, encoded_position));
                /**
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDTESTSDNX, original_severity_fldprocdnx);
                if (SHR_GET_CURRENT_ERR() != BCM_E_PARAM)
                {
                    if (SHR_GET_CURRENT_ERR() == BCM_E_NONE)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "'array_id' (%d) is not for FEMs but no error was injected by DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET. Test has FAILED!\r\n",
                                     array_id);
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "'array_id' (%d) is not for FEMs but error %d (%s) was injected by DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET. Test has FAILED!\r\n",
                                     array_id, SHR_GET_CURRENT_ERR(), shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
                    }
                }
                /*
                 * Negative test. Set 'array_id' which is out of range. Expect 'invalid parameter'.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDTESTSDNX, bslSeverityFatal);
                input_fem_id = 3;
                if (input_fem_id < nof_fem_id_per_array)
                {
                    array_id = 4;
                    fem_id_in_array = input_fem_id;
                }
                else
                {
                    array_id = 5;
                    fem_id_in_array = input_fem_id - nof_fem_id_per_array;
                }
                encoded_position = BCM_FIELD_ACTION_POSITION(array_id, fem_id_in_array);
                SHR_SET_CURRENT_ERR(dnx_field_action_priority_position_fem_id_get
                                    (&resultant_fem_id, unit, encoded_position));
                if (SHR_GET_CURRENT_ERR() != BCM_E_PARAM)
                {
                    if (SHR_GET_CURRENT_ERR() == BCM_E_NONE)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "'array_id' (%d) is out of range but no error was injected by DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET. Test has FAILED!\r\n",
                                     array_id);
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL,
                                     "'array_id' (%d) is out of range but error %d (%s) was injected by DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET. Test has FAILED!\r\n",
                                     array_id, SHR_GET_CURRENT_ERR(), shrextend_errmsg_get(SHR_GET_CURRENT_ERR()));
                    }
                }
                /**
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDTESTSDNX, original_severity_fldprocdnx);
            }
            {
                /*
                 * Test conversion between encoded priority and FEM ID, as the two conversions use different DNX data.
                 */
                dnx_field_stage_e field_stage_arr[] = { DNX_FIELD_STAGE_IPMF1, DNX_FIELD_STAGE_IPMF2 };
                int field_stage_index;
                for (field_stage_index = 0; field_stage_index < (sizeof(field_stage_arr) / sizeof(field_stage_arr[0]));
                     field_stage_index++)
                {
                    bcm_field_action_priority_t encoded_position;
                    bcm_field_action_priority_t encoded_position_get;
                    bcm_field_fem_id_t fem_id;
                    bcm_field_fem_id_t fem_id_get;
                    int nof_fem_id_per_array;
                    int nof_fem_array;
                    int nof_fem_id;
                    int nof_action_arrays;
                    int array_id;
                    int position_within_array;
                    int fes_array_index;
                    int fem_array_index;
                    dnx_field_stage_e field_stage;

                    field_stage = field_stage_arr[field_stage_index];
                    nof_fem_id_per_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fem_id_per_array;
                    nof_fem_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fem_array;
                    nof_fem_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fem_id;
                    nof_action_arrays = dnx_data_field.common_max_val.nof_array_ids_get(unit);

                    for (fem_id = 0; fem_id < nof_fem_id; fem_id++)
                    {
                        DNX_FIELD_ACTION_FEM_ID_ENCODED_POSITION_GET(encoded_position_get, unit, fem_id);
                        DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(fem_id_get, unit, encoded_position_get);
                        if (fem_id != fem_id_get)
                        {
                            SHR_ERR_EXIT(_SHR_E_FAIL,
                                         "fem_id %d converted to encoded position 0x%x and converted back to %d.\r\n",
                                         fem_id, encoded_position_get, fem_id_get);
                        }
                    }
                    for (array_id = 0; array_id < nof_fem_array; array_id++)
                    {
                        for (position_within_array = 0; position_within_array < nof_fem_id_per_array;
                             position_within_array++)
                        {
                            encoded_position = BCM_FIELD_ACTION_POSITION(array_id * 2 + 1, position_within_array);
                            DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(fem_id_get, unit, encoded_position);
                            DNX_FIELD_ACTION_FEM_ID_ENCODED_POSITION_GET(encoded_position_get, unit, fem_id_get);
                            if (encoded_position != encoded_position_get)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL,
                                             "FEM array %d position within array %d converts to "
                                             "encoded position 0x%x, converts to fem_id %d converts to "
                                             "encoded position 0x%x.\r\n",
                                             array_id, position_within_array, encoded_position, fem_id_get,
                                             encoded_position_get);
                            }
                        }
                    }
                    fes_array_index = 0;
                    fem_array_index = 0;
                    for (array_id = 0; array_id < nof_action_arrays; array_id++)
                    {
                        if (array_id % 2 == 0)
                        {
                            if (dnx_data_field.common_max_val.array_id_type_get(unit, array_id)->is_fem)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL, "Array %d is marked as FEM.\r\n", array_id);
                            }
                            if (dnx_data_field.common_max_val.array_id_type_get(unit, array_id)->is_fes == FALSE)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL, "Array %d is not marked as FES.\r\n", array_id);
                            }
                            if (dnx_data_field.common_max_val.array_id_type_get(unit, array_id)->fes_array_index !=
                                fes_array_index)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL, "Array %d is FES number %d instead of %d.\r\n", array_id,
                                             dnx_data_field.common_max_val.array_id_type_get(unit,
                                                                                             array_id)->fes_array_index,
                                             fes_array_index);
                            }
                            fes_array_index++;
                        }
                        else
                        {
                            if (dnx_data_field.common_max_val.array_id_type_get(unit, array_id)->is_fem == FALSE)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL, "Array %d is not marked as FEM.\r\n", array_id);
                            }
                            if (dnx_data_field.common_max_val.array_id_type_get(unit, array_id)->is_fes)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL, "Array %d is marked as FES.\r\n", array_id);
                            }
                            if (dnx_data_field.common_max_val.array_id_type_get(unit, array_id)->fem_array_index !=
                                fem_array_index)
                            {
                                SHR_ERR_EXIT(_SHR_E_FAIL, "Array %d is FEM number %d instead of %d.\r\n", array_id,
                                             dnx_data_field.common_max_val.array_id_type_get(unit,
                                                                                             array_id)->fem_array_index,
                                             fem_array_index);
                            }
                            fem_array_index++;
                        }
                    }
                    if (fes_array_index != dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "Number of FES arrays %d instead of %d.\r\n",
                                     fes_array_index,
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array);
                    }
                    if (fem_array_index != nof_fem_array)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "Number of FEM arrays %d instead of %d.\r\n",
                                     fem_array_index, nof_fem_array);
                    }
                    if (nof_fem_id != nof_fem_array * nof_fem_id_per_array)
                    {
                        SHR_ERR_EXIT(_SHR_E_FAIL, "Number of FEM IDs is %d, while number of FEM arrays (%d) "
                                     "times FEM IDs per array (%d) is %d..\r\n",
                                     nof_fem_id, nof_fem_array, nof_fem_id_per_array,
                                     (nof_fem_array * nof_fem_id_per_array));
                    }
                }
            }
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, SUCCESSFULLY completed testing encoding/decooding of 'fem_id' within 'encoded_position': %s %s\r\n",
                        __FUNCTION__, __LINE__, "DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET()",
                        "BCM_FIELD_ACTION_POSITION()");
            break;
        }
        case CTEST_DNX_FIELD_FEM_TEST_FEM_CASCADE_TYPE:
        {
            /*
             * Carry out the following test:
             * 'a'. 'fg_add' a TCAM Field Group on IPMF2 (to be the 'secondary')
             * 'b'. 'fg_add' a TCAM Field Group on IPMF1 (to be the 'owner' ('mail'))
             * 'c'. Create a context on IPMF1 (to be used by the 'owner')
             * 'd'. Create a number of contexts (say, three) on IPMF2 which are cascaded from
             *      the 'owner' context. These are the 'sibling' contexts
             * 'e'. Attach the 'sibling' contexts to the 'secondary' Field Group.
             * 'f'. 'fem_add' a single FEM to the 'owner' Field Group.
             * 'g'. Attach the 'owner' Field Group to its context.
             * 'h'. Try to detach a 'sibling' context (expected to fail)
             * 'i'. Detach the 'owner' context. Verify that the 'sibling's are not
             *      on the list of this FEM (not on any 'fem context')
             * 'j'. Detach the 'sibling' contexts.
             * 'k'. 'fem_remove' added fems from 'owner' field group.
             * 'l'. 'fg_delete' all pending field groups.
             */
            /*
             * Variables related to the creation of the field groups.
             */
            dnx_field_group_t fg_id[CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_CASCADE];
            dnx_field_group_info_t fg_info[CTEST_DNX_FIELD_FEM_NUM_FGS_TO_CREATE_ON_CASCADE];
            dnx_field_context_t ctx_id_for_owner[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_OWNER_FG_ON_CASCADE];
            dnx_field_context_t ctx_id_for_secondary[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE];

/* *INDENT-OFF* */
            /*
             * Input parameters for the 'owner' TCAM field group. To be used on the 'FEM' test below.
             * First table is qualifiers' parameters and second table contains the matching actions.
             * Note that the total size for both needs to be the same!
             */
            dnx_ctest_field_quals_for_fg_t owner_tcam_quals[] =
            {
                 /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
                {"user_qual_0",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                  5       }},

                {"user_qual_1",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
                {"user_qual_2",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }},

                {"user_qual_3",    3,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 27       }},
                {"user_qual_4",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 30       }},
                {"user_qual_5",    4,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 35       }}
            };
            dnx_ctest_field_actions_for_fg_t owner_tcam_actions[] =
            {
                 /* name */      /* size */      /* DBAL action id */                    /* Priority */
                {"user_action_0",    5,    DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {       3         }},
                {"user_action_1",   15,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       4         }},
                {"user_action_2",   12,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       5         }}
            };
            /*
             * Input parameters for 'secondary' TCAM field group. To be used on the 'FEM' test below.
             * First table is qualifiers' parameters and second table contains the matching actions.
             * Note that the total size for both needs to be the same!
             */
            dnx_ctest_field_quals_for_fg_t secondary_tcam_quals[] =
            {
                 /* name */    /* size */  /* input type */                     /* input arg */    /* offset */
                {"user_qual_10",    5,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                  5       }},
                {"user_qual_11",    8,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 10       }},
                {"user_qual_12",    7,    {DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE,      0,                 20       }}
            };
            dnx_ctest_field_actions_for_fg_t secondary_tcam_actions[] =
            {
                 /* name */      /* size */      /* DBAL action id */                    /* Priority */
                {"user_action_10",    5,    DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1, {       3         }},
                {"user_action_11",   10,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       4         }},
                {"user_action_12",   12,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {       5         }},
                {"user_action_13",   32,    DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA,      {      12         }}
            };

/* *INDENT-ON* */

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s(), line %d, Testing various FEM operations, related to context: %s %s\r\n",
                        __FUNCTION__, __LINE__, "dnx_field_fem_action_add(), dnx_field_fem_action_remove()", EMPTY);
            LOG_INFO_EX(BSL_LOG_MODULE,
                        "%s %s %s %s\r\n",
                        "==> dnx_field_group_context_attach(), dnx_field_group_context_detach()\r\n", EMPTY, EMPTY,
                        EMPTY);

            {
                int fg_index;
                unsigned int num_quals;
                unsigned int num_actions;
                /**
                 * Number of qualifiers for 'owner' Field Group on DNX level.
                 */
                num_quals = sizeof(owner_tcam_quals) / sizeof(owner_tcam_quals[0]);
                /**
                 * Number of actions for 'owner' Field Group on DNX level.
                 */
                num_actions = sizeof(owner_tcam_actions) / sizeof(owner_tcam_actions[0]);
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE;
                /**
                 * Load 'fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info[fg_index]));
                /**
                 * Select stage IPMF1 from dnx_field_stage_e
                 */
                fg_info[fg_index].field_stage = DNX_FIELD_STAGE_IPMF1;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                fg_info[fg_index].fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
                /*
                 * *** Step 'a' ***
                 * Create owner TCAM field group
                 * Create TCAM field group no. 0 (starting from '0')
                 */
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg(unit, num_quals, owner_tcam_quals, num_actions,
                                                                 owner_tcam_actions, sand_control, 1, "_TCAM_P",
                                                                 &(fg_info[fg_index]), &fg_id[fg_index]));
                /**
                 * Number of qualifiers for 'secondary' Field Group on DNX level.
                 */
                num_quals = sizeof(secondary_tcam_quals) / sizeof(secondary_tcam_quals[0]);
                /**
                 * Number of actions for 'secondary' Field Group on DNX level.
                 */
                num_actions = sizeof(secondary_tcam_actions) / sizeof(secondary_tcam_actions[0]);
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE;
                /**
                 * Load 'fg_info'
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info[fg_index]));
                /**
                 * Select stage IPMF1 from dnx_field_stage_e
                 */
                fg_info[fg_index].field_stage = DNX_FIELD_STAGE_IPMF2;
                /**
                 * Select type from dnx_field_group_type_e
                 */
                fg_info[fg_index].fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
                /*
                 * *** Step 'a' ***
                 * Create secondary TCAM field group
                 * Create TCAM field group no. 1 (starting from '0')
                 */
                SHR_IF_ERR_EXIT(ctest_dnx_field_action_create_fg(unit, num_quals, secondary_tcam_quals, num_actions,
                                                                 secondary_tcam_actions, sand_control, 1, "_TCAM_S",
                                                                 &(fg_info[fg_index]), &fg_id[fg_index]));

            }
            {
                int fg_index;
                dnx_field_context_mode_t
                    context_mode_for_owner[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_OWNER_FG_ON_CASCADE];
                dnx_field_stage_e
                    stage_for_context_create_for_owner[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_OWNER_FG_ON_CASCADE];
                dnx_field_context_mode_t
                    context_mode_for_secondary[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE];
                dnx_field_stage_e
                    stage_for_context_create_for_secondary
                    [CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE];
                unsigned int context_index;
                dnx_field_context_flags_e flags;
                /*
                 * Create Context on IPMF1 for 'owner'. ( *** step 'c' ***)
                 */
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE;
                context_index = 0;
                SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode_for_owner[context_index]));
                stage_for_context_create_for_owner[context_index] = fg_info[fg_index].field_stage;
                flags = 0;
                SHR_IF_ERR_EXIT(dnx_field_context_create
                                (unit, flags, stage_for_context_create_for_owner[context_index],
                                 &context_mode_for_owner[context_index], &ctx_id_for_owner[context_index]));
                /*
                 * Create Contexts on IPMF2 for 'secondary'. ( *** step 'd' ***)
                 */
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE;
                flags = 0;
                for (context_index = 0; context_index < CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE;
                     context_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode_for_secondary[context_index]));
                    stage_for_context_create_for_secondary[context_index] = fg_info[fg_index].field_stage;
                    context_mode_for_secondary[context_index].context_ipmf2_mode.cascaded_from =
                        ctx_id_for_owner[CTEST_DNX_FIELD_FEM_FIRST_CONTEXT_PER_OWNER_FG_ON_CASCADE];
                    SHR_IF_ERR_EXIT(dnx_field_context_create
                                    (unit, flags, stage_for_context_create_for_secondary[context_index],
                                     &context_mode_for_secondary[context_index], &ctx_id_for_secondary[context_index]));
                }
            }
            {
                /*
                 * *** Step 'e' ***
                 * Attach contexts to secondary Field Group.
                 */
                dnx_field_group_attach_info_t attach_info[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE];
                unsigned int attach_index;
                unsigned int fg_index;
                unsigned int qual_index, action_index;
                unsigned int num_quals;
                unsigned int num_actions;
                dnx_ctest_field_quals_for_fg_t *tcam_quals;

                /**
                 * Number of qualifiers for 'secondary' Field Group on DNX level.
                 */
                num_quals = sizeof(secondary_tcam_quals) / sizeof(secondary_tcam_quals[0]);
                /**
                 * Number of actions for 'secondary' Field Group on DNX level.
                 */
                num_actions = sizeof(secondary_tcam_actions) / sizeof(secondary_tcam_actions[0]);
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE;
                tcam_quals = secondary_tcam_quals;
                for (attach_index = 0; attach_index < CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE;
                     attach_index++)
                {
                    /*
                     * Configure attach API info
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info[attach_index]));
                    for (qual_index = 0; qual_index < num_quals; qual_index++)
                    {
                        attach_info[attach_index].dnx_quals[qual_index] = fg_info[fg_index].dnx_quals[qual_index];
                        attach_info[attach_index].qual_info[qual_index].input_type =
                            tcam_quals[qual_index].qual_attach_info.input_type;
                        attach_info[attach_index].qual_info[qual_index].input_arg =
                            tcam_quals[qual_index].qual_attach_info.input_arg;
                        attach_info[attach_index].qual_info[qual_index].offset =
                            tcam_quals[qual_index].qual_attach_info.offset;
                    }
                    for (action_index = 0; action_index < num_actions; action_index++)
                    {
                        attach_info[attach_index].dnx_actions[action_index] =
                            fg_info[fg_index].dnx_actions[action_index];
                        attach_info[attach_index].action_info[action_index].priority =
                            BCM_FIELD_ACTION_PRIORITY(0, action_index);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_group_context_attach
                                    (unit, 0, fg_id[fg_index], ctx_id_for_secondary[attach_index],
                                     &attach_info[attach_index]));
                }
                /*
                 * Note that we need to also attach the 'owner' context since it is always 'cascaded from'
                 * its own context. (A context created oin IPMF1 automatically gets a 'cascaded' image
                 * on IPMF2.)
                 */
                attach_index = CTEST_DNX_FIELD_FEM_FIRST_CONTEXT_PER_OWNER_FG_ON_CASCADE;
                {
                    /*
                     * Configure attach API info
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info[attach_index]));
                    for (qual_index = 0; qual_index < num_quals; qual_index++)
                    {
                        attach_info[attach_index].dnx_quals[qual_index] = fg_info[fg_index].dnx_quals[qual_index];
                        attach_info[attach_index].qual_info[qual_index].input_type =
                            tcam_quals[qual_index].qual_attach_info.input_type;
                        attach_info[attach_index].qual_info[qual_index].input_arg =
                            tcam_quals[qual_index].qual_attach_info.input_arg;
                        attach_info[attach_index].qual_info[qual_index].offset =
                            tcam_quals[qual_index].qual_attach_info.offset;
                    }
                    for (action_index = 0; action_index < num_actions; action_index++)
                    {
                        attach_info[attach_index].dnx_actions[action_index] =
                            fg_info[fg_index].dnx_actions[action_index];
                        attach_info[attach_index].action_info[action_index].priority =
                            BCM_FIELD_ACTION_PRIORITY(0, (fg_id[fg_index] * 100) + action_index);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_group_context_attach
                                    (unit, 0, fg_id[fg_index], ctx_id_for_owner[attach_index],
                                     &attach_info[attach_index]));
                }
            }
            {
                /*
                 * *** Step 'f' ***
                 * Load info for 'fem_id' no. 6 and fem id no. 0 on the 'context free' side and add to
                 * 'owner' field group.
                 */
                dnx_field_fem_condition_entry_t
                    fem_condition_entry[CTEST_DNX_FIELD_FEM_NUM_FEM_SETUPS_CASCADED]
                    [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION];
                dnx_field_fem_condition_t fem_condition_index, fem_condition_max;
                int fem_setup;
                uint8 input_offset, input_size;
                dnx_field_fem_id_t fem_id;
                dnx_field_fem_condition_ms_bit_t fem_condition_ms_bit;
                unsigned int fg_index, second_fg_index;
                dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
                dbal_enum_value_field_ipmf1_action_e base_fem_encoded_actions;
                dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX];
                dnx_field_fem_map_index_t fem_map_index, fem_map_index_max;
                dnx_field_fem_map_data_t fem_map_data_max;
                dnx_field_fem_bit_index_t fem_bit_index_max, fem_bit_index;

                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE;
                second_fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE;
                fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
                fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
                fem_map_data_max =
                    dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_map_data_field;
                fem_bit_index_max =
                    (dnx_field_fem_bit_index_t) (sizeof(fem_action_entry[0].fem_bit_info) /
                                                 sizeof(fem_action_entry[0].fem_bit_info[0]));
                /*
                 * Load array for 'condition's
                 */
                for (fem_setup = 0; fem_setup < CTEST_DNX_FIELD_FEM_NUM_FEM_SETUPS_CASCADED; fem_setup++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init(unit, fem_condition_entry[fem_setup]));
                    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
                    {
                        fem_condition_entry[fem_setup][fem_condition_index].fem_map_index =
                            fem_condition_index % fem_map_index_max;
                        fem_condition_entry[fem_setup][fem_condition_index].fem_map_data =
                            fem_condition_index % fem_map_data_max;
                        if (fem_setup == 0)
                        {
                            /*
                             * For 'fem_setup==0', set 'valid' for first two 'actions'
                             */
                            if (fem_condition_entry[fem_setup][fem_condition_index].fem_map_index < 2)
                            {
                                fem_condition_entry[fem_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                        else
                        {
                            /*
                             * For other 'fem_setup's, set 'valid' for last three 'actions'
                             */
                            if (fem_condition_entry[fem_setup][fem_condition_index].fem_map_index > 0)
                            {
                                fem_condition_entry[fem_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_ACTIVE;
                            }
                            else
                            {
                                fem_condition_entry[fem_setup][fem_condition_index].fem_action_valid =
                                    DBAL_ENUM_FVAL_FIELD_FEM_ACTION_VALID_FEM_ACTIONS_IS_NOT_ACTIVE;
                            }
                        }
                    }
                }
                for (fem_setup = 0; fem_setup < CTEST_DNX_FIELD_FEM_NUM_FEM_SETUPS_CASCADED; fem_setup++)
                {
                    if (fem_setup == 0)
                    {
                        fem_id = 6;
                        input_offset = 16;
                        input_size = 32;
                        fem_condition_ms_bit = 4;
                        base_fem_encoded_actions = DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0;
                    }
                    else
                    {
                        fem_id = 0;
                        input_offset = 16;
                        input_size = 32;
                        fem_condition_ms_bit = 6;
                        base_fem_encoded_actions = DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0;
                    }
                    /*
                     * 'FEM id's '0' and '1' have only 4 bits on action.
                     */
                    if (fem_id <
                        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                    {
                        fem_bit_index_max =
                            dnx_data_field.stage.stage_info_get(unit,
                                                                DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_action_fems_0_1;
                    }
                    /*
                     * Load 'map indices' with pointers to 'action's
                     */
                    for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
                    {
                        dnx_field_stage_e field_stage;

                        SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry[fem_map_index]));
                        /*
                         * Note that, for now, this code is not meaningful. We still need to pick specific actions, with specific
                         * sizes, and set the bits accordingly.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id[fg_index], &field_stage));
                        fem_encoded_actions[fem_map_index] =
                            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage,
                                       (base_fem_encoded_actions + fem_map_index));
                        for (fem_bit_index = 0; fem_bit_index < fem_bit_index_max; fem_bit_index++)
                        {
                            fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].fem_bit_format =
                                DNX_FIELD_FEM_BIT_FORMAT_FROM_KEY_SELECT;
                            fem_action_entry[fem_map_index].fem_bit_info[fem_bit_index].
                                fem_bit_value.bit_on_key_select = fem_bit_index;
                        }
                        if (fem_id <
                            dnx_data_field.stage.stage_info_get(unit,
                                                                DNX_FIELD_STAGE_IPMF1)->num_fems_with_short_action)
                        {
                            fem_action_entry[fem_map_index].fem_adder = 0;
                        }
                        else
                        {
                            fem_action_entry[fem_map_index].fem_adder = (dnx_field_fem_adder_t) (fem_map_index + 16);
                        }
                    }
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                                    (unit, fem_id, fg_id[fg_index], input_offset, input_size, fg_id[second_fg_index],
                                     fem_condition_ms_bit, &fem_condition_entry[fem_setup][0], fem_encoded_actions,
                                     fem_action_entry));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                }
            }
            {
                /*
                 * *** Step 'g' ***
                 * Attach contexts to 'owner' Field Group.
                 */
                dnx_field_group_attach_info_t attach_info[CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_OWNER_FG_ON_CASCADE];
                unsigned int attach_index;
                unsigned int fg_index;
                unsigned int qual_index, action_index;
                unsigned int num_quals;
                unsigned int num_actions;
                dnx_ctest_field_quals_for_fg_t *tcam_quals;

                /**
                 * Number of qualifiers for 'owner' Field Group on DNX level.
                 */
                num_quals = sizeof(owner_tcam_quals) / sizeof(owner_tcam_quals[0]);
                /**
                 * Number of actions for 'owner' Field Group on DNX level.
                 */
                num_actions = sizeof(owner_tcam_actions) / sizeof(owner_tcam_actions[0]);
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE;
                tcam_quals = owner_tcam_quals;
                attach_index = CTEST_DNX_FIELD_FEM_FIRST_CONTEXT_PER_OWNER_FG_ON_CASCADE;
                {
                    /*
                     * Configure attach API info
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info[attach_index]));
                    for (qual_index = 0; qual_index < num_quals; qual_index++)
                    {
                        attach_info[attach_index].dnx_quals[qual_index] = fg_info[fg_index].dnx_quals[qual_index];
                        attach_info[attach_index].qual_info[qual_index].input_type =
                            tcam_quals[qual_index].qual_attach_info.input_type;
                        attach_info[attach_index].qual_info[qual_index].input_arg =
                            tcam_quals[qual_index].qual_attach_info.input_arg;
                        attach_info[attach_index].qual_info[qual_index].offset =
                            tcam_quals[qual_index].qual_attach_info.offset;
                    }
                    for (action_index = 0; action_index < num_actions; action_index++)
                    {
                        attach_info[attach_index].dnx_actions[action_index] =
                            fg_info[fg_index].dnx_actions[action_index];
                        attach_info[attach_index].action_info[action_index].priority =
                            BCM_FIELD_ACTION_PRIORITY(2, (fg_id[fg_index] * 100) + action_index);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_group_context_attach
                                    (unit, 0, fg_id[fg_index], ctx_id_for_owner[attach_index],
                                     &attach_info[attach_index]));
                }
            }
            if (1)
            {
                /*
                 * *** Step 'h' ***
                 * Try to detach contexts of 'sibling' Field Group.
                 * Negative test: Should fail.
                 */
                unsigned int fg_index;
                unsigned int attach_index;

                /**
                 * Increase the severity to 'fatal' to avoid seing errors messages on the screen.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE;
                attach_index = 0;
                rv = dnx_field_group_context_detach(unit, fg_id[fg_index], ctx_id_for_secondary[attach_index]);
                if (rv != _SHR_E_NONE)
                {
                    LOG_INFO_EX(BSL_LOG_MODULE,
                                "%s(), line %d, NEGATIVE TEST for dnx_field_group_context_detach() (Detach secondary) has failed as expected! %s%s\r\n",
                                __FUNCTION__, __LINE__, EMPTY, EMPTY);
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "NEGATIVE TEST for dnx_field_group_context_detach() (Detach secondary) has succeeded. Test has failed!\r\n");
                }
                /**
                 *  Restore the original severity after the end of Negative test.
                 */
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
                SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
            }
            if (clean_resources)
            {
                unsigned int fg_index;
                unsigned int attach_index;
                int fem_setup;
                dnx_field_fem_id_t fem_id;

                /*
                 * *** Step 'i' ***
                 * Detach context of 'parent' Field Group.
                 */
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE;
                attach_index = CTEST_DNX_FIELD_FEM_FIRST_CONTEXT_PER_OWNER_FG_ON_CASCADE;
                SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, fg_id[fg_index], ctx_id_for_owner[attach_index]));
                /*
                 * *** Step 'j' ***
                 * Detach contexts of 'sibling' Field Group.
                 */
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_SECONDARY_FG_ON_CASCADE;
                for (attach_index = 0; attach_index < CTEST_DNX_FIELD_FEM_NUM_CONTEXTS_PER_SECONDARY_FG_ON_CASCADE;
                     attach_index++)
                {
                    if (attach_index == 0)
                    {
                        /*
                         * Complete the detach of the FEM context. Started on the 'negative' test above.
                         * Start the 'detach' loop below at index '1'.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_fems_context_detach
                                        (unit, fg_id[fg_index], ctx_id_for_secondary[attach_index]));
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_field_group_context_detach
                                        (unit, fg_id[fg_index], ctx_id_for_secondary[attach_index]));
                    }
                }
                fg_index = CTEST_DNX_FIELD_FEM_INDEX_OF_OWNER_FG_ON_CASCADE;
                for (fem_setup = 0; fem_setup < CTEST_DNX_FIELD_FEM_NUM_FEM_SETUPS_CASCADED; fem_setup++)
                {
                    if (fem_setup == 0)
                    {
                        fem_id = 6;
                    }
                    else
                    {
                        fem_id = 0;
                    }
                    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id, fg_id[fg_index]));
                    APPL_DNX_ACTION_SWSTATE_FEM_DISPLAY(unit, fem_id, "action_add", sand_control);
                }
            }
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal FEM test identifier (%d).\r\n", fem_test_type);
        }
    }
    /*
     * Make sure to clear any error which might have been raised due to a negative test.
     */
    SHR_SET_CURRENT_ERR(BCM_E_NONE);
exit:
    SHR_FREE(fem_condition_entry_p);
    SHR_FREE(fem_action_entry_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDTESTSDNX, original_severity_fldtestdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_swstate);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *    Performs operational testing for the code in the field actions module.
 *    Tests:
 *    * dnx_field_actions_fes_set()
 *    * dnx_field_actions_fes_detach()
 *    * dnx_field_actions_context_fes_info_get()
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which the database is to be created and tested (IPMF1, IPMF2, IPMF3, EPMF).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
static shr_error_e
appl_dnx_action_tester_fes_cfg(
    int unit,
    dnx_field_stage_e field_stage)
{
    dnx_field_stage_e field_stage_for_context = field_stage;
    dnx_field_group_t fg_id[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG];
    dnx_field_group_t fg_id_extra;
    unsigned int fg_id_ndx;
    dnx_field_context_t context_id;
    dnx_field_context_t context_id_second;
    int is_dir_ext[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG];
    unsigned int fes2msb_ndx;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    dnx_field_actions_fes_context_group_get_info_t group_fes_get_info;
    dnx_field_action_type_t action_type[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    dnx_field_fes_key_select_t key_select[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    unsigned int valid_bits[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    unsigned int shift[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    unsigned int type[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    unsigned int polarity[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    unsigned int required_mask[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    dnx_field_context_mode_t context_mode;
    unsigned int action_ndx;
    unsigned int fes_quartet_ndx;
    unsigned int place_in_fg;
    int found_action[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG][CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    dnx_field_fes_id_t
        allocated_fes_id[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG][CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    dnx_field_action_priority_t
        priority[CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG][CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS];
    dnx_field_action_priority_t priority_extra;
    unsigned int nof_instr_per_fes_quartet;
    unsigned int nof_fes_id;
    unsigned int nof_fg_prev_attached;
    dnx_field_fes_id_t fes_id_found;
    dnx_field_fes_pgm_id_t fes_pgm_id_found;
    dnx_field_action_type_t action_type_extra;
    unsigned int nof_fes_array;
    unsigned int nof_fes_id_per_array;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialization to appease the compiler.
     */
    fes_pgm_id_found = 0;

    /*
     * Create the contexts.
     */
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        field_stage_for_context = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        field_stage_for_context = field_stage;
    }
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, field_stage_for_context, &context_mode, &context_id));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, field_stage_for_context, &context_mode, &context_id_second));

    /*
     * Verify that the contexts are empty.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    nof_fg_prev_attached = context_fes_get_info.nof_fes_quartets;
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "New context ID %d in stage %s isn't empty.\r\n",
                     context_id, dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, field_stage, context_id_second, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "New context ID %d in stage %s isn't empty.\r\n",
                     context_id_second, dnx_field_stage_text(unit, field_stage));
    }

    /*
     * Select the actions and key_selects according to the stage.
     */
    nof_instr_per_fes_quartet = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
    nof_fes_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
    nof_fes_id_per_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array;
    nof_fes_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array;
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            action_type[0] = DBAL_ENUM_FVAL_IPMF1_ACTION_DP;
            action_type[1] = DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA;
            action_type_extra = DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING;
            key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0;
            key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            action_type[0] = DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET;
            action_type[1] = DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE;
            action_type_extra = DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3;
            key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0;
            key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            action_type[0] = DBAL_ENUM_FVAL_EPMF_ACTION_TC;
            action_type[1] = DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH;
            action_type_extra = DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE;
            key_select[0] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_LEXEM_4_B0_59_0;
            key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_KEY_A_31_0_SEXEM_59_28;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d).\r\n", field_stage);
        }
    }

    /*
     * Configure parameter, including three field groups with the same two actions.
     * Note that the field group
     */
    fg_id[0] = CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID + 0;
    fg_id[1] = CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID + 1;
    fg_id[2] = CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID + 2;
    fg_id_extra = CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID + 3;
    is_dir_ext[0] = FALSE;
    is_dir_ext[1] = FALSE;
    is_dir_ext[2] = TRUE;
    valid_bits[0] = 1;
    valid_bits[1] = 5;
    shift[0] = 2;
    shift[1] = 46;
    type[0] = 0;
    type[1] = 0;
    polarity[0] = 1;
    polarity[1] = 1;
    required_mask[0] = 3;
    required_mask[1] = 0;
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));
    fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type[0];
    fes_inst_info[0].common_info.fes2msb_info[0].valid_bits = valid_bits[0];
    fes_inst_info[0].common_info.fes2msb_info[0].shift = shift[0];
    fes_inst_info[0].common_info.fes2msb_info[0].type = type[0];
    fes_inst_info[0].common_info.fes2msb_info[0].polarity = polarity[0];
    fes_inst_info[0].alloc_info.fes2msb_info[0].required_mask = required_mask[0];
    fes_inst_info[0].write_info.key_select = key_select[0];
    fes_inst_info[1].common_info.fes2msb_info[0].action_type = action_type[1];
    fes_inst_info[1].common_info.fes2msb_info[0].valid_bits = valid_bits[1];
    fes_inst_info[1].common_info.fes2msb_info[0].shift = shift[1];
    fes_inst_info[1].common_info.fes2msb_info[0].type = type[1];
    fes_inst_info[1].common_info.fes2msb_info[0].polarity = polarity[1];
    fes_inst_info[1].alloc_info.fes2msb_info[0].required_mask = required_mask[1];
    fes_inst_info[1].write_info.key_select = key_select[1];
    /*
     * Copy the same configuration for all (4) FES2msb instructions.
     */
    for (fes2msb_ndx = 1; fes2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_ndx++)
    {
        fes_inst_info[0].common_info.fes2msb_info[fes2msb_ndx] = fes_inst_info[0].common_info.fes2msb_info[0];
        fes_inst_info[0].alloc_info.fes2msb_info[fes2msb_ndx] = fes_inst_info[0].alloc_info.fes2msb_info[0];
        fes_inst_info[1].common_info.fes2msb_info[fes2msb_ndx] = fes_inst_info[1].common_info.fes2msb_info[0];
        fes_inst_info[1].alloc_info.fes2msb_info[fes2msb_ndx] = fes_inst_info[1].alloc_info.fes2msb_info[0];
    }
    /*
     * Set priorities differently between field group, first action middle field group is lowest, second highest.
     */
    priority[0][0] = BCM_FIELD_ACTION_PRIORITY(0, 20);
    priority[0][1] = BCM_FIELD_ACTION_PRIORITY(0, 20);
    priority[1][0] = BCM_FIELD_ACTION_PRIORITY(0, 5);
    priority[1][1] = BCM_FIELD_ACTION_PRIORITY(0, 50);
    priority[2][0] = BCM_FIELD_ACTION_PRIORITY(0, 10);
    priority[2][1] = BCM_FIELD_ACTION_PRIORITY(0, 10);
    priority_extra = BCM_FIELD_ACTION_PRIORITY(0, 60);

    /*
     * Write to FESes.
     */
    fes_inst_info[0].alloc_info.priority = priority[0][0];
    fes_inst_info[1].alloc_info.priority = priority[0][1];
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[0], context_id, is_dir_ext[0], FALSE, fes_inst_info));
    fes_inst_info[0].alloc_info.priority = priority[1][0];
    fes_inst_info[1].alloc_info.priority = priority[1][1];
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[1], context_id, is_dir_ext[1], FALSE, fes_inst_info));
    fes_inst_info[0].alloc_info.priority = priority[2][0];
    fes_inst_info[1].alloc_info.priority = priority[2][1];
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[2], context_id, is_dir_ext[2], FALSE, fes_inst_info));

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets !=
        ((CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG * CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS) + nof_fg_prev_attached))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Found %d FES quartets in context ID %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets,
                     context_id,
                     (CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG * CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS) +
                     nof_fg_prev_attached);
    }
    /** Initialize found_action*/
    for (fg_id_ndx = 0; fg_id_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG; fg_id_ndx++)
    {
        for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS; action_ndx++)
        {
            found_action[fg_id_ndx][action_ndx] = FALSE;
        }
    }
    /** Verify each action*/
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        place_in_fg = context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg;
        fg_id_ndx =
            context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id - CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID;
        if (place_in_fg >= CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                         "In FES ID %d FES program ID %d found FES quartet belonging to "
                         "field group %d context ID %d with place in field group too large %d, "
                         "when we have only %d FES quartets per field group.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS);
        }
        if ((unsigned int) fg_id_ndx >= CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                         "In FES ID %d FES program ID %d found FES quartet belonging to "
                         "context ID %d with unknown field group %d, range of "
                         "field group IDs used by test %d to %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id,
                         context_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID,
                         CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID + CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG - 1);
        }
        /** Verify the FES configuration.*/
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != priority[fg_id_ndx][place_in_fg] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].key_select != key_select[place_in_fg] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type != action_type[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits != valid_bits[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift != shift[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type != type[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity != polarity[place_in_fg])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                         "Mismatch found in FES ID %d FES program ID %d, context ID %d, "
                         "field group ID %d, FES quartet %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id,
                         context_id, context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id, place_in_fg);
        }
        /*
         * Verify that the result are the same for all FES2msb instructions.
         */
        for (fes2msb_ndx = 1; fes2msb_ndx < nof_instr_per_fes_quartet; fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After first run, the FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
        /*
         * Update found_action.
         */
        found_action[fg_id_ndx][place_in_fg] = TRUE;
        allocated_fes_id[fg_id_ndx][place_in_fg] = context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id;
    }
    /** Verify that all actions were found.*/
    for (fg_id_ndx = 0; fg_id_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG; fg_id_ndx++)
    {
        for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS; action_ndx++)
        {
            if (found_action[fg_id_ndx][action_ndx] == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After first run, the FES configuration was not as expected, "
                             "Field group ID %d action %d was not written.\r\n", fg_id[fg_id_ndx], action_ndx);
            }
        }
    }
    /** Verify that the actions were allocated by priority.*/
    if (allocated_fes_id[0][0] <= allocated_fes_id[1][0] || allocated_fes_id[1][0] >= allocated_fes_id[2][0] ||
        allocated_fes_id[0][0] <= allocated_fes_id[2][0] ||
        allocated_fes_id[0][1] >= allocated_fes_id[1][1] || allocated_fes_id[1][1] <= allocated_fes_id[2][1] ||
        allocated_fes_id[0][1] <= allocated_fes_id[2][1])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES quartets given FES IDs out of order of their priorities.\r\n");
    }

    /*
     * Test attach info for dnx_field_actions_context_fes_info_to_group_fes_info(). 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id[0], &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != priority[0][0] ||
        group_fes_get_info.initial_fes_quartets[1].priority != priority[0][1])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d.\r\n", fg_id[0]);
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id[1], &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != priority[1][0] ||
        group_fes_get_info.initial_fes_quartets[1].priority != priority[1][1])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d.\r\n", fg_id[1]);
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id[2], &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != priority[2][0] ||
        group_fes_get_info.initial_fes_quartets[1].priority != priority[2][1])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d.\r\n", fg_id[2]);
    }
    
    if (group_fes_get_info.initial_fes_quartets[1].priority != priority[2][1]
        || group_fes_get_info.initial_fes_quartets[1].key_select != key_select[1]
        || group_fes_get_info.initial_fes_quartets[1].fes2msb_info[0].action_type != action_type[1]
        || group_fes_get_info.initial_fes_quartets[1].fes2msb_info[0].valid_bits != valid_bits[1]
        || group_fes_get_info.initial_fes_quartets[1].fes2msb_info[0].shift != shift[1]
        || group_fes_get_info.initial_fes_quartets[1].fes2msb_info[0].type != type[1]
        || group_fes_get_info.initial_fes_quartets[1].fes2msb_info[0].polarity != polarity[1])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected FES configuration for per group get function.\r\n");
    }

    /*
     * Delete FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[0], context_id, DNX_FIELD_EFES_ID_INVALID));
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[1], context_id, DNX_FIELD_EFES_ID_INVALID));
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[2], context_id, DNX_FIELD_EFES_ID_INVALID));
    /*
     * Check that the configuration was deleted.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != nof_fg_prev_attached)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }

    /*
     * Rewrtie to FESes after delete.
     */
    fes_inst_info[0].alloc_info.priority = priority[0][0];
    fes_inst_info[1].alloc_info.priority = priority[0][1];
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[0], context_id, is_dir_ext[0], FALSE, fes_inst_info));
    fes_inst_info[0].alloc_info.priority = priority[1][0];
    fes_inst_info[1].alloc_info.priority = priority[1][1];
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[1], context_id, is_dir_ext[1], FALSE, fes_inst_info));
    fes_inst_info[0].alloc_info.priority = priority[2][0];
    fes_inst_info[1].alloc_info.priority = priority[2][1];
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[2], context_id, is_dir_ext[2], FALSE, fes_inst_info));

    /*
     * Get FES configuration and check that it is unchanged.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    /*
     * Check that the results are the same after rewrite.
     */
    if (context_fes_get_info.nof_fes_quartets !=
        ((CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG * CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS) + nof_fg_prev_attached))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Found %d FES quartets in context ID %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets,
                     context_id,
                     (CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG * CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS) +
                     nof_fg_prev_attached);
    }
    /** Initialize found_action*/
    for (fg_id_ndx = 0; fg_id_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG; fg_id_ndx++)
    {
        for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS; action_ndx++)
        {
            found_action[fg_id_ndx][action_ndx] = FALSE;
        }
    }
    /** Verify each action*/
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        place_in_fg = context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg;
        fg_id_ndx =
            context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id - CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID;
        if (place_in_fg >= CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After second run, the FES configuration was not as expected. "
                         "In FES ID %d FES program ID %d found FES quartet belonging to "
                         "field group %d context ID %d with place in field group too large %d, "
                         "when we have only %d FES quartets per field group.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS);
        }
        if ((unsigned int) fg_id_ndx >= CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After second run, the FES configuration was not as expected. "
                         "In FES ID %d FES program ID %d found FES quartet belonging to "
                         "context ID %d with unknown field group %d, range of "
                         "field group IDs used by test %d to %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id,
                         context_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID,
                         CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID + CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG - 1);
        }
        /** Verify the FES configuration.*/
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != priority[fg_id_ndx][place_in_fg] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].key_select != key_select[place_in_fg] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type != action_type[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits != valid_bits[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift != shift[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type != type[place_in_fg]
            || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity != polarity[place_in_fg])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After second run, the FES configuration was not as expected. "
                         "Mismatch found in FES ID %d FES program ID %d, context ID %d, "
                         "field group ID %d, FES quartet %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id,
                         context_id, context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id, place_in_fg);
        }
        /*
         * Verify that the result are the same for all FES2msb instructions.
         */
        for (fes2msb_ndx = 1; fes2msb_ndx < nof_instr_per_fes_quartet; fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After second run, the FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
        /*
         * Update found_action.
         */
        found_action[fg_id_ndx][place_in_fg] = TRUE;
        allocated_fes_id[fg_id_ndx][place_in_fg] = context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id;
    }
    /** Verify that all actions were found.*/
    for (fg_id_ndx = 0; fg_id_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG; fg_id_ndx++)
    {
        for (action_ndx = 0; action_ndx < CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS; action_ndx++)
        {
            if (found_action[fg_id_ndx][action_ndx] == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After second run, the FES configuration was not as expected, "
                             "Field group ID %d action %d was not written.\r\n", fg_id[fg_id_ndx], action_ndx);
            }
        }
    }
    /** Verify that the actions were allocated by priority.*/
    if (allocated_fes_id[0][0] <= allocated_fes_id[1][0] || allocated_fes_id[1][0] >= allocated_fes_id[2][0] ||
        allocated_fes_id[0][0] <= allocated_fes_id[2][0] ||
        allocated_fes_id[0][1] >= allocated_fes_id[1][1] || allocated_fes_id[1][1] <= allocated_fes_id[2][1] ||
        allocated_fes_id[0][1] <= allocated_fes_id[2][1])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES quartets given FES IDs out of order of their priorities.\r\n");
    }

    /*
     * Attach the same field group to a different context to verify sharing.
     */
    fes_inst_info[0].alloc_info.priority = priority_extra;
    fes_inst_info[1].alloc_info.priority = priority_extra;
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[0], context_id_second, is_dir_ext[0], FALSE, fes_inst_info));
    /** Find the location of the FES quartet in the first context.*/
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    fes_id_found = nof_fes_id;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id >= nof_fes_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action with FES ID %d, maximum %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id, nof_fes_id - 1);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id[0] &&
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 0)
        {
            fes_id_found = context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id;
            fes_pgm_id_found = context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id;
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != priority[0][0])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After sharing action priority changed from 0x%x to 0x%x.\r\n",
                             priority[0][0], context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
            }
            break;
        }
    }
    if (fes_id_found >= nof_fes_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "First action in first field group not found attached to context, "
                     "after previously found to be attached.\r\n");
    }
    /** check if the location of the FES quartet in the second context is the same as the first.*/
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, field_stage, context_id_second, &context_fes_get_info));
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id >= nof_fes_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Action with FES ID %d, maximum %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id, nof_fes_id - 1);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id[0] &&
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 0)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != fes_id_found ||
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id != fes_pgm_id_found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "First action in field group %d should be shared between contexts %d "
                             "(FES ID %d FES program ID %d) and %d (FES ID %d FES program ID %d), but it is not.\r\n",
                             fg_id[0], context_id, fes_id_found, fes_pgm_id_found, context_id_second,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id);
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != fes_inst_info[0].alloc_info.priority)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After sharing action priority changed from 0x%x to 0x%x.\r\n",
                             priority_extra, context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
            }
            break;
        }
    }
    /*
     * Attach another field group in the same location with position mandating priority and verify that the 
     * context is moved. 
     * Note we assume FES_id==FES_position_in_array because it is te first array.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));
    fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type_extra;
    fes_inst_info[0].alloc_info.priority = BCM_FIELD_ACTION_POSITION(0, fes_id_found);
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id_extra, context_id, is_dir_ext[0], FALSE, fes_inst_info));
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id[0] &&
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 0)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id == fes_id_found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Action with mandatory position did not cause existing action to move.\r\n");
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != priority[0][0])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After moving action priority changed from 0x%x to 0x%x.\r\n",
                             priority[0][0], context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
            }
            break;
        }
    }
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id_extra &&
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 0)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != fes_id_found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Action with mandatory position did not get placed in its requested position.\r\n");
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != fes_inst_info[0].alloc_info.priority)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After replacing another action action priority changed from 0x%x to 0x%x.\r\n",
                             fes_inst_info[0].alloc_info.priority,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
            }
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, field_stage, context_id_second, &context_fes_get_info));
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id[0] &&
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 0)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != fes_id_found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Action with mandatory position moved action in a different context.\r\n");
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != priority_extra)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After shared action was moved priority changed from 0x%x to 0x%x.\r\n",
                             priority_extra, context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
            }
            break;
        }
    }

    /*
     * Delete FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[0], context_id, DNX_FIELD_EFES_ID_INVALID));
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[1], context_id, DNX_FIELD_EFES_ID_INVALID));
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[2], context_id, DNX_FIELD_EFES_ID_INVALID));
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach
                    (unit, field_stage, fg_id_extra, context_id, DNX_FIELD_EFES_ID_INVALID));
    /*
     * Check that the shared action (one was moved, but one should still be shared) wan't deleted before we delete it.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, field_stage, context_id_second, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != nof_fg_prev_attached + CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shared FES quartet deleted when detached from another context.\r\n");
    }
    /*
     * Check that the configuration was deleted.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != nof_fg_prev_attached)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[0], context_id_second,
                                                 DNX_FIELD_EFES_ID_INVALID));
    /*
     * Check that the configuration was deleted.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get
                    (unit, field_stage, context_id_second, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != nof_fg_prev_attached)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }

    /*
     * If we are in PMF1/2, write to the second array.
     */
    if (nof_fes_array > 1)
    {
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));
        fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type[0];
        fes_inst_info[0].alloc_info.priority = BCM_FIELD_ACTION_PRIORITY(2, 70);
        fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type[0];
        fes_inst_info[0].alloc_info.priority = BCM_FIELD_ACTION_POSITION(2, 0);
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                        (unit, field_stage, fg_id[0], context_id, is_dir_ext[0], FALSE, fes_inst_info));
        SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
        for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id[0] &&
                context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 0)
            {
                if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id < nof_fes_id_per_array)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Action written to FES ID %d in stage %s, when priority 0x%x is meant to "
                                 "write to the second array.\r\n",
                                 context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                                 dnx_field_stage_text(unit, field_stage), fes_inst_info[0].alloc_info.priority);
                }
                if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != fes_inst_info[0].alloc_info.priority)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Priority changed from 0x%x to 0x%x.\r\n",
                                 fes_inst_info[0].alloc_info.priority,
                                 context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
                }
                break;
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id == fg_id[0] &&
                context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg == 1)
            {
                if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != nof_fes_id_per_array)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Action written to FES ID %d in stage %s, when priority 0x%x is meant to "
                                 "mandate writing to the first FES in the second array (FES ID %d).\r\n",
                                 context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                                 dnx_field_stage_text(unit, field_stage), fes_inst_info[0].alloc_info.priority,
                                 nof_fes_id_per_array);
                }
                if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != fes_inst_info[0].alloc_info.priority)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Priority changed from 0x%x to 0x%x.\r\n",
                                 fes_inst_info[0].alloc_info.priority,
                                 context_fes_get_info.fes_quartets[fes_quartet_ndx].priority);
                }
                break;
            }
        }
    }
    /*
     * Delete FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[0], context_id, DNX_FIELD_EFES_ID_INVALID));
    /*
     * Check that the configuration was deleted.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != nof_fg_prev_attached)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }

    /*
     * Configure a field group with priorities out of order.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));
    fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type[0];
    fes_inst_info[0].common_info.fes2msb_info[0].valid_bits = valid_bits[0];
    fes_inst_info[0].common_info.fes2msb_info[0].shift = shift[0];
    fes_inst_info[0].common_info.fes2msb_info[0].type = type[0];
    fes_inst_info[0].common_info.fes2msb_info[0].polarity = polarity[0];
    fes_inst_info[0].alloc_info.fes2msb_info[0].required_mask = 0;
    fes_inst_info[0].write_info.key_select = key_select[0];
    /*
     * Copy the same configuration for all (4) FES2msb instructions, for three separate fes quartets
     */
    for (fes2msb_ndx = 0; fes2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_ndx++)
    {
        for (fes_quartet_ndx = 0; fes_quartet_ndx < 3; fes_quartet_ndx++)
        {
            fes_inst_info[fes_quartet_ndx].common_info.fes2msb_info[fes2msb_ndx] =
                fes_inst_info[0].common_info.fes2msb_info[0];
            fes_inst_info[fes_quartet_ndx].alloc_info.fes2msb_info[fes2msb_ndx] =
                fes_inst_info[0].alloc_info.fes2msb_info[0];
        }
        fes_inst_info[fes_quartet_ndx].write_info.key_select = fes_inst_info[0].write_info.key_select;
    }
    /*
     * Set different masks, valid_bits and priorities
     */
    fes_inst_info[0].alloc_info.fes2msb_info[0].required_mask = 1;
    fes_inst_info[1].alloc_info.fes2msb_info[0].required_mask = 2;
    fes_inst_info[2].alloc_info.fes2msb_info[0].required_mask = 3;
    fes_inst_info[0].common_info.fes2msb_info[0].valid_bits = 1;
    fes_inst_info[1].common_info.fes2msb_info[0].valid_bits = 2;
    fes_inst_info[2].common_info.fes2msb_info[0].valid_bits = 3;
    fes_inst_info[0].alloc_info.priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    fes_inst_info[1].alloc_info.priority = BCM_FIELD_ACTION_PRIORITY(0, 30);
    fes_inst_info[2].alloc_info.priority = BCM_FIELD_ACTION_PRIORITY(0, 25);
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                    (unit, field_stage, fg_id[0], context_id, is_dir_ext[0], FALSE, fes_inst_info));

    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 3 ||
        context_fes_get_info.fes_quartets[0].fes2msb_info[0].valid_bits != 1 ||
        context_fes_get_info.fes_quartets[1].fes2msb_info[0].valid_bits != 3 ||
        context_fes_get_info.fes_quartets[2].fes2msb_info[0].valid_bits != 2 ||
        context_fes_get_info.fes_quartets[0].fes2msb_info[0].mask != 1 ||
        context_fes_get_info.fes_quartets[1].fes2msb_info[0].mask != 3 ||
        context_fes_get_info.fes_quartets[2].fes2msb_info[0].mask != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Order swap withing field group unsuccessful.\r\n");
    }

    /*
     * Delete FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id[0], context_id, DNX_FIELD_EFES_ID_INVALID));
    /*
     * Check that the configuration was deleted.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != nof_fg_prev_attached)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }

    /*
     * Destroy the context.
     */
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, field_stage_for_context, context_id));
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, field_stage_for_context, context_id_second));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the code in the field actions module, 
 *    including moving more than one action at a time.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 *   * appl_dnx_action_tester_fes_cfg
 */
static shr_error_e
appl_dnx_action_tester_fes_cfg_mul(
    int unit)
{
    unsigned int fg_id_ndx;
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_ndx;
    unsigned int initial_fes_quartet_within_array_ndx;
    unsigned int nof_fes_id_per_array;
    unsigned int array_id;
    unsigned int past_invalidate_next;
    unsigned int nof_initial_fgs = 8;
    unsigned int nof_invalidate_next_fgs = 2;
    /*
     * BCM variables.
     */
    bcm_field_stage_t bcm_stage = bcmFieldStageIngressPMF2;

    bcm_field_group_t *bcm_fg_id_1_p = NULL;
    bcm_field_group_t *bcm_fg_id_2_p = NULL;

    bcm_field_group_info_t bcm_fg_info_1;
    bcm_field_group_info_t bcm_fg_info_2;
    bcm_field_group_attach_info_t bcm_attach_info_1;
    bcm_field_group_attach_info_t bcm_attach_info_2;
    bcm_field_context_info_t bcm_context_info;
    bcm_field_context_t bcm_context_id;
    /*
     * DNX variables.
     */
    dnx_field_stage_e dnx_stage;

    dnx_field_group_t *dnx_fg_id_1_p = NULL;
    dnx_field_group_t *dnx_fg_id_2_p = NULL;

    dnx_field_context_t dnx_context_id;
    dnx_field_action_t dnx_action_initial[2];
    dnx_field_action_t dnx_action_ivalidate_next;
    dnx_field_action_type_t action_type_initial[2];
    dnx_field_action_type_t action_type_ivalidate_next;

    dnx_field_actions_fes_context_get_info_t context_fes_get_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bcm_fg_id_1_p, sizeof(*bcm_fg_id_1_p) * nof_initial_fgs, "bcm_fg_id_1_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(bcm_fg_id_2_p, sizeof(*bcm_fg_id_2_p) * nof_invalidate_next_fgs,
              "bcm_fg_id_2_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(dnx_fg_id_1_p, sizeof(*dnx_fg_id_1_p) * nof_initial_fgs, "dnx_fg_id_1_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(dnx_fg_id_2_p, sizeof(*dnx_fg_id_2_p) * nof_initial_fgs, "dnx_fg_id_2_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Configure multiple actions.
     */
    bcm_field_context_info_t_init(&bcm_context_info);
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &bcm_context_info, &bcm_context_id));

    bcm_field_group_info_t_init(&bcm_fg_info_1);
    bcm_fg_info_1.stage = bcm_stage;
    bcm_fg_info_1.fg_type = bcmFieldGroupTypeDirectExtraction;
    bcm_fg_info_1.nof_quals = 2;
    bcm_fg_info_1.qual_types[0] = bcmFieldQualifyIntPriority;
    bcm_fg_info_1.qual_types[1] = bcmFieldQualifyColor;
    bcm_fg_info_1.nof_actions = 2;
    bcm_fg_info_1.action_types[0] = bcmFieldActionPrioIntNew;
    bcm_fg_info_1.action_types[1] = bcmFieldActionDropPrecedence;
    bcm_fg_info_1.action_with_valid_bit[0] = FALSE;
    bcm_fg_info_1.action_with_valid_bit[1] = FALSE;

    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_add
                        (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &bcm_fg_info_1, &bcm_fg_id_1_p[fg_id_ndx]));
    }

    bcm_field_group_attach_info_t_init(&bcm_attach_info_1);
    bcm_attach_info_1.key_info.nof_quals = bcm_fg_info_1.nof_quals;
    sal_memcpy(bcm_attach_info_1.key_info.qual_types, bcm_fg_info_1.qual_types,
               sizeof(bcm_attach_info_1.key_info.qual_types));
    bcm_attach_info_1.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_attach_info_1.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    bcm_attach_info_1.payload_info.nof_actions = bcm_fg_info_1.nof_actions;
    sal_memcpy(bcm_attach_info_1.payload_info.action_types, bcm_fg_info_1.action_types,
               sizeof(bcm_attach_info_1.payload_info.action_types));

    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs; fg_id_ndx++)
    {
        bcm_attach_info_1.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, fg_id_ndx);
        bcm_attach_info_1.payload_info.action_info[1].priority = BCM_FIELD_ACTION_PRIORITY(2, fg_id_ndx);
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                        (unit, 0, bcm_fg_id_1_p[fg_id_ndx], bcm_context_id, &bcm_attach_info_1));
    }

    /*
     * Configure invalidate next with a fixed position to push the actions away.
     */
    bcm_field_group_info_t_init(&bcm_fg_info_2);
    bcm_fg_info_2.stage = bcm_stage;
    bcm_fg_info_2.fg_type = bcmFieldGroupTypeDirectExtraction;
    bcm_fg_info_2.nof_quals = 1;
    bcm_fg_info_2.qual_types[0] = bcmFieldQualifyIpMulticastCompatible;
    bcm_fg_info_2.nof_actions = 1;
    bcm_fg_info_2.action_types[0] = bcmFieldActionInvalidNext;
    bcm_fg_info_2.action_with_valid_bit[0] = FALSE;
    bcm_fg_info_2.action_with_valid_bit[1] = FALSE;

    for (fg_id_ndx = 0; fg_id_ndx < nof_invalidate_next_fgs; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_add
                        (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &bcm_fg_info_2, &bcm_fg_id_2_p[fg_id_ndx]));
    }

    bcm_field_group_attach_info_t_init(&bcm_attach_info_2);
    bcm_attach_info_2.key_info.nof_quals = bcm_fg_info_2.nof_quals;
    sal_memcpy(bcm_attach_info_2.key_info.qual_types, bcm_fg_info_2.qual_types,
               sizeof(bcm_attach_info_2.key_info.qual_types));
    bcm_attach_info_2.payload_info.nof_actions = bcm_fg_info_2.nof_actions;
    bcm_attach_info_2.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    sal_memcpy(bcm_attach_info_2.payload_info.action_types, bcm_fg_info_2.action_types,
               sizeof(bcm_attach_info_2.payload_info.action_types));

    for (fg_id_ndx = 0; fg_id_ndx < nof_invalidate_next_fgs; fg_id_ndx++)
    {
        bcm_attach_info_2.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(2 * fg_id_ndx, 2);
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                        (unit, 0, bcm_fg_id_2_p[fg_id_ndx], bcm_context_id, &bcm_attach_info_2));
    }

    /*
     * Convert BCM variables to DNX variable.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));
    dnx_context_id = bcm_context_id;
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs; fg_id_ndx++)
    {
        dnx_fg_id_1_p[fg_id_ndx] = bcm_fg_id_1_p[fg_id_ndx];
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_invalidate_next_fgs; fg_id_ndx++)
    {
        dnx_fg_id_2_p[fg_id_ndx] = bcm_fg_id_2_p[fg_id_ndx];
    }
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                    (unit, dnx_stage, bcm_fg_info_1.action_types[0], &dnx_action_initial[0]));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, dnx_stage, dnx_action_initial[0], &action_type_initial[0]));
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                    (unit, dnx_stage, bcm_fg_info_1.action_types[1], &dnx_action_initial[1]));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, dnx_stage, dnx_action_initial[1], &action_type_initial[1]));
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                    (unit, dnx_stage, bcm_fg_info_2.action_types[0], &dnx_action_ivalidate_next));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type
                    (unit, dnx_stage, dnx_action_ivalidate_next, &action_type_ivalidate_next));

    nof_fes_id_per_array = dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_fes_id_per_array;

    if (2 != dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_fes_array)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test assumes number of arrays in iPMF1 is 2.\r\n");
    }

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, dnx_stage, dnx_context_id, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets !=
        ((nof_initial_fgs * bcm_fg_info_1.nof_actions) + (nof_invalidate_next_fgs * bcm_fg_info_2.nof_actions)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The FES configuration was not as expected. "
                     "Found %d FES quartets in context ID %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets,
                     dnx_context_id,
                     ((nof_initial_fgs * bcm_fg_info_1.nof_actions) +
                      (nof_invalidate_next_fgs * bcm_fg_info_2.nof_actions)));
    }
    /*
     * Verify that the FES_IDs are ordered.
     */
    for (fes_quartet_ndx = 1; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id <=
            context_fes_get_info.fes_quartets[fes_quartet_ndx - 1].fes_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES quartets out of order.\r\n");
        }
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }

    /*
     * Verify the actions that are not invalidate next.
     */
    fes_quartet_ndx = 0;
    for (array_id = 0; array_id < dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_fes_array; array_id++)
    {
        initial_fes_quartet_within_array_ndx = 0;
        past_invalidate_next = 0;
        for (; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets / 2; fes_quartet_ndx++)
        {
            unsigned int fes_id_within_array =
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id % nof_fes_id_per_array;
            if (fes_id_within_array == 2)
            {
                past_invalidate_next = 2;
                continue;
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id !=
                (initial_fes_quartet_within_array_ndx + (array_id * nof_fes_id_per_array) + past_invalidate_next))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d "
                             "in FES quartet %d.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx);
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority !=
                BCM_FIELD_ACTION_PRIORITY(array_id * 2, initial_fes_quartet_within_array_ndx))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d "
                             "in FES quartet %d has priority %x instead of %x.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].priority,
                             BCM_FIELD_ACTION_PRIORITY(array_id * 2, initial_fes_quartet_within_array_ndx));
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id !=
                dnx_fg_id_1_p[initial_fes_quartet_within_array_ndx] ||
                context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg != array_id)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d "
                             "in FES quartet %d has fg_id and place_in_fg %d and %d instead of %d and %d.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             dnx_fg_id_1_p[initial_fes_quartet_within_array_ndx], array_id);
            }
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                action_type_initial[array_id])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d "
                             "in FES quartet %d has action type %d instead of %d.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type,
                             action_type_initial[array_id]);
            }
            initial_fes_quartet_within_array_ndx++;
        }
    }

    /*
     * Verify the invalidate next actions.
     */
    for (array_id = 0; array_id < dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_fes_array; array_id++)
    {
        fes_quartet_ndx = 2 + (array_id * (context_fes_get_info.nof_fes_quartets / 2));
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != (2 + (array_id * nof_fes_id_per_array)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority != BCM_FIELD_ACTION_POSITION(array_id * 2, 2))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has priority %x instead of %x.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].priority,
                         BCM_FIELD_ACTION_POSITION(array_id * 2, 2));
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
            action_type_ivalidate_next)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has action type %d instead of %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type,
                         action_type_ivalidate_next);
        }
    }

    /*
     * Delete the configuration.
     */
    for (fg_id_ndx = 0; fg_id_ndx < nof_invalidate_next_fgs; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, bcm_fg_id_2_p[fg_id_ndx], bcm_context_id));
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, bcm_fg_id_2_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, bcm_fg_id_1_p[fg_id_ndx], bcm_context_id));
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, bcm_fg_id_1_p[fg_id_ndx]));
    }
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_id));

exit:
    SHR_FREE(bcm_fg_id_1_p);
    SHR_FREE(bcm_fg_id_2_p);
    SHR_FREE(dnx_fg_id_1_p);
    SHR_FREE(dnx_fg_id_2_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the code in the field actions module, 
 *    including moving more than one action at a time, to a lower EFES ID number
 * \param [in] unit -
 *    Identifier of HW platform.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 *   * appl_dnx_action_tester_fes_cfg
 */
static shr_error_e
appl_dnx_action_tester_fes_cfg_mul_back(
    int unit)
{
    unsigned int fg_id_ndx;
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_ndx;
    unsigned int nof_fes_id_per_array;
    unsigned int nof_initial_fgs_1 = 3;
    unsigned int nof_initial_fgs_2 = 12;
    unsigned int nof_initial_fgs_3 = 2;
    unsigned int fg_counter;
    /*
     * BCM variables.
     */
    bcm_field_stage_t bcm_stage = bcmFieldStageIngressPMF2;

    bcm_field_group_t *bcm_fg_id_1_p = NULL;
    bcm_field_group_t *bcm_fg_id_2_p = NULL;
    bcm_field_group_t *bcm_fg_id_3_p = NULL;

    bcm_field_group_info_t bcm_fg_info;
    bcm_field_group_attach_info_t bcm_attach_info;
    bcm_field_context_info_t bcm_context_info;
    bcm_field_context_t bcm_context_id;
    /*
     * DNX variables.
     */
    dnx_field_stage_e dnx_stage;

    dnx_field_group_t *dnx_fg_id_1_p = NULL;
    dnx_field_group_t *dnx_fg_id_2_p = NULL;
    dnx_field_group_t *dnx_fg_id_3_p = NULL;

    dnx_field_context_t dnx_context_id;
    dnx_field_action_t dnx_action_initial;
    dnx_field_action_type_t action_type_initial;

    dnx_field_actions_fes_context_get_info_t context_fes_get_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(bcm_fg_id_1_p, sizeof(*bcm_fg_id_1_p) * nof_initial_fgs_1,
              "bcm_fg_id_1_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(bcm_fg_id_2_p, sizeof(*bcm_fg_id_2_p) * nof_initial_fgs_2,
              "bcm_fg_id_2_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(bcm_fg_id_3_p, sizeof(*bcm_fg_id_3_p) * nof_initial_fgs_3,
              "bcm_fg_id_3_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC(dnx_fg_id_1_p, sizeof(*dnx_fg_id_1_p) * nof_initial_fgs_1,
              "dnx_fg_id_1_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(dnx_fg_id_2_p, sizeof(*dnx_fg_id_2_p) * nof_initial_fgs_2,
              "dnx_fg_id_2_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(dnx_fg_id_3_p, sizeof(*dnx_fg_id_3_p) * nof_initial_fgs_3,
              "dnx_fg_id_3_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    /*
     * Configure multiple actions.
     */
    bcm_field_context_info_t_init(&bcm_context_info);
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &bcm_context_info, &bcm_context_id));

    bcm_field_group_info_t_init(&bcm_fg_info);
    bcm_fg_info.stage = bcm_stage;
    bcm_fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    bcm_fg_info.nof_quals = 1;
    bcm_fg_info.qual_types[0] = bcmFieldQualifyIntPriority;
    bcm_fg_info.nof_actions = 1;
    bcm_fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    bcm_fg_info.action_with_valid_bit[0] = FALSE;

    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_1; fg_id_ndx++)
    {
        sal_snprintf((char *) bcm_fg_info.name, sizeof(bcm_fg_info.name), "act_mul_%d_%d", 1, fg_id_ndx);
        SHR_IF_ERR_EXIT(bcm_field_group_add
                        (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &bcm_fg_info, &bcm_fg_id_1_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fg_id_ndx++)
    {
        sal_snprintf((char *) bcm_fg_info.name, sizeof(bcm_fg_info.name), "act_mul_%d_%d", 2, fg_id_ndx);
        SHR_IF_ERR_EXIT(bcm_field_group_add
                        (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &bcm_fg_info, &bcm_fg_id_2_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_3; fg_id_ndx++)
    {
        sal_snprintf((char *) bcm_fg_info.name, sizeof(bcm_fg_info.name), "act_mul_%d_%d", 3, fg_id_ndx);
        SHR_IF_ERR_EXIT(bcm_field_group_add
                        (unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &bcm_fg_info, &bcm_fg_id_3_p[fg_id_ndx]));
    }

    bcm_field_group_attach_info_t_init(&bcm_attach_info);
    bcm_attach_info.key_info.nof_quals = bcm_fg_info.nof_quals;
    sal_memcpy(bcm_attach_info.key_info.qual_types, bcm_fg_info.qual_types,
               sizeof(bcm_attach_info.key_info.qual_types));
    bcm_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    bcm_attach_info.payload_info.nof_actions = bcm_fg_info.nof_actions;
    sal_memcpy(bcm_attach_info.payload_info.action_types, bcm_fg_info.action_types,
               sizeof(bcm_attach_info.payload_info.action_types));

    fg_counter = 0;
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_1; fg_id_ndx++)
    {
        bcm_attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, fg_counter);
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                        (unit, 0, bcm_fg_id_1_p[fg_id_ndx], bcm_context_id, &bcm_attach_info));
        fg_counter++;
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fg_id_ndx++)
    {
        bcm_attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, fg_counter);
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                        (unit, 0, bcm_fg_id_2_p[fg_id_ndx], bcm_context_id, &bcm_attach_info));
        fg_counter++;
    }

    /*
     * Convert BCM variables to DNX variable.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));
    dnx_context_id = bcm_context_id;
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_1; fg_id_ndx++)
    {
        dnx_fg_id_1_p[fg_id_ndx] = bcm_fg_id_1_p[fg_id_ndx];
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fg_id_ndx++)
    {
        dnx_fg_id_2_p[fg_id_ndx] = bcm_fg_id_2_p[fg_id_ndx];
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_3; fg_id_ndx++)
    {
        dnx_fg_id_3_p[fg_id_ndx] = bcm_fg_id_3_p[fg_id_ndx];
    }

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, dnx_stage, bcm_fg_info.action_types[0], &dnx_action_initial));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_action_type(unit, dnx_stage, dnx_action_initial, &action_type_initial));

    nof_fes_id_per_array = dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_fes_id_per_array;

    if (16 != nof_fes_id_per_array)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test assumes number of EFES per array in iPMF1 is 16.\r\n");
    }

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, dnx_stage, dnx_context_id, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != nof_initial_fgs_1 + nof_initial_fgs_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The FES configuration was not as expected. "
                     "Found %d FES quartets in context ID %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, dnx_context_id, nof_initial_fgs_1 + nof_initial_fgs_2);
    }
    /*
     * Verify that the FES_IDs are ordered.
     */
    for (fes_quartet_ndx = 1; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id <=
            context_fes_get_info.fes_quartets[fes_quartet_ndx - 1].fes_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES quartets out of order.\r\n");
        }
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }

    /*
     * Verify the actions.
     */
    fes_quartet_ndx = 0;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority !=
            BCM_FIELD_ACTION_PRIORITY(0, fes_quartet_ndx))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has priority %x instead of %x.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].priority,
                         BCM_FIELD_ACTION_PRIORITY(0, fes_quartet_ndx));
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type != action_type_initial)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has action type %d instead of %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type,
                         action_type_initial);
        }
    }
    for (fes_quartet_ndx = 0, fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_1; fes_quartet_ndx++, fg_id_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != 0 + fg_id_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d. FES ID should be %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx, 0 + fg_id_ndx);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id != dnx_fg_id_1_p[fg_id_ndx] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has fg_id and place_in_fg %d and %d instead of %d and %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg, dnx_fg_id_1_p[fg_id_ndx], 0);
        }
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fes_quartet_ndx++, fg_id_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != 3 + fg_id_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d. FES ID should be %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx, 3 + fg_id_ndx);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id != dnx_fg_id_2_p[fg_id_ndx] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has fg_id and place_in_fg %d and %d instead of %d and %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg, dnx_fg_id_2_p[fg_id_ndx], 0);
        }
    }

    /*
     * Detach the first group of field groups.
     */
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_1; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, bcm_fg_id_1_p[fg_id_ndx], bcm_context_id));
    }

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, dnx_stage, dnx_context_id, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != nof_initial_fgs_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The FES configuration was not as expected. "
                     "Found %d FES quartets in context ID %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, dnx_context_id, nof_initial_fgs_2);
    }
    /*
     * Verify that the FES_IDs are ordered.
     */
    for (fes_quartet_ndx = 1; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id <=
            context_fes_get_info.fes_quartets[fes_quartet_ndx - 1].fes_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES quartets out of order.\r\n");
        }
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }

    /*
     * Verify the actions.
     */
    fes_quartet_ndx = 0;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority !=
            BCM_FIELD_ACTION_PRIORITY(0, nof_initial_fgs_1 + fes_quartet_ndx))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has priority %x instead of %x.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].priority,
                         BCM_FIELD_ACTION_PRIORITY(0, nof_initial_fgs_1 + fes_quartet_ndx));
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type != action_type_initial)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has action type %d instead of %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type,
                         action_type_initial);
        }
    }
    for (fes_quartet_ndx = 0, fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fes_quartet_ndx++, fg_id_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != 3 + fg_id_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d. FES ID should be %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx, 3 + fg_id_ndx);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id != dnx_fg_id_2_p[fg_id_ndx] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has fg_id and place_in_fg %d and %d instead of %d and %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg, dnx_fg_id_2_p[fg_id_ndx], 0);
        }
    }

    /*
     * Attach the third group of field groups.
     */
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_3; fg_id_ndx++)
    {
        bcm_attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, fg_counter);
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach
                        (unit, 0, bcm_fg_id_3_p[fg_id_ndx], bcm_context_id, &bcm_attach_info));
        fg_counter++;
    }

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, dnx_stage, dnx_context_id, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != nof_initial_fgs_2 + nof_initial_fgs_3)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The FES configuration was not as expected. "
                     "Found %d FES quartets in context ID %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, dnx_context_id, nof_initial_fgs_2 + nof_initial_fgs_3);
    }
    /*
     * Verify that the FES_IDs are ordered.
     */
    for (fes_quartet_ndx = 1; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id <=
            context_fes_get_info.fes_quartets[fes_quartet_ndx - 1].fes_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES quartets out of order.\r\n");
        }
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, dnx_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }

    /*
     * Verify the actions.
     */
    fes_quartet_ndx = 0;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].priority !=
            BCM_FIELD_ACTION_PRIORITY(0, nof_initial_fgs_1 + fes_quartet_ndx))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has priority %x instead of %x.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].priority,
                         BCM_FIELD_ACTION_PRIORITY(0, nof_initial_fgs_1 + fes_quartet_ndx));
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type != action_type_initial)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has action type %d instead of %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type,
                         action_type_initial);
        }
    }
    for (fes_quartet_ndx = 0, fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fes_quartet_ndx++, fg_id_ndx++)
    {
        if (fg_id_ndx == 0)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d "
                             "in FES quartet %d. FES ID should be %d.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx, 0);
            }
        }
        else
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != 2 + fg_id_ndx)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d "
                             "in FES quartet %d. FES ID should be %d.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                             2 + fg_id_ndx);
            }
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id != dnx_fg_id_2_p[fg_id_ndx] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has fg_id and place_in_fg %d and %d instead of %d and %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg, dnx_fg_id_2_p[fg_id_ndx], 0);
        }
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_3; fes_quartet_ndx++, fg_id_ndx++)
    {
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id != 14 + fg_id_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d. FES ID should be %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         14 + fg_id_ndx);
        }
        if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id != dnx_fg_id_3_p[fg_id_ndx] ||
            context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The FES configuration was not as expected, "
                         "action %d in field group %d FES ID %d FES program ID %d "
                         "in FES quartet %d has fg_id and place_in_fg %d and %d instead of %d and %d.\r\n",
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes_quartet_ndx,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                         context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg, dnx_fg_id_3_p[fg_id_ndx], 0);
        }
    }

    /*
     * Delete the configuration.
     */
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_1; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, bcm_fg_id_1_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, bcm_fg_id_2_p[fg_id_ndx], bcm_context_id));
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, bcm_fg_id_2_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_3; fg_id_ndx++)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, bcm_fg_id_3_p[fg_id_ndx], bcm_context_id));
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, bcm_fg_id_3_p[fg_id_ndx]));
    }
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_id));

exit:
    SHR_FREE(bcm_fg_id_1_p);
    SHR_FREE(bcm_fg_id_2_p);
    SHR_FREE(bcm_fg_id_3_p);

    SHR_FREE(dnx_fg_id_1_p);
    SHR_FREE(dnx_fg_id_2_p);
    SHR_FREE(dnx_fg_id_3_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the code in the field actions module, when using priority invalidate.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which the database is to be created and tested (IPMF1, IPMF2, IPMF3, EPMF).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
static shr_error_e
appl_dnx_action_tester_fes_cfg_invalidate_prio(
    int unit,
    dnx_field_stage_e field_stage)
{
    dnx_field_stage_e field_stage_for_context = field_stage;
    bcm_field_group_t fg_id;
    dnx_field_context_mode_t context_mode;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    dnx_field_context_t context_id_1;
    dnx_field_context_t context_id_2;
    dnx_field_context_t context_id_empty;
    dnx_field_context_t context_id_full;
    dnx_field_actions_fes_quartet_context_get_info_t fes_cfg_context_id_1_0;
    dnx_field_actions_fes_quartet_context_get_info_t fes_cfg_context_id_2_1;
    dnx_field_actions_fes_quartet_context_get_info_t fes_cfg_context_id_full_0;
    dnx_field_actions_fes_quartet_context_get_info_t fes_cfg_context_id_full_1;
    unsigned int fes_quartet_ndx;
    unsigned int fes2msb_ndx;
    dnx_field_actions_fes_context_get_info_t context_fes_get_info;
    dnx_field_actions_fes_context_group_get_info_t group_fes_get_info;
    unsigned int nof_instr_per_fes_quartet;
    dnx_field_group_context_full_info_t attach_full_info_get;
    dnx_field_action_t dnx_action_1;
    dnx_field_action_t dnx_action_void;
    dnx_field_action_t dnx_action_2;
    bcm_field_stage_t bcm_stage;
    bcm_field_group_attach_info_t bcm_attach_info_get;
    int nof_void_actions = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_instr_per_fes_quartet = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;

    /*
     * Create the contexts.
     */
    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        field_stage_for_context = DNX_FIELD_STAGE_IPMF1;
    }
    else
    {
        field_stage_for_context = field_stage;
    }
    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, field_stage_for_context, &context_mode, &context_id_1));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, field_stage_for_context, &context_mode, &context_id_2));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, field_stage_for_context, &context_mode, &context_id_empty));
    SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, field_stage_for_context, &context_mode, &context_id_full));

    /*
     * Verify that the contexts are empty.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_1, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "New context ID %d in stage %s isn't empty.\r\n",
                     context_id_1, dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_2, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "New context ID %d in stage %s isn't empty.\r\n",
                     context_id_2, dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_empty, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "New context ID %d in stage %s isn't empty.\r\n",
                     context_id_empty, dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_full, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "New context ID %d in stage %s isn't empty.\r\n",
                     context_id_full, dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    /*
     * Create a field group.
     */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = bcm_stage;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyIpFrag;
    fg_info.nof_actions = 3;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    fg_info.action_types[1] = bcmFieldActionVoid;
    fg_info.action_with_valid_bit[1] = FALSE;
    nof_void_actions++;
    fg_info.action_types[2] = bcmFieldActionPrioIntNew;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    /*
     * Create the attach info.
     */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];
    attach_info.payload_info.action_types[2] = fg_info.action_types[2];

    /*
     * Attach to context_id_1.
     */
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 1);
    attach_info.payload_info.action_info[2].priority = BCM_FIELD_ACTION_INVALIDATE;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id_1, &attach_info));

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_1, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Found %d FES quartets in context_id_1 %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, context_id_1, 1);
    }
    if (context_fes_get_info.fes_quartets[0].place_in_fg != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Action's place in field group (%d) isn't %d.\r\n",
                     context_fes_get_info.fes_quartets[0].place_in_fg, 0);
    }
    if (context_fes_get_info.fes_quartets[0].fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "fg_id (%d) isn't %d.\r\n", context_fes_get_info.fes_quartets[0].fg_id, fg_id);
    }
    if (context_fes_get_info.fes_quartets[0].priority != attach_info.payload_info.action_info[0].priority)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected.\r\n");
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < nof_instr_per_fes_quartet; fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After first run, the FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }
    /*
     * Save configuration for comparison later.
     */
    sal_memcpy(&fes_cfg_context_id_1_0, &context_fes_get_info.fes_quartets[0], sizeof(fes_cfg_context_id_1_0));
    /*
     * Test attach info for dnx_field_actions_context_fes_info_to_group_fes_info(). 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id, &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != BCM_FIELD_ACTION_PRIORITY(0, 1) ||
        group_fes_get_info.initial_fes_quartets[1].priority != DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d in context %d.\r\n", fg_id,
                     context_id_1);
    }

    /*
     * Attach to context_id_2.
     */
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_INVALIDATE;
    attach_info.payload_info.action_info[2].priority = BCM_FIELD_ACTION_DONT_CARE;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id_2, &attach_info));

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_2, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Found %d FES quartets in context_id_2 %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, context_id_2, 1);
    }
    if (context_fes_get_info.fes_quartets[0].place_in_fg != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Action's place in field group (%d) isn't %d.\r\n",
                     context_fes_get_info.fes_quartets[0].place_in_fg, 1);
    }
    if (context_fes_get_info.fes_quartets[0].fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "fg_id (%d) isn't %d.\r\n", context_fes_get_info.fes_quartets[0].fg_id, fg_id);
    }
    if (context_fes_get_info.fes_quartets[0].priority != attach_info.payload_info.action_info[2].priority)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected.\r\n");
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < nof_instr_per_fes_quartet; fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After first run, the FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }
    /*
     * Save configuration for comparison later.
     */
    sal_memcpy(&fes_cfg_context_id_2_1, &context_fes_get_info.fes_quartets[0], sizeof(fes_cfg_context_id_2_1));
    /*
     * Test attach info for dnx_field_actions_context_fes_info_to_group_fes_info(). 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id, &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != DNX_FIELD_ACTION_PRIORITY_INVALID ||
        group_fes_get_info.initial_fes_quartets[1].priority != BCM_FIELD_ACTION_DONT_CARE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d in context %d.\r\n", fg_id,
                     context_id_2);
    }

    /*
     * Attach to context_id_empty.
     */
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_INVALIDATE;
    attach_info.payload_info.action_info[2].priority = BCM_FIELD_ACTION_INVALIDATE;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id_empty, &attach_info));

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_empty, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Found %d FES quartets in context_id_empty %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, context_id_empty, 0);
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < nof_instr_per_fes_quartet; fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After first run, the FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }
    /*
     * Test attach info for dnx_field_actions_context_fes_info_to_group_fes_info(). 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id, &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != DNX_FIELD_ACTION_PRIORITY_INVALID ||
        group_fes_get_info.initial_fes_quartets[1].priority != DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d in context %d.\r\n", fg_id,
                     context_id_empty);
    }

    /*
     * Attach to context_id_full.
     */
    attach_info.payload_info.action_info[0].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    attach_info.payload_info.action_info[2].priority = BCM_FIELD_ACTION_POSITION(0, 0);
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id_full, &attach_info));

    /*
     * Get FES configuration.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_full, &context_fes_get_info));
    /*
     * Verify the FES configuration.
     */
    if (context_fes_get_info.nof_fes_quartets != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Found %d FES quartets in context_id_full %d, instead of %d.\r\n",
                     context_fes_get_info.nof_fes_quartets, context_id_full, 2);
    }
    if (context_fes_get_info.fes_quartets[0].place_in_fg != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Action's place in field group (%d) isn't %d.\r\n",
                     context_fes_get_info.fes_quartets[0].place_in_fg, 1);
    }
    if (context_fes_get_info.fes_quartets[1].place_in_fg != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "Action's place in field group (%d) isn't %d.\r\n",
                     context_fes_get_info.fes_quartets[1].place_in_fg, 0);
    }
    if (context_fes_get_info.fes_quartets[0].fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "fg_id (%d) isn't %d.\r\n", context_fes_get_info.fes_quartets[0].fg_id, fg_id);
    }
    if (context_fes_get_info.fes_quartets[1].fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected. "
                     "fg_id (%d) isn't %d.\r\n", context_fes_get_info.fes_quartets[1].fg_id, fg_id);
    }
    if (context_fes_get_info.fes_quartets[0].priority != attach_info.payload_info.action_info[2].priority)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected.\r\n");
    }
    if (context_fes_get_info.fes_quartets[1].priority != attach_info.payload_info.action_info[0].priority)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After first run, the FES configuration was not as expected.\r\n");
    }
    /*
     * Verify that the result are the same for all FES2msb instructions.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < context_fes_get_info.nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes2msb_ndx = 1; fes2msb_ndx < nof_instr_per_fes_quartet; fes2msb_ndx++)
        {
            if (context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].action_type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].action_type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].valid_bits !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].valid_bits
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].shift !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].shift
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].type !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].type
                || context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[0].polarity !=
                context_fes_get_info.fes_quartets[fes_quartet_ndx].fes2msb_info[fes2msb_ndx].polarity)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "After first run, the FES configuration was not as expected, "
                             "action %d in field group %d FES ID %d FES program ID %d"
                             "FES2msb ID %d does not have the same FES configuration as FES2msb ID 0.\r\n",
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].place_in_fg,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fg_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_id,
                             context_fes_get_info.fes_quartets[fes_quartet_ndx].fes_pgm_id, fes2msb_ndx);
            }
        }
    }
    /*
     * Save configuration for comparison later.
     */
    sal_memcpy(&fes_cfg_context_id_full_0, &context_fes_get_info.fes_quartets[1], sizeof(fes_cfg_context_id_full_0));
    sal_memcpy(&fes_cfg_context_id_full_1, &context_fes_get_info.fes_quartets[0], sizeof(fes_cfg_context_id_full_1));
    /*
     * Test attach info for dnx_field_actions_context_fes_info_to_group_fes_info(). 
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id, &context_fes_get_info, &group_fes_get_info));
    if (group_fes_get_info.initial_fes_quartets[0].priority != BCM_FIELD_ACTION_POSITION(0, 1) ||
        group_fes_get_info.initial_fes_quartets[1].priority != BCM_FIELD_ACTION_POSITION(0, 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected allocation first run field group %d in context %d.\r\n", fg_id,
                     context_id_2);
    }

    /*
     * Compare EFES configuration.
     */
    if (sal_memcmp(fes_cfg_context_id_1_0.fes2msb_info, fes_cfg_context_id_full_0.fes2msb_info,
                   sizeof(fes_cfg_context_id_1_0.fes2msb_info)) ||
        (fes_cfg_context_id_1_0.key_select != fes_cfg_context_id_full_0.key_select) ||
        (fes_cfg_context_id_1_0.place_in_fg != fes_cfg_context_id_full_0.place_in_fg))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Compare failed for first action.\r\n");
    }
    if (sal_memcmp(fes_cfg_context_id_2_1.fes2msb_info, fes_cfg_context_id_full_1.fes2msb_info,
                   sizeof(fes_cfg_context_id_2_1.fes2msb_info)) ||
        (fes_cfg_context_id_2_1.key_select != fes_cfg_context_id_full_1.key_select) ||
        (fes_cfg_context_id_2_1.place_in_fg != fes_cfg_context_id_full_1.place_in_fg))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Compare failed for second action.\r\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, field_stage, fg_info.action_types[0], &dnx_action_1));
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, field_stage, fg_info.action_types[1], &dnx_action_void));
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, field_stage, fg_info.action_types[2], &dnx_action_2));

    /*
     * Get FES info using dnx_field_group_context_get.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_1, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[1] != dnx_action_void ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_PRIORITY(0, 1) ||
        attach_full_info_get.attach_basic_info.action_info[1].priority != BCM_FIELD_ACTION_DONT_CARE ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity !=
        attach_info.payload_info.action_info[0].valid_bit_polarity ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != fes_cfg_context_id_1_0.fes_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_pgm_id != fes_cfg_context_id_1_0.fes_pgm_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].key_select != fes_cfg_context_id_1_0.key_select ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority !=
        attach_full_info_get.attach_basic_info.action_info[0].priority ||
        sal_memcmp(fes_cfg_context_id_1_0.fes2msb_info,
                   attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes2msb_info,
                   sizeof(fes_cfg_context_id_1_0.fes2msb_info)) ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority != DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_2, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[1] != dnx_action_void ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[1].priority != BCM_FIELD_ACTION_DONT_CARE ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_DONT_CARE ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity != 1 ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity !=
        attach_info.payload_info.action_info[2].valid_bit_polarity)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority != DNX_FIELD_ACTION_PRIORITY_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != fes_cfg_context_id_2_1.fes_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_pgm_id != fes_cfg_context_id_2_1.fes_pgm_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].key_select != fes_cfg_context_id_2_1.key_select ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority !=
        attach_full_info_get.attach_basic_info.action_info[2].priority ||
        sal_memcmp(fes_cfg_context_id_2_1.fes2msb_info,
                   attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes2msb_info,
                   sizeof(fes_cfg_context_id_2_1.fes2msb_info)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_empty, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[1] != dnx_action_void ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[1].priority != BCM_FIELD_ACTION_DONT_CARE ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity != 1 ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority != DNX_FIELD_ACTION_PRIORITY_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority != DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_full, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[1] != dnx_action_void ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_POSITION(0, 1) ||
        attach_full_info_get.attach_basic_info.action_info[1].priority != BCM_FIELD_ACTION_DONT_CARE ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_POSITION(0, 0) ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity !=
        attach_info.payload_info.action_info[0].valid_bit_polarity ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity !=
        attach_info.payload_info.action_info[2].valid_bit_polarity)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != fes_cfg_context_id_full_0.fes_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_pgm_id != fes_cfg_context_id_full_0.fes_pgm_id
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[0].key_select !=
        fes_cfg_context_id_full_0.key_select
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority !=
        attach_full_info_get.attach_basic_info.action_info[0].priority
        || sal_memcmp(fes_cfg_context_id_full_0.fes2msb_info,
                      attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes2msb_info,
                      sizeof(fes_cfg_context_id_full_0.fes2msb_info))
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != fes_cfg_context_id_full_1.fes_id
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_pgm_id !=
        fes_cfg_context_id_full_1.fes_pgm_id
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].key_select !=
        fes_cfg_context_id_full_1.key_select
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority !=
        attach_full_info_get.attach_basic_info.action_info[2].priority
        || sal_memcmp(fes_cfg_context_id_full_1.fes2msb_info,
                      attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes2msb_info,
                      sizeof(fes_cfg_context_id_full_1.fes2msb_info)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }

    /*
     * Verify bcm_field_group_context_info_get.
     */
    SHR_IF_ERR_EXIT(bcm_field_group_context_info_get(unit, fg_id, context_id_1, &bcm_attach_info_get));
    if (bcm_attach_info_get.payload_info.nof_actions != 3 ||
        bcm_attach_info_get.payload_info.action_types[0] != bcmFieldActionDropPrecedenceRaw ||
        bcm_attach_info_get.payload_info.action_types[2] != bcmFieldActionPrioIntNew ||
        bcm_attach_info_get.payload_info.action_info[0].priority != BCM_FIELD_ACTION_PRIORITY(0, 1) ||
        bcm_attach_info_get.payload_info.action_info[2].priority != BCM_FIELD_ACTION_INVALIDATE ||
        bcm_attach_info_get.payload_info.action_info[0].valid_bit_polarity != 1 ||
        bcm_attach_info_get.payload_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bcm_field_group_context_info_get unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_info_get(unit, fg_id, context_id_2, &bcm_attach_info_get));
    if (bcm_attach_info_get.payload_info.nof_actions != 3 ||
        bcm_attach_info_get.payload_info.action_types[0] != bcmFieldActionDropPrecedenceRaw ||
        bcm_attach_info_get.payload_info.action_types[2] != bcmFieldActionPrioIntNew ||
        bcm_attach_info_get.payload_info.action_info[0].priority != BCM_FIELD_ACTION_INVALIDATE ||
        bcm_attach_info_get.payload_info.action_info[2].priority != BCM_FIELD_ACTION_DONT_CARE ||
        bcm_attach_info_get.payload_info.action_info[0].valid_bit_polarity != 1 ||
        bcm_attach_info_get.payload_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bcm_field_group_context_info_get unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_info_get(unit, fg_id, context_id_empty, &bcm_attach_info_get));
    if (bcm_attach_info_get.payload_info.nof_actions != 3 ||
        bcm_attach_info_get.payload_info.action_types[0] != bcmFieldActionDropPrecedenceRaw ||
        bcm_attach_info_get.payload_info.action_types[2] != bcmFieldActionPrioIntNew ||
        bcm_attach_info_get.payload_info.action_info[0].priority != BCM_FIELD_ACTION_INVALIDATE ||
        bcm_attach_info_get.payload_info.action_info[2].priority != BCM_FIELD_ACTION_INVALIDATE ||
        bcm_attach_info_get.payload_info.action_info[0].valid_bit_polarity != 1 ||
        bcm_attach_info_get.payload_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bcm_field_group_context_info_get unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(bcm_field_group_context_info_get(unit, fg_id, context_id_full, &bcm_attach_info_get));
    if (bcm_attach_info_get.payload_info.nof_actions != 3 ||
        bcm_attach_info_get.payload_info.action_types[0] != bcmFieldActionDropPrecedenceRaw ||
        bcm_attach_info_get.payload_info.action_types[2] != bcmFieldActionPrioIntNew ||
        bcm_attach_info_get.payload_info.action_info[0].priority != BCM_FIELD_ACTION_POSITION(0, 1) ||
        bcm_attach_info_get.payload_info.action_info[2].priority != BCM_FIELD_ACTION_POSITION(0, 0) ||
        bcm_attach_info_get.payload_info.action_info[0].valid_bit_polarity != 1 ||
        bcm_attach_info_get.payload_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "bcm_field_group_context_info_get unsuccessful.\r\n");
    }

    /*
     * Detach field group from one context and check the configuration.
     */
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id_1));
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_1, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_2, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_empty, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_full, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_2, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_DONT_CARE ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity != 1 ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity !=
        attach_info.payload_info.action_info[2].valid_bit_polarity)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority != DNX_FIELD_ACTION_PRIORITY_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != fes_cfg_context_id_2_1.fes_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_pgm_id != fes_cfg_context_id_2_1.fes_pgm_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].key_select != fes_cfg_context_id_2_1.key_select ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority !=
        attach_full_info_get.attach_basic_info.action_info[2].priority ||
        sal_memcmp(fes_cfg_context_id_2_1.fes2msb_info,
                   attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes2msb_info,
                   sizeof(fes_cfg_context_id_2_1.fes2msb_info)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_empty, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_INVALIDATE ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity != 1 ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority != DNX_FIELD_ACTION_PRIORITY_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != DNX_FIELD_EFES_ID_INVALID ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority != DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id_full, &attach_full_info_get));
    if (attach_full_info_get.attach_basic_info.dnx_actions[0] != dnx_action_1 ||
        attach_full_info_get.attach_basic_info.dnx_actions[2] != dnx_action_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get actions as expected.\r\n");
    }
    if (attach_full_info_get.attach_basic_info.action_info[0].priority != BCM_FIELD_ACTION_POSITION(0, 1) ||
        attach_full_info_get.attach_basic_info.action_info[2].priority != BCM_FIELD_ACTION_POSITION(0, 0) ||
        attach_full_info_get.attach_basic_info.action_info[0].valid_bit_polarity !=
        attach_info.payload_info.action_info[0].valid_bit_polarity ||
        attach_full_info_get.attach_basic_info.action_info[2].valid_bit_polarity !=
        attach_info.payload_info.action_info[2].valid_bit_polarity)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get action attach info as expected.\r\n");
    }
    if (attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_id != fes_cfg_context_id_full_0.fes_id ||
        attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes_pgm_id != fes_cfg_context_id_full_0.fes_pgm_id
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[0].key_select !=
        fes_cfg_context_id_full_0.key_select
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[0].priority !=
        attach_full_info_get.attach_basic_info.action_info[0].priority
        || sal_memcmp(fes_cfg_context_id_full_0.fes2msb_info,
                      attach_full_info_get.actions_fes_info.initial_fes_quartets[0].fes2msb_info,
                      sizeof(fes_cfg_context_id_full_0.fes2msb_info))
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_id != fes_cfg_context_id_full_1.fes_id
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes_pgm_id !=
        fes_cfg_context_id_full_1.fes_pgm_id
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].key_select !=
        fes_cfg_context_id_full_1.key_select
        || attach_full_info_get.actions_fes_info.initial_fes_quartets[1].priority !=
        attach_full_info_get.attach_basic_info.action_info[2].priority
        || sal_memcmp(fes_cfg_context_id_full_1.fes2msb_info,
                      attach_full_info_get.actions_fes_info.initial_fes_quartets[1].fes2msb_info,
                      sizeof(fes_cfg_context_id_full_1.fes2msb_info)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to get EFES info as expected.\r\n");
    }

    /*
     * Detach field group from the other contexts.
     */
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id_2));
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id_empty));
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id_full));

    /*
     * Check that the configuration was deleted.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_1, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_2, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_empty, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }
    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id_full, &context_fes_get_info));
    if (context_fes_get_info.nof_fes_quartets != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES delete unsuccessful.\r\n");
    }

    /*
     * Delete field group.
     */
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));
    /*
     * Destroy the contexts.
     */
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, field_stage_for_context, context_id_1));
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, field_stage_for_context, context_id_2));
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, field_stage_for_context, context_id_empty));
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, field_stage_for_context, context_id_full));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'action' testing
 *   application.
 *   For 'database', this function supplies all required SW info.
 *   For 'entry', this function sets all required HW
 *   configuration for the 'action' template which is supposed to have been
 *   set on the 'database' step.
 *
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_action_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *action_test_type_name;
    dnx_field_stage_e field_stage;
    ctest_dnx_field_fem_test_type_e fem_test_type;
    int clean_resources;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR(DNX_DIAG_ACTION_OPTION_TEST_TYPE, action_test_type_name);
    SH_SAND_GET_ENUM(DNX_DIAG_ACTION_OPTION_TEST_STAGE, field_stage);
    SH_SAND_GET_ENUM(DNX_DIAG_ACTION_OPTION_FEM_TEST_TYPE, fem_test_type);
    SH_SAND_GET_BOOL(DNX_DIAG_ACTION_OPTION_TEST_CLEAN, clean_resources);
    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (sal_strcasecmp(action_test_type_name, TEST_TYPE_DATABASE) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Tesing 'data base only' for stage %s. %s\r\n",
                    __FUNCTION__, __LINE__, dnx_field_stage_text(unit, field_stage), EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_db_only(unit, field_stage));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Options %s %s. Testing 'data base only' returned 'success' flag.\r\n",
                    __FUNCTION__, __LINE__, action_test_type_name, dnx_field_stage_text(unit, field_stage));
    }
    else if (sal_strcasecmp(action_test_type_name, TEST_TYPE_FES_CFG) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'fes configuration' for stage %s %s\r\n",
                    __FUNCTION__, __LINE__, dnx_field_stage_text(unit, field_stage), EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg(unit, field_stage));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Options %s %s. Testing 'fes configuration' returned 'success' flag.\r\n",
                    __FUNCTION__, __LINE__, action_test_type_name, dnx_field_stage_text(unit, field_stage));
    }
    else if (sal_strcasecmp(action_test_type_name, TEST_TYPE_FES_CFG_MUL) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'fes configuration multiple' %s%s\r\n",
                    __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg_mul(unit));
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg_mul_back(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'fes configuration multiple' returned 'success' flag. %s\r\n",
                    __FUNCTION__, __LINE__, action_test_type_name, EMPTY);
    }
    else if (sal_strcasecmp(action_test_type_name, TEST_TYPE_FES_CFG_INVALIDATE) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'fes configuration invalidate priority' %s%s\r\n",
                    __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg_invalidate_prio(unit, field_stage));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'fes configuration invalidate priority' returned 'success' flag. %s\r\n",
                    __FUNCTION__, __LINE__, action_test_type_name, EMPTY);
    }
    else if (sal_strcasecmp(action_test_type_name, TEST_TYPE_FEM) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing various FEM operations. %s %s\r\n", __FUNCTION__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fem(unit, fem_test_type, clean_resources, sand_control));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Options '%s %s'. Testing Testing various FEM operations returned 'success' flag.\r\n",
                    __FUNCTION__, __LINE__, action_test_type_name,
                    SH_SAND_GET_ENUM_STR(DNX_DIAG_ACTION_OPTION_FEM_TEST_TYPE, fem_test_type));
        if (clean_resources)
        {
            /**
             * Delete the created FG (if any)
             */
            SHR_IF_ERR_EXIT(ctest_dnx_field_util_clean(unit));
        }
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, action_test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - run 'action' sequence in diag shell
 */
shr_error_e
sh_dnx_field_action_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 count, count_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
        SHR_IF_ERR_EXIT(appl_dnx_action_starter(unit, args, sand_control));
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

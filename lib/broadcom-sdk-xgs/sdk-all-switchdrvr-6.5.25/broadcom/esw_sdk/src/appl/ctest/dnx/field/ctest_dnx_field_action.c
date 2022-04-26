/** \file diag_dnx_field_action.c
 * $Id$
 *
 * 'Action' operations (for payload and FES/FEM configuration and allocation) procedures for DNX.
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
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>
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
#define TEST_TYPE_KEY_SELECT       "KEY_SELECT"
#define TEST_TYPE_MAP_OVERRIDE     "MAP_OVERRIDE"

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
    {"DNX_action_for_key_select",  "type=key_select",                                CTEST_POSTCOMMIT},
    {"DNX_action_for_map_override","type=map_override",                              CTEST_POSTCOMMIT},
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
    {DNX_DIAG_ACTION_OPTION_TEST_TYPE,       SAL_FIELD_TYPE_STR,     "Type of test (database, fes_cfg)",                           NULL},
    {DNX_DIAG_ACTION_OPTION_TEST_STAGE,      SAL_FIELD_TYPE_ENUM,    "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",                    "ipmf3",           (void *)Field_stage_enum_table},
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
        "    dnx_field_actions_context_fes_info_to_group_fes_info(),\r\n" "    dnx_field_actions_fes_detach().\r\n",
    .synopsis = "ctest field action type=<database | fes_cfg | fes_cfg_mul> stage=<ipmf1 | ipmf2 | ipmf3 | epmf> \r\n",
    .examples = "type=database\n" "TY=database stage=ipmf1\n"
};

/** The number of field groups used in the FES configuration test.*/
#define CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_FG        3
/** The number of actions used per field group in the FES configuration test.*/
#define CTEST_DNX_FIELD_ACTION_FES_CFG_NOF_ACTIONS   2
/** The number ofrom which the field group IDs start in the FES configuration test.*/
#define CTEST_DNX_FIELD_ACTION_FES_CFG_BASE_FG_ID    40

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
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_PP_PORT);
        actions_info_arr[0].dont_use_valid_bit = 0;

        actions_info_arr[1].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_DST_DATA);
        actions_info_arr[1].dont_use_valid_bit = 0;

        actions_info_arr[2].dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_DP);
        actions_info_arr[2].dont_use_valid_bit = 0;

        actions_info_arr[3].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_NWK_QOS);
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
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_ADMT_PROFILE);
        actions_info_arr[0].dont_use_valid_bit = 0;

        actions_info_arr[1].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_BIER_STR_OFFSET);
        actions_info_arr[1].dont_use_valid_bit = 0;

        actions_info_arr[2].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_BIER_STR_SIZE);
        actions_info_arr[2].dont_use_valid_bit = 0;

        actions_info_arr[3].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_BYTES_TO_REMOVE);
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
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_DISCARD);
        actions_info_arr[0].dont_use_valid_bit = 0;

        actions_info_arr[1].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_TC_MAP_PROFILE);
        actions_info_arr[1].dont_use_valid_bit = 0;

        actions_info_arr[2].dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_TC);
        actions_info_arr[2].dont_use_valid_bit = 0;

        actions_info_arr[3].dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_DP);
        actions_info_arr[3].dont_use_valid_bit = 0;

        actions_info_arr[4].dnx_action =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, field_stage, DNX_FIELD_ACTION_PP_DSP);
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
            action_type[0] = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_DP, id);
            action_type[1] = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_DST_DATA, id);
            action_type_extra = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_END_OF_PACKET_EDITING, id);
            key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_0_63_0;
            key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_A_FES_KEY_SELECT_TCAM_2_31_0_TCAM_1_63_32;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            action_type[0] = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_BIER_STR_OFFSET, id);
            action_type[1] = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_BIER_STR_SIZE, id);
            action_type_extra = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_EXT_STATISTICS_3, id);
            key_select[0] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_TCAM_0_63_0;
            key_select[1] = DBAL_ENUM_FVAL_FIELD_PMF_B_FES_KEY_SELECT_SEXEM_27_0_4_B0_TCAM_1_63_32;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            action_type[0] = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_TC, id);
            action_type[1] = DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_SNOOP_STRENGTH, id);
            action_type_extra =
                DNX_FIELD_DNX_ACTION_PARAM_GET(field_stage, DNX_FIELD_ACTION_PMF_FWD_OVERRIDE_ENABLE, id);
            key_select[0] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_0_PAYLOAD_63_0;
            key_select[1] = DBAL_ENUM_FVAL_FIELD_E_PMF_FES_KEY_SELECT_TCAM_1_PAYLOAD_63_0;
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
    /*
     * Partial testing for FES info for the last (second) FES quartet of the last (third) field group
     * from dnx_field_actions_context_fes_info_to_group_fes_info().
     */
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
     * Note we assume FES_id==FES_position_in_array because it is the first array.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_info_t_init(unit, field_stage, fes_inst_info));
    fes_inst_info[0].common_info.fes2msb_info[0].action_type = action_type_extra;
    fes_inst_info[0].alloc_info.priority = BCM_FIELD_ACTION_POSITION(0, fes_id_found);
    fes_inst_info[0].write_info.key_select = key_select[1];
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
        fes_inst_info[0].write_info.key_select = key_select[1];
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
    fes_inst_info[0].write_info.key_select = key_select[0];
    fes_inst_info[1].write_info.key_select = key_select[0];
    fes_inst_info[2].write_info.key_select = key_select[0];
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
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &bcm_fg_info_1, &bcm_fg_id_1_p[fg_id_ndx]));
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
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &bcm_fg_info_2, &bcm_fg_id_2_p[fg_id_ndx]));
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
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &bcm_fg_info, &bcm_fg_id_1_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_2; fg_id_ndx++)
    {
        sal_snprintf((char *) bcm_fg_info.name, sizeof(bcm_fg_info.name), "act_mul_%d_%d", 2, fg_id_ndx);
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &bcm_fg_info, &bcm_fg_id_2_p[fg_id_ndx]));
    }
    for (fg_id_ndx = 0; fg_id_ndx < nof_initial_fgs_3; fg_id_ndx++)
    {
        sal_snprintf((char *) bcm_fg_info.name, sizeof(bcm_fg_info.name), "act_mul_%d_%d", 3, fg_id_ndx);
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &bcm_fg_info, &bcm_fg_id_3_p[fg_id_ndx]));
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
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));

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
 *    Test the DNX data for EFES ad FEM key_select
 * \param [in] unit -
 *    Identifier of HW platform.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
static shr_error_e
appl_dnx_action_tester_key_select(
    int unit)
{
    int efes_max_nof_key_selects = dnx_data_field.efes.max_nof_key_selects_per_field_io_get(unit);
    int fem_max_nof_key_selects = dnx_data_field.fem.max_nof_key_selects_per_field_io_get(unit);
    int num_bits_fem = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select;
    int field_io_iter;
    dnx_field_stage_e stage_iter;
    int key_select_iter;
    int nof_key_select;
    uint32 field_io_enum_iter;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check EFES key select table size.
     */
    if (dnx_data_field.efes.key_select_properties_info_get(unit)->key_size[0] != DNX_FIELD_STAGE_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Unexpected size for stage key of EFES key_select_properties: %d instead of %d.\r\n",
                     dnx_data_field.efes.key_select_properties_info_get(unit)->key_size[0], DNX_FIELD_STAGE_NOF);
    }
    if (dnx_data_field.efes.key_select_properties_info_get(unit)->key_size[1] !=
        DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Unexpected size for field_io key of EFES key_select_properties: %d instead of %d.\r\n",
                     dnx_data_field.efes.key_select_properties_info_get(unit)->key_size[1],
                     DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS);
    }

    /*
     * Check FEM key select table size.
     */
    if (dnx_data_field.fem.key_select_properties_info_get(unit)->key_size[0] !=
        DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Unexpected size for field_io key of FEM key_select_properties: %d instead of %d.\r\n",
                     dnx_data_field.fem.key_select_properties_info_get(unit)->key_size[0],
                     DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS);
    }
    if (dnx_data_field.fem.key_select_properties_info_get(unit)->key_size[1] != 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Unexpected size for unused key of FEM key_select_properties: %d instead of %d.\r\n",
                     dnx_data_field.fem.key_select_properties_info_get(unit)->key_size[0], 0);
    }

    /*
     * Verify that all field_io enum values fit in DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS.
     */
    SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get(unit, DBAL_FIELD_FIELD_IO, INVALID_ENUM, &field_io_enum_iter));
    if (field_io_enum_iter == INVALID_ENUM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field IO DBAL ENUM is empty.\n");
    }
    while (field_io_enum_iter != INVALID_ENUM)
    {
        if (field_io_enum_iter >= DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "DBAL ENUM DBAL_FIELD_FIELD_IO has value %d, exceeds %d.\n",
                         field_io_enum_iter, DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS);
        }
        SHR_IF_ERR_EXIT(dbal_fields_enum_next_enum_value_get
                        (unit, DBAL_FIELD_FIELD_IO, field_io_enum_iter, &field_io_enum_iter));
    }

    /*
     * EFES
     */
    for (stage_iter = 0; stage_iter < DNX_FIELD_STAGE_NOF; stage_iter++)
    {
        int num_bits_efes = dnx_data_field.stage.stage_info_get(unit, stage_iter)->nof_bits_in_fes_key_select;
        if (stage_iter == DNX_FIELD_STAGE_ACE)
        {
            continue;
        }
        for (field_io_iter = 0; field_io_iter < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS; field_io_iter++)
        {
            const uint8 *num_bits_p =
                dnx_data_field.efes.key_select_properties_get(unit, stage_iter, field_io_iter)->num_bits;
            for (nof_key_select = 0; (nof_key_select < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO) &&
                 (num_bits_p[nof_key_select] != 0); nof_key_select++);
            if (nof_key_select > efes_max_nof_key_selects)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "EFES key_select_properties stage %d field_io %d %d key_selects, maximum %d.\r\n",
                             stage_iter, field_io_iter, nof_key_select, efes_max_nof_key_selects);
            }
            for (key_select_iter = nof_key_select;
                 key_select_iter < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; key_select_iter++)
            {
                uint32 num_bits = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                                field_io_iter)->num_bits
                    [key_select_iter];
                uint32 lsb = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                           field_io_iter)->lsb[key_select_iter];
                uint32 num_bits_not_on_key = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                                           field_io_iter)->
                    num_bits_not_on_key[key_select_iter];
                uint32 key_select = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                                  field_io_iter)->key_select
                    [key_select_iter];
                if (num_bits != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "EFES key_select_properties stage %d field_io %d %d key selects, "
                                 "but %d isn't empty num_bits %d.\r\n",
                                 stage_iter, field_io_iter, nof_key_select, key_select_iter, num_bits);
                }
                if (key_select != (uint8) (-1))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "EFES key_select_properties stage %d field_io %d %d key selects, "
                                 "but %d isn't empty key_select %d.\r\n",
                                 stage_iter, field_io_iter, nof_key_select, key_select_iter, key_select);
                }
                if (lsb != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "EFES key_select_properties stage %d field_io %d %d key selects, "
                                 "but %d isn't empty lsb %d.\r\n",
                                 stage_iter, field_io_iter, nof_key_select, key_select_iter, lsb);
                }
                if (num_bits_not_on_key != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "EFES key_select_properties stage %d field_io %d %d key selects, "
                                 "but %d isn't empty num_bits_not_on_key %d.\r\n",
                                 stage_iter, field_io_iter, nof_key_select, key_select_iter, num_bits_not_on_key);
                }
            }
            if ((nof_key_select != 0) && (num_bits_efes != 64))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "In stage %d field_io %d there are %d key selects, "
                             "but number of bits in EFES key select is %d.\r\n",
                             stage_iter, field_io_iter, nof_key_select, num_bits_efes);
            }
            if ((nof_key_select != 0) && stage_iter == DNX_FIELD_STAGE_IPMF1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "EFES key_select_properties stage iPMF1 has %d key selects for field_io %d, "
                             "expected only for iPMF2.\r\n", nof_key_select, field_io_iter);
            }
            for (key_select_iter = 0; key_select_iter < nof_key_select; key_select_iter++)
            {
                uint32 num_bits = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                                field_io_iter)->num_bits
                    [key_select_iter];
                uint32 lsb = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                           field_io_iter)->lsb[key_select_iter];
                uint32 num_bits_not_on_key = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                                           field_io_iter)->
                    num_bits_not_on_key[key_select_iter];
                uint32 key_select = dnx_data_field.efes.key_select_properties_get(unit, stage_iter,
                                                                                  field_io_iter)->key_select
                    [key_select_iter];
                if (((int) num_bits < 0) || ((int) lsb < 0) || ((int) num_bits_not_on_key < 0))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %d field_io %d key select number %d (%d) has num_bits %d, "
                                 "lsb %d and num_bits_not_on_key %d.\r\n",
                                 stage_iter, field_io_iter, key_select_iter, key_select,
                                 num_bits, lsb, num_bits_not_on_key);
                }
                if (num_bits > num_bits_efes)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %d field_io %d key select number %d (%d) has number of bits %d, "
                                 "above maximum %d.\r\n",
                                 stage_iter, field_io_iter, key_select_iter, key_select,
                                 num_bits_not_on_key, num_bits_efes);
                }
                if ((num_bits + num_bits_not_on_key) > num_bits_efes)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %d field_io %d key select number %d (%d) has num_bits %d, "
                                 "num_bits_not_on_key %d, together %d, above maximum %d.\r\n",
                                 stage_iter, field_io_iter, key_select_iter, key_select,
                                 num_bits, num_bits_not_on_key, num_bits + num_bits_not_on_key, num_bits_efes);
                }
                if ((num_bits_not_on_key != 0) && (lsb != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %d field_io %d key select number %d (%d) has "
                                 "num_bits_not_on_key %d and lsb %d. "
                                 "If we start not at the beginning of the key select, we expect the key select to "
                                 "contain the beginning of the field IO.\r\n",
                                 stage_iter, field_io_iter, key_select_iter, key_select, num_bits_not_on_key, lsb);
                }
                {
                    uint8 is_enum_illegal;
                    dbal_fields_e key_select_dbal_field_id;
                    switch (stage_iter)
                    {
                        case DNX_FIELD_STAGE_IPMF2:
                        {
                            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_A_FES_KEY_SELECT;
                            break;
                        }
                        case DNX_FIELD_STAGE_IPMF3:
                        {
                            key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_B_FES_KEY_SELECT;
                            break;
                        }
                        case DNX_FIELD_STAGE_EPMF:
                        {

                            key_select_dbal_field_id = DBAL_FIELD_FIELD_E_PMF_FES_KEY_SELECT;
                            break;
                        }
                        default:
                        {
                            SHR_ERR_EXIT(_SHR_E_CONFIG, "Illegal stage %d for EFES.\r\n", stage_iter);
                        }
                    }
                    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                                    (unit, key_select_dbal_field_id, key_select, &is_enum_illegal));
                    if (is_enum_illegal)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "In stage %d field_io %d key select number %d (%d) is invalid.\r\n",
                                     stage_iter, field_io_iter, key_select_iter, key_select);
                    }
                }
            }
        }
    }

    /*
     * FEM
     */
    for (field_io_iter = 0; field_io_iter < DNX_DATA_MAX_FIELD_EFES_MAX_NOF_FIELD_IOS; field_io_iter++)
    {
        const uint8 *num_bits_p = dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->num_bits;
        for (nof_key_select = 0; (nof_key_select < DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO) &&
             (num_bits_p[nof_key_select] != 0); nof_key_select++);
        if (nof_key_select > fem_max_nof_key_selects)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "FEM key_select_properties field_io %d %d key_selects, maximum %d.\r\n",
                         field_io_iter, nof_key_select, fem_max_nof_key_selects);
        }
        for (key_select_iter = nof_key_select;
             key_select_iter < DNX_DATA_MAX_FIELD_FEM_MAX_NOF_KEY_SELECTS_PER_FIELD_IO; key_select_iter++)
        {
            uint32 num_bits =
                dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->num_bits[key_select_iter];
            uint32 lsb = dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->lsb[key_select_iter];
            uint32 num_bits_not_on_key =
                dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->num_bits_not_on_key[key_select_iter];
            uint32 key_select =
                dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->key_select[key_select_iter];
            if (num_bits != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "FEM key_select_properties field_io %d %d key selects, "
                             "but %d isn't empty num_bits %d.\r\n",
                             field_io_iter, nof_key_select, key_select_iter, num_bits);
            }
            if (key_select != (uint8) (-1))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "FEM key_select_properties field_io %d %d key selects, "
                             "but %d isn't empty key_select %d.\r\n",
                             field_io_iter, nof_key_select, key_select_iter, key_select);
            }
            if (lsb != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "FEM key_select_properties field_io %d %d key selects, "
                             "but %d isn't empty lsb %d.\r\n", field_io_iter, nof_key_select, key_select_iter, lsb);
            }
            if (num_bits_not_on_key != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "FEM key_select_properties field_io %d %d key selects, "
                             "but %d isn't empty num_bits_not_on_key %d.\r\n",
                             field_io_iter, nof_key_select, key_select_iter, num_bits_not_on_key);
            }
        }
        if ((nof_key_select != 0) && (num_bits_fem != 32))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "In field_io %d there are %d key selects, "
                         "but number of bits in FEM key select is %d.\r\n",
                         field_io_iter, nof_key_select, num_bits_fem);
        }
        for (key_select_iter = 0; key_select_iter < nof_key_select; key_select_iter++)
        {
            uint32 num_bits =
                dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->num_bits[key_select_iter];
            uint32 lsb = dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->lsb[key_select_iter];
            uint32 num_bits_not_on_key =
                dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->num_bits_not_on_key[key_select_iter];
            uint32 key_select =
                dnx_data_field.fem.key_select_properties_get(unit, field_io_iter)->key_select[key_select_iter];
            if (((int) num_bits < 0) || ((int) lsb < 0) || ((int) num_bits_not_on_key < 0))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "In field_io %d key select number %d (%d) has num_bits %d, "
                             "lsb %d and num_bits_not_on_key %d.\r\n",
                             field_io_iter, key_select_iter, key_select, num_bits, lsb, num_bits_not_on_key);
            }
            if (num_bits > num_bits_fem)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "In field_io %d key select number %d (%d) has number of bits %d, "
                             "above maximum %d.\r\n",
                             field_io_iter, key_select_iter, key_select, num_bits_not_on_key, num_bits_fem);
            }
            if ((num_bits + num_bits_not_on_key) > num_bits_fem)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "In field_io %d key select number %d (%d) has num_bits %d, "
                             "num_bits_not_on_key %d, together %d, above maximum %d.\r\n",
                             field_io_iter, key_select_iter, key_select,
                             num_bits, num_bits_not_on_key, num_bits + num_bits_not_on_key, num_bits_fem);
            }
            if ((num_bits_not_on_key != 0) && (lsb != 0))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "In field_io %d key select number %d (%d) has "
                             "num_bits_not_on_key %d and lsb %d. "
                             "If we start not at the beginning of the key select, we expect the key select to "
                             "contain the beginning of the field IO.\r\n",
                             field_io_iter, key_select_iter, key_select, num_bits_not_on_key, lsb);
            }
            {
                uint8 is_enum_illegal;
                dbal_fields_e key_select_dbal_field_id = DBAL_FIELD_FIELD_PMF_A_FEM_KEY_SELECT;
                SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value
                                (unit, key_select_dbal_field_id, key_select, &is_enum_illegal));
                if (is_enum_illegal)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In field_io %d key select number %d (%d) is invalid.\r\n",
                                 field_io_iter, key_select_iter, key_select);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Test the mapping for actions that override the field map data mapping.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
static shr_error_e
appl_dnx_action_tester_map_override(
    int unit)
{
    uint8 is_std_1;
    int cbts_action_mapping_override_done;
    dnx_field_action_t dnx_action_get;
    dnx_field_action_t dnx_action_compare;
    bcm_field_action_t bcm_action_get;
    uint8 bcm_action_found;
    shr_error_e rv;
    bsl_severity_t original_severity_fld_proc = BSL_INFO;
    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
    cbts_action_mapping_override_done = ((dnx_data_field.features.ecmp_tunnel_priority_get(unit)) && is_std_1);

    /*
     * Check the mappings of all GENERAL DATA actions in iPMF1/2.
     */
    /*
     * Check that GENERAL DATA3 is mapped to container.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                    (unit, DNX_FIELD_STAGE_IPMF1, bcmFieldActionContainer, &dnx_action_get));
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA3);
    if (dnx_action_get != dnx_action_compare)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "In iPMF1 bcmFieldActionContainer is mapped to 0x%x instead of 0x%x.\r\n",
                     dnx_action_get, dnx_action_compare);
    }
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get));
    if (bcm_action_get != bcmFieldActionContainer)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "In iPMF1 general data container 3 is mapped to %d instead of %d (bcmFieldActionContainer).\r\n",
                     bcm_action_get, bcmFieldActionContainer);
    }
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                    (unit, DNX_FIELD_STAGE_IPMF2, bcmFieldActionContainer, &dnx_action_get));
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_ACTION_GENERAL_DATA3);
    if (dnx_action_get != dnx_action_compare)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "In iPMF2 bcmFieldActionContainer is mapped to 0x%x instead of 0x%x.\r\n",
                     dnx_action_get, dnx_action_compare);
    }
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get));
    if (bcm_action_get != bcmFieldActionContainer)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "In iPMF1 general data container 3 is mapped to %d instead of %d (bcmFieldActionContainer).\r\n",
                     bcm_action_get, bcmFieldActionContainer);
    }
    /*
     * Check that GENERAL DATA 1 is not mapped.
     */
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA1);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF1 general data container 1 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_ACTION_GENERAL_DATA1);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF2 general data container 1 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    /*
     * Check that GENERAL DATA 2 is not mapped.
     */
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA2);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF1 general data container 2 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_ACTION_GENERAL_DATA2);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF2 general data container 2 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    /*
     * Check GENERAL DATA 0.
     */
    if (cbts_action_mapping_override_done)
    {
        /*
         * Check that GENERAL DATA3 is mapped to CBTS.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                        (unit, DNX_FIELD_STAGE_IPMF1, bcmFieldActionCbts, &dnx_action_get));
        dnx_action_compare =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_PEM_GENERAL_DATA_CBTS);
        if (dnx_action_get != dnx_action_compare)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "In iPMF1 bcmFieldActionCbts is mapped to 0x%x instead of 0x%x.\r\n",
                         dnx_action_get, dnx_action_compare);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                        (unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get));
        if (bcm_action_get != bcmFieldActionCbts)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "In iPMF1 general data container 0 is mapped to %d instead of %d (bcmFieldActionCbts).\r\n",
                         bcm_action_get, bcmFieldActionContainer);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                        (unit, DNX_FIELD_STAGE_IPMF2, bcmFieldActionCbts, &dnx_action_get));
        dnx_action_compare =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_ACTION_PEM_GENERAL_DATA_CBTS);
        if (dnx_action_get != dnx_action_compare)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "In iPMF2 bcmFieldActionCbts is mapped to 0x%x instead of 0x%x.\r\n",
                         dnx_action_get, dnx_action_compare);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                        (unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get));
        if (bcm_action_get != bcmFieldActionCbts)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "In iPMF1 general data container 0 is mapped to %d instead of %d (bcmFieldActionCbts).\r\n",
                         bcm_action_get, bcmFieldActionContainer);
        }
    }
    else
    {
        /*
         * Check that GENERAL DATA 0 is not mapped.
         */
        dnx_action_compare =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_ACTION_GENERAL_DATA0);
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                        (unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get, &bcm_action_found));
        if (bcm_action_found)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF1 general data container 0 is mapped to bcm action %d.\r\n",
                         bcm_action_get);
        }
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF1, dnx_action_compare, &bcm_action_get);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        if (rv != _SHR_E_INTERNAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
        }
        dnx_action_compare =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_ACTION_GENERAL_DATA0);
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                        (unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get, &bcm_action_found));
        if (bcm_action_found)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF2 general data container 0 is mapped to bcm action %d.\r\n",
                         bcm_action_get);
        }
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
        rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF2, dnx_action_compare, &bcm_action_get);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
        if (rv != _SHR_E_INTERNAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
        }
    }

    /*
     * For iPMF3 all GENERAL DATA actions are unmapped.
     */
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA0);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF3 general data container 0 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA1);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF3 general data container 1 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA2);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF3 general data container 2 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }
    dnx_action_compare =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF3, DNX_FIELD_ACTION_GENERAL_DATA3);
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm_int
                    (unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get, &bcm_action_found));
    if (bcm_action_found)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In iPMF3 general data container 3 is mapped to bcm action %d.\r\n",
                     bcm_action_get);
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_field_map_action_dnx_to_bcm(unit, DNX_FIELD_STAGE_IPMF3, dnx_action_compare, &bcm_action_get);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    if (rv != _SHR_E_INTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal mapping succeeded but non internal failed.\r\n");
    }

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

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR(DNX_DIAG_ACTION_OPTION_TEST_TYPE, action_test_type_name);
    SH_SAND_GET_ENUM(DNX_DIAG_ACTION_OPTION_TEST_STAGE, field_stage);
    /*
     * For now, a fail to match the input parameter result by just a message.
     * Procedure returns with 'success'.
     */
    if (sal_strncasecmp(action_test_type_name, TEST_TYPE_DATABASE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Tesing 'data base only' for stage %s. %s\r\n",
                    __func__, __LINE__, dnx_field_stage_text(unit, field_stage), EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_db_only(unit, field_stage));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Options %s %s. Testing 'data base only' returned 'success' flag.\r\n",
                    __func__, __LINE__, action_test_type_name, dnx_field_stage_text(unit, field_stage));
    }
    else if (sal_strncasecmp(action_test_type_name, TEST_TYPE_FES_CFG, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) ==
             0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'fes configuration' for stage %s %s\r\n",
                    __func__, __LINE__, dnx_field_stage_text(unit, field_stage), EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg(unit, field_stage));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Options %s %s. Testing 'fes configuration' returned 'success' flag.\r\n",
                    __func__, __LINE__, action_test_type_name, dnx_field_stage_text(unit, field_stage));
    }
    else if (sal_strncasecmp(action_test_type_name, TEST_TYPE_FES_CFG_MUL, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)
             == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'fes configuration multiple' %s%s\r\n", __func__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg_mul(unit));
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg_mul_back(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'fes configuration multiple' returned 'success' flag. %s\r\n",
                    __func__, __LINE__, action_test_type_name, EMPTY);
    }
    else if (sal_strncasecmp
             (action_test_type_name, TEST_TYPE_FES_CFG_INVALIDATE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'fes configuration invalidate priority' %s%s\r\n",
                    __func__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_fes_cfg_invalidate_prio(unit, field_stage));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'fes configuration invalidate priority' returned 'success' flag. %s\r\n",
                    __func__, __LINE__, action_test_type_name, EMPTY);
    }
    else if (sal_strncasecmp
             (action_test_type_name, TEST_TYPE_KEY_SELECT, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'key select configuration' %s%s\r\n", __func__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_key_select(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'key select configuration' returned 'success' flag. %s\r\n",
                    __func__, __LINE__, action_test_type_name, EMPTY);
    }
    else if (sal_strncasecmp
             (action_test_type_name, TEST_TYPE_MAP_OVERRIDE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Testing 'map override configuration' %s%s\r\n", __func__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(appl_dnx_action_tester_map_override(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %s. Testing 'map override configuration' returned 'success' flag. %s\r\n",
                    __func__, __LINE__, action_test_type_name, EMPTY);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s is not implemented. Illegal parameter. %s\r\n",
                    __func__, __LINE__, action_test_type_name, EMPTY);
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

/** \file diag_dnx_algo_field.c
 * $Id$
 *
 * 'Action' operations (for database create and entry add) procedures for DNX.
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
#include <bcm/error.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_algo_field.h"
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_action_access.h>

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
#define TEST_TYPE_FES_ALLOC   "FES_ALLOC"

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
 *   Keyword for test type on algo command.
 */
#define DNX_DIAG_ALGO_FIELD_OPTION_TEST_TYPE         "type"
/**
 * \brief
 *   Keyword for the stage for which database is to be tested.
 */
#define DNX_DIAG_ALGO_FIELD_OPTION_TEST_STAGE        "stage"
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
 *   List of tests for 'algo' field shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t sh_dnx_algo_field_tests[] = {
    {"DNX_action_for_FES_alloc_1", "type=fes_alloc stage=ipmf1", CTEST_POSTCOMMIT}
    ,
    {"DNX_action_for_FES_alloc_2", "type=fes_alloc stage=ipmf2", CTEST_POSTCOMMIT}
    ,
    {"DNX_action_for_FES_alloc_3", "type=fes_alloc stage=ipmf3", CTEST_POSTCOMMIT}
    ,
    {"DNX_action_for_FES_alloc_e", "type=fes_alloc stage=epmf",  CTEST_POSTCOMMIT}
    ,
    {NULL}
};
/**
 * \brief
 *   Options list for 'algo' field shell command
 * \remark
 *   Set the default value such that no test is carried out but procedure does return with 'success'
 *   This ensures success on regression and pre-commit:
 */
sh_sand_option_t dnx_algo_field_options[] = {
     /* Name */                             /* Type */           /* Description */                                       /* Default */
    {DNX_DIAG_ALGO_FIELD_OPTION_TEST_TYPE,  SAL_FIELD_TYPE_STR,  "Type of test (currently only FES_alloc is supported)", NULL        },
    {DNX_DIAG_ALGO_FIELD_OPTION_TEST_STAGE, SAL_FIELD_TYPE_ENUM, "Stage of test (ipmf1,ipmf2,ipmf3,epmf)",               "ipmf3", (void *)Field_stage_enum_table},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */
/*
 * }
 */

/**
 *  Database (TCAM) Look-ups leaf details
 */
sh_sand_man_t sh_dnx_algo_field_man = {
    "'Algo' related test utilities for field",
    "Activate 'Algo' related test utilities fore field processor.\r\n"
        "This covers at this point only FES allocation tests.\r\n",
    "ctest field action type=<fes_alloc> stage=<ipmf1 | ipmf2 | ipmf3 | epmf>",
    "type=fes_alloc\r\n" "TY=fes_alloc stage=ipmf1",
};

/**
 * \brief
 *    Updates the current state of the FESes accoriding to the allocation result.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] nof_fes2ms_instr_per_fes_quartet -
 *    The number of FES instructions in the FES quartet in the stage.
 * \param [in] fes_fg_in_p -
 *    The input to the FES allocation algorithm
 * \param [in] alloc_result_p -
 *    The result of the FES allocation algorithm
 * \param [in,out] current_state_p -
 *    The current state of the FESes to be updated.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * The procedure returns the algo_fiels_action SW state to the state it was when
 *     the procedure was called, if the test was successful.
 * \see
 *   * appl_dnx_algo_field_starter
 */
static shr_error_e
diag_dnx_algo_field_action_tester_fes_allocate_update_current_state(
    int unit,
    unsigned int nof_fes2ms_instr_per_fes_quartet,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_p)
{
    unsigned int fes_quartet_ndx;
    unsigned int fes_2msb_ndx;
    unsigned int fes_2msb_ndx_2;
    unsigned int nof_masks;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");
    SHR_NULL_CHECK(current_state_p, _SHR_E_PARAM, "current_state_p");

    for (fes_quartet_ndx = 0; fes_quartet_ndx < alloc_result_p->nof_fes_quartet_changes; fes_quartet_ndx++)
    {
        dnx_field_fes_id_t dest_fes_id = alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_id;

        /*
         * Count the number of distinct non zero masks.
         */
        nof_masks = 0;
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_fes2ms_instr_per_fes_quartet; fes_2msb_ndx++)
        {
            if (current_state_p->context_state.fes_id_info[dest_fes_id].fes_mask_id[fes_2msb_ndx] !=
                DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                nof_masks++;
                for (fes_2msb_ndx_2 = 0; fes_2msb_ndx_2 < fes_2msb_ndx; fes_2msb_ndx_2++)
                {
                    if (current_state_p->context_state.fes_id_info[dest_fes_id].fes_mask_id[fes_2msb_ndx] ==
                        current_state_p->context_state.fes_id_info[dest_fes_id].fes_mask_id[fes_2msb_ndx_2])
                    {
                        nof_masks--;
                        break;
                    }

                }
            }
        }
        if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc == DNX_ALGO_FIELD_ACTION_MOVEMENT)
        {
            dnx_field_fes_id_t source_fes_id = alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_id;
            /*
             * If the change is a movement, add the FES quartet to the destination.
             */
            current_state_p->context_state.fes_id_info[dest_fes_id].fes_pgm_id =
                alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id;
            sal_memcpy(current_state_p->context_state.fes_id_info[dest_fes_id].fes_action_type,
                       current_state_p->context_state.fes_id_info[source_fes_id].fes_action_type,
                       sizeof(current_state_p->context_state.fes_id_info[0].fes_action_type));
            sal_memcpy(current_state_p->context_state.fes_id_info[dest_fes_id].fes_mask_id,
                       alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id,
                       sizeof(current_state_p->context_state.fes_id_info[0].fes_mask_id));
            current_state_p->context_state.fes_id_info[dest_fes_id].is_shared =
                alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared;
            if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared == FALSE)
            {
                current_state_p->fes_state.fes_id_info[dest_fes_id].nof_fes_quartets++;
                current_state_p->fes_state.fes_id_info[dest_fes_id].nof_masks += nof_masks;
            }
            /*
             * If the change is a movement, remove the FES quartet from the source.
             */
            current_state_p->context_state.fes_id_info[source_fes_id].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
            if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared == FALSE)
            {
                current_state_p->fes_state.fes_id_info[source_fes_id].nof_fes_quartets--;
                current_state_p->fes_state.fes_id_info[source_fes_id].nof_masks -= nof_masks;
            }
        }
        else if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc >= 0)
        {
            unsigned int place_in_alloc = alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc;
            /*
             * If the change is a new FES quartet, add the FES quartet to the destination.
             */
            current_state_p->context_state.fes_id_info[dest_fes_id].fes_pgm_id =
                alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id;
            sal_memcpy(current_state_p->context_state.fes_id_info[dest_fes_id].fes_action_type,
                       fes_fg_in_p->fes_quartet[place_in_alloc].fes_action_type,
                       sizeof(current_state_p->context_state.fes_id_info[0].fes_action_type));
            sal_memcpy(current_state_p->context_state.fes_id_info[dest_fes_id].fes_mask_id,
                       alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id,
                       sizeof(current_state_p->context_state.fes_id_info[0].fes_mask_id));
            current_state_p->context_state.fes_id_info[dest_fes_id].is_shared =
                alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared;
            if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared == FALSE)
            {
                current_state_p->fes_state.fes_id_info[dest_fes_id].nof_fes_quartets++;
                current_state_p->fes_state.fes_id_info[dest_fes_id].nof_masks += nof_masks;
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal value for place in action: %d."
                         "Must be non-negative or %d.\r\n",
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc,
                         DNX_ALGO_FIELD_ACTION_MOVEMENT);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Performs operational testing for the code in the field actions module.
 *    Called from ctest field action.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] field_stage -
 *    For which the database is to be created and tested (IPMF1, IPMF2, IPMF3, EPMF).
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * The procedure returns the algo_fiels_action SW state to the state it was when
 *     the procedure was called, if the test was successful.
 * \see
 *   * appl_dnx_algo_field_starter
 */
static shr_error_e
diag_dnx_algo_field_action_tester_fes_allocate(
    int unit,
    dnx_field_stage_e field_stage)
{
    dnx_algo_field_action_ipmf2_t *old_sw_state_ipmf2_p;
    dnx_algo_field_action_ipmf3_t *old_sw_state_ipmf3_p;
    dnx_algo_field_action_epmf_t *old_sw_state_epmf_p;
    dnx_algo_field_action_fes_alloc_state_in_t *current_state_p;
    dnx_algo_field_action_fes_alloc_state_in_t *current_state_2_p;
    dnx_algo_field_action_fes_dealloc_state_in_t *current_state_dealloc_p;
    dnx_algo_field_action_fes_alloc_in_t *fes_fg_in_p;
    dnx_algo_field_action_fes_alloc_out_t *alloc_result_p;
    dnx_algo_field_action_fes_dealloc_out_t *dealloc_result_p;
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_quartet_ndx;
    unsigned int fes_pgm_ndx;
    dnx_field_context_t context_ndx;
    unsigned int fes_2msb_ndx;
    dnx_field_fes_mask_id_t mask_ndx;
    unsigned int fes_quartet_change_ndx;
    dnx_field_fes_id_t fes_id_2_ndx;
    dnx_field_fes_id_t fes_id_within_array_ndx;
    unsigned int skipping_zero_mask;
    dnx_algo_field_action_fes_state_t zero_fes_state;
    unsigned int first_allocable_mask_id;
    dnx_field_context_t context_id;
    dnx_field_group_t fg_id;
    int rv;
    bsl_severity_t original_severity_fldprocdnx;
    dnx_field_action_type_t invalid_action_type;
    int nof_fes_instruction_per_context;
    int nof_prog_per_fes;
    int nof_masks_per_fes;
    int nof_fes_id_per_array;
    int nof_fes_programs;
    int nof_fes_array;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize the pointers for memory allocation, so we won't try no free an uninitialized pointer.
     */
    old_sw_state_ipmf2_p = (dnx_algo_field_action_ipmf2_t *) NULL;
    old_sw_state_ipmf3_p = (dnx_algo_field_action_ipmf3_t *) NULL;
    old_sw_state_epmf_p = (dnx_algo_field_action_epmf_t *) NULL;
    current_state_p = (dnx_algo_field_action_fes_alloc_state_in_t *) NULL;
    current_state_2_p = (dnx_algo_field_action_fes_alloc_state_in_t *) NULL;
    fes_fg_in_p = (dnx_algo_field_action_fes_alloc_in_t *) NULL;
    alloc_result_p = (dnx_algo_field_action_fes_alloc_out_t *) NULL;
    current_state_dealloc_p = (dnx_algo_field_action_fes_dealloc_state_in_t *) NULL;
    dealloc_result_p = (dnx_algo_field_action_fes_dealloc_out_t *) NULL;

    /*
     * Obtain the invalid action of the stage.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &invalid_action_type));

    /*
     * Get the relevant DNX data for the stage.
     */
    nof_fes_instruction_per_context =
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
    nof_prog_per_fes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
    nof_masks_per_fes = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
    nof_fes_id_per_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array;
    nof_fes_programs = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs;
    nof_fes_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array;

    /*
     * Allocate memory space for saving the SW state and save it,
     * so that we don't overwrite it.
     * Notice that we assume the code only changes the SW state of the stage it's given.
     * An error that leads to writing to the SW state of a different stage will leak out of this test.
     * Also get the maximum number of FESes per array for the stage.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_ALLOC(old_sw_state_ipmf2_p, sizeof(dnx_algo_field_action_ipmf2_t),
                      "old_sw_state_ipmf2", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.get(unit, old_sw_state_ipmf2_p));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_ALLOC(old_sw_state_ipmf3_p, sizeof(dnx_algo_field_action_ipmf3_t),
                      "old_sw_state_ipmf3", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.get(unit, old_sw_state_ipmf3_p));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_ALLOC(old_sw_state_epmf_p, sizeof(dnx_algo_field_action_epmf_t),
                      "old_sw_state_epmf", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.get(unit, old_sw_state_epmf_p));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Initialize zero_fes_state and reset the SW state with it.
     */
    for (fes_pgm_ndx = 0; fes_pgm_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_ndx++)
    {
        zero_fes_state.field_group[fes_pgm_ndx] = DNX_FIELD_GROUP_INVALID;
        zero_fes_state.place_in_fg[fes_pgm_ndx] = 0;
    }
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        zero_fes_state.mask_is_alloc[mask_ndx] = 0;
    }
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }
    /*
     * Allocate Memory for current_state, fes_fg_in and alloc_result.
     */
    SHR_ALLOC_SET_ZERO(current_state_p, sizeof(dnx_algo_field_action_fes_alloc_state_in_t),
                       "current_state_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(current_state_2_p, sizeof(dnx_algo_field_action_fes_alloc_state_in_t),
                       "current_state_2_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(fes_fg_in_p, sizeof(dnx_algo_field_action_fes_alloc_in_t),
                       "fes_fg_in_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(alloc_result_p, sizeof(dnx_algo_field_action_fes_alloc_out_t),
                       "alloc_result_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(current_state_dealloc_p, sizeof(dnx_algo_field_action_fes_dealloc_state_in_t),
                       "current_state_dealloc_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dealloc_result_p, sizeof(dnx_algo_field_action_fes_dealloc_out_t),
                       "dealloc_result_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    /*
     * Set the first mask ID that would be allocated to.
     */
    if (DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID == 0)
    {
        first_allocable_mask_id = 1;
    }
    else
    {
        first_allocable_mask_id = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID - 1;
    }
    /*
     * Test 1. Allocation sanity.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 1.1
     * Build a request for context ID 1 and field group 11 with one FES quartet and a numerical priority 10.
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.1 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.1 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.2
     * Build a request for context ID 2 and field group 11 with one FES quartet and a numerical priority 20.
     * use action type 1 and no masks.
     * We expect it to be shared with the previous request.
     */
    context_id = 2;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared == FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.2 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.2 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * We do not update the current state of the new context, just mark the sharing for context 1.
     */
    current_state_p->context_state.fes_id_info[alloc_result_p->fes_quartet_change[0].dest_fes_id].is_shared = TRUE;

    /*
     * 1.3
     * Build a request for context ID 1 and field group 12 with one FES quartet and a numerical priority 5.
     * We use action type 1 and no masks.
     * We expect it to push forward the FES quartet with the same actions and higher priority.
     * We expect them not to be shared.
     */
    context_id = 1;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 5);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].source_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[0].source_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].source_is_shared == FALSE ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 2 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.3 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].source_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
            || alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID
            || alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.3 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.4
     * Build a request for context ID 1 and field group 13 with one FES quartet and a numerical priority 30.
     * We use action type 1 and no masks.
     * We expect it to come after the previous allocations.
     */
    context_id = 1;
    fg_id = 13;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 30);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.4 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.4 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.5
     * Build a request for context ID 1 and field group 14 with one FES quartet and a numerical priority 4.
     * We use action type 2 and one mask used by FES instruction0 0.
     * We expect it to come after the previous allocations, since there is no contention.
     */
    context_id = 1;
    fg_id = 14;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].fes_mask_id[0] = first_allocable_mask_id;
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 4);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 3 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.5 of FES allocation.\r\n");
    }
    /*
     * Check the mask allocation.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID && fes_2msb_ndx != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.5 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] != first_allocable_mask_id
            && fes_2msb_ndx == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.5 of FES allocation. "
                         "Mask not allocated where it should be.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.6
     * Build a request for context ID 1 and field group 15 with one FES quartet and a numerical priority 5.
     * We use action type 2 and one mask used by FES instructions 2 and 3.
     * We expect it to come after the previous allocations.
     */
    context_id = 1;
    fg_id = 15;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].fes_mask_id[2] = first_allocable_mask_id;
    fes_fg_in_p->fes_quartet[0].fes_mask_id[3] = first_allocable_mask_id;
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 5);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 4 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.5 of FES allocation.\r\n");
    }
    /*
     * Check the mask allocation.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID && fes_2msb_ndx != 2 && fes_2msb_ndx != 3)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.6 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] != 1
            && (fes_2msb_ndx == 2 || fes_2msb_ndx == 3))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.6 of FES allocation. "
                         "Mask not allocated where it should be.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.7
     * Build a request for context ID 1 and field group 16 with one FES quartet and a numerical priority 4.
     * We use action type 3 and the maximum number of masks.
     * We expect it to come after the previous allocations, since there is no contention.
     */
    context_id = 1;
    fg_id = 16;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 3;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    skipping_zero_mask = 0;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes && (fes_2msb_ndx < nof_masks_per_fes - 1); fes_2msb_ndx++)
    {
        if (fes_2msb_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            skipping_zero_mask = 1;
        }
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = fes_2msb_ndx + skipping_zero_mask;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 4);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 5 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.7 of FES allocation.\r\n");
    }
    /*
     * Check the mask allocation.
     */
    skipping_zero_mask = 0;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes && fes_2msb_ndx < (nof_masks_per_fes - 1); fes_2msb_ndx++)
    {
        if (fes_2msb_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            skipping_zero_mask = 1;
        }
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] != fes_2msb_ndx + skipping_zero_mask)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.7 of FES allocation. "
                         "Mask not allocated where it should be.\r\n");
        }
    }
    for (; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.7 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.8
     * Build a request for context ID 1 and field group 17 with one FES quartet and mandatory placement.
     * We use action type 4 and no masks.
     */
    context_id = 1;
    fg_id = 17;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 4;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_POSITION(0, 7);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 7 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.8 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.8 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.9
     * Build a request for context ID 1 and field group 18 with one FES quartet and mandatory placement.
     * We use action type 4 and no masks.
     * We expect it to work despite the contention, because it has contention with another FES quartet with mandatory
     * placement.
     */
    context_id = 1;
    fg_id = 18;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 4;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_POSITION(0, 8);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 8 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.9 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.9 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.10
     * Build a request for context ID 1 and field group 19 with one FES quartet with numerical priority.
     * We use action type 1 and no masks.
     * We expect it to work despite the contention, because it has contention with another FES quartet with mandatory
     * placement.
     */
    context_id = 1;
    fg_id = 19;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 35);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 6 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.10 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.10 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 1.11
     * Negative test
     * Build a request for context ID 1 and field group 20 with one FES quartet and mandatory placement.
     * We use action type 1 and no masks.
     * We expect it to fail, because it have contention with a FES quartet without mandatory placement.
     */
    context_id = 1;
    fg_id = 20;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_POSITION(0, 10);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 1.11, succeeded in allocating mismatching priority types with "
                     "contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 1.11: %d.\r\n", rv);
    }

    /*
     * 1.12
     * Negative test
     * Build a request for context ID 1 and field group 21 with one FES quartet and numerical priority.
     * We use action type 4 and no masks.
     * We expect it to fail, because it have contention with a FES quartet without mandatory placement.
     */
    context_id = 1;
    fg_id = 21;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 4;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 70);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 1.12, succeeded in allocating mismatching priority types with "
                     "contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 1.12: %d.\r\n", rv);
    }

    /*
     * 1.13
     * Negative test
     * Build a request for context ID 1 and field group 22 with one FES quartet and numerical priority.
     * We use action type 4 and 10 and no masks.
     * We expect it to fail, because it have contention with a FES quartet without mandatory placement,
     * and only one instruction with contention is enough.
     */
    context_id = 1;
    fg_id = 22;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 10;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].fes_action_type[2] = 4;
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 71);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 1.13, succeeded in allocating mismatching priority types with "
                     "contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 1.13: %d.\r\n", rv);
    }

    /*
     * 1.14
     * Negative test
     * Build a request for context ID 1 and field group 23 with two FES quartets,
     * one with numerical priority and the other with mandatory position.
     * We use action type 15 and 10 and no masks.
     * We expect it to fail, because it have contention and mismatching priorities within itself.
     * Note that we expect this test to change the SW state.
     */
    context_id = 1;
    fg_id = 23;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 15;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 15;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 72);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_POSITION(0, 10);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 1.14, succeeded in allocating mismatching priority types with "
                     "contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 1.14: %d.\r\n", rv);
    }

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 2. More allocation sanity.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 2.1
     * Build a request for context ID 1 and field group 11 with two FES quartets with different priorities.
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.1 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.1 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 2.2
     * Build a request for context ID 2 and field group 12 with three FES quartets to be places in the same FES IDs
     * as the previous request and one after.
     * use action type 5 and one masks.
     * Note that there should be no priority mismatch because it is a different context.
     */
    context_id = 2;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 3;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 5;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 5;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
        fes_fg_in_p->fes_quartet[2].fes_action_type[fes_2msb_ndx] = 5;
        fes_fg_in_p->fes_quartet[2].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_POSITION(0, 0);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    fes_fg_in_p->fes_quartet[2].priority = BCM_FIELD_ACTION_POSITION(0, 2);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 3 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 2 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1 ||
        alloc_result_p->fes_quartet_change[2].dest_fes_id != 2 ||
        alloc_result_p->fes_quartet_change[2].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[2].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[2].place_in_alloc != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.2 of FES allocation.\r\n");
    }
    /*
     * Check the mask allocation.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] != first_allocable_mask_id ||
            alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] != first_allocable_mask_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 1.5 of FES allocation. "
                         "Mask not allocated where it should be.\r\n");
        }
    }
    /*
     * Do not update the current state.
     */

    /*
     * 2.3
     * Build a request for context ID 1 and field group 12 with one FES quartets with priority 15.
     * We use action type 1 and maximum number of masks.
     * We expect the second FES quartet to be pushed two FES IDs higher.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    skipping_zero_mask = 0;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes && (fes_2msb_ndx < nof_masks_per_fes - 1); fes_2msb_ndx++)
    {
        if (fes_2msb_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            skipping_zero_mask = 1;
        }
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = fes_2msb_ndx + skipping_zero_mask;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 15);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].source_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 4 ||
        alloc_result_p->fes_quartet_change[0].source_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].source_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 3 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.3 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated for the movement.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.3 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Check the mask allocation for the new FES quartet.
     */
    skipping_zero_mask = 0;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes && fes_2msb_ndx < (nof_masks_per_fes - 1); fes_2msb_ndx++)
    {
        if (fes_2msb_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            skipping_zero_mask = 1;
        }
        if (alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] != fes_2msb_ndx + skipping_zero_mask)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.3 of FES allocation. "
                         "Mask not allocated where it should be.\r\n");
        }
    }
    for (; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.3 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 2.4
     * Build a request for context ID 3 and field group 20 with one less than the maximal number of FES IDs.
     * use action type 30 and no masks.
     */
    context_id = 3;
    fg_id = 20;
    fes_fg_in_p->nof_fes_quartets = nof_fes_instruction_per_context - 1;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
        {
            fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_action_type[fes_2msb_ndx] = 30;
            fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes_2msb_ndx] =
                DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        }
        fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority =
            BCM_FIELD_ACTION_PRIORITY((fes_quartet_ndx / (nof_fes_id_per_array)) * 2,
                                      fes_quartet_ndx % (nof_fes_id_per_array));
    }
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != fes_fg_in_p->nof_fes_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.4 of FES allocation.\r\n");
    }
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_id != fes_quartet_ndx ||
            alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id > 3 ||
            alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared != FALSE ||
            alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc != fes_quartet_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.4 of FES allocation.\r\n");
        }
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
        {
            if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id[fes_2msb_ndx] !=
                DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 2.4 of FES allocation. "
                             "Mask allocated when it shouldn't have been.\r\n");
            }
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_2_p));
    /*
     * 2.5
     * Negative test
     * Build a request for context ID 3 and field group 21 with two FES quartets.
     * We expect it to faild because we don't have enough FES IDs.
     */
    context_id = 3;
    fg_id = 23;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 31;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 31;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_DONT_CARE;
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_2_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 2.5, succeeded in allocating beyond capacity.\r\n");
    }
    else if (rv != _SHR_E_RESOURCE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 2.5: %d.\r\n", rv);
    }

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 3. Capacity testing.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 3.1
     * Build requests to fill all FES instructions.
     */
    for (fes_pgm_ndx = 0; fes_pgm_ndx < nof_fes_programs - 1; fes_pgm_ndx++)
    {
        context_id = fes_pgm_ndx;
        fg_id = fes_pgm_ndx;
        fes_fg_in_p->nof_fes_quartets = nof_fes_instruction_per_context;
        for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
        {
            for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
            {
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_action_type[fes_2msb_ndx] =
                    fes_pgm_ndx + invalid_action_type + 2;
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes_2msb_ndx] =
                    DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
            }
            fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority =
                BCM_FIELD_ACTION_PRIORITY((fes_quartet_ndx /
                                           (dnx_data_field.stage.
                                            stage_info_get(unit, field_stage)->nof_fes_id_per_array)) * 2,
                                          fes_quartet_ndx %
                                          (dnx_data_field.stage.
                                           stage_info_get(unit, field_stage)->nof_fes_id_per_array));
        }
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                        (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE,
                         alloc_result_p));
    }
    /*
     * Check that one of the FES quarteta was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != fes_fg_in_p->nof_fes_quartets ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != dnx_data_field.stage.stage_info_get(unit,
                                                                                                     field_stage)->nof_fes_programs
        - 1 || alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[0].place_in_alloc != 0
        || alloc_result_p->fes_quartet_change[5].dest_fes_id != 5
        || alloc_result_p->fes_quartet_change[5].dest_fes_pgm_id != dnx_data_field.stage.stage_info_get(unit,
                                                                                                        field_stage)->nof_fes_programs
        - 1 || alloc_result_p->fes_quartet_change[5].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[5].place_in_alloc != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 3.1 of FES allocation.\r\n");
    }
    /*
     * Do not pdate the current state.
     */

    /*
     * 3.2
     * Negative test
     * Build a request for one FES quartet.
     * We expect it to fail, due to lack of free FES quartets.
     */
    context_id = fes_pgm_ndx;
    fg_id = fes_pgm_ndx;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = fes_pgm_ndx + invalid_action_type + 2;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 3.2, succeeded in allocating beyond capacity.\r\n");
    }
    else if (rv != _SHR_E_RESOURCE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 3.2: %d.\r\n", rv);
    }

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 4. More capacity testing.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 4.1
     * Build requests to fill all FES instructions except the top FES program ID.
     */
    for (fes_pgm_ndx = 0; fes_pgm_ndx < nof_fes_programs - 2; fes_pgm_ndx++)
    {
        context_id = fes_pgm_ndx;
        fg_id = fes_pgm_ndx;
        fes_fg_in_p->nof_fes_quartets = nof_fes_instruction_per_context;
        for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
        {
            for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
            {
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_action_type[fes_2msb_ndx] =
                    fes_pgm_ndx + invalid_action_type + 2;
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes_2msb_ndx] =
                    DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
            }
            fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority =
                BCM_FIELD_ACTION_PRIORITY((fes_quartet_ndx /
                                           (dnx_data_field.stage.
                                            stage_info_get(unit, field_stage)->nof_fes_id_per_array)) * 2,
                                          fes_quartet_ndx %
                                          (dnx_data_field.stage.
                                           stage_info_get(unit, field_stage)->nof_fes_id_per_array));
        }
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                        (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE,
                         alloc_result_p));
    }
    /*
     * Check that one of the FES quarteta was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != fes_fg_in_p->nof_fes_quartets ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != dnx_data_field.stage.stage_info_get(unit,
                                                                                                     field_stage)->nof_fes_programs
        - 2 || alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 4.1 of FES allocation.\r\n");
    }
    /*
     * Do not update the current state.
     */

    /*
     * 4.2
     * Fill all but one of the FES quartets in the top FES program ID.
     */
    for (; fes_pgm_ndx < nof_fes_programs - 1; fes_pgm_ndx++)
    {
        context_id = fes_pgm_ndx;
        fg_id = fes_pgm_ndx;
        fes_fg_in_p->nof_fes_quartets = nof_fes_instruction_per_context - 1;
        for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
        {
            for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
            {
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_action_type[fes_2msb_ndx] =
                    fes_pgm_ndx + invalid_action_type + 2;
                fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id[fes_2msb_ndx] =
                    DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
            }
            fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority =
                BCM_FIELD_ACTION_PRIORITY((fes_quartet_ndx /
                                           (dnx_data_field.stage.
                                            stage_info_get(unit, field_stage)->nof_fes_id_per_array)) * 2,
                                          fes_quartet_ndx %
                                          (dnx_data_field.stage.
                                           stage_info_get(unit, field_stage)->nof_fes_id_per_array));
        }
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                        (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE,
                         alloc_result_p));
    }
    /*
     * Do not update the current state.
     */

    /*
     * 4.3
     * Fill the last FES quartet.
     */
    context_id = fes_pgm_ndx + 1;
    fg_id = fes_pgm_ndx + 1;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = fes_pgm_ndx + invalid_action_type + 2;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Do not update the current state.
     */

    /*
     * 4.4
     * Negative test
     * Build a request for one FES quartet.
     * We expect it to fail, due to lack of free FES quartets.
     */
    context_id = fes_pgm_ndx + 2;
    fg_id = fes_pgm_ndx + 2;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = fes_pgm_ndx + invalid_action_type + 2;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 4.4, succeeded in allocating beyond capacity.\r\n");
    }
    else if (rv != _SHR_E_RESOURCE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 4.4: %d.\r\n", rv);
    }

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 5. Deallocation.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }
    /*
     * Create an empty deallocation state.
     */
    sal_memset(current_state_dealloc_p, 0x0, sizeof(*current_state_dealloc_p));
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_dealloc_p->context_state.fes_pgm_id[fes_id_ndx] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    }

    /*
     * 5.1
     * Build a request for context ID 1 and field group 11 with two FES quartet and ascending numerical priority.
     * Use different action types and one FES quartet has a mask.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check the FES allocation
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.1 of FES allocation.\r\n");
    }
    /*
     * Check mask allocation
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.1 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
        if (alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] != first_allocable_mask_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.1 of FES allocation. "
                         "Mask not allocated where it should have.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 5.2
     * Build a request for context ID 2 and field group 11 with two FES quartets and descending numerical priority.
     * Use different action types and one FES quartet has a mask.
     * We expect it to be shared with the previous request, despite the descending priority.
     */
    context_id = 2;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check the FES allocation
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared == FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared == FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.2 of FES allocation.\r\n");
    }
    /*
     * Mask allocation is irrelevant since both FES quartets are shared.
     */
    /*
     * We do not update the current state of the new context, just mark the sharing for context 1.
     */
    current_state_p->context_state.fes_id_info[alloc_result_p->fes_quartet_change[0].dest_fes_id].is_shared = TRUE;
    current_state_p->context_state.fes_id_info[alloc_result_p->fes_quartet_change[1].dest_fes_id].is_shared = TRUE;

    /*
     * 5.3
     * Build a request for context ID 3 and field group 12 with two FES quartets with a mask in the same location as 
     * the previous request, in reverse order.
     * We expect the masks to not be deleted later.
     */
    context_id = 3;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 5;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 5;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
    }
    fes_fg_in_p->fes_quartet[0].priority =
        BCM_FIELD_ACTION_POSITION(0, alloc_result_p->fes_quartet_change[1].dest_fes_id);
    fes_fg_in_p->fes_quartet[1].priority =
        BCM_FIELD_ACTION_POSITION(0, alloc_result_p->fes_quartet_change[0].dest_fes_id);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check the FES allocation
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 2 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.3 of FES allocation.\r\n");
    }
    /*
     * Check mask allocation
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] != first_allocable_mask_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.3 of FES allocation. "
                         "Mask not allocated where it should have.\r\n");
        }
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] == first_allocable_mask_id ||
            alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] ==
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.3 of FES allocation. "
                         "Mask not allocated where it should have.\r\n");
        }
    }
    for (fes_2msb_ndx = 1; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx - 1])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.3 of FES allocation. "
                         "Not all masks in FES quartet are the same.\r\n");
        }
        if (alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx] !=
            alloc_result_p->fes_quartet_change[1].dest_fes_mask_id[fes_2msb_ndx - 1])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.3 of FES allocation. "
                         "Not all masks in FES quartet are the same.\r\n");
        }
    }
    /*
     * We do not update the current state of the new context.
     */

    /*
     * 5.4
     * Build a request for context ID 1 and field group 12 with one FES quartet.
     */
    context_id = 1;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 7;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check the FES allocation
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.4 of FES allocation.\r\n");
    }
    /*
     * Check mask allocation
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.1 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 5.5
     * Build a current state for deallocation for context ID 1 and delete field group 11.
     * We expect the FES conmfiguration to not be deleted since it is shared.
     */
    current_state_dealloc_p->context_state.fes_pgm_id[0] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[0] = 2;
    current_state_dealloc_p->context_state.mask_nof_refs[0][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[1] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[1] = 2;
    current_state_dealloc_p->context_state.mask_nof_refs[1][1] = 2;
    /*
     * Deallocate
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc
                    (unit, field_stage, 11, 1, DNX_FIELD_EFES_ID_INVALID, current_state_dealloc_p, dealloc_result_p));
    /*
     * check the output.
     */
    if (dealloc_result_p->belongs_to_fg[0] == FALSE ||
        dealloc_result_p->belongs_to_fg[1] == FALSE ||
        dealloc_result_p->belongs_to_fg[2] != FALSE ||
        dealloc_result_p->belongs_to_fg[3] != FALSE ||
        dealloc_result_p->belongs_to_fg[4] != FALSE ||
        dealloc_result_p->belongs_to_fg[5] != FALSE ||
        dealloc_result_p->belongs_to_fg[6] != FALSE ||
        dealloc_result_p->fes_pgm_id_to_delete[0] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[1] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[2] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[3] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[4] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[5] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[6] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.5 of FES allocation.\r\n");
    }
    /*
     * Check the deleted masks.
     */
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        if (dealloc_result_p->delete_mask[0][mask_ndx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.5 of FES allocation.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    current_state_p->context_state.fes_id_info[0].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    current_state_p->context_state.fes_id_info[1].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;

    /*
     * 5.6
     * Build a current state for deallocation for context ID 1 and delete field group 11 again.
     */
    current_state_dealloc_p->context_state.fes_pgm_id[0] = 0;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[0] = 0;
    current_state_dealloc_p->context_state.mask_nof_refs[0][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[1] = 0;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[1] = 0;
    current_state_dealloc_p->context_state.mask_nof_refs[1][1] = 0;
    /*
     * Deallocate
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc
                    (unit, field_stage, 11, 1, DNX_FIELD_EFES_ID_INVALID, current_state_dealloc_p, dealloc_result_p));
    /*
     * check the output.
     */
    if (dealloc_result_p->fes_pgm_id_to_delete[0] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[1] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[2] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[3] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[4] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[5] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[1] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.6 of FES allocation.\r\n");
    }
    /*
     * Check the deleted masks.
     */
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        if (dealloc_result_p->delete_mask[0][mask_ndx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.6 of FES allocation.\r\n");
        }
    }

    /*
     * 5.7
     * Build a request for context ID 1 and field group 11 with two FES quartet and ascending numerical priority.
     * Use different action types and one FES quartet has a mask.
     * We rewrite the field group, only changing the priority.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = first_allocable_mask_id;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 12);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check the FES allocation
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared == FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared == FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 5.7 of FES allocation.\r\n");
    }
    /*
     * Mask allocation is irrelevant, since it is shared.
     */
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 5.8
     * Build a current state for deallocation for context ID 1 and delete field group 11 once more.
     * This time we input fes_quartet_nof_refs to pretend it isn't shared.
     */
    current_state_dealloc_p->context_state.fes_pgm_id[0] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[0] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[0][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[1] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[1] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[1][1] = 1;
    /*
     * Deallocate
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc
                    (unit, field_stage, 11, 1, DNX_FIELD_EFES_ID_INVALID, current_state_dealloc_p, dealloc_result_p));
    /*
     * check the output.
     */
    if (dealloc_result_p->belongs_to_fg[0] == FALSE ||
        dealloc_result_p->belongs_to_fg[1] == FALSE ||
        dealloc_result_p->belongs_to_fg[2] != FALSE ||
        dealloc_result_p->belongs_to_fg[3] != FALSE ||
        dealloc_result_p->belongs_to_fg[4] != FALSE ||
        dealloc_result_p->belongs_to_fg[5] != FALSE ||
        dealloc_result_p->belongs_to_fg[6] != FALSE ||
        dealloc_result_p->fes_pgm_id_to_delete[0] != 1 ||
        dealloc_result_p->fes_pgm_id_to_delete[1] != 1 ||
        dealloc_result_p->fes_pgm_id_to_delete[2] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[3] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[4] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[5] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[6] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.8 of FES allocation.\r\n");
    }
    /*
     * Check the deleted masks.
     */
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        if (dealloc_result_p->delete_mask[0][mask_ndx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.8 of FES allocation.\r\n");
        }
        if (dealloc_result_p->delete_mask[1][mask_ndx] && mask_ndx != first_allocable_mask_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.8 of FES allocation.\r\n");
        }
        if (dealloc_result_p->delete_mask[1][mask_ndx] == FALSE && mask_ndx == first_allocable_mask_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 5.8 of FES allocation.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    current_state_dealloc_p->context_state.fes_pgm_id[0] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    current_state_dealloc_p->context_state.fes_pgm_id[4] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 6. More deallocation.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 6.1
     * Build a request for context ID 1 and field group 11 with two FES quartets and a ascending numerical priority.
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 15);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.1 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.1 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 6.2
     * Build a request for context ID 1 and field group 12 with two FES quartets and a ascending numerical priority.
     * use action type 2 and no masks.
     */
    context_id = 1;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 15);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 3 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.2 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.2 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 6.3
     * Build a request for context ID 1 and field group 13 with one FES quartet 
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 13;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 12);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 2 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 4 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT ||
        alloc_result_p->fes_quartet_change[1].dest_fes_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[1].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.3 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.3 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * 6.4
     * Build a current state for deallocation for context ID 1 and delete field group 11.
     * We expect the FES conmfiguration to not be deleted since it is shared.
     */
    current_state_dealloc_p->context_state.fes_pgm_id[0] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[0] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[0][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[1] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[1] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[1][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[2] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[2] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[2][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[3] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[3] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[3][1] = 0;
    current_state_dealloc_p->context_state.fes_pgm_id[4] = 1;
    current_state_dealloc_p->context_state.fes_quartet_nof_refs[4] = 1;
    current_state_dealloc_p->context_state.mask_nof_refs[4][1] = 0;
    /*
     * Deallocate
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc
                    (unit, field_stage, 11, 1, DNX_FIELD_EFES_ID_INVALID, current_state_dealloc_p, dealloc_result_p));
    /*
     * check the output.
     */
    if (dealloc_result_p->belongs_to_fg[0] == FALSE ||
        dealloc_result_p->belongs_to_fg[1] != FALSE ||
        dealloc_result_p->belongs_to_fg[2] != FALSE ||
        dealloc_result_p->belongs_to_fg[3] != FALSE ||
        dealloc_result_p->belongs_to_fg[4] == FALSE ||
        dealloc_result_p->belongs_to_fg[5] != FALSE ||
        dealloc_result_p->belongs_to_fg[6] != FALSE ||
        dealloc_result_p->fes_pgm_id_to_delete[0] != 1 ||
        dealloc_result_p->fes_pgm_id_to_delete[1] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[2] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[3] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[4] != 1 ||
        dealloc_result_p->fes_pgm_id_to_delete[5] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        dealloc_result_p->fes_pgm_id_to_delete[6] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 6.4 of FES allocation.\r\n");
    }
    /*
     * Check the deleted masks.
     */
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        if (dealloc_result_p->delete_mask[0][mask_ndx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in deallocation test 6.4 of FES allocation.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    current_state_p->context_state.fes_id_info[0].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    current_state_p->context_state.fes_id_info[4].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;

    /*
     * 6.5
     * Build a request for context ID 1 and field group 14 with one FES quartet 
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 14;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 11);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_id != 0 ||
        alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1 ||
        alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE ||
        alloc_result_p->fes_quartet_change[0].place_in_alloc != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.5 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 6.5 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 7. Other FES array.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 7.1
     * Build a request for context ID 1 and field group 11 with two FES quartets with different priorities.
     * use action type 1 and no masks, in the second array
     */
    if (nof_fes_array > 1)
    {
        context_id = 1;
        fg_id = 11;
        fes_fg_in_p->nof_fes_quartets = 2;
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
        {
            fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
            fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
            fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
            fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        }
        fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(2, 10);
        fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(2, 20);
        /*
         * Allocate.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                        (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE,
                         alloc_result_p));
        /*
         * Check that one FES quartet was allocated in the right place.
         */
        if (alloc_result_p->nof_fes_quartet_changes != 2 ||
            alloc_result_p->fes_quartet_change[0].dest_fes_id != dnx_data_field.stage.stage_info_get(unit,
                                                                                                     field_stage)->nof_fes_id_per_array
            || alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1
            || alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE
            || alloc_result_p->fes_quartet_change[0].place_in_alloc != 0
            || alloc_result_p->fes_quartet_change[1].dest_fes_id != dnx_data_field.stage.stage_info_get(unit,
                                                                                                        field_stage)->nof_fes_id_per_array
            + 1 || alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1
            || alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE
            || alloc_result_p->fes_quartet_change[1].place_in_alloc != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 7.1 of FES allocation.\r\n");
        }
        /*
         * Check that no mask was allocated.
         */
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
        {
            if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
                DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 7.1 of FES allocation. "
                             "Mask allocated when it shouldn't have been.\r\n");
            }
        }
        /*
         * Update the current state.
         */
        SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                        (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_p));
    }

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 8. Mismatching priorites.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 8.1
     * Build a request for context ID 1 and field group 11 with two FES quartets with different priorities.
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 20000);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place. Includes one movement
     */
    if (alloc_result_p->nof_fes_quartet_changes != 3
        || alloc_result_p->fes_quartet_change[0].dest_fes_id != 0
        || alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 1
        || alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[0].place_in_alloc != 0
        || alloc_result_p->fes_quartet_change[1].dest_fes_id != 1
        || alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 1
        || alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[1].place_in_alloc != -1
        || alloc_result_p->fes_quartet_change[1].source_fes_id != 0
        || alloc_result_p->fes_quartet_change[1].source_fes_pgm_id != 1
        || alloc_result_p->fes_quartet_change[1].source_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[2].dest_fes_id != 0
        || alloc_result_p->fes_quartet_change[2].dest_fes_pgm_id != 1
        || alloc_result_p->fes_quartet_change[2].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[2].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 8.1 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 8.1 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }
    /*
     * Update the current state.
     */
    SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate_update_current_state
                    (unit, nof_prog_per_fes, fes_fg_in_p, alloc_result_p, current_state_2_p));

    /*
     * 8.2
     * Build a request for context ID 2 and field group 12 with two FES quartets with the same priorities as before.
     * use action type 1 and no masks.
     */
    context_id = 2;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 20000);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that one FES quartet was allocated in the right place.
     */
    if (alloc_result_p->nof_fes_quartet_changes != 3
        || alloc_result_p->fes_quartet_change[0].dest_fes_id != 0
        || alloc_result_p->fes_quartet_change[0].dest_fes_pgm_id != 2
        || alloc_result_p->fes_quartet_change[0].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[0].place_in_alloc != 0
        || alloc_result_p->fes_quartet_change[1].dest_fes_id != 1
        || alloc_result_p->fes_quartet_change[1].dest_fes_pgm_id != 2
        || alloc_result_p->fes_quartet_change[1].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[1].place_in_alloc != -1
        || alloc_result_p->fes_quartet_change[1].source_fes_id != 0
        || alloc_result_p->fes_quartet_change[1].source_fes_pgm_id != 2
        || alloc_result_p->fes_quartet_change[1].source_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[2].dest_fes_id != 0
        || alloc_result_p->fes_quartet_change[2].dest_fes_pgm_id != 2
        || alloc_result_p->fes_quartet_change[2].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[2].place_in_alloc != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 8.2 of FES allocation.\r\n");
    }
    /*
     * Check that no mask was allocated.
     */
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[0].dest_fes_mask_id[fes_2msb_ndx] !=
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 8.2 of FES allocation. "
                         "Mask allocated when it shouldn't have been.\r\n");
        }
    }

    /*
     * 8.3
     * Negative test.
     * Build a request for context ID 1 and field group 13 with two FES quartets with the same priorities as before.
     * use action type 1 and no masks.
     */
    context_id = 2;
    fg_id = 13;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 20000);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_2_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 8.3, succeeded in allocating The same priority twice despite "
                     "contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 8.3: %d.\r\n", rv);
    }

    /*
     * 8.4
     * Negative test.
     * Build a request for context ID 4 and field group 14 with two FES quartets with identical priorites.
     * use action type 1 and no masks.
     */
    context_id = 4;
    fg_id = 14;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 8.4, succeeded in allocating The same priority twice despite "
                     "contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 8.4: %d.\r\n", rv);
    }

    /*
     * 8.5
     * Negative test.
     * Build a request for context ID 5 and field group 15 with two FES quartets with "don't care" and numerical 
     * priority.
     * use action type 1 and no masks.
     */
    context_id = 5;
    fg_id = 15;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_PRIORITY(0, 20);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 8.5, succeeded in allocating both numerical and \"don't care\" priority "
                     "despite contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 8.5: %d.\r\n", rv);
    }

    /*
     * 8.6
     * Negative test.
     * Build a request for context ID 1 and field group 16 with one FES quartets with "don't care" priority.
     * use action type 1 and no masks.
     */
    context_id = 1;
    fg_id = 16;
    fes_fg_in_p->nof_fes_quartets = 1;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_2_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 8.6, succeeded in allocating both numerical and \"don't care\" priority "
                     "despite contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 8.6: %d.\r\n", rv);
    }

    /*
     * 8.7
     * Negative test.
     * Build a request for context ID 7 and field group 17 with two FES quartets with "don't care" and position 
     * priority.
     * use action type 1 and no masks.
     */
    context_id = 7;
    fg_id = 17;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_DONT_CARE;
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 8.7, succeeded in allocating both numerical and \"don't care\" priority "
                     "despite contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 8.7: %d.\r\n", rv);
    }

    /*
     * 8.8
     * Negative test.
     * Build a request for context ID 8 and field group 18 with two FES quartets with numerical and position 
     * priority.
     * use action type 1 and no masks.
     */
    context_id = 8;
    fg_id = 18;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 18);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    /*
     * Allocate.
     * Set the severity to 'fatal' so we don't get any prints on screen when we get the expected error.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = dnx_algo_field_action_fes_allocate(unit, field_stage, fg_id, context_id, current_state_p,
                                            fes_fg_in_p, TRUE, FALSE, alloc_result_p);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In test 8.8, succeeded in allocating both numerical and position priority "
                     "despite contention.\r\n");
    }
    else if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected error in negative test 8.84: %d.\r\n", rv);
    }

    /*
     * 8.9
     * Build a request for context ID 9 and field group 19 with two FES quartets with numerical and position 
     * priority.
     * use different action types and no masks.
     */
    context_id = 9;
    fg_id = 19;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 18);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));

    /*
     * 8.10
     * Build a request for context ID 10 and field group 20 with two FES quartets with numerical and "don't care" 
     * priority.
     * use different action types and no masks.
     */
    context_id = 10;
    fg_id = 20;
    fes_fg_in_p->nof_fes_quartets = 2;
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[0].fes_action_type[fes_2msb_ndx] = 1;
        fes_fg_in_p->fes_quartet[0].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        fes_fg_in_p->fes_quartet[1].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[1].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    fes_fg_in_p->fes_quartet[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 18);
    fes_fg_in_p->fes_quartet[1].priority = BCM_FIELD_ACTION_DONT_CARE;
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * Test 9. Multiple moves.
     */
    /*
     * Create an empty context state.
     */
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        current_state_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
        current_state_2_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_fes_quartets = 0;
        current_state_2_p->fes_state.fes_id_info[fes_id_ndx].nof_masks = 0;
    }

    /*
     * 9.1
     * Build a request for context ID 1 and field group 11 with all fes quartets with descending priorities.
     */
    context_id = 1;
    fg_id = 11;
    fes_fg_in_p->nof_fes_quartets = nof_fes_instruction_per_context;
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
        {

            fes_fg_in_p->fes_quartet[fes_id_ndx].fes_action_type[fes_2msb_ndx] = 1;
            fes_fg_in_p->fes_quartet[fes_id_ndx].fes_mask_id[fes_2msb_ndx] = 1;
            fes_fg_in_p->fes_quartet[fes_id_ndx].priority =
                BCM_FIELD_ACTION_PRIORITY(((fes_id_ndx >= nof_fes_id_per_array) ? (2) : (0)),
                                          nof_fes_instruction_per_context - fes_id_ndx);
        }
    }
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_p, fes_fg_in_p, TRUE, FALSE, alloc_result_p));
    /*
     * Check that the FES quartet were allocated in descending order, including movements
     */
    fes_quartet_change_ndx = 0;
    for (fes_id_ndx = 0; fes_id_ndx < nof_fes_instruction_per_context; fes_id_ndx++)
    {
        fes_id_within_array_ndx = fes_id_ndx % nof_fes_id_per_array;

        for (fes_id_2_ndx = 0; fes_id_2_ndx < fes_id_within_array_ndx; fes_id_2_ndx++)
        {
            if (alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_id !=
                (((fes_id_ndx / nof_fes_id_per_array) * nof_fes_id_per_array) + fes_id_within_array_ndx -
                 fes_id_2_ndx)
                || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_pgm_id != 1
                || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_is_shared != FALSE
                || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].place_in_alloc != -1
                || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].source_fes_id !=
                (((fes_id_ndx / nof_fes_id_per_array) * nof_fes_id_per_array) + fes_id_within_array_ndx -
                 fes_id_2_ndx - 1)
                || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].source_fes_pgm_id != 1
                || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].source_is_shared != FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.1 of FES allocation. "
                             "Action %d, movement %d..\r\n", fes_id_ndx, fes_id_2_ndx);
            }
            fes_quartet_change_ndx++;
        }

        if (alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_id !=
            ((fes_id_ndx / nof_fes_id_per_array) * nof_fes_id_per_array)
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_pgm_id != 1
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_is_shared != FALSE
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].place_in_alloc != fes_id_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.1 of FES allocation (action %d).\r\n",
                         fes_id_ndx);
        }
        fes_quartet_change_ndx++;
    }
    if (alloc_result_p->nof_fes_quartet_changes != fes_quartet_change_ndx)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.1 of FES allocation. "
                     "Number of changes %d instead of %d.\r\n",
                     alloc_result_p->nof_fes_quartet_changes, fes_quartet_change_ndx);
    }

    /*
     * 9.2
     * Build a request for context ID 1 and field group 11 with 8 fes quartets with ascending priorities, and then a 
     * fes quartet with mandatory position pusing them.
     */
    context_id = 2;
    fg_id = 12;
    fes_fg_in_p->nof_fes_quartets = 9;
    for (fes_id_ndx = 0; fes_id_ndx < 8; fes_id_ndx++)
    {
        for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
        {
            fes_fg_in_p->fes_quartet[fes_id_ndx].fes_action_type[fes_2msb_ndx] = 1;
            fes_fg_in_p->fes_quartet[fes_id_ndx].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
            fes_fg_in_p->fes_quartet[fes_id_ndx].priority = BCM_FIELD_ACTION_PRIORITY(0, fes_id_ndx);
        }
    }
    for (fes_2msb_ndx = 0; fes_2msb_ndx < nof_prog_per_fes; fes_2msb_ndx++)
    {
        fes_fg_in_p->fes_quartet[fes_id_ndx].fes_action_type[fes_2msb_ndx] = 2;
        fes_fg_in_p->fes_quartet[fes_id_ndx].fes_mask_id[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        fes_fg_in_p->fes_quartet[fes_id_ndx].priority = BCM_FIELD_ACTION_POSITION(0, 1);
    }
    /*
     * Allocate.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate
                    (unit, field_stage, fg_id, context_id, current_state_2_p, fes_fg_in_p, TRUE, FALSE,
                     alloc_result_p));
    /*
     * Check that the FES quartet were allocated in descending order, including movements
     */
    fes_quartet_change_ndx = 0;
    for (fes_id_ndx = 0; fes_id_ndx < 8; fes_id_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_id != fes_id_ndx
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_pgm_id != 2
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_is_shared != FALSE
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].place_in_alloc != fes_id_ndx)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.2 of FES allocation (action %d).\r\n",
                         fes_id_ndx);
        }
        fes_quartet_change_ndx++;

    }
    for (fes_id_2_ndx = 7; fes_id_2_ndx > 0; fes_id_2_ndx--)
    {
        if (alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_id != fes_id_2_ndx + 1
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_pgm_id != 2
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_is_shared != FALSE
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].place_in_alloc != -1
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].source_fes_id != fes_id_2_ndx
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].source_fes_pgm_id != 2
            || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].source_is_shared != FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.2 of FES allocation. "
                         "Action %d, movement %d.\r\n", fes_id_ndx, 7 - fes_id_2_ndx);
        }
        fes_quartet_change_ndx++;
    }
    if (alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_id != 1
        || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_fes_pgm_id != 2
        || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].dest_is_shared != FALSE
        || alloc_result_p->fes_quartet_change[fes_quartet_change_ndx].place_in_alloc != fes_id_ndx)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.2 of FES allocation (action %d).\r\n", fes_id_ndx);
    }
    fes_quartet_change_ndx++;
    if (alloc_result_p->nof_fes_quartet_changes != fes_quartet_change_ndx)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected result in test 9.2 of FES allocation. "
                     "Number of changes %d instead of %d.\r\n",
                     alloc_result_p->nof_fes_quartet_changes, fes_quartet_change_ndx);
    }

    /*
     * Clear the SW state for the next test.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_IPMF3:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        case DNX_FIELD_STAGE_EPMF:
            for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
                for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                }
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    /*
     * The tests have ended.
     * Reset the SW state to be as it was before the tests.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.set(unit, old_sw_state_ipmf2_p));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.set(unit, old_sw_state_ipmf3_p));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.set(unit, old_sw_state_epmf_p));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

exit:
    SHR_FREE(old_sw_state_ipmf2_p);
    SHR_FREE(old_sw_state_ipmf3_p);
    SHR_FREE(old_sw_state_epmf_p);
    SHR_FREE(current_state_p);
    SHR_FREE(current_state_2_p);
    SHR_FREE(fes_fg_in_p);
    SHR_FREE(alloc_result_p);
    SHR_FREE(current_state_dealloc_p);
    SHR_FREE(dealloc_result_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic 'algo' field testing application.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
appl_dnx_algo_field_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type_name;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR("type", test_type_name);
    SH_SAND_GET_ENUM("stage", field_stage);

    if (sal_strncasecmp(test_type_name, TEST_TYPE_FES_ALLOC, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "%s(), line %d, Tesing 'fes allocation' %s%s\r\n", __func__, __LINE__, EMPTY,
                     EMPTY);
        SHR_IF_ERR_EXIT(diag_dnx_algo_field_action_tester_fes_allocate(unit, field_stage));
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "%s(), line %d, Options %s. Tesing 'fes allocation' returned 'success' flag %s\r\n", __func__,
                     __LINE__, test_type_name, EMPTY);
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s is not implemented. Illegal parameter. %s\r\n",
                     __func__, __LINE__, test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - run 'algo' field sequence in diag shell
 */
shr_error_e
sh_dnx_algo_field_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SHR_IF_ERR_EXIT(appl_dnx_algo_field_starter(unit, args, sand_control));
exit:
    SHR_FUNC_EXIT;
}

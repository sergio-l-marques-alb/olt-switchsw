/** \file diag_dnx_field_context_attach.h
 *  Context attach/detach usage example in DNX level.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/field/field_map.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_actions_types.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_CONTEXT_ATTACH_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_CONTEXT_ATTACH_H_INCLUDED

/** Stages that we use for array size allocation */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_NOF_PMF_STAGES               4

/*
 * Legal value that we use to set the qualifier info in the attach info,
 * so we can get different value from zero for compare, semantic testing.
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_QUAL_INPUT_ARG_AND_OFFSET    1
/**
 *  Giving random mid-range action priority, the value can be changed,
 *  it will not influence the test.
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_ACTION_PRIORITY (BCM_FIELD_ACTION_PRIORITY(0,7))
/**
 * Number of qualifiers and actions, which have to be tested.
 * Will be assigned to nof_actions and nof_quals in bcm_field_group_info_t.
 */
#define CTEST_DNX_FIELD_CONTEXT_ATTACH_NOF_QUALS_ACTIONS    1

/**
 * This is a custom structure for ctest_dnx_field_context_attach.c
 * It holds all(minimum) relevant info(per stage) needed for ctest_dnx_field_context_attach_fg_add_dnx.
 */
typedef struct
{
    /**  DNX Action */
    dnx_field_action_t dnx_action;

    /** DNX Qualifier */
    dnx_field_qual_t dnx_qualifier;

    /** Qualifier class for the DNX action set */
    dnx_field_qual_class_e qual_class;

} ctest_dnx_field_context_attach_info_per_stage_t;

/**
 * This is a custom structure for ctest_dnx_field_context_attach.c
 * It holds all(minimum) relevant info(per stage) needed for ctest_dnx_field_context_attach_*_bcm.
 * Used all for initialization of FG group array and attach info array.
 */
typedef struct
{
    /**  BCM Action */
    bcm_field_action_t bcm_action;

    /** BCM Qualifier */
    bcm_field_qualify_t bcm_qualifier;

    /**
     * BCM Qualifier attach info:
     * input_type, input_arg and offset.
     */
    bcm_field_qualify_attach_info_t qualifier_attach_info;

} ctest_dnx_field_bcm_context_attach_info_per_stage_t;

/*
 * }
 */
/**
 * \brief
 *   Options list for 'ConTeXt_Attach' shell command
 */
extern sh_sand_option_t Sh_dnx_field_context_attach_options[];

/**
 * \brief
 *   man for 'ConTeXt_Attach' command
 */
extern sh_sand_man_t Sh_dnx_field_context_attach_man;
/**
 * \brief
 *   Options list for 'ConTeXt_Attach' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_context_attach_tests[];

/**
 * \brief - run ConTeXt_Attach init sequence in diag shell
 *
 * \param [in] unit - Number of hardware unit used
 *      [in] args - Pointer to args_t struct, not used
 *      [in] sand_control - Pointer to comamnd control structure used for parameter delivery
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e sh_dnx_field_context_attach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_CONTEXT_ATTACH_H_INCLUDED */

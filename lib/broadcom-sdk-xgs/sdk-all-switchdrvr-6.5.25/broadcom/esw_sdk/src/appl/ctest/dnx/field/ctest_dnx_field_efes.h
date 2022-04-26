/** \file diag_dnx_field_efes.h
 *
 *      EFES algorithm testing.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
/*
 * }
 */

#ifndef DIAG_DNX_FIELD_EFES_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_FIELD_EFES_H_INCLUDED

/*
 * Number of qualifiers which will be used in the EFES tests.
 */
#define CTEST_DNX_FIELD_EFES_NOF_QUALS   1
/*
 * Number of stages which will be tested in the EFES tests.
 */
#define CTEST_DNX_FIELD_EFES_NOF_STAGES   4
/*
 * Number of Context Selections which will be used in the EFES tests.
 */
#define CTEST_DNX_FIELD_EFES_NOF_CS   32
/*
 * Number of action, for different cases, which will be tested in the EFES tests.
 */
#define CTEST_DNX_FIELD_EFES_POS_PRIO_NOF_ACTIONS   4
#define CTEST_DNX_FIELD_EFES_POS_PRIO_IPMF3_EPMF_NOF_ACTIONS   2
#define CTEST_DNX_FIELD_EFES_SHARING_NOF_ACTIONS    3
#define CTEST_DNX_FIELD_EFES_DONT_CARE_POS_PRIO_NOF_ACTIONS   1
#define CTEST_DNX_FIELD_EFES_DONT_CARE_NOF_ACTIONS   16
#define CTEST_DNX_FIELD_EFES_COMB_FIRST_NOF_ACTIONS     6
#define CTEST_DNX_FIELD_EFES_COMB_SECOND_NOF_ACTIONS     2
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_NOF_ACTIONS     16
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_IPMF3_NOF_ACTIONS_PER_FG     8
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_EPMF_NOF_ACTIONS_PER_FG     6
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_ACTIONS     1
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_FG3_NOF_ACTIONS   13
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_EPMF_NOF_ACTIONS   12
#define CTEST_DNX_FIELD_EFES_DESCENDING_NOF_ACTIONS    5
#define CTEST_DNX_FIELD_EFES_NEGATIVE_DIFF_PRIO_TYPES_NOF_ACTIONS   2
#define CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_TYPE_PRIO_NOF_ACTIONS     2
#define CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_POS_NOF_ACTIONS     2
#define CTEST_DNX_FIELD_EFES_NEGATIVE_SAME_ACT_DIFF_PRIO_NOF_ACTIONS    2
#define CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_ACTIONS   1
#define CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_FG3_NOF_ACTIONS   14
/*
 * Number of FGs, for different cases, which will be tested in the EFES tests.
 */
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_PGM_NOF_FGS     32
#define CTEST_DNX_FIELD_EFES_EXHAUSTIVE_MASKS_NOF_FGS     3
#define CTEST_DNX_FIELD_EFES_EVACUATED_ONLY_PLACE_NOF_FGS     4

/**
 * Enum to indicate, what will be the priority type of the action
 * which is going to be used in the EFES testing.
 */
typedef enum
{
    PRIORITY_BY_POSITION = 0,
    PRIORITY_BY_NUM_PRIO = 1,
    PRIORITY_DONT_CARE = 2
} ctest_dnx_field_efes_priority_type_e;

/**
 * This structure holds the information about context,
 * which will be created for EFES Algo tests.
 */
typedef struct
{
    /*
     * Context id. 
     */
    bcm_field_context_t context_id;
    /*
     * Context name. 
     */
    char *context_name;
    /*
     * Presel id. 
     */
    bcm_field_presel_t presel_id;

} ctest_dnx_field_efes_cs_info_t;

/**
 * This structure holds the information about
 * action priority, like prio_type, array_id,
 * position and priority.
 */
typedef struct
{
    /*
     * Action priority type. 
     */
    ctest_dnx_field_efes_priority_type_e priority_type;
    /*
     * FES array index. 
     */
    int array_id;
    /*
     * Will be used for two purposes:
     *   - Position in the FES array.
     *   - Numerical priority value.
     *
     * depends on what action_prio_type was chosen.
     */
    int position_priority;
    /*
     * Expected EFES ID to be used for comparison to the
     * returned info from the context info.
     */
    int efes_id_expected;

} ctest_dnx_field_efes_action_priority_info_t;

/**
 * This structure holds the information about
 * user and predefined actions,
 * which will be created for EFES Algo tests.
 */
typedef struct
{
    /*
     * Action name. 
     */
    char *name;
    /*
     * Action type. 
     */
    bcm_field_action_t action_type;
    /*
     * Action priority info. 
     */
    ctest_dnx_field_efes_action_priority_info_t action_prio_info;
    /*
     * Action info. 
     */
    bcm_field_action_info_t action_info;

} ctest_dnx_field_efes_action_info_t;

/**
 * This structure holds all needed information
 * about configuring of one field group.
 */
typedef struct
{
    /*
     * Field Group ID. 
     */
    bcm_field_group_t fg_id;
    /*
     * Field Group Name. 
     */
    char *fg_name;
    /*
     * Number of actions per group. 
     */
    int nof_actions;
    /*
     * Action Info. 
     */
    ctest_dnx_field_efes_action_info_t *efes_action_info;

} ctest_dnx_field_efes_fg_info_t;

/**
 * \brief
 *   Options list for 'EFES' shell command
 */
extern sh_sand_option_t Sh_dnx_field_efes_options[];

/**
 * \brief
 *   man for 'EFES' command
 */
extern sh_sand_man_t Sh_dnx_field_efes_man;
/**
 * \brief
 *   Options list for 'EFES' tests to run on regression, precommit, ...
 */
extern sh_sand_invoke_t Sh_dnx_field_efes_tests[];

/**
 * \brief - run EFES init sequence in diag shell
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
shr_error_e sh_dnx_field_efes_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DIAG_DNX_FIELD_EFES_H_INCLUDED */

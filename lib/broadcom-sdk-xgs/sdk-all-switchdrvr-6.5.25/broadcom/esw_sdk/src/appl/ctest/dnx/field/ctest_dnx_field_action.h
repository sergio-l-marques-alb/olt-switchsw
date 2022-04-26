/**
 * \file ctest_dnx_field_action.h
 *
 * 'Action' operations (for database create and entry add) procedures for DNX.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_DNX_FIELD_ACTION_H_INCLUDED
/*
 * {
 */
#define CTEST_DNX_FIELD_ACTION_H_INCLUDED
/*
* Include files.
* {
*/
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm/field.h>
#include <bcm/types.h>
#include <bcm/error.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
/** soc */
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
/*
 * }
 */
/*
 * DEFINEs
 * {
 */

/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/**
 * \brief
 *   Structure of descriptors of qualifiers to use for testing interface
 *   of direct extraction.
 * \see
 *   DNX_CTEST_DIR_EXT_NUM_QUALS_FOR_N_DE
 */
typedef struct
{
    /**
     * Qualifier's Name
     */
    char *qual_name;
    /**
     * Number of bits to assign to this user defined qualifier
     */
    unsigned int qual_nof_bits;
    /**
     * input_type/input_arg/offset to assign to this user defined qualifier.
     */
    dnx_field_qual_attach_info_t qual_attach_info;
    /**
     * Memory space to load identifier of this user defined qualifier (result
     * of dnx_field_qual_create()).
     */
    dnx_field_qual_t dnx_qual;
} dnx_ctest_field_quals_for_fg_t;
/**
 * \brief
 *   Structure of descriptors of actions to use for testing interface
 *   of direct extraction.
 *   Note that we are only handling user defined actions (class = DNX_FIELD_ACTION_CLASS_USER)
 * \see
 *   DNX_CTEST_DIR_EXT_NUM_QUALS_FOR_N_DE
 */
typedef struct
{
    /**
     * Action's Name
     */
    char *action_name;
    /**
     * Number of bits to assign to this user defined action
     */
    unsigned int action_nof_bits;
    /**
     * DBAL identifier for this action. Also used as index to get base action's
     * properties. See dnx_ipmf1_action_info[].
     */
    dbal_enum_value_field_ipmf1_action_e dbal_action_id;
    /*
     * Action info required for attaching context to filed group.
     * Currently, only contains 'priority'.
     */
    dnx_field_action_attach_info_t action_attach_info;
    /**
     * Memory space to load identifier of this user defined action (result
     * of dnx_field_action_create()).
     */
    dnx_field_action_t dnx_action;
} dnx_ctest_field_actions_for_fg_t;

/*
 * }
 */

extern sh_sand_man_t Sh_dnx_field_action_man;
/**
 * \brief
 *   Options list for 'action' tests to run on regression, precommit, ...
 * \see
 *   appl_dnx_action_starter() in diag_dnx_field_action.c
 */
extern sh_sand_invoke_t Sh_dnx_field_action_tests[];
/**
 * \brief
 *   Options list for 'action' shell command
 * \see
 *   appl_dnx_action_starter() in diag_dnx_field_action.c
 */
extern sh_sand_option_t Sh_dnx_field_action_options[];

/**
 * \brief - run 'action' sequence in diag shell
 */
shr_error_e sh_dnx_field_action_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/*
 * }
 */

#endif /* CTEST_DNX_FIELD_ACTION_H_INCLUDED */

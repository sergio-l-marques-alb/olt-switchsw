/** \file diag_dnx_field_attach.c
 *
 * Diagnostics procedures, for DNX, for 'attach' operations.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_attach.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/field/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include "diag_dnx_field_utils.h"

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
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Options
 * {
 */
/*
 * }
 */

/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Attach info (shell commands).
 */
static sh_sand_man_t Field_attach_info_man = {
    .brief = "'Attach'- displays the Field attach info about a specific field group and context (filter by field group id and context)",
    .full = "'Attach' displays Field attach info of a given FG and context.\r\n"
      "If no 'group' or 'context' are specified then an error will occur.\r\n"
      "Input parameters 'group' and 'context' are mandatory!\r\n",
    .synopsis = "[group=<0-127>] [context=<0-63>]",
    .examples = "group=1 context=63",
};
static sh_sand_option_t Field_attach_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_GROUP,     SAL_FIELD_TYPE_UINT32, "Field group id to get its attach info",     NULL   ,(void *)Field_group_enum_table},
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Field context id to which the FG is attached",     NULL, (void *)Field_context_enum_table_for_display},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function set all needed information about used qualifiers by a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] context_id - Id of the current Context.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_qual_print(
    int unit,
    int qual_iter,
    dnx_field_context_t context_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    bcm_field_input_types_t bcm_input_type;

    SHR_FUNC_INIT_VARS(unit);

    /** Print all needed info about current qualifier. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit, attach_full_info_p->attach_basic_info.dnx_quals[qual_iter]));
    PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size);
    PRT_CELL_SET("%d", fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                             attach_full_info_p->attach_basic_info.
                                                             qual_info[qual_iter].input_type, &bcm_input_type));
    PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
    if (attach_full_info_p->attach_basic_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
    {
        PRT_CELL_SET("%s", "-");
        if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_iter]) == DNX_FIELD_QUAL_CLASS_USER)
        {
            PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].offset);
        }
        else
        {
            PRT_CELL_SET("%s", "-");
        }
    }
    else
    {
        PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].input_arg);
        PRT_CELL_SET("%d", fg_info_p->group_full_info.context_info[context_id].qual_attach_info[qual_iter].offset);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function set all needed information about used actions by a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] action_iter - Id of the current action.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_action_print(
    int unit,
    dnx_field_group_t fg_id,
    int action_iter,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    int action_offset;
    unsigned int action_size;

    SHR_FUNC_INIT_VARS(unit);

    action_offset = 0;

    /** Get action size for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                  attach_full_info_p->attach_basic_info.dnx_actions[0][action_iter],
                                                  &action_size));

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id,
                                                      attach_full_info_p->attach_basic_info.dnx_actions[0][action_iter],
                                                      &action_offset));

    /** Print all needed info about current action. */
    PRT_CELL_SET("%s",
                 dnx_field_dnx_action_text(unit, attach_full_info_p->attach_basic_info.dnx_actions[0][action_iter]));
    PRT_CELL_SET("%d", action_size);
    PRT_CELL_SET("%d", action_offset);
    /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
    if (fg_info_p->group_basic_info.field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        if (attach_full_info_p->attach_basic_info.action_info[0][action_iter].priority == BCM_FIELD_ACTION_DONT_CARE)
        {
            PRT_CELL_SET("%s", "Don't care");
        }
        else
        {
            PRT_CELL_SET("%u", attach_full_info_p->attach_basic_info.action_info[0][action_iter].priority);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints key and payload information about a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Id of the current FG.
 * \param [in] context_id - Id of the current Context.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_key_and_payload_print(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_action_iter;
    int max_num_qual_actions;
    int qual_left, action_left;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = TRUE;
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG, DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG);

    PRT_TITLE_SET("Key & Payload Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Lsb");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Arg");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Lsb");
    /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
    if (fg_info_p->group_basic_info.field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Priority");
    }

    /** Iterate over maximum number of qualifiers and actions per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions && (qual_left || action_left);
         qual_action_iter++)
    {
        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
            && attach_full_info_p->attach_basic_info.dnx_quals[qual_action_iter] == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG
            && attach_full_info_p->attach_basic_info.dnx_actions[0][qual_action_iter] == DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left)
        {
            break;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            /** Print the qualifiers info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_qual_print
                            (unit, qual_action_iter, context_id, fg_info_p, attach_full_info_p, prt_ctr));
        }
        else
        {
            qual_left = FALSE;
            /**
             * Skip 6 cells (Qual, Size, Lsb, Input Type, Input Arg, Offset) about qualifier info
             * if no more valid qualifiers were found, but we still have valid actions to be printed.
             */
            PRT_CELL_SKIP(6);
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            /** Print the actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_action_print
                            (unit, fg_id, qual_action_iter, fg_info_p, attach_full_info_p, prt_ctr));
        }
        else
        {
            action_left = FALSE;
            /** In case of External stage the action priority is irrelevant, then we remove it from the table. */
            if (fg_info_p->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
            {
                /**
                 * Skip 3 cells (Action, Size, Lsb) about action info if no more valid actions were found,
                 * but we still have valid qualifiers to be printed.
                 */
                PRT_CELL_SKIP(3);
            }
            else
            {
                /**
                 * Skip 4 cells (Action, Size, Lsb, Priority) about action info if no more valid actions were found,
                 * but we still have valid qualifiers to be printed.
                 */
                PRT_CELL_SKIP(4);
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints information about which keys are
 *   being used by the given FG attached to the given context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
 * \param [in] fg_id - Id of the group which is attached to the given context.
 * \param [in] context_id - Id of the current context.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_fg_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    int key_id_iter;
    char keys_per_fg_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    PRT_TITLE_SET("Attached FG %d Key usage", fg_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "KEY ID");
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    /** Get attach info for the current context in fgs_per_context[]. */
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, attach_full_info));

    /** Iterate over max number of keys in double key and check if we a valid key id. */
    for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
         attach_full_info->key_id.id[key_id_iter] != DNX_FIELD_KEY_ID_INVALID; key_id_iter++)
    {
        /**
          * Set a temporary string buffer to store the KEY name (A, B, C, D...)
          * then concatenate it to keys_per_fg_string and print them later one.
          */
        char key_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%s, ",
                                              Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
        sal_snprintf(key_id_buff, sizeof(key_id_buff), "%s, ",
                     Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(keys_per_fg_string, "%s", key_id_buff);
        sal_strncat(keys_per_fg_string, key_id_buff, sizeof(keys_per_fg_string) - 1);
    }

    /**
      * Extract last 2 symbols of the constructed string
      * to not present comma and space ", " at the end of it.
      */
    keys_per_fg_string[sal_strlen(keys_per_fg_string) - 2] = '\0';
    PRT_CELL_SET("%s", keys_per_fg_string);

    PRT_COMMITX;
exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints FFCs and qualifiers, which are using them about a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_ffc_print(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int qual_iter, ffc_iter;
    int ffc_iter_internal;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FFC Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FFC ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");

    /** Prepare and print FFCs and qualifiers which are using them. */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type !=
         DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        for (ffc_iter = 0; ffc_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
             attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id !=
             DNX_FIELD_FFC_ID_INVALID; ffc_iter++)
        {
            for (ffc_iter_internal = 0; ffc_iter_internal < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC;
                 ffc_iter_internal++)
            {
                if (ffc_iter_internal ==
                    attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                    PRT_CELL_SET("FFC %d",
                                 attach_full_info_p->group_ffc_info.key.
                                 qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id);

                    /** Print qualifier name. */
                    PRT_CELL_SET("%s", dnx_field_dnx_qual_text(unit,
                                                               attach_full_info_p->group_ffc_info.
                                                               key.qualifier_ffc_info[qual_iter].qual_type));
                }
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints FESs and actions, which are using them about a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_fes_print(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    int fes_quartets_iter;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FES Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");

    /** Iterate over used FESs and print their IDs and actions, which are using them. */
    for (fes_quartets_iter = 0; fes_quartets_iter < attach_full_info_p->actions_fes_info.nof_fes_quartets;
         fes_quartets_iter++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("FES %d", attach_full_info_p->actions_fes_info.fes_quartets[fes_quartets_iter].fes_id);

        PRT_CELL_SET("%s",
                     dnx_field_dnx_action_text(unit,
                                               attach_full_info_p->
                                               attach_basic_info.dnx_actions[0][fes_quartets_iter]));
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints External Lookup info, ID and offset.
 * \param [in] unit - The unit number.
 * \param [in] attach_full_info_p - Pointer, which contains all Attach related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_attach_info_ifwd2_lookup_info_print(
    int unit,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Lookup Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lookup ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lookup Offset");

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.payload_id);
    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.payload_offset);

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field Attach info per given group ID and context ID, specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
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
diag_dnx_field_attach_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    dnx_field_group_t fg_id;
    dnx_field_context_t context_id;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_group_full_info_t *fg_info = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_GROUP, fg_id);
    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_CONTEXT, context_id);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);
    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Check if the specified FG ID or rage of FG IDs are allocated. */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        LOG_CLI_EX("\r\n" "The given FG Id %d is not allocated!!!%s%s%s\r\n\n", fg_id, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    /** Get information about the current Field Group. */
    SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id, fg_info));

    /** Check if the specified Context ID is allocated. */
    SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                    (unit, fg_info->group_basic_info.field_stage, context_id, &is_allocated));
    if (!is_allocated)
    {
        LOG_CLI_EX("\r\n" "The given FG Id %d is not attached to the given CS Id %d (not allocated)!!!%s%s\r\n\n",
                   fg_id, context_id, EMPTY, EMPTY);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
    SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id, context_id, attach_full_info));

    /**
     * Prepare and print a table, which contains attach information about key and payload
     * for a specific field group and context.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_key_and_payload_print
                    (unit, fg_id, context_id, fg_info, attach_full_info, sand_control));

    SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_fg_key_print
                    (unit, fg_info->group_basic_info.field_stage, fg_id, context_id, sand_control));

    /**
     * Prepare and print a table, which contains FFCs and qualifiers, which are using them
     * for a specific field group and context.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_ffc_print(unit, fg_info, attach_full_info, sand_control));

    if (fg_info->group_basic_info.field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        /** In case of External stage we should print Lookup info. */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_ifwd2_lookup_info_print(unit, attach_full_info, sand_control));
    }
    else
    {
        /**
         * Prepare and print a table, which contains FESs and actions, which are using them
         * for a specific field group and context.
         */
        SHR_IF_ERR_EXIT(diag_dnx_field_attach_info_fes_print(unit, fg_info, attach_full_info, sand_control));
    }

exit:
    SHR_FREE(attach_full_info);
    SHR_FREE(fg_info);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/*
 * }
 */
/* *INDENT-ON* */

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'attach' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_attach_cmds[] = {
    {"info", diag_dnx_field_attach_info_cb, NULL, Field_attach_info_options, &Field_attach_info_man}
    ,
    {NULL}
};

/*
 * }
 */

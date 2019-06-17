/** \file diag_dnx_field_key.c
 *
 * 'key' operations procedures for DNX.
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
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_key.h"
#include <bcm_int/dnx/field/field_key.h>
#include <appl/diag/dnx/field/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include "diag_dnx_field_utils.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/**
 * Array, which contains information for
 * relevant DBAL table per stage.
 * Used in field_key_last_key_info_cb() to allocate
 * a handle to the table.
 */
static dbal_tables_e Last_key_info_tables[DNX_FIELD_KEY_NUM_STAGES] = {
    DBAL_TABLE_FIELD_IPMF1_LAST_KEY_INFO,
    DBAL_TABLE_FIELD_IPMF2_LAST_KEY_INFO,
    
    DBAL_NOF_TABLES,
    DBAL_NOF_TABLES
};
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
 *  'Help' description for 'INFO' (shell commands).
 */
static sh_sand_man_t Field_key_info_man = {
    .brief = "'Info' related utilities",
    .full = "'Info' display info related to keys. Mandatory option is 'Type'. Can be BY_HALF_KEY or BY_BIT_RANGE.\r\n"
    "If no type is specified then an error will occur.\r\n"
    "If no context is specified then 'minimal val'-'maximal val' is assumed.\r\n"
    "If no stage is specified then range of all-stages is assumed.\r\n"
    "If no group_type is specified then DIRECT_EXTRACTION type will is assumed as default.\r\n"
    "If only one value is specified for context/stage/group_type then this 'single value' range is assumed.\r\n"
    "If 'valid' is not specified or is set to 'yes' then only stage/context combinations, with at"
    " least one allocated key or half key, are displayed.\r\n",
    .synopsis = "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]"
                " [context=<<Lowest context number>-<Highest context number>>]"
                " [group_type=<<Lowest  type>-<Highest group type>>] [valid=<YES | NO>]",
    .examples = "type=by_half_key valid=YES context=2-8 stage=IPMF1-IPMF3 group_type=DIRECT_EXTRACTION",
};

static sh_sand_option_t Field_key_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_TYPE,       SAL_FIELD_TYPE_ENUM, "Type of key display info BY_BIT_RANGE, BY_HALF_KEY",  NULL,  (void *)Field_key_display_type_enum_table},
    {DIAG_DNX_FIELD_OPTION_CONTEXT,    SAL_FIELD_TYPE_UINT32, "Lowest-highest context ID to get key info for",  "CONTEXT_LOWEST-CONTEXT_HIGHEST",  (void *)Field_context_enum_table_for_display},
    {DIAG_DNX_FIELD_OPTION_STAGE,      SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to get key info for", "IPMF1-EPMF",                      (void *)Field_stage_enum_table},
    {DIAG_DNX_FIELD_OPTION_GROUP_TYPE, SAL_FIELD_TYPE_ENUM,   "Lowest-highest group_type to get key info for",  "DIRECT_EXTRACTION",(void *)Group_type_enum_table},
    {DIAG_DNX_FIELD_OPTION_VALID,      SAL_FIELD_TYPE_BOOL,   "If TRUE, display non-empty key blocks only",     "Yes"},
    {NULL}
};


/*
 * }
 */
/* *INDENT-ON* */
/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by field groups.
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_fg_key_usage_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    prt_control_t * prt_ctr,
    int *any_key_was_displayed_p)
{
    dnx_field_group_t group_index;
    dnx_field_group_context_full_info_t *attach_full_info_p;
    dnx_field_group_t fg_ids[dnx_data_field.group.nof_fgs_get(unit)];
    uint32 nof_fgs;
    field_group_name_t group_name;
    uint32 qual_index, ffc_in_qual_index;
    bcm_field_input_types_t bcm_input_type;

    SHR_FUNC_INIT_VARS(unit);

    attach_full_info_p = NULL;

    SHR_ALLOC(attach_full_info_p, sizeof(*attach_full_info_p), "initial keys",
              "Issue with memory for attach_full_info %s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get all field groups, attached to the given context_id. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_groups_per_context_get
                    (unit, context_id, DNX_FIELD_STAGE_IPMF2, &nof_fgs, fg_ids));

    /** Iterate over nof_fgs and retrieve relevant information for them. */
    for (group_index = 0; group_index < nof_fgs; group_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_ids[group_index], context_id, attach_full_info_p));

        /** Iterate over all valid qualifiers for the current FG. */
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             attach_full_info_p->attach_basic_info.dnx_quals[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {
            /** Iterate over valid FFCs for the current qualifier.*/
            for (ffc_in_qual_index = 0; ffc_in_qual_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
                 attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_index].
                 ffc_info[ffc_in_qual_index].ffc_initial.ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID; ffc_in_qual_index++)
            {
                /**
                 * Check if the key_id, used by the current FG, is not the same as the given one.
                 * In case they are different continue until next group key_id arrive.
                 */
                if (attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_index].
                    ffc_info[ffc_in_qual_index].ffc_initial.ffc.key_id != initial_key_id)
                {
                    continue;
                }

                /**
                 * Add new row with underscore separator in case we have to print
                 * new context ID and set the new_ctx_print_p flag to FALSE.
                 * Otherwise add rows without separators and skip the context ID cell.
                 */
                if (*new_ctx_print_p)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    /**
                     * Check if Context name is being provided, in case no set N/A as default value,
                     * in other case set the provided name.
                     */
                    if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
                    {
                        PRT_CELL_SET("%d / \"%s\"", context_id, context_name_p);
                    }
                    else
                    {
                        PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
                    }

                    *new_ctx_print_p = FALSE;
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                }

                /**
                 * Set the initial_key_id cell in case we have to print
                 * new initial key and set the new_key_print_p flag to FALSE.
                 * Otherwise skip the context ID cell.
                 */
                if (*new_key_print_p)
                {
                    PRT_CELL_SET("%d / \"%c\"", initial_key_id, ('A' + initial_key_id));
                    *new_key_print_p = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }
                PRT_CELL_SET("%s",
                             dnx_field_dnx_qual_text(unit,
                                                     attach_full_info_p->attach_basic_info.dnx_quals[qual_index]));
                PRT_CELL_SET("FFC %d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc_initial.ffc.ffc_id);
                PRT_CELL_SET("%d",
                             (attach_full_info_p->group_ffc_info.key.
                              qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc_initial.ffc.
                              ffc_instruction.size + 1));
                PRT_CELL_SET("%d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc_initial.key_dest_offset);

                /** Convert the input type, of the current qualifier, to its BCM type. */
                SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                                         attach_full_info_p->attach_basic_info.
                                                                         qual_info[qual_index].input_type,
                                                                         &bcm_input_type));
                PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
                /**
                 * In case of META_DATA input type, we don't case about the input_arg and we print '-' instead.
                 * Otherwise print the retrieved values.
                 */
                if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                    DNX_FIELD_INPUT_TYPE_META_DATA)
                {
                    PRT_CELL_SET("%s", "-");
                    /**
                     * If the current META_DATA qualifier is an USER_DEFINE qualifier,
                     * we care about the give offset, otherwise print '-'.
                     */
                    if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_index]) ==
                        DNX_FIELD_QUAL_CLASS_USER)
                    {
                        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "-");
                    }
                }
                else
                {
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                }

                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_ids[group_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%s\"", fg_ids[group_index], group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fg_ids[group_index], "N/A");
                }

                /**
                 * Update the any_key_was_displayed_p flag,
                 * because we set information at least for one key.
                 */
                *any_key_was_displayed_p = TRUE;
            }
        }
    }

exit:
    SHR_FREE(attach_full_info_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by state table field groups.
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] state_table_info - Context state table info.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_ctx_mode_key_usage_state_table_info_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dnx_field_context_state_table_info_t state_table_info,
    dbal_enum_value_field_field_key_e initial_key_id,
    prt_control_t * prt_ctr,
    int *any_key_was_displayed_p)
{
    dnx_field_group_t group_index;
    dnx_field_group_context_full_info_t *attach_full_info_p;
    dnx_field_group_t fg_ids[dnx_data_field.group.nof_fgs_get(unit)];
    uint32 nof_fgs;
    field_group_name_t group_name;
    dnx_field_group_type_e fg_type;
    uint32 qual_index, ffc_in_qual_index;
    bcm_field_input_types_t bcm_input_type;

    SHR_FUNC_INIT_VARS(unit);

    attach_full_info_p = NULL;

    SHR_ALLOC(attach_full_info_p, sizeof(*attach_full_info_p), "initial keys",
              "Issue with memory for attach_full_info %s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get all field groups, attached to the given context_id. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_groups_per_context_get
                    (unit, context_id, DNX_FIELD_STAGE_IPMF1, &nof_fgs, fg_ids));

    /** Iterate over nof_fgs and retrieve relevant information for them. */
    for (group_index = 0; group_index < nof_fgs; group_index++)
    {
        /** Get the type of the current field group. */
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_ids[group_index], &fg_type));
        /**
         * Check if the current FG is not of type STATE_TABLE,
         * then we can skip it and continue iterating.
         */
        if (fg_type != DNX_FIELD_GROUP_TYPE_STATE_TABLE)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_ids[group_index], context_id, attach_full_info_p));

        /** Iterate over all valid qualifiers for the current FG. */
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             attach_full_info_p->attach_basic_info.dnx_quals[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {
            /** Iterate over valid FFCs for the current qualifier.*/
            for (ffc_in_qual_index = 0; ffc_in_qual_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
                 attach_full_info_p->group_ffc_info.key.qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].
                 ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID; ffc_in_qual_index++)
            {
                /**
                 * Check if the key_id, used by the State Table FG, is not the same as the given one.
                 * In case they are different continue until next group key_id arrive.
                 */
                if (state_table_info.key_id.id[0] != initial_key_id)
                {
                    continue;
                }

                /**
                 * Add new row with underscore separator in case we have to print
                 * new context ID and set the new_ctx_print_p flag to FALSE.
                 * Otherwise add rows without separators and skip the context ID cell.
                 */
                if (*new_ctx_print_p)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    /**
                     * Check if Context name is being provided, in case no set N/A as default value,
                     * in other case set the provided name.
                     */
                    if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
                    {
                        PRT_CELL_SET("%d / \"%s\"", context_id, context_name_p);
                    }
                    else
                    {
                        PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
                    }

                    *new_ctx_print_p = FALSE;
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                }

                /**
                 * Set the initial_key_id cell in case we have to print
                 * new initial key and set the new_key_print_p flag to FALSE.
                 * Otherwise skip the context ID cell.
                 */
                if (*new_key_print_p)
                {
                    PRT_CELL_SET("%d / \"%c\"", initial_key_id, ('A' + initial_key_id));
                    *new_key_print_p = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }
                PRT_CELL_SET("%s",
                             dnx_field_dnx_qual_text(unit,
                                                     attach_full_info_p->attach_basic_info.dnx_quals[qual_index]));
                PRT_CELL_SET("FFC %d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc.ffc_id);
                PRT_CELL_SET("%d",
                             attach_full_info_p->group_ffc_info.key.
                             qualifier_ffc_info[qual_index].ffc_info[ffc_in_qual_index].ffc.ffc_instruction.size);
                PRT_CELL_SET("%d", 0);

                /** Convert the input type, of the current qualifier, to its BCM type. */
                SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                                         attach_full_info_p->attach_basic_info.
                                                                         qual_info[qual_index].input_type,
                                                                         &bcm_input_type));
                PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
                /**
                 * In case of META_DATA input type, we don't case about the input_arg and we print '-' instead.
                 * Otherwise print the retrieved values.
                 */
                if (attach_full_info_p->attach_basic_info.qual_info[qual_index].input_type ==
                    DNX_FIELD_INPUT_TYPE_META_DATA)
                {
                    /**
                     * If the current META_DATA qualifier is an USER_DEFINE qualifier,
                     * we care about the give offset, otherwise print '-'.
                     */
                    PRT_CELL_SET("%s", "-");
                    if (DNX_QUAL_CLASS(attach_full_info_p->attach_basic_info.dnx_quals[qual_index]) ==
                        DNX_FIELD_QUAL_CLASS_USER)
                    {
                        PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "-");
                    }
                }
                else
                {
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].input_arg);
                    PRT_CELL_SET("%d", attach_full_info_p->attach_basic_info.qual_info[qual_index].offset);
                }

                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_ids[group_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%s\"", fg_ids[group_index], group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fg_ids[group_index], "N/A");
                }

                /**
                 * Update the any_key_was_displayed_p flag,
                 * because we set information at least for one key.
                 */
                *any_key_was_displayed_p = TRUE;
            }
        }
    }

exit:
    SHR_FREE(attach_full_info_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by context modes (compare and hashing).
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] key_info_p - Key information about the current context mode, contains
 *  key_id, key_template and attach information.
 * \param [in] ctx_mode_name_p - Name of the current context mode, can be one
 *  of following (CMP1, CMP2 and HASH).
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_ctx_mode_key_usage_cmp_hash_info_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    dnx_field_context_key_info_t * key_info_p,
    char *ctx_mode_name_p,
    prt_control_t * prt_ctr,
    int *any_key_was_displayed_p)
{
    uint32 qual_index, ffc_in_qual_index;
    bcm_field_input_types_t bcm_input_type;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_initial_ffc_info_t ipmf1_ffc_initial;
    uint8 key_index;
    int max_num_bits_in_ffc;
    uint8 number_of_used_ffc_per_qual;
    uint32 ffc_id_index;

    SHR_FUNC_INIT_VARS(unit);

    ffc_id_index = 0;

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);

    /**
     * In compare and HASH modes we can have up to two keys,
     * iterate over them and check if any of them matches on
     * the current initial key, if not exit from the function,
     * otherwise break the loop and continue with presenting the data.
     */
    for (key_index = 0; key_index < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_index++)
    {
        if (key_info_p->key_id.id[key_index] == initial_key_id)
        {
            break;
        }
    }
    if (key_index == DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX)
    {
        SHR_EXIT();
    }

    /** Get an array of ffc_id, which are used for construction of the current inital_key. */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, initial_key_id, ffc_id));
    /**
     * Check if we have at least one valid FFCs returned.
     * The last element of the returned array have to be indicated with DNX_FIELD_FFC_ID_INVALID.
     * In case there are no valid FFCs exit the function.
     */
    if (ffc_id[0] == DNX_FIELD_FFC_ID_INVALID)
    {
        SHR_EXIT();
    }

    /** Iterate over all valid qualifiers for the current context mode. */
    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         key_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
    {
        /**
         * Set the the maximum numbers of used FFCs by current qualifiers to iterate on,
         * depends on current qual_size and max number of bits in FFC.
         */
        number_of_used_ffc_per_qual =
            ((key_info_p->key_template.key_qual_map[qual_index].size - 1) / max_num_bits_in_ffc);
        if ((number_of_used_ffc_per_qual % max_num_bits_in_ffc) != 0)
        {
            number_of_used_ffc_per_qual += 1;
        }
        else
        {
            /** Set number_of_used_ffc_per_qual to '1' in case the qualifier is less or equal to 32b. */
            number_of_used_ffc_per_qual = 1;
        }

        /** Iterate over maximum number of FFCs per qualifier.*/
        for (ffc_in_qual_index = 0; ffc_in_qual_index < number_of_used_ffc_per_qual; ffc_in_qual_index++)
        {
            /** Get initial FFC info about the current FFC. */
            SHR_IF_ERR_EXIT(dnx_field_key_single_initial_ffc_get
                            (unit, context_id, initial_key_id, ffc_id[ffc_id_index], &ipmf1_ffc_initial));

            if (key_info_p->key_template.key_qual_map[qual_index].lsb != ipmf1_ffc_initial.key_dest_offset)
            {
                ffc_id_index++;
                continue;
            }
            /**
             * Add new row with underscore separator in case we have to print
             * new context ID and set the new_ctx_print_p flag to FALSE.
             * Otherwise add rows without separators and skip the context ID cell.
             */
            if (*new_ctx_print_p)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /**
                 * Check if Context name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(context_name_p, "", DBAL_MAX_STRING_LENGTH))
                {
                    PRT_CELL_SET("%d / \"%s\"", context_id, context_name_p);
                }
                else
                {
                    PRT_CELL_SET("%d / \"%s\"", context_id, "N/A");
                }

                *new_ctx_print_p = FALSE;
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SKIP(1);
            }

            /**
             * Set the initial_key_id cell in case we have to print
             * new initial key and set the new_key_print_p flag to FALSE.
             * Otherwise skip the context ID cell.
             */
            if (*new_key_print_p)
            {
                PRT_CELL_SET("%d / \"%c\"", initial_key_id, ('A' + initial_key_id));
                *new_key_print_p = FALSE;
            }
            else
            {
                PRT_CELL_SKIP(1);
            }

            PRT_CELL_SET("%s",
                         dnx_field_dnx_qual_text(unit, key_info_p->key_template.key_qual_map[qual_index].qual_type));

            PRT_CELL_SET("FFC %d", ffc_id[ffc_id_index]);
            PRT_CELL_SET("%d", key_info_p->key_template.key_qual_map[qual_index].size);
            PRT_CELL_SET("%d", key_info_p->key_template.key_qual_map[qual_index].lsb);

            /** Convert the input type, of the current qualifier, to its BCM type. */
            SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                                     key_info_p->attach_info[qual_index].input_type,
                                                                     &bcm_input_type));
            PRT_CELL_SET("%s", dnx_field_bcm_input_type_text(bcm_input_type));
            /**
             * In case of META_DATA input type, we don't case about the input_arg and we print '-' instead.
             * Otherwise print the retrieved values.
             */
            if (key_info_p->attach_info[qual_index].input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
            {
                PRT_CELL_SET("%s", "-");
                /**
                 * If the current META_DATA qualifier is an USER_DEFINE qualifier,
                 * we care about the give offset, otherwise print '-'.
                 */
                if (DNX_QUAL_CLASS(key_info_p->key_template.key_qual_map[qual_index].qual_type) ==
                    DNX_FIELD_QUAL_CLASS_USER)
                {
                    PRT_CELL_SET("%d", key_info_p->attach_info[qual_index].offset);
                }
                else
                {
                    PRT_CELL_SET("%s", "-");
                }
            }
            else
            {
                PRT_CELL_SET("%d", key_info_p->attach_info[qual_index].input_arg);
                PRT_CELL_SET("%d", key_info_p->attach_info[qual_index].offset);
            }

            PRT_CELL_SET("%s", ctx_mode_name_p);

            /**
             * Update the any_key_was_displayed_p flag,
             * because we set information at least for one key.
             */
            *any_key_was_displayed_p = TRUE;
            ffc_id_index++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about usage of the initial keys
 *   by context modes (compare, hashing and state_table).
 * \param [in] unit - The unit number.
 * \param [in] new_ctx_print_p - Flag to indicate whether to print
 *  the context ID in the relevant sell or to skip it.
 * \param [in] new_key_print_p - Flag to indicate whether to print
 *  the initial key ID in the relevant sell or to skip it.
 * \param [in] context_id - Context Id for which information for
 *  initial key usage have to be presented.
 * \param [in] context_name_p - Name of the given context ID to be
 *  presented in the relevant cell.
 * \param [in] initial_key_id - Initial key for which information
 *  will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] any_key_was_displayed_p - Flag to be set to TRUE in case any data
 *  about for the given initial key was set. Default value is FALSE.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_ctx_mode_key_usage_set(
    int unit,
    uint8 *new_ctx_print_p,
    uint8 *new_key_print_p,
    dnx_field_context_t context_id,
    char *context_name_p,
    dbal_enum_value_field_field_key_e initial_key_id,
    prt_control_t * prt_ctr,
    int *any_key_was_displayed_p)
{
    dnx_field_context_mode_t context_mode;
    dnx_field_context_ipmf1_sw_info_t ctx_mode_key_usage_info;
    char ctx_mode_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
    /** If the current context is allocated, get the information about it. */
    SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, &context_mode));

    /** Get information for compare, hash, state table modes for the current context. */
    SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_key_usage_info_get
                    (unit, context_id, context_mode, &ctx_mode_key_usage_info));

    /** In case one of the compare pairs has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
        context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        uint8 cmp_key_index;
        uint8 max_num_cmp_keys;
        uint8 cmp_pair_index;
        uint8 max_num_cmp_pairs;
        max_num_cmp_pairs = dnx_data_field.common_max_val.nof_compare_pairs_get(unit);
        max_num_cmp_keys = dnx_data_field.common_max_val.nof_compare_keys_in_compare_mode_get(unit);

        for (cmp_pair_index = 0; cmp_pair_index < max_num_cmp_pairs; cmp_pair_index++)
        {
            for (cmp_key_index = 0; cmp_key_index < max_num_cmp_keys; cmp_key_index++)
            {
                if (cmp_pair_index == DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1)
                {
                    sal_strncpy(ctx_mode_name, "CMP1", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_cmp_hash_info_set
                                    (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                                     &ctx_mode_key_usage_info.compare_info.pair_1.key_info[cmp_key_index],
                                     ctx_mode_name, prt_ctr, any_key_was_displayed_p));
                }
                else
                {
                    sal_strncpy(ctx_mode_name, "CMP2", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_cmp_hash_info_set
                                    (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                                     &ctx_mode_key_usage_info.compare_info.pair_2.key_info[cmp_key_index],
                                     ctx_mode_name, prt_ctr, any_key_was_displayed_p));
                }
            }
        }
    }

    /** In case hashing has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        sal_strncpy(ctx_mode_name, "HASH", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
        SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_cmp_hash_info_set
                        (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p, initial_key_id,
                         &ctx_mode_key_usage_info.hashing_info.key_info, ctx_mode_name, prt_ctr,
                         any_key_was_displayed_p));
    }

    /** In case state table has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.state_table_mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_state_table_info_set
                        (unit, new_ctx_print_p, new_key_print_p, context_id, context_name_p,
                         ctx_mode_key_usage_info.state_table_info, initial_key_id, prt_ctr, any_key_was_displayed_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays allocated initial keys.
 *   Data can be sorted by context_id and initial key, specified by the caller.
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
diag_dnx_field_key_initial_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_t context_lower, context_upper;
    dnx_field_context_t context_index;
    dbal_enum_value_field_field_key_e initial_key_lower, initial_key_upper;
    dbal_enum_value_field_field_key_e initial_key_index;
    int any_key_was_displayed;
    uint8 ipmf1_ctx_is_alloc, ipmf2_ctx_is_alloc;
    char context_name[DBAL_MAX_STRING_LENGTH];
    uint8 new_ctx_print, new_key_print;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_KEY, initial_key_lower, initial_key_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, context_lower, context_upper);

    any_key_was_displayed = FALSE;
    new_ctx_print = FALSE;
    new_key_print = FALSE;

    PRT_TITLE_SET("Initial Keys Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key ID/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Types");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FFC ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset in Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Input Arg");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group (ID/Name) / Mode");

    /** Iterate over all contexts or the range specified by the caller. */
    for (context_index = context_lower; context_index <= context_upper; context_index++)
    {
        /** Check if the current Context ID is allocated for IPMF1 stage. */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                        (unit, DNX_FIELD_STAGE_IPMF1, context_index, &ipmf1_ctx_is_alloc));
        /**
         * Check if the current Context ID is allocated for IPMF2 stage.
         * Done in case we have context switch.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated
                        (unit, DNX_FIELD_STAGE_IPMF2, context_index, &ipmf2_ctx_is_alloc));

        if (!ipmf1_ctx_is_alloc && !ipmf2_ctx_is_alloc)
        {
            continue;
        }

        sal_memset(context_name, 0, sizeof(context_name));
        if (ipmf1_ctx_is_alloc)
        {
            /** Take the name for current context from the SW state. */
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, context_index, DNX_FIELD_STAGE_IPMF1, context_name));
        }
        else
        {
            /** Take the name for current context from the SW state. */
            SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                            value.stringget(unit, context_index, DNX_FIELD_STAGE_IPMF2, context_name));
        }

        /** Indicates that new context have to be printed. */
        new_ctx_print = TRUE;

        /** Iterate over all initial keys or the range specified by the caller. */
        for (initial_key_index = initial_key_lower; initial_key_index <= initial_key_upper; initial_key_index++)
        {
            /** Indicates that new initial key have to be printed. */
            new_key_print = TRUE;

            /** Present field groups, which are using current initial key, and relevant information about it. */
            SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_fg_key_usage_set(unit, &new_ctx_print, &new_key_print,
                                                                        context_index, context_name, initial_key_index,
                                                                        prt_ctr, &any_key_was_displayed));

            /** Check the context modes key usage only for IPMF1 context. */
            if (ipmf1_ctx_is_alloc)
            {
                /**
                 * Present context modes (CMP, HASH. STATE_TABLE),
                 * which are using current initial key and relevant information to it.
                 */
                SHR_IF_ERR_EXIT(diag_dnx_field_key_initial_ctx_mode_key_usage_set(unit, &new_ctx_print, &new_key_print,
                                                                                  context_index, context_name,
                                                                                  initial_key_index, prt_ctr,
                                                                                  &any_key_was_displayed));
            }
        }
    }

    if (!any_key_was_displayed)
    {
        LOG_CLI_EX("\r\n" "NO occupied initial key was found within specified ranges!! %s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
        SHR_EXIT();
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/** Context ID to be used for init and deinit procedures. */
static bcm_field_context_t context_id;

/**
 * \brief
 *   This function allocates some resources to be used
 *   for testing during the 'field key initial exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_init_before_run(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_context_hash_info_t hash_info;
    bcm_field_context_compare_info_t compare_info;
    void *dest_char;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "key_initial_diag", sizeof(context_info.name));
    context_info.hashing_enabled = TRUE;
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));

    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 3;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 0;
    compare_info.first_key_info.qual_types[1] = bcmFieldQualifyColor;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.first_key_info.qual_types[2] = bcmFieldQualifyIpProtocol;
    compare_info.first_key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[2].input_arg = 1;
    compare_info.first_key_info.qual_info[2].offset = 0;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 2;
    compare_info.second_key_info.qual_info[0].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));

    bcm_field_context_hash_info_t_init(&hash_info);
    hash_info.hash_function = bcmFieldContextHashFunctionCrc16Bisync;
    hash_info.order = TRUE;
    hash_info.hash_config.function_select = bcmFieldContextHashActionValueReplaceCrc;
    hash_info.hash_config.action_key = bcmFieldContextHashActionKeyAdditionalLbKey;
    hash_info.key_info.nof_quals = 5;
    hash_info.key_info.qual_types[0] = bcmFieldQualifySrcIp;
    hash_info.key_info.qual_types[1] = bcmFieldQualifyDstIp;
    hash_info.key_info.qual_types[2] = bcmFieldQualifyIpProtocol;
    hash_info.key_info.qual_types[3] = bcmFieldQualifyL4SrcPort;
    hash_info.key_info.qual_types[4] = bcmFieldQualifyL4DstPort;
    hash_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[0].input_arg = 1;
    hash_info.key_info.qual_info[0].offset = 0;
    hash_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[1].input_arg = 1;
    hash_info.key_info.qual_info[1].offset = 0;
    hash_info.key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[2].input_arg = 1;
    hash_info.key_info.qual_info[2].offset = 0;
    hash_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[3].input_arg = 2;
    hash_info.key_info.qual_info[3].offset = 0;
    hash_info.key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    hash_info.key_info.qual_info[4].input_arg = 2;
    hash_info.key_info.qual_info[4].offset = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_hash_create(unit, 0, bcmFieldStageIngressPMF1, context_id, &hash_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function deallocates all resources
 *   allocated by diag_dnx_field_key_initial_init_before_run.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_initial_deinit_after_run(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Compare destroy the first pair_id. */
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 1));

    /** Hashing destroy. */
    SHR_IF_ERR_EXIT(bcm_field_context_hash_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    /** Destroy the context. */
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays allocated bit-ranges as specified by the caller.
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
diag_dnx_field_key_display_bit_ranges_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_context_t context_lower, context_upper;
    dnx_field_group_type_e group_type;
    int non_empty_only, any_key_was_displayed;
    dnx_field_stage_e stage_index;
    dnx_field_context_t context_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, context_lower, context_upper);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_GROUP_TYPE, group_type);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "%s(), line %d:\r\n"
                 "Display non empty entries only: %d%s\r\n", __FUNCTION__, __LINE__, non_empty_only, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> stages: from %d to %d, Contexts: from %d to %d\r\n",
                 stage_lower, stage_upper, context_lower, context_upper);

    any_key_was_displayed = FALSE;
    PRT_TITLE_SET("");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Context");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "KEY\nid");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "KEY\nname");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Half-key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Half-key\nstate");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group\ntype");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base\nbit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Num\nbits");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        int none_occupied, do_display;
        do_display = FALSE;
        /*
         * Within the range, relate to 'pmf' stages only (and not, for example, to, say, DNX_FIELD_STAGE_INGRESS_VTT)
         */
        if (DNX_FIELD_IS_FP_STAGE(stage_index))
        {
            for (context_index = context_lower; context_index <= context_upper; context_index++)
            {
                do_display = FALSE;
                if (non_empty_only)
                {
                    /*
                     * If caller requests to only see allocated/occupied keys, set 'do_display' accordingly.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_key_is_any_key_occupied
                                    (unit, stage_index, context_index, group_type, TRUE, &none_occupied));
                    if (!none_occupied)
                    {
                        do_display = TRUE;
                        break;
                    }
                }
                else
                {
                    /*
                     * If caller does not request to only see allocated/occupied keys, then display all.
                     */
                    do_display = TRUE;
                    break;
                }
            }
            if (do_display)
            {
                any_key_was_displayed = TRUE;
                SHR_IF_ERR_EXIT(dnx_field_diag_display_bit_range_occupation_partial
                                (unit, stage_index, context_index, sand_control, prt_ctr));
            }
        }
    }
    if (!any_key_was_displayed)
    {
        LOG_CLI_EX("\r\n" "NO occupied key was found within specified ranges!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY,
                   EMPTY);
        SHR_EXIT();
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function displays keys per range as specified by the caller.
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
diag_dnx_field_key_display_range_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_context_t context_lower, context_upper;
    dnx_field_group_type_e group_type;
    int non_empty_only, any_key_was_displayed;
    dnx_field_stage_e stage_index;
    dnx_field_context_t context_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, context_lower, context_upper);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_GROUP_TYPE, group_type);
    SH_SAND_GET_BOOL(DIAG_DNX_FIELD_OPTION_VALID, non_empty_only);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "%s(), line %d:\r\n"
                 "Display non empty entries only: %d%s\r\n", __FUNCTION__, __LINE__, non_empty_only, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "==> stages: from %d to %d, Contexts: from %d to %d\r\n",
                 stage_lower, stage_upper, context_lower, context_upper);

    any_key_was_displayed = FALSE;
    PRT_TITLE_SET("");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Context");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "KEY\nid");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "KEY\nname");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MSB half-key state");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Allocation id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "LSB half-key state");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Group type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Allocation id");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
    {
        int none_occupied, do_display;
        do_display = FALSE;
        /*
         * Within the range, relate to 'pmf' stages only (and not, for example, to, say, DNX_FIELD_STAGE_INGRESS_VTT)
         */
        if (DNX_FIELD_IS_FP_STAGE(stage_index))
        {
            for (context_index = context_lower; context_index <= context_upper; context_index++)
            {
                do_display = FALSE;
                if (non_empty_only)
                {
                    /*
                     * If caller requests to only see allocated/occupied keys, set 'do_display' accordingly.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_key_is_any_key_occupied
                                    (unit, stage_index, context_index, group_type, FALSE, &none_occupied));
                    if (!none_occupied)
                    {
                        do_display = TRUE;
                        break;
                    }
                }
                else
                {
                    /*
                     * If caller does not request to only see allocated/occupied keys, then display all.
                     */
                    do_display = TRUE;
                    break;
                }
            }
            if (do_display)
            {
                any_key_was_displayed = TRUE;
                SHR_IF_ERR_EXIT(dnx_field_diag_display_key_occupation_partial
                                (unit, stage_index, context_index, sand_control, prt_ctr));
            }
        }
    }
    if (!any_key_was_displayed)
    {
        LOG_CLI_EX("\r\n" "NO occupied key was found within specified ranges!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY,
                   EMPTY);
        SHR_EXIT();
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays keys per range as specified by the caller.
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
diag_dnx_field_key_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int key_display_type;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_TYPE, key_display_type);

    switch (key_display_type)
    {
        case 0:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_key_display_range_cb(unit, args, sand_control));
            break;
        }
        case 1:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_key_display_bit_ranges_cb(unit, args, sand_control));
            break;
        }
        default:
        {
            LOG_CLI_EX("\r\n" "Key Display Type '%s' is not supported by KEY Info diagnostic!! %s%s%s\r\n\n",
                       Field_key_display_type_enum_table[key_display_type].string, EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/**
 *  'Help' description for KEY display (shell commands).
 */
static sh_sand_man_t field_key_last_key_info_man = {
    .brief = "Dump a given key and display information about it in a table.",
    .full = "Field stage, for which we want to retrieve key information, should be specified. Can be: 'ipmf1' or 'ipmf2' or 'ipmf3' or 'epmf', \r\n",
            "Key should be specified if not the default is to display info for all keys of the given stage.\r\n",
    .synopsis = "field key last stage=<IPMF1 | IPMF2 | IPMF3 | EPMF> key=<A - J | ALL>",
    .examples = "stage=ipmf1 key=all",
};

static sh_sand_option_t field_key_last_key_info_options[] = {
    /* Name */                                                  /* Type */                           /* Description */                               /* Default */
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_ENUM,    "Stage to get key info for (ipmf1, ipmf2, ipmf3, epmf)",    "ipmf1",  (void *)Field_stage_enum_table},
    {DIAG_DNX_FIELD_OPTION_KEY,    SAL_FIELD_TYPE_ENUM,    "Key to be displayed (A-J or ALL)",                         "all",  (void *)Field_key_enum_table},
    {NULL}
    /** End of options list - must be last. */
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *  This function fills a table with information,
 *  about a given key, taken from DBAL.
 * \param [in] unit -
 *  The unit number.
 * \param [in] core_id -
 *  From which core, the DBAL table info should be taken.
 * \param [in] key_id -
 *  Id of the key for which information should be presented.
 * \param [in] key_name_p -
 *  String, which contains the key name,
 *  parsed to the table to be displayed.
 * \param [in] entry_handle_id -
 *  Handle to the DBAL table.
 * \param [in] prt_ctr -
 *  Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_key_last_key_info_table_fill(
    int unit,
    int core_id,
    int key_id,
    char *key_name_p,
    uint32 entry_handle_id,
    prt_control_t * prt_ctr)
{
    /**
     * An array of 5 elements to store the returned
     * 160 bits key information, from the DBAL table.
     */
    uint32 field_key_value[5];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Specifying from which core, the DBAL table info should be taken.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /*
     * Setting the key ID, for which we want to retrieve an information.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    /*
     * Perform the DBAL read
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Get a result fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_KEY, INST_SINGLE, field_key_value));

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("KEY_%s", key_name_p);

    PRT_CELL_SET("0x%08x%08x%08x%08x%08x", field_key_value[4],
                 field_key_value[3], field_key_value[2], field_key_value[1], field_key_value[0]);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function dumps and displays information about a given key.
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
diag_dnx_field_key_last_key_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e field_stage;
    int field_key;
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    int key_id;
    int core_id, nof_cores;
    int max_nof_keys_ipmf2;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_KEY, field_key);

    
    if (field_stage == DNX_FIELD_STAGE_IPMF3 || field_stage == DNX_FIELD_STAGE_EPMF)
    {
        LOG_CLI((BSL_META
                 ("%s(), line %d, Field stage %s is not supported, yet! Only IPMF1 and IPMF2 are supported! %s\r\n"),
                 __FUNCTION__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY));
    }

    /*
     * Get max number of cores
     */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    max_nof_keys_ipmf2 = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF2)->nof_keys_alloc * 2;

    table_id = Last_key_info_tables[field_stage];
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        PRT_TITLE_SET("Table: %s   Core: %d", dbal_logical_table_to_string(unit, table_id), core_id);

        PRT_COLUMN_ADD("Key");
        PRT_COLUMN_ADD("Value");

        /*
         * If caller wants to see information about all keys (option "key=all") for a given stage.
         */
        if (field_key == DNX_FIELD_KEY_ALL)
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                for (key_id = 0; key_id < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_keys_alloc;
                     key_id++)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                                    (unit, core_id, key_id, Field_key_enum_table[key_id].string, entry_handle_id,
                                     prt_ctr));
                }
            }
            else if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                /**
                 * "key_id" started from 5 because in IPMF2 we have only 5 keys,
                 * which are from "F" to "J".
                 */
                for (key_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_keys_alloc;
                     key_id < max_nof_keys_ipmf2; key_id++)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                                    (unit, core_id, key_id, Field_key_enum_table[key_id].string, entry_handle_id,
                                     prt_ctr));
                }
            }
            else
            {
                LOG_CLI((BSL_META
                         ("%s(), line %d, Field stage %s is illegal. %s\r\n"),
                         __FUNCTION__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY));
            }
        }
        else
        {
            /*
             * If caller wants to see information about a specific key (option "key=A-J") for a given stage.
             */
            if (field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                                (unit, core_id, field_key, Field_key_enum_table[field_key].string, entry_handle_id,
                                 prt_ctr));
            }
            else if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_key_last_key_info_table_fill
                                (unit, core_id, field_key, Field_key_enum_table[field_key].string, entry_handle_id,
                                 prt_ctr));
            }
            else
            {
                LOG_CLI((BSL_META
                         ("%s(), line %d, Field stage %s is illegal. %s\r\n"),
                         __FUNCTION__, __LINE__, dnx_field_stage_e_get_name(field_stage), EMPTY));
            }
        }

        PRT_COMMITX;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */

/**
 *  'Help' description for 'KEY INITIAL' (shell commands).
 */
static sh_sand_man_t Field_key_initial_man = {
    .brief = "'Initial key' related display utilities",
    .full =
        "'Key' display of 'initial' keys. For all inputs {'group' (field group), 'context', 'group type' and 'initial key'\r\n"
        "a range may be specified.\r\n" "If no context is specified then 'minimal val'-'maximal val' is assumed.\r\n"
        "If only one value is specified for context/stage/group_type then this 'single value' range is assumed.\r\n"
        "If 'init_key' is not specified then all used initial keys are displayed. Otherwise, only specified 'initial key's"
        " are displayed.\r\n",
    .synopsis =
        " [context=<<Lowest context number>-<Highest context number>>]" " [initial_key=<<Lowest key>-<<Highest key>>]",
    .examples = "key=MIN-MAX context=CONTEXT_LOWEST-CONTEXT_HIGHEST",
    .init_cb = diag_dnx_field_key_initial_init_before_run,
    .deinit_cb = diag_dnx_field_key_initial_deinit_after_run
};

static sh_sand_option_t Field_key_initial_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT, SAL_FIELD_TYPE_UINT32, "Lowest-highest context ID to get initial keys of",
     "CONTEXT_LOWEST-CONTEXT_HIGHEST", (void *) Field_context_enum_table_for_display, "CONTEXT_LOWEST-CONTEXT_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_KEY, SAL_FIELD_TYPE_UINT32, "Initial key to be displayed (F-J)", "MIN-MAX",
     (void *) Field_init_key_enum_table, "MIN-MAX"},
    {NULL}
};

sh_sand_cmd_t Sh_dnx_field_key_cmds[] = {
    {"info", diag_dnx_field_key_info_cb, NULL, Field_key_info_options,
     &Field_key_info_man},
    {"initial", diag_dnx_field_key_initial_cb, NULL, Field_key_initial_options,
     &Field_key_initial_man},
    {"last", diag_dnx_field_key_last_key_info_cb, NULL, field_key_last_key_info_options,
     &field_key_last_key_info_man},
    {NULL}
};
/*
 * }
 */

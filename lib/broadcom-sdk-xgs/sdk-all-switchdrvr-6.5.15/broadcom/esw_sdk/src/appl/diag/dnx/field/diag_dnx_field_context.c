/** \file diag_dnx_field_context.c
 *
 * Diagnostics procedures, for DNX, for 'context' operations.
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
#include "diag_dnx_field_context.h"
#include <bcm_int/dnx/field/field_context.h>
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
 *  'Help' description for Context display (shell commands).
 */
static sh_sand_man_t Field_context_list_man = {
    .brief = "'Context'- displays the basic info about a specific context or range of contexts "
             "on a certain stage (filter by field context id and field stage)",
    .full = "'Context' display for a range of contexts or field stages. Range is specified via 'context/stage'.\r\n"
            "If no 'context' is specified then '0'-'DNX_DATA.field.common_max_val.nof_contexts-1' is assumed (0-63).\r\n"
            "If only one value is specified for context then this 'single value' range is assumed.\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[context=<lower value>-<upper value>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "context=0-20 stage=IPMF1-IPMF3",
};
static sh_sand_option_t Field_context_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field context to get its info",     "CONTEXT_LOWEST-CONTEXT_HIGHEST",     (void *)Field_context_enum_table_for_display},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "IPMF1-EPMF",    (void *)Field_stage_enum_table},
    {NULL}
};

/**
 *  'Help' description for Context display (shell commands).
 */
static sh_sand_man_t Field_context_info_man = {
    .brief = "'Context'- displays the full info about a specific context on a specific field stage (filter by context and stage)",
    .full = "'Context' display for a context on a specific stage. \r\n"
      "If no 'context' is specified then an error will occur.\r\n"
      "Input parameters 'context' and 'stage' are mandatory.\r\n",
    .synopsis = "[context=<0-63>] [stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>",
    .examples = "context=63 stage=ipmf1",
};
static sh_sand_option_t Field_context_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_CONTEXT,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field context to get its info",     NULL,    NULL},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_ENUM, "Lowest-highest field stage to get context info for",   NULL,    (void *)Field_stage_enum_table},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function prepares and sets information about the given context.
 *   Like ID, stage, name, presel ids and quals, and context modes.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] context_mode_p - Pointer, which contains
 *  all context mode information or IPMF1 and IPMF2 context modes.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    prt_control_t * prt_ctr,
    dnx_field_context_mode_t * context_mode_p)
{
    bcm_field_presel_t presel_index, presel_qual_index;
    uint32 max_nof_cs_lines;
    char presel_quals_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    /** The size is multiplied by 3 because we have to show all 0-127 presel IDs. */
    char presel_ids_string[DIAG_DNX_FIELD_UTILS_STR_SIZE * 3] = { 0 };
    char context_name[DBAL_MAX_STRING_LENGTH];
    char mode_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    bcm_field_stage_t bcm_stage;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;

    SHR_FUNC_INIT_VARS(unit);

    /** Get max number of context selection lines. */
    max_nof_cs_lines = dnx_data_field.common_max_val.nof_cs_lines_get(unit);

    /** Take the name for current context from the SW state. */
    sal_memset(context_name, 0, sizeof(context_name));
    SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                    value.stringget(unit, context_id, field_stage, context_name));

    /** Convert DNX to BCM Field Stage. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%d", context_id);
    PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));

    /**
    * Check if Context name is being provided, in case no set N/A as default value,
    * in other case set the provided name.
    */
    if (sal_strncmp(context_name, "", sizeof(context_name)))
    {
        PRT_CELL_SET("%s", context_name);
    }
    else
    {
        PRT_CELL_SET("N/A");
    }

    /** Get information about preselector, which has being set to the current context. */
    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    for (presel_index = 0; presel_index < max_nof_cs_lines; presel_index++)
    {
        /** String to store the current presel ID. */
        char presel_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        entry_id.presel_id = presel_index;
        entry_id.stage = bcm_stage;
        /** Retrieve information about the current presel_id. */
        SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
        /**
         * Check if the presel entry is valid for the current context.
         * In case no, continue iterating over all preselectors.
         */
        if (!((entry_data.entry_valid) && (entry_data.context_id == context_id)))
        {
            continue;
        }

        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", presel_index);
        sal_snprintf(presel_id_buff, sizeof(presel_id_buff), "%d, ", presel_index);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_ids_string, "%s", presel_id_buff);
        sal_strncat(presel_ids_string, presel_id_buff, sizeof(presel_ids_string) - 1);

        sal_strncpy(presel_quals_string, "", sizeof(presel_quals_string) - 1);

        /** In case current context_id is not the default one print information about it. */
        if (context_id != DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
        {
            /**
             * Construct a string, which contains information (name/value) about
             * all preselector qualifiers.
             */
            for (presel_qual_index = 0; presel_qual_index < entry_data.nof_qualifiers; presel_qual_index++)
            {
                /** String to store current qual type and its value. */
                char presel_qual_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                /**
                 * In case of bcmFieldQualifyForwardingType qualifier we will print the name of the FWD type value.
                 * Otherwise print the value itself.
                 */
                if (entry_data.qual_data[presel_qual_index].qual_type == bcmFieldQualifyForwardingType)
                {
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                          "%s(%s),\n",
                                                          dnx_field_bcm_qual_text(unit, entry_data.qual_data
                                                                                  [presel_qual_index].qual_type),
                                                          dnx_field_bcm_layer_type_text(entry_data.qual_data
                                                                                        [presel_qual_index].qual_value));
                    sal_snprintf(presel_qual_buff, sizeof(presel_qual_buff), "%s(%s),\n",
                                 dnx_field_bcm_qual_text(unit, entry_data.qual_data[presel_qual_index].qual_type),
                                 dnx_field_bcm_layer_type_text(entry_data.qual_data[presel_qual_index].qual_value));
                }
                else
                {
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                          "%s(%d),\n",
                                                          dnx_field_bcm_qual_text(unit, entry_data.qual_data
                                                                                  [presel_qual_index].qual_type),
                                                          entry_data.qual_data[presel_qual_index].qual_value);
                    sal_snprintf(presel_qual_buff, sizeof(presel_qual_buff), "%s(%d),\n",
                                 dnx_field_bcm_qual_text(unit, entry_data.qual_data[presel_qual_index].qual_type),
                                 entry_data.qual_data[presel_qual_index].qual_value);
                }
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(presel_quals_string, "%s", presel_qual_buff);
                sal_strncat(presel_quals_string, presel_qual_buff, sizeof(presel_quals_string) - 1);
            }
        }
        else
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                /** In case of DEFAULT context and stage is IPMF2, we will print "-" (dash) in the relevant cell. */
                sal_strncpy(presel_quals_string, "-, ", sizeof(presel_quals_string) - 1);
            }
            else
            {
                /** In case of DEFAULT context we will print "Always Hit" in the relevant cell. */
                sal_strncpy(presel_quals_string, "Always Hit, ", sizeof(presel_quals_string) - 1);
            }
        }
    }

    if (sal_strncmp(presel_ids_string, "", sizeof(presel_ids_string)))
    {
        
        if (context_id == DNX_FIELD_CONTEXT_ID_DEFAULT(unit))
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                PRT_CELL_SET("%s", "No hit - iPMF1 ctx chosen!");
            }
            else
            {
                PRT_CELL_SET("%d", (max_nof_cs_lines - 1));
            }
        }
        else
        {
            /**
             * Extract last 2 symbols of the constructed string
             * to not present comma and empty space ", " at the end of it.
             */
            presel_ids_string[sal_strlen(presel_ids_string) - 2] = '\0';
            PRT_CELL_SET("%s", presel_ids_string);
        }
        /**
         * Extract last 2 symbols of the constructed string
         * to not present comma and empty space or '\n'  ", " at the end of it.
         */
        presel_quals_string[sal_strlen(presel_quals_string) - 2] = '\0';
        PRT_CELL_SET("%s", presel_quals_string);
    }
    else
    {
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            PRT_CELL_SET("%s", "No hit - iPMF1 ctx chosen!");
        }
        else
        {
            PRT_CELL_SET("%s", "No Presel (unreachable context)!");
        }
        PRT_CELL_SET("%s", "-");
    }

    if (field_stage != DNX_FIELD_STAGE_IPMF3 && field_stage != DNX_FIELD_STAGE_EPMF &&
        field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, context_mode_p));
        /** If the current context is allocated, get the information about it. */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, field_stage, context_id, context_mode_p));
    }

    /** Prepare and set information about context modes, which have being enabled for the current context. */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_context_modes_string_get(unit, *context_mode_p, mode_string));

        if (sal_strncmp(mode_string, "", sizeof(mode_string)))
        {
            /**
             * Extract last 2 symbols of the constructed string
             * to not present comma and space ", " at the end of it.
             */
            mode_string[sal_strlen(mode_string) - 2] = '\0';
            PRT_CELL_SET("%s", mode_string);
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
        }
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2
             && (context_mode_p->context_ipmf2_mode.cascaded_from != DNX_FIELD_CONTEXT_ID_INVALID))
    {
        PRT_CELL_SET("Cascading_Ctx %d", context_mode_p->context_ipmf2_mode.cascaded_from);
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Contexts per context ID or range of IDs and stage (optional), specified by the caller.
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
diag_dnx_field_context_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    dnx_field_context_t ctx_id_index;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    dnx_field_context_t ctx_id_lower, ctx_id_upper;
    dnx_field_context_mode_t context_mode;
    int do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_CONTEXT, ctx_id_lower, ctx_id_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    PRT_TITLE_SET("CONTEXT");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel IDs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel(Value)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mode");

    do_display = FALSE;

    for (stage_index = stage_lower; stage_index <= stage_upper && (DNX_FIELD_IS_FP_STAGE(stage_index)); stage_index++)
    {
        for (ctx_id_index = ctx_id_lower; ctx_id_index <= ctx_id_upper; ctx_id_index++)
        {
            /** Check if the specified Context ID is allocated. */
            SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated(unit, stage_index, ctx_id_index, &is_allocated));
            if (!is_allocated)
            {
                continue;
            }

            /**
             * Prepare and print a table, which contains all basic info about the current context id on
             * the given field stage.
             */
            SHR_IF_ERR_EXIT(diag_dnx_field_context_info_print(unit, stage_index, ctx_id_index, prt_ctr, &context_mode));

            do_display = TRUE;
        }
    }
    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was printed then just clear loaded PRT memory.
         * This could be left to 'exit' but it is clearer here.
         */
        PRT_FREE;
        LOG_CLI_EX("\r\n" "NO allocated Context was found on specified filters!! %s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints compare key information about a specific compare pair.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] nof_cmp_keys - Number of keys to compare.
 * \param [in] pair_id - Id of the compare Pair.
 * \param [in] context_compare_info_p - Pointer, which contains
 *  all needed context compare related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_cmp_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    int nof_cmp_keys,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_context_compare_info_t * context_compare_info_p,
    sh_sand_control_t * sand_control)
{
    int cmp_key_id, key_qual_index;
    dnx_field_context_compare_mode_info_t *cmp_pair;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    cmp_pair = NULL;

    if (pair_id == DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR)
    {
        cmp_pair = &(context_compare_info_p->pair_1);
    }
    else if (pair_id == DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR)
    {
        cmp_pair = &(context_compare_info_p->pair_2);
    }

    /** Prepare context compare key info. */
    for (cmp_key_id = 0; cmp_key_id < nof_cmp_keys; cmp_key_id++)
    {
        PRT_TITLE_SET("CMP%d Key %d", pair_id, cmp_key_id + 1);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");

        for (key_qual_index = 0; key_qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].qual_type !=
             DNX_FIELD_QUAL_TYPE_INVALID; key_qual_index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                         (unit, cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].qual_type));
            PRT_CELL_SET("%d", cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].size);
            PRT_CELL_SET("%d", cmp_pair->key_info[cmp_key_id].key_template.key_qual_map[key_qual_index].lsb);
        }

        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints context hash key information.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] hashing_info_p - Pointer, which contains
 *  all needed context hashing related information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_hash_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_hashing_info_t * hashing_info_p,
    sh_sand_control_t * sand_control)
{
    int key_qual_index;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("HASH Key");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lsb");

    /** Prepare context hashing key info. */
    for (key_qual_index = 0; key_qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].qual_type !=
         DNX_FIELD_QUAL_TYPE_INVALID; key_qual_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                     (unit, hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].qual_type));
        PRT_CELL_SET("%d", hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].size);
        PRT_CELL_SET("%d", hashing_info_p->key_info.key_template.key_qual_map[key_qual_index].lsb);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints context hash key information.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Id of the current context.
 * \param [in] context_mode_p - Pointer, which contains
 *  all context mode information.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_cmp_hash_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t * context_mode_p,
    sh_sand_control_t * sand_control)
{
    int nof_cmp_keys;
    dnx_field_context_compare_info_t context_compare_info;
    dnx_field_context_hashing_info_t hashing_info;

    SHR_FUNC_INIT_VARS(unit);

    nof_cmp_keys = 0;

    SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &context_compare_info));

    /** Prepare and print information about context compare and hashing keys. */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        if (context_mode_p->context_ipmf1_mode.compare_mode_1 == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE)
        {
            nof_cmp_keys = 1;
            if (context_compare_info.pair_1.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR,
                                 &context_compare_info, sand_control));
            }
        }
        else if (context_mode_p->context_ipmf1_mode.compare_mode_1 == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            nof_cmp_keys = 2;
            if (context_compare_info.pair_1.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR,
                                 &context_compare_info, sand_control));
            }
        }

        if (context_mode_p->context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE)
        {
            nof_cmp_keys = 1;
            if (context_compare_info.pair_2.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR,
                                 &context_compare_info, sand_control));
            }
        }
        else if (context_mode_p->context_ipmf1_mode.compare_mode_2 == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
        {
            nof_cmp_keys = 2;
            if (context_compare_info.pair_2.is_set)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_key_print
                                (unit, field_stage, nof_cmp_keys, DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR,
                                 &context_compare_info, sand_control));
            }
        }

        if (context_mode_p->context_ipmf1_mode.hash_mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED)
        {
            SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get(unit, context_id, &hashing_info));
            SHR_IF_ERR_EXIT(diag_dnx_field_context_info_hash_key_print(unit, field_stage, &hashing_info, sand_control));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns number of used ffc by initial keys, per context and stage.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Context Id for which ffc usage will be presented.
 * \param [out] nof_used_initial_ffcs - Number of used ffcs by initial keys.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_nof_used_initial_keys_ffc_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 * nof_used_initial_ffcs)
{
    uint32 ffc_index;
    uint32 nof_used_initial_ffcs_internal;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dbal_enum_value_field_field_key_e initial_key_index;

    SHR_FUNC_INIT_VARS(unit);

    *nof_used_initial_ffcs = 0;
    nof_used_initial_ffcs_internal = 0;

    /** Iterate over all initial keys or the range specified by the caller. */
    for (initial_key_index = DBAL_ENUM_FVAL_FIELD_KEY_F; initial_key_index <= DBAL_ENUM_FVAL_FIELD_KEY_J;
         initial_key_index++)
    {
        /** Get an array of ffc_id, which are used for construction of the current inital_key. */
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, initial_key_index, ffc_id));

        for (ffc_index = 0; ffc_id[ffc_index] != DNX_FIELD_FFC_ID_INVALID; ffc_index++)
        {
            nof_used_initial_ffcs_internal++;
        }
    }

    *nof_used_initial_ffcs = nof_used_initial_ffcs_internal;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function prepares/sets/prints information about used and free FFCs per given stage.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage for which the FFC Groups info will be taken and present.
 * \param [in] context_id - Id of the current context.
 * \param [in] ffc_ids_p - Pointer, which contains all used and free FFCs.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_ffc_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int *ffc_ids_p,
    sh_sand_control_t * sand_control)
{
    int ffc_iter, ffc_group_iter;
    int nof_free_ffcs;
    int ffc_iter_lower, ffc_iter_upper;
    char ffc_group_name[DIAG_DNX_FIELD_UTILS_STR_SIZE];
    uint32 nof_used_initial_ffcs;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("FFC Groups");
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_nof_used_initial_keys_ffc_get(unit, field_stage, context_id, &nof_used_initial_ffcs));
        PRT_INFO_ADD("Number of used FFCs by initial keys: %d.", nof_used_initial_ffcs);
        PRT_INFO_ADD("- The indication '(i)' means that this FFC is used by initial keys.");
        PRT_INFO_ADD("Note: Group 4 cannot be at initial Keys.");
    }

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FFC Group ID (Types)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Used FFC Id's ");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "NOF Free FFCs");

    /**
     * Iterate over max number of FFC Groups for the given stage:
     *      - get the lower and upper FFC values
     *      - generate a string with all used FFCs and prepare it for print
     *      - set the relevant cells with used FFCs and number of free FFCs
     */
    for (ffc_group_iter = 0; ffc_group_iter < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups;
         ffc_group_iter++)
    {
        char used_ffcs_string[DIAG_DNX_FIELD_UTILS_STR_SIZE*2] = { 0 };
        nof_free_ffcs = 0;
        nof_used_initial_ffcs = 0;
        /** Indicate FFC ranges per FFC group. */
        switch (ffc_group_iter)
        {
            case 0:
            {
                ffc_iter_lower = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_one_lower;
                ffc_iter_upper = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_one_upper;
                break;
            }
            case 1:
            {
                ffc_iter_lower = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_two_lower;
                ffc_iter_upper = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_two_upper;
                break;
            }
            case 2:
            {
                ffc_iter_lower = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_three_lower;
                ffc_iter_upper = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_three_upper;
                break;
            }
            case 3:
            {
                ffc_iter_lower = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_four_lower;
                ffc_iter_upper = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_four_upper;
                break;
            }
            default:
            {
                ffc_iter_lower = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_one_lower;
                ffc_iter_upper = dnx_data_field.stage.stage_info_get(unit, field_stage)->ffc_group_one_upper;
                break;
            }
        }
        for (ffc_iter = ffc_iter_lower; ffc_iter <= ffc_iter_upper; ffc_iter++)
        {
            if (ffc_ids_p[ffc_iter] != DNX_FIELD_FFC_ID_INVALID)
            {
                char ffc_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d, ", ffc_ids_p[ffc_iter]);
                sal_snprintf(ffc_id_buff, sizeof(ffc_id_buff), "%d, ", ffc_ids_p[ffc_iter]);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(used_ffcs_string, "%s", ffc_id_buff);
                sal_strncat(used_ffcs_string, ffc_id_buff, sizeof(used_ffcs_string) - 1);
            }
            else
            {
                nof_free_ffcs++;
            }

            if (field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                uint32 initial_ffc_index;
                uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
                dbal_enum_value_field_field_key_e initial_key_index;
                /** Iterate over all initial keys or the range specified by the caller. */
                for (initial_key_index = DBAL_ENUM_FVAL_FIELD_KEY_F; initial_key_index <= DBAL_ENUM_FVAL_FIELD_KEY_J;
                     initial_key_index++)
                {
                    /** Get an array of ffc_id, which are used for construction of the current inital_key. */
                    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, initial_key_index, ffc_id));

                    for (initial_ffc_index = 0; ffc_id[initial_ffc_index] != DNX_FIELD_FFC_ID_INVALID; initial_ffc_index++)
                    {
                        if (ffc_id[initial_ffc_index] == ffc_iter)
                        {
                            char ffc_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%d(i), ", ffc_id[initial_ffc_index]);
                            sal_snprintf(ffc_id_buff, sizeof(ffc_id_buff), "%d(i), ", ffc_id[initial_ffc_index]);
                            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(used_ffcs_string, "%s", ffc_id_buff);
                            sal_strncat(used_ffcs_string, ffc_id_buff, sizeof(used_ffcs_string) - 1);

                            nof_used_initial_ffcs++;
                        }
                    }
                }
            }
        }

        if (field_stage == DNX_FIELD_STAGE_IPMF1)
        {
            nof_free_ffcs -= nof_used_initial_ffcs;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        SHR_IF_ERR_EXIT(diag_dnx_field_utils_ffc_groups_names_get
                        (unit, field_stage, ffc_group_iter, &(ffc_group_name[0])));
        PRT_CELL_SET("%d (%s)", (ffc_group_iter + 1), ffc_group_name);

        if (sal_strncmp(used_ffcs_string, "", sizeof(used_ffcs_string)))
        {
            /**
             * Extract last 2 symbols of the constructed string
             * to not present comma and space ", " at the end of it.
             */
            used_ffcs_string[sal_strlen(used_ffcs_string) - 2] = '\0';
            PRT_CELL_SET("%s", used_ffcs_string);
            PRT_CELL_SET("%d", nof_free_ffcs);
        }
        else
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%d", nof_free_ffcs);
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
 *   being used by FGs attached to the current context.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which the context was created.
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
diag_dnx_field_context_info_fg_key_print(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    sh_sand_control_t * sand_control)
{
    uint8 is_fg_allocated;
    int ctx_id_index, fg_id_index, fg_per_ctx_index;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_group_t fgs_per_context[dnx_data_field.group.nof_fgs_get(unit)];
    int key_id_iter;
    int qual_iter, ffc_iter, ffc_out_index = 0;
    int ffc_ids[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_stage_e fg_stage;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    /** Initialize the ffc_ids[] array with invalid values. */
    sal_memset(ffc_ids, DNX_FIELD_FFC_ID_INVALID, sizeof(ffc_ids));

    /**
     * Iterate over maximum number of FGs and take information for those, which are being
     * attached to the current context.
     */
    fg_per_ctx_index = 0;
    for (fg_id_index = 0; fg_id_index < dnx_data_field.group.nof_fgs_get(unit); fg_id_index++)
    {
        dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** If FG_ ID is not allocated, move to next FG ID */
            continue;
        }

        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_index, &fg_stage));
        if (fg_stage != field_stage)
        {
            /**
             * If the current allocated FG ID is not on the same stage as the given context,
             * continue iterating over all FGs.
             */
            continue;
        }

        sal_memset(context_id_arr, DNX_FIELD_CONTEXT_ID_INVALID, sizeof(context_id_arr));

        /** Get the list of all contexts that the FG is attached to. */
        SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_index, context_id_arr));

        for (ctx_id_index = 0; ctx_id_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
             context_id_arr[ctx_id_index] != DNX_FIELD_CONTEXT_ID_INVALID; ctx_id_index++)
        {
            if (context_id_arr[ctx_id_index] == context_id)
            {
                fgs_per_context[fg_per_ctx_index] = fg_id_index;
                fg_per_ctx_index++;
            }
        }
    }

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_TITLE_SET("Attached FGs and Lookup Ids");
    }
    else
    {
        PRT_TITLE_SET("Attached FGs and Key usage");
    }
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID");

    /** Print all attached FGs. */
    for (fg_id_index = 0; fg_id_index < fg_per_ctx_index; fg_id_index++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "fg %d", fgs_per_context[fg_id_index]);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        PRT_CELL_SET("%s", "LOOKUP_ID");
    }
    else
    {
        PRT_CELL_SET("%s", "KEY");
    }
    /** Print all used keys by the attached FGs. */
    for (fg_id_index = 0; fg_id_index < fg_per_ctx_index; fg_id_index++)
    {
        char keys_per_fg_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

        /** Get attach info for the current context in fgs_per_context[]. */
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fgs_per_context[fg_id_index], context_id, attach_full_info));
        if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
        {
            PRT_CELL_SET("%d", attach_full_info->attach_basic_info.payload_id);
        }
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
                                                  Field_key_enum_table[attach_full_info->key_id.
                                                                       id[key_id_iter]].string);
            sal_snprintf(key_id_buff, sizeof(key_id_buff), "%s, ",
                         Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(keys_per_fg_string, "%s", key_id_buff);
            sal_strncat(keys_per_fg_string, key_id_buff, sizeof(keys_per_fg_string) - 1);

            /** Iterate over max number of qualifiers for FG and check if current qual is valid for it. */
            for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
                 attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].qual_type !=
                 DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
            {
                /**
                 * Iterate over max number of FFCs for qualifier until we reach invalid one.
                 * Fill the ffc_ids[] array with the valid FFC IDs indexes.
                 */
                for (ffc_iter = 0; ffc_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
                     attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].ffc.ffc_id !=
                     DNX_FIELD_FFC_ID_INVALID; ffc_iter++)
                {
                    /**
                     * Set only those indexes from the ffc_ids[] array, which are valid. All others
                     * will say with the initial value DNX_FIELD_CONTEXT_ID_INVALID.
                     */
                    ffc_ids[ffc_out_index] =
                        attach_full_info->group_ffc_info.key.qualifier_ffc_info[qual_iter].ffc_info[ffc_iter].
                        ffc.ffc_id;
                    ffc_out_index++;
                }
            }
        }
        if (field_stage != DNX_FIELD_STAGE_EXTERNAL)
        {
            /**
             * Extract last 2 symbols of the constructed string
             * to not present comma and space ", " at the end of it.
             */
            keys_per_fg_string[sal_strlen(keys_per_fg_string) - 2] = '\0';
            PRT_CELL_SET("%s", keys_per_fg_string);
        }
    }

    if (fg_per_ctx_index)
    {
    PRT_COMMITX}
    else
    {
        PRT_FREE;
        LOG_CLI_EX("\r\n" "No attached Field Groups were found for context %d !!!%s%s%s\r\n\n", context_id, EMPTY,
                   EMPTY, EMPTY);
    }

    /** Print information about used and free FFCs. */
    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_ffc_print(unit, field_stage, context_id, ffc_ids, sand_control));

exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns Key Gen value for the given stage and context ID.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context has been allocated.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [out] key_gen_value_p - Key Gen Value, which was set by context_param_set().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_key_gen_var_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 *key_gen_value_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_STAGE, field_stage);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROGRAM_KEY_GEN_VAR, INST_SINGLE, key_gen_value_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns System Header Profile index for the given context ID.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [out] sys_hdr_prof_index_p - Sytem header profile index, which was set by context_param_set().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_sys_hdr_prof_index_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 *sys_hdr_prof_index_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF1_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_HEADER_PROFILE, INST_SINGLE,
                               sys_hdr_prof_index_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function returns Bytes to remove header info for the given context ID.
 *
 * \param [in] unit - The unit number.
 * \param [in] context_id - Id of the current context for which info will be presented.
 * \param [out] bytes_to_remove_p - Bytes to remove, which was set by context_param_set().
 * \param [out] layer_to_remove_p - Layer to remove, which was set by context_param_set().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_context_info_bytes_to_remove_header_info_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 *bytes_to_remove_p,
    uint32 *layer_to_remove_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_IPMF2_CONTEXT_GENERAL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_BYTES_TO_REMOVE, INST_SINGLE, bytes_to_remove_p);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NUM_LAYERS_TO_REMOVE, INST_SINGLE, layer_to_remove_p);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Context full info per context ID
 *   on a specific field stage specified by the caller.
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
diag_dnx_field_context_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated;
    dnx_field_context_t context_id;
    dnx_field_stage_e field_stage;
    dnx_field_context_mode_t context_mode;
    bcm_field_stage_t bcm_stage;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_CONTEXT, context_id);
    SH_SAND_GET_ENUM(DIAG_DNX_FIELD_OPTION_STAGE, field_stage);

    /** Verify if the given context_id is in range. */
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    /** Check if the specified Context ID is allocated. */
    SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated(unit, field_stage, context_id, &is_allocated));
    if (!is_allocated)
    {
        /** Convert DNX to BCM Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
        LOG_CLI_EX("\r\n" "Context ID %d is not allocated for %s stage!!%s%s\r\n\n", context_id,
                   dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY);
    }

    PRT_TITLE_SET("CONTEXT");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CTX ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel IDs");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel(Value)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Mode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Gen Var");
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "System Header Profile");
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Bytes to Remove");
    }

    /**
     * Prepare and print a table, which contains all basic info about the current context id on
     * the given field stage. Returns context_mode info to be used in for compare and hash key
     * preparation and printing.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_print(unit, field_stage, context_id, prt_ctr, &context_mode));

    /** Key Gen value is not available on egress stage, in other cases we will present it. */
    if (field_stage != DNX_FIELD_STAGE_EPMF)
    {
        uint32 key_gen_value;

        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_key_gen_var_info_get
                        (unit, field_stage, context_id, &key_gen_value));

        PRT_CELL_SET("0x%08x", key_gen_value);
    }
    else
    {
        PRT_CELL_SET("%s", "N/A");
    }

    /** Present the system header profiles in case of IPMF1 and layers and bytes to remove for IPMF3. */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        uint32 sys_hdr_prof_index;

        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_sys_hdr_prof_index_get(unit, context_id, &sys_hdr_prof_index));

        PRT_CELL_SET("%s", dnx_data_field.system_headers.system_header_profiles_get(unit, sys_hdr_prof_index)->name);
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        uint32 bytes_to_remove;
        uint32 layer_to_remove;

        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_bytes_to_remove_header_info_get
                        (unit, context_id, &bytes_to_remove, &layer_to_remove));

        PRT_CELL_SET("%s + %d bytes", dnx_field_bcm_packet_remove_layer_text(layer_to_remove), bytes_to_remove);
    }

    PRT_COMMITX;

    /**
     * Prepare and print a table, which contains information about which keys are
     * being used by FGs attached to the current context.
     */
    SHR_IF_ERR_EXIT(diag_dnx_field_context_info_fg_key_print(unit, field_stage, context_id, sand_control));

    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        /** Prepare and print tables, which contain information about compare and hash key construction. */
        SHR_IF_ERR_EXIT(diag_dnx_field_context_info_cmp_hash_key_print
                        (unit, field_stage, context_id, &context_mode, sand_control));
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'context' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_context_cmds[] = {
    {"list", diag_dnx_field_context_list_cb, NULL, Field_context_list_options, &Field_context_list_man}
    ,
    {"info", diag_dnx_field_context_info_cb, NULL, Field_context_info_options, &Field_context_info_man}
    ,
    {NULL}
};

/*
 * }
 */

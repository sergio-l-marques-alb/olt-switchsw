/** \file diag_dnx_field_last.c
 *
 * Diagnostics procedures, for DNX, for 'last' (last packet) operations.
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
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/field/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/sand/sand_signals.h>
#include <bcm/instru.h>
#include <src/bcm/dnx/field/map/field_map_local.h>
#include "diag_dnx_field_utils.h"
#include "diag_dnx_field_last.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/**
 * Retrieve the value of the 'tcam_cs_hit_bit_support' from DNX-DATA to be used for checking
 * if current device is supporting the TCAM Context Selection HIT Indication.
 * Will be used as flag in the diagnostic.
 * This flag will be used, because on some of the devices (For Example: JR2, JR2_B0)
 * the TCAM CS HIT indication memories will be not supported, and this flag will avoid reading of the memory.
 */
#define DIAG_DNX_FIELD_LAST_TCAM_HIT_INDICATION_ENABLE  dnx_data_field.diag.feature_get(unit, dnx_data_field_diag_tcam_cs_hit_bit_support)
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
 *  'Help' description for Last (shell commands).
 */
static sh_sand_man_t Field_last_man = {
    .brief = "'Last Info'- displays system view of the entire field lookup process for a certain stage (filter by field stage).",
    .full = "'Last Info' displays system view of the entire field lookup process for a certain stage.\r\n"
            "If no stage 'IPMF1'-'EPMF' is specified then an error will occur.\r\n",
    .synopsis = "[stage=<IPMF1 | IPMF2 | IPMF3 | EPMF>]"
                "[core=<0 | 1 | all>]",
    .examples = "stage=IPMF1 core=all",
};
static sh_sand_option_t Field_last_options[] = {
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Field stage to show system info for",   "IPMF1-EPMF",    (void *)Field_stage_enum_table},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function returns the presel ID which was hit.
 * \param [in] unit - The unit number.
 * \param [in] cs_hit_indic_table - Relevant CS hit indication DBAL table per stage.
 * \param [out] presel_id - Presel id, which was hit, to be returned as result of the
 *  function.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_hit_info_get(
    int unit,
    dbal_tables_e cs_hit_indic_table,
    bcm_field_presel_t * presel_id)
{
    uint8 cs_line_id, cs_entry_id;
    uint8 max_nof_cs_lines, max_nof_entries_per_line;
    uint8 hit_indication_value;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get max values from DNX data. */
    max_nof_entries_per_line = dnx_data_field.tcam.hit_indication_entries_per_byte_get(unit);
    max_nof_cs_lines = (dnx_data_field.common_max_val.nof_cs_lines_get(unit) / max_nof_entries_per_line);

    /** Initialize the presel_id to its default value. */
    *presel_id = DNX_FIELD_PRESEL_ID_INVALID;
    hit_indication_value = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cs_hit_indic_table, &entry_handle_id));

    /** Iterate over the context selection lines. */
    for (cs_line_id = 0; cs_line_id < max_nof_cs_lines; cs_line_id++)
    {
        /** Iterate over the presel ID in the 8b bitmap. */
        for (cs_entry_id = 0; cs_entry_id < max_nof_entries_per_line; cs_entry_id++)
        {
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_ROW_IDX, cs_line_id);
            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_IDX, cs_entry_id);
            dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TCAM_HIT, INST_SINGLE, &hit_indication_value);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            /** In case a hit was found, return the presel ID. */
            if (hit_indication_value)
            {
                *presel_id = ((cs_line_id * max_nof_entries_per_line) + cs_entry_id);
                break;
            }
        }
        if (*presel_id != DNX_FIELD_PRESEL_ID_INVALID)
        {
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets relevant information about qualifiers of the
 *   by context modes (compare and hashing) used keys.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] is_hash - Flag to indicate if the function was called
 *  to present hash qualifier info. Can be TRUE (hash) or FALSE (cmp).
 * \param [in] cmp_key_index - Compare key index.
 * \param [in] cmp_pair_index - Compare pair index.
 * \param [in] context_id - ID of the selected context.
 * \param [in] key_info_p - Key information about the current context mode, contains
 *  key_id, key_template and attach information.
 * \param [in] new_cmp_mode_id_print_p - Flag to indicate if we have to print,
 *  a new compare mode on different row, with separator.
 * \param [out] do_display_any_key_info_p - Flag to be set to TRUE in case any info
 *  about keys was set. Default value is FALSE.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_cmp_hash_qual_info_set(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    uint8 is_hash,
    uint8 cmp_key_index,
    uint8 cmp_pair_index,
    dnx_field_context_t context_id,
    dnx_field_context_key_info_t * key_info_p,
    uint8 *new_cmp_mode_id_print_p,
    uint8 *do_display_any_key_info_p,
    prt_control_t * prt_ctr)
{
    uint32 qual_index;
    int val_iter;
    int max_val_iter;
    int max_num_bits_in_ffc;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    dnx_field_map_qual_signal_info_t qual_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
    uint32 signal_iter;
    uint8 key_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);

    /** In compare and HASH modes we can have up to two keys, iterate over them. */
    for (key_index = 0; key_index < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
         key_info_p->key_id.id[key_index] != DNX_FIELD_KEY_ID_INVALID; key_index++)
    {
        /** Iterate over all valid qualifiers for the current context mode. */
        for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
             key_info_p->key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID; qual_index++)
        {
            /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
            max_val_iter = (key_info_p->key_template.key_qual_map[qual_index].size / max_num_bits_in_ffc);
            if ((max_val_iter % max_num_bits_in_ffc) != 0)
            {
                max_val_iter += 1;
            }
            else
            {
                max_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY - 4;
            }

            /** In case we have more than one valid qualifiers for the current key, we have to skip some cells. */
            if (!qual_index)
            {
                /** In case of HASH we print the key ID on first line of the table. */
                if (is_hash)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                }
                else
                {
                    /**
                     * In case the flag new_cmp_mode_id_print_p is set to TRUE,
                     * we will print cmp mode ID, cmp pair ID and the key ID on
                     * first line of the table. Otherwise we skip the cmp mode ID
                     * cell.
                     */
                    if (*new_cmp_mode_id_print_p)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("%d", (cmp_pair_index + 1));
                        PRT_CELL_SET("%d", (cmp_key_index + 1));
                        PRT_CELL_SET("%c", ('A' + key_info_p->key_id.id[key_index]));

                        *new_cmp_mode_id_print_p = FALSE;
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(1);
                        PRT_CELL_SET("%d", (cmp_key_index + 1));
                        PRT_CELL_SET("%c", ('A' + key_info_p->key_id.id[key_index]));
                    }
                }
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                if (!is_hash)
                {
                    /**
                     * In case of CMP and we have more than one qualifier,
                     * we should skip the cells for the CMP Mode Id, Pair Id and Key Id.
                     */
                    PRT_CELL_SKIP(3);
                }
            }

            PRT_CELL_SET("%s",
                         dnx_field_dnx_qual_text(unit, key_info_p->key_template.key_qual_map[qual_index].qual_type));
            PRT_CELL_SET("%d", key_info_p->key_template.key_qual_map[qual_index].lsb);
            PRT_CELL_SET("%d", key_info_p->key_template.key_qual_map[qual_index].size);

            if (DNX_QUAL_CLASS(key_info_p->key_template.key_qual_map[qual_index].qual_type) == DNX_FIELD_QUAL_CLASS_USER &&
                (key_info_p->attach_info[qual_index].input_type == DNX_FIELD_INPUT_TYPE_META_DATA ||
                 key_info_p->attach_info[qual_index].input_type == DNX_FIELD_INPUT_TYPE_META_DATA2))
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_utils_user_qual_metadata_value_get(unit, core_id, block_p, DNX_FIELD_STAGE_IPMF1,
                    key_info_p->key_template.key_qual_map[qual_index].qual_type, key_info_p->attach_info[qual_index].offset, qual_val_string));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_signals(unit, DNX_FIELD_STAGE_IPMF1,
                                                               key_info_p->key_template.key_qual_map[qual_index].qual_type,
                                                               qual_signals));

                sal_memset(qual_val, 0, sizeof(qual_val));
                sal_strncpy(qual_val_string, "", sizeof(qual_val_string) - 1);
                for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
                {
                    char sig_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                    char sig_val_buff_internal[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
                    /** Break in case we reach an empty string, which is indication the end of signals array. */
                    if (qual_signals[signal_iter].name == 0)
                    {
                        break;
                    }

                    /** Retrieve the qualifier value from the relevant signal. */
                    SHR_IF_ERR_EXIT(dpp_dsig_read
                                    (unit, core_id, block_p, qual_signals[signal_iter].from,
                                     qual_signals[signal_iter].to, qual_signals[signal_iter].name, qual_val, max_val_iter));

                    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
                    {
                        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%X", qual_val[val_iter]);
                        sal_snprintf(val_buff, sizeof(val_buff), "%X", qual_val[val_iter]);
                        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(sig_val_buff, "%s", val_buff);
                        sal_strncat(sig_val_buff, val_buff, sizeof(sig_val_buff) - 1);
                    }

                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s: 0x%s\n", qual_signals[signal_iter].name, sig_val_buff);
                    sal_snprintf(sig_val_buff_internal, sizeof(sig_val_buff_internal), "%s: 0x%s\n",
                                 qual_signals[signal_iter].name, sig_val_buff);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", sig_val_buff_internal);
                    sal_strncat(qual_val_string, sig_val_buff_internal, sizeof(qual_val_string) - 1);
                }
            }

            PRT_CELL_SET("%s", qual_val_string);

            *do_display_any_key_info_p = TRUE;
        }

        /**
         * For HASH we are not dividing key info table per key,
         * all the used qualifiers will be present for both keys.
         */
        if (is_hash)
        {
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents information about, keys used by the context modes.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] context_mode - Context mode info (cmp, hash).
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_mode_key_info_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t context_mode,
    char *block_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_ipmf1_sw_info_t ctx_mode_key_usage_info;
    uint8 do_display_any_key_info;
    uint8 new_cmp_mode_id_print;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    do_display_any_key_info = FALSE;

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

        PRT_TITLE_SET("Compare Key Info");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CMP Mode Id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Pair Id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Initial Key Id");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");

        for (cmp_pair_index = 0; cmp_pair_index < max_num_cmp_pairs; cmp_pair_index++)
        {
            new_cmp_mode_id_print = TRUE;

            for (cmp_key_index = 0; cmp_key_index < max_num_cmp_keys; cmp_key_index++)
            {
                if (cmp_pair_index == DNX_FIELD_CONTEXT_COMPARE_MODE_PAIR_1)
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_cmp_hash_qual_info_set
                                    (unit, core_id, block_p, FALSE, cmp_key_index, cmp_pair_index, context_id,
                                     &ctx_mode_key_usage_info.compare_info.pair_1.key_info[cmp_key_index],
                                     &new_cmp_mode_id_print, &do_display_any_key_info, prt_ctr));
                }
                else
                {
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_cmp_hash_qual_info_set
                                    (unit, core_id, block_p, FALSE, cmp_key_index, cmp_pair_index, context_id,
                                     &ctx_mode_key_usage_info.compare_info.pair_2.key_info[cmp_key_index],
                                     &new_cmp_mode_id_print, &do_display_any_key_info, prt_ctr));
                }
            }
        }
    }

    if (do_display_any_key_info)
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
    }

    do_display_any_key_info = FALSE;

    /** In case hashing has valid mode, present its information, otherwise skip it. */
    if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        PRT_TITLE_SET("Hash Key Info of initial keys 'I' and 'J'");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");

        SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_cmp_hash_qual_info_set
                        (unit, core_id, block_p, TRUE, FALSE, FALSE, context_id,
                         &ctx_mode_key_usage_info.hashing_info.key_info, FALSE, &do_display_any_key_info, prt_ctr));
    }

    if (do_display_any_key_info)
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
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function presents information about, which context was hit and relevant
 *   presel ID.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [out] context_id_p - ID of the selected context.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_cs_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_stage_e field_stage,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_context_t * context_id_p,
    sh_sand_control_t * sand_control)
{
    uint32 context_id;
    char context_name[DBAL_MAX_STRING_LENGTH];
    bcm_field_stage_t bcm_stage;

    bcm_field_presel_t hit_presel_id;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Context Selection Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel Id");

    /** Read the signal of 'Context' to retrieve the selected context id. */
    SHR_IF_ERR_EXIT(dpp_dsig_read(unit, core_id, block_p, from_p, to_p, "Context", &context_id, 1));
    *context_id_p = (dnx_field_context_t) context_id;

    /** Take the name for current context from the SW state. */
    sal_memset(context_name, 0, sizeof(context_name));
    SHR_IF_ERR_EXIT(dnx_field_context_sw.context_info.name.
                    value.stringget(unit, *context_id_p, field_stage, context_name));

    /** Convert DNX to BCM Field Stage. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

    /** Check if the CS TCAM HIT indication is supported on current device. */
    if (DIAG_DNX_FIELD_LAST_TCAM_HIT_INDICATION_ENABLE)
    {
        /** Get the presel ID which was hit. */
        SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_hit_info_get
                        (unit, dnx_field_map_stage_info[field_stage].cs_hit_indication_table_id, &hit_presel_id));

        if (hit_presel_id != DNX_FIELD_PRESEL_ID_INVALID)
        {
            /** Get information about preselector, which has being set to the current context. */
            bcm_field_presel_entry_id_info_init(&entry_id);
            bcm_field_presel_entry_data_info_init(&entry_data);
            entry_id.presel_id = hit_presel_id;
            entry_id.stage = bcm_stage;
            /** Retrieve information about the current presel_id. */
            SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
            /**
             * Check if the retrieved presel id is valid for the selected context.
             */
            if ((entry_data.entry_valid) && (entry_data.context_id == *context_id_p))
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%d", hit_presel_id);
            }
            else
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "N/A");
            }
        }
        else
        {
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "N/A");
        }
    }
    else
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", "N/A");
    }

    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

    PRT_CELL_SET("%s", "Context Id/Name");

    /**
    * Check if Context name is being provided, in case no set N/A as default value,
    * in other case set the provided name.
    */
    if (sal_strncmp(context_name, "", sizeof(context_name)))
    {
        PRT_CELL_SET("%d/\"%s\"", *context_id_p, context_name);
    }
    else
    {
        PRT_CELL_SET("%d/\"%s\"", *context_id_p, "N/A");
    }

    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        dnx_field_context_mode_t context_mode;
        uint32 sys_prof_index;
        /** Read the signal of 'Sys_Header_Profile_Index' to retrieve its value. */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, block_p, from_p, to_p, "Sys_Header_Profile_Index", &sys_prof_index, 1));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "System Header Profile");
        PRT_CELL_SET("%s", dnx_data_field.system_headers.system_header_profiles_get(unit, sys_prof_index)->name);

        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &context_mode));
        /** If the current context is allocated, get the information about it. */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, DNX_FIELD_STAGE_IPMF1, context_id, &context_mode));

        /** In case one of the context modes CMP or HASH has valid mode, present its information, otherwise skip it. */
        if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
            context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
            context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
        {
            char mode_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_context_modes_string_get(unit, context_mode, mode_string));
            /**
             * Extract last 2 symbols of the constructed string
             * to not present comma and space ", " at the end of it.
             */
            mode_string[sal_strlen(mode_string) - 2] = '\0';

            /** Add new row, to present the context modes. */
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%s", "Mode");
            PRT_CELL_SET("%s", mode_string);
            PRT_COMMITX;

            /** Print the key info for the different context modes: CMP, HASH. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_mode_key_info_print(unit, core_id, context_id,
                                                                       context_mode, block_p, sand_control));
        }
        else
        {
            PRT_COMMITX;
        }
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        uint32 bytes_to_remove_hdr_sig_val;
        uint32 bytes_to_remove;
        uint32 layer_to_remove;
        /** Read the signal of 'Bytes_to_Remove_Header' to retrieve its value. */
        SHR_IF_ERR_EXIT(dpp_dsig_read
                        (unit, core_id, "IRPP", "IPMF3", "LBP", "Bytes_to_Remove_Header", &bytes_to_remove_hdr_sig_val,
                         1));

        layer_to_remove = bytes_to_remove_hdr_sig_val & dnx_data_field.diag.layers_to_remove_mask_get(unit);
        bytes_to_remove =
            (bytes_to_remove_hdr_sig_val >> dnx_data_field.
             diag.layers_to_remove_size_in_bit_get(unit)) & dnx_data_field.diag.bytes_to_remove_mask_get(unit);
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Bytes to Remove");
        PRT_CELL_SET("%s + %d bytes", dnx_field_bcm_packet_remove_layer_text(layer_to_remove), bytes_to_remove);

        PRT_COMMITX;
    }
    else
    {
        PRT_COMMITX;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function retrieve information about last hit entry per field group.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id_index - Iterator value to know, on which place to save the entry id.
 * \param [in] fg_id - Field group ID, which performed lookups and have a hit.
 * \param [out] last_diag_fg_info_p - Stores entry which was hit per FG.
 * \param [out] is_entry_found_p - Indication if we find at least one entry.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_entry_info_get(
    int unit,
    bcm_core_t core_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *is_entry_found_p,
    sh_sand_control_t * sand_control)
{
    uint32 bank_id, bank_line_id, bank_sram_id, sram_line_entry_id;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_tcam_big_banks, max_nof_srams_per_tcam_bank;
    uint32 max_nof_80_bit_ent_per_bank, max_nof_big_bank_lines;
    uint8 hit_indication_value;
    uint32 entry_handle_id;
    dnx_field_entry_t entry_info;
    uint32 entry_id_from_location;
    uint8 found;
    uint32 entry_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get max values from DNX data. */
    max_nof_tcam_big_banks = dnx_data_field.tcam.nof_big_banks_get(unit);
    max_nof_srams_per_tcam_bank = dnx_data_field.tcam.action_width_selector_size_get(unit);
    max_nof_big_bank_lines = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
    max_nof_80_bit_ent_per_bank = (max_nof_big_bank_lines * max_nof_tcam_big_banks);
    max_nof_entries_in_two_srams =
        (dnx_data_field.tcam.hit_indication_entries_per_byte_get(unit) * max_nof_srams_per_tcam_bank);

    hit_indication_value = 0;
    *is_entry_found_p = FALSE;
    entry_id_from_location = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_TCAM_TCAM_ACTION_HIT_INDICATION, &entry_handle_id));

    for (entry_id = 0; entry_id < max_nof_80_bit_ent_per_bank; entry_id++)
    {
        bank_id = (entry_id / max_nof_big_bank_lines);
        bank_line_id = ((entry_id % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
        bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (entry_id % 2));
        sram_line_entry_id =
            (((entry_id % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_BANK_SRAM_ID, bank_sram_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_BANK_LINE_ID, bank_line_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCAM_ENTRY_ID, sram_line_entry_id);
        dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TCAM_ENTRY_HIT, INST_SINGLE, &hit_indication_value);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** Check whether we have a hit. */
        if (hit_indication_value)
        {
            /** Get the entry id from its location. */
            dnx_field_tcam_access_sw.entry_location_hash.get_by_index(unit, core_id, entry_id, &entry_id_from_location,
                                                                      &found);

            if (found)
            {
                /**
                 * Get the info for the current entry which was hit.
                 * In case the entry_get() returns NOT_FOUND error, it means that,
                 * the current entry is relevant for the current FG ID.
                 * Then set the entry_id to its default value and continue
                 * iterating until we find the right entry for the current FG.
                 */
                SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
                SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_id_from_location, &entry_info));
                if (SHR_GET_CURRENT_ERR())
                {
                    entry_id_from_location = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
    }

    if (entry_id_from_location != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
    {
        last_diag_fg_info_p[fg_id_index].entry_id = entry_id_from_location;
        *is_entry_found_p = TRUE;
    }
    else
    {
        last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function retrieve information about last hit DT entry per field group.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id_index - Iterator value to know, on which place to save the entry id.
 * \param [in] fg_id - Field group ID, which performed lookups and have a hit.
 * \param [out] last_diag_fg_info_p - Stores entry which was hit per FG.
 * \param [out] is_entry_found_p - Indication if we find at least one entry.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_dt_entry_info_get(
    int unit,
    bcm_core_t core_id,
    int fg_id_index,
    dnx_field_group_t fg_id,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    uint8 *is_entry_found_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_entry_t entry_info;
    uint32 entry_handle;

    SHR_FUNC_INIT_VARS(unit);

    *is_entry_found_p = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
    /**
     * Calculate the entry_handle, using the first array of the key_values, because in
     * case of DT we have 11 bit key and the key is the actual entry offset in the bank (location).
     */
    entry_handle = DNX_FIELD_TCAM_ENTRY(fg_id, last_diag_fg_info_p[fg_id_index].key_values[0][0], core_id);
    SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
    }
    else
    {
        last_diag_fg_info_p[fg_id_index].entry_id = entry_handle;
        *is_entry_found_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about which FGs performed lookups and
 *   the keys which they are using.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] context_id - ID of the selected context.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [out] fg_performed_lookup_p - Array with field groups, which performed
 *  lookups and have a hit.
 * \param [out] num_fgs - Number of the field groups, which performed lookups.
 * \param [out] last_diag_fg_info_p - Should store Key IDs and their values,
 *  which are used by field groups.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_group_t * fg_performed_lookup_p,
    int *num_fgs,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    sh_sand_control_t * sand_control)
{
    uint8 is_fg_allocated;
    int ctx_id_index, fg_id_index, fg_per_ctx_index;
    int max_num_fgs;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    dnx_field_group_t fgs_per_context[dnx_data_field.group.nof_fgs_get(unit)];
    int key_id_iter;
    dnx_field_stage_e fg_stage;
    field_group_name_t group_name;
    uint8 add_new_fg_id;
    uint32 field_key_hit;
    dnx_field_group_type_e fg_type;
    int do_display;
    uint8 is_entry_found_p;
    /**
     * An array of 5 elements to store 160 bits
     * key information, from the signals.
     */
    uint32 field_key_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    do_display = FALSE;
    field_key_hit = 0;

    /** Get the max num of FGs from DNX data. */
    max_num_fgs = dnx_data_field.group.nof_fgs_get(unit);

    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    /**
     * Iterate over maximum number of FGs and take information for those, which are being
     * attached to the selected context.
     */
    fg_per_ctx_index = 0;
    for (fg_id_index = 0; fg_id_index < max_num_fgs; fg_id_index++)
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

    if (!fg_per_ctx_index)
    {
        LOG_CLI_EX("\r\n" "No attached Field Groups were found for context %d !!!%s%s%s\r\n\n", context_id, EMPTY,
                   EMPTY, EMPTY);
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    PRT_TITLE_SET("Field Group Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Id/Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Raw Data");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Hit");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Id");

    /** Print all used keys by the attached FGs. */
    for (fg_id_index = 0; fg_id_index < fg_per_ctx_index; fg_id_index++)
    {
        sal_memset(last_diag_fg_info_p[fg_id_index].key_ids, DNX_FIELD_KEY_ID_INVALID,
                   sizeof(last_diag_fg_info_p[fg_id_index].key_ids));

        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fgs_per_context[fg_id_index], &fg_type));

        switch (fg_type)
        {
            case DNX_FIELD_GROUP_TYPE_TCAM:
            case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fgs_per_context[fg_id_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], "N/A");
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
                PRT_CELL_SET("%s", converted_fg_type);

                /** Get attach info for the current context in fgs_per_context[]. */
                SHR_IF_ERR_EXIT(dnx_field_group_context_get
                                (unit, fgs_per_context[fg_id_index], context_id, attach_full_info));

                /**
                 * On every new iteration of FG IDs we should add new line with underscore separator.
                 * For this case we are using following flag add_new_fg_id, which is being set to TRUE for this purpose.
                 */
                add_new_fg_id = TRUE;
                /** Iterate over max number of keys in double key and check if we a valid key id. */
                for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
                     attach_full_info->key_id.id[key_id_iter] != DNX_FIELD_KEY_ID_INVALID; key_id_iter++)
                {
                    /** In case the flag is not set, we should skip the first cell related to FG ID. */
                    if (!add_new_fg_id)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(2);
                    }

                    PRT_CELL_SET("%s", Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);

                    /** We have to read different signals per key. */
                    switch (Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string[0])
                    {
                        case 'A':
                        {
                            /**
                             * In case of DE, read the key values from the debug registers, because
                             * the signals are not mapped.
                             */
                            if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                            {
                                SHR_IF_ERR_EXIT(diag_dnx_field_utils_last_key_table_info_get
                                                (unit, DBAL_TABLE_FIELD_IPMF3_LAST_KEY_INFO, core_id,
                                                 DBAL_ENUM_FVAL_FIELD_KEY_A, field_key_value));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, from_p, to_p, "Key_A", field_key_value, 5));
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, to_p, from_p, "Hit_A", &field_key_hit, 1));
                            }
                            break;
                        }
                        case 'B':
                        {
                            /**
                             * In case of DE, read the key values from the debug registers, because
                             * the signals are not mapped.
                             */
                            if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                            {
                                SHR_IF_ERR_EXIT(diag_dnx_field_utils_last_key_table_info_get
                                                (unit, DBAL_TABLE_FIELD_IPMF3_LAST_KEY_INFO, core_id,
                                                 DBAL_ENUM_FVAL_FIELD_KEY_B, field_key_value));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, from_p, to_p, "Key_B", field_key_value, 5));
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, to_p, from_p, "Hit_B", &field_key_hit, 1));
                            }
                            break;
                        }
                        case 'C':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, to_p, "Key_C", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, to_p, from_p, "Hit_C", &field_key_hit, 1));
                            break;
                        }
                        case 'D':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, to_p, "Key_D", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, to_p, from_p, "Hit_D", &field_key_hit, 1));
                            break;
                        }
                        case 'E':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, to_p, "Key_E", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, to_p, from_p, "Hit", &field_key_hit, 1));
                            break;
                        }
                        case 'F':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, to_p, "Key_F", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, to_p, from_p, "Hit_F", &field_key_hit, 1));
                            break;
                        }
                        case 'G':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, to_p, "Key_G", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, to_p, from_p, "Hit_G", &field_key_hit, 1));
                            break;
                        }
                        case 'H':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, to_p, "Key_H", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, to_p, from_p, "Hit_H", &field_key_hit, 1));
                            break;
                        }
                        case 'I':
                        {
                            /**
                             * In case of DE, read the key values from the debug registers, because
                             * the signals are not mapped.
                             */
                            if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                            {
                                SHR_IF_ERR_EXIT(diag_dnx_field_utils_last_key_table_info_get
                                                (unit, DBAL_TABLE_FIELD_IPMF2_LAST_KEY_INFO, core_id,
                                                 DBAL_ENUM_FVAL_FIELD_KEY_I, field_key_value));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, from_p, to_p, "Key_I", field_key_value, 5));
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, to_p, from_p, "Hit_I", &field_key_hit, 1));
                            }
                            break;
                        }
                        case 'J':
                        {
                            /**
                             * In case of DE, read the key values from the debug registers, because
                             * the signals are not mapped.
                             */
                            if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                            {
                                SHR_IF_ERR_EXIT(diag_dnx_field_utils_last_key_table_info_get
                                                (unit, DBAL_TABLE_FIELD_IPMF2_LAST_KEY_INFO, core_id,
                                                 DBAL_ENUM_FVAL_FIELD_KEY_J, field_key_value));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, from_p, to_p, "Key_J", field_key_value, 5));
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, to_p, from_p, "Hit", &field_key_hit, 1));
                            }
                            break;
                        }
                        default:
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key (%s).\r\n",
                                         Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
                        }
                    }
                    PRT_CELL_SET("0x%08x%08x%08x%08x%08x", field_key_value[4],
                                 field_key_value[3], field_key_value[2], field_key_value[1], field_key_value[0]);
                    /**
                     * In case of hit we should present 'Yes' in the relevant sell,
                     * and to update the output arrays.
                     */
                    if (field_key_hit)
                    {
                        PRT_CELL_SET("%s", "Yes");
                    }
                    /** In case of DE, there is not meaning of HIT, because of that print N/A. */
                    else if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                    {
                        PRT_CELL_SET("%s", "N/A");
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "No");
                    }

                    fg_performed_lookup_p[fg_id_index] = fgs_per_context[fg_id_index];
                    last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter] = attach_full_info->key_id.id[key_id_iter];
                    sal_memcpy(last_diag_fg_info_p[fg_id_index].key_values[key_id_iter], field_key_value,
                               sizeof(field_key_value));

                    /**
                     * In case the flag is set, it means that a new field group ID was added to the relevant column
                     * Then we should set value for Entry ID on this roll for this FG and after that to set
                     * the flag to FALSE.
                     */
                    if (add_new_fg_id)
                    {
                        SHR_IF_ERR_EXIT(diag_dnx_field_last_entry_info_get
                                        (unit, core_id, fg_id_index, fgs_per_context[fg_id_index], last_diag_fg_info_p,
                                         &is_entry_found_p, sand_control));

                        if (!is_entry_found_p && fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
                        {
                            PRT_CELL_SET("%s", "No Entries");
                        }
                        /** In case of DE, there is not meaning of ENTRY, because of that print N/A. */
                        else if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                        {
                            PRT_CELL_SET("%s", "N/A");
                        }
                        else
                        {
                            PRT_CELL_SET("0x%06X", last_diag_fg_info_p[fg_id_index].entry_id);
                        }

                        /**
                         * Set the flag to FALSE, if we have more then one KEY for current FG,
                         * we should skip the cells related to FG ID and Entry ID.
                         */
                        add_new_fg_id = FALSE;
                    }
                    else
                    {
                        /** In case the flag is not set we should skip the cell related to Entry ID. */
                        PRT_CELL_SKIP(1);
                    }

                    do_display = TRUE;
                }
                break;
            }
            case DNX_FIELD_GROUP_TYPE_CONST:
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fgs_per_context[fg_id_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], "N/A");
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
                PRT_CELL_SET("%s", converted_fg_type);

                fg_performed_lookup_p[fg_id_index] = fgs_per_context[fg_id_index];

                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                break;
            }
            case DNX_FIELD_GROUP_TYPE_EXEM:
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fgs_per_context[fg_id_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], "N/A");
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
                PRT_CELL_SET("%s", converted_fg_type);

                /** Get attach info for the current context in fgs_per_context[]. */
                SHR_IF_ERR_EXIT(dnx_field_group_context_get
                                (unit, fgs_per_context[fg_id_index], context_id, attach_full_info));

                /**
                 * On every new iteration of FG IDs we should add new line with underscore separator.
                 * For this case we are using following flag add_new_fg_id, which is being set to TRUE for this purpose.
                 */
                add_new_fg_id = TRUE;
                /** Iterate over max number of keys in double key and check if we a valid key id. */
                for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
                     attach_full_info->key_id.id[key_id_iter] != DNX_FIELD_KEY_ID_INVALID; key_id_iter++)
                {
                    PRT_CELL_SET("%s", Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);

                    /** We have to read different signals per key. */
                    switch (Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string[0])
                    {
                        case 'A':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, "LEXEM1", "Key_A", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "LEXEM1", from_p, "Hit_A", &field_key_hit, 1));
                            break;
                        }
                        case 'C':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, "SEXEM3", "Key_C", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "SEXEM3", from_p, "Hit_C", &field_key_hit, 1));
                            break;
                        }
                        case 'E':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, "LEXEM1", "Key_E", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "LEXEM1", from_p, "Hit", &field_key_hit, 1));
                            break;
                        }
                        case 'J':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, "SEXEM3orDE", "Key_J", field_key_value,
                                             5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "SEXEM3", from_p, "Hit", &field_key_hit, 1));
                            break;
                        }
                        default:
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key (%s).\r\n",
                                         Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
                        }
                    }
                    PRT_CELL_SET("0x%08x%08x%08x%08x%08x", field_key_value[4],
                                 field_key_value[3], field_key_value[2], field_key_value[1], field_key_value[0]);
                    /**
                     * In case of hit we should present 'Yes' in the relevant sell,
                     * and to update the output arrays.
                     */
                    if (field_key_hit)
                    {
                        PRT_CELL_SET("%s", "Yes");
                        /**
                         * In case of HIT, set entry ID to be different than
                         * DNX_FIELD_ENTRY_ACCESS_ID_INVALID (-1), it will be checked in next functions.
                         */
                        last_diag_fg_info_p[fg_id_index].entry_id = 0;
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "No");
                        /**
                         * In case no HIT, set entry ID to DNX_FIELD_ENTRY_ACCESS_ID_INVALID (-1),
                         * it will be checked in next functions.
                         */
                        last_diag_fg_info_p[fg_id_index].entry_id = DNX_FIELD_ENTRY_ACCESS_ID_INVALID;
                    }

                    last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter] = attach_full_info->key_id.id[key_id_iter];
                    sal_memcpy(last_diag_fg_info_p[fg_id_index].key_values[key_id_iter], field_key_value,
                               sizeof(field_key_value));

                    PRT_CELL_SET("%s", "N/A");

                    do_display = TRUE;
                }

                fg_performed_lookup_p[fg_id_index] = fgs_per_context[fg_id_index];

                break;
            }
            case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                /** Take the name for current FG from the SW state. */
                SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fgs_per_context[fg_id_index], &group_name));
                /**
                 * Check if Field Group name is being provided, in case no set N/A as default value,
                 * in other case set the provided name.
                 */
                if (sal_strncmp(group_name.value, "", sizeof(group_name.value)))
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], group_name.value);
                }
                else
                {
                    PRT_CELL_SET("%d/\"%s\"", fgs_per_context[fg_id_index], "N/A");
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));
                PRT_CELL_SET("%s", converted_fg_type);

                /** Get attach info for the current context in fgs_per_context[]. */
                SHR_IF_ERR_EXIT(dnx_field_group_context_get
                                (unit, fgs_per_context[fg_id_index], context_id, attach_full_info));

                /**
                 * On every new iteration of FG IDs we should add new line with underscore separator.
                 * For this case we are using following flag add_new_fg_id, which is being set to TRUE for this purpose.
                 */
                add_new_fg_id = TRUE;
                /** Iterate over max number of keys in double key and check if we a valid key id. */
                for (key_id_iter = 0; key_id_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX &&
                     attach_full_info->key_id.id[key_id_iter] != DNX_FIELD_KEY_ID_INVALID; key_id_iter++)
                {
                    PRT_CELL_SET("%s", Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);

                    /** We have to read different signals per key. */
                    switch (Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string[0])
                    {
                        case 'A':
                        {
                            if (field_stage == DNX_FIELD_STAGE_IPMF3)
                            {
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, from_p, "TCAMorDE", "Key_A", field_key_value,
                                                 5));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dpp_dsig_read
                                                (unit, core_id, block_p, from_p, "TCAM", "Key_A", field_key_value, 5));
                            }
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "TCAM", from_p, "Hit_A", &field_key_hit, 1));
                            break;
                        }
                        case 'B':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, "TCAM", "Key_B", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "TCAM", from_p, "Hit_B", &field_key_hit, 1));
                            break;
                        }
                        case 'F':
                        {
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, from_p, "TCAM", "Key_F", field_key_value, 5));
                            SHR_IF_ERR_EXIT(dpp_dsig_read
                                            (unit, core_id, block_p, "TCAM", from_p, "Hit_F", &field_key_hit, 1));
                            break;
                        }
                        default:
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal key (%s).\r\n",
                                         Field_key_enum_table[attach_full_info->key_id.id[key_id_iter]].string);
                        }
                    }
                    PRT_CELL_SET("0x%08x%08x%08x%08x%08x", field_key_value[4],
                                 field_key_value[3], field_key_value[2], field_key_value[1], field_key_value[0]);
                    /**
                     * In case of hit we should present 'Yes' in the relevant sell,
                     * and to update the output arrays.
                     */
                    if (field_key_hit)
                    {
                        PRT_CELL_SET("%s", "Yes");
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "No");
                    }

                    fg_performed_lookup_p[fg_id_index] = fgs_per_context[fg_id_index];
                    last_diag_fg_info_p[fg_id_index].key_ids[key_id_iter] = attach_full_info->key_id.id[key_id_iter];
                    sal_memcpy(last_diag_fg_info_p[fg_id_index].key_values[key_id_iter], field_key_value,
                               sizeof(field_key_value));

                    /**
                     * In case the flag is set, it means that a new field group ID was added to the relevant column
                     * Then we should set value for Entry ID on this roll for this FG and after that to set
                     * the flag to FALSE.
                     */
                    if (add_new_fg_id)
                    {
                        SHR_IF_ERR_EXIT(diag_dnx_field_last_dt_entry_info_get
                                        (unit, core_id, fg_id_index, fgs_per_context[fg_id_index], last_diag_fg_info_p,
                                         &is_entry_found_p, sand_control));

                        if (!is_entry_found_p)
                        {
                            PRT_CELL_SET("%s", "No Entries");
                        }
                        else
                        {
                            PRT_CELL_SET("0x%06X", last_diag_fg_info_p[fg_id_index].entry_id);
                        }

                        /**
                         * Set the flag to FALSE, if we have more then one KEY for current FG,
                         * we should skip the cells related to FG ID and Entry ID.
                         */
                        add_new_fg_id = FALSE;
                    }
                    else
                    {
                        /** In case the flag is not set we should skip the cell related to Entry ID. */
                        PRT_CELL_SKIP(1);
                    }

                    do_display = TRUE;
                }

                break;
            }
            case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            case DNX_FIELD_GROUP_TYPE_KBP:
            {
                break;
            }
            default:
            {
                /** None of the supported types. */
                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion(unit, fg_type, converted_fg_type));

                LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by Field Last Info diagnostic!! %s%s%s\r\n\n",
                           converted_fg_type, EMPTY, EMPTY, EMPTY);
                break;
            }
        }
    }

    *num_fgs = fg_per_ctx_index;

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
        LOG_CLI_EX("\r\n" "No info for FGs, to be presented!! %s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(attach_full_info);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers of a specific Entry.
 *   Used for TCAM FG.
 * \param [in] unit - The unit number.
 * \param [in] fg_id - Field Group ID.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_qual_print(
    int unit,
    dnx_field_group_t fg_id,
    uint8 is_hit_found,
    int qual_iter,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    prt_control_t * prt_ctr)
{
    int qual_val_mask_iter, val_iter;
    int max_val_iter;
    int max_num_bits_in_ffc;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_exp_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_mask_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint32 qual_lsb;
    uint32 single_key_size;
    int qual_valid;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size / max_num_bits_in_ffc);
    if ((max_val_iter % max_num_bits_in_ffc) != 0)
    {
        max_val_iter += 1;
    }
    else
    {
        max_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY - 4;
    }

    sal_strncpy(qual_exp_val_string, "0x", sizeof(qual_exp_val_string) - 1);
    sal_strncpy(qual_mask_string, "0x", sizeof(qual_mask_string) - 1);
    for (qual_val_mask_iter = max_val_iter - 1; qual_val_mask_iter >= 0; qual_val_mask_iter--)
    {
        char qual_val_mas_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%08X",
                                              entry_info_p->key_info.
                                              qual_info[qual_iter].qual_value[qual_val_mask_iter]);
        sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                     entry_info_p->key_info.qual_info[qual_iter].qual_value[qual_val_mask_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_exp_val_string, "%s", qual_val_mas_buff);
        sal_strncat(qual_exp_val_string, qual_val_mas_buff, sizeof(qual_exp_val_string) - 1);

        sal_strncpy(qual_val_mas_buff, "", sizeof(qual_val_mas_buff) - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%08X",
                                              entry_info_p->key_info.
                                              qual_info[qual_iter].qual_mask[qual_val_mask_iter]);
        sal_snprintf(qual_val_mas_buff, sizeof(qual_val_mas_buff), "%08X",
                     entry_info_p->key_info.qual_info[qual_iter].qual_mask[qual_val_mask_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_mask_string, "%s", qual_val_mas_buff);
        sal_strncat(qual_mask_string, qual_val_mas_buff, sizeof(qual_mask_string) - 1);
    }

    /** Check if the current qual is valid for the entry. */
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        qual_valid = utilex_bitstream_have_one_in_range(entry_info_p->key_info.qual_info[qual_iter].qual_value,
                                                        0,
                                                        WORDS2BITS(DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY) -
                                                        1);
    }
    else
    {
        qual_valid = utilex_bitstream_have_one_in_range(entry_info_p->key_info.qual_info[qual_iter].qual_mask,
                                                        0,
                                                        WORDS2BITS(DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY) -
                                                        1);
    }

    /** Print all needed info about current qualifier. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    if (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb >= single_key_size)
    {
        qual_lsb = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb - single_key_size;
    }
    else
    {
        qual_lsb = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb;
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p, qual_lsb,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                                                   qual_val));

    sal_strncpy(qual_val_string, "0x", sizeof(qual_val_string) - 1);
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat(qual_val_string, val_buff, sizeof(qual_val_string) - 1);
    }

    if (qual_valid)
    {
        PRT_CELL_SET("%s", qual_val_string);
        if (is_hit_found)
        {
            PRT_CELL_SET("%s", qual_exp_val_string);
            if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
            {
                PRT_CELL_SET("%s", qual_mask_string);
            }
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
            if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
            {
                PRT_CELL_SET("%s", "N/A");
            }
        }
    }
    else
    {
        PRT_CELL_SET("%s", "-");
        PRT_CELL_SET("%s", "-");
        if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
        {
            PRT_CELL_SET("%s", "-");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers by DE FGs.
 * \param [in] unit - The unit number.
 * \param [in] context_id - ID of the selected context.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] qual_size - Size of the current qualifier.
 * \param [in] qual_lsb - Lsb of the current qualifier,
 *  default should be '0'. Only in cases when the current qualifier
 *  is shared between two actions the value can be different.
 * \param [in] qual_msb - Msb of the current qualifier,
 *  default should be '0'. Only in cases when the current qualifier
 *  is shared between two actions the value can be different
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_de_qual_print(
    int unit,
    dnx_field_context_t context_id,
    int qual_iter,
    uint8 qual_size,
    uint8 qual_lsb,
    uint8 qual_msb,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    prt_control_t * prt_ctr)
{
    int val_iter;
    int max_val_iter;
    int max_num_bits_in_ffc;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    uint32 qual_lsb_internal;
    uint32 single_key_size;
    int qual_additional_offset;

    SHR_FUNC_INIT_VARS(unit);

    qual_additional_offset = 0;

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size / max_num_bits_in_ffc);
    if ((max_val_iter % max_num_bits_in_ffc) != 0)
    {
        max_val_iter += 1;
    }
    else
    {
        max_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY - 4;
    }

    /** Set current qualifier info. */
    if (qual_lsb != 0 || qual_msb != 0)
    {
        PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_qual_text
                     (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type), qual_lsb,
                     qual_msb);

        qual_additional_offset = qual_lsb;
    }
    else
    {
        PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                     (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));
    }

    PRT_CELL_SET("%d", (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb + qual_additional_offset));
    PRT_CELL_SET("%d", qual_size);

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    if (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb >= single_key_size)
    {
        qual_lsb_internal = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb - single_key_size;
    }
    else
    {
        qual_lsb_internal = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb +
            fg_info_p->group_full_info.context_info[context_id].key_id.bit_range.bit_range_offset;
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p, qual_lsb_internal,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                                                   qual_val));

    sal_strncpy(qual_val_string, "0x", sizeof(qual_val_string) - 1);
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat(qual_val_string, val_buff, sizeof(qual_val_string) - 1);
    }

    PRT_CELL_SET("%s", qual_val_string);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all needed information about used qualifiers of a specific Entry.
 *   Used for EXEM FG.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] qual_iter - Id of the current qualifier.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_exem_qual_print(
    int unit,
    uint8 is_hit_found,
    int qual_iter,
    uint32 *key_values_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_entry_t * entry_info_p,
    prt_control_t * prt_ctr)
{
    int qual_val_iter, val_iter;
    int max_val_iter;
    int max_num_bits_in_ffc;
    uint32 qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY];
    char qual_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char qual_exp_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size / max_num_bits_in_ffc);
    if ((max_val_iter % max_num_bits_in_ffc) != 0)
    {
        max_val_iter += 1;
    }
    else
    {
        max_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY - 4;
    }

    sal_strncpy(qual_exp_val_string, "0x", sizeof(qual_exp_val_string) - 1);
    for (qual_val_iter = max_val_iter - 1; qual_val_iter >= 0; qual_val_iter--)
    {
        char qual_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                              "%08X",
                                              entry_info_p->key_info.qual_info[qual_iter].qual_value[qual_val_iter]);
        sal_snprintf(qual_val_buff, sizeof(qual_val_buff), "%08X",
                     entry_info_p->key_info.qual_info[qual_iter].qual_value[qual_val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_exp_val_string, "%s", qual_val_buff);
        sal_strncat(qual_exp_val_string, qual_val_buff, sizeof(qual_exp_val_string) - 1);
    }

    /** Print all needed info about current qualifier. */
    PRT_CELL_SET("%s", dnx_field_dnx_qual_text
                 (unit, fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type));

    /** Extract the actual value of the current qual from the KEY. */
    sal_memset(qual_val, 0, sizeof(qual_val));
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb,
                                                   fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size,
                                                   qual_val));

    sal_strncpy(qual_val_string, "0x", sizeof(qual_val_string) - 1);
    for (val_iter = max_val_iter - 1; val_iter >= 0; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%08X", qual_val[val_iter]);
        sal_snprintf(val_buff, sizeof(val_buff), "%08X", qual_val[val_iter]);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(qual_val_string, "%s", val_buff);
        sal_strncat(qual_val_string, val_buff, sizeof(qual_val_string) - 1);
    }

    PRT_CELL_SET("%s", qual_val_string);
    if (is_hit_found)
    {
        PRT_CELL_SET("%s", qual_exp_val_string);
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
 *   This function sets all cells with information about performed FES actions of a specific Entry.
 *   Used for TCAM FGs.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] action_iter - Id of the current action.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] entry_info_p - Pointer, which contains all Entry related information.
 * \param [out] start_bit_p - In-out parameter, which is used to store the offset where the
 *  next action value should be written to action_result_buff_p.
 * \param [out] action_result_buff_p - The action result buffer, which is used to store
 *  the values for all FES actions.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_action_print(
    int unit,
    uint8 is_hit_found,
    bcm_core_t core_id,
    int action_iter,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    dnx_field_entry_t * entry_info_p,
    int *start_bit_p,
    uint32 *action_result_buff_p,
    prt_control_t * prt_ctr)
{
    unsigned int action_size;
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    int act_val_iter, sig_iter;
    uint32 action_val;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char action_val_exp_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int ent_action_iter;
    char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];
    int action_valid;

    SHR_FUNC_INIT_VARS(unit);

    action_valid = FALSE;
    ent_action_iter = 0;

    if (is_hit_found)
    {
        /** Iterate over entry actions array and check, which actions are valid. */
        for (ent_action_iter = 0; ent_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
             entry_info_p->payload_info.action_info[ent_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID;
             ent_action_iter++)
        {
            if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action ==
                entry_info_p->payload_info.action_info[ent_action_iter].dnx_action)
            {
                action_valid = TRUE;
                break;
            }
        }
    }

    sal_strncpy(action_val_string, "", sizeof(action_val_string) - 1);
    sal_strncpy(action_val_exp_string, "0x", sizeof(action_val_exp_string) - 1);

    /** Get action size, base action and BCM equivalent for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                  fg_info_p->group_full_info.
                                                  actions_payload_info.actions_on_payload_info[0][action_iter].
                                                  dnx_action, &action_size));
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action,
                     &bcm_action));
    /**
     * Check if the action is not user defined, then we shouldn't get its base action.
     * Otherwise get the base action.
     */
    if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
    {
        base_dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                        (unit, fg_info_p->group_basic_info.field_stage,
                         fg_info_p->group_full_info.actions_payload_info.
                         actions_on_payload_info[0][action_iter].dnx_action, &base_dnx_action));
    }

    if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        PRT_CELL_SET("%s", "None");
    }
    else
    {
        PRT_CELL_SET("FES %d", attach_full_info_p->actions_fes_info.fes_quartets[action_iter].fes_id);
    }

    PRT_CELL_SET("%s",
                 dnx_field_dnx_action_text(unit,
                                           fg_info_p->group_full_info.actions_payload_info.
                                           actions_on_payload_info[0][action_iter].dnx_action));
    /** If the action is user define, then we print the base action type. */
    if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
    {
        PRT_CELL_SET("%s", "N/A");
    }
    else
    {
        PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                     (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
    }

    if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        PRT_CELL_SET("%s", "N/A");
    }
    else
    {
        PRT_CELL_SET("%s", "Always Valid");
    }

    if (action_valid)
    {
        for (act_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY - 1; act_val_iter >= 0; act_val_iter--)
        {
            char act_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL,
                                                  "%X",
                                                  entry_info_p->payload_info.
                                                  action_info[ent_action_iter].action_value[act_val_iter]);
            sal_snprintf(act_val_buff, sizeof(act_val_buff), "%X",
                         entry_info_p->payload_info.action_info[ent_action_iter].action_value[act_val_iter]);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_exp_string, "%s", act_val_buff);
            sal_strncat(action_val_exp_string, act_val_buff, sizeof(action_val_exp_string) - 1);

            /** Store the action result values in the action_result_buff_p. */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (entry_info_p->payload_info.action_info[ent_action_iter].action_value, *start_bit_p,
                             action_size, action_result_buff_p));
            *start_bit_p += action_size;
        }

        if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
        {
            PRT_CELL_SET("%s", "Void Action");
        }
        else
        {
            if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, fg_info_p->group_basic_info.field_stage,
                                                                 fg_info_p->group_full_info.
                                                                 actions_payload_info.actions_on_payload_info[0]
                                                                 [action_iter].dnx_action, action_signals));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, fg_info_p->group_basic_info.field_stage,
                                                                 base_dnx_action, action_signals));
            }
            /** Iterate over all signals related to the current action. */
            for (sig_iter = 0; sig_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; sig_iter++)
            {
                char sig_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                /** Check if the current action signal is empty, then break. */
                if (action_signals[sig_iter] == 0)
                {
                    break;
                }
                /** Retrieve the action value from the relevant signal. */
                SHR_IF_ERR_EXIT(dpp_dsig_read
                                (unit, core_id, block_p, from_p, to_p, action_signals[sig_iter], &action_val, 1));

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s: 0x%X\n", action_signals[sig_iter], action_val);
                sal_snprintf(sig_val_buff, sizeof(sig_val_buff), "%s: 0x%X\n", action_signals[sig_iter], action_val);
                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_string, "%s", sig_val_buff);
                sal_strncat(action_val_string, sig_val_buff, sizeof(action_val_string) - 1);
            }

            PRT_CELL_SET("%s", action_val_string);
        }
        PRT_CELL_SET("%s", action_val_exp_string);
    }
    else
    {
        if (is_hit_found)
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%s", "-");
        }
        else
        {
            PRT_CELL_SET("%s", "N/A");
            PRT_CELL_SET("%s", "N/A");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about performed FES actions of a DE FGs.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id - DE Field group ID.
 * \param [in] action_iter - Id of the current action.
 * \param [in] action_size - Id of the current action.
 * \param [in] action_lsb - Lsb of the current action,
 *  default should be '0'. Only in cases when the current action
 *  is shared between two qualifiers the value can be different.
 * \param [in] action_msb - Msb of the current action,
 *  default should be '0'. Only in cases when the current action
 *  is shared between two qualifiers the value can be different.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_de_action_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_group_t fg_id,
    int action_iter,
    unsigned int action_size,
    unsigned int action_lsb,
    unsigned int action_msb,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    int sig_iter;
    uint32 action_val;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];
    int action_offset;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(action_val_string, "", sizeof(action_val_string) - 1);
    action_offset = 0;

    /** Take the offset for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id,
                                                      fg_info_p->group_full_info.
                                                      actions_payload_info.actions_on_payload_info[0][action_iter].
                                                      dnx_action, &action_offset));

    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action,
                     &bcm_action));
    /**
     * Check if the action is not user defined, then we shouldn't get its base action.
     * Otherwise get the base action.
     */
    if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
    {
        base_dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                        (unit, fg_info_p->group_basic_info.field_stage,
                         fg_info_p->group_full_info.actions_payload_info.
                         actions_on_payload_info[0][action_iter].dnx_action, &base_dnx_action));
    }

    if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        PRT_CELL_SET("%s", "None");
    }
    else
    {
        PRT_CELL_SET("FES %d", attach_full_info_p->actions_fes_info.fes_quartets[action_iter].fes_id);
    }

    if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dont_use_valid_bit)
    {
        if (action_lsb != 0 || action_msb != 0)
        {
            /** Set current action information. */
            PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_action_text(unit,
                                                                 fg_info_p->group_full_info.
                                                                 actions_payload_info.actions_on_payload_info[0]
                                                                 [action_iter].dnx_action), action_lsb, action_msb);
        }
        else
        {
            /** Set current action information. */
            PRT_CELL_SET("%s", dnx_field_dnx_action_text(unit,
                                                         fg_info_p->group_full_info.
                                                         actions_payload_info.actions_on_payload_info[0][action_iter].
                                                         dnx_action));
        }

        /** If the action is user define, then we print the base action type. */
        if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else
        {
            PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                         (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
        }

        if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else
        {
            PRT_CELL_SET("%s", "Always Valid");
        }
    }
    else
    {
        if (action_lsb == 0 && action_msb != 0)
        {
            /** Set current action information. */
            PRT_CELL_SET("%s [%d:%d] + condition", dnx_field_dnx_action_text(unit,
                                                                             fg_info_p->group_full_info.
                                                                             actions_payload_info.actions_on_payload_info
                                                                             [0][action_iter].dnx_action), action_lsb,
                         action_msb);
        }
        else if (action_lsb != 0 && action_msb > action_size)
        {
            /** Set current action information. */
            PRT_CELL_SET("%s [%d:%d]", dnx_field_dnx_action_text(unit,
                                                                 fg_info_p->group_full_info.
                                                                 actions_payload_info.actions_on_payload_info[0]
                                                                 [action_iter].dnx_action), action_lsb, action_msb);
        }
        else
        {
            /** Set current action information. */
            PRT_CELL_SET("%s + condition", dnx_field_dnx_action_text(unit,
                                                                     fg_info_p->group_full_info.
                                                                     actions_payload_info.actions_on_payload_info[0]
                                                                     [action_iter].dnx_action));
        }

        /** If the action is user define, then we print the base action type. */
        if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else
        {
            PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                         (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));
        }

        if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else
        {
            PRT_CELL_SET("%s", "Packet Value");
        }
    }

    if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        PRT_CELL_SET("%s", "Void Action");
    }
    else
    {
        if (bcm_action < BCM_FIELD_FIRST_USER_ACTION_ID)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, fg_info_p->group_basic_info.field_stage,
                                                             fg_info_p->group_full_info.
                                                             actions_payload_info.actions_on_payload_info[0]
                                                             [action_iter].dnx_action, action_signals));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, fg_info_p->group_basic_info.field_stage,
                                                             base_dnx_action, action_signals));
        }
        /** Iterate over all signals related to the current action. */
        for (sig_iter = 0; sig_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; sig_iter++)
        {
            char sig_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
            /** Check if the current action signal is empty, then break. */
            if (action_signals[sig_iter] == 0)
            {
                break;
            }

            /** Retrieve the action value from the relevant signal. */
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, core_id, block_p, from_p, to_p, action_signals[sig_iter], &action_val, 1));

            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s: 0x%X\n", action_signals[sig_iter], action_val);
            sal_snprintf(sig_val_buff, sizeof(sig_val_buff), "%s: 0x%X\n", action_signals[sig_iter], action_val);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_string, "%s", sig_val_buff);
            sal_strncat(action_val_string, sig_val_buff, sizeof(action_val_string) - 1);
        }

        PRT_CELL_SET("%s", action_val_string);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about performed FES actions,
 *   of a specific CONST FGs.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] action_iter - Id of the current action.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_const_fg_action_print(
    int unit,
    bcm_core_t core_id,
    int action_iter,
    char *block_p,
    char *from_p,
    char *to_p,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    prt_control_t * prt_ctr)
{
    dnx_field_action_t base_dnx_action;
    bcm_field_action_t bcm_action;
    int sig_iter;
    uint32 action_val;
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char *action_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION];

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(action_val_string, "", sizeof(action_val_string) - 1);

    /** Get base action and BCM equivalent for the current action. */
    SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action,
                     &bcm_action));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                    (unit, fg_info_p->group_basic_info.field_stage,
                     fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action,
                     &base_dnx_action));

    if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        PRT_CELL_SET("%s", "None");
    }
    else
    {
        PRT_CELL_SET("FES %d", attach_full_info_p->actions_fes_info.fes_quartets[action_iter].fes_id);
    }

    PRT_CELL_SET("%s",
                 dnx_field_dnx_action_text(unit,
                                           fg_info_p->group_full_info.actions_payload_info.
                                           actions_on_payload_info[0][action_iter].dnx_action));

    PRT_CELL_SET("%s (%d)", dnx_field_dnx_action_text(unit, base_dnx_action),
                 (bcm_action - BCM_FIELD_FIRST_USER_ACTION_ID));

    if (DNX_ACTION_ID(base_dnx_action) == DNX_FIELD_SW_ACTION_VOID)
    {
        PRT_CELL_SET("%s", "Void Action");
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_signals(unit, fg_info_p->group_basic_info.field_stage,
                                                         base_dnx_action, action_signals));

        /** Iterate over all signals related to the current action. */
        for (sig_iter = 0; sig_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; sig_iter++)
        {
            char sig_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
            /** Check if the current action signal is empty, then break. */
            if (action_signals[sig_iter] == 0)
            {
                break;
            }

            /** Retrieve the action value from the relevant signal. */
            SHR_IF_ERR_EXIT(dpp_dsig_read
                            (unit, core_id, block_p, from_p, to_p, action_signals[sig_iter], &action_val, 1));

            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s: 0x%X\n", action_signals[sig_iter], action_val);
            sal_snprintf(sig_val_buff, sizeof(sig_val_buff), "%s: 0x%X\n", action_signals[sig_iter], action_val);
            DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_string, "%s", sig_val_buff);
            sal_strncat(action_val_string, sig_val_buff, sizeof(action_val_string) - 1);
        }

        PRT_CELL_SET("%s", action_val_string);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets all cells with information about performed FEM actions of a specific FG.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id - Field Group Id to retrieve the used FEMs for.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] key_values_p - Values of the used Keys.
 * \param [in] action_result_buff_p - The action result buffer, which is used in case of
 *  TCAM FG and bcmFieldFemExtractionOutputSourceTypeKey, because we are not taking the values
 *  form the key, they are taken from the result buffer.
 * \param [out] fem_start_index_p - The FEM ID index, which was found as allocated,
 *  and which info was presented. Will be checked in the upper function.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fem_actions_print(
    int unit,
    uint8 is_hit_found,
    bcm_core_t core_id,
    dnx_field_group_t fg_id,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 *key_values_p,
    uint32 *action_result_buff_p,
    dnx_field_fem_id_t * fem_start_index_p,
    prt_control_t * prt_ctr)
{
    int sig_iter;
    uint32 action_val;
    uint32 action_val_bitmap[1] = { 0 };
    char action_val_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char action_val_exp_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
    bcm_field_fem_action_info_t fem_action_info;
    dnx_field_fem_id_t dnx_fem_id, max_nof_fems;
    unsigned int bit_in_fem_action;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    uint32 fem_condition_bits_max;
    uint8 add_new_fem_id;
    uint32 condition_bits_value;
    bcm_field_action_priority_t encoded_position;
    int nof_fem_id_per_array;
    dnx_field_action_position_t fem_position;
    dnx_field_action_array_t fem_array;
    uint8 do_fem_display;

    dnx_field_group_type_e fg_type;
    dnx_field_stage_e fg_stage;
    dnx_field_action_t dnx_action;
    dnx_field_action_t dnx_action_id;

    SHR_FUNC_INIT_VARS(unit);

    do_fem_display = FALSE;

    /** Get the Field Group type. */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    /*
     * Maximum number of FEM 'map indices' in iPMF1/2 == 4
     * This is the number of actions available per FEM
     */
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    max_nof_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    fem_condition_bits_max =
        dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_condition;
    nof_fem_id_per_array = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id_per_array;

    /** Set the starting index for dnx_fem_id to be equal to the last allocated FES + '1'. */
    for (dnx_fem_id = *fem_start_index_p; dnx_fem_id < max_nof_fems; dnx_fem_id++)
    {
        /*
         * If a match is found then display all four 'actions' corresponding to this FEM id
         */
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            dnx_field_group_t returned_fg_id;

            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, dnx_fem_id, fem_program_index, &returned_fg_id));
            if (returned_fg_id == DNX_FIELD_GROUP_INVALID || returned_fg_id != fg_id)
            {
                /*
                 * This 'fem_program' is not occupied for specified 'fem_id'. Go to the next.
                 */
                continue;
            }

            /**
             * We should present only valid FEM IDs.
             * For this case we are using following flag add_new_fem_id, which is being set to TRUE for this purpose.
             */
            add_new_fem_id = TRUE;

            /** Calculate the fem_array and fem_position from the current dnx_fem_id. */
            fem_array = (((dnx_fem_id / nof_fem_id_per_array) * 2) + 1);
            fem_position = (dnx_fem_id % nof_fem_id_per_array);
            encoded_position = BCM_FIELD_ACTION_POSITION(fem_array, fem_position);
            SHR_IF_ERR_EXIT(bcm_field_fem_action_info_get
                            (unit, (bcm_field_group_t) fg_id, encoded_position, &fem_action_info));

            for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
            {
                /**
                 * Check if the action_iter is still in the actions range (dnx_data_field.stage.stage_info_get(unit)->nof_fem_map_index) and if the
                 * current action is invalid (bcmFieldActionCount), we should continue iterating over all actions.
                 */
                if (fem_map_index < fem_map_index_max
                    && fem_action_info.fem_extraction[fem_map_index].action_type == bcmFieldActionCount)
                {
                    continue;
                }

                /** In case the flag is set, we should set value of the current FEM ID on this roll. */
                if (add_new_fem_id)
                {
                    PRT_CELL_SET("FEM %d", dnx_fem_id);

                    /** Set the flag to FALSE, if we have more then one action for the current FEM, we should skip FEM ID cell. */
                    add_new_fem_id = FALSE;
                }
                else
                {
                    /** Add new row, without separator on this, if we have more than one valid action for current FEM. */
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    /**
                     * In case we have more than one valid action, we should skip all the cells related to qualifiers
                     * on this row, plus the cell about FEM ID.
                     */
                    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                    {
                        PRT_CELL_SKIP(6);
                    }
                    else
                    {
                        PRT_CELL_SKIP(8);
                    }
                }

                PRT_CELL_SET("%s",
                             dnx_field_bcm_action_text(unit,
                                                       fem_action_info.fem_extraction[fem_map_index].action_type));
                PRT_CELL_SET("%s", "N/A");

                for (bit_in_fem_action = 0;
                     bit_in_fem_action < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION; bit_in_fem_action++)
                {
                    /**
                     * In case of TCAM FG we should extract the 4 condition bits from the action_result_buff_p,
                     * otherwise we extract them from the key_values.
                     */
                    if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM)
                    {
                        /** Extract the condition bits value from the key_values. */
                        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p,
                                                                       ((fem_action_info.fem_input.input_offset +
                                                                         (fem_action_info.condition_msb + 1)) -
                                                                        fem_condition_bits_max), fem_condition_bits_max,
                                                                       &condition_bits_value));
                    }
                    else
                    {
                        /** Extract the condition bits value from the action_res_buff. */
                        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(action_result_buff_p,
                                                                       ((fem_action_info.fem_input.input_offset +
                                                                         (fem_action_info.condition_msb + 1)) -
                                                                        fem_condition_bits_max), fem_condition_bits_max,
                                                                       &condition_bits_value));
                    }

                    switch (fem_action_info.fem_extraction[fem_map_index].output_bit[bit_in_fem_action].source_type)
                    {
                        case bcmFieldFemExtractionOutputSourceTypeKey:
                        {
                            /** Variable to store the value of action bit taken from the key. */
                            uint32 output_key_action_bit_value;

                            if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM)
                            {
                                /**
                                 * If the source_type is KEY, we should take the relevant bit from the key.
                                 */
                                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                                                (key_values_p,
                                                 (fem_action_info.fem_input.input_offset +
                                                  fem_action_info.
                                                  fem_extraction[fem_map_index].output_bit[bit_in_fem_action].offset),
                                                 1, &output_key_action_bit_value));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(action_result_buff_p,
                                                                               (fem_action_info.fem_input.input_offset +
                                                                                fem_action_info.fem_extraction
                                                                                [fem_map_index].output_bit
                                                                                [bit_in_fem_action].offset), 1,
                                                                               &output_key_action_bit_value));
                            }

                            SHR_IF_ERR_EXIT(utilex_bitstream_set
                                            (action_val_bitmap, bit_in_fem_action, output_key_action_bit_value));
                            break;
                        }
                        case bcmFieldFemExtractionOutputSourceTypeForce:
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_set(action_val_bitmap, bit_in_fem_action,
                                                                 fem_action_info.
                                                                 fem_extraction[fem_map_index].output_bit
                                                                 [bit_in_fem_action].forced_value));
                            break;
                        }
                        default:
                        {
                            /** In case no bit is set by the user or taken from the key, set its default value '0'. */
                            SHR_IF_ERR_EXIT(utilex_bitstream_set(action_val_bitmap, bit_in_fem_action, 0));
                            break;
                        }
                    }
                }

                PRT_CELL_SET("0x%X", condition_bits_value);

                action_val_bitmap[0] += fem_action_info.fem_extraction[fem_map_index].increment;

                DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "0x%X", action_val_bitmap[0]);
                sal_snprintf(action_val_exp_string, sizeof(action_val_exp_string), "0x%X", action_val_bitmap[0]);

                sal_strncpy(action_val_string, "", sizeof(action_val_string) - 1);

                SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &fg_stage));
                SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, fg_stage,
                                                                fem_action_info.
                                                                fem_extraction[fem_map_index].action_type,
                                                                &dnx_action));
                dnx_action_id = DNX_ACTION_ID(dnx_action);

                /** Iterate over all signals related to the current action. */
                for (sig_iter = 0; sig_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; sig_iter++)
                {
                    char sig_val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE];
                    /** Break in case we reach an empty string, which is indicating the end of signals array. */
                    if (dnx_field_map_stage_info[fg_stage].static_action_info[dnx_action_id].signal_name[sig_iter] == 0)
                    {
                        break;
                    }

                    /** Retrieve the action value from the relevant signal. */
                    SHR_IF_ERR_EXIT(dpp_dsig_read
                                    (unit, core_id, block_p, from_p, to_p,
                                     dnx_field_map_stage_info[fg_stage].
                                     static_action_info[dnx_action_id].signal_name[sig_iter], &action_val, 1));

                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(NULL, "%s: 0x%X\n",
                                                          dnx_field_map_stage_info[fg_stage].static_action_info
                                                          [dnx_action_id].signal_name[sig_iter], action_val);
                    sal_snprintf(sig_val_buff, sizeof(sig_val_buff), "%s: 0x%X\n",
                                 dnx_field_map_stage_info[fg_stage].
                                 static_action_info[dnx_action_id].signal_name[sig_iter], action_val);
                    DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(action_val_string, "%s", sig_val_buff);
                    sal_strncat(action_val_string, sig_val_buff, sizeof(action_val_string) - 1);
                }

                if (is_hit_found)
                {
                    PRT_CELL_SET("%s", action_val_string);
                    PRT_CELL_SET("%s", action_val_exp_string);
                }
                else
                {
                    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                    {
                        PRT_CELL_SET("%s", action_val_string);
                    }
                    else
                    {
                        PRT_CELL_SET("%s", "N/A");
                        PRT_CELL_SET("%s", "N/A");
                    }
                }
            }

            do_fem_display = TRUE;
        }

        /** Return the current FEM ID to be checked in upper function. */
        *fem_start_index_p = dnx_fem_id;

        /**
         * In case we find and set values for the current FEM, we should go to
         * exit and continue presenting information about qualifiers and actions.
         */
        if (do_fem_display)
        {
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info per field group, which key was used and
 *  its key template. Shows all configured qualifiers and actions
 *  with the expected and actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] is_hit_found - Indicates if there is a hit or no.
 * \param [in] none_fem_occupied - Indicates if there are no occupied FEMs.
 * \param [in] nof_allocated_fems - Number of allocated FEMs for current FG.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] entry_info_p - Contains all Entry related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] key_values - Values of the used Keys.
 * \param [in] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_common_fg_key_payload_print(
    int unit,
    uint8 is_hit_found,
    int none_fem_occupied,
    dnx_field_fem_id_t nof_allocated_fems,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    dnx_field_entry_t * entry_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 key_values[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX][DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY],
    uint8 *do_display_any_fg_info_p,
    sh_sand_control_t * sand_control)
{
    int max_num_quals;
    uint32 action_result_buff[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY] = { 0 };
    int start_bit;
    int max_num_actions;
    int max_num_qual_actions;
    int qual_left, action_left, fem_left;
    uint32 max_num_qual_actions_fems;
    dnx_field_fem_id_t max_num_fems;
    int qual_action_iter;
    dnx_field_fem_id_t fem_id;
    uint8 key_index;
    uint32 single_key_size;
    uint8 add_new_fg_id;
    uint8 do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    max_num_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);
    max_num_qual_actions_fems = max_num_qual_actions + max_num_fems;
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    start_bit = 0;
    /** Used as flags to indicate if we have any quals/actions(fes, fem) to print. */
    qual_left = action_left = fem_left = TRUE;
    add_new_fg_id = TRUE;
    do_display = FALSE;
    fem_id = 0;

    /** Iterate over maximum number of qualifiers per FG, to retrieve information about these, which are valid. */
    for (qual_action_iter = 0; qual_action_iter < max_num_qual_actions_fems; qual_action_iter++)
    {
        /** In case the lsb of the current qualifier is more than 160 or equal to it,
         * we should take the data from the second KEY with index '1', otherwise from
         * first KEY with index '0'.
         */
        if (fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].lsb >= single_key_size)
        {
            key_index = 1;
        }
        else
        {
            key_index = 0;
        }

        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_action_iter < max_num_quals
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_action_iter].qual_type ==
            DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (qual_action_iter < max_num_actions
            && fg_info_p->group_full_info.actions_payload_info.
            actions_on_payload_info[0][qual_action_iter].dnx_action == DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * Do some checks in case of which we should stop printing FEMs:
         *      - fem_id is equal to maximum number of FEMs
         *      - the nof_allcated_fems was reduced to zero during the printing of FEMs
         *      - if they are any occupied FEMs for current FG (none_fem_occupied)
         */
        if ((fem_id == max_num_fems) || (nof_allocated_fems == 0) || none_fem_occupied)
        {
            fem_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left && !fem_left)
        {
            break;
        }

        /** In case the flag is set, we should set value for FG ID cell on this roll. */
        if (add_new_fg_id)
        {
            if (is_hit_found)
            {
                PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "HIT");
            }
            else
            {
                PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "NO HIT");
            }
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Key Value");
            if (fg_info_p->group_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Key Mask");
            }
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES/\nFEM\nId");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid/\nCondition\nbit");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signal Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Entry Payload Value");

            /** Set the flag to FALSE, if we have more then one key for the current FG, we should skip FG ID cell. */
            add_new_fg_id = FALSE;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        /**
         * If we still have any valid qualifiers (qual_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the qualifiers,
         * as it is mentioned in the 'else' statement.
         */
        if (qual_left)
        {
            /** Print the qualifiers info. */
            if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_qual_print
                                (unit, fg_id, is_hit_found, qual_action_iter, key_values[key_index], fg_info_p,
                                 entry_info_p, prt_ctr));
            }
            else
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_exem_qual_print
                                (unit, is_hit_found, qual_action_iter, key_values[key_index], fg_info_p, entry_info_p,
                                 prt_ctr));
            }
        }
        else
        {
            /**
             * Skip 4 cells (Qual, Pak Val, Val, Mask) about qualifier info if no more valid qualifiers were found,
             * but we still have valid actions to be printed.
             */
            if (fg_info_p->group_basic_info.fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
            {
                PRT_CELL_SKIP(4);
            }
            else
            {
                /** For EXEM FGs we don't have Mask column. */
                PRT_CELL_SKIP(3);
            }
        }

        /** Set an empty column to separate qualifiers and actions. */
        PRT_CELL_SET(" ");

        /**
         * If we still have any valid actions (action_left or fem_left), then print information about them.
         * Otherwise skip the cells, which are relevant to the actions,
         * as it is mentioned in the 'else' statement.
         */
        if (action_left)
        {
            /** Print the FES actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_action_print
                            (unit, is_hit_found, core_id, qual_action_iter, block_p, from_p, to_p, fg_info_p,
                             attach_full_info_p, entry_info_p, &start_bit, action_result_buff, prt_ctr));
        }
        else if (fem_left)
        {
            /** Print the FEM actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fem_actions_print
                            (unit, is_hit_found, core_id, fg_id, block_p, from_p, to_p,
                             key_values[key_index], action_result_buff, &fem_id, prt_ctr));
            /**
             * Increase the returned fem_id_p, because it is used as starting index for the loop in
             * diag_dnx_field_last_fem_actions_print().
             */
            fem_id += 1;
            /**
             * Reduce the number of allocated FEMs, to use it as indication
             * if we still have FEMs to present.
             */
            nof_allocated_fems--;
        }
        else
        {
            /**
             * Skip 6 cells (Fes/Fem, Actions, Base act, Valid/Cond, Perf Val, Val) about action info if no more valid actions were found,
             * but we still have valid qualifiers to be printed.
             */
            PRT_CELL_SKIP(6);
        }

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info for DE field groups, which key was used and
 *  its key template. Shows all configured qualifiers and actions
 *  with the actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] none_fem_occupied - Indicates if there are no occupied FEMs.
 * \param [in] nof_allocated_fems - Number of allocated FEMs for current FG.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] key_values - Values of the used Keys.
 * \param [in] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_de_fg_key_payload_print(
    int unit,
    int none_fem_occupied,
    dnx_field_fem_id_t nof_allocated_fems,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint32 key_values[DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX][DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY],
    uint8 *do_display_any_fg_info_p,
    sh_sand_control_t * sand_control)
{
    int max_num_quals;
    int max_num_actions;
    int max_num_qual_actions;
    int qual_left, action_left, fem_left;
    uint32 max_num_qual_actions_fems;
    dnx_field_fem_id_t max_num_fems;
    dnx_field_fem_id_t fem_id;
    uint8 key_index;
    uint32 single_key_size;
    int qual_iter, action_iter;
    uint8 print_new_row;
    unsigned int action_size;
    uint8 qual_size;
    int qual_total_size, action_total_size;
    int qual_size_residue, action_size_residue;
    int qual_current_size, action_current_size;
    uint8 add_new_fg_id;
    uint8 do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_quals = dnx_data_field.group.nof_quals_per_fg_get(unit);
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);
    max_num_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    /** The max number of qualifiers and actions per field group (32). */
    max_num_qual_actions = MAX(max_num_quals, max_num_actions);
    max_num_qual_actions_fems = max_num_qual_actions + max_num_fems;
    single_key_size = dnx_data_field.tcam.key_size_single_get(unit);

    /** Used as flags to indicate if we have any quals/actions to print. */
    qual_left = action_left = fem_left = TRUE;
    add_new_fg_id = TRUE;
    do_display = FALSE;
    print_new_row = TRUE;

    fem_id = 0;

    /** Action and qualifier sizes. */
    action_size = qual_size = 0;
    /** Sum of action and qualifier sizes. */
    action_total_size = qual_total_size = 0;
    /** Will store the residue of the action sizes in case there is any. */
    action_size_residue = qual_size_residue = 0;
    action_current_size = qual_current_size = 0;

    /** Iterate over maximum number of qualifiers per FG, to retrieve information about these, which are valid. */
    for (qual_iter = 0, action_iter = 0;
         (qual_iter < max_num_qual_actions_fems) && (action_iter < max_num_qual_actions_fems);)
    {
        /** In case the lsb of the current qualifier is more than 160 or equal to it,
         * we should take the data from the second KEY with index '1', otherwise from
         * first KEY with index '0'.
         */
        if (fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].lsb >= single_key_size)
        {
            key_index = 1;
        }
        else
        {
            key_index = 0;
        }

        /**
         * Check if the qual_action_iter is still in the qualifiers range (DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) and if the
         * current qualifier is invalid (DNX_FIELD_QUAL_TYPE_INVALID), we should set the qual_left to FALSE,
         * which will indicate that we should stop printing any info related to qualifiers.
         */
        if (qual_iter < max_num_quals
            && fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            qual_left = FALSE;
        }
        /**
         * Check if the qual_action_iter is still in the actions range (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) and if the
         * current action is invalid (DNX_FIELD_ACTION_INVALID), we should set the action_left to FALSE,
         * which will indicate that we should stop printing any info related to actions.
         */
        if (action_iter < max_num_actions
            && fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            action_left = FALSE;
        }
        /**
         * Do some checks in case of which we should stop printing FEMs:
         *      - fem_id is equal to maximum number of FEMs
         *      - the nof_allcated_fems was reduced to zero during the printing of FEMs
         *      - if they are any occupied FEMs for current FG (none_fem_occupied)
         */
        if ((fem_id == max_num_fems) || (nof_allocated_fems == 0) || none_fem_occupied)
        {
            fem_left = FALSE;
        }
        /**
         * In case we reached the end of both qualifiers and actions arrays,
         * which means that qual_left and action_left flags are being set to FALSE.
         * We have to break and to stop printing information for both.
         */
        if (!qual_left && !action_left && !fem_left)
        {
            break;
        }

        /** Get the size of the current qualifier. */
        if (qual_left)
        {
            qual_size = fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size;
        }

        /** Get the size of the current action. */
        if (action_left)
        {
            /** Get action size for the current action. */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                          fg_info_p->group_full_info.
                                                          actions_payload_info.actions_on_payload_info[0][action_iter].
                                                          dnx_action, &action_size));

            /** In case we have valid bit, we should add 1 bit to the actions size. */
            if (!
                (fg_info_p->group_full_info.actions_payload_info.
                 actions_on_payload_info[0][action_iter].dont_use_valid_bit))
            {
                action_size += 1;
            }
        }

        /** In case the flag is set, we should set value for FG ID cell on this roll. */
        if (add_new_fg_id)
        {
            PRT_TITLE_SET("Key Info & Performed Actions for FG ID %d (%s)", fg_id, "DE");

            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual LSB");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Qual Size");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Packet Value");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, " ");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES/\nFEM\nId");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Valid/\nCondition\nbit");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signal Value");

            /** Set the flag to FALSE, if we have more then one key for the current FG, we should skip FG ID cell. */
            add_new_fg_id = FALSE;
        }

        /**
         * In case the qual_size is lower than the actions_size, it means that we have more then
         * one qualifier in the key, from where the action will take its value.
         */
        if (qual_size < action_size && qual_left)
        {
            /** Calculate the total qualifiers size. */
            qual_total_size += qual_size;

            /**
             * Add new row with underscore separator in case we have to print
             * qualifier and action one same row. Otherwise add rows without separators.
             */
            if (print_new_row)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            /**
             * In case total size of qualifiers is lower than size of the current action, it means
             * that qualifiers are still related to the current action.
             */
            if (qual_total_size < action_size)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter, qual_size, 0, 0, key_values[key_index], fg_info_p,
                                 prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                if (print_new_row && action_left)
                {
                    /** Print the actions info. */
                    SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                    (unit, core_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p, to_p,
                                     fg_info_p, attach_full_info_p, prt_ctr));

                    print_new_row = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(4);
                }

                /** Increase the qualifier iterator. */
                qual_iter++;
            }
            /**
             * If the total size of the qualifiers get bigger than size of the current action, it means
             * that we need to divide the qualifier to two parts. Where the first part will be the MSB of
             * the current action and the second part will be the LSB of the next action.
             */
            else if (qual_total_size > action_size)
            {
                /** Calculate the bits, which will be shown as MSB for the first action. */
                qual_current_size = action_size - (qual_total_size - qual_size);

                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter, qual_current_size, 0, (qual_current_size - 1),
                                 key_values[key_index], fg_info_p, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Skip the action related cells. */
                PRT_CELL_SKIP(4);

                /** Add row with separator on which the next qualifier and action will be presented. */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                /** Calculate the residue of the qualifier, which will be shown as LSB of the next action. */
                qual_size_residue = qual_total_size - action_size;

                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter, qual_size_residue, qual_current_size, (qual_size - 1),
                                 key_values[key_index], fg_info_p, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Increase the action iterator to get to the next action. */
                action_iter++;

                /** Get action size for the next action. */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, fg_info_p->group_basic_info.field_stage,
                                                              fg_info_p->group_full_info.
                                                              actions_payload_info.actions_on_payload_info[0]
                                                              [action_iter].dnx_action, &action_size));

                /** In case we have valid bit, we should add 1 bit to the actions size. */
                if (!
                    (fg_info_p->group_full_info.actions_payload_info.
                     actions_on_payload_info[0][action_iter].dont_use_valid_bit))
                {
                    action_size += 1;
                }

                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p, to_p, fg_info_p,
                                 attach_full_info_p, prt_ctr));

                /** Increase the qualifier iterator. */
                qual_iter++;

                /** Set the total size of the qualifiers to be equal to the residue of the last qualifier. */
                qual_total_size = qual_size_residue;
            }
            /**
             * If total size of the qualifiers becomes equal to current action size, just
             * print the current qualifier and skip the cells related to the action.
             */
            else
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter, qual_size, 0, 0, key_values[key_index], fg_info_p,
                                 prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Skip the cells related to the action. */
                PRT_CELL_SKIP(4);

                /** Increase the action and qualifier iterators. */
                action_iter++;
                qual_iter++;
                /** Clear the total size of the qualifiers. */
                qual_total_size = 0;
                print_new_row = TRUE;
            }
        }
        /**
         * In case the qual_size is bigger than the actions_size, it means that we have more then
         * one action, which will take its values from the current qualifier.
         */
        else if (qual_size > action_size && action_left)
        {
            /** Calculate the total size of the actions. */
            action_total_size += action_size;

            /**
             * Add new row with underscore separator in case we have to print
             * qualifier and action one same row. Otherwise add rows without separators.
             */
            if (print_new_row)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            if (print_new_row && qual_left)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter,
                                 fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size, 0, 0,
                                 key_values[key_index], fg_info_p, prt_ctr));

                print_new_row = FALSE;
            }
            else
            {
                PRT_CELL_SKIP(4);
            }

            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            /**
             * In case total size of actions is lower than size of the current qualifier, it means
             * that actions are still getting its values from the current qualifier.
             */
            if (action_total_size < qual_size)
            {
                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p, to_p, fg_info_p,
                                 attach_full_info_p, prt_ctr));

                /** Increase the action iterator. */
                action_iter++;
            }
            /**
             * If the total size of the actions get bigger than size of the current qualifier, it means
             * that we need to divide the action to two parts. Where the first part will be the MSB of
             * the current qualifier and the second part will be the LSB of the next qualifier.
             */
            else if (action_total_size > qual_size)
            {
                /** Calculate the action bits, which will be shown as MSB for the first qualifier. */
                action_current_size = qual_size - (action_total_size - action_size);

                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, fg_id, action_iter, action_current_size, 0, (action_current_size - 1),
                                 block_p, from_p, to_p, fg_info_p, attach_full_info_p, prt_ctr));

                /** Increase the qualifier iteration to get to the next qualifier. */
                qual_iter++;

                /** Add row with separator on which the next qualifier and action will be presented. */
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter,
                                 fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size, 0, 0,
                                 key_values[key_index], fg_info_p, prt_ctr));

                /** Set an empty column to separate qualifiers and actions. */
                PRT_CELL_SET(" ");

                /** Calculate the residue of the action, which will be shown as LSB of the next qualifier. */
                action_size_residue = action_total_size - qual_size;

                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, fg_id, action_iter, action_size_residue, action_current_size,
                                 (action_size - 1), block_p, from_p, to_p, fg_info_p, attach_full_info_p, prt_ctr));

                /** Increase the action iterator. */
                action_iter++;
                /** Set the total size of the actions to be equal to the residue of the last action. */
                action_total_size = action_size_residue;
            }
            /**
             * If total size of the actions becomes equal to current qualifier size, just
             * print the current action.
             */
            else
            {
                /** Print the actions info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p, to_p, fg_info_p,
                                 attach_full_info_p, prt_ctr));

                /** Increase the action and qualifier iterators. */
                action_iter++;
                qual_iter++;
                /** Clear the total size of the actions. */
                action_total_size = 0;
                print_new_row = TRUE;
            }
        }
        /**
         * If there are no qualifiers and FES actions to be presented,
         * we are presenting the FEM actions, if any.
         */
        else if (!qual_left && !action_left && fem_left)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /**
             * Skip 4 cells (Qualifier, Lsb, Size, Pkt Val) about qualifier info if no more valid qualifiers were found,
             * but we still have valid actions to be printed.
             */
            PRT_CELL_SKIP(4);

            /** Set an empty column to separate qualifiers and actions. */
            PRT_CELL_SET(" ");

            /** Print the FEM actions info. */
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fem_actions_print
                            (unit, FALSE, core_id, fg_id, block_p, from_p, to_p,
                             key_values[key_index], NULL, &fem_id, prt_ctr));
            /**
             * Increase the returned fem_id_p, because it is used as starting index for the loop in
             * diag_dnx_field_last_fem_actions_print().
             */
            fem_id += 1;
            /**
             * Reduce the number of allocated FEMs, to use it as indication
             * if we still have FEMs to present.
             */
            nof_allocated_fems--;
        }
        else
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

            /**
             * If we still have any valid qualifiers (qual_left), then print information about them.
             * Otherwise skip the cells, which are relevant to the qualifiers,
             * as it is mentioned in the 'else' statement.
             */
            if (qual_left)
            {
                /** Print the qualifiers info. */
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_qual_print
                                (unit, context_id, qual_iter,
                                 fg_info_p->group_full_info.key_template.key_qual_map[qual_iter].size, 0, 0,
                                 key_values[key_index], fg_info_p, prt_ctr));

                qual_iter++;
            }
            else
            {
                /**
                 * Skip 4 cells (Qualifier, Lsb, Size, Pkt Val) about qualifier info if no more valid qualifiers were found,
                 * but we still have valid actions to be printed.
                 */
                PRT_CELL_SKIP(4);
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
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_action_print
                                (unit, core_id, fg_id, action_iter, action_size, 0, 0, block_p, from_p, to_p, fg_info_p,
                                 attach_full_info_p, prt_ctr));

                action_iter++;
            }
            else
            {
                /**
                 * Skip 4 cells (FES/FEM, Action, Valid/Cond bit, Performed val) about action info if no more valid actions were found,
                 * but we still have valid qualifiers to be printed.
                 */
                PRT_CELL_SKIP(4);
            }

            print_new_row = TRUE;
        }

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info for CONST field groups.
 *  Which were the performed actions.
 *  with the actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] fg_id - Field group ID.
 * \param [in] fg_info_p - Pointer, which contains all FG related information.
 * \param [in] attach_full_info_p - Pointer, which contains all needed Context Attach related information.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] do_display_any_fg_info_p - Indicates if there is any data to be
 *  presented in the table.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_const_fg_payload_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p,
    dnx_field_group_context_full_info_t * attach_full_info_p,
    char *block_p,
    char *from_p,
    char *to_p,
    uint8 *do_display_any_fg_info_p,
    sh_sand_control_t * sand_control)
{
    int max_num_actions;
    int action_iter;
    uint8 add_new_fg_id;
    uint8 do_display;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** The max values from the DNX data. */
    max_num_actions = dnx_data_field.group.nof_action_per_fg_get(unit);

    add_new_fg_id = TRUE;
    do_display = FALSE;

    /** Iterate over maximum number of actions per FG, to retrieve information about these, which are valid. */
    for (action_iter = 0; action_iter < max_num_actions; action_iter++)
    {
        /**
         * Check if the current action is invalid (DNX_FIELD_ACTION_INVALID), we should break
         * the loop and stop printing actions info.
         */
        if (fg_info_p->group_full_info.actions_payload_info.actions_on_payload_info[0][action_iter].dnx_action ==
            DNX_FIELD_ACTION_INVALID)
        {
            break;
        }

        /** In case the flag is set, we should set value for FG ID cell on this roll. */
        if (add_new_fg_id)
        {
            PRT_TITLE_SET("Performed Actions for FG ID %d (%s)", fg_id, "CONST");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FES Id");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Base Action (ID)");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Action Priority");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Signal Value");

            add_new_fg_id = FALSE;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);

        /** Print the FES actions info. */
        SHR_IF_ERR_EXIT(diag_dnx_field_last_const_fg_action_print
                        (unit, core_id, action_iter, block_p, from_p, to_p, fg_info_p, attach_full_info_p, prt_ctr));

        do_display = TRUE;
    }

    if (do_display)
    {
        PRT_COMMITX;
        *do_display_any_fg_info_p = TRUE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function displays info per field group.
 *  Shows all configured qualifiers and performed actions with the expected
 *  and actual values of them.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core Id on which the lookup was performed.
 * \param [in] context_id - ID of the selected context.
 * \param [in] fg_id_p - Array with field groups, which performed
 *  lookups and have a hit.
 * \param [in] num_fgs - Number of the field groups, which performed lookups.
 * \param [in] block_p - Name of PP block, one of IRPP, ERPP, ETPP.
 *  If NULL all blocks will be searched for match of the given signal.
 * \param [in] from_p - Name of stage signal comes from, If null any from
 *  stages will be searched for match.
 * \param [in] to_p - Name of stage signal goes to, if null any to stage
 *  will be searched for match.
 * \param [in] last_diag_fg_info_p - Contains Key IDs, their values and entries which were hit per FG.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_fg_key_payload_print(
    int unit,
    bcm_core_t core_id,
    dnx_field_context_t context_id,
    dnx_field_group_t * fg_id_p,
    int num_fgs,
    char *block_p,
    char *from_p,
    char *to_p,
    diag_dnx_field_last_fg_info_t * last_diag_fg_info_p,
    sh_sand_control_t * sand_control)
{
    dnx_field_entry_t entry_info;
    int fg_iter;
    dnx_field_group_full_info_t *fg_info = NULL;
    dnx_field_group_type_e fg_type;
    dnx_field_group_context_full_info_t *attach_full_info = NULL;
    uint8 do_display_any_fg_info;
    int none_fem_occupied;
    uint8 is_hit_found;
    dnx_field_fem_id_t nof_allocated_fems;
    char converted_fg_type[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(fg_info, sizeof(dnx_field_group_full_info_t), "fg_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(attach_full_info, sizeof(dnx_field_group_context_full_info_t), "attach_full_info", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);

    do_display_any_fg_info = FALSE;

    /** Iterate over field group array. */
    for (fg_iter = 0; fg_iter < num_fgs; fg_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id_p[fg_iter], &fg_type));

        /**
         * In case the entry ID of the current allocated FG is invalid, we don't have a hit,
         * otherwise we have a hit and valid entry ID.
         */
        if (last_diag_fg_info_p[fg_iter].entry_id != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {
            is_hit_found = TRUE;
        }
        else
        {
            is_hit_found = FALSE;
        }

        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_get(unit, fg_id_p[fg_iter], fg_info));
        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &(attach_full_info->attach_basic_info)));
        SHR_IF_ERR_EXIT(dnx_field_group_context_get(unit, fg_id_p[fg_iter], context_id, attach_full_info));

        /** Check if any FEM actions are used by the current FG. */
        SHR_IF_ERR_EXIT(dnx_field_fem_is_any_fem_occupied_on_fg(unit, fg_id_p[fg_iter], DNX_FIELD_GROUP_INVALID,
                                                                DNX_FIELD_IGNORE_ALL_ACTIONS, &none_fem_occupied));
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_nof_alloc_fems_per_fg_get(unit, fg_id_p[fg_iter], &nof_allocated_fems));

        /** Print information only for supported FG Types. */
        switch (fg_type)
        {
            case DNX_FIELD_GROUP_TYPE_TCAM:
            case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
            {
                if (is_hit_found)
                {
                    /** Get information about the relevant entry for current FG. */
                    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
                    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get
                                    (unit, fg_id_p[fg_iter], last_diag_fg_info_p[fg_iter].entry_id, &entry_info));
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_last_common_fg_key_payload_print
                                (unit, is_hit_found, none_fem_occupied, nof_allocated_fems, core_id, context_id,
                                 fg_id_p[fg_iter], fg_info, attach_full_info, &entry_info, block_p, from_p, to_p,
                                 last_diag_fg_info_p[fg_iter].key_values, &do_display_any_fg_info, sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_de_fg_key_payload_print
                                (unit, none_fem_occupied, nof_allocated_fems, core_id, context_id, fg_id_p[fg_iter],
                                 fg_info, attach_full_info, block_p, from_p, to_p,
                                 last_diag_fg_info_p[fg_iter].key_values, &do_display_any_fg_info, sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_CONST:
            {
                SHR_IF_ERR_EXIT(diag_dnx_field_last_const_fg_payload_print
                                (unit, core_id, fg_id_p[fg_iter], fg_info, attach_full_info, block_p, from_p, to_p,
                                 &do_display_any_fg_info, sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_EXEM:
            {
                dnx_field_entry_key_t entry_key;
                if (is_hit_found)
                {
                    /** Get information about the relevant entry for current FG. */
                    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_exem_entry_key_info_get
                                    (unit, fg_info, last_diag_fg_info_p[fg_iter].key_values[0], &entry_key));
                    SHR_IF_ERR_EXIT(dnx_field_entry_exem_get(unit, fg_id_p[fg_iter], &entry_key, &entry_info));
                }

                SHR_IF_ERR_EXIT(diag_dnx_field_last_common_fg_key_payload_print
                                (unit, is_hit_found, none_fem_occupied, nof_allocated_fems, core_id, context_id,
                                 fg_id_p[fg_iter], fg_info, attach_full_info, &entry_info, block_p, from_p, to_p,
                                 last_diag_fg_info_p[fg_iter].key_values, &do_display_any_fg_info, sand_control));
                break;
            }
            case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            case DNX_FIELD_GROUP_TYPE_KBP:
            default:
            {
                /** None of the supported types. */
                SHR_IF_ERR_EXIT(diag_dnx_field_utils_group_type_conversion
                                (unit, fg_info->group_basic_info.fg_type, converted_fg_type));

                LOG_CLI_EX("\r\n" "Field Group Type %s is not supported by FG Info diagnostic!! %s%s%s\r\n\n",
                           converted_fg_type, EMPTY, EMPTY, EMPTY);
                break;
            }
        }
    }

    if (!do_display_any_fg_info)
    {
        LOG_CLI_EX("\r\n" "No key info and performed actions were found, to be presented!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FREE(fg_info);
    SHR_FREE(attach_full_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about field configurations and performed lookups for
 *   a certain stage.
 * \param [in] unit - The unit number.
 * \param [in] field_stage - For which of the PMF stages the information will
 *  be retrieve and present (iPMF1/2/3, ePMF.....).
 * \param [in] core_id - Core Id on which the packet was sent.
 * \param [in] sand_control - Sand_control, which is implicitly provided
 *  by the shell command invocation parameters, is required because of PRT_COMMITX.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_last_info_print(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_core_t core_id,
    sh_sand_control_t * sand_control)
{
    bcm_field_stage_t bcm_stage;
    int num_fgs;

    dnx_field_context_t context_id;
    /** Array to store the FGs, which performed lookups. */
    dnx_field_group_t fg_performed_lookup[dnx_data_field.group.nof_fgs_get(unit)];
    diag_dnx_field_last_fg_info_t last_diag_fg_info[dnx_data_field.group.nof_fgs_get(unit)];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(fg_performed_lookup, DNX_FIELD_GROUP_INVALID, sizeof(fg_performed_lookup));

    /** Print information only for supported Field stages. */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_print
                            (unit, core_id, field_stage, "IRPP", "IPMF1", "FER", &context_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print
                            (unit, core_id, field_stage, context_id, "IRPP", "IPMF1", "TCAM", fg_performed_lookup,
                             &num_fgs, last_diag_fg_info, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_key_payload_print
                            (unit, core_id, context_id, fg_performed_lookup, num_fgs, "IRPP", "IPMF1", "FER",
                             last_diag_fg_info, sand_control));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_print
                            (unit, core_id, field_stage, "IRPP", "IPMF1", "FER", &context_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print
                            (unit, core_id, field_stage, context_id, "IRPP", "IPMF2", "TCAM", fg_performed_lookup,
                             &num_fgs, last_diag_fg_info, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_key_payload_print
                            (unit, core_id, context_id, fg_performed_lookup, num_fgs, "IRPP", "IPMF1", "FER",
                             last_diag_fg_info, sand_control));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_print
                            (unit, core_id, field_stage, "IRPP", "IPMF3", "LBP", &context_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print
                            (unit, core_id, field_stage, context_id, "IRPP", "IPMF3", "TCAM", fg_performed_lookup,
                             &num_fgs, last_diag_fg_info, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_key_payload_print
                            (unit, core_id, context_id, fg_performed_lookup, num_fgs, "IRPP", "IPMF3", "LBP",
                             last_diag_fg_info, sand_control));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(diag_dnx_field_last_cs_print
                            (unit, core_id, field_stage, "ERPP", "EPMF", "ETMR", &context_id, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_print
                            (unit, core_id, field_stage, context_id, "ERPP", "EPMF", "TCAM", fg_performed_lookup,
                             &num_fgs, last_diag_fg_info, sand_control));
            SHR_IF_ERR_EXIT(diag_dnx_field_last_fg_key_payload_print
                            (unit, core_id, context_id, fg_performed_lookup, num_fgs, "ERPP", "EPMF", "ETMR",
                             last_diag_fg_info, sand_control));
            break;
        }
        default:
        {
            /** Convert DNX to BCM Field Stage */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
            LOG_CLI_EX("\r\n" "Field Stage '%s' is not supported by Last Packet diagnostic!! %s%s%s\r\n\n",
                       dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays information about field configurations and performed lookups for
 *   a certain stage or range of stages, specified by the caller.
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
diag_dnx_field_last_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;

    bcm_core_t core_id, core_index;
    uint8 num_of_cores;
    uint8 is_traffic_found;
    bcm_instru_vis_mode_control_t visibility_mode;

    bsl_severity_t original_severity_fld_diag = BSL_INFO;
    bsl_severity_t original_severity_fld_proc = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    LOG_CLI_EX("\r\n" "Last Info diagnostic is not available for Cmodel!!!%s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
    SHR_EXIT();
#endif

    core_id = 0;

    /**
     * Save the original severity level of the fld diag severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDDIAGSDNX, original_severity_fld_diag);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDDIAGSDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);
    SH_SAND_GET_INT32("core", core_id);

    if (core_id != 0 && core_id != 1 && core_id != _SHR_CORE_ALL)
    {
        LOG_CLI_EX("\r\n" "Invalid CORE was given!!! CORE values can be '0', '1' or 'all'!%s%s%s%s\r\n\n", EMPTY, EMPTY,
                   EMPTY, EMPTY);
        SHR_EXIT();
    }

    is_traffic_found = FALSE;

    if (core_id != _SHR_CORE_ALL)
    {
        core_index = core_id;
        num_of_cores = core_id;
    }
    else
    {
        core_index = 0;
        num_of_cores = (dnx_data_device.general.nof_cores_get(unit) - 1);
    }

    /**
     * Iterate over max number of cores and check memory status of entire direction.
     * Used the core ID later to check the signals on it.
     */
    for (; core_index <= num_of_cores; core_index++)
    {
        if (dnx_debug_direction_is_ready(unit, core_index, BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS,
                                                                         DNX_VISIBILITY_BLOCK_UNDEFINED) == _SHR_E_NONE)
        {
            bsl_printf("\r\n******************************\r\n");
            bsl_printf("*      CORE ID %d            *\r\n", core_index);
            bsl_printf("******************************\r\n\n");
            /** Iterate over all stages and check if they are FP stages. */
            for (stage_index = stage_lower; stage_index <= stage_upper && (DNX_FIELD_IS_FP_STAGE(stage_index));
                 stage_index++)
            {
                bsl_printf("\r\n******************************\r\n");
                bsl_printf("*           %s            *\r\n",
                           dnx_field_stage_text(unit, Field_stage_enum_table[stage_index].value));
                bsl_printf("******************************\r\n");
                diag_dnx_field_last_info_print(unit, Field_stage_enum_table[stage_index].value, core_index,
                                               sand_control);
            }

            is_traffic_found = TRUE;
        }
    }

    SHR_IF_ERR_EXIT(dnx_visibility_mode_get(unit, &visibility_mode, FALSE));
    /** In visibility mode is Always, we should reset the TCAM hit bit indication. */
    if (visibility_mode == bcmInstruVisModeAlways)
    {
        /** Reset TCAM hit indication register in case of visibility resume command was invoked. */
        SHR_IF_ERR_EXIT(bcm_field_entry_hit_flush(unit, BCM_FIELD_ENTRY_HIT_FLUSH_ALL, 0));
    }

    if (!is_traffic_found)
    {
        LOG_CLI_EX("\r\n" "No Traffic was found !!!%s%s%s%s\r\n\n", EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

exit:
    /**
     * Set back the severity level of the fld diag and proc to the original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDDIAGSDNX, original_severity_fld_diag);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fld_proc);
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'last' shell commands (info)
 */
sh_sand_cmd_t Sh_dnx_field_last_cmds[] = {
    {"info", diag_dnx_field_last_cb, NULL, Field_last_options, &Field_last_man, NULL,
     NULL, SH_CMD_SKIP_EXEC}
    ,
    {NULL}
};

/*
 * }
 */

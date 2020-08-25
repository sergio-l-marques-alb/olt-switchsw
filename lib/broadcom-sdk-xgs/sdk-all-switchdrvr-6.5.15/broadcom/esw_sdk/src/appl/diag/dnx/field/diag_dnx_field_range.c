/** \file diag_dnx_field_range.c
 *
 * Diagnostics procedures, for DNX, for 'range' operations.
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
#include "diag_dnx_field_range.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/field/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include "diag_dnx_field_utils.h"
#include "include/bcm_int/dnx/field/field_range.h"
#include <src/bcm/dnx/field/map/field_map_local.h>

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
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Qualifier display (shell commands).
 */
static sh_sand_man_t Field_range_list_man = {
    .brief = "'type'- displays the basic info about a specific range type"
             "on a certain stage (filter by field context id and field stage)",
    .full = "'type' display for a range of field ranges. Field range is specified via 'range'.\r\n"
            "If no 'type' is specified then '0'-'nof_ranges_types-1' is assumed (0-255).\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[type=<<Lowest range type>-<Highest range type>>]"
            "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "type=L4_SRC stage=ipmf1",
};

static sh_sand_man_t Field_range_info_man = {
    .brief = "'id'- displays the basic info about a specific field range"
            "'type'- displays the basic info about a specific range type"
            "on a certain stage (filter by field context id and field stage)",
    .full = "'id' display for a range of field ranges. Field range is specified via 'range'.\r\n"
            "If no 'id' is specified then '0'-'nof_ranges-1' is assumed (0-255).\r\n"
            "'type' display for a range of field ranges. Field range is specified via 'range'.\r\n"
            "If no 'type' is specified then '0'-'nof_ranges_types-1' is assumed (0-255).\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[id=<value>]"
                "[type=<<Lowest range type>-<Highest range type>>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "id=0 type=L4_SRC stage=ipmf1",
};

static sh_sand_option_t Field_range_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_TYPE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range type to get its info",    "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST",       (void *)Field_range_type_enum_table},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "IPMF1-EPMF",    (void *)Field_stage_enum_table},
    {NULL}
};

static sh_sand_option_t Field_range_info_options[] = {
    {DIAG_DNX_FIELD_OPTION_ID,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range ids to get its info",       NULL,       NULL},
    {DIAG_DNX_FIELD_OPTION_TYPE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range type to get its info",    "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST",       (void *)Field_range_type_enum_table},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "IPMF1-EPMF",    (void *)Field_stage_enum_table},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 * This function gives the Maximum Range ID's per stage and per range_type.
 *
 *  \param [in] unit - The unit number.
 *  \param [in] range_type - The range_type.
 *  \param [in] field_stage - The field stage.
 *  \param [out] max_nof_range_id - The maximum number
 *               of range_ids per range_type and stage.
* \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_list_max_nof_range_id_get(
    int unit,
    dnx_field_range_type_e range_type,
    dnx_field_stage_e field_stage,
    int *max_nof_range_id)
{
    SHR_FUNC_INIT_VARS(unit);
   /**
    * Switch between all range types and get the max_nof_range_id.
    */
    switch (range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
        {
            *max_nof_range_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_l4_ops_ranges;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            *max_nof_range_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_out_lif_ranges;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            *max_nof_range_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_out_lif_ranges;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid range type");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays Field ranges per range ID or range of IDs  (optional), specified by the caller.
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
diag_dnx_field_range_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int do_display;
    int range_type_lower, range_type_upper;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    dnx_field_range_type_e range_type;
    dnx_field_range_info_t range_info;
    uint32 range_id;
    int max_nof_range_id;
    bcm_field_stage_t bcm_stage;
    dnx_field_range_map_t *dnx_range_map;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_TYPE, range_type_lower, range_type_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    do_display = FALSE;
    for (range_type = (DNX_FIELD_RANGE_TYPE_FIRST + range_type_lower); range_type <= range_type_upper; range_type++)
    {
        char range_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = "";
        dnx_range_map = (dnx_field_range_map_t *) & range_map[range_type][bcmFieldStageIngressPMF1];
        /**
         * Add a new Table for each range.
         */
        PRT_TITLE_SET("%s Range", dnx_range_map->name);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range IDs");

        /**
         * Iterate over all given stages.
         */
        for (stage_index = stage_lower; stage_index <= stage_upper && (DNX_FIELD_IS_FP_STAGE(stage_index));
             stage_index++)
        {
            /**
             * Convert DNX to BCM Field Stage
             */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, stage_index, &bcm_stage));
            dnx_range_map = (dnx_field_range_map_t *) & range_map[range_type][bcm_stage];
            if (dnx_range_map->table_id == DBAL_TABLE_EMPTY)
            {
                continue;
            }
            /**
             * Get the max_range_id to iterate over.
             */
            diag_dnx_field_range_list_max_nof_range_id_get(unit, range_type, stage_index, &max_nof_range_id);
            /**
             * Iterate over all range_ids
             */
            for (range_id = 0; range_id < max_nof_range_id; range_id++)
            {
                /**
                 * Init the range_info
                 */
                SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &range_info));
                range_info.range_type = range_type;
                /**
                 * Get the range_info
                 */
                SHR_IF_ERR_EXIT(dnx_field_range_get(unit, stage_index, &range_id, &range_info));
                /**
                 * Check if min or max_val is set and if it is print the range_id.
                 */
                if (range_info.min_val != 0 || range_info.max_val != 0)
                {
                    do_display = TRUE;
                    /**
                     * If the string is empty, add only one element into it.
                     */
                    if (sal_strncmp(range_id_buff, "", DIAG_DNX_FIELD_UTILS_STR_SIZE) == 0)
                    {
                        sal_snprintf(range_id_buff, sizeof(range_id_buff), "%d", range_id);
                    }
                    /**
                     * If there is already something in the string add the range_id right after it.
                     */
                    else
                    {
                        sal_snprintf(range_id_buff, sizeof(range_id_buff), "%s, %d", range_id_buff, range_id);
                    }
                }
            }
        }
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", range_id_buff);
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
            LOG_CLI_EX("\r\n" "NO ranges configured for range type: %s!! %s%s%s\r\n\n",
                       dnx_field_range_type_text(range_type), EMPTY, EMPTY, EMPTY);
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets fg_info for a given fg_id.
 * \param [in] unit       - The unit number.
 * \param [in] range_id   - The range ID.
 * \param [in] range_type - The range_type.
 * \param [in] fg_id      - The Field Group ID.
 * \param [in] prt_ctr    - Pointer to control structure
 *                          used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_info_fg_info_print(
    int unit,
    uint32 range_id,
    dnx_field_range_type_e range_type,
    dnx_field_group_t fg_id,
    prt_control_t * prt_ctr)
{
    int fg_qual_iter, entry_qual_iter;
    bcm_field_group_info_t fg_info;
    uint32 entry_iter, next_iter;
    uint32 tcam_handler_id;
    dbal_tables_e dbal_table_id;
    bcm_field_entry_info_t entry_info;
    int l4_ops_entry_result;
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get information about the current Field Group.
     */
    SHR_IF_ERR_EXIT(bcm_field_group_info_get(unit, fg_id, &fg_info));
    /**
     * Iterate over all quals in the FG and stop if the qual is being found.
     */
    for (fg_qual_iter = 0; fg_qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP &&
         fg_info.qual_types[fg_qual_iter] != bcmFieldQualifyCount; fg_qual_iter++)
    {
        if ((fg_info.qual_types[fg_qual_iter] == bcmFieldQualifyPacketLengthRangeCheck ||
             fg_info.qual_types[fg_qual_iter] == bcmFieldQualifyRangeCheck))
        {
            /**
             * Retrieve information about all entries, which are being add to the current FG, and used TCAM banks.
             */
            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));
            /**
             * TCAM handler provides an iterator to loop on all entries for the field group
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, tcam_handler_id, &entry_iter));

            while (entry_iter != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
            {
                /**
                 * Get the next entry.
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next(unit, core, tcam_handler_id, entry_iter, &next_iter));
                /**
                 * Get information about the FG_entry.
                 */
                bcm_field_entry_info_t_init(&entry_info);
                SHR_SET_CURRENT_ERR(bcm_field_entry_info_get(unit, fg_id, entry_iter, &entry_info));
                /**
                 * Iterate over all quals in the FG and stop if the qual is being found.
                 */
                for (entry_qual_iter = 0; entry_qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP &&
                     entry_info.entry_qual[entry_qual_iter].type != bcmFieldQualifyCount; entry_qual_iter++)
                {
                    /**
                     * Calculate the AND operation of entry_value and entry_mask to be used for L4 Ops.
                     */
                    l4_ops_entry_result = entry_info.entry_qual[entry_qual_iter].value[0]
                        & entry_info.entry_qual[entry_qual_iter].mask[0];
                    /**
                     * IF the qual type is bcmFieldQualifyPacketLengthRangeCheck.
                     * The range_id should be equal to the value of the entry_qual.
                     */
                    if (entry_info.entry_qual[entry_qual_iter].type == bcmFieldQualifyPacketLengthRangeCheck &&
                        entry_info.entry_qual[entry_qual_iter].value[0] == range_id)
                    {
                        /**
                         * Save the Packet Header Size qual FG ID in the string with space and comma after it.
                         */
                        if (sal_strncmp((char *) (fg_info.name), "", sizeof(fg_info.name)))
                        {
                            PRT_CELL_SET("%d/ \"%s\"", fg_id, fg_info.name);
                        }
                        else
                        {
                            PRT_CELL_SET("%d/ \"N/A\"", fg_id);
                        }
                    }
                    /**
                     * IF the qual type is bcmFieldQualifyRangeCheck.
                     * The l4_ops_entry_result shifter right 'range_id' times should be equal to 1,
                     * because it is bit_map.
                     */
                    else if (entry_info.entry_qual[entry_qual_iter].type == bcmFieldQualifyRangeCheck &&
                             ((l4_ops_entry_result >> range_id) == 1))
                    {
                        /**
                         * Save the L4 Ops qual FG ID in the string with space and comma after it.
                         */
                        if (sal_strncmp((char *) (fg_info.name), "", sizeof(fg_info.name)))
                        {
                            PRT_CELL_SET("%d/ \"%s\"", fg_id, fg_info.name);
                        }
                        else
                        {
                            PRT_CELL_SET("%d/ \"N/A\"", fg_id);
                        }
                    }
                }
                entry_iter = next_iter;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function sets fg_info for a given fg_id.
 * \param [in] unit       - The unit number.
 * \param [in] range_id   - The range ID.
 * \param [in] presel_id  - The Presel ID.
 * \param [in] bcm_stage  - The bcm_stage.
 * \param [in] prt_ctr    - Pointer to control structure
 *                          used by pretty printing,
 *  declared by PRT_INIT_VARS().
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_range_info_presel_id_print(
    int unit,
    uint32 range_id,
    uint32 presel_id,
    bcm_field_stage_t bcm_stage,
    prt_control_t * prt_ctr)
{
    int presel_qual_iter;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    char presel_id_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = "";
    SHR_FUNC_INIT_VARS(unit);

    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);

    entry_id.presel_id = presel_id;
    entry_id.stage = bcm_stage;
    /**
     * Get information about the current Presel.
     */
    SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, 0, &entry_id, &entry_data));
    /**
     * Iterate over all quals in the Presel and stop if the qual is being found.
     */
    for (presel_qual_iter = 0; presel_qual_iter < entry_data.nof_qualifiers; presel_qual_iter++)
    {
        if (entry_data.qual_data[presel_qual_iter].qual_type == bcmFieldQualifyVPortRangeCheck &&
            range_id == entry_data.qual_data[presel_qual_iter].qual_value)
        {
            /**
             * Print the Presel ID.
             */
            if (sal_strncmp(presel_id_buff, "", DIAG_DNX_FIELD_UTILS_STR_SIZE) == 0)
            {
                sal_snprintf(presel_id_buff, sizeof(presel_id_buff), "%d", presel_id);
            }
            /**
             * If there is already something in the string add the range_id right after it.
             */
            else
            {
                sal_snprintf(presel_id_buff, sizeof(presel_id_buff), "%s, %d", presel_id_buff, presel_id);
            }
            PRT_CELL_SET("%s", presel_id_buff);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays FG IDs in which the specified by the caller Field ranges
 *   per range ID or range of IDs  (optional) is used.
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
diag_dnx_field_range_info_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int do_display;
    int range_type_lower, range_type_upper;
    dnx_field_stage_e stage_lower, stage_upper, stage_index;
    dnx_field_range_type_e range_type;
    dnx_field_range_info_t range_info;
    bcm_field_stage_t bcm_stage;
    int fg_id_index, max_nof_fg, max_nof_cs_lines, presel_index;
    uint8 is_fg_allocated;
    uint32 range_id;
    int max_range_id;
    dnx_field_range_map_t *dnx_range_map;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(DIAG_DNX_FIELD_OPTION_ID, range_id);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_TYPE, range_type_lower, range_type_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    /**
     * Add a new Table for each range.
     */
    PRT_TITLE_SET("Range Info");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Field Stage");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range min");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range max");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FG ID/ FG Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Presel ID");

    do_display = FALSE;
    max_nof_cs_lines = dnx_data_field.common_max_val.nof_cs_lines_get(unit);
    max_nof_fg = dnx_data_field.group.nof_fgs_get(unit);
    /**
     * Iterate over all given range_types
     */
    for (range_type = (DNX_FIELD_RANGE_TYPE_FIRST + range_type_lower); range_type <= range_type_upper; range_type++)
    {
        do_display = TRUE;
        /**
         * Iterate over all given stages.
         */
        for (stage_index = stage_lower; stage_index <= stage_upper && (DNX_FIELD_IS_FP_STAGE(stage_index));
             stage_index++)
        {
            /**
             * Convert DNX to BCM Field Stage
             */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, stage_index, &bcm_stage));
            /**
             * Get the max_range_id to iterate over.
             */
            max_range_id = 0;
            diag_dnx_field_range_list_max_nof_range_id_get(unit, range_type, stage_index, &max_range_id);
            /**
             * Check if the given range ID isn't bigger than the max_range_id per range_type and stage.
             */
            if (range_id >= max_range_id)
            {
                continue;
            }
            else
            {
                /**
                 * If the stage is not supported for the given range_type continue.
                 */
                dnx_range_map = (dnx_field_range_map_t *) & range_map[range_type][bcm_stage];
                if (dnx_range_map->table_id == DBAL_TABLE_EMPTY)
                {
                    continue;
                }
                /**
                 * Init the range info structure.
                 */
                SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &range_info));
                range_info.range_type = range_type;
                /**
                 * Get the range_info
                 */
                SHR_IF_ERR_EXIT(dnx_field_range_get(unit, stage_index, &range_id, &range_info));

                /**
                 * Check if min or max_val is set and if it is print the range_id.
                 */
                if (range_info.min_val != 0 || range_info.max_val != 0)
                {
                    do_display = TRUE;
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    PRT_CELL_SET("%d", range_id);
                    PRT_CELL_SET("%s", dnx_field_range_type_text(range_type));
                    PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));
                    PRT_CELL_SET("%d", range_info.min_val);
                    PRT_CELL_SET("%d", range_info.max_val);
                    /**
                     * If the range_type is out_lif get the presel_id.
                     * If the range_type is L4_ops or Packet header size get the fg_info.
                     */
                    if (range_type != DNX_FIELD_RANGE_TYPE_OUT_LIF)
                    {
                        /**
                         * Iterate over maximum number of FGs.
                         */
                        for (fg_id_index = 0; fg_id_index < max_nof_fg; fg_id_index++)
                        {
                            /**
                             * Check if the FG is being allocated.
                             */
                            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
                            if (!is_fg_allocated)
                            {
                                /**
                                 * If FG_ ID is not allocated, move to next FG ID
                                 */
                                continue;
                            }
                            /**
                             * Print the FG info.
                             */
                            SHR_IF_ERR_EXIT(diag_dnx_field_range_info_fg_info_print
                                            (unit, range_id, range_type, fg_id_index, prt_ctr));
                        }
                        /**
                         * In this case no presel_id is presented.
                         */
                        PRT_CELL_SET("-");
                    }
                    else
                    {
                        /**
                         * In this case no fg_id is presented.
                         */
                        PRT_CELL_SET("-");
                        /**
                         * Iterate over all presel_ids
                         */
                        for (presel_index = 0; presel_index < max_nof_cs_lines; presel_index++)
                        {
                            /**
                             * Print the presel info.
                             */
                            SHR_IF_ERR_EXIT(diag_dnx_field_range_info_presel_id_print
                                            (unit, range_id, presel_index, bcm_stage, prt_ctr));
                        }
                    }
                }
            }
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
        LOG_CLI_EX("\r\n" "NO allocated field ranges were found on specified filters!! %s%s%s%s\r\n\n", EMPTY,
                   EMPTY, EMPTY, EMPTY);
    }
exit:
    PRT_FREE;
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
 *   List of shell options for 'range' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_range_cmds[] = {
    {"list", diag_dnx_field_range_list_cb, NULL, Field_range_list_options, &Field_range_list_man}
    ,
    {"info", diag_dnx_field_range_info_cb, NULL, Field_range_info_options, &Field_range_info_man}
    ,
    {NULL}
};

/*
 * }
 */

/** \file diag_dnx_field_range.c
 *
 * Diagnostics procedures, for DNX, for 'range' operations.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <appl/diag/dnx/diag_dnx_field.h>
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
 *  'Help' description for Range list (shell commands).
 */
static sh_sand_man_t Field_range_list_man = {
    .brief = "'list'- displays the basic info about a specific field range"
            "'type'- displays the basic info about a specific range type"
            "on a certain stage (filter by field context id and field stage)",
    .full = "'list' display for a range of field ranges. Field range is specified via 'type'.\r\n"
            "'type' display for a range of field ranges. Field range is specified via 'type'.\r\n"
            "If no 'type' is specified then '0'-'nof_ranges_types-1' is assumed (0-3).\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[type=<<Lowest range type>-<Highest range type>>]"
                "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "type=L4_SRC stage=ipmf1",
};

static sh_sand_option_t Field_range_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_TYPE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest range type to get its info",    "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST",       (void *)Field_range_type_enum_table, "RANGE_TYPE_LOWEST-RANGE_TYPE_HIGHEST"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_range_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
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
            *max_nof_range_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_l4_ops_ranges_legacy;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            *max_nof_range_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_out_lif_ranges;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            *max_nof_range_id = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_pkt_hdr_ranges;
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
 *   This function displays Field ranges per range_type or range of types  (optional), specified by the caller.
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
    uint8 do_display, range_type_display;
    int range_type_lower, range_type_upper;
    dnx_field_stage_e stage_lower, stage_upper, stage_index;
    dnx_field_range_type_e range_type;
    dnx_field_range_info_t range_info;
    bcm_field_stage_t bcm_stage;
    uint32 range_id;
    int max_range_id;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_TYPE, range_type_lower, range_type_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    range_type_display = FALSE;
    do_display = FALSE;
    /** Iterate over all given range_types. */
    for (range_type = (DNX_FIELD_RANGE_TYPE_FIRST + range_type_lower); range_type <= range_type_upper; range_type++)
    {
        if (range_type == DNX_FIELD_RANGE_TYPE_L4_SRC_PORT || range_type == DNX_FIELD_RANGE_TYPE_L4_DST_PORT)
        {
            dnx_field_range_type_e range_type_internal;

            range_type_display = FALSE;

            PRT_TITLE_SET("L4SRC and L4DST Ranges");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range ID");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Field Stage");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range min");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range max");

            /** Get the max_range_id to iterate over. */
            max_range_id = 0;
            diag_dnx_field_range_list_max_nof_range_id_get(unit, range_type, DNX_FIELD_STAGE_IPMF1, &max_range_id);
            /** Iterate over all range_ids. */
            for (range_id = 0; range_id < max_range_id; range_id++)
            {
                uint8 new_range_id_set = TRUE;
                /** Iterate over all given stages. */
                for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
                {
                    uint8 new_stage_id_set = TRUE;
                    dnx_field_stage_e field_stage;
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                                    (unit, Field_range_diag_stage_enum_table[stage_index].string, &field_stage));

                    for (range_type_internal = DNX_FIELD_RANGE_TYPE_FIRST;
                         range_type_internal <= DNX_FIELD_RANGE_TYPE_L4_DST_PORT; range_type_internal++)
                    {
                        /** Convert DNX to BCM Field Stage. */
                        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

                        /** If the stage is not supported for the given range_type continue. */
                        if (range_map_legacy[range_type_internal][field_stage].table_id == DBAL_TABLE_EMPTY)
                        {
                            continue;
                        }

                        /** Init the range info structure. */
                        SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &range_info));
                        range_info.range_type = range_type_internal;
                        /** Get the range_info. */
                        SHR_IF_ERR_EXIT(dnx_field_range_get(unit, field_stage, &range_id, &range_info));
                        /** Check if min or max_val is set and if it is print the range_id. */
                        if (range_info.min_val != DNX_FIELD_RANGE_VALUE_INVALID
                            || range_info.max_val != DNX_FIELD_RANGE_VALUE_INVALID)
                        {
                            /**
                             * In case the flag new_range_id_set is set to TRUE,
                             * we will print Range ID on the first line of the table.
                             * Otherwise we skip the Range ID cell.
                             */
                            if (new_range_id_set)
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                                PRT_CELL_SET("%d", range_id);
                                new_range_id_set = FALSE;
                            }
                            else
                            {
                                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                                PRT_CELL_SKIP(1);
                            }

                            /**
                             * In case the flag new_stage_id_set is set to TRUE,
                             * we will print Range Stage on current line of the table.
                             * Otherwise we skip the Range Stage cell.
                             */
                            if (new_stage_id_set)
                            {
                                PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));
                                new_stage_id_set = FALSE;
                            }
                            else
                            {
                                PRT_CELL_SKIP(1);
                            }

                            PRT_CELL_SET("%s", dnx_field_range_type_text(range_type_internal));
                            PRT_CELL_SET("%d", range_info.min_val);
                            PRT_CELL_SET("%d", range_info.max_val);

                            range_type_display = TRUE;
                        }
                    }
                }
            }

            if (range_type_display)
            {
                PRT_COMMITX;
                do_display = TRUE;
            }
            else
            {
                PRT_FREE;
            }

            /**
             * We already presented first two types SRC and DST PORTs,
             * then we will continue with the next one after increasing
             * the value of the range_type in the for() loop.
             */
            range_type = DNX_FIELD_RANGE_TYPE_L4_DST_PORT;
        }
        else
        {
            /**
             * Add a new Table for each range.
             */
            PRT_TITLE_SET("%s Range", range_map_legacy[range_type][DNX_FIELD_STAGE_IPMF1].name);
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range ID");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range Field Stage");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range min");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Range max");

            /** Get the max_range_id to iterate over. */
            max_range_id = 0;
            diag_dnx_field_range_list_max_nof_range_id_get(unit, range_type, DNX_FIELD_STAGE_IPMF1, &max_range_id);
            /** Iterate over all range_ids. */
            for (range_id = 0; range_id < max_range_id; range_id++)
            {
                uint8 new_range_id_set = TRUE;
                /** Iterate over all given stages. */
                for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
                {
                    uint8 new_stage_id_set = TRUE;
                    dnx_field_stage_e field_stage;
                    SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                                    (unit, Field_range_diag_stage_enum_table[stage_index].string, &field_stage));

                    /** Convert DNX to BCM Field Stage. */
                    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));

                    /** If the stage is not supported for the given range_type continue. */
                    if (range_map_legacy[range_type][field_stage].table_id == DBAL_TABLE_EMPTY)
                    {
                        continue;
                    }

                    /** Init the range info structure. */
                    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &range_info));
                    range_info.range_type = range_type;
                    /** Get the range_info. */
                    SHR_IF_ERR_EXIT(dnx_field_range_get(unit, field_stage, &range_id, &range_info));
                    /** Check if min or max_val is set and if it is print the range_id. */
                    if (range_info.min_val != DNX_FIELD_RANGE_VALUE_INVALID
                        || range_info.max_val != DNX_FIELD_RANGE_VALUE_INVALID)
                    {
                        /**
                         * In case the flag new_range_id_set is set to TRUE,
                         * we will print Range ID on the first line of the table.
                         * Otherwise we skip the Range ID cell.
                         */
                        if (new_range_id_set)
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                            PRT_CELL_SET("%d", range_id);
                            new_range_id_set = FALSE;
                        }
                        else
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SKIP(1);
                        }

                        /**
                         * In case the flag new_stage_id_set is set to TRUE,
                         * we will print Range Stage on current line of the table.
                         * Otherwise we skip the Range Stage cell.
                         */
                        if (new_stage_id_set)
                        {
                            PRT_CELL_SET("%s", dnx_field_bcm_stage_text(bcm_stage));
                            new_stage_id_set = FALSE;
                        }
                        else
                        {
                            PRT_CELL_SKIP(1);
                        }

                        PRT_CELL_SET("%d", range_info.min_val);
                        PRT_CELL_SET("%d", range_info.max_val);

                        range_type_display = TRUE;
                    }
                }
            }

            if (range_type_display)
            {
                PRT_COMMITX;
                do_display = TRUE;
            }
            else
            {
                PRT_FREE;
            }
        }
    }

    if (!do_display)
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
 *   List of shell options for 'range' shell commands (list)
 */
sh_sand_cmd_t Sh_dnx_field_range_cmds[] = {
    {"list", diag_dnx_field_range_list_cb, NULL, Field_range_list_options, &Field_range_list_man}
    ,
    {NULL}
};

/*
 * }
 */


/*
 * \file diag_dnx_switch.c
 *
 * Root file: Diagnostics procedures, for DNX, for 'switch' operations.
 * Under switch, we currently have:
 *   load_balancing
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCHDIAGSDNX
/*
 * Includes
 * {
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/diag.h>
#include "diag_dnx_switch.h"
#include "diag_dnx_switch_load_balancing.h"
#include <bcm_int/dnx/switch/switch_load_balancing.h>
/*
 * }
 */
/*
 * Globals
 * {
 */
sh_sand_man_t Sh_dnx_switch_man = {
    .brief = "Presenting 'switch' diagnostics for BCM level and for DNX level",
    .full = NULL
};
static sh_sand_man_t Sh_dnx_diag_switch_load_balancing_man = {
    .brief = "Presenting 'switch/load_balancing' diagnostics for BCM level and for DNX level",
    .full = NULL
};
/* *INDENT-OFF* */
sh_sand_cmd_t Sh_dnx_switch_cmds[] = {
    {"load_balancing",       NULL,     Sh_dnx_diag_switch_load_balancing_cmds,    NULL, &Sh_dnx_diag_switch_load_balancing_man},
    {NULL}
};

/* *INDENT-ON* */
/**
 * Enum for options display level
 */
sh_sand_enum_t Switch_load_balancing_enum_table_for_display_level[] = {
    {"HIGH", DISPLAY_LEVEL_HIGH, "Detailed level of display"},
    {"MEDIUM", DISPLAY_LEVEL_MEDIUM, "Concise level of display"},
    {NULL}
};

/*
 * }
 */
/*
 * Diagnostics procedures to be used by, say, diag_dnx_switch_* and ctest_dnx_switch_*
 * {
 */
/**
 * See header in diag_dnx_switch.h
 */
shr_error_e
diag_dnx_switch_lb_display_reserved_labels(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_mpls_reserved_t mpls_reserved;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'reserved_labels' table below.
         */
        PRT_TITLE_SET("Legend - For table of 'reserved labels', below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Label indication");
        PRT_CELL_SET("%s",
                     "Bit map of 16 bits. Each bit corresponds to a reserved label. If set "
                     "then this label is included in the search.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Force layer");
        PRT_CELL_SET("%s",
                     "Bit map of 16 bits. Each bit corresponds to a reserved label. If set then this label "
                     "is included in the search even if terminated.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Next label indication");
        PRT_CELL_SET("%s",
                     "Bit map of 16 bits. Each bit corresponds to a reserved label. If set then the label "
                     "following this label is included in the search even if terminated.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Next label valid");
        PRT_CELL_SET("%s", "Currently, this bitmaps should just be equal to 'Next label indication'");
        PRT_COMMITX;
    }

    PRT_TITLE_SET("Setup for handling of MPLS reserved labels - LOAD_BALANCING_RESERVED_LABELS");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Label indication");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Force layer");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Next label indication");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Next label valid");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_reserved_label_get(unit, &mpls_reserved));

    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_current_label_indication);
    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_current_label_force_layer);
    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_next_label_indication);
    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_next_label_valid);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_switch.h
 */
shr_error_e
diag_dnx_switch_lb_display_mpls_identification(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_mpls_identification_t mpls_identification;
    uint32 mpls_identifier, num_mpls_identifiers;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    num_mpls_identifiers = sizeof(mpls_identification.mpls_identification) * SAL_UINT8_NOF_BITS;
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'mpls_identification' table below.
         */
        PRT_TITLE_SET("Legend - For table of 'mpls identification', below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Identification bitmap");
        PRT_CELL_SET("%s",
                     "Bit map of 32 bits. Each bit corresponds to identification of MPLS protocol "
                     "within the 5-bits protocol identifiers list.");
        PRT_COMMITX;
    }

    PRT_TITLE_SET("Bitmap of MPLS protocol identifiers - MPLS_LAYER_IDENTIFICATION_BITMAP_CFG");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Identification bit");
    for (mpls_identifier = 0; mpls_identifier < num_mpls_identifiers; mpls_identifier++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%d", mpls_identifier);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_identification_get(unit, &mpls_identification));
    PRT_CELL_SET("%s", "Value");
    for (mpls_identifier = 0; mpls_identifier < num_mpls_identifiers; mpls_identifier++)
    {
        if (mpls_identification.mpls_identification & SAL_BIT(mpls_identifier))
        {
            PRT_CELL_SET("%d", 1);
        }
        else
        {
            PRT_CELL_SET("%d", 0);
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_switch.h
 */
shr_error_e
diag_dnx_switch_lb_display_clients_and_crc_functions(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    int index;
    dnx_switch_lb_tcam_key_t tcam_key;
    dnx_switch_lb_tcam_result_t tcam_result;
    dnx_switch_lb_valid_t tcam_valid;
    uint32 physical_client, max_physical_client;
    bcm_switch_control_t logical_client[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_LB_CLIENTS];
    char *logical_client_name[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_LB_CLIENTS];
    char *logical_client_description;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    {
        /*
         * Prepare arrays for display.
         */
        max_physical_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
        for (physical_client = 0; physical_client < max_physical_client; physical_client++)
        {
            char *client_name;
            SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical
                            (unit, physical_client, &(logical_client[physical_client])));
            SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, logical_client[physical_client], &client_name));
            logical_client_name[physical_client] = client_name;
        }
    }
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'Load balancing clients and corresponding crc functions' table below.
         */
        PRT_TITLE_SET("Legend - For table of clients and corresponding crc functions, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        for (physical_client = 0; physical_client < max_physical_client; physical_client++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            if (logical_client_name[physical_client] == (char *) 0)
            {
                /*
                 * This should never happen: All physical clients should be translatable to logical.
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "physical_client (%d) could not be translated to a logical string. Probably internal error.\r\n",
                             physical_client);
            }
            PRT_CELL_SET("%s", logical_client_name[physical_client]);
            switch (logical_client[physical_client])
            {
                case bcmSwitchECMPHashConfig:
                {
                    logical_client_description = "CRC function for first ECMP hierarchy";
                    break;
                }
                case bcmSwitchECMPSecondHierHashConfig:
                {
                    logical_client_description = "CRC function for second ECMP hierarchy";
                    break;
                }
                case bcmSwitchECMPThirdHierHashConfig:
                {
                    logical_client_description = "CRC function for Third ECMP hierarchy";
                    break;
                }
                case bcmSwitchTrunkHashConfig:
                {
                    logical_client_description = "CRC function for LAG";
                    break;
                }
                case bcmSwitchNwkHashConfig:
                {
                    logical_client_description = "CRC function for network load balancing";
                    break;
                }
                default:
                {
                    /*
                     * This should never happen: All logical clients should be handled above.
                     */
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "logical_client (%d) is is not one that is supported for load balancing. Probably internal error.\r\n",
                                 logical_client[physical_client]);
                    break;
                }
            }
            PRT_CELL_SET("%s", logical_client_description);
        }
        PRT_COMMITX;
    }
    PRT_TITLE_SET("Load balancing clients and corresponding crc functions.");
    PRT_INFO_ADD("%s", "  plus selection of LS or MS 16 bits out of 32 assigned per layer.");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_INFO_ADD("%s", "Handling Tables LOAD_BALANCING_CRC_SELECTION_HL_TCAM and");
    PRT_INFO_ADD("%s", "LOAD_BALANCING_CLIET_FWD_BIT_SEL.");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_INFO_ADD("%s", "The last row displays an 8-bits bitmap. It indicates whether specified");
    PRT_INFO_ADD("%s", "client would use the LS 16 bits (0) or the MS 16 bits of each of the");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_INFO_ADD("%s", "8 record layers. There are 32 bits per record layer ");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical client name");
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", logical_client_name[physical_client]);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Physical client id");
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        PRT_CELL_SET("%d", physical_client);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Logical client id");
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        PRT_CELL_SET("%d", logical_client[physical_client]);
    }
    index = 0;
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_get(unit, index, &tcam_key, &tcam_result, &tcam_valid));
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "crc function id");
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        PRT_CELL_SET("%d", tcam_result.client_x_crc_16_sel[physical_client]);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Select LS/MS 16-bits");
    {
        dnx_switch_lb_lsms_crc_select_t lsms_crc_select;

        for (physical_client = 0; physical_client < max_physical_client; physical_client++)
        {
            lsms_crc_select.lb_client = (uint8) physical_client;
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_lsms_crc_select_get(unit, &lsms_crc_select));
            PRT_CELL_SET("0x%04X", lsms_crc_select.lb_selection_bit_map);
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_switch.h
 */
shr_error_e
diag_dnx_switch_lb_display_crc_seed_per_function(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    uint16 crc_index, max_crc_index;
    dnx_switch_lb_crc_seed_t crc_seed[dnx_data_switch.load_balancing.nof_crc_functions_get(unit)];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    max_crc_index = dnx_data_switch.load_balancing.nof_crc_functions_get(unit);
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'crc_seeds' table below.
         */
        PRT_TITLE_SET("Legend - For table of crc_seed_per_function, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Identifier of crc function");
        PRT_CELL_SET("%s", "Seed for crc calculation. variable of 16 bits");
        PRT_COMMITX;
    }
    for (crc_index = 0; crc_index < max_crc_index; crc_index++)
    {
        crc_seed[crc_index].lb_crc_function_index = crc_index;
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_seed_get(unit, &crc_seed[crc_index]));
    }
    PRT_TITLE_SET("Load balancing crc functions and corresponding seeds - LOAD_BALANCING_CRC_SEEDS");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "crc function id");
    for (crc_index = 0; crc_index < max_crc_index; crc_index++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%d", crc_seed[crc_index].lb_crc_function_index);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "16-bits seed");
    for (crc_index = 0; crc_index < max_crc_index; crc_index++)
    {
        PRT_CELL_SET("0x%04X", crc_seed[crc_index].lb_crc_seed);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/**
 * See header in diag_dnx_switch.h
 */
shr_error_e
diag_dnx_switch_lb_display_crc_general_seeds(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_general_seeds_t general_seeds;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'general_seeds' table below.
         */
        PRT_TITLE_SET("Legend - For table of general crc seeds for MPLS label collection and for parser, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "MPLS_STACK_0");
        PRT_CELL_SET("%s", "Seed for the CRC32 function on label collection and compression for first MPLS stack");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "MPLS_STACK_1");
        PRT_CELL_SET("%s", "Seed for the CRC32 function on label collection and compression for second MPLS stack");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "PARSER");
        PRT_CELL_SET("%s", "Seed for the CRC32 function on data collection and compression for parser");
        PRT_COMMITX;
    }
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &general_seeds));
    PRT_TITLE_SET("Load balancing general crc seeds - LOAD_BALANCING_GENERAL_SEEDS");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MPLS_STACK_0");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MPLS_STACK_1");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "PARSER");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "32-bits seed");
    PRT_CELL_SET("0x%08X", general_seeds.mpls_stack_0);
    PRT_CELL_SET("0x%08X", general_seeds.mpls_stack_1);
    PRT_CELL_SET("0x%08X", general_seeds.parser);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * }
 */

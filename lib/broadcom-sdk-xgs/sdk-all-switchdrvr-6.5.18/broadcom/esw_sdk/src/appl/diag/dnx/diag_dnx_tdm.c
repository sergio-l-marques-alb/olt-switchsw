/** \file diag_dnx_tdm.c
 * 
 * main file for tdm diagnostics
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/** bcm/bcm_int */
#include <bcm/tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/init/init.h>

/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
/*
 * }
 */

shr_error_e
sh_dnx_is_tdm_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * if init is not done, return that tdm is available.
     * this is done to prevent access to DNX-Data before it is init
     */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }

    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_tdm_global_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int tdm_mode, system_headers_mode, out_header_type;
    int min_packet_size, max_packet_size, mc_prefix;
    char *mode_str = "N/A", *ftmh_header = "N/A";

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Device TDM Configuration");

    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Value");

    tdm_mode = dnx_data_tdm.params.mode_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (tdm_mode)
    {
        case TDM_MODE_NONE:
            mode_str = "NONE";
            break;
        case TDM_MODE_OPTIMIZED:
            mode_str = "BYPASS";
            ftmh_header = "Optimized";
            break;
        case TDM_MODE_STANDARD:
            mode_str = "BYPASS";
            ftmh_header = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ?
                "Standard" : "Standard J1";
            break;
        case TDM_MODE_PACKET:
            mode_str = "PACKET";
            ftmh_header = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ?
                "Standard" : "Standard J1";
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported TDM mode:%d\n", tdm_mode);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Mode");
    PRT_CELL_SET("%s", mode_str);

    if (tdm_mode != TDM_MODE_NONE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("FTMH Type");
        PRT_CELL_SET("%s", ftmh_header);
        /*
         * Out header type is verified in bcm_tdm_control_get
         */
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmOutHeaderType, &out_header_type));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Out Header Type");
        PRT_CELL_SET("%s", out_header_type == BCM_TDM_CONTROL_OUT_HEADER_OTMH ? "OTMH" : "FTMH");

        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmBypassMinPacketSize, &min_packet_size));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Min Packet Size");
        PRT_CELL_SET("%d", min_packet_size);

        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmBypassMaxPacketSize, &max_packet_size));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Max Packet Size");
        PRT_CELL_SET("%d", max_packet_size);

        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmStreamMulticastPrefix, &mc_prefix));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MC Prefix");
        PRT_CELL_SET("0x%x", mc_prefix);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sh_dnx_tdm_global_man = {
    .brief = "Present device TDM Configuration",
};

static shr_error_e
sh_dnx_tdm_context_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     * int id_start, id_end; 
     */

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * SH_SAND_GET_INT32_RANGE("id", id_start, id_end); 
     */

    PRT_TITLE_SET("TDM Ingress Contexts");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Drop Status");

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sh_dnx_tdm_context_man = {
    .brief = "Present context content",
    .examples = "id=1"
};

/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t sh_dnx_tdm_context_arguments[] = {
    /*
     * Name | Type | Description | Default 
     */
    {"id", SAL_FIELD_TYPE_INT32, "Context ID", "-1"},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief DNX TDM diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for TDM diagnostic commands
 */
sh_sand_cmd_t sh_dnx_tdm_cmds[] = {
    /*
     * keyword, action, Array, arguments, man 
     */
    {"global", sh_dnx_tdm_global_cmd, NULL, NULL, &sh_dnx_tdm_global_man},
    {"context", sh_dnx_tdm_context_cmd, NULL, sh_dnx_tdm_context_arguments, &sh_dnx_tdm_context_man},
    {NULL}
};

sh_sand_man_t sh_dnx_tdm_man = {
    .brief = "TDM commands"
};

/* *INDENT-ON* */

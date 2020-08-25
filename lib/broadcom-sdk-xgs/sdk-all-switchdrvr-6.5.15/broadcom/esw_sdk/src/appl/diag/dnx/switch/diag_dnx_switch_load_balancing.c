/**
 * \file diag_dnx_switch_load_balancing.c
 *
 * Diagnostics procedures, for DNX, for 'switch/loiad_balancing' operations.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCHDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_switch.h"
#include "diag_dnx_switch_load_balancing.h"
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
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for action display (shell commands).
 */
static
sh_sand_man_t Switch_load_balancing_display_man = {
    .brief = "Display load balancing info in various levels of details (user selected)",
    .full = "Display load balancing info as specified by the user\r\n"
            "'level' indicates the level of detail.\r\n"
            "If 'level' is 'high' then display the highest detail level\r\n"
            "If 'level' is 'medium' then display all details but no legends\r\n"
            "If 'level' is not specified, display at the highest detail level\r\n",
    .synopsis = "[level=<detail level>]",
    .examples = "level=high",
};
static
sh_sand_option_t Switch_load_balancing_display_options[] = {
    {DNX_DIAG_SWITCH_LB_OPTION_DISPLAY_LEVEL,  SAL_FIELD_TYPE_UINT32, "Lowest-highest user defined action ids to get its info",       "HIGH",       (void *)Switch_load_balancing_enum_table_for_display_level},
    {NULL}
};
/*
 * }
 */
/* *INDENT-ON* */
/**
 * \brief
 *   This function displays load-balancing info as per detail level, specified by the caller.
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
diag_dnx_switch_load_balancing_display_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 detail_level;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DNX_DIAG_SWITCH_LB_OPTION_DISPLAY_LEVEL, detail_level);
    SHR_IF_ERR_EXIT(diag_dnx_switch_lb_display_reserved_labels(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_switch_lb_display_mpls_identification(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_switch_lb_display_clients_and_crc_functions(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_switch_lb_display_crc_seed_per_function(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_switch_lb_display_crc_general_seeds(unit, detail_level, sand_control));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-ON* */

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'load_balancing' shell commands (display, ...)
 */
sh_sand_cmd_t Sh_dnx_diag_switch_load_balancing_cmds[] = {
    {"display", diag_dnx_switch_load_balancing_display_cb, NULL, Switch_load_balancing_display_options,
     &Switch_load_balancing_display_man}
    ,
    {NULL}
};
/*
 * }
 */

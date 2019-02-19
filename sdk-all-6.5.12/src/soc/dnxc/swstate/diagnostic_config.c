/** \file diagnostic_config.c
 *
 * sw state diagnostics config definitions
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/sw_state_defs.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <soc/dnxc/swstate/diagnostic_config.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

sh_sand_cmd_t dnxc_swstate_config_cmds[] = {
    {"dump", sh_dnxc_swstate_config_dump_cmd, NULL, dnxc_swstate_config_dump_options, &dnxc_swstate_config_dump_man},
    {NULL}
};

sh_sand_man_t dnxc_swstate_config_man = {
    cmd_dnxc_swstate_config_desc,
    NULL,
    NULL,
    NULL,
};

const char cmd_dnxc_swstate_config_desc[] = "swstate config commands";

shr_error_e sh_dnxc_swstate_config_dump_cmd(int unit, args_t *args, sh_sand_control_t *sand_control)
{
    char *directory;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("directory", directory);

    dnx_sw_state_dump_directory_set(unit, directory);
    
exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnxc_swstate_config_dump_options[] = {
    {"directory", SAL_FIELD_TYPE_STR, "full directory path", NULL},
    {NULL}
};

sh_sand_man_t dnxc_swstate_config_dump_man = {
    "swstate config dump",
    "swstate config dump properties",
    "swstate config dump [directory=<string>]",
    "swstate config dump directory=/projects/NTSW_SW_USRS_6/arpaxadt/git/master/tmp/sw_state_dump_output",
};

#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef _ERR_MSG_MODULE_NAME

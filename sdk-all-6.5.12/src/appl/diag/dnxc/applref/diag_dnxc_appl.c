/** \file diag_dnxc_appl.c
 * 
 * DNX APPL DIAG PACK - diagnostic pack for application reference
 * 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
/** bcm */
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnxc_appl_run.h"
#include "diag_dnxc_appl_list.h"
/*
 * }
 */

 /*
  * LOCAL DIAG PACK:
  * {
  */

extern sh_sand_man_t    sh_dnxc_appl_run_man;
extern sh_sand_option_t sh_dnxc_appl_run_options[];

extern sh_sand_man_t    sh_dnxc_appl_list_man;
extern sh_sand_option_t sh_dnxc_appl_list_options[];

/**
 * DNX PORT diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */

sh_sand_cmd_t sh_dnxc_appl_cmds[] = {
    /*keyword,   action,                        command,                        options,                        man */               
    {"run",      sh_dnxc_appl_run_cmd,          NULL,                           sh_dnxc_appl_run_options,       &sh_dnxc_appl_run_man},
    {"list",     sh_dnxc_appl_list_cmd,         NULL,                           sh_dnxc_appl_list_options,      &sh_dnxc_appl_list_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_appl_man = {
    "Reference application diagnostic pack",
    NULL,
    NULL,
    NULL,
};

/*
 * }
 */

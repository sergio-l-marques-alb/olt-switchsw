/** \file diag_dnx_time_analyzer.c
 * 
 * diagnostic pack for time analyzer
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COUNTER

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*appl*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/*shared*/
#include <shared/utilex/utilex_time_analyzer.h>

/*sal*/
#include <sal/appl/sal.h>

/*
 * }
 */

/*
 * dump
 */

/* *INDENT-OFF* */
sh_sand_man_t dnx_time_analyzer_dump_man = {
    .brief =    "Dump time analyzer table"
};
/* *INDENT-ON* */

/**
 * \brief - display time analyzer statistics
 */
shr_error_e
cmd_dnx_time_analyzer_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_modules;
    utilex_time_analyzer_diag_info_t diag_info[UTILEX_TIME_ANALYZER_MAX_NOF_MODULES];
    int module_index;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print time analyzer statistics table
     */
    PRT_TITLE_SET("Time Analyzer");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Total Time");
    PRT_COLUMN_ADD("Occurrences");
    PRT_COLUMN_ADD("Avg Time");

    SHR_IF_ERR_EXIT(utilex_time_analyzer_diag_info_get
                    (unit, UTILEX_TIME_ANALYZER_MAX_NOF_MODULES, diag_info, &nof_modules));

    for (module_index = 0; module_index < nof_modules; module_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", diag_info[module_index].name);
        /** if bigger than 2 seconds - print in seconds units  - otherwise milliseconds*/
        if (diag_info[module_index].time > /* 2 seconds */ 2 * 1000 * 1000)
        {
            PRT_CELL_SET("%u [seconds]", diag_info[module_index].time / (1000 * 1000));
        }
        else
        {
            PRT_CELL_SET("%u [milliseconds]", diag_info[module_index].time / (1000));
        }
        PRT_CELL_SET("%d", diag_info[module_index].occurences);
        if (diag_info[module_index].occurences == 0)
        {
            PRT_CELL_SET("0 [microseconds]");
        }
        else
        {
            PRT_CELL_SET("%d [microseconds]", diag_info[module_index].time / diag_info[module_index].occurences);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Clear
 */
/* *INDENT-OFF* */
sh_sand_man_t dnx_time_analyzer_clear_man = {
    .brief    = "Diagnostic for time analyzer clear",
    .full     = "Clear Time analyzer table and restart the counting"
};
/* *INDENT-ON* */

/**
 * \brief - time analyzer clear command
 */
shr_error_e
cmd_dnx_time_analyzer_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_time_analyzer_clear(unit));

exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */
sh_sand_man_t sh_dnx_time_analyzer_man = {
    .brief =    "Various Time Analyzer controls",
    .full =     "Used to control Time Analyzer - special tool to analyze operation times"
};

sh_sand_cmd_t sh_dnx_time_analyzer_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"dump",     cmd_dnx_time_analyzer_dump,     NULL,   NULL,     &dnx_time_analyzer_dump_man},
    {"clear",    cmd_dnx_time_analyzer_clear,    NULL,   NULL,     &dnx_time_analyzer_clear_man},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */

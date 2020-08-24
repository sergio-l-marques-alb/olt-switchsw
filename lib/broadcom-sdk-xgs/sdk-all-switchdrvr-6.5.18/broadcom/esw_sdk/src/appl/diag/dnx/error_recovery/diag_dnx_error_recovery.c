/** \file diag_dnx_error_recovery.c
 *
 * Main diagnostics for Error Recovery applications All CLI commands
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif


#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include "diag_dnx_error_recovery.h"

/*************
 * TYPEDEFS  *
 *************/

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
/**
 * \brief - starts the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - commit the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_commit(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - rollback the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_rollback(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - suppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_suppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DNX_ERR_RECOVERY_SUPPRESS(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - unsuppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_unsuppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DNX_ERR_RECOVERY_UNSUPPRESS(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_transaction_swstate_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_print_journal(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_transaction_dbal_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_print_journal(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_transaction_swstate_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_transaction_dbal_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_rollback_journal_logger_state_change(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_compare_swstate_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_print_journal(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - print the journal
 */
static shr_error_e
sh_dnx_err_rec_compare_dbal_print(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_print_journal(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_compare_swstate_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_journal_logger_state_change(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - journal turn logger on
 */
static shr_error_e
sh_dnx_err_rec_compare_dbal_logger(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_journal_logger_state_change(unit, DNXC_ROLLBACK_JOURNAL_SUBTYPE_DBAL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - starts the comparison journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - end the comparison journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_end(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - suppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_suppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_tmp_suppress(unit, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - unsuppress the rollback journal
 */
static shr_error_e
sh_dnx_err_rec_comparison_jrnl_unsuppress(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_comparison_journal_tmp_suppress(unit, FALSE));

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/**
 * \brief - turn on rollback api testing
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_test_mode_on(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regress_testing_api_test_mode_change(unit, TRUE);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - turn off rollback api testing
 */
static shr_error_e
sh_dnx_err_rec_rollback_jrnl_test_mode_off(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    dnx_er_regress_testing_api_test_mode_change(unit, FALSE);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */

sh_sand_man_t sh_dnx_err_rec_transaction_jrnl_man = {
    .brief = "Rollback journal menu",
    .full = NULL
};

sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_man = {
    .brief = "Comparison journal menu",
    .full = NULL
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_start_man = {
    .brief = "Start a new rollback journal transaction",
    .full = "Start a new rollback journal transaction. Nesting transactions is permitted."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_commit_man = {
    .brief = "Commit the current rollback journal transaction",
    .full = "Commit the current rollback journal transaction. No rollback is being done."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_rollback_man = {
    .brief = "Rollback the current rollback journal transaction",
    .full = "Rollback the current rollback journal transaction. All changes during the transaction will be rolled back."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_swstate_man = {
    .brief = "SW STATE rollback journal commands",
    .full = "SW STATE rollback journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_dbal_man = {
    .brief = "DBAL rollback journal commands",
    .full = "DBAL rollback journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_suppress_man = {
    .brief = "Suppresses the rollback journal",
    .full =
        "Temporary suppresses the current rollback journal transaction. No operations are journaled during this period."
};

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_unsuppress_man = {
    .brief = "Unsuppresses the rollback journal",
    .full =
        "Unsuppresses the temporary current rollback journal transaction. Journaling is resumed for the current transaction."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_start_man = {
    .brief = "Start a new comparison journal transaction",
    .full = "Start a new comparison journal transaction. Nesting transactions is permitted."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_end_man = {
    .brief = "End and compare the current comparison journal transaction",
    .full = "End and compare the current comparison journal transaction. Differences are logged."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_swstate_man = {
    .brief = "SW STATE comparison journal commands",
    .full = "SW STATE comparison journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_dbal_man = {
    .brief = "DBAL comparison journal commands",
    .full = "DBAL comparison journal commands"
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_suppress_man = {
    .brief = "Suppresses the comparison journal",
    .full =
        "Temporary suppresses the current comparison journal transaction. No operations are journaled during this period."
};

static sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_unsuppress_man = {
    .brief = "Unsuppresses the comparison journal",
    .full =
        "Unsuppresses the temporary current comparison journal transaction. Journaling is resumed for the current transaction."
};

static sh_sand_man_t sh_dnx_err_rec_jrnl_print_man = {
    .brief = "Print the contents of the current journal transaction",
    .full = "Print the contents of the current journal transaction."
};

static sh_sand_man_t sh_dnx_err_rec_jrnl_logger_man = {
    .brief = "Log contents of the current journal transaction as they are being inserted",
    .full = "Log contents of the current journal transaction as they are being inserted"
};

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING

static sh_sand_man_t sh_dnx_err_rec_rollback_jrnl_rollback_test_man = {
    .brief = "Rollback toggle current test mode",
    .full = "Allow or disallow rollback journal regression testing for each API opting-in for error recovery"
};

/**
 * \brief DNX Rollback journal test command pack
 */
sh_sand_cmd_t sh_dnx_err_rec_transaction_test_cmds[] = {
    {"ON", sh_dnx_err_rec_rollback_jrnl_test_mode_on, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_test_man},
    {"OFF", sh_dnx_err_rec_rollback_jrnl_test_mode_off, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_test_man},
    {NULL}
};

#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
/**
 * \brief DNX Rollback journal command pack
 */

sh_sand_cmd_t sh_dnx_err_rec_transaction_swstate[] = {
    {"PRinT", sh_dnx_err_rec_transaction_swstate_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_transaction_swstate_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_transaction_dbal[] = {
    {"PRinT", sh_dnx_err_rec_transaction_dbal_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_transaction_dbal_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_compare_swstate[] = {
    {"PRinT", sh_dnx_err_rec_compare_swstate_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_compare_swstate_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_compare_dbal[] = {
    {"PRinT", sh_dnx_err_rec_compare_dbal_print, NULL, NULL, &sh_dnx_err_rec_jrnl_print_man},
    {"LoGger", sh_dnx_err_rec_compare_dbal_logger, NULL, NULL, &sh_dnx_err_rec_jrnl_logger_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_cmd_t sh_dnx_err_rec_transaction_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
   **********************************************************************************************************/
    {"START", sh_dnx_err_rec_rollback_jrnl_start, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_start_man},
    {"SuSPenD", sh_dnx_err_rec_rollback_jrnl_suppress, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_suppress_man},
    {"ReSuMe", sh_dnx_err_rec_rollback_jrnl_unsuppress, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_unsuppress_man},
    {"CoMmit", sh_dnx_err_rec_rollback_jrnl_commit, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_commit_man, NULL, NULL,
     SH_CMD_SKIP_EXEC},
    {"RoLLBaCK", sh_dnx_err_rec_rollback_jrnl_rollback, NULL, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_man},
    {"SWSTate", NULL, sh_dnx_err_rec_transaction_swstate, NULL, &sh_dnx_err_rec_rollback_jrnl_swstate_man},
    {"DBaL", NULL, sh_dnx_err_rec_transaction_dbal, NULL, &sh_dnx_err_rec_rollback_jrnl_dbal_man},
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    {"Test_MoDe", NULL, sh_dnx_err_rec_transaction_test_cmds, NULL, &sh_dnx_err_rec_rollback_jrnl_rollback_test_man},
#endif /* DNX_ERR_RECOVERY_REGRESSION_TESTING */
    {NULL}
};

/**
 * \brief DNX Comparison journal command pack
 */
sh_sand_cmd_t sh_dnx_err_rec_comparison_cmds[] = {
  /**********************************************************************************************************
   * CMD_NAME *     CMD_ACTION              * Next *        Options                   *       MAN           *
   *          *                             * Level*                                  *                     *
   *          *                             * CMD  *                                  *                     *
   **********************************************************************************************************/
    {"START", sh_dnx_err_rec_comparison_jrnl_start, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_start_man},
    {"SuSPenD", sh_dnx_err_rec_comparison_jrnl_suppress, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_suppress_man},
    {"ReSuMe", sh_dnx_err_rec_comparison_jrnl_unsuppress, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_unsuppress_man},
    {"END", sh_dnx_err_rec_comparison_jrnl_end, NULL, NULL, &sh_dnx_err_rec_comparison_jrnl_end_man},
    {"SWSTate", NULL, sh_dnx_err_rec_compare_swstate, NULL, &sh_dnx_err_rec_comparison_jrnl_swstate_man},
    {"DBaL", NULL, sh_dnx_err_rec_compare_dbal, NULL, &sh_dnx_err_rec_comparison_jrnl_dbal_man},
    {NULL}
};
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

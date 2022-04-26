
/*! \file diag_dnx_error_recovery.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_ERROR_RECOVERY_H_INCLUDED
#define DIAG_DNX_ERROR_RECOVERY_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/recovery/rollback_journal_utils.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

extern sh_sand_man_t sh_dnx_err_rec_transaction_jrnl_man;
extern sh_sand_man_t sh_dnx_err_rec_comparison_jrnl_man;
extern sh_sand_man_t sh_dnx_err_rec_man;

extern sh_sand_cmd_t sh_dnx_err_rec_transaction_cmds[];
extern sh_sand_cmd_t sh_dnx_err_rec_comparison_cmds[];
extern sh_sand_cmd_t sh_dnx_err_rec_cmds[];

#endif /* DIAG_DNX_ERROR_RECOVERY_H_INCLUDED */

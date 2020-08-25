/*
 * ! \file diag_sand_access.h Purpose: shell registers commands for Dune Devices 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNX_DSIG_H_INCLUDED
#define DIAG_DNX_DSIG_H_INCLUDED

/*
 * Defines
 * {
 */

#define MAX_NUM_OF_INTS_IN_SIG_GETTER (2000)
#define BITS_IN_WORD (32)

/*
 * }
 */

extern sh_sand_cmd_t sh_sand_signal_cmds[];
extern sh_sand_man_t sh_sand_signal_man;

extern const char cmd_sand_signal_usage[];
extern const char cmd_sand_signal_desc[];

/* Serves Legacy invocation */
cmd_result_t cmd_sand_signal(
    int unit,
    args_t * args);

/*
 * takes a hex string and converts it to a hex number
 * returns val - pointer to uint32 array
 */
shr_error_e
hex2int(
    char *hex,
    uint32 *val,
    uint8 verbosity);


/**
 * \brief
 *    Finds a signal according to the specified parameters. The resulting parsed signal tree resides as
 *   the last entry in *sig_list.
 *    By default, the signals are looked up using the following flags:
 *   SIGNALS_MATCH_EXPAND
 *   SIGNALS_MATCH_EXACT
 *   SIGNALS_MATCH_RETRY
 *   If more flags are required, they can be passed as the parameter extra_flags.
 *    If a NULL pointer is passed as *sig_list, (i.e outside this function, sig_list is declared, but
 *   nulled, for example: 'rhlist_t *sig_list = NULL; sand_signal_find(..., &sig_list);'), a new list
 *   will be created. Note that the argument itself must not be NULL.
 *   Note: The list must be freed, even if the signal was not found!
 */
shr_error_e sand_signal_find(
    int unit,
    int core,
    int extra_flags,
    char *block,
    char *from,
    char *to,
    char *name,
    rhlist_t ** sig_list);

/**
* \brief
* Function: compare_adapter_signal_values
* Purpose: Check if the value of the signal that is got from the adapter is equal to the expected one
*
* Params:
* unit      - Unit id
* in_port   - Input Port
* out_port  - Output Port
* block     - Block name
* from      - Stage/Memory signal originated from or passed through
* to        - Stage/Memory signal destined to
* name      - Signal`s name
* compareTo - Expected value of the signal
*/
int compare_adapter_signal_values_inner(
    int unit,
    int in_port,
    int out_port,
    char *block,
    char *from,
    char *to,
    char *name,
    char *sig_exp_value,
    int fail_test_if_signals_mismatch);

#endif /* DIAG_SAND_ACCESS_H_INCLUDED */

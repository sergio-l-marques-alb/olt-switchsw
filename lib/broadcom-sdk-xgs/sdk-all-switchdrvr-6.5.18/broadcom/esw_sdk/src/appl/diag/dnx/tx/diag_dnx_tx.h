
/*! \file diag_dnx_tx.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_TX_H_INCLUDED
#define DIAG_DNX_TX_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <appl/diag/shell.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/
extern char cmd_dnx_tx_usage[];

/*************
 * FUNCTIONS *
 *************/

/*!
 * \brief
 *   Process a tx command from the bcm shell
 *
 *  \par DIRECT INPUT:
 *    \param [in] u -
 *      Unit number.
 *    \param [in] *a -
 *      Pointer to struct with command content.
 *  \par DIRECT OUTPUT:
 *    cmd_result_t - \n
 *      Return value of tx call.
 *  \remark
 *    None
 *  \see
 *    None
 *****************************************************/
cmd_result_t cmd_dnx_tx(
    int u,
    args_t * a);

#endif /* DIAG_DNX_TX_H_INCLUDED */

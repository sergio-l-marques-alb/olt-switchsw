/*! \file diag_dnx_queue.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_DNX_QUEUE_H_INCLUDED
#define DIAG_DNX_QUEUE_H_INCLUDED

/*************
 * INCLUDES  *
 *************/

/*************
 *  DEFINES  *
 *************/
/*************
 *  MACROES  *
 *************/

/*************
 * GLOBALS   *
 *************/

extern sh_sand_cmd_t sh_dnx_tm_queue_cmds[];
extern sh_sand_man_t sh_dnx_tm_queue_man;

/*************
 * FUNCTIONS *
 *************/

/*
 * brief - get base_voq and cosq from voq.
 */
shr_error_e sh_dnx_tm_queue_base_queue_get(
    int unit,
    int queue,
    bcm_core_t core,
    int *base_queue,
    int *num_cos);

#endif /** DIAG_DNX_QUEUE_H_INCLUDED */

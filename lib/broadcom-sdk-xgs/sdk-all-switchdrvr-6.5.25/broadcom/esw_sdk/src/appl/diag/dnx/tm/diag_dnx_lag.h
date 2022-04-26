/** \file diag_dnx_lag.h
 * 
 * DNX TM lag diagnostics
 * 
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNX_LAG_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_LAG_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Creat some lags to provide meaningful content to lag related commands
 */
shr_error_e sh_dnx_lag_init_cb(
    int unit);

extern sh_sand_option_t sh_dnx_lag_arguments[];
extern sh_sand_man_t sh_dnx_lag_man;
extern sh_sand_option_t sh_dnx_flow_agg_arguments[];
extern sh_sand_man_t sh_dnx_flow_agg_man;

/**
 * \brief - get lag information per pool and group
 */
shr_error_e sh_dnx_lag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

/**
 * \brief - get flow agg information per flow agg id
 */
shr_error_e sh_dnx_flow_agg_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_LAG_H_INCLUDED */

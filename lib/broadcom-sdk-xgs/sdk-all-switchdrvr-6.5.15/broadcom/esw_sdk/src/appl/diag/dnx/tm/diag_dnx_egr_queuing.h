/** \file diag_dnx_egr_queuing.h
 * 
 * DNX TM ingress reassembly diagnostics
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNX_EGR_QUEUING_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_EGR_QUEUING_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

extern sh_sand_option_t sh_dnx_egq_shaping_options[];
extern sh_sand_man_t sh_dnx_egq_shaping_man;

/**
 * \brief - dump egq shaping information per port
 */
shr_error_e sh_dnx_egq_shaping_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egq_compensation_options[];
extern sh_sand_man_t sh_dnx_egq_compensation_man;

/**
 * \brief - dump egq compensation information per port
 */
shr_error_e sh_dnx_egq_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_rqp_rate_options[];
extern sh_sand_man_t sh_dnx_egr_rqp_rate_man;

/**
 * \brief - display egress rqp coutner rate
 */
shr_error_e sh_dnx_egr_rqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_pqp_rate_options[];
extern sh_sand_man_t sh_dnx_egr_pqp_rate_man;

/**
 * \brief - display egress pqp coutner rate
 */
shr_error_e sh_dnx_egr_pqp_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_epep_rate_options[];
extern sh_sand_man_t sh_dnx_egr_epep_rate_man;

/**
 * \brief - display egress epep coutner rate
 */
shr_error_e sh_dnx_egr_epep_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_epni_rate_options[];
extern sh_sand_man_t sh_dnx_egr_epni_rate_man;

/**
 * \brief - calculate and display egress epni according to the scheme
 */
shr_error_e sh_dnx_egr_epni_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_option_t sh_dnx_egr_queuing_rate_options[];
extern sh_sand_man_t sh_dnx_egr_queuing_rate_man;

/**
 * \brief - display egress queuing coutner rate
 */
shr_error_e sh_dnx_egr_queuing_rate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_EGR_QUEUING_H_INCLUDED */

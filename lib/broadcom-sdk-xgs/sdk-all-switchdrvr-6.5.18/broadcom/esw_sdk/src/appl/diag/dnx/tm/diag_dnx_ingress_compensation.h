/** \file diag_dnx_ingress_compensation.h
 * 
 * DNX TM ingress compensation diagnostics
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNX_INGRESS_COMPENSATION_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_INGRESS_COMPENSATION_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

extern sh_sand_option_t sh_dnx_ingress_compensation_options[];
extern sh_sand_man_t sh_dnx_ingress_compensation_man;

/**
 * \brief - dump ingress compensation statistics 
 * \param [in] unit - unit id 
 * \param [in] args - args 
 * \param [in] sand_control  -  sand_control
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None 
 */
shr_error_e sh_dnx_ingress_compensation_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /** DIAG_DNX_INGRESS_COMPENSATION_H_INCLUDED */

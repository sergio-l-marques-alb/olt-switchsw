/** \file diag_dnxc_appl_run.h
 * 
 * DNX APPL RUN DIAG PACK - diagnostic pack for application reference
 * 
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXC_APPL_RUN_H_INCLUDED
/*
 * {
 */
#define DIAG_DNXC_APPL_RUN_H_INCLUDED

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

/**
 * \brief - application run diagnostics implementation 
 * This function invokes a reference application requested by the user. 
 * 
 * \param [in] unit - Unit ID
 * \param [in] args - Application name povided by command line
 * \param [in] sand_control - pointer to sand_control
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_dnxc_appl_run_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnxc_appl_run_man;
extern sh_sand_option_t sh_dnxc_appl_run_options[];

/*
 * }
 */
#endif /** DIAG_DNXC_APPL_RUN_H_INCLUDED*/

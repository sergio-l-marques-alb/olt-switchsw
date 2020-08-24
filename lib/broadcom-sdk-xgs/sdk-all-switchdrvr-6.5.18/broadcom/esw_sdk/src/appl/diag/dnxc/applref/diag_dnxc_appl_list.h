/** \file diag_dnxc_appl_list.h
 * 
 * DNX APPL RUN DIAG PACK - diagnostic pack for application reference
 * 
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_DNXC_APPL_LIST_H_INCLUDED
/*
 * {
 */
#define DIAG_DNXC_APPL_LIST_H_INCLUDED

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

/**
 * \brief - application list diagnostics implementation 
 * This function prints a list of applications by name and ID. 
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
shr_error_e sh_dnxc_appl_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

extern sh_sand_man_t sh_dnxc_appl_list_man;

/*
 * }
 */
#endif /** DIAG_DNXC_APPL_LIST_H_INCLUDED*/

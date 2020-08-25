/**
 * \file ctest_dnxc_infra_init.h
 *
 * Infrastructure init utilities, structures and definitions
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DNXC_CTEST_INFRA_INIT_H_INCLUDED
#define DNXC_CTEST_INFRA_INIT_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_man_t sh_dnxc_infra_init_parallel_test_man;

extern sh_sand_invoke_t sh_dnxc_infra_init_parallel_tests[];

extern sh_sand_option_t sh_dnxc_infra_init_parallel_test_options[];

/**
 * \brief - execute parallel (multi threaded) deinit+init for all active units
 *
 * \param [in] unit - Unit ID
 * \param [in] args -  command args
 * \param [in] sand_control - shell sand control
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e sh_dnxc_infra_init_parallel_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* DNXC_CTEST_INFRA_INIT_H_INCLUDED */

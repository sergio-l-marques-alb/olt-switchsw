/** \file consistent_hashing_manager_ctest.h
 *
 * consistent hashing manager c-tests.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef ALGO_CONSISTENT_HASHING_MANAGER_CTEST_H
#define ALGO_CONSISTENT_HASHING_MANAGER_CTEST_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
* INCLUDE FILES:
* {
*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
/**
 * }
 */

/*
 * structs to support bcm shell command
 */
extern sh_sand_option_t dnx_chm_test_options[];
extern sh_sand_man_t dnx_chm_test_man;

shr_error_e dnx_chm_unit_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* ALGO_CONSISTENT_HASHING_MANAGER_CTEST_H */

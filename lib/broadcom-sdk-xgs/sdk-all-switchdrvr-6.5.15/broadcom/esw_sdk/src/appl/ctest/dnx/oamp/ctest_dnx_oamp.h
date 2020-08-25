/**
 * \file ctest_dnx_oamp.h
 *
 * 'Load balancing' operations (set and get parameters) procedures for DNX.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#ifndef CTEST_OAMP_H_INCLUDED
#define CTEST_OAMP_H_INCLUDED

/** Tables defined in ctest_dnx_oamp_traffic.c */
extern sh_sand_option_t diag_dnx_oamp_traffic_test_options[];
extern sh_sand_man_t diag_dnx_oamp_traffic_test_man;
extern sh_sand_invoke_t diag_dnx_oamp_traffic_tests[];

/** Test function defined in ctest_dnx_oamp_traffic.c */
shr_error_e diag_dnx_oamp_traffic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_OAMP_H_INCLUDED */

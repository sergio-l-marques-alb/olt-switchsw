/* \file ctest_dnx_kbp.h
 *
 * Contains all of the KBP diag&test declarations for external usage
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_KBP_H_INCLUDED
#define CTEST_DNX_KBP_H_INCLUDED

#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_invoke_t dnx_kbp_fwd_tests[];
extern sh_sand_option_t dnx_kbp_fwd_options[];
extern sh_sand_man_t dnx_kbp_fwd_man;

extern sh_sand_invoke_t dnx_kbp_acl_tests[];
extern sh_sand_option_t dnx_kbp_acl_options[];
extern sh_sand_man_t dnx_kbp_acl_man;

shr_error_e dnx_kbp_acl_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_KBP_H_INCLUDED */

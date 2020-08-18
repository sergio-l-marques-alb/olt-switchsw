/** \file ctest_dnx_l3.h
 * $Id$
 *
 * Contains the L3 ctest declarations for external usage
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_L3_H_INCLUDED
#define CTEST_DNX_L3_H_INCLUDED
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

extern sh_sand_cmd_t dnx_l3_test_cmds[];
extern sh_sand_man_t dnx_l3_test_man;

extern sh_sand_cmd_t dnx_ecmp_test_cmds[];
extern sh_sand_cmd_t dnx_l3_host_test_cmds[];
extern sh_sand_cmd_t dnx_l3_fec_test_cmds[];

#endif /* CTEST_DNX_L3_H_INCLUDED */

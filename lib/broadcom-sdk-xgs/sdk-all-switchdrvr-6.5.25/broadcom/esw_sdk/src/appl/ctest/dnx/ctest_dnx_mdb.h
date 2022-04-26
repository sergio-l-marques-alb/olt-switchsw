/*
 * ! \file mdb_diag.h Contains all of the MDB diag&test declarations for external usage
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_MDB_H_INCLUDED
#define CTEST_DNX_MDB_H_INCLUDED

extern sh_sand_invoke_t dnx_mdb_tests[];
extern sh_sand_option_t dnx_mdb_test_options[];
extern sh_sand_man_t dnx_mdb_test_man;

shr_error_e cmd_mdb_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_MDB_H_INCLUDED */

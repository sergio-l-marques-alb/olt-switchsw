/*
 * ! \file diag_sand_access.h Purpose: shell registers commands for Dune Devices 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef DIAG_SAND_ACCESS_H_INCLUDED
#define DIAG_SAND_ACCESS_H_INCLUDED

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>

#define MAX_FIELDS_NUM  256
#define SAND_DCL_CMD(_f)  \
    extern cmd_result_t _f(int, args_t *); \
    extern char     _f##_usage[];

extern sh_sand_man_t sh_sand_access_man;
extern sh_sand_cmd_t sh_sand_access_cmds[];

extern const char cmd_sand_access_usage[];
extern const char cmd_sand_access_desc[];

cmd_result_t cmd_sand_access(
    int unit,
    args_t * a);

SAND_DCL_CMD(cmd_sand_reg_list)
SAND_DCL_CMD(cmd_sand_reg_get) SAND_DCL_CMD(cmd_sand_reg_set) SAND_DCL_CMD(cmd_sand_reg_modify)
     int
     diag_sand_reg_get_all(
    int unit,
    int is_debug);

/*
 * List the tables, or fields of a table entry
 */
SAND_DCL_CMD(cmd_sand_mem_list)
SAND_DCL_CMD(cmd_sand_mem_get)
SAND_DCL_CMD(cmd_sand_mem_write)
SAND_DCL_CMD(cmd_sand_mem_modify)
SAND_DCL_CMD(cmd_sand_dma)
SAND_DCL_CMD(cmd_sand_intr)

#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__)
SAND_DCL_CMD(cmd_sand_cpu_i2c)
SAND_DCL_CMD(cmd_sand_cpu_regs)
#endif

SAND_DCL_CMD(cmd_sand_pcie_reg)
SAND_DCL_CMD(cmd_sand_pcic_access)
#endif /* DIAG_SAND_ACCESS_H_INCLUDED */

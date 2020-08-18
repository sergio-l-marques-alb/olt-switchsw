/**
 * \file ctest_dnx_root.c
 *
 * Root for ctests for DNX devices
 *
 */

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/ctest/dnx/ctest_dnx_swstate.h>
#include <appl/ctest/dnx/ctest_dnx_err_recovery.h>
#include "dbal/ctest_dnx_dbal.h"
#include "algo/ctest_dnx_algo.h"
#include "ctest_dnx_mdb.h"
#include "ctest_dnx_packet.h"

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

extern sh_sand_cmd_t dnx_l2_test_cmds[];
extern sh_sand_cmd_t dnx_l3_test_cmds[];
extern sh_sand_cmd_t dnx_tm_test_cmds[];
extern sh_sand_cmd_t dnx_bfd_test_cmds[];
extern sh_sand_cmd_t dnx_oam_test_cmds[];
extern sh_sand_cmd_t dnx_lif_test_cmds[];
extern sh_sand_cmd_t dnx_field_test_cmds[];
extern sh_sand_cmd_t Dnx_switch_test_cmds[];
extern sh_sand_cmd_t dnx_data_test_cmds[];
extern sh_sand_cmd_t dnx_oamp_test_cmds[];
extern sh_sand_cmd_t dnx_vlan_test_cmds[];
extern sh_sand_cmd_t dnx_mpls_test_cmds[];
extern sh_sand_cmd_t dnx_rx_trap_test_cmds[];
extern sh_sand_cmd_t dnx_infra_test_cmds[];
extern sh_sand_cmd_t dnx_crps_test_cmds[];
extern sh_sand_cmd_t dnx_kbp_test_cmds[];
extern sh_sand_cmd_t ctest_dnx_access_cmds[];
/*
 * 'Man' for 'ctest switch'
 * Currently only short description.
 */
static sh_sand_man_t Switch_man = {
    "'Switch'-related test utilities",
};

/* *INDENT-OFF* */
sh_sand_man_t sh_dnx_ctest_man = {
    .brief = "Run unit tests",
    .full  = "Root command for DNX specific tests. Use \"ct cmdname[s] usage\" for details\n"
};

sh_sand_cmd_t sh_dnx_ctest_commands[] = {
   /* Name         | Leaf Action | Junction Array Pointer   | Option list for leaf | Man page         | Dynamic Option CB   */
    {"access",         NULL,                ctest_dnx_access_cmds},
    {"algo",           NULL,                sh_dnx_algo_test_cmds},
    {"bfd",            NULL,                dnx_bfd_test_cmds},
    {"crps",           NULL,                dnx_crps_test_cmds},
    {"data",           NULL,                dnx_data_test_cmds},
    {"error_recovery", NULL,                dnx_err_recovery_test_cmds},
    {"field",          NULL,                dnx_field_test_cmds},
    {"layer2",         NULL,                dnx_l2_test_cmds},
    {"layer3",         NULL,                dnx_l3_test_cmds},
    {"framework",      NULL,                dnxc_framework_test_cmds},
    {"lif",            NULL,                dnx_lif_test_cmds},
    {"mdb",            cmd_mdb_test,        NULL,              dnx_mdb_test_options, &dnx_mdb_test_man, NULL, dnx_mdb_tests},
    {"tx",             dnx_packet_test_cmd, NULL, dnx_packet_test_options, &dnx_packet_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"oam",            NULL,                dnx_oam_test_cmds},
    {"switch",         NULL,                Dnx_switch_test_cmds, NULL,   &Switch_man},
    {"swstate",        NULL,                dnx_swstate_test_cmds},
    {"tm",             NULL,                dnx_tm_test_cmds},
    {"rx_trap",        NULL,                dnx_rx_trap_test_cmds},
    {"oamp",           NULL,                dnx_oamp_test_cmds},
    {"vlan",           NULL,                dnx_vlan_test_cmds},
    {"mpls",           NULL,                dnx_mpls_test_cmds},
    {"kbp",            NULL,                dnx_kbp_test_cmds},
    {"INFRAstructure", NULL,                dnx_infra_test_cmds},
    {"dbal",           NULL,                dnx_dbal_test_cmds}, /* Must be the last before NULL */
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */

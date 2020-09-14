/**
 * \file ctest_dnx_root.c
 *
 * Root for ctests for DNX devices
 *
 */

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/ctest/dnx/ctest_dnx_root.h>
#include <appl/diag/dnx/diag_dnx_fabric.h>
#include "dbal/ctest_dnx_dbal.h"
#include "algo/ctest_dnx_algo.h"
#include "ctest_dnx_mdb.h"

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

extern sh_sand_cmd_t dnx_packet_test_cmds[];
extern sh_sand_cmd_t dnx_pparse_test_cmds[];
extern sh_sand_cmd_t dnx_l2_test_cmds[];
extern sh_sand_cmd_t dnx_l3_test_cmds[];
extern sh_sand_cmd_t dnx_tm_test_cmds[];
extern sh_sand_cmd_t dnx_fabric_test_cmds[];
extern sh_sand_cmd_t dnx_bfd_test_cmds[];
extern sh_sand_cmd_t dnx_oam_test_cmds[];
extern sh_sand_cmd_t dnx_lif_test_cmds[];
extern sh_sand_cmd_t dnx_lif_table_mngr_test_cmds[];
extern sh_sand_cmd_t dnx_field_test_cmds[];
extern sh_sand_cmd_t Dnx_switch_test_cmds[];
extern sh_sand_cmd_t dnx_data_test_cmds[];
extern sh_sand_cmd_t dnx_diag_test_cmds[];
extern sh_sand_cmd_t dnx_oamp_test_cmds[];
extern sh_sand_cmd_t dnx_vlan_test_cmds[];
extern sh_sand_cmd_t dnx_mpls_test_cmds[];
extern sh_sand_cmd_t dnx_tdm_test_cmds[];
extern sh_sand_cmd_t dnx_rx_trap_test_cmds[];
extern sh_sand_cmd_t dnx_infra_test_cmds[];
extern sh_sand_cmd_t dnx_crps_test_cmds[];
extern sh_sand_cmd_t dnx_kbp_test_cmds[];
extern sh_sand_cmd_t dnx_kaps_test_cmds[];
extern sh_sand_cmd_t ctest_dnx_access_cmds[];
extern sh_sand_cmd_t dnx_srv6_test_cmds[];
extern sh_sand_cmd_t dnx_trunk_test_cmds[];
extern sh_sand_cmd_t dnxc_framework_test_cmds[];
extern sh_sand_cmd_t dnx_swstate_test_cmds[];
extern sh_sand_cmd_t dnx_bare_metal_cmds[];
extern sh_sand_cmd_t dnx_apt_cmds[];

extern sh_sand_cmd_t dnx_err_recovery_test_cmds[];

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
   /* Name           | Leaf Action | Junction Array Pointer   | Option list for leaf | Man page         | Dynamic Option CB   */
    {"access",         NULL,                ctest_dnx_access_cmds},
    {"algo",           NULL,                sh_dnx_algo_test_cmds},
    {"apt",            NULL,                dnx_apt_cmds},
    {"bare_metal",     NULL,                dnx_bare_metal_cmds},
    {"bfd",            NULL,                dnx_bfd_test_cmds},
    {"crps",           NULL,                dnx_crps_test_cmds},
    {"data",           NULL,                dnx_data_test_cmds},
    {"diag",           NULL,                dnx_diag_test_cmds},
    {"error_recovery", NULL,                dnx_err_recovery_test_cmds},
    {"fabric",         NULL,                dnx_fabric_test_cmds, NULL, NULL, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_fabric_available},
    {"field",          NULL,                dnx_field_test_cmds},
    {"layer2",         NULL,                dnx_l2_test_cmds},
    {"layer3",         NULL,                dnx_l3_test_cmds, NULL, NULL, NULL, NULL, CTEST_UM},
    {"framework",      NULL,                dnxc_framework_test_cmds},
    {"lif",            NULL,                dnx_lif_test_cmds},
    {"lif_table_mngr", NULL,                dnx_lif_table_mngr_test_cmds},
    {"mdb",            cmd_mdb_test,        NULL, dnx_mdb_test_options, &dnx_mdb_test_man, NULL, dnx_mdb_tests, CTEST_UM},
    {"packet",         NULL,                dnx_packet_test_cmds},
    {"pparse",         NULL,                dnx_pparse_test_cmds},
    {"oam",            NULL,                dnx_oam_test_cmds},
    {"switch",         NULL,                Dnx_switch_test_cmds, NULL,   &Switch_man},
    {"swstate",        NULL,                dnx_swstate_test_cmds},
    {"tm",             NULL,                dnx_tm_test_cmds},
    {"trunk",          NULL,                dnx_trunk_test_cmds},
    {"rx_trap",        NULL,                dnx_rx_trap_test_cmds},
    {"oamp",           NULL,                dnx_oamp_test_cmds},
    {"vlan",           NULL,                dnx_vlan_test_cmds},
    {"mpls",           NULL,                dnx_mpls_test_cmds},
    {"tdm",            NULL,                dnx_tdm_test_cmds, NULL, NULL,  NULL, NULL, CTEST_POSTCOMMIT|SH_CMD_CONDITIONAL, sh_dnx_is_tdm_available},
#ifdef INCLUDE_KBP
    {"kbp",            NULL,                dnx_kbp_test_cmds, .label="kbp"},
#endif
    {"kaps",           NULL,                dnx_kaps_test_cmds},
    {"INFRAstructure", NULL,                dnx_infra_test_cmds},
    {"srv6",           NULL,                dnx_srv6_test_cmds},
    {"dbal",           NULL,                dnx_dbal_test_cmds}, /* Must be the last before NULL */
    {NULL}      /* This line should always stay as last one */
};
/* *INDENT-ON* */

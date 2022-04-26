/**
 **  \file dnx\ctest_dnx_diag.c
 *
 * File:        ctest_dnx_diag.c
 * Purpose:     Utility to send packet and verify diag counter as a result
 * Requires:
 */
/*
 * All Rights Reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX
#include <sal/core/boot.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/reference/dnx/appl_ref_e2e_scheme.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include "../../diag/sand/diag_sand_framework_internal.h"

extern sh_sand_invoke_t dnx_diag_counter_tests[];
extern sh_sand_man_t dnx_diag_counter_man;

#define CTEST_DNX_DIAG_COUNTER_G_UC_XML_FILE "shell_diag_counter_g_uc.xml"

static shr_error_e
dnx_get_first_and_only_port(
    int unit,
    bcm_pbmp_t logical_ports,
    bcm_port_t * dst_port_p)
{
    int count;
    bcm_port_t dst_port;

    SHR_FUNC_INIT_VARS(unit);
    BCM_PBMP_COUNT(logical_ports, count);
    if (count != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There should be exactly one logical port in input bitmap\r\n");
    }
    _SHR_PBMP_FIRST(logical_ports, dst_port);
    *dst_port_p = dst_port;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_diag_counter_g_uc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_name = "dnx_diag_counter_g_uc";
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    int flags = 0;
    uint32 vlan;
    char *command;
    char full_command[SH_SAND_MAX_TOKEN_SIZE];
    char gold_file_name[SH_SAND_MAX_TOKEN_SIZE];
    char local_file_name[SH_SAND_MAX_TOKEN_SIZE];
    char file_only[SH_SAND_MAX_TOKEN_SIZE];
    char folder[SH_SAND_MAX_TOKEN_SIZE] = { 0 };
    char filter[RHNAME_MAX_SIZE] = { 0 };
    int to_clean = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    /** Device check */
    if (SAL_BOOT_PLISIM)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Test is not supported on adapter\n");
    }
    SH_SAND_GET_PORT("port", logical_ports);
    SHR_IF_ERR_EXIT(dnx_get_first_and_only_port(unit, logical_ports, &port));
    SH_SAND_GET_UINT32("VLan", vlan);

    /*
     * clear counter
     */
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nStep1. Clear diag counter\n")));
    command = "diag counter g";
    sal_snprintf(full_command, SH_SAND_MAX_TOKEN_SIZE - 1, "%s", command);
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, full_command), "");

    /*
     * tx 1 packet
     */
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\nStep2. Tx 1 uc packet to port %d with vlan_id %d\n"), port, vlan));
    sal_snprintf(full_command, SH_SAND_MAX_TOKEN_SIZE - 1, "tx 1 pbm=%d vl=%d", port, vlan);
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, full_command), "");

    /*
     * Get gold path
     */
    command = "diag_counter_g_uc";
    sh_sand_gold_filename(command, file_only);
    SHR_CLI_EXIT_IF_ERR(sh_sand_gold_path(unit, flags, file_only, gold_file_name),
                        "Gold result file:'%s' cannot be obtained\n", gold_file_name);
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Gold path is %s\n"), gold_file_name));

    /*
     * Output xml file
     */
    if (dbx_file_get_sdk_path(unit, "xml/", "/tests/dnx/", folder, FALSE) < 0)
    {
        SHR_CLI_EXIT_IF_ERR(dbx_file_get_sdk_path(unit, "xml/", "/regress/bcm/tests/dnx/", folder, TRUE),
                            "could not get sdk path");
    }
    if (!dbx_file_dir_exists(folder))
    {
        SHR_CLI_EXIT_IF_ERR(dbx_file_dir_create(folder), "Failed to create %s\n", folder);
    }
    SHR_IF_ERR_EXIT(dbx_file_add_device_specific_suffix(unit, folder, TRUE));
    sal_snprintf(local_file_name, SH_SAND_MAX_TOKEN_SIZE - 1, "%s/%s", folder, file_only);

    command = "diag counter g file=";
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "\nStep3. Output diag counter g result to xml file=%s\n"), local_file_name));
    sal_snprintf(full_command, SH_SAND_MAX_TOKEN_SIZE - 1, "%s%s", command, local_file_name);

    /*
     * Remove file if file exists
     */
    if (dbx_file_exists(local_file_name))
    {
        SHR_CLI_EXIT_IF_ERR(dbx_file_remove(local_file_name), "");
    }
    SHR_CLI_EXIT_IF_ERR(sh_sand_execute(unit, full_command), "");

    /*
     * Compare the output xml file with gold xml
     */
    /*
     * Use filter simple in FilterCase.xml, skip line with "CPU__not_compared_to_gold_only_for_debugging"
     */
    sal_snprintf(filter, RHNAME_MAX_SIZE - 1, "simple");
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "\nStep4. Compare the output xml file %s with gold xml %s\n"), local_file_name,
              gold_file_name));
    SHR_CLI_EXIT_IF_ERR(sh_sand_gold_compare(unit, local_file_name, file_only, filter, flags),
                        "The output file is inconsistent with gold file\n");

exit:
    SH_SAND_GET_BOOL("clean", to_clean);
    if (to_clean == TRUE)
    {
        /*
         * Remove test file
         */
        if (dbx_file_exists(local_file_name))
        {
            SHR_CLI_EXIT_IF_ERR(dbx_file_remove(local_file_name), "");
        }
    }
    if (SHR_FUNC_ERR())
    {
        LOG_CLI_EX("\r\n" "Test '%s' FAIL. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        LOG_CLI_EX("\r\n" "Test '%s' PASS. %s%s%s\r\n", test_name, EMPTY, EMPTY, EMPTY);
    }
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_diag_counter_g_uc_options[] = {
    {"port", SAL_FIELD_TYPE_PORT, "port to send packet", "eth13"}
    ,
    {"VLan", SAL_FIELD_TYPE_UINT32, "VLAN ID for sending packet", "2"}
    ,
    {"clean", SAL_FIELD_TYPE_BOOL, "Clean the output xml after test", "yes"}
    ,
    {NULL}
};

sh_sand_invoke_t dnx_diag_counter_g_tests[] = {
    {"DIAG_counter_g_Test", "Test", CTEST_PRECOMMIT}
    ,
    {NULL}
};

sh_sand_man_t dnx_diag_counter_g_uc_man = {
    "Test diag counter g with uc packets",
    "Test diag counter g with uc packets, and verify the counter with gold xml file",
    "",
    ""
};

sh_sand_man_t dnx_diag_counter_g_man = {
    "Test diag counter g with different packets types",
    "Test diag counter g with different packets types",
    "",
    ""
};

sh_sand_man_t dnx_diag_counter_man = {
    "Test diag counter with different packet types",
    "Test diag counter g and other blocks with different packet types",
    "",
    ""
};

sh_sand_cmd_t dnx_diag_counter_g_cmds[] = {
    {"unicast", dnx_diag_counter_g_uc_cmd, NULL, dnx_diag_counter_g_uc_options, &dnx_diag_counter_g_uc_man, NULL}
    ,
    {NULL}
};

sh_sand_cmd_t dnx_diag_counter_cmds[] = {
    {"Graphical", NULL, dnx_diag_counter_g_cmds, NULL, &dnx_diag_counter_g_man, NULL, dnx_diag_counter_g_tests}
    ,
    {NULL}
};

sh_sand_cmd_t dnx_diag_test_cmds[] = {
    {"COUnter", NULL, dnx_diag_counter_cmds, NULL, &dnx_diag_counter_man}
    ,
    {NULL}
};

/** \file ctest_dnx_ipfix_tests.c
 *
 * Tests for IPFIX
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/instru.h>

int encap[5];
int encap_index = 0;
int interface_index = 0;

int exp_in_port = 200;
int exp_out_port = 100;
uint16 exp_intf_in = 1000;
uint32 exp_vrf_in = 5000;
uint16 exp_intf_out = 2000;
uint32 exp_vrf_out = 6000;

int
encap_call_back(
    int unit,
    bcm_instru_ipfix_encap_info_t * ipfix_encap_info,
    void *user_data)
{
    int rv = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (ipfix_encap_info->ipfix_encap_id != encap[encap_index])
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error: Encap #%d - expected: 0x%x, Actual: 0x%x\n", encap_index, encap[encap_index],
                     ipfix_encap_info->ipfix_encap_id);
    }
    else
    {
        sal_printf("Encap #%d - expected: 0x%x, Actual: 0x%x\n", encap_index, encap[encap_index],
                   ipfix_encap_info->ipfix_encap_id);
    }

    rv = bcm_instru_ipfix_encap_delete(unit, ipfix_encap_info);
    if (rv != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_instru_ipfix_encap_delete #%d\n", rv, encap_index);
    }

    encap_index++;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ipfix_encap_traverse_test(
    int unit)
{
    int i;
    int rv = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < 5; i++)
    {
        /*
         * Create the IPFIX ETPS entry.
         */
        bcm_instru_ipfix_encap_info_t ipfix_encap_info;
        sal_memset(&ipfix_encap_info, 0, sizeof(ipfix_encap_info));

        rv = bcm_instru_ipfix_encap_create(unit, &ipfix_encap_info);
        if (rv != BCM_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_instru_ipfix_encap_create #%d\n", rv, i);
        }
        encap[i] = ipfix_encap_info.ipfix_encap_id;
    }

    rv = bcm_instru_ipfix_encap_traverse(unit, &encap_call_back, (void *) 0);
    if (rv != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_instru_ipfix_encap_traverse\n", rv);
    }

    sal_printf("\n ---  IPFIX encap traverse test PASSED  --\n\n");

exit:
    SHR_FUNC_EXIT;
}

int
interface_call_back(
    int unit,
    bcm_instru_ipfix_interface_info_t * ipfix_interface_info,
    void *user_data)
{
    int rv = 0;
    int exp_port;
    int exp_intf;
    int exp_vrf;
    SHR_FUNC_INIT_VARS(unit);

    if (ipfix_interface_info->flags == BCM_INSTRU_IPFIX_INTERFACE_INPUT)
    {
        exp_port = exp_in_port;
        exp_intf = exp_intf_in;
        exp_vrf = exp_vrf_in;
    }
    else
    {
        exp_port = exp_out_port;
        exp_intf = exp_intf_out;
        exp_vrf = exp_vrf_out;
    }
    if (ipfix_interface_info->port != exp_port + interface_index)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error: Interface port #%d - expected: 0x%x, Actual: 0x%x\n", interface_index,
                     exp_port + interface_index, ipfix_interface_info->port);
    }
    else
    {
        sal_printf("Interface  - port #%d - expected: 0x%x, Actual: 0x%x\n", interface_index,
                   exp_port + interface_index, ipfix_interface_info->port);
    }
    if (ipfix_interface_info->interface != exp_intf + interface_index * 10)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error: Interface  - interface #%d - expected: 0x%x, Actual: 0x%x\n", interface_index,
                     exp_intf + interface_index * 10, ipfix_interface_info->interface);
    }
    else
    {
        sal_printf("Interface  - interface #%d - expected: 0x%x, Actual: 0x%x\n", interface_index,
                   exp_intf + interface_index * 10, ipfix_interface_info->interface);
    }
    if (ipfix_interface_info->vrf_id != exp_vrf + interface_index * 100)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error: Interface - vrf #%d - expected: 0x%x, Actual: 0x%x\n", interface_index,
                     exp_vrf + interface_index * 100, ipfix_interface_info->vrf_id);
    }
    else
    {
        sal_printf("Interface - vrf #%d - expected: 0x%x, Actual: 0x%x\n", interface_index,
                   exp_vrf + interface_index * 100, ipfix_interface_info->vrf_id);
    }
    rv = bcm_instru_ipfix_interface_remove(unit, ipfix_interface_info);
    if (rv != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_instru_ipfix_interface_remove \n", rv);
    }

    interface_index++;
    if (interface_index == 5)
    {
        interface_index = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ipfix_interface_traverse_test(
    int unit)
{
    int i;
    bcm_instru_ipfix_interface_traverse_info_t ipfix_interface_traverse_info;
    bcm_instru_ipfix_interface_info_t ipfix_iterface;
    int output_interface;
    int rv = 0;
    int gport = 200;
    int in_port = 200;
    int out_port = 100;
    uint16 intf_in = 1000;
    uint32 vrf_in = 5000;
    uint16 intf_out = 2000;
    uint32 vrf_out = 6000;

    SHR_FUNC_INIT_VARS(unit);

    for (i = 0; i < 5; i++)
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(gport, in_port);

        ipfix_iterface.flags = BCM_INSTRU_IPFIX_INTERFACE_INPUT;
        ipfix_iterface.port = gport;
        ipfix_iterface.interface = intf_in;
        ipfix_iterface.vrf_id = vrf_in;
        rv = bcm_instru_ipfix_interface_add(unit, &ipfix_iterface);
        if (rv != BCM_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_instru_ipfix_interface_add input #%d\n", rv, i);
        }

        ipfix_iterface.flags = BCM_INSTRU_IPFIX_INTERFACE_OUTPUT;
        output_interface = out_port & 0xff;
        ipfix_iterface.port = output_interface;
        ipfix_iterface.interface = intf_out;
        ipfix_iterface.vrf_id = vrf_out;
        rv = bcm_instru_ipfix_interface_add(unit, &ipfix_iterface);
        if (rv != BCM_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_instru_ipfix_interface_add output #%d\n", rv, i);
        }

        gport += 1;
        in_port += 1;
        out_port += 1;
        intf_in += 10;
        intf_out += 10;
        vrf_in += 100;
        vrf_out += 100;

    }

    ipfix_interface_traverse_info.flags = BCM_INSTRU_IPFIX_INTERFACE_INPUT;
    rv = bcm_instru_ipfix_interface_traverse(unit, &ipfix_interface_traverse_info, interface_call_back, (void *) 0);
    if (rv != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_dnx_instru_ipfix_interface_traverse\n", rv);
    }

    ipfix_interface_traverse_info.flags = BCM_INSTRU_IPFIX_INTERFACE_OUTPUT;
    rv = bcm_instru_ipfix_interface_traverse(unit, &ipfix_interface_traverse_info, interface_call_back, (void *) 0);
    if (rv != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Error (%d), in bcm_dnx_instru_ipfix_interface_traverse\n", rv);
    }
    sal_printf("\n ---  IPFIX interface traverse test PASSED  --\n\n");
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_ipfix_traverse_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(dnx_ipfix_encap_traverse_test(unit), "IPFIX encap traverse test failed\n");
    SHR_CLI_EXIT_IF_ERR(dnx_ipfix_interface_traverse_test(unit), "IPFIX interface traverse test failed\n");

exit:
    SHR_FUNC_EXIT;
}

/** Test manual   */
static sh_sand_man_t dnx_ipfix_config_man = {
    /** Brief */
    "Semantic test of IPFIX traverse APIs",
    /** Full */
    "Create multiple IPFIX encaps and interfaces.  Traverse IPFIX encaps and interfaces and compare."
    /** Synopsis   */
        "ctest ipfix traverse",
    /** Example   */
    "",
};

/* *INDENT-OFF* */
sh_sand_cmd_t dnx_ipfix_test_cmds[] = {
    /** CMD_name |Action                 |Child               |Options                   |Manual                 |CB   |Static invokes |Flags                               |Invoke_CB                     */
    {"traverse",    dnx_ipfix_traverse_test_cmd,  NULL,          NULL,                     &dnx_ipfix_config_man,  NULL, NULL,            CTEST_POSTCOMMIT,  NULL},
    {NULL}
};
/* *INDENT-ON* */

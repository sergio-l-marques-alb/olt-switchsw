/* \file ctests_dnx_priority_propagation_tests.c
 *
 * Tests for priority
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL
/************
 * INCLUDES  *
 *************/
#include "ctest_dnx_priority_propagation_tests.h"

#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>

#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/sat/sat.h>

#include <include/appl/diag/dnx/diag_dnx_l2.h>

#include <shared/shrextend/shrextend_debug.h>

#include <sal/appl/sal.h>
#include <sal/compiler.h>

/************
 * TYPEDEFS  *
 *************/

 /************
 *  DEFINES   *
 *************/

/************
* FUNCTIONS *
*************/
shr_error_e
sh_cmd_port_priority_propagation_supported(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * this is done to prevent access to DNX-Data before it is init
     */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }

    if (dnx_data_sch.general.feature_get(unit, dnx_data_sch_general_port_priority_propagation_supported) != TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }
exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_tm_priority_basic_test_man = {
    "Tests for Port Priority Propagation",
    "Test adding two ports with Priority propagation with different priority and check if the traffic goes to port with High priority\n",
    "ctest tm scheduler priority_propagation basic\n",
    "enable=0 priority=2"
};

/**
 * \brief
 *   Options list for 'priority' shell command
 */
/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_tm_priority_basic_test_options[] = {
    /* Name         Type                  Description                                 Default */
    {"SAT",         SAL_FIELD_TYPE_INT32, "Port that will be used by SAT generator",  "1"},
    {"port",        SAL_FIELD_TYPE_INT32, "ILKN port that will receive the traffic",  "6"},
    {"enable",      SAL_FIELD_TYPE_BOOL,  "Port priority propagation enable/disable", "0"},
    {"priority",    SAL_FIELD_TYPE_INT32, "Number of priorities   2/4/8",             "2"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */

/**
 * \brief
 *   List of tests for 'priority' shell command (to be run on
 *   regression, precommit, etc.)
 */
sh_sand_invoke_t sh_dnx_tm_priority_basic_tests[] = {
    {"priority_propagation_disabled_num_priorities_2", "SAT=1 port=6 enable=0 priority=2", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_disabled_num_priorities_4", "SAT=1 port=6 enable=0 priority=4", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_disabled_num_priorities_8", "SAT=1 port=6 enable=0 priority=8", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_enabled_num_priorities_2", "SAT=1 port=6 enable=1 priority=2", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_enabled_num_priorities_4", "SAT=1 port=6 enable=1 priority=4", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_enabled_num_priorities_8", "SAT=1 port=6 enable=1 priority=8", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

shr_error_e
egress_shapers_rates_set(
    int unit,
    int egress_port,
    int rate)
{
    bcm_cosq_gport_info_t gport_info;
    int egress_if;
    int cos;
    int num_cosq;

    SHR_FUNC_INIT_VARS(unit);

    /** Getting handle to Local port */
    gport_info.in_gport = egress_port;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPort, &gport_info));

    /** Rate - Interface */
    SHR_IF_ERR_EXIT(bcm_fabric_port_get(unit, gport_info.out_gport, 0, &egress_if));
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, egress_if, 0, 0, rate, 0));

    /** Rate - port */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, 0, 0, rate, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, egress_port, &num_cosq));

    for (cos = 0; cos < num_cosq; cos++)
    {
        gport_info.cosq = cos;

        /** Rate - TC port */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPortTC, &gport_info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, gport_info.cosq, 0, rate, 0));

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
e2e_shapers_rates_set(
    int unit,
    int egress_port,
    int rate)
{
    bcm_cosq_gport_info_t gport_info;
    int egress_if;
    int cos;
    int num_cosq;

    SHR_FUNC_INIT_VARS(unit);

    /** Getting handle to E2E port */
    gport_info.in_gport = egress_port;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPort, &gport_info));

    /** Rate - Interface */
    SHR_IF_ERR_EXIT(bcm_fabric_port_get(unit, gport_info.out_gport, 0, &egress_if));
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, egress_if, 0, 0, rate, 0));

    /** Rate - port */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, 0, 0, rate, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, egress_port, &num_cosq));

    for (cos = 0; cos < num_cosq; cos++)
    {
        gport_info.cosq = cos;

        /** Rate - TC port */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC, &gport_info));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport, gport_info.cosq, 0, rate, 0));

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tm_priority_basic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t src_port, dest_port1, dest_port2;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    bcm_core_t core_id_src, core_id_dest;
    int nif_interface_id_dest = 0;
    uint32 tm_dest_port1;
    char ucode_dest_port1_name[RHNAME_MAX_SIZE], ucode_dest_port1_value[RHNAME_MAX_SIZE];
    char ucode_dest_port2_name[RHNAME_MAX_SIZE], ucode_dest_port2_value[RHNAME_MAX_SIZE];
    char enable[RHNAME_MAX_SIZE], number[RHNAME_MAX_SIZE];
    char ports_to_remove[3][RHNAME_MAX_SIZE];
    char ports_priority[2][RHNAME_MAX_SIZE];
    rhhandle_t ctest_soc_set_h = NULL;
    int priority_en = FALSE;
    int i_pipe;
    int credit = 5;
    uint32 vid_dest = 101;
    rhhandle_t packet1_h[2];
    int stream_rate = 80000000; 
    int rate = 100000000;
    int packet_size = 128;
    int stream_duration = 1;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 1000;
    int stream_granularity = -1;
    uint64 stream1_rx_packet_count, stream2_rx_packet_count, bytes_count, stream1_tx_packet_count,
        stream2_tx_packet_count;
    bcm_l2_addr_t l2addr;
    sal_mac_addr_t src_mac_addr = { 00, 00, 00, 00, 00, 01 };
    sal_mac_addr_t dst_mac_addr = { 00, 00, 00, 00, 00, 02 };
    sal_mac_addr_t dst_mac_addr2 = { 00, 00, 00, 00, 00, 03 };
    char mac_str[RHNAME_MAX_SIZE];
    bcm_pbmp_t p, u;
    uint64 temp_packet_count;
    uint32 pcp1 = 0, pcp2 = 4, dei = 0;
    int percentage1, percentage2;
    int num_priorities;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

#if defined(ADAPTER_SERVER_MODE)
    SHR_ERR_EXIT(_SHR_E_NONE, "Priority propagation traffic tests are not available over adapter.\n");
#endif
    /*
     * Get input from shell
     */
    SH_SAND_GET_INT32("SAT", src_port);
    SH_SAND_GET_INT32("port", dest_port1);
    SH_SAND_GET_BOOL("enable", priority_en);
    SH_SAND_GET_INT32("priority", num_priorities);

    /*
     * Get port parameters and use the soc property to add them again.
     */
    dest_port2 = dest_port1 + 1;
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, dest_port1, &nif_interface_id_dest /** ilkn id */ ));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, dest_port1, &core_id_dest, &tm_dest_port1));
    sal_snprintf(enable, RHNAME_MAX_SIZE, "%d", priority_en);
    sal_snprintf(number, RHNAME_MAX_SIZE, "%d", num_priorities);
    sal_snprintf(ports_priority[0], RHNAME_MAX_SIZE, "port_priorities_%d", dest_port1);
    sal_snprintf(ports_priority[1], RHNAME_MAX_SIZE, "port_priorities_%d", dest_port2);
    if (priority_en)
    {
        sal_snprintf(ucode_dest_port1_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port1);
        sal_snprintf(ucode_dest_port1_value, RHNAME_MAX_SIZE, "ILKN%d.1:core_%d.%d:sch_priority_propagation",
                     nif_interface_id_dest, core_id_dest, tm_dest_port1);
        sal_snprintf(ucode_dest_port2_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port2);
        sal_snprintf(ucode_dest_port2_value, RHNAME_MAX_SIZE, "ILKN%d.2:core_%d.%d:sch_priority_propagation",
                     nif_interface_id_dest, core_id_dest, dest_port2);
    }
    else
    {
        sal_snprintf(ucode_dest_port1_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port1);
        sal_snprintf(ucode_dest_port1_value, RHNAME_MAX_SIZE, "ILKN%d.1:core_%d.%d", nif_interface_id_dest,
                     core_id_dest, tm_dest_port1);
        sal_snprintf(ucode_dest_port2_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port2);
        sal_snprintf(ucode_dest_port2_value, RHNAME_MAX_SIZE, "ILKN%d.2:core_%d.%d", nif_interface_id_dest,
                     core_id_dest, dest_port2);
    }

    /*
     * Here we calculate which ports needs to be removed.
     */
    for (int i = 0; i < 3; i++)
    {
        sal_snprintf(ports_to_remove[i], RHNAME_MAX_SIZE, "ucode_port_%d*", (dest_port2 - i));
    }

    {
        ctest_soc_property_t ctest_soc_property[] = {
            {ports_to_remove[0], NULL},
            {ports_to_remove[1], NULL},
            {ports_to_remove[2], NULL},
            {"port_sch_priority_propagation_enable", enable},
            {ucode_dest_port1_name, ucode_dest_port1_value},
            {ucode_dest_port2_name, ucode_dest_port2_value},
            {ports_priority[0], number},
            {ports_priority[1], number},
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    /*
     * Configure the L2 forwarding
     */
    bcm_l2_addr_t_init(&l2addr, dst_mac_addr, vid_dest);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = dest_port1;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    bcm_l2_addr_t_init(&l2addr, dst_mac_addr2, vid_dest);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = dest_port2;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vid_dest));
    BCM_PBMP_PORT_SET(p, src_port);
    BCM_PBMP_PORT_ADD(p, dest_port1);
    BCM_PBMP_PORT_ADD(p, dest_port2);
    BCM_PBMP_CLEAR(u);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vid_dest, p, u));

    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid_dest, src_port, 0));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid_dest, dest_port1, 0));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid_dest, dest_port2, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id_src));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));

    SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, dest_port1, 1));
    SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, dest_port2, 1));

    /*
     * Forward the traffic returned from ILKN ports to the src_port  instead of dropping it
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, dest_port1, src_port, 1));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, dest_port2, src_port, 1));

    for (i_pipe = 0; i_pipe < 4; i_pipe++)
    {
        SHR_IF_ERR_EXIT(dnx_sat_gtf_credit_config(unit, i_pipe, credit));
    }

    /*
     * Configure the Sch and Egq shapers.
     */
    SHR_IF_ERR_EXIT(e2e_shapers_rates_set(unit, dest_port1, rate));
    SHR_IF_ERR_EXIT(e2e_shapers_rates_set(unit, dest_port2, rate));
    SHR_IF_ERR_EXIT(egress_shapers_rates_set(unit, dest_port1, rate));
    SHR_IF_ERR_EXIT(egress_shapers_rates_set(unit, dest_port2, rate));

    /*
     * Create packet and send traffic
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet1_h[0]));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet1_h[0], "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[0], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet1_h[0], "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[0], "ETH1.VLAN.VID", &vid_dest, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[0], "ETH1.VLAN.PCP", &pcp1, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[0], "ETH1.VLAN.DEI", &dei, 1));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet1_h[0], "ETH1.SA", mac_str));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 dst_mac_addr[0], dst_mac_addr[1], dst_mac_addr[2], dst_mac_addr[3], dst_mac_addr[4], dst_mac_addr[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet1_h[0], "ETH1.DA", mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core_id_src, packet1_h[0], packet_size, stream_duration,
                                                   type, stream_rate, stream_burst, stream_granularity));

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet1_h[1]));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet1_h[1], "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[1], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet1_h[1], "ETH1"));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet1_h[1], "ETH1.SA", mac_str));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 dst_mac_addr2[0], dst_mac_addr2[1], dst_mac_addr2[2], dst_mac_addr2[3], dst_mac_addr2[4],
                 dst_mac_addr2[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet1_h[1], "ETH1.DA", mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[1], "ETH1.VLAN.VID", &vid_dest, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[1], "ETH1.VLAN.PCP", &pcp2, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet1_h[1], "ETH1.VLAN.DEI", &dei, 1));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create(unit, core_id_src, packet1_h[1], packet_size, stream_duration,
                                                   type, stream_rate, stream_burst, stream_granularity));

    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet1_h, 2));

    sal_sleep(2);

    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet1_h[0], &bytes_count, &stream1_tx_packet_count));

    LOG_CLI(("----FIRST STREAM---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream1_tx_packet_count),
             COMPILER_64_LO(stream1_tx_packet_count)));

    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet1_h[1], &bytes_count, &stream2_tx_packet_count));
    LOG_CLI(("----SECOND STREAM---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream2_tx_packet_count),
             COMPILER_64_LO(stream2_tx_packet_count)));

    SHR_IF_ERR_EXIT(bcm_stat_get(unit, dest_port1, snmpEtherStatsTXNoErrors, &stream1_rx_packet_count));
    LOG_CLI(("----FIRST PORT---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream1_rx_packet_count),
             COMPILER_64_LO(stream1_rx_packet_count)));

    SHR_IF_ERR_EXIT(bcm_stat_get(unit, dest_port2, snmpEtherStatsTXNoErrors, &stream2_rx_packet_count));
    LOG_CLI(("----SECOND PORT---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream2_rx_packet_count),
             COMPILER_64_LO(stream2_rx_packet_count)));

    /*
     * Check if the Priority propagation mode works as expected.
     * When it is disabled check if we have not more than 2% difference in the ratios of the both ports.
     * When it is enabled check if all the high priority traffic pass on High priority port and
     * on low priority we have under 1% low priority traffic.
     */

    if (!priority_en)
    {
        COMPILER_64_COPY(temp_packet_count, stream1_rx_packet_count);
        COMPILER_64_UMUL_32(temp_packet_count, 100);
        COMPILER_64_UDIV_64(temp_packet_count, stream1_tx_packet_count);
        percentage1 = COMPILER_64_LO(temp_packet_count);

        COMPILER_64_COPY(temp_packet_count, stream2_rx_packet_count);
        COMPILER_64_UMUL_32(temp_packet_count, 100);
        COMPILER_64_UDIV_64(temp_packet_count, stream2_tx_packet_count);
        percentage2 = COMPILER_64_LO(temp_packet_count);
        if (utilex_abs(percentage1 - percentage2) > 3)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic is not as expected! Percentages: port %d is %d, port %d is %d \n",
                         dest_port1, percentage1, dest_port2, percentage2);
        }
    }
    else
    {
        COMPILER_64_COPY(temp_packet_count, stream1_rx_packet_count);
        COMPILER_64_UMUL_32(temp_packet_count, 100);
        COMPILER_64_UDIV_64(temp_packet_count, stream1_tx_packet_count);
        percentage1 = COMPILER_64_LO(temp_packet_count);
        if (percentage1 != 100)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic on port %d is not as expected!Expected is 100 percent \n", dest_port1);
        }
        COMPILER_64_COPY(temp_packet_count, stream2_rx_packet_count);
        COMPILER_64_UMUL_32(temp_packet_count, 100);
        COMPILER_64_UDIV_64(temp_packet_count, stream2_tx_packet_count);
        percentage2 = COMPILER_64_LO(temp_packet_count);
        if ((0 > percentage2) || (percentage2 >= 10))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic on port %d is not as expected!Expected is between 0 and 10 percent\n",
                         dest_port2);
        }
    }

    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE));
    LOG_CLI((BSL_META("Done. \n")));

exit:
#if !defined(ADAPTER_SERVER_MODE)
    diag_sand_packet_free(unit, packet1_h[0]);
    diag_sand_packet_free(unit, packet1_h[1]);
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
#endif
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_tm_priority_propagation_cmds[] = {
    {"basic", dnx_tm_priority_basic_test_cmd, NULL, sh_dnx_tm_priority_basic_test_options,
     &sh_dnx_tm_priority_basic_test_man, NULL,
     sh_dnx_tm_priority_basic_tests, CTEST_PRECOMMIT | SH_CMD_CONDITIONAL, sh_cmd_port_priority_propagation_supported}
    ,
    {NULL}
};

/** \file ctests_dnx_priority_propagation_tests.c
 *
 * Tests for priority
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
/************
 * INCLUDES  *
 *************/
#include "ctest_dnx_priority_propagation_tests.h"

#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <src/bcm/dnx/cosq/scheduler/scheduler_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>

#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/sat/sat.h>
#include <bcm/cosq.h>

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
#define CTEST_DNX_EGQ_PDS_DROP_THRESHOLD_VALUE 1000
#define CTEST_DNX_STOP_INTERVAL_COUNT_VALUE 2500
/************
* FUNCTIONS *
*************/

static sh_sand_enum_t sh_enum_table_tc_types[] = {
    {"high", 0, "Set the stream to high priority traffic"},
    {"low", 4, "Set the stream to low priority traffic"},
    {NULL}
};

sh_sand_man_t sh_dnx_tm_priority_basic_test_man = {
    "Tests for Port Priority Propagation",
    "Test adding two ports with Priority propagation with different priority and check if the traffic goes to port with High priority\n",
    "ctest tm scheduler priority_propagation basic\n",
    "enable=0 priority=2"
};

sh_sand_man_t sh_dnx_tm_priority_fc_test_man = {
    "Flow control test for Port Priority Propagation",
    "Test if we support mapping high / low egress interface flow control to sch interface.\n",
    "ctest tm scheduler priority_propagation flow_control",
    "priority=low flow_control=1"
};

/**
 * \brief
 *   Options list for 'priority' shell command
 */
/* *INDENT-OFF* */
sh_sand_option_t sh_dnx_tm_priority_basic_test_options[] = {
    /* Name         Type                  Description                                 Default */
    {"enable",      SAL_FIELD_TYPE_BOOL,  "Port priority propagation enable/disable", "0"},
    {"priority",    SAL_FIELD_TYPE_INT32, "Number of priorities   2/4/8",             "2"},
    {NULL}      /* End of options list - must be last. */
};

sh_sand_option_t sh_dnx_tm_priority_fc_test_options[] = {
    /* Name           Type                  Description                                 Default */
    {"priority",      SAL_FIELD_TYPE_ENUM,  "Set the stream to high/low priority",      "high", sh_enum_table_tc_types},
    {"fc_enable",     SAL_FIELD_TYPE_BOOL,  "flow control enable (0,1)",     "0"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */

/**
 * \brief
 *   List of tests for 'priority' shell command (to be run on
 *   regression, precommit, etc.)
 */
sh_sand_invoke_t sh_dnx_tm_priority_basic_tests[] = {
    {"priority_propagation_disabled_num_priorities_2", "enable=0 priority=2", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_disabled_num_priorities_4", "enable=0 priority=4", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_disabled_num_priorities_8", "enable=0 priority=8", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_enabled_num_priorities_2", "enable=1 priority=2", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_enabled_num_priorities_4", "enable=1 priority=4", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_enabled_num_priorities_8", "enable=1 priority=8", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

sh_sand_invoke_t sh_dnx_tm_priority_fc_tests[] = {
    {"priority_propagation_fc_disabled_priority_high", "Priority=high fc_enable=0", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_fc_disabled_priority_low", "Priority=low fc_enable=0", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_fc_enabled_priority_high", "Priority=high fc_enable=1", CTEST_POSTCOMMIT}
    ,
    {"priority_propagation_fc_enabled_priority_low", "Priority=low fc_enable=1", CTEST_POSTCOMMIT}
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
get_interface_gport(
    int unit,
    int egress_port,
    bcm_gport_t * handle)
{
    bcm_cosq_gport_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    gport_info.in_gport = egress_port;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeLocalPort, &gport_info));
    SHR_IF_ERR_EXIT(bcm_fabric_port_get(unit, gport_info.out_gport, 0, handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tm_priority_basic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t src_port = 1, dest_port1 = 160, dest_port2 = 161;
    bcm_port_t dest_eth1 = 1, dest_eth2 = 2;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    bcm_core_t core_id_src = 0, core_id_dest;
    char ucode_dest_port1_name[RHNAME_MAX_SIZE], ucode_dest_port1_value[RHNAME_MAX_SIZE];
    char ucode_dest_port2_name[RHNAME_MAX_SIZE], ucode_dest_port2_value[RHNAME_MAX_SIZE];
    char enable[RHNAME_MAX_SIZE], number[RHNAME_MAX_SIZE];
    char ports_priority[2][RHNAME_MAX_SIZE];
    rhhandle_t ctest_soc_set_h = NULL;
    int priority_en = FALSE;
    uint32 vid_dest = 101;
    sal_mac_addr_t dst_mac_addr = { 00, 00, 00, 00, 00, 02 };
    sal_mac_addr_t dst_mac_addr2 = { 00, 00, 00, 00, 00, 03 };
    bcm_pbmp_t p, u;
    bcm_l2_addr_t l2addr;
    int num_priorities;
    rhhandle_t packet1_h[2] = { NULL };
    int stream_rate = 100000000;
    int rate = 100000000;
    int packet_size = 128;
    int stream_duration = 1;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 1000;
    int stream_granularity = -1;
    int compensation = 24;
    uint64 stream1_rx_packet_count, stream2_rx_packet_count, bytes_count, stream1_tx_packet_count,
        stream2_tx_packet_count;
    sal_mac_addr_t src_mac_addr = { 00, 00, 00, 00, 00, 01 };
    char mac_str[RHNAME_MAX_SIZE];
    uint32 pcp1 = 0, pcp2 = 4, dei = 0;
#if !defined(ADAPTER_SERVER_MODE)
    uint64 temp_packet_count;
    int percentage1, percentage2;
#endif

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    if ((dnx_data_sat.general.feature_get(unit, dnx_data_sat_general_is_sat_supported) == 0) ||
        (dnx_data_device.general.feature_get(unit, dnx_data_device_general_hw_support) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Priority propagation traffic tests are not available over adapter.\n");
    }

    /*
     * Get input from shell
     */
    SH_SAND_GET_BOOL("enable", priority_en);
    SH_SAND_GET_INT32("priority", num_priorities);

    /*
     * Add soc property for new ports
     */
    core_id_dest = 0;
    sal_snprintf(enable, RHNAME_MAX_SIZE, "%d", priority_en);
    sal_snprintf(number, RHNAME_MAX_SIZE, "%d", num_priorities);
    sal_snprintf(ports_priority[0], RHNAME_MAX_SIZE, "port_priorities_%d", dest_port1);
    sal_snprintf(ports_priority[1], RHNAME_MAX_SIZE, "port_priorities_%d", dest_port2);
    sal_snprintf(ucode_dest_port1_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port1);
    sal_snprintf(ucode_dest_port1_value, RHNAME_MAX_SIZE, "RCY1.%d:core_%d.%d", dest_port1, core_id_dest, dest_port1);
    sal_snprintf(ucode_dest_port2_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port2);
    sal_snprintf(ucode_dest_port2_value, RHNAME_MAX_SIZE, "RCY1.%d:core_%d.%d", dest_port2, core_id_dest, dest_port2);
    if (priority_en)
    {
        sal_strncat_s(ucode_dest_port1_value, ":sch_priority_propagation", sizeof(ucode_dest_port1_value));
        sal_strncat_s(ucode_dest_port2_value, ":sch_priority_propagation", sizeof(ucode_dest_port2_value));
    }

    {
/* *INDENT-OFF* */
        ctest_soc_property_t ctest_soc_property[] = {
            {"ucode_port_1.*", NULL},
            {"ucode_port_5.*", NULL},
            {"ucode_port_6.*", NULL},
            {"ucode_port_13.*", NULL},
            {"ucode_port_14.*", NULL},
            {"ucode_port_15.*", NULL},
            {"ucode_port_16.*", NULL},
            {"ucode_port_17.*", NULL},
            {"ucode_port_221.*", NULL},
            {"ucode_port_222.*", NULL},
            {"port_init_speed_rcy", "100000"},
            {"port_sch_priority_propagation_enable", enable},
            {ucode_dest_port1_name, ucode_dest_port1_value},
            {ucode_dest_port2_name, ucode_dest_port2_value},
            {ports_priority[0], number},
            {ports_priority[1], number},
            {"ucode_port_1", "CDGE0:core_0.1"},
            {"ucode_port_2", "CDGE1:core_0.2"},
            {NULL}
        };
/* *INDENT-ON* */
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

    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));

    /*
     * Forward the traffic returned from RCY ports to the ETH ports
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, dest_port1, dest_eth1, 1));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, dest_port2, dest_eth2, 1));

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
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_with_compensation_create
                    (unit, core_id_src, packet1_h[0], packet_size, stream_duration, type, stream_rate, stream_burst,
                     stream_granularity, compensation));

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
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_with_compensation_create
                    (unit, core_id_src, packet1_h[1], packet_size, stream_duration, type, stream_rate, stream_burst,
                     stream_granularity, compensation));

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

    SHR_IF_ERR_EXIT(bcm_stat_get(unit, dest_eth1, snmpEtherStatsTXNoErrors, &stream1_rx_packet_count));
    LOG_CLI(("----FIRST PORT---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream1_rx_packet_count),
             COMPILER_64_LO(stream1_rx_packet_count)));

    SHR_IF_ERR_EXIT(bcm_stat_get(unit, dest_eth2, snmpEtherStatsTXNoErrors, &stream2_rx_packet_count));
    LOG_CLI(("----SECOND PORT---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream2_rx_packet_count),
             COMPILER_64_LO(stream2_rx_packet_count)));

    /*
     * Check if the Priority propagation mode works as expected.
     * When it is disabled check if we have not more than 8% difference in the ratios of the both ports.
     * When it is enabled check if all the high priority traffic pass on High priority port and
     * on low priority we have under 10% low priority traffic.
     */

#if !defined(ADAPTER_SERVER_MODE)
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
        if (utilex_abs(percentage1 - percentage2) > 8)
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
#endif

    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
    LOG_CLI((BSL_META("Done. \n")));

exit:
    diag_sand_packet_free(unit, packet1_h[0]);
    diag_sand_packet_free(unit, packet1_h[1]);
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/*
 * Test sequence:
 * 1. Disable all types of flow control except interface level.
 * 2. Set the EGQ interface level shaper into 1G and SAT stream into 10G.
 * 3. Set the thresholds for EGQ interface FC into low value./
 * 4. Limit the EGQ buffer size to 1000 PDs.
 * 5. Forward traffic from SAT port to destination port and check result.
 *
 * Test case 1: HP stream - FC disabled:
 * 1. Map scheduler interface #0 into LP FC
 * 2. Send 5000 number of packets with TC 0 to have congestion in EGQ
 * 3. FC should not be generated and egress will drop traffic. Expect packets drops.
 *
 * Test case 2: LP stream - FC disabled:
 * 1. Map scheduler interface #1 into HP FC
 * 2. Send 5000 number of packets with TC 4 to have congestion in EGQ
 * 3. FC should not be generated and egress will drop traffic. Expect packets drops.
 *
 * Test case 3: HP stream - FC enabled:
 * 1. Map scheduler interface #0 into HP FC
 * 2. Send 5000 number of packets with TC 0 to have congestion in EGQ
 * 3. FC should be generated and we don't expect packets drops.
 *
 * Test case 4: HP stream - FC enabled:
 * 1. Map scheduler interface #1 into LP FC
 * 2. Send 5000 number of packets with TC 4 to have congestion in EGQ
 * 3. FC should be generated and we don't expect packets drops.
 */
shr_error_e
sh_dnx_tm_priority_fc_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t src_port = 13, dest_port1 = 14;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags;
    bcm_core_t core_id_src, core_id_dest;
    int nif_interface_id_dest = 0;
    char ucode_dest_port1_name[RHNAME_MAX_SIZE], ucode_dest_port1_value[RHNAME_MAX_SIZE];
    rhhandle_t ctest_soc_set_h = NULL;
    uint32 vid_dest = 101, tm_dest1_port;
    rhhandle_t packet_h = NULL;
    int stream_rate = 10000000; /* Rate 10GB */
    int packet_size = 128, stream_duration = 1, stream_burst = 100000, stream_granularity = 0;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    uint64 bytes_count, stream_tx_packet_count, stream_rx_packet_count;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    sal_mac_addr_t src_mac_addr = { 00, 00, 00, 00, 00, 01 };
    sal_mac_addr_t dst1_mac_addr = { 00, 00, 00, 00, 00, 02 };
    char mac_str[RHNAME_MAX_SIZE];
    uint32 pcp, dei = 0;
    int flow_control = FALSE;
    int egress_if;
    bcm_cos_queue_t cosq = -1;
    bcm_cosq_threshold_t threshold;
    bcm_gport_t queue, sch_interface, core;
    bcm_sat_gtf_rate_pattern_t rate_pattern;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    SH_SAND_GET_ENUM("priority", pcp);
    SH_SAND_GET_BOOL("fc_enable", flow_control);

    if ((dnx_data_sat.general.feature_get(unit, dnx_data_sat_general_is_sat_supported) == 0) ||
        (dnx_data_device.general.feature_get(unit, dnx_data_device_general_hw_support) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "Priority propagation traffic tests are not available over adapter.\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, dest_port1, &nif_interface_id_dest));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, dest_port1, &flags, &interface_info, &mapping_info));
    core_id_dest = mapping_info.core;
    tm_dest1_port = mapping_info.tm_port;

    sal_snprintf(ucode_dest_port1_name, RHNAME_MAX_SIZE, "ucode_port_%d", dest_port1);
    sal_snprintf(ucode_dest_port1_value, RHNAME_MAX_SIZE, "XE%d.1:core_%d.%d:sch_priority_propagation",
                 nif_interface_id_dest, core_id_dest, tm_dest1_port);

    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"ucode_port_14*", NULL}
            ,
            {"port_sch_priority_propagation_enable", "1"}
            ,
            {ucode_dest_port1_name, ucode_dest_port1_value}
            ,
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    /*
     * Enable interface level FC only to avoid the effect of other FC types.
     */
    SHR_IF_ERR_EXIT(dnx_sch_fc_enablers_set(unit, core_id_dest, 0, 0, 0, 1, 0));

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, src_port, &core_id_src));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));

    bcm_l2_addr_t_init(&l2addr, dst1_mac_addr, vid_dest);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = dest_port1;
    SHR_IF_ERR_EXIT(bcm_l2_addr_add(unit, &l2addr));

    SHR_IF_ERR_EXIT(bcm_vlan_create(unit, vid_dest));
    BCM_PBMP_PORT_SET(p, src_port);
    BCM_PBMP_PORT_ADD(p, dest_port1);
    BCM_PBMP_CLEAR(u);
    SHR_IF_ERR_EXIT(bcm_vlan_port_add(unit, vid_dest, p, u));

    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid_dest, src_port, 0));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vid_dest, dest_port1, 0));

    sal_sleep(2);

    SHR_IF_ERR_EXIT(get_interface_gport(unit, dest_port1, &egress_if));
    /*
     * Reduce EGQ interface bandwidth to 1G to create congestion at egress.
     */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, egress_if, 0, 0, 1000000, 0));

    BCM_COSQ_GPORT_PORT_TC_SET(queue, dest_port1);
    /*
     * Set Q-Pair priority
     */
    SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, queue, 0, bcmCosqControlPrioritySelect, BCM_COSQ_SP0));
    SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, queue, 1, bcmCosqControlPrioritySelect, BCM_COSQ_SP1));

    /*
     * Connect FC signal to SCH interface
     */
    BCM_COSQ_GPORT_E2E_INTERFACE_SET(sch_interface, dest_port1);
    if ((flow_control == 0) && (pcp == 0))
    {
        sal_printf("Case HP stream  FC disabled \n");
        SHR_IF_ERR_EXIT(bcm_cosq_control_set
                        (unit, sch_interface, 0, bcmCosqControlPrioritySelect, BCM_COSQ_LOW_PRIORITY));
    }
    else if ((flow_control == 0) && (pcp == 4))
    {
        sal_printf("Case LP stream  FC disabled \n");
        SHR_IF_ERR_EXIT(bcm_cosq_control_set
                        (unit, sch_interface, 1, bcmCosqControlPrioritySelect, BCM_COSQ_HIGH_PRIORITY));
    }
    else if ((flow_control == 1) && (pcp == 0))
    {
        sal_printf("Case HP stream  FC enabled \n");
        SHR_IF_ERR_EXIT(bcm_cosq_control_set
                        (unit, sch_interface, 0, bcmCosqControlPrioritySelect, BCM_COSQ_HIGH_PRIORITY));
    }
    else if ((flow_control == 1) && (pcp == 4))
    {
        sal_printf("Case LP stream  FC  enabled \n");
        SHR_IF_ERR_EXIT(bcm_cosq_control_set
                        (unit, sch_interface, 1, bcmCosqControlPrioritySelect, BCM_COSQ_LOW_PRIORITY));
    }

    /*
     * Set EGQ interface level FC thresholds values to minimum.
     */
    threshold.priority = BCM_COSQ_LOW_PRIORITY;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;

    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = 1;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_set(unit, egress_if, cosq, &threshold));

    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = 1;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_set(unit, egress_if, cosq, &threshold));

    threshold.priority = BCM_COSQ_HIGH_PRIORITY;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_FLOW_CONTROL;

    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = 1;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_set(unit, egress_if, cosq, &threshold));

    threshold.type = bcmCosqThresholdDataBuffers;
    threshold.value = 1;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_set(unit, egress_if, cosq, &threshold));

    /*
     * Set EGQ drop thresholds.
     */
    BCM_COSQ_GPORT_CORE_SET(core, BCM_CORE_ALL);

    threshold.priority = BCM_COSQ_LOW_PRIORITY;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_DROP;

    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = CTEST_DNX_EGQ_PDS_DROP_THRESHOLD_VALUE;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_set(unit, core, cosq, &threshold));

    BCM_COSQ_GPORT_CORE_SET(core, BCM_CORE_ALL);

    threshold.priority = BCM_COSQ_HIGH_PRIORITY;
    threshold.flags = BCM_COSQ_THRESHOLD_EGRESS | BCM_COSQ_THRESHOLD_UNICAST | BCM_COSQ_THRESHOLD_DROP;

    threshold.type = bcmCosqThresholdPacketDescriptors;
    threshold.value = CTEST_DNX_EGQ_PDS_DROP_THRESHOLD_VALUE;
    SHR_IF_ERR_EXIT(bcm_cosq_gport_threshold_set(unit, core, cosq, &threshold));

    /*
     * Configure the rate pattern
     */

    rate_pattern.rate_pattern_mode = bcmSatGtfRatePatternCombined;
    rate_pattern.high_threshold = 5000;
    rate_pattern.low_threshold = 1;
    rate_pattern.stop_iter_count = 1;
    rate_pattern.stop_burst_count = 0;
    rate_pattern.stop_interval_count = CTEST_DNX_STOP_INTERVAL_COUNT_VALUE;
    rate_pattern.burst_packet_weight = 1;
    rate_pattern.interval_packet_weight = 1;
    rate_pattern.flags = 1;

    /*
     * Create packet and send traffic
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vid_dest, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.DEI", &dei, 1));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 src_mac_addr[0], src_mac_addr[1], src_mac_addr[2], src_mac_addr[3], src_mac_addr[4], src_mac_addr[5]);

    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", mac_str));
    sal_snprintf(mac_str, RHNAME_MAX_SIZE - 1, "%02x:%02x:%02x:%02x:%02x:%02x",
                 dst1_mac_addr[0], dst1_mac_addr[1], dst1_mac_addr[2], dst1_mac_addr[3], dst1_mac_addr[4],
                 dst1_mac_addr[5]);
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_pattern_create(unit, core_id_src, packet_h, packet_size, stream_duration,
                                                           type, stream_rate, rate_pattern, stream_burst,
                                                           stream_granularity));

    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));

    sal_sleep(1);

    SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &stream_tx_packet_count));

    LOG_CLI(("----STREAM STATS---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream_tx_packet_count),
             COMPILER_64_LO(stream_tx_packet_count)));

    SHR_IF_ERR_EXIT(bcm_stat_get(unit, dest_port1, snmpEtherStatsTXNoErrors, &stream_rx_packet_count));
    LOG_CLI(("----PORT STATS---- \n"));
    LOG_CLI(("Packet counts 0x%x,0x%x \n", COMPILER_64_HI(stream_rx_packet_count),
             COMPILER_64_LO(stream_rx_packet_count)));

#if !defined(ADAPTER_SERVER_MODE)
    if (flow_control)
    {
        if (COMPILER_64_LO(stream_tx_packet_count) != COMPILER_64_LO(stream_rx_packet_count))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic on port %d is not as expected!Expected no drops\n", dest_port1);
        }
    }
    else
    {
        if (COMPILER_64_LO(stream_tx_packet_count) == COMPILER_64_LO(stream_rx_packet_count))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic on port %d is not as expected!Expected packet drops\n", dest_port1);
        }
    }
#endif
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));
    LOG_CLI((BSL_META("Done. \n")));
exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_tm_priority_propagation_cmds[] = {
    {"basic", dnx_tm_priority_basic_test_cmd, NULL, sh_dnx_tm_priority_basic_test_options,
     &sh_dnx_tm_priority_basic_test_man, NULL,
     sh_dnx_tm_priority_basic_tests, CTEST_PRECOMMIT, NULL}
    ,
    {"flow_control", sh_dnx_tm_priority_fc_test_cmd, NULL, sh_dnx_tm_priority_fc_test_options,
     &sh_dnx_tm_priority_fc_test_man, NULL,
     sh_dnx_tm_priority_fc_tests, CTEST_PRECOMMIT, NULL}
    ,
    {NULL}
};

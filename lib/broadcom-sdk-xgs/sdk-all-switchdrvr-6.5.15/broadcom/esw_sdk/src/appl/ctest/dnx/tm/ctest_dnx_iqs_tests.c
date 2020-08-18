/** \file diag_dnx_iqs_tests.c
 * 
 * Tests for IQS (Ingress Queue Scheduler)
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include <appl/reference/sand/appl_ref_sand.h>

#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <bcm/cosq.h>
#include <bcm/types.h>

/*
 * \brief - send 'count' packets from src_port to dst_port using force-forward forwarding.
 */
static shr_error_e
sh_dnx_iqs_send_packet(
    int unit,
    bcm_port_t src_port,
    bcm_port_t dst_port,
    int count)
{
    rhhandle_t packet_h = NULL;
    int i_pkt;
    uint32 pcp;

    SHR_FUNC_INIT_VARS(unit);

    /** set forwarding */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, src_port, dst_port, 1));

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", (uint32 *) (&src_port), 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));

    pcp = 0;
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));

    /** send packet according to count */
    for (i_pkt = 0; i_pkt < count; i_pkt++)
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, TRUE));
    }

    /** unset forwarding */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, src_port, dst_port, 0));

exit:
    diag_sand_packet_free(unit, packet_h);
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set watchdog timer to the provided interval and verify packet is deleted once timer is expired:
 *   1. Extract credit class profile from port
 *   2. Set watch watchdog timer for the credit class profile
 *   3. Stop sending credits (set e2e port shaper to 0).
 *   4. Send packet to the same port.
 *   5. Verify packet is deleted once timer is expired
 *   6. Set back watchdog timer to default
 */
static shr_error_e
sh_dnx_iqs_watchdog_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 interval_sec = 0, interval_ms_def = 0, credit_class = 0;
    bcm_port_t port = 200;
    int base_voq, weight, iqs_profile, count, core, burst;
    bcm_gport_t voq_gport, sch_tc_gport;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    dnx_ingress_congestion_counter_info_t counter_info;

    SHR_FUNC_INIT_VARS(unit);

#if defined(ADAPTER_SERVER_MODE)
    SHR_ERR_EXIT(_SHR_E_NONE, "Watchdog test is not available over cmodel.\n");
#endif

    SH_SAND_GET_UINT32("interval", interval_sec);

    /** get voq assuming dnx default application (system port = logical port) */
    SHR_IF_ERR_EXIT(appl_sand_sysport_id_to_base_voq_id_convert(unit, port, &base_voq));

    /** get credit class proile from base_voq */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, base_voq);
    SHR_IF_ERR_EXIT(bcm_cosq_gport_sched_get(unit, voq_gport, 0, &iqs_profile, &weight));

    /** get credit class profile attributes */
    SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_get(unit, iqs_profile, &delay_tolerance));

    /** get core */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    /** set watchdog timer and stop scheduler from sending credits */
    {
        /** set watchdog timer (and save default timer) */
        LOG_CLI((BSL_META("Set credit_class (%d) watchdog timer to (%d) seconds\n"), credit_class, interval_sec));
        interval_ms_def = delay_tolerance.credit_request_watchdog_delete_queue_thresh;
        delay_tolerance.credit_request_watchdog_delete_queue_thresh = interval_sec * 1000;
        SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_set(unit, iqs_profile, &delay_tolerance));

        /** set port TC 0 scheduler burst to 0 (and save default rate) */
        BCM_COSQ_GPORT_E2E_PORT_TC_SET(sch_tc_gport, port);
        SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, sch_tc_gport, 0, bcmCosqControlBandwidthBurstMax, &burst));
        LOG_CLI((BSL_META("Set port %d tc 0 burst size to 0, original burst size is (%d) \n"), port, burst));
        SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, sch_tc_gport, 0, bcmCosqControlBandwidthBurstMax, 0));
    }

    /** send few packets to drain voq from its initial credit balance */
    {
        count = 100;
        LOG_CLI((BSL_META("Sending (%d) packets to drain voq initial credit balance\n"), count));
        SHR_IF_ERR_EXIT(sh_dnx_iqs_send_packet(unit, port, port, count));

        /** sleep for interval time to empty credits and all packets */
        LOG_CLI((BSL_META("Sleeping for (%d) seconds to empty voq\n"), interval_sec + 1));
        sal_sleep(interval_sec + 1);

        /** read counters to clear them */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));
    }

    /** send 1 packet and verify watchdog */
    count = 1;
    LOG_CLI((BSL_META("Sending (%d) packets\n"), count));
    SHR_IF_ERR_EXIT(sh_dnx_iqs_send_packet(unit, port, port, count));

    /** sleep for half of the interval */
    LOG_CLI((BSL_META("Sleeping for (%d) seconds to verify there are no deleted packets \n"), interval_sec / 2));
    sal_sleep(interval_sec / 2);

    /** verify delete counter is 0 */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));
    if (counter_info.sram_del_pkt_cnt != 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected 0 deleted packet but got %d.", counter_info.sram_del_pkt_cnt);
    }

    /** sleep the second half with 1 extra second */
    LOG_CLI((BSL_META("Sleeping for (%d) seconds to verify packets are deleted \n"), interval_sec / 2));
    sal_sleep(interval_sec / 2 + 1);

    /** verify there are deleted packets */
    SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_programmable_counter_info_get(unit, core, &counter_info));
    if (counter_info.sram_del_pkt_cnt != count)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected %d deleted packets but got %d.",
                     count, counter_info.sram_del_pkt_cnt);
    }

    /** restore defaults */
    {
        LOG_CLI((BSL_META("Restoring defaults\n")));

        /** delay tolerance level */
        delay_tolerance.credit_request_watchdog_delete_queue_thresh = interval_ms_def;
        SHR_IF_ERR_EXIT(bcm_cosq_delay_tolerance_level_set(unit, iqs_profile, &delay_tolerance));

        /** port scheduler shaper */
        SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, sch_tc_gport, 0, bcmCosqControlBandwidthBurstMax, burst));
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sh_dnx_iqs_watchdog_test_man = {
    "Test VoQs watchdog mechanism",
    "Set a certain watchdog interval. Send a packet, sleep for 'interval' time and verify packet was deleted",
    NULL,
    "interval=4",
};

static sh_sand_option_t sh_dnx_iqs_watchdog_test_options[] = {
    {"interval", SAL_FIELD_TYPE_UINT32, "Time in seconds", "4"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_ipq_alloc_tests[] = {
    {"interval_4_seconds", "interval=4"},
    {NULL}
};

sh_sand_cmd_t sh_dnx_tm_iqs_test_cmds[] = {
    {"WatchDog", sh_dnx_iqs_watchdog_cmd, NULL, sh_dnx_iqs_watchdog_test_options, &sh_dnx_iqs_watchdog_test_man, NULL,
     diag_dnx_ipq_alloc_tests, CTEST_POSTCOMMIT},
    {NULL}
};

/** \file diag_dnx_iqs_tests.c
 *
 * Tests for IQS (Ingress Queue Scheduler)
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

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>

#include <appl/reference/sand/appl_ref_sand.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>

#include <bcm/cosq.h>
#include <bcm/types.h>

extern shr_error_e sh_dnx_tm_queue_base_queue_get(
    int unit,
    int queue,
    bcm_core_t core,
    int *base_queue,
    int *num_cos);

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
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));

    /** set forwarding */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, src_port, dst_port, 1));

    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));

    pcp = 0;
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));

    /** send packet according to count */
    for (i_pkt = 0; i_pkt < count; i_pkt++)
    {
        SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_h, SAND_PACKET_RESUME));
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
    SHR_ERR_EXIT(_SHR_E_NONE, "Watchdog test is not available over adapter.\n");
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

/*
 * \brief
 *   Set flush timeout for a specific queue and make sure its empty afterwards
 *   1. Stop sending credits to a specific queue (set e2e port shaper to 0).
 *   2. Send packet to the queue (packets should be stuck at the queue).
 *   3. Call flush API for the queue, with given timeout
 *   5. if the API returns error, return (queue was not emptied during the timeout time frame)
 *   6. if API returns no errors, make sure the queue is indeed empty
 */
static shr_error_e
sh_dnx_iqs_queue_flush_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 voq = 0, timeout = 0, base_queue_quartet = 0, master_sysport = 0;
    bcm_gport_t voq_gport, sch_tc_gport;
    bcm_port_t port;
    int min_bundle_size = dnx_data_ipq.queues.min_bundle_size_get(unit);
    int rv, count, core, burst, num_cos, base_voq, cos, max_mc_queue = -1, watchdog_enabled;
    dnx_ingress_congestion_voq_occupancy_info_t voq_occupancy_info;
    SHR_FUNC_INIT_VARS(unit);

#if defined(ADAPTER_SERVER_MODE)
    SHR_ERR_EXIT(_SHR_E_NONE, "Queue Flush test is not available over adapter.\n");
#endif

    SH_SAND_GET_UINT32("voq", voq);
    SH_SAND_GET_UINT32("timeout", timeout);

    /** Turn on the watchdog so that packets will remain in the queue*/
    SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricWatchdogQueueEnable, &watchdog_enabled));
    SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, BCM_FABRIC_WATCHDOG_QUEUE_DISABLE));

    /** Get base queue */
    core = DNXCMN_CORE_ID2INDEX(unit, BCM_CORE_ALL);
    SHR_IF_ERR_EXIT(sh_dnx_tm_queue_base_queue_get(unit, voq, core, &base_voq, &num_cos));
    if (base_voq < 0)
    {
        /** Queue is invalid  */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue %d provided", voq);
    }
    /** Create Voq Gport */
    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricMulticastQueueMax, &max_mc_queue));
    }
    if (voq >= 0 && voq <= max_mc_queue)
    {
        /** Multicast */
        BCM_GPORT_MCAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, BCM_CORE_ALL, base_voq);
    }
    else if (voq >= 0 && voq < dnx_data_ipq.queues.nof_queues_get(unit))
    {
        /** Unicast */
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(voq_gport, BCM_CORE_ALL, base_voq);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid queue %d provided", voq);
    }

    /** Get port number from base queue */
    base_queue_quartet = base_voq / min_bundle_size;
    SHR_IF_ERR_EXIT(dnx_cosq_ipq_dbal_queue_quartet_to_sys_port_map_get
                    (unit, BCM_CORE_ALL, base_queue_quartet, &master_sysport));

    /** Assuming default application: system port = logical port*/
    port = master_sysport;
    /** Set port TC scheduler burst to 0 (and save default rate) */
    cos = voq - base_voq;
    BCM_COSQ_GPORT_E2E_PORT_TC_SET(sch_tc_gport, port);
    SHR_IF_ERR_EXIT(bcm_cosq_control_get(unit, sch_tc_gport, 0, bcmCosqControlBandwidthBurstMax, &burst));
    LOG_CLI((BSL_META("Set port %d tc 0 burst size to 0, original burst size is (%d) \n"), port, burst));
    SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, sch_tc_gport, 0, bcmCosqControlBandwidthBurstMax, 0));

    /** Send 10 packets */
    count = 10;
    LOG_CLI((BSL_META("Sending (%d) packets\n"), count));
    SHR_IF_ERR_EXIT(sh_dnx_iqs_send_packet(unit, port, port, count));

    /** Call Flush API */
    rv = bcm_cosq_control_set(unit, voq_gport, cos, bcmCosqControlFlush, timeout);
    if (timeout == 0)
    {
        /** timeout 0 - not waiting to check queue status, but make sure the API returned no error */
        SHR_IF_ERR_EXIT(rv);
    }
    else if (rv != _SHR_E_NONE)
    {
        /** timeout != 0, and API returned error, it might mean the timeout was not enough to flush the queue */
        SHR_CLI_EXIT(_SHR_E_NONE, "Flush API returned status %d, try to increase the timeout\n", rv);
    }
    else
    {
        /** Get port core */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        /** Validate the queue is indeed empty */
        SHR_IF_ERR_EXIT(dnx_ingress_congestion_voq_occupancy_info_get(unit, core, voq, &voq_occupancy_info));
        if (voq_occupancy_info.size > 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Queue %d expected to be empty but got size of %d", voq, voq_occupancy_info.size);
        }
    }

    /** restore defaults */
    LOG_CLI((BSL_META("Restoring defaults\n")));
    /** port scheduler shaper */
    SHR_IF_ERR_EXIT(bcm_cosq_control_set(unit, sch_tc_gport, 0, bcmCosqControlBandwidthBurstMax, burst));
    /** watchdog enable */
    SHR_IF_ERR_EXIT(bcm_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, watchdog_enabled));

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sh_dnx_iqs_watchdog_test_man = {
    "Test VoQs watchdog mechanism",
    "Set a certain watchdog interval. Send a packet, sleep for 'interval' time and verify packet was deleted",
    NULL,
    "interval=10",
};

static sh_sand_option_t sh_dnx_iqs_watchdog_test_options[] = {
    {"interval", SAL_FIELD_TYPE_UINT32, "Time in seconds", "10"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_iqs_watchdog_tests[] = {
    {"interval_4_seconds", "interval=10"},
    {NULL}
};

static sh_sand_man_t sh_dnx_iqs_queue_flush_test_man = {
    "Test VOQs flush mechanism",
    "set a queue to flush with timeout. make sure the API returns no errors, and that the queue is empty afterwards",
    NULL,
    "timeout=10",
};

static sh_sand_option_t sh_dnx_iqs_queue_flush_test_options[] = {
    {"voq", SAL_FIELD_TYPE_UINT32, "VOQ to Flush", "144"},
    {"timeout", SAL_FIELD_TYPE_UINT32, "Timeout in microseconds", "10"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_queue_flush_tests[] = {
    {"timuout_40_microseconds", "timeout=100"},
    {NULL}
};

sh_sand_cmd_t sh_dnx_tm_iqs_test_cmds[] = {
    {"WatchDog", sh_dnx_iqs_watchdog_cmd, NULL, sh_dnx_iqs_watchdog_test_options, &sh_dnx_iqs_watchdog_test_man, NULL,
     diag_dnx_iqs_watchdog_tests, CTEST_POSTCOMMIT},
    {"QueueFlush", sh_dnx_iqs_queue_flush_cmd, NULL, sh_dnx_iqs_queue_flush_test_options,
     &sh_dnx_iqs_queue_flush_test_man, NULL,
     diag_dnx_queue_flush_tests, CTEST_POSTCOMMIT},
    {NULL}
};

/** \file diag_dnx_ipq_tests.c
 *
 * Tests for IPQ
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

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <bcm/fabric.h>
#include <bcm/stat.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/algo/ipq/ipq_alloc_mngr.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_ipq_alloc_mngr_types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <sal/core/thread.h>

static shr_error_e
diag_dnx_ipq_alloc_nof_queue_bundles_get(
    int unit,
    dnx_ipq_queue_type_e queue_type,
    int num_cos,
    int *nof_queue_bundles)
{
    int nof_queues = 0;

    uint32 invalid_queue = dnx_data_ipq.queues.invalid_queue_get(unit);
    int invalid_queue_is_in_range;
    SHR_FUNC_INIT_VARS(unit);

    *nof_queue_bundles = 0;
    nof_queues = DNX_IPQ_NOF_QUEUES(unit);

    invalid_queue_is_in_range = (invalid_queue < nof_queues) ? TRUE : FALSE;
    if (invalid_queue_is_in_range)
    {
        /*
         * if the invalid queue is is within the queue range,
         * we want to make sure not to try to allocate this queue.
         */
        --nof_queues;
    }

    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST_INTERDIGITATED)
    {
        /*
         * Interdigitated cause queues bundle to be doubled, because
         * only half of the queues in the bundle are active, so it takes
         * 2 bundles instead of 1.
         * In addition, the first region can't be interdigitated.
         */
        nof_queues -= DNX_IPQ_NOF_QUEUES_IN_REGION(unit);
        *nof_queue_bundles = (nof_queues / num_cos) / 2;
    }
    else if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST)
    {
        *nof_queue_bundles = (dnx_data_ipq.queues.max_fmq_id_get(unit) + 1) / num_cos;
    }
    else if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST)
    {
        /** remove place reserved for MC queues when calculating nof UC queues */
        int max_mc_queue;
        int nof_mc_queues;
        SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.max_multicast_queue.get(unit, &max_mc_queue));
        nof_mc_queues = max_mc_queue + 1;
        *nof_queue_bundles = (nof_queues / num_cos) - UTILEX_DIV_ROUND_UP(nof_mc_queues, num_cos);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Test for allocating all queues.
 *
 *   Test walkthrough:
 *   1) Allocate all queues.
 *   2) Allocate 1 more queue. Expect to fail.
 *   3) Deallocate every other queue bundle.
 *   4) Allocate the other half of queue bundles - make sure no queue was allocated twice
 *   5) Deallocate all queue bundles
 *   6) Allocate all queue bundles again - to make sure everything was deallocated
 *   7) Cleanup - Deallocate all queue bundles
 *
 * Note:
 *   Test depends on queue bundle size and type.
 */
static shr_error_e
diag_dnx_ipq_alloc_all_test_cmd(
    int unit,
    dnx_ipq_queue_type_e queue_type,
    int num_cos)
{
    shr_error_e rv = _SHR_E_NONE;
    uint32 flags = 0;
    int nof_queues = 0, base_queue = 0, nof_queue_bundles = 0;
    int bundle_idx = 0, nof_expected_deallocated_queues = 0;
    int *queue_is_allocated = NULL, *allocated_base_queues = NULL;
    SHR_FUNC_INIT_VARS(unit);

    nof_queues = DNX_IPQ_NOF_QUEUES(unit);

    SHR_IF_ERR_EXIT(diag_dnx_ipq_alloc_nof_queue_bundles_get(unit, queue_type, num_cos, &nof_queue_bundles));

    SHR_ALLOC_SET_ZERO(queue_is_allocated, nof_queues * sizeof(int),
                       "queue is allocated", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(allocated_base_queues, nof_queue_bundles * sizeof(int), "base queues", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(allocated_base_queues, -1, nof_queue_bundles * sizeof(int));

    /*
     * 1. Allocate all queue bundles
     */
    LOG_CLI((BSL_META("Allocate all queue bundles (%d)\n"), nof_queue_bundles));
    for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
    {
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_allocate(unit, BCM_CORE_ALL, flags, queue_type, num_cos, &base_queue),
                            "Test FAIL. First Alloc: Failed to allocate the %d queue bundle\n", bundle_idx);
        allocated_base_queues[bundle_idx] = base_queue;
        queue_is_allocated[base_queue] = TRUE;
    }

    /*
     * 2. Try allocate a queue after all queues were allocated - should fail.
     */
    LOG_CLI((BSL_META("Try allocating one more queue bundle (should fail)\n")));
    rv = dnx_cosq_ipq_queue_allocate(unit, BCM_CORE_ALL, flags, queue_type, num_cos, &base_queue);
    if (SHR_SUCCESS(rv))
    {
        SHR_CLI_EXIT(rv, "Test FAIL. Allocated the %d queue bundle, which is more than expected.", bundle_idx);
    }

    /*
     * 3. Deallocate every other queue bundle
     */
    LOG_CLI((BSL_META("Deallocate every other queue bundles\n")));
    for (bundle_idx = 0; bundle_idx < nof_queue_bundles; bundle_idx += 2)
    {
        base_queue = allocated_base_queues[bundle_idx];
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_deallocate(unit, BCM_CORE_ALL, base_queue),
                            "Test FAIL. First Dealloc: Failed to deallocate queue bundle %d\n", base_queue);
        queue_is_allocated[base_queue] = FALSE;
    }

    /*
     * 4. Allocate all queue bundles left
     */
    nof_expected_deallocated_queues = nof_queue_bundles / 2 + nof_queue_bundles % 2;
    LOG_CLI((BSL_META("Allocate all queue bundles left. should have (%d) queues left\n"),
             nof_expected_deallocated_queues));
    for (bundle_idx = 0; bundle_idx < nof_expected_deallocated_queues; ++bundle_idx)
    {
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_allocate(unit, BCM_CORE_ALL, flags, queue_type, num_cos, &base_queue),
                            "Test FAIL. Second Alloc: Failed to allocate the %d queue bundle\n", bundle_idx);
        if (queue_is_allocated[base_queue])
        {
            SHR_CLI_EXIT(rv, "Test FAIL. Base queue %d was allocated twice!", base_queue);
        }
    }

    /*
     * 5. Deallocate all queue bundles
     */
    LOG_CLI((BSL_META("Deallocate all queue bundles\n")));
    for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
    {
        base_queue = allocated_base_queues[bundle_idx];
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_deallocate(unit, BCM_CORE_ALL, base_queue),
                            "Test FAIL. Second Dealloc: Failed to deallocate queue bundle %d\n", base_queue);
        queue_is_allocated[base_queue] = FALSE;
    }

    /*
     * 6. Allocate all queue bundles again, backwards - to make sure all queues were deallocated
     */
    LOG_CLI((BSL_META("Allocate all queue bundles (%d)\n"), nof_queue_bundles));
    for (bundle_idx = nof_queue_bundles - 1; bundle_idx >= 0; --bundle_idx)
    {
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_allocate(unit, BCM_CORE_ALL, flags, queue_type, num_cos, &base_queue),
                            "Test FAIL. Last Alloc: Failed to allocate the %d queue bundle\n",
                            nof_queue_bundles - 1 - bundle_idx);
        allocated_base_queues[bundle_idx] = base_queue;
        queue_is_allocated[base_queue] = TRUE;
    }

    /*
     * 7. Finally - Deallocate all queue bundles
     */
    LOG_CLI((BSL_META("Finally, Deallocate all allocated queue bundles\n")));
    for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
    {
        base_queue = allocated_base_queues[bundle_idx];
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_deallocate(unit, BCM_CORE_ALL, base_queue),
                            "Test FAIL. Last Dealloc: Failed to deallocate queue bundle %d\n", base_queue);
        queue_is_allocated[base_queue] = FALSE;
    }
exit:
    SHR_FREE(queue_is_allocated);
    SHR_FREE(allocated_base_queues);
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Test for allocating all queues WITH_ID.
 *
 *   Test walkthrough:
 *   1) Allocate all queues in order to save all allocated base queues.
 *   2) Deallocate all queues before calling WITH_ID.
 *   3) Allocate all queues WITH_ID.
 *   4) Deallocate all queues WITH_ID.
 *   5) Repeat steps 3-4.
 *
 * Note:
 *   Test depends on queue bundle size and type.
 */
static shr_error_e
diag_dnx_ipq_alloc_all_with_id_test_cmd(
    int unit,
    dnx_ipq_queue_type_e queue_type,
    int num_cos)
{
    uint32 flags = 0;
    int base_queue = 0, nof_queue_bundles = 0;
    int bundle_idx = 0, iter = 0;
    int *allocated_base_queues = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_dnx_ipq_alloc_nof_queue_bundles_get(unit, queue_type, num_cos, &nof_queue_bundles));

    SHR_ALLOC(allocated_base_queues, nof_queue_bundles * sizeof(int), "base queues", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(allocated_base_queues, -1, nof_queue_bundles * sizeof(int));

    /*
     * 1. Allocate all queue bundles without ID - just to get the base queues.
     */
    LOG_CLI((BSL_META("Allocate all queue bundles (%d)\n"), nof_queue_bundles));
    for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
    {
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_allocate(unit, BCM_CORE_ALL, flags, queue_type, num_cos, &base_queue),
                            "Test FAIL. Failed to allocate the %d queue bundle\n", bundle_idx);
        allocated_base_queues[bundle_idx] = base_queue;
    }

    /*
     * 2. Deallocate all queue bundles before starting to allocate WITH_ID
     */
    LOG_CLI((BSL_META("Deallocate all allocated queue bundles\n")));
    for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
    {
        base_queue = allocated_base_queues[bundle_idx];
        SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_deallocate(unit, BCM_CORE_ALL, base_queue),
                            "Test FAIL. Failed to deallocate queue bundle %d\n", base_queue);
    }

    for (iter = 0; iter < 2; ++iter)
    {
        /*
         * Allocate all queue bundles WITH_ID
         */
        LOG_CLI((BSL_META("Allocate all queue bundles WITH_ID\n")));
        for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
        {
            flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
            base_queue = allocated_base_queues[bundle_idx];
            SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_allocate
                                (unit, BCM_CORE_ALL, flags, queue_type, num_cos, &base_queue),
                                "Test FAIL. Failed to allocate the %d queue bundle\n", bundle_idx);
        }

        /*
         * Deallocate all queue bundles
         */
        LOG_CLI((BSL_META("Deallocate all allocated queue bundles\n")));
        for (bundle_idx = 0; bundle_idx < nof_queue_bundles; ++bundle_idx)
        {
            base_queue = allocated_base_queues[bundle_idx];
            SHR_CLI_EXIT_IF_ERR(dnx_cosq_ipq_queue_deallocate(unit, BCM_CORE_ALL, base_queue),
                                "Test FAIL. Failed to deallocate queue bundle %d\n", base_queue);
        }
    }
exit:
    SHR_FREE(allocated_base_queues);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_ipq_delete_voq_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    bcm_cosq_gport_connection_t connection;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) || BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        /** first disconnect voq from its connector */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.voq = gport;
        connection.remote_modid = 0;
        BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(connection.voq_connector, 0, 0);
        SHR_IF_ERR_EXIT(bcm_cosq_gport_connection_set(unit, &connection));

        /** delete the voq */
        SHR_IF_ERR_EXIT(bcm_cosq_gport_delete(unit, gport));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Call the right queue allocation test function according to 'with_id' param.
 */
static shr_error_e
diag_dnx_ipq_alloc_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int prev_sched_mode = 0, prev_max_mc_queue = 0;
    int with_id = 0, num_cos = 0;
    int max_fmq_id = 0;
    dnx_ipq_queue_type_e queue_type = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("number", num_cos);
    SH_SAND_GET_ENUM("type", queue_type);
    SH_SAND_GET_BOOL("with_id", with_id);

    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST &&
        !dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "FMQs are not supported on this device.\n");

    }

    max_fmq_id = dnx_data_ipq.queues.max_fmq_id_get(unit);

    /** Delete all existing VOQs */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_traverse(unit, diag_dnx_ipq_delete_voq_cb, 0));

    LOG_CLI((BSL_META("Test with num_cos = %d, type = %d, with_id = %d\n"), num_cos, queue_type, with_id));

    /*
     * if MC, set FMQ to enhanced scheduler mode
     */
    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST)
    {
        /*
         * Get previous configuration to restore at the end of the test.
         */
        SHR_CLI_EXIT_IF_ERR(bcm_fabric_control_get(unit, bcmFabricMulticastSchedulerMode, &prev_sched_mode),
                            "Test FAIL. Failed to get FMQ enhance scheduler mode.\n");
        SHR_CLI_EXIT_IF_ERR(bcm_fabric_control_get(unit, bcmFabricMulticastQueueMax, &prev_max_mc_queue),
                            "Test FAIL. Failed to get max multicast queue.\n");
        /*
         * Set all queues regions to be multicast regions.
         */
        SHR_CLI_EXIT_IF_ERR(bcm_fabric_control_set(unit, bcmFabricMulticastSchedulerMode, 1),
                            "Test FAIL. Failed to set FMQ enhance scheduler mode.\n");
        SHR_CLI_EXIT_IF_ERR(bcm_fabric_control_set(unit, bcmFabricMulticastQueueMax, max_fmq_id),
                            "Test FAIL. Failed to set max multicast queue.\n");
    }

    if (with_id)
    {
        SHR_IF_ERR_EXIT(diag_dnx_ipq_alloc_all_with_id_test_cmd(unit, queue_type, num_cos));
    }
    else
    {
        SHR_IF_ERR_EXIT(diag_dnx_ipq_alloc_all_test_cmd(unit, queue_type, num_cos));
    }

    /*
     * Restore previous settings.
     */
    if (queue_type == DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST)
    {
        SHR_CLI_EXIT_IF_ERR(bcm_fabric_control_set(unit, bcmFabricMulticastQueueMax, prev_max_mc_queue),
                            "Test FAIL. Failed to restore max multicast queue.\n");
        SHR_CLI_EXIT_IF_ERR(bcm_fabric_control_set(unit, bcmFabricMulticastSchedulerMode, prev_sched_mode),
                            "Test FAIL. Failed to restore FMQ enhance scheduler mode.\n");
    }
    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    ctest_dnxc_init_deinit(unit, NULL);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnx_ipq_send_packet(
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

    /** set forwarding */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, src_port, dst_port, 1));
    SHR_IF_ERR_EXIT(bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info));
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
 *   Call the right queue allocation test function according to 'with_id' param.
 */
static shr_error_e
diag_dnx_ipq_flow_based_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t system_port;
    bcm_gport_t sysport_gport;
    uint32 voq_id;
    uint64 recieved_packets = COMPILER_64_INIT(0, 0);
    int cosq, voq_gport = 0, nof_cosq = 8, cpu_port, sent_packets;
    bcm_cosq_ingress_queue_bundle_gport_config_t config;
    SHR_FUNC_INIT_VARS(unit);

#if defined(ADAPTER_SERVER_MODE)
    SHR_ERR_EXIT(_SHR_E_NONE, "Flow-based test is not available over adapter.\n");
#endif

    SH_SAND_GET_UINT32("port", system_port);
    SH_SAND_GET_UINT32("voq_id", voq_id);

    /*
     * Create a new flow based VOQ points to an existing system port
     * (assuming the system port already have a system-port-based VOQ pointing to it)
     */
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(voq_gport, voq_id);
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, system_port);
    config.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_WITH_ID | BCM_COSQ_GPORT_TM_FLOW_ID;
    config.port = sysport_gport;
    config.local_core_id = BCM_CORE_ALL;
    config.numq = nof_cosq;
    for (cosq = 0; cosq < config.numq; ++cosq)
    {
        config.queue_atrributes[cosq].delay_tolerance_level = 0;
        config.queue_atrributes[cosq].rate_class = 0;
    }
    SHR_IF_ERR_EXIT(bcm_cosq_ingress_queue_bundle_gport_add(unit, &config, &voq_gport));

    /*
     * Delete the recently created flow based VOQ
     */
    SHR_IF_ERR_EXIT(bcm_cosq_gport_delete(unit, voq_gport));

    /*
     * Send one packet from any CPU port to the given destination to make sure it passes.
     */
    cpu_port = 200;
    sent_packets = 1;
    SHR_IF_ERR_EXIT(diag_dnx_ipq_send_packet(unit, cpu_port, system_port, sent_packets));

    sal_sleep(2);
    SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
    /*
     * Check if packet arrived
     */
    SHR_IF_ERR_EXIT(bcm_stat_get(unit, system_port, snmpEtherStatsTXNoErrors, &recieved_packets));
    if (COMPILER_64_LO(recieved_packets) != sent_packets)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Fail - Expected %d packets but got %d.\n",
                     sent_packets, COMPILER_64_LO(recieved_packets));
    }
    /*
     * clear the counters for the port
     */
    SHR_IF_ERR_EXIT(bcm_stat_clear(unit, system_port));

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");
exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t diag_dnx_ipq_alloc_test_man = {
    "Test allocating all ingress queues",
    "Allocate all queues." " Try to allocate 1 more queue - expect to fail." " Deallocate all queues.",
    NULL,
    "num=4 type=unicast with_id=1",
};

static sh_sand_enum_t diag_dnx_ipq_alloc_all_type_enum_table[] = {
    {"UNICAST", DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST},
    {"MULTICAST", DNX_ALGO_IPQ_QUEUE_TYPE_MULTICAST},
    {"INTERDIG", DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST_INTERDIGITATED},
    {NULL}
};

static sh_sand_option_t diag_dnx_ipq_alloc_test_options[] = {
    {"number", SAL_FIELD_TYPE_INT32, "Number of COS", "4"},
    {"type", SAL_FIELD_TYPE_ENUM, "Queues type", "UNICAST", (void *) diag_dnx_ipq_alloc_all_type_enum_table},
    {"with_id", SAL_FIELD_TYPE_BOOL, "WITH_ID", "0"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_ipq_alloc_tests[] = {
    {"cos_4_uc", "num=4 type=unicast"},
    {"cos_8_uc", "num=8 type=unicast"},
    {"cos_4_mc", "num=4 type=multicast"},
    {"cos_8_mc", "num=8 type=multicast"},
    {"cos_4_interdig", "num=4 type=interdig"},
    {"cos_8_interdig", "num=8 type=interdig"},
    {"cos_4_uc_with_id", "num=4 type=unicast with_id=1"},
    {"cos_8_uc_with_id", "num=8 type=unicast with_id=1"},
    {"cos_4_mc_with_id", "num=4 type=multicast with_id=1"},
    {"cos_8_mc_with_id", "num=8 type=multicast with_id=1"},
    {"cos_4_interdig_with_id", "num=4 type=interdig with_id=1"},
    {"cos_8_interdig_with_id", "num=8 type=interdig with_id=1"},
    {NULL}
};

static sh_sand_man_t diag_dnx_ipq_flow_based_test_man = {
    "This test is allocating a flow based VOQ to existing destination and deletes it.",
    "Allocate a flow based VOQ to an existing destination,"
        "to create a scenario where one Flow based and one system port based VOQs point to the same destination,"
        "delete the flow based VOQ, and make sure the other VOQ is not affected.",
    NULL,
    "port=13 voq_id=2048",
};

static sh_sand_option_t diag_dnx_ipq_flow_based_test_options[] = {
    {"port", SAL_FIELD_TYPE_UINT32, "System port number", "1"},
    {"voq_id", SAL_FIELD_TYPE_UINT32, "VOQ id to create", "1024"},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_ipq_flow_based_tests[] = {
    {"alloc_delete", "port=1 voq_id=1024"},
    {NULL}
};

sh_sand_cmd_t sh_dnx_tm_ipq_test_cmds[] = {
    {"allocate", diag_dnx_ipq_alloc_test_cmd, NULL, diag_dnx_ipq_alloc_test_options, &diag_dnx_ipq_alloc_test_man, NULL,
     diag_dnx_ipq_alloc_tests},
    {"flow_based", diag_dnx_ipq_flow_based_test_cmd, NULL, diag_dnx_ipq_flow_based_test_options,
     &diag_dnx_ipq_flow_based_test_man, NULL, diag_dnx_ipq_flow_based_tests},
    {NULL}
};

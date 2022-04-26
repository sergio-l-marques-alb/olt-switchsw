/** \file ctest_dnx_mrps.c
 *
 * Main ctests for mrps
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/dnx/diag_dnx_utils.h>
#include <appl/diag/dnx/diag_dnx_l2.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
/** bcm */
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <src/bcm/dnx/cosq/scheduler/scheduler_dbal.h>
#include <src/bcm/dnx/cosq/egress/ecgm_dbal.h>
/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>

static shr_error_e
dnx_mrps_expansion_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int database_id;
    int expand_per_tc;
    int mode;
    int color_blind;
    bcm_pbmp_t logical_ports;
    int logical_port = -1;
    int port_temp;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    dnx_algo_port_info_s port_info;
    uint32 flags;
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_cint_argument_t cint_arguments[6];
#if !defined(ADAPTER_SERVER_MODE)
    bcm_policer_fwd_types_t dmac_idx;
    rhhandle_t packet_h = NULL;
    char dmac_str[bcmPolicerFwdTypeMax][18] =
        { "00:00:00:00:00:01", "00:00:00:00:00:02", "01:00:5e:00:00:01", "01:00:00:00:00:01", "FF:FF:FF:FF:FF:FF" };
    char traffic_type_str[bcmPolicerFwdTypeMax][4] = { "UC", "UUC", "MC", "UMC", "BC" };
    char *smac_str = "00:66:77:88:99:aa";
    uint32 vlan = 1;
    uint32 cfi = 0;
    uint32 pcp;
    int stream_rate = 10000000; /** 10GB in kbits*/
    int packet_size_bytes = 128;
    int stream_duration = 10;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 1000;
    int stream_granularity = -1;
    int offset;
    uint32 base_meter_rate = 30000000; /** 30Mbits in bits */
    uint64 recieved_packets;
    uint64 expected_packets_low;
    uint64 expected_packets_high;
    uint32 expected_rate_bits;
    uint64 expected_rate_bits_low;
    uint64 expected_rate_bits_high;
#endif
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get input parameters
     */
    SH_SAND_GET_UINT32("database_id", database_id);
    SH_SAND_GET_BOOL("per_tc", expand_per_tc);
    SH_SAND_GET_BOOL("color_blind", color_blind);
    SH_SAND_GET_BOOL("mode", mode);

    /**
     * Vertify user input - can not have expansion per TC is the feature is not supported
     */
    if ((expand_per_tc == TRUE)
        && (dnx_data_meter.expansion.feature_get(unit, dnx_data_meter_expansion_expansion_based_on_tc_support) ==
            FALSE))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Expansion pet TC not supported for this device!%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    /**
     * Need to remove CPU ports. They are not dynamic, so they need to be removed with SOC properties.
     */
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"ucode_port_20*", NULL}
            ,
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    /**
     * Load Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_port_misc_configuration.c"),
                        "Loading cint_port_misc_configuration.c failed!\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/policer/cint_policer_generic_meter.c"),
                        "Loading cint_policer_generic_meter.c failed!\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_dynamic_port_add_remove.c"),
                        "Loading cint_dynamic_port_add_remove.c failed\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/internal/port/cint_test_dynamic_port_add.c"),
                        "Loading cint_dynamic_port_add_remove.c failed\n");

    /**
     * Get a logical port that will be used for the test. The InPort and OutPort can be the same, so only one port is needed.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &logical_ports));

    /**
     * Remove dynamically all NIF ports
     */
    BCM_PBMP_ITER(logical_ports, port_temp)
    {
        /**
         * Need to leave one NIF Eth port for the test
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port_temp, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0) && (logical_port == -1))
        {
            logical_port = port_temp;
            continue;
        }
        sal_memset(cint_arguments, 0, sizeof(cint_arguments));
        cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
        cint_arguments[0].value.value_int32 = port_temp; /** port to remove */
        cint_arguments[1].type = SAL_FIELD_TYPE_UINT32;
        cint_arguments[1].value.value_uint32 = 0; /** flags */
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "cint_dyn_port_remove_port_full_example", cint_arguments, 2),
                            "Running cint_dyn_port_remove_port_full_example failed!\n");
    }

    /**
     * Get port properties
     */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info));

    /**
     * Disable learning for the port. This is done because there are scenarios that muct be tested with unknown address
     */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, logical_port, BCM_PORT_LEARN_FWD));

    /**
     * Set Cint arguments
     */
    sal_memset(cint_arguments, 0, sizeof(cint_arguments));
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = logical_port;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = logical_port;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = database_id;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = expand_per_tc;
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = ((mode == 0) ? bcmPolicerModeSrTcm : bcmPolicerModeTrTcmDs);
    cint_arguments[5].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[5].value.value_int32 = color_blind;

    /**
     * Run the Cint function to create one Ingress Meter Database with expansion enabled
     * and configuiration based on the user input.
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_ingress_generic_expansion_example", cint_arguments, 6),
                        "Running policer_ingress_generic_expansion_example failed!\n");

    SHR_IF_ERR_EXIT(bcm_switch_control_port_set(unit, logical_port, bcmSwitchMeterAdjust, 4));

    /**
     * Test all cases (traffic skipped in adapter)
     * Build packets - max of 40 streams with 40 types of packets will be built.
     * 0 - UC with PCP 0 and CFI 0
     * 1 - MC with PCP 0 and CFI 0
     * 2 - UMC with PCP 0 and CFI 0
     * 3 - UUC with PCP 0 and CFI 0
     * 4 - BC with PCP 0 and CFI 0
     *  ...... etc. up to:
     * 36 - UC with PCP 7 and CFI 0
     * 37 - MC with PCP 7 and CFI 0
     * 38 - UMC with PCP 7 and CFI 0
     * 39 - UUC with PCP 7 and CFI 0
     * 40 - BC with PCP 7 and CFI 0
     */
#if !defined(ADAPTER_SERVER_MODE)
    /** 
     * Allocate a handle for the packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    for (pcp = 0; pcp < BCM_COS_COUNT; pcp++)
    {
        for (dmac_idx = bcmPolicerFwdTypeUc; dmac_idx < bcmPolicerFwdTypeMax; dmac_idx++)
        {
            /**
             * Check if expansion for unknown address is supported.
             * If not, skip unknown address cases
             */
            if ((expand_per_tc == FALSE)
                && (dnx_data_meter.
                    expansion.feature_get(unit, dnx_data_meter_expansion_expansion_for_uuc_umc_support) == FALSE)
                && ((dmac_idx == bcmPolicerFwdTypeUnkownUc) || (dmac_idx == bcmPolicerFwdTypeUnkownMc)))
            {
                continue;
            }

            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                            (unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac_str[dmac_idx]));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac_str));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.DEI", &cfi, 1));
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                            (unit, mapping_info.core, packet_h, packet_size_bytes, stream_duration, type, stream_rate,
                             stream_burst, stream_granularity));

            /**
             * Clear stats on port
             */
            SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

            /**
             * Test UC stream with PCP 0 (relevant for both Color blind and Color sensitive modes)
             */
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));
            sal_sleep(2);

            /**
             * Destroy stream without freeing the packet handle, 
             * so it can be re-used in the next iterations.
             * This is done in order not to go over the allowed
             * number of streams that can be created at the same time
             */
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h));

            /**
             * In both color blind and color aware mode the results will be the same,
             * because PCP 0 is marked as green. There will be difference according to the mode:
             *    bcmPolicerModeSrTcm - the stream will go through the committed bucket, but not through the excess bucket as there are no credits in it.
             *    This means that for the base meter the packets will pass at a rate of 30 Mbit/s.
             *    bcmPolicerModeTrTcmDs - the stream will go through the committed bucket and afterwards also through the excess bucket.
             *    This means that for the base meter the packets will pass at a rate of 50 Mbit/s (30 Mbit/s from the committed bucket
             *    and 20 Mbit/s from the excess bucket).
             */
            if (expand_per_tc == TRUE)
            {
                /**
                 * Expansion is per TC.
                 * TC0 is mapped to meter with offset=7 and TC7 is mapped to offset=0 and each meter is 5 Mbit/s above the previous.
                 * Expansion per TC does not care about L2-FDW Type, so the rate does not depend on it.
                 */
                offset = BCM_COS_COUNT - (pcp + 1);
            }
            else
            {
                /**
                 * Expansion is per L2-FWD Type and does not care about TC.
                 * UC is mapped to meter with offset=4, MC is mapped to meter with offset=3,
                 * UMC is mapped to offset=2, UUC is mapped to offset=1 and BC is mapped to meter with offset=0
                 * Each meter is 5 Mbit/s above the previous.
                 */
                offset = bcmPolicerFwdTypeMax - (dmac_idx + 1);
            }

            if ((color_blind == FALSE) && (pcp > 3))
            {
                /**
                 * Configuration is not color_blind and the input color is taken into account.
                 * All streams with TC 4 are marked a syellow and will only take credits from the excess bucket,
                 * which is configured the same (20M) for all meters.
                 */
                expected_rate_bits = 20000000;
            }
            else
            {
                /**
                 * Base meter's rate is 30M and each meter is 5 Mbit/s above the previous.
                 * If the mode is two rates another 20M needs to be added for the excess bucket.
                 */
                expected_rate_bits =
                    ((mode ==
                      0) ? (base_meter_rate + (offset * 5000000)) : (base_meter_rate + (offset * 5000000) + 20000000));
            }

            /**
             * Calculate the number of expected packets based on the rate.
             * Stream duration is 10 sec.
             */

            /** Calculate 98% */
            COMPILER_64_SET(expected_rate_bits_low, 0, expected_rate_bits);
            COMPILER_64_UMUL_32(expected_rate_bits_low, 98);
            COMPILER_64_UDIV_32(expected_rate_bits_low, 100);
            /** Calculate 102% */
            COMPILER_64_SET(expected_rate_bits_high, 0, expected_rate_bits);
            COMPILER_64_UMUL_32(expected_rate_bits_high, 102);
            COMPILER_64_UDIV_32(expected_rate_bits_high, 100);

            /**
             * Calculate expacted packets high level
             */
            COMPILER_64_SET(expected_packets_high, COMPILER_64_HI(expected_rate_bits_high),
                            COMPILER_64_LO(expected_rate_bits_high));
            COMPILER_64_UDIV_32(expected_packets_high, (packet_size_bytes * 8));
            COMPILER_64_UMUL_32(expected_packets_high, stream_duration);
            /**
             * Calculate expacted packets low level
             */
            COMPILER_64_SET(expected_packets_low, COMPILER_64_HI(expected_rate_bits_low),
                            COMPILER_64_LO(expected_rate_bits_low));
            COMPILER_64_UDIV_32(expected_packets_low, (packet_size_bytes * 8));
            COMPILER_64_UMUL_32(expected_packets_low, stream_duration);

            /**
             * Get TX statistics from the port - how many packets exited through it.
             * Compare with expected result, allowing 2% deviation.
             */
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));
            LOG_CLI(("Testing %s traffic with TC %d ...\n", traffic_type_str[dmac_idx], pcp));

            if (COMPILER_64_LT(recieved_packets, expected_packets_low)
                || COMPILER_64_GT(recieved_packets, expected_packets_high))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - Expected  from %d to %d packets on port %d but got %d.\n",
                             COMPILER_64_LO(expected_packets_low), COMPILER_64_LO(expected_packets_high), logical_port,
                             COMPILER_64_LO(recieved_packets));
            }
        }
    }
#endif
    /**
     * Reset Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

    /**
     * Clear L2 entries
     */
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));

exit:
#if !defined(ADAPTER_SERVER_MODE)
    if (packet_h != NULL)
    {
        diag_sand_packet_free(unit, packet_h);
    }
#endif
    if (ctest_soc_set_h != NULL)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_mrps_tcsm_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int database_id;
    int mode;
    int color_blind;
    bcm_pbmp_t logical_ports;
    int logical_port;
    int base_qpair;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    ctest_cint_argument_t cint_arguments[5];
    rhhandle_t ctest_soc_set_h = NULL;
#if !defined(ADAPTER_SERVER_MODE)
    rhhandle_t packet_h[2];
    char *dmac_str = "00:00:00:00:00:01";
    char *smac_str = "00:66:77:88:99:aa";
    uint32 vlan = 1;
    uint32 cfi = 0;
    uint32 pcp = 0;
    uint32 pcp_low_prio = 0;
    int stream_rate = 100000; /** 100 Mbit/s in kbits*/
    int packet_size_bytes = 128;
    int stream_duration = 10;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 1000;
    int stream_granularity = -1;
    uint32 expected_packets = 0;
    uint32 expected_packets_low = 0;
    uint32 expected_packets_high = 0;
    uint64 recieved_packets;
#endif
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Need to work in 8 priorities mode.
     */
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"port_priorities", "8"}
            ,
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    /**
     * Get input parameters
     */
    SH_SAND_GET_UINT32("database_id", database_id);
    SH_SAND_GET_BOOL("color_blind", color_blind);
    SH_SAND_GET_BOOL("mode", mode);

    /**
     * Get a logical port that will be used for the test. The InPort and OutPort can be the same, so only one port is needed.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, 0, &logical_ports));
    _SHR_PBMP_FIRST(logical_ports, logical_port);
    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info));

    /**
     * Load Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/port/cint_port_misc_configuration.c"),
                        "Loading cint_port_misc_configuration.c failed!\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/policer/cint_policer_generic_meter.c"),
                        "Loading cint_policer_generic_meter.c failed!\n");

    /**
     * Set Cint arguments
     */
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = logical_port;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = logical_port;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = database_id;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = ((mode == 0) ? bcmPolicerModeSrTcm : bcmPolicerModeTrTcmDs);
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = color_blind;

    /**
     * Run the Cint function to cteare the Ingress Meter Database and configure TCSM with the specified parameters
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_ingress_generic_tcsm_example", cint_arguments, 5),
                        "Running policer_ingress_generic_tcsm_example failed!\n");

    /**
     * Disable the EGQ to Scheduler FC
     */
    SHR_IF_ERR_EXIT(dnx_sch_fc_enablers_set(unit, mapping_info.core, 0, 0, 0, 0, 0));

    /**
     * Get the base Q-pair of the port. It will be used later
     * to disable relevant Q-airs of some PCPs
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_qpair));

    /**
     * Disable learning
     */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, logical_port, BCM_PORT_LEARN_FWD));
    /**
     * Test all TCs (skip traffic for adapter)
     * Build packets to test the following thresholds (set in the cint):
     * TC0 - 0%
     * TC1 - 0%
     * TC2 - 0%
     * TC3 - 0%
     * TC4 - 30%
     * TC5 - 30%
     * TC6 - 30%
     * TC7 - 30%
     */
#if !defined(ADAPTER_SERVER_MODE)
    /** 
     * Allocate a handle for the packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h[0]));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h[1]));
    for (pcp = 0; pcp < (BCM_COS_COUNT / 2); pcp++)
    {
        /**
         * TCs 0-3 are high priority with threshold 0%
         * TCs 4-7 are low priority with thresholds 30%
         */
        pcp_low_prio = pcp + BCM_COS_COUNT / 2;

        /**
         * Set the stream rate and calculate expected packets. In color sensitive mode PCPs 1,3,5 and 7
         * go to the Yellow bucket, which is 20 Mbps
         */
        if ((pcp % 2) && (color_blind = FALSE))
        {
            stream_rate = 20000; /** 20 Mbit/s in kbits*/
        }
        expected_packets = (((stream_rate * 1000) / 8) / packet_size_bytes) * stream_duration;
        expected_packets_high = expected_packets * 105 / 100;
        expected_packets_low = expected_packets * 95 / 100;

        /**
         * Create two streams for the different PCPs and set them to 50% rate.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[0], "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h[0], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[0], "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[0], "ETH1.DA", dmac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[0], "ETH1.SA", smac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[0], "ETH1.VLAN.VID", &vlan, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[0], "ETH1.VLAN.PCP", &pcp, 3));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[0], "ETH1.VLAN.DEI", &cfi, 1));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                        (unit, mapping_info.core, packet_h[0], packet_size_bytes, stream_duration, type,
                         (stream_rate * 50 / 100), stream_burst, stream_granularity));

        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[1], "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h[1], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[1], "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[1], "ETH1.DA", dmac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[1], "ETH1.SA", smac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[1], "ETH1.VLAN.VID", &vlan, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[1], "ETH1.VLAN.PCP", &pcp_low_prio, 3));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[1], "ETH1.VLAN.DEI", &cfi, 1));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                        (unit, mapping_info.core, packet_h[1], packet_size_bytes, stream_duration, type,
                         (stream_rate * 50 / 100), stream_burst, stream_granularity));

        /**
         * Clear stats on port
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

        /**
         * Test UC streams with PCP and pcp_low_prio
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, 2));
        sal_sleep(2);
        /**
         * Get TX statistics from the port - how many packets exited through it.
         */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

        /**
         * The first scenario is to make sure all packets are passing
         */
        if ((COMPILER_64_LO(recieved_packets) > (expected_packets_high)) ||
            (COMPILER_64_LO(recieved_packets) < (expected_packets_low)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "FAILURE - Not all packets passed: got %d, expected %d!\n",
                         COMPILER_64_LO(recieved_packets), expected_packets);
        }

        /**
         * Disable the Q-pair of pcp_low_prio. This way the output packets will be compared to
         * the expected results only for PCP
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp_low_prio), 1));

        /**
         * Clear stats on port
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

        /**
         * Test UC streams with PCP and pcp_low_prio
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, 2));
        sal_sleep(2);
        /**
         * Get TX statistics from the port - how many packets exited through it.
         */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

        /**
         * The thresholds of the PCPs are as follows:
         * PCP - 0, meaning empty level for this PCP will be when the committed bucket is actually empty
         * pcp_low_prio - 30, meaning empty level for this PCP will be when the committed bucket is at 30%
         * Since the two streams of packets were both set to 50% of the total, for both of them we should
         * receive all packets, equally divided between the two PCPs (5% deviation). However, since we
         * disabled the Q-pair of pcp_low_prio, we expect to receive only 50% of the packets.
         */
        if ((COMPILER_64_LO(recieved_packets) > (expected_packets_high / 2)) ||
            (COMPILER_64_LO(recieved_packets) < (expected_packets_low / 2)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "FAILURE - Packets not equally distributed between TCs: TC%d got %d, expected %d!\n", pcp,
                         COMPILER_64_LO(recieved_packets), (expected_packets / 2));
        }

        /**
         * Disable the Q-pair of pcp. This way the output packets will be compared to
         * the expected results only for pcp_low_prio
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp_low_prio), 0));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp), 1));

        /**
         * Clear stats on port
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

        /**
         * Test UC streams with PCP and pcp_low_prio
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, 2));
        sal_sleep(2);
        /**
         * Get TX statistics from the port - how many packets exited through it.
         */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

        /**
         * The same as above test scenario, but this time for the other PCP - pcp_low_prio
         */
        if ((COMPILER_64_LO(recieved_packets) > (expected_packets_high / 2)) ||
            (COMPILER_64_LO(recieved_packets) < (expected_packets_low / 2)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "FAILURE - Packets not equally distributed between TCs: TC%d got %d, expected %d!\n",
                         pcp_low_prio, COMPILER_64_LO(recieved_packets), (expected_packets / 2));
        }

        /**
         * Destroy streams without freeing the packet pointer.
         * This is done in order not to go over the allowed
         * number of streams that can be created at the same time
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h[0]));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h[1]));

        /**
         * Create two streams for the different PCPs and set them to 100% rate.
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[0], "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h[0], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[0], "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[0], "ETH1.DA", dmac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[0], "ETH1.SA", smac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[0], "ETH1.VLAN.VID", &vlan, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[0], "ETH1.VLAN.PCP", &pcp, 3));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[0], "ETH1.VLAN.DEI", &cfi, 1));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                        (unit, mapping_info.core, packet_h[0], packet_size_bytes, stream_duration, type, stream_rate,
                         stream_burst, stream_granularity));

        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[1], "PTCH_2"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                        (unit, packet_h[1], "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h[1], "ETH1"));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[1], "ETH1.DA", dmac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h[1], "ETH1.SA", smac_str));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[1], "ETH1.VLAN.VID", &vlan, 12));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[1], "ETH1.VLAN.PCP", &pcp_low_prio, 3));
        SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h[1], "ETH1.VLAN.DEI", &cfi, 1));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                        (unit, mapping_info.core, packet_h[1], packet_size_bytes, stream_duration, type, stream_rate,
                         stream_burst, stream_granularity));

        /**
         * Re-enable both Q-pairs. This scenario is to validate that the meter is passing only 100Mbps.
         * Currently the two streams togeter are 200Mbps, but all pcp_low_prio traffic should be dropped
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp_low_prio), 0));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp), 0));

        /**
         * Clear stats on port
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

        /**
         * Test UC streams with PCP and pcp_low_prio
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, 2));
        sal_sleep(2);

        /**
         * Get TX statistics from the port - how many packets exited through it.
         */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

        /**
         * The thresholds of the PCPs are as follows:
         * PCP - 0, meaning empty level for this PCP will be when the committed bucket is actually empty
         * pcp_low_prio - 30, meaning empty level for this PCP will be when the committed bucket is at 30%
         * Since the two streams of packets were both set to 100% of the total, if there was no TCSM we
         * would receive 50% total traffic, but still equally divided between TCs.
         * However, since there is congestion and the bucket is below 50% level,
         * all the BW should go to PCP and none for the pcp_low_prio. Since we
         * disabled the Q-pair of pcp_low_prio, we expect to receive 100% of the packets.
         * Allow up to 20% more packets due to initial burst + some pcp_low_prio that passe4d befor the bucket got below empty level
         */
        if ((COMPILER_64_LO(recieved_packets) > (expected_packets_high + (expected_packets * 20 / 100))) ||
            (COMPILER_64_LO(recieved_packets) < expected_packets_low))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - wrong number of packets for TC %d, expected %d and received %d.\n",
                         pcp, expected_packets, COMPILER_64_LO(recieved_packets));
        }

        /**
         * Disable only the Q-pair correcponding to pcp_low_prio and validate the result is same as above,
         * meaning that all traffic that passed was pcp
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp_low_prio), 1));

        /**
         * Clear stats on port
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

        /**
         * Test UC streams with PCP and pcp_low_prio
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, 2));
        sal_sleep(2);

        /**
         * Get TX statistics from the port - how many packets exited through it.
         */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

        /**
         * The same check as above scenario
         */
        if ((COMPILER_64_LO(recieved_packets) > expected_packets_high) ||
            (COMPILER_64_LO(recieved_packets) < expected_packets_low))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - wrong number of packets for TC %d, expected %d and received %d.\n",
                         pcp, expected_packets, COMPILER_64_LO(recieved_packets));
        }

        /**
         * Re-enable the Q-pair of pcp_low_prio and disable the Qpair of pcp. All traffic should be dropped
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp_low_prio), 0));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp), 1));

        /**
         * Clear stats on port
         */
        SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

        /**
         * Test UC streams with PCP and pcp_low_prio
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, packet_h, 2));
        sal_sleep(2);

        /**
         * Get TX statistics from the port - how many packets exited through it.
         */
        COMPILER_64_ZERO(recieved_packets);
        SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
        SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

        /**
         * Check that we did not get more than 25% of the expected packets.
         */
        if (COMPILER_64_LO(recieved_packets) > (expected_packets * 25 / 100))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "FAILURE - wrong number of packets for TC %d, expected %d and received %d.\n",
                         pcp_low_prio, (expected_packets * 25 / 100), COMPILER_64_LO(recieved_packets));
        }

        /**
         * Destroy streams without freeing the packet pointer.
         * This is done in order not to go over the allowed
         * number of streams that can be created at the same time
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h[0]));
        SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h[1]));

        /**
         * Enable the Q-pair of pcp.
         */
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, mapping_info.core, (base_qpair + pcp), 0));
    }
#endif
    /**
     * Set Cint arguments for clean-up
     */
    sal_memset(cint_arguments, 0, sizeof(ctest_cint_argument_t));
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = mapping_info.core;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = database_id;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = ((mode == 0) ? bcmPolicerModeSrTcm : bcmPolicerModeTrTcmDs);

    /**
     * Run the Cint function to destroy the Ingress Meter Database
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_ingress_generic_tcsm_destroy", cint_arguments, 3),
                        "Running policer_ingress_generic_expansion_example_destroy failed!\n");

    /**
     * Reset Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

    /**
     * Clear L2 entries
     */
    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));

exit:
#if !defined(ADAPTER_SERVER_MODE)
    diag_sand_packet_free(unit, packet_h[0]);
    diag_sand_packet_free(unit, packet_h[1]);
#endif
    if (ctest_soc_set_h != NULL)
    {
        ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_is_tcsm_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_meter.meter_db.feature_get(unit, dnx_data_meter_meter_db_tcsm_support) != TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_mrps_single_bucket_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int database_id;
    int engine;
    bcm_pbmp_t logical_ports;
    bcm_port_t logical_port;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_policer_config_t pol_cfg;
    bcm_core_t core_id;
    bcm_policer_engine_section_t section = bcmPolicerEngineSectionAll;
    int meter_idx, policer_id, database_handle;
    int is_ingress = 0, base_pointer = 0, single_bucket_mode = TRUE;
    int expansion_enable = 0, expand_per_tc = 0, configure_meters = 0;
    int total_nof_meters = 1;
    uint32 flags;
    uint32 vlan = 100;
    ctest_cint_argument_t cint_arguments[10];
#if !defined(ADAPTER_SERVER_MODE)
    rhhandle_t packet_h = NULL;
    char *dmac_str = "00:00:00:00:00:01";
    char *smac_str = "00:66:77:88:99:aa";
    uint32 cfi = 0;
    uint32 pcp = 0;
    int stream_rate = 30000;  /** 30 Mbit/s in kbits*/
    int packet_size_bytes = 128;
    int stream_duration = 5;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 1000;
    int stream_granularity = -1;
    int deviation = 5;
    uint32 expected_packets = 0;
    uint64 recieved_packets;
#endif

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");

    /*
     * Get input from shell
     */
    SH_SAND_GET_UINT32("database_id", database_id);
    SH_SAND_GET_UINT32("engine", engine);

    /**
     * Check if we try to add small engine on database different that 0
     */
    if ((engine == dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(unit)) && (database_id != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Small engine is only allowed on database 0.\n");
    }
    /**
     * Check if predefined values for engine was used
     */
    if ((engine != dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(unit)) &&
        (engine != dnx_data_meter.mem_mgmt.base_big_engine_for_meter_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Please use predefined values for engine.\n");
    }
    /**
     * Load Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_load(unit, "dnx/policer/cint_policer_generic_meter.c"),
                        "Loading cint_policer_generic_meter.c failed!\n");

    /**
     * Get a logical port that will be used for the test.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, 0, &logical_ports));
    _SHR_PBMP_FIRST(logical_ports, logical_port);
    SHR_IF_ERR_EXIT(bcm_port_get(unit, logical_port, &flags, &interface_info, &mapping_info));
    core_id = mapping_info.core;

    /**
     * Set Cint arguments
     */
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = core_id;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = is_ingress;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = database_id;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = base_pointer;
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = single_bucket_mode;
    cint_arguments[5].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[5].value.value_int32 = expansion_enable;
    cint_arguments[6].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[6].value.value_int32 = expand_per_tc;
    cint_arguments[7].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[7].value.value_int32 = total_nof_meters;
    cint_arguments[8].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[8].value.value_int32 = configure_meters;
    cint_arguments[9].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[9].value.value_int32 = engine;

    /**
     * Run the Cint function to create the Egress Meter Database with single bucket_engine
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_generic_meter_database_example", cint_arguments, 10),
                        "Running policer_generic_meter_database_example failed!\n");

    /**
     * Configure meter
     */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeCommitted;
    pol_cfg.ckbits_sec = 15000;
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.pkbits_sec = 0;
    pol_cfg.pkbits_burst = 0;
    pol_cfg.max_pkbits_sec = 15000;
    pol_cfg.core_id = core_id;
    meter_idx = 8;
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, 0, 0, core_id, database_id);
    BCM_POLICER_ID_SET(policer_id, database_handle, meter_idx);
    SHR_IF_ERR_EXIT(bcm_policer_create(unit, &pol_cfg, &policer_id));

    /**
     * Set Cint arguments
     */
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = core_id;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = meter_idx;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = database_id;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = logical_port;
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = vlan;

    /**
     * Run the Cint function to generate meter pointer and configure egress pp trap
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_egress_pointer_generation", cint_arguments, 5),
                        "Running policer_egress_pointer_generation failed!\n");
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_egress_drop_packet", cint_arguments, 0),
                        "Running policer_egress_drop_packet failed!\n");

#if !defined(ADAPTER_SERVER_MODE)
    /**
     * Allocate a handle for the packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));

    /**
     * Create stream
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.DEI", &cfi, 1));
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                    (unit, mapping_info.core, packet_h, packet_size_bytes, stream_duration, type,
                     stream_rate, stream_burst, stream_granularity));
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, logical_port, logical_port, 1));

    /**
     * Clear stats on port
     */
    SHR_IF_ERR_EXIT(bcm_stat_clear(unit, logical_port));

    /**
     * Send stream
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));
    sal_sleep(2);

    /**
     * Get TX statistics from the port - how many packets exited through it.
     */
    COMPILER_64_ZERO(recieved_packets);
    SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
    SHR_IF_ERR_EXIT(bcm_stat_get(unit, logical_port, snmpEtherStatsTXNoErrors, &recieved_packets));

    /**
     * Calculate expected packets
     */
    expected_packets = (((stream_rate * 1000) / 8) / packet_size_bytes) * stream_duration;

    sal_printf("expected_packets are %d\n", expected_packets / 2);
    sal_printf("received_packets are %d\n", COMPILER_64_LO(recieved_packets));

    /**
     * Verify that received packets are not over 50% (with 5% deviation)
     */
    if (((COMPILER_64_LO(recieved_packets) * 100) / expected_packets) > (50 + deviation))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Received packets are above expected \n");
    }

    /**
     * Verify that received packets are not below 50% (with 5% deviation)
     */
    if (((COMPILER_64_LO(recieved_packets) * 100) / expected_packets) < (50 - deviation))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Received packets are below expected \n");
    }

    /**
     * Start clean up
     */
    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, logical_port, logical_port, 0));
#endif
    /**
     * Set Cint arguments for clean-up
     */
    if (engine != dnx_data_meter.mem_mgmt.egress_single_bucket_engine_get(unit))
    {
        section = bcmPolicerEngineSectionLow;
    }
    /**
     * Set Cint arguments
     */
    sal_memset(cint_arguments, 0, sizeof(ctest_cint_argument_t));
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = core_id;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = is_ingress;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = database_id;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = engine;
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = section;

    /**
     * Run the Cint function to destroy the Egress Meter Database
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_generic_meter_database_destroy", cint_arguments, 5),
                        "Running policer_generic_meter_database_destroy failed!\n");

    /**
     * Set Cint arguments
     */
    sal_memset(cint_arguments, 0, sizeof(ctest_cint_argument_t));
    cint_arguments[0].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[0].value.value_int32 = core_id;
    cint_arguments[1].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[1].value.value_int32 = meter_idx;
    cint_arguments[2].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[2].value.value_int32 = database_id;
    cint_arguments[3].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[3].value.value_int32 = logical_port;
    cint_arguments[4].type = SAL_FIELD_TYPE_INT32;
    cint_arguments[4].value.value_int32 = vlan;

    /**
     * Run the Cint function to destroy meter pointer
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_run(unit, "policer_egress_pointer_destroy", cint_arguments, 5),
                        "Running policer_egress_pointer_generation failed!\n");

    /**
     * Reset Cints
     */
    SHR_CLI_EXIT_IF_ERR(ctest_dnxc_cint_reset(unit), "CINT Reset Failed\n");

    SHR_IF_ERR_EXIT(sh_dnx_l2_clear_all(unit, TRUE, TRUE));

exit:
#if !defined(ADAPTER_SERVER_MODE)
    if (packet_h != NULL)
    {
        diag_sand_packet_free(unit, packet_h);
    }
#endif
    SHR_FUNC_EXIT;
}

sh_sand_enum_t database_id_table[] = {
    {
     .string = "max",
     .desc = "Ingress database ID",
     .value = 0,
     .plugin_str = "DNX_DATA.meter.meter_db.nof_ingress_db-1"}
    ,
    {NULL}
};

sh_sand_enum_t egress_database_id_table[] = {
    {
     .string = "max_egress_db",
     .desc = "Egress database ID",
     .value = 0,
     .plugin_str = "DNX_DATA.meter.meter_db.nof_egress_db-1"}
    ,
    {NULL}
};

sh_sand_enum_t engine_table[] = {
    {
     .string = "small",
     .desc = "Set small engine",
     .value = 0,
     .plugin_str = "DNX_DATA.meter.mem_mgmt.egress_single_bucket_engine"}
    ,
    {
     .string = "big",
     .desc = "Set big engine",
     .value = 0,
     .plugin_str = "DNX_DATA.meter.mem_mgmt.base_big_engine_for_meter"}
    ,
    {NULL}
};
static sh_sand_option_t dnx_mrps_expansion_list_options[] = {
    {"database_id", SAL_FIELD_TYPE_UINT32, "Ingress database ID", "0", database_id_table, "0-max"},
    {"per_tc", SAL_FIELD_TYPE_BOOL, "Is expansion per TC (TRUE/FALSE)", "0"},
    {"color_blind", SAL_FIELD_TYPE_BOOL, "Is input color taken into consideration (TRUE/FALSE)", "1"},
    {"mode", SAL_FIELD_TYPE_BOOL, "Metering mode: 0 - single rate (3 colors) or 1 - two rates (3 colors)", "1"},
    {NULL}
};

static sh_sand_option_t dnx_mrps_tcsm_list_options[] = {
    {"database_id", SAL_FIELD_TYPE_UINT32, "Ingress database ID", "0", database_id_table, "0-max"},
    {"mode", SAL_FIELD_TYPE_BOOL, "Metering mode: 0 - single rate (3 colors) or 1 - two rates (3 colors)", "1"},
    {"color_blind", SAL_FIELD_TYPE_BOOL, "Is input color taken into consideration (TRUE/FALSE)", "1"},
    {NULL}
};

static sh_sand_option_t dnx_mrps_single_bucket_list_options[] = {
    {"database_id", SAL_FIELD_TYPE_UINT32, "Egress database ID", "0", egress_database_id_table, "0-max_egress_db"},
    {"engine", SAL_FIELD_TYPE_UINT32, "Is engine small or big", "small", engine_table},
    {NULL}
};

static sh_sand_man_t dnx_mrps_expansion_man = {
    .brief = "Run a test for Meter pointer expansion.",
    .full =
        "Run a test for Meter pointer expansion - Configure Ingress database to have expansion enabled, create the relevant meters with different rates and test with traffic.",
    .synopsis = NULL,
    .examples = "database_id=0 per_tc=1 color_blind=TRUE mode=0"
};

static sh_sand_man_t dnx_mrps_tcsm_man = {
    .brief = "Run a test for Traffic Class Sensitive metering.",
    .full =
        "Run a test for Traffic Class Sensitive metering - Configure Ingress database to have TCSM enabled, create the relevant meters and test with traffic.",
    .synopsis = NULL,
    .examples = "database_id=0 color_blind=TRUE"
};

static sh_sand_man_t dnx_mrps_single_bucket_man = {
    .brief = "Run a test for Egress metering.",
    .full =
        "Run a test for Egress metering - Configure Egress database in single bucket mode, create the relevant meters and test with traffic.",
    .synopsis = NULL,
    .examples = "database_id=0 engine=small"
};

static sh_sand_invoke_t dnx_mrps_expansion_tests[] = {
    {"expand_per_tc_single_rate_color_blind", "database_id=0 per_tc=1 color_blind=TRUE mode=0"},
    {"expand_per_tc_two_rates_color_blind", "database_id=0 per_tc=1 color_blind=TRUE mode=1"},
    {"expand_per_tc_single_rate_color_sensitive", "database_id=0 per_tc=1 color_blind=FALSE mode=0"},
    {"expand_per_tc_two_rates_color_sensitive", "database_id=0 per_tc=1 color_blind=FALSE mode=1"},
    {"expand_per_l2_fwd_single_rate_color_blind", "database_id=0 per_tc=0 color_blind=TRUE mode=0"},
    {"expand_per_l2_fwd_two_rates_color_blind", "database_id=0 per_tc=0 color_blind=TRUE mode=1"},
    {"expand_per_l2_fwd_single_rate_color_sensitive", "database_id=0 per_tc=0 color_blind=FALSE mode=0"},
    {"expand_per_l2_fwd_two_rates_color_sensitive", "database_id=0 per_tc=0 color_blind=FALSE mode=1"},
    {NULL}
};

static sh_sand_invoke_t dnx_mrps_tcsm_tests[] = {
    {"tcsm_single_rate_color_blind", "database_id=0 color_blind=TRUE mode=0"},
    {"tcsm_two_rates_color_blind", "database_id=0 color_blind=TRUE mode=1"},
    {"tcsm_single_rate_color_sensitive", "database_id=0 color_blind=FALSE mode=0"},
    {"tcsm_two_rates_color_sensitive", "database_id=0 color_blind=FALSE mode=1"},
    {NULL}
};

static sh_sand_invoke_t dnx_mrps_single_bucket_tests[] = {
    {"egress_single_bucket_small_db0", "database_id=0 engine=small"},
    {"egress_single_bucket_big_db0", "database_id=0 engine=big"},
    {"egress_single_bucket_big_db1", "database_id=1 engine=big"},
    {NULL}
};

/**
 * \brief DNX MRPS ctests
 * List of the supported ctests, pointer to command function and
 * command usage function. This is the entry point for MRPS ctest commands
 */
sh_sand_cmd_t dnx_mrps_test_cmds[] = {
    {"expansion", dnx_mrps_expansion_cmd, NULL, dnx_mrps_expansion_list_options, &dnx_mrps_expansion_man, NULL,
     dnx_mrps_expansion_tests},
    {"tcsm", dnx_mrps_tcsm_cmd, NULL, dnx_mrps_tcsm_list_options, &dnx_mrps_tcsm_man, NULL, dnx_mrps_tcsm_tests,
     CTEST_POSTCOMMIT * SH_CMD_CONDITIONAL, sh_dnx_is_tcsm_available},
    {"egress_single_bucket", dnx_mrps_single_bucket_cmd, NULL, dnx_mrps_single_bucket_list_options,
     &dnx_mrps_single_bucket_man, NULL,
     dnx_mrps_single_bucket_tests},
    {NULL}
};

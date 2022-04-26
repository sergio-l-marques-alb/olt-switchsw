/**
 * ctest_dnx_recycle_tests.c
 *
 *  Created on: Jan 11, 2021
 *      Author: vg889551
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTS

/************
 * INCLUDES *
 ************/
#include <shared/shrextend/shrextend_debug.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

/***********
 * DEFINES *
 ***********/
#define STR_OVERKILL_SIZE 100

/********************
 * STATIC VARIABLES *
 ********************/
/* *INDENT-OFF* */
static sh_sand_enum_t sand_egr_prio_enum[] = {
   /**String      Value                          Description */
    {"1",         DNX_EGR_PORT_ONE_PRIORITY,     "Ports have one priority"},
    {"2",         DNX_EGR_PORT_TWO_PRIORITIES,   "Ports have two priorities"},
    {"4",         DNX_EGR_PORT_FOUR_PRIORITIES,  "Ports have four priorities"},
    {"8",         DNX_EGR_PORT_EIGHT_PRIORITIES, "Ports have eight priorities"},
    {NULL}
};

static sh_sand_option_t sh_dnx_recycle_priorities_options[] = {
    /*Name  Type                  Description                 Default   Extension*/
    {"nof", SAL_FIELD_TYPE_ENUM, "Number of port priorities", "2",      (void *) sand_egr_prio_enum},
    {NULL}
};

static sh_sand_man_t sh_dnx_recycle_priorities_man = {
    .brief = "Run a traffic test for Recycle ports with different number of priorities.",
    .full =
        "Run a traffic test for Recycle ports with different number of priorities - Configure Recycle ports with the required number of priorities into a snake, send traffic and check if it is received back. "
        "The Recycle ports will be configured on different interfaces if possible and their count will be calculated in such way that they span on at least 2 Port schedulers. Traffic with different TCs will be checked.",
    .synopsis = NULL,
    .examples = "nof=8"
};

static sh_sand_invoke_t sh_dnx_recycle_priorities_tests[] = {
    {"1", "nof=1"},
    {"2", "nof=2"},
    {"4", "nof=4"},
    {"8", "nof=8"},
    {NULL}
};

static sh_sand_man_t sh_dnx_recycle_bandwidth_man = {
    .brief = "Run a traffic test for max bandwidth of Recycle ports.",
    .full =
        "Run a traffic test for max bandwidth of Recycle ports. - Create the necessary amount of Recycle ports to utilize the full Recycle bandwidth."
        "The Recycle ports will be configured on different interfaces if possible. ",
    .synopsis = NULL,
    .examples = ""
};


/* *INDENT-ON* */

/********************
 * STATIC FUNCTIONS *
 ********************/
/**
 * Brief - Check if SAT is available
 */
static shr_error_e
sh_dnx_is_sat_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_sat.general.feature_get(unit, dnx_data_sat_general_is_sat_supported) != TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_recycle_priorities_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int priorities = 0;
    int rcy_ports_nof_existing = 0;
    int rcy_ports_nof_needed = 0;
    int rcy_index = 0;
    int rcy_if = 0;
    int rcy_if_rate_mbps = 0;
    int rcy_port_first = 50;
    bcm_port_t rcy_port = 0;
    bcm_pbmp_t rcy_ports;
    bcm_core_t core_id = 0;
    int soc_property_iterator = 0;
    char string_temp1[STR_OVERKILL_SIZE] = { 0 };
    char string_temp2[STR_OVERKILL_SIZE] = { 0 };
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t *ctest_soc_property = NULL;
    bcm_pbmp_t nif_ports;
    bcm_port_t eth_port;
    uint32 flags;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    bcm_gport_t interface_gport = 0;
    bcm_gport_t local_gport = 0;
    bcm_gport_t tc_gport = 0;
    int rate_kbps = 10000000; /** 10 Gbit/s in Kbit/s */
    uint32 pcp = 0;
#if !defined(ADAPTER_SERVER_MODE)
    rhhandle_t packet_h = NULL;
    char *dmac_str = "00:00:00:00:00:01";
    char *smac_str = "00:66:77:88:99:aa";
    uint32 vlan = 100;
    uint32 cfi = 0;
    int packet_size_bytes = 500;
    int stream_duration = 2;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 0;
    int stream_granularity = 100000000; /** 100 Mbit/s in bits/s */
    uint64 bytes_count;
    uint64 expected_packets;
    uint64 expected_packets_low;
    uint64 recieved_packets;
#endif
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Get the user-defined number of priorities to test and the core for the RCY ports
     */
    SH_SAND_GET_ENUM("nof", priorities);

    /**
     * Based on the priorities, we need to calculate the number of RCY ports in such a way that
     * they will span on at least 2 Port schedulers.
     */
    rcy_ports_nof_needed = (DNX_EGR_NOF_Q_PAIRS_IN_PS * 2) / priorities;

    /** Get a NIF ETH port that will be used for the test. The InPort and OutPort can be the same, so only one port is needed. */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, 0, &nif_ports));
    _SHR_PBMP_FIRST(nif_ports, eth_port);
    SHR_IF_ERR_EXIT(bcm_port_get(unit, eth_port, &flags, &interface_info, &mapping_info));

    /** Disable learning on the NIF ETH port */
    SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, eth_port, BCM_PORT_LEARN_FWD));

    /**  Iterate over the cores, run the test one time for each core */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /**
         * For the goals of the test, we need to remove all RCY ports.
         * However, dynamic cint port procedures for RCY ports are not supported, so we
         * need to get the existing ones and remove them using SOC properties
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_RCY, 0, &rcy_ports));
        BCM_PBMP_COUNT(rcy_ports, rcy_ports_nof_existing);
        SHR_ALLOC_SET_ZERO(ctest_soc_property,
                           sizeof(ctest_soc_property_t) * (rcy_ports_nof_existing + rcy_ports_nof_needed + 3),
                           "ctest_soc_property", "%s%s%s", EMPTY, EMPTY, EMPTY);

        /** Remove all RCY ports using SOC property */
        soc_property_iterator = 0;
        BCM_PBMP_ITER(rcy_ports, rcy_port)
        {
            /** Allocate the SOC properties property name, no need to allocate value, must be NULL to delete  */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 2),
                               "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
            /** Set the SOC property string */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "ucode_port_%d*", rcy_port);
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            /** Value must be NULL to delete */
            ctest_soc_property[soc_property_iterator].value = NULL;
            soc_property_iterator++;
        }

        /** Add the needed RCY ports for the test */
        for (rcy_index = 0; rcy_index < rcy_ports_nof_needed; rcy_index++)
        {
            /** Rcy index will be used as a channel. Calculate the logical port number*/
            rcy_port = rcy_index + rcy_port_first;
            /** Calculate the RCY interface number. If the device supports 2 Rcy interfaces, make sure to create ports on both */
            rcy_if = (dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit) > 1) ? (rcy_index % 2) : 0;
            /** Allocate the SOC properties structure */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
            /** Set the SOC property strings */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "ucode_port_%d", rcy_port);
            sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "RCY%d.%d:core_%d.%d", rcy_if, rcy_index, core_id, rcy_port);
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
            soc_property_iterator++;
        }

        /** Set the RCY total rate per IF SOC property in Mbits/s, each port will be 10G */
        rcy_if_rate_mbps =
            (rcy_ports_nof_needed * (rate_kbps / 1000)) / dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit);
        /** Allocate the SOC properties structure */
        SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                           "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                           "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
        /** Set the SOC property strings */
        sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "port_init_speed_rcy");
        sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "%d", rcy_if_rate_mbps);
        sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
        sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
        soc_property_iterator++;

        /** Set the number of port priorities */
        /** Allocate the SOC properties structure */
        SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                           "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                           "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
        /** Set the SOC property strings */
        sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "port_priorities");
        sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "%d", priorities);
        sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
        sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
        soc_property_iterator++;

        /** Last member must be NULL */
        ctest_soc_property[soc_property_iterator].property = NULL;
        ctest_soc_property[soc_property_iterator].value = NULL;
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

        /** Forward traffic from the NIF ETH to the first RCY port */
        SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, eth_port, rcy_port_first, 1));

        /** Set the interface shaper for the RCY ports. Need to call the API only for the first RCY port, as
         * they are represented by a single interface in the egress shapers */
        BCM_GPORT_LOCAL_SET(local_gport, rcy_port_first);
        SHR_IF_ERR_EXIT(bcm_fabric_port_get(unit, local_gport, 0, &interface_gport));
        SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set
                        (unit, interface_gport, 0, 0, (rate_kbps * rcy_ports_nof_needed), 0));

        for (rcy_index = 0; rcy_index < rcy_ports_nof_needed; rcy_index++)
        {
            /** Calculate the logical port number*/
            rcy_port = rcy_index + rcy_port_first;
            /** Disable learning on the current RCY port */
            SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, rcy_port, BCM_PORT_LEARN_FWD));
            if (rcy_index == (rcy_ports_nof_needed - 1))
            {
                /** The last RCY port must return traffic to the NIF ETH port */
                SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, rcy_port, eth_port, 1));
            }
            else
            {
                /** Set force-forward to the next RCY port */
                SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, rcy_port, (rcy_port + 1), 1));
            }

            /** Use also the same loop to set the port and TC shapers for the RCY ports */
            BCM_GPORT_LOCAL_SET(local_gport, rcy_port);
            SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, rate_kbps, 0));
            for (pcp = 0; pcp < priorities; pcp++)
            {
                BCM_COSQ_GPORT_PORT_TC_SET(tc_gport, rcy_port);
                SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, tc_gport, pcp, 0, rate_kbps, 0));
            }
        }
#if !defined(ADAPTER_SERVER_MODE)
        /** Calculate the SAT burst size based on the device and required rate */
        stream_burst = ((rate_kbps * 1000) / 8) / dnx_data_device.general.core_clock_khz_get(unit);

        /** Allocate a handle for the packet */
        SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
        for (pcp = 0; pcp < BCM_COS_COUNT; pcp++)
        {
            LOG_CLI(("Testing %d Recycle ports with %d priorities for TC %d ...\n", rcy_ports_nof_needed, priorities,
                     pcp));

            /** Create a stream for the current PCP and set it to 10G rate for 2 seconds */
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                            (unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac_str));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac_str));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.DEI", &cfi, 1));
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                            (unit, mapping_info.core, packet_h, packet_size_bytes, stream_duration, type, rate_kbps,
                             stream_burst, stream_granularity));

            /** Clear stats on port */
            SHR_IF_ERR_EXIT(bcm_stat_clear(unit, eth_port));

            /** Send stream */
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));
            sal_sleep(2);

            /** Calculate the expected packets */
            COMPILER_64_ZERO(expected_packets);
            COMPILER_64_ZERO(expected_packets_low);
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &expected_packets));
            COMPILER_64_SET(expected_packets_low, COMPILER_64_HI(expected_packets), COMPILER_64_LO(expected_packets));
            COMPILER_64_UMUL_32(expected_packets_low, 99);
            COMPILER_64_UDIV_32(expected_packets_low, 100);

            /** Get TX statistics from the port - how many packets exited through it */
            COMPILER_64_ZERO(recieved_packets);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, eth_port, snmpEtherStatsTXNoErrors, &recieved_packets));

            /** Make sure all packets are passing */
            if ((COMPILER_64_LO(recieved_packets) > COMPILER_64_LO(expected_packets)) ||
                (COMPILER_64_LO(recieved_packets) < COMPILER_64_LO(expected_packets_low)))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "FAILURE - Not all packets passed: got %d, expected %d!\n",
                             COMPILER_64_LO(recieved_packets), COMPILER_64_LO(expected_packets));
            }

            /** Destroy streams without freeing the packet pointer and continue to next PCP */
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h));
        }
#endif
        /** Free the SOC properties structure */
        while (soc_property_iterator >= 0)
        {
            SHR_FREE(ctest_soc_property[soc_property_iterator].property);
            SHR_FREE(ctest_soc_property[soc_property_iterator].value);
            soc_property_iterator--;
        }
        SHR_FREE(ctest_soc_property);
        /** Restore SOC properties */
        if (ctest_soc_set_h != NULL)
        {
            ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
        }
    }

exit:
#if !defined(ADAPTER_SERVER_MODE)
    /** Free the packet header handle */
    diag_sand_packet_free(unit, packet_h);
#endif
    /** Free the SOC properties structure */
    if (ctest_soc_property != NULL)
    {
        while (soc_property_iterator >= 0)
        {
            SHR_FREE(ctest_soc_property[soc_property_iterator].property);
            SHR_FREE(ctest_soc_property[soc_property_iterator].value);
            soc_property_iterator--;
        }
    }
    SHR_FREE(ctest_soc_property);
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_recycle_bandwidth_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx = 0;
    int nof_configurations = 0;
    int current_configuration = 0;
    int max_rcy_rate_mbps[5] = { 0 };
    int segment_in_bits = 0;
    int rate_kbps = 100000000; /** 100 Gbit/s in Kbit/s */
    int priorities = 0;
    int rcy_ports_nof_existing = 0;
    int rcy_ports_nof_needed = 0;
    int rcy_index = 0;
    int rcy_if = 0;
    int rcy_if_rate_mbps = 0;
    int rcy_port_first = 50;
    bcm_port_t rcy_port = 0;
    bcm_pbmp_t rcy_ports;
    bcm_core_t core_id = 0;
    int soc_property_iterator = 0;
    char string_temp1[STR_OVERKILL_SIZE] = { 0 };
    char string_temp2[STR_OVERKILL_SIZE] = { 0 };
    rhhandle_t ctest_soc_set_h = NULL;
    ctest_soc_property_t *ctest_soc_property = NULL;
    bcm_port_t eth_port = 1;
    bcm_pbmp_t nif_ports;
    bcm_port_t nif_port_i;
    int nif_ports_nof_existing = 0;
    bcm_gport_t interface_gport = 0;
    bcm_gport_t local_gport = 0;
    bcm_gport_t tc_gport = 0;
    uint32 pcp = 0;
    uint32 flags;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
#if !defined(ADAPTER_SERVER_MODE)
    rhhandle_t packet_h = NULL;
    char *dmac_str = "00:00:00:00:00:01";
    char *smac_str = "00:66:77:88:99:aa";
    uint32 vlan = 100;
    uint32 cfi = 0;
    int packet_size_bytes = 2000;
    int stream_duration = 10;
    int type = BCM_SAT_GTF_RATE_IN_BYTES;
    int stream_burst = 0;
    int stream_granularity = 20000000;
    uint64 bytes_count;
    uint64 expected_packets;
    uint64 expected_packets_low;
    uint64 recieved_packets;
#endif
    SHR_FUNC_INIT_VARS(unit)
    /** Check if the current device supports only one RCY max rate configuration or several */
        nof_configurations = 0;
    for (idx = 0; idx < dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, 0)->if_id ==
            dnx_data_egr_queuing.params.egr_if_rcy_get(unit))
        {
            if (dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, 0)->speed == -1)
            {
                /** There is only one max RCY rate configuration, calculate it */
                nof_configurations = 1;
                /** To determine the max possible RCY BW for the current device, the bus segment size that the RCY interface
                 * is utilizing is used in the calculation. The RCY interface can utilize half pipe. */
                segment_in_bits = dnx_data_device.general.bus_size_in_bits_get(unit) / 2;
                max_rcy_rate_mbps[0] = (dnx_data_device.general.core_clock_khz_get(unit) * segment_in_bits) / 1000;
                break;
            }
            else
            {
                /** There are several RCY rate configurations, get the speed for all of them */
                while (dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, 0)->if_id ==
                       dnx_data_egr_queuing.params.egr_if_rcy_get(unit))
                {
                    max_rcy_rate_mbps[nof_configurations] =
                        dnx_data_egr_queuing.params.if_speed_params_get(unit, idx, 0)->speed;
                    nof_configurations++;
                    idx++;
                }
                break;
            }
        }
    }

    /** Test each possible configuration for max RCY bandwidth */
    for (current_configuration = 0; current_configuration < nof_configurations; current_configuration++)
    {
        /** Calculate how many RCY ports of 100G are needed to reach max bandwidth */
        rcy_ports_nof_needed = max_rcy_rate_mbps[current_configuration] / (rate_kbps / 1000);

        /**  Iterate over the cores, run the test one time for each core */
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
        {
            /**
             * Get all RCY and NIF ports and remove them using SOC properties
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_RCY, 0, &rcy_ports));
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &nif_ports));
            BCM_PBMP_COUNT(rcy_ports, rcy_ports_nof_existing);
            BCM_PBMP_COUNT(nif_ports, nif_ports_nof_existing);

            SHR_ALLOC_SET_ZERO(ctest_soc_property,
                               sizeof(ctest_soc_property_t) * (rcy_ports_nof_existing + rcy_ports_nof_needed +
                                                               nif_ports_nof_existing + 6), "ctest_soc_property",
                               "%s%s%s", EMPTY, EMPTY, EMPTY);

            /** Remove all RCY ports using SOC property */
            soc_property_iterator = 0;
            BCM_PBMP_ITER(rcy_ports, rcy_port)
            {
                /** Allocate the SOC properties property name, no need to allocate value, must be NULL to delete  */
                SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property,
                                   sizeof(char) * (RHNAME_MAX_SIZE + 1), "ctest_soc_property.property", "%s%s%s", EMPTY,
                                   EMPTY, EMPTY);
                /** Set the SOC property string */
                sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "ucode_port_%d*", rcy_port);
                sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
                /** Value must be NULL to delete */
                ctest_soc_property[soc_property_iterator].value = NULL;
                soc_property_iterator++;
            }

            /** Remove all NIF ports using SOC property */
            BCM_PBMP_ITER(nif_ports, nif_port_i)
            {
                /** Allocate the SOC properties property name, no need to allocate value, must be NULL to delete  */
                SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property,
                                   sizeof(char) * (RHNAME_MAX_SIZE + 1), "ctest_soc_property.property", "%s%s%s", EMPTY,
                                   EMPTY, EMPTY);
                /** Set the SOC property string */
                sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "ucode_port_%d.*", nif_port_i);
                sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
                /** Value must be NULL to delete */
                ctest_soc_property[soc_property_iterator].value = NULL;
                soc_property_iterator++;
            }

            /** Add the needed RCY ports for the test */
            for (rcy_index = 0; rcy_index < rcy_ports_nof_needed; rcy_index++)
            {
                /** Rcy index will be used as a channel. Calculate the logical port number*/
                rcy_port = rcy_index + rcy_port_first;
                /** Calculate the RCY interface number. If the device supports 2 Rcy interfaces, make sure to create ports on both */
                rcy_if = (dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit) > 1) ? (rcy_index % 2) : 0;
                /** Allocate the SOC properties structure */
                SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property,
                                   sizeof(char) * (RHNAME_MAX_SIZE + 1), "ctest_soc_property.property", "%s%s%s", EMPTY,
                                   EMPTY, EMPTY);
                SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value,
                                   sizeof(char) * (RHNAME_MAX_SIZE + 1), "ctest_soc_property.value", "%s%s%s", EMPTY,
                                   EMPTY, EMPTY);
                /** Set the SOC property strings */
                sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "ucode_port_%d", rcy_port);
                sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "RCY%d.%d:core_%d.%d", rcy_if, rcy_index, core_id,
                             rcy_port);
                sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
                sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
                soc_property_iterator++;
            }

            /** Set the RCY total rate per IF SOC property in Mbits/s*/
            rcy_if_rate_mbps =
                max_rcy_rate_mbps[current_configuration] / dnx_data_egr_queuing.params.nof_egr_rcy_interfaces_get(unit);
            /** Allocate the SOC properties structure */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
            /** Set the SOC property strings */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "port_init_speed_rcy");
            sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "%d", rcy_if_rate_mbps);
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
            soc_property_iterator++;

            /** Add the needed NIF ETH 100G port - Allocate the SOC properties structure */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property,
                               sizeof(char) * (RHNAME_MAX_SIZE + 1), "ctest_soc_property.property", "%s%s%s", EMPTY,
                               EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value,
                               sizeof(char) * (RHNAME_MAX_SIZE + 1), "ctest_soc_property.value", "%s%s%s", EMPTY,
                               EMPTY, EMPTY);
            /** Set the SOC property strings */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "ucode_port_%d", eth_port);
            sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "CGE%d:core_0.%d", eth_port, eth_port);
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
            soc_property_iterator++;

            /** Set the NIF ETH rate per IF SOC property in Mbits/s - Allocate the SOC properties structure */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
            /** Set the SOC property strings */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "port_init_speed_%d", eth_port);
            sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "%d", (rate_kbps / 1000));
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
            soc_property_iterator++;

            /** Set the SAT total rate with SOC property to 100G in Mbits/s*/
            /** Allocate the SOC properties structure */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
            /** Set the SOC property strings */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "port_init_speed_sat");
            sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "%d", (rate_kbps / 1000));
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
            soc_property_iterator++;

            /** Set the Jumbo packet size of the MIB counters, so that they will be able to count large packets */
            /** Allocate the SOC properties structure */
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].property, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.property", "%s%s%s", EMPTY, EMPTY, EMPTY);
            SHR_ALLOC_SET_ZERO(ctest_soc_property[soc_property_iterator].value, sizeof(char) * (RHNAME_MAX_SIZE + 1),
                               "ctest_soc_property.value", "%s%s%s", EMPTY, EMPTY, EMPTY);
            /** Set the SOC property strings */
            sal_snprintf(string_temp1, RHNAME_MAX_SIZE, "bcm_stat_jumbo");
            sal_snprintf(string_temp2, RHNAME_MAX_SIZE, "%d", 2048);
            sal_strncpy(ctest_soc_property[soc_property_iterator].property, string_temp1, RHNAME_MAX_SIZE);
            sal_strncpy(ctest_soc_property[soc_property_iterator].value, string_temp2, RHNAME_MAX_SIZE);
            soc_property_iterator++;

            /** Last member must be NULL */
            ctest_soc_property[soc_property_iterator].property = NULL;
            ctest_soc_property[soc_property_iterator].value = NULL;

            SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

            /** Get port info to use the core for SAT configurations */
            SHR_IF_ERR_EXIT(bcm_port_get(unit, eth_port, &flags, &interface_info, &mapping_info));

            /** Disable learning on the NIF ETH port */
            SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, eth_port, BCM_PORT_LEARN_FWD));

            /** Forward traffic from the NIF ETH to the first RCY port */
            SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, eth_port, rcy_port_first, 1));

            /** Set the interface shaper for the RCY ports. Need to call the API only for the first RCY port, as
             * they are represented by a single interface in the egress shapers */
            BCM_GPORT_LOCAL_SET(local_gport, rcy_port_first);
            SHR_IF_ERR_EXIT(bcm_fabric_port_get(unit, local_gport, 0, &interface_gport));
            SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set
                            (unit, interface_gport, 0, 0, (rate_kbps * rcy_ports_nof_needed), 0));

            for (rcy_index = 0; rcy_index < rcy_ports_nof_needed; rcy_index++)
            {
                /** Calculate the logical port number*/
                rcy_port = rcy_index + rcy_port_first;
                /** Disable learning on the current RCY port */
                SHR_IF_ERR_EXIT(bcm_port_learn_set(unit, rcy_port, BCM_PORT_LEARN_FWD));

                if (rcy_index == (rcy_ports_nof_needed - 1))
                {
                    /** The last RCY port must return traffic to the NIF ETH port */
                    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, rcy_port, eth_port, 1));
                }
                else
                {
                    /** Set force-forward to the next RCY port */
                    SHR_IF_ERR_EXIT(bcm_port_force_forward_set(unit, rcy_port, (rcy_port + 1), 1));
                }

                /** Use also the same loop to set the port and TC shapers for the RCY ports */
                BCM_GPORT_LOCAL_SET(local_gport, rcy_port);
                SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, rate_kbps, 0));
                SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, rcy_port, &priorities));
                for (pcp = 0; pcp < priorities; pcp++)
                {
                    BCM_COSQ_GPORT_PORT_TC_SET(tc_gport, rcy_port);
                    SHR_IF_ERR_EXIT(bcm_cosq_gport_bandwidth_set(unit, tc_gport, pcp, 0, rate_kbps, 0));
                }
            }

            LOG_CLI(("Testing %d Recycle ports with total RCY rate of %d Mbit/s...\n", rcy_ports_nof_needed,
                     max_rcy_rate_mbps[current_configuration]));
#if !defined(ADAPTER_SERVER_MODE)
            /** Calculate the SAT burst size based on the device and required rate */
            stream_burst = ((rate_kbps * 1000) / 8) / dnx_data_device.general.core_clock_khz_get(unit);

            /** Allocate a handle for the packet */
            SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));

            /** Create a stream for the current PCP and set it to 10G rate for 2 seconds */
            pcp = 0;
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32
                            (unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", dmac_str));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", smac_str));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", &vlan, 12));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.PCP", &pcp, 3));
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.DEI", &cfi, 1));
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_create
                            (unit, mapping_info.core, packet_h, packet_size_bytes, stream_duration, type, rate_kbps,
                             stream_burst, stream_granularity));

            /** Clear stats on port */
            SHR_IF_ERR_EXIT(bcm_stat_clear(unit, eth_port));

            /** Send stream */
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_send(unit, &packet_h, 1));
            sal_sleep(2);

            /** Get TX statistics from the port - how many packets exited through it */
            COMPILER_64_ZERO(recieved_packets);
            SHR_IF_ERR_EXIT(bcm_stat_sync(unit));
            SHR_IF_ERR_EXIT(bcm_stat_get(unit, eth_port, snmpEtherStatsTXNoErrors, &recieved_packets));

            /** Calculate the expected packets */
            COMPILER_64_ZERO(expected_packets);
            COMPILER_64_ZERO(expected_packets_low);
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_get_stats(unit, packet_h, &bytes_count, &expected_packets));
            COMPILER_64_SET(expected_packets_low, COMPILER_64_HI(expected_packets), COMPILER_64_LO(expected_packets));
            COMPILER_64_UMUL_32(expected_packets_low, 99);
            COMPILER_64_UDIV_32(expected_packets_low, 100);

            /** Make sure all packets are passing */
            if ((COMPILER_64_LO(recieved_packets) > COMPILER_64_LO(expected_packets)) ||
                (COMPILER_64_LO(recieved_packets) < COMPILER_64_LO(expected_packets_low)))
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "FAILURE - Not all packets passed: got %d, expected %d!\n",
                             COMPILER_64_LO(recieved_packets), COMPILER_64_LO(expected_packets));
            }

            LOG_CLI(("Testing passed: got %d, expected %d!\n",
                     COMPILER_64_LO(recieved_packets), COMPILER_64_LO(expected_packets)));

            /** Destroy streams and free handle */
            SHR_IF_ERR_EXIT(diag_sand_packet_stream_destroy(unit, packet_h));
            diag_sand_packet_free(unit, packet_h);
#endif
            /** Free the SOC properties structure */
            while (soc_property_iterator >= 0)
            {
                SHR_FREE(ctest_soc_property[soc_property_iterator].property);
                SHR_FREE(ctest_soc_property[soc_property_iterator].value);
                soc_property_iterator--;
            }
            SHR_FREE(ctest_soc_property);
            /** Restore SOC properties */
            if (ctest_soc_set_h != NULL)
            {
                ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
            }
        }
    }
exit:
    /** Free the SOC properties structure */
    if (ctest_soc_property != NULL)
    {
        while (soc_property_iterator >= 0)
        {
            SHR_FREE(ctest_soc_property[soc_property_iterator].property);
            SHR_FREE(ctest_soc_property[soc_property_iterator].value);
            soc_property_iterator--;
        }
    }
    SHR_FREE(ctest_soc_property);
    SHR_FUNC_EXIT;
}

/********************
 * GLOBAL VARIABLES *
 ********************/
/************************************************************
 * This is the entry point for DNX C-Tests for Recycle port *
 ************************************************************/
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnx_recycle_test_cmds[] = {
    /** String     Main function                  */
    {"priorities", sh_dnx_recycle_priorities_cmd, NULL, sh_dnx_recycle_priorities_options, &sh_dnx_recycle_priorities_man, NULL, sh_dnx_recycle_priorities_tests, CTEST_POSTCOMMIT * SH_CMD_CONDITIONAL, sh_dnx_is_sat_available},
    {"bandwidth",  sh_dnx_recycle_bandwidth_cmd,  NULL, NULL,                              &sh_dnx_recycle_bandwidth_man,  NULL, NULL,                            CTEST_POSTCOMMIT * SH_CMD_CONDITIONAL, sh_dnx_is_sat_available},
    {NULL}
};
/* *INDENT-ON* */
